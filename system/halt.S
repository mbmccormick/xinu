/* halt.S - halt */

#include <mips.h>

.text
	.align 4
	.globl	halt

/*------------------------------------------------------------------------
 *  halt - put the processor in a tight loop
 *------------------------------------------------------------------------
 */

/*
 *  Note: this version is a busy wait.  We do that instead of using the
 * opcode 'wait' because waiting puts the processor in a powersave state
 * (which allows the processor to be awakened for arbitrary reasons)
 */
	.ent halt
halt:
	wait
	j	halt
	.end halt
