#include "sig.h"
#include "headers.h"

void sig(int backgroundprocessesid[],int numberofbackgroundprocess,int processno,int signalnumber)
{
    if(processno>numberofbackgroundprocess)
    {
        perror("Invalid Process Number\n");
        return;
    }

    kill(backgroundprocessesid[processno-1],signalnumber);
    return;
}