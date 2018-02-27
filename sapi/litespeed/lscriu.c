/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at the following url:        |
   | http://www.php.net/license/3_01.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: George Wang <gwang@litespeedtech.com>                        |
   +----------------------------------------------------------------------+
*/
/*
Copyright (c) 2002-2018, Lite Speed Technologies Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met: 

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer. 
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution. 
    * Neither the name of the Lite Speed Technologies Inc nor the
      names of its contributors may be used to endorse or promote
      products derived from this software without specific prior
      written permission.  

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
*/

#define HAVE_MSGHDR_MSG_CONTROL
#include "lsapilib.h"

#include <stdio.h>

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <sys/wait.h>


#include <sys/stat.h>

#if HAVE_SYS_TYPES_H

#include <sys/types.h>

#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>

#include <signal.h>
#include <time.h>
#include <sys/timeb.h>
#include <unistd.h>
#include "lscriu.h"

#define  LSCRIU_PATH    256

// Begin CRIU inclusion
//CRIU inclusion
static int s_initial_start_reqs = 0;
static int s_requests_count = 0;
static int s_restored = 0;
static int (*s_lscapi_dump_me)() = NULL;
static int (*s_lscapi_prepare_me)() = NULL;
static int s_native = 0;
static int s_tried_checkpoint = 0;
static int s_criu_debug = 0;

typedef enum 
{
    CRIU_GCOUNTER_SHM,
    CRIU_GCOUNTER_SIG,
    CRIU_GCOUNTER_PIPE
} GlobalCounterType_t;
static GlobalCounterType_t s_global_counter_type = CRIU_GCOUNTER_SHM;

#ifndef sighandler_t
typedef void (*sighandler_t)(int);
#endif

void lsapi_error( const char * pMessage, int err_no );
void LSAPI_reset_server_state( void );
int LSAPI_Get_ppid();

#ifdef LSAPILIB_DEBUG_CRIU
#define lscriu_dbg(...) \
    do { if (s_criu_debug) fprintf(stderr, __VA_ARGS__); } while(0)
#else
#define lscriu_dbg(...) 
#endif
        
#define lscriu_err(...) fprintf(stderr, __VA_ARGS__)

        
#define SUN_PATH_MAX   (sizeof(((struct sockaddr_un *)NULL)->sun_path))

typedef struct 
{
    pid_t m_iPidToDump;
    char  m_chImageDirectory[1024];
    char  m_chSocketDir[SUN_PATH_MAX];
    char  m_chServiceAddress[SUN_PATH_MAX];
} criu_native_dump_t;

typedef struct 
{
    int   m_iDumpResult;
    char  m_chDumpResponseMessage[1024];
} criu_native_dump_response_t;

typedef sem_t * (*psem_open_t) (const char *__name, int __oflag, ...);
typedef int (*psem_post_t) (sem_t *__sem);
typedef int (*psem_close_t) (sem_t *__sem);

psem_open_t psem_open = NULL;
psem_post_t psem_post = NULL;
psem_close_t psem_close = NULL;

static void lsapi_criu_signal(int signo, sighandler_t handler)
{
    struct sigaction sa;

    sigaction(signo, NULL, &sa);

    if (sa.sa_handler == SIG_DFL) {
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sa.sa_handler = handler;
        sigaction(signo, &sa, NULL);
    }
}


static void lsapi_siguser2(int sig)
{
    // child requests counter for master process
    ++s_requests_count;
}


static void LSCRIU_Set_Initial_Start_Reqs(int reqs)
{
    s_initial_start_reqs = reqs;
}


static void LSCRIU_Set_Global_Counter_Type(GlobalCounterType_t tp)
{
    if ((tp == CRIU_GCOUNTER_SHM) || (tp == CRIU_GCOUNTER_SIG)
        || (tp == CRIU_GCOUNTER_PIPE)) {
        s_global_counter_type = tp;
    } else {
        s_global_counter_type = CRIU_GCOUNTER_SHM;
    }

}



static int LSCRIU_Get_Global_Counter_Type(void)
{
    return s_global_counter_type;
}

#if 0
static int *s_shm_global_counter = NULL;


static int LSCRIU_Init_Global_Counter(int value)
{
    if (LSCRIU_Get_Global_Counter_Type() != CRIU_GCOUNTER_SHM
        || !s_initial_start_reqs) {
        return 0;
    }

    s_shm_global_counter = mmap(NULL, sizeof * s_shm_global_counter,
                                PROT_READ | PROT_WRITE, 
                                MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (!s_shm_global_counter) {
        fprintf(stderr, "LSCRIU (%d): memory for global counter allocation error\n", 
                getpid());
        return -1;
    }
    __sync_lock_test_and_set(s_shm_global_counter, value);
    lscriu_dbg("LSCRIU (%d): semaphore and shared memory created\n", getpid());
    return 0;
}


//static void LSCRIU_Destroy_Global_Counter(int only_unmap)
//{
//    if (LSCRIU_Get_Global_Counter_Type() != CRIU_GCOUNTER_SHM) {
//        return;
//    }
//
//    if (s_shm_global_counter) {
//        munmap(s_shm_global_counter, sizeof * s_shm_global_counter);
//    }
//}


static void LSCRIU_Increase_Global_Counter(void)
{
    if (LSCRIU_Get_Global_Counter_Type() != CRIU_GCOUNTER_SHM
        || !s_initial_start_reqs) {
        return;
    }

    if (s_shm_global_counter) {
        s_requests_count = __sync_add_and_fetch(s_shm_global_counter, 1);
        //lscriu_dbg("LSCRIU (%d): increase counter %d\n", getpid(), 
        //           s_requests_count);
    } else {
        s_requests_count = 0;
        LSCRIU_Set_Initial_Start_Reqs(0);
    }
}


static void LSCRIU_Get_Global_Counter(void)
{
    if (!s_initial_start_reqs) {
        return;
    }

    if (s_shm_global_counter) {
        s_requests_count = __sync_fetch_and_or(s_shm_global_counter, 0);
        //lscriu_dbg("LSCRIU (%d): get counter value %d\n", getpid(), 
        //           s_requests_count);
    } else {
        lscriu_dbg("LSCRIU (%d): Reset requests in get\n", getpid());
        s_requests_count = 0;
        LSCRIU_Set_Initial_Start_Reqs(0);
    }
}
#else


static int LSCRIU_Init_Global_Counter(int value)
{
    if (LSCRIU_Get_Global_Counter_Type() != CRIU_GCOUNTER_SHM
        || !s_initial_start_reqs) {
        return 0;
    }

    return 0;
}


static void LSCRIU_Increase_Global_Counter(void)
{
    if (LSCRIU_Get_Global_Counter_Type() != CRIU_GCOUNTER_SHM
        || !s_initial_start_reqs) {
        return;
    }

    s_requests_count = LSAPI_Inc_Req_Processed(1);
}


static void LSCRIU_Get_Global_Counter(void)
{
    if (!s_initial_start_reqs) {
        return;
    }
    s_requests_count = LSAPI_Inc_Req_Processed(0);

}
#endif


static int LSCRIU_need_checkpoint(void)
{
    if (!s_initial_start_reqs) {
        return 0;
    }

    if (LSCRIU_Get_Global_Counter_Type() == CRIU_GCOUNTER_SHM
        && s_requests_count <= s_initial_start_reqs) {
        LSCRIU_Get_Global_Counter();
    }
    if (s_initial_start_reqs > 0
        && s_requests_count >= s_initial_start_reqs) {
        return 1;
    }

    return 0;
}


static int LSCRIU_load_liblscapi(void)
{
    void *lib_handle = NULL;
    void *pthread_lib_handle = NULL;
    char ch;
    
    char *pchForceNative = NULL;
    char *pchNative = NULL;
    
    pchForceNative = getenv("LSAPI_CRIU_FORCE_NATIVE");
    pchNative = getenv("LSCAPI_CRIU_NATIVE");
    
    if (pchForceNative 
        && ((ch = *pchForceNative | 0x20) == '1' || ch == 't' 
            || (ch == 'o' && ((*(pchForceNative + 1) | 0x20) == 'n'))))
        s_native = 1;
    else if (pchNative
        && ((ch = *pchNative | 0x20) == '1' || ch == 't'
            || (ch == 'o' && ((*(pchNative + 1) | 0x20) == 'n')))) 
        s_native = 1;
    else if (!s_native)
    {
        // Numerical signals indicates Apache
        int error = 1;
        char *last;
        
        if (!(lib_handle = dlopen(last = "liblscapi.so", RTLD_LAZY)) /*||
            !(pthread_lib_handle = dlopen(last = "libpthread.so", RTLD_LAZY))*/)
            fprintf(stderr, "LSCRIU (%d): failed to dlopen %s: %s - ignore CRIU\n", 
                    getpid(), last, dlerror());
        else if (!(s_lscapi_dump_me = dlsym(lib_handle, last = "lscapi_dump_me")) ||
                 !(s_lscapi_prepare_me = dlsym(lib_handle, last = "lscapi_prepare_me")) ||
                 !(psem_open = dlsym(pthread_lib_handle, last = "sem_open")) ||
                 !(psem_post = dlsym(pthread_lib_handle, last = "sem_post")) ||
                 !(psem_close = dlsym(pthread_lib_handle, last = "sem_close")))
            fprintf(stderr, "LSCRIU (%d): failed to dlsym %s: %s - ignore CRIU\n",
                    getpid(), last, dlerror());
        else
            error = 0;
        if (error) 
        {
            // close the dll handles so we release the resources
            if (lib_handle) 
                dlclose(lib_handle);
            if (pthread_lib_handle)
                dlclose(pthread_lib_handle);
            return -1;
        }
    }
    return 0;
}


static void LSCRIU_Wink_Server_is_Ready(void)
{
    char sem_name[60];

    if (s_native) {
        // Not used for native
        return;
    }
    if (getenv("LSAPI_UNIQE")) 
        snprintf(sem_name, sizeof sem_name - 1, "lsphp[hash=%s].is_ready",
                 getenv("LSAPI_UNIQE"));
    else 
        snprintf(sem_name, sizeof sem_name - 1, "lsphp[euid=0x%x].is_ready",
                 geteuid());

    sem_t *is_ready_sem = psem_open(sem_name, O_RDWR);
    if (is_ready_sem) {
        if (psem_post(is_ready_sem) < 0)
            lsapi_error(sem_name, errno);

        if (psem_close(is_ready_sem) < 0)
            lsapi_error(sem_name, errno);
    }
    else if (errno != ENOENT) 
        lsapi_error(sem_name, errno);
    
}


/*
  To reset server state to proper initial state after restoring server
  process from CRIU image
*/
//static void reset_server_state( void );



#if defined(__FreeBSD__)
# include <sys/param.h>
#endif

#ifndef __CMSG_ALIGN
#define __CMSG_ALIGN(p) (((u_int)(p) + sizeof(int) - 1) &~(sizeof(int) - 1))
#endif

/* Length of the contents of a control message of length len */
#ifndef CMSG_LEN
#define CMSG_LEN(len)   (__CMSG_ALIGN(sizeof(struct cmsghdr)) + (len))
#endif

/* Length of the space taken up by a padded control message of
length len */
#ifndef CMSG_SPACE
#define CMSG_SPACE(len) (__CMSG_ALIGN(sizeof(struct cmsghdr)) + __CMSG_ALIGN(len))
#endif

static char *LSCRIU_Error_File_Name(char *pchFile, int max_len) 
{
    const char *pchDefaultSocketPath = "/tmp/";
    const char *pchDefaultLogFileName = "lsws_error.log";
    snprintf(pchFile, max_len, "%s%s", pchDefaultSocketPath, 
             pchDefaultLogFileName);
    return pchFile;
}
    
#ifdef LSAPILIB_DEBUG_CRIU    
static void LSCRIU_Debugging(void) {
    char *pchCRIUDebug;
    pchCRIUDebug = getenv("LSAPI_CRIU_DEBUG");
    if (!pchCRIUDebug) 
        pchCRIUDebug = getenv("LSCAPI_CRIU_DEBUG");
    //fprintf(stderr,"(%d) LSCRIU: CRIU debug environment variable: %s\n",  
    //        getpid(), pchCRIUDebug);
    // I've made it easy to turn on debugging.  CloudLinux Apache sets
    // LSCAPI_CRIU_DEBUG to nothing to indicate it's on.  Sigh.
    if ((!pchCRIUDebug) ||
        ((!*pchCRIUDebug) ||
         (*pchCRIUDebug == '0') ||
         (*pchCRIUDebug == 'f') ||
         (*pchCRIUDebug == 'F') ||
         (((*pchCRIUDebug == 'O') ||
            (*pchCRIUDebug == 'o')) &&
           ((*(pchCRIUDebug + 1)) &&
            ((*(pchCRIUDebug + 1) == 'F') || (*(pchCRIUDebug + 1) == 'f')))))) 
    {
        lscriu_dbg("LSCRIU (%d): CRIU Debugging disabled by environment\n", getpid());
        s_criu_debug = 0;
    }
    else {
        s_criu_debug = 1;
        lscriu_dbg("LSCRIU (%d): CRIU Debugging enabled by environment\n", getpid());
        fprintf(stderr,"LSCRIU (%d): CRIU debug environment variable: %s\n",  
                getpid(), pchCRIUDebug);
    }
}


static void LSCRIU_Restored_Error(int iFatal, char *format, ...) {
    // This routine deals with the awful situation of trying to get info while the stderr handle is closed on purpose.
    int iOldUMask;
    int iFd = -1;
    char chFile[1024];
    
    if (!iFatal) {
        // LSCRIU_Debugging();
        if (!s_criu_debug) {
            // Debugging message and debugging is off
            return;
        }
    }
    if (!LSCRIU_Error_File_Name(chFile, sizeof(chFile))) {
        // We're done here...nowhere to write
        return;
    }
    iOldUMask = umask(0);
    iFd = open( chFile, O_WRONLY | O_APPEND | O_CREAT, 
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    umask(iOldUMask);
    if (iFd >= 0) {
        char chFullMessage[0x1000];
        struct timeb sTimeb;
        struct tm    sTm;
        ftime(&sTimeb);
        localtime_r(&sTimeb.time,&sTm);
        va_list ap;
        va_start(ap, format);
        char buf[0x1000];
        vsnprintf(buf, sizeof(buf), format, ap);
        va_end(ap);
        
        int n = snprintf(chFullMessage, sizeof(chFullMessage),
                "%04d-%02d-%02d %02d:%02d:%02d.%03d: LSCRIU (%d): %s %s\n", 
                sTm.tm_year + 1900,
                sTm.tm_mon + 1,
                sTm.tm_mday,
                sTm.tm_hour,
                sTm.tm_min,
                sTm.tm_sec,
                sTimeb.millitm,
                getpid(), 
                iFatal ? "FATAL! " : "(debug) ", 
                buf);
        if (n > (int)sizeof(chFullMessage))
            n = sizeof(chFullMessage);
        write(iFd, chFullMessage, n);
        close(iFd);
    }
}
#else // no debugging
static void inline LSCRIU_Debugging(void) {}
static void inline LSCRIU_Restored_Error(int iFatal, char *format, ...) {}
#endif


static ssize_t read_fd(int fd, void *ptr, size_t nbytes, int *recvfd)
{
    struct msghdr   msg;
    struct iovec    iov[1];
    ssize_t         n;

#ifdef  HAVE_MSGHDR_MSG_CONTROL
    union {
      struct cmsghdr    cm;
      char              control[CMSG_SPACE(sizeof(int))];
    } control_un;
    struct cmsghdr  *cmptr;

    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);
#else
    int             newfd;
    msg.msg_accrights = (caddr_t) &newfd;
    msg.msg_accrightslen = sizeof(int);
#endif

    msg.msg_name = NULL;
    msg.msg_namelen = 0;

    iov[0].iov_base = ptr;
    iov[0].iov_len = nbytes;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    if ( (n = recvmsg(fd, &msg, 0)) <= 0)
        return(n);

#ifdef  HAVE_MSGHDR_MSG_CONTROL
    if ( (cmptr = CMSG_FIRSTHDR(&msg)) != NULL &&
        cmptr->cmsg_len == CMSG_LEN(sizeof(int))) {
        if (cmptr->cmsg_level != SOL_SOCKET) {
            LSCRIU_Restored_Error(1, "control level != SOL_SOCKET");
            return(-1);
        }
        if (cmptr->cmsg_type != SCM_RIGHTS) {
            LSCRIU_Restored_Error(1, "control type != SCM_RIGHTS");
            return(-1);
        }
        *recvfd = *((int *) CMSG_DATA(cmptr));
    } else
        *recvfd = -1;       /* descriptor was not passed */
#else
/* *INDENT-OFF* */
    if (msg.msg_accrightslen == sizeof(int))
        *recvfd = newfd;
    else
        *recvfd = -1;       /* descriptor was not passed */
/* *INDENT-ON* */
#endif

    return(n);
}


static char *CloudLinux_Socket_Name(pid_t iPidDumped,
                                    char  chFullName[],
                                    int   iMaxSize)
{
    snprintf(chFullName,iMaxSize,"/tmp/%lu.sock",iPidDumped);
    return chFullName;
}


static int LSCRIU_Receive_Handles(char  *pchSocketPath,
                                  pid_t *piPidSender,
                                  int   iDumped) 
{
    time_t iTime;
    int  iError = 0;
    time(&iTime);
    LSCRIU_Restored_Error(0, "Receive handles on %s and then Reset requests",
                          pchSocketPath);
    {
        int fdSocket = -1;
        int fdConnected = -1;
        int fdZero = -1;
        int fdTwo = -1;
        int fdSave = -1;
        struct sockaddr_un sSockaddrUn, sSockaddrUnRemote;
        int iRc, iRemoteLength;
        char chSocketPath[SUN_PATH_MAX];
        
        int path_len = strlen(pchSocketPath);
        if (path_len > (int)sizeof(chSocketPath) - 1)
            return -1;
        memmove(chSocketPath, pchSocketPath, path_len + 1);
        pchSocketPath = chSocketPath;
        
        fdSave = g_req.m_fdListen;
        LSCRIU_Restored_Error(0, "Opening domain socket defined at "
                              "LSCAPI_CRIU_SOCKET_NAME: %s, comm #%d, %s", 
                              pchSocketPath, fdSave, iDumped ? "DUMP" : "RESTORE");
        // Before listening, let's delete it, just in case
        fdSocket = socket(AF_UNIX, SOCK_STREAM, 0);
        if (fdSocket == -1) {
            LSCRIU_Restored_Error(1, "Error performing simple socket call: %s", 
                                  strerror(errno));
            iError = 1;
        }
        else {
            memset(&sSockaddrUn, 0, sizeof(sSockaddrUn));
            sSockaddrUn.sun_family = AF_UNIX;
            memmove(sSockaddrUn.sun_path, pchSocketPath, path_len);
            iRc = bind(fdSocket, (struct sockaddr *) &sSockaddrUn, 
                       sizeof(sSockaddrUn));
            if (iRc == -1) {
                LSCRIU_Restored_Error( 1, "Error binding socket %s: %s", 
                                       pchSocketPath, strerror(errno));
                iError = 1;
            }
        }
        if (!iError) {
            iRc = listen(fdSocket, 5);
            if (iRc == -1) {
                LSCRIU_Restored_Error( 1, "Error listening on socket %s: %s", 
                                       pchSocketPath, strerror(errno));
                iError = 1;
            }
        }
        if (!iError) {
            fdConnected = accept(fdSocket, (struct sockaddr *)&sSockaddrUnRemote, 
                                 (unsigned int *)&iRemoteLength);
            if (fdConnected == -1) {
                LSCRIU_Restored_Error( 1, "Error accepting on socket %s: %s", 
                                       pchSocketPath, strerror(errno));
                iError = 1;
            }
        }
        if (!iError) {
            LSCRIU_Restored_Error( 0, "Connected - now receive the file handles");          
            // Zero is only used on a restore - it's the fd_listen handle on a dump.  
            iRc = read_fd(fdConnected, piPidSender, sizeof(pid_t), &fdZero);
            if (iRc == -1) {
                LSCRIU_Restored_Error( 1, "Error reading handle %d through "
                                       "sockets: %s", 
                                       (iDumped ? fdSave : 0), strerror(errno));
                iError = 1;
            }
        }
        if (!iError) {
            LSCRIU_Restored_Error(0, "Now receive handle 2");            
            iRc = read_fd(fdConnected, piPidSender, sizeof(pid_t), &fdTwo);
            if (iRc == -1) {
                LSCRIU_Restored_Error( 1, "Error reading handle 2 through "
                                       "sockets: %s", strerror(errno));
                iError = 1;
            }
        }
        if (!iError) {
            LSCRIU_Restored_Error( 0, "Received handles, Dup and use them.  "
                                   "Received caller pid: %d, handles 1, 2 and "
                                   "fdSave=%d, fdZero=%d, fdTwo=%d",
                                   *piPidSender, fdSave, fdZero, fdTwo);
            // So we don't step on ourselves.
            if (fdSave == fdTwo) {
                dup2(fdTwo,2);
                dup2(fdZero,fdSave);
            }
            else {
                dup2(fdZero,fdSave);
                dup2(fdTwo,2);
            }
            dup2(2,1);
            // Theoretically I can now go back to normal tracing...but keep 
            // logging the other way, just in case...
            LSCRIU_Restored_Error( 0, "One final trace before writing to "
                                   "stderr\n");
            lscriu_dbg("LSCRIU (%d): Restored handles - almost ready to actually resume!\n", getpid());
            LSCRIU_Restored_Error( 0, "We just wrote to stderr");
        }
        if ((fdTwo > 3) && (fdTwo != fdSave)) { // So we don't close something we need!
            lscriu_dbg("LSCRIU (%d): Restored handles - about to close fdTwo: %d\n", 
                       getpid(), fdTwo);
            close(fdTwo);
        }
        if ((fdZero > 3) && (fdZero != fdSave)) {
            // We verify that we're not closing a socket we just dup-ed to!
            lscriu_dbg("LSCRIU (%d): Restored handles - about to close fdZero: %d\n", 
                       getpid(), fdZero);
            close(fdZero);
        }
        if ((fdConnected > 3) && (fdConnected != fdSave)) {
            lscriu_dbg("LSCRIU (%d): Restored handles - about to close fdConnected: %d\n", 
                       getpid(), fdConnected);
            close(fdConnected);
        }
        if ((fdSocket > 3) && (fdSocket != fdSave)) {
            lscriu_dbg("LSCRIU (%d): Restored handles - about to close fdSocket: %d\n", 
                       getpid(), fdSocket);
            close(fdSocket);
        }
        /* Leave the socket path file around so we can restore it again!
        lscriu_dbg("LSCRIU (%d): Closed the sockets, now delete the socket file\n", getpid());
        if (pchSocketPath) {
            if (unlink(pchSocketPath) == -1) {
                lscriu_err("LSCRIU (%d): Error deleting socket path: %s: %s\n", 
                           getpid(), pchSocketPath, strerror(errno));
            }
            else {
                lscriu_dbg("LSCRIU (%d): Deleted %s\n",  getpid(), pchSocketPath);
            }
        }
        else {
            char buf[0x1000];
            snprintf(buf, sizeof(buf) - 1, 
                     "LSCRIU (%d): Lost the LSCAPI_CRIU_SOCKET_NAME environment"
                     " variable", getpid());
            fprintf(stderr, "%s\n", buf );
            LSCRIU_Restored_Error(1, buf );            
        }
        */
    }    
    return(iError ? -1 : 0);    
}


/* This is pretty standard code taken from: 
 * http://www.cs.cmu.edu/afs/cs/academic/class/15213-f00/unpv12e/lib/write_fd.c
 * But also in UNIX Network Programming */
static ssize_t write_fd(int fd, void *ptr, size_t nbytes, int sendfd)
{
    struct msghdr   msg;
    struct iovec    iov[1];

#ifdef  HAVE_MSGHDR_MSG_CONTROL
    union {
      struct cmsghdr    cm;
      char              control[CMSG_SPACE(sizeof(int))];
    } control_un;
    struct cmsghdr  *cmptr;

    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);

    cmptr = CMSG_FIRSTHDR(&msg);
    cmptr->cmsg_len = CMSG_LEN(sizeof(int));
    cmptr->cmsg_level = SOL_SOCKET;
    cmptr->cmsg_type = SCM_RIGHTS;
    *((int *) CMSG_DATA(cmptr)) = sendfd;
#else
    msg.msg_accrights = (caddr_t) &sendfd;
    msg.msg_accrightslen = sizeof(int);
#endif

    msg.msg_name = NULL;
    msg.msg_namelen = 0;

    iov[0].iov_base = ptr;
    iov[0].iov_len = nbytes;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    return(sendmsg(fd, &msg, 0));
}


/*
int LSCRIU_Send_File_Handles(char *pchSocketName) 
{
    // Called by the child after the fork, sends the two closed handles
    struct sockaddr_un  sSockaddrUn;
    int    iFdSocket = -1;
    int    iLength;
    int    iRc;
    time_t iTimeEntry = 0;
    time_t iTimeNow;
    int    iError = 0;
    int    iConnected = 0;
    
    sSockaddrUn.sun_family = AF_UNIX;
    LSCRIU_Restored_Error(0, "Send file handles using socket file: %s",
                          pchSocketName);
    strncpy(sSockaddrUn.sun_path, pchSocketName, sizeof(sSockaddrUn.sun_path));
    iFdSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (iFdSocket == -1) {
        LSCRIU_Restored_Error( 1, "Send file handles to restored images, error "
                               "creating initial socket: %s",strerror(errno));
        return(0);
    }
    iLength = strlen(sSockaddrUn.sun_path) + sizeof(sSockaddrUn.sun_family);
    do {
        iRc = connect(iFdSocket, (struct sockaddr *)&sSockaddrUn, iLength);
        if (iRc == -1) {
            time(&iTimeNow);
            if (!iTimeEntry) {
                iTimeEntry = iTimeNow;
            }
            else if (iTimeNow - iTimeEntry > 5) {
                LSCRIU_Restored_Error( 1, "Timed out trying to connect to "
                                       "restore handles");
                iError = 1;
            }
            else {
                usleep(10); // Keep waiting
            }
        }
        else {
            LSCRIU_Restored_Error(0, "Connected to restored child to send "
                                  "handles.");
            iConnected = 1;
        }
    } while ((!iConnected) && (!iError));
    if (iConnected) {
        pid_t iPid = LSAPI_Get_ppid();
        if (!iPid) {
            iPid = getpid();
        }
        if (write_fd(iFdSocket, &iPid, sizeof(iPid), g_req.m_fdListen) == -1) {
            LSCRIU_Restored_Error( 1, "Error writing listen socket #%d to "
                                   "parent: %s", g_req.m_fdListen, 
                                   strerror(errno));
            iError = 1;
        }
        else if (write_fd(iFdSocket, &iPid, sizeof(iPid), 2) == -1) {
            LSCRIU_Restored_Error( 1, "Error writing socket #2 to parent: %s", 
                                   strerror(errno));
            iError = 1;
        }
        else {
            LSCRIU_Restored_Error(0, "Sockets sent to child - it can now perform"
                                  " the function");
        }
    }
    if (iFdSocket != -1) {
        close(iFdSocket);
    }
    return(!iError);       
}
*/

static int LSCRIU_Native_Dump(pid_t iPid,
                              char  *pchSocketName,
                              char  *pchSocketPath,
                              char  *pchImagePath,
                              int   iFdNative) {
    criu_native_dump_t criu_native_dump;
    char *pchLastSlash;
    criu_native_dump_response_t criu_native_dump_response;
        
    criu_native_dump.m_iPidToDump = iPid;
    strncpy(criu_native_dump.m_chServiceAddress, pchSocketPath, 
            sizeof(criu_native_dump.m_chServiceAddress));
    strncpy(criu_native_dump.m_chImageDirectory, pchImagePath,
            sizeof(criu_native_dump.m_chImageDirectory));
    strncpy(criu_native_dump.m_chSocketDir, pchSocketName,
            sizeof(criu_native_dump.m_chSocketDir));
    pchLastSlash = strrchr(criu_native_dump.m_chSocketDir,'/');
    if (pchLastSlash) {
        pchLastSlash++;
        (*pchLastSlash) = 0;
    }
    if (write(iFdNative,
              &criu_native_dump,
              sizeof(criu_native_dump)) == -1) {
        lscriu_err("LSCRIU (%d): Error sending dump request to the listener: %s\n", 
                   getpid(), strerror(errno));
        return(-1);
    }
    lscriu_dbg("LSCRIU (%d): Sent the dump request to the listener\n", getpid());
    //while (sleep(7200));
    if (read(iFdNative,
             &criu_native_dump_response,
             sizeof(criu_native_dump_response)) == -1) {
        // The test will actually fail it!
        //LSCRIU_Restored_Error(1, "Error reading dump socket #%d from parent: %s", 
        //                      iFdNative, strerror(errno));
        //return(-1);
    }
    return(0);       
}


static void LSCRIU_CloudLinux_Checkpoint(void)
{
    int iRet;
    
    if ((!s_native) && (!s_lscapi_dump_me)) {
        lscriu_dbg("LSCRIU (%d): Not native and unable to dump - abandon one-time "
                   "dump\n", getpid());
        return;
    }
    // NOTE - NOTE - NOTE
    // In the CloudLinux implementation they do not do a prepare_me.  Why, I
    // don't know, so I won't either!
    //iRet = s_lscapi_prepare_me();
    //if (iRet) {
        //LSCRIU_Restored_Error(1, "lscapi_prepare_me failed: %s",
        //                      strerror(errno));
        //lscriu_err("LSCRIU (%d): failed to lscapi_prepare_me: %s\n", 
        //           getpid(), strerror(errno));
        //But keep going anyway!  Not fatal yet
    //}
    iRet = s_lscapi_dump_me();
    if (iRet < 0) {
        char *pchError;
        lscriu_err("LSCRIU: CloudLinux dump of PID: %d, error: %s\n", 
                   getpid(), strerror(errno));
    }
    if (iRet == 0) {
        // Dumped.  To continue the child must send us the handles back
        lscriu_err("LSCRIU: Successful CloudLinux dump of PID: %d\n", getpid());
    }
    else {
        s_restored = 1;
        LSAPI_reset_server_state();
        /*
         Here we have restored the php process, so we should to tell (via 
         semaphore) mod_lsapi that we are started and ready to receive data.
        */
        LSCRIU_Wink_Server_is_Ready();
        lscriu_err("LSCRIU: Successful CloudLinux restore of PID: %d, parent: %d.\n", 
                   getpid(), getppid());
    }
    LSCRIU_Set_Initial_Start_Reqs(0);
}


static void LSCRIU_try_checkpoint(void)
{
    int iRet;
    pid_t iPid;
    pid_t iPidDump = getpid();
    int   iFdNative;
    char *pchImagePath;
    char *pchSocketPath;
    char *pchSocketName;
    
    if (s_tried_checkpoint) {
        lscriu_dbg("LSCRIU (%d): Already tried checkpoint - one time per customer\n",
                   getpid());
        return;
    }
    lscriu_dbg("LSCRIU (%d): Trying checkpoint\n", getpid());
    s_tried_checkpoint = 1;
    if (!s_native) {
        LSCRIU_CloudLinux_Checkpoint();
        return;
    }
    char *pchFd;
    pchSocketName = getenv("LSCAPI_CRIU_SOCKET_NAME");

    if (!pchSocketName) {
        lscriu_err("LSCRIU (%d): LSCAPI_CRIU_SOCKET_NAME internal environment "
                   "variable not set - contact Litespeed tech support\n", getpid());
        return;
    }
    unlink(pchSocketName);
    pchFd = getenv("LSCAPI_CRIU_SYNC_FD");
    pchImagePath = getenv("LSCAPI_CRIU_IMAGE_PATH");
    pchSocketPath = getenv("LSAPI_CRIU_SOCKET_PATH");                
    if (!pchSocketPath) {
        pchSocketPath = getenv("LSCAPI_CRIU_SOCKET_PATH");
    }
    if (!pchFd) {
        lscriu_err("LSCRIU (%d): LSCAPI_CRIU_SYNC_FD internal environment "
                   "variable not set - contact Litespeed tech support\n", getpid());
        return;
    }        
    if (!pchImagePath) {
        lscriu_err("LSCRIU (%d): LSCAPI_CRIU_IMAGE_PATH internal environment "
                   "variable not set - contact Litespeed tech support\n", getpid());
        return;
    }
    if (!pchSocketPath) {
        lscriu_err("LSCRIU (%d): LSAPI_CRIU_SOCKET_PATH internal environment "
                   "variable not set - contact Litespeed tech support\n", getpid());
        return;
    }
    lscriu_dbg("LSCRIU (%d): Checkpoint dump.  ImagePath: %s, SocketName %s, "
               "SocketPath: %s\n", getpid(), pchImagePath, pchSocketName, pchSocketPath);
        
    iFdNative = atoi(pchFd);
    lscriu_dbg("LSCRIU (%d): Native checkpoint.  Use filepointer %d (%s) to send "
               "pid %d\n", getpid(), iFdNative, pchFd, iPidDump);
    
    // Handle 0 is the connected comm handle, handle 1 and 2 are the output data 
    // streams.  We can't do a dump with 1 or 2 open.  And there's no reason to 
    // dump 0 since it will change.  So on George's advice, we'll fork here, let 
    // the child close the handles and do the dump.  
    // On restore, the recovered child will create a domain socket, and the 
    // parent will send it the handles it can use to continue.
    
    lscriu_dbg("LSCRIU (%d): fork!\n", getpid());
    iPid = fork();
    if (iPid < 0) {
        lscriu_err("LSCRIU (%d): Can't checkpoint due to a fork error: %s\n", 
                   getpid(), strerror(errno));
        return;
    }
    if (iPid > 0) {
        // We're the parent.  Close the handles, do the dump and receive them 
        // back from the child or the parent
        //close(0);
        // Let the child do the dump
        usleep(1); // Get the child to run and close everything so we can dump
        pid_t   iPidParent = getppid();
        iRet = LSCRIU_Native_Dump(iPid,
                                  pchSocketName,
                                  pchSocketPath,
                                  pchImagePath,
                                  iFdNative);
        if (iRet == 0) {
            if (iPidParent != getppid()) 
                lscriu_dbg("LSCRIU (%d): PID %d dumped to disk; PID %d killed in "
                           "prior message - is this the restore?\n", getpid(), 
                           iPidDump, iPid);
            else 
                lscriu_err("LSCRIU (%d): PID %d dumped to disk and killed in "
                           "prior message\n", getpid(), iPid);
        }
        else 
            lscriu_err("LSCRIU (%d): Native dump of PID: %d FAILED\n", getpid(),
                       iPid);
        LSCRIU_Restored_Error(1, "Restored child process - this is unexpected!");
    }
    else {
        int     iResult;
        pid_t   iPidSender;
        pid_t   iPidParent = getppid();
        
        setsid();
        close(1);
        close(2);
        close(g_req.m_fdListen); // If we don't do this we're dead...
        close(iFdNative);
        // Now get restored.  We know if we're restored if the ppid changes!
        // If we're dumped, we're killed (no use worrying about that!).
        {
            time_t  iTimeStart = 0;
            time_t  iTimeNow;
            int     iRestored = 0;
            do {
                usleep(10);
                time(&iTimeNow);
                if (!iTimeStart) {
                    iTimeStart = iTimeNow;
                }
                else if ((iPidParent != getppid()) || 
                         (iTimeNow - iTimeStart > 10)) {
                    iRestored = 1;
                }
                else if (iTimeNow - iTimeStart > 5) {
                    LSCRIU_Restored_Error(1, "Timed out waiting to be dumped");
                    exit(1);
                }
            } while (!iRestored);
        }
        LSCRIU_Restored_Error(0, "Restored!");
        iRet = LSCRIU_Receive_Handles(pchSocketName, &iPidSender, 0);
        if (iRet == 0) {
            //lscriu_dbg("LSCRIU (%d): Fully restored, parent: %d.\n", getpid(), iPidSender);
            LSAPI_Set_Restored_Parent_Pid(iPidSender);
            lscriu_err("LSCRIU: Fully restored, pid: %d, parent: %d.\n", 
                       getpid(), iPidSender);
            LSAPI_reset_server_state();
            s_restored = 1;
            /*
            Here we have restored the php process, so we should to
            tell(via semaphore) mod_lsapi that we are started and ready
            to receive data
            */
            // Not used for native!
            //LSCRIU_Wink_Server_is_Ready();
        }
        else {
            LSCRIU_Restored_Error(1, "Restore error - terminating");
            exit(-1);
        }
    }
    LSCRIU_Set_Initial_Start_Reqs(0);
}


static int LSCRIU_Init_Env_Parameters(void)
{
    const char *p;
    int n;

    // LSAPI_CRIU is not passed down in Apache.  So we won't require it
    //p = getenv("LSAPI_CRIU");
    //if ((p) &&
    //    (*p) &&
    //    ((*p == '1') ||
    //     (*p == 'T') ||
    //     (*p == 't') ||
    //     (*p == 'Y') ||
    //     (*p == 'y') ||
    //     (((*p == 'O') || 
    //       (*p == 'o')) &&
    //      (((*(p + 1)) == 'N') ||
    //       ((*(p + 1)) == 'n'))))) {
    //    lscriu_dbg("LSCRIU (%d): Enabled by environment.\n", getpid());
    //    // We're enabled!
    //}
    //else {
    //    lscriu_dbg("LSCRIU (%d): Disabled by environment.\n", getpid());
    //    return 0;
    //}
    p = getenv("LSAPI_INITIAL_START");
    if (!p) 
        p = getenv("LSAPI_BACKEND_INITIAL_START");
    if (p)
    {
        n = atoi(p);

        if (n > 0) 
        {
            lscriu_dbg("LSCRIU (%d): Set start requests based on environment (%d)\n", 
                       getpid(), n);
            LSCRIU_Set_Initial_Start_Reqs(n);
        }
        else 
        {
            lscriu_dbg("LSCRIU (%d): LSAPI_INITIAL_START set to 0 disabled\n", 
                       getpid());
            return 0;
        }
    }
    else 
    {
        lscriu_dbg("LSCRIU (%d): LSAPI_INITIAL_START NOT set - disabled\n", 
                   getpid());
        return 0;
    }
    if (LSAPI_Is_Listen()) 
    {
        lscriu_dbg("LSCRIU (%d): Listening...\n", getpid());
        GlobalCounterType_t gc_type = CRIU_GCOUNTER_SHM;
        char *env;
        if ((env = getenv("LSAPI_CRIU_USE_SHM")))
        {
            // CloudLinux doc: Off (shared memory) or Signals.  
            // Litespeed doc: On (shared memory) or Signals
            // So just check the first character for an 'S' and if not, then 
            // use shared memory.  Pipe support is lost (sigh).
            if ((*env == 'S') || (*env == 's')) 
                gc_type = CRIU_GCOUNTER_SIG; // Just assume the rest is signals
            // else use the default of shared memory
        }
        else if ((env = getenv("LSAPI_SIGNALS")))
        {
            if ((*env == '1') ||
                (*env == 'Y') ||
                (*env == 'y') ||
                (*env == 'T') ||
                (*env == 't') ||
                (((*env == 'O') || (*env == 'o')) &&
                 ((*(env + 1) == 'N') || (*(env + 1) == 'n'))))
                gc_type = CRIU_GCOUNTER_SIG;
            else if (*env == 2) 
                gc_type = CRIU_GCOUNTER_PIPE; // The only case for pipe
            //else use the default of shared memory
        }
        if (gc_type != CRIU_GCOUNTER_SHM) 
        {
            lscriu_dbg("LSCRIU (%d): Use %s\n", getpid(),
                       gc_type == CRIU_GCOUNTER_SIG ? "signals" : "pipe");
            lsapi_criu_signal(SIGUSR2, lsapi_siguser2);
        }
        else
            lscriu_dbg("LSCRIU (%d): Use shared memory\n", getpid());
        LSCRIU_Set_Global_Counter_Type(gc_type);
    }
    else 
        lscriu_dbg("LSCRIU (%d): NOT Listening\n", getpid());
    //unset_lsapi_envs();
    return 0;
}


void LSCRIU_inc_req_procssed()
{
    if (!LSCRIU_Get_Global_Counter_Type()) {
        ++s_requests_count;
    }

    lscriu_dbg("LSCRIU (%d): s_requests_count %d counter %d\n", getpid(), 
               s_requests_count, s_initial_start_reqs);

    if (s_initial_start_reqs > 0 && s_requests_count <= s_initial_start_reqs) {
        if (LSCRIU_Get_Global_Counter_Type() == CRIU_GCOUNTER_SHM) {
            LSCRIU_Increase_Global_Counter();
            if (s_requests_count >= s_initial_start_reqs) {
                //Maybe this time we can stop to send signal and wait for
                //1 second of select timeout
                //kill( LSCRIU_Get_ppid(), SIGUSR2 );
                lscriu_dbg("LSCRIU (%d): Time to dump main process with semaphore\n",
                           getpid());
            }
        } else {
            kill(LSAPI_Get_ppid(), SIGUSR2);
            lscriu_dbg("LSCRIU (%d): Send kill to main process with signals\n",
                       getpid());
        }
    }
}


static void LSCRIU_on_timer(void)
{
    lscriu_dbg("LSCRIU (%d): LSCRIU_on_timer\n", getpid());
    if (LSCRIU_need_checkpoint()) {
        LSCRIU_try_checkpoint();
    }
}


int LSCRIU_Init(void)
{
    LSCRIU_Debugging();
    LSCRIU_Init_Env_Parameters();
    if (s_initial_start_reqs) {
        if (LSCRIU_load_liblscapi() == -1)
            s_initial_start_reqs = 0;
    }
    if (s_initial_start_reqs) {
        LSCRIU_Wink_Server_is_Ready();
        lscriu_dbg("LSCRIU (%d): LSAPI_Register_Pgrp_Timer_Callback\n", getpid());
        LSAPI_Register_Pgrp_Timer_Callback(LSCRIU_on_timer);
        LSCRIU_Init_Global_Counter(0);
    }
    return s_initial_start_reqs > 0;
}

