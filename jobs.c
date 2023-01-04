#include "jobs.h"
#include "headers.h"

void jobs(char *backgroundprocesses[], int backgroundprocessesid[], int numberofbackgroundprocess, char **args)
{
    for (int i = 0; i < numberofbackgroundprocess; i++)
    {
        char statpath[40];
        snprintf(statpath, 40, "/proc/%d/stat", backgroundprocessesid[i]);

        FILE *fp = fopen(statpath, "r");
        if (!fp)
            continue;
        char status;

        fscanf(fp,
               "%*d %*s %c %*s %*d %*s %*s %*d %*s %*s %*s %*s %*s %*s %*s %*s %*s "
               "%*s %*s %*s %*s %*s %*d",
               &status);

        if (((status == 'R') || (status == 'S')) && ((args[1] == NULL) || (strcmp(args[1], "-r") == 0)))
            printf("[%d] Running %s [%d]\n", i + 1, backgroundprocesses[i], backgroundprocessesid[i]);
        else if ((status == 'T') && ((args[1] == NULL) || (strcmp(args[1], "-s") == 0) != 0))
            printf("[%d] Stopped %s [%d]\n", i + 1, backgroundprocesses[i], backgroundprocessesid[i]);

        fclose(fp);
    }
}