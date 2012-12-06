/* create.c - create, newpid */

#include <xinu.h>
#include <mips.h>

static	pid32	newpid(void);

/*------------------------------------------------------------------------
 *  create, newpid  -  Create a process to start running a procedure
 *------------------------------------------------------------------------
 */
pid32	create(
	  void		*funcaddr,	/* address of function to run	*/
	  uint32	ssize,		/* stack size in bytes		*/
	  pri16		priority,	/* process priority		*/
	  char		*name,		/* process name (for debugging)	*/
	  uint32	nargs,		/* number of args that follow	*/
	  ...
	)
{
	intmask	mask;			/* saved interrupt mask		*/
	struct	procent *prptr;		/* ptr to process' table entry	*/
	uint32	*saddr;			/* stack address		*/
	uint32	*savargs;		/* pointer to arg save area	*/
	pid32	pid;			/* ID of newly created process	*/
	uint32	*ap;			/* points to list of var args	*/
	int32	pad;			/* padding needed for arg area	*/
	uint32	i;
	void	INITRET(void);

	mask = disable();

	if (   (ssize < MINSTK)
	    || (priority <= 0)
	    || (((int32)(pid = newpid())) == (int32) SYSERR)
	    || ((saddr = (uint32 *)getstk(ssize)) == (uint32 *)SYSERR)) {
		restore(mask);
		return SYSERR;
	}

	prcount++;
	prptr = &proctab[pid];

	/* Initialize process table entry for new process */

	prptr->prstate = PR_SUSP;	/* initial state is suspended	*/
	prptr->prprio = priority;
	prptr->prstkptr = (char *)saddr;
	prptr->prstkbase = (char *)saddr;
	prptr->prstklen = ssize;
	prptr->prname[PNMLEN-1] = NULLCH;
	for (i=0 ; i<PNMLEN-1 && (prptr->prname[i]=name[i])!=NULLCH; i++)
		;
	prptr->prparent = (pid32)getpid();
	prptr->prhasmsg = FALSE;

	/* Set up initial device descriptors for the shell		*/

	prptr->prdesc[0] = CONSOLE;	/* stdin  is CONSOLE device	*/
	prptr->prdesc[1] = CONSOLE;	/* stdout is CONSOLE device	*/
	prptr->prdesc[2] = CONSOLE;	/* stderr is CONSOLE device	*/

	/* Initialize stack as if the process was called		*/

	*saddr = STACKMAGIC;
	*--saddr = pid;
	*--saddr = (uint32)prptr->prstklen;
	*--saddr = (uint32)prptr->prstkbase - prptr->prstklen
					+ sizeof(int);

	if (nargs == 0) {		/* compute padding		*/
		pad = 4;
	} else if ((nargs%4) == 0) {	/* pad for A0 - A3		*/
		pad = 0;
	} else {
		pad = 4 - (nargs % 4);
	}

	for (i = 0; i < pad; i++) {	/* pad stack by inserting zeroes*/
		*--saddr = 0;
	}

	for (i = nargs; i > 0; i--) {	/* reserve space for arguments	*/
		*--saddr = 0;
	}
	savargs = saddr;		/* loc. of args on the stack	*/

	/* Build the context record that ctxsw expects			*/

	for (i = (CONTEXT_WORDS); i > 0; i--) {
		*--saddr = 0;
	}
	prptr->prstkptr = (char *)saddr;

	/* Address of process entry point				*/

	saddr[(CONTEXT_WORDS) - 1] = (uint32) funcaddr;

	/* Return address value						*/

	saddr[(CONTEXT_WORDS) - 2] = (uint32) INITRET;

	/* Copy arguments into activation record			*/

	ap = (uint32 *)(&nargs + 1);	/* machine dependent code to	*/
	for (i = 0; i < nargs; i++) {	/* copy args onto process stack	*/
		*savargs++ = *ap++;
	}
	restore(mask);
	return pid;
}

/*------------------------------------------------------------------------
 *  newpid  -  Obtain a new (free) process ID
 *------------------------------------------------------------------------
 */
local	pid32	newpid(void)
{
	uint32	i;			/* iterate through all processes*/
	static	pid32 nextpid = 1;	/* position in table to try or	*/
					/*  one beyond end of table	*/

	/* Check all NPROC slots */

	for (i = 0; i < NPROC; i++) {
		nextpid %= NPROC;	/* wrap around to beginning */
		if (proctab[nextpid].prstate == PR_FREE) {
			return nextpid++;
		} else {
			nextpid++;
		}
	}
	return (pid32) SYSERR;
}
