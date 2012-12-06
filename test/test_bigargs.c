#include <xinu.h>
#include <testsuite.h>
#include <stdio.h>

static void bigargs(byte a, byte b, byte c, byte d, byte e, byte f,
                    byte *testArray)
{
    testArray[0] = a;
    testArray[1] = b;
    testArray[2] = c;
    testArray[3] = d;
    testArray[4] = e;
    testArray[5] = f;
    testArray[6] = a + b + c + d + e + f;
    return;
}

process test_bigargs(bool8 verbose)
{
    byte testArray[20];

    ready(create((void *)bigargs, INITSTK, 31, "BIGARGS",
                 7, 10, 20, 30, 40, 50, 60, testArray), RESCHED_YES);

    if ((10 == testArray[0])
        && (20 == testArray[1])
        && (30 == testArray[2])
        && (40 == testArray[3])
        && (50 == testArray[4])
        && (60 == testArray[5]) && (210 == testArray[6]))
    {
        testPass(TRUE, "");
    }
    else
    {
        testFail(TRUE, "");
    }

    return OK;
}
