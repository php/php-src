/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2013 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Anatol Belski <ab@php.net>                                  |
   +----------------------------------------------------------------------+
*/

/* $Id$ */


#ifndef ZEND_LONG_H
#define ZEND_LONG_H

#include "main/php_stdint.h"

/* This is the heart of the whole int64 enablement in zval. Based on this
   macros the size_t datatype is handled, see zend_types.h */
#if defined(__X86_64__) || defined(__LP64__) || defined(_LP64) || defined(_WIN64)
# define ZEND_ENABLE_INT64 1
#endif

/* Integer types or the old bad long. */
#ifdef ZEND_ENABLE_INT64
# ifdef ZEND_WIN32
#  ifdef _WIN64
typedef __int64 zend_long;
typedef unsigned __int64 zend_ulong;
typedef __int64 zend_off_t;
#   define ZEND_LONG_MAX _I64_MAX
#   define ZEND_LONG_MIN _I64_MIN
#   define ZEND_ULONG_MAX _UI64_MAX
#   define Z_I(i) i##i64
#   define Z_UL(i) i##Ui64
#  else
#   error Cant enable 64 bit integers on non 64 bit platform
#  endif
# else
#  if defined(__X86_64__) || defined(__LP64__) || defined(_LP64)
typedef int64_t zend_long;
typedef uint64_t zend_ulong;
typedef off_t zend_off_t;
#   define ZEND_LONG_MAX INT64_MAX
#   define ZEND_LONG_MIN INT64_MIN
#   define ZEND_ULONG_MAX UINT64_MAX
#   define Z_I(i) i##LL
#   define Z_UL(i) i##ULL
#  else
#   error Cant enable 64 bit integers on non 64 bit platform
#  endif
# endif
# define SIZEOF_ZEND_INT 8
#else
typedef long zend_long;
typedef unsigned long zend_ulong;
typedef long zend_off_t;
# define ZEND_LONG_MAX LONG_MAX
# define ZEND_LONG_MIN LONG_MIN
# define ZEND_ULONG_MAX ULONG_MAX
# define Z_I(i) i##L
# define Z_UL(i) i##UL
# define SIZEOF_ZEND_INT SIZEOF_LONG
#endif


/* conversion macros */
#define ZEND_LTOA_BUF_LEN 65

#ifdef ZEND_ENABLE_INT64
# ifdef PHP_WIN32
#  define ZEND_LTOA(i, s, len) _i64toa_s((i), (s), (len), 10)
#  define ZEND_ATOL(i, s) i = _atoi64((s))
#  define ZEND_STRTOL(s0, s1, base) _strtoi64((s0), (s1), (base))
#  define ZEND_STRTOUL(s0, s1, base) _strtoui64((s0), (s1), (base))
#  define ZEND_LONG_FMT "%I64d"
#  define ZEND_ULONG_FMT "%I64u"
#  define ZEND_LONG_FMT_SPEC "I64d"
#  define ZEND_ULONG_FMT_SPEC "I64u"
#  define ZEND_STRTOL_PTR _strtoi64
#  define ZEND_STRTOUL_PTR _strtoui64
#  define ZEND_ABS _abs64
# else
#  define ZEND_LTOA(i, s, len) \
	do { \
		int st = snprintf((s), (len), "%lld", (i)); \
		(s)[st] = '\0'; \
 	} while (0)
#  define ZEND_ATOL(i, s) (i) = atoll((s))
#  define ZEND_STRTOL(s0, s1, base) strtoll((s0), (s1), (base))
#  define ZEND_STRTOUL(s0, s1, base) strtoull((s0), (s1), (base))
#  define ZEND_LONG_FMT "%" PRId64
#  define ZEND_ULONG_FMT "%" PRIu64
#  define ZEND_LONG_FMT_SPEC PRId64
#  define ZEND_ULONG_FMT_SPEC PRIu64
#  define ZEND_STRTOL_PTR strtoll
#  define ZEND_STRTOUL_PTR strtoull
#  define ZEND_ABS llabs
# endif
#else
# define ZEND_STRTOL(s0, s1, base) strtol((s0), (s1), (base))
# define ZEND_STRTOUL(s0, s1, base) strtoul((s0), (s1), (base))
# ifdef PHP_WIN32
#  define ZEND_LTOA(i, s, len) _ltoa_s((i), (s), (len), 10)
#  define ZEND_ATOL(i, s) i = atol((s))
# else
#  define ZEND_LTOA(i, s, len) \
	do { \
		int st = snprintf((s), (len), "%ld", (i)); \
		(s)[st] = '\0'; \
 	} while (0)
#  define ZEND_ATOL(i, s) (i) = atol((s))
# endif
# define ZEND_LONG_FMT "%ld"
# define ZEND_ULONG_FMT "%lu"
# define ZEND_LONG_FMT_SPEC "ld"
# define ZEND_ULONG_FMT_SPEC "lu"
# define ZEND_STRTOL_PTR strtol
# define ZEND_STRTOUL_PTR strtoul
# define ZEND_ABS abs
#endif


#endif /* ZEND_LONG_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
