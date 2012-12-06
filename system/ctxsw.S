/* ctxsw.S - ctxsw */

#include <mips.h>

.text
	.align 4
	.globl	ctxsw

/*------------------------------------------------------------------------
 *  ctxsw  -  Switch from one process context to another
 *------------------------------------------------------------------------
 */
	.ent ctxsw
ctxsw:
	/* Build context record on the current process' stack */

	addiu	sp, sp, -CONTEXT
	sw	ra, CONTEXT-4(sp)
	sw	ra, CONTEXT-8(sp)

	/* Save callee-save (non-volatile) registers */

	sw	s0, S0_CON(sp)
	sw	s1, S1_CON(sp)
	sw	s2, S2_CON(sp)
	sw	s3, S3_CON(sp)
	sw	s4, S4_CON(sp)
	sw	s5, S5_CON(sp)
	sw	s6, S6_CON(sp)
	sw	s7, S7_CON(sp)
	sw	s8, S8_CON(sp)
	sw	s9, S9_CON(sp)

	/* Save outgoing process' stack pointer */

	sw	sp, 0(a0)

	/* Load incoming process' stack pointer */

	lw	sp, 0(a1)

	/* At this point, we have switched from the run-time stack */
	/*    of the outgoing process to the incoming process      */

	/* Restore callee-save (non-volatile) registers from new stack	*/

	lw	s0, S0_CON(sp)
	lw	s1, S1_CON(sp)
	lw	s2, S2_CON(sp)
	lw	s3, S3_CON(sp)
	lw	s4, S4_CON(sp)
	lw	s5, S5_CON(sp)
	lw	s6, S6_CON(sp)
	lw	s7, S7_CON(sp)
	lw	s8, S8_CON(sp)
	lw	s9, S9_CON(sp)

	/* Restore argument registers for the new process */

	lw	a0, CONTEXT(sp)
	lw	a1, CONTEXT+4(sp)
	lw	a2, CONTEXT+8(sp)
	lw	a3, CONTEXT+12(sp)

	/* Remove context record from the new process' stack */

	lw	v0, CONTEXT-4(sp)
	lw	ra, CONTEXT-8(sp)
	addiu	sp, sp, CONTEXT

	/* If this is a newly created process, ensure */
	/*    it starts with interrupts enabled       */

	beq	v0, ra, ctxdone
	mfc0	v1, CP0_STATUS
	ori	v1, v1, STATUS_IE
	mtc0	v1, CP0_STATUS

ctxdone:
	jr	v0
	.end ctxsw
