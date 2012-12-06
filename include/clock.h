/* clock.h */

#define CLKTICKS_PER_SEC  1000	/* clock timer resolution		*/
#define CLKCYCS_PER_TICK  200000

extern	uint32	clkticks;	/* counts clock interrupts		*/
extern	uint32	clktime;	/* current time in secs since boot	*/

extern	qid16	sleepq;		/* queue for sleeping processes		*/
extern	uint32	preempt;	/* preemption counter			*/

extern	void	clkupdate(uint32);
