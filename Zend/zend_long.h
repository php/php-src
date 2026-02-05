/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright © Zend Technologies Ltd., a subsidiary company of          |
   |     Perforce Software, Inc., and Contributors.                       |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Anatol Belski <ab@php.net>                                  |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_LONG_H
#define ZEND_LONG_H

#include <inttypes.h>
#include <stdint.h>

/* This is the heart of the whole int64 enablement in zval. */
#if defined(__x86_64__) || defined(__LP64__) || defined(_LP64) || defined(_WIN64)
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


#ifdef ZEND_ENABLE_ZVAL_LONG64
# define ZEND_LONG_FMT "%" PRId64
# define ZEND_ULONG_FMT "%" PRIu64
# define ZEND_XLONG_FMT "%" PRIx64
# define ZEND_LONG_FMT_SPEC PRId64
# define ZEND_ULONG_FMT_SPEC PRIu64
# ifdef ZEND_WIN32
#  define ZEND_ATOL(s) _atoi64((s))
#  define ZEND_STRTOL(s0, s1, base) _strtoi64((s0), (s1), (base))
#  define ZEND_STRTOUL(s0, s1, base) _strtoui64((s0), (s1), (base))
#  define ZEND_STRTOL_PTR _strtoi64
#  define ZEND_STRTOUL_PTR _strtoui64
#  define ZEND_ABS _abs64
# else
#  define ZEND_ATOL(s) atoll((s))
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
# define ZEND_XLONG_FMT "%" PRIx32
# define ZEND_LONG_FMT_SPEC PRId32
# define ZEND_ULONG_FMT_SPEC PRIu32
# ifdef ZEND_WIN32
#  define ZEND_ATOL(s) atol((s))
# else
#  define ZEND_ATOL(s) atol((s))
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

#if SIZEOF_SIZE_T == 4
# define ZEND_ADDR_FMT "0x%08zx"
#elif SIZEOF_SIZE_T == 8
# define ZEND_ADDR_FMT "0x%016zx"
#else
# error "Unknown SIZEOF_SIZE_T"
#endif

#endif /* ZEND_LONG_H */
