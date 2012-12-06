/* rflClose.c - rflClose */

#include <xinu.h>

/*------------------------------------------------------------------------
 * rflClose - Close a remote file device
 *------------------------------------------------------------------------
 */
devcall	rflClose (
	  struct dentry	*devptr		/* entry in device switch table	*/
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

	/* Mark device closed */

	rfptr->rfstate = RF_FREE;
	signal(Rf_data.rf_mutex);
	return OK;
}
