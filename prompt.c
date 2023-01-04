#include "prompt.h"
#include "headers.h"

char *username = NULL;
char *systemname = NULL;
char initdir[256]="";
char prevdir[256]="";
int shellpid=0;

void initialise()
{
    struct utsname uts;
    uname(&uts);

    username = (char *)malloc(sizeof(char) * 100);
    systemname = (char *)malloc(sizeof(char) * 100);

    strcpy(username, uts.nodename);
    strcpy(systemname, uts.sysname);
    getcwd(initdir,sizeof(initdir)); 
    strcpy(prevdir,initdir);
    shellpid=getpid();
}

void prompt()
{
    char buff[256];
    getcwd(buff,sizeof(buff));

    char curdir[100]="~";

    strcat(curdir,&buff[strlen(initdir)]);
    
    printf("\033[1;32m<%s@\033[0m", username);
    printf("\033[1;33m%s:\033[0m", systemname);
    printf("\033[1;36m%s \033[0m", curdir);
}