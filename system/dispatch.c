/* dispatch.c */

#include <xinu.h>
#include <mips.h>
#include <ar9130.h>

/* Initialize list of interrupts */

char *interrupts[] = {
    "Software interrupt request 0",
    "Software interrupt request 1",
    "Hardware interrupt request 0, wmac",
    "Hardware interrupt request 1, usb",
    "Hardware interrupt request 2, eth0",
    "Hardware interrupt request 3, eth1",
    "Hardware interrupt request 4, misc",
    "Hardware interrupt request 5, timer",
    "Miscellaneous interrupt request 0, timer",
    "Miscellaneous interrupt request 1, error",
    "Miscellaneous interrupt request 2, gpio",
    "Miscellaneous interrupt request 3, uart",
    "Miscellaneous interrupt request 4, watchdog",
    "Miscellaneous interrupt request 5, perf",
    "Miscellaneous interrupt request 6, reserved",
    "Miscellaneous interrupt request 7, mbox",
};

/*------------------------------------------------------------------------
 * dispatch - high-level piece of interrupt dispatcher
 *------------------------------------------------------------------------
 */

void	dispatch(
	  int32	cause,		/* identifies interrupt cause 		*/
	  int32	*frame		/* pointer to interrupt frame that	*/
				/*  contains saved status		*/
	)
{
	intmask	mask;		/* saved interrupt status		*/
	int32	irqcode = 0;	/* code for interrupt			*/
	int32	irqnum = -1;	/* interrupt number			*/
	void	(*handler)(void);/* address of handler function to call	*/

	if (cause & CAUSE_EXC) exception(cause, frame);

	/* Obtain the IRQ code */

	irqcode = (cause & CAUSE_IRQ) >> CAUSE_IRQ_SHIFT;

	/* Calculate the interrupt number */

	while (irqcode) {
		irqnum++;
		irqcode = irqcode >> 1;
	}

	if (IRQ_ATH_MISC == irqnum) {
		uint32 *miscStat = (uint32 *)RST_MISC_INTERRUPT_STATUS;
		irqcode = *miscStat & RST_MISC_IRQ_MASK;
		irqnum = 7;
		while (irqcode) {
			irqnum++;
			irqcode = irqcode >> 1;
		}
	}

	/* Check for registered interrupt handler */

	if ((handler = interruptVector[irqnum]) == NULL) {
		kprintf("Xinu Interrupt %d uncaught, %s\r\n",
			irqnum, interrupts[irqnum]);
		while (1) {
			;	       /* forever */
		}
	}

	mask = disable();	/* Disable interrupts for duration */

	exlreset();		/* Reset system-wide exception bit */

	(*handler) ();		/* Invoke device-specific handler  */

	exlset();		/* Set system-wide exception bit   */
	restore(mask);
}

/*------------------------------------------------------------------------
 * enable_irq - enable a specific IRQ
 *------------------------------------------------------------------------
 */

void enable_irq(
	intmask irqnumber		/* specific IRQ to enable	*/
	)
{
	int32	enable_cpuirq(int);
	int irqmisc;
	uint32 *miscMask = (uint32 *)RST_MISC_INTERRUPT_MASK;
	if (irqnumber >= 8) {
		irqmisc = irqnumber - 8;
		enable_cpuirq(IRQ_ATH_MISC);
		*miscMask |= (1 << irqmisc);
	} else {
		enable_cpuirq(irqnumber);
	}
}
