/* intdispatch.S - intdispatch */

#include <mips.h>
#include <interrupt.h>

.text
	.align 4
	.globl intdispatch

/*------------------------------------------------------------------------
 * intdispatch - low-level piece of interrupt dispatcher
 *------------------------------------------------------------------------
 */
	.ent intdispatch
intdispatch:
	.set noreorder
	.set noat
	j	savestate               /* Jump to low-level handler    */
	nop
savestate:
	addiu	sp, sp, -IRQREC_SIZE    /* Allocate space on stack      */
	sw	AT, IRQREC_AT(sp)       /* Save assembler temp reg first*/
	mfc0	k0, CP0_CAUSE           /* Save interrupt CAUSE value   */
	mfc0	k1, CP0_EPC             /* Save interrupted PC value    */
	sw	k0, IRQREC_CAUSE(sp)
	mfc0	k0, CP0_STATUS          /* Save co-processor STATUS     */
	sw	k1, IRQREC_EPC(sp)
	sw	k0, IRQREC_STATUS(sp)
	.set at
	.set reorder
	sw	v0, IRQREC_V0(sp)       /* Save all general purpose regs*/
	sw	v1, IRQREC_V1(sp)
	sw	a0, IRQREC_A0(sp)
	sw	a1, IRQREC_A1(sp)
	sw	a2, IRQREC_A2(sp)
	sw	a3, IRQREC_A3(sp)
	sw	t0, IRQREC_T0(sp)
	sw	t1, IRQREC_T1(sp)
	sw	t2, IRQREC_T2(sp)
	sw	t3, IRQREC_T3(sp)
	sw	t4, IRQREC_T4(sp)
	sw	t5, IRQREC_T5(sp)
	sw	t6, IRQREC_T6(sp)
	sw	t7, IRQREC_T7(sp)
	sw	s0, IRQREC_S0(sp)
	sw	s1, IRQREC_S1(sp)
	sw	s2, IRQREC_S2(sp)
	sw	s3, IRQREC_S3(sp)
	sw	s4, IRQREC_S4(sp)
	sw	s5, IRQREC_S5(sp)
	sw	s6, IRQREC_S6(sp)
	sw	s7, IRQREC_S7(sp)
	sw	t8, IRQREC_T8(sp)
	sw	t9, IRQREC_T9(sp)
	sw	k0, IRQREC_K0(sp)
	sw	k1, IRQREC_K1(sp)
	sw	gp, IRQREC_S8(sp)
	sw	sp, IRQREC_SP(sp)
	sw	fp, IRQREC_S9(sp)
	sw	ra, IRQREC_RA(sp)
	sw	zero, IRQREC_ZER(sp)
	mfhi	t0                      /* Save hi and lo               */
	mflo	t1
	sw	t0, IRQREC_HI(sp)
	sw	t1, IRQREC_LO(sp)

	lw	a0, IRQREC_CAUSE(sp)    /* Pass cause and state info to */
	move	a1, sp                  /*  high-level dispatcher       */
	jal	dispatch

restorestate:                           /* On return from dispatcher    */
	lw	t0, IRQREC_HI(sp)       /*  restore all state           */
	lw	t1, IRQREC_LO(sp)
	mthi	t0
	mtlo	t1
	lw	ra, IRQREC_RA(sp)       /* Restore general purpose regs */
	lw	fp, IRQREC_S9(sp)
 	lw	gp, IRQREC_S8(sp)
 	lw	t9, IRQREC_T9(sp)
 	lw	t8, IRQREC_T8(sp)
 	lw	s7, IRQREC_S7(sp)
 	lw	s6, IRQREC_S6(sp)
 	lw	s5, IRQREC_S5(sp)
 	lw	s4, IRQREC_S4(sp)
 	lw	s3, IRQREC_S3(sp)
 	lw	s2, IRQREC_S2(sp)
 	lw	s1, IRQREC_S1(sp)
 	lw	s0, IRQREC_S0(sp)
 	lw	t7, IRQREC_T7(sp)
 	lw	t6, IRQREC_T6(sp)
 	lw	t5, IRQREC_T5(sp)
 	lw	t4, IRQREC_T4(sp)
 	lw	t3, IRQREC_T3(sp)
 	lw	t2, IRQREC_T2(sp)
 	lw	t1, IRQREC_T1(sp)
 	lw	t0, IRQREC_T0(sp)
 	lw	a3, IRQREC_A3(sp)
 	lw	a2, IRQREC_A2(sp)
 	lw	a1, IRQREC_A1(sp)
 	lw	a0, IRQREC_A0(sp)
 	lw	v1, IRQREC_V1(sp)
 	lw	v0, IRQREC_V0(sp)

	.set noreorder
	.set noat
	lw	k0, IRQREC_EPC(sp)      /* Restore interrupted PC value */
 	lw	AT, IRQREC_AT(sp)       /* Restore assembler temp reg   */
	mtc0	k0, CP0_EPC
	lw	k1, IRQREC_STATUS(sp)   /* Restore global status reg    */
	addiu	sp, sp, IRQREC_SIZE     /* Restore stack pointer        */
	mtc0	k1, CP0_STATUS
	nop				/* Delay for co-processor       */
	eret	                        /* Return from interrupt        */
	nop
	nop
	.set at
	.set reorder
	.end intdispatch
