/* rdsRead.c  -  rdsRead */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rdsRead - Read a block from a remote disk
 *------------------------------------------------------------------------
 */
devcall	rdsRead (
	  struct dentry	*devptr,	/* entry in device switch table	*/
	  char	*buff,			/* buffer to hold disk block	*/
	  int32	blk			/* block number of block to read*/
	)
{
	struct	rdscblk	*rdptr;		/* pointer to control block	*/
	struct	rdbuff	*bptr;		/* ptr to buffer possibly on	*/
					/*  the request list		*/
	struct	rdbuff	*nptr;		/* ptr to "next" node on a	*/
					/*  list			*/
	struct	rdbuff	*pptr;		/* ptr to "previous" node on	*/
					/*  a list			*/
	struct	rdbuff	*cptr;		/* ptr used to walk the cache	*/

	/* If device not currently in use, report an error */

	rdptr = &rdstab[devptr->dvminor];
	if (rdptr->rd_state != RD_OPEN) {
		return SYSERR;
	}

	/* Search the cache for specified block */

	bptr = rdptr->rd_chnext;
	while (bptr != (struct rdbuff *)&rdptr->rd_ctnext) {
		if (bptr->rd_blknum == blk) {
			if (bptr->rd_status == RD_INVALID) {
				break;
			}
			memcpy(buff, bptr->rd_block, RD_BLKSIZ);
			return OK;
		}
		bptr = bptr->rd_next;
	}

	/* Search the request list for most recent occurrence of block */

	bptr = rdptr->rd_rtprev;	/* start at tail of list */

	while (bptr != (struct rdbuff *)&rdptr->rd_rhnext) {
	    if (bptr->rd_blknum == blk)  {

		/* If most recent request for block is write, copy data */

		if (bptr->rd_op == RD_OP_WRITE) {
			memcpy(buff, bptr->rd_block, RD_BLKSIZ);
			return OK;
		}
		break;
	    }
	    bptr = bptr->rd_prev;
	}

	/* Allocate a buffer and add read request to tail of req. queue */

	bptr = rdsbufalloc(rdptr);	
	bptr->rd_op = RD_OP_READ;
	bptr->rd_refcnt = 1;
	bptr->rd_blknum = blk;
	bptr->rd_status = RD_INVALID;
	bptr->rd_pid = getpid();

	/* Insert new request into list just before tail */

	pptr = rdptr->rd_rtprev;
	rdptr->rd_rtprev = bptr;
	bptr->rd_next = pptr->rd_next;
	bptr->rd_prev = pptr;
	pptr->rd_next = bptr;

	/* Prepare to receive message when read completes */

	recvclr();

	/* Signal semaphore to start communication process */

	signal(rdptr->rd_reqsem);

	/* Block to wait for message */

	bptr = (struct rdbuff *)receive();
	if (bptr == (struct rdbuff *)SYSERR) {	
		return SYSERR;
	}
	memcpy(buff, bptr->rd_block, RD_BLKSIZ);
	bptr->rd_refcnt--;
	if (bptr->rd_refcnt <= 0) {

		/* Look for previous item in cache with the same block	*/
		/*    number to see if this item was only being kept	*/
		/*    until pending read completed			*/
				
		cptr = rdptr->rd_chnext;
		while (cptr != bptr) {
			if (cptr->rd_blknum == blk) {

				/* Unlink from cache */

				pptr = bptr->rd_prev;
				nptr = bptr->rd_next;
				pptr->rd_next = nptr;
				nptr->rd_prev = pptr;

				/* Add to the free list */

				bptr->rd_next = rdptr->rd_free;
				rdptr->rd_free = bptr;
			}
		}
	}
	return OK;
}
