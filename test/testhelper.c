#include <xinu.h>
#include <testsuite.h>
#include <stdio.h>

/**< table of test cases */
struct testcase testtab[] = {
	{"Argument Passing", test_bigargs},
	{"Added Arguments", test_addargs},
	{"Priority Scheduling", test_schedule},
	{"Process Preemption", test_preempt},
	{"Recursion", test_recursion},
#if NSEM
	{"Single Semaphore", test_semaphore},
	{"Multiple Semaphores", test_semaphore2},
	{"Counting Semaphores", test_semaphore3},
	{"Killing Semaphores", test_semaphore4},
	{"Resetting Semaphores", test_semaphore5},
#endif
	{"Standard Input/Output", test_libStdio},
};

int ntests = sizeof(testtab) / sizeof(struct testcase);

void testPass(bool8 verbose, const char *msg)
{
    if (TRUE == verbose)
    {
        printf("\033[40G[\033[1;32mPASS\033[0;39m] %s\n", msg);
    }
}

void testFail(bool8 verbose, const char *msg)
{
    if (TRUE == verbose)
    {
        printf("\033[40G[\033[1;31mFAIL\033[0;39m] %s\n", msg);
    }
}

void testSkip(bool8 verbose, const char *msg)
{
    if (TRUE == verbose)
    {
        printf("\033[40G[\033[1;33mSKIP\033[0;39m] %s\n", msg);
    }
}

void testPrint(bool8 verbose, const char *msg)
{
    if (TRUE == verbose)
    {
        printf("  %s", msg);
    }
}
