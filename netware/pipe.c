
/*
 * FILENAME		:	pipe.c
 * DESCRIPTION	:	Functions to implement pipes on NetWare.
 * Author		:	Anantha Kesari H Y, Venkat Raghavan S
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "netware/pipe.h"
#include "netware/mktemp.h"


/* Following definitions unavailable in LibC, hence borrowed from CLib */
#define P_WAIT      0
#define P_NOWAIT    1

#define WHITESPACE  " \t"
#define MAX_ARGS    10


FILE* popen(const char* commandline, const char* mode)
{
    char *command = NULL, *argv[MAX_ARGS] = {"\0"};
	char *tempName = "phpXXXXXX.tmp";
    char *filePath = NULL;
    FILE *fp = NULL;
    char *ptr = NULL;
    int ptrLen = 0, argc = 0, i = 0;

    /* Check for validity of input parameters */
    if (!commandline || !mode)
        return NULL;

    /* Get temporary file name */
    filePath = mktemp(tempName);
	if (!filePath)
		return NULL;
	consoleprintf("filePath = %s\n", filePath);

    /* Separate commandline string into words */

    ptr = strtok((char*)commandline, WHITESPACE);
    ptrLen = strlen(ptr);

    command = (char*)malloc(ptrLen + 1);
    strcpy (command, ptr);

    ptr = strtok(NULL, WHITESPACE);
    while (ptr && (argc < MAX_ARGS))
    {
        ptrLen = strlen(ptr);

        argv[argc] = (char*)malloc(ptrLen + 1);
        strcpy (argv[argc], ptr);

        argc++;

        ptr = strtok(NULL, WHITESPACE);
    }
/*consoleprintf ("PHP | popen: command = %s\n", command);*/
    if(strchr(mode,'r') != 0)
    {
        /*spawnvp(P_WAIT, command, argv);*/

        fp = fopen(filePath, "r"); /* Get the file handle of the pipe */
    }
	else if(strchr(mode,'w') != 0)
        fp = fopen(filePath, "w"); /* Get the file handle of the pipe */

    /* Free resources */
    free (command);
    for (i = 0; i < argc; i++)
    {
/*consoleprintf ("%s\t", argv[i]);*/
        free (argv[i]);
    }
/*consoleprintf ("\n");*/

	return fp;  /* Return the file handle.*/
}


int pclose(FILE* stream)
{
    return (fclose(stream));
}
