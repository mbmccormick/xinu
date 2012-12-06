/* ethStat.c - ethStat */

#include <xinu.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * ethStat - print statistics gathered by an Ethernet driver
 *------------------------------------------------------------------------
 */
void	ethStat (
	 uint16	minor			/* minor device number to print	*/
	)
{
	struct	ether	*ethptr;        /* ptr to device control block	*/
	struct	ag71xx	*nicptr;	/* ptr to device CSRs		*/
	uint32	tmp;			/* temporary			*/
	byte	mac[6];			/* MAC address			*/

	/* Point to control block and device */

	ethptr = &ethertab[minor];
	nicptr = ethptr->csr;

	 printf("eth%d:\n", minor);
	control(ethptr->dev->dvnum, ETH_CTRL_GET_MAC, (int32)mac, 0);
	printf("  MAC Address  %02X:%02X:%02X:%02X:%02X:%02X",
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	printf("      %8d MTU", ethptr->mtu);

	tmp = ethptr->state;
	printf("  Device ");
	if (tmp == ETH_STATE_FREE) {
		printf("FREE\n");
	}
	if (tmp == ETH_STATE_UP) {
		printf("UP  \n");
	} else if (tmp == ETH_STATE_DOWN) {
		printf("DOWN\n");
	}

	printf("  Errors %d\n", ethptr->errors);

	/* Ether statistics */
	tmp = ethptr->interruptStatus;
	printf("  IRQ Status   0x%08X [", tmp);
	if (tmp & IRQ_TX_PKTSENT) {
		printf(" TX");
	}
	if (tmp & IRQ_TX_UNDERFLOW) {
		printf(" UF");
	}
	if (tmp & IRQ_TX_BUSERR) {
		printf(" TE");
	}
	if (tmp & IRQ_RX_PKTRECV) {
		printf(" RX");
	}
	if (tmp & IRQ_RX_BUSERR) {
		printf(" RE");
	}
	printf(" ]\n");

	tmp = ethptr->interruptMask;
	printf("  IRQ Mask     0x%08X [", tmp);
	if (tmp & IRQ_TX_PKTSENT) {
		printf(" TX");
	}
	if (tmp & IRQ_TX_UNDERFLOW) {
		printf(" UF");
	}
	if (tmp & IRQ_TX_BUSERR) {
		printf(" TE");
	}
	if (tmp & IRQ_RX_PKTRECV) {
		printf(" RX");
	}
	if (tmp & IRQ_RX_BUSERR) {
		printf(" RE");
	}
	printf(" ]\n");

	tmp = nicptr->macConfig1;
	printf("  MAC Config 1 0x%08X [", tmp);
	if (tmp & MAC_CFG1_TX) {
		printf(" TX");
	}
	if (tmp & MAC_CFG1_SYNC_TX) {
		printf(" ST");
	}
	if (tmp & MAC_CFG1_RX) {
		printf(" RX");
	}
	if (tmp & MAC_CFG1_SYNC_RX) {
		printf(" SR");
	}
	if (tmp & MAC_CFG1_LOOPBACK) {
		printf(" LB");
	}
	if (tmp & MAC_CFG1_SOFTRESET) {
		printf(" RS");
	}
	printf(" ]\n");

	tmp = nicptr->macConfig2;
	printf("  MAC Config 2 0x%08X [", tmp);
	if (tmp & MAC_CFG2_FDX) {
		printf(" FDX");
	}
	if (tmp & MAC_CFG2_CRC) {
		printf(" CRC");
	}
	if (tmp & MAC_CFG2_PAD) {
		printf(" PAD");
	}
	if (tmp & MAC_CFG2_LEN_CHECK) {
		printf(" LEN");
	}
	printf(" ]\n");

	printf("  FIFO Config0 0x%08X", nicptr->fifoConfig0);
	printf("  FIFO Config1 0x%08X", nicptr->fifoConfig1);
	printf("  FIFO Config2 0x%08X\n", nicptr->fifoConfig2);
	printf("  FIFO Config3 0x%08X", nicptr->fifoConfig3);
	printf("  FIFO Config4 0x%08X", nicptr->fifoConfig4);
	printf("  FIFO Config5 0x%08X\n", nicptr->fifoConfig5);
	printf("\n");

	printf("  Tx Ring      0x%08X", ethptr->txRing);
	printf("  Tx Bufs      0x%08X", ethptr->txBufs);
	printf("  Tx Ring Size   %8d\n", ethptr->txRingSize);
	printf("  Tx IRQ Count   %8d", ethptr->txirq);
	printf("  Tx Head        %8d", ethptr->txHead);
	printf("  Tx Tail        %8d\n", ethptr->txTail);
	printf("  Tx Control   0x%08X", nicptr->txControl);
	printf("  Tx DMA       0x%08X", nicptr->txDMA);
	printf("  Tx Status    0x%08X\n", nicptr->txStatus);
	printf("\n");
	printf("  Rx Ring      0x%08X", ethptr->rxRing);
	printf("  Rx Bufs      0x%08X", ethptr->rxBufs);
	printf("  Rx Ring Size   %8d\n", ethptr->rxRingSize);
	printf("  Rx IRQ Count   %8d", ethptr->rxirq);
	printf("  Rx Head        %8d", ethptr->rxHead);
	printf("  Rx Errors      %8d\n", ethptr->rxErrors);
	printf("  Rx Control   0x%08X", nicptr->rxControl);
	printf("  Rx DMA       0x%08X", nicptr->rxDMA);
	printf("  Rx Status    0x%08X\n", nicptr->rxStatus);

	printf("\n");
}

/*------------------------------------------------------------------------
 * ethStat2 - print statistics gathered by an Ethernet driver
 *------------------------------------------------------------------------
 */
void ethStat2(void)
{
	struct ether *ethptr = NULL;		/* pointer to ether entry			*/
	int i = 0;

	/* Initialize pointers */
	ethptr = &ethertab[0];

	printf("ethStat2()\r\n");
	struct dmaDescriptor *dmaptr;
	for (i = 0; i < ETH_RX_RING_ENTRIES; i++)
	{
		dmaptr = &ethptr->rxRing[i];
		if (0 == dmaptr->control)
		{
			printf("[%08X]Rx Addr 0x%08X", (uint32)dmaptr,
					dmaptr->address);
			printf("  Rx Cntl 0x%08X", dmaptr->control);
			printf("  Rx Next 0x%08X\r\n", dmaptr->next);
		}
	}
}
