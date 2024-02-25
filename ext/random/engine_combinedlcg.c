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
 * 2^31 - 85 and 2^31 - 249. The period of this function
 * is equal to the product of both primes.
 */
#define MODMULT(a, b, c, m, s) q = s / a; s = b * (s - a * q) - c * q; if (s < 0) s += m

static void seed(void *status, uint64_t seed)
{
	php_random_status_state_combinedlcg *s = status;

	s->state[0] = seed & 0xffffffffU;
	s->state[1] = seed >> 32;
}

static php_random_result generate(void *status)
{
	php_random_status_state_combinedlcg *s = status;
	int32_t q, z;

	MODMULT(53668, 40014, 12211, 2147483563L, s->state[0]);
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

static zend_long range(void *status, zend_long min, zend_long max)
{
	return php_random_range((php_random_algo_with_state){
		.algo = &php_random_algo_combinedlcg,
		.status = status,
	}, min, max);
}

static bool serialize(void *status, HashTable *data)
{
	php_random_status_state_combinedlcg *s = status;
	zval t;

	for (uint32_t i = 0; i < 2; i++) {
		ZVAL_STR(&t, php_random_bin2hex_le(&s->state[i], sizeof(uint32_t)));
		zend_hash_next_index_insert(data, &t);
	}

	return true;
}

static bool unserialize(void *status, HashTable *data)
{
	php_random_status_state_combinedlcg *s = status;
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

const php_random_algo php_random_algo_combinedlcg = {
	sizeof(php_random_status_state_combinedlcg),
	seed,
	generate,
	range,
	serialize,
	unserialize
};

/* {{{ php_random_combinedlcg_seed_default */
PHPAPI void php_random_combinedlcg_seed_default(php_random_status_state_combinedlcg *state)
{
	struct timeval tv;

	if (gettimeofday(&tv, NULL) == 0) {
		state->state[0] = tv.tv_usec ^ (tv.tv_usec << 11);
	} else {
		state->state[0] = 1;
	}

#ifdef ZTS
	state->state[1] = (zend_long) tsrm_thread_id();
#else
	state->state[1] = (zend_long) getpid();
#endif

	/* Add entropy to s2 by calling gettimeofday() again */
	if (gettimeofday(&tv, NULL) == 0) {
		state->state[1] ^= (tv.tv_usec << 11);
	}
}
/* }}} */
