/* pdump.c - packet dump function */

#include <xinu.h>

/*------------------------------------------------------------------------
 * pdump - dump a packet
 *------------------------------------------------------------------------
 */
void	pdump(struct  netpacket *pptr)
{
	struct	arppacket *aptr;
	struct	dhcpmsg	  *dptr;
	int32	i;
	char	*cptr;
	uint32	b1, b2, b3, b4;

    kprintf("Eth dst: %02x:%02x:%02x:%02x:%02x:%02x  ",
		pptr->net_ethdst[0],
		pptr->net_ethdst[1],
		pptr->net_ethdst[2],
		pptr->net_ethdst[3],
		pptr->net_ethdst[4],
		pptr->net_ethdst[5]
    );
    kprintf("Eth src: %02x:%02x:%02x:%02x:%02x:%02x  Eth typ: %04x\n\n\r",
		pptr->net_ethsrc[0],
		pptr->net_ethsrc[1],
		pptr->net_ethsrc[2],
		pptr->net_ethsrc[3],
		pptr->net_ethsrc[4],
		pptr->net_ethsrc[5],
		pptr->net_ethtype
    );

    switch (pptr->net_ethtype) {

    case 0x0800:
	kprintf("IP src: %d.%d.%d.%d   IP dst: %d.%d.%d.%d   IP proto: %d\n\n\r",
		(pptr->net_ipsrc>>24)&0xff, (pptr->net_ipsrc>>16)&0xff,
		(pptr->net_ipsrc>>8)&0xff, pptr->net_ipsrc&0xff,
		(pptr->net_ipdst>>24)&0xff, (pptr->net_ipdst>>16)&0xff,
		(pptr->net_ipdst>>8)&0xff, pptr->net_ipdst&0xff,
		pptr->net_ipproto);
	kprintf("IP vh=%02x tos=%d iplen=%d id=%d frag=%d, ttl=%d\n\n\r",
		pptr->net_ipvh,pptr->net_iptos,pptr->net_iplen,
		pptr->net_ipid, pptr->net_ipfrag,pptr->net_ipttl);


	if (pptr->net_ipproto == IP_ICMP) {
		kprintf("ICMP: ");
		if (pptr->net_ictype == ICMP_ECHOREQST) {
			kprintf("  echo request: type=%2d, code=%2d, cksum=%04X, id=%5d",
				pptr->net_ictype, pptr->net_iccode, pptr->net_iccksum,
				pptr->net_icident);
			kprintf(" seq=%5d\n\r       data=",pptr->net_icseq);
			for (i=0 ; i<20 ; i++) {
				kprintf(" %02X",0xff&pptr->net_icdata[i]);
			}
		 	kprintf("\n\r");
			break;
		} else if (pptr->net_ictype == ICMP_ECHOREPLY) {
			kprintf("  echo reply:   type=%2d, code=%2d, cksum=%04X, id=%5d",
				pptr->net_ictype, pptr->net_iccode, pptr->net_iccksum,
				pptr->net_icident);
			kprintf(" seq=%5d\n\r       data=",pptr->net_icseq);
			for (i=0 ; i<20 ; i++) {
				kprintf(" %02X",0xff&pptr->net_icdata[i]);
			}
		 	kprintf("\n\r");
			break;
		}
		cptr = (char *) &pptr->net_ictype;
		for (i=0; i<32;i++) {
			kprintf("%02X ",0xff & *cptr++);
		}
		kprintf("\n\r");
		break;
	}

	if (pptr->net_ipproto != IP_UDP) {	/* skip non-UDP packets */
		break;
	}
	kprintf("UDP src port: %d  UDP dst port: %d  UDP len: %d  UDP Cksum %04x\n\n\r",
		pptr->net_udpsport, pptr->net_udpdport, pptr->net_udplen,pptr->net_udpcksum);

		cptr = ((char *)pptr);
		b1 = 0xff & *cptr++; b2 = 0xff & *cptr++;
		kprintf("           %02x  %02x  --           %3d  %3d\n\r",
			b1, b2, b1, b2);
		for (i = (pptr->net_udplen + 34) >> 2; i>0 ; i--) {
			b1 = 0xff & *cptr++; b2 = 0xff & *cptr++;
			b3 = 0xff & *cptr++; b4 = 0xff & *cptr++;
			kprintf(
			"   %02x  %02x  %02x  %02x  -- %3d  %3d  %3d  %3d  --  %5d  %5d\n\r",
				b1, b2, b3, b4, b1, b2, b3, b4, (b1<<8)|b2, (b3<<8)|b4);
		}

	/* only handle DHCP */

	if ( (pptr->net_udpsport != UDP_DHCP_CPORT) &&
	     (pptr->net_udpsport != UDP_DHCP_SPORT) ) {
		break;
	}
	dptr = (struct dhcpmsg *) pptr->net_udpdata;
	kprintf("DHCP op=%d xid=%08x yid=%08x opt=%d paktyp=%d\n\n\r",
		dptr->dc_bop, dptr->dc_xid, dptr->dc_yip,
		dptr->dc_opt[0], dptr->dc_opt[2]);
		break;

    case 0x0806:
	aptr = (struct arppacket *)pptr;

	kprintf("ARP %s sndha: %02x:%02x:%02x:%02x:%02x:%02x  ",
		aptr->arp_op == 1 ? "REQ " : "REPLY",
		aptr->arp_sndha[0],
		aptr->arp_sndha[1],
		aptr->arp_sndha[2],
		aptr->arp_sndha[3],
		aptr->arp_sndha[4],
		aptr->arp_sndha[5]
	);

	kprintf("   sndpa: %d.%d.%d.%d\n\n\r",
		(aptr->arp_sndpa>>24)&0xff, (aptr->arp_sndpa>>16)&0xff,
		(aptr->arp_sndpa>>8)&0xff, aptr->arp_sndpa&0xff);

	kprintf("ARP      tarha: %02x:%02x:%02x:%02x:%02x:%02x  ",
		aptr->arp_tarha[0],
		aptr->arp_tarha[1],
		aptr->arp_tarha[2],
		aptr->arp_tarha[3],
		aptr->arp_tarha[4],
		aptr->arp_tarha[5]
	);

	kprintf("   tarpa: %d.%d.%d.%d\n\n\r",
		(aptr->arp_tarpa>>24)&0xff, (aptr->arp_tarpa>>16)&0xff,
		(aptr->arp_tarpa>>8)&0xff, aptr->arp_tarpa&0xff
	);


	break;
    }
    return;
}
