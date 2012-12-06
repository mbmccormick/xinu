/* rflWrite.c - rflWrite */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rflWrite - Write data to a remote file
 *------------------------------------------------------------------------
 */
devcall	rflWrite (
	  struct dentry	*devptr,	/* entry in device switch table	*/
	  char	*buff,			/* buffer of bytes		*/
	  int32	count 			/* count of bytes to write	*/
	)
{
	struct	rflcblk	*rfptr;		/* pointer to control block	*/
	int32	retval;			/* return value			*/
	struct	rf_msg_wreq  msg;	/* request message to send	*/
	struct	rf_msg_wres resp;	/* buffer for response		*/
	char	*from, *to;		/* used to copy name		*/
	int	i;			/* counts bytes copied into req	*/
	int32	len;			/* length of name		*/

	/* Wait for exclusive access */

	wait(Rf_data.rf_mutex);

	/* Verify count is legitimate */

	if ( (count <= 0) || (count > RF_DATALEN) ) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Verify pseudo-device is in use and mode allows writing */

	rfptr = &rfltab[devptr->dvminor];
	if ( (rfptr->rfstate == RF_FREE) ||
	     ! (rfptr->rfmode & RF_MODE_W) ) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Form write request */

	msg.rf_type = htons(RF_MSG_WREQ);
	msg.rf_status = htons(0);
	msg.rf_seq = 0;			/* rfscomm will set sequence	*/
	from = rfptr->rfname;
	to = msg.rf_name;
	memset(to, NULLCH, RF_NAMLEN);	/* start name as all zero bytes	*/
	len = 0;
	while ( (*to++ = *from++) ) {	/* copy name to request		*/
		if (++len >= RF_NAMLEN) {
			signal(Rf_data.rf_mutex);
			return SYSERR;
		}
	}
	while ( (*to++ = *from++) ) {	/* copy name into request	*/
		;
	}
	msg.rf_pos = htonl(rfptr->rfpos);/* set file position		*/
	msg.rf_len = htonl(count);	/* set count of bytes to write	*/
	for (i=0; i<count; i++) {	/* copy data into message	*/
		msg.rf_data[i] = *buff++;
	}
	while (i < RF_DATALEN) {
		msg.rf_data[i++] = NULLCH;
	}

	/* Send message and receive response */

	retval = rfscomm((struct rf_msg_hdr *)&msg,
					sizeof(struct rf_msg_wreq),
			 (struct rf_msg_hdr *)&resp,
				sizeof(struct rf_msg_wres) );

	/* Check response */

	if (retval == SYSERR) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	} else if (retval == TIMEOUT) {
		kprintf("Timeout during remote file read\n\r");
		signal(Rf_data.rf_mutex);
		return SYSERR;
	} else if (ntohs(resp.rf_status) != 0) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Report results to caller */

	rfptr->rfpos += ntohl(resp.rf_len);

	signal(Rf_data.rf_mutex);
	return ntohl(resp.rf_len);
}
