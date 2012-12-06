/* memory.h - roundmb, truncmb, freestk */

#define	PAGE_SIZE	4096
#define MAXADDR		0x02000000	/* 160NL has 32MB RAM		*/

/*----------------------------------------------------------------------
 * roundmb, truncmb - round or truncate address to memory block size
 *----------------------------------------------------------------------
 */
#define	roundmb(x)	(char *)( (7 + (uint32)(x)) & (~7) )
#define	truncmb(x)	(char *)( ((uint32)(x)) & (~7) )

/*----------------------------------------------------------------------
 *  freestk  --  free stack memory allocated by getstk
 *----------------------------------------------------------------------
 */
#define	freestk(p,len)	freemem((char *)((uint32)(p)		\
				- ((uint32)roundmb(len))	\
				+ (uint32)sizeof(uint32)),	\
				(uint32)roundmb(len) )

struct	memblk	{			/* see roundmb & truncmb	*/
	struct	memblk	*mnext;		/* ptr to next free memory blk	*/
	uint32	mlength;		/* size of blk (includes memblk)*/
	};
extern	struct	memblk	memlist;	/* head of free memory list	*/
extern	void	*maxheap;		/* max free memory address	*/
extern	void	*minheap;		/* address beyond loaded memory	*/

/* added by linker */

extern	int	end;			/* end of program		*/
extern	int	edata;			/* end of data segment		*/
extern	int	etext;			/* end of text segment		*/
