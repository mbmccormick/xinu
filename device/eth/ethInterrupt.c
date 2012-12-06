/* ethInterrupt.c - ethInterrupt */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rxPackets - handler for receiver interrupts
 *------------------------------------------------------------------------
 */
void	rxPackets (
	 struct	ether *ethptr,		/* ptr to control block		*/
	 struct	ag71xx *nicptr		/* ptr to device CSRs		*/
	)
{
	struct	dmaDescriptor *dmaptr;	/* ptr to DMA descriptor	*/
	struct	ethPktBuffer  *pkt;	/* ptr to one packet buffer	*/
	int32	head;


	/* Move to next packet, wrapping around if needed */

	head = ethptr->rxHead % ETH_RX_RING_ENTRIES;
	dmaptr = &ethptr->rxRing[head];
	if (dmaptr->control & ETH_DESC_CTRL_EMPTY) {
		nicptr->rxStatus = RX_STAT_RECVD;
		return;
	}

	pkt = ethptr->rxBufs[head];
	pkt->length = dmaptr->control & ETH_DESC_CTRL_LEN;

	if (ethptr->icount < ETH_IBUFSIZ) {
		allocRxBuffer(ethptr, head);
		ethptr->in[(ethptr->istart + ethptr->icount) %
				ETH_IBUFSIZ] =  pkt;
		ethptr->icount++;
		signal(ethptr->isema);
	} else {
		ethptr->ovrrun++;
		memset(pkt->buf, '\0', pkt->length);
	}

	ethptr->rxHead++;

	/* Clear the Rx interrupt */

	nicptr->rxStatus = RX_STAT_RECVD;
	return;
}

/*------------------------------------------------------------------------
 * txPackets - handler for transmitter interrupts
 *------------------------------------------------------------------------
 */
void	txPackets (
	 struct	ether *ethptr,		/* ptr to control block		*/
	 struct ag71xx *nicptr		/* ptr to device CSRs		*/
	)
{
	struct	dmaDescriptor *dmaptr;
	struct	ethPktBuffer **epb = NULL;
	struct	ethPktBuffer *pkt = NULL;
	uint32	head;

	if (ethptr->txHead == ethptr->txTail) {
		nicptr->txStatus = TX_STAT_SENT;
		return;
	}

	/* While packets remain to be transmitted */

	while (ethptr->txHead != ethptr->txTail) {
		head = ethptr->txHead % ETH_TX_RING_ENTRIES;
		dmaptr = &ethptr->txRing[head];
		if (!(dmaptr->control & ETH_DESC_CTRL_EMPTY)) {
			break;
		}

		epb = &ethptr->txBufs[head];

		/* Clear the Tx interrupt */

		nicptr->txStatus = TX_STAT_SENT;

		ethptr->txHead++;
		pkt = *epb;
		if (NULL == pkt) {
			continue;
		}
		freebuf((void *)((bpid32)pkt & (PMEM_MASK | KSEG0_BASE)));
		*epb = NULL;
	}
	return;
}

/*------------------------------------------------------------------------
 * ethInterrupt - decode and handle interrupt from an Ethernet device
 *------------------------------------------------------------------------
 */
interrupt ethInterrupt(void)
{
	struct	ether	*ethptr;	/* ptr to control block		*/
	struct	ag71xx	*nicptr;	/* ptr to device CSRs 		*/
	uint32	status;
	uint32	mask;

	/* Initialize structure pointers */

	ethptr = &ethertab[0];		/* default physical Ethernet	*/
	if (!ethptr) {
		return;
	}
	nicptr = ethptr->csr;
	if (!nicptr) {
		return;
	}

	/* Obtain status bits from device */

	mask = nicptr->interruptMask;
	status = nicptr->interruptStatus & mask;

	/* Record status in ether struct */

	ethptr->interruptStatus = status;

	if (status == 0) {
		return;
	}

	sched_cntl(DEFER_START);

	if (status & IRQ_TX_PKTSENT) {	/* handle transmitter interrupt	*/
		ethptr->txirq++;
		txPackets(ethptr, nicptr);
	}

	if (status & IRQ_RX_PKTRECV) {	/* handle receiver interrupt	*/
		ethptr->rxirq++;
		rxPackets(ethptr, nicptr);
	}

	/* Handle errors (transmit or receive overflow) */

	if (status & IRQ_RX_OVERFLOW) {
		/* Clear interrupt and restart processing */
		nicptr->rxStatus = RX_STAT_OVERFLOW;
		nicptr->rxControl = RX_CTRL_RXE;
		ethptr->errors++;
	}

	if ((status & IRQ_TX_UNDERFLOW) ||
		(status & IRQ_TX_BUSERR) || (status & IRQ_RX_BUSERR)) {
		panic("Catastrophic Ethernet error");
	}
	sched_cntl(DEFER_STOP);
	return;
}
