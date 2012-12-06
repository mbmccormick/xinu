/* ethControl.c -  ethControl */

#include <xinu.h>

/*------------------------------------------------------------------------
 * ethControl - implement control function for an Ethernet device
 *------------------------------------------------------------------------
 */
devcall	ethControl (
	 struct	dentry	*devptr,	/* entry in device switch table	*/
	 int32	func,			/* control function		*/
	 int32	arg1,			/* argument 1, if needed	*/
	 int32	arg2			/* argument 2, if needed	*/
	)
{
	struct	ether	*ethptr;	/* ptr to control block		*/
	struct	ag71xx	*nicptr;	/* ptr to device CSRs		*/
	byte	*macptr;		/* ptr to MAC address		*/
	uint32	temp;			/* temporary			*/

	ethptr = &ethertab[devptr->dvminor];
	if (ethptr->csr == NULL) {
		return SYSERR;
	}
	nicptr = ethptr->csr;

	switch (func) {

	/* Program MAC address into card. */

	case ETH_CTRL_SET_MAC:
        	macptr = (byte *)arg1;

		temp  = ((uint32)macptr[0]) << 24;
		temp |= ((uint32)macptr[1]) << 16;
		temp |= ((uint32)macptr[2]) <<  8;
		temp |= ((uint32)macptr[3]) <<  0;
		nicptr->macAddr1 = temp;

		temp  = 0;
		temp  = ((uint32)macptr[4]) << 24;
		temp |= ((uint32)macptr[5]) << 16;
		nicptr->macAddr2 = temp;
		break;

	/* Get MAC address from card */

	case ETH_CTRL_GET_MAC:
		macptr = (byte *)arg1;

		temp = nicptr->macAddr1;
		macptr[0] = (temp >> 24) & 0xff;
		macptr[1] = (temp >> 16) & 0xff;
		macptr[2] = (temp >>  8) & 0xff;
		macptr[3] = (temp >>  0) & 0xff;

		temp = nicptr->macAddr2;
		macptr[4] = (temp >> 24) & 0xff;
		macptr[5] = (temp >> 16) & 0xff;
		break;

	/* Set receiver mode */

	case ETH_CTRL_SET_LOOPBK:
		if (TRUE == (uint32)arg1) {
			nicptr->macConfig1 |= MAC_CFG1_LOOPBACK;
		} else {
			nicptr->macConfig1 &= ~MAC_CFG1_LOOPBACK;
		}
		break;
	default:
		return SYSERR;
    }
    return OK;
}
