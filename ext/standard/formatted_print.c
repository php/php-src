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
   | Author: Stig S�ther Bakken <ssb@php.net>                             |
   +----------------------------------------------------------------------+
 */

#include <math.h>				/* modf() */
#include "php.h"
#include "ext/standard/head.h"
#include "php_string.h"
#include "zend_execute.h"
#include <stdio.h>

#include <locale.h>
#ifdef ZTS
#include "ext/standard/php_string.h"
#define LCONV_DECIMAL_POINT (*lconv.decimal_point)
#else
#define LCONV_DECIMAL_POINT (*lconv->decimal_point)
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

static const char hexchars[] = "0123456789abcdef";
static const char HEXCHARS[] = "0123456789ABCDEF";

/* php_spintf_appendchar() {{{ */
inline static void
php_sprintf_appendchar(zend_string **buffer, size_t *pos, char add)
{
	if ((*pos + 1) >= ZSTR_LEN(*buffer)) {
		PRINTF_DEBUG(("%s(): ereallocing buffer to %d bytes\n", get_active_function_name(), ZSTR_LEN(*buffer)));
		*buffer = zend_string_extend(*buffer, ZSTR_LEN(*buffer) << 1, 0);
	}
	PRINTF_DEBUG(("sprintf: appending '%c', pos=\n", add, *pos));
	ZSTR_VAL(*buffer)[(*pos)++] = add;
}
/* }}} */

/* php_spintf_appendchar() {{{ */
inline static void
php_sprintf_appendchars(zend_string **buffer, size_t *pos, char *add, size_t len)
{
	if ((*pos + len) >= ZSTR_LEN(*buffer)) {
		size_t nlen = ZSTR_LEN(*buffer);

		PRINTF_DEBUG(("%s(): ereallocing buffer to %d bytes\n", get_active_function_name(), ZSTR_LEN(*buffer)));
		do {
			nlen = nlen << 1;
		} while ((*pos + len) >= nlen);
		*buffer = zend_string_extend(*buffer, nlen, 0);
	}
	PRINTF_DEBUG(("sprintf: appending \"%s\", pos=\n", add, *pos));
	memcpy(ZSTR_VAL(*buffer) + (*pos), add, len);
	*pos += len;
}
/* }}} */

/* php_spintf_appendstring() {{{ */
inline static void
php_sprintf_appendstring(zend_string **buffer, size_t *pos, char *add,
						   size_t min_width, size_t max_width, char padding,
						   size_t alignment, size_t len, bool neg, int expprec, int always_sign)
{
	size_t npad;
	size_t req_size;
	size_t copy_len;
	size_t m_width;

	copy_len = (expprec ? MIN(max_width, len) : len);
	npad = (min_width < copy_len) ? 0 : min_width - copy_len;

	PRINTF_DEBUG(("sprintf: appendstring(%x, %d, %d, \"%s\", %d, '%c', %d)\n",
				  *buffer, *pos, ZSTR_LEN(*buffer), add, min_width, padding, alignment));
	m_width = MAX(min_width, copy_len);

	if(m_width > INT_MAX - *pos - 1) {
		zend_error_noreturn(E_ERROR, "Field width %zd is too long", m_width);
	}

	req_size = *pos + m_width + 1;

	if (req_size > ZSTR_LEN(*buffer)) {
		size_t size = ZSTR_LEN(*buffer);
		while (req_size > size) {
			if (size > ZEND_SIZE_MAX/2) {
				zend_error_noreturn(E_ERROR, "Field width %zd is too long", req_size);
			}
			size <<= 1;
		}
		PRINTF_DEBUG(("sprintf ereallocing buffer to %d bytes\n", size));
		*buffer = zend_string_extend(*buffer, size, 0);
	}
	if (alignment == ALIGN_RIGHT) {
		if ((neg || always_sign) && padding=='0') {
			ZSTR_VAL(*buffer)[(*pos)++] = (neg) ? '-' : '+';
			add++;
			len--;
			copy_len--;
		}
		while (npad-- > 0) {
			ZSTR_VAL(*buffer)[(*pos)++] = padding;
		}
	}
	PRINTF_DEBUG(("sprintf: appending \"%s\"\n", add));
	memcpy(&ZSTR_VAL(*buffer)[*pos], add, copy_len + 1);
	*pos += copy_len;
	if (alignment == ALIGN_LEFT) {
		while (npad--) {
			ZSTR_VAL(*buffer)[(*pos)++] = padding;
		}
	}
}
/* }}} */

/* php_spintf_appendint() {{{ */
inline static void
php_sprintf_appendint(zend_string **buffer, size_t *pos, zend_long number,
						size_t width, char padding, size_t alignment,
						int always_sign)
{
	char numbuf[NUM_BUF_SIZE];
	zend_ulong magn, nmagn;
	unsigned int i = NUM_BUF_SIZE - 1, neg = 0;

	PRINTF_DEBUG(("sprintf: appendint(%x, %x, %x, %d, %d, '%c', %d)\n",
				  *buffer, pos, &ZSTR_LEN(*buffer), number, width, padding, alignment));
	if (number < 0) {
		neg = 1;
		magn = ((zend_ulong) -(number + 1)) + 1;
	} else {
		magn = (zend_ulong) number;
	}

	/* Can't right-pad 0's on integers */
	if(alignment==0 && padding=='0') padding=' ';

	numbuf[i] = '\0';

	do {
		nmagn = magn / 10;

		numbuf[--i] = (unsigned char)(magn - (nmagn * 10)) + '0';
		magn = nmagn;
	}
	while (magn > 0 && i > 1);
	if (neg) {
		numbuf[--i] = '-';
	} else if (always_sign) {
		numbuf[--i] = '+';
	}
	PRINTF_DEBUG(("sprintf: appending %d as \"%s\", i=%d\n",
				  number, &numbuf[i], i));
	php_sprintf_appendstring(buffer, pos, &numbuf[i], width, 0,
							 padding, alignment, (NUM_BUF_SIZE - 1) - i,
							 neg, 0, always_sign);
}
/* }}} */

/* php_spintf_appenduint() {{{ */
inline static void
php_sprintf_appenduint(zend_string **buffer, size_t *pos,
					   zend_ulong number,
					   size_t width, char padding, size_t alignment)
{
	char numbuf[NUM_BUF_SIZE];
	zend_ulong magn, nmagn;
	unsigned int i = NUM_BUF_SIZE - 1;

	PRINTF_DEBUG(("sprintf: appenduint(%x, %x, %x, %d, %d, '%c', %d)\n",
				  *buffer, pos, &ZSTR_LEN(*buffer), number, width, padding, alignment));
	magn = (zend_ulong) number;

	/* Can't right-pad 0's on integers */
	if (alignment == 0 && padding == '0') padding = ' ';

	numbuf[i] = '\0';

	do {
		nmagn = magn / 10;

		numbuf[--i] = (unsigned char)(magn - (nmagn * 10)) + '0';
		magn = nmagn;
	} while (magn > 0 && i > 0);

	PRINTF_DEBUG(("sprintf: appending %d as \"%s\", i=%d\n", number, &numbuf[i], i));
	php_sprintf_appendstring(buffer, pos, &numbuf[i], width, 0,
							 padding, alignment, (NUM_BUF_SIZE - 1) - i, /* neg */ false, 0, 0);
}
/* }}} */

/* php_spintf_appenddouble() {{{ */
inline static void
php_sprintf_appenddouble(zend_string **buffer, size_t *pos,
						 double number,
						 size_t width, char padding,
						 size_t alignment, int precision,
						 int adjust, char fmt,
						 int always_sign
						)
{
	char num_buf[NUM_BUF_SIZE];
	char *s = NULL;
	size_t s_len = 0;
	bool is_negative = false;
#ifdef ZTS
	struct lconv lconv;
#else
	struct lconv *lconv;
#endif

	PRINTF_DEBUG(("sprintf: appenddouble(%x, %x, %x, %f, %d, '%c', %d, %c)\n",
				  *buffer, pos, &ZSTR_LEN(*buffer), number, width, padding, alignment, fmt));
	if ((adjust & ADJ_PRECISION) == 0) {
		precision = FLOAT_PRECISION;
	} else if (precision > MAX_FLOAT_PRECISION) {
		php_error_docref(NULL, E_NOTICE, "Requested precision of %d digits was truncated to PHP maximum of %d digits", precision, MAX_FLOAT_PRECISION);
		precision = MAX_FLOAT_PRECISION;
	}

	if (zend_isnan(number)) {
		is_negative = (number<0);
		php_sprintf_appendstring(buffer, pos, "NaN", 3, 0, padding,
								 alignment, 3, is_negative, 0, always_sign);
		return;
	}

	if (zend_isinf(number)) {
		is_negative = (number<0);
		php_sprintf_appendstring(buffer, pos, "INF", 3, 0, padding,
								 alignment, 3, is_negative, 0, always_sign);
		return;
	}

	switch (fmt) {
		case 'e':
		case 'E':
		case 'f':
		case 'F':
#ifdef ZTS
			localeconv_r(&lconv);
#else
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
		case 'h':
		case 'H':
		{
			if (precision == 0)
				precision = 1;

			char decimal_point = '.';
			if (fmt == 'g' || fmt == 'G') {
#ifdef ZTS
				localeconv_r(&lconv);
#else
				lconv = localeconv();
#endif
				decimal_point = LCONV_DECIMAL_POINT;
			}

			char exp_char = fmt == 'G' || fmt == 'H' ? 'E' : 'e';
			/* We use &num_buf[ 1 ], so that we have room for the sign. */
			s = zend_gcvt(number, precision, decimal_point, exp_char, &num_buf[1]);
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
	}

	php_sprintf_appendstring(buffer, pos, s, width, 0, padding,
							 alignment, s_len, is_negative, 0, always_sign);
}
/* }}} */

/* php_spintf_appendd2n() {{{ */
inline static void
php_sprintf_append2n(zend_string **buffer, size_t *pos, zend_long number,
					 size_t width, char padding, size_t alignment, int n,
					 const char *chartable, int expprec)
{
	char numbuf[NUM_BUF_SIZE];
	zend_ulong num;
	zend_ulong  i = NUM_BUF_SIZE - 1;
	int andbits = (1 << n) - 1;

	PRINTF_DEBUG(("sprintf: append2n(%x, %x, %x, %d, %d, '%c', %d, %d, %x)\n",
				  *buffer, pos, &ZSTR_LEN(*buffer), number, width, padding, alignment, n,
				  chartable));
	PRINTF_DEBUG(("sprintf: append2n 2^%d andbits=%x\n", n, andbits));

	num = (zend_ulong) number;
	numbuf[i] = '\0';

	do {
		numbuf[--i] = chartable[(num & andbits)];
		num >>= n;
	}
	while (num > 0);

	php_sprintf_appendstring(buffer, pos, &numbuf[i], width, 0,
							 padding, alignment, (NUM_BUF_SIZE - 1) - i,
							 /* neg */ false, expprec, 0);
}
/* }}} */

/* php_spintf_getnumber() {{{ */
inline static int
php_sprintf_getnumber(char **buffer, size_t *len)
{
	char *endptr;
	zend_long num = ZEND_STRTOL(*buffer, &endptr, 10);
	size_t i;

	if (endptr != NULL) {
		i = (endptr - *buffer);
		*len -= i;
		*buffer = endptr;
	}
	PRINTF_DEBUG(("sprintf_getnumber: number was %d bytes long\n", i));

	if (num >= INT_MAX || num < 0) {
		return -1;
	} else {
		return (int) num;
	}
}
/* }}} */

#define ARG_NUM_NEXT -1
#define ARG_NUM_INVALID -2

int php_sprintf_get_argnum(char **format, size_t *format_len) {
	char *temppos = *format;
	while (isdigit((int) *temppos)) temppos++;
	if (*temppos != '$') {
		return ARG_NUM_NEXT;
	}

	int argnum = php_sprintf_getnumber(format, format_len);
	if (argnum <= 0) {
		zend_value_error("Argument number specifier must be greater than zero and less than %d", INT_MAX);
		return ARG_NUM_INVALID;
	}

	(*format)++;  /* skip the '$' */
	(*format_len)--;
	return argnum - 1;
}

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
 * nb_additional_parameters is used for throwing errors:
 *  - -1: ValueError is thrown (for vsprintf where args originates from an array)
 *  - 0 or more: ArgumentCountError is thrown
 */
static zend_string *
php_formatted_print(char *format, size_t format_len, zval *args, int argc, int nb_additional_parameters)
{
	size_t size = 240, outpos = 0;
	int alignment, currarg, adjusting, argnum, width, precision;
	char *temppos, padding;
	zend_string *result;
	int always_sign;
	int max_missing_argnum = -1;

	result = zend_string_alloc(size, 0);

	currarg = 0;
	argnum = 0;

	while (format_len) {
		int expprec;
		zval *tmp;

		temppos = memchr(format, '%', format_len);
		if (!temppos) {
			php_sprintf_appendchars(&result, &outpos, format, format_len);
			break;
		} else if (temppos != format) {
			php_sprintf_appendchars(&result, &outpos, format, temppos - format);
			format_len -= temppos - format;
			format = temppos;
		}
		format++;			/* skip the '%' */
		format_len--;

		if (*format == '%') {
			php_sprintf_appendchar(&result, &outpos, '%');
			format++;
			format_len--;
		} else {
			/* starting a new format specifier, reset variables */
			alignment = ALIGN_RIGHT;
			adjusting = 0;
			padding = ' ';
			always_sign = 0;
			expprec = 0;

			PRINTF_DEBUG(("sprintf: first looking at '%c', inpos=%d\n",
						  *format, format - Z_STRVAL_P(z_format)));
			if (isalpha((int)*format)) {
				width = precision = 0;
				argnum = ARG_NUM_NEXT;
			} else {
				/* first look for argnum */
				argnum = php_sprintf_get_argnum(&format, &format_len);
				if (argnum == ARG_NUM_INVALID) {
					goto fail;
				}

				/* after argnum comes modifiers */
				PRINTF_DEBUG(("sprintf: looking for modifiers\n"
							  "sprintf: now looking at '%c', inpos=%d\n",
							  *format, format - Z_STRVAL_P(z_format)));
				for (;; format++, format_len--) {
					if (*format == ' ' || *format == '0') {
						padding = *format;
					} else if (*format == '-') {
						alignment = ALIGN_LEFT;
						/* space padding, the default */
					} else if (*format == '+') {
						always_sign = 1;
					} else if (*format == '\'') {
						if (format_len > 1) {
							format++;
							format_len--;
							padding = *format;
						} else {
							zend_value_error("Missing padding character");
							goto fail;
						}
					} else {
						PRINTF_DEBUG(("sprintf: end of modifiers\n"));
						break;
					}
				}
				PRINTF_DEBUG(("sprintf: padding='%c'\n", padding));
				PRINTF_DEBUG(("sprintf: alignment=%s\n",
							  (alignment == ALIGN_LEFT) ? "left" : "right"));


				/* after modifiers comes width */
				if (*format == '*') {
					format++;
					format_len--;

					int width_argnum = php_sprintf_get_argnum(&format, &format_len);
					if (width_argnum == ARG_NUM_INVALID) {
						goto fail;
					}
					if (width_argnum == ARG_NUM_NEXT) {
						width_argnum = currarg++;
					}
					if (width_argnum >= argc) {
						max_missing_argnum = MAX(max_missing_argnum, width_argnum);
						continue;
					}
					tmp = &args[width_argnum];
					ZVAL_DEREF(tmp);
					if (Z_TYPE_P(tmp) != IS_LONG) {
						zend_value_error("Width must be an integer");
						goto fail;
					}
					if (Z_LVAL_P(tmp) < 0 || Z_LVAL_P(tmp) > INT_MAX) {
						zend_value_error("Width must be greater than zero and less than %d", INT_MAX);
						goto fail;
					}
					width = Z_LVAL_P(tmp);
					adjusting |= ADJ_WIDTH;
				} else if (isdigit((int)*format)) {
					PRINTF_DEBUG(("sprintf: getting width\n"));
					if ((width = php_sprintf_getnumber(&format, &format_len)) < 0) {
						zend_value_error("Width must be greater than zero and less than %d", INT_MAX);
						goto fail;
					}
					adjusting |= ADJ_WIDTH;
				} else {
					width = 0;
				}
				PRINTF_DEBUG(("sprintf: width=%d\n", width));

				/* after width and argnum comes precision */
				if (*format == '.') {
					format++;
					format_len--;
					PRINTF_DEBUG(("sprintf: getting precision\n"));
					if (*format == '*') {
						format++;
						format_len--;

						int prec_argnum = php_sprintf_get_argnum(&format, &format_len);
						if (prec_argnum == ARG_NUM_INVALID) {
							goto fail;
						}
						if (prec_argnum == ARG_NUM_NEXT) {
							prec_argnum = currarg++;
						}
						if (prec_argnum >= argc) {
							max_missing_argnum = MAX(max_missing_argnum, prec_argnum);
							continue;
						}
						tmp = &args[prec_argnum];
						ZVAL_DEREF(tmp);
						if (Z_TYPE_P(tmp) != IS_LONG) {
							zend_value_error("Precision must be an integer");
							goto fail;
						}
						if (Z_LVAL_P(tmp) < -1 || Z_LVAL_P(tmp) > INT_MAX) {
							zend_value_error("Precision must be between -1 and %d", INT_MAX);
							goto fail;
						}
						precision = Z_LVAL_P(tmp);
						adjusting |= ADJ_PRECISION;
						expprec = 1;
					} else if (isdigit((int)*format)) {
						if ((precision = php_sprintf_getnumber(&format, &format_len)) < 0) {
							zend_value_error("Precision must be greater than zero and less than %d", INT_MAX);
							goto fail;
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
			}

			if (*format == 'l') {
				format++;
				format_len--;
			}
			PRINTF_DEBUG(("sprintf: format character='%c'\n", *format));

			if (argnum == ARG_NUM_NEXT) {
				argnum = currarg++;
			}
			if (argnum >= argc) {
				max_missing_argnum = MAX(max_missing_argnum, argnum);
				continue;
			}

			if (expprec && precision == -1
					&& *format != 'g' && *format != 'G' && *format != 'h' && *format != 'H') {
				zend_value_error("Precision -1 is only supported for %%g, %%G, %%h and %%H");
				goto fail;
			}

			/* now we expect to find a type specifier */
			tmp = &args[argnum];
			switch (*format) {
				case 's': {
					zend_string *t;
					zend_string *str = zval_get_tmp_string(tmp, &t);
					php_sprintf_appendstring(&result, &outpos,
											 ZSTR_VAL(str),
											 width, precision, padding,
											 alignment,
											 ZSTR_LEN(str),
											 /* neg */ false, expprec, 0);
					zend_tmp_string_release(t);
					break;
				}

				case 'd':
					php_sprintf_appendint(&result, &outpos,
										  zval_get_long(tmp),
										  width, padding, alignment,
										  always_sign);
					break;

				case 'u':
					php_sprintf_appenduint(&result, &outpos,
										  zval_get_long(tmp),
										  width, padding, alignment);
					break;

				case 'e':
				case 'E':
				case 'f':
				case 'F':
				case 'g':
				case 'G':
				case 'h':
				case 'H':
					php_sprintf_appenddouble(&result, &outpos,
											 zval_get_double(tmp),
											 width, padding, alignment,
											 precision, adjusting,
											 *format, always_sign
											);
					break;

				case 'c':
					php_sprintf_appendchar(&result, &outpos,
										(char) zval_get_long(tmp));
					break;

				case 'o':
					php_sprintf_append2n(&result, &outpos,
										 zval_get_long(tmp),
										 width, padding, alignment, 3,
										 hexchars, expprec);
					break;

				case 'x':
					php_sprintf_append2n(&result, &outpos,
										 zval_get_long(tmp),
										 width, padding, alignment, 4,
										 hexchars, expprec);
					break;

				case 'X':
					php_sprintf_append2n(&result, &outpos,
										 zval_get_long(tmp),
										 width, padding, alignment, 4,
										 HEXCHARS, expprec);
					break;

				case 'b':
					php_sprintf_append2n(&result, &outpos,
										 zval_get_long(tmp),
										 width, padding, alignment, 1,
										 hexchars, expprec);
					break;

				case '%':
					php_sprintf_appendchar(&result, &outpos, '%');

					break;

				case '\0':
					if (!format_len) {
						zend_value_error("Missing format specifier at end of string");
						goto fail;
					}
					ZEND_FALLTHROUGH;

				default:
					zend_value_error("Unknown format specifier \"%c\"", *format);
					goto fail;
			}
			format++;
			format_len--;
		}
	}

	if (max_missing_argnum >= 0) {
		if (nb_additional_parameters == -1) {
			zend_value_error("The arguments array must contain %d items, %d given", max_missing_argnum + 1, argc);
		} else {
			zend_argument_count_error("%d arguments are required, %d given", max_missing_argnum + nb_additional_parameters + 1, argc + nb_additional_parameters);
		}
		goto fail;
	}

	/* possibly, we have to make sure we have room for the terminating null? */
	ZSTR_VAL(result)[outpos]=0;
	ZSTR_LEN(result) = outpos;
	return result;

fail:
	zend_string_efree(result);
	return NULL;
}
/* }}} */

/* php_formatted_print_get_array() {{{ */
static zval *php_formatted_print_get_array(zend_array *array, int *argc)
{
	zval *args, *zv;
	int n;

	n = zend_hash_num_elements(array);
	args = (zval *)safe_emalloc(n, sizeof(zval), 0);
	n = 0;
	ZEND_HASH_FOREACH_VAL(array, zv) {
		ZVAL_COPY_VALUE(&args[n], zv);
		n++;
	} ZEND_HASH_FOREACH_END();

	*argc = n;
	return args;
}
/* }}} */

/* {{{ Return a formatted string */
PHP_FUNCTION(sprintf)
{
	zend_string *result;
	char *format;
	size_t format_len;
	zval *args;
	int argc;

	ZEND_PARSE_PARAMETERS_START(1, -1)
		Z_PARAM_STRING(format, format_len)
		Z_PARAM_VARIADIC('*', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	result = php_formatted_print(format, format_len, args, argc, 1);
	if (result == NULL) {
		RETURN_THROWS();
	}
	RETVAL_STR(result);
}
/* }}} */

/* {{{ Return a formatted string */
PHP_FUNCTION(vsprintf)
{
	zend_string *result;
	char *format;
	size_t format_len;
	zval *args;
	zend_array *array;
	int argc;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STRING(format, format_len)
		Z_PARAM_ARRAY_HT(array)
	ZEND_PARSE_PARAMETERS_END();

	args = php_formatted_print_get_array(array, &argc);

	result = php_formatted_print(format, format_len, args, argc, -1);
	efree(args);
	if (result == NULL) {
		RETURN_THROWS();
	}
	RETVAL_STR(result);
}
/* }}} */

/* {{{ Output a formatted string */
PHP_FUNCTION(printf)
{
	zend_string *result;
	size_t rlen;
	char *format;
	size_t format_len;
	zval *args;
	int argc;

	ZEND_PARSE_PARAMETERS_START(1, -1)
		Z_PARAM_STRING(format, format_len)
		Z_PARAM_VARIADIC('*', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	result = php_formatted_print(format, format_len, args, argc, 1);
	if (result == NULL) {
		RETURN_THROWS();
	}
	rlen = PHPWRITE(ZSTR_VAL(result), ZSTR_LEN(result));
	zend_string_efree(result);
	RETURN_LONG(rlen);
}
/* }}} */

/* {{{ Output a formatted string */
PHP_FUNCTION(vprintf)
{
	zend_string *result;
	size_t rlen;
	char *format;
	size_t format_len;
	zval *args;
	zend_array *array;
	int argc;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STRING(format, format_len)
		Z_PARAM_ARRAY_HT(array)
	ZEND_PARSE_PARAMETERS_END();

	args = php_formatted_print_get_array(array, &argc);

	result = php_formatted_print(format, format_len, args, argc, -1);
	efree(args);
	if (result == NULL) {
		RETURN_THROWS();
	}
	rlen = PHPWRITE(ZSTR_VAL(result), ZSTR_LEN(result));
	zend_string_efree(result);
	RETURN_LONG(rlen);
}
/* }}} */

/* {{{ Output a formatted string into a stream */
PHP_FUNCTION(fprintf)
{
	php_stream *stream;
	char *format;
	size_t format_len;
	zval *arg1, *args;
	int argc;
	zend_string *result;

	ZEND_PARSE_PARAMETERS_START(2, -1)
		Z_PARAM_RESOURCE(arg1)
		Z_PARAM_STRING(format, format_len)
		Z_PARAM_VARIADIC('*', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	php_stream_from_zval(stream, arg1);

	result = php_formatted_print(format, format_len, args, argc, 2);
	if (result == NULL) {
		RETURN_THROWS();
	}

	php_stream_write(stream, ZSTR_VAL(result), ZSTR_LEN(result));

	RETVAL_LONG(ZSTR_LEN(result));
	zend_string_efree(result);
}
/* }}} */

/* {{{ Output a formatted string into a stream */
PHP_FUNCTION(vfprintf)
{
	php_stream *stream;
	char *format;
	size_t format_len;
	zval *arg1, *args;
	zend_array *array;
	int argc;
	zend_string *result;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_RESOURCE(arg1)
		Z_PARAM_STRING(format, format_len)
		Z_PARAM_ARRAY_HT(array)
	ZEND_PARSE_PARAMETERS_END();

	php_stream_from_zval(stream, arg1);

	args = php_formatted_print_get_array(array, &argc);

	result = php_formatted_print(format, format_len, args, argc, -1);
	efree(args);
	if (result == NULL) {
		RETURN_THROWS();
	}

	php_stream_write(stream, ZSTR_VAL(result), ZSTR_LEN(result));

	RETVAL_LONG(ZSTR_LEN(result));
	zend_string_efree(result);
}
/* }}} */
