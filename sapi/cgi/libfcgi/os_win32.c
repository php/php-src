/*
 * os_win32.c --
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
 *
 * (Special thanks to Karen and Bill.  They made my job much easier and
 *  significantly more enjoyable.)
 */
#ifndef lint
static const char rcsid[] = "$Id$";
#endif /* not lint */

#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <winsock2.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <sys/timeb.h>

#define DLLAPI  __declspec(dllexport)

#include "fcgimisc.h"
#include "fcgios.h"

#define WIN32_OPEN_MAX 128 /* XXX: Small hack */

/*
 * millisecs to wait for a client connection before checking the 
 * shutdown flag (then go back to waiting for a connection, etc).
 */
#define ACCEPT_TIMEOUT 1000

#define LOCALHOST "localhost"

static HANDLE hIoCompPort = INVALID_HANDLE_VALUE;
static HANDLE hStdinCompPort = INVALID_HANDLE_VALUE;
static HANDLE hStdinThread = INVALID_HANDLE_VALUE;

static HANDLE stdioHandles[3] = {INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
				 INVALID_HANDLE_VALUE};

static HANDLE acceptMutex = INVALID_HANDLE_VALUE;

static BOOLEAN shutdownPending = FALSE;
static BOOLEAN shutdownNow = FALSE;

static BOOLEAN bImpersonate = FALSE;
/*
 * An enumeration of the file types
 * supported by the FD_TABLE structure.
 *
 * XXX: Not all currently supported.  This allows for future
 *      functionality.
 */
typedef enum {
    FD_UNUSED,
    FD_FILE_SYNC,
    FD_FILE_ASYNC,
    FD_SOCKET_SYNC,
    FD_SOCKET_ASYNC,
    FD_PIPE_SYNC,
    FD_PIPE_ASYNC
} FILE_TYPE;

typedef union {
    HANDLE fileHandle;
    SOCKET sock;
    unsigned int value;
} DESCRIPTOR;

/*
 * Structure used to map file handle and socket handle
 * values into values that can be used to create unix-like
 * select bitmaps, read/write for both sockets/files.
 */
struct FD_TABLE {
    DESCRIPTOR fid;
    FILE_TYPE type;
    char *path;
    DWORD Errno;
    unsigned long instance;
    int status;
    int offset;			/* only valid for async file writes */
    LPDWORD offsetHighPtr;	/* pointers to offset high and low words */
    LPDWORD offsetLowPtr;	/* only valid for async file writes (logs) */
    HANDLE  hMapMutex;		/* mutex handle for multi-proc offset update */
    LPVOID  ovList;		/* List of associated OVERLAPPED_REQUESTs */
};

/* 
 * XXX Note there is no dyanmic sizing of this table, so if the
 * number of open file descriptors exceeds WIN32_OPEN_MAX the 
 * app will blow up.
 */
static struct FD_TABLE fdTable[WIN32_OPEN_MAX];

static CRITICAL_SECTION  fdTableCritical;

struct OVERLAPPED_REQUEST {
    OVERLAPPED overlapped;
    unsigned long instance;	/* file instance (won't match after a close) */
    OS_AsyncProc procPtr;	/* callback routine */
    ClientData clientData;	/* callback argument */
    ClientData clientData1;	/* additional clientData */
};
typedef struct OVERLAPPED_REQUEST *POVERLAPPED_REQUEST;

static const char *bindPathPrefix = "\\\\.\\pipe\\FastCGI\\";

static FILE_TYPE listenType = FD_UNUSED;

// XXX This should be a DESCRIPTOR
static HANDLE hListen = INVALID_HANDLE_VALUE;

static OVERLAPPED listenOverlapped;
static BOOLEAN libInitialized = FALSE;

/*
 *--------------------------------------------------------------
 *
 * Win32NewDescriptor --
 *
 *	Set up for I/O descriptor masquerading.
 *
 * Results:
 *	Returns "fake id" which masquerades as a UNIX-style "small
 *	non-negative integer" file/socket descriptor.
 *	Win32_* routine below will "do the right thing" based on the
 *	descriptor's actual type. -1 indicates failure.
 *
 * Side effects:
 *	Entry in fdTable is reserved to represent the socket/file.
 *
 *--------------------------------------------------------------
 */
static int Win32NewDescriptor(FILE_TYPE type, int fd, int desiredFd)
{
    int index = -1;

    EnterCriticalSection(&fdTableCritical);

    /*
     * If desiredFd is set, try to get this entry (this is used for
     * mapping stdio handles).  Otherwise try to get the fd entry.
     * If this is not available, find a the first empty slot.  .
     */
    if (desiredFd >= 0 && desiredFd < WIN32_OPEN_MAX)
    {
        if (fdTable[desiredFd].type == FD_UNUSED) 
        {
            index = desiredFd;
        }
	}
    else if (fd > 0)
    {
        if (fd < WIN32_OPEN_MAX && fdTable[fd].type == FD_UNUSED)
        {
	        index = fd;
        }
        else 
        {
            int i;

            for (i = 1; i < WIN32_OPEN_MAX; ++i)
            {
	            if (fdTable[i].type == FD_UNUSED)
                {
                    index = i;
                    break;
                }
            }
        }
    }
    
    if (index != -1) 
    {
        fdTable[index].fid.value = fd;
        fdTable[index].type = type;
        fdTable[index].path = NULL;
        fdTable[index].Errno = NO_ERROR;
        fdTable[index].status = 0;
        fdTable[index].offset = -1;
        fdTable[index].offsetHighPtr = fdTable[index].offsetLowPtr = NULL;
        fdTable[index].hMapMutex = NULL;
        fdTable[index].ovList = NULL;
    }

    LeaveCriticalSection(&fdTableCritical);
    return index;
}

/*
 *--------------------------------------------------------------
 *
 * StdinThread--
 *
 *	This thread performs I/O on stadard input.  It is needed
 *      because you can't guarantee that all applications will
 *      create standard input with sufficient access to perform
 *      asynchronous I/O.  Since we don't want to block the app
 *      reading from stdin we make it look like it's using I/O
 *      completion ports to perform async I/O.
 *
 * Results:
 *	Data is read from stdin and posted to the io completion
 *      port.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */
static void StdinThread(LPDWORD startup){

    int doIo = TRUE;
    unsigned long fd;
    unsigned long bytesRead;
    POVERLAPPED_REQUEST pOv;

    // Touch the arg to prevent warning
    startup = NULL;

    while(doIo) {
        /*
         * Block until a request to read from stdin comes in or a
         * request to terminate the thread arrives (fd = -1).
         */
        if (!GetQueuedCompletionStatus(hStdinCompPort, &bytesRead, &fd,
	    (LPOVERLAPPED *)&pOv, (DWORD)-1) && !pOv) {
            doIo = 0;
            break;
        }

	ASSERT((fd == STDIN_FILENO) || (fd == -1));
        if(fd == -1) {
            doIo = 0;
            break;
        }
        ASSERT(pOv->clientData1 != NULL);

        if(ReadFile(stdioHandles[STDIN_FILENO], pOv->clientData1, bytesRead,
                    &bytesRead, NULL)) {
            PostQueuedCompletionStatus(hIoCompPort, bytesRead,
                                       STDIN_FILENO, (LPOVERLAPPED)pOv);
        } else {
            doIo = 0;
            break;
        }
    }

    ExitThread(0);
}

void OS_ShutdownPending(void)
{
    shutdownPending = TRUE;
}

/* XXX Need a shutdown now event */
static DWORD WINAPI ShutdownRequestThread(LPVOID arg)
{
    HANDLE shutdownEvent = (HANDLE) arg;
    WaitForSingleObject(shutdownEvent, INFINITE);
    shutdownPending = TRUE;
    // Before an accept() is entered the shutdownPending flag is checked.
    // If set, OS_Accept() will return -1.  If not, it waits
    // on a connection request for one second, checks the flag, & repeats.
    // Only one process/thread is allowed to do this at time by
    // wrapping the accept() with mutex.
    return 0;
}

int OS_SetImpersonate(void)
{
	char *os_name = NULL;
	os_name = getenv("OS");
	if (os_name && stricmp(os_name, "Windows_NT") == 0) {
		bImpersonate = TRUE;
		return 1;
	}
	return 0;
}

/*
 *--------------------------------------------------------------
 *
 * OS_LibInit --
 *
 *	Set up the OS library for use.
 *
 * Results:
 *	Returns 0 if success, -1 if not.
 *
 * Side effects:
 *	Sockets initialized, pseudo file descriptors setup, etc.
 *
 *--------------------------------------------------------------
 */
int OS_LibInit(int stdioFds[3])
{
    WORD  wVersion;
    WSADATA wsaData;
    int err;
    int fakeFd;
    DWORD threadId;
    char *cLenPtr = NULL;
    char *val = NULL;
        
    if(libInitialized)
        return 0;

    InitializeCriticalSection(&fdTableCritical);   
    
    /*
     * Initialize windows sockets library.
     */
    wVersion = MAKEWORD(2,0);
    err = WSAStartup( wVersion, &wsaData );
    if (err) {
        fprintf(stderr, "Error starting Windows Sockets.  Error: %d",
		WSAGetLastError());
	//exit(111);
		return -1;
    }

    /*
     * Create the I/O completion port to be used for our I/O queue.
     */
    if (hIoCompPort == INVALID_HANDLE_VALUE) {
	hIoCompPort = CreateIoCompletionPort (INVALID_HANDLE_VALUE, NULL,
					      0, 1);
	if(hIoCompPort == INVALID_HANDLE_VALUE) {
	    printf("<H2>OS_LibInit Failed CreateIoCompletionPort!  ERROR: %d</H2>\r\n\r\n",
	       GetLastError());
	    return -1;
	}
    }

    /*
     * If a shutdown event is in the env, save it (I don't see any to 
     * remove it from the environment out from under the application).
     * Spawn a thread to wait on the shutdown request.
     */
    val = getenv(SHUTDOWN_EVENT_NAME);
    if (val != NULL) 
    {
        HANDLE shutdownEvent = (HANDLE) atoi(val);

        if (! CreateThread(NULL, 0, ShutdownRequestThread, 
                           shutdownEvent, 0, NULL))
        {
            return -1;
        }
    }

    /*
     * If an accept mutex is in the env, save it and remove it.
     */
    val = getenv(MUTEX_VARNAME);
    if (val != NULL) 
    {
        acceptMutex = (HANDLE) atoi(val);
    }


    /*
     * Determine if this library is being used to listen for FastCGI
     * connections.  This is communicated by STDIN containing a
     * valid handle to a listener object.  In this case, both the
     * "stdout" and "stderr" handles will be INVALID (ie. closed) by
     * the starting process.
     *
     * The trick is determining if this is a pipe or a socket...
     *
     * XXX: Add the async accept test to determine socket or handle to a
     *      pipe!!!
     */
    if((GetStdHandle(STD_OUTPUT_HANDLE) == INVALID_HANDLE_VALUE) &&
       (GetStdHandle(STD_ERROR_HANDLE)  == INVALID_HANDLE_VALUE) &&
       (GetStdHandle(STD_INPUT_HANDLE)  != INVALID_HANDLE_VALUE) ) 
    {
        DWORD pipeMode = PIPE_READMODE_BYTE | PIPE_WAIT;
        HANDLE oldStdIn = GetStdHandle(STD_INPUT_HANDLE);

        // Move the handle to a "low" number
        if (! DuplicateHandle(GetCurrentProcess(), oldStdIn,
                              GetCurrentProcess(), &hListen,
                              0, TRUE, DUPLICATE_SAME_ACCESS))
        {
            return -1;
        }

        if (! SetStdHandle(STD_INPUT_HANDLE, hListen))
        {
            return -1;
        }

        CloseHandle(oldStdIn);

	/*
	 * Set the pipe handle state so that it operates in wait mode.
	 *
	 * NOTE: The listenFd is not mapped to a pseudo file descriptor
	 *       as all work done on it is contained to the OS library.
	 *
	 * XXX: Initial assumption is that SetNamedPipeHandleState will
	 *      fail if this is an IP socket...
	 */
        if (SetNamedPipeHandleState(hListen, &pipeMode, NULL, NULL)) 
        {
            listenType = FD_PIPE_SYNC;
            listenOverlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        } 
        else 
        {
            listenType = FD_SOCKET_SYNC;
        }
    }

    /*
     * If there are no stdioFds passed in, we're done.
     */
    if(stdioFds == NULL) {
        libInitialized = 1;
        return 0;
    }

    /*
     * Setup standard input asynchronous I/O.  There is actually a separate
     * thread spawned for this purpose.  The reason for this is that some
     * web servers use anonymous pipes for the connection between itself
     * and a CGI application.  Anonymous pipes can't perform asynchronous
     * I/O or use I/O completion ports.  Therefore in order to present a
     * consistent I/O dispatch model to an application we emulate I/O
     * completion port behavior by having the standard input thread posting
     * messages to the hIoCompPort which look like a complete overlapped
     * I/O structure.  This keeps the event dispatching simple from the
     * application perspective.
     */
    stdioHandles[STDIN_FILENO] = GetStdHandle(STD_INPUT_HANDLE);

    if(!SetHandleInformation(stdioHandles[STDIN_FILENO],
			     HANDLE_FLAG_INHERIT, 0)) {
/*
 * XXX: Causes error when run from command line.  Check KB
        err = GetLastError();
        DebugBreak();
	exit(99);
 */
		return -1;
    }

    if ((fakeFd = Win32NewDescriptor(FD_PIPE_SYNC,
				     (int)stdioHandles[STDIN_FILENO],
				     STDIN_FILENO)) == -1) {
        return -1;
    } else {
        /*
	 * Set stdin equal to our pseudo FD and create the I/O completion
	 * port to be used for async I/O.
	 */
	stdioFds[STDIN_FILENO] = fakeFd;
    }

    /*
     * Create the I/O completion port to be used for communicating with
     * the thread doing I/O on standard in.  This port will carry read
     * and possibly thread termination requests to the StdinThread.
     */
    if (hStdinCompPort == INVALID_HANDLE_VALUE) {
	hStdinCompPort = CreateIoCompletionPort (INVALID_HANDLE_VALUE, NULL,
					      0, 1);
	if(hStdinCompPort == INVALID_HANDLE_VALUE) {
	    printf("<H2>OS_LibInit Failed CreateIoCompletionPort: STDIN!  ERROR: %d</H2>\r\n\r\n",
	       GetLastError());
	    return -1;
	}
    }

    /*
     * Create the thread that will read stdin if the CONTENT_LENGTH
     * is non-zero.
     */
    if((cLenPtr = getenv("CONTENT_LENGTH")) != NULL &&
       atoi(cLenPtr) > 0) {
        hStdinThread = CreateThread(NULL, 8192,
				    (LPTHREAD_START_ROUTINE)&StdinThread,
				    NULL, 0, &threadId);
	if (hStdinThread == NULL) {
	    printf("<H2>OS_LibInit Failed to create STDIN thread!  ERROR: %d</H2>\r\n\r\n",
		   GetLastError());
	    return -1;
        }
    }

    /*
     * STDOUT will be used synchronously.
     *
     * XXX: May want to convert this so that it could be used for OVERLAPPED
     *      I/O later.  If so, model it after the Stdin I/O as stdout is
     *      also incapable of async I/O on some servers.
     */
    stdioHandles[STDOUT_FILENO] = GetStdHandle(STD_OUTPUT_HANDLE);
    if(!SetHandleInformation(stdioHandles[STDOUT_FILENO],
			     HANDLE_FLAG_INHERIT, FALSE)) {
	//exit(99);
		return -1;
    }

    if ((fakeFd = Win32NewDescriptor(FD_PIPE_SYNC,
				     (int)stdioHandles[STDOUT_FILENO],
				     STDOUT_FILENO)) == -1) {
        return -1;
    } else {
        /*
	 * Set stdout equal to our pseudo FD
	 */
	stdioFds[STDOUT_FILENO] = fakeFd;
    }

    stdioHandles[STDERR_FILENO] = GetStdHandle(STD_ERROR_HANDLE);
    if(!SetHandleInformation(stdioHandles[STDERR_FILENO],
			     HANDLE_FLAG_INHERIT, FALSE)) {
	//exit(99);
		return -1;
    }
    if ((fakeFd = Win32NewDescriptor(FD_PIPE_SYNC,
				     (int)stdioHandles[STDERR_FILENO],
				     STDERR_FILENO)) == -1) {
        return -1;
    } else {
        /*
	 * Set stderr equal to our pseudo FD
	 */
	stdioFds[STDERR_FILENO] = fakeFd;
    }

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
 *	Memory freed, handles closed.
 *
 *--------------------------------------------------------------
 */
void OS_LibShutdown()
{

    if (hIoCompPort != INVALID_HANDLE_VALUE) 
    {
        CloseHandle(hIoCompPort);
        hIoCompPort = INVALID_HANDLE_VALUE;
    }

    if (hStdinCompPort != INVALID_HANDLE_VALUE) 
    {
        CloseHandle(hStdinCompPort);
        hStdinCompPort = INVALID_HANDLE_VALUE;
    }

    if (acceptMutex != INVALID_HANDLE_VALUE) 
    {
        ReleaseMutex(acceptMutex);
		CloseHandle(acceptMutex);
    }

	/* we only want to do this if we're not a web server */
    if (stdioHandles[0] != INVALID_HANDLE_VALUE) {
		DisconnectNamedPipe(hListen);
		CancelIo(hListen);
		if (bImpersonate) RevertToSelf();
	}

	DeleteCriticalSection(&fdTableCritical);
    WSACleanup();
}

/*
 *--------------------------------------------------------------
 *
 * Win32FreeDescriptor --
 *
 *	Free I/O descriptor entry in fdTable.
 *
 * Results:
 *	Frees I/O descriptor entry in fdTable.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */
static void Win32FreeDescriptor(int fd)
{
    /* Catch it if fd is a bogus value */
    ASSERT((fd >= 0) && (fd < WIN32_OPEN_MAX));

    EnterCriticalSection(&fdTableCritical);
    
    if (fdTable[fd].type != FD_UNUSED)
    {   
        switch (fdTable[fd].type) 
        {
	    case FD_FILE_SYNC:
	    case FD_FILE_ASYNC:
        
	        /* Free file path string */
	        ASSERT(fdTable[fd].path != NULL);
			
	        free(fdTable[fd].path);
	        fdTable[fd].path = NULL;
	        break;
		case FD_PIPE_ASYNC:
			break;
	    default:
	        break;
        }

        ASSERT(fdTable[fd].path == NULL);

        fdTable[fd].type = FD_UNUSED;
        fdTable[fd].path = NULL;
        fdTable[fd].Errno = NO_ERROR;
        fdTable[fd].offsetHighPtr = fdTable[fd].offsetLowPtr = NULL;

        if (fdTable[fd].hMapMutex != NULL) 
        {
            CloseHandle(fdTable[fd].hMapMutex);
            fdTable[fd].hMapMutex = NULL;
        }
    }

    LeaveCriticalSection(&fdTableCritical);

    return;
}

static short getPort(const char * bindPath)
{
    short port = 0;
    char * p = strchr(bindPath, ':');

    if (p && *++p) 
    {
        char buf[6];

        strncpy(buf, p, 6);
        buf[5] = '\0';

        port = (short) atoi(buf);
    }
 
    return port;
}

/**
This function builds a Dacl which grants the creator of the objects
FILE_ALL_ACCESS and Everyone FILE_GENERIC_READ and FILE_GENERIC_WRITE
access to the object.

This Dacl allows for higher security than a NULL Dacl, which is common for
named-pipes, as this only grants the creator/owner write access to the
security descriptor, and grants Everyone the ability to "use" the named-pipe.
This scenario prevents a malevolent user from disrupting service by preventing
arbitrary access manipulation.
**/
BOOL
BuildNamedPipeAcl(
    PACL pAcl,
    PDWORD cbAclSize
    )
{
    DWORD dwAclSize;

    SID_IDENTIFIER_AUTHORITY siaWorld = SECURITY_WORLD_SID_AUTHORITY;
    SID_IDENTIFIER_AUTHORITY siaCreator = SECURITY_CREATOR_SID_AUTHORITY;

    BYTE BufEveryoneSid[32];
    BYTE BufOwnerSid[32];

    PSID pEveryoneSid = (PSID)BufEveryoneSid;
    PSID pOwnerSid = (PSID)BufOwnerSid;

    //
    // compute size of acl
    //
    dwAclSize = sizeof(ACL) +
        2 * ( sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD) ) +
        GetSidLengthRequired( 1 ) + // well-known Everyone Sid
        GetSidLengthRequired( 1 ) ; // well-known Creator Owner Sid

    if(*cbAclSize < dwAclSize) {
        *cbAclSize = dwAclSize;
        return FALSE;
    }

    *cbAclSize = dwAclSize;

    //
    // intialize well known sids
    //

    if(!InitializeSid(pEveryoneSid, &siaWorld, 1)) return FALSE;
    *GetSidSubAuthority(pEveryoneSid, 0) = SECURITY_WORLD_RID;

    if(!InitializeSid(pOwnerSid, &siaCreator, 1)) return FALSE;
    *GetSidSubAuthority(pOwnerSid, 0) = SECURITY_CREATOR_OWNER_RID;

    if(!InitializeAcl(pAcl, dwAclSize, ACL_REVISION))
        return FALSE;

    //
    //
    if(!AddAccessAllowedAce(
        pAcl,
        ACL_REVISION,
        FILE_GENERIC_READ | FILE_GENERIC_WRITE,
        pEveryoneSid
        ))
        return FALSE;

    //
    //
    return AddAccessAllowedAce(
        pAcl,
        ACL_REVISION,
        FILE_ALL_ACCESS,
        pOwnerSid
        );
}


/*
 * OS_CreateLocalIpcFd --
 *
 *   This procedure is responsible for creating the listener pipe
 *   on Windows NT for local process communication.  It will create a
 *   named pipe and return a file descriptor to it to the caller.
 *
 * Results:
 *      Listener pipe created.  This call returns either a valid
 *      pseudo file descriptor or -1 on error.
 *
 * Side effects:
 *      Listener pipe and IPC address are stored in the FCGI info
 *         structure.
 *      'errno' will set on errors (-1 is returned).
 *
 *----------------------------------------------------------------------
 */
int OS_CreateLocalIpcFd(const char *bindPath, int backlog, int bCreateMutex)
{
    int pseudoFd = -1;
    short port = getPort(bindPath);
    HANDLE mutex = INVALID_HANDLE_VALUE;
    char mutexEnvString[100];

	if (bCreateMutex) {
		mutex = CreateMutex(NULL, FALSE, NULL);
		if (! SetHandleInformation(mutex, HANDLE_FLAG_INHERIT, TRUE))
		{
			CloseHandle(mutex);
			return -3;
		}
		// This is a nail for listening to more than one port..
		// This should really be handled by the caller.
		_snprintf(mutexEnvString, sizeof(mutexEnvString)-1, MUTEX_VARNAME "=%d", (int) mutex);
		putenv(mutexEnvString);
	}

    // There's nothing to be gained (at the moment) by a shutdown Event    

    if (port && *bindPath != ':' && strncmp(bindPath, LOCALHOST, strlen(LOCALHOST)))
    {
	    fprintf(stderr, "To start a service on a TCP port can not "
			    "specify a host name.\n"
			    "You should either use \"localhost:<port>\" or "
			    " just use \":<port>.\"\n");
	    //exit(1);
		if (bCreateMutex) CloseHandle(mutexEnvString);
		return -1;
    }

    listenType = (port) ? FD_SOCKET_SYNC : FD_PIPE_ASYNC;
    
    if (port) 
    {
        SOCKET listenSock;
        struct  sockaddr_in	sockAddr;
        int sockLen = sizeof(sockAddr);
        
        memset(&sockAddr, 0, sizeof(sockAddr));
        sockAddr.sin_family = AF_INET;
        sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        sockAddr.sin_port = htons(port);

        listenSock = socket(AF_INET, SOCK_STREAM, 0);
        if (listenSock == INVALID_SOCKET) 
        {
			if (bCreateMutex)CloseHandle(mutexEnvString);
	        return -4;
	    }

	    if (bind(listenSock, (struct sockaddr *) &sockAddr, sockLen)  )
        {
			if (bCreateMutex)CloseHandle(mutexEnvString);
	        return -12;
	    }

	    if (listen(listenSock, backlog)) 
        {
			if (bCreateMutex)CloseHandle(mutexEnvString);
	        return -5;
	    }

        pseudoFd = Win32NewDescriptor(listenType, listenSock, -1);
        
        if (pseudoFd == -1) 
        {
			if (bCreateMutex)CloseHandle(mutexEnvString);
            closesocket(listenSock);
            return -6;
        }

        hListen = (HANDLE) listenSock;        
    }
    else
    {
		SECURITY_ATTRIBUTES sa;
		SECURITY_DESCRIPTOR sd;
		BYTE AclBuf[ 64 ];
		DWORD cbAclSize = 64;
		PACL pAcl = (PACL)AclBuf;

		HANDLE hListenPipe = INVALID_HANDLE_VALUE;
        char *pipePath = malloc(strlen(bindPathPrefix) + strlen(bindPath) + 1);
        
        if (! pipePath) 
        {
			if (bCreateMutex)CloseHandle(mutexEnvString);
            return -7;
        }

        strcpy(pipePath, bindPathPrefix);
        strcat(pipePath, bindPath);

		if (bImpersonate) {
			// get the security attributes for Everybody to connect
			// we do this so that multithreaded servers that run
			// threads under secured users can access pipes created
			// by a system level thread (for instance, IIS)
			//
			// suppress errors regarding startup directory, etc
			//
			SetErrorMode(SEM_FAILCRITICALERRORS);

			if(!BuildNamedPipeAcl(pAcl, &cbAclSize)) {
				fprintf(stderr, "BuildNamedPipeAcl");
				return -100;
			}

			if(!InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION)) {
				fprintf(stderr, "InitializeSecurityDescriptor");
				return -100;
			}

			if(!SetSecurityDescriptorDacl(&sd, TRUE, pAcl, FALSE)) {
				fprintf(stderr, "SetSecurityDescriptorDacl");
				return -100;
			}

		    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
			sa.lpSecurityDescriptor = &sd; // default Dacl of caller
			sa.bInheritHandle = TRUE;

		}

        hListenPipe = CreateNamedPipe(pipePath,
		        PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
		        PIPE_TYPE_BYTE | PIPE_WAIT | PIPE_READMODE_BYTE,
		        PIPE_UNLIMITED_INSTANCES,
				4096, 4096, 0, bImpersonate?&sa:NULL);
        
        free(pipePath);

        if (hListenPipe == INVALID_HANDLE_VALUE)
        {
			if (bCreateMutex)CloseHandle(mutexEnvString);
            return -8;
        }

        if (! SetHandleInformation(hListenPipe, HANDLE_FLAG_INHERIT, TRUE))
        {
			if (bCreateMutex)CloseHandle(mutexEnvString);
            return -9;
        }

        pseudoFd = Win32NewDescriptor(listenType, (int) hListenPipe, -1);
        
        if (pseudoFd == -1) 
        {
			if (bCreateMutex)CloseHandle(mutexEnvString);
            CloseHandle(hListenPipe);
            return -10;
        }

        hListen = (HANDLE) hListenPipe;
    }

    return pseudoFd;
}

/*
 *----------------------------------------------------------------------
 *
 * OS_FcgiConnect --
 *
 *	Create the pipe pathname connect to the remote application if
 *      possible.
 *
 * Results:
 *      -1 if fail or a valid handle if connection succeeds.
 *
 * Side effects:
 *      Remote connection established.
 *
 *----------------------------------------------------------------------
 */
int OS_FcgiConnect(char *bindPath)
{
    short port = getPort(bindPath);
    int pseudoFd = -1;
    unsigned int flags = FILE_FLAG_OVERLAPPED;
    if (port) 
    {
	    struct hostent *hp;
        char *host = NULL;
        struct sockaddr_in sockAddr;
        int sockLen = sizeof(sockAddr);
        SOCKET sock;
        
        if (*bindPath != ':')
        {
            char * p = strchr(bindPath, ':');
            if (p) {
                int len = p - bindPath + 1;

                host = (char *)malloc(len);
                if (!host) {
                    fprintf(stderr, "Unable to allocate memory\n");
                    return -1;
                }
                strncpy(host, bindPath, len);
                host[len-1] = '\0';
            }
        }
        
        hp = gethostbyname(host ? host : LOCALHOST);

        if (host)
        {
            free(host);
        }

	    if (hp == NULL) 
        {
	        fprintf(stderr, "Unknown host: %s\n", bindPath);
	        return -1;
	    }
       
        memset(&sockAddr, 0, sizeof(sockAddr));
        sockAddr.sin_family = AF_INET;
	    memcpy(&sockAddr.sin_addr, hp->h_addr, hp->h_length);
	    sockAddr.sin_port = htons(port);

	    sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET)
        {
            return -1;
        }

        if (connect(sock, (struct sockaddr *) &sockAddr, sockLen) == SOCKET_ERROR) 
        {
	        closesocket(sock);
	        return -1;
	    }

	    pseudoFd = Win32NewDescriptor(FD_SOCKET_SYNC, sock, -1);
	    if (pseudoFd == -1) 
        {
	        closesocket(sock);
            return -1;
	    }
    }
    else
    {
        char *pipePath = malloc(strlen(bindPathPrefix) + strlen(bindPath) + 1);
        HANDLE hPipe;
        
        if (! pipePath) 
        {
            return -1;
        }

        strcpy(pipePath, bindPathPrefix);
        strcat(pipePath, bindPath);

		if (bImpersonate) {
			flags |= SECURITY_SQOS_PRESENT | SECURITY_IMPERSONATION;
		}

        hPipe = CreateFile(pipePath,
			    GENERIC_WRITE | GENERIC_READ,
			    FILE_SHARE_READ | FILE_SHARE_WRITE,
			    NULL,
			    OPEN_EXISTING,
			    flags,
			    NULL);

        free(pipePath);

        if( hPipe == INVALID_HANDLE_VALUE || hPipe == 0) 
        {
            return -1;
        }

        pseudoFd = Win32NewDescriptor(FD_PIPE_ASYNC, (int) hPipe, -1);
        
        if (pseudoFd == -1) 
        {
            CloseHandle(hPipe);
            return -1;
        } 
        
        /*
	     * Set stdin equal to our pseudo FD and create the I/O completion
	     * port to be used for async I/O.
	     */
        if (! CreateIoCompletionPort(hPipe, hIoCompPort, pseudoFd, 1))
        {
	        Win32FreeDescriptor(pseudoFd);
	        CloseHandle(hPipe);
	        return -1;
	    }
    }

    return pseudoFd;    
}

/*
 *--------------------------------------------------------------
 *
 * OS_Read --
 *
 *	Pass through to the appropriate NT read function.
 *
 * Results:
 *	Returns number of byes read. Mimics unix read:.
 *		n bytes read, 0 or -1 failure: errno contains actual error
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */
int OS_Read(int fd, char * buf, size_t len)
{
    DWORD bytesRead;
    int ret = -1;

    ASSERT((fd >= 0) && (fd < WIN32_OPEN_MAX));

    if (shutdownNow) return -1;

    switch (fdTable[fd].type) 
    {
	case FD_FILE_SYNC:
	case FD_FILE_ASYNC:
	case FD_PIPE_SYNC:
	case FD_PIPE_ASYNC:

	    if (ReadFile(fdTable[fd].fid.fileHandle, buf, len, &bytesRead, NULL)) 
        {
            ret = bytesRead;
        }
        else
        {
		    fdTable[fd].Errno = GetLastError();
			ret = -1;
	    }

        break;

	case FD_SOCKET_SYNC:
	case FD_SOCKET_ASYNC:

        ret = recv(fdTable[fd].fid.sock, buf, len, 0);
	    if (ret == SOCKET_ERROR) 
        {
		    fdTable[fd].Errno = WSAGetLastError();
		    ret = -1;
	    }

        break;
        
    default:

        ASSERT(0);
    }

    return ret;
}

/*
 *--------------------------------------------------------------
 *
 * OS_Write --
 *
 *	Perform a synchronous OS write.
 *
 * Results:
 *	Returns number of bytes written. Mimics unix write:
 *		n bytes written, 0 or -1 failure (??? couldn't find man page).
 *
 * Side effects:
 *	none.
 *
 *--------------------------------------------------------------
 */
int OS_Write(int fd, char * buf, size_t len)
{
    DWORD bytesWritten;
    int ret = -1;

    ASSERT(fd >= 0 && fd < WIN32_OPEN_MAX);

    if (shutdownNow) return -1;

    switch (fdTable[fd].type) 
    {
	case FD_FILE_SYNC:
	case FD_FILE_ASYNC:
	case FD_PIPE_SYNC:
	case FD_PIPE_ASYNC:

        if (WriteFile(fdTable[fd].fid.fileHandle, buf, len, &bytesWritten, NULL)) 
        {
            ret = bytesWritten;
        }
        else
        {
		    fdTable[fd].Errno = GetLastError();
	    }

        break;

	case FD_SOCKET_SYNC:
	case FD_SOCKET_ASYNC:

        ret = send(fdTable[fd].fid.sock, buf, len, 0);
        if (ret == SOCKET_ERROR) 
        {
		    fdTable[fd].Errno = WSAGetLastError();
		    ret = -1;
	    }

        break;

    default:

        ASSERT(0);
    }

    return ret;
}

/*
 *----------------------------------------------------------------------
 *
 * OS_SpawnChild --
 *
 *	Spawns a new server listener process, and stores the information
 *      relating to the child in the supplied record.  A wait handler is
 *	registered on the child's completion.  This involves creating
 *        a process on NT and preparing a command line with the required
 *        state (currently a -childproc flag and the server socket to use
 *        for accepting connections).
 *
 * Results:
 *      0 if success, -1 if error.
 *
 * Side effects:
 *      Child process spawned.
 *
 *----------------------------------------------------------------------
 */
int OS_SpawnChild(char *execPath, int listenFd, PROCESS_INFORMATION *pInfo, char *env)
{
    STARTUPINFO StartupInfo;
    BOOL success;

    memset((void *)&StartupInfo, 0, sizeof(STARTUPINFO));
    StartupInfo.cb = sizeof (STARTUPINFO);
    StartupInfo.lpReserved = NULL;
    StartupInfo.lpReserved2 = NULL;
    StartupInfo.cbReserved2 = 0;
    StartupInfo.lpDesktop = NULL;
	StartupInfo.wShowWindow = SW_HIDE;
    /*
     * FastCGI on NT will set the listener pipe HANDLE in the stdin of
     * the new process.  The fact that there is a stdin and NULL handles
     * for stdout and stderr tells the FastCGI process that this is a
     * FastCGI process and not a CGI process.
     */
    StartupInfo.dwFlags = STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
    /*
     * XXX: Do I have to dup the handle before spawning the process or is
     *      it sufficient to use the handle as it's reference counted
     *      by NT anyway?
     */
    StartupInfo.hStdInput  = fdTable[listenFd].fid.fileHandle;
    StartupInfo.hStdOutput = INVALID_HANDLE_VALUE;
    StartupInfo.hStdError  = INVALID_HANDLE_VALUE;

    /*
     * Make the listener socket inheritable.
     */
    success = SetHandleInformation(StartupInfo.hStdInput, HANDLE_FLAG_INHERIT,
				   TRUE);
    if(!success) {
        //exit(99);
		return -1;
    }

    /*
     * XXX: Might want to apply some specific security attributes to the
     *      processes.
     */
    success = CreateProcess(execPath,	/* LPCSTR address of module name */
			NULL,           /* LPCSTR address of command line */
		        NULL,		/* Process security attributes */
			NULL,		/* Thread security attributes */
			TRUE,		/* Inheritable Handes inherited. */
			0,		/* DWORD creation flags  */
		    env,           /* Use parent environment block */
			NULL,		/* Address of current directory name */
			&StartupInfo,   /* Address of STARTUPINFO  */
			pInfo);	/* Address of PROCESS_INFORMATION   */
    if(success) {
        return 0;
    } else {
        return -1;
    }
}

/*
 *--------------------------------------------------------------
 *
 * OS_AsyncReadStdin --
 *
 *	This initiates an asynchronous read on the standard
 *	input handle.  This handle is not guaranteed to be
 *      capable of performing asynchronous I/O so we send a
 *      message to the StdinThread to do the synchronous read.
 *
 * Results:
 *	-1 if error, 0 otherwise.
 *
 * Side effects:
 *	Asynchronous message is queued to the StdinThread and an
 *      overlapped structure is allocated/initialized.
 *
 *--------------------------------------------------------------
 */
int OS_AsyncReadStdin(void *buf, int len, OS_AsyncProc procPtr,
                      ClientData clientData)
{
    POVERLAPPED_REQUEST pOv;

    ASSERT(fdTable[STDIN_FILENO].type != FD_UNUSED);

    pOv = (POVERLAPPED_REQUEST)malloc(sizeof(struct OVERLAPPED_REQUEST));
    ASSERT(pOv);
    memset((void *)pOv, 0, sizeof(struct OVERLAPPED_REQUEST));
    pOv->clientData1 = (ClientData)buf;
    pOv->instance = fdTable[STDIN_FILENO].instance;
    pOv->procPtr = procPtr;
    pOv->clientData = clientData;

    PostQueuedCompletionStatus(hStdinCompPort, len, STDIN_FILENO,
                               (LPOVERLAPPED)pOv);
    return 0;
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
    DWORD bytesRead;
    POVERLAPPED_REQUEST pOv;

    /*
     * Catch any bogus fd values
     */
    ASSERT((fd >= 0) && (fd < WIN32_OPEN_MAX));
    /*
     * Confirm that this is an async fd
     */
    ASSERT(fdTable[fd].type != FD_UNUSED);
    ASSERT(fdTable[fd].type != FD_FILE_SYNC);
    ASSERT(fdTable[fd].type != FD_PIPE_SYNC);
    ASSERT(fdTable[fd].type != FD_SOCKET_SYNC);

    pOv = (POVERLAPPED_REQUEST)malloc(sizeof(struct OVERLAPPED_REQUEST));
    ASSERT(pOv);
    memset((void *)pOv, 0, sizeof(struct OVERLAPPED_REQUEST));
    /*
     * Only file offsets should be non-zero, but make sure.
     */
    if (fdTable[fd].type == FD_FILE_ASYNC)
	if (fdTable[fd].offset >= 0)
	    pOv->overlapped.Offset = fdTable[fd].offset;
	else
	    pOv->overlapped.Offset = offset;
    pOv->instance = fdTable[fd].instance;
    pOv->procPtr = procPtr;
    pOv->clientData = clientData;
    bytesRead = fd;
    /*
     * ReadFile returns: TRUE success, FALSE failure
     */
    if (!ReadFile(fdTable[fd].fid.fileHandle, buf, len, &bytesRead,
	(LPOVERLAPPED)pOv)) {
	fdTable[fd].Errno = GetLastError();
	if(fdTable[fd].Errno == ERROR_NO_DATA ||
	   fdTable[fd].Errno == ERROR_PIPE_NOT_CONNECTED) {
	    PostQueuedCompletionStatus(hIoCompPort, 0, fd, (LPOVERLAPPED)pOv);
	    return 0;
	}
	if(fdTable[fd].Errno != ERROR_IO_PENDING) {
	    PostQueuedCompletionStatus(hIoCompPort, 0, fd, (LPOVERLAPPED)pOv);
	    return -1;
	}
	fdTable[fd].Errno = 0;
    }
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
    DWORD bytesWritten;
    POVERLAPPED_REQUEST pOv;

    /*
     * Catch any bogus fd values
     */
    ASSERT((fd >= 0) && (fd < WIN32_OPEN_MAX));
    /*
     * Confirm that this is an async fd
     */
    ASSERT(fdTable[fd].type != FD_UNUSED);
    ASSERT(fdTable[fd].type != FD_FILE_SYNC);
    ASSERT(fdTable[fd].type != FD_PIPE_SYNC);
    ASSERT(fdTable[fd].type != FD_SOCKET_SYNC);

    pOv = (POVERLAPPED_REQUEST)malloc(sizeof(struct OVERLAPPED_REQUEST));
    ASSERT(pOv);
    memset((void *)pOv, 0, sizeof(struct OVERLAPPED_REQUEST));
    /*
     * Only file offsets should be non-zero, but make sure.
     */
    if (fdTable[fd].type == FD_FILE_ASYNC)
	/*
	 * Only file opened via OS_AsyncWrite with
	 * O_APPEND will have an offset != -1.
	 */
	if (fdTable[fd].offset >= 0)
	    /*
	     * If the descriptor has a memory mapped file
	     * handle, take the offsets from there.
	     */
	    if (fdTable[fd].hMapMutex != NULL) {
		/*
		 * Wait infinitely; this *should* not cause problems.
		 */
		WaitForSingleObject(fdTable[fd].hMapMutex, INFINITE);

		/*
		 * Retrieve the shared offset values.
		 */
		pOv->overlapped.OffsetHigh = *(fdTable[fd].offsetHighPtr);
		pOv->overlapped.Offset = *(fdTable[fd].offsetLowPtr);

		/*
		 * Update the shared offset values for the next write
		 */
		*(fdTable[fd].offsetHighPtr) += 0;	/* XXX How do I handle overflow */
		*(fdTable[fd].offsetLowPtr) += len;

		ReleaseMutex(fdTable[fd].hMapMutex);
	    } else
	        pOv->overlapped.Offset = fdTable[fd].offset;
	else
	    pOv->overlapped.Offset = offset;
    pOv->instance = fdTable[fd].instance;
    pOv->procPtr = procPtr;
    pOv->clientData = clientData;
    bytesWritten = fd;
    /*
     * WriteFile returns: TRUE success, FALSE failure
     */
    if (!WriteFile(fdTable[fd].fid.fileHandle, buf, len, &bytesWritten,
	(LPOVERLAPPED)pOv)) {
	fdTable[fd].Errno = GetLastError();
	if(fdTable[fd].Errno != ERROR_IO_PENDING) {
	    PostQueuedCompletionStatus(hIoCompPort, 0, fd, (LPOVERLAPPED)pOv);
	    return -1;
	}
	fdTable[fd].Errno = 0;
    }
    if (fdTable[fd].offset >= 0)
	fdTable[fd].offset += len;
    return 0;
}

/*
 *--------------------------------------------------------------
 *
 * OS_Close --
 *
 *	Closes the descriptor with routine appropriate for
 *      descriptor's type.
 *
 * Results:
 *	Socket or file is closed. Return values mimic Unix close:
 *		0 success, -1 failure
 *
 * Side effects:
 *	Entry in fdTable is marked as free.
 *
 *--------------------------------------------------------------
 */
int OS_Close(int fd)
{
    int ret = 0;

    /*
     * Catch it if fd is a bogus value
     */
    ASSERT((fd >= 0) && (fd < WIN32_OPEN_MAX));
    ASSERT(fdTable[fd].type != FD_UNUSED);

    switch (fdTable[fd].type) {
	case FD_PIPE_SYNC:
	case FD_PIPE_ASYNC:
	case FD_FILE_SYNC:
	case FD_FILE_ASYNC:
        /*
         * CloseHandle returns: TRUE success, 0 failure
         */
        /*
        XXX don't close here, fcgi apps fail if we do so
        need to examine resource leaks if any might exist
		if (CloseHandle(fdTable[fd].fid.fileHandle) == FALSE)
			ret = -1;
	*/
        break;
    case FD_SOCKET_SYNC:
	case FD_SOCKET_ASYNC:
	    /*
	     * Closing a socket that has an async read outstanding causes a
	     * tcp reset and possible data loss.  The shutdown call seems to
	     * prevent this.
	     */

        /* shutdown(fdTable[fd].fid.sock, SD_BOTH); */

        {
        char buf[16];
        int r;

        shutdown(fdTable[fd].fid.sock,SD_SEND);

        do
        { 
            r = recv(fdTable[fd].fid.sock,buf,16,0);
        } while (r > 0);
        }
        /*
	     * closesocket returns: 0 success, SOCKET_ERROR failure
	     */
	    if (closesocket(fdTable[fd].fid.sock) == SOCKET_ERROR)
			ret = -1;
	    break;
	default:
	    return -1;		/* fake failure */
    }

    Win32FreeDescriptor(fd);
    return ret;
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
    int ret = 0;

    /*
     * Catch it if fd is a bogus value
     */
    ASSERT((fd >= 0) && (fd < WIN32_OPEN_MAX));
    ASSERT(fdTable[fd].type == FD_SOCKET_ASYNC
	|| fdTable[fd].type == FD_SOCKET_SYNC);

    if (shutdown(fdTable[fd].fid.sock,SD_RECEIVE) == SOCKET_ERROR)
	ret = -1;
    return ret;
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
    unsigned long fd;
    unsigned long bytes;
    POVERLAPPED_REQUEST pOv;
    struct timeb tb;
    int ms;
    int ms_last;
    int err;

    /* XXX
     * We can loop in here, but not too long, as wait handlers
     * must run.
     * For cgi stdin, apparently select returns when io completion
     * ports don't, so don't wait the full timeout.
     */
    if(tmo)
	ms = (tmo->tv_sec*1000 + tmo->tv_usec/1000) / 2;
    else
	ms = 1000;
    ftime(&tb);
    ms_last = tb.time*1000 + tb.millitm;
    while (ms >= 0) {
	if(tmo && (ms = tmo->tv_sec*1000 + tmo->tv_usec/1000)> 100)
	    ms = 100;
	if (!GetQueuedCompletionStatus(hIoCompPort, &bytes, &fd,
	    (LPOVERLAPPED *)&pOv, ms) && !pOv) {
	    err = WSAGetLastError();
	    return 0; /* timeout */
        }

	ASSERT((fd >= 0) && (fd < WIN32_OPEN_MAX));
	/* call callback if descriptor still valid */
	ASSERT(pOv);
	if(pOv->instance == fdTable[fd].instance)
	  (*pOv->procPtr)(pOv->clientData, bytes);
	free(pOv);

	ftime(&tb);
	ms -= (tb.time*1000 + tb.millitm - ms_last);
	ms_last = tb.time*1000 + tb.millitm;
    }
    return 0;
}

static int isAddrOK(struct sockaddr_in * inet_sockaddr, const char * okAddrs)
{
    static const char *token = " ,;:\t";
    char *ipaddr;
    char *p;

    if (okAddrs == NULL) return TRUE;

    ipaddr = inet_ntoa(inet_sockaddr->sin_addr);
    p = strstr(okAddrs, ipaddr);

    if (p == NULL) return FALSE;

    if (p == okAddrs)
    {
        p += strlen(ipaddr);
        return (strchr(token, *p) != NULL);
    }

    if (strchr(token, *--p) != NULL)
    {
        p += strlen(ipaddr) + 1;
        return (strchr(token, *p) != NULL);
    }

    return FALSE;
}

#ifndef NO_WSAACEPT
static int CALLBACK isAddrOKCallback(LPWSABUF  lpCallerId,
                                     LPWSABUF  dc0,
                                     LPQOS     dc1,
                                     LPQOS     dc2,
                                     LPWSABUF  dc3,
                                     LPWSABUF  dc4,
                                     GROUP     *dc5,
                                     DWORD     data)
{
    struct sockaddr_in *sockaddr = (struct sockaddr_in *) lpCallerId->buf;

    // Touch the args to avoid warnings
    dc0 = NULL; dc1 = NULL; dc2 = NULL; dc3 = NULL; dc4 = NULL; dc5 = NULL;

    if ((void *) data == NULL) return CF_ACCEPT;

    if (sockaddr->sin_family != AF_INET) return CF_ACCEPT;

    return isAddrOK(sockaddr, (const char *) data) ? CF_ACCEPT : CF_REJECT;
}
#endif

static printLastError(const char * text)
{
    LPVOID buf;

    FormatMessage( 
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM | 
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        GetLastError(),
        0,
        (LPTSTR) &buf,
        0,
        NULL 
    );
    
    fprintf(stderr, "%s: %s\n", text, (LPCTSTR) buf);
    LocalFree(buf);
}

static int acceptNamedPipe()
{
    int ipcFd = -1;

    if (! ConnectNamedPipe(hListen, &listenOverlapped))
    {
        switch (GetLastError())
        {
            case ERROR_PIPE_CONNECTED:

                // A client connected after CreateNamedPipe but
                // before ConnectNamedPipe. Its a good connection.

                break;
        
            case ERROR_IO_PENDING:

                // Wait for a connection to complete.

                while (WaitForSingleObject(listenOverlapped.hEvent, 
                                           ACCEPT_TIMEOUT) == WAIT_TIMEOUT) 
                {
                    if (shutdownPending) 
                    {
                        OS_LibShutdown();
                        return -1;
                    }            
                }

                break;

            case ERROR_PIPE_LISTENING:

                // The pipe handle is in nonblocking mode.

            case ERROR_NO_DATA:

                // The previous client closed its handle (and we failed
                // to call DisconnectNamedPipe)

            default:

                printLastError("unexpected ConnectNamedPipe() error");
        }
    }

    //
    // impersonate the client
    //
    if(bImpersonate && !ImpersonateNamedPipeClient(hListen)) {
        DisconnectNamedPipe(hListen);
    } else {
		ipcFd = Win32NewDescriptor(FD_PIPE_SYNC, (int) hListen, -1);
		if (ipcFd == -1) 
		{
			DisconnectNamedPipe(hListen);
			if (bImpersonate) RevertToSelf();
		}
	}

    return ipcFd;
}

static int acceptSocket(const char *webServerAddrs)
{
    SOCKET hSock;
    int ipcFd = -1;

    for (;;)
    {
        struct sockaddr sockaddr;
        int sockaddrLen = sizeof(sockaddr);

        for (;;)
        {
            const struct timeval timeout = {1, 0};
            fd_set readfds;

            FD_ZERO(&readfds);
            FD_SET((unsigned int) hListen, &readfds);

            if (select(0, &readfds, NULL, NULL, &timeout) == 0)
            {
                if (shutdownPending) 
                {
                    OS_LibShutdown();
                    return -1;
                }
            }
            else 
            {
                break;
            }
        }
    
#if NO_WSAACEPT
        hSock = accept((SOCKET) hListen, &sockaddr, &sockaddrLen);

        if (hSock == INVALID_SOCKET)
        {
            break;
        }

        if (isAddrOK((struct sockaddr_in *) &sockaddr, webServerAddrs))
        {
            break;
        }

        closesocket(hSock);
#else
        hSock = WSAAccept((unsigned int) hListen,                    
                          &sockaddr,  
                          &sockaddrLen,               
                          isAddrOKCallback,  
                          (DWORD) webServerAddrs);

        if (hSock != INVALID_SOCKET)
        {
            break;
        }
        
        if (WSAGetLastError() != WSAECONNREFUSED)
        {
            break;
        }
#endif
    }

    if (hSock == INVALID_SOCKET) 
    {
        /* Use FormatMessage() */
        fprintf(stderr, "accept()/WSAAccept() failed: %d", WSAGetLastError());
        return -1;
    }
    
    ipcFd = Win32NewDescriptor(FD_SOCKET_SYNC, hSock, -1);
	if (ipcFd == -1) 
    {
	    closesocket(hSock);
	}

    return ipcFd;
}

/*
 *----------------------------------------------------------------------
 *
 * OS_Accept --
 *
 *  Accepts a new FastCGI connection.  This routine knows whether
 *  we're dealing with TCP based sockets or NT Named Pipes for IPC.
 *
 *  fail_on_intr is ignored in the Win lib.
 *
 * Results:
 *      -1 if the operation fails, otherwise this is a valid IPC fd.
 *
 *----------------------------------------------------------------------
 */
int OS_Accept(int listen_sock, int fail_on_intr, const char *webServerAddrs)
{
    int ipcFd = -1;

    // Touch args to prevent warnings
    listen_sock = 0; fail_on_intr = 0;

    // @todo Muliple listen sockets and sockets other than 0 are not
    // supported due to the use of globals.

    if (shutdownPending) 
    {
        OS_LibShutdown();
        return -1;
    }

    // The mutex is to keep other processes (and threads, when supported)
    // from going into the accept cycle.  The accept cycle needs to
    // periodically break out to check the state of the shutdown flag
    // and there's no point to having more than one thread do that.
    
    if (acceptMutex != INVALID_HANDLE_VALUE) 
    {
		DWORD ret;
        while ((ret = WaitForSingleObject(acceptMutex, ACCEPT_TIMEOUT)) == WAIT_TIMEOUT) 
        {
			if (shutdownPending) break;
		}
		if (ret == WAIT_FAILED) {
            printLastError("WaitForSingleObject() failed");
            return -1;
        }
    }
    
    if (shutdownPending) 
    {
        OS_LibShutdown();
    }
    else if (listenType == FD_PIPE_SYNC) 
    {
        ipcFd = acceptNamedPipe();
    }
    else if (listenType == FD_SOCKET_SYNC)
    {
        ipcFd = acceptSocket(webServerAddrs);
    }
    else
    {
        fprintf(stderr, "unknown listenType (%d)\n", listenType);
    }
	    
    if (acceptMutex != INVALID_HANDLE_VALUE) 
    {
        ReleaseMutex(acceptMutex);
    }

    return ipcFd;
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
    if (ipcFd == -1)
        return 0;

    /*
     * Catch it if fd is a bogus value
     */
    ASSERT((ipcFd >= 0) && (ipcFd < WIN32_OPEN_MAX));
    ASSERT(fdTable[ipcFd].type != FD_UNUSED);

    switch(listenType) {

    case FD_PIPE_SYNC:
	/*
	 * Make sure that the client (ie. a Web Server in this case) has
	 * read all data from the pipe before we disconnect.
	 */
	if(!FlushFileBuffers(fdTable[ipcFd].fid.fileHandle))
	    return -1;
	if(DisconnectNamedPipe(fdTable[ipcFd].fid.fileHandle)) {
	    OS_Close(ipcFd);
		if (bImpersonate) RevertToSelf();
	    return 0;
	} else {
	    return -1;
	}
	break;

    case FD_SOCKET_SYNC:
	OS_Close(ipcFd);
        return 0;
	break;

    case FD_UNUSED:
    default:
	//exit(106);
	return -1;
	break;
    }
	return -1;
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
    // Touch args to prevent warnings
    sock = 0;

    /* XXX This is broken for sock */

	return (listenType != FD_UNUSED); 
}

/*
 *----------------------------------------------------------------------
 *
 * OS_SetFlags --
 *
 *      Sets selected flag bits in an open file descriptor.  Currently
 *      this is only to put a SOCKET into non-blocking mode.
 *
 *----------------------------------------------------------------------
 */
void OS_SetFlags(int fd, int flags)
{
    unsigned long pLong = 1L;

    if (fdTable[fd].type == FD_SOCKET_SYNC && flags == O_NONBLOCK) {
        if (ioctlsocket(fdTable[fd].fid.sock, FIONBIO, &pLong) ==
	        SOCKET_ERROR) {
	    //exit(WSAGetLastError());
			SetLastError(WSAGetLastError());
			return;
        }
        if (!CreateIoCompletionPort((HANDLE)fdTable[fd].fid.sock,
				    hIoCompPort, fd, 1)) {
	    //err = GetLastError();
	    //exit(err);
			return;
	}

        fdTable[fd].type = FD_SOCKET_ASYNC;
    }
    return;
}

