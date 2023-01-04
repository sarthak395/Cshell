#ifndef __LS_H
#define __LS_H
#include <sys/dir.h>

void sortstrings(struct dirent* strings[],int n);
void ls(char **args);

#endif