/* rflInit.c - rflInit */

#include <xinu.h>

struct	rflcblk	rfltab[Nrfl];		/* rfl device control blocks	*/

/*------------------------------------------------------------------------
 *  rflInit - initialize a remote file device
 *------------------------------------------------------------------------
 */
devcall	rflInit(
	  struct dentry	*devptr		/* entry in device switch table	*/
	)
{
	struct	rflcblk	*rflptr;	/* ptr. to control block entry	*/
	int32	i;			/* walks through name arrary	*/

	rflptr = &rfltab[ devptr->dvminor ];

	/* Initialize entry to unused */

	rflptr->rfstate = RF_FREE;
	rflptr->rfdev = devptr->dvnum;
	for (i=0; i<RF_NAMLEN; i++) {
		rflptr->rfname[i] = NULLCH;
	}
	rflptr->rfpos = rflptr->rfmode = 0;
	return OK;
}
