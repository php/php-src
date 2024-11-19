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
   | Author: Andi Gutmans <andi@php.net>                                  |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"

#ifdef HAVE_BCMATH

#include "php_ini.h"
#include "zend_exceptions.h"
#include "zend_interfaces.h"
#include "bcmath_arginfo.h"
#include "ext/standard/info.h"
#include "php_bcmath.h"
#include "libbcmath/src/bcmath.h"

/* Always pair SETUP with TEARDOWN, and do so in the outer scope!
 * Should not be used when data can escape the function. */
#define BC_ARENA_SETUP \
	char bc_arena[BC_ARENA_SIZE]; \
	BCG(arena) = bc_arena;

#define BC_ARENA_TEARDOWN \
	BCG(arena) = NULL; \
	BCG(arena_offset) = 0;

ZEND_DECLARE_MODULE_GLOBALS(bcmath)
static PHP_GINIT_FUNCTION(bcmath);
static PHP_GSHUTDOWN_FUNCTION(bcmath);
static PHP_MINIT_FUNCTION(bcmath);
static PHP_MSHUTDOWN_FUNCTION(bcmath);
static PHP_MINFO_FUNCTION(bcmath);

zend_module_entry bcmath_module_entry = {
	STANDARD_MODULE_HEADER,
	"bcmath",
	ext_functions,
	PHP_MINIT(bcmath),
	PHP_MSHUTDOWN(bcmath),
	NULL,
	NULL,
	PHP_MINFO(bcmath),
	PHP_BCMATH_VERSION,
	PHP_MODULE_GLOBALS(bcmath),
	PHP_GINIT(bcmath),
	PHP_GSHUTDOWN(bcmath),
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_BCMATH
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(bcmath)
#endif

ZEND_INI_MH(OnUpdateScale)
{
	int *p;
	zend_long tmp;

	tmp = zend_ini_parse_quantity_warn(new_value, entry->name);
	if (tmp < 0 || tmp > INT_MAX) {
		return FAILURE;
	}

	p = (int *) ZEND_INI_GET_ADDR();
	*p = (int) tmp;

	return SUCCESS;
}

/* {{{ PHP_INI */
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("bcmath.scale", "0", PHP_INI_ALL, OnUpdateScale, bc_precision, zend_bcmath_globals, bcmath_globals)
PHP_INI_END()
/* }}} */

/* {{{ PHP_GINIT_FUNCTION */
static PHP_GINIT_FUNCTION(bcmath)
{
#if defined(COMPILE_DL_BCMATH) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	bcmath_globals->bc_precision = 0;
	bcmath_globals->arena = NULL;
	bcmath_globals->arena_offset = 0;
	bc_init_numbers();
}
/* }}} */

/* {{{ PHP_GSHUTDOWN_FUNCTION */
static PHP_GSHUTDOWN_FUNCTION(bcmath)
{
	_bc_free_num_ex(&bcmath_globals->_zero_, 1);
	_bc_free_num_ex(&bcmath_globals->_one_, 1);
	_bc_free_num_ex(&bcmath_globals->_two_, 1);
	bcmath_globals->arena = NULL;
	bcmath_globals->arena_offset = 0;
}
/* }}} */

static void bcmath_number_register_class(void);

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(bcmath)
{
	REGISTER_INI_ENTRIES();
	bcmath_number_register_class();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(bcmath)
{
	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(bcmath)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "BCMath support", "enabled");
	php_info_print_table_end();
	DISPLAY_INI_ENTRIES();
}
/* }}} */

static zend_always_inline zend_result bcmath_check_scale(zend_long scale, uint32_t arg_num)
{
	if (UNEXPECTED(scale < 0 || scale > INT_MAX)) {
		zend_argument_value_error(arg_num, "must be between 0 and %d", INT_MAX);
		return FAILURE;
	}
	return SUCCESS;
}

static void php_long2num(bc_num *num, zend_long lval)
{
	*num = bc_long2num(lval);
}

static zend_result php_str2num_ex(bc_num *num, const zend_string *str, size_t *full_scale)
{
	if (!bc_str2num(num, ZSTR_VAL(str), ZSTR_VAL(str) + ZSTR_LEN(str), 0, full_scale, true)) {
		return FAILURE;
	}

	return SUCCESS;
}

/* {{{ php_str2num
   Convert to bc_num detecting scale */
static zend_result php_str2num(bc_num *num, const zend_string *str)
{
	return php_str2num_ex(num, str, NULL);
}
/* }}} */

/* {{{ Returns the sum of two arbitrary precision numbers */
PHP_FUNCTION(bcadd)
{
	zend_string *left, *right;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num first = NULL, second = NULL, result = NULL;
	int scale;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(left)
		Z_PARAM_STR(right)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_param, scale_param_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (scale_param_is_null) {
		scale = BCG(bc_precision);
	} else if (bcmath_check_scale(scale_param, 3) == FAILURE) {
		RETURN_THROWS();
	} else {
		scale = (int) scale_param;
	}

	BC_ARENA_SETUP;

	if (php_str2num(&first, left) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&second, right) == FAILURE) {
		zend_argument_value_error(2, "is not well-formed");
		goto cleanup;
	}

	result = bc_add (first, second, scale);

	RETVAL_NEW_STR(bc_num2str_ex(result, scale));

	cleanup: {
		bc_free_num(&first);
		bc_free_num(&second);
		bc_free_num(&result);
		BC_ARENA_TEARDOWN;
	};
}
/* }}} */

/* {{{ Returns the difference between two arbitrary precision numbers */
PHP_FUNCTION(bcsub)
{
	zend_string *left, *right;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num first = NULL, second = NULL, result = NULL;
	int scale;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(left)
		Z_PARAM_STR(right)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_param, scale_param_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (scale_param_is_null) {
		scale = BCG(bc_precision);
	} else if (bcmath_check_scale(scale_param, 3) == FAILURE) {
		RETURN_THROWS();
	} else {
		scale = (int) scale_param;
	}

	BC_ARENA_SETUP;

	if (php_str2num(&first, left) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&second, right) == FAILURE) {
		zend_argument_value_error(2, "is not well-formed");
		goto cleanup;
	}

	result = bc_sub (first, second, scale);

	RETVAL_NEW_STR(bc_num2str_ex(result, scale));

	cleanup: {
		bc_free_num(&first);
		bc_free_num(&second);
		bc_free_num(&result);
		BC_ARENA_TEARDOWN;
	};
}
/* }}} */

/* {{{ Returns the multiplication of two arbitrary precision numbers */
PHP_FUNCTION(bcmul)
{
	zend_string *left, *right;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num first = NULL, second = NULL, result = NULL;
	int scale;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(left)
		Z_PARAM_STR(right)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_param, scale_param_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (scale_param_is_null) {
		scale = BCG(bc_precision);
	} else if (bcmath_check_scale(scale_param, 3) == FAILURE) {
		RETURN_THROWS();
	} else {
		scale = (int) scale_param;
	}

	BC_ARENA_SETUP;

	if (php_str2num(&first, left) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&second, right) == FAILURE) {
		zend_argument_value_error(2, "is not well-formed");
		goto cleanup;
	}

	result = bc_multiply (first, second, scale);

	RETVAL_NEW_STR(bc_num2str_ex(result, scale));

	cleanup: {
		bc_free_num(&first);
		bc_free_num(&second);
		bc_free_num(&result);
		BC_ARENA_TEARDOWN;
	};
}
/* }}} */

/* {{{ Returns the quotient of two arbitrary precision numbers (division) */
PHP_FUNCTION(bcdiv)
{
	zend_string *left, *right;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num first = NULL, second = NULL, result;
	int scale = BCG(bc_precision);

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(left)
		Z_PARAM_STR(right)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_param, scale_param_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (scale_param_is_null) {
		scale = BCG(bc_precision);
	} else if (bcmath_check_scale(scale_param, 3) == FAILURE) {
		RETURN_THROWS();
	} else {
		scale = (int) scale_param;
	}

	BC_ARENA_SETUP;

	bc_init_num(&result);

	if (php_str2num(&first, left) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&second, right) == FAILURE) {
		zend_argument_value_error(2, "is not well-formed");
		goto cleanup;
	}

	if (!bc_divide(first, second, &result, scale)) {
		zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Division by zero");
		goto cleanup;
	}

	RETVAL_NEW_STR(bc_num2str_ex(result, scale));

	cleanup: {
		bc_free_num(&first);
		bc_free_num(&second);
		bc_free_num(&result);
		BC_ARENA_TEARDOWN;
	};
}
/* }}} */

/* {{{ Returns the modulus of the two arbitrary precision operands */
PHP_FUNCTION(bcmod)
{
	zend_string *left, *right;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num first = NULL, second = NULL, result;
	int scale = BCG(bc_precision);

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(left)
		Z_PARAM_STR(right)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_param, scale_param_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (scale_param_is_null) {
		scale = BCG(bc_precision);
	} else if (bcmath_check_scale(scale_param, 3) == FAILURE) {
		RETURN_THROWS();
	} else {
		scale = (int) scale_param;
	}

	BC_ARENA_SETUP;

	bc_init_num(&result);

	if (php_str2num(&first, left) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&second, right) == FAILURE) {
		zend_argument_value_error(2, "is not well-formed");
		goto cleanup;
	}

	if (!bc_modulo(first, second, &result, scale)) {
		zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Modulo by zero");
		goto cleanup;
	}

	RETVAL_NEW_STR(bc_num2str_ex(result, scale));

	cleanup: {
		bc_free_num(&first);
		bc_free_num(&second);
		bc_free_num(&result);
		BC_ARENA_TEARDOWN;
	};
}
/* }}} */

PHP_FUNCTION(bcdivmod)
{
	zend_string *left, *right;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num first = NULL, second = NULL, quot = NULL, rem = NULL;
	int scale = BCG(bc_precision);

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(left)
		Z_PARAM_STR(right)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_param, scale_param_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (scale_param_is_null) {
		scale = BCG(bc_precision);
	} else if (bcmath_check_scale(scale_param, 3) == FAILURE) {
		RETURN_THROWS();
	} else {
		scale = (int) scale_param;
	}

	BC_ARENA_SETUP;

	if (php_str2num(&first, left) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&second, right) == FAILURE) {
		zend_argument_value_error(2, "is not well-formed");
		goto cleanup;
	}

	if (!bc_divmod(first, second, &quot, &rem, scale)) {
		zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Division by zero");
		goto cleanup;
	}

	zval z_quot, z_rem;
	ZVAL_STR(&z_quot, bc_num2str_ex(quot, 0));
	ZVAL_STR(&z_rem, bc_num2str_ex(rem, scale));

	RETVAL_ARR(zend_new_pair(&z_quot, &z_rem));

	cleanup: {
		bc_free_num(&first);
		bc_free_num(&second);
		bc_free_num(&quot);
		bc_free_num(&rem);
		BC_ARENA_TEARDOWN;
	};
}

/* {{{ Returns the value of an arbitrary precision number raised to the power of another reduced by a modulus */
PHP_FUNCTION(bcpowmod)
{
	zend_string *base_str, *exponent_str, *modulus_str;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num bc_base = NULL, bc_expo = NULL, bc_modulus = NULL, result;
	int scale = BCG(bc_precision);

	ZEND_PARSE_PARAMETERS_START(3, 4)
		Z_PARAM_STR(base_str)
		Z_PARAM_STR(exponent_str)
		Z_PARAM_STR(modulus_str)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_param, scale_param_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (scale_param_is_null) {
		scale = BCG(bc_precision);
	} else if (bcmath_check_scale(scale_param, 4) == FAILURE) {
		RETURN_THROWS();
	} else {
		scale = (int) scale_param;
	}

	BC_ARENA_SETUP;

	bc_init_num(&result);

	if (php_str2num(&bc_base, base_str) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&bc_expo, exponent_str) == FAILURE) {
		zend_argument_value_error(2, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&bc_modulus, modulus_str) == FAILURE) {
		zend_argument_value_error(3, "is not well-formed");
		goto cleanup;
	}

	raise_mod_status status = bc_raisemod(bc_base, bc_expo, bc_modulus, &result, scale);
	switch (status) {
		case BASE_HAS_FRACTIONAL:
			zend_argument_value_error(1, "cannot have a fractional part");
			goto cleanup;
		case EXPO_HAS_FRACTIONAL:
			zend_argument_value_error(2, "cannot have a fractional part");
			goto cleanup;
		case EXPO_IS_NEGATIVE:
			zend_argument_value_error(2, "must be greater than or equal to 0");
			goto cleanup;
		case MOD_HAS_FRACTIONAL:
			zend_argument_value_error(3, "cannot have a fractional part");
			goto cleanup;
		case MOD_IS_ZERO:
			zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Modulo by zero");
			goto cleanup;
		case OK:
			RETVAL_NEW_STR(bc_num2str_ex(result, scale));
			break;
		EMPTY_SWITCH_DEFAULT_CASE();
	}

	cleanup: {
		bc_free_num(&bc_base);
		bc_free_num(&bc_expo);
		bc_free_num(&bc_modulus);
		bc_free_num(&result);
		BC_ARENA_TEARDOWN;
	};
}
/* }}} */

/* {{{ Returns the value of an arbitrary precision number raised to the power of another */
PHP_FUNCTION(bcpow)
{
	zend_string *base_str, *exponent_str;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num first = NULL, bc_exponent = NULL, result;
	int scale = BCG(bc_precision);

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(base_str)
		Z_PARAM_STR(exponent_str)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_param, scale_param_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (scale_param_is_null) {
		scale = BCG(bc_precision);
	} else if (bcmath_check_scale(scale_param, 3) == FAILURE) {
		RETURN_THROWS();
	} else {
		scale = (int) scale_param;
	}

	BC_ARENA_SETUP;

	bc_init_num(&result);

	if (php_str2num(&first, base_str) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&bc_exponent, exponent_str) == FAILURE) {
		zend_argument_value_error(2, "is not well-formed");
		goto cleanup;
	}

	/* Check the exponent for scale digits and convert to a long. */
	if (bc_exponent->n_scale != 0) {
		zend_argument_value_error(2, "cannot have a fractional part");
		goto cleanup;
	}
	long exponent = bc_num2long(bc_exponent);
	if (exponent == 0 && (bc_exponent->n_len > 1 || bc_exponent->n_value[0] != 0)) {
		zend_argument_value_error(2, "is too large");
		goto cleanup;
	}

	if (!bc_raise(first, exponent, &result, scale)) {
		zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Negative power of zero");
		goto cleanup;
	}

	RETVAL_NEW_STR(bc_num2str_ex(result, scale));

	cleanup: {
		bc_free_num(&first);
		bc_free_num(&bc_exponent);
		bc_free_num(&result);
		BC_ARENA_TEARDOWN;
	};
}
/* }}} */

/* {{{ Returns the square root of an arbitrary precision number */
PHP_FUNCTION(bcsqrt)
{
	zend_string *left;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num result = NULL;
	int scale = BCG(bc_precision);

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(left)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_param, scale_param_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (scale_param_is_null) {
		scale = BCG(bc_precision);
	} else if (bcmath_check_scale(scale_param, 2) == FAILURE) {
		RETURN_THROWS();
	} else {
		scale = (int) scale_param;
	}

	BC_ARENA_SETUP;

	if (php_str2num(&result, left) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (bc_sqrt (&result, scale) != 0) {
		RETVAL_NEW_STR(bc_num2str_ex(result, scale));
	} else {
		zend_argument_value_error(1, "must be greater than or equal to 0");
	}

	cleanup: {
		bc_free_num(&result);
		BC_ARENA_TEARDOWN;
	};
}
/* }}} */

/* {{{ Compares two arbitrary precision numbers */
PHP_FUNCTION(bccomp)
{
	zend_string *left, *right;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num first = NULL, second = NULL;
	int scale = BCG(bc_precision);

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(left)
		Z_PARAM_STR(right)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_param, scale_param_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (scale_param_is_null) {
		scale = BCG(bc_precision);
	} else if (bcmath_check_scale(scale_param, 3) == FAILURE) {
		RETURN_THROWS();
	} else {
		scale = (int) scale_param;
	}

	BC_ARENA_SETUP;

	if (!bc_str2num(&first, ZSTR_VAL(left), ZSTR_VAL(left) + ZSTR_LEN(left), scale, NULL, false)) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (!bc_str2num(&second, ZSTR_VAL(right), ZSTR_VAL(right) + ZSTR_LEN(right), scale, NULL, false)) {
		zend_argument_value_error(2, "is not well-formed");
		goto cleanup;
	}

	RETVAL_LONG(bc_compare(first, second, scale));

	cleanup: {
		bc_free_num(&first);
		bc_free_num(&second);
		BC_ARENA_TEARDOWN;
	};
}
/* }}} */

/* {{{ floor or ceil */
static void bcfloor_or_bcceil(INTERNAL_FUNCTION_PARAMETERS, bool is_floor)
{
	zend_string *numstr;
	bc_num num = NULL, result = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(numstr)
	ZEND_PARSE_PARAMETERS_END();

	BC_ARENA_SETUP;

	if (php_str2num(&num, numstr) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	result = bc_floor_or_ceil(num, is_floor);
	RETVAL_NEW_STR(bc_num2str_ex(result, 0));

	cleanup: {
		bc_free_num(&num);
		bc_free_num(&result);
		BC_ARENA_TEARDOWN;
	};
}
/* }}} */

/* {{{ Returns floor of num */
PHP_FUNCTION(bcfloor)
{
	bcfloor_or_bcceil(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}
/* }}} */

/* {{{ Returns ceil of num */
PHP_FUNCTION(bcceil)
{
	bcfloor_or_bcceil(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}
/* }}} */

/* {{{ Returns num rounded to the digits specified by precision. */
PHP_FUNCTION(bcround)
{
	zend_string *numstr;
	zend_long precision = 0;
	zend_long mode = PHP_ROUND_HALF_UP;
	zend_object *mode_object = NULL;
	bc_num num = NULL, result;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_STR(numstr)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(precision)
		Z_PARAM_OBJ_OF_CLASS(mode_object, rounding_mode_ce)
	ZEND_PARSE_PARAMETERS_END();

	if (mode_object != NULL) {
		mode = php_math_round_mode_from_enum(mode_object);
	}

	switch (mode) {
		case PHP_ROUND_HALF_UP:
		case PHP_ROUND_HALF_DOWN:
		case PHP_ROUND_HALF_EVEN:
		case PHP_ROUND_HALF_ODD:
		case PHP_ROUND_CEILING:
		case PHP_ROUND_FLOOR:
		case PHP_ROUND_TOWARD_ZERO:
		case PHP_ROUND_AWAY_FROM_ZERO:
			break;
		default:
			/* This is currently unreachable, but might become reachable when new modes are added. */
			zend_argument_value_error(3, "is an unsupported rounding mode");
			return;
	}

	BC_ARENA_SETUP;

	bc_init_num(&result);

	if (php_str2num(&num, numstr) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	bc_round(num, precision, mode, &result);
	RETVAL_NEW_STR(bc_num2str_ex(result, result->n_scale));

	cleanup: {
		bc_free_num(&num);
		bc_free_num(&result);
		BC_ARENA_TEARDOWN;
	};
}
/* }}} */

/* {{{ Sets default scale parameter for all bc math functions */
PHP_FUNCTION(bcscale)
{
	zend_long old_scale, new_scale;
	bool new_scale_is_null = 1;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(new_scale, new_scale_is_null)
	ZEND_PARSE_PARAMETERS_END();

	old_scale = BCG(bc_precision);

	if (!new_scale_is_null) {
		if (bcmath_check_scale(new_scale, 1) == FAILURE) {
			RETURN_THROWS();
		}

		zend_string *ini_name = ZSTR_INIT_LITERAL("bcmath.scale", 0);
		zend_string *new_scale_str = zend_long_to_str(new_scale);
		zend_alter_ini_entry(ini_name, new_scale_str, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
		zend_string_release(new_scale_str);
		zend_string_release(ini_name);
	}

	RETURN_LONG(old_scale);
}
/* }}} */



static zend_class_entry *bcmath_number_ce;
static zend_object_handlers bcmath_number_obj_handlers;

static zend_result bcmath_number_do_operation(uint8_t opcode, zval *ret_val, zval *op1, zval *op2);
static int bcmath_number_compare(zval *op1, zval *op2);

#if SIZEOF_SIZE_T >= 8
#  define CHECK_RET_SCALE_OVERFLOW(scale, origin_scale) (scale > INT_MAX)
#else
#  define CHECK_RET_SCALE_OVERFLOW(scale, origin_scale) (scale > INT_MAX || scale < origin_scale)
#endif
#define CHECK_SCALE_OVERFLOW(scale) (scale > INT_MAX)

static zend_always_inline bcmath_number_obj_t *get_bcmath_number_from_obj(const zend_object *obj)
{
	return (bcmath_number_obj_t*)((char*)(obj) - XtOffsetOf(bcmath_number_obj_t, std));
}

static zend_always_inline bcmath_number_obj_t *get_bcmath_number_from_zval(const zval *zv)
{
	return get_bcmath_number_from_obj(Z_OBJ_P(zv));
}

static zend_always_inline zend_string *bcmath_number_value_to_str(bcmath_number_obj_t *intern)
{
	if (intern->value == NULL) {
		intern->value = bc_num2str_ex(intern->num, intern->scale);
	}
	return intern->value;
}

static zend_object *bcmath_number_create(zend_class_entry *ce)
{
	bcmath_number_obj_t *intern = zend_object_alloc(sizeof(bcmath_number_obj_t), ce);

	zend_object_std_init(&intern->std, ce);
	object_properties_init(&intern->std, ce);

	intern->scale = 1;

	return &intern->std;
}

static void bcmath_number_free(zend_object *obj)
{
	bcmath_number_obj_t *intern = get_bcmath_number_from_obj(obj);
	if (intern->num) {
		bc_free_num(&intern->num);
		intern->num = NULL;
	}
	if (intern->value) {
		zend_string_release(intern->value);
		intern->value = NULL;
	}
	zend_object_std_dtor(&intern->std);
}

static zend_object *bcmath_number_clone(zend_object *obj)
{
	bcmath_number_obj_t *original = get_bcmath_number_from_obj(obj);
	bcmath_number_obj_t *clone = get_bcmath_number_from_obj(bcmath_number_create(bcmath_number_ce));

	clone->num = bc_copy_num(original->num);
	if (original->value) {
		clone->value = zend_string_copy(original->value);
	}
	clone->scale = original->scale;

	return &clone->std;
}

static HashTable *bcmath_number_get_properties_for(zend_object *obj, zend_prop_purpose purpose)
{
	zval zv;
	bcmath_number_obj_t *intern = get_bcmath_number_from_obj(obj);
	HashTable *props = zend_array_dup(zend_std_get_properties(obj));

	ZVAL_STR_COPY(&zv, bcmath_number_value_to_str(intern));
	zend_hash_update(props, ZSTR_KNOWN(ZEND_STR_VALUE), &zv);
	ZVAL_LONG(&zv, intern->scale);
	zend_hash_str_update(props, ZEND_STRL("scale"), &zv);

	return props;
}

static zval *bcmath_number_write_property(zend_object *obj, zend_string *name, zval *value, void **cache_slot)
{
	if (zend_string_equals(name, ZSTR_KNOWN(ZEND_STR_VALUE)) || zend_string_equals_literal(name, "scale")) {
		zend_readonly_property_modification_error_ex(ZSTR_VAL(obj->ce->name), ZSTR_VAL(name));
		return &EG(error_zval);
	}

	return zend_std_write_property(obj, name, value, cache_slot);
}

static void bcmath_number_unset_property(zend_object *obj, zend_string *name, void **cache_slot)
{
	if (zend_string_equals(name, ZSTR_KNOWN(ZEND_STR_VALUE)) || zend_string_equals_literal(name, "scale")) {
		zend_throw_error(NULL, "Cannot unset readonly property %s::$%s", ZSTR_VAL(obj->ce->name), ZSTR_VAL(name));
		return;
	}

	zend_std_unset_property(obj, name, cache_slot);
}

static zval *bcmath_number_read_property(zend_object *obj, zend_string *name, int type, void **cache_slot, zval *rv)
{
	bcmath_number_obj_t *intern = get_bcmath_number_from_obj(obj);

	if (zend_string_equals(name, ZSTR_KNOWN(ZEND_STR_VALUE))) {
		ZVAL_STR_COPY(rv, bcmath_number_value_to_str(intern));
		return rv;
	}

	if (zend_string_equals_literal(name, "scale")) {
		ZVAL_LONG(rv, intern->scale);
		return rv;
	}

	return zend_std_read_property(obj, name, type, cache_slot, rv);
}

static int bcmath_number_has_property(zend_object *obj, zend_string *name, int check_empty, void **cache_slot)
{
	if (check_empty == ZEND_PROPERTY_NOT_EMPTY) {
		bcmath_number_obj_t *intern = get_bcmath_number_from_obj(obj);

		if (zend_string_equals(name, ZSTR_KNOWN(ZEND_STR_VALUE))) {
			return !bc_is_zero(intern->num);
		}

		if (zend_string_equals_literal(name, "scale")) {
			return intern->scale != 0;
		}
	}
	return zend_string_equals(name, ZSTR_KNOWN(ZEND_STR_VALUE)) || zend_string_equals_literal(name, "scale");
}

static zend_result bcmath_number_cast_object(zend_object *obj, zval *ret, int type)
{
	if (type == _IS_BOOL) {
		bcmath_number_obj_t *intern = get_bcmath_number_from_obj(obj);
		ZVAL_BOOL(ret, !bc_is_zero(intern->num));
		return SUCCESS;
	}

	return zend_std_cast_object_tostring(obj, ret, type);
}

static void bcmath_number_register_class(void)
{
	bcmath_number_ce = register_class_BcMath_Number(zend_ce_stringable);
	bcmath_number_ce->create_object = bcmath_number_create;
	bcmath_number_ce->default_object_handlers = &bcmath_number_obj_handlers;

	memcpy(&bcmath_number_obj_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	bcmath_number_obj_handlers.offset = XtOffsetOf(bcmath_number_obj_t, std);
	bcmath_number_obj_handlers.free_obj = bcmath_number_free;
	bcmath_number_obj_handlers.clone_obj = bcmath_number_clone;
	bcmath_number_obj_handlers.do_operation = bcmath_number_do_operation;
	bcmath_number_obj_handlers.compare = bcmath_number_compare;
	bcmath_number_obj_handlers.write_property = bcmath_number_write_property;
	bcmath_number_obj_handlers.unset_property = bcmath_number_unset_property;
	bcmath_number_obj_handlers.has_property = bcmath_number_has_property;
	bcmath_number_obj_handlers.read_property = bcmath_number_read_property;
	bcmath_number_obj_handlers.get_properties_for = bcmath_number_get_properties_for;
	bcmath_number_obj_handlers.cast_object = bcmath_number_cast_object;
}

static zend_always_inline void bcmath_number_add_internal(
	bc_num n1, bc_num n2, bc_num *ret,
	size_t n1_full_scale, size_t n2_full_scale, size_t *scale, bool auto_scale
) {
	if (auto_scale) {
		*scale = MAX(n1_full_scale, n2_full_scale);
	}
	*ret = bc_add(n1, n2, *scale);
	(*ret)->n_scale = MIN(*scale, (*ret)->n_scale);
	bc_rm_trailing_zeros(*ret);
}

static zend_always_inline void bcmath_number_sub_internal(
	bc_num n1, bc_num n2, bc_num *ret,
	size_t n1_full_scale, size_t n2_full_scale, size_t *scale, bool auto_scale
) {
	if (auto_scale) {
		*scale = MAX(n1_full_scale, n2_full_scale);
	}
	*ret = bc_sub(n1, n2, *scale);
	(*ret)->n_scale = MIN(*scale, (*ret)->n_scale);
	bc_rm_trailing_zeros(*ret);
}

static zend_always_inline zend_result bcmath_number_mul_internal(
	bc_num n1, bc_num n2, bc_num *ret,
	size_t n1_full_scale, size_t n2_full_scale, size_t *scale, bool auto_scale
) {
	if (auto_scale) {
		*scale = n1_full_scale + n2_full_scale;
		if (UNEXPECTED(CHECK_RET_SCALE_OVERFLOW(*scale, n1_full_scale))) {
			zend_value_error("scale of the result is too large");
			return FAILURE;
		}
	}
	*ret = bc_multiply(n1, n2, *scale);
	(*ret)->n_scale = MIN(*scale, (*ret)->n_scale);
	bc_rm_trailing_zeros(*ret);
	return SUCCESS;
}

static zend_always_inline zend_result bcmath_number_div_internal(
	bc_num n1, bc_num n2, bc_num *ret,
	size_t n1_full_scale, size_t *scale, bool auto_scale
) {
	if (auto_scale) {
		*scale = n1_full_scale + BC_MATH_NUMBER_EXPAND_SCALE;
		if (UNEXPECTED(CHECK_RET_SCALE_OVERFLOW(*scale, n1_full_scale))) {
			zend_value_error("scale of the result is too large");
			return FAILURE;
		}
	}
	if (!bc_divide(n1, n2, ret, *scale)) {
		zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Division by zero");
		return FAILURE;
	}
	bc_rm_trailing_zeros(*ret);
	if (auto_scale) {
		size_t diff = *scale - (*ret)->n_scale;
		*scale -= diff > BC_MATH_NUMBER_EXPAND_SCALE ? BC_MATH_NUMBER_EXPAND_SCALE : diff;
	}
	return SUCCESS;
}

static zend_always_inline zend_result bcmath_number_mod_internal(
	bc_num n1, bc_num n2, bc_num *ret,
	size_t n1_full_scale, size_t n2_full_scale, size_t *scale, bool auto_scale
) {
	if (auto_scale) {
		*scale = MAX(n1_full_scale, n2_full_scale);
	}
	if (!bc_modulo(n1, n2, ret, *scale)) {
		zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Modulo by zero");
		return FAILURE;
	}
	bc_rm_trailing_zeros(*ret);
	return SUCCESS;
}

static zend_result bcmath_number_pow_internal(
	bc_num n1, bc_num n2, bc_num *ret,
	size_t n1_full_scale, size_t *scale, bool auto_scale, bool is_op
) {
	/* Check the exponent for scale digits and convert to a long. */
	if (UNEXPECTED(n2->n_scale != 0)) {
		if (is_op) {
			zend_value_error("exponent cannot have a fractional part");
		} else {
			zend_argument_value_error(1, "exponent cannot have a fractional part");
		}
		return FAILURE;
	}
	long exponent = bc_num2long(n2);

	bool scale_expand = false;
	if (auto_scale) {
		if (exponent > 0) {
			*scale = n1_full_scale * exponent;
			if (UNEXPECTED(*scale > INT_MAX || *scale < n1_full_scale)) {
				zend_value_error("scale of the result is too large");
				return FAILURE;
			}
		} else if (exponent < 0) {
			*scale = n1_full_scale + BC_MATH_NUMBER_EXPAND_SCALE;
			if (UNEXPECTED(CHECK_RET_SCALE_OVERFLOW(*scale, n1_full_scale))) {
				zend_value_error("scale of the result is too large");
				return FAILURE;
			}
			scale_expand = true;
		} else {
			*scale = 0;
		}
	}

	/**
	 * bc_num2long() returns 0 if exponent is too large.
	 * Here, if n2->n_value is not 0 but exponent is 0, it is considered too large.
	 */
	if (UNEXPECTED(exponent == 0 && (n2->n_len > 1 || n2->n_value[0] != 0))) {
		if (is_op) {
			zend_value_error("exponent is too large");
		} else {
			zend_argument_value_error(1, "exponent is too large");
		}
		return FAILURE;
	}
	if (!bc_raise(n1, exponent, ret, *scale)) {
		zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Negative power of zero");
		return FAILURE;
	}
	bc_rm_trailing_zeros(*ret);
	if (scale_expand) {
		size_t diff = *scale - (*ret)->n_scale;
		*scale -= diff > BC_MATH_NUMBER_EXPAND_SCALE ? BC_MATH_NUMBER_EXPAND_SCALE : diff;
	}
	return SUCCESS;
}

static zend_always_inline bcmath_number_obj_t *bcmath_number_new_obj(bc_num ret, size_t scale)
{
	bcmath_number_obj_t *intern = get_bcmath_number_from_obj(bcmath_number_create(bcmath_number_ce));
	intern->num = ret;
	intern->scale = scale;
	return intern;
}

static zend_result bcmath_number_parse_num(zval *zv, zend_object **obj, zend_string **str, zend_long *lval)
{
	if (Z_TYPE_P(zv) == IS_OBJECT && instanceof_function(Z_OBJCE_P(zv), bcmath_number_ce)) {
		*obj = Z_OBJ_P(zv);
		return SUCCESS;
	} else {
		switch (Z_TYPE_P(zv)) {
			case IS_LONG:
				*lval = Z_LVAL_P(zv);
				return SUCCESS;

			case IS_STRING:
				*str = Z_STR_P(zv);
				return SUCCESS;

			case IS_NULL:
				*lval = 0;
				return FAILURE;

			default:
				return zend_parse_arg_long_slow(zv, lval, 1 /* dummy */) ? SUCCESS : FAILURE;
		}
	}
}

static zend_result bc_num_from_obj_or_str_or_long(
	bc_num *num, size_t *full_scale, const zend_object *obj, const zend_string *str, zend_long lval)
{
	if (obj) {
		bcmath_number_obj_t *intern = get_bcmath_number_from_obj(obj);
		*num = intern->num;
		if (full_scale) {
			*full_scale = intern->scale;
		}
		return SUCCESS;
	} else if (str) {
		return php_str2num_ex(num, str, full_scale);
	} else {
		php_long2num(num, lval);
		if (full_scale) {
			*full_scale = 0;
		}
		return SUCCESS;
	}
}

static zend_result bcmath_number_do_operation(uint8_t opcode, zval *ret_val, zval *op1, zval *op2)
{
	switch (opcode) {
		case ZEND_ADD:
		case ZEND_SUB:
		case ZEND_MUL:
		case ZEND_DIV:
		case ZEND_MOD:
		case ZEND_POW:
			break;
		default:
			return FAILURE;
	}

	zend_object *obj1 = NULL;
	zend_string *str1 = NULL;
	zend_long lval1 = 0;

	zend_object *obj2 = NULL;
	zend_string *str2 = NULL;
	zend_long lval2 = 0;

	if (UNEXPECTED(bcmath_number_parse_num(op1, &obj1, &str1, &lval1) == FAILURE || bcmath_number_parse_num(op2, &obj2, &str2, &lval2) == FAILURE)) {
		return FAILURE;
	}

	bc_num n1 = NULL;
	bc_num n2 = NULL;
	size_t n1_full_scale;
	size_t n2_full_scale;
	if (UNEXPECTED(bc_num_from_obj_or_str_or_long(&n1, &n1_full_scale, obj1, str1, lval1) == FAILURE)) {
		zend_value_error("Left string operand cannot be converted to BcMath\\Number");
		goto fail;
	}
	if (UNEXPECTED(bc_num_from_obj_or_str_or_long(&n2, &n2_full_scale, obj2, str2, lval2) == FAILURE)) {
		zend_value_error("Right string operand cannot be converted to BcMath\\Number");
		goto fail;
	}

	if (UNEXPECTED(CHECK_SCALE_OVERFLOW(n1_full_scale) || CHECK_SCALE_OVERFLOW(n2_full_scale))) {
		zend_value_error("scale must be between 0 and %d", INT_MAX);
		goto fail;
	}

	bc_num ret = NULL;
	size_t scale;
	switch (opcode) {
		case ZEND_ADD:
			bcmath_number_add_internal(n1, n2, &ret, n1_full_scale, n2_full_scale, &scale, true);
			break;
		case ZEND_SUB:
			bcmath_number_sub_internal(n1, n2, &ret, n1_full_scale, n2_full_scale, &scale, true);
			break;
		case ZEND_MUL:
			if (UNEXPECTED(bcmath_number_mul_internal(n1, n2, &ret, n1_full_scale, n2_full_scale, &scale, true) == FAILURE)) {
				goto fail;
			}
			break;
		case ZEND_DIV:
			if (UNEXPECTED(bcmath_number_div_internal(n1, n2, &ret, n1_full_scale, &scale, true) == FAILURE)) {
				goto fail;
			}
			break;
		case ZEND_MOD:
			if (UNEXPECTED(bcmath_number_mod_internal(n1, n2, &ret, n1_full_scale, n2_full_scale, &scale, true) == FAILURE)) {
				goto fail;
			}
			break;
		case ZEND_POW:
			if (UNEXPECTED(bcmath_number_pow_internal(n1, n2, &ret, n1_full_scale, &scale, true, true) == FAILURE)) {
				goto fail;
			}
			break;
		EMPTY_SWITCH_DEFAULT_CASE();
	}

	if (Z_TYPE_P(op1) != IS_OBJECT) {
		bc_free_num(&n1);
	}
	if (Z_TYPE_P(op2) != IS_OBJECT) {
		bc_free_num(&n2);
	}

	bcmath_number_obj_t *intern = bcmath_number_new_obj(ret, scale);

	/* For increment and decrement, etc */
	if (ret_val == op1) {
		zval_ptr_dtor(ret_val);
	}
	ZVAL_OBJ(ret_val, &intern->std);

	return SUCCESS;

fail:
	if (Z_TYPE_P(op1) != IS_OBJECT) {
		bc_free_num(&n1);
	}
	if (Z_TYPE_P(op2) != IS_OBJECT) {
		bc_free_num(&n2);
	}
	return FAILURE;
}

static int bcmath_number_compare(zval *op1, zval *op2)
{
	zend_object *obj1 = NULL;
	zend_string *str1 = NULL;
	zend_long lval1 = 0;

	zend_object *obj2 = NULL;
	zend_string *str2 = NULL;
	zend_long lval2 = 0;

	bc_num n1 = NULL;
	bc_num n2 = NULL;

	int ret = ZEND_UNCOMPARABLE;

	if (UNEXPECTED(bcmath_number_parse_num(op1, &obj1, &str1, &lval1) == FAILURE)) {
		goto failure;
	}

	if (UNEXPECTED(bcmath_number_parse_num(op2, &obj2, &str2, &lval2) == FAILURE)) {
		goto failure;
	}

	size_t n1_full_scale;
	size_t n2_full_scale;
	if (UNEXPECTED(bc_num_from_obj_or_str_or_long(&n1, &n1_full_scale, obj1, str1, lval1) == FAILURE ||
		bc_num_from_obj_or_str_or_long(&n2, &n2_full_scale, obj2, str2, lval2) == FAILURE)) {
		goto failure;
	}

	if (UNEXPECTED(CHECK_SCALE_OVERFLOW(n1_full_scale) || CHECK_SCALE_OVERFLOW(n2_full_scale))) {
		zend_value_error("scale must be between 0 and %d", INT_MAX);
		goto failure;
	}

	ret = bc_compare(n1, n2, MAX(n1->n_scale, n2->n_scale));

failure:
	if (Z_TYPE_P(op1) != IS_OBJECT) {
		bc_free_num(&n1);
	}
	if (Z_TYPE_P(op2) != IS_OBJECT) {
		bc_free_num(&n2);
	}

	return ret;
}

#define BCMATH_PARAM_NUMBER_OR_STR_OR_LONG(dest_obj, ce, dest_str, dest_long) \
	Z_PARAM_PROLOGUE(0, 0); \
	if (UNEXPECTED(!(zend_parse_arg_obj(_arg, &(dest_obj), ce, 0) || \
		zend_parse_arg_str_or_long(_arg, &(dest_str), &(dest_long), &_dummy, 0, _i)))) { \
		zend_argument_type_error(_i, "must be of type int, string, or %s, %s given", \
			ZSTR_VAL(bcmath_number_ce->name), zend_zval_value_name(_arg));             \
		_error_code = ZPP_ERROR_FAILURE; \
 		break; \
 	}

static zend_always_inline zend_result bc_num_from_obj_or_str_or_long_with_err(
	bc_num *num, size_t *scale, zend_object *obj, zend_string *str, zend_long lval, uint32_t arg_num)
{
	size_t full_scale = 0;
	if (UNEXPECTED(bc_num_from_obj_or_str_or_long(num, &full_scale, obj, str, lval) == FAILURE)) {
		zend_argument_value_error(arg_num, "is not well-formed");
		return FAILURE;
	}
	if (UNEXPECTED(CHECK_SCALE_OVERFLOW(full_scale))) {
		zend_argument_value_error(arg_num, "must be between 0 and %d", INT_MAX);
		return FAILURE;
	}
	if (scale != NULL) {
		*scale = full_scale;
	}
	return SUCCESS;
}

PHP_METHOD(BcMath_Number, __construct)
{
	zend_string *str = NULL;
	zend_long lval = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR_OR_LONG(str, lval);
	ZEND_PARSE_PARAMETERS_END();

	bcmath_number_obj_t *intern = get_bcmath_number_from_zval(ZEND_THIS);
	if (UNEXPECTED(intern->num != NULL)) {
		zend_readonly_property_modification_error_ex(ZSTR_VAL(bcmath_number_ce->name), "value");
		RETURN_THROWS();
	}

	bc_num num = NULL;
	size_t scale = 0;
	if (bc_num_from_obj_or_str_or_long_with_err(&num, &scale, NULL, str, lval, 1) == FAILURE) {
		bc_free_num(&num);
		RETURN_THROWS();
	}

	intern->num = num;
	intern->scale = scale;
}

static void bcmath_number_calc_method(INTERNAL_FUNCTION_PARAMETERS, uint8_t opcode)
{
	zend_object *num_obj = NULL;
	zend_string *num_str = NULL;
	zend_long num_lval = 0;
	zend_long scale_lval = 0;
	bool scale_is_null = true;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		BCMATH_PARAM_NUMBER_OR_STR_OR_LONG(num_obj, bcmath_number_ce, num_str, num_lval);
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_lval, scale_is_null);
	ZEND_PARSE_PARAMETERS_END();

	bc_num num = NULL;
	size_t num_full_scale = 0;
	if (bc_num_from_obj_or_str_or_long_with_err(&num, &num_full_scale, num_obj, num_str, num_lval, 1) == FAILURE) {
		goto fail;
	}
	if (bcmath_check_scale(scale_lval, 2) == FAILURE) {
		goto fail;
	}

	bc_num ret = NULL;
	size_t scale = scale_lval;
	bcmath_number_obj_t *intern = get_bcmath_number_from_zval(ZEND_THIS);

	switch (opcode) {
		case ZEND_ADD:
			bcmath_number_add_internal(intern->num, num, &ret, intern->scale, num_full_scale, &scale, scale_is_null);
			break;
		case ZEND_SUB:
			bcmath_number_sub_internal(intern->num, num, &ret, intern->scale, num_full_scale, &scale, scale_is_null);
			break;
		case ZEND_MUL:
			if (UNEXPECTED(bcmath_number_mul_internal(intern->num, num, &ret, intern->scale, num_full_scale, &scale, scale_is_null) == FAILURE)) {
				goto fail;
			}
			break;
		case ZEND_DIV:
			if (UNEXPECTED(bcmath_number_div_internal(intern->num, num, &ret, intern->scale, &scale, scale_is_null) == FAILURE)) {
				goto fail;
			}
			break;
		case ZEND_MOD:
			if (UNEXPECTED(bcmath_number_mod_internal(intern->num, num, &ret, intern->scale, num_full_scale, &scale, scale_is_null) == FAILURE)) {
				goto fail;
			}
			break;
		case ZEND_POW:
			if (UNEXPECTED(bcmath_number_pow_internal(intern->num, num, &ret, intern->scale, &scale, scale_is_null, false) == FAILURE)) {
				goto fail;
			}
			break;
		EMPTY_SWITCH_DEFAULT_CASE();
	}

	if (num_obj == NULL) {
		bc_free_num(&num);
	}

	bcmath_number_obj_t *new_intern = bcmath_number_new_obj(ret, scale);
	RETURN_OBJ(&new_intern->std);

fail:
	if (num_obj == NULL) {
		bc_free_num(&num);
	}
	RETURN_THROWS();
}

PHP_METHOD(BcMath_Number, add)
{
	bcmath_number_calc_method(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ADD);
}

PHP_METHOD(BcMath_Number, sub)
{
	bcmath_number_calc_method(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_SUB);
}

PHP_METHOD(BcMath_Number, mul)
{
	bcmath_number_calc_method(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_MUL);
}

PHP_METHOD(BcMath_Number, div)
{
	bcmath_number_calc_method(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_DIV);
}

PHP_METHOD(BcMath_Number, mod)
{
	bcmath_number_calc_method(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_MOD);
}

PHP_METHOD(BcMath_Number, pow)
{
	bcmath_number_calc_method(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_POW);
}

PHP_METHOD(BcMath_Number, divmod)
{
	zend_object *num_obj = NULL;
	zend_string *num_str = NULL;
	zend_long num_lval = 0;
	zend_long scale_lval = 0;
	bool scale_is_null = true;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		BCMATH_PARAM_NUMBER_OR_STR_OR_LONG(num_obj, bcmath_number_ce, num_str, num_lval);
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_lval, scale_is_null);
	ZEND_PARSE_PARAMETERS_END();

	bc_num num = NULL;
	size_t num_full_scale;
	if (bc_num_from_obj_or_str_or_long_with_err(&num, &num_full_scale, num_obj, num_str, num_lval, 1) == FAILURE) {
		goto fail;
	}
	if (bcmath_check_scale(scale_lval, 2) == FAILURE) {
		goto fail;
	}

	bc_num quot = NULL;
	bc_num rem = NULL;
	size_t scale = scale_lval;
	bcmath_number_obj_t *intern = get_bcmath_number_from_zval(ZEND_THIS);

	if (scale_is_null) {
		scale = MAX(intern->scale, num_full_scale);
	}

	if (!bc_divmod(intern->num, num, &quot, &rem, scale)) {
		zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Division by zero");
		goto fail;
	}
	bc_rm_trailing_zeros(quot);
	bc_rm_trailing_zeros(rem);

	if (num_obj == NULL) {
		bc_free_num(&num);
	}

	bcmath_number_obj_t *quot_intern = bcmath_number_new_obj(quot, 0);
	bcmath_number_obj_t *rem_intern = bcmath_number_new_obj(rem, scale);

	zval z_quot, z_rem;
	ZVAL_OBJ(&z_quot, &quot_intern->std);
	ZVAL_OBJ(&z_rem, &rem_intern->std);

	RETURN_ARR(zend_new_pair(&z_quot, &z_rem));

fail:
	if (num_obj == NULL) {
		bc_free_num(&num);
	}
	RETURN_THROWS();
}

PHP_METHOD(BcMath_Number, powmod)
{
	zend_object *exponent_obj = NULL;
	zend_string *exponent_str = NULL;
	zend_long exponent_lval = 0;

	zend_object *modulus_obj = NULL;
	zend_string *modulus_str = NULL;
	zend_long modulus_lval = 0;

	zend_long scale_lval = 0;
	bool scale_is_null = true;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		BCMATH_PARAM_NUMBER_OR_STR_OR_LONG(exponent_obj, bcmath_number_ce, exponent_str, exponent_lval);
		BCMATH_PARAM_NUMBER_OR_STR_OR_LONG(modulus_obj, bcmath_number_ce, modulus_str, modulus_lval);
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_lval, scale_is_null);
	ZEND_PARSE_PARAMETERS_END();

	bc_num exponent_num = NULL;
	bc_num modulus_num = NULL;
	if (bc_num_from_obj_or_str_or_long_with_err(&exponent_num, NULL, exponent_obj, exponent_str, exponent_lval, 1) == FAILURE) {
		goto cleanup;
	}
	if (bc_num_from_obj_or_str_or_long_with_err(&modulus_num, NULL, modulus_obj, modulus_str, modulus_lval, 2) == FAILURE) {
		goto cleanup;
	}
	if (bcmath_check_scale(scale_lval, 3) == FAILURE) {
		goto cleanup;
	}

	bcmath_number_obj_t *intern = get_bcmath_number_from_zval(ZEND_THIS);
	bc_num ret = NULL;
	size_t scale = scale_lval;
	raise_mod_status status = bc_raisemod(intern->num, exponent_num, modulus_num, &ret, scale);
	switch (status) {
		case BASE_HAS_FRACTIONAL:
			zend_value_error("Base number cannot have a fractional part");
			goto cleanup;
		case EXPO_HAS_FRACTIONAL:
			zend_argument_value_error(1, "cannot have a fractional part");
			goto cleanup;
		case EXPO_IS_NEGATIVE:
			zend_argument_value_error(1, "must be greater than or equal to 0");
			goto cleanup;
		case MOD_HAS_FRACTIONAL:
			zend_argument_value_error(2, "cannot have a fractional part");
			goto cleanup;
		case MOD_IS_ZERO:
			zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Modulo by zero");
			goto cleanup;
		case OK:
			break;
		EMPTY_SWITCH_DEFAULT_CASE();
	}

	bc_rm_trailing_zeros(ret);

	if (exponent_obj == NULL) {
		bc_free_num(&exponent_num);
	}
	if (modulus_obj == NULL) {
		bc_free_num(&modulus_num);
	}

	bcmath_number_obj_t *new_intern = bcmath_number_new_obj(ret, scale);
	RETURN_OBJ(&new_intern->std);

cleanup:
	if (exponent_obj == NULL) {
		bc_free_num(&exponent_num);
	}
	if (modulus_obj == NULL) {
		bc_free_num(&modulus_num);
	}
	RETURN_THROWS();
}

PHP_METHOD(BcMath_Number, sqrt)
{
	zend_long scale_lval = 0;
	bool scale_is_null = true;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_lval, scale_is_null);
	ZEND_PARSE_PARAMETERS_END();

	if (bcmath_check_scale(scale_lval, 1) == FAILURE) {
		RETURN_THROWS();
	}

	bcmath_number_obj_t *intern = get_bcmath_number_from_zval(ZEND_THIS);

	size_t scale;
	if (scale_is_null) {
		scale = intern->scale + BC_MATH_NUMBER_EXPAND_SCALE;
		if (UNEXPECTED(CHECK_RET_SCALE_OVERFLOW(scale, intern->scale))) {
			zend_value_error("scale of the result is too large");
			RETURN_THROWS();
		}
	} else {
		scale = scale_lval;
	}

	bc_num ret = bc_copy_num(intern->num);
	if (!bc_sqrt (&ret, scale)) {
		zend_value_error("Base number must be greater than or equal to 0");
		bc_free_num(&ret);
		RETURN_THROWS();
	}

	ret->n_scale = MIN(scale, ret->n_scale);
	bc_rm_trailing_zeros(ret);
	if (scale_is_null) {
		size_t diff = scale - ret->n_scale;
		scale -= diff > BC_MATH_NUMBER_EXPAND_SCALE ? BC_MATH_NUMBER_EXPAND_SCALE : diff;
	}

	bcmath_number_obj_t *new_intern = bcmath_number_new_obj(ret, scale);
	RETURN_OBJ(&new_intern->std);
}

PHP_METHOD(BcMath_Number, compare)
{
	zend_object *num_obj = NULL;
	zend_string *num_str = NULL;
	zend_long num_lval = 0;
	zend_long scale_lval = 0;
	bool scale_is_null = true;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		BCMATH_PARAM_NUMBER_OR_STR_OR_LONG(num_obj, bcmath_number_ce, num_str, num_lval);
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_lval, scale_is_null);
	ZEND_PARSE_PARAMETERS_END();

	bc_num num = NULL;
	size_t num_full_scale = 0;
	if (bc_num_from_obj_or_str_or_long_with_err(&num, &num_full_scale, num_obj, num_str, num_lval, 1) == FAILURE) {
		goto fail;
	}
	if (bcmath_check_scale(scale_lval, 2) == FAILURE) {
		goto fail;
	}

	size_t scale;
	bcmath_number_obj_t *intern = get_bcmath_number_from_zval(ZEND_THIS);
	if (scale_is_null) {
		scale = MAX(intern->num->n_scale, num->n_scale);
	} else {
		scale = scale_lval;
	}
	zend_long ret = bc_compare(intern->num, num, scale);

	if (num_obj == NULL) {
		bc_free_num(&num);
	}
	RETURN_LONG(ret);

fail:
	if (num_obj == NULL) {
		bc_free_num(&num);
	}
	RETURN_THROWS();
}

static void bcmath_number_floor_or_ceil(INTERNAL_FUNCTION_PARAMETERS, bool is_floor)
{
	ZEND_PARSE_PARAMETERS_NONE();

	bcmath_number_obj_t *intern = get_bcmath_number_from_zval(ZEND_THIS);

	bc_num ret = bc_floor_or_ceil(intern->num, is_floor);

	bcmath_number_obj_t *new_intern = bcmath_number_new_obj(ret, 0);
	RETURN_OBJ(&new_intern->std);
}

PHP_METHOD(BcMath_Number, floor)
{
	bcmath_number_floor_or_ceil(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

PHP_METHOD(BcMath_Number, ceil)
{
	bcmath_number_floor_or_ceil(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

PHP_METHOD(BcMath_Number, round)
{
	zend_long precision = 0;
	zend_long rounding_mode = PHP_ROUND_HALF_UP;
	zend_object *mode_object = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(precision);
		Z_PARAM_OBJ_OF_CLASS(mode_object, rounding_mode_ce);
	ZEND_PARSE_PARAMETERS_END();

	if (mode_object != NULL) {
		rounding_mode = php_math_round_mode_from_enum(mode_object);
	}

	switch (rounding_mode) {
		case PHP_ROUND_HALF_UP:
		case PHP_ROUND_HALF_DOWN:
		case PHP_ROUND_HALF_EVEN:
		case PHP_ROUND_HALF_ODD:
		case PHP_ROUND_CEILING:
		case PHP_ROUND_FLOOR:
		case PHP_ROUND_TOWARD_ZERO:
		case PHP_ROUND_AWAY_FROM_ZERO:
			break;
		default:
			zend_argument_value_error(2, "is an unsupported rounding mode");
			RETURN_THROWS();
	}

	bcmath_number_obj_t *intern = get_bcmath_number_from_zval(ZEND_THIS);

	bc_num ret = NULL;
	bc_round(intern->num, precision, rounding_mode, &ret);

	bc_rm_trailing_zeros(ret);

	bcmath_number_obj_t *new_intern = bcmath_number_new_obj(ret, ret->n_scale);
	RETURN_OBJ(&new_intern->std);
}

PHP_METHOD(BcMath_Number, __toString)
{
	ZEND_PARSE_PARAMETERS_NONE();
	bcmath_number_obj_t *intern = get_bcmath_number_from_zval(ZEND_THIS);
	RETURN_STR_COPY(bcmath_number_value_to_str(intern));
}

PHP_METHOD(BcMath_Number, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	bcmath_number_obj_t *intern = get_bcmath_number_from_zval(ZEND_THIS);

	array_init(return_value);
	HashTable *props = Z_ARRVAL_P(return_value);

	zval zv;
	ZVAL_STR_COPY(&zv, bcmath_number_value_to_str(intern));
	zend_hash_update(props, ZSTR_KNOWN(ZEND_STR_VALUE), &zv);
}

PHP_METHOD(BcMath_Number, __unserialize)
{
	HashTable *props;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(props)
	ZEND_PARSE_PARAMETERS_END();

	zval *zv = zend_hash_find(props, ZSTR_KNOWN(ZEND_STR_VALUE));
	if (!zv || Z_TYPE_P(zv) != IS_STRING || Z_STRLEN_P(zv) == 0) {
		goto fail;
	}

	bcmath_number_obj_t *intern = get_bcmath_number_from_zval(ZEND_THIS);
	if (UNEXPECTED(intern->num != NULL)) {
		zend_readonly_property_modification_error_ex(ZSTR_VAL(bcmath_number_ce->name), "value");
		RETURN_THROWS();
	}

	bc_num num = NULL;
	size_t scale = 0;
	if (php_str2num_ex(&num, Z_STR_P(zv), &scale) == FAILURE || CHECK_SCALE_OVERFLOW(scale)) {
		bc_free_num(&num);
		goto fail;
	}

	intern->num = num;
	intern->scale = scale;

	return;

fail:
	zend_throw_exception_ex(NULL, 0, "Invalid serialization data for %s object", ZSTR_VAL(bcmath_number_ce->name));
	RETURN_THROWS();
}

#endif
