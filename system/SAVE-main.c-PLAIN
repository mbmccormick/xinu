/*  main.c  - main */

#include <xinu.h>
#include <ramdisk.h>

extern process shell(void);

/************************************************************************/
/*									*/
/* main - main program that Xinu runs as the first user process		*/
/*									*/
/************************************************************************/

int main(int argc, char **argv)
{
	int32	retval;
	pid32	netpid;

	/* Open the Ethernet (required for MIPS version) */

	open(ETHER0,NULL, NULL);

	/* Create the netin process */

	resume( (netpid = create(netin, 8192, 500, "netin", 0)) );

	/* Use DHCP to obtain: IP and router addresses and subnet mask	*/

	kprintf("\n\r**********************************************************\n\r");

	NetData.ipvalid = FALSE;
	retval = getlocalip();
	if (retval == SYSERR) {
		panic("Error: could not obtain an IP address\n\r");
	} else {
	    kprintf("IP address is %d.%d.%d.%d   %08x\n\r",
		(retval>>24)&0xff, (retval>>16)&0xff, (retval>>8)&0xff,
		 retval&0xff,retval);

	    kprintf("Subnet mask is %d.%d.%d.%d and router is %d.%d.%d.%d\n\r",
		(NetData.addrmask>>24)&0xff, (NetData.addrmask>>16)&0xff,
		(NetData.addrmask>> 8)&0xff,  NetData.addrmask&0xff,
		(NetData.routeraddr>>24)&0xff, (NetData.routeraddr>>16)&0xff,
		(NetData.routeraddr>> 8)&0xff, NetData.routeraddr&0xff);
	}

	lfscreate(TESTDISK, 21, RM_BLKS*RM_BLKSIZ);

	/* create a shell process */

	kprintf("Creating shell...\n\r");
	resume(create(shell, 4096, 1, "shell", 1, CONSOLE));
	retval = recvclr();
	while (TRUE) {
		retval = receive();
		kprintf("\n\n\rMain process recreating shell\n\n\r");
		resume(create(shell, 4096, 1, "shell", 1, CONSOLE));
	}

	/* Execution never reaches here unless while loop is removed */

	return OK;
}
