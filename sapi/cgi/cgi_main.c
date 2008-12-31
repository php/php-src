/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2009 The PHP Group                                |
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
#include "win32/time.h"
#include "win32/signal.h"
#include <process.h>
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
#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#if HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#include "zend.h"
#include "zend_extensions.h"
#include "php_ini.h"
#include "php_globals.h"
#include "php_main.h"
#include "fopen_wrappers.h"
#include "ext/standard/php_standard.h"
#ifdef PHP_WIN32
#include <io.h>
#include <fcntl.h>
#include "win32/php_registry.h"
#endif

#ifdef __riscos__
#include <unixlib/local.h>
int __riscosify_control = __RISCOSIFY_STRICT_UNIX_SPECS;
#endif

#include "zend_compile.h"
#include "zend_execute.h"
#include "zend_highlight.h"
#include "zend_indent.h"

#include "php_getopt.h"

#if PHP_FASTCGI
#include "fastcgi.h"

#ifndef PHP_WIN32
/* XXX this will need to change later when threaded fastcgi is
   implemented.  shane */
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
#if PHP_FASTCGI
 	{'T', 1, "timing"},
#endif
	{'-', 0, NULL} /* end of args */
};

typedef struct _php_cgi_globals_struct {
	zend_bool rfc2616_headers;
	zend_bool nph;
	zend_bool check_shebang_line;
#if ENABLE_PATHINFO_CHECK
	zend_bool fix_pathinfo;
#endif
#if FORCE_CGI_REDIRECT
	zend_bool force_redirect;
	char *redirect_status_env;
#endif
#if PHP_FASTCGI
	zend_bool fcgi_logging;
# ifdef PHP_WIN32
	zend_bool impersonate;
# endif
#endif
} php_cgi_globals_struct;

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

	return strcasecmp(((zend_module_entry *)f->pData)->name,
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

static int extension_name_cmp(const zend_llist_element **f,
							  const zend_llist_element **s TSRMLS_DC)
{
	return strcmp(((zend_extension *)(*f)->data)->name,
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
#ifdef PHP_WRITE_STDOUT
	long ret;
#else
	size_t ret;
#endif

#if PHP_FASTCGI
	if (fcgi_is_fastcgi()) {
		fcgi_request *request = (fcgi_request*) SG(server_context);
		long ret = fcgi_write(request, FCGI_STDOUT, str, str_length);
		if (ret <= 0) {
			return 0;
		}
		return ret;
	}
#endif
#ifdef PHP_WRITE_STDOUT
	ret = write(STDOUT_FILENO, str, str_length);
	if (ret <= 0) return 0;
	return ret;
#else
	ret = fwrite(str, 1, MIN(str_length, 16384), stdout);
	return ret;
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


static void sapi_cgibin_flush(void *server_context)
{
#if PHP_FASTCGI
	if (fcgi_is_fastcgi()) {
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
#endif
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
			    strncasecmp(SG(sapi_headers).http_status_line, "HTTP/", 5) == 0) {
				len = slprintf(buf, sizeof(buf), "Status:%s\r\n", s);
				response_status = atoi((s + 1));
			} else {
				h = (sapi_header_struct*)zend_llist_get_first_ex(&sapi_headers->headers, &pos);
				while (h) {
					if (h->header_len > sizeof("Status:")-1 &&
					    strncasecmp(h->header, "Status:", sizeof("Status:")-1) == 0) {
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
			    strncasecmp(h->header, "Status:", sizeof("Status:")-1) == 0) {
			    if (!ignore_status) {
				    ignore_status = 1;
					PHPWRITE_H(h->header, h->header_len);
					PHPWRITE_H("\r\n", 2);
				}
			} else if (response_status == 304 && h->header_len > sizeof("Content-Type:")-1 && 
					strncasecmp(h->header, "Content-Type:", sizeof("Content-Type:")-1) == 0) {
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


static int sapi_cgi_read_post(char *buffer, uint count_bytes TSRMLS_DC)
{
	int read_bytes=0, tmp_read_bytes;

	count_bytes = MIN(count_bytes, (uint) SG(request_info).content_length - SG(read_post_bytes));
	while (read_bytes < count_bytes) {
#if PHP_FASTCGI
		if (fcgi_is_fastcgi()) {
			fcgi_request *request = (fcgi_request*) SG(server_context);
			tmp_read_bytes = fcgi_read(request, buffer + read_bytes, count_bytes - read_bytes);
		} else {
			tmp_read_bytes = read(0, buffer + read_bytes, count_bytes - read_bytes);
		}
#else
		tmp_read_bytes = read(0, buffer + read_bytes, count_bytes - read_bytes);
#endif

		if (tmp_read_bytes <= 0) {
			break;
		}
		read_bytes += tmp_read_bytes;
	}
	return read_bytes;
}

static char *sapi_cgibin_getenv(char *name, size_t name_len TSRMLS_DC)
{
#if PHP_FASTCGI
	/* when php is started by mod_fastcgi, no regular environment
	   is provided to PHP.  It is always sent to PHP at the start
	   of a request.  So we have to do our own lookup to get env
	   vars.  This could probably be faster somehow.  */
	if (fcgi_is_fastcgi()) {
		fcgi_request *request = (fcgi_request*) SG(server_context);
		return fcgi_getenv(request, name, name_len);
	}
#endif
	/*  if cgi, or fastcgi and not found in fcgi env
		check the regular environment */
	return getenv(name);
}

static char *_sapi_cgibin_putenv(char *name, char *value TSRMLS_DC)
{
	int name_len;
#if !HAVE_SETENV || !HAVE_UNSETENV
	int len;
	char *buf;
#endif

	if (!name) {
		return NULL;
	}
	name_len = strlen(name);

#if PHP_FASTCGI
	/* when php is started by mod_fastcgi, no regular environment
	   is provided to PHP.  It is always sent to PHP at the start
	   of a request.  So we have to do our own lookup to get env
	   vars.  This could probably be faster somehow.  */
	if (fcgi_is_fastcgi()) {
		fcgi_request *request = (fcgi_request*) SG(server_context);
		return fcgi_putenv(request, name, name_len, value);
	}
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
	return sapi_cgibin_getenv((char *) "HTTP_COOKIE", sizeof("HTTP_COOKIE")-1 TSRMLS_CC);
}

#if PHP_FASTCGI
void cgi_php_import_environment_variables(zval *array_ptr TSRMLS_DC)
{
	if (PG(http_globals)[TRACK_VARS_ENV] &&
	    array_ptr != PG(http_globals)[TRACK_VARS_ENV] &&
	    Z_TYPE_P(PG(http_globals)[TRACK_VARS_ENV]) == IS_ARRAY &&
	    zend_hash_num_elements(Z_ARRVAL_P(PG(http_globals)[TRACK_VARS_ENV])) > 0) {
	    zval_dtor(array_ptr);
	    *array_ptr = *PG(http_globals)[TRACK_VARS_ENV];
	    INIT_PZVAL(array_ptr);
	    zval_copy_ctor(array_ptr);
	    return;
	} else if (PG(http_globals)[TRACK_VARS_SERVER] &&
		array_ptr != PG(http_globals)[TRACK_VARS_SERVER] &&
	    Z_TYPE_P(PG(http_globals)[TRACK_VARS_SERVER]) == IS_ARRAY &&
	    zend_hash_num_elements(Z_ARRVAL_P(PG(http_globals)[TRACK_VARS_SERVER])) > 0) {
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
		HashPosition pos;
		int magic_quotes_gpc = PG(magic_quotes_gpc);
		char *var, **val;
		uint var_len;
		ulong idx;
		int filter_arg = (array_ptr == PG(http_globals)[TRACK_VARS_ENV])?PARSE_ENV:PARSE_SERVER;

		/* turn off magic_quotes while importing environment variables */
		PG(magic_quotes_gpc) = 0;
		for (zend_hash_internal_pointer_reset_ex(&request->env, &pos);
		     zend_hash_get_current_key_ex(&request->env, &var, &var_len, &idx, 0, &pos) == HASH_KEY_IS_STRING &&
		     zend_hash_get_current_data_ex(&request->env, (void **) &val, &pos) == SUCCESS;
		     zend_hash_move_forward_ex(&request->env, &pos)) {
			unsigned int new_val_len;
			if (sapi_module.input_filter(filter_arg, var, val, strlen(*val), &new_val_len TSRMLS_CC)) {
				php_register_variable_safe(var, *val, new_val_len, array_ptr TSRMLS_CC);
			}
		}
		PG(magic_quotes_gpc) = magic_quotes_gpc;
	}
}
#endif

static void sapi_cgi_register_variables(zval *track_vars_array TSRMLS_DC)
{
	unsigned int php_self_len;
	char *php_self;

	/* In CGI mode, we consider the environment to be a part of the server
	 * variables
	 */
	php_import_environment_variables(track_vars_array TSRMLS_CC);

#if ENABLE_PATHINFO_CHECK
	if (CGIG(fix_pathinfo)) {
		char *script_name   = SG(request_info).request_uri;
		unsigned int script_name_len = script_name ? strlen(script_name) : 0;
		char *path_info     = sapi_cgibin_getenv("PATH_INFO", sizeof("PATH_INFO")-1 TSRMLS_CC);
		unsigned int path_info_len = path_info ? strlen(path_info) : 0;

		php_self_len = script_name_len + path_info_len;
		php_self = emalloc(php_self_len + 1);
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
		return;
	}
#endif

	php_self = SG(request_info).request_uri ? SG(request_info).request_uri : "";
	php_self_len = strlen(php_self);
	if (sapi_module.input_filter(PARSE_SERVER, "PHP_SELF", &php_self, php_self_len, &php_self_len TSRMLS_CC)) {
		php_register_variable_safe("PHP_SELF", php_self, php_self_len, track_vars_array TSRMLS_CC);
	}
}

static void sapi_cgi_log_message(char *message)
{
#if PHP_FASTCGI
	TSRMLS_FETCH();

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
	} else
#endif /* PHP_FASTCGI */
	fprintf(stderr, "%s\n", message);
}

static int sapi_cgi_deactivate(TSRMLS_D)
{
	/* flush only when SAPI was started. The reasons are:
		1. SAPI Deactivate is called from two places: module init and request shutdown
		2. When the first call occurs and the request is not set up, flush fails on 
			FastCGI.
	*/
	if (SG(sapi_started)) {
		sapi_cgibin_flush(SG(server_context));
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
#if PHP_FASTCGI
	"cgi-fcgi",						/* name */
	"CGI/FastCGI",					/* pretty name */
#else
	"cgi",							/* name */
	"CGI",							/* pretty name */
#endif

	php_cgi_startup,				/* startup */
	php_module_shutdown_wrapper,	/* shutdown */

	NULL,							/* activate */
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

	STANDARD_SAPI_MODULE_PROPERTIES
};
/* }}} */

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

	php_printf("Usage: %s [-q] [-h] [-s] [-v] [-i] [-f <file>]\n"
			   "       %s <file> [args...]\n"
			   "  -a               Run interactively\n"
#if PHP_FASTCGI
			   "  -b <address:port>|<port> Bind Path for external FASTCGI Server mode\n"
#endif
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
#if PHP_FASTCGI
			   "  -T <count>       Measure execution time of script repeated <count> times.\n"
#endif
			   ,
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
	const char *p;

	if (!path) {
		return 0;
	}
	p = strstr(path, "..");
	if (p) {
		if ((p == path || IS_SLASH(*(p-1))) &&
		    (*(p+2) == 0 || IS_SLASH(*(p+2)))) {
			return 0;
		}
		while (1) {
			p = strstr(p+1, "..");
			if (!p) {
				break;
			}
			if (IS_SLASH(*(p-1)) &&
			    (*(p+2) == 0 || IS_SLASH(*(p+2)))) {
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
	rather than the interpreter.  PHP_SELF is set to this.

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
	char *env_script_filename = sapi_cgibin_getenv("SCRIPT_FILENAME", sizeof("SCRIPT_FILENAME")-1 TSRMLS_CC);
	char *env_path_translated = sapi_cgibin_getenv("PATH_TRANSLATED", sizeof("PATH_TRANSLATED")-1 TSRMLS_CC);
	char *script_path_translated = env_script_filename;

#if !DISCARD_PATH
	/* some broken servers do not have script_filename or argv0
	   an example, IIS configured in some ways.  then they do more
	   broken stuff and set path_translated to the cgi script location */
	if (!script_path_translated && env_path_translated) {
		script_path_translated = env_path_translated;
	}
#endif

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
	   we are running in a cgi environment, since it is always
	   null otherwise.  otherwise, the filename
	   of the script will be retreived later via argc/argv */
	if (script_path_translated) {
		const char *auth;
		char *content_length = sapi_cgibin_getenv("CONTENT_LENGTH", sizeof("CONTENT_LENGTH")-1 TSRMLS_CC);
		char *content_type = sapi_cgibin_getenv("CONTENT_TYPE", sizeof("CONTENT_TYPE")-1 TSRMLS_CC);
		char *env_path_info = sapi_cgibin_getenv("PATH_INFO", sizeof("PATH_INFO")-1 TSRMLS_CC);
		char *env_script_name = sapi_cgibin_getenv("SCRIPT_NAME", sizeof("SCRIPT_NAME")-1 TSRMLS_CC);
#if ENABLE_PATHINFO_CHECK
		struct stat st;
		char *env_redirect_url = sapi_cgibin_getenv("REDIRECT_URL", sizeof("REDIRECT_URL")-1 TSRMLS_CC);
		char *env_document_root = sapi_cgibin_getenv("DOCUMENT_ROOT", sizeof("DOCUMENT_ROOT")-1 TSRMLS_CC);
		int script_path_translated_len;

		/* Hack for buggy IIS that sets incorrect PATH_INFO */
		char *env_server_software = sapi_cgibin_getenv("SERVER_SOFTWARE", sizeof("SERVER_SOFTWARE")-1 TSRMLS_CC);
		if (env_server_software &&
		    env_script_name &&
		    env_path_info &&
		    strncmp(env_server_software, "Microsoft-IIS", sizeof("Microsoft-IIS")-1) == 0 &&
		    strncmp(env_path_info, env_script_name, strlen(env_script_name)) == 0) {
			env_path_info = _sapi_cgibin_putenv("ORIG_PATH_INFO", env_path_info TSRMLS_CC);
		    env_path_info += strlen(env_script_name);
		    if (*env_path_info == 0) {
		    	env_path_info = NULL;
		    }
			env_path_info = _sapi_cgibin_putenv("PATH_INFO", env_path_info TSRMLS_CC);
		}

		if (CGIG(fix_pathinfo)) {
			char *real_path = NULL;
			char *orig_path_translated = env_path_translated;
			char *orig_path_info = env_path_info;
			char *orig_script_name = env_script_name;
			char *orig_script_filename = env_script_filename;

			if (!env_document_root && PG(doc_root)) {
				env_document_root = _sapi_cgibin_putenv("DOCUMENT_ROOT", PG(doc_root) TSRMLS_CC);
				/* fix docroot */
				TRANSLATE_SLASHES(env_document_root);
 			}

			if (env_path_translated != NULL && env_redirect_url != NULL) {
				/* 
				   pretty much apache specific.  If we have a redirect_url
				   then our script_filename and script_name point to the
				   php executable
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
			     (real_path = tsrm_realpath(script_path_translated, NULL TSRMLS_CC)) == NULL)) {
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
						   SCRIPT_FILENAME minus SCRIPT_NAME
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
						} else if (env_script_name && 
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
				if (is_valid_path(script_path_translated)) {
					SG(request_info).path_translated = estrdup(script_path_translated);
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
				if (is_valid_path(script_path_translated)) {
					SG(request_info).path_translated = estrdup(script_path_translated);
				}
				free(real_path);
			}
		} else {
#endif
			/* pre 4.3 behaviour, shouldn't be used but provides BC */
			if (env_path_info) {
				SG(request_info).request_uri = env_path_info;
			} else {
				SG(request_info).request_uri = env_script_name;
			}
#if !DISCARD_PATH
			if (env_path_translated) {
				script_path_translated = env_path_translated;
			}
#endif
			if (is_valid_path(script_path_translated)) {
				SG(request_info).path_translated = estrdup(script_path_translated);
			}
#if ENABLE_PATHINFO_CHECK
		}
#endif
		SG(request_info).request_method = sapi_cgibin_getenv("REQUEST_METHOD", sizeof("REQUEST_METHOD")-1 TSRMLS_CC);
		/* FIXME - Work out proto_num here */
		SG(request_info).query_string = sapi_cgibin_getenv("QUERY_STRING", sizeof("QUERY_STRING")-1 TSRMLS_CC);
		SG(request_info).content_type = (content_type ? content_type : "" );
		SG(request_info).content_length = (content_length ? atoi(content_length) : 0);
		
		/* The CGI RFC allows servers to pass on unvalidated Authorization data */
		auth = sapi_cgibin_getenv("HTTP_AUTHORIZATION", sizeof("HTTP_AUTHORIZATION")-1 TSRMLS_CC);
		php_handle_auth_data(auth TSRMLS_CC);
	}
}
/* }}} */

#if PHP_FASTCGI && !defined(PHP_WIN32)
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
#if FORCE_CGI_REDIRECT
	STD_PHP_INI_ENTRY("cgi.force_redirect",      "1",  PHP_INI_SYSTEM, OnUpdateBool,   force_redirect, php_cgi_globals_struct, php_cgi_globals)
	STD_PHP_INI_ENTRY("cgi.redirect_status_env", NULL, PHP_INI_SYSTEM, OnUpdateString, redirect_status_env, php_cgi_globals_struct, php_cgi_globals)
#endif
#if ENABLE_PATHINFO_CHECK
	STD_PHP_INI_ENTRY("cgi.fix_pathinfo",        "1",  PHP_INI_SYSTEM, OnUpdateBool,   fix_pathinfo, php_cgi_globals_struct, php_cgi_globals)
#endif
#if PHP_FASTCGI
	STD_PHP_INI_ENTRY("fastcgi.logging",         "1",  PHP_INI_SYSTEM, OnUpdateBool,   fcgi_logging, php_cgi_globals_struct, php_cgi_globals)
# ifdef PHP_WIN32
	STD_PHP_INI_ENTRY("fastcgi.impersonate",     "0",  PHP_INI_SYSTEM, OnUpdateBool,   impersonate, php_cgi_globals_struct, php_cgi_globals)
# endif
#endif
PHP_INI_END()

/* {{{ php_cgi_globals_ctor
 */
static void php_cgi_globals_ctor(php_cgi_globals_struct *php_cgi_globals TSRMLS_DC)
{
	php_cgi_globals->rfc2616_headers = 0;
	php_cgi_globals->nph = 0;
	php_cgi_globals->check_shebang_line = 1;
#if FORCE_CGI_REDIRECT
	php_cgi_globals->force_redirect = 1;
	php_cgi_globals->redirect_status_env = NULL;
#endif
#if ENABLE_PATHINFO_CHECK
	php_cgi_globals->fix_pathinfo = 1;
#endif
#if PHP_FASTCGI
	php_cgi_globals->fcgi_logging = 1;
# ifdef PHP_WIN32
	php_cgi_globals->impersonate = 0;
# endif
#endif
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

static zend_module_entry cgi_module_entry = {
	STANDARD_MODULE_HEADER,
#if PHP_FASTCGI
	"cgi-fcgi",
#else
	"cgi",
#endif
	NULL, 
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
	int retval = FAILURE;
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

#if PHP_FASTCGI
	int max_requests = 500;
	int requests = 0;
	int fastcgi = fcgi_is_fastcgi();
	char *bindpath = NULL;
	int fcgi_fd = 0;
	fcgi_request request;
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
#endif /* PHP_FASTCGI */

#if 0 && defined(PHP_DEBUG)
	/* IIS is always making things more difficult.  This allows
	   us to stop PHP and attach a debugger before much gets started */
	{
		char szMessage [256];
		wsprintf (szMessage, "Please attach a debugger to the process 0x%X [%d] (%s) and click OK",
			  GetCurrentProcessId(), GetCurrentProcessId(), argv[0]);
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
	cgi_sapi_module.php_ini_path_override = NULL;

#ifdef PHP_WIN32
	_fmode = _O_BINARY; /* sets default for file streams to binary */
	setmode(_fileno(stdin),  O_BINARY);	/* make the stdio mode be binary */
	setmode(_fileno(stdout), O_BINARY);	/* make the stdio mode be binary */
	setmode(_fileno(stderr), O_BINARY);	/* make the stdio mode be binary */
#endif

#if PHP_FASTCGI
	if (!fastcgi) {
#endif
	/* Make sure we detect we are a cgi - a bit redundancy here,
	   but the default case is that we have to check only the first one. */
	if (getenv("SERVER_SOFTWARE") ||
		getenv("SERVER_NAME") ||
		getenv("GATEWAY_INTERFACE") ||
		getenv("REQUEST_METHOD")
	) {
		cgi = 1;
	}
#if PHP_FASTCGI
	}
#endif

	while ((c = php_getopt(argc, argv, OPTIONS, &php_optarg, &php_optind, 0)) != -1) {
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
#if PHP_FASTCGI
			/* if we're started on command line, check to see if
			   we are being started as an 'external' fastcgi
			   server by accepting a bindpath parameter. */
			case 'b':
				if (!fastcgi) {
					bindpath = strdup(php_optarg);
				}
				break;
#endif
			case 's': /* generate highlighted HTML from source */
				behavior = PHP_MODE_HIGHLIGHT;
				break;

		}

	}
	php_optind = orig_optind;
	php_optarg = orig_optarg;

#ifdef ZTS
	SG(request_info).path_translated = NULL;
#endif

	cgi_sapi_module.executable_location = argv[0];

	/* startup after we get the above ini override se we get things right */
	if (cgi_sapi_module.startup(&cgi_sapi_module) == FAILURE) {
#ifdef ZTS
		tsrm_shutdown();
#endif
		return FAILURE;
	}

#if FORCE_CGI_REDIRECT
	/* check force_cgi after startup, so we have proper output */
	if (cgi && CGIG(force_redirect)) {
		/* Apache will generate REDIRECT_STATUS,
		 * Netscape and redirect.so will generate HTTP_REDIRECT_STATUS.
		 * redirect.so and installation instructions available from
		 * http://www.koehntopp.de/php.
		 *   -- kk@netuse.de
		 */
		if (!getenv("REDIRECT_STATUS")
			&& !getenv ("HTTP_REDIRECT_STATUS")
			/* this is to allow a different env var to be configured
			   in case some server does something different than above */
			&& (!CGIG(redirect_status_env) || !getenv(CGIG(redirect_status_env)))
			) {
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

#if defined(ZTS) && !defined(PHP_DEBUG)
			/* XXX we're crashing here in msvc6 debug builds at
			   php_message_handler_for_zend:839 because
			   SG(request_info).path_translated is an invalid pointer.
			   It still happens even though I set it to null, so something
			   weird is going on.
			*/
			tsrm_shutdown();
#endif
			return FAILURE;
		}
	}
#endif	/* FORCE_CGI_REDIRECT */

#if PHP_FASTCGI
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
		fcgi_init_request(&request, fcgi_fd);

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
		    sigaction(SIGINT,  &act, &old_int) ||
		    sigaction(SIGQUIT, &act, &old_quit)) {
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
#endif /* FASTCGI */

	zend_first_try {
		while ((c = php_getopt(argc, argv, OPTIONS, &php_optarg, &php_optind, 1)) != -1) {
			switch (c) {
#if PHP_FASTCGI
				case 'T':
			        benchmark = 1;
					repeats = atoi(php_optarg);
#ifdef HAVE_GETTIMEOFDAY
					gettimeofday(&start, NULL);
#else
					time(&start);
#endif
					break;
#endif
				case 'h':
				case '?':
#if PHP_FASTCGI
					fcgi_shutdown();
#endif
					no_headers = 1;
					php_output_startup();
					php_output_activate(TSRMLS_C);
					SG(headers_sent) = 1;
					php_cgi_usage(argv[0]);
					php_end_ob_buffers(1 TSRMLS_CC);
					exit_status = 0;
					goto out;
			}
		}
		php_optind = orig_optind;
		php_optarg = orig_optarg;

#if PHP_FASTCGI
		/* start of FAST CGI loop */
		/* Initialise FastCGI request structure */
#ifdef PHP_WIN32
		/* attempt to set security impersonation for fastcgi
		   will only happen on NT based OS, others will ignore it. */
		if (fastcgi && CGIG(impersonate)) {
			fcgi_impersonate();
		}
#endif
		while (!fastcgi || fcgi_accept_request(&request) >= 0) {
#endif

#if PHP_FASTCGI
		SG(server_context) = (void *) &request;
#else
		SG(server_context) = (void *) 1; /* avoid server_context==NULL checks */
#endif
		init_request_info(TSRMLS_C);
		CG(interactive) = 0;

		if (!cgi
#if PHP_FASTCGI
			&& !fastcgi
#endif
		) {
			while ((c = php_getopt(argc, argv, OPTIONS, &php_optarg, &php_optind, 0)) != -1) {
				switch (c) {

  				case 'a':	/* interactive mode */
						printf("Interactive mode enabled\n\n");
						CG(interactive) = 1;
						break;

				case 'C': /* don't chdir to the script directory */
						SG(options) |= SAPI_OPTION_NO_CHDIR;
						break;

  				case 'e': /* enable extended info output */
						CG(extended_info) = 1;
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
					php_output_startup();
					php_output_activate(TSRMLS_C);
					SG(headers_sent) = 1;
					php_printf("[PHP Modules]\n");
					print_modules(TSRMLS_C);
					php_printf("\n[Zend Modules]\n");
					print_extensions(TSRMLS_C);
					php_printf("\n");
					php_end_ob_buffers(1 TSRMLS_CC);
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
						php_printf("PHP %s (%s) (built: %s %s) (DEBUG)\nCopyright (c) 1997-2009 The PHP Group\n%s", PHP_VERSION, sapi_module.name, __DATE__, __TIME__, get_zend_version());
#else
						php_printf("PHP %s (%s) (built: %s %s)\nCopyright (c) 1997-2009 The PHP Group\n%s", PHP_VERSION, sapi_module.name, __DATE__, __TIME__, get_zend_version());
#endif
						php_request_shutdown((void *) 0);
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
			   this section of code concatenates all remaining arguments
			   into a single string, seperating args with a &
			   this allows command lines like:

			   test.php v1=test v2=hello+world!
			   test.php "v1=test&v2=hello world!"
			   test.php v1=test "v2=hello world!"
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
		if (SG(request_info).path_translated || cgi 
#if PHP_FASTCGI
			|| fastcgi
#endif
		)
		{
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
		   get path_translated */
		if (php_request_startup(TSRMLS_C) == FAILURE) {
#if PHP_FASTCGI
			if (fastcgi) {
				fcgi_finish_request(&request);
			}
#endif
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
		retval = FAILURE;
		if (cgi || SG(request_info).path_translated) {
			if (!php_check_open_basedir(SG(request_info).path_translated TSRMLS_CC)) {
				retval = php_fopen_primary_script(&file_handle TSRMLS_CC);
			}
		}
		/* 
			if we are unable to open path_translated and we are not
			running from shell (so fp == NULL), then fail.
		*/
		if (retval == FAILURE && file_handle.handle.fp == NULL) {
			if (errno == EACCES) {
				SG(sapi_headers).http_response_code = 403;
				PUTS("Access denied.\n");
			} else {
				SG(sapi_headers).http_response_code = 404;
				PUTS("No input file specified.\n");
			}
#if PHP_FASTCGI
			/* we want to serve more requests if this is fastcgi
			   so cleanup and continue, request shutdown is
			   handled later */
			if (fastcgi) {
				goto fastcgi_request_done;
			}
#endif

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

		if (CGIG(check_shebang_line) && file_handle.handle.fp && (file_handle.handle.fp != stdin)) {
			/* #!php support */
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
					fclose(file_handle.handle.fp);
					php_end_ob_buffers(1 TSRMLS_CC);
				}
				return SUCCESS;
				break;
			case PHP_MODE_HIGHLIGHT:
				{
					zend_syntax_highlighter_ini syntax_highlighter_ini;

					if (open_file_for_scanning(&file_handle TSRMLS_CC) == SUCCESS) {
						php_get_highlight_struct(&syntax_highlighter_ini);
						zend_highlight(&syntax_highlighter_ini TSRMLS_CC);
#if PHP_FASTCGI
						if (fastcgi) {
							goto fastcgi_request_done;
						}
#endif
						fclose(file_handle.handle.fp);
						php_end_ob_buffers(1 TSRMLS_CC);
					}
					return SUCCESS;
				}
				break;
#if 0
			/* Zeev might want to do something with this one day */
			case PHP_MODE_INDENT:
				open_file_for_scanning(&file_handle TSRMLS_CC);
				zend_indent();
				fclose(file_handle.handle.fp);
				return SUCCESS;
				break;
#endif
		}

#if PHP_FASTCGI
fastcgi_request_done:
#endif
		{
			char *path_translated;

			/*	Go through this trouble so that the memory manager doesn't warn
			 *	about SG(request_info).path_translated leaking
			 */
			if (SG(request_info).path_translated) {
				path_translated = strdup(SG(request_info).path_translated);
				STR_FREE(SG(request_info).path_translated);
				SG(request_info).path_translated = path_translated;
			}

			php_request_shutdown((void *) 0);
			if (exit_status == 0) {
				exit_status = EG(exit_status);
			}

			if (SG(request_info).path_translated) {
				free(SG(request_info).path_translated);
				SG(request_info).path_translated = NULL;
			}
			if (free_query_string && SG(request_info).query_string) {
				free(SG(request_info).query_string);
				SG(request_info).query_string = NULL;
			}

		}

#if PHP_FASTCGI
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
				fcgi_finish_request(&request);
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
		fcgi_shutdown();
#endif

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
#if PHP_FASTCGI
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
#endif

#ifndef PHP_WIN32
parent_out:
#endif

	SG(server_context) = NULL;
	php_module_shutdown(TSRMLS_C);
	sapi_shutdown();

#ifdef ZTS
	/*tsrm_shutdown();*/
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
