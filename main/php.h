/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
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

#define PHP_API_VERSION 20020918
#define PHP_HAVE_STREAMS
#define YYDEBUG 0

#include "php_version.h"
#include "zend.h"
#include "zend_qsort.h"
#include "php_compat.h"

#include "zend_API.h"

#if PHP_BROKEN_SPRINTF
#undef sprintf
#define sprintf php_sprintf
#endif

/* PHP's DEBUG value must match Zend's ZEND_DEBUG value */
#undef PHP_DEBUG
#define PHP_DEBUG ZEND_DEBUG

#ifdef PHP_WIN32
#include "tsrm_win32.h"
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

#ifdef NETWARE
/* For php_get_uname() function */
#define PHP_UNAME  "NetWare"
/*
 * This is obtained using uname(2) on Unix and assigned in the case of Windows;
 * we'll do it this way at least for now.
 */
#define PHP_OS      PHP_UNAME
#endif

#include "php_regex.h"

#if HAVE_ASSERT_H
#if PHP_DEBUG
#undef NDEBUG
#else
#ifndef NDEBUG
#define NDEBUG
#endif
#endif
#include <assert.h>
#else /* HAVE_ASSERT_H */
#define assert(expr) ((void) (0))
#endif /* HAVE_ASSERT_H */

#define APACHE 0

#if HAVE_UNIX_H
#include <unix.h>
#endif

#if HAVE_ALLOCA_H
#include <alloca.h>
#endif

#if HAVE_BUILD_DEFS_H
#include "build-defs.h"
#endif

/*
 * This is a fast version of strlcpy which should be used, if you
 * know the size of the destination buffer and if you know
 * the length of the source string.
 *
 * size is the allocated number of bytes of dst
 * src_size is the number of bytes excluding the NUL of src
 */

#define PHP_STRLCPY(dst, src, size, src_size)	\
	{											\
		size_t php_str_len;						\
												\
		if (src_size >= size)					\
			php_str_len = size - 1;				\
		else									\
			php_str_len = src_size;				\
		memcpy(dst, src, php_str_len);			\
		dst[php_str_len] = '\0';				\
	}

#ifndef HAVE_STRLCPY
PHPAPI size_t php_strlcpy(char *dst, const char *src, size_t siz);
#define strlcpy php_strlcpy
#endif

#ifndef HAVE_STRLCAT
PHPAPI size_t php_strlcat(char *dst, const char *src, size_t siz);
#define strlcat php_strlcat
#endif

#ifndef HAVE_STRTOK_R
char *strtok_r(char *s, const char *delim, char **last);
#endif

#ifndef HAVE_SOCKLEN_T
typedef unsigned int socklen_t;
#endif

#define CREATE_MUTEX(a, b)
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

#if (REGEX == 1 || REGEX == 0) && !defined(NO_REGEX_EXTRA_H)
#include "regex/regex_extra.h"
#endif

#if HAVE_PWD_H
# ifdef PHP_WIN32
#include "win32/pwd.h"
#include "win32/param.h"
#elif defined(NETWARE)
#ifdef NEW_LIBC
#include <sys/param.h>
#else
#include "NetWare/param.h"
#endif
#include "NetWare/pwd.h"
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

#if !defined(HAVE_SNPRINTF) || !defined(HAVE_VSNPRINTF) || PHP_BROKEN_SPRINTF || PHP_BROKEN_SNPRINTF || PHP_BROKEN_VSNPRINTF
#include "snprintf.h"
#endif
#include "spprintf.h"

#define EXEC_INPUT_BUF 4096

#define PHP_MIME_TYPE "application/x-httpd-php"

/* macros */
#define STR_PRINT(str)	((str)?(str):"")

#ifndef MAXPATHLEN
# ifdef PATH_MAX
#  define MAXPATHLEN PATH_MAX
# else
#  define MAXPATHLEN 256    /* Should be safe for any weird systems that do not define it */
# endif
#endif


/* global variables */
extern pval *data;
#if !defined(PHP_WIN32)
#ifdef NETWARE
#ifdef NEW_LIBC
/*#undef environ*/  /* For now, so that our 'environ' implementation is used */
#define php_sleep sleep
#else
#define php_sleep   delay   /* sleep() and usleep() are not available */
#define usleep      delay
#endif
extern char **environ;
#else
extern char **environ;
#define php_sleep sleep
#endif
#endif

#ifdef PHP_PWRITE_64
ssize_t pwrite(int, void *, size_t, off64_t);
#endif

#ifdef PHP_PREAD_64
ssize_t pread(int, void *, size_t, off64_t);
#endif

void phperror(char *error);
PHPAPI int php_write(void *buf, uint size TSRMLS_DC);
PHPAPI int php_printf(const char *format, ...);
PHPAPI void php_log_err(char *log_message TSRMLS_DC);
int Debug(char *format, ...);
int cfgparse(void);

#define php_error zend_error

PHPAPI void php_verror(const char *docref, const char *params, int type, const char *format, va_list args TSRMLS_DC) ;

/* PHPAPI void php_error(int type, const char *format, ...); */
PHPAPI void php_error_docref0(const char *docref TSRMLS_DC, int type, const char *format, ...);
PHPAPI void php_error_docref1(const char *docref TSRMLS_DC, const char *param1, int type, const char *format, ...);
PHPAPI void php_error_docref2(const char *docref TSRMLS_DC, const char *param1, const char *param2, int type, const char *format, ...);

#define php_error_docref php_error_docref0

#define zenderror phperror
#define zendlex phplex

#define phpparse zendparse
#define phprestart zendrestart
#define phpin zendin

/* functions */
int php_startup_internal_extensions(void);

int php_mergesort(void *base, size_t nmemb, register size_t size, int (*cmp)(const void *, const void * TSRMLS_DC) TSRMLS_DC);

PHPAPI void php_register_pre_request_shutdown(void (*func)(void *), void *userdata);

PHPAPI int cfg_get_long(char *varname, long *result);
PHPAPI int cfg_get_double(char *varname, double *result);
PHPAPI int cfg_get_string(char *varname, char **result);


/* PHP-named Zend macro wrappers */
#define PHP_FN					ZEND_FN
#define PHP_NAMED_FUNCTION		ZEND_NAMED_FUNCTION
#define PHP_FUNCTION			ZEND_FUNCTION

#define PHP_NAMED_FE	ZEND_NAMED_FE
#define PHP_FE			ZEND_FE
#define PHP_FALIAS		ZEND_FALIAS
#define PHP_STATIC_FE	ZEND_STATIC_FE

#define PHP_MODULE_STARTUP_N	ZEND_MODULE_STARTUP_N
#define PHP_MODULE_SHUTDOWN_N	ZEND_MODULE_SHUTDOWN_N
#define PHP_MODULE_ACTIVATE_N	ZEND_MODULE_ACTIVATE_N
#define PHP_MODULE_DEACTIVATE_N	ZEND_MODULE_DEACTIVATE_N
#define PHP_MODULE_INFO_N		ZEND_MODULE_INFO_N

#define PHP_MODULE_STARTUP_D	ZEND_MODULE_STARTUP_D
#define PHP_MODULE_SHUTDOWN_D	ZEND_MODULE_SHUTDOWN_D
#define PHP_MODULE_ACTIVATE_D	ZEND_MODULE_ACTIVATE_D
#define PHP_MODULE_DEACTIVATE_D	ZEND_MODULE_DEACTIVATE_D
#define PHP_MODULE_INFO_D		ZEND_MODULE_INFO_D

/* Compatibility macros */
#define PHP_MINIT		ZEND_MODULE_STARTUP_N
#define PHP_MSHUTDOWN	ZEND_MODULE_SHUTDOWN_N
#define PHP_RINIT		ZEND_MODULE_ACTIVATE_N
#define PHP_RSHUTDOWN	ZEND_MODULE_DEACTIVATE_N
#define PHP_MINFO		ZEND_MODULE_INFO_N

#define PHP_MINIT_FUNCTION		ZEND_MODULE_STARTUP_D
#define PHP_MSHUTDOWN_FUNCTION	ZEND_MODULE_SHUTDOWN_D
#define PHP_RINIT_FUNCTION		ZEND_MODULE_ACTIVATE_D
#define PHP_RSHUTDOWN_FUNCTION	ZEND_MODULE_DEACTIVATE_D
#define PHP_MINFO_FUNCTION		ZEND_MODULE_INFO_D


/* Output support */
#include "main/php_output.h"
#define PHPWRITE(str, str_len)		php_body_write((str), (str_len) TSRMLS_CC)
#define PUTS(str)					php_body_write((str), strlen((str)) TSRMLS_CC)
#define PUTC(c)						(php_body_write(&(c), 1 TSRMLS_CC), (c))
#define PHPWRITE_H(str, str_len)	php_header_write((str), (str_len) TSRMLS_CC)
#define PUTS_H(str)					php_header_write((str), strlen((str)) TSRMLS_CC)
#define PUTC_H(c)					(php_header_write(&(c), 1 TSRMLS_CC), (c))

#ifdef ZTS
#define VIRTUAL_DIR
#endif

#include "php_streams.h"
#include "php_memory_streams.h"
#include "fopen_wrappers.h"


/* Virtual current working directory support */
#include "tsrm_virtual_cwd.h"

#include "zend_constants.h"

/* connection status states */
#define PHP_CONNECTION_NORMAL  0
#define PHP_CONNECTION_ABORTED 1
#define PHP_CONNECTION_TIMEOUT 2

#include "php_reentrancy.h"

/* Finding offsets of elements within structures.
 * Taken from the Apache code, which in turn, was taken from X code...
 */

#ifndef XtOffset
#if defined(CRAY) || (defined(__arm) && !defined(LINUX))
#ifdef __STDC__
#define XtOffset(p_type, field) _Offsetof(p_type, field)
#else
#ifdef CRAY2
#define XtOffset(p_type, field) \
    (sizeof(int)*((unsigned int)&(((p_type)NULL)->field)))

#else /* !CRAY2 */

#define XtOffset(p_type, field) ((unsigned int)&(((p_type)NULL)->field))

#endif /* !CRAY2 */
#endif /* __STDC__ */
#else /* ! (CRAY || __arm) */

#define XtOffset(p_type, field) \
    ((long) (((char *) (&(((p_type)NULL)->field))) - ((char *) NULL)))

#endif /* !CRAY */
#endif /* ! XtOffset */

#ifndef XtOffsetOf
#ifdef offsetof
#define XtOffsetOf(s_type, field) offsetof(s_type, field)
#else
#define XtOffsetOf(s_type, field) XtOffset(s_type*, field)
#endif
#endif /* !XtOffsetOf */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
