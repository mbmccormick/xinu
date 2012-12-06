/* ether.h */

/* Ethernet packet format:

 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |  Dest. MAC (6)  |  Src. MAC (6)   |Type (2)|      Data (46-1500)...   |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

#define	ETH_ADDR_LEN	6	/* Length of Ethernet (MAC) address	*/

/* Ethernet packet header */

struct	etherPkt {
	byte	dst[ETH_ADDR_LEN];	/* Destination Mac address	*/
	byte	src[ETH_ADDR_LEN];	/* Source Mac address		*/
	uint16	type;			/* Ether type field		*/
	byte	data[1];		/* Packet payload		*/
};

#define	ETH_HDR_LEN	14	/* Length of Ethernet packet header	*/

/* Ethernet Buffer lengths */

#define	ETH_IBUFSIZ	1024	/* input buffer size			*/

/* Ethernet DMA buffer sizes */

#define	ETH_MTU		1500	/* Maximum transmission unit		*/
#define	ETH_VLAN_LEN	4	/* Length of Ethernet vlan tag		*/
#define ETH_CRC_LEN	4	/* Length of CRC on Ethernet frame	*/

#define	ETH_MAX_PKT_LEN	( ETH_HDR_LEN + ETH_VLAN_LEN + ETH_MTU )

#define	ETH_RX_BUF_SIZE	( ETH_MAX_PKT_LEN + ETH_CRC_LEN \
				+ sizeof(struct rxHeader) )

#define	ETH_TX_BUF_SIZE	( ETH_MAX_PKT_LEN )

/* State of the Ethernet interface */

#define	ETH_STATE_FREE	0	/* control block is unused */
#define	ETH_STATE_DOWN	1	/* interface is currently inactive */
#define	ETH_STATE_UP	2	/* interface is currently active */

/* Ethernet device control functions */

#define	ETH_CTRL_CLEAR_STATS 1  /* Reset Ethernet Statistics		*/
#define	ETH_CTRL_SET_MAC     2  /* Set the MAC for this device		*/
#define	ETH_CTRL_GET_MAC     3  /* Get the MAC for this device		*/
#define	ETH_CTRL_SET_LOOPBK  4  /* Set Loopback Mode			*/
#define	ETH_CTRL_RESET       5  /* Reset the Ethernet device		*/
#define	ETH_CTRL_DISABLE     6  /* Disable the Ethernet device		*/

/* Ethernet packet buffer */

struct	ethPktBuffer {
	byte	*buf;		/* Pointer to a packet buffer		*/
	byte	*data;		/* Start of data within the buffer	*/
	int32	length;		/* Length of data in the packet buffer	*/
};

/* Ethernet control block */

#define	ETH_INVALID	(-1)	/* Invalid data (virtual devices)	*/

struct	ether	{
	byte	state;		/* ETH_STATE_... as defined above	*/
	struct	dentry	*phy;	/* physical ethernet device for Tx DMA	*/

	/* Pointers to associated structures */

	struct	dentry	*dev;	/* address in device switch table	*/
	void	*csr;		/* addr.of control and status regs.	*/

	uint32	interruptMask;	/* interrupt mask			*/
	uint32	interruptStatus;/* interrupt status			*/

	struct	dmaDescriptor *rxRing;/* array of receive ring descrip.	*/
	struct	ethPktBuffer **rxBufs;/* Rx ring array			*/
	uint32	rxHead;		/* Index of current head of Rx ring	*/
	uint32	rxTail;		/* Index of current tail of Rx ring	*/
	uint32	rxRingSize;	/* size of Rx ring descriptor array	*/
	uint32	rxirq;		/* Count of Rx interrupt requests	*/
	uint32	rxOffset;	/* Size in bytes of rxHeader		*/
	uint32	rxErrors;	/* Count of Rx errors			*/

	struct	dmaDescriptor *txRing;/* array of transmit ring descrip.*/
	struct	ethPktBuffer **txBufs;/* Tx ring array			*/
	uint32	txHead;		/* Index of current head of Tx ring	*/
	uint32	txTail;		/* Index of current tail of Tx ring	*/
	uint32	txRingSize;	/* size of Tx ring descriptor array	*/
	uint32	txirq;		/* Count of Tx interrupt requests	*/

	byte	devAddress[ETH_ADDR_LEN]; /* MAC address */

	byte	addressLength;	/* Hardware address length		*/
	uint16	mtu;		/* Maximum transmission unit (payload)	*/

	uint32	errors;		/* Number of Ethernet errors		*/
	uint16	ovrrun;		/* Buffer overruns			*/
	sid32	isema;		/* I/0 semaphore for Ethernet input	*/
	uint16	istart;		/* Index of packet in the input buffer	*/
	uint16	icount;		/* Count of packets in the input buffer	*/

	struct	ethPktBuffer *in[ETH_IBUFSIZ]; /* Input buffer		*/

	int	inPool;		/* Buffer pool ID for input buffers	*/
	int	outPool;	/* Buffer pool ID for output buffers	*/
};
extern	struct	ether	ethertab[];	/* array of control blocks */

int32	colon2mac(char *, byte *);
int32	allocRxBuffer(struct ether *, int32);
int32	waitOnBit(volatile uint32 *, uint32, const int32, int32);
