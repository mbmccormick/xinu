/* rfscomm.c - rfscomm */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rfscomm - handle communication with RFS server (send request and
 *		receive a reply, including sequencing and retries)
 *------------------------------------------------------------------------
 */
int32	rfscomm (
	 struct	rf_msg_hdr *msg,	/* message to send		*/
	 int32	mlen,			/* message length		*/
	 struct	rf_msg_hdr *reply,	/* buffer for reply		*/
	 int32	rlen			/* size of reply buffer		*/
	)
{
	int32	i;			/* counts retries		*/
	int32	retval;			/* return value			*/
	int32	seq;			/* sequence for this exchange	*/
	int16	rtype;			/* reply type in host byte order*/

	/* For the first time after reboot, register the server port */

	if ( ! Rf_data.rf_registered ) {
		retval = udp_register(0, Rf_data.rf_ser_port,
                                Rf_data.rf_loc_port);
		Rf_data.rf_registered = TRUE;
	}

	/* Assign message next sequence number */

	seq = Rf_data.rf_seq++;
	msg->rf_seq = htonl(seq);

	/* Repeat RF_RETRIES times: send message and receive reply */

	for (i=0; i<RF_RETRIES; i++) {

		/* Send a copy of the message */

		retval = udp_send(Rf_data.rf_ser_ip, Rf_data.rf_ser_port,
			NetData.ipaddr, Rf_data.rf_loc_port, (char *)msg,
			mlen);
		if (retval == SYSERR) {
			kprintf("Cannot send to remote file server\n\r");
			return SYSERR;
		}

		/* Receive a reply */

		retval = udp_recv(0, Rf_data.rf_ser_port,
			Rf_data.rf_loc_port, (char *)reply, rlen,
			RF_TIMEOUT);

		if (retval == TIMEOUT) {
			continue;
		} else if (retval == SYSERR) {
			kprintf("Error reading remote file reply\n\r");
			return SYSERR;
		}

		/* Verify that sequence in reply matches request */

		if (ntohl(reply->rf_seq) != seq) {
			continue;
		}

		/* Verify the type in the reply matches the request */

		rtype = ntohs(reply->rf_type);
		if (rtype != ( ntohs(msg->rf_type) | RF_MSG_RESPONSE) ) {
			continue;
		}

		return retval;		/* return length to caller */
	}

	/* Retries exhausted without success */

	kprintf("Timeout on exchange with remote file server\n\r");
	return TIMEOUT;
}
