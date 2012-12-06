/* kprintf.c -  kputc, kgetc, kprintf */

#include <xinu.h>
#include <stdarg.h>

/*------------------------------------------------------------------------
 * kputc - use polled I/O to write a character to the console serial line
 *------------------------------------------------------------------------
 */
syscall kputc(byte c)
{
    int status, irmask;
    volatile struct uart_csreg *regptr;
	struct	dentry	*devptr;

	devptr = (struct dentry *) &devtab[CONSOLE];
    regptr = (struct uart_csreg *)devptr->dvcsr;

    irmask = regptr->ier;       /* Save UART interrupt state.   */
    regptr->ier = 0;            /* Disable UART interrupts.     */

    do                          /* Wait for transmitter         */
    {
        status = regptr->lsr;
    }
    while ((status & UART_LSR_TEMT) != UART_LSR_TEMT);

    /* Send character. */
    regptr->thr = c;

    regptr->ier = irmask;       /* Restore UART interrupts.     */
    return c;
}

/*------------------------------------------------------------------------
 * kgetc - use polled I/O to read a character from the console serial line
 *------------------------------------------------------------------------
 */
syscall kgetc(void)
{
    int irmask;
    volatile struct uart_csreg *regptr;
    byte c;
	struct	dentry	*devptr;

	devptr = (struct dentry *) &devtab[CONSOLE];
    regptr = (struct uart_csreg *)devptr->dvcsr;

    irmask = regptr->ier;       /* Save UART interrupt state.   */
    regptr->ier = 0;            /* Disable UART interrupts.     */

    while (0 == (regptr->lsr & UART_LSR_DR))
    {                           /* Do Nothing */
    }

    /* read character from Receive Holding Register */
    c = regptr->rbr;
    regptr->ier = irmask;       /* Restore UART interrupts.     */
    return c;
}

extern	void	_doprnt(char *,int (*)(int), ...);

/*------------------------------------------------------------------------
 *  kprintf - kernel printf using unbuffered, polled output to CONSOLE
 *------------------------------------------------------------------------
 */
syscall kprintf(char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    _doprnt(fmt, ap, (int (*)(int))kputc, (int)&devtab[CONSOLE]);
    va_end(ap);
    return OK;
}
