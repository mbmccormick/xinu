/* pause.S - pause */

#include <mips.h>

.text
	.align 4	
	.globl pause

/*------------------------------------------------------------------------
 * pause - Enter state that suspends execution until an interrupt occurs
 *------------------------------------------------------------------------
 */
	.ent pause
pause:
	wait
	jr	ra
	.end pause
