/* rdsprocess.c  -  rdsprocess */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rdsprocess - high-priority background process that repeatedly extracts
 *		an item from the request queue and sends the request to
 *		the remote disk server
 *------------------------------------------------------------------------
 */
void	rdsprocess (
	  struct rdscblk    *rdptr	/* ptr to device control block	*/
	)
{
	struct	rd_msg_wreq msg;	/* message to be sent		*/
					/*  (includes data area)	*/
	struct	rd_msg_rres resp;	/* buffer to hold response	*/
					/*  (includes data area)	*/
	int32	retval;			/* return value from rdscomm	*/
	char	*idto;			/* ptr to ID string copy	*/
	char	*idfrom;		/* ptr into ID string		*/
	struct	rdbuff	*bptr;		/* ptr to buffer at the head of	*/
					/*   the request queue		*/
	struct	rdbuff	*nptr;		/* ptr to next buffer on the	*/
					/*   request queue		*/
	struct	rdbuff	*pptr;		/* ptr to previous buffer	*/
	struct	rdbuff	*qptr;		/* ptr that runs along the	*/
					/*   request queue		*/
	int32	i;			/* loop index			*/

	while (TRUE) {			/* do forever */

	    /* Wait until the request queue contains a node */
	    wait(rdptr->rd_reqsem);
	    bptr = rdptr->rd_rhnext;

	    /* Use operation in request to determine action */

	   switch (bptr->rd_op) {

	   case RD_OP_READ:

		/* Build a read request message for the server */

		msg.rd_type = htons(RD_MSG_RREQ);	/* read request	*/
		msg.rd_status = htons(0);
		msg.rd_seq = 0;		/* rdscomm fills in an entry	*/
		idto = msg.rd_id;
		memset(idto, NULLCH, RD_IDLEN);/* initialize ID to zero	*/
		idfrom = rdptr->rd_id;
		while ( (*idto++ = *idfrom++) != NULLCH ) { /* copy ID	*/
			;
		}

		/* Send the message and receive a response */

		retval = rdscomm((struct rd_msg_hdr *)&msg,
					sizeof(struct rd_msg_rreq),
				 (struct rd_msg_hdr *)&resp,
					sizeof(struct rd_msg_rres),
				  rdptr );

		/* Check response */

		if ( (retval == SYSERR) || (retval == TIMEOUT) ||
				(ntohs(resp.rd_status) != 0) ) {
			panic("Failed to contact remote disk server");
		}

		/* Copy data from the reply into the buffer */

		for (i=0; i<RD_BLKSIZ; i++) {
			bptr->rd_block[i] = resp.rd_data[i];
		}

		/* Unlink buffer from the request queue */

		nptr = bptr->rd_next;
		pptr = bptr->rd_prev;
		nptr->rd_prev = bptr->rd_prev;
		pptr->rd_next = bptr->rd_next;

		/* Insert buffer in the cache */

		pptr = (struct rdbuff *) &rdptr->rd_chnext;
		nptr = pptr->rd_next;
		bptr->rd_next = nptr;
		bptr->rd_prev = pptr;
		pptr->rd_next = bptr;
		nptr->rd_prev = bptr;

		/* Initialize reference count */

		bptr->rd_refcnt = 1;

		/* Signal the available semaphore */

		signal(rdptr->rd_availsem);

		/* Send a message to waiting process */

		send(bptr->rd_pid, (uint32)bptr);

		/* If other processes are waiting to read the  */
		/*   block, notify them and remove the request */

		qptr = rdptr->rd_rhnext;
		while (qptr != (struct rdbuff *)&rdptr->rd_rtnext) {
			if (qptr->rd_blknum == bptr->rd_blknum) {
				bptr->rd_refcnt++;
				send(qptr->rd_pid,(uint32)bptr);

				/* Unlink request from queue	*/

				pptr = qptr->rd_prev;
				nptr = qptr->rd_next;
				pptr->rd_next = bptr->rd_next;
				nptr->rd_prev = bptr->rd_prev;

				/* Move buffer to the free list	*/

				qptr->rd_next = rdptr->rd_free;
				rdptr->rd_free = qptr;
				signal(rdptr->rd_availsem);
				break;
			}
			qptr = qptr->rd_next;
		}
		break;

	    case RD_OP_WRITE:

		/* Build a write request message for the server */

		msg.rd_type = htons(RD_MSG_WREQ);	/* write request*/
		msg.rd_blk = bptr->rd_blknum;
		msg.rd_status = htons(0);
		msg.rd_seq = 0;		/* rdscomm fills in an entry	*/
		idto = msg.rd_id;
		memset(idto, NULLCH, RD_IDLEN);/* initialize ID to zero	*/
		idfrom = rdptr->rd_id;
		while ( (*idto++ = *idfrom++) != NULLCH ) { /* copy ID	*/
			;
		}
		for (i=0; i<RD_BLKSIZ; i++) {
			msg.rd_data[i] = bptr->rd_block[i];
		}

		/* Unlink buffer from request queue */

		nptr = bptr->rd_next;
		pptr = bptr->rd_prev;
		pptr->rd_next = nptr;
		nptr->rd_prev = pptr;

		/* Insert buffer in the cache */

		pptr = (struct rdbuff *) &rdptr->rd_chnext;
		nptr = pptr->rd_next;
		bptr->rd_next = nptr;
		bptr->rd_prev = pptr;
		pptr->rd_next = bptr;
		nptr->rd_prev = bptr;

		/* Declare that buffer is eligible for reuse */

		bptr->rd_refcnt = 0;
		signal(rdptr->rd_availsem);

		/* Send the message and receive a response */

		retval = rdscomm((struct rd_msg_hdr *)&msg,
					sizeof(struct rd_msg_wreq),
				 (struct rd_msg_hdr *)&resp,
					sizeof(struct rd_msg_wres),
				  rdptr );

		/* Check response */

		if ( (retval == SYSERR) || (retval == TIMEOUT) ||
				(ntohs(resp.rd_status) != 0) ) {
			panic("failed to contact remote disk server");
		}
		break;

	    case RD_OP_SYNC:

		/* Send a message to the waiting process */

		send(bptr->rd_pid, OK);

		/* Unlink buffer from the request queue */

		nptr = bptr->rd_next;
		pptr = bptr->rd_prev;
		nptr->rd_prev = bptr->rd_prev;
		pptr->rd_next = bptr->rd_next;

		/* Insert buffer into the free list */

		bptr->rd_next = rdptr->rd_free;
		rdptr->rd_free = bptr;
		signal(rdptr->rd_availsem);
		break;
	   }
	}
}
