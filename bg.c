#include "headers.h"
#include "bg.h"

void bg(int backgroundprocessesid[],int numberofbackgroundprocess,int backgroundprocessno)
{
    // printf("background process no is %d and id is %d\n",backgroundprocessno,backgroundprocessesid[backgroundprocessno-1]);
    // change status of stopped background process to running
    if(kill(backgroundprocessesid[backgroundprocessno-1],SIGCONT)<0)
        perror("Error");
    
    return;
    
}