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
   | Based on code from: David Blackman                                   |
   |                     Sebastiano Vigna <vigna@acm.org>                 |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_random.h"
#include "php_random_csprng.h"

#include "Zend/zend_exceptions.h"

static inline uint64_t splitmix64(uint64_t *seed)
{
	uint64_t r;

	r = (*seed += 0x9e3779b97f4a7c15ULL);
	r = (r ^ (r >> 30)) * 0xbf58476d1ce4e5b9ULL;
	r = (r ^ (r >> 27)) * 0x94d049bb133111ebULL;
	return (r ^ (r >> 31));
}

ZEND_ATTRIBUTE_CONST static inline uint64_t rotl(const uint64_t x, int k)
{
	return (x << k) | (x >> (64 - k));
}

static inline uint64_t generate_state(php_random_status_state_xoshiro256starstar *s)
{
	const uint64_t r = rotl(s->state[1] * 5, 7) * 9;
	const uint64_t t = s->state[1] << 17;

	s->state[2] ^= s->state[0];
	s->state[3] ^= s->state[1];
	s->state[1] ^= s->state[2];
	s->state[0] ^= s->state[3];

	s->state[2] ^= t;

	s->state[3] = rotl(s->state[3], 45);

	return r;
}

static inline void jump(php_random_status_state_xoshiro256starstar *state, const uint64_t *jmp)
{
	uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;

	for (uint32_t i = 0; i < 4; i++) {
		for (uint32_t j = 0; j < 64; j++) {
			if (jmp[i] & 1ULL << j) {
				s0 ^= state->state[0];
				s1 ^= state->state[1];
				s2 ^= state->state[2];
				s3 ^= state->state[3];
			}

			generate_state(state);
		}
	}

	state->state[0] = s0;
	state->state[1] = s1;
	state->state[2] = s2;
	state->state[3] = s3;
}

PHPAPI inline void php_random_xoshiro256starstar_seed256(php_random_status_state_xoshiro256starstar *state, uint64_t s0, uint64_t s1, uint64_t s2, uint64_t s3)
{
	state->state[0] = s0;
	state->state[1] = s1;
	state->state[2] = s2;
	state->state[3] = s3;
}

PHPAPI inline void php_random_xoshiro256starstar_seed64(php_random_status_state_xoshiro256starstar *state, uint64_t seed)
{
	uint64_t s[4];

	s[0] = splitmix64(&seed);
	s[1] = splitmix64(&seed);
	s[2] = splitmix64(&seed);
	s[3] = splitmix64(&seed);

	php_random_xoshiro256starstar_seed256(state, s[0], s[1], s[2], s[3]);
}

static php_random_result generate(void *state)
{
	return (php_random_result){
		.size = sizeof(uint64_t),
		.result = generate_state(state),
	};
}

static zend_long range(void *state, zend_long min, zend_long max)
{
	return php_random_range((php_random_algo_with_state){
		.algo = &php_random_algo_xoshiro256starstar,
		.state = state,
	}, min, max);
}

static bool serialize(void *state, HashTable *data)
{
	php_random_status_state_xoshiro256starstar *s = state;
	zval t;

	for (uint32_t i = 0; i < 4; i++) {
		ZVAL_STR(&t, php_random_bin2hex_le(&s->state[i], sizeof(uint64_t)));
		zend_hash_next_index_insert(data, &t);
	}

	return true;
}

static bool unserialize(void *state, HashTable *data)
{
	php_random_status_state_xoshiro256starstar *s = state;
	zval *t;

	/* Verify the expected number of elements, this implicitly ensures that no additional elements are present. */
	if (zend_hash_num_elements(data) != 4) {
		return false;
	}

	for (uint32_t i = 0; i < 4; i++) {
		t = zend_hash_index_find(data, i);
		if (!t || Z_TYPE_P(t) != IS_STRING || Z_STRLEN_P(t) != (2 * sizeof(uint64_t))) {
			return false;
		}
		if (!php_random_hex2bin_le(Z_STR_P(t), &s->state[i])) {
			return false;
		}
	}

	return true;
}

PHPAPI const php_random_algo php_random_algo_xoshiro256starstar = {
	sizeof(php_random_status_state_xoshiro256starstar),
	generate,
	range,
	serialize,
	unserialize
};

PHPAPI void php_random_xoshiro256starstar_jump(php_random_status_state_xoshiro256starstar *state)
{
	static const uint64_t jmp[] = {0x180ec6d33cfd0aba, 0xd5a61266f0c9392c, 0xa9582618e03fc9aa, 0x39abdc4529b1661c};
	jump(state, jmp);
}

PHPAPI void php_random_xoshiro256starstar_jump_long(php_random_status_state_xoshiro256starstar *state)
{
	static const uint64_t jmp[] = {0x76e15d3efefdcbbf, 0xc5004e441c522fb3, 0x77710069854ee241, 0x39109bb02acbe635};
	jump(state, jmp);
}

/* {{{ Random\Engine\Xoshiro256StarStar::jump() */
PHP_METHOD(Random_Engine_Xoshiro256StarStar, jump)
{
	php_random_algo_with_state engine = Z_RANDOM_ENGINE_P(ZEND_THIS)->engine;
	php_random_status_state_xoshiro256starstar *state = engine.state;

	ZEND_PARSE_PARAMETERS_NONE();

	php_random_xoshiro256starstar_jump(state);
}
/* }}} */

/* {{{ Random\Engine\Xoshiro256StarStar::jumpLong() */
PHP_METHOD(Random_Engine_Xoshiro256StarStar, jumpLong)
{
	php_random_algo_with_state engine = Z_RANDOM_ENGINE_P(ZEND_THIS)->engine;
	php_random_status_state_xoshiro256starstar *state = engine.state;

	ZEND_PARSE_PARAMETERS_NONE();

	php_random_xoshiro256starstar_jump_long(state);
}
/* }}} */

/* {{{ Random\Engine\Xoshiro256StarStar::__construct */
PHP_METHOD(Random_Engine_Xoshiro256StarStar, __construct)
{
	php_random_algo_with_state engine = Z_RANDOM_ENGINE_P(ZEND_THIS)->engine;
	php_random_status_state_xoshiro256starstar *state = engine.state;
	zend_string *str_seed = NULL;
	zend_long int_seed = 0;
	bool seed_is_null = true;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL;
		Z_PARAM_STR_OR_LONG_OR_NULL(str_seed, int_seed, seed_is_null);
	ZEND_PARSE_PARAMETERS_END();

	if (seed_is_null) {
		uint64_t t[4];

		do {
			if (php_random_bytes_throw(&t, sizeof(t)) == FAILURE) {
				zend_throw_exception(random_ce_Random_RandomException, "Failed to generate a random seed", 0);
				RETURN_THROWS();
			}
		} while (UNEXPECTED(t[0] == 0 && t[1] == 0 && t[2] == 0 && t[3] == 0));

		php_random_xoshiro256starstar_seed256(state, t[0], t[1], t[2], t[3]);
	} else {
		if (str_seed) {
			/* char (byte: 8 bit) * 32 = 256 bits */
			if (ZSTR_LEN(str_seed) == 32) {
				uint64_t t[4];

				/* Endianness safe copy */
				for (uint32_t i = 0; i < 4; i++) {
					t[i] = 0;
					for (uint32_t j = 0; j < 8; j++) {
						t[i] += ((uint64_t) (unsigned char) ZSTR_VAL(str_seed)[(i * 8) + j]) << (j * 8);
					}
				}

				if (UNEXPECTED(t[0] == 0 && t[1] == 0 && t[2] == 0 && t[3] == 0)) {
					zend_argument_value_error(1, "must not consist entirely of NUL bytes");
					RETURN_THROWS();
				}

				php_random_xoshiro256starstar_seed256(state, t[0], t[1], t[2], t[3]);
			} else {
				zend_argument_value_error(1, "must be a 32 byte (256 bit) string");
				RETURN_THROWS();
			}
		} else {
			php_random_xoshiro256starstar_seed64(state, (uint64_t) int_seed);
		}
	}
}
/* }}} */
