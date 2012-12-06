#include <xinu.h>
#include <testsuite.h>
#include <stdio.h>

void test_semWaiter(sid32 s, int times, byte *testResult)
{
    while (times > 0)
    {
        wait(s);
        (*testResult)++;
        times--;
    }
}

bool8 test_checkSemCount(sid32 s, short c, bool8 verbose)
{
    char msg[50];

    if (!isbadsem(s) && c != semcount(s))
    {
        sprintf(msg, "count = %d, not %d", semcount(s), c);
        testFail(verbose, msg);
        return FALSE;
    }
    return TRUE;
}

bool8 test_checkProcState(pid32 pid, byte state, bool8 verbose)
{
    char msg[50];

    if (state != proctab[pid].prstate)
    {
        sprintf(msg, "pid %d state %d, not %d",
                pid, proctab[pid].prstate, state);
        testFail(verbose, msg);
        return FALSE;
    }
    return TRUE;
}

bool8 test_checkResult(byte testResult, byte expected, bool8 verbose)
{
    char msg[80];

    if (expected != testResult)
    {
        sprintf(msg,
                "process didn't seem to wait, expected %d, saw %d",
                expected, testResult);
        testFail(verbose, msg);
        return FALSE;
    }
    return TRUE;
}

process test_semaphore(bool8 verbose)
{
    pid32 apid;
    bool8 passed = TRUE;
    sid32 s;
    byte testResult = 0;
    char msg[50];

    /* Single semaphore tests */
    testPrint(verbose, "Semaphore creation: ");
    s = semcreate(0);
    if (isbadsem(s))
    {
        passed = FALSE;
        sprintf(msg, "%d", s);
        testFail(verbose, msg);
    }
    else if (test_checkSemCount(s, 0, verbose))
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }

    testPrint(verbose, "Wait on semaphore: ");
    if ((SYSERR != resume(apid =
          create((void *)test_semWaiter, INITSTK, 31,
                 "SEMAPHORE-A", 3, s, 1, &testResult)))
    	&& test_checkProcState(apid, PR_WAIT, verbose)
        && test_checkSemCount(s, -1, verbose)
        && test_checkResult(testResult, 0, verbose))
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }


    testPrint(verbose, "Signal semaphore: ");
    if ((OK == signal(s))
    	&& test_checkProcState(apid, PR_FREE, verbose)
        && test_checkSemCount(s, 0, verbose)
        && test_checkResult(testResult, 1, verbose))
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }

    testPrint(verbose, "Signaln semaphore (valid count): ");
    if ((OK == signaln(s, 5))
    	&& test_checkProcState(apid, PR_FREE, verbose)
        && test_checkSemCount(s, 5, verbose)
        && test_checkResult(testResult, 1, verbose))
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }

    testPrint(verbose, "Signaln semaphore (invalid count): ");
    if (SYSERR == signaln(s, -5))
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }


    /* Free semaphore, single semaphore tests */
    testPrint(verbose, "Delete valid semaphore: ");
    if ((OK == semdelete(s))
	&& (semtab[s].sstate == S_FREE)
	&& isempty(semtab[s].squeue))
    {
	testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }

    testPrint(verbose, "Delete invalid semaphore: ");
    if (SYSERR == semdelete(-1))
    {   
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }
 
    testPrint(verbose, "Delete free semaphore: ");
    if (SYSERR == semdelete(s))
    {   
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }
 
    testPrint(verbose, "Signal bad semaphore id: ");
    if (SYSERR == signal(-1))
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }

    testPrint(verbose, "Signal free semaphore: ");
    if (SYSERR == signal(s))
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }


    testPrint(verbose, "Signaln bad semaphore id: ");
    if (SYSERR == signaln(-1, 4))
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }

    testPrint(verbose, "Signaln free semaphore: ");
    if (SYSERR == signaln(s, 4))
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }

    testPrint(verbose, "Wait on bad semaphore id: ");
    if (SYSERR == wait(-1))
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }
 
    testPrint(verbose, "Wait on free semaphore: ");
    if (SYSERR == wait(s))
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
    }



    /* Process A should be dead, but in case the test failed. */
    kill(apid);

    /* General semaphore pass/fail */
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
