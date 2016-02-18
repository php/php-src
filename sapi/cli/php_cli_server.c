/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Moriyoshi Koizumi <moriyoshi@php.net>                        |
   |         Xinchen Hui       <laruence@php.net>                         |
   +----------------------------------------------------------------------+
*/

/* $Id: php_cli.c 306938 2011-01-01 02:17:06Z felipe $ */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>

#ifdef PHP_WIN32
# include <process.h>
# include <io.h>
# include "win32/time.h"
# include "win32/signal.h"
# include "win32/php_registry.h"
# include <sys/timeb.h>
#else
# include "php_config.h"
#endif

#ifdef __riscos__
#include <unixlib/local.h>
#endif


#if HAVE_TIME_H
#include <time.h>
#endif
#if HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_SIGNAL_H
#include <signal.h>
#endif
#if HAVE_SETLOCALE
#include <locale.h>
#endif
#if HAVE_DLFCN_H
#include <dlfcn.h>
#endif

#include "SAPI.h"
#include "php.h"
#include "php_ini.h"
#include "php_main.h"
#include "php_globals.h"
#include "php_variables.h"
#include "zend_hash.h"
#include "zend_modules.h"
#include "fopen_wrappers.h"

#include "zend_compile.h"
#include "zend_execute.h"
#include "zend_highlight.h"
#include "zend_indent.h"
#include "zend_exceptions.h"

#include "php_getopt.h"

#ifndef PHP_WIN32
# define php_select(m, r, w, e, t)	select(m, r, w, e, t)
# define SOCK_EINVAL EINVAL
# define SOCK_EAGAIN EAGAIN
# define SOCK_EINTR EINTR
# define SOCK_EADDRINUSE EADDRINUSE
#else
# include "win32/select.h"
# define SOCK_EINVAL WSAEINVAL
# define SOCK_EAGAIN WSAEWOULDBLOCK
# define SOCK_EINTR WSAEINTR
# define SOCK_EADDRINUSE WSAEADDRINUSE
#endif

#ifndef S_ISDIR
#define S_ISDIR(mode)	(((mode)&S_IFMT) == S_IFDIR)
#endif

#include "ext/standard/file.h" /* for php_set_sock_blocking() :-( */
#include "ext/standard/php_smart_str.h"
#include "ext/standard/html.h"
#include "ext/standard/url.h" /* for php_raw_url_decode() */
#include "ext/standard/php_string.h" /* for php_dirname() */
#include "php_network.h"

#include "php_http_parser.h"
#include "php_cli_server.h"

#include "php_cli_process_title.h"

#define OUTPUT_NOT_CHECKED -1
#define OUTPUT_IS_TTY 1
#define OUTPUT_NOT_TTY 0

typedef struct php_cli_server_poller {
	fd_set rfds, wfds;
	struct {
		fd_set rfds, wfds;
	} active;
	php_socket_t max_fd;
} php_cli_server_poller;

typedef struct php_cli_server_request {
	enum php_http_method request_method;
	int protocol_version;
	char *request_uri;
	size_t request_uri_len;
	char *vpath;
	size_t vpath_len;
	char *path_translated;
	size_t path_translated_len;
	char *path_info;
	size_t path_info_len;
	char *query_string;
	size_t query_string_len;
	HashTable headers;
	HashTable headers_original_case;
	char *content;
	size_t content_len;
	const char *ext;
	size_t ext_len;
	struct stat sb;
} php_cli_server_request;

typedef struct php_cli_server_chunk {
	struct php_cli_server_chunk *next;
	enum php_cli_server_chunk_type {
		PHP_CLI_SERVER_CHUNK_HEAP,
		PHP_CLI_SERVER_CHUNK_IMMORTAL
	} type;
	union {
		struct { void *block; char *p; size_t len; } heap;
		struct { const char *p; size_t len; } immortal;
	} data;
} php_cli_server_chunk;

typedef struct php_cli_server_buffer {
	php_cli_server_chunk *first;
	php_cli_server_chunk *last;
} php_cli_server_buffer;

typedef struct php_cli_server_content_sender {
	php_cli_server_buffer buffer;
} php_cli_server_content_sender;

typedef struct php_cli_server_client {
	struct php_cli_server *server;
	php_socket_t sock;
	struct sockaddr *addr;
	socklen_t addr_len;
	char *addr_str;
	size_t addr_str_len;
	php_http_parser parser;
	unsigned int request_read:1;
	char *current_header_name;
	size_t current_header_name_len;
	unsigned int current_header_name_allocated:1;
	size_t post_read_offset;
	php_cli_server_request request;
	unsigned int content_sender_initialized:1;
	php_cli_server_content_sender content_sender;
	int file_fd;
} php_cli_server_client;

typedef struct php_cli_server {
	php_socket_t server_sock;
	php_cli_server_poller poller;
	int is_running;
	char *host;
	int port;
	int address_family;
	char *document_root;
	size_t document_root_len;
	char *router;
	size_t router_len;
	socklen_t socklen;
	HashTable clients;
} php_cli_server;

typedef struct php_cli_server_http_response_status_code_pair {
	int code;
	const char *str;
} php_cli_server_http_response_status_code_pair;

typedef struct php_cli_server_ext_mime_type_pair {
	const char *ext;
	const char *mime_type;
} php_cli_server_ext_mime_type_pair;

static php_cli_server_http_response_status_code_pair status_map[] = {
	{ 100, "Continue" },
	{ 101, "Switching Protocols" },
	{ 200, "OK" },
	{ 201, "Created" },
	{ 202, "Accepted" },
	{ 203, "Non-Authoritative Information" },
	{ 204, "No Content" },
	{ 205, "Reset Content" },
	{ 206, "Partial Content" },
	{ 300, "Multiple Choices" },
	{ 301, "Moved Permanently" },
	{ 302, "Found" },
	{ 303, "See Other" },
	{ 304, "Not Modified" },
	{ 305, "Use Proxy" },
	{ 307, "Temporary Redirect" },
	{ 308, "Permanent Redirect" },
	{ 400, "Bad Request" },
	{ 401, "Unauthorized" },
	{ 402, "Payment Required" },
	{ 403, "Forbidden" },
	{ 404, "Not Found" },
	{ 405, "Method Not Allowed" },
	{ 406, "Not Acceptable" },
	{ 407, "Proxy Authentication Required" },
	{ 408, "Request Timeout" },
	{ 409, "Conflict" },
	{ 410, "Gone" },
	{ 411, "Length Required" },
	{ 412, "Precondition Failed" },
	{ 413, "Request Entity Too Large" },
	{ 414, "Request-URI Too Long" },
	{ 415, "Unsupported Media Type" },
	{ 416, "Requested Range Not Satisfiable" },
	{ 417, "Expectation Failed" },
	{ 426, "Upgrade Required" },
	{ 428, "Precondition Required" },
	{ 429, "Too Many Requests" },
	{ 431, "Request Header Fields Too Large" },
	{ 451, "Unavailable For Legal Reasons"},
	{ 500, "Internal Server Error" },
	{ 501, "Not Implemented" },
	{ 502, "Bad Gateway" },
	{ 503, "Service Unavailable" },
	{ 504, "Gateway Timeout" },
	{ 505, "HTTP Version Not Supported" },
	{ 511, "Network Authentication Required" },
};

static php_cli_server_http_response_status_code_pair template_map[] = {
	{ 400, "<h1>%s</h1><p>Your browser sent a request that this server could not understand.</p>" },
	{ 404, "<h1>%s</h1><p>The requested resource <code class=\"url\">%s</code> was not found on this server.</p>" },
	{ 500, "<h1>%s</h1><p>The server is temporarily unavailable.</p>" },
	{ 501, "<h1>%s</h1><p>Request method not supported.</p>" }
};

static php_cli_server_ext_mime_type_pair mime_type_map[] = {
	{ "html", "text/html" },
	{ "htm", "text/html" },
	{ "js", "text/javascript" },
	{ "css", "text/css" },
	{ "gif", "image/gif" },
	{ "jpg", "image/jpeg" },
	{ "jpeg", "image/jpeg" },
	{ "jpe", "image/jpeg" },
	{ "pdf", "application/pdf" },
	{ "png", "image/png" },
	{ "svg", "image/svg+xml" },
	{ "txt", "text/plain" },
	{ "webm", "video/webm" },
	{ "ogv", "video/ogg" },
	{ "ogg", "audio/ogg" },
	{ "3gp", "video/3gpp" },    /* This is standard video format used for MMS in phones */
	{ "apk", "application/vnd.android.package-archive" },
	{ "avi", "video/x-msvideo" },
	{ "bmp", "image/x-ms-bmp" },
	{ "csv", "text/comma-separated-values" },
	{ "doc", "application/msword" },
	{ "docx", "application/msword" },
	{ "flac", "audio/flac" },
	{ "gz",  "application/x-gzip" },
	{ "gzip", "application/x-gzip" },
	{ "ics", "text/calendar" },
	{ "kml", "application/vnd.google-earth.kml+xml" },
	{ "kmz", "application/vnd.google-earth.kmz" },
	{ "m4a", "audio/mp4" },
	{ "mp3", "audio/mpeg" },
	{ "mp4", "video/mp4" },
	{ "mpg", "video/mpeg" },
	{ "mpeg", "video/mpeg" },
	{ "mov", "video/quicktime" },
	{ "odp", "application/vnd.oasis.opendocument.presentation" },
	{ "ods", "application/vnd.oasis.opendocument.spreadsheet" },
	{ "odt", "application/vnd.oasis.opendocument.text" },
	{ "oga", "audio/ogg" },
	{ "pdf", "application/pdf" },
	{ "pptx", "application/vnd.ms-powerpoint" },
	{ "pps", "application/vnd.ms-powerpoint" },
	{ "qt", "video/quicktime" },
	{ "swf", "application/x-shockwave-flash" },
	{ "tar", "application/x-tar" },
	{ "text", "text/plain" },
	{ "tif", "image/tiff" },
	{ "wav", "audio/wav" },
	{ "wmv", "video/x-ms-wmv" },
	{ "xls", "application/vnd.ms-excel" },
	{ "xlsx", "application/vnd.ms-excel" },
	{ "zip", "application/x-zip-compressed" },
	{ "xml", "application/xml" },
	{ "xsl", "application/xml" },
	{ "xsd", "application/xml" },
	{ NULL, NULL }
};

static int php_cli_output_is_tty = OUTPUT_NOT_CHECKED;

static size_t php_cli_server_client_send_through(php_cli_server_client *client, const char *str, size_t str_len);
static php_cli_server_chunk *php_cli_server_chunk_heap_new_self_contained(size_t len);
static void php_cli_server_buffer_append(php_cli_server_buffer *buffer, php_cli_server_chunk *chunk);
static void php_cli_server_logf(const char *format TSRMLS_DC, ...);
static void php_cli_server_log_response(php_cli_server_client *client, int status, const char *message TSRMLS_DC);

ZEND_DECLARE_MODULE_GLOBALS(cli_server);

/* {{{ static char php_cli_server_css[]
 * copied from ext/standard/info.c
 */
static const char php_cli_server_css[] = "<style>\n" \
										"body { background-color: #fcfcfc; color: #333333; margin: 0; padding:0; }\n" \
										"h1 { font-size: 1.5em; font-weight: normal; background-color: #9999cc; min-height:2em; line-height:2em; border-bottom: 1px inset black; margin: 0; }\n" \
										"h1, p { padding-left: 10px; }\n" \
										"code.url { background-color: #eeeeee; font-family:monospace; padding:0 2px;}\n" \
										"</style>\n";
/* }}} */

#ifdef PHP_WIN32
int php_cli_server_get_system_time(char *buf) {
	struct _timeb system_time;
	errno_t err;

	if (buf == NULL) {
		return -1;
	}

	_ftime(&system_time);
	err = ctime_s(buf, 52, &(system_time.time) );
	if (err) {
		return -1;
	}
	return 0;
}
#else
int php_cli_server_get_system_time(char *buf) {
	struct timeval tv;
	struct tm tm;

	gettimeofday(&tv, NULL);

	/* TODO: should be checked for NULL tm/return vaue */
	php_localtime_r(&tv.tv_sec, &tm);
	php_asctime_r(&tm, buf);
	return 0;
}
#endif

static void char_ptr_dtor_p(char **p) /* {{{ */
{
	pefree(*p, 1);
} /* }}} */

static char *get_last_error() /* {{{ */
{
	return pestrdup(strerror(errno), 1);
} /* }}} */

static int status_comp(const void *a, const void *b) /* {{{ */
{
	const php_cli_server_http_response_status_code_pair *pa = (const php_cli_server_http_response_status_code_pair *) a;
	const php_cli_server_http_response_status_code_pair *pb = (const php_cli_server_http_response_status_code_pair *) b;

	if (pa->code < pb->code) {
		return -1;
	} else if (pa->code > pb->code) {
		return 1;
	}

	return 0;
} /* }}} */

static const char *get_status_string(int code) /* {{{ */
{
	php_cli_server_http_response_status_code_pair needle, *result = NULL;

	needle.code = code;
	needle.str = NULL;

	result = bsearch(&needle, status_map, sizeof(status_map) / sizeof(needle), sizeof(needle), status_comp);

	if (result) {
		return result->str;
	}

	/* Returning NULL would require complicating append_http_status_line() to
	 * not segfault in that case, so let's just return a placeholder, since RFC
	 * 2616 requires a reason phrase. This is basically what a lot of other Web
	 * servers do in this case anyway. */
	return "Unknown Status Code";
} /* }}} */

static const char *get_template_string(int code) /* {{{ */
{
	size_t e = (sizeof(template_map) / sizeof(php_cli_server_http_response_status_code_pair));
	size_t s = 0;

	while (e != s) {
		size_t c = MIN((e + s + 1) / 2, e - 1);
		int d = template_map[c].code;
		if (d > code) {
			e = c;
		} else if (d < code) {
			s = c;
		} else {
			return template_map[c].str;
		}
	}
	return NULL;
} /* }}} */

static void append_http_status_line(smart_str *buffer, int protocol_version, int response_code, int persistent) /* {{{ */
{
	if (!response_code) {
		response_code = 200;
	}
	smart_str_appendl_ex(buffer, "HTTP", 4, persistent);
	smart_str_appendc_ex(buffer, '/', persistent);
	smart_str_append_generic_ex(buffer, protocol_version / 100, persistent, int, _unsigned);
	smart_str_appendc_ex(buffer, '.', persistent);
	smart_str_append_generic_ex(buffer, protocol_version % 100, persistent, int, _unsigned);
	smart_str_appendc_ex(buffer, ' ', persistent);
	smart_str_append_generic_ex(buffer, response_code, persistent, int, _unsigned);
	smart_str_appendc_ex(buffer, ' ', persistent);
	smart_str_appends_ex(buffer, get_status_string(response_code), persistent);
	smart_str_appendl_ex(buffer, "\r\n", 2, persistent);
} /* }}} */

static void append_essential_headers(smart_str* buffer, php_cli_server_client *client, int persistent) /* {{{ */
{
	{
		char **val;
		if (SUCCESS == zend_hash_find(&client->request.headers, "host", sizeof("host"), (void**)&val)) {
			smart_str_appendl_ex(buffer, "Host", sizeof("Host") - 1, persistent);
			smart_str_appendl_ex(buffer, ": ", sizeof(": ") - 1, persistent);
			smart_str_appends_ex(buffer, *val, persistent);
			smart_str_appendl_ex(buffer, "\r\n", 2, persistent);
		}
	}
	smart_str_appendl_ex(buffer, "Connection: close\r\n", sizeof("Connection: close\r\n") - 1, persistent);
} /* }}} */

static const char *get_mime_type(const char *ext, size_t ext_len) /* {{{ */
{
	php_cli_server_ext_mime_type_pair *pair;
	for (pair = mime_type_map; pair->ext; pair++) {
		size_t len = strlen(pair->ext);
		if (len == ext_len && memcmp(pair->ext, ext, len) == 0) {
			return pair->mime_type;
		}
	}
	return NULL;
} /* }}} */

PHP_FUNCTION(apache_request_headers) /* {{{ */
{
	php_cli_server_client *client;
	HashTable *headers;
	char *key;
	uint key_len;
	char **value_pointer;
	HashPosition pos;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	client = SG(server_context);
	headers = &client->request.headers_original_case;

	array_init_size(return_value, zend_hash_num_elements(headers));

	zend_hash_internal_pointer_reset_ex(headers, &pos);
	while (zend_hash_get_current_data_ex(headers, (void **)&value_pointer, &pos) == SUCCESS) {
		zend_hash_get_current_key_ex(headers, &key, &key_len, NULL, 0, &pos);
		add_assoc_string_ex(return_value, key, key_len, *value_pointer, 1);
		zend_hash_move_forward_ex(headers, &pos);
	}
}
/* }}} */

static void add_response_header(sapi_header_struct *h, zval *return_value TSRMLS_DC) /* {{{ */
{
	char *s, *p;
	int  len;
	ALLOCA_FLAG(use_heap)

	if (h->header_len > 0) {
		p = strchr(h->header, ':');
		len = p - h->header;
		if (p && (len > 0)) {
			while (len > 0 && (h->header[len-1] == ' ' || h->header[len-1] == '\t')) {
				len--;
			}
			if (len) {
				s = do_alloca(len + 1, use_heap);
				memcpy(s, h->header, len);
				s[len] = 0;
				do {
					p++;
				} while (*p == ' ' || *p == '\t');
				add_assoc_stringl_ex(return_value, s, len+1, p, h->header_len - (p - h->header), 1);
				free_alloca(s, use_heap);
			}
		}
	}
}
/* }}} */

PHP_FUNCTION(apache_response_headers) /* {{{ */
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (!&SG(sapi_headers).headers) {
		RETURN_FALSE;
	}
	array_init(return_value);
	zend_llist_apply_with_argument(&SG(sapi_headers).headers, (llist_apply_with_arg_func_t)add_response_header, return_value TSRMLS_CC);
}
/* }}} */

/* {{{ cli_server module
 */

static void cli_server_init_globals(zend_cli_server_globals *cg TSRMLS_DC)
{
	cg->color = 0;
}

PHP_INI_BEGIN()
	STD_PHP_INI_BOOLEAN("cli_server.color", "0", PHP_INI_ALL, OnUpdateBool, color, zend_cli_server_globals, cli_server_globals)
PHP_INI_END()

static PHP_MINIT_FUNCTION(cli_server)
{
	ZEND_INIT_MODULE_GLOBALS(cli_server, cli_server_init_globals, NULL);
	REGISTER_INI_ENTRIES();
	return SUCCESS;
}

static PHP_MSHUTDOWN_FUNCTION(cli_server)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}

static PHP_MINFO_FUNCTION(cli_server)
{
	DISPLAY_INI_ENTRIES();
}

zend_module_entry cli_server_module_entry = {
	STANDARD_MODULE_HEADER,
	"cli_server",
	NULL,
	PHP_MINIT(cli_server),
	PHP_MSHUTDOWN(cli_server),
	NULL,
	NULL,
	PHP_MINFO(cli_server),
	PHP_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

ZEND_BEGIN_ARG_INFO(arginfo_no_args, 0)
ZEND_END_ARG_INFO()

const zend_function_entry server_additional_functions[] = {
	PHP_FE(cli_set_process_title,        arginfo_cli_set_process_title)
	PHP_FE(cli_get_process_title,        arginfo_cli_get_process_title)
	PHP_FE(apache_request_headers, arginfo_no_args)
	PHP_FE(apache_response_headers, arginfo_no_args)
	PHP_FALIAS(getallheaders, apache_request_headers, arginfo_no_args)
	{NULL, NULL, NULL}
};

static int sapi_cli_server_startup(sapi_module_struct *sapi_module) /* {{{ */
{
	if (php_module_startup(sapi_module, &cli_server_module_entry, 1) == FAILURE) {
		return FAILURE;
	}
	return SUCCESS;
} /* }}} */

static int sapi_cli_server_ub_write(const char *str, uint str_length TSRMLS_DC) /* {{{ */
{
	php_cli_server_client *client = SG(server_context);
	if (!client) {
		return 0;
	}
	return php_cli_server_client_send_through(client, str, str_length);
} /* }}} */

static void sapi_cli_server_flush(void *server_context) /* {{{ */
{
	php_cli_server_client *client = server_context;
	TSRMLS_FETCH();

	if (!client) {
		return;
	}

	if (client->sock < 0) {
		php_handle_aborted_connection();
		return;
	}

	if (!SG(headers_sent)) {
		sapi_send_headers(TSRMLS_C);
		SG(headers_sent) = 1;
	}
} /* }}} */

static int sapi_cli_server_discard_headers(sapi_headers_struct *sapi_headers TSRMLS_DC) /* {{{ */{
	return SAPI_HEADER_SENT_SUCCESSFULLY;
}
/* }}} */

static int sapi_cli_server_send_headers(sapi_headers_struct *sapi_headers TSRMLS_DC) /* {{{ */
{
	php_cli_server_client *client = SG(server_context);
	smart_str buffer = { 0 };
	sapi_header_struct *h;
	zend_llist_position pos;

	if (client == NULL || SG(request_info).no_headers) {
		return SAPI_HEADER_SENT_SUCCESSFULLY;
	}

	if (SG(sapi_headers).http_status_line) {
		smart_str_appends(&buffer, SG(sapi_headers).http_status_line);
		smart_str_appendl(&buffer, "\r\n", 2);
	} else {
		append_http_status_line(&buffer, client->request.protocol_version, SG(sapi_headers).http_response_code, 0);
	}

	append_essential_headers(&buffer, client, 0);

	h = (sapi_header_struct*)zend_llist_get_first_ex(&sapi_headers->headers, &pos);
	while (h) {
		if (h->header_len) {
			smart_str_appendl(&buffer, h->header, h->header_len);
			smart_str_appendl(&buffer, "\r\n", 2);
		}
		h = (sapi_header_struct*)zend_llist_get_next_ex(&sapi_headers->headers, &pos);
	}
	smart_str_appendl(&buffer, "\r\n", 2);

	php_cli_server_client_send_through(client, buffer.c, buffer.len);

	smart_str_free(&buffer);
	return SAPI_HEADER_SENT_SUCCESSFULLY;
}
/* }}} */

static char *sapi_cli_server_read_cookies(TSRMLS_D) /* {{{ */
{
	php_cli_server_client *client = SG(server_context);
	char **val;
	if (FAILURE == zend_hash_find(&client->request.headers, "cookie", sizeof("cookie"), (void**)&val)) {
		return NULL;
	}
	return *val;
} /* }}} */

static int sapi_cli_server_read_post(char *buf, uint count_bytes TSRMLS_DC) /* {{{ */
{
	php_cli_server_client *client = SG(server_context);
	if (client->request.content) {
		size_t content_len = client->request.content_len;
		size_t nbytes_copied = MIN(client->post_read_offset + count_bytes, content_len) - client->post_read_offset;
		memmove(buf, client->request.content + client->post_read_offset, nbytes_copied);
		client->post_read_offset += nbytes_copied;
		return nbytes_copied;
	}
	return 0;
} /* }}} */

static void sapi_cli_server_register_variable(zval *track_vars_array, const char *key, const char *val TSRMLS_DC) /* {{{ */
{
	char *new_val = (char *)val;
	uint new_val_len;

	if (NULL == val) {
		return;
	}

	if (sapi_module.input_filter(PARSE_SERVER, (char*)key, &new_val, strlen(val), &new_val_len TSRMLS_CC)) {
		php_register_variable_safe((char *)key, new_val, new_val_len, track_vars_array TSRMLS_CC);
	}
} /* }}} */

static int sapi_cli_server_register_entry_cb(char **entry TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */ {
	zval *track_vars_array = va_arg(args, zval *);
	if (hash_key->nKeyLength) {
		char *real_key, *key;
		uint i;
		key = estrndup(hash_key->arKey, hash_key->nKeyLength);
		for(i=0; i<hash_key->nKeyLength; i++) {
			if (key[i] == '-') {
				key[i] = '_';
			} else {
				key[i] = toupper(key[i]);
			}
		}
		spprintf(&real_key, 0, "%s_%s", "HTTP", key);
		if (strcmp(key, "CONTENT_TYPE") == 0 || strcmp(key, "CONTENT_LENGTH") == 0) {
			sapi_cli_server_register_variable(track_vars_array, key, *entry TSRMLS_CC);
		}
		sapi_cli_server_register_variable(track_vars_array, real_key, *entry TSRMLS_CC);
		efree(key);
		efree(real_key);
	}

	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

static void sapi_cli_server_register_variables(zval *track_vars_array TSRMLS_DC) /* {{{ */
{
	php_cli_server_client *client = SG(server_context);
	sapi_cli_server_register_variable(track_vars_array, "DOCUMENT_ROOT", client->server->document_root TSRMLS_CC);
	{
		char *tmp;
		if ((tmp = strrchr(client->addr_str, ':'))) {
			char addr[64], port[8];
			strncpy(port, tmp + 1, 8);
			port[7] = '\0';
			strncpy(addr, client->addr_str, tmp - client->addr_str);
			addr[tmp - client->addr_str] = '\0';
			sapi_cli_server_register_variable(track_vars_array, "REMOTE_ADDR", addr TSRMLS_CC);
			sapi_cli_server_register_variable(track_vars_array, "REMOTE_PORT", port TSRMLS_CC);
		} else {
			sapi_cli_server_register_variable(track_vars_array, "REMOTE_ADDR", client->addr_str TSRMLS_CC);
		}
	}
	{
		char *tmp;
		spprintf(&tmp, 0, "PHP %s Development Server", PHP_VERSION);
		sapi_cli_server_register_variable(track_vars_array, "SERVER_SOFTWARE", tmp TSRMLS_CC);
		efree(tmp);
	}
	{
		char *tmp;
		spprintf(&tmp, 0, "HTTP/%d.%d", client->request.protocol_version / 100, client->request.protocol_version % 100);
		sapi_cli_server_register_variable(track_vars_array, "SERVER_PROTOCOL", tmp TSRMLS_CC);
		efree(tmp);
	}
	sapi_cli_server_register_variable(track_vars_array, "SERVER_NAME", client->server->host TSRMLS_CC);
	{
		char *tmp;
		spprintf(&tmp, 0, "%i",  client->server->port);
		sapi_cli_server_register_variable(track_vars_array, "SERVER_PORT", tmp TSRMLS_CC);
		efree(tmp);
	}

	sapi_cli_server_register_variable(track_vars_array, "REQUEST_URI", client->request.request_uri TSRMLS_CC);
	sapi_cli_server_register_variable(track_vars_array, "REQUEST_METHOD", SG(request_info).request_method TSRMLS_CC);
	sapi_cli_server_register_variable(track_vars_array, "SCRIPT_NAME", client->request.vpath TSRMLS_CC);
	if (SG(request_info).path_translated) {
		sapi_cli_server_register_variable(track_vars_array, "SCRIPT_FILENAME", SG(request_info).path_translated TSRMLS_CC);
	} else if (client->server->router) {
		char *temp;
		spprintf(&temp, 0, "%s/%s", client->server->document_root, client->server->router);
		sapi_cli_server_register_variable(track_vars_array, "SCRIPT_FILENAME", temp TSRMLS_CC);
		efree(temp);
	}
	if (client->request.path_info) {
		sapi_cli_server_register_variable(track_vars_array, "PATH_INFO", client->request.path_info TSRMLS_CC);
	}
	if (client->request.path_info_len) {
		char *tmp;
		spprintf(&tmp, 0, "%s%s", client->request.vpath, client->request.path_info);
		sapi_cli_server_register_variable(track_vars_array, "PHP_SELF", tmp TSRMLS_CC);
		efree(tmp);
	} else {
		sapi_cli_server_register_variable(track_vars_array, "PHP_SELF", client->request.vpath TSRMLS_CC);
	}
	if (client->request.query_string) {
		sapi_cli_server_register_variable(track_vars_array, "QUERY_STRING", client->request.query_string TSRMLS_CC);
	}
	zend_hash_apply_with_arguments(&client->request.headers TSRMLS_CC, (apply_func_args_t)sapi_cli_server_register_entry_cb, 1, track_vars_array);
} /* }}} */

static void sapi_cli_server_log_message(char *msg TSRMLS_DC) /* {{{ */
{
	char buf[52];

	if (php_cli_server_get_system_time(buf) != 0) {
		memmove(buf, "unknown time, can't be fetched", sizeof("unknown time, can't be fetched"));
	} else {
		size_t l = strlen(buf);
		if (l > 0) {
			buf[l - 1] = '\0';
		} else {
			memmove(buf, "unknown", sizeof("unknown"));
		}
	}
	fprintf(stderr, "[%s] %s\n", buf, msg);
} /* }}} */

/* {{{ sapi_module_struct cli_server_sapi_module
 */
sapi_module_struct cli_server_sapi_module = {
	"cli-server",							/* name */
	"Built-in HTTP server",		/* pretty name */

	sapi_cli_server_startup,				/* startup */
	php_module_shutdown_wrapper,	/* shutdown */

	NULL,							/* activate */
	NULL,							/* deactivate */

	sapi_cli_server_ub_write,		/* unbuffered write */
	sapi_cli_server_flush,			/* flush */
	NULL,							/* get uid */
	NULL,							/* getenv */

	php_error,						/* error handler */

	NULL,							/* header handler */
	sapi_cli_server_send_headers,	/* send headers handler */
	NULL,							/* send header handler */

	sapi_cli_server_read_post,		/* read POST data */
	sapi_cli_server_read_cookies,	/* read Cookies */

	sapi_cli_server_register_variables,	/* register server variables */
	sapi_cli_server_log_message,	/* Log message */
	NULL,							/* Get request time */
	NULL,							/* Child terminate */

	STANDARD_SAPI_MODULE_PROPERTIES
}; /* }}} */

static int php_cli_server_poller_ctor(php_cli_server_poller *poller) /* {{{ */
{
	FD_ZERO(&poller->rfds);
	FD_ZERO(&poller->wfds);
	poller->max_fd = -1;
	return SUCCESS;
} /* }}} */

static void php_cli_server_poller_add(php_cli_server_poller *poller, int mode, int fd) /* {{{ */
{
	if (mode & POLLIN) {
		PHP_SAFE_FD_SET(fd, &poller->rfds);
	}
	if (mode & POLLOUT) {
		PHP_SAFE_FD_SET(fd, &poller->wfds);
	}
	if (fd > poller->max_fd) {
		poller->max_fd = fd;
	}
} /* }}} */

static void php_cli_server_poller_remove(php_cli_server_poller *poller, int mode, int fd) /* {{{ */
{
	if (mode & POLLIN) {
		PHP_SAFE_FD_CLR(fd, &poller->rfds);
	}
	if (mode & POLLOUT) {
		PHP_SAFE_FD_CLR(fd, &poller->wfds);
	}
#ifndef PHP_WIN32
	if (fd == poller->max_fd) {
		while (fd > 0) {
			fd--;
			if (PHP_SAFE_FD_ISSET(fd, &poller->rfds) || PHP_SAFE_FD_ISSET(fd, &poller->wfds)) {
				break;
			}
		}
		poller->max_fd = fd;
	}
#endif
} /* }}} */

static int php_cli_server_poller_poll(php_cli_server_poller *poller, struct timeval *tv) /* {{{ */
{
	memmove(&poller->active.rfds, &poller->rfds, sizeof(poller->rfds));
	memmove(&poller->active.wfds, &poller->wfds, sizeof(poller->wfds));
	return php_select(poller->max_fd + 1, &poller->active.rfds, &poller->active.wfds, NULL, tv);
} /* }}} */

static int php_cli_server_poller_iter_on_active(php_cli_server_poller *poller, void *opaque, int(*callback)(void *, int fd, int events)) /* {{{ */
{
	int retval = SUCCESS;
#ifdef PHP_WIN32
	struct socket_entry {
		SOCKET fd;
		int events;
	} entries[FD_SETSIZE * 2];
	php_socket_t fd = 0;
	size_t i;
	struct socket_entry *n = entries, *m;

	for (i = 0; i < poller->active.rfds.fd_count; i++) {
		n->events = POLLIN;
		n->fd = poller->active.rfds.fd_array[i];
		n++;
	}

	m = n;
	for (i = 0; i < poller->active.wfds.fd_count; i++) {
		struct socket_entry *e;
		SOCKET fd = poller->active.wfds.fd_array[i];
		for (e = entries; e < m; e++) {
			if (e->fd == fd) {
				e->events |= POLLOUT;
			}
		}
		if (e == m) {
			assert(n < entries + FD_SETSIZE * 2);
			n->events = POLLOUT;
			n->fd = fd;
			n++;
		}
	}

	{
		struct socket_entry *e = entries;
		for (; e < n; e++) {
			if (SUCCESS != callback(opaque, e->fd, e->events)) {
				retval = FAILURE;
			}
		}
	}

#else
	php_socket_t fd;
	const php_socket_t max_fd = poller->max_fd;

	for (fd=0 ; fd<=max_fd ; fd++)  {
		if (PHP_SAFE_FD_ISSET(fd, &poller->active.rfds)) {
                if (SUCCESS != callback(opaque, fd, POLLIN)) {
                    retval = FAILURE;
                }
		}
		if (PHP_SAFE_FD_ISSET(fd, &poller->active.wfds)) {
                if (SUCCESS != callback(opaque, fd, POLLOUT)) {
                    retval = FAILURE;
                }
		}
	}
#endif
	return retval;
} /* }}} */

static size_t php_cli_server_chunk_size(const php_cli_server_chunk *chunk) /* {{{ */
{
	switch (chunk->type) {
	case PHP_CLI_SERVER_CHUNK_HEAP:
		return chunk->data.heap.len;
	case PHP_CLI_SERVER_CHUNK_IMMORTAL:
		return chunk->data.immortal.len;
	}
	return 0;
} /* }}} */

static void php_cli_server_chunk_dtor(php_cli_server_chunk *chunk) /* {{{ */
{
	switch (chunk->type) {
	case PHP_CLI_SERVER_CHUNK_HEAP:
		if (chunk->data.heap.block != chunk) {
			pefree(chunk->data.heap.block, 1);
		}
		break;
	case PHP_CLI_SERVER_CHUNK_IMMORTAL:
		break;
	}
} /* }}} */

static void php_cli_server_buffer_dtor(php_cli_server_buffer *buffer) /* {{{ */
{
	php_cli_server_chunk *chunk, *next;
	for (chunk = buffer->first; chunk; chunk = next) {
		next = chunk->next;
		php_cli_server_chunk_dtor(chunk);
		pefree(chunk, 1);
	}
} /* }}} */

static void php_cli_server_buffer_ctor(php_cli_server_buffer *buffer) /* {{{ */
{
	buffer->first = NULL;
	buffer->last = NULL;
} /* }}} */

static void php_cli_server_buffer_append(php_cli_server_buffer *buffer, php_cli_server_chunk *chunk) /* {{{ */
{
	php_cli_server_chunk *last;
	for (last = chunk; last->next; last = last->next);
	if (!buffer->last) {
		buffer->first = chunk;
	} else {
		buffer->last->next = chunk;
	}
	buffer->last = last;
} /* }}} */

static void php_cli_server_buffer_prepend(php_cli_server_buffer *buffer, php_cli_server_chunk *chunk) /* {{{ */
{
	php_cli_server_chunk *last;
	for (last = chunk; last->next; last = last->next);
	last->next = buffer->first;
	if (!buffer->last) {
		buffer->last = last;
	}
	buffer->first = chunk;
} /* }}} */

static size_t php_cli_server_buffer_size(const php_cli_server_buffer *buffer) /* {{{ */
{
	php_cli_server_chunk *chunk;
	size_t retval = 0;
	for (chunk = buffer->first; chunk; chunk = chunk->next) {
		retval += php_cli_server_chunk_size(chunk);
	}
	return retval;
} /* }}} */

static php_cli_server_chunk *php_cli_server_chunk_immortal_new(const char *buf, size_t len) /* {{{ */
{
	php_cli_server_chunk *chunk = pemalloc(sizeof(php_cli_server_chunk), 1);
	if (!chunk) {
		return NULL;
	}

	chunk->type = PHP_CLI_SERVER_CHUNK_IMMORTAL;
	chunk->next = NULL;
	chunk->data.immortal.p = buf;
	chunk->data.immortal.len = len;
	return chunk;
} /* }}} */

static php_cli_server_chunk *php_cli_server_chunk_heap_new(char *block, char *buf, size_t len) /* {{{ */
{
	php_cli_server_chunk *chunk = pemalloc(sizeof(php_cli_server_chunk), 1);
	if (!chunk) {
		return NULL;
	}

	chunk->type = PHP_CLI_SERVER_CHUNK_HEAP;
	chunk->next = NULL;
	chunk->data.heap.block = block;
	chunk->data.heap.p = buf;
	chunk->data.heap.len = len;
	return chunk;
} /* }}} */

static php_cli_server_chunk *php_cli_server_chunk_heap_new_self_contained(size_t len) /* {{{ */
{
	php_cli_server_chunk *chunk = pemalloc(sizeof(php_cli_server_chunk) + len, 1);
	if (!chunk) {
		return NULL;
	}

	chunk->type = PHP_CLI_SERVER_CHUNK_HEAP;
	chunk->next = NULL;
	chunk->data.heap.block = chunk;
	chunk->data.heap.p = (char *)(chunk + 1);
	chunk->data.heap.len = len;
	return chunk;
} /* }}} */

static void php_cli_server_content_sender_dtor(php_cli_server_content_sender *sender) /* {{{ */
{
	php_cli_server_buffer_dtor(&sender->buffer);
} /* }}} */

static void php_cli_server_content_sender_ctor(php_cli_server_content_sender *sender) /* {{{ */
{
	php_cli_server_buffer_ctor(&sender->buffer);
} /* }}} */

static int php_cli_server_content_sender_send(php_cli_server_content_sender *sender, php_socket_t fd, size_t *nbytes_sent_total) /* {{{ */
{
	php_cli_server_chunk *chunk, *next;
	size_t _nbytes_sent_total = 0;

	for (chunk = sender->buffer.first; chunk; chunk = next) {
		ssize_t nbytes_sent;
		next = chunk->next;

		switch (chunk->type) {
		case PHP_CLI_SERVER_CHUNK_HEAP:
			nbytes_sent = send(fd, chunk->data.heap.p, chunk->data.heap.len, 0);
			if (nbytes_sent < 0) {
				*nbytes_sent_total = _nbytes_sent_total;
				return php_socket_errno();
			} else if (nbytes_sent == chunk->data.heap.len) {
				php_cli_server_chunk_dtor(chunk);
				pefree(chunk, 1);
				sender->buffer.first = next;
				if (!next) {
					sender->buffer.last = NULL;
				}
			} else {
				chunk->data.heap.p += nbytes_sent;
				chunk->data.heap.len -= nbytes_sent;
			}
			_nbytes_sent_total += nbytes_sent;
			break;

		case PHP_CLI_SERVER_CHUNK_IMMORTAL:
			nbytes_sent = send(fd, chunk->data.immortal.p, chunk->data.immortal.len, 0);
			if (nbytes_sent < 0) {
				*nbytes_sent_total = _nbytes_sent_total;
				return php_socket_errno();
			} else if (nbytes_sent == chunk->data.immortal.len) {
				php_cli_server_chunk_dtor(chunk);
				pefree(chunk, 1);
				sender->buffer.first = next;
				if (!next) {
					sender->buffer.last = NULL;
				}
			} else {
				chunk->data.immortal.p += nbytes_sent;
				chunk->data.immortal.len -= nbytes_sent;
			}
			_nbytes_sent_total += nbytes_sent;
			break;
		}
	}
	*nbytes_sent_total = _nbytes_sent_total;
	return 0;
} /* }}} */

static int php_cli_server_content_sender_pull(php_cli_server_content_sender *sender, int fd, size_t *nbytes_read) /* {{{ */
{
	ssize_t _nbytes_read;
	php_cli_server_chunk *chunk = php_cli_server_chunk_heap_new_self_contained(131072);

	_nbytes_read = read(fd, chunk->data.heap.p, chunk->data.heap.len);
	if (_nbytes_read < 0) {
		char *errstr = get_last_error();
		TSRMLS_FETCH();
		php_cli_server_logf("%s" TSRMLS_CC, errstr);
		pefree(errstr, 1);
		php_cli_server_chunk_dtor(chunk);
		pefree(chunk, 1);
		return 1;
	}
	chunk->data.heap.len = _nbytes_read;
	php_cli_server_buffer_append(&sender->buffer, chunk);
	*nbytes_read = _nbytes_read;
	return 0;
} /* }}} */

#if HAVE_UNISTD_H
static int php_cli_is_output_tty() /* {{{ */
{
	if (php_cli_output_is_tty == OUTPUT_NOT_CHECKED) {
		php_cli_output_is_tty = isatty(STDOUT_FILENO);
	}
	return php_cli_output_is_tty;
} /* }}} */
#endif

static void php_cli_server_log_response(php_cli_server_client *client, int status, const char *message TSRMLS_DC) /* {{{ */
{
	int color = 0, effective_status = status;
	char *basic_buf, *message_buf = "", *error_buf = "";
	zend_bool append_error_message = 0;

	if (PG(last_error_message)) {
		switch (PG(last_error_type)) {
			case E_ERROR:
			case E_CORE_ERROR:
			case E_COMPILE_ERROR:
			case E_USER_ERROR:
			case E_PARSE:
				if (status == 200) {
					/* the status code isn't changed by a fatal error, so fake it */
					effective_status = 500;
				}

				append_error_message = 1;
				break;
		}
	}

#if HAVE_UNISTD_H
	if (CLI_SERVER_G(color) && php_cli_is_output_tty() == OUTPUT_IS_TTY) {
		if (effective_status >= 500) {
			/* server error: red */
			color = 1;
		} else if (effective_status >= 400) {
			/* client error: yellow */
			color = 3;
		} else if (effective_status >= 200) {
			/* success: green */
			color = 2;
		}
	}
#endif

	/* basic */
	spprintf(&basic_buf, 0, "%s [%d]: %s", client->addr_str, status, client->request.request_uri);
	if (!basic_buf) {
		return;
	}

	/* message */
	if (message) {
		spprintf(&message_buf, 0, " - %s", message);
		if (!message_buf) {
			efree(basic_buf);
			return;
		}
	}

	/* error */
	if (append_error_message) {
		spprintf(&error_buf, 0, " - %s in %s on line %d", PG(last_error_message), PG(last_error_file), PG(last_error_lineno));
		if (!error_buf) {
			efree(basic_buf);
			if (message) {
				efree(message_buf);
			}
			return;
		}
	}

	if (color) {
		php_cli_server_logf("\x1b[3%dm%s%s%s\x1b[0m" TSRMLS_CC, color, basic_buf, message_buf, error_buf);
	} else {
		php_cli_server_logf("%s%s%s" TSRMLS_CC, basic_buf, message_buf, error_buf);
	}

	efree(basic_buf);
	if (message) {
		efree(message_buf);
	}
	if (append_error_message) {
		efree(error_buf);
	}
} /* }}} */

static void php_cli_server_logf(const char *format TSRMLS_DC, ...) /* {{{ */
{
	char *buf = NULL;
	va_list ap;
#ifdef ZTS
	va_start(ap, tsrm_ls);
#else
	va_start(ap, format);
#endif
	vspprintf(&buf, 0, format, ap);
	va_end(ap);

	if (!buf) {
		return;
	}

	if (sapi_module.log_message) {
		sapi_module.log_message(buf TSRMLS_CC);
	}

	efree(buf);
} /* }}} */

static int php_network_listen_socket(const char *host, int *port, int socktype, int *af, socklen_t *socklen, char **errstr TSRMLS_DC) /* {{{ */
{
	int retval = SOCK_ERR;
	int err = 0;
	struct sockaddr *sa = NULL, **p, **sal;

	int num_addrs = php_network_getaddresses(host, socktype, &sal, errstr TSRMLS_CC);
	if (num_addrs == 0) {
		return -1;
	}
	for (p = sal; *p; p++) {
		if (sa) {
			pefree(sa, 1);
			sa = NULL;
		}

		retval = socket((*p)->sa_family, socktype, 0);
		if (retval == SOCK_ERR) {
			continue;
		}

		switch ((*p)->sa_family) {
#if HAVE_GETADDRINFO && HAVE_IPV6
		case AF_INET6:
			sa = pemalloc(sizeof(struct sockaddr_in6), 1);
			if (!sa) {
				closesocket(retval);
				retval = SOCK_ERR;
				*errstr = NULL;
				goto out;
			}
			*(struct sockaddr_in6 *)sa = *(struct sockaddr_in6 *)*p;
			((struct sockaddr_in6 *)sa)->sin6_port = htons(*port);
			*socklen = sizeof(struct sockaddr_in6);
			break;
#endif
		case AF_INET:
			sa = pemalloc(sizeof(struct sockaddr_in), 1);
			if (!sa) {
				closesocket(retval);
				retval = SOCK_ERR;
				*errstr = NULL;
				goto out;
			}
			*(struct sockaddr_in *)sa = *(struct sockaddr_in *)*p;
			((struct sockaddr_in *)sa)->sin_port = htons(*port);
			*socklen = sizeof(struct sockaddr_in);
			break;
		default:
			/* Unknown family */
			*socklen = 0;
			closesocket(retval);
			continue;
		}

#ifdef SO_REUSEADDR
		{
			int val = 1;
			setsockopt(retval, SOL_SOCKET, SO_REUSEADDR, (char*)&val, sizeof(val));
		}
#endif

		if (bind(retval, sa, *socklen) == SOCK_CONN_ERR) {
			err = php_socket_errno();
			if (err == SOCK_EINVAL || err == SOCK_EADDRINUSE) {
				goto out;
			}
			closesocket(retval);
			retval = SOCK_ERR;
			continue;
		}
		err = 0;

		*af = sa->sa_family;
		if (*port == 0) {
			if (getsockname(retval, sa, socklen)) {
				err = php_socket_errno();
				goto out;
			}
			switch (sa->sa_family) {
#if HAVE_GETADDRINFO && HAVE_IPV6
			case AF_INET6:
				*port = ntohs(((struct sockaddr_in6 *)sa)->sin6_port);
				break;
#endif
			case AF_INET:
				*port = ntohs(((struct sockaddr_in *)sa)->sin_port);
				break;
			}
		}

		break;
	}

	if (retval == SOCK_ERR) {
		goto out;
	}

	if (listen(retval, SOMAXCONN)) {
		err = php_socket_errno();
		goto out;
	}

out:
	if (sa) {
		pefree(sa, 1);
	}
	if (sal) {
		php_network_freeaddresses(sal);
	}
	if (err) {
		if (retval >= 0) {
			closesocket(retval);
		}
		if (errstr) {
			*errstr = php_socket_strerror(err, NULL, 0);
		}
		return SOCK_ERR;
	}
	return retval;
} /* }}} */

static int php_cli_server_request_ctor(php_cli_server_request *req) /* {{{ */
{
	req->protocol_version = 0;
	req->request_uri = NULL;
	req->request_uri_len = 0;
	req->vpath = NULL;
	req->vpath_len = 0;
	req->path_translated = NULL;
	req->path_translated_len = 0;
	req->path_info = NULL;
	req->path_info_len = 0;
	req->query_string = NULL;
	req->query_string_len = 0;
	zend_hash_init(&req->headers, 0, NULL, (void(*)(void*))char_ptr_dtor_p, 1);
	zend_hash_init(&req->headers_original_case, 0, NULL, NULL, 1);
	req->content = NULL;
	req->content_len = 0;
	req->ext = NULL;
	req->ext_len = 0;
	return SUCCESS;
} /* }}} */

static void php_cli_server_request_dtor(php_cli_server_request *req) /* {{{ */
{
	if (req->request_uri) {
		pefree(req->request_uri, 1);
	}
	if (req->vpath) {
		pefree(req->vpath, 1);
	}
	if (req->path_translated) {
		pefree(req->path_translated, 1);
	}
	if (req->path_info) {
		pefree(req->path_info, 1);
	}
	if (req->query_string) {
		pefree(req->query_string, 1);
	}
	zend_hash_destroy(&req->headers);
	zend_hash_destroy(&req->headers_original_case);
	if (req->content) {
		pefree(req->content, 1);
	}
} /* }}} */

static void php_cli_server_request_translate_vpath(php_cli_server_request *request, const char *document_root, size_t document_root_len) /* {{{ */
{
	struct stat sb;
	static const char *index_files[] = { "index.php", "index.html", NULL };
	char *buf = safe_pemalloc(1, request->vpath_len, 1 + document_root_len + 1 + sizeof("index.html"), 1);
	char *p = buf, *prev_path = NULL, *q, *vpath;
	size_t prev_path_len = 0;
	int  is_static_file = 0;

	if (!buf) {
		return;
	}

	memmove(p, document_root, document_root_len);
	p += document_root_len;
	vpath = p;
	if (request->vpath_len > 0 && request->vpath[0] != '/') {
		*p++ = DEFAULT_SLASH;
	}
	q = request->vpath + request->vpath_len;
	while (q > request->vpath) {
		if (*q-- == '.') {
			is_static_file = 1;
			break;
		}
	}
	memmove(p, request->vpath, request->vpath_len);
#ifdef PHP_WIN32
	q = p + request->vpath_len;
	do {
		if (*q == '/') {
			*q = '\\';
		}
	} while (q-- > p);
#endif
	p += request->vpath_len;
	*p = '\0';
	q = p;
	while (q > buf) {
		if (!stat(buf, &sb)) {
			if (sb.st_mode & S_IFDIR) {
				const char **file = index_files;
				if (q[-1] != DEFAULT_SLASH) {
					*q++ = DEFAULT_SLASH;
				}
				while (*file) {
					size_t l = strlen(*file);
					memmove(q, *file, l + 1);
					if (!stat(buf, &sb) && (sb.st_mode & S_IFREG)) {
						q += l;
						break;
					}
					file++;
				}
				if (!*file || is_static_file) {
					if (prev_path) {
						pefree(prev_path, 1);
					}
					pefree(buf, 1);
					return;
				}
			}
			break; /* regular file */
		}
		if (prev_path) {
			pefree(prev_path, 1);
			*q = DEFAULT_SLASH;
		}
		while (q > buf && *(--q) != DEFAULT_SLASH);
		prev_path_len = p - q;
		prev_path = pestrndup(q, prev_path_len, 1);
		*q = '\0';
	}
	if (prev_path) {
		request->path_info_len = prev_path_len;
#ifdef PHP_WIN32
		while (prev_path_len--) {
			if (prev_path[prev_path_len] == '\\') {
				prev_path[prev_path_len] = '/';
			}
		}
#endif
		request->path_info = prev_path;
		pefree(request->vpath, 1);
		request->vpath = pestrndup(vpath, q - vpath, 1);
		request->vpath_len = q - vpath;
		request->path_translated = buf;
		request->path_translated_len = q - buf;
	} else {
		pefree(request->vpath, 1);
		request->vpath = pestrndup(vpath, q - vpath, 1);
		request->vpath_len = q - vpath;
		request->path_translated = buf;
		request->path_translated_len = q - buf;
	}
#ifdef PHP_WIN32
	{
		uint i = 0;
		for (;i<request->vpath_len;i++) {
			if (request->vpath[i] == '\\') {
				request->vpath[i] = '/';
			}
		}
	}
#endif
	request->sb = sb;
} /* }}} */

static void normalize_vpath(char **retval, size_t *retval_len, const char *vpath, size_t vpath_len, int persistent) /* {{{ */
{
	char *decoded_vpath = NULL;
	char *decoded_vpath_end;
	char *p;

	*retval = NULL;

	decoded_vpath = pestrndup(vpath, vpath_len, persistent);
	if (!decoded_vpath) {
		return;
	}

	decoded_vpath_end = decoded_vpath + php_raw_url_decode(decoded_vpath, vpath_len);

#ifdef PHP_WIN32
	{
		char *p = decoded_vpath;
		
		do {
			if (*p == '\\') {
				*p = '/';
			}
		} while (*p++);
	}
#endif

	p = decoded_vpath;

	if (p < decoded_vpath_end && *p == '/') {
		char *n = p;
		while (n < decoded_vpath_end && *n == '/') n++;
		memmove(++p, n, decoded_vpath_end - n);
		decoded_vpath_end -= n - p;
	}

	while (p < decoded_vpath_end) {
		char *n = p;
		while (n < decoded_vpath_end && *n != '/') n++;
		if (n - p == 2 && p[0] == '.' && p[1] == '.') {
			if (p > decoded_vpath) {
				--p;
				for (;;) {
					if (p == decoded_vpath) {
						if (*p == '/') {
							p++;
						}
						break;
					}
					if (*(--p) == '/') {
						p++;
						break;
					}
				}
			}
			while (n < decoded_vpath_end && *n == '/') n++;
			memmove(p, n, decoded_vpath_end - n);
			decoded_vpath_end -= n - p;
		} else if (n - p == 1 && p[0] == '.') {
			while (n < decoded_vpath_end && *n == '/') n++;
			memmove(p, n, decoded_vpath_end - n);
			decoded_vpath_end -= n - p;
		} else {
			if (n < decoded_vpath_end) {
				char *nn = n;
				while (nn < decoded_vpath_end && *nn == '/') nn++;
				p = n + 1;
				memmove(p, nn, decoded_vpath_end - nn);
				decoded_vpath_end -= nn - p;
			} else {
				p = n;
			}
		}
	}

	*decoded_vpath_end = '\0';
	*retval = decoded_vpath;
	*retval_len = decoded_vpath_end - decoded_vpath;
} /* }}} */

/* {{{ php_cli_server_client_read_request */
static int php_cli_server_client_read_request_on_message_begin(php_http_parser *parser)
{
	return 0;
}

static int php_cli_server_client_read_request_on_path(php_http_parser *parser, const char *at, size_t length)
{
	php_cli_server_client *client = parser->data;
	{
		char *vpath;
		size_t vpath_len;
		normalize_vpath(&vpath, &vpath_len, at, length, 1);
		client->request.vpath = vpath;
		client->request.vpath_len = vpath_len;
	}
	return 0;
}

static int php_cli_server_client_read_request_on_query_string(php_http_parser *parser, const char *at, size_t length)
{
	php_cli_server_client *client = parser->data;
	client->request.query_string = pestrndup(at, length, 1);
	client->request.query_string_len = length;
	return 0;
}

static int php_cli_server_client_read_request_on_url(php_http_parser *parser, const char *at, size_t length)
{
	php_cli_server_client *client = parser->data;
	client->request.request_method = parser->method;
	client->request.request_uri = pestrndup(at, length, 1);
	client->request.request_uri_len = length;
	return 0;
}

static int php_cli_server_client_read_request_on_fragment(php_http_parser *parser, const char *at, size_t length)
{
	return 0;
}

static int php_cli_server_client_read_request_on_header_field(php_http_parser *parser, const char *at, size_t length)
{
	php_cli_server_client *client = parser->data;
	if (client->current_header_name_allocated) {
		pefree(client->current_header_name, 1);
		client->current_header_name_allocated = 0;
	}
	client->current_header_name = (char *)at;
	client->current_header_name_len = length;
	return 0;
}

static int php_cli_server_client_read_request_on_header_value(php_http_parser *parser, const char *at, size_t length)
{
	php_cli_server_client *client = parser->data;
	char *value = pestrndup(at, length, 1);
	if (!value) {
		return 1;
	}
	{
		/* strip off the colon */
		char *orig_header_name = estrndup(client->current_header_name, client->current_header_name_len);
		char *lc_header_name = zend_str_tolower_dup(client->current_header_name, client->current_header_name_len);

		zend_hash_add(&client->request.headers, lc_header_name, client->current_header_name_len + 1, &value, sizeof(char *), NULL);
		zend_hash_add(&client->request.headers_original_case, orig_header_name, client->current_header_name_len + 1, &value, sizeof(char *), NULL);
		efree(lc_header_name);
		efree(orig_header_name);
	}

	if (client->current_header_name_allocated) {
		pefree(client->current_header_name, 1);
		client->current_header_name_allocated = 0;
	}
	return 0;
}

static int php_cli_server_client_read_request_on_headers_complete(php_http_parser *parser)
{
	php_cli_server_client *client = parser->data;
	if (client->current_header_name_allocated) {
		pefree(client->current_header_name, 1);
		client->current_header_name_allocated = 0;
	}
	client->current_header_name = NULL;
	return 0;
}

static int php_cli_server_client_read_request_on_body(php_http_parser *parser, const char *at, size_t length)
{
	php_cli_server_client *client = parser->data;
	if (!client->request.content) {
		client->request.content = pemalloc(parser->content_length, 1);
		if (!client->request.content) {
			return -1;
		}
		client->request.content_len = 0;
	}
	client->request.content = perealloc(client->request.content, client->request.content_len + length, 1);
	memmove(client->request.content + client->request.content_len, at, length);
	client->request.content_len += length;
	return 0;
}

static int php_cli_server_client_read_request_on_message_complete(php_http_parser *parser)
{
	php_cli_server_client *client = parser->data;
	client->request.protocol_version = parser->http_major * 100 + parser->http_minor;
	php_cli_server_request_translate_vpath(&client->request, client->server->document_root, client->server->document_root_len);
	{
		const char *vpath = client->request.vpath, *end = vpath + client->request.vpath_len, *p = end;
		client->request.ext = end;
		client->request.ext_len = 0;
		while (p > vpath) {
			--p;
			if (*p == '.') {
				++p;
				client->request.ext = p;
				client->request.ext_len = end - p;
				break;
			}
		}
	}
	client->request_read = 1;
	return 0;
}

static int php_cli_server_client_read_request(php_cli_server_client *client, char **errstr TSRMLS_DC)
{
	char buf[16384];
	static const php_http_parser_settings settings = {
		php_cli_server_client_read_request_on_message_begin,
		php_cli_server_client_read_request_on_path,
		php_cli_server_client_read_request_on_query_string,
		php_cli_server_client_read_request_on_url,
		php_cli_server_client_read_request_on_fragment,
		php_cli_server_client_read_request_on_header_field,
		php_cli_server_client_read_request_on_header_value,
		php_cli_server_client_read_request_on_headers_complete,
		php_cli_server_client_read_request_on_body,
		php_cli_server_client_read_request_on_message_complete
	};
	size_t nbytes_consumed;
	int nbytes_read;
	if (client->request_read) {
		return 1;
	}
	nbytes_read = recv(client->sock, buf, sizeof(buf) - 1, 0);
	if (nbytes_read < 0) {
		int err = php_socket_errno();
		if (err == SOCK_EAGAIN) {
			return 0;
		}
		*errstr = php_socket_strerror(err, NULL, 0);
		return -1;
	} else if (nbytes_read == 0) {
		*errstr = estrdup("Unexpected EOF");
		return -1;
	}
	client->parser.data = client;
	nbytes_consumed = php_http_parser_execute(&client->parser, &settings, buf, nbytes_read);
	if (nbytes_consumed != nbytes_read) {
		if (buf[0] & 0x80 /* SSLv2 */ || buf[0] == 0x16 /* SSLv3/TLSv1 */) {
			*errstr = estrdup("Unsupported SSL request");
		} else {
			*errstr = estrdup("Malformed HTTP request");
		}
		return -1;
	}
	if (client->current_header_name) {
		char *header_name = safe_pemalloc(client->current_header_name_len, 1, 1, 1);
		if (!header_name) {
			return -1;
		}
		memmove(header_name, client->current_header_name, client->current_header_name_len);
		client->current_header_name = header_name;
		client->current_header_name_allocated = 1;
	}
	return client->request_read ? 1: 0;
}
/* }}} */

static size_t php_cli_server_client_send_through(php_cli_server_client *client, const char *str, size_t str_len) /* {{{ */
{
	struct timeval tv = { 10, 0 };
	ssize_t nbytes_left = str_len;
	do {
		ssize_t nbytes_sent = send(client->sock, str + str_len - nbytes_left, nbytes_left, 0);
		if (nbytes_sent < 0) {
			int err = php_socket_errno();
			if (err == SOCK_EAGAIN) {
				int nfds = php_pollfd_for(client->sock, POLLOUT, &tv);
				if (nfds > 0) {
					continue;
				} else if (nfds < 0) {
					/* error */
					php_handle_aborted_connection();
					return nbytes_left;
				} else {
					/* timeout */
					php_handle_aborted_connection();
					return nbytes_left;
				}
			} else {
				php_handle_aborted_connection();
				return nbytes_left;
			}
		}
		nbytes_left -= nbytes_sent;
	} while (nbytes_left > 0);

	return str_len;
} /* }}} */

static void php_cli_server_client_populate_request_info(const php_cli_server_client *client, sapi_request_info *request_info) /* {{{ */
{
	char **val;

	request_info->request_method = php_http_method_str(client->request.request_method);
	request_info->proto_num = client->request.protocol_version;
	request_info->request_uri = client->request.request_uri;
	request_info->path_translated = client->request.path_translated;
	request_info->query_string = client->request.query_string;
	request_info->content_length = client->request.content_len;
	request_info->auth_user = request_info->auth_password = request_info->auth_digest = NULL;
	if (SUCCESS == zend_hash_find(&client->request.headers, "content-type", sizeof("content-type"), (void**)&val)) {
		request_info->content_type = *val;
	}
} /* }}} */

static void destroy_request_info(sapi_request_info *request_info) /* {{{ */
{
} /* }}} */

static int php_cli_server_client_ctor(php_cli_server_client *client, php_cli_server *server, int client_sock, struct sockaddr *addr, socklen_t addr_len TSRMLS_DC) /* {{{ */
{
	client->server = server;
	client->sock = client_sock;
	client->addr = addr;
	client->addr_len = addr_len;
	{
		char *addr_str = 0;
		long addr_str_len = 0;
		php_network_populate_name_from_sockaddr(addr, addr_len, &addr_str, &addr_str_len, NULL, 0 TSRMLS_CC);
		client->addr_str = pestrndup(addr_str, addr_str_len, 1);
		client->addr_str_len = addr_str_len;
		efree(addr_str);
	}
	php_http_parser_init(&client->parser, PHP_HTTP_REQUEST);
	client->request_read = 0;
	client->current_header_name = NULL;
	client->current_header_name_len = 0;
	client->current_header_name_allocated = 0;
	client->post_read_offset = 0;
	if (FAILURE == php_cli_server_request_ctor(&client->request)) {
		return FAILURE;
	}
	client->content_sender_initialized = 0;
	client->file_fd = -1;
	return SUCCESS;
} /* }}} */

static void php_cli_server_client_dtor(php_cli_server_client *client) /* {{{ */
{
	php_cli_server_request_dtor(&client->request);
	if (client->file_fd >= 0) {
		close(client->file_fd);
		client->file_fd = -1;
	}
	pefree(client->addr, 1);
	pefree(client->addr_str, 1);
	if (client->content_sender_initialized) {
		php_cli_server_content_sender_dtor(&client->content_sender);
	}
} /* }}} */

static void php_cli_server_close_connection(php_cli_server *server, php_cli_server_client *client TSRMLS_DC) /* {{{ */
{
#ifdef DEBUG
	php_cli_server_logf("%s Closing" TSRMLS_CC, client->addr_str);
#endif
	zend_hash_index_del(&server->clients, client->sock);
} /* }}} */

static int php_cli_server_send_error_page(php_cli_server *server, php_cli_server_client *client, int status TSRMLS_DC) /* {{{ */
{
	char *escaped_request_uri = NULL;
	size_t escaped_request_uri_len;
	const char *status_string = get_status_string(status);
	const char *content_template = get_template_string(status);
	char *errstr = get_last_error();
	assert(status_string && content_template);

	php_cli_server_content_sender_ctor(&client->content_sender);
	client->content_sender_initialized = 1;

	escaped_request_uri = php_escape_html_entities_ex((unsigned char *)client->request.request_uri, client->request.request_uri_len, &escaped_request_uri_len, 0, ENT_QUOTES, NULL, 0 TSRMLS_CC);

	{
		static const char prologue_template[] = "<!doctype html><html><head><title>%d %s</title>";
		php_cli_server_chunk *chunk = php_cli_server_chunk_heap_new_self_contained(strlen(prologue_template) + 3 + strlen(status_string) + 1);
		if (!chunk) {
			goto fail;
		}
		snprintf(chunk->data.heap.p, chunk->data.heap.len, prologue_template, status, status_string, escaped_request_uri);
		chunk->data.heap.len = strlen(chunk->data.heap.p);
		php_cli_server_buffer_append(&client->content_sender.buffer, chunk);
	}
	{
		php_cli_server_chunk *chunk = php_cli_server_chunk_immortal_new(php_cli_server_css, sizeof(php_cli_server_css) - 1);
		if (!chunk) {
			goto fail;
		}
		php_cli_server_buffer_append(&client->content_sender.buffer, chunk);
	}
	{
		static const char template[] = "</head><body>";
		php_cli_server_chunk *chunk = php_cli_server_chunk_immortal_new(template, sizeof(template) - 1);
		if (!chunk) {
			goto fail;
		}
		php_cli_server_buffer_append(&client->content_sender.buffer, chunk);
	}
	{
		php_cli_server_chunk *chunk = php_cli_server_chunk_heap_new_self_contained(strlen(content_template) + escaped_request_uri_len + 3 + strlen(status_string) + 1);
		if (!chunk) {
			goto fail;
		}
		snprintf(chunk->data.heap.p, chunk->data.heap.len, content_template, status_string, escaped_request_uri);
		chunk->data.heap.len = strlen(chunk->data.heap.p);
		php_cli_server_buffer_append(&client->content_sender.buffer, chunk);
	}
	{
		static const char epilogue_template[] = "</body></html>";
		php_cli_server_chunk *chunk = php_cli_server_chunk_immortal_new(epilogue_template, sizeof(epilogue_template) - 1);
		if (!chunk) {
			goto fail;
		}
		php_cli_server_buffer_append(&client->content_sender.buffer, chunk);
	}

	{
		php_cli_server_chunk *chunk;
		smart_str buffer = { 0 };
		append_http_status_line(&buffer, client->request.protocol_version, status, 1);
		if (!buffer.c) {
			/* out of memory */
			goto fail;
		}
		append_essential_headers(&buffer, client, 1);
		smart_str_appends_ex(&buffer, "Content-Type: text/html; charset=UTF-8\r\n", 1);
		smart_str_appends_ex(&buffer, "Content-Length: ", 1);
		smart_str_append_generic_ex(&buffer, php_cli_server_buffer_size(&client->content_sender.buffer), 1, size_t, _unsigned);
		smart_str_appendl_ex(&buffer, "\r\n", 2, 1);
		smart_str_appendl_ex(&buffer, "\r\n", 2, 1);

		chunk = php_cli_server_chunk_heap_new(buffer.c, buffer.c, buffer.len);
		if (!chunk) {
			smart_str_free_ex(&buffer, 1);
			goto fail;
		}
		php_cli_server_buffer_prepend(&client->content_sender.buffer, chunk);
	}

	php_cli_server_log_response(client, status, errstr ? errstr : "?" TSRMLS_CC);
	php_cli_server_poller_add(&server->poller, POLLOUT, client->sock);
	if (errstr) {
		pefree(errstr, 1);
	}
	efree(escaped_request_uri);
	return SUCCESS;

fail:
	if (errstr) {
		pefree(errstr, 1);
	}
	efree(escaped_request_uri);
	return FAILURE;
} /* }}} */

static int php_cli_server_dispatch_script(php_cli_server *server, php_cli_server_client *client TSRMLS_DC) /* {{{ */
{
	if (strlen(client->request.path_translated) != client->request.path_translated_len) {
		/* can't handle paths that contain nul bytes */
		return php_cli_server_send_error_page(server, client, 400 TSRMLS_CC);
	}
	{
		zend_file_handle zfd;
		zfd.type = ZEND_HANDLE_FILENAME;
		zfd.filename = SG(request_info).path_translated;
		zfd.handle.fp = NULL;
		zfd.free_filename = 0;
		zfd.opened_path = NULL;
		zend_try {
			php_execute_script(&zfd TSRMLS_CC);
		} zend_end_try();
	}

	php_cli_server_log_response(client, SG(sapi_headers).http_response_code, NULL TSRMLS_CC);
	return SUCCESS;
} /* }}} */

static int php_cli_server_begin_send_static(php_cli_server *server, php_cli_server_client *client TSRMLS_DC) /* {{{ */
{
	int fd;
	int status = 200;

	if (client->request.path_translated && strlen(client->request.path_translated) != client->request.path_translated_len) {
		/* can't handle paths that contain nul bytes */
		return php_cli_server_send_error_page(server, client, 400 TSRMLS_CC);
	}

#ifdef PHP_WIN32
	/* The win32 namespace will cut off trailing dots and spaces. Since the
	   VCWD functionality isn't used here, a sophisticated functionality
	   would have to be reimplemented to know ahead there are no files
	   with invalid names there. The simplest is just to forbid invalid
	   filenames, which is done here. */
	if (client->request.path_translated &&
		('.' == client->request.path_translated[client->request.path_translated_len-1] ||
		 ' ' == client->request.path_translated[client->request.path_translated_len-1])) {
		return php_cli_server_send_error_page(server, client, 500 TSRMLS_CC);
	}
#endif

	fd = client->request.path_translated ? open(client->request.path_translated, O_RDONLY): -1;
	if (fd < 0) {
		return php_cli_server_send_error_page(server, client, 404 TSRMLS_CC);
	}

	php_cli_server_content_sender_ctor(&client->content_sender);
	client->content_sender_initialized = 1;
	client->file_fd = fd;

	{
		php_cli_server_chunk *chunk;
		smart_str buffer = { 0 };
		const char *mime_type = get_mime_type(client->request.ext, client->request.ext_len);
		if (!mime_type) {
			mime_type = "application/octet-stream";
		}

		append_http_status_line(&buffer, client->request.protocol_version, status, 1);
		if (!buffer.c) {
			/* out of memory */
			php_cli_server_log_response(client, 500, NULL TSRMLS_CC);
			return FAILURE;
		}
		append_essential_headers(&buffer, client, 1);
		smart_str_appendl_ex(&buffer, "Content-Type: ", sizeof("Content-Type: ") - 1, 1);
		smart_str_appends_ex(&buffer, mime_type, 1);
		if (strncmp(mime_type, "text/", 5) == 0) {
			smart_str_appends_ex(&buffer, "; charset=UTF-8", 1);
		}
		smart_str_appendl_ex(&buffer, "\r\n", 2, 1);
		smart_str_appends_ex(&buffer, "Content-Length: ", 1);
		smart_str_append_generic_ex(&buffer, client->request.sb.st_size, 1, size_t, _unsigned);
		smart_str_appendl_ex(&buffer, "\r\n", 2, 1);
		smart_str_appendl_ex(&buffer, "\r\n", 2, 1);
		chunk = php_cli_server_chunk_heap_new(buffer.c, buffer.c, buffer.len);
		if (!chunk) {
			smart_str_free_ex(&buffer, 1);
			php_cli_server_log_response(client, 500, NULL TSRMLS_CC);
			return FAILURE;
		}
		php_cli_server_buffer_append(&client->content_sender.buffer, chunk);
	}
	php_cli_server_log_response(client, 200, NULL TSRMLS_CC);
	php_cli_server_poller_add(&server->poller, POLLOUT, client->sock);
	return SUCCESS;
}
/* }}} */

static int php_cli_server_request_startup(php_cli_server *server, php_cli_server_client *client TSRMLS_DC) { /* {{{ */
	char **auth;
	php_cli_server_client_populate_request_info(client, &SG(request_info));
	if (SUCCESS == zend_hash_find(&client->request.headers, "authorization", sizeof("authorization"), (void**)&auth)) {
		php_handle_auth_data(*auth TSRMLS_CC);
	}
	SG(sapi_headers).http_response_code = 200;
	if (FAILURE == php_request_startup(TSRMLS_C)) {
		/* should never be happen */
		destroy_request_info(&SG(request_info));
		return FAILURE;
	}
	PG(during_request_startup) = 0;

	return SUCCESS;
}
/* }}} */

static int php_cli_server_request_shutdown(php_cli_server *server, php_cli_server_client *client TSRMLS_DC) { /* {{{ */
	php_request_shutdown(0);
	php_cli_server_close_connection(server, client TSRMLS_CC);
	destroy_request_info(&SG(request_info));
	SG(server_context) = NULL;
	SG(rfc1867_uploaded_files) = NULL;
	return SUCCESS;
}
/* }}} */

static int php_cli_server_dispatch_router(php_cli_server *server, php_cli_server_client *client TSRMLS_DC) /* {{{ */
{
	int decline = 0;
	zend_file_handle zfd;
	char *old_cwd;

	ALLOCA_FLAG(use_heap)
	old_cwd = do_alloca(MAXPATHLEN, use_heap);
	old_cwd[0] = '\0';
	php_ignore_value(VCWD_GETCWD(old_cwd, MAXPATHLEN - 1));

	zfd.type = ZEND_HANDLE_FILENAME;
	zfd.filename = server->router;
	zfd.handle.fp = NULL;
	zfd.free_filename = 0;
	zfd.opened_path = NULL;

	zend_try {
		zval *retval = NULL;
		if (SUCCESS == zend_execute_scripts(ZEND_REQUIRE TSRMLS_CC, &retval, 1, &zfd)) {
			if (retval) {
				decline = Z_TYPE_P(retval) == IS_BOOL && !Z_LVAL_P(retval);
				zval_ptr_dtor(&retval);
			}
		} else {
			decline = 1;
		}
	} zend_end_try();

	if (old_cwd[0] != '\0') {
		php_ignore_value(VCWD_CHDIR(old_cwd));
	}

	free_alloca(old_cwd, use_heap);

	return decline;
}
/* }}} */

static int php_cli_server_dispatch(php_cli_server *server, php_cli_server_client *client TSRMLS_DC) /* {{{ */
{
	int is_static_file  = 0;

	SG(server_context) = client;
	if (client->request.ext_len != 3 || memcmp(client->request.ext, "php", 3) || !client->request.path_translated) {
		is_static_file = 1;
	}

	if (server->router || !is_static_file) {
		if (FAILURE == php_cli_server_request_startup(server, client TSRMLS_CC)) {
			SG(server_context) = NULL;
			php_cli_server_close_connection(server, client TSRMLS_CC);
			destroy_request_info(&SG(request_info));
			return SUCCESS;
		}
	}

	if (server->router) {
		if (!php_cli_server_dispatch_router(server, client TSRMLS_CC)) {
			php_cli_server_request_shutdown(server, client TSRMLS_CC);
			return SUCCESS;
		}
	}

	if (!is_static_file) {
		if (SUCCESS == php_cli_server_dispatch_script(server, client TSRMLS_CC)
				|| SUCCESS != php_cli_server_send_error_page(server, client, 500 TSRMLS_CC)) {
			if (SG(sapi_headers).http_response_code == 304) {
				SG(sapi_headers).send_default_content_type = 0;
			}
			php_cli_server_request_shutdown(server, client TSRMLS_CC);
			return SUCCESS;
		}
	} else {
		if (server->router) {
			static int (*send_header_func)(sapi_headers_struct * TSRMLS_DC);
			send_header_func = sapi_module.send_headers;
			/* do not generate default content type header */
		    SG(sapi_headers).send_default_content_type = 0;
			/* we don't want headers to be sent */
			sapi_module.send_headers = sapi_cli_server_discard_headers;
			php_request_shutdown(0);
			sapi_module.send_headers = send_header_func;
		    SG(sapi_headers).send_default_content_type = 1;
			SG(rfc1867_uploaded_files) = NULL;
		}
		if (SUCCESS != php_cli_server_begin_send_static(server, client TSRMLS_CC)) {
			php_cli_server_close_connection(server, client TSRMLS_CC);
		}
		SG(server_context) = NULL;
		return SUCCESS;
	}

	SG(server_context) = NULL;
	destroy_request_info(&SG(request_info));
	return SUCCESS;
}
/* }}} */

static void php_cli_server_dtor(php_cli_server *server TSRMLS_DC) /* {{{ */
{
	zend_hash_destroy(&server->clients);
	if (server->server_sock >= 0) {
		closesocket(server->server_sock);
	}
	if (server->host) {
		pefree(server->host, 1);
	}
	if (server->document_root) {
		pefree(server->document_root, 1);
	}
	if (server->router) {
		pefree(server->router, 1);
	}
} /* }}} */

static void php_cli_server_client_dtor_wrapper(php_cli_server_client **p) /* {{{ */
{
	closesocket((*p)->sock);
	php_cli_server_poller_remove(&(*p)->server->poller, POLLIN | POLLOUT, (*p)->sock);
	php_cli_server_client_dtor(*p);
	pefree(*p, 1);
} /* }}} */

static int php_cli_server_ctor(php_cli_server *server, const char *addr, const char *document_root, const char *router TSRMLS_DC) /* {{{ */
{
	int retval = SUCCESS;
	char *host = NULL;
	char *errstr = NULL;
	char *_document_root = NULL;
	char *_router = NULL;
	int err = 0;
	int port = 3000;
	php_socket_t server_sock = SOCK_ERR;
	char *p = NULL;

	if (addr[0] == '[') {
		host = pestrdup(addr + 1, 1);
		if (!host) {
			return FAILURE;
		}
		p = strchr(host, ']');
		if (p) {
			*p++ = '\0';
			if (*p == ':') {
				port = strtol(p + 1, &p, 10);
				if (port <= 0 || port > 65535) {
					p = NULL;
				}
			} else if (*p != '\0') {
				p = NULL;
			}
		}
	} else {
		host = pestrdup(addr, 1);
		if (!host) {
			return FAILURE;
		}
		p = strchr(host, ':');
		if (p) {
			*p++ = '\0';
			port = strtol(p, &p, 10);
			if (port <= 0 || port > 65535) {
				p = NULL;
			}
		}
	}
	if (!p) {
		fprintf(stderr, "Invalid address: %s\n", addr);
		retval = FAILURE;
		goto out;
	}

	server_sock = php_network_listen_socket(host, &port, SOCK_STREAM, &server->address_family, &server->socklen, &errstr TSRMLS_CC);
	if (server_sock == SOCK_ERR) {
		php_cli_server_logf("Failed to listen on %s:%d (reason: %s)" TSRMLS_CC, host, port, errstr ? errstr: "?");
		efree(errstr);
		retval = FAILURE;
		goto out;
	}
	server->server_sock = server_sock;

	err = php_cli_server_poller_ctor(&server->poller);
	if (SUCCESS != err) {
		goto out;
	}

	php_cli_server_poller_add(&server->poller, POLLIN, server_sock);

	server->host = host;
	server->port = port;

	zend_hash_init(&server->clients, 0, NULL, (void(*)(void*))php_cli_server_client_dtor_wrapper, 1);

	{
		size_t document_root_len = strlen(document_root);
		_document_root = pestrndup(document_root, document_root_len, 1);
		if (!_document_root) {
			retval = FAILURE;
			goto out;
		}
		server->document_root = _document_root;
		server->document_root_len = document_root_len;
	}

	if (router) {
		size_t router_len = strlen(router);
		_router = pestrndup(router, router_len, 1);
		if (!_router) {
			retval = FAILURE;
			goto out;
		}
		server->router = _router;
		server->router_len = router_len;
	} else {
		server->router = NULL;
		server->router_len = 0;
	}

	server->is_running = 1;
out:
	if (retval != SUCCESS) {
		if (host) {
			pefree(host, 1);
		}
		if (_document_root) {
			pefree(_document_root, 1);
		}
		if (_router) {
			pefree(_router, 1);
		}
		if (server_sock > -1) {
			closesocket(server_sock);
		}
	}
	return retval;
} /* }}} */

static int php_cli_server_recv_event_read_request(php_cli_server *server, php_cli_server_client *client TSRMLS_DC) /* {{{ */
{
	char *errstr = NULL;
	int status = php_cli_server_client_read_request(client, &errstr TSRMLS_CC);
	if (status < 0) {
		php_cli_server_logf("%s Invalid request (%s)" TSRMLS_CC, client->addr_str, errstr);
		efree(errstr);
		php_cli_server_close_connection(server, client TSRMLS_CC);
		return FAILURE;
	} else if (status == 1 && client->request.request_method == PHP_HTTP_NOT_IMPLEMENTED) {
		return php_cli_server_send_error_page(server, client, 501 TSRMLS_CC);
	} else if (status == 1) {
		php_cli_server_poller_remove(&server->poller, POLLIN, client->sock);
		php_cli_server_dispatch(server, client TSRMLS_CC);
	} else {
		php_cli_server_poller_add(&server->poller, POLLIN, client->sock);
	}

	return SUCCESS;
} /* }}} */

static int php_cli_server_send_event(php_cli_server *server, php_cli_server_client *client TSRMLS_DC) /* {{{ */
{
	if (client->content_sender_initialized) {
		if (client->file_fd >= 0 && !client->content_sender.buffer.first) {
			size_t nbytes_read;
			if (php_cli_server_content_sender_pull(&client->content_sender, client->file_fd, &nbytes_read)) {
				php_cli_server_close_connection(server, client TSRMLS_CC);
				return FAILURE;
			}
			if (nbytes_read == 0) {
				close(client->file_fd);
				client->file_fd = -1;
			}
		}
		{
			size_t nbytes_sent;
			int err = php_cli_server_content_sender_send(&client->content_sender, client->sock, &nbytes_sent);
			if (err && err != SOCK_EAGAIN) {
				php_cli_server_close_connection(server, client TSRMLS_CC);
				return FAILURE;
			}
		}
		if (!client->content_sender.buffer.first && client->file_fd < 0) {
			php_cli_server_close_connection(server, client TSRMLS_CC);
		}
	}
	return SUCCESS;
}
/* }}} */

typedef struct php_cli_server_do_event_for_each_fd_callback_params {
#ifdef ZTS
	void ***tsrm_ls;
#endif
	php_cli_server *server;
	int(*rhandler)(php_cli_server*, php_cli_server_client* TSRMLS_DC);
	int(*whandler)(php_cli_server*, php_cli_server_client* TSRMLS_DC);
} php_cli_server_do_event_for_each_fd_callback_params;

static int php_cli_server_do_event_for_each_fd_callback(void *_params, int fd, int event) /* {{{ */
{
	php_cli_server_do_event_for_each_fd_callback_params *params = _params;
#ifdef ZTS
	void ***tsrm_ls = params->tsrm_ls;
#endif
	php_cli_server *server = params->server;
	if (server->server_sock == fd) {
		php_cli_server_client *client = NULL;
		php_socket_t client_sock;
		socklen_t socklen = server->socklen;
		struct sockaddr *sa = pemalloc(server->socklen, 1);
		if (!sa) {
			return FAILURE;
		}
		client_sock = accept(server->server_sock, sa, &socklen);
		if (client_sock < 0) {
			char *errstr;
			errstr = php_socket_strerror(php_socket_errno(), NULL, 0);
			php_cli_server_logf("Failed to accept a client (reason: %s)" TSRMLS_CC, errstr);
			efree(errstr);
			pefree(sa, 1);
			return SUCCESS;
		}
		if (SUCCESS != php_set_sock_blocking(client_sock, 0 TSRMLS_CC)) {
			pefree(sa, 1);
			closesocket(client_sock);
			return SUCCESS;
		}
		if (!(client = pemalloc(sizeof(php_cli_server_client), 1)) || FAILURE == php_cli_server_client_ctor(client, server, client_sock, sa, socklen TSRMLS_CC)) {
			php_cli_server_logf("Failed to create a new request object" TSRMLS_CC);
			pefree(sa, 1);
			closesocket(client_sock);
			return SUCCESS;
		}
#ifdef DEBUG
		php_cli_server_logf("%s Accepted" TSRMLS_CC, client->addr_str);
#endif
		zend_hash_index_update(&server->clients, client_sock, &client, sizeof(client), NULL);
		php_cli_server_recv_event_read_request(server, client TSRMLS_CC);
	} else {
		php_cli_server_client **client;
		if (SUCCESS == zend_hash_index_find(&server->clients, fd, (void **)&client)) {
			if (event & POLLIN) {
				params->rhandler(server, *client TSRMLS_CC);
			}
			if (event & POLLOUT) {
				params->whandler(server, *client TSRMLS_CC);
			}
		}
	}
	return SUCCESS;
} /* }}} */

static void php_cli_server_do_event_for_each_fd(php_cli_server *server, int(*rhandler)(php_cli_server*, php_cli_server_client* TSRMLS_DC), int(*whandler)(php_cli_server*, php_cli_server_client* TSRMLS_DC) TSRMLS_DC) /* {{{ */
{
	php_cli_server_do_event_for_each_fd_callback_params params = {
#ifdef ZTS
		tsrm_ls,
#endif
		server,
		rhandler,
		whandler
	};

	php_cli_server_poller_iter_on_active(&server->poller, &params, php_cli_server_do_event_for_each_fd_callback);
} /* }}} */

static int php_cli_server_do_event_loop(php_cli_server *server TSRMLS_DC) /* {{{ */
{
	int retval = SUCCESS;
	while (server->is_running) {
		struct timeval tv = { 1, 0 };
		int n = php_cli_server_poller_poll(&server->poller, &tv);
		if (n > 0) {
			php_cli_server_do_event_for_each_fd(server,
					php_cli_server_recv_event_read_request,
					php_cli_server_send_event TSRMLS_CC);
		} else if (n == 0) {
			/* do nothing */
		} else {
			int err = php_socket_errno();
			if (err != SOCK_EINTR) {
				char *errstr = php_socket_strerror(err, NULL, 0);
				php_cli_server_logf("%s" TSRMLS_CC, errstr);
				efree(errstr);
				retval = FAILURE;
				goto out;
			}
		}
	}
out:
	return retval;
} /* }}} */

static php_cli_server server;

static void php_cli_server_sigint_handler(int sig) /* {{{ */
{
	server.is_running = 0;
}
/* }}} */

int do_cli_server(int argc, char **argv TSRMLS_DC) /* {{{ */
{
	char *php_optarg = NULL;
	int php_optind = 1;
	int c;
	const char *server_bind_address = NULL;
	extern const opt_struct OPTIONS[];
	const char *document_root = NULL;
	const char *router = NULL;
	char document_root_buf[MAXPATHLEN];

	while ((c = php_getopt(argc, argv, OPTIONS, &php_optarg, &php_optind, 0, 2))!=-1) {
		switch (c) {
			case 'S':
				server_bind_address = php_optarg;
				break;
			case 't':
				document_root = php_optarg;
				break;
		}
	}

	if (document_root) {
		struct stat sb;

		if (stat(document_root, &sb)) {
			fprintf(stderr, "Directory %s does not exist.\n", document_root);
			return 1;
		}
		if (!S_ISDIR(sb.st_mode)) {
			fprintf(stderr, "%s is not a directory.\n", document_root);
			return 1;
		}
		if (VCWD_REALPATH(document_root, document_root_buf)) {
			document_root = document_root_buf;
		}
	} else {
		char *ret = NULL;

#if HAVE_GETCWD
		ret = VCWD_GETCWD(document_root_buf, MAXPATHLEN);
#elif HAVE_GETWD
		ret = VCWD_GETWD(document_root_buf);
#endif
		document_root = ret ? document_root_buf: ".";
	}

	if (argc > php_optind) {
		router = argv[php_optind];
	}

	if (FAILURE == php_cli_server_ctor(&server, server_bind_address, document_root, router TSRMLS_CC)) {
		return 1;
	}
	sapi_module.phpinfo_as_text = 0;

	{
		char buf[52];

		if (php_cli_server_get_system_time(buf) != 0) {
			memmove(buf, "unknown time, can't be fetched", sizeof("unknown time, can't be fetched"));
		}

		printf("PHP %s Development Server started at %s"
				"Listening on http://%s\n"
				"Document root is %s\n"
				"Press Ctrl-C to quit.\n",
				PHP_VERSION, buf, server_bind_address, document_root);
	}

#if defined(HAVE_SIGNAL_H) && defined(SIGINT)
	signal(SIGINT, php_cli_server_sigint_handler);
#endif
	php_cli_server_do_event_loop(&server TSRMLS_CC);
	php_cli_server_dtor(&server TSRMLS_CC);
	return 0;
} /* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
