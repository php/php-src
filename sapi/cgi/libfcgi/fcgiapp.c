/*
 * fcgiapp.c --
 *
 *	FastCGI application library: request-at-a-time
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

#include <assert.h>
#include <errno.h>
#include <fcntl.h>      /* for fcntl */
#include <math.h>
#include <memory.h>     /* for memchr() */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "fcgi_config.h"

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h> /* for getpeername */
#endif

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

#ifdef _WIN32
#define DLLAPI  __declspec(dllexport)
#endif

#include "fcgimisc.h"
#include "fastcgi.h"
#include "fcgios.h"
#include "fcgiapp.h"

/*
 * This is a workaround for one version of the HP C compiler
 * (c89 on HP-UX 9.04, also Stratus FTX), which will dump core
 * if given 'long double' for varargs.
 */
#ifdef HAVE_VA_ARG_LONG_DOUBLE_BUG
#define LONG_DOUBLE double
#else
#define LONG_DOUBLE long double
#endif

/*
 * Globals
 */
static int libInitialized = 0;
static int isFastCGI = -1;
static char *webServerAddressList = NULL;
static FCGX_Request the_request;
void _FCGX_FreeStream(FCGX_Stream **streamPtr, int freeData);

void FCGX_ShutdownPending(void)
{
    OS_ShutdownPending();
}

static void *Malloc(size_t size)
{
    void *result = malloc(size);
    ASSERT(size == 0 || result != NULL);
    return result;
}

static char *StringCopy(char *str)
{
    int strLen = strlen(str);
    char *newString = (char *)Malloc(strLen + 1);
    memcpy(newString, str, strLen);
    newString[strLen] = '\000';
    return newString;
}


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
int FCGX_GetChar(FCGX_Stream *stream)
{
    if(stream->rdNext != stream->stop)
        return *stream->rdNext++;
    if(stream->isClosed || !stream->isReader)
        return EOF;
    stream->fillBuffProc(stream);
    stream->stopUnget = stream->rdNext;
    if(stream->rdNext != stream->stop)
        return *stream->rdNext++;
    ASSERT(stream->isClosed); /* bug in fillBufProc if not */
    return EOF;
}

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
int FCGX_GetStr(char *str, int n, FCGX_Stream *stream)
{
    int m, bytesMoved;

    if(n <= 0) {
        return 0;
    }
    /*
     * Fast path: n bytes are already available
     */
    if(n <= (stream->stop - stream->rdNext)) {
        memcpy(str, stream->rdNext, n);
        stream->rdNext += n;
        return n;
    }
    /*
     * General case: stream is closed or buffer fill procedure
     * needs to be called
     */
    bytesMoved = 0;
    for (;;) {
        if(stream->rdNext != stream->stop) {
            m = min(n - bytesMoved, stream->stop - stream->rdNext);
            memcpy(str, stream->rdNext, m);
            bytesMoved += m;
            stream->rdNext += m;
            if(bytesMoved == n)
                return bytesMoved;
            str += m;
	}
        if(stream->isClosed || !stream->isReader)
            return bytesMoved;
        stream->fillBuffProc(stream);
        stream->stopUnget = stream->rdNext;
    }
}

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
char *FCGX_GetLine(char *str, int n, FCGX_Stream *stream)
{
    int c;
    char *p = str;

    n--;
    while (n > 0) {
        c = FCGX_GetChar(stream);
        if(c == EOF) {
            if(p == str)
                return NULL;
            else
                break;
        }
        *p++ = (char) c;
        n--;
        if(c == '\n')
            break;
    }
    *p = '\0';
    return str;
}

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
int FCGX_UnGetChar(int c, FCGX_Stream *stream) {
    if(c == EOF
            || stream->isClosed
            || !stream->isReader
            || stream->rdNext == stream->stopUnget)
        return EOF;
    --(stream->rdNext);
    *stream->rdNext = (unsigned char) c;
    return c;
}

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
int FCGX_HasSeenEOF(FCGX_Stream *stream) {
    return (stream->isClosed) ? EOF : 0;
}

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
int FCGX_PutChar(int c, FCGX_Stream *stream)
{
    if(stream->wrNext != stream->stop)
        return (*stream->wrNext++ = (unsigned char) c);
    if(stream->isClosed || stream->isReader)
        return EOF;
    stream->emptyBuffProc(stream, FALSE);
    if(stream->wrNext != stream->stop)
        return (*stream->wrNext++ = (unsigned char) c);
    ASSERT(stream->isClosed); /* bug in emptyBuffProc if not */
    return EOF;
}

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
int FCGX_PutStr(const char *str, int n, FCGX_Stream *stream)
{
    int m, bytesMoved;

    /*
     * Fast path: room for n bytes in the buffer
     */
    if(n <= (stream->stop - stream->wrNext)) {
        memcpy(stream->wrNext, str, n);
        stream->wrNext += n;
        return n;
    }
    /*
     * General case: stream is closed or buffer empty procedure
     * needs to be called
     */
    bytesMoved = 0;
    for (;;) {
        if(stream->wrNext != stream->stop) {
            m = min(n - bytesMoved, stream->stop - stream->wrNext);
            memcpy(stream->wrNext, str, m);
            bytesMoved += m;
            stream->wrNext += m;
            if(bytesMoved == n)
                return bytesMoved;
            str += m;
	}
        if(stream->isClosed || stream->isReader)
            return -1;
        stream->emptyBuffProc(stream, FALSE);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * FCGX_PutS --
 *
 *      Writes a character string to the output stream.
 *
 * Results:
 *      number of bytes written for normal return,
 *      EOF (-1) if an error occurred.
 *
 *----------------------------------------------------------------------
 */
int FCGX_PutS(const char *str, FCGX_Stream *stream)
{
    return FCGX_PutStr(str, strlen(str), stream);
}

/*
 *----------------------------------------------------------------------
 *
 * FCGX_FPrintF --
 *
 *      Performs output formatting and writes the results
 *      to the output stream.
 *
 * Results:
 *      number of bytes written for normal return,
 *      EOF (-1) if an error occurred.
 *
 *----------------------------------------------------------------------
 */
int FCGX_FPrintF(FCGX_Stream *stream, const char *format, ...)
{
    int result;
    va_list ap;
    va_start(ap, format);
    result = FCGX_VFPrintF(stream, format, ap);
    va_end(ap);
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * FCGX_VFPrintF --
 *
 *      Performs output formatting and writes the results
 *      to the output stream.
 *
 * Results:
 *      number of bytes written for normal return,
 *      EOF (-1) if an error occurred.
 *
 *----------------------------------------------------------------------
 */

#define PRINTF_BUFFLEN 100
    /*
     * More than sufficient space for all unmodified conversions
     * except %s and %f.
     */
#define FMT_BUFFLEN 25
    /*
     * Max size of a format specifier is 1 + 5 + 7 + 7 + 2 + 1 + slop
     */
static void CopyAndAdvance(char **destPtr, char **srcPtr, int n);

int FCGX_VFPrintF(FCGX_Stream *stream, const char *format, va_list arg)
{
    char *f, *fStop, *percentPtr, *p, *fmtBuffPtr, *buffPtr;
    int op, performedOp, sizeModifier, buffCount = 0, buffLen, specifierLength;
    int fastPath, n, auxBuffLen = 0, buffReqd, minWidth, precision, exp;
    char *auxBuffPtr = NULL;
    int streamCount = 0;
    char fmtBuff[FMT_BUFFLEN];
    char buff[PRINTF_BUFFLEN];

    int intArg;
    short shortArg;
    long longArg;
    unsigned unsignedArg;
    unsigned long uLongArg;
    unsigned short uShortArg;
    char *charPtrArg = NULL;
    void *voidPtrArg;
    int *intPtrArg;
    long *longPtrArg;
    short *shortPtrArg;
    double doubleArg = 0.0;
    LONG_DOUBLE lDoubleArg = 0.0L;

    fmtBuff[0] = '%';
    f = (char *) format;
    fStop = f + strlen(f);
    while (f != fStop) {
        percentPtr = (char *)memchr(f, '%', fStop - f);
        if(percentPtr == NULL) percentPtr = fStop;
        if(percentPtr != f) {
            if(FCGX_PutStr(f, percentPtr - f, stream) < 0)
                goto ErrorReturn;
            streamCount += percentPtr - f;
            f = percentPtr;
            if(f == fStop) break;
	}
        fastPath = TRUE;
        /*
         * The following loop always executes either once or twice.
         */
        for (;;) {
            if(fastPath) {
                /*
                 * Fast path: Scan optimistically, hoping that no flags,
                 * minimum field width, or precision are specified.
                 * Use the preallocated buffer, which is large enough
                 * for all fast path cases.  If the conversion specifier
                 * is really more complex, run the loop a second time
                 * using the slow path.
                 * Note that fast path execution of %s bypasses the buffer
                 * and %f is not attempted on the fast path due to
                 * its large buffering requirements.
                 */
                op = *(percentPtr + 1);
                switch(op) {
	            case 'l':
	            case 'L':
                    case 'h':
                        sizeModifier = op;
                        op = *(percentPtr + 2);
                        fmtBuff[1] = (char) sizeModifier;
                        fmtBuff[2] = (char) op;
                        fmtBuff[3] = '\0';
                        specifierLength = 3;
                        break;
	            default:
                        sizeModifier = ' ';
                        fmtBuff[1] = (char) op;
                        fmtBuff[2] = '\0';
                        specifierLength = 2;
                        break;
	        }
                buffPtr = buff;
                buffLen = PRINTF_BUFFLEN;
	    } else {
                /*
                 * Slow path: Scan the conversion specifier and construct
                 * a new format string, compute an upper bound on the
                 * amount of buffering that sprintf will require,
                 * and allocate a larger buffer if necessary.
                 */
                p = percentPtr + 1;
                fmtBuffPtr = &fmtBuff[1];
                /*
                 * Scan flags
                 */
                n = strspn(p, "-0+ #");
                if(n > 5)
                    goto ErrorReturn;
                CopyAndAdvance(&fmtBuffPtr, &p, n);
                /*
                 * Scan minimum field width
                 */
                n = strspn(p, "0123456789");
                if(n == 0) {
                    if(*p == '*') {
                        minWidth = va_arg(arg, int);
                        if(abs(minWidth) > 999999)
                            goto ErrorReturn;
			/*
			 * The following use of strlen rather than the
			 * value returned from sprintf is because SUNOS4
			 * returns a char * instead of an int count.
			 */
			sprintf(fmtBuffPtr, "%d", minWidth);
                        fmtBuffPtr += strlen(fmtBuffPtr);
                        p++;
	            } else {
                        minWidth = 0;
	            }
	        } else if(n <= 6) {
                    minWidth = strtol(p, NULL, 10);
                    CopyAndAdvance(&fmtBuffPtr, &p, n);
                } else {
                    goto ErrorReturn;
                }
                /*
                 * Scan precision
                 */
	        if(*p == '.') {
                    CopyAndAdvance(&fmtBuffPtr, &p, 1);
                    n = strspn(p, "0123456789");
                    if(n == 0) {
                        if(*p == '*') {
                            precision = va_arg(arg, int);
                            if(precision < 0) precision = 0;
                            if(precision > 999999)
                                goto ErrorReturn;
			/*
			 * The following use of strlen rather than the
			 * value returned from sprintf is because SUNOS4
			 * returns a char * instead of an int count.
			 */
			    sprintf(fmtBuffPtr, "%d", precision);
			    fmtBuffPtr += strlen(fmtBuffPtr);
                            p++;
	                } else {
                            precision = 0;
	                }
	            } else if(n <= 6) {
                        precision = strtol(p, NULL, 10);
                        CopyAndAdvance(&fmtBuffPtr, &p, n);
                    } else {
                        goto ErrorReturn;
                    }
                } else {
                    precision = -1;
                }
                /*
                 * Scan size modifier and conversion operation
                 */
                switch(*p) {
	            case 'l':
                    case 'L':
                    case 'h':
                        sizeModifier = *p;
                        CopyAndAdvance(&fmtBuffPtr, &p, 1);
                        break;
	            default:
                        sizeModifier = ' ';
                        break;
                }
                op = *p;
                CopyAndAdvance(&fmtBuffPtr, &p, 1);
                ASSERT(fmtBuffPtr - fmtBuff < FMT_BUFFLEN);
                *fmtBuffPtr = '\0';
                specifierLength = p - percentPtr;
                /*
                 * Bound the required buffer size.  For s and f
                 * conversions this requires examining the argument.
                 */
                switch(op) {
	            case 'd':
                    case 'i':
                    case 'u':
                    case 'o':
                    case 'x':
                    case 'X':
                    case 'c':
                    case 'p':
                        buffReqd = max(precision, 46);
                        break;
	            case 's':
                        charPtrArg = va_arg(arg, char *);
			if (!charPtrArg) charPtrArg = "(null)";
                        if(precision == -1) {
			    buffReqd = strlen(charPtrArg);
		        } else {
			    p = (char *)memchr(charPtrArg, '\0', precision);
                            buffReqd =
			      (p == NULL) ? precision : p - charPtrArg;
			}
                        break;
	            case 'f':
                        switch(sizeModifier) {
                            case ' ':
                                doubleArg = va_arg(arg, double);
				                frexp(doubleArg, &exp);
                                break;
                            case 'L':
                                lDoubleArg = va_arg(arg, LONG_DOUBLE);
                                /* XXX Need to check for the presence of 
                                 * frexpl() and use it if available */
				                frexp((double) lDoubleArg, &exp);
                                break;
                            default:
                                goto ErrorReturn;
                        }
                        if(precision == -1) precision = 6;
                        buffReqd = precision + 3 + ((exp > 0) ? exp/3 : 0);
                        break;
	            case 'e':
	            case 'E':
	            case 'g':
	            case 'G':
                        if(precision == -1) precision = 6;
                        buffReqd = precision + 8;
                        break;
	            case 'n':
	            case '%':
	            default:
                        goto ErrorReturn;
                        break;
                }
                buffReqd = max(buffReqd + 10, minWidth);
                /*
                 * Allocate the buffer
                 */
	        if(buffReqd <= PRINTF_BUFFLEN) {
                    buffPtr = buff;
		    buffLen = PRINTF_BUFFLEN;
	        } else {
                    if(auxBuffPtr == NULL || buffReqd > auxBuffLen) {
		        if(auxBuffPtr != NULL) free(auxBuffPtr);
                        auxBuffPtr = (char *)Malloc(buffReqd);
                        auxBuffLen = buffReqd;
                        if(auxBuffPtr == NULL)
                            goto ErrorReturn;
		    }
                    buffPtr = auxBuffPtr;
		    buffLen = auxBuffLen;
		}
	    }
            /*
             * This giant switch statement requires the following variables
             * to be set up: op, sizeModifier, arg, buffPtr, fmtBuff.
             * When fastPath == FALSE and op == 's' or 'f', the argument
             * has been read into charPtrArg, doubleArg, or lDoubleArg.
             * The statement produces the boolean performedOp, TRUE iff
             * the op/sizeModifier were executed and argument consumed;
             * if performedOp, the characters written into buffPtr[]
             * and the character count buffCount (== EOF meaning error).
             *
             * The switch cases are arranged in the same order as in the
             * description of fprintf in section 15.11 of Harbison and Steele.
             */
            performedOp = TRUE;
            switch(op) {
	        case 'd':
	        case 'i':
                    switch(sizeModifier) {
                        case ' ':
                            intArg = va_arg(arg, int);
			    sprintf(buffPtr, fmtBuff, intArg);
                            buffCount = strlen(buffPtr);
                            break;
	                case 'l':
                            longArg = va_arg(arg, long);
                            sprintf(buffPtr, fmtBuff, longArg);
                            buffCount = strlen(buffPtr);
                            break;
	                case 'h':
                            shortArg = (short) va_arg(arg, int);
                            sprintf(buffPtr, fmtBuff, shortArg);
                            buffCount = strlen(buffPtr);
                            break;
	                default:
                            goto ErrorReturn;
	            }
                    break;
	        case 'u':
                case 'o':
                case 'x':
                case 'X':
                    switch(sizeModifier) {
                        case ' ':
                            unsignedArg = va_arg(arg, unsigned);
			    sprintf(buffPtr, fmtBuff, unsignedArg);
                            buffCount = strlen(buffPtr);
                            break;
	                case 'l':
                            uLongArg = va_arg(arg, unsigned long);
			    sprintf(buffPtr, fmtBuff, uLongArg);
                            buffCount = strlen(buffPtr);
                            break;
                        case 'h':
                            uShortArg = (unsigned short) va_arg(arg, int);
                            sprintf(buffPtr, fmtBuff, uShortArg);
                            buffCount = strlen(buffPtr);
                            break;
                        default:
                            goto ErrorReturn;
                    }
                    break;
                case 'c':
                    switch(sizeModifier) {
                        case ' ':
                            intArg = va_arg(arg, int);
			    sprintf(buffPtr, fmtBuff, intArg);
                            buffCount = strlen(buffPtr);
                            break;
	                case 'l':
                            /*
                             * XXX: Allowed by ISO C Amendment 1, but
                             * many platforms don't yet support wint_t
                             */
                            goto ErrorReturn;
                    default:
                            goto ErrorReturn;
                    }
                    break;
	        case 's':
                    switch(sizeModifier) {
                        case ' ':
		            if(fastPath) {
			        buffPtr = va_arg(arg, char *);
                                buffCount = strlen(buffPtr);
                                buffLen = buffCount + 1;
			    } else {
				sprintf(buffPtr, fmtBuff, charPtrArg);
	                        buffCount = strlen(buffPtr);
			    }
			    break;
	                case 'l':
                            /*
                             * XXX: Don't know how to convert a sequence
                             * of wide characters into a byte stream, or
                             * even how to predict the buffering required.
                             */
                            goto ErrorReturn;
                        default:
                            goto ErrorReturn;
                    }
                    break;
                case 'p':
                    if(sizeModifier != ' ')
                        goto ErrorReturn;
                    voidPtrArg = va_arg(arg, void *);
		    sprintf(buffPtr, fmtBuff, voidPtrArg);
                    buffCount = strlen(buffPtr);
                    break;
                case 'n':
                    switch(sizeModifier) {
                        case ' ':
                            intPtrArg = va_arg(arg, int *);
                            *intPtrArg = streamCount;
                            break;
                        case 'l':
                            longPtrArg = va_arg(arg, long *);
                            *longPtrArg = streamCount;
                            break;
                        case 'h':
                            shortPtrArg = (short *) va_arg(arg, short *);
                            *shortPtrArg = (short) streamCount;
                            break;
	                default:
                            goto ErrorReturn;
	            }
                    buffCount = 0;
                    break;
                case 'f':
		    if(fastPath) {
		        performedOp = FALSE;
                        break;
		    }
                    switch(sizeModifier) {
                        case ' ':
			    sprintf(buffPtr, fmtBuff, doubleArg);
                            buffCount = strlen(buffPtr);
                            break;
                        case 'L':
			    sprintf(buffPtr, fmtBuff, lDoubleArg);
                            buffCount = strlen(buffPtr);
                            break;
                        default:
                            goto ErrorReturn;
                    }
                    break;
                case 'e':
                case 'E':
                case 'g':
                case 'G':
                    switch(sizeModifier) {
                        case ' ':
                            doubleArg = va_arg(arg, double);
			    sprintf(buffPtr, fmtBuff, doubleArg);
                            buffCount = strlen(buffPtr);
                            break;
                        case 'L':
                            lDoubleArg = va_arg(arg, LONG_DOUBLE);
			    sprintf(buffPtr, fmtBuff, lDoubleArg);
                            buffCount = strlen(buffPtr);
                            break;
                        default:
                            goto ErrorReturn;
                    }
                    break;
                case '%':
                    if(sizeModifier != ' ')
                        goto ErrorReturn;
                    buff[0] = '%';
                    buffCount = 1;
                    break;
                case '\0':
                    goto ErrorReturn;
                default:
                    performedOp = FALSE;
                    break;
            } /* switch(op) */
            if(performedOp) break;
            if(!fastPath)
                goto ErrorReturn;
            fastPath = FALSE;
        } /* for (;;) */
        ASSERT(buffCount < buffLen);
        if(buffCount > 0) {
            if(FCGX_PutStr(buffPtr, buffCount, stream) < 0)
                goto ErrorReturn;
            streamCount += buffCount;
        } else if(buffCount < 0) {
            goto ErrorReturn;
	}
        f += specifierLength;
    } /* while(f != fStop) */
    goto NormalReturn;
  ErrorReturn:
    streamCount = -1;
  NormalReturn:
    if(auxBuffPtr != NULL) free(auxBuffPtr);
    return streamCount;
}

/*
 * Copy n characters from *srcPtr to *destPtr, then increment
 * both *srcPtr and *destPtr by n.
 */
static void CopyAndAdvance(char **destPtr, char **srcPtr, int n)
{
    char *dest = *destPtr;
    char *src = *srcPtr;
    int i;
    for (i = 0; i < n; i++)
        *dest++ = *src++;
    *destPtr = dest;
    *srcPtr = src;
}

/*
 *----------------------------------------------------------------------
 *
 * FCGX_FFlush --
 *
 *      Flushes any buffered output.
 *
 *      Server-push is a legitimate application of FCGX_FFlush.
 *      Otherwise, FCGX_FFlush is not very useful, since FCGX_Accept
 *      does it implicitly.  FCGX_FFlush may reduce performance
 *      by increasing the total number of operating system calls
 *      the application makes.
 *
 * Results:
 *      EOF (-1) if an error occurred.
 *
 *----------------------------------------------------------------------
 */
int FCGX_FFlush(FCGX_Stream *stream)
{
    if(!stream || stream->isClosed || stream->isReader)
        return 0;
    stream->emptyBuffProc(stream, FALSE);
    return (stream->isClosed) ? -1 : 0;
}

/*
 *----------------------------------------------------------------------
 *
 * FCGX_FClose --
 *
 *      Performs FCGX_FFlush and closes the stream.
 *
 *      This is not a very useful operation, since FCGX_Accept
 *      does it implicitly.  Closing the out stream before the
 *      err stream results in an extra write if there's nothing
 *      in the err stream, and therefore reduces performance.
 *
 * Results:
 *      EOF (-1) if an error occurred.
 *
 *----------------------------------------------------------------------
 */
int FCGX_FClose(FCGX_Stream *stream)
{
    if (stream == NULL) return 0;

    if(!stream->wasFCloseCalled) {
        if(!stream->isReader) {
            stream->emptyBuffProc(stream, TRUE);
        }
        stream->wasFCloseCalled = TRUE;
        stream->isClosed = TRUE;
        if(stream->isReader) {
            stream->wrNext = stream->stop = stream->rdNext;
        } else {
            stream->rdNext = stream->stop = stream->wrNext;
        }
    }
    return (stream->FCGI_errno == 0) ? 0 : EOF;
}

/*
 *----------------------------------------------------------------------
 *
 * SetError --
 *
 *      An error has occurred; save the error code in the stream
 *      for diagnostic purposes and set the stream state so that
 *      reads return EOF and writes have no effect.
 *
 *----------------------------------------------------------------------
 */
static void SetError(FCGX_Stream *stream, int FCGI_errno)
{
    /*
     * Preserve only the first error.
     */
    if(stream->FCGI_errno == 0) {
        stream->FCGI_errno = FCGI_errno;
        stream->isClosed = TRUE;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * FCGX_GetError --
 *
 *      Return the stream error code.  0 means no error, > 0
 *      is an errno(2) error, < 0 is an FCGX_errno error.
 *
 *----------------------------------------------------------------------
 */
int FCGX_GetError(FCGX_Stream *stream) {
    return stream->FCGI_errno;
}

/*
 *----------------------------------------------------------------------
 *
 * FCGX_ClearError --
 *
 *      Clear the stream error code and end-of-file indication.
 *
 *----------------------------------------------------------------------
 */
void FCGX_ClearError(FCGX_Stream *stream) {
    stream->FCGI_errno = 0;
    /*
     * stream->isClosed = FALSE;
     * XXX: should clear isClosed but work is needed to make it safe
     * to do so.  For example, if an application calls FClose, gets
     * an I/O error on the write, calls ClearError and retries
     * the FClose, FClose (really EmptyBuffProc) will write a second
     * EOF record.  If an application calls PutChar instead of FClose
     * after the ClearError, the application will write more data.
     * The stream's state must discriminate between various states
     * of the stream that are now all lumped under isClosed.
     */
}

/*
 *======================================================================
 * Parameters
 *======================================================================
 */

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
 *----------------------------------------------------------------------
 *
 * NewParams --
 *
 *	Creates a new Params structure.
 *
 * Results:
 *      Pointer to the new structure.
 *
 *----------------------------------------------------------------------
 */
static ParamsPtr NewParams(int length)
{
    ParamsPtr result;
    result = (Params *)Malloc(sizeof(Params));
    result->vec = (char **)Malloc(length * sizeof(char *));
    result->length = length;
    result->cur = result->vec;
    *result->cur = NULL;
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * FreeParams --
 *
 *	Frees a Params structure and all the parameters it contains.
 *
 * Side effects:
 *      env becomes invalid.
 *
 *----------------------------------------------------------------------
 */
static void FreeParams(ParamsPtr *paramsPtrPtr)
{
    ParamsPtr paramsPtr = *paramsPtrPtr;
    char **p;
    if(paramsPtr == NULL) {
        return;
    }
    for (p = paramsPtr->vec; p < paramsPtr->cur; p++) {
        free(*p);
    }
    free(paramsPtr->vec);
    free(paramsPtr);
    *paramsPtrPtr = NULL;
}

/*
 *----------------------------------------------------------------------
 *
 * PutParam --
 *
 *	Add a name/value pair to a Params structure.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      Parameters structure updated.
 *
 *----------------------------------------------------------------------
 */
static void PutParam(ParamsPtr paramsPtr, char *nameValue)
{
    int size;

    *paramsPtr->cur++ = nameValue;
    size = paramsPtr->cur - paramsPtr->vec;
    if(size >= paramsPtr->length) {
	paramsPtr->length *= 2;
	paramsPtr->vec = (FCGX_ParamArray)realloc(paramsPtr->vec, paramsPtr->length * sizeof(char *));
	paramsPtr->cur = paramsPtr->vec + size;
    }
    *paramsPtr->cur = NULL;
}

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
char *FCGX_GetParam(const char *name, FCGX_ParamArray envp)
{
    int len;
    char **p;

	if (name == NULL || envp == NULL) return NULL;

    len = strlen(name);

    for (p = envp; *p; ++p) {
        if((strncmp(name, *p, len) == 0) && ((*p)[len] == '=')) {
            return *p+len+1;
        }
    }
    return NULL;
}

/*
 *----------------------------------------------------------------------
 *
 * Start of FastCGI-specific code
 *
 *----------------------------------------------------------------------
 */

/*
 *----------------------------------------------------------------------
 *
 * ReadParams --
 *
 *      Reads FastCGI name-value pairs from stream until EOF.  Converts
 *      each pair to name=value format and adds it to Params structure.
 *
 *----------------------------------------------------------------------
 */
static int ReadParams(Params *paramsPtr, FCGX_Stream *stream)
{
    int nameLen, valueLen;
    unsigned char lenBuff[3];
    char *nameValue;

    while((nameLen = FCGX_GetChar(stream)) != EOF) {
        /*
         * Read name length (one or four bytes) and value length
         * (one or four bytes) from stream.
         */
        if((nameLen & 0x80) != 0) {
            if(FCGX_GetStr((char *) &lenBuff[0], 3, stream) != 3) {
                SetError(stream, FCGX_PARAMS_ERROR);
                return -1;
	    }
            nameLen = ((nameLen & 0x7f) << 24) + (lenBuff[0] << 16)
                    + (lenBuff[1] << 8) + lenBuff[2];
        }
        if((valueLen = FCGX_GetChar(stream)) == EOF) {
            SetError(stream, FCGX_PARAMS_ERROR);
            return -1;
	}
        if((valueLen & 0x80) != 0) {
            if(FCGX_GetStr((char *) &lenBuff[0], 3, stream) != 3) {
                SetError(stream, FCGX_PARAMS_ERROR);
                return -1;
	    }
            valueLen = ((valueLen & 0x7f) << 24) + (lenBuff[0] << 16)
                    + (lenBuff[1] << 8) + lenBuff[2];
        }
        /*
         * nameLen and valueLen are now valid; read the name and value
         * from stream and construct a standard environment entry.
         */
        nameValue = (char *)Malloc(nameLen + valueLen + 2);
        if(FCGX_GetStr(nameValue, nameLen, stream) != nameLen) {
            SetError(stream, FCGX_PARAMS_ERROR);
            free(nameValue);
            return -1;
	}
        *(nameValue + nameLen) = '=';
        if(FCGX_GetStr(nameValue + nameLen + 1, valueLen, stream)
                != valueLen) {
            SetError(stream, FCGX_PARAMS_ERROR);
            free(nameValue);
            return -1;
	}
        *(nameValue + nameLen + valueLen + 1) = '\0';
        PutParam(paramsPtr, nameValue);
    }
    return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * MakeHeader --
 *
 *      Constructs an FCGI_Header struct.
 *
 *----------------------------------------------------------------------
 */
static FCGI_Header MakeHeader(
        int type,
        int requestId,
        int contentLength,
        int paddingLength)
{
    FCGI_Header header;
    ASSERT(contentLength >= 0 && contentLength <= FCGI_MAX_LENGTH);
    ASSERT(paddingLength >= 0 && paddingLength <= 0xff);
    header.version = FCGI_VERSION_1;
    header.type             = (unsigned char) type;
    header.requestIdB1      = (unsigned char) ((requestId     >> 8) & 0xff);
    header.requestIdB0      = (unsigned char) ((requestId         ) & 0xff);
    header.contentLengthB1  = (unsigned char) ((contentLength >> 8) & 0xff);
    header.contentLengthB0  = (unsigned char) ((contentLength     ) & 0xff);
    header.paddingLength    = (unsigned char) paddingLength;
    header.reserved         =  0;
    return header;
}

/*
 *----------------------------------------------------------------------
 *
 * MakeEndRequestBody --
 *
 *      Constructs an FCGI_EndRequestBody struct.
 *
 *----------------------------------------------------------------------
 */
static FCGI_EndRequestBody MakeEndRequestBody(
        int appStatus,
        int protocolStatus)
{
    FCGI_EndRequestBody body;
    body.appStatusB3    = (unsigned char) ((appStatus >> 24) & 0xff);
    body.appStatusB2    = (unsigned char) ((appStatus >> 16) & 0xff);
    body.appStatusB1    = (unsigned char) ((appStatus >>  8) & 0xff);
    body.appStatusB0    = (unsigned char) ((appStatus      ) & 0xff);
    body.protocolStatus = (unsigned char) protocolStatus;
    memset(body.reserved, 0, sizeof(body.reserved));
    return body;
}

/*
 *----------------------------------------------------------------------
 *
 * MakeUnknownTypeBody --
 *
 *      Constructs an FCGI_MakeUnknownTypeBody struct.
 *
 *----------------------------------------------------------------------
 */
static FCGI_UnknownTypeBody MakeUnknownTypeBody(
        int type)
{
    FCGI_UnknownTypeBody body;
    body.type = (unsigned char) type;
    memset(body.reserved, 0, sizeof(body.reserved));
    return body;
}

/*
 *----------------------------------------------------------------------
 *
 * AlignInt8 --
 *
 *      Returns the smallest integer greater than or equal to n
 *      that's a multiple of 8.
 *
 *----------------------------------------------------------------------
 */
static int AlignInt8(unsigned n) {
    return (n + 7) & (UINT_MAX - 7);
}

/*
 *----------------------------------------------------------------------
 *
 * AlignPtr8 --
 *
 *      Returns the smallest pointer greater than or equal to p
 *      that's a multiple of 8.
 *
 *----------------------------------------------------------------------
 */
static unsigned char *AlignPtr8(unsigned char *p) {
    unsigned long u = (unsigned long) p;
    u = ((u + 7) & (ULONG_MAX - 7)) - u;
    return p + u;
}


/*
 * State associated with a stream
 */
typedef struct FCGX_Stream_Data {
    unsigned char *buff;      /* buffer after alignment */
    int bufflen;              /* number of bytes buff can store */
    unsigned char *mBuff;     /* buffer as returned by Malloc */
    unsigned char *buffStop;  /* reader: last valid byte + 1 of entire buffer.
                               * stop generally differs from buffStop for
                               * readers because of record structure.
                               * writer: buff + bufflen */
    int type;                 /* reader: FCGI_PARAMS or FCGI_STDIN
                               * writer: FCGI_STDOUT or FCGI_STDERR */
    int eorStop;              /* reader: stop stream at end-of-record */
    int skip;                 /* reader: don't deliver content bytes */
    int contentLen;           /* reader: bytes of unread content */
    int paddingLen;           /* reader: bytes of unread padding */
    int isAnythingWritten;    /* writer: data has been written to ipcFd */
    int rawWrite;             /* writer: write data without stream headers */
    FCGX_Request *reqDataPtr; /* request data not specific to one stream */
} FCGX_Stream_Data;

/*
 *----------------------------------------------------------------------
 *
 * WriteCloseRecords --
 *
 *      Writes an EOF record for the stream content if necessary.
 *      If this is the last writer to close, writes an FCGI_END_REQUEST
 *      record.
 *
 *----------------------------------------------------------------------
 */
static void WriteCloseRecords(struct FCGX_Stream *stream)
{
    FCGX_Stream_Data *data = (FCGX_Stream_Data *)stream->data;
    /*
     * Enter rawWrite mode so final records won't be encapsulated as
     * stream data.
     */
    data->rawWrite = TRUE;
    /*
     * Generate EOF for stream content if needed.
     */
    if(!(data->type == FCGI_STDERR
            && stream->wrNext == data->buff
            && !data->isAnythingWritten)) {
        FCGI_Header header;
        header = MakeHeader(data->type, data->reqDataPtr->requestId, 0, 0);
        FCGX_PutStr((char *) &header, sizeof(header), stream);
    };
    /*
     * Generate FCGI_END_REQUEST record if needed.
     */
    if(data->reqDataPtr->nWriters == 1) {
        FCGI_EndRequestRecord endRequestRecord;
        endRequestRecord.header = MakeHeader(FCGI_END_REQUEST,
                data->reqDataPtr->requestId,
                sizeof(endRequestRecord.body), 0);
        endRequestRecord.body = MakeEndRequestBody(
                data->reqDataPtr->appStatus, FCGI_REQUEST_COMPLETE);
        FCGX_PutStr((char *) &endRequestRecord,
                sizeof(endRequestRecord), stream);
    }
    data->reqDataPtr->nWriters--;
}



static int write_it_all(int fd, char *buf, int len)
{
    int wrote;

    while (len) {
        wrote = OS_Write(fd, buf, len);
        if (wrote < 0)
            return wrote;
        len -= wrote;
        buf += wrote;
    }
    return len;
}

/*
 *----------------------------------------------------------------------
 *
 * EmptyBuffProc --
 *
 *      Encapsulates any buffered stream content in a FastCGI
 *      record.  Writes the data, making the buffer empty.
 *
 *----------------------------------------------------------------------
 */
static void EmptyBuffProc(struct FCGX_Stream *stream, int doClose)
{
    FCGX_Stream_Data *data = (FCGX_Stream_Data *)stream->data;
    int cLen, eLen;
    /*
     * If the buffer contains stream data, fill in the header.
     * Pad the record to a multiple of 8 bytes in length.  Padding
     * can't overflow the buffer because the buffer is a multiple
     * of 8 bytes in length.  If the buffer contains no stream
     * data, reclaim the space reserved for the header.
     */
    if(!data->rawWrite) {
        cLen = stream->wrNext - data->buff - sizeof(FCGI_Header);
        if(cLen > 0) {
            eLen = AlignInt8(cLen);
            /*
             * Giving the padding a well-defined value keeps Purify happy.
             */
            memset(stream->wrNext, 0, eLen - cLen);
            stream->wrNext += eLen - cLen;
            *((FCGI_Header *) data->buff)
                    = MakeHeader(data->type,
                            data->reqDataPtr->requestId, cLen, eLen - cLen);
        } else {
            stream->wrNext = data->buff;
	}
    }
    if(doClose) {
        WriteCloseRecords(stream);
    };
    if (stream->wrNext != data->buff) {
        data->isAnythingWritten = TRUE;
        if (write_it_all(data->reqDataPtr->ipcFd, (char *)data->buff, stream->wrNext - data->buff) < 0) {
            SetError(stream, OS_Errno);
            return;
        }
        stream->wrNext = data->buff;
    }
    /*
     * The buffer is empty.
     */
    if(!data->rawWrite) {
        stream->wrNext += sizeof(FCGI_Header);
    }
}

/*
 * Return codes for Process* functions
 */
#define STREAM_RECORD 0
#define SKIP          1
#define BEGIN_RECORD  2
#define MGMT_RECORD   3

/*
 *----------------------------------------------------------------------
 *
 * ProcessManagementRecord --
 *
 *      Reads and responds to a management record.  The only type of
 *      management record this library understands is FCGI_GET_VALUES.
 *      The only variables that this library's FCGI_GET_VALUES
 *      understands are FCGI_MAX_CONNS, FCGI_MAX_REQS, and FCGI_MPXS_CONNS.
 *      Ignore other FCGI_GET_VALUES variables; respond to other
 *      management records with a FCGI_UNKNOWN_TYPE record.
 *
 *----------------------------------------------------------------------
 */
static int ProcessManagementRecord(int type, FCGX_Stream *stream)
{
    FCGX_Stream_Data *data = (FCGX_Stream_Data *)stream->data;
    ParamsPtr paramsPtr = NewParams(3);
    char **pPtr;
    char response[64]; /* 64 = 8 + 3*(1+1+14+1)* + padding */
    char *responseP = &response[FCGI_HEADER_LEN];
    char *name, value = '\0';
    int len, paddedLen;
    if(type == FCGI_GET_VALUES) {
        ReadParams(paramsPtr, stream);
        if((FCGX_GetError(stream) != 0) || (data->contentLen != 0)) {
            FreeParams(&paramsPtr);
            return FCGX_PROTOCOL_ERROR;
        }
        for (pPtr = paramsPtr->vec; pPtr < paramsPtr->cur; pPtr++) {
            name = *pPtr;
            *(strchr(name, '=')) = '\0';
            if(strcmp(name, FCGI_MAX_CONNS) == 0) {
                value = '1';
            } else if(strcmp(name, FCGI_MAX_REQS) == 0) {
                value = '1';
            } else if(strcmp(name, FCGI_MPXS_CONNS) == 0) {
                value = '0';
            } else {
                name = NULL;
            }
            if(name != NULL) {
                len = strlen(name);
                sprintf(responseP, "%c%c%s%c", len, 1, name, value);
                responseP += len + 3;
	    }
        }
        len = responseP - &response[FCGI_HEADER_LEN];
        paddedLen = AlignInt8(len);
        *((FCGI_Header *) response)
            = MakeHeader(FCGI_GET_VALUES_RESULT, FCGI_NULL_REQUEST_ID,
                         len, paddedLen - len);
        FreeParams(&paramsPtr);
    } else {
        paddedLen = len = sizeof(FCGI_UnknownTypeBody);
        ((FCGI_UnknownTypeRecord *) response)->header
            = MakeHeader(FCGI_UNKNOWN_TYPE, FCGI_NULL_REQUEST_ID,
                         len, 0);
        ((FCGI_UnknownTypeRecord *) response)->body
            = MakeUnknownTypeBody(type);
    }
    if (write_it_all(data->reqDataPtr->ipcFd, response, FCGI_HEADER_LEN + paddedLen) < 0) {
        SetError(stream, OS_Errno);
        return -1;
    }

    return MGMT_RECORD;
}

/*
 *----------------------------------------------------------------------
 *
 * ProcessBeginRecord --
 *
 *      Reads an FCGI_BEGIN_REQUEST record.
 *
 * Results:
 *      BEGIN_RECORD for normal return.  FCGX_PROTOCOL_ERROR for
 *      protocol error.  SKIP for attempt to multiplex
 *      connection.  -1 for error from write (errno in stream).
 *
 * Side effects:
 *      In case of BEGIN_RECORD return, stores requestId, role,
 *      keepConnection values, and sets isBeginProcessed = TRUE.
 *
 *----------------------------------------------------------------------
 */
static int ProcessBeginRecord(int requestId, FCGX_Stream *stream)
{
    FCGX_Stream_Data *data = (FCGX_Stream_Data *)stream->data;
    FCGI_BeginRequestBody body;
    if(requestId == 0 || data->contentLen != sizeof(body)) {
        return FCGX_PROTOCOL_ERROR;
    }
    if(data->reqDataPtr->isBeginProcessed) {
        /*
         * The Web server is multiplexing the connection.  This library
         * doesn't know how to handle multiplexing, so respond with
         * FCGI_END_REQUEST{protocolStatus = FCGI_CANT_MPX_CONN}
         */
        FCGI_EndRequestRecord endRequestRecord;
        endRequestRecord.header = MakeHeader(FCGI_END_REQUEST,
                requestId, sizeof(endRequestRecord.body), 0);
        endRequestRecord.body
                = MakeEndRequestBody(0, FCGI_CANT_MPX_CONN);
        if (write_it_all(data->reqDataPtr->ipcFd, (char *)&endRequestRecord, sizeof(endRequestRecord)) < 0) {
            SetError(stream, OS_Errno);
            return -1;
        }

        return SKIP;
    }
    /*
     * Accept this new request.  Read the record body.
     */
    data->reqDataPtr->requestId = requestId;
    if(FCGX_GetStr((char *) &body, sizeof(body), stream)
            != sizeof(body)) {
        return FCGX_PROTOCOL_ERROR;
    }
    data->reqDataPtr->keepConnection = (body.flags & FCGI_KEEP_CONN);
    data->reqDataPtr->role = (body.roleB1 << 8) + body.roleB0;
    data->reqDataPtr->isBeginProcessed = TRUE;
    return BEGIN_RECORD;
}

/*
 *----------------------------------------------------------------------
 *
 * ProcessHeader --
 *
 *      Interprets FCGI_Header.  Processes FCGI_BEGIN_REQUEST and
 *      management records here; extracts information from stream
 *      records (FCGI_PARAMS, FCGI_STDIN) into stream.
 *
 * Results:
 *      >= 0 for a normal return, < 0 for error
 *
 * Side effects:
 *      XXX: Many (more than there used to be).
 *      If !stream->isRequestIdSet, ProcessHeader initializes
 *      stream->requestId from header and sets stream->isRequestIdSet
 *      to TRUE.  ProcessHeader also sets stream->contentLen to header's
 *      contentLength, and sets stream->paddingLen to the header's
 *      paddingLength.
 *
 *----------------------------------------------------------------------
 */
static int ProcessHeader(FCGI_Header header, FCGX_Stream *stream)
{
    FCGX_Stream_Data *data = (FCGX_Stream_Data *)stream->data;
    int requestId;
    if(header.version != FCGI_VERSION_1) {
        return FCGX_UNSUPPORTED_VERSION;
    }
    requestId =        (header.requestIdB1 << 8)
                         + header.requestIdB0;
    data->contentLen = (header.contentLengthB1 << 8)
                         + header.contentLengthB0;
    data->paddingLen = header.paddingLength;
    if(header.type == FCGI_BEGIN_REQUEST) {
        return ProcessBeginRecord(requestId, stream);
    }
    if(requestId  == FCGI_NULL_REQUEST_ID) {
        return ProcessManagementRecord(header.type, stream);
    }
    if(requestId != data->reqDataPtr->requestId) {
        return SKIP;
    }
    if(header.type != data->type) {
        return FCGX_PROTOCOL_ERROR;
    }
    return STREAM_RECORD;
}

/*
 *----------------------------------------------------------------------
 *
 * FillBuffProc --
 *
 *      Reads bytes from the ipcFd, supplies bytes to a stream client.
 *
 *----------------------------------------------------------------------
 */
static void FillBuffProc(FCGX_Stream *stream)
{
    FCGX_Stream_Data *data = (FCGX_Stream_Data *)stream->data;
    FCGI_Header header;
    int headerLen = 0;
    int status, count;

    for (;;) {
        /*
         * If data->buff is empty, do a read.
         */
        if(stream->rdNext == data->buffStop) {
            count = OS_Read(data->reqDataPtr->ipcFd, (char *)data->buff,
                            data->bufflen);
            if(count <= 0) {
                SetError(stream, (count == 0 ? FCGX_PROTOCOL_ERROR : OS_Errno));
                return;
            }
            stream->rdNext = data->buff;
            data->buffStop = data->buff + count;
	}
        /*
         * Now data->buff is not empty.  If the current record contains
         * more content bytes, deliver all that are present in data->buff.
         */
        if(data->contentLen > 0) {
            count = min(data->contentLen, data->buffStop - stream->rdNext);
            data->contentLen -= count;
            if(!data->skip) {
                stream->wrNext = stream->stop = stream->rdNext + count;
                return;
	    } else {
                stream->rdNext += count;
                if(data->contentLen > 0) {
                    continue;
	        } else {
                    data->skip = FALSE;
	        }
	    }
	}
        /*
         * If the current record (whose content has been fully consumed by
         * the client) was padded, skip over the padding bytes.
         */
        if(data->paddingLen > 0) {
            count = min(data->paddingLen, data->buffStop - stream->rdNext);
            data->paddingLen -= count;
            stream->rdNext += count;
            if(data->paddingLen > 0) {
                continue;
	    }
	}
        /*
         * All done with the current record, including the padding.
         * If we're in a recursive call from ProcessHeader, deliver EOF.
         */
        if(data->eorStop) {
            stream->stop = stream->rdNext;
            stream->isClosed = TRUE;
            return;
        }
        /*
         * Fill header with bytes from the input buffer.
         */
        count = min((int)sizeof(header) - headerLen,
                        data->buffStop - stream->rdNext);
        memcpy(((char *)(&header)) + headerLen, stream->rdNext, count);
        headerLen += count;
        stream->rdNext += count;
        if(headerLen < sizeof(header)) {
            continue;
	};
        headerLen = 0;
        /*
         * Interpret header.  eorStop prevents ProcessHeader from reading
         * past the end-of-record when using stream to read content.
         */
        data->eorStop = TRUE;
        stream->stop = stream->rdNext;
        status = ProcessHeader(header, stream);
        data->eorStop = FALSE;
        stream->isClosed = FALSE;
        switch(status) {
            case STREAM_RECORD:
                /*
                 * If this stream record header marked the end of stream
                 * data deliver EOF to the stream client, otherwise loop
                 * and deliver data.
                 *
                 * XXX: If this is final stream and
                 * stream->rdNext != data->buffStop, buffered
                 * data is next request (server pipelining)?
                 */
                if(data->contentLen == 0) {
                    stream->wrNext = stream->stop = stream->rdNext;
                    stream->isClosed = TRUE;
                    return;
	        }
                break;
	    case SKIP:
                data->skip = TRUE;
                break;
            case BEGIN_RECORD:
                /*
                 * If this header marked the beginning of a new
                 * request, return role information to caller.
                 */
                return;
                break;
            case MGMT_RECORD:
                break;
            default:
                ASSERT(status < 0);
                SetError(stream, status);
                return;
                break;
	}
    }
}

/*
 *----------------------------------------------------------------------
 *
 * NewStream --
 *
 *      Creates a stream to read or write from an open ipcFd.
 *      The stream performs reads/writes of up to bufflen bytes.
 *
 *----------------------------------------------------------------------
 */
static FCGX_Stream *NewStream(
        FCGX_Request *reqDataPtr, int bufflen, int isReader, int streamType)
{
    /*
     * XXX: It would be a lot cleaner to have a NewStream that only
     * knows about the type FCGX_Stream, with all other
     * necessary data passed in.  It appears that not just
     * data and the two procs are needed for initializing stream,
     * but also data->buff and data->buffStop.  This has implications
     * for procs that want to swap buffers, too.
     */
    FCGX_Stream *stream = (FCGX_Stream *)Malloc(sizeof(FCGX_Stream));
    FCGX_Stream_Data *data = (FCGX_Stream_Data *)Malloc(sizeof(FCGX_Stream_Data));
    data->reqDataPtr = reqDataPtr;
    bufflen = AlignInt8(min(max(bufflen, 32), FCGI_MAX_LENGTH + 1));
    data->bufflen = bufflen;
    data->mBuff = (unsigned char *)Malloc(bufflen);
    data->buff = AlignPtr8(data->mBuff);
    if(data->buff != data->mBuff) {
        data->bufflen -= 8;
    }
    if(isReader) {
        data->buffStop = data->buff;
    } else {
        data->buffStop = data->buff + data->bufflen;
    }
    data->type = streamType;
    data->eorStop = FALSE;
    data->skip = FALSE;
    data->contentLen = 0;
    data->paddingLen = 0;
    data->isAnythingWritten = FALSE;
    data->rawWrite = FALSE;

    stream->data = data;
    stream->isReader = isReader;
    stream->isClosed = FALSE;
    stream->wasFCloseCalled = FALSE;
    stream->FCGI_errno = 0;
    if(isReader) {
        stream->fillBuffProc = FillBuffProc;
        stream->emptyBuffProc = NULL;
        stream->rdNext = data->buff;
        stream->stop = stream->rdNext;
        stream->stopUnget = data->buff;
        stream->wrNext = stream->stop;
    } else {
        stream->fillBuffProc = NULL;
        stream->emptyBuffProc = EmptyBuffProc;
        stream->wrNext = data->buff + sizeof(FCGI_Header);
        stream->stop = data->buffStop;
        stream->stopUnget = NULL;
        stream->rdNext = stream->stop;
    }
    return stream;
}

/*
 *----------------------------------------------------------------------
 *
 * FCGX_FreeStream --
 *
 *      Frees all storage allocated when *streamPtr was created,
 *      and nulls out *streamPtr.
 *
 *----------------------------------------------------------------------
 */
void FCGX_FreeStream(FCGX_Stream **streamPtr)
{
	_FCGX_FreeStream(streamPtr, TRUE);
}

void _FCGX_FreeStream(FCGX_Stream **streamPtr, int freeData)
{
    FCGX_Stream *stream = *streamPtr;
    FCGX_Stream_Data *data;
    if(stream == NULL) {
        return;
    }
    data = (FCGX_Stream_Data *)stream->data;
	if (freeData && data->reqDataPtr) free(data->reqDataPtr);
    data->reqDataPtr = NULL;
    free(data->mBuff);
    free(data);
    free(stream);
    *streamPtr = NULL;
}

/*
 *----------------------------------------------------------------------
 *
 * SetReaderType --
 *
 *      Re-initializes the stream to read data of the specified type.
 *
 *----------------------------------------------------------------------
 */
static FCGX_Stream *SetReaderType(FCGX_Stream *stream, int streamType)
{
    FCGX_Stream_Data *data = (FCGX_Stream_Data *)stream->data;
    ASSERT(stream->isReader);
    data->type = streamType;
    data->eorStop = FALSE;
    data->skip = FALSE;
    data->contentLen = 0;
    data->paddingLen = 0;
    stream->wrNext = stream->stop = stream->rdNext;
    stream->isClosed = FALSE;
    return stream;
}

/*
 *----------------------------------------------------------------------
 *
 * NewReader --
 *
 *      Creates a stream to read streamType records for the given
 *      request.  The stream performs OS reads of up to bufflen bytes.
 *
 *----------------------------------------------------------------------
 */
static FCGX_Stream *NewReader(FCGX_Request *reqDataPtr, int bufflen, int streamType)
{
    return NewStream(reqDataPtr, bufflen, TRUE, streamType);
}

/*
 *----------------------------------------------------------------------
 *
 * NewWriter --
 *
 *      Creates a stream to write streamType FastCGI records, using
 *      the ipcFd and RequestId contained in *reqDataPtr.
 *      The stream performs OS writes of up to bufflen bytes.
 *
 *----------------------------------------------------------------------
 */
static FCGX_Stream *NewWriter(FCGX_Request *reqDataPtr, int bufflen, int streamType)
{
    return NewStream(reqDataPtr, bufflen, FALSE, streamType);
}

/*
 *----------------------------------------------------------------------
 *
 * FCGX_CreateWriter --
 *
 *      Creates a stream to write streamType FastCGI records, using
 *      the given ipcFd and request Id.  This function is provided
 *      for use by cgi-fcgi.  In order to be defensive against misuse,
 *      this function leaks a little storage; cgi-fcgi doesn't care.
 *
 *----------------------------------------------------------------------
 */
FCGX_Stream *FCGX_CreateWriter(
        int ipcFd,
        int requestId,
        int bufflen,
        int streamType)
{
    FCGX_Request *reqDataPtr = (FCGX_Request *)Malloc(sizeof(FCGX_Request));
    reqDataPtr->ipcFd = ipcFd;
    reqDataPtr->requestId = requestId;
    /*
     * Suppress writing an FCGI_END_REQUEST record.
     */
    reqDataPtr->nWriters = 2;
    return NewWriter(reqDataPtr, bufflen, streamType);
}

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
 *      This routine determines if the process is running as a CGI or
 *      FastCGI process.  The distinction is made by determining whether
 *      FCGI_LISTENSOCK_FILENO is a listener ipcFd or the end of a
 *      pipe (ie. standard in).
 *
 * Results:
 *      TRUE if the process is a CGI process, FALSE if FastCGI.
 *
 *----------------------------------------------------------------------
 */
int FCGX_IsCGI(void)
{
    if (isFastCGI != -1) {
        return !isFastCGI;
    }

    if (!libInitialized) {
        int rc = FCGX_Init();
        if (rc) {
            /* exit() isn't great, but hey */
            //exit((rc < 0) ? rc : -rc);
			return 0;
        }
    }

    isFastCGI = OS_IsFcgi(FCGI_LISTENSOCK_FILENO);

    return !isFastCGI;
}

/*
 *----------------------------------------------------------------------
 *
 * FCGX_Finish --
 *
 *      Finishes the current request from the HTTP server.
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

void FCGX_Finish(void)
{
    FCGX_Finish_r(&the_request);
}

/*
 *----------------------------------------------------------------------
 *
 * FCGX_Finish_r --
 *
 *      Finishes the current request from the HTTP server.
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
void FCGX_Finish_r(FCGX_Request *reqDataPtr)
{
    int close;

    if (reqDataPtr == NULL) {
        return;
    }

    close = !reqDataPtr->keepConnection;

    /* This should probably use a 'status' member instead of 'in' */
    if (reqDataPtr->in) {
        close |= FCGX_FClose(reqDataPtr->err);
        close |= FCGX_FClose(reqDataPtr->out);

	close |= FCGX_GetError(reqDataPtr->in);
    }

    FCGX_Free(reqDataPtr, close);
}

void FCGX_Free(FCGX_Request * request, int close)
{
    if (request == NULL) 
        return;

    _FCGX_FreeStream(&request->in, FALSE);
    _FCGX_FreeStream(&request->out, FALSE);
    _FCGX_FreeStream(&request->err, FALSE);
    FreeParams(&request->paramsPtr);

    if (close) {
        OS_IpcClose(request->ipcFd);
        request->ipcFd = -1;
    }
}

int FCGX_OpenSocket(const char *path, int backlog)
{
    int rc = OS_CreateLocalIpcFd(path, backlog, 1);
    if (rc == FCGI_LISTENSOCK_FILENO && isFastCGI == 0) {
        /* XXX probably need to call OS_LibInit() again for Win */
        isFastCGI = 1;
    }
    return rc;
}

int FCGX_InitRequest(FCGX_Request *request, int sock, int flags)
{
    memset(request, 0, sizeof(FCGX_Request));

    /* @@@ Should check that sock is open and listening */
    request->listen_sock = sock;

    /* @@@ Should validate against "known" flags */
    request->flags = flags;

    request->ipcFd = -1;

    return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * FCGX_Init --
 *
 *      Initilize the FCGX library.  This is called by FCGX_Accept()
 *      but must be called by the user when using FCGX_Accept_r().
 *
 * Results:
 *	    0 for successful call.
 *
 *----------------------------------------------------------------------
 */
int FCGX_Init(void)
{
    char *p;

    if (libInitialized) {
        return 0;
    }

    FCGX_InitRequest(&the_request, FCGI_LISTENSOCK_FILENO, 0);

    if (OS_LibInit(NULL) == -1) {
        return OS_Errno ? OS_Errno : -9997;
    }

    p = getenv("FCGI_WEB_SERVER_ADDRS");
    webServerAddressList = p ? StringCopy(p) : NULL;

    libInitialized = 1;
    return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * FCGX_Accept --
 *
 *      Accepts a new request from the HTTP server.
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

int FCGX_Accept(
        FCGX_Stream **in,
        FCGX_Stream **out,
        FCGX_Stream **err,
        FCGX_ParamArray *envp)
{
    int rc;

    if (! libInitialized) {
        rc = FCGX_Init();
        if (rc) {
            return rc;
        }
    }

    rc = FCGX_Accept_r(&the_request);

    *in = the_request.in;
    *out = the_request.out;
    *err = the_request.err;
    *envp = the_request.envp;

    return rc;
}

/*
 *----------------------------------------------------------------------
 *
 * FCGX_Accept_r --
 *
 *      Accepts a new request from the HTTP server.
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
int FCGX_Accept_r(FCGX_Request *reqDataPtr)
{
    if (!libInitialized) {
        return -9998;
    }

    /* Finish the current request, if any. */
    FCGX_Finish_r(reqDataPtr);

    for (;;) {
        /*
         * If a connection isn't open, accept a new connection (blocking).
         * If an OS error occurs in accepting the connection,
         * return -1 to the caller, who should exit.
         */
        if (reqDataPtr->ipcFd < 0) {
            int fail_on_intr = reqDataPtr->flags & FCGI_FAIL_ACCEPT_ON_INTR;

            reqDataPtr->ipcFd = OS_Accept(reqDataPtr->listen_sock, fail_on_intr, webServerAddressList);
            if (reqDataPtr->ipcFd < 0) {
                return (errno > 0) ? (0 - errno) : -9999;
            }
        }
        /*
         * A connection is open.  Read from the connection in order to
         * get the request's role and environment.  If protocol or other
         * errors occur, close the connection and try again.
         */
        reqDataPtr->isBeginProcessed = FALSE;
        reqDataPtr->in = NewReader(reqDataPtr, 8192, 0);
        FillBuffProc(reqDataPtr->in);
        if(!reqDataPtr->isBeginProcessed) {
            goto TryAgain;
        }
        {
            char *roleStr;
            switch(reqDataPtr->role) {
                case FCGI_RESPONDER:
                    roleStr = "FCGI_ROLE=RESPONDER";
                    break;
                case FCGI_AUTHORIZER:
                    roleStr = "FCGI_ROLE=AUTHORIZER";
                    break;
                case FCGI_FILTER:
                    roleStr = "FCGI_ROLE=FILTER";
                    break;
                default:
                    goto TryAgain;
            }
            reqDataPtr->paramsPtr = NewParams(30);
            PutParam(reqDataPtr->paramsPtr, StringCopy(roleStr));
        }
        SetReaderType(reqDataPtr->in, FCGI_PARAMS);
        if(ReadParams(reqDataPtr->paramsPtr, reqDataPtr->in) >= 0) {
            /*
             * Finished reading the environment.  No errors occurred, so
             * leave the connection-retry loop.
             */
            break;
        }

        /*
         * Close the connection and try again.
         */
TryAgain:
        FCGX_Free(reqDataPtr, 1);

    } /* for (;;) */
    /*
     * Build the remaining data structures representing the new
     * request and return successfully to the caller.
     */
    SetReaderType(reqDataPtr->in, FCGI_STDIN);
    reqDataPtr->out = NewWriter(reqDataPtr, 8192, FCGI_STDOUT);
    reqDataPtr->err = NewWriter(reqDataPtr, 512, FCGI_STDERR);
    reqDataPtr->nWriters = 2;
    reqDataPtr->envp = reqDataPtr->paramsPtr->vec;
    return 0;
}

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

int FCGX_StartFilterData(FCGX_Stream *stream)
{
    FCGX_Stream_Data *data = (FCGX_Stream_Data *)stream->data;
    if(data->reqDataPtr->role != FCGI_FILTER
            || !stream->isReader
            || !stream->isClosed
            || data->type != FCGI_STDIN) {
        SetError(stream, FCGX_CALL_SEQ_ERROR);
        return -1;
    }
    SetReaderType(stream, FCGI_DATA);
    return 0;
}

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

void FCGX_SetExitStatus(int status, FCGX_Stream *stream)
{
    FCGX_Stream_Data *data = (FCGX_Stream_Data *)stream->data;
    data->reqDataPtr->appStatus = status;
}

