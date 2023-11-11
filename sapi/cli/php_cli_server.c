/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Moriyoshi Koizumi <moriyoshi@php.net>                        |
   |         Xinchen Hui       <laruence@php.net>                         |
   +----------------------------------------------------------------------+
*/

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

#if HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <signal.h>
#include <locale.h>

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
#include "http_status_codes.h"

#include "zend_compile.h"
#include "zend_execute.h"
#include "zend_highlight.h"
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

#include "ext/standard/file.h" /* for php_set_sock_blocking() :-( */
#include "zend_smart_str.h"
#include "ext/standard/html.h"
#include "ext/standard/url.h" /* for php_raw_url_decode() */
#include "ext/standard/php_string.h" /* for php_dirname() */
#include "ext/date/php_date.h" /* for php_format_date() */
#include "php_network.h"

#include "php_http_parser.h"
#include "php_cli_server.h"
#include "php_cli_server_arginfo.h"
#include "mime_type_map.h"

#include "php_cli_process_title.h"
#include "php_cli_process_title_arginfo.h"

#define OUTPUT_NOT_CHECKED -1
#define OUTPUT_IS_TTY 1
#define OUTPUT_NOT_TTY 0

#if HAVE_FORK
# include <sys/wait.h>
static pid_t	 php_cli_server_master;
static pid_t	*php_cli_server_workers;
static zend_long php_cli_server_workers_max;
#endif

static zend_string* cli_concat_persistent_zstr_with_char(zend_string *old_str, const char *at, size_t length);

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
	zend_string *request_uri;
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
	zend_stat_t sb;
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
	zend_string *addr_str;
	php_http_parser parser;
	bool request_read;
	zend_string *current_header_name;
	zend_string *current_header_value;
	enum { HEADER_NONE=0, HEADER_FIELD, HEADER_VALUE } last_header_element;
	size_t post_read_offset;
	php_cli_server_request request;
	bool content_sender_initialized;
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
	HashTable extension_mime_types;
} php_cli_server;

typedef struct php_cli_server_http_response_status_code_pair {
	int code;
	const char *str;
} php_cli_server_http_response_status_code_pair;

static php_cli_server_http_response_status_code_pair template_map[] = {
	{ 400, "<h1>%s</h1><p>Your browser sent a request that this server could not understand.</p>" },
	{ 404, "<h1>%s</h1><p>The requested resource <code class=\"url\">%s</code> was not found on this server.</p>" },
	{ 405, "<h1>%s</h1><p>Requested method not allowed.</p>" },
	{ 500, "<h1>%s</h1><p>The server is temporarily unavailable.</p>" },
	{ 501, "<h1>%s</h1><p>Request method not supported.</p>" }
};

#define PHP_CLI_SERVER_LOG_PROCESS 1
#define PHP_CLI_SERVER_LOG_ERROR   2
#define PHP_CLI_SERVER_LOG_MESSAGE 3

static int php_cli_server_log_level = 3;

#if HAVE_UNISTD_H || defined(PHP_WIN32)
static int php_cli_output_is_tty = OUTPUT_NOT_CHECKED;
#endif

static const char php_cli_server_request_error_unexpected_eof[] = "Unexpected EOF";

static size_t php_cli_server_client_send_through(php_cli_server_client *client, const char *str, size_t str_len);
static php_cli_server_chunk *php_cli_server_chunk_heap_new_self_contained(size_t len);
static void php_cli_server_buffer_append(php_cli_server_buffer *buffer, php_cli_server_chunk *chunk);
static void php_cli_server_logf(int type, const char *format, ...);
static void php_cli_server_log_response(php_cli_server_client *client, int status, const char *message);

ZEND_DECLARE_MODULE_GLOBALS(cli_server)

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
static bool php_cli_server_get_system_time(char *buf) {
	struct _timeb system_time;
	errno_t err;

	if (buf == NULL) {
		return false;
	}

	_ftime(&system_time);
	err = ctime_s(buf, 52, &(system_time.time) );
	if (err) {
		return false;
	}
	return true;
}
#else
static bool php_cli_server_get_system_time(char *buf) {
	struct timeval tv;
	struct tm tm;

	gettimeofday(&tv, NULL);

	/* TODO: should be checked for NULL tm/return value */
	php_localtime_r(&tv.tv_sec, &tm);
	php_asctime_r(&tm, buf);
	return true;
}
#endif

/* Destructor for php_cli_server_request->headers, this frees header value */
static void cli_header_value_dtor(zval *zv) /* {{{ */
{
	zend_string_release_ex(Z_STR_P(zv), /* persistent */ true);
} /* }}} */

static char *get_last_error(void) /* {{{ */
{
	return pestrdup(strerror(errno), 1);
} /* }}} */

static int status_comp(const void *a, const void *b) /* {{{ */
{
	const http_response_status_code_pair *pa = (const http_response_status_code_pair *) a;
	const http_response_status_code_pair *pb = (const http_response_status_code_pair *) b;

	if (pa->code < pb->code) {
		return -1;
	} else if (pa->code > pb->code) {
		return 1;
	}

	return 0;
} /* }}} */

static const char *get_status_string(int code) /* {{{ */
{
	http_response_status_code_pair needle = {code, NULL},
		*result = NULL;

	result = bsearch(&needle, http_status_map, http_status_map_len, sizeof(needle), status_comp);

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

static void append_http_status_line(smart_str *buffer, int protocol_version, int response_code, bool persistent) /* {{{ */
{
	if (!response_code) {
		response_code = 200;
	}
	smart_str_appendl_ex(buffer, "HTTP", 4, persistent);
	smart_str_appendc_ex(buffer, '/', persistent);
	smart_str_append_long_ex(buffer, protocol_version / 100, persistent);
	smart_str_appendc_ex(buffer, '.', persistent);
	smart_str_append_long_ex(buffer, protocol_version % 100, persistent);
	smart_str_appendc_ex(buffer, ' ', persistent);
	smart_str_append_long_ex(buffer, response_code, persistent);
	smart_str_appendc_ex(buffer, ' ', persistent);
	smart_str_appends_ex(buffer, get_status_string(response_code), persistent);
	smart_str_appendl_ex(buffer, "\r\n", 2, persistent);
} /* }}} */

static void append_essential_headers(smart_str* buffer, php_cli_server_client *client, bool persistent, sapi_headers_struct *sapi_headers) /* {{{ */
{
	zval *val;
	struct timeval tv = {0};
	bool append_date_header = true;

	if (sapi_headers != NULL) {
		zend_llist_position pos;
		sapi_header_struct *h = (sapi_header_struct*)zend_llist_get_first_ex(&sapi_headers->headers, &pos);
		while (h) {
			if (h->header_len > strlen("Date:")) {
				if (strncasecmp(h->header, "Date:", strlen("Date:")) == 0) {
					append_date_header = false;
					break;
				}
			}
			h = (sapi_header_struct*)zend_llist_get_next_ex(&sapi_headers->headers, &pos);
		}
	}

	if (NULL != (val = zend_hash_str_find(&client->request.headers, "host", sizeof("host")-1))) {
		smart_str_appends_ex(buffer, "Host: ", persistent);
		smart_str_append_ex(buffer, Z_STR_P(val), persistent);
		smart_str_appends_ex(buffer, "\r\n", persistent);
	}

	if (append_date_header && !gettimeofday(&tv, NULL)) {
		zend_string *dt = php_format_date("D, d M Y H:i:s", sizeof("D, d M Y H:i:s") - 1, tv.tv_sec, 0);
		smart_str_appends_ex(buffer, "Date: ", persistent);
		smart_str_append_ex(buffer, dt, persistent);
		smart_str_appends_ex(buffer, " GMT\r\n", persistent);
		zend_string_release_ex(dt, 0);
	}

	smart_str_appendl_ex(buffer, "Connection: close\r\n", sizeof("Connection: close\r\n") - 1, persistent);
} /* }}} */

static const char *get_mime_type(const php_cli_server *server, const char *ext, size_t ext_len) /* {{{ */
{
	char *ret;
	ALLOCA_FLAG(use_heap)
	char *ext_lower = do_alloca(ext_len + 1, use_heap);
	zend_str_tolower_copy(ext_lower, ext, ext_len);
	ret = zend_hash_str_find_ptr(&server->extension_mime_types, ext_lower, ext_len);
	free_alloca(ext_lower, use_heap);
	return (const char*)ret;
} /* }}} */

PHP_FUNCTION(apache_request_headers) /* {{{ */
{
	php_cli_server_client *client;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	client = SG(server_context);

	/* Need to duplicate the header HashTable */
	RETURN_ARR(zend_array_dup(&client->request.headers_original_case));
}
/* }}} */

static void add_response_header(sapi_header_struct *h, zval *return_value) /* {{{ */
{
	char *s, *p;
	ptrdiff_t  len;
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
				add_assoc_stringl_ex(return_value, s, (uint32_t)len, p, h->header_len - (p - h->header));
				free_alloca(s, use_heap);
			}
		}
	}
}
/* }}} */

PHP_FUNCTION(apache_response_headers) /* {{{ */
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	array_init(return_value);
	zend_llist_apply_with_argument(&SG(sapi_headers).headers, (llist_apply_with_arg_func_t)add_response_header, return_value);
}
/* }}} */

/* {{{ cli_server module */

static void cli_server_init_globals(zend_cli_server_globals *cg)
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

const zend_function_entry server_additional_functions[] = {
	PHP_FE(cli_set_process_title,	arginfo_cli_set_process_title)
	PHP_FE(cli_get_process_title,	arginfo_cli_get_process_title)
	PHP_FE(apache_request_headers,	arginfo_apache_request_headers)
	PHP_FE(apache_response_headers,	arginfo_apache_response_headers)
	PHP_FALIAS(getallheaders, apache_request_headers, arginfo_getallheaders)
	PHP_FE_END
};

static int sapi_cli_server_startup(sapi_module_struct *sapi_module) /* {{{ */
{
	return php_module_startup(sapi_module, &cli_server_module_entry);
} /* }}} */

static size_t sapi_cli_server_ub_write(const char *str, size_t str_length) /* {{{ */
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

	if (!client) {
		return;
	}

	if (!ZEND_VALID_SOCKET(client->sock)) {
		php_handle_aborted_connection();
		return;
	}

	if (!SG(headers_sent)) {
		sapi_send_headers();
		SG(headers_sent) = 1;
	}
} /* }}} */

static int sapi_cli_server_discard_headers(sapi_headers_struct *sapi_headers) /* {{{ */{
	return SAPI_HEADER_SENT_SUCCESSFULLY;
}
/* }}} */

static int sapi_cli_server_send_headers(sapi_headers_struct *sapi_headers) /* {{{ */
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

	append_essential_headers(&buffer, client, 0, sapi_headers);

	h = (sapi_header_struct*)zend_llist_get_first_ex(&sapi_headers->headers, &pos);
	while (h) {
		if (h->header_len) {
			smart_str_appendl(&buffer, h->header, h->header_len);
			smart_str_appendl(&buffer, "\r\n", 2);
		}
		h = (sapi_header_struct*)zend_llist_get_next_ex(&sapi_headers->headers, &pos);
	}
	smart_str_appendl(&buffer, "\r\n", 2);

	php_cli_server_client_send_through(client, ZSTR_VAL(buffer.s), ZSTR_LEN(buffer.s));

	smart_str_free(&buffer);
	return SAPI_HEADER_SENT_SUCCESSFULLY;
}
/* }}} */

static char *sapi_cli_server_read_cookies(void) /* {{{ */
{
	php_cli_server_client *client = SG(server_context);
	zval *val;
	if (NULL == (val = zend_hash_str_find(&client->request.headers, "cookie", sizeof("cookie")-1))) {
		return NULL;
	}
	return Z_STRVAL_P(val);
} /* }}} */

static size_t sapi_cli_server_read_post(char *buf, size_t count_bytes) /* {{{ */
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

static void sapi_cli_server_register_variable(zval *track_vars_array, const char *key, const char *val) /* {{{ */
{
	char *new_val = (char *)val;
	size_t new_val_len;

	if (NULL == val) {
		return;
	}

	if (sapi_module.input_filter(PARSE_SERVER, (char*)key, &new_val, strlen(val), &new_val_len)) {
		php_register_variable_safe((char *)key, new_val, new_val_len, track_vars_array);
	}
} /* }}} */

static void sapi_cli_server_register_known_var_char(zval *track_vars_array,
	const char *var_name, size_t var_name_len, const char *value, size_t value_len)
{
	zval new_entry;

	if (!value) {
		return;
	}

	ZVAL_STRINGL_FAST(&new_entry, value, value_len);

	php_register_known_variable(var_name, var_name_len, &new_entry, track_vars_array);
}

static void sapi_cli_server_register_known_var_str(zval *track_vars_array,
	const char *var_name, size_t var_name_len, /* const */ zend_string *value)
{
	zval new_entry;

	if (!value) {
		return;
	}

	ZVAL_STR_COPY(&new_entry, value);

	php_register_known_variable(var_name, var_name_len, &new_entry, track_vars_array);
}

/* The entry zval will always contain a zend_string* */
static int sapi_cli_server_register_entry_cb(zval *entry, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */ {
	zval *track_vars_array = va_arg(args, zval *);

	ZEND_ASSERT(Z_TYPE_P(entry) == IS_STRING);

	if (hash_key->key) {
		char *real_key, *key;
		uint32_t i;
		key = estrndup(ZSTR_VAL(hash_key->key), ZSTR_LEN(hash_key->key));
		for(i=0; i<ZSTR_LEN(hash_key->key); i++) {
			if (key[i] == '-') {
				key[i] = '_';
			} else {
				key[i] = toupper(key[i]);
			}
		}
		spprintf(&real_key, 0, "%s_%s", "HTTP", key);
		if (strcmp(key, "CONTENT_TYPE") == 0 || strcmp(key, "CONTENT_LENGTH") == 0) {
			/* Is it possible to use sapi_cli_server_register_known_var_char() and not go through the SAPI filter? */
			sapi_cli_server_register_variable(track_vars_array, key, Z_STRVAL_P(entry));
		}
		sapi_cli_server_register_variable(track_vars_array, real_key, Z_STRVAL_P(entry));
		efree(key);
		efree(real_key);
	}

	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

static void sapi_cli_server_register_variables(zval *track_vars_array) /* {{{ */
{
	php_cli_server_client *client = SG(server_context);

	sapi_cli_server_register_known_var_char(track_vars_array,
		"DOCUMENT_ROOT", strlen("DOCUMENT_ROOT"), client->server->document_root, client->server->document_root_len);
	{
		char *tmp;
		if ((tmp = strrchr(ZSTR_VAL(client->addr_str), ':'))) {
			char addr[64], port[8];
			const char *addr_start = ZSTR_VAL(client->addr_str), *addr_end = tmp;
			if (addr_start[0] == '[') addr_start++;
			if (addr_end[-1] == ']') addr_end--;

			strncpy(port, tmp + 1, 8);
			port[7] = '\0';
			size_t addr_len = addr_end - addr_start;
			strncpy(addr, addr_start, addr_len);
			addr[addr_len] = '\0';
			ZEND_ASSERT(addr_len == strlen(addr));
			sapi_cli_server_register_known_var_char(track_vars_array,
				"REMOTE_ADDR", strlen("REMOTE_ADDR"), addr, addr_len);
			sapi_cli_server_register_known_var_char(track_vars_array,
				"REMOTE_PORT", strlen("REMOTE_PORT"), port, strlen(port));
		} else {
			sapi_cli_server_register_known_var_str(track_vars_array,
				"REMOTE_ADDR", strlen("REMOTE_ADDR"), client->addr_str);
		}
	}
	{
		zend_string *tmp = strpprintf(0, "PHP %s Development Server", PHP_VERSION);
		sapi_cli_server_register_known_var_str(track_vars_array, "SERVER_SOFTWARE", strlen("SERVER_SOFTWARE"), tmp);
		zend_string_release_ex(tmp, /* persistent */ false);
	}
	{
		zend_string *tmp = strpprintf(0, "HTTP/%d.%d", client->request.protocol_version / 100, client->request.protocol_version % 100);
		sapi_cli_server_register_known_var_str(track_vars_array, "SERVER_PROTOCOL", strlen("SERVER_PROTOCOL"), tmp);
		zend_string_release_ex(tmp, /* persistent */ false);
	}
	sapi_cli_server_register_known_var_char(track_vars_array,
		"SERVER_NAME", strlen("SERVER_NAME"), client->server->host, strlen(client->server->host));
	{
		zend_string *tmp = strpprintf(0, "%i",  client->server->port);
		sapi_cli_server_register_known_var_str(track_vars_array, "SERVER_PORT", strlen("SERVER_PORT"), tmp);
		zend_string_release_ex(tmp, /* persistent */ false);
	}

	sapi_cli_server_register_known_var_str(track_vars_array,
		"REQUEST_URI", strlen("REQUEST_URI"), client->request.request_uri);
	sapi_cli_server_register_known_var_char(track_vars_array,
		"REQUEST_METHOD", strlen("REQUEST_METHOD"),
		SG(request_info).request_method, strlen(SG(request_info).request_method));
	sapi_cli_server_register_known_var_char(track_vars_array,
		"SCRIPT_NAME", strlen("SCRIPT_NAME"), client->request.vpath, client->request.vpath_len);
	if (SG(request_info).path_translated) {
		sapi_cli_server_register_known_var_char(track_vars_array,
			"SCRIPT_FILENAME", strlen("SCRIPT_FILENAME"),
			SG(request_info).path_translated, strlen(SG(request_info).path_translated));
	} else if (client->server->router) {
		sapi_cli_server_register_known_var_char(track_vars_array,
			"SCRIPT_FILENAME", strlen("SCRIPT_FILENAME"), client->server->router, client->server->router_len);
	}
	if (client->request.path_info) {
		sapi_cli_server_register_known_var_char(track_vars_array,
			"PATH_INFO", strlen("PATH_INFO"), client->request.path_info, client->request.path_info_len);
	}
	if (client->request.path_info_len) {
		zend_string *tmp = strpprintf(0, "%s%s", client->request.vpath, client->request.path_info);
		sapi_cli_server_register_known_var_str(track_vars_array, "PHP_SELF", strlen("PHP_SELF"), tmp);
		zend_string_release_ex(tmp, /* persistent */ false);
	} else {
		sapi_cli_server_register_known_var_char(track_vars_array,
			"PHP_SELF", strlen("PHP_SELF"), client->request.vpath, client->request.vpath_len);
	}
	if (client->request.query_string) {
		/* Use sapi_cli_server_register_variable() to pass query string through SAPI input filter,
		 * and check keys are proper PHP var names */
		sapi_cli_server_register_variable(track_vars_array, "QUERY_STRING", client->request.query_string);
	}
	/* Use sapi_cli_server_register_variable() to pass header values through SAPI input filter,
	 * and check keys are proper PHP var names */
	zend_hash_apply_with_arguments(&client->request.headers, (apply_func_args_t)sapi_cli_server_register_entry_cb, 1, track_vars_array);
} /* }}} */

static void sapi_cli_server_log_write(int type, const char *msg) /* {{{ */
{
	char buf[52];

	if (php_cli_server_log_level < type) {
		return;
	}

	if (!php_cli_server_get_system_time(buf)) {
		memmove(buf, "unknown time, can't be fetched", sizeof("unknown time, can't be fetched"));
	} else {
		size_t l = strlen(buf);
		if (l > 0) {
			buf[l - 1] = '\0';
		} else {
			memmove(buf, "unknown", sizeof("unknown"));
		}
	}
#ifdef HAVE_FORK
	if (php_cli_server_workers_max > 1) {
		fprintf(stderr, "[%ld] [%s] %s\n", (long) getpid(), buf, msg);
	} else {
		fprintf(stderr, "[%s] %s\n", buf, msg);
	}
#else
	fprintf(stderr, "[%s] %s\n", buf, msg);
#endif
} /* }}} */

static void sapi_cli_server_log_message(const char *msg, int syslog_type_int) /* {{{ */
{
	sapi_cli_server_log_write(PHP_CLI_SERVER_LOG_MESSAGE, msg);
} /* }}} */

/* {{{ sapi_module_struct cli_server_sapi_module */
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

static void php_cli_server_poller_ctor(php_cli_server_poller *poller) /* {{{ */
{
	FD_ZERO(&poller->rfds);
	FD_ZERO(&poller->wfds);
	poller->max_fd = -1;
} /* }}} */

static void php_cli_server_poller_add(php_cli_server_poller *poller, int mode, php_socket_t fd) /* {{{ */
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

static void php_cli_server_poller_remove(php_cli_server_poller *poller, int mode, php_socket_t fd) /* {{{ */
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

// TODO Return value is unused, refactor?
static zend_result php_cli_server_poller_iter_on_active(php_cli_server_poller *poller, void *opaque, zend_result(*callback)(void *, php_socket_t fd, int events)) /* {{{ */
{
	zend_result retval = SUCCESS;
#ifdef PHP_WIN32
	struct socket_entry {
		SOCKET fd;
		int events;
	} entries[FD_SETSIZE * 2];
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

	chunk->type = PHP_CLI_SERVER_CHUNK_IMMORTAL;
	chunk->next = NULL;
	chunk->data.immortal.p = buf;
	chunk->data.immortal.len = len;
	return chunk;
} /* }}} */

static php_cli_server_chunk *php_cli_server_chunk_heap_new(void *block, char *buf, size_t len) /* {{{ */
{
	php_cli_server_chunk *chunk = pemalloc(sizeof(php_cli_server_chunk), 1);

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
#ifdef PHP_WIN32
		int nbytes_sent;
#else
		ssize_t nbytes_sent;
#endif
		next = chunk->next;

		switch (chunk->type) {
		case PHP_CLI_SERVER_CHUNK_HEAP:
#ifdef PHP_WIN32
			nbytes_sent = send(fd, chunk->data.heap.p, (int)chunk->data.heap.len, 0);
#else
			nbytes_sent = send(fd, chunk->data.heap.p, chunk->data.heap.len, 0);
#endif
			if (nbytes_sent < 0) {
				*nbytes_sent_total = _nbytes_sent_total;
				return php_socket_errno();
#ifdef PHP_WIN32
			} else if (nbytes_sent == chunk->data.heap.len) {
#else
			} else if (nbytes_sent == (ssize_t)chunk->data.heap.len) {
#endif
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
#ifdef PHP_WIN32
			nbytes_sent = send(fd, chunk->data.immortal.p, (int)chunk->data.immortal.len, 0);
#else
			nbytes_sent = send(fd, chunk->data.immortal.p, chunk->data.immortal.len, 0);
#endif
			if (nbytes_sent < 0) {
				*nbytes_sent_total = _nbytes_sent_total;
				return php_socket_errno();
#ifdef PHP_WIN32
			} else if (nbytes_sent == chunk->data.immortal.len) {
#else
			} else if (nbytes_sent == (ssize_t)chunk->data.immortal.len) {
#endif
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

static bool php_cli_server_content_sender_pull(php_cli_server_content_sender *sender, int fd, size_t *nbytes_read) /* {{{ */
{
#ifdef PHP_WIN32
	int _nbytes_read;
#else
	ssize_t _nbytes_read;
#endif
	php_cli_server_chunk *chunk = php_cli_server_chunk_heap_new_self_contained(131072);

#ifdef PHP_WIN32
	_nbytes_read = read(fd, chunk->data.heap.p, (unsigned int)chunk->data.heap.len);
#else
	_nbytes_read = read(fd, chunk->data.heap.p, chunk->data.heap.len);
#endif
	if (_nbytes_read < 0) {
		if (php_cli_server_log_level >= PHP_CLI_SERVER_LOG_ERROR) {
			char *errstr = get_last_error();
			php_cli_server_logf(PHP_CLI_SERVER_LOG_ERROR, "%s", errstr);
			pefree(errstr, 1);
		}
		php_cli_server_chunk_dtor(chunk);
		pefree(chunk, 1);
		return false;
	}
	chunk->data.heap.len = _nbytes_read;
	php_cli_server_buffer_append(&sender->buffer, chunk);
	*nbytes_read = _nbytes_read;
	return true;
} /* }}} */

#if HAVE_UNISTD_H
static int php_cli_is_output_tty(void) /* {{{ */
{
	if (php_cli_output_is_tty == OUTPUT_NOT_CHECKED) {
		php_cli_output_is_tty = isatty(STDOUT_FILENO);
	}
	return php_cli_output_is_tty;
} /* }}} */
#elif defined(PHP_WIN32)
static int php_cli_is_output_tty() /* {{{ */
{
	if (php_cli_output_is_tty == OUTPUT_NOT_CHECKED) {
		php_cli_output_is_tty = php_win32_console_fileno_is_console(STDOUT_FILENO) && php_win32_console_fileno_has_vt100(STDOUT_FILENO);
	}
	return php_cli_output_is_tty;
} /* }}} */
#endif

static void php_cli_server_log_response(php_cli_server_client *client, int status, const char *message) /* {{{ */
{
	int color = 0, effective_status = status;
	char *basic_buf, *message_buf = "", *error_buf = "";
	bool append_error_message = 0;

	if (PG(last_error_message)) {
		if (PG(last_error_type) & E_FATAL_ERRORS) {
			if (status == 200) {
				/* the status code isn't changed by a fatal error, so fake it */
				effective_status = 500;
			}

			append_error_message = 1;
		}
	}

#if HAVE_UNISTD_H || defined(PHP_WIN32)
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
	spprintf(&basic_buf, 0, "%s [%d]: %s %s", ZSTR_VAL(client->addr_str), status,
		php_http_method_str(client->request.request_method), ZSTR_VAL(client->request.request_uri));
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
		spprintf(&error_buf, 0, " - %s in %s on line %d",
			ZSTR_VAL(PG(last_error_message)), ZSTR_VAL(PG(last_error_file)), PG(last_error_lineno));
		if (!error_buf) {
			efree(basic_buf);
			if (message) {
				efree(message_buf);
			}
			return;
		}
	}

	if (color) {
		php_cli_server_logf(PHP_CLI_SERVER_LOG_MESSAGE, "\x1b[3%dm%s%s%s\x1b[0m", color, basic_buf, message_buf, error_buf);
	} else {
		php_cli_server_logf(PHP_CLI_SERVER_LOG_MESSAGE, "%s%s%s", basic_buf, message_buf, error_buf);
	}

	efree(basic_buf);
	if (message) {
		efree(message_buf);
	}
	if (append_error_message) {
		efree(error_buf);
	}
} /* }}} */

static void php_cli_server_logf(int type, const char *format, ...) /* {{{ */
{
	char *buf = NULL;
	va_list ap;

	if (php_cli_server_log_level < type) {
		return;
	}

	va_start(ap, format);
	vspprintf(&buf, 0, format, ap);
	va_end(ap);

	if (!buf) {
		return;
	}

	sapi_cli_server_log_write(type, buf);

	efree(buf);
} /* }}} */

static php_socket_t php_network_listen_socket(const char *host, int *port, int socktype, int *af, socklen_t *socklen, zend_string **errstr) /* {{{ */
{
	php_socket_t retval = SOCK_ERR;
	int err = 0;
	struct sockaddr *sa = NULL, **p, **sal;

	int num_addrs = php_network_getaddresses(host, socktype, &sal, errstr);
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
			*(struct sockaddr_in6 *)sa = *(struct sockaddr_in6 *)*p;
			((struct sockaddr_in6 *)sa)->sin6_port = htons(*port);
			*socklen = sizeof(struct sockaddr_in6);
			break;
#endif
		case AF_INET:
			sa = pemalloc(sizeof(struct sockaddr_in), 1);
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
		if (ZEND_VALID_SOCKET(retval)) {
			closesocket(retval);
		}
		if (errstr) {
			*errstr = php_socket_error_str(err);
		}
		return SOCK_ERR;
	}
	return retval;
} /* }}} */

static void php_cli_server_request_ctor(php_cli_server_request *req) /* {{{ */
{
	req->protocol_version = 0;
	req->request_uri = NULL;
	req->vpath = NULL;
	req->vpath_len = 0;
	req->path_translated = NULL;
	req->path_translated_len = 0;
	req->path_info = NULL;
	req->path_info_len = 0;
	req->query_string = NULL;
	req->query_string_len = 0;
	zend_hash_init(&req->headers, 0, NULL, cli_header_value_dtor, 1);
	/* No destructor is registered as the value pointed by is the same as for &req->headers */
	GC_MAKE_PERSISTENT_LOCAL(&req->headers);
	zend_hash_init(&req->headers_original_case, 0, NULL, NULL, 1);
	GC_MAKE_PERSISTENT_LOCAL(&req->headers_original_case);
	req->content = NULL;
	req->content_len = 0;
	req->ext = NULL;
	req->ext_len = 0;
} /* }}} */

static void php_cli_server_request_dtor(php_cli_server_request *req) /* {{{ */
{
	if (req->request_uri) {
		zend_string_release_ex(req->request_uri, /* persistent */ true);
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
	zend_stat_t sb = {0};
	static const char *index_files[] = { "index.php", "index.html", NULL };
	char *buf = safe_pemalloc(1, request->vpath_len, 1 + document_root_len + 1 + sizeof("index.html"), 1);
	char *p = buf, *prev_path = NULL, *q, *vpath;
	size_t prev_path_len = 0;
	int  is_static_file = 0;

	memmove(p, document_root, document_root_len);
	p += document_root_len;
	vpath = p;
	if (request->vpath_len != 0) {
		if (request->vpath[0] != '/') {
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
	}
	*p = '\0';
	q = p;
	while (q > buf) {
		if (!php_sys_stat(buf, &sb)) {
			if (sb.st_mode & S_IFDIR) {
				const char **file = index_files;
				if (q[-1] != DEFAULT_SLASH) {
					*q++ = DEFAULT_SLASH;
				}
				while (*file) {
					size_t l = strlen(*file);
					memmove(q, *file, l + 1);
					if (!php_sys_stat(buf, &sb) && (sb.st_mode & S_IFREG)) {
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
		uint32_t i = 0;
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
	*retval_len = 0;

	decoded_vpath = pestrndup(vpath, vpath_len, persistent);
	if (!decoded_vpath) {
		return;
	}

	decoded_vpath_end = decoded_vpath + php_raw_url_decode(decoded_vpath, (int)vpath_len);

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

// TODO Update these functions and php_http_parser.h
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
		if (UNEXPECTED(client->request.vpath != NULL)) {
			return 1;
		}
		normalize_vpath(&vpath, &vpath_len, at, length, 1);
		client->request.vpath = vpath;
		client->request.vpath_len = vpath_len;
	}
	return 0;
}

static int php_cli_server_client_read_request_on_query_string(php_http_parser *parser, const char *at, size_t length)
{
	php_cli_server_client *client = parser->data;
	if (EXPECTED(client->request.query_string == NULL)) {
		client->request.query_string = pestrndup(at, length, 1);
		client->request.query_string_len = length;
	} else {
		ZEND_ASSERT(length <= PHP_HTTP_MAX_HEADER_SIZE && PHP_HTTP_MAX_HEADER_SIZE - length >= client->request.query_string_len);
		client->request.query_string = perealloc(client->request.query_string, client->request.query_string_len + length + 1, 1);
		memcpy(client->request.query_string + client->request.query_string_len, at, length);
		client->request.query_string_len += length;
		client->request.query_string[client->request.query_string_len] = '\0';
	}
	return 0;
}

static int php_cli_server_client_read_request_on_url(php_http_parser *parser, const char *at, size_t length)
{
	php_cli_server_client *client = parser->data;
	if (EXPECTED(client->request.request_uri == NULL)) {
		client->request.request_method = parser->method;
		client->request.request_uri = zend_string_init(at, length, /* persistent */ true);
        GC_MAKE_PERSISTENT_LOCAL(client->request.request_uri);
	} else {
		ZEND_ASSERT(client->request.request_method == parser->method);
		ZEND_ASSERT(length <= PHP_HTTP_MAX_HEADER_SIZE && PHP_HTTP_MAX_HEADER_SIZE - length >= client->request.query_string_len);
		/* Extend URI, append content to it */
        client->request.request_uri = cli_concat_persistent_zstr_with_char(client->request.request_uri, at, length);
	}
	return 0;
}

static int php_cli_server_client_read_request_on_fragment(php_http_parser *parser, const char *at, size_t length)
{
	return 0;
}

static void php_cli_server_client_save_header(php_cli_server_client *client)
{
	/* Wrap header value in a zval to add is to the HashTable which acts as an array */
	zval tmp;
	ZVAL_STR(&tmp, client->current_header_value);
	/* strip off the colon */
	zend_string *lc_header_name = zend_string_tolower_ex(client->current_header_name, /* persistent */ true);
	GC_MAKE_PERSISTENT_LOCAL(lc_header_name);

	/* Add the wrapped zend_string to the HashTable */
	zend_hash_add(&client->request.headers, lc_header_name, &tmp);
	zend_hash_add(&client->request.headers_original_case, client->current_header_name, &tmp);

	zend_string_release_ex(lc_header_name, /* persistent */ true);
	zend_string_release_ex(client->current_header_name, /* persistent */ true);

	client->current_header_name = NULL;
	client->current_header_value = NULL;
}

static zend_string* cli_concat_persistent_zstr_with_char(zend_string *old_str, const char *at, size_t length)
{
	/* Assert that there is only one reference to the string, as then zend_string_extends()
	 * will reallocate it such that we do not need to release the old value. */
	ZEND_ASSERT(GC_REFCOUNT(old_str) == 1);
	/* Previous element was part of header value, append content to it */
	size_t old_length = ZSTR_LEN(old_str);
	zend_string *str = zend_string_extend(old_str, old_length + length, /* persistent */ true);
	memcpy(ZSTR_VAL(str) + old_length, at, length);
	// Null terminate
	ZSTR_VAL(str)[ZSTR_LEN(str)] = '\0';
	return str;
}

static int php_cli_server_client_read_request_on_header_field(php_http_parser *parser, const char *at, size_t length)
{
	php_cli_server_client *client = parser->data;
	switch (client->last_header_element) {
		case HEADER_VALUE:
			/* Save previous header before creating new one */
			php_cli_server_client_save_header(client);
			ZEND_FALLTHROUGH;
		case HEADER_NONE:
			/* Create new header field */
			client->current_header_name = zend_string_init(at, length, /* persistent */ true);
			GC_MAKE_PERSISTENT_LOCAL(client->current_header_name);
			break;
		case HEADER_FIELD: {
			/* Append header name to the previous value of it */
			client->current_header_name = cli_concat_persistent_zstr_with_char(client->current_header_name, at, length);
			break;
		}
	}

	client->last_header_element = HEADER_FIELD;
	return 0;
}

static int php_cli_server_client_read_request_on_header_value(php_http_parser *parser, const char *at, size_t length)
{
	php_cli_server_client *client = parser->data;
	switch (client->last_header_element) {
		case HEADER_FIELD:
			/* Previous element was the header field, create the header value */
			client->current_header_value = zend_string_init(at, length, /* persistent */ true);
			GC_MAKE_PERSISTENT_LOCAL(client->current_header_value);
			break;
		case HEADER_VALUE: {
			/* Append header value to the previous value of it */
			client->current_header_value = cli_concat_persistent_zstr_with_char(client->current_header_value, at, length);
			break;
		}
		case HEADER_NONE:
			/* Cannot happen as a header field must have been found before */
			assert(0);
			break;
	}
	client->last_header_element = HEADER_VALUE;
	return 0;
}

static int php_cli_server_client_read_request_on_headers_complete(php_http_parser *parser)
{
	php_cli_server_client *client = parser->data;
	switch (client->last_header_element) {
	case HEADER_NONE:
		break;
	case HEADER_FIELD:
		/* Previous element was the header field, set it's value to an empty string */
		client->current_header_value = ZSTR_EMPTY_ALLOC();
		ZEND_FALLTHROUGH;
	case HEADER_VALUE:
		/* Save last header value */
		php_cli_server_client_save_header(client);
		break;
	}
	client->last_header_element = HEADER_NONE;
	return 0;
}

static int php_cli_server_client_read_request_on_body(php_http_parser *parser, const char *at, size_t length)
{
	php_cli_server_client *client = parser->data;
	if (!client->request.content) {
		client->request.content = pemalloc(parser->content_length, 1);
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
	client->request_read = true;
	return 0;
}

/* Returns:
 -1 when an error occurs
  0 when the request has not been read (try again?)
  1 when the request has been read
*/
static int php_cli_server_client_read_request(php_cli_server_client *client, char **errstr)
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

		if (php_cli_server_log_level >= PHP_CLI_SERVER_LOG_ERROR) {
			*errstr = php_socket_strerror(err, NULL, 0);
		}

		return -1;
	} else if (nbytes_read == 0) {
		if (php_cli_server_log_level >= PHP_CLI_SERVER_LOG_ERROR) {
			*errstr = estrdup(php_cli_server_request_error_unexpected_eof);
		}

		return -1;
	}
	client->parser.data = client;
	nbytes_consumed = php_http_parser_execute(&client->parser, &settings, buf, nbytes_read);
	if (nbytes_consumed != (size_t)nbytes_read) {
		if (php_cli_server_log_level >= PHP_CLI_SERVER_LOG_ERROR) {
			if ((buf[0] & 0x80) /* SSLv2 */ || buf[0] == 0x16 /* SSLv3/TLSv1 */) {
				*errstr = estrdup("Unsupported SSL request");
			} else {
				*errstr = estrdup("Malformed HTTP request");
			}
		}

		return -1;
	}

	return client->request_read ? 1: 0;
}
/* }}} */

static size_t php_cli_server_client_send_through(php_cli_server_client *client, const char *str, size_t str_len) /* {{{ */
{
	struct timeval tv = { 10, 0 };
#ifdef PHP_WIN32
	int nbytes_left = (int)str_len;
#else
	ssize_t nbytes_left = (ssize_t)str_len;
#endif
	do {
#ifdef PHP_WIN32
		int nbytes_sent;
#else
		ssize_t nbytes_sent;
#endif

		nbytes_sent = send(client->sock, str + str_len - nbytes_left, nbytes_left, 0);
		if (nbytes_sent < 0) {
			int err = php_socket_errno();
			if (err == SOCK_EAGAIN) {
				int nfds = php_pollfd_for(client->sock, POLLOUT, &tv);
				if (nfds > 0) {
					continue;
				} else {
					/* error or timeout */
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
	zval *val;

	request_info->request_method = php_http_method_str(client->request.request_method);
	request_info->proto_num = client->request.protocol_version;
	request_info->request_uri = ZSTR_VAL(client->request.request_uri);
	request_info->path_translated = client->request.path_translated;
	request_info->query_string = client->request.query_string;
	request_info->content_length = client->request.content_len;
	request_info->auth_user = request_info->auth_password = request_info->auth_digest = NULL;
	if (NULL != (val = zend_hash_str_find(&client->request.headers, "content-type", sizeof("content-type")-1))) {
		request_info->content_type = Z_STRVAL_P(val);
	}
} /* }}} */

// TODO Remove?
static void destroy_request_info(sapi_request_info *request_info) /* {{{ */
{
} /* }}} */

static void php_cli_server_client_ctor(php_cli_server_client *client, php_cli_server *server, php_socket_t client_sock, struct sockaddr *addr, socklen_t addr_len) /* {{{ */
{
	client->server = server;
	client->sock = client_sock;
	client->addr = addr;
	client->addr_len = addr_len;

	// TODO To prevent the reallocation need to retrieve a persistent string
	// Create a new php_network_populate_name_from_sockaddr_ex() API with a persistent flag?
	zend_string *tmp_addr = NULL;
	php_network_populate_name_from_sockaddr(addr, addr_len, &tmp_addr, NULL, 0);
	client->addr_str = zend_string_dup(tmp_addr, /* persistent */ true);
	GC_MAKE_PERSISTENT_LOCAL(client->addr_str);
	zend_string_release_ex(tmp_addr, /* persistent */ false);

	php_http_parser_init(&client->parser, PHP_HTTP_REQUEST);
	client->request_read = false;

	client->last_header_element = HEADER_NONE;
	client->current_header_name = NULL;
	client->current_header_value = NULL;

	client->post_read_offset = 0;

	php_cli_server_request_ctor(&client->request);

	client->content_sender_initialized = false;
	client->file_fd = -1;
} /* }}} */

static void php_cli_server_client_dtor(php_cli_server_client *client) /* {{{ */
{
	php_cli_server_request_dtor(&client->request);
	if (client->file_fd >= 0) {
		close(client->file_fd);
		client->file_fd = -1;
	}
	pefree(client->addr, 1);
	zend_string_release_ex(client->addr_str, /* persistent */ true);

	if (client->content_sender_initialized) {
		/* Headers must be set if we reached the content initialisation */
		assert(client->current_header_name == NULL);
		assert(client->current_header_value == NULL);
		php_cli_server_content_sender_dtor(&client->content_sender);
	}
} /* }}} */

static void php_cli_server_close_connection(php_cli_server *server, php_cli_server_client *client) /* {{{ */
{
	php_cli_server_logf(PHP_CLI_SERVER_LOG_MESSAGE, "%s Closing", ZSTR_VAL(client->addr_str));

	zend_hash_index_del(&server->clients, client->sock);
} /* }}} */

static zend_result php_cli_server_send_error_page(php_cli_server *server, php_cli_server_client *client, int status) /* {{{ */
{
	zend_string *escaped_request_uri = NULL;
	const char *status_string = get_status_string(status);
	const char *content_template = get_template_string(status);
	char *errstr = get_last_error();
	assert(status_string && content_template);

	php_cli_server_content_sender_ctor(&client->content_sender);
	client->content_sender_initialized = true;

	if (client->request.request_method != PHP_HTTP_HEAD) {
		escaped_request_uri = php_escape_html_entities_ex((const unsigned char *) ZSTR_VAL(client->request.request_uri), ZSTR_LEN(client->request.request_uri), 0, ENT_QUOTES, NULL, /* double_encode */ 0, /* quiet */ 0);

		{
			static const char prologue_template[] = "<!doctype html><html><head><title>%d %s</title>";
			php_cli_server_chunk *chunk = php_cli_server_chunk_heap_new_self_contained(strlen(prologue_template) + 3 + strlen(status_string) + 1);
			if (!chunk) {
				goto fail;
			}
			snprintf(chunk->data.heap.p, chunk->data.heap.len, prologue_template, status, status_string);
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
			php_cli_server_chunk *chunk = php_cli_server_chunk_heap_new_self_contained(strlen(content_template) + ZSTR_LEN(escaped_request_uri) + 3 + strlen(status_string) + 1);
			if (!chunk) {
				goto fail;
			}
			snprintf(chunk->data.heap.p, chunk->data.heap.len, content_template, status_string, ZSTR_VAL(escaped_request_uri));
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
	}

	{
		php_cli_server_chunk *chunk;
		smart_str buffer = { 0 };
		append_http_status_line(&buffer, client->request.protocol_version, status, 1);
		if (!buffer.s) {
			/* out of memory */
			goto fail;
		}
		append_essential_headers(&buffer, client, 1, NULL);
		smart_str_appends_ex(&buffer, "Content-Type: text/html; charset=UTF-8\r\n", 1);
		smart_str_appends_ex(&buffer, "Content-Length: ", 1);
		smart_str_append_unsigned_ex(&buffer, php_cli_server_buffer_size(&client->content_sender.buffer), 1);
		smart_str_appendl_ex(&buffer, "\r\n", 2, 1);
		if (status == 405) {
			smart_str_appends_ex(&buffer, "Allow: ", 1);
			smart_str_appends_ex(&buffer, php_http_method_str(PHP_HTTP_GET), 1);
			smart_str_appends_ex(&buffer, ", ", 1);
			smart_str_appends_ex(&buffer, php_http_method_str(PHP_HTTP_HEAD), 1);
			smart_str_appends_ex(&buffer, ", ", 1);
			smart_str_appends_ex(&buffer, php_http_method_str(PHP_HTTP_POST), 1);
			smart_str_appendl_ex(&buffer, "\r\n", 2, 1);
		}
		smart_str_appendl_ex(&buffer, "\r\n", 2, 1);

		chunk = php_cli_server_chunk_heap_new(buffer.s, ZSTR_VAL(buffer.s), ZSTR_LEN(buffer.s));
		if (!chunk) {
			smart_str_free_ex(&buffer, 1);
			goto fail;
		}
		php_cli_server_buffer_prepend(&client->content_sender.buffer, chunk);
	}

	php_cli_server_log_response(client, status, errstr ? errstr : "?");
	php_cli_server_poller_add(&server->poller, POLLOUT, client->sock);
	if (errstr) {
		pefree(errstr, 1);
	}
	if (escaped_request_uri) {
		zend_string_free(escaped_request_uri);
	}
	return SUCCESS;

fail:
	if (errstr) {
		pefree(errstr, 1);
	}
	if (escaped_request_uri) {
		zend_string_free(escaped_request_uri);
	}
	return FAILURE;
} /* }}} */

static zend_result php_cli_server_dispatch_script(php_cli_server *server, php_cli_server_client *client) /* {{{ */
{
	if (strlen(client->request.path_translated) != client->request.path_translated_len) {
		/* can't handle paths that contain nul bytes */
		return php_cli_server_send_error_page(server, client, 400);
	}

	zend_file_handle zfd;
	zend_stream_init_filename(&zfd, SG(request_info).path_translated);
	zfd.primary_script = 1;
	zend_try {
		php_execute_script(&zfd);
	} zend_end_try();
	zend_destroy_file_handle(&zfd);

	php_cli_server_log_response(client, SG(sapi_headers).http_response_code, NULL);
	return SUCCESS;
} /* }}} */

static zend_result php_cli_server_begin_send_static(php_cli_server *server, php_cli_server_client *client) /* {{{ */
{
	int fd;
	int status = 200;

	if (client->request.request_method == PHP_HTTP_DELETE
		|| client->request.request_method == PHP_HTTP_PUT
		|| client->request.request_method == PHP_HTTP_PATCH) {
		return php_cli_server_send_error_page(server, client, 405);
	}

	if (client->request.path_translated && strlen(client->request.path_translated) != client->request.path_translated_len) {
		/* can't handle paths that contain nul bytes */
		return php_cli_server_send_error_page(server, client, 400);
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
		return php_cli_server_send_error_page(server, client, 500);
	}

	fd = client->request.path_translated ? php_win32_ioutil_open(client->request.path_translated, O_RDONLY): -1;
#else
	fd = client->request.path_translated ? open(client->request.path_translated, O_RDONLY): -1;
#endif
	if (fd < 0) {
		return php_cli_server_send_error_page(server, client, 404);
	}

	php_cli_server_content_sender_ctor(&client->content_sender);
	client->content_sender_initialized = true;
	if (client->request.request_method != PHP_HTTP_HEAD) {
		client->file_fd = fd;
	}

	{
		php_cli_server_chunk *chunk;
		smart_str buffer = { 0 };
		const char *mime_type = get_mime_type(server, client->request.ext, client->request.ext_len);

		append_http_status_line(&buffer, client->request.protocol_version, status, 1);
		if (!buffer.s) {
			/* out of memory */
			php_cli_server_log_response(client, 500, NULL);
			return FAILURE;
		}
		append_essential_headers(&buffer, client, 1, NULL);
		if (mime_type) {
			smart_str_appendl_ex(&buffer, "Content-Type: ", sizeof("Content-Type: ") - 1, 1);
			smart_str_appends_ex(&buffer, mime_type, 1);
			if (strncmp(mime_type, "text/", 5) == 0) {
				smart_str_appends_ex(&buffer, "; charset=UTF-8", 1);
			}
			smart_str_appendl_ex(&buffer, "\r\n", 2, 1);
		}
		smart_str_appends_ex(&buffer, "Content-Length: ", 1);
		smart_str_append_unsigned_ex(&buffer, client->request.sb.st_size, 1);
		smart_str_appendl_ex(&buffer, "\r\n", 2, 1);
		smart_str_appendl_ex(&buffer, "\r\n", 2, 1);
		chunk = php_cli_server_chunk_heap_new(buffer.s, ZSTR_VAL(buffer.s), ZSTR_LEN(buffer.s));
		if (!chunk) {
			smart_str_free_ex(&buffer, 1);
			php_cli_server_log_response(client, 500, NULL);
			return FAILURE;
		}
		php_cli_server_buffer_append(&client->content_sender.buffer, chunk);
	}
	php_cli_server_log_response(client, 200, NULL);
	php_cli_server_poller_add(&server->poller, POLLOUT, client->sock);
	return SUCCESS;
}
/* }}} */

static zend_result php_cli_server_request_startup(php_cli_server *server, php_cli_server_client *client) { /* {{{ */
	zval *auth;
	php_cli_server_client_populate_request_info(client, &SG(request_info));
	if (NULL != (auth = zend_hash_str_find(&client->request.headers, "authorization", sizeof("authorization")-1))) {
		php_handle_auth_data(Z_STRVAL_P(auth));
	}
	SG(sapi_headers).http_response_code = 200;
	if (FAILURE == php_request_startup()) {
		return FAILURE;
	}
	PG(during_request_startup) = 0;

	return SUCCESS;
}
/* }}} */

static void php_cli_server_request_shutdown(php_cli_server *server, php_cli_server_client *client) { /* {{{ */
	php_request_shutdown(0);
	php_cli_server_close_connection(server, client);
	destroy_request_info(&SG(request_info));
	SG(server_context) = NULL;
	SG(rfc1867_uploaded_files) = NULL;
}
/* }}} */

// TODO Use bool, return value is strange
static int php_cli_server_dispatch_router(php_cli_server *server, php_cli_server_client *client) /* {{{ */
{
	int decline = 0;
	zend_file_handle zfd;
	char *old_cwd;

	ALLOCA_FLAG(use_heap)
	old_cwd = do_alloca(MAXPATHLEN, use_heap);
	old_cwd[0] = '\0';
	php_ignore_value(VCWD_GETCWD(old_cwd, MAXPATHLEN - 1));

	zend_stream_init_filename(&zfd, server->router);
	zfd.primary_script = 1;

	zend_try {
		zval retval;

		ZVAL_UNDEF(&retval);
		if (SUCCESS == zend_execute_scripts(ZEND_REQUIRE, &retval, 1, &zfd)) {
			if (Z_TYPE(retval) != IS_UNDEF) {
				decline = Z_TYPE(retval) == IS_FALSE;
				zval_ptr_dtor(&retval);
			}
		}
	} zend_end_try();

	zend_destroy_file_handle(&zfd);

	if (old_cwd[0] != '\0') {
		php_ignore_value(VCWD_CHDIR(old_cwd));
	}

	free_alloca(old_cwd, use_heap);

	return decline;
}
/* }}} */

// TODO Return FAILURE on error? Or voidify as return value of function not checked
static zend_result php_cli_server_dispatch(php_cli_server *server, php_cli_server_client *client) /* {{{ */
{
	int is_static_file  = 0;
	const char *ext = client->request.ext;

	SG(server_context) = client;
	if (client->request.ext_len != 3
	 || (ext[0] != 'p' && ext[0] != 'P') || (ext[1] != 'h' && ext[1] != 'H') || (ext[2] != 'p' && ext[2] != 'P')
	 || !client->request.path_translated) {
		is_static_file = 1;
	}

	if (server->router || !is_static_file) {
		if (FAILURE == php_cli_server_request_startup(server, client)) {
			php_cli_server_request_shutdown(server, client);
			return SUCCESS;
		}
	}

	if (server->router) {
		if (!php_cli_server_dispatch_router(server, client)) {
			php_cli_server_request_shutdown(server, client);
			return SUCCESS;
		}
	}

	if (!is_static_file) {
		// TODO What?
		if (SUCCESS == php_cli_server_dispatch_script(server, client)
				|| FAILURE == php_cli_server_send_error_page(server, client, 500)) {
			if (SG(sapi_headers).http_response_code == 304) {
				SG(sapi_headers).send_default_content_type = 0;
			}
			php_cli_server_request_shutdown(server, client);
			return SUCCESS;
		}
	} else {
		if (server->router) {
			static int (*send_header_func)(sapi_headers_struct *);
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
		if (FAILURE == php_cli_server_begin_send_static(server, client)) {
			php_cli_server_close_connection(server, client);
		}
		SG(server_context) = NULL;
		return SUCCESS;
	}

	SG(server_context) = NULL;
	destroy_request_info(&SG(request_info));
	return SUCCESS;
}
/* }}} */

static void php_cli_server_mime_type_ctor(php_cli_server *server, const php_cli_server_ext_mime_type_pair *mime_type_map) /* {{{ */
{
	const php_cli_server_ext_mime_type_pair *pair;

	zend_hash_init(&server->extension_mime_types, 0, NULL, NULL, 1);
	GC_MAKE_PERSISTENT_LOCAL(&server->extension_mime_types);

	for (pair = mime_type_map; pair->ext; pair++) {
		size_t ext_len = strlen(pair->ext);
		zend_hash_str_add_ptr(&server->extension_mime_types, pair->ext, ext_len, (void*)pair->mime_type);
	}
} /* }}} */

static void php_cli_server_dtor(php_cli_server *server) /* {{{ */
{
	zend_hash_destroy(&server->clients);
	zend_hash_destroy(&server->extension_mime_types);
	if (ZEND_VALID_SOCKET(server->server_sock)) {
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
#if HAVE_FORK
	if (php_cli_server_workers_max > 1 &&
		php_cli_server_workers &&
		getpid() == php_cli_server_master) {
		zend_long php_cli_server_worker;

		for (php_cli_server_worker = 0;
			 php_cli_server_worker < php_cli_server_workers_max;
			 php_cli_server_worker++) {
			 int php_cli_server_worker_status;

			 do {
				if (waitpid(php_cli_server_workers[php_cli_server_worker],
						   &php_cli_server_worker_status,
						   0) == FAILURE) {
					/* an extremely bad thing happened */
					break;
				}

			 } while (!WIFEXITED(php_cli_server_worker_status) &&
					  !WIFSIGNALED(php_cli_server_worker_status));
		}

		pefree(php_cli_server_workers, 1);
	}
#endif
} /* }}} */

static void php_cli_server_client_dtor_wrapper(zval *zv) /* {{{ */
{
	php_cli_server_client *p = Z_PTR_P(zv);

	shutdown(p->sock, SHUT_RDWR);
	closesocket(p->sock);
	php_cli_server_poller_remove(&p->server->poller, POLLIN | POLLOUT, p->sock);
	php_cli_server_client_dtor(p);
	pefree(p, 1);
} /* }}} */

/**
 * Parse the host and port portions of an address specifier in
 * one of the following forms:
 * - hostOrIP:port
 * - [hostOrIP]:port
 */
static char *php_cli_server_parse_addr(const char *addr, int *pport) {
	const char *p, *end;
	long port;

	if (addr[0] == '[') {
		/* Encapsulated [hostOrIP]:port */
		const char *start = addr + 1;
		end = strchr(start, ']');
		if (!end) {
			/* No ending ] delimiter to match [ */
			return NULL;
		}

		p = end + 1;
		if (*p != ':') {
			/* Invalid char following address/missing port */
			return NULL;
		}

		port = strtol(p + 1, (char**)&p, 10);
		if (p && *p) {
			/* Non-numeric in port */
			return NULL;
		}
		if (port < 0 || port > 65535) {
			/* Invalid port */
			return NULL;
		}

		/* Full [hostOrIP]:port provided */
		*pport = (int)port;
		return pestrndup(start, end - start, 1);
	}

	end = strchr(addr, ':');
	if (!end) {
		/* Missing port */
		return NULL;
	}

	port = strtol(end + 1, (char**)&p, 10);
	if (p && *p) {
		/* Non-numeric port */
		return NULL;
	}
	if (port < 0 || port > 65535) {
		/* Invalid port */
		return NULL;
	}
	*pport = (int)port;
	return pestrndup(addr, end - addr, 1);
}

static void php_cli_server_startup_workers(void) {
	char *workers = getenv("PHP_CLI_SERVER_WORKERS");
	if (!workers) {
		return;
	}

#if HAVE_FORK
	php_cli_server_workers_max = ZEND_ATOL(workers);
	if (php_cli_server_workers_max > 1) {
		zend_long php_cli_server_worker;

		php_cli_server_workers = pecalloc(
			php_cli_server_workers_max, sizeof(pid_t), 1);

		php_cli_server_master = getpid();

		for (php_cli_server_worker = 0;
			 php_cli_server_worker < php_cli_server_workers_max;
			 php_cli_server_worker++) {
			pid_t pid = fork();

			if (pid < 0) {
				/* no more forks allowed, work with what we have ... */
				php_cli_server_workers_max =
					php_cli_server_worker + 1;
				return;
			} else if (pid == 0) {
				return;
			} else {
				php_cli_server_workers[php_cli_server_worker] = pid;
			}
		}
	} else {
		fprintf(stderr, "number of workers must be larger than 1\n");
	}
#else
	fprintf(stderr, "forking is not supported on this platform\n");
#endif
}

static zend_result php_cli_server_ctor(php_cli_server *server, const char *addr, const char *document_root, const char *router) /* {{{ */
{
	zend_result retval = SUCCESS;
	char *host = NULL;
	zend_string *errstr = NULL;
	char *_document_root = NULL;
	char *_router = NULL;
	int port = 3000;
	php_socket_t server_sock = SOCK_ERR;

	host = php_cli_server_parse_addr(addr, &port);
	if (!host) {
		fprintf(stderr, "Invalid address: %s\n", addr);
		retval = FAILURE;
		goto out;
	}

	server_sock = php_network_listen_socket(host, &port, SOCK_STREAM, &server->address_family, &server->socklen, &errstr);
	if (server_sock == SOCK_ERR) {
		php_cli_server_logf(PHP_CLI_SERVER_LOG_ERROR, "Failed to listen on %s:%d (reason: %s)", host, port, errstr ? ZSTR_VAL(errstr) : "?");
		if (errstr) {
			zend_string_release_ex(errstr, 0);
		}
		retval = FAILURE;
		goto out;
	}
	// server_sock needs to be non-blocking when using multiple processes. Without it, the first process would
	// successfully accept the connection but the others would block, causing client sockets of the same select
	// call not to be handled.
	if (SUCCESS != php_set_sock_blocking(server_sock, 0)) {
		php_cli_server_logf(PHP_CLI_SERVER_LOG_ERROR, "Failed to make server socket non-blocking");
		retval = FAILURE;
		goto out;
	}
	server->server_sock = server_sock;

	php_cli_server_startup_workers();

	php_cli_server_poller_ctor(&server->poller);

	php_cli_server_poller_add(&server->poller, POLLIN, server_sock);

	server->host = host;
	server->port = port;

	zend_hash_init(&server->clients, 0, NULL, php_cli_server_client_dtor_wrapper, 1);

	{
		size_t document_root_len = strlen(document_root);
		_document_root = pestrndup(document_root, document_root_len, 1);
		server->document_root = _document_root;
		server->document_root_len = document_root_len;
	}

	if (router) {
		size_t router_len = strlen(router);
		_router = pestrndup(router, router_len, 1);
		server->router = _router;
		server->router_len = router_len;
	} else {
		server->router = NULL;
		server->router_len = 0;
	}

	php_cli_server_mime_type_ctor(server, mime_type_map);

	server->is_running = 1;
out:
	if (retval == FAILURE) {
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

static zend_result php_cli_server_recv_event_read_request(php_cli_server *server, php_cli_server_client *client) /* {{{ */
{
	char *errstr = NULL;

	switch (php_cli_server_client_read_request(client, &errstr)) {
		case -1:
			if (errstr) {
				if (strcmp(errstr, php_cli_server_request_error_unexpected_eof) == 0 && client->parser.state == s_start_req) {
					php_cli_server_logf(PHP_CLI_SERVER_LOG_MESSAGE,
						"%s Closed without sending a request; it was probably just an unused speculative preconnection", ZSTR_VAL(client->addr_str));
				} else {
					php_cli_server_logf(PHP_CLI_SERVER_LOG_ERROR, "%s Invalid request (%s)", ZSTR_VAL(client->addr_str), errstr);
				}
				efree(errstr);
			}
			php_cli_server_close_connection(server, client);
			return FAILURE;
		case 1:
			if (client->request.request_method == PHP_HTTP_NOT_IMPLEMENTED) {
				return php_cli_server_send_error_page(server, client, 501);
			}
			php_cli_server_poller_remove(&server->poller, POLLIN, client->sock);
			php_cli_server_dispatch(server, client);
			return SUCCESS;
		case 0:
			php_cli_server_poller_add(&server->poller, POLLIN, client->sock);
			return SUCCESS;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
	/* Under ASAN the compiler somehow doesn't realise that the switch block always returns */
	return FAILURE;
} /* }}} */

static zend_result php_cli_server_send_event(php_cli_server *server, php_cli_server_client *client) /* {{{ */
{
	if (client->content_sender_initialized) {
		if (client->file_fd >= 0 && !client->content_sender.buffer.first) {
			size_t nbytes_read;
			if (!php_cli_server_content_sender_pull(&client->content_sender, client->file_fd, &nbytes_read)) {
				php_cli_server_close_connection(server, client);
				return FAILURE;
			}
			if (nbytes_read == 0) {
				close(client->file_fd);
				client->file_fd = -1;
			}
		}

		size_t nbytes_sent;
		int err = php_cli_server_content_sender_send(&client->content_sender, client->sock, &nbytes_sent);
		if (err && err != SOCK_EAGAIN) {
			php_cli_server_close_connection(server, client);
			return FAILURE;
		}

		if (!client->content_sender.buffer.first && client->file_fd < 0) {
			php_cli_server_close_connection(server, client);
		}
	}
	return SUCCESS;
}
/* }}} */

typedef struct php_cli_server_do_event_for_each_fd_callback_params {
	php_cli_server *server;
	zend_result(*rhandler)(php_cli_server*, php_cli_server_client*);
	zend_result(*whandler)(php_cli_server*, php_cli_server_client*);
} php_cli_server_do_event_for_each_fd_callback_params;

// TODO return FAILURE on failure???
static zend_result php_cli_server_do_event_for_each_fd_callback(void *_params, php_socket_t fd, int event) /* {{{ */
{
	php_cli_server_do_event_for_each_fd_callback_params *params = _params;
	php_cli_server *server = params->server;
	if (server->server_sock == fd) {
		php_cli_server_client *client = NULL;
		php_socket_t client_sock;
		socklen_t socklen = server->socklen;
		struct sockaddr *sa = pemalloc(server->socklen, 1);
		client_sock = accept(server->server_sock, sa, &socklen);
		if (!ZEND_VALID_SOCKET(client_sock)) {
			int err = php_socket_errno();
			if (err != SOCK_EAGAIN && php_cli_server_log_level >= PHP_CLI_SERVER_LOG_ERROR) {
				char *errstr = php_socket_strerror(php_socket_errno(), NULL, 0);
				php_cli_server_logf(PHP_CLI_SERVER_LOG_ERROR,
					"Failed to accept a client (reason: %s)", errstr);
				efree(errstr);
			}
			pefree(sa, 1);
			return SUCCESS;
		}
		if (SUCCESS != php_set_sock_blocking(client_sock, 0)) {
			pefree(sa, 1);
			closesocket(client_sock);
			return SUCCESS;
		}
		client = pemalloc(sizeof(php_cli_server_client), 1);

		php_cli_server_client_ctor(client, server, client_sock, sa, socklen);

		php_cli_server_logf(PHP_CLI_SERVER_LOG_MESSAGE, "%s Accepted", ZSTR_VAL(client->addr_str));

		zend_hash_index_update_ptr(&server->clients, client_sock, client);

		php_cli_server_poller_add(&server->poller, POLLIN, client->sock);
	} else {
		php_cli_server_client *client;
		if (NULL != (client = zend_hash_index_find_ptr(&server->clients, fd))) {
			if (event & POLLIN) {
				params->rhandler(server, client);
			}
			if (event & POLLOUT) {
				params->whandler(server, client);
			}
		}
	}
	return SUCCESS;
} /* }}} */

static void php_cli_server_do_event_for_each_fd(php_cli_server *server,
	zend_result(*rhandler)(php_cli_server*, php_cli_server_client*),
	zend_result(*whandler)(php_cli_server*, php_cli_server_client*)) /* {{{ */
{
	php_cli_server_do_event_for_each_fd_callback_params params = {
		server,
		rhandler,
		whandler
	};

	php_cli_server_poller_iter_on_active(&server->poller, &params, php_cli_server_do_event_for_each_fd_callback);
} /* }}} */

// TODO Return value of function is not used
static zend_result php_cli_server_do_event_loop(php_cli_server *server) /* {{{ */
{
	zend_result retval = SUCCESS;
	while (server->is_running) {
		struct timeval tv = { 1, 0 };
		int n = php_cli_server_poller_poll(&server->poller, &tv);
		if (n > 0) {
			php_cli_server_do_event_for_each_fd(server,
					php_cli_server_recv_event_read_request,
					php_cli_server_send_event);
		} else if (n == 0) {
			/* do nothing */
		} else {
			int err = php_socket_errno();
			if (err != SOCK_EINTR) {
				if (php_cli_server_log_level >= PHP_CLI_SERVER_LOG_ERROR) {
					char *errstr = php_socket_strerror(err, NULL, 0);
					php_cli_server_logf(PHP_CLI_SERVER_LOG_ERROR, "%s", errstr);
					efree(errstr);
				}
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

/* Returns status code */
int do_cli_server(int argc, char **argv) /* {{{ */
{
	char *php_optarg = NULL;
	int php_optind = 1;
	int c;
	const char *server_bind_address = NULL;
	extern const opt_struct OPTIONS[];
	const char *document_root = NULL;
#ifdef PHP_WIN32
	char document_root_tmp[MAXPATHLEN];
	size_t k;
#endif
	const char *router = NULL;
	char document_root_buf[MAXPATHLEN];

	while ((c = php_getopt(argc, argv, OPTIONS, &php_optarg, &php_optind, 0, 2))!=-1) {
		switch (c) {
			case 'S':
				server_bind_address = php_optarg;
				break;
			case 't':
#ifndef PHP_WIN32
				document_root = php_optarg;
#else
				k = strlen(php_optarg);
				if (k + 1 > MAXPATHLEN) {
					fprintf(stderr, "Document root path is too long.\n");
					return 1;
				}
				memmove(document_root_tmp, php_optarg, k + 1);
				/* Clean out any trailing garbage that might have been passed
					from a batch script. */
				do {
					document_root_tmp[k] = '\0';
					k--;
				} while ('"' == document_root_tmp[k] || ' ' == document_root_tmp[k]);
				document_root = document_root_tmp;
#endif
				break;
			case 'q':
				if (php_cli_server_log_level > 1) {
					php_cli_server_log_level--;
				}
				break;
		}
	}

	if (document_root) {
		zend_stat_t sb = {0};

		if (php_sys_stat(document_root, &sb)) {
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

	if (FAILURE == php_cli_server_ctor(&server, server_bind_address, document_root, router)) {
		return 1;
	}
	sapi_module.phpinfo_as_text = 0;

	{
		bool ipv6 = strchr(server.host, ':');
		php_cli_server_logf(
			PHP_CLI_SERVER_LOG_PROCESS,
			"PHP %s Development Server (http://%s%s%s:%d) started",
			PHP_VERSION, ipv6 ? "[" : "", server.host,
			ipv6 ? "]" : "", server.port);
	}

#if defined(SIGINT)
	signal(SIGINT, php_cli_server_sigint_handler);
#endif

#if defined(SIGPIPE)
	signal(SIGPIPE, SIG_IGN);
#endif

	zend_signal_init();

	php_cli_server_do_event_loop(&server);
	php_cli_server_dtor(&server);
	return 0;
} /* }}} */
