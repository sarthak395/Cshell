#include "headers.h"

void echo(char **str)
{
    int i = 0;
    while (str[i] != NULL)
    {
        printf("%s ", str[i]);
        i++;
    }
    printf("\n");
}