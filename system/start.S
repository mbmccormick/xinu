/*  start.S  _start, memzero */

/************************************************************************/
/*									*/
/*   External symbol start (_start in assembly language) gives the	*/
/* location where execution begins after the bootstrap loader has	*/
/* placed a Xinu image in memory and is ready to execute the image.	*/
/*									*/
/*   After initializing the hardware and establishing a run-time	*/
/* environment suitable for C (including a valid stack pointer), the	*/
/* code jumps to the C function nulluser.				*/
/************************************************************************/

#include <interrupt.h>
#include <mips.h>

#define NULLSTK      8192       /* Safe size for NULLSTK */

.extern	flash_size

.text
	.align	4
	.globl	_minheap
	.globl	_start

/*------------------------------------------------------------------------
 *
 * _start   - set up interrupts, initialize the stack pointer, clear the
 *            null process stack, zero the BSS (uninitialized data)
 *            segment, and invoke nulluser
 *------------------------------------------------------------------------
 */
	.ent _start
_start:

	/* Pick up flash size from a3 (where the boot loader leaves it)	*/

	sw	a3, flash_size

	/* Clear Xinu-defined trap and interrupt vectors */

	la	a0, IRQ_ADDR
	la	a1, IRQVEC_END
	jal	memzero
	
	/* Copy low-level interrupt dispatcher to reserved location.	*/

	la	a0, IRQ_ADDR		/* Reserved vector location	*/
	la	a1, intdispatch		/* Start of dispatch code	*/
	lw	v0, 0(a1)
	sw	v0, 0(a0)		/* Store jump opcode		*/

	/* Clear interrupt related registers in the coprocessor */

	mtc0	zero, CP0_STATUS	/* Clear interrupt masks	*/
	mtc0	zero, CP0_CAUSE		/* Clear interrupt cause reg.	*/

	/* Clear and invalidate the L1 instruction and data caches */

	jal	flushcache
		
	/* Set up Stack segment (see function summary) */

	li	s0, NULLSTK		/* Stack is NULLSTK bytes	*/
	la	a0, _end
	addu	s0, s0, a0		/* Top of stack = _end+NULLSTK	*/

	/* Word align the top of the stack */

	subu	s1, s0, 1
	srl	s1, 4
	sll	s1, 4
	
	/* Initialize the stack and frame pointers */

	move	sp, s1
	move	fp, s1
	
	/* Zero NULLSTK space below new stack pointer */

	move	a1, s0		/* note; a0 still points to _end */
	jal	memzero

	/* Clear the BSS segment */

	la	a0, _bss
	la	a1, _end
	jal	memzero

	/* Store bottom of the heap */

	la	t0, minheap
	sw	s0, 0(t0)

	j	nulluser	/* jump to the null process code */
	.end	_start

/*------------------------------------------------------------------------
 * memzero - clear a specified area of memory
 *
 * 	args are: starting address and ending address
 *------------------------------------------------------------------------
 */
	.ent	memzero
memzero:
	sw	zero, 0(a0)
	addiu	a0, a0, 4
	blt	a0, a1, memzero
	jr	ra
	.end	memzero
