/* icmp.c - icmp_init, icmp_in, icmp_register, icmp_recv, icmp_release	*/

#include <xinu.h>

struct	icmpentry icmptab[ICMP_SLOTS];	/* table of processes using ping*/
struct	icoutq    Icmpout;		/* queue of outgoing packets	*/
sid32	icmpmutex;			/* mutual exclusion semaphore	*/

/*------------------------------------------------------------------------
 * icmp_init - initialize icmp table
 *------------------------------------------------------------------------
 */
void	icmp_init(void) {

	int32	i;			/* table index */

	for(i=0; i<ICMP_SLOTS; i++) {
		icmptab[i].icstate = ICMP_FREE;
	}
	icmpmutex = semcreate(1);
	Icmpout.icosem = semcreate(0);		/* output semaphore */
	resume(create(icmp_out, 8192, 200, "icmp output", 0));
	return;
}

/*------------------------------------------------------------------------
 * icmp_in - handle an incoming icmp packet
 *------------------------------------------------------------------------
 */
void	icmp_in(void) {			/* currpkt points to the packet	*/

	int32	slot;			/* index into icmptab		*/
	struct	icmpentry *icmptr;	/* pointer to icmptab entry	*/

	/* Discard all ICMP messages except ping */

	if ( (currpkt->net_ictype != ICMP_ECHOREPLY) &&
	     (currpkt->net_ictype != ICMP_ECHOREQST)  )	{
		return;
	}

	/* Handle echo request message */

	if (currpkt->net_ictype == ICMP_ECHOREQST) {

	/* send echo reply message */

		icmp_send(currpkt->net_ipsrc, ICMP_ECHOREPLY,
			currpkt->net_icident, currpkt->net_icseq,
			(char *) &currpkt->net_icdata,
			currpkt->net_iplen-IP_HDR_LEN-ICMP_HDR_LEN);
		return;
	}

	/* Handle Echo Reply message: verify that slot number is valid */

	slot = ntohs(currpkt->net_icident);
	if ( (slot < 0) || (slot >= ICMP_SLOTS) ) {
		return;		/* discard bad code */
	}

	/* Verify that slot in table is in use and IP address	*/
	/*    in incomming packet matches IP address in table	*/

	icmptr = &icmptab[slot];
	if (   (icmptr->icstate == ICMP_FREE)
	    || (currpkt->net_ipsrc != icmptr->icremip) ) {
		return;			/* discard packet */
	}

	/* Add packet to queue */

	icmptr->iccount++;
	icmptr->icqueue[icmptr->ictail++] = currpkt;
	if (icmptr->ictail >= ICMP_QSIZ) {
		icmptr->ictail = 0;
	}
	currpkt = (struct netpacket *)getbuf(netbufpool);
	if (icmptr->icstate == ICMP_RECV) {
		icmptr->icstate = ICMP_USED;
		send (icmptr->icpid, OK);
	}
	return;
}

/*------------------------------------------------------------------------
 * icmp_register - register a remote IP address for ping replies
 *------------------------------------------------------------------------
 */
int32	icmp_register (
	 uint32	remip			/* remote IP address		*/
	)
{
	int32	i;			/* index into icmptab		*/
	int32	slot;			/* index of slot to use		*/
	struct	icmpentry *icmptr;	/* pointer to icmptab entry	*/

	wait(icmpmutex);
	slot = -1;
	for (i=0; i<ICMP_SLOTS; i++) {
		icmptr = &icmptab[i];
		if (icmptr->icstate == ICMP_FREE) {
			if (slot == -1) {
				slot = i;
			}
		} else if (icmptr->icremip == remip) {
			signal(icmpmutex);
			return SYSERR;	/* already registered */
		}
	}
	if (slot == -1) {
		signal(icmpmutex);
		return SYSERR;		/* no free entries */
	}

	/* Fill in table entry */

	icmptr = &icmptab[slot];
	icmptr->icstate = ICMP_USED;
	icmptr->icremip = remip;
	icmptr->iccount = 0;
	icmptr->ichead = icmptr->ictail = 0;
	icmptr->icpid = -1;
	signal(icmpmutex);
	return slot;
}

/*------------------------------------------------------------------------
 * icmp_recv - receive an icmp echo reply packet
 *------------------------------------------------------------------------
 */
int32	icmp_recv (
	 int32	slot,			/* ICMP identifier		*/
	 char   *buff,			/* buffer to ICMP data		*/
	 int32	len,			/* length of buffer		*/
	 uint32	timeout			/* time to wait in msec		*/
	)
{
	intmask	mask;			/* interrupt mask		*/
	struct	icmpentry *icmptr;	/* pointer to icmptab entry	*/
	umsg32	msg;			/* message from recvtime()	*/
	struct	netpacket *pkt;		/* ptr to packet being read	*/
	int32	datalen;		/* length of ICMP data area	*/
	char	*icdataptr;		/* pointer to icmp data		*/
	int32	i;			/* counter for data copy	*/

	if ( (slot < 0) || (slot >= ICMP_SLOTS) ) {
		return SYSERR;
	}
	icmptr = &icmptab[slot];
	mask = disable();
	if (icmptr->icstate != ICMP_USED) {
		restore(mask);
		return SYSERR;
	}

	if (icmptr->iccount == 0) {	/* no packet is waiting */
		icmptr->icstate = ICMP_RECV;
		icmptr->icpid = currpid;
		msg = recvclr();
		msg = recvtime(timeout);	/* wait for packet */
		icmptr->icstate = ICMP_USED;
		if (msg == TIMEOUT) {
			restore(mask);
			return TIMEOUT;
		} else if (msg != OK) {
			restore(mask);
			return SYSERR;
		}
	}

	/* packet has arrived -- dequeue it */

	pkt = icmptr->icqueue[icmptr->ichead++];
	if (icmptr->ichead >= ICMP_SLOTS) {
		icmptr->ichead = 0;
	}
	icmptr->iccount--;

	/* copy icmp data from packet into caller's buffer */

	datalen = pkt->net_iplen - IP_HDR_LEN - ICMP_HDR_LEN;
	icdataptr = (char *) &pkt->net_icdata;
	for (i=0; i<datalen; i++) {
		if (i >= len) {
			break;
		}
		*buff++ = *icdataptr++;
	}
	freebuf((char *)pkt);
	restore(mask);
	return i;
}

/*------------------------------------------------------------------------
 * icmp_send - send an icmp packet
 *------------------------------------------------------------------------
 */
status	icmp_send (
	 uint32	remip,			/* remote IP address to use	*/
	 uint16	type,			/* ICMP type (req. or reply)	*/
	 uint16	ident,			/* ICMP identifier value	*/
	 uint16	seq,			/* ICMP sequence number		*/
	 char	*buf,			/* ptr to data buffer		*/
	 int32	len			/* length of data in buffer	*/
	)
{
	struct	netpacket *pkt;		/* local packet buffer		*/
	int32	pktlen;			/* total packet length		*/
	static	uint16 ipident = 32767;	/* datagram IDENT field		*/

	/* Allocate packet */

	pkt = (struct netpacket *)getbuf(netbufpool);

	/* compute packet length as icmp data size + fixed header size	*/

	pktlen = ((char *) &pkt->net_icdata - (char *) pkt) + len;

	/* create icmp packet in pkt */

	memcpy(pkt->net_ethsrc, NetData.ethaddr, ETH_ADDR_LEN);
        pkt->net_ethtype = 0x800;	/* Type is IP */
	pkt->net_ipvh = 0x45;		/* IP version and hdr length	*/
	pkt->net_iptos = 0x00;		/* Type of service		*/
	pkt->net_iplen= pktlen - ETH_HDR_LEN;/* total IP datagram length*/
	pkt->net_ipid = ipident++;	/* datagram gets next IDENT	*/
	pkt->net_ipfrag = 0x0000;	/* IP flags & fragment offset	*/
	pkt->net_ipttl = 0xff;		/* IP time-to-live		*/
	pkt->net_ipproto = IP_ICMP;	/* datagram carries icmp	*/
	pkt->net_ipcksum = 0x0000;	/* initial checksum		*/
	pkt->net_ipsrc = NetData.ipaddr;/* IP source address		*/
	pkt->net_ipdst = remip;		/* IP destination address	*/

	/* compute IP header checksum */
	pkt->net_ipcksum = 0xffff & ipcksum(pkt);

	pkt->net_ictype = type;		/* ICMP type			*/
	pkt->net_iccode = 0;		/* code is zero for ping	*/
	pkt->net_iccksum = 0x0000;	/* temporarily zero the cksum	*/
	pkt->net_icident = ident;	/* ICMP identification		*/
	pkt->net_icseq = seq;		/* ICMP sequence number		*/
	memcpy(pkt->net_icdata, buf, len);

	/* compute ICMP checksum */

	pkt->net_iccksum = 0xffff & icmp_cksum((char *)&pkt->net_ictype,
				len+ICMP_HDR_LEN);

	/* Enqueue packet for transmission */

	Icmpout.icoutq[Icmpout.icotail++] = pkt;
	if (Icmpout.icotail >= ICMP_OQSIZ) {
		Icmpout.icotail = 0;
	}

	signal(Icmpout.icosem);
	return OK;
}

/*------------------------------------------------------------------------
 * icmp_out - process to send ICMP output packets synchronously
 *------------------------------------------------------------------------
 */
process	icmp_out(void) {

	struct	netpacket *pkt;		/* packet to send		*/
	uint32	ipaddr;			/* IP address			*/
	byte	ethbcast[] = {0xff,0xff,0xff,0xff,0xff,0xff};
	int32	pktlen;			/* length of IP packet		*/

	while(TRUE) {

		/* Wait for packet in queue */

		wait(Icmpout.icosem);
		pkt = Icmpout.icoutq[Icmpout.icohead++];
		if (Icmpout.icohead >= ICMP_OQSIZ) {
			Icmpout.icohead = 0;
		}

		/* Get destination address from packet */

		ipaddr = pkt->net_ipdst;

		/* Handle broadcast */

		if (ipaddr == IP_BCAST) {
			memcpy(pkt->net_ethdst, ethbcast, ETH_ADDR_LEN);
			pktlen = pkt->net_iplen + ETH_HDR_LEN;
			write(ETHER0, (char *)pkt, pktlen);
			freebuf((char *)pkt);
			continue;
		}

		/* If not on local net, use router address */

		if (	(NetData.ipaddr & NetData.addrmask)
		     != (ipaddr & NetData.addrmask)  ) {
			ipaddr = NetData.routeraddr;
		}

		if (arp_resolve(ipaddr, pkt->net_ethdst) != OK) {
			kprintf("icmp_out: cannot resolve %08x\n\r",
					ipaddr);
			freebuf( (char *)pkt );
			continue;
		}
		pktlen = pkt->net_iplen + ETH_HDR_LEN;
		write(ETHER0, (char *)pkt, pktlen);
		freebuf( (char *)pkt );
	}
}

/*------------------------------------------------------------------------
 * icmp_release - release a previously-registered remote IP, remote
 *			port, and local port (exact match required)
 *------------------------------------------------------------------------
 */
status	icmp_release (
	 int32	slot			/* slot in icmptab to release	*/
	)
{
	struct	icmpentry *icmptr;	/* ptr to icmptab entry		*/
	struct	netpacket *pkt;		/* ptr to packet		*/

	wait(icmpmutex);

	/* check arg and insure entry in table is in use */

	if ( (slot < 0) || (slot >= ICMP_SLOTS) ) {
		signal(icmpmutex);
		return SYSERR;
	}
	icmptr = &icmptab[slot];
	if (icmptr->icstate != ICMP_USED) {
		signal(icmpmutex);
		return SYSERR;
	}

	/* Remove each packet from the queue and free the buffer */

	sched_cntl(DEFER_START);
	while (icmptr->iccount > 0) {
		pkt = icmptr->icqueue[icmptr->ichead++];
		if (icmptr->ichead >= ICMP_SLOTS) {
			icmptr->ichead = 0;

		}
		freebuf((char *)pkt);
		icmptr->iccount--;
	}

	/* mark the entry free */

	icmptr->icstate = ICMP_FREE;
	sched_cntl(DEFER_STOP);
	signal(icmpmutex);
	return OK;
}

/*------------------------------------------------------------------------
 * icmp_cksum - compute a checksum for a specified set of data bytes
 *------------------------------------------------------------------------
 */
uint16	icmp_cksum (
	 char	*buf,			/* buffer of items for checksum	*/
	 int32	buflen			/* size of buffer in bytes	*/
	)
{
	int32	scount;			/* number of 16-bit values buf	*/
	uint32	cksum;			/* checksum being computed	*/
	uint16	*sptr;			/* walks along buffer		*/

	/* walk along buffer and sum all 16-bit values */

	scount = buflen >> 1;		/* divide by 2 and round down	*/
	sptr = (uint16 *)buf;
	cksum = 0;
	for (; scount > 0; scount--) {
		cksum += (uint32) *sptr++;
	}

	/* if buffer lenght is odd, add last byte */

	if ( (buflen & 0x01) !=0 ) {
		cksum += (uint32) (*((char *)sptr) << 8);
	}
	cksum += (cksum >> 16);
        cksum = 0xffff & ~cksum;
        return (uint16) (0xffff & cksum);
}
