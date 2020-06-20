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

	tmp = zend_atol(ZSTR_VAL(new_value), ZSTR_LEN(new_value));
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

/* {{{ PHP_GINIT_FUNCTION
 */
static PHP_GINIT_FUNCTION(bcmath)
{
#if defined(COMPILE_DL_BCMATH) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	bcmath_globals->bc_precision = 0;
	bc_init_numbers();
}
/* }}} */

/* {{{ PHP_GSHUTDOWN_FUNCTION
 */
static PHP_GSHUTDOWN_FUNCTION(bcmath)
{
	_bc_free_num_ex(&bcmath_globals->_zero_, 1);
	_bc_free_num_ex(&bcmath_globals->_one_, 1);
	_bc_free_num_ex(&bcmath_globals->_two_, 1);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(bcmath)
{
	REGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(bcmath)
{
	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
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
static void php_str2num(bc_num *num, char *str)
{
	char *p;

	if (!(p = strchr(str, '.'))) {
		if (!bc_str2num(num, str, 0)) {
			php_error_docref(NULL, E_WARNING, "bcmath function argument is not well-formed");
		}
		return;
	}

	if (!bc_str2num(num, str, strlen(p+1))) {
		php_error_docref(NULL, E_WARNING, "bcmath function argument is not well-formed");
	}
}
/* }}} */

/* {{{ proto string bcadd(string left_operand, string right_operand [, int scale])
   Returns the sum of two arbitrary precision numbers */
PHP_FUNCTION(bcadd)
{
	zend_string *left, *right;
	zend_long scale_param;
	zend_bool scale_param_is_null = 1;
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
	php_str2num(&first, ZSTR_VAL(left));
	php_str2num(&second, ZSTR_VAL(right));
	bc_add (first, second, &result, scale);

	RETVAL_STR(bc_num2str_ex(result, scale));
	bc_free_num(&first);
	bc_free_num(&second);
	bc_free_num(&result);
	return;
}
/* }}} */

/* {{{ proto string bcsub(string left_operand, string right_operand [, int scale])
   Returns the difference between two arbitrary precision numbers */
PHP_FUNCTION(bcsub)
{
	zend_string *left, *right;
	zend_long scale_param;
	zend_bool scale_param_is_null = 1;
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
	php_str2num(&first, ZSTR_VAL(left));
	php_str2num(&second, ZSTR_VAL(right));
	bc_sub (first, second, &result, scale);

	RETVAL_STR(bc_num2str_ex(result, scale));
	bc_free_num(&first);
	bc_free_num(&second);
	bc_free_num(&result);
	return;
}
/* }}} */

/* {{{ proto string bcmul(string left_operand, string right_operand [, int scale])
   Returns the multiplication of two arbitrary precision numbers */
PHP_FUNCTION(bcmul)
{
	zend_string *left, *right;
	zend_long scale_param;
	zend_bool scale_param_is_null = 1;
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
	php_str2num(&first, ZSTR_VAL(left));
	php_str2num(&second, ZSTR_VAL(right));
	bc_multiply (first, second, &result, scale);

	RETVAL_STR(bc_num2str_ex(result, scale));
	bc_free_num(&first);
	bc_free_num(&second);
	bc_free_num(&result);
	return;
}
/* }}} */

/* {{{ proto string bcdiv(string left_operand, string right_operand [, int scale])
   Returns the quotient of two arbitrary precision numbers (division) */
PHP_FUNCTION(bcdiv)
{
	zend_string *left, *right;
	zend_long scale_param;
	zend_bool scale_param_is_null = 1;
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
	php_str2num(&first, ZSTR_VAL(left));
	php_str2num(&second, ZSTR_VAL(right));

	switch (bc_divide(first, second, &result, scale)) {
		case 0: /* OK */
			RETVAL_STR(bc_num2str_ex(result, scale));
			break;
		case -1: /* division by zero */
			zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Division by zero");
			break;
	}

	bc_free_num(&first);
	bc_free_num(&second);
	bc_free_num(&result);
	return;
}
/* }}} */

/* {{{ proto string bcmod(string left_operand, string right_operand [, int scale])
   Returns the modulus of the two arbitrary precision operands */
PHP_FUNCTION(bcmod)
{
	zend_string *left, *right;
	zend_long scale_param;
	zend_bool scale_param_is_null = 1;
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
	php_str2num(&first, ZSTR_VAL(left));
	php_str2num(&second, ZSTR_VAL(right));

	switch (bc_modulo(first, second, &result, scale)) {
		case 0:
			RETVAL_STR(bc_num2str_ex(result, scale));
			break;
		case -1:
			zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Modulo by zero");
			break;
	}

	bc_free_num(&first);
	bc_free_num(&second);
	bc_free_num(&result);
}
/* }}} */

/* {{{ proto string bcpowmod(string x, string y, string mod [, int scale])
   Returns the value of an arbitrary precision number raised to the power of another reduced by a modulus */
PHP_FUNCTION(bcpowmod)
{
	zend_string *left, *right, *modulus;
	zend_long scale_param;
	zend_bool scale_param_is_null = 1;
	bc_num first, second, mod, result;
	int scale = BCG(bc_precision);

	ZEND_PARSE_PARAMETERS_START(3, 4)
		Z_PARAM_STR(left)
		Z_PARAM_STR(right)
		Z_PARAM_STR(modulus)
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

	bc_init_num(&first);
	bc_init_num(&second);
	bc_init_num(&mod);
	bc_init_num(&result);
	php_str2num(&first, ZSTR_VAL(left));
	php_str2num(&second, ZSTR_VAL(right));
	php_str2num(&mod, ZSTR_VAL(modulus));

	switch (bc_raisemod(first, second, mod, &result, scale)) {
		case 0:
			RETVAL_STR(bc_num2str_ex(result, scale));
			break;
		case -1:
			zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Modulo by zero");
			break;
		case -2:
			zend_argument_value_error(2, "must be greater than 0");
			break;
	}

	bc_free_num(&first);
	bc_free_num(&second);
	bc_free_num(&mod);
	bc_free_num(&result);
}
/* }}} */

/* {{{ proto string bcpow(string x, string y [, int scale])
   Returns the value of an arbitrary precision number raised to the power of another */
PHP_FUNCTION(bcpow)
{
	zend_string *left, *right;
	zend_long scale_param;
	zend_bool scale_param_is_null = 1;
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
	php_str2num(&first, ZSTR_VAL(left));
	php_str2num(&second, ZSTR_VAL(right));
	bc_raise (first, second, &result, scale);

	RETVAL_STR(bc_num2str_ex(result, scale));
	bc_free_num(&first);
	bc_free_num(&second);
	bc_free_num(&result);
}
/* }}} */

/* {{{ proto string bcsqrt(string operand [, int scale])
   Returns the square root of an arbitrary precision number */
PHP_FUNCTION(bcsqrt)
{
	zend_string *left;
	zend_long scale_param;
	zend_bool scale_param_is_null = 1;
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
	php_str2num(&result, ZSTR_VAL(left));

	if (bc_sqrt (&result, scale) != 0) {
		RETVAL_STR(bc_num2str_ex(result, scale));
	} else {
		zend_value_error("Square root of negative number");
	}

	bc_free_num(&result);
	return;
}
/* }}} */

/* {{{ proto int bccomp(string left_operand, string right_operand [, int scale])
   Compares two arbitrary precision numbers */
PHP_FUNCTION(bccomp)
{
	zend_string *left, *right;
	zend_long scale_param;
	zend_bool scale_param_is_null = 1;
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
		php_error_docref(NULL, E_WARNING, "bcmath function argument is not well-formed");
	}
	if (!bc_str2num(&second, ZSTR_VAL(right), scale)) {
		php_error_docref(NULL, E_WARNING, "bcmath function argument is not well-formed");
	}
	RETVAL_LONG(bc_compare(first, second));

	bc_free_num(&first);
	bc_free_num(&second);
	return;
}
/* }}} */

/* {{{ proto int bcscale([int scale])
   Sets default scale parameter for all bc math functions */
PHP_FUNCTION(bcscale)
{
	zend_long old_scale, new_scale;
	zend_bool new_scale_is_null = 1;

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

		BCG(bc_precision) = (int) new_scale;
	}

	RETURN_LONG(old_scale);
}
/* }}} */


#endif
