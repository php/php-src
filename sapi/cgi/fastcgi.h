/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2006 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/* FastCGI protocol */

#define FCGI_VERSION_1 1

#define FCGI_MAX_LENGTH 0xffff

#define FCGI_KEEP_CONN  1

typedef enum _fcgi_role {
	FCGI_RESPONDER	= 1,
	FCGI_AUTHORIZER	= 2,
	FCGI_FILTER		= 3
} fcgi_role;

typedef enum _fcgi_request_type {
	FCGI_BEGIN_REQUEST		=  1, /* [in]                              */
	FCGI_ABORT_REQUEST		=  2, /* [in]  (not supported)             */
	FCGI_END_REQUEST		=  3, /* [out]                             */
	FCGI_PARAMS				=  4, /* [in]  environment variables       */
	FCGI_STDIN				=  5, /* [in]  post data                   */
	FCGI_STDOUT				=  6, /* [out] response                    */
	FCGI_STDERR				=  7, /* [out] errors                      */
	FCGI_DATA				=  8, /* [in]  filter data (not supported) */
	FCGI_GET_VALUES			=  9, /* [in]                              */
	FCGI_GET_VALUES_RESULT	= 10  /* [out]                             */
} fcgi_request_type;

typedef enum _fcgi_protocol_status {
	FCGI_REQUEST_COMPLETE	= 0,
	FCGI_CANT_MPX_CONN		= 1,
	FCGI_OVERLOADED			= 2,
	FCGI_UNKNOWN_ROLE		= 3
} dcgi_protocol_status;

typedef struct _fcgi_header {
	unsigned char version;
	unsigned char type;
	unsigned char requestIdB1;
	unsigned char requestIdB0;
	unsigned char contentLengthB1;
	unsigned char contentLengthB0;
	unsigned char paddingLength;
	unsigned char reserved;
} fcgi_header;

typedef struct _fcgi_begin_request {
	unsigned char roleB1;
	unsigned char roleB0;
	unsigned char flags;
	unsigned char reserved[5];
} fcgi_begin_request;

typedef struct _fcgi_begin_request_rec {
	fcgi_header hdr;
	fcgi_begin_request body;
} fcgi_begin_request_rec;

typedef struct _fcgi_end_request {
    unsigned char appStatusB3;
    unsigned char appStatusB2;
    unsigned char appStatusB1;
    unsigned char appStatusB0;
    unsigned char protocolStatus;
    unsigned char reserved[3];
} fcgi_end_request;

typedef struct _fcgi_end_request_rec {
	fcgi_header hdr;
	fcgi_end_request body;
} fcgi_end_request_rec;

/* FastCGI client API */

typedef struct _fcgi_request {
	int            listen_socket;
	int            fd;
	int            id;
	int            keep;

	int            in_len;
	int            in_pad;

	fcgi_header   *out_hdr;
	unsigned char *out_pos;
	unsigned char  out_buf[1024*8];
	unsigned char  reserved[sizeof(fcgi_end_request_rec)];

	char          *env[128];
} fcgi_request;

int fcgi_init(void);
int fcgi_is_fastcgi(void);
int fcgi_listen(const char *path, int backlog);
void fcgi_init_request(fcgi_request *req, int listen_socket);
int fcgi_accept_request(fcgi_request *req);
int fcgi_finish_request(fcgi_request *req);

char* fcgi_getenv(fcgi_request *req, const char* var, int var_len);
void fcgi_putenv(fcgi_request *req, char* var, int var_len);

int fcgi_read(fcgi_request *req, char *str, int len);

int fcgi_write(fcgi_request *req, fcgi_request_type type, const char *str, int len);
int fcgi_flush(fcgi_request *req, int close);

/* Some defines for limited libfcgi comatibility */

typedef struct _FCGX_Stream {
	fcgi_request      *req;
	fcgi_request_type  type;
} FCGX_Stream;

typedef struct _FCGX_Request {
	fcgi_request req;
	FCGX_Stream  in;
	FCGX_Stream  out;
	FCGX_Stream  err;
	char **envp;
} FCGX_Request;

#define FCGX_Init()
#define FCGX_IsCGI()						(!fcgi_is_fastcgi())
#define FCGX_OpenSocket(path, backlog)		fcgi_listen(path, backlog)

#define FCGX_InitRequest(r, sock, flags)	\
	do {									\
		fcgi_init_request(&(r)->req, sock);	\
		(r)->in.req = &(r)->req;			\
		(r)->out.req = &(r)->req;			\
		(r)->err.req = &(r)->req;			\
		(r)->in.type = FCGI_STDIN;			\
		(r)->out.type = FCGI_STDOUT;		\
		(r)->err.type = FCGI_STDERR;		\
		(r)->envp = (r)->req.env;			\
	} while (0);


#define FCGX_Accept_r(r)					fcgi_accept_request(&(r)->req)
#define FCGX_Finish_r(r)					fcgi_finish_request(&(r)->req)

#define FCGX_PutStr(str, len, stream)		fcgi_write((stream).req, (stream).type, str, len)
#define FCGX_PutS(str, len, stream)			fcgi_write((stream).req, (stream).type, str, len)
#define FCGX_FFlush(stream)                 fcgi_flush((stream).req, 0)
#define FCGX_GetStr(str, len, stream)       fcgi_read((stream).req, str, len)

#define FCGX_GetParam(var, envp)			fcgi_getenv_helper(envp, var, strlen(var));

#define FCGX_PutEnv(r, var)					fcgi_putenv(&(r)->req, var, strlen(var));

int FCGX_FPrintF(FCGX_Stream stream, const char *format, ...);

/* Internal helper functions. They shouldn't be used directly. */

char* fcgi_getenv_helper(char** env, const char *name, int len);

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
