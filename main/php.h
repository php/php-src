/* 
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef PHP_H
#define PHP_H

#ifdef HAVE_DMALLOC
#include <dmalloc.h>
#endif

#define PHP_API_VERSION 19990421

#define YYDEBUG 0

#include "php_version.h"
#include "zend.h"
#include "php_compat.h"

/* automake defines PACKAGE and VERSION for Zend too */
#undef PACKAGE
#undef VERSION

#include "zend_API.h"

#if PHP_BROKEN_SPRINTF
#undef sprintf
#define sprintf php_sprintf
#endif

extern unsigned char first_arg_force_ref[];   
extern unsigned char first_arg_allow_ref[];
extern unsigned char second_arg_force_ref[];
extern unsigned char second_arg_allow_ref[];


/* somebody stealing BOOL from windows.  pick something else!
#ifndef BOOL
#define BOOL MYBOOL
#endif
*/


#ifdef PHP_WIN32
#include "win95nt.h"
#	ifdef PHP_EXPORTS
#	define PHPAPI __declspec(dllexport) 
#	else
#	define PHPAPI __declspec(dllimport) 
#	endif
#define PHP_DIR_SEPARATOR '\\'
#else
#define PHPAPI
#define THREAD_LS
#define PHP_DIR_SEPARATOR '/'
#endif

#include "php_regex.h"

/* PHP's DEBUG value must match Zend's ZEND_DEBUG value */
#undef PHP_DEBUG
#define PHP_DEBUG ZEND_DEBUG


#if PHP_DEBUG || !(defined(__GNUC__)||defined(PHP_WIN32))
#ifdef inline
#undef inline
#endif
#define inline
#endif

#define APACHE 0
#define CGI_BINARY 0

#if HAVE_UNIX_H
#include <unix.h>
#endif

#if HAVE_ALLOCA_H
#include <alloca.h>
#endif

#ifndef HAVE_STRLCPY
PHPAPI size_t strlcpy(char *dst, const char *src, size_t siz);
#endif

#ifndef HAVE_STRLCAT
PHPAPI size_t strlcat(char *dst, const char *src, size_t siz);
#endif

#ifndef HAVE_STRTOK_R
char *strtok_r(char *s, const char *delim, char **last);
#endif

#ifndef HAVE_SOCKLEN_T
typedef unsigned int socklen_t;
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

#if STDC_HEADERS
# include <string.h>
#else
# ifndef HAVE_MEMCPY
#  define memcpy(d, s, n)	bcopy((s), (d), (n))
# endif
# ifndef HAVE_MEMMOVE
#  define memmove(d, s, n)	bcopy ((s), (d), (n))
# endif
#endif

#include "safe_mode.h"

#ifndef HAVE_STRERROR
char *strerror(int);
#endif

#include "fopen-wrappers.h"

#if (REGEX == 1 || REGEX == 0) && !defined(NO_REGEX_EXTRA_H)
#include "regex/regex_extra.h"
#endif

#if HAVE_PWD_H
# ifdef PHP_WIN32
#include "win32/pwd.h"
#include "win32/param.h"
# else
#include <pwd.h>
#include <sys/param.h>
# endif
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

#if !defined(HAVE_SNPRINTF) || !defined(HAVE_VSNPRINTF) || defined(BROKEN_SPRINTF)
#include "snprintf.h"
#endif

#define EXEC_INPUT_BUF 4096


#define DONT_FREE 0
#define DO_FREE 1

#define PHP_MIME_TYPE "application/x-httpd-php"

/* macros */
#undef COPY_STRING
#define COPY_STRING(yy)   (yy).value.str.val = (char *) estrndup((yy).value.str.val,(yy).value.str.len)
#define STR_PRINT(str)	((str)?(str):"")

#ifndef MAXPATHLEN
#define MAXPATHLEN 256    /* Should be safe for any weird systems that do not define it */
#endif

#define PHP_FN(name) php_if_##name
#define PHP_NAMED_FUNCTION(name) void name(INTERNAL_FUNCTION_PARAMETERS)
#define PHP_FUNCTION(name) PHP_NAMED_FUNCTION(PHP_FN(name))

#define PHP_NAMED_FE(php_name, name, arg_types) { #php_name, name, arg_types },
#define PHP_FE(name, arg_types) PHP_NAMED_FE(name, PHP_FN(name), arg_types)
#define PHP_FALIAS(name, alias, arg_types) PHP_NAMED_FE(name, PHP_FN(alias), arg_types)

#define PHP_MINIT(module)	php_minit_##module
#define PHP_MSHUTDOWN(module)	php_mshutdown_##module
#define PHP_RINIT(module)	php_rinit_##module
#define PHP_RSHUTDOWN(module)	php_rshutdown_##module
#define PHP_MINFO(module)	php_info_##module
#define PHP_GINIT(module)	php_ginit_##module
#define PHP_GSHUTDOWN(module)	php_gshutdown_##module

#define PHP_MINIT_FUNCTION(module)	int PHP_MINIT(module)(INIT_FUNC_ARGS)
#define PHP_MSHUTDOWN_FUNCTION(module)	int PHP_MSHUTDOWN(module)(SHUTDOWN_FUNC_ARGS)
#define PHP_RINIT_FUNCTION(module)	int PHP_RINIT(module)(INIT_FUNC_ARGS)
#define PHP_RSHUTDOWN_FUNCTION(module)	int PHP_RSHUTDOWN(module)(SHUTDOWN_FUNC_ARGS)
#define PHP_MINFO_FUNCTION(module)	void PHP_MINFO(module)(ZEND_MODULE_INFO_FUNC_ARGS)
#define PHP_GINIT_FUNCTION(module)	int PHP_GINIT(module)(GINIT_FUNC_ARGS)
#define PHP_GSHUTDOWN_FUNCTION(module)	int PHP_GSHUTDOWN(module)(void)


/* global variables */
extern pval *data;
#if !defined(PHP_WIN32)
extern char **environ;
#define php_sleep sleep
#endif

void phperror(char *error);
PHPAPI int php_write(void *buf, uint size);
PHPAPI int php_printf(const char *format, ...);
void php_log_err(char *log_message);
int Debug(char *format, ...);
int cfgparse(void);

#define php_error zend_error


#define zenderror phperror
#define zendlex phplex

#define phpparse zendparse
#define phprestart zendrestart
#define phpin zendin

/* functions */
int php_startup_internal_extensions(void);
int php_global_startup_internal_extensions(void);
int php_global_shutdown_internal_extensions(void);

int mergesort(void *base, size_t nmemb, register size_t size, int (*cmp) (const void *, const void *));

PHPAPI void php_register_pre_request_shutdown(void (*func)(void *), void *userdata);

PHPAPI int cfg_get_long(char *varname, long *result);
PHPAPI int cfg_get_double(char *varname, double *result);
PHPAPI int cfg_get_string(char *varname, char **result);


/* Output support */
#include "ext/standard/php_output.h"
#define PHPWRITE(str, str_len)		php_body_write((str), (str_len))
#define PUTS(str)					php_body_write((str), strlen((str)))
#define PUTC(c)						(php_body_write(&(c), 1), (c))
#define PHPWRITE_H(str, str_len)	php_header_write((str), (str_len))
#define PUTS_H(str)					php_header_write((str), strlen((str)))
#define PUTC_H(c)					(php_header_write(&(c), 1), (c))

#ifdef ZTS
#define VIRTUAL_DIR
#endif

#include "php_virtual_cwd.h"

/* Virtual current directory support */
#ifdef VIRTUAL_DIR

#define V_GETCWD(buff, size) virtual_getcwd(buff,size)
#define V_FOPEN(path, mode) virtual_fopen(path, mode)
/* The V_OPEN macro will need to be used as V_OPEN((path, flags, ...)) */
#define V_OPEN(open_args) virtual_open open_args
#define V_CREAT(path, mode) virtual_creat(path, mode)
#define V_CHDIR(path) virtual_chdir(path)
#define V_CHDIR_FILE(path) virtual_chdir_file(path)
#define V_GETWD(buf)
#define V_REALPATH(path,real_path) virtual_realpath(path,real_path)
#define V_STAT(path, buff) virtual_stat(path, buff)
#ifdef PHP_WIN32
#define V_LSTAT(path, buff) virtual_stat(path, buff)
#else
#define V_LSTAT(path, buff) virtual_lstat(path, buff)
#endif
#define V_UNLINK(path) virtual_unlink(path)
#define V_MKDIR(pathname, mode) virtual_mkdir(pathname, mode)
#define V_RMDIR(pathname) virtual_rmdir(pathname)
#define V_OPENDIR(pathname) virtual_opendir(pathname)
#define V_POPEN(command, type) virtual_popen(command, type)
#if HAVE_UTIME
#define V_UTIME(path,time) virtual_utime(path,time)
#endif
#define V_CHMOD(path,mode) virtual_chmod(path,mode)
#ifndef PHP_WIN32
#define V_CHOWN(path,owner,group) virtual_chown(path,owner,group)
#endif
#else

#define V_GETCWD(buff, size) getcwd(buff,size)
#define V_FOPEN(path, mode)  fopen(path, mode)
#define V_OPEN(open_args) open open_args
#define V_CREAT(path, mode) creat(path, mode)
#define V_CHDIR(path) chdir(path)
#define V_CHDIR_FILE(path) virtual_real_chdir_file(path)
#define V_GETWD(buf) getwd(buf)
#define V_STAT(path, buff) stat(path, buff)
#define V_LSTAT(path, buff) lstat(path, buff)
#define V_UNLINK(path) unlink(path)
#define V_MKDIR(pathname, mode) mkdir(pathname, mode)
#define V_RMDIR(pathname) rmdir(pathname)
#define V_OPENDIR(pathname) opendir(pathname)
#define V_POPEN(command, type) popen(command, type)
#define V_REALPATH(path,real_path) realpath(path,real_path)
#if HAVE_UTIME
#define V_UTIME(path,time) utime(path,time)
#endif
#define V_CHMOD(path,mode) chmod(path,mode)
#ifndef PHP_WIN32
#define V_CHOWN(path,owner,group) chown(path,owner,group)
#endif
#endif

#include "zend_constants.h"

/* connection status states */
#define PHP_CONNECTION_NORMAL  0
#define PHP_CONNECTION_ABORTED 1
#define PHP_CONNECTION_TIMEOUT 2

#include "php_reentrancy.h"

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

PHPAPI PHP_FUNCTION(warn_not_available);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
