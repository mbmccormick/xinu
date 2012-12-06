/* rdsWrite.c  -  rdsWrite */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rdsWrite - Write a block to a remote disk
 *------------------------------------------------------------------------
 */
devcall	rdsWrite (
	  struct dentry	*devptr,	/* entry in device switch table	*/
	  char	*buff,			/* buffer that holds a disk blk	*/
	  int32	blk			/* block number to write	*/
	)
{
	struct	rdscblk	*rdptr;		/* pointer to control block	*/
	struct	rdbuff	*bptr;		/* ptr to buffer on a list	*/
	struct	rdbuff	*pptr;		/* ptr to previous buff on list	*/
	struct	rdbuff	*nptr;		/* ptr to next buffer on list	*/
	bool8	found;			/* was buff found during search?*/

	/* If device not currently in use, report an error */

	rdptr = &rdstab[devptr->dvminor];
	if (rdptr->rd_state != RD_OPEN) {
		return SYSERR;
	}

	/* If request queue already contains a write request */
	/*    for the block, replace the contents	     */

	bptr = rdptr->rd_rhnext;
	while (bptr != (struct rdbuff *)&rdptr->rd_rtnext) {
		if ( (bptr->rd_blknum == blk) &&
			(bptr->rd_op == RD_OP_WRITE) ) {
			memcpy(bptr->rd_block, buff, RD_BLKSIZ);
			return OK;
		}
		bptr = bptr->rd_next;
	}

	
	/* Search cache for cached copy of block */

	bptr = rdptr->rd_chnext;
	found = FALSE;
	while (bptr != (struct rdbuff *)&rdptr->rd_ctnext) {
		if (bptr->rd_blknum == blk) {
			if (bptr->rd_refcnt <= 0) {
				pptr = bptr->rd_prev;
				nptr = bptr->rd_next;

				/* Unlink node from cache list and reset*/
				/*  the available semaphore accordingly	*/

				pptr->rd_next = bptr->rd_next;
				nptr->rd_prev = bptr->rd_prev;
				semreset(rdptr->rd_availsem,
					semcount(rdptr->rd_availsem) - 1);
				found = TRUE;
			}
			break;
		}
		bptr = bptr->rd_next;
	}

	if ( !found ) {
		bptr = rdsbufalloc(rdptr);
	}

	/* Create a write request */

	memcpy(bptr->rd_block, buff, RD_BLKSIZ);
	bptr->rd_op = RD_OP_WRITE;
	bptr->rd_refcnt = 0;
	bptr->rd_blknum = blk;
	bptr->rd_status = RD_VALID;
	bptr->rd_pid = getpid();

	/* Insert new request into list just before tail */

	pptr = rdptr->rd_rtprev;
	rdptr->rd_rtprev = bptr;
	bptr->rd_next = pptr->rd_next;
	bptr->rd_prev = pptr;
	pptr->rd_next = bptr;

	/* Signal semaphore to start communication process */

	signal(rdptr->rd_reqsem);
	return OK;
}
