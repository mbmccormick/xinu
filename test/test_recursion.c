#include <xinu.h>
#include <testsuite.h>
#include <stdio.h>

#define TIMES 5

static void test5(int times, byte *testArray, int *index)
{
    if (times > 0)
    {
        // Sharing index in this way is a race condition, but is
        // unlikely to go wrong with reasonable preemption quanta.
        testArray[*index] = currpid;
        *index = *index + 1;
        testArray[*index] = times;
        *index = *index + 1;
        yield();
        test5(times - 1, testArray, index);
    }
}

process test_recursion(bool8 verbose)
{
    pid32 apid, bpid, cpid, dpid;
    int i, j;
    bool8 passed = TRUE;
    byte testArray[TIMES * 8];
    char str[50];
    int index = 0;
    intmask mask;

    mask = disable();
    ready(apid = create((void *)test5, INITSTK, 31, "RECURSION-A",
                        3, TIMES, testArray, &index), 0);
    ready(bpid = create((void *)test5, INITSTK, 32, "RECURSION-B",
                        3, TIMES, testArray, &index), 0);
    ready(cpid = create((void *)test5, INITSTK, 34, "RECURSION-C",
                        3, TIMES, testArray, &index), 0);
    ready(dpid = create((void *)test5, INITSTK, 32, "RECURSION-D",
                        3, TIMES, testArray, &index), 0);
    restore(mask);
    yield();

//      for (i = 0; i < 8 * TIMES; i++)
//      { printf("testArray[%d] == %d\n", i, testArray[i]); }

    for (i = 0, j = TIMES; i < TIMES; i += 2, j--)
    {
        if (cpid != testArray[i])
        {
            passed = FALSE;
            sprintf(str,
                    "Expected testArray[%d] == %d, not %d\n",
                    i, cpid, testArray[i]);
            testFail(verbose, str);
        }
        if (j != testArray[i + 1])
        {
            passed = FALSE;
            sprintf(str,
                    "Expected testArray[%d] == %d, not %d\n",
                    i + 1, j, testArray[i]);
            testFail(verbose, str);
        }
    }

    for (i = TIMES * 2, j = TIMES; i < 6 * TIMES; i += 4, j--)
    {
        if (bpid != testArray[i])
        {
            passed = FALSE;
            sprintf(str,
                    "Expected testArray[%d] == %d, not %d\n",
                    i, bpid, testArray[i]);
            testFail(verbose, str);
        }
        if (j != testArray[i + 1])
        {
            passed = FALSE;
            sprintf(str,
                    "Expected testArray[%d] == %d, not %d\n",
                    i + 1, j, testArray[i]);
            testFail(verbose, str);
        }
    }

    for (i = TIMES * 2 + 2, j = TIMES; i < 6 * TIMES; i += 4, j--)
    {
        if (dpid != testArray[i])
        {
            passed = FALSE;
            sprintf(str,
                    "Expected testArray[%d] == %d, not %d\n",
                    i, dpid, testArray[i]);
            testFail(verbose, str);
        }
        if (j != testArray[i + 1])
        {
            passed = FALSE;
            sprintf(str,
                    "Expected testArray[%d] == %d, not %d\n",
                    i + 1, j, testArray[i]);
            testFail(verbose, str);
        }
    }

    for (i = 6 * TIMES, j = TIMES; i < 8 * TIMES; i += 2, j--)
    {
        if (apid != testArray[i])
        {
            passed = FALSE;
            sprintf(str,
                    "Expected testArray[%d] == %d, not %d\n",
                    i, apid, testArray[i]);
            testFail(verbose, str);
        }
        if (j != testArray[i + 1])
        {
            passed = FALSE;
            sprintf(str,
                    "Expected testArray[%d] == %d, not %d\n",
                    i + 1, j, testArray[i]);
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
