/* xsh_memdump.c - xsh_memdump */

#include <xinu.h>
#include <string.h>
#include <stdio.h>

static	uint32	parseval(char *);

/*------------------------------------------------------------------------
 * xsh_memdump - dump a region of memory by displaying values in hex
 *			 and ascii
 *------------------------------------------------------------------------
 */
shellcmd xsh_memdump(int nargs, char *args[])
{
	bool8	force = FALSE;		/* ignore address sanity checks	*/
	uint32	start;			/* starting address		*/
	uint32	length;			/* length of region to dump	*/
	int32	arg;			/* index into args array	*/
	uint32	l;			/* counts length during dump	*/
	int32	i;			/* counts words during dump	*/
	uint32	*addr;			/* address to dump		*/
	char	*chptr;			/* character address to dump	*/
	char	ch;			/* next character to print	*/

	/* For argument '--help', emit help about the 'memdump' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s [-f] START LENGTH\n\n", args[0]);
		printf("Description:\n");
		printf("\tDumps a LENGTH bytes of memory starting at\n");
		printf("\tthe specified START address (both START and\n");
		printf("\tLENGTH can be specified in decimal or hex)\n");
		printf("Options:\n");
		printf("\t-f\t\tignore sanity checks for addresses\n");
		printf("\tSTART\t\tmemory address at which to start\n");
		printf("\tLENGTH\tnumber of bytes to dump\n");
		printf("\t--help\t display this help and exit\n");
		return 0;
	}

	/* Check for valid number of arguments */

	if (nargs < 3 || nargs > 4) {
		fprintf(stderr, "%s: incorrect number of arguments\n",
				args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
		return 1;
	}

	arg = 1;
	if (strncmp(args[arg], "-f", 2) == 0) {
		force = TRUE;
		arg++;
		nargs --;
	}

	if (nargs != 3) {
		fprintf(stderr, "%s: too few arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
		return 1;
	}

	if ( (start=parseval(args[arg])) == 0 ) {
		fprintf(stderr, "%s: invalid starting address\n",
				args[0]);
		return 1;
	}
	if ( (length =parseval(args[arg+1])) == 0 ) {
		fprintf(stderr, "%s: invalid length address\n",
				args[0]);
		return 1;
	}

	/* Round starting address down to multiple of four and round	*/
	/*	length up to a multiple of four				*/

	start &= ~0x3;
	length = (length + 3) & ~0x3;

	/* verify that the address and length are reasonable */

	if (force || (KSEG0_BASE <= start && start < KSEG2_BASE)) {

		/* values are valid; perform dump */

		chptr = (char *)start;
		for (l=0; l<length; l+=16) {
			printf("%08x: ", start);
			addr = (uint32 *)start;
			for (i=0; i<4; i++) {
				printf("%08x ",*addr++);
			}
			printf("  *");
			for (i=0; i<16; i++) {
				ch = *chptr++;
				if ( (ch >= 0x20) && (ch <= 0x7e) ) {
					printf("%c",ch);
				} else {
					printf(".");
				}
			}
			printf("*\n");
			start += 16;
		}
		return 0;
	}
	printf("Values are out of range; use -f to force\n");
	return 1;
}

/*------------------------------------------------------------------------
 * parse - parse an argument that is either a decimal or hex value
 *------------------------------------------------------------------------
 */
static	uint32	parseval(
	  char	*string			/* argument string to parse	*/
	)
{
	uint32	value;			/* value to return		*/
	char	ch;			/* next character		*/
	

	value = 0;

	/* argument string must consists of decimal digits or	*/
	/*	0x followed by hex digits			*/

	ch = *string++;
	if (ch == '0') {		/* hexadecimal */
		if (*string++ != 'x') {
			return 0;
		}
		for (ch = *string++; ch != NULLCH; ch = *string++) {
			if ((ch >= '0') && (ch <= '9') ) {
				value = 16*value + (ch - '0');
			} else if ((ch >= 'a') && (ch <= 'f') ) {
				value = 16*value + 10 + (ch - 'a');
			} else if ((ch >= 'A') && (ch <= 'F') ) {
				value = 16*value + 10 + (ch - 'A');
			} else {
				return 0;
			}
		}
	} else {			/* decimal */
		while (ch != NULLCH) {
			if ( (ch < '0') || (ch > '9') ) {
				return 0;
			}
			value = 10*value + (ch - '0');
			ch = *string++;
		}
	}
	return value;
}
