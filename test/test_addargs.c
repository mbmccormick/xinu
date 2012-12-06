#include <xinu.h>
#include <testsuite.h>
#include <stdio.h>
#include <string.h>

#define TESTSTR 4
#define STRMAX  20

local void testadd(int argc, char **argv, int *result)
{
	int i;
	for (i = 0; i < argc; i++)
	{
		*result += strnlen(argv[i], 256);
	//	printf("Argv[%d] = %s\n", i, argv[i]);
	}
	return;
}

process test_addargs(bool8 verbose)
{
	int dummy = 0, i = 0;
	pid32 pid = -1;
	int result = 0;
	int argc = 0, argvsize = 0;
	char **argv = NULL;
	char teststrings[TESTSTR][STRMAX]
		= {"Hello", "world", "these are your", "test strings"};

	argv = (char **)getmem(4 * sizeof(char *));
	if ((char **)SYSERR == argv)
	{   testFail(TRUE, ""); return OK; }

	argc = TESTSTR;
	for (i = 0; i < argc; i++)
	{
		argv[i] = &teststrings[i][0];
		argvsize += strnlen(teststrings[i], STRMAX) + 1;
	}

	pid = create((void *)testadd, INITSTK + argvsize, 40,
		"ADDARGS", 3, argc, &dummy, &result);

	addargs(pid, argc, argv, &dummy);
	resume(pid);
	i = receive();
	if (result == argvsize - argc) {
		testPass(TRUE, "");
	} else {
		testFail(TRUE, "");
	}

	return OK;
}
