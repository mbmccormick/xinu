/* in file addargs.c */
extern	status	addargs(pid32, int32, int32[], int32,char *, void *);

/* in file arp.c */
extern	int32	arp_alloc(void);
extern	void	arp_in(void);
extern	void	arp_init(void);
extern	status	arp_resolve(uint32, byte *);

/* in file ascdate.c */
extern	status	ascdate(uint32, char *);

/* in file bufinit.c */
extern	status	bufinit(void);

/* in file chprio.c */
extern	pri16	chprio(pid32, pri16);

/* in file clkupdate.S */
extern	uint32	clkcount(void);

/* in file clkhandler.c */
extern	interrupt clkhandler(void);

/* in file clkinit.c */
extern	void	clkinit(void);

/* in file close.c */
extern	syscall	close(did32);

/* in file control.c */
extern	syscall	control(did32, int32, int32, int32);

/* in file create.c */
extern	pid32	create(void *, uint32, pri16, char *, uint32, ...);

/* in file ctxsw.S */
extern	void	ctxsw(void *, void *);

/* in file dot2ip.c */
extern	uint32	dot2ip(char *, uint32 *);

/* in file queue.c */
extern	pid32	enqueue(pid32, qid16);

/* in file intutils.S */
extern	intmask	disable(void);

/* in file intutils.S */
extern	void	enable(void);

/* in file ethClose.c */
extern	devcall	ethClose(struct dentry *);

/* in file ethControl.c */
extern	devcall	ethControl(struct dentry *, int32, int32, int32);

/* in file ethInit.c */
extern	devcall	ethInit(struct dentry *);

/* in file etherInterupt.c */
extern	interrupt	ethInterrupt(void);

/* in file ethOpen.c */
extern	devcall	ethOpen(struct dentry *);

/* in file ethRead.c */
extern	devcall ethRead(struct dentry *, void *, uint32);

/* in file ethStat.c */
extern	void	ethStat(uint16);

/* in file ethWrite.c */
extern	devcall	ethWrite(struct dentry *, void *, uint32);

/* in file exception.c */
extern  void exception(int32, int32*);

/* in file freebuf.c */
extern	syscall	freebuf(char *);

/* in file freemem.c */
extern	syscall	freemem(char *, uint32);

/* in file getbuf.c */
extern	char	*getbuf(bpid32);

/* in file getc.c */
extern	syscall	getc(did32);

/* in file getitem.c */
extern	pid32	getfirst(qid16);

/* in file dhcp.c */
extern	uint32	getlocalip(void);

/* in file getmem.c */
extern	char	*getmem(uint32);

/* in file getpid.c */
extern	pid32	getpid(void);

/* in file getprio.c */
extern	syscall	getprio(pid32);

/* in file getstk.c */
extern	char	*getstk(uint32);

/* in file gettime.c */
extern	status	gettime(uint32 *);

/* in file getutime.c */
extern	status	getutime(uint32 *);

/* in file halt.S */
extern	void	halt(void);

/* in file icmp.c */
extern	void	icmp_init (void);
extern	void	icmp_in (void);
extern	process	icmp_out (void);
extern	int32	icmp_register (uint32);
extern	int32	icmp_recv (int32, char *, int32, uint32);
extern	status	icmp_send (uint32, uint16, uint16, uint16, char	*, int32);
extern	status	icmp_release (int32);
extern	uint16	icmp_cksum (char *, int32);

/* in file init.c */
extern	syscall	init(did32);

/* in file insert.c */
extern	status	insert(pid32, qid16, int32);

/* in file insertd.c */
extern	status	insertd(pid32, qid16, int32);

/* in file ioerr.c */
extern	devcall	ioerr(void);

/* in file ionull.c */
extern	devcall	ionull(void);

/* in file ipdump.c */
extern	void	ipdump(char *, int32);

/* in file netin.c */
extern	uint16	ipcksum(struct netpacket *);

/* in file kill.c */
extern	syscall	kill(pid32);

/* in file lexan.c */
extern	int32	lexan(char *, int32, char *, int32 *, int32 [], int32 []);

/* in file lfibclear.c */
extern	void	lfibclear(struct lfiblk *, int32);

/* in file lfibget.c */
extern	void	lfibget(did32, ibid32, struct lfiblk *);

/* in file lfibput.c */
extern	status	lfibput(did32, ibid32, struct lfiblk *);

/* in file lfdbfree.c */
extern	status	lfdbfree(did32, dbid32);

/* in file lfdballoc.c */
extern	dbid32	lfdballoc(struct lfdbfree *);

/* in file lfflush.c */
extern	status	lfflush(struct lflcblk *);

/* in file lfgetmode.c */
extern	int32	lfgetmode(char *);

/* in file lfiballoc.c */
extern	ibid32	lfiballoc(void);

/* in file lflClose.c */
extern	devcall	lflClose(struct dentry *);

/* in file lflControl.c */
extern	devcall	lflControl(struct dentry *, int32, int32, int32);

/* in file lflGetc.c */
extern	devcall	lflGetc(struct dentry *);

/* in file lflInit.c */
extern	devcall	lflInit(struct dentry *);

/* in file lflPutc.c */
extern	devcall	lflPutc(struct dentry *, char);

/* in file lflRead.c */
extern	devcall	lflRead(struct dentry *, char *, int32);

/* in file lflSeek.c */
extern	devcall	lflSeek(struct dentry *, uint32);

/* in file lflWrite.c */
extern	devcall	lflWrite(struct dentry *, char *, int32);

/* in file lfscreate.c */
extern  status  lfscreate(did32, ibid32, uint32);

/* in file lfsInit.c */
extern	devcall	lfsInit(struct dentry *);

/* in file lfsOpen.c */
extern	devcall	lfsOpen(struct dentry *, char *, char *);

/* in file lfsetup.c */
extern	status	lfsetup(struct lflcblk *);

/* in file lftruncate.c */
extern	status	lftruncate(struct lflcblk *);

/* in file lpgetc.c */
extern	devcall	lpgetc(struct dentry *);

/* in file lpinit.c */
extern	devcall	lpinit(struct dentry *);

/* in file lpopen.c */
extern	devcall	lpopen(struct dentry *, char *, char *);

/* in file lpputc.c */
extern	devcall	lpputc(struct dentry *, char);

/* in file lpread.c */
extern	devcall	lpread(struct dentry *, char *, int32);

/* in file lpwrite.c */
extern	devcall	lpwrite(struct dentry *, char *, int32);

/* in file mark.c */
extern	void	_mkinit(void);

/* in file memcpy.c */
extern	void	*memcpy(void *, const void *, int32);

/* in file memset.c */
extern  void    *memset(void *, const int, int32);

/* in file mkbufpool.c */
extern	bpid32	mkbufpool(int32, int32);

/* in file mount.c */
extern	syscall	mount(char *, char *, did32);
extern	int32	namlen(char *, int32);

/* in file namInit.c */
extern	status	namInit(void);

/* in file nammap.c */
extern	devcall	nammap(char *, char[], did32);
extern	did32	namrepl(char *, char[]);
extern	status	namcpy(char *, char *, int32);

/* in file namOpen.c */
extern	devcall	namOpen(struct dentry *, char *, char *);

/* in file netin.c */
extern	process	netin(void);

/* in file newqueue.c */
extern	qid16	newqueue(void);

/* in file open.c */
extern	syscall	open(did32, char *, char *);

/* in file panic.c */
extern	void	panic(char *);

/* in file pdump.c */
extern	void	pdump(struct netpacket *);

/* in file ptclear.c */
extern	void	_ptclear(struct ptentry *, uint16, int32 (*)(int32));

/* in file ptcount.c */
extern	int32	ptcount(int32);

/* in file ptcreate.c */
extern	syscall	ptcreate(int32);

/* in file ptdelete.c */
extern	syscall	ptdelete(int32, int32 (*)(int32));

/* in file ptinit.c */
extern	syscall	ptinit(int32);

/* in file ptrecv.c */
extern	uint32	ptrecv(int32);

/* in file ptreset.c */
extern	syscall	ptreset(int32, int32 (*)(int32));

/* in file ptsend.c */
extern	syscall	ptsend(int32, umsg32);

/* in file putc.c */
extern	syscall	putc(did32, char);

/* in file ramClose.c */
extern	devcall	ramClose(struct dentry *);

/* in file ramInit.c */
extern	devcall	ramInit(struct dentry *);

/* in file ramOpen.c */

extern	devcall	ramOpen(struct dentry *, char *, char *);

/* in file ramRead.c */
extern	devcall	ramRead(struct dentry *, char *, int32);

/* in file ramWrite.c */
extern	devcall	ramWrite(struct dentry *, char *, int32);

/* in file rdsClose.c */
extern	devcall	rdsClose(struct dentry *);

/* in file rdsControl.c */
extern	devcall	rdsControl(struct dentry *, int32, int32, int32);

/* in file rdsInit.c */
extern	devcall	rdsInit(struct dentry *);

/* in file rdsOpen.c */
extern	devcall	rdsOpen(struct dentry *, char *, char *);

/* in file rdsRead.c */
extern	devcall	rdsRead(struct dentry *, char *, int32);

/* in file rdsWrite.c */
extern	devcall	rdsWrite(struct dentry *, char *, int32);

/* in file rdsbufalloc.c */
extern	struct	rdbuff * rdsbufalloc(struct rdscblk *);

/* in file rdscomm.c */
extern	status	rdscomm(struct rd_msg_hdr *, int32, struct rd_msg_hdr *,
		int32, struct rdscblk *);

/* in file rdsprocess.c */
extern	void	rdsprocess(struct rdscblk *);

/* in file read.c */
extern	syscall	read(did32, char *, uint32);

/* in file ready.c */
extern	status	ready(pid32, bool8);

/* in file receive.c */
extern	umsg32	receive(void);

/* in file recvclr.c */
extern	umsg32	recvclr(void);

/* in file recvtime.c */
extern	umsg32	recvtime(int32);

/* in file resched.c */
extern	void	resched(void);

/* in file intutils.S */
extern	void	restore(intmask);

/* in file resume.c */
extern	pri16	resume(pid32);

/* in file rfsgetmode.c */
extern	int32	rfsgetmode(char * );

/* in file rflClose.c */
extern	devcall	rflClose(struct dentry *);

/* in file rfsControl.c */
extern	devcall	rfsControl(struct dentry *, int32, int32, int32);

/* in file rflGetc.c */
extern	devcall	rflGetc(struct dentry *);

/* in file rflInit.c */
extern	devcall	rflInit(struct dentry *);

/* in file rflPutc.c */
extern	devcall	rflPutc(struct dentry *, char );

/* in file rflRead.c */
extern	devcall	rflRead(struct dentry *, char *, int32 );

/* in file rflSeek.c */
extern	devcall	rflSeek(struct dentry *, uint32 );

/* in file rflWrite.c */
extern	devcall	rflWrite(struct dentry *, char *, int32 );

/* in file rfsndmsg.c */
extern	status	rfsndmsg(uint16, char *);

/* in file rfsInit.c */
extern	devcall	rfsInit(struct dentry *);

/* in file rfsOpen.c */
extern	devcall	rfsOpen(struct dentry  *devptr, char *, char *);

/* in file rfscomm.c */
extern	int32	rfscomm(struct rf_msg_hdr *, int32, struct rf_msg_hdr *, int32);

/* in file rdsClose.c */
extern	devcall	rdsClose(struct dentry *);

/* in file rdsControl.c */
extern	devcall	rdsControl(struct dentry *, int32, int32, int32);

/* in file rdsInit.c */
extern	devcall	rdsInit(struct dentry *);

/* in file rdsOpen.c */
extern	devcall	rdsOpen(struct dentry *, char *, char *);

/* in file rdsRead.c */
extern	devcall	rdsRead(struct dentry *, char *, int32);

/* in file rdsWrite.c */
extern	devcall	rdsWrite(struct dentry *, char *, int32);

/* in file rdsbufalloc.c */
extern	struct	rdbuff * rdsbufalloc(struct rdscblk *);

/* in file rdscomm.c */
extern	status	rdscomm(struct rd_msg_hdr *, int32, struct rd_msg_hdr *, int32, struct rdscblk *);

/* in file rdsprocess.c */
extern	void	rdsprocess(struct rdscblk *);

/* in file sched_cntl.c */
extern	status	sched_cntl(int32);

/* in file seek.c */
extern	syscall	seek(did32, uint32);

/* in file semcount.c */
extern	syscall	semcount(sid32);

/* in file semcreate.c */
extern	sid32	semcreate(int32);

/* in file semdelete.c */
extern	syscall	semdelete(sid32);

/* in file semreset.c */
extern	syscall	semreset(sid32, int32);

/* in file send.c */
extern	syscall	send(pid32, umsg32);

/* in file signal.c */
extern	syscall	signal(sid32);

/* in file signaln.c */
extern	syscall	signaln(sid32, int32);

/* in file sleep.c */
extern	syscall	sleepms(uint32);
extern	syscall	sleep(uint32);

/* in file suspend.c */
extern	syscall	suspend(pid32);

/* in file ttyControl.c */
extern	devcall	ttyControl(struct dentry *, int32, int32, int32);

/* in file ttyGetc.c */
extern	devcall	ttyGetc(struct dentry *);

/* in file ttyInter_in.c */
extern	void	ttyInter_in(struct ttycblk *, struct uart_csreg *);

/* in file ttyInter_out.c */
extern	void	ttyInter_out(struct ttycblk *, struct uart_csreg *);

/* in file ttyInterrupt.c */
extern	interrupt	ttyInterrupt(void);

/* in file ttyInit.c */
extern	devcall	ttyInit(struct dentry *);

/* in file ttyKickOut.c */
extern	void	ttyKickOut(struct ttycblk *, struct uart_csreg *);

/* in file ttyPutc.c */
extern	devcall	ttyPutc(struct dentry *, char);

/* in file ttyRead.c */
extern	devcall	ttyRead(struct dentry *, char *, int32);

/* in file ttyWrite.c */
extern	devcall	ttyWrite(struct dentry *, char *, int32);

/* in file udp.c */
extern	void	udp_in(void);
extern	void	udp_init(void);
extern	int32	udp_recv(uint32, uint16, uint16, char *, int32, uint32);
extern	int32	udp_recvaddr(uint32 *, uint16*, uint16, char *, int32, uint32);
extern	status	udp_register(uint32, uint16, uint16);
extern	status	udp_send(uint32, uint16, uint32, uint16, char *, int32);
extern	status	udp_release(uint32, uint16, uint16);

/* in file unsleep.c */
extern	syscall	unsleep(pid32);

/* in file userret.c */
extern	void	userret(void);

/* in file wait.c */
extern	syscall	wait(sid32);

/* in file wakeup.c */
extern	void	wakeup(void);

/* in file write.c */
extern	syscall	write(did32, char *, uint32);

/* in file xdone.c */
extern	void	xdone(void);

/* in file yield.c */
extern	syscall	yield(void);

/* NETWORK BYTE ORDER CONVERSION NOT NEEDED ON A BIG-ENDIAN COMPUTER */
#define	htons(x)	(x)
#define	htonl(x)	(x)
#define	ntohs(x)	(x)
#define	ntohl(x)	(x)
