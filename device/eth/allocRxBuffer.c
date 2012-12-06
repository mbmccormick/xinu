/* allocRxBuffer.c - allocRxBuffer */

#include <xinu.h>

/*------------------------------------------------------------------------
 * allocRxBuffer - allocate an Ethernet packet buffer structure
 *------------------------------------------------------------------------
 */
int32	allocRxBuffer (
	 struct	ether	*ethptr,	/* ptr to device control block	*/
	 int32	destIndex		/* index in receive ring	*/
	)
{
	struct	ethPktBuffer	*pkt;
	struct	dmaDescriptor	*dmaptr;

	/* Compute next ring location modulo the ring size */

	destIndex %= ethptr->rxRingSize;

	/* Allocate a packet buffer */

	pkt = (struct ethPktBuffer *)getbuf(ethptr->inPool);

	if ((uint32)pkt == SYSERR) {
		kprintf("eth0 allocRxBuffer() error\r\n");
		return SYSERR;
	}
	pkt->length = ETH_RX_BUF_SIZE;
	pkt->buf = (byte *)(pkt + 1);

	/* Data region offset by size of rx header */

	pkt->data = pkt->buf + ethptr->rxOffset;

	ethptr->rxBufs[destIndex] = pkt;

	/* Fill in DMA descriptor fields */

	dmaptr = ethptr->rxRing + destIndex;
	dmaptr->control = ETH_DESC_CTRL_EMPTY;
	dmaptr->address = (uint32)(pkt->buf) & PMEM_MASK;

	return OK;
}
