/* xsh_arp.c - xsh_arp */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

/*------------------------------------------------------------------------
 * xsh_arp - display the current ARP cache
 *------------------------------------------------------------------------
 */
shellcmd xsh_arp(int nargs, char *args[])
{
	int32	i, j;			/* index into the ARP table	*/

	/* For argument '--help', emit help about the 'arp' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tDisplays information from the ARP cache\n");
		printf("Options:\n");
		printf("\t--help\t display this help and exit\n");
		return 0;
	}

	/* Check for valid number of arguments */

	if (nargs > 1) {
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
		return 1;
	}

	/* Print entries from the ARP table */

	printf("State Pid    IP Address    Hardware Address\n");
	printf("----- --- --------------- -----------------\n");
	for (i = 0; i < ARP_SIZ; i++) {
		if (arpcache[i].arstate == AR_FREE) {
			continue;
		}
		switch(arpcache[i].arstate) {
		    case AR_PENDING:	printf("PEND "); break;
		    case AR_RESOLVED:	printf("RESLV"); break;
		    default:		printf("?????"); break;
		}
		if (arpcache[i].arstate == AR_PENDING) {
			printf("%4d ", arpcache[i].arpid);
		} else {
			printf("     ");
		}
		printf("%3d.", (arpcache[i].arpaddr & 0xFF000000) >> 24);
		printf("%3d.", (arpcache[i].arpaddr & 0x00FF0000) >> 16);
		printf("%3d.", (arpcache[i].arpaddr & 0x0000FF00) >> 8);
		printf("%3d",  (arpcache[i].arpaddr & 0x000000FF));

		printf(" %02X", arpcache[i].arhaddr[0]);
		for (j = 1; j < ARP_HALEN; j++) {
			printf(":%02X", arpcache[i].arhaddr[j]);
		}
		printf("\n");
	}
	return 0;
}
