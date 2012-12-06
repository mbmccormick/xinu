/* xsh_udpecho.c - xsh_udpecho */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

/*------------------------------------------------------------------------
 * xsh_udpecho - shell command that can send a message to a remote UDP
 *			echo server and receive a reply
 *------------------------------------------------------------------------
 */
shellcmd xsh_udpecho(int nargs, char *args[])
{
	int	i;			/* index into buffer		*/
	int	retval;			/* return value			*/
	char	msg[] = "Xinu testing UDP echo"; /* message to send	*/
	char	inbuf[1500];		/* buffer for incoming reply	*/
	int32	msglen;			/* length of outgoing message	*/
	uint32	remoteip;		/* remote IP address to use	*/
	uint32	localip;		/* local IP address to use	*/
	uint16	echoport= 7;		/* port number for UDP echo	*/
	uint16	locport	= 52743;	/* local port to use		*/
	int32	retries	= 3;		/* number of retries		*/
	int32	delay	= 2000;		/* reception delay in ms	*/

	/* For argument '--help', emit help about the 'udpecho' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s  REMOTEIP\n\n", args[0]);
		printf("Description:\n");
		printf("\tBounce a message off a remote UDP echo server\n");
		printf("Options:\n");
		printf("\tREMOTEIP:\tIP address in dotted decimal\n");
		printf("\t--help\t display this help and exit\n");
		return 0;
	}

	/* Check for valid IP address argument */

	if (nargs != 2) {
		fprintf(stderr, "%s: invalid argument(s)\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
		return 1;
	}

	if (dot2ip(args[1], &remoteip) == SYSERR) {
		fprintf(stderr, "%s: invalid IP address argument\r\n",
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

	retval = udp_register(remoteip, echoport, locport);
	if (retval == SYSERR) {
		fprintf(stderr, "%s: could not reserve UDP port %d\n",
				args[0], locport);
		return 1;
	}

	/* Retry sending outgoing datagram and getting response */

	for (i=0; i<retries; i++) {
		msglen = strnlen(msg, 1500);
		retval = udp_send(remoteip, echoport, localip, locport,
			msg, msglen);
		if (retval == SYSERR) {
			fprintf(stderr, "%s: error sending UDP \n",
				args[0]);
			return 1;
		}

		retval = udp_recv(remoteip, echoport, locport, inbuf,
			sizeof(inbuf), delay);
		if (retval == TIMEOUT) {
			fprintf(stderr, "%s: timeout...\n", args[0]);
			continue;
		} else if (retval == SYSERR) {
			fprintf(stderr, "%s: error from udp_recv \n",
				args[0]);
			udp_release(remoteip, echoport, locport);
			return 1;
		}
		break;
	}

	udp_release(remoteip, echoport, locport);
	if (retval == TIMEOUT) {
		fprintf(stderr, "%s: retry limit exceeded\n",
			args[0]);
		return 1;
	}

	/* Response received - check contents */

	if (retval != msglen) {
		fprintf(stderr, "%s: sent %d bytes and received %d\n",
			args[0], msglen, retval);
		return 1;
	}
	for (i = 0; i < msglen; i++) {
		if (msg[i] != inbuf[i]) {
			fprintf(stderr, "%s: reply differs at byte %d\n",
				args[0], i);
			return 1;
		}
	}

	printf("UDP echo test was successful\n");
	return 0;
}
