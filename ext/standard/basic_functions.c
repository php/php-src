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
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "php_assert.h"
#include "php_crypt.h"
#include "php_streams.h"
#include "php_main.h"
#include "php_globals.h"
#include "php_variables.h"
#include "php_ini.h"
#include "php_image.h"
#include "php_standard.h"
#include "php_math.h"
#include "php_http.h"
#include "php_incomplete_class.h"
#include "php_getopt.h"
#include "php_ext_syslog.h"
#include "ext/standard/info.h"
#include "ext/session/php_session.h"
#include "zend_exceptions.h"
#include "zend_attributes.h"
#include "zend_ini.h"
#include "zend_operators.h"
#include "ext/standard/php_dns.h"
#include "ext/standard/php_uuencode.h"
#include "ext/standard/crc32_x86.h"

#ifdef PHP_WIN32
#include "win32/php_win32_globals.h"
#include "win32/time.h"
#include "win32/ioutil.h"
#endif

typedef struct yy_buffer_state *YY_BUFFER_STATE;

#include "zend.h"
#include "zend_ini_scanner.h"
#include "zend_language_scanner.h"
#include <zend_language_parser.h>

#include "zend_portability.h"

#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>

#ifndef PHP_WIN32
#include <sys/types.h>
#include <sys/stat.h>
#endif

#ifndef PHP_WIN32
# include <netdb.h>
#else
#include "win32/inet.h"
#endif

#ifdef HAVE_ARPA_INET_H
# include <arpa/inet.h>
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include <string.h>
#include <locale.h>
#ifdef HAVE_LANGINFO_H
# include <langinfo.h>
#endif

#ifdef HAVE_SYS_MMAN_H
# include <sys/mman.h>
#endif

#ifdef HAVE_SYS_LOADAVG_H
# include <sys/loadavg.h>
#endif

#ifdef PHP_WIN32
# include "win32/unistd.h"
#endif

#ifndef INADDR_NONE
# define INADDR_NONE ((zend_ulong) -1)
#endif

#include "zend_globals.h"
#include "php_globals.h"
#include "SAPI.h"
#include "php_ticks.h"

#ifdef ZTS
PHPAPI int basic_globals_id;
#else
PHPAPI php_basic_globals basic_globals;
#endif

#include "php_fopen_wrappers.h"
#include "streamsfuncs.h"
#include "zend_frameless_function.h"
#include "basic_functions_arginfo.h"

typedef struct _user_tick_function_entry {
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;
	bool calling;
} user_tick_function_entry;

#if HAVE_PUTENV
typedef struct {
	char *putenv_string;
	char *previous_value;
	zend_string *key;
} putenv_entry;
#endif

/* some prototypes for local functions */
static void user_shutdown_function_dtor(zval *zv);
static void user_tick_function_dtor(user_tick_function_entry *tick_function_entry);

static const zend_module_dep standard_deps[] = { /* {{{ */
	ZEND_MOD_OPTIONAL("session")
	ZEND_MOD_END
};
/* }}} */

zend_module_entry basic_functions_module = { /* {{{ */
	STANDARD_MODULE_HEADER_EX,
	NULL,
	standard_deps,
	"standard",					/* extension name */
	ext_functions,				/* function list */
	PHP_MINIT(basic),			/* process startup */
	PHP_MSHUTDOWN(basic),		/* process shutdown */
	PHP_RINIT(basic),			/* request startup */
	PHP_RSHUTDOWN(basic),		/* request shutdown */
	PHP_MINFO(basic),			/* extension info */
	PHP_STANDARD_VERSION,		/* extension version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef HAVE_PUTENV
static void php_putenv_destructor(zval *zv) /* {{{ */
{
	putenv_entry *pe = Z_PTR_P(zv);

	if (pe->previous_value) {
# ifdef PHP_WIN32
		/* MSVCRT has a bug in putenv() when setting a variable that
		 * is already set; if the SetEnvironmentVariable() API call
		 * fails, the Crt will double free() a string.
		 * We try to avoid this by setting our own value first */
		SetEnvironmentVariable(ZSTR_VAL(pe->key), "bugbug");
# endif
		putenv(pe->previous_value);
# ifdef PHP_WIN32
		efree(pe->previous_value);
# endif
	} else {
# ifdef HAVE_UNSETENV
		unsetenv(ZSTR_VAL(pe->key));
# elif defined(PHP_WIN32)
		SetEnvironmentVariable(ZSTR_VAL(pe->key), NULL);
# ifndef ZTS
		_putenv_s(ZSTR_VAL(pe->key), "");
# endif
# else
		char **env;

		for (env = environ; env != NULL && *env != NULL; env++) {
			if (!strncmp(*env, ZSTR_VAL(pe->key), ZSTR_LEN(pe->key))
					&& (*env)[ZSTR_LEN(pe->key)] == '=') {	/* found it */
				*env = "";
				break;
			}
		}
# endif
	}
#ifdef HAVE_TZSET
	/* don't forget to reset the various libc globals that
	 * we might have changed by an earlier call to tzset(). */
	if (zend_string_equals_literal_ci(pe->key, "TZ")) {
		tzset();
	}
#endif

	free(pe->putenv_string);
	zend_string_release(pe->key);
	efree(pe);
}
/* }}} */
#endif

static void basic_globals_ctor(php_basic_globals *basic_globals_p) /* {{{ */
{
	BG(umask) = -1;
	BG(user_tick_functions) = NULL;
	BG(user_filter_map) = NULL;
	BG(serialize_lock) = 0;

	memset(&BG(serialize), 0, sizeof(BG(serialize)));
	memset(&BG(unserialize), 0, sizeof(BG(unserialize)));

	memset(&BG(url_adapt_session_ex), 0, sizeof(BG(url_adapt_session_ex)));
	memset(&BG(url_adapt_output_ex), 0, sizeof(BG(url_adapt_output_ex)));

	BG(url_adapt_session_ex).type = 1;
	BG(url_adapt_output_ex).type  = 0;

	zend_hash_init(&BG(url_adapt_session_hosts_ht), 0, NULL, NULL, 1);
	zend_hash_init(&BG(url_adapt_output_hosts_ht), 0, NULL, NULL, 1);

#if defined(_REENTRANT)
	memset(&BG(mblen_state), 0, sizeof(BG(mblen_state)));
#endif

	BG(page_uid) = -1;
	BG(page_gid) = -1;

	BG(syslog_device) = NULL;
}
/* }}} */

static void basic_globals_dtor(php_basic_globals *basic_globals_p) /* {{{ */
{
	if (basic_globals_p->url_adapt_session_ex.tags) {
		zend_hash_destroy(basic_globals_p->url_adapt_session_ex.tags);
		free(basic_globals_p->url_adapt_session_ex.tags);
	}
	if (basic_globals_p->url_adapt_output_ex.tags) {
		zend_hash_destroy(basic_globals_p->url_adapt_output_ex.tags);
		free(basic_globals_p->url_adapt_output_ex.tags);
	}

	zend_hash_destroy(&basic_globals_p->url_adapt_session_hosts_ht);
	zend_hash_destroy(&basic_globals_p->url_adapt_output_hosts_ht);
}
/* }}} */

PHPAPI double php_get_nan(void) /* {{{ */
{
	return ZEND_NAN;
}
/* }}} */

PHPAPI double php_get_inf(void) /* {{{ */
{
	return ZEND_INFINITY;
}
/* }}} */

#define BASIC_MINIT_SUBMODULE(module) \
	if (PHP_MINIT(module)(INIT_FUNC_ARGS_PASSTHRU) != SUCCESS) {\
		return FAILURE; \
	}

#define BASIC_RINIT_SUBMODULE(module) \
	PHP_RINIT(module)(INIT_FUNC_ARGS_PASSTHRU);

#define BASIC_MINFO_SUBMODULE(module) \
	PHP_MINFO(module)(ZEND_MODULE_INFO_FUNC_ARGS_PASSTHRU);

#define BASIC_RSHUTDOWN_SUBMODULE(module) \
	PHP_RSHUTDOWN(module)(SHUTDOWN_FUNC_ARGS_PASSTHRU);

#define BASIC_MSHUTDOWN_SUBMODULE(module) \
	PHP_MSHUTDOWN(module)(SHUTDOWN_FUNC_ARGS_PASSTHRU);

PHP_MINIT_FUNCTION(basic) /* {{{ */
{
#ifdef ZTS
	ts_allocate_id(&basic_globals_id, sizeof(php_basic_globals), (ts_allocate_ctor) basic_globals_ctor, (ts_allocate_dtor) basic_globals_dtor);
# ifdef PHP_WIN32
	ts_allocate_id(&php_win32_core_globals_id, sizeof(php_win32_core_globals), (ts_allocate_ctor)php_win32_core_globals_ctor, (ts_allocate_dtor)php_win32_core_globals_dtor );
# endif
#else
	basic_globals_ctor(&basic_globals);
# ifdef PHP_WIN32
	php_win32_core_globals_ctor(&the_php_win32_core_globals);
# endif
#endif

	register_basic_functions_symbols(module_number);

	php_ce_incomplete_class = register_class___PHP_Incomplete_Class();
	php_register_incomplete_class_handlers();

	assertion_error_ce = register_class_AssertionError(zend_ce_error);

	BASIC_MINIT_SUBMODULE(var)
	BASIC_MINIT_SUBMODULE(file)
	BASIC_MINIT_SUBMODULE(pack)
	BASIC_MINIT_SUBMODULE(browscap)
	BASIC_MINIT_SUBMODULE(standard_filters)
	BASIC_MINIT_SUBMODULE(user_filters)
	BASIC_MINIT_SUBMODULE(password)

#ifdef ZTS
	BASIC_MINIT_SUBMODULE(localeconv)
#endif

#ifdef ZEND_INTRIN_SSE4_2_FUNC_PTR
	BASIC_MINIT_SUBMODULE(string_intrin)
#endif

#ifdef ZEND_INTRIN_SSE4_2_PCLMUL_FUNC_PTR
	BASIC_MINIT_SUBMODULE(crc32_x86_intrin)
#endif

#if defined(ZEND_INTRIN_AVX2_FUNC_PTR) || defined(ZEND_INTRIN_SSSE3_FUNC_PTR)
	BASIC_MINIT_SUBMODULE(base64_intrin)
#endif

	BASIC_MINIT_SUBMODULE(crypt)

	BASIC_MINIT_SUBMODULE(dir)
#ifdef HAVE_SYSLOG_H
	BASIC_MINIT_SUBMODULE(syslog)
#endif
	BASIC_MINIT_SUBMODULE(array)
	BASIC_MINIT_SUBMODULE(assert)
	BASIC_MINIT_SUBMODULE(url_scanner_ex)
#ifdef PHP_CAN_SUPPORT_PROC_OPEN
	BASIC_MINIT_SUBMODULE(proc_open)
#endif
	BASIC_MINIT_SUBMODULE(exec)

	BASIC_MINIT_SUBMODULE(user_streams)

	php_register_url_stream_wrapper("php", &php_stream_php_wrapper);
	php_register_url_stream_wrapper("file", &php_plain_files_wrapper);
#ifdef HAVE_GLOB
	php_register_url_stream_wrapper("glob", &php_glob_stream_wrapper);
#endif
	php_register_url_stream_wrapper("data", &php_stream_rfc2397_wrapper);
	php_register_url_stream_wrapper("http", &php_stream_http_wrapper);
	php_register_url_stream_wrapper("ftp", &php_stream_ftp_wrapper);

	return SUCCESS;
}
/* }}} */

PHP_MSHUTDOWN_FUNCTION(basic) /* {{{ */
{
#ifdef ZTS
	ts_free_id(basic_globals_id);
#ifdef PHP_WIN32
	ts_free_id(php_win32_core_globals_id);
#endif
#else
	basic_globals_dtor(&basic_globals);
#ifdef PHP_WIN32
	php_win32_core_globals_dtor(&the_php_win32_core_globals);
#endif
#endif

	php_unregister_url_stream_wrapper("php");
	php_unregister_url_stream_wrapper("http");
	php_unregister_url_stream_wrapper("ftp");

	BASIC_MSHUTDOWN_SUBMODULE(browscap)
	BASIC_MSHUTDOWN_SUBMODULE(array)
	BASIC_MSHUTDOWN_SUBMODULE(assert)
	BASIC_MSHUTDOWN_SUBMODULE(url_scanner_ex)
	BASIC_MSHUTDOWN_SUBMODULE(file)
	BASIC_MSHUTDOWN_SUBMODULE(standard_filters)
#ifdef ZTS
	BASIC_MSHUTDOWN_SUBMODULE(localeconv)
#endif
	BASIC_MSHUTDOWN_SUBMODULE(crypt)
	BASIC_MSHUTDOWN_SUBMODULE(password)

	return SUCCESS;
}
/* }}} */

PHP_RINIT_FUNCTION(basic) /* {{{ */
{
	memset(BG(strtok_table), 0, 256);

	BG(serialize_lock) = 0;
	memset(&BG(serialize), 0, sizeof(BG(serialize)));
	memset(&BG(unserialize), 0, sizeof(BG(unserialize)));

	BG(strtok_string) = NULL;
	BG(strtok_last) = NULL;
	BG(ctype_string) = NULL;
	BG(locale_changed) = 0;
	BG(user_compare_fci) = empty_fcall_info;
	BG(user_compare_fci_cache) = empty_fcall_info_cache;
	BG(page_uid) = -1;
	BG(page_gid) = -1;
	BG(page_inode) = -1;
	BG(page_mtime) = -1;
#ifdef HAVE_PUTENV
	zend_hash_init(&BG(putenv_ht), 1, NULL, php_putenv_destructor, 0);
#endif
	BG(user_shutdown_function_names) = NULL;

	PHP_RINIT(filestat)(INIT_FUNC_ARGS_PASSTHRU);
	BASIC_RINIT_SUBMODULE(dir)
	BASIC_RINIT_SUBMODULE(url_scanner_ex)

	/* Setup default context */
	FG(default_context) = NULL;

	/* Default to global wrappers only */
	FG(stream_wrappers) = NULL;

	/* Default to global filters only */
	FG(stream_filters) = NULL;

	return SUCCESS;
}
/* }}} */

PHP_RSHUTDOWN_FUNCTION(basic) /* {{{ */
{
	if (BG(strtok_string)) {
		zend_string_release(BG(strtok_string));
		BG(strtok_string) = NULL;
	}
#ifdef HAVE_PUTENV
	tsrm_env_lock();
	zend_hash_destroy(&BG(putenv_ht));
	tsrm_env_unlock();
#endif

	if (BG(umask) != -1) {
		umask(BG(umask));
	}

	/* Check if locale was changed and change it back
	 * to the value in startup environment */
	if (BG(locale_changed)) {
		setlocale(LC_ALL, "C");
		zend_reset_lc_ctype_locale();
		zend_update_current_locale();
		if (BG(ctype_string)) {
			zend_string_release_ex(BG(ctype_string), 0);
			BG(ctype_string) = NULL;
		}
	}

	/* FG(stream_wrappers) and FG(stream_filters) are destroyed
	 * during php_request_shutdown() */

	PHP_RSHUTDOWN(filestat)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
#ifdef HAVE_SYSLOG_H
	BASIC_RSHUTDOWN_SUBMODULE(syslog);
#endif
	BASIC_RSHUTDOWN_SUBMODULE(assert)
	BASIC_RSHUTDOWN_SUBMODULE(url_scanner_ex)
	BASIC_RSHUTDOWN_SUBMODULE(streams)
#ifdef PHP_WIN32
	BASIC_RSHUTDOWN_SUBMODULE(win32_core_globals)
#endif

	if (BG(user_tick_functions)) {
		zend_llist_destroy(BG(user_tick_functions));
		efree(BG(user_tick_functions));
		BG(user_tick_functions) = NULL;
	}

	BASIC_RSHUTDOWN_SUBMODULE(user_filters)
	BASIC_RSHUTDOWN_SUBMODULE(browscap)

	BG(page_uid) = -1;
	BG(page_gid) = -1;
	return SUCCESS;
}
/* }}} */

PHP_MINFO_FUNCTION(basic) /* {{{ */
{
	php_info_print_table_start();
	BASIC_MINFO_SUBMODULE(dl)
	BASIC_MINFO_SUBMODULE(mail)
	php_info_print_table_end();
	BASIC_MINFO_SUBMODULE(assert)
}
/* }}} */

/* {{{ Given the name of a constant this function will return the constant's associated value */
PHP_FUNCTION(constant)
{
	zend_string *const_name;
	zval *c;
	zend_class_entry *scope;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(const_name)
	ZEND_PARSE_PARAMETERS_END();

	scope = zend_get_executed_scope();
	c = zend_get_constant_ex(const_name, scope, ZEND_FETCH_CLASS_EXCEPTION);
	if (!c) {
		RETURN_THROWS();
	}

	ZVAL_COPY_OR_DUP(return_value, c);
	if (Z_TYPE_P(return_value) == IS_CONSTANT_AST) {
		if (UNEXPECTED(zval_update_constant_ex(return_value, scope) != SUCCESS)) {
			RETURN_THROWS();
		}
	}
}
/* }}} */

/* {{{ Converts a packed inet address to a human readable IP address string */
PHP_FUNCTION(inet_ntop)
{
	char *address;
	size_t address_len;
	int af = AF_INET;
	char buffer[40];

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(address, address_len)
	ZEND_PARSE_PARAMETERS_END();

#ifdef HAVE_IPV6
	if (address_len == 16) {
		af = AF_INET6;
	} else
#endif
	if (address_len != 4) {
		RETURN_FALSE;
	}

	if (!inet_ntop(af, address, buffer, sizeof(buffer))) {
		RETURN_FALSE;
	}

	RETURN_STRING(buffer);
}
/* }}} */

#ifdef HAVE_INET_PTON
/* {{{ Converts a human readable IP address to a packed binary string */
PHP_FUNCTION(inet_pton)
{
	int ret, af = AF_INET;
	char *address;
	size_t address_len;
	char buffer[17];

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(address, address_len)
	ZEND_PARSE_PARAMETERS_END();

	memset(buffer, 0, sizeof(buffer));

#ifdef HAVE_IPV6
	if (strchr(address, ':')) {
		af = AF_INET6;
	} else
#endif
	if (!strchr(address, '.')) {
		RETURN_FALSE;
	}

	ret = inet_pton(af, address, buffer);

	if (ret <= 0) {
		RETURN_FALSE;
	}

	RETURN_STRINGL(buffer, af == AF_INET ? 4 : 16);
}
/* }}} */
#endif /* HAVE_INET_PTON */

/* {{{ Converts a string containing an (IPv4) Internet Protocol dotted address into a proper address */
PHP_FUNCTION(ip2long)
{
	char *addr;
	size_t addr_len;
#ifdef HAVE_INET_PTON
	struct in_addr ip;
#else
	zend_ulong ip;
#endif

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(addr, addr_len)
	ZEND_PARSE_PARAMETERS_END();

#ifdef HAVE_INET_PTON
	if (addr_len == 0 || inet_pton(AF_INET, addr, &ip) != 1) {
		RETURN_FALSE;
	}
	RETURN_LONG(ntohl(ip.s_addr));
#else
	if (addr_len == 0 || (ip = inet_addr(addr)) == INADDR_NONE) {
		/* The only special case when we should return -1 ourselves,
		 * because inet_addr() considers it wrong. We return 0xFFFFFFFF and
		 * not -1 or ~0 because of 32/64bit issues. */
		if (addr_len == sizeof("255.255.255.255") - 1 &&
			!memcmp(addr, "255.255.255.255", sizeof("255.255.255.255") - 1)
		) {
			RETURN_LONG(0xFFFFFFFF);
		}
		RETURN_FALSE;
	}
	RETURN_LONG(ntohl(ip));
#endif
}
/* }}} */

/* {{{ Converts an (IPv4) Internet network address into a string in Internet standard dotted format */
PHP_FUNCTION(long2ip)
{
	zend_ulong ip;
	zend_long sip;
	struct in_addr myaddr;
	char str[40];

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(sip)
	ZEND_PARSE_PARAMETERS_END();

	/* autoboxes on 32bit platforms, but that's expected */
	ip = (zend_ulong)sip;

	myaddr.s_addr = htonl(ip);
	if (inet_ntop(AF_INET, &myaddr, str, sizeof(str))) {
		RETURN_STRING(str);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/********************
 * System Functions *
 ********************/

PHPAPI zend_string *php_getenv(const char *str, size_t str_len) {
#ifdef PHP_WIN32
	{
		wchar_t *keyw = php_win32_cp_conv_any_to_w(str, str_len, PHP_WIN32_CP_IGNORE_LEN_P);
		if (!keyw) {
			return NULL;
		}

		SetLastError(0);
		/* If the given buffer is not large enough to hold the data, the return value is
		 * the buffer size,  in characters, required to hold the string and its terminating
		 * null character. We use this return value to alloc the final buffer. */
		wchar_t dummybuf;
		DWORD size = GetEnvironmentVariableW(keyw, &dummybuf, 0);
		if (GetLastError() == ERROR_ENVVAR_NOT_FOUND) {
			/* The environment variable doesn't exist. */
			free(keyw);
			return NULL;
		}

		if (size == 0) {
			/* env exists, but it is empty */
			free(keyw);
			return ZSTR_EMPTY_ALLOC();
		}

		wchar_t *valw = emalloc((size + 1) * sizeof(wchar_t));
		size = GetEnvironmentVariableW(keyw, valw, size);
		if (size == 0) {
			/* has been removed between the two calls */
			free(keyw);
			efree(valw);
			return ZSTR_EMPTY_ALLOC();
		} else {
			char *ptr = php_win32_cp_w_to_any(valw);
			zend_string *result = zend_string_init(ptr, strlen(ptr), 0);
			free(ptr);
			free(keyw);
			efree(valw);
			return result;
		}
	}
#else
	tsrm_env_lock();

	/* system method returns a const */
	char *ptr = getenv(str);
	zend_string *result = NULL;
	if (ptr) {
		result = zend_string_init(ptr, strlen(ptr), 0);
	}

	tsrm_env_unlock();
	return result;
#endif
}

/* {{{ Get the value of an environment variable or every available environment variable
   if no varname is present  */
PHP_FUNCTION(getenv)
{
	char *str = NULL;
	size_t str_len;
	bool local_only = 0;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING_OR_NULL(str, str_len)
		Z_PARAM_BOOL(local_only)
	ZEND_PARSE_PARAMETERS_END();

	if (!str) {
		array_init(return_value);
		php_load_environment_variables(return_value);
		return;
	}

	if (!local_only) {
		/* SAPI method returns an emalloc()'d string */
		char *ptr = sapi_getenv(str, str_len);
		if (ptr) {
			// TODO: avoid reallocation ???
			RETVAL_STRING(ptr);
			efree(ptr);
			return;
		}
	}

	zend_string *res = php_getenv(str, str_len);
	if (res) {
		RETURN_STR(res);
	}
	RETURN_FALSE;
}
/* }}} */

#ifdef HAVE_PUTENV
/* {{{ Set the value of an environment variable */
PHP_FUNCTION(putenv)
{
	char *setting;
	size_t setting_len;
	char *p, **env;
	putenv_entry pe;
#ifdef PHP_WIN32
	const char *value = NULL;
	int error_code;
#endif

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(setting, setting_len)
	ZEND_PARSE_PARAMETERS_END();

	if (setting_len == 0 || setting[0] == '=') {
		zend_argument_value_error(1, "must have a valid syntax");
		RETURN_THROWS();
	}

	pe.putenv_string = zend_strndup(setting, setting_len);
	if ((p = strchr(setting, '='))) {
		pe.key = zend_string_init(setting, p - setting, 0);
#ifdef PHP_WIN32
		value = p + 1;
#endif
	} else {
		pe.key = zend_string_init(setting, setting_len, 0);
	}

	tsrm_env_lock();
	zend_hash_del(&BG(putenv_ht), pe.key);

	/* find previous value */
	pe.previous_value = NULL;
	for (env = environ; env != NULL && *env != NULL; env++) {
		if (!strncmp(*env, ZSTR_VAL(pe.key), ZSTR_LEN(pe.key))
				&& (*env)[ZSTR_LEN(pe.key)] == '=') {	/* found it */
#ifdef PHP_WIN32
			/* must copy previous value because MSVCRT's putenv can free the string without notice */
			pe.previous_value = estrdup(*env);
#else
			pe.previous_value = *env;
#endif
			break;
		}
	}

#ifdef HAVE_UNSETENV
	if (!p) { /* no '=' means we want to unset it */
		unsetenv(pe.putenv_string);
	}
	if (!p || putenv(pe.putenv_string) == 0) { /* success */
#else
# ifndef PHP_WIN32
	if (putenv(pe.putenv_string) == 0) { /* success */
# else
		wchar_t *keyw, *valw = NULL;

		keyw = php_win32_cp_any_to_w(ZSTR_VAL(pe.key));
		if (value) {
			valw = php_win32_cp_any_to_w(value);
		}
		/* valw may be NULL, but the failed conversion still needs to be checked. */
		if (!keyw || !valw && value) {
			tsrm_env_unlock();
			free(pe.putenv_string);
			zend_string_release(pe.key);
			free(keyw);
			free(valw);
			RETURN_FALSE;
		}

	error_code = SetEnvironmentVariableW(keyw, valw);

	if (error_code != 0
# ifndef ZTS
	/* We need both SetEnvironmentVariable and _putenv here as some
		dependency lib could use either way to read the environment.
		Obviously the CRT version will be useful more often. But
		generally, doing both brings us on the safe track at least
		in NTS build. */
	&& _wputenv_s(keyw, valw ? valw : L"") == 0
# endif
	) { /* success */
# endif
#endif
		zend_hash_add_mem(&BG(putenv_ht), pe.key, &pe, sizeof(putenv_entry));
#ifdef HAVE_TZSET
		if (zend_string_equals_literal_ci(pe.key, "TZ")) {
			tzset();
		}
#endif
		tsrm_env_unlock();
#ifdef PHP_WIN32
		free(keyw);
		free(valw);
#endif
		RETURN_TRUE;
	} else {
		free(pe.putenv_string);
		zend_string_release(pe.key);
#ifdef PHP_WIN32
		free(keyw);
		free(valw);
#endif
		RETURN_FALSE;
	}
}
/* }}} */
#endif

/* {{{ free_argv()
   Free the memory allocated to an argv array. */
static void free_argv(char **argv, int argc)
{
	int i;

	if (argv) {
		for (i = 0; i < argc; i++) {
			if (argv[i]) {
				efree(argv[i]);
			}
		}
		efree(argv);
	}
}
/* }}} */

/* {{{ free_longopts()
   Free the memory allocated to an longopt array. */
static void free_longopts(opt_struct *longopts)
{
	opt_struct *p;

	if (longopts) {
		for (p = longopts; p && p->opt_char != '-'; p++) {
			if (p->opt_name != NULL) {
				efree((char *)(p->opt_name));
			}
		}
	}
}
/* }}} */

/* {{{ parse_opts()
   Convert the typical getopt input characters to the php_getopt struct array */
static int parse_opts(char * opts, opt_struct ** result)
{
	opt_struct * paras = NULL;
	unsigned int i, count = 0;
	unsigned int opts_len = (unsigned int)strlen(opts);

	for (i = 0; i < opts_len; i++) {
		if ((opts[i] >= 48 && opts[i] <= 57) ||
			(opts[i] >= 65 && opts[i] <= 90) ||
			(opts[i] >= 97 && opts[i] <= 122)
		) {
			count++;
		}
	}

	paras = safe_emalloc(sizeof(opt_struct), count, 0);
	memset(paras, 0, sizeof(opt_struct) * count);
	*result = paras;
	while ( (*opts >= 48 && *opts <= 57) || /* 0 - 9 */
			(*opts >= 65 && *opts <= 90) || /* A - Z */
			(*opts >= 97 && *opts <= 122)   /* a - z */
	) {
		paras->opt_char = *opts;
		paras->need_param = *(++opts) == ':';
		paras->opt_name = NULL;
		if (paras->need_param == 1) {
			opts++;
			if (*opts == ':') {
				paras->need_param++;
				opts++;
			}
		}
		paras++;
	}
	return count;
}
/* }}} */

/* {{{ Get options from the command line argument list */
PHP_FUNCTION(getopt)
{
	char *options = NULL, **argv = NULL;
	char opt[2] = { '\0' };
	char *optname;
	int argc = 0, o;
	size_t options_len = 0, len;
	char *php_optarg = NULL;
	int php_optind = 1;
	zval val, *args = NULL, *p_longopts = NULL;
	zval *zoptind = NULL;
	size_t optname_len = 0;
	opt_struct *opts, *orig_opts;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_STRING(options, options_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY(p_longopts)
		Z_PARAM_ZVAL(zoptind)
	ZEND_PARSE_PARAMETERS_END();

	/* Init zoptind to 1 */
	if (zoptind) {
		ZEND_TRY_ASSIGN_REF_LONG(zoptind, 1);
	}

	/* Get argv from the global symbol table. We calculate argc ourselves
	 * in order to be on the safe side, even though it is also available
	 * from the symbol table. */
	if ((Z_TYPE(PG(http_globals)[TRACK_VARS_SERVER]) == IS_ARRAY || zend_is_auto_global(ZSTR_KNOWN(ZEND_STR_AUTOGLOBAL_SERVER))) &&
		((args = zend_hash_find_ex_ind(Z_ARRVAL_P(&PG(http_globals)[TRACK_VARS_SERVER]), ZSTR_KNOWN(ZEND_STR_ARGV), 1)) != NULL ||
		(args = zend_hash_find_ex_ind(&EG(symbol_table), ZSTR_KNOWN(ZEND_STR_ARGV), 1)) != NULL)
	) {
		int pos = 0;
		zval *entry;

		if (Z_TYPE_P(args) != IS_ARRAY) {
			RETURN_FALSE;
		}
		argc = zend_hash_num_elements(Z_ARRVAL_P(args));

		/* Attempt to allocate enough memory to hold all of the arguments
		 * and a trailing NULL */
		argv = (char **) safe_emalloc(sizeof(char *), (argc + 1), 0);

		/* Iterate over the hash to construct the argv array. */
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(args), entry) {
			zend_string *tmp_arg_str;
			zend_string *arg_str = zval_get_tmp_string(entry, &tmp_arg_str);

			argv[pos++] = estrdup(ZSTR_VAL(arg_str));

			zend_tmp_string_release(tmp_arg_str);
		} ZEND_HASH_FOREACH_END();

		/* The C Standard requires argv[argc] to be NULL - this might
		 * keep some getopt implementations happy. */
		argv[argc] = NULL;
	} else {
		/* Return false if we can't find argv. */
		RETURN_FALSE;
	}

	len = parse_opts(options, &opts);

	if (p_longopts) {
		int count;
		zval *entry;

		count = zend_hash_num_elements(Z_ARRVAL_P(p_longopts));

		/* the first <len> slots are filled by the one short ops
		 * we now extend our array and jump to the new added structs */
		opts = (opt_struct *) safe_erealloc(opts, sizeof(opt_struct), (len + count + 1), 0);
		orig_opts = opts;
		opts += len;

		memset(opts, 0, count * sizeof(opt_struct));

		/* Iterate over the hash to construct the argv array. */
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(p_longopts), entry) {
			zend_string *tmp_arg_str;
			zend_string *arg_str = zval_get_tmp_string(entry, &tmp_arg_str);

			opts->need_param = 0;
			opts->opt_name = estrdup(ZSTR_VAL(arg_str));
			len = strlen(opts->opt_name);
			if ((len > 0) && (opts->opt_name[len - 1] == ':')) {
				opts->need_param++;
				opts->opt_name[len - 1] = '\0';
				if ((len > 1) && (opts->opt_name[len - 2] == ':')) {
					opts->need_param++;
					opts->opt_name[len - 2] = '\0';
				}
			}
			opts->opt_char = 0;
			opts++;

			zend_tmp_string_release(tmp_arg_str);
		} ZEND_HASH_FOREACH_END();
	} else {
		opts = (opt_struct*) erealloc(opts, sizeof(opt_struct) * (len + 1));
		orig_opts = opts;
		opts += len;
	}

	/* php_getopt want to identify the last param */
	opts->opt_char   = '-';
	opts->need_param = 0;
	opts->opt_name   = NULL;

	/* Initialize the return value as an array. */
	array_init(return_value);

	/* after our pointer arithmetic jump back to the first element */
	opts = orig_opts;

	while ((o = php_getopt(argc, argv, opts, &php_optarg, &php_optind, 0, 1)) != -1) {
		/* Skip unknown arguments. */
		if (o == PHP_GETOPT_INVALID_ARG) {
			continue;
		}

		/* Prepare the option character and the argument string. */
		if (o == 0) {
			optname = opts[php_optidx].opt_name;
		} else {
			if (o == 1) {
				o = '-';
			}
			opt[0] = o;
			optname = opt;
		}

		if (php_optarg != NULL) {
			/* keep the arg as binary, since the encoding is not known */
			ZVAL_STRING(&val, php_optarg);
		} else {
			ZVAL_FALSE(&val);
		}

		/* Add this option / argument pair to the result hash. */
		optname_len = strlen(optname);
		if (!(optname_len > 1 && optname[0] == '0') && is_numeric_string(optname, optname_len, NULL, NULL, 0) == IS_LONG) {
			/* numeric string */
			int optname_int = atoi(optname);
			if ((args = zend_hash_index_find(Z_ARRVAL_P(return_value), optname_int)) != NULL) {
				if (Z_TYPE_P(args) != IS_ARRAY) {
					convert_to_array(args);
				}
				zend_hash_next_index_insert(Z_ARRVAL_P(args), &val);
			} else {
				zend_hash_index_update(Z_ARRVAL_P(return_value), optname_int, &val);
			}
		} else {
			/* other strings */
			if ((args = zend_hash_str_find(Z_ARRVAL_P(return_value), optname, strlen(optname))) != NULL) {
				if (Z_TYPE_P(args) != IS_ARRAY) {
					convert_to_array(args);
				}
				zend_hash_next_index_insert(Z_ARRVAL_P(args), &val);
			} else {
				zend_hash_str_add(Z_ARRVAL_P(return_value), optname, strlen(optname), &val);
			}
		}

		php_optarg = NULL;
	}

	/* Set zoptind to php_optind */
	if (zoptind) {
		ZEND_TRY_ASSIGN_REF_LONG(zoptind, php_optind);
	}

	free_longopts(orig_opts);
	efree(orig_opts);
	free_argv(argv, argc);
}
/* }}} */

/* {{{ Flush the output buffer */
PHP_FUNCTION(flush)
{
	ZEND_PARSE_PARAMETERS_NONE();

	sapi_flush();
}
/* }}} */

/* {{{ Delay for a given number of seconds */
PHP_FUNCTION(sleep)
{
	zend_long num;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(num)
	ZEND_PARSE_PARAMETERS_END();

	if (num < 0) {
		zend_argument_value_error(1, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	RETURN_LONG(php_sleep((unsigned int)num));
}
/* }}} */

/* {{{ Delay for a given number of micro seconds */
PHP_FUNCTION(usleep)
{
	zend_long num;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(num)
	ZEND_PARSE_PARAMETERS_END();

	if (num < 0) {
		zend_argument_value_error(1, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

#ifdef HAVE_USLEEP
	usleep((unsigned int)num);
#endif
}
/* }}} */

#ifdef HAVE_NANOSLEEP
/* {{{ Delay for a number of seconds and nano seconds */
PHP_FUNCTION(time_nanosleep)
{
	zend_long tv_sec, tv_nsec;
	struct timespec php_req, php_rem;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(tv_sec)
		Z_PARAM_LONG(tv_nsec)
	ZEND_PARSE_PARAMETERS_END();

	if (tv_sec < 0) {
		zend_argument_value_error(1, "must be greater than or equal to 0");
		RETURN_THROWS();
	}
	if (tv_nsec < 0) {
		zend_argument_value_error(2, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	php_req.tv_sec = (time_t) tv_sec;
	php_req.tv_nsec = (long)tv_nsec;
	if (!nanosleep(&php_req, &php_rem)) {
		RETURN_TRUE;
	} else if (errno == EINTR) {
		array_init(return_value);
		add_assoc_long_ex(return_value, "seconds", sizeof("seconds")-1, php_rem.tv_sec);
		add_assoc_long_ex(return_value, "nanoseconds", sizeof("nanoseconds")-1, php_rem.tv_nsec);
		return;
	} else if (errno == EINVAL) {
		zend_value_error("Nanoseconds was not in the range 0 to 999 999 999 or seconds was negative");
		RETURN_THROWS();
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ Make the script sleep until the specified time */
PHP_FUNCTION(time_sleep_until)
{
	double target_secs;
	struct timeval tm;
	struct timespec php_req, php_rem;
	uint64_t current_ns, target_ns, diff_ns;
	const uint64_t ns_per_sec = 1000000000;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(target_secs)
	ZEND_PARSE_PARAMETERS_END();

	if (gettimeofday((struct timeval *) &tm, NULL) != 0) {
		RETURN_FALSE;
	}

	target_ns = (uint64_t) (target_secs * ns_per_sec);
	current_ns = ((uint64_t) tm.tv_sec) * ns_per_sec + ((uint64_t) tm.tv_usec) * 1000;
	if (target_ns < current_ns) {
		php_error_docref(NULL, E_WARNING, "Argument #1 ($timestamp) must be greater than or equal to the current time");
		RETURN_FALSE;
	}

	diff_ns = target_ns - current_ns;
	php_req.tv_sec = (time_t) (diff_ns / ns_per_sec);
	php_req.tv_nsec = (long) (diff_ns % ns_per_sec);

	while (nanosleep(&php_req, &php_rem)) {
		if (errno == EINTR) {
			php_req.tv_sec = php_rem.tv_sec;
			php_req.tv_nsec = php_rem.tv_nsec;
		} else {
			RETURN_FALSE;
		}
	}

	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ Get the name of the owner of the current PHP script */
PHP_FUNCTION(get_current_user)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_STRING(php_get_current_user());
}
/* }}} */

#define ZVAL_SET_INI_STR(zv, val) do { \
	if (ZSTR_IS_INTERNED(val)) { \
		ZVAL_INTERNED_STR(zv, val); \
	} else if (ZSTR_LEN(val) == 0) { \
		ZVAL_EMPTY_STRING(zv); \
	} else if (ZSTR_LEN(val) == 1) { \
		ZVAL_CHAR(zv, ZSTR_VAL(val)[0]); \
	} else if (!(GC_FLAGS(val) & GC_PERSISTENT)) { \
		ZVAL_NEW_STR(zv, zend_string_copy(val)); \
	} else { \
		ZVAL_NEW_STR(zv, zend_string_init(ZSTR_VAL(val), ZSTR_LEN(val), 0)); \
	} \
} while (0)

static void add_config_entries(HashTable *hash, zval *return_value);

/* {{{ add_config_entry */
static void add_config_entry(zend_ulong h, zend_string *key, zval *entry, zval *retval)
{
	if (Z_TYPE_P(entry) == IS_STRING) {
		zval str_zv;
		ZVAL_SET_INI_STR(&str_zv, Z_STR_P(entry));
		if (key) {
			add_assoc_zval_ex(retval, ZSTR_VAL(key), ZSTR_LEN(key), &str_zv);
		} else {
			add_index_zval(retval, h, &str_zv);
		}
	} else if (Z_TYPE_P(entry) == IS_ARRAY) {
		zval tmp;
		array_init(&tmp);
		add_config_entries(Z_ARRVAL_P(entry), &tmp);
		zend_hash_update(Z_ARRVAL_P(retval), key, &tmp);
	}
}
/* }}} */

/* {{{ add_config_entries */
static void add_config_entries(HashTable *hash, zval *return_value) /* {{{ */
{
	zend_ulong h;
	zend_string *key;
	zval *zv;

	ZEND_HASH_FOREACH_KEY_VAL(hash, h, key, zv)
		add_config_entry(h, key, zv, return_value);
	ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ Get the value of a PHP configuration option */
PHP_FUNCTION(get_cfg_var)
{
	zend_string *varname;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(varname)
	ZEND_PARSE_PARAMETERS_END();

	zval *retval = cfg_get_entry_ex(varname);

	if (retval) {
		if (Z_TYPE_P(retval) == IS_ARRAY) {
			array_init(return_value);
			add_config_entries(Z_ARRVAL_P(retval), return_value);
			return;
		} else {
			ZVAL_SET_INI_STR(return_value, Z_STR_P(retval));
		}
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/*
	1st arg = error message
	2nd arg = error option
	3rd arg = optional parameters (email address or tcp address)
	4th arg = used for additional headers if email

error options:
	0 = send to php_error_log (uses syslog or file depending on ini setting)
	1 = send via email to 3rd parameter 4th option = additional headers
	2 = send via tcp/ip to 3rd parameter (name or ip:port)
	3 = save to file in 3rd parameter
	4 = send to SAPI logger directly
*/

/* {{{ Send an error message somewhere */
PHP_FUNCTION(error_log)
{
	char *message, *opt = NULL, *headers = NULL;
	size_t message_len, opt_len = 0, headers_len = 0;
	zend_long erropt = 0;

	ZEND_PARSE_PARAMETERS_START(1, 4)
		Z_PARAM_STRING(message, message_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(erropt)
		Z_PARAM_PATH_OR_NULL(opt, opt_len)
		Z_PARAM_STRING_OR_NULL(headers, headers_len)
	ZEND_PARSE_PARAMETERS_END();

	if (_php_error_log_ex((int) erropt, message, message_len, opt, headers) == FAILURE) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* For BC (not binary-safe!) */
PHPAPI int _php_error_log(int opt_err, const char *message, const char *opt, const char *headers) /* {{{ */
{
	return _php_error_log_ex(opt_err, message, (opt_err == 3) ? strlen(message) : 0, opt, headers);
}
/* }}} */

PHPAPI int _php_error_log_ex(int opt_err, const char *message, size_t message_len, const char *opt, const char *headers) /* {{{ */
{
	php_stream *stream = NULL;
	size_t nbytes;

	switch (opt_err)
	{
		case 1:		/*send an email */
			if (!php_mail(opt, "PHP error_log message", message, headers, NULL)) {
				return FAILURE;
			}
			break;

		case 2:		/*send to an address */
			zend_value_error("TCP/IP option is not available for error logging");
			return FAILURE;

		case 3:		/*save to a file */
			stream = php_stream_open_wrapper(opt, "a", REPORT_ERRORS, NULL);
			if (!stream) {
				return FAILURE;
			}
			nbytes = php_stream_write(stream, message, message_len);
			php_stream_close(stream);
			if (nbytes != message_len) {
				return FAILURE;
			}
			break;

		case 4: /* send to SAPI */
			if (sapi_module.log_message) {
				sapi_module.log_message(message, -1);
			} else {
				return FAILURE;
			}
			break;

		default:
			php_log_err_with_severity(message, LOG_NOTICE);
			break;
	}
	return SUCCESS;
}
/* }}} */

/* {{{ Get the last occurred error as associative array. Returns NULL if there hasn't been an error yet. */
PHP_FUNCTION(error_get_last)
{
	ZEND_PARSE_PARAMETERS_NONE();

	if (PG(last_error_message)) {
		zval tmp;
		array_init(return_value);

		ZVAL_LONG(&tmp, PG(last_error_type));
		zend_hash_update(Z_ARR_P(return_value), ZSTR_KNOWN(ZEND_STR_TYPE), &tmp);

		ZVAL_STR_COPY(&tmp, PG(last_error_message));
		zend_hash_update(Z_ARR_P(return_value), ZSTR_KNOWN(ZEND_STR_MESSAGE), &tmp);

		ZVAL_STR_COPY(&tmp, PG(last_error_file));
		zend_hash_update(Z_ARR_P(return_value), ZSTR_KNOWN(ZEND_STR_FILE), &tmp);

		ZVAL_LONG(&tmp, PG(last_error_lineno));
		zend_hash_update(Z_ARR_P(return_value), ZSTR_KNOWN(ZEND_STR_LINE), &tmp);
	}
}
/* }}} */

/* {{{ Clear the last occurred error. */
PHP_FUNCTION(error_clear_last)
{
	ZEND_PARSE_PARAMETERS_NONE();

	if (PG(last_error_message)) {
		PG(last_error_type) = 0;
		PG(last_error_lineno) = 0;

		zend_string_release(PG(last_error_message));
		PG(last_error_message) = NULL;

		if (PG(last_error_file)) {
			zend_string_release(PG(last_error_file));
			PG(last_error_file) = NULL;
		}
	}
}
/* }}} */

/* {{{ Call a user function which is the first parameter
   Warning: This function is special-cased by zend_compile.c and so is usually bypassed */
PHP_FUNCTION(call_user_func)
{
	zval retval;
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;

	ZEND_PARSE_PARAMETERS_START(1, -1)
		Z_PARAM_FUNC(fci, fci_cache)
		Z_PARAM_VARIADIC_WITH_NAMED(fci.params, fci.param_count, fci.named_params)
	ZEND_PARSE_PARAMETERS_END();

	fci.retval = &retval;

	if (zend_call_function(&fci, &fci_cache) == SUCCESS && Z_TYPE(retval) != IS_UNDEF) {
		if (Z_ISREF(retval)) {
			zend_unwrap_reference(&retval);
		}
		ZVAL_COPY_VALUE(return_value, &retval);
	}
}
/* }}} */

/* {{{ Call a user function which is the first parameter with the arguments contained in array
   Warning: This function is special-cased by zend_compile.c and so is usually bypassed */
PHP_FUNCTION(call_user_func_array)
{
	zval retval;
	HashTable *params;
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_FUNC(fci, fci_cache)
		Z_PARAM_ARRAY_HT(params)
	ZEND_PARSE_PARAMETERS_END();

	fci.named_params = params;
	fci.retval = &retval;

	if (zend_call_function(&fci, &fci_cache) == SUCCESS && Z_TYPE(retval) != IS_UNDEF) {
		if (Z_ISREF(retval)) {
			zend_unwrap_reference(&retval);
		}
		ZVAL_COPY_VALUE(return_value, &retval);
	}
}
/* }}} */

/* {{{ Call a user function which is the first parameter */
PHP_FUNCTION(forward_static_call)
{
	zval retval;
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;
	zend_class_entry *called_scope;

	ZEND_PARSE_PARAMETERS_START(1, -1)
		Z_PARAM_FUNC(fci, fci_cache)
		Z_PARAM_VARIADIC('*', fci.params, fci.param_count)
	ZEND_PARSE_PARAMETERS_END();

	if (!EX(prev_execute_data) || !EX(prev_execute_data)->func->common.scope) {
		zend_throw_error(NULL, "Cannot call forward_static_call() when no class scope is active");
		RETURN_THROWS();
	}

	fci.retval = &retval;

	called_scope = zend_get_called_scope(execute_data);
	if (called_scope && fci_cache.calling_scope &&
		instanceof_function(called_scope, fci_cache.calling_scope)) {
			fci_cache.called_scope = called_scope;
	}

	if (zend_call_function(&fci, &fci_cache) == SUCCESS && Z_TYPE(retval) != IS_UNDEF) {
		if (Z_ISREF(retval)) {
			zend_unwrap_reference(&retval);
		}
		ZVAL_COPY_VALUE(return_value, &retval);
	}
}
/* }}} */

/* {{{ Call a static method which is the first parameter with the arguments contained in array */
PHP_FUNCTION(forward_static_call_array)
{
	zval retval;
	HashTable *params;
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;
	zend_class_entry *called_scope;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_FUNC(fci, fci_cache)
		Z_PARAM_ARRAY_HT(params)
	ZEND_PARSE_PARAMETERS_END();

	fci.retval = &retval;
	/* Add positional arguments */
	fci.named_params = params;

	called_scope = zend_get_called_scope(execute_data);
	if (called_scope && fci_cache.calling_scope &&
		instanceof_function(called_scope, fci_cache.calling_scope)) {
			fci_cache.called_scope = called_scope;
	}

	if (zend_call_function(&fci, &fci_cache) == SUCCESS && Z_TYPE(retval) != IS_UNDEF) {
		if (Z_ISREF(retval)) {
			zend_unwrap_reference(&retval);
		}
		ZVAL_COPY_VALUE(return_value, &retval);
	}
}
/* }}} */

static void fci_addref(zend_fcall_info *fci, zend_fcall_info_cache *fci_cache)
{
	Z_TRY_ADDREF(fci->function_name);
	if (fci_cache->object) {
		GC_ADDREF(fci_cache->object);
	}
}

static void fci_release(zend_fcall_info *fci, zend_fcall_info_cache *fci_cache)
{
	zval_ptr_dtor(&fci->function_name);
	if (fci_cache->object) {
		zend_object_release(fci_cache->object);
	}
}

void user_shutdown_function_dtor(zval *zv) /* {{{ */
{
	php_shutdown_function_entry *shutdown_function_entry = Z_PTR_P(zv);

	zend_fcall_info_args_clear(&shutdown_function_entry->fci, true);
	fci_release(&shutdown_function_entry->fci, &shutdown_function_entry->fci_cache);
	efree(shutdown_function_entry);
}
/* }}} */

void user_tick_function_dtor(user_tick_function_entry *tick_function_entry) /* {{{ */
{
	zend_fcall_info_args_clear(&tick_function_entry->fci, true);
	fci_release(&tick_function_entry->fci, &tick_function_entry->fci_cache);
}
/* }}} */

static int user_shutdown_function_call(zval *zv) /* {{{ */
{
	php_shutdown_function_entry *shutdown_function_entry = Z_PTR_P(zv);
	zval retval;
	zend_result call_status;

	/* set retval zval for FCI struct */
	shutdown_function_entry->fci.retval = &retval;
	call_status = zend_call_function(&shutdown_function_entry->fci, &shutdown_function_entry->fci_cache);
	ZEND_ASSERT(call_status == SUCCESS);
	zval_ptr_dtor(&retval);

	return 0;
}
/* }}} */

static void user_tick_function_call(user_tick_function_entry *tick_fe) /* {{{ */
{
	/* Prevent re-entrant calls to the same user ticks function */
	if (!tick_fe->calling) {
		zval tmp;

		/* set tmp zval */
		tick_fe->fci.retval = &tmp;

		tick_fe->calling = true;
		zend_call_function(&tick_fe->fci, &tick_fe->fci_cache);

		/* Destroy return value */
		zval_ptr_dtor(&tmp);
		tick_fe->calling = false;
	}
}
/* }}} */

static void run_user_tick_functions(int tick_count, void *arg) /* {{{ */
{
	zend_llist_apply(BG(user_tick_functions), (llist_apply_func_t) user_tick_function_call);
}
/* }}} */

static int user_tick_function_compare(user_tick_function_entry * tick_fe1, user_tick_function_entry * tick_fe2) /* {{{ */
{
	zval *func1 = &tick_fe1->fci.function_name;
	zval *func2 = &tick_fe2->fci.function_name;
	int ret;

	if (Z_TYPE_P(func1) == IS_STRING && Z_TYPE_P(func2) == IS_STRING) {
		ret = zend_binary_zval_strcmp(func1, func2) == 0;
	} else if (Z_TYPE_P(func1) == IS_ARRAY && Z_TYPE_P(func2) == IS_ARRAY) {
		ret = zend_compare_arrays(func1, func2) == 0;
	} else if (Z_TYPE_P(func1) == IS_OBJECT && Z_TYPE_P(func2) == IS_OBJECT) {
		ret = zend_compare_objects(func1, func2) == 0;
	} else {
		ret = 0;
	}

	if (ret && tick_fe1->calling) {
		zend_throw_error(NULL, "Registered tick function cannot be unregistered while it is being executed");
		return 0;
	}
	return ret;
}
/* }}} */

PHPAPI void php_call_shutdown_functions(void) /* {{{ */
{
	if (BG(user_shutdown_function_names)) {
		zend_try {
			zend_hash_apply(BG(user_shutdown_function_names), user_shutdown_function_call);
		} zend_end_try();
	}
}
/* }}} */

PHPAPI void php_free_shutdown_functions(void) /* {{{ */
{
	if (BG(user_shutdown_function_names))
		zend_try {
			zend_hash_destroy(BG(user_shutdown_function_names));
			FREE_HASHTABLE(BG(user_shutdown_function_names));
			BG(user_shutdown_function_names) = NULL;
		} zend_catch {
			/* maybe shutdown method call exit, we just ignore it */
			FREE_HASHTABLE(BG(user_shutdown_function_names));
			BG(user_shutdown_function_names) = NULL;
		} zend_end_try();
}
/* }}} */

/* {{{ Register a user-level function to be called on request termination */
PHP_FUNCTION(register_shutdown_function)
{
	php_shutdown_function_entry entry;
	zval *params = NULL;
	uint32_t param_count = 0;
	bool status;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "f*", &entry.fci, &entry.fci_cache, &params, &param_count) == FAILURE) {
		RETURN_THROWS();
	}

	fci_addref(&entry.fci, &entry.fci_cache);
	zend_fcall_info_argp(&entry.fci, param_count, params);

	status = append_user_shutdown_function(&entry);
	ZEND_ASSERT(status);
}
/* }}} */

PHPAPI bool register_user_shutdown_function(const char *function_name, size_t function_len, php_shutdown_function_entry *shutdown_function_entry) /* {{{ */
{
	if (!BG(user_shutdown_function_names)) {
		ALLOC_HASHTABLE(BG(user_shutdown_function_names));
		zend_hash_init(BG(user_shutdown_function_names), 0, NULL, user_shutdown_function_dtor, 0);
	}

	zend_hash_str_update_mem(BG(user_shutdown_function_names), function_name, function_len, shutdown_function_entry, sizeof(php_shutdown_function_entry));
	return 1;
}
/* }}} */

PHPAPI bool remove_user_shutdown_function(const char *function_name, size_t function_len) /* {{{ */
{
	if (BG(user_shutdown_function_names)) {
		return zend_hash_str_del(BG(user_shutdown_function_names), function_name, function_len) != FAILURE;
	}

	return 0;
}
/* }}} */

PHPAPI bool append_user_shutdown_function(php_shutdown_function_entry *shutdown_function_entry) /* {{{ */
{
	if (!BG(user_shutdown_function_names)) {
		ALLOC_HASHTABLE(BG(user_shutdown_function_names));
		zend_hash_init(BG(user_shutdown_function_names), 0, NULL, user_shutdown_function_dtor, 0);
	}

	return zend_hash_next_index_insert_mem(BG(user_shutdown_function_names), shutdown_function_entry, sizeof(php_shutdown_function_entry)) != NULL;
}
/* }}} */

ZEND_API void php_get_highlight_struct(zend_syntax_highlighter_ini *syntax_highlighter_ini) /* {{{ */
{
	syntax_highlighter_ini->highlight_comment = INI_STR("highlight.comment");
	syntax_highlighter_ini->highlight_default = INI_STR("highlight.default");
	syntax_highlighter_ini->highlight_html    = INI_STR("highlight.html");
	syntax_highlighter_ini->highlight_keyword = INI_STR("highlight.keyword");
	syntax_highlighter_ini->highlight_string  = INI_STR("highlight.string");
}
/* }}} */

/* {{{ Syntax highlight a source file */
PHP_FUNCTION(highlight_file)
{
	char *filename;
	size_t filename_len;
	int ret;
	zend_syntax_highlighter_ini syntax_highlighter_ini;
	bool i = 0;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_PATH(filename, filename_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(i)
	ZEND_PARSE_PARAMETERS_END();

	if (php_check_open_basedir(filename)) {
		RETURN_FALSE;
	}

	if (i) {
		php_output_start_default();
	}

	php_get_highlight_struct(&syntax_highlighter_ini);

	ret = highlight_file(filename, &syntax_highlighter_ini);

	if (ret == FAILURE) {
		if (i) {
			php_output_end();
		}
		RETURN_FALSE;
	}

	if (i) {
		php_output_get_contents(return_value);
		php_output_discard();
		ZEND_ASSERT(Z_TYPE_P(return_value) == IS_STRING);
	} else {
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ Return source with stripped comments and whitespace */
PHP_FUNCTION(php_strip_whitespace)
{
	zend_string *filename;
	zend_lex_state original_lex_state;
	zend_file_handle file_handle;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_PATH_STR(filename)
	ZEND_PARSE_PARAMETERS_END();

	php_output_start_default();

	zend_stream_init_filename_ex(&file_handle, filename);
	zend_save_lexical_state(&original_lex_state);
	if (open_file_for_scanning(&file_handle) == FAILURE) {
		zend_restore_lexical_state(&original_lex_state);
		php_output_end();
		zend_destroy_file_handle(&file_handle);
		RETURN_EMPTY_STRING();
	}

	zend_strip();

	zend_restore_lexical_state(&original_lex_state);

	php_output_get_contents(return_value);
	php_output_discard();
	zend_destroy_file_handle(&file_handle);
}
/* }}} */

/* {{{ Syntax highlight a string or optionally return it */
PHP_FUNCTION(highlight_string)
{
	zend_string *str;
	zend_syntax_highlighter_ini syntax_highlighter_ini;
	char *hicompiled_string_description;
	bool i = 0;
	int old_error_reporting = EG(error_reporting);

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(str)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(i)
	ZEND_PARSE_PARAMETERS_END();

	if (i) {
		php_output_start_default();
	}

	EG(error_reporting) = E_ERROR;

	php_get_highlight_struct(&syntax_highlighter_ini);

	hicompiled_string_description = zend_make_compiled_string_description("highlighted code");

	highlight_string(str, &syntax_highlighter_ini, hicompiled_string_description);
	efree(hicompiled_string_description);

	EG(error_reporting) = old_error_reporting;

	if (i) {
		php_output_get_contents(return_value);
		php_output_discard();
		ZEND_ASSERT(Z_TYPE_P(return_value) == IS_STRING);
	} else {
		// TODO Make this function void?
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ Get interpreted size from the ini shorthand syntax */
PHP_FUNCTION(ini_parse_quantity)
{
	zend_string *shorthand;
	zend_string *errstr;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(shorthand)
	ZEND_PARSE_PARAMETERS_END();

	RETVAL_LONG(zend_ini_parse_quantity(shorthand, &errstr));

	if (errstr) {
		zend_error(E_WARNING, "%s", ZSTR_VAL(errstr));
		zend_string_release(errstr);
	}
}
/* }}} */

/* {{{ Get a configuration option */
PHP_FUNCTION(ini_get)
{
	zend_string *varname, *val;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(varname)
	ZEND_PARSE_PARAMETERS_END();

	val = zend_ini_get_value(varname);

	if (!val) {
		RETURN_FALSE;
	}

	ZVAL_SET_INI_STR(return_value, val);
}
/* }}} */

/* {{{ Get all configuration options */
PHP_FUNCTION(ini_get_all)
{
	char *extname = NULL;
	size_t extname_len = 0, module_number = 0;
	zend_module_entry *module;
	bool details = 1;
	zend_string *key;
	zend_ini_entry *ini_entry;


	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING_OR_NULL(extname, extname_len)
		Z_PARAM_BOOL(details)
	ZEND_PARSE_PARAMETERS_END();

	zend_ini_sort_entries();

	if (extname) {
		if ((module = zend_hash_str_find_ptr(&module_registry, extname, extname_len)) == NULL) {
			php_error_docref(NULL, E_WARNING, "Extension \"%s\" cannot be found", extname);
			RETURN_FALSE;
		}
		module_number = module->module_number;
	}

	array_init(return_value);
	ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(EG(ini_directives), key, ini_entry) {
		zval option;

		if (module_number != 0 && ini_entry->module_number != module_number) {
			continue;
		}

		if (key == NULL || ZSTR_VAL(key)[0] != 0) {
			if (details) {
				array_init(&option);

				if (ini_entry->orig_value) {
					add_assoc_str(&option, "global_value", zend_string_copy(ini_entry->orig_value));
				} else if (ini_entry->value) {
					add_assoc_str(&option, "global_value", zend_string_copy(ini_entry->value));
				} else {
					add_assoc_null(&option, "global_value");
				}

				if (ini_entry->value) {
					add_assoc_str(&option, "local_value", zend_string_copy(ini_entry->value));
				} else {
					add_assoc_null(&option, "local_value");
				}

				add_assoc_long(&option, "access", ini_entry->modifiable);

				zend_symtable_update(Z_ARRVAL_P(return_value), ini_entry->name, &option);
			} else {
				if (ini_entry->value) {
					zval zv;

					ZVAL_STR_COPY(&zv, ini_entry->value);
					zend_symtable_update(Z_ARRVAL_P(return_value), ini_entry->name, &zv);
				} else {
					zend_symtable_update(Z_ARRVAL_P(return_value), ini_entry->name, &EG(uninitialized_zval));
				}
			}
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

static int php_ini_check_path(char *option_name, size_t option_len, char *new_option_name, size_t new_option_len) /* {{{ */
{
	if (option_len + 1 != new_option_len) {
		return 0;
	}

	return !strncmp(option_name, new_option_name, option_len);
}
/* }}} */

/* {{{ Set a configuration option, returns false on error and the old value of the configuration option on success */
PHP_FUNCTION(ini_set)
{
	zend_string *varname;
	zval *new_value;
	zend_string *val;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR(varname)
		Z_PARAM_ZVAL(new_value)
	ZEND_PARSE_PARAMETERS_END();

	if (Z_TYPE_P(new_value) > IS_STRING) {
		zend_argument_type_error(2, "must be of type string|int|float|bool|null");
		RETURN_THROWS();
	}

	val = zend_ini_get_value(varname);

	if (val) {
		ZVAL_SET_INI_STR(return_value, val);
	} else {
		RETVAL_FALSE;
	}

	zend_string *new_value_tmp_str;
	zend_string *new_value_str = zval_get_tmp_string(new_value, &new_value_tmp_str);

#define _CHECK_PATH(var, var_len, ini) php_ini_check_path(var, var_len, ini, sizeof(ini))
	/* open basedir check */
	if (PG(open_basedir)) {
		if (_CHECK_PATH(ZSTR_VAL(varname), ZSTR_LEN(varname), "error_log") ||
			_CHECK_PATH(ZSTR_VAL(varname), ZSTR_LEN(varname), "java.class.path") ||
			_CHECK_PATH(ZSTR_VAL(varname), ZSTR_LEN(varname), "java.home") ||
			_CHECK_PATH(ZSTR_VAL(varname), ZSTR_LEN(varname), "mail.log") ||
			_CHECK_PATH(ZSTR_VAL(varname), ZSTR_LEN(varname), "java.library.path") ||
			_CHECK_PATH(ZSTR_VAL(varname), ZSTR_LEN(varname), "vpopmail.directory")) {
			if (php_check_open_basedir(ZSTR_VAL(new_value_str))) {
				zval_ptr_dtor_str(return_value);
				zend_tmp_string_release(new_value_tmp_str);
				RETURN_FALSE;
			}
		}
	}
#undef _CHECK_PATH

	if (zend_alter_ini_entry_ex(varname, new_value_str, PHP_INI_USER, PHP_INI_STAGE_RUNTIME, 0) == FAILURE) {
		zval_ptr_dtor_str(return_value);
		RETVAL_FALSE;
	}
	zend_tmp_string_release(new_value_tmp_str);
}
/* }}} */

/* {{{ Restore the value of a configuration option specified by varname */
PHP_FUNCTION(ini_restore)
{
	zend_string *varname;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(varname)
	ZEND_PARSE_PARAMETERS_END();

	zend_restore_ini_entry(varname, PHP_INI_STAGE_RUNTIME);
}
/* }}} */

/* {{{ Sets the include_path configuration option */
PHP_FUNCTION(set_include_path)
{
	zend_string *new_value;
	char *old_value;
	zend_string *key;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_PATH_STR(new_value)
	ZEND_PARSE_PARAMETERS_END();

	old_value = zend_ini_string("include_path", sizeof("include_path") - 1, 0);
	/* copy to return here, because alter might free it! */
	if (old_value) {
		RETVAL_STRING(old_value);
	} else {
		RETVAL_FALSE;
	}

	key = ZSTR_INIT_LITERAL("include_path", 0);
	if (zend_alter_ini_entry_ex(key, new_value, PHP_INI_USER, PHP_INI_STAGE_RUNTIME, 0) == FAILURE) {
		zend_string_release_ex(key, 0);
		zval_ptr_dtor_str(return_value);
		RETURN_FALSE;
	}
	zend_string_release_ex(key, 0);
}
/* }}} */

/* {{{ Get the current include_path configuration option */
PHP_FUNCTION(get_include_path)
{
	char *str;

	ZEND_PARSE_PARAMETERS_NONE();

	str = zend_ini_string("include_path", sizeof("include_path") - 1, 0);

	if (str == NULL) {
		RETURN_FALSE;
	}

	RETURN_STRING(str);
}
/* }}} */

/* {{{ Prints out or returns information about the specified variable */
PHP_FUNCTION(print_r)
{
	zval *var;
	bool do_return = 0;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ZVAL(var)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(do_return)
	ZEND_PARSE_PARAMETERS_END();

	if (do_return) {
		RETURN_STR(zend_print_zval_r_to_str(var, 0));
	} else {
		zend_print_zval_r(var, 0);
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ Returns true if client disconnected */
PHP_FUNCTION(connection_aborted)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_LONG(PG(connection_status) & PHP_CONNECTION_ABORTED);
}
/* }}} */

/* {{{ Returns the connection status bitfield */
PHP_FUNCTION(connection_status)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_LONG(PG(connection_status));
}
/* }}} */

/* {{{ Set whether we want to ignore a user abort event or not */
PHP_FUNCTION(ignore_user_abort)
{
	bool arg = 0;
	bool arg_is_null = 1;
	int old_setting;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL_OR_NULL(arg, arg_is_null)
	ZEND_PARSE_PARAMETERS_END();

	old_setting = (unsigned short)PG(ignore_user_abort);

	if (!arg_is_null) {
		zend_string *key = ZSTR_INIT_LITERAL("ignore_user_abort", 0);
		zend_alter_ini_entry_chars(key, arg ? "1" : "0", 1, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
		zend_string_release_ex(key, 0);
	}

	RETURN_LONG(old_setting);
}
/* }}} */

#ifdef HAVE_GETSERVBYNAME
/* {{{ Returns port associated with service. Protocol must be "tcp" or "udp" */
PHP_FUNCTION(getservbyname)
{
	zend_string *name;
	char *proto;
	size_t proto_len;
	struct servent *serv;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR(name)
		Z_PARAM_STRING(proto, proto_len)
	ZEND_PARSE_PARAMETERS_END();


/* empty string behaves like NULL on windows implementation of
   getservbyname. Let be portable instead. */
#ifdef PHP_WIN32
	if (proto_len == 0) {
		RETURN_FALSE;
	}
#endif

	serv = getservbyname(ZSTR_VAL(name), proto);

#ifdef _AIX
	/*
        On AIX, imap is only known as imap2 in /etc/services, while on Linux imap is an alias for imap2.
        If a request for imap gives no result, we try again with imap2.
        */
	if (serv == NULL && zend_string_equals_literal(name, "imap")) {
		serv = getservbyname("imap2", proto);
	}
#endif
	if (serv == NULL) {
		RETURN_FALSE;
	}

	RETURN_LONG(ntohs(serv->s_port));
}
/* }}} */
#endif

#ifdef HAVE_GETSERVBYPORT
/* {{{ Returns service name associated with port. Protocol must be "tcp" or "udp" */
PHP_FUNCTION(getservbyport)
{
	char *proto;
	size_t proto_len;
	zend_long port;
	struct servent *serv;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(port)
		Z_PARAM_STRING(proto, proto_len)
	ZEND_PARSE_PARAMETERS_END();

	serv = getservbyport(htons((unsigned short) port), proto);

	if (serv == NULL) {
		RETURN_FALSE;
	}

	RETURN_STRING(serv->s_name);
}
/* }}} */
#endif

#ifdef HAVE_GETPROTOBYNAME
/* {{{ Returns protocol number associated with name as per /etc/protocols */
PHP_FUNCTION(getprotobyname)
{
	char *name;
	size_t name_len;
	struct protoent *ent;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(name, name_len)
	ZEND_PARSE_PARAMETERS_END();

	ent = getprotobyname(name);

	if (ent == NULL) {
		RETURN_FALSE;
	}

	RETURN_LONG(ent->p_proto);
}
/* }}} */
#endif

#ifdef HAVE_GETPROTOBYNUMBER
/* {{{ Returns protocol name associated with protocol number proto */
PHP_FUNCTION(getprotobynumber)
{
	zend_long proto;
	struct protoent *ent;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(proto)
	ZEND_PARSE_PARAMETERS_END();

	ent = getprotobynumber((int)proto);

	if (ent == NULL) {
		RETURN_FALSE;
	}

	RETURN_STRING(ent->p_name);
}
/* }}} */
#endif

/* {{{ Registers a tick callback function */
PHP_FUNCTION(register_tick_function)
{
	user_tick_function_entry tick_fe;
	zval *params = NULL;
	uint32_t param_count = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "f*", &tick_fe.fci, &tick_fe.fci_cache, &params, &param_count) == FAILURE) {
		RETURN_THROWS();
	}

	tick_fe.calling = false;
	fci_addref(&tick_fe.fci, &tick_fe.fci_cache);
	zend_fcall_info_argp(&tick_fe.fci, param_count, params);

	if (!BG(user_tick_functions)) {
		BG(user_tick_functions) = (zend_llist *) emalloc(sizeof(zend_llist));
		zend_llist_init(BG(user_tick_functions),
						sizeof(user_tick_function_entry),
						(llist_dtor_func_t) user_tick_function_dtor, 0);
		php_add_tick_function(run_user_tick_functions, NULL);
	}

	zend_llist_add_element(BG(user_tick_functions), &tick_fe);

	RETURN_TRUE;
}
/* }}} */

/* {{{ Unregisters a tick callback function */
PHP_FUNCTION(unregister_tick_function)
{
	user_tick_function_entry tick_fe;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_FUNC(tick_fe.fci, tick_fe.fci_cache)
	ZEND_PARSE_PARAMETERS_END();

	if (!BG(user_tick_functions)) {
		return;
	}

	zend_llist_del_element(BG(user_tick_functions), &tick_fe, (int (*)(void *, void *)) user_tick_function_compare);
}
/* }}} */

/* {{{ Check if file was created by rfc1867 upload */
PHP_FUNCTION(is_uploaded_file)
{
	char *path;
	size_t path_len;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_PATH(path, path_len)
	ZEND_PARSE_PARAMETERS_END();

	if (!SG(rfc1867_uploaded_files)) {
		RETURN_FALSE;
	}

	if (zend_hash_str_exists(SG(rfc1867_uploaded_files), path, path_len)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Move a file if and only if it was created by an upload */
PHP_FUNCTION(move_uploaded_file)
{
	char *path, *new_path;
	size_t path_len, new_path_len;
	bool successful = 0;

#ifndef PHP_WIN32
	int oldmask; int ret;
#endif

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STRING(path, path_len)
		Z_PARAM_PATH(new_path, new_path_len)
	ZEND_PARSE_PARAMETERS_END();

	if (!SG(rfc1867_uploaded_files)) {
		RETURN_FALSE;
	}

	if (!zend_hash_str_exists(SG(rfc1867_uploaded_files), path, path_len)) {
		RETURN_FALSE;
	}

	if (php_check_open_basedir(new_path)) {
		RETURN_FALSE;
	}

	if (VCWD_RENAME(path, new_path) == 0) {
		successful = 1;
#ifndef PHP_WIN32
		oldmask = umask(077);
		umask(oldmask);

		ret = VCWD_CHMOD(new_path, 0666 & ~oldmask);

		if (ret == -1) {
			php_error_docref(NULL, E_WARNING, "%s", strerror(errno));
		}
#endif
	} else if (php_copy_file_ex(path, new_path, STREAM_DISABLE_OPEN_BASEDIR) == SUCCESS) {
		VCWD_UNLINK(path);
		successful = 1;
	}

	if (successful) {
		zend_hash_str_del(SG(rfc1867_uploaded_files), path, path_len);
	} else {
		php_error_docref(NULL, E_WARNING, "Unable to move \"%s\" to \"%s\"", path, new_path);
	}

	RETURN_BOOL(successful);
}
/* }}} */

/* {{{ php_simple_ini_parser_cb */
static void php_simple_ini_parser_cb(zval *arg1, zval *arg2, zval *arg3, int callback_type, zval *arr)
{
	switch (callback_type) {

		case ZEND_INI_PARSER_ENTRY:
			if (!arg2) {
				/* bare string - nothing to do */
				break;
			}
			Z_TRY_ADDREF_P(arg2);
			zend_symtable_update(Z_ARRVAL_P(arr), Z_STR_P(arg1), arg2);
			break;

		case ZEND_INI_PARSER_POP_ENTRY:
		{
			zval hash, *find_hash;

			if (!arg2) {
				/* bare string - nothing to do */
				break;
			}

			/* entry in the form x[a]=b where x might need to be an array index */
			if (!(Z_STRLEN_P(arg1) > 1 && Z_STRVAL_P(arg1)[0] == '0') && is_numeric_string(Z_STRVAL_P(arg1), Z_STRLEN_P(arg1), NULL, NULL, 0) == IS_LONG) {
				zend_ulong key = (zend_ulong) ZEND_STRTOUL(Z_STRVAL_P(arg1), NULL, 0);
				if ((find_hash = zend_hash_index_find(Z_ARRVAL_P(arr), key)) == NULL) {
					array_init(&hash);
					find_hash = zend_hash_index_add_new(Z_ARRVAL_P(arr), key, &hash);
				}
			} else {
				if ((find_hash = zend_hash_find(Z_ARRVAL_P(arr), Z_STR_P(arg1))) == NULL) {
					array_init(&hash);
					find_hash = zend_hash_add_new(Z_ARRVAL_P(arr), Z_STR_P(arg1), &hash);
				}
			}

			if (Z_TYPE_P(find_hash) != IS_ARRAY) {
				zval_ptr_dtor_nogc(find_hash);
				array_init(find_hash);
			}

			if (!arg3 || (Z_TYPE_P(arg3) == IS_STRING && Z_STRLEN_P(arg3) == 0)) {
				Z_TRY_ADDREF_P(arg2);
				add_next_index_zval(find_hash, arg2);
			} else {
				array_set_zval_key(Z_ARRVAL_P(find_hash), arg3, arg2);
			}
		}
		break;

		case ZEND_INI_PARSER_SECTION:
			break;
	}
}
/* }}} */

/* {{{ php_ini_parser_cb_with_sections */
static void php_ini_parser_cb_with_sections(zval *arg1, zval *arg2, zval *arg3, int callback_type, zval *arr)
{
	if (callback_type == ZEND_INI_PARSER_SECTION) {
		array_init(&BG(active_ini_file_section));
		zend_symtable_update(Z_ARRVAL_P(arr), Z_STR_P(arg1), &BG(active_ini_file_section));
	} else if (arg2) {
		zval *active_arr;

		if (Z_TYPE(BG(active_ini_file_section)) != IS_UNDEF) {
			active_arr = &BG(active_ini_file_section);
		} else {
			active_arr = arr;
		}

		php_simple_ini_parser_cb(arg1, arg2, arg3, callback_type, active_arr);
	}
}
/* }}} */

/* {{{ Parse configuration file */
PHP_FUNCTION(parse_ini_file)
{
	zend_string *filename = NULL;
	bool process_sections = 0;
	zend_long scanner_mode = ZEND_INI_SCANNER_NORMAL;
	zend_file_handle fh;
	zend_ini_parser_cb_t ini_parser_cb;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_PATH_STR(filename)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(process_sections)
		Z_PARAM_LONG(scanner_mode)
	ZEND_PARSE_PARAMETERS_END();

	if (ZSTR_LEN(filename) == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	/* Set callback function */
	if (process_sections) {
		ZVAL_UNDEF(&BG(active_ini_file_section));
		ini_parser_cb = (zend_ini_parser_cb_t) php_ini_parser_cb_with_sections;
	} else {
		ini_parser_cb = (zend_ini_parser_cb_t) php_simple_ini_parser_cb;
	}

	/* Setup filehandle */
	zend_stream_init_filename_ex(&fh, filename);

	array_init(return_value);
	if (zend_parse_ini_file(&fh, 0, (int)scanner_mode, ini_parser_cb, return_value) == FAILURE) {
		zend_array_destroy(Z_ARR_P(return_value));
		RETVAL_FALSE;
	}
	zend_destroy_file_handle(&fh);
}
/* }}} */

/* {{{ Parse configuration string */
PHP_FUNCTION(parse_ini_string)
{
	char *string = NULL, *str = NULL;
	size_t str_len = 0;
	bool process_sections = 0;
	zend_long scanner_mode = ZEND_INI_SCANNER_NORMAL;
	zend_ini_parser_cb_t ini_parser_cb;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_STRING(str, str_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(process_sections)
		Z_PARAM_LONG(scanner_mode)
	ZEND_PARSE_PARAMETERS_END();

	if (INT_MAX - str_len < ZEND_MMAP_AHEAD) {
		RETVAL_FALSE;
	}

	/* Set callback function */
	if (process_sections) {
		ZVAL_UNDEF(&BG(active_ini_file_section));
		ini_parser_cb = (zend_ini_parser_cb_t) php_ini_parser_cb_with_sections;
	} else {
		ini_parser_cb = (zend_ini_parser_cb_t) php_simple_ini_parser_cb;
	}

	/* Setup string */
	string = (char *) emalloc(str_len + ZEND_MMAP_AHEAD);
	memcpy(string, str, str_len);
	memset(string + str_len, 0, ZEND_MMAP_AHEAD);

	array_init(return_value);
	if (zend_parse_ini_string(string, 0, (int)scanner_mode, ini_parser_cb, return_value) == FAILURE) {
		zend_array_destroy(Z_ARR_P(return_value));
		RETVAL_FALSE;
	}
	efree(string);
}
/* }}} */

#if ZEND_DEBUG
/* This function returns an array of ALL valid ini options with values and
 *  is not the same as ini_get_all() which returns only registered ini options. Only useful for devs to debug php.ini scanner/parser! */
PHP_FUNCTION(config_get_hash) /* {{{ */
{
	ZEND_PARSE_PARAMETERS_NONE();

	HashTable *hash = php_ini_get_configuration_hash();

	array_init(return_value);
	add_config_entries(hash, return_value);
}
/* }}} */
#endif

#ifdef HAVE_GETLOADAVG
/* {{{ */
PHP_FUNCTION(sys_getloadavg)
{
	double load[3];

	ZEND_PARSE_PARAMETERS_NONE();

	if (getloadavg(load, 3) == -1) {
		RETURN_FALSE;
	} else {
		array_init(return_value);
		add_index_double(return_value, 0, load[0]);
		add_index_double(return_value, 1, load[1]);
		add_index_double(return_value, 2, load[2]);
	}
}
/* }}} */
#endif
