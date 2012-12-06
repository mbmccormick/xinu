#include <xinu.h>
#include <testsuite.h>
#include <stdio.h>

extern bool8 test_checkSemCount(sid32 s, short c);
extern bool8 test_checkProcState(pid32 pid, byte state);
extern bool8 test_checkResult(byte testResult, byte expected);

extern void test_semWaiter(sid32 s, int times, byte *testResult);

process test_semaphore3(bool8 verbose)
{
    pid32 apid, bpid;
    bool8 passed = TRUE;
    sid32 s;
    byte testResult = 0;
    char msg[50];

    testPrint(verbose, "Semaphore creation: ");
    s = semcreate(1);
    if (isbadsem(s))
    {
        passed = FALSE;
        sprintf(msg, "%d", s);
        testFail(verbose, msg);
    }
    else if (test_checkSemCount(s, 1))
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }

    ready(apid =
          create((void *)test_semWaiter, INITSTK, getprio(getpid()) + 10,
                 "SEMAPHORE-A", 3, s, 1, &testResult), RESCHED_NO);
    ready(bpid =
          create((void *)test_semWaiter, INITSTK, getprio(getpid()) + 10,
                 "SEMAPHORE-B", 3, s, 1, &testResult), RESCHED_YES);

    testPrint(verbose, "Wait on semaphore: ");
    /* Process A should be admitted, but B should wait. */
    if (test_checkProcState(apid, PR_FREE)
        && test_checkProcState(bpid, PR_WAIT)
        && test_checkSemCount(s, -1) && test_checkResult(testResult, 1))
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }

    /* Process B waited, so signal should release it. */
    testPrint(verbose, "Signal waiting semaphore: ");
    if ((OK == signal(s))
    	&& test_checkProcState(bpid, PR_FREE)
        && test_checkSemCount(s, 0) && test_checkResult(testResult, 2))
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

    /* Processes should be dead, but in case the test failed. */
    kill(apid);
    kill(bpid);
    semdelete(s);

    return OK;
}
