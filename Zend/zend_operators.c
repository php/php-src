/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2008 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include <ctype.h>

#include "zend.h"
#include "zend_operators.h"
#include "zend_variables.h"
#include "zend_globals.h"
#include "zend_list.h"
#include "zend_API.h"
#include "zend_multiply.h"
#include "zend_strtod.h"
#include "zend_exceptions.h"
#include "zend_float.h"

#include "unicode/uchar.h"
#include "unicode/ucol.h"

#define LONG_SIGN_MASK (1L << (8*sizeof(long)-1))

#if ZEND_USE_TOLOWER_L
#include <locale.h>
static _locale_t current_locale = NULL;
/* this is true global! may lead to strange effects on ZTS, but so may setlocale() */
#define zend_tolower(c) _tolower_l(c, current_locale)
#else
#define zend_tolower(c) tolower(c)
#endif

#define TYPE_PAIR(t1,t2) (((t1) << 4) | (t2))

ZEND_API int zend_atoi(const char *str, int str_len) /* {{{ */
{
	int retval;

	if (!str_len) {
		str_len = strlen(str);
	}
	retval = strtol(str, NULL, 0);
	if (str_len>0) {
		switch (str[str_len-1]) {
			case 'g':
			case 'G':
				retval *= 1024;
				/* break intentionally missing */
			case 'm':
			case 'M':
				retval *= 1024;
				/* break intentionally missing */
			case 'k':
			case 'K':
				retval *= 1024;
				break;
		}
	}
	return retval;
}
/* }}} */

ZEND_API long zend_atol(const char *str, int str_len) /* {{{ */
{
	long retval;

	if (!str_len) {
		str_len = strlen(str);
	}
	retval = strtol(str, NULL, 0);
	if (str_len>0) {
		switch (str[str_len-1]) {
			case 'g':
			case 'G':
				retval *= 1024;
				/* break intentionally missing */
			case 'm':
			case 'M':
				retval *= 1024;
				/* break intentionally missing */
			case 'k':
			case 'K':
				retval *= 1024;
				break;
		}
	}
	return retval;
}
/* }}} */

ZEND_API double zend_string_to_double(const char *number, zend_uint length) /* {{{ */
{
	double divisor = 10.0;
	double result = 0.0;
	double exponent;
	const char *end = number+length;
	const char *digit = number;

	if (!length) {
		return result;
	}

	while (digit < end) {
		if ((*digit <= '9' && *digit >= '0')) {
			result *= 10;
			result += *digit - '0';
		} else if (*digit == '.') {
			digit++;
			break;
		} else if (toupper(*digit) == 'E') {
			exponent = (double) atoi(digit+1);
			result *= pow(10.0, exponent);
			return result;
		} else {
			return result;
		}
		digit++;
	}

	while (digit < end) {
		if ((*digit <= '9' && *digit >= '0')) {
			result += (*digit - '0') / divisor;
			divisor *= 10;
		} else if (toupper(*digit) == 'E') {
			exponent = (double) atoi(digit+1);
			result *= pow(10.0, exponent);
			return result;
		} else {
			return result;
		}
		digit++;
	}
	return result;
}
/* }}} */

ZEND_API int convert_scalar_to_number(zval *op TSRMLS_DC) /* {{{ */
{
	switch (Z_TYPE_P(op)) {
		case IS_STRING:
			{
				char *strval;

				strval = Z_STRVAL_P(op);
				switch ((Z_TYPE_P(op)=is_numeric_string(strval, Z_STRLEN_P(op), &Z_LVAL_P(op), &Z_DVAL_P(op), 1))) {
					case IS_DOUBLE:
					case IS_LONG:
						break;
					default:
						Z_LVAL_P(op) = strtol(Z_STRVAL_P(op), NULL, 10);
						Z_TYPE_P(op) = IS_LONG;
						break;
				}
				STR_FREE(strval);
				break;
			}
		case IS_UNICODE:
			{
				UChar *strval;

				strval = Z_USTRVAL_P(op);
				switch ((Z_TYPE_P(op)=is_numeric_unicode(strval, Z_USTRLEN_P(op), &Z_LVAL_P(op), &Z_DVAL_P(op), 1))) {
					case IS_DOUBLE:
					case IS_LONG:
						break;
					default:
						Z_LVAL_P(op) = zend_u_strtol(Z_USTRVAL_P(op), NULL, 10);
						Z_TYPE_P(op) = IS_LONG;
						break;
				}
				USTR_FREE(strval);
				break;
			}
			break;
		case IS_BOOL:
			Z_TYPE_P(op) = IS_LONG;
			break;
		case IS_RESOURCE:
			zend_list_delete(Z_LVAL_P(op));
			Z_TYPE_P(op) = IS_LONG;
			break;
		case IS_OBJECT:
			convert_to_long_base(op, 10);
			break;
		case IS_NULL:
			Z_TYPE_P(op) = IS_LONG;
			Z_LVAL_P(op) = 0;
			break;
	}

	return SUCCESS;
}
/* }}} */

/* {{{ zendi_convert_scalar_to_number */
#define zendi_convert_scalar_to_number(op, holder, result)			\
	if (op==result) {												\
		if (Z_TYPE_P(op) != IS_LONG) {								\
			convert_scalar_to_number(op TSRMLS_CC);					\
		}															\
	} else {														\
		switch (Z_TYPE_P(op)) {										\
			case IS_STRING:											\
				{													\
					switch ((Z_TYPE(holder)=is_numeric_string(Z_STRVAL_P(op), Z_STRLEN_P(op), &Z_LVAL(holder), &Z_DVAL(holder), 1))) {	\
						case IS_DOUBLE:										\
						case IS_LONG:										\
							break;											\
						default:											\
							Z_LVAL(holder) = strtol(Z_STRVAL_P(op), NULL, 10);	\
							Z_TYPE(holder) = IS_LONG;						\
							break;											\
					}														\
					(op) = &(holder);										\
					break;													\
				}															\
			case IS_UNICODE: \
				{ \
					switch ((Z_TYPE(holder)=is_numeric_unicode(Z_USTRVAL_P(op), Z_USTRLEN_P(op), &Z_LVAL(holder), &Z_DVAL(holder), 1))) { \
						case IS_DOUBLE: \
						case IS_LONG: \
							break; \
						default: \
							Z_LVAL(holder) = zend_u_strtol(Z_USTRVAL_P(op), NULL, 10); \
							Z_TYPE(holder) = IS_LONG; \
							break; \
					} \
					(op) = &(holder);										\
					break;													\
				}															\
			case IS_BOOL:													\
			case IS_RESOURCE:												\
				Z_LVAL(holder) = Z_LVAL_P(op);								\
				Z_TYPE(holder) = IS_LONG;									\
				(op) = &(holder);											\
				break;														\
			case IS_NULL:													\
				Z_LVAL(holder) = 0;											\
				Z_TYPE(holder) = IS_LONG;									\
				(op) = &(holder);											\
				break;														\
			case IS_OBJECT:													\
				(holder) = (*(op));											\
				zval_copy_ctor(&(holder));									\
				convert_to_long_base(&(holder), 10);						\
				if (Z_TYPE(holder) == IS_LONG) {								\
					(op) = &(holder);										\
				}															\
				break;														\
		}																	\
	}

/* }}} */

/* {{{ DVAL_TO_LVAL */
#define MAX_UNSIGNED_INT ((double) LONG_MAX * 2) + 1
#ifdef _WIN64
# define DVAL_TO_LVAL(d, l) \
	if ((d) > LONG_MAX) { \
		(l) = (long)(unsigned long)(__int64) (d); \
	} else { \
		(l) = (long) (d); \
	}
#elif !defined(_WIN64) && __WORDSIZE == 64
# define DVAL_TO_LVAL(d, l) \
	if ((d) >= LONG_MAX) { \
		(l) = LONG_MAX; \
	} else if ((d) <=  LONG_MIN) { \
		(l) = LONG_MIN; \
	} else {\
		(l) = (long) (d); \
	}
#else
# define DVAL_TO_LVAL(d, l) \
	if ((d) > LONG_MAX) { \
		if ((d) > MAX_UNSIGNED_INT) { \
			(l) = LONG_MAX; \
		} else { \
			(l) = (unsigned long) (d); \
		} \
	} else { \
		if((d) < LONG_MIN) { \
			(l) = LONG_MIN; \
		} else { \
			(l) = (long) (d); \
		} \
	}
#endif
/* }}} */

/* {{{ zendi_convert_to_long */
#define zendi_convert_to_long(op, holder, result)					\
	if (op == result) {												\
		convert_to_long(op);										\
	} else if (Z_TYPE_P(op) != IS_LONG) {							\
		switch (Z_TYPE_P(op)) {										\
			case IS_NULL:											\
				Z_LVAL(holder) = 0;									\
				break;												\
			case IS_DOUBLE:											\
				DVAL_TO_LVAL(Z_DVAL_P(op), Z_LVAL(holder));			\
				break;												\
			case IS_STRING:											\
				Z_LVAL(holder) = strtol(Z_STRVAL_P(op), NULL, 10);	\
				break;												\
			case IS_UNICODE:                                        \
				Z_LVAL(holder) = zend_u_strtol(Z_USTRVAL_P(op), NULL, 10);	\
				break;                                              \
			case IS_ARRAY:											\
				Z_LVAL(holder) = (zend_hash_num_elements(Z_ARRVAL_P(op))?1:0);	\
				break;												\
			case IS_OBJECT:											\
				(holder) = (*(op));									\
				zval_copy_ctor(&(holder));							\
				convert_to_long_base(&(holder), 10);				\
				break;												\
			case IS_BOOL:											\
			case IS_RESOURCE:										\
				Z_LVAL(holder) = Z_LVAL_P(op);						\
				break;												\
			default:												\
				zend_error(E_WARNING, "Cannot convert to ordinal value");	\
				Z_LVAL(holder) = 0;									\
				break;												\
		}															\
		Z_TYPE(holder) = IS_LONG;									\
		(op) = &(holder);											\
	}

/* }}} */

/* {{{ zendi_convert_to_boolean */
#define zendi_convert_to_boolean(op, holder, result)				\
	if (op==result) {												\
		convert_to_boolean(op);										\
	} else if (Z_TYPE_P(op) != IS_BOOL) {							\
		switch (Z_TYPE_P(op)) {										\
			case IS_NULL:											\
				Z_LVAL(holder) = 0;									\
				break;												\
			case IS_RESOURCE:										\
			case IS_LONG:											\
				Z_LVAL(holder) = (Z_LVAL_P(op) ? 1 : 0);			\
				break;												\
			case IS_DOUBLE:											\
				Z_LVAL(holder) = (Z_DVAL_P(op) ? 1 : 0);			\
				break;												\
			case IS_STRING:											\
				if (Z_STRLEN_P(op) == 0								\
					|| (Z_STRLEN_P(op)==1 && Z_STRVAL_P(op)[0]=='0')) {	\
					Z_LVAL(holder) = 0;								\
				} else {											\
					Z_LVAL(holder) = 1;								\
				}													\
				break;												\
			case IS_UNICODE:                                        \
				if (Z_USTRLEN_P(op) == 0	 						\
					|| (Z_USTRLEN_P(op)==1 &&	 					\
						(Z_USTRVAL_P(op)[0]=='0'))) {				\
					Z_LVAL(holder) = 0;		                        \
				} else {                                            \
					Z_LVAL(holder) = 1;		                        \
				}                                                   \
				break;                                              \
			case IS_ARRAY:											\
				Z_LVAL(holder) = (zend_hash_num_elements(Z_ARRVAL_P(op))?1:0);	\
				break;												\
			case IS_OBJECT:											\
				(holder) = (*(op));									\
				zval_copy_ctor(&(holder));							\
				convert_to_boolean(&(holder));						\
				break;												\
			default:												\
				Z_LVAL(holder) = 0;									\
				break;												\
		}															\
		Z_TYPE(holder) = IS_BOOL;									\
		(op) = &(holder);											\
	}

/* }}} */

/* {{{ convert_object_to_type */
#define convert_object_to_type(op, ctype, conv_func)										\
	if (Z_OBJ_HT_P(op)->cast_object) {														\
		zval dst;																			\
		if (Z_OBJ_HT_P(op)->cast_object(op, &dst, ctype, NULL TSRMLS_CC) == FAILURE) {		\
			zend_error(E_RECOVERABLE_ERROR, 												\
			"Object of class %v could not be converted to %s", Z_OBJCE_P(op)->name,			\
			zend_get_type_by_const(ctype));													\
		} else {																			\
			zval_dtor(op);																	\
			Z_TYPE_P(op) = ctype;															\
			op->value = dst.value;															\
		}																					\
	} else {																				\
		if(Z_OBJ_HT_P(op)->get) {															\
			zval *newop = Z_OBJ_HT_P(op)->get(op TSRMLS_CC);								\
			if(Z_TYPE_P(newop) != IS_OBJECT) {												\
				/* for safety - avoid loop */												\
				zval_dtor(op);																\
				*op = *newop;																\
				FREE_ZVAL(newop);															\
				conv_func(op);																\
			}																				\
		}																					\
	}

/* }}} */

ZEND_API int convert_to_long(zval *op) /* {{{ */
{
	return convert_to_long_base(op, 10);
}
/* }}} */

ZEND_API int convert_to_long_base(zval *op, int base) /* {{{ */
{
	long tmp;

	switch (Z_TYPE_P(op)) {
		case IS_NULL:
			Z_LVAL_P(op) = 0;
			break;
		case IS_RESOURCE: {
				TSRMLS_FETCH();

				zend_list_delete(Z_LVAL_P(op));
			}
			/* break missing intentionally */
		case IS_BOOL:
		case IS_LONG:
			break;
		case IS_DOUBLE:
			DVAL_TO_LVAL(Z_DVAL_P(op), Z_LVAL_P(op));
			break;
		case IS_STRING:
			{
				char *strval = Z_STRVAL_P(op);
				Z_LVAL_P(op) = strtol(strval, NULL, base);
				STR_FREE(strval);
			}
			break;
		case IS_UNICODE:
			{
				UChar *strval = Z_USTRVAL_P(op);
				Z_LVAL_P(op) = zend_u_strtol(strval, NULL, base);
				USTR_FREE(strval);
			}
			break;
		case IS_ARRAY:
			tmp = (zend_hash_num_elements(Z_ARRVAL_P(op))?1:0);
			zval_dtor(op);
			Z_LVAL_P(op) = tmp;
			break;
		case IS_OBJECT:
			{
				int retval = 1;
				TSRMLS_FETCH();

				convert_object_to_type(op, IS_LONG, convert_to_long);

				if (Z_TYPE_P(op) == IS_LONG) {
					return SUCCESS;
				}

				zend_error(E_NOTICE, "Object of class %v could not be converted to int", Z_OBJCE_P(op)->name);

				zval_dtor(op);
				ZVAL_LONG(op, retval);
				return FAILURE;
			}
		default:
			zend_error(E_WARNING, "Cannot convert to ordinal value");
			zval_dtor(op);
			Z_LVAL_P(op) = 0;
			return FAILURE;
	}

	Z_TYPE_P(op) = IS_LONG;
	return SUCCESS;
}
/* }}} */

ZEND_API int convert_to_double(zval *op) /* {{{ */
{
	double tmp;

	switch (Z_TYPE_P(op)) {
		case IS_NULL:
			Z_DVAL_P(op) = 0.0;
			break;
		case IS_RESOURCE: {
				TSRMLS_FETCH();

				zend_list_delete(Z_LVAL_P(op));
			}
			/* break missing intentionally */
		case IS_BOOL:
		case IS_LONG:
			Z_DVAL_P(op) = (double) Z_LVAL_P(op);
			break;
		case IS_DOUBLE:
			break;
		case IS_STRING:
			{
				char *strval = Z_STRVAL_P(op);

				Z_DVAL_P(op) = zend_strtod(strval, NULL);
				STR_FREE(strval);
			}
			break;
		case IS_UNICODE:
			{
				UChar *strval = Z_USTRVAL_P(op);

				Z_DVAL_P(op) = zend_u_strtod(strval, NULL);
				USTR_FREE(strval);
			}
			break;
		case IS_ARRAY:
			tmp = (zend_hash_num_elements(Z_ARRVAL_P(op))?1:0);
			zval_dtor(op);
			Z_DVAL_P(op) = tmp;
			break;
		case IS_OBJECT:
			{
				double retval = 1.0;
				TSRMLS_FETCH();

				convert_object_to_type(op, IS_DOUBLE, convert_to_double);

				if (Z_TYPE_P(op) == IS_DOUBLE) {
					return SUCCESS;
				}

				zend_error(E_NOTICE, "Object of class %v could not be converted to double", Z_OBJCE_P(op)->name);

				zval_dtor(op);
				ZVAL_DOUBLE(op, retval);
				return FAILURE;
			}
		default:
			zend_error(E_WARNING, "Cannot convert to real value (type=%d)", Z_TYPE_P(op));
			zval_dtor(op);
			Z_DVAL_P(op) = 0;
			return FAILURE;
	}

	Z_TYPE_P(op) = IS_DOUBLE;
	return SUCCESS;
}
/* }}} */

ZEND_API int convert_to_null(zval *op) /* {{{ */
{
	if (Z_TYPE_P(op) == IS_OBJECT) {
		if (Z_OBJ_HT_P(op)->cast_object) {
			zval *org;
			TSRMLS_FETCH();

			ALLOC_ZVAL(org);
			*org = *op;
			if (Z_OBJ_HT_P(op)->cast_object(org, op, IS_NULL, NULL TSRMLS_CC) == SUCCESS) {
				zval_dtor(org);
				return SUCCESS;
			}
			*op = *org;
			FREE_ZVAL(org);
		}
	}

	zval_dtor(op);
	Z_TYPE_P(op) = IS_NULL;
	return SUCCESS;
}
/* }}} */

ZEND_API int convert_to_boolean(zval *op) /* {{{ */
{
	int tmp;

	switch (Z_TYPE_P(op)) {
		case IS_BOOL:
			break;
		case IS_NULL:
			Z_LVAL_P(op) = 0;
			break;
		case IS_RESOURCE: {
				TSRMLS_FETCH();

				zend_list_delete(Z_LVAL_P(op));
			}
			/* break missing intentionally */
		case IS_LONG:
			Z_LVAL_P(op) = (Z_LVAL_P(op) ? 1 : 0);
			break;
		case IS_DOUBLE:
			Z_LVAL_P(op) = (Z_DVAL_P(op) ? 1 : 0);
			break;
		case IS_STRING:
			{
				char *strval = Z_STRVAL_P(op);

				if (Z_STRLEN_P(op) == 0
					|| (Z_STRLEN_P(op)==1 && Z_STRVAL_P(op)[0]=='0')) {
					Z_LVAL_P(op) = 0;
				} else {
					Z_LVAL_P(op) = 1;
				}
				STR_FREE(strval);
			}
			break;
		case IS_UNICODE:
			{
				UChar *strval = Z_USTRVAL_P(op);

				if (Z_USTRLEN_P(op) == 0
					|| (Z_USTRLEN_P(op)==1 && Z_USTRVAL_P(op)[0]=='0')) {
					Z_LVAL_P(op) = 0;
				} else {
					Z_LVAL_P(op) = 1;
				}
				USTR_FREE(strval);
			}
			break;
		case IS_ARRAY:
			tmp = (zend_hash_num_elements(Z_ARRVAL_P(op))?1:0);
			zval_dtor(op);
			Z_LVAL_P(op) = tmp;
			break;
		case IS_OBJECT:
			{
				zend_bool retval = 1;
				TSRMLS_FETCH();

				convert_object_to_type(op, IS_BOOL, convert_to_boolean);

				if (Z_TYPE_P(op) == IS_BOOL) {
					return SUCCESS;
				}

				zval_dtor(op);
				ZVAL_BOOL(op, retval);
				return FAILURE;
			}
		default:
			zval_dtor(op);
			Z_LVAL_P(op) = 0;
			break;
	}

	Z_TYPE_P(op) = IS_BOOL;
	return SUCCESS;
}
/* }}} */

#define NUM_BUF_SIZE 512

/* rewrite of ap_php_conv_10 for UChar support */
static UChar* zend_u_format_long(long lnum, UChar *result_end, int *result_len) /* {{{ */
{
	UChar *p = result_end;
	unsigned long magnitude;

	*--p = 0;

	if (lnum < 0) {
		long t = lnum + 1;
		magnitude = ((unsigned long) - t) + 1;
	} else {
		magnitude = (unsigned long) lnum;
	}

	do {
		unsigned long new_magnitude = magnitude / 10;

		*--p = (UChar)(magnitude - new_magnitude * 10 + 0x30 /*'0'*/);
		magnitude = new_magnitude;
	}
	while (magnitude);

	if (lnum < 0) {
		*--p = (UChar) 0x2d /*'-'*/;
	}

	*result_len = result_end - p - 1;
	return p;
}
/* }}} */

#define NDIG 320

/* rewrite of ap_php_cvt for UChar support */
static UChar* zend_u_format_double(double arg, int ndigits, int *decpt, int *sign, int eflag, UChar *buf) /* {{{ */
{
	register int r2;
	int mvl;
	double fi, fj;
	register UChar *p, *p1;

	if (ndigits >= NDIG - 1)
		ndigits = NDIG - 2;
	r2 = 0;
	*sign = 0;
	p = &buf[0];
	if (arg < 0) {
		*sign = 1;
		arg = -arg;
	}
	arg = modf(arg, &fi);
	p1 = &buf[NDIG];
	/*
	 * Do integer part
	 */
	if (fi != 0) {
		while (fi != 0) {
			fj = modf(fi / 10, &fi);
			if (p1 <= &buf[0]) {
				mvl = NDIG - ndigits;
				if (ndigits > 0) {
					memmove(&buf[mvl], &buf[0], (NDIG-mvl-1) * sizeof(UChar));
				}
				p1 += mvl;
			}
			*--p1 = (UChar) ((fj + .03) * 10) + 0x30 /*'0'*/;
			r2++;
		}
		while (p1 < &buf[NDIG]) {
			*p++ = *p1++;
		}
	} else if (arg > 0) {
		while ((fj = arg * 10) < 1) {
			if (!eflag && (r2 * -1) < ndigits) {
				break;
			}
			arg = fj;
			r2--;
		}
	}
	p1 = &buf[ndigits];
	if (eflag == 0)
		p1 += r2;
	*decpt = r2;
	if (p1 < &buf[0]) {
		buf[0] = 0;
		return (buf);
	}
	if (p <= p1 && p < &buf[NDIG]) {
		arg = modf(arg * 10, &fj);
		if ((int)fj==10) {
			*p++ = (UChar) 0x31 /*'1'*/;
			fj = 0;
			*decpt = ++r2;
		}
		while (p <= p1 && p < &buf[NDIG]) {
			*p++ = (UChar) fj + 0x30 /*'0'*/;
			arg = modf(arg * 10, &fj);
		}
	}
	if (p1 >= &buf[NDIG]) {
		buf[NDIG - 1] = 0;
		return (buf);
	}
	p = p1;
	*p1 += 5;
	while (*p1 > (UChar) 0x39 /*'9'*/) {
		*p1 = (UChar) 0x30 /*'0'*/;
		if (p1 > buf)
			++ * --p1;
		else {
			*p1 = (UChar) 0x31 /*'1'*/;
			(*decpt)++;
			if (eflag == 0) {
				if (p > buf)
					*p = (UChar) 0x30 /*'0'*/;
				p++;
			}
		}
	}
	*p = 0;
	return (buf);
}
/* }}} */

/* rewrite of ap_php_gcvt for UChar support */
static UChar* zend_u_format_gdouble(double dnum, int ndigit, UChar *result) /* {{{ */
{
	int sign, decpt;
	register UChar *p1, *p2;
	register int i;
	UChar buf1[NDIG];
	static zend_bool did_string_init = FALSE;
	U_STRING_DECL(u_nan, "NAN", 3);
	U_STRING_DECL(u_inf, "INF", 3);
	U_STRING_DECL(u_ninf, "-INF", 4);

	if (!did_string_init) {
		U_STRING_INIT(u_nan, "NAN", 3);
		U_STRING_INIT(u_inf, "INF", 3);
		U_STRING_INIT(u_ninf, "-INF", 4);
		did_string_init = TRUE;
	}

	/* check for out-of-bounds numbers */
	if (zend_isnan(dnum)) {
		u_memcpy(result, u_nan, 3);
		result[3] = 0;
		return result;
	} else if (zend_isinf(dnum)) {
		if (dnum > 0) {
			u_memcpy(result, u_inf, 3);
			result[3] = 0;
		} else {
			u_memcpy(result, u_ninf, 3);
			result[4] = 0;
		}
		return result;
	}

	if (ndigit >= NDIG - 1) {
		ndigit = NDIG - 2;
	}

	p1 = zend_u_format_double(dnum, ndigit, &decpt, &sign, 1, buf1);
	p2 = result;

	if (sign) {
		*p2++ = (UChar) 0x2d /*'-'*/;
	}
	/* if decimal point position is less than precision, cut zeros only in fractional part */
	if (decpt <= ndigit) {
		i = ndigit - 1;
		while (i > 0 && i >= decpt && p1[i] == (UChar) 0x30  /*'0'*/) {
			ndigit--;
			i--;
		}
	} else {
	/* otherwise cut all trailing zeros */
		for (i = ndigit - 1; i > 0 && p1[i] == (UChar) 0x30  /*'0'*/; i--) {
			ndigit--;
		}
	}

	if ((decpt >= 0 && decpt - ndigit > 4)
		|| (decpt < 0 && decpt < -3)) {		/* use E-style */
		decpt--;
		*p2++ = *p1++;
		*p2++ = (UChar) 0x2e /*'.'*/;
		for (i = 1; i < ndigit; i++)
			*p2++ = *p1++;
		if (*(p2 - 1) == (UChar) 0x2e /*'.'*/) {
			*p2++ = (UChar) 0x30 /*'0'*/;
		}
		*p2++ = (UChar) 0x45 /*'E'*/;
		if (decpt < 0) {
			decpt = -decpt;
			*p2++ = (UChar) 0x2d /*'-'*/;
		} else
			*p2++ = (UChar) 0x2b /*'+'*/;
		if (decpt / 100 > 0)
			*p2++ = (UChar) (decpt / 100 + 0x30 /*'0'*/);
		if (decpt / 10 > 0)
			*p2++ = (UChar) ((decpt % 100) / 10 + 0x30 /*'0'*/);
		*p2++ = (UChar) (decpt % 10 + 0x30 /*'0'*/);
	} else {
		if (decpt <= 0) {
			if (*p1 != (UChar) 0x30 /*'0'*/) {
				*p2++ = (UChar) 0x30 /*'0'*/;
				*p2++ = (UChar) 0x2e /*'.'*/;
			}
			while (decpt < 0) {
				decpt++;
				*p2++ = (UChar) 0x30 /*'0'*/;
			}
		}
		for (i = 1; i <= ndigit; i++) {
			*p2++ = *p1++;
			if (i == decpt)
				*p2++ = (UChar) 0x2e /*'.'*/;
		}
		if (ndigit < decpt) {
			while (ndigit++ < decpt)
				*p2++ = (UChar) 0x30 /*'0'*/;
			*p2++ = (UChar) 0x2e /*'.'*/;
		}
	}
	if (p2[-1] == (UChar) 0x2e /*'.'*/)
		p2--;
	*p2 = 0;
	return (result);
}
/* }}} */

ZEND_API int _convert_to_unicode(zval *op TSRMLS_DC ZEND_FILE_LINE_DC) /* {{{ */
{
    return _convert_to_unicode_with_converter(op, ZEND_U_CONVERTER(UG(runtime_encoding_conv)) TSRMLS_CC ZEND_FILE_LINE_CC);
}
/* }}} */

ZEND_API int _convert_to_unicode_with_converter(zval *op, UConverter *conv TSRMLS_DC ZEND_FILE_LINE_DC) /* {{{ */
{
	switch (Z_TYPE_P(op)) {
		case IS_NULL:
			Z_USTRVAL_P(op) = USTR_MAKE_REL("");
			Z_USTRLEN_P(op) = 0;
			break;
		case IS_UNICODE:
			break;
		case IS_STRING:
			return zval_string_to_unicode_ex(op, conv TSRMLS_CC);
		case IS_BOOL:
			if (Z_LVAL_P(op)) {
				Z_USTRVAL_P(op) = USTR_MAKE_REL("1");
				Z_USTRLEN_P(op) = 1;
			} else {
				Z_USTRVAL_P(op) = USTR_MAKE_REL("");
				Z_USTRLEN_P(op) = 0;
			}
			break;
		case IS_RESOURCE: {
			UChar num_buf[NUM_BUF_SIZE], *result;
			int result_len, rstr_len;
			long rval = Z_LVAL_P(op);

			zend_list_delete(Z_LVAL_P(op));
			result = zend_u_format_long(rval, &num_buf[NUM_BUF_SIZE], &result_len);

			rstr_len = sizeof("Resource id #")-1;
			Z_USTRLEN_P(op) = rstr_len + result_len;
			Z_USTRVAL_P(op) = eumalloc_rel(Z_USTRLEN_P(op) + 1);
			u_charsToUChars("Resource id #", Z_USTRVAL_P(op), rstr_len);
			/* result_len+1 takes care of terminating NULL */
			u_memcpy(Z_USTRVAL_P(op) + rstr_len, result, result_len+1);
			break;
		}
		case IS_LONG: {
			UChar num_buf[NUM_BUF_SIZE], *result;
			int result_len;
			long lval = Z_LVAL_P(op);

			result = zend_u_format_long(lval, &num_buf[NUM_BUF_SIZE], &result_len);

			Z_USTRVAL_P(op) = eustrndup(result, result_len);
			Z_USTRLEN_P(op) = result_len;
			break;
	    }
		case IS_DOUBLE: {
			UChar num_buf[NUM_BUF_SIZE], *result;
			double dval = Z_DVAL_P(op);

			result = zend_u_format_gdouble(dval, (int) EG(precision), &num_buf[1]);
			if (*result == (UChar) 0x2b /*'+'*/) {
				result++;
			}
			Z_USTRLEN_P(op) = u_strlen(result);
			Z_USTRVAL_P(op) = eustrndup(result, Z_USTRLEN_P(op));
			break;
		}
		case IS_ARRAY:
			zend_error(E_NOTICE, "Array to string conversion");
			zval_dtor(op);
			Z_USTRVAL_P(op) = USTR_MAKE_REL("Array");
			Z_USTRLEN_P(op) = sizeof("Array")-1;
			Z_TYPE_P(op) = IS_UNICODE;
			return FAILURE;
		case IS_OBJECT: {
			int retval = FAILURE;

			if (Z_OBJ_HT_P(op)->cast_object) {
				zval dst;
				if (Z_OBJ_HT_P(op)->cast_object(op, &dst, IS_UNICODE, conv TSRMLS_CC) == FAILURE) {
					zend_error(E_RECOVERABLE_ERROR,
							   "Object of class %v could not be converted to %s", Z_OBJCE_P(op)->name,
							   zend_get_type_by_const(IS_UNICODE));
				} else {
					zval_dtor(op);
					Z_TYPE_P(op) = IS_UNICODE;
					op->value = dst.value;
					retval = SUCCESS;
				}
			} else {
				if(Z_OBJ_HT_P(op)->get) {
					zval *newop = Z_OBJ_HT_P(op)->get(op TSRMLS_CC);
					if(Z_TYPE_P(newop) != IS_OBJECT) {
						/* for safety - avoid loop */
						zval_dtor(op);
						*op = *newop;
						FREE_ZVAL(newop);
						retval = convert_to_string_with_converter(op, conv);
					}
				}
			}

			if (retval == SUCCESS && Z_TYPE_P(op) == IS_UNICODE) {
				return SUCCESS;
			}

			zend_error(E_NOTICE, "Object of class %v to string conversion", Z_OBJCE_P(op)->name);
			zval_dtor(op);
			Z_USTRVAL_P(op) = USTR_MAKE_REL("Object");
			Z_USTRLEN_P(op) = sizeof("Object")-1;
			Z_TYPE_P(op) = IS_UNICODE;
			return FAILURE;
		}
		default:
			zval_dtor(op);
			ZVAL_BOOL(op, 0);
			return FAILURE;
	}

	Z_TYPE_P(op) = IS_UNICODE;
	return SUCCESS;
}
/* }}} */

ZEND_API int _convert_to_string(zval *op ZEND_FILE_LINE_DC) /* {{{ */
{
    TSRMLS_FETCH();
    return _convert_to_string_with_converter(op, ZEND_U_CONVERTER(UG(runtime_encoding_conv)) TSRMLS_CC ZEND_FILE_LINE_CC);
}
/* }}} */

ZEND_API int _convert_to_string_with_converter(zval *op, UConverter *conv TSRMLS_DC ZEND_FILE_LINE_DC) /* {{{ */
{
	long lval;
	double dval;

	switch (Z_TYPE_P(op)) {
		case IS_NULL:
			Z_STRVAL_P(op) = STR_EMPTY_ALLOC();
			Z_STRLEN_P(op) = 0;
			break;
		case IS_STRING:
			break;
		case IS_UNICODE:
			return zval_unicode_to_string_ex(op, conv TSRMLS_CC);
		case IS_BOOL:
			if (Z_LVAL_P(op)) {
				Z_STRVAL_P(op) = estrndup_rel("1", 1);
				Z_STRLEN_P(op) = 1;
			} else {
				Z_STRVAL_P(op) = STR_EMPTY_ALLOC();
				Z_STRLEN_P(op) = 0;
			}
			break;
		case IS_RESOURCE: {
			long tmp = Z_LVAL_P(op);
			TSRMLS_FETCH();

			zend_list_delete(Z_LVAL_P(op));
			Z_STRLEN_P(op) = zend_spprintf(&Z_STRVAL_P(op), 0, "Resource id #%ld", tmp);
			break;
		}
		case IS_LONG:
			lval = Z_LVAL_P(op);

			Z_STRLEN_P(op) = zend_spprintf(&Z_STRVAL_P(op), 0, "%ld", lval);
			break;
		case IS_DOUBLE: {
			TSRMLS_FETCH();
			dval = Z_DVAL_P(op);
			Z_STRLEN_P(op) = zend_spprintf(&Z_STRVAL_P(op), 0, "%.*G", (int) EG(precision), dval);
			/* %G already handles removing trailing zeros from the fractional part, yay */
			break;
		}
		case IS_ARRAY:
			zend_error(E_NOTICE, "Array to string conversion");
			zval_dtor(op);
			Z_STRVAL_P(op) = estrndup_rel("Array", sizeof("Array")-1);
			Z_STRLEN_P(op) = sizeof("Array")-1;
			Z_TYPE_P(op) = IS_STRING;
			return FAILURE;
		case IS_OBJECT: {
			int retval = FAILURE;
			TSRMLS_FETCH();

			if (Z_OBJ_HT_P(op)->cast_object) {
				zval dst;
				if (Z_OBJ_HT_P(op)->cast_object(op, &dst, IS_STRING, conv TSRMLS_CC) == FAILURE) {
					zend_error(E_RECOVERABLE_ERROR,
							   "Object of class %v could not be converted to %s", Z_OBJCE_P(op)->name,
							   zend_get_type_by_const(IS_STRING));
				} else {
					zval_dtor(op);
					Z_TYPE_P(op) = IS_STRING;
					op->value = dst.value;
					retval = SUCCESS;
				}
			} else {
				if(Z_OBJ_HT_P(op)->get) {
					zval *newop = Z_OBJ_HT_P(op)->get(op TSRMLS_CC);
					if(Z_TYPE_P(newop) != IS_OBJECT) {
						/* for safety - avoid loop */
						zval_dtor(op);
						*op = *newop;
						FREE_ZVAL(newop);
						retval = convert_to_string_with_converter(op, conv);
					}
				}
			}

			if (retval == SUCCESS && Z_TYPE_P(op) == IS_STRING) {
				return SUCCESS;
			}

			zend_error(E_NOTICE, "Object of class %v to string conversion", Z_OBJCE_P(op)->name);
			zval_dtor(op);
			Z_STRVAL_P(op) = estrndup_rel("Object", sizeof("Object")-1);
			Z_STRLEN_P(op) = sizeof("Object")-1;
			Z_TYPE_P(op) = IS_STRING;
			return FAILURE;
		}
		default:
			zval_dtor(op);
			ZVAL_BOOL(op, 0);
			return FAILURE;
	}

	Z_TYPE_P(op) = IS_STRING;
	return SUCCESS;
}
/* }}} */

static int convert_scalar_to_array(zval *op, int type TSRMLS_DC) /* {{{ */
{
	zval *entry;

	ALLOC_ZVAL(entry);
	*entry = *op;
	INIT_PZVAL(entry);

	switch (type) {
		case IS_ARRAY:
			ALLOC_HASHTABLE(Z_ARRVAL_P(op));
			zend_u_hash_init(Z_ARRVAL_P(op), 0, NULL, ZVAL_PTR_DTOR, 0, 0);
			zend_hash_index_update(Z_ARRVAL_P(op), 0, (void *) &entry, sizeof(zval *), NULL);
			Z_TYPE_P(op) = IS_ARRAY;
			break;
		case IS_OBJECT:
			{
				/* OBJECTS_OPTIMIZE */
				TSRMLS_FETCH();

				object_init(op);
				zend_hash_update(Z_OBJPROP_P(op), "scalar", sizeof("scalar"), (void *) &entry, sizeof(zval *), NULL);
			}
			break;
	}

	return SUCCESS;
}
/* }}} */

ZEND_API int convert_to_array(zval *op) /* {{{ */
{
	TSRMLS_FETCH();

	switch (Z_TYPE_P(op)) {
		case IS_ARRAY:
			break;
/* OBJECTS_OPTIMIZE */
		case IS_OBJECT:
			{
				zval *tmp;
				HashTable *ht;
				int retval = FAILURE;

				ALLOC_HASHTABLE(ht);
				zend_u_hash_init(ht, 0, NULL, ZVAL_PTR_DTOR, 0, 0);
				if (Z_OBJ_HT_P(op)->get_properties) {
					HashTable *obj_ht = Z_OBJ_HT_P(op)->get_properties(op TSRMLS_CC);
					if (obj_ht) {
						zend_hash_copy(ht, obj_ht, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
						retval = SUCCESS;
					}
				} else {
					convert_object_to_type(op, IS_ARRAY, convert_to_array);

					if (Z_TYPE_P(op) == IS_ARRAY) {
						zend_hash_destroy(ht);
						FREE_HASHTABLE(ht);
						return SUCCESS;
					}
				}
				zval_dtor(op);
				Z_TYPE_P(op) = IS_ARRAY;
				Z_ARRVAL_P(op) = ht;
				return retval;
			}
			break;
		case IS_NULL:
			ALLOC_HASHTABLE(Z_ARRVAL_P(op));
			zend_u_hash_init(Z_ARRVAL_P(op), 0, NULL, ZVAL_PTR_DTOR, 0, 0);
			Z_TYPE_P(op) = IS_ARRAY;
			break;
		default:
			return convert_scalar_to_array(op, IS_ARRAY TSRMLS_CC);
	}

	return SUCCESS;
}
/* }}} */

ZEND_API int convert_to_object(zval *op) /* {{{ */
{
	TSRMLS_FETCH();
	switch (Z_TYPE_P(op)) {
		case IS_ARRAY:
			{
				object_and_properties_init(op, zend_standard_class_def, Z_ARRVAL_P(op));
				break;
			}
		case IS_OBJECT:
			break;
		case IS_NULL:
			{
				/* OBJECTS_OPTIMIZE */
				TSRMLS_FETCH();

				object_init(op);
				break;
			}
		default:
			return convert_scalar_to_array(op, IS_OBJECT TSRMLS_CC);
	}

	return SUCCESS;
}
/* }}} */

ZEND_API void multi_convert_to_long_ex(int argc, ...) /* {{{ */
{
	zval **arg;
	va_list ap;

	va_start(ap, argc);

	while (argc--) {
		arg = va_arg(ap, zval **);
		convert_to_long_ex(arg);
	}

	va_end(ap);
}
/* }}} */

ZEND_API void multi_convert_to_double_ex(int argc, ...) /* {{{ */
{
	zval **arg;
	va_list ap;

	va_start(ap, argc);

	while (argc--) {
		arg = va_arg(ap, zval **);
		convert_to_double_ex(arg);
	}

	va_end(ap);
}
/* }}} */

ZEND_API void multi_convert_to_string_ex(int argc, ...) /* {{{ */
{
	zval **arg;
	va_list ap;

	va_start(ap, argc);

	while (argc--) {
		arg = va_arg(ap, zval **);
		convert_to_string_ex(arg);
	}

	va_end(ap);
}
/* }}} */

ZEND_API int add_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	ZEND_FLOAT_DECLARE
	zval op1_copy, op2_copy;
	int converted = 0;

	while (1) {
		switch (TYPE_PAIR(Z_TYPE_P(op1), Z_TYPE_P(op2))) {
			case TYPE_PAIR(IS_LONG, IS_LONG): {
				long lval = Z_LVAL_P(op1) + Z_LVAL_P(op2);

				/* check for overflow by comparing sign bits */
				if ((Z_LVAL_P(op1) & LONG_SIGN_MASK) == (Z_LVAL_P(op2) & LONG_SIGN_MASK)
					&& (Z_LVAL_P(op1) & LONG_SIGN_MASK) != (lval & LONG_SIGN_MASK)) {

					ZEND_FLOAT_ENSURE();
					ZVAL_DOUBLE(result, (double) Z_LVAL_P(op1) + (double) Z_LVAL_P(op2));
					ZEND_FLOAT_RESTORE();
				} else {
					ZVAL_LONG(result, lval);
				}
				return SUCCESS;
			}

			case TYPE_PAIR(IS_LONG, IS_DOUBLE):
				ZEND_FLOAT_ENSURE();
				ZVAL_DOUBLE(result, ((double)Z_LVAL_P(op1)) + Z_DVAL_P(op2));
				ZEND_FLOAT_RESTORE();
				return SUCCESS;

			case TYPE_PAIR(IS_DOUBLE, IS_LONG):
				ZEND_FLOAT_ENSURE();
				ZVAL_DOUBLE(result, Z_DVAL_P(op1) + ((double)Z_LVAL_P(op2)));
				ZEND_FLOAT_RESTORE();
				return SUCCESS;

			case TYPE_PAIR(IS_DOUBLE, IS_DOUBLE):
				ZEND_FLOAT_ENSURE();
				ZVAL_DOUBLE(result, Z_DVAL_P(op1) + Z_DVAL_P(op2));
				ZEND_FLOAT_RESTORE();
				return SUCCESS;

			case TYPE_PAIR(IS_ARRAY, IS_ARRAY): {
				zval *tmp;

				if ((result == op1) && (result == op2)) {
					/* $a += $a */
					return SUCCESS;
				}
				if (result != op1) {
					*result = *op1;
					zval_copy_ctor(result);
				}
				zend_hash_merge(Z_ARRVAL_P(result), Z_ARRVAL_P(op2), (void (*)(void *pData)) zval_add_ref, (void *) &tmp, sizeof(zval *), 0);
				return SUCCESS;
			}

			default:
				if (!converted) {
					zendi_convert_scalar_to_number(op1, op1_copy, result);
					zendi_convert_scalar_to_number(op2, op2_copy, result);
					converted = 1;
				} else {
					zend_error(E_ERROR, "Unsupported operand types");
					return FAILURE; /* unknown datatype */
				}
 		}
 	}
}
/* }}} */

ZEND_API int sub_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	ZEND_FLOAT_DECLARE
	zval op1_copy, op2_copy;
	int converted = 0;

	while (1) {
		switch (TYPE_PAIR(Z_TYPE_P(op1), Z_TYPE_P(op2))) {
			case TYPE_PAIR(IS_LONG, IS_LONG): {
				long lval = Z_LVAL_P(op1) - Z_LVAL_P(op2);

				/* check for overflow by comparing sign bits */
				if ((Z_LVAL_P(op1) & LONG_SIGN_MASK) != (Z_LVAL_P(op2) & LONG_SIGN_MASK)
					&& (Z_LVAL_P(op1) & LONG_SIGN_MASK) != (lval & LONG_SIGN_MASK)) {

					ZEND_FLOAT_ENSURE();
					ZVAL_DOUBLE(result, (double) Z_LVAL_P(op1) - (double) Z_LVAL_P(op2));
					ZEND_FLOAT_RESTORE();
				} else {
					ZVAL_LONG(result, lval);
				}
				return SUCCESS;

			}
			case TYPE_PAIR(IS_LONG, IS_DOUBLE):
				ZEND_FLOAT_ENSURE();
				ZVAL_DOUBLE(result, ((double)Z_LVAL_P(op1)) - Z_DVAL_P(op2));
				ZEND_FLOAT_RESTORE();
				return SUCCESS;

			case TYPE_PAIR(IS_DOUBLE, IS_LONG):
				ZEND_FLOAT_ENSURE();
				ZVAL_DOUBLE(result, Z_DVAL_P(op1) - ((double)Z_LVAL_P(op2)));
				ZEND_FLOAT_RESTORE();
				return SUCCESS;

			case TYPE_PAIR(IS_DOUBLE, IS_DOUBLE):
				ZEND_FLOAT_ENSURE();
				ZVAL_DOUBLE(result, Z_DVAL_P(op1) - Z_DVAL_P(op2));
				ZEND_FLOAT_RESTORE();
				return SUCCESS;

			default:
				if (!converted) {
					zendi_convert_scalar_to_number(op1, op1_copy, result);
					zendi_convert_scalar_to_number(op2, op2_copy, result);
					converted = 1;
				} else {
					zend_error(E_ERROR, "Unsupported operand types");
					return FAILURE; /* unknown datatype */
				}
		}
	}
}
/* }}} */

ZEND_API int mul_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	ZEND_FLOAT_DECLARE
	zval op1_copy, op2_copy;
	int converted = 0;

	while (1) {
		switch (TYPE_PAIR(Z_TYPE_P(op1), Z_TYPE_P(op2))) {
			case TYPE_PAIR(IS_LONG, IS_LONG): {
				long overflow;

				ZEND_FLOAT_ENSURE();
				ZEND_SIGNED_MULTIPLY_LONG(Z_LVAL_P(op1),Z_LVAL_P(op2), Z_LVAL_P(result),Z_DVAL_P(result),overflow);
				ZEND_FLOAT_RESTORE();
				Z_TYPE_P(result) = overflow ? IS_DOUBLE : IS_LONG;
				return SUCCESS;

			}
			case TYPE_PAIR(IS_LONG, IS_DOUBLE):
				ZEND_FLOAT_ENSURE();
				ZVAL_DOUBLE(result, ((double)Z_LVAL_P(op1)) * Z_DVAL_P(op2));
				ZEND_FLOAT_RESTORE();
				return SUCCESS;

			case TYPE_PAIR(IS_DOUBLE, IS_LONG):
				ZEND_FLOAT_ENSURE();
				ZVAL_DOUBLE(result, Z_DVAL_P(op1) * ((double)Z_LVAL_P(op2)));
				ZEND_FLOAT_RESTORE();
				return SUCCESS;

			case TYPE_PAIR(IS_DOUBLE, IS_DOUBLE):
				ZEND_FLOAT_ENSURE();
				ZVAL_DOUBLE(result, Z_DVAL_P(op1) * Z_DVAL_P(op2));
				ZEND_FLOAT_RESTORE();
				return SUCCESS;

			default:
				if (!converted) {
					zendi_convert_scalar_to_number(op1, op1_copy, result);
					zendi_convert_scalar_to_number(op2, op2_copy, result);
					converted = 1;
				} else {
					zend_error(E_ERROR, "Unsupported operand types");
					return FAILURE; /* unknown datatype */
				}
		}
	}
}
/* }}} */

ZEND_API int div_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	ZEND_FLOAT_DECLARE
	zval op1_copy, op2_copy;
	int converted = 0;

	while (1) {
		switch (TYPE_PAIR(Z_TYPE_P(op1), Z_TYPE_P(op2))) {
			case TYPE_PAIR(IS_LONG, IS_LONG):
				if (Z_LVAL_P(op2) == 0) {
					zend_error(E_WARNING, "Division by zero");
					ZVAL_BOOL(result, 0);
					return FAILURE;			/* division by zero */
				} else if (Z_LVAL_P(op2) == -1 && Z_LVAL_P(op1) == LONG_MIN) {
					/* Prevent overflow error/crash */
					ZEND_FLOAT_ENSURE();
					ZVAL_DOUBLE(result, (double) LONG_MIN / -1);
					ZEND_FLOAT_RESTORE();
					return SUCCESS;
				}
				if (Z_LVAL_P(op1) % Z_LVAL_P(op2) == 0) { /* integer */
					ZVAL_LONG(result, Z_LVAL_P(op1) / Z_LVAL_P(op2));
				} else {
					ZEND_FLOAT_ENSURE();
					ZVAL_DOUBLE(result, ((double) Z_LVAL_P(op1)) / Z_LVAL_P(op2));
					ZEND_FLOAT_RESTORE();
				}
				return SUCCESS;

			case TYPE_PAIR(IS_DOUBLE, IS_LONG):
				if (Z_LVAL_P(op2) == 0) {
					zend_error(E_WARNING, "Division by zero");
					ZVAL_BOOL(result, 0);
					return FAILURE;			/* division by zero */
				}
				ZEND_FLOAT_ENSURE();
				ZVAL_DOUBLE(result, Z_DVAL_P(op1) / (double)Z_LVAL_P(op2));
				ZEND_FLOAT_RESTORE();
				return SUCCESS;

			case TYPE_PAIR(IS_LONG, IS_DOUBLE):
				if (Z_DVAL_P(op2) == 0) {
					zend_error(E_WARNING, "Division by zero");
					ZVAL_BOOL(result, 0);
					return FAILURE;			/* division by zero */
				}
				ZEND_FLOAT_ENSURE();
				ZVAL_DOUBLE(result, (double)Z_LVAL_P(op1) / Z_DVAL_P(op2));
				ZEND_FLOAT_RESTORE();
				return SUCCESS;

			case TYPE_PAIR(IS_DOUBLE, IS_DOUBLE):
				if (Z_DVAL_P(op2) == 0) {
					zend_error(E_WARNING, "Division by zero");
					ZVAL_BOOL(result, 0);
					return FAILURE;			/* division by zero */
				}
				ZEND_FLOAT_ENSURE();
				ZVAL_DOUBLE(result, Z_DVAL_P(op1) / Z_DVAL_P(op2));
				ZEND_FLOAT_RESTORE();
				return SUCCESS;

			default:
				if (!converted) {
					zendi_convert_scalar_to_number(op1, op1_copy, result);
					zendi_convert_scalar_to_number(op2, op2_copy, result);
					converted = 1;
				} else {
					zend_error(E_ERROR, "Unsupported operand types");
					return FAILURE; /* unknown datatype */
				}
		}
	}
}
/* }}} */

ZEND_API int mod_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	zval op1_copy, op2_copy;

	zendi_convert_to_long(op1, op1_copy, result);
	zendi_convert_to_long(op2, op2_copy, result);

	if (Z_LVAL_P(op2) == 0) {
		zend_error(E_WARNING, "Division by zero");
		ZVAL_BOOL(result, 0);
		return FAILURE;			/* modulus by zero */
	}

	if (abs(Z_LVAL_P(op2)) == 1) {
		ZVAL_LONG(result, 0);
		return SUCCESS;
	}

	Z_TYPE_P(result) = IS_LONG;
	Z_LVAL_P(result) = Z_LVAL_P(op1) % Z_LVAL_P(op2);
	return SUCCESS;
}
/* }}} */

ZEND_API int boolean_xor_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	zval op1_copy, op2_copy;

	Z_TYPE_P(result) = IS_BOOL;

	zendi_convert_to_boolean(op1, op1_copy, result);
	zendi_convert_to_boolean(op2, op2_copy, result);
	Z_LVAL_P(result) = Z_LVAL_P(op1) ^ Z_LVAL_P(op2);
	return SUCCESS;
}
/* }}} */

ZEND_API int boolean_not_function(zval *result, zval *op1 TSRMLS_DC) /* {{{ */
{
	zval op1_copy;

	zendi_convert_to_boolean(op1, op1_copy, result);

	Z_TYPE_P(result) = IS_BOOL;
	Z_LVAL_P(result) = !Z_LVAL_P(op1);
	return SUCCESS;
}
/* }}} */

ZEND_API int bitwise_not_function(zval *result, zval *op1 TSRMLS_DC) /* {{{ */
{
	zval op1_copy = *op1;

	op1 = &op1_copy;

	if (Z_TYPE_P(op1) == IS_LONG) {
		ZVAL_LONG(result, ~Z_LVAL_P(op1));
		return SUCCESS;
	} else if (Z_TYPE_P(op1) == IS_DOUBLE) {
		ZVAL_LONG(result, ~(long)Z_DVAL_P(op1));
		return SUCCESS;
	} else if (Z_TYPE_P(op1) == IS_STRING) {
		int i;

		Z_TYPE_P(result) = Z_TYPE_P(op1);
		Z_STRVAL_P(result) = estrndup(Z_STRVAL_P(op1), Z_STRLEN_P(op1));
		Z_STRLEN_P(result) = Z_STRLEN_P(op1);
		for (i = 0; i < Z_STRLEN_P(op1); i++) {
			Z_STRVAL_P(result)[i] = ~Z_STRVAL_P(op1)[i];
		}
		return SUCCESS;
	}
	zend_error(E_ERROR, "Unsupported operand types");
	return FAILURE;				/* unknown datatype */
}
/* }}} */

ZEND_API int bitwise_or_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	zval op1_copy, op2_copy;

	if (Z_TYPE_P(op1) == IS_STRING && Z_TYPE_P(op2) == IS_STRING) {
		zval *longer, *shorter;
		char *result_str;
		int i, result_len;

		if (Z_STRLEN_P(op1) >= Z_STRLEN_P(op2)) {
			longer = op1;
			shorter = op2;
		} else {
			longer = op2;
			shorter = op1;
		}

		Z_TYPE_P(result) = Z_TYPE_P(op1);
		result_len = Z_STRLEN_P(longer);
		result_str = estrndup(Z_STRVAL_P(longer), Z_STRLEN_P(longer));
		for (i = 0; i < Z_STRLEN_P(shorter); i++) {
			result_str[i] |= Z_STRVAL_P(shorter)[i];
		}
		if (result==op1) {
			STR_FREE(Z_STRVAL_P(result));
		}
		Z_STRVAL_P(result) = result_str;
		Z_STRLEN_P(result) = result_len;
		return SUCCESS;
	}
	if (Z_TYPE_P(op1) == IS_UNICODE && Z_TYPE_P(op2) == IS_UNICODE) {
		zend_error(E_ERROR, "Unsupported operand types");
		return FAILURE;
	}
	zendi_convert_to_long(op1, op1_copy, result);
	zendi_convert_to_long(op2, op2_copy, result);

	Z_TYPE_P(result) = IS_LONG;
	Z_LVAL_P(result) = Z_LVAL_P(op1) | Z_LVAL_P(op2);
	return SUCCESS;
}
/* }}} */

ZEND_API int bitwise_and_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	zval op1_copy, op2_copy;

	if (Z_TYPE_P(op1) == IS_STRING && Z_TYPE_P(op2) == IS_STRING) {
		zval *longer, *shorter;
		char *result_str;
		int i, result_len;

		if (Z_STRLEN_P(op1) >= Z_STRLEN_P(op2)) {
			longer = op1;
			shorter = op2;
		} else {
			longer = op2;
			shorter = op1;
		}

		Z_TYPE_P(result) = Z_TYPE_P(op1);
		result_len = Z_STRLEN_P(shorter);
		result_str = estrndup(Z_STRVAL_P(shorter), Z_STRLEN_P(shorter));
		for (i = 0; i < Z_STRLEN_P(shorter); i++) {
			result_str[i] &= Z_STRVAL_P(longer)[i];
		}
		if (result==op1) {
			STR_FREE(Z_STRVAL_P(result));
		}
		Z_STRVAL_P(result) = result_str;
		Z_STRLEN_P(result) = result_len;
		return SUCCESS;
	}

	if (Z_TYPE_P(op1) == IS_UNICODE && Z_TYPE_P(op2) == IS_UNICODE) {
		zend_error(E_ERROR, "Unsupported operand types");
		return FAILURE;
	}

	zendi_convert_to_long(op1, op1_copy, result);
	zendi_convert_to_long(op2, op2_copy, result);

	Z_TYPE_P(result) = IS_LONG;
	Z_LVAL_P(result) = Z_LVAL_P(op1) & Z_LVAL_P(op2);
	return SUCCESS;
}
/* }}} */

ZEND_API int bitwise_xor_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	zval op1_copy, op2_copy;

	if (Z_TYPE_P(op1) == IS_STRING && Z_TYPE_P(op2) == IS_STRING) {
		zval *longer, *shorter;
		char *result_str;
		int i, result_len;

		if (Z_STRLEN_P(op1) >= Z_STRLEN_P(op2)) {
			longer = op1;
			shorter = op2;
		} else {
			longer = op2;
			shorter = op1;
		}

		Z_TYPE_P(result) = Z_TYPE_P(op1);
		result_len = Z_STRLEN_P(shorter);
		result_str = estrndup(Z_STRVAL_P(shorter), Z_STRLEN_P(shorter));
		for (i = 0; i < Z_STRLEN_P(shorter); i++) {
			result_str[i] ^= Z_STRVAL_P(longer)[i];
		}
		if (result==op1) {
			STR_FREE(Z_STRVAL_P(result));
		}
		Z_STRVAL_P(result) = result_str;
		Z_STRLEN_P(result) = result_len;
		return SUCCESS;
	}

	if (Z_TYPE_P(op1) == IS_UNICODE && Z_TYPE_P(op2) == IS_UNICODE) {
		zend_error(E_ERROR, "Unsupported operand types");
		return FAILURE;
	}

	zendi_convert_to_long(op1, op1_copy, result);
	zendi_convert_to_long(op2, op2_copy, result);

	Z_TYPE_P(result) = IS_LONG;
	Z_LVAL_P(result) = Z_LVAL_P(op1) ^ Z_LVAL_P(op2);
	return SUCCESS;
}
/* }}} */

ZEND_API int shift_left_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	zval op1_copy, op2_copy;

	zendi_convert_to_long(op1, op1_copy, result);
	zendi_convert_to_long(op2, op2_copy, result);
	Z_LVAL_P(result) = Z_LVAL_P(op1) << Z_LVAL_P(op2);
	Z_TYPE_P(result) = IS_LONG;
	return SUCCESS;
}
/* }}} */

ZEND_API int shift_right_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	zval op1_copy, op2_copy;

	zendi_convert_to_long(op1, op1_copy, result);
	zendi_convert_to_long(op2, op2_copy, result);
	Z_LVAL_P(result) = Z_LVAL_P(op1) >> Z_LVAL_P(op2);
	Z_TYPE_P(result) = IS_LONG;
	return SUCCESS;
}
/* }}} */

/* must support result==op1 */
ZEND_API int add_char_to_string(zval *result, const zval *op1, const zval *op2) /* {{{ */
{
	if (Z_TYPE_P(op1) == IS_UNICODE) {
		UChar32 codepoint = (UChar32) Z_LVAL_P(op2);

		if (U_IS_BMP(codepoint)) {
			Z_USTRLEN_P(result) = Z_USTRLEN_P(op1) + 1;
			Z_USTRVAL_P(result) = eurealloc(Z_USTRVAL_P(op1), Z_USTRLEN_P(result)+1);
			Z_USTRVAL_P(result)[Z_USTRLEN_P(result) - 1] = (UChar) Z_LVAL_P(op2);
		} else {
			Z_USTRLEN_P(result) = Z_USTRLEN_P(op1) + 2;
			Z_USTRVAL_P(result) = eurealloc(Z_USTRVAL_P(op1), Z_USTRLEN_P(result)+1);
			Z_USTRVAL_P(result)[Z_USTRLEN_P(result) - 2] = (UChar) U16_LEAD(codepoint);
			Z_USTRVAL_P(result)[Z_USTRLEN_P(result) - 1] = (UChar) U16_TRAIL(codepoint);
		}
		Z_USTRVAL_P(result)[Z_USTRLEN_P(result)] = 0;
		Z_TYPE_P(result) = IS_UNICODE;
	} else {
		Z_STRLEN_P(result) = Z_STRLEN_P(op1) + 1;
		Z_STRVAL_P(result) = (char *) erealloc(Z_STRVAL_P(op1), Z_STRLEN_P(result)+1);
		Z_STRVAL_P(result)[Z_STRLEN_P(result) - 1] = (char) Z_LVAL_P(op2);
		Z_STRVAL_P(result)[Z_STRLEN_P(result)] = 0;
		Z_TYPE_P(result) = Z_TYPE_P(op1);
	}
	return SUCCESS;
}
/* }}} */

/* must support result==op1 */
ZEND_API int add_string_to_string(zval *result, const zval *op1, const zval *op2) /* {{{ */
{
	assert(Z_TYPE_P(op1) == Z_TYPE_P(op2));

	if (Z_TYPE_P(op1) == IS_UNICODE) {
		int length = Z_USTRLEN_P(op1) + Z_USTRLEN_P(op2);

		Z_USTRVAL_P(result) = eurealloc(Z_USTRVAL_P(op1), length+1);
		u_memcpy(Z_USTRVAL_P(result)+Z_USTRLEN_P(op1), Z_USTRVAL_P(op2), Z_USTRLEN_P(op2));
		Z_USTRVAL_P(result)[length] = 0;
		Z_USTRLEN_P(result) = length;
		Z_TYPE_P(result) = IS_UNICODE;
	} else {
		int length = Z_STRLEN_P(op1) + Z_STRLEN_P(op2);

		Z_STRVAL_P(result) = (char *) erealloc(Z_STRVAL_P(op1), length+1);
		memcpy(Z_STRVAL_P(result)+Z_STRLEN_P(op1), Z_STRVAL_P(op2), Z_STRLEN_P(op2));
		Z_STRVAL_P(result)[length] = 0;
		Z_STRLEN_P(result) = length;
		Z_TYPE_P(result) = Z_TYPE_P(op1);
	}
	return SUCCESS;
}
/* }}} */

ZEND_API int concat_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	zval op1_copy, op2_copy;
	int use_copy1 = 0, use_copy2 = 0;
	zend_uchar result_type;

	if (Z_TYPE_P(op1) == IS_STRING && Z_TYPE_P(op2) == IS_STRING) {
		result_type = IS_STRING;
	} else if (Z_TYPE_P(op1) == IS_UNICODE || Z_TYPE_P(op2) == IS_UNICODE || UG(unicode)) {
		zend_make_unicode_zval(op1, &op1_copy, &use_copy1);
		zend_make_unicode_zval(op2, &op2_copy, &use_copy2);
		result_type = IS_UNICODE;
	} else {
		result_type = IS_STRING;
		zend_make_string_zval(op1, &op1_copy, &use_copy1);
		zend_make_string_zval(op2, &op2_copy, &use_copy2);
	}

	if (use_copy1) {
		/* We have created a converted copy of op1. Therefore, op1 won't become the result so
		 * we have to free it.
		 */
		if (result == op1) {
			zval_dtor(op1);
		}
		op1 = &op1_copy;
	}
	if (use_copy2) {
		op2 = &op2_copy;
	}
	if (result==op1) {	/* special case, perform operations on result */
		add_string_to_string(result, op1, op2);
	} else {
		if (result_type == IS_UNICODE) {
			Z_USTRLEN_P(result) = Z_USTRLEN_P(op1) + Z_USTRLEN_P(op2);
			Z_USTRVAL_P(result) = eumalloc(Z_USTRLEN_P(result) + 1);
			u_memcpy(Z_USTRVAL_P(result), Z_USTRVAL_P(op1), Z_USTRLEN_P(op1));
			u_memcpy(Z_USTRVAL_P(result)+Z_USTRLEN_P(op1), Z_USTRVAL_P(op2), Z_USTRLEN_P(op2));
			Z_USTRVAL_P(result)[Z_USTRLEN_P(result)] = 0;
			Z_TYPE_P(result) = IS_UNICODE;
		} else {
			Z_STRLEN_P(result) = Z_STRLEN_P(op1) + Z_STRLEN_P(op2);
			Z_STRVAL_P(result) = (char *) emalloc(Z_STRLEN_P(result) + 1);
			memcpy(Z_STRVAL_P(result), Z_STRVAL_P(op1), Z_STRLEN_P(op1));
			memcpy(Z_STRVAL_P(result)+Z_STRLEN_P(op1), Z_STRVAL_P(op2), Z_STRLEN_P(op2));
			Z_STRVAL_P(result)[Z_STRLEN_P(result)] = 0;
			Z_TYPE_P(result) = result_type;
		}
	}
	if (use_copy1) {
		zval_dtor(op1);
	}
	if (use_copy2) {
		zval_dtor(op2);
	}
	return SUCCESS;
}
/* }}} */

ZEND_API int string_compare_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	zval op1_copy, op2_copy;
	int use_copy1, use_copy2;

	if (UG(unicode)) {
		zend_make_unicode_zval(op1, &op1_copy, &use_copy1);
		zend_make_unicode_zval(op2, &op2_copy, &use_copy2);
	} else {
		zend_make_string_zval(op1, &op1_copy, &use_copy1);
		zend_make_string_zval(op2, &op2_copy, &use_copy2);
	}

	if (use_copy1) {
		op1 = &op1_copy;
	}
	if (use_copy2) {
		op2 = &op2_copy;
	}

	if (UG(unicode)) {
		Z_LVAL_P(result) = zend_u_binary_zval_strcmp(op1, op2);
	} else {
		Z_LVAL_P(result) = zend_binary_zval_strcmp(op1, op2);
	}
	Z_TYPE_P(result) = IS_LONG;

	if (use_copy1) {
		zval_dtor(op1);
	}
	if (use_copy2) {
		zval_dtor(op2);
	}
	return SUCCESS;
}
/* }}} */

ZEND_API int string_locale_compare_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	zval op1_copy, op2_copy;
	int use_copy1, use_copy2;

	if (UG(unicode)) {
		zend_make_unicode_zval(op1, &op1_copy, &use_copy1);
		zend_make_unicode_zval(op2, &op2_copy, &use_copy2);
	} else {
		zend_make_string_zval(op1, &op1_copy, &use_copy1);
		zend_make_string_zval(op2, &op2_copy, &use_copy2);
	}

	if (use_copy1) {
		op1 = &op1_copy;
	}
	if (use_copy2) {
		op2 = &op2_copy;
	}

	if (UG(unicode)) {
		Z_LVAL_P(result) = ucol_strcoll(UG(default_collator)->coll, Z_USTRVAL_P(op1), Z_USTRLEN_P(op1), Z_USTRVAL_P(op2), Z_USTRLEN_P(op2));
	} else {
		Z_LVAL_P(result) = strcoll(Z_STRVAL_P(op1), Z_STRVAL_P(op2));
	}
	Z_TYPE_P(result) = IS_LONG;

	if (use_copy1) {
		zval_dtor(op1);
	}
	if (use_copy2) {
		zval_dtor(op2);
	}

	return SUCCESS;
}
/* }}} */

ZEND_API int numeric_compare_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	zval op1_copy, op2_copy;

	op1_copy = *op1;
	zval_copy_ctor(&op1_copy);

	op2_copy = *op2;
	zval_copy_ctor(&op2_copy);

	convert_to_double(&op1_copy);
	convert_to_double(&op2_copy);

	ZVAL_LONG(result, ZEND_NORMALIZE_BOOL(Z_DVAL(op1_copy)-Z_DVAL(op2_copy)));

	return SUCCESS;
}
/* }}} */

static inline void zend_free_obj_get_result(zval *op TSRMLS_DC) /* {{{ */
{
	if (Z_REFCOUNT_P(op) == 0) {
		GC_REMOVE_ZVAL_FROM_BUFFER(op);
		zval_dtor(op);
		FREE_ZVAL(op);
	} else {
		zval_ptr_dtor(&op);
	}
}
/* }}} */

ZEND_API int compare_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
    int ret;
	int converted = 0;
	zval op1_copy, op2_copy;
	zval *op_free;

	while (1) {
		switch (TYPE_PAIR(Z_TYPE_P(op1), Z_TYPE_P(op2))) {
			case TYPE_PAIR(IS_LONG, IS_LONG):
				ZVAL_LONG(result, Z_LVAL_P(op1)>Z_LVAL_P(op2)?1:(Z_LVAL_P(op1)<Z_LVAL_P(op2)?-1:0));
				return SUCCESS;

			case TYPE_PAIR(IS_DOUBLE, IS_LONG):
				Z_DVAL_P(result) = Z_DVAL_P(op1) - (double)Z_LVAL_P(op2);
				ZVAL_LONG(result, ZEND_NORMALIZE_BOOL(Z_DVAL_P(result)));
				return SUCCESS;

			case TYPE_PAIR(IS_LONG, IS_DOUBLE):
				Z_DVAL_P(result) = (double)Z_LVAL_P(op1) - Z_DVAL_P(op2);
				ZVAL_LONG(result, ZEND_NORMALIZE_BOOL(Z_DVAL_P(result)));
				return SUCCESS;

			case TYPE_PAIR(IS_DOUBLE, IS_DOUBLE):
				Z_DVAL_P(result) = Z_DVAL_P(op1) - Z_DVAL_P(op2);
				ZVAL_LONG(result, ZEND_NORMALIZE_BOOL(Z_DVAL_P(result)));
				return SUCCESS;

			case TYPE_PAIR(IS_ARRAY, IS_ARRAY):
				zend_compare_arrays(result, op1, op2 TSRMLS_CC);
				return SUCCESS;

			case TYPE_PAIR(IS_NULL, IS_NULL):
				ZVAL_LONG(result, 0);
				return SUCCESS;

			case TYPE_PAIR(IS_NULL, IS_BOOL):
				ZVAL_LONG(result, Z_LVAL_P(op2) ? -1 : 0);
				return SUCCESS;

			case TYPE_PAIR(IS_BOOL, IS_NULL):
				ZVAL_LONG(result, Z_LVAL_P(op1) ? 1 : 0);
				return SUCCESS;

			case TYPE_PAIR(IS_BOOL, IS_BOOL):
				ZVAL_LONG(result, ZEND_NORMALIZE_BOOL(Z_LVAL_P(op1) - Z_LVAL_P(op2)));
				return SUCCESS;

			case TYPE_PAIR(IS_STRING, IS_STRING):
				zendi_smart_strcmp(result, op1, op2);
				return SUCCESS;

			case TYPE_PAIR(IS_UNICODE, IS_UNICODE):
				zendi_u_smart_strcmp(result, op1, op2);
				return SUCCESS;

			case TYPE_PAIR(IS_STRING, IS_UNICODE):
				zendi_u_smart_strcmp(result, op1, op2);
				return SUCCESS;

			case TYPE_PAIR(IS_UNICODE, IS_STRING):
				zendi_u_smart_strcmp(result, op1, op2);
				return SUCCESS;

			case TYPE_PAIR(IS_NULL, IS_STRING):
				ZVAL_LONG(result, zend_binary_strcmp("", 0, Z_STRVAL_P(op2), Z_STRLEN_P(op2)));
				return SUCCESS;

			case TYPE_PAIR(IS_STRING, IS_NULL):
				ZVAL_LONG(result, zend_binary_strcmp(Z_STRVAL_P(op1), Z_STRLEN_P(op1), "", 0));
				return SUCCESS;

			case TYPE_PAIR(IS_NULL, IS_UNICODE): {
				UChar *empty_str = USTR_MAKE("");

				ZVAL_LONG(result, zend_u_binary_strcmp(empty_str, 0, Z_USTRVAL_P(op2), Z_USTRLEN_P(op2)));
				efree(empty_str);
				return SUCCESS;
			}

			case TYPE_PAIR(IS_UNICODE, IS_NULL): {
				UChar *empty_str = USTR_MAKE("");

				ZVAL_LONG(result, zend_u_binary_strcmp(Z_USTRVAL_P(op1), Z_USTRLEN_P(op1), empty_str, 0));
				efree(empty_str);
				return SUCCESS;
			}

			case TYPE_PAIR(IS_OBJECT, IS_NULL):
				ZVAL_LONG(result, 1);
				return SUCCESS;

			case TYPE_PAIR(IS_NULL, IS_OBJECT):
				ZVAL_LONG(result, -1);
				return SUCCESS;

			case TYPE_PAIR(IS_OBJECT, IS_OBJECT):
				/* If both are objects sharing the same comparision handler then use is */
				if (Z_OBJ_HANDLER_P(op1,compare_objects) == Z_OBJ_HANDLER_P(op2,compare_objects)) {
					if (Z_OBJ_HANDLE_P(op1) == Z_OBJ_HANDLE_P(op2)) {
						/* object handles are identical, apprently this is the same object */
						ZVAL_LONG(result, 0);
						return SUCCESS;
					}
					ZVAL_LONG(result, Z_OBJ_HT_P(op1)->compare_objects(op1, op2 TSRMLS_CC));
					return SUCCESS;
				}
				/* break missing intentionally */

			default:
				if (Z_TYPE_P(op1) == IS_OBJECT) {
					if (Z_OBJ_HT_P(op1)->get) {
						op_free = Z_OBJ_HT_P(op1)->get(op1 TSRMLS_CC);
						ret = compare_function(result, op_free, op2 TSRMLS_CC);
						zend_free_obj_get_result(op_free TSRMLS_CC);
						return ret;
					} else if (Z_TYPE_P(op2) != IS_OBJECT &&
					           Z_OBJ_HT_P(op1)->cast_object) {
						ALLOC_INIT_ZVAL(op_free);
						if (Z_OBJ_HT_P(op1)->cast_object(op1, op_free, Z_TYPE_P(op2), NULL TSRMLS_CC) == FAILURE) {
							ZVAL_LONG(result, 1);
							zend_free_obj_get_result(op_free TSRMLS_CC);
							return SUCCESS;
						}
						ret = compare_function(result, op_free, op2 TSRMLS_CC);
						zend_free_obj_get_result(op_free TSRMLS_CC);
						return ret;
					}
				}
				if (Z_TYPE_P(op2) == IS_OBJECT) {
					if (Z_OBJ_HT_P(op2)->get) {
						op_free = Z_OBJ_HT_P(op2)->get(op2 TSRMLS_CC);
						ret = compare_function(result, op1, op_free TSRMLS_CC);
						zend_free_obj_get_result(op_free TSRMLS_CC);
						return ret;
					} else if (Z_TYPE_P(op1) != IS_OBJECT &&
					           Z_OBJ_HT_P(op2)->cast_object) {
						ALLOC_INIT_ZVAL(op_free);
						if (Z_OBJ_HT_P(op2)->cast_object(op2, op_free, Z_TYPE_P(op1), NULL TSRMLS_CC) == FAILURE) {
							ZVAL_LONG(result, -1);
							zend_free_obj_get_result(op_free TSRMLS_CC);
							return SUCCESS;
						}
						ret = compare_function(result, op1, op_free TSRMLS_CC);
						zend_free_obj_get_result(op_free TSRMLS_CC);
						return ret;
					}
				}
				if (!converted) {
					if (Z_TYPE_P(op1) == IS_NULL) {
						zendi_convert_to_boolean(op2, op2_copy, result);
						ZVAL_LONG(result, Z_LVAL_P(op2) ? -1 : 0);
						return SUCCESS;
					} else if (Z_TYPE_P(op2) == IS_NULL) {
						zendi_convert_to_boolean(op1, op1_copy, result);
						ZVAL_LONG(result, Z_LVAL_P(op1) ? 1 : 0);
						return SUCCESS;
					} else if (Z_TYPE_P(op1) == IS_BOOL) {
						zendi_convert_to_boolean(op2, op2_copy, result);
						ZVAL_LONG(result, ZEND_NORMALIZE_BOOL(Z_LVAL_P(op1) - Z_LVAL_P(op2)));
						return SUCCESS;
					} else if (Z_TYPE_P(op2) == IS_BOOL) {
						zendi_convert_to_boolean(op1, op1_copy, result);
						ZVAL_LONG(result, ZEND_NORMALIZE_BOOL(Z_LVAL_P(op1) - Z_LVAL_P(op2)));
						return SUCCESS;
					} else {
						zendi_convert_scalar_to_number(op1, op1_copy, result);
						zendi_convert_scalar_to_number(op2, op2_copy, result);
						converted = 1;
					}
				} else if (Z_TYPE_P(op1)==IS_ARRAY) {
					ZVAL_LONG(result, 1);
					return SUCCESS;
				} else if (Z_TYPE_P(op2)==IS_ARRAY) {
					ZVAL_LONG(result, -1);
					return SUCCESS;
				} else if (Z_TYPE_P(op1)==IS_OBJECT) {
					ZVAL_LONG(result, 1);
					return SUCCESS;
				} else if (Z_TYPE_P(op2)==IS_OBJECT) {
					ZVAL_LONG(result, -1);
					return SUCCESS;
				} else {
					ZVAL_LONG(result, 0);
					return FAILURE;
				}
		}
	}
}
/* }}} */

static int hash_zval_identical_function(const zval **z1, const zval **z2) /* {{{ */
{
	zval result;
	TSRMLS_FETCH();

	/* is_identical_function() returns 1 in case of identity and 0 in case
	 * of a difference;
	 * whereas this comparison function is expected to return 0 on identity,
	 * and non zero otherwise.
	 */
	if (is_identical_function(&result, (zval *) *z1, (zval *) *z2 TSRMLS_CC)==FAILURE) {
		return 1;
	}
	return !Z_LVAL(result);
}
/* }}} */

ZEND_API int is_identical_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	Z_TYPE_P(result) = IS_BOOL;
	if (Z_TYPE_P(op1) != Z_TYPE_P(op2)) {
		Z_LVAL_P(result) = 0;
		return SUCCESS;
	}
	switch (Z_TYPE_P(op1)) {
		case IS_NULL:
			Z_LVAL_P(result) = 1;
			break;
		case IS_BOOL:
		case IS_LONG:
		case IS_RESOURCE:
			Z_LVAL_P(result) = (Z_LVAL_P(op1) == Z_LVAL_P(op2));
			break;
		case IS_DOUBLE:
			Z_LVAL_P(result) = (Z_DVAL_P(op1) == Z_DVAL_P(op2));
			break;
		case IS_STRING:
			if ((Z_STRLEN_P(op1) == Z_STRLEN_P(op2))
				&& (!memcmp(Z_STRVAL_P(op1), Z_STRVAL_P(op2), Z_STRLEN_P(op1)))) {
				Z_LVAL_P(result) = 1;
			} else {
				Z_LVAL_P(result) = 0;
			}
			break;
		case IS_UNICODE:
			if ((Z_USTRLEN_P(op1) == Z_USTRLEN_P(op2))
				&& (!memcmp(Z_USTRVAL_P(op1), Z_USTRVAL_P(op2), UBYTES(Z_USTRLEN_P(op1))))) {
				Z_LVAL_P(result) = 1;
			} else {
				Z_LVAL_P(result) = 0;
			}
			break;
		case IS_ARRAY:
			if (zend_hash_compare(Z_ARRVAL_P(op1), Z_ARRVAL_P(op2), (compare_func_t) hash_zval_identical_function, 1 TSRMLS_CC)==0) {
				Z_LVAL_P(result) = 1;
			} else {
				Z_LVAL_P(result) = 0;
			}
			break;
		case IS_OBJECT:
			if (Z_OBJ_HT_P(op1) == Z_OBJ_HT_P(op2)) {
				Z_LVAL_P(result) = (Z_OBJ_HANDLE_P(op1) == Z_OBJ_HANDLE_P(op2));
			} else {
				Z_LVAL_P(result) = 0;
			}
			break;
		default:
			Z_LVAL_P(result) = 0;
			return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

ZEND_API int is_not_identical_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
   if (is_identical_function(result, op1, op2 TSRMLS_CC) == FAILURE) {
	  return FAILURE;
   }
   Z_LVAL_P(result) = !Z_LVAL_P(result);
   return SUCCESS;
}
/* }}} */

ZEND_API int is_equal_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	if (compare_function(result, op1, op2 TSRMLS_CC) == FAILURE) {
		return FAILURE;
	}
	ZVAL_BOOL(result, (Z_LVAL_P(result) == 0));
	return SUCCESS;
}
/* }}} */

ZEND_API int is_not_equal_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	if (compare_function(result, op1, op2 TSRMLS_CC) == FAILURE) {
		return FAILURE;
	}
	ZVAL_BOOL(result, (Z_LVAL_P(result) != 0));
	return SUCCESS;
}
/* }}} */

ZEND_API int is_smaller_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	if (compare_function(result, op1, op2 TSRMLS_CC) == FAILURE) {
		return FAILURE;
	}
	ZVAL_BOOL(result, (Z_LVAL_P(result) < 0));
	return SUCCESS;
}
/* }}} */

ZEND_API int is_smaller_or_equal_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	if (compare_function(result, op1, op2 TSRMLS_CC) == FAILURE) {
		return FAILURE;
	}
	ZVAL_BOOL(result, (Z_LVAL_P(result) <= 0));
	return SUCCESS;
}
/* }}} */

ZEND_API zend_bool instanceof_function_ex(const zend_class_entry *instance_ce, const zend_class_entry *ce, zend_bool interfaces_only TSRMLS_DC) /* {{{ */
{
	zend_uint i;

	for (i=0; i<instance_ce->num_interfaces; i++) {
		if (instanceof_function(instance_ce->interfaces[i], ce TSRMLS_CC)) {
			return 1;
		}
	}
	if (!interfaces_only) {
		while (instance_ce) {
			if (instance_ce == ce) {
				return 1;
			}
			instance_ce = instance_ce->parent;
		}
	}

	return 0;
}
/* }}} */

ZEND_API zend_bool instanceof_function(const zend_class_entry *instance_ce, const zend_class_entry *ce TSRMLS_DC) /* {{{ */
{
	return instanceof_function_ex(instance_ce, ce, 0 TSRMLS_CC);
}
/* }}} */

#define LOWER_CASE 1
#define UPPER_CASE 2
#define NUMERIC 3

static void increment_string(zval *str) /* {{{ */
{
    int carry=0;
    int pos=Z_STRLEN_P(str)-1;
    char *s=Z_STRVAL_P(str);
    char *t;
    int last=0; /* Shut up the compiler warning */
    int ch;

	if (Z_STRLEN_P(str) == 0) {
		STR_FREE(Z_STRVAL_P(str));
		Z_STRVAL_P(str) = estrndup("1", sizeof("1")-1);
		Z_STRLEN_P(str) = 1;
		return;
	}

	while (pos >= 0) {
        ch = s[pos];
        if (ch >= 'a' && ch <= 'z') {
            if (ch == 'z') {
                s[pos] = 'a';
                carry=1;
            } else {
                s[pos]++;
                carry=0;
            }
            last=LOWER_CASE;
        } else if (ch >= 'A' && ch <= 'Z') {
            if (ch == 'Z') {
                s[pos] = 'A';
                carry=1;
            } else {
                s[pos]++;
                carry=0;
            }
            last=UPPER_CASE;
        } else if (ch >= '0' && ch <= '9') {
            if (ch == '9') {
                s[pos] = '0';
                carry=1;
            } else {
                s[pos]++;
                carry=0;
            }
            last = NUMERIC;
        } else {
            carry=0;
            break;
        }
        if (carry == 0) {
            break;
        }
        pos--;
    }

    if (carry) {
        t = (char *) emalloc(Z_STRLEN_P(str)+1+1);
        memcpy(t+1, Z_STRVAL_P(str), Z_STRLEN_P(str));
        Z_STRLEN_P(str)++;
        t[Z_STRLEN_P(str)] = '\0';
        switch (last) {
            case NUMERIC:
            	t[0] = '1';
            	break;
            case UPPER_CASE:
            	t[0] = 'A';
            	break;
            case LOWER_CASE:
            	t[0] = 'a';
            	break;
        }
        STR_FREE(Z_STRVAL_P(str));
        Z_STRVAL_P(str) = t;
    }
}
/* }}} */

static void increment_unicode(zval *str) /* {{{ */
{
    int carry=0;
    int pos=Z_USTRLEN_P(str)-1;
    UChar *s=Z_USTRVAL_P(str);
    UChar *t;
    int last=0; /* Shut up the compiler warning */
    int ch;

	if (Z_USTRLEN_P(str) == 0) {
		TSRMLS_FETCH();

		USTR_FREE(Z_USTRVAL_P(str));
		ZVAL_ASCII_STRINGL(str, "1", sizeof("1")-1, 1);
		return;
	}

	while (pos >= 0) {
        ch = s[pos];
        if (ch >= 'a' && ch <= 'z') {
            if (ch == 'z') {
                s[pos] = 'a';
                carry=1;
            } else {
                s[pos]++;
                carry=0;
            }
            last=LOWER_CASE;
        } else if (ch >= 'A' && ch <= 'Z') {
            if (ch == 'Z') {
                s[pos] = 'A';
                carry=1;
            } else {
                s[pos]++;
                carry=0;
            }
            last=UPPER_CASE;
        } else if (ch >= '0' && ch <= '9') {
            if (ch == '9') {
                s[pos] = '0';
                carry=1;
            } else {
                s[pos]++;
                carry=0;
            }
            last = NUMERIC;
        } else {
            carry=0;
            break;
        }
        if (carry == 0) {
            break;
        }
        pos--;
    }

    if (carry) {
        t = (UChar *) eumalloc(Z_USTRLEN_P(str)+1+1);
        memcpy(t+1, Z_USTRVAL_P(str), UBYTES(Z_USTRLEN_P(str)));
        Z_USTRLEN_P(str)++;
        t[Z_USTRLEN_P(str)] = 0;
        switch (last) {
            case NUMERIC:
            	t[0] = '1';
            	break;
            case UPPER_CASE:
            	t[0] = 'A';
            	break;
            case LOWER_CASE:
            	t[0] = 'a';
            	break;
        }
        USTR_FREE(Z_USTRVAL_P(str));
        Z_USTRVAL_P(str) = t;
    }
}
/* }}} */

ZEND_API int increment_function(zval *op1) /* {{{ */
{
	switch (Z_TYPE_P(op1)) {
		case IS_LONG:
			if (Z_LVAL_P(op1) == LONG_MAX) {
				/* switch to double */
				double d = (double)Z_LVAL_P(op1);
				ZVAL_DOUBLE(op1, d+1);
			} else {
			Z_LVAL_P(op1)++;
			}
			break;
		case IS_DOUBLE:
			Z_DVAL_P(op1) = Z_DVAL_P(op1) + 1;
			break;
		case IS_NULL:
			Z_LVAL_P(op1) = 1;
			Z_TYPE_P(op1) = IS_LONG;
			break;
		case IS_STRING: {
				long lval;
				double dval;
				char *strval = Z_STRVAL_P(op1);

				switch (is_numeric_string(strval, Z_STRLEN_P(op1), &lval, &dval, 0)) {
					case IS_LONG:
						if (lval == LONG_MAX) {
							/* switch to double */
							double d = (double)lval;
							ZVAL_DOUBLE(op1, d+1);
						} else {
						Z_LVAL_P(op1) = lval+1;
						Z_TYPE_P(op1) = IS_LONG;
						}
						efree(strval); /* should never be empty_string */
						break;
					case IS_DOUBLE:
						Z_DVAL_P(op1) = dval+1;
						Z_TYPE_P(op1) = IS_DOUBLE;
						efree(strval); /* should never be empty_string */
						break;
					default:
						/* Perl style string increment */
						increment_string(op1);
						break;
				}
			}
			break;
		case IS_UNICODE: {
				long lval;
				double dval;
				UChar *ustrval = Z_USTRVAL_P(op1);

				switch (is_numeric_unicode(ustrval, Z_USTRLEN_P(op1), &lval, &dval, 0)) {
					case IS_LONG:
						if (lval == LONG_MAX) {
							/* switch to double */
							double d = (double)lval;
							ZVAL_DOUBLE(op1, d+1);
						} else {
						Z_LVAL_P(op1) = lval+1;
						Z_TYPE_P(op1) = IS_LONG;
						}
						efree(ustrval); /* should never be empty_string */
						break;
					case IS_DOUBLE:
						Z_DVAL_P(op1) = dval+1;
						Z_TYPE_P(op1) = IS_DOUBLE;
						efree(ustrval); /* should never be empty_string */
						break;
					default:
						/* Perl style string increment */
						increment_unicode(op1);
						break;
				}
			}
			break;
		default:
			return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

ZEND_API int decrement_function(zval *op1) /* {{{ */
{
	long lval;
	double dval;

	switch (Z_TYPE_P(op1)) {
		case IS_LONG:
			if (Z_LVAL_P(op1) == LONG_MIN) {
				double d = (double)Z_LVAL_P(op1);
				ZVAL_DOUBLE(op1, d-1);
			} else {
			Z_LVAL_P(op1)--;
			}
			break;
		case IS_DOUBLE:
			Z_DVAL_P(op1) = Z_DVAL_P(op1) - 1;
			break;
		case IS_STRING:		/* Like perl we only support string increment */
			if (Z_STRLEN_P(op1) == 0) { /* consider as 0 */
				STR_FREE(Z_STRVAL_P(op1));
				Z_LVAL_P(op1) = -1;
				Z_TYPE_P(op1) = IS_LONG;
				break;
			}
			switch (is_numeric_string(Z_STRVAL_P(op1), Z_STRLEN_P(op1), &lval, &dval, 0)) {
				case IS_LONG:
					STR_FREE(Z_STRVAL_P(op1));
					if (lval == LONG_MIN) {
						double d = (double)lval;
						ZVAL_DOUBLE(op1, d-1);
					} else {
						Z_LVAL_P(op1) = lval-1;
						Z_TYPE_P(op1) = IS_LONG;
					}
					break;
				case IS_DOUBLE:
					STR_FREE(Z_STRVAL_P(op1));
					Z_DVAL_P(op1) = dval - 1;
					Z_TYPE_P(op1) = IS_DOUBLE;
					break;
			}
			break;
		case IS_UNICODE:
			if (Z_USTRLEN_P(op1) == 0) { /* consider as 0 */
				USTR_FREE(Z_USTRVAL_P(op1));
				Z_LVAL_P(op1) = -1;
				Z_TYPE_P(op1) = IS_LONG;
				break;
			}
			switch (is_numeric_unicode(Z_USTRVAL_P(op1), Z_USTRLEN_P(op1), &lval, &dval, 0)) {
				case IS_LONG:
					USTR_FREE(Z_USTRVAL_P(op1));
					if (lval == LONG_MIN) {
						double d = (double)lval;
						ZVAL_DOUBLE(op1, d-1);
					} else {
						Z_LVAL_P(op1) = lval-1;
						Z_TYPE_P(op1) = IS_LONG;
					}
					break;
				case IS_DOUBLE:
					USTR_FREE(Z_USTRVAL_P(op1));
					Z_DVAL_P(op1) = dval - 1;
					Z_TYPE_P(op1) = IS_DOUBLE;
					break;
			}
			break;
		default:
			return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

ZEND_API int zval_is_true(zval *op) /* {{{ */
{
	convert_to_boolean(op);
	return (Z_LVAL_P(op) ? 1 : 0);
}
/* }}} */

#ifdef ZEND_USE_TOLOWER_L
ZEND_API void zend_update_current_locale(void) /* {{{ */
{
        current_locale = _get_current_locale();
}
/* }}} */
#endif

ZEND_API char *zend_str_tolower_copy(char *dest, const char *source, unsigned int length) /* {{{ */
{
	register unsigned char *str = (unsigned char*)source;
	register unsigned char *result = (unsigned char*)dest;
	register unsigned char *end = str + length;

	while (str < end) {
		*result++ = zend_tolower((int)*str++);
	}
	*result = '\0';

	return dest;
}
/* }}} */

ZEND_API char *zend_str_tolower_dup(const char *source, unsigned int length) /* {{{ */
{
	return zend_str_tolower_copy((char *)emalloc(length+1), source, length);
}
/* }}} */

ZEND_API zstr zend_u_str_tolower_copy(zend_uchar type, zstr dest, zstr source, unsigned int length) /* {{{ */
{
	if (type == IS_UNICODE) {
		register UChar *str = source.u;
		register UChar *result = dest.u;
		register UChar *end = str + length;

		while (str < end) {
			*result++ = u_tolower((int)*str++);
		}
		*result = '\0';

		return dest;
	} else {
		zstr ret;

		ret.s = zend_str_tolower_copy(dest.s, source.s, length);
		return ret;
	}
}
/* }}} */

ZEND_API zstr zend_u_str_tolower_dup(zend_uchar type, zstr source, unsigned int length) /* {{{ */
{
	zstr ret;

	if (type == IS_UNICODE) {
		ret.u = eumalloc(length+1);
		ret = zend_u_str_tolower_copy(IS_UNICODE, ret, source, length);
	} else {
		ret.s = zend_str_tolower_copy((char*)emalloc(length+1), source.s, length);
	}
	return ret;
}
/* }}} */

ZEND_API void zend_str_tolower(char *str, unsigned int length) /* {{{ */
{
	register unsigned char *p = (unsigned char*)str;
	register unsigned char *end = p + length;

	while (p < end) {
		*p = zend_tolower((int)*p);
		p++;
	}
}
/* }}} */

ZEND_API void zend_u_str_tolower(zend_uchar type, zstr str, unsigned int length) /* {{{ */
{
	if (type == IS_UNICODE) {
		register UChar *p = str.u;
		register UChar *end = p + length;

		while (p < end) {
			*p = u_tolower((int)*p);
			p++;
		}
	} else {
		zend_str_tolower(str.s, length);
	}
}
/* }}} */

ZEND_API zstr zend_u_str_case_fold(zend_uchar type, zstr source, unsigned int length, zend_bool normalize, unsigned int *new_len) /* {{{ */
{
	zstr ret;

	if (type == IS_UNICODE) {
		int ret_len = length;

		if (normalize) {
			if (zend_normalize_identifier(&ret.u, &ret_len, source.u, length, 1) == FAILURE) {
				zend_error(E_NOTICE, "Could not normalize identifier");
				ret.u = eustrndup(source.u, length);
			}
		} else {
			UErrorCode status = U_ZERO_ERROR;

			zend_case_fold_string(&ret.u, &ret_len, source.u, length, U_FOLD_CASE_DEFAULT, &status);
			if (U_FAILURE(status)) {
				zend_error(E_NOTICE, "Could not case-fold string");
				ret.u = eustrndup(source.u, length);
			}
		}

		*new_len = ret_len;
	} else {
		*new_len = length;
		ret.s = zend_str_tolower_dup(source.s, length);
	}
	return ret;
}
/* }}} */

ZEND_API int zend_binary_strcmp(const char *s1, uint len1, const char *s2, uint len2) /* {{{ */
{
	int retval;

	retval = memcmp(s1, s2, MIN(len1, len2));
	if (!retval) {
		return (len1 - len2);
	} else {
		return retval;
	}
}
/* }}} */

ZEND_API int zend_u_binary_strcmp(UChar *s1, int len1, UChar *s2, int len2) /* {{{ */
{
	int result = u_strCompare(s1, len1, s2, len2, 1);
	return ZEND_NORMALIZE_BOOL(result);
}
/* }}} */

ZEND_API int zend_binary_strncmp(const char *s1, uint len1, const char *s2, uint len2, uint length) /* {{{ */
{
	int retval;

	retval = memcmp(s1, s2, MIN(length, MIN(len1, len2)));
	if (!retval) {
		return (MIN(length, len1) - MIN(length, len2));
	} else {
		return retval;
	}
}
/* }}} */

ZEND_API int zend_u_binary_strncmp(UChar *s1, int len1, UChar *s2, int len2, uint length) /* {{{ */
{
	int32_t off1 = 0, off2 = 0;
	UChar32 c1, c2;
	int result = 0;

	for( ; length > 0; --length) {
		if (off1 >= len1 || off2 >= len2) {
			result = len1 - len2;
			return ZEND_NORMALIZE_BOOL(result);
		}
		U16_NEXT(s1, off1, len1, c1);
		U16_NEXT(s2, off2, len2, c2);
		if (c1 != c2) {
			result = (int32_t)c1-(int32_t)c2;
			return ZEND_NORMALIZE_BOOL(result);
		}
	}

	return 0;
}
/* }}} */

ZEND_API int zend_binary_strcasecmp(const char *s1, uint len1, const char *s2, uint len2) /* {{{ */
{
	int len;
	int c1, c2;

	len = MIN(len1, len2);

	while (len--) {
		c1 = zend_tolower((int)*(unsigned char *)s1++);
		c2 = zend_tolower((int)*(unsigned char *)s2++);
		if (c1 != c2) {
			return c1 - c2;
		}
	}

	return len1 - len2;
}
/* }}} */

ZEND_API int zend_u_binary_strcasecmp(UChar *s1, int len1, UChar *s2, int len2) /* {{{ */
{
	UErrorCode status = U_ZERO_ERROR;
	int result = u_strCaseCompare(s1, len1, s2, len2, U_COMPARE_CODE_POINT_ORDER, &status);
	return ZEND_NORMALIZE_BOOL(result);
}
/* }}} */

ZEND_API int zend_binary_strncasecmp(const char *s1, uint len1, const char *s2, uint len2, uint length) /* {{{ */
{
	int len;
	int c1, c2;

	len = MIN(length, MIN(len1, len2));

	while (len--) {
		c1 = zend_tolower((int)*(unsigned char *)s1++);
		c2 = zend_tolower((int)*(unsigned char *)s2++);
		if (c1 != c2) {
			return c1 - c2;
		}
	}

	return MIN(length, len1) - MIN(length, len2);
}
/* }}} */

/*
 * Because we do not know UChar offsets for the passed-in limit of the number of
 * codepoints to compare, we take a hit upfront by iterating over both strings
 * until we find them. Then we can simply use u_strCaseCompare().
 */
ZEND_API int zend_u_binary_strncasecmp(UChar *s1, int len1, UChar *s2, int len2, uint length) /* {{{ */
{
	UErrorCode status = U_ZERO_ERROR;
	int32_t off1 = 0, off2 = 0;
	int result;

	U16_FWD_N(s1, off1, len1, length);
	U16_FWD_N(s2, off2, len2, length);
	result = u_strCaseCompare(s1, off1, s2, off2, U_COMPARE_CODE_POINT_ORDER, &status);
	return ZEND_NORMALIZE_BOOL(result);
}
/* }}} */

ZEND_API int zend_binary_zval_strcmp(zval *s1, zval *s2) /* {{{ */
{
	return zend_binary_strcmp(Z_STRVAL_P(s1), Z_STRLEN_P(s1), Z_STRVAL_P(s2), Z_STRLEN_P(s2));
}
/* }}} */

ZEND_API int zend_u_binary_zval_strcmp(zval *s1, zval *s2) /* {{{ */
{
	return zend_u_binary_strcmp(Z_USTRVAL_P(s1), Z_USTRLEN_P(s1), Z_USTRVAL_P(s2), Z_USTRLEN_P(s2));
}
/* }}} */

ZEND_API int zend_binary_zval_strncmp(zval *s1, zval *s2, zval *s3) /* {{{ */
{
	return zend_binary_strncmp(Z_STRVAL_P(s1), Z_STRLEN_P(s1), Z_STRVAL_P(s2), Z_STRLEN_P(s2), Z_LVAL_P(s3));
}
/* }}} */

ZEND_API int zend_binary_zval_strcasecmp(zval *s1, zval *s2) /* {{{ */
{
	return zend_binary_strcasecmp(Z_STRVAL_P(s1), Z_STRLEN_P(s1), Z_STRVAL_P(s2), Z_STRLEN_P(s2));
}
/* }}} */

ZEND_API int zend_binary_zval_strncasecmp(zval *s1, zval *s2, zval *s3) /* {{{ */
{
	return zend_binary_strncasecmp(Z_STRVAL_P(s1), Z_STRLEN_P(s1), Z_STRVAL_P(s2), Z_STRLEN_P(s2), Z_LVAL_P(s3));
}
/* }}} */

ZEND_API void zendi_smart_strcmp(zval *result, zval *s1, zval *s2) /* {{{ */
{
	int ret1, ret2;
	long lval1, lval2;
	double dval1, dval2;

	if ((ret1=is_numeric_string(Z_STRVAL_P(s1), Z_STRLEN_P(s1), &lval1, &dval1, 0)) &&
		(ret2=is_numeric_string(Z_STRVAL_P(s2), Z_STRLEN_P(s2), &lval2, &dval2, 0))) {
		if ((ret1==IS_DOUBLE) || (ret2==IS_DOUBLE)) {
			if (ret1!=IS_DOUBLE) {
				dval1 = (double) lval1;
			} else if (ret2!=IS_DOUBLE) {
				dval2 = (double) lval2;
			} else if (dval1 == dval2 && !zend_finite(dval1)) {
				/* Both values overflowed and have the same sign,
				 * so a numeric comparison would be inaccurate */
				goto string_cmp;
			}
			Z_DVAL_P(result) = dval1 - dval2;
			ZVAL_LONG(result, ZEND_NORMALIZE_BOOL(Z_DVAL_P(result)));
		} else { /* they both have to be long's */
			ZVAL_LONG(result, lval1 > lval2 ? 1 : (lval1 < lval2 ? -1 : 0));
		}
	} else {
string_cmp:
		Z_LVAL_P(result) = zend_binary_zval_strcmp(s1, s2);
		Z_LVAL_P(result) = ZEND_NORMALIZE_BOOL(Z_LVAL_P(result));
		Z_TYPE_P(result) = IS_LONG;
	}
	return;
}
/* }}} */

ZEND_API void zendi_u_smart_strcmp(zval *result, zval *s1, zval *s2) /* {{{ */
{
	int ret1, ret2;
	long lval1, lval2;
	double dval1, dval2;
	zval s1_copy, s2_copy;
	int use_copy1 = 0, use_copy2 = 0;

	if (Z_TYPE_P(s1) != IS_UNICODE || Z_TYPE_P(s2) != IS_UNICODE) {
		zend_make_unicode_zval(s1, &s1_copy, &use_copy1);
		zend_make_unicode_zval(s2, &s2_copy, &use_copy2);
		if (use_copy1) {
			s1 = &s1_copy;
		}
		if (use_copy2) {
			s2 = &s2_copy;
		}
	}

	if ((ret1=is_numeric_unicode(Z_USTRVAL_P(s1), Z_USTRLEN_P(s1), &lval1, &dval1, 0)) &&
		(ret2=is_numeric_unicode(Z_USTRVAL_P(s2), Z_USTRLEN_P(s2), &lval2, &dval2, 0))) {
		if ((ret1==IS_DOUBLE) || (ret2==IS_DOUBLE)) {
			if (ret1!=IS_DOUBLE) {
				dval1 = (double) lval1;
			} else if (ret2!=IS_DOUBLE) {
				dval2 = (double) lval2;
			} else if (dval1 == dval2 && !zend_finite(dval1)) {
				/* Both values overflowed and have the same sign,
				 * so a numeric comparison would be inaccurate */
				goto string_cmp;
			}
			Z_DVAL_P(result) = dval1 - dval2;
			ZVAL_LONG(result, ZEND_NORMALIZE_BOOL(Z_DVAL_P(result)));
		} else { /* they both have to be long's */
			ZVAL_LONG(result, lval1 > lval2 ? 1 : (lval1 < lval2 ? -1 : 0));
		}
	} else {
string_cmp:
		Z_LVAL_P(result) = zend_u_binary_zval_strcmp(s1, s2);
		Z_LVAL_P(result) = ZEND_NORMALIZE_BOOL(Z_LVAL_P(result));
		Z_TYPE_P(result) = IS_LONG;
	}

	if (use_copy1) {
		zval_dtor(s1);
	}
	if (use_copy2) {
		zval_dtor(s2);
	}
	return;
}
/* }}} */

static int hash_zval_compare_function(const zval **z1, const zval **z2 TSRMLS_DC) /* {{{ */
{
	zval result;

	if (compare_function(&result, (zval *) *z1, (zval *) *z2 TSRMLS_CC)==FAILURE) {
		return 1;
	}
	return Z_LVAL(result);
}
/* }}} */

ZEND_API int zend_compare_symbol_tables_i(HashTable *ht1, HashTable *ht2 TSRMLS_DC) /* {{{ */
{
	return zend_hash_compare(ht1, ht2, (compare_func_t) hash_zval_compare_function, 0 TSRMLS_CC);
}
/* }}} */

ZEND_API void zend_compare_symbol_tables(zval *result, HashTable *ht1, HashTable *ht2 TSRMLS_DC) /* {{{ */
{
	Z_TYPE_P(result) = IS_LONG;
	Z_LVAL_P(result) = zend_hash_compare(ht1, ht2, (compare_func_t) hash_zval_compare_function, 0 TSRMLS_CC);
}
/* }}} */

ZEND_API void zend_compare_arrays(zval *result, zval *a1, zval *a2 TSRMLS_DC) /* {{{ */
{
	zend_compare_symbol_tables(result, Z_ARRVAL_P(a1), Z_ARRVAL_P(a2) TSRMLS_CC);
}
/* }}} */

ZEND_API void zend_compare_objects(zval *result, zval *o1, zval *o2 TSRMLS_DC) /* {{{ */
{
	Z_TYPE_P(result) = IS_LONG;

	if (Z_OBJ_HANDLE_P(o1) == Z_OBJ_HANDLE_P(o2)) {
		Z_LVAL_P(result) = 0;
		return;
	}

	if (Z_OBJ_HT_P(o1)->compare_objects == NULL) {
		Z_LVAL_P(result) = 1;
	} else {
		Z_LVAL_P(result) = Z_OBJ_HT_P(o1)->compare_objects(o1, o2 TSRMLS_CC);
	}
}
/* }}} */

ZEND_API void zend_locale_sprintf_double(zval *op ZEND_FILE_LINE_DC) /* {{{ */
{
	TSRMLS_FETCH();

	Z_STRLEN_P(op) = zend_spprintf(&Z_STRVAL_P(op), 0, "%.*G", (int) EG(precision), (double)Z_DVAL_P(op));
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
