/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* #define CRASH_DETECTION */

#define SHUTDOWN_DEBUG(resource) fprintf(stderr, "*** Shutting down " resource "\n" )
#undef SHUTDOWN_DEBUG
#define SHUTDOWN_DEBUG(resource)

#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include <stdio.h>
#include "php.h"
#ifdef MSVC5
#include "win32/time.h"
#include "win32/signal.h"
#include <process.h>
#else
#include "build-defs.h"
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
#include "php_ini.h"
#include "php_globals.h"
#include "main.h"
#include "control_structures.h"
#include "fopen-wrappers.h"
#include "functions/basic_functions.h"
#include "functions/info.h"
#include "functions/head.h"
#include "functions/post.h"
#include "functions/head.h"
#include "functions/type.h"
#include "snprintf.h"
#if WIN32|WINNT
#include <io.h>
#include <fcntl.h>
#include "win32/syslog.h"
#else
#include <syslog.h>
#endif

#include "zend_compile.h"
#include "zend_execute.h"
#include "zend_highlight.h"
#include "zend_indent.h"

#if USE_SAPI
#include "serverapi/sapi.h"
void *gLock;
#ifndef THREAD_SAFE
struct sapi_request_info *sapi_rqst;
#endif
#endif

#if MSVC5 || !defined(HAVE_GETOPT)
#include "getopt.h"
#endif

#ifdef ZTS
int compiler_globals_id;
int executor_globals_id;
#endif

#ifndef ZTS
php_core_globals core_globals;
#endif

void _php3_build_argv(char * ELS_DC);
static void php3_timeout(int dummy);
static void php3_set_timeout(long seconds INLINE_TLS);

void *gLock;					/*mutex variable */


#define PHP_MODE_STANDARD	1
#define PHP_MODE_HIGHLIGHT	2
#define PHP_MODE_INDENT		3

/* True globals (no need for thread safety) */
HashTable configuration_hash;
char *php3_ini_path = NULL;


static PHP_INI_MH(OnSetPrecision)
{
	ELS_FETCH();

	EG(precision) = atoi(new_value);
	return SUCCESS;
}


static PHP_INI_MH(OnChangeMaxExecutionTime)
{
	int new_timeout;
	
	if (new_value) {
		new_timeout = atoi(new_value);
	} else {
		new_timeout = 0;
	}
	php3_set_timeout(new_timeout);
	return SUCCESS;
}


static PHP_INI_MH(OnChangeMemoryLimit)
{
	int new_limit;

	if (new_value) {
		new_limit = atoi(new_value);
	} else {
		new_limit = 2<<30;		/* effectively, no limit */
	}
	return zend_set_memory_limit(new_limit);
}


static PHP_INI_MH(OnUpdateErrorReporting)
{
	ELS_FETCH();

	if (!new_value) {
		EG(error_reporting) = E_ALL & ~E_NOTICE;
	} else {
		EG(error_reporting) = atoi(new_value);
	}
	return SUCCESS;
}


/* Need to convert to strings and make use of:
 * DEFAULT_SHORT_OPEN_TAG
 * PHP_SAFE_MODE
 *
 * Need to be read from the environment (?):
 * PHP_AUTO_PREPEND_FILE
 * PHP_AUTO_APPEND_FILE
 * PHP_DOCUMENT_ROOT
 * PHP_USER_DIR
 * PHP_INCLUDE_PATH
 */

#ifndef SAFE_MODE_EXEC_DIR
#	define SAFE_MODE_EXEC_DIR "/"
#endif

#ifdef PHP_PROG_SENDMAIL
#	define DEFAULT_SENDMAIL_PATH PHP_PROG_SENDMAIL " -t"
#else
#	define DEFAULT_SENDMAIL_PATH NULL
#endif

PHP_INI_BEGIN()
	PHP_INI_ENTRY("short_open_tag",		"1",		PHP_INI_ALL,		OnUpdateInt,			(void *) XtOffsetOf(php_core_globals, short_tags))
	PHP_INI_ENTRY("asp_tags",			"0",		PHP_INI_ALL,		OnUpdateInt,			(void *) XtOffsetOf(php_core_globals, asp_tags))
	PHP_INI_ENTRY("precision",			"14",		PHP_INI_ALL,		OnSetPrecision,	NULL)

	PHP_INI_ENTRY("highlight.comment",	HL_COMMENT_COLOR,	PHP_INI_ALL,		NULL,			NULL)
	PHP_INI_ENTRY("highlight.default",	HL_DEFAULT_COLOR,	PHP_INI_ALL,		NULL,			NULL)
	PHP_INI_ENTRY("highlight.html",		HL_HTML_COLOR,		PHP_INI_ALL,		NULL,			NULL)
	PHP_INI_ENTRY("highlight.string",	HL_STRING_COLOR,	PHP_INI_ALL,		NULL,			NULL)
	PHP_INI_ENTRY("highlight.bg",		HL_BG_COLOR,		PHP_INI_ALL,		NULL,			NULL)
	PHP_INI_ENTRY("highlight.keyword",	HL_KEYWORD_COLOR,	PHP_INI_ALL,		NULL,			NULL)

	PHP_INI_ENTRY("magic_quotes_gpc",		"1",			PHP_INI_ALL,		OnUpdateInt,	(void *) XtOffsetOf(php_core_globals, magic_quotes_gpc))
	PHP_INI_ENTRY("magic_quotes_runtime",	"0",			PHP_INI_ALL,		OnUpdateInt,	(void *) XtOffsetOf(php_core_globals, magic_quotes_runtime))
	PHP_INI_ENTRY("magic_quotes_sybase",	"0",			PHP_INI_ALL,		OnUpdateInt,	(void *) XtOffsetOf(php_core_globals, magic_quotes_sybase))

	PHP_INI_ENTRY("safe_mode",				"0",			PHP_INI_SYSTEM,		OnUpdateInt,	(void *) XtOffsetOf(php_core_globals, safe_mode))
	PHP_INI_ENTRY("sql.safe_mode",			"0",			PHP_INI_SYSTEM,		OnUpdateInt,	(void *) XtOffsetOf(php_core_globals, sql_safe_mode))
	PHP_INI_ENTRY("safe_mode_exec_dir",		SAFE_MODE_EXEC_DIR,		PHP_INI_SYSTEM,	OnUpdateString,	(void *) XtOffsetOf(php_core_globals, safe_mode_exec_dir))
	PHP_INI_ENTRY("enable_dl",				"1",			PHP_INI_SYSTEM,		OnUpdateInt,	(void *) XtOffsetOf(php_core_globals, enable_dl))

	PHP_INI_ENTRY("SMTP",			"localhost",			PHP_INI_ALL,		NULL,		NULL)
	PHP_INI_ENTRY("sendmail_path",	DEFAULT_SENDMAIL_PATH,	PHP_INI_SYSTEM,		NULL,		NULL)
	PHP_INI_ENTRY("sendmail_from",	NULL,					PHP_INI_ALL,		NULL,		NULL)

	PHP_INI_ENTRY("error_reporting",	NULL,				PHP_INI_ALL,		OnUpdateErrorReporting,		NULL)
	PHP_INI_ENTRY("display_errors",		"1",				PHP_INI_ALL,		OnUpdateInt,	(void *) XtOffsetOf(php_core_globals, display_errors))
	PHP_INI_ENTRY("track_errors",		"0",				PHP_INI_ALL,		OnUpdateInt,	(void *) XtOffsetOf(php_core_globals, track_errors))
	PHP_INI_ENTRY("log_errors",			"0",				PHP_INI_ALL,		OnUpdateInt,	(void *) XtOffsetOf(php_core_globals, log_errors))
	PHP_INI_ENTRY("error_log",			NULL,				PHP_INI_ALL,		OnUpdateString,	(void *) XtOffsetOf(php_core_globals, error_log))

	PHP_INI_ENTRY("auto_prepend_file",	NULL,				PHP_INI_ALL,		OnUpdateString,	(void *) XtOffsetOf(php_core_globals, auto_prepend_file))
	PHP_INI_ENTRY("auto_append_file",	NULL,				PHP_INI_ALL,		OnUpdateString,	(void *) XtOffsetOf(php_core_globals, auto_append_file))

	PHP_INI_ENTRY("y2k_compliance",		"0",				PHP_INI_ALL,		OnUpdateInt,	(void *) XtOffsetOf(php_core_globals, y2k_compliance))

	PHP_INI_ENTRY("doc_root",			NULL,				PHP_INI_SYSTEM,		OnUpdateStringUnempty,	(void *) XtOffsetOf(php_core_globals, doc_root))
	PHP_INI_ENTRY("user_dir",			NULL,				PHP_INI_SYSTEM,		OnUpdateStringUnempty,	(void *) XtOffsetOf(php_core_globals, user_dir))
	PHP_INI_ENTRY("include_path",		NULL,				PHP_INI_ALL,		OnUpdateStringUnempty,	(void *) XtOffsetOf(php_core_globals, include_path))
	PHP_INI_ENTRY("open_basedir",		NULL,				PHP_INI_SYSTEM,		OnUpdateStringUnempty,	(void *) XtOffsetOf(php_core_globals, open_basedir))
	PHP_INI_ENTRY("extension_dir",		NULL,				PHP_INI_SYSTEM,		OnUpdateStringUnempty,	(void *) XtOffsetOf(php_core_globals, extension_dir))

	PHP_INI_ENTRY("upload_tmp_dir",			NULL,			PHP_INI_SYSTEM,		OnUpdateStringUnempty,	(void *) XtOffsetOf(php_core_globals, upload_tmp_dir))
	PHP_INI_ENTRY("upload_max_filesize",	"2097152",		PHP_INI_ALL,		OnUpdateInt,			(void *) XtOffsetOf(php_core_globals, upload_max_filesize))

	PHP_INI_ENTRY("browscap",			NULL,				PHP_INI_SYSTEM,		NULL,			NULL)

	PHP_INI_ENTRY("define_syslog_variables",	"0",		PHP_INI_ALL,		NULL,			NULL)

	PHP_INI_ENTRY("max_execution_time",		"30",			PHP_INI_ALL,		OnChangeMaxExecutionTime,		NULL)
	PHP_INI_ENTRY("memory_limit",			"8388608",		PHP_INI_ALL,		OnChangeMemoryLimit,			NULL)

	PHP_INI_ENTRY("gpc_order",				"GPC",			PHP_INI_ALL,		OnUpdateStringUnempty,	(void *) XtOffsetOf(php_core_globals, gpc_order))
	PHP_INI_ENTRY("arg_separator",			"&",			PHP_INI_ALL,		OnUpdateStringUnempty,	(void *) XtOffsetOf(php_core_globals, arg_separator))
PHP_INI_END()



#ifndef THREAD_SAFE
/*
 * Globals yet to be protected
 */
int initialized;				/* keep track of which resources were successfully initialized */
static int module_initialized = 0;
unsigned char header_is_being_sent;

#if WIN32|WINNT
unsigned int wintimer;
unsigned int timerstart;
unsigned int wintimer_counter = 0;
#endif

#if APACHE
request_rec *php3_rqst = NULL;	/* request record pointer for apache module version */
#endif

/*
 * End of globals to be protected
 */

#endif

#if APACHE
void php3_apache_puts(const char *s)
{
	TLS_VARS;

	if (GLOBAL(php3_rqst)) {
		rputs(s, GLOBAL(php3_rqst));
	} else {
		fputs(s, stdout);
	}
}

void php3_apache_putc(char c)
{
	TLS_VARS;

	if (GLOBAL(php3_rqst)) {
		rputc(c, GLOBAL(php3_rqst));
	} else {
		fputc(c, stdout);
	}
}
#endif

void php3_log_err(char *log_message)
{
	FILE *log_file;
	TLS_VARS;

	/* Try to use the specified logging location. */
	if (PG(error_log) != NULL) {
#if HAVE_SYSLOG_H
		if (strcmp(PG(error_log), "syslog")) {
			syslog(LOG_NOTICE, log_message);
			return;
		} else {
#endif
			log_file = fopen(PG(error_log), "a");
			if (log_file != NULL) {
				fprintf(log_file, log_message);
				fprintf(log_file, "\n");
				fclose(log_file);
				return;
			}
#if HAVE_SYSLOG_H
		}
#endif
	}
	/* Otherwise fall back to the default logging location. */
#if APACHE
	if (GLOBAL(php3_rqst)) {
#if MODULE_MAGIC_NUMBER >= 19970831
		aplog_error(NULL, 0, APLOG_ERR | APLOG_NOERRNO, php3_rqst->server, log_message);
#else
		log_error(log_message, php3_rqst->server);
#endif
	} else {
		fprintf(stderr, log_message);
		fprintf(stderr, "\n");
	}
#endif							/*APACHE */

#if CGI_BINARY
	if (php3_header()) {
		fprintf(stderr, log_message);
		fprintf(stderr, "\n");
	}
#endif
}


/* is 4K big enough? */
#define PRINTF_BUFFER_SIZE 1024*4

/* wrapper for modules to use PHPWRITE */
PHPAPI int php3_write(void *buf, int size)
{
	TLS_VARS;
	return PHPWRITE(buf, size);
}

PHPAPI int php3_printf(const char *format,...)
{
	va_list args;
	int ret;
	char buffer[PRINTF_BUFFER_SIZE];
	int size;

	va_start(args, format);
	size = vsprintf(buffer, format, args);
	ret = PHPWRITE(buffer, size);
	va_end(args);
	
	return ret;
}


/* extended error handling function */
PHPAPI void php3_error(int type, const char *format,...)
{
	va_list args;
	char *error_filename = NULL;
	uint error_lineno;
	char buffer[1024];
	int size = 0;
	ELS_FETCH();

	if (!(type & E_CORE)) {
		if (!GLOBAL(initialized)) {	/* don't display further errors after php3_request_shutdown() */
			return;
		}
	}
	switch (type) {
		case E_CORE_ERROR:
		case E_CORE_WARNING:
			error_filename = NULL;
			error_lineno = 0;
			break;
		case E_PARSE:
		case E_COMPILE_ERROR:
		case E_COMPILE_WARNING: {
				CLS_FETCH();

				error_filename = zend_get_compiled_filename();
				error_lineno = CG(zend_lineno);
			}
			break;
		case E_ERROR:
		case E_NOTICE:
		case E_WARNING:
			error_filename = zend_get_executed_filename(ELS_C);
			error_lineno = zend_get_executed_lineno(ELS_C);
			break;
		default:
			error_filename = NULL;
			error_lineno = 0;
			break;
	}
		
	if (EG(error_reporting) & type || (type & E_CORE)) {
		char *error_type_str;

		switch (type) {
			case E_ERROR:
			case E_CORE_ERROR:
			case E_COMPILE_ERROR:
				error_type_str = "Fatal error";
				break;
			case E_WARNING:
			case E_CORE_WARNING:
			case E_COMPILE_WARNING:
				error_type_str = "Warning";
				break;
			case E_PARSE:
				error_type_str = "Parse error";
				break;
			case E_NOTICE:
				error_type_str = "Warning";
				break;
			default:
				error_type_str = "Unknown error";
				break;
		}

		/* get include file name */
		if (PG(log_errors) || PG(display_errors)) {
			va_start(args, format);
			size = vsnprintf(buffer, sizeof(buffer) - 1, format, args);
			va_end(args);
			buffer[sizeof(buffer) - 1] = 0;

			if (PG(log_errors)) {
				char log_buffer[1024];

				snprintf(log_buffer, 1024, "PHP 3 %s:  %s in %s on line %d", error_type_str, buffer, error_filename, error_lineno);
				php3_log_err(log_buffer);
			}
			if (PG(display_errors)) {
				char *prepend_string = INI_STR("error_prepend_string");
				char *append_string = INI_STR("error_append_string");

				if (prepend_string) {
					PUTS(prepend_string);
				}		
				php3_printf("<br>\n<b>%s</b>:  %s in <b>%s</b> on line <b>%d</b><br>\n", error_type_str, buffer, error_filename, error_lineno);
				if (append_string) {
					PUTS(append_string);
				}		
			}
		}
	}
	if (PG(track_errors)) {
		pval tmp;

		va_start(args, format);
		size = vsnprintf(buffer, sizeof(buffer) - 1, format, args);
		va_end(args);
		buffer[sizeof(buffer) - 1] = 0;

		tmp.value.str.val = (char *) estrndup(buffer, size);
		tmp.value.str.len = size;
		tmp.type = IS_STRING;

		_php3_hash_update(EG(active_symbol_table), "php_errormsg", sizeof("php_errormsg"), (void *) & tmp, sizeof(pval), NULL);
	}

	switch (type) {
		case E_ERROR:
		case E_CORE_ERROR:
		/*case E_PARSE: the parser would return 1 (failure), we can bail out nicely */
		case E_COMPILE_ERROR:
			zend_bailout();
			break;
	}
}




#if HAVE_SETITIMER
static void php3_timeout(int dummy)
{
	TLS_VARS;

	php3_error(E_ERROR, "Maximum execution time of %d seconds exceeded", PG(max_execution_time));
}
#endif

/* This one doesn't exists on QNX */
#ifndef SIGPROF
#define SIGPROF 27
#endif

static void php3_set_timeout(long seconds INLINE_TLS)
{
#if WIN32|WINNT
	if (seconds > 0) {
		GLOBAL(timerstart) = (unsigned int) clock();
		GLOBAL(wintimer) = GLOBAL(timerstart) + (CLOCKS_PER_SEC * seconds);
	} else {
		GLOBAL(wintimer) = 0;
	}
#else
#if HAVE_SETITIMER
	struct itimerval t_r;		/* timeout requested */

	t_r.it_value.tv_sec = seconds;
	t_r.it_value.tv_usec = t_r.it_interval.tv_sec = t_r.it_interval.tv_usec = 0;

	setitimer(ITIMER_PROF, &t_r, NULL);
	signal(SIGPROF, php3_timeout);
#endif
#endif
}


static void php3_unset_timeout(INLINE_TLS_VOID)
{
#if WIN32|WINNT
	GLOBAL(wintimer) = 0;
#else
#if HAVE_SETITIMER
	struct itimerval no_timeout;

	no_timeout.it_value.tv_sec = no_timeout.it_value.tv_usec = no_timeout.it_interval.tv_sec = no_timeout.it_interval.tv_usec = 0;

	setitimer(ITIMER_PROF, &no_timeout, NULL);
#endif
#endif
}


void php3_set_time_limit(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *new_timeout;

	if (PG(safe_mode)) {
		php3_error(E_WARNING, "Cannot set time limit in safe mode");
		RETURN_FALSE;
	}
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &new_timeout) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(new_timeout);
	/* FIXME ** This is BAD...in a threaded situation, any user
	   can set the timeout for php on a server wide basis. 
	   INI variables should not be reset via a user script

	   Fix what?  At least on Unix, timers like these are
	   per-thread timers.  Well, with a little work they will
	   be.  If we use a bound thread and proper masking it
	   should work fine.  Is this FIXME a WIN32 problem?  Is
	   there no way to do per-thread timers on WIN32?
	 */
	php3_unset_timeout(_INLINE_TLS_VOID);
	php3_set_timeout(new_timeout->value.lval _INLINE_TLS);
}


static FILE *php_fopen_wrapper_for_zend(const char *filename)
{
	int issock=0, socketd=0;

	FILE *retval=php3_fopen_wrapper((char *) filename, "r", USE_PATH|IGNORE_URL_WIN, &issock, &socketd);
	if (issock) {
		retval = fdopen(socketd, "r");
	}
	return retval;
}


static void php_message_handler_for_zend(long message, void *data)
{
	switch (message) {
		case ZMSG_ENABLE_TRACK_VARS:
			PG(track_vars) = 1;
			break;
		case ZMSG_FAILED_INCLUDE_FOPEN:
			php3_error(E_WARNING, "Failed opening '%s' for inclusion", php3_strip_url_passwd((char *) data));
			break;
		case ZMSG_FAILED_REQUIRE_FOPEN:
			php3_error(E_ERROR, "Failed opening required '%s'", php3_strip_url_passwd((char *) data));
			break;
		case ZMSG_FAILED_HIGHLIGHT_FOPEN:
			php3_error(E_WARNING, "Failed opening '%s' for highlighting", php3_strip_url_passwd((char *) data));
			break;
		case ZMSG_MEMORY_LEAK_DETECTED: {
				mem_header *t = (mem_header *) data;
				ELS_FETCH();

				if (EG(error_reporting)&E_WARNING) {
#if ZEND_DEBUG
#	if APACHE  /* log into the errorlog, since at this time we can't send messages to the browser */
					char memory_leak_buf[512];

					snprintf(memory_leak_buf,512,"Possible PHP3 memory leak detected (harmless):  %d bytes from %s:%d",t->size,t->filename,t->lineno);
#		if MODULE_MAGIC_NUMBER >= 19970831
					aplog_error(NULL, 0, APLOG_ERR | APLOG_NOERRNO, GLOBAL(php3_rqst)->server, memory_leak_buf);
#		else
					log_error(memory_leak_buf,GLOBAL(php3_rqst)->server);
#		endif
#	else
					php3_printf("Freeing 0x%0.8X (%d bytes), allocated in %s on line %d<br>\n",(void *)((char *)t+sizeof(mem_header)+PLATFORM_PADDING),t->size,t->filename,t->lineno);
#	endif
#endif
				}
			}
			break;
	}
}


int php3_request_startup(CLS_D ELS_DC)
{
	zend_output_startup();
#if APACHE && defined(CRASH_DETECTION)
	{
		char log_message[256];
		
		snprintf(log_message,256,"php3_request_startup(): script='%s', pid=%d",php3_rqst->filename,getpid());
		log_error(log_message, php3_rqst->server);
	}
#endif

	php3_set_timeout(PG(max_execution_time) _INLINE_TLS);

	GLOBAL(initialized) = 0;

#if APACHE
	/*
	 * For the Apache module version, this bit of code registers a cleanup
	 * function that gets triggered when our request pool is destroyed.
	 * We need this because at any point in our code we can be interrupted
	 * and that may happen before we have had time to free our memory.
	 * The php3_shutdown function needs to free all outstanding allocated
	 * memory.  
	 */
	block_alarms();
	register_cleanup(GLOBAL(php3_rqst)->pool, NULL, php3_request_shutdown, php3_request_shutdown_for_exec);
	unblock_alarms();
#endif

	/* initialize global variables */
	{
		GLOBAL(header_is_being_sent) = 0;
	}

	if (php3_init_request_info(NULL)) {
		php3_printf("Unable to initialize request info.\n");
		return FAILURE;
	}
	GLOBAL(initialized) |= INIT_REQUEST_INFO;
	
	init_compiler(CLS_C ELS_CC);
	init_executor(CLS_C ELS_CC);

	startup_scanner(CLS_C);


	return SUCCESS;
}


void php3_request_shutdown_for_exec(void *dummy)
{
	TLS_VARS;
	
	/* used to close fd's in the 3..255 range here, but it's problematic
	 */
	shutdown_memory_manager(1, 1);
}


int return_one(void *p)
{
	return 1;
}


void php3_request_shutdown(void *dummy INLINE_TLS)
{
#if FHTTPD
	char tmpline[128];
	int i, serverdefined;
#endif
#if APACHE && defined(CRASH_DETECTION)
	{
		char log_message[256];
		
		snprintf(log_message,256,"php3_request_shutdown(): script='%s', pid=%d",php3_rqst->filename,getpid());
		log_error(log_message, php3_rqst->server);
	}
#endif
	CLS_FETCH();
	ELS_FETCH();

	php3_header();
	zend_end_ob_buffering(1);


	php3_call_shutdown_functions();
	
	GLOBAL(initialized) &= ~INIT_ENVIRONMENT;	/* does not require any special shutdown */

	php_ini_rshutdown();

	shutdown_scanner(CLS_C);

	shutdown_compiler(CLS_C);
	shutdown_executor(ELS_C);

	if (GLOBAL(initialized) & INIT_REQUEST_INFO) {
		SHUTDOWN_DEBUG("Request info");
		php3_destroy_request_info(NULL);
		GLOBAL(initialized) &= ~INIT_REQUEST_INFO;
	}
	if (GLOBAL(initialized) & INIT_SCANNER) {
		SHUTDOWN_DEBUG("Scanner");
		reset_scanner(CLS_C);
		GLOBAL(initialized) &= ~INIT_SCANNER;
	}
	SHUTDOWN_DEBUG("Memory manager");
	shutdown_memory_manager(0, 0);
	if (GLOBAL(initialized)) {
		php3_error(E_WARNING, "Unknown resources in request shutdown function");
	}
	php3_unset_timeout(_INLINE_TLS_VOID);


#if CGI_BINARY
	fflush(stdout);
	if(GLOBAL(request_info).php_argv0) {
		free(GLOBAL(request_info).php_argv0);
		GLOBAL(request_info).php_argv0 = NULL;
	}
#endif
#if FHTTPD
	if (response) {
		if (!headermade) {
			makestandardheader(response, 200, "text/html", "fhttpd", req && req->keepalive);
		} else {
			if (headerfirstline)
				putlinetoheader(response, headerfirstline);
			else
				putlinetoheader(response, "HTTP/1.0 200 OK\r\n");
			serverdefined = 0;
			for (i = 0; i < headerlines; i++) {
				if (!strncmp(currentheader[i], "Server:", 7))
					serverdefined = 1;
				putlinetoheader(response, currentheader[i]);
			}
			if (!serverdefined)
				putlinetoheader(response, "Server: fhttpd\r\n");
			if (response->datasize) {
				sprintf(tmpline, "Content-Length: %ld\r\n", response->datasize);
				putlinetoheader(response, tmpline);
				if (req && req->keepalive)
					putlinetoheader(response,
									"Connection: Keep-Alive\r\nKeep-Alive: max=0, timeout=30\r\n");
			}
			php3_fhttpd_free_header();
		}
		sendresponse(server, response);
		if (response->datasize)
			finishresponse(server, response);
		else
			finishdropresponse(server, response);
		deleteresponse(response);
	}
	response = NULL;
	if (req)
		deleterequest(req);
	req = NULL;
#endif
#if USE_SAPI
	GLOBAL(sapi_rqst)->flush(GLOBAL(sapi_rqst)->scid);
#endif
}


static int php3_config_ini_startup(ELS_D)
{
	if (php3_init_config() == FAILURE) {
		php3_printf("PHP:  Unable to parse configuration file.\n");
		return FAILURE;
	}
	return SUCCESS;
}

static void php3_config_ini_shutdown(INLINE_TLS_VOID)
{
#if USE_SAPI
	php3_shutdown_config();
#else
	if (GLOBAL(module_initialized) & INIT_CONFIG) {
		php3_shutdown_config();
		GLOBAL(module_initialized) &= ~INIT_CONFIG;
	}
#endif
}

int php3_module_startup(CLS_D ELS_DC)
{
	zend_utility_functions zuf;
	zend_utility_values zuv;
	int module_number=0;	/* for REGISTER_INI_ENTRIES() */

#if (WIN32|WINNT) && !(USE_SAPI)
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD(2, 0);
#else
	if (GLOBAL(module_initialized)) {
		return SUCCESS;
	}
#endif

	zend_output_startup();

	zuf.error_function = php3_error;
	zuf.printf_function = php3_printf;
	zuf.write_function = zend_body_write;
	zuf.fopen_function = php_fopen_wrapper_for_zend;
	zuf.message_handler = php_message_handler_for_zend;
	zuf.block_interruptions = BLOCK_INTERRUPTIONS;
	zuf.unblock_interruptions = UNBLOCK_INTERRUPTIONS;

	zend_startup(&zuf, NULL);

#if HAVE_SETLOCALE
	setlocale(LC_CTYPE, "");
#endif

	EG(error_reporting) = E_ALL;

#if (WIN32|WINNT) && !(USE_SAPI)
	/* start up winsock services */
	if (WSAStartup(wVersionRequested, &wsaData) != 0) {
		php3_printf("\nwinsock.dll unusable. %d\n", WSAGetLastError());
		return FAILURE;
	}
	GLOBAL(module_initialized) |= INIT_WINSOCK;
#endif

	SET_MUTEX(gLock);
	le_index_ptr = _register_list_destructors(NULL, NULL, 0);
	FREE_MUTEX(gLock);

	if (php3_config_ini_startup(ELS_C) == FAILURE) {
		return FAILURE;
	}

	php_ini_mstartup();
	REGISTER_INI_ENTRIES();

	zuv.short_tags = (unsigned char) PG(short_tags);
	zuv.asp_tags = (unsigned char) PG(asp_tags);
	zend_set_utility_values(&zuv);

	if (module_startup_modules() == FAILURE) {
		php3_printf("Unable to start modules\n");
		return FAILURE;
	}
	return SUCCESS;
}



void php3_module_shutdown_for_exec(void)
{
	/* used to close fd's in the range 3.255 here, but it's problematic */
}

void php3_module_shutdown()
{
	int module_number=0;	/* for UNREGISTER_INI_ENTRIES() */
	CLS_FETCH();
	ELS_FETCH();

#if !USE_SAPI
	/* close down the ini config */
	php3_config_ini_shutdown(_INLINE_TLS_VOID);
#endif

#if (WIN32|WINNT) && !(USE_SAPI)
	/*close winsock */
	if (GLOBAL(module_initialized) & INIT_WINSOCK) {
		WSACleanup();
		GLOBAL(module_initialized) &= ~INIT_WINSOCK;
	}
#endif

	if (GLOBAL(module_initialized)) {
		php3_error(E_WARNING, "Unknown resource in module shutdown");
	}
#if CGI_BINARY
	fflush(stdout);
#endif
#if 0							/* SAPI */
	GLOBAL(sapi_rqst)->flush(GLOBAL(sapi_rqst)->scid);
#endif

	zend_shutdown();
	UNREGISTER_INI_ENTRIES();
	php_ini_mshutdown();
	shutdown_memory_manager(0, 1);
}


/* in 3.1 some of this should move into sapi */
int _php3_hash_environment(void)
{
	char **env, *p, *t;
	unsigned char _gpc_flags[3] = {0,0,0};
	pval *tmp;
	ELS_FETCH();
	
	p = PG(gpc_order);
	while(*p) {
		switch(*p++) {
			case 'p':
			case 'P':
				if (!_gpc_flags[0] && php3_headers_unsent() && GLOBAL(request_info).request_method && !strcasecmp(GLOBAL(request_info).request_method, "post")) {
					php3_treat_data(PARSE_POST, NULL);	/* POST Data */
					_gpc_flags[0]=1;
				}
				break;
			case 'c':
			case 'C':
				if (!_gpc_flags[1]) {
					php3_treat_data(PARSE_COOKIE, NULL);	/* Cookie Data */
					_gpc_flags[1]=1;
				}
				break;
			case 'g':
			case 'G':
				if (!_gpc_flags[2]) {
					php3_treat_data(PARSE_GET, NULL);	/* GET Data */
					_gpc_flags[2]=1;
				}
				break;
		}
	}

	
	for (env = environ; env != NULL && *env != NULL; env++) {
		p = strchr(*env, '=');
		if (!p) {				/* malformed entry? */
			continue;
		}
		t = estrndup(*env, p - *env);
		tmp = (pval *) emalloc(sizeof(pval));
		tmp->value.str.len = strlen(p + 1);
		tmp->value.str.val = estrndup(p + 1, tmp->value.str.len);
		tmp->type = IS_STRING;
		tmp->refcount=1;
		tmp->is_ref=0;
		/* environmental variables never take precedence over get/post/cookie variables */
		_php3_hash_add(&EG(symbol_table), t, p - *env + 1, &tmp, sizeof(pval *), NULL);
		efree(t);
	}

#if APACHE
	{
		pval **tmp_ptr;
		register int i;
		array_header *arr = table_elts(GLOBAL(php3_rqst)->subprocess_env);
		table_entry *elts = (table_entry *) arr->elts;
		int len;

		for (i = 0; i < arr->nelts; i++) {
			len = strlen(elts[i].key);
			t = elts[i].key;
			tmp = (pval *) emalloc(sizeof(pval));
			if (elts[i].val) {
				tmp->value.str.len = strlen(elts[i].val);
				tmp->value.str.val = estrndup(elts[i].val, tmp->value.str.len);
			} else {
				tmp->value.str.len = 0;
				tmp->value.str.val = empty_string;
			}
			tmp->refcount=1;
			tmp->is_ref=0;
			tmp->type = IS_STRING;
			_php3_hash_update(&EG(symbol_table), t, strlen(t)+1, &tmp, sizeof(pval *), NULL);
		}
		/* insert special variables */
		if (_php3_hash_find(&EG(symbol_table), "SCRIPT_FILENAME", sizeof("SCRIPT_FILENAME"), (void **) &tmp_ptr) == SUCCESS) {
			(*tmp_ptr)->refcount++;
			_php3_hash_update(&EG(symbol_table), "PATH_TRANSLATED", sizeof("PATH_TRANSLATED"), tmp_ptr, sizeof(pval *), NULL);
		}
		tmp = (pval *) emalloc(sizeof(pval));
		tmp->value.str.len = strlen(GLOBAL(php3_rqst)->uri);
		tmp->value.str.val = estrndup(GLOBAL(php3_rqst)->uri, tmp->value.str.len);
		tmp->refcount=1;
		tmp->is_ref=0;
		tmp->type = IS_STRING;
		_php3_hash_update(&EG(symbol_table), "PHP_SELF", sizeof("PHP_SELF"), (void *) &tmp, sizeof(pval *), NULL);
	}
#else
#if FHTTPD
	{
		int i, j;
		if (req) {
			for (i = 0; i < req->nlines; i++) {
				if (req->lines[i].paramc > 1 && req->lines[i].params[0] && req->lines[i].params[1]) {
					tmp = (pval *) emalloc(sizeof(pval));
					tmp->value.str.len = strlen(req->lines[i].params[1]);
					tmp->value.str.val = estrndup(req->lines[i].params[1], tmp->value.str.len);
					tmp->type = IS_STRING;
					tmp->refcount=1;
					tmp->is_ref=0;
					_php3_hash_update(&EG(symbol_table), req->lines[i].params[0],
									strlen(req->lines[i].params[0]) + 1, &tmp, 
									sizeof(pval *), NULL);
				}
			}
			if (req->script_name_resolved) {
				i = strlen(req->script_name_resolved);
				tmp = (pval *) emalloc(sizeof(pval));
				tmp->value.str.len = i;
				tmp->value.str.val = estrndup(req->script_name_resolved, i);
				tmp->type = IS_STRING;
				tmp->refcount=1;
				tmp->is_ref=0;
				_php3_hash_update(&EG(symbol_table), "PATH_TRANSLATED",
								sizeof("PATH_TRANSLATED"),
								&tmp, sizeof(pval *), NULL);
				if (req->script_name) {
					j = i - strlen(req->script_name);
					if (j > 0
						&& !strcmp(req->script_name_resolved + j,
								   req->script_name)) {
						tmp = (pval *) emalloc(sizeof(pval));
						tmp->value.str.len = j;
						tmp->value.str.val = estrndup(req->script_name_resolved, j);
						tmp->type = IS_STRING;
						tmp->refcount=1;
						tmp->is_ref=0;
						_php3_hash_update(&EG(symbol_table), "DOCUMENT_ROOT",
										sizeof("DOCUMENT_ROOT"),
							   &tmp, sizeof(pval *), NULL);
					}
				}
			}
		}
	}
#endif
	{
		/* Build the special-case PHP_SELF variable for the CGI version */
		char *pi;
#if FORCE_CGI_REDIRECT
		pi = GLOBAL(request_info).path_info;
		tmp = (pval *) emalloc(sizeof(pval));
		tmp->value.str.val = emalloc(((pi)?strlen(pi):0) + 1);
		tmp->value.str.len = _php3_sprintf(tmp->value.str.val, "%s", (pi ? pi : ""));	/* SAFE */
		tmp->type = IS_STRING;
		tmp->refcount=1;
		tmp->is_ref=0;
#else
		int l = 0;
		char *sn;
		sn = GLOBAL(request_info).script_name;
		pi = GLOBAL(request_info).path_info;
		if (sn)
			l += strlen(sn);
		if (pi)
			l += strlen(pi);
		if (pi && sn && !strcmp(pi, sn)) {
			l -= strlen(pi);
			pi = NULL;
		}
		tmp = (pval *) emalloc(sizeof(pval));
		tmp->value.str.val = emalloc(l + 1);
		tmp->value.str.len = _php3_sprintf(tmp->value.str.val, "%s%s", (sn ? sn : ""), (pi ? pi : ""));	/* SAFE */
		tmp->type = IS_STRING;
		tmp->refcount=1;
		tmp->is_ref=0;
#endif
		_php3_hash_update(&EG(symbol_table), "PHP_SELF", sizeof("PHP_SELF"), (void *) & tmp, sizeof(pval *), NULL);
	}
#endif


	/* need argc/argv support as well */
	_php3_build_argv(GLOBAL(request_info).query_string ELS_CC);

	GLOBAL(initialized) |= INIT_ENVIRONMENT;

	return SUCCESS;
}

void _php3_build_argv(char *s ELS_DC)
{
	pval *arr, *tmp;
	int count = 0;
	char *ss, *space;

	arr = (pval *) emalloc(sizeof(pval));
	arr->value.ht = (HashTable *) emalloc(sizeof(HashTable));
	if (_php3_hash_init(arr->value.ht, 0, NULL, PVAL_PTR_DTOR, 0) == FAILURE) {
		php3_error(E_WARNING, "Unable to create argv array");
	} else {
		arr->type = IS_ARRAY;
		arr->refcount=1;
		arr->is_ref=0;
		_php3_hash_update(&EG(symbol_table), "argv", sizeof("argv"), &arr, sizeof(pval *), NULL);
	}
	/* now pick out individual entries */
	ss = s;
	while (ss) {
		space = strchr(ss, '+');
		if (space) {
			*space = '\0';
		}
		/* auto-type */
		tmp = (pval *) emalloc(sizeof(pval));
		tmp->type = IS_STRING;
		tmp->value.str.len = strlen(ss);
		tmp->value.str.val = estrndup(ss, tmp->value.str.len);
		tmp->refcount=1;
		tmp->is_ref=0;
		count++;
		if (_php3_hash_next_index_insert(arr->value.ht, &tmp, sizeof(pval *), NULL)==FAILURE) {
			if (tmp->type == IS_STRING) {
				efree(tmp->value.str.val);
			}
		}
		if (space) {
			*space = '+';
			ss = space + 1;
		} else {
			ss = space;
		}
	}
	tmp = (pval *) emalloc(sizeof(pval));
	tmp->value.lval = count;
	tmp->type = IS_LONG;
	tmp->refcount=1;
	tmp->is_ref=0;
	_php3_hash_add(&EG(symbol_table), "argc", sizeof("argc"), &tmp, sizeof(pval *), NULL);
}


#include "logos.h"

static void php3_parse(zend_file_handle *primary_file CLS_DC ELS_DC)
{
	zend_file_handle *prepend_file_p, *append_file_p;
	zend_file_handle prepend_file, append_file;

	if (request_info.query_string && request_info.query_string[0]=='=') {
		if (!strcmp(request_info.query_string+1, "PHPE9568F34-D428-11d2-A769-00AA001ACF42")) {
			char *header_line = estrndup("Content-type:  image/gif", sizeof("Content-type:  image/gif")-1);

			php4i_add_header_information(header_line);
			PHPWRITE(php4_logo, sizeof(php4_logo));
			efree(header_line);
			return;
		} else if (!strcmp(request_info.query_string+1, "PHPE9568F35-D428-11d2-A769-00AA001ACF42")) {
			char *header_line = estrndup("Content-type:  image/gif", sizeof("Content-type:  image/gif")-1);

			php4i_add_header_information(header_line);
			PHPWRITE(zendtech_logo, sizeof(zendtech_logo));
			efree(header_line);
			return;
		}
	}

	if (setjmp(EG(bailout))!=0) {
		return;
	}
	_php3_hash_environment();


	if (PG(auto_prepend_file) && PG(auto_prepend_file)[0]) {
		prepend_file.filename = PG(auto_prepend_file);
		prepend_file.type = ZEND_HANDLE_FILENAME;
		prepend_file_p = &prepend_file;
	} else {
		prepend_file_p = NULL;
	}
	if (PG(auto_append_file) && PG(auto_append_file)[0]) {
		append_file.filename = PG(auto_append_file);
		append_file.type = ZEND_HANDLE_FILENAME;
		append_file_p = &append_file;
	} else {
		append_file_p = NULL;
	}	
	EG(main_op_array) = zend_compile_files(0 CLS_CC, 3, prepend_file_p, primary_file, append_file_p);
	if (EG(main_op_array)) {
		EG(active_op_array) = EG(main_op_array);
		zend_execute(EG(main_op_array) ELS_CC);
	}
}



#if CGI_BINARY

static void _php3_usage(char *argv0)
{
	char *prog;

	prog = strrchr(argv0, '/');
	if (prog) {
		prog++;
	} else {
		prog = "php";
	}

	php3_printf("Usage: %s [-q] [-h]"
				" [-s]"
				" [-v] [-i] [-f <file>] | "
				"{<file> [args...]}\n"
				"  -q       Quiet-mode.  Suppress HTTP Header output.\n"
				"  -s       Display colour syntax highlighted source.\n"
				"  -f<file> Parse <file>.  Implies `-q'\n"
				"  -v       Version number\n"
				"  -c<path> Look for php3.ini file in this directory\n"
#if SUPPORT_INTERACTIVE
				"  -a		Run interactively\n"
#endif
				"  -e		Generate extended information for debugger/profiler\n"
				"  -i       PHP information\n"
				"  -h       This help\n", prog);
}

/* some systems are missing these from their header files */
extern char *optarg;
extern int optind;

#if THREAD_SAFE
extern flex_globals *yy_init_tls(void);
extern void yy_destroy_tls(void);
#endif

int main(int argc, char *argv[])
{
	int cgi = 0, c, i, len;
	zend_file_handle file_handle;
	char *s;
/* temporary locals */
	char *_cgi_filename=NULL;
	int _cgi_started=0;
	int behavior=PHP_MODE_STANDARD;
#if SUPPORT_INTERACTIVE
	int interactive=0;
#endif
/* end of temporary locals */
#ifdef ZTS
	zend_compiler_globals *compiler_globals;
	zend_executor_globals *executor_globals;
#endif


	if (setjmp(EG(bailout))!=0) {
		return -1;
	}
	
#ifdef ZTS
	sapi_startup(1,1,0);
	compiler_globals_id = ts_allocate_id(sizeof(zend_compiler_globals));
	executor_globals_id = ts_allocate_id(sizeof(zend_executor_globals));

	compiler_globals = ts_resource(compiler_globals_id);
	executor_globals = ts_resource(executor_globals_id);
#endif

	
#if WIN32|WINNT
	_fmode = _O_BINARY;			/*sets default for file streams to binary */
	setmode(_fileno(stdin), O_BINARY);		/* make the stdio mode be binary */
	setmode(_fileno(stdout), O_BINARY);		/* make the stdio mode be binary */
	setmode(_fileno(stderr), O_BINARY);		/* make the stdio mode be binary */
#endif


	/* Make sure we detect we are a cgi - a bit redundancy here,
	   but the default case is that we have to check only the first one. */
	if (getenv("SERVER_SOFTWARE")
		|| getenv("SERVER_NAME")
		|| getenv("GATEWAY_INTERFACE")
		|| getenv("REQUEST_METHOD")) {
		cgi = 1;
		if (argc > 1)
			GLOBAL(request_info).php_argv0 = strdup(argv[1]);
		else GLOBAL(request_info).php_argv0 = NULL;
#if FORCE_CGI_REDIRECT
		if (!getenv("REDIRECT_STATUS")) {
			PUTS("<b>Security Alert!</b>  PHP CGI cannot be accessed directly.\n\
\n\
<P>This PHP CGI binary was compiled with force-cgi-redirect enabled.  This\n\
means that a page will only be served up if the REDIRECT_STATUS CGI variable is\n\
set.  This variable is set, for example, by Apache's Action directive redirect.\n\
<P>You may disable this restriction by recompiling the PHP binary with the\n\
--disable-force-cgi-redirect switch.  If you do this and you have your PHP CGI\n\
binary accessible somewhere in your web tree, people will be able to circumvent\n\
.htaccess security by loading files through the PHP parser.  A good way around\n\
this is to define doc_root in your php3.ini file to something other than your\n\
top-level DOCUMENT_ROOT.  This way you can separate the part of your web space\n\n\
which uses PHP from the normal part using .htaccess security.  If you do not have\n\
any .htaccess restrictions anywhere on your site you can leave doc_root undefined.\n\
\n");

			/* remove that detailed explanation some time */

			return FAILURE;
		}
#endif							/* FORCE_CGI_REDIRECT */
	}


	CG(extended_info) = 0;

	if (!cgi) {					/* never execute the arguments if you are a CGI */
		GLOBAL(request_info).php_argv0 = NULL;
		while ((c = getopt(argc, argv, "c:qvisnaeh?vf:")) != -1) {
			switch (c) {
				case 'f':
					if (!_cgi_started){ 
						if (php3_module_startup(CLS_C ELS_CC) == FAILURE || php3_request_startup(CLS_C ELS_CC) == FAILURE) {
							return FAILURE;
						}
					}
					_cgi_started=1;
					_cgi_filename = estrdup(optarg);
					/* break missing intentionally */
				case 'q':
					php3_noheader();
					break;
				case 'v':
					if (!_cgi_started) {
						if (php3_module_startup(CLS_C ELS_CC) == FAILURE || php3_request_startup(CLS_C ELS_CC) == FAILURE) {
							return FAILURE;
						}
					}
					php3_printf("%s\n", PHP_VERSION);
					exit(1);
					break;
				case 'i':
					if (!_cgi_started) {
						if (php3_module_startup(CLS_C ELS_CC) == FAILURE || php3_request_startup(CLS_C ELS_CC) == FAILURE) {
							return FAILURE;
						}
					}
					_cgi_started=1;
					php3_TreatHeaders();
					_php3_info();
					exit(1);
					break;
				case 's':
					behavior=PHP_MODE_HIGHLIGHT;
					break;
				case 'n':
					behavior=PHP_MODE_INDENT;
					break;
				case 'c':
					GLOBAL(php3_ini_path) = strdup(optarg);		/* intentional leak */
					break;
				case 'a':
#if SUPPORT_INTERACTIVE
					printf("Interactive mode enabled\n\n");
					interactive=1;
#else
					printf("Interactive mode not supported!\n\n");
#endif
					break;
				case 'e':
					CG(extended_info) = 1;
					break;
				case 'h':
				case '?':
					php3_noheader();
					zend_output_startup();
					_php3_usage(argv[0]);
					exit(1);
					break;
				default:
					break;
			}
		}
	}							/* not cgi */

#if SUPPORT_INTERACTIVE
	EG(interactive) = interactive;
#endif

	if (!_cgi_started) {
		if (php3_module_startup(CLS_C ELS_CC) == FAILURE || php3_request_startup(CLS_C ELS_CC) == FAILURE) {
			return FAILURE;
		}
	}
	file_handle.filename = "-";
	file_handle.type = ZEND_HANDLE_FP;
	file_handle.handle.fp = stdin;
	if (_cgi_filename) {
		GLOBAL(request_info).filename = _cgi_filename;
	}

	php3_TreatHeaders();

	if (!cgi) {
		if (!GLOBAL(request_info).query_string) {
			for (i = optind, len = 0; i < argc; i++)
				len += strlen(argv[i]) + 1;

			s = malloc(len + 1);	/* leak - but only for command line version, so ok */
			*s = '\0';			/* we are pretending it came from the environment  */
			for (i = optind, len = 0; i < argc; i++) {
				strcat(s, argv[i]);
				if (i < (argc - 1))
					strcat(s, "+");
			}
			GLOBAL(request_info).query_string = s;
		}
		if (!GLOBAL(request_info).filename && argc > optind)
			GLOBAL(request_info).filename = argv[optind];
	}
	/* If for some reason the CGI interface is not setting the
	   PATH_TRANSLATED correctly, request_info.filename is NULL.
	   We still call php3_fopen_for_parser, because if you set doc_root
	   or user_dir configuration directives, PATH_INFO is used to construct
	   the filename as a side effect of php3_fopen_for_parser.
	 */
	if (cgi || GLOBAL(request_info).filename) {
		file_handle.filename = GLOBAL(request_info).filename;
		file_handle.handle.fp = php3_fopen_for_parser();
	}

	if (cgi && !file_handle.handle.fp) {
		PUTS("No input file specified.\n");
#if 0	/* this is here for debuging under windows */
		if (argc) {
			i = 0;
			php3_printf("\nargc %d\n",argc); 
			while (i <= argc) {
				php3_printf("%s\n",argv[i]); 
				i++;
			}
		}
#endif
		php3_request_shutdown((void *) 0 _INLINE_TLS);
		php3_module_shutdown(_INLINE_TLS_VOID);
		return FAILURE;
	} else if (file_handle.handle.fp && file_handle.handle.fp!=stdin) {
		/* #!php support */
		c = fgetc(file_handle.handle.fp);
		if (c == '#') {
			while (c != 10 && c != 13) {
				c = fgetc(file_handle.handle.fp);	/* skip to end of line */
			}
			CG(zend_lineno)++;
		} else {
			rewind(file_handle.handle.fp);
		}
		GLOBAL(initialized) |= INIT_SCANNER;
		/* This shouldn't be necessary - if it is - it should move to Zend
		 * phprestart(GLOBAL(phpin));
		 */
	}

	switch (behavior) {
		case PHP_MODE_STANDARD:
			php3_parse(&file_handle CLS_CC ELS_CC);
			break;
		case PHP_MODE_HIGHLIGHT: {
				zend_syntax_highlighter_ini syntax_highlighter_ini;

				if (open_file_for_scanning(&file_handle CLS_CC)==SUCCESS) {
					php_get_highlight_struct(&syntax_highlighter_ini);
					zend_highlight(&syntax_highlighter_ini);
					fclose(file_handle.handle.fp);
				}
				return 0;
			}
			break;
		case PHP_MODE_INDENT:
			open_file_for_scanning(&file_handle CLS_CC);
			zend_indent();
			fclose(file_handle.handle.fp);
			return 0;
			break;
	}

	if (GLOBAL(initialized)) {
		php3_header();			/* Make sure headers have been sent */
		php3_request_shutdown((void *) 0 _INLINE_TLS);
		php3_module_shutdown(_INLINE_TLS_VOID);
#ifdef THREAD_SAFE
		yy_destroy_tls();
		tls_shutdown();
		tls_destroy();
#endif
		return SUCCESS;
	} else {
		return FAILURE;
	}
}
#endif							/* CGI_BINARY */


#if APACHE
PHPAPI int apache_php3_module_main(request_rec * r, int fd, int display_source_mode)
{
	zend_file_handle file_handle;
#ifdef ZTS
	zend_compiler_globals cg;
	zend_executor_globals eg;
	zend_compiler_globals *compiler_globals=&cg;
	zend_executor_globals *executor_globals=&eg;
#endif

	GLOBAL(php3_rqst) = r;

	if (php3_request_startup(CLS_C ELS_CC) == FAILURE) {
		return FAILURE;
	}
	php3_TreatHeaders();
	file_handle.type = ZEND_HANDLE_FD;
	file_handle.handle.fd = fd;
	file_handle.filename = request_info.filename;
	GLOBAL(initialized) |= INIT_SCANNER;
	(void) php3_parse(&file_handle CLS_CC ELS_CC);
	
	if (GLOBAL(initialized)) {
		php3_header();			/* Make sure headers have been sent */
		zend_end_ob_buffering(1);
	}
	return (OK);
}
#endif							/* APACHE */

#if FHTTPD

char *get_pretokenized_name(void)
{
	char *pretokenized_name = NULL;

	if (GLOBAL(request_info).filename) {
		int length = strlen(GLOBAL(request_info).filename);

		if (length > (sizeof(".php3") - 1) && !strcmp(GLOBAL(request_info).filename + length - sizeof(".php3") + 1, ".php3")) {
			pretokenized_name = (char *) emalloc(length + 2);
			strcpy(pretokenized_name, GLOBAL(request_info).filename);
			strcat(pretokenized_name, "p");
		} else {
			length += sizeof(".php3p");
			pretokenized_name = (char *) emalloc(length + 1);
			strcpy(pretokenized_name, GLOBAL(request_info).filename);
			strcat(pretokenized_name, ".php3p");
		}
	} else {
		pretokenized_name = estrdup("stdin.php3p");
	}
	return pretokenized_name;
}


void _php3_usage(char *progname)
{
	fprintf(stderr,
	   "Usage: %s [options] [appname] [username] [hostname] [portname]\n"
			"Options:\n"
			"  -d       Daemon mode -- never attempt terminal I/O\n"
			"  -s       Socket mode, fhttpd internal use only\n"
			"  -p       Pipe mode, fhttpd internal use only\n"
			"  -u<mask> Set umask\n"
			"  -t<time> Idle timeout in seconds, 0 - disable\n"
			"  -S       Display colour syntax highlighted source\n"
			"  -P       Make and execute a pretokenized script\n"
			"           (.php3p file) or, if pretokenized script, newer\n"
			"           than original file exists, execute it instead\n"
			"  -E       Execute a pretokenized (.php3p) script\n"
			"  -c<path> Look for php3.ini file in this directory\n"
			"           (must appear before any other options)\n"
			"  -v       Version number\n"
			"  -h       This help\n",
			progname);
}

int main(int argc, char **argv)
{
	int c, i, processing_error;
	FILE *in = NULL;
	FILE *in2;
	int preprocess_mode = PREPROCESS_NONE;
	int argc1;
	char **argv1;
	int human = 1, fd2;
	int i0 = 0, i1 = 0;
	char *pn;
	struct stat statbuf, pstatbuf;
#ifdef ZTS
	zend_compiler_globals cg;
	zend_executor_globals eg;
	zend_compiler_globals *compiler_globals=&cg;
	zend_executor_globals *executor_globals=&eg;
#endif

#ifdef THREAD_SAFE
	php3_globals_struct *php3_globals;
	flex_globals *php_flex_gbl;
	tls_startup();
	tls_create();
	php_flex_gbl = yy_init_tls();
	php3_globals = TlsGetValue(TlsIndex);

	if ((php3_globals == 0) && (GetLastError() != 0)) {
		PUTS("TlsGetValue error\n");
		return FAILURE;
	}
#endif

#if HAVE_SETLOCALE
	setlocale(LC_CTYPE, "");
#endif

	if (php3_module_startup(CLS_C ELS_CC) == FAILURE) {
		return FAILURE;
	}
	signal(SIGPIPE, SIG_IGN);
	umask(077);

	while ((c = getopt(argc, argv, "spdu:t:c:PESvh")) != -1) {
		switch (c) {
			case 'd':
				human = 0;
				break;
			case 's':
				i0 = 1;
				break;
			case 'p':
				i1 = 1;
				break;
			case 'u':
				if (*optarg == '0')
					umask(strtoul(optarg, NULL, 8));
				else
					umask(strtoul(optarg, NULL, 10));
				break;
			case 't':
				idle_timeout = atoi(optarg);
				break;
			case 'c':
				GLOBAL(php3_ini_path) = strdup(optarg);		/* intentional leak */
				break;
			case 'P':			/* preprocess */
				preprocess_mode = PREPROCESS_PREPROCESS;
				break;
			case 'E':			/* execute preprocessed script */
				preprocess_mode = PREPROCESS_EXECUTE;
				break;
			case 'S':
				printf ("Not implemented yet\n");
				break;
			case 'v':
				printf("%s\n", PHP_VERSION);
				exit(1);
				break;
			case 'h':
			case ':':
			case '?':
				_php3_usage(argv[0]);
				return -1;
		}
	}

	argc1 = argc - optind;
	argv1 = (char **) malloc(sizeof(char *) * (argc1 + 2));
	if (!argv1)
		return -1;
	argv1 += 2;
	for (i = optind; i < argc; i++)
		argv1[i - optind] = argv[i];

	if (i0) {
		argv1--;
		*argv1 = "-s";
		argc1++;
	} else {
		if (i1) {
			argv1--;
			*argv1 = "-p";
			argc1++;
		}
	}
	argv1--;
	argc1++;
	*argv1 = *argv;

	server = createserver();
	if (!server)
		return -1;

	switch (servproc_init(server, human, argc1, argv1)) {
		case 0:
			break;
		case APP_ERR_HUMAN:
			_php3_usage(argv[0]);
			exit(1);
			break;
		case APP_ERR_CONFIG:
			fprintf(stderr, "%s: configuration error\n", server->app_progname);
			exit(1);
			break;
		case APP_ERR_READ:
			fprintf(stderr, "%s: read error\n", server->app_progname);
			exit(1);
			break;
		case APP_ERR_HOSTNAME:
			fprintf(stderr, "%s: can't resolve server hostname\n", server->app_progname);
			exit(1);
			break;
		case APP_ERR_SOCKET:
			fprintf(stderr, "%s: can't create socket\n", server->app_progname);
			exit(1);
			break;
		case APP_ERR_CONNECT:
			fprintf(stderr, "%s: can't connect\n", server->app_progname);
			exit(1);
			break;
		case APP_ERR_APPCONNECT:
			fprintf(stderr, "%s: connect error\n", server->app_progname);
			exit(1);
			break;
		case APP_ERR_USER:
			fprintf(stderr, "%s: login error\n", server->app_progname);
			exit(1);
			break;
		case APP_ERR_PASSWORD:
			fprintf(stderr, "%s: login error\n", server->app_progname);
			exit(1);
			break;
		case APP_ERR_APPLICATION:
			fprintf(stderr, "%s: application rejected by server\n", server->app_progname);
			exit(1);
			break;
		case APP_ERR_INSANE:
		case APP_ERR_DAEMON:
		case APP_ERR_AUTH:
		default:
			if (server->infd < 0)
				exit(1);
	}

	if (server->infd == 0 && server->outfd == 1) {
		close(2);
		fd2 = open("/dev/null", O_WRONLY);
		if (fd2 != 2) {
			dup2(fd2, 2);
			close(fd2);
		}
	}
	setcapabilities(server, APP_CAP_KEEPALIVE);

	exit_status = 0;
	while (!exit_status) {
		processing_error = 0;
		if (php3_request_startup(_INLINE_TLS_VOID) == FAILURE) {
			processing_error = 1;
		}
		if (!processing_error) {
			GLOBAL(phpin) = NULL;
			GLOBAL(current_lineno) = 0;

			php3_TreatHeaders();

			in = php3_fopen_for_parser();

			GLOBAL(php3_preprocess) = preprocess_mode;

			if (!in) {
				PUTS("No input file specified.\n");
				php3_request_shutdown((void *) 0 _INLINE_TLS);
				processing_error = 1;
			} else {
				if (GLOBAL(php3_preprocess) == PREPROCESS_PREPROCESS) {
					pn = get_pretokenized_name();
					if (pn) {
						if (!stat(pn, &pstatbuf)
							&& !fstat(fileno(in), &statbuf)
							&& S_ISREG(pstatbuf.st_mode)
							&& statbuf.st_mtime < pstatbuf.st_mtime) {
							in2 = fopen(pn, "r");
							if (in2) {
								fclose(in);
								in = in2;
								GLOBAL(php3_preprocess) = PREPROCESS_EXECUTE;
							}
						}
						efree(pn);
					}
				}
				if (GLOBAL(php3_preprocess) != PREPROCESS_EXECUTE) {
					/* #!php support */
					c = fgetc(in);
					if (c == '#') {
						while (c != 10 && c != 13) {
							c = fgetc(in);	/* skip to end of line */
						}
						CG(phplineno)++;
					} else {
						rewind(in);
					}
				}
				GLOBAL(phpin) = in;
				GLOBAL(initialized) |= INIT_SCANNER;
				phprestart(GLOBAL(phpin));

				if (!processing_error) {
					if (GLOBAL(php3_preprocess) == PREPROCESS_EXECUTE) {
						if (tcm_load(&GLOBAL(token_cache_manager), GLOBAL(phpin))==FAILURE) {
							/* should bail out on an error, don't know how to do it in fhttpd */
						}
						GLOBAL(php3_preprocess) = PREPROCESS_NONE;
					}
					if (GLOBAL(php3_preprocess)!=PREPROCESS_NONE) {
						pval yylval;

						while (phplex(&yylval));	/* create the token cache */
						tcm_save(&GLOBAL(token_cache_manager));
						seek_token(&GLOBAL(token_cache_manager), 0, NULL);
						GLOBAL(php3_preprocess) = PREPROCESS_NONE;
					}
					php3_parse(GLOBAL(phpin) CLS_CC ELS_CC);

				}
			}
		}
		if (GLOBAL(initialized)) {
			php3_header();		/* Make sure headers have been sent */
			php3_request_shutdown((void *) 0 _INLINE_TLS);
		}
	}
	php3_module_shutdown(_INLINE_TLS_VOID);
#ifdef THREAD_SAFE
	if (GLOBAL(initialized)) {
		yy_destroy_tls();
		tls_shutdown();
		tls_destroy();
	}
#endif
	return 0;
}
#endif							/* FHTTPD */

#if USE_SAPI

PHPAPI int php3_sapi_main(struct sapi_request_info *sapi_info)
{
#if DEBUG
	char logmessage[1024];
#endif
	FILE *in = NULL;
	int c;
	YY_TLS_VARS;
	TLS_VARS;

	GLOBAL(php3_preprocess) = sapi_info->preprocess;
	GLOBAL(php3_display_source) = sapi_info->display_source_mode;
	GLOBAL(sapi_rqst) = sapi_info;

#if DEBUG
	snprintf(logmessage,1024,"%d:php3_sapi_main: entry\n",GLOBAL(sapi_rqst)->scid);
	OutputDebugString(logmessage);
#endif

/*	if (php3_module_startup(php3_globals) == FAILURE) {
		return FAILURE;
	}*/

	if (php3_request_startup(CLS_C ELS_CC) == FAILURE) {
#if DEBUG
	snprintf(logmessage,1024,"%d:php3_sapi_main: request starup failed\n",GLOBAL(sapi_rqst)->scid);
	OutputDebugString(logmessage);
#endif
		return FAILURE;
	}
	if (sapi_info->preprocess == PREPROCESS_PREPROCESS || sapi_info->quiet_mode) {
		php3_noheader();
	}
	if (sapi_info->info_only) {
		_php3_info();
		php3_request_shutdown((void *) GLOBAL(sapi_rqst), php3_globals);
		/*php3_module_shutdown(php3_globals);*/
#if DEBUG
	snprintf(logmessage,1024,"%d:php3_sapi_main: info_only\n",GLOBAL(sapi_rqst)->scid);
	OutputDebugString(logmessage);
#endif
		return (1);
	}
	/* if its not cgi, require that we have a filename now */
#if DEBUG
	snprintf(logmessage,1024,"%d:php3_sapi_main: File: %s\n",GLOBAL(sapi_rqst)->scid,GLOBAL(sapi_rqst)->filename);
	OutputDebugString(logmessage);
#endif
	if (!sapi_info->cgi && !sapi_info->filename) {
		php3_printf("No input file specified.\n");
		php3_request_shutdown((void *) GLOBAL(sapi_rqst), php3_globals);
		/*php3_module_shutdown(php3_globals);*/
#if DEBUG
	snprintf(logmessage,1024,"%d:php3_sapi_main: No input file specified\n",GLOBAL(sapi_rqst)->scid);
	OutputDebugString(logmessage);
#endif
		return FAILURE;
	}
	/* 
	   if request_info.filename is null and cgi, fopen_for_parser is responsible
	   request_info.filename will only be estrduped in fopen_for parser
	   if it is null at this point
	 */
	in = php3_fopen_for_parser();

	if (sapi_info->cgi && !in) {
		php3_printf("No input file specified for cgi.\n");
		php3_request_shutdown((void *) GLOBAL(sapi_rqst), php3_globals);
		/*php3_module_shutdown(php3_globals);*/
#if DEBUG
	snprintf(logmessage,1024,"%d:php3_sapi_main: No input file specified for cgi.\n",GLOBAL(sapi_rqst)->scid);
	OutputDebugString(logmessage);
#endif
		return FAILURE;
	}
	if (sapi_info->cgi && in) {
		/* #!php support */
		c = fgetc(in);
		if (c == '#') {
			while (c != 10 && c != 13) {
				c = fgetc(in);	/* skip to end of line */
			}
		} else {
			rewind(in);
		}
	}
	if (in) {
		GLOBAL(phpin) = in;
		phprestart(GLOBAL(phpin));
		GLOBAL(initialized) |= INIT_SCANNER;
	}
	if (sapi_info->display_source_mode) {
		GLOBAL(php3_display_source) = 1;
		PUTS("<html><head><title>Source for ");
		PUTS(sapi_info->filename);
		PUTS("</title></head><body bgcolor=\"");
		PUTS(php3_ini.highlight_bg);
		PUTS("\" text=\"");
		PUTS(php3_ini.highlight_html);
		PUTS("\">\n");			/* color: seashell */
	}
	if (sapi_info->display_source_mode && sapi_info->preprocess == PREPROCESS_PREPROCESS) {
		php3_printf("Can't preprocess while displaying source.<br>\n");
		return FAILURE;
	}
	if (sapi_info->preprocess == PREPROCESS_EXECUTE) {
		tcm_load(&GLOBAL(token_cache_manager));
		GLOBAL(php3_preprocess) = PREPROCESS_NONE;
	}
	if (sapi_info->preprocess==PREPROCESS_NONE) {
#if DEBUG
	snprintf(logmessage,1024,"%d:php3_sapi_main: start php3_parse() file:%s\n",GLOBAL(sapi_rqst)->scid,GLOBAL(sapi_rqst)->filename);
	OutputDebugString(logmessage);
#endif
		php3_parse(GLOBAL(phpin) _INLINE_TLS);
#if DEBUG
	snprintf(logmessage,1024,"%d:php3_sapi_main: done php3_parse()\n",GLOBAL(sapi_rqst)->scid);
	OutputDebugString(logmessage);
#endif
	} else {
		pval yylval;

#if DEBUG
	snprintf(logmessage,1024,"%d:php3_sapi_main: entering phplex()\n",GLOBAL(sapi_rqst)->scid);
	OutputDebugString(logmessage);
#endif
#ifdef THREAD_SAFE
		while (phplex(&yylval, php3_globals, php_gbl));		/* create the token cache */
#else
		while (phplex(&yylval));	/* create the token cache */
#endif
#if DEBUG
	snprintf(logmessage,1024,"%d:php3_sapi_main: done phplex()\n",GLOBAL(sapi_rqst)->scid);
	OutputDebugString(logmessage);
#endif
		tcm_save(&GLOBAL(token_cache_manager));
	}

	if (sapi_info->display_source_mode) {
		php3_printf("\n</html>\n");
	}
	if (GLOBAL(initialized)) {
		php3_header();			/* Make sure headers have been sent */
		php3_request_shutdown((void *) GLOBAL(sapi_rqst), php3_globals);
		/*php3_module_shutdown(php3_globals);*/
#if DEBUG
	snprintf(logmessage,1024,"%d:php3_sapi_main: success!\n",GLOBAL(sapi_rqst)->scid);
	OutputDebugString(logmessage);
#endif
		return SUCCESS;
	} else {
#if DEBUG
	snprintf(logmessage,1024,"%d:php3_sapi_main: request not initialized!\n",GLOBAL(sapi_rqst)->scid);
	OutputDebugString(logmessage);
#endif
		return FAILURE;
	}
}
#if WIN32|WINNT
extern int tls_create(void);
extern int tls_destroy(void);
extern int tls_startup(void);
extern int tls_shutdown(void);
extern flex_globals *yy_init_tls(void);
extern void yy_destroy_tls(void);
extern VOID ErrorExit(LPTSTR lpszMessage);

BOOL WINAPI DllMain(HANDLE hModule,
					DWORD ul_reason_for_call,
					LPVOID lpReserved)
{
	php3_globals_struct *php3_globals;
#if DEBUG
	OutputDebugString("PHP_Core DllMain Entry\n");
#endif
	switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH:
			/* 
			   I should be loading ini vars here 
			   and doing whatever true global inits
			   need to be done
			 */
			_fmode = _O_BINARY;	/*sets default for file streams to binary */
			/* make the stdio mode be binary */
			setmode(_fileno(stdin), O_BINARY);
			setmode(_fileno(stdout), O_BINARY);
			setmode(_fileno(stderr), O_BINARY);
			setlocale(LC_CTYPE, "");

			CREATE_MUTEX(gLock, "GENERAL");

			if (!tls_startup())
				return 0;
			if (!tls_create())
				return 0;
			php3_globals = TlsGetValue(TlsIndex);
			yy_init_tls();
			if (php3_config_ini_startup(_INLINE_TLS_VOID) == FAILURE) {
				return 0;
			}
			if (php3_module_startup(php3_globals) == FAILURE) {
				ErrorExit("module startup failed");
				return 0;
			}

#if DEBUG
	OutputDebugString("PHP_Core DllMain Process Attached\n");
#endif
			break;
		case DLL_THREAD_ATTACH:
#if DEBUG
	OutputDebugString("PHP_Core DllMain Thread Attach\n");
#endif
			if (!tls_create())
				return 0;
			php3_globals = TlsGetValue(TlsIndex);
			yy_init_tls();
			if (php3_module_startup(php3_globals) == FAILURE) {
				ErrorExit("module startup failed");
#if DEBUG
	OutputDebugString("PHP_Core DllMain module startup failed\n");
#endif
				return 0;
			}
			break;
		case DLL_THREAD_DETACH:
#if DEBUG
	OutputDebugString("PHP_Core DllMain Detache\n");
#endif
			php3_globals = TlsGetValue(TlsIndex);
			php3_module_shutdown(php3_globals);
			if (!tls_destroy())
#if DEBUG
	OutputDebugString("PHP_Core DllMain Detache Error tls_destroy\n");
#endif
				return 0;
			yy_destroy_tls();
			break;
		case DLL_PROCESS_DETACH:
			/*
			   close down anything down in process_attach 
			 */
			php3_globals = TlsGetValue(TlsIndex);
			php3_module_shutdown(php3_globals);

			php3_config_ini_shutdown(_INLINE_TLS_VOID);

			if (!tls_destroy())
#if DEBUG
	OutputDebugString("PHP_Core DllMain tls_destroy failed\n");
#endif
				return 0;
			if (!tls_shutdown())
#if DEBUG
	OutputDebugString("PHP_Core DllMain tls_shutdown failed\n");
#endif
				return 0;
			yy_destroy_tls();
#if DEBUG
	OutputDebugString("PHP_Core DllMain Process Detatched\n");
#endif
			break;
	}
#if DEBUG
	OutputDebugString("PHP_Core DllMain Successful Exit\n");
#endif
	return 1;
}

#endif
#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
