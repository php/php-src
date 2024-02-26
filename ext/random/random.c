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
   | Authors: Sammy Kaye Powers <me@sammyk.me>                            |
   |          Go Kudo <zeriyoshi@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

#include "php.h"

#include "Zend/zend_enum.h"
#include "Zend/zend_exceptions.h"

#include "php_random.h"
#include "php_random_csprng.h"

#if HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef PHP_WIN32
# include "win32/time.h"
# include "win32/winutil.h"
# include <process.h>
#else
# include <sys/time.h>
#endif

#if HAVE_SYS_PARAM_H
# include <sys/param.h>
#endif

#include "random_arginfo.h"

PHPAPI ZEND_DECLARE_MODULE_GLOBALS(random)

PHPAPI zend_class_entry *random_ce_Random_Engine;
PHPAPI zend_class_entry *random_ce_Random_CryptoSafeEngine;

PHPAPI zend_class_entry *random_ce_Random_Engine_Mt19937;
PHPAPI zend_class_entry *random_ce_Random_Engine_PcgOneseq128XslRr64;
PHPAPI zend_class_entry *random_ce_Random_Engine_Xoshiro256StarStar;
PHPAPI zend_class_entry *random_ce_Random_Engine_Secure;

PHPAPI zend_class_entry *random_ce_Random_Randomizer;

PHPAPI zend_class_entry *random_ce_Random_IntervalBoundary;

PHPAPI zend_class_entry *random_ce_Random_RandomError;
PHPAPI zend_class_entry *random_ce_Random_BrokenRandomEngineError;
PHPAPI zend_class_entry *random_ce_Random_RandomException;

static zend_object_handlers random_engine_mt19937_object_handlers;
static zend_object_handlers random_engine_pcgoneseq128xslrr64_object_handlers;
static zend_object_handlers random_engine_xoshiro256starstar_object_handlers;
static zend_object_handlers random_engine_secure_object_handlers;
static zend_object_handlers random_randomizer_object_handlers;

PHPAPI uint32_t php_random_range32(php_random_algo_with_state engine, uint32_t umax)
{
	const php_random_algo *algo = engine.algo;
	void *state = engine.status;

	uint32_t result, limit;
	size_t total_size = 0;
	uint32_t count = 0;

	result = 0;
	total_size = 0;
	do {
		php_random_result r = algo->generate(state);
		result = result | (((uint32_t) r.result) << (total_size * 8));
		total_size += r.size;
		if (EG(exception)) {
			return 0;
		}
	} while (total_size < sizeof(uint32_t));

	/* Special case where no modulus is required */
	if (UNEXPECTED(umax == UINT32_MAX)) {
		return result;
	}

	/* Increment the max so range is inclusive of max */
	umax++;

	/* Powers of two are not biased */
	if ((umax & (umax - 1)) == 0) {
		return result & (umax - 1);
	}

	/* Ceiling under which UINT32_MAX % max == 0 */
	limit = UINT32_MAX - (UINT32_MAX % umax) - 1;

	/* Discard numbers over the limit to avoid modulo bias */
	while (UNEXPECTED(result > limit)) {
		/* If the requirements cannot be met in a cycles, return fail */
		if (++count > PHP_RANDOM_RANGE_ATTEMPTS) {
			zend_throw_error(random_ce_Random_BrokenRandomEngineError, "Failed to generate an acceptable random number in %d attempts", PHP_RANDOM_RANGE_ATTEMPTS);
			return 0;
		}

		result = 0;
		total_size = 0;
		do {
			php_random_result r = algo->generate(state);
			result = result | (((uint32_t) r.result) << (total_size * 8));
			total_size += r.size;
			if (EG(exception)) {
				return 0;
			}
		} while (total_size < sizeof(uint32_t));
	}

	return result % umax;
}

PHPAPI uint64_t php_random_range64(php_random_algo_with_state engine, uint64_t umax)
{
	const php_random_algo *algo = engine.algo;
	void *state = engine.status;

	uint64_t result, limit;
	size_t total_size = 0;
	uint32_t count = 0;

	result = 0;
	total_size = 0;
	do {
		php_random_result r = algo->generate(state);
		result = result | (r.result << (total_size * 8));
		total_size += r.size;
		if (EG(exception)) {
			return 0;
		}
	} while (total_size < sizeof(uint64_t));

	/* Special case where no modulus is required */
	if (UNEXPECTED(umax == UINT64_MAX)) {
		return result;
	}

	/* Increment the max so range is inclusive of max */
	umax++;

	/* Powers of two are not biased */
	if ((umax & (umax - 1)) == 0) {
		return result & (umax - 1);
	}

	/* Ceiling under which UINT64_MAX % max == 0 */
	limit = UINT64_MAX - (UINT64_MAX % umax) - 1;

	/* Discard numbers over the limit to avoid modulo bias */
	while (UNEXPECTED(result > limit)) {
		/* If the requirements cannot be met in a cycles, return fail */
		if (++count > PHP_RANDOM_RANGE_ATTEMPTS) {
			zend_throw_error(random_ce_Random_BrokenRandomEngineError, "Failed to generate an acceptable random number in %d attempts", PHP_RANDOM_RANGE_ATTEMPTS);
			return 0;
		}

		result = 0;
		total_size = 0;
		do {
			php_random_result r = algo->generate(state);
			result = result | (r.result << (total_size * 8));
			total_size += r.size;
			if (EG(exception)) {
				return 0;
			}
		} while (total_size < sizeof(uint64_t));
	}

	return result % umax;
}

static zend_object *php_random_engine_mt19937_new(zend_class_entry *ce)
{
	return &php_random_engine_common_init(ce, &random_engine_mt19937_object_handlers, &php_random_algo_mt19937)->std;
}

static zend_object *php_random_engine_pcgoneseq128xslrr64_new(zend_class_entry *ce)
{
	return &php_random_engine_common_init(ce, &random_engine_pcgoneseq128xslrr64_object_handlers, &php_random_algo_pcgoneseq128xslrr64)->std;
}

static zend_object *php_random_engine_xoshiro256starstar_new(zend_class_entry *ce)
{
	return &php_random_engine_common_init(ce, &random_engine_xoshiro256starstar_object_handlers, &php_random_algo_xoshiro256starstar)->std;
}

static zend_object *php_random_engine_secure_new(zend_class_entry *ce)
{
	return &php_random_engine_common_init(ce, &random_engine_secure_object_handlers, &php_random_algo_secure)->std;
}

static zend_object *php_random_randomizer_new(zend_class_entry *ce)
{
	php_random_randomizer *randomizer = zend_object_alloc(sizeof(php_random_randomizer), ce);

	zend_object_std_init(&randomizer->std, ce);
	object_properties_init(&randomizer->std, ce);

	return &randomizer->std;
}

static void randomizer_free_obj(zend_object *object) {
	php_random_randomizer *randomizer = php_random_randomizer_from_obj(object);

	if (randomizer->is_userland_algo) {
		php_random_status_free(randomizer->engine.status, false);
	}

	zend_object_std_dtor(&randomizer->std);
}

PHPAPI void *php_random_status_alloc(const php_random_algo *algo, const bool persistent)
{
	return algo->state_size > 0 ? pecalloc(1, algo->state_size, persistent) : NULL;
}

PHPAPI void *php_random_status_copy(const php_random_algo *algo, void *old_status, void *new_status)
{
	return memcpy(new_status, old_status, algo->state_size);
}

PHPAPI void php_random_status_free(void *status, const bool persistent)
{
	pefree(status, persistent);
}

PHPAPI php_random_engine *php_random_engine_common_init(zend_class_entry *ce, zend_object_handlers *handlers, const php_random_algo *algo)
{
	php_random_engine *engine = zend_object_alloc(sizeof(php_random_engine), ce);

	zend_object_std_init(&engine->std, ce);
	object_properties_init(&engine->std, ce);

	engine->engine = (php_random_algo_with_state){
		.algo = algo,
		.status = php_random_status_alloc(algo, false)
	};
	engine->std.handlers = handlers;

	return engine;
}

PHPAPI void php_random_engine_common_free_object(zend_object *object)
{
	php_random_engine *engine = php_random_engine_from_obj(object);

	php_random_status_free(engine->engine.status, false);
	zend_object_std_dtor(object);
}

PHPAPI zend_object *php_random_engine_common_clone_object(zend_object *object)
{
	php_random_engine *old_engine = php_random_engine_from_obj(object);
	php_random_engine *new_engine = php_random_engine_from_obj(old_engine->std.ce->create_object(old_engine->std.ce));

	new_engine->engine.algo = old_engine->engine.algo;
	if (old_engine->engine.status) {
		new_engine->engine.status = php_random_status_copy(old_engine->engine.algo, old_engine->engine.status, new_engine->engine.status);
	}

	zend_objects_clone_members(&new_engine->std, &old_engine->std);

	return &new_engine->std;
}

/* {{{ php_random_range */
PHPAPI zend_long php_random_range(php_random_algo_with_state engine, zend_long min, zend_long max)
{
	zend_ulong umax = (zend_ulong) max - (zend_ulong) min;

	if (umax > UINT32_MAX) {
		return (zend_long) (php_random_range64(engine, umax) + min);
	}

	return (zend_long) (php_random_range32(engine, umax) + min);
}
/* }}} */

/* {{{ php_random_default_algo */
PHPAPI const php_random_algo *php_random_default_algo(void)
{
	return &php_random_algo_mt19937;
}
/* }}} */

/* {{{ php_random_default_status */
PHPAPI void *php_random_default_status(void)
{
	php_random_status_state_mt19937 *state = RANDOM_G(mt19937);

	if (!RANDOM_G(mt19937_seeded)) {
		php_random_mt19937_seed_default(state);
		RANDOM_G(mt19937_seeded) = true;
	}

	return state;
}
/* }}} */

/* this is read-only, so it's ok */
ZEND_SET_ALIGNED(16, static const char hexconvtab[]) = "0123456789abcdef";

/* {{{ php_random_bin2hex_le */
/* stolen from standard/string.c */
PHPAPI zend_string *php_random_bin2hex_le(const void *ptr, const size_t len)
{
	zend_string *str;
	size_t i;

	str = zend_string_safe_alloc(len, 2 * sizeof(char), 0, 0);

	i = 0;
#ifdef WORDS_BIGENDIAN
	/* force little endian */
	for (zend_long j = (len - 1); 0 <= j; j--) {
#else
	for (zend_long j = 0; j < len; j++) {
#endif
		ZSTR_VAL(str)[i++] = hexconvtab[((unsigned char *) ptr)[j] >> 4];
		ZSTR_VAL(str)[i++] = hexconvtab[((unsigned char *) ptr)[j] & 15];
	}
	ZSTR_VAL(str)[i] = '\0';

	return str;
}
/* }}} */

/* {{{ php_random_hex2bin_le */
/* stolen from standard/string.c */
PHPAPI bool php_random_hex2bin_le(zend_string *hexstr, void *dest)
{
	size_t len = hexstr->len >> 1;
	unsigned char *str = (unsigned char *) hexstr->val, c, l, d;
	unsigned char *ptr = (unsigned char *) dest;
	int is_letter, i = 0;

#ifdef WORDS_BIGENDIAN
	/* force little endian */
	for (zend_long j = (len - 1); 0 <= j; j--) {
#else
	for (zend_long j = 0; j < len; j++) {
#endif
		c = str[i++];
		l = c & ~0x20;
		is_letter = ((uint32_t) ((l - 'A') ^ (l - 'F' - 1))) >> (8 * sizeof(uint32_t) - 1);

		/* basically (c >= '0' && c <= '9') || (l >= 'A' && l <= 'F') */
		if (EXPECTED((((c ^ '0') - 10) >> (8 * sizeof(uint32_t) - 1)) | is_letter)) {
			d = (l - 0x10 - 0x27 * is_letter) << 4;
		} else {
			return false;
		}
		c = str[i++];
		l = c & ~0x20;
		is_letter = ((uint32_t) ((l - 'A') ^ (l - 'F' - 1))) >> (8 * sizeof(uint32_t) - 1);
		if (EXPECTED((((c ^ '0') - 10) >> (8 * sizeof(uint32_t) - 1)) | is_letter)) {
			d |= l - 0x10 - 0x27 * is_letter;
		} else {
			return false;
		}
		ptr[j] = d;
	}
	return true;
}
/* }}} */

/* {{{ php_combined_lcg */
PHPAPI double php_combined_lcg(void)
{
	php_random_status_state_combinedlcg *state = RANDOM_G(combined_lcg);

	if (!RANDOM_G(combined_lcg_seeded)) {
		php_random_combinedlcg_seed_default(state);
		RANDOM_G(combined_lcg_seeded) = true;
	}

	return php_random_algo_combinedlcg.generate(state).result * 4.656613e-10;
}
/* }}} */

/* {{{ php_mt_srand */
PHPAPI void php_mt_srand(uint32_t seed)
{
	/* Seed the generator with a simple uint32 */
	php_random_algo_mt19937.seed(php_random_default_status(), (zend_long) seed);
}
/* }}} */

/* {{{ php_mt_rand */
PHPAPI uint32_t php_mt_rand(void)
{
	return (uint32_t) php_random_algo_mt19937.generate(php_random_default_status()).result;
}
/* }}} */

/* {{{ php_mt_rand_range */
PHPAPI zend_long php_mt_rand_range(zend_long min, zend_long max)
{
	return php_random_algo_mt19937.range(php_random_default_status(), min, max);
}
/* }}} */

/* {{{ php_mt_rand_common
 * rand() allows min > max, mt_rand does not */
PHPAPI zend_long php_mt_rand_common(zend_long min, zend_long max)
{
	php_random_status_state_mt19937 *s = php_random_default_status();

	if (s->mode == MT_RAND_MT19937) {
		return php_mt_rand_range(min, max);
	}

	uint64_t r = php_random_algo_mt19937.generate(php_random_default_status()).result >> 1;

	/* This is an inlined version of the RAND_RANGE_BADSCALING macro that does not invoke UB when encountering
	 * (max - min) > ZEND_LONG_MAX.
	 */
	zend_ulong offset = (double) ( (double) max - min + 1.0) * (r / (PHP_MT_RAND_MAX + 1.0));

	return (zend_long) (offset + min);
}
/* }}} */

/* {{{ php_srand */
PHPAPI void php_srand(zend_long seed)
{
	php_mt_srand((uint32_t) seed);
}
/* }}} */

/* {{{ php_rand */
PHPAPI zend_long php_rand(void)
{
	return php_mt_rand();
}
/* }}} */

/* {{{ Returns a value from the combined linear congruential generator */
PHP_FUNCTION(lcg_value)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_DOUBLE(php_combined_lcg());
}
/* }}} */

/* {{{ Seeds Mersenne Twister random number generator */
PHP_FUNCTION(mt_srand)
{
	zend_long seed = 0;
	bool seed_is_null = true;
	zend_long mode = MT_RAND_MT19937;
	php_random_status_state_mt19937 *state = RANDOM_G(mt19937);

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(seed, seed_is_null)
		Z_PARAM_LONG(mode)
	ZEND_PARSE_PARAMETERS_END();

	state->mode = mode;

	/* Anything that is not MT_RAND_MT19937 was interpreted as MT_RAND_PHP. */
	if (state->mode != MT_RAND_MT19937) {
		zend_error(E_DEPRECATED, "The MT_RAND_PHP variant of Mt19937 is deprecated");
	}

	if (seed_is_null) {
		php_random_mt19937_seed_default(state);
	} else {
		php_random_algo_mt19937.seed(state, (uint64_t) seed);
	}
	RANDOM_G(mt19937_seeded) = true;
}
/* }}} */

/* {{{ Returns a random number from Mersenne Twister */
PHP_FUNCTION(mt_rand)
{
	zend_long min, max;
	int argc = ZEND_NUM_ARGS();

	if (argc == 0) {
		/* genrand_int31 in mt19937ar.c performs a right shift */
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

/* {{{ Returns a random number from Mersenne Twister */
PHP_FUNCTION(rand)
{
	zend_long min, max;
	int argc = ZEND_NUM_ARGS();

	if (argc == 0) {
		/* genrand_int31 in mt19937ar.c performs a right shift */
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
	zend_long min, max, result;

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

/* {{{ PHP_GINIT_FUNCTION */
static PHP_GINIT_FUNCTION(random)
{
	random_globals->random_fd = -1;

	random_globals->combined_lcg = php_random_status_alloc(&php_random_algo_combinedlcg, true);
	random_globals->combined_lcg_seeded = false;

	random_globals->mt19937 = php_random_status_alloc(&php_random_algo_mt19937, true);
	random_globals->mt19937_seeded = false;
}
/* }}} */

/* {{{ PHP_GSHUTDOWN_FUNCTION */
static PHP_GSHUTDOWN_FUNCTION(random)
{
	if (random_globals->random_fd >= 0) {
		close(random_globals->random_fd);
		random_globals->random_fd = -1;
	}

	php_random_status_free(random_globals->combined_lcg, true);
	random_globals->combined_lcg = NULL;

	php_random_status_free(random_globals->mt19937, true);
	random_globals->mt19937 = NULL;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(random)
{
	/* Random\Engine */
	random_ce_Random_Engine = register_class_Random_Engine();

	/* Random\CryptoSafeEngine */
	random_ce_Random_CryptoSafeEngine = register_class_Random_CryptoSafeEngine(random_ce_Random_Engine);

	/* Random\RandomError */
	random_ce_Random_RandomError = register_class_Random_RandomError(zend_ce_error);

	/* Random\BrokenRandomEngineError */
	random_ce_Random_BrokenRandomEngineError = register_class_Random_BrokenRandomEngineError(random_ce_Random_RandomError);

	/* Random\RandomException */
	random_ce_Random_RandomException = register_class_Random_RandomException(zend_ce_exception);

	/* Random\Engine\Mt19937 */
	random_ce_Random_Engine_Mt19937 = register_class_Random_Engine_Mt19937(random_ce_Random_Engine);
	random_ce_Random_Engine_Mt19937->create_object = php_random_engine_mt19937_new;
	memcpy(&random_engine_mt19937_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	random_engine_mt19937_object_handlers.offset = XtOffsetOf(php_random_engine, std);
	random_engine_mt19937_object_handlers.free_obj = php_random_engine_common_free_object;
	random_engine_mt19937_object_handlers.clone_obj = php_random_engine_common_clone_object;

	/* Random\Engine\PcgOnseq128XslRr64 */
	random_ce_Random_Engine_PcgOneseq128XslRr64 = register_class_Random_Engine_PcgOneseq128XslRr64(random_ce_Random_Engine);
	random_ce_Random_Engine_PcgOneseq128XslRr64->create_object = php_random_engine_pcgoneseq128xslrr64_new;
	memcpy(&random_engine_pcgoneseq128xslrr64_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	random_engine_pcgoneseq128xslrr64_object_handlers.offset = XtOffsetOf(php_random_engine, std);
	random_engine_pcgoneseq128xslrr64_object_handlers.free_obj = php_random_engine_common_free_object;
	random_engine_pcgoneseq128xslrr64_object_handlers.clone_obj = php_random_engine_common_clone_object;

	/* Random\Engine\Xoshiro256StarStar */
	random_ce_Random_Engine_Xoshiro256StarStar = register_class_Random_Engine_Xoshiro256StarStar(random_ce_Random_Engine);
	random_ce_Random_Engine_Xoshiro256StarStar->create_object = php_random_engine_xoshiro256starstar_new;
	memcpy(&random_engine_xoshiro256starstar_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	random_engine_xoshiro256starstar_object_handlers.offset = XtOffsetOf(php_random_engine, std);
	random_engine_xoshiro256starstar_object_handlers.free_obj = php_random_engine_common_free_object;
	random_engine_xoshiro256starstar_object_handlers.clone_obj = php_random_engine_common_clone_object;

	/* Random\Engine\Secure */
	random_ce_Random_Engine_Secure = register_class_Random_Engine_Secure(random_ce_Random_CryptoSafeEngine);
	random_ce_Random_Engine_Secure->create_object = php_random_engine_secure_new;
	memcpy(&random_engine_secure_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	random_engine_secure_object_handlers.offset = XtOffsetOf(php_random_engine, std);
	random_engine_secure_object_handlers.free_obj = php_random_engine_common_free_object;
	random_engine_secure_object_handlers.clone_obj = NULL;

	/* Random\Randomizer */
	random_ce_Random_Randomizer = register_class_Random_Randomizer();
	random_ce_Random_Randomizer->create_object = php_random_randomizer_new;
	random_ce_Random_Randomizer->default_object_handlers = &random_randomizer_object_handlers;
	memcpy(&random_randomizer_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	random_randomizer_object_handlers.offset = XtOffsetOf(php_random_randomizer, std);
	random_randomizer_object_handlers.free_obj = randomizer_free_obj;
	random_randomizer_object_handlers.clone_obj = NULL;

	/* Random\IntervalBoundary */
	random_ce_Random_IntervalBoundary = register_class_Random_IntervalBoundary();

	register_random_symbols(module_number);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(random)
{
	RANDOM_G(combined_lcg_seeded) = false;
	RANDOM_G(mt19937_seeded) = false;

	return SUCCESS;
}
/* }}} */

/* {{{ random_module_entry */
zend_module_entry random_module_entry = {
	STANDARD_MODULE_HEADER,
	"random",					/* Extension name */
	ext_functions,				/* zend_function_entry */
	PHP_MINIT(random),			/* PHP_MINIT - Module initialization */
	NULL,						/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(random),			/* PHP_RINIT - Request initialization */
	NULL,						/* PHP_RSHUTDOWN - Request shutdown */
	NULL,						/* PHP_MINFO - Module info */
	PHP_VERSION,				/* Version */
	PHP_MODULE_GLOBALS(random),	/* ZTS Module globals */
	PHP_GINIT(random),			/* PHP_GINIT - Global initialization */
	PHP_GSHUTDOWN(random),		/* PHP_GSHUTDOWN - Global shutdown */
	NULL,						/* Post deactivate */
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */
