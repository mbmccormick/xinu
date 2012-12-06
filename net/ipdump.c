/* hexdump.c - hexdump */

#include <xinu.h>

/*------------------------------------------------------------------------
 * ipdump - dump an Ethernet/IP/UDP packet in hexadecimal
 *------------------------------------------------------------------------
 */
void	ipdump	(
	  char *p, int32 len		/* ptr to packet and length of	*/
					/*  payload (IP datagram)	*/
	)
{
	int32	i;			/* counts bytes			*/
	int32	word;			/* counts 32 bits words		*/

	for (i=0; i<6; i++) {
		kprintf("%02x", 0xff & *p++);
	}
	kprintf("  ");

	for (i=0; i<6; i++) {
		kprintf("%02x", 0xff & *p++);
	}
	kprintf("  ");

	for (i=0; i<2; i++) {
		kprintf("%02x", 0xff & *p++);
	}
	kprintf("  End Ethernet header\r\n");

	for (word=0,i=0; i<len; i++) {
		if ((i % 4) == 0) {
			word++;
			if (word == 6) {
				kprintf("    HV TOS LENGTH    ID FRAG    TTL PRO CKSUM    IP_SRC IP_DST\r\n");
			}
			if (word==8) {
				kprintf("       SRC_PRT DEST-PORT    LENGTH CKSUM\r\n");
				word = 16;
			} else if ((word % 16) ==0) {
				kprintf("\r\n");
			}
			kprintf(" ");
		}
		kprintf("%02x", 0xff & *p++);
	}
	kprintf("\r\n");
	return;
}
