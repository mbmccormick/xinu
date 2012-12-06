/* ethInit.c - ethInit */

#include <xinu.h>

struct	ether	ethertab[Neth];		/* Ethernet control blocks */

/*------------------------------------------------------------------------
 * udelay - microsecond delay loop (CPU loop)
 *------------------------------------------------------------------------
 */
void	udelay(uint32 n) {

	uint32	delay;			/* amount to delay measured in	*/
					/*   clock cycles		*/
	uint32	start = 0;		/* clock at start of delay	*/
	uint32	target = 0;		/* computed clk at end of delay	*/
	uint32	count = 0;		/* current clock during loop	*/

	delay = 200 * n;		/* 200 CPU cycles per usec */

	start = clkcount();		/* Get current clock */
	target = start + delay;		/* Compute finish time */

	if (target >= start) {
        	while (((count = clkcount()) < target) &&
				(count >= start)) {
			;  /* spin doing nothing */
		}
	} else {

		/* need to wrap around counter */

		while ((count = clkcount()) > start) {
			;  /* spin doing nothing */
		}
		while ((count = clkcount()) < target) {
			;  /* spin doing nothing */
		}
	}
}

/*------------------------------------------------------------------------
 * mdelay - millisecond delay loop (CPU loop)
 *------------------------------------------------------------------------
 */
void	mdelay(uint32 n) {
	int i;

	for (i = 0; i < n; i++) {
		udelay(1000);
	}
}

/*------------------------------------------------------------------------
 * ethInit - Initialize Ethernet device structures
 *------------------------------------------------------------------------
 */
devcall	ethInit (
	 struct dentry *devptr
	)
{
	struct	ether	*ethptr;
	struct	ag71xx	*nicptr;
	uint32	*rstptr;
	uint32	rstbit;

	/* Initialize structure pointers */

	ethptr = &ethertab[devptr->dvminor];
	memset(ethptr, '\0', sizeof(struct ether));
	ethptr->dev = devptr;
	ethptr->csr = devptr->dvcsr;

	/* Get device CSR address */

	nicptr = (struct ag71xx *)devptr->dvcsr;
	rstptr = (uint32 *)RESET_CORE;
	if (devptr->dvminor == 0) {	/* use E0 on first device only */
		rstbit = RESET_E0_MAC;
	} else {
		rstbit = RESET_E1_MAC;
	}

	ethptr->state = ETH_STATE_DOWN;
	ethptr->rxRingSize = ETH_RX_RING_ENTRIES;
	ethptr->txRingSize = ETH_TX_RING_ENTRIES;
	ethptr->mtu = ETH_MTU;
	ethptr->interruptMask = IRQ_TX_PKTSENT | IRQ_TX_BUSERR
		| IRQ_RX_PKTRECV | IRQ_RX_OVERFLOW | IRQ_RX_BUSERR;

	ethptr->errors = 0;
	ethptr->isema = semcreate(0);
	ethptr->istart = 0;
	ethptr->icount = 0;
	ethptr->ovrrun = 0;
	ethptr->rxOffset = ETH_PKT_RESERVE;

	colon2mac(nvramGet("et0macaddr"), ethptr->devAddress);
	ethptr->addressLength = ETH_ADDR_LEN;

	/* Reset the device */

	nicptr->macConfig1 |= MAC_CFG1_SOFTRESET;
	udelay(20);
	*rstptr |= rstbit;
	mdelay(100);
	*rstptr &= ~rstbit;
	mdelay(100);

	/* Enable transmit and receive */

	nicptr->macConfig1 = MAC_CFG1_TX | MAC_CFG1_SYNC_TX |
		MAC_CFG1_RX | MAC_CFG1_SYNC_RX;

	/* Configure full duplex, auto padding CRC, */
	/*	and interface mode		    */

	nicptr->macConfig2 |= MAC_CFG2_FDX | MAC_CFG2_PAD |
			MAC_CFG2_LEN_CHECK | MAC_CFG2_IMNIBBLE;

	/* Enable FIFO modules */

	nicptr->fifoConfig0 = FIFO_CFG0_WTMENREQ | FIFO_CFG0_SRFENREQ |
          FIFO_CFG0_FRFENREQ | FIFO_CFG0_STFENREQ | FIFO_CFG0_FTFENREQ;

	nicptr->fifoConfig1 = 0x0FFF0000;

	/* Max out number of words to store in Receiver RAM */

	nicptr->fifoConfig2 = 0x00001FFF;

	/* Drop any incoming packet with errors in the Rx stats vector	*/

	nicptr->fifoConfig4 = 0x0003FFFF;

	/* Drop short packets (set "don't care" on Rx stats vector bits	*/

	nicptr->fifoConfig5 = 0x0003FFFF;

	/* Buffers should be page-aligned and cache-aligned */

	ethptr->rxBufs = (struct ethPktBuffer **)getstk(PAGE_SIZE);
	ethptr->txBufs = (struct ethPktBuffer **)getstk(PAGE_SIZE);
	ethptr->rxRing = (struct dmaDescriptor *)getstk(PAGE_SIZE);
	ethptr->txRing = (struct dmaDescriptor *)getstk(PAGE_SIZE);

	if (  ( (int32)ethptr->rxBufs == SYSERR )
	   || ( (int32)ethptr->txBufs == SYSERR )
	   || ( (int32)ethptr->rxRing == SYSERR )
	   || ( (int32)ethptr->txRing == SYSERR ) ) {
		return SYSERR;
	}

	/* Translate buffer and ring pointers to KSEG1 */

	ethptr->rxBufs = (struct ethPktBuffer **)
		(((uint32)ethptr->rxBufs - PAGE_SIZE +
		  sizeof(int32)) | KSEG1_BASE);
	ethptr->txBufs = (struct ethPktBuffer **)
		(((uint32)ethptr->txBufs - PAGE_SIZE +
		  sizeof(int32)) | KSEG1_BASE);
	ethptr->rxRing = (struct dmaDescriptor *)
		(((uint32)ethptr->rxRing - PAGE_SIZE +
		  sizeof(int32)) | KSEG1_BASE);
	ethptr->txRing = (struct dmaDescriptor *)
		(((uint32)ethptr->txRing - PAGE_SIZE +
		  sizeof(int32)) | KSEG1_BASE);

	/* Set buffer pointers and rings to zero */

	memset(ethptr->rxBufs, '\0', PAGE_SIZE);
	memset(ethptr->txBufs, '\0', PAGE_SIZE);
	memset(ethptr->rxRing, '\0', PAGE_SIZE);
	memset(ethptr->txRing, '\0', PAGE_SIZE);

	/* Initialize the interrupt vector and enable the device */

	interruptVector[devptr->dvirq] = devptr->dvintr;
	enable_irq(devptr->dvirq);

	return OK;
}
