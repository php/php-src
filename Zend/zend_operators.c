/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2012 Zend Technologies Ltd. (http://www.zend.com) |
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
#include "zend_strtod.h"
#include "zend_exceptions.h"
#include "zend_closures.h"

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

ZEND_API void convert_scalar_to_number(zval *op TSRMLS_DC) /* {{{ */
{
	switch (Z_TYPE_P(op)) {
		case IS_STRING:
			{
				char *strval;

				strval = Z_STRVAL_P(op);
				if ((Z_TYPE_P(op)=is_numeric_string(strval, Z_STRLEN_P(op), &Z_LVAL_P(op), &Z_DVAL_P(op), 1)) == 0) {
					ZVAL_LONG(op, 0);
				}
				STR_FREE(strval);
				break;
			}
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
			ZVAL_LONG(op, 0);
			break;
	}
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
					if ((Z_TYPE(holder)=is_numeric_string(Z_STRVAL_P(op), Z_STRLEN_P(op), &Z_LVAL(holder), &Z_DVAL(holder), 1)) == 0) {	\
						ZVAL_LONG(&(holder), 0);							\
					}														\
					(op) = &(holder);										\
					break;													\
				}															\
			case IS_BOOL:													\
			case IS_RESOURCE:												\
				ZVAL_LONG(&(holder), Z_LVAL_P(op));							\
				(op) = &(holder);											\
				break;														\
			case IS_NULL:													\
				ZVAL_LONG(&(holder), 0);									\
				(op) = &(holder);											\
				break;														\
			case IS_OBJECT:													\
				(holder) = (*(op));											\
				zval_copy_ctor(&(holder));									\
				convert_to_long_base(&(holder), 10);						\
				if (Z_TYPE(holder) == IS_LONG) {							\
					(op) = &(holder);										\
				}															\
				break;														\
		}																	\
	}

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
				Z_LVAL(holder) = zend_dval_to_lval(Z_DVAL_P(op));	\
				break;												\
			case IS_STRING:											\
				Z_LVAL(holder) = strtol(Z_STRVAL_P(op), NULL, 10);	\
				break;												\
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
		if (Z_OBJ_HT_P(op)->cast_object(op, &dst, ctype TSRMLS_CC) == FAILURE) {			\
			zend_error(E_RECOVERABLE_ERROR,													\
				"Object of class %s could not be converted to %s", Z_OBJCE_P(op)->name,		\
			zend_get_type_by_const(ctype));													\
		} else {																			\
			zval_dtor(op);																	\
			Z_TYPE_P(op) = ctype;															\
			op->value = dst.value;															\
		}																					\
	} else {																				\
		if (Z_OBJ_HT_P(op)->get) {															\
			zval *newop = Z_OBJ_HT_P(op)->get(op TSRMLS_CC);								\
			if (Z_TYPE_P(newop) != IS_OBJECT) {												\
				/* for safety - avoid loop */												\
				zval_dtor(op);																\
				*op = *newop;																\
				FREE_ZVAL(newop);															\
				conv_func(op);																\
			}																				\
		}																					\
	}

/* }}} */

ZEND_API void convert_to_long(zval *op) /* {{{ */
{
	if (Z_TYPE_P(op) != IS_LONG) {
		convert_to_long_base(op, 10);
	}
}
/* }}} */

ZEND_API void convert_to_long_base(zval *op, int base) /* {{{ */
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
			Z_LVAL_P(op) = zend_dval_to_lval(Z_DVAL_P(op));
			break;
		case IS_STRING:
			{
				char *strval = Z_STRVAL_P(op);

				Z_LVAL_P(op) = strtol(strval, NULL, base);
				STR_FREE(strval);
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
					return;
				}
				zend_error(E_NOTICE, "Object of class %s could not be converted to int", Z_OBJCE_P(op)->name);

				zval_dtor(op);
				ZVAL_LONG(op, retval);
				return;
			}
		default:
			zend_error(E_WARNING, "Cannot convert to ordinal value");
			zval_dtor(op);
			Z_LVAL_P(op) = 0;
			break;
	}

	Z_TYPE_P(op) = IS_LONG;
}
/* }}} */

ZEND_API void convert_to_double(zval *op) /* {{{ */
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
					return;
				}
				zend_error(E_NOTICE, "Object of class %s could not be converted to double", Z_OBJCE_P(op)->name);

				zval_dtor(op);
				ZVAL_DOUBLE(op, retval);
				break;
			}
		default:
			zend_error(E_WARNING, "Cannot convert to real value (type=%d)", Z_TYPE_P(op));
			zval_dtor(op);
			Z_DVAL_P(op) = 0;
			break;
	}
	Z_TYPE_P(op) = IS_DOUBLE;
}
/* }}} */

ZEND_API void convert_to_null(zval *op) /* {{{ */
{
	if (Z_TYPE_P(op) == IS_OBJECT) {
		if (Z_OBJ_HT_P(op)->cast_object) {
			zval *org;
			TSRMLS_FETCH();

			ALLOC_ZVAL(org);
			*org = *op;
			if (Z_OBJ_HT_P(op)->cast_object(org, op, IS_NULL TSRMLS_CC) == SUCCESS) {
				zval_dtor(org);
				return;
			}
			*op = *org;
			FREE_ZVAL(org);
		}
	}

	zval_dtor(op);
	Z_TYPE_P(op) = IS_NULL;
}
/* }}} */

ZEND_API void convert_to_boolean(zval *op) /* {{{ */
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
					return;
				}

				zval_dtor(op);
				ZVAL_BOOL(op, retval);
				break;
			}
		default:
			zval_dtor(op);
			Z_LVAL_P(op) = 0;
			break;
	}
	Z_TYPE_P(op) = IS_BOOL;
}
/* }}} */

ZEND_API void _convert_to_string(zval *op ZEND_FILE_LINE_DC) /* {{{ */
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
			break;
		case IS_OBJECT: {
			TSRMLS_FETCH();

			convert_object_to_type(op, IS_STRING, convert_to_string);

			if (Z_TYPE_P(op) == IS_STRING) {
				return;
			}

			zend_error(E_NOTICE, "Object of class %s to string conversion", Z_OBJCE_P(op)->name);
			zval_dtor(op);
			Z_STRVAL_P(op) = estrndup_rel("Object", sizeof("Object")-1);
			Z_STRLEN_P(op) = sizeof("Object")-1;
			break;
		}
		default:
			zval_dtor(op);
			ZVAL_BOOL(op, 0);
			break;
	}
	Z_TYPE_P(op) = IS_STRING;
}
/* }}} */

static void convert_scalar_to_array(zval *op, int type TSRMLS_DC) /* {{{ */
{
	zval *entry;

	ALLOC_ZVAL(entry);
	*entry = *op;
	INIT_PZVAL(entry);

	switch (type) {
		case IS_ARRAY:
			ALLOC_HASHTABLE(Z_ARRVAL_P(op));
			zend_hash_init(Z_ARRVAL_P(op), 0, NULL, ZVAL_PTR_DTOR, 0);
			zend_hash_index_update(Z_ARRVAL_P(op), 0, (void *) &entry, sizeof(zval *), NULL);
			Z_TYPE_P(op) = IS_ARRAY;
			break;
		case IS_OBJECT:
			object_init(op);
			zend_hash_update(Z_OBJPROP_P(op), "scalar", sizeof("scalar"), (void *) &entry, sizeof(zval *), NULL);
			break;
	}
}
/* }}} */

ZEND_API void convert_to_array(zval *op) /* {{{ */
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

				ALLOC_HASHTABLE(ht);
				zend_hash_init(ht, 0, NULL, ZVAL_PTR_DTOR, 0);
				if (Z_OBJCE_P(op) == zend_ce_closure) {
					convert_scalar_to_array(op, IS_ARRAY TSRMLS_CC);
					if (Z_TYPE_P(op) == IS_ARRAY) {
						zend_hash_destroy(ht);
						FREE_HASHTABLE(ht);
						return;
					}
				} else if (Z_OBJ_HT_P(op)->get_properties) {
					HashTable *obj_ht = Z_OBJ_HT_P(op)->get_properties(op TSRMLS_CC);
					if (obj_ht) {
						zend_hash_copy(ht, obj_ht, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
					}
				} else {
					convert_object_to_type(op, IS_ARRAY, convert_to_array);

					if (Z_TYPE_P(op) == IS_ARRAY) {
						zend_hash_destroy(ht);
						FREE_HASHTABLE(ht);
						return;
					}
				}
				zval_dtor(op);
				Z_TYPE_P(op) = IS_ARRAY;
				Z_ARRVAL_P(op) = ht;
			}
			break;
		case IS_NULL:
			ALLOC_HASHTABLE(Z_ARRVAL_P(op));
			zend_hash_init(Z_ARRVAL_P(op), 0, NULL, ZVAL_PTR_DTOR, 0);
			Z_TYPE_P(op) = IS_ARRAY;
			break;
		default:
			convert_scalar_to_array(op, IS_ARRAY TSRMLS_CC);
			break;
	}
}
/* }}} */

ZEND_API void convert_to_object(zval *op) /* {{{ */
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
			object_init(op);
			break;
		default:
			convert_scalar_to_array(op, IS_OBJECT TSRMLS_CC);
			break;
	}
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
	zval op1_copy, op2_copy;
	int converted = 0;

	while (1) {
		switch (TYPE_PAIR(Z_TYPE_P(op1), Z_TYPE_P(op2))) {
			case TYPE_PAIR(IS_LONG, IS_LONG): {
				long lval = Z_LVAL_P(op1) + Z_LVAL_P(op2);

				/* check for overflow by comparing sign bits */
				if ((Z_LVAL_P(op1) & LONG_SIGN_MASK) == (Z_LVAL_P(op2) & LONG_SIGN_MASK)
					&& (Z_LVAL_P(op1) & LONG_SIGN_MASK) != (lval & LONG_SIGN_MASK)) {

					ZVAL_DOUBLE(result, (double) Z_LVAL_P(op1) + (double) Z_LVAL_P(op2));
				} else {
					ZVAL_LONG(result, lval);
				}
				return SUCCESS;
			}

			case TYPE_PAIR(IS_LONG, IS_DOUBLE):
				ZVAL_DOUBLE(result, ((double)Z_LVAL_P(op1)) + Z_DVAL_P(op2));
				return SUCCESS;

			case TYPE_PAIR(IS_DOUBLE, IS_LONG):
				ZVAL_DOUBLE(result, Z_DVAL_P(op1) + ((double)Z_LVAL_P(op2)));
				return SUCCESS;

			case TYPE_PAIR(IS_DOUBLE, IS_DOUBLE):
				ZVAL_DOUBLE(result, Z_DVAL_P(op1) + Z_DVAL_P(op2));
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
	zval op1_copy, op2_copy;
	int converted = 0;

	while (1) {
		switch (TYPE_PAIR(Z_TYPE_P(op1), Z_TYPE_P(op2))) {
			case TYPE_PAIR(IS_LONG, IS_LONG): {
				long lval = Z_LVAL_P(op1) - Z_LVAL_P(op2);

				/* check for overflow by comparing sign bits */
				if ((Z_LVAL_P(op1) & LONG_SIGN_MASK) != (Z_LVAL_P(op2) & LONG_SIGN_MASK)
					&& (Z_LVAL_P(op1) & LONG_SIGN_MASK) != (lval & LONG_SIGN_MASK)) {

					ZVAL_DOUBLE(result, (double) Z_LVAL_P(op1) - (double) Z_LVAL_P(op2));
				} else {
					ZVAL_LONG(result, lval);
				}
				return SUCCESS;

			}
			case TYPE_PAIR(IS_LONG, IS_DOUBLE):
				ZVAL_DOUBLE(result, ((double)Z_LVAL_P(op1)) - Z_DVAL_P(op2));
				return SUCCESS;

			case TYPE_PAIR(IS_DOUBLE, IS_LONG):
				ZVAL_DOUBLE(result, Z_DVAL_P(op1) - ((double)Z_LVAL_P(op2)));
				return SUCCESS;

			case TYPE_PAIR(IS_DOUBLE, IS_DOUBLE):
				ZVAL_DOUBLE(result, Z_DVAL_P(op1) - Z_DVAL_P(op2));
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
	zval op1_copy, op2_copy;
	int converted = 0;

	while (1) {
		switch (TYPE_PAIR(Z_TYPE_P(op1), Z_TYPE_P(op2))) {
			case TYPE_PAIR(IS_LONG, IS_LONG): {
				long overflow;

				ZEND_SIGNED_MULTIPLY_LONG(Z_LVAL_P(op1),Z_LVAL_P(op2), Z_LVAL_P(result),Z_DVAL_P(result),overflow);
				Z_TYPE_P(result) = overflow ? IS_DOUBLE : IS_LONG;
				return SUCCESS;

			}
			case TYPE_PAIR(IS_LONG, IS_DOUBLE):
				ZVAL_DOUBLE(result, ((double)Z_LVAL_P(op1)) * Z_DVAL_P(op2));
				return SUCCESS;

			case TYPE_PAIR(IS_DOUBLE, IS_LONG):
				ZVAL_DOUBLE(result, Z_DVAL_P(op1) * ((double)Z_LVAL_P(op2)));
				return SUCCESS;

			case TYPE_PAIR(IS_DOUBLE, IS_DOUBLE):
				ZVAL_DOUBLE(result, Z_DVAL_P(op1) * Z_DVAL_P(op2));
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
					ZVAL_DOUBLE(result, (double) LONG_MIN / -1);
					return SUCCESS;
				}
				if (Z_LVAL_P(op1) % Z_LVAL_P(op2) == 0) { /* integer */
					ZVAL_LONG(result, Z_LVAL_P(op1) / Z_LVAL_P(op2));
				} else {
					ZVAL_DOUBLE(result, ((double) Z_LVAL_P(op1)) / Z_LVAL_P(op2));
				}
				return SUCCESS;

			case TYPE_PAIR(IS_DOUBLE, IS_LONG):
				if (Z_LVAL_P(op2) == 0) {
					zend_error(E_WARNING, "Division by zero");
					ZVAL_BOOL(result, 0);
					return FAILURE;			/* division by zero */
				}
				ZVAL_DOUBLE(result, Z_DVAL_P(op1) / (double)Z_LVAL_P(op2));
				return SUCCESS;

			case TYPE_PAIR(IS_LONG, IS_DOUBLE):
				if (Z_DVAL_P(op2) == 0) {
					zend_error(E_WARNING, "Division by zero");
					ZVAL_BOOL(result, 0);
					return FAILURE;			/* division by zero */
				}
				ZVAL_DOUBLE(result, (double)Z_LVAL_P(op1) / Z_DVAL_P(op2));
				return SUCCESS;

			case TYPE_PAIR(IS_DOUBLE, IS_DOUBLE):
				if (Z_DVAL_P(op2) == 0) {
					zend_error(E_WARNING, "Division by zero");
					ZVAL_BOOL(result, 0);
					return FAILURE;			/* division by zero */
				}
				ZVAL_DOUBLE(result, Z_DVAL_P(op1) / Z_DVAL_P(op2));
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
	long op1_lval;

	zendi_convert_to_long(op1, op1_copy, result);
	op1_lval = Z_LVAL_P(op1);
	zendi_convert_to_long(op2, op2_copy, result);

	if (Z_LVAL_P(op2) == 0) {
		zend_error(E_WARNING, "Division by zero");
		ZVAL_BOOL(result, 0);
		return FAILURE;			/* modulus by zero */
	}

	if (Z_LVAL_P(op2) == -1) {
		/* Prevent overflow error/crash if op1==LONG_MIN */
		ZVAL_LONG(result, 0);
		return SUCCESS;
	}

	ZVAL_LONG(result, op1_lval % Z_LVAL_P(op2));
	return SUCCESS;
}
/* }}} */

ZEND_API int boolean_xor_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	zval op1_copy, op2_copy;
	long op1_lval;

	zendi_convert_to_boolean(op1, op1_copy, result);
	op1_lval = Z_LVAL_P(op1);
	zendi_convert_to_boolean(op2, op2_copy, result);
	ZVAL_BOOL(result, op1_lval ^ Z_LVAL_P(op2));
	return SUCCESS;
}
/* }}} */

ZEND_API int boolean_not_function(zval *result, zval *op1 TSRMLS_DC) /* {{{ */
{
	zval op1_copy;

	zendi_convert_to_boolean(op1, op1_copy, result);
	ZVAL_BOOL(result, !Z_LVAL_P(op1));
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
		ZVAL_LONG(result, ~zend_dval_to_lval(Z_DVAL_P(op1)));
		return SUCCESS;
	} else if (Z_TYPE_P(op1) == IS_STRING) {
		int i;

		Z_TYPE_P(result) = IS_STRING;
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
	long op1_lval;

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

		Z_TYPE_P(result) = IS_STRING;
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
	zendi_convert_to_long(op1, op1_copy, result);
	op1_lval = Z_LVAL_P(op1);
	zendi_convert_to_long(op2, op2_copy, result);

	ZVAL_LONG(result, op1_lval | Z_LVAL_P(op2));
	return SUCCESS;
}
/* }}} */

ZEND_API int bitwise_and_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	zval op1_copy, op2_copy;
	long op1_lval;

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

		Z_TYPE_P(result) = IS_STRING;
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


	zendi_convert_to_long(op1, op1_copy, result);
	op1_lval = Z_LVAL_P(op1);
	zendi_convert_to_long(op2, op2_copy, result);

	ZVAL_LONG(result, op1_lval & Z_LVAL_P(op2));
	return SUCCESS;
}
/* }}} */

ZEND_API int bitwise_xor_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	zval op1_copy, op2_copy;
	long op1_lval;

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

		Z_TYPE_P(result) = IS_STRING;
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

	zendi_convert_to_long(op1, op1_copy, result);
	op1_lval = Z_LVAL_P(op1);
	zendi_convert_to_long(op2, op2_copy, result);

	ZVAL_LONG(result, op1_lval ^ Z_LVAL_P(op2));
	return SUCCESS;
}
/* }}} */

ZEND_API int shift_left_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	zval op1_copy, op2_copy;
	long op1_lval;

	zendi_convert_to_long(op1, op1_copy, result);
	op1_lval = Z_LVAL_P(op1);
	zendi_convert_to_long(op2, op2_copy, result);
	ZVAL_LONG(result, op1_lval << Z_LVAL_P(op2));
	return SUCCESS;
}
/* }}} */

ZEND_API int shift_right_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	zval op1_copy, op2_copy;
	long op1_lval;

	zendi_convert_to_long(op1, op1_copy, result);
	op1_lval = Z_LVAL_P(op1);
	zendi_convert_to_long(op2, op2_copy, result);
	ZVAL_LONG(result, op1_lval >> Z_LVAL_P(op2));
	return SUCCESS;
}
/* }}} */

/* must support result==op1 */
ZEND_API int add_char_to_string(zval *result, const zval *op1, const zval *op2) /* {{{ */
{
	int length = Z_STRLEN_P(op1) + 1;
	char *buf;

	if (IS_INTERNED(Z_STRVAL_P(op1))) {
		buf = (char *) emalloc(length + 1);
		memcpy(buf, Z_STRVAL_P(op1), Z_STRLEN_P(op1));
	} else {
		buf = (char *) erealloc(Z_STRVAL_P(op1), length + 1);
	}
	buf[length - 1] = (char) Z_LVAL_P(op2);
	buf[length] = 0;
	ZVAL_STRINGL(result, buf, length, 0);
	return SUCCESS;
}
/* }}} */

/* must support result==op1 */
ZEND_API int add_string_to_string(zval *result, const zval *op1, const zval *op2) /* {{{ */
{
	int length = Z_STRLEN_P(op1) + Z_STRLEN_P(op2);
	char *buf;

	if (IS_INTERNED(Z_STRVAL_P(op1))) {
		buf = (char *) emalloc(length+1);
		memcpy(buf, Z_STRVAL_P(op1), Z_STRLEN_P(op1));
	} else {
		buf = (char *) erealloc(Z_STRVAL_P(op1), length+1);
	}
	memcpy(buf + Z_STRLEN_P(op1), Z_STRVAL_P(op2), Z_STRLEN_P(op2));
	buf[length] = 0;
	ZVAL_STRINGL(result, buf, length, 0);
	return SUCCESS;
}
/* }}} */

ZEND_API int concat_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	zval op1_copy, op2_copy;
	int use_copy1 = 0, use_copy2 = 0;

	if (Z_TYPE_P(op1) != IS_STRING) {
		zend_make_printable_zval(op1, &op1_copy, &use_copy1);
	}
	if (Z_TYPE_P(op2) != IS_STRING) {
		zend_make_printable_zval(op2, &op2_copy, &use_copy2);
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
	if (result==op1 && !IS_INTERNED(Z_STRVAL_P(op1))) {	/* special case, perform operations on result */
		uint res_len = Z_STRLEN_P(op1) + Z_STRLEN_P(op2);

		if (Z_STRLEN_P(result) < 0 || (int) (Z_STRLEN_P(op1) + Z_STRLEN_P(op2)) < 0) {
			efree(Z_STRVAL_P(result));
			ZVAL_EMPTY_STRING(result);
			zend_error(E_ERROR, "String size overflow");
		}

		Z_STRVAL_P(result) = erealloc(Z_STRVAL_P(result), res_len+1);

		memcpy(Z_STRVAL_P(result)+Z_STRLEN_P(result), Z_STRVAL_P(op2), Z_STRLEN_P(op2));
		Z_STRVAL_P(result)[res_len]=0;
		Z_STRLEN_P(result) = res_len;
	} else {
		int length = Z_STRLEN_P(op1) + Z_STRLEN_P(op2);
		char *buf = (char *) emalloc(length + 1);

		memcpy(buf, Z_STRVAL_P(op1), Z_STRLEN_P(op1));
		memcpy(buf + Z_STRLEN_P(op1), Z_STRVAL_P(op2), Z_STRLEN_P(op2));
		buf[length] = 0;
		ZVAL_STRINGL(result, buf, length, 0);
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

ZEND_API int string_compare_function_ex(zval *result, zval *op1, zval *op2, zend_bool case_insensitive TSRMLS_DC) /* {{{ */
{
	zval op1_copy, op2_copy;
	int use_copy1 = 0, use_copy2 = 0;

	if (Z_TYPE_P(op1) != IS_STRING) {
		zend_make_printable_zval(op1, &op1_copy, &use_copy1);
	}
	if (Z_TYPE_P(op2) != IS_STRING) {
		zend_make_printable_zval(op2, &op2_copy, &use_copy2);
	}

	if (use_copy1) {
		op1 = &op1_copy;
	}
	if (use_copy2) {
		op2 = &op2_copy;
	}

	if (case_insensitive) {
		ZVAL_LONG(result, zend_binary_zval_strcasecmp(op1, op2));
	} else {
		ZVAL_LONG(result, zend_binary_zval_strcmp(op1, op2));
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
	return string_compare_function_ex(result, op1, op2, 0 TSRMLS_CC);
}
/* }}} */

ZEND_API int string_case_compare_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	return string_compare_function_ex(result, op1, op2, 1 TSRMLS_CC);
}
/* }}} */

#if HAVE_STRCOLL
ZEND_API int string_locale_compare_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	zval op1_copy, op2_copy;
	int use_copy1 = 0, use_copy2 = 0;

	if (Z_TYPE_P(op1) != IS_STRING) {
		zend_make_printable_zval(op1, &op1_copy, &use_copy1);
	}
	if (Z_TYPE_P(op2) != IS_STRING) {
		zend_make_printable_zval(op2, &op2_copy, &use_copy2);
	}

	if (use_copy1) {
		op1 = &op1_copy;
	}
	if (use_copy2) {
		op2 = &op2_copy;
	}

	ZVAL_LONG(result, strcoll(Z_STRVAL_P(op1), Z_STRVAL_P(op2)));

	if (use_copy1) {
		zval_dtor(op1);
	}
	if (use_copy2) {
		zval_dtor(op2);
	}
	return SUCCESS;
}
/* }}} */
#endif

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
				if (Z_DVAL_P(op1) == Z_DVAL_P(op2)) {
					ZVAL_LONG(result, 0);
				} else {
					Z_DVAL_P(result) = Z_DVAL_P(op1) - Z_DVAL_P(op2);
					ZVAL_LONG(result, ZEND_NORMALIZE_BOOL(Z_DVAL_P(result)));
				}
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

			case TYPE_PAIR(IS_NULL, IS_STRING):
				ZVAL_LONG(result, zend_binary_strcmp("", 0, Z_STRVAL_P(op2), Z_STRLEN_P(op2)));
				return SUCCESS;

			case TYPE_PAIR(IS_STRING, IS_NULL):
				ZVAL_LONG(result, zend_binary_strcmp(Z_STRVAL_P(op1), Z_STRLEN_P(op1), "", 0));
				return SUCCESS;

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
					} else if (Z_TYPE_P(op2) != IS_OBJECT && Z_OBJ_HT_P(op1)->cast_object) {
						ALLOC_INIT_ZVAL(op_free);
						if (Z_OBJ_HT_P(op1)->cast_object(op1, op_free, Z_TYPE_P(op2) TSRMLS_CC) == FAILURE) {
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
					} else if (Z_TYPE_P(op1) != IS_OBJECT && Z_OBJ_HT_P(op2)->cast_object) {
						ALLOC_INIT_ZVAL(op_free);
						if (Z_OBJ_HT_P(op2)->cast_object(op2, op_free, Z_TYPE_P(op1) TSRMLS_CC) == FAILURE) {
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
			Z_LVAL_P(result) = ((Z_STRLEN_P(op1) == Z_STRLEN_P(op2))
				&& (!memcmp(Z_STRVAL_P(op1), Z_STRVAL_P(op2), Z_STRLEN_P(op1))));
			break;
		case IS_ARRAY:
			Z_LVAL_P(result) = zend_hash_compare(Z_ARRVAL_P(op1), Z_ARRVAL_P(op2), (compare_func_t) hash_zval_identical_function, 1 TSRMLS_CC)==0;
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

	if (IS_INTERNED(s)) {
		s = (char*) emalloc(Z_STRLEN_P(str) + 1);
		memcpy(s, Z_STRVAL_P(str), Z_STRLEN_P(str) + 1);
		Z_STRVAL_P(str) = s;
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
			ZVAL_LONG(op1, 1);
			break;
		case IS_STRING: {
				long lval;
				double dval;

				switch (is_numeric_string(Z_STRVAL_P(op1), Z_STRLEN_P(op1), &lval, &dval, 0)) {
					case IS_LONG:
						str_efree(Z_STRVAL_P(op1));
						if (lval == LONG_MAX) {
							/* switch to double */
							double d = (double)lval;
							ZVAL_DOUBLE(op1, d+1);
						} else {
							ZVAL_LONG(op1, lval+1);
						}
						break;
					case IS_DOUBLE:
						str_efree(Z_STRVAL_P(op1));
						ZVAL_DOUBLE(op1, dval+1);
						break;
					default:
						/* Perl style string increment */
						increment_string(op1);
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
				ZVAL_LONG(op1, -1);
				break;
			}
			switch (is_numeric_string(Z_STRVAL_P(op1), Z_STRLEN_P(op1), &lval, &dval, 0)) {
				case IS_LONG:
					STR_FREE(Z_STRVAL_P(op1));
					if (lval == LONG_MIN) {
						double d = (double)lval;
						ZVAL_DOUBLE(op1, d-1);
					} else {
						ZVAL_LONG(op1, lval-1);
					}
					break;
				case IS_DOUBLE:
					STR_FREE(Z_STRVAL_P(op1));
					ZVAL_DOUBLE(op1, dval - 1);
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

ZEND_API int zend_binary_strcmp(const char *s1, uint len1, const char *s2, uint len2) /* {{{ */
{
	int retval;

	if (s1 == s2) {
		return 0;
	}
	retval = memcmp(s1, s2, MIN(len1, len2));
	if (!retval) {
		return (len1 - len2);
	} else {
		return retval;
	}
}
/* }}} */

ZEND_API int zend_binary_strncmp(const char *s1, uint len1, const char *s2, uint len2, uint length) /* {{{ */
{
	int retval;

	if (s1 == s2) {
		return 0;
	}
	retval = memcmp(s1, s2, MIN(length, MIN(len1, len2)));
	if (!retval) {
		return (MIN(length, len1) - MIN(length, len2));
	} else {
		return retval;
	}
}
/* }}} */

ZEND_API int zend_binary_strcasecmp(const char *s1, uint len1, const char *s2, uint len2) /* {{{ */
{
	int len;
	int c1, c2;

	if (s1 == s2) {
		return 0;
	}

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

ZEND_API int zend_binary_strncasecmp(const char *s1, uint len1, const char *s2, uint len2, uint length) /* {{{ */
{
	int len;
	int c1, c2;

	if (s1 == s2) {
		return 0;
	}
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

ZEND_API int zend_binary_zval_strcmp(zval *s1, zval *s2) /* {{{ */
{
	return zend_binary_strcmp(Z_STRVAL_P(s1), Z_STRLEN_P(s1), Z_STRVAL_P(s2), Z_STRLEN_P(s2));
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
	int oflow1, oflow2;
	long lval1, lval2;
	double dval1, dval2;

	if ((ret1=is_numeric_string_ex(Z_STRVAL_P(s1), Z_STRLEN_P(s1), &lval1, &dval1, 0, &oflow1)) &&
		(ret2=is_numeric_string_ex(Z_STRVAL_P(s2), Z_STRLEN_P(s2), &lval2, &dval2, 0, &oflow2))) {
#if ULONG_MAX == 0xFFFFFFFF
		if (oflow1 != 0 && oflow1 == oflow2 && dval1 - dval2 == 0. &&
			((oflow1 == 1 && dval1 > 9007199254740991. /*0x1FFFFFFFFFFFFF*/)
			|| (oflow1 == -1 && dval1 < -9007199254740991.))) {
#else
		if (oflow1 != 0 && oflow1 == oflow2 && dval1 - dval2 == 0.) {
#endif
			/* both values are integers overflown to the same side, and the
			 * double comparison may have resulted in crucial accuracy lost */
			goto string_cmp;
		}
		if ((ret1==IS_DOUBLE) || (ret2==IS_DOUBLE)) {
			if (ret1!=IS_DOUBLE) {
				if (oflow2) {
					/* 2nd operand is integer > LONG_MAX (oflow2==1) or < LONG_MIN (-1) */
					ZVAL_LONG(result, -1 * oflow2);
					return;
				}
				dval1 = (double) lval1;
			} else if (ret2!=IS_DOUBLE) {
				if (oflow1) {
					ZVAL_LONG(result, oflow1);
					return; 
				}
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
		ZVAL_LONG(result, ZEND_NORMALIZE_BOOL(Z_LVAL_P(result)));
	}
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
	ZVAL_LONG(result, zend_hash_compare(ht1, ht2, (compare_func_t) hash_zval_compare_function, 0 TSRMLS_CC));
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
