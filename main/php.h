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

#ifndef PHP_H
#define PHP_H

#ifdef HAVE_DMALLOC
#include <dmalloc.h>
#endif

#define PHP_API_VERSION 20230901
#define PHP_HAVE_STREAMS
#define YYDEBUG 0
#define PHP_DEFAULT_CHARSET "UTF-8"

#include "php_version.h"
#include "zend.h"
#include "zend_sort.h"
#include "php_compat.h"

#include "zend_API.h"

#define php_sprintf sprintf

/* Operating system family definition */
#ifdef PHP_WIN32
# define PHP_OS_FAMILY			"Windows"
#elif defined(BSD) || defined(__DragonFly__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
# define PHP_OS_FAMILY			"BSD"
#elif defined(__APPLE__) || defined(__MACH__)
# define PHP_OS_FAMILY			"Darwin"
#elif defined(__sun__)
# define PHP_OS_FAMILY			"Solaris"
#elif defined(__linux__)
# define PHP_OS_FAMILY			"Linux"
#else
# define PHP_OS_FAMILY			"Unknown"
#endif

/* PHP's DEBUG value must match Zend's ZEND_DEBUG value */
#undef PHP_DEBUG
#define PHP_DEBUG ZEND_DEBUG

#ifdef PHP_WIN32
#	include "tsrm_win32.h"
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
#	define PHP_DIR_SEPARATOR '/'
#	define PHP_EOL "\n"
#endif

/* Windows specific defines */
#ifdef PHP_WIN32
# define PHP_PROG_SENDMAIL		"Built in mailer"
# define WIN32_LEAN_AND_MEAN
# define NOOPENFILE

# include <io.h>
# include <malloc.h>
# include <direct.h>
# include <stdlib.h>
# include <stdio.h>
# include <stdarg.h>
# include <sys/types.h>
# include <process.h>

typedef int uid_t;
typedef int gid_t;
typedef char * caddr_t;
typedef int pid_t;

# define M_TWOPI        (M_PI * 2.0)
# define off_t			_off_t

# define lstat(x, y)	php_sys_lstat(x, y)
# define chdir(path)	_chdir(path)
# define mkdir(a, b)	_mkdir(a)
# define rmdir(a)		_rmdir(a)
# define getpid			_getpid
# define php_sleep(t)	SleepEx(t*1000, TRUE)

# ifndef getcwd
#  define getcwd(a, b)	_getcwd(a, b)
# endif
#endif

#if PHP_DEBUG
#undef NDEBUG
#else
#ifndef NDEBUG
#define NDEBUG
#endif
#endif
#include <assert.h>

#ifdef HAVE_UNIX_H
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
#define HAVE_STRLCPY 1
#define USE_STRLCPY_PHP_IMPL 1
#endif

#ifndef HAVE_STRLCAT
BEGIN_EXTERN_C()
PHPAPI size_t php_strlcat(char *dst, const char *src, size_t siz);
END_EXTERN_C()
#undef strlcat
#define strlcat php_strlcat
#define HAVE_STRLCAT 1
#define USE_STRLCAT_PHP_IMPL 1
#endif

#ifndef HAVE_EXPLICIT_BZERO
BEGIN_EXTERN_C()
PHPAPI void php_explicit_bzero(void *dst, size_t siz);
END_EXTERN_C()
#undef explicit_bzero
#define explicit_bzero php_explicit_bzero
#endif

BEGIN_EXTERN_C()
PHPAPI int php_safe_bcmp(const zend_string *a, const zend_string *b);
END_EXTERN_C()

#ifndef HAVE_STRTOK_R
BEGIN_EXTERN_C()
char *strtok_r(char *s, const char *delim, char **last);
END_EXTERN_C()
#endif

#ifndef HAVE_SOCKLEN_T
# ifdef PHP_WIN32
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

#include <stdarg.h>

#include "zend_hash.h"
#include "zend_alloc.h"
#include "zend_stack.h"
#include <string.h>

#if HAVE_PWD_H
# ifdef PHP_WIN32
#include "win32/param.h"
# else
#include <pwd.h>
#include <sys/param.h>
# endif
#endif

#include <limits.h>

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

#define PHP_DOUBLE_MAX_LENGTH ZEND_DOUBLE_MAX_LENGTH

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
# ifdef PHP_WIN32
#  include "win32/ioutil.h"
#  define MAXPATHLEN PHP_WIN32_IOUTIL_MAXPATHLEN
# elif PATH_MAX
#  define MAXPATHLEN PATH_MAX
# elif defined(MAX_PATH)
#  define MAXPATHLEN MAX_PATH
# else
#  define MAXPATHLEN 256    /* Should be safe for any weird systems that do not define it */
# endif
#endif

#define php_ignore_value(x) ZEND_IGNORE_VALUE(x)

/* global variables */
#ifndef PHP_WIN32
#define php_sleep sleep
extern char **environ;
#endif	/* ifndef PHP_WIN32 */

#ifdef PHP_PWRITE_64
ssize_t pwrite(int, void *, size_t, off64_t);
#endif

#ifdef PHP_PREAD_64
ssize_t pread(int, void *, size_t, off64_t);
#endif

BEGIN_EXTERN_C()
void phperror(char *error);
PHPAPI size_t php_write(void *buf, size_t size);
PHPAPI size_t php_printf(const char *format, ...) PHP_ATTRIBUTE_FORMAT(printf, 1, 2);
PHPAPI size_t php_printf_unchecked(const char *format, ...);
PHPAPI bool php_during_module_startup(void);
PHPAPI bool php_during_module_shutdown(void);
PHPAPI bool php_get_module_initialized(void);
#ifdef HAVE_SYSLOG_H
#include "php_syslog.h"
#define php_log_err(msg) php_log_err_with_severity(msg, LOG_NOTICE)
#else
#define php_log_err(msg) php_log_err_with_severity(msg, 5)
#endif
PHPAPI ZEND_COLD void php_log_err_with_severity(const char *log_message, int syslog_type_int);
int Debug(char *format, ...) PHP_ATTRIBUTE_FORMAT(printf, 1, 2);
int cfgparse(void);
END_EXTERN_C()

#define php_error zend_error
#define error_handling_t zend_error_handling_t

BEGIN_EXTERN_C()
static inline ZEND_ATTRIBUTE_DEPRECATED void php_set_error_handling(error_handling_t error_handling, zend_class_entry *exception_class)
{
	zend_replace_error_handling(error_handling, exception_class, NULL);
}
static inline ZEND_ATTRIBUTE_DEPRECATED void php_std_error_handling(void) {}

PHPAPI ZEND_COLD void php_verror(const char *docref, const char *params, int type, const char *format, va_list args) PHP_ATTRIBUTE_FORMAT(printf, 4, 0);

/* PHPAPI void php_error(int type, const char *format, ...); */
PHPAPI ZEND_COLD void php_error_docref(const char *docref, int type, const char *format, ...)
	PHP_ATTRIBUTE_FORMAT(printf, 3, 4);
PHPAPI ZEND_COLD void php_error_docref_unchecked(const char *docref, int type, const char *format, ...);
PHPAPI ZEND_COLD void php_error_docref1(const char *docref, const char *param1, int type, const char *format, ...)
	PHP_ATTRIBUTE_FORMAT(printf, 4, 5);
PHPAPI ZEND_COLD void php_error_docref2(const char *docref, const char *param1, const char *param2, int type, const char *format, ...)
	PHP_ATTRIBUTE_FORMAT(printf, 5, 6);
#ifdef PHP_WIN32
PHPAPI ZEND_COLD void php_win32_docref1_from_error(DWORD error, const char *param1);
PHPAPI ZEND_COLD void php_win32_docref2_from_error(DWORD error, const char *param1, const char *param2);
#endif
END_EXTERN_C()

#define zenderror phperror
#define zendlex phplex

#define phpparse zendparse
#define phprestart zendrestart
#define phpin zendin

#define php_memnstr zend_memnstr
#define php_memnistr zend_memnistr

/* functions */
BEGIN_EXTERN_C()
PHPAPI extern int (*php_register_internal_extensions_func)(void);
PHPAPI int php_register_internal_extensions(void);
PHPAPI void php_register_pre_request_shutdown(void (*func)(void *), void *userdata);
PHPAPI void php_com_initialize(void);
PHPAPI char *php_get_current_user(void);

PHPAPI const char *php_get_internal_encoding(void);
PHPAPI const char *php_get_input_encoding(void);
PHPAPI const char *php_get_output_encoding(void);
PHPAPI extern void (*php_internal_encoding_changed)(void);
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
#include "zend_virtual_cwd.h"

#include "zend_constants.h"

/* connection status states */
#define PHP_CONNECTION_NORMAL  0
#define PHP_CONNECTION_ABORTED 1
#define PHP_CONNECTION_TIMEOUT 2

#include "php_reentrancy.h"

/* the following typedefs are deprecated and will be removed in PHP
 * 9.0; use the standard C99 types instead */
typedef bool zend_bool;
typedef intptr_t zend_intptr_t;
typedef uintptr_t zend_uintptr_t;

#endif
