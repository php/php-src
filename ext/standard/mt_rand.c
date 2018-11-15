/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Pedro Melo <melo@ip.pt>                                     |
   |          Sterling Hughes <sterling@php.net>                          |
   |                                                                      |
   | Based on code from: Richard J. Wagner <rjwagner@writeme.com>         |
   |                     Makoto Matsumoto <matumoto@math.keio.ac.jp>      |
   |                     Takuji Nishimura                                 |
   |                     Shawn Cokus <Cokus@math.washington.edu>          |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "php_rand.h"
#include "php_mt_rand.h"

/* MT RAND FUNCTIONS */

/*
	The following php_mt_...() functions are based on a C++ class MTRand by
	Richard J. Wagner. For more information see the web page at
	http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/VERSIONS/C-LANG/MersenneTwister.h

	Mersenne Twister random number generator -- a C++ class MTRand
	Based on code by Makoto Matsumoto, Takuji Nishimura, and Shawn Cokus
	Richard J. Wagner  v1.0  15 May 2003  rjwagner@writeme.com

	The Mersenne Twister is an algorithm for generating random numbers.  It
	was designed with consideration of the flaws in various other generators.
	The period, 2^19937-1, and the order of equidistribution, 623 dimensions,
	are far greater.  The generator is also fast; it avoids multiplication and
	division, and it benefits from caches and pipelines.  For more information
	see the inventors' web page at http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html

	Reference
	M. Matsumoto and T. Nishimura, "Mersenne Twister: A 623-Dimensionally
	Equidistributed Uniform Pseudo-Random Number Generator", ACM Transactions on
	Modeling and Computer Simulation, Vol. 8, No. 1, January 1998, pp 3-30.

	Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
	Copyright (C) 2000 - 2003, Richard J. Wagner
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:

	1. Redistributions of source code must retain the above copyright
	   notice, this list of conditions and the following disclaimer.

	2. Redistributions in binary form must reproduce the above copyright
	   notice, this list of conditions and the following disclaimer in the
	   documentation and/or other materials provided with the distribution.

	3. The names of its contributors may not be used to endorse or promote
	   products derived from this software without specific prior written
	   permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
	CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
	PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
	PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
	LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
	NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define N             MT_N                 /* length of state vector */
#define M             (397)                /* a period parameter */
#define hiBit(u)      ((u) & 0x80000000U)  /* mask all but highest   bit of u */
#define loBit(u)      ((u) & 0x00000001U)  /* mask all but lowest    bit of u */
#define loBits(u)     ((u) & 0x7FFFFFFFU)  /* mask     the highest   bit of u */
#define mixBits(u, v) (hiBit(u)|loBits(v)) /* move hi bit of u to hi bit of v */

#define twist(m,u,v)  (m ^ (mixBits(u,v)>>1) ^ ((uint32_t)(-(int32_t)(loBit(v))) & 0x9908b0dfU))
#define twist_php(m,u,v)  (m ^ (mixBits(u,v)>>1) ^ ((uint32_t)(-(int32_t)(loBit(u))) & 0x9908b0dfU))

/* {{{ php_mt_initialize
 */
static inline void php_mt_initialize(uint32_t seed, uint32_t *state)
{
	/* Initialize generator state with seed
	   See Knuth TAOCP Vol 2, 3rd Ed, p.106 for multiplier.
	   In previous versions, most significant bits (MSBs) of the seed affect
	   only MSBs of the state array.  Modified 9 Jan 2002 by Makoto Matsumoto. */

	register uint32_t *s = state;
	register uint32_t *r = state;
	register int i = 1;

	*s++ = seed & 0xffffffffU;
	for( ; i < N; ++i ) {
		*s++ = ( 1812433253U * ( *r ^ (*r >> 30) ) + i ) & 0xffffffffU;
		r++;
	}
}
/* }}} */

/* {{{ php_mt_reload
 */
static inline void php_mt_reload(void)
{
	/* Generate N new values in state
	   Made clearer and faster by Matthew Bellew (matthew.bellew@home.com) */

	register uint32_t *state = BG(state);
	register uint32_t *p = state;
	register int i;

	if (BG(mt_rand_mode) == MT_RAND_MT19937) {
		for (i = N - M; i--; ++p)
			*p = twist(p[M], p[0], p[1]);
		for (i = M; --i; ++p)
			*p = twist(p[M-N], p[0], p[1]);
		*p = twist(p[M-N], p[0], state[0]);
	}
	else {
		for (i = N - M; i--; ++p)
			*p = twist_php(p[M], p[0], p[1]);
		for (i = M; --i; ++p)
			*p = twist_php(p[M-N], p[0], p[1]);
		*p = twist_php(p[M-N], p[0], state[0]);
	}
	BG(left) = N;
	BG(next) = state;
}
/* }}} */

/* {{{ php_mt_srand
 */
PHPAPI void php_mt_srand(uint32_t seed)
{
	/* Seed the generator with a simple uint32 */
	php_mt_initialize(seed, BG(state));
	php_mt_reload();

	/* Seed only once */
	BG(mt_rand_is_seeded) = 1;
}
/* }}} */

/* {{{ php_mt_rand
 */
PHPAPI uint32_t php_mt_rand(void)
{
	/* Pull a 32-bit integer from the generator state
	   Every other access function simply transforms the numbers extracted here */

	register uint32_t s1;

	if (UNEXPECTED(!BG(mt_rand_is_seeded))) {
		php_mt_srand(GENERATE_SEED());
	}

	if (BG(left) == 0) {
		php_mt_reload();
	}
	--BG(left);

	s1 = *BG(next)++;
	s1 ^= (s1 >> 11);
	s1 ^= (s1 <<  7) & 0x9d2c5680U;
	s1 ^= (s1 << 15) & 0xefc60000U;
	return ( s1 ^ (s1 >> 18) );
}
/* }}} */

/* {{{ proto void mt_srand([int seed])
   Seeds Mersenne Twister random number generator */
PHP_FUNCTION(mt_srand)
{
	zend_long seed = 0;
	zend_long mode = MT_RAND_MT19937;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(seed)
		Z_PARAM_LONG(mode)
	ZEND_PARSE_PARAMETERS_END();

	if (ZEND_NUM_ARGS() == 0)
		seed = GENERATE_SEED();

	switch (mode) {
		case MT_RAND_PHP:
			BG(mt_rand_mode) = MT_RAND_PHP;
			break;
		default:
			BG(mt_rand_mode) = MT_RAND_MT19937;
	}

	php_mt_srand(seed);
}
/* }}} */

static uint32_t rand_range32(uint32_t umax) {
	uint32_t result, limit;

	result = php_mt_rand();

	/* Special case where no modulus is required */
	if (UNEXPECTED(umax == UINT32_MAX)) {
		return result;
	}

	/* Increment the max so the range is inclusive of max */
	umax++;

	/* Powers of two are not biased */
	if ((umax & (umax - 1)) == 0) {
		return result & (umax - 1);
	}

	/* Ceiling under which UINT32_MAX % max == 0 */
	limit = UINT32_MAX - (UINT32_MAX % umax) - 1;

	/* Discard numbers over the limit to avoid modulo bias */
	while (UNEXPECTED(result > limit)) {
		result = php_mt_rand();
	}

	return result % umax;
}

#if ZEND_ULONG_MAX > UINT32_MAX
static uint64_t rand_range64(uint64_t umax) {
	uint64_t result, limit;

	result = php_mt_rand();
	result = (result << 32) | php_mt_rand();

	/* Special case where no modulus is required */
	if (UNEXPECTED(umax == UINT64_MAX)) {
		return result;
	}

	/* Increment the max so the range is inclusive of max */
	umax++;

	/* Powers of two are not biased */
	if ((umax & (umax - 1)) == 0) {
		return result & (umax - 1);
	}

	/* Ceiling under which UINT64_MAX % max == 0 */
	limit = UINT64_MAX - (UINT64_MAX % umax) - 1;

	/* Discard numbers over the limit to avoid modulo bias */
	while (UNEXPECTED(result > limit)) {
		result = php_mt_rand();
		result = (result << 32) | php_mt_rand();
	}

	return result % umax;
}
#endif

/* {{{ php_mt_rand_range
 */
PHPAPI zend_long php_mt_rand_range(zend_long min, zend_long max)
{
	zend_ulong umax = max - min;

#if ZEND_ULONG_MAX > UINT32_MAX
	if (umax > UINT32_MAX) {
		return (zend_long) (rand_range64(umax) + min);
	}
#endif

	return (zend_long) (rand_range32(umax) + min);
}
/* }}} */

/* {{{ php_mt_rand_common
 * rand() allows min > max, mt_rand does not */
PHPAPI zend_long php_mt_rand_common(zend_long min, zend_long max)
{
	int64_t n;

	if (BG(mt_rand_mode) == MT_RAND_MT19937) {
		return php_mt_rand_range(min, max);
	}

	/* Legacy mode deliberately not inside php_mt_rand_range()
	 * to prevent other functions being affected */
	n = (int64_t)php_mt_rand() >> 1;
	RAND_RANGE_BADSCALING(n, min, max, PHP_MT_RAND_MAX);

	return n;
}
/* }}} */

/* {{{ proto int mt_rand([int min, int max])
   Returns a random number from Mersenne Twister */
PHP_FUNCTION(mt_rand)
{
	zend_long min;
	zend_long max;
	int argc = ZEND_NUM_ARGS();

	if (argc == 0) {
		// genrand_int31 in mt19937ar.c performs a right shift
		RETURN_LONG(php_mt_rand() >> 1);
	}

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(min)
		Z_PARAM_LONG(max)
	ZEND_PARSE_PARAMETERS_END();

	if (UNEXPECTED(max < min)) {
		php_error_docref(NULL, E_WARNING, "max(" ZEND_LONG_FMT ") is smaller than min(" ZEND_LONG_FMT ")", max, min);
		RETURN_FALSE;
	}

	RETURN_LONG(php_mt_rand_common(min, max));
}
/* }}} */

/* {{{ proto int mt_getrandmax(void)
   Returns the maximum value a random number from Mersenne Twister can have */
PHP_FUNCTION(mt_getrandmax)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	/*
	 * Melo: it could be 2^^32 but we only use 2^^31 to maintain
	 * compatibility with the previous php_rand
	 */
  	RETURN_LONG(PHP_MT_RAND_MAX); /* 2^^31 */
}
/* }}} */

PHP_MINIT_FUNCTION(mt_rand)
{
	REGISTER_LONG_CONSTANT("MT_RAND_MT19937", MT_RAND_MT19937, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MT_RAND_PHP",     MT_RAND_PHP, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
