/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2017 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andrea Faulds <ajf@ajf.me>                                  |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "php_operators.h"
#include "zend_operators.h"

/* {{{ proto number +(number|array a[, number|array b])
   Returns sum or union of a and b (or convesion of a to int or float as appropriate) */
PHP_NAMED_FUNCTION(php_operator_add)
{
	zval *a, *b = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|z", &a, &b) == FAILURE) {
		return;
	}

	/* Unary +a is actually (+1 * a) since (0 + a) handles signs wrong */
	if (!b) {
		zval factor;
		ZVAL_LONG(&factor, 1);
		mul_function(return_value, &factor, a);
	} else {
		add_function(return_value, a, b);
	}
}
/* }}} */

/* {{{ proto number -(number a[, number b])
   Returns difference of a and b (or opposite of a) */
PHP_NAMED_FUNCTION(php_operator_sub)
{
	zval *a, *b = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|z", &a, &b) == FAILURE) {
		return;
	}

	/* Unary -a is actually (-1 * a) since (0 - a) handles signs wrong */
	if (!b) {
		zval factor;
		ZVAL_LONG(&factor, -1);
		mul_function(return_value, &factor, a);
	} else {
		sub_function(return_value, a, b);
	}
}
/* }}} */

/* {{{ proto number *(number a, number b)
   Returns product of a and b */
PHP_NAMED_FUNCTION(php_operator_mul)
{
	zval *a, *b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a, &b) == FAILURE) {
		return;
	}

	mul_function(return_value, a, b);
}
/* }}} */

/* {{{ proto number /(number a, number b)
   Returns quotient of a and b */
PHP_NAMED_FUNCTION(php_operator_div)
{
	zval *a, *b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a, &b) == FAILURE) {
		return;
	}

	div_function(return_value, a, b);
}
/* }}} */

/* {{{ proto int %(int a, int b)
   Returns remainder of a divided by b */
PHP_NAMED_FUNCTION(php_operator_mod)
{
	zval *a, *b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a, &b) == FAILURE) {
		return;
	}

	mod_function(return_value, a, b);
}
/* }}} */

/* {{{ proto number **(number a, number b)
   Returns result of raising a to the b'th power */
PHP_NAMED_FUNCTION(php_operator_pow)
{
	zval *a, *b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a, &b) == FAILURE) {
		return;
	}

	pow_function(return_value, a, b);
}
/* }}} */

/* {{{ proto int|string &(int|string a, int|string b)
   Returns bitwise AND of a and b */
PHP_NAMED_FUNCTION(php_operator_bitwise_and)
{
	zval *a, *b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a, &b) == FAILURE) {
		return;
	}

	bitwise_and_function(return_value, a, b);
}
/* }}} */

/* {{{ proto int|string |(int|string a, int|string b)
   Returns bitwise OR of a and b */
PHP_NAMED_FUNCTION(php_operator_bitwise_or)
{
	zval *a, *b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a, &b) == FAILURE) {
		return;
	}

	bitwise_or_function(return_value, a, b);
}
/* }}} */

/* {{{ proto int|string ^(int|string a, int|string b)
   Returns bitwise XOR of a and b */
PHP_NAMED_FUNCTION(php_operator_bitwise_xor)
{
	zval *a, *b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a, &b) == FAILURE) {
		return;
	}

	bitwise_xor_function(return_value, a, b);
}
/* }}} */

/* {{{ proto int|string ~(int|string a)
   Returns bitwise NOT of a */
PHP_NAMED_FUNCTION(php_operator_bitwise_not)
{
	zval *a;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &a) == FAILURE) {
		return;
	}

	bitwise_not_function(return_value, a);
}
/* }}} */

/* {{{ proto int <<(int a, int b)
   Returns result of shifting bits of a left by b */
PHP_NAMED_FUNCTION(php_operator_shift_left)
{
	zval *a, *b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a, &b) == FAILURE) {
		return;
	}

	shift_left_function(return_value, a, b);
}
/* }}} */

/* {{{ proto int >>(int a, int b)
   Returns result of shifting bits of a left by b */
PHP_NAMED_FUNCTION(php_operator_shift_right)
{
	zval *a, *b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a, &b) == FAILURE) {
		return;
	}

	shift_right_function(return_value, a, b);
}
/* }}} */

/* {{{ proto bool ==(mixed a, mixed b)
   Returns TRUE if a is equal to b after type juggling */
PHP_NAMED_FUNCTION(php_operator_equal)
{
	zval *a, *b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a, &b) == FAILURE) {
		return;
	}

	is_equal_function(return_value, a, b);
}
/* }}} */

/* {{{ proto bool ===(mixed a, mixed b)
   Returns TRUE if a is equal to b, and they are of the same type */
PHP_NAMED_FUNCTION(php_operator_identical)
{
	zval *a, *b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a, &b) == FAILURE) {
		return;
	}

	is_identical_function(return_value, a, b);
}
/* }}} */

// Also <>(a, b)
/* {{{ proto bool !=(mixed a, mixed b)
   Returns TRUE if a is not equal to b after type juggling */
PHP_NAMED_FUNCTION(php_operator_not_equal)
{
	zval *a, *b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a, &b) == FAILURE) {
		return;
	}

	is_not_equal_function(return_value, a, b);
}
/* }}} */

/* {{{ proto bool !==(mixed a, mixed b)
   Returns TRUE if a is not equal to b, or they are not of the same type */
PHP_NAMED_FUNCTION(php_operator_not_identical)
{
	zval *a, *b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a, &b) == FAILURE) {
		return;
	}

	is_not_identical_function(return_value, a, b);
}
/* }}} */

/* {{{ proto bool <(mixed a, mixed b)
   Returns TRUE if a is strictly less than b */
PHP_NAMED_FUNCTION(php_operator_less_than)
{
	zval *a, *b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a, &b) == FAILURE) {
		return;
	}

	is_smaller_function(return_value, a, b);
}
/* }}} */

/* {{{ proto bool >(mixed a, mixed b)
   Returns TRUE if a is strictly greater than b */
PHP_NAMED_FUNCTION(php_operator_greater_than)
{
	zval *a, *b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a, &b) == FAILURE) {
		return;
	}

	is_smaller_function(return_value, b, a);
}
/* }}} */

/* {{{ proto bool <=(mixed a, mixed b)
   Returns TRUE if a is strictly less than or equal to b */
PHP_NAMED_FUNCTION(php_operator_less_than_or_equal_to)
{
	zval *a, *b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a, &b) == FAILURE) {
		return;
	}

	is_smaller_or_equal_function(return_value, a, b);
}
/* }}} */

/* {{{ proto bool >=(mixed a, mixed b)
   Returns TRUE if a is strictly greater than or equal to b */
PHP_NAMED_FUNCTION(php_operator_greater_than_or_equal_to)
{
	zval *a, *b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a, &b) == FAILURE) {
		return;
	}

	is_smaller_or_equal_function(return_value, b, a);
}
/* }}} */

/* {{{ proto int <=>(mixed a, mixed b)
   Returns -1 if a is less than b, 0 if a is equal to b, or 1 if a is greater than b */
PHP_NAMED_FUNCTION(php_operator_spaceship)
{
	zval *a, *b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a, &b) == FAILURE) {
		return;
	}

	compare_function(return_value, a, b);
}
/* }}} */

// Also and(a, b)
/* {{{ proto bool &&(bool a, bool b)
   Returns TRUE if both a and b are true */
PHP_NAMED_FUNCTION(php_operator_boolean_and)
{
	zval *a, *b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a, &b) == FAILURE) {
		return;
	}

	RETURN_BOOL(i_zend_is_true(a) && i_zend_is_true(b));
}
/* }}} */

// Also or(a, b)
/* {{{ proto bool ||(bool a, bool b)
   Returns TRUE if either a or b is true */
PHP_NAMED_FUNCTION(php_operator_boolean_or)
{
	zval *a, *b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a, &b) == FAILURE) {
		return;
	}

	RETURN_BOOL(i_zend_is_true(a) || i_zend_is_true(b));
}
/* }}} */

/* {{{ proto bool xor(bool a, bool b)
   Returns TRUE if either a or b is true, but not both */
PHP_NAMED_FUNCTION(php_operator_boolean_xor)
{
	zval *a, *b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a, &b) == FAILURE) {
		return;
	}

	boolean_xor_function(return_value, a, b);
}
/* }}} */

/* {{{ proto bool !(bool a)
   Returns TRUE if a is not true */
PHP_NAMED_FUNCTION(php_operator_boolean_not)
{
	zval *a;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &a) == FAILURE) {
		return;
	}

	boolean_not_function(return_value, a);
}
/* }}} */

/* {{{ proto string .(string a, string b)
   Returns the result of concatenating a and b */
PHP_NAMED_FUNCTION(php_operator_concat)
{
	zval *a, *b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a, &b) == FAILURE) {
		return;
	}

	concat_function(return_value, a, b);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
