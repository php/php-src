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
#include "config.h"
#endif

#include "php.h"

#ifdef HAVE_BCMATH

#include "php_ini.h"
#include "zend_exceptions.h"
#include "bcmath_arginfo.h"
#include "ext/standard/info.h"
#include "php_bcmath.h"
#include "libbcmath/src/bcmath.h"

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
	bc_init_numbers();
}
/* }}} */

/* {{{ PHP_GSHUTDOWN_FUNCTION */
static PHP_GSHUTDOWN_FUNCTION(bcmath)
{
	_bc_free_num_ex(&bcmath_globals->_zero_, 1);
	_bc_free_num_ex(&bcmath_globals->_one_, 1);
	_bc_free_num_ex(&bcmath_globals->_two_, 1);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(bcmath)
{
	REGISTER_INI_ENTRIES();

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

/* {{{ php_str2num
   Convert to bc_num detecting scale */
static zend_result php_str2num(bc_num *num, char *str)
{
	char *p;

	if (!(p = strchr(str, '.'))) {
		if (!bc_str2num(num, str, 0)) {
			return FAILURE;
		}

		return SUCCESS;
	}

	if (!bc_str2num(num, str, strlen(p+1))) {
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

/* {{{ Returns the sum of two arbitrary precision numbers */
PHP_FUNCTION(bcadd)
{
	zend_string *left, *right;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num first, second, result;
	int scale;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(left)
		Z_PARAM_STR(right)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_param, scale_param_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (scale_param_is_null) {
		scale = BCG(bc_precision);
	} else if (scale_param < 0 || scale_param > INT_MAX) {
		zend_argument_value_error(3, "must be between 0 and %d", INT_MAX);
		RETURN_THROWS();
	} else {
		scale = (int) scale_param;
	}

	bc_init_num(&first);
	bc_init_num(&second);
	bc_init_num(&result);

	if (php_str2num(&first, ZSTR_VAL(left)) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&second, ZSTR_VAL(right)) == FAILURE) {
		zend_argument_value_error(2, "is not well-formed");
		goto cleanup;
	}

	bc_add (first, second, &result, scale);

	RETVAL_STR(bc_num2str_ex(result, scale));

	cleanup: {
		bc_free_num(&first);
		bc_free_num(&second);
		bc_free_num(&result);
	};
}
/* }}} */

/* {{{ Returns the difference between two arbitrary precision numbers */
PHP_FUNCTION(bcsub)
{
	zend_string *left, *right;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num first, second, result;
	int scale;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(left)
		Z_PARAM_STR(right)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_param, scale_param_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (scale_param_is_null) {
		scale = BCG(bc_precision);
	} else if (scale_param < 0 || scale_param > INT_MAX) {
		zend_argument_value_error(3, "must be between 0 and %d", INT_MAX);
		RETURN_THROWS();
	} else {
		scale = (int) scale_param;
	}

	bc_init_num(&first);
	bc_init_num(&second);
	bc_init_num(&result);

	if (php_str2num(&first, ZSTR_VAL(left)) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&second, ZSTR_VAL(right)) == FAILURE) {
		zend_argument_value_error(2, "is not well-formed");
		goto cleanup;
	}

	bc_sub (first, second, &result, scale);

	RETVAL_STR(bc_num2str_ex(result, scale));

	cleanup: {
		bc_free_num(&first);
		bc_free_num(&second);
		bc_free_num(&result);
	};
}
/* }}} */

/* {{{ Returns the multiplication of two arbitrary precision numbers */
PHP_FUNCTION(bcmul)
{
	zend_string *left, *right;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num first, second, result;
	int scale;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(left)
		Z_PARAM_STR(right)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_param, scale_param_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (scale_param_is_null) {
		scale = BCG(bc_precision);
	} else if (scale_param < 0 || scale_param > INT_MAX) {
		zend_argument_value_error(3, "must be between 0 and %d", INT_MAX);
		RETURN_THROWS();
	} else {
		scale = (int) scale_param;
	}

	bc_init_num(&first);
	bc_init_num(&second);
	bc_init_num(&result);

	if (php_str2num(&first, ZSTR_VAL(left)) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&second, ZSTR_VAL(right)) == FAILURE) {
		zend_argument_value_error(2, "is not well-formed");
		goto cleanup;
	}

	bc_multiply (first, second, &result, scale);

	RETVAL_STR(bc_num2str_ex(result, scale));

	cleanup: {
		bc_free_num(&first);
		bc_free_num(&second);
		bc_free_num(&result);
	};
}
/* }}} */

/* {{{ Returns the quotient of two arbitrary precision numbers (division) */
PHP_FUNCTION(bcdiv)
{
	zend_string *left, *right;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num first, second, result;
	int scale = BCG(bc_precision);

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(left)
		Z_PARAM_STR(right)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_param, scale_param_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (scale_param_is_null) {
		scale = BCG(bc_precision);
	} else if (scale_param < 0 || scale_param > INT_MAX) {
		zend_argument_value_error(3, "must be between 0 and %d", INT_MAX);
		RETURN_THROWS();
	} else {
		scale = (int) scale_param;
	}

	bc_init_num(&first);
	bc_init_num(&second);
	bc_init_num(&result);

	if (php_str2num(&first, ZSTR_VAL(left)) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&second, ZSTR_VAL(right)) == FAILURE) {
		zend_argument_value_error(2, "is not well-formed");
		goto cleanup;
	}

	if (!bc_divide(first, second, &result, scale)) {
		zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Division by zero");
		goto cleanup;
	}

	RETVAL_STR(bc_num2str_ex(result, scale));

	cleanup: {
		bc_free_num(&first);
		bc_free_num(&second);
		bc_free_num(&result);
	};
}
/* }}} */

/* {{{ Returns the modulus of the two arbitrary precision operands */
PHP_FUNCTION(bcmod)
{
	zend_string *left, *right;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num first, second, result;
	int scale = BCG(bc_precision);

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(left)
		Z_PARAM_STR(right)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_param, scale_param_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (scale_param_is_null) {
		scale = BCG(bc_precision);
	} else if (scale_param < 0 || scale_param > INT_MAX) {
		zend_argument_value_error(3, "must be between 0 and %d", INT_MAX);
		RETURN_THROWS();
	} else {
		scale = (int) scale_param;
	}

	bc_init_num(&first);
	bc_init_num(&second);
	bc_init_num(&result);

	if (php_str2num(&first, ZSTR_VAL(left)) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&second, ZSTR_VAL(right)) == FAILURE) {
		zend_argument_value_error(2, "is not well-formed");
		goto cleanup;
	}

	if (!bc_modulo(first, second, &result, scale)) {
		zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Modulo by zero");
		goto cleanup;
	}

	RETVAL_STR(bc_num2str_ex(result, scale));

	cleanup: {
		bc_free_num(&first);
		bc_free_num(&second);
		bc_free_num(&result);
	};
}
/* }}} */

/* {{{ Returns the value of an arbitrary precision number raised to the power of another reduced by a modulus */
PHP_FUNCTION(bcpowmod)
{
	zend_string *base_str, *exponent_str, *modulus_str;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num bc_base, bc_expo, bc_modulus, result;
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
	} else if (scale_param < 0 || scale_param > INT_MAX) {
		zend_argument_value_error(4, "must be between 0 and %d", INT_MAX);
		RETURN_THROWS();
	} else {
		scale = (int) scale_param;
	}

	bc_init_num(&bc_base);
	bc_init_num(&bc_expo);
	bc_init_num(&bc_modulus);
	bc_init_num(&result);

	if (php_str2num(&bc_base, ZSTR_VAL(base_str)) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&bc_expo, ZSTR_VAL(exponent_str)) == FAILURE) {
		zend_argument_value_error(2, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&bc_modulus, ZSTR_VAL(modulus_str)) == FAILURE) {
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
			RETVAL_STR(bc_num2str_ex(result, scale));
			break;
		EMPTY_SWITCH_DEFAULT_CASE();
	}

	cleanup: {
		bc_free_num(&bc_base);
		bc_free_num(&bc_expo);
		bc_free_num(&bc_modulus);
		bc_free_num(&result);
	};
}
/* }}} */

/* {{{ Returns the value of an arbitrary precision number raised to the power of another */
PHP_FUNCTION(bcpow)
{
	zend_string *base_str, *exponent_str;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num first, bc_exponent, result;
	int scale = BCG(bc_precision);

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(base_str)
		Z_PARAM_STR(exponent_str)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_param, scale_param_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (scale_param_is_null) {
		scale = BCG(bc_precision);
	} else if (scale_param < 0 || scale_param > INT_MAX) {
		zend_argument_value_error(3, "must be between 0 and %d", INT_MAX);
		RETURN_THROWS();
	} else {
		scale = (int) scale_param;
	}

	bc_init_num(&first);
	bc_init_num(&bc_exponent);
	bc_init_num(&result);

	if (php_str2num(&first, ZSTR_VAL(base_str)) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&bc_exponent, ZSTR_VAL(exponent_str)) == FAILURE) {
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

	bc_raise(first, exponent, &result, scale);

	RETVAL_STR(bc_num2str_ex(result, scale));

	cleanup: {
		bc_free_num(&first);
		bc_free_num(&bc_exponent);
		bc_free_num(&result);
	};
}
/* }}} */

/* {{{ Returns the square root of an arbitrary precision number */
PHP_FUNCTION(bcsqrt)
{
	zend_string *left;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num result;
	int scale = BCG(bc_precision);

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(left)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_param, scale_param_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (scale_param_is_null) {
		scale = BCG(bc_precision);
	} else if (scale_param < 0 || scale_param > INT_MAX) {
		zend_argument_value_error(2, "must be between 0 and %d", INT_MAX);
		RETURN_THROWS();
	} else {
		scale = (int) scale_param;
	}

	bc_init_num(&result);

	if (php_str2num(&result, ZSTR_VAL(left)) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (bc_sqrt (&result, scale) != 0) {
		RETVAL_STR(bc_num2str_ex(result, scale));
	} else {
		zend_argument_value_error(1, "must be greater than or equal to 0");
	}

	cleanup: {
		bc_free_num(&result);
	};
}
/* }}} */

/* {{{ Compares two arbitrary precision numbers */
PHP_FUNCTION(bccomp)
{
	zend_string *left, *right;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num first, second;
	int scale = BCG(bc_precision);

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(left)
		Z_PARAM_STR(right)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_param, scale_param_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (scale_param_is_null) {
		scale = BCG(bc_precision);
	} else if (scale_param < 0 || scale_param > INT_MAX) {
		zend_argument_value_error(3, "must be between 0 and %d", INT_MAX);
		RETURN_THROWS();
	} else {
		scale = (int) scale_param;
	}

	bc_init_num(&first);
	bc_init_num(&second);

	if (!bc_str2num(&first, ZSTR_VAL(left), scale)) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (!bc_str2num(&second, ZSTR_VAL(right), scale)) {
		zend_argument_value_error(2, "is not well-formed");
		goto cleanup;
	}

	RETVAL_LONG(bc_compare(first, second));

	cleanup: {
		bc_free_num(&first);
		bc_free_num(&second);
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
		if (new_scale < 0 || new_scale > INT_MAX) {
			zend_argument_value_error(1, "must be between 0 and %d", INT_MAX);
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


#endif
