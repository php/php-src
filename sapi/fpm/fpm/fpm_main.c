/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Stig Bakken <ssb@php.net>                                   |
   |          Zeev Suraski <zeev@zend.com>                                |
   | FastCGI: Ben Mansell <php@slimyhorror.com>                           |
   |          Shane Caraveo <shane@caraveo.com>                           |
   |          Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

/* $Id: cgi_main.c 291497 2009-11-30 14:43:22Z dmitry $ */

#include "php.h"
#include "php_globals.h"
#include "php_variables.h"
#include "zend_modules.h"
#include "php.h"
#include "zend_ini_scanner.h"
#include "zend_globals.h"
#include "zend_stream.h"

#include "SAPI.h"

#include <stdio.h>
#include "php.h"

#ifdef PHP_WIN32
# include "win32/time.h"
# include "win32/signal.h"
# include <process.h>
#endif

#if HAVE_SYS_TIME_H
# include <sys/time.h>
#endif

#if HAVE_UNISTD_H
# include <unistd.h>
#endif

#if HAVE_SIGNAL_H
# include <signal.h>
#endif

#if HAVE_SETLOCALE
# include <locale.h>
#endif

#if HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#if HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif

#if HAVE_FCNTL_H
# include <fcntl.h>
#endif

#include "zend.h"
#include "zend_extensions.h"
#include "php_ini.h"
#include "php_globals.h"
#include "php_main.h"
#include "fopen_wrappers.h"
#include "ext/standard/php_standard.h"

#ifdef PHP_WIN32
# include <io.h>
# include <fcntl.h>
# include "win32/php_registry.h"
#endif

#ifdef __riscos__
# include <unixlib/local.h>
int __riscosify_control = __RISCOSIFY_STRICT_UNIX_SPECS;
#endif

#include "zend_compile.h"
#include "zend_execute.h"
#include "zend_highlight.h"
#include "zend_indent.h"

#include "php_getopt.h"

#include "fastcgi.h"

#include <php_config.h>
#include "fpm.h"
#include "fpm_request.h"
#include "fpm_status.h"
#include "fpm_conf.h"
#include "fpm_php.h"
#include "fpm_log.h"
#include "zlog.h"

#ifndef PHP_WIN32
/* XXX this will need to change later when threaded fastcgi is implemented.  shane */
struct sigaction act, old_term, old_quit, old_int;
#endif

static void (*php_php_import_environment_variables)(zval *array_ptr TSRMLS_DC);

#ifndef PHP_WIN32
/* these globals used for forking children on unix systems */

/**
 * Set to non-zero if we are the parent process
 */
static int parent = 1;
#endif

static int request_body_fd;
static int fpm_is_running = 0;

static char *sapi_cgibin_getenv(char *name, size_t name_len TSRMLS_DC);
static void fastcgi_ini_parser(zval *arg1, zval *arg2, zval *arg3, int callback_type, void *arg TSRMLS_DC);

#define PHP_MODE_STANDARD	1
#define PHP_MODE_HIGHLIGHT	2
#define PHP_MODE_INDENT		3
#define PHP_MODE_LINT		4
#define PHP_MODE_STRIP		5

static char *php_optarg = NULL;
static int php_optind = 1;
static zend_module_entry cgi_module_entry;

static const opt_struct OPTIONS[] = {
	{'c', 1, "php-ini"},
	{'d', 1, "define"},
	{'e', 0, "profile-info"},
	{'h', 0, "help"},
	{'i', 0, "info"},
	{'m', 0, "modules"},
	{'n', 0, "no-php-ini"},
	{'?', 0, "usage"},/* help alias (both '?' and 'usage') */
	{'v', 0, "version"},
	{'y', 1, "fpm-config"},
	{'t', 0, "test"},
	{'p', 1, "prefix"},
	{'g', 1, "pid"},
	{'R', 0, "allow-to-run-as-root"},
	{'D', 0, "daemonize"},
	{'F', 0, "nodaemonize"},
	{'-', 0, NULL} /* end of args */
};

typedef struct _php_cgi_globals_struct {
	zend_bool rfc2616_headers;
	zend_bool nph;
	zend_bool fix_pathinfo;
	zend_bool force_redirect;
	zend_bool discard_path;
	zend_bool fcgi_logging;
	char *redirect_status_env;
	HashTable user_config_cache;
	char *error_header;
	char *fpm_config;
} php_cgi_globals_struct;

/* {{{ user_config_cache
 *
 * Key for each cache entry is dirname(PATH_TRANSLATED).
 *
 * NOTE: Each cache entry config_hash contains the combination from all user ini files found in
 *       the path starting from doc_root throught to dirname(PATH_TRANSLATED).  There is no point
 *       storing per-file entries as it would not be possible to detect added / deleted entries
 *       between separate files.
 */
typedef struct _user_config_cache_entry {
	time_t expires;
	HashTable *user_config;
} user_config_cache_entry;

static void user_config_cache_entry_dtor(user_config_cache_entry *entry)
{
	zend_hash_destroy(entry->user_config);
	free(entry->user_config);
}
/* }}} */

#ifdef ZTS
static int php_cgi_globals_id;
#define CGIG(v) TSRMG(php_cgi_globals_id, php_cgi_globals_struct *, v)
#else
static php_cgi_globals_struct php_cgi_globals;
#define CGIG(v) (php_cgi_globals.v)
#endif

#ifdef PHP_WIN32
#define TRANSLATE_SLASHES(path) \
	{ \
		char *tmp = path; \
		while (*tmp) { \
			if (*tmp == '\\') *tmp = '/'; \
			tmp++; \
		} \
	}
#else
#define TRANSLATE_SLASHES(path)
#endif

static int print_module_info(zend_module_entry *module, void *arg TSRMLS_DC)
{
	php_printf("%s\n", module->name);
	return 0;
}

static int module_name_cmp(const void *a, const void *b TSRMLS_DC)
{
	Bucket *f = *((Bucket **) a);
	Bucket *s = *((Bucket **) b);

	return strcasecmp(	((zend_module_entry *)f->pData)->name,
						((zend_module_entry *)s->pData)->name);
}

static void print_modules(TSRMLS_D)
{
	HashTable sorted_registry;
	zend_module_entry tmp;

	zend_hash_init(&sorted_registry, 50, NULL, NULL, 1);
	zend_hash_copy(&sorted_registry, &module_registry, NULL, &tmp, sizeof(zend_module_entry));
	zend_hash_sort(&sorted_registry, zend_qsort, module_name_cmp, 0 TSRMLS_CC);
	zend_hash_apply_with_argument(&sorted_registry, (apply_func_arg_t) print_module_info, NULL TSRMLS_CC);
	zend_hash_destroy(&sorted_registry);
}

static int print_extension_info(zend_extension *ext, void *arg TSRMLS_DC)
{
	php_printf("%s\n", ext->name);
	return 0;
}

static int extension_name_cmp(const zend_llist_element **f, const zend_llist_element **s TSRMLS_DC)
{
	return strcmp(	((zend_extension *)(*f)->data)->name,
					((zend_extension *)(*s)->data)->name);
}

static void print_extensions(TSRMLS_D)
{
	zend_llist sorted_exts;

	zend_llist_copy(&sorted_exts, &zend_extensions);
	sorted_exts.dtor = NULL;
	zend_llist_sort(&sorted_exts, extension_name_cmp TSRMLS_CC);
	zend_llist_apply_with_argument(&sorted_exts, (llist_apply_with_arg_func_t) print_extension_info, NULL TSRMLS_CC);
	zend_llist_destroy(&sorted_exts);
}

#ifndef STDOUT_FILENO	 
#define STDOUT_FILENO 1	 
#endif

static inline size_t sapi_cgibin_single_write(const char *str, uint str_length TSRMLS_DC)
{
	ssize_t ret;

	/* sapi has started which means everyhting must be send through fcgi */
	if (fpm_is_running) {
		fcgi_request *request = (fcgi_request*) SG(server_context);
		ret = fcgi_write(request, FCGI_STDOUT, str, str_length);
		if (ret <= 0) {
			return 0;
		}
		return (size_t)ret;
	}

	/* sapi has not started, output to stdout instead of fcgi */
#ifdef PHP_WRITE_STDOUT	 
	ret = write(STDOUT_FILENO, str, str_length);	 
	if (ret <= 0) {
		return 0;
	}
	return (size_t)ret;
#else
	return fwrite(str, 1, MIN(str_length, 16384), stdout);
#endif
}

static int sapi_cgibin_ub_write(const char *str, uint str_length TSRMLS_DC)
{
	const char *ptr = str;
	uint remaining = str_length;
	size_t ret;

	while (remaining > 0) {
		ret = sapi_cgibin_single_write(ptr, remaining TSRMLS_CC);
		if (!ret) {
			php_handle_aborted_connection();
			return str_length - remaining;
		}
		ptr += ret;
		remaining -= ret;
	}

	return str_length;
}


static void sapi_cgibin_flush(void *server_context TSRMLS_DC)
{
	/* fpm has started, let use fcgi instead of stdout */
	if (fpm_is_running) {
		fcgi_request *request = (fcgi_request*) server_context;
		if (
#ifndef PHP_WIN32
	      !parent &&
#endif
	      request && !fcgi_flush(request, 0)) {
			php_handle_aborted_connection();
		}
		return;
	}

	/* fpm has not started yet, let use stdout instead of fcgi */
	if (fflush(stdout) == EOF) {
		php_handle_aborted_connection();
	}
}

#define SAPI_CGI_MAX_HEADER_LENGTH 1024

typedef struct _http_error {
	int code;
	const char* msg;
} http_error;

static const http_error http_error_codes[] = {
	{100, "Continue"},
	{101, "Switching Protocols"},
	{200, "OK"},
	{201, "Created"},
	{202, "Accepted"},
	{203, "Non-Authoritative Information"},
	{204, "No Content"},
	{205, "Reset Content"},
	{206, "Partial Content"},
	{300, "Multiple Choices"},
	{301, "Moved Permanently"},
	{302, "Moved Temporarily"},
	{303, "See Other"},
	{304, "Not Modified"},
	{305, "Use Proxy"},
	{400, "Bad Request"},
	{401, "Unauthorized"},
	{402, "Payment Required"},
	{403, "Forbidden"},
	{404, "Not Found"},
	{405, "Method Not Allowed"},
	{406, "Not Acceptable"},
	{407, "Proxy Authentication Required"},
	{408, "Request Time-out"},
	{409, "Conflict"},
	{410, "Gone"},
	{411, "Length Required"},
	{412, "Precondition Failed"},
	{413, "Request Entity Too Large"},
	{414, "Request-URI Too Large"},
	{415, "Unsupported Media Type"},
	{428, "Precondition Required"},
	{429, "Too Many Requests"},
	{431, "Request Header Fields Too Large"},
	{500, "Internal Server Error"},
	{501, "Not Implemented"},
	{502, "Bad Gateway"},
	{503, "Service Unavailable"},
	{504, "Gateway Time-out"},
	{505, "HTTP Version not supported"},
	{511, "Network Authentication Required"},
	{0,   NULL}
};

static int sapi_cgi_send_headers(sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	char buf[SAPI_CGI_MAX_HEADER_LENGTH];
	sapi_header_struct *h;
	zend_llist_position pos;
	zend_bool ignore_status = 0;
	int response_status = SG(sapi_headers).http_response_code;

	if (SG(request_info).no_headers == 1) {
		return  SAPI_HEADER_SENT_SUCCESSFULLY;
	}

	if (CGIG(nph) || SG(sapi_headers).http_response_code != 200)
	{
		int len;
		zend_bool has_status = 0;

		if (CGIG(rfc2616_headers) && SG(sapi_headers).http_status_line) {
			char *s;
			len = slprintf(buf, SAPI_CGI_MAX_HEADER_LENGTH, "%s\r\n", SG(sapi_headers).http_status_line);
			if ((s = strchr(SG(sapi_headers).http_status_line, ' '))) {
				response_status = atoi((s + 1));
			}

			if (len > SAPI_CGI_MAX_HEADER_LENGTH) {
				len = SAPI_CGI_MAX_HEADER_LENGTH;
			}

		} else {
			char *s;

			if (SG(sapi_headers).http_status_line &&
				(s = strchr(SG(sapi_headers).http_status_line, ' ')) != 0 &&
				(s - SG(sapi_headers).http_status_line) >= 5 &&
				strncasecmp(SG(sapi_headers).http_status_line, "HTTP/", 5) == 0
			) {
				len = slprintf(buf, sizeof(buf), "Status:%s\r\n", s);
				response_status = atoi((s + 1));
			} else {
				h = (sapi_header_struct*)zend_llist_get_first_ex(&sapi_headers->headers, &pos);
				while (h) {
					if (h->header_len > sizeof("Status:") - 1 &&
						strncasecmp(h->header, "Status:", sizeof("Status:") - 1) == 0
					) {
						has_status = 1;
						break;
					}
					h = (sapi_header_struct*)zend_llist_get_next_ex(&sapi_headers->headers, &pos);
				}
				if (!has_status) {
					http_error *err = (http_error*)http_error_codes;

					while (err->code != 0) {
						if (err->code == SG(sapi_headers).http_response_code) {
							break;
						}
						err++;
					}
					if (err->msg) {
						len = slprintf(buf, sizeof(buf), "Status: %d %s\r\n", SG(sapi_headers).http_response_code, err->msg);
					} else {
						len = slprintf(buf, sizeof(buf), "Status: %d\r\n", SG(sapi_headers).http_response_code);
					}
				}
			}
		}

		if (!has_status) {
			PHPWRITE_H(buf, len);
			ignore_status = 1;
		}
	}

	h = (sapi_header_struct*)zend_llist_get_first_ex(&sapi_headers->headers, &pos);
	while (h) {
		/* prevent CRLFCRLF */
		if (h->header_len) {
			if (h->header_len > sizeof("Status:") - 1 && 
				strncasecmp(h->header, "Status:", sizeof("Status:") - 1) == 0
			) {
				if (!ignore_status) {
					ignore_status = 1;
					PHPWRITE_H(h->header, h->header_len);
					PHPWRITE_H("\r\n", 2);
				}
			} else if (response_status == 304 && h->header_len > sizeof("Content-Type:") - 1 &&
				strncasecmp(h->header, "Content-Type:", sizeof("Content-Type:") - 1) == 0
			) {
				h = (sapi_header_struct*)zend_llist_get_next_ex(&sapi_headers->headers, &pos);
				continue;
			} else {
				PHPWRITE_H(h->header, h->header_len);
				PHPWRITE_H("\r\n", 2);
			}
		}
		h = (sapi_header_struct*)zend_llist_get_next_ex(&sapi_headers->headers, &pos);
	}
	PHPWRITE_H("\r\n", 2);

	return SAPI_HEADER_SENT_SUCCESSFULLY;
}

#ifndef STDIN_FILENO
# define STDIN_FILENO 0
#endif

static int sapi_cgi_read_post(char *buffer, uint count_bytes TSRMLS_DC)
{
	uint read_bytes = 0;
	int tmp_read_bytes;
	size_t remaining = SG(request_info).content_length - SG(read_post_bytes);

	if (remaining < count_bytes) {
		count_bytes = remaining;
	}
	while (read_bytes < count_bytes) {
		fcgi_request *request = (fcgi_request*) SG(server_context);
		if (request_body_fd == -1) {
			char *request_body_filename = sapi_cgibin_getenv((char *) "REQUEST_BODY_FILE",
					sizeof("REQUEST_BODY_FILE") - 1 TSRMLS_CC);

			if (request_body_filename && *request_body_filename) {
				request_body_fd = open(request_body_filename, O_RDONLY);

				if (0 > request_body_fd) {
					php_error(E_WARNING, "REQUEST_BODY_FILE: open('%s') failed: %s (%d)",
							request_body_filename, strerror(errno), errno);
					return 0;
				}
			}
		}

		/* If REQUEST_BODY_FILE variable not available - read post body from fastcgi stream */
		if (request_body_fd < 0) {
			tmp_read_bytes = fcgi_read(request, buffer + read_bytes, count_bytes - read_bytes);
		} else {
			tmp_read_bytes = read(request_body_fd, buffer + read_bytes, count_bytes - read_bytes);
		}
		if (tmp_read_bytes <= 0) {
			break;
		}
		read_bytes += tmp_read_bytes;
	}
	return read_bytes;
}

static char *sapi_cgibin_getenv(char *name, size_t name_len TSRMLS_DC)
{
	/* if fpm has started, use fcgi env */
	if (fpm_is_running) {
		fcgi_request *request = (fcgi_request*) SG(server_context);
		return fcgi_getenv(request, name, name_len);
	}

	/* if fpm has not started yet, use std env */
	return getenv(name);
}

static char *_sapi_cgibin_putenv(char *name, char *value TSRMLS_DC)
{
	int name_len;

	if (!name) {
		return NULL;
	}
	name_len = strlen(name);

	fcgi_request *request = (fcgi_request*) SG(server_context);
	return fcgi_putenv(request, name, name_len, value);
}

static char *sapi_cgi_read_cookies(TSRMLS_D)
{
	return sapi_cgibin_getenv((char *) "HTTP_COOKIE", sizeof("HTTP_COOKIE") - 1 TSRMLS_CC);
}

void cgi_php_import_environment_variables(zval *array_ptr TSRMLS_DC)
{
	fcgi_request *request;
	HashPosition pos;
	char *var, **val;
	uint var_len;
	ulong idx;
	int filter_arg;


	if (PG(http_globals)[TRACK_VARS_ENV] &&
		array_ptr != PG(http_globals)[TRACK_VARS_ENV] &&
		Z_TYPE_P(PG(http_globals)[TRACK_VARS_ENV]) == IS_ARRAY &&
		zend_hash_num_elements(Z_ARRVAL_P(PG(http_globals)[TRACK_VARS_ENV])) > 0
	) {
		zval_dtor(array_ptr);
		*array_ptr = *PG(http_globals)[TRACK_VARS_ENV];
		INIT_PZVAL(array_ptr);
		zval_copy_ctor(array_ptr);
		return;
	} else if (PG(http_globals)[TRACK_VARS_SERVER] &&
		array_ptr != PG(http_globals)[TRACK_VARS_SERVER] &&
		Z_TYPE_P(PG(http_globals)[TRACK_VARS_SERVER]) == IS_ARRAY &&
		zend_hash_num_elements(Z_ARRVAL_P(PG(http_globals)[TRACK_VARS_SERVER])) > 0
	) {
		zval_dtor(array_ptr);
		*array_ptr = *PG(http_globals)[TRACK_VARS_SERVER];
		INIT_PZVAL(array_ptr);
		zval_copy_ctor(array_ptr);
		return;
	}

	/* call php's original import as a catch-all */
	php_php_import_environment_variables(array_ptr TSRMLS_CC);

	request = (fcgi_request*) SG(server_context);
	filter_arg = (array_ptr == PG(http_globals)[TRACK_VARS_ENV])?PARSE_ENV:PARSE_SERVER;

	for (zend_hash_internal_pointer_reset_ex(request->env, &pos);
	     zend_hash_get_current_key_ex(request->env, &var, &var_len, &idx, 0, &pos) == HASH_KEY_IS_STRING &&
	     zend_hash_get_current_data_ex(request->env, (void **) &val, &pos) == SUCCESS;
	     zend_hash_move_forward_ex(request->env, &pos)
	) {
		unsigned int new_val_len;

		if (sapi_module.input_filter(filter_arg, var, val, strlen(*val), &new_val_len TSRMLS_CC)) {
			php_register_variable_safe(var, *val, new_val_len, array_ptr TSRMLS_CC);
		}
	}
}

static void sapi_cgi_register_variables(zval *track_vars_array TSRMLS_DC)
{
	unsigned int php_self_len;
	char *php_self;

	/* In CGI mode, we consider the environment to be a part of the server
	 * variables
	 */
	php_import_environment_variables(track_vars_array TSRMLS_CC);

	if (CGIG(fix_pathinfo)) {
		char *script_name = SG(request_info).request_uri;
		unsigned int script_name_len = script_name ? strlen(script_name) : 0;
		char *path_info = sapi_cgibin_getenv("PATH_INFO", sizeof("PATH_INFO") - 1 TSRMLS_CC);
		unsigned int path_info_len = path_info ? strlen(path_info) : 0;

		php_self_len = script_name_len + path_info_len;
		php_self = emalloc(php_self_len + 1);

		/* Concat script_name and path_info into php_self */
		if (script_name) {
			memcpy(php_self, script_name, script_name_len + 1);
		}
		if (path_info) {
			memcpy(php_self + script_name_len, path_info, path_info_len + 1);
		}

		/* Build the special-case PHP_SELF variable for the CGI version */
		if (sapi_module.input_filter(PARSE_SERVER, "PHP_SELF", &php_self, php_self_len, &php_self_len TSRMLS_CC)) {
			php_register_variable_safe("PHP_SELF", php_self, php_self_len, track_vars_array TSRMLS_CC);
		}
		efree(php_self);
	} else {
		php_self = SG(request_info).request_uri ? SG(request_info).request_uri : "";
		php_self_len = strlen(php_self);
		if (sapi_module.input_filter(PARSE_SERVER, "PHP_SELF", &php_self, php_self_len, &php_self_len TSRMLS_CC)) {
			php_register_variable_safe("PHP_SELF", php_self, php_self_len, track_vars_array TSRMLS_CC);
		}
	}
}

/* {{{ sapi_cgi_log_fastcgi
 *
 * Ignore level, we want to send all messages through fastcgi
 */
void sapi_cgi_log_fastcgi(int level, char *message, size_t len)
{
	TSRMLS_FETCH();

	fcgi_request *request = (fcgi_request*) SG(server_context);

	/* ensure we want:
	 * - to log (fastcgi.logging in php.ini)
	 * - we are currently dealing with a request
	 * - the message is not empty
	 */
	if (CGIG(fcgi_logging) && request && message && len > 0) {
		char *buf = malloc(len + 2);
		memcpy(buf, message, len);
		memcpy(buf + len, "\n", sizeof("\n"));
		fcgi_write(request, FCGI_STDERR, buf, len+1);
		free(buf);
	}
}
/* }}} */

/* {{{ sapi_cgi_log_message
 */
static void sapi_cgi_log_message(char *message)
{
	zlog(ZLOG_NOTICE, "PHP message: %s", message);
}
/* }}} */

/* {{{ php_cgi_ini_activate_user_config
 */
static void php_cgi_ini_activate_user_config(char *path, int path_len, const char *doc_root, int doc_root_len, int start TSRMLS_DC)
{
	char *ptr;
	user_config_cache_entry *new_entry, *entry;
	time_t request_time = sapi_get_request_time(TSRMLS_C);

	/* Find cached config entry: If not found, create one */
	if (zend_hash_find(&CGIG(user_config_cache), path, path_len + 1, (void **) &entry) == FAILURE) {
		new_entry = pemalloc(sizeof(user_config_cache_entry), 1);
		new_entry->expires = 0;
		new_entry->user_config = (HashTable *) pemalloc(sizeof(HashTable), 1);
		zend_hash_init(new_entry->user_config, 0, NULL, (dtor_func_t) config_zval_dtor, 1);
		zend_hash_update(&CGIG(user_config_cache), path, path_len + 1, new_entry, sizeof(user_config_cache_entry), (void **) &entry);
		free(new_entry);
	}

	/* Check whether cache entry has expired and rescan if it is */
	if (request_time > entry->expires) {
		char * real_path;
		int real_path_len;
		char *s1, *s2;
		int s_len;

		/* Clear the expired config */
		zend_hash_clean(entry->user_config);

		if (!IS_ABSOLUTE_PATH(path, path_len)) {
			real_path = tsrm_realpath(path, NULL TSRMLS_CC);
			if (real_path == NULL) {
				return;
			}
			real_path_len = strlen(real_path);
			path = real_path;
			path_len = real_path_len;
		}

		if (path_len > doc_root_len) {
			s1 = (char *) doc_root;
			s2 = path;
			s_len = doc_root_len;
		} else {
			s1 = path;
			s2 = (char *) doc_root;
			s_len = path_len;
		}

		/* we have to test if path is part of DOCUMENT_ROOT.
		  if it is inside the docroot, we scan the tree up to the docroot 
			to find more user.ini, if not we only scan the current path.
		  */
#ifdef PHP_WIN32
		if (strnicmp(s1, s2, s_len) == 0) {
#else 
		if (strncmp(s1, s2, s_len) == 0) {
#endif
			ptr = s2 + start;  /* start is the point where doc_root ends! */
			while ((ptr = strchr(ptr, DEFAULT_SLASH)) != NULL) {
				*ptr = 0;
				php_parse_user_ini_file(path, PG(user_ini_filename), entry->user_config TSRMLS_CC);
				*ptr = '/';
				ptr++;
			}
		} else {
			php_parse_user_ini_file(path, PG(user_ini_filename), entry->user_config TSRMLS_CC);
		}

		entry->expires = request_time + PG(user_ini_cache_ttl);
	}

	/* Activate ini entries with values from the user config hash */
	php_ini_activate_config(entry->user_config, PHP_INI_PERDIR, PHP_INI_STAGE_HTACCESS TSRMLS_CC);
}
/* }}} */

static int sapi_cgi_activate(TSRMLS_D)
{
	char *path, *doc_root, *server_name;
	uint path_len, doc_root_len, server_name_len;

	/* PATH_TRANSLATED should be defined at this stage but better safe than sorry :) */
	if (!SG(request_info).path_translated) {
		return FAILURE;
	}

	if (php_ini_has_per_host_config()) {
		/* Activate per-host-system-configuration defined in php.ini and stored into configuration_hash during startup */
		server_name = sapi_cgibin_getenv("SERVER_NAME", sizeof("SERVER_NAME") - 1 TSRMLS_CC);
		/* SERVER_NAME should also be defined at this stage..but better check it anyway */
		if (server_name) {
			server_name_len = strlen(server_name);
			server_name = estrndup(server_name, server_name_len);
			zend_str_tolower(server_name, server_name_len);
			php_ini_activate_per_host_config(server_name, server_name_len + 1 TSRMLS_CC);
			efree(server_name);
		}
	}

	if (php_ini_has_per_dir_config() ||
		(PG(user_ini_filename) && *PG(user_ini_filename))
	) {
		/* Prepare search path */
		path_len = strlen(SG(request_info).path_translated);

		/* Make sure we have trailing slash! */
		if (!IS_SLASH(SG(request_info).path_translated[path_len])) {
			path = emalloc(path_len + 2);
			memcpy(path, SG(request_info).path_translated, path_len + 1);
			path_len = zend_dirname(path, path_len);
			path[path_len++] = DEFAULT_SLASH;
		} else {
			path = estrndup(SG(request_info).path_translated, path_len);
			path_len = zend_dirname(path, path_len);
		}
		path[path_len] = 0;

		/* Activate per-dir-system-configuration defined in php.ini and stored into configuration_hash during startup */
		php_ini_activate_per_dir_config(path, path_len TSRMLS_CC); /* Note: for global settings sake we check from root to path */

		/* Load and activate user ini files in path starting from DOCUMENT_ROOT */
		if (PG(user_ini_filename) && *PG(user_ini_filename)) {
			doc_root = sapi_cgibin_getenv("DOCUMENT_ROOT", sizeof("DOCUMENT_ROOT") - 1 TSRMLS_CC);
			/* DOCUMENT_ROOT should also be defined at this stage..but better check it anyway */
			if (doc_root) {
				doc_root_len = strlen(doc_root);
				if (doc_root_len > 0 && IS_SLASH(doc_root[doc_root_len - 1])) {
					--doc_root_len;
				}
#ifdef PHP_WIN32
				/* paths on windows should be case-insensitive */
				doc_root = estrndup(doc_root, doc_root_len);
				zend_str_tolower(doc_root, doc_root_len);
#endif
				php_cgi_ini_activate_user_config(path, path_len, doc_root, doc_root_len, doc_root_len - 1 TSRMLS_CC);
			}
		}

#ifdef PHP_WIN32
		efree(doc_root);
#endif
		efree(path);
	}

	return SUCCESS;
}

static int sapi_cgi_deactivate(TSRMLS_D)
{
	/* flush only when SAPI was started. The reasons are:
		1. SAPI Deactivate is called from two places: module init and request shutdown
		2. When the first call occurs and the request is not set up, flush fails on FastCGI.
	*/
	if (SG(sapi_started)) {
		if (
#ifndef PHP_WIN32
		    !parent &&
#endif
		    !fcgi_finish_request((fcgi_request*)SG(server_context), 0)) {
			php_handle_aborted_connection();
		}
	}
	return SUCCESS;
}

static int php_cgi_startup(sapi_module_struct *sapi_module)
{
	if (php_module_startup(sapi_module, &cgi_module_entry, 1) == FAILURE) {
		return FAILURE;
	}
	return SUCCESS;
}

/* {{{ sapi_module_struct cgi_sapi_module
 */
static sapi_module_struct cgi_sapi_module = {
	"fpm-fcgi",						/* name */
	"FPM/FastCGI",					/* pretty name */

	php_cgi_startup,				/* startup */
	php_module_shutdown_wrapper,	/* shutdown */

	sapi_cgi_activate,				/* activate */
	sapi_cgi_deactivate,			/* deactivate */

	sapi_cgibin_ub_write,			/* unbuffered write */
	sapi_cgibin_flush,				/* flush */
	NULL,							/* get uid */
	sapi_cgibin_getenv,				/* getenv */

	php_error,						/* error handler */

	NULL,							/* header handler */
	sapi_cgi_send_headers,			/* send headers handler */
	NULL,							/* send header handler */

	sapi_cgi_read_post,				/* read POST data */
	sapi_cgi_read_cookies,			/* read Cookies */

	sapi_cgi_register_variables,	/* register server variables */
	sapi_cgi_log_message,			/* Log message */
	NULL,							/* Get request time */
	NULL,							/* Child terminate */

	STANDARD_SAPI_MODULE_PROPERTIES
};
/* }}} */

/* {{{ arginfo ext/standard/dl.c */
ZEND_BEGIN_ARG_INFO(arginfo_dl, 0)
	ZEND_ARG_INFO(0, extension_filename)
ZEND_END_ARG_INFO()
/* }}} */

static const zend_function_entry additional_functions[] = {
	ZEND_FE(dl, arginfo_dl)
	{NULL, NULL, NULL}
};

/* {{{ php_cgi_usage
 */
static void php_cgi_usage(char *argv0)
{
	char *prog;

	prog = strrchr(argv0, '/');
	if (prog) {
		prog++;
	} else {
		prog = "php";
	}

	php_printf(	"Usage: %s [-n] [-e] [-h] [-i] [-m] [-v] [-t] [-p <prefix>] [-g <pid>] [-c <file>] [-d foo[=bar]] [-y <file>] [-D] [-F]\n"
				"  -c <path>|<file> Look for php.ini file in this directory\n"
				"  -n               No php.ini file will be used\n"
				"  -d foo[=bar]     Define INI entry foo with value 'bar'\n"
				"  -e               Generate extended information for debugger/profiler\n"
				"  -h               This help\n"
				"  -i               PHP information\n"
				"  -m               Show compiled in modules\n"
				"  -v               Version number\n"
				"  -p, --prefix <dir>\n"
				"                   Specify alternative prefix path to FastCGI process manager (default: %s).\n"
				"  -g, --pid <file>\n"
				"                   Specify the PID file location.\n"
				"  -y, --fpm-config <file>\n"
				"                   Specify alternative path to FastCGI process manager config file.\n"
				"  -t, --test       Test FPM configuration and exit\n"
				"  -D, --daemonize  force to run in background, and ignore daemonize option from config file\n"
				"  -F, --nodaemonize\n"
				"                   force to stay in foreground, and ignore daemonize option from config file\n"
				"  -R, --allow-to-run-as-root\n"
				"                   Allow pool to run as root (disabled by default)\n",
				prog, PHP_PREFIX);
}
/* }}} */

/* {{{ is_valid_path
 *
 * some server configurations allow '..' to slip through in the
 * translated path.   We'll just refuse to handle such a path.
 */
static int is_valid_path(const char *path)
{
	const char *p;

	if (!path) {
		return 0;
	}
	p = strstr(path, "..");
	if (p) {
		if ((p == path || IS_SLASH(*(p-1))) &&
			(*(p+2) == 0 || IS_SLASH(*(p+2)))
		) {
			return 0;
		}
		while (1) {
			p = strstr(p+1, "..");
			if (!p) {
				break;
			}
			if (IS_SLASH(*(p-1)) &&
				(*(p+2) == 0 || IS_SLASH(*(p+2)))
			) {
					return 0;
			}
		}
	}
	return 1;
}
/* }}} */

/* {{{ init_request_info

  initializes request_info structure

  specificly in this section we handle proper translations
  for:

  PATH_INFO
	derived from the portion of the URI path following
	the script name but preceding any query data
	may be empty

  PATH_TRANSLATED
    derived by taking any path-info component of the
	request URI and performing any virtual-to-physical
	translation appropriate to map it onto the server's
	document repository structure

	empty if PATH_INFO is empty

	The env var PATH_TRANSLATED **IS DIFFERENT** than the
	request_info.path_translated variable, the latter should
	match SCRIPT_FILENAME instead.

  SCRIPT_NAME
    set to a URL path that could identify the CGI script
	rather than the interpreter.  PHP_SELF is set to this

  REQUEST_URI
    uri section following the domain:port part of a URI

  SCRIPT_FILENAME
    The virtual-to-physical translation of SCRIPT_NAME (as per
	PATH_TRANSLATED)

  These settings are documented at
  http://cgi-spec.golux.com/


  Based on the following URL request:

  http://localhost/info.php/test?a=b

  should produce, which btw is the same as if
  we were running under mod_cgi on apache (ie. not
  using ScriptAlias directives):

  PATH_INFO=/test
  PATH_TRANSLATED=/docroot/test
  SCRIPT_NAME=/info.php
  REQUEST_URI=/info.php/test?a=b
  SCRIPT_FILENAME=/docroot/info.php
  QUERY_STRING=a=b

  but what we get is (cgi/mod_fastcgi under apache):

  PATH_INFO=/info.php/test
  PATH_TRANSLATED=/docroot/info.php/test
  SCRIPT_NAME=/php/php-cgi  (from the Action setting I suppose)
  REQUEST_URI=/info.php/test?a=b
  SCRIPT_FILENAME=/path/to/php/bin/php-cgi  (Action setting translated)
  QUERY_STRING=a=b

  Comments in the code below refer to using the above URL in a request

 */
static void init_request_info(TSRMLS_D)
{
	char *env_script_filename = sapi_cgibin_getenv("SCRIPT_FILENAME", sizeof("SCRIPT_FILENAME") - 1 TSRMLS_CC);
	char *env_path_translated = sapi_cgibin_getenv("PATH_TRANSLATED", sizeof("PATH_TRANSLATED") - 1 TSRMLS_CC);
	char *script_path_translated = env_script_filename;
	char *ini;
	int apache_was_here = 0;

	/* some broken servers do not have script_filename or argv0
	 * an example, IIS configured in some ways.  then they do more
	 * broken stuff and set path_translated to the cgi script location */
	if (!script_path_translated && env_path_translated) {
		script_path_translated = env_path_translated;
	}

	/* initialize the defaults */
	SG(request_info).path_translated = NULL;
	SG(request_info).request_method = NULL;
	SG(request_info).proto_num = 1000;
	SG(request_info).query_string = NULL;
	SG(request_info).request_uri = NULL;
	SG(request_info).content_type = NULL;
	SG(request_info).content_length = 0;
	SG(sapi_headers).http_response_code = 200;

	/* script_path_translated being set is a good indication that
	 * we are running in a cgi environment, since it is always
	 * null otherwise.  otherwise, the filename
	 * of the script will be retreived later via argc/argv */
	if (script_path_translated) {
		const char *auth;
		char *content_length = sapi_cgibin_getenv("CONTENT_LENGTH", sizeof("CONTENT_LENGTH") - 1 TSRMLS_CC);
		char *content_type = sapi_cgibin_getenv("CONTENT_TYPE", sizeof("CONTENT_TYPE") - 1 TSRMLS_CC);
		char *env_path_info = sapi_cgibin_getenv("PATH_INFO", sizeof("PATH_INFO") - 1 TSRMLS_CC);
		char *env_script_name = sapi_cgibin_getenv("SCRIPT_NAME", sizeof("SCRIPT_NAME") - 1 TSRMLS_CC);

		/* Hack for buggy IIS that sets incorrect PATH_INFO */
		char *env_server_software = sapi_cgibin_getenv("SERVER_SOFTWARE", sizeof("SERVER_SOFTWARE") - 1 TSRMLS_CC);
		if (env_server_software &&
			env_script_name &&
			env_path_info &&
			strncmp(env_server_software, "Microsoft-IIS", sizeof("Microsoft-IIS") - 1) == 0 &&
			strncmp(env_path_info, env_script_name, strlen(env_script_name)) == 0
		) {
			env_path_info = _sapi_cgibin_putenv("ORIG_PATH_INFO", env_path_info TSRMLS_CC);
			env_path_info += strlen(env_script_name);
			if (*env_path_info == 0) {
				env_path_info = NULL;
			}
			env_path_info = _sapi_cgibin_putenv("PATH_INFO", env_path_info TSRMLS_CC);
		}

#define APACHE_PROXY_FCGI_PREFIX "proxy:fcgi://"
		/* Fix proxy URLs in SCRIPT_FILENAME generated by Apache mod_proxy_fcgi:
		 *     proxy:fcgi://localhost:9000/some-dir/info.php/test?foo=bar
		 * should be changed to:
		 *     /some-dir/info.php/test
		 * See: http://bugs.php.net/bug.php?id=54152
		 *      https://issues.apache.org/bugzilla/show_bug.cgi?id=50851
		 */
		if (env_script_filename &&
			strncasecmp(env_script_filename, APACHE_PROXY_FCGI_PREFIX, sizeof(APACHE_PROXY_FCGI_PREFIX) - 1) == 0) {
			/* advance to first character of hostname */
			char *p = env_script_filename + (sizeof(APACHE_PROXY_FCGI_PREFIX) - 1);
			while (*p != '\0' && *p != '/') {
				p++;	/* move past hostname and port */
			}
			if (*p != '\0') {
				/* Copy path portion in place to avoid memory leak.  Note
				 * that this also affects what script_path_translated points
				 * to. */
				memmove(env_script_filename, p, strlen(p) + 1);
				apache_was_here = 1;
			}
			/* ignore query string if sent by Apache (RewriteRule) */
			p = strchr(env_script_filename, '?');
			if (p) {
				*p =0;
			}
		}

		if (CGIG(fix_pathinfo)) {
			struct stat st;
			char *real_path = NULL;
			char *env_redirect_url = sapi_cgibin_getenv("REDIRECT_URL", sizeof("REDIRECT_URL") - 1 TSRMLS_CC);
			char *env_document_root = sapi_cgibin_getenv("DOCUMENT_ROOT", sizeof("DOCUMENT_ROOT") - 1 TSRMLS_CC);
			char *orig_path_translated = env_path_translated;
			char *orig_path_info = env_path_info;
			char *orig_script_name = env_script_name;
			char *orig_script_filename = env_script_filename;
			int script_path_translated_len;

			if (!env_document_root && PG(doc_root)) {
				env_document_root = _sapi_cgibin_putenv("DOCUMENT_ROOT", PG(doc_root) TSRMLS_CC);
				/* fix docroot */
				TRANSLATE_SLASHES(env_document_root);
			}

			if (env_path_translated != NULL && env_redirect_url != NULL &&
			    env_path_translated != script_path_translated &&
			    strcmp(env_path_translated, script_path_translated) != 0) {
				/*
				 * pretty much apache specific.  If we have a redirect_url
				 * then our script_filename and script_name point to the
				 * php executable
				 */
				script_path_translated = env_path_translated;
				/* we correct SCRIPT_NAME now in case we don't have PATH_INFO */
				env_script_name = env_redirect_url;
			}

#ifdef __riscos__
			/* Convert path to unix format*/
			__riscosify_control |= __RISCOSIFY_DONT_CHECK_DIR;
			script_path_translated = __unixify(script_path_translated, 0, NULL, 1, 0);
#endif

			/*
			 * if the file doesn't exist, try to extract PATH_INFO out
			 * of it by stat'ing back through the '/'
			 * this fixes url's like /info.php/test
			 */
			if (script_path_translated &&
				(script_path_translated_len = strlen(script_path_translated)) > 0 &&
				(script_path_translated[script_path_translated_len-1] == '/' ||
#ifdef PHP_WIN32
				script_path_translated[script_path_translated_len-1] == '\\' ||
#endif
				(real_path = tsrm_realpath(script_path_translated, NULL TSRMLS_CC)) == NULL)
			) {
				char *pt = estrndup(script_path_translated, script_path_translated_len);
				int len = script_path_translated_len;
				char *ptr;

				if (pt) {
					while ((ptr = strrchr(pt, '/')) || (ptr = strrchr(pt, '\\'))) {
						*ptr = 0;
						if (stat(pt, &st) == 0 && S_ISREG(st.st_mode)) {
							/*
							 * okay, we found the base script!
							 * work out how many chars we had to strip off;
							 * then we can modify PATH_INFO
							 * accordingly
							 *
							 * we now have the makings of
							 * PATH_INFO=/test
							 * SCRIPT_FILENAME=/docroot/info.php
							 *
							 * we now need to figure out what docroot is.
							 * if DOCUMENT_ROOT is set, this is easy, otherwise,
							 * we have to play the game of hide and seek to figure
							 * out what SCRIPT_NAME should be
							 */
							int ptlen = strlen(pt);
							int slen = len - ptlen;
							int pilen = env_path_info ? strlen(env_path_info) : 0;
							int tflag = 0;
							char *path_info;
							if (apache_was_here) {
								/* recall that PATH_INFO won't exist */
								path_info = script_path_translated + ptlen;
								tflag = (slen != 0 && (!orig_path_info || strcmp(orig_path_info, path_info) != 0));
							} else {
								path_info = env_path_info ? env_path_info + pilen - slen : NULL;
								tflag = (orig_path_info != path_info);
							}

							if (tflag) {
								if (orig_path_info) {
									char old;

									_sapi_cgibin_putenv("ORIG_PATH_INFO", orig_path_info TSRMLS_CC);
									old = path_info[0];
									path_info[0] = 0;
									if (!orig_script_name ||
										strcmp(orig_script_name, env_path_info) != 0) {
										if (orig_script_name) {
											_sapi_cgibin_putenv("ORIG_SCRIPT_NAME", orig_script_name TSRMLS_CC);
										}
										SG(request_info).request_uri = _sapi_cgibin_putenv("SCRIPT_NAME", env_path_info TSRMLS_CC);
									} else {
										SG(request_info).request_uri = orig_script_name;
									}
									path_info[0] = old;
								}
								env_path_info = _sapi_cgibin_putenv("PATH_INFO", path_info TSRMLS_CC);
							}
							if (!orig_script_filename ||
								strcmp(orig_script_filename, pt) != 0) {
								if (orig_script_filename) {
									_sapi_cgibin_putenv("ORIG_SCRIPT_FILENAME", orig_script_filename TSRMLS_CC);
								}
								script_path_translated = _sapi_cgibin_putenv("SCRIPT_FILENAME", pt TSRMLS_CC);
							}
							TRANSLATE_SLASHES(pt);

							/* figure out docroot
							 * SCRIPT_FILENAME minus SCRIPT_NAME
							 */
							if (env_document_root) {
								int l = strlen(env_document_root);
								int path_translated_len = 0;
								char *path_translated = NULL;

								if (l && env_document_root[l - 1] == '/') {
									--l;
								}

								/* we have docroot, so we should have:
								 * DOCUMENT_ROOT=/docroot
								 * SCRIPT_FILENAME=/docroot/info.php
								 */

								/* PATH_TRANSLATED = DOCUMENT_ROOT + PATH_INFO */
								path_translated_len = l + (env_path_info ? strlen(env_path_info) : 0);
								path_translated = (char *) emalloc(path_translated_len + 1);
								memcpy(path_translated, env_document_root, l);
								if (env_path_info) {
									memcpy(path_translated + l, env_path_info, (path_translated_len - l));
								}
								path_translated[path_translated_len] = '\0';
								if (orig_path_translated) {
									_sapi_cgibin_putenv("ORIG_PATH_TRANSLATED", orig_path_translated TSRMLS_CC);
								}
								env_path_translated = _sapi_cgibin_putenv("PATH_TRANSLATED", path_translated TSRMLS_CC);
								efree(path_translated);
							} else if (	env_script_name &&
										strstr(pt, env_script_name)
							) {
								/* PATH_TRANSLATED = PATH_TRANSLATED - SCRIPT_NAME + PATH_INFO */
								int ptlen = strlen(pt) - strlen(env_script_name);
								int path_translated_len = ptlen + (env_path_info ? strlen(env_path_info) : 0);
								char *path_translated = NULL;

								path_translated = (char *) emalloc(path_translated_len + 1);
								memcpy(path_translated, pt, ptlen);
								if (env_path_info) {
									memcpy(path_translated + ptlen, env_path_info, path_translated_len - ptlen);
								}
								path_translated[path_translated_len] = '\0';
								if (orig_path_translated) {
									_sapi_cgibin_putenv("ORIG_PATH_TRANSLATED", orig_path_translated TSRMLS_CC);
								}
								env_path_translated = _sapi_cgibin_putenv("PATH_TRANSLATED", path_translated TSRMLS_CC);
								efree(path_translated);
							}
							break;
						}
					}
				} else {
					ptr = NULL;
				}
				if (!ptr) {
					/*
					 * if we stripped out all the '/' and still didn't find
					 * a valid path... we will fail, badly. of course we would
					 * have failed anyway... we output 'no input file' now.
					 */
					if (orig_script_filename) {
						_sapi_cgibin_putenv("ORIG_SCRIPT_FILENAME", orig_script_filename TSRMLS_CC);
					}
					script_path_translated = _sapi_cgibin_putenv("SCRIPT_FILENAME", NULL TSRMLS_CC);
					SG(sapi_headers).http_response_code = 404;
				}
				if (!SG(request_info).request_uri) {
					if (!orig_script_name ||
						strcmp(orig_script_name, env_script_name) != 0) {
						if (orig_script_name) {
							_sapi_cgibin_putenv("ORIG_SCRIPT_NAME", orig_script_name TSRMLS_CC);
						}
						SG(request_info).request_uri = _sapi_cgibin_putenv("SCRIPT_NAME", env_script_name TSRMLS_CC);
					} else {
						SG(request_info).request_uri = orig_script_name;
					}
				}
				if (pt) {
					efree(pt);
				}
			} else {
				/* make sure path_info/translated are empty */
				if (!orig_script_filename ||
					(script_path_translated != orig_script_filename &&
					strcmp(script_path_translated, orig_script_filename) != 0)) {
					if (orig_script_filename) {
						_sapi_cgibin_putenv("ORIG_SCRIPT_FILENAME", orig_script_filename TSRMLS_CC);
					}
					script_path_translated = _sapi_cgibin_putenv("SCRIPT_FILENAME", script_path_translated TSRMLS_CC);
				}
				if (env_redirect_url) {
					if (orig_path_info) {
						_sapi_cgibin_putenv("ORIG_PATH_INFO", orig_path_info TSRMLS_CC);
						_sapi_cgibin_putenv("PATH_INFO", NULL TSRMLS_CC);
					}
					if (orig_path_translated) {
						_sapi_cgibin_putenv("ORIG_PATH_TRANSLATED", orig_path_translated TSRMLS_CC);
						_sapi_cgibin_putenv("PATH_TRANSLATED", NULL TSRMLS_CC);
					}
				}
				if (env_script_name != orig_script_name) {
					if (orig_script_name) {
						_sapi_cgibin_putenv("ORIG_SCRIPT_NAME", orig_script_name TSRMLS_CC);
					}
					SG(request_info).request_uri = _sapi_cgibin_putenv("SCRIPT_NAME", env_script_name TSRMLS_CC);
				} else {
					SG(request_info).request_uri = env_script_name;
				}
				efree(real_path);
			}
		} else {
			/* pre 4.3 behaviour, shouldn't be used but provides BC */
			if (env_path_info) {
				SG(request_info).request_uri = env_path_info;
			} else {
				SG(request_info).request_uri = env_script_name;
			}
			if (!CGIG(discard_path) && env_path_translated) {
				script_path_translated = env_path_translated;
			}
		}

		if (is_valid_path(script_path_translated)) {
			SG(request_info).path_translated = estrdup(script_path_translated);
		}

		SG(request_info).request_method = sapi_cgibin_getenv("REQUEST_METHOD", sizeof("REQUEST_METHOD") - 1 TSRMLS_CC);
		/* FIXME - Work out proto_num here */
		SG(request_info).query_string = sapi_cgibin_getenv("QUERY_STRING", sizeof("QUERY_STRING") - 1 TSRMLS_CC);
		SG(request_info).content_type = (content_type ? content_type : "" );
		SG(request_info).content_length = (content_length ? atol(content_length) : 0);

		/* The CGI RFC allows servers to pass on unvalidated Authorization data */
		auth = sapi_cgibin_getenv("HTTP_AUTHORIZATION", sizeof("HTTP_AUTHORIZATION") - 1 TSRMLS_CC);
		php_handle_auth_data(auth TSRMLS_CC);
	}

	/* INI stuff */
	ini = sapi_cgibin_getenv("PHP_VALUE", sizeof("PHP_VALUE") - 1 TSRMLS_CC);
	if (ini) {
		int mode = ZEND_INI_USER;
		char *tmp;
		spprintf(&tmp, 0, "%s\n", ini);
		zend_parse_ini_string(tmp, 1, ZEND_INI_SCANNER_NORMAL, (zend_ini_parser_cb_t)fastcgi_ini_parser, &mode TSRMLS_CC);
		efree(tmp);
	}

	ini = sapi_cgibin_getenv("PHP_ADMIN_VALUE", sizeof("PHP_ADMIN_VALUE") - 1 TSRMLS_CC);
	if (ini) {
		int mode = ZEND_INI_SYSTEM;
		char *tmp;
		spprintf(&tmp, 0, "%s\n", ini);
		zend_parse_ini_string(tmp, 1, ZEND_INI_SCANNER_NORMAL, (zend_ini_parser_cb_t)fastcgi_ini_parser, &mode TSRMLS_CC);
		efree(tmp);
	}
}
/* }}} */

static void fastcgi_ini_parser(zval *arg1, zval *arg2, zval *arg3, int callback_type, void *arg TSRMLS_DC) /* {{{ */
{
	int *mode = (int *)arg;
	char *key;
	char *value = NULL;
	struct key_value_s kv;

	if (!mode || !arg1) return;

	if (callback_type != ZEND_INI_PARSER_ENTRY) {
		zlog(ZLOG_ERROR, "Passing INI directive through FastCGI: only classic entries are allowed");
		return;
	}

	key = Z_STRVAL_P(arg1);

	if (!key || strlen(key) < 1) {
		zlog(ZLOG_ERROR, "Passing INI directive through FastCGI: empty key");
		return;
	}

	if (arg2) {
		value = Z_STRVAL_P(arg2);
	}

	if (!value) {
		zlog(ZLOG_ERROR, "Passing INI directive through FastCGI: empty value for key '%s'", key);
		return;
	}

	kv.key = key;
	kv.value = value;
	kv.next = NULL;
	if (fpm_php_apply_defines_ex(&kv, *mode) == -1) {
		zlog(ZLOG_ERROR, "Passing INI directive through FastCGI: unable to set '%s'", key);
	}
}
/* }}} */

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("cgi.rfc2616_headers",     "0",  PHP_INI_ALL,    OnUpdateBool,   rfc2616_headers, php_cgi_globals_struct, php_cgi_globals)
	STD_PHP_INI_ENTRY("cgi.nph",                 "0",  PHP_INI_ALL,    OnUpdateBool,   nph, php_cgi_globals_struct, php_cgi_globals)
	STD_PHP_INI_ENTRY("cgi.force_redirect",      "1",  PHP_INI_SYSTEM, OnUpdateBool,   force_redirect, php_cgi_globals_struct, php_cgi_globals)
	STD_PHP_INI_ENTRY("cgi.redirect_status_env", NULL, PHP_INI_SYSTEM, OnUpdateString, redirect_status_env, php_cgi_globals_struct, php_cgi_globals)
	STD_PHP_INI_ENTRY("cgi.fix_pathinfo",        "1",  PHP_INI_SYSTEM, OnUpdateBool,   fix_pathinfo, php_cgi_globals_struct, php_cgi_globals)
	STD_PHP_INI_ENTRY("cgi.discard_path",        "0",  PHP_INI_SYSTEM, OnUpdateBool,   discard_path, php_cgi_globals_struct, php_cgi_globals)
	STD_PHP_INI_ENTRY("fastcgi.logging",         "1",  PHP_INI_SYSTEM, OnUpdateBool,   fcgi_logging, php_cgi_globals_struct, php_cgi_globals)
	STD_PHP_INI_ENTRY("fastcgi.error_header",    NULL, PHP_INI_SYSTEM, OnUpdateString, error_header, php_cgi_globals_struct, php_cgi_globals)
	STD_PHP_INI_ENTRY("fpm.config",    NULL, PHP_INI_SYSTEM, OnUpdateString, fpm_config, php_cgi_globals_struct, php_cgi_globals)
PHP_INI_END()

/* {{{ php_cgi_globals_ctor
 */
static void php_cgi_globals_ctor(php_cgi_globals_struct *php_cgi_globals TSRMLS_DC)
{
	php_cgi_globals->rfc2616_headers = 0;
	php_cgi_globals->nph = 0;
	php_cgi_globals->force_redirect = 1;
	php_cgi_globals->redirect_status_env = NULL;
	php_cgi_globals->fix_pathinfo = 1;
	php_cgi_globals->discard_path = 0;
	php_cgi_globals->fcgi_logging = 1;
	zend_hash_init(&php_cgi_globals->user_config_cache, 0, NULL, (dtor_func_t) user_config_cache_entry_dtor, 1);
	php_cgi_globals->error_header = NULL;
	php_cgi_globals->fpm_config = NULL;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
static PHP_MINIT_FUNCTION(cgi)
{
#ifdef ZTS
	ts_allocate_id(&php_cgi_globals_id, sizeof(php_cgi_globals_struct), (ts_allocate_ctor) php_cgi_globals_ctor, NULL);
#else
	php_cgi_globals_ctor(&php_cgi_globals TSRMLS_CC);
#endif
	REGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
static PHP_MSHUTDOWN_FUNCTION(cgi)
{
	zend_hash_destroy(&CGIG(user_config_cache));

	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
static PHP_MINFO_FUNCTION(cgi)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "php-fpm", "active");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

PHP_FUNCTION(fastcgi_finish_request) /* {{{ */
{
	fcgi_request *request = (fcgi_request*) SG(server_context);

	if (request->fd >= 0) {

		php_output_end_all(TSRMLS_C);
		php_header(TSRMLS_C);

		fcgi_flush(request, 1);
		fcgi_close(request, 0, 0);
		RETURN_TRUE;
	}

	RETURN_FALSE;

}
/* }}} */

static const zend_function_entry cgi_fcgi_sapi_functions[] = {
	PHP_FE(fastcgi_finish_request,              NULL)
	{NULL, NULL, NULL}
};

static zend_module_entry cgi_module_entry = {
	STANDARD_MODULE_HEADER,
	"cgi-fcgi",
	cgi_fcgi_sapi_functions,
	PHP_MINIT(cgi),
	PHP_MSHUTDOWN(cgi),
	NULL,
	NULL,
	PHP_MINFO(cgi),
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

/* {{{ main
 */
int main(int argc, char *argv[])
{
	int exit_status = FPM_EXIT_OK;
	int cgi = 0, c, use_extended_info = 0;
	zend_file_handle file_handle;

	/* temporary locals */
	int orig_optind = php_optind;
	char *orig_optarg = php_optarg;
	int ini_entries_len = 0;
	/* end of temporary locals */

#ifdef ZTS
	void ***tsrm_ls;
#endif

	int max_requests = 500;
	int requests = 0;
	int fcgi_fd = 0;
	fcgi_request request;
	char *fpm_config = NULL;
	char *fpm_prefix = NULL;
	char *fpm_pid = NULL;
	int test_conf = 0;
	int force_daemon = -1;
	int php_information = 0;
	int php_allow_to_run_as_root = 0;

#ifdef HAVE_SIGNAL_H
#if defined(SIGPIPE) && defined(SIG_IGN)
	signal(SIGPIPE, SIG_IGN); /* ignore SIGPIPE in standalone mode so
								that sockets created via fsockopen()
								don't kill PHP if the remote site
								closes it.  in apache|apxs mode apache
								does that for us!  thies@thieso.net
								20000419 */
#endif
#endif

#ifdef ZTS
	tsrm_startup(1, 1, 0, NULL);
	tsrm_ls = ts_resource(0);
#endif

	sapi_startup(&cgi_sapi_module);
	cgi_sapi_module.php_ini_path_override = NULL;
	cgi_sapi_module.php_ini_ignore_cwd = 1;
	
	fcgi_init();

#ifdef PHP_WIN32
	_fmode = _O_BINARY; /* sets default for file streams to binary */
	setmode(_fileno(stdin),  O_BINARY);	/* make the stdio mode be binary */
	setmode(_fileno(stdout), O_BINARY);	/* make the stdio mode be binary */
	setmode(_fileno(stderr), O_BINARY);	/* make the stdio mode be binary */
#endif

	while ((c = php_getopt(argc, argv, OPTIONS, &php_optarg, &php_optind, 0, 2)) != -1) {
		switch (c) {
			case 'c':
				if (cgi_sapi_module.php_ini_path_override) {
					free(cgi_sapi_module.php_ini_path_override);
				}
				cgi_sapi_module.php_ini_path_override = strdup(php_optarg);
				break;

			case 'n':
				cgi_sapi_module.php_ini_ignore = 1;
				break;

			case 'd': {
				/* define ini entries on command line */
				int len = strlen(php_optarg);
				char *val;

				if ((val = strchr(php_optarg, '='))) {
					val++;
					if (!isalnum(*val) && *val != '"' && *val != '\'' && *val != '\0') {
						cgi_sapi_module.ini_entries = realloc(cgi_sapi_module.ini_entries, ini_entries_len + len + sizeof("\"\"\n\0"));
						memcpy(cgi_sapi_module.ini_entries + ini_entries_len, php_optarg, (val - php_optarg));
						ini_entries_len += (val - php_optarg);
						memcpy(cgi_sapi_module.ini_entries + ini_entries_len, "\"", 1);
						ini_entries_len++;
						memcpy(cgi_sapi_module.ini_entries + ini_entries_len, val, len - (val - php_optarg));
						ini_entries_len += len - (val - php_optarg);
						memcpy(cgi_sapi_module.ini_entries + ini_entries_len, "\"\n\0", sizeof("\"\n\0"));
						ini_entries_len += sizeof("\n\0\"") - 2;
					} else {
						cgi_sapi_module.ini_entries = realloc(cgi_sapi_module.ini_entries, ini_entries_len + len + sizeof("\n\0"));
						memcpy(cgi_sapi_module.ini_entries + ini_entries_len, php_optarg, len);
						memcpy(cgi_sapi_module.ini_entries + ini_entries_len + len, "\n\0", sizeof("\n\0"));
						ini_entries_len += len + sizeof("\n\0") - 2;
					}
				} else {
					cgi_sapi_module.ini_entries = realloc(cgi_sapi_module.ini_entries, ini_entries_len + len + sizeof("=1\n\0"));
					memcpy(cgi_sapi_module.ini_entries + ini_entries_len, php_optarg, len);
					memcpy(cgi_sapi_module.ini_entries + ini_entries_len + len, "=1\n\0", sizeof("=1\n\0"));
					ini_entries_len += len + sizeof("=1\n\0") - 2;
				}
				break;
			}

			case 'y':
				fpm_config = php_optarg;
				break;

			case 'p':
				fpm_prefix = php_optarg;
				break;

			case 'g':
				fpm_pid = php_optarg;
				break;

			case 'e': /* enable extended info output */
				use_extended_info = 1;
				break;

			case 't': 
				test_conf++;
				break;

			case 'm': /* list compiled in modules */
				cgi_sapi_module.startup(&cgi_sapi_module);
				php_output_activate(TSRMLS_C);
				SG(headers_sent) = 1;
				php_printf("[PHP Modules]\n");
				print_modules(TSRMLS_C);
				php_printf("\n[Zend Modules]\n");
				print_extensions(TSRMLS_C);
				php_printf("\n");
				php_output_end_all(TSRMLS_C);
				php_output_deactivate(TSRMLS_C);
				fcgi_shutdown();
				exit_status = FPM_EXIT_OK;
				goto out;

			case 'i': /* php info & quit */
				php_information = 1;
				break;

			case 'R': /* allow to run as root */
				php_allow_to_run_as_root = 1;
				break;

			case 'D': /* daemonize */
				force_daemon = 1;
				break;

			case 'F': /* nodaemonize */
				force_daemon = 0;
				break;

			default:
			case 'h':
			case '?':
				cgi_sapi_module.startup(&cgi_sapi_module);
				php_output_activate(TSRMLS_C);
				SG(headers_sent) = 1;
				php_cgi_usage(argv[0]);
				php_output_end_all(TSRMLS_C);
				php_output_deactivate(TSRMLS_C);
				fcgi_shutdown();
				exit_status = (c == 'h') ? FPM_EXIT_OK : FPM_EXIT_USAGE;
				goto out;

			case 'v': /* show php version & quit */
				cgi_sapi_module.startup(&cgi_sapi_module);
				if (php_request_startup(TSRMLS_C) == FAILURE) {
					SG(server_context) = NULL;
					php_module_shutdown(TSRMLS_C);
					return FPM_EXIT_SOFTWARE;
				}
				SG(headers_sent) = 1;
				SG(request_info).no_headers = 1;

#if ZEND_DEBUG
				php_printf("PHP %s (%s) (built: %s %s) (DEBUG)\nCopyright (c) 1997-2014 The PHP Group\n%s", PHP_VERSION, sapi_module.name, __DATE__,        __TIME__, get_zend_version());
#else
				php_printf("PHP %s (%s) (built: %s %s)\nCopyright (c) 1997-2014 The PHP Group\n%s", PHP_VERSION, sapi_module.name, __DATE__, __TIME__,      get_zend_version());
#endif
				php_request_shutdown((void *) 0);
				fcgi_shutdown();
				exit_status = FPM_EXIT_OK;
				goto out;
		}
	}

	if (php_information) {
		cgi_sapi_module.phpinfo_as_text = 1;
		cgi_sapi_module.startup(&cgi_sapi_module);
		if (php_request_startup(TSRMLS_C) == FAILURE) {
			SG(server_context) = NULL;
			php_module_shutdown(TSRMLS_C);
			return FPM_EXIT_SOFTWARE;
		}
		SG(headers_sent) = 1;
		SG(request_info).no_headers = 1;
		php_print_info(0xFFFFFFFF TSRMLS_CC);
		php_request_shutdown((void *) 0);
		fcgi_shutdown();
		exit_status = FPM_EXIT_OK;
		goto out;
	}

	/* No other args are permitted here as there is no interactive mode */
	if (argc != php_optind) {
		cgi_sapi_module.startup(&cgi_sapi_module);
		php_output_activate(TSRMLS_C);
		SG(headers_sent) = 1;
		php_cgi_usage(argv[0]);
		php_output_end_all(TSRMLS_C);
		php_output_deactivate(TSRMLS_C);
		fcgi_shutdown();
		exit_status = FPM_EXIT_USAGE;
		goto out;
	}

	php_optind = orig_optind;
	php_optarg = orig_optarg;

#ifdef ZTS
	SG(request_info).path_translated = NULL;
#endif

	cgi_sapi_module.additional_functions = additional_functions;
	cgi_sapi_module.executable_location = argv[0];

	/* startup after we get the above ini override se we get things right */
	if (cgi_sapi_module.startup(&cgi_sapi_module) == FAILURE) {
#ifdef ZTS
		tsrm_shutdown();
#endif
		return FPM_EXIT_SOFTWARE;
	}
	
	if (use_extended_info) {
		CG(compiler_options) |= ZEND_COMPILE_EXTENDED_INFO;
	}

	/* check force_cgi after startup, so we have proper output */
	if (cgi && CGIG(force_redirect)) {
		/* Apache will generate REDIRECT_STATUS,
		 * Netscape and redirect.so will generate HTTP_REDIRECT_STATUS.
		 * redirect.so and installation instructions available from
		 * http://www.koehntopp.de/php.
		 *   -- kk@netuse.de
		 */
		if (!getenv("REDIRECT_STATUS") &&
			!getenv ("HTTP_REDIRECT_STATUS") &&
			/* this is to allow a different env var to be configured
			 * in case some server does something different than above */
			(!CGIG(redirect_status_env) || !getenv(CGIG(redirect_status_env)))
		) {
			zend_try {
				SG(sapi_headers).http_response_code = 400;
				PUTS("<b>Security Alert!</b> The PHP CGI cannot be accessed directly.\n\n\
<p>This PHP CGI binary was compiled with force-cgi-redirect enabled.  This\n\
means that a page will only be served up if the REDIRECT_STATUS CGI variable is\n\
set, e.g. via an Apache Action directive.</p>\n\
<p>For more information as to <i>why</i> this behaviour exists, see the <a href=\"http://php.net/security.cgi-bin\">\
manual page for CGI security</a>.</p>\n\
<p>For more information about changing this behaviour or re-enabling this webserver,\n\
consult the installation file that came with this distribution, or visit \n\
<a href=\"http://php.net/install.windows\">the manual page</a>.</p>\n");
			} zend_catch {
			} zend_end_try();
#if defined(ZTS) && !defined(PHP_DEBUG)
			/* XXX we're crashing here in msvc6 debug builds at
			 * php_message_handler_for_zend:839 because
			 * SG(request_info).path_translated is an invalid pointer.
			 * It still happens even though I set it to null, so something
			 * weird is going on.
			 */
			tsrm_shutdown();
#endif
			return FPM_EXIT_SOFTWARE;
		}
	}

	if (0 > fpm_init(argc, argv, fpm_config ? fpm_config : CGIG(fpm_config), fpm_prefix, fpm_pid, test_conf, php_allow_to_run_as_root, force_daemon)) {

		if (fpm_globals.send_config_pipe[1]) {
			int writeval = 0;
			zlog(ZLOG_DEBUG, "Sending \"0\" (error) to parent via fd=%d", fpm_globals.send_config_pipe[1]);
			write(fpm_globals.send_config_pipe[1], &writeval, sizeof(writeval));
			close(fpm_globals.send_config_pipe[1]);
		}
		return FPM_EXIT_CONFIG;
	}

	if (fpm_globals.send_config_pipe[1]) {
		int writeval = 1;
		zlog(ZLOG_DEBUG, "Sending \"1\" (OK) to parent via fd=%d", fpm_globals.send_config_pipe[1]);
		write(fpm_globals.send_config_pipe[1], &writeval, sizeof(writeval));
		close(fpm_globals.send_config_pipe[1]);
	}
	fpm_is_running = 1;

	fcgi_fd = fpm_run(&max_requests);
	parent = 0;

	/* onced forked tell zlog to also send messages through sapi_cgi_log_fastcgi() */
	zlog_set_external_logger(sapi_cgi_log_fastcgi);

	/* make php call us to get _ENV vars */
	php_php_import_environment_variables = php_import_environment_variables;
	php_import_environment_variables = cgi_php_import_environment_variables;

	/* library is already initialized, now init our request */
	fcgi_init_request(&request, fcgi_fd);

	zend_first_try {
		while (fcgi_accept_request(&request) >= 0) {
			request_body_fd = -1;
			SG(server_context) = (void *) &request;
			init_request_info(TSRMLS_C);
			CG(interactive) = 0;
			char *primary_script = NULL;

			fpm_request_info();

			/* request startup only after we've done all we can to
			 *            get path_translated */
			if (php_request_startup(TSRMLS_C) == FAILURE) {
				fcgi_finish_request(&request, 1);
				SG(server_context) = NULL;
				php_module_shutdown(TSRMLS_C);
				return FPM_EXIT_SOFTWARE;
			}

			/* check if request_method has been sent.
			 * if not, it's certainly not an HTTP over fcgi request */
			if (!SG(request_info).request_method) {
				goto fastcgi_request_done;
			}

			if (fpm_status_handle_request(TSRMLS_C)) {
				goto fastcgi_request_done;
			}

			/* If path_translated is NULL, terminate here with a 404 */
			if (!SG(request_info).path_translated) {
				zend_try {
					zlog(ZLOG_DEBUG, "Primary script unknown");
					SG(sapi_headers).http_response_code = 404;
					PUTS("File not found.\n");
				} zend_catch {
				} zend_end_try();
				goto fastcgi_request_done;
			}

			if (fpm_php_limit_extensions(SG(request_info).path_translated)) {
				SG(sapi_headers).http_response_code = 403;
				PUTS("Access denied.\n");
				goto fastcgi_request_done;
			}

			/* 
			 * have to duplicate SG(request_info).path_translated to be able to log errrors
			 * php_fopen_primary_script seems to delete SG(request_info).path_translated on failure
			 */
			primary_script = estrdup(SG(request_info).path_translated);

			/* path_translated exists, we can continue ! */
			if (php_fopen_primary_script(&file_handle TSRMLS_CC) == FAILURE) {
				zend_try {
					zlog(ZLOG_ERROR, "Unable to open primary script: %s (%s)", primary_script, strerror(errno));
					if (errno == EACCES) {
						SG(sapi_headers).http_response_code = 403;
						PUTS("Access denied.\n");
					} else {
						SG(sapi_headers).http_response_code = 404;
						PUTS("No input file specified.\n");
					}
				} zend_catch {
				} zend_end_try();
				/* we want to serve more requests if this is fastcgi
				 * so cleanup and continue, request shutdown is
				 * handled later */

				goto fastcgi_request_done;
			}

			fpm_request_executing();

			php_execute_script(&file_handle TSRMLS_CC);

fastcgi_request_done:
			if (primary_script) {
				efree(primary_script);
			}

			if (request_body_fd != -1) {
				close(request_body_fd);
			}
			request_body_fd = -2;

			if (EG(exit_status) == 255) {
				if (CGIG(error_header) && *CGIG(error_header)) {
					sapi_header_line ctr = {0};

					ctr.line = CGIG(error_header);
					ctr.line_len = strlen(CGIG(error_header));
					sapi_header_op(SAPI_HEADER_REPLACE, &ctr TSRMLS_CC);
				}
			}

			fpm_request_end(TSRMLS_C);
			fpm_log_write(NULL TSRMLS_CC);

			STR_FREE(SG(request_info).path_translated);
			SG(request_info).path_translated = NULL;

			php_request_shutdown((void *) 0);

			requests++;
			if (max_requests && (requests == max_requests)) {
				fcgi_finish_request(&request, 1);
				break;
			}
			/* end of fastcgi loop */
		}
		fcgi_shutdown();

		if (cgi_sapi_module.php_ini_path_override) {
			free(cgi_sapi_module.php_ini_path_override);
		}
		if (cgi_sapi_module.ini_entries) {
			free(cgi_sapi_module.ini_entries);
		}
	} zend_catch {
		exit_status = FPM_EXIT_SOFTWARE;
	} zend_end_try();

out:

	SG(server_context) = NULL;
	php_module_shutdown(TSRMLS_C);

	if (parent) {
		sapi_shutdown();
	}

#ifdef ZTS
	tsrm_shutdown();
#endif

#if defined(PHP_WIN32) && ZEND_DEBUG && 0
	_CrtDumpMemoryLeaks();
#endif

	return exit_status;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
