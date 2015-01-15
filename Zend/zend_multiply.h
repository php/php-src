/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2015 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   |          Ard Biesheuvel <ard.biesheuvel@linaro.org>                  |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_MULTIPLY_H
#define ZEND_MULTIPLY_H

#include "zend_bigint.h"

/* So __has_builtin won't cause an error if it's unavailable */
#ifndef __has_builtin
#	define __has_builtin(x) 0
#endif

#if __has_builtin(__builtin_smul_overflow) && SIZEOF_LONG == SIZEOF_ZEND_LONG

#define ZEND_SIGNED_MULTIPLY_LONG(a, b, lval, big, usedval) do {	\
	long   __lres  = (a) * (b);										\
	if (__builtin_smull_overflow((a), (b), &__lres)) {				\
		zend_bigint *__out = zend_bigint_init_alloc();				\
		zend_bigint_long_multiply_long(__out, a, b);				\
		(big) = __out;												\
		(usedval) = 1;												\
	} else {														\
		(lval) = __lres;											\
		(usedval) = 0;												\
	}																\
} while (0)

#elif __has_builtin(__builtin_saddll_overflow) && SIZEOF_LONG_LONG == SIZEOF_ZEND_LONG

#define ZEND_SIGNED_MULTIPLY_LONG(a, b, lval, big, usedval) do {	\
	long long  __llres  = (a) * (b);								\
	if (__builtin_smulll_overflow((a), (b), &__llres)) {			\
		zend_bigint *__out = zend_bigint_init_alloc();				\
		zend_bigint_long_multiply_long(__out, a, b);				\
		(big) = __out;												\
		(usedval) = 1;												\
	} else {														\
		(lval) = __llres;											\
		(usedval) = 0;												\
	}																\
} while (0)

#elif SIZEOF_ZEND_LONG == 4

#define ZEND_SIGNED_MULTIPLY_LONG(a, b, lval, big, usedval) do {	\
	int64_t __result = (int64_t) (a) * (int64_t) (b);				\
	if (__result > ZEND_LONG_MAX || __result < ZEND_LONG_MIN) {		\
		zend_bigint *__out = zend_bigint_init_alloc();				\
		zend_bigint_long_multiply_long(__out, a, b);				\
		(big) = __out;												\
		(usedval) = 1;												\
	} else {														\
		(lval) = (long) __result;									\
		(usedval) = 0;												\
	}																\
} while (0)

#else

#define ZEND_SIGNED_MULTIPLY_LONG(a, b, lval, big, usedval) do {	\
	long   __lres  = (a) * (b);										\
	long double __dres  = (long double)(a) * (long double)(b);		\
	long double __delta = (long double) __lres - __dres;			\
	if ( ((usedval) = (( __dres + __delta ) != __dres))) {			\
		zend_bigint *__out = zend_bigint_init_alloc();				\
		zend_bigint_long_multiply_long(__out, a, b);				\
		(big) = __out;												\
		(usedval) = 1;												\
	} else {														\
		(lval) = __lres;											\
		(usedval) = 0;												\
	}																\
} while (0)

#endif

#if defined(__GNUC__) && (defined(__native_client__) || defined(i386))

static zend_always_inline size_t zend_safe_address(size_t nmemb, size_t size, size_t offset, int *overflow)
{
	size_t res = nmemb;
	size_t m_overflow = 0;

	__asm__ ("mull %3\n\taddl %4,%0\n\tadcl $0,%1"
	     : "=&a"(res), "=&d" (m_overflow)
	     : "%0"(res),
	       "rm"(size),
	       "rm"(offset));

	if (UNEXPECTED(m_overflow)) {
		*overflow = 1;
		return 0;
	}
	*overflow = 0;
	return res;
}

#elif defined(__GNUC__) && defined(__x86_64__)

static zend_always_inline size_t zend_safe_address(size_t nmemb, size_t size, size_t offset, int *overflow)
{
	size_t res = nmemb;
	zend_ulong m_overflow = 0;

#ifdef __ILP32__ /* x32 */
# define LP_SUFF "l"
#else /* amd64 */
# define LP_SUFF "q"
#endif

	__asm__ ("mul" LP_SUFF  " %3\n\t"
		"add %4,%0\n\t"
		"adc $0,%1"
		: "=&a"(res), "=&d" (m_overflow)
		: "%0"(res),
		  "rm"(size),
		  "rm"(offset));

#undef LP_SUFF
	if (UNEXPECTED(m_overflow)) {
		*overflow = 1;
		return 0;
	}
	*overflow = 0;
	return res;
}

#elif defined(__GNUC__) && defined(__arm__)

static zend_always_inline size_t zend_safe_address(size_t nmemb, size_t size, size_t offset, int *overflow)
{
	size_t res;
	zend_ulong m_overflow;

	__asm__ ("umlal %0,%1,%2,%3"
		: "=r"(res), "=r"(m_overflow)
		: "r"(nmemb),
		  "r"(size),
		  "0"(offset),
		  "1"(0));

	if (UNEXPECTED(m_overflow)) {
		*overflow = 1;
		return 0;
	}
	*overflow = 0;
	return res;
}

#elif defined(__GNUC__) && defined(__aarch64__)

static zend_always_inline size_t zend_safe_address(size_t nmemb, size_t size, size_t offset, int *overflow)
{
	size_t res;
	zend_ulong m_overflow;

	__asm__ ("mul %0,%2,%3\n\tumulh %1,%2,%3\n\tadds %0,%0,%4\n\tadc %1,%1,xzr"
		: "=&r"(res), "=&r"(m_overflow)
		: "r"(nmemb),
		  "r"(size),
		  "r"(offset));

	if (UNEXPECTED(m_overflow)) {
		*overflow = 1;
		return 0;
	}
	*overflow = 0;
	return res;
}

#elif defined(__GNUC__) && defined(__powerpc64__)

static zend_always_inline size_t zend_safe_address(size_t nmemb, size_t size, size_t offset, int *overflow)
{
        size_t res;
        unsigned long m_overflow;

        __asm__ ("mulld %0,%2,%3\n\t"
                 "mulhdu %1,%2,%3\n\t"
                 "addc %0,%0,%4\n\t"
                 "addze %1,%1\n"
             : "=&r"(res), "=&r"(m_overflow)
             : "r"(nmemb),
               "r"(size),
               "r"(offset));

        if (UNEXPECTED(m_overflow)) {
                *overflow = 1;
                return 0;
        }
        *overflow = 0;
        return res;
}

#elif SIZEOF_SIZE_T == 4

static zend_always_inline size_t zend_safe_address(size_t nmemb, size_t size, size_t offset, int *overflow)
{
	uint64_t res = (uint64_t) nmemb * (uint64_t) size + (uint64_t) offset;

	if (UNEXPECTED(res > UINT64_C(0xFFFFFFFF))) {
		*overflow = 1;
		return 0;
	}
	*overflow = 0;
	return (size_t) res;
}

#else

static zend_always_inline size_t zend_safe_address(size_t nmemb, size_t size, size_t offset, int *overflow)
{
	size_t res = nmemb * size + offset;
	double _d  = (double)nmemb * (double)size + (double)offset;
	double _delta = (double)res - _d;

	if (UNEXPECTED((_d + _delta ) != _d)) {
		*overflow = 1;
		return 0;
	}
	*overflow = 0;
	return res;
}
#endif

#endif /* ZEND_MULTIPLY_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
