/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Jim Winstead <jimw@php.net>                                 |
   |          Stig Sæther Bakken <ssb@fast.no>                            |
   |          Zeev Suraski <zeev@zend.com>                                |
   | PHP 4.0 patches by Thies C. Arntzen <thies@thieso.net>               |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "php_math.h"

#include <math.h>
#include <float.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* {{{ proto int abs(int number)
   Return the absolute value of the number */

PHP_FUNCTION(abs) 
{
	zval **value;
	
	if (ZEND_NUM_ARGS()!=1||zend_get_parameters_ex(1, &value)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_scalar_to_number_ex(value);
	
	if (Z_TYPE_PP(value) == IS_DOUBLE) {
		RETURN_DOUBLE(fabs(Z_DVAL_PP(value)));
	} else if (Z_TYPE_PP(value) == IS_LONG) {
		if (Z_LVAL_PP(value) == LONG_MIN) {
			RETURN_DOUBLE(-(double)LONG_MIN);
		} else {
			RETURN_LONG(Z_LVAL_PP(value) < 0 ? -Z_LVAL_PP(value) : Z_LVAL_PP(value));
		}
	}

	RETURN_FALSE;
}

/* }}} */ 
/* {{{ proto float ceil(float number)
   Returns the next highest integer value of the number */
PHP_FUNCTION(ceil) 
{
	zval **value;
	
	if (ZEND_NUM_ARGS()!=1||zend_get_parameters_ex(1, &value)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_scalar_to_number_ex(value);

	if (Z_TYPE_PP(value) == IS_DOUBLE) {
		RETURN_DOUBLE(ceil(Z_DVAL_PP(value)));
	} else if (Z_TYPE_PP(value) == IS_LONG) {
		convert_to_double_ex(value);
		RETURN_DOUBLE(Z_DVAL_PP(value));
	}

	RETURN_FALSE;
}

/* }}} */
/* {{{ proto float floor(float number)
   Returns the next lowest integer value from the number */
PHP_FUNCTION(floor)
{
	zval **value;
	
	if (ZEND_NUM_ARGS()!=1||zend_get_parameters_ex(1, &value)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_scalar_to_number_ex(value);

	if (Z_TYPE_PP(value) == IS_DOUBLE) {
		RETURN_DOUBLE(floor(Z_DVAL_PP(value)));
	} else if (Z_TYPE_PP(value) == IS_LONG) {
		convert_to_double_ex(value);
		RETURN_DOUBLE(Z_DVAL_PP(value));
	}

	RETURN_FALSE;
}

/* }}} */


/* {{{ proto float round(float number [, int precision])
   Returns the number rounded to specified precision */
PHP_FUNCTION(round)
{
	zval **value, **precision;
	int places = 0;
	double f, return_val;
	
	if (ZEND_NUM_ARGS() < 1 || ZEND_NUM_ARGS() > 2 ||
		zend_get_parameters_ex(ZEND_NUM_ARGS(), &value, &precision) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (ZEND_NUM_ARGS() == 2) {
		convert_to_long_ex(precision);
		places = (int) Z_LVAL_PP(precision);
	}

	convert_scalar_to_number_ex(value);

	switch (Z_TYPE_PP(value)) {
		case IS_LONG:
			/* Simple case - long that doesn't need to be rounded. */
			if (places >= 0) {
				RETURN_DOUBLE((double) Z_LVAL_PP(value));
			}
			/* break omitted intentionally */

		case IS_DOUBLE:
			return_val = (Z_TYPE_PP(value) == IS_LONG) ?
							(double)Z_LVAL_PP(value) : Z_DVAL_PP(value);

			f = pow(10.0, (double) places);

			return_val *= f;
			if (return_val >= 0.0)
				return_val = floor(return_val + 0.5);
			else
				return_val = ceil(return_val - 0.5);
			return_val /= f;

			RETURN_DOUBLE(return_val);
			break;

		default:
			RETURN_FALSE;
			break;
	}
}
/* }}} */
/* {{{ proto float sin(float number)
   Returns the sine of the number in radians */

PHP_FUNCTION(sin)
{
	zval **num;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(num);
	Z_DVAL_P(return_value) = sin(Z_DVAL_PP(num));
	Z_TYPE_P(return_value) = IS_DOUBLE;
}

/* }}} */
/* {{{ proto float cos(float number)
   Returns the cosine of the number in radians */

PHP_FUNCTION(cos)
{
	zval **num;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(num);
	Z_DVAL_P(return_value) = cos(Z_DVAL_PP(num));
	Z_TYPE_P(return_value) = IS_DOUBLE;
}
/* }}} */
/* {{{ proto float tan(float number)
   Returns the tangent of the number in radians */
PHP_FUNCTION(tan)
{
	zval **num;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(num);
	Z_DVAL_P(return_value) = tan(Z_DVAL_PP(num));
	Z_TYPE_P(return_value) = IS_DOUBLE;
}

/* }}} */
/* {{{ proto float asin(float number)
   Returns the arc sine of the number in radians */

PHP_FUNCTION(asin)
{
	zval **num;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(num);
	Z_DVAL_P(return_value) = asin(Z_DVAL_PP(num));
	Z_TYPE_P(return_value) = IS_DOUBLE;
}

/* }}} */
/* {{{ proto float acos(float number)
   Return the arc cosine of the number in radians */

PHP_FUNCTION(acos)
{
	zval **num;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(num);
	Z_DVAL_P(return_value) = acos(Z_DVAL_PP(num));
	Z_TYPE_P(return_value) = IS_DOUBLE;
}

/* }}} */
/* {{{ proto float atan(float number)
   Returns the arc tangent of the number in radians */

PHP_FUNCTION(atan)
{
	zval **num;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(num);
	Z_DVAL_P(return_value) = atan(Z_DVAL_PP(num));
	Z_TYPE_P(return_value) = IS_DOUBLE;
}

/* }}} */
/* {{{ proto float atan2(float y, float x)
   Returns the arc tangent of y/x, with the resulting quadrant determined by the signs of y and x */

PHP_FUNCTION(atan2)
{
	zval **num1, **num2;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &num1, &num2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(num1);
	convert_to_double_ex(num2);
	Z_DVAL_P(return_value) = atan2(Z_DVAL_PP(num1), Z_DVAL_PP(num2));
	Z_TYPE_P(return_value) = IS_DOUBLE;
}

/* }}} */
/* {{{ proto float sinh(float number)
   Returns the hyperbolic sine of the number, defined as (exp(number) - exp(-number))/2 */

PHP_FUNCTION(sinh)
{
	zval **num;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(num);
	Z_DVAL_P(return_value) = sinh(Z_DVAL_PP(num));
	Z_TYPE_P(return_value) = IS_DOUBLE;
}

/* }}} */
/* {{{ proto float cosh(float number)
   Returns the hyperbolic cosine of the number, defined as (exp(number) + exp(-number))/2 */

PHP_FUNCTION(cosh)
{
	zval **num;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(num);
	Z_DVAL_P(return_value) = cosh(Z_DVAL_PP(num));
	Z_TYPE_P(return_value) = IS_DOUBLE;
}
/* }}} */
/* {{{ proto float tanh(float number)
   Returns the hyperbolic tangent of the number, defined as sinh(number)/cosh(number) */
PHP_FUNCTION(tanh)
{
	zval **num;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(num);
	Z_DVAL_P(return_value) = tanh(Z_DVAL_PP(num));
	Z_TYPE_P(return_value) = IS_DOUBLE;
}

/* }}} */

#if !defined(PHP_WIN32) && !defined(NETWARE)
/* {{{ proto float asinh(float number)
   Returns the inverse hyperbolic sine of the number, i.e. the value whose hyperbolic sine is number */

PHP_FUNCTION(asinh)
{
	zval **num;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(num);
	Z_DVAL_P(return_value) = asinh(Z_DVAL_PP(num));
	Z_TYPE_P(return_value) = IS_DOUBLE;
}

/* }}} */
/* {{{ proto float acosh(float number)
   Returns the inverse hyperbolic cosine of the number, i.e. the value whose hyperbolic cosine is number */

PHP_FUNCTION(acosh)
{
	zval **num;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(num);
	Z_DVAL_P(return_value) = acosh(Z_DVAL_PP(num));
	Z_TYPE_P(return_value) = IS_DOUBLE;
}

/* }}} */
/* {{{ proto float atanh(float number)
   Returns the inverse hyperbolic tangent of the number, i.e. the value whose hyperbolic tangent is number */

PHP_FUNCTION(atanh)
{
	zval **num;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(num);
	Z_DVAL_P(return_value) = atanh(Z_DVAL_PP(num));
	Z_TYPE_P(return_value) = IS_DOUBLE;
}

/* }}} */
#endif

/* {{{ proto float pi(void)
   Returns an approximation of pi */

PHP_FUNCTION(pi)
{
	Z_DVAL_P(return_value) = M_PI;
	Z_TYPE_P(return_value) = IS_DOUBLE;
}

/* }}} */


/* {{{ proto bool is_finite(float val)
   Returns whether argument is finite */
PHP_FUNCTION(is_finite)
{
	double dval;


	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &dval) == FAILURE) {
		return;
	}
	RETURN_BOOL(zend_finite(dval));
}
/* }}} */

/* {{{ proto bool is_infinite(float val)
   Returns whether argument is infinite */
PHP_FUNCTION(is_infinite)
{
	double dval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &dval) == FAILURE) {
		return;
	}
	RETURN_BOOL(zend_isinf(dval));
}
/* }}} */

/* {{{ proto bool is_nan(float val)
   Returns whether argument is not a number */
PHP_FUNCTION(is_nan)
{
	double dval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &dval) == FAILURE) {
		return;
	}
	RETURN_BOOL(zend_isnan(dval));
}
/* }}} */

/* {{{ proto number pow(number base, number exponent)
   Returns base raised to the power of exponent. Returns integer result when possible */
PHP_FUNCTION(pow)
{
	zval *zbase, *zexp;
	double dval;
	zend_bool wantlong;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z/z/", &zbase, &zexp) == FAILURE) {
		return;
	}

	/* make sure we're dealing with numbers */
	convert_scalar_to_number(zbase TSRMLS_CC);
	convert_scalar_to_number(zexp TSRMLS_CC);

	/* if both base and exponent were longs, we'll try to get a long out */
	wantlong = Z_TYPE_P(zbase) == IS_LONG 
	        && Z_TYPE_P(zexp ) == IS_LONG && Z_LVAL_P(zexp) >= 0;

	convert_to_double(zbase);
	convert_to_double(zexp);
	
	/* go ahead and calculate things. */
	dval = pow(Z_DVAL_P(zbase),Z_DVAL_P(zexp));

	/* if we wanted a long, and dval < LONG_MAX, it must be a long. */
	if (wantlong && zend_finite(dval) && dval <= (double)LONG_MAX) {
		RETURN_LONG((long)dval);
	}

	/* otherwise just return the double. */
	RETURN_DOUBLE(dval);
}
/* }}} */

/* {{{ proto float exp(float number)
   Returns e raised to the power of the number */

PHP_FUNCTION(exp)
{
	zval **num;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(num);
	Z_DVAL_P(return_value) = exp(Z_DVAL_PP(num));
	Z_TYPE_P(return_value) = IS_DOUBLE;
}

/* }}} */


#if !defined(PHP_WIN32) && !defined(NETWARE)
/* {{{ proto float expm1(float number)
   Returns exp(number) - 1, computed in a way that accurate even when the value of number is close to zero */

/*
   WARNING: this function is expermental: it could change its name or 
   disappear in the next version of PHP!
*/

PHP_FUNCTION(expm1)
{
	zval **num;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(num);
	Z_DVAL_P(return_value) = expm1(Z_DVAL_PP(num));
	Z_TYPE_P(return_value) = IS_DOUBLE;
}

/* }}} */
/* {{{ proto float log1p(float number)
   Returns log(1 + number), computed in a way that accurate even when the value of number is close to zero */ 

/*
   WARNING: this function is expermental: it could change its name or 
   disappear in the next version of PHP!
*/

PHP_FUNCTION(log1p)
{
	zval **num;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(num);
	Z_DVAL_P(return_value) = log1p(Z_DVAL_PP(num));
	Z_TYPE_P(return_value) = IS_DOUBLE;
}

/* }}} */

#endif
/* {{{ proto float log(float number)
   Returns the natural logarithm of the number */

PHP_FUNCTION(log)
{
	zval **num;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(num);
	Z_DVAL_P(return_value) = log(Z_DVAL_PP(num));
	Z_TYPE_P(return_value) = IS_DOUBLE;
}

/* }}} */
/* {{{ proto float log10(float number)
   Returns the base-10 logarithm of the number */

PHP_FUNCTION(log10)
{
	zval **num;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(num);
	Z_DVAL_P(return_value) = log10(Z_DVAL_PP(num));
	Z_TYPE_P(return_value) = IS_DOUBLE;
}

/* }}} */
/* {{{ proto float sqrt(float number)
   Returns the square root of the number */

PHP_FUNCTION(sqrt)
{
	zval **num;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(num);
	Z_DVAL_P(return_value) = sqrt(Z_DVAL_PP(num));
	Z_TYPE_P(return_value) = IS_DOUBLE;
}

/* }}} */


/* {{{ proto float hypot(float num1, float num2)
   Returns sqrt(num1*num1 + num2*num2) */ 

/*
   WARNING: this function is expermental: it could change its name or 
   disappear in the next version of PHP!
*/

PHP_FUNCTION(hypot)
{
	zval **num1, **num2;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &num1, &num2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(num1);
	convert_to_double_ex(num2);
	Z_DVAL_P(return_value) = hypot(Z_DVAL_PP(num1), Z_DVAL_PP(num2));
	Z_TYPE_P(return_value) = IS_DOUBLE;
}

/* }}} */

/* {{{ proto float deg2rad(float number)
   Converts the number in degrees to the radian equivalent */

PHP_FUNCTION(deg2rad)
{
	zval **deg;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &deg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(deg);
	RETVAL_DOUBLE((Z_DVAL_PP(deg) / 180.0) * M_PI);
}

/* }}} */
/* {{{ proto float rad2deg(float number)
   Converts the radian number to the equivalent number in degrees */

PHP_FUNCTION(rad2deg)
{
	zval **rad;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &rad) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(rad);
	RETVAL_DOUBLE((Z_DVAL_PP(rad) / M_PI) * 180);
}

/* }}} */
/* {{{ _php_math_basetolong */

/*
 * Convert a string representation of a base(2-36) number to a long.
 */
PHPAPI long
_php_math_basetolong(zval *arg, int base) {
	long num = 0, digit, onum;
	int i;
	char c, *s;

	if (Z_TYPE_P(arg) != IS_STRING || base < 2 || base > 36) {
		return 0;
	}

	s = Z_STRVAL_P(arg);

	for (i = Z_STRLEN_P(arg); i > 0; i--) {
		c = *s++;
		
		digit = (c >= '0' && c <= '9') ? c - '0'
			: (c >= 'A' && c <= 'Z') ? c - 'A' + 10
			: (c >= 'a' && c <= 'z') ? c - 'a' + 10
			: base;
		
		if (digit >= base) {
			continue;
		}

		onum = num;
		num = num * base + digit;
		if (num > onum)
			continue;

		php_error(E_WARNING, "base_to_long: number '%s' is too big to fit in long", s);
		return LONG_MAX;
	}

	return num;
}

/* }}} */
/* {{{ _php_math_longtobase */

/* {{{ _php_math_basetozval */

/*
 * Convert a string representation of a base(2-36) number to a zval.
 */
PHPAPI int
_php_math_basetozval(zval *arg, int base, zval *ret) {
	long num = 0;
	double fnum;
	int i;
	int mode = 0;
	char c, *s;
	unsigned long cutoff;
	int cutlim;

	if (Z_TYPE_P(arg) != IS_STRING || base < 2 || base > 36) {
		return FAILURE;
	}

	s = Z_STRVAL_P(arg);

	cutoff = LONG_MAX / base;
	cutlim = LONG_MAX % base;
	
	for (i = Z_STRLEN_P(arg); i > 0; i--) {
		c = *s++;

		/* might not work for EBCDIC */
		if (c >= '0' && c <= '9') 
			c -= '0';
		else if (c >= 'A' && c <= 'Z') 
			c -= 'A' - 10;
		else if (c >= 'a' && c <= 'z') 
			c -= 'a' - 10;
		else
			continue;

		if (c >= base)
			continue;
		
		switch (mode) {
		case 0: /* Integer */
			if (num < cutoff || (num == cutoff && c <= cutlim)) {
				num = num * base + c;
				break;
			} else {
				fnum = num;
				mode = 1;
			}
			/* fall-through */
		case 1: /* Float */
			fnum = fnum * base + c;
		}	
	}

	if (mode == 1) {
		ZVAL_DOUBLE(ret, fnum);
	} else {
		ZVAL_LONG(ret, num);
	}
	return SUCCESS;
}

/* }}} */
/* {{{ _php_math_longtobase */

/*
 * Convert a long to a string containing a base(2-36) representation of
 * the number.
 */
PHPAPI char *
_php_math_longtobase(zval *arg, int base)
{
	static char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	char buf[(sizeof(unsigned long) << 3) + 1];
	char *ptr, *end;
	unsigned long value;

	if (Z_TYPE_P(arg) != IS_LONG || base < 2 || base > 36) {
		return empty_string;
	}

	value = Z_LVAL_P(arg);

	end = ptr = buf + sizeof(buf) - 1;
	*ptr = '\0';

	do {
		*--ptr = digits[value % base];
		value /= base;
	} while (ptr > buf && value);

	return estrndup(ptr, end - ptr);
}

/* }}} */
/* {{{ _php_math_zvaltobase */

/*
 * Convert a zval to a string containing a base(2-36) representation of
 * the number.
 */
PHPAPI char *
_php_math_zvaltobase(zval *arg, int base TSRMLS_DC)
{
	static char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";

	if ((Z_TYPE_P(arg) != IS_LONG && Z_TYPE_P(arg) != IS_DOUBLE) || base < 2 || base > 36) {
		return empty_string;
	}

	if (Z_TYPE_P(arg) == IS_DOUBLE) {
		double fvalue = floor(Z_DVAL_P(arg)); /* floor it just in case */
		char *ptr, *end;
		char buf[(sizeof(double) << 3) + 1];

		/* Don't try to convert +/- infinity */
		if (fvalue == HUGE_VAL || fvalue == -HUGE_VAL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Number too large");
			return empty_string;
		}

		end = ptr = buf + sizeof(buf) - 1;
		*ptr = '\0';

		do {
			*--ptr = digits[(int) fmod(fvalue, base)];
			fvalue /= base;
		} while (ptr > buf && fabs(fvalue) >= 1);

		return estrndup(ptr, end - ptr);
	}
	
	return _php_math_longtobase(arg, base);
}	

/* }}} */
/* {{{ proto int bindec(string binary_number)
   Returns the decimal equivalent of the binary number */

PHP_FUNCTION(bindec)
{
	zval **arg;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg);
	if(_php_math_basetozval(*arg, 2, return_value) != SUCCESS) {
		RETURN_FALSE;
	}
}

/* }}} */
/* {{{ proto int hexdec(string hexadecimal_number)
   Returns the decimal equivalent of the hexadecimal number */

PHP_FUNCTION(hexdec)
{
	zval **arg;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg);

	if(_php_math_basetozval(*arg, 16, return_value) != SUCCESS) {
		RETURN_FALSE;
	}
}

/* }}} */
/* {{{ proto int octdec(string octal_number)
   Returns the decimal equivalent of an octal string */

PHP_FUNCTION(octdec)
{
	zval **arg;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg);

	if(_php_math_basetozval(*arg, 8, return_value) != SUCCESS) {
		RETURN_FALSE;
	}
}

/* }}} */
/* {{{ proto string decbin(int decimal_number)
   Returns a string containing a binary representation of the number */

PHP_FUNCTION(decbin)
{
	zval **arg;
	char *result;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(arg);

	result = _php_math_longtobase(*arg, 2);
	Z_TYPE_P(return_value) = IS_STRING;
	Z_STRLEN_P(return_value) = strlen(result);
	Z_STRVAL_P(return_value) = result;
}

/* }}} */
/* {{{ proto string decoct(int decimal_number)
   Returns a string containing an octal representation of the given number */

PHP_FUNCTION(decoct)
{
	zval **arg;
	char *result;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(arg);

	result = _php_math_longtobase(*arg, 8);
	Z_TYPE_P(return_value) = IS_STRING;
	Z_STRLEN_P(return_value) = strlen(result);
	Z_STRVAL_P(return_value) = result;
}

/* }}} */
/* {{{ proto string dechex(int decimal_number)
   Returns a string containing a hexadecimal representation of the given number */

PHP_FUNCTION(dechex)
{
	zval **arg;
	char *result;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(arg);

	result = _php_math_longtobase(*arg, 16);
	Z_TYPE_P(return_value) = IS_STRING;
	Z_STRLEN_P(return_value) = strlen(result);
	Z_STRVAL_P(return_value) = result;
}

/* }}} */
/* {{{ proto string base_convert(string number, int frombase, int tobase)
   Converts a number in a string from any base <= 36 to any base <= 36 */

PHP_FUNCTION(base_convert)
{
	zval **number, **frombase, **tobase, temp;
	char *result;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &number, &frombase, &tobase) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(number);
	convert_to_long_ex(frombase);
	convert_to_long_ex(tobase);
	if (Z_LVAL_PP(frombase) < 2 || Z_LVAL_PP(frombase) > 36) {
		php_error(E_WARNING, "base_convert: invalid `from base' (%d)", Z_LVAL_PP(frombase));
		RETURN_FALSE;
	}
	if (Z_LVAL_PP(tobase) < 2 || Z_LVAL_PP(tobase) > 36) {
		php_error(E_WARNING, "base_convert: invalid `to base' (%d)", Z_LVAL_PP(tobase));
		RETURN_FALSE;
	}

	if(_php_math_basetozval(*number, Z_LVAL_PP(frombase), &temp) != SUCCESS) {
		RETURN_FALSE;
	}
	result = _php_math_zvaltobase(&temp, Z_LVAL_PP(tobase) TSRMLS_CC);
	RETVAL_STRING(result, 0);
} 

/* }}} */
/* {{{ _php_math_number_format */

PHPAPI char *_php_math_number_format(double d, int dec, char dec_point, char thousand_sep)
{
	char *tmpbuf, *resbuf;
	char *s, *t;  /* source, target */
	int tmplen, reslen=0;
	int count=0;
	int is_negative=0;
	
	if (d<0) {
		is_negative=1;
		d = -d;
	}
	dec = MAX(0, dec);
	tmpbuf = (char *) emalloc(1+DBL_MAX_10_EXP+1+dec+1);
	
	tmplen=sprintf(tmpbuf, "%.*f", dec, d);

	if (!isdigit((int)tmpbuf[0])) {
		return tmpbuf;
	}

	if (dec) {
		reslen = dec+1 + (tmplen-dec-1) + ((thousand_sep) ? (tmplen-1-dec-1)/3 : 0);
	} else {
		reslen = tmplen+((thousand_sep) ? (tmplen-1)/3 : 0);
	}
	if (is_negative) {
		reslen++;
	}
	resbuf = (char *) emalloc(reslen+1);
	
	s = tmpbuf+tmplen-1;
	t = resbuf+reslen;
	*t-- = 0;
	
	if (dec) {
		while (isdigit((int)*s)) {
			*t-- = *s--;
		}
		*t-- = dec_point;  /* copy that dot */
		s--;
	}
	
	while(s>=tmpbuf) {
		*t-- = *s--;
		if (thousand_sep && (++count%3)==0 && s>=tmpbuf) {
			*t-- = thousand_sep;
		}
	}
	if (is_negative) {
		*t-- = '-';
	}
	efree(tmpbuf);
	return resbuf;
}

/* }}} */
/* {{{ proto string number_format(float number [, int num_decimal_places [, string dec_seperator, string thousands_seperator]])
   Formats a number with grouped thousands */

PHP_FUNCTION(number_format)
{
	zval **num, **dec, **t_s, **d_p;
	char thousand_sep=',', dec_point='.';
	
	switch(ZEND_NUM_ARGS()) {
	case 1:
		if (zend_get_parameters_ex(1, &num)==FAILURE) {
			RETURN_FALSE;
		}
		convert_to_double_ex(num);
		RETURN_STRING(_php_math_number_format(Z_DVAL_PP(num), 0, dec_point, thousand_sep), 0);
		break;
	case 2:
		if (zend_get_parameters_ex(2, &num, &dec)==FAILURE) {
			RETURN_FALSE;
		}
		convert_to_double_ex(num);
		convert_to_long_ex(dec);
		RETURN_STRING(_php_math_number_format(Z_DVAL_PP(num), Z_LVAL_PP(dec), dec_point, thousand_sep), 0);
		break;
	case 4:
		if (zend_get_parameters_ex(4, &num, &dec, &d_p, &t_s)==FAILURE) {
			RETURN_FALSE;
		}
		convert_to_double_ex(num);
		convert_to_long_ex(dec);
		convert_to_string_ex(d_p);
		convert_to_string_ex(t_s);
		if (Z_STRLEN_PP(d_p)==1) {
			dec_point=Z_STRVAL_PP(d_p)[0];
		}
		if (Z_STRLEN_PP(t_s)==1) {
			thousand_sep=Z_STRVAL_PP(t_s)[0];
		} else if(Z_STRLEN_PP(t_s)==0) {
			thousand_sep=0;	
		}
		RETURN_STRING(_php_math_number_format(Z_DVAL_PP(num), Z_LVAL_PP(dec), dec_point, thousand_sep), 0);
		break;
	default:
		WRONG_PARAM_COUNT;
		break;
	}
}
/* }}} */

/* {{{ proto float fmod(float x, float y)
   Returns the remainder of dividing x by y as a float */
PHP_FUNCTION(fmod)
{
	double num1, num2;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd",  &num1, &num2) == FAILURE) {
		return;
	}
	
	Z_DVAL_P(return_value) = fmod(num1, num2);
	Z_TYPE_P(return_value) = IS_DOUBLE;
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
