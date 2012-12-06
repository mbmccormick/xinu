/* rfsOpen.c - rfsOpen */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rfsOpen - allocate a remote file pseudo-device for a specific file
 *------------------------------------------------------------------------
 */

devcall	rfsOpen (
	 struct	dentry	*devptr,	/* entry in device switch table	*/
	 char	*name,			/* file name to use		*/
	 char	*mode			/* mode chars: 'r' 'w' 'o' 'n'	*/
	)
{
	struct	rflcblk	*rfptr;		/* ptr to control block entry	*/
	struct	rf_msg_oreq msg;	/* message to be sent		*/
	struct	rf_msg_ores resp;	/* buffer to hold response	*/
	int32	retval;			/* return value from rfscomm	*/
	int32	len;			/* counts chars in name		*/
	char	*nptr;			/* pointer into name string	*/
	char	*fptr;			/* pointer into file name	*/
	int32	i;			/* general loop index		*/

	/* Wait for exclusive access */

	wait(Rf_data.rf_mutex);

	/* Search control block array to find a free entry */

	for(i=0; i<Nrfl; i++) {
		rfptr = &rfltab[i];
		if (rfptr->rfstate == RF_FREE) {
			break;
		}
	}
	if (i >= Nrfl) {		/* No free table slots remain	*/
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Copy name into free table slot */

	nptr = name;
	fptr = rfptr->rfname;
	len = 0;
	while ( (*fptr++ = *nptr++) != NULLCH) {
		len++;
		if (len >= RF_NAMLEN) {	/* File name is too long	*/
			signal(Rf_data.rf_mutex);
			return SYSERR;
		}
	}

	/* Verify that name is non-null */

	if (len==0) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Parse mode string */

	if ( (rfptr->rfmode = rfsgetmode(mode)) == SYSERR ) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Form an open request to create a new file or open an old one */

	msg.rf_type = htons(RF_MSG_OREQ);/* Request a file open		*/
	msg.rf_status = htons(0);
	msg.rf_seq = 0;			/* rfscomm fills in seq. number	*/
	nptr = msg.rf_name;
	memset(nptr, NULLCH, RF_NAMLEN);/* initialize name to zero bytes*/
	while ( (*nptr++ = *name++) != NULLCH ) { /* copy name to req.	*/
		;
	}
	msg.rf_mode = htonl(rfptr->rfmode); /* Set mode in request	*/

	/* Send message and receive response */

	retval = rfscomm((struct rf_msg_hdr *)&msg,
					sizeof(struct rf_msg_oreq),
			 (struct rf_msg_hdr *)&resp,
					sizeof(struct rf_msg_ores) );

	/* Check response */

	if (retval == SYSERR) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	} else if (retval == TIMEOUT) {
		kprintf("Timeout during remote file open\n\r");
		signal(Rf_data.rf_mutex);
		return SYSERR;
	} else if (ntohs(resp.rf_status) != 0) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Set initial file position */

	rfptr->rfpos = 0;

	/* Mark state as currently used */

	rfptr->rfstate = RF_USED;

	/* Return device descriptor of newly created pseudo-device */

	signal(Rf_data.rf_mutex);
	return rfptr->rfdev;
}
