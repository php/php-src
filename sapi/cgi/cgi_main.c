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

static const opt_struct OPTIONS[] = {
	{'a', 0, "interactive"},
#ifndef PHP_WIN32
	{'b', 1, "bindpath"},
#endif
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
	{'-', 0, NULL} /* end of args */
};

/* true global.  this is retreived once only, even for fastcgi */
static long fix_pathinfo = 1;
static long discard_path = 0;
static long fcgi_logging = 1;

static long rfc2616_headers = 0;
static long cgi_nph = 0;

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

	if (fcgi_is_fastcgi()) {
		fcgi_request *request = (fcgi_request*) SG(server_context);
		long ret = fcgi_write(request, FCGI_STDOUT, str, str_length);
		if (ret <= 0) {
			return 0;
		}
		return ret;
	}

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
	if (fflush(stdout) == EOF) {
		php_handle_aborted_connection();
	}
}

#define SAPI_CGI_MAX_HEADER_LENGTH 1024

static int sapi_cgi_send_headers(sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	char buf[SAPI_CGI_MAX_HEADER_LENGTH];
	sapi_header_struct *h;
	zend_llist_position pos;

	if (SG(request_info).no_headers == 1) {
		return  SAPI_HEADER_SENT_SUCCESSFULLY;
	}

	if (cgi_nph || SG(sapi_headers).http_response_code != 200)
	{
		int len;

		if (rfc2616_headers && SG(sapi_headers).http_status_line) {
			len = snprintf(buf, SAPI_CGI_MAX_HEADER_LENGTH,
						   "%s\r\n", SG(sapi_headers).http_status_line);

			if (len > SAPI_CGI_MAX_HEADER_LENGTH) {
				len = SAPI_CGI_MAX_HEADER_LENGTH;
			}

		} else {
			len = sprintf(buf, "Status: %d\r\n", SG(sapi_headers).http_response_code);
		}

		PHPWRITE_H(buf, len);
	}

	h = (sapi_header_struct*)zend_llist_get_first_ex(&sapi_headers->headers, &pos);
	while (h) {
		/* prevent CRLFCRLF */
		if (h->header_len) {
			PHPWRITE_H(h->header, h->header_len);
			PHPWRITE_H("\r\n", 2);
		}
		h = (sapi_header_struct*)zend_llist_get_next_ex(&sapi_headers->headers, &pos);
	}
	PHPWRITE_H("\r\n", 2);

	return SAPI_HEADER_SENT_SUCCESSFULLY;
}


static int sapi_cgi_read_post(char *buffer, uint count_bytes TSRMLS_DC)
{
	uint read_bytes=0, tmp_read_bytes;
	char *pos = buffer;

	count_bytes = MIN(count_bytes, (uint) SG(request_info).content_length - SG(read_post_bytes));
	while (read_bytes < count_bytes) {
		if (fcgi_is_fastcgi()) {
			fcgi_request *request = (fcgi_request*) SG(server_context);
			tmp_read_bytes = fcgi_read(request, pos, count_bytes - read_bytes);
			pos += tmp_read_bytes;
		} else {
			tmp_read_bytes = read(0, buffer + read_bytes, count_bytes - read_bytes);
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
	/* when php is started by mod_fastcgi, no regular environment
	   is provided to PHP.  It is always sent to PHP at the start
	   of a request.  So we have to do our own lookup to get env
	   vars.  This could probably be faster somehow.  */
	if (fcgi_is_fastcgi()) {
		fcgi_request *request = (fcgi_request*) SG(server_context);
		return fcgi_getenv(request, name, name_len);
	}
	/*  if cgi, or fastcgi and not found in fcgi env
		check the regular environment */
	return getenv(name);
}

static char *_sapi_cgibin_putenv(char *name, char *value TSRMLS_DC)
{
	int name_len;
	int len;
	char *buf;

	if (!name) {
		return NULL;
	}
	name_len = strlen(name);

	/* when php is started by mod_fastcgi, no regular environment
	   is provided to PHP.  It is always sent to PHP at the start
	   of a request.  So we have to do our own lookup to get env
	   vars.  This could probably be faster somehow.  */
	if (fcgi_is_fastcgi()) {
		fcgi_request *request = (fcgi_request*) SG(server_context);
		return fcgi_putenv(request, name, name_len, value);
	}

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
		len = snprintf(buf, len - 1, "%s=%s", name, value);
		putenv(buf);
	}
#endif
#if !HAVE_UNSETENV
	if (!value) {
		len = snprintf(buf, len - 1, "%s=", name);
		putenv(buf);
	}
#endif
	return getenv(name);
}

static char *sapi_cgi_read_cookies(TSRMLS_D)
{
	return sapi_cgibin_getenv((char *) "HTTP_COOKIE", sizeof("HTTP_COOKIE")-1 TSRMLS_CC);
}

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
	if (fcgi_is_fastcgi()) {
		fcgi_request *request = (fcgi_request*) SG(server_context);
		HashPosition pos;
		zstr var;
		uint var_len;
		char **val;
		ulong idx;

		for (zend_hash_internal_pointer_reset_ex(&request->env, &pos);
		     zend_hash_get_current_key_ex(&request->env, &var, &var_len, &idx, 0, &pos) == HASH_KEY_IS_STRING &&
		     zend_hash_get_current_data_ex(&request->env, (void **) &val, &pos) == SUCCESS;
		     zend_hash_move_forward_ex(&request->env, &pos)) {
			php_register_variable(var.s, *val, array_ptr TSRMLS_CC);
		}
	}
	/* call php's original import as a catch-all */
	php_php_import_environment_variables(array_ptr TSRMLS_CC);
}

static void sapi_cgi_register_variables(zval *track_vars_array TSRMLS_DC)
{
	/* In CGI mode, we consider the environment to be a part of the server
	 * variables
	 */
	php_import_environment_variables(track_vars_array TSRMLS_CC);
	/* Build the special-case PHP_SELF variable for the CGI version */
	php_register_variable("PHP_SELF", (SG(request_info).request_uri ? SG(request_info).request_uri : ""), track_vars_array TSRMLS_CC);
}

static void sapi_cgi_log_message(char *message)
{
	if (fcgi_is_fastcgi() && fcgi_logging) {
		fcgi_request *request;
		TSRMLS_FETCH();
		
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
	if (php_module_startup(sapi_module, NULL, 0) == FAILURE) {
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

static zend_function_entry additional_functions[] = {
	ZEND_FE(dl, NULL)
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

	php_printf("Usage: %s [-q] [-h] [-s] [-v] [-i] [-f <file>]\n"
			   "       %s <file> [args...]\n"
			   "  -a               Run interactively\n"
#if !defined(PHP_WIN32)
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
			   "  -z <file>        Load Zend extension <file>.\n",
			   prog, prog);
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

	/* some broken servers do not have script_filename or argv0
	   an example, IIS configured in some ways.  then they do more
	   broken stuff and set path_translated to the cgi script location */
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
	   we are running in a cgi environment, since it is always
	   null otherwise.  otherwise, the filename
	   of the script will be retreived later via argc/argv */
	if (script_path_translated) {
		const char *auth;
		char *content_length = sapi_cgibin_getenv("CONTENT_LENGTH", sizeof("CONTENT_LENGTH")-1 TSRMLS_CC);
		char *content_type = sapi_cgibin_getenv("CONTENT_TYPE", sizeof("CONTENT_TYPE")-1 TSRMLS_CC);
		char *env_path_info = sapi_cgibin_getenv("PATH_INFO", sizeof("PATH_INFO")-1 TSRMLS_CC);
		char *env_script_name = sapi_cgibin_getenv("SCRIPT_NAME", sizeof("SCRIPT_NAME")-1 TSRMLS_CC);

		if (fix_pathinfo) {
			struct stat st;
			char *env_redirect_url = sapi_cgibin_getenv("REDIRECT_URL", sizeof("REDIRECT_URL")-1 TSRMLS_CC);
			char *env_document_root = sapi_cgibin_getenv("DOCUMENT_ROOT", sizeof("DOCUMENT_ROOT")-1 TSRMLS_CC);

			/* save the originals first for anything we change later */
			if (env_path_translated) {
				_sapi_cgibin_putenv("ORIG_PATH_TRANSLATED", env_path_translated TSRMLS_CC);
			}
			if (env_path_info) {
				_sapi_cgibin_putenv("ORIG_PATH_INFO", env_path_info TSRMLS_CC);
			}
			if (env_script_name) {
				_sapi_cgibin_putenv("ORIG_SCRIPT_NAME", env_script_name TSRMLS_CC);
			}
			if (env_script_filename) {
				_sapi_cgibin_putenv("ORIG_SCRIPT_FILENAME", env_script_filename TSRMLS_CC);
			}
			if (!env_document_root) {
				/* ini version of document root */
				if (!env_document_root) {
					env_document_root = PG(doc_root);
				}
				/* set the document root, this makes a more
				   consistent env for php scripts */
				if (env_document_root) {
					env_document_root = _sapi_cgibin_putenv("DOCUMENT_ROOT", env_document_root TSRMLS_CC);
					/* fix docroot */
					TRANSLATE_SLASHES(env_document_root);
				}
			}

			if (env_path_translated != NULL && env_redirect_url != NULL) {
				/* 
				   pretty much apache specific.  If we have a redirect_url
				   then our script_filename and script_name point to the
				   php executable
				*/
				script_path_translated = env_path_translated;
				/* we correct SCRIPT_NAME now in case we don't have PATH_INFO */
				env_script_name = _sapi_cgibin_putenv("SCRIPT_NAME", env_redirect_url TSRMLS_CC);
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
			if (script_path_translated && stat(script_path_translated, &st) == -1 ) {
				char *pt = estrdup(script_path_translated);
				int len = strlen(pt);
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
						int pilen = strlen(env_path_info);
						char *path_info = env_path_info + pilen - slen;

						env_path_info = _sapi_cgibin_putenv("PATH_INFO", path_info TSRMLS_CC);
						script_path_translated = _sapi_cgibin_putenv("SCRIPT_FILENAME", pt TSRMLS_CC);
						TRANSLATE_SLASHES(pt);

						/* figure out docroot
						   SCRIPT_FILENAME minus SCRIPT_NAME
						*/

						if (env_document_root)
						{
							int l = strlen(env_document_root);
							int path_translated_len = 0;
							char *path_translated = NULL;
							
							if (env_document_root[l - 1] == '/') {
								--l;
							}

							/* we have docroot, so we should have:
							 * DOCUMENT_ROOT=/docroot
							 * SCRIPT_FILENAME=/docroot/info.php
							 *
							 * SCRIPT_NAME is the portion of the path beyond docroot
							 */
							env_script_name = _sapi_cgibin_putenv("SCRIPT_NAME", pt + l TSRMLS_CC);

							/* PATH_TRANSATED = DOCUMENT_ROOT + PATH_INFO */
							path_translated_len = l + strlen(env_path_info) + 2;
							path_translated = (char *) emalloc(path_translated_len);
							*path_translated = 0;
							strncat(path_translated, env_document_root, l);
							strcat(path_translated, env_path_info);
							env_path_translated = _sapi_cgibin_putenv("PATH_TRANSLATED", path_translated TSRMLS_CC);
							efree(path_translated);
						} else if (env_script_name && 
								   strstr(pt, env_script_name)
						) {
							/* PATH_TRANSATED = PATH_TRANSATED - SCRIPT_NAME + PATH_INFO */
							int ptlen = strlen(pt) - strlen(env_script_name);
							int path_translated_len = ptlen + strlen(env_path_info) + 2;
							char *path_translated = NULL;

							path_translated = (char *) emalloc(path_translated_len);
							*path_translated = 0;
							strncat(path_translated, pt, ptlen);
							strcat(path_translated, env_path_info);
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
					script_path_translated = _sapi_cgibin_putenv("SCRIPT_FILENAME", NULL TSRMLS_CC);
					SG(sapi_headers).http_response_code = 404;
				}
				if (pt) {
					efree(pt);
				}
			} else {
				/* make sure path_info/translated are empty */
				script_path_translated = _sapi_cgibin_putenv("SCRIPT_FILENAME", script_path_translated TSRMLS_CC);
				_sapi_cgibin_putenv("PATH_INFO", NULL TSRMLS_CC);
				_sapi_cgibin_putenv("PATH_TRANSLATED", NULL TSRMLS_CC);
			}
			SG(request_info).request_uri = sapi_cgibin_getenv("SCRIPT_NAME", sizeof("SCRIPT_NAME")-1 TSRMLS_CC);
		} else {
			/* pre 4.3 behaviour, shouldn't be used but provides BC */
			if (env_path_info) {
				SG(request_info).request_uri = env_path_info;
			} else {
				SG(request_info).request_uri = env_script_name;
			}
			if (!discard_path && env_path_translated) {
				script_path_translated = env_path_translated;
			}
		}

		SG(request_info).request_method = sapi_cgibin_getenv("REQUEST_METHOD", sizeof("REQUEST_METHOD")-1 TSRMLS_CC);
		/* FIXME - Work out proto_num here */
		SG(request_info).query_string = sapi_cgibin_getenv("QUERY_STRING", sizeof("REQUEST_METHOD")-1 TSRMLS_CC);
		/* some server configurations allow '..' to slip through in the
		   translated path.   We'll just refuse to handle such a path. */
		if (script_path_translated && !strstr(script_path_translated, "..")) {
			SG(request_info).path_translated = estrdup(script_path_translated);
		}
		SG(request_info).content_type = (content_type ? content_type : "" );
		SG(request_info).content_length = (content_length ? atoi(content_length) : 0);
		
		/* The CGI RFC allows servers to pass on unvalidated Authorization data */
		auth = sapi_cgibin_getenv("HTTP_AUTHORIZATION", sizeof("HTTP_AUTHORIZATION")-1 TSRMLS_CC);
		php_handle_auth_data(auth TSRMLS_CC);
	}
}
/* }}} */

/**
 * Clean up child processes upon exit
 */
void fastcgi_cleanup(int signal)
{
#ifdef DEBUG_FASTCGI
	fprintf(stderr, "FastCGI shutdown, pid %d\n", getpid());
#endif

#ifndef PHP_WIN32
	sigaction(SIGTERM, &old_term, 0);

	/* Kill all the processes in our process group */
	kill(-pgroup, SIGTERM);
#endif

	/* We should exit at this point, but MacOSX doesn't seem to */
	exit(0);
}

#ifndef PHP_WIN32
static int is_port_number(const char *bindpath)
{
	while (*bindpath) {
		if (*bindpath < '0' || *bindpath > '9') {
			return 0;
		}
		bindpath++;
	}
	return 1;
}
#endif

/* {{{ main
 */
int main(int argc, char *argv[])
{
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
	zend_compiler_globals *compiler_globals;
	zend_executor_globals *executor_globals;
	php_core_globals *core_globals;
	sapi_globals_struct *sapi_globals;
	void ***tsrm_ls;
#endif

	int max_requests = 500;
	int requests = 0;
	int fastcgi = fcgi_is_fastcgi();
#ifndef PHP_WIN32
	char *bindpath = NULL;
#endif
	int fcgi_fd = 0;
	fcgi_request request;
#ifdef PHP_WIN32
	long impersonate = 0;
#else
	int status = 0;
#endif

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
#endif

	sapi_startup(&cgi_sapi_module);

#ifdef PHP_WIN32
	_fmode = _O_BINARY; /* sets default for file streams to binary */
	setmode(_fileno(stdin),  O_BINARY);	/* make the stdio mode be binary */
	setmode(_fileno(stdout), O_BINARY);	/* make the stdio mode be binary */
	setmode(_fileno(stderr), O_BINARY);	/* make the stdio mode be binary */
#endif

	if (!fastcgi) {
		/* Make sure we detect we are a cgi - a bit redundancy here,
		   but the default case is that we have to check only the first one. */
		if (getenv("SERVER_SOFTWARE") ||
			getenv("SERVER_NAME") ||
			getenv("GATEWAY_INTERFACE") ||
			getenv("REQUEST_METHOD")) {
			cgi = 1;
		}
	}

	while ((c = php_getopt(argc, argv, OPTIONS, &php_optarg, &php_optind, 0)) != -1) {
		switch (c) {
			case 'c':
				cgi_sapi_module.php_ini_path_override = strdup(php_optarg);
				break;
			case 'n':
				cgi_sapi_module.php_ini_ignore = 1;
				break;
#ifndef PHP_WIN32
			/* if we're started on command line, check to see if
			   we are being started as an 'external' fastcgi
			   server by accepting a bindpath parameter. */
			case 'd': { 
				/* define ini entries on command line */
				int len = strlen(php_optarg);

				if (strchr(php_optarg, '=')) {
					cgi_sapi_module.ini_entries = realloc(cgi_sapi_module.ini_entries, ini_entries_len + len + sizeof("\n\0"));
					memcpy(cgi_sapi_module.ini_entries + ini_entries_len, php_optarg, len);
					memcpy(cgi_sapi_module.ini_entries + ini_entries_len + len, "\n\0", sizeof("\n\0"));
					ini_entries_len += len + sizeof("\n\0") - 2;
				} else {
					cgi_sapi_module.ini_entries = realloc(cgi_sapi_module.ini_entries, ini_entries_len + len + sizeof("=1\n\0"));
					memcpy(cgi_sapi_module.ini_entries + ini_entries_len, php_optarg, len);
					memcpy(cgi_sapi_module.ini_entries + ini_entries_len + len, "=1\n\0", sizeof("=1\n\0"));
					ini_entries_len += len + sizeof("=1\n\0") - 2;
				}
				break;
			}
			case 'b':
				if (!fastcgi) {
					bindpath = strdup(php_optarg);
				}
				break;
#endif
		}

	}
	php_optind = orig_optind;
	php_optarg = orig_optarg;

#ifdef ZTS
	compiler_globals = ts_resource(compiler_globals_id);
	executor_globals = ts_resource(executor_globals_id);
	core_globals = ts_resource(core_globals_id);
	sapi_globals = ts_resource(sapi_globals_id);
	tsrm_ls = ts_resource(0);
	SG(request_info).path_translated = NULL;
#endif

	cgi_sapi_module.executable_location = argv[0];
	cgi_sapi_module.additional_functions = additional_functions;

	/* startup after we get the above ini override se we get things right */
	if (php_module_startup(&cgi_sapi_module, NULL, 0) == FAILURE) {
#ifdef ZTS
		tsrm_shutdown();
#endif
		return FAILURE;
	}

	if (cgi) {
		long force_redirect;
		char *redirect_status_env;
	
		/* check force_cgi after startup, so we have proper output */
		if (cfg_get_long("cgi.force_redirect", &force_redirect) == FAILURE) {
			force_redirect = 1;
		}
		if (force_redirect) {
			if (cfg_get_string("cgi.redirect_status_env", &redirect_status_env) == FAILURE) {
				redirect_status_env = NULL;
			}
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
				&& (!redirect_status_env || !getenv(redirect_status_env))
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
	}

	if (cfg_get_long("cgi.fix_pathinfo", &fix_pathinfo) == FAILURE) {
		fix_pathinfo = 1;
	}

	if (cfg_get_long("cgi.discard_path", &discard_path) == FAILURE) {
		discard_path = 0;
	}

	if (cfg_get_long("fastcgi.logging", &fcgi_logging) == FAILURE) {
		fcgi_logging = 1;
	}


	/* Check wheater to send RFC2616 style headers compatible with
	 * PHP versions 4.2.3 and earlier compatible with web servers
	 * such as IIS. Default is informal CGI RFC header compatible
	 * with Apache.
	 */
	if (cfg_get_long("cgi.rfc2616_headers", &rfc2616_headers) == FAILURE) {
		rfc2616_headers = 0;
	}

	if (cfg_get_long("cgi.nph", &cgi_nph) == FAILURE) {
		cgi_nph = 0;
	}

#ifndef PHP_WIN32
	/* for windows, socket listening is broken in the fastcgi library itself
	   so dissabling this feature on windows till time is available to fix it */
	if (bindpath) {
		/* Pass on the arg to the FastCGI library, with one exception.
		 * If just a port is specified, then we prepend a ':' onto the
		 * path (it's what the fastcgi library expects)
		 */		
		if (strchr(bindpath, ':') == NULL && is_port_number(bindpath)) {
			char *tmp;

			tmp = malloc(strlen(bindpath) + 2);
			tmp[0] = ':';
			memcpy(tmp + 1, bindpath, strlen(bindpath) + 1);

			fcgi_fd = fcgi_listen(tmp, 128);
			free(tmp);
		} else {
			fcgi_fd = fcgi_listen(bindpath, 128);
		}
		if (fcgi_fd < 0) {
			fprintf(stderr, "Couldn't create FastCGI listen socket on port %s\n", bindpath);
#ifdef ZTS
			tsrm_shutdown();
#endif
			return FAILURE;
		}
		fastcgi = fcgi_is_fastcgi();
	}
#endif
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
		children = atoi(getenv("PHP_FCGI_CHILDREN"));
		if (children < 0) {
			fprintf(stderr, "PHP_FCGI_CHILDREN is not valid\n");
			return FAILURE;
		}
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
				while (wait(&status) < 0) {
				}
				running--;
			}
		}
	} else {
		parent = 0;
	}

#endif /* WIN32 */
	}

	zend_first_try {
		while ((c = php_getopt(argc, argv, OPTIONS, &php_optarg, &php_optind, 1)) != -1) {
			switch (c) {
				case 'h':
				case '?':
					no_headers = 1;
					php_output_tearup();
					SG(headers_sent) = 1;
					php_cgi_usage(argv[0]);
					php_output_teardown();
					exit(1);
					break;
			}
		}
		php_optind = orig_optind;
		php_optarg = orig_optarg;

		/* start of FAST CGI loop */
		/* Initialise FastCGI request structure */
#ifdef PHP_WIN32
		/* attempt to set security impersonation for fastcgi
		   will only happen on NT based OS, others will ignore it. */
		if (fastcgi) {
			if (cfg_get_long("fastcgi.impersonate", &impersonate) == FAILURE) {
				impersonate = 0;
			}
			if (impersonate) fcgi_impersonate();
		}
#endif
		while (!fastcgi || fcgi_accept_request(&request) >= 0) {
			SG(server_context) = (void *) &request;
			init_request_info(TSRMLS_C);
			CG(interactive) = 0;

			if (!cgi && !fastcgi) {
				if (cgi_sapi_module.php_ini_path_override && cgi_sapi_module.php_ini_ignore) {
					no_headers = 1;
					php_output_tearup();
					SG(headers_sent) = 1;
					php_printf("You cannot use both -n and -c switch. Use -h for help.\n");
					php_output_teardown();
					exit(1);
				}

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
							script_file = estrdup(php_optarg);
							no_headers = 1;
							/* arguments after the file are considered script args */
							SG(request_info).argc = argc - (php_optind - 1);
							SG(request_info).argv = &argv[php_optind - 1];
							break;

						case 'i': /* php info & quit */
							if (php_request_startup(TSRMLS_C) == FAILURE) {
								php_module_shutdown(TSRMLS_C);
								return FAILURE;
							}
							if (no_headers) {
								SG(headers_sent) = 1;
								SG(request_info).no_headers = 1;
							}
							php_print_info(0xFFFFFFFF TSRMLS_CC);
							php_output_teardown();
							exit(0);
							break;

	  				case 'l': /* syntax check mode */
							no_headers = 1;
							behavior = PHP_MODE_LINT;
							break;

						case 'm': /* list compiled in modules */
							php_output_tearup();
							SG(headers_sent) = 1;
							php_printf("[PHP Modules]\n");
							print_modules(TSRMLS_C);
							php_printf("\n[Zend Modules]\n");
							print_extensions(TSRMLS_C);
							php_printf("\n");
							php_output_teardown();
							exit(0);
							break;

#if 0 /* not yet operational, see also below ... */
	  				case '': /* generate indented source mode*/
							behavior=PHP_MODE_INDENT;
							break;
#endif

	  				case 'q': /* do not generate HTTP headers */
							no_headers = 1;
							break;

	  				case 's': /* generate highlighted HTML from source */
							behavior = PHP_MODE_HIGHLIGHT;
							break;

						case 'v': /* show php version & quit */
							no_headers = 1;
							if (php_request_startup(TSRMLS_C) == FAILURE) {
								php_module_shutdown(TSRMLS_C);
								return FAILURE;
							}
							if (no_headers) {
								SG(headers_sent) = 1;
								SG(request_info).no_headers = 1;
							}
#if ZEND_DEBUG
							php_printf("PHP %s (%s) (built: %s %s) (DEBUG)\nCopyright (c) 1997-2006 The PHP Group\n%s", PHP_VERSION, sapi_module.name, __DATE__, __TIME__, get_zend_version());
#else
							php_printf("PHP %s (%s) (built: %s %s)\nCopyright (c) 1997-2006 The PHP Group\n%s", PHP_VERSION, sapi_module.name, __DATE__, __TIME__, get_zend_version());
#endif
							php_output_teardown();
							exit(0);
							break;

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
				}

				if (no_headers) {
					SG(headers_sent) = 1;
					SG(request_info).no_headers = 1;
				}

				if (!SG(request_info).path_translated && argc > php_optind) {
					/* arguments after the file are considered script args */
					SG(request_info).argc = argc - php_optind;
					SG(request_info).argv = &argv[php_optind];
					/* file is on command line, but not in -f opt */
					SG(request_info).path_translated = estrdup(argv[php_optind++]);
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
					len = 0;
					for (i = php_optind; i < argc; i++) {
						len += strlen(argv[i]) + 1;
					}

					s = malloc(len + 1);	/* leak - but only for command line version, so ok */
					*s = '\0';			/* we are pretending it came from the environment  */
					for (i = php_optind, len = 0; i < argc; i++) {
						strcat(s, argv[i]);
						if (i < (argc - 1)) {
							strcat(s, PG(arg_separator).input);
						}
					}
					SG(request_info).query_string = s;
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
			   get path_translated */
			if (php_request_startup(TSRMLS_C) == FAILURE) {
				if (fastcgi) {
					fcgi_finish_request(&request);
				}
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
			if (cgi || SG(request_info).path_translated) {
				retval = php_fopen_primary_script(&file_handle TSRMLS_CC);
			}
			/* 
				if we are unable to open path_translated and we are not
				running from shell (so fp == NULL), then fail.
			*/
			if (retval == FAILURE && file_handle.handle.fp == NULL) {
				SG(sapi_headers).http_response_code = 404;
				PUTS("No input file specified.\n");
				/* we want to serve more requests if this is fastcgi
				   so cleanup and continue, request shutdown is
				   handled later */
				if (fastcgi) {
					goto fastcgi_request_done;
				}
				php_request_shutdown((void *) 0);
				php_module_shutdown(TSRMLS_C);
				return FAILURE;
			}

			if (file_handle.handle.fp && (file_handle.handle.fp != stdin)) {
				/* #!php support */
				c = fgetc(file_handle.handle.fp);
				if (c == '#') {
					while (c != 10 && c != 13) {
						c = fgetc(file_handle.handle.fp);	/* skip to end of line */
					}
					/* handle situations where line is terminated by \r\n */
					if (c == 13) {
						if (fgetc(file_handle.handle.fp) != 10) {
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
							fclose(file_handle.handle.fp);
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
					fclose(file_handle.handle.fp);
					php_output_teardown();
					return SUCCESS;
					break;
#endif
			}

fastcgi_request_done:
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
			}

			if (!fastcgi) break;
			/* only fastcgi will get here */
			requests++;
			if (max_requests && (requests == max_requests)) {
				fcgi_finish_request(&request);
#ifndef PHP_WIN32
				if (bindpath) {
					free(bindpath);
				}
#endif
				break;
			}
			/* end of fastcgi loop */
		}

		if (cgi_sapi_module.php_ini_path_override) {
			free(cgi_sapi_module.php_ini_path_override);
		}
		if (cgi_sapi_module.ini_entries) {
			free(cgi_sapi_module.ini_entries);
		}
	} zend_catch {
		exit_status = 255;
	} zend_end_try();

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
