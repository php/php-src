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
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_random.h"

#include "ext/standard/php_array.h"
#include "ext/standard/php_string.h"

#include "Zend/zend_enum.h"
#include "Zend/zend_exceptions.h"

static inline void randomizer_common_init(php_random_randomizer *randomizer, zend_object *engine_object) {
	if (engine_object->ce->type == ZEND_INTERNAL_CLASS) {
		/* Internal classes always php_random_engine struct */
		php_random_engine *engine = php_random_engine_from_obj(engine_object);

		/* Copy engine pointers */
		randomizer->algo = engine->algo;
		randomizer->status = engine->status;
	} else {
		/* Self allocation */
		randomizer->status = php_random_status_alloc(&php_random_algo_user, false);
		php_random_status_state_user *state = randomizer->status->state;
		zend_string *mname;
		zend_function *generate_method;

		mname = ZSTR_INIT_LITERAL("generate", 0);
		generate_method = zend_hash_find_ptr(&engine_object->ce->function_table, mname);
		zend_string_release(mname);

		/* Create compatible state */
		state->object = engine_object;
		state->generate_method = generate_method;

		/* Copy common pointers */
		randomizer->algo = &php_random_algo_user;

		/* Mark self-allocated for memory management */
		randomizer->is_userland_algo = true;
	}
}

/* {{{ Random\Randomizer::__construct() */
PHP_METHOD(Random_Randomizer, __construct)
{
	php_random_randomizer *randomizer = Z_RANDOM_RANDOMIZER_P(ZEND_THIS);
	zval engine;
	zval *param_engine = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS_OR_NULL(param_engine, random_ce_Random_Engine);
	ZEND_PARSE_PARAMETERS_END();

	if (param_engine != NULL) {
		ZVAL_COPY(&engine, param_engine);
	} else {
		/* Create default RNG instance */
		object_init_ex(&engine, random_ce_Random_Engine_Secure);
	}

	zend_update_property(random_ce_Random_Randomizer, Z_OBJ_P(ZEND_THIS), "engine", strlen("engine"), &engine);

	OBJ_RELEASE(Z_OBJ_P(&engine));

	if (EG(exception)) {
		RETURN_THROWS();
	}

	randomizer_common_init(randomizer, Z_OBJ_P(&engine));
}
/* }}} */

/* {{{ Generate a float in [0, 1) */
PHP_METHOD(Random_Randomizer, nextFloat)
{
	php_random_randomizer *randomizer = Z_RANDOM_RANDOMIZER_P(ZEND_THIS);
	uint64_t result;
	size_t total_size;

	ZEND_PARSE_PARAMETERS_NONE();

	result = 0;
	total_size = 0;
	do {
		php_random_result r = randomizer->algo->generate(randomizer->status);
		result = result | (r.result << (total_size * 8));
		total_size += r.size;
		if (EG(exception)) {
			RETURN_THROWS();
		}
	} while (total_size < sizeof(uint64_t));

	/* A double has 53 bits of precision, thus we must not
	 * use the full 64 bits of the uint64_t, because we would
	 * introduce a bias / rounding error.
	 */
#if DBL_MANT_DIG != 53
# error "Random_Randomizer::nextFloat(): Requires DBL_MANT_DIG == 53 to work."
#endif
	const double step_size = 1.0 / (1ULL << 53);

	/* Use the upper 53 bits, because some engine's lower bits
	 * are of lower quality.
	 */
	result = (result >> 11);

	RETURN_DOUBLE(step_size * result);
}
/* }}} */

/* {{{ Generates a random float within a configurable interval.
 *
 * This method uses the γ-section algorithm by Frédéric Goualard.
 */
PHP_METHOD(Random_Randomizer, getFloat)
{
	php_random_randomizer *randomizer = Z_RANDOM_RANDOMIZER_P(ZEND_THIS);
	double min, max;
	zend_object *bounds = NULL;
	int bounds_type = 'C' + sizeof("ClosedOpen") - 1;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_DOUBLE(min)
		Z_PARAM_DOUBLE(max)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJ_OF_CLASS(bounds, random_ce_Random_IntervalBoundary);
	ZEND_PARSE_PARAMETERS_END();

	if (!zend_finite(min)) {
		zend_argument_value_error(1, "must be finite");
		RETURN_THROWS();
	}

	if (!zend_finite(max)) {
		zend_argument_value_error(2, "must be finite");
		RETURN_THROWS();
	}

	if (bounds) {
		zval *case_name = zend_enum_fetch_case_name(bounds);
		zend_string *bounds_name = Z_STR_P(case_name);

		bounds_type = ZSTR_VAL(bounds_name)[0] + ZSTR_LEN(bounds_name);
	}

	switch (bounds_type) {
	case 'C' + sizeof("ClosedOpen") - 1:
		if (UNEXPECTED(max <= min)) {
			zend_argument_value_error(2, "must be greater than argument #1 ($min)");
			RETURN_THROWS();
		}

		RETURN_DOUBLE(php_random_gammasection_closed_open(randomizer->algo, randomizer->status, min, max));
	case 'C' + sizeof("ClosedClosed") - 1:
		if (UNEXPECTED(max < min)) {
			zend_argument_value_error(2, "must be greater than or equal to argument #1 ($min)");
			RETURN_THROWS();
		}

		RETURN_DOUBLE(php_random_gammasection_closed_closed(randomizer->algo, randomizer->status, min, max));
	case 'O' + sizeof("OpenClosed") - 1:
		if (UNEXPECTED(max <= min)) {
			zend_argument_value_error(2, "must be greater than argument #1 ($min)");
			RETURN_THROWS();
		}

		RETURN_DOUBLE(php_random_gammasection_open_closed(randomizer->algo, randomizer->status, min, max));
	case 'O' + sizeof("OpenOpen") - 1:
		if (UNEXPECTED(max <= min)) {
			zend_argument_value_error(2, "must be greater than argument #1 ($min)");
			RETURN_THROWS();
		}

		RETVAL_DOUBLE(php_random_gammasection_open_open(randomizer->algo, randomizer->status, min, max));

		if (UNEXPECTED(isnan(Z_DVAL_P(return_value)))) {
			zend_value_error("The given interval is empty, there are no floats between argument #1 ($min) and argument #2 ($max).");
			RETURN_THROWS();
		}

		return;
	default:
		ZEND_UNREACHABLE();
	}
}
/* }}} */

/* {{{ Generate positive random number */
PHP_METHOD(Random_Randomizer, nextInt)
{
	php_random_randomizer *randomizer = Z_RANDOM_RANDOMIZER_P(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_NONE();

	php_random_result result = randomizer->algo->generate(randomizer->status);
	if (EG(exception)) {
		RETURN_THROWS();
	}
	if (result.size > sizeof(zend_long)) {
		zend_throw_exception(random_ce_Random_RandomException, "Generated value exceeds size of int", 0);
		RETURN_THROWS();
	}

	RETURN_LONG((zend_long) (result.result >> 1));
}
/* }}} */

/* {{{ Generate random number in range */
PHP_METHOD(Random_Randomizer, getInt)
{
	php_random_randomizer *randomizer = Z_RANDOM_RANDOMIZER_P(ZEND_THIS);
	uint64_t result;
	zend_long min, max;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(min)
		Z_PARAM_LONG(max)
	ZEND_PARSE_PARAMETERS_END();

	if (UNEXPECTED(max < min)) {
		zend_argument_value_error(2, "must be greater than or equal to argument #1 ($min)");
		RETURN_THROWS();
	}

	if (UNEXPECTED(
		randomizer->algo->range == php_random_algo_mt19937.range
		&& ((php_random_status_state_mt19937 *) randomizer->status->state)->mode != MT_RAND_MT19937
	)) {
		uint64_t r = php_random_algo_mt19937.generate(randomizer->status).result >> 1;

		/* This is an inlined version of the RAND_RANGE_BADSCALING macro that does not invoke UB when encountering
		 * (max - min) > ZEND_LONG_MAX.
		 */
		zend_ulong offset = (double) ( (double) max - min + 1.0) * (r / (PHP_MT_RAND_MAX + 1.0));

		result = (zend_long) (offset + min);
	} else {
		result = randomizer->algo->range(randomizer->status, min, max);
	}

	if (EG(exception)) {
		RETURN_THROWS();
	}

	RETURN_LONG((zend_long) result);
}
/* }}} */

/* {{{ Generate random bytes string in ordered length */
PHP_METHOD(Random_Randomizer, getBytes)
{
	php_random_randomizer *randomizer = Z_RANDOM_RANDOMIZER_P(ZEND_THIS);
	zend_string *retval;
	zend_long length;
	size_t total_size = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(length)
	ZEND_PARSE_PARAMETERS_END();

	if (length < 1) {
		zend_argument_value_error(1, "must be greater than 0");
		RETURN_THROWS();
	}

	retval = zend_string_alloc(length, 0);

	while (total_size < length) {
		php_random_result result = randomizer->algo->generate(randomizer->status);
		if (EG(exception)) {
			zend_string_free(retval);
			RETURN_THROWS();
		}
		for (size_t i = 0; i < result.size; i++) {
			ZSTR_VAL(retval)[total_size++] = (result.result >> (i * 8)) & 0xff;
			if (total_size >= length) {
				break;
			}
		}
	}

	ZSTR_VAL(retval)[length] = '\0';
	RETURN_STR(retval);
}
/* }}} */

/* {{{ Shuffling array */
PHP_METHOD(Random_Randomizer, shuffleArray)
{
	php_random_randomizer *randomizer = Z_RANDOM_RANDOMIZER_P(ZEND_THIS);
	zval *array;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY(array)
	ZEND_PARSE_PARAMETERS_END();

	ZVAL_DUP(return_value, array);
	if (!php_array_data_shuffle(randomizer->algo, randomizer->status, return_value)) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ Shuffling binary */
PHP_METHOD(Random_Randomizer, shuffleBytes)
{
	php_random_randomizer *randomizer = Z_RANDOM_RANDOMIZER_P(ZEND_THIS);
	zend_string *bytes;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(bytes)
	ZEND_PARSE_PARAMETERS_END();

	if (ZSTR_LEN(bytes) < 2) {
		RETURN_STR_COPY(bytes);
	}

	RETVAL_STRINGL(ZSTR_VAL(bytes), ZSTR_LEN(bytes));
	if (!php_binary_string_shuffle(randomizer->algo, randomizer->status, Z_STRVAL_P(return_value), (zend_long) Z_STRLEN_P(return_value))) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ Pick keys */
PHP_METHOD(Random_Randomizer, pickArrayKeys)
{
	php_random_randomizer *randomizer = Z_RANDOM_RANDOMIZER_P(ZEND_THIS);
	zval *input, t;
	zend_long num_req;

	ZEND_PARSE_PARAMETERS_START(2, 2);
		Z_PARAM_ARRAY(input)
		Z_PARAM_LONG(num_req)
	ZEND_PARSE_PARAMETERS_END();

	if (!php_array_pick_keys(
		randomizer->algo,
		randomizer->status,
		input,
		num_req,
		return_value,
		false)
	) {
		RETURN_THROWS();
	}

	/* Keep compatibility, But the result is always an array */
	if (Z_TYPE_P(return_value) != IS_ARRAY) {
		ZVAL_COPY_VALUE(&t, return_value);
		array_init(return_value);
		zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &t);
	}
}
/* }}} */

/* {{{ Get Random Bytes for String */
PHP_METHOD(Random_Randomizer, getBytesFromString)
{
	php_random_randomizer *randomizer = Z_RANDOM_RANDOMIZER_P(ZEND_THIS);
	zend_long length;
	zend_string *source, *retval;
	size_t total_size = 0;

	ZEND_PARSE_PARAMETERS_START(2, 2);
		Z_PARAM_STR(source)
		Z_PARAM_LONG(length)
	ZEND_PARSE_PARAMETERS_END();

	const size_t source_length = ZSTR_LEN(source);
	const size_t max_offset = source_length - 1;

	if (source_length < 1) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	if (length < 1) {
		zend_argument_value_error(2, "must be greater than 0");
		RETURN_THROWS();
	}

	retval = zend_string_alloc(length, 0);

	if (max_offset > 0xff) {
		while (total_size < length) {
			uint64_t offset = randomizer->algo->range(randomizer->status, 0, max_offset);

			if (EG(exception)) {
				zend_string_free(retval);
				RETURN_THROWS();
			}

			ZSTR_VAL(retval)[total_size++] = ZSTR_VAL(source)[offset];
		}
	} else {
		uint64_t mask = max_offset;
		// Copy the top-most bit into all lower bits.
		// Shifting by 4 is sufficient, because max_offset
		// is guaranteed to fit in an 8-bit integer at this
		// point.
		mask |= mask >> 1;
		mask |= mask >> 2;
		mask |= mask >> 4;

		int failures = 0;
		while (total_size < length) {
			php_random_result result = randomizer->algo->generate(randomizer->status);
			if (EG(exception)) {
				zend_string_free(retval);
				RETURN_THROWS();
			}

			for (size_t i = 0; i < result.size; i++) {
				uint64_t offset = (result.result >> (i * 8)) & mask;

				if (offset > max_offset) {
					if (++failures > PHP_RANDOM_RANGE_ATTEMPTS) {
						zend_string_free(retval);
						zend_throw_error(random_ce_Random_BrokenRandomEngineError, "Failed to generate an acceptable random number in %d attempts", PHP_RANDOM_RANGE_ATTEMPTS);
						RETURN_THROWS();
					}

					continue;
				}

				failures = 0;

				ZSTR_VAL(retval)[total_size++] = ZSTR_VAL(source)[offset];
				if (total_size >= length) {
					break;
				}
			}
		}
	}

	ZSTR_VAL(retval)[length] = '\0';
	RETURN_STR(retval);
}
/* }}} */

/* {{{ Random\Randomizer::__serialize() */
PHP_METHOD(Random_Randomizer, __serialize)
{
	php_random_randomizer *randomizer = Z_RANDOM_RANDOMIZER_P(ZEND_THIS);
	zval t;

	ZEND_PARSE_PARAMETERS_NONE();

	array_init(return_value);
	ZVAL_ARR(&t, zend_std_get_properties(&randomizer->std));
	Z_TRY_ADDREF(t);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &t);
}
/* }}} */

/* {{{ Random\Randomizer::__unserialize() */
PHP_METHOD(Random_Randomizer, __unserialize)
{
	php_random_randomizer *randomizer = Z_RANDOM_RANDOMIZER_P(ZEND_THIS);
	HashTable *d;
	zval *members_zv;
	zval *zengine;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(d);
	ZEND_PARSE_PARAMETERS_END();

	/* Verify the expected number of elements, this implicitly ensures that no additional elements are present. */
	if (zend_hash_num_elements(d) != 1) {
		zend_throw_exception(NULL, "Invalid serialization data for Random\\Randomizer object", 0);
		RETURN_THROWS();
	}

	members_zv = zend_hash_index_find(d, 0);
	if (!members_zv || Z_TYPE_P(members_zv) != IS_ARRAY) {
		zend_throw_exception(NULL, "Invalid serialization data for Random\\Randomizer object", 0);
		RETURN_THROWS();
	}
	object_properties_load(&randomizer->std, Z_ARRVAL_P(members_zv));
	if (EG(exception)) {
		zend_throw_exception(NULL, "Invalid serialization data for Random\\Randomizer object", 0);
		RETURN_THROWS();
	}

	zengine = zend_read_property(randomizer->std.ce, &randomizer->std, "engine", strlen("engine"), 1, NULL);
	if (Z_TYPE_P(zengine) != IS_OBJECT || !instanceof_function(Z_OBJCE_P(zengine), random_ce_Random_Engine)) {
		zend_throw_exception(NULL, "Invalid serialization data for Random\\Randomizer object", 0);
		RETURN_THROWS();
	}

	randomizer_common_init(randomizer, Z_OBJ_P(zengine));
}
/* }}} */
