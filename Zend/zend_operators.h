/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski                  |
   +----------------------------------------------------------------------+
   | This source file is subject to version 0.91 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/0_91.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


#ifndef _OPERATORS_H
#define _OPERATORS_H

#define MAX_LENGTH_OF_LONG 18
#define MAX_LENGTH_OF_DOUBLE 32

ZEND_API int add_function(zval *result, zval *op1, zval *op2);
ZEND_API int sub_function(zval *result, zval *op1, zval *op2);
ZEND_API int mul_function(zval *result, zval *op1, zval *op2);
ZEND_API int div_function(zval *result, zval *op1, zval *op2);
ZEND_API int mod_function(zval *result, zval *op1, zval *op2);
ZEND_API int boolean_or_function(zval *result, zval *op1, zval *op2);
ZEND_API int boolean_xor_function(zval *result, zval *op1, zval *op2);
ZEND_API int boolean_and_function(zval *result, zval *op1, zval *op2);
ZEND_API int boolean_not_function(zval *result, zval *op1);
ZEND_API int bitwise_not_function(zval *result, zval *op1);
ZEND_API int bitwise_or_function(zval *result, zval *op1, zval *op2);
ZEND_API int bitwise_and_function(zval *result, zval *op1, zval *op2);
ZEND_API int bitwise_xor_function(zval *result, zval *op1, zval *op2);
ZEND_API int shift_left_function(zval *result, zval *op1, zval *op2);
ZEND_API int shift_right_function(zval *result, zval *op1, zval *op2);
ZEND_API int concat_function(zval *result, zval *op1, zval *op2);

ZEND_API int is_equal_function(zval *result, zval *op1, zval *op2);
ZEND_API int is_identical_function(zval *result, zval *op1, zval *op2);
ZEND_API int is_not_equal_function(zval *result, zval *op1, zval *op2);
ZEND_API int is_smaller_function(zval *result, zval *op1, zval *op2);
ZEND_API int is_smaller_or_equal_function(zval *result, zval *op1, zval *op2);
ZEND_API inline int is_numeric_string(char *str, int length, long *lval, double *dval);

ZEND_API int increment_function(zval *op1);
ZEND_API int decrement_function(zval *op2);

BEGIN_EXTERN_C()
ZEND_API void convert_scalar_to_number(zval *op);
ZEND_API void convert_to_string(zval *op);
ZEND_API void convert_to_long(zval *op);
ZEND_API void convert_to_long_base(zval *op, int base);
ZEND_API void convert_to_unset(zval *op);
ZEND_API void convert_to_boolean(zval *op);
ZEND_API void convert_to_array(zval *op);
ZEND_API void convert_to_object(zval *op);
ZEND_API int add_char_to_string(zval *result, zval *op1, zval *op2);
ZEND_API int add_string_to_string(zval *result, zval *op1, zval *op2);
ZEND_API void convert_to_double(zval *op);
END_EXTERN_C()

ZEND_API int zval_is_true(zval *op);
ZEND_API int compare_function(zval *result, zval *op1, zval *op2);

ZEND_API void zend_str_tolower(char *str, unsigned int length);
ZEND_API int zend_binary_zval_strcmp(zval *s1, zval *s2);
ZEND_API int zend_binary_zval_strcasecmp(zval *s1, zval *s2);
ZEND_API int zend_binary_strcmp(char *s1, uint len1, char *s2, uint len2);
ZEND_API int zend_binary_strcasecmp(char *s1, uint len1, char *s2, uint len2);

ZEND_API void zendi_smart_strcmp(zval *result, zval *s1, zval *s2);

#define convert_to_long_ex(ppzv)							\
	if ((*ppzv)->type!=IS_LONG) {							\
		if (!(*ppzv)->is_ref) {							\
			SEPARATE_ZVAL(ppzv);							\
		}													\
		convert_to_long(*ppzv);								\
	}

#define convert_to_double_ex(ppzv)							\
	if ((*ppzv)->type!=IS_DOUBLE) {							\
		if (!(*ppzv)->is_ref) {							\
			SEPARATE_ZVAL(ppzv);							\
		}													\
		convert_to_double(*ppzv);							\
	}

#define convert_to_string_ex(ppzv)							\
	if ((*ppzv)->type!=IS_STRING) {							\
		if (!(*ppzv)->is_ref) {							\
			SEPARATE_ZVAL(ppzv);							\
		}													\
		convert_to_string(*ppzv);							\
	}

#define convert_to_array_ex(ppzv)							\
	if ((*ppzv)->type!=IS_ARRAY) {							\
		if (!(*ppzv)->is_ref) {							\
			SEPARATE_ZVAL(ppzv);							\
		}													\
		convert_to_array(*ppzv);							\
	}

#define convert_to_object_ex(ppzv)							\
	if ((*ppzv)->type!=IS_OBJECT) {							\
		if (!(*ppzv)->is_ref) {							\
			SEPARATE_ZVAL(ppzv);							\
		}													\
		convert_to_object(*ppzv);							\
	}

#define convert_to_boolean_ex(ppzv)							\
	if ((*ppzv)->type!=IS_BOOL) {							\
		if (!(*ppzv)->is_ref) {							\
			SEPARATE_ZVAL(ppzv);							\
		}													\
		convert_to_boolean(*ppzv);							\
	}

#define convert_scalar_to_number_ex(ppzv)							\
	if ((*ppzv)->type!=IS_LONG && (*ppzv)->type!=IS_DOUBLE) {		\
		if (!(*ppzv)->is_ref) {										\
			SEPARATE_ZVAL(ppzv);									\
		}															\
		convert_scalar_to_number(*ppzv);							\
	}

#endif
