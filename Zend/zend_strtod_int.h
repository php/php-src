/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
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

#ifndef ZEND_STRTOD_INT_H
#define ZEND_STRTOD_INT_H

#ifdef ZTS
#include <TSRM.h>
#endif

#include <stddef.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <math.h>

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

/* TODO check to undef this option, this might
	make more perf. destroy_freelist()
	should be adapted then. */
#define Omit_Private_Memory 1

/* HEX strings aren't supported as per
	https://wiki.php.net/rfc/remove_hex_support_in_numeric_strings */
#define NO_HEX_FP 1

#include <inttypes.h>

#ifdef USE_LOCALE
#undef USE_LOCALE
#endif

#ifndef NO_INFNAN_CHECK
#define NO_INFNAN_CHECK
#endif

#ifndef NO_ERRNO
#define NO_ERRNO
#endif

#ifdef WORDS_BIGENDIAN
#define IEEE_BIG_ENDIAN 1
#else
#define IEEE_LITTLE_ENDIAN 1
#endif

#if (defined(__APPLE__) || defined(__APPLE_CC__)) && (defined(__BIG_ENDIAN__) || defined(__LITTLE_ENDIAN__))
# if defined(__LITTLE_ENDIAN__)
#  undef WORDS_BIGENDIAN
# else
#  if defined(__BIG_ENDIAN__)
#   define WORDS_BIGENDIAN
#  endif
# endif
#endif

#if defined(__arm__) && !defined(__VFP_FP__)
/*
 *  * Although the CPU is little endian the FP has different
 *   * byte and word endianness. The byte order is still little endian
 *    * but the word order is big endian.
 *     */
#define IEEE_BIG_ENDIAN
#undef IEEE_LITTLE_ENDIAN
#endif

#ifdef __vax__
#define VAX
#undef IEEE_LITTLE_ENDIAN
#endif

#ifdef IEEE_LITTLE_ENDIAN
#define IEEE_8087 1
#endif

#ifdef IEEE_BIG_ENDIAN
#define IEEE_MC68k 1
#endif

#if defined(_MSC_VER)
#ifndef int32_t
#define int32_t __int32
#endif
#ifndef uint32_t
#define uint32_t unsigned __int32
#endif
#endif

#ifdef ZTS
#define MULTIPLE_THREADS 1

#define  ACQUIRE_DTOA_LOCK(x) \
	if (0 == x) { \
		tsrm_mutex_lock(dtoa_mutex); \
	} else if (1 == x) { \
		tsrm_mutex_lock(pow5mult_mutex); \
	}

#define FREE_DTOA_LOCK(x) \
	if (0 == x) { \
		tsrm_mutex_unlock(dtoa_mutex); \
	} else if (1 == x) { \
		tsrm_mutex_unlock(pow5mult_mutex); \
	}


#endif

#endif /* ZEND_STRTOD_INT_H */
