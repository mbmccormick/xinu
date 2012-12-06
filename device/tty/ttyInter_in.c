/* ttyInter_in.c ttyInter_in, erase1, eputc, echoch */

#include <xinu.h>

local	void	erase1(struct ttycblk *, struct uart_csreg *);
local	void	echoch(char, struct ttycblk *, struct uart_csreg *);
local	void	eputc(char, struct ttycblk *, struct uart_csreg *);

/*------------------------------------------------------------------------
 *  ttyInter_in  --  handle one arriving char (interrupts disabled)
 *------------------------------------------------------------------------
 */
void	ttyInter_in (
	  struct ttycblk *typtr,	/* ptr to ttytab entry		*/
	  struct uart_csreg *uptr	/* address of UART's CSRs	*/
	)
{
	char	ch;			/* next char from device	*/
	int32	avail;			/* chars available in buffer	*/

	ch = uptr->buffer;		/* extract char. from device	*/

	/* Compute chars available */

	avail = semcount(typtr->tyisem);
	if (avail < 0) {		/* one or more processes waiting*/
		avail = 0;
	}

	/* Handle raw mode */

	if (typtr->tyimode == TY_IMRAW) {
		if (avail >= TY_IBUFLEN) { /* no space => ignore input	*/
			return;
		}

		/* Place char in buffer with no editing */

		*typtr->tyitail++ = ch;

		/* Wrap buffer pointer	*/

		if (typtr->tyitail >= &typtr->tyibuff[TY_OBUFLEN]) {
			typtr->tyitail = typtr->tyibuff;
		}

		/* Signal input semaphore and return */

		signal(typtr->tyisem);
		return;
	}

	/* Handle cooked and cbreak modes (common part) */

	if ( (ch == TY_RETURN) && typtr->tyicrlf ) {
		ch = TY_NEWLINE;
	}

	/* If flow control is in effect, handle ^S and ^Q */

	if (typtr->tyoflow) {
		if (ch == typtr->tyostart) {	    /* ^Q starts output	*/
			typtr->tyoheld = FALSE;
			ttyKickOut(typtr, uptr);
			return;
		} else if (ch == typtr->tyostop) {  /* ^S stops	output	*/
			typtr->tyoheld = TRUE;
			return;
		}
	}

	typtr->tyoheld = FALSE;		/* Any other char starts output	*/

	if (typtr->tyimode == TY_IMCBREAK) {	   /* Just cbreak mode	*/

		/* If input buffer is full, send bell to user */

		if (avail >= TY_IBUFLEN) {
			eputc(typtr->tyifullc, typtr, uptr);
		} else {	/* Input buffer has space for this char */
			*typtr->tyitail++ = ch;

			/* Wrap around buffer */

			if (typtr->tyitail>=&typtr->tyibuff[TY_IBUFLEN]) {
				typtr->tyitail = typtr->tyibuff;
			}
			if (typtr->tyiecho) {	/* are we echoing chars?*/
				echoch(ch, typtr, uptr);
			}
		}
		return;

	} else {	/* Just cooked mode (see common code above)	*/

		/* Line kill character arrives - kill entire line */

		if (ch == typtr->tyikillc && typtr->tyikill) {
			typtr->tyitail -= typtr->tyicursor;
			if (typtr->tyitail < typtr->tyibuff) {
				typtr->tyihead += TY_IBUFLEN;
			}
			typtr->tyicursor = 0;
			eputc(TY_RETURN, typtr, uptr);
			eputc(TY_NEWLINE, typtr, uptr);
			return;
		}

		/* Erase (backspace) character */

		if ( (ch == typtr->tyierasec) && typtr->tyierase) {
			if (typtr->tyicursor > 0) {
				typtr->tyicursor--;
				erase1(typtr, uptr);
			}
			return;
		}

		/* End of line */

		if ( (ch == TY_NEWLINE) || (ch == TY_RETURN) ) {
			if (typtr->tyiecho) {
				echoch(ch, typtr, uptr);
			}
			*typtr->tyitail++ = ch;
			if (typtr->tyitail>=&typtr->tyibuff[TY_IBUFLEN]) {
				typtr->tyitail = typtr->tyibuff;
			}

			/*  Make entire line (plus \n or \r) available */

			signaln(typtr->tyisem, typtr->tyicursor + 1);
			typtr->tyicursor = 0; 	/* Reset for next line	*/
			return;
		}

		/* Character to be placed in buffer - send bell if	*/
		/*	buffer has overflowed				*/

		avail = semcount(typtr->tyisem);
		if (avail < 0) {
			avail = 0;
		}
		if ((avail + typtr->tyicursor) >= TY_IBUFLEN-1) {
			eputc(typtr->tyifullc, typtr, uptr);
			return;
		}

		/* EOF character: recognize at beginning of line, but	*/
		/*	print and ignore otherwise.			*/

		if (ch == typtr->tyeofch && typtr->tyeof) {
			if (typtr->tyiecho) {
				echoch(ch, typtr, uptr);
			}
			if (typtr->tyicursor != 0) {
				return;
			}
			*typtr->tyitail++ = ch;
			signal(typtr->tyisem);
			return;			
		}


		/* Echo the character */

		if (typtr->tyiecho) {
			echoch(ch, typtr, uptr);
		}

		/* Insert character in the input buffer */

		typtr->tyicursor++;
		*typtr->tyitail++ = ch;

		/* Wrap around if needed */

		if (typtr->tyitail >= &typtr->tyibuff[TY_IBUFLEN]) {
			typtr->tyitail = typtr->tyibuff;
		}
		return;
	}
}

/*------------------------------------------------------------------------
 *  erase1  --  erase one character honoring erasing backspace
 *------------------------------------------------------------------------
 */
local	void	erase1(
	  struct ttycblk	*typtr,	/* ptr to ttytab entry		*/
	  struct uart_csreg *uptr	/* address of UART's CSRs	*/
	)
{
	char	ch;			/* character to erase		*/

	if ( (--typtr->tyitail) < typtr->tyibuff) {
		typtr->tyitail += TY_IBUFLEN;
	}

	/* Pick up char to erase */

	ch = *typtr->tyitail;
	if (typtr->tyiecho) {			   /* Are we echoing? 	*/
		if (ch < TY_BLANK || ch == 0177) { /* Nonprintable	*/
			if (typtr->tyevis) {	   /* Visual cntl chars	*/
				eputc(TY_BACKSP, typtr, uptr);
				if (typtr->tyieback) {	/* erase char 	*/
					eputc(TY_BLANK, typtr, uptr);
					eputc(TY_BACKSP, typtr, uptr);
				}
			}
			eputc(TY_BACKSP, typtr, uptr);/* bypass up arrow*/
			if (typtr->tyieback) {
				eputc(TY_BLANK, typtr, uptr);
				eputc(TY_BACKSP, typtr, uptr);
			}
		} else {  /* A normal character that is printable	*/
			eputc(TY_BACKSP, typtr, uptr);
			if (typtr->tyieback) {	/* erase the character	*/
				eputc(TY_BLANK, typtr, uptr);
				eputc(TY_BACKSP, typtr, uptr);
			}
		}
	}
	return;
}

/*------------------------------------------------------------------------
 *  echoch  --  echo a character with visual and output crlf options
 *------------------------------------------------------------------------
 */
local	void	echoch(
	  char	ch,			/* character to	echo		*/
	  struct ttycblk *typtr,	/* ptr to ttytab entry		*/
	  struct uart_csreg *uptr	/* address of UART's CSRs	*/
	)
{
	if ((ch==TY_NEWLINE || ch==TY_RETURN) && typtr->tyecrlf) {
		eputc(TY_RETURN, typtr, uptr);
		eputc(TY_NEWLINE, typtr, uptr);
	} else if ( (ch<TY_BLANK||ch==0177) && typtr->tyevis) {
		eputc(TY_UPARROW, typtr, uptr);	/* print ^x		*/
		eputc(ch+0100, typtr, uptr);	/* make it printable	*/
	} else {
		eputc(ch, typtr, uptr);
	}
}

/*------------------------------------------------------------------------
 *  eputc - put one character in the echo queue
 *------------------------------------------------------------------------
 */
local	void	eputc(
	  char	ch,			/* character to	echo		*/
	  struct ttycblk *typtr,	/* ptr to ttytab entry		*/
	  struct uart_csreg *uptr	/* address of UART's CSRs	*/
	)
{
	*typtr->tyetail++ = ch;

	/* Wrap around buffer, if needed */

	if (typtr->tyetail >= &typtr->tyebuff[TY_EBUFLEN]) {
		typtr->tyetail = typtr->tyebuff;
	}
	ttyKickOut(typtr, uptr);
	return;
}
