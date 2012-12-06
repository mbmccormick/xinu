/* xsh_nvram.c - xsh_nvram */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

/*------------------------------------------------------------------------
 * xsh_nvram  -  shell command to list all items in NVRAM (Flash) or to
 *			display one item
 *------------------------------------------------------------------------
 */
shellcmd xsh_nvram(int nargs, char *args[])
{
	char	*value;			/* value to print		*/
	uint16	n;			/* iterates through items	*/
	uint16	count;			/* counts items found		*/
	struct nvram_tuple *tuple;

	/* insure nvram can be initialized */

	if (nvramInit() == SYSERR) {
		fprintf(stderr, "error: device does not appear to have nvram.\n");
		return 1;
	}

	/* For argument '--help', emit help about the 'nvram' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s OPTION \n\n", args[0]);
		printf("Description:\n");
		printf("%s,%s\n", "\tDisplays a list of items in NVRAM ",
				"or the value of an item");
		printf("Options:\n");
		printf("\tlist\t\t\tdisplay all <NAME>=<VALUE> tuples\n");
		printf("\tget <NAME>\t\tdisplay the value of <NAME>\n");
		printf("\t--help\t display this help and exit\n");
		return 0;
	}


	if ( (nargs==2) && (strncmp(args[1], "list", 5)==0) ) {
		count = 0;
		for (n = 0; n < NVRAM_NHASH; n++) {
			tuple = nvram_tuples[n];
			if (tuple == NULL) {
				continue;
			}
			do {
				printf("%s\n", tuple->pair);
				count++;
			} while ((tuple = tuple->next) != NULL);
		}
		printf("%d pairs occupt %d bytes (%d bytes remain unused)\n",
			   count, nvram_header->length,
			   NVRAM_SIZE - nvram_header->length);
			return 0;
	} else if ( (nargs == 3) && (strncmp(args[1], "get", 4) == 0) ) {
		value = nvramGet(args[2]);
		if (value != 0) {
			printf("%s\n", nvramGet(args[2]));
			return 0;
		} else {
			fprintf(stderr, "error: no NVRAM binding\n");
			return 1;
		}
	}
	fprintf(stderr, "%s: incorrect arguments\n", args[0]);
	fprintf(stderr, "Try '%s --help' for more information\n",
			args[0]);
	return 1;
}
