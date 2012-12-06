/* rfsControl.c - rfsControl */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rfsControl - Provide control functions for the remote file system
 *------------------------------------------------------------------------
 */
devcall	rfsControl (
	 struct dentry	*devptr,	/* entry in device switch table	*/
	 int32	func,			/* a control function		*/
	 int32	arg1,			/* argument #1			*/
	 int32	arg2			/* argument #2			*/
	)
{
	int32	len;			/* length of name		*/
	struct	rf_msg_sreq msg;	/* buffer for size request	*/
	struct	rf_msg_sres resp;	/* buffer for size response	*/
	struct	rflcblk	*rfptr;		/* pointer to entry in rfltab	*/
	char	*to, *from;		/* used during name copy	*/
	int32	retval;			/* return value			*/

	/* Wait for exclusive access */

	wait(Rf_data.rf_mutex);

	/* Check length and copy (needed for size) */

	rfptr = &rfltab[devptr->dvminor];
	from = rfptr->rfname;
	to = msg.rf_name;
	len = 0;
	memset(to, NULLCH, RF_NAMLEN);	/* start name as all zeroes	*/
	while ( (*to++ = *from++) ) {	/* copy name to message		*/
		len++;
		if (len >= (RF_NAMLEN - 1) ) {
			signal(Rf_data.rf_mutex);
			return SYSERR;
		}
	}

	switch (func) {

	/* Delete a file */

	case RFS_CTL_DEL:
		if (rfsndmsg(RF_MSG_DREQ, (char *)arg1) == SYSERR) {
			signal(Rf_data.rf_mutex);
			return SYSERR;
		}
		break;

	/* Truncate a file */

	case RFS_CTL_TRUNC:
		if (rfsndmsg(RF_MSG_TREQ, (char *)arg1) == SYSERR) {
			signal(Rf_data.rf_mutex);
			return SYSERR;
		}
		break;



	/* Make a directory */

	case RFS_CTL_MKDIR:
		if (rfsndmsg(RF_MSG_MREQ, (char *)arg1) == SYSERR) {
			signal(Rf_data.rf_mutex);
			return SYSERR;
		}
		break;

	/* Remove a directory */

	case RFS_CTL_RMDIR:
		if (rfsndmsg(RF_MSG_XREQ, (char *)arg1) == SYSERR) {
			signal(Rf_data.rf_mutex);
			return SYSERR;
		}
		break;

	/* Obtain current file size (non-standard message size) */

	case RFS_CTL_SIZE:

		/* Hand-craft a size request message */

		msg.rf_type = htons(RF_MSG_SREQ);
		msg.rf_status = htons(0);
		msg.rf_seq = 0;		/* rfscomm will set the seq num	*/

		/* Send the request to server and obtain a response	*/

		retval = rfscomm( (struct rf_msg_hdr *)&msg,
					sizeof(struct rf_msg_sreq),
				  (struct rf_msg_hdr *)&resp,
					sizeof(struct rf_msg_sres) );
		if ( (retval == SYSERR) || (retval == TIMEOUT) ) {
			signal(Rf_data.rf_mutex);
			return SYSERR;
		} else {
			signal(Rf_data.rf_mutex);
			return ntohl(resp.rf_size);
		}

	default:
		kprintf("rfsControl: function %d not valid\n\r", func);
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	signal(Rf_data.rf_mutex);
	return OK;
}
