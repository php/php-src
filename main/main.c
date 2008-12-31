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
#include "zend_extensions.h"
#include "php_ini.h"
#include "php_globals.h"
#include "php_main.h"
#include "fopen_wrappers.h"
#include "ext/standard/php_standard.h"
#include "ext/standard/php_string.h"
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

#include "php_content_types.h"
#include "php_ticks.h"
#include "php_logos.h"
#include "php_streams.h"
#include "php_open_temporary_file.h"

#include "SAPI.h"
#include "rfc1867.h"
/* }}} */

#ifndef ZTS
php_core_globals core_globals;
#else
PHPAPI int core_globals_id;
#endif

#define SAFE_FILENAME(f) ((f)?(f):"-")

/* {{{ PHP_INI_MH
 */
static PHP_INI_MH(OnSetPrecision)
{
	int i = atoi(new_value);
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
		PG(memory_limit) = zend_atoi(new_value, new_value_length);
	} else {
		PG(memory_limit) = 1<<30;		/* effectively, no limit */
	}
	return zend_set_memory_limit(PG(memory_limit));
}
/* }}} */


/* {{{ php_disable_functions
 */
static void php_disable_functions(TSRMLS_D)
{
	char *s = NULL, *e;

	if (!*(INI_STR("disable_functions"))) {
		return;
	}

	e = PG(disable_functions) = strdup(INI_STR("disable_functions"));

	while (*e) {
		switch (*e) {
			case ' ':
			case ',':
				if (s) {
					*e = '\0';
					zend_disable_function(s, e-s TSRMLS_CC);
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
		zend_disable_function(s, e-s TSRMLS_CC);
	}
}
/* }}} */

/* {{{ php_disable_classes
 */
static void php_disable_classes(TSRMLS_D)
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
					zend_disable_class(s, e-s TSRMLS_CC);
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
		zend_disable_class(s, e-s TSRMLS_CC);
	}
}
/* }}} */

/* {{{ PHP_INI_MH
 */
static PHP_INI_MH(OnUpdateTimeout)
{
	EG(timeout_seconds) = atoi(new_value);
	if (stage==PHP_INI_STAGE_STARTUP) {
		/* Don't set a timeout on startup, only per-request */
		return SUCCESS;
	}
	zend_unset_timeout(TSRMLS_C);
	zend_set_timeout(EG(timeout_seconds));
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
		mode = atoi(value);
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
	PG(display_errors) = (zend_bool) php_get_display_errors_mode(new_value, new_value_length);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_INI_DISP
 */
static PHP_INI_DISP(display_errors_mode)
{
	int mode, tmp_value_length, cgi_or_cli;
	char *tmp_value;
	TSRMLS_FETCH();

	if (type == ZEND_INI_DISPLAY_ORIG && ini_entry->modified) {
		tmp_value = (ini_entry->orig_value ? ini_entry->orig_value : NULL );
		tmp_value_length = ini_entry->orig_value_length;
	} else if (ini_entry->value) {
		tmp_value = ini_entry->value;
		tmp_value_length = ini_entry->value_length;
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
static PHP_INI_MH(OnUpdateErrorLog)
{
	/* Only do the safemode/open_basedir check at runtime */
	if ((stage == PHP_INI_STAGE_RUNTIME || stage == PHP_INI_STAGE_HTACCESS) &&
		strcmp(new_value, "syslog")) {
		if (PG(safe_mode) && (!php_checkuid(new_value, NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
			return FAILURE;
		}

		if (PG(open_basedir) && php_check_open_basedir(new_value TSRMLS_CC)) {
			return FAILURE;
		}

	}
	OnUpdateString(entry, new_value, new_value_length, mh_arg1, mh_arg2, mh_arg3, stage TSRMLS_CC);
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


/* Need to convert to strings and make use of:
 * PHP_SAFE_MODE
 *
 * Need to be read from the environment (?):
 * PHP_AUTO_PREPEND_FILE
 * PHP_AUTO_APPEND_FILE
 * PHP_DOCUMENT_ROOT
 * PHP_USER_DIR
 * PHP_INCLUDE_PATH
 */

#ifndef PHP_SAFE_MODE_EXEC_DIR
#	define PHP_SAFE_MODE_EXEC_DIR ""
#endif

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
	PHP_INI_ENTRY_EX("define_syslog_variables",	"0",				PHP_INI_ALL,	NULL,			php_ini_boolean_displayer_cb)
	PHP_INI_ENTRY_EX("highlight.bg",			HL_BG_COLOR,		PHP_INI_ALL,	NULL,			php_ini_color_displayer_cb)
	PHP_INI_ENTRY_EX("highlight.comment",		HL_COMMENT_COLOR,	PHP_INI_ALL,	NULL,			php_ini_color_displayer_cb)
	PHP_INI_ENTRY_EX("highlight.default",		HL_DEFAULT_COLOR,	PHP_INI_ALL,	NULL,			php_ini_color_displayer_cb)
	PHP_INI_ENTRY_EX("highlight.html",			HL_HTML_COLOR,		PHP_INI_ALL,	NULL,			php_ini_color_displayer_cb)
	PHP_INI_ENTRY_EX("highlight.keyword",		HL_KEYWORD_COLOR,	PHP_INI_ALL,	NULL,			php_ini_color_displayer_cb)
	PHP_INI_ENTRY_EX("highlight.string",		HL_STRING_COLOR,	PHP_INI_ALL,	NULL,			php_ini_color_displayer_cb)

	STD_PHP_INI_BOOLEAN("allow_call_time_pass_reference",	"1",	PHP_INI_SYSTEM|PHP_INI_PERDIR,		OnUpdateBool,	allow_call_time_pass_reference,	zend_compiler_globals,	compiler_globals)
	STD_PHP_INI_BOOLEAN("asp_tags",				"0",		PHP_INI_SYSTEM|PHP_INI_PERDIR,		OnUpdateBool,			asp_tags,				zend_compiler_globals,	compiler_globals)
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
	STD_PHP_INI_BOOLEAN("magic_quotes_gpc",		"1",		PHP_INI_PERDIR|PHP_INI_SYSTEM,	OnUpdateBool,	magic_quotes_gpc,		php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("magic_quotes_runtime",	"0",		PHP_INI_ALL,		OnUpdateBool,			magic_quotes_runtime,	php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("magic_quotes_sybase",	"0",		PHP_INI_ALL,		OnUpdateBool,			magic_quotes_sybase,	php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("output_buffering",		"0",		PHP_INI_PERDIR|PHP_INI_SYSTEM,	OnUpdateLong,	output_buffering,		php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("output_handler",			NULL,		PHP_INI_PERDIR|PHP_INI_SYSTEM,	OnUpdateString,	output_handler,		php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("register_argc_argv",	"1",		PHP_INI_PERDIR|PHP_INI_SYSTEM,	OnUpdateBool,	register_argc_argv,		php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("register_globals",		"0",		PHP_INI_PERDIR|PHP_INI_SYSTEM,	OnUpdateBool,	register_globals,		php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("register_long_arrays",	"1",		PHP_INI_PERDIR|PHP_INI_SYSTEM,	OnUpdateBool,	register_long_arrays,	php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("auto_globals_jit",		"1",		PHP_INI_PERDIR|PHP_INI_SYSTEM,	OnUpdateBool,	auto_globals_jit,	php_core_globals,	core_globals)
#if PHP_SAFE_MODE
	STD_PHP_INI_BOOLEAN("safe_mode",			"1",		PHP_INI_SYSTEM,		OnUpdateBool,			safe_mode,				php_core_globals,	core_globals)
#else
	STD_PHP_INI_BOOLEAN("safe_mode",			"0",		PHP_INI_SYSTEM,		OnUpdateBool,			safe_mode,				php_core_globals,	core_globals)
#endif
	STD_PHP_INI_ENTRY("safe_mode_include_dir",	NULL,		PHP_INI_SYSTEM,		OnUpdateString,			safe_mode_include_dir,	php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("safe_mode_gid",		"0",		PHP_INI_SYSTEM,		OnUpdateBool,			safe_mode_gid,			php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("short_open_tag",	DEFAULT_SHORT_OPEN_TAG,	PHP_INI_SYSTEM|PHP_INI_PERDIR,		OnUpdateBool,			short_tags,				zend_compiler_globals,	compiler_globals)
	STD_PHP_INI_BOOLEAN("sql.safe_mode",		"0",		PHP_INI_SYSTEM,		OnUpdateBool,			sql_safe_mode,			php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("track_errors",			"0",		PHP_INI_ALL,		OnUpdateBool,			track_errors,			php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("y2k_compliance",		"1",		PHP_INI_ALL,		OnUpdateBool,			y2k_compliance,			php_core_globals,	core_globals)

	STD_PHP_INI_ENTRY("unserialize_callback_func",	NULL,	PHP_INI_ALL,		OnUpdateString,			unserialize_callback_func,	php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("serialize_precision",	"100",	PHP_INI_ALL,		OnUpdateLongGEZero,			serialize_precision,	php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("arg_separator.output",	"&",		PHP_INI_ALL,		OnUpdateStringUnempty,	arg_separator.output,	php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("arg_separator.input",	"&",		PHP_INI_SYSTEM|PHP_INI_PERDIR,	OnUpdateStringUnempty,	arg_separator.input,	php_core_globals,	core_globals)

	STD_PHP_INI_ENTRY("auto_append_file",		NULL,		PHP_INI_SYSTEM|PHP_INI_PERDIR,		OnUpdateString,			auto_append_file,		php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("auto_prepend_file",		NULL,		PHP_INI_SYSTEM|PHP_INI_PERDIR,		OnUpdateString,			auto_prepend_file,		php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("doc_root",				NULL,		PHP_INI_SYSTEM,		OnUpdateStringUnempty,	doc_root,				php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("default_charset",		SAPI_DEFAULT_CHARSET,	PHP_INI_ALL,	OnUpdateString,			default_charset,		sapi_globals_struct,sapi_globals)
	STD_PHP_INI_ENTRY("default_mimetype",		SAPI_DEFAULT_MIMETYPE,	PHP_INI_ALL,	OnUpdateString,			default_mimetype,		sapi_globals_struct,sapi_globals)
	STD_PHP_INI_ENTRY("error_log",				NULL,		PHP_INI_ALL,		OnUpdateErrorLog,			error_log,				php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("extension_dir",			PHP_EXTENSION_DIR,		PHP_INI_SYSTEM,		OnUpdateStringUnempty,	extension_dir,			php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("include_path",			PHP_INCLUDE_PATH,		PHP_INI_ALL,		OnUpdateStringUnempty,	include_path,			php_core_globals,	core_globals)
	PHP_INI_ENTRY("max_execution_time",			"30",		PHP_INI_ALL,			OnUpdateTimeout)
	STD_PHP_INI_ENTRY("open_basedir",			NULL,		PHP_INI_SYSTEM,		OnUpdateString,			open_basedir,			php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("safe_mode_exec_dir",		PHP_SAFE_MODE_EXEC_DIR,	PHP_INI_SYSTEM,		OnUpdateString,			safe_mode_exec_dir,		php_core_globals,	core_globals)

	STD_PHP_INI_BOOLEAN("file_uploads",			"1",		PHP_INI_SYSTEM,		OnUpdateBool,			file_uploads,			php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("upload_max_filesize",	"2M",		PHP_INI_SYSTEM|PHP_INI_PERDIR,		OnUpdateLong,			upload_max_filesize,	php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("post_max_size",			"8M",		PHP_INI_SYSTEM|PHP_INI_PERDIR,		OnUpdateLong,			post_max_size,			sapi_globals_struct,sapi_globals)
	STD_PHP_INI_ENTRY("upload_tmp_dir",			NULL,		PHP_INI_SYSTEM,		OnUpdateStringUnempty,	upload_tmp_dir,			php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("max_input_nesting_level", "64",		PHP_INI_SYSTEM|PHP_INI_PERDIR,		OnUpdateLongGEZero,	max_input_nesting_level,			php_core_globals,	core_globals)

	STD_PHP_INI_ENTRY("user_dir",				NULL,		PHP_INI_SYSTEM,		OnUpdateString,			user_dir,				php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("variables_order",		"EGPCS",	PHP_INI_SYSTEM|PHP_INI_PERDIR,		OnUpdateStringUnempty,	variables_order,		php_core_globals,	core_globals)

	STD_PHP_INI_ENTRY("error_append_string",	NULL,		PHP_INI_ALL,		OnUpdateString,			error_append_string,	php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("error_prepend_string",	NULL,		PHP_INI_ALL,		OnUpdateString,			error_prepend_string,	php_core_globals,	core_globals)

	PHP_INI_ENTRY("SMTP",						"localhost",PHP_INI_ALL,		NULL)
	PHP_INI_ENTRY("smtp_port",					"25",		PHP_INI_ALL,		NULL)
	PHP_INI_ENTRY("browscap",					NULL,		PHP_INI_SYSTEM,		NULL)
	PHP_INI_ENTRY("memory_limit",				"128M",		PHP_INI_ALL,		OnChangeMemoryLimit)
	PHP_INI_ENTRY("precision",					"14",		PHP_INI_ALL,		OnSetPrecision)
	PHP_INI_ENTRY("sendmail_from",				NULL,		PHP_INI_ALL,		NULL)
	PHP_INI_ENTRY("sendmail_path",	DEFAULT_SENDMAIL_PATH,	PHP_INI_SYSTEM,		NULL)
	PHP_INI_ENTRY("mail.force_extra_parameters",NULL,		PHP_INI_SYSTEM|PHP_INI_PERDIR,		OnChangeMailForceExtra)
	PHP_INI_ENTRY("disable_functions",			"",			PHP_INI_SYSTEM,		NULL)
	PHP_INI_ENTRY("disable_classes",			"",			PHP_INI_SYSTEM,		NULL)

	STD_PHP_INI_BOOLEAN("allow_url_fopen",		"1",		PHP_INI_SYSTEM,		OnUpdateBool,		allow_url_fopen,		php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("allow_url_include",	"0",		PHP_INI_SYSTEM,		OnUpdateBool,		allow_url_include,		php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("always_populate_raw_post_data",	"0",	PHP_INI_SYSTEM|PHP_INI_PERDIR,	OnUpdateBool,	always_populate_raw_post_data,	php_core_globals,	core_globals)

	STD_PHP_INI_ENTRY("realpath_cache_size",	"16K",		PHP_INI_SYSTEM,		OnUpdateLong,	realpath_cache_size_limit,	virtual_cwd_globals,	cwd_globals)
	STD_PHP_INI_ENTRY("realpath_cache_ttl",		"120",		PHP_INI_SYSTEM,		OnUpdateLong,	realpath_cache_ttl,			virtual_cwd_globals,	cwd_globals)
PHP_INI_END()
/* }}} */

/* True globals (no need for thread safety */
/* But don't make them a single int bitfield */
static int module_initialized = 0;
static int module_startup = 1;
static int module_shutdown = 0;

/* {{{ php_log_err
 */
PHPAPI void php_log_err(char *log_message TSRMLS_DC)
{
	int fd = -1;
	char error_time_str[128];
	struct tm tmbuf;
	time_t error_time;

	/* Try to use the specified logging location. */
	if (PG(error_log) != NULL) {
#ifdef HAVE_SYSLOG_H
		if (!strcmp(PG(error_log), "syslog")) {
			php_syslog(LOG_NOTICE, "%.500s", log_message);
			return;
		}
#endif
		fd = VCWD_OPEN_MODE(PG(error_log), O_CREAT | O_APPEND | O_WRONLY, 0644);
		if (fd != -1) {
			char *tmp;
			int len;
			time(&error_time);
			strftime(error_time_str, sizeof(error_time_str), "%d-%b-%Y %H:%M:%S", php_localtime_r(&error_time, &tmbuf));
			len = spprintf(&tmp, 0, "[%s] %s%s", error_time_str, log_message, PHP_EOL);
#ifdef PHP_WIN32
			php_flock(fd, 2);
#endif
			write(fd, tmp, len);
			efree(tmp);
			close(fd);
			return;
		}
	}

	/* Otherwise fall back to the default logging location, if we have one */

	if (sapi_module.log_message) {
		sapi_module.log_message(log_message);
	}
}
/* }}} */

/* {{{ php_write
   wrapper for modules to use PHPWRITE */
PHPAPI int php_write(void *buf, uint size TSRMLS_DC)
{
	return PHPWRITE(buf, size);
}
/* }}} */

/* {{{ php_printf
 */
PHPAPI int php_printf(const char *format, ...)
{
	va_list args;
	int ret;
	char *buffer;
	int size;
	TSRMLS_FETCH();

	va_start(args, format);
	size = vspprintf(&buffer, 0, format, args);
	ret = PHPWRITE(buffer, size);
	efree(buffer);
	va_end(args);

	return ret;
}
/* }}} */

/* {{{ php_verror helpers */

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

/* }}} */

/* {{{ php_verror */
/* php_verror is called from php_error_docref<n> functions.
 * Its purpose is to unify error messages and automatically generate clickable
 * html error messages if correcponding ini setting (html_errors) is activated.
 * See: CODING_STANDARDS for details.
 */
PHPAPI void php_verror(const char *docref, const char *params, int type, const char *format, va_list args TSRMLS_DC)
{
	char *buffer = NULL, *docref_buf = NULL, *target = NULL;
	char *docref_target = "", *docref_root = "";
	char *p;
	int buffer_len = 0;
	char *space = "";
	char *class_name = "";
	char *function;
	int origin_len;
	char *origin;
	char *message;
	int is_function = 0;

	/* get error text into buffer and escape for html if necessary */
	buffer_len = vspprintf(&buffer, 0, format, args);
	if (PG(html_errors)) {
		int len;
		char *replace = php_escape_html_entities(buffer, buffer_len, &len, 0, ENT_COMPAT, NULL TSRMLS_CC);
		efree(buffer);
		buffer = replace;
		buffer_len = len;
	}

	/* which function caused the problem if any at all */
	if (php_during_module_startup()) {
		function = "PHP Startup";
	} else if (php_during_module_shutdown()) {
		function = "PHP Shutdown";
	} else if (EG(current_execute_data) &&
				EG(current_execute_data)->opline &&
				EG(current_execute_data)->opline->opcode == ZEND_INCLUDE_OR_EVAL
	) {
		switch (EG(current_execute_data)->opline->op2.u.constant.value.lval) {
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
		function = get_active_function_name(TSRMLS_C);
		if (!function || !strlen(function)) {
			function = "Unknown";
		} else {
			is_function = 1;
			class_name = get_active_class_name(&space TSRMLS_CC);
		}
	}

	/* if we still have memory then format the origin */
	if (is_function) {
		origin_len = spprintf(&origin, 0, "%s%s%s(%s)", class_name, space, function, params);
	} else {
		origin_len = spprintf(&origin, 0, "%s", function);
	}

	if (PG(html_errors)) {
		int len;
		char *replace = php_escape_html_entities(origin, origin_len, &len, 0, ENT_COMPAT, NULL TSRMLS_CC);
		efree(origin);
		origin = replace;
	}

	/* origin and buffer available, so lets come up with the error message */
	if (docref && docref[0] == '#') {
		docref_target = strchr(docref, '#');
		docref = NULL;
	}

	/* no docref given but function is known (the default) */
	if (!docref && is_function) {
		int doclen;
		if (space[0] == '\0') {
			doclen = spprintf(&docref_buf, 0, "function.%s", function);
		} else {
			doclen = spprintf(&docref_buf, 0, "%s.%s", class_name, function);
		}
		while((p = strchr(docref_buf, '_')) != NULL) {
			*p = '-';
		}
		docref = php_strtolower(docref_buf, doclen);
	}

	/* we have a docref for a function AND
	 * - we show erroes in html mode OR
	 * - the user wants to see the links anyway
	 */
	if (docref && is_function && (PG(html_errors) || strlen(PG(docref_root)))) {
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
	efree(origin);
	if (docref_buf) {
		efree(docref_buf);
	}

	if (PG(track_errors) && module_initialized && EG(active_symbol_table) && 
			(!EG(user_error_handler) || !(EG(user_error_handler_error_reporting) & type))) {
		zval *tmp;
		ALLOC_INIT_ZVAL(tmp);
		ZVAL_STRINGL(tmp, buffer, buffer_len, 1);
		zend_hash_update(EG(active_symbol_table), "php_errormsg", sizeof("php_errormsg"), (void **) &tmp, sizeof(zval *), NULL);
	}
	efree(buffer);

	php_error(type, "%s", message);
	efree(message);
}
/* }}} */

/* {{{ php_error_docref0 */
/* See: CODING_STANDARDS for details. */
PHPAPI void php_error_docref0(const char *docref TSRMLS_DC, int type, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	php_verror(docref, "", type, format, args TSRMLS_CC);
	va_end(args);
}
/* }}} */

/* {{{ php_error_docref1 */
/* See: CODING_STANDARDS for details. */
PHPAPI void php_error_docref1(const char *docref TSRMLS_DC, const char *param1, int type, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	php_verror(docref, param1, type, format, args TSRMLS_CC);
	va_end(args);
}
/* }}} */

/* {{{ php_error_docref2 */
/* See: CODING_STANDARDS for details. */
PHPAPI void php_error_docref2(const char *docref TSRMLS_DC, const char *param1, const char *param2, int type, const char *format, ...)
{
	char *params;
	va_list args;

	spprintf(&params, 0, "%s,%s", param1, param2);
	va_start(args, format);
	php_verror(docref, params ? params : "...", type, format, args TSRMLS_CC);
	va_end(args);
	if (params) {
		efree(params);
	}
}
/* }}} */

/* {{{ php_html_puts */
PHPAPI void php_html_puts(const char *str, uint size TSRMLS_DC)
{
	zend_html_puts(str, size TSRMLS_CC);
}
/* }}} */

/* {{{ php_suppress_errors */
PHPAPI void php_set_error_handling(error_handling_t error_handling, zend_class_entry *exception_class TSRMLS_DC)
{
	PG(error_handling) = error_handling;
	PG(exception_class) = exception_class;
	if (PG(last_error_message)) {
		free(PG(last_error_message));
		PG(last_error_message) = NULL;
	}
	if (PG(last_error_file)) {
		free(PG(last_error_file));
		PG(last_error_file) = NULL;
	}
	PG(last_error_lineno) = 0;
}
/* }}} */

/* {{{ php_error_cb
 extended error handling function */
static void php_error_cb(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args)
{
	char *buffer;
	int buffer_len, display;
	TSRMLS_FETCH();

	buffer_len = vspprintf(&buffer, PG(log_errors_max_len), format, args);

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
		if (PG(last_error_message)) {
			free(PG(last_error_message));
		}
		if (PG(last_error_file)) {
			free(PG(last_error_file));
		}
		PG(last_error_type) = type;
		PG(last_error_message) = strdup(buffer);
		PG(last_error_file) = strdup(error_filename);
		PG(last_error_lineno) = error_lineno;
	}

	/* according to error handling mode, suppress error, throw exception or show it */
	if (PG(error_handling) != EH_NORMAL) {
		switch (type) {
			case E_ERROR:
			case E_CORE_ERROR:
			case E_COMPILE_ERROR:
			case E_USER_ERROR:
			case E_PARSE:
				/* fatal errors are real errors and cannot be made exceptions */
				break;
			case E_STRICT:
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
				if (PG(error_handling) == EH_THROW && !EG(exception)) {
					zend_throw_error_exception(PG(exception_class), buffer, 0, type TSRMLS_CC);
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
			default:
				error_type_str = "Unknown error";
				break;
		}

		if (!module_initialized || PG(log_errors)) {
			char *log_buffer;
#ifdef PHP_WIN32
			if ((type == E_CORE_ERROR || type == E_CORE_WARNING) && PG(display_startup_errors)) {
				MessageBox(NULL, buffer, error_type_str, MB_OK|ZEND_SERVICE_MB_STYLE);
			}
#endif
			spprintf(&log_buffer, 0, "PHP %s:  %s in %s on line %d", error_type_str, buffer, error_filename, error_lineno);
			php_log_err(log_buffer TSRMLS_CC);
			efree(log_buffer);
		}
		if (PG(display_errors)
			&& ((module_initialized && !PG(during_request_startup))
				|| (PG(display_startup_errors) 
					&& (OG(php_body_write)==php_default_output_func || OG(php_body_write)==php_ub_body_write_no_header || OG(php_body_write)==php_ub_body_write)
					)
				)
			) {

			if (PG(xmlrpc_errors)) {
				php_printf("<?xml version=\"1.0\"?><methodResponse><fault><value><struct><member><name>faultCode</name><value><int>%ld</int></value></member><member><name>faultString</name><value><string>%s:%s in %s on line %d</string></value></member></struct></value></fault></methodResponse>", PG(xmlrpc_error_number), error_type_str, buffer, error_filename, error_lineno);
			} else {
				char *prepend_string = INI_STR("error_prepend_string");
				char *append_string = INI_STR("error_append_string");

				if (PG(html_errors)) {
					if (type == E_ERROR) {
						int len;
						char *buf = php_escape_html_entities(buffer, buffer_len, &len, 0, ENT_COMPAT, NULL TSRMLS_CC);
						php_printf("%s<br />\n<b>%s</b>:  %s in <b>%s</b> on line <b>%d</b><br />\n%s", STR_PRINT(prepend_string), error_type_str, buf, error_filename, error_lineno, STR_PRINT(append_string));
						efree(buf);
					} else {
						php_printf("%s<br />\n<b>%s</b>:  %s in <b>%s</b> on line <b>%d</b><br />\n%s", STR_PRINT(prepend_string), error_type_str, buffer, error_filename, error_lineno, STR_PRINT(append_string));
					}
				} else {
					/* Write CLI/CGI errors to stderr if display_errors = "stderr" */
					if ((!strcmp(sapi_module.name, "cli") || !strcmp(sapi_module.name, "cgi")) &&
						PG(display_errors) == PHP_DISPLAY_ERRORS_STDERR
					) {
						fprintf(stderr, "%s: %s in %s on line %d\n", error_type_str, buffer, error_filename, error_lineno);
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
					ctr.line_len = strlen(ctr.line);
					sapi_header_op(SAPI_HEADER_REPLACE, &ctr TSRMLS_CC);
				}
				/* the parser would return 1 (failure), we can bail out nicely */
				if (type != E_PARSE) {
					/* restore memory limit */
					zend_set_memory_limit(PG(memory_limit));
					efree(buffer);
					zend_objects_store_mark_destructed(&EG(objects_store) TSRMLS_CC);
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

	if (PG(track_errors) && module_initialized && EG(active_symbol_table)) {
		zval *tmp;
		ALLOC_INIT_ZVAL(tmp);
		ZVAL_STRINGL(tmp, buffer, buffer_len, 1);
		zend_hash_update(EG(active_symbol_table), "php_errormsg", sizeof("php_errormsg"), (void **) & tmp, sizeof(zval *), NULL);
	}

	efree(buffer);
}
/* }}} */

/* {{{ proto bool set_time_limit(int seconds)
   Sets the maximum time a script can run */
PHP_FUNCTION(set_time_limit)
{
	zval **new_timeout;

	if (PG(safe_mode)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot set time limit in safe mode");
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &new_timeout) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(new_timeout);
	if (zend_alter_ini_entry("max_execution_time", sizeof("max_execution_time"), Z_STRVAL_PP(new_timeout), Z_STRLEN_PP(new_timeout), PHP_INI_USER, PHP_INI_STAGE_RUNTIME) == SUCCESS) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ php_fopen_wrapper_for_zend
 */
static FILE *php_fopen_wrapper_for_zend(const char *filename, char **opened_path)
{
	TSRMLS_FETCH();

	return php_stream_open_wrapper_as_file((char *)filename, "rb", ENFORCE_SAFE_MODE|USE_PATH|IGNORE_URL_WIN|REPORT_ERRORS|STREAM_OPEN_FOR_INCLUDE, opened_path);
}
/* }}} */

static void stream_closer_for_zend(void *handle TSRMLS_DC) /* {{{ */
{
	php_stream_close((php_stream*)handle);
}
/* }}} */

static long stream_fteller_for_zend(void *handle TSRMLS_DC) /* {{{ */
{
	return (long)php_stream_tell((php_stream*)handle);
}
/* }}} */

static int php_stream_open_for_zend(const char *filename, zend_file_handle *handle TSRMLS_DC) /* {{{ */
{
	return php_stream_open_for_zend_ex(filename, handle, ENFORCE_SAFE_MODE|USE_PATH|REPORT_ERRORS|STREAM_OPEN_FOR_INCLUDE TSRMLS_CC);
}
/* }}} */

PHPAPI int php_stream_open_for_zend_ex(const char *filename, zend_file_handle *handle, int mode TSRMLS_DC) /* {{{ */
{
	php_stream *stream;

	stream = php_stream_open_wrapper((char *)filename, "rb", mode, &handle->opened_path);

	if (stream) {
		handle->type = ZEND_HANDLE_STREAM;
		handle->filename = (char*)filename;
		handle->free_filename = 0;
		handle->handle.stream.handle = stream;
		handle->handle.stream.reader = (zend_stream_reader_t)_php_stream_read;
		handle->handle.stream.closer = stream_closer_for_zend;
		handle->handle.stream.fteller = stream_fteller_for_zend;
		handle->handle.stream.interactive = 0;
		/* suppress warning if this stream is not explicitly closed */
		php_stream_auto_cleanup(stream);

		return SUCCESS;
	}
	return FAILURE;
}
/* }}} */

/* {{{ php_get_configuration_directive_for_zend
 */
static int php_get_configuration_directive_for_zend(char *name, uint name_length, zval *contents)
{
	zval *retval = cfg_get_entry(name, name_length);

	if (retval) {
		*contents = *retval;
		return SUCCESS;
	} else {
		return FAILURE;
	}
}
/* }}} */

/* {{{ php_message_handler_for_zend
 */
static void php_message_handler_for_zend(long message, void *data)
{
	TSRMLS_FETCH();

	switch (message) {
		case ZMSG_FAILED_INCLUDE_FOPEN:
			php_error_docref("function.include" TSRMLS_CC, E_WARNING, "Failed opening '%s' for inclusion (include_path='%s')", php_strip_url_passwd((char *) data), STR_PRINT(PG(include_path)));
			break;
		case ZMSG_FAILED_REQUIRE_FOPEN:
			php_error_docref("function.require" TSRMLS_CC, E_COMPILE_ERROR, "Failed opening required '%s' (include_path='%s')", php_strip_url_passwd((char *) data), STR_PRINT(PG(include_path)));
			break;
		case ZMSG_FAILED_HIGHLIGHT_FOPEN:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed opening '%s' for highlighting", php_strip_url_passwd((char *) data));
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

				snprintf(memory_leak_buf, 512, "=== Total %d memory leaks detected ===\n", *((zend_uint *) data));
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


void php_on_timeout(int seconds TSRMLS_DC)
{
	PG(connection_status) |= PHP_CONNECTION_TIMEOUT;
	zend_set_timeout(EG(timeout_seconds));
}

#if PHP_SIGCHILD
/* {{{ sigchld_handler
 */
static void sigchld_handler(int apar)
{
	while (waitpid(-1, NULL, WNOHANG) > 0);
	signal(SIGCHLD, sigchld_handler);
}
/* }}} */
#endif

/* {{{ php_start_sapi()
 */
static int php_start_sapi(TSRMLS_D)
{
	int retval = SUCCESS;

	if(!SG(sapi_started)) {
		zend_try {
			PG(during_request_startup) = 1;

			/* initialize global variables */
			PG(modules_activated) = 0;
			PG(header_is_being_sent) = 0;
			PG(connection_status) = PHP_CONNECTION_NORMAL;

			zend_activate(TSRMLS_C);
			zend_set_timeout(EG(timeout_seconds));
			zend_activate_modules(TSRMLS_C);
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
int php_request_startup(TSRMLS_D)
{
	int retval = SUCCESS;

#ifdef PHP_WIN32
	PG(com_initialized) = 0;
#endif

#if PHP_SIGCHILD
	signal(SIGCHLD, sigchld_handler);
#endif

	zend_try {
		PG(during_request_startup) = 1;

		php_output_activate(TSRMLS_C);

		/* initialize global variables */
		PG(modules_activated) = 0;
		PG(header_is_being_sent) = 0;
		PG(connection_status) = PHP_CONNECTION_NORMAL;
		PG(in_user_include) = 0;

		zend_activate(TSRMLS_C);
		sapi_activate(TSRMLS_C);

		if (PG(max_input_time) == -1) {
			zend_set_timeout(EG(timeout_seconds));
		} else {
			zend_set_timeout(PG(max_input_time));
		}

		/* Disable realpath cache if safe_mode or open_basedir are set */
		if (PG(safe_mode) || (PG(open_basedir) && *PG(open_basedir))) {
			CWDG(realpath_cache_size_limit) = 0;
		}

		if (PG(expose_php)) {
			sapi_add_header(SAPI_PHP_VERSION_HEADER, sizeof(SAPI_PHP_VERSION_HEADER)-1, 1);
		}

		if (PG(output_handler) && PG(output_handler)[0]) {
			php_start_ob_buffer_named(PG(output_handler), 0, 1 TSRMLS_CC);
		} else if (PG(output_buffering)) {
			if (PG(output_buffering)>1) {
				php_start_ob_buffer(NULL, PG(output_buffering), 1 TSRMLS_CC);
			} else {
				php_start_ob_buffer(NULL, 0, 1 TSRMLS_CC);
			}
		} else if (PG(implicit_flush)) {
			php_start_implicit_flush(TSRMLS_C);
		}

		/* We turn this off in php_execute_script() */
		/* PG(during_request_startup) = 0; */

		php_hash_environment(TSRMLS_C);
		zend_activate_modules(TSRMLS_C);
		PG(modules_activated)=1;
	} zend_catch {
		retval = FAILURE;
	} zend_end_try();

	SG(sapi_started) = 1;

	return retval;
}
# else
int php_request_startup(TSRMLS_D)
{
	int retval = SUCCESS;

#if PHP_SIGCHILD
	signal(SIGCHLD, sigchld_handler);
#endif

	if (php_start_sapi() == FAILURE) {
		return FAILURE;
	}

	php_output_activate(TSRMLS_C);
	sapi_activate(TSRMLS_C);
	php_hash_environment(TSRMLS_C);

	zend_try {
		PG(during_request_startup) = 1;
		php_output_activate(TSRMLS_C);
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
int php_request_startup_for_hook(TSRMLS_D)
{
	int retval = SUCCESS;

#if PHP_SIGCHLD
	signal(SIGCHLD, sigchld_handler);
#endif

	if (php_start_sapi(TSRMLS_C) == FAILURE) {
		return FAILURE;
	}

	php_output_activate(TSRMLS_C);
	sapi_activate_headers_only(TSRMLS_C);
	php_hash_environment(TSRMLS_C);

	return retval;
}
/* }}} */

/* {{{ php_request_shutdown_for_exec
 */
void php_request_shutdown_for_exec(void *dummy)
{
	TSRMLS_FETCH();

	/* used to close fd's in the 3..255 range here, but it's problematic
	 */
	shutdown_memory_manager(1, 1 TSRMLS_CC);
}
/* }}} */

/* {{{ php_request_shutdown_for_hook
 */
void php_request_shutdown_for_hook(void *dummy)
{
	TSRMLS_FETCH();

	if (PG(modules_activated)) zend_try {
		php_call_shutdown_functions(TSRMLS_C);
	} zend_end_try();

	if (PG(modules_activated)) {
		zend_deactivate_modules(TSRMLS_C);
		php_free_shutdown_functions(TSRMLS_C);
	}

	zend_try {
		int i;

		for (i = 0; i < NUM_TRACK_VARS; i++) {
			if (PG(http_globals)[i]) {
				zval_ptr_dtor(&PG(http_globals)[i]);
			}
		}
	} zend_end_try();

	zend_deactivate(TSRMLS_C);

	zend_try {
		sapi_deactivate(TSRMLS_C);
	} zend_end_try();

	zend_try {
		php_shutdown_stream_hashes(TSRMLS_C);
	} zend_end_try();

	zend_try {
		shutdown_memory_manager(CG(unclean_shutdown), 0 TSRMLS_CC);
	} zend_end_try();

	zend_try {
		zend_unset_timeout(TSRMLS_C);
	} zend_end_try();
}

/* }}} */

/* {{{ php_request_shutdown
 */
void php_request_shutdown(void *dummy)
{
	zend_bool report_memleaks;
	TSRMLS_FETCH();

	report_memleaks = PG(report_memleaks);

	/* EG(opline_ptr) points into nirvana and therefore cannot be safely accessed
	 * inside zend_executor callback functions.
	 */
	EG(opline_ptr) = NULL;
	EG(active_op_array) = NULL;

	php_deactivate_ticks(TSRMLS_C);

	/* 1. Call all possible shutdown functions registered with register_shutdown_function() */
	if (PG(modules_activated)) zend_try {
		php_call_shutdown_functions(TSRMLS_C);
	} zend_end_try();

	/* 2. Call all possible __destruct() functions */
	zend_try {
		zend_call_destructors(TSRMLS_C);
	} zend_end_try();

	/* 3. Flush all output buffers */
	zend_try {
		php_end_ob_buffers((zend_bool)(SG(request_info).headers_only?0:1) TSRMLS_CC);
	} zend_end_try();

	/* 4. Send the set HTTP headers (note: This must be done AFTER php_end_ob_buffers() !!) */
	zend_try {
		sapi_send_headers(TSRMLS_C);
	} zend_end_try();

	/* 5. Call all extensions RSHUTDOWN functions */
	if (PG(modules_activated)) {
		zend_deactivate_modules(TSRMLS_C);
		php_free_shutdown_functions(TSRMLS_C);
	}

	/* 6. Destroy super-globals */
	zend_try {
		int i;

		for (i=0; i<NUM_TRACK_VARS; i++) {
			if (PG(http_globals)[i]) {
				zval_ptr_dtor(&PG(http_globals)[i]);
			}
		}
	} zend_end_try();

	/* 6.5 free last error information */
	if (PG(last_error_message)) {
		free(PG(last_error_message));
		PG(last_error_message) = NULL;
	}
	if (PG(last_error_file)) {
		free(PG(last_error_file));
		PG(last_error_file) = NULL;
	}

	/* 7. Shutdown scanner/executor/compiler and restore ini entries */
	zend_deactivate(TSRMLS_C);

	/* 8. Call all extensions post-RSHUTDOWN functions */
	zend_try {
		zend_post_deactivate_modules(TSRMLS_C);
	} zend_end_try();

	/* 9. SAPI related shutdown (free stuff) */
	zend_try {
		sapi_deactivate(TSRMLS_C);
	} zend_end_try();

	/* 10. Destroy stream hashes */
	zend_try {
		php_shutdown_stream_hashes(TSRMLS_C);
	} zend_end_try();

	/* 11. Free Willy (here be crashes) */
	zend_try {
		shutdown_memory_manager(CG(unclean_shutdown) || !report_memleaks, 0 TSRMLS_CC);
	} zend_end_try();

	/* 12. Reset max_execution_time */
	zend_try {
		zend_unset_timeout(TSRMLS_C);
	} zend_end_try();

#ifdef PHP_WIN32
	if (PG(com_initialized)) {
		CoUninitialize();
		PG(com_initialized) = 0;
	}
#endif
}
/* }}} */

/* {{{ php_com_initialize
 */
PHPAPI void php_com_initialize(TSRMLS_D)
{
#ifdef PHP_WIN32
	if (!PG(com_initialized)) {
		CoInitialize(NULL);
		PG(com_initialized) = 1;
	}
#endif
}
/* }}} */

/* {{{ php_body_write_wrapper
 */
static int php_body_write_wrapper(const char *str, uint str_length)
{
	TSRMLS_FETCH();
	return php_body_write(str, str_length TSRMLS_CC);
}
/* }}} */

#ifdef ZTS
/* {{{ core_globals_ctor
 */
static void core_globals_ctor(php_core_globals *core_globals TSRMLS_DC)
{
	memset(core_globals, 0, sizeof(*core_globals));
}
/* }}} */
#endif

/* {{{ core_globals_dtor
 */
static void core_globals_dtor(php_core_globals *core_globals TSRMLS_DC)
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
}
/* }}} */

/* {{{ php_register_extensions
 */
int php_register_extensions(zend_module_entry **ptr, int count TSRMLS_DC)
{
	zend_module_entry **end = ptr + count;

	while (ptr < end) {
		if (*ptr) {
			if (zend_register_internal_module(*ptr TSRMLS_CC)==NULL) {
				return FAILURE;
			}
		}
		ptr++;
	}
	return SUCCESS;
}
/* }}} */

#if defined(PHP_WIN32) && defined(_MSC_VER) && (_MSC_VER >= 1400)
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
		called = 1;
		if (function) {
			if (file) {
				len = _snprintf(buf, sizeof(buf)-1, "Invalid parameter detected in CRT function '%ws' (%ws:%d)", function, file, line);
			} else {
				len = _snprintf(buf, sizeof(buf)-1, "Invalid parameter detected in CRT function '%ws'", function);
			}
		} else {
			len = _snprintf(buf, sizeof(buf)-1, "Invalid CRT parameters detected");
		}
		zend_error(E_WARNING, "%s", buf);
		called = 0;
	}
}
#endif

/* {{{ php_module_startup
 */
int php_module_startup(sapi_module_struct *sf, zend_module_entry *additional_modules, uint num_additional_modules)
{
	zend_utility_functions zuf;
	zend_utility_values zuv;
	int module_number=0;	/* for REGISTER_INI_ENTRIES() */
	char *php_os;
#ifdef ZTS
	zend_executor_globals *executor_globals;
	void ***tsrm_ls;

	php_core_globals *core_globals;
#endif
#if defined(PHP_WIN32) || (defined(NETWARE) && defined(USE_WINSOCK))
	WORD wVersionRequested = MAKEWORD(2, 0);
	WSADATA wsaData;
#endif
#ifdef PHP_WIN32
	{
		DWORD dwVersion = GetVersion();

		/* Get build numbers for Windows NT or Win95 */
		if (dwVersion < 0x80000000){
			php_os="WINNT";
		} else {
			php_os="WIN32";
		}
	}
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
	old_invalid_parameter_handler =
		_set_invalid_parameter_handler(dummy_invalid_parameter_handler);
	if (old_invalid_parameter_handler != NULL) {
		_set_invalid_parameter_handler(old_invalid_parameter_handler);
	}
#endif
#else
	php_os=PHP_OS;
#endif

#ifdef ZTS
	tsrm_ls = ts_resource(0);
#endif

	module_shutdown = 0;
	module_startup = 1;
	sapi_initialize_empty_request(TSRMLS_C);
	sapi_activate(TSRMLS_C);

	if (module_initialized) {
		return SUCCESS;
	}

	sapi_module = *sf;

	php_output_startup();

	zuf.error_function = php_error_cb;
	zuf.printf_function = php_printf;
	zuf.write_function = php_body_write_wrapper;
	zuf.fopen_function = php_fopen_wrapper_for_zend;
	zuf.message_handler = php_message_handler_for_zend;
	zuf.block_interruptions = sapi_module.block_interruptions;
	zuf.unblock_interruptions = sapi_module.unblock_interruptions;
	zuf.get_configuration_directive = php_get_configuration_directive_for_zend;
	zuf.ticks_function = php_run_ticks;
	zuf.on_timeout = php_on_timeout;
	zuf.stream_open_function = php_stream_open_for_zend;
	zuf.vspprintf_function = vspprintf;
	zuf.getenv_function = sapi_getenv;
	zend_startup(&zuf, NULL, 1);

#ifdef ZTS
	executor_globals = ts_resource(executor_globals_id);
	ts_allocate_id(&core_globals_id, sizeof(php_core_globals), (ts_allocate_ctor) core_globals_ctor, (ts_allocate_dtor) core_globals_dtor);
	core_globals = ts_resource(core_globals_id);
#ifdef PHP_WIN32
	ts_allocate_id(&php_win32_core_globals_id, sizeof(php_win32_core_globals), (ts_allocate_ctor) php_win32_core_globals_ctor, (ts_allocate_dtor) php_win32_core_globals_dtor);
#endif
#endif
	EG(bailout) = NULL;
	EG(error_reporting) = E_ALL & ~E_NOTICE;

	PG(header_is_being_sent) = 0;
	SG(request_info).headers_only = 0;
	SG(request_info).argv0 = NULL;
	SG(request_info).argc=0;
	SG(request_info).argv=(char **)NULL;
	PG(connection_status) = PHP_CONNECTION_NORMAL;
	PG(during_request_startup) = 0;
	PG(last_error_message) = NULL;
	PG(last_error_file) = NULL;
	PG(last_error_lineno) = 0;
	PG(error_handling) = EH_NORMAL;
	PG(disable_functions) = NULL;
	PG(disable_classes) = NULL;

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

	/* this will read in php.ini, set up the configuration parameters,
	   load zend extensions and register php function extensions
	   to be loaded later */
	if (php_init_config(TSRMLS_C) == FAILURE) {
		return FAILURE;
	}

	/* Register PHP core ini entries */
	REGISTER_INI_ENTRIES();

	/* Register Zend ini entries */
	zend_register_standard_ini_entries(TSRMLS_C);

	/* Disable realpath cache if safe_mode or open_basedir are set */
	if (PG(safe_mode) || (PG(open_basedir) && *PG(open_basedir))) {
		CWDG(realpath_cache_size_limit) = 0;
	}

	/* initialize stream wrappers registry
	 * (this uses configuration parameters from php.ini)
	 */
	if (php_init_stream_wrappers(module_number TSRMLS_CC) == FAILURE)	{
		php_printf("PHP:  Unable to initialize stream url wrappers.\n");
		return FAILURE;
	}

	/* initialize registry for images to be used in phpinfo() 
	   (this uses configuration parameters from php.ini)
	 */
	if (php_init_info_logos() == FAILURE) {
		php_printf("PHP:  Unable to initialize info phpinfo logos.\n");
		return FAILURE;
	}

	zuv.html_errors = 1;
	zuv.import_use_extension = ".php";
	php_startup_auto_globals(TSRMLS_C);
	zend_set_utility_values(&zuv);
	php_startup_sapi_content_types(TSRMLS_C);

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
	REGISTER_MAIN_STRINGL_CONSTANT("PHP_LIBDIR", PHP_LIBDIR, sizeof(PHP_LIBDIR)-1, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_STRINGL_CONSTANT("PHP_DATADIR", PHP_DATADIR, sizeof(PHP_DATADIR)-1, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_STRINGL_CONSTANT("PHP_SYSCONFDIR", PHP_SYSCONFDIR, sizeof(PHP_SYSCONFDIR)-1, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_STRINGL_CONSTANT("PHP_LOCALSTATEDIR", PHP_LOCALSTATEDIR, sizeof(PHP_LOCALSTATEDIR)-1, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_STRINGL_CONSTANT("PHP_CONFIG_FILE_PATH", PHP_CONFIG_FILE_PATH, strlen(PHP_CONFIG_FILE_PATH), CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_STRINGL_CONSTANT("PHP_CONFIG_FILE_SCAN_DIR", PHP_CONFIG_FILE_SCAN_DIR, sizeof(PHP_CONFIG_FILE_SCAN_DIR)-1, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_STRINGL_CONSTANT("PHP_SHLIB_SUFFIX", PHP_SHLIB_SUFFIX, sizeof(PHP_SHLIB_SUFFIX)-1, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_STRINGL_CONSTANT("PHP_EOL", PHP_EOL, sizeof(PHP_EOL)-1, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("PHP_INT_MAX", LONG_MAX, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("PHP_INT_SIZE", sizeof(long), CONST_PERSISTENT | CONST_CS);
	php_output_register_constants(TSRMLS_C);
	php_rfc1867_register_constants(TSRMLS_C);

	if (php_startup_ticks(TSRMLS_C) == FAILURE) {
		php_printf("Unable to start PHP ticks\n");
		return FAILURE;
	}

	/* Register internal Zend classes */
	zend_register_default_classes(TSRMLS_C);

	/* startup extensions staticly compiled in */
	if (php_register_internal_extensions(TSRMLS_C) == FAILURE) {
		php_printf("Unable to start builtin modules\n");
		return FAILURE;
	}

	/* start additional PHP extensions */
	php_register_extensions(&additional_modules, num_additional_modules TSRMLS_CC);

	/* load and startup extensions compiled as shared objects (aka DLLs)
	   as requested by php.ini entries
	   theese are loaded after initialization of internal extensions
	   as extensions *might* rely on things from ext/standard
	   which is always an internal extension and to be initialized
	   ahead of all other internals
	 */
	php_ini_register_extensions(TSRMLS_C);
	zend_startup_modules(TSRMLS_C);

	/* disable certain classes and functions as requested by php.ini */
	php_disable_functions(TSRMLS_C);
	php_disable_classes(TSRMLS_C);

	/* start Zend extensions */
	zend_startup_extensions();

#ifdef ZTS
	zend_post_startup(TSRMLS_C);
#endif

	module_initialized = 1;
	sapi_deactivate(TSRMLS_C);
	module_startup = 0;

	shutdown_memory_manager(1, 0 TSRMLS_CC);

	/* we're done */
	return SUCCESS;
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
	TSRMLS_FETCH();
	php_module_shutdown(TSRMLS_C);
	return SUCCESS;
}
/* }}} */

/* {{{ php_module_shutdown
 */
void php_module_shutdown(TSRMLS_D)
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

	php_shutdown_ticks(TSRMLS_C);
	sapi_flush(TSRMLS_C);

	zend_shutdown(TSRMLS_C);
	
	/* Destroys filter & transport registries too */
	php_shutdown_stream_wrappers(module_number TSRMLS_CC);

	php_shutdown_info_logos();
	UNREGISTER_INI_ENTRIES();

	/* close down the ini config */
	php_shutdown_config();

#ifndef ZTS
	zend_ini_shutdown(TSRMLS_C);
	shutdown_memory_manager(CG(unclean_shutdown), 1 TSRMLS_CC);
	core_globals_dtor(&core_globals TSRMLS_CC);
#else
	zend_ini_global_shutdown(TSRMLS_C);
	ts_free_id(core_globals_id);
#endif

	php_shutdown_temporary_directory();

	module_initialized = 0;

#if defined(PHP_WIN32) && defined(_MSC_VER) && (_MSC_VER >= 1400)
	if (old_invalid_parameter_handler == NULL) {
		_set_invalid_parameter_handler(old_invalid_parameter_handler);
	}
#endif
}
/* }}} */

/* {{{ php_execute_script
 */
PHPAPI int php_execute_script(zend_file_handle *primary_file TSRMLS_DC)
{
	zend_file_handle *prepend_file_p, *append_file_p;
	zend_file_handle prepend_file = {0}, append_file = {0};
#if HAVE_BROKEN_GETCWD 
	int old_cwd_fd = -1;
#else
	char *old_cwd;
#endif
	int retval = 0;

	EG(exit_status) = 0;
	if (php_handle_special_queries(TSRMLS_C)) {
		zend_file_handle_dtor(primary_file);
		return 0;
	}
#ifndef HAVE_BROKEN_GETCWD
# define OLD_CWD_SIZE 4096
	old_cwd = do_alloca(OLD_CWD_SIZE);
	old_cwd[0] = '\0';
#endif

	zend_try {
		char realfile[MAXPATHLEN];

#ifdef PHP_WIN32
		UpdateIniFromRegistry(primary_file->filename TSRMLS_CC);
#endif

		PG(during_request_startup) = 0;

		if ((primary_file->type == ZEND_HANDLE_FILENAME || primary_file->type == ZEND_HANDLE_STREAM) && primary_file->filename) {
#if HAVE_BROKEN_GETCWD
			/* this looks nasty to me */
			old_cwd_fd = open(".", 0);
#else
			VCWD_GETCWD(old_cwd, OLD_CWD_SIZE-1);
#endif
			VCWD_CHDIR_FILE(primary_file->filename);
		}

 		/* Only lookup the real file path and add it to the included_files list if already opened
		 *   otherwise it will get opened and added to the included_files list in zend_execute_scripts
		 */
 		if (primary_file->filename &&
 			primary_file->opened_path == NULL &&
 			primary_file->type != ZEND_HANDLE_FILENAME
		) {
			int realfile_len;
			int dummy = 1;

			if (expand_filepath(primary_file->filename, realfile TSRMLS_CC)) {
				realfile_len =  strlen(realfile);
				zend_hash_add(&EG(included_files), realfile, realfile_len+1, (void *)&dummy, sizeof(int), NULL);
				primary_file->opened_path = estrndup(realfile, realfile_len);
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
			zend_unset_timeout(TSRMLS_C);
#endif
			zend_set_timeout(INI_INT("max_execution_time"));
		}
		retval = (zend_execute_scripts(ZEND_REQUIRE TSRMLS_CC, NULL, 3, prepend_file_p, primary_file, append_file_p) == SUCCESS);

	} zend_end_try();

#if HAVE_BROKEN_GETCWD
	if (old_cwd_fd != -1) {
		fchdir(old_cwd_fd);
		close(old_cwd_fd);
	}
#else
	if (old_cwd[0] != '\0') {
		VCWD_CHDIR(old_cwd);
	}
	free_alloca(old_cwd);
#endif
	return retval;
}
/* }}} */

/* {{{ php_execute_simple_script
 */
PHPAPI int php_execute_simple_script(zend_file_handle *primary_file, zval **ret TSRMLS_DC)
{
	char *old_cwd;

	EG(exit_status) = 0;
#define OLD_CWD_SIZE 4096
	old_cwd = do_alloca(OLD_CWD_SIZE);
	old_cwd[0] = '\0';

	zend_try {
#ifdef PHP_WIN32
		UpdateIniFromRegistry(primary_file->filename TSRMLS_CC);
#endif

		PG(during_request_startup) = 0;

		if (primary_file->type == ZEND_HANDLE_FILENAME && primary_file->filename) {
			VCWD_GETCWD(old_cwd, OLD_CWD_SIZE-1);
			VCWD_CHDIR_FILE(primary_file->filename);
		}
		zend_execute_scripts(ZEND_REQUIRE TSRMLS_CC, ret, 1, primary_file);
	} zend_end_try();

	if (old_cwd[0] != '\0') {
		VCWD_CHDIR(old_cwd);
	}

	free_alloca(old_cwd);
	return EG(exit_status);
}
/* }}} */

/* {{{ php_handle_aborted_connection
 */
PHPAPI void php_handle_aborted_connection(void)
{
	TSRMLS_FETCH();

	PG(connection_status) = PHP_CONNECTION_ABORTED;
	php_output_set_status(0 TSRMLS_CC);

	if (!PG(ignore_user_abort)) {
		zend_bailout();
	}
}
/* }}} */

/* {{{ php_handle_auth_data
 */
PHPAPI int php_handle_auth_data(const char *auth TSRMLS_DC)
{
	int ret = -1;

	if (auth && auth[0] != '\0' && strncmp(auth, "Basic ", 6) == 0) {
		char *pass;
		char *user;

		user = php_base64_decode(auth + 6, strlen(auth) - 6, NULL);
		if (user) {
			pass = strchr(user, ':');
			if (pass) {
				*pass++ = '\0';
				SG(request_info).auth_user = user;
				SG(request_info).auth_password = estrdup(pass);
				ret = 0;
			} else {
				efree(user);
			}
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
PHPAPI int php_lint_script(zend_file_handle *file TSRMLS_DC)
{
	zend_op_array *op_array;
	int retval = FAILURE;

	zend_try {
		op_array = zend_compile_file(file, ZEND_INCLUDE TSRMLS_CC);
		zend_destroy_file_handle(file TSRMLS_CC);

		if (op_array) {
			destroy_op_array(op_array TSRMLS_CC);
			efree(op_array);
			retval = SUCCESS;
		}
	} zend_end_try();

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
