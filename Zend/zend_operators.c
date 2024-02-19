/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

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

#ifdef __SSE2__
#include <emmintrin.h>
#endif

#ifdef ZEND_USE_TOLOWER_L
#include <locale.h>
static _locale_t current_locale = NULL;
/* this is true global! may lead to strange effects on ZTS, but so may setlocale() */
#define zend_tolower(c) _tolower_l(c, current_locale)
#else
#define zend_tolower(c) tolower(c)
#endif

#define TYPE_PAIR(t1,t2) (((t1) << 4) | (t2))

static const unsigned char tolower_map[256] = {
0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
0x40,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x5b,0x5c,0x5d,0x5e,0x5f,
0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,
0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,
0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,
0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,
0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,
0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,
0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff
};

#define zend_tolower_ascii(c) (tolower_map[(unsigned char)(c)])

/* ctype's isalnum is isalpha + isdigit(0-9) */
ZEND_API const bool zend_isalnum_map[256] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

/**
 * Functions using locale lowercase:
 	 	zend_binary_strncasecmp_l
 	 	zend_binary_strcasecmp_l
		zend_binary_zval_strcasecmp
		zend_binary_zval_strncasecmp
		string_compare_function_ex
		string_case_compare_function
 * Functions using ascii lowercase:
  		zend_str_tolower_copy
		zend_str_tolower_dup
		zend_str_tolower
		zend_binary_strcasecmp
		zend_binary_strncasecmp
 */

ZEND_API int ZEND_FASTCALL zend_atoi(const char *str, size_t str_len) /* {{{ */
{
	int retval;

	if (!str_len) {
		str_len = strlen(str);
	}
	retval = ZEND_STRTOL(str, NULL, 0);
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

ZEND_API zend_long ZEND_FASTCALL zend_atol(const char *str, size_t str_len) /* {{{ */
{
	zend_long retval;

	if (!str_len) {
		str_len = strlen(str);
	}
	retval = ZEND_STRTOL(str, NULL, 0);
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

/* {{{ convert_object_to_type: dst will be either ctype or UNDEF */
#define convert_object_to_type(op, dst, ctype)									\
	ZVAL_UNDEF(dst);																		\
	if (Z_OBJ_HT_P(op)->cast_object(Z_OBJ_P(op), dst, ctype) == FAILURE) {					\
		zend_error(E_WARNING,																\
			"Object of class %s could not be converted to %s", ZSTR_VAL(Z_OBJCE_P(op)->name),\
		zend_get_type_by_const(ctype));														\
	} 																						\

/* }}} */

ZEND_API void ZEND_FASTCALL convert_scalar_to_number(zval *op) /* {{{ */
{
try_again:
	switch (Z_TYPE_P(op)) {
		case IS_REFERENCE:
			zend_unwrap_reference(op);
			goto try_again;
		case IS_STRING:
			{
				zend_string *str;

				str = Z_STR_P(op);
				if ((Z_TYPE_INFO_P(op)=is_numeric_string(ZSTR_VAL(str), ZSTR_LEN(str), &Z_LVAL_P(op), &Z_DVAL_P(op), 1)) == 0) {
					ZVAL_LONG(op, 0);
				}
				zend_string_release_ex(str, 0);
				break;
			}
		case IS_NULL:
		case IS_FALSE:
			ZVAL_LONG(op, 0);
			break;
		case IS_TRUE:
			ZVAL_LONG(op, 1);
			break;
		case IS_RESOURCE:
			{
				zend_long l = Z_RES_HANDLE_P(op);
				zval_ptr_dtor(op);
				ZVAL_LONG(op, l);
			}
			break;
		case IS_OBJECT:
			{
				zval dst;

				convert_object_to_type(op, &dst, _IS_NUMBER);
				zval_ptr_dtor(op);

				if (Z_TYPE(dst) == IS_LONG || Z_TYPE(dst) == IS_DOUBLE) {
					ZVAL_COPY_VALUE(op, &dst);
				} else {
					ZVAL_LONG(op, 1);
				}
			}
			break;
	}
}
/* }}} */

static zend_never_inline zval* ZEND_FASTCALL _zendi_convert_scalar_to_number_silent(zval *op, zval *holder) /* {{{ */
{
	switch (Z_TYPE_P(op)) {
		case IS_NULL:
		case IS_FALSE:
			ZVAL_LONG(holder, 0);
			return holder;
		case IS_TRUE:
			ZVAL_LONG(holder, 1);
			return holder;
		case IS_STRING:
			if ((Z_TYPE_INFO_P(holder) = is_numeric_string(Z_STRVAL_P(op), Z_STRLEN_P(op), &Z_LVAL_P(holder), &Z_DVAL_P(holder), 1)) == 0) {
				ZVAL_LONG(holder, 0);
			}
			return holder;
		case IS_RESOURCE:
			ZVAL_LONG(holder, Z_RES_HANDLE_P(op));
			return holder;
		case IS_OBJECT:
			convert_object_to_type(op, holder, _IS_NUMBER);
			if (UNEXPECTED(EG(exception)) ||
			    UNEXPECTED(Z_TYPE_P(holder) != IS_LONG && Z_TYPE_P(holder) != IS_DOUBLE)) {
				ZVAL_LONG(holder, 1);
			}
			return holder;
		case IS_LONG:
		case IS_DOUBLE:
		default:
			return op;
	}
}
/* }}} */

static zend_never_inline zend_result ZEND_FASTCALL _zendi_try_convert_scalar_to_number(zval *op, zval *holder) /* {{{ */
{
	switch (Z_TYPE_P(op)) {
		case IS_NULL:
		case IS_FALSE:
			ZVAL_LONG(holder, 0);
			return SUCCESS;
		case IS_TRUE:
			ZVAL_LONG(holder, 1);
			return SUCCESS;
		case IS_STRING:
		{
			bool trailing_data = false;
			/* For BC reasons we allow errors so that we can warn on leading numeric string */
			if (0 == (Z_TYPE_INFO_P(holder) = is_numeric_string_ex(Z_STRVAL_P(op), Z_STRLEN_P(op),
					&Z_LVAL_P(holder), &Z_DVAL_P(holder),  /* allow errors */ true, NULL, &trailing_data))) {
				/* Will lead to invalid OP type error */
				return FAILURE;
			}
			if (UNEXPECTED(trailing_data)) {
				zend_error(E_WARNING, "A non-numeric value encountered");
				if (UNEXPECTED(EG(exception))) {
					return FAILURE;
				}
			}
			return SUCCESS;
		}
		case IS_OBJECT:
			if (Z_OBJ_HT_P(op)->cast_object(Z_OBJ_P(op), holder, _IS_NUMBER) == FAILURE
					|| EG(exception)) {
				return FAILURE;
			}
			ZEND_ASSERT(Z_TYPE_P(holder) == IS_LONG || Z_TYPE_P(holder) == IS_DOUBLE);
			return SUCCESS;
		case IS_RESOURCE:
		case IS_ARRAY:
			return FAILURE;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}
/* }}} */

static zend_always_inline zend_result zendi_try_convert_scalar_to_number(zval *op, zval *holder) /* {{{ */
{
	if (Z_TYPE_P(op) == IS_LONG || Z_TYPE_P(op) == IS_DOUBLE) {
		ZVAL_COPY_VALUE(holder, op);
		return SUCCESS;
	} else {
		return _zendi_try_convert_scalar_to_number(op, holder);
	}
}
/* }}} */

static zend_never_inline zend_long ZEND_FASTCALL zendi_try_get_long(zval *op, zend_bool *failed) /* {{{ */
{
	*failed = 0;
	switch (Z_TYPE_P(op)) {
		case IS_NULL:
		case IS_FALSE:
			return 0;
		case IS_TRUE:
			return 1;
		case IS_DOUBLE:
			return zend_dval_to_lval(Z_DVAL_P(op));
		case IS_STRING:
			{
				zend_uchar type;
				zend_long lval;
				double dval;
				bool trailing_data = false;

				/* For BC reasons we allow errors so that we can warn on leading numeric string */
				type = is_numeric_string_ex(Z_STRVAL_P(op), Z_STRLEN_P(op), &lval, &dval,
					/* allow errors */ true, NULL, &trailing_data);
				if (type == 0) {
					*failed = 1;
					return 0;
				}
				if (UNEXPECTED(trailing_data)) {
					zend_error(E_WARNING, "A non-numeric value encountered");
					if (UNEXPECTED(EG(exception))) {
						*failed = 1;
					}
				}
				if (EXPECTED(type == IS_LONG)) {
					return lval;
				} else {
					/* Previously we used strtol here, not is_numeric_string,
					 * and strtol gives you LONG_MAX/_MIN on overflow.
					 * We use use saturating conversion to emulate strtol()'s
					 * behaviour.
					 */
					 return zend_dval_to_lval_cap(dval);
				}
			}
		case IS_OBJECT:
			{
				zval dst;
				if (Z_OBJ_HT_P(op)->cast_object(Z_OBJ_P(op), &dst, IS_LONG) == FAILURE
						|| EG(exception)) {
					*failed = 1;
					return 0;
				}
				ZEND_ASSERT(Z_TYPE(dst) == IS_LONG);
				return Z_LVAL(dst);
			}
		case IS_RESOURCE:
		case IS_ARRAY:
			*failed = 1;
			return 0;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}
/* }}} */

#define ZEND_TRY_BINARY_OP1_OBJECT_OPERATION(opcode) \
	if (UNEXPECTED(Z_TYPE_P(op1) == IS_OBJECT) \
		&& UNEXPECTED(Z_OBJ_HANDLER_P(op1, do_operation))) { \
		if (EXPECTED(SUCCESS == Z_OBJ_HANDLER_P(op1, do_operation)(opcode, result, op1, op2))) { \
			return SUCCESS; \
		} \
	}

#define ZEND_TRY_BINARY_OP2_OBJECT_OPERATION(opcode) \
	if (UNEXPECTED(Z_TYPE_P(op2) == IS_OBJECT) \
		&& UNEXPECTED(Z_OBJ_HANDLER_P(op2, do_operation)) \
		&& EXPECTED(SUCCESS == Z_OBJ_HANDLER_P(op2, do_operation)(opcode, result, op1, op2))) { \
		return SUCCESS; \
	}

#define ZEND_TRY_BINARY_OBJECT_OPERATION(opcode) \
	ZEND_TRY_BINARY_OP1_OBJECT_OPERATION(opcode) \
	else \
	ZEND_TRY_BINARY_OP2_OBJECT_OPERATION(opcode)

#define ZEND_TRY_UNARY_OBJECT_OPERATION(opcode) \
	if (UNEXPECTED(Z_TYPE_P(op1) == IS_OBJECT) \
		&& UNEXPECTED(Z_OBJ_HANDLER_P(op1, do_operation)) \
		&& EXPECTED(SUCCESS == Z_OBJ_HANDLER_P(op1, do_operation)(opcode, result, op1, NULL))) { \
		return SUCCESS; \
	}

#define convert_op1_op2_long(op1, op1_lval, op2, op2_lval, result, opcode, sigil) \
	do {																\
		if (UNEXPECTED(Z_TYPE_P(op1) != IS_LONG)) {						\
			zend_bool failed;											\
			if (Z_ISREF_P(op1)) {										\
				op1 = Z_REFVAL_P(op1);									\
				if (Z_TYPE_P(op1) == IS_LONG) {							\
					op1_lval = Z_LVAL_P(op1);							\
					break;												\
				}														\
			}															\
			ZEND_TRY_BINARY_OP1_OBJECT_OPERATION(opcode);				\
			op1_lval = zendi_try_get_long(op1, &failed);				\
			if (UNEXPECTED(failed)) {									\
				zend_binop_error(sigil, op1, op2);						\
				if (result != op1) {									\
					ZVAL_UNDEF(result);									\
				}														\
				return FAILURE;											\
			}															\
		} else {														\
			op1_lval = Z_LVAL_P(op1);									\
		}																\
	} while (0);														\
	do {																\
		if (UNEXPECTED(Z_TYPE_P(op2) != IS_LONG)) {						\
			zend_bool failed;											\
			if (Z_ISREF_P(op2)) {										\
				op2 = Z_REFVAL_P(op2);									\
				if (Z_TYPE_P(op2) == IS_LONG) {							\
					op2_lval = Z_LVAL_P(op2);							\
					break;												\
				}														\
			}															\
			ZEND_TRY_BINARY_OP2_OBJECT_OPERATION(opcode);				\
			op2_lval = zendi_try_get_long(op2, &failed);				\
			if (UNEXPECTED(failed)) {									\
				zend_binop_error(sigil, op1, op2);						\
				if (result != op1) {									\
					ZVAL_UNDEF(result);									\
				}														\
				return FAILURE;											\
			}															\
		} else {														\
			op2_lval = Z_LVAL_P(op2);									\
		}																\
	} while (0);

ZEND_API void ZEND_FASTCALL convert_to_long(zval *op) /* {{{ */
{
	if (Z_TYPE_P(op) != IS_LONG) {
		convert_to_long_base(op, 10);
	}
}
/* }}} */

ZEND_API void ZEND_FASTCALL convert_to_long_base(zval *op, int base) /* {{{ */
{
	zend_long tmp;

try_again:
	switch (Z_TYPE_P(op)) {
		case IS_NULL:
		case IS_FALSE:
			ZVAL_LONG(op, 0);
			break;
		case IS_TRUE:
			ZVAL_LONG(op, 1);
			break;
		case IS_RESOURCE:
			tmp = Z_RES_HANDLE_P(op);
			zval_ptr_dtor(op);
			ZVAL_LONG(op, tmp);
			break;
		case IS_LONG:
			break;
		case IS_DOUBLE:
			ZVAL_LONG(op, zend_dval_to_lval(Z_DVAL_P(op)));
			break;
		case IS_STRING:
			{
				zend_string *str = Z_STR_P(op);
				if (base == 10) {
					ZVAL_LONG(op, zval_get_long(op));
				} else {
					ZVAL_LONG(op, ZEND_STRTOL(ZSTR_VAL(str), NULL, base));
				}
				zend_string_release_ex(str, 0);
			}
			break;
		case IS_ARRAY:
			tmp = (zend_hash_num_elements(Z_ARRVAL_P(op))?1:0);
			zval_ptr_dtor(op);
			ZVAL_LONG(op, tmp);
			break;
		case IS_OBJECT:
			{
				zval dst;

				convert_object_to_type(op, &dst, IS_LONG);
				zval_ptr_dtor(op);

				if (Z_TYPE(dst) == IS_LONG) {
					ZVAL_LONG(op, Z_LVAL(dst));
				} else {
					ZVAL_LONG(op, 1);
				}
				return;
			}
		case IS_REFERENCE:
			zend_unwrap_reference(op);
			goto try_again;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}
/* }}} */

ZEND_API void ZEND_FASTCALL convert_to_double(zval *op) /* {{{ */
{
	double tmp;

try_again:
	switch (Z_TYPE_P(op)) {
		case IS_NULL:
		case IS_FALSE:
			ZVAL_DOUBLE(op, 0.0);
			break;
		case IS_TRUE:
			ZVAL_DOUBLE(op, 1.0);
			break;
		case IS_RESOURCE: {
				double d = (double) Z_RES_HANDLE_P(op);
				zval_ptr_dtor(op);
				ZVAL_DOUBLE(op, d);
			}
			break;
		case IS_LONG:
			ZVAL_DOUBLE(op, (double) Z_LVAL_P(op));
			break;
		case IS_DOUBLE:
			break;
		case IS_STRING:
			{
				zend_string *str = Z_STR_P(op);

				ZVAL_DOUBLE(op, zend_strtod(ZSTR_VAL(str), NULL));
				zend_string_release_ex(str, 0);
			}
			break;
		case IS_ARRAY:
			tmp = (zend_hash_num_elements(Z_ARRVAL_P(op))?1:0);
			zval_ptr_dtor(op);
			ZVAL_DOUBLE(op, tmp);
			break;
		case IS_OBJECT:
			{
				zval dst;

				convert_object_to_type(op, &dst, IS_DOUBLE);
				zval_ptr_dtor(op);

				if (Z_TYPE(dst) == IS_DOUBLE) {
					ZVAL_DOUBLE(op, Z_DVAL(dst));
				} else {
					ZVAL_DOUBLE(op, 1.0);
				}
				break;
			}
		case IS_REFERENCE:
			zend_unwrap_reference(op);
			goto try_again;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}
/* }}} */

ZEND_API void ZEND_FASTCALL convert_to_null(zval *op) /* {{{ */
{
	zval_ptr_dtor(op);
	ZVAL_NULL(op);
}
/* }}} */

ZEND_API void ZEND_FASTCALL convert_to_boolean(zval *op) /* {{{ */
{
	bool tmp;

try_again:
	switch (Z_TYPE_P(op)) {
		case IS_FALSE:
		case IS_TRUE:
			break;
		case IS_NULL:
			ZVAL_FALSE(op);
			break;
		case IS_RESOURCE: {
				zend_long l = (Z_RES_HANDLE_P(op) ? 1 : 0);

				zval_ptr_dtor(op);
				ZVAL_BOOL(op, l);
			}
			break;
		case IS_LONG:
			ZVAL_BOOL(op, Z_LVAL_P(op) ? 1 : 0);
			break;
		case IS_DOUBLE:
			ZVAL_BOOL(op, Z_DVAL_P(op) ? 1 : 0);
			break;
		case IS_STRING:
			{
				zend_string *str = Z_STR_P(op);

				if (ZSTR_LEN(str) == 0
					|| (ZSTR_LEN(str) == 1 && ZSTR_VAL(str)[0] == '0')) {
					ZVAL_FALSE(op);
				} else {
					ZVAL_TRUE(op);
				}
				zend_string_release_ex(str, 0);
			}
			break;
		case IS_ARRAY:
			tmp = (zend_hash_num_elements(Z_ARRVAL_P(op))?1:0);
			zval_ptr_dtor(op);
			ZVAL_BOOL(op, tmp);
			break;
		case IS_OBJECT:
			{
				zval dst;

				convert_object_to_type(op, &dst, _IS_BOOL);
				zval_ptr_dtor(op);

				if (Z_TYPE_INFO(dst) == IS_FALSE || Z_TYPE_INFO(dst) == IS_TRUE) {
					Z_TYPE_INFO_P(op) = Z_TYPE_INFO(dst);
				} else {
					ZVAL_TRUE(op);
				}
				break;
			}
		case IS_REFERENCE:
			zend_unwrap_reference(op);
			goto try_again;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}
/* }}} */

ZEND_API void ZEND_FASTCALL _convert_to_string(zval *op) /* {{{ */
{
try_again:
	switch (Z_TYPE_P(op)) {
		case IS_UNDEF:
		case IS_NULL:
		case IS_FALSE: {
			ZVAL_EMPTY_STRING(op);
			break;
		}
		case IS_TRUE:
			ZVAL_CHAR(op, '1');
			break;
		case IS_STRING:
			break;
		case IS_RESOURCE: {
			zend_string *str = zend_strpprintf(0, "Resource id #" ZEND_LONG_FMT, (zend_long)Z_RES_HANDLE_P(op));
			zval_ptr_dtor(op);
			ZVAL_NEW_STR(op, str);
			break;
		}
		case IS_LONG: {
			ZVAL_STR(op, zend_long_to_str(Z_LVAL_P(op)));
			break;
		}
		case IS_DOUBLE: {
			zend_string *str;
			double dval = Z_DVAL_P(op);

			str = zend_strpprintf_unchecked(0, "%.*H", (int) EG(precision), dval);

			ZVAL_NEW_STR(op, str);
			break;
		}
		case IS_ARRAY:
			zend_error(E_WARNING, "Array to string conversion");
			zval_ptr_dtor(op);
			ZVAL_INTERNED_STR(op, ZSTR_KNOWN(ZEND_STR_ARRAY_CAPITALIZED));
			break;
		case IS_OBJECT: {
			zval tmp;
			if (Z_OBJ_HT_P(op)->cast_object(Z_OBJ_P(op), &tmp, IS_STRING) == SUCCESS) {
				zval_ptr_dtor(op);
				ZVAL_COPY_VALUE(op, &tmp);
				return;
			}
			if (!EG(exception)) {
				zend_throw_error(NULL, "Object of class %s could not be converted to string", ZSTR_VAL(Z_OBJCE_P(op)->name));
			}
			zval_ptr_dtor(op);
			ZVAL_EMPTY_STRING(op);
			break;
		}
		case IS_REFERENCE:
			zend_unwrap_reference(op);
			goto try_again;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}
/* }}} */

ZEND_API zend_bool ZEND_FASTCALL _try_convert_to_string(zval *op) /* {{{ */
{
	zend_string *str;

	ZEND_ASSERT(Z_TYPE_P(op) != IS_STRING);
	str = zval_try_get_string_func(op);
	if (UNEXPECTED(!str)) {
		return 0;
	}
	zval_ptr_dtor(op);
	ZVAL_STR(op, str);
	return 1;
}
/* }}} */

static void convert_scalar_to_array(zval *op) /* {{{ */
{
	HashTable *ht = zend_new_array(1);
	zend_hash_index_add_new(ht, 0, op);
	ZVAL_ARR(op, ht);
}
/* }}} */

ZEND_API void ZEND_FASTCALL convert_to_array(zval *op) /* {{{ */
{
try_again:
	switch (Z_TYPE_P(op)) {
		case IS_ARRAY:
			break;
/* OBJECTS_OPTIMIZE */
		case IS_OBJECT:
			if (Z_OBJCE_P(op) == zend_ce_closure) {
				convert_scalar_to_array(op);
			} else {
				HashTable *obj_ht = zend_get_properties_for(op, ZEND_PROP_PURPOSE_ARRAY_CAST);
				if (obj_ht) {
					HashTable *new_obj_ht = zend_proptable_to_symtable(obj_ht,
						(Z_OBJCE_P(op)->default_properties_count ||
						 Z_OBJ_P(op)->handlers != &std_object_handlers ||
						 GC_IS_RECURSIVE(obj_ht)));
					zval_ptr_dtor(op);
					ZVAL_ARR(op, new_obj_ht);
					zend_release_properties(obj_ht);
				} else {
					zval_ptr_dtor(op);
					/*ZVAL_EMPTY_ARRAY(op);*/
					array_init(op);
				}
			}
			break;
		case IS_NULL:
			/*ZVAL_EMPTY_ARRAY(op);*/
			array_init(op);
			break;
		case IS_REFERENCE:
			zend_unwrap_reference(op);
			goto try_again;
		default:
			convert_scalar_to_array(op);
			break;
	}
}
/* }}} */

ZEND_API void ZEND_FASTCALL convert_to_object(zval *op) /* {{{ */
{
try_again:
	switch (Z_TYPE_P(op)) {
		case IS_ARRAY:
			{
				HashTable *ht = zend_symtable_to_proptable(Z_ARR_P(op));
				zend_object *obj;

				if (GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE) {
					/* TODO: try not to duplicate immutable arrays as well ??? */
					ht = zend_array_dup(ht);
				} else if (ht != Z_ARR_P(op)) {
					zval_ptr_dtor(op);
				} else {
					GC_DELREF(ht);
				}
				obj = zend_objects_new(zend_standard_class_def);
				obj->properties = ht;
				ZVAL_OBJ(op, obj);
				break;
			}
		case IS_OBJECT:
			break;
		case IS_NULL:
			object_init(op);
			break;
		case IS_REFERENCE:
			zend_unwrap_reference(op);
			goto try_again;
		default: {
			zval tmp;
			ZVAL_COPY_VALUE(&tmp, op);
			object_init(op);
			zend_hash_add_new(Z_OBJPROP_P(op), ZSTR_KNOWN(ZEND_STR_SCALAR), &tmp);
			break;
		}
	}
}
/* }}} */

ZEND_API zend_long ZEND_FASTCALL zval_get_long_func(zval *op) /* {{{ */
{
try_again:
	switch (Z_TYPE_P(op)) {
		case IS_UNDEF:
		case IS_NULL:
		case IS_FALSE:
			return 0;
		case IS_TRUE:
			return 1;
		case IS_RESOURCE:
			return Z_RES_HANDLE_P(op);
		case IS_LONG:
			return Z_LVAL_P(op);
		case IS_DOUBLE:
			return zend_dval_to_lval(Z_DVAL_P(op));
		case IS_STRING:
			{
				zend_uchar type;
				zend_long lval;
				double dval;
				if (0 == (type = is_numeric_string(Z_STRVAL_P(op), Z_STRLEN_P(op), &lval, &dval, true))) {
					return 0;
				} else if (EXPECTED(type == IS_LONG)) {
					return lval;
				} else {
					/* Previously we used strtol here, not is_numeric_string,
					 * and strtol gives you LONG_MAX/_MIN on overflow.
					 * We use saturating conversion to emulate strtol()'s
					 * behaviour.
					 */
					 return zend_dval_to_lval_cap(dval);
				}
			}
		case IS_ARRAY:
			return zend_hash_num_elements(Z_ARRVAL_P(op)) ? 1 : 0;
		case IS_OBJECT:
			{
				zval dst;
				convert_object_to_type(op, &dst, IS_LONG);
				if (Z_TYPE(dst) == IS_LONG) {
					return Z_LVAL(dst);
				} else {
					return 1;
				}
			}
		case IS_REFERENCE:
			op = Z_REFVAL_P(op);
			goto try_again;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
	return 0;
}
/* }}} */

ZEND_API double ZEND_FASTCALL zval_get_double_func(zval *op) /* {{{ */
{
try_again:
	switch (Z_TYPE_P(op)) {
		case IS_NULL:
		case IS_FALSE:
			return 0.0;
		case IS_TRUE:
			return 1.0;
		case IS_RESOURCE:
			return (double) Z_RES_HANDLE_P(op);
		case IS_LONG:
			return (double) Z_LVAL_P(op);
		case IS_DOUBLE:
			return Z_DVAL_P(op);
		case IS_STRING:
			return zend_strtod(Z_STRVAL_P(op), NULL);
		case IS_ARRAY:
			return zend_hash_num_elements(Z_ARRVAL_P(op)) ? 1.0 : 0.0;
		case IS_OBJECT:
			{
				zval dst;
				convert_object_to_type(op, &dst, IS_DOUBLE);

				if (Z_TYPE(dst) == IS_DOUBLE) {
					return Z_DVAL(dst);
				} else {
					return 1.0;
				}
			}
		case IS_REFERENCE:
			op = Z_REFVAL_P(op);
			goto try_again;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
	return 0.0;
}
/* }}} */

static zend_always_inline zend_string* __zval_get_string_func(zval *op, zend_bool try) /* {{{ */
{
try_again:
	switch (Z_TYPE_P(op)) {
		case IS_UNDEF:
		case IS_NULL:
		case IS_FALSE:
			return ZSTR_EMPTY_ALLOC();
		case IS_TRUE:
			return ZSTR_CHAR('1');
		case IS_RESOURCE: {
			return zend_strpprintf(0, "Resource id #" ZEND_LONG_FMT, (zend_long)Z_RES_HANDLE_P(op));
		}
		case IS_LONG: {
			return zend_long_to_str(Z_LVAL_P(op));
		}
		case IS_DOUBLE: {
			return zend_strpprintf_unchecked(0, "%.*H", (int) EG(precision), Z_DVAL_P(op));
		}
		case IS_ARRAY:
			zend_error(E_WARNING, "Array to string conversion");
			return (try && UNEXPECTED(EG(exception))) ?
				NULL : ZSTR_KNOWN(ZEND_STR_ARRAY_CAPITALIZED);
		case IS_OBJECT: {
			zval tmp;
			if (Z_OBJ_HT_P(op)->cast_object(Z_OBJ_P(op), &tmp, IS_STRING) == SUCCESS) {
				return Z_STR(tmp);
			}
			if (!EG(exception)) {
				zend_throw_error(NULL, "Object of class %s could not be converted to string", ZSTR_VAL(Z_OBJCE_P(op)->name));
			}
			return try ? NULL : ZSTR_EMPTY_ALLOC();
		}
		case IS_REFERENCE:
			op = Z_REFVAL_P(op);
			goto try_again;
		case IS_STRING:
			return zend_string_copy(Z_STR_P(op));
		EMPTY_SWITCH_DEFAULT_CASE()
	}
	return NULL;
}
/* }}} */

ZEND_API zend_string* ZEND_FASTCALL zval_get_string_func(zval *op) /* {{{ */
{
	return __zval_get_string_func(op, 0);
}
/* }}} */

ZEND_API zend_string* ZEND_FASTCALL zval_try_get_string_func(zval *op) /* {{{ */
{
	return __zval_get_string_func(op, 1);
}
/* }}} */

static ZEND_COLD zend_never_inline void ZEND_FASTCALL zend_binop_error(const char *operator, zval *op1, zval *op2) /* {{{ */ {
	if (EG(exception)) {
		return;
	}

	zend_type_error("Unsupported operand types: %s %s %s",
		zend_zval_type_name(op1), operator, zend_zval_type_name(op2));
}
/* }}} */

static zend_never_inline void ZEND_FASTCALL add_function_array(zval *result, zval *op1, zval *op2) /* {{{ */
{
	if (result == op1 && Z_ARR_P(op1) == Z_ARR_P(op2)) {
		/* $a += $a */
		return;
	}
	if (result != op1) {
		ZVAL_ARR(result, zend_array_dup(Z_ARR_P(op1)));
	} else {
		SEPARATE_ARRAY(result);
	}
	zend_hash_merge(Z_ARRVAL_P(result), Z_ARRVAL_P(op2), zval_add_ref, 0);
}
/* }}} */

static zend_always_inline zend_result add_function_fast(zval *result, zval *op1, zval *op2) /* {{{ */
{
	zend_uchar type_pair = TYPE_PAIR(Z_TYPE_P(op1), Z_TYPE_P(op2));

	if (EXPECTED(type_pair == TYPE_PAIR(IS_LONG, IS_LONG))) {
		fast_long_add_function(result, op1, op2);
		return SUCCESS;
	} else if (EXPECTED(type_pair == TYPE_PAIR(IS_DOUBLE, IS_DOUBLE))) {
		ZVAL_DOUBLE(result, Z_DVAL_P(op1) + Z_DVAL_P(op2));
		return SUCCESS;
	} else if (EXPECTED(type_pair == TYPE_PAIR(IS_LONG, IS_DOUBLE))) {
		ZVAL_DOUBLE(result, ((double)Z_LVAL_P(op1)) + Z_DVAL_P(op2));
		return SUCCESS;
	} else if (EXPECTED(type_pair == TYPE_PAIR(IS_DOUBLE, IS_LONG))) {
		ZVAL_DOUBLE(result, Z_DVAL_P(op1) + ((double)Z_LVAL_P(op2)));
		return SUCCESS;
	} else if (EXPECTED(type_pair == TYPE_PAIR(IS_ARRAY, IS_ARRAY))) {
		add_function_array(result, op1, op2);
		return SUCCESS;
	} else {
		return FAILURE;
	}
} /* }}} */

static zend_never_inline zend_result ZEND_FASTCALL add_function_slow(zval *result, zval *op1, zval *op2) /* {{{ */
{
	ZVAL_DEREF(op1);
	ZVAL_DEREF(op2);
	if (add_function_fast(result, op1, op2) == SUCCESS) {
		return SUCCESS;
	}

	ZEND_TRY_BINARY_OBJECT_OPERATION(ZEND_ADD);

	zval op1_copy, op2_copy;
	if (UNEXPECTED(zendi_try_convert_scalar_to_number(op1, &op1_copy) == FAILURE)
			|| UNEXPECTED(zendi_try_convert_scalar_to_number(op2, &op2_copy) == FAILURE)) {
		zend_binop_error("+", op1, op2);
		if (result != op1) {
			ZVAL_UNDEF(result);
		}
		return FAILURE;
	}

	if (result == op1) {
		zval_ptr_dtor(result);
	}

	if (add_function_fast(result, &op1_copy, &op2_copy) == SUCCESS) {
		return SUCCESS;
	}

	ZEND_ASSERT(0 && "Operation must succeed");
	return FAILURE;
} /* }}} */

ZEND_API zend_result ZEND_FASTCALL add_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	if (add_function_fast(result, op1, op2) == SUCCESS) {
		return SUCCESS;
	} else {
		return add_function_slow(result, op1, op2);
	}
}
/* }}} */

static zend_always_inline zend_result sub_function_fast(zval *result, zval *op1, zval *op2) /* {{{ */
{
	zend_uchar type_pair = TYPE_PAIR(Z_TYPE_P(op1), Z_TYPE_P(op2));

	if (EXPECTED(type_pair == TYPE_PAIR(IS_LONG, IS_LONG))) {
		fast_long_sub_function(result, op1, op2);
		return SUCCESS;
	} else if (EXPECTED(type_pair == TYPE_PAIR(IS_DOUBLE, IS_DOUBLE))) {
		ZVAL_DOUBLE(result, Z_DVAL_P(op1) - Z_DVAL_P(op2));
		return SUCCESS;
	} else if (EXPECTED(type_pair == TYPE_PAIR(IS_LONG, IS_DOUBLE))) {
		ZVAL_DOUBLE(result, ((double)Z_LVAL_P(op1)) - Z_DVAL_P(op2));
		return SUCCESS;
	} else if (EXPECTED(type_pair == TYPE_PAIR(IS_DOUBLE, IS_LONG))) {
		ZVAL_DOUBLE(result, Z_DVAL_P(op1) - ((double)Z_LVAL_P(op2)));
		return SUCCESS;
	} else {
		return FAILURE;
	}
}
/* }}} */

static zend_never_inline zend_result ZEND_FASTCALL sub_function_slow(zval *result, zval *op1, zval *op2) /* {{{ */
{
	ZVAL_DEREF(op1);
	ZVAL_DEREF(op2);
	if (sub_function_fast(result, op1, op2) == SUCCESS) {
		return SUCCESS;
	}

	ZEND_TRY_BINARY_OBJECT_OPERATION(ZEND_SUB);

	zval op1_copy, op2_copy;
	if (UNEXPECTED(zendi_try_convert_scalar_to_number(op1, &op1_copy) == FAILURE)
			|| UNEXPECTED(zendi_try_convert_scalar_to_number(op2, &op2_copy) == FAILURE)) {
		zend_binop_error("-", op1, op2);
		if (result != op1) {
			ZVAL_UNDEF(result);
		}
		return FAILURE;
	}

	if (result == op1) {
		zval_ptr_dtor(result);
	}

	if (sub_function_fast(result, &op1_copy, &op2_copy) == SUCCESS) {
		return SUCCESS;
	}

	ZEND_ASSERT(0 && "Operation must succeed");
	return FAILURE;
}
/* }}} */

ZEND_API zend_result ZEND_FASTCALL sub_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	if (sub_function_fast(result, op1, op2) == SUCCESS) {
		return SUCCESS;
	} else {
		return sub_function_slow(result, op1, op2);
	}
}
/* }}} */

static zend_always_inline zend_result mul_function_fast(zval *result, zval *op1, zval *op2) /* {{{ */
{
	zend_uchar type_pair = TYPE_PAIR(Z_TYPE_P(op1), Z_TYPE_P(op2));

	if (EXPECTED(type_pair == TYPE_PAIR(IS_LONG, IS_LONG))) {
		zend_long overflow;
		ZEND_SIGNED_MULTIPLY_LONG(
			Z_LVAL_P(op1), Z_LVAL_P(op2),
			Z_LVAL_P(result), Z_DVAL_P(result), overflow);
		Z_TYPE_INFO_P(result) = overflow ? IS_DOUBLE : IS_LONG;
		return SUCCESS;
	} else if (EXPECTED(type_pair == TYPE_PAIR(IS_DOUBLE, IS_DOUBLE))) {
		ZVAL_DOUBLE(result, Z_DVAL_P(op1) * Z_DVAL_P(op2));
		return SUCCESS;
	} else if (EXPECTED(type_pair == TYPE_PAIR(IS_LONG, IS_DOUBLE))) {
		ZVAL_DOUBLE(result, ((double)Z_LVAL_P(op1)) * Z_DVAL_P(op2));
		return SUCCESS;
	} else if (EXPECTED(type_pair == TYPE_PAIR(IS_DOUBLE, IS_LONG))) {
		ZVAL_DOUBLE(result, Z_DVAL_P(op1) * ((double)Z_LVAL_P(op2)));
		return SUCCESS;
	} else {
		return FAILURE;
	}
}
/* }}} */

static zend_never_inline zend_result ZEND_FASTCALL mul_function_slow(zval *result, zval *op1, zval *op2) /* {{{ */
{
	ZVAL_DEREF(op1);
	ZVAL_DEREF(op2);
	if (mul_function_fast(result, op1, op2) == SUCCESS) {
		return SUCCESS;
	}

	ZEND_TRY_BINARY_OBJECT_OPERATION(ZEND_MUL);

	zval op1_copy, op2_copy;
	if (UNEXPECTED(zendi_try_convert_scalar_to_number(op1, &op1_copy) == FAILURE)
			|| UNEXPECTED(zendi_try_convert_scalar_to_number(op2, &op2_copy) == FAILURE)) {
		zend_binop_error("*", op1, op2);
		if (result != op1) {
			ZVAL_UNDEF(result);
		}
		return FAILURE;
	}

	if (result == op1) {
		zval_ptr_dtor(result);
	}

	if (mul_function_fast(result, &op1_copy, &op2_copy) == SUCCESS) {
		return SUCCESS;
	}

	ZEND_ASSERT(0 && "Operation must succeed");
	return FAILURE;
}
/* }}} */

ZEND_API zend_result ZEND_FASTCALL mul_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	if (mul_function_fast(result, op1, op2) == SUCCESS) {
		return SUCCESS;
	} else {
		return mul_function_slow(result, op1, op2);
	}
}
/* }}} */

static zend_result ZEND_FASTCALL pow_function_base(zval *result, zval *op1, zval *op2) /* {{{ */
{
	zend_uchar type_pair = TYPE_PAIR(Z_TYPE_P(op1), Z_TYPE_P(op2));

	if (EXPECTED(type_pair == TYPE_PAIR(IS_LONG, IS_LONG))) {
		if (Z_LVAL_P(op2) >= 0) {
			zend_long l1 = 1, l2 = Z_LVAL_P(op1), i = Z_LVAL_P(op2);

			if (i == 0) {
				ZVAL_LONG(result, 1L);
				return SUCCESS;
			} else if (l2 == 0) {
				ZVAL_LONG(result, 0);
				return SUCCESS;
			}

			while (i >= 1) {
				zend_long overflow;
				double dval = 0.0;

				if (i % 2) {
					--i;
					ZEND_SIGNED_MULTIPLY_LONG(l1, l2, l1, dval, overflow);
					if (overflow) {
						ZVAL_DOUBLE(result, dval * pow(l2, i));
						return SUCCESS;
					}
				} else {
					i /= 2;
					ZEND_SIGNED_MULTIPLY_LONG(l2, l2, l2, dval, overflow);
					if (overflow) {
						ZVAL_DOUBLE(result, (double)l1 * pow(dval, i));
						return SUCCESS;
					}
				}
			}
			/* i == 0 */
			ZVAL_LONG(result, l1);
		} else {
			ZVAL_DOUBLE(result, pow((double)Z_LVAL_P(op1), (double)Z_LVAL_P(op2)));
		}
		return SUCCESS;
	} else if (EXPECTED(type_pair == TYPE_PAIR(IS_DOUBLE, IS_DOUBLE))) {
		ZVAL_DOUBLE(result, pow(Z_DVAL_P(op1), Z_DVAL_P(op2)));
		return SUCCESS;
	} else if (EXPECTED(type_pair == TYPE_PAIR(IS_LONG, IS_DOUBLE))) {
		ZVAL_DOUBLE(result, pow((double)Z_LVAL_P(op1), Z_DVAL_P(op2)));
		return SUCCESS;
	} else if (EXPECTED(type_pair == TYPE_PAIR(IS_DOUBLE, IS_LONG))) {
		ZVAL_DOUBLE(result, pow(Z_DVAL_P(op1), (double)Z_LVAL_P(op2)));
		return SUCCESS;
	} else {
		return FAILURE;
	}
}
/* }}} */

ZEND_API zend_result ZEND_FASTCALL pow_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	ZVAL_DEREF(op1);
	ZVAL_DEREF(op2);
	if (pow_function_base(result, op1, op2) == SUCCESS) {
		return SUCCESS;
	}

	ZEND_TRY_BINARY_OBJECT_OPERATION(ZEND_POW);

	zval op1_copy, op2_copy;
	if (UNEXPECTED(zendi_try_convert_scalar_to_number(op1, &op1_copy) == FAILURE)
			|| UNEXPECTED(zendi_try_convert_scalar_to_number(op2, &op2_copy) == FAILURE)) {
		zend_binop_error("**", op1, op2);
		if (result != op1) {
			ZVAL_UNDEF(result);
		}
		return FAILURE;
	}

	if (result == op1) {
		zval_ptr_dtor(result);
	}

	if (pow_function_base(result, &op1_copy, &op2_copy) == SUCCESS) {
		return SUCCESS;
	}

	ZEND_ASSERT(0 && "Operation must succeed");
	return FAILURE;
}
/* }}} */

/* Returns SUCCESS/TYPES_NOT_HANDLED/DIV_BY_ZERO */
#define TYPES_NOT_HANDLED 1
#define DIV_BY_ZERO 2
static int ZEND_FASTCALL div_function_base(zval *result, zval *op1, zval *op2) /* {{{ */
{
	zend_uchar type_pair = TYPE_PAIR(Z_TYPE_P(op1), Z_TYPE_P(op2));

	if (EXPECTED(type_pair == TYPE_PAIR(IS_LONG, IS_LONG))) {
		if (Z_LVAL_P(op2) == 0) {
			return DIV_BY_ZERO;
		} else if (Z_LVAL_P(op2) == -1 && Z_LVAL_P(op1) == ZEND_LONG_MIN) {
			/* Prevent overflow error/crash */
			ZVAL_DOUBLE(result, (double) ZEND_LONG_MIN / -1);
			return SUCCESS;
		}
		if (Z_LVAL_P(op1) % Z_LVAL_P(op2) == 0) { /* integer */
			ZVAL_LONG(result, Z_LVAL_P(op1) / Z_LVAL_P(op2));
		} else {
			ZVAL_DOUBLE(result, ((double) Z_LVAL_P(op1)) / Z_LVAL_P(op2));
		}
		return SUCCESS;
	} else if (EXPECTED(type_pair == TYPE_PAIR(IS_DOUBLE, IS_DOUBLE))) {
		if (Z_DVAL_P(op2) == 0) {
			return DIV_BY_ZERO;
		}
		ZVAL_DOUBLE(result, Z_DVAL_P(op1) / Z_DVAL_P(op2));
		return SUCCESS;
	} else if (EXPECTED(type_pair == TYPE_PAIR(IS_DOUBLE, IS_LONG))) {
		if (Z_LVAL_P(op2) == 0) {
			return DIV_BY_ZERO;
		}
		ZVAL_DOUBLE(result, Z_DVAL_P(op1) / (double)Z_LVAL_P(op2));
		return SUCCESS;
	} else if (EXPECTED(type_pair == TYPE_PAIR(IS_LONG, IS_DOUBLE))) {
		if (Z_DVAL_P(op2) == 0) {
			return DIV_BY_ZERO;
		}
		ZVAL_DOUBLE(result, (double)Z_LVAL_P(op1) / Z_DVAL_P(op2));
		return SUCCESS;
	} else {
		return TYPES_NOT_HANDLED;
	}
}
/* }}} */

ZEND_API zend_result ZEND_FASTCALL div_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	ZVAL_DEREF(op1);
	ZVAL_DEREF(op2);

	int retval = div_function_base(result, op1, op2);
	if (EXPECTED(retval == SUCCESS)) {
		return SUCCESS;
	}

	if (UNEXPECTED(retval == DIV_BY_ZERO)) {
		goto div_by_zero;
	}

	ZEND_TRY_BINARY_OBJECT_OPERATION(ZEND_DIV);

	zval result_copy, op1_copy, op2_copy;
	if (UNEXPECTED(zendi_try_convert_scalar_to_number(op1, &op1_copy) == FAILURE)
			|| UNEXPECTED(zendi_try_convert_scalar_to_number(op2, &op2_copy) == FAILURE)) {
		zend_binop_error("/", op1, op2);
		if (result != op1) {
			ZVAL_UNDEF(result);
		}
		return FAILURE;
	}

	retval = div_function_base(&result_copy, &op1_copy, &op2_copy);
	if (retval == SUCCESS) {
		if (result == op1) {
			zval_ptr_dtor(result);
		}
		ZVAL_COPY_VALUE(result, &result_copy);
		return SUCCESS;
	}

div_by_zero:
	ZEND_ASSERT(retval == DIV_BY_ZERO && "TYPES_NOT_HANDLED should not occur here");
	if (result != op1) {
		ZVAL_UNDEF(result);
	}
	zend_throw_error(zend_ce_division_by_zero_error, "Division by zero");
	return FAILURE;
}
/* }}} */

ZEND_API zend_result ZEND_FASTCALL mod_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	zend_long op1_lval, op2_lval;

	convert_op1_op2_long(op1, op1_lval, op2, op2_lval, result, ZEND_MOD, "%");

	if (op2_lval == 0) {
		/* modulus by zero */
		if (EG(current_execute_data) && !CG(in_compilation)) {
			zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Modulo by zero");
		} else {
			zend_error_noreturn(E_ERROR, "Modulo by zero");
		}
		if (op1 != result) {
			ZVAL_UNDEF(result);
		}
		return FAILURE;
	}

	if (op1 == result) {
		zval_ptr_dtor(result);
	}

	if (op2_lval == -1) {
		/* Prevent overflow error/crash if op1==LONG_MIN */
		ZVAL_LONG(result, 0);
		return SUCCESS;
	}

	ZVAL_LONG(result, op1_lval % op2_lval);
	return SUCCESS;
}
/* }}} */

ZEND_API zend_result ZEND_FASTCALL boolean_xor_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	int op1_val, op2_val;

	do {
		if (Z_TYPE_P(op1) == IS_FALSE) {
			op1_val = 0;
		} else if (EXPECTED(Z_TYPE_P(op1) == IS_TRUE)) {
			op1_val = 1;
		} else {
			if (Z_ISREF_P(op1)) {
				op1 = Z_REFVAL_P(op1);
				if (Z_TYPE_P(op1) == IS_FALSE) {
					op1_val = 0;
					break;
				} else if (EXPECTED(Z_TYPE_P(op1) == IS_TRUE)) {
					op1_val = 1;
					break;
				}
			}
			ZEND_TRY_BINARY_OP1_OBJECT_OPERATION(ZEND_BOOL_XOR);
			op1_val = zval_is_true(op1);
		}
	} while (0);
	do {
		if (Z_TYPE_P(op2) == IS_FALSE) {
			op2_val = 0;
		} else if (EXPECTED(Z_TYPE_P(op2) == IS_TRUE)) {
			op2_val = 1;
		} else {
			if (Z_ISREF_P(op2)) {
				op2 = Z_REFVAL_P(op2);
				if (Z_TYPE_P(op2) == IS_FALSE) {
					op2_val = 0;
					break;
				} else if (EXPECTED(Z_TYPE_P(op2) == IS_TRUE)) {
					op2_val = 1;
					break;
				}
			}
			ZEND_TRY_BINARY_OP2_OBJECT_OPERATION(ZEND_BOOL_XOR);
			op2_val = zval_is_true(op2);
		}
	} while (0);

	ZVAL_BOOL(result, op1_val ^ op2_val);
	return SUCCESS;
}
/* }}} */

ZEND_API zend_result ZEND_FASTCALL boolean_not_function(zval *result, zval *op1) /* {{{ */
{
	if (Z_TYPE_P(op1) < IS_TRUE) {
		ZVAL_TRUE(result);
	} else if (EXPECTED(Z_TYPE_P(op1) == IS_TRUE)) {
		ZVAL_FALSE(result);
	} else {
		if (Z_ISREF_P(op1)) {
			op1 = Z_REFVAL_P(op1);
			if (Z_TYPE_P(op1) < IS_TRUE) {
				ZVAL_TRUE(result);
				return SUCCESS;
			} else if (EXPECTED(Z_TYPE_P(op1) == IS_TRUE)) {
				ZVAL_FALSE(result);
				return SUCCESS;
			}
		}
		ZEND_TRY_UNARY_OBJECT_OPERATION(ZEND_BOOL_NOT);

		ZVAL_BOOL(result, !zval_is_true(op1));
	}
	return SUCCESS;
}
/* }}} */

ZEND_API zend_result ZEND_FASTCALL bitwise_not_function(zval *result, zval *op1) /* {{{ */
{
try_again:
	switch (Z_TYPE_P(op1)) {
		case IS_LONG:
			ZVAL_LONG(result, ~Z_LVAL_P(op1));
			return SUCCESS;
		case IS_DOUBLE:
			ZVAL_LONG(result, ~zend_dval_to_lval(Z_DVAL_P(op1)));
			return SUCCESS;
		case IS_STRING: {
			size_t i;

			if (Z_STRLEN_P(op1) == 1) {
				zend_uchar not = (zend_uchar) ~*Z_STRVAL_P(op1);
				ZVAL_CHAR(result, not);
			} else {
				ZVAL_NEW_STR(result, zend_string_alloc(Z_STRLEN_P(op1), 0));
				for (i = 0; i < Z_STRLEN_P(op1); i++) {
					Z_STRVAL_P(result)[i] = ~Z_STRVAL_P(op1)[i];
				}
				Z_STRVAL_P(result)[i] = 0;
			}
			return SUCCESS;
		}
		case IS_REFERENCE:
			op1 = Z_REFVAL_P(op1);
			goto try_again;
		default:
			ZEND_TRY_UNARY_OBJECT_OPERATION(ZEND_BW_NOT);

			if (result != op1) {
				ZVAL_UNDEF(result);
			}
			zend_type_error("Cannot perform bitwise not on %s", zend_zval_type_name(op1));
			return FAILURE;
	}
}
/* }}} */

ZEND_API zend_result ZEND_FASTCALL bitwise_or_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	zend_long op1_lval, op2_lval;

	if (EXPECTED(Z_TYPE_P(op1) == IS_LONG) && EXPECTED(Z_TYPE_P(op2) == IS_LONG)) {
		ZVAL_LONG(result, Z_LVAL_P(op1) | Z_LVAL_P(op2));
		return SUCCESS;
	}

	ZVAL_DEREF(op1);
	ZVAL_DEREF(op2);

	if (Z_TYPE_P(op1) == IS_STRING && EXPECTED(Z_TYPE_P(op2) == IS_STRING)) {
		zval *longer, *shorter;
		zend_string *str;
		size_t i;

		if (EXPECTED(Z_STRLEN_P(op1) >= Z_STRLEN_P(op2))) {
			if (EXPECTED(Z_STRLEN_P(op1) == Z_STRLEN_P(op2)) && Z_STRLEN_P(op1) == 1) {
				zend_uchar or = (zend_uchar) (*Z_STRVAL_P(op1) | *Z_STRVAL_P(op2));
				if (result==op1) {
					zval_ptr_dtor_str(result);
				}
				ZVAL_CHAR(result, or);
				return SUCCESS;
			}
			longer = op1;
			shorter = op2;
		} else {
			longer = op2;
			shorter = op1;
		}

		str = zend_string_alloc(Z_STRLEN_P(longer), 0);
		for (i = 0; i < Z_STRLEN_P(shorter); i++) {
			ZSTR_VAL(str)[i] = Z_STRVAL_P(longer)[i] | Z_STRVAL_P(shorter)[i];
		}
		memcpy(ZSTR_VAL(str) + i, Z_STRVAL_P(longer) + i, Z_STRLEN_P(longer) - i + 1);
		if (result==op1) {
			zval_ptr_dtor_str(result);
		}
		ZVAL_NEW_STR(result, str);
		return SUCCESS;
	}

	if (UNEXPECTED(Z_TYPE_P(op1) != IS_LONG)) {
		zend_bool failed;
		ZEND_TRY_BINARY_OP1_OBJECT_OPERATION(ZEND_BW_OR);
		op1_lval = zendi_try_get_long(op1, &failed);
		if (UNEXPECTED(failed)) {
			zend_binop_error("|", op1, op2);
			if (result != op1) {
				ZVAL_UNDEF(result);
			}
			return FAILURE;
		}
	} else {
		op1_lval = Z_LVAL_P(op1);
	}
	if (UNEXPECTED(Z_TYPE_P(op2) != IS_LONG)) {
		zend_bool failed;
		ZEND_TRY_BINARY_OP2_OBJECT_OPERATION(ZEND_BW_OR);
		op2_lval = zendi_try_get_long(op2, &failed);
		if (UNEXPECTED(failed)) {
			zend_binop_error("|", op1, op2);
			if (result != op1) {
				ZVAL_UNDEF(result);
			}
			return FAILURE;
		}
	} else {
		op2_lval = Z_LVAL_P(op2);
	}

	if (op1 == result) {
		zval_ptr_dtor(result);
	}
	ZVAL_LONG(result, op1_lval | op2_lval);
	return SUCCESS;
}
/* }}} */

ZEND_API zend_result ZEND_FASTCALL bitwise_and_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	zend_long op1_lval, op2_lval;

	if (EXPECTED(Z_TYPE_P(op1) == IS_LONG) && EXPECTED(Z_TYPE_P(op2) == IS_LONG)) {
		ZVAL_LONG(result, Z_LVAL_P(op1) & Z_LVAL_P(op2));
		return SUCCESS;
	}

	ZVAL_DEREF(op1);
	ZVAL_DEREF(op2);

	if (Z_TYPE_P(op1) == IS_STRING && Z_TYPE_P(op2) == IS_STRING) {
		zval *longer, *shorter;
		zend_string *str;
		size_t i;

		if (EXPECTED(Z_STRLEN_P(op1) >= Z_STRLEN_P(op2))) {
			if (EXPECTED(Z_STRLEN_P(op1) == Z_STRLEN_P(op2)) && Z_STRLEN_P(op1) == 1) {
				zend_uchar and = (zend_uchar) (*Z_STRVAL_P(op1) & *Z_STRVAL_P(op2));
				if (result==op1) {
					zval_ptr_dtor_str(result);
				}
				ZVAL_CHAR(result, and);
				return SUCCESS;
			}
			longer = op1;
			shorter = op2;
		} else {
			longer = op2;
			shorter = op1;
		}

		str = zend_string_alloc(Z_STRLEN_P(shorter), 0);
		for (i = 0; i < Z_STRLEN_P(shorter); i++) {
			ZSTR_VAL(str)[i] = Z_STRVAL_P(shorter)[i] & Z_STRVAL_P(longer)[i];
		}
		ZSTR_VAL(str)[i] = 0;
		if (result==op1) {
			zval_ptr_dtor_str(result);
		}
		ZVAL_NEW_STR(result, str);
		return SUCCESS;
	}

	if (UNEXPECTED(Z_TYPE_P(op1) != IS_LONG)) {
		zend_bool failed;
		ZEND_TRY_BINARY_OP1_OBJECT_OPERATION(ZEND_BW_AND);
		op1_lval = zendi_try_get_long(op1, &failed);
		if (UNEXPECTED(failed)) {
			zend_binop_error("&", op1, op2);
			if (result != op1) {
				ZVAL_UNDEF(result);
			}
			return FAILURE;
		}
	} else {
		op1_lval = Z_LVAL_P(op1);
	}
	if (UNEXPECTED(Z_TYPE_P(op2) != IS_LONG)) {
		zend_bool failed;
		ZEND_TRY_BINARY_OP2_OBJECT_OPERATION(ZEND_BW_AND);
		op2_lval = zendi_try_get_long(op2, &failed);
		if (UNEXPECTED(failed)) {
			zend_binop_error("&", op1, op2);
			if (result != op1) {
				ZVAL_UNDEF(result);
			}
			return FAILURE;
		}
	} else {
		op2_lval = Z_LVAL_P(op2);
	}

	if (op1 == result) {
		zval_ptr_dtor(result);
	}
	ZVAL_LONG(result, op1_lval & op2_lval);
	return SUCCESS;
}
/* }}} */

ZEND_API zend_result ZEND_FASTCALL bitwise_xor_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	zend_long op1_lval, op2_lval;

	if (EXPECTED(Z_TYPE_P(op1) == IS_LONG) && EXPECTED(Z_TYPE_P(op2) == IS_LONG)) {
		ZVAL_LONG(result, Z_LVAL_P(op1) ^ Z_LVAL_P(op2));
		return SUCCESS;
	}

	ZVAL_DEREF(op1);
	ZVAL_DEREF(op2);

	if (Z_TYPE_P(op1) == IS_STRING && Z_TYPE_P(op2) == IS_STRING) {
		zval *longer, *shorter;
		zend_string *str;
		size_t i;

		if (EXPECTED(Z_STRLEN_P(op1) >= Z_STRLEN_P(op2))) {
			if (EXPECTED(Z_STRLEN_P(op1) == Z_STRLEN_P(op2)) && Z_STRLEN_P(op1) == 1) {
				zend_uchar xor = (zend_uchar) (*Z_STRVAL_P(op1) ^ *Z_STRVAL_P(op2));
				if (result==op1) {
					zval_ptr_dtor_str(result);
				}
				ZVAL_CHAR(result, xor);
				return SUCCESS;
			}
			longer = op1;
			shorter = op2;
		} else {
			longer = op2;
			shorter = op1;
		}

		str = zend_string_alloc(Z_STRLEN_P(shorter), 0);
		for (i = 0; i < Z_STRLEN_P(shorter); i++) {
			ZSTR_VAL(str)[i] = Z_STRVAL_P(shorter)[i] ^ Z_STRVAL_P(longer)[i];
		}
		ZSTR_VAL(str)[i] = 0;
		if (result==op1) {
			zval_ptr_dtor_str(result);
		}
		ZVAL_NEW_STR(result, str);
		return SUCCESS;
	}

	if (UNEXPECTED(Z_TYPE_P(op1) != IS_LONG)) {
		zend_bool failed;
		ZEND_TRY_BINARY_OP1_OBJECT_OPERATION(ZEND_BW_XOR);
		op1_lval = zendi_try_get_long(op1, &failed);
		if (UNEXPECTED(failed)) {
			zend_binop_error("^", op1, op2);
			if (result != op1) {
				ZVAL_UNDEF(result);
			}
			return FAILURE;
		}
	} else {
		op1_lval = Z_LVAL_P(op1);
	}
	if (UNEXPECTED(Z_TYPE_P(op2) != IS_LONG)) {
		zend_bool failed;
		ZEND_TRY_BINARY_OP2_OBJECT_OPERATION(ZEND_BW_XOR);
		op2_lval = zendi_try_get_long(op2, &failed);
		if (UNEXPECTED(failed)) {
			zend_binop_error("^", op1, op2);
			if (result != op1) {
				ZVAL_UNDEF(result);
			}
			return FAILURE;
		}
	} else {
		op2_lval = Z_LVAL_P(op2);
	}

	if (op1 == result) {
		zval_ptr_dtor(result);
	}
	ZVAL_LONG(result, op1_lval ^ op2_lval);
	return SUCCESS;
}
/* }}} */

ZEND_API zend_result ZEND_FASTCALL shift_left_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	zend_long op1_lval, op2_lval;

	convert_op1_op2_long(op1, op1_lval, op2, op2_lval, result, ZEND_SL, "<<");

	/* prevent wrapping quirkiness on some processors where << 64 + x == << x */
	if (UNEXPECTED((zend_ulong)op2_lval >= SIZEOF_ZEND_LONG * 8)) {
		if (EXPECTED(op2_lval > 0)) {
			if (op1 == result) {
				zval_ptr_dtor(result);
			}
			ZVAL_LONG(result, 0);
			return SUCCESS;
		} else {
			if (EG(current_execute_data) && !CG(in_compilation)) {
				zend_throw_exception_ex(zend_ce_arithmetic_error, 0, "Bit shift by negative number");
			} else {
				zend_error_noreturn(E_ERROR, "Bit shift by negative number");
			}
			if (op1 != result) {
				ZVAL_UNDEF(result);
			}
			return FAILURE;
		}
	}

	if (op1 == result) {
		zval_ptr_dtor(result);
	}

	/* Perform shift on unsigned numbers to get well-defined wrap behavior. */
	ZVAL_LONG(result, (zend_long) ((zend_ulong) op1_lval << op2_lval));
	return SUCCESS;
}
/* }}} */

ZEND_API zend_result ZEND_FASTCALL shift_right_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	zend_long op1_lval, op2_lval;

	convert_op1_op2_long(op1, op1_lval, op2, op2_lval, result, ZEND_SR, ">>");

	/* prevent wrapping quirkiness on some processors where >> 64 + x == >> x */
	if (UNEXPECTED((zend_ulong)op2_lval >= SIZEOF_ZEND_LONG * 8)) {
		if (EXPECTED(op2_lval > 0)) {
			if (op1 == result) {
				zval_ptr_dtor(result);
			}
			ZVAL_LONG(result, (op1_lval < 0) ? -1 : 0);
			return SUCCESS;
		} else {
			if (EG(current_execute_data) && !CG(in_compilation)) {
				zend_throw_exception_ex(zend_ce_arithmetic_error, 0, "Bit shift by negative number");
			} else {
				zend_error_noreturn(E_ERROR, "Bit shift by negative number");
			}
			if (op1 != result) {
				ZVAL_UNDEF(result);
			}
			return FAILURE;
		}
	}

	if (op1 == result) {
		zval_ptr_dtor(result);
	}

	ZVAL_LONG(result, op1_lval >> op2_lval);
	return SUCCESS;
}
/* }}} */

ZEND_API zend_result ZEND_FASTCALL concat_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
    zval *orig_op1 = op1;
	zval op1_copy, op2_copy;

	ZVAL_UNDEF(&op1_copy);
	ZVAL_UNDEF(&op2_copy);

	do {
	 	if (UNEXPECTED(Z_TYPE_P(op1) != IS_STRING)) {
	 		if (Z_ISREF_P(op1)) {
	 			op1 = Z_REFVAL_P(op1);
	 			if (Z_TYPE_P(op1) == IS_STRING) break;
	 		}
			ZEND_TRY_BINARY_OBJECT_OPERATION(ZEND_CONCAT);
			ZVAL_STR(&op1_copy, zval_get_string_func(op1));
			if (UNEXPECTED(EG(exception))) {
				zval_ptr_dtor_str(&op1_copy);
				if (orig_op1 != result) {
					ZVAL_UNDEF(result);
				}
				return FAILURE;
			}
			if (result == op1) {
				if (UNEXPECTED(op1 == op2)) {
					op2 = &op1_copy;
				}
			}
			op1 = &op1_copy;
		}
	} while (0);
	do {
		if (UNEXPECTED(Z_TYPE_P(op2) != IS_STRING)) {
	 		if (Z_ISREF_P(op2)) {
	 			op2 = Z_REFVAL_P(op2);
	 			if (Z_TYPE_P(op2) == IS_STRING) break;
	 		}
			ZEND_TRY_BINARY_OP2_OBJECT_OPERATION(ZEND_CONCAT);
			ZVAL_STR(&op2_copy, zval_get_string_func(op2));
			if (UNEXPECTED(EG(exception))) {
				zval_ptr_dtor_str(&op1_copy);
				zval_ptr_dtor_str(&op2_copy);
				if (orig_op1 != result) {
					ZVAL_UNDEF(result);
				}
				return FAILURE;
			}
			op2 = &op2_copy;
		}
	} while (0);

	if (UNEXPECTED(Z_STRLEN_P(op1) == 0)) {
		if (EXPECTED(result != op2)) {
			if (result == orig_op1) {
				i_zval_ptr_dtor(result);
			}
			ZVAL_COPY(result, op2);
		}
	} else if (UNEXPECTED(Z_STRLEN_P(op2) == 0)) {
		if (EXPECTED(result != op1)) {
			if (result == orig_op1) {
				i_zval_ptr_dtor(result);
			}
			ZVAL_COPY(result, op1);
		}
	} else {
		size_t op1_len = Z_STRLEN_P(op1);
		size_t op2_len = Z_STRLEN_P(op2);
		size_t result_len = op1_len + op2_len;
		zend_string *result_str;

		if (UNEXPECTED(op1_len > ZSTR_MAX_LEN - op2_len)) {
			zend_throw_error(NULL, "String size overflow");
			zval_ptr_dtor_str(&op1_copy);
			zval_ptr_dtor_str(&op2_copy);
			if (orig_op1 != result) {
				ZVAL_UNDEF(result);
			}
			return FAILURE;
		}

		if (result == op1 && Z_REFCOUNTED_P(result)) {
			/* special case, perform operations on result */
			result_str = zend_string_extend(Z_STR_P(result), result_len, 0);
		} else {
			result_str = zend_string_alloc(result_len, 0);
			memcpy(ZSTR_VAL(result_str), Z_STRVAL_P(op1), op1_len);
			if (result == orig_op1) {
				i_zval_ptr_dtor(result);
			}
		}

		/* This has to happen first to account for the cases where result == op1 == op2 and
		 * the realloc is done. In this case this line will also update Z_STRVAL_P(op2) to
		 * point to the new string. The first op2_len bytes of result will still be the same. */
		ZVAL_NEW_STR(result, result_str);

		memcpy(ZSTR_VAL(result_str) + op1_len, Z_STRVAL_P(op2), op2_len);
		ZSTR_VAL(result_str)[result_len] = '\0';
	}

	zval_ptr_dtor_str(&op1_copy);
	zval_ptr_dtor_str(&op2_copy);
	return SUCCESS;
}
/* }}} */

ZEND_API int ZEND_FASTCALL string_compare_function_ex(zval *op1, zval *op2, zend_bool case_insensitive) /* {{{ */
{
	zend_string *tmp_str1, *tmp_str2;
	zend_string *str1 = zval_get_tmp_string(op1, &tmp_str1);
	zend_string *str2 = zval_get_tmp_string(op2, &tmp_str2);
	int ret;

	if (case_insensitive) {
		ret = zend_binary_strcasecmp_l(ZSTR_VAL(str1), ZSTR_LEN(str1), ZSTR_VAL(str2), ZSTR_LEN(str2));
	} else {
		ret = zend_binary_strcmp(ZSTR_VAL(str1), ZSTR_LEN(str1), ZSTR_VAL(str2), ZSTR_LEN(str2));
	}

	zend_tmp_string_release(tmp_str1);
	zend_tmp_string_release(tmp_str2);
	return ret;
}
/* }}} */

ZEND_API int ZEND_FASTCALL string_compare_function(zval *op1, zval *op2) /* {{{ */
{
	if (EXPECTED(Z_TYPE_P(op1) == IS_STRING) &&
	    EXPECTED(Z_TYPE_P(op2) == IS_STRING)) {
		if (Z_STR_P(op1) == Z_STR_P(op2)) {
			return 0;
		} else {
			return zend_binary_strcmp(Z_STRVAL_P(op1), Z_STRLEN_P(op1), Z_STRVAL_P(op2), Z_STRLEN_P(op2));
		}
	} else {
		zend_string *tmp_str1, *tmp_str2;
		zend_string *str1 = zval_get_tmp_string(op1, &tmp_str1);
		zend_string *str2 = zval_get_tmp_string(op2, &tmp_str2);
		int ret = zend_binary_strcmp(ZSTR_VAL(str1), ZSTR_LEN(str1), ZSTR_VAL(str2), ZSTR_LEN(str2));

		zend_tmp_string_release(tmp_str1);
		zend_tmp_string_release(tmp_str2);
		return ret;
	}
}
/* }}} */

ZEND_API int ZEND_FASTCALL string_case_compare_function(zval *op1, zval *op2) /* {{{ */
{
	if (EXPECTED(Z_TYPE_P(op1) == IS_STRING) &&
	    EXPECTED(Z_TYPE_P(op2) == IS_STRING)) {
		if (Z_STR_P(op1) == Z_STR_P(op2)) {
			return 0;
		} else {
			return zend_binary_strcasecmp_l(Z_STRVAL_P(op1), Z_STRLEN_P(op1), Z_STRVAL_P(op2), Z_STRLEN_P(op2));
		}
	} else {
		zend_string *tmp_str1, *tmp_str2;
		zend_string *str1 = zval_get_tmp_string(op1, &tmp_str1);
		zend_string *str2 = zval_get_tmp_string(op2, &tmp_str2);
		int ret = zend_binary_strcasecmp_l(ZSTR_VAL(str1), ZSTR_LEN(str1), ZSTR_VAL(str2), ZSTR_LEN(str2));

		zend_tmp_string_release(tmp_str1);
		zend_tmp_string_release(tmp_str2);
		return ret;
	}
}
/* }}} */

ZEND_API int ZEND_FASTCALL string_locale_compare_function(zval *op1, zval *op2) /* {{{ */
{
	zend_string *tmp_str1, *tmp_str2;
	zend_string *str1 = zval_get_tmp_string(op1, &tmp_str1);
	zend_string *str2 = zval_get_tmp_string(op2, &tmp_str2);
	int ret = strcoll(ZSTR_VAL(str1), ZSTR_VAL(str2));

	zend_tmp_string_release(tmp_str1);
	zend_tmp_string_release(tmp_str2);
	return ret;
}
/* }}} */

ZEND_API int ZEND_FASTCALL numeric_compare_function(zval *op1, zval *op2) /* {{{ */
{
	double d1, d2;

	d1 = zval_get_double(op1);
	d2 = zval_get_double(op2);

	return ZEND_NORMALIZE_BOOL(d1 - d2);
}
/* }}} */

ZEND_API zend_result ZEND_FASTCALL compare_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	ZVAL_LONG(result, zend_compare(op1, op2));
	return SUCCESS;
}
/* }}} */

static int compare_long_to_string(zend_long lval, zend_string *str) /* {{{ */
{
	zend_long str_lval;
	double str_dval;
	zend_uchar type = is_numeric_string(ZSTR_VAL(str), ZSTR_LEN(str), &str_lval, &str_dval, 0);

	if (type == IS_LONG) {
		return lval > str_lval ? 1 : lval < str_lval ? -1 : 0;
	}

	if (type == IS_DOUBLE) {
		double diff = (double) lval - str_dval;
		return ZEND_NORMALIZE_BOOL(diff);
	}

	zend_string *lval_as_str = zend_long_to_str(lval);
	int cmp_result = zend_binary_strcmp(
		ZSTR_VAL(lval_as_str), ZSTR_LEN(lval_as_str), ZSTR_VAL(str), ZSTR_LEN(str));
	zend_string_release(lval_as_str);
	return ZEND_NORMALIZE_BOOL(cmp_result);
}
/* }}} */

static int compare_double_to_string(double dval, zend_string *str) /* {{{ */
{
	zend_long str_lval;
	double str_dval;
	zend_uchar type = is_numeric_string(ZSTR_VAL(str), ZSTR_LEN(str), &str_lval, &str_dval, 0);

	if (type == IS_LONG) {
		double diff = dval - (double) str_lval;
		return ZEND_NORMALIZE_BOOL(diff);
	}

	if (type == IS_DOUBLE) {
		if (dval == str_dval) {
			return 0;
		}
		return ZEND_NORMALIZE_BOOL(dval - str_dval);
	}

	zend_string *dval_as_str = zend_strpprintf(0, "%.*G", (int) EG(precision), dval);
	int cmp_result = zend_binary_strcmp(
		ZSTR_VAL(dval_as_str), ZSTR_LEN(dval_as_str), ZSTR_VAL(str), ZSTR_LEN(str));
	zend_string_release(dval_as_str);
	return ZEND_NORMALIZE_BOOL(cmp_result);
}
/* }}} */

ZEND_API int ZEND_FASTCALL zend_compare(zval *op1, zval *op2) /* {{{ */
{
	int converted = 0;
	zval op1_copy, op2_copy;

	while (1) {
		switch (TYPE_PAIR(Z_TYPE_P(op1), Z_TYPE_P(op2))) {
			case TYPE_PAIR(IS_LONG, IS_LONG):
				return Z_LVAL_P(op1)>Z_LVAL_P(op2)?1:(Z_LVAL_P(op1)<Z_LVAL_P(op2)?-1:0);

			case TYPE_PAIR(IS_DOUBLE, IS_LONG):
				return ZEND_NORMALIZE_BOOL(Z_DVAL_P(op1) - (double)Z_LVAL_P(op2));

			case TYPE_PAIR(IS_LONG, IS_DOUBLE):
				return ZEND_NORMALIZE_BOOL((double)Z_LVAL_P(op1) - Z_DVAL_P(op2));

			case TYPE_PAIR(IS_DOUBLE, IS_DOUBLE):
				if (Z_DVAL_P(op1) == Z_DVAL_P(op2)) {
					return 0;
				} else {
					return ZEND_NORMALIZE_BOOL(Z_DVAL_P(op1) - Z_DVAL_P(op2));
				}

			case TYPE_PAIR(IS_ARRAY, IS_ARRAY):
				return zend_compare_arrays(op1, op2);

			case TYPE_PAIR(IS_NULL, IS_NULL):
			case TYPE_PAIR(IS_NULL, IS_FALSE):
			case TYPE_PAIR(IS_FALSE, IS_NULL):
			case TYPE_PAIR(IS_FALSE, IS_FALSE):
			case TYPE_PAIR(IS_TRUE, IS_TRUE):
				return 0;

			case TYPE_PAIR(IS_NULL, IS_TRUE):
				return -1;

			case TYPE_PAIR(IS_TRUE, IS_NULL):
				return 1;

			case TYPE_PAIR(IS_STRING, IS_STRING):
				if (Z_STR_P(op1) == Z_STR_P(op2)) {
					return 0;
				}
				return zendi_smart_strcmp(Z_STR_P(op1), Z_STR_P(op2));

			case TYPE_PAIR(IS_NULL, IS_STRING):
				return Z_STRLEN_P(op2) == 0 ? 0 : -1;

			case TYPE_PAIR(IS_STRING, IS_NULL):
				return Z_STRLEN_P(op1) == 0 ? 0 : 1;

			case TYPE_PAIR(IS_LONG, IS_STRING):
				return compare_long_to_string(Z_LVAL_P(op1), Z_STR_P(op2));

			case TYPE_PAIR(IS_STRING, IS_LONG):
				return -compare_long_to_string(Z_LVAL_P(op2), Z_STR_P(op1));

			case TYPE_PAIR(IS_DOUBLE, IS_STRING):
				if (zend_isnan(Z_DVAL_P(op1))) {
					return 1;
				}

				return compare_double_to_string(Z_DVAL_P(op1), Z_STR_P(op2));

			case TYPE_PAIR(IS_STRING, IS_DOUBLE):
				if (zend_isnan(Z_DVAL_P(op2))) {
					return 1;
				}

				return -compare_double_to_string(Z_DVAL_P(op2), Z_STR_P(op1));

			case TYPE_PAIR(IS_OBJECT, IS_NULL):
				return 1;

			case TYPE_PAIR(IS_NULL, IS_OBJECT):
				return -1;

			default:
				if (Z_ISREF_P(op1)) {
					op1 = Z_REFVAL_P(op1);
					continue;
				} else if (Z_ISREF_P(op2)) {
					op2 = Z_REFVAL_P(op2);
					continue;
				}

				if (Z_TYPE_P(op1) == IS_OBJECT
				 && Z_TYPE_P(op2) == IS_OBJECT
				 && Z_OBJ_P(op1) == Z_OBJ_P(op2)) {
					return 0;
				} else if (Z_TYPE_P(op1) == IS_OBJECT) {
					return Z_OBJ_HANDLER_P(op1, compare)(op1, op2);
				} else if (Z_TYPE_P(op2) == IS_OBJECT) {
					return Z_OBJ_HANDLER_P(op2, compare)(op1, op2);
				}

				if (!converted) {
					if (Z_TYPE_P(op1) < IS_TRUE) {
						return zval_is_true(op2) ? -1 : 0;
					} else if (Z_TYPE_P(op1) == IS_TRUE) {
						return zval_is_true(op2) ? 0 : 1;
					} else if (Z_TYPE_P(op2) < IS_TRUE) {
						return zval_is_true(op1) ? 1 : 0;
					} else if (Z_TYPE_P(op2) == IS_TRUE) {
						return zval_is_true(op1) ? 0 : -1;
					} else {
						op1 = _zendi_convert_scalar_to_number_silent(op1, &op1_copy);
						op2 = _zendi_convert_scalar_to_number_silent(op2, &op2_copy);
						if (EG(exception)) {
							return 1; /* to stop comparison of arrays */
						}
						converted = 1;
					}
				} else if (Z_TYPE_P(op1)==IS_ARRAY) {
					return 1;
				} else if (Z_TYPE_P(op2)==IS_ARRAY) {
					return -1;
				} else {
					ZEND_UNREACHABLE();
					zend_throw_error(NULL, "Unsupported operand types");
					return 1;
				}
		}
	}
}
/* }}} */

/* return int to be compatible with compare_func_t */
static int hash_zval_identical_function(zval *z1, zval *z2) /* {{{ */
{
	/* is_identical_function() returns 1 in case of identity and 0 in case
	 * of a difference;
	 * whereas this comparison function is expected to return 0 on identity,
	 * and non zero otherwise.
	 */
	ZVAL_DEREF(z1);
	ZVAL_DEREF(z2);
	return fast_is_not_identical_function(z1, z2);
}
/* }}} */

ZEND_API zend_bool ZEND_FASTCALL zend_is_identical(zval *op1, zval *op2) /* {{{ */
{
	if (Z_TYPE_P(op1) != Z_TYPE_P(op2)) {
		return 0;
	}
	switch (Z_TYPE_P(op1)) {
		case IS_NULL:
		case IS_FALSE:
		case IS_TRUE:
			return 1;
		case IS_LONG:
			return (Z_LVAL_P(op1) == Z_LVAL_P(op2));
		case IS_RESOURCE:
			return (Z_RES_P(op1) == Z_RES_P(op2));
		case IS_DOUBLE:
			return (Z_DVAL_P(op1) == Z_DVAL_P(op2));
		case IS_STRING:
			return zend_string_equals(Z_STR_P(op1), Z_STR_P(op2));
		case IS_ARRAY:
			return (Z_ARRVAL_P(op1) == Z_ARRVAL_P(op2) ||
				zend_hash_compare(Z_ARRVAL_P(op1), Z_ARRVAL_P(op2), (compare_func_t) hash_zval_identical_function, 1) == 0);
		case IS_OBJECT:
			return (Z_OBJ_P(op1) == Z_OBJ_P(op2));
		default:
			return 0;
	}
}
/* }}} */

ZEND_API zend_result ZEND_FASTCALL is_identical_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	ZVAL_BOOL(result, zend_is_identical(op1, op2));
	return SUCCESS;
}
/* }}} */

ZEND_API zend_result ZEND_FASTCALL is_not_identical_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	ZVAL_BOOL(result, !zend_is_identical(op1, op2));
	return SUCCESS;
}
/* }}} */

ZEND_API zend_result ZEND_FASTCALL is_equal_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	ZVAL_BOOL(result, zend_compare(op1, op2) == 0);
	return SUCCESS;
}
/* }}} */

ZEND_API zend_result ZEND_FASTCALL is_not_equal_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	ZVAL_BOOL(result, (zend_compare(op1, op2) != 0));
	return SUCCESS;
}
/* }}} */

ZEND_API zend_result ZEND_FASTCALL is_smaller_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	ZVAL_BOOL(result, (zend_compare(op1, op2) < 0));
	return SUCCESS;
}
/* }}} */

ZEND_API zend_result ZEND_FASTCALL is_smaller_or_equal_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	ZVAL_BOOL(result, (zend_compare(op1, op2) <= 0));
	return SUCCESS;
}
/* }}} */

ZEND_API zend_bool ZEND_FASTCALL zend_class_implements_interface(const zend_class_entry *class_ce, const zend_class_entry *interface_ce) /* {{{ */
{
	uint32_t i;
	ZEND_ASSERT(interface_ce->ce_flags & ZEND_ACC_INTERFACE);

	if (class_ce->num_interfaces) {
		ZEND_ASSERT(class_ce->ce_flags & ZEND_ACC_RESOLVED_INTERFACES);
		for (i = 0; i < class_ce->num_interfaces; i++) {
			if (class_ce->interfaces[i] == interface_ce) {
				return 1;
			}
		}
	}
	return 0;
}
/* }}} */

ZEND_API zend_bool ZEND_FASTCALL instanceof_function_slow(const zend_class_entry *instance_ce, const zend_class_entry *ce) /* {{{ */
{
	ZEND_ASSERT(instance_ce != ce && "Should have been checked already");
	if (ce->ce_flags & ZEND_ACC_INTERFACE) {
		uint32_t i;

		if (instance_ce->num_interfaces) {
			ZEND_ASSERT(instance_ce->ce_flags & ZEND_ACC_RESOLVED_INTERFACES);
			for (i = 0; i < instance_ce->num_interfaces; i++) {
				if (instance_ce->interfaces[i] == ce) {
					return 1;
				}
			}
		}
		return 0;
	} else {
		while (1) {
			instance_ce = instance_ce->parent;
			if (instance_ce == ce) {
				return 1;
			}
			if (instance_ce == NULL) {
				return 0;
			}
		}
	}
}
/* }}} */

#define LOWER_CASE 1
#define UPPER_CASE 2
#define NUMERIC 3

static void ZEND_FASTCALL increment_string(zval *str) /* {{{ */
{
	int carry=0;
	size_t pos=Z_STRLEN_P(str)-1;
	char *s;
	zend_string *t;
	int last=0; /* Shut up the compiler warning */
	int ch;

	if (Z_STRLEN_P(str) == 0) {
		zval_ptr_dtor_str(str);
		ZVAL_CHAR(str, '1');
		return;
	}

	if (!Z_REFCOUNTED_P(str)) {
		Z_STR_P(str) = zend_string_init(Z_STRVAL_P(str), Z_STRLEN_P(str), 0);
		Z_TYPE_INFO_P(str) = IS_STRING_EX;
	} else if (Z_REFCOUNT_P(str) > 1) {
		Z_DELREF_P(str);
		Z_STR_P(str) = zend_string_init(Z_STRVAL_P(str), Z_STRLEN_P(str), 0);
	} else {
		zend_string_forget_hash_val(Z_STR_P(str));
	}
	s = Z_STRVAL_P(str);

	do {
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
	} while (pos-- > 0);

	if (carry) {
		t = zend_string_alloc(Z_STRLEN_P(str)+1, 0);
		memcpy(ZSTR_VAL(t) + 1, Z_STRVAL_P(str), Z_STRLEN_P(str));
		ZSTR_VAL(t)[Z_STRLEN_P(str) + 1] = '\0';
		switch (last) {
			case NUMERIC:
				ZSTR_VAL(t)[0] = '1';
				break;
			case UPPER_CASE:
				ZSTR_VAL(t)[0] = 'A';
				break;
			case LOWER_CASE:
				ZSTR_VAL(t)[0] = 'a';
				break;
		}
		zend_string_free(Z_STR_P(str));
		ZVAL_NEW_STR(str, t);
	}
}
/* }}} */

ZEND_API zend_result ZEND_FASTCALL increment_function(zval *op1) /* {{{ */
{
try_again:
	switch (Z_TYPE_P(op1)) {
		case IS_LONG:
			fast_long_increment_function(op1);
			break;
		case IS_DOUBLE:
			Z_DVAL_P(op1) = Z_DVAL_P(op1) + 1;
			break;
		case IS_NULL:
			ZVAL_LONG(op1, 1);
			break;
		case IS_STRING: {
				zend_long lval;
				double dval;

				switch (is_numeric_str_function(Z_STR_P(op1), &lval, &dval)) {
					case IS_LONG:
						zval_ptr_dtor_str(op1);
						if (lval == ZEND_LONG_MAX) {
							/* switch to double */
							double d = (double)lval;
							ZVAL_DOUBLE(op1, d+1);
						} else {
							ZVAL_LONG(op1, lval+1);
						}
						break;
					case IS_DOUBLE:
						zval_ptr_dtor_str(op1);
						ZVAL_DOUBLE(op1, dval+1);
						break;
					default:
						/* Perl style string increment */
						increment_string(op1);
						break;
				}
			}
			break;
		case IS_FALSE:
		case IS_TRUE:
			/* Do nothing. */
			break;
		case IS_REFERENCE:
			op1 = Z_REFVAL_P(op1);
			goto try_again;
		case IS_OBJECT:
			if (Z_OBJ_HANDLER_P(op1, do_operation)) {
				zval op2;
				ZVAL_LONG(&op2, 1);
				if (Z_OBJ_HANDLER_P(op1, do_operation)(ZEND_ADD, op1, op1, &op2) == SUCCESS) {
					return SUCCESS;
				}
			}
			/* break missing intentionally */
		case IS_RESOURCE:
		case IS_ARRAY:
			zend_type_error("Cannot increment %s", zend_zval_type_name(op1));
			return FAILURE;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
	return SUCCESS;
}
/* }}} */

ZEND_API zend_result ZEND_FASTCALL decrement_function(zval *op1) /* {{{ */
{
	zend_long lval;
	double dval;

try_again:
	switch (Z_TYPE_P(op1)) {
		case IS_LONG:
			fast_long_decrement_function(op1);
			break;
		case IS_DOUBLE:
			Z_DVAL_P(op1) = Z_DVAL_P(op1) - 1;
			break;
		case IS_STRING:		/* Like perl we only support string increment */
			if (Z_STRLEN_P(op1) == 0) { /* consider as 0 */
				zval_ptr_dtor_str(op1);
				ZVAL_LONG(op1, -1);
				break;
			}
			switch (is_numeric_str_function(Z_STR_P(op1), &lval, &dval)) {
				case IS_LONG:
					zval_ptr_dtor_str(op1);
					if (lval == ZEND_LONG_MIN) {
						double d = (double)lval;
						ZVAL_DOUBLE(op1, d-1);
					} else {
						ZVAL_LONG(op1, lval-1);
					}
					break;
				case IS_DOUBLE:
					zval_ptr_dtor_str(op1);
					ZVAL_DOUBLE(op1, dval - 1);
					break;
			}
			break;
		case IS_NULL:
		case IS_FALSE:
		case IS_TRUE:
			/* Do nothing. */
			break;
		case IS_REFERENCE:
			op1 = Z_REFVAL_P(op1);
			goto try_again;
		case IS_OBJECT:
			if (Z_OBJ_HANDLER_P(op1, do_operation)) {
				zval op2;
				ZVAL_LONG(&op2, 1);
				if (Z_OBJ_HANDLER_P(op1, do_operation)(ZEND_SUB, op1, op1, &op2) == SUCCESS) {
					return SUCCESS;
				}
			}
			/* break missing intentionally */
		case IS_RESOURCE:
		case IS_ARRAY:
			zend_type_error("Cannot decrement %s", zend_zval_type_name(op1));
			return FAILURE;
		EMPTY_SWITCH_DEFAULT_CASE()
	}

	return SUCCESS;
}
/* }}} */

ZEND_API int ZEND_FASTCALL zend_is_true(zval *op) /* {{{ */
{
	return (int) i_zend_is_true(op);
}
/* }}} */

ZEND_API bool ZEND_FASTCALL zend_object_is_true(zval *op) /* {{{ */
{
	zend_object *zobj = Z_OBJ_P(op);
	zval tmp;
	if (zobj->handlers->cast_object(zobj, &tmp, _IS_BOOL) == SUCCESS) {
		return Z_TYPE(tmp) == IS_TRUE;
	}
	zend_error(E_RECOVERABLE_ERROR, "Object of class %s could not be converted to bool", ZSTR_VAL(zobj->ce->name));
	return false;
}
/* }}} */

#ifdef ZEND_USE_TOLOWER_L
ZEND_API void zend_update_current_locale(void) /* {{{ */
{
	current_locale = _get_current_locale();
}
/* }}} */
#endif

static zend_always_inline void zend_str_tolower_impl(char *dest, const char *str, size_t length) /* {{{ */ {
	unsigned char *p = (unsigned char*)str;
	unsigned char *q = (unsigned char*)dest;
	unsigned char *end = p + length;
#ifdef __SSE2__
	if (length >= 16) {
		const __m128i _A = _mm_set1_epi8('A' - 1);
		const __m128i Z_ = _mm_set1_epi8('Z' + 1);
		const __m128i delta = _mm_set1_epi8('a' - 'A');
		do {
			__m128i op = _mm_loadu_si128((__m128i*)p);
			__m128i gt = _mm_cmpgt_epi8(op, _A);
			__m128i lt = _mm_cmplt_epi8(op, Z_);
			__m128i mingle = _mm_and_si128(gt, lt);
			__m128i add = _mm_and_si128(mingle, delta);
			__m128i lower = _mm_add_epi8(op, add);
			_mm_storeu_si128((__m128i *)q, lower);
			p += 16;
			q += 16;
		} while (p + 16 <= end);
	}
#endif
	while (p < end) {
		*q++ = zend_tolower_ascii(*p++);
	}
}
/* }}} */

ZEND_API char* ZEND_FASTCALL zend_str_tolower_copy(char *dest, const char *source, size_t length) /* {{{ */
{
	zend_str_tolower_impl(dest, source, length);
	dest[length] = '\0';
	return dest;
}
/* }}} */

ZEND_API char* ZEND_FASTCALL zend_str_tolower_dup(const char *source, size_t length) /* {{{ */
{
	return zend_str_tolower_copy((char *)emalloc(length+1), source, length);
}
/* }}} */

ZEND_API void ZEND_FASTCALL zend_str_tolower(char *str, size_t length) /* {{{ */
{
	zend_str_tolower_impl(str, (const char*)str, length);
}
/* }}} */

ZEND_API char* ZEND_FASTCALL zend_str_tolower_dup_ex(const char *source, size_t length) /* {{{ */
{
	register const unsigned char *p = (const unsigned char*)source;
	register const unsigned char *end = p + length;

	while (p < end) {
		if (*p != zend_tolower_ascii(*p)) {
			char *res = (char*)emalloc(length + 1);
			register unsigned char *r;

			if (p != (const unsigned char*)source) {
				memcpy(res, source, p - (const unsigned char*)source);
			}
			r = (unsigned char*)p + (res - source);
			zend_str_tolower_impl((char *)r, (const char*)p, end - p);
			res[length] = '\0';
			return res;
		}
		p++;
	}
	return NULL;
}
/* }}} */

ZEND_API zend_string* ZEND_FASTCALL zend_string_tolower_ex(zend_string *str, bool persistent) /* {{{ */
{
	size_t length = ZSTR_LEN(str);
	unsigned char *p = (unsigned char *) ZSTR_VAL(str);
	unsigned char *end = p + length;

#ifdef __SSE2__
	while (p + 16 <= end) {
		const __m128i _A = _mm_set1_epi8('A' - 1);
		const __m128i Z_ = _mm_set1_epi8('Z' + 1);
		__m128i op = _mm_loadu_si128((__m128i*)p);
		__m128i gt = _mm_cmpgt_epi8(op, _A);
		__m128i lt = _mm_cmplt_epi8(op, Z_);
		__m128i mingle = _mm_and_si128(gt, lt);
		if (_mm_movemask_epi8(mingle)) {
			zend_string *res = zend_string_alloc(length, persistent);
			memcpy(ZSTR_VAL(res), ZSTR_VAL(str), p - (unsigned char *) ZSTR_VAL(str));
			unsigned char *q = p + (ZSTR_VAL(res) - ZSTR_VAL(str));

			/* Lowercase the chunk we already compared. */
			const __m128i delta = _mm_set1_epi8('a' - 'A');
			__m128i add = _mm_and_si128(mingle, delta);
			__m128i lower = _mm_add_epi8(op, add);
			_mm_storeu_si128((__m128i *) q, lower);

			/* Lowercase the rest of the string. */
			p += 16; q += 16;
			zend_str_tolower_impl((char *) q, (const char *) p, end - p);
			ZSTR_VAL(res)[length] = '\0';
			return res;
		}
		p += 16;
	}
#endif

	while (p < end) {
		if (*p != zend_tolower_ascii(*p)) {
			zend_string *res = zend_string_alloc(length, persistent);
			memcpy(ZSTR_VAL(res), ZSTR_VAL(str), p - (unsigned char*) ZSTR_VAL(str));

			unsigned char *q = p + (ZSTR_VAL(res) - ZSTR_VAL(str));
			while (p < end) {
				*q++ = zend_tolower_ascii(*p++);
			}
			ZSTR_VAL(res)[length] = '\0';
			return res;
		}
		p++;
	}

	return zend_string_copy(str);
}
/* }}} */

ZEND_API int ZEND_FASTCALL zend_binary_strcmp(const char *s1, size_t len1, const char *s2, size_t len2) /* {{{ */
{
	int retval;

	if (s1 == s2) {
		return 0;
	}
	retval = memcmp(s1, s2, MIN(len1, len2));
	if (!retval) {
		return (int)(len1 - len2);
	} else {
		return retval;
	}
}
/* }}} */

ZEND_API int ZEND_FASTCALL zend_binary_strncmp(const char *s1, size_t len1, const char *s2, size_t len2, size_t length) /* {{{ */
{
	int retval;

	if (s1 == s2) {
		return 0;
	}
	retval = memcmp(s1, s2, MIN(length, MIN(len1, len2)));
	if (!retval) {
		return (int)(MIN(length, len1) - MIN(length, len2));
	} else {
		return retval;
	}
}
/* }}} */

ZEND_API int ZEND_FASTCALL zend_binary_strcasecmp(const char *s1, size_t len1, const char *s2, size_t len2) /* {{{ */
{
	size_t len;
	int c1, c2;

	if (s1 == s2) {
		return 0;
	}

	len = MIN(len1, len2);
	while (len--) {
		c1 = zend_tolower_ascii(*(unsigned char *)s1++);
		c2 = zend_tolower_ascii(*(unsigned char *)s2++);
		if (c1 != c2) {
			return c1 - c2;
		}
	}

	return (int)(len1 - len2);
}
/* }}} */

ZEND_API int ZEND_FASTCALL zend_binary_strncasecmp(const char *s1, size_t len1, const char *s2, size_t len2, size_t length) /* {{{ */
{
	size_t len;
	int c1, c2;

	if (s1 == s2) {
		return 0;
	}
	len = MIN(length, MIN(len1, len2));
	while (len--) {
		c1 = zend_tolower_ascii(*(unsigned char *)s1++);
		c2 = zend_tolower_ascii(*(unsigned char *)s2++);
		if (c1 != c2) {
			return c1 - c2;
		}
	}

	return (int)(MIN(length, len1) - MIN(length, len2));
}
/* }}} */

ZEND_API int ZEND_FASTCALL zend_binary_strcasecmp_l(const char *s1, size_t len1, const char *s2, size_t len2) /* {{{ */
{
	size_t len;
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

	return (int)(len1 - len2);
}
/* }}} */

ZEND_API int ZEND_FASTCALL zend_binary_strncasecmp_l(const char *s1, size_t len1, const char *s2, size_t len2, size_t length) /* {{{ */
{
	size_t len;
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

	return (int)(MIN(length, len1) - MIN(length, len2));
}
/* }}} */

ZEND_API int ZEND_FASTCALL zend_binary_zval_strcmp(zval *s1, zval *s2) /* {{{ */
{
	return zend_binary_strcmp(Z_STRVAL_P(s1), Z_STRLEN_P(s1), Z_STRVAL_P(s2), Z_STRLEN_P(s2));
}
/* }}} */

ZEND_API int ZEND_FASTCALL zend_binary_zval_strncmp(zval *s1, zval *s2, zval *s3) /* {{{ */
{
	return zend_binary_strncmp(Z_STRVAL_P(s1), Z_STRLEN_P(s1), Z_STRVAL_P(s2), Z_STRLEN_P(s2), Z_LVAL_P(s3));
}
/* }}} */

ZEND_API int ZEND_FASTCALL zend_binary_zval_strcasecmp(zval *s1, zval *s2) /* {{{ */
{
	return zend_binary_strcasecmp_l(Z_STRVAL_P(s1), Z_STRLEN_P(s1), Z_STRVAL_P(s2), Z_STRLEN_P(s2));
}
/* }}} */

ZEND_API int ZEND_FASTCALL zend_binary_zval_strncasecmp(zval *s1, zval *s2, zval *s3) /* {{{ */
{
	return zend_binary_strncasecmp_l(Z_STRVAL_P(s1), Z_STRLEN_P(s1), Z_STRVAL_P(s2), Z_STRLEN_P(s2), Z_LVAL_P(s3));
}
/* }}} */

ZEND_API bool ZEND_FASTCALL zendi_smart_streq(zend_string *s1, zend_string *s2) /* {{{ */
{
	zend_uchar ret1, ret2;
	int oflow1, oflow2;
	zend_long lval1 = 0, lval2 = 0;
	double dval1 = 0.0, dval2 = 0.0;

	if ((ret1 = is_numeric_string_ex(s1->val, s1->len, &lval1, &dval1, false, &oflow1, NULL)) &&
		(ret2 = is_numeric_string_ex(s2->val, s2->len, &lval2, &dval2, false, &oflow2, NULL))) {
#if ZEND_ULONG_MAX == 0xFFFFFFFF
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
		if ((ret1 == IS_DOUBLE) || (ret2 == IS_DOUBLE)) {
			if (ret1 != IS_DOUBLE) {
				if (oflow2) {
					/* 2nd operand is integer > LONG_MAX (oflow2==1) or < LONG_MIN (-1) */
					return 0;
				}
				dval1 = (double) lval1;
			} else if (ret2 != IS_DOUBLE) {
				if (oflow1) {
					return 0;
				}
				dval2 = (double) lval2;
			} else if (dval1 == dval2 && !zend_finite(dval1)) {
				/* Both values overflowed and have the same sign,
				 * so a numeric comparison would be inaccurate */
				goto string_cmp;
			}
			return dval1 == dval2;
		} else { /* they both have to be long's */
			return lval1 == lval2;
		}
	} else {
string_cmp:
		return zend_string_equal_content(s1, s2);
	}
}
/* }}} */

ZEND_API int ZEND_FASTCALL zendi_smart_strcmp(zend_string *s1, zend_string *s2) /* {{{ */
{
	zend_uchar ret1, ret2;
	int oflow1, oflow2;
	zend_long lval1 = 0, lval2 = 0;
	double dval1 = 0.0, dval2 = 0.0;

	if ((ret1 = is_numeric_string_ex(s1->val, s1->len, &lval1, &dval1, false, &oflow1, NULL)) &&
		(ret2 = is_numeric_string_ex(s2->val, s2->len, &lval2, &dval2, false, &oflow2, NULL))) {
#if ZEND_ULONG_MAX == 0xFFFFFFFF
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
		if ((ret1 == IS_DOUBLE) || (ret2 == IS_DOUBLE)) {
			if (ret1 != IS_DOUBLE) {
				if (oflow2) {
					/* 2nd operand is integer > LONG_MAX (oflow2==1) or < LONG_MIN (-1) */
					return -1 * oflow2;
				}
				dval1 = (double) lval1;
			} else if (ret2 != IS_DOUBLE) {
				if (oflow1) {
					return oflow1;
				}
				dval2 = (double) lval2;
			} else if (dval1 == dval2 && !zend_finite(dval1)) {
				/* Both values overflowed and have the same sign,
				 * so a numeric comparison would be inaccurate */
				goto string_cmp;
			}
			dval1 = dval1 - dval2;
			return ZEND_NORMALIZE_BOOL(dval1);
		} else { /* they both have to be long's */
			return lval1 > lval2 ? 1 : (lval1 < lval2 ? -1 : 0);
		}
	} else {
		int strcmp_ret;
string_cmp:
		strcmp_ret = zend_binary_strcmp(s1->val, s1->len, s2->val, s2->len);
		return ZEND_NORMALIZE_BOOL(strcmp_ret);
	}
}
/* }}} */

static int hash_zval_compare_function(zval *z1, zval *z2) /* {{{ */
{
	return zend_compare(z1, z2);
}
/* }}} */

ZEND_API int ZEND_FASTCALL zend_compare_symbol_tables(HashTable *ht1, HashTable *ht2) /* {{{ */
{
	return ht1 == ht2 ? 0 : zend_hash_compare(ht1, ht2, (compare_func_t) hash_zval_compare_function, 0);
}
/* }}} */

ZEND_API int ZEND_FASTCALL zend_compare_arrays(zval *a1, zval *a2) /* {{{ */
{
	return zend_compare_symbol_tables(Z_ARRVAL_P(a1), Z_ARRVAL_P(a2));
}
/* }}} */

ZEND_API int ZEND_FASTCALL zend_compare_objects(zval *o1, zval *o2) /* {{{ */
{
	if (Z_OBJ_P(o1) == Z_OBJ_P(o2)) {
		return 0;
	}

	if (Z_OBJ_HT_P(o1)->compare == NULL) {
		return 1;
	} else {
		return Z_OBJ_HT_P(o1)->compare(o1, o2);
	}
}
/* }}} */

ZEND_API void ZEND_FASTCALL zend_locale_sprintf_double(zval *op ZEND_FILE_LINE_DC) /* {{{ */
{
	zend_string *str;

	str = zend_strpprintf(0, "%.*G", (int) EG(precision), (double)Z_DVAL_P(op));
	ZVAL_NEW_STR(op, str);
}
/* }}} */

ZEND_API zend_string* ZEND_FASTCALL zend_long_to_str(zend_long num) /* {{{ */
{
	if ((zend_ulong)num <= 9) {
		return ZSTR_CHAR((zend_uchar)'0' + (zend_uchar)num);
	} else {
		char buf[MAX_LENGTH_OF_LONG + 1];
		char *res = zend_print_long_to_buf(buf + sizeof(buf) - 1, num);
		return zend_string_init(res, buf + sizeof(buf) - 1 - res, 0);
	}
}
/* }}} */

ZEND_API zend_uchar ZEND_FASTCALL is_numeric_str_function(const zend_string *str, zend_long *lval, double *dval) /* {{{ */ {
    return is_numeric_string(ZSTR_VAL(str), ZSTR_LEN(str), lval, dval, false);
}
/* }}} */

ZEND_API zend_uchar ZEND_FASTCALL _is_numeric_string_ex(const char *str, size_t length, zend_long *lval,
	double *dval, bool allow_errors, int *oflow_info, bool *trailing_data) /* {{{ */
{
	const char *ptr;
	int digits = 0, dp_or_e = 0;
	double local_dval = 0.0;
	zend_uchar type;
	zend_ulong tmp_lval = 0;
	int neg = 0;

	if (!length) {
		return 0;
	}

	if (oflow_info != NULL) {
		*oflow_info = 0;
	}
	if (trailing_data != NULL) {
		*trailing_data = false;
	}

	/* Skip any whitespace
	 * This is much faster than the isspace() function */
	while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r' || *str == '\v' || *str == '\f') {
		str++;
		length--;
	}
	ptr = str;

	if (*ptr == '-') {
		neg = 1;
		ptr++;
	} else if (*ptr == '+') {
		ptr++;
	}

	if (ZEND_IS_DIGIT(*ptr)) {
		/* Skip any leading 0s */
		while (*ptr == '0') {
			ptr++;
		}

		/* Count the number of digits. If a decimal point/exponent is found,
		 * it's a double. Otherwise, if there's a dval or no need to check for
		 * a full match, stop when there are too many digits for a long */
		for (type = IS_LONG; !(digits >= MAX_LENGTH_OF_LONG && (dval || allow_errors)); digits++, ptr++) {
check_digits:
			if (ZEND_IS_DIGIT(*ptr)) {
				tmp_lval = tmp_lval * 10 + (*ptr) - '0';
				continue;
			} else if (*ptr == '.' && dp_or_e < 1) {
				goto process_double;
			} else if ((*ptr == 'e' || *ptr == 'E') && dp_or_e < 2) {
				const char *e = ptr + 1;

				if (*e == '-' || *e == '+') {
					ptr = e++;
				}
				if (ZEND_IS_DIGIT(*e)) {
					goto process_double;
				}
			}

			break;
		}

		if (digits >= MAX_LENGTH_OF_LONG) {
			if (oflow_info != NULL) {
				*oflow_info = *str == '-' ? -1 : 1;
			}
			dp_or_e = -1;
			goto process_double;
		}
	} else if (*ptr == '.' && ZEND_IS_DIGIT(ptr[1])) {
process_double:
		type = IS_DOUBLE;

		/* If there's a dval, do the conversion; else continue checking
		 * the digits if we need to check for a full match */
		if (dval) {
			local_dval = zend_strtod(str, &ptr);
		} else if (!allow_errors && dp_or_e != -1) {
			dp_or_e = (*ptr++ == '.') ? 1 : 2;
			goto check_digits;
		}
	} else {
		return 0;
	}

	if (ptr != str + length) {
		const char *endptr = ptr;
		while (*endptr == ' ' || *endptr == '\t' || *endptr == '\n' || *endptr == '\r' || *endptr == '\v' || *endptr == '\f') {
			endptr++;
			length--;
		}
		if (ptr != str + length) {
			if (!allow_errors) {
				return 0;
			}
			if (trailing_data != NULL) {
				*trailing_data = true;
			}
		}
	}

	if (type == IS_LONG) {
		if (digits == MAX_LENGTH_OF_LONG - 1) {
			int cmp = strcmp(&ptr[-digits], long_min_digits);

			if (!(cmp < 0 || (cmp == 0 && *str == '-'))) {
				if (dval) {
					*dval = zend_strtod(str, NULL);
				}
				if (oflow_info != NULL) {
					*oflow_info = *str == '-' ? -1 : 1;
				}

				return IS_DOUBLE;
			}
		}

		if (lval) {
			if (neg) {
				tmp_lval = -tmp_lval;
			}
			*lval = (zend_long) tmp_lval;
		}

		return IS_LONG;
	} else {
		if (dval) {
			*dval = local_dval;
		}

		return IS_DOUBLE;
	}
}
/* }}} */

/*
 * String matching - Sunday algorithm
 * http://www.iti.fh-flensburg.de/lang/algorithmen/pattern/sundayen.htm
 */
static zend_always_inline void zend_memnstr_ex_pre(unsigned int td[], const char *needle, size_t needle_len, int reverse) /* {{{ */ {
	int i;

	for (i = 0; i < 256; i++) {
		td[i] = needle_len + 1;
	}

	if (reverse) {
		for (i = needle_len - 1; i >= 0; i--) {
			td[(unsigned char)needle[i]] = i + 1;
		}
	} else {
		size_t i;

		for (i = 0; i < needle_len; i++) {
			td[(unsigned char)needle[i]] = (int)needle_len - i;
		}
	}
}
/* }}} */

ZEND_API const char* ZEND_FASTCALL zend_memnstr_ex(const char *haystack, const char *needle, size_t needle_len, const char *end) /* {{{ */
{
	unsigned int td[256];
	register size_t i;
	register const char *p;

	if (needle_len == 0 || (end - haystack) < needle_len) {
		return NULL;
	}

	zend_memnstr_ex_pre(td, needle, needle_len, 0);

	p = haystack;
	end -= needle_len;

	while (p <= end) {
		for (i = 0; i < needle_len; i++) {
			if (needle[i] != p[i]) {
				break;
			}
		}
		if (i == needle_len) {
			return p;
		}
		if (UNEXPECTED(p == end)) {
			return NULL;
		}
		p += td[(unsigned char)(p[needle_len])];
	}

	return NULL;
}
/* }}} */

ZEND_API const char* ZEND_FASTCALL zend_memnrstr_ex(const char *haystack, const char *needle, size_t needle_len, const char *end) /* {{{ */
{
	unsigned int td[256];
	register size_t i;
	register const char *p;

	if (needle_len == 0 || (end - haystack) < needle_len) {
		return NULL;
	}

	zend_memnstr_ex_pre(td, needle, needle_len, 1);

	p = end;
	p -= needle_len;

	while (p >= haystack) {
		for (i = 0; i < needle_len; i++) {
			if (needle[i] != p[i]) {
				break;
			}
		}

		if (i == needle_len) {
			return (const char *)p;
		}

		if (UNEXPECTED(p == haystack)) {
			return NULL;
		}

		p -= td[(unsigned char)(p[-1])];
	}

	return NULL;
}
/* }}} */

#if SIZEOF_ZEND_LONG == 4
ZEND_API zend_long ZEND_FASTCALL zend_dval_to_lval_slow(double d) /* {{{ */
{
	double	two_pow_32 = pow(2., 32.),
			dmod;

	dmod = fmod(d, two_pow_32);
	if (dmod < 0) {
		/* we're going to make this number positive; call ceil()
		 * to simulate rounding towards 0 of the negative number */
		dmod = ceil(dmod) + two_pow_32;
	}
	return (zend_long)(zend_ulong)dmod;
}
#else
ZEND_API zend_long ZEND_FASTCALL zend_dval_to_lval_slow(double d)
{
	double	two_pow_64 = pow(2., 64.),
			dmod;

	dmod = fmod(d, two_pow_64);
	if (dmod < 0) {
		/* no need to call ceil; original double must have had no
		 * fractional part, hence dmod does not have one either */
		dmod += two_pow_64;
	}
	return (zend_long)(zend_ulong)dmod;
}
/* }}} */
#endif
