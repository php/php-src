/*
 * os_unix.c --
 *
 *      Description of file.
 *
 *
 *  Copyright (c) 1995 Open Market, Inc.
 *  All rights reserved.
 *
 *  This file contains proprietary and confidential information and
 *  remains the unpublished property of Open Market, Inc. Use,
 *  disclosure, or reproduction is prohibited except as permitted by
 *  express written license agreement with Open Market, Inc.
 *
 *  Bill Snapper
 *  snapper@openmarket.com
 */

#ifndef lint
static const char rcsid[] = "$Id$";
#endif /* not lint */

#include "fcgi_config.h"

#include <sys/types.h>

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>      /* for fcntl */
#include <math.h>
#include <memory.h>     /* for memchr() */
#include <netinet/tcp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/un.h>
#include <signal.h>

#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h> /* for getpeername */
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "fastcgi.h"
#include "fcgimisc.h"
#include "fcgios.h"

#ifndef INADDR_NONE
#define INADDR_NONE ((unsigned long) -1)
#endif

/*
 * This structure holds an entry for each oustanding async I/O operation.
 */
typedef struct {
    OS_AsyncProc procPtr;	    /* callout completion procedure */
    ClientData clientData;	    /* caller private data */
    int fd;
    int len;
    int offset;
    void *buf;
    int inUse;
} AioInfo;

/*
 * Entries in the async I/O table are allocated 2 per file descriptor.
 *
 * Read Entry Index  = fd * 2
 * Write Entry Index = (fd * 2) + 1
 */
#define AIO_RD_IX(fd) (fd * 2)
#define AIO_WR_IX(fd) ((fd * 2) + 1)

static int asyncIoInUse = FALSE;
static int asyncIoTableSize = 16;
static AioInfo *asyncIoTable = NULL;

static int libInitialized = FALSE;

static fd_set readFdSet;
static fd_set writeFdSet;

static fd_set readFdSetPost;
static int numRdPosted = 0;
static fd_set writeFdSetPost;
static int numWrPosted = 0;
static int volatile maxFd = -1;

static int shutdownPending = FALSE;
static int shutdownNow = FALSE;

void OS_ShutdownPending()
{
    shutdownPending = TRUE;
}

static void OS_Sigusr1Handler(int signo)
{
    OS_ShutdownPending();
}

static void OS_SigpipeHandler(int signo)
{
    ;
}

static void installSignalHandler(int signo, const struct sigaction * act, int force)
{
    struct sigaction sa;

    sigaction(signo, NULL, &sa);

    if (force || sa.sa_handler == SIG_DFL) 
    {
        sigaction(signo, act, NULL);
    }
}

static void OS_InstallSignalHandlers(int force)
{
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sa.sa_handler = OS_SigpipeHandler;
    installSignalHandler(SIGPIPE, &sa, force);

    sa.sa_handler = OS_Sigusr1Handler;
    installSignalHandler(SIGUSR1, &sa, force);
}

/*
 *--------------------------------------------------------------
 *
 * OS_LibInit --
 *
 *      Set up the OS library for use.
 *
 *      NOTE: This function is really only needed for application
 *            asynchronous I/O.  It will most likely change in the
 *            future to setup the multi-threaded environment.
 *
 * Results:
 *	Returns 0 if success, -1 if not.
 *
 * Side effects:
 *	Async I/O table allocated and initialized.
 *
 *--------------------------------------------------------------
 */
int OS_LibInit(int stdioFds[3])
{
    if(libInitialized)
        return 0;

    asyncIoTable = (AioInfo *)malloc(asyncIoTableSize * sizeof(AioInfo));
    if(asyncIoTable == NULL) {
        errno = ENOMEM;
        return -1;
    }
    memset((char *) asyncIoTable, 0,
           asyncIoTableSize * sizeof(AioInfo));

    FD_ZERO(&readFdSet);
    FD_ZERO(&writeFdSet);
    FD_ZERO(&readFdSetPost);
    FD_ZERO(&writeFdSetPost);

    OS_InstallSignalHandlers(FALSE);

    libInitialized = TRUE;

    return 0;
}

/*
 *--------------------------------------------------------------
 *
 * OS_LibShutdown --
 *
 *	Shutdown the OS library.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Memory freed, fds closed.
 *
 *--------------------------------------------------------------
 */
void OS_LibShutdown()
{
    if(!libInitialized)
        return;

    free(asyncIoTable);
    asyncIoTable = NULL;
    libInitialized = FALSE;
    return;
}

/*
 *----------------------------------------------------------------------
 *
 * OS_BuildSockAddrUn --
 *
 *      Using the pathname bindPath, fill in the sockaddr_un structure
 *      *servAddrPtr and the length of this structure *servAddrLen.
 *
 *      The format of the sockaddr_un structure changed incompatibly in
 *      4.3BSD Reno.  Digital UNIX supports both formats, other systems
 *      support one or the other.
 *
 * Results:
 *      0 for normal return, -1 for failure (bindPath too long).
 *
 *----------------------------------------------------------------------
 */

static int OS_BuildSockAddrUn(const char *bindPath,
                              struct sockaddr_un *servAddrPtr,
                              int *servAddrLen)
{
    int bindPathLen = strlen(bindPath);

#ifdef HAVE_SOCKADDR_UN_SUN_LEN /* 4.3BSD Reno and later: BSDI, DEC */
    if(bindPathLen >= sizeof(servAddrPtr->sun_path)) {
        return -1;
    }
#else                           /* 4.3 BSD Tahoe: Solaris, HPUX, DEC, ... */
    if(bindPathLen > sizeof(servAddrPtr->sun_path)) {
        return -1;
    }
#endif
    memset((char *) servAddrPtr, 0, sizeof(*servAddrPtr));
    servAddrPtr->sun_family = AF_UNIX;
    memcpy(servAddrPtr->sun_path, bindPath, bindPathLen);
#ifdef HAVE_SOCKADDR_UN_SUN_LEN /* 4.3BSD Reno and later: BSDI, DEC */
    *servAddrLen = sizeof(servAddrPtr->sun_len)
            + sizeof(servAddrPtr->sun_family)
            + bindPathLen + 1;
    servAddrPtr->sun_len = *servAddrLen;
#else                           /* 4.3 BSD Tahoe: Solaris, HPUX, DEC, ... */
    *servAddrLen = sizeof(servAddrPtr->sun_family) + bindPathLen;
#endif
    return 0;
}
union SockAddrUnion {
    struct  sockaddr_un	unixVariant;
    struct  sockaddr_in	inetVariant;
};

/*
 * OS_CreateLocalIpcFd --
 *
 *   This procedure is responsible for creating the listener socket
 *   on Unix for local process communication.  It will create a
 *   domain socket or a TCP/IP socket bound to "localhost" and return
 *   a file descriptor to it to the caller.
 *   bCreateMutex is ignored for unix
 *
 * Results:
 *      Listener socket created.  This call returns either a valid
 *      file descriptor or -1 on error.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */
int OS_CreateLocalIpcFd(const char *bindPath, int backlog, int bCreateMutex)
{
    int listenSock, servLen;
    union   SockAddrUnion sa;
    int	    tcp = FALSE;
    unsigned long tcp_ia = 0;
    char    *tp;
    short   port = 0;
    char    host[MAXPATHLEN];

    strcpy(host, bindPath);
    if((tp = strchr(host, ':')) != 0) {
	*tp++ = 0;
	if((port = atoi(tp)) == 0) {
	    *--tp = ':';
	 } else {
	    tcp = TRUE;
	 }
    }
    if(tcp) {
      if (!*host || !strcmp(host,"*")) {
	tcp_ia = htonl(INADDR_ANY);
      } else {
	tcp_ia = inet_addr(host);
	if (tcp_ia == INADDR_NONE) {
	  struct hostent * hep;
	  hep = gethostbyname(host);
	  if ((!hep) || (hep->h_addrtype != AF_INET || !hep->h_addr_list[0])) {
	    fprintf(stderr, "Cannot resolve host name %s -- exiting!\n", host);
	    return -1;
	  }
	  if (hep->h_addr_list[1]) {
	    fprintf(stderr, "Host %s has multiple addresses ---\n", host);
	    fprintf(stderr, "you must choose one explicitly!!!\n");
	    return -1;
	  }
	  tcp_ia = ((struct in_addr *) (hep->h_addr))->s_addr;
	}
      }
    }

    if(tcp) {
	listenSock = socket(AF_INET, SOCK_STREAM, 0);
        if(listenSock >= 0) {
            int flag = 1;
            if(setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR,
                          (char *) &flag, sizeof(flag)) < 0) {
                fprintf(stderr, "Can't set SO_REUSEADDR.\n");
	        return -1;
	    }
	}
    } else {
	listenSock = socket(AF_UNIX, SOCK_STREAM, 0);
    }
    if(listenSock < 0) {
        return -1;
    }

    /*
     * Bind the listening socket.
     */
    if(tcp) {
	memset((char *) &sa.inetVariant, 0, sizeof(sa.inetVariant));
	sa.inetVariant.sin_family = AF_INET;
	sa.inetVariant.sin_addr.s_addr = tcp_ia;
	sa.inetVariant.sin_port = htons(port);
	servLen = sizeof(sa.inetVariant);
    } else {
	unlink(bindPath);
	if(OS_BuildSockAddrUn(bindPath, &sa.unixVariant, &servLen)) {
	    fprintf(stderr, "Listening socket's path name is too long.\n");
	    return -1;
	}
    }
    if(bind(listenSock, (struct sockaddr *) &sa.unixVariant, servLen) < 0
       || listen(listenSock, backlog) < 0) {
	perror("bind/listen");
        return -1;
    }

    return listenSock;
}

/*
 *----------------------------------------------------------------------
 *
 * OS_FcgiConnect --
 *
 *	Create the socket and connect to the remote application if
 *      possible.
 *
 *      This was lifted from the cgi-fcgi application and was abstracted
 *      out because Windows NT does not have a domain socket and must
 *      use a named pipe which has a different API altogether.
 *
 * Results:
 *      -1 if fail or a valid file descriptor if connection succeeds.
 *
 * Side effects:
 *      Remote connection established.
 *
 *----------------------------------------------------------------------
 */
int OS_FcgiConnect(char *bindPath)
{
    union   SockAddrUnion sa;
    int servLen, resultSock;
    int connectStatus;
    char    *tp;
    char    host[MAXPATHLEN];
    short   port = 0;
    int	    tcp = FALSE;

    strcpy(host, bindPath);
    if((tp = strchr(host, ':')) != 0) {
	*tp++ = 0;
	if((port = atoi(tp)) == 0) {
	    *--tp = ':';
	 } else {
	    tcp = TRUE;
	 }
    }
    if(tcp == TRUE) {
	struct	hostent	*hp;
	if((hp = gethostbyname((*host ? host : "localhost"))) == NULL) {
	    fprintf(stderr, "Unknown host: %s\n", bindPath);
	    return -1;
	}
	sa.inetVariant.sin_family = AF_INET;
	memcpy(&sa.inetVariant.sin_addr, hp->h_addr, hp->h_length);
	sa.inetVariant.sin_port = htons(port);
	servLen = sizeof(sa.inetVariant);
	resultSock = socket(AF_INET, SOCK_STREAM, 0);
    } else {
	if(OS_BuildSockAddrUn(bindPath, &sa.unixVariant, &servLen)) {
	    fprintf(stderr, "Listening socket's path name is too long.\n");
	    return -1;
	}
	resultSock = socket(AF_UNIX, SOCK_STREAM, 0);
    }

    ASSERT(resultSock >= 0);
    connectStatus = connect(resultSock, (struct sockaddr *) &sa.unixVariant,
                             servLen);
    if(connectStatus >= 0) {
        return resultSock;
    } else {
        /*
         * Most likely (errno == ENOENT || errno == ECONNREFUSED)
         * and no FCGI application server is running.
         */
        close(resultSock);
        return -1;
    }
}

/*
 *--------------------------------------------------------------
 *
 * OS_Read --
 *
 *	Pass through to the unix read function.
 *
 * Results:
 *	Returns number of byes read, 0, or -1 failure: errno
 *      contains actual error.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */
int OS_Read(int fd, char * buf, size_t len)
{
    if (shutdownNow) return -1;
    return(read(fd, buf, len));
}

/*
 *--------------------------------------------------------------
 *
 * OS_Write --
 *
 *	Pass through to unix write function.
 *
 * Results:
 *	Returns number of byes read, 0, or -1 failure: errno
 *      contains actual error.
 *
 * Side effects:
 *	none.
 *
 *--------------------------------------------------------------
 */
int OS_Write(int fd, char * buf, size_t len)
{
    if (shutdownNow) return -1;
    return(write(fd, buf, len));
}

/*
 *----------------------------------------------------------------------
 *
 * OS_SpawnChild --
 *
 *	Spawns a new FastCGI listener process.
 *
 * Results:
 *      0 if success, -1 if error.
 *
 * Side effects:
 *      Child process spawned.
 *
 *----------------------------------------------------------------------
 */
int OS_SpawnChild(char *appPath, int listenFd, PROCESS_INFORMATION *pInfo, char *env)
{
    int forkResult;

    forkResult = fork();
    if(forkResult < 0) {
        return -1;
    }

    if(forkResult == 0) {
        /*
         * Close STDIN unconditionally.  It's used by the parent
         * process for CGI communication.  The FastCGI applciation
         * will be replacing this with the FastCGI listenFd IF
         * STDIN_FILENO is the same as FCGI_LISTENSOCK_FILENO
         * (which it is on Unix).  Regardless, STDIN, STDOUT, and
         * STDERR will be closed as the FastCGI process uses a
         * multiplexed socket in their place.
         */
        close(STDIN_FILENO);

        /*
         * If the listenFd is already the value of FCGI_LISTENSOCK_FILENO
         * we're set.  If not, change it so the child knows where to
         * get the listen socket from.
         */
        if(listenFd != FCGI_LISTENSOCK_FILENO) {
            dup2(listenFd, FCGI_LISTENSOCK_FILENO);
            close(listenFd);
        }

	close(STDOUT_FILENO);
	close(STDERR_FILENO);

        /*
	 * We're a child.  Exec the application.
         *
         * XXX: entire environment passes through
	 */
	execl(appPath, appPath, NULL);
	/*
	 * XXX: Can't do this as we've already closed STDERR!!!
	 *
	 * perror("exec");
	 */
	return -1;
    }
    return 0;
}

/*
 *--------------------------------------------------------------
 *
 * OS_AsyncReadStdin --
 *
 *	This initiates an asynchronous read on the standard
 *	input handle.
 *
 *      The abstraction is necessary because Windows NT does not
 *      have a clean way of "select"ing a file descriptor for
 *      I/O.
 *
 * Results:
 *	-1 if error, 0 otherwise.
 *
 * Side effects:
 *	Asynchronous bit is set in the readfd variable and
 *      request is enqueued.
 *
 *--------------------------------------------------------------
 */
int OS_AsyncReadStdin(void *buf, int len, OS_AsyncProc procPtr,
                      ClientData clientData)
{
    int index = AIO_RD_IX(STDIN_FILENO);

    asyncIoInUse = TRUE;
    ASSERT(asyncIoTable[index].inUse == 0);
    asyncIoTable[index].procPtr = procPtr;
    asyncIoTable[index].clientData = clientData;
    asyncIoTable[index].fd = STDIN_FILENO;
    asyncIoTable[index].len = len;
    asyncIoTable[index].offset = 0;
    asyncIoTable[index].buf = buf;
    asyncIoTable[index].inUse = 1;
    FD_SET(STDIN_FILENO, &readFdSet);
    if(STDIN_FILENO > maxFd)
        maxFd = STDIN_FILENO;
    return 0;
}

static void GrowAsyncTable(void)
{
    int oldTableSize = asyncIoTableSize;

    asyncIoTableSize = asyncIoTableSize * 2;
    asyncIoTable = (AioInfo *)realloc(asyncIoTable, asyncIoTableSize * sizeof(AioInfo));
    if(asyncIoTable == NULL) {
        errno = ENOMEM;
        exit(errno);
    }
    memset((char *) &asyncIoTable[oldTableSize], 0,
           oldTableSize * sizeof(AioInfo));

}

/*
 *--------------------------------------------------------------
 *
 * OS_AsyncRead --
 *
 *	This initiates an asynchronous read on the file
 *	handle which may be a socket or named pipe.
 *
 *	We also must save the ProcPtr and ClientData, so later
 *	when the io completes, we know who to call.
 *
 *	We don't look at any results here (the ReadFile may
 *	return data if it is cached) but do all completion
 *	processing in OS_Select when we get the io completion
 *	port done notifications.  Then we call the callback.
 *
 * Results:
 *	-1 if error, 0 otherwise.
 *
 * Side effects:
 *	Asynchronous I/O operation is queued for completion.
 *
 *--------------------------------------------------------------
 */
int OS_AsyncRead(int fd, int offset, void *buf, int len,
		 OS_AsyncProc procPtr, ClientData clientData)
{
    int index = AIO_RD_IX(fd);

    ASSERT(asyncIoTable != NULL);
    asyncIoInUse = TRUE;

    if(fd > maxFd)
        maxFd = fd;

    if(index >= asyncIoTableSize) {
        GrowAsyncTable();
    }

    ASSERT(asyncIoTable[index].inUse == 0);
    asyncIoTable[index].procPtr = procPtr;
    asyncIoTable[index].clientData = clientData;
    asyncIoTable[index].fd = fd;
    asyncIoTable[index].len = len;
    asyncIoTable[index].offset = offset;
    asyncIoTable[index].buf = buf;
    asyncIoTable[index].inUse = 1;
    FD_SET(fd, &readFdSet);
    return 0;
}

/*
 *--------------------------------------------------------------
 *
 * OS_AsyncWrite --
 *
 *	This initiates an asynchronous write on the "fake" file
 *	descriptor (which may be a file, socket, or named pipe).
 *	We also must save the ProcPtr and ClientData, so later
 *	when the io completes, we know who to call.
 *
 *	We don't look at any results here (the WriteFile generally
 *	completes immediately) but do all completion processing
 *	in OS_DoIo when we get the io completion port done
 *	notifications.  Then we call the callback.
 *
 * Results:
 *	-1 if error, 0 otherwise.
 *
 * Side effects:
 *	Asynchronous I/O operation is queued for completion.
 *
 *--------------------------------------------------------------
 */
int OS_AsyncWrite(int fd, int offset, void *buf, int len,
		  OS_AsyncProc procPtr, ClientData clientData)
{
    int index = AIO_WR_IX(fd);

    asyncIoInUse = TRUE;

    if(fd > maxFd)
        maxFd = fd;

    if(index >= asyncIoTableSize) {
        GrowAsyncTable();
    }

    ASSERT(asyncIoTable[index].inUse == 0);
    asyncIoTable[index].procPtr = procPtr;
    asyncIoTable[index].clientData = clientData;
    asyncIoTable[index].fd = fd;
    asyncIoTable[index].len = len;
    asyncIoTable[index].offset = offset;
    asyncIoTable[index].buf = buf;
    asyncIoTable[index].inUse = 1;
    FD_SET(fd, &writeFdSet);
    return 0;
}

/*
 *--------------------------------------------------------------
 *
 * OS_Close --
 *
 *	Closes the descriptor.  This is a pass through to the
 *      Unix close.
 *
 * Results:
 *	0 for success, -1 on failure
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */
int OS_Close(int fd)
{
    if (fd == -1)
        return 0;

    if (asyncIoInUse) {
        int index = AIO_RD_IX(fd);

        FD_CLR(fd, &readFdSet);
        FD_CLR(fd, &readFdSetPost);
        if (asyncIoTable[index].inUse != 0) {
            asyncIoTable[index].inUse = 0;
        }

        FD_CLR(fd, &writeFdSet);
        FD_CLR(fd, &writeFdSetPost);
        index = AIO_WR_IX(fd);
        if (asyncIoTable[index].inUse != 0) {
            asyncIoTable[index].inUse = 0;
        }

        if (maxFd == fd) {
            maxFd--;
        }
    }
    return close(fd);
}

/*
 *--------------------------------------------------------------
 *
 * OS_CloseRead --
 *
 *	Cancel outstanding asynchronous reads and prevent subsequent
 *      reads from completing.
 *
 * Results:
 *	Socket or file is shutdown. Return values mimic Unix shutdown:
 *		0 success, -1 failure
 *
 *--------------------------------------------------------------
 */
int OS_CloseRead(int fd)
{
    if(asyncIoTable[AIO_RD_IX(fd)].inUse != 0) {
        asyncIoTable[AIO_RD_IX(fd)].inUse = 0;
        FD_CLR(fd, &readFdSet);
    }

    return shutdown(fd, 0);
}

/*
 *--------------------------------------------------------------
 *
 * OS_DoIo --
 *
 *	This function was formerly OS_Select.  It's purpose is
 *      to pull I/O completion events off the queue and dispatch
 *      them to the appropriate place.
 *
 * Results:
 *	Returns 0.
 *
 * Side effects:
 *	Handlers are called.
 *
 *--------------------------------------------------------------
 */
int OS_DoIo(struct timeval *tmo)
{
    int fd, len, selectStatus;
    OS_AsyncProc procPtr;
    ClientData clientData;
    AioInfo *aioPtr;
    fd_set readFdSetCpy;
    fd_set writeFdSetCpy;

    asyncIoInUse = TRUE;
    FD_ZERO(&readFdSetCpy);
    FD_ZERO(&writeFdSetCpy);

    for(fd = 0; fd <= maxFd; fd++) {
        if(FD_ISSET(fd, &readFdSet)) {
            FD_SET(fd, &readFdSetCpy);
        }
        if(FD_ISSET(fd, &writeFdSet)) {
            FD_SET(fd, &writeFdSetCpy);
        }
    }

    /*
     * If there were no completed events from a prior call, see if there's
     * any work to do.
     */
    if(numRdPosted == 0 && numWrPosted == 0) {
        selectStatus = select((maxFd+1), &readFdSetCpy, &writeFdSetCpy,
                              NULL, tmo);
        if(selectStatus < 0) {
            /*exit(errno);*/
			/* not sure what's best to do here */
			return -1;
	}

        for(fd = 0; fd <= maxFd; fd++) {
	    /*
	     * Build up a list of completed events.  We'll work off of
	     * this list as opposed to looping through the read and write
	     * fd sets since they can be affected by a callbacl routine.
	     */
	    if(FD_ISSET(fd, &readFdSetCpy)) {
	        numRdPosted++;
		FD_SET(fd, &readFdSetPost);
		FD_CLR(fd, &readFdSet);
	    }

            if(FD_ISSET(fd, &writeFdSetCpy)) {
	        numWrPosted++;
	        FD_SET(fd, &writeFdSetPost);
		FD_CLR(fd, &writeFdSet);
	    }
        }
    }

    if(numRdPosted == 0 && numWrPosted == 0)
        return 0;

    for(fd = 0; fd <= maxFd; fd++) {
        /*
	 * Do reads and dispatch callback.
	 */
        if(FD_ISSET(fd, &readFdSetPost)
	   && asyncIoTable[AIO_RD_IX(fd)].inUse) {

	    numRdPosted--;
	    FD_CLR(fd, &readFdSetPost);
	    aioPtr = &asyncIoTable[AIO_RD_IX(fd)];

	    len = read(aioPtr->fd, aioPtr->buf, aioPtr->len);

	    procPtr = aioPtr->procPtr;
	    aioPtr->procPtr = NULL;
	    clientData = aioPtr->clientData;
	    aioPtr->inUse = 0;

	    (*procPtr)(clientData, len);
	}

        /*
	 * Do writes and dispatch callback.
	 */
        if(FD_ISSET(fd, &writeFdSetPost) &&
           asyncIoTable[AIO_WR_IX(fd)].inUse) {

	    numWrPosted--;
	    FD_CLR(fd, &writeFdSetPost);
	    aioPtr = &asyncIoTable[AIO_WR_IX(fd)];

	    len = write(aioPtr->fd, aioPtr->buf, aioPtr->len);

	    procPtr = aioPtr->procPtr;
	    aioPtr->procPtr = NULL;
	    clientData = aioPtr->clientData;
	    aioPtr->inUse = 0;
	    (*procPtr)(clientData, len);
	}
    }
    return 0;
}

/* 
 * Not all systems have strdup().  
 * @@@ autoconf should determine whether or not this is needed, but for now..
 */
static char * str_dup(const char * str)
{
    char * sdup = (char *) malloc(strlen(str) + 1);

    if (sdup)
        strcpy(sdup, str);

    return sdup;
}

/*
 *----------------------------------------------------------------------
 *
 * ClientAddrOK --
 *
 *      Checks if a client address is in a list of allowed addresses
 *
 * Results:
 *	TRUE if address list is empty or client address is present
 *      in the list, FALSE otherwise.
 *
 *----------------------------------------------------------------------
 */
static int ClientAddrOK(struct sockaddr_in *saPtr, const char *clientList)
{
    int result = FALSE;
    char *clientListCopy, *cur, *next;

    if (clientList == NULL || *clientList == '\0') {
        return TRUE;
    }

    clientListCopy = str_dup(clientList);

    for (cur = clientListCopy; cur != NULL; cur = next) {
        next = strchr(cur, ',');
        if (next != NULL) {
            *next++ = '\0';
        }
        if (inet_addr(cur) == saPtr->sin_addr.s_addr) {
            result = TRUE;
            break;
        }
    }

    free(clientListCopy);
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * AcquireLock --
 *
 *      On platforms that implement concurrent calls to accept
 *      on a shared listening ipcFd, returns 0.  On other platforms,
 *	acquires an exclusive lock across all processes sharing a
 *      listening ipcFd, blocking until the lock has been acquired.
 *
 * Results:
 *      0 for successful call, -1 in case of system error (fatal).
 *
 * Side effects:
 *      This process now has the exclusive lock.
 *
 *----------------------------------------------------------------------
 */
static int AcquireLock(int sock, int fail_on_intr)
{
#ifdef USE_LOCKING
    do {
        struct flock lock;
        lock.l_type = F_WRLCK;
        lock.l_start = 0;
        lock.l_whence = SEEK_SET;
        lock.l_len = 0;

        if (fcntl(sock, F_SETLKW, &lock) != -1)
            return 0;
    } while (errno == EINTR 
             && ! fail_on_intr 
             && ! shutdownPending);

    return -1;

#else
    return 0;
#endif
}

/*
 *----------------------------------------------------------------------
 *
 * ReleaseLock --
 *
 *      On platforms that implement concurrent calls to accept
 *      on a shared listening ipcFd, does nothing.  On other platforms,
 *	releases an exclusive lock acquired by AcquireLock.
 *
 * Results:
 *      0 for successful call, -1 in case of system error (fatal).
 *
 * Side effects:
 *      This process no longer holds the lock.
 *
 *----------------------------------------------------------------------
 */
static int ReleaseLock(int sock)
{
#ifdef USE_LOCKING
    do {
        struct flock lock;
        lock.l_type = F_UNLCK;
        lock.l_start = 0;
        lock.l_whence = SEEK_SET;
        lock.l_len = 0;

        if (fcntl(sock, F_SETLK, &lock) != -1)
            return 0;
    } while (errno == EINTR);

    return -1;

#else
    return 0;
#endif
}

/**********************************************************************
 * Determine if the errno resulting from a failed accept() warrants a
 * retry or exit().  Based on Apache's http_main.c accept() handling
 * and Stevens' Unix Network Programming Vol 1, 2nd Ed, para. 15.6.
 */
static int is_reasonable_accept_errno (const int error)
{
    switch (error) {
#ifdef EPROTO
        /* EPROTO on certain older kernels really means ECONNABORTED, so
         * we need to ignore it for them.  See discussion in new-httpd
         * archives nh.9701 search for EPROTO.  Also see nh.9603, search
         * for EPROTO:  There is potentially a bug in Solaris 2.x x<6, and
         * other boxes that implement tcp sockets in userland (i.e. on top of
         * STREAMS).  On these systems, EPROTO can actually result in a fatal
         * loop.  See PR#981 for example.  It's hard to handle both uses of
         * EPROTO. */
        case EPROTO:
#endif
#ifdef ECONNABORTED
        case ECONNABORTED:
#endif
        /* Linux generates the rest of these, other tcp stacks (i.e.
         * bsd) tend to hide them behind getsockopt() interfaces.  They
         * occur when the net goes sour or the client disconnects after the
         * three-way handshake has been done in the kernel but before
         * userland has picked up the socket. */
#ifdef ECONNRESET
        case ECONNRESET:
#endif
#ifdef ETIMEDOUT
        case ETIMEDOUT:
#endif
#ifdef EHOSTUNREACH
        case EHOSTUNREACH:
#endif
#ifdef ENETUNREACH
        case ENETUNREACH:
#endif
            return 1;

        default:
            return 0;
    }
}

/**********************************************************************
 * This works around a problem on Linux 2.0.x and SCO Unixware (maybe
 * others?).  When a connect() is made to a Unix Domain socket, but its
 * not accept()ed before the web server gets impatient and close()s, an
 * accept() results in a valid file descriptor, but no data to read.
 * This causes a block on the first read() - which never returns!
 *
 * Another approach to this is to write() to the socket to provoke a
 * SIGPIPE, but this is a pain because of the FastCGI protocol, the fact
 * that whatever is written has to be universally ignored by all FastCGI
 * web servers, and a SIGPIPE handler has to be installed which returns
 * (or SIGPIPE is ignored).
 *
 * READABLE_UNIX_FD_DROP_DEAD_TIMEVAL = 2,0 by default.
 *
 * Making it shorter is probably safe, but I'll leave that to you.  Making
 * it 0,0 doesn't work reliably.  The shorter you can reliably make it,
 * the faster your application will be able to recover (waiting 2 seconds
 * may _cause_ the problem when there is a very high demand). At any rate,
 * this is better than perma-blocking.
 */
static int is_af_unix_keeper(const int fd)
{
    struct timeval tval = { READABLE_UNIX_FD_DROP_DEAD_TIMEVAL };
    fd_set read_fds;

    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);

    return select(fd + 1, &read_fds, NULL, NULL, &tval) >= 0 && FD_ISSET(fd, &read_fds);
}

/*
 *----------------------------------------------------------------------
 *
 * OS_Accept --
 *
 *	Accepts a new FastCGI connection.  This routine knows whether
 *      we're dealing with TCP based sockets or NT Named Pipes for IPC.
 *
 * Results:
 *      -1 if the operation fails, otherwise this is a valid IPC fd.
 *
 * Side effects:
 *      New IPC connection is accepted.
 *
 *----------------------------------------------------------------------
 */
int OS_Accept(int listen_sock, int fail_on_intr, const char *webServerAddrs)
{
    int socket = -1;
    union {
        struct sockaddr_un un;
        struct sockaddr_in in;
    } sa;

    for (;;) {
        if (AcquireLock(listen_sock, fail_on_intr))
            return -1;

        for (;;) {
            do {
#ifdef HAVE_SOCKLEN
                socklen_t len = sizeof(sa);
#else
                int len = sizeof(sa);
#endif
                if (shutdownPending) break;
                /* There's a window here */

                socket = accept(listen_sock, (struct sockaddr *)&sa, &len);
            } while (socket < 0 
                     && errno == EINTR 
                     && ! fail_on_intr 
                     && ! shutdownPending);

            if (socket < 0) {
                if (shutdownPending || ! is_reasonable_accept_errno(errno)) {
                    int errnoSave = errno;

                    ReleaseLock(listen_sock);
                    
                    if (! shutdownPending) {
                        errno = errnoSave;
                    }

                    return (-1);
                }
                errno = 0;
            }
            else {  /* socket >= 0 */
                int set = 1;

                if (sa.in.sin_family != AF_INET)
                    break;

#ifdef TCP_NODELAY
                /* No replies to outgoing data, so disable Nagle */
                setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (char *)&set, sizeof(set));
#endif

                /* Check that the client IP address is approved */
                if (ClientAddrOK(&sa.in, webServerAddrs))
                    break;

                close(socket);
            }  /* socket >= 0 */
        }  /* for(;;) */

        if (ReleaseLock(listen_sock))
            return (-1);

        if (sa.in.sin_family != AF_UNIX || is_af_unix_keeper(socket))
            break;

        close(socket);
    }  /* while(1) - lock */

    return (socket);
}

/*
 *----------------------------------------------------------------------
 *
 * OS_IpcClose
 *
 *	OS IPC routine to close an IPC connection.
 *
 * Results:
 *
 *
 * Side effects:
 *      IPC connection is closed.
 *
 *----------------------------------------------------------------------
 */
int OS_IpcClose(int ipcFd)
{
    return OS_Close(ipcFd);
}

/*
 *----------------------------------------------------------------------
 *
 * OS_IsFcgi --
 *
 *	Determines whether this process is a FastCGI process or not.
 *
 * Results:
 *      Returns 1 if FastCGI, 0 if not.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */
int OS_IsFcgi(int sock)
{
	union {
        struct sockaddr_in in;
        struct sockaddr_un un;
    } sa;
#ifdef HAVE_SOCKLEN
    socklen_t len = sizeof(sa);
#else
    int len = sizeof(sa);
#endif

    errno = 0;

    if (getpeername(sock, (struct sockaddr *)&sa, &len) != 0 && errno == ENOTCONN) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * OS_SetFlags --
 *
 *      Sets selected flag bits in an open file descriptor.
 *
 *----------------------------------------------------------------------
 */
void OS_SetFlags(int fd, int flags)
{
    int val;
    if((val = fcntl(fd, F_GETFL, 0)) < 0) {
        return;
    }
    val |= flags;
    if(fcntl(fd, F_SETFL, val) < 0) {
        return;
    }
}
