/* rdsInit.c  -  rdsInit */

#include <xinu.h>

struct	rdscblk	rdstab[Nrds];

/*------------------------------------------------------------------------
 *  rdsInit - initialize the remote disk system device
 *------------------------------------------------------------------------
 */
devcall	rdsInit (
	  struct dentry	*devptr		/* entry in device switch table	*/
	)
{
	struct	rdscblk	*rdptr;		/* ptr to device contol block	*/
	struct	rdbuff	*bptr;		/* ptr to buffer in memory	*/
					/*    used to form linked list	*/
	struct	rdbuff	*pptr;		/* ptr to previous buff on list	*/
	struct	rdbuff	*buffend;	/* last address in buffer memory*/
	uint32	size;			/* total size of memory needed	*/
					/*    buffers			*/

	/* Obtain address of control block */

	rdptr = &rdstab[devptr->dvminor];

	/* Set control block to unused */

	rdptr->rd_state = RD_FREE;
	rdptr->rd_id[0] = NULLCH;
	
	/* Set initial message sequence number */

	rdptr->rd_seq = 1;

	/* Initialize request queue and cache to empty */

	rdptr->rd_rhnext = (struct rdbuff *) &rdptr->rd_rtnext;
	rdptr->rd_rhprev = (struct rdbuff *)NULL;

	rdptr->rd_rtnext = (struct rdbuff *)NULL;
	rdptr->rd_rtprev = (struct rdbuff *) &rdptr->rd_rhnext;


	rdptr->rd_chnext = (struct rdbuff *) &rdptr->rd_ctnext;
	rdptr->rd_chprev = (struct rdbuff *)NULL;

	rdptr->rd_ctnext = (struct rdbuff *)NULL;
	rdptr->rd_ctprev = (struct rdbuff *) &rdptr->rd_chnext;

	/* Allocate memory for a set of buffers (actually request	*/
	/*    blocks and link them to form the initial free list	*/

	size = sizeof(struct rdbuff) * RD_BUFFS;

	bptr = (struct rdbuff *)getmem(size);
	rdptr->rd_free = bptr;

	if ((int32)bptr == SYSERR) {
		panic("Cannot allocate memory for remote disk buffers");
	}

	buffend = (struct rdbuff *) ((char *)bptr + size);
	while (bptr < buffend) {	/* walk through memory */
		pptr = bptr;
		bptr = (struct rdbuff *)
				(sizeof(struct rdbuff)+ (char *)bptr);
		pptr->rd_status = RD_INVALID;	/* buffer is empty	*/
		pptr->rd_next = bptr;		/* point to next buffer */
	}
	pptr->rd_next = (struct rdbuff *) NULL;	/* last buffer on list	*/

	/* Create the request list and available buffer semaphores */

	rdptr->rd_availsem = semcreate(RD_BUFFS);
	rdptr->rd_reqsem   = semcreate(0);

	/* Set the server IP address, server port, and local port */

	if ( dot2ip(RD_SERVER_IP, &rdptr->rd_ser_ip) == SYSERR ) {
		panic("invalid IP address for remote disk server");
	}

	/* Set the port numbers */

	rdptr->rd_ser_port = RD_SERVER_PORT;
	rdptr->rd_loc_port = RD_LOC_PORT + devptr->dvminor;

	/* Specify that the server port is not yet registered */

	rdptr->rd_registered = FALSE;

	/* Create a communication process */

	rdptr->rd_comproc = create(rdsprocess, RD_STACK, RD_PRIO,
						"rdsproc", 1, rdptr);

	if (rdptr->rd_comproc == SYSERR) {
		panic("Cannot create remote disk process");
	}
	resume(rdptr->rd_comproc);

	return OK;
}
