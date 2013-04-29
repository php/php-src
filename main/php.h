/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
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
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef PHP_H
#define PHP_H

#ifdef HAVE_DMALLOC
#include <dmalloc.h>
#endif

#define PHP_API_VERSION 20121113
#define PHP_HAVE_STREAMS
#define YYDEBUG 0

#include "php_version.h"
#include "zend.h"
#include "zend_qsort.h"
#include "php_compat.h"

#include "zend_API.h"

#undef sprintf
#define sprintf php_sprintf

/* PHP's DEBUG value must match Zend's ZEND_DEBUG value */
#undef PHP_DEBUG
#define PHP_DEBUG ZEND_DEBUG

#ifdef PHP_WIN32
#	include "tsrm_win32.h"
#	include "win95nt.h"
#	ifdef PHP_EXPORTS
#		define PHPAPI __declspec(dllexport)
#	else
#		define PHPAPI __declspec(dllimport)
#	endif
#	define PHP_DIR_SEPARATOR '\\'
#	define PHP_EOL "\r\n"
#else
#	if defined(__GNUC__) && __GNUC__ >= 4
#		define PHPAPI __attribute__ ((visibility("default")))
#	else
#		define PHPAPI
#	endif
#	define THREAD_LS
#	define PHP_DIR_SEPARATOR '/'
#	define PHP_EOL "\n"
#endif

#ifdef NETWARE
/* For php_get_uname() function */
#define PHP_UNAME  "NetWare"
#define PHP_OS      PHP_UNAME
#endif

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
#include <build-defs.h>
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
BEGIN_EXTERN_C()
PHPAPI size_t php_strlcpy(char *dst, const char *src, size_t siz);
END_EXTERN_C()
#undef strlcpy
#define strlcpy php_strlcpy
#endif

#ifndef HAVE_STRLCAT
BEGIN_EXTERN_C()
PHPAPI size_t php_strlcat(char *dst, const char *src, size_t siz);
END_EXTERN_C()
#undef strlcat
#define strlcat php_strlcat
#endif

#ifndef HAVE_STRTOK_R
BEGIN_EXTERN_C()
char *strtok_r(char *s, const char *delim, char **last);
END_EXTERN_C()
#endif

#ifndef HAVE_SOCKLEN_T
# if PHP_WIN32
typedef int socklen_t;
# else
typedef unsigned int socklen_t;
# endif
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

#ifndef va_copy
# ifdef __va_copy
#  define va_copy(ap1, ap2)         __va_copy((ap1), (ap2))
# else
#  define va_copy(ap1, ap2)         memcpy((&ap1), (&ap2), sizeof(va_list))
# endif
#endif

#include "zend_hash.h"
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

#ifndef HAVE_STRERROR
char *strerror(int);
#endif

#if HAVE_PWD_H
# ifdef PHP_WIN32
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

#ifndef INT_MAX
#define INT_MAX 2147483647
#endif

#ifndef INT_MIN
#define INT_MIN (- INT_MAX - 1)
#endif

#define PHP_GCC_VERSION ZEND_GCC_VERSION
#define PHP_ATTRIBUTE_MALLOC ZEND_ATTRIBUTE_MALLOC
#define PHP_ATTRIBUTE_FORMAT ZEND_ATTRIBUTE_FORMAT

BEGIN_EXTERN_C()
#include "snprintf.h"
END_EXTERN_C()
#include "spprintf.h"

#define EXEC_INPUT_BUF 4096

#define PHP_MIME_TYPE "application/x-httpd-php"

/* macros */
#define STR_PRINT(str)	((str)?(str):"")

#ifndef MAXPATHLEN
# ifdef PATH_MAX
#  define MAXPATHLEN PATH_MAX
# elif defined(MAX_PATH)
#  define MAXPATHLEN MAX_PATH
# else
#  define MAXPATHLEN 256    /* Should be safe for any weird systems that do not define it */
# endif
#endif

#if defined(__GNUC__) && __GNUC__ >= 4
# define php_ignore_value(x) (({ __typeof__ (x) __x = (x); (void) __x; }))
#else
# define php_ignore_value(x) ((void) (x))
#endif

/* global variables */
#if !defined(PHP_WIN32)
#define PHP_SLEEP_NON_VOID
#define php_sleep sleep
extern char **environ;
#endif	/* !defined(PHP_WIN32) */

#ifdef PHP_PWRITE_64
ssize_t pwrite(int, void *, size_t, off64_t);
#endif

#ifdef PHP_PREAD_64
ssize_t pread(int, void *, size_t, off64_t);
#endif

BEGIN_EXTERN_C()
void phperror(char *error);
PHPAPI int php_write(void *buf, uint size TSRMLS_DC);
PHPAPI int php_printf(const char *format, ...) PHP_ATTRIBUTE_FORMAT(printf, 1,
		2);
PHPAPI int php_get_module_initialized(void);
PHPAPI void php_log_err(char *log_message TSRMLS_DC);
int Debug(char *format, ...) PHP_ATTRIBUTE_FORMAT(printf, 1, 2);
int cfgparse(void);
END_EXTERN_C()

#define php_error zend_error
#define error_handling_t zend_error_handling_t

BEGIN_EXTERN_C()
static inline ZEND_ATTRIBUTE_DEPRECATED void php_set_error_handling(error_handling_t error_handling, zend_class_entry *exception_class TSRMLS_DC)
{
	zend_replace_error_handling(error_handling, exception_class, NULL TSRMLS_CC);
}
static inline ZEND_ATTRIBUTE_DEPRECATED void php_std_error_handling() {}

PHPAPI void php_verror(const char *docref, const char *params, int type, const char *format, va_list args TSRMLS_DC) PHP_ATTRIBUTE_FORMAT(printf, 4, 0);

#ifdef ZTS
#define PHP_ATTR_FMT_OFFSET 1
#else
#define PHP_ATTR_FMT_OFFSET 0
#endif

/* PHPAPI void php_error(int type, const char *format, ...); */
PHPAPI void php_error_docref0(const char *docref TSRMLS_DC, int type, const char *format, ...)
	PHP_ATTRIBUTE_FORMAT(printf, PHP_ATTR_FMT_OFFSET + 3, PHP_ATTR_FMT_OFFSET + 4);
PHPAPI void php_error_docref1(const char *docref TSRMLS_DC, const char *param1, int type, const char *format, ...)
	PHP_ATTRIBUTE_FORMAT(printf, PHP_ATTR_FMT_OFFSET + 4, PHP_ATTR_FMT_OFFSET + 5);
PHPAPI void php_error_docref2(const char *docref TSRMLS_DC, const char *param1, const char *param2, int type, const char *format, ...)
	PHP_ATTRIBUTE_FORMAT(printf, PHP_ATTR_FMT_OFFSET + 5, PHP_ATTR_FMT_OFFSET + 6);
#ifdef PHP_WIN32
PHPAPI void php_win32_docref2_from_error(DWORD error, const char *param1, const char *param2 TSRMLS_DC);
#endif
END_EXTERN_C()

#define php_error_docref php_error_docref0

#define zenderror phperror
#define zendlex phplex

#define phpparse zendparse
#define phprestart zendrestart
#define phpin zendin

#define php_memnstr zend_memnstr

/* functions */
BEGIN_EXTERN_C()
PHPAPI extern int (*php_register_internal_extensions_func)(TSRMLS_D);
PHPAPI int php_register_internal_extensions(TSRMLS_D);
PHPAPI int php_mergesort(void *base, size_t nmemb, register size_t size, int (*cmp)(const void *, const void * TSRMLS_DC) TSRMLS_DC);
PHPAPI void php_register_pre_request_shutdown(void (*func)(void *), void *userdata);
PHPAPI void php_com_initialize(TSRMLS_D);
PHPAPI char *php_get_current_user(TSRMLS_D);
END_EXTERN_C()

/* PHP-named Zend macro wrappers */
#define PHP_FN					ZEND_FN
#define PHP_MN					ZEND_MN
#define PHP_NAMED_FUNCTION		ZEND_NAMED_FUNCTION
#define PHP_FUNCTION			ZEND_FUNCTION
#define PHP_METHOD  			ZEND_METHOD

#define PHP_RAW_NAMED_FE ZEND_RAW_NAMED_FE
#define PHP_NAMED_FE	ZEND_NAMED_FE
#define PHP_FE			ZEND_FE
#define PHP_DEP_FE      ZEND_DEP_FE
#define PHP_FALIAS		ZEND_FALIAS
#define PHP_DEP_FALIAS	ZEND_DEP_FALIAS
#define PHP_ME          ZEND_ME
#define PHP_MALIAS      ZEND_MALIAS
#define PHP_ABSTRACT_ME ZEND_ABSTRACT_ME
#define PHP_ME_MAPPING  ZEND_ME_MAPPING
#define PHP_FE_END      ZEND_FE_END

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
#define PHP_GINIT		ZEND_GINIT
#define PHP_GSHUTDOWN	ZEND_GSHUTDOWN

#define PHP_MINIT_FUNCTION		ZEND_MODULE_STARTUP_D
#define PHP_MSHUTDOWN_FUNCTION	ZEND_MODULE_SHUTDOWN_D
#define PHP_RINIT_FUNCTION		ZEND_MODULE_ACTIVATE_D
#define PHP_RSHUTDOWN_FUNCTION	ZEND_MODULE_DEACTIVATE_D
#define PHP_MINFO_FUNCTION		ZEND_MODULE_INFO_D
#define PHP_GINIT_FUNCTION		ZEND_GINIT_FUNCTION
#define PHP_GSHUTDOWN_FUNCTION	ZEND_GSHUTDOWN_FUNCTION
 
#define PHP_MODULE_GLOBALS		ZEND_MODULE_GLOBALS


/* Output support */
#include "main/php_output.h"


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
#if defined(CRAY) || (defined(__arm) && !(defined(LINUX) || defined(__riscos__)))
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
