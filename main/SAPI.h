/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors:  Zeev Suraski <zeev@zend.com>                               |
   +----------------------------------------------------------------------+
*/


#ifndef SAPI_H
#define SAPI_H

#include "zend.h"
#include "zend_llist.h"
#include "zend_operators.h"
#include <sys/stat.h>

#define SAPI_POST_BLOCK_SIZE 4000

#ifdef PHP_WIN32
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
	zend_bool replace;
} sapi_header_struct;


typedef struct {
	zend_llist headers;
	int http_response_code;
	unsigned char send_default_content_type;
	char *http_status_line;
} sapi_headers_struct;


typedef struct _sapi_post_entry sapi_post_entry;
typedef struct _sapi_module_struct sapi_module_struct;


extern sapi_module_struct sapi_module;  /* true global */

/* Some values in this structure needs to be filled in before
 * calling sapi_activate(). We WILL change the `char *' entries,
 * so make sure that you allocate a separate buffer for them
 * and that you free them after sapi_deactivate().
 */

typedef struct {
	const char *request_method;
	char *query_string;
	char *post_data;
	char *cookie_data;
	long content_length;
	uint post_data_length;

	char *path_translated;
	char *request_uri;

	const char *content_type;

	zend_bool headers_only;
	zend_bool no_headers;

	sapi_post_entry *post_entry;

	char *content_type_dup;

	/* for HTTP authentication */
	char *auth_user;
	char *auth_password;

	/* this is necessary for the CGI SAPI module */
	char *argv0;

	/* this is necessary for Safe Mode */
	char *current_user;
	int current_user_length;
} sapi_request_info;


typedef struct {
	void *server_context;
	sapi_request_info request_info;
	sapi_headers_struct sapi_headers;
	int read_post_bytes;
	unsigned char headers_sent;
	struct stat global_stat;
	char *default_mimetype;
	char *default_charset;
	HashTable *rfc1867_uploaded_files;
	long post_max_size;
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
# define SLS_D	void
# define SLS_DC
# define SLS_C
# define SLS_CC
# define SG(v) (sapi_globals.v)
# define SLS_FETCH()
extern SAPI_API sapi_globals_struct sapi_globals;
#endif


SAPI_API void sapi_startup(sapi_module_struct *sf);
SAPI_API void sapi_shutdown(void);
SAPI_API void sapi_activate(SLS_D);
SAPI_API void sapi_deactivate(SLS_D);
SAPI_API void sapi_initialize_empty_request(SLS_D);

SAPI_API int sapi_add_header_ex(char *header_line, uint header_line_len, zend_bool duplicate, zend_bool replace);
#define sapi_add_header(header_line, header_line_len, duplicate) \
	sapi_add_header_ex((header_line), (header_line_len), (duplicate), 1)
SAPI_API int sapi_send_headers(void);
SAPI_API void sapi_free_header(sapi_header_struct *sapi_header);
SAPI_API void sapi_handle_post(void *arg SLS_DC);

SAPI_API int sapi_register_post_entries(sapi_post_entry *post_entry);
SAPI_API int sapi_register_post_entry(sapi_post_entry *post_entry);
SAPI_API int sapi_add_post_entry(char *content_type
								 , void (*post_reader)(SLS_D)
								 , void (*post_handler)(char *content_type_dup, void *arg SLS_DC));
SAPI_API void sapi_remove_post_entry(char *content_type);
SAPI_API void sapi_unregister_post_entry(sapi_post_entry *post_entry);
SAPI_API int sapi_register_default_post_reader(void (*default_post_reader)(SLS_D));

SAPI_API int sapi_flush(void);
SAPI_API struct stat *sapi_get_stat(void);
SAPI_API char *sapi_getenv(char *name, size_t name_len);

SAPI_API char *sapi_get_default_content_type(SLS_D);
SAPI_API void sapi_get_default_content_type_header(sapi_header_struct *default_header SLS_DC);
SAPI_API size_t sapi_apply_default_charset(char **mimetype, size_t len SLS_DC);

struct _sapi_module_struct {
	char *name;
	char *pretty_name;

	int (*startup)(struct _sapi_module_struct *sapi_module);
	int (*shutdown)(struct _sapi_module_struct *sapi_module);

	int (*activate)(SLS_D);
	int (*deactivate)(SLS_D);

	int (*ub_write)(const char *str, unsigned int str_length);
	void (*flush)(void *server_context);
	struct stat *(*get_stat)(SLS_D);
	char *(*getenv)(char *name, size_t name_len SLS_DC);

	void (*sapi_error)(int type, const char *error_msg, ...);

	int (*header_handler)(sapi_header_struct *sapi_header, sapi_headers_struct *sapi_headers SLS_DC);
	int (*send_headers)(sapi_headers_struct *sapi_headers SLS_DC);
	void (*send_header)(sapi_header_struct *sapi_header, void *server_context);

	int (*read_post)(char *buffer, uint count_bytes SLS_DC);
	char *(*read_cookies)(SLS_D);

	void (*register_server_variables)(zval *track_vars_array ELS_DC SLS_DC PLS_DC);
	void (*log_message)(char *message);

	void (*block_interruptions)(void);
	void (*unblock_interruptions)(void);

	void (*default_post_reader)(SLS_D);
};


struct _sapi_post_entry {
	char *content_type;
	uint content_type_len;
	void (*post_reader)(SLS_D);
	void (*post_handler)(char *content_type_dup, void *arg SLS_DC);
};

/* header_handler() constants */
#define SAPI_HEADER_ADD			(1<<0)
#define SAPI_HEADER_DELETE_ALL	(1<<1)
#define SAPI_HEADER_SEND_NOW	(1<<2)


#define SAPI_HEADER_SENT_SUCCESSFULLY	1
#define SAPI_HEADER_DO_SEND				2
#define SAPI_HEADER_SEND_FAILED			3

#define SAPI_DEFAULT_MIMETYPE		"text/html"
#define SAPI_DEFAULT_CHARSET		""
#define SAPI_PHP_VERSION_HEADER		"X-Powered-By: PHP/" PHP_VERSION

#define SAPI_POST_READER_FUNC(post_reader) SAPI_API void post_reader(SLS_D)
#define SAPI_POST_HANDLER_FUNC(post_handler) SAPI_API void post_handler(char *content_type_dup, void *arg SLS_DC)

SAPI_POST_READER_FUNC(sapi_read_standard_form_data);
SAPI_POST_READER_FUNC(php_default_post_reader);

#define STANDARD_SAPI_MODULE_PROPERTIES NULL

#endif /* SAPI_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
