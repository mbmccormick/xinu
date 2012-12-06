/* ethernet.h */

/* Ether Packet Types */
#define ETHER_TYPE_IPv4   0x0800
#define ETHER_TYPE_ARP    0x0806

#define ETH_ADDR_LEN	6
#define ETH_HDR_LEN		14

/*
 * ETHERNET HEADER
 *
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Destination MAC                                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Destination MAC               | Source MAC                    |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Source MAC                                                    |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Ether Type                    | Data                          |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Data (46 - 1500 octets)                                       |
 * | ...                                                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

/* Ethernet packet header */
struct etherPkt
{
    byte	dst[ETH_ADDR_LEN];    /* Destination Mac */
    byte	src[ETH_ADDR_LEN];    /* Source Mac      */
    uint16	type;		/* Ether type      */


    byte	data[1];              /* Packet data     */
};
