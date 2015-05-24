/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
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

/* this is near the perfect hash function for most useful FastCGI variables
 * which combines efficiency and minimal hash collisions
 */

#define FCGI_HASH_FUNC(var, var_len) \
	(UNEXPECTED(var_len < 3) ? var_len : \
		(((unsigned int)var[3]) << 2) + \
		(((unsigned int)var[var_len-2]) << 4) + \
		(((unsigned int)var[var_len-1]) << 2) + \
		var_len)

#define FCGI_GETENV(request, name) \
	fcgi_quick_getenv(request, name, sizeof(name)-1, FCGI_HASH_FUNC(name, sizeof(name)-1))

#define FCGI_PUTENV(request, name, value) \
	fcgi_quick_putenv(request, name, sizeof(name)-1, FCGI_HASH_FUNC(name, sizeof(name)-1), value)

#ifdef PHP_WIN32
# ifdef FCGI_EXPORTS
#  define FCGI_API __declspec(dllexport)
# else
#  define FCGI_API __declspec(dllimport)
# endif
#else
# define FCGI_API
#endif

typedef enum _fcgi_role {
	FCGI_RESPONDER	= 1,
	FCGI_AUTHORIZER	= 2,
	FCGI_FILTER		= 3
} fcgi_role;

enum {
	FCGI_DEBUG		= 1,
	FCGI_NOTICE		= 2,
	FCGI_WARNING	= 3,
	FCGI_ERROR		= 4,
	FCGI_ALERT		= 5,
};

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

typedef void (*fcgi_apply_func)(char *var, unsigned int var_len, char *val, unsigned int val_len, void *arg);

#define FCGI_HASH_TABLE_SIZE 128
#define FCGI_HASH_TABLE_MASK (FCGI_HASH_TABLE_SIZE - 1)
#define FCGI_HASH_SEG_SIZE   4096

typedef struct _fcgi_hash_bucket {
	unsigned int              hash_value;
	unsigned int              var_len;
	char                     *var;
	unsigned int              val_len;
	char                     *val;
	struct _fcgi_hash_bucket *next;
	struct _fcgi_hash_bucket *list_next;
} fcgi_hash_bucket;

typedef struct _fcgi_hash_buckets {
	unsigned int	           idx;
	struct _fcgi_hash_buckets *next;
	struct _fcgi_hash_bucket   data[FCGI_HASH_TABLE_SIZE];
} fcgi_hash_buckets;

typedef struct _fcgi_data_seg {
	char                  *pos;
	char                  *end;
	struct _fcgi_data_seg *next;
	char                   data[1];
} fcgi_data_seg;

typedef struct _fcgi_hash {
	fcgi_hash_bucket  *hash_table[FCGI_HASH_TABLE_SIZE];
	fcgi_hash_bucket  *list;
	fcgi_hash_buckets *buckets;
	fcgi_data_seg     *data;
} fcgi_hash;

typedef struct _fcgi_request 	fcgi_request;
typedef struct _fcgi_req_hook 	fcgi_req_hook;

struct _fcgi_req_hook {
	void(*on_accept)();
	void(*on_read)();
	void(*on_close)();
};

struct _fcgi_request {
	int            listen_socket;
	int            tcp;
	int            fd;
	int            id;
	int            keep;
#ifdef TCP_NODELAY
	int            nodelay;
#endif
	int            closed;

	fcgi_req_hook  hook;

	int            in_len;
	int            in_pad;

	fcgi_header   *out_hdr;
	unsigned char *out_pos;
	unsigned char  out_buf[1024*8];
	unsigned char  reserved[sizeof(fcgi_end_request_rec)];

	int            has_env;
	fcgi_hash      env;
};

FCGI_API int fcgi_init(void);
FCGI_API void fcgi_shutdown(void);
FCGI_API int fcgi_is_fastcgi(void);
FCGI_API int fcgi_is_closed(fcgi_request *req);
FCGI_API void fcgi_close(fcgi_request *req, int force, int destroy);
FCGI_API int fcgi_in_shutdown(void);
FCGI_API void fcgi_terminate(void);
FCGI_API int fcgi_listen(const char *path, int backlog);
FCGI_API fcgi_request* fcgi_init_request(fcgi_request *request, int listen_socket);
FCGI_API void fcgi_destroy_request(fcgi_request *req);
FCGI_API void fcgi_set_allowed_clients(char *ip);
FCGI_API int fcgi_accept_request(fcgi_request *req);
FCGI_API int fcgi_finish_request(fcgi_request *req, int force_close);
FCGI_API const char *fcgi_get_last_client_ip();
FCGI_API void fcgi_set_in_shutdown(int new_value);

#ifndef HAVE_ATTRIBUTE_WEAK
typedef void (*fcgi_logger)(int type, const char *fmt, ...);
FCGI_API void fcgi_set_logger(fcgi_logger lg);
#endif

FCGI_API char* fcgi_getenv(fcgi_request *req, const char* var, int var_len);
FCGI_API char* fcgi_putenv(fcgi_request *req, char* var, int var_len, char* val);
FCGI_API char* fcgi_quick_getenv(fcgi_request *req, const char* var, int var_len, unsigned int hash_value);
FCGI_API char* fcgi_quick_putenv(fcgi_request *req, char* var, int var_len, unsigned int hash_value, char* val);
FCGI_API void  fcgi_loadenv(fcgi_request *req, fcgi_apply_func load_func, zval *array);

FCGI_API int fcgi_read(fcgi_request *req, char *str, int len);

FCGI_API int fcgi_write(fcgi_request *req, fcgi_request_type type, const char *str, int len);
FCGI_API int fcgi_flush(fcgi_request *req, int close);

#ifdef PHP_WIN32
FCGI_API void fcgi_impersonate(void);
#endif

FCGI_API void fcgi_set_mgmt_var(const char * name, size_t name_len, const char * value, size_t value_len);
FCGI_API void fcgi_free_mgmt_var_cb(zval *zv);

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
