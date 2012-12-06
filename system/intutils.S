/* intutils.S - enable, disable, restore, enable_cpuirq, restore_intr,	*/
/*	exlreset, exlset.						*/

/* Functions that control disabling and enabling of interrupts */

#include <mips.h>

.text
	.globl enable
	.globl disable
	.globl restore
	.globl enable_cpuirq
	.globl exlreset
	.globl exlset
		
/*------------------------------------------------------------------------
 * enable - enable all interrupts
 *------------------------------------------------------------------------
 */
	.ent enable
enable:
	.set noreorder
	mfc0	a1, CP0_STATUS
	/* IE = 1: Master enable bit for SW and HW  */
	li		a2, STATUS_IE
	or		v0, a1, a2
	jr		ra
	mtc0	v0, CP0_STATUS
	.set reorder
	.end enable

/*------------------------------------------------------------------------
 * disable - disbale interrupts and return previous status
 *------------------------------------------------------------------------
 */
	.ent disable
disable:	
	.set noreorder
	mfc0	v0, CP0_STATUS
 	li		a0, ~STATUS_IE
	and		a1, v0, a0
	mtc0	a1, CP0_STATUS
	jr		ra
	nop
	.set reorder
	.end disable

/*------------------------------------------------------------------------
 * restore - restore interrupts according to argument and return previous
 *			state
 *------------------------------------------------------------------------
 */
	.ent restore
restore:
	.set noreorder
	mfc0	a1, CP0_STATUS
	nop
	or		v0, a1, a0
	jr		ra
	mtc0	v0, CP0_STATUS
	.set reorder
	.end restore

	
/*------------------------------------------------------------------------
 * enable_cpuirq - enable the IRQ specified by the argument
 *------------------------------------------------------------------------
 */
	.ent enable_cpuirq
enable_cpuirq:
	.set noreorder
	mfc0	a1, CP0_STATUS
	addi	a3, zero, 7
	and		a0, a0, a3
	addi	a0, a0, 8
	li		a2, 1
	sllv	a2, a2, a0
	or		v0, a1, a2
	jr		ra
	mtc0	v0, CP0_STATUS
	.set reorder
	.end enable_cpuirq
		
/*------------------------------------------------------------------------
 * exlreset - turn EXL bit off, allowing exceptions/interrupts to occur
 *------------------------------------------------------------------------
 */
	.ent exlreset
exlreset:
	.set noreorder
	mfc0	a1, CP0_STATUS
	li		a2, ~STATUS_EXL
	and		v0, a1, a2
	jr		ra
	mtc0	v0, CP0_STATUS
	.set reorder
	.end exlreset

/*------------------------------------------------------------------------
 * exlset - turn EXL bit on, preventing exceptions/interrupts
 *------------------------------------------------------------------------
 */
	.ent exlset
exlset:
	.set noreorder
	mfc0	a1, CP0_STATUS
	li		a2, STATUS_EXL
	or		v0, a1, a2
	jr		ra
	mtc0	v0, CP0_STATUS
	.set reorder
	.end exlset
