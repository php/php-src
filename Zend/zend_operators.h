/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2002 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


#ifndef ZEND_OPERATORS_H
#define ZEND_OPERATORS_H

#include <errno.h>
#include <math.h>

#ifdef HAVE_IEEEFP_H
#include <ieeefp.h>
#endif


#if 0&&WITH_BCMATH
#include "ext/bcmath/libbcmath/src/bcmath.h"
#endif

#define MAX_LENGTH_OF_LONG 18
#define MAX_LENGTH_OF_DOUBLE 32

ZEND_API int add_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int sub_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int mul_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int div_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int mod_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int boolean_xor_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int boolean_not_function(zval *result, zval *op1 TSRMLS_DC);
ZEND_API int bitwise_not_function(zval *result, zval *op1 TSRMLS_DC);
ZEND_API int bitwise_or_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int bitwise_and_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int bitwise_xor_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int shift_left_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int shift_right_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int concat_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);

ZEND_API int is_equal_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int is_identical_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int is_not_identical_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int is_not_equal_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int is_smaller_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int is_smaller_or_equal_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);

ZEND_API int is_class_function(zval *result, zval *op1, zend_class_entry *ce TSRMLS_DC);

static inline zend_bool is_numeric_string(char *str, int length, long *lval, double *dval, zend_bool allow_errors)
{
	long local_lval;
	double local_dval;
	char *end_ptr_long, *end_ptr_double;
	int conv_base=10;

	if (!length) {
		return 0;
	}
	
	/* handle hex numbers */
	if (length>=2 && str[0]=='0' && (str[1]=='x' || str[1]=='X')) {
		conv_base=16;
	}
	errno=0;
	local_lval = strtol(str, &end_ptr_long, conv_base);
	if (errno!=ERANGE) {
		if (end_ptr_long == str+length) { /* integer string */
			if (lval) {
				*lval = local_lval;
			}
			return IS_LONG;
		}
	} else {
		end_ptr_long=NULL;
	}

	if (conv_base==16) { /* hex string, under UNIX strtod() messes it up */
		return 0;
	}

	errno=0;
	local_dval = strtod(str, &end_ptr_double);
	if (errno!=ERANGE) {
		if (end_ptr_double == str+length) { /* floating point string */
			if (! zend_finite(local_dval)) {
				/* "inf","nan" and maybe other weird ones */
				return 0;
			}

			if (dval) {
				*dval = local_dval;
			}
#if 0&&WITH_BCMATH
			if (length>16) {
				register char *ptr=str, *end=str+length;
				
				while (ptr<end) {
					switch(*ptr++) {
						case 'e':
						case 'E':
							/* scientific notation, not handled by the BC library */
							return IS_DOUBLE;
							break;
						default:
							break;
					}
				}
				return FLAG_IS_BC;
			} else {
				return IS_DOUBLE;
			}
#else
			return IS_DOUBLE;
#endif
		}
	} else {
		end_ptr_double=NULL;
	}
	if (allow_errors) {
		if (end_ptr_double>end_ptr_long && dval) {
			*dval = local_dval;
			return IS_DOUBLE;
		} else if (end_ptr_long && lval) {
			*lval = local_lval;
			return IS_LONG;
		}
	}
	return 0;
}

ZEND_API int increment_function(zval *op1);
ZEND_API int decrement_function(zval *op2);

BEGIN_EXTERN_C()
ZEND_API void convert_scalar_to_number(zval *op TSRMLS_DC);
ZEND_API void _convert_to_string(zval *op ZEND_FILE_LINE_DC);
ZEND_API void convert_to_long(zval *op);
ZEND_API void convert_to_double(zval *op);
ZEND_API void convert_to_long_base(zval *op, int base);
ZEND_API void convert_to_null(zval *op);
ZEND_API void convert_to_boolean(zval *op);
ZEND_API void convert_to_array(zval *op);
ZEND_API void convert_to_object(zval *op);
ZEND_API void multi_convert_to_long_ex(int argc, ...);
ZEND_API void multi_convert_to_double_ex(int argc, ...);
ZEND_API void multi_convert_to_string_ex(int argc, ...);
ZEND_API int add_char_to_string(zval *result, zval *op1, zval *op2);
ZEND_API int add_string_to_string(zval *result, zval *op1, zval *op2);
#define convert_to_string(op)			_convert_to_string((op) ZEND_FILE_LINE_CC)

ZEND_API double zend_string_to_double(const char *number, zend_uint length);
END_EXTERN_C()

ZEND_API int zval_is_true(zval *op);
ZEND_API int compare_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int numeric_compare_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int string_compare_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);

ZEND_API void zend_str_tolower(char *str, unsigned int length);
ZEND_API int zend_binary_zval_strcmp(zval *s1, zval *s2);
ZEND_API int zend_binary_zval_strncmp(zval *s1, zval *s2, zval *s3);
ZEND_API int zend_binary_zval_strcasecmp(zval *s1, zval *s2);
ZEND_API int zend_binary_zval_strncasecmp(zval *s1, zval *s2, zval *s3);
ZEND_API int zend_binary_strcmp(char *s1, uint len1, char *s2, uint len2);
ZEND_API int zend_binary_strncmp(char *s1, uint len1, char *s2, uint len2, uint length);
ZEND_API int zend_binary_strcasecmp(char *s1, uint len1, char *s2, uint len2);
ZEND_API int zend_binary_strncasecmp(char *s1, uint len1, char *s2, uint len2, uint length);

ZEND_API void zendi_smart_strcmp(zval *result, zval *s1, zval *s2);
ZEND_API void zend_compare_symbol_tables(zval *result, HashTable *ht1, HashTable *ht2 TSRMLS_DC);
ZEND_API void zend_compare_arrays(zval *result, zval *a1, zval *a2 TSRMLS_DC);
ZEND_API void zend_compare_objects(zval *result, zval *o1, zval *o2 TSRMLS_DC);

ZEND_API int zend_atoi(const char *str, int str_len);

#define convert_to_ex_master(ppzv, lower_type, upper_type)	\
	if ((*ppzv)->type!=IS_##upper_type) {					\
		if (!(*ppzv)->is_ref) {								\
			SEPARATE_ZVAL(ppzv);							\
		}													\
		convert_to_##lower_type(*ppzv);						\
	}

#define convert_to_writable_ex_master(ppzv, lower_type, upper_type)	\
	if ((*ppzv)->type!=IS_##upper_type) {							\
		SEPARATE_ZVAL(ppzv);										\
		convert_to_##lower_type(*ppzv);								\
	}


#define convert_to_boolean_ex(ppzv)	convert_to_ex_master(ppzv, boolean, BOOL)
#define convert_to_long_ex(ppzv)	convert_to_ex_master(ppzv, long, LONG)
#define convert_to_double_ex(ppzv)	convert_to_ex_master(ppzv, double, DOUBLE)
#define convert_to_string_ex(ppzv)	convert_to_ex_master(ppzv, string, STRING)
#define convert_to_array_ex(ppzv)	convert_to_ex_master(ppzv, array, ARRAY)
#define convert_to_object_ex(ppzv)	convert_to_ex_master(ppzv, object, OBJECT)
#define convert_to_null_ex(ppzv)	convert_to_ex_master(ppzv, null, NULL)

#define convert_to_writable_boolean_ex(ppzv)	convert_to_writable_ex_master(ppzv, boolean, BOOL)
#define convert_to_writable_long_ex(ppzv)		convert_to_writable_ex_master(ppzv, long, LONG)
#define convert_to_writable_double_ex(ppzv)		convert_to_writable_ex_master(ppzv, double, DOUBLE)
#define convert_to_writable_string_ex(ppzv)		convert_to_writable_ex_master(ppzv, string, STRING)
#define convert_to_writable_array_ex(ppzv)		convert_to_writable_ex_master(ppzv, array, ARRAY)
#define convert_to_writable_object_ex(ppzv)		convert_to_writable_ex_master(ppzv, object, OBJECT)
#define convert_to_writable_null_ex(ppzv)		convert_to_writable_ex_master(ppzv, null, NULL)


#define convert_scalar_to_number_ex(ppzv)							\
	if ((*ppzv)->type!=IS_LONG && (*ppzv)->type!=IS_DOUBLE) {		\
		if (!(*ppzv)->is_ref) {										\
			SEPARATE_ZVAL(ppzv);									\
		}															\
		convert_scalar_to_number(*ppzv TSRMLS_CC);					\
	}


#define Z_LVAL(zval)		(zval).value.lval
#define Z_BVAL(zval)		((zend_bool)(zval).value.lval)
#define Z_DVAL(zval)		(zval).value.dval
#define Z_STRVAL(zval)		(zval).value.str.val
#define Z_STRLEN(zval)		(zval).value.str.len
#define Z_ARRVAL(zval)		(zval).value.ht
#define Z_OBJ_HANDLE(zval)  (zval).value.obj.handle
#define Z_OBJ_HT(zval)      (zval).value.obj.handlers
#define Z_OBJ(zval)			zend_objects_get_address(&(zval))
#define Z_OBJCE(zval)		zend_get_class_entry(&(zval))
#define Z_OBJPROP(zval)		Z_OBJ(zval)->properties
#define Z_RESVAL(zval)		(zval).value.lval

#define Z_LVAL_P(zval_p)		Z_LVAL(*zval_p)
#define Z_BVAL_P(zval_p)		Z_BVAL(*zval_p)
#define Z_DVAL_P(zval_p)		Z_DVAL(*zval_p)
#define Z_STRVAL_P(zval_p)		Z_STRVAL(*zval_p)
#define Z_STRLEN_P(zval_p)		Z_STRLEN(*zval_p)
#define Z_ARRVAL_P(zval_p)		Z_ARRVAL(*zval_p)
#define Z_OBJ_P(zval_p)			Z_OBJ(*zval_p)
#define Z_OBJPROP_P(zval_p)		Z_OBJPROP(*zval_p)
#define Z_OBJCE_P(zval_p)		Z_OBJCE(*zval_p)
#define Z_RESVAL_P(zval_p)		Z_RESVAL(*zval_p)
#define Z_OBJ_HANDLE_P(zval_p)  Z_OBJ_HANDLE(*zval_p)
#define Z_OBJ_HT_P(zval_p)      Z_OBJ_HT(*zval_p)

#define Z_LVAL_PP(zval_pp)		Z_LVAL(**zval_pp)
#define Z_BVAL_PP(zval_pp)		Z_BVAL(**zval_pp)
#define Z_DVAL_PP(zval_pp)		Z_DVAL(**zval_pp)
#define Z_STRVAL_PP(zval_pp)	Z_STRVAL(**zval_pp)
#define Z_STRLEN_PP(zval_pp)	Z_STRLEN(**zval_pp)
#define Z_ARRVAL_PP(zval_pp)	Z_ARRVAL(**zval_pp)
#define Z_OBJ_PP(zval_pp)		Z_OBJ(**zval_pp)
#define Z_OBJPROP_PP(zval_pp)	Z_OBJPROP(**zval_pp)
#define Z_OBJCE_PP(zval_pp)		Z_OBJCE(**zval_pp)
#define Z_RESVAL_PP(zval_pp)	Z_RESVAL(**zval_pp)
#define Z_OBJ_HANDLE_PP(zval_p) Z_OBJ_HANDLE(**zval_p)
#define Z_OBJ_HT_PP(zval_p)     Z_OBJ_HT(**zval_p)

#define Z_TYPE(zval)		(zval).type
#define Z_TYPE_P(zval_p)	Z_TYPE(*zval_p)
#define Z_TYPE_PP(zval_pp)	Z_TYPE(**zval_pp)

#endif
