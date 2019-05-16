/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
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

#if HAVE_BCMATH

#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_bcmath.h"
#include "libbcmath/src/bcmath.h"

ZEND_DECLARE_MODULE_GLOBALS(bcmath)
static PHP_GINIT_FUNCTION(bcmath);
static PHP_GSHUTDOWN_FUNCTION(bcmath);

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_bcadd, 0, 0, 2)
	ZEND_ARG_INFO(0, left_operand)
	ZEND_ARG_INFO(0, right_operand)
	ZEND_ARG_INFO(0, scale)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bcsub, 0, 0, 2)
	ZEND_ARG_INFO(0, left_operand)
	ZEND_ARG_INFO(0, right_operand)
	ZEND_ARG_INFO(0, scale)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bcmul, 0, 0, 2)
	ZEND_ARG_INFO(0, left_operand)
	ZEND_ARG_INFO(0, right_operand)
	ZEND_ARG_INFO(0, scale)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bcdiv, 0, 0, 2)
	ZEND_ARG_INFO(0, left_operand)
	ZEND_ARG_INFO(0, right_operand)
	ZEND_ARG_INFO(0, scale)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bcmod, 0, 0, 2)
	ZEND_ARG_INFO(0, left_operand)
	ZEND_ARG_INFO(0, right_operand)
	ZEND_ARG_INFO(0, scale)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bcpowmod, 0, 0, 3)
	ZEND_ARG_INFO(0, x)
	ZEND_ARG_INFO(0, y)
	ZEND_ARG_INFO(0, mod)
	ZEND_ARG_INFO(0, scale)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bcpow, 0, 0, 2)
	ZEND_ARG_INFO(0, x)
	ZEND_ARG_INFO(0, y)
	ZEND_ARG_INFO(0, scale)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bcsqrt, 0, 0, 1)
	ZEND_ARG_INFO(0, operand)
	ZEND_ARG_INFO(0, scale)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bccomp, 0, 0, 2)
	ZEND_ARG_INFO(0, left_operand)
	ZEND_ARG_INFO(0, right_operand)
	ZEND_ARG_INFO(0, scale)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bcscale, 0, 0, 0)
	ZEND_ARG_INFO(0, scale)
ZEND_END_ARG_INFO()

/* }}} */

static const zend_function_entry bcmath_functions[] = {
	PHP_FE(bcadd,									arginfo_bcadd)
	PHP_FE(bcsub,									arginfo_bcsub)
	PHP_FE(bcmul,									arginfo_bcmul)
	PHP_FE(bcdiv,									arginfo_bcdiv)
	PHP_FE(bcmod,									arginfo_bcmod)
	PHP_FE(bcpow,									arginfo_bcpow)
	PHP_FE(bcsqrt,									arginfo_bcsqrt)
	PHP_FE(bcscale,									arginfo_bcscale)
	PHP_FE(bccomp,									arginfo_bccomp)
	PHP_FE(bcpowmod,								arginfo_bcpowmod)
	PHP_FE_END
};

zend_module_entry bcmath_module_entry = {
	STANDARD_MODULE_HEADER,
	"bcmath",
	bcmath_functions,
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

/* {{{ PHP_INI */
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("bcmath.scale", "0", PHP_INI_ALL, OnUpdateLongGEZero, bc_precision, zend_bcmath_globals, bcmath_globals)
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
	zend_long scale_param = 0;
	bc_num first, second, result;
	int scale = (int)BCG(bc_precision);

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(left)
		Z_PARAM_STR(right)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(scale_param)
	ZEND_PARSE_PARAMETERS_END();

	if (ZEND_NUM_ARGS() == 3) {
		scale = (int) (scale_param < 0 ? 0 : scale_param);
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
	zend_long scale_param = 0;
	bc_num first, second, result;
	int scale = (int)BCG(bc_precision);

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(left)
		Z_PARAM_STR(right)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(scale_param)
	ZEND_PARSE_PARAMETERS_END();

	if (ZEND_NUM_ARGS() == 3) {
		scale = (int) ((int)scale_param < 0 ? 0 : scale_param);
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
	zend_long scale_param = 0;
	bc_num first, second, result;
	int scale = (int)BCG(bc_precision);

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(left)
		Z_PARAM_STR(right)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(scale_param)
	ZEND_PARSE_PARAMETERS_END();

	if (ZEND_NUM_ARGS() == 3) {
		scale = (int) ((int)scale_param < 0 ? 0 : scale_param);
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
	zend_long scale_param = 0;
	bc_num first, second, result;
	int scale = (int)BCG(bc_precision);

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(left)
		Z_PARAM_STR(right)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(scale_param)
	ZEND_PARSE_PARAMETERS_END();

	if (ZEND_NUM_ARGS() == 3) {
		scale = (int) ((int)scale_param < 0 ? 0 : scale_param);
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
			php_error_docref(NULL, E_WARNING, "Division by zero");
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
	zend_long scale_param = 0;
	bc_num first, second, result;
	int scale = (int)BCG(bc_precision);

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(left)
		Z_PARAM_STR(right)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(scale_param)
	ZEND_PARSE_PARAMETERS_END();

	if (ZEND_NUM_ARGS() == 3) {
		scale = (int) ((int)scale_param < 0 ? 0 : scale_param);
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
			php_error_docref(NULL, E_WARNING, "Division by zero");
			break;
	}

	bc_free_num(&first);
	bc_free_num(&second);
	bc_free_num(&result);
	return;
}
/* }}} */

/* {{{ proto string bcpowmod(string x, string y, string mod [, int scale])
   Returns the value of an arbitrary precision number raised to the power of another reduced by a modulous */
PHP_FUNCTION(bcpowmod)
{
	zend_string *left, *right, *modulous;
	bc_num first, second, mod, result;
	zend_long scale = BCG(bc_precision);
	int scale_int;

	ZEND_PARSE_PARAMETERS_START(3, 4)
		Z_PARAM_STR(left)
		Z_PARAM_STR(right)
		Z_PARAM_STR(modulous)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(scale)
	ZEND_PARSE_PARAMETERS_END();

	bc_init_num(&first);
	bc_init_num(&second);
	bc_init_num(&mod);
	bc_init_num(&result);
	php_str2num(&first, ZSTR_VAL(left));
	php_str2num(&second, ZSTR_VAL(right));
	php_str2num(&mod, ZSTR_VAL(modulous));

	scale_int = (int) ((int)scale < 0 ? 0 : scale);

	if (bc_raisemod(first, second, mod, &result, scale_int) != -1) {
		RETVAL_STR(bc_num2str_ex(result, scale_int));
	} else {
		RETVAL_FALSE;
	}

	bc_free_num(&first);
	bc_free_num(&second);
	bc_free_num(&mod);
	bc_free_num(&result);
	return;
}
/* }}} */

/* {{{ proto string bcpow(string x, string y [, int scale])
   Returns the value of an arbitrary precision number raised to the power of another */
PHP_FUNCTION(bcpow)
{
	zend_string *left, *right;
	zend_long scale_param = 0;
	bc_num first, second, result;
	int scale = (int)BCG(bc_precision);

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(left)
		Z_PARAM_STR(right)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(scale_param)
	ZEND_PARSE_PARAMETERS_END();

	if (ZEND_NUM_ARGS() == 3) {
		scale = (int) ((int)scale_param < 0 ? 0 : scale_param);
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
	return;
}
/* }}} */

/* {{{ proto string bcsqrt(string operand [, int scale])
   Returns the square root of an arbitrary precision number */
PHP_FUNCTION(bcsqrt)
{
	zend_string *left;
	zend_long scale_param = 0;
	bc_num result;
	int scale = (int)BCG(bc_precision);

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(left)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(scale_param)
	ZEND_PARSE_PARAMETERS_END();

	if (ZEND_NUM_ARGS() == 2) {
		scale = (int) ((int)scale_param < 0 ? 0 : scale_param);
	}

	bc_init_num(&result);
	php_str2num(&result, ZSTR_VAL(left));

	if (bc_sqrt (&result, scale) != 0) {
		RETVAL_STR(bc_num2str_ex(result, scale));
	} else {
		php_error_docref(NULL, E_WARNING, "Square root of negative number");
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
	zend_long scale_param = 0;
	bc_num first, second;
	int scale = (int)BCG(bc_precision);

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(left)
		Z_PARAM_STR(right)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(scale_param)
	ZEND_PARSE_PARAMETERS_END();

	if (ZEND_NUM_ARGS() == 3) {
		scale = (int) ((int)scale_param < 0 ? 0 : scale_param);
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

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(new_scale)
	ZEND_PARSE_PARAMETERS_END();

	old_scale = BCG(bc_precision);

	if (ZEND_NUM_ARGS() == 1) {
		BCG(bc_precision) = ((int)new_scale < 0) ? 0 : new_scale;
	}

	RETURN_LONG(old_scale);
}
/* }}} */


#endif
