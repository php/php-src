/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski                  |
   +----------------------------------------------------------------------+
   | This source file is subject to the Zend license, that is bundled     |
   | with this package in the file LICENSE.  If you did not receive a     |
   | copy of the Zend license, please mail us at zend@zend.com so we can  |
   | send you a copy immediately.                                         |
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
ZEND_API int is_not_equal_function(zval *result, zval *op1, zval *op2);
ZEND_API int is_smaller_function(zval *result, zval *op1, zval *op2);
ZEND_API int is_smaller_or_equal_function(zval *result, zval *op1, zval *op2);

ZEND_API int increment_function(zval *op1);
ZEND_API int decrement_function(zval *op2);

BEGIN_EXTERN_C()
ZEND_API void convert_scalar_to_number(zval *op);
ZEND_API void convert_to_string(zval *op);
ZEND_API void convert_to_long(zval *op);
ZEND_API void convert_to_long_base(zval *op, int base);
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
ZEND_API int zend_binary_strcmp(zval *s1, zval *s2);
ZEND_API int zend_binary_strcasecmp(zval *s1, zval *s2);
ZEND_API void zendi_smart_strcmp(zval *result, zval *s1, zval *s2);

#endif
