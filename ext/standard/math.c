/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Jim Winstead (jimw@php.net)                                 |
   |          Stig Sæther Bakken <ssb@guardian.no>                        |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "phpmath.h"
#include "snprintf.h"

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

char *_php3_number_format(double, int, char ,char);

/* {{{ proto int abs(int number)
   Return the absolute value of the number */
PHP_FUNCTION(abs) {
	pval *value;
	
	if (ARG_COUNT(ht)!=1||getParameters(ht,1,&value)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (value->type == IS_STRING) {
		convert_scalar_to_number(value);
	}

	if (value->type == IS_DOUBLE) {
		RETURN_DOUBLE(fabs(value->value.dval));
	}
	else if (value->type == IS_LONG) {
		RETURN_LONG(labs(value->value.lval));
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int ceil(double number)
   Returns the next highest integer value of the number */
PHP_FUNCTION(ceil) {
	pval *value;
	
	if (ARG_COUNT(ht)!=1||getParameters(ht,1,&value)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (value->type == IS_STRING) {
		convert_scalar_to_number(value);
	}

	if (value->type == IS_DOUBLE) {
		RETURN_LONG((long)ceil(value->value.dval));
	}
	else if (value->type == IS_LONG) {
		RETURN_LONG(value->value.lval);
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int floor(double number)
   Returns the next lowest integer value from the number */
PHP_FUNCTION(floor) {
	pval *value;
	
	if (ARG_COUNT(ht)!=1||getParameters(ht,1,&value)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (value->type == IS_STRING) {
		convert_scalar_to_number(value);
	}

	if (value->type == IS_DOUBLE) {
		RETURN_LONG((long)floor(value->value.dval));
	}
	else if (value->type == IS_LONG) {
		RETURN_LONG(value->value.lval);
	}

	RETURN_FALSE;
}
/* }}} */

#ifndef HAVE_RINT
/* emulate rint */
inline double rint(double n)
{
	double i, f;
	f = modf(n, &i);
	if (f > .5)
		i++;
	else if (f < -.5)
		i--;
	return i;
}
#endif

/* {{{ proto int round(double number)
   Returns the rounded value of the number */
PHP_FUNCTION(round)
{
	pval *value;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &value) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	if (value->type == IS_STRING) {
		convert_scalar_to_number(value);
	}
	if (value->type == IS_DOUBLE) {
		RETURN_DOUBLE(rint(value->value.dval));
	}
	if (value->type == IS_LONG) {
		RETURN_DOUBLE((double)value->value.lval);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto double sin(double number)
   Returns the sine of the number in radians */
PHP_FUNCTION(sin)
{
	pval *num;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double(num);
	return_value->value.dval = sin(num->value.dval);
	return_value->type = IS_DOUBLE;
}
/* }}} */

/* {{{ proto double cos(double number)
   Returns the cosine of the number in radians */
PHP_FUNCTION(cos)
{
	pval *num;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double(num);
	return_value->value.dval = cos(num->value.dval);
	return_value->type = IS_DOUBLE;
}
/* }}} */

/* {{{ proto double tan(double number)
   Returns the tangent of the number in radians */
PHP_FUNCTION(tan)
{
	pval *num;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double(num);
	return_value->value.dval = tan(num->value.dval);
	return_value->type = IS_DOUBLE;
}
/* }}} */

/* {{{ proto double asin(double number)
   Returns the arc sine of the number in radians */
PHP_FUNCTION(asin)
{
	pval *num;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double(num);
	return_value->value.dval = asin(num->value.dval);
	return_value->type = IS_DOUBLE;
}
/* }}} */

/* {{{ proto double acos(double number)
   Return the arc cosine of the number in radians */
PHP_FUNCTION(acos)
{
	pval *num;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double(num);
	return_value->value.dval = acos(num->value.dval);
	return_value->type = IS_DOUBLE;
}
/* }}} */

/* {{{ proto double atan(double number)
   Returns the arc tangent of the number in radians */
PHP_FUNCTION(atan)
{
	pval *num;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double(num);
	return_value->value.dval = atan(num->value.dval);
	return_value->type = IS_DOUBLE;
}
/* }}} */

/* {{{ proto double atan2(double y, double x)
   Returns the arc tangent of y/x, with the resulting quadrant determined by the signs of y and x */
PHP_FUNCTION(atan2)
{
	pval *num1, *num2;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &num1, &num2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double(num1);
	convert_to_double(num2);
	return_value->value.dval = atan2(num1->value.dval,num2->value.dval);
	return_value->type = IS_DOUBLE;
}
/* }}} */

/* {{{ proto double pi(void)
   Returns an approximation of pi */
PHP_FUNCTION(pi)
{
	return_value->value.dval = M_PI;
	return_value->type = IS_DOUBLE;
}
/* }}} */

/* {{{ proto double pow(double base, double exponent)
   Returns base raised to the power of expopent */
PHP_FUNCTION(pow)
{
	pval *num1, *num2;
	
	if (ARG_COUNT(ht) != 2 || getParameters(ht,2,&num1,&num2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double(num1);
	convert_to_double(num2);
	RETURN_DOUBLE(pow(num1->value.dval, num2->value.dval));
}
/* }}} */

/* {{{ proto double exp(double number)
   Returns e raised to the power of the number */
PHP_FUNCTION(exp)
{
	pval *num;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double(num);
	return_value->value.dval = exp(num->value.dval);
	return_value->type = IS_DOUBLE;
}
/* }}} */

/* {{{ proto double log(double number)
   Returns the natural logarithm of the number */
PHP_FUNCTION(log)
{
	pval *num;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double(num);
	return_value->value.dval = log(num->value.dval);
	return_value->type = IS_DOUBLE;
}
/* }}} */

/* {{{ proto double log10(double number)
   Returns the base-10 logarithm of the number */
PHP_FUNCTION(log10)
{
	pval *num;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double(num);
	return_value->value.dval = log10(num->value.dval);
	return_value->type = IS_DOUBLE;
}
/* }}} */

/* {{{ proto double sqrt(double number)
   Returns the square root of the number */
PHP_FUNCTION(sqrt)
{
	pval *num;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double(num);
	return_value->value.dval = sqrt(num->value.dval);
	return_value->type = IS_DOUBLE;
}
/* }}} */

/* {{{ proto double deg2rad(double number)
   Converts the number in degrees to the radian equivalent  */
PHP_FUNCTION(deg2rad)
{
	pval *deg;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &deg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double(deg);
	RETVAL_DOUBLE((deg->value.dval / 180.0) * M_PI);
}
/* }}} */

/* {{{ proto double rad2deg(double number)
   Converts the radian number to the equivalent number in degrees */
PHP_FUNCTION(rad2deg)
{
	pval *rad;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &rad) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double(rad);
	RETVAL_DOUBLE((rad->value.dval / M_PI) * 180);
}
/* }}} */


/*
 * Convert a string representation of a base(2-36) number to a long.
 */
static long
_php3_basetolong(pval *arg, int base) {
	long mult = 1, num = 0, digit;
	int i;
	char c, *s;

	if (arg->type != IS_STRING || base < 2 || base > 36) {
		return 0;
	}

	s = arg->value.str.val;

	for (i = arg->value.str.len - 1; i >= 0; i--, mult *= base) {
		c = toupper(s[i]);
		if (c >= '0' && c <= '9') {
			digit = (c - '0');
		} else if (c >= 'A' && c <= 'Z') {
			digit = (c - 'A' + 10);
		} else {
			continue;
		}
		if (digit >= base) {
			continue;
		}
		num += mult * digit;
	}

	return num;
}


/*
 * Convert a long to a string containing a base(2-36) representation of
 * the number.
 */
static char *
_php3_longtobase(pval *arg, int base)
{
	static char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	char *result, *ptr, *ret;
	int len, digit;
	long value;

	if (arg->type != IS_LONG || base < 2 || base > 36) {
		return empty_string;
	}

	value = arg->value.lval;

	/* allocates space for the longest possible result with the lowest base */
	len = (sizeof(arg->value.lval) * 8) + 1;
	result = emalloc((sizeof(arg->value.lval) * 8) + 1);

	ptr = result + len - 1;
	*ptr-- = '\0';

	do {
		digit = value % base;
		*ptr = digits[digit];
		value /= base;
	}
	while (ptr-- > result && value);
	ptr++;
	ret = estrdup(ptr);
	efree(result);

	return ret;
}	

/* {{{ proto int bindec(string binary_number)
   Returns the decimal equivalent of the binary number */
PHP_FUNCTION(bindec)
{
	pval *arg;
	long ret;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string(arg);
	ret = _php3_basetolong(arg, 2);

	RETVAL_LONG(ret);
}
/* }}} */

/* {{{ proto int hexdec(string hexadimal_number)
   Returns the decimal equivalent of the hexadecimal number */
PHP_FUNCTION(hexdec)
{
	pval *arg;
	long ret;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string(arg);

	ret = _php3_basetolong(arg, 16);
	RETVAL_LONG(ret);
}
/* }}} */

/* {{{ proto int octdec(string octal_number)
   Returns the decimal equivalent of an octal string */
PHP_FUNCTION(octdec)
{
	pval *arg;
	long ret;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string(arg);

	ret = _php3_basetolong(arg, 8);
	RETVAL_LONG(ret);
}
/* }}} */

/* {{{ proto string decbin(int decimal_number)
   Returns a string containing a binary representation of the number */
PHP_FUNCTION(decbin)
{
	pval *arg;
	char *result;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg);

	result = _php3_longtobase(arg, 2);
	return_value->type = IS_STRING;
	return_value->value.str.len = strlen(result);
	return_value->value.str.val = result;
}
/* }}} */

/* {{{ proto string decoct(int decimal_number)
   Returns a string containing an octal representation of the given number */
PHP_FUNCTION(decoct)
{
	pval *arg;
	char *result;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg);

	result = _php3_longtobase(arg, 8);
	return_value->type = IS_STRING;
	return_value->value.str.len = strlen(result);
	return_value->value.str.val = result;
}
/* }}} */

/* {{{ proto string dechex(int decimal_number)
   Returns a string containing a hexadecimal representation of the given number */
PHP_FUNCTION(dechex)
{
	pval *arg;
	char *result;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg);

	result = _php3_longtobase(arg, 16);
	return_value->type = IS_STRING;
	return_value->value.str.len = strlen(result);
	return_value->value.str.val = result;
}
/* }}} */


/* {{{ proto string base_convert(string number, int frombase, int tobase)
   Converts a number in a string from any base <= 36 to any base <= 36.
*/
PHP_FUNCTION(base_convert)
{
	pval *number, *frombase, *tobase, temp;
	char *result;

	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &number, &frombase, &tobase)
		== FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(number);
	convert_to_long(frombase);
	convert_to_long(tobase);
	if (frombase->value.lval < 2 || frombase->value.lval > 36) {
		php3_error(E_WARNING, "base_convert: invalid `from base' (%d)",
				   frombase->value.lval);
		RETURN_FALSE;
	}
	if (tobase->value.lval < 2 || tobase->value.lval > 36) {
		php3_error(E_WARNING, "base_convert: invalid `to base' (%d)",
				   tobase->value.lval);
		RETURN_FALSE;
	}
	temp.type = IS_LONG;
	temp.value.lval = _php3_basetolong(number, frombase->value.lval);
	result = _php3_longtobase(&temp, tobase->value.lval);
	RETVAL_STRING(result, 0);
} /* }}} */


char *_php3_number_format(double d,int dec,char dec_point,char thousand_sep)
{
	char *tmpbuf,*resbuf;
	char *s,*t;  /* source, target */
	int tmplen,reslen=0;
	int count=0;
	int is_negative=0;
	
	if (d<0) {
		is_negative=1;
		d = -d;
	}
	dec = MAX(0,dec);
	tmpbuf = (char *) emalloc(32+dec);
	
	tmplen=_php3_sprintf(tmpbuf,"%.*f",dec,d);

	if (!isdigit(tmpbuf[0])) {
		return tmpbuf;
	}

	for (t=tmpbuf; *t; t++) {
		if (*t=='.') {
			*t = dec_point;
		}
	}
	if (dec) {
		reslen = dec+1 + (tmplen-dec-1) + (tmplen-1-dec-1)/3;
	} else {
		reslen = tmplen+(tmplen-1)/3;
	}
	if (is_negative) {
		reslen++;
	}
	resbuf = (char *) emalloc(reslen+1);
	
	s = tmpbuf+tmplen-1;
	t = resbuf+reslen;
	*t-- = 0;
	
	if (dec) {
		while (*s!=dec_point) {
			*t-- = *s--;
		}
		*t-- = *s--;  /* copy that dot */
	}
	
	while(s>=tmpbuf) {
		*t-- = *s--;
		if ((++count%3)==0 && s>=tmpbuf) {
			*t-- = thousand_sep;
		}
	}
	if (is_negative) {
		*t-- = '-';
	}
	efree(tmpbuf);
	return resbuf;
}

/* {{{ proto string number_format(double number, [,int num_decimal_places [, string  dec_seperator, string thousands_seperator)]])
   Formats a number with grouped thousands */
PHP_FUNCTION(number_format)
{
	pval *num,*dec,*t_s,*d_p;
	char thousand_sep=',', dec_point='.';
	
	switch(ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &num)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_double(num);
			RETURN_STRING(_php3_number_format(num->value.dval,0,dec_point,thousand_sep),0);
			break;
		case 2:
			if (getParameters(ht, 2, &num, &dec)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_double(num);
			convert_to_long(dec);
			RETURN_STRING(_php3_number_format(num->value.dval,dec->value.lval,dec_point,thousand_sep),0);
			break;
		case 4:
			if (getParameters(ht, 4, &num, &dec, &d_p, &t_s)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_double(num);
			convert_to_long(dec);
			convert_to_string(d_p);
			convert_to_string(t_s);
			if (d_p->value.str.len==1) {
				dec_point=d_p->value.str.val[0];
			}
			if (t_s->value.str.len==1) {
				thousand_sep=t_s->value.str.val[0];
			}
			RETURN_STRING(_php3_number_format(num->value.dval,dec->value.lval,dec_point,thousand_sep),0);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
}
/* }}} */
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
