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


#include <ctype.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/stat.h>
#include <sched.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/un.h>
#include <php_config.h>

#include "lsapilib.h"

#ifdef HAVE_PRCTL
#include <sys/prctl.h>
#endif

#if defined(__FreeBSD__ ) || defined(__NetBSD__) || defined(__OpenBSD__) \
    || defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
#include <sys/sysctl.h>
#endif

#include <inttypes.h>
#ifndef uint32
#define uint32 uint32_t
#endif

#include <Zend/zend_portability.h>

struct lsapi_MD5Context {
    uint32 buf[4];
    uint32 bits[2];
    unsigned char in[64];
};

void lsapi_MD5Init(struct lsapi_MD5Context *context);
void lsapi_MD5Update(struct lsapi_MD5Context *context, unsigned char const *buf,
           unsigned len);
void lsapi_MD5Final(unsigned char digest[16], struct lsapi_MD5Context *context);

/*
 * This is needed to make RSAREF happy on some MS-DOS compilers.
 */
typedef struct lsapi_MD5Context lsapi_MD5_CTX;


#define LSAPI_ST_REQ_HEADER     1
#define LSAPI_ST_REQ_BODY       2
#define LSAPI_ST_RESP_HEADER    4
#define LSAPI_ST_RESP_BODY      8
#define LSAPI_ST_BACKGROUND     16

#define LSAPI_RESP_BUF_SIZE     8192
#define LSAPI_INIT_RESP_HEADER_LEN 4096

enum
{
    LSAPI_STATE_IDLE,
    LSAPI_STATE_CONNECTED,
    LSAPI_STATE_ACCEPTING,
};

typedef struct lsapi_child_status
{
    int     m_pid;
    long    m_tmStart;

    volatile short   m_iKillSent;
    volatile char    m_inProcess;
    volatile char    m_state;
    volatile int     m_iReqCounter;

    volatile long    m_tmWaitBegin;
    volatile long    m_tmReqBegin;
    volatile long    m_tmLastCheckPoint;
}
lsapi_child_status;

static lsapi_child_status * s_worker_status = NULL;

static int g_inited = 0;
static int g_running = 1;
static int s_ppid;
static int s_restored_ppid = 0;
static int s_pid = 0;
static int s_slow_req_msecs = 0;
static int s_keep_listener = 1;
static int s_dump_debug_info = 0;
static int s_pid_dump_debug_info = 0;
static int s_req_processed = 0;
static int s_skip_write = 0;
static int (*pthread_atfork_func)(void (*prepare)(void), void (*parent)(void),
                                  void (*child)(void)) = NULL;

static int *s_busy_workers = NULL;
static int *s_accepting_workers = NULL;
static int *s_global_counter = &s_req_processed;
static int s_max_busy_workers = -1;
static char *s_stderr_log_path = NULL;
static int s_stderr_is_pipe = 0;
static int s_ignore_pid = -1;
static size_t s_total_pages = 1;
static size_t s_min_avail_pages = 256 * 1024;
static size_t *s_avail_pages = &s_total_pages;

LSAPI_Request g_req =
{ .m_fdListen = -1, .m_fd = -1 };

static char         s_secret[24];

static LSAPI_On_Timer_pf s_proc_group_timer_cb = NULL;

void Flush_RespBuf_r( LSAPI_Request * pReq );
static int lsapi_reopen_stderr(const char *p);

static const char *CGI_HEADERS[H_TRANSFER_ENCODING+1] =
{
    "HTTP_ACCEPT", "HTTP_ACCEPT_CHARSET",
    "HTTP_ACCEPT_ENCODING",
    "HTTP_ACCEPT_LANGUAGE", "HTTP_AUTHORIZATION",
    "HTTP_CONNECTION", "CONTENT_TYPE",
    "CONTENT_LENGTH", "HTTP_COOKIE", "HTTP_COOKIE2",
    "HTTP_HOST", "HTTP_PRAGMA",
    "HTTP_REFERER", "HTTP_USER_AGENT",
    "HTTP_CACHE_CONTROL",
    "HTTP_IF_MODIFIED_SINCE", "HTTP_IF_MATCH",
    "HTTP_IF_NONE_MATCH",
    "HTTP_IF_RANGE",
    "HTTP_IF_UNMODIFIED_SINCE",
    "HTTP_KEEP_ALIVE",
    "HTTP_RANGE",
    "HTTP_X_FORWARDED_FOR",
    "HTTP_VIA",
    "HTTP_TRANSFER_ENCODING"
};

static int CGI_HEADER_LEN[H_TRANSFER_ENCODING+1] =
{    11, 19, 20, 20, 18, 15, 12, 14, 11, 12, 9, 11, 12, 15, 18,
     22, 13, 18, 13, 24, 15, 10, 20, 8, 22 };


static const char *HTTP_HEADERS[H_TRANSFER_ENCODING+1] =
{
    "Accept", "Accept-Charset",
    "Accept-Encoding",
    "Accept-Language", "Authorization",
    "Connection", "Content-Type",
    "Content-Length", "Cookie", "Cookie2",
    "Host", "Pragma",
    "Referer", "User-Agent",
    "Cache-Control",
    "If-Modified-Since", "If-Match",
    "If-None-Match",
    "If-Range",
    "If-Unmodified-Since",
    "Keep-Alive",
    "Range",
    "X-Forwarded-For",
    "Via",
    "Transfer-Encoding"
};

static int HTTP_HEADER_LEN[H_TRANSFER_ENCODING+1] =
{   6, 14, 15, 15, 13, 10, 12, 14, 6, 7, 4, 6, 7, 10, //user-agent
    13,17, 8, 13, 8, 19, 10, 5, 15, 3, 17
};


static const char *s_log_level_names[8] =
{
    "", "DEBUG","INFO", "NOTICE", "WARN", "ERROR", "CRIT", "FATAL"
};


void LSAPI_Log(int flag, const char * fmt, ...)
{
    char buf[1024];
    char *p = buf;
    if ((flag & LSAPI_LOG_TIMESTAMP_BITS)
        && !(s_stderr_is_pipe))
    {
        struct timeval  tv;
        struct tm       tm;
        gettimeofday(&tv, NULL);
        localtime_r(&tv.tv_sec, &tm);
        if (flag & LSAPI_LOG_TIMESTAMP_FULL)
        {
            p += snprintf(p, 1024, "%04d-%02d-%02d %02d:%02d:%02d.%06d ",
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                tm.tm_hour, tm.tm_min, tm.tm_sec, (int)tv.tv_usec);
        }
        else if (flag & LSAPI_LOG_TIMESTAMP_HMS)
        {
            p += snprintf(p, 1024, "%02d:%02d:%02d ",
                tm.tm_hour, tm.tm_min, tm.tm_sec);
        }
    }

    int level = flag & LSAPI_LOG_LEVEL_BITS;
    if (level && level <= LSAPI_LOG_FLAG_FATAL)
    {
        p += snprintf(p, 100, "[%s] ", s_log_level_names[level]);
    }

    if (flag & LSAPI_LOG_PID)
    {
        p += snprintf(p, 100, "[UID:%d][%d] ", getuid(), s_pid);
    }

    if (p > buf)
        fprintf(stderr, "%.*s", (int)(p - buf), buf);
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}

#ifdef LSAPI_DEBUG

#define DBGLOG_FLAG (LSAPI_LOG_TIMESTAMP_FULL|LSAPI_LOG_FLAG_DEBUG|LSAPI_LOG_PID)
#define lsapi_dbg(...)   LSAPI_Log(DBGLOG_FLAG, __VA_ARGS__)

#else

#define lsapi_dbg(...)

#endif

#define lsapi_log(...)  LSAPI_Log(LSAPI_LOG_TIMESTAMP_FULL|LSAPI_LOG_PID, __VA_ARGS__)


void lsapi_perror(const char * pMessage, int err_no)
{
    lsapi_log("%s, errno: %d (%s)\n", pMessage, err_no, strerror(err_no));
}


static int lsapi_parent_dead(void)
{
    // Return non-zero if the parent is dead.  0 if still alive.
    if (!s_ppid) {
        // not checking, so not dead
        return(0);
    }
    if (s_restored_ppid) {
        if (kill(s_restored_ppid,0) == -1) {
            if (errno == EPERM) {
                return(0); // no permission, but it's still there.
            }
            return(1); // Dead
        }
        return(0); // it worked, so it's not dead
    }
    return(s_ppid != getppid());
}


static void lsapi_sigpipe( int sig )
{
}


static void lsapi_siguser1( int sig )
{
    g_running = 0;
}

#ifndef sighandler_t
typedef void (*sighandler_t)(int);
#endif

static void lsapi_signal(int signo, sighandler_t handler)
{
    struct sigaction sa;

    sigaction(signo, NULL, &sa);

    if (sa.sa_handler == SIG_DFL)
    {
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sa.sa_handler = handler;
        sigaction(signo, &sa, NULL);
    }
}


static int s_enable_core_dump = 0;
static void lsapi_enable_core_dump(void)
{
#if defined(__FreeBSD__ ) || defined(__NetBSD__) || defined(__OpenBSD__) \
    || defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
    int  mib[2];
    size_t len;

#if !defined(__OpenBSD__)
    len = 2;
    if ( sysctlnametomib("kern.sugid_coredump", mib, &len) == 0 )
    {
        len = sizeof(s_enable_core_dump);
        if (sysctl(mib, 2, NULL, 0, &s_enable_core_dump, len) == -1)
            perror( "sysctl: Failed to set 'kern.sugid_coredump', "
                    "core dump may not be available!");
    }
#else
    int set = 3;
    len = sizeof(set);
    mib[0] = CTL_KERN;
    mib[1] = KERN_NOSUIDCOREDUMP;
    if (sysctl(mib, 2, NULL, 0, &set, len) == 0) {
	    s_enable_core_dump = 1;
    }
#endif


#endif

#ifdef HAVE_PRCTL
    if (prctl(PR_SET_DUMPABLE, s_enable_core_dump,0,0,0) == -1)
        perror( "prctl: Failed to set dumpable, "
                    "core dump may not be available!");
#endif
}


static inline void lsapi_buildPacketHeader( struct lsapi_packet_header * pHeader,
                                char type, int len )
{
    pHeader->m_versionB0 = LSAPI_VERSION_B0;  /* LSAPI protocol version */
    pHeader->m_versionB1 = LSAPI_VERSION_B1;
    pHeader->m_type      = type;
    pHeader->m_flag      = LSAPI_ENDIAN;
    pHeader->m_packetLen.m_iLen = len;
}


static  int lsapi_set_nblock( int fd, int nonblock )
{
    int val = fcntl( fd, F_GETFL, 0 );
    if ( nonblock )
    {
        if (!( val & O_NONBLOCK ))
        {
            return fcntl( fd, F_SETFL, val | O_NONBLOCK );
        }
    }
    else
    {
        if ( val & O_NONBLOCK )
        {
            return fcntl( fd, F_SETFL, val &(~O_NONBLOCK) );
        }
    }
    return 0;
}


static int lsapi_close( int fd )
{
    int ret;
    while( 1 )
    {
        ret = close( fd );
        if (( ret == -1 )&&( errno == EINTR )&&(g_running))
            continue;
        return ret;
    }
}


static void lsapi_close_connection(LSAPI_Request *pReq)
{
    if (pReq->m_fd == -1)
        return;
    lsapi_close(pReq->m_fd);
    pReq->m_fd = -1;
    if (s_busy_workers)
        __atomic_fetch_sub(s_busy_workers, 1, __ATOMIC_SEQ_CST);
    if (s_worker_status)
        __atomic_store_n(&s_worker_status->m_state, LSAPI_STATE_IDLE,
                         __ATOMIC_SEQ_CST);
}


static inline ssize_t lsapi_read( int fd, void * pBuf, size_t len )
{
    ssize_t ret;
    while( 1 )
    {
        ret = read( fd, (char *)pBuf, len );
        if (( ret == -1 )&&( errno == EINTR )&&(g_running))
            continue;
        return ret;
    }
}


/*
static int lsapi_write( int fd, const void * pBuf, int len )
{
   int ret;
   const char * pCur;
   const char * pEnd;
   if ( len == 0 )
       return 0;
   pCur = (const char *)pBuf;
   pEnd = pCur + len;
   while( g_running && (pCur < pEnd) )
   {
       ret = write( fd, pCur, pEnd - pCur );
       if ( ret >= 0)
           pCur += ret;
       else if (( ret == -1 )&&( errno != EINTR ))
           return ret;
   }
   return pCur - (const char *)pBuf;
}
*/


static int lsapi_writev( int fd, struct iovec ** pVec, int count, int totalLen )
{
    int ret;
    int left = totalLen;
    int n = count;

    if (s_skip_write)
        return totalLen;

    while(( left > 0 )&&g_running )
    {
        ret = writev( fd, *pVec, n );
        if ( ret > 0 )
        {
            left -= ret;
            if (( left <= 0)||( !g_running ))
                return totalLen - left;
            while( ret > 0 )
            {
                if ( (*pVec)->iov_len <= (unsigned int )ret )
                {
                    ret -= (*pVec)->iov_len;
                    ++(*pVec);
                }
                else
                {
                    (*pVec)->iov_base = (char *)(*pVec)->iov_base + ret;
                    (*pVec)->iov_len -= ret;
                    break;
                }
            }
        }
        else if ( ret == -1 )
        {
            if ( errno == EAGAIN )
            {
                if ( totalLen - left > 0 )
                    return totalLen - left;
                else
                    return -1;
            }
            else if ( errno != EINTR )
                return ret;
        }
    }
    return totalLen - left;
}


/*
static int getTotalLen( struct iovec * pVec, int count )
{
   struct iovec * pEnd = pVec + count;
   int total = 0;
   while( pVec < pEnd )
   {
       total += pVec->iov_len;
       ++pVec;
   }
   return total;
}
*/


static inline int allocateBuf( LSAPI_Request * pReq, int size )
{
    char * pBuf = (char *)realloc( pReq->m_pReqBuf, size );
    if ( pBuf )
    {
        pReq->m_pReqBuf = pBuf;
        pReq->m_reqBufSize = size;
        pReq->m_pHeader = (struct lsapi_req_header *)pReq->m_pReqBuf;
        return 0;
    }
    return -1;
}


static int allocateIovec( LSAPI_Request * pReq, int n )
{
    struct iovec * p = (struct iovec *)realloc(
                pReq->m_pIovec, sizeof(struct iovec) * n );
    if ( !p )
        return -1;
    pReq->m_pIovecToWrite = p + ( pReq->m_pIovecToWrite - pReq->m_pIovec );
    pReq->m_pIovecCur = p + ( pReq->m_pIovecCur - pReq->m_pIovec );
    pReq->m_pIovec = p;
    pReq->m_pIovecEnd = p + n;
    return 0;
}


static int allocateRespHeaderBuf( LSAPI_Request * pReq, int size )
{
    char * p = (char *)realloc( pReq->m_pRespHeaderBuf, size );
    if ( !p )
        return -1;
    pReq->m_pRespHeaderBufPos   = p + ( pReq->m_pRespHeaderBufPos - pReq->m_pRespHeaderBuf );
    pReq->m_pRespHeaderBuf      = p;
    pReq->m_pRespHeaderBufEnd   = p + size;
    return 0;
}


static inline int verifyHeader( struct lsapi_packet_header * pHeader, char pktType )
{
    if (( LSAPI_VERSION_B0 != pHeader->m_versionB0 )||
        ( LSAPI_VERSION_B1 != pHeader->m_versionB1 )||
        ( pktType != pHeader->m_type ))
        return -1;
    if ( LSAPI_ENDIAN != (pHeader->m_flag & LSAPI_ENDIAN_BIT ))
    {
        register char b;
        b = pHeader->m_packetLen.m_bytes[0];
        pHeader->m_packetLen.m_bytes[0] = pHeader->m_packetLen.m_bytes[3];
        pHeader->m_packetLen.m_bytes[3] = b;
        b = pHeader->m_packetLen.m_bytes[1];
        pHeader->m_packetLen.m_bytes[1] = pHeader->m_packetLen.m_bytes[2];
        pHeader->m_packetLen.m_bytes[2] = b;
    }
    return pHeader->m_packetLen.m_iLen;
}


static int allocateEnvList( struct LSAPI_key_value_pair ** pEnvList,
                        int *curSize, int newSize )
{
    struct LSAPI_key_value_pair * pBuf;
    if ( *curSize >= newSize )
        return 0;
    if ( newSize > 8192 )
        return -1;
    pBuf = (struct LSAPI_key_value_pair *)realloc( *pEnvList, newSize *
                    sizeof(struct LSAPI_key_value_pair) );
    if ( pBuf )
    {
        *pEnvList = pBuf;
        *curSize  = newSize;
        return 0;
    }
    else
        return -1;

}


static inline int isPipe( int fd )
{
    char        achPeer[128];
    socklen_t   len = 128;
    if (( getpeername( fd, (struct sockaddr *)achPeer, &len ) != 0 )&&
        ( errno == ENOTCONN ))
        return 0;
    else
        return 1;
}


static int parseEnv( struct LSAPI_key_value_pair * pEnvList, int count,
            char **pBegin, char * pEnd )
{
    struct LSAPI_key_value_pair * pEnvEnd;
        int keyLen = 0, valLen = 0;
    if ( count > 8192 )
        return -1;
    pEnvEnd = pEnvList + count;
    while( pEnvList != pEnvEnd )
    {
        if ( pEnd - *pBegin < 4 )
            return -1;
        keyLen = *((unsigned char *)((*pBegin)++));
        keyLen = (keyLen << 8) + *((unsigned char *)((*pBegin)++));
        valLen = *((unsigned char *)((*pBegin)++));
        valLen = (valLen << 8) + *((unsigned char *)((*pBegin)++));
        if ( *pBegin + keyLen + valLen > pEnd )
            return -1;
        if (( !keyLen )||( !valLen ))
            return -1;

        pEnvList->pKey = *pBegin;
        *pBegin += keyLen;
        pEnvList->pValue = *pBegin;
        *pBegin += valLen;

        pEnvList->keyLen = keyLen - 1;
        pEnvList->valLen = valLen - 1;
        ++pEnvList;
    }
    if ( memcmp( *pBegin, "\0\0\0\0", 4 ) != 0 )
        return -1;
    *pBegin += 4;
    return 0;
}


static inline void swapIntEndian( int * pInteger )
{
    char * p = (char *)pInteger;
    register char b;
    b = p[0];
    p[0] = p[3];
    p[3] = b;
    b = p[1];
    p[1] = p[2];
    p[2] = b;

}


static inline void fixEndian( LSAPI_Request * pReq )
{
    struct lsapi_req_header *p= pReq->m_pHeader;
    swapIntEndian( &p->m_httpHeaderLen );
    swapIntEndian( &p->m_reqBodyLen );
    swapIntEndian( &p->m_scriptFileOff );
    swapIntEndian( &p->m_scriptNameOff );
    swapIntEndian( &p->m_queryStringOff );
    swapIntEndian( &p->m_requestMethodOff );
    swapIntEndian( &p->m_cntUnknownHeaders );
    swapIntEndian( &p->m_cntEnv );
    swapIntEndian( &p->m_cntSpecialEnv );
}


static void fixHeaderIndexEndian( LSAPI_Request * pReq )
{
    int i;
    for( i = 0; i < H_TRANSFER_ENCODING; ++i )
    {
        if ( pReq->m_pHeaderIndex->m_headerOff[i] )
        {
            register char b;
            char * p = (char *)(&pReq->m_pHeaderIndex->m_headerLen[i]);
            b = p[0];
            p[0] = p[1];
            p[1] = b;
            swapIntEndian( &pReq->m_pHeaderIndex->m_headerOff[i] );
        }
    }
    if ( pReq->m_pHeader->m_cntUnknownHeaders > 0 )
    {
        struct lsapi_header_offset * pCur, *pEnd;
        pCur = pReq->m_pUnknownHeader;
        pEnd = pCur + pReq->m_pHeader->m_cntUnknownHeaders;
        while( pCur < pEnd )
        {
            swapIntEndian( &pCur->nameOff );
            swapIntEndian( &pCur->nameLen );
            swapIntEndian( &pCur->valueOff );
            swapIntEndian( &pCur->valueLen );
            ++pCur;
        }
    }
}


static int validateHeaders( LSAPI_Request * pReq )
{
    int totalLen = pReq->m_pHeader->m_httpHeaderLen;
    int i;
    for(i = 0; i < H_TRANSFER_ENCODING; ++i)
    {
        if ( pReq->m_pHeaderIndex->m_headerOff[i] )
        {
            if (pReq->m_pHeaderIndex->m_headerOff[i] > totalLen
                || pReq->m_pHeaderIndex->m_headerLen[i]
                    + pReq->m_pHeaderIndex->m_headerOff[i] > totalLen)
                return -1;
        }
    }
    if (pReq->m_pHeader->m_cntUnknownHeaders > 0)
    {
        struct lsapi_header_offset * pCur, *pEnd;
        pCur = pReq->m_pUnknownHeader;
        pEnd = pCur + pReq->m_pHeader->m_cntUnknownHeaders;
        while( pCur < pEnd )
        {
            if (pCur->nameOff > totalLen
                || pCur->nameOff + pCur->nameLen > totalLen
                || pCur->valueOff > totalLen
                || pCur->valueOff + pCur->valueLen > totalLen)
                return -1;
            ++pCur;
        }
    }
    return 0;
}


static uid_t s_uid = 0;
static uid_t s_defaultUid;  //web server need set this
static gid_t s_defaultGid;

#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__gnu_linux__)

#define LSAPI_LVE_DISABLED  0
#define LSAPI_LVE_ENABLED   1
#define LSAPI_CAGEFS_ENABLED 2
#define LSAPI_CAGEFS_NO_SUEXEC 3
struct liblve;
static int s_enable_lve = LSAPI_LVE_DISABLED;
static struct liblve * s_lve = NULL;

static void *s_liblve;
static int (*fp_lve_is_available)(void) = NULL;
static int (*fp_lve_instance_init)(struct liblve *) = NULL;
static int (*fp_lve_destroy)(struct liblve *) = NULL;
static int (*fp_lve_enter)(struct liblve *, uint32_t, int32_t, int32_t, uint32_t *) = NULL;
static int (*fp_lve_leave)(struct liblve *, uint32_t *) = NULL;
static int (*fp_lve_jail)( struct passwd *, char *) = NULL;
static int lsapi_load_lve_lib(void)
{
    s_liblve = DL_LOAD("liblve.so.0");
    if (s_liblve)
    {
        fp_lve_is_available = dlsym(s_liblve, "lve_is_available");
        if (dlerror() == NULL)
        {
            if ( !(*fp_lve_is_available)() )
            {
                int uid = getuid();
                if ( uid )
                {
                    if (setreuid( s_uid, uid )) {};
                    if ( !(*fp_lve_is_available)() )
                        s_enable_lve = 0;
                    if (setreuid( uid, s_uid )) {};
                }
            }
        }
    }
    else
    {
        s_enable_lve = LSAPI_LVE_DISABLED;
    }
    return (s_liblve)? 0 : -1;
}


static int init_lve_ex(void)
{
    int rc;
    if ( !s_liblve )
        return -1;
    fp_lve_instance_init = dlsym(s_liblve, "lve_instance_init");
    fp_lve_destroy = dlsym(s_liblve, "lve_destroy");
    fp_lve_enter = dlsym(s_liblve, "lve_enter");
    fp_lve_leave = dlsym(s_liblve, "lve_leave");
    if ( s_enable_lve >= LSAPI_CAGEFS_ENABLED )
        fp_lve_jail = dlsym(s_liblve, "jail" );

    if ( s_lve == NULL )
    {
        rc = (*fp_lve_instance_init)(NULL);
        s_lve = malloc(rc);
    }
    rc = (*fp_lve_instance_init)(s_lve);
    if (rc != 0)
    {
        perror( "LSAPI: Unable to initialize LVE" );
        free( s_lve );
        s_lve = NULL;
        return -1;
    }
    return 0;

}

#endif



static int readSecret( const char * pSecretFile )
{
    struct stat st;
    int fd = open( pSecretFile, O_RDONLY , 0600 );
    if ( fd == -1 )
    {
        lsapi_log("LSAPI: failed to open secret file: %s!\n", pSecretFile );
        return -1;
    }
    if ( fstat( fd, &st ) == -1 )
    {
        lsapi_log("LSAPI: failed to check state of file: %s!\n", pSecretFile );
        close( fd );
        return -1;
    }
/*
    if ( st.st_uid != s_uid )
    {
        lsapi_log("LSAPI: file owner check failure: %s!\n", pSecretFile );
        close( fd );
        return -1;
    }
*/
    if ( st.st_mode & 0077 )
    {
        lsapi_log("LSAPI: file permission check failure: %s\n", pSecretFile );
        close( fd );
        return -1;
    }
    if ( read( fd, s_secret, 16 ) < 16 )
    {
        lsapi_log("LSAPI: failed to read secret from secret file: %s\n", pSecretFile );
        close( fd );
        return -1;
    }
    close( fd );
    return 0;
}


int LSAPI_is_suEXEC_Daemon(void)
{
    if (( !s_uid )&&( s_secret[0] ))
        return 1;
    else
        return 0;
}


static int LSAPI_perror_r( LSAPI_Request * pReq, const char * pErr1, const char *pErr2 )
{
    char achError[4096];
    int n = snprintf(achError, sizeof(achError), "[UID:%d][%d] %s:%s: %s\n",
                     getuid(), getpid(),
                     pErr1, (pErr2)?pErr2:"", strerror(errno));
    if (n > (int)sizeof(achError))
        n = sizeof(achError);
    if ( pReq )
        LSAPI_Write_Stderr_r( pReq, achError, n );
    else
        if (write( STDERR_FILENO, achError, n )) {};
    return 0;
}


#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__gnu_linux__)
static int lsapi_lve_error( LSAPI_Request * pReq )
{
    static const char * headers[] =
    {
        "Cache-Control: private, no-cache, no-store, must-revalidate, max-age=0",
        "Pragma: no-cache",
        "Retry-After: 60",
        "Content-Type: text/html",
        NULL
    };
    static const char achBody[] =
        "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n"
        "<HTML><HEAD>\n<TITLE>508 Resource Limit Is Reached</TITLE>\n"
        "</HEAD><BODY>\n" "<H1>Resource Limit Is Reached</H1>\n"
        "The website is temporarily unable to service your request as it exceeded resource limit.\n"
        "Please try again later.\n"
        "<HR>\n"
        "</BODY></HTML>\n";

    LSAPI_ErrResponse_r( pReq, 508, headers, achBody, sizeof( achBody ) - 1 );
    return 0;
}

static int lsapi_enterLVE( LSAPI_Request * pReq, uid_t uid )
{
    if ( s_lve && uid ) //root user should not do that
    {
        uint32_t cookie;
        int ret = -1;
        ret = (*fp_lve_enter)(s_lve, uid, -1, -1, &cookie);
        if ( ret < 0 )
        {
            //lsapi_log("enter LVE (%d) : result: %d !\n", uid, ret );
            LSAPI_perror_r(pReq, "LSAPI: lve_enter() failure, reached resource limit.", NULL );
            lsapi_lve_error( pReq );
            return -1;
        }
    }

    return 0;
}

static int lsapi_jailLVE( LSAPI_Request * pReq, uid_t uid, struct passwd * pw )
{
    int ret = 0;
    char  error_msg[1024] = "";
    ret = (*fp_lve_jail)( pw, error_msg );
    if ( ret < 0 )
    {
        lsapi_log("LSAPI: LVE jail(%d) result: %d, error: %s !\n",
                  uid, ret, error_msg );
        LSAPI_perror_r( pReq, "LSAPI: jail() failure.", NULL );
        return -1;
    }
    return ret;
}


static int lsapi_initLVE(void)
{
    const char * pEnv;
    if ( (pEnv = getenv( "LSAPI_LVE_ENABLE" ))!= NULL )
    {
        s_enable_lve = atol( pEnv );
        pEnv = NULL;
    }
    else if ( (pEnv = getenv( "LVE_ENABLE" ))!= NULL )
    {
        s_enable_lve = atol( pEnv );
        pEnv = NULL;
    }
    if ( s_enable_lve && !s_uid )
    {
        lsapi_load_lve_lib();
        if ( s_enable_lve )
        {
            return init_lve_ex();
        }

    }
    return 0;
}
#endif


static int setUID_LVE(LSAPI_Request * pReq, uid_t uid, gid_t gid, const char * pChroot)
{
    int rv;
    struct passwd * pw;
    pw = getpwuid( uid );
#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__gnu_linux__)
    if ( s_lve )
    {
        if( lsapi_enterLVE( pReq, uid ) == -1 )
            return -1;
        if ( pw && fp_lve_jail)
        {
            rv = lsapi_jailLVE( pReq, uid, pw );
            if ( rv == -1 )
                return -1;
            if (( rv == 1 )&&(s_enable_lve == LSAPI_CAGEFS_NO_SUEXEC ))    //this mode only use cageFS, does not use suEXEC
            {
                uid = s_defaultUid;
                gid = s_defaultGid;
                pw = getpwuid( uid );
            }
        }
    }
#endif
    //if ( !uid || !gid )  //do not allow root
    //{
    //    return -1;
    //}

#if defined(__FreeBSD__ ) || defined(__NetBSD__) || defined(__OpenBSD__) \
    || defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
    if ( s_enable_core_dump )
        lsapi_enable_core_dump();
#endif

    rv = setgid(gid);
    if (rv == -1)
    {
        LSAPI_perror_r(pReq, "LSAPI: setgid()", NULL);
        return -1;
    }
    if ( pw && (pw->pw_gid == gid ))
    {
        rv = initgroups( pw->pw_name, gid );
        if (rv == -1)
        {
            LSAPI_perror_r(pReq, "LSAPI: initgroups()", NULL);
            return -1;
        }
    }
    else
    {
        rv = setgroups(1, &gid);
        if (rv == -1)
        {
            LSAPI_perror_r(pReq, "LSAPI: setgroups()", NULL);
        }
    }
    if ( pChroot )
    {
        rv = chroot( pChroot );
        if ( rv == -1 )
        {
            LSAPI_perror_r(pReq, "LSAPI: chroot()", NULL);
            return -1;
        }
    }
    rv = setuid(uid);
    if (rv == -1)
    {
        LSAPI_perror_r(pReq, "LSAPI: setuid()", NULL);
        return -1;
    }
#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__gnu_linux__)
    if ( s_enable_core_dump )
        lsapi_enable_core_dump();
#endif
    return 0;
}

static int lsapi_suexec_auth( LSAPI_Request *pReq,
            char * pAuth, int len, char * pUgid, int ugidLen )
{
    lsapi_MD5_CTX md5ctx;
    unsigned char achMD5[16];
    if ( len < 32 )
        return -1;
    memmove( achMD5, pAuth + 16, 16 );
    memmove( pAuth + 16, s_secret, 16 );
    lsapi_MD5Init( &md5ctx );
    lsapi_MD5Update( &md5ctx, (unsigned char *)pAuth, 32 );
    lsapi_MD5Update( &md5ctx, (unsigned char *)pUgid, 8 );
    lsapi_MD5Final( (unsigned char *)pAuth + 16, &md5ctx);
    if ( memcmp( achMD5, pAuth + 16, 16 ) == 0 )
        return 0;
    return 1;
}


static int lsapi_changeUGid( LSAPI_Request * pReq )
{
    int uid = s_defaultUid;
    int gid = s_defaultGid;
    const char *pStderrLog;
    const char *pChroot = NULL;
    struct LSAPI_key_value_pair * pEnv;
    struct LSAPI_key_value_pair * pAuth;
    int i;
    if ( s_uid )
        return 0;
    //with special ID  0x00
    //authenticate the suEXEC request;
    //first one should be MD5( nonce + lscgid secret )
    //remember to clear the secret after verification
    //it should be set at the end of special env
    i = pReq->m_pHeader->m_cntSpecialEnv - 1;
    if ( i >= 0 )
    {
        pEnv = pReq->m_pSpecialEnvList + i;
        if (( *pEnv->pKey == '\000' )&&
            ( strcmp( pEnv->pKey+1, "SUEXEC_AUTH" ) == 0 ))
        {
            --pReq->m_pHeader->m_cntSpecialEnv;
            pAuth = pEnv--;
            if (( *pEnv->pKey == '\000' )&&
                ( strcmp( pEnv->pKey+1, "SUEXEC_UGID" ) == 0 ))
            {
                --pReq->m_pHeader->m_cntSpecialEnv;
                uid = *(uint32_t *)pEnv->pValue;
                gid = *(((uint32_t *)pEnv->pValue) + 1 );
                //lsapi_log("LSAPI: SUEXEC_UGID set UID: %d, GID: %d\n", uid, gid );
            }
            else
            {
                lsapi_log("LSAPI: missing SUEXEC_UGID env, use default user!\n" );
                pEnv = NULL;
            }
            if ( pEnv&& lsapi_suexec_auth( pReq, pAuth->pValue, pAuth->valLen, pEnv->pValue, pEnv->valLen ) == 0 )
            {
                //read UID, GID from specialEnv

            }
            else
            {
                //authentication error
                lsapi_log("LSAPI: SUEXEC_AUTH authentication failed, use default user!\n" );
                uid = 0;
            }
        }
        else
        {
            //lsapi_log("LSAPI: no SUEXEC_AUTH env, use default user!\n" );
        }
    }


    if ( !uid )
    {
        uid = s_defaultUid;
        gid = s_defaultGid;
    }

    //change uid
    if ( setUID_LVE( pReq, uid, gid, pChroot ) == -1 )
    {
        return -1;
    }

    s_uid = uid;

    if ( pReq->m_fdListen != -1 )
    {
        close( pReq->m_fdListen );
        pReq->m_fdListen = -1;
    }

    pStderrLog = LSAPI_GetEnv_r( pReq, "LSAPI_STDERR_LOG");
    if (pStderrLog)
        lsapi_reopen_stderr(pStderrLog);

    return 0;

}


static int parseContentLenFromHeader(LSAPI_Request * pReq)
{
    const char * pContentLen = LSAPI_GetHeader_r( pReq, H_CONTENT_LENGTH );
    if ( pContentLen )
        pReq->m_reqBodyLen = strtoll( pContentLen, NULL, 10 );
    return 0;
}


static int parseRequest( LSAPI_Request * pReq, int totalLen )
{
    int shouldFixEndian;
    char * pBegin = pReq->m_pReqBuf + sizeof( struct lsapi_req_header );
    char * pEnd = pReq->m_pReqBuf + totalLen;
    shouldFixEndian = ( LSAPI_ENDIAN != (
                pReq->m_pHeader->m_pktHeader.m_flag & LSAPI_ENDIAN_BIT ) );
    if ( shouldFixEndian )
    {
        fixEndian( pReq );
    }
    if ( (pReq->m_specialEnvListSize < pReq->m_pHeader->m_cntSpecialEnv )&&
            allocateEnvList( &pReq->m_pSpecialEnvList,
                &pReq->m_specialEnvListSize,
                pReq->m_pHeader->m_cntSpecialEnv ) == -1 )
        return -1;
    if ( (pReq->m_envListSize < pReq->m_pHeader->m_cntEnv )&&
            allocateEnvList( &pReq->m_pEnvList, &pReq->m_envListSize,
                pReq->m_pHeader->m_cntEnv ) == -1 )
        return -1;

    if ( parseEnv( pReq->m_pSpecialEnvList,
                pReq->m_pHeader->m_cntSpecialEnv,
                &pBegin, pEnd ) == -1 )
        return -1;
    if ( parseEnv( pReq->m_pEnvList, pReq->m_pHeader->m_cntEnv,
                &pBegin, pEnd ) == -1 )
        return -1;
    if (pReq->m_pHeader->m_scriptFileOff < 0
        || pReq->m_pHeader->m_scriptFileOff >= totalLen
        || pReq->m_pHeader->m_scriptNameOff < 0
        || pReq->m_pHeader->m_scriptNameOff >= totalLen
        || pReq->m_pHeader->m_queryStringOff < 0
        || pReq->m_pHeader->m_queryStringOff >= totalLen
        || pReq->m_pHeader->m_requestMethodOff < 0
        || pReq->m_pHeader->m_requestMethodOff >= totalLen)
    {
        lsapi_log("Bad request header - ERROR#1\n");
        return -1;
    }
    pReq->m_pScriptFile     = pReq->m_pReqBuf + pReq->m_pHeader->m_scriptFileOff;
    pReq->m_pScriptName     = pReq->m_pReqBuf + pReq->m_pHeader->m_scriptNameOff;
    pReq->m_pQueryString    = pReq->m_pReqBuf + pReq->m_pHeader->m_queryStringOff;
    pReq->m_pRequestMethod  = pReq->m_pReqBuf + pReq->m_pHeader->m_requestMethodOff;

    pBegin = pReq->m_pReqBuf + (( pBegin - pReq->m_pReqBuf + 7 ) & (~0x7));
    pReq->m_pHeaderIndex = ( struct lsapi_http_header_index * )pBegin;
    pBegin += sizeof( struct lsapi_http_header_index );

    pReq->m_pUnknownHeader = (struct lsapi_header_offset *)pBegin;
    pBegin += sizeof( struct lsapi_header_offset) *
                    pReq->m_pHeader->m_cntUnknownHeaders;

    pReq->m_pHttpHeader = pBegin;
    pBegin += pReq->m_pHeader->m_httpHeaderLen;
    if ( pBegin != pEnd )
    {
        lsapi_log("Request header does match total size, total: %d, "
                 "real: %ld\n", totalLen, pBegin - pReq->m_pReqBuf );
        return -1;
    }
    if ( shouldFixEndian )
    {
        fixHeaderIndexEndian( pReq );
    }

    if (validateHeaders(pReq) == -1)
    {
        lsapi_log("Bad request header - ERROR#2\n");
        return -1;
    }

    pReq->m_reqBodyLen = pReq->m_pHeader->m_reqBodyLen;
    if ( pReq->m_reqBodyLen == -2 )
    {
        parseContentLenFromHeader(pReq);
    }

    return 0;
}


//OPTIMIZATION
static char s_accept_notify = 0;
static char s_schedule_notify = 0;
static char s_notify_scheduled = 0;
static char s_notified_pid = 0;

static struct lsapi_packet_header s_ack = {'L', 'S',
                LSAPI_REQ_RECEIVED, LSAPI_ENDIAN, {LSAPI_PACKET_HEADER_LEN} };
static struct lsapi_packet_header s_conn_close_pkt = {'L', 'S',
                LSAPI_CONN_CLOSE, LSAPI_ENDIAN, {LSAPI_PACKET_HEADER_LEN} };


static inline int send_notification_pkt( int fd, struct lsapi_packet_header *pkt )
{
    if ( write( fd, pkt, LSAPI_PACKET_HEADER_LEN ) < LSAPI_PACKET_HEADER_LEN )
        return -1;
    return 0;
}


static inline int send_req_received_notification( int fd )
{
    return send_notification_pkt(fd, &s_ack);
}


static inline int send_conn_close_notification( int fd )
{
    return send_notification_pkt(fd, &s_conn_close_pkt);
}


//static void lsapi_sigalarm( int sig )
//{
//    if ( s_notify_scheduled )
//    {
//        s_notify_scheduled = 0;
//        if ( g_req.m_fd != -1 )
//            write_req_received_notification( g_req.m_fd );
//    }
//}


static inline int lsapi_schedule_notify(void)
{
    if ( !s_notify_scheduled )
    {
        alarm( 2 );
        s_notify_scheduled = 1;
    }
    return 0;
}


static inline int notify_req_received( int fd )
{
    if ( s_schedule_notify )
        return lsapi_schedule_notify();
    return send_req_received_notification( fd );

}


static inline int lsapi_notify_pid( int fd )
{
    char achBuf[16];
    lsapi_buildPacketHeader( (struct lsapi_packet_header *)achBuf, LSAPI_STDERR_STREAM,
                        8 + LSAPI_PACKET_HEADER_LEN );
    memmove( &achBuf[8], "\0PID", 4 );
    *((int *)&achBuf[12]) = getpid();

    if ( write( fd, achBuf, 16 ) < 16 )
        return -1;
    return 0;
}

static int readReq( LSAPI_Request * pReq )
{
    int len;
    int packetLen;
    if ( !pReq )
        return -1;
    if ( pReq->m_reqBufSize < 8192 )
    {
        if ( allocateBuf( pReq, 8192 ) == -1 )
            return -1;
    }

    while ( pReq->m_bufRead < LSAPI_PACKET_HEADER_LEN )
    {
        len = lsapi_read( pReq->m_fd, pReq->m_pReqBuf, pReq->m_reqBufSize );
        if ( len <= 0 )
            return -1;
        pReq->m_bufRead += len;
    }
    pReq->m_reqState = LSAPI_ST_REQ_HEADER;

    packetLen = verifyHeader( &pReq->m_pHeader->m_pktHeader, LSAPI_BEGIN_REQUEST );
    if ( packetLen < 0 )
    {
        lsapi_log("packetLen < 0\n");
        return -1;
    }
    if ( packetLen > LSAPI_MAX_HEADER_LEN )
    {
        lsapi_log("packetLen > %d\n", LSAPI_MAX_HEADER_LEN );
        return -1;
    }

    if ( packetLen + 1024 > pReq->m_reqBufSize )
    {
        if ( allocateBuf( pReq, packetLen + 1024 ) == -1 )
            return -1;
    }
    while( packetLen > pReq->m_bufRead )
    {
        len = lsapi_read( pReq->m_fd, pReq->m_pReqBuf + pReq->m_bufRead, packetLen - pReq->m_bufRead );
        if ( len <= 0 )
            return -1;
        pReq->m_bufRead += len;
    }
    if ( parseRequest( pReq, packetLen ) < 0 )
    {
        lsapi_log("ParseRequest error\n");
        return -1;
    }

    pReq->m_reqState = LSAPI_ST_REQ_BODY | LSAPI_ST_RESP_HEADER;

    if ( !s_uid )
    {
        if ( lsapi_changeUGid( pReq ) )
            return -1;
        memset(s_secret, 0, sizeof(s_secret));
    }
    pReq->m_bufProcessed = packetLen;

    //OPTIMIZATION
    if ( !s_accept_notify && !s_notified_pid )
        return notify_req_received( pReq->m_fd );
    else
    {
        s_notified_pid = 0;
        return 0;
    }
}


int LSAPI_Init(void)
{
    if ( !g_inited )
    {
        s_uid = geteuid();
        s_secret[0] = 0;
        lsapi_signal(SIGPIPE, lsapi_sigpipe);
        lsapi_signal(SIGUSR1, lsapi_siguser1);

#if defined(SIGXFSZ) && defined(SIG_IGN)
        signal(SIGXFSZ, SIG_IGN);
#endif
        /* let STDOUT function as STDERR,
           just in case writing to STDOUT directly */
        dup2( 2, 1 );
        if ( LSAPI_InitRequest( &g_req, LSAPI_SOCK_FILENO ) == -1 )
            return -1;
        g_inited = 1;
        s_ppid = getppid();
        void *pthread_lib = dlopen("libpthread.so", RTLD_LAZY);
        if (pthread_lib)
            pthread_atfork_func = dlsym(pthread_lib, "pthread_atfork");

    }
    return 0;
}


void LSAPI_Stop(void)
{
    g_running = 0;
}


int LSAPI_IsRunning(void)
{
    return g_running;
}


void LSAPI_Register_Pgrp_Timer_Callback(LSAPI_On_Timer_pf cb)
{
    s_proc_group_timer_cb = cb;
}


int LSAPI_InitRequest( LSAPI_Request * pReq, int fd )
{
    int newfd;
    if ( !pReq )
        return -1;
    memset( pReq, 0, sizeof( LSAPI_Request ) );
    if ( allocateIovec( pReq, 16 ) == -1 )
        return -1;
    pReq->m_pRespBuf = pReq->m_pRespBufPos = (char *)malloc( LSAPI_RESP_BUF_SIZE );
    if ( !pReq->m_pRespBuf )
        return -1;
    pReq->m_pRespBufEnd = pReq->m_pRespBuf + LSAPI_RESP_BUF_SIZE;
    pReq->m_pIovecCur = pReq->m_pIovecToWrite = pReq->m_pIovec + 1;
    pReq->m_respPktHeaderEnd = &pReq->m_respPktHeader[5];
    if ( allocateRespHeaderBuf( pReq, LSAPI_INIT_RESP_HEADER_LEN ) == -1 )
        return -1;

    if ( fd == STDIN_FILENO )
    {
        fd = dup( fd );
        newfd = open( "/dev/null", O_RDWR );
        dup2( newfd, STDIN_FILENO );
    }

    if ( isPipe( fd ) )
    {
        pReq->m_fdListen = -1;
        pReq->m_fd = fd;
    }
    else
    {
        pReq->m_fdListen = fd;
        pReq->m_fd = -1;
        lsapi_set_nblock( fd, 1 );
    }
    return 0;
}


int LSAPI_Is_Listen( void )
{
    return LSAPI_Is_Listen_r( &g_req );
}


int LSAPI_Is_Listen_r( LSAPI_Request * pReq)
{
    return pReq->m_fdListen != -1;
}


int LSAPI_Accept_r( LSAPI_Request * pReq )
{
    char        achPeer[128];
    socklen_t   len;
    int         nodelay = 1;

    if ( !pReq )
        return -1;
    if ( LSAPI_Finish_r( pReq ) == -1 )
        return -1;
    lsapi_set_nblock( pReq->m_fdListen , 0 );
    while( g_running )
    {
        if ( pReq->m_fd == -1 )
        {
            if ( pReq->m_fdListen != -1)
            {
                len = sizeof( achPeer );
                pReq->m_fd = accept( pReq->m_fdListen,
                            (struct sockaddr *)&achPeer, &len );
                if ( pReq->m_fd == -1 )
                {
                    if (( errno == EINTR )||( errno == EAGAIN))
                        continue;
                    else
                        return -1;
                }
                else
                {
                    if (s_worker_status)
                        __atomic_store_n(&s_worker_status->m_state,
                                       LSAPI_STATE_CONNECTED, __ATOMIC_SEQ_CST);
                    if (s_busy_workers)
                        __atomic_fetch_add(s_busy_workers, 1, __ATOMIC_SEQ_CST);
                    lsapi_set_nblock( pReq->m_fd , 0 );
                    if (((struct sockaddr *)&achPeer)->sa_family == AF_INET )
                    {
                        setsockopt(pReq->m_fd, IPPROTO_TCP, TCP_NODELAY,
                                   (char *)&nodelay, sizeof(nodelay));
                    }
                    //init_conn_key( pReq->m_fd );
                    //OPTIMIZATION
                    if ( s_accept_notify )
                        if ( notify_req_received( pReq->m_fd ) == -1 )
                                return -1;
                }
            }
            else
                return -1;
        }
        if ( !readReq( pReq ) )
            break;
        //abort();
        lsapi_close_connection(pReq);
        LSAPI_Reset_r( pReq );
    }
    return 0;
}


static struct lsapi_packet_header   finish_close[2] =
{
    {'L', 'S', LSAPI_RESP_END, LSAPI_ENDIAN, {LSAPI_PACKET_HEADER_LEN} },
    {'L', 'S', LSAPI_CONN_CLOSE, LSAPI_ENDIAN, {LSAPI_PACKET_HEADER_LEN} }
};


int LSAPI_Finish_r( LSAPI_Request * pReq )
{
    /* finish req body */
    if ( !pReq )
        return -1;
    if (pReq->m_reqState)
    {
        if ( pReq->m_fd != -1 )
        {
            if ( pReq->m_reqState & LSAPI_ST_RESP_HEADER )
            {
                LSAPI_FinalizeRespHeaders_r( pReq );
            }
            if ( pReq->m_pRespBufPos != pReq->m_pRespBuf )
            {
                Flush_RespBuf_r( pReq );
            }

            pReq->m_pIovecCur->iov_base = (void *)finish_close;
            pReq->m_pIovecCur->iov_len  = LSAPI_PACKET_HEADER_LEN;
            pReq->m_totalLen += LSAPI_PACKET_HEADER_LEN;
            ++pReq->m_pIovecCur;
            LSAPI_Flush_r( pReq );
        }
        LSAPI_Reset_r( pReq );
    }
    return 0;
}


int LSAPI_End_Response_r(LSAPI_Request * pReq)
{
    if (!pReq)
        return -1;
    if (pReq->m_reqState & LSAPI_ST_BACKGROUND)
        return 0;
    if (pReq->m_reqState)
    {
        if ( pReq->m_fd != -1 )
        {
            if ( pReq->m_reqState & LSAPI_ST_RESP_HEADER )
            {
                if ( pReq->m_pRespHeaderBufPos <= pReq->m_pRespHeaderBuf )
                    return 0;

                LSAPI_FinalizeRespHeaders_r( pReq );
            }
            if ( pReq->m_pRespBufPos != pReq->m_pRespBuf )
            {
                Flush_RespBuf_r( pReq );
            }

            pReq->m_pIovecCur->iov_base = (void *)finish_close;
            pReq->m_pIovecCur->iov_len  = LSAPI_PACKET_HEADER_LEN << 1;
            pReq->m_totalLen += LSAPI_PACKET_HEADER_LEN << 1;
            ++pReq->m_pIovecCur;
            LSAPI_Flush_r( pReq );
            lsapi_close_connection(pReq);
        }
        pReq->m_reqState |= LSAPI_ST_BACKGROUND;
    }
    return 0;
}


void LSAPI_Reset_r( LSAPI_Request * pReq )
{
    pReq->m_pRespBufPos         = pReq->m_pRespBuf;
    pReq->m_pIovecCur           = pReq->m_pIovecToWrite = pReq->m_pIovec + 1;
    pReq->m_pRespHeaderBufPos   = pReq->m_pRespHeaderBuf;

    memset( &pReq->m_pHeaderIndex, 0,
            (char *)(pReq->m_respHeaderLen) - (char *)&pReq->m_pHeaderIndex );
}


int LSAPI_Release_r( LSAPI_Request * pReq )
{
    if ( pReq->m_pReqBuf )
        free( pReq->m_pReqBuf );
    if ( pReq->m_pSpecialEnvList )
        free( pReq->m_pSpecialEnvList );
    if ( pReq->m_pEnvList )
        free( pReq->m_pEnvList );
    if ( pReq->m_pRespHeaderBuf )
        free( pReq->m_pRespHeaderBuf );
    return 0;
}


char * LSAPI_GetHeader_r( LSAPI_Request * pReq, int headerIndex )
{
    int off;
    if ( !pReq || ((unsigned int)headerIndex > H_TRANSFER_ENCODING) )
        return NULL;
    off = pReq->m_pHeaderIndex->m_headerOff[ headerIndex ];
    if ( !off )
        return NULL;
    if ( *(pReq->m_pHttpHeader + off
        + pReq->m_pHeaderIndex->m_headerLen[ headerIndex ]) )
    {
        *( pReq->m_pHttpHeader + off
            + pReq->m_pHeaderIndex->m_headerLen[ headerIndex ]) = 0;
    }
    return pReq->m_pHttpHeader + off;
}


static int readBodyToReqBuf( LSAPI_Request * pReq )
{
    off_t bodyLeft;
    ssize_t len = pReq->m_bufRead - pReq->m_bufProcessed;
    if ( len > 0 )
        return len;
    pReq->m_bufRead = pReq->m_bufProcessed = pReq->m_pHeader->m_pktHeader.m_packetLen.m_iLen;

    bodyLeft = pReq->m_reqBodyLen - pReq->m_reqBodyRead;
    len = pReq->m_reqBufSize - pReq->m_bufRead;
    if ( len < 0 )
        return -1;
    if ( len > bodyLeft )
        len = bodyLeft;

    len = lsapi_read( pReq->m_fd, pReq->m_pReqBuf + pReq->m_bufRead, len );
    if ( len > 0 )
        pReq->m_bufRead += len;
    return len;
}


int LSAPI_ReqBodyGetChar_r( LSAPI_Request * pReq )
{
    if (!pReq || (pReq->m_fd ==-1) )
        return EOF;
    if ( pReq->m_bufProcessed >= pReq->m_bufRead )
    {
        if ( readBodyToReqBuf( pReq ) <= 0 )
            return EOF;
    }
    ++pReq->m_reqBodyRead;
    return (unsigned char)*(pReq->m_pReqBuf + pReq->m_bufProcessed++);
}


int LSAPI_ReqBodyGetLine_r( LSAPI_Request * pReq, char * pBuf, size_t bufLen, int *getLF )
{
    ssize_t len;
    ssize_t left;
    char * pBufEnd = pBuf + bufLen - 1;
    char * pBufCur = pBuf;
    char * pCur;
    char * p;
    if (!pReq || pReq->m_fd == -1 || !pBuf || !getLF)
        return -1;
    *getLF = 0;
    while( (left = pBufEnd - pBufCur ) > 0 )
    {

        len = pReq->m_bufRead - pReq->m_bufProcessed;
        if ( len <= 0 )
        {
            if ( (len = readBodyToReqBuf( pReq )) <= 0 )
            {
                *getLF = 1;
                break;
            }
        }
        if ( len > left )
            len = left;
        pCur = pReq->m_pReqBuf + pReq->m_bufProcessed;
        p = memchr( pCur, '\n', len );
        if ( p )
            len = p - pCur + 1;
        memmove( pBufCur, pCur, len );
        pBufCur += len;
        pReq->m_bufProcessed += len;

        pReq->m_reqBodyRead += len;

        if ( p )
        {
            *getLF = 1;
            break;
        }
    }
    *pBufCur = 0;

    return pBufCur - pBuf;
}


ssize_t LSAPI_ReadReqBody_r( LSAPI_Request * pReq, char * pBuf, size_t bufLen )
{
    ssize_t len;
    off_t total;
    /* char *pOldBuf = pBuf; */
    if (!pReq || pReq->m_fd == -1 || !pBuf || (ssize_t)bufLen < 0)
        return -1;

    total = pReq->m_reqBodyLen - pReq->m_reqBodyRead;

    if ( total <= 0 )
        return 0;
    if ( total < (ssize_t)bufLen )
        bufLen = total;

    total = 0;
    len = pReq->m_bufRead - pReq->m_bufProcessed;
    if ( len > 0 )
    {
        if ( len > (ssize_t)bufLen )
            len = bufLen;
        memmove( pBuf, pReq->m_pReqBuf + pReq->m_bufProcessed, len );
        pReq->m_bufProcessed += len;
        total += len;
        pBuf += len;
        bufLen -= len;
    }
    while( bufLen > 0 )
    {
        len = lsapi_read( pReq->m_fd, pBuf, bufLen );
        if ( len > 0 )
        {
            total += len;
            pBuf += len;
            bufLen -= len;
        }
        else if ( len <= 0 )
        {
            if ( !total)
                return -1;
            break;
        }
    }
    pReq->m_reqBodyRead += total;
    return total;

}


ssize_t LSAPI_Write_r( LSAPI_Request * pReq, const char * pBuf, size_t len )
{
    struct lsapi_packet_header * pHeader;
    const char * pEnd;
    const char * p;
    ssize_t bufLen;
    ssize_t toWrite;
    ssize_t packetLen;
    int skip = 0;

    if (!pReq || !pBuf)
        return -1;
    if (pReq->m_reqState & LSAPI_ST_BACKGROUND)
        return len;
    if (pReq->m_fd == -1)
        return -1;
    if ( pReq->m_reqState & LSAPI_ST_RESP_HEADER )
    {
        LSAPI_FinalizeRespHeaders_r( pReq );
/*
        if ( *pBuf == '\r' )
        {
            ++skip;
        }
        if ( *pBuf == '\n' )
        {
            ++skip;
        }
*/
    }
    pReq->m_reqState |= LSAPI_ST_RESP_BODY;

    if ( ((ssize_t)len - skip) < pReq->m_pRespBufEnd - pReq->m_pRespBufPos )
    {
        memmove( pReq->m_pRespBufPos, pBuf + skip, len - skip );
        pReq->m_pRespBufPos += len - skip;
        return len;
    }


    pHeader = pReq->m_respPktHeader;
    p       = pBuf + skip;
    pEnd    = pBuf + len;
    bufLen  = pReq->m_pRespBufPos - pReq->m_pRespBuf;

    while( ( toWrite = pEnd - p ) > 0 )
    {
        packetLen = toWrite + bufLen;
        if ( LSAPI_MAX_DATA_PACKET_LEN < packetLen)
        {
            packetLen = LSAPI_MAX_DATA_PACKET_LEN;
            toWrite = packetLen - bufLen;
        }

        lsapi_buildPacketHeader( pHeader, LSAPI_RESP_STREAM,
                            packetLen + LSAPI_PACKET_HEADER_LEN );
        pReq->m_totalLen += packetLen + LSAPI_PACKET_HEADER_LEN;

        pReq->m_pIovecCur->iov_base = (void *)pHeader;
        pReq->m_pIovecCur->iov_len  = LSAPI_PACKET_HEADER_LEN;
        ++pReq->m_pIovecCur;
        ++pHeader;
        if ( bufLen > 0 )
        {
            pReq->m_pIovecCur->iov_base = (void *)pReq->m_pRespBuf;
            pReq->m_pIovecCur->iov_len  = bufLen;
            pReq->m_pRespBufPos = pReq->m_pRespBuf;
            ++pReq->m_pIovecCur;
            bufLen = 0;
        }

        pReq->m_pIovecCur->iov_base = (void *)p;
        pReq->m_pIovecCur->iov_len  = toWrite;
        ++pReq->m_pIovecCur;
        p += toWrite;

        if ( pHeader >= pReq->m_respPktHeaderEnd - 1)
        {
            if ( LSAPI_Flush_r( pReq ) == -1 )
                return -1;
            pHeader = pReq->m_respPktHeader;
        }
    }
    if ( pHeader != pReq->m_respPktHeader )
        if ( LSAPI_Flush_r( pReq ) == -1 )
            return -1;
    return p - pBuf;
}


#if defined(__FreeBSD__ )
ssize_t gsendfile( int fdOut, int fdIn, off_t* off, size_t size )
{
    ssize_t ret;
    off_t written;
    ret = sendfile( fdIn, fdOut, *off, size, NULL, &written, 0 );
    if ( written > 0 )
    {
        ret = written;
        *off += ret;
    }
    return ret;
}
#endif

#if defined(__OpenBSD__) || defined(__NetBSD__)
ssize_t gsendfile( int fdOut, int fdIn, off_t* off, size_t size )
{
    ssize_t ret;
    off_t written = 0;
    const size_t bufsiz = 16384;
    unsigned char in[bufsiz] = {0};

    if (lseek(fdIn, *off, SEEK_SET) == -1) {
        return -1;
    }

    while (size > 0) {
	    size_t tor = size > sizeof(in) ? sizeof(in) : size;
	    ssize_t c = read(fdIn, in, tor);
	    if (c <= 0) {
		    goto end;
	    }

	    ssize_t w = write(fdOut, in, c);
	    if (w != c) {
		    goto end;
	    }

	    written += w;
	    size -= c;
    }

end:
    *off += written;
    return 0;
}
#endif


#if defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
ssize_t gsendfile( int fdOut, int fdIn, off_t* off, size_t size )
{
    ssize_t ret;
    off_t len = size;
    ret = sendfile( fdIn, fdOut, *off, &len, NULL, 0 );
    if (( ret == 0 )&&( len > 0 ))
    {
        ret = len;
        *off += len;
    }
    return ret;
}
#endif


#if defined(sun) || defined(__sun)
#include <sys/sendfile.h>
ssize_t gsendfile( int fdOut, int fdIn, off_t *off, size_t size )
{
    int n = 0 ;
    sendfilevec_t vec[1];

    vec[n].sfv_fd   = fdIn;
    vec[n].sfv_flag = 0;
    vec[n].sfv_off  = *off;
    vec[n].sfv_len  = size;
    ++n;

    size_t written;
    ssize_t ret = sendfilev( fdOut, vec, n, &written );
    if (( !ret )||( errno == EAGAIN ))
        ret = written;
    if ( ret > 0 )
        *off += ret;
    return ret;
}
#endif


#if defined(linux) || defined(__linux) || defined(__linux__) || \
    defined(__gnu_linux__)
#include <sys/sendfile.h>
#define gsendfile sendfile
#endif


#if defined(HPUX)
ssize_t gsendfile( int fdOut, int fdIn, off_t * off, size_t size )
{
    return sendfile( fdOut, fdIn, off, size, NULL, 0 );
}
#endif


ssize_t LSAPI_sendfile_r( LSAPI_Request * pReq, int fdIn, off_t* off, size_t size )
{
    struct lsapi_packet_header * pHeader = pReq->m_respPktHeader;
    if ( !pReq || (pReq->m_fd == -1) || fdIn == -1 )
        return -1;
    if ( pReq->m_reqState & LSAPI_ST_RESP_HEADER )
    {
        LSAPI_FinalizeRespHeaders_r( pReq );
    }
    pReq->m_reqState |= LSAPI_ST_RESP_BODY;

    LSAPI_Flush_r(pReq);

    lsapi_buildPacketHeader( pHeader, LSAPI_RESP_STREAM,
                            size + LSAPI_PACKET_HEADER_LEN );


    if (write(pReq->m_fd,  (const char *) pHeader, LSAPI_PACKET_HEADER_LEN ) != LSAPI_PACKET_HEADER_LEN)
        return -1;

    return gsendfile( pReq->m_fd, fdIn, off, size );
}


void Flush_RespBuf_r( LSAPI_Request * pReq )
{
    struct lsapi_packet_header * pHeader = pReq->m_respPktHeader;
    int bufLen = pReq->m_pRespBufPos - pReq->m_pRespBuf;
    pReq->m_reqState |= LSAPI_ST_RESP_BODY;
    lsapi_buildPacketHeader( pHeader, LSAPI_RESP_STREAM,
                        bufLen + LSAPI_PACKET_HEADER_LEN );
    pReq->m_totalLen += bufLen + LSAPI_PACKET_HEADER_LEN;

    pReq->m_pIovecCur->iov_base = (void *)pHeader;
    pReq->m_pIovecCur->iov_len  = LSAPI_PACKET_HEADER_LEN;
    ++pReq->m_pIovecCur;
    ++pHeader;
    if ( bufLen > 0 )
    {
        pReq->m_pIovecCur->iov_base = (void *)pReq->m_pRespBuf;
        pReq->m_pIovecCur->iov_len  = bufLen;
        pReq->m_pRespBufPos = pReq->m_pRespBuf;
        ++pReq->m_pIovecCur;
        bufLen = 0;
    }
}


int LSAPI_Flush_r( LSAPI_Request * pReq )
{
    int ret = 0;
    int n;
    if ( !pReq )
        return -1;
    n = pReq->m_pIovecCur - pReq->m_pIovecToWrite;
    if (( 0 == n )&&( pReq->m_pRespBufPos == pReq->m_pRespBuf ))
        return 0;
    if ( pReq->m_fd == -1 )
    {
        pReq->m_pRespBufPos = pReq->m_pRespBuf;
        pReq->m_totalLen = 0;
        pReq->m_pIovecCur = pReq->m_pIovecToWrite = pReq->m_pIovec;
        return -1;
    }
    if ( pReq->m_reqState & LSAPI_ST_RESP_HEADER )
    {
        LSAPI_FinalizeRespHeaders_r( pReq );
    }
    if ( pReq->m_pRespBufPos != pReq->m_pRespBuf )
    {
        Flush_RespBuf_r( pReq );
    }

    n = pReq->m_pIovecCur - pReq->m_pIovecToWrite;
    if ( n > 0 )
    {

        ret = lsapi_writev( pReq->m_fd, &pReq->m_pIovecToWrite,
                  n, pReq->m_totalLen );
        if ( ret < pReq->m_totalLen )
        {
            lsapi_close_connection(pReq);
            ret = -1;
        }
        pReq->m_totalLen = 0;
        pReq->m_pIovecCur = pReq->m_pIovecToWrite = pReq->m_pIovec;
    }
    return ret;
}


ssize_t LSAPI_Write_Stderr_r( LSAPI_Request * pReq, const char * pBuf, size_t len )
{
    struct lsapi_packet_header header;
    const char * pEnd;
    const char * p;
    ssize_t packetLen;
    ssize_t totalLen;
    int ret;
    struct iovec iov[2];
    struct iovec *pIov;

    if ( !pReq )
        return -1;
    if (s_stderr_log_path || pReq->m_fd == -1 || pReq->m_fd == pReq->m_fdListen)
        return write( 2, pBuf, len );
    if ( pReq->m_pRespBufPos != pReq->m_pRespBuf )
    {
        LSAPI_Flush_r( pReq );
    }

    p       = pBuf;
    pEnd    = pBuf + len;

    while( ( packetLen = pEnd - p ) > 0 )
    {
        if ( LSAPI_MAX_DATA_PACKET_LEN < packetLen)
        {
            packetLen = LSAPI_MAX_DATA_PACKET_LEN;
        }

        lsapi_buildPacketHeader( &header, LSAPI_STDERR_STREAM,
                            packetLen + LSAPI_PACKET_HEADER_LEN );
        totalLen = packetLen + LSAPI_PACKET_HEADER_LEN;

        iov[0].iov_base = (void *)&header;
        iov[0].iov_len  = LSAPI_PACKET_HEADER_LEN;

        iov[1].iov_base = (void *)p;
        iov[1].iov_len  = packetLen;
        p += packetLen;
        pIov = iov;
        ret = lsapi_writev( pReq->m_fd, &pIov,
                  2, totalLen );
        if ( ret < totalLen )
        {
            lsapi_close_connection(pReq);
            ret = -1;
        }
    }
    return p - pBuf;
}


static char * GetHeaderVar( LSAPI_Request * pReq, const char * name )
{
    int i;
    char * pValue;
    for( i = 0; i < H_TRANSFER_ENCODING; ++i )
    {
        if ( pReq->m_pHeaderIndex->m_headerOff[i] )
        {
            if ( strcmp( name, CGI_HEADERS[i] ) == 0 )
            {
                pValue = pReq->m_pHttpHeader
                         + pReq->m_pHeaderIndex->m_headerOff[i];
                if ( *(pValue + pReq->m_pHeaderIndex->m_headerLen[i]) != '\0')
                {
                    *(pValue + pReq->m_pHeaderIndex->m_headerLen[i]) = '\0';
                }
                return pValue;
            }
        }
    }
    if ( pReq->m_pHeader->m_cntUnknownHeaders > 0 )
    {
        const char *p;
        char *pKey;
        char *pKeyEnd;
        int  keyLen;
        struct lsapi_header_offset * pCur, *pEnd;
        pCur = pReq->m_pUnknownHeader;
        pEnd = pCur + pReq->m_pHeader->m_cntUnknownHeaders;
        while( pCur < pEnd )
        {
            pKey = pReq->m_pHttpHeader + pCur->nameOff;
            keyLen = pCur->nameLen;
            pKeyEnd = pKey + keyLen;
            p = &name[5];

            while(( pKey < pKeyEnd )&&( *p ))
            {
                char ch = toupper( *pKey );
                if ((ch != *p )||(( *p == '_' )&&( ch != '-')))
                    break;
                ++p; ++pKey;
            }
            if (( pKey == pKeyEnd )&& (!*p ))
            {
                pValue = pReq->m_pHttpHeader + pCur->valueOff;

                if ( *(pValue + pCur->valueLen) != '\0')
                {
                    *(pValue + pCur->valueLen) = '\0';
                }
                return pValue;
            }
            ++pCur;
        }
    }
    return NULL;
}


char * LSAPI_GetEnv_r( LSAPI_Request * pReq, const char * name )
{
    struct LSAPI_key_value_pair * pBegin = pReq->m_pEnvList;
    struct LSAPI_key_value_pair * pEnd = pBegin + pReq->m_pHeader->m_cntEnv;
    if ( !pReq || !name )
        return NULL;
    if ( strncmp( name, "HTTP_", 5 ) == 0 )
    {
        return GetHeaderVar( pReq, name );
    }
    while( pBegin < pEnd )
    {
        if ( strcmp( name, pBegin->pKey ) == 0 )
            return pBegin->pValue;
        ++pBegin;
    }
    return NULL;
}


struct _headerInfo
{
    const char * _name;
    int          _nameLen;
    const char * _value;
    int          _valueLen;
};


int compareValueLocation(const void * v1, const void *v2 )
{
    return ((const struct _headerInfo *)v1)->_value -
           ((const struct _headerInfo *)v2)->_value;
}


int LSAPI_ForeachOrgHeader_r( LSAPI_Request * pReq,
            LSAPI_CB_EnvHandler fn, void * arg )
{
    int i;
    int len = 0;
    char * pValue;
    int ret;
    int count = 0;
    struct _headerInfo headers[512];

    if ( !pReq || !fn )
        return -1;

    if ( !pReq->m_pHeaderIndex )
        return 0;

    for( i = 0; i < H_TRANSFER_ENCODING; ++i )
    {
        if ( pReq->m_pHeaderIndex->m_headerOff[i] )
        {
            len = pReq->m_pHeaderIndex->m_headerLen[i];
            pValue = pReq->m_pHttpHeader + pReq->m_pHeaderIndex->m_headerOff[i];
            *(pValue + len ) = 0;
            headers[count]._name = HTTP_HEADERS[i];
            headers[count]._nameLen = HTTP_HEADER_LEN[i];
            headers[count]._value = pValue;
            headers[count]._valueLen = len;
            ++count;

            //ret = (*fn)( HTTP_HEADERS[i], HTTP_HEADER_LEN[i],
            //            pValue, len, arg );
            //if ( ret <= 0 )
            //    return ret;
        }
    }
    if ( pReq->m_pHeader->m_cntUnknownHeaders > 0 )
    {
        char *pKey;
        int  keyLen;
        struct lsapi_header_offset * pCur, *pEnd;
        pCur = pReq->m_pUnknownHeader;
        pEnd = pCur + pReq->m_pHeader->m_cntUnknownHeaders;
        while( pCur < pEnd )
        {
            pKey = pReq->m_pHttpHeader + pCur->nameOff;
            keyLen = pCur->nameLen;
            *(pKey + keyLen ) = 0;

            pValue = pReq->m_pHttpHeader + pCur->valueOff;
            *(pValue + pCur->valueLen ) = 0;
            headers[count]._name = pKey;
            headers[count]._nameLen = keyLen;
            headers[count]._value = pValue;
            headers[count]._valueLen = pCur->valueLen;
            ++count;
            if ( count == 512 )
                break;
            //ret = (*fn)( pKey, keyLen,
            //            pValue, pCur->valueLen, arg );
            //if ( ret <= 0 )
            //    return ret;
            ++pCur;
        }
    }
    qsort( headers, count, sizeof( struct _headerInfo ), compareValueLocation );
    for( i = 0; i < count; ++i )
    {
        ret = (*fn)( headers[i]._name, headers[i]._nameLen,
                    headers[i]._value, headers[i]._valueLen, arg );
        if ( ret <= 0 )
            return ret;
    }
    return count;
}


int LSAPI_ForeachHeader_r( LSAPI_Request * pReq,
            LSAPI_CB_EnvHandler fn, void * arg )
{
    int i;
    int len = 0;
    char * pValue;
    int ret;
    int count = 0;
    if ( !pReq || !fn )
        return -1;
    for( i = 0; i < H_TRANSFER_ENCODING; ++i )
    {
        if ( pReq->m_pHeaderIndex->m_headerOff[i] )
        {
            len = pReq->m_pHeaderIndex->m_headerLen[i];
            pValue = pReq->m_pHttpHeader + pReq->m_pHeaderIndex->m_headerOff[i];
            *(pValue + len ) = 0;
            ret = (*fn)( CGI_HEADERS[i], CGI_HEADER_LEN[i],
                        pValue, len, arg );
            ++count;
            if ( ret <= 0 )
                return ret;
        }
    }
    if ( pReq->m_pHeader->m_cntUnknownHeaders > 0 )
    {
        char achHeaderName[256];
        char *p;
        char *pKey;
        char *pKeyEnd ;
        int  keyLen;
        struct lsapi_header_offset * pCur, *pEnd;
        pCur = pReq->m_pUnknownHeader;
        pEnd = pCur + pReq->m_pHeader->m_cntUnknownHeaders;
        while( pCur < pEnd )
        {
            pKey = pReq->m_pHttpHeader + pCur->nameOff;
            keyLen = pCur->nameLen;
            if ( keyLen > 250 )
                keyLen = 250;
            pKeyEnd = pKey + keyLen;
            memcpy( achHeaderName, "HTTP_", 5 );
            p = &achHeaderName[5];

            while( pKey < pKeyEnd )
            {
                char ch = *pKey++;
                if ( ch == '-' )
                    *p++ = '_';
                else
                    *p++ = toupper( ch );
            }
            *p = 0;
            keyLen += 5;

            pValue = pReq->m_pHttpHeader + pCur->valueOff;
            *(pValue + pCur->valueLen ) = 0;
            ret = (*fn)( achHeaderName, keyLen,
                        pValue, pCur->valueLen, arg );
            if ( ret <= 0 )
                return ret;
            ++pCur;
        }
    }
    return count + pReq->m_pHeader->m_cntUnknownHeaders;
}


static int EnvForeach( struct LSAPI_key_value_pair * pEnv,
            int n, LSAPI_CB_EnvHandler fn, void * arg )
{
    struct LSAPI_key_value_pair * pEnd = pEnv + n;
    int ret;
    if ( !pEnv || !fn )
        return -1;
    while( pEnv < pEnd )
    {
        ret = (*fn)( pEnv->pKey, pEnv->keyLen,
                    pEnv->pValue, pEnv->valLen, arg );
        if ( ret <= 0 )
            return ret;
        ++pEnv;
    }
    return n;
}


int LSAPI_ForeachEnv_r( LSAPI_Request * pReq,
            LSAPI_CB_EnvHandler fn, void * arg )
{
    if ( !pReq || !fn )
        return -1;
    if ( pReq->m_pHeader->m_cntEnv > 0 )
    {
        return EnvForeach( pReq->m_pEnvList, pReq->m_pHeader->m_cntEnv,
                    fn, arg );
    }
    return 0;
}


int LSAPI_ForeachSpecialEnv_r( LSAPI_Request * pReq,
            LSAPI_CB_EnvHandler fn, void * arg )
{
    if ( !pReq || !fn )
        return -1;
    if ( pReq->m_pHeader->m_cntSpecialEnv > 0 )
    {
        return EnvForeach( pReq->m_pSpecialEnvList,
                pReq->m_pHeader->m_cntSpecialEnv,
                    fn, arg );
    }
    return 0;

}


int LSAPI_FinalizeRespHeaders_r( LSAPI_Request * pReq )
{
    if ( !pReq || !pReq->m_pIovec )
        return -1;
    if ( !( pReq->m_reqState & LSAPI_ST_RESP_HEADER ) )
        return 0;
    pReq->m_reqState &= ~LSAPI_ST_RESP_HEADER;
    if ( pReq->m_pRespHeaderBufPos > pReq->m_pRespHeaderBuf )
    {
        pReq->m_pIovecCur->iov_base = (void *)pReq->m_pRespHeaderBuf;
        pReq->m_pIovecCur->iov_len  = pReq->m_pRespHeaderBufPos - pReq->m_pRespHeaderBuf;
        pReq->m_totalLen += pReq->m_pIovecCur->iov_len;
        ++pReq->m_pIovecCur;
    }

    pReq->m_pIovec->iov_len  = sizeof( struct lsapi_resp_header)
            + pReq->m_respHeader.m_respInfo.m_cntHeaders * sizeof( short );
    pReq->m_totalLen += pReq->m_pIovec->iov_len;

    lsapi_buildPacketHeader( &pReq->m_respHeader.m_pktHeader,
                    LSAPI_RESP_HEADER, pReq->m_totalLen  );
    pReq->m_pIovec->iov_base = (void *)&pReq->m_respHeader;
    pReq->m_pIovecToWrite = pReq->m_pIovec;
    return 0;
}


int LSAPI_AppendRespHeader2_r( LSAPI_Request * pReq, const char * pHeaderName,
                              const char * pHeaderValue )
{
    int nameLen, valLen, len;
    if ( !pReq || !pHeaderName || !pHeaderValue )
        return -1;
    if ( pReq->m_reqState & LSAPI_ST_RESP_BODY )
        return -1;
    if ( pReq->m_respHeader.m_respInfo.m_cntHeaders >= LSAPI_MAX_RESP_HEADERS )
        return -1;
    nameLen = strlen( pHeaderName );
    valLen = strlen( pHeaderValue );
    if ( nameLen == 0 )
        return -1;
    while( nameLen > 0 )
    {
        char ch = *(pHeaderName + nameLen - 1 );
        if (( ch == '\n' )||( ch == '\r' ))
            --nameLen;
        else
            break;
    }
    if ( nameLen <= 0 )
        return 0;
    while( valLen > 0 )
    {
        char ch = *(pHeaderValue + valLen - 1 );
        if (( ch == '\n' )||( ch == '\r' ))
            --valLen;
        else
            break;
    }
    len = nameLen + valLen + 1;
    if ( len > LSAPI_RESP_HTTP_HEADER_MAX )
        return -1;

    if ( pReq->m_pRespHeaderBufPos + len + 1 > pReq->m_pRespHeaderBufEnd )
    {
        int newlen = pReq->m_pRespHeaderBufPos + len + 4096 - pReq->m_pRespHeaderBuf;
        newlen -= newlen % 4096;
        if ( allocateRespHeaderBuf( pReq, newlen ) == -1 )
            return -1;
    }
    memmove( pReq->m_pRespHeaderBufPos, pHeaderName, nameLen );
    pReq->m_pRespHeaderBufPos += nameLen;
    *pReq->m_pRespHeaderBufPos++ = ':';
    memmove( pReq->m_pRespHeaderBufPos, pHeaderValue, valLen );
    pReq->m_pRespHeaderBufPos += valLen;
    *pReq->m_pRespHeaderBufPos++ = 0;
    ++len;  /* add one byte padding for \0 */
    pReq->m_respHeaderLen[pReq->m_respHeader.m_respInfo.m_cntHeaders] = len;
    ++pReq->m_respHeader.m_respInfo.m_cntHeaders;
    return 0;
}


int LSAPI_AppendRespHeader_r( LSAPI_Request * pReq, const char * pBuf, int len )
{
    if ( !pReq || !pBuf || len <= 0 || len > LSAPI_RESP_HTTP_HEADER_MAX )
        return -1;
    if ( pReq->m_reqState & LSAPI_ST_RESP_BODY )
        return -1;
    if ( pReq->m_respHeader.m_respInfo.m_cntHeaders >= LSAPI_MAX_RESP_HEADERS )
        return -1;
    while( len > 0 )
    {
        char ch = *(pBuf + len - 1 );
        if (( ch == '\n' )||( ch == '\r' ))
            --len;
        else
            break;
    }
    if ( len <= 0 )
        return 0;
    if ( pReq->m_pRespHeaderBufPos + len + 1 > pReq->m_pRespHeaderBufEnd )
    {
        int newlen = pReq->m_pRespHeaderBufPos + len + 4096 - pReq->m_pRespHeaderBuf;
        newlen -= newlen % 4096;
        if ( allocateRespHeaderBuf( pReq, newlen ) == -1 )
            return -1;
    }
    memmove( pReq->m_pRespHeaderBufPos, pBuf, len );
    pReq->m_pRespHeaderBufPos += len;
    *pReq->m_pRespHeaderBufPos++ = 0;
    ++len;  /* add one byte padding for \0 */
    pReq->m_respHeaderLen[pReq->m_respHeader.m_respInfo.m_cntHeaders] = len;
    ++pReq->m_respHeader.m_respInfo.m_cntHeaders;
    return 0;
}


int LSAPI_CreateListenSock2( const struct sockaddr * pServerAddr, int backlog )
{
    int ret;
    int fd;
    int flag = 1;
    int addr_len;

    switch( pServerAddr->sa_family )
    {
    case AF_INET:
        addr_len = 16;
        break;
    case AF_INET6:
        addr_len = sizeof( struct sockaddr_in6 );
        break;
    case AF_UNIX:
        addr_len = sizeof( struct sockaddr_un );
        unlink( ((struct sockaddr_un *)pServerAddr)->sun_path );
        break;
    default:
        return -1;
    }

    fd = socket( pServerAddr->sa_family, SOCK_STREAM, 0 );
    if ( fd == -1 )
        return -1;

    fcntl( fd, F_SETFD, FD_CLOEXEC );

    if(setsockopt( fd, SOL_SOCKET, SO_REUSEADDR,
                (char *)( &flag ), sizeof(flag)) == 0)
    {
        ret = bind( fd, pServerAddr, addr_len );
        if ( !ret )
        {
            ret = listen( fd, backlog );
            if ( !ret )
                return fd;
        }
    }

    ret = errno;
    close(fd);
    errno = ret;
    return -1;
}


int LSAPI_ParseSockAddr( const char * pBind, struct sockaddr * pAddr )
{
    char achAddr[256];
    char * p = achAddr;
    char * pEnd;
    struct addrinfo *res, hints;
    int  doAddrInfo = 0;
    int port;

    if ( !pBind )
        return -1;

    while( isspace( *pBind ) )
        ++pBind;

    strncpy(achAddr, pBind, 255);
    achAddr[255] = 0;

    switch( *p )
    {
    case '/':
        pAddr->sa_family = AF_UNIX;
        strncpy( ((struct sockaddr_un *)pAddr)->sun_path, p,
                sizeof(((struct sockaddr_un *)pAddr)->sun_path) );
        return 0;

    case '[':
        pAddr->sa_family = AF_INET6;
        ++p;
        pEnd = strchr( p, ']' );
        if ( !pEnd )
            return -1;
        *pEnd++ = 0;

        if ( *p == '*' )
        {
            strcpy( achAddr, "::" );
            p = achAddr;
        }
        doAddrInfo = 1;
        break;

    default:
        pAddr->sa_family = AF_INET;
        pEnd = strchr( p, ':' );
        if ( !pEnd )
            return -1;
        *pEnd++ = 0;

        doAddrInfo = 0;
        if ( *p == '*' )
        {
            ((struct sockaddr_in *)pAddr)->sin_addr.s_addr = htonl(INADDR_ANY);
        }
        else if (!strcasecmp( p, "localhost" ) )
            ((struct sockaddr_in *)pAddr)->sin_addr.s_addr = htonl( INADDR_LOOPBACK );
        else
        {
#ifdef HAVE_INET_PTON
            if (!inet_pton(AF_INET, p, &((struct sockaddr_in *)pAddr)->sin_addr))
#else
            ((struct sockaddr_in *)pAddr)->sin_addr.s_addr = inet_addr( p );
            if ( ((struct sockaddr_in *)pAddr)->sin_addr.s_addr == INADDR_BROADCAST)
#endif
            {
                doAddrInfo = 1;
            }
        }
        break;
    }
    if ( *pEnd == ':' )
        ++pEnd;

    port = atoi( pEnd );
    if (( port <= 0 )||( port > 65535 ))
        return -1;
    if ( doAddrInfo )
    {

        memset(&hints, 0, sizeof(hints));

        hints.ai_family   = pAddr->sa_family;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        if ( getaddrinfo(p, NULL, &hints, &res) )
        {
            return -1;
        }

        memcpy(pAddr, res->ai_addr, res->ai_addrlen);
        freeaddrinfo(res);
    }

    if ( pAddr->sa_family == AF_INET )
        ((struct sockaddr_in *)pAddr)->sin_port = htons( port );
    else
        ((struct sockaddr_in6 *)pAddr)->sin6_port = htons( port );
    return 0;

}


int LSAPI_CreateListenSock( const char * pBind, int backlog )
{
    char serverAddr[128];
    int ret;
    int fd = -1;
    ret = LSAPI_ParseSockAddr( pBind, (struct sockaddr *)serverAddr );
    if ( !ret )
    {
        fd = LSAPI_CreateListenSock2( (struct sockaddr *)serverAddr, backlog );
    }
    return fd;
}


static fn_select_t g_fnSelect = select;
typedef struct _lsapi_prefork_server
{
    int m_fd;
    int m_iMaxChildren;
    int m_iExtraChildren;
    int m_iCurChildren;
    int m_iMaxIdleChildren;
    int m_iServerMaxIdle;
    int m_iChildrenMaxIdleTime;
    int m_iMaxReqProcessTime;
    int m_iAvoidFork;

    lsapi_child_status * m_pChildrenStatus;
    lsapi_child_status * m_pChildrenStatusCur;
    lsapi_child_status * m_pChildrenStatusEnd;

}lsapi_prefork_server;
static lsapi_prefork_server * g_prefork_server = NULL;


int LSAPI_Init_Prefork_Server( int max_children, fn_select_t fp, int avoidFork )
{
    if ( g_prefork_server )
        return 0;
    if ( max_children <= 1 )
        return -1;
    if ( max_children >= 10000)
        max_children = 10000;

    if (s_max_busy_workers == 0)
        s_max_busy_workers = max_children / 2 + 1;

    g_prefork_server = (lsapi_prefork_server *)malloc( sizeof( lsapi_prefork_server ) );
    if ( !g_prefork_server )
        return -1;
    memset( g_prefork_server, 0, sizeof( lsapi_prefork_server ) );

    if ( fp != NULL )
        g_fnSelect = fp;

    s_ppid = getppid();
    s_pid = getpid();
    setpgid( s_pid, s_pid );
#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__gnu_linux__)
    s_total_pages = sysconf(_SC_PHYS_PAGES);
#endif
    g_prefork_server->m_iAvoidFork = avoidFork;
    g_prefork_server->m_iMaxChildren = max_children;

    g_prefork_server->m_iExtraChildren = ( avoidFork ) ? 0 : (max_children / 3) ;
    g_prefork_server->m_iMaxIdleChildren = ( avoidFork ) ? (max_children + 1) : (max_children / 3);
    if ( g_prefork_server->m_iMaxIdleChildren == 0 )
        g_prefork_server->m_iMaxIdleChildren = 1;
    g_prefork_server->m_iChildrenMaxIdleTime = 300;
    g_prefork_server->m_iMaxReqProcessTime = 3600;

    setsid();

    return 0;
}


void LSAPI_Set_Server_fd( int fd )
{
    if( g_prefork_server )
        g_prefork_server->m_fd = fd;
}


static int lsapi_accept( int fdListen )
{
    int         fd;
    int         nodelay = 1;
    socklen_t   len;
    char        achPeer[128];

    len = sizeof( achPeer );
    fd = accept( fdListen, (struct sockaddr *)&achPeer, &len );
    if ( fd != -1 )
    {
        if (((struct sockaddr *)&achPeer)->sa_family == AF_INET )
        {
            setsockopt( fd, IPPROTO_TCP, TCP_NODELAY,
                    (char *)&nodelay, sizeof(nodelay));
        }

        //OPTIMIZATION
        //if ( s_accept_notify )
        //    notify_req_received( fd );
    }
    return fd;

}


static unsigned int s_max_reqs = UINT_MAX;
static int s_max_idle_secs = 300;
static int s_stop;

static void lsapi_cleanup(int signal)
{
    s_stop = signal;
}


static lsapi_child_status * find_child_status( int pid )
{
    lsapi_child_status * pStatus = g_prefork_server->m_pChildrenStatus;
    lsapi_child_status * pEnd = g_prefork_server->m_pChildrenStatusEnd;
    while( pStatus < pEnd )
    {
        if ( pStatus->m_pid == pid )
        {
            if (pid == 0)
            {
                memset(pStatus, 0, sizeof( *pStatus ) );
                pStatus->m_pid = -1;
            }
            if ( pStatus + 1 > g_prefork_server->m_pChildrenStatusCur )
                g_prefork_server->m_pChildrenStatusCur = pStatus + 1;
            return pStatus;
        }
        ++pStatus;
    }
    return NULL;
}


void LSAPI_reset_server_state( void )
{
    /*
       Reset child status
    */
    g_prefork_server->m_iCurChildren = 0;
    lsapi_child_status * pStatus = g_prefork_server->m_pChildrenStatus;
    lsapi_child_status * pEnd = g_prefork_server->m_pChildrenStatusEnd;
    while( pStatus < pEnd )
    {
        pStatus->m_pid = 0;
        ++pStatus;
    }
    if (s_busy_workers)
        __atomic_store_n(s_busy_workers, 0, __ATOMIC_SEQ_CST);
    if (s_accepting_workers)
        __atomic_store_n(s_accepting_workers, 0, __ATOMIC_SEQ_CST);

}


static void lsapi_sigchild( int signal )
{
    int status, pid;
    char expect_connected = LSAPI_STATE_CONNECTED;
    char expect_accepting = LSAPI_STATE_ACCEPTING;
    lsapi_child_status * child_status;
    if (g_prefork_server == NULL)
        return;
    while( 1 )
    {
        pid = waitpid( -1, &status, WNOHANG|WUNTRACED );
        if ( pid <= 0 )
        {
            break;
        }
        if ( WIFSIGNALED( status ))
        {
            int sig_num = WTERMSIG( status );

#ifdef WCOREDUMP
            const char * dump = WCOREDUMP( status ) ? "yes" : "no";
#else
            const char * dump = "unknown";
#endif
            lsapi_log("Child process with pid: %d was killed by signal: "
                     "%d, core dumped: %s\n", pid, sig_num, dump );
        }
        if ( pid == s_pid_dump_debug_info )
        {
            pid = 0;
            continue;
        }
        if ( pid == s_ignore_pid )
        {
            pid = 0;
            s_ignore_pid = -1;
            continue;
        }
        child_status = find_child_status( pid );
        if ( child_status )
        {
            if (__atomic_compare_exchange_n(&child_status->m_state,
                                            &expect_connected,
                                            LSAPI_STATE_IDLE, 1,
                                            __ATOMIC_SEQ_CST,
                                            __ATOMIC_SEQ_CST))
            {
                if (s_busy_workers)
                    __atomic_fetch_sub(s_busy_workers, 1, __ATOMIC_SEQ_CST);
            }
            else if (__atomic_compare_exchange_n(&child_status->m_state,
                                                 &expect_accepting,
                                                 LSAPI_STATE_IDLE, 1,
                                                 __ATOMIC_SEQ_CST,
                                                 __ATOMIC_SEQ_CST))
            {
                if (s_accepting_workers)
                    __atomic_fetch_sub(s_accepting_workers, 1, __ATOMIC_SEQ_CST);
            }
            child_status->m_pid = 0;
            --g_prefork_server->m_iCurChildren;
        }
    }
    while(( g_prefork_server->m_pChildrenStatusCur > g_prefork_server->m_pChildrenStatus )
            &&( g_prefork_server->m_pChildrenStatusCur[-1].m_pid == 0 ))
        --g_prefork_server->m_pChildrenStatusCur;

}


static int lsapi_init_children_status(void)
{
    char * pBuf;
    int size = 4096;
    int max_children;
    if (g_prefork_server->m_pChildrenStatus)
        return 0;
    max_children = g_prefork_server->m_iMaxChildren
                        + g_prefork_server->m_iExtraChildren;

    size = max_children * sizeof( lsapi_child_status ) * 2 + 3 * sizeof(int);
    size = (size + 4095) / 4096 * 4096;
    pBuf =( char*) mmap( NULL, size, PROT_READ | PROT_WRITE,
        MAP_ANON | MAP_SHARED, -1, 0 );
    if ( pBuf == MAP_FAILED )
    {
        perror( "Anonymous mmap() failed" );
        return -1;
    }
    memset( pBuf, 0, size );
    g_prefork_server->m_pChildrenStatus = (lsapi_child_status *)pBuf;
    g_prefork_server->m_pChildrenStatusCur = (lsapi_child_status *)pBuf;
    g_prefork_server->m_pChildrenStatusEnd = (lsapi_child_status *)pBuf + max_children;
    s_busy_workers = (int *)g_prefork_server->m_pChildrenStatusEnd;
    s_accepting_workers = s_busy_workers + 1;
    s_global_counter = s_accepting_workers + 1;
    s_avail_pages = (size_t *)(s_global_counter + 1);

    setsid();
    return 0;
}


static void dump_debug_info( lsapi_child_status * pStatus, long tmCur )
{
    char achCmd[1024];
    if ( s_pid_dump_debug_info )
    {
        if ( kill( s_pid_dump_debug_info, 0 ) == 0 )
            return;
    }

    lsapi_log("Possible runaway process, UID: %d, PPID: %d, PID: %d, "
             "reqCount: %d, process time: %ld, checkpoint time: %ld, start "
             "time: %ld\n", getuid(), getppid(), pStatus->m_pid,
             pStatus->m_iReqCounter, tmCur - pStatus->m_tmReqBegin,
             tmCur - pStatus->m_tmLastCheckPoint, tmCur - pStatus->m_tmStart );

    s_pid_dump_debug_info = fork();
    if (s_pid_dump_debug_info == 0)
    {
        snprintf( achCmd, 1024, "gdb --batch -ex \"attach %d\" -ex \"set height 0\" "
                "-ex \"bt\" >&2;PATH=$PATH:/usr/sbin lsof -p %d >&2",
                pStatus->m_pid, pStatus->m_pid );
        if ( system( achCmd ) == -1 )
            perror( "system()" );
        exit( 0 );
    }
}


static void lsapi_check_child_status( long tmCur )
{
    int idle = 0;
    int tobekilled;
    int dying = 0;
    int count = 0;
    lsapi_child_status * pStatus = g_prefork_server->m_pChildrenStatus;
    lsapi_child_status * pEnd = g_prefork_server->m_pChildrenStatusCur;
    while( pStatus < pEnd )
    {
        tobekilled = 0;
        if ( pStatus->m_pid != 0 && pStatus->m_pid != -1)
        {
            ++count;
            if ( !pStatus->m_inProcess )
            {
                if (g_prefork_server->m_iCurChildren - dying
                        > g_prefork_server->m_iMaxChildren
                    || idle > g_prefork_server->m_iMaxIdleChildren)
                {
                    ++pStatus->m_iKillSent;
                    //tobekilled = SIGUSR1;
                }
                else
                {
                    if (s_max_idle_secs> 0
                        && tmCur - pStatus->m_tmWaitBegin > s_max_idle_secs + 5)
                    {
                        ++pStatus->m_iKillSent;
                        //tobekilled = SIGUSR1;
                    }
                }
                if (!pStatus->m_iKillSent)
                    ++idle;
            }
            else
            {
                if (tmCur - pStatus->m_tmReqBegin >
                        g_prefork_server->m_iMaxReqProcessTime)
                {
                    if ((pStatus->m_iKillSent % 5) == 0 && s_dump_debug_info)
                        dump_debug_info( pStatus, tmCur );
                    if ( pStatus->m_iKillSent > 5 )
                    {
                        tobekilled = SIGKILL;
                        lsapi_log("Force killing runaway process PID: %d"
                                 " with SIGKILL\n", pStatus->m_pid );
                    }
                    else
                    {
                        tobekilled = SIGTERM;
                        lsapi_log("Killing runaway process PID: %d with "
                                 "SIGTERM\n", pStatus->m_pid );
                    }
                }
            }
            if ( tobekilled )
            {
                if (( kill( pStatus->m_pid, tobekilled ) == -1 ) &&
                    ( errno == ESRCH ))
                {
                    pStatus->m_pid = 0;
                    --count;
                }
                else
                {
                    ++pStatus->m_iKillSent;
                    ++dying;
                }
            }
        }
        ++pStatus;
    }
    if ( abs( g_prefork_server->m_iCurChildren - count ) > 1 )
    {
        lsapi_log("Children tracking is wrong: Cur Children: %d,"
                  " count: %d, idle: %d, dying: %d\n",
                  g_prefork_server->m_iCurChildren, count, idle, dying );
    }
}


//static int lsapi_all_children_must_die(void)
//{
//    int maxWait;
//    int sec =0;
//    g_prefork_server->m_iMaxReqProcessTime = 10;
//    g_prefork_server->m_iMaxIdleChildren = -1;
//    maxWait = 15;
//
//    while( g_prefork_server->m_iCurChildren && (sec < maxWait) )
//    {
//        lsapi_check_child_status(time(NULL));
//        sleep( 1 );
//        sec++;
//    }
//    if ( g_prefork_server->m_iCurChildren != 0 )
//        kill( -getpgrp(), SIGKILL );
//    return 0;
//}


void set_skip_write(void)
{   s_skip_write = 1;   }


int is_enough_free_mem(void)
{
#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__gnu_linux__)
    //minimum 1GB or 10% available free memory
    return (*s_avail_pages > s_min_avail_pages
            || (*s_avail_pages * 10) / s_total_pages > 0);
#endif
    return 1;
}


static int lsapi_prefork_server_accept( lsapi_prefork_server * pServer,
                                        LSAPI_Request * pReq )
{
    struct sigaction act, old_term, old_quit, old_int,
                    old_usr1, old_child;
    lsapi_child_status * child_status;
    int             wait_secs = 0;
    int             ret = 0;
    int             pid;
    time_t          lastTime = 0;
    time_t          curTime = 0;
    fd_set          readfds;
    struct timeval  timeout;

    sigset_t mask;
    sigset_t orig_mask;

    lsapi_init_children_status();

    act.sa_flags = 0;
    act.sa_handler = lsapi_sigchild;
    sigemptyset(&(act.sa_mask));
    if( sigaction( SIGCHLD, &act, &old_child ) )
    {
        perror( "Can't set signal handler for SIGCHILD" );
        return -1;
    }

    /* Set up handler to kill children upon exit */
    act.sa_flags = 0;
    act.sa_handler = lsapi_cleanup;
    sigemptyset(&(act.sa_mask));
    if( sigaction( SIGTERM, &act, &old_term ) ||
        sigaction( SIGINT,  &act, &old_int  ) ||
        sigaction( SIGUSR1, &act, &old_usr1 ) ||
        sigaction( SIGQUIT, &act, &old_quit ))
    {
        perror( "Can't set signals" );
        return -1;
    }

    while( !s_stop )
    {
        if (s_proc_group_timer_cb != NULL) {
            s_proc_group_timer_cb(&s_ignore_pid);
        }

        curTime = time( NULL );
        if (curTime != lastTime )
        {
            lastTime = curTime;
            if (lsapi_parent_dead())
                break;
            lsapi_check_child_status(curTime );
            if (pServer->m_iServerMaxIdle)
            {
                if ( pServer->m_iCurChildren <= 0 )
                {
                    ++wait_secs;
                    if ( wait_secs > pServer->m_iServerMaxIdle )
                        return -1;
                }
                else
                    wait_secs = 0;
            }
        }

#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__gnu_linux__)
        *s_avail_pages = sysconf(_SC_AVPHYS_PAGES);
//        lsapi_log("Memory total: %zd, free: %zd, free %%%zd\n",
//                  s_total_pages, *s_avail_pages, *s_avail_pages * 100 / s_total_pages);

#endif
        FD_ZERO( &readfds );
        FD_SET( pServer->m_fd, &readfds );
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        ret = (*g_fnSelect)(pServer->m_fd+1, &readfds, NULL, NULL, &timeout);
        if (ret == 1 )
        {
            int accepting = 0;
            if (s_accepting_workers)
                accepting = __atomic_load_n(s_accepting_workers, __ATOMIC_SEQ_CST);

            if (pServer->m_iCurChildren > 0
                && accepting > 0)
            {
                usleep(400);
                while(accepting-- > 0)
                    sched_yield();
                continue;
            }
        }
        else if ( ret == -1 )
        {
            if ( errno == EINTR )
                continue;
            /* perror( "select()" ); */
            break;
        }
        else
        {
            continue;
        }

        if (pServer->m_iCurChildren >=
            pServer->m_iMaxChildren + pServer->m_iExtraChildren)
        {
            lsapi_log("Reached max children process limit: %d, extra: %d,"
                     " current: %d, busy: %d, please increase LSAPI_CHILDREN.\n",
                     pServer->m_iMaxChildren, pServer->m_iExtraChildren,
                     pServer->m_iCurChildren,
                     s_busy_workers ? *s_busy_workers : -1 );
            usleep( 100000 );
            continue;
        }

        pReq->m_fd = lsapi_accept( pServer->m_fd );
        if ( pReq->m_fd != -1 )
        {
            wait_secs = 0;
            child_status = find_child_status( 0 );

            sigemptyset( &mask );
            sigaddset( &mask, SIGCHLD );

            if ( sigprocmask(SIG_BLOCK, &mask, &orig_mask) < 0 )
            {
                perror( "sigprocmask(SIG_BLOCK) to block SIGCHLD" );
            }

            pid = fork();

            if ( !pid )
            {
                setsid();
                if (sigprocmask(SIG_SETMASK, &orig_mask, NULL) < 0)
                    perror( "sigprocmask( SIG_SETMASK ) to restore SIGMASK in child" );
                g_prefork_server = NULL;
                s_ppid = getppid();
                s_pid = getpid();
                s_req_processed = 0;
                s_proc_group_timer_cb = NULL;
                s_worker_status = child_status;

                if (pthread_atfork_func)
                    (*pthread_atfork_func)(NULL, NULL, set_skip_write);

                __atomic_store_n(&s_worker_status->m_state,
                                 LSAPI_STATE_CONNECTED, __ATOMIC_SEQ_CST);
                if (s_busy_workers)
                    __atomic_add_fetch(s_busy_workers, 1, __ATOMIC_SEQ_CST);
                lsapi_set_nblock( pReq->m_fd, 0 );
                //keep it open if busy_count is used.
                if (!s_keep_listener && s_busy_workers
                    && *s_busy_workers > (pServer->m_iMaxChildren >> 1))
                    s_keep_listener = 1;
                if ((s_uid == 0 || !s_keep_listener || !is_enough_free_mem())
                    && pReq->m_fdListen != -1 )
                {
                    close( pReq->m_fdListen );
                    pReq->m_fdListen = -1;
                }
                /* don't catch our signals */
                sigaction( SIGCHLD, &old_child, 0 );
                sigaction( SIGTERM, &old_term, 0 );
                sigaction( SIGQUIT, &old_quit, 0 );
                sigaction( SIGINT,  &old_int,  0 );
                sigaction( SIGUSR1, &old_usr1, 0 );
                //init_conn_key( pReq->m_fd );
                lsapi_notify_pid( pReq->m_fd );
                s_notified_pid = 1;
                //if ( s_accept_notify )
                //    return notify_req_received( pReq->m_fd );
                return 0;
            }
            else if ( pid == -1 )
            {
                lsapi_perror("fork() failed, please increase process limit", errno);
                if (child_status)
                    child_status->m_pid = 0;
            }
            else
            {
                ++pServer->m_iCurChildren;
                if ( child_status )
                {
                    child_status->m_pid = pid;
                    child_status->m_tmWaitBegin = curTime;
                    child_status->m_tmStart = curTime;
                }
            }
            close( pReq->m_fd );
            pReq->m_fd = -1;

            if (sigprocmask(SIG_SETMASK, &orig_mask, NULL) < 0)
                perror( "sigprocmask( SIG_SETMASK ) to restore SIGMASK" );

        }
        else
        {
            if (( errno == EINTR )||( errno == EAGAIN))
                continue;
            lsapi_perror("accept() failed", errno);
            return -1;
        }
    }
    sigaction( SIGUSR1, &old_usr1, 0 );
    //kill( -getpgrp(), SIGUSR1 );
    //lsapi_all_children_must_die();  /* Sorry, children ;-) */
    return -1;

}


static struct sigaction old_term, old_quit, old_int,
                    old_usr1, old_child;


int LSAPI_Postfork_Child(LSAPI_Request * pReq)
{
    int max_children = g_prefork_server->m_iMaxChildren;
    s_pid = getpid();
    __atomic_store_n(&pReq->child_status->m_pid, s_pid, __ATOMIC_SEQ_CST);
    s_worker_status = pReq->child_status;

    setsid();
    g_prefork_server = NULL;
    s_ppid = getppid();
    s_req_processed = 0;
    s_proc_group_timer_cb = NULL;

    if (pthread_atfork_func)
        (*pthread_atfork_func)(NULL, NULL, set_skip_write);

    __atomic_store_n(&s_worker_status->m_state,
                     LSAPI_STATE_CONNECTED, __ATOMIC_SEQ_CST);
    if (s_busy_workers)
        __atomic_add_fetch(s_busy_workers, 1, __ATOMIC_SEQ_CST);
    lsapi_set_nblock( pReq->m_fd, 0 );
    //keep it open if busy_count is used.
    if (!s_keep_listener && s_busy_workers
        && *s_busy_workers > (max_children >> 1))
        s_keep_listener = 1;
    if ((s_uid == 0 || !s_keep_listener || !is_enough_free_mem())
        && pReq->m_fdListen != -1 )
    {
        close(pReq->m_fdListen);
        pReq->m_fdListen = -1;
    }

    //init_conn_key( pReq->m_fd );
    lsapi_notify_pid(pReq->m_fd);
    s_notified_pid = 1;
    //if ( s_accept_notify )
    //    return notify_req_received( pReq->m_fd );
    return 0;
}


int LSAPI_Postfork_Parent(LSAPI_Request * pReq)
{
    ++g_prefork_server->m_iCurChildren;
    if (pReq->child_status)
    {
        time_t curTime = time( NULL );
        pReq->child_status->m_tmWaitBegin = curTime;
        pReq->child_status->m_tmStart = curTime;
    }
    close(pReq->m_fd);
    pReq->m_fd = -1;
    return 0;
}


int LSAPI_Accept_Before_Fork(LSAPI_Request * pReq)
{
    time_t          lastTime = 0;
    time_t          curTime = 0;
    fd_set          readfds;
    struct timeval  timeout;
    int             wait_secs = 0;
    int             ret = 0;

    lsapi_prefork_server * pServer = g_prefork_server;

    struct sigaction act;

    lsapi_init_children_status();

    act.sa_flags = 0;
    act.sa_handler = lsapi_sigchild;
    sigemptyset(&(act.sa_mask));
    if (sigaction(SIGCHLD, &act, &old_child))
    {
        perror( "Can't set signal handler for SIGCHILD" );
        return -1;
    }

    /* Set up handler to kill children upon exit */
    act.sa_flags = 0;
    act.sa_handler = lsapi_cleanup;
    sigemptyset(&(act.sa_mask));
    if (sigaction(SIGTERM, &act, &old_term) ||
        sigaction(SIGINT,  &act, &old_int ) ||
        sigaction(SIGUSR1, &act, &old_usr1) ||
        sigaction(SIGQUIT, &act, &old_quit))
    {
        perror( "Can't set signals" );
        return -1;
    }
    s_stop = 0;
    pReq->m_reqState = 0;

    while(!s_stop)
    {
        if (s_proc_group_timer_cb != NULL) {
            s_proc_group_timer_cb(&s_ignore_pid);
        }

        curTime = time(NULL);
        if (curTime != lastTime)
        {
            lastTime = curTime;
            if (lsapi_parent_dead())
                break;
            lsapi_check_child_status(curTime);
            if (pServer->m_iServerMaxIdle)
            {
                if (pServer->m_iCurChildren <= 0)
                {
                    ++wait_secs;
                    if ( wait_secs > pServer->m_iServerMaxIdle )
                        return -1;
                }
                else
                    wait_secs = 0;
            }
        }

#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__gnu_linux__)
        *s_avail_pages = sysconf(_SC_AVPHYS_PAGES);
//        lsapi_log("Memory total: %zd, free: %zd, free %%%zd\n",
//                  s_total_pages, *s_avail_pages, *s_avail_pages * 100 / s_total_pages);

#endif
        FD_ZERO(&readfds);
        FD_SET(pServer->m_fd, &readfds);
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        ret = (*g_fnSelect)(pServer->m_fd+1, &readfds, NULL, NULL, &timeout);
        if (ret == 1 )
        {
            int accepting = 0;
            if (s_accepting_workers)
                accepting = __atomic_load_n(s_accepting_workers, __ATOMIC_SEQ_CST);

            if (pServer->m_iCurChildren > 0
                && accepting > 0)
            {
                usleep( 400);
                while(accepting-- > 0)
                    sched_yield();
                continue;
            }
        }
        else if (ret == -1)
        {
            if (errno == EINTR)
                continue;
            /* perror( "select()" ); */
            break;
        }
        else
        {
            continue;
        }

        if (pServer->m_iCurChildren >=
            pServer->m_iMaxChildren + pServer->m_iExtraChildren)
        {
            lsapi_log("Reached max children process limit: %d, extra: %d,"
                     " current: %d, busy: %d, please increase LSAPI_CHILDREN.\n",
                     pServer->m_iMaxChildren, pServer->m_iExtraChildren,
                     pServer->m_iCurChildren,
                     s_busy_workers ? *s_busy_workers : -1);
            usleep(100000);
            continue;
        }

        pReq->m_fd = lsapi_accept(pServer->m_fd);
        if (pReq->m_fd != -1)
        {
            wait_secs = 0;
            pReq->child_status = find_child_status(0);

            ret = 0;
            break;
        }
        else
        {
            if ((errno == EINTR) || (errno == EAGAIN))
                continue;
            lsapi_perror("accept() failed", errno);
            ret = -1;
            break;
        }
    }

    sigaction(SIGCHLD, &old_child, 0);
    sigaction(SIGTERM, &old_term, 0);
    sigaction(SIGQUIT, &old_quit, 0);
    sigaction(SIGINT,  &old_int,  0);
    sigaction(SIGUSR1, &old_usr1, 0);

    return ret;
}


int LSAPI_Prefork_Accept_r( LSAPI_Request * pReq )
{
    int             fd;
    int             ret;
    int             wait_secs;
    fd_set          readfds;
    struct timeval  timeout;

    if (s_skip_write)
        return -1;

    LSAPI_Finish_r( pReq );

    if ( g_prefork_server )
    {
        if ( g_prefork_server->m_fd != -1 )
            if ( lsapi_prefork_server_accept( g_prefork_server, pReq ) == -1 )
                return -1;
    }
    else if (s_req_processed > 0 && s_max_busy_workers > 0 && s_busy_workers)
    {
        ret = __atomic_load_n(s_busy_workers, __ATOMIC_SEQ_CST);
        if (ret >= s_max_busy_workers)
        {
            send_conn_close_notification(pReq->m_fd);
            lsapi_close_connection(pReq);
        }
    }

    if ( (unsigned int)s_req_processed > s_max_reqs )
        return -1;

    if ( s_worker_status )
    {
        s_worker_status->m_tmWaitBegin = time( NULL );
    }


    while( g_running )
    {
        if ( pReq->m_fd != -1 )
        {
            fd = pReq->m_fd;
        }
        else if ( pReq->m_fdListen != -1 )
            fd = pReq->m_fdListen;
        else
        {
            break;
        }
        wait_secs = 0;
        while( 1 )
        {
            if ( !g_running )
                return -1;
            if (s_req_processed && s_worker_status
                && s_worker_status->m_iKillSent)
                return -1;
            FD_ZERO( &readfds );
            FD_SET( fd, &readfds );
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;
            if (fd == pReq->m_fdListen)
            {
                if (s_worker_status)
                    __atomic_store_n(&s_worker_status->m_state,
                                     LSAPI_STATE_ACCEPTING, __ATOMIC_SEQ_CST);
                if (s_accepting_workers)
                    __atomic_fetch_add(s_accepting_workers, 1, __ATOMIC_SEQ_CST);
            }
            ret = (*g_fnSelect)(fd+1, &readfds, NULL, NULL, &timeout);
            if (fd == pReq->m_fdListen)
            {
                if (s_accepting_workers)
                    __atomic_fetch_sub(s_accepting_workers, 1, __ATOMIC_SEQ_CST);
                if (s_worker_status)
                    __atomic_store_n(&s_worker_status->m_state,
                                     LSAPI_STATE_IDLE, __ATOMIC_SEQ_CST);
            }

            if ( ret == 0 )
            {
                if ( s_worker_status )
                {
                    s_worker_status->m_inProcess = 0;
                    if (fd == pReq->m_fdListen)
                    {
                        if (s_keep_listener == 0 || !is_enough_free_mem())
                            return -1;
                        if (s_keep_listener == 1)
                        {
                            int wait_time = 10;
                            if (s_busy_workers)
                                wait_time += *s_busy_workers * 10;
                            if (s_accepting_workers)
                                wait_time >>= (*s_accepting_workers);
                            if (wait_secs >= wait_time)
                                return -1;
                        }
                    }
                }
                ++wait_secs;
                if (( s_max_idle_secs > 0 )&&(wait_secs >= s_max_idle_secs ))
                    return -1;
                if ( lsapi_parent_dead() )
                    return -1;
            }
            else if ( ret == -1 )
            {
                if ( errno == EINTR )
                    continue;
                else
                    return -1;
            }
            else if ( ret >= 1 )
            {
                if (s_req_processed && s_worker_status
                    && s_worker_status->m_iKillSent)
                    return -1;
                if ( fd == pReq->m_fdListen )
                {
                    pReq->m_fd = lsapi_accept( pReq->m_fdListen );
                    if ( pReq->m_fd != -1 )
                    {
                        if (s_worker_status)
                            __atomic_store_n(&s_worker_status->m_state,
                                             LSAPI_STATE_CONNECTED,
                                             __ATOMIC_SEQ_CST);
                        if (s_busy_workers)
                            __atomic_fetch_add(s_busy_workers, 1, __ATOMIC_SEQ_CST);

                        fd = pReq->m_fd;

                        lsapi_set_nblock( fd, 0 );
                        //init_conn_key( pReq->m_fd );
                        if (!s_keep_listener)
                        {
                            close( pReq->m_fdListen );
                            pReq->m_fdListen = -1;
                        }
                        if ( s_accept_notify )
                            if ( notify_req_received( pReq->m_fd ) == -1 )
                                return -1;
                    }
                    else
                    {
                        if (( errno == EINTR )||( errno == EAGAIN))
                            continue;
                        lsapi_perror( "lsapi_accept() error", errno );
                        return -1;
                    }
                }
                else
                    break;
            }
        }

        if ( !readReq( pReq ) )
        {
            if ( s_worker_status )
            {
                s_worker_status->m_iKillSent = 0;
                s_worker_status->m_inProcess = 1;
                ++s_worker_status->m_iReqCounter;
                s_worker_status->m_tmReqBegin =
                s_worker_status->m_tmLastCheckPoint = time(NULL);
            }
            ++s_req_processed;
            return 0;
        }
        lsapi_close_connection(pReq);
        LSAPI_Reset_r( pReq );
    }
    return -1;

}


void LSAPI_Set_Max_Reqs( int reqs )
{   s_max_reqs = reqs - 1;      }

void LSAPI_Set_Max_Idle( int secs )
{   s_max_idle_secs = secs;     }


void LSAPI_Set_Max_Children( int maxChildren )
{
    if ( g_prefork_server )
        g_prefork_server->m_iMaxChildren = maxChildren;
}


void LSAPI_Set_Extra_Children( int extraChildren )
{
    if (( g_prefork_server )&&( extraChildren >= 0 ))
        g_prefork_server->m_iExtraChildren = extraChildren;
}


void LSAPI_Set_Max_Process_Time( int secs )
{
    if (( g_prefork_server )&&( secs > 0 ))
        g_prefork_server->m_iMaxReqProcessTime = secs;
}


void LSAPI_Set_Max_Idle_Children( int maxIdleChld )
{
    if (( g_prefork_server )&&( maxIdleChld > 0 ))
        g_prefork_server->m_iMaxIdleChildren = maxIdleChld;
}


void LSAPI_Set_Server_Max_Idle_Secs( int serverMaxIdle )
{
    if ( g_prefork_server )
        g_prefork_server->m_iServerMaxIdle = serverMaxIdle;
}


void LSAPI_Set_Slow_Req_Msecs( int msecs )
{
    s_slow_req_msecs = msecs;
}


int  LSAPI_Get_Slow_Req_Msecs(void)
{
    return s_slow_req_msecs;
}


void LSAPI_No_Check_ppid(void)
{
    s_ppid = 0;
}


int LSAPI_Get_ppid(void)
{
    return(s_ppid);
}


#if defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
#include <crt_externs.h>
#else
extern char ** environ;
#endif
static void unset_lsapi_envs(void)
{
    char **env;
#if defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
    env = *_NSGetEnviron();
#else
    env = environ;
#endif
    while( env != NULL && *env != NULL )
    {
        if (!strncmp(*env, "LSAPI_", 6) || !strncmp( *env, "PHP_LSAPI_", 10 )
            || (!strncmp( *env, "PHPRC=", 6 )&&(!s_uid)))
        {
            char ** del = env;
            do
                *del = del[1];
            while( *del++ );
        }
        else
            ++env;
    }
}


static int lsapi_initSuEXEC(void)
{
    int i;
    struct passwd * pw;
    s_defaultUid = 0;
    s_defaultGid = 0;
    if ( s_uid == 0 )
    {
        const char * p = getenv( "LSAPI_DEFAULT_UID" );
        if ( p )
        {
            i = atoi( p );
            if ( i > 0 )
                s_defaultUid = i;
        }
        p = getenv( "LSAPI_DEFAULT_GID" );
        if ( p )
        {
            i = atoi( p );
            if ( i > 0 )
                s_defaultGid = i;
        }
        p = getenv( "LSAPI_SECRET" );
        if (( !p )||( readSecret(p) == -1 ))
                return -1;
        if ( g_prefork_server )
        {
            if ( g_prefork_server->m_iMaxChildren < 100 )
                g_prefork_server->m_iMaxChildren = 100;
            if ( g_prefork_server->m_iExtraChildren < 1000 )
                g_prefork_server->m_iExtraChildren = 1000;
        }
    }
    if ( !s_defaultUid || !s_defaultGid )
    {
        pw = getpwnam( "nobody" );
        if ( pw )
        {
            if ( !s_defaultUid )
                s_defaultUid = pw->pw_uid;
            if ( !s_defaultGid )
                s_defaultGid = pw->pw_gid;
        }
        else
        {
            if ( !s_defaultUid )
                s_defaultUid = 10000;
            if ( !s_defaultGid )
                s_defaultGid = 10000;
        }
    }
    return 0;
}


static int lsapi_check_path(const char *p, char *final, int max_len)
{
    char resolved_path[PATH_MAX+1];
    int len = 0;
    char *end;
    if (*p != '/')
    {
        if (getcwd(final, max_len) == NULL)
            return -1;
        len = strlen(final);
        *(final + len) = '/';
        ++len;
    }
    end = memccpy(&final[len], p, '\0', PATH_MAX - len);
    if (!end)
    {
        errno = EINVAL;
        return -1;
    }
    p = final;
    if (realpath(p, resolved_path) == NULL
        && errno != ENOENT && errno != EACCES)
        return -1;
    if (strncmp(resolved_path, "/etc/", 5) == 0)
    {
        errno = EPERM;
        return -1;
    }
    return 0;
}


static int lsapi_reopen_stderr2(const char *full_path)
{
    int newfd = open(full_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (newfd == -1)
    {
        LSAPI_perror_r(NULL, "Failed to open custom stderr log", full_path);
        return -1;
    }
    if (newfd != 2)
    {
        dup2(newfd, 2);
        close(newfd);
        dup2(2, 1);
    }
    if (s_stderr_log_path && full_path != s_stderr_log_path)
    {
        free(s_stderr_log_path);
        s_stderr_log_path = NULL;
    }
    s_stderr_log_path = strdup(full_path);
    return 0;
}


static int lsapi_reopen_stderr(const char *p)
{
    char full_path[PATH_MAX];
    if (s_uid == 0)
        return -1;
    if (lsapi_check_path(p, full_path, PATH_MAX) == -1)
    {
        LSAPI_perror_r(NULL, "Invalid custom stderr log path", p);
        return -1;
    }
    return lsapi_reopen_stderr2(full_path);
}


int LSAPI_Init_Env_Parameters( fn_select_t fp )
{
    const char *p;
    char ch;
    int n;
    int avoidFork = 0;

    p = getenv("LSAPI_STDERR_LOG");
    if (p)
    {
        lsapi_reopen_stderr(p);
    }
    if (!s_stderr_log_path)
        s_stderr_is_pipe = isPipe(STDERR_FILENO);

    p = getenv( "PHP_LSAPI_MAX_REQUESTS" );
    if ( !p )
        p = getenv( "LSAPI_MAX_REQS" );
    if ( p )
    {
        n = atoi( p );
        if ( n > 0 )
            LSAPI_Set_Max_Reqs( n );
    }

    p = getenv( "LSAPI_KEEP_LISTEN" );
    if ( p )
    {
        n = atoi( p );
        s_keep_listener = n;
    }

    p = getenv( "LSAPI_AVOID_FORK" );
    if ( p )
    {
        avoidFork = atoi( p );
        if (avoidFork)
        {
            s_keep_listener = 2;
            ch = *(p + strlen(p) - 1);
            if (  ch == 'G' || ch == 'g' )
                avoidFork *= 1024 * 1024 * 1024;
            else if (  ch == 'M' || ch == 'm' )
                avoidFork *= 1024 * 1024;
            if (avoidFork >= 1024 * 10240)
                s_min_avail_pages = avoidFork / 4096;
        }
    }

    p = getenv( "LSAPI_ACCEPT_NOTIFY" );
    if ( p )
    {
        s_accept_notify = atoi( p );
    }

    p = getenv( "LSAPI_SLOW_REQ_MSECS" );
    if ( p )
    {
        n = atoi( p );
        LSAPI_Set_Slow_Req_Msecs( n );
    }

#if defined( RLIMIT_CORE )
    p = getenv( "LSAPI_ALLOW_CORE_DUMP" );
    if ( !p )
    {
        struct rlimit limit = { 0, 0 };
        setrlimit( RLIMIT_CORE, &limit );
    }
    else
        s_enable_core_dump = 1;

#endif

    p = getenv( "LSAPI_MAX_IDLE" );
    if ( p )
    {
        n = atoi( p );
        LSAPI_Set_Max_Idle( n );
    }

    if ( LSAPI_Is_Listen() )
    {
        n = 0;
        p = getenv( "PHP_LSAPI_CHILDREN" );
        if ( !p )
            p = getenv( "LSAPI_CHILDREN" );
        if ( p )
            n = atoi( p );
        if ( n > 1 )
        {
            LSAPI_Init_Prefork_Server( n, fp, avoidFork != 0 );
            LSAPI_Set_Server_fd( g_req.m_fdListen );
        }

        p = getenv( "LSAPI_EXTRA_CHILDREN" );
        if ( p )
            LSAPI_Set_Extra_Children( atoi( p ) );

        p = getenv( "LSAPI_MAX_IDLE_CHILDREN" );
        if ( p )
            LSAPI_Set_Max_Idle_Children( atoi( p ) );

        p = getenv( "LSAPI_PGRP_MAX_IDLE" );
        if ( p )
        {
            LSAPI_Set_Server_Max_Idle_Secs( atoi( p ) );
        }

        p = getenv( "LSAPI_MAX_PROCESS_TIME" );
        if ( p )
            LSAPI_Set_Max_Process_Time( atoi( p ) );

        if ( getenv( "LSAPI_PPID_NO_CHECK" ) )
        {
            LSAPI_No_Check_ppid();
        }

        p = getenv("LSAPI_MAX_BUSY_WORKER");
        if (p)
        {
            n = atoi(p);
            s_max_busy_workers = n;
            if (n >= 0)
                LSAPI_No_Check_ppid();
        }


        p = getenv( "LSAPI_DUMP_DEBUG_INFO" );
        if ( p )
            s_dump_debug_info = atoi( p );

        if ( lsapi_initSuEXEC() == -1 )
            return -1;
#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__gnu_linux__)
        lsapi_initLVE();
#endif
    }
    unset_lsapi_envs();
    return 0;
}


int LSAPI_ErrResponse_r( LSAPI_Request * pReq, int code, const char ** pRespHeaders,
                         const char * pBody, int bodyLen )
{
    LSAPI_SetRespStatus_r( pReq, code );
    if ( pRespHeaders )
    {
        while( *pRespHeaders )
        {
            LSAPI_AppendRespHeader_r( pReq, *pRespHeaders, strlen( *pRespHeaders ) );
            ++pRespHeaders;
        }
    }
    if ( pBody &&( bodyLen > 0 ))
    {
        LSAPI_Write_r( pReq, pBody, bodyLen );
    }
    LSAPI_Finish_r( pReq );
    return 0;
}


static void lsapi_MD5Transform(uint32 buf[4], uint32 const in[16]);

/*
 * Note: this code is harmless on little-endian machines.
 */
static void byteReverse(unsigned char *buf, unsigned longs)
{
    uint32 t;
    do {
        t = (uint32) ((unsigned) buf[3] << 8 | buf[2]) << 16 |
            ((unsigned) buf[1] << 8 | buf[0]);
        *(uint32 *) buf = t;
        buf += 4;
    } while (--longs);
}


/*
 * Start MD5 accumulation.  Set bit count to 0 and buffer to mysterious
 * initialization constants.
 */
void lsapi_MD5Init(struct lsapi_MD5Context *ctx)
{
    ctx->buf[0] = 0x67452301;
    ctx->buf[1] = 0xefcdab89;
    ctx->buf[2] = 0x98badcfe;
    ctx->buf[3] = 0x10325476;

    ctx->bits[0] = 0;
    ctx->bits[1] = 0;
}

/*
 * Update context to reflect the concatenation of another buffer full
 * of bytes.
 */
void lsapi_MD5Update(struct lsapi_MD5Context *ctx, unsigned char const *buf, unsigned len)
{
    register uint32 t;

    /* Update bitcount */

    t = ctx->bits[0];
    if ((ctx->bits[0] = t + ((uint32) len << 3)) < t)
        ctx->bits[1]++;                /* Carry from low to high */
    ctx->bits[1] += len >> 29;

    t = (t >> 3) & 0x3f;        /* Bytes already in shsInfo->data */

    /* Handle any leading odd-sized chunks */

    if (t) {
        unsigned char *p = (unsigned char *) ctx->in + t;

        t = 64 - t;
        if (len < t) {
            memmove(p, buf, len);
            return;
        }
        memmove(p, buf, t);
        byteReverse(ctx->in, 16);
        lsapi_MD5Transform(ctx->buf, (uint32 *) ctx->in);
        buf += t;
        len -= t;
    }
    /* Process data in 64-byte chunks */

    while (len >= 64) {
        memmove(ctx->in, buf, 64);
        byteReverse(ctx->in, 16);
        lsapi_MD5Transform(ctx->buf, (uint32 *) ctx->in);
        buf += 64;
        len -= 64;
    }

    /* Handle any remaining bytes of data. */

    memmove(ctx->in, buf, len);
}


/*
 * Final wrap-up - pad to 64-byte boundary with the bit pattern
 * 1 0* (64-bit count of bits processed, MSB-first)
 */
void lsapi_MD5Final(unsigned char digest[16], struct lsapi_MD5Context *ctx)
{
    unsigned int count;
    unsigned char *p;

    /* Compute number of bytes mod 64 */
    count = (ctx->bits[0] >> 3) & 0x3F;

    /* Set the first char of padding to 0x80.  This is safe since there is
       always at least one byte free */
    p = ctx->in + count;
    *p++ = 0x80;

    /* Bytes of padding needed to make 64 bytes */
    count = 64 - 1 - count;

    /* Pad out to 56 mod 64 */
    if (count < 8) {
        /* Two lots of padding:  Pad the first block to 64 bytes */
        memset(p, 0, count);
        byteReverse(ctx->in, 16);
        lsapi_MD5Transform(ctx->buf, (uint32 *) ctx->in);

        /* Now fill the next block with 56 bytes */
        memset(ctx->in, 0, 56);
    } else {
        /* Pad block to 56 bytes */
        memset(p, 0, count - 8);
    }
    byteReverse(ctx->in, 14);

    /* Append length in bits and transform */
    ((uint32 *) ctx->in)[14] = ctx->bits[0];
    ((uint32 *) ctx->in)[15] = ctx->bits[1];

    lsapi_MD5Transform(ctx->buf, (uint32 *) ctx->in);
    byteReverse((unsigned char *) ctx->buf, 4);
    memmove(digest, ctx->buf, 16);
    memset(ctx, 0, sizeof(*ctx));        /* In case it's sensitive */
}


/* The four core functions - F1 is optimized somewhat */

/* #define F1(x, y, z) (x & y | ~x & z) */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

/* This is the central step in the MD5 algorithm. */
#define MD5STEP(f, w, x, y, z, data, s) \
        ( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )

/*
 * The core of the MD5 algorithm, this alters an existing MD5 hash to
 * reflect the addition of 16 longwords of new data.  MD5Update blocks
 * the data and converts bytes into longwords for this routine.
 */
static void lsapi_MD5Transform(uint32 buf[4], uint32 const in[16])
{
    register uint32 a, b, c, d;

    a = buf[0];
    b = buf[1];
    c = buf[2];
    d = buf[3];

    MD5STEP(F1, a, b, c, d, in[0] + 0xd76aa478, 7);
    MD5STEP(F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
    MD5STEP(F1, c, d, a, b, in[2] + 0x242070db, 17);
    MD5STEP(F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
    MD5STEP(F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
    MD5STEP(F1, d, a, b, c, in[5] + 0x4787c62a, 12);
    MD5STEP(F1, c, d, a, b, in[6] + 0xa8304613, 17);
    MD5STEP(F1, b, c, d, a, in[7] + 0xfd469501, 22);
    MD5STEP(F1, a, b, c, d, in[8] + 0x698098d8, 7);
    MD5STEP(F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
    MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
    MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7be, 22);
    MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122, 7);
    MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193, 12);
    MD5STEP(F1, c, d, a, b, in[14] + 0xa679438e, 17);
    MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821, 22);

    MD5STEP(F2, a, b, c, d, in[1] + 0xf61e2562, 5);
    MD5STEP(F2, d, a, b, c, in[6] + 0xc040b340, 9);
    MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51, 14);
    MD5STEP(F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
    MD5STEP(F2, a, b, c, d, in[5] + 0xd62f105d, 5);
    MD5STEP(F2, d, a, b, c, in[10] + 0x02441453, 9);
    MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
    MD5STEP(F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
    MD5STEP(F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
    MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6, 9);
    MD5STEP(F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
    MD5STEP(F2, b, c, d, a, in[8] + 0x455a14ed, 20);
    MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
    MD5STEP(F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
    MD5STEP(F2, c, d, a, b, in[7] + 0x676f02d9, 14);
    MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

    MD5STEP(F3, a, b, c, d, in[5] + 0xfffa3942, 4);
    MD5STEP(F3, d, a, b, c, in[8] + 0x8771f681, 11);
    MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
    MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380c, 23);
    MD5STEP(F3, a, b, c, d, in[1] + 0xa4beea44, 4);
    MD5STEP(F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
    MD5STEP(F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
    MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
    MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
    MD5STEP(F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
    MD5STEP(F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
    MD5STEP(F3, b, c, d, a, in[6] + 0x04881d05, 23);
    MD5STEP(F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
    MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
    MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
    MD5STEP(F3, b, c, d, a, in[2] + 0xc4ac5665, 23);

    MD5STEP(F4, a, b, c, d, in[0] + 0xf4292244, 6);
    MD5STEP(F4, d, a, b, c, in[7] + 0x432aff97, 10);
    MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7, 15);
    MD5STEP(F4, b, c, d, a, in[5] + 0xfc93a039, 21);
    MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3, 6);
    MD5STEP(F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
    MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47d, 15);
    MD5STEP(F4, b, c, d, a, in[1] + 0x85845dd1, 21);
    MD5STEP(F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
    MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
    MD5STEP(F4, c, d, a, b, in[6] + 0xa3014314, 15);
    MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
    MD5STEP(F4, a, b, c, d, in[4] + 0xf7537e82, 6);
    MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235, 10);
    MD5STEP(F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
    MD5STEP(F4, b, c, d, a, in[9] + 0xeb86d391, 21);

    buf[0] += a;
    buf[1] += b;
    buf[2] += c;
    buf[3] += d;
}


int LSAPI_Set_Restored_Parent_Pid(int pid)
{
    int old_ppid = s_ppid;
    s_restored_ppid = pid;
    return old_ppid;
}


int LSAPI_Inc_Req_Processed(int cnt)
{
    return __atomic_add_fetch(s_global_counter, cnt, __ATOMIC_SEQ_CST);
}
