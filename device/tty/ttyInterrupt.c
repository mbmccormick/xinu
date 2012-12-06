/* ttyInterrupt.c - ttyInterrupt */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ttyInterrupt - handle an interrupt for a tty (serial) device
 *------------------------------------------------------------------------
 */
interrupt ttyInterrupt(void)
{
	struct	dentry	*devptr;	/* pointer to devtab entry	*/
	struct	ttycblk	*typtr;		/* pointer to ttytab entry	*/	
	struct	uart_csreg *uptr;	/* address of UART's CSRs	*/
	int32	iir = 0;		/* interrupt identification	*/
	int32	lsr = 0;		/* line status			*/

	/* For now, the CONSOLE is the only serial device */

	devptr = (struct dentry *)&devtab[CONSOLE];

	/* Obtain the CSR address for the UART */

	uptr = (struct uart_csreg *)devptr->dvcsr;

	/* Obtain a pointer to the tty control block */

	typtr = &ttytab[ devptr->dvminor ];

	/* Decode hardware interrupt request from UART device */

        /* Check interrupt identification register */

        iir = uptr->iir;
        if (iir & UART_IIR_IRQ) {
		return;
        }

	/* Decode the interrupt cause based upon the value extracted	*/
	/* from the UART interrupt identification register.  Clear	*/
	/* the interrupt source and perform the appropriate handling	*/
	/* to coordinate with the upper half of the driver		*/

        iir &= UART_IIR_IDMASK;		/* Mask off the interrupt ID */
        switch (iir) {

	    /* Receiver line status interrupt (error) */

	    case UART_IIR_RLSI:
		lsr = uptr->lsr;
		return;

	    /* Receiver data available or timed out */

	    case UART_IIR_RDA:
	    case UART_IIR_RTO:

		sched_cntl(DEFER_START);

		/* For each char in UART buffer, call ttyInter_in */

		while (uptr->lsr & UART_LSR_DR) { /* while chars avail */
			ttyInter_in(typtr, uptr);
                }

		sched_cntl(DEFER_STOP);

		return;

            /* Transmitter output FIFO is empty (i.e., ready for more)	*/

	    case UART_IIR_THRE:
		lsr = uptr->lsr;  /* Read from LSR to clear interrupt */
		ttyInter_out(typtr, uptr);
		return;

	    /* Modem status change (simply ignore) */

	    case UART_IIR_MSC:
		return;
	    }
}
