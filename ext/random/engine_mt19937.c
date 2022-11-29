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
   |          Pedro Melo <melo@ip.pt>                                     |
   |          Sterling Hughes <sterling@php.net>                          |
   |          Go Kudo <zeriyoshi@php.net>                                 |
   |                                                                      |
   | Based on code from: Richard J. Wagner <rjwagner@writeme.com>         |
   |                     Makoto Matsumoto <matumoto@math.keio.ac.jp>      |
   |                     Takuji Nishimura                                 |
   |                     Shawn Cokus <Cokus@math.washington.edu>          |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_random.h"

#include "Zend/zend_exceptions.h"

/*
	The following mt19937 algorithms are based on a C++ class MTRand by
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
#define mixBits(u, v) (hiBit(u) | loBits(v)) /* move hi bit of u to hi bit of v */

#define twist(m,u,v)  (m ^ (mixBits(u,v) >> 1) ^ ((uint32_t)(-(int32_t)(loBit(v))) & 0x9908b0dfU))
#define twist_php(m,u,v)  (m ^ (mixBits(u,v) >> 1) ^ ((uint32_t)(-(int32_t)(loBit(u))) & 0x9908b0dfU))

static inline void mt19937_reload(php_random_status_state_mt19937 *state)
{
	uint32_t *p = state->state;

	if (state->mode == MT_RAND_MT19937) {
		for (uint32_t i = N - M; i--; ++p) {
			*p = twist(p[M], p[0], p[1]);
		}
		for (uint32_t i = M; --i; ++p) {
			*p = twist(p[M-N], p[0], p[1]);
		}
		*p = twist(p[M-N], p[0], state->state[0]);
	} else {
		for (uint32_t i = N - M; i--; ++p) {
			*p = twist_php(p[M], p[0], p[1]);
		}
		for (uint32_t i = M; --i; ++p) {
			*p = twist_php(p[M-N], p[0], p[1]);
		}
		*p = twist_php(p[M-N], p[0], state->state[0]);
	}

	state->count = 0;
}

static inline void mt19937_seed_state(php_random_status_state_mt19937 *state, uint64_t seed)
{
	uint32_t i, prev_state;

	/* Initialize generator state with seed
	   See Knuth TAOCP Vol 2, 3rd Ed, p.106 for multiplier.
	   In previous versions, most significant bits (MSBs) of the seed affect
	   only MSBs of the state array.  Modified 9 Jan 2002 by Makoto Matsumoto. */
	state->state[0] = seed & 0xffffffffU;
	for (i = 1; i < MT_N; i++) {
		prev_state = state->state[i - 1];
		state->state[i] = (1812433253U * (prev_state  ^ (prev_state  >> 30)) + i) & 0xffffffffU;
	}
	state->count = i;

	mt19937_reload(state);
}

static void seed(php_random_status *status, uint64_t seed)
{
	mt19937_seed_state(status->state, seed);
}

static uint64_t generate(php_random_status *status)
{
	php_random_status_state_mt19937 *s = status->state;
	uint32_t s1;

	if (s->count >= MT_N) {
		mt19937_reload(s);
	}

	s1 = s->state[s->count++];
	s1 ^= (s1 >> 11);
	s1 ^= (s1 << 7) & 0x9d2c5680U;
	s1 ^= (s1 << 15) & 0xefc60000U;

	return (uint64_t) (s1 ^ (s1 >> 18));
}

static zend_long range(php_random_status *status, zend_long min, zend_long max)
{
	return php_random_range(&php_random_algo_mt19937, status, min, max);
}

static bool serialize(php_random_status *status, HashTable *data)
{
	php_random_status_state_mt19937 *s = status->state;
	zval t;

	for (uint32_t i = 0; i < MT_N; i++) {
		ZVAL_STR(&t, php_random_bin2hex_le(&s->state[i], sizeof(uint32_t)));
		zend_hash_next_index_insert(data, &t);
	}
	ZVAL_LONG(&t, s->count);
	zend_hash_next_index_insert(data, &t);
	ZVAL_LONG(&t, s->mode);
	zend_hash_next_index_insert(data, &t);

	return true;
}

static bool unserialize(php_random_status *status, HashTable *data)
{
	php_random_status_state_mt19937 *s = status->state;
	zval *t;

	/* Verify the expected number of elements, this implicitly ensures that no additional elements are present. */
	if (zend_hash_num_elements(data) != (MT_N + 2)) {
		return false;
	}

	for (uint32_t i = 0; i < MT_N; i++) {
		t = zend_hash_index_find(data, i);
		if (!t || Z_TYPE_P(t) != IS_STRING || Z_STRLEN_P(t) != (2 * sizeof(uint32_t))) {
			return false;
		}
		if (!php_random_hex2bin_le(Z_STR_P(t), &s->state[i])) {
			return false;
		}
	}
	t = zend_hash_index_find(data, MT_N);
	if (!t || Z_TYPE_P(t) != IS_LONG) {
		return false;
	}
	s->count = Z_LVAL_P(t);
	if (s->count > MT_N) {
		return false;
	}

	t = zend_hash_index_find(data, MT_N + 1);
	if (!t || Z_TYPE_P(t) != IS_LONG) {
		return false;
	}
	s->mode = Z_LVAL_P(t);
	if (s->mode != MT_RAND_MT19937 && s->mode != MT_RAND_PHP) {
		return false;
	}

	return true;
}

const php_random_algo php_random_algo_mt19937 = {
	sizeof(uint32_t),
	sizeof(php_random_status_state_mt19937),
	seed,
	generate,
	range,
	serialize,
	unserialize
};

/* {{{ php_random_mt19937_seed_default */
PHPAPI void php_random_mt19937_seed_default(php_random_status_state_mt19937 *state)
{
	zend_long seed = 0;

	if (php_random_bytes_silent(&seed, sizeof(zend_long)) == FAILURE) {
		seed = GENERATE_SEED();
	}

	mt19937_seed_state(state, (uint64_t) seed);
}
/* }}} */

/* {{{ Random\Engine\Mt19937::__construct() */
PHP_METHOD(Random_Engine_Mt19937, __construct)
{
	php_random_engine *engine = Z_RANDOM_ENGINE_P(ZEND_THIS);
	php_random_status_state_mt19937 *state = engine->status->state;
	zend_long seed, mode = MT_RAND_MT19937;
	bool seed_is_null = true;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL;
		Z_PARAM_LONG_OR_NULL(seed, seed_is_null);
		Z_PARAM_LONG(mode);
	ZEND_PARSE_PARAMETERS_END();

	switch (mode) {
		case MT_RAND_MT19937:
			state->mode = MT_RAND_MT19937;
			break;
		case MT_RAND_PHP:
			state->mode = MT_RAND_PHP;
			break;
		default:
			zend_argument_value_error(2, "must be either MT_RAND_MT19937 or MT_RAND_PHP");
			RETURN_THROWS();
	}

	if (seed_is_null) {
		/* MT19937 has a very large state, uses CSPRNG for seeding only */
		if (php_random_bytes_throw(&seed, sizeof(zend_long)) == FAILURE) {
			zend_throw_exception(random_ce_Random_RandomException, "Failed to generate a random seed", 0);
			RETURN_THROWS();
		}
	}

	engine->algo->seed(engine->status, seed);
}
/* }}} */

/* {{{ Random\Engine\Mt19937::generate() */
PHP_METHOD(Random_Engine_Mt19937, generate)
{
	php_random_engine *engine = Z_RANDOM_ENGINE_P(ZEND_THIS);
	uint64_t generated;
	size_t size;
	zend_string *bytes;

	ZEND_PARSE_PARAMETERS_NONE();

	generated = engine->algo->generate(engine->status);
	size = engine->status->last_generated_size;
	if (EG(exception)) {
		RETURN_THROWS();
	}

	bytes = zend_string_alloc(size, false);

	/* Endianness safe copy */
	for (size_t i = 0; i < size; i++) {
		ZSTR_VAL(bytes)[i] = (generated >> (i * 8)) & 0xff;
	}
	ZSTR_VAL(bytes)[size] = '\0';

	RETURN_STR(bytes);
}
/* }}} */

/* {{{ Random\Engine\Mt19937::__serialize() */
PHP_METHOD(Random_Engine_Mt19937, __serialize)
{
	php_random_engine *engine = Z_RANDOM_ENGINE_P(ZEND_THIS);
	zval t;

	ZEND_PARSE_PARAMETERS_NONE();

	array_init(return_value);

	/* members */
	ZVAL_ARR(&t, zend_std_get_properties(&engine->std));
	Z_TRY_ADDREF(t);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &t);

	/* state */
	array_init(&t);
	if (!engine->algo->serialize(engine->status, Z_ARRVAL(t))) {
		zend_throw_exception(NULL, "Engine serialize failed", 0);
		RETURN_THROWS();
	}
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &t);
}
/* }}} */

/* {{{ Random\Engine\Mt19937::__unserialize() */
PHP_METHOD(Random_Engine_Mt19937, __unserialize)
{
	php_random_engine *engine = Z_RANDOM_ENGINE_P(ZEND_THIS);
	HashTable *d;
	zval *t;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(d);
	ZEND_PARSE_PARAMETERS_END();

	/* Verify the expected number of elements, this implicitly ensures that no additional elements are present. */
	if (zend_hash_num_elements(d) != 2) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(engine->std.ce->name));
		RETURN_THROWS();
	}

	/* members */
	t = zend_hash_index_find(d, 0);
	if (!t || Z_TYPE_P(t) != IS_ARRAY) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(engine->std.ce->name));
		RETURN_THROWS();
	}
	object_properties_load(&engine->std, Z_ARRVAL_P(t));
	if (EG(exception)) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(engine->std.ce->name));
		RETURN_THROWS();
	}

	/* state */
	t = zend_hash_index_find(d, 1);
	if (!t || Z_TYPE_P(t) != IS_ARRAY) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(engine->std.ce->name));
		RETURN_THROWS();
	}
	if (!engine->algo->unserialize(engine->status, Z_ARRVAL_P(t))) {
		zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(engine->std.ce->name));
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ Random\Engine\Mt19937::__debugInfo() */
PHP_METHOD(Random_Engine_Mt19937, __debugInfo)
{
	php_random_engine *engine = Z_RANDOM_ENGINE_P(ZEND_THIS);
	zval t;

	ZEND_PARSE_PARAMETERS_NONE();

	if (!engine->std.properties) {
		rebuild_object_properties(&engine->std);
	}
	ZVAL_ARR(return_value, zend_array_dup(engine->std.properties));

	if (engine->algo->serialize) {
		array_init(&t);
		if (!engine->algo->serialize(engine->status, Z_ARRVAL(t))) {
			zend_throw_exception(NULL, "Engine serialize failed", 0);
			RETURN_THROWS();
		}
		zend_hash_str_add(Z_ARR_P(return_value), "__states", strlen("__states"), &t);
	}
}
/* }}} */
