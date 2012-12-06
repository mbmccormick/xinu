/* xsh_ipaddr.c - xsh_ipaddr */

#include <xinu.h>
#include <string.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xsh_ipaddr - obtain and print the IP address, subnet mask and default
 *			router address
 *------------------------------------------------------------------------
 */
shellcmd xsh_ipaddr(int nargs, char *args[]) {

	int32	retval;

	/* Output info for '--help' argument */

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Usage: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tDisplays IP address information\n");
		printf("Options:\n");
		printf("\t-f\tforce a new DHCP request to be sent\n");
		printf("\t--help\tdisplay this help and exit\n");
		return OK;
	}

	/* Check argument count */

	if (nargs > 2) {
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
			args[0]);
		return SYSERR;
	}

	if (nargs == 2) {
		if (strncmp(args[1], "-f", 3) != 0) {
			fprintf(stderr, "%s: invalid argument\n");
			fprintf(stderr,
				"Try '%s --help' for more information\n",
				args[0]);
			return 1;
		}
		NetData.ipvalid = FALSE;
	}

	retval = getlocalip();
	if (retval == SYSERR) {
		fprintf(stderr,
			"%s: could not obtain an IP address\n",
			args[0]);
		return 1;
	}
	printf("IP address:\t%d.%d.%d.%d\t0x%08x\n",
		(NetData.ipaddr>>24)&0xff, (NetData.ipaddr>>16)&0xff,
		(NetData.ipaddr>>8)&0xff,        NetData.ipaddr&0xff,
		NetData.ipaddr);

	printf("Subnet mask:\t%d.%d.%d.%d\t0x%08x\n",
		(NetData.addrmask>>24)&0xff, (NetData.addrmask>>16)&0xff,
		(NetData.addrmask>> 8)&0xff,       NetData.addrmask&0xff,
		NetData.addrmask);

	printf("Router IP:\t%d.%d.%d.%d\t0x%08x\n",
		(NetData.routeraddr>>24)&0xff, (NetData.routeraddr>>16)&0xff,
		(NetData.routeraddr>> 8)&0xff,       NetData.routeraddr&0xff,
		NetData.routeraddr);
	return OK;
}
