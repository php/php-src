#ifndef _NEW_SAPI_H
#define _NEW_SAPI_H

#include "zend.h"
#include "zend_llist.h"
#include "zend_operators.h"

#define SAPI_POST_BLOCK_SIZE 4000

#if WIN32||WINNT
#	ifdef SAPI_EXPORTS
#	define SAPI_API __declspec(dllexport) 
#	else
#	define SAPI_API __declspec(dllimport) 
#	endif
#else
#define SAPI_API
#endif


typedef struct {
	char *header;
	uint header_len;
} sapi_header_struct;


typedef struct {
	zend_llist headers;
	int http_response_code;
	unsigned char send_default_content_type;
	char *http_status_line;
} sapi_headers_struct;


typedef struct _sapi_module_struct sapi_module_struct;


extern sapi_module_struct sapi_module;  /* true global */


typedef struct {
	char *request_method;
	char *query_string;
	char *post_data;
	char *cookie_data;
	uint content_length;

	char *path_translated;
	char *request_uri;

	char *content_type;

	unsigned char headers_only;

	/* for HTTP authentication */
	char *auth_user;
	char *auth_password;
} sapi_request_info;


typedef struct {
	void *server_context;
	sapi_request_info request_info;
	sapi_headers_struct sapi_headers;
	uint read_post_bytes;
	unsigned char headers_sent;
} sapi_globals_struct;


#ifdef ZTS
# define SLS_D	sapi_globals_struct *sapi_globals
# define SLS_DC	, SLS_D
# define SLS_C	sapi_globals
# define SLS_CC , SLS_C
# define SG(v) (sapi_globals->v)
# define SLS_FETCH()	sapi_globals_struct *sapi_globals = ts_resource(sapi_globals_id)
SAPI_API extern int sapi_globals_id;
#else
# define SLS_D
# define SLS_DC
# define SLS_C
# define SLS_CC
# define SG(v) (sapi_globals.v)
# define SLS_FETCH()
extern SAPI_API sapi_globals_struct sapi_globals;
#endif

typedef struct _sapi_post_content_type_reader {
	char *content_type;
	uint content_type_len;
	void (*post_reader)(char *content_type_dup SLS_DC);
} sapi_post_content_type_reader;


SAPI_API void sapi_startup(sapi_module_struct *sf);
SAPI_API void sapi_activate(SLS_D);
SAPI_API void sapi_deactivate(SLS_D);

SAPI_API int sapi_add_header(char *header_line, uint header_line_len);
SAPI_API int sapi_send_headers();

SAPI_API int sapi_register_post_reader(sapi_post_content_type_reader *post_content_type_reader);
SAPI_API void sapi_unregister_post_reader(sapi_post_content_type_reader *post_content_type_reader);

struct _sapi_module_struct {
	char *name;

	int (*startup)(struct _sapi_module_struct *sapi_module);
	int (*shutdown)(struct _sapi_module_struct *sapi_module);

	int (*ub_write)(const char *str, unsigned int str_length);

	void (*sapi_error)(int type, const char *error_msg, ...);

	int (*header_handler)(sapi_header_struct *sapi_header, sapi_headers_struct *sapi_headers SLS_DC);
	int (*send_headers)(sapi_headers_struct *sapi_headers SLS_DC);
	void (*send_header)(sapi_header_struct *sapi_header, void *server_context);

	int (*read_post)(char *buffer, uint count_bytes SLS_DC);
	char *(*read_cookies)(SLS_D);
};


/* header_handler() constants */
#define SAPI_HEADER_ADD			(1<<0)
#define SAPI_HEADER_DELETE_ALL	(1<<1)
#define SAPI_HEADER_SEND_NOW	(1<<2)


#define SAPI_HEADER_SENT_SUCCESSFULLY	1
#define SAPI_HEADER_DO_SEND				2
#define SAPI_HEADER_SEND_FAILED			3

#define SAPI_DEFAULT_CONTENT_TYPE "Content-Type: text/html"

#define SAPI_POST_READER_FUNC(post_reader) void post_reader(char *content_type_dup SLS_DC)

SAPI_POST_READER_FUNC(sapi_read_standard_form_data);

#endif /* _NEW_SAPI_H */