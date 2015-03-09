/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Jim Winstead <jimw@php.net>                                 |
   |          Stig Sæther Bakken <ssb@php.net>                            |
   |          Zeev Suraski <zeev@zend.com>                                |
   | PHP 4.0 patches by Thies C. Arntzen <thies@thieso.net>               |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "php_math.h"
#include "zend_multiply.h"

#include <math.h>
#include <float.h>
#include <stdlib.h>

#include "basic_functions.h"

/* {{{ php_intlog10abs
   Returns floor(log10(fabs(val))), uses fast binary search */
static inline int php_intlog10abs(double value) {
	int result;
	value = fabs(value);

	if (value < 1e-8 || value > 1e22) {
		result = (int)floor(log10(value));
	} else {
		static const double values[] = {
			1e-8, 1e-7, 1e-6, 1e-5, 1e-4, 1e-3, 1e-2, 1e-1,
			1e0,  1e1,  1e2,  1e3,  1e4,  1e5,  1e6,  1e7,
			1e8,  1e9,  1e10, 1e11, 1e12, 1e13, 1e14, 1e15,
			1e16, 1e17, 1e18, 1e19, 1e20, 1e21, 1e22};
		/* Do a binary search with 5 steps */
		result = 15;
		if (value < values[result]) {
			result -= 8;
		} else {
			result += 8;
		}
		if (value < values[result]) {
			result -= 4;
		} else {
			result += 4;
		}
		if (value < values[result]) {
			result -= 2;
		} else {
			result += 2;
		}
		if (value < values[result]) {
			result -= 1;
		} else {
			result += 1;
		}
		if (value < values[result]) {
			result -= 1;
		}
		result -= 8;
	}
	return result;
}
/* }}} */

/* {{{ php_intpow10
       Returns pow(10.0, (double)power), uses fast lookup table for exact powers */
static inline double php_intpow10(int power) {
	static const double powers[] = {
		1e0,  1e1,  1e2,  1e3,  1e4,  1e5,  1e6,  1e7,
		1e8,  1e9,  1e10, 1e11, 1e12, 1e13, 1e14, 1e15,
		1e16, 1e17, 1e18, 1e19, 1e20, 1e21, 1e22};

	/* Not in lookup table */
	if (power < 0 || power > 22) {
		return pow(10.0, (double)power);
	}
	return powers[power];
}
/* }}} */

/* {{{ php_math_is_finite */
static inline int php_math_is_finite(double value) {
#if defined(PHP_WIN32)
	return _finite(value);
#elif defined(isfinite)
	return isfinite(value);
#else
	return value == value && (value == 0. || value * 2. != value);
#endif
}
/* }}} */

/* {{{ php_round_helper
       Actually performs the rounding of a value to integer in a certain mode */
static inline double php_round_helper(double value, int mode) {
	double tmp_value;

	if (value >= 0.0) {
		tmp_value = floor(value + 0.5);
		if ((mode == PHP_ROUND_HALF_DOWN && value == (-0.5 + tmp_value)) ||
			(mode == PHP_ROUND_HALF_EVEN && value == (0.5 + 2 * floor(tmp_value/2.0))) ||
			(mode == PHP_ROUND_HALF_ODD  && value == (0.5 + 2 * floor(tmp_value/2.0) - 1.0)))
		{
			tmp_value = tmp_value - 1.0;
		}
	} else {
		tmp_value = ceil(value - 0.5);
		if ((mode == PHP_ROUND_HALF_DOWN && value == (0.5 + tmp_value)) ||
			(mode == PHP_ROUND_HALF_EVEN && value == (-0.5 + 2 * ceil(tmp_value/2.0))) ||
			(mode == PHP_ROUND_HALF_ODD  && value == (-0.5 + 2 * ceil(tmp_value/2.0) + 1.0)))
		{
			tmp_value = tmp_value + 1.0;
		}
	}

	return tmp_value;
}
/* }}} */

/* {{{ _php_math_round */
/*
 * Rounds a number to a certain number of decimal places in a certain rounding
 * mode. For the specifics of the algorithm, see http://wiki.php.net/rfc/rounding
 */
PHPAPI double _php_math_round(double value, int places, int mode) {
	double f1, f2;
	double tmp_value;
	int precision_places;

	if (!php_math_is_finite(value)) {
		return value;
	}

	precision_places = 14 - php_intlog10abs(value);

	f1 = php_intpow10(abs(places));

	/* If the decimal precision guaranteed by FP arithmetic is higher than
	   the requested places BUT is small enough to make sure a non-zero value
	   is returned, pre-round the result to the precision */
	if (precision_places > places && precision_places - places < 15) {
		f2 = php_intpow10(abs(precision_places));
		if (precision_places >= 0) {
			tmp_value = value * f2;
		} else {
			tmp_value = value / f2;
		}
		/* preround the result (tmp_value will always be something * 1e14,
		   thus never larger than 1e15 here) */
		tmp_value = php_round_helper(tmp_value, mode);
		/* now correctly move the decimal point */
		f2 = php_intpow10(abs(places - precision_places));
		/* because places < precision_places */
		tmp_value = tmp_value / f2;
	} else {
		/* adjust the value */
		if (places >= 0) {
			tmp_value = value * f1;
		} else {
			tmp_value = value / f1;
		}
		/* This value is beyond our precision, so rounding it is pointless */
		if (fabs(tmp_value) >= 1e15) {
			return value;
		}
	}

	/* round the temp value */
	tmp_value = php_round_helper(tmp_value, mode);

	/* see if it makes sense to use simple division to round the value */
	if (abs(places) < 23) {
		if (places > 0) {
			tmp_value = tmp_value / f1;
		} else {
			tmp_value = tmp_value * f1;
		}
	} else {
		/* Simple division can't be used since that will cause wrong results.
		   Instead, the number is converted to a string and back again using
		   strtod(). strtod() will return the nearest possible FP value for
		   that string. */

		/* 40 Bytes should be more than enough for this format string. The
		   float won't be larger than 1e15 anyway. But just in case, use
		   snprintf() and make sure the buffer is zero-terminated */
		char buf[40];
		snprintf(buf, 39, "%15fe%d", tmp_value, -places);
		buf[39] = '\0';
		tmp_value = zend_strtod(buf, NULL);
		/* couldn't convert to string and back */
		if (!zend_finite(tmp_value) || zend_isnan(tmp_value)) {
			tmp_value = value;
		}
	}

	return tmp_value;
}
/* }}} */

/* {{{ php_asinh
*/
static double php_asinh(double z)
{
#ifdef HAVE_ASINH
	return(asinh(z));
#else
# ifdef _WIN64
	if (z >= 0) {
		return log(z + sqrt(z * z + 1));
	}
	else {
		return -log(-z + sqrt(z * z + 1));
	}
# else
	return(log(z + sqrt(1 + pow(z, 2))) / log(M_E));
# endif
#endif
}
/* }}} */

/* {{{ php_acosh
*/
static double php_acosh(double x)
{
#ifdef HAVE_ACOSH
	return(acosh(x));
#else
# ifdef _WIN64
	if (x >= 1) {
		return log(x + sqrt(x * x - 1));
	} else {
		return (DBL_MAX+DBL_MAX)-(DBL_MAX+DBL_MAX);
	}
# else
	return(log(x + sqrt(x * x - 1)));
# endif
#endif
}
/* }}} */

/* {{{ php_atanh
*/
static double php_atanh(double z)
{
#ifdef HAVE_ATANH
	return(atanh(z));
#else
	return(0.5 * log((1 + z) / (1 - z)));
#endif
}
/* }}} */

/* {{{ php_log1p
*/
static double php_log1p(double x)
{
#ifdef HAVE_LOG1P
	return(log1p(x));
#else
	return(log(1 + x));
#endif
}
/* }}} */

/* {{{ php_expm1
*/
static double php_expm1(double x)
{
#if !defined(PHP_WIN32) && !defined(NETWARE)
	return(expm1(x));
#else
	return(exp(x) - 1);
#endif
}
/* }}}*/

/* {{{ proto int abs(int number)
   Return the absolute value of the number */
PHP_FUNCTION(abs)
{
	zval *value;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &value) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();
#endif

	convert_scalar_to_number_ex(value);

	if (Z_TYPE_P(value) == IS_DOUBLE) {
		RETURN_DOUBLE(fabs(Z_DVAL_P(value)));
	} else if (Z_TYPE_P(value) == IS_LONG) {
		if (Z_LVAL_P(value) == ZEND_LONG_MIN) {
			RETURN_DOUBLE(-(double)ZEND_LONG_MIN);
		} else {
			RETURN_LONG(Z_LVAL_P(value) < 0 ? -Z_LVAL_P(value) : Z_LVAL_P(value));
		}
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto float ceil(float number)
   Returns the next highest integer value of the number */
PHP_FUNCTION(ceil)
{
	zval *value;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &value) == FAILURE) {
		return;
	}
	convert_scalar_to_number_ex(value);

	if (Z_TYPE_P(value) == IS_DOUBLE) {
		RETURN_DOUBLE(ceil(Z_DVAL_P(value)));
	} else if (Z_TYPE_P(value) == IS_LONG) {
		RETURN_DOUBLE(zval_get_double(value));
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto float floor(float number)
   Returns the next lowest integer value from the number */
PHP_FUNCTION(floor)
{
	zval *value;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &value) == FAILURE) {
		return;
	}
	convert_scalar_to_number_ex(value);

	if (Z_TYPE_P(value) == IS_DOUBLE) {
		RETURN_DOUBLE(floor(Z_DVAL_P(value)));
	} else if (Z_TYPE_P(value) == IS_LONG) {
		RETURN_DOUBLE(zval_get_double(value));
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto float round(float number [, int precision [, int mode]])
   Returns the number rounded to specified precision */
PHP_FUNCTION(round)
{
	zval *value;
	int places = 0;
	zend_long precision = 0;
	zend_long mode = PHP_ROUND_HALF_UP;
	double return_val;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|ll", &value, &precision, &mode) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS() >= 2) {
		places = (int) precision;
	}
	convert_scalar_to_number_ex(value);

	switch (Z_TYPE_P(value)) {
		case IS_LONG:
			/* Simple case - long that doesn't need to be rounded. */
			if (places >= 0) {
				RETURN_DOUBLE((double) Z_LVAL_P(value));
			}
			/* break omitted intentionally */

		case IS_DOUBLE:
			return_val = (Z_TYPE_P(value) == IS_LONG) ? (double)Z_LVAL_P(value) : Z_DVAL_P(value);
			return_val = _php_math_round(return_val, (int)places, (int)mode);
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
	double num;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "d", &num) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();
#endif
	RETURN_DOUBLE(sin(num));
}
/* }}} */

/* {{{ proto float cos(float number)
   Returns the cosine of the number in radians */
PHP_FUNCTION(cos)
{
	double num;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "d", &num) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();
#endif
	RETURN_DOUBLE(cos(num));
}
/* }}} */

/* {{{ proto float tan(float number)
   Returns the tangent of the number in radians */
PHP_FUNCTION(tan)
{
	double num;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "d", &num) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();
#endif
	RETURN_DOUBLE(tan(num));
}
/* }}} */

/* {{{ proto float asin(float number)
   Returns the arc sine of the number in radians */
PHP_FUNCTION(asin)
{
	double num;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "d", &num) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();
#endif
	RETURN_DOUBLE(asin(num));
}
/* }}} */

/* {{{ proto float acos(float number)
   Return the arc cosine of the number in radians */
PHP_FUNCTION(acos)
{
	double num;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "d", &num) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();
#endif
	RETURN_DOUBLE(acos(num));
}
/* }}} */

/* {{{ proto float atan(float number)
   Returns the arc tangent of the number in radians */
PHP_FUNCTION(atan)
{
	double num;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "d", &num) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();
#endif
	RETURN_DOUBLE(atan(num));
}
/* }}} */

/* {{{ proto float atan2(float y, float x)
   Returns the arc tangent of y/x, with the resulting quadrant determined by the signs of y and x */
PHP_FUNCTION(atan2)
{
	double num1, num2;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "dd", &num1, &num2) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_DOUBLE(num1)
		Z_PARAM_DOUBLE(num2)
	ZEND_PARSE_PARAMETERS_END();
#endif
	RETURN_DOUBLE(atan2(num1, num2));
}
/* }}} */

/* {{{ proto float sinh(float number)
   Returns the hyperbolic sine of the number, defined as (exp(number) - exp(-number))/2 */
PHP_FUNCTION(sinh)
{
	double num;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "d", &num) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();
#endif
	RETURN_DOUBLE(sinh(num));
}
/* }}} */

/* {{{ proto float cosh(float number)
   Returns the hyperbolic cosine of the number, defined as (exp(number) + exp(-number))/2 */
PHP_FUNCTION(cosh)
{
	double num;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "d", &num) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();
#endif
	RETURN_DOUBLE(cosh(num));
}
/* }}} */

/* {{{ proto float tanh(float number)
   Returns the hyperbolic tangent of the number, defined as sinh(number)/cosh(number) */
PHP_FUNCTION(tanh)
{
	double num;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "d", &num) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();
#endif
	RETURN_DOUBLE(tanh(num));
}
/* }}} */

/* {{{ proto float asinh(float number)
   Returns the inverse hyperbolic sine of the number, i.e. the value whose hyperbolic sine is number */
PHP_FUNCTION(asinh)
{
	double num;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "d", &num) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();
#endif
	RETURN_DOUBLE(php_asinh(num));
}
/* }}} */

/* {{{ proto float acosh(float number)
   Returns the inverse hyperbolic cosine of the number, i.e. the value whose hyperbolic cosine is number */
PHP_FUNCTION(acosh)
{
	double num;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "d", &num) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();
#endif
	RETURN_DOUBLE(php_acosh(num));
}
/* }}} */

/* {{{ proto float atanh(float number)
   Returns the inverse hyperbolic tangent of the number, i.e. the value whose hyperbolic tangent is number */
PHP_FUNCTION(atanh)
{
	double num;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "d", &num) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();
#endif
	RETURN_DOUBLE(php_atanh(num));
}
/* }}} */

/* {{{ proto float pi(void)
   Returns an approximation of pi */
PHP_FUNCTION(pi)
{
	RETURN_DOUBLE(M_PI);
}
/* }}} */

/* {{{ proto bool is_finite(float val)
   Returns whether argument is finite */
PHP_FUNCTION(is_finite)
{
	double dval;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "d", &dval) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(dval)
	ZEND_PARSE_PARAMETERS_END();
#endif
	RETURN_BOOL(zend_finite(dval));
}
/* }}} */

/* {{{ proto bool is_infinite(float val)
   Returns whether argument is infinite */
PHP_FUNCTION(is_infinite)
{
	double dval;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "d", &dval) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(dval)
	ZEND_PARSE_PARAMETERS_END();
#endif
	RETURN_BOOL(zend_isinf(dval));
}
/* }}} */

/* {{{ proto bool is_nan(float val)
   Returns whether argument is not a number */
PHP_FUNCTION(is_nan)
{
	double dval;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "d", &dval) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(dval)
	ZEND_PARSE_PARAMETERS_END();
#endif
	RETURN_BOOL(zend_isnan(dval));
}
/* }}} */

/* {{{ proto number pow(number base, number exponent)
   Returns base raised to the power of exponent. Returns integer result when possible */
PHP_FUNCTION(pow)
{
	zval *zbase, *zexp;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z/z/", &zbase, &zexp) == FAILURE) {
		return;
	}

	pow_function(return_value, zbase, zexp);
}
/* }}} */

/* {{{ proto float exp(float number)
   Returns e raised to the power of the number */
PHP_FUNCTION(exp)
{
	double num;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "d", &num) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();
#endif

	RETURN_DOUBLE(exp(num));
}
/* }}} */

/* {{{ proto float expm1(float number)
   Returns exp(number) - 1, computed in a way that accurate even when the value of number is close to zero */
/*
   WARNING: this function is expermental: it could change its name or
   disappear in the next version of PHP!
*/
PHP_FUNCTION(expm1)
{
	double num;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "d", &num) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();
#endif

	RETURN_DOUBLE(php_expm1(num));
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
	double num;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "d", &num) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();
#endif

	RETURN_DOUBLE(php_log1p(num));
}
/* }}} */

/* {{{ proto float log(float number, [float base])
   Returns the natural logarithm of the number, or the base log if base is specified */
PHP_FUNCTION(log)
{
	double num, base = 0;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "d|d", &num, &base) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_DOUBLE(num)
		Z_PARAM_OPTIONAL
		Z_PARAM_DOUBLE(base)
	ZEND_PARSE_PARAMETERS_END();
#endif

	if (ZEND_NUM_ARGS() == 1) {
		RETURN_DOUBLE(log(num));
	}

#ifdef HAVE_LOG2
	if (base == 2.0) {
		RETURN_DOUBLE(log2(num));
	}
#endif

	if (base == 10.0) {
		RETURN_DOUBLE(log10(num));
	}

	if (base == 1.0) {
		RETURN_DOUBLE(php_get_nan());
	}

	if (base <= 0.0) {
		php_error_docref(NULL, E_WARNING, "base must be greater than 0");
		RETURN_FALSE;
	}

	RETURN_DOUBLE(log(num) / log(base));
}
/* }}} */

/* {{{ proto float log10(float number)
   Returns the base-10 logarithm of the number */
PHP_FUNCTION(log10)
{
	double num;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "d", &num) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();
#endif

	RETURN_DOUBLE(log10(num));
}
/* }}} */

/* {{{ proto float sqrt(float number)
   Returns the square root of the number */
PHP_FUNCTION(sqrt)
{
	double num;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "d", &num) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();
#endif

	RETURN_DOUBLE(sqrt(num));
}
/* }}} */

/* {{{ proto float hypot(float num1, float num2)
   Returns sqrt(num1*num1 + num2*num2) */
PHP_FUNCTION(hypot)
{
	double num1, num2;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "dd", &num1, &num2) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_DOUBLE(num1)
		Z_PARAM_DOUBLE(num2)
	ZEND_PARSE_PARAMETERS_END();
#endif

#if HAVE_HYPOT
	RETURN_DOUBLE(hypot(num1, num2));
#elif defined(_MSC_VER)
	RETURN_DOUBLE(_hypot(num1, num2));
#else
	RETURN_DOUBLE(sqrt((num1 * num1) + (num2 * num2)));
#endif
}
/* }}} */

/* {{{ proto float deg2rad(float number)
   Converts the number in degrees to the radian equivalent */
PHP_FUNCTION(deg2rad)
{
	double deg;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "d", &deg) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(deg)
	ZEND_PARSE_PARAMETERS_END();
#endif
	RETURN_DOUBLE((deg / 180.0) * M_PI);
}
/* }}} */

/* {{{ proto float rad2deg(float number)
   Converts the radian number to the equivalent number in degrees */
PHP_FUNCTION(rad2deg)
{
	double rad;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "d", &rad) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(rad)
	ZEND_PARSE_PARAMETERS_END();
#endif

	RETURN_DOUBLE((rad / M_PI) * 180);
}
/* }}} */

/* {{{ _php_math_basetolong */
/*
 * Convert a string representation of a base(2-36) number to a long.
 */
PHPAPI zend_long _php_math_basetolong(zval *arg, int base)
{
	zend_long num = 0, digit, onum;
	zend_long i;
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

		{

			php_error_docref(NULL, E_WARNING, "Number '%s' is too big to fit in long", s);
			return ZEND_LONG_MAX;
		}
	}

	return num;
}
/* }}} */

/* {{{ _php_math_basetozval */
/*
 * Convert a string representation of a base(2-36) number to a zval.
 */
PHPAPI int _php_math_basetozval(zval *arg, int base, zval *ret)
{
	zend_long num = 0;
	double fnum = 0;
	zend_long i;
	int mode = 0;
	char c, *s;
	zend_long cutoff;
	int cutlim;

	if (Z_TYPE_P(arg) != IS_STRING || base < 2 || base > 36) {
		return FAILURE;
	}

	s = Z_STRVAL_P(arg);

	cutoff = ZEND_LONG_MAX / base;
	cutlim = ZEND_LONG_MAX % base;

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
				fnum = (double)num;
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
PHPAPI zend_string * _php_math_longtobase(zval *arg, int base)
{
	static char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	char buf[(sizeof(zend_ulong) << 3) + 1];
	char *ptr, *end;
	zend_ulong value;

	if (Z_TYPE_P(arg) != IS_LONG || base < 2 || base > 36) {
		return STR_EMPTY_ALLOC();
	}

	value = Z_LVAL_P(arg);

	end = ptr = buf + sizeof(buf) - 1;
	*ptr = '\0';

	do {
		*--ptr = digits[value % base];
		value /= base;
	} while (ptr > buf && value);

	return zend_string_init(ptr, end - ptr, 0);
}
/* }}} */

/* {{{ _php_math_zvaltobase */
/*
 * Convert a zval to a string containing a base(2-36) representation of
 * the number.
 */
PHPAPI zend_string * _php_math_zvaltobase(zval *arg, int base)
{
	static char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";

	if ((Z_TYPE_P(arg) != IS_LONG && Z_TYPE_P(arg) != IS_DOUBLE) || base < 2 || base > 36) {
		return STR_EMPTY_ALLOC();
	}

	if (Z_TYPE_P(arg) == IS_DOUBLE) {
		double fvalue = floor(Z_DVAL_P(arg)); /* floor it just in case */
		char *ptr, *end;
		char buf[(sizeof(double) << 3) + 1];

		/* Don't try to convert +/- infinity */
		if (fvalue == HUGE_VAL || fvalue == -HUGE_VAL) {
			php_error_docref(NULL, E_WARNING, "Number too large");
			return STR_EMPTY_ALLOC();
		}

		end = ptr = buf + sizeof(buf) - 1;
		*ptr = '\0';

		do {
			*--ptr = digits[(int) fmod(fvalue, base)];
			fvalue /= base;
		} while (ptr > buf && fabs(fvalue) >= 1);

		return zend_string_init(ptr, end - ptr, 0);
	}

	return _php_math_longtobase(arg, base);
}
/* }}} */

/* {{{ proto int bindec(string binary_number)
   Returns the decimal equivalent of the binary number */
PHP_FUNCTION(bindec)
{
	zval *arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &arg) == FAILURE) {
		return;
	}
	convert_to_string_ex(arg);
	if (_php_math_basetozval(arg, 2, return_value) == FAILURE) {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int hexdec(string hexadecimal_number)
   Returns the decimal equivalent of the hexadecimal number */
PHP_FUNCTION(hexdec)
{
	zval *arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &arg) == FAILURE) {
		return;
	}
	convert_to_string_ex(arg);
	if (_php_math_basetozval(arg, 16, return_value) == FAILURE) {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int octdec(string octal_number)
   Returns the decimal equivalent of an octal string */
PHP_FUNCTION(octdec)
{
	zval *arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &arg) == FAILURE) {
		return;
	}
	convert_to_string_ex(arg);
	if (_php_math_basetozval(arg, 8, return_value) == FAILURE) {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string decbin(int decimal_number)
   Returns a string containing a binary representation of the number */
PHP_FUNCTION(decbin)
{
	zval *arg;
	zend_string *result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &arg) == FAILURE) {
		return;
	}
	convert_to_long_ex(arg);
	result = _php_math_longtobase(arg, 2);
	RETURN_STR(result);
}
/* }}} */

/* {{{ proto string decoct(int decimal_number)
   Returns a string containing an octal representation of the given number */
PHP_FUNCTION(decoct)
{
	zval *arg;
	zend_string *result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &arg) == FAILURE) {
		return;
	}
	convert_to_long_ex(arg);
	result = _php_math_longtobase(arg, 8);
	RETURN_STR(result);
}
/* }}} */

/* {{{ proto string dechex(int decimal_number)
   Returns a string containing a hexadecimal representation of the given number */
PHP_FUNCTION(dechex)
{
	zval *arg;
	zend_string *result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &arg) == FAILURE) {
		return;
	}
	convert_to_long_ex(arg);
	result = _php_math_longtobase(arg, 16);
	RETURN_STR(result);
}
/* }}} */

/* {{{ proto string base_convert(string number, int frombase, int tobase)
   Converts a number in a string from any base <= 36 to any base <= 36 */
PHP_FUNCTION(base_convert)
{
	zval *number, temp;
	zend_long frombase, tobase;
	zend_string *result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zll", &number, &frombase, &tobase) == FAILURE) {
		return;
	}
	convert_to_string_ex(number);

	if (frombase < 2 || frombase > 36) {
		php_error_docref(NULL, E_WARNING, "Invalid `from base' (%pd)", frombase);
		RETURN_FALSE;
	}
	if (tobase < 2 || tobase > 36) {
		php_error_docref(NULL, E_WARNING, "Invalid `to base' (%pd)", tobase);
		RETURN_FALSE;
	}

	if(_php_math_basetozval(number, (int)frombase, &temp) == FAILURE) {
		RETURN_FALSE;
	}
	result = _php_math_zvaltobase(&temp, (int)tobase);
	RETVAL_STR(result);
}
/* }}} */

/* {{{ _php_math_number_format
*/
PHPAPI zend_string *_php_math_number_format(double d, int dec, char dec_point, char thousand_sep)
{
	return _php_math_number_format_ex(d, dec, &dec_point, 1, &thousand_sep, 1);
}

PHPAPI zend_string *_php_math_number_format_ex(double d, int dec, char *dec_point,
		size_t dec_point_len, char *thousand_sep, size_t thousand_sep_len)
{
	zend_string *res;
	zend_string *tmpbuf;
	char *s, *t;  /* source, target */
	char *dp;
	int integral;
	int reslen = 0;
	int count = 0;
	int is_negative=0;

	if (d < 0) {
		is_negative = 1;
		d = -d;
	}

	dec = MAX(0, dec);
	d = _php_math_round(d, dec, PHP_ROUND_HALF_UP);
	tmpbuf = strpprintf(0, "%.*F", dec, d);
	if (tmpbuf == NULL) {
		return NULL;
	} else if (!isdigit((int)tmpbuf->val[0])) {
		return tmpbuf;
	}

	/* find decimal point, if expected */
	if (dec) {
		dp = strpbrk(tmpbuf->val, ".,");
	} else {
		dp = NULL;
	}

	/* calculate the length of the return buffer */
	if (dp) {
		integral = (int)(dp - tmpbuf->val);
	} else {
		/* no decimal point was found */
		integral = (int)tmpbuf->len;
	}

	/* allow for thousand separators */
	if (thousand_sep) {
		integral += (int)(thousand_sep_len * ((integral-1) / 3));
	}

	reslen = integral;

	if (dec) {
		reslen += dec;

		if (dec_point) {
			reslen += (int)dec_point_len;
		}
	}

	/* add a byte for minus sign */
	if (is_negative) {
		reslen++;
	}
	res = zend_string_alloc(reslen, 0);

	s = tmpbuf->val + tmpbuf->len - 1;
	t = res->val + reslen;
	*t-- = '\0';

	/* copy the decimal places.
	 * Take care, as the sprintf implementation may return less places than
	 * we requested due to internal buffer limitations */
	if (dec) {
		int declen = (int)(dp ? s - dp : 0);
		int topad = dec > declen ? dec - declen : 0;

		/* pad with '0's */
		while (topad--) {
			*t-- = '0';
		}

		if (dp) {
			s -= declen + 1; /* +1 to skip the point */
			t -= declen;

			/* now copy the chars after the point */
			memcpy(t + 1, dp + 1, declen);
		}

		/* add decimal point */
		if (dec_point) {
			t -= dec_point_len;
			memcpy(t + 1, dec_point, dec_point_len);
		}
	}

	/* copy the numbers before the decimal point, adding thousand
	 * separator every three digits */
	while (s >= tmpbuf->val) {
		*t-- = *s--;
		if (thousand_sep && (++count%3)==0 && s>=tmpbuf->val) {
			t -= thousand_sep_len;
			memcpy(t + 1, thousand_sep, thousand_sep_len);
		}
	}

	/* and a minus sign, if needed */
	if (is_negative) {
		*t-- = '-';
	}

	res->len = reslen;
	zend_string_release(tmpbuf);
	return res;
}

/* {{{ proto string number_format(float number [, int num_decimal_places [, string dec_separator, string thousands_separator]])
   Formats a number with grouped thousands */
PHP_FUNCTION(number_format)
{
	double num;
	zend_long dec = 0;
	char *thousand_sep = NULL, *dec_point = NULL;
	char thousand_sep_chr = ',', dec_point_chr = '.';
	size_t thousand_sep_len = 0, dec_point_len = 0;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "d|ls!s!", &num, &dec, &dec_point, &dec_point_len, &thousand_sep, &thousand_sep_len) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 4)
		Z_PARAM_DOUBLE(num)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(dec)
		Z_PARAM_STRING_EX(dec_point, dec_point_len, 1, 0)
		Z_PARAM_STRING_EX(thousand_sep, thousand_sep_len, 1, 0)
	ZEND_PARSE_PARAMETERS_END();
#endif

	switch(ZEND_NUM_ARGS()) {
	case 1:
		RETURN_STR(_php_math_number_format(num, 0, dec_point_chr, thousand_sep_chr));
		break;
	case 2:
		RETURN_STR(_php_math_number_format(num, (int)dec, dec_point_chr, thousand_sep_chr));
		break;
	case 4:
		if (dec_point == NULL) {
			dec_point = &dec_point_chr;
			dec_point_len = 1;
		}

		if (thousand_sep == NULL) {
			thousand_sep = &thousand_sep_chr;
			thousand_sep_len = 1;
		}

		RETVAL_STR(_php_math_number_format_ex(num, (int)dec,
				dec_point, dec_point_len, thousand_sep, thousand_sep_len));
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

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "dd",  &num1, &num2) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_DOUBLE(num1)
		Z_PARAM_DOUBLE(num2)
	ZEND_PARSE_PARAMETERS_END();
#endif

	RETURN_DOUBLE(fmod(num1, num2));
}
/* }}} */

/* {{{ proto int intdiv(int numerator, int divisor)
   Returns the integer division of the numerator by the divisor */
PHP_FUNCTION(intdiv)
{
	zend_long numerator, divisor;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ll", &numerator, &divisor) == FAILURE) {
		return;
	}

	if (divisor == 0) {
		php_error_docref(NULL, E_WARNING, "Division by zero");
		RETURN_BOOL(0);
	} else if (divisor == -1 && numerator == ZEND_LONG_MIN) {
		/* Prevent overflow error/crash
		   We don't return a float here as that violates function contract */
		RETURN_LONG(0);
	}

	RETURN_LONG(numerator/divisor);
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
