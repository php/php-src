/* 
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999 The PHP Group                         |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
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
#if WIN32|WINNT
#include "win32/unistd.h"
#endif
#include "zend_globals.h"
#include "php_globals.h"
#include "php_array.h"

#define EXTR_OVERWRITE		0
#define EXTR_SKIP			1
#define EXTR_PREFIX_SAME	2
#define	EXTR_PREFIX_ALL		3

static zval **user_compare_func_name;
static unsigned char all_args_force_ref[] = { 1, BYREF_FORCE_REST };

function_entry array_functions[] = {
	PHP_FE(ksort,									first_arg_force_ref)
	PHP_FE(krsort,									first_arg_force_ref)
	PHP_FE(asort,									first_arg_force_ref)
	PHP_FE(arsort,									first_arg_force_ref)
	PHP_FE(sort,									first_arg_force_ref)
	PHP_FE(rsort,									first_arg_force_ref)
	PHP_FE(usort,									first_arg_force_ref)
	PHP_FE(uasort,									first_arg_force_ref)
	PHP_FE(uksort,									first_arg_force_ref)
	PHP_FE(shuffle,									first_arg_force_ref)
	PHP_FE(array_walk,								first_arg_force_ref)
	PHP_FE(count,									first_arg_allow_ref)
	PHP_FE(end, 									first_arg_force_ref)
	PHP_FE(prev, 									first_arg_force_ref)
	PHP_FE(next, 									first_arg_force_ref)
	PHP_FE(reset, 									first_arg_force_ref)
	PHP_FE(current, 								first_arg_force_ref)
	PHP_FE(key, 									first_arg_force_ref)
	PHP_FE(min,										NULL)
	PHP_FE(max,										NULL)
	PHP_FE(in_array,								NULL)
	PHP_FE(extract,									NULL)
	PHP_FE(compact,									NULL)
	PHP_FE(range,									NULL)
	PHP_FE(multisort,								all_args_force_ref)
	PHP_FE(array_push,								first_arg_force_ref)
	PHP_FE(array_pop,								first_arg_force_ref)
	PHP_FE(array_shift,								first_arg_force_ref)
	PHP_FE(array_unshift,							first_arg_force_ref)
	PHP_FE(array_splice,							first_arg_force_ref)
	PHP_FE(array_slice,								NULL)
	PHP_FE(array_merge,								NULL)
	PHP_FE(array_keys,								NULL)
	PHP_FE(array_values,							NULL)
	PHP_FE(array_count_values,		 			  	NULL)
	PHP_FE(array_reverse,							NULL)
	PHP_FE(array_pad,								NULL)
	PHP_FE(array_flip,								NULL)

	/* Aliases */
	PHP_FALIAS(pos,				current,			first_arg_force_ref)
	PHP_FALIAS(sizeof,			count,				first_arg_allow_ref)
	
	{NULL, NULL, NULL}
};

zend_module_entry array_module_entry = {
	"Array Functions",			/* extension name */
	array_functions,			/* function list */
	PHP_MINIT(array),			/* process startup */
	NULL,						/* process shutdown */
	PHP_RINIT(array),			/* request startup */
	NULL,						/* request shutdown */
	NULL,						/* extension info */
	STANDARD_MODULE_PROPERTIES
};

PHP_MINIT_FUNCTION(array)
{
	ELS_FETCH();

	REGISTER_LONG_CONSTANT("EXTR_OVERWRITE", EXTR_OVERWRITE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EXTR_SKIP", EXTR_SKIP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EXTR_PREFIX_SAME", EXTR_PREFIX_SAME, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EXTR_PREFIX_ALL", EXTR_PREFIX_ALL, CONST_CS | CONST_PERSISTENT);
	
	return SUCCESS;
}

PHP_RINIT_FUNCTION(array)
{
	user_compare_func_name=NULL;
	return SUCCESS;
}
	
static int array_key_compare(const void *a, const void *b)
{
	Bucket *f;
	Bucket *s;
	pval result;
	pval first;
	pval second;
 
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
 
    if (compare_function(&result, &first, &second) == FAILURE) {
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

PHP_FUNCTION(krsort)
{
	pval **array;
	HashTable *target_hash;

	if (ARG_COUNT(ht) != 1 || getParametersEx(1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Wrong datatype in krsort() call");
		return;
	}
	if (zend_hash_sort(target_hash, qsort, array_reverse_key_compare, 0) == FAILURE) {
		return;
	}
	RETURN_TRUE;
}

PHP_FUNCTION(ksort)
{
	pval **array;
	HashTable *target_hash;

	if (ARG_COUNT(ht) != 1 || getParametersEx(1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Wrong datatype in ksort() call");
		return;
	}
	if (zend_hash_sort(target_hash, qsort, array_key_compare,0) == FAILURE) {
		return;
	}
	RETURN_TRUE;
}


PHP_FUNCTION(count)
{
	pval **array;
	HashTable *target_hash;

	if (ARG_COUNT(ht) != 1 || getParametersEx(1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		if ((*array)->type == IS_STRING && (*array)->value.str.val==undefined_variable_string) {
			RETURN_LONG(0);
		} else {
			RETURN_LONG(1);
		}
	}

	RETURN_LONG(zend_hash_num_elements(target_hash));
}


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
 
	f = *((Bucket **) a);
	s = *((Bucket **) b);
 
	first = *((pval **) f->pData);
	second = *((pval **) s->pData);

    if (compare_function(&result, first, second) == FAILURE) {
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

PHP_FUNCTION(asort)
{
	pval **array;
	HashTable *target_hash;

	if (ARG_COUNT(ht) != 1 || getParametersEx(1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Wrong datatype in asort() call");
		return;
	}
	if (zend_hash_sort(target_hash, qsort, array_data_compare,0) == FAILURE) {
		return;
	}
	RETURN_TRUE;
}

PHP_FUNCTION(arsort)
{
	pval **array;
	HashTable *target_hash;

	if (ARG_COUNT(ht) != 1 || getParametersEx(1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Wrong datatype in arsort() call");
		return;
	}
	if (zend_hash_sort(target_hash, qsort, array_reverse_data_compare,0) == FAILURE) {
		return;
	}
	RETURN_TRUE;
}

PHP_FUNCTION(sort)
{
	pval **array;
	HashTable *target_hash;

	if (ARG_COUNT(ht) != 1 || getParametersEx(1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Wrong datatype in sort() call");
		return;
	}
	if (zend_hash_sort(target_hash, qsort, array_data_compare,1) == FAILURE) {
		return;
	}
	RETURN_TRUE;
}

PHP_FUNCTION(rsort)
{
	pval **array;
	HashTable *target_hash;

	if (ARG_COUNT(ht) != 1 || getParametersEx(1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Wrong datatype in rsort() call");
		return;
	}
	if (zend_hash_sort(target_hash, qsort, array_reverse_data_compare,1) == FAILURE) {
		return;
	}
	RETURN_TRUE;
}


static int array_user_compare(const void *a, const void *b)
{
	Bucket *f;
	Bucket *s;
	pval **args[2];
	pval retval;
	CLS_FETCH();

	f = *((Bucket **) a);
	s = *((Bucket **) b);

	args[0] = (pval **) f->pData;
	args[1] = (pval **) s->pData;

	if (call_user_function_ex(CG(function_table), NULL, *user_compare_func_name, &retval, 2, args, 0)==SUCCESS) {
		convert_to_long(&retval);
		return retval.value.lval;
	} else {
		return 0;
	}
}


PHP_FUNCTION(usort)
{
	pval **array;
	pval **old_compare_func;
	HashTable *target_hash;

	old_compare_func = user_compare_func_name;
	if (ARG_COUNT(ht) != 2 || getParametersEx(2, &array, &user_compare_func_name) == FAILURE) {
		user_compare_func_name = old_compare_func;
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Wrong datatype in usort() call");
		user_compare_func_name = old_compare_func;
		return;
	}
	convert_to_string_ex(user_compare_func_name);
	if (zend_hash_sort(target_hash, qsort, array_user_compare, 1) == FAILURE) {
		user_compare_func_name = old_compare_func;
		return;
	}
	user_compare_func_name = old_compare_func;
	RETURN_TRUE;
}

PHP_FUNCTION(uasort)
{
	pval **array;
	pval **old_compare_func;
	HashTable *target_hash;

	old_compare_func = user_compare_func_name;
	if (ARG_COUNT(ht) != 2 || getParametersEx(2, &array, &user_compare_func_name) == FAILURE) {
		user_compare_func_name = old_compare_func;
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Wrong datatype in uasort() call");
		user_compare_func_name = old_compare_func;
		return;
	}
	convert_to_string_ex(user_compare_func_name);
	if (zend_hash_sort(target_hash, qsort, array_user_compare, 0) == FAILURE) {
		user_compare_func_name = old_compare_func;
		return;
	}
	user_compare_func_name = old_compare_func;
	RETURN_TRUE;
}


static int array_user_key_compare(const void *a, const void *b)
{
	Bucket *f;
	Bucket *s;
	pval key1, key2;
	pval *args[2];
	pval retval;
	int status;
	CLS_FETCH();

	args[0] = &key1;
	args[1] = &key2;
	INIT_PZVAL(&key1);
	INIT_PZVAL(&key2);
	
	f = *((Bucket **) a);
	s = *((Bucket **) b);

	if (f->nKeyLength) {
		key1.value.str.val = estrndup(f->arKey, f->nKeyLength);
		key1.value.str.len = f->nKeyLength;
		key1.type = IS_STRING;
	} else {
		key1.value.lval = f->h;
		key1.type = IS_LONG;
	}
	if (s->nKeyLength) {
		key2.value.str.val = estrndup(s->arKey, s->nKeyLength);
		key2.value.str.len = s->nKeyLength;
		key2.type = IS_STRING;
	} else {
		key2.value.lval = s->h;
		key2.type = IS_LONG;
	}

	status = call_user_function(CG(function_table), NULL, *user_compare_func_name, &retval, 2, args);
	
	zval_dtor(&key1);
	zval_dtor(&key2);
	
	if (status==SUCCESS) {
		convert_to_long(&retval);
		return retval.value.lval;
	} else {
		return 0;
	}
}


PHP_FUNCTION(uksort)
{
	pval **array;
	pval **old_compare_func;
	HashTable *target_hash;

	old_compare_func = user_compare_func_name;
	if (ARG_COUNT(ht) != 2 || getParametersEx(2, &array, &user_compare_func_name) == FAILURE) {
		user_compare_func_name = old_compare_func;
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Wrong datatype in uksort() call");
		user_compare_func_name = old_compare_func;
		return;
	}
	convert_to_string_ex(user_compare_func_name);
	if (zend_hash_sort(target_hash, qsort, array_user_key_compare, 0) == FAILURE) {
		user_compare_func_name = old_compare_func;
		return;
	}
	user_compare_func_name = old_compare_func;
	RETURN_TRUE;
}

PHP_FUNCTION(end)
{
	pval **array, **entry;
	HashTable *target_hash;

	if (ARG_COUNT(ht) != 1 || getParametersEx(1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Variable passed to end() is not an array or object");
		return;
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


PHP_FUNCTION(prev)
{
	pval **array, **entry;
	HashTable *target_hash;

	if (ARG_COUNT(ht) != 1 || getParametersEx(1, &array) == FAILURE) {
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


PHP_FUNCTION(next)
{
	pval **array, **entry;
	HashTable *target_hash;

	if (ARG_COUNT(ht) != 1 || getParametersEx(1, &array) == FAILURE) {
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

	
PHP_FUNCTION(reset)
{
	pval **array, **entry;
	HashTable *target_hash;

	if (ARG_COUNT(ht) != 1 || getParametersEx(1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Variable passed to reset() is not an array or object");
		return;
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

PHP_FUNCTION(current)
{
	pval **array, **entry;
	HashTable *target_hash;

	if (ARG_COUNT(ht) != 1 || getParametersEx(1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Variable passed to current() is not an array or object");
		return;
	}
	if (zend_hash_get_current_data(target_hash, (void **) &entry) == FAILURE) {
		return;
	}
	*return_value = **entry;
	zval_copy_ctor(return_value);
}


PHP_FUNCTION(key)
{
	pval **array;
	char *string_key;
	ulong num_key;
	HashTable *target_hash;

	if (ARG_COUNT(ht) != 1 || getParametersEx(1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Variable passed to key() is not an array or object");
		return;
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


PHP_FUNCTION(min)
{
	int argc=ARG_COUNT(ht);
	pval **result;

	if (argc<=0) {
		php_error(E_WARNING, "min: must be passed at least 1 value");
		var_uninit(return_value);
		return;
	}
	if (argc == 1) {
		pval **arr;

		if (getParametersEx(1, &arr) == FAILURE || (*arr)->type != IS_ARRAY) {
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
		pval ***args = (pval ***) emalloc(sizeof(pval **)*ARG_COUNT(ht));
		pval **min, result;
		int i;

		if (getParametersArrayEx(ARG_COUNT(ht), args)==FAILURE) {
			efree(args);
			WRONG_PARAM_COUNT;
		}

		min = args[0];

		for (i=1; i<ARG_COUNT(ht); i++) {
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


PHP_FUNCTION(max)
{
	int argc=ARG_COUNT(ht);
	pval **result;

	if (argc<=0) {
		php_error(E_WARNING, "max: must be passed at least 1 value");
		var_uninit(return_value);
		return;
	}
	if (argc == 1) {
		pval **arr;

		if (getParametersEx(1, &arr) == FAILURE || (*arr)->type != IS_ARRAY) {
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
		pval ***args = (pval ***) emalloc(sizeof(pval **)*ARG_COUNT(ht));
		pval **max, result;
		int i;

		if (getParametersArrayEx(ARG_COUNT(ht), args)==FAILURE) {
			efree(args);
			WRONG_PARAM_COUNT;
		}

		max = args[0];

		for (i=1; i<ARG_COUNT(ht); i++) {
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

static zval **php_array_walk_func_name;

static int php_array_walk(HashTable *target_hash, zval **userdata)
{
	zval **args[3],			/* Arguments to userland function */
		   retval,			/* Return value - unused */
		  *key;				/* Entry key */
	char  *string_key;
	ulong  num_key;
	CLS_FETCH();

	/* Allocate space for key */
	MAKE_STD_ZVAL(key);
	
	/* Set up known arguments */
	args[1] = &key;
	args[2] = userdata;

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
		call_user_function_ex(CG(function_table), NULL, *php_array_walk_func_name,
						   &retval, userdata ? 3 : 2, args, 0);
		
		/* Clean up the key */
		if (zend_hash_get_current_key_type(target_hash) == HASH_KEY_IS_STRING)
			efree(key->value.str.val);
		
		zend_hash_move_forward(target_hash);
    }
	efree(key);
	
	return 0;
}

/* {{{ proto array_walk(array input, string funcname [, mixed userdata])
   Apply a user function to every member of an array */
PHP_FUNCTION(array_walk) {
	int    argc;
	zval **array,
		 **userdata = NULL,
		 **old_walk_func_name;
	HashTable *target_hash;

	argc = ARG_COUNT(ht);
	old_walk_func_name = php_array_walk_func_name;
	if (argc < 2 || argc > 3 ||
		getParametersEx(argc, &array, &php_array_walk_func_name, &userdata) == FAILURE) {
		php_array_walk_func_name = old_walk_func_name;
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		php_error(E_WARNING, "Wrong datatype in array_walk() call");
		php_array_walk_func_name = old_walk_func_name;
		return;
	}
	convert_to_string_ex(php_array_walk_func_name);
	php_array_walk(target_hash, userdata);
	php_array_walk_func_name = old_walk_func_name;
	RETURN_TRUE;
}


/* {{{ proto bool in_array(mixed needle, array haystack)
   Checks if the given value exists in the array */
PHP_FUNCTION(in_array)
{
	zval **value,				/* value to check for */
		 **array,				/* array to check in */
		 **entry_ptr,			/* pointer to array entry */
		 *entry,				/* actual array entry */
		  res;					/* comparison result */
	HashTable *target_hash;		/* array hashtable */

	if (ARG_COUNT(ht) != 2 || getParametersEx(2, &value, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	if ((*value)->type == IS_ARRAY || (*value)->type == IS_OBJECT) {
		php_error(E_WARNING, "Wrong datatype for first argument in call to in_array()");
		return;
	}
	
	if ((*array)->type != IS_ARRAY) {
		php_error(E_WARNING, "Wrong datatype for second argument in call to in_array()");
		return;
	}

	target_hash = HASH_OF(*array);
	zend_hash_internal_pointer_reset(target_hash);
	while(zend_hash_get_current_data(target_hash, (void **)&entry_ptr) == SUCCESS) {
		entry = *entry_ptr;
     	is_equal_function(&res, *value, entry);
		if (zval_is_true(&res)) {
			RETURN_TRUE;
		}
		
		zend_hash_move_forward(target_hash);
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
	zval **entry, *exist, *data;
	char *varname, *finalname;
	ulong lkey;
	int res, extype;
	ELS_FETCH();

	switch(ARG_COUNT(ht)) {
		case 1:
			if (getParametersEx(1, &var_array) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			extype = EXTR_OVERWRITE;
			break;

		case 2:
			if (getParametersEx(2, &var_array, &etype) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_long_ex(etype);
			extype = (*etype)->value.lval;
			if (extype > EXTR_SKIP && extype <= EXTR_PREFIX_ALL) {
				WRONG_PARAM_COUNT;
			}
			break;
			
		case 3:
			if (getParametersEx(3, &var_array, &etype, &prefix) == FAILURE) {
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
		return;
	}
	
	if ((*var_array)->type != IS_ARRAY) {
		php_error(E_WARNING, "Wrong datatype in call to extract()");
		return;
	}
		
	zend_hash_internal_pointer_reset((*var_array)->value.ht);
	while(zend_hash_get_current_data((*var_array)->value.ht, (void **)&entry) == SUCCESS) {

		if (zend_hash_get_current_key((*var_array)->value.ht, &varname, &lkey) == HASH_KEY_IS_STRING) {

			if (_valid_var_name(varname)) {
				finalname = NULL;

				res = zend_hash_find(EG(active_symbol_table),
									 varname, strlen(varname)+1, (void**)&exist);
				switch (extype) {
					case EXTR_OVERWRITE:
						finalname = estrdup(varname);
						break;

					case EXTR_PREFIX_SAME:
						if (res != SUCCESS)
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
						if (res != SUCCESS)
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
			data = (zval *)emalloc(sizeof(zval));
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


/* {{{ proto array compact(string var_name | array var_names [, ... ])
   Creates a hash containing variables and their values */
PHP_FUNCTION(compact)
{
	zval ***args;			/* function arguments array */
	int i;
	ELS_FETCH();
	
	args = (zval ***)emalloc(ARG_COUNT(ht) * sizeof(zval **));
	
	if (getParametersArrayEx(ARG_COUNT(ht), args) == FAILURE) {
		efree(args);
		WRONG_PARAM_COUNT;
	}

	array_init(return_value);
	
	for (i=0; i<ARG_COUNT(ht); i++)
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
	
	if (ARG_COUNT(ht) != 2 || getParametersEx(2,&zlow,&zhigh) == FAILURE) {
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

	if (ARG_COUNT(ht) != 1 || getParametersEx(1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	if ((*array)->type != IS_ARRAY) {
		php_error(E_WARNING, "Wrong datatype in shuffle() call");
		return;
	}
	if (zend_hash_sort((*array)->value.ht, (sort_func_t)mergesort, array_data_shuffle, 1) == FAILURE) {
		return;
	}
	RETURN_TRUE;
}
/* }}} */


/* HashTable* _phpi_splice(HashTable *in_hash, int offset, int length,
						   zval ***list, int list_count, HashTable **removed) */
HashTable* _phpi_splice(HashTable *in_hash, int offset, int length,
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
	zend_hash_init(out_hash, 0, NULL, PVAL_PTR_DTOR, 0);
	
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


/* {{{ proto int array_push(array stack, mixed var [, ...])
   Pushes elements onto the end of the array */
PHP_FUNCTION(array_push)
{
	zval	  ***args,		/* Function arguments array */
			    *stack,		/* Input array */
			    *new_var;	/* Variable to be pushed */
	int			 i,			/* Loop counter */
				 argc;		/* Number of function arguments */

	/* Get the argument count and check it */
	argc = ARG_COUNT(ht);
	if (argc < 2) {
		WRONG_PARAM_COUNT;
	}
	
	/* Allocate arguments array and get the arguments, checking for errors. */
	args = (zval ***)emalloc(argc * sizeof(zval **));
	if (getParametersArrayEx(argc, args) == FAILURE) {
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
	if (ARG_COUNT(ht) != 1 || getParametersEx(1, &stack) == FAILURE) {
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
	new_hash = _phpi_splice((*stack)->value.ht, (off_the_end) ? -1 : 0, 1, NULL, 0, NULL);
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


/* {{{ proto int array_unshift(array stack, mixed var [, ...])
   Pushes elements onto the beginning of the array */
PHP_FUNCTION(array_unshift)
{
	zval	  ***args,		/* Function arguments array */
				*stack;		/* Input stack */
	HashTable	*new_hash;	/* New hashtable for the stack */
	int			 argc;		/* Number of function arguments */
	

	/* Get the argument count and check it */	
	argc = ARG_COUNT(ht);
	if (argc < 2) {
		WRONG_PARAM_COUNT;
	}
	
	/* Allocate arguments array and get the arguments, checking for errors. */
	args = (zval ***)emalloc(argc * sizeof(zval **));
	if (getParametersArrayEx(argc, args) == FAILURE) {
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
	new_hash = _phpi_splice(stack->value.ht, 0, 0, &args[1], argc-1, NULL);
	zend_hash_destroy(stack->value.ht);
	efree(stack->value.ht);
	stack->value.ht = new_hash;

	/* Clean up and return the number of elements in the stack */
	efree(args);
	RETVAL_LONG(zend_hash_num_elements(stack->value.ht));
}
/* }}} */


/* {{{ proto array array_splice(array input, int offset [, int length [, array replacement]])
   Removes the elements designated by offset and length and replace them with
   supplied array */
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
	argc = ARG_COUNT(ht);
	if (argc < 2 || argc > 4) {
		WRONG_PARAM_COUNT;
	}
	
	/* Allocate arguments array and get the arguments, checking for errors. */
	args = (zval ***)emalloc(argc * sizeof(zval **));
	if (getParametersArrayEx(argc, args) == FAILURE) {
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
	new_hash = _phpi_splice(array->value.ht, offset, length,
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
	argc = ARG_COUNT(ht);
	if (argc < 2 || argc > 3 || getParametersEx(argc, &input, &offset, &length)) {
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


/* {{{ proto array array_merge(array arr1, array arr2 [, ...])
   Merges elements from passed arrays into one array */
PHP_FUNCTION(array_merge)
{
	zval	  ***args = NULL,
			   **entry;
	HashTable	*hash;
	int			 argc,
				 i;
	char		*string_key;
	ulong		 num_key;

	/* Get the argument count and check it */	
	argc = ARG_COUNT(ht);
	if (argc < 2) {
		WRONG_PARAM_COUNT;
	}
	
	/* Allocate arguments array and get the arguments, checking for errors. */
	args = (zval ***)emalloc(argc * sizeof(zval **));
	if (getParametersArrayEx(argc, args) == FAILURE) {
		efree(args);
		WRONG_PARAM_COUNT;
	}
	
	array_init(return_value);
	
	for (i=0; i<argc; i++) {
		if ((*args[i])->type != IS_ARRAY) {
			php_error(E_WARNING, "Skipping argument #%d to array_merge(), since it's not an array", i+1);
			continue;
		}
		hash = (*args[i])->value.ht;
		
		zend_hash_internal_pointer_reset(hash);
		while(zend_hash_get_current_data(hash, (void **)&entry) == SUCCESS) {
			(*entry)->refcount++;
			
			switch (zend_hash_get_current_key(hash, &string_key, &num_key)) {
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

			zend_hash_move_forward(hash);
		}
	}
	
	efree(args);
}
/* }}} */


/* {{{ proto array array_keys(array input [, mixed search_value])
   Return just the keys from the input array, optionally only
   for the specified search_value */
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
	if (ARG_COUNT(ht) < 1 || ARG_COUNT(ht) > 2 ||
		getParametersEx(ARG_COUNT(ht), &input, &search_value) == FAILURE) {
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
	if (ARG_COUNT(ht) != 1 || getParametersEx(ARG_COUNT(ht), &input) == FAILURE) {
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
	if (ARG_COUNT(ht) != 1 || getParametersEx(1, &input) == FAILURE) {
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
	if (ARG_COUNT(ht) != 1 || getParametersEx(1, &input) == FAILURE) {
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
	if (ARG_COUNT(ht) != 3 || getParametersEx(3, &input, &pad_size, &pad_value) == FAILURE) {
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
		new_hash = _phpi_splice(return_value->value.ht, input_size, 0, pads, num_pads, NULL);
	else
		new_hash = _phpi_splice(return_value->value.ht, 0, 0, pads, num_pads, NULL);

	
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
	
	if (ARG_COUNT(ht) != 1 || getParametersEx(1, &array) == FAILURE) {
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

int multisort_compare(const void *a, const void *b)
{
	Bucket**	  ab = *(Bucket ***)a;
	Bucket**	  bb = *(Bucket ***)b;
	int			  r;
	int			  result = 0;
	zval		  temp;

	r = 0;
	do {
		compare_function(&temp, *((zval **)ab[r]->pData), *((zval **)bb[r]->pData));
		result = temp.value.lval;
		if (result != 0)
			return result;
		r++;
	} while (ab[r] != NULL);
	return result;
}

PHP_FUNCTION(multisort)
{
	zval***			args;
	Bucket***		indirect;
	Bucket*			p;
	HashTable*		hash;
	int				argc;
	int				array_size;
	int				i, k;
	
	/* Get the argument count and check it */
	argc = ARG_COUNT(ht);
	if (argc < 1) {
		WRONG_PARAM_COUNT;
	}
	
	/* Allocate arguments array and get the arguments, checking for errors. */
	args = (zval ***)emalloc(argc * sizeof(zval **));
	if (getParametersArrayEx(argc, args) == FAILURE) {
		efree(args);
		WRONG_PARAM_COUNT;
	}

	for (i = 0; i < argc; i++) {
		if ((*args[i])->type != IS_ARRAY) {
			php_error(E_WARNING, "Argument %i to %s() is not an array", i+1,
					  get_active_function_name());
			efree(args);
			return;
		}
	}
	
	/* Make sure the arrays are of the same size */
	array_size = zend_hash_num_elements((*args[0])->value.ht);
	for (i = 0; i < argc; i++) {
		if (zend_hash_num_elements((*args[i])->value.ht) != array_size) {
			php_error(E_WARNING, "Array sizes are inconsistent");
			efree(args);
			return;
		}
	}

	/* Create the indirection array */
	indirect = (Bucket ***)emalloc(array_size * sizeof(Bucket **));
	for (i = 0; i < array_size; i++)
		indirect[i] = (Bucket **)emalloc((argc+1) * sizeof(Bucket *));
	
	for (i = 0; i < argc; i++) {
		k = 0;
		for (p = (*args[i])->value.ht->pListHead; p; p = p->pListNext, k++) {
			indirect[k][i] = p;
		}
	}
	for (k = 0; k < array_size; k++)
		indirect[k][argc] = NULL;

	/* Do the actual sort */
	qsort(indirect, array_size, sizeof(Bucket **), multisort_compare);
	
	HANDLE_BLOCK_INTERRUPTIONS();
	for (i = 0; i < argc; i++) {
		hash = (*args[i])->value.ht;
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
	efree(args);
}
