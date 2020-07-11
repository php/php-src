/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Michael Wallner <mike@php.net>                               |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_STDINT_H
#define PHP_STDINT_H

/* C99 requires these for C++ to get the definitions
 * of INT64_MAX and other macros used by Zend/zend_long.h
 * C11 drops this requirement, so these effectively
 * just backport that piece of behavior.
 *
 * These defines are placed here instead of
 * with the include below, because sys/types
 * and inttypes may include stdint themselves.
 * And these definitions MUST come first.
 */
#ifdef __cplusplus
# ifndef __STDC_LIMIT_MACROS
#  define __STDC_LIMIT_MACROS
# endif
# ifndef __STDC_CONSTANT_MACROS
#  define __STDC_CONSTANT_MACROS
# endif
# ifndef __STDC_FORMAT_MACROS
#  define __STDC_FORMAT_MACROS
# endif
#endif

#include <inttypes.h>
#include <stdint.h>
#if defined(_MSC_VER)
# ifndef u_char
typedef unsigned __int8   u_char;
# endif
#else

#include "php_config.h"

#if HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#ifndef HAVE_INT8_T
# ifdef HAVE_INT8
typedef int8 int8_t;
# else
typedef signed char int8_t;
# endif
#endif

#ifndef INT8_C
# define INT8_C(c) c
#endif

#ifndef HAVE_UINT8_T
# ifdef HAVE_UINT8
typedef uint8 uint8_t
# elif HAVE_U_INT8_T
typedef u_int8_t uint8_t;
# else
typedef unsigned char uint8_t;
# endif
#endif

#ifndef UINT8_C
# define UINT8_C(c) c
#endif

#ifndef HAVE_INT16_T
# ifdef HAVE_INT16
typedef int16 int16_t;
# elif SIZEOF_SHORT >= 2
typedef signed short int16_t;
# else
#  error "No suitable 16bit integer type found"
# endif
#endif

#ifndef INT16_C
# define INT16_C(c) c
#endif

#ifndef HAVE_UINT16_T
# ifdef HAVE_UINT16
typedef uint16 uint16_t
# elif HAVE_U_INT16_T
typedef u_int16_t uint16_t;
# elif SIZEOF_SHORT >= 2
typedef unsigned short uint16_t;
# else
#  error "No suitable 16bit integer type found"
# endif
#endif

#ifndef UINT16_C
# define UINT16_C(c) c
#endif

#ifndef HAVE_INT32_T
# ifdef HAVE_INT32
typedef int32 int32_t;
# elif SIZEOF_INT >= 4
typedef int int32_t;
# elif SIZEOF_LONG >= 4
typedef long int32_t;
# else
#  error "No suitable 32bit integer type found"
# endif
#endif

#ifndef INT32_C
# define INT32_C(c) c
#endif

#ifndef HAVE_UINT32_T
# ifdef HAVE_UINT32
typedef uint32 uint32_t
# elif HAVE_U_INT32_T
typedef u_int32_t uint32_t;
# elif SIZEOF_INT >= 4
typedef unsigned int uint32_t;
# elif SIZEOF_LONG >= 4
typedef unsigned long uint32_t;
# else
#  error "No suitable 32bit integer type found"
# endif
#endif

#ifndef UINT32_C
# define UINT32_C(c) c ## U
#endif

#ifndef HAVE_INT64_T
# ifdef HAVE_INT64
typedef int64 int64_t;
# elif SIZEOF_INT >= 8
typedef int int64_t;
# elif SIZEOF_LONG >= 8
typedef long int64_t;
# elif SIZEOF_LONG_LONG >= 8
typedef long long int64_t;
# else
#  error "No suitable 64bit integer type found"
# endif
#endif

#ifndef INT64_C
# if SIZEOF_INT >= 8
#  define INT64_C(c) c
# elif SIZEOF_LONG >= 8
#  define INT64_C(c) c ## L
# elif SIZEOF_LONG_LONG >= 8
#  define INT64_C(c) c ## LL
# endif
#endif

#ifndef HAVE_UINT64_T
# ifdef HAVE_UINT64
typedef uint64 uint64_t
# elif HAVE_U_INT64_T
typedef u_int64_t uint64_t;
# elif SIZEOF_INT >= 8
typedef unsigned int uint64_t;
# elif SIZEOF_LONG >= 8
typedef unsigned long uint64_t;
# elif SIZEOF_LONG_LONG >= 8
typedef unsigned long long uint64_t;
# else
#  error "No suitable 64bit integer type found"
# endif
#endif

#ifndef UINT64_C
# if SIZEOF_INT >= 8
#  define UINT64_C(c) c ## U
# elif SIZEOF_LONG >= 8
#  define UINT64_C(c) c ## UL
# elif SIZEOF_LONG_LONG >= 8
#  define UINT64_C(c) c ## ULL
# endif
#endif

#endif /* !PHP_WIN32 */
#endif /* PHP_STDINT_H */
