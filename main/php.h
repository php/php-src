/* 
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999 The PHP Group                         |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef _PHP_H
#define _PHP_H

#ifdef HAVE_DMALLOC
#include <dmalloc.h>
#endif

#define PHP_API_VERSION 19990421

#define YYDEBUG 0

#define CGI_BINARY (!APACHE && !USE_SAPI && !FHTTPD)

#include "php_version.h"
#include "zend.h"

/* automake defines PACKAGE and VERSION for Zend too */
#ifdef PACKAGE
# undef PACKAGE
#endif
#ifdef VERSION
# undef VERSION
#endif

#include "zend_API.h"


extern unsigned char first_arg_force_ref[];   
extern unsigned char first_arg_allow_ref[];
extern unsigned char second_arg_force_ref[];
extern unsigned char second_arg_allow_ref[];


/* somebody stealing BOOL from windows.  pick something else!
#ifndef BOOL
#define BOOL MYBOOL
#endif
*/


#if WIN32
#include "config.w32.h"
#include "win95nt.h"
#	ifdef PHP_EXPORTS
#	define PHPAPI __declspec(dllexport) 
#	else
#	define PHPAPI __declspec(dllimport) 
#	endif
#else
#include "php_config.h"
#define PHPAPI
#define THREAD_LS
#endif


/* PHP's DEBUG value must match Zend's ZEND_DEBUG value */
#undef DEBUG
#define DEBUG ZEND_DEBUG


#if DEBUG || !(defined(__GNUC__)||defined(WIN32))
#ifdef inline
#undef inline
#endif
#define inline
#endif


#if HAVE_UNIX_H
#include <unix.h>
#endif

#if HAVE_ALLOCA_H
#include <alloca.h>
#endif

#include "request_info.h"

#if HAVE_LIBDL
# if MSVC5
#  include <windows.h>
#  define dlclose FreeLibrary
#  define dlopen(a,b) LoadLibrary(a)
#  define dlsym GetProcAddress
# else
#if HAVE_DLFCN_H && !((defined(_AIX) || defined(AIX)) && APACHE)
#  include <dlfcn.h>
#endif
# endif
#endif

#define CREATE_MUTEX(a,b)
#define SET_MUTEX(a)
#define FREE_MUTEX(a)

/*
 * Then the ODBC support can use both iodbc and Solid,
 * uncomment this.
 * #define HAVE_ODBC (HAVE_IODBC|HAVE_SOLID)
 */

#include <stdlib.h>
#include <ctype.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_STDARG_H
#include <stdarg.h>
#else   
# if HAVE_SYS_VARARGS_H
# include <sys/varargs.h>
# endif 
#endif 


#include "zend_hash.h"
#include "php3_compat.h"
#include "zend_alloc.h"
#include "zend_stack.h"

typedef zval pval;

#define pval_copy_constructor	zval_copy_ctor
#define pval_destructor			zval_dtor

#if STDC_HEADERS
# include <string.h>
#else
# ifndef HAVE_MEMCPY
#  define memcpy(d, s, n)	bcopy((s), (d), (n))
#  define memmove(d, s, n)	bcopy ((s), (d), (n))
# endif
#endif

#include "safe_mode.h"

#ifndef HAVE_STRERROR
extern char *strerror(int);
#endif

#include "fopen-wrappers.h"

#if APACHE /* apache httpd */
# if HAVE_AP_CONFIG_H
#include "ap_config_auto.h"
#ifdef RHAPSODY
#undef HAVE_SNPRINTF
#endif
#include "ap_config.h"
#ifdef RHAPSODY
#undef HAVE_SNPRINTF
#define HAVE_SNPRINTF 1
#endif
# endif
# if HAVE_OLD_COMPAT_H
#include "compat.h"
# endif
# if HAVE_AP_COMPAT_H
#include "ap_compat.h"
# endif
#include "httpd.h"
#include "http_main.h"
#include "http_core.h"
#include "http_request.h"
#include "http_protocol.h"
#include "http_config.h"
#include "http_log.h"
#define BLOCK_INTERRUPTIONS block_alarms
#define UNBLOCK_INTERRUPTIONS unblock_alarms
#endif

#if HAVE_PWD_H
# if WIN32||WINNT
#include "win32/pwd.h"
#include "win32/param.h"
# else
#include <pwd.h>
#include <sys/param.h>
# endif
#endif
#if CGI_BINARY /* CGI version */
#define BLOCK_INTERRUPTIONS		NULL
#define UNBLOCK_INTERRUPTIONS	NULL
#endif

#if HAVE_LIMITS_H
#include <limits.h>
#endif

#ifndef LONG_MAX
#define LONG_MAX 2147483647L
#endif

#ifndef LONG_MIN
#define LONG_MIN (- LONG_MAX - 1)
#endif

#if (!HAVE_SNPRINTF)
#define snprintf ap_snprintf
#define vsnprintf ap_vsnprintf
extern int ap_snprintf(char *, size_t, const char *, ...);
extern int ap_vsnprintf(char *, size_t, const char *, va_list);
#endif

#define EXEC_INPUT_BUF 4096


#define DONT_FREE 0
#define DO_FREE 1

#define PHP3_MIME_TYPE "application/x-httpd-php3"

/* macros */
#undef MIN
#undef MAX
#undef COPY_STRING
#define DO_OR_DIE(retvalue) if (retvalue==FAILURE) { return FAILURE; }
#define MAX(a,b)  (((a)>(b))?(a):(b))
#define MIN(a,b)  (((a)<(b))?(a):(b))
#define STR_FREE(ptr) if (ptr && ptr!=empty_string && ptr!=undefined_variable_string) { efree(ptr); }
#define COPY_STRING(yy)   (yy).value.str.val = (char *) estrndup((yy).value.str.val,(yy).value.str.len)
#define STR_PRINT(str)	((str)?(str):"")

#ifndef MAXPATHLEN
#define MAXPATHLEN 256    /* Should be safe for any weird systems that do not define it */
#endif

#define PHP_FN(name) php3_##name
#define PHP_NAMED_FUNCTION(name) void name(INTERNAL_FUNCTION_PARAMETERS)
#define PHP_FUNCTION(name) PHP_NAMED_FUNCTION(php3_##name)

#define PHP_NAMED_FE(php_name, name, arg_types) { #php_name, name, arg_types },
#define PHP_FE(name, arg_types) PHP_NAMED_FE(name, php3_##name, arg_types)
#define PHP_FALIAS(name, alias, arg_types) PHP_NAMED_FE(name, php3_##alias, arg_types)

#define PHP_MINIT(module)	php3_minit_##module
#define PHP_MSHUTDOWN(module)	php3_mshutdown_##module
#define PHP_RINIT(module)	php3_rinit_##module
#define PHP_RSHUTDOWN(module)	php3_rshutdown_##module
#define PHP_MINFO(module)	php3_info_##module
#define PHP_GINIT(module)		php3_ginit_##module
#define PHP_GSHUTDOWN(module)	php3_gshutdown_##module

#define PHP_MINIT_FUNCTION(module)	int PHP_MINIT(module)(INIT_FUNC_ARGS)
#define PHP_MSHUTDOWN_FUNCTION(module)	int PHP_MSHUTDOWN(module)(SHUTDOWN_FUNC_ARGS)
#define PHP_RINIT_FUNCTION(module)	int PHP_RINIT(module)(INIT_FUNC_ARGS)
#define PHP_RSHUTDOWN_FUNCTION(module)	int PHP_RSHUTDOWN(module)(SHUTDOWN_FUNC_ARGS)
#define PHP_MINFO_FUNCTION(module)	void PHP_MINFO(module)(ZEND_MODULE_INFO_FUNC_ARGS)
#define PHP_GINIT_FUNCTION(module)	static int PHP_GINIT(module)(void)
#define PHP_GSHUTDOWN_FUNCTION(module)	static int PHP_GSHUTDOWN(module)(void)


/* global variables */
extern pval *data;
#if !(WIN32||WINNT)
extern char **environ;
#endif

extern void phperror(char *error);
extern PHPAPI void php_error(int type, const char *format,...);
extern PHPAPI int php3_write(void *buf, int size);
extern PHPAPI int php_printf(const char *format,...);
extern void php3_log_err(char *log_message);
extern int Debug(char *format,...);
extern int cfgparse(void);

extern void html_putc(char c);

#define zenderror phperror
#define zendlex phplex

#define phpparse zendparse
#define phprestart zendrestart
#define phpin zendin

/* functions */
int module_startup_modules(void);
int module_global_startup_modules(void);
int module_global_shutdown_modules(void);

/* needed for modules only */
extern PHPAPI int php3i_get_le_fp(void);

/*from basic functions*/
extern PHPAPI int _php_error_log(int opt_err,char *message,char *opt,char *headers);

PHPAPI int cfg_get_long(char *varname, long *result);
PHPAPI int cfg_get_double(char *varname, double *result);
PHPAPI int cfg_get_string(char *varname, char **result);


/* Output support */
#include "output.h"
#define PHPWRITE(str, str_len)		php_body_write((str), (str_len))
#define PUTS(str)					php_body_write((str), strlen((str)))
#define PUTC(c)						(php_body_write(&(c), 1), (c))
#define PHPWRITE_H(str, str_len)	php_header_write((str), (str_len))
#define PUTS_H(str)					php_header_write((str), strlen((str)))
#define PUTC_H(c)					(php_header_write(&(c), 1), (c))


#include "zend_operators.h"
#include "zend_variables.h"
#include "zend_constants.h"

/* connection status states */
#define PHP_CONNECTION_NORMAL  0
#define PHP_CONNECTION_ABORTED 1
#define PHP_CONNECTION_TIMEOUT 2


/* Finding offsets of elements within structures.
 * Taken from the Apache code, which in turn, was taken from X code...
 */

#if defined(CRAY) || (defined(__arm) && !defined(LINUX))
#ifdef __STDC__
#define XtOffset(p_type,field) _Offsetof(p_type,field)
#else
#ifdef CRAY2
#define XtOffset(p_type,field) \
    (sizeof(int)*((unsigned int)&(((p_type)NULL)->field)))

#else /* !CRAY2 */

#define XtOffset(p_type,field) ((unsigned int)&(((p_type)NULL)->field))

#endif /* !CRAY2 */  
#endif /* __STDC__ */
#else /* ! (CRAY || __arm) */

#define XtOffset(p_type,field) \
    ((long) (((char *) (&(((p_type)NULL)->field))) - ((char *) NULL)))

#endif /* !CRAY */

#ifdef offsetof
#define XtOffsetOf(s_type,field) offsetof(s_type,field)
#else
#define XtOffsetOf(s_type,field) XtOffset(s_type*,field)
#endif

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
