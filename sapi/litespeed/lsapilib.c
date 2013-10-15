/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
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

/* $Id$ */

/*
Copyright (c) 2007, Lite Speed Technologies Inc.
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

#include <lsapilib.h>

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/un.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define LSAPI_ST_REQ_HEADER     1
#define LSAPI_ST_REQ_BODY       2
#define LSAPI_ST_RESP_HEADER    4
#define LSAPI_ST_RESP_BODY      8

#define LSAPI_RESP_BUF_SIZE     8192
#define LSAPI_INIT_RESP_HEADER_LEN 4096


static int g_inited = 0;
static int g_running = 1;
static int s_ppid;
LSAPI_Request g_req = { -1, -1 };

void Flush_RespBuf_r( LSAPI_Request * pReq );

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
     
static int CGI_HEADER_LEN[H_TRANSFER_ENCODING+1] = {
     11, 19, 20, 20, 18, 15, 12, 14, 11, 12, 9, 11, 12, 15, 18,
     22, 13, 18, 13, 24, 15, 10, 20, 8, 22 
};


static const char *HTTP_HEADERS[H_TRANSFER_ENCODING+1] = {
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

static int HTTP_HEADER_LEN[H_TRANSFER_ENCODING+1] = {
    6, 14, 15, 15, 13, 10, 12, 14, 6, 7, 4, 6, 7, 10, /* user-agent */
    13,17, 8, 13, 8, 19, 10, 5, 15, 3, 17
};
     
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

    if (sa.sa_handler == SIG_DFL) {
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sa.sa_handler = handler;
        sigaction(signo, &sa, NULL);
    }
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
    while( 1 ) {
        ret = close( fd );
        if (( ret == -1 )&&( errno == EINTR )&&(g_running)) {
            continue;
        }
        return ret;
    }
}

static inline int lsapi_read( int fd, void * pBuf, int len )
{
    int ret;
    while( 1 ) {
        ret = read( fd, (char *)pBuf, len );
        if (( ret == -1 )&&( errno == EINTR )&&(g_running)) {
            continue;
        }
        return ret;
    }
}


static int lsapi_writev( int fd, struct iovec ** pVec, int count, int totalLen )
{
    int ret;
    int left = totalLen;
    int n = count;
    while(( left > 0 )&&g_running ) {
        ret = writev( fd, *pVec, n );
        if ( ret > 0 ) {
            left -= ret;
            if (( left <= 0)||( !g_running )) {
                return totalLen - left;
            }
            while( ret > 0 ) {
                if ( (*pVec)->iov_len <= ret ) {
                    ret -= (*pVec)->iov_len;
                    ++(*pVec);
                } else {
                    (*pVec)->iov_base = (char *)(*pVec)->iov_base + ret;
                    (*pVec)->iov_len -= ret;
                    break;
                }
            }
        } else if ( ret == -1 ) {
            if ( errno == EAGAIN ) {
                if ( totalLen - left > 0 ) {
                    return totalLen - left;
                } else {
                    return -1;
                }
            } else {
                if ( errno != EINTR ) {
                    return ret;
                }
            }
        }
    }
    return totalLen - left;
}

static inline int allocateBuf( LSAPI_Request * pReq, int size )
{
    char * pBuf = (char *)realloc( pReq->m_pReqBuf, size );
    if ( pBuf ) {
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
    if ( !p ) {
        return -1;
    }
    pReq->m_pIovecToWrite = p + ( pReq->m_pIovecToWrite - pReq->m_pIovec );
    pReq->m_pIovecCur = p + ( pReq->m_pIovecCur - pReq->m_pIovec );
    pReq->m_pIovec = p;
    pReq->m_pIovecEnd = p + n;
    return 0;
}

static int allocateRespHeaderBuf( LSAPI_Request * pReq, int size )
{
    char * p = (char *)realloc( pReq->m_pRespHeaderBuf, size );
    if ( !p ) {
        return -1;
    }
    pReq->m_pRespHeaderBufPos   = p + ( pReq->m_pRespHeaderBufPos - pReq->m_pRespHeaderBuf );
    pReq->m_pRespHeaderBuf      = p;
    pReq->m_pRespHeaderBufEnd   = p + size;
    return 0;
}


static inline int verifyHeader( struct lsapi_packet_header * pHeader, char pktType )
{
    if (( LSAPI_VERSION_B0 != pHeader->m_versionB0 )||
        ( LSAPI_VERSION_B1 != pHeader->m_versionB1 )||
        ( pktType != pHeader->m_type )) {
        return -1;
    }
    if ( LSAPI_ENDIAN != (pHeader->m_flag & LSAPI_ENDIAN_BIT )) {
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
    if ( *curSize >= newSize ) {
        return 0;
    }
    if ( newSize > 8192 ) {
        return -1;
    }
    pBuf = (struct LSAPI_key_value_pair *)realloc( *pEnvList, newSize *
                    sizeof(struct LSAPI_key_value_pair) );
    if ( pBuf ) {
        *pEnvList = pBuf;
        *curSize  = newSize;
        return 0;
    } else {
        return -1;
    }

}

static inline int isPipe( int fd )
{
    char        achPeer[128];
    socklen_t   len = 128;
    if (( getpeername( fd, (struct sockaddr *)achPeer, &len ) != 0 )&&
        ( errno == ENOTCONN )) {
        return 0;
    } else {
        return 1;
    }
}

static int parseEnv( struct LSAPI_key_value_pair * pEnvList, int count,
            char **pBegin, char * pEnd )
{
    struct LSAPI_key_value_pair * pEnvEnd;
    int keyLen = 0, valLen = 0;
    if ( count > 8192 ) {
        return -1;
    }
    pEnvEnd = pEnvList + count;
    while( pEnvList != pEnvEnd ) {
        if ( pEnd - *pBegin < 4 ) {
            return -1;
        }
        keyLen = *((unsigned char *)((*pBegin)++));
        keyLen = (keyLen << 8) + *((unsigned char *)((*pBegin)++));
        valLen = *((unsigned char *)((*pBegin)++));
        valLen = (valLen << 8) + *((unsigned char *)((*pBegin)++));
        if ( *pBegin + keyLen + valLen > pEnd ) {
            return -1;
        }
        if (( !keyLen )||( !valLen )) {
            return -1;
        }

        pEnvList->pKey = *pBegin;
        *pBegin += keyLen;
        pEnvList->pValue = *pBegin;
        *pBegin += valLen;

        pEnvList->keyLen = keyLen - 1;
        pEnvList->valLen = valLen - 1;
        ++pEnvList;
    }
    if ( memcmp( *pBegin, "\0\0\0\0", 4 ) != 0 ) {
        return -1;
    }
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
    for( i = 0; i < H_TRANSFER_ENCODING; ++i ) {
        if ( pReq->m_pHeaderIndex->m_headerOff[i] ) {
            register char b;
            char * p = (char *)(&pReq->m_pHeaderIndex->m_headerLen[i]);
            b = p[0];
            p[0] = p[1];
            p[1] = b;
            swapIntEndian( &pReq->m_pHeaderIndex->m_headerOff[i] );
        }
    }
    if ( pReq->m_pHeader->m_cntUnknownHeaders > 0 ) {
        struct lsapi_header_offset * pCur, *pEnd;
        pCur = pReq->m_pUnknownHeader;
        pEnd = pCur + pReq->m_pHeader->m_cntUnknownHeaders;
        while( pCur < pEnd ) {
            swapIntEndian( &pCur->nameOff );
            swapIntEndian( &pCur->nameLen );
            swapIntEndian( &pCur->valueOff );
            swapIntEndian( &pCur->valueLen );
            ++pCur;
        }
    }    
}

static int parseRequest( LSAPI_Request * pReq, int totalLen )
{
    int shouldFixEndian;
    char * pBegin = pReq->m_pReqBuf + sizeof( struct lsapi_req_header );
    char * pEnd = pReq->m_pReqBuf + totalLen;
    shouldFixEndian = ( LSAPI_ENDIAN != (
                pReq->m_pHeader->m_pktHeader.m_flag & LSAPI_ENDIAN_BIT ) );
    if ( shouldFixEndian ) {
        fixEndian( pReq );
    }
    if ( (pReq->m_specialEnvListSize < pReq->m_pHeader->m_cntSpecialEnv )&&
            allocateEnvList( &pReq->m_pSpecialEnvList,
                              &pReq->m_specialEnvListSize,
                             pReq->m_pHeader->m_cntSpecialEnv ) == -1 ) {
        return -1;
    }
    if ( (pReq->m_envListSize < pReq->m_pHeader->m_cntEnv )&&
            allocateEnvList( &pReq->m_pEnvList, &pReq->m_envListSize,
                             pReq->m_pHeader->m_cntEnv ) == -1 ) {
        return -1;
    }
    if ( parseEnv( pReq->m_pSpecialEnvList,
                   pReq->m_pHeader->m_cntSpecialEnv,
                   &pBegin, pEnd ) == -1 ) {
        return -1;
    }
    if ( parseEnv( pReq->m_pEnvList, pReq->m_pHeader->m_cntEnv,
                   &pBegin, pEnd ) == -1 ) {
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
    if ( pBegin != pEnd ) {
        return -1;
    }

    if ( shouldFixEndian ) {
        fixHeaderIndexEndian( pReq );
    }

    return 0;
}

static struct lsapi_packet_header ack = {'L', 'S',
                LSAPI_REQ_RECEIVED, LSAPI_ENDIAN, {LSAPI_PACKET_HEADER_LEN} };
static inline int notify_req_received( LSAPI_Request * pReq )
{
    if ( write( pReq->m_fd, &ack, LSAPI_PACKET_HEADER_LEN )
         < LSAPI_PACKET_HEADER_LEN ) {
        return -1;
    }
    return 0;
}


static int readReq( LSAPI_Request * pReq )
{
    int len;
    int packetLen;
    if ( !pReq ) {
        return -1;
    }
    if ( pReq->m_reqBufSize < 8192 ) {
        if ( allocateBuf( pReq, 8192 ) == -1 ) {
            return -1;
        }
    }

    while ( pReq->m_bufRead < LSAPI_PACKET_HEADER_LEN ) {
        len = lsapi_read( pReq->m_fd, pReq->m_pReqBuf, pReq->m_reqBufSize );
        if ( len <= 0 ) {
            return -1;
        }
        pReq->m_bufRead += len;
    }
    pReq->m_reqState = LSAPI_ST_REQ_HEADER;

    packetLen = verifyHeader( &pReq->m_pHeader->m_pktHeader, LSAPI_BEGIN_REQUEST );
    if ( packetLen < 0 ) {
        return -1;
    }
    if ( packetLen > LSAPI_MAX_HEADER_LEN ) {
        return -1;
    }

    if ( packetLen + 1024 > pReq->m_reqBufSize ) {
        if ( allocateBuf( pReq, packetLen + 1024 ) == -1 ) {
            return -1;
        }
    }
    while( packetLen > pReq->m_bufRead ) {
        len = lsapi_read( pReq->m_fd, pReq->m_pReqBuf + pReq->m_bufRead, packetLen - pReq->m_bufRead );
        if ( len <= 0 ) {
            return -1;
        }
        pReq->m_bufRead += len;
    }
    if ( parseRequest( pReq, packetLen ) < 0 ) {
        return -1;
    }
    pReq->m_bufProcessed = packetLen;
    pReq->m_reqState = LSAPI_ST_REQ_BODY | LSAPI_ST_RESP_HEADER;

    return notify_req_received( pReq );
}



int LSAPI_Init(void)
{
    if ( !g_inited ) {
        lsapi_signal(SIGPIPE, lsapi_sigpipe);
        lsapi_signal(SIGUSR1, lsapi_siguser1);

#if defined(SIGXFSZ) && defined(SIG_IGN)
        signal(SIGXFSZ, SIG_IGN);
#endif
        /* let STDOUT function as STDERR, 
           just in case writing to STDOUT directly */
        dup2( 2, 1 );

        if ( LSAPI_InitRequest( &g_req, LSAPI_SOCK_FILENO ) == -1 ) {
            return -1;
        }
        g_inited = 1;
        s_ppid = getppid();
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

int LSAPI_InitRequest( LSAPI_Request * pReq, int fd )
{
    if ( !pReq ) {
        return -1;
    }
    memset( pReq, 0, sizeof( LSAPI_Request ) );
    if ( allocateIovec( pReq, 16 ) == -1 ) {
        return -1;
    }
    pReq->m_pRespBuf = pReq->m_pRespBufPos = (char *)malloc( LSAPI_RESP_BUF_SIZE );
    if ( !pReq->m_pRespBuf ) {
        return -1;
    }
    pReq->m_pRespBufEnd = pReq->m_pRespBuf + LSAPI_RESP_BUF_SIZE;
    pReq->m_pIovecCur = pReq->m_pIovecToWrite = pReq->m_pIovec + 1;
    pReq->m_respPktHeaderEnd = &pReq->m_respPktHeader[5];
    if ( allocateRespHeaderBuf( pReq, LSAPI_INIT_RESP_HEADER_LEN ) == -1 ) {
        return -1;
    }
 
    if ( isPipe( fd ) ) {
        pReq->m_fdListen = -1;
        pReq->m_fd = fd;
    } else {
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
    
    if ( !pReq ) {
        return -1;
    }
    if ( LSAPI_Finish_r( pReq ) == -1 ) {
        return -1;
    }
    while( g_running ) {
        if ( pReq->m_fd == -1 ) {
            if ( pReq->m_fdListen != -1) {
                len = sizeof( achPeer );
                pReq->m_fd = accept( pReq->m_fdListen,
                            (struct sockaddr *)&achPeer, &len );
                if ( pReq->m_fd == -1 ) {
                    if (( errno == EINTR )||( errno == EAGAIN)) {
                        continue;
                    } else {
                        return -1;
                    }
                } else {
                    lsapi_set_nblock( pReq->m_fd , 0 );
                    if (((struct sockaddr *)&achPeer)->sa_family == AF_INET ) {    
                        setsockopt(pReq->m_fd, IPPROTO_TCP, TCP_NODELAY,
                                (char *)&nodelay, sizeof(nodelay));
                    }
                }
            } else {
                return -1;
            }
        }
        if ( !readReq( pReq ) ) {
            break;
        }
        lsapi_close( pReq->m_fd );
        pReq->m_fd = -1;
        LSAPI_Reset_r( pReq );
    }
    return 0;
}

static struct lsapi_packet_header   finish = {'L', 'S',
                LSAPI_RESP_END, LSAPI_ENDIAN, {LSAPI_PACKET_HEADER_LEN} };

int LSAPI_Finish_r( LSAPI_Request * pReq )
{
    /* finish req body */
    if ( !pReq ) {
        return -1;
    }
    if (pReq->m_reqState) {
        if ( pReq->m_fd != -1 ) {
            if ( pReq->m_reqState & LSAPI_ST_RESP_HEADER ) {
                LSAPI_FinalizeRespHeaders_r( pReq );
            }
            if ( pReq->m_pRespBufPos != pReq->m_pRespBuf ) {
                Flush_RespBuf_r( pReq );
            }
            
            pReq->m_pIovecCur->iov_base = (void *)&finish;
            pReq->m_pIovecCur->iov_len  = LSAPI_PACKET_HEADER_LEN;
            pReq->m_totalLen += LSAPI_PACKET_HEADER_LEN;
            ++pReq->m_pIovecCur;
            LSAPI_Flush_r( pReq );
        }
        LSAPI_Reset_r( pReq );
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
    if ( pReq->m_pReqBuf ) {
        free( pReq->m_pReqBuf );
    }
    if ( pReq->m_pSpecialEnvList ) {
        free( pReq->m_pSpecialEnvList );
    }
    if ( pReq->m_pEnvList ) {
        free( pReq->m_pEnvList );
    }
    if ( pReq->m_pRespHeaderBuf ) {
        free( pReq->m_pRespHeaderBuf );  
    }
    return 0;
}


char * LSAPI_GetHeader_r( LSAPI_Request * pReq, int headerIndex )
{
    int off;
    if ( !pReq || ((unsigned int)headerIndex > H_TRANSFER_ENCODING) ) {
        return NULL;
    }
    off = pReq->m_pHeaderIndex->m_headerOff[ headerIndex ];
    if ( !off ) {
        return NULL;
    }
    if ( *(pReq->m_pHttpHeader + off + 
           pReq->m_pHeaderIndex->m_headerLen[ headerIndex ]) ) {
        *( pReq->m_pHttpHeader + off + 
            pReq->m_pHeaderIndex->m_headerLen[ headerIndex ]) = 0;
    }
    return pReq->m_pHttpHeader + off;
}

static int readBodyToReqBuf( LSAPI_Request * pReq )
{
    int bodyLeft;
    int len = pReq->m_bufRead - pReq->m_bufProcessed;
    if ( len > 0 ) {
        return len;
    }
    pReq->m_bufRead = pReq->m_bufProcessed = pReq->m_pHeader->m_pktHeader.m_packetLen.m_iLen;

    bodyLeft = pReq->m_pHeader->m_reqBodyLen - pReq->m_reqBodyRead;
    len = pReq->m_reqBufSize - pReq->m_bufRead;
    if ( len < 0 ) {
        return -1;
    }
    if ( len > bodyLeft ) {
        len = bodyLeft;
    }
    len = lsapi_read( pReq->m_fd, pReq->m_pReqBuf + pReq->m_bufRead, len );
    if ( len > 0 ) {
        pReq->m_bufRead += len;
    }
    return len;
}


int LSAPI_ReqBodyGetChar_r( LSAPI_Request * pReq )
{
    if (!pReq || (pReq->m_fd ==-1) ) {
        return EOF;
    }
    if ( pReq->m_bufProcessed >= pReq->m_bufRead ) {
        if ( readBodyToReqBuf( pReq ) <= 0 ) {
            return EOF;
        }
    }
    ++pReq->m_reqBodyRead;
    return (unsigned char)*(pReq->m_pReqBuf + pReq->m_bufProcessed++);
}



int LSAPI_ReqBodyGetLine_r( LSAPI_Request * pReq, char * pBuf, int bufLen, int *getLF )
{
    int len;
    int left;
    char * pBufEnd = pBuf + bufLen - 1;
    char * pBufCur = pBuf;
    char * pCur;
    char * p;
    if (!pReq || (pReq->m_fd ==-1) ||( !pBuf )||(bufLen < 0 )|| !getLF ) {
        return -1;
    }
    *getLF = 0;
    while( (left = pBufEnd - pBufCur ) > 0 ) {
        
        len = pReq->m_bufRead - pReq->m_bufProcessed;
        if ( len <= 0 ) {
            if ( (len = readBodyToReqBuf( pReq )) <= 0 ) {
                *getLF = 1;
                break;
            }
        }
        if ( len > left ) {
            len = left;
        }
        pCur = pReq->m_pReqBuf + pReq->m_bufProcessed;
        p = memchr( pCur, '\n', len );
        if ( p ) {
            len = p - pCur + 1;
        }
        memmove( pBufCur, pCur, len );
        pBufCur += len;
        pReq->m_bufProcessed += len;

        pReq->m_reqBodyRead += len;
        
        if ( p ) {
            *getLF = 1;
            break;
        }
    }
    *pBufCur = 0;
  
    return pBufCur - pBuf;
}


int LSAPI_ReadReqBody_r( LSAPI_Request * pReq, char * pBuf, int bufLen )
{
    int len;
    int total;
    /* char *pOldBuf = pBuf; */
    if (!pReq || (pReq->m_fd ==-1) || ( !pBuf )||(bufLen < 0 )) {
        return -1;
    }
    total = pReq->m_pHeader->m_reqBodyLen - pReq->m_reqBodyRead;
    
    if ( total <= 0 ) {
        return 0;
    }
    if ( total < bufLen ) {
        bufLen = total;
    }

    total = 0;
    len = pReq->m_bufRead - pReq->m_bufProcessed;
    if ( len > 0 ) {
        if ( len > bufLen ) {
            len = bufLen;
        }
        memmove( pBuf, pReq->m_pReqBuf + pReq->m_bufProcessed, len );
        pReq->m_bufProcessed += len;
        total += len;
        pBuf += len;
        bufLen -= len;
    }
    while( bufLen > 0 ) {
        len = lsapi_read( pReq->m_fd, pBuf, bufLen );
        if ( len > 0 ) {
            total += len;
            pBuf += len;
            bufLen -= len;
        } else {
            if ( len <= 0 ) {
                if ( !total) {
                    return -1;
                }
                break;
            }
        }
    }
    pReq->m_reqBodyRead += total;
    return total;
        
}


int LSAPI_Write_r( LSAPI_Request * pReq, const char * pBuf, int len )
{
    struct lsapi_packet_header * pHeader;
    const char * pEnd;
    const char * p;
    int bufLen;
    int toWrite;
    int packetLen;
    
    if ( !pReq || !pBuf || (pReq->m_fd == -1) ) {
        return -1;
    }
    if ( len < pReq->m_pRespBufEnd - pReq->m_pRespBufPos ) {
        memmove( pReq->m_pRespBufPos, pBuf, len );
        pReq->m_pRespBufPos += len;
        return len;
    }
    
    if ( pReq->m_reqState & LSAPI_ST_RESP_HEADER ) {
        LSAPI_FinalizeRespHeaders_r( pReq );
    }
    pReq->m_reqState |= LSAPI_ST_RESP_BODY;
    
    pHeader = pReq->m_respPktHeader;
    p       = pBuf;
    pEnd    = pBuf + len;
    bufLen  = pReq->m_pRespBufPos - pReq->m_pRespBuf;
    
    while( ( toWrite = pEnd - p ) > 0 ) {
        packetLen = toWrite + bufLen;
        if ( LSAPI_MAX_DATA_PACKET_LEN < packetLen) {
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
        if ( bufLen > 0 ) {
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

        if ( pHeader >= pReq->m_respPktHeaderEnd - 1) {
            if ( LSAPI_Flush_r( pReq ) == -1 ) {
                return -1;
            }
            pHeader = pReq->m_respPktHeader;
        }
    }
    if ( pHeader != pReq->m_respPktHeader ) {
        if ( LSAPI_Flush_r( pReq ) == -1 ) {
            return -1;
        }
    }
    return p - pBuf;
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
    if ( bufLen > 0 ) {
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
    if ( !pReq ) {
        return -1;
    }
    n = pReq->m_pIovecCur - pReq->m_pIovecToWrite;
    if (( 0 == n )&&( pReq->m_pRespBufPos == pReq->m_pRespBuf )) {
        return 0;
    }
    if ( pReq->m_fd == -1 ) {
        pReq->m_pRespBufPos = pReq->m_pRespBuf;
        pReq->m_totalLen = 0;
        pReq->m_pIovecCur = pReq->m_pIovecToWrite = pReq->m_pIovec;
        return -1;
    }
    if ( pReq->m_reqState & LSAPI_ST_RESP_HEADER ) {
        LSAPI_FinalizeRespHeaders_r( pReq );
    }
    if ( pReq->m_pRespBufPos != pReq->m_pRespBuf ) {
        Flush_RespBuf_r( pReq );
    }
    
    n = pReq->m_pIovecCur - pReq->m_pIovecToWrite;
    if ( n > 0 ) {
        
        ret = lsapi_writev( pReq->m_fd, &pReq->m_pIovecToWrite,
                  n, pReq->m_totalLen );
        if ( ret < pReq->m_totalLen ) {
            lsapi_close( pReq->m_fd );
            pReq->m_fd = -1;
            ret = -1;
        }
        pReq->m_totalLen = 0;
        pReq->m_pIovecCur = pReq->m_pIovecToWrite = pReq->m_pIovec;
    }
    return ret;
}


int LSAPI_Write_Stderr_r( LSAPI_Request * pReq, const char * pBuf, int len )
{
    struct lsapi_packet_header header;
    const char * pEnd;
    const char * p;
    int packetLen;
    int totalLen;
    int ret;
    struct iovec iov[2];
    struct iovec *pIov;
    
    if ( !pReq ) {
        return -1;
    }
    if (( pReq->m_fd == -1 )||(pReq->m_fd == pReq->m_fdListen )) {
        return write( 2, pBuf, len );
    }
    if ( pReq->m_pRespBufPos != pReq->m_pRespBuf ) {
        LSAPI_Flush_r( pReq );
    }
    
    p       = pBuf;
    pEnd    = pBuf + len;

    while( ( packetLen = pEnd - p ) > 0 ) {
        if ( LSAPI_MAX_DATA_PACKET_LEN < packetLen) {
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
        if ( ret < totalLen ) {
            lsapi_close( pReq->m_fd );
            pReq->m_fd = -1;
            ret = -1;
        }
    }
    return p - pBuf;
}

static char * GetHeaderVar( LSAPI_Request * pReq, const char * name )
{
    int i;
    for( i = 0; i < H_TRANSFER_ENCODING; ++i ) {
        if ( pReq->m_pHeaderIndex->m_headerOff[i] ) {
            if ( strcmp( name, CGI_HEADERS[i] ) == 0 ) {
                return pReq->m_pHttpHeader + pReq->m_pHeaderIndex->m_headerOff[i];
            }
        }
    }
    if ( pReq->m_pHeader->m_cntUnknownHeaders > 0 ) {
        const char *p;
        char *pKey;
        char *pKeyEnd;
        int  keyLen;
        struct lsapi_header_offset * pCur, *pEnd;
        pCur = pReq->m_pUnknownHeader;
        pEnd = pCur + pReq->m_pHeader->m_cntUnknownHeaders;
        while( pCur < pEnd ) {
            pKey = pReq->m_pHttpHeader + pCur->nameOff;
            keyLen = pCur->nameLen;
            pKeyEnd = pKey + keyLen;
            p = &name[5];

            while(( pKey < pKeyEnd )&&( *p )) {
                char ch = toupper( *pKey );
                if ((ch != *p )||(( *p == '_' )&&( ch != '-'))) {
                    break;
                }
                ++p; ++pKey;
            }
            if (( pKey == pKeyEnd )&& (!*p )) {
                return pReq->m_pHttpHeader + pCur->valueOff;
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
    if ( !pReq || !name ) {
        return NULL;
    }
    if ( strncmp( name, "HTTP_", 5 ) == 0 ) {
        return GetHeaderVar( pReq, name );
    }
    while( pBegin < pEnd ) {
        if ( strcmp( name, pBegin->pKey ) == 0 ) {
            return pBegin->pValue;
        }
        ++pBegin;
    }
    return NULL;
}

int LSAPI_ForeachOrgHeader_r( LSAPI_Request * pReq,
            LSAPI_CB_EnvHandler fn, void * arg )
{
    int i;
    int len = 0;
    char * pValue;
    int ret;
    int count = 0;
    if ( !pReq || !fn ) {
        return -1;
    }
    for( i = 0; i < H_TRANSFER_ENCODING; ++i ) {
        if ( pReq->m_pHeaderIndex->m_headerOff[i] ) {
            len = pReq->m_pHeaderIndex->m_headerLen[i];
            pValue = pReq->m_pHttpHeader + pReq->m_pHeaderIndex->m_headerOff[i];
            *(pValue + len ) = 0;
            ret = (*fn)( HTTP_HEADERS[i], HTTP_HEADER_LEN[i],
                        pValue, len, arg );
            ++count;
            if ( ret <= 0 ) {
                return ret;
            }
        }
    }
    if ( pReq->m_pHeader->m_cntUnknownHeaders > 0 ) {
        char *pKey;
        int  keyLen;
        struct lsapi_header_offset * pCur, *pEnd;
        pCur = pReq->m_pUnknownHeader;
        pEnd = pCur + pReq->m_pHeader->m_cntUnknownHeaders;
        while( pCur < pEnd ) {
            pKey = pReq->m_pHttpHeader + pCur->nameOff;
            keyLen = pCur->nameLen;

            pValue = pReq->m_pHttpHeader + pCur->valueOff;
            *(pValue + pCur->valueLen ) = 0;
            ret = (*fn)( pKey, keyLen, 
                        pValue, pCur->valueLen, arg );
            if ( ret <= 0 ) {
                return ret;
            }
            ++pCur;
        }
    }
    return count + pReq->m_pHeader->m_cntUnknownHeaders;
    
}


int LSAPI_ForeachHeader_r( LSAPI_Request * pReq,
            LSAPI_CB_EnvHandler fn, void * arg )
{
    int i;
    int len = 0;
    char * pValue;
    int ret;
    int count = 0;
    if ( !pReq || !fn ) {
        return -1;
    }
    for( i = 0; i < H_TRANSFER_ENCODING; ++i ) {
        if ( pReq->m_pHeaderIndex->m_headerOff[i] ) {
            len = pReq->m_pHeaderIndex->m_headerLen[i];
            pValue = pReq->m_pHttpHeader + pReq->m_pHeaderIndex->m_headerOff[i];
            *(pValue + len ) = 0;
            ret = (*fn)( CGI_HEADERS[i], CGI_HEADER_LEN[i],
                        pValue, len, arg );
            ++count;
            if ( ret <= 0 ) {
                return ret;
            }
        }
    }
    if ( pReq->m_pHeader->m_cntUnknownHeaders > 0 ) {
        char achHeaderName[256];
        char *p;
        char *pKey;
        char *pKeyEnd ;
        int  keyLen;
        struct lsapi_header_offset * pCur, *pEnd;
        pCur = pReq->m_pUnknownHeader;
        pEnd = pCur + pReq->m_pHeader->m_cntUnknownHeaders;
        while( pCur < pEnd ) {
            pKey = pReq->m_pHttpHeader + pCur->nameOff;
            keyLen = pCur->nameLen;
            pKeyEnd = pKey + keyLen;
            memcpy( achHeaderName, "HTTP_", 5 );
            p = &achHeaderName[5];
            if ( keyLen > 250 ) {
                keyLen = 250;
            }

            while( pKey < pKeyEnd ) {
                char ch = *pKey++;
                if ( ch == '-' ) {
                    *p++ = '_';
                } else {
                    *p++ = toupper( ch );
                }
            }
            *p = 0;
            keyLen += 5;

            pValue = pReq->m_pHttpHeader + pCur->valueOff;
            *(pValue + pCur->valueLen ) = 0;
            ret = (*fn)( achHeaderName, keyLen, 
                        pValue, pCur->valueLen, arg );
            if ( ret <= 0 ) {
                return ret;
            }
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
    if ( !pEnv || !fn ) {
        return -1;
    }
    while( pEnv < pEnd ) {
        ret = (*fn)( pEnv->pKey, pEnv->keyLen,
                    pEnv->pValue, pEnv->valLen, arg );
        if ( ret <= 0 ) {
            return ret;
        }
        ++pEnv;
    }
    return n;
}



int LSAPI_ForeachEnv_r( LSAPI_Request * pReq,
            LSAPI_CB_EnvHandler fn, void * arg )
{
    if ( !pReq || !fn ) {
        return -1;
    }
    if ( pReq->m_pHeader->m_cntEnv > 0 ) {
        return EnvForeach( pReq->m_pEnvList, pReq->m_pHeader->m_cntEnv,
                    fn, arg );
    }
    return 0;
}



int LSAPI_ForeachSpecialEnv_r( LSAPI_Request * pReq,
            LSAPI_CB_EnvHandler fn, void * arg )
{
    if ( !pReq || !fn ) {
        return -1;
    }
    if ( pReq->m_pHeader->m_cntSpecialEnv > 0 ) {
        return EnvForeach( pReq->m_pSpecialEnvList,
                pReq->m_pHeader->m_cntSpecialEnv,
                    fn, arg );
    }
    return 0;

}



int LSAPI_FinalizeRespHeaders_r( LSAPI_Request * pReq )
{
    if ( !pReq || !pReq->m_pIovec ) {
        return -1;
    }
    if ( !( pReq->m_reqState & LSAPI_ST_RESP_HEADER ) ) {
        return 0;
    }
    pReq->m_reqState &= ~LSAPI_ST_RESP_HEADER;
    if ( pReq->m_pRespHeaderBufPos > pReq->m_pRespHeaderBuf ) {
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




int LSAPI_AppendRespHeader_r( LSAPI_Request * pReq, char * pBuf, int len )
{
    if ( !pReq || !pBuf || len <= 0 || len > LSAPI_RESP_HTTP_HEADER_MAX ) {
        return -1;
    }
    if ( pReq->m_reqState & LSAPI_ST_RESP_BODY ) {
        return -1;
    }
    if ( pReq->m_respHeader.m_respInfo.m_cntHeaders >= LSAPI_MAX_RESP_HEADERS ) {
        return -1;
    }
    if ( pReq->m_pRespHeaderBufPos + len + 1 > pReq->m_pRespHeaderBufEnd ) {
        int newlen = pReq->m_pRespHeaderBufPos + len + 4096 - pReq->m_pRespHeaderBuf;
        newlen -= newlen % 4096;
        if ( allocateRespHeaderBuf( pReq, newlen ) == -1 ) {
            return -1;
        }
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

    switch( pServerAddr->sa_family ) {
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
    if ( fd == -1 ) {
        return -1;
    }

    fcntl( fd, F_SETFD, FD_CLOEXEC );

    if(setsockopt( fd, SOL_SOCKET, SO_REUSEADDR,
                (char *)( &flag ), sizeof(flag)) == 0) {
        ret = bind( fd, pServerAddr, addr_len );
        if ( !ret ) {
            ret = listen( fd, backlog );
            if ( !ret ) {
                return fd;
            }
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
    
    if ( !pBind ) {
        return -1;
    }

    while( isspace( *pBind ) ) {
        ++pBind;
    }

    strncpy( achAddr, pBind, 256 );

    switch( *p ) {
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
        
        if ( *p == '*' ) {
            strcpy( achAddr, "::" );
            p = achAddr;
        }
        doAddrInfo = 1;
        break;

    default:
        pAddr->sa_family = AF_INET;
        pEnd = strchr( p, ':' );
        if ( !pEnd ) {
            return -1;
        }
        *pEnd++ = 0;
        
        doAddrInfo = 0;
        if ( *p == '*' ) {
            ((struct sockaddr_in *)pAddr)->sin_addr.s_addr = htonl(INADDR_ANY);
        } else {
            if (!strcasecmp( p, "localhost" ) ) {
                ((struct sockaddr_in *)pAddr)->sin_addr.s_addr = htonl( INADDR_LOOPBACK );
            } else  {
                ((struct sockaddr_in *)pAddr)->sin_addr.s_addr = inet_addr( p );
                if ( ((struct sockaddr_in *)pAddr)->sin_addr.s_addr == INADDR_BROADCAST) {
                    doAddrInfo = 1;
                }
            }
        }
        break;
    }
    if ( *pEnd == ':' ) {
        ++pEnd;
    }
        
    port = atoi( pEnd );
    if (( port <= 0 )||( port > 65535 )) {
        return -1;
    }
    if ( doAddrInfo ) {

        memset(&hints, 0, sizeof(hints));

        hints.ai_family   = pAddr->sa_family;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        if ( getaddrinfo(p, NULL, &hints, &res) ) {
            return -1;
        }

        memcpy(pAddr, res->ai_addr, res->ai_addrlen);
        freeaddrinfo(res);
    }
    
    if ( pAddr->sa_family == AF_INET ) {
        ((struct sockaddr_in *)pAddr)->sin_port = htons( port );
    } else {
        ((struct sockaddr_in6 *)pAddr)->sin6_port = htons( port );
    }
    return 0;
    
}

int LSAPI_CreateListenSock( const char * pBind, int backlog )
{
    char serverAddr[128];
    int ret;
    int fd = -1;
    ret = LSAPI_ParseSockAddr( pBind, (struct sockaddr *)serverAddr );
    if ( !ret ) {
        fd = LSAPI_CreateListenSock2( (struct sockaddr *)serverAddr, backlog );
    }
    return fd;
}

static fn_select_t g_fnSelect = select;

typedef struct _lsapi_child_status
{
    int     m_pid;

    volatile short   m_iKillSent;
    volatile short   m_inProcess;

    volatile long    m_tmWaitBegin;
    volatile long    m_tmReqBegin;
    volatile long    m_tmLastCheckPoint;
}
lsapi_child_status;

static lsapi_child_status * s_pChildStatus = NULL;

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
    
}lsapi_prefork_server;

static lsapi_prefork_server * g_prefork_server = NULL;

int LSAPI_Init_Prefork_Server( int max_children, fn_select_t fp, int avoidFork )
{
    if ( g_prefork_server ) {
        return 0;
    }
    if ( max_children <= 1 ) {
        return -1;
    }
    if ( max_children >= 10000) {
        max_children = 10000;
    }

    
    g_prefork_server = (lsapi_prefork_server *)malloc( sizeof( lsapi_prefork_server ) );
    if ( !g_prefork_server ) {
        return -1;
    }
    memset( g_prefork_server, 0, sizeof( lsapi_prefork_server ) );

    if ( fp != NULL ) {
        g_fnSelect = fp;
    }

    s_ppid = getppid();
    g_prefork_server->m_iAvoidFork = avoidFork;
    g_prefork_server->m_iMaxChildren = max_children;
    
    g_prefork_server->m_iExtraChildren = ( avoidFork ) ? 0 : (max_children / 3) ;
    g_prefork_server->m_iMaxIdleChildren = ( avoidFork ) ? (max_children + 1) : (max_children / 3);
    g_prefork_server->m_iChildrenMaxIdleTime = 300;
    g_prefork_server->m_iMaxReqProcessTime = 300;
    return 0;    
}

void LSAPI_Set_Server_fd( int fd )
{
    if( g_prefork_server ) {
        g_prefork_server->m_fd = fd;
    }
}


static int lsapi_accept( int fdListen )
{
    int         fd;
    int         nodelay = 1;
    socklen_t   len;
    char        achPeer[128];

    len = sizeof( achPeer );
    fd = accept( fdListen, (struct sockaddr *)&achPeer, &len );
    if ( fd != -1 ) {
        if (((struct sockaddr *)&achPeer)->sa_family == AF_INET ) {
            setsockopt( fd, IPPROTO_TCP, TCP_NODELAY,
                    (char *)&nodelay, sizeof(nodelay));
        }
    }
    return fd;

}




static int s_req_processed = 0;
static int s_max_reqs = 10000;
static int s_max_idle_secs = 300;

static int s_stop;

static void lsapi_cleanup(int signal)
{
    s_stop = signal;
}

static lsapi_child_status * find_child_status( int pid )
{
    lsapi_child_status * pStatus = g_prefork_server->m_pChildrenStatus;
    lsapi_child_status * pEnd = g_prefork_server->m_pChildrenStatus + g_prefork_server->m_iMaxChildren * 2;
    while( pStatus < pEnd ) {
        if ( pStatus->m_pid == pid ) {
            return pStatus;
        }
        ++pStatus;
    }
    return NULL;
}



static void lsapi_sigchild( int signal )
{
    int status, pid;
    lsapi_child_status * child_status;
    while( 1 ) {
        pid = waitpid( -1, &status, WNOHANG|WUNTRACED );
        if ( pid <= 0 ) {
            break;
        }
        child_status = find_child_status( pid );
        if ( child_status ) {
            child_status->m_pid = 0;
        }
        --g_prefork_server->m_iCurChildren;
    }

}

static int lsapi_init_children_status()
{
    int size = 4096;
    
    char * pBuf;
    size = g_prefork_server->m_iMaxChildren * sizeof( lsapi_child_status ) * 2;
    size = (size + 4095 ) / 4096 * 4096;
    pBuf =( char*) mmap( NULL, size, PROT_READ | PROT_WRITE,
        MAP_ANON | MAP_SHARED, -1, 0 );
    if ( pBuf == MAP_FAILED ) {
        perror( "Anonymous mmap() failed" );
        return -1;
    }
    memset( pBuf, 0, size );
    g_prefork_server->m_pChildrenStatus = (lsapi_child_status *)pBuf;    
    return 0;
}

static void lsapi_check_child_status( long tmCur )
{
    int idle = 0;
    int tobekilled;
    int dying = 0;
    lsapi_child_status * pStatus = g_prefork_server->m_pChildrenStatus;
    lsapi_child_status * pEnd = g_prefork_server->m_pChildrenStatus + g_prefork_server->m_iMaxChildren * 2;
    while( pStatus < pEnd ) {
        tobekilled = pStatus->m_iKillSent;
        if ( pStatus->m_pid != 0 ) {
            if ( !tobekilled ) {
                if ( !pStatus->m_inProcess ) {
                    
                    if (( g_prefork_server->m_iCurChildren - dying > g_prefork_server->m_iMaxChildren)||
                        ( idle >= g_prefork_server->m_iMaxIdleChildren )) {
                    
                        tobekilled = 1;
                    } else {
                        if (( s_max_idle_secs> 0)&&(tmCur - pStatus->m_tmWaitBegin > s_max_idle_secs + 5 )) {
                            tobekilled = 1;
                        }
                    }
                    if ( !tobekilled ) {
                        ++idle;
                    }
                } else {
                    if ( tmCur - pStatus->m_tmReqBegin > 
                         g_prefork_server->m_iMaxReqProcessTime ) {
                        tobekilled = 1;
                    }
                }
            } else {
                if ( pStatus->m_inProcess ) {
                    tobekilled = pStatus->m_iKillSent = 0;
                }
            }
            if ( tobekilled ) {
                tobekilled = 0;
                if ( pStatus->m_iKillSent > 5 ) {
                    tobekilled = SIGKILL;
                } else {    
                    if ( pStatus->m_iKillSent == 3 ) {
                        tobekilled = SIGTERM;
                    } else {
                        if ( pStatus->m_iKillSent == 1 ) {
                            tobekilled = SIGUSR1;
                        }
                    }
                }
                if ( tobekilled ) {
                    kill( pStatus->m_pid, tobekilled );
                }
                ++pStatus->m_iKillSent;
                ++dying;
            }
                
        } else {
            ++dying;
        }
        ++pStatus;
    }
}

static int lsapi_all_children_must_die()
{
    int maxWait;
    int sec =0;
    g_prefork_server->m_iMaxReqProcessTime = 10;
    g_prefork_server->m_iMaxIdleChildren = -1;
    maxWait = 15;

    while( g_prefork_server->m_iCurChildren && (sec < maxWait) ) {
        lsapi_check_child_status(time(NULL));
        sleep( 1 );
        sec++;
    }
    if ( g_prefork_server->m_iCurChildren != 0 ) {
        kill( -getpgrp(), SIGKILL );
    }
    return 0;
}



static int lsapi_prefork_server_accept( lsapi_prefork_server * pServer, LSAPI_Request * pReq )
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

    lsapi_init_children_status();
    
    setsid();

    act.sa_flags = 0;
    act.sa_handler = lsapi_sigchild;
    if( sigaction( SIGCHLD, &act, &old_child ) ) {
        perror( "Can't set signal handler for SIGCHILD" );
        return -1;
    }

    /* Set up handler to kill children upon exit */
    act.sa_flags = 0;
    act.sa_handler = lsapi_cleanup;
    if( sigaction( SIGTERM, &act, &old_term ) ||
        sigaction( SIGINT,  &act, &old_int  ) ||
        sigaction( SIGUSR1, &act, &old_usr1 ) ||
        sigaction( SIGQUIT, &act, &old_quit )) {
        perror( "Can't set signals" );
        return -1;
    }
    s_stop = 0;
    while( !s_stop ) {
        if ( ret ) {
            curTime = time( NULL );
        } else {
            ++curTime;
        }
        if (curTime != lastTime ) {
            lastTime = curTime;
            if (s_ppid && (getppid() != s_ppid )) {
                break;
            }
            lsapi_check_child_status(curTime );
            if (pServer->m_iServerMaxIdle) {
                if ( pServer->m_iCurChildren <= 0 ) {
                    ++wait_secs;
                    if ( wait_secs > pServer->m_iServerMaxIdle ) {
                        return -1;
                    }
                } else {
                    wait_secs = 0;
                }
            }
        }

        if ( pServer->m_iCurChildren >= (pServer->m_iMaxChildren + pServer->m_iExtraChildren ) ) {
            usleep( 100000 );
            continue;
        }

        FD_ZERO( &readfds );
        FD_SET( pServer->m_fd, &readfds );
        timeout.tv_sec = 1; timeout.tv_usec = 0;
        if ((ret = (*g_fnSelect)(pServer->m_fd+1, &readfds, NULL, NULL, &timeout)) == 1 ) {
            if ( pServer->m_iCurChildren >= 0 ) {
                usleep( 10 );
                FD_ZERO( &readfds );
                FD_SET( pServer->m_fd, &readfds );
                timeout.tv_sec = 0; timeout.tv_usec = 0;
                if ( (*g_fnSelect)(pServer->m_fd+1, &readfds, NULL, NULL, &timeout) == 0 ) {
                    continue;
                }
            }
        } else {
            if ( ret == -1 ) {
                if ( errno == EINTR ) {
                    continue;
                }
                /* perror( "select()" ); */
                break;
            } else {
                continue;
            }
        }

        pReq->m_fd = lsapi_accept( pServer->m_fd );
        if ( pReq->m_fd != -1 ) {
            child_status = find_child_status( 0 );
            pid = fork();
            if ( !pid ) {
                g_prefork_server = NULL;
                s_ppid = getppid();
                s_req_processed = 0;
                s_pChildStatus = child_status;
                child_status->m_iKillSent = 0;
                lsapi_set_nblock( pReq->m_fd, 0 );

                /* don't catch our signals */
                sigaction( SIGCHLD, &old_child, 0 );
                sigaction( SIGTERM, &old_term, 0 );
                sigaction( SIGQUIT, &old_quit, 0 );
                sigaction( SIGINT,  &old_int,  0 );
                sigaction( SIGUSR1, &old_usr1, 0 );
                return 0;
            } else {
                if ( pid == -1 ) {
                    perror( "fork() failed, please increase process limit" );
                } else {
                    ++pServer->m_iCurChildren;
                    if ( child_status ) {
                        child_status->m_pid = pid;
                        child_status->m_iKillSent = 0;
                        child_status->m_tmWaitBegin = time(NULL);
                    }
                }
            }
            close( pReq->m_fd );
            pReq->m_fd = -1;

        } else {
            if (( errno == EINTR )||( errno == EAGAIN)) {
                continue;
            }
            perror( "accept() failed" );
            return -1;
        }
    }
    sigaction( SIGUSR1, &old_usr1, 0 );
    kill( -getpgrp(), SIGUSR1 );
    lsapi_all_children_must_die();  /* Sorry, children ;-) */
    return -1;

}

int LSAPI_Prefork_Accept_r( LSAPI_Request * pReq )
{
    int             fd;
    int             ret;
    int             wait_secs;
    fd_set          readfds;
    struct timeval  timeout;

    LSAPI_Finish_r( pReq );


    if ( g_prefork_server ) {
        if ( g_prefork_server->m_fd != -1 ) {
            if ( lsapi_prefork_server_accept( g_prefork_server, pReq ) == -1 ) {
                return -1;
            }
        }
    }
    if ( s_req_processed >= s_max_reqs ) {
        return -1;
    }

    if ( s_pChildStatus ) {
        s_pChildStatus->m_tmWaitBegin = time( NULL );
    }
    
    while( g_running ) {
        if ( pReq->m_fd != -1 ) {
            fd = pReq->m_fd;
        } else {
            if ( pReq->m_fdListen != -1 ) {
                fd = pReq->m_fdListen;
            } else {
                return -1;
            }
        }
        wait_secs = 0;
        while( 1 ) {
            if ( !g_running ) {
                return -1;
            }
            if (( s_pChildStatus )&&( s_pChildStatus->m_iKillSent )) {
                return -1; 
            }
            FD_ZERO( &readfds );
            FD_SET( fd, &readfds );
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;
            ret = (*g_fnSelect)(fd+1, &readfds, NULL, NULL, &timeout);
            if ( ret == 0 ) {
                if ( s_pChildStatus ) {
                    s_pChildStatus->m_inProcess = 0;
                }
                ++wait_secs;
                if (( s_max_idle_secs > 0 )&&(wait_secs >= s_max_idle_secs )) {
                    return -1;
                }
                if ( s_ppid &&( getppid() != s_ppid)) {
                    return -1;
                }
            } else {
                if ( ret == -1 ) {
                    if ( errno == EINTR ) {
                        continue;
                    } else {
                        return -1;
                    }
                } else {
                    if ( ret >= 1 ) {
                        if (( s_pChildStatus )&&( s_pChildStatus->m_iKillSent )) {
                            return -1; 
                        }
                        if ( fd == pReq->m_fdListen ) {
                            pReq->m_fd = lsapi_accept( pReq->m_fdListen );
                            if ( pReq->m_fd != -1 ) {
                                fd = pReq->m_fd;
                                lsapi_set_nblock( fd, 0 );
                            } else {
                                if (( errno == EINTR )||( errno == EAGAIN)) {
                                    continue;
								}
                                return -1;
                            }
                        } else {
                            break;
                        }
                    }
                }
            }
        }
        if ( !readReq( pReq ) ) {
            if ( s_pChildStatus ) {
                s_pChildStatus->m_inProcess = 1;
                s_pChildStatus->m_tmReqBegin = s_pChildStatus->m_tmLastCheckPoint = time(NULL);
            }
            ++s_req_processed;
            return 0;
        }
        lsapi_close( pReq->m_fd );
        pReq->m_fd = -1;
        LSAPI_Reset_r( pReq );
    }
    return -1;
    
}

void LSAPI_Set_Max_Reqs( int reqs )
{   
    s_max_reqs = reqs;          
}

void LSAPI_Set_Max_Idle( int secs )
{   
    s_max_idle_secs = secs;     
}

void LSAPI_Set_Max_Children( int maxChildren )
{
    if ( g_prefork_server ) {
        g_prefork_server->m_iMaxChildren = maxChildren;
    }
}

void LSAPI_Set_Extra_Children( int extraChildren )
{
    if (( g_prefork_server )&&( extraChildren >= 0 )) {
        g_prefork_server->m_iExtraChildren = extraChildren;
    }
}

void LSAPI_Set_Max_Process_Time( int secs )
{
    if (( g_prefork_server )&&( secs > 0 )) {
        g_prefork_server->m_iMaxReqProcessTime = secs;
    }
}


void LSAPI_Set_Max_Idle_Children( int maxIdleChld )
{
    if (( g_prefork_server )&&( maxIdleChld > 0 )) {
        g_prefork_server->m_iMaxIdleChildren = maxIdleChld;
    }
}

void LSAPI_Set_Server_Max_Idle_Secs( int serverMaxIdle )
{
    if ( g_prefork_server ) {
        g_prefork_server->m_iServerMaxIdle = serverMaxIdle;
    }
}


void LSAPI_No_Check_ppid()
{
    s_ppid = 0;
}

#if defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
#include <crt_externs.h>
#else
extern char ** environ;
#endif
static void unset_lsapi_envs()
{
    char **env;
#if defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
    env = *_NSGetEnviron();
#else
    env = environ;
#endif
    while( env != NULL && *env != NULL ) {
        if ( !strncmp(*env, "LSAPI_", 6) || 
             !strncmp( *env, "PHP_LSAPI_", 10 ) ) {   
            char ** del = env;
            do {
                *del = del[1];
            } while( *del++ );
        } else {
            ++env;
        }
    }  
}

void LSAPI_Init_Env_Parameters( fn_select_t fp )
{
    const char *p;
    int n;
    int avoidFork = 0;
    p = getenv( "PHP_LSAPI_MAX_REQUESTS" );
    if ( !p ) {
        p = getenv( "LSAPI_MAX_REQS" );
    }
    if ( p ) {
        n = atoi( p );
        if ( n > 0 ) {
            LSAPI_Set_Max_Reqs( n );
        }
    }

    p = getenv( "LSAPI_AVOID_FORK" );
    if ( p ) {
        avoidFork = atoi( p );
    }    

#if defined( RLIMIT_CORE )
    p = getenv( "LSAPI_ALLOW_CORE_DUMP" );
    if ( !p ) {
        struct rlimit limit = { 0, 0 };
        setrlimit( RLIMIT_CORE, &limit );
    }
#endif    

    p = getenv( "LSAPI_MAX_IDLE" );
    if ( p ) {
        n = atoi( p );
        LSAPI_Set_Max_Idle( n );
    }

    if ( LSAPI_Is_Listen() ) {
        n = 0;
        p = getenv( "PHP_LSAPI_CHILDREN" );
        if ( !p ) {
            p = getenv( "LSAPI_CHILDREN" );
        }
        if ( p ) {
            n = atoi( p );
        }
        if ( n > 1 ) {
            LSAPI_Init_Prefork_Server( n, fp, avoidFork );
            LSAPI_Set_Server_fd( g_req.m_fdListen );
        }

        p = getenv( "LSAPI_EXTRA_CHILDREN" );
        if ( p ) {
            LSAPI_Set_Extra_Children( atoi( p ) );
        }
        
        p = getenv( "LSAPI_MAX_IDLE_CHILDREN" );
        if ( p ) {
            LSAPI_Set_Max_Idle_Children( atoi( p ) );
        }
        p = getenv( "LSAPI_PGRP_MAX_IDLE" );
        if ( p ) {
            LSAPI_Set_Server_Max_Idle_Secs( atoi( p ) );
        }
        
        p = getenv( "LSAPI_MAX_PROCESS_TIME" );
        if ( p ) {     
            LSAPI_Set_Max_Process_Time( atoi( p ) );
        }
        if ( getenv( "LSAPI_PPID_NO_CHECK" ) ) {
            LSAPI_No_Check_ppid();
        }
    }
    unset_lsapi_envs();
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */


