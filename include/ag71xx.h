/* ag71xx.h  -  Definitions for an Atheros ag71xx Ethernet device	*/

/* Ring buffer sizes */

#define	ETH_RX_RING_ENTRIES 64  /* Number of buffers on Rx Ring	*/
#define ETH_TX_RING_ENTRIES 128 /* Number of buffers on Tx Ring	*/

#define ETH_PKT_RESERVE     64

/* Control and Status register layout for the ag71xx  */

struct ag71xx {
    volatile uint32 macConfig1;		/* 0x000 MAC configuration 1	*/

#define MAC_CFG1_TX        (1 <<  0)	/* Enable Transmitter		*/
#define MAC_CFG1_SYNC_TX   (1 <<  1)	/* Syncronize Transmitter	*/
#define MAC_CFG1_RX        (1 <<  2)	/* Enable Receiver		*/
#define MAC_CFG1_SYNC_RX   (1 <<  3)	/* Syncronize Receiver		*/
#define MAC_CFG1_LOOPBACK  (1 <<  8)	/* Enable Loopback		*/
#define MAC_CFG1_SOFTRESET (1 << 31)	/* Software Reset		*/

    volatile uint32 macConfig2;		/* 0x004 MAC configuration 2	*/
#define MAC_CFG2_FDX       (1 <<  0)	/* Enable Full Duplex		*/
#define MAC_CFG2_CRC       (1 <<  1)	/* Enable CRC appending		*/
#define MAC_CFG2_PAD       (1 <<  2)	/* Enable padding of short pkts	*/
#define MAC_CFG2_LEN_CHECK (1 <<  4)	/* Enable length field checking	*/
#define MAC_CFG2_HUGE      (1 <<  5)	/* Enable frames longer than max*/
#define MAC_CFG2_IMNIBBLE  (1 <<  8)	/* "nibble mode" interface type	*/
#define MAC_CFG2_IMBYTE    (2 <<  8)	/* "byte mode" interface type	*/


    volatile uint32 pad00[2];
    volatile uint32 pad01[4];
    volatile uint32 pad02[4];
    volatile uint32 pad03[4];

    volatile uint32 macAddr1;		/* 0x040 MAC Address part 1	*/
    volatile uint32 macAddr2;		/* 0x044 MAC Address part 2	*/

    volatile uint32 fifoConfig0;	/* 0x048 MAC configuration 0	*/

#define FIFO_CFG0_WTMENREQ (1 <<  8)	/* Enable FIFO watermark module	*/
#define FIFO_CFG0_SRFENREQ (1 <<  9)	/* Enable FIFO system Rx module	*/
#define FIFO_CFG0_FRFENREQ (1 << 10)	/* Enable FIFO fabric Rx module	*/
#define FIFO_CFG0_STFENREQ (1 << 11)	/* Enable FIFO system Tx module	*/
#define FIFO_CFG0_FTFENREQ (1 << 12)	/* Enable FIFO fabric Tx module	*/

    volatile uint32 fifoConfig1;	/* 0x04C MAC configuration 1	*/
    volatile uint32 fifoConfig2;	/* 0x050 MAC configuration 2	*/
    volatile uint32 fifoConfig3;	/* 0x054 MAC configuration 3	*/
    volatile uint32 fifoConfig4;	/* 0x058 MAC configuration 4	*/
    volatile uint32 fifoConfig5;	/* 0x05C MAC configuration 5	*/

    volatile uint32 pad06[72];

    volatile uint32 txControl;		/* 0x180 Tx Control		*/

#define TX_CTRL_ENABLE     (1 <<  0)	/* Enable Tx                    */
    volatile uint32 txDMA;		/* 0x184 Tx DMA Descriptor	*/
    volatile uint32 txStatus;		/* 0x188 Tx Status        	*/

#define TX_STAT_SENT       (1 <<  0)	/* Packet Sent                  */
#define TX_STAT_UNDER      (1 <<  1)	/* Tx Underrun                  */

    volatile uint32 rxControl;		/* 0x18C Rx Control          	*/

#define RX_CTRL_RXE        (1 <<  0)	/* Enable receiver            	*/

    volatile uint32 rxDMA;		/* 0x190 Rx DMA Descriptor    	*/
    volatile uint32 rxStatus;		/* 0x194 Rx Status          	*/

#define RX_STAT_RECVD      (1 <<  0)	/* Packet Received              */
#define RX_STAT_OVERFLOW   (1 <<  2)	/* DMA Rx overflow              */
#define RX_STAT_COUNT   (0xFF << 16)	/* Count of packets received    */

    volatile uint32 interruptMask;	/* 0x198 Interrupt Mask     	*/

#define IRQ_TX_PKTSENT     (1 <<  0)	/* Packet Sent                  */
#define IRQ_TX_UNDERFLOW   (1 <<  1)	/* Tx packet underflow          */
#define IRQ_TX_BUSERR      (1 <<  3)	/* Tx Bus Error                 */
#define IRQ_RX_PKTRECV     (1 <<  4)	/* Rx Packet received           */
#define IRQ_RX_OVERFLOW    (1 <<  6)	/* Rx Overflow                  */
#define IRQ_RX_BUSERR      (1 <<  7)	/* Rx Bus Error                 */

    volatile uint32 interruptStatus;    /* 0x19C Interrupt Status     	*/
};

/* Receiver header struct and constants */

#define ETH_RX_FLAG_OFIFO   0x0001	/* FIFO Overflow            	*/
#define ETH_RX_FLAG_CRCERR  0x0002	/* CRC Error                    */
#define ETH_RX_FLAG_SERR    0x0004	/* Receive Symbol Error         */
#define ETH_RX_FLAG_ODD     0x0008	/* Frame has odd number nibbles */
#define ETH_RX_FLAG_LARGE   0x0010	/* Frame is > RX MAX Length     */
#define ETH_RX_FLAG_MCAST   0x0020	/* Dest is Multicast Address    */
#define ETH_RX_FLAG_BCAST   0x0040	/* Dest is Broadcast Address    */
#define ETH_RX_FLAG_MISS    0x0080	/* Received due to promisc mode */
#define ETH_RX_FLAG_LAST    0x0800	/* Last buffer in frame         */
#define ETH_RX_FLAG_ERRORS  ( ETH_RX_FLAG_ODD | ETH_RX_FLAG_SERR | \
			      ETH_RX_FLAG_CRCERR | ETH_RX_FLAG_OFIFO )

/* Header on a received packet */

struct rxHeader {
    uint16 length;     			/* Length of packet data      	*/
    uint16 flags;           		/* Receive flags          	*/
    uint16 pad[12];             	/* Padding                     	*/
};

/* Ethernet DMA descriptor */

#define ETH_DESC_CTRL_LEN   0x00001fff	/* Mask for length field      	*/
#define ETH_DESC_CTRL_MORE  0x10000000	/* More fragments             	*/
#define ETH_DESC_CTRL_EMPTY 0x80000000	/* Empty descriptor        	*/

/* Descriptor for the DMA engine to determine where to	*/
/*	find a packet buffer.				*/

struct dmaDescriptor {
    uint32 address;            		/* Stored as physical address 	*/
    uint32 control;            		/* DMA control bits             */
    uint32 next;                	/* Next descriptor in the ring  */
};

#define RESET_CORE 0xB806001C		/* Atheros bus core reset reg	*/
#define RESET_E0_MAC (1 <<  9)		/* Reset Ethernet zero MAC     	*/
#define RESET_E1_MAC (1 << 13)		/* Reset Ethernet one  MAC    	*/
