/* ethClose.c -  ethClose */

#include <xinu.h>

/*------------------------------------------------------------------------
 * ethClose - Close an ethernet device
 *------------------------------------------------------------------------
 */
devcall	ethClose (
	 struct	dentry	*devptr		/* entry in device switch table	*/
	)
{
	struct	ether *ethptr;
	struct	ag71xx *nicptr;

	ethptr = &ethertab[devptr->dvminor];
	nicptr = ethptr->csr;

	/* flag interface as down */
	ethptr->state = ETH_STATE_DOWN;

	/* disable ether interrupt source */
	nicptr->interruptMask = ethptr->interruptMask = 0x0;
	nicptr->interruptStatus = 0x0;

	ethControl(devptr, ETH_CTRL_RESET, 0, 0);

    return OK;
}
