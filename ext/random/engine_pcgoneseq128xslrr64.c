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
   | Author: Go Kudo <zeriyoshi@php.net>                                  |
   |                                                                      |
   | Based on code from: Melissa O'Neill <oneill@pcg-random.org>          |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_random.h"
#include "php_random_csprng.h"
#include "php_random_uint128.h"

#include "Zend/zend_exceptions.h"

static inline void step(php_random_status_state_pcgoneseq128xslrr64 *s)
{
	s->state = php_random_uint128_add(
		php_random_uint128_multiply(s->state, php_random_uint128_constant(2549297995355413924ULL,4865540595714422341ULL)),
		php_random_uint128_constant(6364136223846793005ULL,1442695040888963407ULL)
	);
}

static inline void seed128(php_random_status_state_pcgoneseq128xslrr64 *s, php_random_uint128_t seed)
{
	s->state = php_random_uint128_constant(0ULL, 0ULL);
	step(s);
	s->state = php_random_uint128_add(s->state, seed);
	step(s);
}

static void seed(php_random_status *status, uint64_t seed)
{
	seed128(status->state, php_random_uint128_constant(0ULL, seed));
}

static php_random_result generate(php_random_status *status)
{
	php_random_status_state_pcgoneseq128xslrr64 *s = status->state;

	step(s);

	return (php_random_result){
		.size = sizeof(uint64_t),
		.result = php_random_pcgoneseq128xslrr64_rotr64(s->state),
	};
}

static zend_long range(php_random_status *status, zend_long min, zend_long max)
{
	return php_random_range(&php_random_algo_pcgoneseq128xslrr64, status, min, max);
}

static bool serialize(php_random_status *status, HashTable *data)
{
	php_random_status_state_pcgoneseq128xslrr64 *s = status->state;
	uint64_t u;
	zval z;

	u = php_random_uint128_hi(s->state);
	ZVAL_STR(&z, php_random_bin2hex_le(&u, sizeof(uint64_t)));
	zend_hash_next_index_insert(data, &z);

	u = php_random_uint128_lo(s->state);
	ZVAL_STR(&z, php_random_bin2hex_le(&u, sizeof(uint64_t)));
	zend_hash_next_index_insert(data, &z);

	return true;
}

static bool unserialize(php_random_status *status, HashTable *data)
{
	php_random_status_state_pcgoneseq128xslrr64 *s = status->state;
	uint64_t u[2];
	zval *t;

	/* Verify the expected number of elements, this implicitly ensures that no additional elements are present. */
	if (zend_hash_num_elements(data) != 2) {
		return false;
	}

	for (uint32_t i = 0; i < 2; i++) {
		t = zend_hash_index_find(data, i);
		if (!t || Z_TYPE_P(t) != IS_STRING || Z_STRLEN_P(t) != (2 * sizeof(uint64_t))) {
			return false;
		}
		if (!php_random_hex2bin_le(Z_STR_P(t), &u[i])) {
			return false;
		}
	}
	s->state = php_random_uint128_constant(u[0], u[1]);

	return true;
}

const php_random_algo php_random_algo_pcgoneseq128xslrr64 = {
	sizeof(php_random_status_state_pcgoneseq128xslrr64),
	seed,
	generate,
	range,
	serialize,
	unserialize
};

/* {{{ php_random_pcgoneseq128xslrr64_advance */
PHPAPI void php_random_pcgoneseq128xslrr64_advance(php_random_status_state_pcgoneseq128xslrr64 *state, uint64_t advance)
{
	php_random_uint128_t
		cur_mult = php_random_uint128_constant(2549297995355413924ULL,4865540595714422341ULL),
		cur_plus = php_random_uint128_constant(6364136223846793005ULL,1442695040888963407ULL),
		acc_mult = php_random_uint128_constant(0ULL, 1ULL),
		acc_plus = php_random_uint128_constant(0ULL, 0ULL);

	while (advance > 0) {
		if (advance & 1) {
			acc_mult = php_random_uint128_multiply(acc_mult, cur_mult);
			acc_plus = php_random_uint128_add(php_random_uint128_multiply(acc_plus, cur_mult), cur_plus);
		}
		cur_plus = php_random_uint128_multiply(php_random_uint128_add(cur_mult, php_random_uint128_constant(0ULL, 1ULL)), cur_plus);
		cur_mult = php_random_uint128_multiply(cur_mult, cur_mult);
		advance /= 2;
	}

	state->state = php_random_uint128_add(php_random_uint128_multiply(acc_mult, state->state), acc_plus);
}
/* }}} */

/* {{{ Random\Engine\PcgOneseq128XslRr64::__construct */
PHP_METHOD(Random_Engine_PcgOneseq128XslRr64, __construct)
{
	php_random_engine *engine = Z_RANDOM_ENGINE_P(ZEND_THIS);
	php_random_status_state_pcgoneseq128xslrr64 *state = engine->status->state;
	zend_string *str_seed = NULL;
	zend_long int_seed = 0;
	bool seed_is_null = true;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL;
		Z_PARAM_STR_OR_LONG_OR_NULL(str_seed, int_seed, seed_is_null);
	ZEND_PARSE_PARAMETERS_END();

	if (seed_is_null) {
		if (php_random_bytes_throw(&state->state, sizeof(state->state)) == FAILURE) {
			zend_throw_exception(random_ce_Random_RandomException, "Failed to generate a random seed", 0);
			RETURN_THROWS();
		}
	} else {
		if (str_seed) {
			/* char (byte: 8 bit) * 16 = 128 bits */
			if (ZSTR_LEN(str_seed) == 16) {
				uint64_t t[2];

				/* Endianness safe copy */
				for (uint32_t i = 0; i < 2; i++) {
					t[i] = 0;
					for (uint32_t j = 0; j < 8; j++) {
						t[i] += ((uint64_t) (unsigned char) ZSTR_VAL(str_seed)[(i * 8) + j]) << (j * 8);
					}
				}

				seed128(state, php_random_uint128_constant(t[0], t[1]));
			} else {
				zend_argument_value_error(1, "must be a 16 byte (128 bit) string");
				RETURN_THROWS();
			}
		} else {
			seed128(state, php_random_uint128_constant(0ULL, (uint64_t) int_seed));
		}
	}
}
/* }}} */

/* {{{ Random\Engine\PcgOneseq128XslRr64::jump() */
PHP_METHOD(Random_Engine_PcgOneseq128XslRr64, jump)
{
	php_random_engine *engine = Z_RANDOM_ENGINE_P(ZEND_THIS);
	php_random_status_state_pcgoneseq128xslrr64 *state = engine->status->state;
	zend_long advance = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(advance);
	ZEND_PARSE_PARAMETERS_END();

	if (UNEXPECTED(advance < 0)) {
		zend_argument_value_error(1, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	php_random_pcgoneseq128xslrr64_advance(state, advance);
}
/* }}} */
