/* 
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Rasmus Lerdorf <rasmus@php.net>                             |
   |          Andrei Zmievski <andrei@ispi.net>                           |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "php_ini.h"
#include "zend_operators.h"
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#if HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#ifdef PHP_WIN32
#include "win32/unistd.h"
#endif
#include "zend_globals.h"
#include "php_globals.h"
#include "php_array.h"
#include "basic_functions.h"
#include "php_string.h"
#include "php_rand.h"

#ifdef ZTS
int array_globals_id;
#else
php_array_globals array_globals;
#endif

#define EXTR_OVERWRITE		0
#define EXTR_SKIP			1
#define EXTR_PREFIX_SAME	2
#define	EXTR_PREFIX_ALL		3

#define SORT_DESC		   -1
#define SORT_ASC		    1

#define SORT_REGULAR		0
#define SORT_NUMERIC		1
#define	SORT_STRING			2

PHP_MINIT_FUNCTION(array)
{
#ifdef ZTS
    array_globals_id = ts_allocate_id(sizeof(php_array_globals), NULL, NULL);
#endif

	REGISTER_LONG_CONSTANT("EXTR_OVERWRITE", EXTR_OVERWRITE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EXTR_SKIP", EXTR_SKIP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EXTR_PREFIX_SAME", EXTR_PREFIX_SAME, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EXTR_PREFIX_ALL", EXTR_PREFIX_ALL, CONST_CS | CONST_PERSISTENT);
	
	REGISTER_LONG_CONSTANT("SORT_ASC", SORT_ASC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SORT_DESC", SORT_DESC, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("SORT_REGULAR", SORT_REGULAR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SORT_NUMERIC", SORT_NUMERIC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SORT_STRING", SORT_STRING, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(array)
{
#ifdef ZTS
    ts_free_id(array_globals_id);
#endif

    return SUCCESS;
}

static void set_compare_func(int sort_type)
{
	ARRAYLS_FETCH();

	switch (sort_type) {
		case SORT_NUMERIC:
			ARRAYG(compare_func) = numeric_compare_function;
			break;

		case SORT_STRING:
			ARRAYG(compare_func) = string_compare_function;
			break;

		case SORT_REGULAR:
		default:
			ARRAYG(compare_func) = compare_function;
			break;
	}
}

static int array_key_compare(const void *a, const void *b)
{
	Bucket *f;
	Bucket *s;
	pval result;
	pval first;
	pval second;
	ARRAYLS_FETCH();
 
	f = *((Bucket **) a);
	s = *((Bucket **) b);

	if (f->nKeyLength == 0) {
		first.type = IS_LONG;
		first.value.lval = f->h;
	} else {
		first.type = IS_STRING;
		first.value.str.val = f->arKey;
		first.value.str.len = f->nKeyLength;
	}

	if (s->nKeyLength == 0) {
		second.type = IS_LONG;
		second.value.lval = s->h;
	} else {
		second.type = IS_STRING;
		second.value.str.val = s->arKey;
		second.value.str.len = s->nKeyLength;
	}
 
    if (ARRAYG(compare_func)(&result, &first, &second) == FAILURE) {
        return 0;
    } 

    if (result.type == IS_DOUBLE) {
        if (result.value.dval < 0) {
			return -1;
        } else if (result.value.dval > 0) {
			return 1;
        } else {
			return 0;
		}
    }

	convert_to_long(&result);

	if (result.value.lval < 0) {
		return -1;
	} else if (result.value.lval > 0) {
		return 1;
	} 

	return 0;
}

static int array_reverse_key_compare(const void *a, const void *b)
{
	return array_key_compare(a,b)*-1;
}

/* {{{ proto int krsort(array array_arg)
   Sort an array reverse by key */
PHP_FUNCTION(krsort)
{
	zval **array, **sort_type;
	int sort_type_val = SORT_REGULAR;
	HashTable *target_hash;

	if (ZEND_NUM_ARGS() < 1 || ZEND_NUM_ARGS() > 2 ||
		zend_get_parameters_ex(ZEND_NUM_ARGS(), &array, &sort_type) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Wrong datatype in krsort() call");
		return;
	}
	if (ZEND_NUM_ARGS() == 2) {
		convert_to_long_ex(sort_type);
		sort_type_val = Z_LVAL_PP(sort_type);
	}
	set_compare_func(sort_type_val);
	if (zend_hash_sort(target_hash, qsort, array_reverse_key_compare, 0) == FAILURE) {
		return;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ksort(array array_arg)
   Sort an array by key */
PHP_FUNCTION(ksort)
{
	zval **array, **sort_type;
	int sort_type_val = SORT_REGULAR;
	HashTable *target_hash;

	if (ZEND_NUM_ARGS() < 1 || ZEND_NUM_ARGS() > 2 ||
		zend_get_parameters_ex(ZEND_NUM_ARGS(), &array, &sort_type) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Wrong datatype in ksort() call");
		return;
	}
	if (ZEND_NUM_ARGS() == 2) {
		convert_to_long_ex(sort_type);
		sort_type_val = Z_LVAL_PP(sort_type);
	}
	set_compare_func(sort_type_val);
	if (zend_hash_sort(target_hash, qsort, array_key_compare,0) == FAILURE) {
		return;
	}
	RETURN_TRUE;
}

/* {{{ proto int count(mixed var)
   Count the number of elements in a variable (usually an array) */
PHP_FUNCTION(count)
{
	pval **array;
	HashTable *target_hash;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		if ((*array)->type == IS_NULL) {
			RETURN_LONG(0);
		} else {
			RETURN_LONG(1);
		}
	}

	RETURN_LONG(zend_hash_num_elements(target_hash));
}
/* }}} */

/* Numbers are always smaller than strings int this function as it
 * anyway doesn't make much sense to compare two different data types.
 * This keeps it consistant and simple.
 */
static int array_data_compare(const void *a, const void *b)
{
	Bucket *f;
	Bucket *s;
	pval result;
	pval *first;
	pval *second;
	ARRAYLS_FETCH();
 
	f = *((Bucket **) a);
	s = *((Bucket **) b);
 
	first = *((pval **) f->pData);
	second = *((pval **) s->pData);

    if (ARRAYG(compare_func)(&result, first, second) == FAILURE) {
        return 0;
    } 

    if (result.type == IS_DOUBLE) {
        if (result.value.dval < 0) {
			return -1;
        } else if (result.value.dval > 0) {
			return 1;
        } else {
			return 0;
		}
    }

	convert_to_long(&result);

	if (result.value.lval < 0) {
		return -1;
	} else if (result.value.lval > 0) {
		return 1;
	} 

	return 0;
}

static int array_reverse_data_compare(const void *a, const void *b)
{
	return array_data_compare(a,b)*-1;
}

static int array_natural_general_compare(const void *a, const void *b, int fold_case)
{
	Bucket *f, *s;
	zval *fval, *sval;
	zval first, second;
	int result;
 
	f = *((Bucket **) a);
	s = *((Bucket **) b);
 
	fval = *((pval **) f->pData);
	sval = *((pval **) s->pData);
	first = *fval;
	second = *sval;
	if (fval->type != IS_STRING) {
		zval_copy_ctor(&first);
		convert_to_string(&first);
	}
	if (sval->type != IS_STRING) {
		zval_copy_ctor(&first);
		convert_to_string(&second);
	}

	result = strnatcmp_ex(first.value.str.val, first.value.str.len,
						  second.value.str.val, second.value.str.len, fold_case);

	if (fval->type != IS_STRING)
		zval_dtor(&first);
	if (sval->type != IS_STRING)
		zval_dtor(&second);
	
	return result;
}

static int array_natural_compare(const void *a, const void *b)
{
	return array_natural_general_compare(a, b, 0);
}

static int array_natural_case_compare(const void *a, const void *b)
{
	return array_natural_general_compare(a, b, 1);
}

static void php_natsort(INTERNAL_FUNCTION_PARAMETERS, int fold_case)
{
	zval **array;
	HashTable *target_hash;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Wrong datatype in %s() call",
				  get_active_function_name());
		return;
	}

	if (fold_case) {
		if (zend_hash_sort(target_hash, qsort, array_natural_case_compare, 0) == FAILURE) {
			return;
		}
	} else {
		if (zend_hash_sort(target_hash, qsort, array_natural_compare, 0) == FAILURE) {
			return;
		}
	}

	RETURN_TRUE;
}


/* {{{ proto void natsort(array array_arg)
   Sort an array using natural sort */
PHP_FUNCTION(natsort)
{
	php_natsort(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */


/* {{{ proto void natcasesort(array array_arg)
   Sort an array using case-insensitive natural sort */
PHP_FUNCTION(natcasesort)
{
	php_natsort(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */


/* {{{ proto void asort(array array_arg)
   Sort an array and maintain index association */
PHP_FUNCTION(asort)
{
	zval **array, **sort_type;
	int sort_type_val = SORT_REGULAR;
	HashTable *target_hash;

	if (ZEND_NUM_ARGS() < 1 || ZEND_NUM_ARGS() > 2 ||
		zend_get_parameters_ex(ZEND_NUM_ARGS(), &array, &sort_type) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Wrong datatype in asort() call");
		return;
	}
	if (ZEND_NUM_ARGS() == 2) {
		convert_to_long_ex(sort_type);
		sort_type_val = Z_LVAL_PP(sort_type);
	}
	set_compare_func(sort_type_val);
	if (zend_hash_sort(target_hash, qsort, array_data_compare,0) == FAILURE) {
		return;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void arsort(array array_arg)
   Sort an array in reverse order and maintain index association */
PHP_FUNCTION(arsort)
{
	zval **array, **sort_type;
	int sort_type_val = SORT_REGULAR;
	HashTable *target_hash;

	if (ZEND_NUM_ARGS() < 1 || ZEND_NUM_ARGS() > 2 ||
		zend_get_parameters_ex(ZEND_NUM_ARGS(), &array, &sort_type) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Wrong datatype in arsort() call");
		RETURN_FALSE;
	}
	if (ZEND_NUM_ARGS() == 2) {
		convert_to_long_ex(sort_type);
		sort_type_val = Z_LVAL_PP(sort_type);
	}
	set_compare_func(sort_type_val);
	if (zend_hash_sort(target_hash, qsort, array_reverse_data_compare,0) == FAILURE) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void sort(array array_arg)
   Sort an array */
PHP_FUNCTION(sort)
{
	zval **array, **sort_type;
	int sort_type_val = SORT_REGULAR;
	HashTable *target_hash;

	if (ZEND_NUM_ARGS() < 1 || ZEND_NUM_ARGS() > 2 ||
		zend_get_parameters_ex(ZEND_NUM_ARGS(), &array, &sort_type) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Wrong datatype in sort() call");
		RETURN_FALSE;
	}
	if (ZEND_NUM_ARGS() == 2) {
		convert_to_long_ex(sort_type);
		sort_type_val = Z_LVAL_PP(sort_type);
	}
	set_compare_func(sort_type_val);
	if (zend_hash_sort(target_hash, qsort, array_data_compare,1) == FAILURE) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void rsort(array array_arg)
   Sort an array in reverse order */
PHP_FUNCTION(rsort)
{
	zval **array, **sort_type;
	int sort_type_val = SORT_REGULAR;
	HashTable *target_hash;

	if (ZEND_NUM_ARGS() < 1 || ZEND_NUM_ARGS() > 2 ||
		zend_get_parameters_ex(ZEND_NUM_ARGS(), &array, &sort_type) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Wrong datatype in rsort() call");
		RETURN_FALSE;
	}
	if (ZEND_NUM_ARGS() == 2) {
		convert_to_long_ex(sort_type);
		sort_type_val = Z_LVAL_PP(sort_type);
	}
	set_compare_func(sort_type_val);
	if (zend_hash_sort(target_hash, qsort, array_reverse_data_compare,1) == FAILURE) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}


static int array_user_compare(const void *a, const void *b)
{
	Bucket *f;
	Bucket *s;
	pval **args[2];
	pval *retval_ptr;
	CLS_FETCH();
	BLS_FETCH();

	f = *((Bucket **) a);
	s = *((Bucket **) b);

	args[0] = (pval **) f->pData;
	args[1] = (pval **) s->pData;

	if (call_user_function_ex(CG(function_table), NULL, *BG(user_compare_func_name), &retval_ptr, 2, args, 0, NULL)==SUCCESS
		&& retval_ptr) {
		long retval;

		convert_to_long_ex(&retval_ptr);
		retval = retval_ptr->value.lval;
		zval_ptr_dtor(&retval_ptr);
		return retval;
	} else {
		return 0;
	}
}

/* {{{ proto void usort(array array_arg, string cmp_function)
   Sort an array by values using a user-defined comparison function */
PHP_FUNCTION(usort)
{
	pval **array;
	pval **old_compare_func;
	HashTable *target_hash;
	BLS_FETCH();

	old_compare_func = BG(user_compare_func_name);
	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &array, &BG(user_compare_func_name)) == FAILURE) {
		BG(user_compare_func_name) = old_compare_func;
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Wrong datatype in usort() call");
		BG(user_compare_func_name) = old_compare_func;
		RETURN_FALSE;
	}
	if (zend_hash_sort(target_hash, qsort, array_user_compare, 1) == FAILURE) {
		BG(user_compare_func_name) = old_compare_func;
		RETURN_FALSE;
	}
	BG(user_compare_func_name) = old_compare_func;
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void uasort(array array_arg, string cmp_function)
   Sort an array with a user-defined comparison function and maintain index association */
PHP_FUNCTION(uasort)
{
	pval **array;
	pval **old_compare_func;
	HashTable *target_hash;
	BLS_FETCH();

	old_compare_func = BG(user_compare_func_name);
	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &array, &BG(user_compare_func_name)) == FAILURE) {
		BG(user_compare_func_name) = old_compare_func;
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Wrong datatype in uasort() call");
		BG(user_compare_func_name) = old_compare_func;
		RETURN_FALSE;
	}
	if (zend_hash_sort(target_hash, qsort, array_user_compare, 0) == FAILURE) {
		BG(user_compare_func_name) = old_compare_func;
		RETURN_FALSE;
	}
	BG(user_compare_func_name) = old_compare_func;
	RETURN_TRUE;
}
/* }}} */

static int array_user_key_compare(const void *a, const void *b)
{
	Bucket *f;
	Bucket *s;
	pval key1, key2;
	pval *args[2];
	pval retval;
	int status;
	CLS_FETCH();
	BLS_FETCH();

	args[0] = &key1;
	args[1] = &key2;
	INIT_PZVAL(&key1);
	INIT_PZVAL(&key2);
	
	f = *((Bucket **) a);
	s = *((Bucket **) b);

	if (f->nKeyLength) {
		key1.value.str.val = estrndup(f->arKey, f->nKeyLength);
		key1.value.str.len = f->nKeyLength-1;
		key1.type = IS_STRING;
	} else {
		key1.value.lval = f->h;
		key1.type = IS_LONG;
	}
	if (s->nKeyLength) {
		key2.value.str.val = estrndup(s->arKey, s->nKeyLength);
		key2.value.str.len = s->nKeyLength-1;
		key2.type = IS_STRING;
	} else {
		key2.value.lval = s->h;
		key2.type = IS_LONG;
	}

	status = call_user_function(CG(function_table), NULL, *BG(user_compare_func_name), &retval, 2, args);
	
	zval_dtor(&key1);
	zval_dtor(&key2);
	
	if (status==SUCCESS) {
		convert_to_long(&retval);
		return retval.value.lval;
	} else {
		return 0;
	}
}

/* {{{ proto void uksort(array array_arg, string cmp_function)
   Sort an array by keys using a user-defined comparison function */
PHP_FUNCTION(uksort)
{
	pval **array;
	pval **old_compare_func;
	HashTable *target_hash;
	BLS_FETCH();

	old_compare_func = BG(user_compare_func_name);
	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &array, &BG(user_compare_func_name)) == FAILURE) {
		BG(user_compare_func_name) = old_compare_func;
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Wrong datatype in uksort() call");
		BG(user_compare_func_name) = old_compare_func;
		RETURN_FALSE;
	}
	if (zend_hash_sort(target_hash, qsort, array_user_key_compare, 0) == FAILURE) {
		BG(user_compare_func_name) = old_compare_func;
		RETURN_FALSE;
	}
	BG(user_compare_func_name) = old_compare_func;
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto mixed end(array array_arg)
   Advances array argument's internal pointer to the last element and return it */
PHP_FUNCTION(end)
{
	pval **array, **entry;
	HashTable *target_hash;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Variable passed to end() is not an array or object");
		RETURN_FALSE;
	}
	zend_hash_internal_pointer_end(target_hash);

	if (return_value_used) {	
		if (zend_hash_get_current_data(target_hash, (void **) &entry) == FAILURE) {
			RETURN_FALSE;
		}

		*return_value = **entry;
		zval_copy_ctor(return_value);
	}
}
/* }}} */

/* {{{ proto mixed prev(array array_arg)
   Move array argument's internal pointer to the previous element and return it */
PHP_FUNCTION(prev)
{
	pval **array, **entry;
	HashTable *target_hash;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Variable passed to prev() is not an array or object");
		RETURN_FALSE;
	}
	zend_hash_move_backwards(target_hash);

	if (return_value_used) {	
		if (zend_hash_get_current_data(target_hash, (void **) &entry) == FAILURE) {
			RETURN_FALSE;
		}
	
		*return_value = **entry;
		zval_copy_ctor(return_value);
	}
}
/* }}} */

/* {{{ proto mixed next(array array_arg)
   Move array argument's internal pointer to the next element and return it */
PHP_FUNCTION(next)
{
	pval **array, **entry;
	HashTable *target_hash;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Variable passed to next() is not an array or object");
		RETURN_FALSE;
	}
	zend_hash_move_forward(target_hash);

	if (return_value_used) {
		if (zend_hash_get_current_data(target_hash, (void **) &entry) == FAILURE) {
			RETURN_FALSE;
		}
	  
		*return_value = **entry;
		zval_copy_ctor(return_value);
	}
}
/* }}} */

/* {{{ proto mixed reset(array array_arg)
   Set array argument's internal pointer to the first element and return it */	
PHP_FUNCTION(reset)
{
	pval **array, **entry;
	HashTable *target_hash;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Variable passed to reset() is not an array or object");
		RETURN_FALSE;
	}
	zend_hash_internal_pointer_reset(target_hash);

	if (return_value_used) {	
		if (zend_hash_get_current_data(target_hash, (void **) &entry) == FAILURE) {
			RETURN_FALSE;
		}

		*return_value = **entry;
		zval_copy_ctor(return_value);
	}
}
/* }}} */

/* {{{ proto mixed current(array array_arg)
   Return the element currently pointed to by the internal array pointer */
PHP_FUNCTION(current)
{
	pval **array, **entry;
	HashTable *target_hash;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Variable passed to current() is not an array or object");
		RETURN_FALSE;
	}
	if (zend_hash_get_current_data(target_hash, (void **) &entry) == FAILURE) {
		RETURN_FALSE;
	}
	*return_value = **entry;
	zval_copy_ctor(return_value);
}
/* }}} */

/* {{{ proto mixed key(array array_arg)
   Return the key of the element currently pointed to by the internal array pointer */
PHP_FUNCTION(key)
{
	pval **array;
	char *string_key;
	ulong num_key;
	HashTable *target_hash;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Variable passed to key() is not an array or object");
		RETURN_FALSE;
	}
	switch (zend_hash_get_current_key(target_hash, &string_key, &num_key)) {
		case HASH_KEY_IS_STRING:
			return_value->value.str.val = string_key;
			return_value->value.str.len = strlen(string_key);
			return_value->type = IS_STRING;
			break;
		case HASH_KEY_IS_LONG:
			return_value->type = IS_LONG;
			return_value->value.lval = num_key;
			break;
		case HASH_KEY_NON_EXISTANT:
			return;
	}
}
/* }}} */

/* {{{ proto mixed min(mixed arg1 [, mixed arg2 [, mixed ...]])
   Return the lowest value in an array or a series of arguments */
PHP_FUNCTION(min)
{
	int argc=ZEND_NUM_ARGS();
	pval **result;

	if (argc<=0) {
		php_error(E_WARNING, "min: must be passed at least 1 value");
		RETURN_NULL();
	}
	set_compare_func(SORT_REGULAR);
	if (argc == 1) {
		pval **arr;

		if (zend_get_parameters_ex(1, &arr) == FAILURE || (*arr)->type != IS_ARRAY) {
			WRONG_PARAM_COUNT;
		}
		if (zend_hash_minmax((*arr)->value.ht, array_data_compare, 0, (void **) &result)==SUCCESS) {
			*return_value = **result;
			zval_copy_ctor(return_value);
		} else {
			php_error(E_WARNING, "min: array must contain at least 1 element");
			RETURN_FALSE;
		}
	} else {
		pval ***args = (pval ***) emalloc(sizeof(pval **)*ZEND_NUM_ARGS());
		pval **min, result;
		int i;

		if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), args)==FAILURE) {
			efree(args);
			WRONG_PARAM_COUNT;
		}

		min = args[0];

		for (i=1; i<ZEND_NUM_ARGS(); i++) {
			is_smaller_function(&result, *args[i], *min);
			if (result.value.lval == 1) {
				min = args[i];
			}
		}

		*return_value = **min;
		zval_copy_ctor(return_value);

		efree(args);
	}
}
/* }}} */

/* {{{ proto mixed max(mixed arg1 [, mixed arg2 [, mixed ...]])
   Return the highest value in an array or a series of arguments */
PHP_FUNCTION(max)
{
	int argc=ZEND_NUM_ARGS();
	pval **result;

	if (argc<=0) {
		php_error(E_WARNING, "max: must be passed at least 1 value");
		RETURN_NULL();
	}
	set_compare_func(SORT_REGULAR);
	if (argc == 1) {
		pval **arr;

		if (zend_get_parameters_ex(1, &arr) == FAILURE || (*arr)->type != IS_ARRAY) {
			WRONG_PARAM_COUNT;
		}
		if (zend_hash_minmax((*arr)->value.ht, array_data_compare, 1, (void **) &result)==SUCCESS) {
			*return_value = **result;
			zval_copy_ctor(return_value);
		} else {
			php_error(E_WARNING, "max: array must contain at least 1 element");
			RETURN_FALSE;
		}
	} else {
		pval ***args = (pval ***) emalloc(sizeof(pval **)*ZEND_NUM_ARGS());
		pval **max, result;
		int i;

		if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), args)==FAILURE) {
			efree(args);
			WRONG_PARAM_COUNT;
		}

		max = args[0];

		for (i=1; i<ZEND_NUM_ARGS(); i++) {
			is_smaller_or_equal_function(&result, *args[i], *max);
			if (result.value.lval == 0) {
				max = args[i];
			}
		}

		*return_value = **max;
		zval_copy_ctor(return_value);

		efree(args);
	}
}
/* }}} */

static int php_array_walk(HashTable *target_hash, zval **userdata)
{
	zval **args[3],			/* Arguments to userland function */
		  *retval_ptr,			/* Return value - unused */
		  *key;				/* Entry key */
	char  *string_key;
	ulong  num_key;
	CLS_FETCH();
	BLS_FETCH();

	/* Allocate space for key */
	MAKE_STD_ZVAL(key);
	
	/* Set up known arguments */
	args[1] = &key;
	args[2] = userdata;

	zend_hash_internal_pointer_reset(target_hash);

	/* Iterate through hash */
	while(zend_hash_get_current_data(target_hash, (void **)&args[0]) == SUCCESS) {
		/* Set up the key */
		if (zend_hash_get_current_key(target_hash, &string_key, &num_key) == HASH_KEY_IS_LONG) {
			key->type = IS_LONG;
			key->value.lval = num_key;
		} else {
			key->type = IS_STRING;
			key->value.str.val = string_key;
			key->value.str.len = strlen(string_key);
		}
		
		/* Call the userland function */
		if (call_user_function_ex(CG(function_table), NULL, *BG(array_walk_func_name),
						   &retval_ptr, userdata ? 3 : 2, args, 0, NULL) == SUCCESS) {
		
			zval_ptr_dtor(&retval_ptr);
		} else
			php_error(E_WARNING,"Unable to call %s() - function does not exist",
					  (*BG(array_walk_func_name))->value.str.val);

		/* Clean up the key */
		if (zend_hash_get_current_key_type(target_hash) == HASH_KEY_IS_STRING)
			efree(key->value.str.val);
		
		zend_hash_move_forward(target_hash);
    }
	efree(key);
	
	return 0;
}

/* {{{ proto int array_walk(array input, string funcname [, mixed userdata])
   Apply a user function to every member of an array */
PHP_FUNCTION(array_walk) {
	int    argc;
	zval **array,
		 **userdata = NULL,
		 **old_walk_func_name;
	HashTable *target_hash;
	BLS_FETCH();

	argc = ZEND_NUM_ARGS();
	old_walk_func_name = BG(array_walk_func_name);
	if (argc < 2 || argc > 3 ||
		zend_get_parameters_ex(argc, &array, &BG(array_walk_func_name), &userdata) == FAILURE) {
		BG(array_walk_func_name) = old_walk_func_name;
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Wrong datatype in array_walk() call");
		BG(array_walk_func_name) = old_walk_func_name;
		RETURN_FALSE;
	}
	convert_to_string_ex(BG(array_walk_func_name));
	php_array_walk(target_hash, userdata);
	BG(array_walk_func_name) = old_walk_func_name;
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool in_array(mixed needle, array haystack [, bool strict])
   Checks if the given value exists in the array */
PHP_FUNCTION(in_array)
{
	zval **value,				/* value to check for */
		 **array,				/* array to check in */
		 **strict,				/* strict comparison or not */
		 **entry,				/* pointer to array entry */
		  res;					/* comparison result */
	HashTable *target_hash;		/* array hashtable */
	HashPosition pos;			/* hash iterator */
	int (*compare_func)(zval *, zval *, zval *) = is_equal_function;

	if (ZEND_NUM_ARGS() < 2 || ZEND_NUM_ARGS() > 3 ||
		zend_get_parameters_ex(ZEND_NUM_ARGS(), &value, &array, &strict) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	if ((*value)->type == IS_ARRAY || (*value)->type == IS_OBJECT) {
		php_error(E_WARNING, "Wrong datatype for first argument in call to in_array()");
		RETURN_FALSE;
	}
	
	if ((*array)->type != IS_ARRAY) {
		php_error(E_WARNING, "Wrong datatype for second argument in call to in_array()");
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() == 3) {
		convert_to_boolean_ex(strict);
		if (Z_LVAL_PP(strict) == 1)
			compare_func = is_identical_function;
	}

	target_hash = HASH_OF(*array);
	zend_hash_internal_pointer_reset_ex(target_hash, &pos);
	while(zend_hash_get_current_data_ex(target_hash, (void **)&entry, &pos) == SUCCESS) {
     	compare_func(&res, *value, *entry);
		if (Z_LVAL(res) == 1) {
			RETURN_TRUE;
		}
		
		zend_hash_move_forward_ex(target_hash, &pos);
	}
	
	RETURN_FALSE;
}
/* }}} */


/* {{{ int _valid_var_name(char *varname) */
static int _valid_var_name(char *varname)
{
	int len, i;
	
	if (!varname)
		return 0;
	
	len = strlen(varname);
	
	if (!isalpha((int)varname[0]) && varname[0] != '_')
		return 0;
	
	if (len > 1) {
		for(i=1; i<len; i++) {
			if (!isalnum((int)varname[i]) && varname[i] != '_') {
				return 0;
			}
		}
	}
	
	return 1;
}
/* }}} */


/* {{{ proto void extract(array var_array, int extract_type [, string prefix])
   Imports variables into symbol table from an array */
PHP_FUNCTION(extract)
{
	zval **var_array, **etype, **prefix;
	zval **entry, *data;
	char *varname, *finalname;
	ulong lkey;
	int res, extype;

	switch(ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &var_array) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			extype = EXTR_OVERWRITE;
			break;

		case 2:
			if (zend_get_parameters_ex(2, &var_array, &etype) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_long_ex(etype);
			extype = (*etype)->value.lval;
			if (extype > EXTR_SKIP && extype <= EXTR_PREFIX_ALL) {
				WRONG_PARAM_COUNT;
			}
			break;
			
		case 3:
			if (zend_get_parameters_ex(3, &var_array, &etype, &prefix) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_long_ex(etype);
			extype = (*etype)->value.lval;
			convert_to_string_ex(prefix);
			break;

		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	if (extype < EXTR_OVERWRITE || extype > EXTR_PREFIX_ALL) {
		php_error(E_WARNING, "Wrong argument in call to extract()");
		RETURN_FALSE;
	}
	
	if ((*var_array)->type != IS_ARRAY) {
		php_error(E_WARNING, "Wrong datatype in call to extract()");
		RETURN_FALSE;
	}
		
	zend_hash_internal_pointer_reset((*var_array)->value.ht);
	while(zend_hash_get_current_data((*var_array)->value.ht, (void **)&entry) == SUCCESS) {

		if (zend_hash_get_current_key((*var_array)->value.ht, &varname, &lkey) == HASH_KEY_IS_STRING) {

			if (_valid_var_name(varname)) {
				finalname = NULL;

				res = zend_hash_exists(EG(active_symbol_table), varname, strlen(varname)+1);
				switch (extype) {
					case EXTR_OVERWRITE:
						finalname = estrdup(varname);
						break;

					case EXTR_PREFIX_SAME:
						if (!res)
							finalname = estrdup(varname);
						/* break omitted intentionally */

					case EXTR_PREFIX_ALL:
						if (!finalname) {
							finalname = emalloc(strlen(varname) + (*prefix)->value.str.len + 2);
							strcpy(finalname, (*prefix)->value.str.val);
							strcat(finalname, "_");
							strcat(finalname, varname);
						}
						break;

					default:
						if (!res)
							finalname = estrdup(varname);
						break;
				}

				if (finalname) {
					MAKE_STD_ZVAL(data);
					*data = **entry;
					zval_copy_ctor(data);

					ZEND_SET_SYMBOL(EG(active_symbol_table), finalname, data);
					efree(finalname);
				}
			}

			efree(varname);
		}

		zend_hash_move_forward((*var_array)->value.ht);
	}
}
/* }}} */


/* {{{ void _compact_var(HashTable *eg_active_symbol_table, zval *return_value, zval *entry) */
static void _compact_var(HashTable *eg_active_symbol_table, zval *return_value, zval *entry)
{
	zval **value_ptr, *value, *data;
	
	if (entry->type == IS_STRING) {
		if (zend_hash_find(eg_active_symbol_table, entry->value.str.val,
						   entry->value.str.len+1, (void **)&value_ptr) != FAILURE) {
			value = *value_ptr;
			ALLOC_ZVAL(data);
			*data = *value;
			zval_copy_ctor(data);
			INIT_PZVAL(data);
			
			zend_hash_update(return_value->value.ht, entry->value.str.val,
							 entry->value.str.len+1, &data, sizeof(zval *), NULL);
		}
	}
	else if (entry->type == IS_ARRAY) {
		zend_hash_internal_pointer_reset(entry->value.ht);

		while(zend_hash_get_current_data(entry->value.ht, (void**)&value_ptr) == SUCCESS) {
			value = *value_ptr;

			_compact_var(eg_active_symbol_table, return_value, value);
			zend_hash_move_forward(entry->value.ht);
		}
	}
}
/* }}} */


/* {{{ proto array compact(mixed var_names [, mixed ... ])
   Creates a hash containing variables and their values */
PHP_FUNCTION(compact)
{
	zval ***args;			/* function arguments array */
	int i;
	
	args = (zval ***)emalloc(ZEND_NUM_ARGS() * sizeof(zval **));
	
	if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), args) == FAILURE) {
		efree(args);
		WRONG_PARAM_COUNT;
	}

	array_init(return_value);
	
	for (i=0; i<ZEND_NUM_ARGS(); i++)
	{
		_compact_var(EG(active_symbol_table), return_value, *args[i]);
	}
	
	efree(args);
}
/* }}} */

/* {{{ proto array range(int low, int high)
   Create an array containing the range of integers from low to high (inclusive) */
PHP_FUNCTION(range)
{
	zval **zlow, **zhigh;
	int low, high;
	
	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2,&zlow,&zhigh) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(zlow);
	convert_to_long_ex(zhigh);
	low = (*zlow)->value.lval;
	high = (*zhigh)->value.lval;

    /* allocate an array for return */
    if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
    }

	for (; low <= high; low++) {
		add_next_index_long(return_value, low);
	}	
}
/* }}} */


static int array_data_shuffle(const void *a, const void*b) {
	return (
	/* This is just a little messy. */
#ifdef HAVE_LRAND48
        lrand48()
#else
#ifdef HAVE_RANDOM
        random()
#else
        rand()
#endif
#endif
	% 2) ? 1 : -1;
}


/* {{{ proto int shuffle(array array_arg)
   Randomly shuffle the contents of an array */
PHP_FUNCTION(shuffle)
{
	zval **array;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	if ((*array)->type != IS_ARRAY) {
		php_error(E_WARNING, "Wrong datatype in shuffle() call");
		RETURN_FALSE;
	}
	if (zend_hash_sort((*array)->value.ht, (sort_func_t)mergesort, array_data_shuffle, 1) == FAILURE) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */


/* HashTable* php_splice(HashTable *in_hash, int offset, int length,
						 zval ***list, int list_count, HashTable **removed) */
HashTable* php_splice(HashTable *in_hash, int offset, int length,
					  zval ***list, int list_count, HashTable **removed)
{
	HashTable 	*out_hash = NULL;	/* Output hashtable */
	int			 num_in,			/* Number of entries in the input hashtable */
				 pos,				/* Current position in the hashtable */
				 i;					/* Loop counter */
	Bucket		*p;					/* Pointer to hash bucket */
	zval		*entry;				/* Hash entry */
	
	/* If input hash doesn't exist, we have nothing to do */
	if (!in_hash)
		return NULL;
	
	/* Get number of entries in the input hash */
	num_in = zend_hash_num_elements(in_hash);
	
	/* Clamp the offset.. */
	if (offset > num_in)
		offset = num_in;
	else if (offset < 0 && (offset=num_in+offset) < 0)
		offset = 0;
	
	/* ..and the length */
	if (length < 0)
		length = num_in-offset+length;
	else if(offset+length > num_in)
		length = num_in-offset;

	/* Create and initialize output hash */
	out_hash = (HashTable *)emalloc(sizeof(HashTable));
	zend_hash_init(out_hash, 0, NULL, ZVAL_PTR_DTOR, 0);
	
	/* Start at the beginning of the input hash and copy
	   entries to output hash until offset is reached */
	for (pos=0, p=in_hash->pListHead; pos<offset && p ; pos++, p=p->pListNext) {
		/* Get entry and increase reference count */
		entry = *((zval **)p->pData);
		entry->refcount++;
		
		/* Update output hash depending on key type */
		if (p->nKeyLength)
			zend_hash_update(out_hash, p->arKey, p->nKeyLength, &entry, sizeof(zval *), NULL);
		else
			zend_hash_next_index_insert(out_hash, &entry, sizeof(zval *), NULL);
	}
	
	/* If hash for removed entries exists, go until offset+length
	   and copy the entries to it */
	if (removed != NULL) {
		for( ; pos<offset+length && p; pos++, p=p->pListNext) {
			entry = *((zval **)p->pData);
			entry->refcount++;
			if (p->nKeyLength)
				zend_hash_update(*removed, p->arKey, p->nKeyLength, &entry, sizeof(zval *), NULL);
			else
				zend_hash_next_index_insert(*removed, &entry, sizeof(zval *), NULL);
		}
	} else /* otherwise just skip those entries */
		for( ; pos<offset+length && p; pos++, p=p->pListNext);
	
	/* If there are entries to insert.. */
	if (list != NULL) {
		/* ..for each one, create a new zval, copy entry into it
		   and copy it into the output hash */
		for (i=0; i<list_count; i++) {
			entry = *list[i];
			entry->refcount++;
			zend_hash_next_index_insert(out_hash, &entry, sizeof(zval *), NULL);
		}
	}
	
	/* Copy the remaining input hash entries to the output hash */
	for ( ; p ; p=p->pListNext) {
		entry = *((zval **)p->pData);
		entry->refcount++;
		if (p->nKeyLength)
			zend_hash_update(out_hash, p->arKey, p->nKeyLength, &entry, sizeof(zval *), NULL);
		else
			zend_hash_next_index_insert(out_hash, &entry, sizeof(zval *), NULL);
	}

	zend_hash_internal_pointer_reset(out_hash);
	return out_hash;
}
/* }}} */


/* {{{ proto int array_push(array stack, mixed var [, mixed ...])
   Pushes elements onto the end of the array */
PHP_FUNCTION(array_push)
{
	zval	  ***args,		/* Function arguments array */
			    *stack,		/* Input array */
			    *new_var;	/* Variable to be pushed */
	int			 i,			/* Loop counter */
				 argc;		/* Number of function arguments */

	/* Get the argument count and check it */
	argc = ZEND_NUM_ARGS();
	if (argc < 2) {
		WRONG_PARAM_COUNT;
	}
	
	/* Allocate arguments array and get the arguments, checking for errors. */
	args = (zval ***)emalloc(argc * sizeof(zval **));
	if (zend_get_parameters_array_ex(argc, args) == FAILURE) {
		efree(args);
		WRONG_PARAM_COUNT;
	}

	/* Get first argument and check that it's an array */	
	stack = *args[0];
	if (stack->type != IS_ARRAY) {
		php_error(E_WARNING, "First argument to array_push() needs to be an array");
		RETURN_FALSE;
	}

	/* For each subsequent argument, make it a reference, increase refcount,
	   and add it to the end of the array */
	for (i=1; i<argc; i++) {
		new_var = *args[i];
		new_var->refcount++;
	
		zend_hash_next_index_insert(stack->value.ht, &new_var, sizeof(zval *), NULL);
	}
	
	/* Clean up and return the number of values in the stack */
	efree(args);
	RETVAL_LONG(zend_hash_num_elements(stack->value.ht));
}
/* }}} */


/* {{{ void _phpi_pop(INTERNAL_FUNCTION_PARAMETERS, int which_end) */
static void _phpi_pop(INTERNAL_FUNCTION_PARAMETERS, int off_the_end)
{
	zval	   **stack,			/* Input stack */
			   **val;			/* Value to be popped */
	HashTable	*new_hash;		/* New stack */
	
	/* Get the arguments and do error-checking */
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &stack) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	if ((*stack)->type != IS_ARRAY) {
		php_error(E_WARNING, "The argument needs to be an array");
		return;
	}

	if (zend_hash_num_elements((*stack)->value.ht) == 0) {
		return;
	}
		
	/* Get the first or last value and copy it into the return value */
	if (off_the_end)
		zend_hash_internal_pointer_end((*stack)->value.ht);
	else
		zend_hash_internal_pointer_reset((*stack)->value.ht);
	zend_hash_get_current_data((*stack)->value.ht, (void **)&val);
	*return_value = **val;
	zval_copy_ctor(return_value);
	INIT_PZVAL(return_value);
	
	/* Delete the first or last value */
	new_hash = php_splice((*stack)->value.ht, (off_the_end) ? -1 : 0, 1, NULL, 0, NULL);
	zend_hash_destroy((*stack)->value.ht);
	efree((*stack)->value.ht);
	(*stack)->value.ht = new_hash;
}
/* }}} */


/* {{{ proto mixed array_pop(array stack)
   Pops an element off the end of the array */
PHP_FUNCTION(array_pop)
{
	_phpi_pop(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */


/* {{{ proto mixed array_shift(array stack)
   Pops an element off the beginning of the array */
PHP_FUNCTION(array_shift)
{
	_phpi_pop(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */


/* {{{ proto int array_unshift(array stack, mixed var [, mixed ...])
   Pushes elements onto the beginning of the array */
PHP_FUNCTION(array_unshift)
{
	zval	  ***args,		/* Function arguments array */
				*stack;		/* Input stack */
	HashTable	*new_hash;	/* New hashtable for the stack */
	int			 argc;		/* Number of function arguments */
	

	/* Get the argument count and check it */	
	argc = ZEND_NUM_ARGS();
	if (argc < 2) {
		WRONG_PARAM_COUNT;
	}
	
	/* Allocate arguments array and get the arguments, checking for errors. */
	args = (zval ***)emalloc(argc * sizeof(zval **));
	if (zend_get_parameters_array_ex(argc, args) == FAILURE) {
		efree(args);
		WRONG_PARAM_COUNT;
	}
	
	/* Get first argument and check that it's an array */
	stack = *args[0];
	if (stack->type != IS_ARRAY) {
		php_error(E_WARNING, "First argument to array_unshift() needs to be an array");
		RETURN_FALSE;
	}

	/* Use splice to insert the elements at the beginning.  Destroy old
	   hashtable and replace it with new one */
	new_hash = php_splice(stack->value.ht, 0, 0, &args[1], argc-1, NULL);
	zend_hash_destroy(stack->value.ht);
	efree(stack->value.ht);
	stack->value.ht = new_hash;

	/* Clean up and return the number of elements in the stack */
	efree(args);
	RETVAL_LONG(zend_hash_num_elements(stack->value.ht));
}
/* }}} */


/* {{{ proto array array_splice(array input, int offset [, int length [, array replacement]])
   Removes the elements designated by offset and length and replace them with supplied array */
PHP_FUNCTION(array_splice)
{
	zval	  ***args,				/* Function arguments array */
				*array,				/* Input array */
			  ***repl = NULL;		/* Replacement elements */
	HashTable	*new_hash = NULL;	/* Output array's hash */
	Bucket		*p;					/* Bucket used for traversing hash */
	int			 argc,				/* Number of function arguments */
				 i,
				 offset,
				 length,
				 repl_num = 0;		/* Number of replacement elements */

	/* Get the argument count and check it */
	argc = ZEND_NUM_ARGS();
	if (argc < 2 || argc > 4) {
		WRONG_PARAM_COUNT;
	}
	
	/* Allocate arguments array and get the arguments, checking for errors. */
	args = (zval ***)emalloc(argc * sizeof(zval **));
	if (zend_get_parameters_array_ex(argc, args) == FAILURE) {
		efree(args);
		WRONG_PARAM_COUNT;
	}	

	/* Get first argument and check that it's an array */
	array = *args[0];
	if (array->type != IS_ARRAY) {
		php_error(E_WARNING, "First argument to array_splice() should be an array");
		efree(args);
		return;
	}
	
	/* Get the next two arguments.  If length is omitted,
	   it's assumed to be until the end of the array */
	convert_to_long_ex(args[1]);
	offset = (*args[1])->value.lval;
	if (argc > 2) {
		convert_to_long_ex(args[2]);
		length = (*args[2])->value.lval;
	} else
		length = zend_hash_num_elements(array->value.ht);

	if (argc == 4) {
		/* Make sure the last argument, if passed, is an array */
		convert_to_array_ex(args[3]);
		
		/* Create the array of replacement elements */
		repl_num = zend_hash_num_elements((*args[3])->value.ht);
		repl = (zval ***)emalloc(repl_num * sizeof(zval **));
		for (p=(*args[3])->value.ht->pListHead, i=0; p; p=p->pListNext, i++) {
			repl[i] = ((zval **)p->pData);
		}
	}
	
	/* Initialize return value */
	array_init(return_value);
	
	/* Perform splice */
	new_hash = php_splice(array->value.ht, offset, length,
							repl, repl_num,
							&return_value->value.ht);
	
	/* Replace input array's hashtable with the new one */
	zend_hash_destroy(array->value.ht);
	efree(array->value.ht);
	array->value.ht = new_hash;
	
	/* Clean up */
	if (argc == 4)
		efree(repl);
	efree(args);
}
/* }}} */


/* {{{ proto array array_slice(array input, int offset [, int length])
   Returns elements specified by offset and length */
PHP_FUNCTION(array_slice)
{
	zval	   **input,			/* Input array */
			   **offset,		/* Offset to get elements from */
			   **length,		/* How many elements to get */
			   **entry;			/* An array entry */
	int			 offset_val,	/* Value of the offset argument */
				 length_val,	/* Value of the length argument */
				 num_in,		/* Number of elements in the input array */
				 pos,			/* Current position in the array */
				 argc;			/* Number of function arguments */
				 
	char		*string_key;
	ulong		 num_key;
	

	/* Get the arguments and do error-checking */	
	argc = ZEND_NUM_ARGS();
	if (argc < 2 || argc > 3 || zend_get_parameters_ex(argc, &input, &offset, &length)) {
		WRONG_PARAM_COUNT;
	}
	
	if ((*input)->type != IS_ARRAY) {
		php_error(E_WARNING, "First argument to array_slice() should be an array");
		return;
	}
	
	/* Make sure offset and length are integers and assume
	   we want all entries from offset to the end if length
	   is not passed */
	convert_to_long_ex(offset);
	offset_val = (*offset)->value.lval;
	if (argc == 3) {
		convert_to_long_ex(length);
		length_val = (*length)->value.lval;
	} else
		length_val = zend_hash_num_elements((*input)->value.ht);
	
	/* Initialize returned array */
	array_init(return_value);
	
	/* Get number of entries in the input hash */
	num_in = zend_hash_num_elements((*input)->value.ht);
	
	/* Clamp the offset.. */
	if (offset_val > num_in)
		return;
	else if (offset_val < 0 && (offset_val=num_in+offset_val) < 0)
		offset_val = 0;
	
	/* ..and the length */
	if (length_val < 0)
		length_val = num_in-offset_val+length_val;
	else if(offset_val+length_val > num_in)
		length_val = num_in-offset_val;
	
	if (length_val == 0)
		return;
	
	/* Start at the beginning and go until we hit offset */
	pos = 0;
	zend_hash_internal_pointer_reset((*input)->value.ht);
	while(pos < offset_val &&
		  zend_hash_get_current_data((*input)->value.ht, (void **)&entry) == SUCCESS) {
		pos++;
		zend_hash_move_forward((*input)->value.ht);
	}
	
	/* Copy elements from input array to the one that's returned */
	while(pos < offset_val+length_val &&
		  zend_hash_get_current_data((*input)->value.ht, (void **)&entry) == SUCCESS) {
		
		(*entry)->refcount++;

		switch (zend_hash_get_current_key((*input)->value.ht, &string_key, &num_key)) {
			case HASH_KEY_IS_STRING:
				zend_hash_update(return_value->value.ht, string_key, strlen(string_key)+1,
								 entry, sizeof(zval *), NULL);
				efree(string_key);
				break;
	
			case HASH_KEY_IS_LONG:
				zend_hash_next_index_insert(return_value->value.ht,
											entry, sizeof(zval *), NULL);
				break;
		}
		pos++;
		zend_hash_move_forward((*input)->value.ht);
	}
}
/* }}} */


static void php_array_merge_impl(HashTable *dest, HashTable *src, int recursive)
{
	zval	  **src_entry,
			  **dest_entry;
	char	   *string_key;
	ulong		num_key;

	zend_hash_internal_pointer_reset(src);
	while(zend_hash_get_current_data(src, (void **)&src_entry) == SUCCESS) {
		switch (zend_hash_get_current_key(src, &string_key, &num_key)) {
			case HASH_KEY_IS_STRING:
				if (recursive &&
					zend_hash_find(dest, string_key, strlen(string_key) + 1,
								   (void **)&dest_entry) == SUCCESS) {
					convert_to_array_ex(dest_entry);
					convert_to_array_ex(src_entry);
					php_array_merge_impl(Z_ARRVAL_PP(dest_entry),
										 Z_ARRVAL_PP(src_entry), recursive);
				} else {
					(*src_entry)->refcount++;

					zend_hash_update(dest, string_key, strlen(string_key)+1,
									 src_entry, sizeof(zval *), NULL);
				}
				efree(string_key);
				break;

			case HASH_KEY_IS_LONG:
				(*src_entry)->refcount++;
				zend_hash_next_index_insert(dest, src_entry, sizeof(zval *), NULL);
				break;
		}

		zend_hash_move_forward(src);
	}
}

static void php_array_merge(INTERNAL_FUNCTION_PARAMETERS, int recursive)
{
	zval	  ***args = NULL;
	int			 argc,
				 i;

	/* Get the argument count and check it */	
	argc = ZEND_NUM_ARGS();
	if (argc < 2) {
		WRONG_PARAM_COUNT;
	}
	
	/* Allocate arguments array and get the arguments, checking for errors. */
	args = (zval ***)emalloc(argc * sizeof(zval **));
	if (zend_get_parameters_array_ex(argc, args) == FAILURE) {
		efree(args);
		WRONG_PARAM_COUNT;
	}
	
	array_init(return_value);
	
	for (i=0; i<argc; i++) {
		convert_to_array_ex(args[i]);
		php_array_merge_impl(Z_ARRVAL_P(return_value), Z_ARRVAL_PP(args[i]), recursive);
	}
	
	efree(args);
}


/* {{{ proto array array_merge(array arr1, array arr2 [, mixed ...])
   Merges elements from passed arrays into one array */
PHP_FUNCTION(array_merge)
{
	php_array_merge(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */


/* {{{ proto array array_merge_recursive(array arr1, array arr2 [, mixed ...])
   Recursively merges elements from passed arrays into one array */
PHP_FUNCTION(array_merge_recursive)
{
	php_array_merge(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */


/* {{{ proto array array_keys(array input [, mixed search_value])
   Return just the keys from the input array, optionally only for the specified search_value */
PHP_FUNCTION(array_keys)
{
	zval	   **input,			/* Input array */
			   **search_value,	/* Value to search for */
			   **entry,			/* An entry in the input array */
				 res,			/* Result of comparison */
				*new_val;		/* New value */
	int			 add_key;		/* Flag to indicate whether a key should be added */
	char		*string_key;	/* String key */
	ulong		 num_key;		/* Numeric key */

	search_value = NULL;
	
	/* Get arguments and do error-checking */
	if (ZEND_NUM_ARGS() < 1 || ZEND_NUM_ARGS() > 2 ||
		zend_get_parameters_ex(ZEND_NUM_ARGS(), &input, &search_value) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	if ((*input)->type != IS_ARRAY) {
		php_error(E_WARNING, "First argument to array_keys() should be an array");
		return;
	}
	
	/* Initialize return array */
	array_init(return_value);
	add_key = 1;
	
	/* Go through input array and add keys to the return array */
	zend_hash_internal_pointer_reset((*input)->value.ht);
	while(zend_hash_get_current_data((*input)->value.ht, (void **)&entry) == SUCCESS) {
		if (search_value != NULL) {
     		is_equal_function(&res, *search_value, *entry);
			add_key = zval_is_true(&res);
		}
	
		if (add_key) {	
			MAKE_STD_ZVAL(new_val);

			switch (zend_hash_get_current_key((*input)->value.ht, &string_key, &num_key)) {
				case HASH_KEY_IS_STRING:
					new_val->type = IS_STRING;
					new_val->value.str.val = string_key;
					new_val->value.str.len = strlen(string_key);
					zend_hash_next_index_insert(return_value->value.ht, &new_val,
												sizeof(zval *), NULL);
					break;

				case HASH_KEY_IS_LONG:
					new_val->type = IS_LONG;
					new_val->value.lval = num_key;
					zend_hash_next_index_insert(return_value->value.ht, &new_val,
												sizeof(zval *), NULL);
					break;
			}
		}

		zend_hash_move_forward((*input)->value.ht);
	}
}
/* }}} */


/* {{{ proto array array_values(array input)
   Return just the values from the input array */
PHP_FUNCTION(array_values)
{
	zval	   **input,		/* Input array */
			   **entry;		/* An entry in the input array */
	
	/* Get arguments and do error-checking */
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(ZEND_NUM_ARGS(), &input) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	if ((*input)->type != IS_ARRAY) {
		php_error(E_WARNING, "Argument to array_values() should be an array");
		return;
	}
	
	/* Initialize return array */
	array_init(return_value);

	/* Go through input array and add values to the return array */	
	zend_hash_internal_pointer_reset((*input)->value.ht);
	while(zend_hash_get_current_data((*input)->value.ht, (void **)&entry) == SUCCESS) {
		
		(*entry)->refcount++;
		zend_hash_next_index_insert(return_value->value.ht, entry,
											sizeof(zval *), NULL);

		zend_hash_move_forward((*input)->value.ht);
	}
}
/* }}} */


/* {{{ proto array array_count_values(array input)
   Return the value as key and the frequency of that value in <input> as value */
PHP_FUNCTION(array_count_values)
{
	zval	   **input,		/* Input array */
			   **entry;		/* An entry in the input array */
	zval       **tmp;
	HashTable   *myht;
	
	/* Get arguments and do error-checking */
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &input) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	if ((*input)->type != IS_ARRAY) {
		php_error(E_WARNING, "Argument to array_count_values() should be an array");
		return;
	}
	
	/* Initialize return array */
	array_init(return_value);

	/* Go through input array and add values to the return array */	
	myht = (*input)->value.ht;
	zend_hash_internal_pointer_reset(myht);
	while (zend_hash_get_current_data(myht, (void **)&entry) == SUCCESS) {
		if ((*entry)->type == IS_LONG) {
			if (zend_hash_index_find(return_value->value.ht, 
									 (*entry)->value.lval, 
									 (void**)&tmp) == FAILURE) {
				zval *data;
				MAKE_STD_ZVAL(data);
				data->type = IS_LONG;
				data->value.lval = 1;
				zend_hash_index_update(return_value->value.ht,(*entry)->value.lval, &data, sizeof(data), NULL);
			} else {
				(*tmp)->value.lval++;
			}
		} else if ((*entry)->type == IS_STRING) {
			if (zend_hash_find(return_value->value.ht, 
							   (*entry)->value.str.val, 
							   (*entry)->value.str.len+1, 
							   (void**)&tmp) == FAILURE) {
				zval *data;
				MAKE_STD_ZVAL(data);
				data->type = IS_LONG;
				data->value.lval = 1;
				zend_hash_update(return_value->value.ht,(*entry)->value.str.val,(*entry)->value.str.len + 1, &data, sizeof(data), NULL);
			} else {
				(*tmp)->value.lval++;
			}
		} else {
			php_error(E_WARNING, "Can only count STRING and INTEGER values!");
		}

		zend_hash_move_forward(myht);
	}
}
/* }}} */


/* {{{ proto array array_reverse(array input)
   Return input as a new array with the order of the entries reversed */
PHP_FUNCTION(array_reverse)
{
	zval	   **input,			/* Input array */
			   **entry;			/* An entry in the input array */
	char		*string_key;
	ulong		 num_key;
	
	/* Get arguments and do error-checking */
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &input) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	if ((*input)->type != IS_ARRAY) {
		php_error(E_WARNING, "Argument to array_reverse() should be an array");
		return;
	}
	
	/* Initialize return array */
	array_init(return_value);
	
	zend_hash_internal_pointer_end((*input)->value.ht);
	while(zend_hash_get_current_data((*input)->value.ht, (void **)&entry) == SUCCESS) {
		(*entry)->refcount++;
		
		switch (zend_hash_get_current_key((*input)->value.ht, &string_key, &num_key)) {
			case HASH_KEY_IS_STRING:
				zend_hash_update(return_value->value.ht, string_key, strlen(string_key)+1,
								 entry, sizeof(zval *), NULL);
				efree(string_key);
				break;

			case HASH_KEY_IS_LONG:
				zend_hash_next_index_insert(return_value->value.ht,
											entry, sizeof(zval *), NULL);
				break;
		}
		
		zend_hash_move_backwards((*input)->value.ht);
	}
}
/* }}} */


/* {{{ proto array array_pad(array input, int pad_size, mixed pad_value)
   Returns a copy of input array padded with pad_value to size pad_size */
PHP_FUNCTION(array_pad)
{
	zval		**input;		/* Input array */
	zval		**pad_size;		/* Size to pad to */
	zval		**pad_value;	/* Padding value obviously */
	zval	   ***pads;			/* Array to pass to splice */
	HashTable	 *new_hash;		/* Return value from splice */
	int			  input_size;	/* Size of the input array */
	int			  pad_size_abs; /* Absolute value of pad_size */
	int			  num_pads;		/* How many pads do we need */
	int			  do_pad;		/* Whether we should do padding at all */
	int			  i;
	
	/* Get arguments and do error-checking */
	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &input, &pad_size, &pad_value) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	/* Make sure arguments are of the proper type */
	if ((*input)->type != IS_ARRAY) {
		php_error(E_WARNING, "Argument to %s() should be an array",
				  get_active_function_name());
		return;
	}
	convert_to_long_ex(pad_size);
	
	/* Do some initial calculations */
	input_size = zend_hash_num_elements((*input)->value.ht);
	pad_size_abs = abs((*pad_size)->value.lval);
	do_pad = (input_size >= pad_size_abs) ? 0 : 1;
	
	/* Copy the original array */
	*return_value = **input;
	zval_copy_ctor(return_value);
	
	/* If no need to pad, no need to continue */
	if (!do_pad)
		return;
	
	/* Populate the pads array */
	num_pads = pad_size_abs - input_size;
	pads = (zval ***)emalloc(num_pads * sizeof(zval **));
	for (i = 0; i < num_pads; i++)
		pads[i] = pad_value;
	
	/* Pad on the right or on the left */
	if ((*pad_size)->value.lval > 0)
		new_hash = php_splice(return_value->value.ht, input_size, 0, pads, num_pads, NULL);
	else
		new_hash = php_splice(return_value->value.ht, 0, 0, pads, num_pads, NULL);

	
	/* Copy the result hash into return value */
	zend_hash_destroy(return_value->value.ht);
	efree(return_value->value.ht);
	return_value->value.ht = new_hash;
	
	/* Clean up */
	efree(pads);
}
/* }}} */

/* {{{ proto array array_flip(array input)
   Return array with key <-> value flipped */
PHP_FUNCTION(array_flip)
{
	zval **array, **entry, *data;
	HashTable *target_hash;
	char *string_key;
	ulong num_key;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Wrong datatype in array_flip() call");
		RETURN_FALSE;
	}
	
	array_init(return_value);

	zend_hash_internal_pointer_reset(target_hash);
	while (zend_hash_get_current_data(target_hash, (void **)&entry) == SUCCESS) {
		MAKE_STD_ZVAL(data);
		switch (zend_hash_get_current_key(target_hash, &string_key, &num_key)) {
			case HASH_KEY_IS_STRING:
				data->value.str.val = string_key;
				data->value.str.len = strlen(string_key);
				data->type = IS_STRING;
				break;
			case HASH_KEY_IS_LONG:
				data->type = IS_LONG;
				data->value.lval = num_key;
				break;
		}

		if ((*entry)->type == IS_LONG) {
			zend_hash_index_update(return_value->value.ht,(*entry)->value.lval, &data, sizeof(data), NULL);
		} else if ((*entry)->type == IS_STRING) {
			zend_hash_update(return_value->value.ht,(*entry)->value.str.val,(*entry)->value.str.len + 1, &data, sizeof(data), NULL);
		} else {
			zval_dtor(data);
			php_error(E_WARNING, "Can only flip STRING and INTEGER values!");
		}
	
		zend_hash_move_forward(target_hash);
	}
}
/* }}} */

/* {{{ proto array array_unique(array input)
   Removes duplicate values from array */
PHP_FUNCTION(array_unique)
{
	zval **array;
	HashTable *target_hash;
	Bucket **arTmp, **cmpdata, **lastkept;
	Bucket *p;
	int i;

	if (ARG_COUNT(ht) != 1 || zend_get_parameters_ex(1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Wrong datatype in array_unique() call");
		RETURN_FALSE;
	}

	/* copy the argument array */
	*return_value = **array;
	zval_copy_ctor(return_value);

	if (target_hash->nNumOfElements <= 1) /* nothing to do */
	        return;

	/* create and sort array with pointers to the target_hash buckets */
	arTmp = (Bucket **) pemalloc((target_hash->nNumOfElements + 1) * sizeof(Bucket *), target_hash->persistent);
	if (!arTmp)
		RETURN_FALSE;
	for (i = 0, p = target_hash->pListHead; p; i++, p = p->pListNext)
		arTmp[i] = p;
	arTmp[i] = NULL;
    set_compare_func(SORT_REGULAR);
	qsort((void *) arTmp, i, sizeof(Bucket *), array_data_compare);

	/* go through the sorted array and delete duplicates from the copy */
	lastkept = arTmp;
	for (cmpdata = arTmp + 1; *cmpdata; cmpdata++) {
		if (array_data_compare(lastkept, cmpdata)) {
		        lastkept = cmpdata;
		} else {
			p = *cmpdata;
			if (p->nKeyLength)
			        zend_hash_del(return_value->value.ht, p->arKey, p->nKeyLength);  
			else
			        zend_hash_index_del(return_value->value.ht, p->h);  
		}
	}
	pefree(arTmp, target_hash->persistent);
}
/* }}} */

/* {{{ proto array array_intersect(array arr1, array arr2 [, mixed ...])
   Returns the entries of arr1 that have values which are present in all the other arguments */
PHP_FUNCTION(array_intersect)
{
        zval ***args = NULL;
	HashTable *hash;
	int argc, i, c = 0;
	Bucket ***lists, **list, ***ptrs, *p;
	zval *entry;

	/* Get the argument count and check it */	
	argc = ARG_COUNT(ht);
	if (argc < 2) {
		WRONG_PARAM_COUNT;
	}
	/* Allocate arguments array and get the arguments, checking for errors. */
	args = (zval ***)emalloc(argc * sizeof(zval **));
	if (zend_get_parameters_array_ex(argc, args) == FAILURE) {
		efree(args);
		WRONG_PARAM_COUNT;
	}
	array_init(return_value);
	/* for each argument, create and sort list with pointers to the hash buckets */
	lists = (Bucket ***)emalloc(argc * sizeof(Bucket **));
	ptrs = (Bucket ***)emalloc(argc * sizeof(Bucket **));
    set_compare_func(SORT_REGULAR);
	for (i=0; i<argc; i++) {
		if ((*args[i])->type != IS_ARRAY) {
			php_error(E_WARNING, "Argument #%d to array_intersect() is not an array", i+1);
			argc = i; /* only free up to i-1 */
			goto out;
		}
		hash = HASH_OF(*args[i]);
		list = (Bucket **) pemalloc((hash->nNumOfElements + 1) * sizeof(Bucket *), hash->persistent);
		if (!list)
		        RETURN_FALSE;
		lists[i] = list;
		ptrs[i] = list;
		for (p = hash->pListHead; p; p = p->pListNext)
		        *list++ = p;
		*list = NULL;
		qsort((void *) lists[i], hash->nNumOfElements, sizeof(Bucket *), array_data_compare);
	}
	/* go through the lists and look for common values */
	while (*ptrs[0]) {
		for (i=1; i<argc; i++) {
		        while (*ptrs[i] && (0 < (c = array_data_compare(ptrs[0], ptrs[i]))))
			        ptrs[i]++;
			if (!*ptrs[i])
			        goto out;
			if (c)
			        break;
			ptrs[i]++;
		}
		if (c) {
		  /* ptrs[0] not in all arguments, next candidate is ptrs[i] */
		        for (;;) {
		                if (!*++ptrs[0])
			                goto out;
				if (0 <= array_data_compare(ptrs[0], ptrs[i]))
			                break;
			}
		} else {
		        /* ptrs[0] is present in all the arguments */
		        /* Go through all entries with same value as ptrs[0] */
		        for (;;) {
			        p = *ptrs[0];
				entry = *((zval **)p->pData);
				entry->refcount++;
				if (p->nKeyLength)
				        zend_hash_update(return_value->value.ht,
						 p->arKey, p->nKeyLength,
						 &entry, sizeof(zval *),
						 NULL);  
				else
				        zend_hash_index_update(return_value->value.ht,
						       p->h, &entry,
						       sizeof(zval *),
						       NULL);
				if (!*++ptrs[0])
				        goto out;
				if (array_data_compare(ptrs[0]-1, ptrs[0]))
				        break;
			}
		}
	}
out:
	for (i=0; i<argc; i++) {
	        hash = HASH_OF(*args[i]);
		pefree(lists[i], hash->persistent);
	}
	efree(ptrs);
	efree(lists);
	efree(args);
}
/* }}} */

/* {{{ proto array array_diff(array arr1, array arr2 [, mixed ...])
   Returns the entries of arr1 that have values which are not present in
   any of the others arguments */
PHP_FUNCTION(array_diff)
{
        zval ***args = NULL;
	HashTable *hash;
	int argc, i, c;
	Bucket ***lists, **list, ***ptrs, *p;
	zval *entry;

	/* Get the argument count and check it */	
	argc = ARG_COUNT(ht);
	if (argc < 2) {
		WRONG_PARAM_COUNT;
	}
	/* Allocate arguments array and get the arguments, checking for errors. */
	args = (zval ***)emalloc(argc * sizeof(zval **));
	if (zend_get_parameters_array_ex(argc, args) == FAILURE) {
		efree(args);
		WRONG_PARAM_COUNT;
	}
	array_init(return_value);
	/* for each argument, create and sort list with pointers to the hash buckets */
	lists = (Bucket ***)emalloc(argc * sizeof(Bucket **));
	ptrs = (Bucket ***)emalloc(argc * sizeof(Bucket **));
    set_compare_func(SORT_REGULAR);
	for (i=0; i<argc; i++) {
		if ((*args[i])->type != IS_ARRAY) {
			php_error(E_WARNING, "Argument #%d to array_intersect() is not an array", i+1);
			argc = i; /* only free up to i-1 */
			goto out;
		}
		hash = HASH_OF(*args[i]);
		list = (Bucket **) pemalloc((hash->nNumOfElements + 1) * sizeof(Bucket *), hash->persistent);
		if (!list)
		        RETURN_FALSE;
		lists[i] = list;
		ptrs[i] = list;
		for (p = hash->pListHead; p; p = p->pListNext)
		        *list++ = p;
		*list = NULL;
		qsort((void *) lists[i], hash->nNumOfElements, sizeof(Bucket *), array_data_compare);
	}
	/* go through the lists and look for values of ptr[0]
           that are not in the others */
	while (*ptrs[0]) {
	        c = 1;
		for (i=1; i<argc; i++) {
		        while (*ptrs[i] && (0 < (c = array_data_compare(ptrs[0], ptrs[i]))))
			        ptrs[i]++;
			if (!c) {
			        if (*ptrs[i])
				        ptrs[i]++;
			        break;
			}
		}
		if (!c) {
		  /* ptrs[0] in one of the other arguments */
		  /* skip all entries with value as ptrs[0] */
		        for (;;) {
		                if (!*++ptrs[0])
			                goto out;
				if (array_data_compare(ptrs[0]-1, ptrs[0]))
			                break;
			}
		} else {
		        /* ptrs[0] is in none of the other arguments */
		        for (;;) {
			        p = *ptrs[0];
				entry = *((zval **)p->pData);
				entry->refcount++;
				if (p->nKeyLength)
				        zend_hash_update(return_value->value.ht,
						 p->arKey, p->nKeyLength,
						 &entry, sizeof(zval *),
						 NULL);  
				else
				        zend_hash_index_update(return_value->value.ht,
						       p->h, &entry,
						       sizeof(zval *),
						       NULL);
				if (!*++ptrs[0])
				        goto out;
				if (array_data_compare(ptrs[0]-1, ptrs[0]))
				        break;
			}
		}
	}
out:
	for (i=0; i<argc; i++) {
	        hash = HASH_OF(*args[i]);
		pefree(lists[i], hash->persistent);
	}
	efree(ptrs);
	efree(lists);
	efree(args);
}
/* }}} */

int multisort_compare(const void *a, const void *b)
{
	Bucket**	  ab = *(Bucket ***)a;
	Bucket**	  bb = *(Bucket ***)b;
	int			  r;
	int			  result = 0;
	zval		  temp;
	ARRAYLS_FETCH();
	
	r = 0;
	do {
		ARRAYG(compare_func)(&temp, *((zval **)ab[r]->pData), *((zval **)bb[r]->pData));
		result = ARRAYG(multisort_flags)[r] * temp.value.lval;
		if (result != 0)
			return result;
		r++;
	} while (ab[r] != NULL);
	return result;
}

#define MULTISORT_ABORT						\
	efree(ARRAYG(multisort_flags));			\
	efree(arrays);							\
	efree(args);							\
	RETURN_FALSE;

/* {{{ proto bool array_multisort(array ar1 [, SORT_ASC|SORT_DESC] [, array ar2 [, SORT_ASC|SORT_DESC], ...])
   Sort multiple arrays at once similar to how ORDER BY clause works in SQL */
PHP_FUNCTION(array_multisort)
{
	zval***			args;
	zval***			arrays;
	Bucket***		indirect;
	Bucket*			p;
	HashTable*		hash;
	int				argc;
	int				array_size;
	int				num_arrays = 0;
	int				parse_state = 0;      /* 0 - flag not allowed
                                             1 - flag allowed     */
	int				sort_order = SORT_ASC;
	int				i, k;
	ARRAYLS_FETCH();
	
	/* Get the argument count and check it */
	argc = ZEND_NUM_ARGS();
	if (argc < 1) {
		WRONG_PARAM_COUNT;
	}
	
	/* Allocate arguments array and get the arguments, checking for errors. */
	args = (zval ***)emalloc(argc * sizeof(zval **));
	if (zend_get_parameters_array_ex(argc, args) == FAILURE) {
		efree(args);
		WRONG_PARAM_COUNT;
	}

	/* Allocate space for storing pointers to input arrays and sort flags */
	arrays = (zval ***)ecalloc(argc, sizeof(zval **)); 
	ARRAYG(multisort_flags) = (int *)ecalloc(argc, sizeof(int));

	/* Here we go through the input arguments and parse them. Each one can
	   be either an array or a sort order flag which follows an array. If
	   not specified, the sort order flag defaults to SORT_ASC. There can't
	   be two sort order flags in a row, and the very first argument has
	   to be an array.
	 */
	for (i = 0; i < argc; i++) {
		if ((*args[i])->type == IS_ARRAY) {
			/* We see the next array so update the sort order of
			   the previous array and reset the sort order */
			if (i > 0) {
				ARRAYG(multisort_flags)[num_arrays-1] = sort_order;
				sort_order = SORT_ASC;
			}
			arrays[num_arrays++] = args[i];

			/* next one may be array or sort flag */
			parse_state = 1;
		} else if ((*args[i])->type == IS_LONG) {
			/* flag allowed here */
			if (parse_state == 1) {
				if ((*args[i])->value.lval == SORT_ASC || (*args[i])->value.lval == SORT_DESC) {
					/* Save the flag and make sure then next arg is not a flag */
					sort_order = (*args[i])->value.lval;
					parse_state = 0;
				} else {
					php_error(E_WARNING, "Argument %i to %s() is an unknown sort flag", i+1,
							  get_active_function_name());
					MULTISORT_ABORT;
				}
			} else {
				php_error(E_WARNING, "Argument %i to %s() is expected to be an array", i+1,
						  get_active_function_name());
				MULTISORT_ABORT;
			}
		} else {
			php_error(E_WARNING, "Argument %i to %s() is expected to be an array or a sort flag", i+1,
					  get_active_function_name());
			MULTISORT_ABORT;
		}
	}
	/* Take care of the last array sort flag */
	ARRAYG(multisort_flags)[num_arrays-1] = sort_order;
	
	/* Make sure the arrays are of the same size */
	array_size = zend_hash_num_elements((*arrays[0])->value.ht);
	for (i = 0; i < num_arrays; i++) {
		if (zend_hash_num_elements((*arrays[i])->value.ht) != array_size) {
			php_error(E_WARNING, "Array sizes are inconsistent");
			MULTISORT_ABORT;
		}
	}

	/* If all arrays are empty or have only one entry,
	   we don't need to do anything. */
	if (array_size <= 1) {
		efree(ARRAYG(multisort_flags));
		efree(arrays);
		efree(args);
		RETURN_TRUE;
	}

	/* Create the indirection array. This array is of size MxN, where 
	   M is the number of entries in each input array and N is the number
	   of the input arrays + 1. The last column is NULL to indicate the end
	   of the row.
	 */
	indirect = (Bucket ***)emalloc(array_size * sizeof(Bucket **));
	for (i = 0; i < array_size; i++)
		indirect[i] = (Bucket **)emalloc((num_arrays+1) * sizeof(Bucket *));
	
	for (i = 0; i < num_arrays; i++) {
		k = 0;
		for (p = (*arrays[i])->value.ht->pListHead; p; p = p->pListNext, k++) {
			indirect[k][i] = p;
		}
	}
	for (k = 0; k < array_size; k++)
		indirect[k][num_arrays] = NULL;

	/* For now, assume it's always regular sort. */
	set_compare_func(SORT_REGULAR);

	/* Do the actual sort magic - bada-bim, bada-boom */
	qsort(indirect, array_size, sizeof(Bucket **), multisort_compare);
	
	/* Restructure the arrays based on sorted indirect - this is mostly
	   take from zend_hash_sort() function. */
	HANDLE_BLOCK_INTERRUPTIONS();
	for (i = 0; i < num_arrays; i++) {
		hash = (*arrays[i])->value.ht;
		hash->pListHead = indirect[0][i];;
		hash->pListTail = NULL;
		hash->pInternalPointer = hash->pListHead;

		for (k = 0; k < array_size; k++) {
			if (hash->pListTail) {
				hash->pListTail->pListNext = indirect[k][i];
			}
			indirect[k][i]->pListLast = hash->pListTail;
			indirect[k][i]->pListNext = NULL;
			hash->pListTail = indirect[k][i];
		}
		
		p = hash->pListHead;
		k = 0;
		while (p != NULL) {
			if (p->nKeyLength == 0)
				p->h = k++;
			p = p->pListNext;
		}
		hash->nNextFreeElement = array_size;
		zend_hash_rehash(hash);
	}
	HANDLE_UNBLOCK_INTERRUPTIONS();
		
	/* Clean up */	
	for (i = 0; i < array_size; i++)
		efree(indirect[i]);
	efree(indirect);
	efree(ARRAYG(multisort_flags));
	efree(arrays);
	efree(args);
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto mixed array_rand(array input [, int num_req])
   Return key/keys for random entry/entries in the array */
PHP_FUNCTION(array_rand)
{
	zval **input, **num_req;
	long randval;
	int num_req_val, num_avail, key_type;
	char *string_key;
	ulong num_key;

	if (ZEND_NUM_ARGS() < 1 || ZEND_NUM_ARGS() > 2 ||
		zend_get_parameters_ex(ZEND_NUM_ARGS(), &input, &num_req) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (Z_TYPE_PP(input) != IS_ARRAY) {
		zend_error(E_WARNING, "Argument to %s() has to be an array",
				   get_active_function_name());
		return;
	}

	num_avail = zend_hash_num_elements(Z_ARRVAL_PP(input));

	if (ZEND_NUM_ARGS() > 1) {
		convert_to_long_ex(num_req);
		num_req_val = Z_LVAL_PP(num_req);
		if (num_req_val <= 0 || num_req_val > num_avail) {
			zend_error(E_WARNING, "Second argument to %s() has to be between 1 and the number of elements in the array", get_active_function_name());
			return;
		}
	} else
		num_req_val = 1;

	/* Make the return value an array only if we need to pass back more than one
	   result. */
	if (num_req_val > 1)
		array_init(return_value);

	/* We can't use zend_hash_index_find() because the array may have string keys or gaps. */
	zend_hash_internal_pointer_reset(Z_ARRVAL_PP(input));
	while (num_req_val && (key_type = zend_hash_get_current_key(Z_ARRVAL_PP(input), &string_key, &num_key)) != HASH_KEY_NON_EXISTANT) {

#ifdef HAVE_LRAND48
		randval = lrand48();
#else
#ifdef HAVE_RANDOM
		randval = random();
#else
		randval = rand();
#endif
#endif

		if ((double)(randval/(PHP_RAND_MAX+1.0)) < (double)num_req_val/(double)num_avail) {
			/* If we are returning a single result, just do it. */
			if (Z_TYPE_P(return_value) != IS_ARRAY) {
				if (key_type == HASH_KEY_IS_STRING) {
					RETURN_STRING(string_key, 0);
				} else {
					RETURN_LONG(num_key);
				}
			} else {
				/* Append the result to the return value. */
				if (key_type == HASH_KEY_IS_STRING)
					add_next_index_string(return_value, string_key, 0);
				else
					add_next_index_long(return_value, num_key);
			}
			num_req_val--;
		} else if (key_type == HASH_KEY_IS_STRING)
			efree(string_key);

		num_avail--;
		zend_hash_move_forward(Z_ARRVAL_PP(input));
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
