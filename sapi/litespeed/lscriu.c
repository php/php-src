/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
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
#include <stdlib.h>

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

#include <Zend/zend_portability.h>

#define  LSCRIU_PATH    256

// Begin CRIU inclusion
//CRIU inclusion
static int s_initial_start_reqs = 0;
static int s_requests_count = 0;
static int s_restored = 0;
static int (*s_lscapi_dump_me)(void) = NULL;
static int (*s_lscapi_prepare_me)(void) = NULL;
static int s_native = 0;
static int s_tried_checkpoint = 0;
static int s_criu_debug = 0;
static int s_fd_native = -1;
static char *s_criu_image_path = NULL;
static int s_pid = 0;

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

void lsapi_perror( const char * pMessage, int err_no );
void LSAPI_reset_server_state( void );
int LSAPI_Get_ppid(void);

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

    if (s_native)
        return 0;
    // Numerical signals indicates Apache
    int error = 1;
    char *last;

    if (!(lib_handle = DL_LOAD(last = "liblscapi.so")) /*||
        !(pthread_lib_handle = DL_LOAD(last = "libpthread.so"))*/)
        fprintf(stderr, "LSCRIU (%d): failed to dlopen %s: %s - ignore CRIU\n",
                s_pid, last, dlerror());
    else if (!(s_lscapi_dump_me = dlsym(lib_handle, last = "lscapi_dump_me")) ||
                !(s_lscapi_prepare_me = dlsym(lib_handle, last = "lscapi_prepare_me")) ||
                !(psem_open = dlsym(pthread_lib_handle, last = "sem_open")) ||
                !(psem_post = dlsym(pthread_lib_handle, last = "sem_post")) ||
                !(psem_close = dlsym(pthread_lib_handle, last = "sem_close")))
        fprintf(stderr, "LSCRIU (%d): failed to dlsym %s: %s - ignore CRIU\n",
                s_pid, last, dlerror());
    else
        error = 0;
    if (error) {
        // close the dll handles so we release the resources
        if (lib_handle)
            dlclose(lib_handle);
        if (pthread_lib_handle)
            dlclose(pthread_lib_handle);
        return -1;
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
            lsapi_perror(sem_name, errno);

        if (psem_close(is_ready_sem) < 0)
            lsapi_perror(sem_name, errno);
    }
    else if (errno != ENOENT)
        lsapi_perror(sem_name, errno);
}


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
    //        s_pid, pchCRIUDebug);
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
        lscriu_dbg("LSCRIU (%d): CRIU Debugging disabled by environment\n", s_pid);
        s_criu_debug = 0;
    }
    else {
        s_criu_debug = 1;
        lscriu_dbg("LSCRIU (%d): CRIU Debugging enabled by environment\n", s_pid);
        fprintf(stderr,"LSCRIU (%d): CRIU debug environment variable: %s\n",
                s_pid, pchCRIUDebug);
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
                s_pid,
                iFatal ? "FATAL! " : "(debug) ",
                buf);
        if (n > (int)sizeof(chFullMessage))
            n = sizeof(chFullMessage);
        write(iFd, chFullMessage, n);
        close(iFd);
    }
}
#else // no debugging
static inline void LSCRIU_Debugging(void) {}
static inline void LSCRIU_Restored_Error(int iFatal, char *format, ...) {}
#endif


static int LSCRIU_Native_Dump(pid_t iPid,
                              char  *pchImagePath,
                              int   iFdNative) {
    criu_native_dump_t criu_native_dump;
    char *pchLastSlash;

    memset(&criu_native_dump, 0, sizeof(criu_native_dump));
    criu_native_dump.m_iPidToDump = iPid;
    strncpy(criu_native_dump.m_chImageDirectory, pchImagePath,
            sizeof(criu_native_dump.m_chImageDirectory));
    pchLastSlash = strrchr(criu_native_dump.m_chSocketDir,'/');
    if (pchLastSlash) {
        pchLastSlash++;
        (*pchLastSlash) = 0;
    }
    lscriu_dbg("LSCRIU (%d): Sent the dump request to the listener\n", s_pid);
    if (write(iFdNative,
              &criu_native_dump,
              sizeof(criu_native_dump)) == -1) {
        lscriu_err("LSCRIU (%d): Error sending dump request to the listener: %s\n",
                   s_pid, strerror(errno));
        return(-1);
    }
    return 0;
}


static void LSCRIU_CloudLinux_Checkpoint(void)
{
    int iRet;

    if ((!s_native) && (!s_lscapi_dump_me)) {
        lscriu_dbg("LSCRIU (%d): Not native and unable to dump - abandon one-time "
                   "dump\n", s_pid);
        return;
    }

    iRet = s_lscapi_dump_me();
    if (iRet < 0) {
        lscriu_err("LSCRIU: CloudLinux dump of PID: %d, error: %s\n",
                   s_pid, strerror(errno));
    }
    if (iRet == 0) {
        // Dumped.  To continue the child must send us the handles back
        lscriu_err("LSCRIU: Successful CloudLinux dump of PID: %d\n", s_pid);
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


static void LSCRIU_Wait_Dump_Finish_Or_Restored(int pid_parent)
{
    // Now get restored.  We know if we're restored if the ppid changes!
    // If we're dumped, we're killed (no use worrying about that!).
    time_t  iTimeStart = 0;
    time_t  iTimeNow;
    int     iRestored = 0;
    do {
        usleep(1000);
        time(&iTimeNow);
        if (!iTimeStart) {
            iTimeStart = iTimeNow;
        }
        else if ((pid_parent != getppid()) ||
                    (iTimeNow - iTimeStart > 10)) {
            iRestored = 1;
        }
        else if (iTimeNow - iTimeStart > 5) {
            LSCRIU_Restored_Error(1, "Timed out waiting to be dumped");
            exit(1);
        }
    } while (!iRestored);
}


static void LSCRIU_try_checkpoint(int *forked_pid)
{
    int iRet;
    pid_t iPid;
    pid_t iPidDump = getpid();

    if (s_tried_checkpoint) {
        lscriu_dbg("LSCRIU (%d): Already tried checkpoint - one time per customer\n",
                   iPidDump);
        return;
    }
    lscriu_dbg("LSCRIU (%d): Trying checkpoint\n", iPidDump);
    s_tried_checkpoint = 1;
    if (!s_native) {
        LSCRIU_CloudLinux_Checkpoint();
        return;
    }

    lscriu_dbg("LSCRIU (%d): fork!\n", iPidDump);
    iPid = fork();
    if (iPid < 0) {
        lscriu_err("LSCRIU (%d): Can't checkpoint due to a fork error: %s\n",
                   iPidDump, strerror(errno));
        return;
    }
    if (iPid == 0) {
        pid_t   iPidParent = getppid();

        setsid();
        iRet = LSCRIU_Native_Dump(iPidDump,
                                  s_criu_image_path,
                                  s_fd_native);
        close(s_fd_native);

        LSCRIU_Wait_Dump_Finish_Or_Restored(iPidParent);
        LSCRIU_Restored_Error(0, "Restored!");
        LSAPI_reset_server_state();
        s_restored = 1;
    }
    else {
        if (forked_pid)
            *forked_pid = iPid;
    }
    LSCRIU_Set_Initial_Start_Reqs(0);
}


static int init_native_env(void)
{
    char *pchFd;
    pchFd = getenv("LSAPI_CRIU_SYNC_FD");
    if (!pchFd)
        pchFd = getenv("LSCAPI_CRIU_SYNC_FD");

    const char *image_path;
    image_path = getenv("LSAPI_CRIU_IMAGE_PATH");
    if (!pchFd) {
        lscriu_err("LSCRIU (%d): LSAPI_CRIU_SYNC_FD internal environment "
                   "variable not set - contact Litespeed tech support\n", getpid());
        return -1;
    }
    if (!image_path) {
        lscriu_err("LSCRIU (%d): LSAPI_CRIU_IMAGE_PATH internal environment "
                   "variable not set - contact Litespeed tech support\n", getpid());
        return -1;
    }
    lscriu_dbg("LSCRIU (%d): Checkpoint dump.  ImagePath: %s\n",
               getpid(), image_path);

    s_fd_native = atoi(pchFd);
    lscriu_dbg("LSCRIU (%d): Native checkpoint.  Use filepointer %d (%s) to send "
               "pid %d\n", getpid(), s_fd_native, pchFd, iPidDump);
    s_criu_image_path  = strdup(image_path);
    return 0;
}

static int LSCRIU_Init_Env_Parameters(void)
{
    const char *p;
    int n;

    p = getenv("LSAPI_INITIAL_START");
    if (!p)
        p = getenv("LSAPI_BACKEND_INITIAL_START");
    if (p) {
        n = atoi(p);

        if (n > 0) {
            lscriu_dbg("LSCRIU (%d): Set start requests based on environment (%d)\n",
                       getpid(), n);
            LSCRIU_Set_Initial_Start_Reqs(n);
        } else {
            lscriu_dbg("LSCRIU (%d): LSAPI_INITIAL_START set to 0 disabled\n",
                       getpid());
            return 0;
        }
    } else {
        lscriu_dbg("LSCRIU (%d): LSAPI_INITIAL_START NOT set - disabled\n",
                   getpid());
        return 0;
    }
    if (LSAPI_Is_Listen()) {
        lscriu_dbg("LSCRIU (%d): Listening...\n", getpid());
        GlobalCounterType_t gc_type = CRIU_GCOUNTER_SHM;
        char *env;
        if ((env = getenv("LSAPI_CRIU_USE_SHM"))) {
            // CloudLinux doc: Off (shared memory) or Signals.
            // Litespeed doc: On (shared memory) or Signals
            // So just check the first character for an 'S' and if not, then
            // use shared memory.  Pipe support is lost (sigh).
            if ((*env == 'S') || (*env == 's'))
                gc_type = CRIU_GCOUNTER_SIG; // Just assume the rest is signals
            // else use the default of shared memory
        }
        else if ((env = getenv("LSAPI_SIGNALS"))) {
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
        if (gc_type != CRIU_GCOUNTER_SHM) {
            lscriu_dbg("LSCRIU (%d): Use %s\n", getpid(),
                       gc_type == CRIU_GCOUNTER_SIG ? "signals" : "pipe");
            lsapi_criu_signal(SIGUSR2, lsapi_siguser2);
        }
        else {
            lscriu_dbg("LSCRIU (%d): Use shared memory\n", getpid());
	}
    	LSCRIU_Set_Global_Counter_Type(gc_type);
    }
    else {
        lscriu_dbg("LSCRIU (%d): NOT Listening\n", getpid());
    }

    char *criu_mode = NULL;
    criu_mode = getenv("LSAPI_CRIU");
    // 0 disabled
    // 1 cloudlinux
    // 2 native
    if (criu_mode) {
        if (*criu_mode == '0') {
             lscriu_dbg("LSCRIU (%d): Disabled by environment.\n", getpid());
             LSCRIU_Set_Initial_Start_Reqs(0);
        }
        else if (*criu_mode == '2') {
            lscriu_dbg("LSCRIU (%d): Disabled by environment.\n", getpid());
            s_native = 1;
        }
    }

    if (s_native && init_native_env() == -1)
        LSCRIU_Set_Initial_Start_Reqs(0);

    //unset_lsapi_envs();
    return 0;
}


void LSCRIU_inc_req_processed()
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


static void LSCRIU_on_timer(int *forked_pid)
{
    lscriu_dbg("LSCRIU (%d): LSCRIU_on_timer\n", getpid());
    if (LSCRIU_need_checkpoint()) {
        LSCRIU_try_checkpoint(forked_pid);
    }
}


int LSCRIU_Init(void)
{
    s_pid = getpid();
    LSCRIU_Debugging();
    LSCRIU_Init_Env_Parameters();
    if (s_initial_start_reqs && !s_native) {
        if (LSCRIU_load_liblscapi() == -1)
            s_initial_start_reqs = 0;
    }
    if (s_initial_start_reqs) {
        LSCRIU_Wink_Server_is_Ready();
        lscriu_dbg("LSCRIU (%d): LSAPI_Register_Pgrp_Timer_Callback\n", s_pid);
        LSAPI_Register_Pgrp_Timer_Callback(LSCRIU_on_timer);
        LSCRIU_Init_Global_Counter(0);
    }
    return s_initial_start_reqs > 0;
}
