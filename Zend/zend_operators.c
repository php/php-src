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

#include "zend.h"
#include "zend_operators.h"
#include "zend_variables.h"
#include "zend_globals.h"
#include "zend_list.h"
#include "zend_fast_cache.h"
#include "zend_API.h"

#if 0&&WITH_BCMATH
#include "ext/bcmath/number.h"
#endif

ZEND_API int zend_atoi(const char *str, int str_len)
{
	int retval;

	if (!str_len) {
		str_len = strlen(str);
	}
	retval = strtol(str, NULL, 0);
	if (str_len>0) {
		switch (str[str_len-1]) {
			case 'k':
			case 'K':
				retval *= 1024;
				break;
			case 'm':
			case 'M':
				retval *= 1048576;
				break;
		}
	}
	return retval;
}


ZEND_API double zend_string_to_double(const char *number, zend_uint length)
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


ZEND_API void convert_scalar_to_number(zval *op TSRMLS_DC)
{
	switch (op->type) {
		case IS_STRING:
			{
				char *strval;

				strval = op->value.str.val;
				switch ((op->type=is_numeric_string(strval, op->value.str.len, &op->value.lval, &op->value.dval, 1))) {
					case IS_DOUBLE:
					case IS_LONG:
						break;
#if 0 && WITH_BCMATH
					case FLAG_IS_BC:
						op->type = IS_DOUBLE; /* may have lost significant digits */
						break;
#endif
					default:
						op->value.lval = strtol(op->value.str.val, NULL, 10);
						op->type = IS_LONG;
						break;
				}
				STR_FREE(strval);
				break;
			}
		case IS_BOOL:
			op->type = IS_LONG;
			break;
		case IS_RESOURCE:
			zend_list_delete(op->value.lval);
			op->type = IS_LONG;
			break;
		case IS_NULL:
			op->type = IS_LONG;
			op->value.lval = 0;
			break;
	}
}

#define zendi_convert_scalar_to_number(op, holder, result)			\
	if (op==result) {												\
		convert_scalar_to_number(op TSRMLS_CC);						\
	} else {														\
		switch ((op)->type) {										\
			case IS_STRING:											\
				{													\
					switch (((holder).type=is_numeric_string((op)->value.str.val, (op)->value.str.len, &(holder).value.lval, &(holder).value.dval, 1))) {	\
						case IS_DOUBLE:															\
						case IS_LONG:															\
							break;																\
						case FLAG_IS_BC:														\
							(holder).type = IS_DOUBLE; /* may have lost significant digits */	\
							break;																\
						default:																\
							(holder).value.lval = strtol((op)->value.str.val, NULL, 10);		\
							(holder).type = IS_LONG;						\
							break;											\
					}														\
					(op) = &(holder);										\
					break;													\
				}															\
			case IS_BOOL:													\
			case IS_RESOURCE:												\
				(holder).value.lval = (op)->value.lval;						\
				(holder).type = IS_LONG;									\
				(op) = &(holder);											\
				break;														\
			case IS_NULL:													\
				(holder).value.lval = 0;									\
				(holder).type = IS_LONG;									\
				(op) = &(holder);											\
				break;														\
		}																	\
	}


#define DVAL_TO_LVAL(d, l) (l) = (d) > LONG_MAX ? (unsigned long) (d) : (long) (d)

#define zendi_convert_to_long(op, holder, result)					\
	if (op==result) {												\
		convert_to_long(op);										\
	} else if ((op)->type != IS_LONG) {								\
		switch ((op)->type) {										\
			case IS_NULL:											\
				(holder).value.lval = 0;							\
				break;												\
			case IS_DOUBLE:											\
				DVAL_TO_LVAL((op)->value.dval, (holder).value.lval);	\
				break;												\
			case IS_STRING:											\
				(holder).value.lval = strtol((op)->value.str.val, NULL, 10);					\
				break;												\
			case IS_ARRAY:											\
				(holder).value.lval = (zend_hash_num_elements((op)->value.ht)?1:0);				\
				break;												\
			case IS_OBJECT:											\
				(holder).value.lval = 1; /* TBI!! */				\
				break;												\
			case IS_BOOL:											\
			case IS_RESOURCE:										\
				(holder).value.lval = (op)->value.lval;				\
				break;												\
			default:												\
				zend_error(E_WARNING, "Cannot convert to ordinal value");						\
				(holder).value.lval = 0;							\
				break;												\
		}															\
		(holder).type = IS_LONG;									\
		(op) = &(holder);											\
	}


#define zendi_convert_to_boolean(op, holder, result)				\
	if (op==result) {												\
		convert_to_boolean(op);										\
	} else if ((op)->type != IS_BOOL) {								\
		switch ((op)->type) {										\
			case IS_NULL:											\
				(holder).value.lval = 0;							\
				break;												\
			case IS_RESOURCE:										\
			case IS_LONG:											\
				(holder).value.lval = ((op)->value.lval ? 1 : 0);	\
				break;												\
			case IS_DOUBLE:											\
				(holder).value.lval = ((op)->value.dval ? 1 : 0);	\
				break;												\
			case IS_STRING:											\
				if ((op)->value.str.len == 0						\
					|| ((op)->value.str.len==1 && (op)->value.str.val[0]=='0')) {	\
					(holder).value.lval = 0;						\
				} else {											\
					(holder).value.lval = 1;						\
				}													\
				break;												\
			case IS_ARRAY:											\
				(holder).value.lval = (zend_hash_num_elements((op)->value.ht)?1:0);	\
				break;												\
			case IS_OBJECT:											\
				(holder).value.lval = 1; /* TBI!! */				\
				break;												\
			default:												\
				(holder).value.lval = 0;							\
				break;												\
		}															\
		(holder).type = IS_BOOL;									\
		(op) = &(holder);											\
	}


ZEND_API void convert_to_long(zval *op)
{
	convert_to_long_base(op, 10);
}


ZEND_API void convert_to_long_base(zval *op, int base)
{
	char *strval;
	long tmp;

	switch (op->type) {
		case IS_NULL:
			op->value.lval = 0;
			break;
		case IS_RESOURCE: {
				TSRMLS_FETCH();

				zend_list_delete(op->value.lval);
			}
			/* break missing intentionally */
		case IS_BOOL:
		case IS_LONG:
			break;
		case IS_DOUBLE:
			DVAL_TO_LVAL(op->value.dval, op->value.lval);
			break;
		case IS_STRING:
			strval = op->value.str.val;
			op->value.lval = strtol(strval, NULL, base);
			STR_FREE(strval);
			break;
		case IS_ARRAY:
			tmp = (zend_hash_num_elements(op->value.ht)?1:0);
			zval_dtor(op);
			op->value.lval = tmp;
			break;
		case IS_OBJECT:
			zval_dtor(op);
			op->value.lval = 1; /* TBI!! */
			break;
		default:
			zend_error(E_WARNING, "Cannot convert to ordinal value");
			zval_dtor(op);
			op->value.lval = 0;
			break;
	}

	op->type = IS_LONG;
}


ZEND_API void convert_to_double(zval *op)
{
	char *strval;
	double tmp;

	switch (op->type) {
		case IS_NULL:
			op->value.dval = 0.0;
			break;
		case IS_RESOURCE: {
				TSRMLS_FETCH();

				zend_list_delete(op->value.lval);
			}
			/* break missing intentionally */
		case IS_BOOL:
		case IS_LONG:
			op->value.dval = (double) op->value.lval;
			break;
		case IS_DOUBLE:
			break;
		case IS_STRING:
			strval = op->value.str.val;

			op->value.dval = strtod(strval, NULL);
			STR_FREE(strval);
			break;
		case IS_ARRAY:
			tmp = (zend_hash_num_elements(op->value.ht)?1:0);
			zval_dtor(op);
			op->value.dval = tmp;
			break;
		case IS_OBJECT:
			zval_dtor(op);
			op->value.dval = 1; /* TBI!! */
			break;			
		default:
			zend_error(E_WARNING, "Cannot convert to real value (type=%d)", op->type);
			zval_dtor(op);
			op->value.dval = 0;
			break;
	}
	op->type = IS_DOUBLE;
}


ZEND_API void convert_to_null(zval *op)
{
	zval_dtor(op);
	op->type = IS_NULL;
}


ZEND_API void convert_to_boolean(zval *op)
{
	char *strval;
	int tmp;

	switch (op->type) {
		case IS_BOOL:
			break;
		case IS_NULL:
			op->value.lval = 0;
			break;
		case IS_RESOURCE: {
				TSRMLS_FETCH();

				zend_list_delete(op->value.lval);
			}
			/* break missing intentionally */
		case IS_LONG:
			op->value.lval = (op->value.lval ? 1 : 0);
			break;
		case IS_DOUBLE:
			op->value.lval = (op->value.dval ? 1 : 0);
			break;
		case IS_STRING:
			strval = op->value.str.val;

			if (op->value.str.len == 0
				|| (op->value.str.len==1 && op->value.str.val[0]=='0')) {
				op->value.lval = 0;
			} else {
				op->value.lval = 1;
			}
			STR_FREE(strval);
			break;
		case IS_ARRAY:
			tmp = (zend_hash_num_elements(op->value.ht)?1:0);
			zval_dtor(op);
			op->value.lval = tmp;
			break;
		case IS_OBJECT:
			zval_dtor(op);
			op->value.lval = 1; /* TBI!! */
			break;
		default:
			zval_dtor(op);
			op->value.lval = 0;
			break;
	}
	op->type = IS_BOOL;
}


ZEND_API void _convert_to_string(zval *op ZEND_FILE_LINE_DC)
{
	long lval;
	double dval;
	TSRMLS_FETCH();

	switch (op->type) {
		case IS_NULL:
			op->value.str.val = empty_string;
			op->value.str.len = 0;
			break;
		case IS_STRING:
			break;
		case IS_BOOL:
			if (op->value.lval) {
				op->value.str.val = estrndup_rel("1", 1);
				op->value.str.len = 1;
			} else {
				op->value.str.val = empty_string;
				op->value.str.len = 0;
			}
			break;
		case IS_RESOURCE: {
			long tmp = op->value.lval;

			zend_list_delete(op->value.lval);
			op->value.str.val = (char *) emalloc(sizeof("Resource id #")-1 + MAX_LENGTH_OF_LONG);
			op->value.str.len = sprintf(op->value.str.val, "Resource id #%ld", tmp);
			break;
		}
		case IS_LONG:
			lval = op->value.lval;

			op->value.str.val = (char *) emalloc_rel(MAX_LENGTH_OF_LONG + 1);
			op->value.str.len = zend_sprintf(op->value.str.val, "%ld", lval);  /* SAFE */
			break;
		case IS_DOUBLE: {
			dval = op->value.dval;
			op->value.str.val = (char *) emalloc_rel(MAX_LENGTH_OF_DOUBLE + EG(precision) + 1);
			op->value.str.len = zend_sprintf(op->value.str.val, "%.*G", (int) EG(precision), dval);  /* SAFE */
			/* %G already handles removing trailing zeros from the fractional part, yay */
			break;
		}
		case IS_ARRAY:
			zval_dtor(op);
			op->value.str.val = estrndup_rel("Array", sizeof("Array")-1);
			op->value.str.len = sizeof("Array")-1;
			zend_error(E_NOTICE, "Array to string conversion");
			break;
		case IS_OBJECT:
			zval_dtor(op);
			op->value.str.val = estrndup_rel("Object", sizeof("Object")-1);
			op->value.str.len = sizeof("Object")-1;
			zend_error(E_NOTICE, "Object to string conversion");
			break;
		default:
			zval_dtor(op);
			ZVAL_BOOL(op, 0);
			break;
	}
	op->type = IS_STRING;
}


static void convert_scalar_to_array(zval *op, int type)
{
	zval *entry;
	
	ALLOC_ZVAL(entry);
	*entry = *op;
	INIT_PZVAL(entry);
	
	switch (type) {
		case IS_ARRAY:
			ALLOC_HASHTABLE(op->value.ht);
			zend_hash_init(op->value.ht, 0, NULL, ZVAL_PTR_DTOR, 0);
			zend_hash_index_update(op->value.ht, 0, (void *) &entry, sizeof(zval *), NULL);
			op->type = IS_ARRAY;
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
}


ZEND_API void convert_to_array(zval *op)
{
	TSRMLS_FETCH();

	switch(op->type) {
		case IS_ARRAY:
			return;
			break;
/* OBJECTS_OPTIMIZE */
		case IS_OBJECT:
			{
				zval *tmp;
				HashTable *ht;

				ALLOC_HASHTABLE(ht);
				zend_hash_init(ht, 0, NULL, ZVAL_PTR_DTOR, 0);
				if (Z_OBJ_HT_P(op)->get_properties) {
					zend_hash_copy(ht, Z_OBJ_HT_P(op)->get_properties(op TSRMLS_CC), (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
				}
				zval_dtor(op);
				op->type = IS_ARRAY;
				op->value.ht = ht;
			}
			return;
		case IS_NULL:
			ALLOC_HASHTABLE(op->value.ht);
			zend_hash_init(op->value.ht, 0, NULL, ZVAL_PTR_DTOR, 0);
			op->type = IS_ARRAY;
			break;
		default:
			convert_scalar_to_array(op, IS_ARRAY);
			break;
	}
}


ZEND_API void convert_to_object(zval *op)
{
	switch(op->type) {
		case IS_ARRAY:
			{
				/* OBJECTS_OPTIMIZE */
				TSRMLS_FETCH();

				object_and_properties_init(op, zend_standard_class_def, op->value.ht);
				return;
				break;
			}
		case IS_OBJECT:
			return;
		case IS_NULL:
			{
				/* OBJECTS_OPTIMIZE */
				TSRMLS_FETCH();

				object_init(op);
				break;
			}
		default:
			convert_scalar_to_array(op, IS_OBJECT);
			break;
	}
}

ZEND_API void multi_convert_to_long_ex(int argc, ...)
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

ZEND_API void multi_convert_to_double_ex(int argc, ...)
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

ZEND_API void multi_convert_to_string_ex(int argc, ...)
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

ZEND_API int add_function(zval *result, zval *op1, zval *op2 TSRMLS_DC)
{
	zval op1_copy, op2_copy;

	if (op1->type == IS_ARRAY && op2->type == IS_ARRAY) {
		zval *tmp;

		if ((result == op1) && (result == op2)) {
			/* $a += $a */
			return SUCCESS;
		}
		if (result != op1) {
			*result = *op1;
			zval_copy_ctor(result);
		}
		zend_hash_merge(result->value.ht, op2->value.ht, (void (*)(void *pData)) zval_add_ref, (void *) &tmp, sizeof(zval *), 0);
		return SUCCESS;
	}
	zendi_convert_scalar_to_number(op1, op1_copy, result);
	zendi_convert_scalar_to_number(op2, op2_copy, result);


	if (op1->type == IS_LONG && op2->type == IS_LONG) {
		double dval = (double) op1->value.lval + (double) op2->value.lval;

		if ((dval > (double) LONG_MAX) || (dval < (double) LONG_MIN)) {
			result->value.dval = dval;
			result->type = IS_DOUBLE;
		} else {
			result->value.lval = op1->value.lval + op2->value.lval;
			result->type = IS_LONG;
		}
		return SUCCESS;
	}
	if ((op1->type == IS_DOUBLE && op2->type == IS_LONG)
		|| (op1->type == IS_LONG && op2->type == IS_DOUBLE)) {
		result->value.dval = (op1->type == IS_LONG ?
						 (((double) op1->value.lval) + op2->value.dval) :
						 (op1->value.dval + ((double) op2->value.lval)));
		result->type = IS_DOUBLE;
		return SUCCESS;
	}
	if (op1->type == IS_DOUBLE && op2->type == IS_DOUBLE) {
		result->type = IS_DOUBLE;
		result->value.dval = op1->value.dval + op2->value.dval;
		return SUCCESS;
	}
	zend_error(E_ERROR, "Unsupported operand types");
	return FAILURE;				/* unknown datatype */
}


ZEND_API int sub_function(zval *result, zval *op1, zval *op2 TSRMLS_DC)
{
	zval op1_copy, op2_copy;
	
	zendi_convert_scalar_to_number(op1, op1_copy, result);
	zendi_convert_scalar_to_number(op2, op2_copy, result);

	if (op1->type == IS_LONG && op2->type == IS_LONG) {
		double dval = (double) op1->value.lval - (double) op2->value.lval;

		if ((dval < (double) LONG_MIN) || (dval > (double) LONG_MAX)) {
			result->value.dval = dval;
			result->type = IS_DOUBLE;
		} else {
			result->value.lval = op1->value.lval - op2->value.lval;
			result->type = IS_LONG;
		}
		return SUCCESS;
	}
	if ((op1->type == IS_DOUBLE && op2->type == IS_LONG)
		|| (op1->type == IS_LONG && op2->type == IS_DOUBLE)) {
		result->value.dval = (op1->type == IS_LONG ?
						 (((double) op1->value.lval) - op2->value.dval) :
						 (op1->value.dval - ((double) op2->value.lval)));
		result->type = IS_DOUBLE;
		return SUCCESS;
	}
	if (op1->type == IS_DOUBLE && op2->type == IS_DOUBLE) {
		result->type = IS_DOUBLE;
		result->value.dval = op1->value.dval - op2->value.dval;
		return SUCCESS;
	}
	zend_error(E_ERROR, "Unsupported operand types");
	return FAILURE;				/* unknown datatype */
}


ZEND_API int mul_function(zval *result, zval *op1, zval *op2 TSRMLS_DC)
{
	zval op1_copy, op2_copy;
	
	zendi_convert_scalar_to_number(op1, op1_copy, result);
	zendi_convert_scalar_to_number(op2, op2_copy, result);

	if (op1->type == IS_LONG && op2->type == IS_LONG) {
		double dval = (double) op1->value.lval * (double) op2->value.lval;

		if ((dval > (double) LONG_MAX) || (dval < (double) LONG_MIN)) {
			result->value.dval = dval;
			result->type = IS_DOUBLE;
		} else {
			result->value.lval = op1->value.lval * op2->value.lval;
			result->type = IS_LONG;
		}
		return SUCCESS;
	}
	if ((op1->type == IS_DOUBLE && op2->type == IS_LONG)
		|| (op1->type == IS_LONG && op2->type == IS_DOUBLE)) {
		result->value.dval = (op1->type == IS_LONG ?
						 (((double) op1->value.lval) * op2->value.dval) :
						 (op1->value.dval * ((double) op2->value.lval)));
		result->type = IS_DOUBLE;
		return SUCCESS;
	}
	if (op1->type == IS_DOUBLE && op2->type == IS_DOUBLE) {
		result->type = IS_DOUBLE;
		result->value.dval = op1->value.dval * op2->value.dval;
		return SUCCESS;
	}
	zend_error(E_ERROR, "Unsupported operand types");
	return FAILURE;				/* unknown datatype */
}

ZEND_API int div_function(zval *result, zval *op1, zval *op2 TSRMLS_DC)
{
	zval op1_copy, op2_copy;
	
	zendi_convert_scalar_to_number(op1, op1_copy, result);
	zendi_convert_scalar_to_number(op2, op2_copy, result);

	if ((op2->type == IS_LONG && op2->value.lval == 0) || (op2->type == IS_DOUBLE && op2->value.dval == 0.0)) {
		zend_error(E_WARNING, "Division by zero");
		ZVAL_BOOL(result, 0);
		return FAILURE;			/* division by zero */
	}
	if (op1->type == IS_LONG && op2->type == IS_LONG) {
		if (op1->value.lval % op2->value.lval == 0) { /* integer */
			result->type = IS_LONG;
			result->value.lval = op1->value.lval / op2->value.lval;
		} else {
			result->type = IS_DOUBLE;
			result->value.dval = ((double) op1->value.lval) / op2->value.lval;
		}
		return SUCCESS;
	}
	if ((op1->type == IS_DOUBLE && op2->type == IS_LONG)
		|| (op1->type == IS_LONG && op2->type == IS_DOUBLE)) {
		result->value.dval = (op1->type == IS_LONG ?
						 (((double) op1->value.lval) / op2->value.dval) :
						 (op1->value.dval / ((double) op2->value.lval)));
		result->type = IS_DOUBLE;
		return SUCCESS;
	}
	if (op1->type == IS_DOUBLE && op2->type == IS_DOUBLE) {
		result->type = IS_DOUBLE;
		result->value.dval = op1->value.dval / op2->value.dval;
		return SUCCESS;
	}
	zend_error(E_ERROR, "Unsupported operand types");
	return FAILURE;				/* unknown datatype */
}


ZEND_API int mod_function(zval *result, zval *op1, zval *op2 TSRMLS_DC)
{
	zval op1_copy, op2_copy;
	
	zendi_convert_to_long(op1, op1_copy, result);
	zendi_convert_to_long(op2, op2_copy, result);

	if (op2->value.lval == 0) {
		ZVAL_BOOL(result, 0);
		return FAILURE;			/* modulus by zero */
	}

	result->type = IS_LONG;
	result->value.lval = op1->value.lval % op2->value.lval;
	return SUCCESS;
}



ZEND_API int boolean_xor_function(zval *result, zval *op1, zval *op2 TSRMLS_DC)
{
	zval op1_copy, op2_copy;
	
	result->type = IS_BOOL;

	zendi_convert_to_boolean(op1, op1_copy, result);
	zendi_convert_to_boolean(op2, op2_copy, result);
	result->value.lval = op1->value.lval ^ op2->value.lval;
	return SUCCESS;
}


ZEND_API int boolean_not_function(zval *result, zval *op1 TSRMLS_DC)
{
	zval op1_copy;
	
	zendi_convert_to_boolean(op1, op1_copy, result);

	result->type = IS_BOOL;
	result->value.lval = !op1->value.lval;
	return SUCCESS;
}


ZEND_API int bitwise_not_function(zval *result, zval *op1 TSRMLS_DC)
{
	zval op1_copy = *op1;
	
	op1 = &op1_copy;
	
	if (op1->type == IS_DOUBLE) {
		op1->value.lval = (long) op1->value.dval;
		op1->type = IS_LONG;
	}
	if (op1->type == IS_LONG) {
		result->value.lval = ~op1->value.lval;
		result->type = IS_LONG;
		return SUCCESS;
	}
	if (op1->type == IS_STRING) {
		zend_uint i;

		result->type = IS_STRING;
		result->value.str.val = estrndup(op1->value.str.val, op1->value.str.len);
		result->value.str.len = op1->value.str.len;
		for (i = 0; i < op1->value.str.len; i++) {
			result->value.str.val[i] = ~op1->value.str.val[i];
		}
		return SUCCESS;
	}
	zend_error(E_ERROR, "Unsupported operand types");
	return FAILURE;				/* unknown datatype */
}


ZEND_API int bitwise_or_function(zval *result, zval *op1, zval *op2 TSRMLS_DC)
{
	zval op1_copy, op2_copy;
	
	if (op1->type == IS_STRING && op2->type == IS_STRING) {
		zval *longer, *shorter;
		char *result_str;
		zend_uint i, result_len;

		if (op1->value.str.len >= op2->value.str.len) {
			longer = op1;
			shorter = op2;
		} else {
			longer = op2;
			shorter = op1;
		}

		result->type = IS_STRING;
		result_len = longer->value.str.len;
		result_str = estrndup(longer->value.str.val, longer->value.str.len);
		for (i = 0; i < shorter->value.str.len; i++) {
			result_str[i] |= shorter->value.str.val[i];
		}
		if (result==op1) {
			STR_FREE(result->value.str.val);
		}
		result->value.str.val = result_str;
		result->value.str.len = result_len;
		return SUCCESS;
	}
	zendi_convert_to_long(op1, op1_copy, result);
	zendi_convert_to_long(op2, op2_copy, result);

	result->type = IS_LONG;
	result->value.lval = op1->value.lval | op2->value.lval;
	return SUCCESS;
}


ZEND_API int bitwise_and_function(zval *result, zval *op1, zval *op2 TSRMLS_DC)
{
	zval op1_copy, op2_copy;
	
	if (op1->type == IS_STRING && op2->type == IS_STRING) {
		zval *longer, *shorter;
		char *result_str;
		zend_uint i, result_len;

		if (op1->value.str.len >= op2->value.str.len) {
			longer = op1;
			shorter = op2;
		} else {
			longer = op2;
			shorter = op1;
		}

		result->type = IS_STRING;
		result_len = shorter->value.str.len;
		result_str = estrndup(shorter->value.str.val, shorter->value.str.len);
		for (i = 0; i < shorter->value.str.len; i++) {
			result_str[i] &= longer->value.str.val[i];
		}
		if (result==op1) {
			STR_FREE(result->value.str.val);
		}
		result->value.str.val = result_str;
		result->value.str.len = result_len;
		return SUCCESS;
	}
	

	zendi_convert_to_long(op1, op1_copy, result);
	zendi_convert_to_long(op2, op2_copy, result);

	result->type = IS_LONG;
	result->value.lval = op1->value.lval & op2->value.lval;
	return SUCCESS;
}


ZEND_API int bitwise_xor_function(zval *result, zval *op1, zval *op2 TSRMLS_DC)
{
	zval op1_copy, op2_copy;
	
	if (op1->type == IS_STRING && op2->type == IS_STRING) {
		zval *longer, *shorter;
		char *result_str;
		zend_uint i, result_len;

		if (op1->value.str.len >= op2->value.str.len) {
			longer = op1;
			shorter = op2;
		} else {
			longer = op2;
			shorter = op1;
		}

		result->type = IS_STRING;
		result_len = shorter->value.str.len;
		result_str = estrndup(shorter->value.str.val, shorter->value.str.len);
		for (i = 0; i < shorter->value.str.len; i++) {
			result_str[i] ^= longer->value.str.val[i];
		}
		if (result==op1) {
			STR_FREE(result->value.str.val);
		}
		result->value.str.val = result_str;
		result->value.str.len = result_len;
		return SUCCESS;
	}

	zendi_convert_to_long(op1, op1_copy, result);
	zendi_convert_to_long(op2, op2_copy, result);	

	result->type = IS_LONG;
	result->value.lval = op1->value.lval ^ op2->value.lval;
	return SUCCESS;
}


ZEND_API int shift_left_function(zval *result, zval *op1, zval *op2 TSRMLS_DC)
{
	zval op1_copy, op2_copy;
	
	zendi_convert_to_long(op1, op1_copy, result);
	zendi_convert_to_long(op2, op2_copy, result);
	result->value.lval = op1->value.lval << op2->value.lval;
	result->type = IS_LONG;
	return SUCCESS;
}


ZEND_API int shift_right_function(zval *result, zval *op1, zval *op2 TSRMLS_DC)
{
	zval op1_copy, op2_copy;
	
	zendi_convert_to_long(op1, op1_copy, result);
	zendi_convert_to_long(op2, op2_copy, result);
	result->value.lval = op1->value.lval >> op2->value.lval;
	result->type = IS_LONG;
	return SUCCESS;
}



/* must support result==op1 */
ZEND_API int add_char_to_string(zval *result, zval *op1, zval *op2)
{
	result->value.str.len = op1->value.str.len + 1;
	result->value.str.val = (char *) erealloc(op1->value.str.val, result->value.str.len+1);
    result->value.str.val[result->value.str.len - 1] = (char) op2->value.lval;
	result->value.str.val[result->value.str.len] = 0;
	result->type = IS_STRING;
	return SUCCESS;
}


/* must support result==op1 */
ZEND_API int add_string_to_string(zval *result, zval *op1, zval *op2)
{
	int length = op1->value.str.len + op2->value.str.len;
	if (op1->value.str.val == empty_string) {
		result->value.str.val = (char *) emalloc(length+1);
	} else {
		result->value.str.val = (char *) erealloc(op1->value.str.val, length+1);
	}
    memcpy(result->value.str.val+op1->value.str.len, op2->value.str.val, op2->value.str.len);
    result->value.str.val[length] = 0;
	result->value.str.len = length;
	result->type = IS_STRING;
	return SUCCESS;
}


ZEND_API int concat_function(zval *result, zval *op1, zval *op2 TSRMLS_DC)
{
	zval op1_copy, op2_copy;
	int use_copy1, use_copy2;


	zend_make_printable_zval(op1, &op1_copy, &use_copy1);
	zend_make_printable_zval(op2, &op2_copy, &use_copy2);

	if (use_copy1) {
		op1 = &op1_copy;
	}
	if (use_copy2) {
		op2 = &op2_copy;
	}
	if (result==op1) {	/* special case, perform operations on result */
		uint res_len = op1->value.str.len + op2->value.str.len;
		
		if (result->value.str.len == 0) { /* handle empty_string */
			STR_FREE(result->value.str.val);
			result->value.str.val = emalloc(res_len+1);
		} else {
			result->value.str.val = erealloc(result->value.str.val, res_len+1);
		}
		memcpy(result->value.str.val+result->value.str.len, op2->value.str.val, op2->value.str.len);
		result->value.str.val[res_len]=0;
		result->value.str.len = res_len;
	} else {
		result->value.str.len = op1->value.str.len + op2->value.str.len;
		result->value.str.val = (char *) emalloc(result->value.str.len + 1);
		memcpy(result->value.str.val, op1->value.str.val, op1->value.str.len);
		memcpy(result->value.str.val+op1->value.str.len, op2->value.str.val, op2->value.str.len);
		result->value.str.val[result->value.str.len] = 0;
		result->type = IS_STRING;
	}
	if (use_copy1) {
		zval_dtor(op1);
	}
	if (use_copy2) {
		zval_dtor(op2);
	}
	return SUCCESS;
}


ZEND_API int string_compare_function(zval *result, zval *op1, zval *op2 TSRMLS_DC)
{
	zval op1_copy, op2_copy;
	int use_copy1, use_copy2;

	zend_make_printable_zval(op1, &op1_copy, &use_copy1);
	zend_make_printable_zval(op2, &op2_copy, &use_copy2);

	if (use_copy1) {
		op1 = &op1_copy;
	}
	if (use_copy2) {
		op2 = &op2_copy;
	}

	result->value.lval = zend_binary_zval_strcmp(op1, op2);
	result->type = IS_LONG;

	if (use_copy1) {
		zval_dtor(op1);
	}
	if (use_copy2) {
		zval_dtor(op2);
	}
	return SUCCESS;
}

ZEND_API int numeric_compare_function(zval *result, zval *op1, zval *op2 TSRMLS_DC)
{
	zval op1_copy, op2_copy;

	op1_copy = *op1;
	zval_copy_ctor(&op1_copy);

	op2_copy = *op2;
	zval_copy_ctor(&op2_copy);

	convert_to_double(&op1_copy);
	convert_to_double(&op2_copy);

	result->value.lval = ZEND_NORMALIZE_BOOL(op1_copy.value.dval-op2_copy.value.dval);
	result->type = IS_LONG;

	return SUCCESS;
}


ZEND_API int compare_function(zval *result, zval *op1, zval *op2 TSRMLS_DC)
{
	zval op1_copy, op2_copy;

	if ((op1->type == IS_NULL && op2->type == IS_STRING)
		|| (op2->type == IS_NULL && op1->type == IS_STRING)) {
		if (op1->type == IS_NULL) {
			result->type = IS_LONG;
			result->value.lval = zend_binary_strcmp("", 0, op2->value.str.val, op2->value.str.len);
			return SUCCESS;
		} else {
			result->type = IS_LONG;
			result->value.lval = zend_binary_strcmp(op1->value.str.val, op1->value.str.len, "", 0);
			return SUCCESS;
		}
	}
		
	if (op1->type == IS_STRING && op2->type == IS_STRING) {
		zendi_smart_strcmp(result, op1, op2);
		return SUCCESS;
	}
	
	if (op1->type == IS_BOOL || op2->type == IS_BOOL
		|| op1->type == IS_NULL || op2->type == IS_NULL) {
		zendi_convert_to_boolean(op1, op1_copy, result);
		zendi_convert_to_boolean(op2, op2_copy, result);
		result->type = IS_LONG;
		result->value.lval = ZEND_NORMALIZE_BOOL(op1->value.lval-op2->value.lval);
		return SUCCESS;
	}

	zendi_convert_scalar_to_number(op1, op1_copy, result);
	zendi_convert_scalar_to_number(op2, op2_copy, result);

	if (op1->type == IS_LONG && op2->type == IS_LONG) {
		result->type = IS_LONG;
		result->value.lval = op1->value.lval>op2->value.lval?1:(op1->value.lval<op2->value.lval?-1:0);
		return SUCCESS;
	}
	if ((op1->type == IS_DOUBLE || op1->type == IS_LONG)
		&& (op2->type == IS_DOUBLE || op2->type == IS_LONG)) {
		result->value.dval = (op1->type == IS_LONG ? (double) op1->value.lval : op1->value.dval) - (op2->type == IS_LONG ? (double) op2->value.lval : op2->value.dval);
		result->value.lval = ZEND_NORMALIZE_BOOL(result->value.dval);
		result->type = IS_LONG;
		return SUCCESS;
	}
	if (op1->type==IS_ARRAY && op2->type==IS_ARRAY) {
		zend_compare_arrays(result, op1, op2 TSRMLS_CC);
		return SUCCESS;
	}

	if (op1->type==IS_OBJECT && op2->type==IS_OBJECT) {
		zend_compare_objects(result, op1, op2 TSRMLS_CC);
		return SUCCESS;
	}

	if (op1->type==IS_ARRAY) {
		result->value.lval = 1;
		result->type = IS_LONG;
		return SUCCESS;
	}
	if (op2->type==IS_ARRAY) {
		result->value.lval = -1;
		result->type = IS_LONG;
		return SUCCESS;
	}
	if (op1->type==IS_OBJECT) {
		result->value.lval = 1;
		result->type = IS_LONG;
		return SUCCESS;
	}
	if (op2->type==IS_OBJECT) {
		result->value.lval = -1;
		result->type = IS_LONG;
		return SUCCESS;
	}

	ZVAL_BOOL(result, 0);
	return FAILURE;
}


static int hash_zval_identical_function(const zval **z1, const zval **z2)
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
	return !result.value.lval;
}


ZEND_API int is_identical_function(zval *result, zval *op1, zval *op2 TSRMLS_DC)
{
	result->type = IS_BOOL;
	if (op1->type != op2->type) {
		result->value.lval = 0;
		return SUCCESS;
	}
	switch (op1->type) {
		case IS_NULL:
			result->value.lval = (op2->type==IS_NULL);
			break;
		case IS_BOOL:
		case IS_LONG:
		case IS_RESOURCE:
			result->value.lval = (op1->value.lval == op2->value.lval);
			break;
		case IS_DOUBLE:
			result->value.lval = (op1->value.dval == op2->value.dval);
			break;
		case IS_STRING:
			if ((op1->value.str.len == op2->value.str.len)
				&& (!memcmp(op1->value.str.val, op2->value.str.val, op1->value.str.len))) {
				result->value.lval = 1;
			} else {
				result->value.lval = 0;
			}
			break;
		case IS_ARRAY:
			if (zend_hash_compare(op1->value.ht, op2->value.ht, (compare_func_t) hash_zval_identical_function, 1 TSRMLS_CC)==0) {
				result->value.lval = 1;
			} else {
				result->value.lval = 0;
			}
			break;
		case IS_OBJECT:
			if (Z_OBJ_HT_P(op1) == Z_OBJ_HT_P(op2) && Z_OBJ_HANDLE_P(op1) == Z_OBJ_HANDLE_P(op2)) {
				result->value.lval = 1;
			} else {
				result->value.lval = 0;
			}
			break;
		default:
			ZVAL_BOOL(result, 0);
			return FAILURE;
	}
	return SUCCESS;
}


ZEND_API int is_not_identical_function(zval *result, zval *op1, zval *op2 TSRMLS_DC)
{
   result->type = IS_BOOL;
   if (is_identical_function(result, op1, op2 TSRMLS_CC) == FAILURE) {
	  return FAILURE;
   }
   result->value.lval = !result->value.lval;
   return SUCCESS;
}


ZEND_API int is_equal_function(zval *result, zval *op1, zval *op2 TSRMLS_DC)
{
	if (compare_function(result, op1, op2 TSRMLS_CC) == FAILURE) {
		return FAILURE;
	}
	convert_to_boolean(result);
	if (result->value.lval == 0) {
		result->value.lval = 1;
	} else {
		result->value.lval = 0;
	}
	return SUCCESS;
}


ZEND_API int is_not_equal_function(zval *result, zval *op1, zval *op2 TSRMLS_DC)
{
	if (compare_function(result, op1, op2 TSRMLS_CC) == FAILURE) {
		return FAILURE;
	}
	convert_to_boolean(result);
	if (result->value.lval) {
		result->value.lval = 1;
	} else {
		result->value.lval = 0;
	}
	return SUCCESS;
}


ZEND_API int is_smaller_function(zval *result, zval *op1, zval *op2 TSRMLS_DC)
{
	if (compare_function(result, op1, op2 TSRMLS_CC) == FAILURE) {
		return FAILURE;
	}
	if (result->type == IS_LONG) {
		result->type = IS_BOOL;
		if (result->value.lval < 0) {
			result->value.lval = 1;
		} else {
			result->value.lval = 0;
		}
		return SUCCESS;
	}
	if (result->type == IS_DOUBLE) {
		result->type = IS_BOOL;
		if (result->value.dval < 0) {
			result->value.lval = 1;
		} else {
			result->value.lval = 0;
		}
		return SUCCESS;
	}
	zend_error(E_ERROR, "Unsupported operand types");
	return FAILURE;
}


ZEND_API int is_smaller_or_equal_function(zval *result, zval *op1, zval *op2 TSRMLS_DC)
{
	if (compare_function(result, op1, op2 TSRMLS_CC) == FAILURE) {
		return FAILURE;
	}
	if (result->type == IS_LONG) {
		result->type = IS_BOOL;
		if (result->value.lval <= 0) {
			result->value.lval = 1;
		} else {
			result->value.lval = 0;
		}
		return SUCCESS;
	}
	if (result->type == IS_DOUBLE) {
		result->type = IS_BOOL;
		if (result->value.dval <= 0) {
			result->value.lval = 1;
		} else {
			result->value.lval = 0;
		}
		return SUCCESS;
	}
	zend_error(E_ERROR, "Unsupported operand types");
	return FAILURE;
}


ZEND_API int is_class_function(zval *result, zval *op1, zend_class_entry *class TSRMLS_DC)
{
	if (Z_TYPE_P(op1) == IS_OBJECT) {
		zend_class_entry *ce;
		for (ce = Z_OBJCE_P(op1); ce != NULL; ce = ce->parent) {
			if (ce == class) {
				ZVAL_BOOL(result, 1);
				return SUCCESS;
			}
		}
	}
	ZVAL_BOOL(result, 0);
	return SUCCESS;
}

#define LOWER_CASE 1
#define UPPER_CASE 2
#define NUMERIC 3


static void increment_string(zval *str)
{
    int carry=0;
    int pos=str->value.str.len-1;
    char *s=str->value.str.val;
    char *t;
    int last=0; /* Shut up the compiler warning */
    int ch;
    
	if (str->value.str.len == 0) {
		STR_FREE(str->value.str.val);
		str->value.str.val = estrndup("1", sizeof("1")-1);
		str->value.str.len = 1;
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
        t = (char *) emalloc(str->value.str.len+1+1);
        memcpy(t+1, str->value.str.val, str->value.str.len);
        str->value.str.len++;
        t[str->value.str.len] = '\0';
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
        STR_FREE(str->value.str.val);
        str->value.str.val = t;
    }
}


ZEND_API int increment_function(zval *op1)
{
	switch (op1->type) {
		case IS_LONG:
			if (op1->value.lval == LONG_MAX) {
				/* switch to double */
				double d = (double)op1->value.lval;
				ZVAL_DOUBLE(op1, d+1);
			} else {
			op1->value.lval++;
			} 
			break;
		case IS_DOUBLE:
			op1->value.dval = op1->value.dval + 1;
			break;
		case IS_NULL:
			op1->value.lval = 1;
			op1->type = IS_LONG;
			break;
		case IS_STRING: {
				long lval;
				double dval;
				char *strval = op1->value.str.val;

				switch (is_numeric_string(strval, op1->value.str.len, &lval, &dval, 0)) {
					case IS_LONG:
						if (lval == LONG_MAX) {
							/* switch to double */
							double d = (double)lval;
							ZVAL_DOUBLE(op1, d+1);
						} else {
						op1->value.lval = lval+1;
						op1->type = IS_LONG;
						}
						efree(strval); /* should never be empty_string */
						break;
					case IS_DOUBLE:
						op1->value.dval = dval+1;
						op1->type = IS_DOUBLE;
						efree(strval); /* should never be empty_string */
						break;
#if 0
					case FLAG_IS_BC:
						/* Not implemented */
#endif
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


ZEND_API int decrement_function(zval *op1)
{
	long lval;
	double dval;
	
	switch (op1->type) {
		case IS_LONG:
			if (op1->value.lval == LONG_MIN) {
				double d = (double)op1->value.lval;
				ZVAL_DOUBLE(op1, d-1);
			} else {
			op1->value.lval--;
			}
			break;
		case IS_DOUBLE:
			op1->value.dval = op1->value.dval - 1;
			break;
		case IS_STRING:		/* Like perl we only support string increment */
			if (op1->value.str.len == 0) { /* consider as 0 */
				STR_FREE(op1->value.str.val);
				op1->value.lval = -1;
				op1->type = IS_LONG;
				break;
			}
			switch(is_numeric_string(op1->value.str.val, op1->value.str.len, &lval, &dval, 0)) {
				case IS_LONG:
					STR_FREE(op1->value.str.val);
					if (lval == LONG_MIN) {
						double d = (double)lval;
						ZVAL_DOUBLE(op1, d-1);
					} else {
						op1->value.lval = lval-1;
						op1->type = IS_LONG;
					}
					break;
				case IS_DOUBLE:
					STR_FREE(op1->value.str.val);
					op1->value.dval = dval - 1;
					op1->type = IS_DOUBLE;
					break;
			}
			break;
		default:
			return FAILURE;
	}

	return SUCCESS;
}


ZEND_API int zval_is_true(zval *op)
{
	convert_to_boolean(op);
	return (op->value.lval ? 1 : 0);
}

ZEND_API void zend_str_tolower(char *str, unsigned int length)
{
	register char *p=str, *end=p+length;
	
	while (p<end) {
		if (*p >= 'A' && *p <= 'Z') {
			*p = (*p)+32;
		}	
		p++;
	}
}

ZEND_API int zend_binary_strcmp(char *s1, uint len1, char *s2, uint len2)
{
	int retval;
	
	retval = memcmp(s1, s2, MIN(len1, len2));
	if (!retval) {
		return (len1 - len2);
	} else {
		return retval;
	}
}

ZEND_API int zend_binary_strncmp(char *s1, uint len1, char *s2, uint len2, uint length)
{
	int retval;
	
	retval = memcmp(s1, s2, MIN(length, MIN(len1, len2)));
	if (!retval) {
		return (MIN(length, len1) - MIN(length, len2));
	} else {
		return retval;
	}
}


ZEND_API int zend_binary_strcasecmp(char *s1, uint len1, char *s2, uint len2)
{
	int len;
	int c1, c2;

	len = MIN(len1, len2);

	while (len--) {
		c1 = tolower(*s1++);
		c2 = tolower(*s2++);
		if (c1 != c2) {
			return c1 - c2;
		}
	}

	return len1 - len2;
}


ZEND_API int zend_binary_strncasecmp(char *s1, uint len1, char *s2, uint len2, uint length)
{
	int len;
	int c1, c2;

	len = MIN(length, MIN(len1, len2));

	while (len--) {
		c1 = tolower(*s1++);
		c2 = tolower(*s2++);
		if (c1 != c2) {
			return c1 - c2;
		}
	}

	return MIN(length, len1) - MIN(length, len2);
}


ZEND_API int zend_binary_zval_strcmp(zval *s1, zval *s2)
{
	return zend_binary_strcmp(s1->value.str.val, s1->value.str.len, s2->value.str.val, s2->value.str.len);
}

ZEND_API int zend_binary_zval_strncmp(zval *s1, zval *s2, zval *s3)
{
	return zend_binary_strncmp(s1->value.str.val, s1->value.str.len, s2->value.str.val, s2->value.str.len, s3->value.lval);
}


ZEND_API int zend_binary_zval_strcasecmp(zval *s1, zval *s2)
{
	return zend_binary_strcasecmp(s1->value.str.val, s1->value.str.len, s2->value.str.val, s2->value.str.len);
}


ZEND_API int zend_binary_zval_strncasecmp(zval *s1, zval *s2, zval *s3)
{
	return zend_binary_strncasecmp(s1->value.str.val, s1->value.str.len, s2->value.str.val, s2->value.str.len, s3->value.lval);
}


ZEND_API void zendi_smart_strcmp(zval *result, zval *s1, zval *s2)
{
	int ret1, ret2;
	long lval1, lval2;
	double dval1, dval2;
	
	if ((ret1=is_numeric_string(s1->value.str.val, s1->value.str.len, &lval1, &dval1, 0)) &&
		(ret2=is_numeric_string(s2->value.str.val, s2->value.str.len, &lval2, &dval2, 0))) {
#if 0&&WITH_BCMATH
		if ((ret1==FLAG_IS_BC) || (ret2==FLAG_IS_BC)) {
			bc_num first, second;
			
			/* use the BC math library to compare the numbers */
			init_num(&first);
			init_num(&second);
			str2num(&first, s1->value.str.val, 100); /* this scale should do */
			str2num(&second, s2->value.str.val, 100); /* ditto */
			result->value.lval = bc_compare(first, second);
			result->value.lval = ZEND_NORMALIZE_BOOL(result->value.lval);
			result->type = IS_LONG;
			free_num(&first);
			free_num(&second);
		} else
#endif
		if ((ret1==IS_DOUBLE) || (ret2==IS_DOUBLE)) {
			if (ret1!=IS_DOUBLE) {
				dval1 = strtod(s1->value.str.val, NULL);
			} else if (ret2!=IS_DOUBLE) {
				dval2 = strtod(s2->value.str.val, NULL);
			}
			result->value.dval = dval1 - dval2;
			result->value.lval = ZEND_NORMALIZE_BOOL(result->value.dval);
			result->type = IS_LONG;
		} else { /* they both have to be long's */
			result->value.lval = lval1 - lval2;
			result->value.lval = ZEND_NORMALIZE_BOOL(result->value.lval);
			result->type = IS_LONG;
		}
	} else {
		result->value.lval = zend_binary_zval_strcmp(s1, s2);
		result->value.lval = ZEND_NORMALIZE_BOOL(result->value.lval);
		result->type = IS_LONG;
	}
	return;	
}


static int hash_zval_compare_function(const zval **z1, const zval **z2 TSRMLS_DC)
{
	zval result;

	if (compare_function(&result, (zval *) *z1, (zval *) *z2 TSRMLS_CC)==FAILURE) {
		return 1;
	}
	return result.value.lval;
}

ZEND_API int zend_compare_symbol_tables_i(HashTable *ht1, HashTable *ht2 TSRMLS_DC)
{
	return zend_hash_compare(ht1, ht2, (compare_func_t) hash_zval_compare_function, 0 TSRMLS_CC);
}



ZEND_API void zend_compare_symbol_tables(zval *result, HashTable *ht1, HashTable *ht2 TSRMLS_DC)
{
	result->type = IS_LONG;
	result->value.lval = zend_hash_compare(ht1, ht2, (compare_func_t) hash_zval_compare_function, 0 TSRMLS_CC);
}


ZEND_API void zend_compare_arrays(zval *result, zval *a1, zval *a2 TSRMLS_DC)
{
	zend_compare_symbol_tables(result, a1->value.ht, a2->value.ht TSRMLS_CC);
}


ZEND_API void zend_compare_objects(zval *result, zval *o1, zval *o2 TSRMLS_DC)
{
	result->type = IS_LONG;
	if (Z_OBJ_HT_P(o1) != Z_OBJ_HT_P(o2)) {
		result->value.lval = 1;	/* Comparing objects of different types is pretty much meaningless */
		return;
	}

	if (Z_OBJ_HT_P(o1)->compare_objects == NULL) {
		if (Z_OBJ_HANDLE_P(o1) == Z_OBJ_HANDLE_P(o2)) {
			result->value.lval = 0;
		} else {
			result->value.lval = 1;
		}			
		return;
	} else {
		result->value.lval = Z_OBJ_HT_P(o1)->compare_objects(o1, o2 TSRMLS_CC);
	}
}
