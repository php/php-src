/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/* {{{ includes
 */

#define ZEND_INCLUDE_FULL_WINDOWS_HEADERS

#include "php.h"
#include <stdio.h>
#include <fcntl.h>
#ifdef PHP_WIN32
#include "win32/time.h"
#include "win32/signal.h"
#include "win32/php_win32_globals.h"
#include "win32/winutil.h"
#include <process.h>
#elif defined(NETWARE)
#include <sys/timeval.h>
#ifdef USE_WINSOCK
#include <novsock2.h>
#endif
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
#include "zend.h"
#include "zend_types.h"
#include "zend_extensions.h"
#include "php_ini.h"
#include "php_globals.h"
#include "php_main.h"
#include "fopen_wrappers.h"
#include "ext/standard/php_standard.h"
#include "ext/standard/php_string.h"
#include "ext/date/php_date.h"
#include "php_variables.h"
#include "ext/standard/credits.h"
#ifdef PHP_WIN32
#include <io.h>
#include "win32/php_registry.h"
#include "ext/standard/flock_compat.h"
#endif
#include "php_syslog.h"
#include "Zend/zend_exceptions.h"

#if PHP_SIGCHILD
#include <sys/types.h>
#include <sys/wait.h>
#endif

#include "zend_compile.h"
#include "zend_execute.h"
#include "zend_highlight.h"
#include "zend_indent.h"
#include "zend_extensions.h"
#include "zend_ini.h"
#include "zend_dtrace.h"

#include "php_content_types.h"
#include "php_ticks.h"
#include "php_streams.h"
#include "php_open_temporary_file.h"

#include "SAPI.h"
#include "rfc1867.h"

#if HAVE_MMAP || defined(PHP_WIN32)
# if HAVE_UNISTD_H
#  include <unistd.h>
#  if defined(_SC_PAGESIZE)
#    define REAL_PAGE_SIZE sysconf(_SC_PAGESIZE);
#  elif defined(_SC_PAGE_SIZE)
#    define REAL_PAGE_SIZE sysconf(_SC_PAGE_SIZE);
#  endif
# endif
# if HAVE_SYS_MMAN_H
#  include <sys/mman.h>
# endif
# ifndef REAL_PAGE_SIZE
#  ifdef PAGE_SIZE
#   define REAL_PAGE_SIZE PAGE_SIZE
#  else
#   define REAL_PAGE_SIZE 4096
#  endif
# endif
#endif
/* }}} */

#ifndef S_ISREG
#define S_ISREG(mode) (((mode) & S_IFMT) == S_IFREG)
#endif

PHPAPI int (*php_register_internal_extensions_func)(void) = php_register_internal_extensions;

#ifndef ZTS
php_core_globals core_globals;
#else
PHPAPI int core_globals_id;
#endif

#ifdef PHP_WIN32
#include "win32_internal_function_disabled.h"

static int php_win32_disable_functions(void)
{
	int i;

	if (EG(windows_version_info).dwMajorVersion < 5) {
		for (i = 0; i < function_name_cnt_5; i++) {
			if (zend_hash_str_del(CG(function_table), function_name_5[i], strlen(function_name_5[i]))==FAILURE) {
				php_printf("Unable to disable function '%s'\n", function_name_5[i]);
				return FAILURE;
			}
		}
	}

	if (EG(windows_version_info).dwMajorVersion < 6) {
		for (i = 0; i < function_name_cnt_6; i++) {
			if (zend_hash_str_del(CG(function_table), function_name_6[i], strlen(function_name_6[i]))==FAILURE) {
				php_printf("Unable to disable function '%s'\n", function_name_6[i]);
				return FAILURE;
			}
		}
	}
	return SUCCESS;
}
#endif

#define SAFE_FILENAME(f) ((f)?(f):"-")

/* {{{ PHP_INI_MH
 */
static PHP_INI_MH(OnSetPrecision)
{
	zend_long i;

	ZEND_ATOL(i, new_value->val);
	if (i >= 0) {
		EG(precision) = i;
		return SUCCESS;
	} else {
		return FAILURE;
	}
}
/* }}} */

/* {{{ PHP_INI_MH
 */
static PHP_INI_MH(OnChangeMemoryLimit)
{
	if (new_value) {
		PG(memory_limit) = zend_atol(new_value->val, (int)new_value->len);
	} else {
		PG(memory_limit) = 1<<30;		/* effectively, no limit */
	}
	return zend_set_memory_limit(PG(memory_limit));
}
/* }}} */


/* {{{ php_disable_functions
 */
static void php_disable_functions(void)
{
	char *s = NULL, *e;

	if (!*(INI_STR("disable_functions"))) {
		return;
	}

	e = PG(disable_functions) = strdup(INI_STR("disable_functions"));
	if (e == NULL) {
		return;
	}
	while (*e) {
		switch (*e) {
			case ' ':
			case ',':
				if (s) {
					*e = '\0';
					zend_disable_function(s, e-s);
					s = NULL;
				}
				break;
			default:
				if (!s) {
					s = e;
				}
				break;
		}
		e++;
	}
	if (s) {
		zend_disable_function(s, e-s);
	}
}
/* }}} */

/* {{{ php_disable_classes
 */
static void php_disable_classes(void)
{
	char *s = NULL, *e;

	if (!*(INI_STR("disable_classes"))) {
		return;
	}

	e = PG(disable_classes) = strdup(INI_STR("disable_classes"));

	while (*e) {
		switch (*e) {
			case ' ':
			case ',':
				if (s) {
					*e = '\0';
					zend_disable_class(s, e-s);
					s = NULL;
				}
				break;
			default:
				if (!s) {
					s = e;
				}
				break;
		}
		e++;
	}
	if (s) {
		zend_disable_class(s, e-s);
	}
}
/* }}} */

/* {{{ php_binary_init
 */
static void php_binary_init(void)
{
	char *binary_location;
#ifdef PHP_WIN32
	binary_location = (char *)malloc(MAXPATHLEN);
	if (GetModuleFileName(0, binary_location, MAXPATHLEN) == 0) {
		free(binary_location);
		PG(php_binary) = NULL;
	}
#else
	if (sapi_module.executable_location) {
		binary_location = (char *)malloc(MAXPATHLEN);
		if (!strchr(sapi_module.executable_location, '/')) {
			char *envpath, *path;
			int found = 0;

			if ((envpath = getenv("PATH")) != NULL) {
				char *search_dir, search_path[MAXPATHLEN];
				char *last = NULL;
				zend_stat_t s;

				path = estrdup(envpath);
				search_dir = php_strtok_r(path, ":", &last);

				while (search_dir) {
					snprintf(search_path, MAXPATHLEN, "%s/%s", search_dir, sapi_module.executable_location);
					if (VCWD_REALPATH(search_path, binary_location) && !VCWD_ACCESS(binary_location, X_OK) && VCWD_STAT(binary_location, &s) == 0 && S_ISREG(s.st_mode)) {
						found = 1;
						break;
					}
					search_dir = php_strtok_r(NULL, ":", &last);
				}
				efree(path);
			}
			if (!found) {
				free(binary_location);
				binary_location = NULL;
			}
		} else if (!VCWD_REALPATH(sapi_module.executable_location, binary_location) || VCWD_ACCESS(binary_location, X_OK)) {
			free(binary_location);
			binary_location = NULL;
		}
	} else {
		binary_location = NULL;
	}
#endif
	PG(php_binary) = binary_location;
}
/* }}} */

/* {{{ PHP_INI_MH
 */
static PHP_INI_MH(OnUpdateTimeout)
{
	if (stage==PHP_INI_STAGE_STARTUP) {
		/* Don't set a timeout on startup, only per-request */
		ZEND_ATOL(EG(timeout_seconds), new_value->val);
		return SUCCESS;
	}
	zend_unset_timeout();
	ZEND_ATOL(EG(timeout_seconds), new_value->val);
	zend_set_timeout(EG(timeout_seconds), 0);
	return SUCCESS;
}
/* }}} */

/* {{{ php_get_display_errors_mode() helper function
 */
static int php_get_display_errors_mode(char *value, int value_length)
{
	int mode;

	if (!value) {
		return PHP_DISPLAY_ERRORS_STDOUT;
	}

	if (value_length == 2 && !strcasecmp("on", value)) {
		mode = PHP_DISPLAY_ERRORS_STDOUT;
	} else if (value_length == 3 && !strcasecmp("yes", value)) {
		mode = PHP_DISPLAY_ERRORS_STDOUT;
	} else if (value_length == 4 && !strcasecmp("true", value)) {
		mode = PHP_DISPLAY_ERRORS_STDOUT;
	} else if (value_length == 6 && !strcasecmp(value, "stderr")) {
		mode = PHP_DISPLAY_ERRORS_STDERR;
	} else if (value_length == 6 && !strcasecmp(value, "stdout")) {
		mode = PHP_DISPLAY_ERRORS_STDOUT;
	} else {
		ZEND_ATOL(mode, value);
		if (mode && mode != PHP_DISPLAY_ERRORS_STDOUT && mode != PHP_DISPLAY_ERRORS_STDERR) {
			mode = PHP_DISPLAY_ERRORS_STDOUT;
		}
	}

	return mode;
}
/* }}} */

/* {{{ PHP_INI_MH
 */
static PHP_INI_MH(OnUpdateDisplayErrors)
{
	PG(display_errors) = (zend_bool) php_get_display_errors_mode(new_value->val, (int)new_value->len);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_INI_DISP
 */
static PHP_INI_DISP(display_errors_mode)
{
	int mode, tmp_value_length, cgi_or_cli;
	char *tmp_value;

	if (type == ZEND_INI_DISPLAY_ORIG && ini_entry->modified) {
		tmp_value = (ini_entry->orig_value ? ini_entry->orig_value->val : NULL );
		tmp_value_length = (int)(ini_entry->orig_value? ini_entry->orig_value->len : 0);
	} else if (ini_entry->value) {
		tmp_value = ini_entry->value->val;
		tmp_value_length = (int)ini_entry->value->len;
	} else {
		tmp_value = NULL;
		tmp_value_length = 0;
	}

	mode = php_get_display_errors_mode(tmp_value, tmp_value_length);

	/* Display 'On' for other SAPIs instead of STDOUT or STDERR */
	cgi_or_cli = (!strcmp(sapi_module.name, "cli") || !strcmp(sapi_module.name, "cgi"));

	switch (mode) {
		case PHP_DISPLAY_ERRORS_STDERR:
			if (cgi_or_cli ) {
				PUTS("STDERR");
			} else {
				PUTS("On");
			}
			break;

		case PHP_DISPLAY_ERRORS_STDOUT:
			if (cgi_or_cli ) {
				PUTS("STDOUT");
			} else {
				PUTS("On");
			}
			break;

		default:
			PUTS("Off");
			break;
	}
}
/* }}} */

/* {{{ PHP_INI_MH
 */
static PHP_INI_MH(OnUpdateInternalEncoding)
{
	if (new_value) {
		OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
	}
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_INI_MH
 */
static PHP_INI_MH(OnUpdateInputEncoding)
{
	if (new_value) {
		OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
	}
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_INI_MH
 */
static PHP_INI_MH(OnUpdateOutputEncoding)
{
	if (new_value) {
		OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
	}
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_INI_MH
 */
static PHP_INI_MH(OnUpdateErrorLog)
{
	/* Only do the safemode/open_basedir check at runtime */
	if ((stage == PHP_INI_STAGE_RUNTIME || stage == PHP_INI_STAGE_HTACCESS) && new_value && strcmp(new_value->val, "syslog")) {
		if (PG(open_basedir) && php_check_open_basedir(new_value->val)) {
			return FAILURE;
		}
	}
	OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_INI_MH
 */
static PHP_INI_MH(OnUpdateMailLog)
{
	/* Only do the safemode/open_basedir check at runtime */
	if ((stage == PHP_INI_STAGE_RUNTIME || stage == PHP_INI_STAGE_HTACCESS) && new_value) {
		if (PG(open_basedir) && php_check_open_basedir(new_value->val)) {
			return FAILURE;
		}
	}
	OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_INI_MH
 */
static PHP_INI_MH(OnChangeMailForceExtra)
{
	/* Don't allow changing it in htaccess */
	if (stage == PHP_INI_STAGE_HTACCESS) {
			return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

/* defined in browscap.c */
PHP_INI_MH(OnChangeBrowscap);


/* Need to be read from the environment (?):
 * PHP_AUTO_PREPEND_FILE
 * PHP_AUTO_APPEND_FILE
 * PHP_DOCUMENT_ROOT
 * PHP_USER_DIR
 * PHP_INCLUDE_PATH
 */

 /* Windows and Netware use the internal mail */
#if defined(PHP_WIN32) || defined(NETWARE)
# define DEFAULT_SENDMAIL_PATH NULL
#elif defined(PHP_PROG_SENDMAIL)
# define DEFAULT_SENDMAIL_PATH PHP_PROG_SENDMAIL " -t -i "
#else
# define DEFAULT_SENDMAIL_PATH "/usr/sbin/sendmail -t -i"
#endif

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
	PHP_INI_ENTRY_EX("highlight.comment",		HL_COMMENT_COLOR,	PHP_INI_ALL,	NULL,			php_ini_color_displayer_cb)
	PHP_INI_ENTRY_EX("highlight.default",		HL_DEFAULT_COLOR,	PHP_INI_ALL,	NULL,			php_ini_color_displayer_cb)
	PHP_INI_ENTRY_EX("highlight.html",			HL_HTML_COLOR,		PHP_INI_ALL,	NULL,			php_ini_color_displayer_cb)
	PHP_INI_ENTRY_EX("highlight.keyword",		HL_KEYWORD_COLOR,	PHP_INI_ALL,	NULL,			php_ini_color_displayer_cb)
	PHP_INI_ENTRY_EX("highlight.string",		HL_STRING_COLOR,	PHP_INI_ALL,	NULL,			php_ini_color_displayer_cb)

	STD_PHP_INI_ENTRY_EX("display_errors",		"1",		PHP_INI_ALL,		OnUpdateDisplayErrors,	display_errors,			php_core_globals,	core_globals, display_errors_mode)
	STD_PHP_INI_BOOLEAN("display_startup_errors",	"0",	PHP_INI_ALL,		OnUpdateBool,			display_startup_errors,	php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("enable_dl",			"1",		PHP_INI_SYSTEM,		OnUpdateBool,			enable_dl,				php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("expose_php",			"1",		PHP_INI_SYSTEM,		OnUpdateBool,			expose_php,				php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("docref_root", 			"", 		PHP_INI_ALL,		OnUpdateString,			docref_root,			php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("docref_ext",				"",			PHP_INI_ALL,		OnUpdateString,			docref_ext,				php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("html_errors",			"1",		PHP_INI_ALL,		OnUpdateBool,			html_errors,			php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("xmlrpc_errors",		"0",		PHP_INI_SYSTEM,		OnUpdateBool,			xmlrpc_errors,			php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("xmlrpc_error_number",	"0",		PHP_INI_ALL,		OnUpdateLong,			xmlrpc_error_number,	php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("max_input_time",			"-1",	PHP_INI_SYSTEM|PHP_INI_PERDIR,		OnUpdateLong,			max_input_time,	php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("ignore_user_abort",	"0",		PHP_INI_ALL,		OnUpdateBool,			ignore_user_abort,		php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("implicit_flush",		"0",		PHP_INI_ALL,		OnUpdateBool,			implicit_flush,			php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("log_errors",			"0",		PHP_INI_ALL,		OnUpdateBool,			log_errors,				php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("log_errors_max_len",	 "1024",		PHP_INI_ALL,		OnUpdateLong,			log_errors_max_len,		php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("ignore_repeated_errors",	"0",	PHP_INI_ALL,		OnUpdateBool,			ignore_repeated_errors,	php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("ignore_repeated_source",	"0",	PHP_INI_ALL,		OnUpdateBool,			ignore_repeated_source,	php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("report_memleaks",		"1",		PHP_INI_ALL,		OnUpdateBool,			report_memleaks,		php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("report_zend_debug",	"1",		PHP_INI_ALL,		OnUpdateBool,			report_zend_debug,		php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("output_buffering",		"0",		PHP_INI_PERDIR|PHP_INI_SYSTEM,	OnUpdateLong,	output_buffering,		php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("output_handler",			NULL,		PHP_INI_PERDIR|PHP_INI_SYSTEM,	OnUpdateString,	output_handler,		php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("register_argc_argv",	"1",		PHP_INI_PERDIR|PHP_INI_SYSTEM,	OnUpdateBool,	register_argc_argv,		php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("auto_globals_jit",		"1",		PHP_INI_PERDIR|PHP_INI_SYSTEM,	OnUpdateBool,	auto_globals_jit,	php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("short_open_tag",	DEFAULT_SHORT_OPEN_TAG,	PHP_INI_SYSTEM|PHP_INI_PERDIR,		OnUpdateBool,			short_tags,				zend_compiler_globals,	compiler_globals)
	STD_PHP_INI_BOOLEAN("sql.safe_mode",		"0",		PHP_INI_SYSTEM,		OnUpdateBool,			sql_safe_mode,			php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("track_errors",			"0",		PHP_INI_ALL,		OnUpdateBool,			track_errors,			php_core_globals,	core_globals)

	STD_PHP_INI_ENTRY("unserialize_callback_func",	NULL,	PHP_INI_ALL,		OnUpdateString,			unserialize_callback_func,	php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("serialize_precision",	"17",	PHP_INI_ALL,		OnUpdateLongGEZero,			serialize_precision,	php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("arg_separator.output",	"&",		PHP_INI_ALL,		OnUpdateStringUnempty,	arg_separator.output,	php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("arg_separator.input",	"&",		PHP_INI_SYSTEM|PHP_INI_PERDIR,	OnUpdateStringUnempty,	arg_separator.input,	php_core_globals,	core_globals)

	STD_PHP_INI_ENTRY("auto_append_file",		NULL,		PHP_INI_SYSTEM|PHP_INI_PERDIR,		OnUpdateString,			auto_append_file,		php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("auto_prepend_file",		NULL,		PHP_INI_SYSTEM|PHP_INI_PERDIR,		OnUpdateString,			auto_prepend_file,		php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("doc_root",				NULL,		PHP_INI_SYSTEM,		OnUpdateStringUnempty,	doc_root,				php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("default_charset",		PHP_DEFAULT_CHARSET,	PHP_INI_ALL,	OnUpdateString,			default_charset,		sapi_globals_struct, sapi_globals)
	STD_PHP_INI_ENTRY("default_mimetype",		SAPI_DEFAULT_MIMETYPE,	PHP_INI_ALL,	OnUpdateString,			default_mimetype,		sapi_globals_struct, sapi_globals)
	STD_PHP_INI_ENTRY("internal_encoding",		NULL,			PHP_INI_ALL,	OnUpdateInternalEncoding,	internal_encoding,	php_core_globals, core_globals)
	STD_PHP_INI_ENTRY("input_encoding",			NULL,			PHP_INI_ALL,	OnUpdateInputEncoding,				input_encoding,		php_core_globals, core_globals)
	STD_PHP_INI_ENTRY("output_encoding",		NULL,			PHP_INI_ALL,	OnUpdateOutputEncoding,				output_encoding,	php_core_globals, core_globals)
	STD_PHP_INI_ENTRY("error_log",				NULL,		PHP_INI_ALL,		OnUpdateErrorLog,			error_log,				php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("extension_dir",			PHP_EXTENSION_DIR,		PHP_INI_SYSTEM,		OnUpdateStringUnempty,	extension_dir,			php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("sys_temp_dir",			NULL,		PHP_INI_SYSTEM,		OnUpdateStringUnempty,	sys_temp_dir,			php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("include_path",			PHP_INCLUDE_PATH,		PHP_INI_ALL,		OnUpdateStringUnempty,	include_path,			php_core_globals,	core_globals)
	PHP_INI_ENTRY("max_execution_time",			"30",		PHP_INI_ALL,			OnUpdateTimeout)
	STD_PHP_INI_ENTRY("open_basedir",			NULL,		PHP_INI_ALL,		OnUpdateBaseDir,			open_basedir,			php_core_globals,	core_globals)

	STD_PHP_INI_BOOLEAN("file_uploads",			"1",		PHP_INI_SYSTEM,		OnUpdateBool,			file_uploads,			php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("upload_max_filesize",	"2M",		PHP_INI_SYSTEM|PHP_INI_PERDIR,		OnUpdateLong,			upload_max_filesize,	php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("post_max_size",			"8M",		PHP_INI_SYSTEM|PHP_INI_PERDIR,		OnUpdateLong,			post_max_size,			sapi_globals_struct,sapi_globals)
	STD_PHP_INI_ENTRY("upload_tmp_dir",			NULL,		PHP_INI_SYSTEM,		OnUpdateStringUnempty,	upload_tmp_dir,			php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("max_input_nesting_level", "64",		PHP_INI_SYSTEM|PHP_INI_PERDIR,		OnUpdateLongGEZero,	max_input_nesting_level,			php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("max_input_vars",			"1000",		PHP_INI_SYSTEM|PHP_INI_PERDIR,		OnUpdateLongGEZero,	max_input_vars,						php_core_globals,	core_globals)

	STD_PHP_INI_ENTRY("user_dir",				NULL,		PHP_INI_SYSTEM,		OnUpdateString,			user_dir,				php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("variables_order",		"EGPCS",	PHP_INI_SYSTEM|PHP_INI_PERDIR,		OnUpdateStringUnempty,	variables_order,		php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("request_order",			NULL,		PHP_INI_SYSTEM|PHP_INI_PERDIR,		OnUpdateString,	request_order,		php_core_globals,	core_globals)

	STD_PHP_INI_ENTRY("error_append_string",	NULL,		PHP_INI_ALL,		OnUpdateString,			error_append_string,	php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("error_prepend_string",	NULL,		PHP_INI_ALL,		OnUpdateString,			error_prepend_string,	php_core_globals,	core_globals)

	PHP_INI_ENTRY("SMTP",						"localhost",PHP_INI_ALL,		NULL)
	PHP_INI_ENTRY("smtp_port",					"25",		PHP_INI_ALL,		NULL)
	STD_PHP_INI_BOOLEAN("mail.add_x_header",			"0",		PHP_INI_SYSTEM|PHP_INI_PERDIR,		OnUpdateBool,			mail_x_header,			php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("mail.log",					NULL,		PHP_INI_SYSTEM|PHP_INI_PERDIR,		OnUpdateMailLog,			mail_log,			php_core_globals,	core_globals)
	PHP_INI_ENTRY("browscap",					NULL,		PHP_INI_SYSTEM,		OnChangeBrowscap)
	PHP_INI_ENTRY("memory_limit",				"128M",		PHP_INI_ALL,		OnChangeMemoryLimit)
	PHP_INI_ENTRY("precision",					"14",		PHP_INI_ALL,		OnSetPrecision)
	PHP_INI_ENTRY("sendmail_from",				NULL,		PHP_INI_ALL,		NULL)
	PHP_INI_ENTRY("sendmail_path",	DEFAULT_SENDMAIL_PATH,	PHP_INI_SYSTEM,		NULL)
	PHP_INI_ENTRY("mail.force_extra_parameters",NULL,		PHP_INI_SYSTEM|PHP_INI_PERDIR,		OnChangeMailForceExtra)
	PHP_INI_ENTRY("disable_functions",			"",			PHP_INI_SYSTEM,		NULL)
	PHP_INI_ENTRY("disable_classes",			"",			PHP_INI_SYSTEM,		NULL)
	PHP_INI_ENTRY("max_file_uploads",			"20",			PHP_INI_SYSTEM|PHP_INI_PERDIR,		NULL)

	STD_PHP_INI_BOOLEAN("allow_url_fopen",		"1",		PHP_INI_SYSTEM,		OnUpdateBool,		allow_url_fopen,		php_core_globals,		core_globals)
	STD_PHP_INI_BOOLEAN("allow_url_include",	"0",		PHP_INI_SYSTEM,		OnUpdateBool,		allow_url_include,		php_core_globals,		core_globals)
	STD_PHP_INI_BOOLEAN("enable_post_data_reading",	"1",	PHP_INI_SYSTEM|PHP_INI_PERDIR,	OnUpdateBool,	enable_post_data_reading,	php_core_globals,	core_globals)

	STD_PHP_INI_ENTRY("realpath_cache_size",	"16K",		PHP_INI_SYSTEM,		OnUpdateLong,	realpath_cache_size_limit,	virtual_cwd_globals,	cwd_globals)
	STD_PHP_INI_ENTRY("realpath_cache_ttl",		"120",		PHP_INI_SYSTEM,		OnUpdateLong,	realpath_cache_ttl,			virtual_cwd_globals,	cwd_globals)

	STD_PHP_INI_ENTRY("user_ini.filename",		".user.ini",	PHP_INI_SYSTEM,		OnUpdateString,		user_ini_filename,	php_core_globals,		core_globals)
	STD_PHP_INI_ENTRY("user_ini.cache_ttl",		"300",			PHP_INI_SYSTEM,		OnUpdateLong,		user_ini_cache_ttl,	php_core_globals,		core_globals)
	STD_PHP_INI_BOOLEAN("exit_on_timeout",		"0",		PHP_INI_ALL,		OnUpdateBool,			exit_on_timeout,			php_core_globals,	core_globals)
#ifdef PHP_WIN32
	STD_PHP_INI_BOOLEAN("windows.show_crt_warning",		"0",		PHP_INI_ALL,		OnUpdateBool,			windows_show_crt_warning,			php_core_globals,	core_globals)
#endif
PHP_INI_END()
/* }}} */

/* True globals (no need for thread safety */
/* But don't make them a single int bitfield */
static int module_initialized = 0;
static int module_startup = 1;
static int module_shutdown = 0;

/* {{{ php_during_module_startup */
static int php_during_module_startup(void)
{
	return module_startup;
}
/* }}} */

/* {{{ php_during_module_shutdown */
static int php_during_module_shutdown(void)
{
	return module_shutdown;
}
/* }}} */

/* {{{ php_get_module_initialized
 */
PHPAPI int php_get_module_initialized(void)
{
	return module_initialized;
}
/* }}} */

/* {{{ php_log_err
 */
PHPAPI void php_log_err(char *log_message)
{
	int fd = -1;
	time_t error_time;

	if (PG(in_error_log)) {
		/* prevent recursive invocation */
		return;
	}
	PG(in_error_log) = 1;

	/* Try to use the specified logging location. */
	if (PG(error_log) != NULL) {
#ifdef HAVE_SYSLOG_H
		if (!strcmp(PG(error_log), "syslog")) {
			php_syslog(LOG_NOTICE, "%s", log_message);
			PG(in_error_log) = 0;
			return;
		}
#endif
		fd = VCWD_OPEN_MODE(PG(error_log), O_CREAT | O_APPEND | O_WRONLY, 0644);
		if (fd != -1) {
			char *tmp;
			size_t len;
			zend_string *error_time_str;

			time(&error_time);
#ifdef ZTS
			if (!php_during_module_startup()) {
				error_time_str = php_format_date("d-M-Y H:i:s e", 13, error_time, 1);
			} else {
				error_time_str = php_format_date("d-M-Y H:i:s e", 13, error_time, 0);
			}
#else
			error_time_str = php_format_date("d-M-Y H:i:s e", 13, error_time, 1);
#endif
			len = spprintf(&tmp, 0, "[%s] %s%s", error_time_str->val, log_message, PHP_EOL);
#ifdef PHP_WIN32
			php_flock(fd, 2);
			/* XXX should eventually write in a loop if len > UINT_MAX */
			php_ignore_value(write(fd, tmp, (unsigned)len));
#else
			php_ignore_value(write(fd, tmp, len));
#endif
			efree(tmp);
			zend_string_free(error_time_str);
			close(fd);
			PG(in_error_log) = 0;
			return;
		}
	}

	/* Otherwise fall back to the default logging location, if we have one */

	if (sapi_module.log_message) {
		sapi_module.log_message(log_message);
	}
	PG(in_error_log) = 0;
}
/* }}} */

/* {{{ php_write
   wrapper for modules to use PHPWRITE */
PHPAPI size_t php_write(void *buf, size_t size)
{
	return PHPWRITE(buf, size);
}
/* }}} */

/* {{{ php_printf
 */
PHPAPI size_t php_printf(const char *format, ...)
{
	va_list args;
	size_t ret;
	char *buffer;
	size_t size;

	va_start(args, format);
	size = vspprintf(&buffer, 0, format, args);
	ret = PHPWRITE(buffer, size);
	efree(buffer);
	va_end(args);

	return ret;
}
/* }}} */

/* {{{ php_verror */
/* php_verror is called from php_error_docref<n> functions.
 * Its purpose is to unify error messages and automatically generate clickable
 * html error messages if correcponding ini setting (html_errors) is activated.
 * See: CODING_STANDARDS for details.
 */
PHPAPI void php_verror(const char *docref, const char *params, int type, const char *format, va_list args)
{
	zend_string *replace_buffer = NULL, *replace_origin = NULL;
	char *buffer = NULL, *docref_buf = NULL, *target = NULL;
	char *docref_target = "", *docref_root = "";
	char *p;
	int buffer_len = 0;
	const char *space = "";
	const char *class_name = "";
	const char *function;
	int origin_len;
	char *origin;
	char *message;
	int is_function = 0;

	/* get error text into buffer and escape for html if necessary */
	buffer_len = (int)vspprintf(&buffer, 0, format, args);

	if (PG(html_errors)) {
		replace_buffer = php_escape_html_entities((unsigned char*)buffer, buffer_len, 0, ENT_COMPAT, NULL);
		efree(buffer);
		buffer = replace_buffer->val;
		buffer_len = (int)replace_buffer->len;
	}

	/* which function caused the problem if any at all */
	if (php_during_module_startup()) {
		function = "PHP Startup";
	} else if (php_during_module_shutdown()) {
		function = "PHP Shutdown";
	} else if (EG(current_execute_data) &&
				EG(current_execute_data)->func &&
				ZEND_USER_CODE(EG(current_execute_data)->func->common.type) &&
				EG(current_execute_data)->opline &&
				EG(current_execute_data)->opline->opcode == ZEND_INCLUDE_OR_EVAL
	) {
		switch (EG(current_execute_data)->opline->extended_value) {
			case ZEND_EVAL:
				function = "eval";
				is_function = 1;
				break;
			case ZEND_INCLUDE:
				function = "include";
				is_function = 1;
				break;
			case ZEND_INCLUDE_ONCE:
				function = "include_once";
				is_function = 1;
				break;
			case ZEND_REQUIRE:
				function = "require";
				is_function = 1;
				break;
			case ZEND_REQUIRE_ONCE:
				function = "require_once";
				is_function = 1;
				break;
			default:
				function = "Unknown";
		}
	} else {
		function = get_active_function_name();
		if (!function || !strlen(function)) {
			function = "Unknown";
		} else {
			is_function = 1;
			class_name = get_active_class_name(&space);
		}
	}

	/* if we still have memory then format the origin */
	if (is_function) {
		origin_len = (int)spprintf(&origin, 0, "%s%s%s(%s)", class_name, space, function, params);
	} else {
		origin_len = (int)spprintf(&origin, 0, "%s", function);
	}

	if (PG(html_errors)) {
		replace_origin = php_escape_html_entities((unsigned char*)origin, origin_len, 0, ENT_COMPAT, NULL);
		efree(origin);
		origin = replace_origin->val;
	}

	/* origin and buffer available, so lets come up with the error message */
	if (docref && docref[0] == '#') {
		docref_target = strchr(docref, '#');
		docref = NULL;
	}

	/* no docref given but function is known (the default) */
	if (!docref && is_function) {
		int doclen;
		while (*function == '_') {
			function++;
		}
		if (space[0] == '\0') {
			doclen = (int)spprintf(&docref_buf, 0, "function.%s", function);
		} else {
			doclen = (int)spprintf(&docref_buf, 0, "%s.%s", class_name, function);
		}
		while((p = strchr(docref_buf, '_')) != NULL) {
			*p = '-';
		}
		docref = php_strtolower(docref_buf, doclen);
	}

	/* we have a docref for a function AND
	 * - we show errors in html mode AND
	 * - the user wants to see the links
	 */
	if (docref && is_function && PG(html_errors) && strlen(PG(docref_root))) {
		if (strncmp(docref, "http://", 7)) {
			/* We don't have 'http://' so we use docref_root */

			char *ref;  /* temp copy for duplicated docref */

			docref_root = PG(docref_root);

			ref = estrdup(docref);
			if (docref_buf) {
				efree(docref_buf);
			}
			docref_buf = ref;
			/* strip of the target if any */
			p = strrchr(ref, '#');
			if (p) {
				target = estrdup(p);
				if (target) {
					docref_target = target;
					*p = '\0';
				}
			}
			/* add the extension if it is set in ini */
			if (PG(docref_ext) && strlen(PG(docref_ext))) {
				spprintf(&docref_buf, 0, "%s%s", ref, PG(docref_ext));
				efree(ref);
			}
			docref = docref_buf;
		}
		/* display html formatted or only show the additional links */
		if (PG(html_errors)) {
			spprintf(&message, 0, "%s [<a href='%s%s%s'>%s</a>]: %s", origin, docref_root, docref, docref_target, docref, buffer);
		} else {
			spprintf(&message, 0, "%s [%s%s%s]: %s", origin, docref_root, docref, docref_target, buffer);
		}
		if (target) {
			efree(target);
		}
	} else {
		spprintf(&message, 0, "%s: %s", origin, buffer);
	}
	if (replace_origin) {
		zend_string_free(replace_origin);
	} else {
		efree(origin);
	}
	if (docref_buf) {
		efree(docref_buf);
	}

	if (PG(track_errors) && module_initialized && EG(valid_symbol_table) &&
			(Z_TYPE(EG(user_error_handler)) == IS_UNDEF || !(EG(user_error_handler_error_reporting) & type))) {
		zval tmp;
		ZVAL_STRINGL(&tmp, buffer, buffer_len);
		if (EG(current_execute_data)) {
			if (zend_set_local_var_str("php_errormsg", sizeof("php_errormsg")-1, &tmp, 0) == FAILURE) {
				zval_ptr_dtor(&tmp);
			}
		} else {
			zend_hash_str_update_ind(&EG(symbol_table), "php_errormsg", sizeof("php_errormsg")-1, &tmp);
		}
	}
	if (replace_buffer) {
		zend_string_free(replace_buffer);
	} else {
		efree(buffer);
	}

	php_error(type, "%s", message);
	efree(message);
}
/* }}} */

/* {{{ php_error_docref0 */
/* See: CODING_STANDARDS for details. */
PHPAPI void php_error_docref0(const char *docref, int type, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	php_verror(docref, "", type, format, args);
	va_end(args);
}
/* }}} */

/* {{{ php_error_docref1 */
/* See: CODING_STANDARDS for details. */
PHPAPI void php_error_docref1(const char *docref, const char *param1, int type, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	php_verror(docref, param1, type, format, args);
	va_end(args);
}
/* }}} */

/* {{{ php_error_docref2 */
/* See: CODING_STANDARDS for details. */
PHPAPI void php_error_docref2(const char *docref, const char *param1, const char *param2, int type, const char *format, ...)
{
	char *params;
	va_list args;

	spprintf(&params, 0, "%s,%s", param1, param2);
	va_start(args, format);
	php_verror(docref, params ? params : "...", type, format, args);
	va_end(args);
	if (params) {
		efree(params);
	}
}
/* }}} */

#ifdef PHP_WIN32
#define PHP_WIN32_ERROR_MSG_BUFFER_SIZE 512
PHPAPI void php_win32_docref2_from_error(DWORD error, const char *param1, const char *param2) {
	if (error == 0) {
		php_error_docref2(NULL, param1, param2, E_WARNING, "%s", strerror(errno));
	} else {
		char buf[PHP_WIN32_ERROR_MSG_BUFFER_SIZE + 1];
		int buf_len;

		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error, 0, buf, PHP_WIN32_ERROR_MSG_BUFFER_SIZE, NULL);
		buf_len = (int)strlen(buf);
		if (buf_len >= 2) {
			buf[buf_len - 1] = '\0';
			buf[buf_len - 2] = '\0';
		}
		php_error_docref2(NULL, param1, param2, E_WARNING, "%s (code: %lu)", (char *)buf, error);
	}
}
#undef PHP_WIN32_ERROR_MSG_BUFFER_SIZE
#endif

/* {{{ php_html_puts */
PHPAPI void php_html_puts(const char *str, size_t size)
{
	zend_html_puts(str, size);
}
/* }}} */

/* {{{ php_error_cb
 extended error handling function */
static void php_error_cb(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args)
{
	char *buffer;
	int buffer_len, display;

	buffer_len = (int)vspprintf(&buffer, PG(log_errors_max_len), format, args);

	/* check for repeated errors to be ignored */
	if (PG(ignore_repeated_errors) && PG(last_error_message)) {
		/* no check for PG(last_error_file) is needed since it cannot
		 * be NULL if PG(last_error_message) is not NULL */
		if (strcmp(PG(last_error_message), buffer)
			|| (!PG(ignore_repeated_source)
				&& ((PG(last_error_lineno) != (int)error_lineno)
					|| strcmp(PG(last_error_file), error_filename)))) {
			display = 1;
		} else {
			display = 0;
		}
	} else {
		display = 1;
	}

	/* store the error if it has changed */
	if (display) {
#ifdef ZEND_SIGNALS
		HANDLE_BLOCK_INTERRUPTIONS();
#endif
		if (PG(last_error_message)) {
			free(PG(last_error_message));
			PG(last_error_message) = NULL;
		}
		if (PG(last_error_file)) {
			free(PG(last_error_file));
			PG(last_error_file) = NULL;
		}
#ifdef ZEND_SIGNALS
		HANDLE_UNBLOCK_INTERRUPTIONS();
#endif
		if (!error_filename) {
			error_filename = "Unknown";
		}
		PG(last_error_type) = type;
		PG(last_error_message) = strdup(buffer);
		PG(last_error_file) = strdup(error_filename);
		PG(last_error_lineno) = error_lineno;
	}

	/* according to error handling mode, suppress error, throw exception or show it */
	if (EG(error_handling) != EH_NORMAL) {
		switch (type) {
			case E_ERROR:
			case E_CORE_ERROR:
			case E_COMPILE_ERROR:
			case E_USER_ERROR:
			case E_PARSE:
				/* fatal errors are real errors and cannot be made exceptions */
				break;
			case E_STRICT:
			case E_DEPRECATED:
			case E_USER_DEPRECATED:
				/* for the sake of BC to old damaged code */
				break;
			case E_NOTICE:
			case E_USER_NOTICE:
				/* notices are no errors and are not treated as such like E_WARNINGS */
				break;
			default:
				/* throw an exception if we are in EH_THROW mode
				 * but DO NOT overwrite a pending exception
				 */
				if (EG(error_handling) == EH_THROW && !EG(exception)) {
					zend_throw_error_exception(EG(exception_class), buffer, 0, type);
				}
				efree(buffer);
				return;
		}
	}

	/* display/log the error if necessary */
	if (display && (EG(error_reporting) & type || (type & E_CORE))
		&& (PG(log_errors) || PG(display_errors) || (!module_initialized))) {
		char *error_type_str;

		switch (type) {
			case E_ERROR:
			case E_CORE_ERROR:
			case E_COMPILE_ERROR:
			case E_USER_ERROR:
				error_type_str = "Fatal error";
				break;
			case E_RECOVERABLE_ERROR:
				error_type_str = "Catchable fatal error";
				break;
			case E_WARNING:
			case E_CORE_WARNING:
			case E_COMPILE_WARNING:
			case E_USER_WARNING:
				error_type_str = "Warning";
				break;
			case E_PARSE:
				error_type_str = "Parse error";
				break;
			case E_NOTICE:
			case E_USER_NOTICE:
				error_type_str = "Notice";
				break;
			case E_STRICT:
				error_type_str = "Strict Standards";
				break;
			case E_DEPRECATED:
			case E_USER_DEPRECATED:
				error_type_str = "Deprecated";
				break;
			default:
				error_type_str = "Unknown error";
				break;
		}

		if (!module_initialized || PG(log_errors)) {
			char *log_buffer;
#ifdef PHP_WIN32
			if (type == E_CORE_ERROR || type == E_CORE_WARNING) {
				syslog(LOG_ALERT, "PHP %s: %s (%s)", error_type_str, buffer, GetCommandLine());
			}
#endif
			spprintf(&log_buffer, 0, "PHP %s:  %s in %s on line %d", error_type_str, buffer, error_filename, error_lineno);
			php_log_err(log_buffer);
			efree(log_buffer);
		}

		if (PG(display_errors) && ((module_initialized && !PG(during_request_startup)) || (PG(display_startup_errors)))) {
			if (PG(xmlrpc_errors)) {
				php_printf("<?xml version=\"1.0\"?><methodResponse><fault><value><struct><member><name>faultCode</name><value><int>%pd</int></value></member><member><name>faultString</name><value><string>%s:%s in %s on line %d</string></value></member></struct></value></fault></methodResponse>", PG(xmlrpc_error_number), error_type_str, buffer, error_filename, error_lineno);
			} else {
				char *prepend_string = INI_STR("error_prepend_string");
				char *append_string = INI_STR("error_append_string");

				if (PG(html_errors)) {
					if (type == E_ERROR || type == E_PARSE) {
						zend_string *buf = php_escape_html_entities((unsigned char*)buffer, buffer_len, 0, ENT_COMPAT, NULL);
						php_printf("%s<br />\n<b>%s</b>:  %s in <b>%s</b> on line <b>%d</b><br />\n%s", STR_PRINT(prepend_string), error_type_str, buf->val, error_filename, error_lineno, STR_PRINT(append_string));
						zend_string_free(buf);
					} else {
						php_printf("%s<br />\n<b>%s</b>:  %s in <b>%s</b> on line <b>%d</b><br />\n%s", STR_PRINT(prepend_string), error_type_str, buffer, error_filename, error_lineno, STR_PRINT(append_string));
					}
				} else {
					/* Write CLI/CGI errors to stderr if display_errors = "stderr" */
					if ((!strcmp(sapi_module.name, "cli") || !strcmp(sapi_module.name, "cgi")) &&
						PG(display_errors) == PHP_DISPLAY_ERRORS_STDERR
					) {
#ifdef PHP_WIN32
						fprintf(stderr, "%s: %s in %s on line %u\n", error_type_str, buffer, error_filename, error_lineno);
						fflush(stderr);
#else
						fprintf(stderr, "%s: %s in %s on line %u\n", error_type_str, buffer, error_filename, error_lineno);
#endif
					} else {
						php_printf("%s\n%s: %s in %s on line %d\n%s", STR_PRINT(prepend_string), error_type_str, buffer, error_filename, error_lineno, STR_PRINT(append_string));
					}
				}
			}
		}
#if ZEND_DEBUG
		if (PG(report_zend_debug)) {
			zend_bool trigger_break;

			switch (type) {
				case E_ERROR:
				case E_CORE_ERROR:
				case E_COMPILE_ERROR:
				case E_USER_ERROR:
					trigger_break=1;
					break;
				default:
					trigger_break=0;
					break;
			}
			zend_output_debug_string(trigger_break, "%s(%d) : %s - %s", error_filename, error_lineno, error_type_str, buffer);
		}
#endif
	}

	/* Bail out if we can't recover */
	switch (type) {
		case E_CORE_ERROR:
			if(!module_initialized) {
				/* bad error in module startup - no way we can live with this */
				exit(-2);
			}
		/* no break - intentionally */
		case E_ERROR:
		case E_RECOVERABLE_ERROR:
		case E_PARSE:
		case E_COMPILE_ERROR:
		case E_USER_ERROR:
			EG(exit_status) = 255;
			if (module_initialized) {
				if (!PG(display_errors) &&
				    !SG(headers_sent) &&
					SG(sapi_headers).http_response_code == 200
				) {
					sapi_header_line ctr = {0};

					ctr.line = "HTTP/1.0 500 Internal Server Error";
					ctr.line_len = sizeof("HTTP/1.0 500 Internal Server Error") - 1;
					sapi_header_op(SAPI_HEADER_REPLACE, &ctr);
				}
				/* the parser would return 1 (failure), we can bail out nicely */
				if (type == E_PARSE) {
					CG(parse_error) = 0;
				} else {
					/* restore memory limit */
					zend_set_memory_limit(PG(memory_limit));
					efree(buffer);
					zend_objects_store_mark_destructed(&EG(objects_store));
					zend_bailout();
					return;
				}
			}
			break;
	}

	/* Log if necessary */
	if (!display) {
		efree(buffer);
		return;
	}

	if (PG(track_errors) && module_initialized && EG(valid_symbol_table)) {
		zval tmp;

		ZVAL_STRINGL(&tmp, buffer, buffer_len);
		if (EG(current_execute_data)) {
			if (zend_set_local_var_str("php_errormsg", sizeof("php_errormsg")-1, &tmp, 0) == FAILURE) {
				zval_ptr_dtor(&tmp);
			}
		} else {
			zend_hash_str_update_ind(&EG(symbol_table), "php_errormsg", sizeof("php_errormsg")-1, &tmp);
		}
	}

	efree(buffer);
}
/* }}} */

/* {{{ php_get_current_user
 */
PHPAPI char *php_get_current_user(void)
{
	zend_stat_t *pstat;

	if (SG(request_info).current_user) {
		return SG(request_info).current_user;
	}

	/* FIXME: I need to have this somehow handled if
	USE_SAPI is defined, because cgi will also be
	interfaced in USE_SAPI */

	pstat = sapi_get_stat();

	if (!pstat) {
		return "";
	} else {
#ifdef PHP_WIN32
		char name[256];
		DWORD len = sizeof(name)-1;

		if (!GetUserName(name, &len)) {
			return "";
		}
		name[len] = '\0';
		SG(request_info).current_user_length = len;
		SG(request_info).current_user = estrndup(name, len);
		return SG(request_info).current_user;
#else
		struct passwd *pwd;
#if defined(ZTS) && defined(HAVE_GETPWUID_R) && defined(_SC_GETPW_R_SIZE_MAX)
		struct passwd _pw;
		struct passwd *retpwptr = NULL;
		int pwbuflen = sysconf(_SC_GETPW_R_SIZE_MAX);
		char *pwbuf;

		if (pwbuflen < 1) {
			return "";
		}
		pwbuf = emalloc(pwbuflen);
		if (getpwuid_r(pstat->st_uid, &_pw, pwbuf, pwbuflen, &retpwptr) != 0) {
			efree(pwbuf);
			return "";
		}
		if (retpwptr == NULL) {
			efree(pwbuf);
			return "";
		}
		pwd = &_pw;
#else
		if ((pwd=getpwuid(pstat->st_uid))==NULL) {
			return "";
		}
#endif
		SG(request_info).current_user_length = strlen(pwd->pw_name);
		SG(request_info).current_user = estrndup(pwd->pw_name, SG(request_info).current_user_length);
#if defined(ZTS) && defined(HAVE_GETPWUID_R) && defined(_SC_GETPW_R_SIZE_MAX)
		efree(pwbuf);
#endif
		return SG(request_info).current_user;
#endif
	}
}
/* }}} */

/* {{{ proto bool set_time_limit(int seconds)
   Sets the maximum time a script can run */
PHP_FUNCTION(set_time_limit)
{
	zend_long new_timeout;
	char *new_timeout_str;
	int new_timeout_strlen;
	zend_string *key;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &new_timeout) == FAILURE) {
		return;
	}

	new_timeout_strlen = (int)zend_spprintf(&new_timeout_str, 0, ZEND_LONG_FMT, new_timeout);

	key = zend_string_init("max_execution_time", sizeof("max_execution_time")-1, 0);
	if (zend_alter_ini_entry_chars_ex(key, new_timeout_str, new_timeout_strlen, PHP_INI_USER, PHP_INI_STAGE_RUNTIME, 0) == SUCCESS) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}
	zend_string_release(key);
	efree(new_timeout_str);
}
/* }}} */

/* {{{ php_fopen_wrapper_for_zend
 */
static FILE *php_fopen_wrapper_for_zend(const char *filename, zend_string **opened_path)
{
	return php_stream_open_wrapper_as_file((char *)filename, "rb", USE_PATH|IGNORE_URL_WIN|REPORT_ERRORS|STREAM_OPEN_FOR_INCLUDE, opened_path);
}
/* }}} */

static void php_zend_stream_closer(void *handle) /* {{{ */
{
	php_stream_close((php_stream*)handle);
}
/* }}} */

static void php_zend_stream_mmap_closer(void *handle) /* {{{ */
{
	php_stream_mmap_unmap((php_stream*)handle);
	php_zend_stream_closer(handle);
}
/* }}} */

static size_t php_zend_stream_fsizer(void *handle) /* {{{ */
{
	php_stream_statbuf  ssb;
	if (php_stream_stat((php_stream*)handle, &ssb) == 0) {
		return ssb.sb.st_size;
	}
	return 0;
}
/* }}} */

static int php_stream_open_for_zend(const char *filename, zend_file_handle *handle) /* {{{ */
{
	return php_stream_open_for_zend_ex(filename, handle, USE_PATH|REPORT_ERRORS|STREAM_OPEN_FOR_INCLUDE);
}
/* }}} */

PHPAPI int php_stream_open_for_zend_ex(const char *filename, zend_file_handle *handle, int mode) /* {{{ */
{
	char *p;
	size_t len, mapped_len;
	php_stream *stream = php_stream_open_wrapper((char *)filename, "rb", mode, &handle->opened_path);

	if (stream) {
#if HAVE_MMAP || defined(PHP_WIN32)
		size_t page_size = REAL_PAGE_SIZE;
#endif

		handle->filename = (char*)filename;
		handle->free_filename = 0;
		handle->handle.stream.handle  = stream;
		handle->handle.stream.reader  = (zend_stream_reader_t)_php_stream_read;
		handle->handle.stream.fsizer  = php_zend_stream_fsizer;
		handle->handle.stream.isatty  = 0;
		/* can we mmap immediately? */
		memset(&handle->handle.stream.mmap, 0, sizeof(handle->handle.stream.mmap));
		len = php_zend_stream_fsizer(stream);
		if (len != 0
#if HAVE_MMAP || defined(PHP_WIN32)
		&& ((len - 1) % page_size) <= page_size - ZEND_MMAP_AHEAD
#endif
		&& php_stream_mmap_possible(stream)
		&& (p = php_stream_mmap_range(stream, 0, len, PHP_STREAM_MAP_MODE_SHARED_READONLY, &mapped_len)) != NULL) {
			handle->handle.stream.closer   = php_zend_stream_mmap_closer;
			handle->handle.stream.mmap.buf = p;
			handle->handle.stream.mmap.len = mapped_len;
			handle->type = ZEND_HANDLE_MAPPED;
		} else {
			handle->handle.stream.closer = php_zend_stream_closer;
			handle->type = ZEND_HANDLE_STREAM;
		}
		/* suppress warning if this stream is not explicitly closed */
		php_stream_auto_cleanup(stream);

		return SUCCESS;
	}
	return FAILURE;
}
/* }}} */

static zend_string *php_resolve_path_for_zend(const char *filename, int filename_len) /* {{{ */
{
	return php_resolve_path(filename, filename_len, PG(include_path));
}
/* }}} */

/* {{{ php_get_configuration_directive_for_zend
 */
static zval *php_get_configuration_directive_for_zend(zend_string *name)
{
	return cfg_get_entry_ex(name);
}
/* }}} */

/* {{{ php_message_handler_for_zend
 */
static void php_message_handler_for_zend(zend_long message, const void *data)
{
	switch (message) {
		case ZMSG_FAILED_INCLUDE_FOPEN:
			php_error_docref("function.include", E_WARNING, "Failed opening '%s' for inclusion (include_path='%s')", php_strip_url_passwd((char *) data), STR_PRINT(PG(include_path)));
			break;
		case ZMSG_FAILED_REQUIRE_FOPEN:
			php_error_docref("function.require", E_COMPILE_ERROR, "Failed opening required '%s' (include_path='%s')", php_strip_url_passwd((char *) data), STR_PRINT(PG(include_path)));
			break;
		case ZMSG_FAILED_HIGHLIGHT_FOPEN:
			php_error_docref(NULL, E_WARNING, "Failed opening '%s' for highlighting", php_strip_url_passwd((char *) data));
			break;
		case ZMSG_MEMORY_LEAK_DETECTED:
		case ZMSG_MEMORY_LEAK_REPEATED:
#if ZEND_DEBUG
			if (EG(error_reporting) & E_WARNING) {
				char memory_leak_buf[1024];

				if (message==ZMSG_MEMORY_LEAK_DETECTED) {
					zend_leak_info *t = (zend_leak_info *) data;

					snprintf(memory_leak_buf, 512, "%s(%d) :  Freeing 0x%.8lX (%zu bytes), script=%s\n", t->filename, t->lineno, (zend_uintptr_t)t->addr, t->size, SAFE_FILENAME(SG(request_info).path_translated));
					if (t->orig_filename) {
						char relay_buf[512];

						snprintf(relay_buf, 512, "%s(%d) : Actual location (location was relayed)\n", t->orig_filename, t->orig_lineno);
						strlcat(memory_leak_buf, relay_buf, sizeof(memory_leak_buf));
					}
				} else {
					unsigned long leak_count = (zend_uintptr_t) data;

					snprintf(memory_leak_buf, 512, "Last leak repeated %ld time%s\n", leak_count, (leak_count>1?"s":""));
				}
#	if defined(PHP_WIN32)
				OutputDebugString(memory_leak_buf);
#	else
				fprintf(stderr, "%s", memory_leak_buf);
#	endif
			}
#endif
			break;
		case ZMSG_MEMORY_LEAKS_GRAND_TOTAL:
#if ZEND_DEBUG
			if (EG(error_reporting) & E_WARNING) {
				char memory_leak_buf[512];

				snprintf(memory_leak_buf, 512, "=== Total %d memory leaks detected ===\n", *((uint32_t *) data));
#	if defined(PHP_WIN32)
				OutputDebugString(memory_leak_buf);
#	else
				fprintf(stderr, "%s", memory_leak_buf);
#	endif
			}
#endif
			break;
		case ZMSG_LOG_SCRIPT_NAME: {
				struct tm *ta, tmbuf;
				time_t curtime;
				char *datetime_str, asctimebuf[52];
				char memory_leak_buf[4096];

				time(&curtime);
				ta = php_localtime_r(&curtime, &tmbuf);
				datetime_str = php_asctime_r(ta, asctimebuf);
				if (datetime_str) {
					datetime_str[strlen(datetime_str)-1]=0;	/* get rid of the trailing newline */
					snprintf(memory_leak_buf, sizeof(memory_leak_buf), "[%s]  Script:  '%s'\n", datetime_str, SAFE_FILENAME(SG(request_info).path_translated));
				} else {
					snprintf(memory_leak_buf, sizeof(memory_leak_buf), "[null]  Script:  '%s'\n", SAFE_FILENAME(SG(request_info).path_translated));
				}
#	if defined(PHP_WIN32)
				OutputDebugString(memory_leak_buf);
#	else
				fprintf(stderr, "%s", memory_leak_buf);
#	endif
			}
			break;
	}
}
/* }}} */


void php_on_timeout(int seconds)
{
	PG(connection_status) |= PHP_CONNECTION_TIMEOUT;
	zend_set_timeout(EG(timeout_seconds), 1);
	if(PG(exit_on_timeout)) sapi_terminate_process();
}

#if PHP_SIGCHILD
/* {{{ sigchld_handler
 */
static void sigchld_handler(int apar)
{
	int errno_save = errno;

	while (waitpid(-1, NULL, WNOHANG) > 0);
	signal(SIGCHLD, sigchld_handler);

	errno = errno_save;
}
/* }}} */
#endif

/* {{{ php_start_sapi()
 */
static int php_start_sapi(void)
{
	int retval = SUCCESS;

	if(!SG(sapi_started)) {
		zend_try {
			PG(during_request_startup) = 1;

			/* initialize global variables */
			PG(modules_activated) = 0;
			PG(header_is_being_sent) = 0;
			PG(connection_status) = PHP_CONNECTION_NORMAL;

			zend_activate();
			zend_set_timeout(EG(timeout_seconds), 1);
			zend_activate_modules();
			PG(modules_activated)=1;
		} zend_catch {
			retval = FAILURE;
		} zend_end_try();

		SG(sapi_started) = 1;
	}
	return retval;
}

/* }}} */

/* {{{ php_request_startup
 */
#ifndef APACHE_HOOKS
int php_request_startup(void)
{
	int retval = SUCCESS;

#ifdef HAVE_DTRACE
	DTRACE_REQUEST_STARTUP(SAFE_FILENAME(SG(request_info).path_translated), SAFE_FILENAME(SG(request_info).request_uri), (char *)SAFE_FILENAME(SG(request_info).request_method));
#endif /* HAVE_DTRACE */

#ifdef PHP_WIN32
	PG(com_initialized) = 0;
#endif

#if PHP_SIGCHILD
	signal(SIGCHLD, sigchld_handler);
#endif

	zend_try {
		PG(in_error_log) = 0;
		PG(during_request_startup) = 1;

		php_output_activate();

		/* initialize global variables */
		PG(modules_activated) = 0;
		PG(header_is_being_sent) = 0;
		PG(connection_status) = PHP_CONNECTION_NORMAL;
		PG(in_user_include) = 0;

		zend_activate();
		sapi_activate();

#ifdef ZEND_SIGNALS
		zend_signal_activate();
#endif

		if (PG(max_input_time) == -1) {
			zend_set_timeout(EG(timeout_seconds), 1);
		} else {
			zend_set_timeout(PG(max_input_time), 1);
		}

		/* Disable realpath cache if an open_basedir is set */
		if (PG(open_basedir) && *PG(open_basedir)) {
			CWDG(realpath_cache_size_limit) = 0;
		}

		if (PG(expose_php)) {
			sapi_add_header(SAPI_PHP_VERSION_HEADER, sizeof(SAPI_PHP_VERSION_HEADER)-1, 1);
		}

		if (PG(output_handler) && PG(output_handler)[0]) {
			zval oh;

			ZVAL_STRING(&oh, PG(output_handler));
			php_output_start_user(&oh, 0, PHP_OUTPUT_HANDLER_STDFLAGS);
			zval_ptr_dtor(&oh);
		} else if (PG(output_buffering)) {
			php_output_start_user(NULL, PG(output_buffering) > 1 ? PG(output_buffering) : 0, PHP_OUTPUT_HANDLER_STDFLAGS);
		} else if (PG(implicit_flush)) {
			php_output_set_implicit_flush(1);
		}

		/* We turn this off in php_execute_script() */
		/* PG(during_request_startup) = 0; */

		php_hash_environment();
		zend_activate_modules();
		PG(modules_activated)=1;
	} zend_catch {
		retval = FAILURE;
	} zend_end_try();

	SG(sapi_started) = 1;

	return retval;
}
# else
int php_request_startup(void)
{
	int retval = SUCCESS;

#if PHP_SIGCHILD
	signal(SIGCHLD, sigchld_handler);
#endif

	if (php_start_sapi() == FAILURE) {
		return FAILURE;
	}

	php_output_activate();
	sapi_activate();
	php_hash_environment();

	zend_try {
		PG(during_request_startup) = 1;
		if (PG(expose_php)) {
			sapi_add_header(SAPI_PHP_VERSION_HEADER, sizeof(SAPI_PHP_VERSION_HEADER)-1, 1);
		}
	} zend_catch {
		retval = FAILURE;
	} zend_end_try();

	return retval;
}
# endif
/* }}} */

/* {{{ php_request_startup_for_hook
 */
int php_request_startup_for_hook(void)
{
	int retval = SUCCESS;

#if PHP_SIGCHLD
	signal(SIGCHLD, sigchld_handler);
#endif

	if (php_start_sapi() == FAILURE) {
		return FAILURE;
	}

	php_output_activate();
	sapi_activate_headers_only();
	php_hash_environment();

	return retval;
}
/* }}} */

/* {{{ php_request_shutdown_for_exec
 */
void php_request_shutdown_for_exec(void *dummy)
{

	/* used to close fd's in the 3..255 range here, but it's problematic
	 */
	shutdown_memory_manager(1, 1);
	zend_interned_strings_restore();
}
/* }}} */

/* {{{ php_request_shutdown_for_hook
 */
void php_request_shutdown_for_hook(void *dummy)
{

	if (PG(modules_activated)) zend_try {
		php_call_shutdown_functions();
	} zend_end_try();

	if (PG(modules_activated)) {
		zend_deactivate_modules();
		php_free_shutdown_functions();
	}

	zend_try {
		zend_unset_timeout();
	} zend_end_try();

	zend_try {
		int i;

		for (i = 0; i < NUM_TRACK_VARS; i++) {
			zval_ptr_dtor(&PG(http_globals)[i]);
		}
	} zend_end_try();

	zend_deactivate();

	zend_try {
		sapi_deactivate();
	} zend_end_try();

	zend_try {
		php_shutdown_stream_hashes();
	} zend_end_try();

	zend_try {
		shutdown_memory_manager(CG(unclean_shutdown), 0);
	} zend_end_try();

	zend_interned_strings_restore();

#ifdef ZEND_SIGNALS
	zend_try {
		zend_signal_deactivate();
	} zend_end_try();
#endif
}

/* }}} */

/* {{{ php_request_shutdown
 */
void php_request_shutdown(void *dummy)
{
	zend_bool report_memleaks;

	report_memleaks = PG(report_memleaks);

	/* EG(current_execute_data) points into nirvana and therefore cannot be safely accessed
	 * inside zend_executor callback functions.
	 */
	EG(current_execute_data) = NULL;

	php_deactivate_ticks();

	/* 1. Call all possible shutdown functions registered with register_shutdown_function() */
	if (PG(modules_activated)) zend_try {
		php_call_shutdown_functions();
	} zend_end_try();

	/* 2. Call all possible __destruct() functions */
	zend_try {
		zend_call_destructors();
	} zend_end_try();

	/* 3. Flush all output buffers */
	zend_try {
		zend_bool send_buffer = SG(request_info).headers_only ? 0 : 1;

		if (CG(unclean_shutdown) && PG(last_error_type) == E_ERROR &&
			(size_t)PG(memory_limit) < zend_memory_usage(1)
		) {
			send_buffer = 0;
		}

		if (!send_buffer) {
			php_output_discard_all();
		} else {
			php_output_end_all();
		}
	} zend_end_try();

	/* 4. Reset max_execution_time (no longer executing php code after response sent) */
	zend_try {
		zend_unset_timeout();
	} zend_end_try();

	/* 5. Call all extensions RSHUTDOWN functions */
	if (PG(modules_activated)) {
		zend_deactivate_modules();
		php_free_shutdown_functions();
	}

	/* 6. Shutdown output layer (send the set HTTP headers, cleanup output handlers, etc.) */
	zend_try {
		php_output_deactivate();
	} zend_end_try();

	/* 7. Destroy super-globals */
	zend_try {
		int i;

		for (i=0; i<NUM_TRACK_VARS; i++) {
			zval_ptr_dtor(&PG(http_globals)[i]);
		}
	} zend_end_try();

	/* 8. free last error information */
	if (PG(last_error_message)) {
		free(PG(last_error_message));
		PG(last_error_message) = NULL;
	}
	if (PG(last_error_file)) {
		free(PG(last_error_file));
		PG(last_error_file) = NULL;
	}

	/* 9. Shutdown scanner/executor/compiler and restore ini entries */
	zend_deactivate();

	/* 10. Call all extensions post-RSHUTDOWN functions */
	zend_try {
		zend_post_deactivate_modules();
	} zend_end_try();

	/* 11. SAPI related shutdown (free stuff) */
	zend_try {
		sapi_deactivate();
	} zend_end_try();

	/* 12. free virtual CWD memory */
	virtual_cwd_deactivate();

	/* 13. Destroy stream hashes */
	zend_try {
		php_shutdown_stream_hashes();
	} zend_end_try();

	/* 14. Free Willy (here be crashes) */
	zend_interned_strings_restore();
	zend_try {
		shutdown_memory_manager(CG(unclean_shutdown) || !report_memleaks, 0);
	} zend_end_try();

	/* 15. Reset max_execution_time */
	zend_try {
		zend_unset_timeout();
	} zend_end_try();

#ifdef PHP_WIN32
	if (PG(com_initialized)) {
		CoUninitialize();
		PG(com_initialized) = 0;
	}
#endif

#ifdef HAVE_DTRACE
	DTRACE_REQUEST_SHUTDOWN(SAFE_FILENAME(SG(request_info).path_translated), SAFE_FILENAME(SG(request_info).request_uri), (char *)SAFE_FILENAME(SG(request_info).request_method));
#endif /* HAVE_DTRACE */
}
/* }}} */

/* {{{ php_com_initialize
 */
PHPAPI void php_com_initialize(void)
{
#ifdef PHP_WIN32
	if (!PG(com_initialized)) {
		if (CoInitialize(NULL) == S_OK) {
			PG(com_initialized) = 1;
		}
	}
#endif
}
/* }}} */

/* {{{ php_output_wrapper
 */
static size_t php_output_wrapper(const char *str, size_t str_length)
{
	return php_output_write(str, str_length);
}
/* }}} */

#ifdef ZTS
/* {{{ core_globals_ctor
 */
static void core_globals_ctor(php_core_globals *core_globals)
{
	memset(core_globals, 0, sizeof(*core_globals));
}
/* }}} */
#endif

/* {{{ core_globals_dtor
 */
static void core_globals_dtor(php_core_globals *core_globals)
{
	if (core_globals->last_error_message) {
		free(core_globals->last_error_message);
	}
	if (core_globals->last_error_file) {
		free(core_globals->last_error_file);
	}
	if (core_globals->disable_functions) {
		free(core_globals->disable_functions);
	}
	if (core_globals->disable_classes) {
		free(core_globals->disable_classes);
	}
	if (core_globals->php_binary) {
		free(core_globals->php_binary);
	}

	php_shutdown_ticks();
}
/* }}} */

PHP_MINFO_FUNCTION(php_core) { /* {{{ */
	php_info_print_table_start();
	php_info_print_table_row(2, "PHP Version", PHP_VERSION);
	php_info_print_table_end();
	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ php_register_extensions
 */
int php_register_extensions(zend_module_entry **ptr, int count)
{
	zend_module_entry **end = ptr + count;

	while (ptr < end) {
		if (*ptr) {
			if (zend_register_internal_module(*ptr)==NULL) {
				return FAILURE;
			}
		}
		ptr++;
	}
	return SUCCESS;
}

/* A very long time ago php_module_startup() was refactored in a way
 * which broke calling it with more than one additional module.
 * This alternative to php_register_extensions() works around that
 * by walking the shallower structure.
 *
 * See algo: https://bugs.php.net/bug.php?id=63159
 */
static int php_register_extensions_bc(zend_module_entry *ptr, int count)
{
	while (count--) {
		if (zend_register_internal_module(ptr++) == NULL) {
			return FAILURE;
 		}
	}
	return SUCCESS;
}
/* }}} */

#if defined(PHP_WIN32) && _MSC_VER >= 1400
static _invalid_parameter_handler old_invalid_parameter_handler;

void dummy_invalid_parameter_handler(
		const wchar_t *expression,
		const wchar_t *function,
		const wchar_t *file,
		unsigned int   line,
		uintptr_t      pEwserved)
{
	static int called = 0;
	char buf[1024];
	int len;

	if (!called) {
			if(PG(windows_show_crt_warning)) {
			called = 1;
			if (function) {
				if (file) {
					len = _snprintf(buf, sizeof(buf)-1, "Invalid parameter detected in CRT function '%ws' (%ws:%u)", function, file, line);
				} else {
					len = _snprintf(buf, sizeof(buf)-1, "Invalid parameter detected in CRT function '%ws'", function);
				}
			} else {
				len = _snprintf(buf, sizeof(buf)-1, "Invalid CRT parameter detected (function not known)");
			}
			zend_error(E_WARNING, "%s", buf);
			called = 0;
		}
	}
}
#endif

/* {{{ php_module_startup
 */
int php_module_startup(sapi_module_struct *sf, zend_module_entry *additional_modules, uint num_additional_modules)
{
	zend_utility_functions zuf;
	zend_utility_values zuv;
	int retval = SUCCESS, module_number=0;	/* for REGISTER_INI_ENTRIES() */
	char *php_os;
	zend_module_entry *module;

#if defined(PHP_WIN32) || (defined(NETWARE) && defined(USE_WINSOCK))
	WORD wVersionRequested = MAKEWORD(2, 0);
	WSADATA wsaData;
#endif
#ifdef PHP_WIN32
	php_os = "WINNT";
#if _MSC_VER >= 1400
	old_invalid_parameter_handler =
		_set_invalid_parameter_handler(dummy_invalid_parameter_handler);
	if (old_invalid_parameter_handler != NULL) {
		_set_invalid_parameter_handler(old_invalid_parameter_handler);
	}

	/* Disable the message box for assertions.*/
	_CrtSetReportMode(_CRT_ASSERT, 0);
#endif
#else
	php_os = PHP_OS;
#endif

#ifdef ZTS
	(void)ts_resource(0);
#endif

#ifdef PHP_WIN32
	php_win32_init_rng_lock();
#endif

	module_shutdown = 0;
	module_startup = 1;
	sapi_initialize_empty_request();
	sapi_activate();

	if (module_initialized) {
		return SUCCESS;
	}

	sapi_module = *sf;

	php_output_startup();

#ifdef ZTS
	ts_allocate_id(&core_globals_id, sizeof(php_core_globals), (ts_allocate_ctor) core_globals_ctor, (ts_allocate_dtor) core_globals_dtor);
	php_startup_ticks();
#ifdef PHP_WIN32
	ts_allocate_id(&php_win32_core_globals_id, sizeof(php_win32_core_globals), (ts_allocate_ctor) php_win32_core_globals_ctor, (ts_allocate_dtor) php_win32_core_globals_dtor);
#endif
#else
	php_startup_ticks();
#endif
	gc_globals_ctor();

	zuf.error_function = php_error_cb;
	zuf.printf_function = php_printf;
	zuf.write_function = php_output_wrapper;
	zuf.fopen_function = php_fopen_wrapper_for_zend;
	zuf.message_handler = php_message_handler_for_zend;
	zuf.block_interruptions = sapi_module.block_interruptions;
	zuf.unblock_interruptions = sapi_module.unblock_interruptions;
	zuf.get_configuration_directive = php_get_configuration_directive_for_zend;
	zuf.ticks_function = php_run_ticks;
	zuf.on_timeout = php_on_timeout;
	zuf.stream_open_function = php_stream_open_for_zend;
	zuf.vspprintf_function = vspprintf;
	zuf.vstrpprintf_function = vstrpprintf;
	zuf.getenv_function = sapi_getenv;
	zuf.resolve_path_function = php_resolve_path_for_zend;
	zend_startup(&zuf, NULL);

#ifdef PHP_WIN32
	{
		OSVERSIONINFOEX *osvi = &EG(windows_version_info);

		ZeroMemory(osvi, sizeof(OSVERSIONINFOEX));
		osvi->dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		if( !GetVersionEx((OSVERSIONINFO *) osvi)) {
			php_printf("\nGetVersionEx unusable. %d\n", GetLastError());
			return FAILURE;
		}
	}
#endif

#if HAVE_SETLOCALE
	setlocale(LC_CTYPE, "");
	zend_update_current_locale();
#endif

#if HAVE_TZSET
	tzset();
#endif

#if defined(PHP_WIN32) || (defined(NETWARE) && defined(USE_WINSOCK))
	/* start up winsock services */
	if (WSAStartup(wVersionRequested, &wsaData) != 0) {
		php_printf("\nwinsock.dll unusable. %d\n", WSAGetLastError());
		return FAILURE;
	}
#endif

	le_index_ptr = zend_register_list_destructors_ex(NULL, NULL, "index pointer", 0);

	/* Register constants */
	REGISTER_MAIN_STRINGL_CONSTANT("PHP_VERSION", PHP_VERSION, sizeof(PHP_VERSION)-1, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("PHP_MAJOR_VERSION", PHP_MAJOR_VERSION, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("PHP_MINOR_VERSION", PHP_MINOR_VERSION, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("PHP_RELEASE_VERSION", PHP_RELEASE_VERSION, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_STRINGL_CONSTANT("PHP_EXTRA_VERSION", PHP_EXTRA_VERSION, sizeof(PHP_EXTRA_VERSION) - 1, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("PHP_VERSION_ID", PHP_VERSION_ID, CONST_PERSISTENT | CONST_CS);
#ifdef ZTS
	REGISTER_MAIN_LONG_CONSTANT("PHP_ZTS", 1, CONST_PERSISTENT | CONST_CS);
#else
	REGISTER_MAIN_LONG_CONSTANT("PHP_ZTS", 0, CONST_PERSISTENT | CONST_CS);
#endif
	REGISTER_MAIN_LONG_CONSTANT("PHP_DEBUG", PHP_DEBUG, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_STRINGL_CONSTANT("PHP_OS", php_os, strlen(php_os), CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_STRINGL_CONSTANT("PHP_SAPI", sapi_module.name, strlen(sapi_module.name), CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_STRINGL_CONSTANT("DEFAULT_INCLUDE_PATH", PHP_INCLUDE_PATH, sizeof(PHP_INCLUDE_PATH)-1, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_STRINGL_CONSTANT("PEAR_INSTALL_DIR", PEAR_INSTALLDIR, sizeof(PEAR_INSTALLDIR)-1, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_STRINGL_CONSTANT("PEAR_EXTENSION_DIR", PHP_EXTENSION_DIR, sizeof(PHP_EXTENSION_DIR)-1, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_STRINGL_CONSTANT("PHP_EXTENSION_DIR", PHP_EXTENSION_DIR, sizeof(PHP_EXTENSION_DIR)-1, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_STRINGL_CONSTANT("PHP_PREFIX", PHP_PREFIX, sizeof(PHP_PREFIX)-1, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_STRINGL_CONSTANT("PHP_BINDIR", PHP_BINDIR, sizeof(PHP_BINDIR)-1, CONST_PERSISTENT | CONST_CS);
#ifndef PHP_WIN32
	REGISTER_MAIN_STRINGL_CONSTANT("PHP_MANDIR", PHP_MANDIR, sizeof(PHP_MANDIR)-1, CONST_PERSISTENT | CONST_CS);
#endif
	REGISTER_MAIN_STRINGL_CONSTANT("PHP_LIBDIR", PHP_LIBDIR, sizeof(PHP_LIBDIR)-1, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_STRINGL_CONSTANT("PHP_DATADIR", PHP_DATADIR, sizeof(PHP_DATADIR)-1, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_STRINGL_CONSTANT("PHP_SYSCONFDIR", PHP_SYSCONFDIR, sizeof(PHP_SYSCONFDIR)-1, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_STRINGL_CONSTANT("PHP_LOCALSTATEDIR", PHP_LOCALSTATEDIR, sizeof(PHP_LOCALSTATEDIR)-1, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_STRINGL_CONSTANT("PHP_CONFIG_FILE_PATH", PHP_CONFIG_FILE_PATH, strlen(PHP_CONFIG_FILE_PATH), CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_STRINGL_CONSTANT("PHP_CONFIG_FILE_SCAN_DIR", PHP_CONFIG_FILE_SCAN_DIR, sizeof(PHP_CONFIG_FILE_SCAN_DIR)-1, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_STRINGL_CONSTANT("PHP_SHLIB_SUFFIX", PHP_SHLIB_SUFFIX, sizeof(PHP_SHLIB_SUFFIX)-1, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_STRINGL_CONSTANT("PHP_EOL", PHP_EOL, sizeof(PHP_EOL)-1, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("PHP_MAXPATHLEN", MAXPATHLEN, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("PHP_INT_MAX", ZEND_LONG_MAX, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("PHP_INT_MIN", ZEND_LONG_MIN, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("PHP_INT_SIZE", SIZEOF_ZEND_LONG, CONST_PERSISTENT | CONST_CS);

#ifdef PHP_WIN32
	REGISTER_MAIN_LONG_CONSTANT("PHP_WINDOWS_VERSION_MAJOR",      EG(windows_version_info).dwMajorVersion, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("PHP_WINDOWS_VERSION_MINOR",      EG(windows_version_info).dwMinorVersion, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("PHP_WINDOWS_VERSION_BUILD",      EG(windows_version_info).dwBuildNumber, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("PHP_WINDOWS_VERSION_PLATFORM",   EG(windows_version_info).dwPlatformId, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("PHP_WINDOWS_VERSION_SP_MAJOR",   EG(windows_version_info).wServicePackMajor, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("PHP_WINDOWS_VERSION_SP_MINOR",   EG(windows_version_info).wServicePackMinor, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("PHP_WINDOWS_VERSION_SUITEMASK",  EG(windows_version_info).wSuiteMask, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("PHP_WINDOWS_VERSION_PRODUCTTYPE", EG(windows_version_info).wProductType, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("PHP_WINDOWS_NT_DOMAIN_CONTROLLER", VER_NT_DOMAIN_CONTROLLER, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("PHP_WINDOWS_NT_SERVER", VER_NT_SERVER, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("PHP_WINDOWS_NT_WORKSTATION", VER_NT_WORKSTATION, CONST_PERSISTENT | CONST_CS);
#endif

	php_binary_init();
	if (PG(php_binary)) {
		REGISTER_MAIN_STRINGL_CONSTANT("PHP_BINARY", PG(php_binary), strlen(PG(php_binary)), CONST_PERSISTENT | CONST_CS);
	} else {
		REGISTER_MAIN_STRINGL_CONSTANT("PHP_BINARY", "", 0, CONST_PERSISTENT | CONST_CS);
	}

	php_output_register_constants();
	php_rfc1867_register_constants();

	/* this will read in php.ini, set up the configuration parameters,
	   load zend extensions and register php function extensions
	   to be loaded later */
	if (php_init_config() == FAILURE) {
		return FAILURE;
	}

	/* Register PHP core ini entries */
	REGISTER_INI_ENTRIES();

	/* Register Zend ini entries */
	zend_register_standard_ini_entries();

	/* Disable realpath cache if an open_basedir is set */
	if (PG(open_basedir) && *PG(open_basedir)) {
		CWDG(realpath_cache_size_limit) = 0;
	}

	/* initialize stream wrappers registry
	 * (this uses configuration parameters from php.ini)
	 */
	if (php_init_stream_wrappers(module_number) == FAILURE)	{
		php_printf("PHP:  Unable to initialize stream url wrappers.\n");
		return FAILURE;
	}

	zuv.html_errors = 1;
	zuv.import_use_extension = ".php";
	zuv.import_use_extension_length = (uint)strlen(zuv.import_use_extension);
	php_startup_auto_globals();
	zend_set_utility_values(&zuv);
	php_startup_sapi_content_types();

	/* startup extensions statically compiled in */
	if (php_register_internal_extensions_func() == FAILURE) {
		php_printf("Unable to start builtin modules\n");
		return FAILURE;
	}

	/* start additional PHP extensions */
	php_register_extensions_bc(additional_modules, num_additional_modules);

	/* load and startup extensions compiled as shared objects (aka DLLs)
	   as requested by php.ini entries
	   these are loaded after initialization of internal extensions
	   as extensions *might* rely on things from ext/standard
	   which is always an internal extension and to be initialized
	   ahead of all other internals
	 */
	php_ini_register_extensions();
	zend_startup_modules();

	/* start Zend extensions */
	zend_startup_extensions();

	zend_collect_module_handlers();

	/* register additional functions */
	if (sapi_module.additional_functions) {
		if ((module = zend_hash_str_find_ptr(&module_registry, "standard", sizeof("standard")-1)) != NULL) {
			EG(current_module) = module;
			zend_register_functions(NULL, sapi_module.additional_functions, NULL, MODULE_PERSISTENT);
			EG(current_module) = NULL;
		}
	}

	/* disable certain classes and functions as requested by php.ini */
	php_disable_functions();
	php_disable_classes();

	/* make core report what it should */
	if ((module = zend_hash_str_find_ptr(&module_registry, "core", sizeof("core")-1)) != NULL) {
		module->version = PHP_VERSION;
		module->info_func = PHP_MINFO(php_core);
	}


#ifdef PHP_WIN32
	/* Disable incompatible functions for the running platform */
	if (php_win32_disable_functions() == FAILURE) {
		php_printf("Unable to disable unsupported functions\n");
		return FAILURE;
	}
#endif

	zend_post_startup();

	module_initialized = 1;

	/* Check for deprecated directives */
	/* NOTE: If you add anything here, remember to add it to Makefile.global! */
	{
		struct {
			const long error_level;
			const char *phrase;
			const char *directives[17]; /* Remember to change this if the number of directives change */
		} directives[2] = {
			{
				E_DEPRECATED,
				"Directive '%s' is deprecated in PHP 5.3 and greater",
				{
					NULL
				}
			},
			{
				E_CORE_ERROR,
				"Directive '%s' is no longer available in PHP",
				{
					"allow_call_time_pass_reference",
					"asp_tags",
					"define_syslog_variables",
					"highlight.bg",
					"magic_quotes_gpc",
					"magic_quotes_runtime",
					"magic_quotes_sybase",
					"register_globals",
					"register_long_arrays",
					"safe_mode",
					"safe_mode_gid",
					"safe_mode_include_dir",
					"safe_mode_exec_dir",
					"safe_mode_allowed_env_vars",
					"safe_mode_protected_env_vars",
					"zend.ze1_compatibility_mode",
					NULL
				}
			}
		};

		unsigned int i;

		zend_try {
			/* 2 = Count of deprecation structs */
			for (i = 0; i < 2; i++) {
				const char **p = directives[i].directives;

				while(*p) {
					zend_long value;

					if (cfg_get_long((char*)*p, &value) == SUCCESS && value) {
						zend_error(directives[i].error_level, directives[i].phrase, *p);
					}

					++p;
				}
			}
		} zend_catch {
			retval = FAILURE;
		} zend_end_try();
	}

	sapi_deactivate();
	module_startup = 0;

	shutdown_memory_manager(1, 0);
	zend_interned_strings_snapshot();
 	virtual_cwd_activate();

	/* we're done */
	return retval;
}
/* }}} */

void php_module_shutdown_for_exec(void)
{
	/* used to close fd's in the range 3.255 here, but it's problematic */
}

/* {{{ php_module_shutdown_wrapper
 */
int php_module_shutdown_wrapper(sapi_module_struct *sapi_globals)
{
	php_module_shutdown();
	return SUCCESS;
}
/* }}} */

/* {{{ php_module_shutdown
 */
void php_module_shutdown(void)
{
	int module_number=0;	/* for UNREGISTER_INI_ENTRIES() */

	module_shutdown = 1;

	if (!module_initialized) {
		return;
	}

#ifdef ZTS
	ts_free_worker_threads();
#endif

#if defined(PHP_WIN32) || (defined(NETWARE) && defined(USE_WINSOCK))
	/*close winsock */
	WSACleanup();
#endif

#ifdef PHP_WIN32
	php_win32_free_rng_lock();
#endif

	sapi_flush();

	zend_shutdown();

	/* Destroys filter & transport registries too */
	php_shutdown_stream_wrappers(module_number);

	UNREGISTER_INI_ENTRIES();

	/* close down the ini config */
	php_shutdown_config();

#ifndef ZTS
	zend_ini_shutdown();
	shutdown_memory_manager(CG(unclean_shutdown), 1);
#else
	zend_ini_global_shutdown();
#endif

	php_output_shutdown();
	php_shutdown_temporary_directory();

	module_initialized = 0;

#ifndef ZTS
	core_globals_dtor(&core_globals);
	gc_globals_dtor();
#else
	ts_free_id(core_globals_id);
#endif

#if defined(PHP_WIN32) && defined(_MSC_VER) && (_MSC_VER >= 1400)
	if (old_invalid_parameter_handler == NULL) {
		_set_invalid_parameter_handler(old_invalid_parameter_handler);
	}
#endif
}
/* }}} */

/* {{{ php_execute_script
 */
PHPAPI int php_execute_script(zend_file_handle *primary_file)
{
	zend_file_handle *prepend_file_p, *append_file_p;
	zend_file_handle prepend_file = {{0}, NULL, NULL, 0, 0}, append_file = {{0}, NULL, NULL, 0, 0};
#if HAVE_BROKEN_GETCWD
	volatile int old_cwd_fd = -1;
#else
	char *old_cwd;
	ALLOCA_FLAG(use_heap)
#endif
	int retval = 0;

	EG(exit_status) = 0;
#ifndef HAVE_BROKEN_GETCWD
# define OLD_CWD_SIZE 4096
	old_cwd = do_alloca(OLD_CWD_SIZE, use_heap);
	old_cwd[0] = '\0';
#endif

	zend_try {
		char realfile[MAXPATHLEN];

#ifdef PHP_WIN32
		if(primary_file->filename) {
			UpdateIniFromRegistry((char*)primary_file->filename);
		}
#endif

		PG(during_request_startup) = 0;

		if (primary_file->filename && !(SG(options) & SAPI_OPTION_NO_CHDIR)) {
#if HAVE_BROKEN_GETCWD
			/* this looks nasty to me */
			old_cwd_fd = open(".", 0);
#else
			php_ignore_value(VCWD_GETCWD(old_cwd, OLD_CWD_SIZE-1));
#endif
			VCWD_CHDIR_FILE(primary_file->filename);
		}

 		/* Only lookup the real file path and add it to the included_files list if already opened
		 *   otherwise it will get opened and added to the included_files list in zend_execute_scripts
		 */
 		if (primary_file->filename &&
 		    (primary_file->filename[0] != '-' || primary_file->filename[1] != 0) &&
 			primary_file->opened_path == NULL &&
 			primary_file->type != ZEND_HANDLE_FILENAME
		) {
			if (expand_filepath(primary_file->filename, realfile)) {
				primary_file->opened_path = zend_string_init(realfile, strlen(realfile), 0);
				zend_hash_add_empty_element(&EG(included_files), primary_file->opened_path);
			}
		}

		if (PG(auto_prepend_file) && PG(auto_prepend_file)[0]) {
			prepend_file.filename = PG(auto_prepend_file);
			prepend_file.opened_path = NULL;
			prepend_file.free_filename = 0;
			prepend_file.type = ZEND_HANDLE_FILENAME;
			prepend_file_p = &prepend_file;
		} else {
			prepend_file_p = NULL;
		}

		if (PG(auto_append_file) && PG(auto_append_file)[0]) {
			append_file.filename = PG(auto_append_file);
			append_file.opened_path = NULL;
			append_file.free_filename = 0;
			append_file.type = ZEND_HANDLE_FILENAME;
			append_file_p = &append_file;
		} else {
			append_file_p = NULL;
		}
		if (PG(max_input_time) != -1) {
#ifdef PHP_WIN32
			zend_unset_timeout();
#endif
			zend_set_timeout(INI_INT("max_execution_time"), 0);
		}

		/*
		   If cli primary file has shabang line and there is a prepend file,
		   the `start_lineno` will be used by prepend file but not primary file,
		   save it and restore after prepend file been executed.
		 */
		if (CG(start_lineno) && prepend_file_p) {
			int orig_start_lineno = CG(start_lineno);

			CG(start_lineno) = 0;
			if (zend_execute_scripts(ZEND_REQUIRE, NULL, 1, prepend_file_p) == SUCCESS) {
				CG(start_lineno) = orig_start_lineno;
				retval = (zend_execute_scripts(ZEND_REQUIRE, NULL, 2, primary_file, append_file_p) == SUCCESS);
			}
		} else {
			retval = (zend_execute_scripts(ZEND_REQUIRE, NULL, 3, prepend_file_p, primary_file, append_file_p) == SUCCESS);
		}
	} zend_end_try();

	if (EG(exception)) {
		zend_try {
			zend_exception_error(EG(exception), E_ERROR);
		} zend_end_try();
	}

#if HAVE_BROKEN_GETCWD
	if (old_cwd_fd != -1) {
		fchdir(old_cwd_fd);
		close(old_cwd_fd);
	}
#else
	if (old_cwd[0] != '\0') {
		php_ignore_value(VCWD_CHDIR(old_cwd));
	}
	free_alloca(old_cwd, use_heap);
#endif
	return retval;
}
/* }}} */

/* {{{ php_execute_simple_script
 */
PHPAPI int php_execute_simple_script(zend_file_handle *primary_file, zval *ret)
{
	char *old_cwd;
	ALLOCA_FLAG(use_heap)

	EG(exit_status) = 0;
#define OLD_CWD_SIZE 4096
	old_cwd = do_alloca(OLD_CWD_SIZE, use_heap);
	old_cwd[0] = '\0';

	zend_try {
#ifdef PHP_WIN32
		if(primary_file->filename) {
			UpdateIniFromRegistry((char*)primary_file->filename);
		}
#endif

		PG(during_request_startup) = 0;

		if (primary_file->filename && !(SG(options) & SAPI_OPTION_NO_CHDIR)) {
			php_ignore_value(VCWD_GETCWD(old_cwd, OLD_CWD_SIZE-1));
			VCWD_CHDIR_FILE(primary_file->filename);
		}
		zend_execute_scripts(ZEND_REQUIRE, ret, 1, primary_file);
	} zend_end_try();

	if (old_cwd[0] != '\0') {
		php_ignore_value(VCWD_CHDIR(old_cwd));
	}

	free_alloca(old_cwd, use_heap);
	return EG(exit_status);
}
/* }}} */

/* {{{ php_handle_aborted_connection
 */
PHPAPI void php_handle_aborted_connection(void)
{

	PG(connection_status) = PHP_CONNECTION_ABORTED;
	php_output_set_status(PHP_OUTPUT_DISABLED);

	if (!PG(ignore_user_abort)) {
		zend_bailout();
	}
}
/* }}} */

/* {{{ php_handle_auth_data
 */
PHPAPI int php_handle_auth_data(const char *auth)
{
	int ret = -1;

	if (auth && auth[0] != '\0' && strncmp(auth, "Basic ", 6) == 0) {
		char *pass;
		zend_string *user;

		user = php_base64_decode((const unsigned char*)auth + 6, strlen(auth) - 6);
		if (user) {
			pass = strchr(user->val, ':');
			if (pass) {
				*pass++ = '\0';
				SG(request_info).auth_user = estrndup(user->val, user->len);
				SG(request_info).auth_password = estrdup(pass);
				ret = 0;
			}
			zend_string_free(user);
		}
	}

	if (ret == -1) {
		SG(request_info).auth_user = SG(request_info).auth_password = NULL;
	} else {
		SG(request_info).auth_digest = NULL;
	}

	if (ret == -1 && auth && auth[0] != '\0' && strncmp(auth, "Digest ", 7) == 0) {
		SG(request_info).auth_digest = estrdup(auth + 7);
		ret = 0;
	}

	if (ret == -1) {
		SG(request_info).auth_digest = NULL;
	}

	return ret;
}
/* }}} */

/* {{{ php_lint_script
 */
PHPAPI int php_lint_script(zend_file_handle *file)
{
	zend_op_array *op_array;
	int retval = FAILURE;

	zend_try {
		op_array = zend_compile_file(file, ZEND_INCLUDE);
		zend_destroy_file_handle(file);

		if (op_array) {
			destroy_op_array(op_array);
			efree(op_array);
			retval = SUCCESS;
		}
	} zend_end_try();
	if (EG(exception)) {
		zend_exception_error(EG(exception), E_ERROR);
	}

	return retval;
}
/* }}} */

#ifdef PHP_WIN32
/* {{{ dummy_indent
   just so that this symbol gets exported... */
PHPAPI void dummy_indent(void)
{
	zend_indent();
}
/* }}} */
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
