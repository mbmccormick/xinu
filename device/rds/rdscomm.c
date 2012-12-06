/* rdscomm.c  -  rdscomm */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rdscomm - handle communication with a remote disk server (send a
 *		request and receive a reply, including sequencing and
 *		retries)
 *------------------------------------------------------------------------
 */
status	rdscomm (
	  struct rd_msg_hdr *msg,	/* message to send		*/
	  int32		    mlen,	/* message length		*/
	  struct rd_msg_hdr *reply,	/* buffer for reply		*/
	  int32		    rlen,	/* size of reply buffer		*/
	  struct rdscblk    *rdptr	/* ptr to device control block	*/
	)
{
	int32	i;			/* counts retries		*/
	int32	retval;			/* return value			*/
	int32	seq;			/* sequence for this exchange	*/
	uint32	localip;		/* local IP address		*/
	int16	rtype;			/* reply type in host byte order*/
	bool8	xmit;			/* Should we transmit again?	*/

	/* For the first time after reboot, register the server port */

	if ( ! rdptr->rd_registered ) {
		retval = udp_register(0, rdptr->rd_ser_port,
                                rdptr->rd_loc_port);
		rdptr->rd_registered = TRUE;
	}

	if ( (int32)(localip = getlocalip()) == SYSERR ) {
		return SYSERR;
	}

	/* Assign message next sequence number */

	seq = rdptr->rd_seq++;
	msg->rd_seq = htonl(seq);

	/* Repeat RD_RETRIES times: send message and receive reply */

	xmit = TRUE;
	for (i=0; i<RD_RETRIES; i++) {
	    if (xmit) {

		/* Send a copy of the message */

		retval = udp_send(rdptr->rd_ser_ip, rdptr->rd_ser_port,
			localip, rdptr->rd_loc_port, (char *)msg, mlen);
		if (retval == SYSERR) {
			kprintf("Cannot send to remote disk server\n\r");
			return SYSERR;
		}
	    } else {
		xmit = TRUE;
	    }

	    /* Receive a reply */

	    retval = udp_recv(0, rdptr->rd_ser_port,
		rdptr->rd_loc_port, (char *)reply, rlen,
		RD_TIMEOUT);

	    if (retval == TIMEOUT) {
		continue;
	    } else if (retval == SYSERR) {
		kprintf("Error reading remote disk reply\n\r");
		return SYSERR;
	    }

	    /* Verify that sequence in reply matches request */

		
	    if (ntohl(reply->rd_seq) < seq) {
		xmit = FALSE;
	    } else if (ntohl(reply->rd_seq) != seq) {
			continue;
	    }

	    /* Verify the type in the reply matches the request */

	    rtype = ntohs(reply->rd_type);
	    if (rtype != ( ntohs(msg->rd_type) | RD_MSG_RESPONSE) ) {
		continue;
	    }

	    /* Check the status */

	    if (ntohs(reply->rd_status) != 0) {
		return SYSERR;
	    }

	    return OK;
	}

	/* Retries exhausted without success */

	kprintf("Timeout on exchange with remote disk server\n\r");
	return TIMEOUT;
}
