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
   | Authors: Jim Winstead <jimw@php.net>                                 |
   |          Stig Sæther Bakken <ssb@php.net>                            |
   |          Zeev Suraski <zeev@php.net>                                 |
   | PHP 4.0 patches by Thies C. Arntzen <thies@thieso.net>               |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "php_math.h"
#include "zend_bitset.h"
#include "zend_exceptions.h"
#include "zend_multiply.h"
#include "zend_portability.h"

#include <float.h>
#include <math.h>
#include <stdlib.h>

#include "basic_functions.h"

/* {{{ php_intpow10
       Returns pow(10.0, (double)power), uses fast lookup table for exact powers */
static inline double php_intpow10(int power) {
	/* Not in lookup table */
	if (power < 0 || power > 22) {
		return pow(10.0, (double)power);
	}

	static const double powers[] = {
			1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9, 1e10, 1e11,
			1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19, 1e20, 1e21, 1e22};

	return powers[power];
}
/* }}} */

/* {{{ php_round_helper
       Actually performs the rounding of a value to integer in a certain mode */
static inline double php_round_helper(double adjusted_value, double value, double coefficient, int mode) {
	double integral = adjusted_value >= 0.0 ? floor(adjusted_value) : ceil(adjusted_value);
	double value_abs = fabs(value);
	double edge_case;

	if (fabs(adjusted_value) >= value_abs) {
		edge_case = fabs((integral + copysign(0.5, integral)) / coefficient);
	} else {
		edge_case = fabs((integral + copysign(0.5, integral)) * coefficient);
	}

	switch (mode) {
		case PHP_ROUND_HALF_UP:
			if (value_abs >= edge_case) {
				/* We must increase the magnitude of the integral part
				 * (rounding up / towards infinity). copysign(1.0, integral)
				 * will either result in 1.0 or -1.0 depending on the sign
				 * of the input, thus increasing the magnitude, but without
				 * generating branches in the assembly.
				 *
				 * This pattern is equally used for all the other modes.
				 */
				return integral + copysign(1.0, integral);
			}

			return integral;

		case PHP_ROUND_HALF_DOWN:
			if (value_abs > edge_case) {
				return integral + copysign(1.0, integral);
			}

			return integral;

		case PHP_ROUND_CEILING:
			if (value > 0.0 && fractional > 0.0) {
				return integral + 1.0;
			}

			return integral;

		case PHP_ROUND_FLOOR:
			if (value < 0.0 && fractional > 0.0) {
				return integral - 1.0;
			}

			return integral;

		case PHP_ROUND_TOWARD_ZERO:
			return integral;

		case PHP_ROUND_AWAY_FROM_ZERO:
			if (fractional > 0.0) {
				return integral + copysign(1.0, integral);
			}

			return integral;

		case PHP_ROUND_HALF_EVEN:
			if (value_abs > edge_case) {
				return integral + copysign(1.0, integral);
			} else if (value_abs == edge_case) {
				bool even = !fmod(integral, 2.0);

				/* If the integral part is not even we can make it even
				 * by adding one in the direction of the existing sign.
				 */
				if (!even) {
					return integral + copysign(1.0, integral);
				}
			}

			return integral;

		case PHP_ROUND_HALF_ODD:
			if (value_abs > edge_case) {
				return integral + copysign(1.0, integral);
			} else if (value_abs == edge_case) {
				bool even = !fmod(integral, 2.0);

				if (even) {
					return integral + copysign(1.0, integral);
				}
			}

			return integral;

		EMPTY_SWITCH_DEFAULT_CASE();
	}
	// FIXME: GCC bug, branch is considered reachable.
	ZEND_UNREACHABLE();
}
/* }}} */

/* {{{ _php_math_round */
/*
 * Rounds a number to a certain number of decimal places in a certain rounding
 * mode. For the specifics of the algorithm, see http://wiki.php.net/rfc/rounding
 */
PHPAPI double _php_math_round(double value, int places, int mode) {
	double f1;
	double tmp_value;

	if (!zend_finite(value) || value == 0.0) {
		return value;
	}

	places = places < INT_MIN+1 ? INT_MIN+1 : places;

	f1 = php_intpow10(abs(places));

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

	/* round the temp value */
	tmp_value = php_round_helper(tmp_value, value, f1, mode);

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

/* {{{ Return the absolute value of the number */
PHP_FUNCTION(abs)
{
	zval *value;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_NUMBER(value)
	ZEND_PARSE_PARAMETERS_END();

	switch (Z_TYPE_P(value)) {
		case IS_LONG:
			if (UNEXPECTED(Z_LVAL_P(value) == ZEND_LONG_MIN)) {
				RETURN_DOUBLE(-(double)ZEND_LONG_MIN);
			} else {
				RETURN_LONG(Z_LVAL_P(value) < 0 ? -Z_LVAL_P(value) : Z_LVAL_P(value));
			}
		case IS_DOUBLE:
			RETURN_DOUBLE(fabs(Z_DVAL_P(value)));
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}
/* }}} */

/* {{{ Returns the next highest integer value of the number */
PHP_FUNCTION(ceil)
{
	zval *value;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_NUMBER(value)
	ZEND_PARSE_PARAMETERS_END();

	switch (Z_TYPE_P(value)) {
		case IS_LONG:
			RETURN_DOUBLE(zval_get_double(value));
		case IS_DOUBLE:
			RETURN_DOUBLE(ceil(Z_DVAL_P(value)));
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}
/* }}} */

/* {{{ Returns the next lowest integer value from the number */
PHP_FUNCTION(floor)
{
	zval *value;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_NUMBER(value)
	ZEND_PARSE_PARAMETERS_END();

	switch (Z_TYPE_P(value)) {
		case IS_LONG:
			RETURN_DOUBLE(zval_get_double(value));
		case IS_DOUBLE:
			RETURN_DOUBLE(floor(Z_DVAL_P(value)));
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}
/* }}} */

/* {{{ Returns the number rounded to specified precision */
PHP_FUNCTION(round)
{
	zval *value;
	int places = 0;
	zend_long precision = 0;
	zend_long mode = PHP_ROUND_HALF_UP;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_NUMBER(value)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(precision)
		Z_PARAM_LONG(mode)
	ZEND_PARSE_PARAMETERS_END();

	if (ZEND_NUM_ARGS() >= 2) {
		if (precision >= 0) {
			places = ZEND_LONG_INT_OVFL(precision) ? INT_MAX : (int)precision;
		} else {
			places = ZEND_LONG_INT_UDFL(precision) ? INT_MIN : (int)precision;
		}
	}

	switch (mode) {
		case PHP_ROUND_HALF_UP:
		case PHP_ROUND_HALF_DOWN:
		case PHP_ROUND_HALF_EVEN:
		case PHP_ROUND_HALF_ODD:
		case PHP_ROUND_AWAY_FROM_ZERO:
		case PHP_ROUND_TOWARD_ZERO:
		case PHP_ROUND_CEILING:
		case PHP_ROUND_FLOOR:
			break;
		default:
			zend_argument_value_error(3, "must be a valid rounding mode (PHP_ROUND_*)");
			RETURN_THROWS();
	}

	switch (Z_TYPE_P(value)) {
		case IS_LONG:
			/* Simple case - long that doesn't need to be rounded. */
			if (places >= 0) {
				RETURN_DOUBLE(zval_get_double(value));
			}
			ZEND_FALLTHROUGH;

		case IS_DOUBLE:
			RETURN_DOUBLE(_php_math_round(zval_get_double(value), (int)places, (int)mode));

		EMPTY_SWITCH_DEFAULT_CASE();
	}
}
/* }}} */

/* {{{ Returns the sine of the number in radians */
PHP_FUNCTION(sin)
{
	double num;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();
	RETURN_DOUBLE(sin(num));
}
/* }}} */

/* {{{ Returns the cosine of the number in radians */
PHP_FUNCTION(cos)
{
	double num;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();
	RETURN_DOUBLE(cos(num));
}
/* }}} */

/* {{{ Returns the tangent of the number in radians */
PHP_FUNCTION(tan)
{
	double num;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();
	RETURN_DOUBLE(tan(num));
}
/* }}} */

/* {{{ Returns the arc sine of the number in radians */
PHP_FUNCTION(asin)
{
	double num;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();
	RETURN_DOUBLE(asin(num));
}
/* }}} */

/* {{{ Return the arc cosine of the number in radians */
PHP_FUNCTION(acos)
{
	double num;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();
	RETURN_DOUBLE(acos(num));
}
/* }}} */

/* {{{ Returns the arc tangent of the number in radians */
PHP_FUNCTION(atan)
{
	double num;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();
	RETURN_DOUBLE(atan(num));
}
/* }}} */

/* {{{ Returns the arc tangent of y/x, with the resulting quadrant determined by the signs of y and x */
PHP_FUNCTION(atan2)
{
	double num1, num2;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_DOUBLE(num1)
		Z_PARAM_DOUBLE(num2)
	ZEND_PARSE_PARAMETERS_END();
	RETURN_DOUBLE(atan2(num1, num2));
}
/* }}} */

/* {{{ Returns the hyperbolic sine of the number, defined as (exp(number) - exp(-number))/2 */
PHP_FUNCTION(sinh)
{
	double num;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();
	RETURN_DOUBLE(sinh(num));
}
/* }}} */

/* {{{ Returns the hyperbolic cosine of the number, defined as (exp(number) + exp(-number))/2 */
PHP_FUNCTION(cosh)
{
	double num;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();
	RETURN_DOUBLE(cosh(num));
}
/* }}} */

/* {{{ Returns the hyperbolic tangent of the number, defined as sinh(number)/cosh(number) */
PHP_FUNCTION(tanh)
{
	double num;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();
	RETURN_DOUBLE(tanh(num));
}
/* }}} */

/* {{{ Returns the inverse hyperbolic sine of the number, i.e. the value whose hyperbolic sine is number */
PHP_FUNCTION(asinh)
{
	double num;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();
	RETURN_DOUBLE(asinh(num));
}
/* }}} */

/* {{{ Returns the inverse hyperbolic cosine of the number, i.e. the value whose hyperbolic cosine is number */
PHP_FUNCTION(acosh)
{
	double num;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();
	RETURN_DOUBLE(acosh(num));
}
/* }}} */

/* {{{ Returns the inverse hyperbolic tangent of the number, i.e. the value whose hyperbolic tangent is number */
PHP_FUNCTION(atanh)
{
	double num;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();
	RETURN_DOUBLE(atanh(num));
}
/* }}} */

/* {{{ Returns an approximation of pi */
PHP_FUNCTION(pi)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_DOUBLE(M_PI);
}
/* }}} */

/* {{{ Returns whether argument is finite */
PHP_FUNCTION(is_finite)
{
	double dval;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(dval)
	ZEND_PARSE_PARAMETERS_END();
	RETURN_BOOL(zend_finite(dval));
}
/* }}} */

/* {{{ Returns whether argument is infinite */
PHP_FUNCTION(is_infinite)
{
	double dval;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(dval)
	ZEND_PARSE_PARAMETERS_END();
	RETURN_BOOL(zend_isinf(dval));
}
/* }}} */

/* {{{ Returns whether argument is not a number */
PHP_FUNCTION(is_nan)
{
	double dval;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(dval)
	ZEND_PARSE_PARAMETERS_END();
	RETURN_BOOL(zend_isnan(dval));
}
/* }}} */

/* {{{ Returns base raised to the power of exponent. Returns integer result when possible */
PHP_FUNCTION(pow)
{
	zval *zbase, *zexp;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(zbase)
		Z_PARAM_ZVAL(zexp)
	ZEND_PARSE_PARAMETERS_END();

	pow_function(return_value, zbase, zexp);
}
/* }}} */

/* {{{ Returns e raised to the power of the number */
PHP_FUNCTION(exp)
{
	double num;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_DOUBLE(exp(num));
}
/* }}} */

/* {{{ Returns exp(number) - 1, computed in a way that accurate even when the value of number is close to zero */
PHP_FUNCTION(expm1)
{
	double num;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_DOUBLE(expm1(num));
}
/* }}} */

/* {{{ Returns log(1 + number), computed in a way that accurate even when the value of number is close to zero */
PHP_FUNCTION(log1p)
{
	double num;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_DOUBLE(log1p(num));
}
/* }}} */

/* {{{ Returns the natural logarithm of the number, or the base log if base is specified */
PHP_FUNCTION(log)
{
	double num, base = 0;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_DOUBLE(num)
		Z_PARAM_OPTIONAL
		Z_PARAM_DOUBLE(base)
	ZEND_PARSE_PARAMETERS_END();

	if (ZEND_NUM_ARGS() == 1) {
		RETURN_DOUBLE(log(num));
	}

	if (base == 2.0) {
		RETURN_DOUBLE(log2(num));
	}

	if (base == 10.0) {
		RETURN_DOUBLE(log10(num));
	}

	if (base == 1.0) {
		RETURN_DOUBLE(ZEND_NAN);
	}

	if (base <= 0.0) {
		zend_argument_value_error(2, "must be greater than 0");
		RETURN_THROWS();
	}

	RETURN_DOUBLE(log(num) / log(base));
}
/* }}} */

/* {{{ Returns the base-10 logarithm of the number */
PHP_FUNCTION(log10)
{
	double num;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_DOUBLE(log10(num));
}
/* }}} */

/* {{{ Returns the square root of the number */
PHP_FUNCTION(sqrt)
{
	double num;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(num)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_DOUBLE(sqrt(num));
}
/* }}} */

/* {{{ Returns sqrt(num1*num1 + num2*num2) */
PHP_FUNCTION(hypot)
{
	double num1, num2;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_DOUBLE(num1)
		Z_PARAM_DOUBLE(num2)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_DOUBLE(hypot(num1, num2));
}
/* }}} */

/* {{{ Converts the number in degrees to the radian equivalent */
PHP_FUNCTION(deg2rad)
{
	double deg;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(deg)
	ZEND_PARSE_PARAMETERS_END();
	RETURN_DOUBLE((deg / 180.0) * M_PI);
}
/* }}} */

/* {{{ Converts the radian number to the equivalent number in degrees */
PHP_FUNCTION(rad2deg)
{
	double rad;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(rad)
	ZEND_PARSE_PARAMETERS_END();

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

			php_error_docref(NULL, E_WARNING, "Number %s is too big to fit in long", s);
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
PHPAPI void _php_math_basetozval(zend_string *str, int base, zval *ret)
{
	zend_long num = 0;
	double fnum = 0;
	int mode = 0;
	char c, *s, *e;
	zend_long cutoff;
	int cutlim;
	int invalidchars = 0;

	s = ZSTR_VAL(str);
	e = s + ZSTR_LEN(str);

	/* Skip leading whitespace */
	while (s < e && isspace(*s)) s++;
	/* Skip trailing whitespace */
	while (s < e && isspace(*(e-1))) e--;

	if (e - s >= 2) {
		if (base == 16 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) s += 2;
		if (base == 8 && s[0] == '0' && (s[1] == 'o' || s[1] == 'O')) s += 2;
		if (base == 2 && s[0] == '0' && (s[1] == 'b' || s[1] == 'B')) s += 2;
	}

	cutoff = ZEND_LONG_MAX / base;
	cutlim = ZEND_LONG_MAX % base;

	while (s < e) {
		c = *s++;

		/* might not work for EBCDIC */
		if (c >= '0' && c <= '9')
			c -= '0';
		else if (c >= 'A' && c <= 'Z')
			c -= 'A' - 10;
		else if (c >= 'a' && c <= 'z')
			c -= 'a' - 10;
		else {
			invalidchars++;
			continue;
		}

		if (c >= base) {
			invalidchars++;
			continue;
		}

		switch (mode) {
		case 0: /* Integer */
			if (num < cutoff || (num == cutoff && c <= cutlim)) {
				num = num * base + c;
				break;
			} else {
				fnum = (double)num;
				mode = 1;
			}
			ZEND_FALLTHROUGH;
		case 1: /* Float */
			fnum = fnum * base + c;
		}
	}

	if (invalidchars > 0) {
		zend_error(E_DEPRECATED, "Invalid characters passed for attempted conversion, these have been ignored");
	}

	if (mode == 1) {
		ZVAL_DOUBLE(ret, fnum);
	} else {
		ZVAL_LONG(ret, num);
	}
}
/* }}} */

/* {{{ _php_math_longtobase */
/*
 * Convert a long to a string containing a base(2-36) representation of
 * the number.
 */
PHPAPI zend_string * _php_math_longtobase(zend_long arg, int base)
{
	static const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	char buf[(sizeof(zend_ulong) << 3) + 1];
	char *ptr, *end;
	zend_ulong value;

	if (base < 2 || base > 36) {
		return ZSTR_EMPTY_ALLOC();
	}

	value = arg;

	end = ptr = buf + sizeof(buf) - 1;
	*ptr = '\0';

	do {
		ZEND_ASSERT(ptr > buf);
		*--ptr = digits[value % base];
		value /= base;
	} while (value);

	return zend_string_init(ptr, end - ptr, 0);
}
/* }}} */

/* {{{ _php_math_longtobase_pwr2 */
/*
 * Convert a long to a string containing a base(2,4,6,16,32) representation of
 * the number.
 */
static zend_always_inline zend_string * _php_math_longtobase_pwr2(zend_long arg, int base_log2)
{
	static const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	zend_ulong value;
	size_t len;
	zend_string *ret;
	char *ptr;

	value = arg;

	if (value == 0) {
		len = 1;
	} else {
		len = ((sizeof(value) * 8 - zend_ulong_nlz(value)) + (base_log2 - 1)) / base_log2;
	}

	ret = zend_string_alloc(len, 0);
	ptr = ZSTR_VAL(ret) + len;
	*ptr = '\0';

	do {
		ZEND_ASSERT(ptr > ZSTR_VAL(ret));
		*--ptr = digits[value & ((1 << base_log2) - 1)];
		value >>= base_log2;
	} while (value);

	return ret;
}
/* }}} */

/* {{{ _php_math_zvaltobase */
/*
 * Convert a zval to a string containing a base(2-36) representation of
 * the number.
 */
PHPAPI zend_string * _php_math_zvaltobase(zval *arg, int base)
{
	static const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";

	if ((Z_TYPE_P(arg) != IS_LONG && Z_TYPE_P(arg) != IS_DOUBLE) || base < 2 || base > 36) {
		return ZSTR_EMPTY_ALLOC();
	}

	if (Z_TYPE_P(arg) == IS_DOUBLE) {
		double fvalue = floor(Z_DVAL_P(arg)); /* floor it just in case */
		char *ptr, *end;
		char buf[(sizeof(double) << 3) + 1];

		/* Don't try to convert +/- infinity */
		if (fvalue == ZEND_INFINITY || fvalue == -ZEND_INFINITY) {
			zend_value_error("An infinite value cannot be converted to base %d", base);
			return NULL;
		}

		end = ptr = buf + sizeof(buf) - 1;
		*ptr = '\0';

		do {
			*--ptr = digits[(int) fmod(fvalue, base)];
			fvalue /= base;
		} while (ptr > buf && fabs(fvalue) >= 1);

		return zend_string_init(ptr, end - ptr, 0);
	}

	return _php_math_longtobase(Z_LVAL_P(arg), base);
}
/* }}} */

/* {{{ Returns the decimal equivalent of the binary number */
PHP_FUNCTION(bindec)
{
	zend_string *arg;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(arg)
	ZEND_PARSE_PARAMETERS_END();

	_php_math_basetozval(arg, 2, return_value);
}
/* }}} */

/* {{{ Returns the decimal equivalent of the hexadecimal number */
PHP_FUNCTION(hexdec)
{
	zend_string *arg;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(arg)
	ZEND_PARSE_PARAMETERS_END();

	_php_math_basetozval(arg, 16, return_value);
}
/* }}} */

/* {{{ Returns the decimal equivalent of an octal string */
PHP_FUNCTION(octdec)
{
	zend_string *arg;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(arg)
	ZEND_PARSE_PARAMETERS_END();

	_php_math_basetozval(arg, 8, return_value);
}
/* }}} */

/* {{{ Returns a string containing a binary representation of the number */
PHP_FUNCTION(decbin)
{
	zend_long arg;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(arg)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_STR(_php_math_longtobase_pwr2(arg, 1));
}
/* }}} */

/* {{{ Returns a string containing an octal representation of the given number */
PHP_FUNCTION(decoct)
{
	zend_long arg;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(arg)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_STR(_php_math_longtobase_pwr2(arg, 3));
}
/* }}} */

/* {{{ Returns a string containing a hexadecimal representation of the given number */
PHP_FUNCTION(dechex)
{
	zend_long arg;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(arg)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_STR(_php_math_longtobase_pwr2(arg, 4));
}
/* }}} */

/* {{{ Converts a number in a string from any base <= 36 to any base <= 36 */
PHP_FUNCTION(base_convert)
{
	zval temp;
	zend_string *number;
	zend_long frombase, tobase;
	zend_string *result;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_STR(number)
		Z_PARAM_LONG(frombase)
		Z_PARAM_LONG(tobase)
	ZEND_PARSE_PARAMETERS_END();

	if (frombase < 2 || frombase > 36) {
		zend_argument_value_error(2, "must be between 2 and 36 (inclusive)");
		RETURN_THROWS();
	}
	if (tobase < 2 || tobase > 36) {
		zend_argument_value_error(3, "must be between 2 and 36 (inclusive)");
		RETURN_THROWS();
	}

	_php_math_basetozval(number, (int)frombase, &temp);
	result = _php_math_zvaltobase(&temp, (int)tobase);
	if (!result) {
		RETURN_THROWS();
	}

	RETVAL_STR(result);
}
/* }}} */

/* {{{ _php_math_number_format */
PHPAPI zend_string *_php_math_number_format(double d, int dec, char dec_point, char thousand_sep)
{
	return _php_math_number_format_ex(d, dec, &dec_point, 1, &thousand_sep, 1);
}

PHPAPI zend_string *_php_math_number_format_ex(double d, int dec, const char *dec_point,
		size_t dec_point_len, const char *thousand_sep, size_t thousand_sep_len)
{
	zend_string *res;
	zend_string *tmpbuf;
	char *s, *t;  /* source, target */
	char *dp;
	size_t integral;
	size_t reslen = 0;
	int count = 0;
	int is_negative = 0;

	if (d < 0) {
		is_negative = 1;
		d = -d;
	}

	d = _php_math_round(d, dec, PHP_ROUND_HALF_UP);
	dec = MAX(0, dec);
	tmpbuf = strpprintf(0, "%.*F", dec, d);
	if (tmpbuf == NULL) {
		return NULL;
	} else if (!isdigit((int)ZSTR_VAL(tmpbuf)[0])) {
		return tmpbuf;
	}

	/* Check if the number is no longer negative after rounding */
	if (is_negative && d == 0) {
		is_negative = 0;
	}

	/* find decimal point, if expected */
	if (dec) {
		dp = strpbrk(ZSTR_VAL(tmpbuf), ".,");
	} else {
		dp = NULL;
	}

	/* calculate the length of the return buffer */
	if (dp) {
		integral = (dp - ZSTR_VAL(tmpbuf));
	} else {
		/* no decimal point was found */
		integral = ZSTR_LEN(tmpbuf);
	}

	/* allow for thousand separators */
	if (thousand_sep) {
		integral = zend_safe_addmult((integral-1)/3, thousand_sep_len, integral, "number formatting");
	}

	reslen = integral;

	if (dec) {
		reslen += dec;

		if (dec_point) {
			reslen = zend_safe_addmult(reslen, 1, dec_point_len, "number formatting");
		}
	}

	/* add a byte for minus sign */
	if (is_negative) {
		reslen++;
	}
	res = zend_string_alloc(reslen, 0);

	s = ZSTR_VAL(tmpbuf) + ZSTR_LEN(tmpbuf) - 1;
	t = ZSTR_VAL(res) + reslen;
	*t-- = '\0';

	/* copy the decimal places.
	 * Take care, as the sprintf implementation may return less places than
	 * we requested due to internal buffer limitations */
	if (dec) {
		size_t declen = (dp ? s - dp : 0);
		size_t topad = (size_t)dec > declen ? dec - declen : 0;

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
	while (s >= ZSTR_VAL(tmpbuf)) {
		*t-- = *s--;
		if (thousand_sep && (++count%3)==0 && s >= ZSTR_VAL(tmpbuf)) {
			t -= thousand_sep_len;
			memcpy(t + 1, thousand_sep, thousand_sep_len);
		}
	}

	/* and a minus sign, if needed */
	if (is_negative) {
		*t-- = '-';
	}

	ZSTR_LEN(res) = reslen;
	zend_string_release_ex(tmpbuf, 0);
	return res;
}

PHPAPI zend_string *_php_math_number_format_long(zend_long num, zend_long dec, const char *dec_point,
		size_t dec_point_len, const char *thousand_sep, size_t thousand_sep_len)
{
	static const zend_ulong powers[] = {
		1, 10, 100, 1000, 10000,
		100000, 1000000, 10000000, 100000000, 1000000000,
#if SIZEOF_ZEND_LONG == 8
		10000000000, 100000000000, 1000000000000, 10000000000000, 100000000000000,
		1000000000000000, 10000000000000000, 100000000000000000, 1000000000000000000, 10000000000000000000ul
#elif SIZEOF_ZEND_LONG > 8
# error "Unknown SIZEOF_ZEND_LONG"
#endif
	};

	int is_negative = 0;
	zend_ulong tmpnum;
	zend_ulong power;
	zend_ulong power_half;
	zend_ulong rest;

	zend_string *tmpbuf;
	zend_string *res;
	size_t reslen;
	char *s, *t;  /* source, target */
	int count = 0;
	size_t topad;

	// unsigned absolute number and memorize negative sign
	if (num < 0) {
		is_negative = 1;
		tmpnum = ((zend_ulong)-(num + 1)) + 1;
	} else {
		tmpnum = (zend_ulong)num;
	}

	// rounding the number
	if (dec < 0) {
		// Check rounding to more negative places than possible
		if (dec < -(sizeof(powers) / sizeof(powers[0]) - 1)) {
			tmpnum = 0;
		} else {
			power = powers[-dec];
			power_half = power / 2;
			rest = tmpnum % power;
			tmpnum = tmpnum / power;

			if (rest >= power_half) {
				tmpnum = tmpnum * power + power;
			} else {
				tmpnum = tmpnum * power;
			}
		}

		// prevent resulting in negative zero
		if (tmpnum == 0) {
			is_negative = 0;
		}
	}

	tmpbuf = strpprintf(0, ZEND_ULONG_FMT, tmpnum);
	reslen = ZSTR_LEN(tmpbuf);

	/* allow for thousand separators */
	if (thousand_sep) {
		reslen = zend_safe_addmult((reslen-1)/3, thousand_sep_len, reslen, "number formatting");
	}

	reslen += is_negative;

	if (dec > 0) {
		reslen += dec;

		if (dec_point) {
			reslen = zend_safe_addmult(reslen, 1, dec_point_len, "number formatting");
		}
	}

	res = zend_string_alloc(reslen, 0);

	s = ZSTR_VAL(tmpbuf) + ZSTR_LEN(tmpbuf) - 1;
	t = ZSTR_VAL(res) + reslen;
	*t-- = '\0';

	/* copy the decimal places. */
	if (dec > 0) {
		topad = (size_t)dec;

		/* pad with '0's */
		while (topad--) {
			*t-- = '0';
		}

		/* add decimal point */
		if (dec_point) {
			t -= dec_point_len;
			memcpy(t + 1, dec_point, dec_point_len);
		}
	}

	/* copy the numbers before the decimal point, adding thousand
	 * separator every three digits */
	while (s >= ZSTR_VAL(tmpbuf)) {
		*t-- = *s--;
		if (thousand_sep && (++count % 3) == 0 && s >= ZSTR_VAL(tmpbuf)) {
			t -= thousand_sep_len;
			memcpy(t + 1, thousand_sep, thousand_sep_len);
		}
	}

	if (is_negative) {
		*t-- = '-';
	}

	ZSTR_LEN(res) = reslen;
	zend_string_release_ex(tmpbuf, 0);
	return res;
}

/* {{{ Formats a number with grouped thousands */
PHP_FUNCTION(number_format)
{
	zval* num;
	zend_long dec = 0;
	int dec_int;
	char *thousand_sep = NULL, *dec_point = NULL;
	size_t thousand_sep_len = 0, dec_point_len = 0;

	ZEND_PARSE_PARAMETERS_START(1, 4)
		Z_PARAM_NUMBER(num)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(dec)
		Z_PARAM_STRING_OR_NULL(dec_point, dec_point_len)
		Z_PARAM_STRING_OR_NULL(thousand_sep, thousand_sep_len)
	ZEND_PARSE_PARAMETERS_END();

	if (dec_point == NULL) {
		dec_point = ".";
		dec_point_len = 1;
	}
	if (thousand_sep == NULL) {
		thousand_sep = ",";
		thousand_sep_len = 1;
	}

	switch (Z_TYPE_P(num)) {
		case IS_LONG:
			RETURN_STR(_php_math_number_format_long(Z_LVAL_P(num), dec, dec_point, dec_point_len, thousand_sep, thousand_sep_len));
			break;

		case IS_DOUBLE:
			// double values of >= 2^52 can not have fractional digits anymore
			// Casting to long on 64bit will not loose precision on rounding
			if (UNEXPECTED(
				(Z_DVAL_P(num) >= 4503599627370496.0 || Z_DVAL_P(num) <= -4503599627370496.0)
				&& ZEND_DOUBLE_FITS_LONG(Z_DVAL_P(num))
			)) {
				RETURN_STR(_php_math_number_format_long((zend_long)Z_DVAL_P(num), dec, dec_point, dec_point_len, thousand_sep, thousand_sep_len));
                break;
			}

			if (dec >= 0) {
				dec_int = ZEND_LONG_INT_OVFL(dec) ? INT_MAX : (int)dec;
			} else {
				dec_int = ZEND_LONG_INT_UDFL(dec) ? INT_MIN : (int)dec;
			}
			RETURN_STR(_php_math_number_format_ex(Z_DVAL_P(num), dec_int, dec_point, dec_point_len, thousand_sep, thousand_sep_len));
			break;

		EMPTY_SWITCH_DEFAULT_CASE()
	}
}
/* }}} */

/* {{{ Returns the remainder of dividing x by y as a float */
PHP_FUNCTION(fmod)
{
	double num1, num2;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_DOUBLE(num1)
		Z_PARAM_DOUBLE(num2)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_DOUBLE(fmod(num1, num2));
}
/* }}} */

/* {{{ Perform floating-point division of dividend / divisor
   with IEEE-754 semantics for division by zero. */
#ifdef __clang__
__attribute__((no_sanitize("float-divide-by-zero")))
#endif
PHP_FUNCTION(fdiv)
{
	double dividend, divisor;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_DOUBLE(dividend)
		Z_PARAM_DOUBLE(divisor)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_DOUBLE(dividend / divisor);
}
/* }}} */

/* {{{ Returns the integer quotient of the division of dividend by divisor */
PHP_FUNCTION(intdiv)
{
	zend_long dividend, divisor;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(dividend)
		Z_PARAM_LONG(divisor)
	ZEND_PARSE_PARAMETERS_END();

	if (divisor == 0) {
		zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Division by zero");
		RETURN_THROWS();
	} else if (divisor == -1 && dividend == ZEND_LONG_MIN) {
		/* Prevent overflow error/crash ... really should not happen:
		   We don't return a float here as that violates function contract */
		zend_throw_exception_ex(zend_ce_arithmetic_error, 0, "Division of PHP_INT_MIN by -1 is not an integer");
		RETURN_THROWS();
	}

	RETURN_LONG(dividend / divisor);
}
/* }}} */
