/*
 * fcgi_stdio.c --
 *
 *      FastCGI-stdio compatibility package
 *
 *
 * Copyright (c) 1996 Open Market, Inc.
 *
 * See the file "LICENSE.TERMS" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

#ifndef lint
static const char rcsid[] = "$Id$";
#endif /* not lint */

#include <errno.h>  /* for errno */
#include <stdarg.h> /* for va_arg */
#include <stdlib.h> /* for malloc */
#include <string.h> /* for strerror */

#include "fcgi_config.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef _WIN32
#define DLLAPI  __declspec(dllexport)
#endif

#include "fcgiapp.h"
#include "fcgios.h"
#include "fcgimisc.h"

#define NO_FCGI_DEFINES
#include "fcgi_stdio.h"
#undef NO_FCGI_DEFINES

#ifndef _WIN32

extern char **environ;

#ifdef HAVE_FILENO_PROTO
#include <stdio.h>
#else
extern int fileno(FILE *stream);
#endif

extern FILE *fdopen(int fildes, const char *type);
extern FILE *popen(const char *command, const char *type);
extern int pclose(FILE *stream);

#else /* _WIN32 */

#define popen _popen
#define pclose _pclose

#endif /* _WIN32 */

FCGI_FILE _fcgi_sF[3];


/*
 *----------------------------------------------------------------------
 *
 * FCGI_Accept --
 *
 *      Accepts a new request from the HTTP server and creates
 *      a conventional execution environment for the request.
 *
 *      If the application was invoked as a FastCGI server,
 *      the first call to FCGI_Accept indicates that the application
 *      has completed its initialization and is ready to accept
 *      a request.  Subsequent calls to FCGI_Accept indicate that
 *      the application has completed its processing of the
 *      current request and is ready to accept a new request.
 *
 *      If the application was invoked as a CGI program, the first
 *      call to FCGI_Accept is essentially a no-op and the second
 *      call returns EOF (-1).
 *
 * Results:
 *    0 for successful call, -1 for error (application should exit).
 *
 * Side effects:
 *      If the application was invoked as a FastCGI server,
 *      and this is not the first call to this procedure,
 *      FCGI_Accept first performs the equivalent of FCGI_Finish.
 *
 *      On every call, FCGI_Accept accepts the new request and
 *      reads the FCGI_PARAMS stream into an environment array,
 *      i.e. a NULL-terminated array of strings of the form
 *      ``name=value''.  It assigns a pointer to this array
 *      to the global variable environ, used by the standard
 *      library function getenv.  It creates new FCGI_FILE *s
 *      representing input from the HTTP server, output to the HTTP
 *      server, and error output to the HTTP server, and assigns these
 *      new files to stdin, stdout, and stderr respectively.
 *
 *      DO NOT mutate or retain pointers to environ or any values
 *      contained in it (e.g. to the result of calling getenv(3)),
 *      since these are freed by the next call to FCGI_Finish or
 *      FCGI_Accept.  In particular do not use setenv(3) or putenv(3)
 *      in conjunction with FCGI_Accept.
 *
 *----------------------------------------------------------------------
 */
static int acceptCalled = FALSE;
static int isCGI = FALSE;

int FCGI_Accept(void)
{
    if(!acceptCalled) {
        /*
         * First call to FCGI_Accept.  Is application running
         * as FastCGI or as CGI?
         */
        isCGI = FCGX_IsCGI();
        acceptCalled = TRUE;
        atexit(&FCGI_Finish);
    } else if(isCGI) {
        /*
         * Not first call to FCGI_Accept and running as CGI means
         * application is done.
         */
        return(EOF);
    }
    if(isCGI) {
        FCGI_stdin->stdio_stream = stdin;
        FCGI_stdin->fcgx_stream = NULL;
        FCGI_stdout->stdio_stream = stdout;
        FCGI_stdout->fcgx_stream = NULL;
        FCGI_stderr->stdio_stream = stderr;
        FCGI_stderr->fcgx_stream = NULL;
    } else {
        FCGX_Stream *in, *out, *error;
        FCGX_ParamArray envp;
        int acceptResult = FCGX_Accept(&in, &out, &error, &envp);
        if(acceptResult < 0) {
            return acceptResult;
        }
        FCGI_stdin->stdio_stream = NULL;
        FCGI_stdin->fcgx_stream = in;
        FCGI_stdout->stdio_stream = NULL;
        FCGI_stdout->fcgx_stream = out;
        FCGI_stderr->stdio_stream = NULL;
        FCGI_stderr->fcgx_stream = error;
        environ = envp;
    }
    return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * FCGI_Finish --
 *
 *      Finishes the current request from the HTTP server.
 *
 * Side effects:
 *
 *      Flushes any buffered output to the HTTP server.  Then frees
 *      all storage allocated by the previous call, including all
 *      storage reachable from the value of environ set by the previous
 *      call to FCGI_Accept.
 *
 *      DO NOT use stdin, stdout, stderr, or environ between calling
 *      FCGI_Finish and calling FCGI_Accept.
 *
 *      DO NOT mutate or retain pointers to environ or any values
 *      contained in it (e.g. to the result of calling getenv(3)),
 *      since these are freed by the next call to FCGI_Finish or
 *      FCGI_Accept.  In particular do not use setenv(3) or putenv(3)
 *      in conjunction with FCGI_Accept.
 *
 *----------------------------------------------------------------------
 */
void FCGI_Finish(void)
{
    if(!acceptCalled || isCGI) {
        return;
    }
    FCGX_Finish();
    FCGI_stdin->fcgx_stream = NULL;
    FCGI_stdout->fcgx_stream = NULL;
    FCGI_stderr->fcgx_stream = NULL;
    environ = NULL;
}

/*
 *----------------------------------------------------------------------
 *
 * FCGI_StartFilterData --
 *
 *
 *      The current request is for the filter role, and stdin is
 *      positioned at EOF of FCGI_STDIN.  The call repositions
 *      stdin to the start of FCGI_DATA.
 *      If the preconditions are not met (e.g. FCGI_STDIN has not
 *      been read to EOF), the call sets the stream error code to
 *      FCGX_CALL_SEQ_ERROR.
 *
 * Results:
 *      0 for a normal return, < 0 for error
 *
 *----------------------------------------------------------------------
 */
int FCGI_StartFilterData(void)
{
    if(FCGI_stdin->stdio_stream) {
        return -1;
    } else {
        return FCGX_StartFilterData(FCGI_stdin->fcgx_stream);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * FCGI_SetExitStatus --
 *
 *      Sets the exit status for the current request. The exit status
 *      is the status code the request would have exited with, had
 *      the request been run as a CGI program.  You can call
 *      FCGI_SetExitStatus several times during a request; the last call
 *      before the request ends (by calling FCGI_Accept) determines the
 *      value.
 *
 *----------------------------------------------------------------------
 */
void FCGI_SetExitStatus(int status)
{
    if(FCGI_stdin->fcgx_stream) {
        FCGX_SetExitStatus(status, FCGI_stdin->fcgx_stream);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * FCGI_perror --
 *
 *       Wrapper for function defined in H&S Section 11.2
 *
 *----------------------------------------------------------------------
 */
void FCGI_perror(const char *str)
{
    FCGI_fputs(str, FCGI_stderr);
    FCGI_fputs(": ", FCGI_stderr);
    FCGI_fputs(strerror(OS_Errno), FCGI_stderr);
    return;
}

/*
 *----------------------------------------------------------------------
 *
 * FCGI_OpenFromFILE --
 *
 *      Constructs a new FCGI_FILE * from the FILE *stream.
 *
 * Results:
 *	NULL if stream == NULL or storage could not be allocated,
 *      otherwise the new FCGI_FILE *.
 *
 *----------------------------------------------------------------------
 */
static FCGI_FILE *FCGI_OpenFromFILE(FILE *stream)
{
    FCGI_FILE *fp;

    if (stream == NULL)
        return NULL;

    fp = (FCGI_FILE *) malloc(sizeof(FCGI_FILE));
    if (fp != NULL)
    {
        fp->stdio_stream = stream;
        fp->fcgx_stream = NULL;
    }

    return fp;
}

/*
 *----------------------------------------------------------------------
 *
 * FCGI_fopen, FCGI_fclose, FCGI_fflush, FCGI_freopen --
 *
 *       Wrappers for functions defined in H&S Section 15.2
 *
 *----------------------------------------------------------------------
 */
FCGI_FILE *FCGI_fopen(const char *path, const char *mode)
{
    FILE * file = fopen(path, mode);
    FCGI_FILE * fcgi_file = FCGI_OpenFromFILE(file);

    if (file && !fcgi_file)
        fclose(file);

    return fcgi_file;
}

int FCGI_fclose(FCGI_FILE *fp)
{
    int n = EOF;
    if(fp->stdio_stream) {
        n = fclose(fp->stdio_stream);
        fp->stdio_stream = NULL;
    } else if(fp->fcgx_stream) {
        n = FCGX_FClose(fp->fcgx_stream);
        fp->fcgx_stream = NULL;
    }
    if((fp != FCGI_stdin) && (fp != FCGI_stdout) && (fp != FCGI_stderr)) {
        free(fp);
    }
    return n;
}

int FCGI_fflush(FCGI_FILE *fp)
{
    if(fp == NULL)
    	return fflush(NULL);
    if(fp->stdio_stream)
        return fflush(fp->stdio_stream);
    else if(fp->fcgx_stream)
        return FCGX_FFlush(fp->fcgx_stream);
    return EOF;
}

FCGI_FILE *FCGI_freopen(const char *path, const char *mode,
                        FCGI_FILE *fp)
{
    if(fp->stdio_stream) {
        if(freopen(path, mode, fp->stdio_stream) == NULL)
            return NULL;
        else
            return fp;
    } else if(fp->fcgx_stream) {
        (void) FCGX_FClose(fp->fcgx_stream);
        fp->stdio_stream = fopen(path, mode);
        if(fp->stdio_stream == NULL)
            return NULL;
        else {
	    fp->fcgx_stream = NULL;
            return fp;
	}
    }
    return NULL;
}

/*
 *----------------------------------------------------------------------
 *
 * FCGI_setvbuf, FCGI_setbuf --
 *
 *       Wrappers for functions defined in H&S Section 15.3
 *
 *----------------------------------------------------------------------
 */
int FCGI_setvbuf(FCGI_FILE *fp, char *buf, int bufmode, size_t size)
{
    if(fp->stdio_stream)
        return setvbuf(fp->stdio_stream, buf, bufmode, size);
    else {
        return -1;
    }
}

void FCGI_setbuf(FCGI_FILE *fp, char *buf)
{
    if(fp->stdio_stream)
        setbuf(fp->stdio_stream, buf);
}

/*
 *----------------------------------------------------------------------
 *
 * FCGI_fseek, FCGI_ftell, FCGI_rewind, FCGI_fgetpos, FCGI_fsetpos --
 *
 *       Wrappers for functions defined in H&S Section 15.5
 *
 *----------------------------------------------------------------------
 */
int FCGI_fseek(FCGI_FILE *fp, long offset, int whence)
{
    if(fp->stdio_stream)
        return fseek(fp->stdio_stream, offset, whence);
    else {
        OS_SetErrno(ESPIPE);
        return -1;
    }
}

int FCGI_ftell(FCGI_FILE *fp)
{
    if(fp->stdio_stream)
        return ftell(fp->stdio_stream);
    else {
        OS_SetErrno(ESPIPE);
        return -1;
    }
}

void FCGI_rewind(FCGI_FILE *fp)
{
    if(fp->stdio_stream)
        rewind(fp->stdio_stream);
    else
        OS_SetErrno(ESPIPE);
}

#ifdef HAVE_FPOS
int FCGI_fgetpos(FCGI_FILE *fp, fpos_t *pos)
{
    if(fp->stdio_stream)
        return fgetpos(fp->stdio_stream, pos);
    else {
        OS_SetErrno(ESPIPE);
        return -1;
    }
}

int FCGI_fsetpos(FCGI_FILE *fp, const fpos_t *pos)
{
    if(fp->stdio_stream)
        return fsetpos(fp->stdio_stream, pos);
    else {
        OS_SetErrno(ESPIPE);
        return -1;
    }
}
#endif

/*
 *----------------------------------------------------------------------
 *
 * FCGI_fgetc, FCGI_getchar, FCGI_ungetc --
 *
 *       Wrappers for functions defined in H&S Section 15.6
 *
 *       XXX: getc and getchar are generally defined as macros
 *            for performance reasons
 *
 *----------------------------------------------------------------------
 */
int FCGI_fgetc(FCGI_FILE *fp)
{
    if(fp->stdio_stream)
        return fgetc(fp->stdio_stream);
    else if(fp->fcgx_stream)
        return FCGX_GetChar(fp->fcgx_stream);
    return EOF;
}

int FCGI_getchar(void)
{
    return FCGI_fgetc(FCGI_stdin);
}

int FCGI_ungetc(int c, FCGI_FILE *fp)
{
    if(fp->stdio_stream)
        return ungetc(c, fp->stdio_stream);
    else if(fp->fcgx_stream)
        return FCGX_UnGetChar(c, fp->fcgx_stream);
    return EOF;
}

/*
 *----------------------------------------------------------------------
 *
 * FCGI_fgets, FCGI_gets --
 *
 *       Wrappers for functions defined in H&S Section 15.7
 *
 *----------------------------------------------------------------------
 */
char *FCGI_fgets(char *str, int size, FCGI_FILE *fp)
{
    if(fp->stdio_stream)
        return fgets(str, size, fp->stdio_stream);
    else if(fp->fcgx_stream)
        return FCGX_GetLine(str, size, fp->fcgx_stream);
    return NULL;
}

/*
 * The gets() function reads characters from the standard input stream
 * into the array pointed to by str until a newline character is read
 * or an end-of-file condition is encountered.  The newline character
 * is discarded and the string is terminated with a null character. 
 */
char *FCGI_gets(char *str)
{
    char *s;
    int c;

    for (s = str; ((c = FCGI_getchar()) != '\n');) {
        if(c == EOF) {
            if(s == str)
                return NULL;
            else
                break;
        } else
            *s++ = (char) c;
    }
    *s = 0;
    return str;
}

/*
 *----------------------------------------------------------------------
 *
 *       Wrappers for functions defined in H&S Section 15.8
 *
 *       XXX: missing: fscanf, scanf
 *
 *----------------------------------------------------------------------
 */

/*
 *----------------------------------------------------------------------
 *
 * FCGI_fputc, FCGI_putchar --
 *
 *       Wrappers for functions defined in H&S Section 15.9
 *
 *       XXX: putc and putchar are generally defined as macros
 *            for performance reasons
 *
 *----------------------------------------------------------------------
 */
int FCGI_fputc(int c, FCGI_FILE *fp)
{
    if(fp->stdio_stream)
        return fputc(c, fp->stdio_stream);
    else if(fp->fcgx_stream)
        return FCGX_PutChar(c, fp->fcgx_stream);
    else return EOF;
}

int FCGI_putchar(int c)
{
    return FCGI_fputc(c, FCGI_stdout);
}

/*
 *----------------------------------------------------------------------
 *
 * FCGI_fputs, FCGI_puts
 *
 *       Wrappers for functions defined in H&S Section 15.10
 *
 *----------------------------------------------------------------------
 */
int FCGI_fputs(const char *str, FCGI_FILE *fp)
{
    if(fp->stdio_stream)
        return fputs(str, fp->stdio_stream);
    else if(fp->fcgx_stream)
        return FCGX_PutS(str, fp->fcgx_stream);
    return EOF;
}

int FCGI_puts(const char *str)
{
    int n;
    if(FCGI_stdout->stdio_stream) {
        n = fputs(str, FCGI_stdout->stdio_stream);
        if(n < 0)
            return n;
        else
            return fputc('\n', FCGI_stdout->stdio_stream);
    } else if(FCGI_stdout->fcgx_stream) {
        n = FCGX_PutS(str, FCGI_stdout->fcgx_stream);
        if(n < 0)
            return n;
        else
            return FCGX_PutChar('\n', FCGI_stdout->fcgx_stream);
    }
    return EOF;
}

/*
 *----------------------------------------------------------------------
 *
 * FCGI_fprintf, FCGI_printf --
 *
 *       Wrappers for functions defined in H&S Section 15.11
 *
 *----------------------------------------------------------------------
 */
int FCGI_fprintf(FCGI_FILE *fp, const char *format, ...)
{
    va_list ap;
    int n = 0;
    va_start(ap, format);
    if(fp->stdio_stream)
        n = vfprintf(fp->stdio_stream, format, ap);
    else if(fp->fcgx_stream)
        n = FCGX_VFPrintF(fp->fcgx_stream, format, ap);
    va_end(ap);
    return n;
}

int FCGI_printf(const char *format, ...)
{
    va_list ap;
    int n;
    va_start(ap, format);
    n = FCGI_vfprintf(FCGI_stdout, format, ap);
    va_end(ap);
    return n;
}

/*
 *----------------------------------------------------------------------
 *
 * FCGI_vfprintf, FCGI_vprintf --
 *
 *       Wrappers for functions defined in H&S Section 15.12
 *
 *----------------------------------------------------------------------
 */
int FCGI_vfprintf(FCGI_FILE *fp, const char *format, va_list ap)
{
    if(fp->stdio_stream)
        return vfprintf(fp->stdio_stream, format, ap);
    else if(fp->fcgx_stream)
        return FCGX_VFPrintF(fp->fcgx_stream, format, ap);
    return EOF;
}

int FCGI_vprintf(const char *format, va_list ap)
{
    if(FCGI_stdout->stdio_stream)
        return vfprintf(FCGI_stdout->stdio_stream, format, ap);
    else if(FCGI_stdout->fcgx_stream)
        return FCGX_VFPrintF(FCGI_stdout->fcgx_stream, format, ap);
    return EOF;
}

/*
 *----------------------------------------------------------------------
 *
 * FCGI_fread, FCGI_fwrite --
 *
 *       Wrappers for functions defined in H&S Section 15.13
 *
 *----------------------------------------------------------------------
 */
size_t FCGI_fread(void *ptr, size_t size, size_t nmemb, FCGI_FILE *fp)
{
    int n;
    if(fp->stdio_stream)
        return fread(ptr, size, nmemb, fp->stdio_stream);
    else if(fp->fcgx_stream) {
        if((size * nmemb) == 0) {
            return 0;
        }
        n = FCGX_GetStr((char *) ptr, size * nmemb, fp->fcgx_stream);
        return (n/size);
    }
    return (size_t)EOF;
}

size_t FCGI_fwrite(void *ptr, size_t size, size_t nmemb, FCGI_FILE *fp)
{
    int n;
    if(fp->stdio_stream)
        return fwrite(ptr, size, nmemb, fp->stdio_stream);
    else if(fp->fcgx_stream) {
        if((size * nmemb) == 0) {
            return 0;
        }
        n = FCGX_PutStr((char *) ptr, size * nmemb, fp->fcgx_stream);
        return (n/size);
    }
    return (size_t)EOF;
}

/*
 *----------------------------------------------------------------------
 *
 * FCGI_feof, FCGI_ferror, FCGI_clearerr --
 *
 *       Wrappers for functions defined in H&S Section 15.14
 *
 *----------------------------------------------------------------------
 */
int FCGI_feof(FCGI_FILE *fp)
{
    if(fp->stdio_stream) {
        return feof(fp->stdio_stream);
    } else if (fp->fcgx_stream){
        return FCGX_HasSeenEOF(fp->fcgx_stream);
    }
    return -1;

}

int FCGI_ferror(FCGI_FILE *fp)
{
    if(fp->stdio_stream) {
        return ferror(fp->stdio_stream);
    } else if(fp->fcgx_stream) {
        return FCGX_GetError(fp->fcgx_stream);
    }
    return -1;
}

void FCGI_clearerr(FCGI_FILE *fp)
{
    if(fp->stdio_stream) {
        clearerr(fp->stdio_stream);
    } else if(fp->fcgx_stream) {
        FCGX_ClearError(fp->fcgx_stream);
    }
    return;
}

/*
 *----------------------------------------------------------------------
 *
 * FCGI_tmpfile --
 *
 *       Wrappers for function defined in H&S Section 15.16
 *
 *----------------------------------------------------------------------
 */
FCGI_FILE *FCGI_tmpfile(void)
{
    FILE * file = tmpfile();
    FCGI_FILE * fcgi_file = FCGI_OpenFromFILE(file);

    if (file && !fcgi_file)
        fclose(file);

    return fcgi_file;
}

/*
 *----------------------------------------------------------------------
 *
 * FCGI_fileno, FCGI_fdopen, FCGI_popen, FCGI_pclose --
 *
 *       Wrappers for POSIX, X/OPEN functions not in ISO C
 *
 *----------------------------------------------------------------------
 */
int FCGI_fileno(FCGI_FILE *fp)
{
    if(fp->stdio_stream)
        return fileno(fp->stdio_stream);
    else
        return -1;
}

FCGI_FILE *FCGI_fdopen(int fd, const char *mode)
{
    FILE * file = fdopen(fd, mode);
    FCGI_FILE * fcgi_file = FCGI_OpenFromFILE(file);

    if (file && !fcgi_file)
        fclose(file);

    return fcgi_file;
}

FCGI_FILE *FCGI_popen(const char *cmd, const char *type)
{
    FILE * file = popen(cmd, type);
    FCGI_FILE * fcgi_file = FCGI_OpenFromFILE(file);

    if (file && !fcgi_file)
        pclose(file);

    return fcgi_file;
}

int FCGI_pclose(FCGI_FILE *fp)
{
    int n = EOF;
    if (fp->stdio_stream) {
        n = pclose(fp->stdio_stream);
        fp->stdio_stream = NULL;
    } else if(fp->fcgx_stream) {
        /*
         * The caller is deeply confused; don't free the storage.
         */
        return EOF;
    }
    if((fp != FCGI_stdin) && (fp != FCGI_stdout) && (fp != FCGI_stderr)) {
        free(fp);
    }
    return n;
}
