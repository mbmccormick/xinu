/* ethWrite.c -  etherWrite */

#include <xinu.h>

/*------------------------------------------------------------------------
 * ethWrite - write a packet to an Ethernet device
 *------------------------------------------------------------------------
 */
devcall	ethWrite (
	 struct	dentry	*devptr,	/* entry in device switch table	*/
	 void	*buf,			/* buffer to hold packet	*/
	 uint32	len			/* length of buffer		*/
	)
{
	struct	ether	*ethptr;
	struct	ag71xx	*nicptr;
	struct	ethPktBuffer *pkt;
	struct	dmaDescriptor *dmaptr;
	uint32	tail = 0;
	byte	*buffer;

	buffer = buf;

	ethptr = &ethertab[devptr->dvminor];
	nicptr = ethptr->csr;

	if ((ETH_STATE_UP != ethptr->state)
		|| (len < ETH_HDR_LEN)
		|| (len > (ETH_TX_BUF_SIZE - ETH_VLAN_LEN))) {
		return SYSERR;
	}

	tail = ethptr->txTail % ETH_TX_RING_ENTRIES;
	dmaptr = &ethptr->txRing[tail];

	if (!(dmaptr->control & ETH_DESC_CTRL_EMPTY)) {
		ethptr->errors++;
		return SYSERR;
	}

	pkt = (struct ethPktBuffer *)getbuf(ethptr->outPool);
	if ((uint32)pkt == SYSERR) {
		ethptr->errors++;
		return SYSERR;
	}

	/* Translate pkt pointer into uncached memory space */

	pkt = (struct ethPktBuffer *)((int)pkt | KSEG1_BASE);
	pkt->buf = (byte *)(pkt + 1);
	pkt->data = pkt->buf;
	memcpy(pkt->data, buffer, len);

	/* Place filled buffer in outgoing queue */
	ethptr->txBufs[tail] = pkt;

	/* Add the buffer to the transmit ring.  Note that the address	*/
	/*  must be physical (USEG) because the DMA engine will used it	*/

	ethptr->txRing[tail].address = (uint32)pkt->data & PMEM_MASK;

	/* Clear empty flag and write the length */

	ethptr->txRing[tail].control = len & ETH_DESC_CTRL_LEN;

	/* move to next position */

	ethptr->txTail++;

	if (nicptr->txStatus & TX_STAT_UNDER) {
		nicptr->txDMA = ((uint32)(ethptr->txRing + tail))
			& PMEM_MASK;
		nicptr->txStatus = TX_STAT_UNDER;
	}

	/* Enable transmit interrupts */

	nicptr->txControl = TX_CTRL_ENABLE;
	return len;
}
