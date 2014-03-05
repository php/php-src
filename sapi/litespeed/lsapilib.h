
/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
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



#ifndef  _LSAPILIB_H_
#define  _LSAPILIB_H_

#if defined (c_plusplus) || defined (__cplusplus)
extern "C" {
#endif

#include <stddef.h>
#include <lsapidef.h>

#include <sys/time.h>
#include <sys/types.h>

struct LSAPI_key_value_pair
{
    char * pKey;
    char * pValue;
    int    keyLen;
    int    valLen;
};


#define LSAPI_MAX_RESP_HEADERS  100

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
    int               m_reqBodyRead;
    int               m_bufProcessed;
    int               m_bufRead;
    
    struct lsapi_packet_header        m_respPktHeader[5];
    
    struct lsapi_resp_header          m_respHeader;
    short                             m_respHeaderLen[LSAPI_MAX_RESP_HEADERS];

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
            

int LSAPI_ReadReqBody_r( LSAPI_Request * pReq, char * pBuf, int len );

int LSAPI_ReqBodyGetChar_r( LSAPI_Request * pReq );

int LSAPI_ReqBodyGetLine_r( LSAPI_Request * pReq, char * pBuf, int bufLen, int *getLF );


int LSAPI_FinalizeRespHeaders_r( LSAPI_Request * pReq );

int LSAPI_Write_r( LSAPI_Request * pReq, const char * pBuf, int len );

int LSAPI_Write_Stderr_r( LSAPI_Request * pReq, const char * pBuf, int len );

int LSAPI_Flush_r( LSAPI_Request * pReq );

int LSAPI_AppendRespHeader_r( LSAPI_Request * pHeader, char * pBuf, int len );

static inline int LSAPI_SetRespStatus_r( LSAPI_Request * pReq, int code )
{
    if ( !pReq )
        return -1;
    pReq->m_respHeader.m_respInfo.m_status = code;
    return 0;
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



static inline int  LSAPI_GetReqBodyLen_r( LSAPI_Request * pReq )
{
    if ( pReq )
        return pReq->m_pHeader->m_reqBodyLen;
    return -1;
}

static inline int LSAPI_GetReqBodyRemain_r( LSAPI_Request * pReq )
{
    if ( pReq )
        return pReq->m_pHeader->m_reqBodyLen - pReq->m_reqBodyRead;
    return -1;
}


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

static inline char * LSAPI_GetQueryString()
{   return LSAPI_GetQueryString_r( &g_req );                }

static inline char * LSAPI_GetScriptFileName()
{   return LSAPI_GetScriptFileName_r( &g_req );             }

static inline char * LSAPI_GetScriptName()
{    return LSAPI_GetScriptName_r( &g_req );                }

static inline char * LSAPI_GetRequestMethod()
{   return LSAPI_GetRequestMethod_r( &g_req );              }

static inline int LSAPI_GetReqBodyLen()
{   return LSAPI_GetReqBodyLen_r( &g_req );                 }

static inline int LSAPI_GetReqBodyRemain()
{   return LSAPI_GetReqBodyRemain_r( &g_req );                 }

static inline int LSAPI_ReadReqBody( char * pBuf, int len )
{   return LSAPI_ReadReqBody_r( &g_req, pBuf, len );        }

static inline int LSAPI_ReqBodyGetChar()
{   return LSAPI_ReqBodyGetChar_r( &g_req );        }

static inline int LSAPI_ReqBodyGetLine( char * pBuf, int len, int *getLF )
{   return LSAPI_ReqBodyGetLine_r( &g_req, pBuf, len, getLF );        }



static inline int LSAPI_FinalizeRespHeaders(void)
{   return LSAPI_FinalizeRespHeaders_r( &g_req );           }

static inline int LSAPI_Write( const char * pBuf, int len )
{   return LSAPI_Write_r( &g_req, pBuf, len );              }

static inline int LSAPI_Write_Stderr( const char * pBuf, int len )
{   return LSAPI_Write_Stderr_r( &g_req, pBuf, len );       }

static inline int LSAPI_Flush()
{   return LSAPI_Flush_r( &g_req );                         }

static inline int LSAPI_AppendRespHeader( char * pBuf, int len )
{   return LSAPI_AppendRespHeader_r( &g_req, pBuf, len );   }

static inline int LSAPI_SetRespStatus( int code )
{   return LSAPI_SetRespStatus_r( &g_req, code );           }

int LSAPI_IsRunning(void);

int LSAPI_CreateListenSock( const char * pBind, int backlog );

typedef int (*fn_select_t)( int, fd_set *, fd_set *, fd_set *, struct timeval * );

int LSAPI_Init_Prefork_Server( int max_children, fn_select_t fp, int avoidFork );

void LSAPI_Set_Server_fd( int fd );

int LSAPI_Prefork_Accept_r( LSAPI_Request * pReq );

void LSAPI_Set_Max_Reqs( int reqs );

void LSAPI_Set_Max_Idle( int secs );

void LSAPI_Set_Max_Children( int maxChildren );

void LSAPI_Set_Max_Idle_Children( int maxIdleChld );

void LSAPI_Set_Server_Max_Idle_Secs( int serverMaxIdle );

void LSAPI_Set_Max_Process_Time( int secs );

void LSAPI_Init_Env_Parameters( fn_select_t fp );

#if defined (c_plusplus) || defined (__cplusplus)
}
#endif


#endif







