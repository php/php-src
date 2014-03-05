/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Stig Sæther Bakken <ssb@php.net>                             |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include <math.h>				/* modf() */
#include "php.h"
#include "ext/standard/head.h"
#include "php_string.h"
#include "zend_execute.h"
#include <stdio.h>

#ifdef HAVE_LOCALE_H
#include <locale.h>
#define LCONV_DECIMAL_POINT (*lconv->decimal_point)
#else
#define LCONV_DECIMAL_POINT '.'
#endif

#define ALIGN_LEFT 0
#define ALIGN_RIGHT 1
#define ADJ_WIDTH 1
#define ADJ_PRECISION 2
#define NUM_BUF_SIZE 500
#define FLOAT_PRECISION 6
#define MAX_FLOAT_PRECISION 53

#if 0
/* trick to control varargs functions through cpp */
# define PRINTF_DEBUG(arg) php_printf arg
#else
# define PRINTF_DEBUG(arg)
#endif

static char hexchars[] = "0123456789abcdef";
static char HEXCHARS[] = "0123456789ABCDEF";

/* php_spintf_appendchar() {{{ */
inline static void
php_sprintf_appendchar(char **buffer, int *pos, int *size, char add TSRMLS_DC)
{
	if ((*pos + 1) >= *size) {
		*size <<= 1;
		PRINTF_DEBUG(("%s(): ereallocing buffer to %d bytes\n", get_active_function_name(TSRMLS_C), *size));
		*buffer = erealloc(*buffer, *size);
	}
	PRINTF_DEBUG(("sprintf: appending '%c', pos=\n", add, *pos));
	(*buffer)[(*pos)++] = add;
}
/* }}} */

/* php_spintf_appendstring() {{{ */
inline static void
php_sprintf_appendstring(char **buffer, int *pos, int *size, char *add,
						   int min_width, int max_width, char padding,
						   int alignment, int len, int neg, int expprec, int always_sign)
{
	register int npad;
	int req_size;
	int copy_len;
	int m_width;

	copy_len = (expprec ? MIN(max_width, len) : len);
	npad = min_width - copy_len;

	if (npad < 0) {
		npad = 0;
	}
	
	PRINTF_DEBUG(("sprintf: appendstring(%x, %d, %d, \"%s\", %d, '%c', %d)\n",
				  *buffer, *pos, *size, add, min_width, padding, alignment));
	m_width = MAX(min_width, copy_len);

	if(m_width > INT_MAX - *pos - 1) {
		zend_error_noreturn(E_ERROR, "Field width %d is too long", m_width);
	}

	req_size = *pos + m_width + 1;

	if (req_size > *size) {
		while (req_size > *size) {
			if(*size > INT_MAX/2) {
				zend_error_noreturn(E_ERROR, "Field width %d is too long", req_size); 
			}
			*size <<= 1;
		}
		PRINTF_DEBUG(("sprintf ereallocing buffer to %d bytes\n", *size));
		*buffer = erealloc(*buffer, *size);
	}
	if (alignment == ALIGN_RIGHT) {
		if ((neg || always_sign) && padding=='0') {
			(*buffer)[(*pos)++] = (neg) ? '-' : '+';
			add++;
			len--;
			copy_len--;
		}
		while (npad-- > 0) {
			(*buffer)[(*pos)++] = padding;
		}
	}
	PRINTF_DEBUG(("sprintf: appending \"%s\"\n", add));
	memcpy(&(*buffer)[*pos], add, copy_len + 1);
	*pos += copy_len;
	if (alignment == ALIGN_LEFT) {
		while (npad--) {
			(*buffer)[(*pos)++] = padding;
		}
	}
}
/* }}} */

/* php_spintf_appendint() {{{ */
inline static void
php_sprintf_appendint(char **buffer, int *pos, int *size, long number,
						int width, char padding, int alignment, 
						int always_sign)
{
	char numbuf[NUM_BUF_SIZE];
	register unsigned long magn, nmagn;
	register unsigned int i = NUM_BUF_SIZE - 1, neg = 0;

	PRINTF_DEBUG(("sprintf: appendint(%x, %x, %x, %d, %d, '%c', %d)\n",
				  *buffer, pos, size, number, width, padding, alignment));
	if (number < 0) {
		neg = 1;
		magn = ((unsigned long) -(number + 1)) + 1;
	} else {
		magn = (unsigned long) number;
	}

	/* Can't right-pad 0's on integers */
	if(alignment==0 && padding=='0') padding=' ';

	numbuf[i] = '\0';

	do {
		nmagn = magn / 10;

		numbuf[--i] = (unsigned char)(magn - (nmagn * 10)) + '0';
		magn = nmagn;
	}
	while (magn > 0 && i > 0);
	if (neg) {
		numbuf[--i] = '-';
	} else if (always_sign) {
		numbuf[--i] = '+';
	}
	PRINTF_DEBUG(("sprintf: appending %d as \"%s\", i=%d\n",
				  number, &numbuf[i], i));
	php_sprintf_appendstring(buffer, pos, size, &numbuf[i], width, 0,
							 padding, alignment, (NUM_BUF_SIZE - 1) - i,
							 neg, 0, always_sign);
}
/* }}} */

/* php_spintf_appenduint() {{{ */
inline static void
php_sprintf_appenduint(char **buffer, int *pos, int *size,
					   unsigned long number,
					   int width, char padding, int alignment)
{
	char numbuf[NUM_BUF_SIZE];
	register unsigned long magn, nmagn;
	register unsigned int i = NUM_BUF_SIZE - 1;

	PRINTF_DEBUG(("sprintf: appenduint(%x, %x, %x, %d, %d, '%c', %d)\n",
				  *buffer, pos, size, number, width, padding, alignment));
	magn = (unsigned long) number;

	/* Can't right-pad 0's on integers */
	if (alignment == 0 && padding == '0') padding = ' ';

	numbuf[i] = '\0';

	do {
		nmagn = magn / 10;

		numbuf[--i] = (unsigned char)(magn - (nmagn * 10)) + '0';
		magn = nmagn;
	} while (magn > 0 && i > 0);

	PRINTF_DEBUG(("sprintf: appending %d as \"%s\", i=%d\n", number, &numbuf[i], i));
	php_sprintf_appendstring(buffer, pos, size, &numbuf[i], width, 0,
							 padding, alignment, (NUM_BUF_SIZE - 1) - i, 0, 0, 0);
}
/* }}} */

/* php_spintf_appenddouble() {{{ */
inline static void
php_sprintf_appenddouble(char **buffer, int *pos,
						 int *size, double number,
						 int width, char padding,
						 int alignment, int precision,
						 int adjust, char fmt,
						 int always_sign
						 TSRMLS_DC)
{
	char num_buf[NUM_BUF_SIZE];
	char *s = NULL;
	int s_len = 0, is_negative = 0;
#ifdef HAVE_LOCALE_H
	struct lconv *lconv;
#endif

	PRINTF_DEBUG(("sprintf: appenddouble(%x, %x, %x, %f, %d, '%c', %d, %c)\n",
				  *buffer, pos, size, number, width, padding, alignment, fmt));
	if ((adjust & ADJ_PRECISION) == 0) {
		precision = FLOAT_PRECISION;
	} else if (precision > MAX_FLOAT_PRECISION) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Requested precision of %d digits was truncated to PHP maximum of %d digits", precision, MAX_FLOAT_PRECISION);
		precision = MAX_FLOAT_PRECISION;
	}
	
	if (zend_isnan(number)) {
		is_negative = (number<0);
		php_sprintf_appendstring(buffer, pos, size, "NaN", 3, 0, padding,
								 alignment, 3, is_negative, 0, always_sign);
		return;
	}

	if (zend_isinf(number)) {
		is_negative = (number<0);
		php_sprintf_appendstring(buffer, pos, size, "INF", 3, 0, padding,
								 alignment, 3, is_negative, 0, always_sign);
		return;
	}

	switch (fmt) {			
		case 'e':
		case 'E':
		case 'f':
		case 'F':
#ifdef HAVE_LOCALE_H
			lconv = localeconv();
#endif
			s = php_conv_fp((fmt == 'f')?'F':fmt, number, 0, precision,
						(fmt == 'f')?LCONV_DECIMAL_POINT:'.',
						&is_negative, &num_buf[1], &s_len);
			if (is_negative) {
				num_buf[0] = '-';
				s = num_buf;
				s_len++;
			} else if (always_sign) {
				num_buf[0] = '+';
				s = num_buf;
				s_len++;
			}
			break;

		case 'g':
		case 'G':
			if (precision == 0)
				precision = 1;
			/*
			 * * We use &num_buf[ 1 ], so that we have room for the sign
			 */
#ifdef HAVE_LOCALE_H
			lconv = localeconv();
#endif
			s = php_gcvt(number, precision, LCONV_DECIMAL_POINT, (fmt == 'G')?'E':'e', &num_buf[1]);
			is_negative = 0;
			if (*s == '-') {
				is_negative = 1;
				s = &num_buf[1];
			} else if (always_sign) {
				num_buf[0] = '+';
				s = num_buf;
			}

			s_len = strlen(s);
			break;
	}

	php_sprintf_appendstring(buffer, pos, size, s, width, 0, padding,
							 alignment, s_len, is_negative, 0, always_sign);
}
/* }}} */

/* php_spintf_appendd2n() {{{ */
inline static void
php_sprintf_append2n(char **buffer, int *pos, int *size, long number,
					 int width, char padding, int alignment, int n,
					 char *chartable, int expprec)
{
	char numbuf[NUM_BUF_SIZE];
	register unsigned long num;
	register unsigned int  i = NUM_BUF_SIZE - 1;
	register int andbits = (1 << n) - 1;

	PRINTF_DEBUG(("sprintf: append2n(%x, %x, %x, %d, %d, '%c', %d, %d, %x)\n",
				  *buffer, pos, size, number, width, padding, alignment, n,
				  chartable));
	PRINTF_DEBUG(("sprintf: append2n 2^%d andbits=%x\n", n, andbits));

	num = (unsigned long) number;
	numbuf[i] = '\0';

	do {
		numbuf[--i] = chartable[(num & andbits)];
		num >>= n;
	}
	while (num > 0);

	php_sprintf_appendstring(buffer, pos, size, &numbuf[i], width, 0,
							 padding, alignment, (NUM_BUF_SIZE - 1) - i,
							 0, expprec, 0);
}
/* }}} */

/* php_spintf_getnumber() {{{ */
inline static int
php_sprintf_getnumber(char *buffer, int *pos)
{
	char *endptr;
	register long num = strtol(&buffer[*pos], &endptr, 10);
	register int i = 0;

	if (endptr != NULL) {
		i = (endptr - &buffer[*pos]);
	}
	PRINTF_DEBUG(("sprintf_getnumber: number was %d bytes long\n", i));
	*pos += i;

	if (num >= INT_MAX || num < 0) {
		return -1;
	} else {
		return (int) num;
	}
}
/* }}} */

/* php_formatted_print() {{{
 * New sprintf implementation for PHP.
 *
 * Modifiers:
 *
 *  " "   pad integers with spaces
 *  "-"   left adjusted field
 *   n    field size
 *  "."n  precision (floats only)
 *  "+"   Always place a sign (+ or -) in front of a number
 *
 * Type specifiers:
 *
 *  "%"   literal "%", modifiers are ignored.
 *  "b"   integer argument is printed as binary
 *  "c"   integer argument is printed as a single character
 *  "d"   argument is an integer
 *  "f"   the argument is a float
 *  "o"   integer argument is printed as octal
 *  "s"   argument is a string
 *  "x"   integer argument is printed as lowercase hexadecimal
 *  "X"   integer argument is printed as uppercase hexadecimal
 *
 */
static char *
php_formatted_print(int ht, int *len, int use_array, int format_offset TSRMLS_DC)
{
	zval ***args, **z_format;
	int argc, size = 240, inpos = 0, outpos = 0, temppos;
	int alignment, currarg, adjusting, argnum, width, precision;
	char *format, *result, padding;
	int always_sign;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "+", &args, &argc) == FAILURE) {
		return NULL;
	}

	/* verify the number of args */
	if ((use_array && argc != (2 + format_offset)) 
			|| (!use_array && argc < (1 + format_offset))) {
		efree(args);
		WRONG_PARAM_COUNT_WITH_RETVAL(NULL);
	}
	
	if (use_array) {
		int i = 1;
		zval ***newargs;
		zval **array;

		z_format = args[format_offset];
		array = args[1 + format_offset];
		
		SEPARATE_ZVAL(array);
		convert_to_array_ex(array);
		
		argc = 1 + zend_hash_num_elements(Z_ARRVAL_PP(array));
		newargs = (zval ***)safe_emalloc(argc, sizeof(zval *), 0);
		newargs[0] = z_format;
		
		for (zend_hash_internal_pointer_reset(Z_ARRVAL_PP(array));
			 zend_hash_get_current_data(Z_ARRVAL_PP(array), (void **)&newargs[i++]) == SUCCESS;
			 zend_hash_move_forward(Z_ARRVAL_PP(array)));

		efree(args);
		args = newargs;
		format_offset = 0;
	}
	
	convert_to_string_ex(args[format_offset]);
	format = Z_STRVAL_PP(args[format_offset]);
	result = emalloc(size);

	currarg = 1;

	while (inpos<Z_STRLEN_PP(args[format_offset])) {
		int expprec = 0, multiuse = 0;
		zval *tmp;

		PRINTF_DEBUG(("sprintf: format[%d]='%c'\n", inpos, format[inpos]));
		PRINTF_DEBUG(("sprintf: outpos=%d\n", outpos));
		if (format[inpos] != '%') {
			php_sprintf_appendchar(&result, &outpos, &size, format[inpos++] TSRMLS_CC);
		} else if (format[inpos + 1] == '%') {
			php_sprintf_appendchar(&result, &outpos, &size, '%' TSRMLS_CC);
			inpos += 2;
		} else {
			/* starting a new format specifier, reset variables */
			alignment = ALIGN_RIGHT;
			adjusting = 0;
			padding = ' ';
			always_sign = 0;
			inpos++;			/* skip the '%' */

			PRINTF_DEBUG(("sprintf: first looking at '%c', inpos=%d\n",
						  format[inpos], inpos));
			if (isascii((int)format[inpos]) && !isalpha((int)format[inpos])) {
				/* first look for argnum */
				temppos = inpos;
				while (isdigit((int)format[temppos])) temppos++;
				if (format[temppos] == '$') {
					argnum = php_sprintf_getnumber(format, &inpos);

					if (argnum <= 0) {
						efree(result);
						efree(args);
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Argument number must be greater than zero");
						return NULL;
					}

					multiuse = 1;
					inpos++;  /* skip the '$' */
				} else {
					argnum = currarg++;
				}

				argnum += format_offset;

				/* after argnum comes modifiers */
				PRINTF_DEBUG(("sprintf: looking for modifiers\n"
							  "sprintf: now looking at '%c', inpos=%d\n",
							  format[inpos], inpos));
				for (;; inpos++) {
					if (format[inpos] == ' ' || format[inpos] == '0') {
						padding = format[inpos];
					} else if (format[inpos] == '-') {
						alignment = ALIGN_LEFT;
						/* space padding, the default */
					} else if (format[inpos] == '+') {
						always_sign = 1;
					} else if (format[inpos] == '\'') {
						padding = format[++inpos];
					} else {
						PRINTF_DEBUG(("sprintf: end of modifiers\n"));
						break;
					}
				}
				PRINTF_DEBUG(("sprintf: padding='%c'\n", padding));
				PRINTF_DEBUG(("sprintf: alignment=%s\n",
							  (alignment == ALIGN_LEFT) ? "left" : "right"));


				/* after modifiers comes width */
				if (isdigit((int)format[inpos])) {
					PRINTF_DEBUG(("sprintf: getting width\n"));
					if ((width = php_sprintf_getnumber(format, &inpos)) < 0) {
						efree(result);
						efree(args);
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Width must be greater than zero and less than %d", INT_MAX);
						return NULL;
					}
					adjusting |= ADJ_WIDTH;
				} else {
					width = 0;
				}
				PRINTF_DEBUG(("sprintf: width=%d\n", width));

				/* after width and argnum comes precision */
				if (format[inpos] == '.') {
					inpos++;
					PRINTF_DEBUG(("sprintf: getting precision\n"));
					if (isdigit((int)format[inpos])) {
						if ((precision = php_sprintf_getnumber(format, &inpos)) < 0) {
							efree(result);
							efree(args);
							php_error_docref(NULL TSRMLS_CC, E_WARNING, "Precision must be greater than zero and less than %d", INT_MAX);
							return NULL;
						}
						adjusting |= ADJ_PRECISION;
						expprec = 1;
					} else {
						precision = 0;
					}
				} else {
					precision = 0;
				}
				PRINTF_DEBUG(("sprintf: precision=%d\n", precision));
			} else {
				width = precision = 0;
				argnum = currarg++ + format_offset;
			}

			if (argnum >= argc) {
				efree(result);
				efree(args);
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Too few arguments");
				return NULL;
			}

			if (format[inpos] == 'l') {
				inpos++;
			}
			PRINTF_DEBUG(("sprintf: format character='%c'\n", format[inpos]));
			/* now we expect to find a type specifier */
			if (multiuse) {
				MAKE_STD_ZVAL(tmp);
				*tmp = **(args[argnum]);
				INIT_PZVAL(tmp);
				zval_copy_ctor(tmp);
			} else {
				SEPARATE_ZVAL(args[argnum]);
				tmp = *(args[argnum]);
			}

			switch (format[inpos]) {
				case 's': {
					zval *var, var_copy;
					int use_copy;

					zend_make_printable_zval(tmp, &var_copy, &use_copy);
					if (use_copy) {
						var = &var_copy;
					} else {
						var = tmp;
					}
					php_sprintf_appendstring(&result, &outpos, &size,
											 Z_STRVAL_P(var),
											 width, precision, padding,
											 alignment,
											 Z_STRLEN_P(var),
											 0, expprec, 0);
					if (use_copy) {
						zval_dtor(&var_copy);
					}
					break;
				}

				case 'd':
					convert_to_long(tmp);
					php_sprintf_appendint(&result, &outpos, &size,
										  Z_LVAL_P(tmp),
										  width, padding, alignment,
										  always_sign);
					break;

				case 'u':
					convert_to_long(tmp);
					php_sprintf_appenduint(&result, &outpos, &size,
										  Z_LVAL_P(tmp),
										  width, padding, alignment);
					break;

				case 'g':
				case 'G':
				case 'e':
				case 'E':
				case 'f':
				case 'F':
					convert_to_double(tmp);
					php_sprintf_appenddouble(&result, &outpos, &size,
											 Z_DVAL_P(tmp),
											 width, padding, alignment,
											 precision, adjusting,
											 format[inpos], always_sign
											 TSRMLS_CC);
					break;
					
				case 'c':
					convert_to_long(tmp);
					php_sprintf_appendchar(&result, &outpos, &size,
										(char) Z_LVAL_P(tmp) TSRMLS_CC);
					break;

				case 'o':
					convert_to_long(tmp);
					php_sprintf_append2n(&result, &outpos, &size,
										 Z_LVAL_P(tmp),
										 width, padding, alignment, 3,
										 hexchars, expprec);
					break;

				case 'x':
					convert_to_long(tmp);
					php_sprintf_append2n(&result, &outpos, &size,
										 Z_LVAL_P(tmp),
										 width, padding, alignment, 4,
										 hexchars, expprec);
					break;

				case 'X':
					convert_to_long(tmp);
					php_sprintf_append2n(&result, &outpos, &size,
										 Z_LVAL_P(tmp),
										 width, padding, alignment, 4,
										 HEXCHARS, expprec);
					break;

				case 'b':
					convert_to_long(tmp);
					php_sprintf_append2n(&result, &outpos, &size,
										 Z_LVAL_P(tmp),
										 width, padding, alignment, 1,
										 hexchars, expprec);
					break;

				case '%':
					php_sprintf_appendchar(&result, &outpos, &size, '%' TSRMLS_CC);

					break;
				default:
					break;
			}
			if (multiuse) {
				zval_ptr_dtor(&tmp);
			}
			inpos++;
		}
	}
	
	efree(args);
	
	/* possibly, we have to make sure we have room for the terminating null? */
	result[outpos]=0;
	*len = outpos;	
	return result;
}
/* }}} */

/* {{{ proto string sprintf(string format [, mixed arg1 [, mixed ...]])
   Return a formatted string */
PHP_FUNCTION(user_sprintf)
{
	char *result;
	int len;
	
	if ((result=php_formatted_print(ht, &len, 0, 0 TSRMLS_CC))==NULL) {
		RETURN_FALSE;
	}
	RETVAL_STRINGL(result, len, 0);
}
/* }}} */

/* {{{ proto string vsprintf(string format, array args)
   Return a formatted string */
PHP_FUNCTION(vsprintf)
{
	char *result;
	int len;
	
	if ((result=php_formatted_print(ht, &len, 1, 0 TSRMLS_CC))==NULL) {
		RETURN_FALSE;
	}
	RETVAL_STRINGL(result, len, 0);
}
/* }}} */

/* {{{ proto int printf(string format [, mixed arg1 [, mixed ...]])
   Output a formatted string */
PHP_FUNCTION(user_printf)
{
	char *result;
	int len, rlen;
	
	if ((result=php_formatted_print(ht, &len, 0, 0 TSRMLS_CC))==NULL) {
		RETURN_FALSE;
	}
	rlen = PHPWRITE(result, len);
	efree(result);
	RETURN_LONG(rlen);
}
/* }}} */

/* {{{ proto int vprintf(string format, array args)
   Output a formatted string */
PHP_FUNCTION(vprintf)
{
	char *result;
	int len, rlen;
	
	if ((result=php_formatted_print(ht, &len, 1, 0 TSRMLS_CC))==NULL) {
		RETURN_FALSE;
	}
	rlen = PHPWRITE(result, len);
	efree(result);
	RETURN_LONG(rlen);
}
/* }}} */

/* {{{ proto int fprintf(resource stream, string format [, mixed arg1 [, mixed ...]])
   Output a formatted string into a stream */
PHP_FUNCTION(fprintf)
{
	php_stream *stream;
	zval *arg1;
	char *result;
	int len;
	
	if (ZEND_NUM_ARGS() < 2) {
		WRONG_PARAM_COUNT;
	}
	
	if (zend_parse_parameters(1 TSRMLS_CC, "r", &arg1) == FAILURE) {
		RETURN_FALSE;
	}
	
	php_stream_from_zval(stream, &arg1);

	if ((result=php_formatted_print(ht, &len, 0, 1 TSRMLS_CC))==NULL) {
		RETURN_FALSE;
	}

	php_stream_write(stream, result, len);

	efree(result);

	RETURN_LONG(len);
}
/* }}} */

/* {{{ proto int vfprintf(resource stream, string format, array args)
   Output a formatted string into a stream */
PHP_FUNCTION(vfprintf)
{
	php_stream *stream;
	zval *arg1;
	char *result;
	int len;
	
	if (ZEND_NUM_ARGS() != 3) {
		WRONG_PARAM_COUNT;
	}
	
	if (zend_parse_parameters(1 TSRMLS_CC, "r", &arg1) == FAILURE) {
		RETURN_FALSE;
	}
	
	php_stream_from_zval(stream, &arg1);

	if ((result=php_formatted_print(ht, &len, 1, 1 TSRMLS_CC))==NULL) {
		RETURN_FALSE;
	}

	php_stream_write(stream, result, len);

	efree(result);

	RETURN_LONG(len);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
