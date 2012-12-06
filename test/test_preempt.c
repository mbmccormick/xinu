#include <xinu.h>
#include <testsuite.h>
#include <stdio.h>

local process spin(void)
{
    while (TRUE)
        ;

    return OK;
}

/**
 * Example of a test program for the Xinu testsuite.  Beyond this file you
 * must add an entry to the testtab in testhelper.c and a prototype in
 * testsuite.h.
 */
process test_preempt(bool8 verbose)
{
    /* the failif macro depends on 'passed' and 'verbose' vars */
    bool8 passed = TRUE;
    pid32 pidspin;

    /* This is the first "subtest" of this suite */
    pidspin =
        create(spin, INITSTK, getprio(currpid), "test_spin", 0);

    /* Make spin ... spin */
    resume(pidspin);

    /* If this next line runs, we're good */
    kill(pidspin);

    /* always print out the overall tests status */
    if (passed)
    {
        testPass(TRUE, "");
    }
    else
    {
        testFail(TRUE, "");
    }

    return OK;
}
