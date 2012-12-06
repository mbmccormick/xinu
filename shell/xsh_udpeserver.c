/* xsh_udpeserver.c - xsh_udpeserver */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

/*------------------------------------------------------------------------
 * xsh_udpeserver - shell command that acts as a UDP echo server (is
 *			usually run in background)
 *------------------------------------------------------------------------
 */
shellcmd xsh_udpeserver(int nargs, char *args[])
{
	int32	retval;			/* return value			*/
	uint32	localip;		/* local IP address		*/
	uint32	remip;			/* remote sender's IP address	*/
	uint16	remport;		/* remote sender's UDP port	*/
	char	buff[1500];		/* buffer for incoming reply	*/
	int32	msglen;			/* length of outgoing message	*/
	uint16	echoserverport= 7;	/* port number for UDP echo	*/

	/* For argument '--help', emit a help message	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tBecome a UDP echo server\n");
		printf("Options:\n");
		printf("\t--help\t display this help and exit\n");
		return 0;
	}

	/* Check for valid IP address argument */

	if (nargs != 1) {
		fprintf(stderr, "%s: no arguments expected\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
		return 1;
	}

	localip = getlocalip();
	if (localip == SYSERR) {
		fprintf(stderr,
			"%s: could not obtain a local IP address\n",
			args[0]);
		return 1;
	}

	/* register local UDP port */

	retval = udp_register(0, 0, echoserverport);
	if (retval == SYSERR) {
		fprintf(stderr, "%s: could not reserve UDP port %d\n",
				args[0], echoserverport);
		return 1;
	}

	/* Do forever: read an incoming datagram and send it back */

	while (TRUE) {
		retval = udp_recvaddr(&remip, &remport,	echoserverport,
				buff, sizeof(buff), 600000);

		if (retval == TIMEOUT) {
			continue;
		} else if (retval == SYSERR) {
			fprintf(stderr, "%s: error receiving UDP\n",
				args[0]);
			return 1;
		}
		msglen = retval;
		retval = udp_send(remip, remport, localip,
				echoserverport, buff, msglen);
		if (retval == SYSERR) {
			fprintf(stderr, "%s: udp_send failed\n",
				args[0]);
			return 1;
		}
	}
	return 0;
}
