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
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   |          Go Kudo <zeriyoshi@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_random.h"

#include "Zend/zend_exceptions.h"

/*
 * combinedLCG() returns a pseudo random number in the range of (0, 1).
 * The function combines two CGs with periods of
 * 2^31 - 85 - 1 and 2^31 - 249 - 1. The period of this function
 * is equal to the product of the two underlying periods, divided
 * by factors shared by the underlying periods, i.e. 2.3 * 10^18.
 *
 * see: https://library.sciencemadness.org/lanl1_a/lib-www/numerica/f7-1.pdf
 */
#define MODMULT(a, b, c, m, s) q = s / a; s = b * (s - a * q) - c * q; if (s < 0) s += m

PHPAPI void php_random_combinedlcg_seed64(php_random_status_state_combinedlcg *state, uint64_t seed)
{
	state->state[0] = seed & 0xffffffffU;
	state->state[1] = seed >> 32;
}

static php_random_result generate(void *state)
{
	php_random_status_state_combinedlcg *s = state;
	int32_t q, z;

	/* s->state[0] = (s->state[0] * 40014) % 2147483563; */
	MODMULT(53668, 40014, 12211, 2147483563L, s->state[0]);
	/* s->state[1] = (s->state[1] * 40692) % 2147483399; */
	MODMULT(52774, 40692, 3791, 2147483399L, s->state[1]);

	z = s->state[0] - s->state[1];
	if (z < 1) {
		z += 2147483562;
	}

	return (php_random_result){
		.size = sizeof(uint32_t),
		.result = (uint64_t) z,
	};
}

static zend_long range(void *state, zend_long min, zend_long max)
{
	return php_random_range((php_random_algo_with_state){
		.algo = &php_random_algo_combinedlcg,
		.state = state,
	}, min, max);
}

static bool serialize(void *state, HashTable *data)
{
	php_random_status_state_combinedlcg *s = state;
	zval t;

	for (uint32_t i = 0; i < 2; i++) {
		ZVAL_STR(&t, php_random_bin2hex_le(&s->state[i], sizeof(uint32_t)));
		zend_hash_next_index_insert(data, &t);
	}

	return true;
}

static bool unserialize(void *state, HashTable *data)
{
	php_random_status_state_combinedlcg *s = state;
	zval *t;

	for (uint32_t i = 0; i < 2; i++) {
		t = zend_hash_index_find(data, i);
		if (!t || Z_TYPE_P(t) != IS_STRING || Z_STRLEN_P(t) != (2 * sizeof(uint32_t))) {
			return false;
		}
		if (!php_random_hex2bin_le(Z_STR_P(t), &s->state[i])) {
			return false;
		}
	}

	return true;
}

PHPAPI const php_random_algo php_random_algo_combinedlcg = {
	sizeof(php_random_status_state_combinedlcg),
	generate,
	range,
	serialize,
	unserialize
};

/* {{{ php_random_combinedlcg_seed_default */
PHPAPI void php_random_combinedlcg_seed_default(php_random_status_state_combinedlcg *state)
{
	uint64_t seed = 0;

	if (php_random_bytes_silent(&seed, sizeof(seed)) == FAILURE) {
		seed = php_random_generate_fallback_seed();
	}

	php_random_combinedlcg_seed64(state, seed);
}
/* }}} */
