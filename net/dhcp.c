/* dhcp.c - getlocalip */

#include <xinu.h>

/*------------------------------------------------------------------------
 * getlocalip - use DHCP to obtain an IP address
 *------------------------------------------------------------------------
 */
uint32	getlocalip(void)
{
	struct	dhcpmsg dmsg;		/* holds outgoing DHCP discover	*/
					/*	message			*/
	struct	dhcpmsg dmsg2;		/* holds incoming DHCP offer	*/
					/* and outgoing request message	*/
	uint32	xid;			/* xid used for the exchange	*/
	int32	i;			/* retry counter		*/
	int32	len;			/* length of data read		*/
	char	*optptr;		/* pointer to options area	*/
	char	*eop;			/* address of end of packet	*/
	int32	msgtype;		/* type of DCHP message		*/
	uint32	addrmask;		/* address mask for network	*/
	uint32	routeraddr;		/* default router address	*/

	if (NetData.ipvalid) {		/* already have an IP address	*/
		return NetData.ipaddr;
	}
	udp_register(0, UDP_DHCP_SPORT, UDP_DHCP_CPORT);
	memcpy(&xid, NetData.ethaddr, 4);
					/* use 4 bytes from MAC as XID	*/

	/* handcraft a DHCP Discover message in dmsg */

	dmsg.dc_bop = 0x01;		/* Outgoing request		*/
	dmsg.dc_htype = 0x01;		/* hardware type is Ethernet	*/
	dmsg.dc_hlen = 0x06;		/* hardware address length	*/
	dmsg.dc_hops = 0x00;		/* Hop count			*/
	dmsg.dc_xid = xid;		/* xid (unique ID)		*/
	dmsg.dc_secs = 0x0000;		/* seconds			*/
	dmsg.dc_flags = 0x0000;		/* flags			*/
	dmsg.dc_cip = 0x00000000;	/* Client IP address		*/
	dmsg.dc_yip = 0x00000000;	/* Your IP address		*/
	dmsg.dc_sip = 0x00000000;	/* Server IP address		*/
	dmsg.dc_gip = 0x00000000;	/* Gateway IP address		*/
	memset(&dmsg.dc_chaddr,'\0',16);/* Client hardware address	*/
	memcpy(&dmsg.dc_chaddr, NetData.ethaddr, ETH_ADDR_LEN);
	memset(&dmsg.dc_bootp,'\0',192);/* zero the bootp area		*/
	dmsg.dc_cookie = 0x63825363;	/* Magic cookie for DHCP	*/

	dmsg.dc_opt[0] = 0xff & 53;	/* DHCP message type option	*/
	dmsg.dc_opt[1] = 0xff &  1;	/* option length		*/
	dmsg.dc_opt[2] = 0xff &  1;	/* DHCP Dicover message		*/
	dmsg.dc_opt[3] = 0xff &  0;	/* Options padding		*/

	dmsg.dc_opt[4] = 0xff & 55;	/* DHCP parameter request list	*/
	dmsg.dc_opt[5] = 0xff &  2;	/* option length		*/
	dmsg.dc_opt[6] = 0xff &  1;	/* request subnet mask 		*/
	dmsg.dc_opt[7] = 0xff &  3;	/* request default router addr.	*/

	dmsg.dc_opt[8] = 0xff &  0;	/* options padding		*/
	dmsg.dc_opt[9] = 0xff &  0;	/* options padding		*/
	dmsg.dc_opt[10]= 0xff &  0;	/* options padding		*/
	dmsg.dc_opt[11]= 0xff &  0;	/* options padding		*/

	len = (char *)&dmsg.dc_opt[11] - (char *)&dmsg + 1;

	udp_send(IP_BCAST, UDP_DHCP_SPORT, IP_ALLZEROS, UDP_DHCP_CPORT,
				(char *)&dmsg, len);

	/* Read 3 incoming DHCP messages and check for an offer	or	*/
	/*    wait for three timeout periods if no message arrives.	*/

	for (i=0; i<3; i++) {
		if ((len=udp_recv(0, UDP_DHCP_SPORT, UDP_DHCP_CPORT,
				(char *)&dmsg2, sizeof(struct dhcpmsg),
						3000)) == TIMEOUT) {
			continue;
		}

		/* Check that incoming message is a valid response (ID	*/
		/* matches our request)					*/

		if ( (dmsg2.dc_xid != xid) ) {
			continue;
		}

		eop = (char *)&dmsg2 + len - 1;
		optptr = (char *)&dmsg2.dc_opt;
		msgtype = addrmask = routeraddr = 0;
		while (optptr < eop) {

		    switch (*optptr) {
			case 53:	/* message type */
				msgtype = 0xff & *(optptr+2);
				break;

			case 1:		/* subnet mask */
				memcpy(&addrmask, optptr+2, 4);
				break;

			case 3:		/* router address */
				memcpy(&routeraddr, optptr+2, 4);
				break;
		    }
		    optptr++;	/* move to length octet */
		    optptr += (0xff & *optptr) + 1;
		}

		if (msgtype == 0x02) {	/* offer - send request	*/
			dmsg2.dc_opt[0] = 0xff & 53;
			dmsg2.dc_opt[1] = 0xff &  1;
			dmsg2.dc_opt[2] = 0xff &  3;
			dmsg2.dc_bop = 0x01;
			udp_send(IP_BCAST, UDP_DHCP_SPORT, IP_ALLZEROS,
				UDP_DHCP_CPORT,	(char *)&dmsg2,
					sizeof(struct dhcpmsg) - 4);
			
		} else if (dmsg2.dc_opt[2] != 0x05) {	/* if not an ack*/
			continue;			/* skip it	*/
		}
		if (addrmask != 0) {
			NetData.addrmask = addrmask;
		}
		if (routeraddr != 0) {
			NetData.routeraddr = routeraddr;
		}
		NetData.ipaddr = dmsg2.dc_yip;
		NetData.ipvalid = TRUE;
		udp_release(0, UDP_DHCP_SPORT, UDP_DHCP_CPORT);
		return NetData.ipaddr;
	}
	kprintf("DHCP failed to get response\r\n");
	udp_release(0, UDP_DHCP_SPORT, UDP_DHCP_CPORT);
	return (uint32)SYSERR;
}
