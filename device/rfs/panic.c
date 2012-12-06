/* panic.c - panic */

#include <xinu.h>

/*------------------------------------------------------------------------
 * panic - display a message and stop all processing
 *------------------------------------------------------------------------
 */
void	panic (
	 char	*msg			/* message to display		*/
	)
{
	intmask	mask;			/* saved interrupt mask		*/

	mask = disable();
	kprintf("\n\n\rpanic: %s\n\n\r", msg);
	gpioLEDOff(GPIO_LED_CISCOWHT);	/* turn off LED "run" light	*/
	halt();				/* halt the processor		*/
}
