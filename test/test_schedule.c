#include <xinu.h>
#include <testsuite.h>
#include <stdio.h>

#define TIMES 5

local void testproc(int times, byte *testArray, int *shared)
{
    int i = 0;

    for (i = 0; i < times; i++)
    {
        testArray[*shared] = currpid;
        *shared = *shared + 1;
        yield();
    }
}

process test_schedule(bool8 verbose)
{
    char str[50];
    byte testArray[TIMES * 4];
    int shared = 0;
    intmask mask;

    pid32 apid, bpid, cpid, dpid;
    bool8 passed = TRUE;
    int i;

    mask = disable();
    ready(apid = create((void *)testproc, INITSTK, 31, "PRIORITY-A",
                        3, TIMES, testArray, &shared), 0);
    ready(bpid = create((void *)testproc, INITSTK, 32, "PRIORITY-B",
                        3, TIMES, testArray, &shared), 0);
    ready(cpid = create((void *)testproc, INITSTK, 34, "PRIORITY-C",
                        3, TIMES, testArray, &shared), 0);
    ready(dpid = create((void *)testproc, INITSTK, 32, "PRIORITY-D",
                        3, TIMES, testArray, &shared), 0);
    restore(mask);
    yield();

    for (i = 0; i < TIMES; i++)
    {
        if (cpid != testArray[i])
        {
            passed = FALSE;
            sprintf(str,
                    "Expected testArray[%d] == %d, not %d\n",
                    i, cpid, testArray[i]);
            testFail(verbose, str);
        }
    }

    for (i = TIMES; i < 3 * TIMES; i += 2)
    {
        if (bpid != testArray[i])
        {
            passed = FALSE;
            sprintf(str,
                    "Expected testArray[%d] == %d, not %d\n",
                    i, bpid, testArray[i]);
            testFail(verbose, str);
        }
    }

    for (i = TIMES + 1; i < 3 * TIMES; i += 2)
    {
        if (dpid != testArray[i])
        {
            passed = FALSE;
            sprintf(str,
                    "Expected testArray[%d] == %d, not %d\n",
                    i, dpid, testArray[i]);
            testFail(verbose, str);
        }
    }

    for (i = 3 * TIMES; i < 4 * TIMES; i++)
    {
        if (apid != testArray[i])
        {
            passed = FALSE;
            sprintf(str,
                    "Expected testArray[%d] == %d, not %d\n",
                    i, apid, testArray[i]);
            testFail(verbose, str);
        }
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
