#include "ls.h"
#include "headers.h"

// ONLY COLOR CODING IS LEFT !!!

int comp(const void *a, const void *b)
{
    // return strcmp(*(const char**)a, *(const char**)b);
    return strcasecmp((*(struct dirent **)a)->d_name, (*(struct dirent **)b)->d_name);
}

void sortstrings(struct direct *strings[], int n)
{
    qsort(strings, n, sizeof(struct dirent *), comp);
}

void fullinfo(char *path, char *filename)
{
    struct stat fileStat;
    stat(path, &fileStat);

    // printing permissions
    printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
    printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
    printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
    printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
    printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
    printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
    printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
    printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
    printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");

    printf(" %d", fileStat.st_nlink); // printing hard links

    struct passwd *pw = getpwuid(fileStat.st_uid);
    struct group *gr = getgrgid(fileStat.st_gid);

    printf(" %s %s", pw->pw_name, gr->gr_name);

    printf(" %ld", fileStat.st_size); // printing size of file

    char time[50];
    strftime(time, 50, "%Y-%m-%d %H:%M:%S", localtime(&fileStat.st_mtime));
    printf(" %s ", time); // printing last modified time

    if (S_ISDIR(fileStat.st_mode))
        printf("\033[1;34m%s\n\033[0m", filename);
    else if ((fileStat.st_mode & S_IXUSR))
        printf("\033[1;32m%s\n\033[0m", filename);
    else
        printf("%s\n", filename);

    return;
}

void ls(char **args)
{
    // 1) FIRST DISTINGUISH BETWEEN FLAGS AND DIRECTORIES
    char *flags[5] = {NULL};
    char *dir[1000] = {NULL};

    int i = 1, flagcount = 0, dircount = 0;
    while (args[i] != NULL)
    {
        if (args[i][0] == '-') // it is a flag
            flags[flagcount++] = args[i++];
        else
        {
            if (strcmp(args[i], "~") == 0) // to account for some functionality
            {
                dir[dircount] = (char *)malloc(strlen(initdir) * sizeof(char));
                strcpy(dir[dircount], initdir);
                dircount++;
                i++;
                continue;
            }
            dir[dircount++] = args[i++];
        }
    }

    // multiple directories
    if (dir[0] == NULL) // if no file name given , we have to consider current directory
    {
        dir[0] = (char *)malloc(10);
        strcpy(dir[0], ".");
    }
    int dirn = 0;
    while (dir[dirn] != NULL)
    {
        if(dirn!=0)
            printf("\n\n");
        
        // for a directory dir[j]
        if (dir[1] != NULL) // if there are more than 1 directories
            printf("%s :\n", dir[dirn]);

        struct direct *pde;
        DIR *pdr = opendir(dir[dirn]);

        if (pdr == NULL)
        {
            // check for file
            if (access(dir[dirn], F_OK) == 0) // if file exists
                printf("%s\n\n\n", args[1]);
            else
                perror("");
            dirn++;
            continue;
        }

        struct direct *strings[1000];
        i = 0;
        while ((pde = readdir(pdr)) != NULL)
        {
            strings[i] = pde;
            i++;
        }

        sortstrings(strings, i); // got all the filenames

        // 3) NOW LET'S LOOK AT FLAGS
        // FIRST CHECK HOW MANY FLAGS ARE THERE
        if (flags[0] == NULL) // i.e no flags
        {
            // print as it is, hiding hidden files
            for (int j = 0; j < i; j++)
            {
                if ((strings[j]->d_name)[0] == '.') // first character is '.', i.e they are hidden files
                    continue;
                struct stat fileStat;
                char filepath[1000];
                strcpy(filepath, dir[dirn]);
                strcat(filepath, "/");
                strcat(filepath, strings[j]->d_name);
                stat(filepath, &fileStat);

                if (S_ISDIR(fileStat.st_mode))
                    printf("\033[1;34m%s\n\033[0m", strings[j]->d_name);
                else if ((fileStat.st_mode & S_IXUSR))
                    printf("\033[1;32m%s\n\033[0m", strings[j]->d_name);
                else
                    printf("%s\n", strings[j]->d_name);
            }
        }
        else if (flags[1] == NULL) // i.e a single flag is present
        {
            if (strcmp(flags[0], "-l") == 0)
            {
                for (int j = 0; j < i; j++)
                {
                    if ((strings[j]->d_name)[0] == '.') // first character is '.' , i.e hidden files
                        continue;

                    char filepath[1000];
                    strcpy(filepath, dir[dirn]);
                    strcat(filepath, "/");
                    strcat(filepath, strings[j]->d_name);
                    fullinfo(filepath, strings[j]->d_name);
                }
            }
            else if (strcmp(flags[0], "-a") == 0)
            {
                for (int j = 0; j < i; j++)
                {
                    struct stat fileStat;
                    char filepath[1000];
                    strcpy(filepath, dir[dirn]);
                    strcat(filepath, "/");
                    strcat(filepath, strings[j]->d_name);
                    stat(filepath, &fileStat);

                    if (S_ISDIR(fileStat.st_mode))
                        printf("\033[1;34m%s\n\033[0m", strings[j]->d_name);
                    else if ((fileStat.st_mode & S_IXUSR))
                        printf("\033[1;32m%s\n\033[0m", strings[j]->d_name);
                    else
                        printf("%s\n", strings[j]->d_name);
                }
            }
            else if (strcmp(flags[0], "-al") == 0 || strcmp(flags[0], "-la") == 0)
            {
                for (int j = 0; j < i; j++)
                {
                    char filepath[1000];
                    strcpy(filepath, dir[dirn]);
                    strcat(filepath, "/");
                    strcat(filepath, strings[j]->d_name);
                    fullinfo(filepath, strings[j]->d_name);
                }
            }
        }
        else // it is -l -a OR -a -l
        {
            for (int j = 0; j < i; j++)
            {
                char filepath[1000];
                strcpy(filepath, dir[dirn]);
                strcat(filepath, "/");
                strcat(filepath, strings[j]->d_name);
                fullinfo(filepath, strings[j]->d_name);
            }
        }
        closedir(pdr);
        dirn++;
        
    }
}