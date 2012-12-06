/* ethRead.c  - ethRead */

#include <xinu.h>

/*------------------------------------------------------------------------
 * ethRead - read a packet from an Ethernet device
 *------------------------------------------------------------------------
 */
devcall	ethRead (
	 struct	dentry	*devptr,	/* entry in device switch table	*/
	 void	*buf,			/* buffer to hold packet	*/
	 uint32	len			/* length of buffer		*/
	)
{
	struct	ether	*ethptr;	/* ptr to entry in ethertab	*/
	struct	ethPktBuffer	*pkt;	/* ptr to a packet		*/
	uint32	length;			/* packet length		*/

	ethptr = &ethertab[devptr->dvminor];

	if (ETH_STATE_UP != ethptr->state) {
		return SYSERR; /* interface is down */
	}

	/* Make sure user's buffer is large enough to store at least	*/
	/*   the header of a packet					*/

	if (len < ETH_HDR_LEN) {
		return SYSERR;
	}

	/* Wait for a packet to arrive */

	wait(ethptr->isema);

	/* Pick up packet */

	pkt = ethptr->in[ethptr->istart];
	ethptr->in[ethptr->istart] = NULL;
	ethptr->istart = (ethptr->istart + 1) % ETH_IBUFSIZ;
	ethptr->icount--;

	if (pkt == NULL) {
		return 0;
	}

	length = pkt->length;
	if (length > len) {
		length = len;
	}
	memcpy(buf, (byte *)(((uint32)pkt->buf) | KSEG1_BASE), length);
	freebuf((char *)pkt);

	return length;
}
