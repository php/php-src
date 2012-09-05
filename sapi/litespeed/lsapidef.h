
/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
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


#ifndef  _LSAPIDEF_H_
#define  _LSAPIDEF_H_

#include <inttypes.h>

#if defined (c_plusplus) || defined (__cplusplus)
extern "C" {
#endif

enum
{
    H_ACCEPT = 0,
    H_ACC_CHARSET,
    H_ACC_ENCODING,
    H_ACC_LANG,
    H_AUTHORIZATION,
    H_CONNECTION,
    H_CONTENT_TYPE,
    H_CONTENT_LENGTH,
    H_COOKIE,
    H_COOKIE2,
    H_HOST,
    H_PRAGMA,
    H_REFERER,
    H_USERAGENT,
    H_CACHE_CTRL,
    H_IF_MODIFIED_SINCE,
    H_IF_MATCH,
    H_IF_NO_MATCH,
    H_IF_RANGE,
    H_IF_UNMOD_SINCE,
    H_KEEP_ALIVE,
    H_RANGE,
    H_X_FORWARDED_FOR,
    H_VIA,
    H_TRANSFER_ENCODING

};
#define LSAPI_SOCK_FILENO           0

#define LSAPI_VERSION_B0            'L'
#define LSAPI_VERSION_B1            'S'

/* Values for m_flag in lsapi_packet_header */
#define LSAPI_ENDIAN_LITTLE         0
#define LSAPI_ENDIAN_BIG            1 
#define LSAPI_ENDIAN_BIT            1

#if defined(__i386__)||defined( __x86_64 )||defined( __x86_64__ )
#define LSAPI_ENDIAN                LSAPI_ENDIAN_LITTLE
#else
#define LSAPI_ENDIAN                LSAPI_ENDIAN_BIG
#endif

/* Values for m_type in lsapi_packet_header */
#define LSAPI_BEGIN_REQUEST         1
#define LSAPI_ABORT_REQUEST         2
#define LSAPI_RESP_HEADER           3
#define LSAPI_RESP_STREAM           4
#define LSAPI_RESP_END              5
#define LSAPI_STDERR_STREAM         6
#define LSAPI_REQ_RECEIVED          7


#define LSAPI_MAX_HEADER_LEN        65535
#define LSAPI_MAX_DATA_PACKET_LEN   16384

#define LSAPI_RESP_HTTP_HEADER_MAX  4096
#define LSAPI_PACKET_HEADER_LEN     8


struct lsapi_packet_header
{
    char    m_versionB0;      /* LSAPI protocol version */
    char    m_versionB1;
    char    m_type;
    char    m_flag;
    union
    {
        int32_t m_iLen;       /* include this header */
        char    m_bytes[4];
    }m_packetLen;
};

/*
    LSAPI request header packet
    
    1. struct lsapi_req_header
    2. struct lsapi_http_header_index
    3. lsapi_header_offset * unknownHeaders
    4. org http request header
    5. request body if available
*/

struct lsapi_req_header
{
    struct lsapi_packet_header m_pktHeader;
        
    int32_t m_httpHeaderLen;
    int32_t m_reqBodyLen;
    int32_t m_scriptFileOff;   /* path to the script file. */
    int32_t m_scriptNameOff;   /* decrypted URI, without pathinfo, */
    int32_t m_queryStringOff;  /* Query string inside env */
    int32_t m_requestMethodOff;
    int32_t m_cntUnknownHeaders;
    int32_t m_cntEnv;
    int32_t m_cntSpecialEnv;
} ;


struct lsapi_http_header_index
{        
    int16_t m_headerLen[H_TRANSFER_ENCODING+1];
    int32_t m_headerOff[H_TRANSFER_ENCODING+1];
} ;  
 
struct lsapi_header_offset
{
    int32_t nameOff;
    int32_t nameLen;
    int32_t valueOff;
    int32_t valueLen;
} ;

struct lsapi_resp_info
{
    int32_t m_cntHeaders;
    int32_t m_status;
};

struct lsapi_resp_header
{
    struct  lsapi_packet_header  m_pktHeader;
    struct  lsapi_resp_info      m_respInfo;
};

#if defined (c_plusplus) || defined (__cplusplus)
}
#endif


#endif

