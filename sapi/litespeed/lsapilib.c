/*
Copyright (c) 2005, Lite Speed Technologies Inc.
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

/***************************************************************************
                          lsapilib.c  -  description
                             -------------------
    begin                : Mon Feb 21 2005
    copyright            : (C) 2005 by George Wang
    email                : gwang@litespeedtech.com
 ***************************************************************************/


#include <lsapilib.h>

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>

//#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <unistd.h>

#define LSAPI_ST_REQ_HEADER     1
#define LSAPI_ST_REQ_BODY       2
#define LSAPI_ST_RESP_HEADER    4
#define LSAPI_ST_RESP_BODY      8

#define LSAPI_RESP_BUF_SIZE     8192
#define LSAPI_INIT_RESP_HEADER_LEN 4096


static int g_inited = 0;
static int g_running = 1;
LSAPI_Request g_req;

void Flush_RespBuf_r( LSAPI_Request * pReq );

static const char *CGI_HEADERS[H_TRANSFER_ENCODING+1] =
{
    "HTTP_ACCEPT", "HTTP_ACCEPT_CHARSET",
    "HTTP_ACCEPT_ENCODING",
    "HTTP_ACCEPT_LANG", "HTTP_AUTHORIZATION",
    "HTTP_CONNECTION", "CONTENT_TYPE",
    "CONTENT_LENGTH", "HTTP_COOKIE", "HTTP_COOKIE2",
    "HTTP_HOST", "HTTP_PRAGMA",
    "HTTP_REFERER", "HTTP_USER_AGENT",
    "HTTP_CACHE_CTRL",
    "HTTP_IF_MODIFIED_SINCE", "HTTP_IF_MATCH",
    "HTTP_IF_NONE_MATCH",
    "HTTP_IF_RANGE",
    "HTTP_IF_UNMODIFIED_SINCE",
    "HTTP_KEEPALIVE",
    "HTTP_RANGE",
    "HTTP_X_FORWARDED_FOR",
    "HTTP_VIA",
    "HTTP_TRANSFER_ENCODING"
};
     
static int CGI_HEADER_LEN[H_TRANSFER_ENCODING+1] =
{    11, 19, 20, 16, 18, 15, 12, 14, 11, 12, 9, 11, 12, 15, 15,
     22, 13, 18, 13, 24, 14, 10, 20, 8, 22 };

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


static inline void lsapi_buildPacketHeader( struct lsapi_packet_header * pHeader,
                                char type, int len )
{
    pHeader->m_versionB0 = LSAPI_VERSION_B0;      //LSAPI protocol version
    pHeader->m_versionB1 = LSAPI_VERSION_B1;
    pHeader->m_type      = type;
    pHeader->m_flag      = LSAPI_ENDIAN;
    pHeader->m_packetLen.m_iLen = len;
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

static inline int lsapi_read( int fd, void * pBuf, int len )
{
    int ret;
    while( 1 )
    {
        ret = read( fd, (char *)pBuf, len );
        if (( ret == -1 )&&( errno == EINTR )&&(g_running))
            continue;
        return ret;
    }
}

//static int lsapi_write( int fd, const void * pBuf, int len )
//{
//    int ret;
//    const char * pCur;
//    const char * pEnd;
//    if ( len == 0 )
//        return 0;
//    pCur = (const char *)pBuf;
//    pEnd = pCur + len;
//    while( g_running && (pCur < pEnd) )
//    {
//        ret = write( fd, pCur, pEnd - pCur );
//        if ( ret >= 0)
//            pCur += ret;
//        else if (( ret == -1 )&&( errno != EINTR ))
//            return ret;
//    }
//    return pCur - (const char *)pBuf;
//}

static int lsapi_writev( int fd, struct iovec ** pVec, int count, int totalLen )
{
    int ret;
    int left = totalLen;
    int n = count;
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
                if ( (*pVec)->iov_len <= ret )
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
        else if (( ret == -1 )&&( errno != EINTR ))
            return ret;
    }
    return totalLen - left;
}

//static int getTotalLen( struct iovec * pVec, int count )
//{
//    struct iovec * pEnd = pVec + count;
//    int total = 0;
//    while( pVec < pEnd )
//    {
//        total += pVec->iov_len;
//        ++pVec;
//    }
//    return total;
//}


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
    char achPeer[128];
    int len = 128;
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
        return -1;

    if ( shouldFixEndian )
    {
        fixHeaderIndexEndian( pReq );
    }

    return 0;
}

static struct lsapi_packet_header ack = {'L', 'S',
                LSAPI_REQ_RECEIVED, LSAPI_ENDIAN, {LSAPI_PACKET_HEADER_LEN} };
static inline int notify_req_received( LSAPI_Request * pReq )
{
    if ( write( pReq->m_fd, &ack, LSAPI_PACKET_HEADER_LEN )
                < LSAPI_PACKET_HEADER_LEN )
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
        return -1;
    if ( packetLen > LSAPI_MAX_HEADER_LEN )
        return -1;

    if ( packetLen > pReq->m_reqBufSize )
    {
        if ( allocateBuf( pReq, packetLen ) == -1 )
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
        return -1;
    pReq->m_bufProcessed = packetLen;
    pReq->m_reqState = LSAPI_ST_REQ_BODY | LSAPI_ST_RESP_HEADER;

    return notify_req_received( pReq );
}



int LSAPI_Init(void)
{
    if ( !g_inited )
    {
        lsapi_signal(SIGPIPE, lsapi_sigpipe);
        lsapi_signal(SIGUSR1, lsapi_siguser1);

        if ( LSAPI_InitRequest( &g_req, LSAPI_SOCK_FILENO ) == -1 )
            return -1;
        g_inited = 1;
    }
    return 0;
}

void LSAPI_stop(void)
{
    g_running = 0;
}

int LSAPI_InitRequest( LSAPI_Request * pReq, int fd )
{
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
 
    if ( isPipe( fd ) )
    {
        pReq->m_fdListen = -1;
        pReq->m_fd = fd;
    }
    else
    {
        pReq->m_fdListen = fd;
        pReq->m_fd = -1;
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
    char achPeer[128];
    int  len;
    int  nodelay = 1;
    if ( !pReq )
        return -1;
    if ( LSAPI_Finish_r( pReq ) == -1 )
        return -1;
    while( g_running )
    {
        if ( pReq->m_fd == -1 )
        {
            if ( pReq->m_fdListen != -1)
            {
                len = sizeof( achPeer );
                pReq->m_fd = accept( pReq->m_fdListen,
                            (struct sockaddr *)&achPeer, &len );
                if (( pReq->m_fd == -1 )&&( errno == EINTR ))
                    continue;
                if (( pReq->m_fd != -1 )&&
                    (((struct sockaddr *)&achPeer)->sa_family == AF_INET ))
                {    
                    setsockopt(pReq->m_fd, IPPROTO_TCP, TCP_NODELAY,
                                (char *)&nodelay, sizeof(nodelay));
                }
            }
            else
                return -1;
        }
        if ( !readReq( pReq ) )
            break;
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
    //finish req body
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
	if ( *(pReq->m_pHttpHeader + off + 
		 pReq->m_pHeaderIndex->m_headerLen[ headerIndex ]) )
		*( pReq->m_pHttpHeader + off + 
			pReq->m_pHeaderIndex->m_headerLen[ headerIndex ]) = 0;
    return pReq->m_pHttpHeader + off;
}



int LSAPI_ReadReqBody_r( LSAPI_Request * pReq, char * pBuf, int bufLen )
{
    int len;
    int total;
    //char *pOldBuf = pBuf;
    if (!pReq || ( !pBuf )||(bufLen < 0 ))
        return -1;

    total = pReq->m_pHeader->m_reqBodyLen - pReq->m_reqBodyRead;
    
    if ( total <= 0 )
        return 0;
    if ( total < bufLen )
        bufLen = total;

    total = 0;
    len = pReq->m_bufRead - pReq->m_bufProcessed;
    if ( len > 0 )
    {
        if ( len > bufLen )
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
        else if ( len < 0 )
            return -1;
    }
    pReq->m_reqBodyRead += total;
    return total;
        
}


//int LSAPI_Write( const char * pBuf, int len )
//{
//    return LSAPI_Write_r( &g_req, pBuf, len );
//}

int LSAPI_Write_r( LSAPI_Request * pReq, const char * pBuf, int len )
{
    struct lsapi_packet_header * pHeader;
    const char * pEnd;
    const char * p;
    int bufLen;
    int toWrite;
    int packetLen;
    
    if ( !pReq || !pBuf )
        return -1;
    if ( len < pReq->m_pRespBufEnd - pReq->m_pRespBufPos )
    {
        memmove( pReq->m_pRespBufPos, pBuf, len );
        pReq->m_pRespBufPos += len;
        return len;
    }
    
    if ( pReq->m_reqState & LSAPI_ST_RESP_HEADER )
    {
        LSAPI_FinalizeRespHeaders_r( pReq );
    }
    pReq->m_reqState |= LSAPI_ST_RESP_BODY;
    
    pHeader = pReq->m_respPktHeader;
    p       = pBuf;
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
            lsapi_close( pReq->m_fd );
            pReq->m_fd = -1;
            ret = -1;
        }
    }
    return p - pBuf;
}


char * LSAPI_GetEnv_r( LSAPI_Request * pReq, const char * name )
{
    struct LSAPI_key_value_pair * pBegin = pReq->m_pEnvList;
    struct LSAPI_key_value_pair * pEnd = pBegin + pReq->m_pHeader->m_cntEnv;
    if ( !pReq || !name )
        return NULL;
    while( pBegin < pEnd )
    {
        if ( strcmp( name, pBegin->pKey ) == 0 )
            return pBegin->pValue;
        ++pBegin;
    }
    return NULL;
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
            pKeyEnd = pKey + keyLen;
            memcpy( achHeaderName, "HTTP_", 5 );
            p = &achHeaderName[5];
            if ( keyLen > 250 )
                keyLen = 250;

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
            ret = (*fn)( achHeaderName, pCur->valueLen,
                        pValue, len, arg );
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




int LSAPI_AppendRespHeader_r( LSAPI_Request * pReq, char * pBuf, int len )
{
    if ( !pReq || !pBuf || len <= 0 || len > LSAPI_RESP_HTTP_HEADER_MAX )
        return -1;
    if ( pReq->m_reqState & LSAPI_ST_RESP_BODY )
        return -1;
    if ( pReq->m_respHeader.m_respInfo.m_cntHeaders >= LSAPI_MAX_RESP_HEADERS )
        return -1;
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
    ++len;  //add one byte padding for \0
    pReq->m_respHeaderLen[pReq->m_respHeader.m_respInfo.m_cntHeaders] = len;
    ++pReq->m_respHeader.m_respInfo.m_cntHeaders;
    return 0;
}






