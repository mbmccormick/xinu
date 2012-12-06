/* rflPutc.c - rflPutc */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  rflPutc - write one character to a remote file (interrupts disabled)
 *------------------------------------------------------------------------
 */
devcall	rflPutc(
	struct	dentry	*devptr,	/* entry in device switch table	*/
	char	ch			/* character to write		*/
	)
{
	struct	rflcblk	*rfptr;		/* pointer to rfl control block	*/

	rfptr = &rfltab[devptr->dvminor];

	if (rflWrite(devptr, &ch, 1) != 1) {
		return SYSERR;
	}

	return OK;
}
