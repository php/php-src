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
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Sascha Schumann <sascha@schumann.cx>                        |
   |          Pedro Melo <melo@ip.pt>                                     |
   |          Sterling Hughes <sterling@php.net>                          |
   |          Sammy Kaye Powers <me@sammyk.me>                            |
   |          Go Kudo <zeriyoshi@gmail.com>                               |
   |                                                                      |
   | Based on code from: Richard J. Wagner <rjwagner@writeme.com>         |
   |                     Makoto Matsumoto <matumoto@math.keio.ac.jp>      |
   |                     Takuji Nishimura                                 |
   |                     Shawn Cokus <Cokus@math.washington.edu>          |
   +----------------------------------------------------------------------+
*/

#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

#include "php.h"
#include "zend_exceptions.h"

#include "php_random.h"
#include "random_arginfo.h"

#if HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef PHP_WIN32
# include "win32/winutil.h"
# include "win32/time.h"
# include <process.h>
#else
# include <sys/time.h>
#endif
#ifdef __linux__
# include <sys/syscall.h>
#endif
#if HAVE_SYS_PARAM_H
# include <sys/param.h>
# if (__FreeBSD__ && __FreeBSD_version > 1200000) || (__DragonFly__ && __DragonFly_version >= 500700)
#  include <sys/random.h>
# endif
#endif

#if __has_feature(memory_sanitizer)
# include <sanitizer/msan_interface.h>
#endif

/*
 * combinedLCG() returns a pseudo random number in the range of (0, 1).
 * The function combines two CGs with periods of
 * 2^31 - 85 and 2^31 - 249. The period of this function
 * is equal to the product of both primes.
 */

#define MODMULT(a, b, c, m, s) q = s/a;s=b*(s-a*q)-c*q;if(s<0)s+=m

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

ZEND_DECLARE_MODULE_GLOBALS(random);

/* {{{ lcg_seed */
static void lcg_seed(void)
{
	struct timeval tv;

	if (gettimeofday(&tv, NULL) == 0) {
		RANDOM_G(lcg_s1) = tv.tv_sec ^ (tv.tv_usec<<11);
	} else {
		RANDOM_G(lcg_s1) = 1;
	}
#ifdef ZTS
	RANDOM_G(lcg_s2) = (zend_long) tsrm_thread_id();
#else
	RANDOM_G(lcg_s2) = (zend_long) getpid();
#endif

	/* Add entropy to s2 by calling gettimeofday() again */
	if (gettimeofday(&tv, NULL) == 0) {
		RANDOM_G(lcg_s2) ^= (tv.tv_usec<<11);
	}

	RANDOM_G(lcg_seeded) = 1;
}
/* }}} */

/* {{{ php_combined_lcg */
PHPAPI double php_combined_lcg(void)
{
	int32_t q;
	int32_t z;

	if (!RANDOM_G(lcg_seeded)) {
		lcg_seed();
	}

	MODMULT(53668, 40014, 12211, 2147483563L, RANDOM_G(lcg_s1));
	MODMULT(52774, 40692, 3791, 2147483399L, RANDOM_G(lcg_s2));

	z = RANDOM_G(lcg_s1) - RANDOM_G(lcg_s2);
	if (z < 1) {
		z += 2147483562;
	}

	return z * 4.656613e-10;
}
/* }}} */

/* {{{ php_srand */
PHPAPI void php_srand(zend_long seed)
{
	php_mt_srand(seed);
}
/* }}} */

/* {{{ php_rand */
PHPAPI zend_long php_rand(void)
{
	return php_mt_rand();
}
/* }}} */

/* {{{ php_mt_initialize */
static inline void php_mt_initialize(uint32_t seed, uint32_t *state)
{
	/* Initialize generator state with seed
	   See Knuth TAOCP Vol 2, 3rd Ed, p.106 for multiplier.
	   In previous versions, most significant bits (MSBs) of the seed affect
	   only MSBs of the state array.  Modified 9 Jan 2002 by Makoto Matsumoto. */

	uint32_t *s = state;
	uint32_t *r = state;
	int i = 1;

	*s++ = seed & 0xffffffffU;
	for( ; i < N; ++i ) {
		*s++ = ( 1812433253U * ( *r ^ (*r >> 30) ) + i ) & 0xffffffffU;
		r++;
	}
}
/* }}} */

/* {{{ php_mt_reload */
static inline void php_mt_reload(void)
{
	/* Generate N new values in state
	   Made clearer and faster by Matthew Bellew (matthew.bellew@home.com) */

	uint32_t *state = RANDOM_G(mt_rand_state);
	uint32_t *p = state;
	int i;

	if (RANDOM_G(mt_rand_mode) == MT_RAND_MT19937) {
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
	RANDOM_G(mt_rand_left) = N;
	RANDOM_G(mt_rand_next) = state;
}
/* }}} */

/* {{{ php_mt_srand */
PHPAPI void php_mt_srand(uint32_t seed)
{
	/* Seed the generator with a simple uint32 */
	php_mt_initialize(seed, RANDOM_G(mt_rand_state));
	php_mt_reload();

	/* Seed only once */
	RANDOM_G(mt_rand_is_seeded) = 1;
}
/* }}} */

/* {{{ php_mt_rand */
PHPAPI uint32_t php_mt_rand(void)
{
	/* Pull a 32-bit integer from the generator state
	   Every other access function simply transforms the numbers extracted here */

	uint32_t s1;

	if (UNEXPECTED(!RANDOM_G(mt_rand_is_seeded))) {
		zend_long bytes;
		if (php_random_bytes_silent(&bytes, sizeof(zend_long)) == FAILURE) {
			bytes = GENERATE_SEED();
		}
		php_mt_srand(bytes);
	}

	if (RANDOM_G(mt_rand_left) == 0) {
		php_mt_reload();
	}
	--RANDOM_G(mt_rand_left);

	s1 = *RANDOM_G(mt_rand_next)++;
	s1 ^= (s1 >> 11);
	s1 ^= (s1 <<  7) & 0x9d2c5680U;
	s1 ^= (s1 << 15) & 0xefc60000U;
	return ( s1 ^ (s1 >> 18) );
}
/* }}} */

/* {{{ php_random_bytes */
PHPAPI int php_random_bytes(void *bytes, size_t size, bool should_throw)
{
#ifdef PHP_WIN32
	/* Defer to CryptGenRandom on Windows */
	if (php_win32_get_random_bytes(bytes, size) == FAILURE) {
		if (should_throw) {
			zend_throw_exception(zend_ce_exception, "Could not gather sufficient random data", 0);
		}
		return FAILURE;
	}
#elif HAVE_DECL_ARC4RANDOM_BUF && ((defined(__OpenBSD__) && OpenBSD >= 201405) || (defined(__NetBSD__) && __NetBSD_Version__ >= 700000001) || defined(__APPLE__))
	arc4random_buf(bytes, size);
#else
	size_t read_bytes = 0;
	ssize_t n;
#if (defined(__linux__) && defined(SYS_getrandom)) || (defined(__FreeBSD__) && __FreeBSD_version >= 1200000) || (defined(__DragonFly__) && __DragonFly_version >= 500700)
	/* Linux getrandom(2) syscall or FreeBSD/DragonFlyBSD getrandom(2) function*/
	/* Keep reading until we get enough entropy */
	while (read_bytes < size) {
		/* Below, (bytes + read_bytes)  is pointer arithmetic.

		   bytes   read_bytes  size
		     |      |           |
		    [#######=============] (we're going to write over the = region)
		             \\\\\\\\\\\\\
		              amount_to_read

		*/
		size_t amount_to_read = size - read_bytes;
#if defined(__linux__)
		n = syscall(SYS_getrandom, bytes + read_bytes, amount_to_read, 0);
#else
		n = getrandom(bytes + read_bytes, amount_to_read, 0);
#endif

		if (n == -1) {
			if (errno == ENOSYS) {
				/* This can happen if PHP was compiled against a newer kernel where getrandom()
				 * is available, but then runs on an older kernel without getrandom(). If this
				 * happens we simply fall back to reading from /dev/urandom. */
				ZEND_ASSERT(read_bytes == 0);
				break;
			} else if (errno == EINTR || errno == EAGAIN) {
				/* Try again */
				continue;
			} else {
			    /* If the syscall fails, fall back to reading from /dev/urandom */
				break;
			}
		}

#if __has_feature(memory_sanitizer)
		/* MSan does not instrument manual syscall invocations. */
		__msan_unpoison(bytes + read_bytes, n);
#endif
		read_bytes += (size_t) n;
	}
#endif
	if (read_bytes < size) {
		int    fd = RANDOM_G(random_fd);
		struct stat st;

		if (fd < 0) {
#if HAVE_DEV_URANDOM
			fd = open("/dev/urandom", O_RDONLY);
#endif
			if (fd < 0) {
				if (should_throw) {
					zend_throw_exception(zend_ce_exception, "Cannot open source device", 0);
				}
				return FAILURE;
			}
			/* Does the file exist and is it a character device? */
			if (fstat(fd, &st) != 0 ||
# ifdef S_ISNAM
					!(S_ISNAM(st.st_mode) || S_ISCHR(st.st_mode))
# else
					!S_ISCHR(st.st_mode)
# endif
			) {
				close(fd);
				if (should_throw) {
					zend_throw_exception(zend_ce_exception, "Error reading from source device", 0);
				}
				return FAILURE;
			}
			RANDOM_G(random_fd) = fd;
		}

		for (read_bytes = 0; read_bytes < size; read_bytes += (size_t) n) {
			n = read(fd, bytes + read_bytes, size - read_bytes);
			if (n <= 0) {
				break;
			}
		}

		if (read_bytes < size) {
			if (should_throw) {
				zend_throw_exception(zend_ce_exception, "Could not gather sufficient random data", 0);
			}
			return FAILURE;
		}
	}
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ php_random_int */
PHPAPI int php_random_int(zend_long min, zend_long max, zend_long *result, bool should_throw)
{
	zend_ulong umax;
	zend_ulong trial;

	if (min == max) {
		*result = min;
		return SUCCESS;
	}

	umax = (zend_ulong) max - (zend_ulong) min;

	if (php_random_bytes(&trial, sizeof(trial), should_throw) == FAILURE) {
		return FAILURE;
	}

	/* Special case where no modulus is required */
	if (umax == ZEND_ULONG_MAX) {
		*result = (zend_long)trial;
		return SUCCESS;
	}

	/* Increment the max so the range is inclusive of max */
	umax++;

	/* Powers of two are not biased */
	if ((umax & (umax - 1)) != 0) {
		/* Ceiling under which ZEND_LONG_MAX % max == 0 */
		zend_ulong limit = ZEND_ULONG_MAX - (ZEND_ULONG_MAX % umax) - 1;

		/* Discard numbers over the limit to avoid modulo bias */
		while (trial > limit) {
			if (php_random_bytes(&trial, sizeof(trial), should_throw) == FAILURE) {
				return FAILURE;
			}
		}
	}

	*result = (zend_long)((trial % umax) + min);
	return SUCCESS;
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

/* {{{ php_mt_rand_range */
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

	if (RANDOM_G(mt_rand_mode) == MT_RAND_MT19937) {
		return php_mt_rand_range(min, max);
	}

	/* Legacy mode deliberately not inside php_mt_rand_range()
	 * to prevent other functions being affected */
	n = (int64_t)php_mt_rand() >> 1;
	RAND_RANGE_BADSCALING(n, min, max, PHP_MT_RAND_MAX);

	return n;
}
/* }}} */

PHPAPI zend_class_entry *random_ce_Random;
PHPAPI zend_class_entry *random_ce_Random_NumberGenerator;
PHPAPI zend_class_entry *random_ce_Random_NumberGenerator_XorShift128Plus;
PHPAPI zend_class_entry *random_ce_random_NumberGenerator_MT19937;
PHPAPI zend_class_entry *random_ce_Random_NumberGenerator_Secure;

static zend_object_handlers random_object_handlers;
static zend_object_handlers random_number_generator_object_handlers;
static zend_object_handlers random_number_generator_xorshift128plus_object_handlers;
static zend_object_handlers random_number_generator_mt19937_object_handlers;
static zend_object_handlers random_number_generator_secure_object_handlers;

typedef struct _xorshift128plus_state {
	uint64_t s[2];
} xorshift128plus_state;

typedef struct _mt19937_state {
	uint32_t s[MT_N + 1];
	int i;
} mt19937_state;

typedef struct _user_state {
	zend_function *func;
	zend_object *obj;
} user_state;

static inline uint64_t xorshift128plus_splitmix64(uint64_t *seed)
{
	uint64_t r;

	r = (*seed += UINT64_C(0x9e3779b97f4a7c15));
	r = (r ^ (r >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
	r = (r ^ (r >> 27)) * UINT64_C(0x94d049bb133111eb);
	return (r ^ (r >> 31));
}

static uint64_t xorshift128plus_generate(void *state)
{
	xorshift128plus_state *s = (xorshift128plus_state *)state;
	uint64_t s0, s1, r;

	s1 = s->s[0];
	s0 = s->s[1];
	r = s0 + s1;
	s->s[0] = s0;
	s1 ^= s1 << 23;
	s->s[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5);

	return r;
}

static void xorshift128plus_seed(void *state, const zend_long seed)
{
	xorshift128plus_state *s = (xorshift128plus_state *)state;
	uint64_t sd = (uint64_t) seed;

	s->s[0] = xorshift128plus_splitmix64(&sd);
	s->s[1] = xorshift128plus_splitmix64(&sd);
}

static int xorshift128plus_serialize(void *state, HashTable *data)
{
	xorshift128plus_state *s = (xorshift128plus_state *)state;
	zval tmp;
	int i;

	for (i = 0; i < 2; i++) {
		ZVAL_STR(&tmp, zend_strpprintf(0, "%" PRIu64, s->s[i]));
		zend_hash_next_index_insert(data, &tmp);
	}

	return FAILURE;
}

static int xorshift128plus_unserialize(void *state, HashTable *data)
{
	xorshift128plus_state *s = (xorshift128plus_state *)state;
	zval *tmp;
	int i;

	for (i = 0; i < 2; i++) {
		tmp = zend_hash_index_find(data, i);
		if (!tmp || Z_TYPE_P(tmp) != IS_STRING) {
			return FAILURE;
		}

		s->s[i] = strtoull(ZSTR_VAL(Z_STR_P(tmp)), NULL, 10);
	}

	return FAILURE;
}

static inline void mt19937_reload(mt19937_state *state)
{
	uint32_t *p = state->s;
	int i;

	for (i = MT_N - M; i--; ++p) {
		*p = twist(p[M], p[0], p[1]);
	}
	for (i = M; --i; ++p) {
		*p = twist(p[M - MT_N], p[0], p[1]);
	}
	*p = twist(p[M - MT_N], p[0], state->s[0]);

	state->i = 0;
}

static uint64_t mt19937_generate(void *state)
{
	mt19937_state *s = (mt19937_state *)state;
	uint32_t s1;

	if (s->i >= MT_N) {
		mt19937_reload(s);
	}

	s1 = s->s[s->i++];
	s1 ^= (s1 >> 11);
	s1 ^= (s1 << 7) & 0x9d2c5680U;
	s1 ^= (s1 << 15) & 0xefc60000U;
	return ( s1 ^ (s1 >> 18) );
}

static void mt19937_seed(void *state, const zend_long seed)
{
	mt19937_state *s = (mt19937_state *)state;

	s->s[0] = seed & 0xffffffffU;
	for (s->i = 1; s->i < MT_N; s->i++) {
		s->s[s->i] = (1812433253U * (s->s[s->i - 1] ^ (s->s[s->i - 1] >> 30)) + s->i) & 0xffffffffU;
	}
	mt19937_reload(s);
}

static int mt19937_serialize(void *state, HashTable *data)
{
	mt19937_state *s = (mt19937_state *)state;
	zval tmp;
	int i;

	for (i = 0; i< MT_N + 1; i++) {
		ZVAL_LONG(&tmp, s->s[i]);
		zend_hash_next_index_insert(data, &tmp);
	}
	ZVAL_LONG(&tmp, s->i);
	zend_hash_next_index_insert(data, &tmp);

	return SUCCESS;
}

static int mt19937_unserialize(void *state, HashTable *data)
{
	mt19937_state *s = (mt19937_state *)state;
	zval *tmp;
	int i;

	for (i = 0; i < MT_N + 1; i++) {
		tmp = zend_hash_index_find(data, i);
		if (!tmp || Z_TYPE_P(tmp) != IS_LONG) {
			return FAILURE;
		}

		s->s[i] = Z_LVAL_P(tmp);
	}
	tmp = zend_hash_index_find(data, MT_N + 1);
	if (Z_TYPE_P(tmp) != IS_LONG) {
		return FAILURE;
	}
	s->i = Z_LVAL_P(tmp);

	return SUCCESS;
}

static uint64_t secure_generate(void *state)
{
	uint64_t ret;

	php_random_bytes_silent(&ret, sizeof(uint64_t));

	return ret;
}

static uint64_t user_generate(void *state)
{
	user_state *s = (user_state *)state;
	zval retval;

	zend_call_known_instance_method_with_0_params(s->func, s->obj, &retval);

	return Z_LVAL(retval);
}

const php_random_algo php_random_ng_algo_xorshift128plus = {
	sizeof(uint64_t),
	sizeof(xorshift128plus_state),
	xorshift128plus_generate,
	xorshift128plus_seed,
	xorshift128plus_serialize,
	xorshift128plus_unserialize
};

const php_random_algo php_random_ng_algo_mt19937 = {
	sizeof(uint32_t),
	sizeof(mt19937_state),
	mt19937_generate,
	mt19937_seed,
	mt19937_serialize,
	mt19937_unserialize
};

const php_random_algo php_random_ng_algo_secure = {
	sizeof(uint64_t),
	0,
	secure_generate,
	NULL,
	NULL,
	NULL
};

const php_random_algo php_random_ng_algo_user = {
	sizeof(zend_long),
	sizeof(user_state),
	user_generate,
	NULL,
	NULL,
	NULL
};

/* {{{ ng_common_new */
static inline php_random_ng *ng_common_new(zend_class_entry *ce, const php_random_algo *algo, const zend_object_handlers *handlers)
{
	php_random_ng *rng = zend_object_alloc(sizeof(php_random_ng), ce);

	zend_object_std_init(&rng->std, ce);
	object_properties_init(&rng->std, ce);

	rng->algo = algo;
	if (rng->algo->state_size > 0) {
		rng->state = emalloc(algo->state_size);
	}
	rng->std.handlers = handlers;

	return rng;
}
/* }}} */

/* {{{ ng_xorshift128plus_new */
static zend_object *ng_xorshift128plus_new(zend_class_entry *ce)
{
	return &ng_common_new(ce, &php_random_ng_algo_xorshift128plus, &random_number_generator_xorshift128plus_object_handlers)->std;
}
/* }}} */

/* {{{ ng_mt19937_new */
static zend_object *ng_mt19937_new(zend_class_entry *ce)
{
	return &ng_common_new(ce, &php_random_ng_algo_mt19937, &random_number_generator_mt19937_object_handlers)->std;
}
/* }}} */

/* {{{ ng_secure_new */
static zend_object *ng_secure_new(zend_class_entry *ce)
{
	return &ng_common_new(ce, &php_random_ng_algo_secure, &random_number_generator_secure_object_handlers)->std;
}
/* }}} */

/* {{{ ng_abstract_new */
static zend_object *ng_abstract_new(zend_class_entry *ce)
{
	php_random_ng *rng = ng_common_new(ce, &php_random_ng_algo_user, &random_number_generator_object_handlers);
	user_state *state = (user_state *)rng->state;

	state->func = zend_hash_str_find_ptr(&ce->function_table, "generate", sizeof("generate") - 1);
	state->obj = &rng->std;

	return &rng->std;
}
/* }}} */

/* {{{ ng_common_free_obj */
static void ng_common_free_obj(zend_object *object)
{
	php_random_ng *rng = php_random_ng_from_obj(object);

	if (rng->state) {
		efree(rng->state);
	}

	zend_object_std_dtor(object);
}
/* }}} */

/* {{{ ng_common_clone_obj */
static zend_object *ng_common_clone_obj(zend_object *old_object)
{
	php_random_ng	*old_rng = php_random_ng_from_obj(old_object),
					*new_rng = php_random_ng_from_obj(old_object->ce->create_object(old_object->ce));

	zend_objects_clone_members(&new_rng->std, &old_rng->std);

	new_rng->algo = old_rng->algo;
	if (old_rng->state) {
		memcpy(new_rng->state, old_rng->state, old_rng->algo->state_size);
	}

	return &new_rng->std;
}
/* }}} */

/* {{{ new */
static zend_object *new(zend_class_entry *ce)
{
	php_random *random = zend_object_alloc(sizeof(php_random), ce);

	zend_object_std_init(&random->std, ce);
	object_properties_init(&random->std, ce);
	random->std.handlers = &random_object_handlers;

	return &random->std;
}
/* }}} */

/* {{{ free_obj */
static void free_obj(zend_object *object)
{
	php_random *random = php_random_from_obj(object);
	zend_object_std_dtor(&random->std);
}
/* }}} */

static uint32_t rng_rand_range32(php_random_ng *rng, uint32_t umax) {
	uint32_t result, limit;

	result = rng->algo->generate(rng->state);

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
		result = rng->algo->generate(rng->state);
	}

	return result % umax;
}

#if ZEND_ULONG_MAX > UINT32_MAX
static uint64_t rng_rand_range64(php_random_ng *rng, uint64_t umax) {
	uint64_t result, limit;

	result = rng->algo->generate(rng->state);
	result = (result << 32) | rng->algo->generate(rng->state);

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
		result = rng->algo->generate(rng->state);
		result = (result << 32) | rng->algo->generate(rng->state);
	}

	return result % umax;
}
#endif

/* {{{ php_random_ng_next */
PHPAPI uint64_t php_random_ng_next(php_random_ng *rng)
{
	return rng->algo->generate(rng->state);
}
/* }}} */

/* {{{ php_random_ng_next */
PHPAPI zend_long php_random_ng_range(php_random_ng *rng, zend_long min, zend_long max)
{
	zend_ulong umax = max - min;

#if ZEND_ULONG_MAX > UINT32_MAX
	if (umax > UINT32_MAX) {
		return (zend_long) (rng_rand_range64(rng, umax) + min);
	}
#endif

	return (zend_long) (rng_rand_range32(rng, umax) + min);
}
/* }}} */

/* {{{ php_random_ng_array_data_shuffle */
PHPAPI void php_random_ng_array_data_shuffle(php_random_ng *rng, HashTable *ht)
{
	uint32_t idx, j, n_elems;
	Bucket *p, temp;
	zend_long rnd_idx;
	uint32_t n_left;

	n_elems = zend_hash_num_elements(ht);

	if (n_elems < 1) {
		return;
	}

	n_left = n_elems;

	if (EXPECTED(!HT_HAS_ITERATORS(ht))) {
		if (ht->nNumUsed != ht->nNumOfElements) {
			for (j = 0, idx = 0; idx < ht->nNumUsed; idx++) {
				p = ht->arData + idx;
				if (Z_TYPE(p->val) == IS_UNDEF) continue;
				if (j != idx) {
					ht->arData[j] = *p;
				}
				j++;
			}
		}
		while (--n_left) {
			rnd_idx = php_random_ng_range(rng, 0, n_left);
			if (rnd_idx != n_left) {
				temp = ht->arData[n_left];
				ht->arData[n_left] = ht->arData[rnd_idx];
				ht->arData[rnd_idx] = temp;
			}
		}
	} else {
		uint32_t iter_pos = zend_hash_iterators_lower_pos(ht, 0);

		if (ht->nNumUsed != ht->nNumOfElements) {
			for (j = 0, idx = 0; idx < ht->nNumUsed; idx++) {
				p = ht->arData + idx;
				if (Z_TYPE(p->val) == IS_UNDEF) continue;
				if (j != idx) {
					ht->arData[j] = *p;
					if (idx == iter_pos) {
						zend_hash_iterators_update(ht, idx, j);
						iter_pos = zend_hash_iterators_lower_pos(ht, iter_pos + 1);
					}
				}
				j++;
			}
		}
		while (--n_left) {
			rnd_idx = php_random_ng_range(rng, 0, n_left);
			if (rnd_idx != n_left) {
				temp = ht->arData[n_left];
				ht->arData[n_left] = ht->arData[rnd_idx];
				ht->arData[rnd_idx] = temp;
				zend_hash_iterators_update(ht, (uint32_t)rnd_idx, n_left);
			}
		}
	}
	ht->nNumUsed = n_elems;
	ht->nInternalPointer = 0;

	for (j = 0; j < n_elems; j++) {
		p = ht->arData + j;
		if (p->key) {
			zend_string_release_ex(p->key, 0);
		}
		p->h = j;
		p->key = NULL;
	}
	ht->nNextFreeElement = n_elems;
	if (!(HT_FLAGS(ht) & HASH_FLAG_PACKED)) {
		zend_hash_to_packed(ht);
	}
}
/* }}} */

/* {{{ php_random_ng_string_shuffle */
PHPAPI void php_random_ng_string_shuffle(php_random_ng *rng, zend_string *string)
{
	zend_long n_elems, rnd_idx, n_left;
	char temp;
	/* The implementation is stolen from array_data_shuffle       */
	/* Thus the characteristics of the randomization are the same */
	n_elems = ZSTR_LEN(string);

	if (n_elems <= 1) {
		return;
	}

	n_left = n_elems;

	while (--n_left) {
		rnd_idx = php_random_ng_range(rng, 0, n_left);
		if (rnd_idx != n_left) {
			temp = ZSTR_VAL(string)[n_left];
			ZSTR_VAL(string)[n_left] = ZSTR_VAL(string)[rnd_idx];
			ZSTR_VAL(string)[rnd_idx] = temp;
		}
	}
}
/* }}} */

/* {{{ Returns a value from the combined linear congruential generator */
PHP_FUNCTION(lcg_value)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_DOUBLE(php_combined_lcg());
}
/* }}} */

/* {{{ Returns a random number from Mersenne Twister */
PHP_FUNCTION(rand)
{
	zend_long min;
	zend_long max;
	int argc = ZEND_NUM_ARGS();

	if (argc == 0) {
		RETURN_LONG(php_mt_rand() >> 1);
	}

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(min)
		Z_PARAM_LONG(max)
	ZEND_PARSE_PARAMETERS_END();

	if (max < min) {
		RETURN_LONG(php_mt_rand_common(max, min));
	}

	RETURN_LONG(php_mt_rand_common(min, max));
}
/* }}} */

/* {{{ Seeds Mersenne Twister random number generator */
PHP_FUNCTION(mt_srand)
{
	zend_long seed = 0;
	zend_long mode = MT_RAND_MT19937;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(seed)
		Z_PARAM_LONG(mode)
	ZEND_PARSE_PARAMETERS_END();

	if (ZEND_NUM_ARGS() == 0) {
		if (php_random_bytes_silent(&seed, sizeof(zend_long)) == FAILURE) {
			seed = GENERATE_SEED();
		}
	}

	switch (mode) {
		case MT_RAND_PHP:
			RANDOM_G(mt_rand_mode) = MT_RAND_PHP;
			break;
		default:
			RANDOM_G(mt_rand_mode) = MT_RAND_MT19937;
	}

	php_mt_srand(seed);
}
/* }}} */

/* {{{ Returns a random number from Mersenne Twister */
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
		zend_argument_value_error(2, "must be greater than or equal to argument #1 ($min)");
		RETURN_THROWS();
	}

	RETURN_LONG(php_mt_rand_common(min, max));
}
/* }}} */

/* {{{ Returns the maximum value a random number from Mersenne Twister can have */
PHP_FUNCTION(mt_getrandmax)
{
	ZEND_PARSE_PARAMETERS_NONE();

	/*
	 * Melo: it could be 2^^32 but we only use 2^^31 to maintain
	 * compatibility with the previous php_rand
	 */
	RETURN_LONG(PHP_MT_RAND_MAX); /* 2^^31 */
}
/* }}} */

/* {{{ Return an arbitrary length of pseudo-random bytes as binary string */
PHP_FUNCTION(random_bytes)
{
	zend_long size;
	zend_string *bytes;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(size)
	ZEND_PARSE_PARAMETERS_END();

	if (size < 1) {
		zend_argument_value_error(1, "must be greater than 0");
		RETURN_THROWS();
	}

	bytes = zend_string_alloc(size, 0);

	if (php_random_bytes_throw(ZSTR_VAL(bytes), size) == FAILURE) {
		zend_string_release_ex(bytes, 0);
		RETURN_THROWS();
	}

	ZSTR_VAL(bytes)[size] = '\0';

	RETURN_STR(bytes);
}
/* }}} */

/* {{{ Return an arbitrary pseudo-random integer */
PHP_FUNCTION(random_int)
{
	zend_long min;
	zend_long max;
	zend_long result;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(min)
		Z_PARAM_LONG(max)
	ZEND_PARSE_PARAMETERS_END();

	if (min > max) {
		zend_argument_value_error(1, "must be less than or equal to argument #2 ($max)");
		RETURN_THROWS();
	}

	if (php_random_int_throw(min, max, &result) == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_LONG(result);
}
/* }}} */

/* {{{ XorShift128Plus/MT19937 constructor */
PHP_METHOD(Random_NumberGenerator_XorShift128Plus, __construct)
{
	php_random_ng *rng = Z_RANDOM_NG_P(ZEND_THIS);
	zend_long seed;
	bool is_null = true;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(seed, is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (is_null && (php_random_bytes_throw(&seed, sizeof(zend_long)) == FAILURE)) {
		RETURN_THROWS();
	}

	if (rng->algo->seed) {
		rng->algo->seed(rng->state, seed);
	}
}
/* }}} */

/* {{{ Return XorShift128Plus/MT19937 generated pseudo-random number */
PHP_METHOD(Random_NumberGenerator_XorShift128Plus, generate)
{
	php_random_ng *rng = Z_RANDOM_NG_P(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_LONG((zend_long) php_random_ng_next(rng));
}
/* }}} */

/* {{{ Serialize XorShift128Plus/MT19937 instance */
PHP_METHOD(Random_NumberGenerator_XorShift128Plus, __serialize)
{
	php_random_ng *rng = Z_RANDOM_NG_P(ZEND_THIS);
	zval tmp;

	ZEND_PARSE_PARAMETERS_NONE();

	if (!rng->algo->serialize) {
		zend_throw_exception(NULL, "Algorithm does not support serialize", 0);
		RETURN_THROWS();
	}

	array_init(return_value);

	/* members */
	ZVAL_ARR(&tmp, zend_std_get_properties(&rng->std));
	Z_TRY_ADDREF(tmp);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &tmp);

	/* state */
	array_init(&tmp);
	rng->algo->serialize(rng->state, Z_ARRVAL(tmp));
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &tmp);
}
/* }}} */

/* {{{ Unserialize XorShift128Plus/MT19937 instance */
PHP_METHOD(Random_NumberGenerator_XorShift128Plus, __unserialize)
{
	php_random_ng *rng = Z_RANDOM_NG_P(ZEND_THIS);
	HashTable *data;
	zval *tmp;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(data);
	ZEND_PARSE_PARAMETERS_END();

	if (!rng->algo->unserialize) {
		zend_throw_exception(NULL, "Algorithm does not support unserialize", 0);
		RETURN_THROWS();
	}

	/* members */
	tmp = zend_hash_index_find(data, 0);
	if (!tmp || Z_TYPE_P(tmp) != IS_ARRAY) {
		zend_throw_exception(NULL, "Incomplete or ill-formed serialization data", 0);
		RETURN_THROWS();
	}
	object_properties_load(&rng->std, Z_ARRVAL_P(tmp));

	/* state */
	tmp = zend_hash_index_find(data, 1);
	if (!tmp || Z_TYPE_P(tmp) != IS_ARRAY) {
		zend_throw_exception(NULL, "Incomplete or ill-formed serialization data", 0);
		RETURN_THROWS();
	}
	rng->algo->unserialize(rng->state, Z_ARRVAL_P(tmp));
}
/* }}} */

/* {{{ CSPRNG constructor */
PHP_METHOD(Random_NumberGenerator_Secure, __construct)
{
	ZEND_PARSE_PARAMETERS_NONE();
}
/* }}} */

/* {{{ Random utilization class constructor */
PHP_METHOD(Random, __construct)
{
	php_random *random = Z_RANDOM_P(ZEND_THIS);
	zend_object *obj = 0;
	zval zobj;
	zend_string *pname;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJ_OF_CLASS_OR_NULL(obj, random_ce_Random_NumberGenerator);
	ZEND_PARSE_PARAMETERS_END();

	if (!obj) {
		obj = ng_xorshift128plus_new(random_ce_Random_NumberGenerator_XorShift128Plus);
		zend_call_known_instance_method_with_0_params(
			random_ce_Random_NumberGenerator_XorShift128Plus->constructor,
			obj,
			NULL
		);
		/* No needed self-refcount */
		GC_DELREF(obj);
	}

	ZVAL_OBJ(&zobj, obj);

	pname = zend_string_init("rng", sizeof("rng") - 1, 0);
	zend_std_write_property(&random->std, pname, &zobj, NULL);
	zend_string_release(pname);

	random->rng = php_random_ng_from_obj(obj);
}
/* }}} */

/* {{{ Return internal RNG instance */
PHP_METHOD(Random, getNumberGenerator)
{
	php_random *random = Z_RANDOM_P(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_OBJ_COPY(&random->rng->std);
}
/* }}} */

/* {{{ Return an arbitrary pseudo-random integer with RNG */
PHP_METHOD(Random, getInt)
{
	php_random *random = Z_RANDOM_P(ZEND_THIS);
	zend_long min, max;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(min)
		Z_PARAM_LONG(max)
	ZEND_PARSE_PARAMETERS_END();

	if (UNEXPECTED(max < min)) {
		zend_argument_value_error(2, "must be greater than or equal to argument #1 ($min)");
		RETURN_THROWS();
	}

	RETURN_LONG(php_random_ng_range(random->rng, min, max));
}
/* }}} */

/* {{{ Return an arbitrary length of pseudo-random bytes as binary string with RNG */
PHP_METHOD(Random, getBytes)
{
	php_random *random = Z_RANDOM_P(ZEND_THIS);
	zend_string *ret;
	zend_long size;
	uint64_t buf;
	uint8_t *bytes;
	size_t generated_bytes = 0;
	int i;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(size)
	ZEND_PARSE_PARAMETERS_END();

	if (size < 1) {
		zend_argument_value_error(1, "must be greater than 0");
		RETURN_THROWS();
	}

	ret = zend_string_alloc(size, 0);

	while (generated_bytes <= size) {
		buf = php_random_ng_next(random->rng);
		if (random->rng && random->rng->algo->generate_size == sizeof(uint32_t)) {
			buf = (buf << 32) | php_random_ng_next(random->rng);
		}
		bytes = (uint8_t *) &buf;
		for (i = 0; i < (sizeof(uint64_t) / sizeof(uint8_t)); i ++) {
			ZSTR_VAL(ret)[generated_bytes + i] = bytes[i];

			if ((generated_bytes + i) >= size) {
				ZSTR_VAL(ret)[size] = '\0';
				RETURN_STR(ret);
			}
		}
		generated_bytes += (sizeof(uint64_t) / sizeof(uint8_t));
	}
}
/* }}} */

/* {{{ Randomly shuffle the contents of an array with RNG */
PHP_METHOD(Random, shuffleArray)
{
	php_random *random = Z_RANDOM_P(ZEND_THIS);
	zval *array;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY(array)
	ZEND_PARSE_PARAMETERS_END();

	ZVAL_DUP(return_value, array);
	php_random_ng_array_data_shuffle(random->rng, Z_ARRVAL_P(return_value));
}
/* }}} */

/* {{{ Shuffles string with RNG. One permutation of all possible is created */
PHP_METHOD(Random, shuffleString)
{
	php_random *random = Z_RANDOM_P(ZEND_THIS);
	zend_string *string, *retval;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(string)
	ZEND_PARSE_PARAMETERS_END();

	if (ZSTR_LEN(string) < 2) {
		RETURN_STR_COPY(string);
	}

	retval = zend_string_init(ZSTR_VAL(string), ZSTR_LEN(string), false);
	php_random_ng_string_shuffle(random->rng, retval);
	RETURN_STR(retval);
}
/* }}} */

/* {{{ Serialize Random instance */
PHP_METHOD(Random, __serialize)
{
	php_random *random = Z_RANDOM_P(ZEND_THIS);
	zval buf;

	ZEND_PARSE_PARAMETERS_NONE();

	array_init(return_value);

	ZVAL_ARR(&buf, zend_std_get_properties(&random->std));
	Z_TRY_ADDREF(buf);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &buf);
}
/* }}} */

/* {{{ Unserialize Random instance */
PHP_METHOD(Random, __unserialize)
{
	php_random *random = Z_RANDOM_P(ZEND_THIS);
	HashTable *data;
	zval *members_zv;
	zval *zobj;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(data);
	ZEND_PARSE_PARAMETERS_END();

	members_zv = zend_hash_index_find(data, 0);
	if (!members_zv || Z_TYPE_P(members_zv) != IS_ARRAY) {
		zend_throw_exception(NULL, "Incomplete or ill-formed serialization data", 0);
		RETURN_THROWS();
	}
	object_properties_load(&random->std, Z_ARRVAL_P(members_zv));

	zobj = zend_read_property(random->std.ce, &random->std, "rng", sizeof("rng") - 1, 0, NULL);
	if (Z_TYPE_P(zobj) != IS_OBJECT || !instanceof_function(Z_OBJCE_P(zobj), random_ce_Random_NumberGenerator)) {
		zend_throw_exception(NULL, "Incomplete or ill-formed serialization data", 0);
		RETURN_THROWS();
	}

	random->rng = Z_RANDOM_NG_P(zobj);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(random)
{
#if defined(ZTS) && defined(COMPILE_DL_COLOPL_BC)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	REGISTER_LONG_CONSTANT("MT_RAND_MT19937", MT_RAND_MT19937, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MT_RAND_PHP",     MT_RAND_PHP, CONST_CS | CONST_PERSISTENT);

	random_ce_Random = register_class_Random();
	random_ce_Random->create_object = new;
	memcpy(&random_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	random_object_handlers.offset = XtOffsetOf(php_random, std);
	random_object_handlers.free_obj = free_obj;
	random_object_handlers.clone_obj = NULL;

	random_ce_Random_NumberGenerator = register_class_Random_NumberGenerator();
	random_ce_Random_NumberGenerator->create_object = ng_abstract_new;
	memcpy(&random_number_generator_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	random_number_generator_object_handlers.offset = XtOffsetOf(php_random_ng, std);
	random_number_generator_object_handlers.free_obj = ng_common_free_obj;
	random_number_generator_object_handlers.clone_obj = ng_common_clone_obj;

	random_ce_Random_NumberGenerator_XorShift128Plus = register_class_Random_NumberGenerator_XorShift128Plus(random_ce_Random_NumberGenerator);
	random_ce_Random_NumberGenerator_XorShift128Plus->create_object = ng_xorshift128plus_new;
	memcpy(&random_number_generator_xorshift128plus_object_handlers, &random_number_generator_object_handlers, sizeof(zend_object_handlers));

	random_ce_random_NumberGenerator_MT19937 = register_class_Random_NumberGenerator_MT19937(random_ce_Random_NumberGenerator);
	random_ce_random_NumberGenerator_MT19937->create_object = ng_mt19937_new;
	memcpy(&random_number_generator_mt19937_object_handlers, &random_number_generator_object_handlers, sizeof(zend_object_handlers));

	random_ce_Random_NumberGenerator_Secure = register_class_Random_NumberGenerator_Secure(random_ce_Random_NumberGenerator);
	random_ce_Random_NumberGenerator_Secure->create_object = ng_secure_new;
	memcpy(&random_number_generator_secure_object_handlers, &random_number_generator_object_handlers, sizeof(zend_object_handlers));
	random_number_generator_secure_object_handlers.clone_obj = NULL;

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(random)
{
#if defined(ZTS) && defined(COMPILE_DL_COLOPL_BC)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION */
PHP_RSHUTDOWN_FUNCTION(random)
{
	RANDOM_G(mt_rand_is_seeded) = 0;

	return SUCCESS;
}
/* }}} */

/* {{{ random_module_entry */
zend_module_entry random_module_entry = {
	STANDARD_MODULE_HEADER,
	"random",
	ext_functions,
	PHP_MINIT(random),
	NULL,
	NULL,
	PHP_RSHUTDOWN(random),
	NULL,
	PHP_VERSION,
	PHP_MODULE_GLOBALS(random),
	NULL,
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */
