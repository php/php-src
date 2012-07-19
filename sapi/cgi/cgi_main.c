/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
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

/* $Id$ */

#include "php.h"
#include "php_globals.h"
#include "php_variables.h"
#include "zend_modules.h"

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

#include "zend.h"
#include "zend_extensions.h"
#include "php_ini.h"
#include "php_globals.h"
#include "php_main.h"
#include "fopen_wrappers.h"
#include "ext/standard/php_standard.h"
#include "ext/standard/url.h"

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

#ifndef PHP_WIN32
/* XXX this will need to change later when threaded fastcgi is implemented.  shane */
struct sigaction act, old_term, old_quit, old_int;
#endif

static void (*php_php_import_environment_variables)(zval *array_ptr TSRMLS_DC);

#ifndef PHP_WIN32
/* these globals used for forking children on unix systems */
/**
 * Number of child processes that will get created to service requests
 */
static int children = 0;


/**
 * Set to non-zero if we are the parent process
 */
static int parent = 1;

/* Did parent received exit signals SIG_TERM/SIG_INT/SIG_QUIT */
static int exit_signal = 0;

/* Is Parent waiting for children to exit */
static int parent_waiting = 0;

/**
 * Process group
 */
static pid_t pgroup;
#endif

#define PHP_MODE_STANDARD	1
#define PHP_MODE_HIGHLIGHT	2
#define PHP_MODE_INDENT		3
#define PHP_MODE_LINT		4
#define PHP_MODE_STRIP		5

static char *php_optarg = NULL;
static int php_optind = 1;
static zend_module_entry cgi_module_entry;

static const opt_struct OPTIONS[] = {
	{'a', 0, "interactive"},
	{'b', 1, "bindpath"},
	{'C', 0, "no-chdir"},
	{'c', 1, "php-ini"},
	{'d', 1, "define"},
	{'e', 0, "profile-info"},
	{'f', 1, "file"},
	{'h', 0, "help"},
	{'i', 0, "info"},
	{'l', 0, "syntax-check"},
	{'m', 0, "modules"},
	{'n', 0, "no-php-ini"},
	{'q', 0, "no-header"},
	{'s', 0, "syntax-highlight"},
	{'s', 0, "syntax-highlighting"},
	{'w', 0, "strip"},
	{'?', 0, "usage"},/* help alias (both '?' and 'usage') */
	{'v', 0, "version"},
	{'z', 1, "zend-extension"},
 	{'T', 1, "timing"},
	{'-', 0, NULL} /* end of args */
};

typedef struct _php_cgi_globals_struct {
	zend_bool rfc2616_headers;
	zend_bool nph;
	zend_bool check_shebang_line;
	zend_bool fix_pathinfo;
	zend_bool force_redirect;
	zend_bool discard_path;
	zend_bool fcgi_logging;
	char *redirect_status_env;
#ifdef PHP_WIN32
	zend_bool impersonate;
#endif
	HashTable user_config_cache;
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

static inline size_t sapi_cgi_single_write(const char *str, uint str_length TSRMLS_DC)
{
#ifdef PHP_WRITE_STDOUT
	long ret;

	ret = write(STDOUT_FILENO, str, str_length);
	if (ret <= 0) return 0;
	return ret;
#else
	size_t ret;

	ret = fwrite(str, 1, MIN(str_length, 16384), stdout);
	return ret;
#endif
}

static int sapi_cgi_ub_write(const char *str, uint str_length TSRMLS_DC)
{
	const char *ptr = str;
	uint remaining = str_length;
	size_t ret;

	while (remaining > 0) {
		ret = sapi_cgi_single_write(ptr, remaining TSRMLS_CC);
		if (!ret) {
			php_handle_aborted_connection();
			return str_length - remaining;
		}
		ptr += ret;
		remaining -= ret;
	}

	return str_length;
}

static int sapi_fcgi_ub_write(const char *str, uint str_length TSRMLS_DC)
{
	const char *ptr = str;
	uint remaining = str_length;
	fcgi_request *request = (fcgi_request*) SG(server_context);

	while (remaining > 0) {
		long ret = fcgi_write(request, FCGI_STDOUT, ptr, remaining);

		if (ret <= 0) {
			php_handle_aborted_connection();
			return str_length - remaining;
		}
		ptr += ret;
		remaining -= ret;
	}

	return str_length;
}

static void sapi_cgi_flush(void *server_context)
{
	if (fflush(stdout) == EOF) {
		php_handle_aborted_connection();
	}
}

static void sapi_fcgi_flush(void *server_context)
{
	fcgi_request *request = (fcgi_request*) server_context;

	if (
#ifndef PHP_WIN32
		!parent &&
#endif
		request && !fcgi_flush(request, 0)) {

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
	{500, "Internal Server Error"},
	{501, "Not Implemented"},
	{502, "Bad Gateway"},
	{503, "Service Unavailable"},
	{504, "Gateway Time-out"},
	{505, "HTTP Version not supported"},
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
					if (h->header_len > sizeof("Status:")-1 &&
						strncasecmp(h->header, "Status:", sizeof("Status:")-1) == 0
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
			if (h->header_len > sizeof("Status:")-1 && 
				strncasecmp(h->header, "Status:", sizeof("Status:")-1) == 0
			) {
				if (!ignore_status) {
					ignore_status = 1;
					PHPWRITE_H(h->header, h->header_len);
					PHPWRITE_H("\r\n", 2);
				}
			} else if (response_status == 304 && h->header_len > sizeof("Content-Type:")-1 &&
				strncasecmp(h->header, "Content-Type:", sizeof("Content-Type:")-1) == 0
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

	count_bytes = MIN(count_bytes, (uint) SG(request_info).content_length - SG(read_post_bytes));
	while (read_bytes < count_bytes) {
		tmp_read_bytes = read(STDIN_FILENO, buffer + read_bytes, count_bytes - read_bytes);
		if (tmp_read_bytes <= 0) {
			break;
		}
		read_bytes += tmp_read_bytes;
	}
	return read_bytes;
}

static int sapi_fcgi_read_post(char *buffer, uint count_bytes TSRMLS_DC)
{
	uint read_bytes = 0;
	int tmp_read_bytes;
	fcgi_request *request = (fcgi_request*) SG(server_context);

	count_bytes = MIN(count_bytes, (uint) SG(request_info).content_length - SG(read_post_bytes));
	while (read_bytes < count_bytes) {
		tmp_read_bytes = fcgi_read(request, buffer + read_bytes, count_bytes - read_bytes);
		if (tmp_read_bytes <= 0) {
			break;
		}
		read_bytes += tmp_read_bytes;
	}
	return read_bytes;
}

static char *sapi_cgi_getenv(char *name, size_t name_len TSRMLS_DC)
{
	return getenv(name);
}

static char *sapi_fcgi_getenv(char *name, size_t name_len TSRMLS_DC)
{
	/* when php is started by mod_fastcgi, no regular environment
	 * is provided to PHP.  It is always sent to PHP at the start
	 * of a request.  So we have to do our own lookup to get env
	 * vars.  This could probably be faster somehow.  */
	fcgi_request *request = (fcgi_request*) SG(server_context);
	char *ret = fcgi_getenv(request, name, name_len);

	if (ret) return ret;
	/*  if cgi, or fastcgi and not found in fcgi env
		check the regular environment */
	return getenv(name);
}

static char *_sapi_cgi_putenv(char *name, int name_len, char *value)
{
#if !HAVE_SETENV || !HAVE_UNSETENV
	int len;
	char *buf;
#endif

#if HAVE_SETENV
	if (value) {
		setenv(name, value, 1);
	}
#endif
#if HAVE_UNSETENV
	if (!value) {
		unsetenv(name);
	}
#endif

#if !HAVE_SETENV || !HAVE_UNSETENV
	/*  if cgi, or fastcgi and not found in fcgi env
		check the regular environment
		this leaks, but it's only cgi anyway, we'll fix
		it for 5.0
	*/
	len = name_len + (value ? strlen(value) : 0) + sizeof("=") + 2;
	buf = (char *) malloc(len);
	if (buf == NULL) {
		return getenv(name);
	}
#endif
#if !HAVE_SETENV
	if (value) {
		len = slprintf(buf, len - 1, "%s=%s", name, value);
		putenv(buf);
	}
#endif
#if !HAVE_UNSETENV
	if (!value) {
		len = slprintf(buf, len - 1, "%s=", name);
		putenv(buf);
	}
#endif
	return getenv(name);
}

static char *sapi_cgi_read_cookies(TSRMLS_D)
{
	return getenv("HTTP_COOKIE");
}

static char *sapi_fcgi_read_cookies(TSRMLS_D)
{
	fcgi_request *request = (fcgi_request*) SG(server_context);

	return FCGI_GETENV(request, "HTTP_COOKIE");
}

static void cgi_php_load_env_var(char *var, unsigned int var_len, char *val, unsigned int val_len, void *arg TSRMLS_DC)
{
	zval *array_ptr = (zval*)arg;	
	int filter_arg = (array_ptr == PG(http_globals)[TRACK_VARS_ENV])?PARSE_ENV:PARSE_SERVER;
	unsigned int new_val_len;

	if (sapi_module.input_filter(filter_arg, var, &val, strlen(val), &new_val_len TSRMLS_CC)) {
		php_register_variable_safe(var, val, new_val_len, array_ptr TSRMLS_CC);
	}
}

static void cgi_php_import_environment_variables(zval *array_ptr TSRMLS_DC)
{
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

	if (fcgi_is_fastcgi()) {
		fcgi_request *request = (fcgi_request*) SG(server_context);
		fcgi_loadenv(request, cgi_php_load_env_var, array_ptr TSRMLS_CC);
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
		char *path_info;
		int free_php_self;
		ALLOCA_FLAG(use_heap)

		if (fcgi_is_fastcgi()) {
			fcgi_request *request = (fcgi_request*) SG(server_context);

			path_info = FCGI_GETENV(request, "PATH_INFO");
		} else {
			path_info = getenv("PATH_INFO");
		}

		if (path_info) {
			unsigned int path_info_len = strlen(path_info);

			if (script_name) {
				unsigned int script_name_len = strlen(script_name);

				php_self_len = script_name_len + path_info_len;
				php_self = do_alloca(php_self_len + 1, use_heap);
				memcpy(php_self, script_name, script_name_len + 1);
				memcpy(php_self + script_name_len, path_info, path_info_len + 1);
				free_php_self = 1;
			}  else {
				php_self = path_info;
				php_self_len = path_info_len;
				free_php_self = 0;
			}
		} else if (script_name) {
			php_self = script_name;
			php_self_len = strlen(script_name);
			free_php_self = 0;
		} else {
			php_self = "";
			php_self_len = 0;
			free_php_self = 0;
		}

		/* Build the special-case PHP_SELF variable for the CGI version */
		if (sapi_module.input_filter(PARSE_SERVER, "PHP_SELF", &php_self, php_self_len, &php_self_len TSRMLS_CC)) {
			php_register_variable_safe("PHP_SELF", php_self, php_self_len, track_vars_array TSRMLS_CC);
		}
		if (free_php_self) {
			free_alloca(php_self, use_heap);
		}
	} else {
		php_self = SG(request_info).request_uri ? SG(request_info).request_uri : "";
		php_self_len = strlen(php_self);
		if (sapi_module.input_filter(PARSE_SERVER, "PHP_SELF", &php_self, php_self_len, &php_self_len TSRMLS_CC)) {
			php_register_variable_safe("PHP_SELF", php_self, php_self_len, track_vars_array TSRMLS_CC);
		}
	}
}

static void sapi_cgi_log_message(char *message TSRMLS_DC)
{
	if (fcgi_is_fastcgi() && CGIG(fcgi_logging)) {
		fcgi_request *request;

		request = (fcgi_request*) SG(server_context);
		if (request) {
			int len = strlen(message);
			char *buf = malloc(len+2);

			memcpy(buf, message, len);
			memcpy(buf + len, "\n", sizeof("\n"));
			fcgi_write(request, FCGI_STDERR, buf, len+1);
			free(buf);
		} else {
			fprintf(stderr, "%s\n", message);
		}
		/* ignore return code */
	} else {
		fprintf(stderr, "%s\n", message);
	}
}

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
		char *real_path = NULL;
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

		if (real_path) {
			free(real_path);
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
		if (fcgi_is_fastcgi()) {
			fcgi_request *request = (fcgi_request*) SG(server_context);

			server_name = FCGI_GETENV(request, "SERVER_NAME");
		} else {
			server_name = getenv("SERVER_NAME");
		}
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
			if (fcgi_is_fastcgi()) {
				fcgi_request *request = (fcgi_request*) SG(server_context);

				doc_root = FCGI_GETENV(request, "DOCUMENT_ROOT");
			} else {
				doc_root = getenv("DOCUMENT_ROOT");
			}

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
				
#ifdef PHP_WIN32
				efree(doc_root);
#endif
			}
		}

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
		if (fcgi_is_fastcgi()) {
			if (
#ifndef PHP_WIN32
				!parent &&
#endif
				!fcgi_finish_request((fcgi_request*)SG(server_context), 0)) {
				php_handle_aborted_connection();
			}
		} else {
			sapi_cgi_flush(SG(server_context));
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
	"cgi-fcgi",						/* name */
	"CGI/FastCGI",					/* pretty name */

	php_cgi_startup,				/* startup */
	php_module_shutdown_wrapper,	/* shutdown */

	sapi_cgi_activate,				/* activate */
	sapi_cgi_deactivate,			/* deactivate */

	sapi_cgi_ub_write,				/* unbuffered write */
	sapi_cgi_flush,					/* flush */
	NULL,							/* get uid */
	sapi_cgi_getenv,				/* getenv */

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

	php_printf(	"Usage: %s [-q] [-h] [-s] [-v] [-i] [-f <file>]\n"
				"       %s <file> [args...]\n"
				"  -a               Run interactively\n"
				"  -b <address:port>|<port> Bind Path for external FASTCGI Server mode\n"
				"  -C               Do not chdir to the script's directory\n"
				"  -c <path>|<file> Look for php.ini file in this directory\n"
				"  -n               No php.ini file will be used\n"
				"  -d foo[=bar]     Define INI entry foo with value 'bar'\n"
				"  -e               Generate extended information for debugger/profiler\n"
				"  -f <file>        Parse <file>.  Implies `-q'\n"
				"  -h               This help\n"
				"  -i               PHP information\n"
				"  -l               Syntax check only (lint)\n"
				"  -m               Show compiled in modules\n"
				"  -q               Quiet-mode.  Suppress HTTP Header output.\n"
				"  -s               Display colour syntax highlighted source.\n"
				"  -v               Version number\n"
				"  -w               Display source with stripped comments and whitespace.\n"
				"  -z <file>        Load Zend extension <file>.\n"
				"  -T <count>       Measure execution time of script repeated <count> times.\n",
				prog, prog);
}
/* }}} */

/* {{{ is_valid_path
 *
 * some server configurations allow '..' to slip through in the
 * translated path.   We'll just refuse to handle such a path.
 */
static int is_valid_path(const char *path)
{
	const char *p = path;

	if (UNEXPECTED(!p)) {
		return 0;
	}
	if (UNEXPECTED(*p == '.') && *(p+1) == '.' && (!*(p+2) || IS_SLASH(*(p+2)))) {
		return 0;
	}
	while (*p) {
		if (IS_SLASH(*p)) {
			p++;
			if (UNEXPECTED(*p == '.')) {
				p++;
				if (UNEXPECTED(*p == '.')) {
					p++;
					if (UNEXPECTED(!*p) || UNEXPECTED(IS_SLASH(*p))) {
						return 0;
					}											
				}
			}
		}
		p++;
	}
	return 1;
}
/* }}} */

#define CGI_GETENV(name) \
	((request) ? \
		FCGI_GETENV(request, name) : \
    	getenv(name))

#define CGI_PUTENV(name, value) \
	((request) ? \
		FCGI_PUTENV(request, name, value) : \
		_sapi_cgi_putenv(name, sizeof(name)-1, value))

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
static void init_request_info(fcgi_request *request TSRMLS_DC)
{
	char *env_script_filename = CGI_GETENV("SCRIPT_FILENAME");
	char *env_path_translated = CGI_GETENV("PATH_TRANSLATED");
	char *script_path_translated = env_script_filename;

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
		char *content_length = CGI_GETENV("CONTENT_LENGTH");
		char *content_type = CGI_GETENV("CONTENT_TYPE");
		char *env_path_info = CGI_GETENV("PATH_INFO");
		char *env_script_name = CGI_GETENV("SCRIPT_NAME");

#ifdef PHP_WIN32
		/* Hack for buggy IIS that sets incorrect PATH_INFO */
		char *env_server_software = CGI_GETENV("SERVER_SOFTWARE");

		if (env_server_software &&
			env_script_name &&
			env_path_info &&
			strncmp(env_server_software, "Microsoft-IIS", sizeof("Microsoft-IIS")-1) == 0 &&
			strncmp(env_path_info, env_script_name, strlen(env_script_name)) == 0
		) {
			env_path_info = CGI_PUTENV("ORIG_PATH_INFO", env_path_info);
			env_path_info += strlen(env_script_name);
			if (*env_path_info == 0) {
				env_path_info = NULL;
			}
			env_path_info = CGI_PUTENV("PATH_INFO", env_path_info);
		}
#endif

		if (CGIG(fix_pathinfo)) {
			struct stat st;
			char *real_path = NULL;
			char *env_redirect_url = CGI_GETENV("REDIRECT_URL");
			char *env_document_root = CGI_GETENV("DOCUMENT_ROOT");
			char *orig_path_translated = env_path_translated;
			char *orig_path_info = env_path_info;
			char *orig_script_name = env_script_name;
			char *orig_script_filename = env_script_filename;
			int script_path_translated_len;

			if (!env_document_root && PG(doc_root)) {
				env_document_root = CGI_PUTENV("DOCUMENT_ROOT", PG(doc_root));
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
						int slen = len - strlen(pt);
						int pilen = env_path_info ? strlen(env_path_info) : 0;
						char *path_info = env_path_info ? env_path_info + pilen - slen : NULL;

						if (orig_path_info != path_info) {
							if (orig_path_info) {
								char old;

								CGI_PUTENV("ORIG_PATH_INFO", orig_path_info);
								old = path_info[0];
								path_info[0] = 0;
								if (!orig_script_name ||
									strcmp(orig_script_name, env_path_info) != 0) {
									if (orig_script_name) {
										CGI_PUTENV("ORIG_SCRIPT_NAME", orig_script_name);
									}
									SG(request_info).request_uri = CGI_PUTENV("SCRIPT_NAME", env_path_info);
								} else {
									SG(request_info).request_uri = orig_script_name;
								}
								path_info[0] = old;
							}
							env_path_info = CGI_PUTENV("PATH_INFO", path_info);
						}
						if (!orig_script_filename ||
							strcmp(orig_script_filename, pt) != 0) {
							if (orig_script_filename) {
								CGI_PUTENV("ORIG_SCRIPT_FILENAME", orig_script_filename);
							}
							script_path_translated = CGI_PUTENV("SCRIPT_FILENAME", pt);
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
								CGI_PUTENV("ORIG_PATH_TRANSLATED", orig_path_translated);
							}
							env_path_translated = CGI_PUTENV("PATH_TRANSLATED", path_translated);
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
								CGI_PUTENV("ORIG_PATH_TRANSLATED", orig_path_translated);
							}
							env_path_translated = CGI_PUTENV("PATH_TRANSLATED", path_translated);
							efree(path_translated);
						}
						break;
					}
				}
				if (!ptr) {
					/*
					 * if we stripped out all the '/' and still didn't find
					 * a valid path... we will fail, badly. of course we would
					 * have failed anyway... we output 'no input file' now.
					 */
					if (orig_script_filename) {
						CGI_PUTENV("ORIG_SCRIPT_FILENAME", orig_script_filename);
					}
					script_path_translated = CGI_PUTENV("SCRIPT_FILENAME", NULL);
					SG(sapi_headers).http_response_code = 404;
				}
				if (!SG(request_info).request_uri) {
					if (!orig_script_name ||
						strcmp(orig_script_name, env_script_name) != 0) {
						if (orig_script_name) {
							CGI_PUTENV("ORIG_SCRIPT_NAME", orig_script_name);
						}
						SG(request_info).request_uri = CGI_PUTENV("SCRIPT_NAME", env_script_name);
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
						CGI_PUTENV("ORIG_SCRIPT_FILENAME", orig_script_filename);
					}
					script_path_translated = CGI_PUTENV("SCRIPT_FILENAME", script_path_translated);
				}
				if (env_redirect_url) {
					if (orig_path_info) {
						CGI_PUTENV("ORIG_PATH_INFO", orig_path_info);
						CGI_PUTENV("PATH_INFO", NULL);
					}
					if (orig_path_translated) {
						CGI_PUTENV("ORIG_PATH_TRANSLATED", orig_path_translated);
						CGI_PUTENV("PATH_TRANSLATED", NULL);
					}
				}
				if (env_script_name != orig_script_name) {
					if (orig_script_name) {
						CGI_PUTENV("ORIG_SCRIPT_NAME", orig_script_name);
					}
					SG(request_info).request_uri = CGI_PUTENV("SCRIPT_NAME", env_script_name);
				} else {
					SG(request_info).request_uri = env_script_name;
				}
				free(real_path);
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

		SG(request_info).request_method = CGI_GETENV("REQUEST_METHOD");
		/* FIXME - Work out proto_num here */
		SG(request_info).query_string = CGI_GETENV("QUERY_STRING");
		SG(request_info).content_type = (content_type ? content_type : "" );
		SG(request_info).content_length = (content_length ? atol(content_length) : 0);

		/* The CGI RFC allows servers to pass on unvalidated Authorization data */
		auth = CGI_GETENV("HTTP_AUTHORIZATION");
		php_handle_auth_data(auth TSRMLS_CC);
	}
}
/* }}} */

#ifndef PHP_WIN32
/**
 * Clean up child processes upon exit
 */
void fastcgi_cleanup(int signal)
{
#ifdef DEBUG_FASTCGI
	fprintf(stderr, "FastCGI shutdown, pid %d\n", getpid());
#endif

	sigaction(SIGTERM, &old_term, 0);

	/* Kill all the processes in our process group */
	kill(-pgroup, SIGTERM);

	if (parent && parent_waiting) {
		exit_signal = 1;
	} else {
		exit(0);
	}
}
#endif

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("cgi.rfc2616_headers",     "0",  PHP_INI_ALL,    OnUpdateBool,   rfc2616_headers, php_cgi_globals_struct, php_cgi_globals)
	STD_PHP_INI_ENTRY("cgi.nph",                 "0",  PHP_INI_ALL,    OnUpdateBool,   nph, php_cgi_globals_struct, php_cgi_globals)
	STD_PHP_INI_ENTRY("cgi.check_shebang_line",  "1",  PHP_INI_SYSTEM, OnUpdateBool,   check_shebang_line, php_cgi_globals_struct, php_cgi_globals)
	STD_PHP_INI_ENTRY("cgi.force_redirect",      "1",  PHP_INI_SYSTEM, OnUpdateBool,   force_redirect, php_cgi_globals_struct, php_cgi_globals)
	STD_PHP_INI_ENTRY("cgi.redirect_status_env", NULL, PHP_INI_SYSTEM, OnUpdateString, redirect_status_env, php_cgi_globals_struct, php_cgi_globals)
	STD_PHP_INI_ENTRY("cgi.fix_pathinfo",        "1",  PHP_INI_SYSTEM, OnUpdateBool,   fix_pathinfo, php_cgi_globals_struct, php_cgi_globals)
	STD_PHP_INI_ENTRY("cgi.discard_path",        "0",  PHP_INI_SYSTEM, OnUpdateBool,   discard_path, php_cgi_globals_struct, php_cgi_globals)
	STD_PHP_INI_ENTRY("fastcgi.logging",         "1",  PHP_INI_SYSTEM, OnUpdateBool,   fcgi_logging, php_cgi_globals_struct, php_cgi_globals)
#ifdef PHP_WIN32
	STD_PHP_INI_ENTRY("fastcgi.impersonate",     "0",  PHP_INI_SYSTEM, OnUpdateBool,   impersonate, php_cgi_globals_struct, php_cgi_globals)
#endif
PHP_INI_END()

/* {{{ php_cgi_globals_ctor
 */
static void php_cgi_globals_ctor(php_cgi_globals_struct *php_cgi_globals TSRMLS_DC)
{
	php_cgi_globals->rfc2616_headers = 0;
	php_cgi_globals->nph = 0;
	php_cgi_globals->check_shebang_line = 1;
	php_cgi_globals->force_redirect = 1;
	php_cgi_globals->redirect_status_env = NULL;
	php_cgi_globals->fix_pathinfo = 1;
	php_cgi_globals->discard_path = 0;
	php_cgi_globals->fcgi_logging = 1;
#ifdef PHP_WIN32
	php_cgi_globals->impersonate = 0;
#endif
	zend_hash_init(&php_cgi_globals->user_config_cache, 0, NULL, (dtor_func_t) user_config_cache_entry_dtor, 1);
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
	DISPLAY_INI_ENTRIES();
}
/* }}} */

PHP_FUNCTION(apache_child_terminate) /* {{{ */
{
	if (ZEND_NUM_ARGS() > 0) {
		WRONG_PARAM_COUNT;
	}
	if (fcgi_is_fastcgi()) {
		fcgi_terminate();
	}
}
/* }}} */

static void add_request_header(char *var, unsigned int var_len, char *val, unsigned int val_len, void *arg TSRMLS_DC) /* {{{ */
{
	zval *return_value = (zval*)arg;
	char *str = NULL;
	char *p;
	ALLOCA_FLAG(use_heap)

	if (var_len > 5 &&
	    var[0] == 'H' &&
	    var[1] == 'T' &&
	    var[2] == 'T' &&
	    var[3] == 'P' &&
	    var[4] == '_') {

		var_len -= 5;
		p = var + 5;
		var = str = do_alloca(var_len + 1, use_heap);
		*str++ = *p++;
		while (*p) {
			if (*p == '_') {
				*str++ = '-';
				p++;
				if (*p) {
					*str++ = *p++;
				}
			} else if (*p >= 'A' && *p <= 'Z') {
				*str++ = (*p++ - 'A' + 'a');
			} else {
				*str++ = *p++;
			}
		}
		*str = 0;
	} else if (var_len == sizeof("CONTENT_TYPE")-1 &&
	           memcmp(var, "CONTENT_TYPE", sizeof("CONTENT_TYPE")-1) == 0) {
		var = "Content-Type";
	} else if (var_len == sizeof("CONTENT_LENGTH")-1 &&
	           memcmp(var, "CONTENT_LENGTH", sizeof("CONTENT_LENGTH")-1) == 0) {
		var = "Content-Length";
	} else {
		return;
	}
	add_assoc_stringl_ex(return_value, var, var_len+1, val, val_len, 1);
	if (str) {
		free_alloca(var, use_heap);
	}
}
/* }}} */

PHP_FUNCTION(apache_request_headers) /* {{{ */
{
	if (ZEND_NUM_ARGS() > 0) {
		WRONG_PARAM_COUNT;
	}
	array_init(return_value);
	if (fcgi_is_fastcgi()) {
		fcgi_request *request = (fcgi_request*) SG(server_context);

		fcgi_loadenv(request, add_request_header, return_value TSRMLS_CC);
	} else {
		char buf[128];
		char **env, *p, *q, *var, *val, *t = buf;
		size_t alloc_size = sizeof(buf);
		unsigned long var_len;

		for (env = environ; env != NULL && *env != NULL; env++) {
			val = strchr(*env, '=');
			if (!val) {				/* malformed entry? */
				continue;
			}
			var_len = val - *env;
			if (var_len >= alloc_size) {
				alloc_size = var_len + 64;
				t = (t == buf ? emalloc(alloc_size): erealloc(t, alloc_size));
			}
			var = *env;
			if (var_len > 5 &&
			    var[0] == 'H' &&
			    var[1] == 'T' &&
			    var[2] == 'T' &&
			    var[3] == 'P' &&
			    var[4] == '_') {

				var_len -= 5;

				if (var_len >= alloc_size) {
					alloc_size = var_len + 64;
					t = (t == buf ? emalloc(alloc_size): erealloc(t, alloc_size));
				}
				p = var + 5;

				var = q = t;
				/* First char keep uppercase */
				*q++ = *p++;
				while (*p) {
					if (*p == '=') {
						/* End of name */
						break;
					} else if (*p == '_') {
						*q++ = '-';
						p++;
						/* First char after - keep uppercase */
						if (*p && *p!='=') {
							*q++ = *p++;
						}
					} else if (*p >= 'A' && *p <= 'Z') {
						/* lowercase */
						*q++ = (*p++ - 'A' + 'a');
					} else {
						*q++ = *p++;
					}
				}
				*q = 0;
			} else if (var_len == sizeof("CONTENT_TYPE")-1 &&
			           memcmp(var, "CONTENT_TYPE", sizeof("CONTENT_TYPE")-1) == 0) {
				var = "Content-Type";
			} else if (var_len == sizeof("CONTENT_LENGTH")-1 &&
			           memcmp(var, "CONTENT_LENGTH", sizeof("CONTENT_LENGTH")-1) == 0) {
				var = "Content-Length";
			} else {
				continue;
			}
			val++;
			add_assoc_string_ex(return_value, var, var_len+1, val, 1);
		}
		if (t != buf && t != NULL) {
			efree(t);
		}
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
	if (ZEND_NUM_ARGS() > 0) {
		WRONG_PARAM_COUNT;
	}

	if (!&SG(sapi_headers).headers) {
		RETURN_FALSE;
	}
	array_init(return_value);
	zend_llist_apply_with_argument(&SG(sapi_headers).headers, (llist_apply_with_arg_func_t)add_response_header, return_value TSRMLS_CC);
}
/* }}} */

ZEND_BEGIN_ARG_INFO(arginfo_no_args, 0)
ZEND_END_ARG_INFO()

const zend_function_entry cgi_functions[] = {
	PHP_FE(apache_child_terminate, arginfo_no_args)
	PHP_FE(apache_request_headers, arginfo_no_args)
	PHP_FE(apache_response_headers, arginfo_no_args)
	PHP_FALIAS(getallheaders, apache_request_headers, arginfo_no_args)
	{NULL, NULL, NULL}
};

static zend_module_entry cgi_module_entry = {
	STANDARD_MODULE_HEADER,
	"cgi-fcgi",
	cgi_functions,
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
	int free_query_string = 0;
	int exit_status = SUCCESS;
	int cgi = 0, c, i, len;
	zend_file_handle file_handle;
	char *s;

	/* temporary locals */
	int behavior = PHP_MODE_STANDARD;
	int no_headers = 0;
	int orig_optind = php_optind;
	char *orig_optarg = php_optarg;
	char *script_file = NULL;
	int ini_entries_len = 0;
	/* end of temporary locals */

#ifdef ZTS
	void ***tsrm_ls;
#endif

	int max_requests = 500;
	int requests = 0;
	int fastcgi;
	char *bindpath = NULL;
	int fcgi_fd = 0;
	fcgi_request *request = NULL;
	int repeats = 1;
	int benchmark = 0;
#if HAVE_GETTIMEOFDAY
	struct timeval start, end;
#else
	time_t start, end;
#endif
#ifndef PHP_WIN32
	int status = 0;
#endif
	char *query_string;
	char *decoded_query_string;
	int skip_getopt = 0;

#if 0 && defined(PHP_DEBUG)
	/* IIS is always making things more difficult.  This allows
	 * us to stop PHP and attach a debugger before much gets started */
	{
		char szMessage [256];
		wsprintf (szMessage, "Please attach a debugger to the process 0x%X [%d] (%s) and click OK", GetCurrentProcessId(), GetCurrentProcessId(), argv[0]);
		MessageBox(NULL, szMessage, "CGI Debug Time!", MB_OK|MB_SERVICE_NOTIFICATION);
	}
#endif

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
	fastcgi = fcgi_is_fastcgi();
	cgi_sapi_module.php_ini_path_override = NULL;

#ifdef PHP_WIN32
	_fmode = _O_BINARY; /* sets default for file streams to binary */
	setmode(_fileno(stdin),  O_BINARY);	/* make the stdio mode be binary */
	setmode(_fileno(stdout), O_BINARY);	/* make the stdio mode be binary */
	setmode(_fileno(stderr), O_BINARY);	/* make the stdio mode be binary */
#endif

	if (!fastcgi) {
		/* Make sure we detect we are a cgi - a bit redundancy here,
		 * but the default case is that we have to check only the first one. */
		if (getenv("SERVER_SOFTWARE") ||
			getenv("SERVER_NAME") ||
			getenv("GATEWAY_INTERFACE") ||
			getenv("REQUEST_METHOD")
		) {
			cgi = 1;
		}
	}

	if((query_string = getenv("QUERY_STRING")) != NULL && strchr(query_string, '=') == NULL) {
		/* we've got query string that has no = - apache CGI will pass it to command line */
		unsigned char *p;
		decoded_query_string = strdup(query_string);
		php_url_decode(decoded_query_string, strlen(decoded_query_string));
		for (p = decoded_query_string; *p &&  *p <= ' '; p++) {
			/* skip all leading spaces */
		}
		if(*p == '-') {
			skip_getopt = 1;
		}
		free(decoded_query_string);
	}

	while (!skip_getopt && (c = php_getopt(argc, argv, OPTIONS, &php_optarg, &php_optind, 0, 2)) != -1) {
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
			/* if we're started on command line, check to see if
			 * we are being started as an 'external' fastcgi
			 * server by accepting a bindpath parameter. */
			case 'b':
				if (!fastcgi) {
					bindpath = strdup(php_optarg);
				}
				break;
			case 's': /* generate highlighted HTML from source */
				behavior = PHP_MODE_HIGHLIGHT;
				break;
		}
	}
	php_optind = orig_optind;
	php_optarg = orig_optarg;

	if (fastcgi || bindpath) {
		/* Override SAPI callbacks */
		cgi_sapi_module.ub_write     = sapi_fcgi_ub_write;
		cgi_sapi_module.flush        = sapi_fcgi_flush;
		cgi_sapi_module.read_post    = sapi_fcgi_read_post;
		cgi_sapi_module.getenv       = sapi_fcgi_getenv;
		cgi_sapi_module.read_cookies = sapi_fcgi_read_cookies;
	}

#ifdef ZTS
	SG(request_info).path_translated = NULL;
#endif

	cgi_sapi_module.executable_location = argv[0];
	if (!cgi && !fastcgi && !bindpath) {
		cgi_sapi_module.additional_functions = additional_functions;
	}

	/* startup after we get the above ini override se we get things right */
	if (cgi_sapi_module.startup(&cgi_sapi_module) == FAILURE) {
#ifdef ZTS
		tsrm_shutdown();
#endif
		return FAILURE;
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
			return FAILURE;
		}
	}

	if (bindpath) {
		fcgi_fd = fcgi_listen(bindpath, 128);
		if (fcgi_fd < 0) {
			fprintf(stderr, "Couldn't create FastCGI listen socket on port %s\n", bindpath);
#ifdef ZTS
			tsrm_shutdown();
#endif
			return FAILURE;
		}
		fastcgi = fcgi_is_fastcgi();
	}
	if (fastcgi) {
		/* How many times to run PHP scripts before dying */
		if (getenv("PHP_FCGI_MAX_REQUESTS")) {
			max_requests = atoi(getenv("PHP_FCGI_MAX_REQUESTS"));
			if (max_requests < 0) {
				fprintf(stderr, "PHP_FCGI_MAX_REQUESTS is not valid\n");
				return FAILURE;
			}
		}

		/* make php call us to get _ENV vars */
		php_php_import_environment_variables = php_import_environment_variables;
		php_import_environment_variables = cgi_php_import_environment_variables;

		/* library is already initialized, now init our request */
		request = fcgi_init_request(fcgi_fd);

#ifndef PHP_WIN32
	/* Pre-fork, if required */
	if (getenv("PHP_FCGI_CHILDREN")) {
		char * children_str = getenv("PHP_FCGI_CHILDREN");
		children = atoi(children_str);
		if (children < 0) {
			fprintf(stderr, "PHP_FCGI_CHILDREN is not valid\n");
			return FAILURE;
		}
		fcgi_set_mgmt_var("FCGI_MAX_CONNS", sizeof("FCGI_MAX_CONNS")-1, children_str, strlen(children_str));
		/* This is the number of concurrent requests, equals FCGI_MAX_CONNS */
		fcgi_set_mgmt_var("FCGI_MAX_REQS",  sizeof("FCGI_MAX_REQS")-1,  children_str, strlen(children_str));
	} else {
		fcgi_set_mgmt_var("FCGI_MAX_CONNS", sizeof("FCGI_MAX_CONNS")-1, "1", sizeof("1")-1);
		fcgi_set_mgmt_var("FCGI_MAX_REQS",  sizeof("FCGI_MAX_REQS")-1,  "1", sizeof("1")-1);
	}

	if (children) {
		int running = 0;
		pid_t pid;

		/* Create a process group for ourself & children */
		setsid();
		pgroup = getpgrp();
#ifdef DEBUG_FASTCGI
		fprintf(stderr, "Process group %d\n", pgroup);
#endif

		/* Set up handler to kill children upon exit */
		act.sa_flags = 0;
		act.sa_handler = fastcgi_cleanup;
		if (sigaction(SIGTERM, &act, &old_term) ||
			sigaction(SIGINT,  &act, &old_int)  ||
			sigaction(SIGQUIT, &act, &old_quit)
		) {
			perror("Can't set signals");
			exit(1);
		}

		if (fcgi_in_shutdown()) {
			goto parent_out;
		}

		while (parent) {
			do {
#ifdef DEBUG_FASTCGI
				fprintf(stderr, "Forking, %d running\n", running);
#endif
				pid = fork();
				switch (pid) {
				case 0:
					/* One of the children.
					 * Make sure we don't go round the
					 * fork loop any more
					 */
					parent = 0;

					/* don't catch our signals */
					sigaction(SIGTERM, &old_term, 0);
					sigaction(SIGQUIT, &old_quit, 0);
					sigaction(SIGINT,  &old_int,  0);
					break;
				case -1:
					perror("php (pre-forking)");
					exit(1);
					break;
				default:
					/* Fine */
					running++;
					break;
				}
			} while (parent && (running < children));

			if (parent) {
#ifdef DEBUG_FASTCGI
				fprintf(stderr, "Wait for kids, pid %d\n", getpid());
#endif
				parent_waiting = 1;
				while (1) {
					if (wait(&status) >= 0) {
						running--;
						break;
					} else if (exit_signal) {
						break;
					}
				}
				if (exit_signal) {
#if 0
					while (running > 0) {
						while (wait(&status) < 0) {
						}
						running--;
					}
#endif
					goto parent_out;
				}
			}
		}
	} else {
		parent = 0;
	}

#endif /* WIN32 */
	}

	zend_first_try {
		while (!skip_getopt && (c = php_getopt(argc, argv, OPTIONS, &php_optarg, &php_optind, 1, 2)) != -1) {
			switch (c) {
				case 'T':
					benchmark = 1;
					repeats = atoi(php_optarg);
#ifdef HAVE_GETTIMEOFDAY
					gettimeofday(&start, NULL);
#else
					time(&start);
#endif
					break;
				case 'h':
				case '?':
					if (request) {
						fcgi_destroy_request(request);
					}
					fcgi_shutdown();
					no_headers = 1;
					SG(headers_sent) = 1;
					php_cgi_usage(argv[0]);
					php_output_end_all(TSRMLS_C);
					exit_status = 0;
					goto out;
			}
		}
		php_optind = orig_optind;
		php_optarg = orig_optarg;

		/* start of FAST CGI loop */
		/* Initialise FastCGI request structure */
#ifdef PHP_WIN32
		/* attempt to set security impersonation for fastcgi
		 * will only happen on NT based OS, others will ignore it. */
		if (fastcgi && CGIG(impersonate)) {
			fcgi_impersonate();
		}
#endif
		while (!fastcgi || fcgi_accept_request(request) >= 0) {
			SG(server_context) = fastcgi ? (void *) request : (void *) 1;
			init_request_info(request TSRMLS_CC);
			CG(interactive) = 0;

			if (!cgi && !fastcgi) {
				while ((c = php_getopt(argc, argv, OPTIONS, &php_optarg, &php_optind, 0, 2)) != -1) {
					switch (c) {

						case 'a':	/* interactive mode */
							printf("Interactive mode enabled\n\n");
							CG(interactive) = 1;
							break;

						case 'C': /* don't chdir to the script directory */
							SG(options) |= SAPI_OPTION_NO_CHDIR;
							break;

						case 'e': /* enable extended info output */
							CG(compiler_options) |= ZEND_COMPILE_EXTENDED_INFO;
							break;

						case 'f': /* parse file */
							if (script_file) {
								efree(script_file);
							}
							script_file = estrdup(php_optarg);
							no_headers = 1;
							break;

						case 'i': /* php info & quit */
							if (script_file) {
								efree(script_file);
							}
							if (php_request_startup(TSRMLS_C) == FAILURE) {
								SG(server_context) = NULL;
								php_module_shutdown(TSRMLS_C);
								return FAILURE;
							}
							if (no_headers) {
								SG(headers_sent) = 1;
								SG(request_info).no_headers = 1;
							}
							php_print_info(0xFFFFFFFF TSRMLS_CC);
							php_request_shutdown((void *) 0);
							fcgi_shutdown();
							exit_status = 0;
							goto out;

						case 'l': /* syntax check mode */
							no_headers = 1;
							behavior = PHP_MODE_LINT;
							break;

						case 'm': /* list compiled in modules */
							if (script_file) {
								efree(script_file);
							}
							SG(headers_sent) = 1;
							php_printf("[PHP Modules]\n");
							print_modules(TSRMLS_C);
							php_printf("\n[Zend Modules]\n");
							print_extensions(TSRMLS_C);
							php_printf("\n");
							php_output_end_all(TSRMLS_C);
							fcgi_shutdown();
							exit_status = 0;
							goto out;

#if 0 /* not yet operational, see also below ... */
						case '': /* generate indented source mode*/
							behavior=PHP_MODE_INDENT;
							break;
#endif

						case 'q': /* do not generate HTTP headers */
							no_headers = 1;
							break;

						case 'v': /* show php version & quit */
							if (script_file) {
								efree(script_file);
							}
							no_headers = 1;
							if (php_request_startup(TSRMLS_C) == FAILURE) {
								SG(server_context) = NULL;
								php_module_shutdown(TSRMLS_C);
								return FAILURE;
							}
							if (no_headers) {
								SG(headers_sent) = 1;
								SG(request_info).no_headers = 1;
							}
#if ZEND_DEBUG
							php_printf("PHP %s (%s) (built: %s %s) (DEBUG)\nCopyright (c) 1997-2012 The PHP Group\n%s", PHP_VERSION, sapi_module.name, __DATE__, __TIME__, get_zend_version());
#else
							php_printf("PHP %s (%s) (built: %s %s)\nCopyright (c) 1997-2012 The PHP Group\n%s", PHP_VERSION, sapi_module.name, __DATE__, __TIME__, get_zend_version());
#endif
							php_request_shutdown((void *) 0);
							fcgi_shutdown();
							exit_status = 0;
							goto out;

						case 'w':
							behavior = PHP_MODE_STRIP;
							break;

						case 'z': /* load extension file */
							zend_load_extension(php_optarg);
							break;

						default:
							break;
					}
				}

				if (script_file) {
					/* override path_translated if -f on command line */
					STR_FREE(SG(request_info).path_translated);
					SG(request_info).path_translated = script_file;
					/* before registering argv to module exchange the *new* argv[0] */
					/* we can achieve this without allocating more memory */
					SG(request_info).argc = argc - (php_optind - 1);
					SG(request_info).argv = &argv[php_optind - 1];
					SG(request_info).argv[0] = script_file;
				} else if (argc > php_optind) {
					/* file is on command line, but not in -f opt */
					STR_FREE(SG(request_info).path_translated);
					SG(request_info).path_translated = estrdup(argv[php_optind]);
					/* arguments after the file are considered script args */
					SG(request_info).argc = argc - php_optind;
					SG(request_info).argv = &argv[php_optind];
				}

				if (no_headers) {
					SG(headers_sent) = 1;
					SG(request_info).no_headers = 1;
				}

				/* all remaining arguments are part of the query string
				 * this section of code concatenates all remaining arguments
				 * into a single string, seperating args with a &
				 * this allows command lines like:
				 *
				 *  test.php v1=test v2=hello+world!
				 *  test.php "v1=test&v2=hello world!"
				 *  test.php v1=test "v2=hello world!"
				*/
				if (!SG(request_info).query_string && argc > php_optind) {
					int slen = strlen(PG(arg_separator).input);
					len = 0;
					for (i = php_optind; i < argc; i++) {
						if (i < (argc - 1)) {
							len += strlen(argv[i]) + slen;
						} else {
							len += strlen(argv[i]);
						}
					}

					len += 2;
					s = malloc(len);
					*s = '\0';			/* we are pretending it came from the environment  */
					for (i = php_optind; i < argc; i++) {
						strlcat(s, argv[i], len);
						if (i < (argc - 1)) {
							strlcat(s, PG(arg_separator).input, len);
						}
					}
					SG(request_info).query_string = s;
					free_query_string = 1;
				}
			} /* end !cgi && !fastcgi */

			/*
				we never take stdin if we're (f)cgi, always
				rely on the web server giving us the info
				we need in the environment.
			*/
			if (SG(request_info).path_translated || cgi || fastcgi) {
				file_handle.type = ZEND_HANDLE_FILENAME;
				file_handle.filename = SG(request_info).path_translated;
				file_handle.handle.fp = NULL;
			} else {
				file_handle.filename = "-";
				file_handle.type = ZEND_HANDLE_FP;
				file_handle.handle.fp = stdin;
			}

			file_handle.opened_path = NULL;
			file_handle.free_filename = 0;

			/* request startup only after we've done all we can to
			 * get path_translated */
			if (php_request_startup(TSRMLS_C) == FAILURE) {
				if (fastcgi) {
					fcgi_finish_request(request, 1);
				}
				SG(server_context) = NULL;
				php_module_shutdown(TSRMLS_C);
				return FAILURE;
			}
			if (no_headers) {
				SG(headers_sent) = 1;
				SG(request_info).no_headers = 1;
			}

			/*
				at this point path_translated will be set if:
				1. we are running from shell and got filename was there
				2. we are running as cgi or fastcgi
			*/
			if (cgi || fastcgi || SG(request_info).path_translated) {
				if (php_fopen_primary_script(&file_handle TSRMLS_CC) == FAILURE) {
					zend_try {
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
					if (fastcgi) {
						goto fastcgi_request_done;
					}

					STR_FREE(SG(request_info).path_translated);

					if (free_query_string && SG(request_info).query_string) {
						free(SG(request_info).query_string);
						SG(request_info).query_string = NULL;
					}

					php_request_shutdown((void *) 0);
					SG(server_context) = NULL;
					php_module_shutdown(TSRMLS_C);
					sapi_shutdown();
#ifdef ZTS
					tsrm_shutdown();
#endif
					return FAILURE;
				}
			}

			if (CGIG(check_shebang_line)) {
				/* #!php support */
				switch (file_handle.type) {
					case ZEND_HANDLE_FD:
						if (file_handle.handle.fd < 0) {
							break;
						}
						file_handle.type = ZEND_HANDLE_FP;
						file_handle.handle.fp = fdopen(file_handle.handle.fd, "rb");
						/* break missing intentionally */
					case ZEND_HANDLE_FP:
						if (!file_handle.handle.fp ||
						    (file_handle.handle.fp == stdin)) {
							break;
						}
						c = fgetc(file_handle.handle.fp);
						if (c == '#') {
							while (c != '\n' && c != '\r' && c != EOF) {
								c = fgetc(file_handle.handle.fp);	/* skip to end of line */
							}
							/* handle situations where line is terminated by \r\n */
							if (c == '\r') {
								if (fgetc(file_handle.handle.fp) != '\n') {
									long pos = ftell(file_handle.handle.fp);
									fseek(file_handle.handle.fp, pos - 1, SEEK_SET);
								}
							}
							CG(start_lineno) = 2;
						} else {
							rewind(file_handle.handle.fp);
						}
						break;
					case ZEND_HANDLE_STREAM:
						c = php_stream_getc((php_stream*)file_handle.handle.stream.handle);
						if (c == '#') {
							while (c != '\n' && c != '\r' && c != EOF) {
								c = php_stream_getc((php_stream*)file_handle.handle.stream.handle);	/* skip to end of line */
							}
							/* handle situations where line is terminated by \r\n */
							if (c == '\r') {
								if (php_stream_getc((php_stream*)file_handle.handle.stream.handle) != '\n') {
									long pos = php_stream_tell((php_stream*)file_handle.handle.stream.handle);
									php_stream_seek((php_stream*)file_handle.handle.stream.handle, pos - 1, SEEK_SET);
								}
							}
							CG(start_lineno) = 2;
						} else {
							php_stream_rewind((php_stream*)file_handle.handle.stream.handle);
						}
						break;
					case ZEND_HANDLE_MAPPED:
						if (file_handle.handle.stream.mmap.buf[0] == '#') {
						    int i = 1;

						    c = file_handle.handle.stream.mmap.buf[i++];
							while (c != '\n' && c != '\r' && c != EOF) {
								c = file_handle.handle.stream.mmap.buf[i++];
							}
							if (c == '\r') {
								if (file_handle.handle.stream.mmap.buf[i] == '\n') {
									i++;
								}
							}
							file_handle.handle.stream.mmap.buf += i;
							file_handle.handle.stream.mmap.len -= i;
						}
						break;
					default:
						break;
				}
			}

			switch (behavior) {
				case PHP_MODE_STANDARD:
					php_execute_script(&file_handle TSRMLS_CC);
					break;
				case PHP_MODE_LINT:
					PG(during_request_startup) = 0;
					exit_status = php_lint_script(&file_handle TSRMLS_CC);
					if (exit_status == SUCCESS) {
						zend_printf("No syntax errors detected in %s\n", file_handle.filename);
					} else {
						zend_printf("Errors parsing %s\n", file_handle.filename);
					}
					break;
				case PHP_MODE_STRIP:
					if (open_file_for_scanning(&file_handle TSRMLS_CC) == SUCCESS) {
						zend_strip(TSRMLS_C);
						zend_file_handle_dtor(&file_handle TSRMLS_CC);
						php_output_teardown();
					}
					return SUCCESS;
					break;
				case PHP_MODE_HIGHLIGHT:
					{
						zend_syntax_highlighter_ini syntax_highlighter_ini;

						if (open_file_for_scanning(&file_handle TSRMLS_CC) == SUCCESS) {
							php_get_highlight_struct(&syntax_highlighter_ini);
							zend_highlight(&syntax_highlighter_ini TSRMLS_CC);
							if (fastcgi) {
								goto fastcgi_request_done;
							}
							zend_file_handle_dtor(&file_handle TSRMLS_CC);
							php_output_teardown();
						}
						return SUCCESS;
					}
					break;
#if 0
				/* Zeev might want to do something with this one day */
				case PHP_MODE_INDENT:
					open_file_for_scanning(&file_handle TSRMLS_CC);
					zend_indent();
					zend_file_handle_dtor(&file_handle TSRMLS_CC);
					php_output_teardown();
					return SUCCESS;
					break;
#endif
			}

fastcgi_request_done:
			{
				STR_FREE(SG(request_info).path_translated);

				php_request_shutdown((void *) 0);

				if (exit_status == 0) {
					exit_status = EG(exit_status);
				}

				if (free_query_string && SG(request_info).query_string) {
					free(SG(request_info).query_string);
					SG(request_info).query_string = NULL;
				}
			}

			if (!fastcgi) {
				if (benchmark) {
					repeats--;
					if (repeats > 0) {
						script_file = NULL;
						php_optind = orig_optind;
						php_optarg = orig_optarg;
						continue;
					}
				}
				break;
			}

			/* only fastcgi will get here */
			requests++;
			if (max_requests && (requests == max_requests)) {
				fcgi_finish_request(request, 1);
				if (bindpath) {
					free(bindpath);
				}
				if (max_requests != 1) {
					/* no need to return exit_status of the last request */
					exit_status = 0;
				}
				break;
			}
			/* end of fastcgi loop */
		}
		if (request) {
			fcgi_destroy_request(request);
		}
		fcgi_shutdown();

		if (cgi_sapi_module.php_ini_path_override) {
			free(cgi_sapi_module.php_ini_path_override);
		}
		if (cgi_sapi_module.ini_entries) {
			free(cgi_sapi_module.ini_entries);
		}
	} zend_catch {
		exit_status = 255;
	} zend_end_try();

out:
	if (benchmark) {
		int sec;
#ifdef HAVE_GETTIMEOFDAY
		int usec;

		gettimeofday(&end, NULL);
		sec = (int)(end.tv_sec - start.tv_sec);
		if (end.tv_usec >= start.tv_usec) {
			usec = (int)(end.tv_usec - start.tv_usec);
		} else {
			sec -= 1;
			usec = (int)(end.tv_usec + 1000000 - start.tv_usec);
		}
		fprintf(stderr, "\nElapsed time: %d.%06d sec\n", sec, usec);
#else
		time(&end);
		sec = (int)(end - start);
		fprintf(stderr, "\nElapsed time: %d sec\n", sec);
#endif
	}

#ifndef PHP_WIN32
parent_out:
#endif

	SG(server_context) = NULL;
	php_module_shutdown(TSRMLS_C);
	sapi_shutdown();

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
