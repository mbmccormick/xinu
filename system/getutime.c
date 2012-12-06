/* getutime.c - getutime */

#include <xinu.h>

/*------------------------------------------------------------------------
 * getutime  --  obtain time in seconds past Jan 1, 1970, UCT (GMT)
 *------------------------------------------------------------------------
 */
status	getutime(
	  uint32  *timvar		/* location to store the result	*/
	)
{
	uint32	nnow;			/* current time in network fmt	*/
	uint32	now;			/* current time in xinu format	*/
	int32	retval;			/* return value from call	*/
	uint32	serverip;		/* IP address of a time server	*/
	char	prompt[2] = "xx";	/* message to prompt time server*/

	if (Date.dt_bootvalid) {	/* return time from local info	*/
		*timvar = Date.dt_boot + clktime;
		return OK;
	}

	/* Convert time server IP address to binary */

	if (dot2ip(TIMESERVER, &serverip) == SYSERR) {
		return SYSERR;
	}

	/* Contact the time server to get the date and time */

	if (udp_register(serverip,TIMERPORT,TIMELPORT) == SYSERR) {
		return SYSERR;
	}

	/* send arbitrary message to prompt time server */

	if (getlocalip() == SYSERR) {
		return SYSERR;
	}
	retval = udp_send(serverip, TIMERPORT, NetData.ipaddr, TIMELPORT,
			prompt, 2);
	if (retval == SYSERR) {
		udp_release(serverip,TIMERPORT,TIMELPORT);
		return SYSERR;
	}

	retval = udp_recv(serverip, TIMERPORT, TIMELPORT, (char *) &nnow,
		4, TIMETIMEOUT);
	if ( (retval == SYSERR) || (retval == TIMEOUT) ) {
		udp_release(serverip,TIMERPORT,TIMELPORT);
		return SYSERR;
	}
	udp_release(serverip,TIMERPORT,TIMELPORT);
	now = ntim2xtim( ntohl(nnow) );
	Date.dt_boot = now - clktime;
	Date.dt_bootvalid = TRUE;
	*timvar = now;
	return OK;
}
