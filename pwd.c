#include "pwd.h"
#include "headers.h"

void pwd()
{
    char buff[256];
    getcwd(buff,sizeof(buff));
    printf("%s\n",buff);
}