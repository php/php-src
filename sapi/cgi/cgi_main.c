/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
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
   |          Zeev Suraski <zeev@php.net>                                 |
   | FastCGI: Ben Mansell <php@slimyhorror.com>                           |
   |          Shane Caraveo <shane@caraveo.com>                           |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "php_globals.h"
#include "php_variables.h"
#include "zend_modules.h"

#include "SAPI.h"

#include <stdio.h>

#ifdef PHP_WIN32
# include "win32/time.h"
# include "win32/signal.h"
# include "win32/winutil.h"
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
#include "http_status_codes.h"
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

#include "php_getopt.h"

#include "fastcgi.h"

#if defined(PHP_WIN32) && defined(HAVE_OPENSSL)
# include "openssl/applink.c"
#endif

#ifdef HAVE_VALGRIND
# include "valgrind/callgrind.h"
#endif

#ifndef PHP_WIN32
/* XXX this will need to change later when threaded fastcgi is implemented.  shane */
struct sigaction act, old_term, old_quit, old_int;
#endif

static void (*php_php_import_environment_variables)(zval *array_ptr);

/* these globals used for forking children on unix systems */
/**
 * Number of child processes that will get created to service requests
 */
static int children = 0;


/**
 * Set to non-zero if we are the parent process
 */
static int parent = 1;

#ifndef PHP_WIN32
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
	HashTable user_config_cache;
	char *redirect_status_env;
	zend_bool rfc2616_headers;
	zend_bool nph;
	zend_bool check_shebang_line;
	zend_bool fix_pathinfo;
	zend_bool force_redirect;
	zend_bool discard_path;
	zend_bool fcgi_logging;
#ifdef PHP_WIN32
	zend_bool impersonate;
#endif
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

static void user_config_cache_entry_dtor(zval *el)
{
	user_config_cache_entry *entry = (user_config_cache_entry *)Z_PTR_P(el);
	zend_hash_destroy(entry->user_config);
	free(entry->user_config);
	free(entry);
}
/* }}} */

#ifdef ZTS
static int php_cgi_globals_id;
#define CGIG(v) ZEND_TSRMG(php_cgi_globals_id, php_cgi_globals_struct *, v)
#if defined(PHP_WIN32)
ZEND_TSRMLS_CACHE_DEFINE()
#endif
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

#ifdef PHP_WIN32
#define WIN32_MAX_SPAWN_CHILDREN 64
HANDLE kid_cgi_ps[WIN32_MAX_SPAWN_CHILDREN];
int kids, cleaning_up = 0;
HANDLE job = NULL;
JOBOBJECT_EXTENDED_LIMIT_INFORMATION job_info = { 0 };
CRITICAL_SECTION cleanup_lock;
#endif

#ifndef HAVE_ATTRIBUTE_WEAK
static void fcgi_log(int type, const char *format, ...) {
	va_list ap;

	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
}
#endif

static int print_module_info(zval *element)
{
	zend_module_entry *module = Z_PTR_P(element);
	php_printf("%s\n", module->name);
	return ZEND_HASH_APPLY_KEEP;
}

static int module_name_cmp(const void *a, const void *b)
{
	Bucket *f = (Bucket *) a;
	Bucket *s = (Bucket *) b;

	return strcasecmp(	((zend_module_entry *)Z_PTR(f->val))->name,
						((zend_module_entry *)Z_PTR(s->val))->name);
}

static void print_modules(void)
{
	HashTable sorted_registry;

	zend_hash_init(&sorted_registry, 64, NULL, NULL, 1);
	zend_hash_copy(&sorted_registry, &module_registry, NULL);
	zend_hash_sort(&sorted_registry, module_name_cmp, 0);
	zend_hash_apply(&sorted_registry, print_module_info);
	zend_hash_destroy(&sorted_registry);
}

static int print_extension_info(zend_extension *ext, void *arg)
{
	php_printf("%s\n", ext->name);
	return 0;
}

static int extension_name_cmp(const zend_llist_element **f, const zend_llist_element **s)
{
	zend_extension *fe = (zend_extension*)(*f)->data;
	zend_extension *se = (zend_extension*)(*s)->data;
	return strcmp(fe->name, se->name);
}

static void print_extensions(void)
{
	zend_llist sorted_exts;

	zend_llist_copy(&sorted_exts, &zend_extensions);
	sorted_exts.dtor = NULL;
	zend_llist_sort(&sorted_exts, extension_name_cmp);
	zend_llist_apply_with_argument(&sorted_exts, (llist_apply_with_arg_func_t) print_extension_info, NULL);
	zend_llist_destroy(&sorted_exts);
}

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif

static inline size_t sapi_cgi_single_write(const char *str, size_t str_length)
{
#ifdef PHP_WRITE_STDOUT
	int ret;

	ret = write(STDOUT_FILENO, str, str_length);
	if (ret <= 0) return 0;
	return ret;
#else
	size_t ret;

	ret = fwrite(str, 1, MIN(str_length, 16384), stdout);
	return ret;
#endif
}

static size_t sapi_cgi_ub_write(const char *str, size_t str_length)
{
	const char *ptr = str;
	size_t remaining = str_length;
	size_t ret;

	while (remaining > 0) {
		ret = sapi_cgi_single_write(ptr, remaining);
		if (!ret) {
			php_handle_aborted_connection();
			return str_length - remaining;
		}
		ptr += ret;
		remaining -= ret;
	}

	return str_length;
}

static size_t sapi_fcgi_ub_write(const char *str, size_t str_length)
{
	const char *ptr = str;
	size_t remaining = str_length;
	fcgi_request *request = (fcgi_request*) SG(server_context);

	while (remaining > 0) {
		int to_write = remaining > INT_MAX ? INT_MAX : (int)remaining;
		int ret = fcgi_write(request, FCGI_STDOUT, ptr, to_write);

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
		!parent &&
		request && !fcgi_flush(request, 0)) {

		php_handle_aborted_connection();
	}
}

#define SAPI_CGI_MAX_HEADER_LENGTH 1024

static int sapi_cgi_send_headers(sapi_headers_struct *sapi_headers)
{
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
		char buf[SAPI_CGI_MAX_HEADER_LENGTH];

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
					http_response_status_code_pair *err = (http_response_status_code_pair*)http_status_map;

					while (err->code != 0) {
						if (err->code == SG(sapi_headers).http_response_code) {
							break;
						}
						err++;
					}
					if (err->str) {
						len = slprintf(buf, sizeof(buf), "Status: %d %s\r\n", SG(sapi_headers).http_response_code, err->str);
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

static size_t sapi_cgi_read_post(char *buffer, size_t count_bytes)
{
	size_t read_bytes = 0;
	int tmp_read_bytes;
	size_t remaining_bytes;

	assert(SG(request_info).content_length >= SG(read_post_bytes));

	remaining_bytes = (size_t)(SG(request_info).content_length - SG(read_post_bytes));

	count_bytes = MIN(count_bytes, remaining_bytes);
	while (read_bytes < count_bytes) {
#ifdef PHP_WIN32
		size_t diff = count_bytes - read_bytes;
		unsigned int to_read = (diff > UINT_MAX) ? UINT_MAX : (unsigned int)diff;

		tmp_read_bytes = read(STDIN_FILENO, buffer + read_bytes, to_read);
#else
		tmp_read_bytes = read(STDIN_FILENO, buffer + read_bytes, count_bytes - read_bytes);
#endif
		if (tmp_read_bytes <= 0) {
			break;
		}
		read_bytes += tmp_read_bytes;
	}
	return read_bytes;
}

static size_t sapi_fcgi_read_post(char *buffer, size_t count_bytes)
{
	size_t read_bytes = 0;
	int tmp_read_bytes;
	fcgi_request *request = (fcgi_request*) SG(server_context);
	size_t remaining = SG(request_info).content_length - SG(read_post_bytes);

	if (remaining < count_bytes) {
		count_bytes = remaining;
	}
	while (read_bytes < count_bytes) {
		size_t diff = count_bytes - read_bytes;
		int to_read = (diff > INT_MAX) ? INT_MAX : (int)diff;

		tmp_read_bytes = fcgi_read(request, buffer + read_bytes, to_read);
		if (tmp_read_bytes <= 0) {
			break;
		}
		read_bytes += tmp_read_bytes;
	}
	return read_bytes;
}

#ifdef PHP_WIN32
/* The result needs to be freed! See sapi_getenv(). */
static char *cgi_getenv_win32(const char *name, size_t name_len)
{
	char *ret = NULL;
	wchar_t *keyw, *valw;
	size_t size;
	int rc;

	keyw = php_win32_cp_conv_any_to_w(name, name_len, PHP_WIN32_CP_IGNORE_LEN_P);
	if (!keyw) {
		return NULL;
	}

	rc = _wgetenv_s(&size, NULL, 0, keyw);
	if (rc || 0 == size) {
		free(keyw);
		return NULL;
	}

	valw = emalloc((size + 1) * sizeof(wchar_t));

	rc = _wgetenv_s(&size, valw, size, keyw);
	if (!rc) {
		ret = php_win32_cp_w_to_any(valw);
	}

	free(keyw);
	efree(valw);

	return ret;
}
#endif

static char *sapi_cgi_getenv(char *name, size_t name_len)
{
#ifndef PHP_WIN32
	return getenv(name);
#else
	return cgi_getenv_win32(name, name_len);
#endif
}

static char *sapi_fcgi_getenv(char *name, size_t name_len)
{
	/* when php is started by mod_fastcgi, no regular environment
	 * is provided to PHP.  It is always sent to PHP at the start
	 * of a request.  So we have to do our own lookup to get env
	 * vars.  This could probably be faster somehow.  */
	fcgi_request *request = (fcgi_request*) SG(server_context);
	char *ret = fcgi_getenv(request, name, (int)name_len);

#ifndef PHP_WIN32
	if (ret) return ret;
	/*  if cgi, or fastcgi and not found in fcgi env
		check the regular environment */
	return getenv(name);
#else
	if (ret) {
		/* The functions outside here don't know, where does it come
			from. They'll need to free the returned memory as it's
			not necessary from the fcgi env. */
		return strdup(ret);
	}
	/*  if cgi, or fastcgi and not found in fcgi env
		check the regular environment */
	return cgi_getenv_win32(name, name_len);
#endif
}

static char *_sapi_cgi_putenv(char *name, size_t name_len, char *value)
{
#if !HAVE_SETENV || !HAVE_UNSETENV
	size_t len;
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

static char *sapi_cgi_read_cookies(void)
{
	return getenv("HTTP_COOKIE");
}

static char *sapi_fcgi_read_cookies(void)
{
	fcgi_request *request = (fcgi_request*) SG(server_context);

	return FCGI_GETENV(request, "HTTP_COOKIE");
}

static void cgi_php_load_env_var(char *var, unsigned int var_len, char *val, unsigned int val_len, void *arg)
{
	zval *array_ptr = (zval*)arg;
	int filter_arg = (Z_ARR_P(array_ptr) == Z_ARR(PG(http_globals)[TRACK_VARS_ENV]))?PARSE_ENV:PARSE_SERVER;
	size_t new_val_len;

	if (sapi_module.input_filter(filter_arg, var, &val, strlen(val), &new_val_len)) {
		php_register_variable_safe(var, val, new_val_len, array_ptr);
	}
}

static void cgi_php_import_environment_variables(zval *array_ptr)
{
	if (PG(variables_order) && (strchr(PG(variables_order),'E') || strchr(PG(variables_order),'e'))) {
		if (Z_TYPE(PG(http_globals)[TRACK_VARS_ENV]) != IS_ARRAY) {
			zend_is_auto_global_str("_ENV", sizeof("_ENV")-1);
		}

		if (Z_TYPE(PG(http_globals)[TRACK_VARS_ENV]) == IS_ARRAY &&
			Z_ARR_P(array_ptr) != Z_ARR(PG(http_globals)[TRACK_VARS_ENV])) {
			zend_array_destroy(Z_ARR_P(array_ptr));
			Z_ARR_P(array_ptr) = zend_array_dup(Z_ARR(PG(http_globals)[TRACK_VARS_ENV]));
			return;
		}
	}

	/* call php's original import as a catch-all */
	php_php_import_environment_variables(array_ptr);

	if (fcgi_is_fastcgi()) {
		fcgi_request *request = (fcgi_request*) SG(server_context);
		fcgi_loadenv(request, cgi_php_load_env_var, array_ptr);
	}
}

static void sapi_cgi_register_variables(zval *track_vars_array)
{
	size_t php_self_len;
	char *php_self;

	/* In CGI mode, we consider the environment to be a part of the server
	 * variables
	 */
	php_import_environment_variables(track_vars_array);

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
			size_t path_info_len = strlen(path_info);

			if (script_name) {
				size_t script_name_len = strlen(script_name);

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
		if (sapi_module.input_filter(PARSE_SERVER, "PHP_SELF", &php_self, php_self_len, &php_self_len)) {
			php_register_variable_safe("PHP_SELF", php_self, php_self_len, track_vars_array);
		}
		if (free_php_self) {
			free_alloca(php_self, use_heap);
		}
	} else {
		php_self = SG(request_info).request_uri ? SG(request_info).request_uri : "";
		php_self_len = strlen(php_self);
		if (sapi_module.input_filter(PARSE_SERVER, "PHP_SELF", &php_self, php_self_len, &php_self_len)) {
			php_register_variable_safe("PHP_SELF", php_self, php_self_len, track_vars_array);
		}
	}
}

static void sapi_cgi_log_message(char *message, int syslog_type_int)
{
	if (fcgi_is_fastcgi() && CGIG(fcgi_logging)) {
		fcgi_request *request;

		request = (fcgi_request*) SG(server_context);
		if (request) {
			int ret, len = (int)strlen(message);
			char *buf = malloc(len+2);

			memcpy(buf, message, len);
			memcpy(buf + len, "\n", sizeof("\n"));
			ret = fcgi_write(request, FCGI_STDERR, buf, (int)(len + 1));
			free(buf);
			if (ret < 0) {
				php_handle_aborted_connection();
			}
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
static void php_cgi_ini_activate_user_config(char *path, size_t path_len, const char *doc_root, size_t doc_root_len)
{
	user_config_cache_entry *new_entry, *entry;
	time_t request_time = (time_t)sapi_get_request_time();

	/* Find cached config entry: If not found, create one */
	if ((entry = zend_hash_str_find_ptr(&CGIG(user_config_cache), path, path_len)) == NULL) {
		new_entry = pemalloc(sizeof(user_config_cache_entry), 1);
		new_entry->expires = 0;
		new_entry->user_config = (HashTable *) pemalloc(sizeof(HashTable), 1);
		zend_hash_init(new_entry->user_config, 8, NULL, (dtor_func_t) config_zval_dtor, 1);
		entry = zend_hash_str_update_ptr(&CGIG(user_config_cache), path, path_len, new_entry);
	}

	/* Check whether cache entry has expired and rescan if it is */
	if (request_time > entry->expires) {
		char *real_path = NULL;
		char *s1, *s2;
		size_t s_len;

		/* Clear the expired config */
		zend_hash_clean(entry->user_config);

		if (!IS_ABSOLUTE_PATH(path, path_len)) {
			size_t real_path_len;
			real_path = tsrm_realpath(path, NULL);
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
			char *ptr = s2 + doc_root_len;
			while ((ptr = strchr(ptr, DEFAULT_SLASH)) != NULL) {
				*ptr = 0;
				php_parse_user_ini_file(path, PG(user_ini_filename), entry->user_config);
				*ptr = '/';
				ptr++;
			}
		} else {
			php_parse_user_ini_file(path, PG(user_ini_filename), entry->user_config);
		}

		if (real_path) {
			efree(real_path);
		}
		entry->expires = request_time + PG(user_ini_cache_ttl);
	}

	/* Activate ini entries with values from the user config hash */
	php_ini_activate_config(entry->user_config, PHP_INI_PERDIR, PHP_INI_STAGE_HTACCESS);
}
/* }}} */

static int sapi_cgi_activate(void)
{
	/* PATH_TRANSLATED should be defined at this stage but better safe than sorry :) */
	if (!SG(request_info).path_translated) {
		return FAILURE;
	}

	if (php_ini_has_per_host_config()) {
		char *server_name;

		/* Activate per-host-system-configuration defined in php.ini and stored into configuration_hash during startup */
		if (fcgi_is_fastcgi()) {
			fcgi_request *request = (fcgi_request*) SG(server_context);

			server_name = FCGI_GETENV(request, "SERVER_NAME");
		} else {
			server_name = getenv("SERVER_NAME");
		}
		/* SERVER_NAME should also be defined at this stage..but better check it anyway */
		if (server_name) {
			size_t server_name_len = strlen(server_name);
			server_name = estrndup(server_name, server_name_len);
			zend_str_tolower(server_name, server_name_len);
			php_ini_activate_per_host_config(server_name, server_name_len);
			efree(server_name);
		}
	}

	if (php_ini_has_per_dir_config() ||
		(PG(user_ini_filename) && *PG(user_ini_filename))
	) {
		char *path;
		size_t path_len;

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
		php_ini_activate_per_dir_config(path, path_len); /* Note: for global settings sake we check from root to path */

		/* Load and activate user ini files in path starting from DOCUMENT_ROOT */
		if (PG(user_ini_filename) && *PG(user_ini_filename)) {
			char *doc_root;

			if (fcgi_is_fastcgi()) {
				fcgi_request *request = (fcgi_request*) SG(server_context);

				doc_root = FCGI_GETENV(request, "DOCUMENT_ROOT");
			} else {
				doc_root = getenv("DOCUMENT_ROOT");
			}
			/* DOCUMENT_ROOT should also be defined at this stage..but better check it anyway */
			if (doc_root) {
				size_t doc_root_len = strlen(doc_root);
				if (doc_root_len > 0 && IS_SLASH(doc_root[doc_root_len - 1])) {
					--doc_root_len;
				}
#ifdef PHP_WIN32
				/* paths on windows should be case-insensitive */
				doc_root = estrndup(doc_root, doc_root_len);
				zend_str_tolower(doc_root, doc_root_len);
#endif
				php_cgi_ini_activate_user_config(path, path_len, doc_root, doc_root_len);

#ifdef PHP_WIN32
				efree(doc_root);
#endif
			}
		}

		efree(path);
	}

	return SUCCESS;
}

static int sapi_cgi_deactivate(void)
{
	/* flush only when SAPI was started. The reasons are:
		1. SAPI Deactivate is called from two places: module init and request shutdown
		2. When the first call occurs and the request is not set up, flush fails on FastCGI.
	*/
	if (SG(sapi_started)) {
		if (fcgi_is_fastcgi()) {
			if (
				!parent &&
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
	PHP_FE_END
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
	((has_env) ? \
		FCGI_GETENV(request, name) : \
    	getenv(name))

#define CGI_PUTENV(name, value) \
	((has_env) ? \
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
static void init_request_info(fcgi_request *request)
{
	int has_env = fcgi_has_env(request);
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
	 * of the script will be retrieved later via argc/argv */
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
			zend_stat_t st;
			char *real_path = NULL;
			char *env_redirect_url = CGI_GETENV("REDIRECT_URL");
			char *env_document_root = CGI_GETENV("DOCUMENT_ROOT");
			char *orig_path_translated = env_path_translated;
			char *orig_path_info = env_path_info;
			char *orig_script_name = env_script_name;
			char *orig_script_filename = env_script_filename;
			size_t script_path_translated_len;

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
				(real_path = tsrm_realpath(script_path_translated, NULL)) == NULL)
			) {
				char *pt = estrndup(script_path_translated, script_path_translated_len);
				size_t len = script_path_translated_len;
				char *ptr;

				while ((ptr = strrchr(pt, '/')) || (ptr = strrchr(pt, '\\'))) {
					*ptr = 0;
					if (zend_stat(pt, &st) == 0 && S_ISREG(st.st_mode)) {
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
						size_t slen = len - strlen(pt);
						size_t pilen = env_path_info ? strlen(env_path_info) : 0;
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
							size_t l = strlen(env_document_root);
							size_t path_translated_len = 0;
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
							size_t ptlen = strlen(pt) - strlen(env_script_name);
							size_t path_translated_len = ptlen + (env_path_info ? strlen(env_path_info) : 0);

							char *path_translated = (char *) emalloc(path_translated_len + 1);
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

		SG(request_info).request_method = CGI_GETENV("REQUEST_METHOD");
		/* FIXME - Work out proto_num here */
		SG(request_info).query_string = CGI_GETENV("QUERY_STRING");
		SG(request_info).content_type = (content_type ? content_type : "" );
		SG(request_info).content_length = (content_length ? atol(content_length) : 0);

		/* The CGI RFC allows servers to pass on unvalidated Authorization data */
		auth = CGI_GETENV("HTTP_AUTHORIZATION");
		php_handle_auth_data(auth);
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
#else
BOOL WINAPI fastcgi_cleanup(DWORD sig)
{
	int i = kids;

	EnterCriticalSection(&cleanup_lock);
	cleaning_up = 1;
	LeaveCriticalSection(&cleanup_lock);

	while (0 < i--) {
		if (NULL == kid_cgi_ps[i]) {
				continue;
		}

		TerminateProcess(kid_cgi_ps[i], 0);
		CloseHandle(kid_cgi_ps[i]);
		kid_cgi_ps[i] = NULL;
	}

	if (job) {
		CloseHandle(job);
	}

	parent = 0;

	return TRUE;
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
static void php_cgi_globals_ctor(php_cgi_globals_struct *php_cgi_globals)
{
#ifdef ZTS
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
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
	zend_hash_init(&php_cgi_globals->user_config_cache, 8, NULL, user_config_cache_entry_dtor, 1);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
static PHP_MINIT_FUNCTION(cgi)
{
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
	if (zend_parse_parameters_none()) {
		return;
	}
	if (fcgi_is_fastcgi()) {
		fcgi_terminate();
	}
}
/* }}} */


PHP_FUNCTION(apache_request_headers) /* {{{ */
{
	if (zend_parse_parameters_none()) {
		return;
	}
	array_init(return_value);
	if (fcgi_is_fastcgi()) {
		fcgi_request *request = (fcgi_request*) SG(server_context);

		fcgi_loadenv(request, sapi_add_request_header, return_value);
	} else {
		char buf[128];
		char **env, *p, *q, *var, *val, *t = buf;
		size_t alloc_size = sizeof(buf);
		zend_ulong var_len;

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
			add_assoc_string_ex(return_value, var, var_len, val);
		}
		if (t != buf && t != NULL) {
			efree(t);
		}
	}
}
/* }}} */

static void add_response_header(sapi_header_struct *h, zval *return_value) /* {{{ */
{
	if (h->header_len > 0) {
		char *s;
		size_t len = 0;
		ALLOCA_FLAG(use_heap)

		char *p = strchr(h->header, ':');
		if (NULL != p) {
			len = p - h->header;
		}
		if (len > 0) {
			while (len != 0 && (h->header[len-1] == ' ' || h->header[len-1] == '\t')) {
				len--;
			}
			if (len) {
				s = do_alloca(len + 1, use_heap);
				memcpy(s, h->header, len);
				s[len] = 0;
				do {
					p++;
				} while (*p == ' ' || *p == '\t');
				add_assoc_stringl_ex(return_value, s, len, p, h->header_len - (p - h->header));
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

	array_init(return_value);
	zend_llist_apply_with_argument(&SG(sapi_headers).headers, (llist_apply_with_arg_func_t)add_response_header, return_value);
}
/* }}} */

ZEND_BEGIN_ARG_INFO(arginfo_no_args, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry cgi_functions[] = {
	PHP_FE(apache_child_terminate, arginfo_no_args)
	PHP_FE(apache_request_headers, arginfo_no_args)
	PHP_FE(apache_response_headers, arginfo_no_args)
	PHP_FALIAS(getallheaders, apache_request_headers, arginfo_no_args)
	PHP_FE_END
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
	int cgi = 0, c, i;
	size_t len;
	zend_file_handle file_handle;
	char *s;

	/* temporary locals */
	int behavior = PHP_MODE_STANDARD;
	int no_headers = 0;
	int orig_optind = php_optind;
	char *orig_optarg = php_optarg;
	char *script_file = NULL;
	size_t ini_entries_len = 0;
	/* end of temporary locals */

	int max_requests = 500;
	int requests = 0;
	int fastcgi;
	char *bindpath = NULL;
	int fcgi_fd = 0;
	fcgi_request *request = NULL;
	int warmup_repeats = 0;
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
	(void)ts_resource(0);
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	zend_signal_startup();

#ifdef ZTS
	ts_allocate_id(&php_cgi_globals_id, sizeof(php_cgi_globals_struct), (ts_allocate_ctor) php_cgi_globals_ctor, NULL);
#else
	php_cgi_globals_ctor(&php_cgi_globals);
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
		for (p = (unsigned char *)decoded_query_string; *p &&  *p <= ' '; p++) {
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
				size_t len = strlen(php_optarg);
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

#ifndef HAVE_ATTRIBUTE_WEAK
	fcgi_set_logger(fcgi_log);
#endif

	if (bindpath) {
		int backlog = 128;
		if (getenv("PHP_FCGI_BACKLOG")) {
			backlog = atoi(getenv("PHP_FCGI_BACKLOG"));
		}
		fcgi_fd = fcgi_listen(bindpath, backlog);
		if (fcgi_fd < 0) {
			fprintf(stderr, "Couldn't create FastCGI listen socket on port %s\n", bindpath);
#ifdef ZTS
			tsrm_shutdown();
#endif
			return FAILURE;
		}
		fastcgi = fcgi_is_fastcgi();
	}

	/* make php call us to get _ENV vars */
	php_php_import_environment_variables = php_import_environment_variables;
	php_import_environment_variables = cgi_php_import_environment_variables;

	if (fastcgi) {
		/* How many times to run PHP scripts before dying */
		if (getenv("PHP_FCGI_MAX_REQUESTS")) {
			max_requests = atoi(getenv("PHP_FCGI_MAX_REQUESTS"));
			if (max_requests < 0) {
				fprintf(stderr, "PHP_FCGI_MAX_REQUESTS is not valid\n");
				return FAILURE;
			}
		}

		/* library is already initialized, now init our request */
		request = fcgi_init_request(fcgi_fd, NULL, NULL, NULL);

		/* Pre-fork or spawn, if required */
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
#ifdef PHP_WIN32
			/* If this env var is set, the process was invoked as a child. Let
				it show the original PHP_FCGI_CHILDREN value, while don't care
				otherwise. */
			char * children_str = getenv("PHP_FCGI_CHILDREN_FOR_KID");
			if (children_str) {
				char putenv_buf[sizeof("PHP_FCGI_CHILDREN")+5];

				snprintf(putenv_buf, sizeof(putenv_buf), "%s=%s", "PHP_FCGI_CHILDREN", children_str);
				putenv(putenv_buf);
				putenv("PHP_FCGI_CHILDREN_FOR_KID=");

				SetEnvironmentVariable("PHP_FCGI_CHILDREN", children_str);
				SetEnvironmentVariable("PHP_FCGI_CHILDREN_FOR_KID", NULL);
			}
#endif
			fcgi_set_mgmt_var("FCGI_MAX_CONNS", sizeof("FCGI_MAX_CONNS")-1, "1", sizeof("1")-1);
			fcgi_set_mgmt_var("FCGI_MAX_REQS",  sizeof("FCGI_MAX_REQS")-1,  "1", sizeof("1")-1);
		}

#ifndef PHP_WIN32
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
						zend_signal_init();
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
			zend_signal_init();
		}

#else
		if (children) {
			wchar_t *cmd_line_tmp, cmd_line[PHP_WIN32_IOUTIL_MAXPATHLEN];
			size_t cmd_line_len;
			char kid_buf[16];
			int i;

			ZeroMemory(&kid_cgi_ps, sizeof(kid_cgi_ps));
			kids = children < WIN32_MAX_SPAWN_CHILDREN ? children : WIN32_MAX_SPAWN_CHILDREN;

			InitializeCriticalSection(&cleanup_lock);
			SetConsoleCtrlHandler(fastcgi_cleanup, TRUE);

			/* kids will inherit the env, don't let them spawn */
			SetEnvironmentVariable("PHP_FCGI_CHILDREN", NULL);
			/* instead, set a temporary env var, so then the child can read and
				show the actual setting correctly. */
			snprintf(kid_buf, 16, "%d", children);
			SetEnvironmentVariable("PHP_FCGI_CHILDREN_FOR_KID", kid_buf);

			/* The current command line is used as is. This should normally be no issue,
				even if there were some I/O redirection. If some issues turn out, an
				extra parsing might be needed here. */
			cmd_line_tmp = GetCommandLineW();
			if (!cmd_line_tmp) {
				DWORD err = GetLastError();
				char *err_text = php_win32_error_to_msg(err);

				fprintf(stderr, "unable to get current command line: [0x%08lx]: %s\n", err, err_text);

				goto parent_out;
			}

			cmd_line_len = wcslen(cmd_line_tmp);
			if (cmd_line_len > sizeof(cmd_line) - 1) {
				fprintf(stderr, "command line is too long\n");
				goto parent_out;
			}
			memmove(cmd_line, cmd_line_tmp, (cmd_line_len + 1)*sizeof(wchar_t));

			job = CreateJobObject(NULL, NULL);
			if (!job) {
				DWORD err = GetLastError();
				char *err_text = php_win32_error_to_msg(err);

				fprintf(stderr, "unable to create job object: [0x%08lx]: %s\n", err, err_text);

				goto parent_out;
			}

			job_info.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
			if (!SetInformationJobObject(job, JobObjectExtendedLimitInformation, &job_info, sizeof(job_info))) {
				DWORD err = GetLastError();
				char *err_text = php_win32_error_to_msg(err);

				fprintf(stderr, "unable to configure job object: [0x%08lx]: %s\n", err, err_text);
			}

			while (parent) {
				EnterCriticalSection(&cleanup_lock);
				if (cleaning_up) {
					goto parent_loop_end;
				}
				LeaveCriticalSection(&cleanup_lock);

				i = kids;
				while (0 < i--) {
					DWORD status;

					if (NULL != kid_cgi_ps[i]) {
						if(!GetExitCodeProcess(kid_cgi_ps[i], &status) || status != STILL_ACTIVE) {
							CloseHandle(kid_cgi_ps[i]);
							kid_cgi_ps[i] = NULL;
						}
					}
				}

				i = kids;
				while (0 < i--) {
					PROCESS_INFORMATION pi;
					STARTUPINFOW si;

					if (NULL != kid_cgi_ps[i]) {
						continue;
					}

					ZeroMemory(&si, sizeof(si));
					si.cb = sizeof(si);
					ZeroMemory(&pi, sizeof(pi));

					si.dwFlags = STARTF_USESTDHANDLES;
					si.hStdOutput = INVALID_HANDLE_VALUE;
					si.hStdInput  = (HANDLE)_get_osfhandle(fcgi_fd);
					si.hStdError  = INVALID_HANDLE_VALUE;

					if (CreateProcessW(NULL, cmd_line, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
						kid_cgi_ps[i] = pi.hProcess;
						if (!AssignProcessToJobObject(job, pi.hProcess)) {
							DWORD err = GetLastError();
							char *err_text = php_win32_error_to_msg(err);

							fprintf(stderr, "unable to assign child process to job object: [0x%08lx]: %s\n", err, err_text);
						}
						CloseHandle(pi.hThread);
					} else {
						DWORD err = GetLastError();
						char *err_text = php_win32_error_to_msg(err);

						kid_cgi_ps[i] = NULL;

						fprintf(stderr, "unable to spawn: [0x%08lx]: %s\n", err, err_text);
					}
				}

				WaitForMultipleObjects(kids, kid_cgi_ps, FALSE, INFINITE);
			}

parent_loop_end:
			/* restore my env */
			SetEnvironmentVariable("PHP_FCGI_CHILDREN", kid_buf);

			DeleteCriticalSection(&cleanup_lock);

			goto parent_out;
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
					{
						char *comma = strchr(php_optarg, ',');
						if (comma) {
							warmup_repeats = atoi(php_optarg);
							repeats = atoi(comma + 1);
#ifdef HAVE_VALGRIND
							if (warmup_repeats > 0) {
								CALLGRIND_STOP_INSTRUMENTATION;
							}
#endif
						} else {
							repeats = atoi(php_optarg);
						}
					}
#ifdef HAVE_GETTIMEOFDAY
					gettimeofday(&start, NULL);
#else
					time(&start);
#endif
					break;
				case 'h':
				case '?':
				case PHP_GETOPT_INVALID_ARG:
					if (request) {
						fcgi_destroy_request(request);
					}
					fcgi_shutdown();
					no_headers = 1;
					SG(headers_sent) = 1;
					php_cgi_usage(argv[0]);
					php_output_end_all();
					exit_status = 0;
					if (c == PHP_GETOPT_INVALID_ARG) {
						exit_status = 1;
					}
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
			SG(server_context) = fastcgi ? (void *)request : (void *) 1;
			init_request_info(request);

			if (!cgi && !fastcgi) {
				while ((c = php_getopt(argc, argv, OPTIONS, &php_optarg, &php_optind, 0, 2)) != -1) {
					switch (c) {

						case 'a':	/* interactive mode */
							printf("Interactive mode enabled\n\n");
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
							if (php_request_startup() == FAILURE) {
								SG(server_context) = NULL;
								php_module_shutdown();
								return FAILURE;
							}
							if (no_headers) {
								SG(headers_sent) = 1;
								SG(request_info).no_headers = 1;
							}
							php_print_info(0xFFFFFFFF);
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
							print_modules();
							php_printf("\n[Zend Modules]\n");
							print_extensions();
							php_printf("\n");
							php_output_end_all();
							fcgi_shutdown();
							exit_status = 0;
							goto out;

						case 'q': /* do not generate HTTP headers */
							no_headers = 1;
							break;

						case 'v': /* show php version & quit */
							if (script_file) {
								efree(script_file);
							}
							no_headers = 1;
							if (php_request_startup() == FAILURE) {
								SG(server_context) = NULL;
								php_module_shutdown();
								return FAILURE;
							}
							SG(headers_sent) = 1;
							SG(request_info).no_headers = 1;
#if ZEND_DEBUG
							php_printf("PHP %s (%s) (built: %s %s) (DEBUG)\nCopyright (c) 1997-2018 The PHP Group\n%s", PHP_VERSION, sapi_module.name, __DATE__, __TIME__, get_zend_version());
#else
							php_printf("PHP %s (%s) (built: %s %s)\nCopyright (c) 1997-2018 The PHP Group\n%s", PHP_VERSION, sapi_module.name, __DATE__, __TIME__, get_zend_version());
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
					if (SG(request_info).path_translated) efree(SG(request_info).path_translated);
					SG(request_info).path_translated = script_file;
					/* before registering argv to module exchange the *new* argv[0] */
					/* we can achieve this without allocating more memory */
					SG(request_info).argc = argc - (php_optind - 1);
					SG(request_info).argv = &argv[php_optind - 1];
					SG(request_info).argv[0] = script_file;
				} else if (argc > php_optind) {
					/* file is on command line, but not in -f opt */
					if (SG(request_info).path_translated) efree(SG(request_info).path_translated);
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
				 * into a single string, separating args with a &
				 * this allows command lines like:
				 *
				 *  test.php v1=test v2=hello+world!
				 *  test.php "v1=test&v2=hello world!"
				 *  test.php v1=test "v2=hello world!"
				*/
				if (!SG(request_info).query_string && argc > php_optind) {
					size_t slen = strlen(PG(arg_separator).input);
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
				file_handle.filename = "Standard input code";
				file_handle.type = ZEND_HANDLE_FP;
				file_handle.handle.fp = stdin;
			}

			file_handle.opened_path = NULL;
			file_handle.free_filename = 0;

			/* request startup only after we've done all we can to
			 * get path_translated */
			if (php_request_startup() == FAILURE) {
				if (fastcgi) {
					fcgi_finish_request(request, 1);
				}
				SG(server_context) = NULL;
				php_module_shutdown();
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
				if (php_fopen_primary_script(&file_handle) == FAILURE) {
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

					if (SG(request_info).path_translated) {
						efree(SG(request_info).path_translated);
						SG(request_info).path_translated = NULL;
					}

					if (free_query_string && SG(request_info).query_string) {
						free(SG(request_info).query_string);
						SG(request_info).query_string = NULL;
					}

					php_request_shutdown((void *) 0);
					SG(server_context) = NULL;
					php_module_shutdown();
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
									zend_long pos = zend_ftell(file_handle.handle.fp);
									zend_fseek(file_handle.handle.fp, pos - 1, SEEK_SET);
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
									zend_off_t pos = php_stream_tell((php_stream*)file_handle.handle.stream.handle);
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
						    size_t i = 1;

						    c = file_handle.handle.stream.mmap.buf[i++];
							while (c != '\n' && c != '\r' && i < file_handle.handle.stream.mmap.len) {
								c = file_handle.handle.stream.mmap.buf[i++];
							}
							if (c == '\r') {
								if (i < file_handle.handle.stream.mmap.len && file_handle.handle.stream.mmap.buf[i] == '\n') {
									i++;
								}
							}
							if(i > file_handle.handle.stream.mmap.len) {
								i = file_handle.handle.stream.mmap.len;
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
					php_execute_script(&file_handle);
					break;
				case PHP_MODE_LINT:
					PG(during_request_startup) = 0;
					exit_status = php_lint_script(&file_handle);
					if (exit_status == SUCCESS) {
						zend_printf("No syntax errors detected in %s\n", file_handle.filename);
					} else {
						zend_printf("Errors parsing %s\n", file_handle.filename);
					}
					break;
				case PHP_MODE_STRIP:
					if (open_file_for_scanning(&file_handle) == SUCCESS) {
						zend_strip();
						zend_file_handle_dtor(&file_handle);
						php_output_teardown();
					}
					return SUCCESS;
					break;
				case PHP_MODE_HIGHLIGHT:
					{
						zend_syntax_highlighter_ini syntax_highlighter_ini;

						if (open_file_for_scanning(&file_handle) == SUCCESS) {
							php_get_highlight_struct(&syntax_highlighter_ini);
							zend_highlight(&syntax_highlighter_ini);
							if (fastcgi) {
								goto fastcgi_request_done;
							}
							zend_file_handle_dtor(&file_handle);
							php_output_teardown();
						}
						return SUCCESS;
					}
					break;
			}

fastcgi_request_done:
			{
				if (SG(request_info).path_translated) {
					efree(SG(request_info).path_translated);
					SG(request_info).path_translated = NULL;
				}

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
					if (warmup_repeats) {
						warmup_repeats--;
						if (!warmup_repeats) {
#ifdef HAVE_GETTIMEOFDAY
							gettimeofday(&start, NULL);
#else
							time(&start);
#endif
#ifdef HAVE_VALGRIND
							CALLGRIND_START_INSTRUMENTATION;
#endif
						}
						continue;
					} else {
						repeats--;
						if (repeats > 0) {
							script_file = NULL;
							php_optind = orig_optind;
							php_optarg = orig_optarg;
							continue;
						}
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

parent_out:

	SG(server_context) = NULL;
	php_module_shutdown();
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
