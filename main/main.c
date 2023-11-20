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
   |          Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
*/

/* {{{ includes */

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
#endif
#if HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <signal.h>
#include <locale.h>
#include "zend.h"
#include "zend_types.h"
#include "zend_extensions.h"
#include "php_ini.h"
#include "php_globals.h"
#include "php_main.h"
#include "php_syslog.h"
#include "fopen_wrappers.h"
#include "ext/standard/php_standard.h"
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
#include "zend_extensions.h"
#include "zend_ini.h"
#include "zend_dtrace.h"
#include "zend_observer.h"
#include "zend_system_id.h"

#include "php_content_types.h"
#include "php_ticks.h"
#include "php_streams.h"
#include "php_open_temporary_file.h"

#include "SAPI.h"
#include "rfc1867.h"

#include "ext/standard/html_tables.h"
/* }}} */

PHPAPI int (*php_register_internal_extensions_func)(void) = php_register_internal_extensions;

#ifndef ZTS
php_core_globals core_globals;
#else
PHPAPI int core_globals_id;
PHPAPI size_t core_globals_offset;
#endif

#define SAFE_FILENAME(f) ((f)?(f):"-")

/* {{{ PHP_INI_MH */
static PHP_INI_MH(OnSetFacility)
{
	const zend_string *facility = new_value;

#ifdef LOG_AUTH
	if (zend_string_equals_literal(facility, "LOG_AUTH") || zend_string_equals_literal(facility, "auth")
			|| zend_string_equals_literal(facility, "security")) {
		PG(syslog_facility) = LOG_AUTH;
		return SUCCESS;
	}
#endif
#ifdef LOG_AUTHPRIV
	if (zend_string_equals_literal(facility, "LOG_AUTHPRIV") || zend_string_equals_literal(facility, "authpriv")) {
		PG(syslog_facility) = LOG_AUTHPRIV;
		return SUCCESS;
	}
#endif
#ifdef LOG_CRON
	if (zend_string_equals_literal(facility, "LOG_CRON") || zend_string_equals_literal(facility, "cron")) {
		PG(syslog_facility) = LOG_CRON;
		return SUCCESS;
	}
#endif
#ifdef LOG_DAEMON
	if (zend_string_equals_literal(facility, "LOG_DAEMON") || zend_string_equals_literal(facility, "daemon")) {
		PG(syslog_facility) = LOG_DAEMON;
		return SUCCESS;
	}
#endif
#ifdef LOG_FTP
	if (zend_string_equals_literal(facility, "LOG_FTP") || zend_string_equals_literal(facility, "ftp")) {
		PG(syslog_facility) = LOG_FTP;
		return SUCCESS;
	}
#endif
#ifdef LOG_KERN
	if (zend_string_equals_literal(facility, "LOG_KERN") || zend_string_equals_literal(facility, "kern")) {
		PG(syslog_facility) = LOG_KERN;
		return SUCCESS;
	}
#endif
#ifdef LOG_LPR
	if (zend_string_equals_literal(facility, "LOG_LPR") || zend_string_equals_literal(facility, "lpr")) {
		PG(syslog_facility) = LOG_LPR;
		return SUCCESS;
	}
#endif
#ifdef LOG_MAIL
	if (zend_string_equals_literal(facility, "LOG_MAIL") || zend_string_equals_literal(facility, "mail")) {
		PG(syslog_facility) = LOG_MAIL;
		return SUCCESS;
	}
#endif
#ifdef LOG_INTERNAL_MARK
	if (zend_string_equals_literal(facility, "LOG_INTERNAL_MARK") || zend_string_equals_literal(facility, "mark")) {
		PG(syslog_facility) = LOG_INTERNAL_MARK;
		return SUCCESS;
	}
#endif
#ifdef LOG_NEWS
	if (zend_string_equals_literal(facility, "LOG_NEWS") || zend_string_equals_literal(facility, "news")) {
		PG(syslog_facility) = LOG_NEWS;
		return SUCCESS;
	}
#endif
#ifdef LOG_SYSLOG
	if (zend_string_equals_literal(facility, "LOG_SYSLOG") || zend_string_equals_literal(facility, "syslog")) {
		PG(syslog_facility) = LOG_SYSLOG;
		return SUCCESS;
	}
#endif
#ifdef LOG_USER
	if (zend_string_equals_literal(facility, "LOG_USER") || zend_string_equals_literal(facility, "user")) {
		PG(syslog_facility) = LOG_USER;
		return SUCCESS;
	}
#endif
#ifdef LOG_UUCP
	if (zend_string_equals_literal(facility, "LOG_UUCP") || zend_string_equals_literal(facility, "uucp")) {
		PG(syslog_facility) = LOG_UUCP;
		return SUCCESS;
	}
#endif
#ifdef LOG_LOCAL0
	if (zend_string_equals_literal(facility, "LOG_LOCAL0") || zend_string_equals_literal(facility, "local0")) {
		PG(syslog_facility) = LOG_LOCAL0;
		return SUCCESS;
	}
#endif
#ifdef LOG_LOCAL1
	if (zend_string_equals_literal(facility, "LOG_LOCAL1") || zend_string_equals_literal(facility, "local1")) {
		PG(syslog_facility) = LOG_LOCAL1;
		return SUCCESS;
	}
#endif
#ifdef LOG_LOCAL2
	if (zend_string_equals_literal(facility, "LOG_LOCAL2") || zend_string_equals_literal(facility, "local2")) {
		PG(syslog_facility) = LOG_LOCAL2;
		return SUCCESS;
	}
#endif
#ifdef LOG_LOCAL3
	if (zend_string_equals_literal(facility, "LOG_LOCAL3") || zend_string_equals_literal(facility, "local3")) {
		PG(syslog_facility) = LOG_LOCAL3;
		return SUCCESS;
	}
#endif
#ifdef LOG_LOCAL4
	if (zend_string_equals_literal(facility, "LOG_LOCAL4") || zend_string_equals_literal(facility, "local4")) {
		PG(syslog_facility) = LOG_LOCAL4;
		return SUCCESS;
	}
#endif
#ifdef LOG_LOCAL5
	if (zend_string_equals_literal(facility, "LOG_LOCAL5") || zend_string_equals_literal(facility, "local5")) {
		PG(syslog_facility) = LOG_LOCAL5;
		return SUCCESS;
	}
#endif
#ifdef LOG_LOCAL6
	if (zend_string_equals_literal(facility, "LOG_LOCAL6") || zend_string_equals_literal(facility, "local6")) {
		PG(syslog_facility) = LOG_LOCAL6;
		return SUCCESS;
	}
#endif
#ifdef LOG_LOCAL7
	if (zend_string_equals_literal(facility, "LOG_LOCAL7") || zend_string_equals_literal(facility, "local7")) {
		PG(syslog_facility) = LOG_LOCAL7;
		return SUCCESS;
	}
#endif

	return FAILURE;
}
/* }}} */

/* {{{ PHP_INI_MH */
static PHP_INI_MH(OnSetPrecision)
{
	zend_long i = ZEND_ATOL(ZSTR_VAL(new_value));
	if (i >= -1) {
		EG(precision) = i;
		return SUCCESS;
	} else {
		return FAILURE;
	}
}
/* }}} */

/* {{{ PHP_INI_MH */
static PHP_INI_MH(OnSetSerializePrecision)
{
	zend_long i = ZEND_ATOL(ZSTR_VAL(new_value));
	if (i >= -1) {
		PG(serialize_precision) = i;
		return SUCCESS;
	} else {
		return FAILURE;
	}
}
/* }}} */

/* {{{ PHP_INI_MH */
static PHP_INI_MH(OnChangeMemoryLimit)
{
	size_t value;
	if (new_value) {
		value = zend_ini_parse_uquantity_warn(new_value, entry->name);
	} else {
		value = Z_L(1)<<30;		/* effectively, no limit */
	}
	if (zend_set_memory_limit(value) == FAILURE) {
		/* When the memory limit is reset to the original level during deactivation, we may be
		 * using more memory than the original limit while shutdown is still in progress.
		 * Ignore a failure for now, and set the memory limit when the memory manager has been
		 * shut down and the minimal amount of memory is used. */
		if (stage != ZEND_INI_STAGE_DEACTIVATE) {
			zend_error(E_WARNING, "Failed to set memory limit to %zd bytes (Current memory usage is %zd bytes)", value, zend_memory_usage(true));
			return FAILURE;
		}
	}
	PG(memory_limit) = value;
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_INI_MH */
static PHP_INI_MH(OnSetLogFilter)
{
	const zend_string *filter = new_value;

	if (zend_string_equals_literal(filter, "all")) {
		PG(syslog_filter) = PHP_SYSLOG_FILTER_ALL;
		return SUCCESS;
	}
	if (zend_string_equals_literal(filter, "no-ctrl")) {
		PG(syslog_filter) = PHP_SYSLOG_FILTER_NO_CTRL;
		return SUCCESS;
	}
	if (zend_string_equals_literal(filter, "ascii")) {
		PG(syslog_filter) = PHP_SYSLOG_FILTER_ASCII;
		return SUCCESS;
	}
	if (zend_string_equals_literal(filter, "raw")) {
		PG(syslog_filter) = PHP_SYSLOG_FILTER_RAW;
		return SUCCESS;
	}

	return FAILURE;
}
/* }}} */

/* {{{ php_disable_classes */
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

/* {{{ php_binary_init */
static void php_binary_init(void)
{
	char *binary_location = NULL;
#ifdef PHP_WIN32
	binary_location = (char *)pemalloc(MAXPATHLEN, 1);
	if (GetModuleFileName(0, binary_location, MAXPATHLEN) == 0) {
		pefree(binary_location, 1);
		binary_location = NULL;
	}
#else
	if (sapi_module.executable_location) {
		binary_location = (char *)pemalloc(MAXPATHLEN, 1);
		if (!strchr(sapi_module.executable_location, '/')) {
			char *envpath, *path;
			bool found = false;

			if ((envpath = getenv("PATH")) != NULL) {
				char *search_dir, search_path[MAXPATHLEN];
				char *last = NULL;
				zend_stat_t s = {0};

				path = estrdup(envpath);
				search_dir = php_strtok_r(path, ":", &last);

				while (search_dir) {
					snprintf(search_path, MAXPATHLEN, "%s/%s", search_dir, sapi_module.executable_location);
					if (VCWD_REALPATH(search_path, binary_location) && !VCWD_ACCESS(binary_location, X_OK) && VCWD_STAT(binary_location, &s) == 0 && S_ISREG(s.st_mode)) {
						found = true;
						break;
					}
					search_dir = php_strtok_r(NULL, ":", &last);
				}
				efree(path);
			}
			if (!found) {
				pefree(binary_location, 1);
				binary_location = NULL;
			}
		} else if (!VCWD_REALPATH(sapi_module.executable_location, binary_location) || VCWD_ACCESS(binary_location, X_OK)) {
			pefree(binary_location, 1);
			binary_location = NULL;
		}
	}
#endif
	PG(php_binary) = binary_location;
}
/* }}} */

/* {{{ PHP_INI_MH */
static PHP_INI_MH(OnUpdateTimeout)
{
	if (stage==PHP_INI_STAGE_STARTUP) {
		/* Don't set a timeout on startup, only per-request */
		EG(timeout_seconds) = ZEND_ATOL(ZSTR_VAL(new_value));
		return SUCCESS;
	}
	zend_unset_timeout();
	EG(timeout_seconds) = ZEND_ATOL(ZSTR_VAL(new_value));
	if (stage != PHP_INI_STAGE_DEACTIVATE) {
		/*
		 * If we're restoring INI values, we shouldn't reset the timer.
		 * Otherwise, the timer is active when PHP is idle, such as the
		 * the CLI web server or CGI. Running a script will re-activate
		 * the timeout, so it's not needed to do so at script end.
		 */
		zend_set_timeout(EG(timeout_seconds), 0);
	}
	return SUCCESS;
}
/* }}} */

/* {{{ php_get_display_errors_mode() helper function */
static zend_uchar php_get_display_errors_mode(zend_string *value)
{
	if (!value) {
		return PHP_DISPLAY_ERRORS_STDOUT;
	}

	if (zend_string_equals_literal_ci(value, "on")) {
		return PHP_DISPLAY_ERRORS_STDOUT;
	}
	if (zend_string_equals_literal_ci(value, "yes")) {
		return PHP_DISPLAY_ERRORS_STDOUT;
	}

	if (zend_string_equals_literal_ci(value, "true")) {
		return PHP_DISPLAY_ERRORS_STDOUT;
	}
	if (zend_string_equals_literal_ci(value, "stderr")) {
		return PHP_DISPLAY_ERRORS_STDERR;
	}
	if (zend_string_equals_literal_ci(value, "stdout")) {
		return PHP_DISPLAY_ERRORS_STDOUT;
	}

	zend_uchar mode = ZEND_ATOL(ZSTR_VAL(value));
	if (mode && mode != PHP_DISPLAY_ERRORS_STDOUT && mode != PHP_DISPLAY_ERRORS_STDERR) {
		return PHP_DISPLAY_ERRORS_STDOUT;
	}

	return mode;
}
/* }}} */

/* {{{ PHP_INI_MH */
static PHP_INI_MH(OnUpdateDisplayErrors)
{
	PG(display_errors) = php_get_display_errors_mode(new_value);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_INI_DISP */
static PHP_INI_DISP(display_errors_mode)
{
	zend_uchar mode;
	bool cgi_or_cli;
	zend_string *temporary_value;

	if (type == ZEND_INI_DISPLAY_ORIG && ini_entry->modified) {
		temporary_value = (ini_entry->orig_value ? ini_entry->orig_value : NULL );
	} else if (ini_entry->value) {
		temporary_value = ini_entry->value;
	} else {
		temporary_value = NULL;
	}

	mode = php_get_display_errors_mode(temporary_value);

	/* Display 'On' for other SAPIs instead of STDOUT or STDERR */
	cgi_or_cli = (!strcmp(sapi_module.name, "cli") || !strcmp(sapi_module.name, "cgi") || !strcmp(sapi_module.name, "phpdbg"));

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

PHPAPI const char *php_get_internal_encoding(void) {
	if (PG(internal_encoding) && PG(internal_encoding)[0]) {
		return PG(internal_encoding);
	} else if (SG(default_charset) && SG(default_charset)[0]) {
		return SG(default_charset);
	}
	return "UTF-8";
}

PHPAPI const char *php_get_input_encoding(void) {
	if (PG(input_encoding) && PG(input_encoding)[0]) {
		return PG(input_encoding);
	} else if (SG(default_charset) && SG(default_charset)[0]) {
		return SG(default_charset);
	}
	return "UTF-8";
}

PHPAPI const char *php_get_output_encoding(void) {
	if (PG(output_encoding) && PG(output_encoding)[0]) {
		return PG(output_encoding);
	} else if (SG(default_charset) && SG(default_charset)[0]) {
		return SG(default_charset);
	}
	return "UTF-8";
}

PHPAPI void (*php_internal_encoding_changed)(void) = NULL;

/* {{{ PHP_INI_MH */
static PHP_INI_MH(OnUpdateDefaultCharset)
{
	if (memchr(ZSTR_VAL(new_value), '\0', ZSTR_LEN(new_value))
		|| strpbrk(ZSTR_VAL(new_value), "\r\n")) {
		return FAILURE;
	}
	OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
	if (php_internal_encoding_changed) {
		php_internal_encoding_changed();
	}
	if (new_value) {
#ifdef PHP_WIN32
		php_win32_cp_do_update(ZSTR_VAL(new_value));
#endif
	}
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_INI_MH */
static PHP_INI_MH(OnUpdateDefaultMimeTye)
{
	if (memchr(ZSTR_VAL(new_value), '\0', ZSTR_LEN(new_value))
		|| strpbrk(ZSTR_VAL(new_value), "\r\n")) {
		return FAILURE;
	}
	return OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
}
/* }}} */

/* {{{ PHP_INI_MH */
static PHP_INI_MH(OnUpdateInternalEncoding)
{
	OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
	if (php_internal_encoding_changed) {
		php_internal_encoding_changed();
	}
	if (new_value) {
#ifdef PHP_WIN32
		php_win32_cp_do_update(ZSTR_VAL(new_value));
#endif
	}
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_INI_MH */
static PHP_INI_MH(OnUpdateInputEncoding)
{
	OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
	if (php_internal_encoding_changed) {
		php_internal_encoding_changed();
	}
	if (new_value) {
#ifdef PHP_WIN32
		php_win32_cp_do_update(NULL);
#endif
	}
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_INI_MH */
static PHP_INI_MH(OnUpdateOutputEncoding)
{
	OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
	if (php_internal_encoding_changed) {
		php_internal_encoding_changed();
	}
	if (new_value) {
#ifdef PHP_WIN32
		php_win32_cp_do_update(NULL);
#endif
	}
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_INI_MH */
static PHP_INI_MH(OnUpdateErrorLog)
{
	/* Only do the safemode/open_basedir check at runtime */
	if ((stage == PHP_INI_STAGE_RUNTIME || stage == PHP_INI_STAGE_HTACCESS) &&
			new_value && zend_string_equals_literal(new_value, "syslog")) {
		if (PG(open_basedir) && php_check_open_basedir(ZSTR_VAL(new_value))) {
			return FAILURE;
		}
	}
	OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_INI_MH */
static PHP_INI_MH(OnUpdateMailLog)
{
	/* Only do the safemode/open_basedir check at runtime */
	if ((stage == PHP_INI_STAGE_RUNTIME || stage == PHP_INI_STAGE_HTACCESS) && new_value) {
		if (PG(open_basedir) && php_check_open_basedir(ZSTR_VAL(new_value))) {
			return FAILURE;
		}
	}
	OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_INI_MH */
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

 /* Windows use the internal mail */
#if defined(PHP_WIN32)
# define DEFAULT_SENDMAIL_PATH NULL
#else
# define DEFAULT_SENDMAIL_PATH PHP_PROG_SENDMAIL " -t -i"
#endif

/* {{{ PHP_INI */
PHP_INI_BEGIN()
	PHP_INI_ENTRY_EX("highlight.comment",		HL_COMMENT_COLOR,	PHP_INI_ALL,	NULL,			php_ini_color_displayer_cb)
	PHP_INI_ENTRY_EX("highlight.default",		HL_DEFAULT_COLOR,	PHP_INI_ALL,	NULL,			php_ini_color_displayer_cb)
	PHP_INI_ENTRY_EX("highlight.html",			HL_HTML_COLOR,		PHP_INI_ALL,	NULL,			php_ini_color_displayer_cb)
	PHP_INI_ENTRY_EX("highlight.keyword",		HL_KEYWORD_COLOR,	PHP_INI_ALL,	NULL,			php_ini_color_displayer_cb)
	PHP_INI_ENTRY_EX("highlight.string",		HL_STRING_COLOR,	PHP_INI_ALL,	NULL,			php_ini_color_displayer_cb)

	STD_PHP_INI_ENTRY_EX("display_errors",		"1",		PHP_INI_ALL,		OnUpdateDisplayErrors,	display_errors,			php_core_globals,	core_globals, display_errors_mode)
	STD_PHP_INI_BOOLEAN("display_startup_errors",	"1",	PHP_INI_ALL,		OnUpdateBool,			display_startup_errors,	php_core_globals,	core_globals)
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
	STD_PHP_INI_BOOLEAN("ignore_repeated_errors",	"0",	PHP_INI_ALL,		OnUpdateBool,			ignore_repeated_errors,	php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("ignore_repeated_source",	"0",	PHP_INI_ALL,		OnUpdateBool,			ignore_repeated_source,	php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("report_memleaks",		"1",		PHP_INI_ALL,		OnUpdateBool,			report_memleaks,		php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("report_zend_debug",	"0",		PHP_INI_ALL,		OnUpdateBool,			report_zend_debug,		php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("output_buffering",		"0",		PHP_INI_PERDIR|PHP_INI_SYSTEM,	OnUpdateLong,	output_buffering,		php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("output_handler",			NULL,		PHP_INI_PERDIR|PHP_INI_SYSTEM,	OnUpdateString,	output_handler,		php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("register_argc_argv",	"1",		PHP_INI_PERDIR|PHP_INI_SYSTEM,	OnUpdateBool,	register_argc_argv,		php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("auto_globals_jit",		"1",		PHP_INI_PERDIR|PHP_INI_SYSTEM,	OnUpdateBool,	auto_globals_jit,	php_core_globals,	core_globals)
	STD_PHP_INI_BOOLEAN("short_open_tag",	DEFAULT_SHORT_OPEN_TAG,	PHP_INI_SYSTEM|PHP_INI_PERDIR,		OnUpdateBool,			short_tags,				zend_compiler_globals,	compiler_globals)

	STD_PHP_INI_ENTRY("unserialize_callback_func",	NULL,	PHP_INI_ALL,		OnUpdateString,			unserialize_callback_func,	php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("serialize_precision",	"-1",	PHP_INI_ALL,		OnSetSerializePrecision,			serialize_precision,	php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("arg_separator.output",	"&",		PHP_INI_ALL,		OnUpdateStringUnempty,	arg_separator.output,	php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("arg_separator.input",	"&",		PHP_INI_SYSTEM|PHP_INI_PERDIR,	OnUpdateStringUnempty,	arg_separator.input,	php_core_globals,	core_globals)

	STD_PHP_INI_ENTRY("auto_append_file",		NULL,		PHP_INI_SYSTEM|PHP_INI_PERDIR,		OnUpdateString,			auto_append_file,		php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("auto_prepend_file",		NULL,		PHP_INI_SYSTEM|PHP_INI_PERDIR,		OnUpdateString,			auto_prepend_file,		php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("doc_root",				NULL,		PHP_INI_SYSTEM,		OnUpdateStringUnempty,	doc_root,				php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("default_charset",		PHP_DEFAULT_CHARSET,	PHP_INI_ALL,	OnUpdateDefaultCharset,			default_charset,		sapi_globals_struct, sapi_globals)
	STD_PHP_INI_ENTRY("default_mimetype",		SAPI_DEFAULT_MIMETYPE,	PHP_INI_ALL,	OnUpdateDefaultMimeTye,			default_mimetype,		sapi_globals_struct, sapi_globals)
	STD_PHP_INI_ENTRY("internal_encoding",		NULL,			PHP_INI_ALL,	OnUpdateInternalEncoding,	internal_encoding,	php_core_globals, core_globals)
	STD_PHP_INI_ENTRY("input_encoding",			NULL,			PHP_INI_ALL,	OnUpdateInputEncoding,				input_encoding,		php_core_globals, core_globals)
	STD_PHP_INI_ENTRY("output_encoding",		NULL,			PHP_INI_ALL,	OnUpdateOutputEncoding,				output_encoding,	php_core_globals, core_globals)
	STD_PHP_INI_ENTRY("error_log",				NULL,			PHP_INI_ALL,		OnUpdateErrorLog,				error_log,				php_core_globals,	core_globals)
	STD_PHP_INI_ENTRY("error_log_mode",			"0644",			PHP_INI_ALL,		OnUpdateLong,					error_log_mode,			php_core_globals,	core_globals)
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
	STD_PHP_INI_BOOLEAN("mail.mixed_lf_and_crlf",			"0",		PHP_INI_SYSTEM|PHP_INI_PERDIR,		OnUpdateBool,			mail_mixed_lf_and_crlf,			php_core_globals,	core_globals)
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
	PHP_INI_ENTRY("max_multipart_body_parts",	"-1",			PHP_INI_SYSTEM|PHP_INI_PERDIR,		NULL)

	STD_PHP_INI_BOOLEAN("allow_url_fopen",		"1",		PHP_INI_SYSTEM,		OnUpdateBool,		allow_url_fopen,		php_core_globals,		core_globals)
	STD_PHP_INI_BOOLEAN("allow_url_include",	"0",		PHP_INI_SYSTEM,		OnUpdateBool,		allow_url_include,		php_core_globals,		core_globals)
	STD_PHP_INI_BOOLEAN("enable_post_data_reading",	"1",	PHP_INI_SYSTEM|PHP_INI_PERDIR,	OnUpdateBool,	enable_post_data_reading,	php_core_globals,	core_globals)

	STD_PHP_INI_ENTRY("realpath_cache_size",	"4096K",	PHP_INI_SYSTEM,		OnUpdateLong,	realpath_cache_size_limit,	virtual_cwd_globals,	cwd_globals)
	STD_PHP_INI_ENTRY("realpath_cache_ttl",		"120",		PHP_INI_SYSTEM,		OnUpdateLong,	realpath_cache_ttl,			virtual_cwd_globals,	cwd_globals)

	STD_PHP_INI_ENTRY("user_ini.filename",		".user.ini",	PHP_INI_SYSTEM,		OnUpdateString,		user_ini_filename,	php_core_globals,		core_globals)
	STD_PHP_INI_ENTRY("user_ini.cache_ttl",		"300",			PHP_INI_SYSTEM,		OnUpdateLong,		user_ini_cache_ttl,	php_core_globals,		core_globals)
	STD_PHP_INI_ENTRY("hard_timeout",			"2",			PHP_INI_SYSTEM,		OnUpdateLong,		hard_timeout,		zend_executor_globals,	executor_globals)
#ifdef PHP_WIN32
	STD_PHP_INI_BOOLEAN("windows.show_crt_warning",		"0",		PHP_INI_ALL,		OnUpdateBool,			windows_show_crt_warning,			php_core_globals,	core_globals)
#endif
	STD_PHP_INI_ENTRY("syslog.facility",		"LOG_USER",		PHP_INI_SYSTEM,		OnSetFacility,		syslog_facility,	php_core_globals,		core_globals)
	STD_PHP_INI_ENTRY("syslog.ident",		"php",			PHP_INI_SYSTEM,		OnUpdateString,		syslog_ident,		php_core_globals,		core_globals)
	STD_PHP_INI_ENTRY("syslog.filter",		"no-ctrl",		PHP_INI_ALL,		OnSetLogFilter,		syslog_filter,		php_core_globals, 		core_globals)
PHP_INI_END()
/* }}} */

/* True globals (no need for thread safety */
/* But don't make them a single int bitfield */
static bool module_initialized = false;
static bool module_startup = true;
static bool module_shutdown = false;

/* {{{ php_during_module_startup */
PHPAPI bool php_during_module_startup(void)
{
	return module_startup;
}
/* }}} */

/* {{{ php_during_module_shutdown */
PHPAPI bool php_during_module_shutdown(void)
{
	return module_shutdown;
}
/* }}} */

/* {{{ php_get_module_initialized */
PHPAPI bool php_get_module_initialized(void)
{
	return module_initialized;
}
/* }}} */

/* {{{ php_log_err_with_severity */
PHPAPI ZEND_COLD void php_log_err_with_severity(const char *log_message, int syslog_type_int)
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
		int error_log_mode;

#ifdef HAVE_SYSLOG_H
		if (!strcmp(PG(error_log), "syslog")) {
			php_syslog(syslog_type_int, "%s", log_message);
			PG(in_error_log) = 0;
			return;
		}
#endif

		error_log_mode = 0644;

		if (PG(error_log_mode) > 0 && PG(error_log_mode) <= 0777) {
			error_log_mode = PG(error_log_mode);
		}

		fd = VCWD_OPEN_MODE(PG(error_log), O_CREAT | O_APPEND | O_WRONLY, error_log_mode);
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
			len = spprintf(&tmp, 0, "[%s] %s%s", ZSTR_VAL(error_time_str), log_message, PHP_EOL);
#ifdef PHP_WIN32
			php_flock(fd, LOCK_EX);
			/* XXX should eventually write in a loop if len > UINT_MAX */
			php_ignore_value(write(fd, tmp, (unsigned)len));
			php_flock(fd, LOCK_UN);
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
		sapi_module.log_message(log_message, syslog_type_int);
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

/* {{{ php_printf */
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

/* {{{ php_printf_unchecked */
PHPAPI size_t php_printf_unchecked(const char *format, ...)
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

static zend_string *escape_html(const char *buffer, size_t buffer_len) {
	zend_string *result = php_escape_html_entities_ex(
		(const unsigned char *) buffer, buffer_len, 0, ENT_COMPAT,
		/* charset_hint */ NULL, /* double_encode */ 1, /* quiet */ 1);
	if (!result || ZSTR_LEN(result) == 0) {
		/* Retry with substituting invalid chars on fail. */
		result = php_escape_html_entities_ex(
			(const unsigned char *) buffer, buffer_len, 0, ENT_COMPAT | ENT_HTML_SUBSTITUTE_ERRORS,
			/* charset_hint */ NULL, /* double_encode */ 1, /* quiet */ 1);
	}
	return result;
}

/* {{{ php_verror */
/* php_verror is called from php_error_docref<n> functions.
 * Its purpose is to unify error messages and automatically generate clickable
 * html error messages if corresponding ini setting (html_errors) is activated.
 * See: CODING_STANDARDS.md for details.
 */
PHPAPI ZEND_COLD void php_verror(const char *docref, const char *params, int type, const char *format, va_list args)
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
	zend_string *message;
	int is_function = 0;

	/* get error text into buffer and escape for html if necessary */
	buffer_len = (int)vspprintf(&buffer, 0, format, args);

	if (PG(html_errors)) {
		replace_buffer = escape_html(buffer, buffer_len);
		efree(buffer);

		if (replace_buffer) {
			buffer = ZSTR_VAL(replace_buffer);
			buffer_len = (int)ZSTR_LEN(replace_buffer);
		} else {
			buffer = "";
			buffer_len = 0;
		}
	}

	/* which function caused the problem if any at all */
	if (php_during_module_startup()) {
		function = "PHP Startup";
	} else if (php_during_module_shutdown()) {
		function = "PHP Shutdown";
	} else if (PG(during_request_startup)) {
		function = "PHP Request Startup";
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
	} else if ((function = get_active_function_name()) && strlen(function)) {
		is_function = 1;
		class_name = get_active_class_name(&space);
	} else if (EG(flags) & EG_FLAGS_IN_SHUTDOWN) {
		function = "PHP Request Shutdown";
	} else {
		function = "Unknown";
	}

	/* if we still have memory then format the origin */
	if (is_function) {
		origin_len = (int)spprintf(&origin, 0, "%s%s%s(%s)", class_name, space, function, params);
	} else {
		origin_len = (int)spprintf(&origin, 0, "%s", function);
	}

	if (PG(html_errors)) {
		replace_origin = escape_html(origin, origin_len);
		efree(origin);
		origin = ZSTR_VAL(replace_origin);
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
		zend_str_tolower(docref_buf, doclen);
		docref = docref_buf;
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
			message = zend_strpprintf(0, "%s [<a href='%s%s%s'>%s</a>]: %s", origin, docref_root, docref, docref_target, docref, buffer);
		} else {
			message = zend_strpprintf(0, "%s [%s%s%s]: %s", origin, docref_root, docref, docref_target, buffer);
		}
		if (target) {
			efree(target);
		}
	} else {
		message = zend_strpprintf(0, "%s: %s", origin, buffer);
	}
	if (replace_origin) {
		zend_string_free(replace_origin);
	} else {
		efree(origin);
	}
	if (docref_buf) {
		efree(docref_buf);
	}

	if (replace_buffer) {
		zend_string_free(replace_buffer);
	} else {
		efree(buffer);
	}

	zend_error_zstr(type, message);
	zend_string_release(message);
}
/* }}} */

/* {{{ php_error_docref */
/* Generate an error which links to docref or the php.net documentation if docref is NULL */
PHPAPI ZEND_COLD void php_error_docref(const char *docref, int type, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	php_verror(docref, "", type, format, args);
	va_end(args);
}
/* }}} */

/* {{{ php_error_docref1 */
/* See: CODING_STANDARDS.md for details. */
PHPAPI ZEND_COLD void php_error_docref1(const char *docref, const char *param1, int type, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	php_verror(docref, param1, type, format, args);
	va_end(args);
}
/* }}} */

/* {{{ php_error_docref2 */
/* See: CODING_STANDARDS.md for details. */
PHPAPI ZEND_COLD void php_error_docref2(const char *docref, const char *param1, const char *param2, int type, const char *format, ...)
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
PHPAPI ZEND_COLD void php_win32_docref1_from_error(DWORD error, const char *param1) {
	char *buf = php_win32_error_to_msg(error);
	size_t buf_len;

	buf_len = strlen(buf);
	if (buf_len >= 2) {
		buf[buf_len - 1] = '\0';
		buf[buf_len - 2] = '\0';
	}
	php_error_docref1(NULL, param1, E_WARNING, "%s (code: %lu)", buf, error);
	php_win32_error_msg_free(buf);
}

PHPAPI ZEND_COLD void php_win32_docref2_from_error(DWORD error, const char *param1, const char *param2) {
	char *buf = php_win32_error_to_msg(error);
	php_error_docref2(NULL, param1, param2, E_WARNING, "%s (code: %lu)", buf, error);
	php_win32_error_msg_free(buf);
}
#endif

/* {{{ php_html_puts */
PHPAPI void php_html_puts(const char *str, size_t size)
{
	zend_html_puts(str, size);
}
/* }}} */

static void clear_last_error(void) {
	if (PG(last_error_message)) {
		zend_string_release(PG(last_error_message));
		PG(last_error_message) = NULL;
	}
	if (PG(last_error_file)) {
		zend_string_release(PG(last_error_file));
		PG(last_error_file) = NULL;
	}
}

#if ZEND_DEBUG
/* {{{ report_zend_debug_error_notify_cb */
static void report_zend_debug_error_notify_cb(int type, zend_string *error_filename, uint32_t error_lineno, zend_string *message)
{
	if (PG(report_zend_debug)) {
		bool trigger_break;

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

		zend_output_debug_string(trigger_break, "%s(%" PRIu32 ") : %s", ZSTR_VAL(error_filename), error_lineno, ZSTR_VAL(message));
	}
}
/* }}} */
#endif

/* {{{ php_error_cb
 extended error handling function */
static ZEND_COLD void php_error_cb(int orig_type, zend_string *error_filename, const uint32_t error_lineno, zend_string *message)
{
	bool display;
	int type = orig_type & E_ALL;

	/* check for repeated errors to be ignored */
	if (PG(ignore_repeated_errors) && PG(last_error_message)) {
		/* no check for PG(last_error_file) is needed since it cannot
		 * be NULL if PG(last_error_message) is not NULL */
		if (!zend_string_equals(PG(last_error_message), message)
			|| (!PG(ignore_repeated_source)
				&& ((PG(last_error_lineno) != (int)error_lineno)
					|| !zend_string_equals(PG(last_error_file), error_filename)))) {
			display = 1;
		} else {
			display = 0;
		}
	} else {
		display = 1;
	}

	/* according to error handling mode, throw exception or show it */
	if (EG(error_handling) == EH_THROW) {
		switch (type) {
			case E_WARNING:
			case E_CORE_WARNING:
			case E_COMPILE_WARNING:
			case E_USER_WARNING:
				/* throw an exception if we are in EH_THROW mode and the type is warning.
				 * fatal errors are real errors and cannot be made exceptions.
				 * exclude deprecated for the sake of BC to old damaged code.
				 * notices are no errors and are not treated as such like E_WARNINGS.
				 * DO NOT overwrite a pending exception.
				 */
				if (!EG(exception)) {
					zend_throw_error_exception(EG(exception_class), message, 0, type);
				}
				return;
			default:
				break;
		}
	}

	/* store the error if it has changed */
	if (display) {
		clear_last_error();
		if (!error_filename) {
			error_filename = ZSTR_KNOWN(ZEND_STR_UNKNOWN_CAPITALIZED);
		}
		PG(last_error_type) = type;
		PG(last_error_message) = zend_string_copy(message);
		PG(last_error_file) = zend_string_copy(error_filename);
		PG(last_error_lineno) = error_lineno;
	}

	if (zend_alloc_in_memory_limit_error_reporting()) {
		php_output_discard_all();
	}

	/* display/log the error if necessary */
	if (display && ((EG(error_reporting) & type) || (type & E_CORE))
		&& (PG(log_errors) || PG(display_errors) || (!module_initialized))) {
		char *error_type_str;
		int syslog_type_int = LOG_NOTICE;

		switch (type) {
			case E_ERROR:
			case E_CORE_ERROR:
			case E_COMPILE_ERROR:
			case E_USER_ERROR:
				error_type_str = "Fatal error";
				syslog_type_int = LOG_ERR;
				break;
			case E_RECOVERABLE_ERROR:
				error_type_str = "Recoverable fatal error";
				syslog_type_int = LOG_ERR;
				break;
			case E_WARNING:
			case E_CORE_WARNING:
			case E_COMPILE_WARNING:
			case E_USER_WARNING:
				error_type_str = "Warning";
				syslog_type_int = LOG_WARNING;
				break;
			case E_PARSE:
				error_type_str = "Parse error";
				syslog_type_int = LOG_ERR;
				break;
			case E_NOTICE:
			case E_USER_NOTICE:
				error_type_str = "Notice";
				syslog_type_int = LOG_NOTICE;
				break;
			case E_STRICT:
				error_type_str = "Strict Standards";
				syslog_type_int = LOG_INFO;
				break;
			case E_DEPRECATED:
			case E_USER_DEPRECATED:
				error_type_str = "Deprecated";
				syslog_type_int = LOG_INFO;
				break;
			default:
				error_type_str = "Unknown error";
				break;
		}

		if (PG(log_errors)
				|| (!module_initialized && (!PG(display_startup_errors) || !PG(display_errors)))) {
			char *log_buffer;
#ifdef PHP_WIN32
			if (type == E_CORE_ERROR || type == E_CORE_WARNING) {
				syslog(LOG_ALERT, "PHP %s: %s (%s)", error_type_str, ZSTR_VAL(message), GetCommandLine());
			}
#endif
			spprintf(&log_buffer, 0, "PHP %s:  %s in %s on line %" PRIu32, error_type_str, ZSTR_VAL(message), ZSTR_VAL(error_filename), error_lineno);
			php_log_err_with_severity(log_buffer, syslog_type_int);
			efree(log_buffer);
		}

		if (PG(display_errors) && ((module_initialized && !PG(during_request_startup)) || (PG(display_startup_errors)))) {
			if (PG(xmlrpc_errors)) {
				php_printf("<?xml version=\"1.0\"?><methodResponse><fault><value><struct><member><name>faultCode</name><value><int>" ZEND_LONG_FMT "</int></value></member><member><name>faultString</name><value><string>%s:%s in %s on line %" PRIu32 "</string></value></member></struct></value></fault></methodResponse>", PG(xmlrpc_error_number), error_type_str, ZSTR_VAL(message), ZSTR_VAL(error_filename), error_lineno);
			} else {
				char *prepend_string = INI_STR("error_prepend_string");
				char *append_string = INI_STR("error_append_string");

				if (PG(html_errors)) {
					if (type == E_ERROR || type == E_PARSE) {
						zend_string *buf = escape_html(ZSTR_VAL(message), ZSTR_LEN(message));
						php_printf("%s<br />\n<b>%s</b>:  %s in <b>%s</b> on line <b>%" PRIu32 "</b><br />\n%s", STR_PRINT(prepend_string), error_type_str, ZSTR_VAL(buf), ZSTR_VAL(error_filename), error_lineno, STR_PRINT(append_string));
						zend_string_free(buf);
					} else {
						php_printf("%s<br />\n<b>%s</b>:  %s in <b>%s</b> on line <b>%" PRIu32 "</b><br />\n%s", STR_PRINT(prepend_string), error_type_str, ZSTR_VAL(message), ZSTR_VAL(error_filename), error_lineno, STR_PRINT(append_string));
					}
				} else {
					/* Write CLI/CGI errors to stderr if display_errors = "stderr" */
					if ((!strcmp(sapi_module.name, "cli") || !strcmp(sapi_module.name, "cgi") || !strcmp(sapi_module.name, "phpdbg")) &&
						PG(display_errors) == PHP_DISPLAY_ERRORS_STDERR
					) {
						fprintf(stderr, "%s: %s in %s on line %" PRIu32 "\n", error_type_str, ZSTR_VAL(message), ZSTR_VAL(error_filename), error_lineno);
#ifdef PHP_WIN32
						fflush(stderr);
#endif
					} else {
						php_printf("%s\n%s: %s in %s on line %" PRIu32 "\n%s", STR_PRINT(prepend_string), error_type_str, ZSTR_VAL(message), ZSTR_VAL(error_filename), error_lineno, STR_PRINT(append_string));
					}
				}
			}
		}
	}

	/* Bail out if we can't recover */
	switch (type) {
		case E_CORE_ERROR:
			if(!module_initialized) {
				/* bad error in module startup - no way we can live with this */
				exit(-2);
			}
		ZEND_FALLTHROUGH;
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
				if (!(orig_type & E_DONT_BAIL)) {
					/* restore memory limit */
					zend_set_memory_limit(PG(memory_limit));
					zend_objects_store_mark_destructed(&EG(objects_store));
					zend_bailout();
					return;
				}
			}
			break;
	}
}
/* }}} */

/* {{{ php_get_current_user */
PHPAPI char *php_get_current_user(void)
{
	zend_stat_t *pstat = NULL;

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
		char *name = php_win32_get_username();
		int len;

		if (!name) {
			return "";
		}
		len = (int)strlen(name);
		name[len] = '\0';
		SG(request_info).current_user_length = len;
		SG(request_info).current_user = estrndup(name, len);
		free(name);
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

/* {{{ Sets the maximum time a script can run */
PHP_FUNCTION(set_time_limit)
{
	zend_long new_timeout;
	char *new_timeout_str;
	size_t new_timeout_strlen;
	zend_string *key;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &new_timeout) == FAILURE) {
		RETURN_THROWS();
	}

	new_timeout_strlen = zend_spprintf(&new_timeout_str, 0, ZEND_LONG_FMT, new_timeout);

	key = zend_string_init("max_execution_time", sizeof("max_execution_time")-1, 0);
	if (zend_alter_ini_entry_chars_ex(key, new_timeout_str, new_timeout_strlen, PHP_INI_USER, PHP_INI_STAGE_RUNTIME, 0) == SUCCESS) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}
	zend_string_release_ex(key, 0);
	efree(new_timeout_str);
}
/* }}} */

/* {{{ php_fopen_wrapper_for_zend */
static FILE *php_fopen_wrapper_for_zend(zend_string *filename, zend_string **opened_path)
{
	*opened_path = filename;
	return php_stream_open_wrapper_as_file(ZSTR_VAL(filename), "rb", USE_PATH|REPORT_ERRORS|STREAM_OPEN_FOR_INCLUDE|STREAM_OPEN_FOR_ZEND_STREAM, opened_path);
}
/* }}} */

static void php_zend_stream_closer(void *handle) /* {{{ */
{
	php_stream_close((php_stream*)handle);
}
/* }}} */

static size_t php_zend_stream_fsizer(void *handle) /* {{{ */
{
	php_stream *stream = handle;
	php_stream_statbuf ssb;

	/* File size reported by stat() may be inaccurate if stream filters are used.
	 * TODO: Should stat() be generally disabled if filters are used? */
	if (stream->readfilters.head) {
		return 0;
	}

	if (php_stream_stat(stream, &ssb) == 0) {
		return ssb.sb.st_size;
	}
	return 0;
}
/* }}} */

static zend_result php_stream_open_for_zend(zend_file_handle *handle) /* {{{ */
{
	return php_stream_open_for_zend_ex(handle, USE_PATH|REPORT_ERRORS|STREAM_OPEN_FOR_INCLUDE);
}
/* }}} */

PHPAPI zend_result php_stream_open_for_zend_ex(zend_file_handle *handle, int mode) /* {{{ */
{
	zend_string *opened_path;
	zend_string *filename;
	php_stream *stream;

	ZEND_ASSERT(handle->type == ZEND_HANDLE_FILENAME);
	opened_path = filename = handle->filename;
	stream = php_stream_open_wrapper((char *)ZSTR_VAL(filename), "rb", mode | STREAM_OPEN_FOR_ZEND_STREAM, &opened_path);
	if (stream) {
		memset(handle, 0, sizeof(zend_file_handle));
		handle->type = ZEND_HANDLE_STREAM;
		handle->filename = filename;
		handle->opened_path = opened_path;
		handle->handle.stream.handle  = stream;
		handle->handle.stream.reader  = (zend_stream_reader_t)_php_stream_read;
		handle->handle.stream.fsizer  = php_zend_stream_fsizer;
		handle->handle.stream.isatty  = 0;
		handle->handle.stream.closer = php_zend_stream_closer;
		/* suppress warning if this stream is not explicitly closed */
		php_stream_auto_cleanup(stream);
		/* Disable buffering to avoid double buffering between PHP and Zend streams. */
		php_stream_set_option(stream, PHP_STREAM_OPTION_READ_BUFFER, PHP_STREAM_BUFFER_NONE, NULL);

		return SUCCESS;
	}
	return FAILURE;
}
/* }}} */

static zend_string *php_resolve_path_for_zend(zend_string *filename) /* {{{ */
{
	return php_resolve_path(ZSTR_VAL(filename), ZSTR_LEN(filename), PG(include_path));
}
/* }}} */

/* {{{ php_get_configuration_directive_for_zend */
static zval *php_get_configuration_directive_for_zend(zend_string *name)
{
	return cfg_get_entry_ex(name);
}
/* }}} */

/* {{{ php_free_request_globals */
static void php_free_request_globals(void)
{
	clear_last_error();
	if (PG(php_sys_temp_dir)) {
		efree(PG(php_sys_temp_dir));
		PG(php_sys_temp_dir) = NULL;
	}

	EG(filename_override) = NULL;
	EG(lineno_override) = -1;
}
/* }}} */

/* {{{ php_message_handler_for_zend */
static ZEND_COLD void php_message_handler_for_zend(zend_long message, const void *data)
{
	switch (message) {
		case ZMSG_FAILED_INCLUDE_FOPEN: {
			char *tmp = estrdup((char *) data);
			php_error_docref("function.include", E_WARNING, "Failed opening '%s' for inclusion (include_path='%s')", php_strip_url_passwd(tmp), STR_PRINT(PG(include_path)));
			efree(tmp);
			break;
		}
		case ZMSG_FAILED_REQUIRE_FOPEN: {
			char *tmp = estrdup((char *) data);
			zend_throw_error(NULL, "Failed opening required '%s' (include_path='%s')", php_strip_url_passwd(tmp), STR_PRINT(PG(include_path)));
			efree(tmp);
			break;
		}
		case ZMSG_FAILED_HIGHLIGHT_FOPEN: {
			char *tmp = estrdup((char *) data);
			php_error_docref(NULL, E_WARNING, "Failed opening '%s' for highlighting", php_strip_url_passwd(tmp));
			efree(tmp);
			break;
		}
		case ZMSG_MEMORY_LEAK_DETECTED:
		case ZMSG_MEMORY_LEAK_REPEATED:
#if ZEND_DEBUG
			if (EG(error_reporting) & E_WARNING) {
				char memory_leak_buf[1024];

				if (message==ZMSG_MEMORY_LEAK_DETECTED) {
					zend_leak_info *t = (zend_leak_info *) data;

					snprintf(memory_leak_buf, 512, "%s(%" PRIu32 ") :  Freeing " ZEND_ADDR_FMT " (%zu bytes), script=%s\n", t->filename, t->lineno, (size_t)t->addr, t->size, SAFE_FILENAME(SG(request_info).path_translated));
					if (t->orig_filename) {
						char relay_buf[512];

						snprintf(relay_buf, 512, "%s(%" PRIu32 ") : Actual location (location was relayed)\n", t->orig_filename, t->orig_lineno);
						strlcat(memory_leak_buf, relay_buf, sizeof(memory_leak_buf));
					}
				} else {
					unsigned long leak_count = (zend_uintptr_t) data;

					snprintf(memory_leak_buf, 512, "Last leak repeated %lu time%s\n", leak_count, (leak_count>1?"s":""));
				}
#	if defined(PHP_WIN32)
				if (IsDebuggerPresent()) {
					OutputDebugString(memory_leak_buf);
				} else {
					fprintf(stderr, "%s", memory_leak_buf);
				}
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
				if (IsDebuggerPresent()) {
					OutputDebugString(memory_leak_buf);
				} else {
					fprintf(stderr, "%s", memory_leak_buf);
				}
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
				if (IsDebuggerPresent()) {
					OutputDebugString(memory_leak_buf);
				} else {
					fprintf(stderr, "%s", memory_leak_buf);
				}
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
}

#if PHP_SIGCHILD
/* {{{ sigchld_handler */
static void sigchld_handler(int apar)
{
	int errno_save = errno;

	while (waitpid(-1, NULL, WNOHANG) > 0);
	signal(SIGCHLD, sigchld_handler);

	errno = errno_save;
}
/* }}} */
#endif

/* {{{ php_request_startup */
zend_result php_request_startup(void)
{
	zend_result retval = SUCCESS;

	zend_interned_strings_activate();

#ifdef HAVE_DTRACE
	DTRACE_REQUEST_STARTUP(SAFE_FILENAME(SG(request_info).path_translated), SAFE_FILENAME(SG(request_info).request_uri), (char *)SAFE_FILENAME(SG(request_info).request_method));
#endif /* HAVE_DTRACE */

#ifdef PHP_WIN32
# if defined(ZTS)
	_configthreadlocale(_ENABLE_PER_THREAD_LOCALE);
# endif
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

		if (PG(expose_php) && !SG(headers_sent)) {
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
/* }}} */

/* {{{ php_request_shutdown */
void php_request_shutdown(void *dummy)
{
	bool report_memleaks;

	EG(flags) |= EG_FLAGS_IN_SHUTDOWN;

	report_memleaks = PG(report_memleaks);

	/* EG(current_execute_data) points into nirvana and therefore cannot be safely accessed
	 * inside zend_executor callback functions.
	 */
	EG(current_execute_data) = NULL;

	php_deactivate_ticks();

	/* 0. Call any open observer end handlers that are still open after a zend_bailout */
	if (ZEND_OBSERVER_ENABLED) {
		zend_observer_fcall_end_all();
	}

	/* 1. Call all possible shutdown functions registered with register_shutdown_function() */
	if (PG(modules_activated)) {
		php_call_shutdown_functions();
	}

	/* 2. Call all possible __destruct() functions */
	zend_try {
		zend_call_destructors();
	} zend_end_try();

	/* 3. Flush all output buffers */
	zend_try {
		php_output_end_all();
	} zend_end_try();

	/* 4. Reset max_execution_time (no longer executing php code after response sent) */
	zend_try {
		zend_unset_timeout();
	} zend_end_try();

	/* 5. Call all extensions RSHUTDOWN functions */
	if (PG(modules_activated)) {
		zend_deactivate_modules();
	}

	/* 6. Shutdown output layer (send the set HTTP headers, cleanup output handlers, etc.) */
	zend_try {
		php_output_deactivate();
	} zend_end_try();

	/* 7. Free shutdown functions */
	if (PG(modules_activated)) {
		php_free_shutdown_functions();
	}

	/* 8. Destroy super-globals */
	zend_try {
		int i;

		for (i=0; i<NUM_TRACK_VARS; i++) {
			zval_ptr_dtor(&PG(http_globals)[i]);
		}
	} zend_end_try();

	/* 9. Shutdown scanner/executor/compiler and restore ini entries */
	zend_deactivate();

	/* 10. free request-bound globals */
	php_free_request_globals();

	/* 11. Call all extensions post-RSHUTDOWN functions */
	zend_try {
		zend_post_deactivate_modules();
	} zend_end_try();

	/* 12. SAPI related shutdown*/
	zend_try {
		sapi_deactivate_module();
	} zend_end_try();
	/* free SAPI stuff */
	sapi_deactivate_destroy();

	/* 13. free virtual CWD memory */
	virtual_cwd_deactivate();

	/* 14. Destroy stream hashes */
	zend_try {
		php_shutdown_stream_hashes();
	} zend_end_try();

	/* 15. Free Willy (here be crashes) */
	zend_arena_destroy(CG(arena));
	zend_interned_strings_deactivate();
	zend_try {
		shutdown_memory_manager(CG(unclean_shutdown) || !report_memleaks, 0);
	} zend_end_try();

	/* Reset memory limit, as the reset during INI_STAGE_DEACTIVATE may have failed.
	 * At this point, no memory beyond a single chunk should be in use. */
	zend_set_memory_limit(PG(memory_limit));

	/* 16. Deactivate Zend signals */
#ifdef ZEND_SIGNALS
	zend_signal_deactivate();
#endif

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

/* {{{ php_com_initialize */
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

#ifdef ZTS
/* {{{ core_globals_ctor */
static void core_globals_ctor(php_core_globals *core_globals)
{
	memset(core_globals, 0, sizeof(*core_globals));
	php_startup_ticks();
}
/* }}} */
#endif

/* {{{ core_globals_dtor */
static void core_globals_dtor(php_core_globals *core_globals)
{
	/* These should have been freed earlier. */
	ZEND_ASSERT(!core_globals->last_error_message);
	ZEND_ASSERT(!core_globals->last_error_file);

	if (core_globals->disable_classes) {
		free(core_globals->disable_classes);
	}
	if (core_globals->php_binary) {
		free(core_globals->php_binary);
	}

	php_shutdown_ticks(core_globals);
}
/* }}} */

PHP_MINFO_FUNCTION(php_core) { /* {{{ */
	php_info_print_table_start();
	php_info_print_table_row(2, "PHP Version", PHP_VERSION);
	php_info_print_table_end();
	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ php_register_extensions */
zend_result php_register_extensions(zend_module_entry * const * ptr, int count)
{
	zend_module_entry * const * end = ptr + count;

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

#ifdef PHP_WIN32
static _invalid_parameter_handler old_invalid_parameter_handler;

void dummy_invalid_parameter_handler(
		const wchar_t *expression,
		const wchar_t *function,
		const wchar_t *file,
		unsigned int   line,
		uintptr_t      pReserved)
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

/* {{{ php_module_startup */
zend_result php_module_startup(sapi_module_struct *sf, zend_module_entry *additional_module)
{
	zend_utility_functions zuf;
	zend_utility_values zuv;
	zend_result retval = SUCCESS;
	int module_number = 0;
	char *php_os;
	zend_module_entry *module;

#ifdef PHP_WIN32
	WORD wVersionRequested = MAKEWORD(2, 0);
	WSADATA wsaData;

	php_os = "WINNT";

	old_invalid_parameter_handler =
		_set_invalid_parameter_handler(dummy_invalid_parameter_handler);
	if (old_invalid_parameter_handler != NULL) {
		_set_invalid_parameter_handler(old_invalid_parameter_handler);
	}

	/* Disable the message box for assertions.*/
	_CrtSetReportMode(_CRT_ASSERT, 0);
#else
	php_os = PHP_OS;
#endif

#ifdef ZTS
	(void)ts_resource(0);
#endif

#ifdef PHP_WIN32
	if (!php_win32_init_random_bytes()) {
		fprintf(stderr, "\ncrypt algorithm provider initialization failed\n");
		return FAILURE;
	}
#endif

	module_shutdown = false;
	module_startup = true;
	sapi_initialize_empty_request();
	sapi_activate();

	if (module_initialized) {
		return SUCCESS;
	}

	sapi_module = *sf;

	php_output_startup();

#ifdef ZTS
	ts_allocate_fast_id(&core_globals_id, &core_globals_offset, sizeof(php_core_globals), (ts_allocate_ctor) core_globals_ctor, (ts_allocate_dtor) core_globals_dtor);
#ifdef PHP_WIN32
	ts_allocate_id(&php_win32_core_globals_id, sizeof(php_win32_core_globals), (ts_allocate_ctor) php_win32_core_globals_ctor, (ts_allocate_dtor) php_win32_core_globals_dtor);
#endif
#else
	memset(&core_globals, 0, sizeof(core_globals));
	php_startup_ticks();
#endif
	gc_globals_ctor();

	zuf.error_function = php_error_cb;
	zuf.printf_function = php_printf;
	zuf.write_function = php_output_write;
	zuf.fopen_function = php_fopen_wrapper_for_zend;
	zuf.message_handler = php_message_handler_for_zend;
	zuf.get_configuration_directive = php_get_configuration_directive_for_zend;
	zuf.ticks_function = php_run_ticks;
	zuf.on_timeout = php_on_timeout;
	zuf.stream_open_function = php_stream_open_for_zend;
	zuf.printf_to_smart_string_function = php_printf_to_smart_string;
	zuf.printf_to_smart_str_function = php_printf_to_smart_str;
	zuf.getenv_function = sapi_getenv;
	zuf.resolve_path_function = php_resolve_path_for_zend;
	zend_startup(&zuf);
	zend_reset_lc_ctype_locale();
	zend_update_current_locale();

	zend_observer_startup();
#if ZEND_DEBUG
	zend_observer_error_register(report_zend_debug_error_notify_cb);
#endif

#if HAVE_TZSET
	tzset();
#endif

#ifdef PHP_WIN32
	char *img_err;
	if (!php_win32_crt_compatible(&img_err)) {
		php_error(E_CORE_WARNING, img_err);
		efree(img_err);
		return FAILURE;
	}

	/* start up winsock services */
	if (WSAStartup(wVersionRequested, &wsaData) != 0) {
		php_printf("\nwinsock.dll unusable. %d\n", WSAGetLastError());
		return FAILURE;
	}
	php_win32_signal_ctrl_handler_init();
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
	REGISTER_MAIN_STRINGL_CONSTANT("PHP_OS_FAMILY", PHP_OS_FAMILY, sizeof(PHP_OS_FAMILY)-1, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_STRINGL_CONSTANT("PHP_SAPI", sapi_module.name, strlen(sapi_module.name), CONST_PERSISTENT | CONST_CS | CONST_NO_FILE_CACHE);
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
	REGISTER_MAIN_LONG_CONSTANT("PHP_FD_SETSIZE", FD_SETSIZE, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("PHP_FLOAT_DIG", DBL_DIG, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_DOUBLE_CONSTANT("PHP_FLOAT_EPSILON", DBL_EPSILON, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_DOUBLE_CONSTANT("PHP_FLOAT_MAX", DBL_MAX, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_DOUBLE_CONSTANT("PHP_FLOAT_MIN", DBL_MIN, CONST_PERSISTENT | CONST_CS);

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
		REGISTER_MAIN_STRINGL_CONSTANT("PHP_BINARY", PG(php_binary), strlen(PG(php_binary)), CONST_PERSISTENT | CONST_CS | CONST_NO_FILE_CACHE);
	} else {
		REGISTER_MAIN_STRINGL_CONSTANT("PHP_BINARY", "", 0, CONST_PERSISTENT | CONST_CS | CONST_NO_FILE_CACHE);
	}

	php_output_register_constants();
	php_rfc1867_register_constants();

	/* this will read in php.ini, set up the configuration parameters,
	   load zend extensions and register php function extensions
	   to be loaded later */
	zend_stream_init();
	if (php_init_config() == FAILURE) {
		return FAILURE;
	}
	zend_stream_shutdown();

	/* Register PHP core ini entries */
	zend_register_ini_entries_ex(ini_entries, module_number, MODULE_PERSISTENT);

	/* Register Zend ini entries */
	zend_register_standard_ini_entries();

#ifdef ZEND_WIN32
	/* Until the current ini values was setup, the current cp is 65001.
		If the actual ini values are different, some stuff needs to be updated.
		It concerns at least main_cwd_state and there might be more. As we're
		still in the startup phase, lets use the chance and reinit the relevant
		item according to the current codepage. Still, if ini_set() is used
		later on, a more intelligent way to update such stuff is needed.
		Startup/shutdown routines could involve touching globals and thus
		can't always be used on demand. */
	if (!php_win32_cp_use_unicode()) {
		virtual_cwd_main_cwd_init(1);
	}
#endif

	/* Disable realpath cache if an open_basedir is set */
	if (PG(open_basedir) && *PG(open_basedir)) {
		CWDG(realpath_cache_size_limit) = 0;
	}

	PG(have_called_openlog) = 0;

	/* initialize stream wrappers registry
	 * (this uses configuration parameters from php.ini)
	 */
	if (php_init_stream_wrappers(module_number) == FAILURE)	{
		php_printf("PHP:  Unable to initialize stream url wrappers.\n");
		return FAILURE;
	}

	zuv.html_errors = 1;
	php_startup_auto_globals();
	zend_set_utility_values(&zuv);
	php_startup_sapi_content_types();

	/* Begin to fingerprint the process state */
	zend_startup_system_id();

	/* startup extensions statically compiled in */
	if (php_register_internal_extensions_func() == FAILURE) {
		php_printf("Unable to start builtin modules\n");
		return FAILURE;
	}

	/* start additional PHP extensions */
	if (additional_module && (zend_register_internal_module(additional_module) == NULL)) {
		return FAILURE;
	}

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
	zend_disable_functions(INI_STR("disable_functions"));
	php_disable_classes();

	/* make core report what it should */
	if ((module = zend_hash_str_find_ptr(&module_registry, "core", sizeof("core")-1)) != NULL) {
		module->version = PHP_VERSION;
		module->info_func = PHP_MINFO(php_core);
	}

	/* freeze the list of observer fcall_init handlers */
	zend_observer_post_startup();

	/* Extensions that add engine hooks after this point do so at their own peril */
	zend_finalize_system_id();

	module_initialized = true;

	if (zend_post_startup() != SUCCESS) {
		return FAILURE;
	}

	/* Check for deprecated directives */
	/* NOTE: If you add anything here, remember to add it to build/Makefile.global! */
	{
		struct {
			const long error_level;
			const char *phrase;
			const char *directives[18]; /* Remember to change this if the number of directives change */
		} directives[2] = {
			{
				E_DEPRECATED,
				"Directive '%s' is deprecated",
				{
					"allow_url_include",
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
					"track_errors",
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

	virtual_cwd_deactivate();

	sapi_deactivate();
	module_startup = false;

	/* Don't leak errors from startup into the per-request phase. */
	clear_last_error();
	shutdown_memory_manager(1, 0);
	virtual_cwd_activate();

	zend_interned_strings_switch_storage(1);

#if ZEND_RC_DEBUG
	if (retval == SUCCESS) {
		zend_rc_debug = 1;
	}
#endif

	/* we're done */
	return retval;
}
/* }}} */

/* {{{ php_module_shutdown_wrapper */
int php_module_shutdown_wrapper(sapi_module_struct *sapi_globals)
{
	php_module_shutdown();
	return SUCCESS;
}
/* }}} */

/* {{{ php_module_shutdown */
void php_module_shutdown(void)
{
	int module_number=0;

	module_shutdown = true;

	if (!module_initialized) {
		return;
	}

	zend_interned_strings_switch_storage(0);

#if ZEND_RC_DEBUG
	zend_rc_debug = 0;
#endif

#ifdef PHP_WIN32
	(void)php_win32_shutdown_random_bytes();
	php_win32_signal_ctrl_handler_shutdown();
#endif

	sapi_flush();

	zend_shutdown();

#ifdef PHP_WIN32
	/*close winsock */
	WSACleanup();
#endif

	/* Destroys filter & transport registries too */
	php_shutdown_stream_wrappers(module_number);

	zend_unregister_ini_entries_ex(module_number, MODULE_PERSISTENT);

	/* close down the ini config */
	php_shutdown_config();
	clear_last_error();

#ifndef ZTS
	zend_ini_shutdown();
	shutdown_memory_manager(CG(unclean_shutdown), 1);
#else
	zend_ini_global_shutdown();
#endif

	php_output_shutdown();

#ifndef ZTS
	zend_interned_strings_dtor();
#endif

	if (zend_post_shutdown_cb) {
		void (*cb)(void) = zend_post_shutdown_cb;

		zend_post_shutdown_cb = NULL;
		cb();
	}

	module_initialized = false;

#ifndef ZTS
	core_globals_dtor(&core_globals);
	gc_globals_dtor();
#else
	ts_free_id(core_globals_id);
#endif

#ifdef PHP_WIN32
	if (old_invalid_parameter_handler == NULL) {
		_set_invalid_parameter_handler(old_invalid_parameter_handler);
	}
#endif

	zend_observer_shutdown();
}
/* }}} */

/* {{{ php_execute_script */
PHPAPI bool php_execute_script(zend_file_handle *primary_file)
{
	zend_file_handle *prepend_file_p = NULL, *append_file_p = NULL;
	zend_file_handle prepend_file, append_file;
#ifdef HAVE_BROKEN_GETCWD
	volatile int old_cwd_fd = -1;
#else
	char *old_cwd;
	ALLOCA_FLAG(use_heap)
#endif
	bool retval = false;

#ifndef HAVE_BROKEN_GETCWD
# define OLD_CWD_SIZE 4096
	old_cwd = do_alloca(OLD_CWD_SIZE, use_heap);
	old_cwd[0] = '\0';
#endif

	zend_try {
		char realfile[MAXPATHLEN];

#ifdef PHP_WIN32
		if(primary_file->filename) {
			UpdateIniFromRegistry(ZSTR_VAL(primary_file->filename));
		}
#endif

		PG(during_request_startup) = 0;

		if (primary_file->filename && !(SG(options) & SAPI_OPTION_NO_CHDIR)) {
#ifdef HAVE_BROKEN_GETCWD
			/* this looks nasty to me */
			old_cwd_fd = open(".", 0);
#else
			php_ignore_value(VCWD_GETCWD(old_cwd, OLD_CWD_SIZE-1));
#endif
			VCWD_CHDIR_FILE(ZSTR_VAL(primary_file->filename));
		}

		/* Only lookup the real file path and add it to the included_files list if already opened
		 *   otherwise it will get opened and added to the included_files list in zend_execute_scripts
		 */
		if (primary_file->filename &&
			!zend_string_equals_literal(primary_file->filename, "Standard input code") &&
			primary_file->opened_path == NULL &&
			primary_file->type != ZEND_HANDLE_FILENAME
		) {
			if (expand_filepath(ZSTR_VAL(primary_file->filename), realfile)) {
				primary_file->opened_path = zend_string_init(realfile, strlen(realfile), 0);
				zend_hash_add_empty_element(&EG(included_files), primary_file->opened_path);
			}
		}

		if (PG(auto_prepend_file) && PG(auto_prepend_file)[0]) {
			zend_stream_init_filename(&prepend_file, PG(auto_prepend_file));
			prepend_file_p = &prepend_file;
		}

		if (PG(auto_append_file) && PG(auto_append_file)[0]) {
			zend_stream_init_filename(&append_file, PG(auto_append_file));
			append_file_p = &append_file;
		}
		if (PG(max_input_time) != -1) {
#ifdef PHP_WIN32
			zend_unset_timeout();
#endif
			zend_set_timeout(INI_INT("max_execution_time"), 0);
		}

		retval = (zend_execute_scripts(ZEND_REQUIRE, NULL, 3, prepend_file_p, primary_file, append_file_p) == SUCCESS);
	} zend_end_try();

	if (prepend_file_p) {
		zend_destroy_file_handle(prepend_file_p);
	}

	if (append_file_p) {
		zend_destroy_file_handle(append_file_p);
	}

	if (EG(exception)) {
		zend_try {
			zend_exception_error(EG(exception), E_ERROR);
		} zend_end_try();
	}

#ifdef HAVE_BROKEN_GETCWD
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

/* {{{ php_execute_simple_script */
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
			UpdateIniFromRegistry(ZSTR_VAL(primary_file->filename));
		}
#endif

		PG(during_request_startup) = 0;

		if (primary_file->filename && !(SG(options) & SAPI_OPTION_NO_CHDIR)) {
			php_ignore_value(VCWD_GETCWD(old_cwd, OLD_CWD_SIZE-1));
			VCWD_CHDIR_FILE(ZSTR_VAL(primary_file->filename));
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

/* {{{ php_handle_aborted_connection */
PHPAPI void php_handle_aborted_connection(void)
{

	PG(connection_status) = PHP_CONNECTION_ABORTED;
	php_output_set_status(PHP_OUTPUT_DISABLED);

	if (!PG(ignore_user_abort)) {
		zend_bailout();
	}
}
/* }}} */

/* {{{ php_handle_auth_data */
PHPAPI int php_handle_auth_data(const char *auth)
{
	int ret = -1;
	size_t auth_len = auth != NULL ? strlen(auth) : 0;

	if (auth && auth_len > 0 && zend_binary_strncasecmp(auth, auth_len, "Basic ", sizeof("Basic ")-1, sizeof("Basic ")-1) == 0) {
		char *pass;
		zend_string *user;

		user = php_base64_decode((const unsigned char*)auth + 6, auth_len - 6);
		if (user) {
			pass = strchr(ZSTR_VAL(user), ':');
			if (pass) {
				*pass++ = '\0';
				SG(request_info).auth_user = estrndup(ZSTR_VAL(user), ZSTR_LEN(user));
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

	if (ret == -1 && auth && auth_len > 0 && zend_binary_strncasecmp(auth, auth_len, "Digest ", sizeof("Digest ")-1, sizeof("Digest ")-1) == 0) {
		SG(request_info).auth_digest = estrdup(auth + 7);
		ret = 0;
	}

	if (ret == -1) {
		SG(request_info).auth_digest = NULL;
	}

	return ret;
}
/* }}} */

/* {{{ php_lint_script */
PHPAPI zend_result php_lint_script(zend_file_handle *file)
{
	zend_op_array *op_array;
	zend_result retval = FAILURE;

	zend_try {
		op_array = zend_compile_file(file, ZEND_INCLUDE);

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

#ifdef ZTS
/* {{{ php_reserve_tsrm_memory */
PHPAPI void php_reserve_tsrm_memory(void)
{
	tsrm_reserve(
		TSRM_ALIGNED_SIZE(sizeof(zend_compiler_globals)) +
		TSRM_ALIGNED_SIZE(sizeof(zend_executor_globals)) +
		TSRM_ALIGNED_SIZE(sizeof(zend_php_scanner_globals)) +
		TSRM_ALIGNED_SIZE(sizeof(zend_ini_scanner_globals)) +
		TSRM_ALIGNED_SIZE(sizeof(virtual_cwd_globals)) +
#ifdef ZEND_SIGNALS
		TSRM_ALIGNED_SIZE(sizeof(zend_signal_globals_t)) +
#endif
		TSRM_ALIGNED_SIZE(zend_mm_globals_size()) +
		TSRM_ALIGNED_SIZE(zend_gc_globals_size()) +
		TSRM_ALIGNED_SIZE(sizeof(php_core_globals)) +
		TSRM_ALIGNED_SIZE(sizeof(sapi_globals_struct))
	);
}
/* }}} */

/* {{{ php_tsrm_startup */
PHPAPI bool php_tsrm_startup(void)
{
	bool ret = tsrm_startup(1, 1, 0, NULL);
	php_reserve_tsrm_memory();
	(void)ts_resource(0);
	return ret;
}
/* }}} */
#endif
