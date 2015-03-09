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

/* This is the heart of the whole int64 enablement in zval. */
#if defined(__X86_64__) || defined(__LP64__) || defined(_LP64) || defined(_WIN64)
# define ZEND_ENABLE_ZVAL_LONG64 1
#endif

/* Integer types. */
#ifdef ZEND_ENABLE_ZVAL_LONG64
typedef int64_t zend_long;
typedef uint64_t zend_ulong;
typedef int64_t zend_off_t;
# define ZEND_LONG_MAX INT64_MAX
# define ZEND_LONG_MIN INT64_MIN
# define ZEND_ULONG_MAX UINT64_MAX
# define Z_L(i) INT64_C(i)
# define Z_UL(i) UINT64_C(i)
# define SIZEOF_ZEND_LONG 8
#else
typedef int32_t zend_long;
typedef uint32_t zend_ulong;
typedef int32_t zend_off_t;
# define ZEND_LONG_MAX INT32_MAX
# define ZEND_LONG_MIN INT32_MIN
# define ZEND_ULONG_MAX UINT32_MAX
# define Z_L(i) INT32_C(i)
# define Z_UL(i) UINT32_C(i)
# define SIZEOF_ZEND_LONG 4
#endif


/* Conversion macros. */
#define ZEND_LTOA_BUF_LEN 65

#ifdef ZEND_ENABLE_ZVAL_LONG64
# define ZEND_LONG_FMT "%" PRId64
# define ZEND_ULONG_FMT "%" PRIu64
# define ZEND_LONG_FMT_SPEC PRId64
# define ZEND_ULONG_FMT_SPEC PRIu64
# ifdef PHP_WIN32
#  define ZEND_LTOA(i, s, len) _i64toa_s((i), (s), (len), 10)
#  define ZEND_ATOL(i, s) i = _atoi64((s))
#  define ZEND_STRTOL(s0, s1, base) _strtoi64((s0), (s1), (base))
#  define ZEND_STRTOUL(s0, s1, base) _strtoui64((s0), (s1), (base))
#  define ZEND_STRTOL_PTR _strtoi64
#  define ZEND_STRTOUL_PTR _strtoui64
#  define ZEND_ABS _abs64
# else
#  define ZEND_LTOA(i, s, len) \
	do { \
		int st = snprintf((s), (len), ZEND_LONG_FMT, (i)); \
		(s)[st] = '\0'; \
 	} while (0)
#  define ZEND_ATOL(i, s) (i) = atoll((s))
#  define ZEND_STRTOL(s0, s1, base) strtoll((s0), (s1), (base))
#  define ZEND_STRTOUL(s0, s1, base) strtoull((s0), (s1), (base))
#  define ZEND_STRTOL_PTR strtoll
#  define ZEND_STRTOUL_PTR strtoull
#  define ZEND_ABS imaxabs
# endif
#else
# define ZEND_STRTOL(s0, s1, base) strtol((s0), (s1), (base))
# define ZEND_STRTOUL(s0, s1, base) strtoul((s0), (s1), (base))
# define ZEND_LONG_FMT "%" PRId32
# define ZEND_ULONG_FMT "%" PRIu32
# define ZEND_LONG_FMT_SPEC PRId32
# define ZEND_ULONG_FMT_SPEC PRIu32
# ifdef PHP_WIN32
#  define ZEND_LTOA(i, s, len) _ltoa_s((i), (s), (len), 10)
#  define ZEND_ATOL(i, s) i = atol((s))
# else
#  define ZEND_LTOA(i, s, len) \
	do { \
		int st = snprintf((s), (len), ZEND_LONG_FMT, (i)); \
		(s)[st] = '\0'; \
 	} while (0)
#  define ZEND_ATOL(i, s) (i) = atol((s))
# endif
# define ZEND_STRTOL_PTR strtol
# define ZEND_STRTOUL_PTR strtoul
# define ZEND_ABS abs
#endif

#if SIZEOF_ZEND_LONG == 4
# define MAX_LENGTH_OF_LONG 11
# define LONG_MIN_DIGITS "2147483648"
#elif SIZEOF_ZEND_LONG == 8
# define MAX_LENGTH_OF_LONG 20
# define LONG_MIN_DIGITS "9223372036854775808"
#else
# error "Unknown SIZEOF_ZEND_LONG"
#endif

static const char long_min_digits[] = LONG_MIN_DIGITS;

#endif /* ZEND_LONG_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
