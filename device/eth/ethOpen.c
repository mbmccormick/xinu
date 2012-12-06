/* ethOpen.c -  ethOpen */

#include <xinu.h>

/*------------------------------------------------------------------------
 * ethOpen - prepare an ethernet device for use (interrupts disabled)
 *------------------------------------------------------------------------
 */
devcall	ethOpen (
	 struct	dentry	*devptr		/* entry in device switch table	*/
	)
{
	int i;				/* loop index			*/
	intmask im;			/* saved interrupt mask		*/
	struct	ether	*ethptr;	/* ptr to entry in ethertab	*/
	struct	ag71xx	*nicptr;	/* ptr to device CSRs		*/

	/* Initialize structure pointers */

	ethptr = &ethertab[devptr->dvminor];
	nicptr = ethptr->csr;

	im = disable();

	/* Clear the Rx Counter */

	while (nicptr->rxStatus & RX_STAT_COUNT)  {
		nicptr->rxStatus = RX_STAT_RECVD;
	}

	/* Initialize the Tx ring */

	for (i = 0; i < ETH_TX_RING_ENTRIES; i++) {
		ethptr->txRing[i].next = (uint32)(
		    ethptr->txRing + ((i + 1) % ETH_TX_RING_ENTRIES))
		    & PMEM_MASK;
		ethptr->txRing[i].control = ETH_DESC_CTRL_EMPTY;
		ethptr->txBufs[i] = NULL;
	}

	/* Point NIC to start of Tx ring */

	nicptr->txDMA = ((uint32)ethptr->txRing) & PMEM_MASK;

	/* Allocate buffer pool for transmit DMA engine */

	ethptr->outPool = mkbufpool(ETH_TX_BUF_SIZE + ETH_PKT_RESERVE
                 + sizeof(struct ethPktBuffer), ETH_TX_RING_ENTRIES);
	if (ethptr->outPool == SYSERR) {
		kprintf("eth%d cannot make output buffer pool\r\n",
			devptr->dvminor);
		return SYSERR;
	}

	/* Allocate buffer pool for receive DMA engine */

	ethptr->inPool = mkbufpool(ETH_RX_BUF_SIZE + ETH_PKT_RESERVE
		+ sizeof(struct ethPktBuffer),
		  ETH_RX_RING_ENTRIES + ETH_IBUFSIZ);
	if (ethptr->inPool == SYSERR) {
		kprintf("eth%d cannot make input buffer pool\r\n",
			devptr->dvminor);
		return SYSERR;
	}

	/* Initialize receive ring */

	for (i = 0; i < ETH_RX_RING_ENTRIES; i++) {
		ethptr->rxRing[i].next = (uint32)(
		    ethptr->rxRing + ((i + 1) % ETH_RX_RING_ENTRIES))
		    & PMEM_MASK;
	}

	/* Fill up receive ring with available buffers */

	for (i = 0; i < ethptr->rxRingSize; i++) {
		allocRxBuffer(ethptr, i);
	}

	/* Point NIC to start of receive ring */

	nicptr->rxDMA = ((uint32)ethptr->rxRing) & PMEM_MASK;

	ethControl(devptr, ETH_CTRL_SET_MAC,
			(int32)ethptr->devAddress, 0);

	/* Start receive engine */

	nicptr->rxControl = RX_CTRL_RXE;

	ethptr->state = ETH_STATE_UP;

	/* Enable interrupts on the device */

	nicptr->interruptMask = ethptr->interruptMask;

	restore(im);
	return OK;
}
