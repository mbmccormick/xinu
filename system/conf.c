/* conf.c (GENERATED FILE; DO NOT EDIT) */

#include <xinu.h>


extern	devcall	ioerr(void);
extern	devcall	ionull(void);

/* Device independent I/O switch */

struct	dentry	devtab[NDEVS] =
{
/**
 * Format of entries is:
 * dev-number, minor-number, dev-name,
 * init, open, close,
 * read, write, seek,
 * getc, putc, control,
 * dev-csr-address, intr-handler, irq
 */

/* CONSOLE is tty */
	{ 0, 0, "CONSOLE",
	  (void *)ttyInit, (void *)ionull, (void *)ionull,
	  (void *)ttyRead, (void *)ttyWrite, (void *)ioerr,
	  (void *)ttyGetc, (void *)ttyPutc, (void *)ttyControl,
	  (void *)0xb8020000, (void *)ttyInterrupt, 11 },

/* NULLDEV is null */
	{ 1, 0, "NULLDEV",
	  (void *)ionull, (void *)ionull, (void *)ionull,
	  (void *)ionull, (void *)ionull, (void *)ioerr,
	  (void *)ionull, (void *)ionull, (void *)ioerr,
	  (void *)0x0, (void *)ioerr, 0 },

/* ETHER0 is eth */
	{ 2, 0, "ETHER0",
	  (void *)ethInit, (void *)ethOpen, (void *)ioerr,
	  (void *)ethRead, (void *)ethWrite, (void *)ioerr,
	  (void *)ioerr, (void *)ioerr, (void *)ethControl,
	  (void *)0xb9000000, (void *)ethInterrupt, 4 },

/* RFILESYS is rfs */
	{ 3, 0, "RFILESYS",
	  (void *)rfsInit, (void *)rfsOpen, (void *)ioerr,
	  (void *)ioerr, (void *)ioerr, (void *)ioerr,
	  (void *)ioerr, (void *)ioerr, (void *)rfsControl,
	  (void *)0x0, (void *)ionull, 0 },

/* RFILE0 is rfl */
	{ 4, 0, "RFILE0",
	  (void *)rflInit, (void *)ioerr, (void *)rflClose,
	  (void *)rflRead, (void *)rflWrite, (void *)rflSeek,
	  (void *)rflGetc, (void *)rflPutc, (void *)ioerr,
	  (void *)0x0, (void *)ionull, 0 },

/* RFILE1 is rfl */
	{ 5, 1, "RFILE1",
	  (void *)rflInit, (void *)ioerr, (void *)rflClose,
	  (void *)rflRead, (void *)rflWrite, (void *)rflSeek,
	  (void *)rflGetc, (void *)rflPutc, (void *)ioerr,
	  (void *)0x0, (void *)ionull, 0 },

/* RFILE2 is rfl */
	{ 6, 2, "RFILE2",
	  (void *)rflInit, (void *)ioerr, (void *)rflClose,
	  (void *)rflRead, (void *)rflWrite, (void *)rflSeek,
	  (void *)rflGetc, (void *)rflPutc, (void *)ioerr,
	  (void *)0x0, (void *)ionull, 0 },

/* RFILE3 is rfl */
	{ 7, 3, "RFILE3",
	  (void *)rflInit, (void *)ioerr, (void *)rflClose,
	  (void *)rflRead, (void *)rflWrite, (void *)rflSeek,
	  (void *)rflGetc, (void *)rflPutc, (void *)ioerr,
	  (void *)0x0, (void *)ionull, 0 },

/* RFILE4 is rfl */
	{ 8, 4, "RFILE4",
	  (void *)rflInit, (void *)ioerr, (void *)rflClose,
	  (void *)rflRead, (void *)rflWrite, (void *)rflSeek,
	  (void *)rflGetc, (void *)rflPutc, (void *)ioerr,
	  (void *)0x0, (void *)ionull, 0 },

/* RFILE5 is rfl */
	{ 9, 5, "RFILE5",
	  (void *)rflInit, (void *)ioerr, (void *)rflClose,
	  (void *)rflRead, (void *)rflWrite, (void *)rflSeek,
	  (void *)rflGetc, (void *)rflPutc, (void *)ioerr,
	  (void *)0x0, (void *)ionull, 0 },

/* RDISK is rds */
	{ 10, 0, "RDISK",
	  (void *)rdsInit, (void *)rdsOpen, (void *)rdsClose,
	  (void *)rdsRead, (void *)rdsWrite, (void *)ioerr,
	  (void *)ioerr, (void *)ioerr, (void *)rdsControl,
	  (void *)0x0, (void *)ionull, 0 },

/* LFILESYS is lfs */
	{ 11, 0, "LFILESYS",
	  (void *)lfsInit, (void *)lfsOpen, (void *)ioerr,
	  (void *)ioerr, (void *)ioerr, (void *)ioerr,
	  (void *)ioerr, (void *)ioerr, (void *)ioerr,
	  (void *)0x0, (void *)ionull, 0 },

/* LFILE0 is lfl */
	{ 12, 0, "LFILE0",
	  (void *)lflInit, (void *)ioerr, (void *)lflClose,
	  (void *)lflRead, (void *)lflWrite, (void *)lflSeek,
	  (void *)lflGetc, (void *)lflPutc, (void *)lflControl,
	  (void *)0x0, (void *)ionull, 0 },

/* LFILE1 is lfl */
	{ 13, 1, "LFILE1",
	  (void *)lflInit, (void *)ioerr, (void *)lflClose,
	  (void *)lflRead, (void *)lflWrite, (void *)lflSeek,
	  (void *)lflGetc, (void *)lflPutc, (void *)lflControl,
	  (void *)0x0, (void *)ionull, 0 },

/* LFILE2 is lfl */
	{ 14, 2, "LFILE2",
	  (void *)lflInit, (void *)ioerr, (void *)lflClose,
	  (void *)lflRead, (void *)lflWrite, (void *)lflSeek,
	  (void *)lflGetc, (void *)lflPutc, (void *)lflControl,
	  (void *)0x0, (void *)ionull, 0 },

/* LFILE3 is lfl */
	{ 15, 3, "LFILE3",
	  (void *)lflInit, (void *)ioerr, (void *)lflClose,
	  (void *)lflRead, (void *)lflWrite, (void *)lflSeek,
	  (void *)lflGetc, (void *)lflPutc, (void *)lflControl,
	  (void *)0x0, (void *)ionull, 0 },

/* LFILE4 is lfl */
	{ 16, 4, "LFILE4",
	  (void *)lflInit, (void *)ioerr, (void *)lflClose,
	  (void *)lflRead, (void *)lflWrite, (void *)lflSeek,
	  (void *)lflGetc, (void *)lflPutc, (void *)lflControl,
	  (void *)0x0, (void *)ionull, 0 },

/* LFILE5 is lfl */
	{ 17, 5, "LFILE5",
	  (void *)lflInit, (void *)ioerr, (void *)lflClose,
	  (void *)lflRead, (void *)lflWrite, (void *)lflSeek,
	  (void *)lflGetc, (void *)lflPutc, (void *)lflControl,
	  (void *)0x0, (void *)ionull, 0 },

/* TESTDISK is ram */
	{ 18, 0, "TESTDISK",
	  (void *)ramInit, (void *)ramOpen, (void *)ramClose,
	  (void *)ramRead, (void *)ramWrite, (void *)ioerr,
	  (void *)ioerr, (void *)ioerr, (void *)ioerr,
	  (void *)0x0, (void *)ionull, 0 },

/* NAMESPACE is nam */
	{ 19, 0, "NAMESPACE",
	  (void *)namInit, (void *)namOpen, (void *)ioerr,
	  (void *)ioerr, (void *)ioerr, (void *)ioerr,
	  (void *)ioerr, (void *)ioerr, (void *)ioerr,
	  (void *)0x0, (void *)ioerr, 0 }
};
