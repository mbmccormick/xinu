/* xsh_ethstat.c - xsh_ethstat */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

extern void ethStat (uint16 minor);

/*------------------------------------------------------------------------
 * xsh_ethstat - shell command to display status of an Ethernet device
 *------------------------------------------------------------------------
 */
shellcmd xsh_ethstat(int nargs, char *args[])
{

	/* For argument '--help', emit help about the 'sthstat' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tDisplays information about the Ethernet device\n");
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
	ethStat(0);

	return 0;
}
