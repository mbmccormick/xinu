/* clkupdate.S - clkupdate, clkcount */

#include <mips.h>

.text
	.align	4
	.globl	clkupdate
	.globl	clkcount

/*------------------------------------------------------------------------
 * clkupdate - update the timer by a specified number of cycles
 *------------------------------------------------------------------------
 */

/* Note: there are two cases
 *  Normal case: COMPARE is increased by N cycles and stored as the
 *		new threshold (N cycles beyond previous threshold)
 *  Abnormal case: the timer has already accumulated more than N cycles
 *		beyond the previous threshold.  Start over by making
 *		the threshold equal to the current count + N
 */

clkupdate:
	mfc0	v0, CP0_COMPARE	    /* v0 = COMPARE			*/
	mfc0	v1, CP0_COUNT       /* v1 = COUNT			*/
	addu	v0, v0, a0          /* v0 = COMPARE + cycles		*/
	bleu	v0, v1, compare_up  /* v0 <= COUNT, then goto compare_up*/
	mtc0	v0, CP0_COMPARE     /* Update COMPARE			*/
	jr	ra

/* Abnormal case: timer is beyond the next interrupt count; reset	*/

compare_up:
	addu	a0, v1, a0	    /* a0 = COUNT + cycles		*/
	mtc0	a0, CP0_COMPARE	    /* COMPARE = a0			*/
	jr	ra

/*------------------------------------------------------------------------
 * clkcount return the count from the free-running clock
 *------------------------------------------------------------------------
 */
clkcount:
	mfc0 v0, CP0_COUNT
	jr   ra
