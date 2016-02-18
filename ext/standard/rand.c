/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
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
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Pedro Melo <melo@ip.pt>                                     |
   |          Sterling Hughes <sterling@php.net>                          |
   |                                                                      |
   | Based on code from: Richard J. Wagner <rjwagner@writeme.com>         |
   |                     Makoto Matsumoto <matumoto@math.keio.ac.jp>      |
   |                     Takuji Nishimura                                 |
   |                     Shawn Cokus <Cokus@math.washington.edu>          |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include <stdlib.h>

#include "php.h"
#include "php_math.h"
#include "php_rand.h"

#include "basic_functions.h"


/* SYSTEM RAND FUNCTIONS */

/* {{{ php_srand
 */
PHPAPI void php_srand(zend_long seed)
{
#ifdef ZTS
	BG(rand_seed) = (unsigned int) seed;
#else
# if defined(HAVE_SRANDOM)
	srandom((unsigned int) seed);
# elif defined(HAVE_SRAND48)
	srand48(seed);
# else
	srand((unsigned int) seed);
# endif
#endif

	/* Seed only once */
	BG(rand_is_seeded) = 1;
}
/* }}} */

/* {{{ php_rand
 */
PHPAPI zend_long php_rand(void)
{
	zend_long ret;

	if (!BG(rand_is_seeded)) {
		php_srand(GENERATE_SEED());
	}

#ifdef ZTS
	ret = php_rand_r(&BG(rand_seed));
#else
# if defined(HAVE_RANDOM)
	ret = random();
# elif defined(HAVE_LRAND48)
	ret = lrand48();
# else
	ret = rand();
# endif
#endif

	return ret;
}
/* }}} */


/* MT RAND FUNCTIONS */

/*
	The following php_mt_...() functions are based on a C++ class MTRand by
	Richard J. Wagner. For more information see the web page at
	http://www-personal.engin.umich.edu/~wagnerr/MersenneTwister.html

	Mersenne Twister random number generator -- a C++ class MTRand
	Based on code by Makoto Matsumoto, Takuji Nishimura, and Shawn Cokus
	Richard J. Wagner  v1.0  15 May 2003  rjwagner@writeme.com

	The Mersenne Twister is an algorithm for generating random numbers.  It
	was designed with consideration of the flaws in various other generators.
	The period, 2^19937-1, and the order of equidistribution, 623 dimensions,
	are far greater.  The generator is also fast; it avoids multiplication and
	division, and it benefits from caches and pipelines.  For more information
	see the inventors' web page at http://www.math.keio.ac.jp/~matumoto/emt.html

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

#define twist(m,u,v)  (m ^ (mixBits(u,v)>>1) ^ ((php_uint32)(-(php_int32)(loBit(u))) & 0x9908b0dfU))

/* {{{ php_mt_initialize
 */
static inline void php_mt_initialize(php_uint32 seed, php_uint32 *state)
{
	/* Initialize generator state with seed
	   See Knuth TAOCP Vol 2, 3rd Ed, p.106 for multiplier.
	   In previous versions, most significant bits (MSBs) of the seed affect
	   only MSBs of the state array.  Modified 9 Jan 2002 by Makoto Matsumoto. */

	register php_uint32 *s = state;
	register php_uint32 *r = state;
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

	register php_uint32 *state = BG(state);
	register php_uint32 *p = state;
	register int i;

	for (i = N - M; i--; ++p)
		*p = twist(p[M], p[0], p[1]);
	for (i = M; --i; ++p)
		*p = twist(p[M-N], p[0], p[1]);
	*p = twist(p[M-N], p[0], state[0]);
	BG(left) = N;
	BG(next) = state;
}
/* }}} */

/* {{{ php_mt_srand
 */
PHPAPI void php_mt_srand(php_uint32 seed)
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
PHPAPI php_uint32 php_mt_rand(void)
{
	/* Pull a 32-bit integer from the generator state
	   Every other access function simply transforms the numbers extracted here */

	register php_uint32 s1;

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

/* {{{ proto void srand([int seed])
   Seeds random number generator */
PHP_FUNCTION(srand)
{
	zend_long seed = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &seed) == FAILURE)
		return;

	if (ZEND_NUM_ARGS() == 0)
		seed = GENERATE_SEED();

	php_srand(seed);
}
/* }}} */

/* {{{ proto void mt_srand([int seed])
   Seeds Mersenne Twister random number generator */
PHP_FUNCTION(mt_srand)
{
	zend_long seed = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &seed) == FAILURE)
		return;

	if (ZEND_NUM_ARGS() == 0)
		seed = GENERATE_SEED();

	php_mt_srand(seed);
}
/* }}} */


/*
 * A bit of tricky math here.  We want to avoid using a modulus because
 * that simply tosses the high-order bits and might skew the distribution
 * of random values over the range.  Instead we map the range directly.
 *
 * We need to map the range from 0...M evenly to the range a...b
 * Let n = the random number and n' = the mapped random number
 *
 * Then we have: n' = a + n(b-a)/M
 *
 * We have a problem here in that only n==M will get mapped to b which
 # means the chances of getting b is much much less than getting any of
 # the other values in the range.  We can fix this by increasing our range
 # artificially and using:
 #
 #               n' = a + n(b-a+1)/M
 *
 # Now we only have a problem if n==M which would cause us to produce a
 # number of b+1 which would be bad.  So we bump M up by one to make sure
 # this will never happen, and the final algorithm looks like this:
 #
 #               n' = a + n(b-a+1)/(M+1)
 *
 * -RL
 */

/* {{{ proto int rand([int min, int max])
   Returns a random number */
PHP_FUNCTION(rand)
{
	zend_long min;
	zend_long max;
	zend_long number;
	int  argc = ZEND_NUM_ARGS();

	if (argc != 0 && zend_parse_parameters(argc, "ll", &min, &max) == FAILURE)
		return;

	number = php_rand();
	if (argc == 2) {
		RAND_RANGE(number, min, max, PHP_RAND_MAX);
	}

	RETURN_LONG(number);
}
/* }}} */

/* {{{ proto int mt_rand([int min, int max])
   Returns a random number from Mersenne Twister */
PHP_FUNCTION(mt_rand)
{
	zend_long min;
	zend_long max;
	zend_long number;
	int  argc = ZEND_NUM_ARGS();

	if (argc != 0) {
		if (zend_parse_parameters(argc, "ll", &min, &max) == FAILURE) {
			return;
		} else if (max < min) {
			php_error_docref(NULL, E_WARNING, "max(" ZEND_LONG_FMT ") is smaller than min(" ZEND_LONG_FMT ")", max, min);
			RETURN_FALSE;
		}
	}

	if (!BG(mt_rand_is_seeded)) {
		php_mt_srand(GENERATE_SEED());
	}

	/*
	 * Melo: hmms.. randomMT() returns 32 random bits...
	 * Yet, the previous php_rand only returns 31 at most.
	 * So I put a right shift to loose the lsb. It *seems*
	 * better than clearing the msb.
	 * Update:
	 * I talked with Cokus via email and it won't ruin the algorithm
	 */
	number = (zend_long) (php_mt_rand() >> 1);
	if (argc == 2) {
		RAND_RANGE(number, min, max, PHP_MT_RAND_MAX);
	}

	RETURN_LONG(number);
}
/* }}} */

/* {{{ proto int getrandmax(void)
   Returns the maximum value a random number can have */
PHP_FUNCTION(getrandmax)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(PHP_RAND_MAX);
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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
