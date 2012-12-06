/* rflGetc.c - rflGetc */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  rflGetc - read one character from a remote file (interrupts disabled)
 *------------------------------------------------------------------------
 */
devcall	rflGetc(
	struct	dentry	*devptr		/* entry in device switch table	*/
	)
{
	char	ch;			/* character to read		*/
	int32	retval;			/* return value			*/

	retval = rflRead(devptr, &ch, 1);

	if (retval != 1) {
		return SYSERR;
	}

	return (devcall)ch;
}
