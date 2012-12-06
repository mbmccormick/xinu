/* rflSeek.c - rflSeek */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rflSeek - change the current position in an open file
 *------------------------------------------------------------------------
 */
devcall	rflSeek (
	  struct dentry	*devptr,	/* entry in device switch table	*/
	  uint32 pos			/* new file position		*/
	)
{
	struct	rflcblk	*rfptr;		/* pointer to control block	*/

	/* Wait for exclusive access */

	wait(Rf_data.rf_mutex);

	/* Verify remote file device is open */

	rfptr = &rfltab[devptr->dvminor];
	if (rfptr->rfstate == RF_FREE) {
		signal(Rf_data.rf_mutex);
		return SYSERR;
	}

	/* Set the new position */

	rfptr->rfpos = pos;
	signal(Rf_data.rf_mutex);
	return OK;
}
