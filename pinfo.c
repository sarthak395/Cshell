#include "headers.h"
#include "pinfo.h"

int status;
void pinfo(char **args) // RUNS ONLY ON LINUX OS
{
    // CHECKING IF FOREGROUND OR BACKGROUND
    int foreground = 1;
    pid_t console_pid = tcgetpgrp(STDOUT_FILENO);
    pid_t my_pid = getpgrp();
    if (console_pid != my_pid)
        foreground = 0;

    // OPENING PROCESS STATUS , MEMORY AND EXECUTABLE PATH
    FILE *fp;
    char execlocation[100];
    char buf[128];
    char statpath[40],execpath[100];
    if(args[1]==NULL)
    {
        snprintf(statpath,40,"/proc/%d/stat",shellpid);
        snprintf(execpath,100,"/proc/%d/exe",shellpid);
    }
    else {
        snprintf(statpath,40,"/proc/%s/stat",args[1]);
        snprintf(execpath,100,"/proc/%s/exe",args[1]);
    }
    fp=fopen(statpath,"r");
    int execused=readlink(execpath,execlocation,100);
    execlocation[execused]='\0';
    
    if (!(fp))
    {
        printf("Can't open /proc file\n");
        return;
    }

    pid_t pid=-1;
    char status='-';
    int pgrp=-1;
    int tpgid=-1;
    int memory=-1;
    fscanf(fp,
           "%d %*s %c %*s %d %*s %*s %d %*s %*s %*s %*s %*s %*s %*s %*s %*s "
           "%*s %*s %*s %*s %*s %d",
           &pid, &status, &pgrp, &tpgid, &memory);

    printf("pid -- %d\nProcess Status -- %c%c\nmemory -- %d\n", pid, status,
           pgrp == tpgid ? '+' : ' ', memory);

    
    fclose(fp);

    //PRINTING FULL EXECUTABLE PATH
    printf("executable Path : %s\n",execlocation);

    return;
}