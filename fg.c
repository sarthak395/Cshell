#include "headers.h"
#include "fg.h"

void fg(int backgroundprocessesid[], int numberofbackgroundprocess, int backgroundprocessno)
{
    if(backgroundprocessno>numberofbackgroundprocess)
        perror("Invalid process no");
    
    int status;
    setpgid(backgroundprocessesid[backgroundprocessno - 1], getpgid(0)); // setting group id which has control of terminal as of now

    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);

    tcsetpgrp(0, backgroundprocessesid[backgroundprocessno - 1]); // giving foreground control to child

    if (kill(backgroundprocessesid[backgroundprocessno - 1], SIGCONT) < 0)
    {
        perror("Error : Can't Kill");
    }

    waitpid(backgroundprocessesid[backgroundprocessno - 1], &status, WUNTRACED);

    tcsetpgrp(0, getpgid(0)); // regaining control

    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
}