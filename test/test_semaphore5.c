#include <xinu.h>
#include <testsuite.h>
#include <stdio.h>

extern bool8 test_checkSemCount(sid32 s, short c);
extern bool8 test_checkProcState(pid32 tid, byte state);
extern bool8 test_checkResult(byte testResult, byte expected);

extern void test_semWaiter(sid32 s, int times, byte *testResult);

process test_semaphore5(bool8 verbose)
{
    pid32 atid;
    bool8 passed = TRUE;
    sid32 s;
    byte testResult = 0;
    char msg[50];

    testPrint(verbose, "Semaphore creation: ");
    s = semcreate(0);
    if (isbadsem(s))
    {
        passed = FALSE;
        sprintf(msg, "%d", s);
        testFail(verbose, msg);
    }
    else if (test_checkSemCount(s, 0))
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }

    ready(atid =
          create((void *)test_semWaiter, INITSTK, getprio(getpid()) + 10,
                 "SEMAPHORE-A", 3, s, 1, &testResult), RESCHED_YES);

    testPrint(verbose, "Wait on semaphore: ");
    if (test_checkProcState(atid, PR_WAIT)
        && test_checkSemCount(s, -1) && test_checkResult(testResult, 0))
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }

    testPrint(verbose, "Reset semaphore: ");
    if ((OK == semreset(s, 0))
	&& test_checkProcState(atid, PR_FREE)
        && test_checkSemCount(s, 0) && test_checkResult(testResult, 1))
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }

    testPrint(verbose, "Reset semaphore (invalid count): ");
    if (SYSERR == semreset(s, -5))
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }

    testPrint(verbose, "Reset invalid semaphore: ");
    if (SYSERR == semreset(-1, 0))
    {   
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }

    semdelete(s);

    testPrint(verbose, "Reset free semaphore: ");
    if (SYSERR == semreset(s, 0))
    {   
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }

    if (TRUE == passed)
    {
        testPass(TRUE, "");
    }
    else
    {
        testFail(TRUE, "");
    }

    return OK;
}
