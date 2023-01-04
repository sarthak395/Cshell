#include "headers.h"
#include "discover.h"

int countfiles = 0;
void listFilesRecursively(char *basePath, char *strings[],char *basepathstring[])
{
    char path[1000];
    struct dirent *dp;
    
    DIR *dir = opendir(basePath);
   
    // Unable to open directory stream
    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL)
    {
        if (dp->d_name[0]!='.')
        {
            strings[countfiles]=dp->d_name;
            basepathstring[countfiles]=(char *)malloc(strlen(basePath)+1);
            strcpy(basepathstring[countfiles],basePath);
            countfiles++;

            // Construct new path from our base path
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);


            listFilesRecursively(path, strings,basepathstring);
        }
    }

    closedir(dir);
}

void discover(char **args)
{
    countfiles=0;
    char *targetdir = NULL;

    char *flags[5] = {NULL};
    char *filename = NULL;

    int i = 1, flagcount = 0;
    while (args[i] != NULL)
    {
        if (args[i][0] == '-')
            flags[flagcount++]=args[i];
        else if (args[i][0] == '"')
        {
            filename=args[i];
        }
        else
        {
            targetdir=args[i];
        }
        i++;
    }

    if(targetdir==NULL)
    {
        targetdir=(char *)malloc(5);
        strcpy(targetdir,".");
    }

    if(strcmp(targetdir,"~")==0)
        strcpy(targetdir,initdir);
    
    // get all the files and directories of target directory
    char *strings[10000]={NULL};
    char *basepathstrings[10000]={NULL};
    listFilesRecursively(targetdir, strings,basepathstrings);

    // we have all the files in the directories or sub-directories
    // NOW DO FLAG HANDLING AND SEARCH FOR FILE
    if ((flagcount == 0) || (flagcount == 2)) // no flag or both the flags -d , -f
    {
        // search everywhere
        // search for file, remember file name is in quotes
        for (int k = 0; k < countfiles; k++)
        {
            if ((filename==NULL) || (strncmp(strings[k], &filename[1], strlen(filename)-2) == 0))
                printf("%s/%s\n", basepathstrings[k],strings[k]);
        }
    }
    else // flagcount==1
    {
        for(int k=0;k<countfiles;k++)
        {
            // check if a file is a directory or not
            char path[5000];

            strcpy(path,basepathstrings[k]);
            strcat(path,"/");
            strcat(path,strings[k]);

            DIR *dir=opendir(path);
            int isdirectory=0;
            if(dir)
                isdirectory=1;

            if(strcmp(flags[0],"-d")==0 && isdirectory==0) // it is a file but we need to search for directories
                continue;
            if(strcmp(flags[0],"-f")==0 && isdirectory==1) // it is a directory but we need to search for files
                continue;
            
            if ((filename==NULL) || (strncmp(strings[k], &filename[1], strlen(filename)-2) == 0))
                printf("%s/%s\n", basepathstrings[k],strings[k]);
        }
    }
}
