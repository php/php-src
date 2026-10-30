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
   | Authors: Tim Düsterhus <timwolla@php.net>                            |
   |          Go Kudo <zeriyoshi@php.net>                                 |
   |                                                                      |
   | Based on code from: Melissa O'Neill <oneill@pcg-random.org>          |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_RANDOM_UINT128_H
# define PHP_RANDOM_UINT128_H

# include <stdint.h>

# if !defined(__SIZEOF_INT128__) || defined(PHP_RANDOM_FORCE_EMULATE_128)
typedef struct _php_random_uint128_t {
	uint64_t hi;
	uint64_t lo;
} php_random_uint128_t;

static inline uint64_t php_random_uint128_hi(php_random_uint128_t num)
{
	return num.hi;
}

static inline uint64_t php_random_uint128_lo(php_random_uint128_t num)
{
	return num.lo;
}

static inline php_random_uint128_t php_random_uint128_constant(uint64_t hi, uint64_t lo)
{
	php_random_uint128_t r;

	r.hi = hi;
	r.lo = lo;

	return r;
}

static inline php_random_uint128_t php_random_uint128_add(php_random_uint128_t num1, php_random_uint128_t num2)
{
	php_random_uint128_t r;

	r.lo = (num1.lo + num2.lo);
	r.hi = (num1.hi + num2.hi + (r.lo < num1.lo));

	return r;
}

static inline php_random_uint128_t php_random_uint128_multiply(php_random_uint128_t num1, php_random_uint128_t num2)
{
	php_random_uint128_t r;
	const uint64_t
		x0 = num1.lo & 0xffffffffULL,
		x1 = num1.lo >> 32,
		y0 = num2.lo & 0xffffffffULL,
		y1 = num2.lo >> 32,
		z0 = (((x1 * y0) + (x0 * y0 >> 32)) & 0xffffffffULL) + x0 * y1;

	r.hi = num1.hi * num2.lo + num1.lo * num2.hi;
	r.lo = num1.lo * num2.lo;
	r.hi += x1 * y1 + ((x1 * y0 + (x0 * y0 >> 32)) >> 32) + (z0 >> 32);

	return r;
}

static inline uint64_t php_random_pcgoneseq128xslrr64_rotr64(php_random_uint128_t num)
{
	const uint64_t
		v = (num.hi ^ num.lo),
		s = num.hi >> 58U;

	return (v >> s) | (v << ((-s) & 63));
}
# else
typedef __uint128_t php_random_uint128_t;

static inline uint64_t php_random_uint128_hi(php_random_uint128_t num)
{
	return (uint64_t) (num >> 64);
}

static inline uint64_t php_random_uint128_lo(php_random_uint128_t num)
{
	return (uint64_t) num;
}

static inline php_random_uint128_t php_random_uint128_constant(uint64_t hi, uint64_t lo)
{
	php_random_uint128_t r;

	r = ((php_random_uint128_t) hi << 64) + lo;

	return r;
}

static inline php_random_uint128_t php_random_uint128_add(php_random_uint128_t num1, php_random_uint128_t num2)
{
	return num1 + num2;
}

static inline php_random_uint128_t php_random_uint128_multiply(php_random_uint128_t num1, php_random_uint128_t num2)
{
	return num1 * num2;
}

static inline uint64_t php_random_pcgoneseq128xslrr64_rotr64(php_random_uint128_t num)
{
	const uint64_t
		v = ((uint64_t) (num >> 64U)) ^ (uint64_t) num,
		s = num >> 122U;

	return (v >> s) | (v << ((-s) & 63));
}
# endif

#endif	/* PHP_RANDOM_UINT128_H */
