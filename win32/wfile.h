#include  <io.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <errno.h>

#define access _access
#define WFILE_EXISTS	0
#ifndef ENOENT
#define ENOENT	136
#endif
#ifndef EINVAL
#define EINVAL	131
#endif

int readlink(char *, char *, int);
int checkroot(char *path);
