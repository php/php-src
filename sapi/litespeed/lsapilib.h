/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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


#ifndef  _LSAPILIB_H_
#define  _LSAPILIB_H_

#if defined (c_plusplus) || defined (__cplusplus)
extern "C" {
#endif

#include "lsapidef.h"

#include <stddef.h>
#include <sys/time.h>
#include <sys/types.h>

struct LSAPI_key_value_pair
{
    char * pKey;
    char * pValue;
    int    keyLen;
    int    valLen;
};

struct lsapi_child_status;
#define LSAPI_MAX_RESP_HEADERS  1000

typedef struct lsapi_request
{
    int               m_fdListen;
    int               m_fd;

    long              m_lLastActive;
    long              m_lReqBegin;

    char            * m_pReqBuf;
    int               m_reqBufSize;

    char            * m_pRespBuf;
    char            * m_pRespBufEnd;
    char            * m_pRespBufPos;

    char            * m_pRespHeaderBuf;
    char            * m_pRespHeaderBufEnd;
    char            * m_pRespHeaderBufPos;
    struct lsapi_child_status * child_status;


    struct iovec    * m_pIovec;
    struct iovec    * m_pIovecEnd;
    struct iovec    * m_pIovecCur;
    struct iovec    * m_pIovecToWrite;

    struct lsapi_packet_header      * m_respPktHeaderEnd;

    struct lsapi_req_header         * m_pHeader;
    struct LSAPI_key_value_pair     * m_pEnvList;
    struct LSAPI_key_value_pair     * m_pSpecialEnvList;
    int                               m_envListSize;
    int                               m_specialEnvListSize;

    struct lsapi_http_header_index  * m_pHeaderIndex;
    struct lsapi_header_offset      * m_pUnknownHeader;

    char            * m_pScriptFile;
    char            * m_pScriptName;
    char            * m_pQueryString;
    char            * m_pHttpHeader;
    char            * m_pRequestMethod;
    int               m_totalLen;
    int               m_reqState;
    off_t             m_reqBodyLen;
    off_t             m_reqBodyRead;
    int               m_bufProcessed;
    int               m_bufRead;

    struct lsapi_packet_header        m_respPktHeader[5];

    struct lsapi_resp_header          m_respHeader;
    short                             m_respHeaderLen[LSAPI_MAX_RESP_HEADERS];
    void            * m_pAppData;

}LSAPI_Request;

extern LSAPI_Request g_req;


/* return: >0 continue, ==0 stop, -1 failed  */
typedef int (*LSAPI_CB_EnvHandler )( const char * pKey, int keyLen,
                const char * pValue, int valLen, void * arg );


int LSAPI_Init(void);

void LSAPI_Stop(void);

int LSAPI_Is_Listen_r( LSAPI_Request * pReq);

int LSAPI_InitRequest( LSAPI_Request * pReq, int fd );

int LSAPI_Accept_r( LSAPI_Request * pReq );

void LSAPI_Reset_r( LSAPI_Request * pReq );

int LSAPI_Finish_r( LSAPI_Request * pReq );

int LSAPI_Release_r( LSAPI_Request * pReq );

char * LSAPI_GetHeader_r( LSAPI_Request * pReq, int headerIndex );

int LSAPI_ForeachHeader_r( LSAPI_Request * pReq,
            LSAPI_CB_EnvHandler fn, void * arg );

int LSAPI_ForeachOrgHeader_r( LSAPI_Request * pReq,
            LSAPI_CB_EnvHandler fn, void * arg );

int LSAPI_ForeachEnv_r( LSAPI_Request * pReq,
            LSAPI_CB_EnvHandler fn, void * arg );

int LSAPI_ForeachSpecialEnv_r( LSAPI_Request * pReq,
            LSAPI_CB_EnvHandler fn, void * arg );

char * LSAPI_GetEnv_r( LSAPI_Request * pReq, const char * name );

ssize_t LSAPI_ReadReqBody_r( LSAPI_Request * pReq, char * pBuf, size_t len );

int LSAPI_ReqBodyGetChar_r( LSAPI_Request * pReq );

int LSAPI_ReqBodyGetLine_r( LSAPI_Request * pReq, char * pBuf, size_t bufLen, int *getLF );


int LSAPI_FinalizeRespHeaders_r( LSAPI_Request * pReq );

ssize_t LSAPI_Write_r( LSAPI_Request * pReq, const char * pBuf, size_t len );

ssize_t LSAPI_sendfile_r( LSAPI_Request * pReq, int fdIn, off_t* off, size_t size );

ssize_t LSAPI_Write_Stderr_r( LSAPI_Request * pReq, const char * pBuf, size_t len );

int LSAPI_Flush_r( LSAPI_Request * pReq );

int LSAPI_AppendRespHeader_r( LSAPI_Request * pReq, const char * pBuf, int len );

int LSAPI_AppendRespHeader2_r( LSAPI_Request * pReq, const char * pHeaderName,
                              const char * pHeaderValue );

int LSAPI_ErrResponse_r( LSAPI_Request * pReq, int code, const char ** pRespHeaders,
                         const char * pBody, int bodyLen );

static inline int LSAPI_SetRespStatus_r( LSAPI_Request * pReq, int code )
{
    if ( !pReq )
        return -1;
    pReq->m_respHeader.m_respInfo.m_status = code;
    return 0;
}

static inline int LSAPI_SetAppData_r( LSAPI_Request * pReq, void * data )
{
    if ( !pReq )
        return -1;
    pReq->m_pAppData = data;
    return 0;
}

static inline void * LSAPI_GetAppData_r( LSAPI_Request * pReq )
{
    if ( !pReq )
        return NULL;
    return pReq->m_pAppData;
}

static inline char * LSAPI_GetQueryString_r( LSAPI_Request * pReq )
{
    if ( pReq )
        return pReq->m_pQueryString;
    return NULL;
}


static inline char * LSAPI_GetScriptFileName_r( LSAPI_Request * pReq )
{
    if ( pReq )
        return pReq->m_pScriptFile;
    return NULL;
}


static inline char * LSAPI_GetScriptName_r( LSAPI_Request * pReq )
{
    if ( pReq )
        return pReq->m_pScriptName;
    return NULL;
}


static inline char * LSAPI_GetRequestMethod_r( LSAPI_Request * pReq)
{
    if ( pReq )
        return pReq->m_pRequestMethod;
    return NULL;
}



static inline off_t LSAPI_GetReqBodyLen_r( LSAPI_Request * pReq )
{
    if ( pReq )
        return pReq->m_reqBodyLen;
    return -1;
}

static inline off_t LSAPI_GetReqBodyRemain_r( LSAPI_Request * pReq )
{
    if ( pReq )
        return pReq->m_reqBodyLen - pReq->m_reqBodyRead;
    return -1;
}


int LSAPI_End_Response_r(LSAPI_Request * pReq);



int LSAPI_Is_Listen(void);

static inline int LSAPI_Accept( void )
{   return LSAPI_Accept_r( &g_req );                        }

static inline int LSAPI_Finish(void)
{   return LSAPI_Finish_r( &g_req );                        }

static inline char * LSAPI_GetHeader( int headerIndex )
{   return LSAPI_GetHeader_r( &g_req, headerIndex );        }

static inline int LSAPI_ForeachHeader( LSAPI_CB_EnvHandler fn, void * arg )
{   return LSAPI_ForeachHeader_r( &g_req, fn, arg );        }

static inline int LSAPI_ForeachOrgHeader(
            LSAPI_CB_EnvHandler fn, void * arg )
{   return LSAPI_ForeachOrgHeader_r( &g_req, fn, arg );     }

static inline int LSAPI_ForeachEnv( LSAPI_CB_EnvHandler fn, void * arg )
{   return LSAPI_ForeachEnv_r( &g_req, fn, arg );           }

static inline int LSAPI_ForeachSpecialEnv( LSAPI_CB_EnvHandler fn, void * arg )
{   return LSAPI_ForeachSpecialEnv_r( &g_req, fn, arg );    }

static inline char * LSAPI_GetEnv( const char * name )
{   return LSAPI_GetEnv_r( &g_req, name );                  }

static inline char * LSAPI_GetQueryString(void)
{   return LSAPI_GetQueryString_r( &g_req );                }

static inline char * LSAPI_GetScriptFileName(void)
{   return LSAPI_GetScriptFileName_r( &g_req );             }

static inline char * LSAPI_GetScriptName(void)
{    return LSAPI_GetScriptName_r( &g_req );                }

static inline char * LSAPI_GetRequestMethod(void)
{   return LSAPI_GetRequestMethod_r( &g_req );              }

static inline off_t LSAPI_GetReqBodyLen(void)
{   return LSAPI_GetReqBodyLen_r( &g_req );                 }

static inline off_t LSAPI_GetReqBodyRemain(void)
{   return LSAPI_GetReqBodyRemain_r( &g_req );                 }

static inline ssize_t LSAPI_ReadReqBody( char * pBuf, size_t len )
{   return LSAPI_ReadReqBody_r( &g_req, pBuf, len );        }

static inline int LSAPI_ReqBodyGetChar(void)
{   return LSAPI_ReqBodyGetChar_r( &g_req );        }

static inline int LSAPI_ReqBodyGetLine( char * pBuf, int len, int *getLF )
{   return LSAPI_ReqBodyGetLine_r( &g_req, pBuf, len, getLF );        }



static inline int LSAPI_FinalizeRespHeaders(void)
{   return LSAPI_FinalizeRespHeaders_r( &g_req );           }

static inline ssize_t LSAPI_Write( const char * pBuf, ssize_t len )
{   return LSAPI_Write_r( &g_req, pBuf, len );              }

static inline ssize_t LSAPI_sendfile( int fdIn, off_t* off, size_t size )
{
    return LSAPI_sendfile_r(&g_req, fdIn, off, size );
}

static inline ssize_t LSAPI_Write_Stderr( const char * pBuf, ssize_t len )
{   return LSAPI_Write_Stderr_r( &g_req, pBuf, len );       }

static inline int LSAPI_Flush(void)
{   return LSAPI_Flush_r( &g_req );                         }

static inline int LSAPI_AppendRespHeader( char * pBuf, int len )
{   return LSAPI_AppendRespHeader_r( &g_req, pBuf, len );   }

static inline int LSAPI_SetRespStatus( int code )
{   return LSAPI_SetRespStatus_r( &g_req, code );           }

static inline int LSAPI_ErrResponse( int code, const char ** pRespHeaders, const char * pBody, int bodyLen )
{   return LSAPI_ErrResponse_r( &g_req, code, pRespHeaders, pBody, bodyLen );   }

static inline int LSAPI_End_Response(void)
{   return LSAPI_End_Response_r( &g_req );                         }

int LSAPI_IsRunning(void);

int LSAPI_CreateListenSock( const char * pBind, int backlog );

typedef int (*fn_select_t)( int, fd_set *, fd_set *, fd_set *, struct timeval * );

int LSAPI_Init_Prefork_Server( int max_children, fn_select_t fp, int avoidFork );

void LSAPI_Set_Server_fd( int fd );

int LSAPI_Prefork_Accept_r( LSAPI_Request * pReq );

void LSAPI_No_Check_ppid(void);

void LSAPI_Set_Max_Reqs( int reqs );

void LSAPI_Set_Max_Idle( int secs );

void LSAPI_Set_Max_Children( int maxChildren );

void LSAPI_Set_Max_Idle_Children( int maxIdleChld );

void LSAPI_Set_Server_Max_Idle_Secs( int serverMaxIdle );

void LSAPI_Set_Max_Process_Time( int secs );

int LSAPI_Init_Env_Parameters( fn_select_t fp );

void LSAPI_Set_Slow_Req_Msecs( int msecs );

int  LSAPI_Get_Slow_Req_Msecs(void);

int LSAPI_is_suEXEC_Daemon(void);

int LSAPI_Set_Restored_Parent_Pid(int pid);

typedef void (*LSAPI_On_Timer_pf)(int *forked_child_pid);
void LSAPI_Register_Pgrp_Timer_Callback(LSAPI_On_Timer_pf);

int LSAPI_Inc_Req_Processed(int cnt);

int LSAPI_Accept_Before_Fork(LSAPI_Request * pReq);

int LSAPI_Postfork_Child(LSAPI_Request * pReq);

int LSAPI_Postfork_Parent(LSAPI_Request * pReq);

#define LSAPI_LOG_LEVEL_BITS    0xff
#define LSAPI_LOG_FLAG_NONE     0
#define LSAPI_LOG_FLAG_DEBUG    1
#define LSAPI_LOG_FLAG_INFO     2
#define LSAPI_LOG_FLAG_NOTICE   3
#define LSAPI_LOG_FLAG_WARN     4
#define LSAPI_LOG_FLAG_ERROR    5
#define LSAPI_LOG_FLAG_CRIT     6
#define LSAPI_LOG_FLAG_FATAL    7

#define LSAPI_LOG_TIMESTAMP_BITS (0xff00)
#define LSAPI_LOG_TIMESTAMP_FULL (0x100)
#define LSAPI_LOG_TIMESTAMP_HMS  (0x200)
#define LSAPI_LOG_TIMESTAMP_STDERR  (0x400)

#define LSAPI_LOG_PID            (0x10000)

void LSAPI_Log(int flag, const char * fmt, ...)
#if __GNUC__
        __attribute__((format(printf, 2, 3)))
#endif
;


#if defined (c_plusplus) || defined (__cplusplus)
}
#endif


#endif
