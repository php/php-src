/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Andi Gutmans <andi@zend.com>                                 |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if WITH_BCMATH

#include "ext/standard/info.h"
#include "php_bcmath.h"
#include "libbcmath/src/bcmath.h"

ZEND_DECLARE_MODULE_GLOBALS(bcmath);

function_entry bcmath_functions[] = {
	PHP_FE(bcadd,									NULL)
	PHP_FE(bcsub,									NULL)
	PHP_FE(bcmul,									NULL)
	PHP_FE(bcdiv,									NULL)
	PHP_FE(bcmod,									NULL)
	PHP_FE(bcpow,									NULL)
	PHP_FE(bcsqrt,									NULL)
	PHP_FE(bcscale,									NULL)
	PHP_FE(bccomp,									NULL)
	{NULL, NULL, NULL}
};

zend_module_entry bcmath_module_entry = {
	STANDARD_MODULE_HEADER,
	"bcmath",
	bcmath_functions,
#if ZTS
	PHP_MODULE_STARTUP_N(bcmath),
#else
	NULL,
#endif
	NULL,
	PHP_RINIT(bcmath),
	PHP_RSHUTDOWN(bcmath),
	PHP_MINFO(bcmath),
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_BCMATH
ZEND_GET_MODULE(bcmath)
#endif

#ifndef THREAD_SAFE
static long bc_precision;
#endif

#if ZTS
PHP_MODULE_STARTUP_D(bcmath)
{
	zend_bcmath_globals *bcmath_globals;

	ts_allocate_id(&bcmath_globals_id, sizeof(zend_bcmath_globals), NULL, NULL);
	bcmath_globals = ts_resource(bcmath_globals_id);
	return SUCCESS;
}
#endif

PHP_RSHUTDOWN_FUNCTION(bcmath)
{
	bc_free_num(&BCG(_zero_));
	bc_free_num(&BCG(_one_));
	bc_free_num(&BCG(_two_));

	return SUCCESS;
}


PHP_RINIT_FUNCTION(bcmath)
{
	if (cfg_get_long("bcmath.scale", &bc_precision)==FAILURE) {
		bc_precision=0;
	}
	if(bc_precision<0) bc_precision=0;
	bc_init_numbers(TSRMLS_C);
	
	return SUCCESS;
}


PHP_MINFO_FUNCTION(bcmath)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "BCMath support", "enabled");
	php_info_print_table_end();
}

/* {{{ php_str2num
   Convert to bc_num detecting scale */
static void php_str2num(bc_num *num, char *str TSRMLS_DC) 
{
	char *p;

	if (!(p = strchr(str, '.'))) {
		bc_str2num(num, str, 0 TSRMLS_CC);
		return;
	}

	bc_str2num(num, str, strlen(p+1) TSRMLS_CC);
}
/* }}} */

/* {{{ proto string bcadd(string left_operand, string right_operand [, int scale])
   Returns the sum of two arbitrary precision numbers */
PHP_FUNCTION(bcadd)
{
	pval **left, **right, **scale_param;
	bc_num first, second, result;
	int scale=bc_precision;

	switch (ZEND_NUM_ARGS()) {
		case 2:
				if (zend_get_parameters_ex(2, &left, &right) == FAILURE) {
		        	WRONG_PARAM_COUNT;
    			}
				break;
		case 3:
				if (zend_get_parameters_ex(3, &left, &right, &scale_param) == FAILURE) {
		        	WRONG_PARAM_COUNT;
    			}
				convert_to_long_ex(scale_param);
				scale = (int) (Z_LVAL_PP(scale_param)<0) ? 0:Z_LVAL_PP(scale_param);
				break;
		default:
				WRONG_PARAM_COUNT;
				break;
	}
	convert_to_string_ex(left);
	convert_to_string_ex(right);
	bc_init_num(&first TSRMLS_CC);
	bc_init_num(&second TSRMLS_CC);
	bc_init_num(&result TSRMLS_CC);
	php_str2num(&first, Z_STRVAL_PP(left) TSRMLS_CC);
	php_str2num(&second, Z_STRVAL_PP(right) TSRMLS_CC);
	bc_add (first, second, &result, scale);
	if (result->n_scale > scale)
		result->n_scale = scale;
	Z_STRVAL_P(return_value) = bc_num2str(result);
	Z_STRLEN_P(return_value) = strlen(Z_STRVAL_P(return_value));
	Z_TYPE_P(return_value) = IS_STRING;
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
	pval **left, **right, **scale_param;
	bc_num first, second, result;
	int scale=bc_precision;

	switch (ZEND_NUM_ARGS()) {
		case 2:
				if (zend_get_parameters_ex(2, &left, &right) == FAILURE) {
		        	WRONG_PARAM_COUNT;
    			}
				break;
		case 3:
				if (zend_get_parameters_ex(3, &left, &right, &scale_param) == FAILURE) {
		        	WRONG_PARAM_COUNT;
    			}
				convert_to_long_ex(scale_param);
				scale = (int) (Z_LVAL_PP(scale_param)<0) ? 0:Z_LVAL_PP(scale_param);
				break;
		default:
				WRONG_PARAM_COUNT;
				break;
	}
	convert_to_string_ex(left);
	convert_to_string_ex(right);
	bc_init_num(&first TSRMLS_CC);
	bc_init_num(&second TSRMLS_CC);
	bc_init_num(&result TSRMLS_CC);
	php_str2num(&first, Z_STRVAL_PP(left) TSRMLS_CC);
	php_str2num(&second, Z_STRVAL_PP(right) TSRMLS_CC);
	bc_sub (first, second, &result, scale);
	if (result->n_scale > scale)
		result->n_scale = scale;
	Z_STRVAL_P(return_value) = bc_num2str(result);
	Z_STRLEN_P(return_value) = strlen(Z_STRVAL_P(return_value));
	Z_TYPE_P(return_value) = IS_STRING;
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
	pval **left, **right, **scale_param;
	bc_num first, second, result;
	int scale=bc_precision;

	switch (ZEND_NUM_ARGS()) {
		case 2:
				if (zend_get_parameters_ex(2, &left, &right) == FAILURE) {
		        	WRONG_PARAM_COUNT;
    			}
				break;
		case 3:
				if (zend_get_parameters_ex(3, &left, &right, &scale_param) == FAILURE) {
		        	WRONG_PARAM_COUNT;
    			}
				convert_to_long_ex(scale_param);
				scale = (int) (Z_LVAL_PP(scale_param)<0) ? 0:Z_LVAL_PP(scale_param);
				break;
		default:
				WRONG_PARAM_COUNT;
				break;
	}
	convert_to_string_ex(left);
	convert_to_string_ex(right);
	bc_init_num(&first TSRMLS_CC);
	bc_init_num(&second TSRMLS_CC);
	bc_init_num(&result TSRMLS_CC);
	php_str2num(&first, Z_STRVAL_PP(left) TSRMLS_CC);
	php_str2num(&second, Z_STRVAL_PP(right) TSRMLS_CC);
	bc_multiply (first, second, &result, scale TSRMLS_CC);
	if (result->n_scale > scale)
		result->n_scale = scale;
	Z_STRVAL_P(return_value) = bc_num2str(result);
	Z_STRLEN_P(return_value) = strlen(Z_STRVAL_P(return_value));
	Z_TYPE_P(return_value) = IS_STRING;
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
	pval **left, **right, **scale_param;
	bc_num first, second, result;
	int scale=bc_precision;

	switch (ZEND_NUM_ARGS()) {
		case 2:
				if (zend_get_parameters_ex(2, &left, &right) == FAILURE) {
		        	WRONG_PARAM_COUNT;
    			}
				break;
		case 3:
				if (zend_get_parameters_ex(3, &left, &right, &scale_param) == FAILURE) {
		        	WRONG_PARAM_COUNT;
    			}
				convert_to_long_ex(scale_param);
				scale = (int) (Z_LVAL_PP(scale_param)<0) ? 0:Z_LVAL_PP(scale_param);
				break;
		default:
				WRONG_PARAM_COUNT;
				break;
	}
	convert_to_string_ex(left);
	convert_to_string_ex(right);
	bc_init_num(&first TSRMLS_CC);
	bc_init_num(&second TSRMLS_CC);
	bc_init_num(&result TSRMLS_CC);
	php_str2num(&first, Z_STRVAL_PP(left) TSRMLS_CC);
	php_str2num(&second, Z_STRVAL_PP(right) TSRMLS_CC);
	switch (bc_divide (first, second, &result, scale TSRMLS_CC)) {
		case 0: /* OK */
			if (result->n_scale > scale)
				result->n_scale = scale;
			Z_STRVAL_P(return_value) = bc_num2str(result);
			Z_STRLEN_P(return_value) = strlen(Z_STRVAL_P(return_value));
			Z_TYPE_P(return_value) = IS_STRING;
			break;
		case -1: /* division by zero */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Division by zero");
			break;
	}
	bc_free_num(&first);
	bc_free_num(&second);
	bc_free_num(&result);
	return;
}
/* }}} */

/* {{{ proto string bcmod(string left_operand, string right_operand)
   Returns the modulus of the two arbitrary precision operands */
PHP_FUNCTION(bcmod)
{
	pval **left, **right;
	bc_num first, second, result;

	switch (ZEND_NUM_ARGS()) {
		case 2:
				if (zend_get_parameters_ex(2, &left, &right) == FAILURE) {
		        	WRONG_PARAM_COUNT;
    			}
				break;
		default:
				WRONG_PARAM_COUNT;
				break;
	}
	convert_to_string_ex(left);
	convert_to_string_ex(right);
	bc_init_num(&first TSRMLS_CC);
	bc_init_num(&second TSRMLS_CC);
	bc_init_num(&result TSRMLS_CC);
	bc_str2num(&first, Z_STRVAL_PP(left), 0 TSRMLS_CC);
	bc_str2num(&second, Z_STRVAL_PP(right), 0 TSRMLS_CC);
	switch (bc_modulo(first, second, &result, 0 TSRMLS_CC)) {
		case 0:
			Z_STRVAL_P(return_value) = bc_num2str(result);
			Z_STRLEN_P(return_value) = strlen(Z_STRVAL_P(return_value));
			Z_TYPE_P(return_value) = IS_STRING;
			break;
		case -1:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Division by zero");
			break;
	}
	bc_free_num(&first);
	bc_free_num(&second);
	bc_free_num(&result);
	return;
}
/* }}} */

/* {{{ proto string bcpow(string x, string y [, int scale])
   Returns the value of an arbitrary precision number raised to the power of another */
PHP_FUNCTION(bcpow)
{
	pval **left, **right, **scale_param;
	bc_num first, second, result;
	int scale=bc_precision;

	switch (ZEND_NUM_ARGS()) {
		case 2:
				if (zend_get_parameters_ex(2, &left, &right) == FAILURE) {
		        	WRONG_PARAM_COUNT;
    			}
				break;
		case 3:
				if (zend_get_parameters_ex(3, &left, &right, &scale_param) == FAILURE) {
		        	WRONG_PARAM_COUNT;
    			}
				convert_to_long_ex(scale_param);
				scale = (int) (Z_LVAL_PP(scale_param)<0) ? 0:Z_LVAL_PP(scale_param);
				break;
		default:
				WRONG_PARAM_COUNT;
				break;
	}
	convert_to_string_ex(left);
	convert_to_string_ex(right);
	bc_init_num(&first TSRMLS_CC);
	bc_init_num(&second TSRMLS_CC);
	bc_init_num(&result TSRMLS_CC);
	php_str2num(&first, Z_STRVAL_PP(left) TSRMLS_CC);
	php_str2num(&second, Z_STRVAL_PP(right) TSRMLS_CC);
	bc_raise (first, second, &result, scale TSRMLS_CC);
	if (result->n_scale > scale)
		result->n_scale = scale;
	Z_STRVAL_P(return_value) = bc_num2str(result);
	Z_STRLEN_P(return_value) = strlen(Z_STRVAL_P(return_value));
	Z_TYPE_P(return_value) = IS_STRING;
	bc_free_num(&first);
	bc_free_num(&second);
	bc_free_num(&result);
	return;
}
/* }}} */

/* {{{ proto string bcsqrt(string operand [, int scale])
   Returns the square root of an arbitray precision number */
PHP_FUNCTION(bcsqrt)
{
	pval **left, **scale_param;
	bc_num result;
	int scale=bc_precision;

	switch (ZEND_NUM_ARGS()) {
		case 1:
				if (zend_get_parameters_ex(1, &left)== FAILURE) {
		        	WRONG_PARAM_COUNT;
    			}
				break;
		case 2:
				if (zend_get_parameters_ex(2, &left, &scale_param) == FAILURE) {
		        	WRONG_PARAM_COUNT;
    			}
				convert_to_long_ex(scale_param);
				scale = (int) (Z_LVAL_PP(scale_param)<0) ? 0:Z_LVAL_PP(scale_param);
				break;
		default:
				WRONG_PARAM_COUNT;
				break;
	}
	convert_to_string_ex(left);
	bc_init_num(&result TSRMLS_CC);
	php_str2num(&result, Z_STRVAL_PP(left) TSRMLS_CC);
	if (bc_sqrt (&result, scale TSRMLS_CC) != 0) {
		if (result->n_scale > scale)
			result->n_scale = scale;
		Z_STRVAL_P(return_value) = bc_num2str(result);
		Z_STRLEN_P(return_value) = strlen(Z_STRVAL_P(return_value));
		Z_TYPE_P(return_value) = IS_STRING;
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Square root of negative number");
	}
	bc_free_num(&result);
	return;
}
/* }}} */

/* {{{ proto string bccomp(string left_operand, string right_operand [, int scale])
   Compares two arbitrary precision numbers */
PHP_FUNCTION(bccomp)
{
	pval **left, **right, **scale_param;
	bc_num first, second;
	int scale=bc_precision;

	switch (ZEND_NUM_ARGS()) {
		case 2:
				if (zend_get_parameters_ex(2, &left, &right) == FAILURE) {
		        	WRONG_PARAM_COUNT;
    			}
				break;
		case 3:
				if (zend_get_parameters_ex(3, &left, &right, &scale_param) == FAILURE) {
		        	WRONG_PARAM_COUNT;
    			}
				convert_to_long_ex(scale_param);
				scale = (int) (Z_LVAL_PP(scale_param)<0) ? 0:Z_LVAL_PP(scale_param);
				break;
		default:
				WRONG_PARAM_COUNT;
				break;
	}

	convert_to_string_ex(left);
	convert_to_string_ex(right);
	bc_init_num(&first TSRMLS_CC);
	bc_init_num(&second TSRMLS_CC);

	bc_str2num(&first, Z_STRVAL_PP(left), scale TSRMLS_CC);
	bc_str2num(&second, Z_STRVAL_PP(right), scale TSRMLS_CC);
	Z_LVAL_P(return_value) = bc_compare(first, second);
	Z_TYPE_P(return_value) = IS_LONG;

	bc_free_num(&first);
	bc_free_num(&second);
	return;
}
/* }}} */

/* {{{ proto string bcscale(int scale)
   Sets default scale parameter for all bc math functions */
PHP_FUNCTION(bcscale)
{
	pval **new_scale;
	
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &new_scale)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(new_scale);
	bc_precision = (Z_LVAL_PP(new_scale)<0) ? 0 : Z_LVAL_PP(new_scale);

	RETURN_TRUE;
}
/* }}} */


#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
