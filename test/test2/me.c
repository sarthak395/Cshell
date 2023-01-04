#include <stdio.h>
#include <string.h>

int main()
{
    char input[1000];
    scanf("%[^\n]%*c", input);

    char delimiters[20]="|";
    char *token = strtok(input, delimiters);

    printf("%s\n",input);
    printf("%s\n",token);
}