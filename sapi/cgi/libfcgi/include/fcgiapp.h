/*
 * fcgiapp.h --
 *
 *      Definitions for FastCGI application server programs
 *
 *
 * Copyright (c) 1996 Open Market, Inc.
 *
 * See the file "LICENSE.TERMS" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * $Id$
 */

#ifndef _FCGIAPP_H
#define _FCGIAPP_H

/* Hack to see if we are building TCL - TCL needs varargs not stdarg */
#ifndef TCL_LIBRARY
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#ifndef DLLAPI
#ifdef _WIN32
#if defined(_LIB) || defined(FCGI_STATIC)
#define DLLAPI
#else
#define DLLAPI __declspec(dllimport)
#endif
#else
#define DLLAPI
#endif
#endif

#if defined (c_plusplus) || defined (__cplusplus)
extern "C" {
#endif

/*
 * Error codes.  Assigned to avoid conflict with EOF and errno(2).
 */
#define FCGX_UNSUPPORTED_VERSION -2
#define FCGX_PROTOCOL_ERROR -3
#define FCGX_PARAMS_ERROR -4
#define FCGX_CALL_SEQ_ERROR -5

/*
 * This structure defines the state of a FastCGI stream.
 * Streams are modeled after the FILE type defined in stdio.h.
 * (We wouldn't need our own if platform vendors provided a
 * standard way to subclass theirs.)
 * The state of a stream is private and should only be accessed
 * by the procedures defined below.
 */
typedef struct FCGX_Stream {
    unsigned char *rdNext;    /* reader: first valid byte
                               * writer: equals stop */
    unsigned char *wrNext;    /* writer: first free byte
                               * reader: equals stop */
    unsigned char *stop;      /* reader: last valid byte + 1
                               * writer: last free byte + 1 */
    unsigned char *stopUnget; /* reader: first byte of current buffer
                               * fragment, for ungetc
                               * writer: undefined */
    int isReader;
    int isClosed;
    int wasFCloseCalled;
    int FCGI_errno;                /* error status */
    void (*fillBuffProc) (struct FCGX_Stream *stream);
    void (*emptyBuffProc) (struct FCGX_Stream *stream, int doClose);
    void *data;
} FCGX_Stream;

/*
 * An environment (as defined by environ(7)): A NULL-terminated array
 * of strings, each string having the form name=value.
 */
typedef char **FCGX_ParamArray;

/*
 * A vector of pointers representing the parameters received
 * by a FastCGI application server, with the vector's length
 * and last valid element so adding new parameters is efficient.
 */

typedef struct Params {
    FCGX_ParamArray vec;    /* vector of strings */
    int length;		    /* number of string vec can hold */
    char **cur;		    /* current item in vec; *cur == NULL */
} Params;
typedef Params *ParamsPtr;

/*
 * FCGX_Request Flags
 *
 * Setting FCGI_FAIL_ACCEPT_ON_INTR prevents FCGX_Accept() from
 * restarting upon being interrupted.
 */
#define FCGI_FAIL_ACCEPT_ON_INTR	1

/*
 * FCGX_Request -- State associated with a request.
 *
 * Its exposed for API simplicity, I expect parts of it to change!
 */
typedef struct FCGX_Request {
    int requestId;            /* valid if isBeginProcessed */
    int role;
    FCGX_Stream *in;
    FCGX_Stream *out;
    FCGX_Stream *err;
	FCGX_ParamArray envp;

	/* Don't use anything below here */

    ParamsPtr paramsPtr;
    int ipcFd;               /* < 0 means no connection */
    int isBeginProcessed;     /* FCGI_BEGIN_REQUEST seen */
    int keepConnection;       /* don't close ipcFd at end of request */
    int appStatus;
    int nWriters;             /* number of open writers (0..2) */
	int flags;
	int listen_sock;
} FCGX_Request;


/*
 *======================================================================
 * Control
 *======================================================================
 */

/*
 *----------------------------------------------------------------------
 *
 * FCGX_IsCGI --
 *
 *      Returns TRUE iff this process appears to be a CGI process
 *      rather than a FastCGI process.
 *
 *----------------------------------------------------------------------
 */
DLLAPI int FCGX_IsCGI(void);

/*
 *----------------------------------------------------------------------
 *
 * FCGX_Init --
 *
 *      Initialize the FCGX library.  Call in multi-threaded apps
 *      before calling FCGX_Accept_r().
 *
 *      Returns 0 upon success.
 *
 *----------------------------------------------------------------------
 */
DLLAPI int FCGX_Init(void);

/*
 *----------------------------------------------------------------------
 *
 * FCGX_OpenSocket --
 *
 *	Create a FastCGI listen socket.
 *
 *	path is the Unix domain socket (named pipe for WinNT), or a colon
 *	followed by a port number.  e.g. "/tmp/fastcgi/mysocket", ":5000"
 *
 *	backlog is the listen queue depth used in the listen() call.
 *
 *  Returns the socket's file descriptor or -1 on error.
 *
 *----------------------------------------------------------------------
 */
DLLAPI int FCGX_OpenSocket(const char *path, int backlog);

/*
 *----------------------------------------------------------------------
 *
 * FCGX_InitRequest --
 *
 *	Initialize a FCGX_Request for use with FCGX_Accept_r().
 *
 * 	sock is a file descriptor returned by FCGX_OpenSocket() or 0 (default).
 * 	The only supported flag at this time is FCGI_FAIL_ON_INTR.
 *
 * 	Returns 0 upon success.
 *----------------------------------------------------------------------
 */
DLLAPI int FCGX_InitRequest(FCGX_Request *request, int sock, int flags);

/*
 *----------------------------------------------------------------------
 *
 * FCGX_Accept_r --
 *
 *      Accept a new request (multi-thread safe).  Be sure to call
 * 	FCGX_Init() first.
 *
 * Results:
 *	0 for successful call, -1 for error.
 *
 * Side effects:
 *
 *      Finishes the request accepted by (and frees any
 *      storage allocated by) the previous call to FCGX_Accept.
 *      Creates input, output, and error streams and
 *      assigns them to *in, *out, and *err respectively.
 *      Creates a parameters data structure to be accessed
 *      via getenv(3) (if assigned to environ) or by FCGX_GetParam
 *      and assigns it to *envp.
 *
 *      DO NOT retain pointers to the envp array or any strings
 *      contained in it (e.g. to the result of calling FCGX_GetParam),
 *      since these will be freed by the next call to FCGX_Finish
 *      or FCGX_Accept.
 *
 *	DON'T use the FCGX_Request, its structure WILL change.
 *
 *----------------------------------------------------------------------
 */
DLLAPI int FCGX_Accept_r(FCGX_Request *request);

/*
 *----------------------------------------------------------------------
 *
 * FCGX_Finish_r --
 *
 *      Finish the request (multi-thread safe).
 *
 * Side effects:
 *
 *      Finishes the request accepted by (and frees any
 *      storage allocated by) the previous call to FCGX_Accept.
 *
 *      DO NOT retain pointers to the envp array or any strings
 *      contained in it (e.g. to the result of calling FCGX_GetParam),
 *      since these will be freed by the next call to FCGX_Finish
 *      or FCGX_Accept.
 *
 *----------------------------------------------------------------------
 */
DLLAPI void FCGX_Finish_r(FCGX_Request *request);

/*
 *----------------------------------------------------------------------
 *
 * FCGX_Free --
 *
 *      Free the memory and, if close is true, 
 *	    IPC FD associated with the request (multi-thread safe).
 *
 *----------------------------------------------------------------------
 */
DLLAPI void FCGX_Free(FCGX_Request * request, int close);

/*
 *----------------------------------------------------------------------
 *
 * FCGX_Accept --
 *
 *      Accept a new request (NOT multi-thread safe).
 *
 * Results:
 *	0 for successful call, -1 for error.
 *
 * Side effects:
 *
 *      Finishes the request accepted by (and frees any
 *      storage allocated by) the previous call to FCGX_Accept.
 *      Creates input, output, and error streams and
 *      assigns them to *in, *out, and *err respectively.
 *      Creates a parameters data structure to be accessed
 *      via getenv(3) (if assigned to environ) or by FCGX_GetParam
 *      and assigns it to *envp.
 *
 *      DO NOT retain pointers to the envp array or any strings
 *      contained in it (e.g. to the result of calling FCGX_GetParam),
 *      since these will be freed by the next call to FCGX_Finish
 *      or FCGX_Accept.
 *
 *----------------------------------------------------------------------
 */
DLLAPI int FCGX_Accept(
        FCGX_Stream **in,
        FCGX_Stream **out,
        FCGX_Stream **err,
        FCGX_ParamArray *envp);

/*
 *----------------------------------------------------------------------
 *
 * FCGX_Finish --
 *
 *      Finish the current request (NOT multi-thread safe).
 *
 * Side effects:
 *
 *      Finishes the request accepted by (and frees any
 *      storage allocated by) the previous call to FCGX_Accept.
 *
 *      DO NOT retain pointers to the envp array or any strings
 *      contained in it (e.g. to the result of calling FCGX_GetParam),
 *      since these will be freed by the next call to FCGX_Finish
 *      or FCGX_Accept.
 *
 *----------------------------------------------------------------------
 */
DLLAPI void FCGX_Finish(void);

/*
 *----------------------------------------------------------------------
 *
 * FCGX_StartFilterData --
 *
 *      stream is an input stream for a FCGI_FILTER request.
 *      stream is positioned at EOF on FCGI_STDIN.
 *      Repositions stream to the start of FCGI_DATA.
 *      If the preconditions are not met (e.g. FCGI_STDIN has not
 *      been read to EOF) sets the stream error code to
 *      FCGX_CALL_SEQ_ERROR.
 *
 * Results:
 *      0 for a normal return, < 0 for error
 *
 *----------------------------------------------------------------------
 */
DLLAPI int FCGX_StartFilterData(FCGX_Stream *stream);

/*
 *----------------------------------------------------------------------
 *
 * FCGX_SetExitStatus --
 *
 *      Sets the exit status for stream's request. The exit status
 *      is the status code the request would have exited with, had
 *      the request been run as a CGI program.  You can call
 *      SetExitStatus several times during a request; the last call
 *      before the request ends determines the value.
 *
 *----------------------------------------------------------------------
 */
DLLAPI void FCGX_SetExitStatus(int status, FCGX_Stream *stream);

/*
 *======================================================================
 * Parameters
 *======================================================================
 */

/*
 *----------------------------------------------------------------------
 *
 * FCGX_GetParam -- obtain value of FCGI parameter in environment
 *
 *
 * Results:
 *	Value bound to name, NULL if name not present in the
 *      environment envp.  Caller must not mutate the result
 *      or retain it past the end of this request.
 *
 *----------------------------------------------------------------------
 */
DLLAPI char *FCGX_GetParam(const char *name, FCGX_ParamArray envp);
DLLAPI void FCGX_PutEnv(FCGX_Request *request, char *nameValue);

/*
 *======================================================================
 * Readers
 *======================================================================
 */

/*
 *----------------------------------------------------------------------
 *
 * FCGX_GetChar --
 *
 *      Reads a byte from the input stream and returns it.
 *
 * Results:
 *	The byte, or EOF (-1) if the end of input has been reached.
 *
 *----------------------------------------------------------------------
 */
DLLAPI int FCGX_GetChar(FCGX_Stream *stream);

/*
 *----------------------------------------------------------------------
 *
 * FCGX_UnGetChar --
 *
 *      Pushes back the character c onto the input stream.  One
 *      character of pushback is guaranteed once a character
 *      has been read.  No pushback is possible for EOF.
 *
 * Results:
 *	Returns c if the pushback succeeded, EOF if not.
 *
 *----------------------------------------------------------------------
 */
DLLAPI int FCGX_UnGetChar(int c, FCGX_Stream *stream);

/*
 *----------------------------------------------------------------------
 *
 * FCGX_GetStr --
 *
 *      Reads up to n consecutive bytes from the input stream
 *      into the character array str.  Performs no interpretation
 *      of the input bytes.
 *
 * Results:
 *	Number of bytes read.  If result is smaller than n,
 *      the end of input has been reached.
 *
 *----------------------------------------------------------------------
 */
DLLAPI int FCGX_GetStr(char *str, int n, FCGX_Stream *stream);

/*
 *----------------------------------------------------------------------
 *
 * FCGX_GetLine --
 *
 *      Reads up to n-1 consecutive bytes from the input stream
 *      into the character array str.  Stops before n-1 bytes
 *      have been read if '\n' or EOF is read.  The terminating '\n'
 *      is copied to str.  After copying the last byte into str,
 *      stores a '\0' terminator.
 *
 * Results:
 *	NULL if EOF is the first thing read from the input stream,
 *      str otherwise.
 *
 *----------------------------------------------------------------------
 */
DLLAPI char *FCGX_GetLine(char *str, int n, FCGX_Stream *stream);

/*
 *----------------------------------------------------------------------
 *
 * FCGX_HasSeenEOF --
 *
 *      Returns EOF if end-of-file has been detected while reading
 *      from stream; otherwise returns 0.
 *
 *      Note that FCGX_HasSeenEOF(s) may return 0, yet an immediately
 *      following FCGX_GetChar(s) may return EOF.  This function, like
 *      the standard C stdio function feof, does not provide the
 *      ability to peek ahead.
 *
 * Results:
 *	EOF if end-of-file has been detected, 0 if not.
 *
 *----------------------------------------------------------------------
 */

DLLAPI  int FCGX_HasSeenEOF(FCGX_Stream *stream);

/*
 *======================================================================
 * Writers
 *======================================================================
 */

/*
 *----------------------------------------------------------------------
 *
 * FCGX_PutChar --
 *
 *      Writes a byte to the output stream.
 *
 * Results:
 *	The byte, or EOF (-1) if an error occurred.
 *
 *----------------------------------------------------------------------
 */
DLLAPI int FCGX_PutChar(int c, FCGX_Stream *stream);

/*
 *----------------------------------------------------------------------
 *
 * FCGX_PutStr --
 *
 *      Writes n consecutive bytes from the character array str
 *      into the output stream.  Performs no interpretation
 *      of the output bytes.
 *
 * Results:
 *      Number of bytes written (n) for normal return,
 *      EOF (-1) if an error occurred.
 *
 *----------------------------------------------------------------------
 */
DLLAPI int FCGX_PutStr(const char *str, int n, FCGX_Stream *stream);

/*
 *----------------------------------------------------------------------
 *
 * FCGX_PutS --
 *
 *      Writes a null-terminated character string to the output stream.
 *
 * Results:
 *      number of bytes written for normal return,
 *      EOF (-1) if an error occurred.
 *
 *----------------------------------------------------------------------
 */
DLLAPI int FCGX_PutS(const char *str, FCGX_Stream *stream);

/*
 *----------------------------------------------------------------------
 *
 * FCGX_FPrintF, FCGX_VFPrintF --
 *
 *      Performs printf-style output formatting and writes the results
 *      to the output stream.
 *
 * Results:
 *      number of bytes written for normal return,
 *      EOF (-1) if an error occurred.
 *
 *----------------------------------------------------------------------
 */
DLLAPI int FCGX_FPrintF(FCGX_Stream *stream, const char *format, ...);

DLLAPI int FCGX_VFPrintF(FCGX_Stream *stream, const char *format, va_list arg);

/*
 *----------------------------------------------------------------------
 *
 * FCGX_FFlush --
 *
 *      Flushes any buffered output.
 *
 *      Server-push is a legitimate application of FCGX_FFlush.
 *      Otherwise, FCGX_FFlush is not very useful, since FCGX_Accept
 *      does it implicitly.  Calling FCGX_FFlush in non-push applications
 *      results in extra writes and therefore reduces performance.
 *
 * Results:
 *      EOF (-1) if an error occurred.
 *
 *----------------------------------------------------------------------
 */
DLLAPI int FCGX_FFlush(FCGX_Stream *stream);

/*
 *======================================================================
 * Both Readers and Writers
 *======================================================================
 */

/*
 *----------------------------------------------------------------------
 *
 * FCGX_FClose --
 *
 *      Closes the stream.  For writers, flushes any buffered
 *      output.
 *
 *      Close is not a very useful operation since FCGX_Accept
 *      does it implicitly.  Closing the out stream before the
 *      err stream results in an extra write if there's nothing
 *      in the err stream, and therefore reduces performance.
 *
 * Results:
 *      EOF (-1) if an error occurred.
 *
 *----------------------------------------------------------------------
 */
DLLAPI int FCGX_FClose(FCGX_Stream *stream);

/*
 *----------------------------------------------------------------------
 *
 * FCGX_GetError --
 *
 *      Return the stream error code.  0 means no error, > 0
 *      is an errno(2) error, < 0 is an FastCGI error.
 *
 *----------------------------------------------------------------------
 */
DLLAPI int FCGX_GetError(FCGX_Stream *stream);

/*
 *----------------------------------------------------------------------
 *
 * FCGX_ClearError --
 *
 *      Clear the stream error code and end-of-file indication.
 *
 *----------------------------------------------------------------------
 */
DLLAPI void FCGX_ClearError(FCGX_Stream *stream);

/*
 *----------------------------------------------------------------------
 *
 * FCGX_CreateWriter --
 *
 *      Create a FCGX_Stream (used by cgi-fcgi).  This shouldn't 
 *      be needed by a FastCGI applictaion.
 *
 *----------------------------------------------------------------------
 */
DLLAPI FCGX_Stream *FCGX_CreateWriter(
        int socket,
        int requestId,
        int bufflen,
        int streamType);

/*
 *----------------------------------------------------------------------
 *
 * FCGX_FreeStream --
 *
 *      Free a FCGX_Stream (used by cgi-fcgi).  This shouldn't 
 *      be needed by a FastCGI applictaion.
 *
 *----------------------------------------------------------------------
 */
DLLAPI void FCGX_FreeStream(FCGX_Stream **stream);

/* ----------------------------------------------------------------------
 *
 *  Prevent the lib from accepting any new requests.  Signal handler safe.
 *
 * ----------------------------------------------------------------------
 */
DLLAPI void FCGX_ShutdownPending(void);

#if defined (__cplusplus) || defined (c_plusplus)
} /* terminate extern "C" { */
#endif

#endif	/* _FCGIAPP_H */
