/*
 * FILENAME		:	pipe.c
 * DESCRIPTION	:	Functions to implement pipes on NetWare.
 * Author		:	Anantha Kesari H Y, Venkat Raghavan S, Srivathsa M
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
	int               err, count;
	char              pszPipestr[32] = {'\0'};
	char *command = NULL, *argv[MAX_ARGS] = {'\0'};
	int fd = -1;
	fd_set myfds;
	wiring_t wiring;
	pid_t pid=0;
	FILE *fp=NULL;
	char *ptr = NULL;
	int ptrLen = 0, argc = 0, i = 0;


	/* Get a temporary name */
	(void) tmpnam(strecpy(pszPipestr,  "PHP/php$pipe/"));

	wiring.infd=FD_UNUSED;
	wiring.outfd=FD_UNUSED;
	wiring.errfd=FD_UNUSED;

	/* Open a pipe */
	if ( *mode=='r') {
		fd = pipe_open (pszPipestr, O_RDONLY);
		if (fd == -1)
			return NULL;

		wiring.outfd=fd;
	} else if (*mode=='w') {
		fd = pipe_open (pszPipestr, O_WRONLY);
		if (fd == -1)
			return NULL;

		wiring.infd=fd;
	} else {
		consoleprintf ("Unsupported pipe open mode \n");
		return NULL;
	}

	/* Get the file pointer */
	fp = fdopen(fd, mode);
	if (fp == NULL) {
		consoleprintf ("Failure in fdopen \n");
		close (fd);
		return NULL;
	}

	/* Separate commandline string into words */
	ptr = strtok((char*)commandline, WHITESPACE);
	ptrLen = strlen(ptr);

	/* Get the command */
	command = (char*)malloc(ptrLen + 1);
	if(command == NULL) {
		consoleprintf ("Failure in memory allocation \n");
		close (fd);
		fclose (fp);
		return NULL;
	}
	strcpy (command, ptr);

	/* Command as the first argument into prcessve */
	argv[argc] = (char*)malloc(ptrLen + 1);
	if(argv[argc] == NULL) {
		consoleprintf ("Failure in memory allocation \n");
		close (fd);
		fclose (fp);
		if(command) {
			free(command);
			command = NULL;
		}
		return NULL;
	}
	strcpy (argv[argc], ptr);
	argc++;

	/* Get more arguments if any to be passed to prcessve */
	ptr = strtok(NULL, WHITESPACE);
	while (ptr && (argc < MAX_ARGS))
	{
		ptrLen = strlen(ptr);

		argv[argc] = (char*)malloc(ptrLen + 1);
		if(argv[argc] == NULL) {
			consoleprintf ("Failure in memory allocation \n");
			close (fd);
			fclose (fp);
			if(command) {
				free(command);
				command = NULL;
			}
			return NULL;
		}
		strcpy (argv[argc], ptr);
		argc++;

		ptr = strtok(NULL, WHITESPACE);
	}
	argv[argc] = NULL;

	FD_ZERO(&myfds);
	FD_SET(fd, &myfds);

	pid = processve(command, PROC_CURRENT_SPACE, NULL, &wiring,
					&myfds, NULL, (const char **)argv );
	if (pid == -1) {
		consoleprintf ("Failure in processve call \n");
		close (fd);
		fclose(fp);
		if(command) {
			free(command);
			command = NULL;
		}
		for(i=0; i<argc; i++) {
			if(argv[i]) {
				free(argv[i]);
				argv[i] = NULL;
			}
		}
		return NULL;
	}

	close (fd);
	if(command) {
		free(command);
		command = NULL;
	}
	for(i=0; i<argc; i++) {
		if(argv[i]) {
			free(argv[i]);
			argv[i] = NULL;
		}
	}

	return fp;
}


int pclose(FILE* stream)
{
	fclose(stream);
	return 0;
}



#if 0

FILE* popen2(const char* commandline, const char* mode)
{
    int               err, count, var, replaced;
    char              **env, pszPipestr[32];
    NXVmId_t          newVM;
    NXExecEnvSpec_t   envSpec;
    char *command = NULL, *argv[MAX_ARGS] = {"\0"};
    NXHandle_t fd;
    
    fd = (NXHandle_t) -1;
    
    count = NXGetEnvCount() + 1;  // (add one for NULL)
    env   = (char **) NXMemAlloc(sizeof(char *) * count, 0);
    
    if (!env)
        return 0;
    
    err = NXCopyEnv(env, count);  // ensure NULL added at end
    if ( *mode='r'){
        (void) tmpnam(strecpy(pszPipestr,  "PHPPIPE/stdin/"));
        err = NXFifoOpen(0, pszPipestr, NX_O_RDONLY, 0, &fd);
        if (err)
            return 0;
        
        envSpec.esStdin.ssType     = NX_OBJ_CONSOLE;
        envSpec.esStdout.ssType    = NX_OBJ_FIFO;
        envSpec.esStdout.ssPath    = pszPipestr;
    }
    else if (*mode='w') then{
        (void) tmpnam(strecpy(pszPipestr, "PHPPIPE/stdout/"));
        err = NXFifoOpen(0, instr, NX_O_WRONLY, 0, &fd);
        if (err)
            return 0;
        envSpec.esStdin.ssType     = NX_OBJ_FIFO;
        envSpec.esStdout.ssType    = NX_OBJ_CONSOLE;
		envSpec.esStdin.ssPath     =  pszPipestr;
    }
    else
        consoleprintf ("Unsupported pipe open mode \n");
    
    
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
/*    consoleprintf ("PHP | popen: command = %s\n", command); */
    
	//

    envSpec.esArgc             = argc;
    envSpec.esArgv             = argv;
    envSpec.esEnv              = env;
    
    envSpec.esStderr.ssType    = NX_OBJ_CONSOLE;
    
    envSpec.esStdin.ssHandle   =
        envSpec.esStdout.ssHandle  =
        envSpec.esStderr.ssHandle  = -1;
    
    envSpec.esStdin.ssPathCtx  =
        envSpec.esStdout.ssPathCtx =
        envSpec.esStderr.ssPathCtx = NULL;
    
    envSpec.esStderr.ssPath    = NULL;
    
    err = NXVmSpawn(NULL, command, &envSpec, NX_VM_CREATE_DETACHED, &newVM);
    return fd;
    
}

int pclose2(FILE* stream)
{
	NXClose(*(NXHandle_t *)stream);
    return 0;
}

FILE* popen1(const char* commandline, const char* mode)
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

int pclose1(FILE* stream)
{
    return (fclose(stream));
}

# endif
