#include "headers.h"

void cd(char **args)
{
    /*
    1) cd .. goes to previous directory
    2) cd - goes to previously used directory
    3) cd ~ goes to root directory
    4) cd . does nothing
    5) if no argument present, cd into home directory
    */

   if(args[1]==NULL) // no argument is passed
        chdir(initdir);

    else if (args[2] != NULL) // more than 1 argument
        perror("Multiple arguments not Allowed in cd");

    else if (args[1] != NULL) // single argument
    {
        char buff[256];
        getcwd(buff,sizeof(buff));

        int retchdir=-1;

        if(strcmp(args[1],"-")==0) // Going to old working directory
            retchdir=chdir(prevdir);
        
        else if(strcmp(args[1],"~")==0) // go to home directory
            retchdir=chdir(initdir);
        
        else if(strcmp(args[1],".")==0) // do nothing
            ;
        
        else if(strcmp("..",args[1])==0 && strcmp(initdir,buff)==0)
            printf("%s\n",initdir);
        
        else // go to some other directory , " .. " is already implemented here
            retchdir=chdir(args[1]);
        
        if(retchdir==0) // on successful completion of chdir
            strcpy(prevdir,buff);
    }

    
    
}