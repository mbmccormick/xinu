/* arp.c - arp_init, arp_resolve, arp_in, arp_alloc */

#include <xinu.h>

struct	arpentry  arpcache[ARP_SIZ];	/* ARP cache			*/

/*------------------------------------------------------------------------
 * arp_init - initialize ARP mutex and cache
 *------------------------------------------------------------------------
 */
void	arp_init(void) {

	int32	i;			/* ARP cache index		*/

	for (i=1; i<ARP_SIZ; i++) {	/* initialize cache to empty	*/
		arpcache[i].arstate = AR_FREE;
	}
}

/*------------------------------------------------------------------------
 * arp_resolve - use ARP to resolve an IP address into an Ethernet address
 *------------------------------------------------------------------------
 */
status	arp_resolve (
	 uint32	ipaddr,			/* IP address to resolve	*/
	 byte	mac[ETH_ADDR_LEN]	/* array into which Ethernet	*/
	)				/*  address should be placed	*/
{
	intmask	mask;			/* saved interrupt mask		*/
	struct	arppacket apkt;		/* local packet buffer		*/
	int32	i;			/* index into arpcache		*/
	int32	slot;			/* ARP table slot to use	*/
	struct	arpentry  *arptr;	/* ptr to ARP cache entry	*/
	int32	msg;			/* message returned by recvtime	*/
	byte	ethbcast[] = {0xff,0xff,0xff,0xff,0xff,0xff};

	if (ipaddr == IP_BCAST) {	/* set mac address to b-cast	*/
		memcpy(mac, ethbcast, ETH_ADDR_LEN);
		return OK;
	}

	/* Insure only one process uses ARP at a time */

	mask = disable();

	for (i=0; i<ARP_SIZ; i++) {
		arptr = &arpcache[i];
		if (arptr->arstate == AR_FREE) {
			continue;
		}
		if (arptr->arpaddr == ipaddr) { /* adddress is in cache	*/
			break;
		}
	}

	if (i < ARP_SIZ) {	/* entry was found */

		/* Only one request can be pending for an address */

		if (arptr->arstate == AR_PENDING) {
			restore(mask);
			return SYSERR;
		}

		/* Entry is resolved - handle and return */

		memcpy(mac, arptr->arhaddr, ARP_HALEN);
		restore(mask);
		return OK;
	}

	/* Must allocate a new cache entry for the request */

	slot = arp_alloc();
	if (slot == SYSERR) {
		restore(mask);
		return SYSERR;
	}
	arptr = &arpcache[slot];
	arptr->arstate = AR_PENDING;
	arptr->arpaddr = ipaddr;
	arptr->arpid = currpid;

	/* Release ARP cache for others */

	restore(mask);

	/* Hand-craft an ARP Request packet */

	memcpy(apkt.arp_ethdst, ethbcast, ETH_ADDR_LEN);
	memcpy(apkt.arp_ethsrc, NetData.ethaddr, ETH_ADDR_LEN);
	apkt.arp_ethtype = ETH_ARP;	  /* Packet type is ARP		*/
	apkt.arp_htype = ARP_HTYPE;	  /* Hardware type is Ethernet	*/
	apkt.arp_ptype = ARP_PTYPE;	  /* Protocol type is IP	*/
	apkt.arp_hlen = 0xff & ARP_HALEN; /* Ethernet MAC size in bytes	*/
	apkt.arp_plen = 0xff & ARP_PALEN; /* IP address size in bytes	*/
	apkt.arp_op = 0xffff & ARP_OP_REQ;/* ARP type is Request	*/
	memcpy(apkt.arp_sndha, NetData.ethaddr, ARP_HALEN);
	apkt.arp_sndpa = NetData.ipaddr;  /* Local IP address		*/
	memset(apkt.arp_tarha, '\0', ARP_HALEN); /* Target HA is unknown*/
	apkt.arp_tarpa = ipaddr;	  /* Target protocol address	*/

	/* Send the packet ARP_RETRY times and await response*/

	msg = recvclr();
	for (i=0; i<ARP_RETRY; i++) {
		write(ETHER0, (char *)&apkt, sizeof(struct arppacket));
		msg = recvtime(ARP_TIMEOUT);
		if (msg == TIMEOUT) {
			continue;
		} else if (msg == SYSERR) {
			return SYSERR;
		} else {	/* entry is resolved */
			break;
 		}
	}

	/* Obtain exclusive use of the ARP cache again */

	mask = disable();

	/* Verify that entry has not changed */

	if (arptr->arpaddr != ipaddr) {
		restore(mask);
		return SYSERR;
	}

	/* Either return hardware address or TIMEOUT indicator */

	if (i < ARP_RETRY) {
		memcpy(mac, arptr->arhaddr, ARP_HALEN);
		restore(mask);
		return OK;
	} else {
		arptr->arstate = AR_FREE;   /* invalidate cache entry */
		restore(mask);
		return TIMEOUT;
	}
}


/*------------------------------------------------------------------------
 * arp_in - handle an incoming ARP packet
 *------------------------------------------------------------------------
 */
void	arp_in (void) {			/* currpkt points to the packet */

	intmask	mask;			/* saved interrupt mask		*/
	struct	arppacket *pktptr;	/* ptr to incoming packet	*/
	struct	arppacket apkt;		/* Local packet buffer		*/
	int32	slot;			/* slot in cache		*/
	struct	arpentry  *arptr;	/* ptr to ARP cache entry	*/
	bool8	found;			/* is the sender's address in	*/
					/*   the cache?			*/

	pktptr = (struct arppacket *)currpkt;

	/* Verify ARP is for IPv4 and Ethernet */

	if ( (pktptr->arp_htype != ARP_HTYPE) ||
	     (pktptr->arp_ptype != ARP_PTYPE) ) {
		return;
	}

	/* Insure only one process uses ARP at a time */

	mask = disable();

	/* Search cache for sender's IP address */

	found = FALSE;

	for (slot=0; slot < ARP_SIZ; slot++) {
		arptr = &arpcache[slot];

		/* Ignore unless entry valid and address matches */

		if ( (arptr->arstate != AR_FREE) &&
		     (arptr->arpaddr == pktptr->arp_sndpa) ) {
			found = TRUE;
			break;
		}
	}

	if (found) { 	/* Update sender's hardware address */

		memcpy(arptr->arhaddr, pktptr->arp_sndha, ARP_HALEN);

		/* Handle entry that was pending */

		if (arptr->arstate == AR_PENDING) {
			arptr->arstate = AR_RESOLVED;

			/* Notify waiting process */

			send(arptr->arpid, OK);
		}
	}

	/* For an ARP reply, processing is complete */

	if (pktptr->arp_op == ARP_OP_RPLY) {
		restore(mask);
		return;
	}

	/* ARP request packet: if local machine is not	the target,	*/
	/*	processing is complete					*/

	if ((!NetData.ipvalid) || (pktptr->arp_tarpa!=NetData.ipaddr)) {
		restore(mask);
		return;
	}

	/* Request has been sent to local machine: add sender's info	*/
	/*	to cache, if not already present			*/

	if (!found) {
		slot = arp_alloc();
		if (slot == SYSERR) {	/* cache overflow */
			restore(mask);
			return;
		}
		arptr = &arpcache[slot];
		arptr->arstate = AR_RESOLVED;
		arptr->arpaddr = pktptr->arp_sndpa;
		memcpy(arptr->arhaddr, pktptr->arp_sndha, ARP_HALEN);
	}

	/* Release ARP cache for others */
	
	restore(mask);

	/* Hand-craft an ARP reply packet and send */

	memcpy(apkt.arp_ethdst, pktptr->arp_sndha, ARP_HALEN);
	memcpy(apkt.arp_ethsrc, NetData.ethaddr, ARP_HALEN);
	apkt.arp_ethtype= ETH_ARP;	/* Frame carries ARP	*/
	apkt.arp_htype	= ARP_HTYPE;	/* Hardware is Ethernet	*/
	apkt.arp_ptype	= ARP_PTYPE;	/* Protocol is IP	*/
	apkt.arp_hlen	= ARP_HALEN;	/* Ethernet address size*/
	apkt.arp_plen	= ARP_PALEN;	/* IP address size	*/
	apkt.arp_op	= ARP_OP_RPLY;	/* Type is Reply	*/

	/* Insert local Ethernet and IP address in sender fields */

	memcpy(apkt.arp_sndha, NetData.ethaddr, ARP_HALEN);
	apkt.arp_sndpa = NetData.ipaddr;

	/* Copy target Ethernet and IP addresses from request packet */

	memcpy(apkt.arp_tarha, pktptr->arp_sndha, ARP_HALEN);
	apkt.arp_tarpa = pktptr->arp_sndpa;

	/* Send the reply */

	write(ETHER0, (char *)&apkt, sizeof(struct  arppacket));
	return;
}

/*------------------------------------------------------------------------
 * arp_alloc - find a free slot or kick out an entry to create one
 *------------------------------------------------------------------------
 */
int32	arp_alloc (void) {

	static	int32	nextslot = 0;	/* next slot to try		*/
	int32	i;			/* counts slots in the table	*/
	int32	slot;			/* slot that is selected	*/

	/* Search for free slot starting at nextslot */

	for (i=0; i < ARP_SIZ; i++) {
		slot = nextslot++;
		if (nextslot >= ARP_SIZ) {
			nextslot = 0;
		}
		if (arpcache[slot].arstate == AR_FREE) {
			return slot;
		}
	}

	/* Search for resolved entry */

	for (i=0; i < ARP_SIZ; i++) {
		slot = nextslot++;
		if (nextslot >= ARP_SIZ) {
			nextslot = 0;
		}
		if (arpcache[slot].arstate == AR_RESOLVED) {
			return slot;
		}
	}

	/* All slots are pending */

	kprintf("ARP cache size exceeded\n\r");

	return SYSERR;
}
