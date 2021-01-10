/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Go Kudo <zeriyoshi@gmail.com>                               |
   +----------------------------------------------------------------------+
*/

#include <stdlib.h>

#include "php.h"
#include "php_rng.h"

#include "rng_rnginterface_arginfo.h"
#include "rng_rng64interface_arginfo.h"

#include "rng_xorshift128plus.h"
#include "rng_mt19937.h"
#include "rng_osrng.h"

PHPAPI zend_class_entry *rng_ce_RNG_RNGInterface;
PHPAPI zend_class_entry *rng_ce_RNG_RNG64Interface;

PHPAPI php_rng* php_rng_initialize(uint32_t (*next)(php_rng*), uint64_t (*next64)(php_rng*))
{
	php_rng *rng = (php_rng*)ecalloc(1, sizeof(php_rng));
	rng->next = next;
	rng->next64 = next64;

	return rng;
}

PHPAPI int php_rng_next(uint32_t *result, zval *obj)
{	
	/* If internal classes, use php_rng struct. */
	if (Z_OBJCE_P(obj)->type == ZEND_INTERNAL_CLASS) {
		php_rng *rng = Z_RNG_P(obj);
		
		if (rng->next) {
			*result = rng->next(rng);
		} else if (rng->next64) {
			*result = (uint32_t) (rng->next64(rng));
		} else {
			return FAILURE;
		}
		
		return SUCCESS;
	}

	zend_function *function;
	zval function_name, retval;

	ZVAL_STRING(&function_name, "next");
	function = zend_hash_find_ptr(&Z_OBJCE_P(obj)->function_table, Z_STR(function_name));
	zval_ptr_dtor(&function_name);

	if (!function) {
		return FAILURE;
	}

	zend_call_known_instance_method_with_0_params(function, Z_OBJ_P(obj), &retval);
	*result = Z_LVAL(retval);

	return SUCCESS;
}

PHPAPI int php_rng_next64(uint64_t *result, zval *obj)
{	
	/* If internal classes, use php_rng struct. */
	if (Z_OBJCE_P(obj)->type == ZEND_INTERNAL_CLASS) {
		php_rng *rng = Z_RNG_P(obj);
		
		if (rng->next64) {
			*result = rng->next64(rng);
		} else if (rng->next) {
			*result = rng->next(rng);
			*result = (*result << 32) | rng->next(rng);
		} else {
			return FAILURE;
		}

		return SUCCESS;
	}

	zend_function *function;
	zval function_name, retval;

	ZVAL_STRING(&function_name, "next64");
	function = zend_hash_find_ptr(&Z_OBJCE_P(obj)->function_table, Z_STR(function_name));
	zval_ptr_dtor(&function_name);

	if (!function) {
		return FAILURE;
	}

	zend_call_known_instance_method_with_0_params(function, Z_OBJ_P(obj), &retval);
	*result = Z_LVAL(retval);

	return SUCCESS;
}

static uint32_t rng_rand_range32(zval *obj, uint32_t umax)
{
	uint32_t result, limit;

	php_rng_next(&result, obj);

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
		php_rng_next(&result, obj);
	}

	return result % umax;
}

#if ZEND_ULONG_MAX > UINT32_MAX
static uint64_t rng_rand_range64(zval *obj, uint64_t umax)
{
	uint64_t result, limit;
	
	php_rng_next64(&result, obj);

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
		php_rng_next64(&result, obj);
	}

	return result % umax;
}
#endif

PHPAPI zend_long php_rng_range(zval *obj, zend_long min, zend_long max)
{
	/* Checking object defined range implementation. */
	if (Z_OBJCE_P(obj)->type == ZEND_INTERNAL_CLASS) {
		php_rng *rng = Z_RNG_P(obj);
		if (rng->range) {
			return rng->range(rng);
		}
	}

	/* The implementation is stolen from php_mt_rand_range() */
	zend_ulong umax = max - min;
	
#if ZEND_ULONG_MAX > UINT32_MAX
	if (umax > UINT32_MAX) {
		return (zend_long) (rng_rand_range64(obj, umax) + min);
	}
#endif

	return (zend_long) (rng_rand_range32(obj, umax) + min);
}

PHP_FUNCTION(rng_rand)
{
	zval *zrng;
	zend_long min, max;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_OBJECT_OF_CLASS(zrng, rng_ce_RNG_RNGInterface)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(min)
		Z_PARAM_LONG(max)
	ZEND_PARSE_PARAMETERS_END();

	if (ZEND_NUM_ARGS() == 1) {
		uint32_t result;

		php_rng_next(&result, zrng);
		RETURN_LONG((zend_long) (result >> 1));
	}

	if (UNEXPECTED(max < min)) {
		zend_argument_value_error(2, "must be greater than or equal to argument #1 ($min)");
		RETURN_THROWS();
	}

	RETURN_LONG(php_rng_range(zrng, min, max));
}

PHP_FUNCTION(rng_bytes)
{
	zval *zrng;
	zend_long size;
	zend_string *result;
	uint32_t buf;
	uint8_t *bytes;
	size_t generated_bytes = 0;
	int i;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_OBJECT_OF_CLASS(zrng, rng_ce_RNG_RNGInterface)
		Z_PARAM_LONG(size)
	ZEND_PARSE_PARAMETERS_END();

	if (size < 1) {
		zend_argument_value_error(1, "must be greater than 0");
		RETURN_THROWS();
	}

	result = zend_string_alloc(size, 0);

	while (generated_bytes <= size) {
		php_rng_next(&buf, zrng);
		bytes = (uint8_t *) &buf;
		for (i = 0; i < (sizeof(uint32_t) / sizeof(uint8_t)); i ++) {
			ZSTR_VAL(result)[generated_bytes + i] = bytes[i];
			if ((generated_bytes + i) >= size) {
				ZSTR_VAL(result)[size] = '\0';
				RETURN_STR(result);
			}
		}
		generated_bytes += (sizeof(uint32_t) / sizeof(uint8_t));
	}
}

PHP_MINIT_FUNCTION(rng)
{
	zend_class_entry ce_rng, ce_rng64;

	INIT_CLASS_ENTRY(ce_rng, RNG_NAMESPACE "RNGInterface", class_RNG_RNGInterface_methods);
	rng_ce_RNG_RNGInterface = zend_register_internal_interface(&ce_rng);

	INIT_CLASS_ENTRY(ce_rng64, RNG_NAMESPACE "RNG64Interface", class_RNG_RNG64Interface_methods);
	rng_ce_RNG_RNG64Interface = zend_register_internal_interface(&ce_rng64);
	zend_class_implements(rng_ce_RNG_RNG64Interface, 1, rng_ce_RNG_RNGInterface);

	PHP_MINIT(rng_xorshift128plus)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(rng_mt19937)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(rng_osrng)(INIT_FUNC_ARGS_PASSTHRU);

	return SUCCESS;
}
