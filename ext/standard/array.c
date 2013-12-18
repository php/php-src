/* 
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Rasmus Lerdorf <rasmus@php.net>                             |
   |          Andrei Zmievski <andrei@php.net>                            |
   |          Stig Venaas <venaas@php.net>                                |
   |          Jason Greene <jason@php.net>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "php_ini.h"
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
#include "zend_interfaces.h"
#include "php_globals.h"
#include "php_array.h"
#include "basic_functions.h"
#include "php_string.h"
#include "php_rand.h"
#include "php_smart_str.h"
#ifdef HAVE_SPL
#include "ext/spl/spl_array.h"
#endif

/* {{{ defines */
#define EXTR_OVERWRITE			0
#define EXTR_SKIP				1
#define EXTR_PREFIX_SAME		2
#define	EXTR_PREFIX_ALL			3
#define	EXTR_PREFIX_INVALID		4
#define	EXTR_PREFIX_IF_EXISTS	5
#define	EXTR_IF_EXISTS			6

#define EXTR_REFS				0x100

#define CASE_LOWER				0
#define CASE_UPPER				1

#define COUNT_NORMAL			0
#define COUNT_RECURSIVE			1

#define DIFF_NORMAL			1
#define DIFF_KEY			2
#define DIFF_ASSOC			6
#define DIFF_COMP_DATA_NONE    -1
#define DIFF_COMP_DATA_INTERNAL 0
#define DIFF_COMP_DATA_USER     1
#define DIFF_COMP_KEY_INTERNAL  0
#define DIFF_COMP_KEY_USER      1

#define INTERSECT_NORMAL		1
#define INTERSECT_KEY			2
#define INTERSECT_ASSOC			6
#define INTERSECT_COMP_DATA_NONE    -1
#define INTERSECT_COMP_DATA_INTERNAL 0
#define INTERSECT_COMP_DATA_USER     1
#define INTERSECT_COMP_KEY_INTERNAL  0
#define INTERSECT_COMP_KEY_USER      1

#define DOUBLE_DRIFT_FIX	0.000000000000001
/* }}} */

ZEND_DECLARE_MODULE_GLOBALS(array)

/* {{{ php_array_init_globals
*/
static void php_array_init_globals(zend_array_globals *array_globals)
{
	memset(array_globals, 0, sizeof(zend_array_globals));
}
/* }}} */

PHP_MINIT_FUNCTION(array) /* {{{ */
{
	ZEND_INIT_MODULE_GLOBALS(array, php_array_init_globals, NULL);

	REGISTER_LONG_CONSTANT("EXTR_OVERWRITE", EXTR_OVERWRITE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EXTR_SKIP", EXTR_SKIP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EXTR_PREFIX_SAME", EXTR_PREFIX_SAME, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EXTR_PREFIX_ALL", EXTR_PREFIX_ALL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EXTR_PREFIX_INVALID", EXTR_PREFIX_INVALID, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EXTR_PREFIX_IF_EXISTS", EXTR_PREFIX_IF_EXISTS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EXTR_IF_EXISTS", EXTR_IF_EXISTS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EXTR_REFS", EXTR_REFS, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("SORT_ASC", PHP_SORT_ASC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SORT_DESC", PHP_SORT_DESC, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("SORT_REGULAR", PHP_SORT_REGULAR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SORT_NUMERIC", PHP_SORT_NUMERIC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SORT_STRING", PHP_SORT_STRING, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SORT_LOCALE_STRING", PHP_SORT_LOCALE_STRING, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SORT_NATURAL", PHP_SORT_NATURAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SORT_FLAG_CASE", PHP_SORT_FLAG_CASE, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("CASE_LOWER", CASE_LOWER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CASE_UPPER", CASE_UPPER, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("COUNT_NORMAL", COUNT_NORMAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("COUNT_RECURSIVE", COUNT_RECURSIVE, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("ARRAY_FILTER_USE_BOTH", ARRAY_FILTER_USE_BOTH, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ARRAY_FILTER_USE_KEY", ARRAY_FILTER_USE_KEY, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */

PHP_MSHUTDOWN_FUNCTION(array) /* {{{ */
{
#ifdef ZTS
	ts_free_id(array_globals_id);
#endif

	return SUCCESS;
}
/* }}} */

static void php_set_compare_func(int sort_type TSRMLS_DC) /* {{{ */
{
	switch (sort_type & ~PHP_SORT_FLAG_CASE) {
		case PHP_SORT_NUMERIC:
			ARRAYG(compare_func) = numeric_compare_function;
			break;

		case PHP_SORT_STRING:
			ARRAYG(compare_func) = sort_type & PHP_SORT_FLAG_CASE ? string_case_compare_function : string_compare_function;
			break;

		case PHP_SORT_NATURAL:
			ARRAYG(compare_func) = sort_type & PHP_SORT_FLAG_CASE ? string_natural_case_compare_function : string_natural_compare_function;
			break;

#if HAVE_STRCOLL
		case PHP_SORT_LOCALE_STRING:
			ARRAYG(compare_func) = string_locale_compare_function;
			break;
#endif

		case PHP_SORT_REGULAR:
		default:
			ARRAYG(compare_func) = compare_function;
			break;
	}
}
/* }}} */

static int php_array_key_compare(const void *a, const void *b TSRMLS_DC) /* {{{ */
{
	Bucket *f;
	Bucket *s;
	zval result;
	zval first;
	zval second;

	f = *((Bucket **) a);
	s = *((Bucket **) b);

	if (f->nKeyLength == 0) {
		Z_TYPE(first) = IS_LONG;
		Z_LVAL(first) = f->h;
	} else {
		Z_TYPE(first) = IS_STRING;
		Z_STRVAL(first) = (char*)f->arKey;
		Z_STRLEN(first) = f->nKeyLength - 1;
	}

	if (s->nKeyLength == 0) {
		Z_TYPE(second) = IS_LONG;
		Z_LVAL(second) = s->h;
	} else {
		Z_TYPE(second) = IS_STRING;
		Z_STRVAL(second) = (char*)s->arKey;
		Z_STRLEN(second) = s->nKeyLength - 1;
	}

	if (ARRAYG(compare_func)(&result, &first, &second TSRMLS_CC) == FAILURE) {
		return 0;
	}

	if (Z_TYPE(result) == IS_DOUBLE) {
		if (Z_DVAL(result) < 0) {
			return -1;
		} else if (Z_DVAL(result) > 0) {
			return 1;
		} else {
			return 0;
		}
	}

	convert_to_long(&result);

	if (Z_LVAL(result) < 0) {
		return -1;
	} else if (Z_LVAL(result) > 0) {
		return 1;
	}

	return 0;
}
/* }}} */

static int php_array_reverse_key_compare(const void *a, const void *b TSRMLS_DC) /* {{{ */
{
	return php_array_key_compare(a, b TSRMLS_CC) * -1;
}
/* }}} */

/* {{{ proto bool krsort(array &array_arg [, int sort_flags])
   Sort an array by key value in reverse order */
PHP_FUNCTION(krsort)
{
	zval *array;
	long sort_type = PHP_SORT_REGULAR;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|l", &array, &sort_type) == FAILURE) {
		RETURN_FALSE;
	}

	php_set_compare_func(sort_type TSRMLS_CC);

	if (zend_hash_sort(Z_ARRVAL_P(array), zend_qsort, php_array_reverse_key_compare, 0 TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ksort(array &array_arg [, int sort_flags])
   Sort an array by key */
PHP_FUNCTION(ksort)
{
	zval *array;
	long sort_type = PHP_SORT_REGULAR;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|l", &array, &sort_type) == FAILURE) {
		RETURN_FALSE;
	}

	php_set_compare_func(sort_type TSRMLS_CC);

	if (zend_hash_sort(Z_ARRVAL_P(array), zend_qsort, php_array_key_compare, 0 TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

static int php_count_recursive(zval *array, long mode TSRMLS_DC) /* {{{ */
{
	long cnt = 0;
	zval **element;

	if (Z_TYPE_P(array) == IS_ARRAY) {
		if (Z_ARRVAL_P(array)->nApplyCount > 1) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "recursion detected");
			return 0;
		}

		cnt = zend_hash_num_elements(Z_ARRVAL_P(array));
		if (mode == COUNT_RECURSIVE) {
			HashPosition pos;

			for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(array), &pos);
				zend_hash_get_current_data_ex(Z_ARRVAL_P(array), (void **) &element, &pos) == SUCCESS;
				zend_hash_move_forward_ex(Z_ARRVAL_P(array), &pos)
			) {
				Z_ARRVAL_P(array)->nApplyCount++;
				cnt += php_count_recursive(*element, COUNT_RECURSIVE TSRMLS_CC);
				Z_ARRVAL_P(array)->nApplyCount--;
			}
		}
	}

	return cnt;
}
/* }}} */

/* {{{ proto int count(mixed var [, int mode])
   Count the number of elements in a variable (usually an array) */
PHP_FUNCTION(count)
{
	zval *array;
	long mode = COUNT_NORMAL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|l", &array, &mode) == FAILURE) {
		return;
	}

	switch (Z_TYPE_P(array)) {
		case IS_NULL:
			RETURN_LONG(0);
			break;
		case IS_ARRAY:
			RETURN_LONG (php_count_recursive (array, mode TSRMLS_CC));
			break;
		case IS_OBJECT: {
#ifdef HAVE_SPL
			zval *retval;
#endif
			/* first, we check if the handler is defined */
			if (Z_OBJ_HT_P(array)->count_elements) {
				RETVAL_LONG(1);
				if (SUCCESS == Z_OBJ_HT(*array)->count_elements(array, &Z_LVAL_P(return_value) TSRMLS_CC)) {
					return;
				}
			}
#ifdef HAVE_SPL
			/* if not and the object implements Countable we call its count() method */
			if (Z_OBJ_HT_P(array)->get_class_entry && instanceof_function(Z_OBJCE_P(array), spl_ce_Countable TSRMLS_CC)) {
				zend_call_method_with_0_params(&array, NULL, NULL, "count", &retval);
				if (retval) {
					convert_to_long_ex(&retval);
					RETVAL_LONG(Z_LVAL_P(retval));
					zval_ptr_dtor(&retval);
				}
				return;
			}
#endif
		}
		default:
			RETURN_LONG(1);
			break;
	}
}
/* }}} */

/* Numbers are always smaller than strings int this function as it
 * anyway doesn't make much sense to compare two different data types.
 * This keeps it consistent and simple.
 *
 * This is not correct any more, depends on what compare_func is set to.
 */
static int php_array_data_compare(const void *a, const void *b TSRMLS_DC) /* {{{ */
{
	Bucket *f;
	Bucket *s;
	zval result;
	zval *first;
	zval *second;

	f = *((Bucket **) a);
	s = *((Bucket **) b);

	first = *((zval **) f->pData);
	second = *((zval **) s->pData);

	if (ARRAYG(compare_func)(&result, first, second TSRMLS_CC) == FAILURE) {
		return 0;
	}

	if (Z_TYPE(result) == IS_DOUBLE) {
		if (Z_DVAL(result) < 0) {
			return -1;
		} else if (Z_DVAL(result) > 0) {
			return 1;
		} else {
			return 0;
		}
	}

	convert_to_long(&result);

	if (Z_LVAL(result) < 0) {
		return -1;
	} else if (Z_LVAL(result) > 0) {
		return 1;
	}

	return 0;
}
/* }}} */

static int php_array_reverse_data_compare(const void *a, const void *b TSRMLS_DC) /* {{{ */
{
	return php_array_data_compare(a, b TSRMLS_CC) * -1;
}
/* }}} */

static int php_array_natural_general_compare(const void *a, const void *b, int fold_case) /* {{{ */
{
	Bucket *f, *s;
	zval *fval, *sval;
	zval first, second;
	int result;

	f = *((Bucket **) a);
	s = *((Bucket **) b);

	fval = *((zval **) f->pData);
	sval = *((zval **) s->pData);
	first = *fval;
	second = *sval;

	if (Z_TYPE_P(fval) != IS_STRING) {
		zval_copy_ctor(&first);
		convert_to_string(&first);
	}

	if (Z_TYPE_P(sval) != IS_STRING) {
		zval_copy_ctor(&second);
		convert_to_string(&second);
	}

	result = strnatcmp_ex(Z_STRVAL(first), Z_STRLEN(first), Z_STRVAL(second), Z_STRLEN(second), fold_case);

	if (Z_TYPE_P(fval) != IS_STRING) {
		zval_dtor(&first);
	}

	if (Z_TYPE_P(sval) != IS_STRING) {
		zval_dtor(&second);
	}

	return result;
}
/* }}} */

static int php_array_natural_compare(const void *a, const void *b TSRMLS_DC) /* {{{ */
{
	return php_array_natural_general_compare(a, b, 0);
}
/* }}} */

static int php_array_natural_case_compare(const void *a, const void *b TSRMLS_DC) /* {{{ */
{
	return php_array_natural_general_compare(a, b, 1);
}
/* }}} */

static void php_natsort(INTERNAL_FUNCTION_PARAMETERS, int fold_case) /* {{{ */
{
	zval *array;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &array) == FAILURE) {
		return;
	}

	if (fold_case) {
		if (zend_hash_sort(Z_ARRVAL_P(array), zend_qsort, php_array_natural_case_compare, 0 TSRMLS_CC) == FAILURE) {
			return;
		}
	} else {
		if (zend_hash_sort(Z_ARRVAL_P(array), zend_qsort, php_array_natural_compare, 0 TSRMLS_CC) == FAILURE) {
			return;
		}
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void natsort(array &array_arg)
   Sort an array using natural sort */
PHP_FUNCTION(natsort)
{
	php_natsort(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto void natcasesort(array &array_arg)
   Sort an array using case-insensitive natural sort */
PHP_FUNCTION(natcasesort)
{
	php_natsort(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto bool asort(array &array_arg [, int sort_flags])
   Sort an array and maintain index association */
PHP_FUNCTION(asort)
{
	zval *array;
	long sort_type = PHP_SORT_REGULAR;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|l", &array, &sort_type) == FAILURE) {
		RETURN_FALSE;
	}

	php_set_compare_func(sort_type TSRMLS_CC);

	if (zend_hash_sort(Z_ARRVAL_P(array), zend_qsort, php_array_data_compare, 0 TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool arsort(array &array_arg [, int sort_flags])
   Sort an array in reverse order and maintain index association */
PHP_FUNCTION(arsort)
{
	zval *array;
	long sort_type = PHP_SORT_REGULAR;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|l", &array, &sort_type) == FAILURE) {
		RETURN_FALSE;
	}

	php_set_compare_func(sort_type TSRMLS_CC);

	if (zend_hash_sort(Z_ARRVAL_P(array), zend_qsort, php_array_reverse_data_compare, 0 TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool sort(array &array_arg [, int sort_flags])
   Sort an array */
PHP_FUNCTION(sort)
{
	zval *array;
	long sort_type = PHP_SORT_REGULAR;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|l", &array, &sort_type) == FAILURE) {
		RETURN_FALSE;
	}

	php_set_compare_func(sort_type TSRMLS_CC);

	if (zend_hash_sort(Z_ARRVAL_P(array), zend_qsort, php_array_data_compare, 1 TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool rsort(array &array_arg [, int sort_flags])
   Sort an array in reverse order */
PHP_FUNCTION(rsort)
{
	zval *array;
	long sort_type = PHP_SORT_REGULAR;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|l", &array, &sort_type) == FAILURE) {
		RETURN_FALSE;
	}

	php_set_compare_func(sort_type TSRMLS_CC);

	if (zend_hash_sort(Z_ARRVAL_P(array), zend_qsort, php_array_reverse_data_compare, 1 TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

static int php_array_user_compare(const void *a, const void *b TSRMLS_DC) /* {{{ */
{
	Bucket *f;
	Bucket *s;
	zval **args[2];
	zval *retval_ptr = NULL;

	f = *((Bucket **) a);
	s = *((Bucket **) b);

	args[0] = (zval **) f->pData;
	args[1] = (zval **) s->pData;

	BG(user_compare_fci).param_count = 2;
	BG(user_compare_fci).params = args;
	BG(user_compare_fci).retval_ptr_ptr = &retval_ptr;
	BG(user_compare_fci).no_separation = 0;
	if (zend_call_function(&BG(user_compare_fci), &BG(user_compare_fci_cache) TSRMLS_CC) == SUCCESS && retval_ptr) {
		long retval;

		convert_to_long_ex(&retval_ptr);
		retval = Z_LVAL_P(retval_ptr);
		zval_ptr_dtor(&retval_ptr);
		return retval < 0 ? -1 : retval > 0 ? 1 : 0;
	} else {
		return 0;
	}
}
/* }}} */

/* check if comparison function is valid */
#define PHP_ARRAY_CMP_FUNC_CHECK(func_name)	\
	if (!zend_is_callable(*func_name, 0, NULL TSRMLS_CC)) {	\
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid comparison function");	\
		BG(user_compare_fci) = old_user_compare_fci; \
		BG(user_compare_fci_cache) = old_user_compare_fci_cache; \
		RETURN_FALSE;	\
	}	\

	/* Clear FCI cache otherwise : for example the same or other array with
	 * (partly) the same key values has been sorted with uasort() or
	 * other sorting function the comparison is cached, however the name
	 * of the function for comparison is not respected. see bug #28739 AND #33295
	 *
	 * Following defines will assist in backup / restore values. */

#define PHP_ARRAY_CMP_FUNC_VARS \
	zend_fcall_info old_user_compare_fci; \
	zend_fcall_info_cache old_user_compare_fci_cache \

#define PHP_ARRAY_CMP_FUNC_BACKUP() \
	old_user_compare_fci = BG(user_compare_fci); \
	old_user_compare_fci_cache = BG(user_compare_fci_cache); \
	BG(user_compare_fci_cache) = empty_fcall_info_cache; \

#define PHP_ARRAY_CMP_FUNC_RESTORE() \
	BG(user_compare_fci) = old_user_compare_fci; \
	BG(user_compare_fci_cache) = old_user_compare_fci_cache; \

/* {{{ proto bool usort(array array_arg, string cmp_function)
   Sort an array by values using a user-defined comparison function */
PHP_FUNCTION(usort)
{
	zval *array;
	unsigned int refcount;
	PHP_ARRAY_CMP_FUNC_VARS;

	PHP_ARRAY_CMP_FUNC_BACKUP();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "af", &array, &BG(user_compare_fci), &BG(user_compare_fci_cache)) == FAILURE) {
		PHP_ARRAY_CMP_FUNC_RESTORE();
		return;
	}

	/* Clear the is_ref flag, so the attemts to modify the array in user
	 * comparison function will create a copy of array and won't affect the
	 * original array. The fact of modification is detected using refcount
	 * comparison. The result of sorting in such case is undefined and the
	 * function returns FALSE.
	 */
	Z_UNSET_ISREF_P(array);
	refcount = Z_REFCOUNT_P(array);

	if (zend_hash_sort(Z_ARRVAL_P(array), zend_qsort, php_array_user_compare, 1 TSRMLS_CC) == FAILURE) {
		RETVAL_FALSE;
	} else {
		if (refcount > Z_REFCOUNT_P(array)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Array was modified by the user comparison function");
			RETVAL_FALSE;
		} else {
			RETVAL_TRUE;
		}
	}
	
	if (Z_REFCOUNT_P(array) > 1) {
		Z_SET_ISREF_P(array);
	}

	PHP_ARRAY_CMP_FUNC_RESTORE();
}
/* }}} */

/* {{{ proto bool uasort(array array_arg, string cmp_function)
   Sort an array with a user-defined comparison function and maintain index association */
PHP_FUNCTION(uasort)
{
	zval *array;
	unsigned int refcount;
	PHP_ARRAY_CMP_FUNC_VARS;

	PHP_ARRAY_CMP_FUNC_BACKUP();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "af", &array, &BG(user_compare_fci), &BG(user_compare_fci_cache)) == FAILURE) {
		PHP_ARRAY_CMP_FUNC_RESTORE();
		return;
	}

	/* Clear the is_ref flag, so the attemts to modify the array in user
	 * comaprison function will create a copy of array and won't affect the
	 * original array. The fact of modification is detected using refcount
	 * comparison. The result of sorting in such case is undefined and the
	 * function returns FALSE.
	 */
	Z_UNSET_ISREF_P(array);
	refcount = Z_REFCOUNT_P(array);

	if (zend_hash_sort(Z_ARRVAL_P(array), zend_qsort, php_array_user_compare, 0 TSRMLS_CC) == FAILURE) {
		RETVAL_FALSE;
	} else {
		if (refcount > Z_REFCOUNT_P(array)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Array was modified by the user comparison function");
			RETVAL_FALSE;
		} else {
			RETVAL_TRUE;
		}
	}

	if (Z_REFCOUNT_P(array) > 1) {
		Z_SET_ISREF_P(array);
	}

	PHP_ARRAY_CMP_FUNC_RESTORE();
}
/* }}} */

static int php_array_user_key_compare(const void *a, const void *b TSRMLS_DC) /* {{{ */
{
	Bucket *f;
	Bucket *s;
	zval *key1, *key2;
	zval **args[2];
	zval *retval_ptr = NULL;
	long result;

	ALLOC_INIT_ZVAL(key1);
	ALLOC_INIT_ZVAL(key2);
	args[0] = &key1;
	args[1] = &key2;

	f = *((Bucket **) a);
	s = *((Bucket **) b);

	if (f->nKeyLength == 0) {
		Z_LVAL_P(key1) = f->h;
		Z_TYPE_P(key1) = IS_LONG;
	} else {
		Z_STRVAL_P(key1) = estrndup(f->arKey, f->nKeyLength - 1);
		Z_STRLEN_P(key1) = f->nKeyLength - 1;
		Z_TYPE_P(key1) = IS_STRING;
	}
	if (s->nKeyLength == 0) {
		Z_LVAL_P(key2) = s->h;
		Z_TYPE_P(key2) = IS_LONG;
	} else {
		Z_STRVAL_P(key2) = estrndup(s->arKey, s->nKeyLength - 1);
		Z_STRLEN_P(key2) = s->nKeyLength - 1;
		Z_TYPE_P(key2) = IS_STRING;
	}

	BG(user_compare_fci).param_count = 2;
	BG(user_compare_fci).params = args;
	BG(user_compare_fci).retval_ptr_ptr = &retval_ptr;
	BG(user_compare_fci).no_separation = 0;
	if (zend_call_function(&BG(user_compare_fci), &BG(user_compare_fci_cache) TSRMLS_CC) == SUCCESS && retval_ptr) {
		convert_to_long_ex(&retval_ptr);
		result = Z_LVAL_P(retval_ptr);
		zval_ptr_dtor(&retval_ptr);
	} else {
		result = 0;
	}

	zval_ptr_dtor(&key1);
	zval_ptr_dtor(&key2);

	return result;
}
/* }}} */

/* {{{ proto bool uksort(array array_arg, string cmp_function)
   Sort an array by keys using a user-defined comparison function */
PHP_FUNCTION(uksort)
{
	zval *array;
	unsigned int refcount;
	PHP_ARRAY_CMP_FUNC_VARS;

	PHP_ARRAY_CMP_FUNC_BACKUP();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "af", &array, &BG(user_compare_fci), &BG(user_compare_fci_cache)) == FAILURE) {
		PHP_ARRAY_CMP_FUNC_RESTORE();
		return;
	}

	/* Clear the is_ref flag, so the attemts to modify the array in user
	 * comaprison function will create a copy of array and won't affect the
	 * original array. The fact of modification is detected using refcount
	 * comparison. The result of sorting in such case is undefined and the
	 * function returns FALSE.
	 */
	Z_UNSET_ISREF_P(array);
	refcount = Z_REFCOUNT_P(array);

	if (zend_hash_sort(Z_ARRVAL_P(array), zend_qsort, php_array_user_key_compare, 0 TSRMLS_CC) == FAILURE) {
		RETVAL_FALSE;
	} else {
		if (refcount > Z_REFCOUNT_P(array)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Array was modified by the user comparison function");
			RETVAL_FALSE;
		} else {
			RETVAL_TRUE;
		}
	}

	if (Z_REFCOUNT_P(array) > 1) {
		Z_SET_ISREF_P(array);
	}

	PHP_ARRAY_CMP_FUNC_RESTORE();
}
/* }}} */

/* {{{ proto mixed end(array array_arg)
   Advances array argument's internal pointer to the last element and return it */
PHP_FUNCTION(end)
{
	HashTable *array;
	zval **entry;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "H", &array) == FAILURE) {
		return;
	}

	zend_hash_internal_pointer_end(array);

	if (return_value_used) {
		if (zend_hash_get_current_data(array, (void **) &entry) == FAILURE) {
			RETURN_FALSE;
		}

		RETURN_ZVAL_FAST(*entry);
	}
}
/* }}} */

/* {{{ proto mixed prev(array array_arg)
   Move array argument's internal pointer to the previous element and return it */
PHP_FUNCTION(prev)
{
	HashTable *array;
	zval **entry;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "H", &array) == FAILURE) {
		return;
	}

	zend_hash_move_backwards(array);

	if (return_value_used) {
		if (zend_hash_get_current_data(array, (void **) &entry) == FAILURE) {
			RETURN_FALSE;
		}

		RETURN_ZVAL_FAST(*entry);
	}
}
/* }}} */

/* {{{ proto mixed next(array array_arg)
   Move array argument's internal pointer to the next element and return it */
PHP_FUNCTION(next)
{
	HashTable *array;
	zval **entry;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "H", &array) == FAILURE) {
		return;
	}

	zend_hash_move_forward(array);

	if (return_value_used) {
		if (zend_hash_get_current_data(array, (void **) &entry) == FAILURE) {
			RETURN_FALSE;
		}

		RETURN_ZVAL_FAST(*entry);
	}
}
/* }}} */

/* {{{ proto mixed reset(array array_arg)
   Set array argument's internal pointer to the first element and return it */
PHP_FUNCTION(reset)
{
	HashTable *array;
	zval **entry;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "H", &array) == FAILURE) {
		return;
	}

	zend_hash_internal_pointer_reset(array);

	if (return_value_used) {
		if (zend_hash_get_current_data(array, (void **) &entry) == FAILURE) {
			RETURN_FALSE;
		}

		RETURN_ZVAL_FAST(*entry);
	}
}
/* }}} */

/* {{{ proto mixed current(array array_arg)
   Return the element currently pointed to by the internal array pointer */
PHP_FUNCTION(current)
{
	HashTable *array;
	zval **entry;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "H", &array) == FAILURE) {
		return;
	}

	if (zend_hash_get_current_data(array, (void **) &entry) == FAILURE) {
		RETURN_FALSE;
	}

	RETURN_ZVAL_FAST(*entry);
}
/* }}} */

/* {{{ proto mixed key(array array_arg)
   Return the key of the element currently pointed to by the internal array pointer */
PHP_FUNCTION(key)
{
	HashTable *array;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "H", &array) == FAILURE) {
		return;
	}

	zend_hash_get_current_key_zval(array, return_value);
}
/* }}} */

/* {{{ proto mixed min(mixed arg1 [, mixed arg2 [, mixed ...]])
   Return the lowest value in an array or a series of arguments */
PHP_FUNCTION(min)
{
	int argc;
	zval ***args = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "+", &args, &argc) == FAILURE) {
		return;
	}
	
	php_set_compare_func(PHP_SORT_REGULAR TSRMLS_CC);
	
	/* mixed min ( array $values ) */
	if (argc == 1) {
		zval **result;
		
		if (Z_TYPE_PP(args[0]) != IS_ARRAY) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "When only one parameter is given, it must be an array");
			RETVAL_NULL();
		} else {
			if (zend_hash_minmax(Z_ARRVAL_PP(args[0]), php_array_data_compare, 0, (void **) &result TSRMLS_CC) == SUCCESS) {
				RETVAL_ZVAL_FAST(*result);
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Array must contain at least one element");
				RETVAL_FALSE;
			}
		}
	} else {
		/* mixed min ( mixed $value1 , mixed $value2 [, mixed $value3... ] ) */
		zval **min, result;
		int i;

		min = args[0];

		for (i = 1; i < argc; i++) {
			is_smaller_function(&result, *args[i], *min TSRMLS_CC);
			if (Z_LVAL(result) == 1) {
				min = args[i];
			}
		}

		RETVAL_ZVAL_FAST(*min);
	}

	if (args) {
		efree(args);
	}
}
/* }}} */

/* {{{ proto mixed max(mixed arg1 [, mixed arg2 [, mixed ...]])
   Return the highest value in an array or a series of arguments */
PHP_FUNCTION(max)
{
	zval ***args = NULL;
	int argc;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "+", &args, &argc) == FAILURE) {
		return;
	}

	php_set_compare_func(PHP_SORT_REGULAR TSRMLS_CC);
	
	/* mixed max ( array $values ) */
	if (argc == 1) {
		zval **result;

		if (Z_TYPE_PP(args[0]) != IS_ARRAY) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "When only one parameter is given, it must be an array");
			RETVAL_NULL();
		} else {
			if (zend_hash_minmax(Z_ARRVAL_PP(args[0]), php_array_data_compare, 1, (void **) &result TSRMLS_CC) == SUCCESS) {
				RETVAL_ZVAL_FAST(*result);
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Array must contain at least one element");
				RETVAL_FALSE;
			}
		}
	} else {
		/* mixed max ( mixed $value1 , mixed $value2 [, mixed $value3... ] ) */
		zval **max, result;
		int i;

		max = args[0];

		for (i = 1; i < argc; i++) {
			is_smaller_or_equal_function(&result, *args[i], *max TSRMLS_CC);
			if (Z_LVAL(result) == 0) {
				max = args[i];
			}
		}

		RETVAL_ZVAL_FAST(*max);
	}
	
	if (args) {
		efree(args);
	}
}
/* }}} */

static int php_array_walk(HashTable *target_hash, zval *userdata, int recursive TSRMLS_DC) /* {{{ */
{
	zval **args[3],			/* Arguments to userland function */
		  *retval_ptr = NULL,		/* Return value - unused */
		  *key=NULL;		/* Entry key */

	/* Set up known arguments */
	args[1] = &key;
	args[2] = &userdata;
	if (userdata) {
		Z_ADDREF_P(userdata);
	}

	BG(array_walk_fci).retval_ptr_ptr = &retval_ptr;
	BG(array_walk_fci).param_count = userdata ? 3 : 2;
	BG(array_walk_fci).params = args;
	BG(array_walk_fci).no_separation = 0;
	
	/* Iterate through hash */
	zend_hash_internal_pointer_reset(target_hash);
	while (!EG(exception) && zend_hash_get_current_data(target_hash, (void **)&args[0]) == SUCCESS) {
		if (recursive && Z_TYPE_PP(args[0]) == IS_ARRAY) {
			HashTable *thash;
			zend_fcall_info orig_array_walk_fci;
			zend_fcall_info_cache orig_array_walk_fci_cache;

			SEPARATE_ZVAL_IF_NOT_REF(args[0]);
			thash = Z_ARRVAL_PP(args[0]);
			if (thash->nApplyCount > 1) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "recursion detected");
				if (userdata) {
					zval_ptr_dtor(&userdata);
				}
				return 0;
			}

			/* backup the fcall info and cache */
			orig_array_walk_fci = BG(array_walk_fci);
			orig_array_walk_fci_cache = BG(array_walk_fci_cache);

			thash->nApplyCount++;
			php_array_walk(thash, userdata, recursive TSRMLS_CC);
			thash->nApplyCount--;

			/* restore the fcall info and cache */
			BG(array_walk_fci) = orig_array_walk_fci;
			BG(array_walk_fci_cache) = orig_array_walk_fci_cache;
		} else {
			/* Allocate space for key */
			MAKE_STD_ZVAL(key);
			zend_hash_get_current_key_zval(target_hash, key);

			/* Call the userland function */
			if (zend_call_function(&BG(array_walk_fci), &BG(array_walk_fci_cache) TSRMLS_CC) == SUCCESS) {
				if (retval_ptr) {
					zval_ptr_dtor(&retval_ptr);
				}
			} else {
				if (key) {
					zval_ptr_dtor(&key);
					key = NULL;
				}
				break;
			}
		}

		if (key) {
			zval_ptr_dtor(&key);
			key = NULL;
		}
		zend_hash_move_forward(target_hash);
	}

	if (userdata) {
		zval_ptr_dtor(&userdata);
	}
	return 0;
}
/* }}} */

/* {{{ proto bool array_walk(array input, string funcname [, mixed userdata])
   Apply a user function to every member of an array */
PHP_FUNCTION(array_walk)
{
	HashTable *array;
	zval *userdata = NULL;
	zend_fcall_info orig_array_walk_fci;
	zend_fcall_info_cache orig_array_walk_fci_cache;

	orig_array_walk_fci = BG(array_walk_fci);
	orig_array_walk_fci_cache = BG(array_walk_fci_cache);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Hf|z/", &array, &BG(array_walk_fci), &BG(array_walk_fci_cache), &userdata) == FAILURE) {
		BG(array_walk_fci) = orig_array_walk_fci;
		BG(array_walk_fci_cache) = orig_array_walk_fci_cache;
		return;
	}

	php_array_walk(array, userdata, 0 TSRMLS_CC);
	BG(array_walk_fci) = orig_array_walk_fci;
	BG(array_walk_fci_cache) = orig_array_walk_fci_cache;
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool array_walk_recursive(array input, string funcname [, mixed userdata])
   Apply a user function recursively to every member of an array */
PHP_FUNCTION(array_walk_recursive)
{
	HashTable *array;
	zval *userdata = NULL;
	zend_fcall_info orig_array_walk_fci;
	zend_fcall_info_cache orig_array_walk_fci_cache;

	orig_array_walk_fci = BG(array_walk_fci);
	orig_array_walk_fci_cache = BG(array_walk_fci_cache);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Hf|z/", &array, &BG(array_walk_fci), &BG(array_walk_fci_cache), &userdata) == FAILURE) {
		BG(array_walk_fci) = orig_array_walk_fci;
		BG(array_walk_fci_cache) = orig_array_walk_fci_cache;
		return;
	}

	php_array_walk(array, userdata, 1 TSRMLS_CC);
	BG(array_walk_fci) = orig_array_walk_fci;
	BG(array_walk_fci_cache) = orig_array_walk_fci_cache;
	RETURN_TRUE;
}
/* }}} */

/* void php_search_array(INTERNAL_FUNCTION_PARAMETERS, int behavior)
 * 0 = return boolean
 * 1 = return key
 */
static void php_search_array(INTERNAL_FUNCTION_PARAMETERS, int behavior) /* {{{ */
{
	zval *value,				/* value to check for */
		 *array,				/* array to check in */
		 **entry,				/* pointer to array entry */
		  res;					/* comparison result */
	HashPosition pos;			/* hash iterator */
	zend_bool strict = 0;		/* strict comparison or not */
	int (*is_equal_func)(zval *, zval *, zval * TSRMLS_DC) = is_equal_function;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "za|b", &value, &array, &strict) == FAILURE) {
		return;
	}

	if (strict) {
		is_equal_func = is_identical_function;
	}

	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(array), &pos);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(array), (void **)&entry, &pos) == SUCCESS) {
		is_equal_func(&res, value, *entry TSRMLS_CC);
		if (Z_LVAL(res)) {
			if (behavior == 0) {
				RETURN_TRUE;
			} else {
				zend_hash_get_current_key_zval_ex(Z_ARRVAL_P(array), return_value, &pos);
				return;
			}
		}
		zend_hash_move_forward_ex(Z_ARRVAL_P(array), &pos);
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool in_array(mixed needle, array haystack [, bool strict])
   Checks if the given value exists in the array */
PHP_FUNCTION(in_array)
{
	php_search_array(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto mixed array_search(mixed needle, array haystack [, bool strict])
   Searches the array for a given value and returns the corresponding key if successful */
PHP_FUNCTION(array_search)
{
	php_search_array(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

static int php_valid_var_name(char *var_name, int var_name_len) /* {{{ */
{
	int i, ch;

	if (!var_name || !var_name_len) {
		return 0;
	}
	
	/* These are allowed as first char: [a-zA-Z_\x7f-\xff] */
	ch = (int)((unsigned char *)var_name)[0];
	if (var_name[0] != '_' &&
		(ch < 65  /* A    */ || /* Z    */ ch > 90)  &&
		(ch < 97  /* a    */ || /* z    */ ch > 122) &&
		(ch < 127 /* 0x7f */ || /* 0xff */ ch > 255)
	) {
		return 0;
	}

	/* And these as the rest: [a-zA-Z0-9_\x7f-\xff] */
	if (var_name_len > 1) {
		for (i = 1; i < var_name_len; i++) {
			ch = (int)((unsigned char *)var_name)[i];
			if (var_name[i] != '_' &&
				(ch < 48  /* 0    */ || /* 9    */ ch > 57)  &&
				(ch < 65  /* A    */ || /* Z    */ ch > 90)  &&
				(ch < 97  /* a    */ || /* z    */ ch > 122) &&
				(ch < 127 /* 0x7f */ || /* 0xff */ ch > 255)
			) {
				return 0;
			}
		}
	}
	return 1;
}
/* }}} */

PHPAPI int php_prefix_varname(zval *result, zval *prefix, char *var_name, int var_name_len, zend_bool add_underscore TSRMLS_DC) /* {{{ */
{
	Z_STRLEN_P(result) = Z_STRLEN_P(prefix) + (add_underscore ? 1 : 0) + var_name_len;
	Z_TYPE_P(result) = IS_STRING;
	Z_STRVAL_P(result) = emalloc(Z_STRLEN_P(result) + 1);
	memcpy(Z_STRVAL_P(result), Z_STRVAL_P(prefix), Z_STRLEN_P(prefix));

	if (add_underscore) {
		Z_STRVAL_P(result)[Z_STRLEN_P(prefix)] = '_';
	}

	memcpy(Z_STRVAL_P(result) + Z_STRLEN_P(prefix) + (add_underscore ? 1 : 0), var_name, var_name_len + 1);

	return SUCCESS;
}
/* }}} */

/* {{{ proto int extract(array var_array [, int extract_type [, string prefix]])
   Imports variables into symbol table from an array */
PHP_FUNCTION(extract)
{
	zval *var_array, *prefix = NULL;
	long extract_type = EXTR_OVERWRITE;
	zval **entry, *data;
	char *var_name;
	ulong num_key;
	uint var_name_len;
	int var_exists, key_type, count = 0;
	int extract_refs = 0;
	HashPosition pos;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|lz/", &var_array, &extract_type, &prefix) == FAILURE) {
		return;
	}

	extract_refs = (extract_type & EXTR_REFS);
	extract_type &= 0xff;

	if (extract_type < EXTR_OVERWRITE || extract_type > EXTR_IF_EXISTS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid extract type");
		return;
	}

	if (extract_type > EXTR_SKIP && extract_type <= EXTR_PREFIX_IF_EXISTS && ZEND_NUM_ARGS() < 3) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "specified extract type requires the prefix parameter");
		return;
	}

	if (prefix) {
		convert_to_string(prefix);
		if (Z_STRLEN_P(prefix) && !php_valid_var_name(Z_STRVAL_P(prefix), Z_STRLEN_P(prefix))) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "prefix is not a valid identifier");
			return;
		}
	}

	if (!EG(active_symbol_table)) {
		zend_rebuild_symbol_table(TSRMLS_C);
	}

	/* var_array is passed by ref for the needs of EXTR_REFS (needs to
	 * work on the original array to create refs to its members)
	 * simulate pass_by_value if EXTR_REFS is not used */
	if (!extract_refs) {
		SEPARATE_ARG_IF_REF(var_array);
	}

	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(var_array), &pos);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(var_array), (void **)&entry, &pos) == SUCCESS) {
		zval final_name;

		ZVAL_NULL(&final_name);

		key_type = zend_hash_get_current_key_ex(Z_ARRVAL_P(var_array), &var_name, &var_name_len, &num_key, 0, &pos);
		var_exists = 0;

		if (key_type == HASH_KEY_IS_STRING) {
			var_name_len--;
			var_exists = zend_hash_exists(EG(active_symbol_table), var_name, var_name_len + 1);
		} else if (key_type == HASH_KEY_IS_LONG && (extract_type == EXTR_PREFIX_ALL || extract_type == EXTR_PREFIX_INVALID)) {
			zval num;

			ZVAL_LONG(&num, num_key);
			convert_to_string(&num);
			php_prefix_varname(&final_name, prefix, Z_STRVAL(num), Z_STRLEN(num), 1 TSRMLS_CC);
			zval_dtor(&num);
		} else {
			zend_hash_move_forward_ex(Z_ARRVAL_P(var_array), &pos);
			continue;
		}

		switch (extract_type) {
			case EXTR_IF_EXISTS:
				if (!var_exists) break;
				/* break omitted intentionally */

			case EXTR_OVERWRITE:
				/* GLOBALS protection */
				if (var_exists && var_name_len == sizeof("GLOBALS")-1 && !strcmp(var_name, "GLOBALS")) {
					break;
				}
				if (var_exists && var_name_len == sizeof("this")-1  && !strcmp(var_name, "this") && EG(scope) && EG(scope)->name_length != 0) {
					break;
				}
				ZVAL_STRINGL(&final_name, var_name, var_name_len, 1);
				break;

			case EXTR_PREFIX_IF_EXISTS:
				if (var_exists) {
					php_prefix_varname(&final_name, prefix, var_name, var_name_len, 1 TSRMLS_CC);
				}
				break;

			case EXTR_PREFIX_SAME:
				if (!var_exists && var_name_len != 0) {
					ZVAL_STRINGL(&final_name, var_name, var_name_len, 1);
				}
				/* break omitted intentionally */

			case EXTR_PREFIX_ALL:
				if (Z_TYPE(final_name) == IS_NULL && var_name_len != 0) {
					php_prefix_varname(&final_name, prefix, var_name, var_name_len, 1 TSRMLS_CC);
				}
				break;

			case EXTR_PREFIX_INVALID:
				if (Z_TYPE(final_name) == IS_NULL) {
					if (!php_valid_var_name(var_name, var_name_len)) {
						php_prefix_varname(&final_name, prefix, var_name, var_name_len, 1 TSRMLS_CC);
					} else {
						ZVAL_STRINGL(&final_name, var_name, var_name_len, 1);
					}
				}
				break;

			default:
				if (!var_exists) {
					ZVAL_STRINGL(&final_name, var_name, var_name_len, 1);
				}
				break;
		}

		if (Z_TYPE(final_name) != IS_NULL && php_valid_var_name(Z_STRVAL(final_name), Z_STRLEN(final_name))) {
			if (extract_refs) {
				zval **orig_var;

				SEPARATE_ZVAL_TO_MAKE_IS_REF(entry);
				zval_add_ref(entry);

				if (zend_hash_find(EG(active_symbol_table), Z_STRVAL(final_name), Z_STRLEN(final_name) + 1, (void **) &orig_var) == SUCCESS) {
					zval_ptr_dtor(orig_var);
					*orig_var = *entry;
				} else {
					zend_hash_update(EG(active_symbol_table), Z_STRVAL(final_name), Z_STRLEN(final_name) + 1, (void **) entry, sizeof(zval *), NULL);
				}
			} else {
				MAKE_STD_ZVAL(data);
				*data = **entry;
				zval_copy_ctor(data);

				ZEND_SET_SYMBOL_WITH_LENGTH(EG(active_symbol_table), Z_STRVAL(final_name), Z_STRLEN(final_name) + 1, data, 1, 0);
			}
			count++;
		}
		zval_dtor(&final_name);

		zend_hash_move_forward_ex(Z_ARRVAL_P(var_array), &pos);
	}

	if (!extract_refs) {
		zval_ptr_dtor(&var_array);
	}

	RETURN_LONG(count);
}
/* }}} */

static void php_compact_var(HashTable *eg_active_symbol_table, zval *return_value, zval *entry TSRMLS_DC) /* {{{ */
{
	zval **value_ptr, *value, *data;

	if (Z_TYPE_P(entry) == IS_STRING) {
		if (zend_hash_find(eg_active_symbol_table, Z_STRVAL_P(entry), Z_STRLEN_P(entry) + 1, (void **)&value_ptr) != FAILURE) {
			value = *value_ptr;
			ALLOC_ZVAL(data);
			MAKE_COPY_ZVAL(&value, data);

			zend_hash_update(Z_ARRVAL_P(return_value), Z_STRVAL_P(entry), Z_STRLEN_P(entry) + 1, &data, sizeof(zval *), NULL);
		}
	}
	else if (Z_TYPE_P(entry) == IS_ARRAY) {
		HashPosition pos;

		if ((Z_ARRVAL_P(entry)->nApplyCount > 1)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "recursion detected");
			return;
		}

		Z_ARRVAL_P(entry)->nApplyCount++;

		zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(entry), &pos);
		while (zend_hash_get_current_data_ex(Z_ARRVAL_P(entry), (void**)&value_ptr, &pos) == SUCCESS) {
			value = *value_ptr;

			php_compact_var(eg_active_symbol_table, return_value, value TSRMLS_CC);
			zend_hash_move_forward_ex(Z_ARRVAL_P(entry), &pos);
		}
		Z_ARRVAL_P(entry)->nApplyCount--;
	}
}
/* }}} */

/* {{{ proto array compact(mixed var_names [, mixed ...])
   Creates a hash containing variables and their values */
PHP_FUNCTION(compact)
{
	zval ***args = NULL;	/* function arguments array */
	int num_args, i;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "+", &args, &num_args) == FAILURE) {
		return;
	}

	if (!EG(active_symbol_table)) {
		zend_rebuild_symbol_table(TSRMLS_C);
	}

	/* compact() is probably most used with a single array of var_names
	   or multiple string names, rather than a combination of both.
	   So quickly guess a minimum result size based on that */
	if (ZEND_NUM_ARGS() == 1 && Z_TYPE_PP(args[0]) == IS_ARRAY) {
		array_init_size(return_value, zend_hash_num_elements(Z_ARRVAL_PP(args[0])));
	} else {
		array_init_size(return_value, ZEND_NUM_ARGS());
	}

	for (i=0; i<ZEND_NUM_ARGS(); i++) {
		php_compact_var(EG(active_symbol_table), return_value, *args[i] TSRMLS_CC);
	}

	if (args) {
		efree(args);
	}
}
/* }}} */

/* {{{ proto array array_fill(int start_key, int num, mixed val)
   Create an array containing num elements starting with index start_key each initialized to val */
PHP_FUNCTION(array_fill)
{
	zval *val;
	long start_key, num;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llz", &start_key, &num, &val) == FAILURE) {
		return;
	}

	if (num < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Number of elements can't be negative");
		RETURN_FALSE;
	}

	/* allocate an array for return */
	array_init_size(return_value, num);

	if (num == 0) {
		return;
	}

	num--;
	zend_hash_index_update(Z_ARRVAL_P(return_value), start_key, &val, sizeof(zval *), NULL);
	zval_add_ref(&val);

	while (num--) {
		if (zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &val, sizeof(zval *), NULL) == SUCCESS) {
			zval_add_ref(&val);
		} else {
			zval_dtor(return_value);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot add element to the array as the next element is already occupied");
			RETURN_FALSE;
		}
	}
}
/* }}} */

/* {{{ proto array array_fill_keys(array keys, mixed val)
   Create an array using the elements of the first parameter as keys each initialized to val */
PHP_FUNCTION(array_fill_keys)
{
	zval *keys, *val, **entry;
	HashPosition pos;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "az", &keys, &val) == FAILURE) {
		return;
	}

	/* Initialize return array */
	array_init_size(return_value, zend_hash_num_elements(Z_ARRVAL_P(keys)));

	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(keys), &pos);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(keys), (void **)&entry, &pos) == SUCCESS) {

		if (Z_TYPE_PP(entry) == IS_LONG) {
			zval_add_ref(&val);
			zend_hash_index_update(Z_ARRVAL_P(return_value), Z_LVAL_PP(entry), &val, sizeof(zval *), NULL);
		} else {
			zval key, *key_ptr = *entry;

			if (Z_TYPE_PP(entry) != IS_STRING) {
				key = **entry;
				zval_copy_ctor(&key);
				convert_to_string(&key);
				key_ptr = &key;
			}

			zval_add_ref(&val);
			zend_symtable_update(Z_ARRVAL_P(return_value), Z_STRVAL_P(key_ptr), Z_STRLEN_P(key_ptr) + 1, &val, sizeof(zval *), NULL);

			if (key_ptr != *entry) {
				zval_dtor(&key);
			}
		}

		zend_hash_move_forward_ex(Z_ARRVAL_P(keys), &pos);
	}
}
/* }}} */

/* {{{ proto array range(mixed low, mixed high[, int step])
   Create an array containing the range of integers or characters from low to high (inclusive) */
PHP_FUNCTION(range)
{
	zval *zlow, *zhigh, *zstep = NULL;
	int err = 0, is_step_double = 0;
	double step = 1.0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z/z/|z/", &zlow, &zhigh, &zstep) == FAILURE) {
		RETURN_FALSE;
	}

	if (zstep) {
		if (Z_TYPE_P(zstep) == IS_DOUBLE ||
			(Z_TYPE_P(zstep) == IS_STRING && is_numeric_string(Z_STRVAL_P(zstep), Z_STRLEN_P(zstep), NULL, NULL, 0) == IS_DOUBLE)
		) {
			is_step_double = 1;
		}

		convert_to_double_ex(&zstep);
		step = Z_DVAL_P(zstep);

		/* We only want positive step values. */
		if (step < 0.0) {
			step *= -1;
		}
	}

	/* Initialize the return_value as an array. */
	array_init(return_value);

	/* If the range is given as strings, generate an array of characters. */
	if (Z_TYPE_P(zlow) == IS_STRING && Z_TYPE_P(zhigh) == IS_STRING && Z_STRLEN_P(zlow) >= 1 && Z_STRLEN_P(zhigh) >= 1) {
		int type1, type2;
		unsigned char *low, *high;
		long lstep = (long) step;

		type1 = is_numeric_string(Z_STRVAL_P(zlow), Z_STRLEN_P(zlow), NULL, NULL, 0);
		type2 = is_numeric_string(Z_STRVAL_P(zhigh), Z_STRLEN_P(zhigh), NULL, NULL, 0);

		if (type1 == IS_DOUBLE || type2 == IS_DOUBLE || is_step_double) {
			goto double_str;
		} else if (type1 == IS_LONG || type2 == IS_LONG) {
			goto long_str;
		}

		convert_to_string(zlow);
		convert_to_string(zhigh);
		low = (unsigned char *)Z_STRVAL_P(zlow);
		high = (unsigned char *)Z_STRVAL_P(zhigh);

		if (*low > *high) {		/* Negative Steps */
			unsigned char ch = *low;

			if (lstep <= 0) {
				err = 1;
				goto err;
			}
			for (; ch >= *high; ch -= (unsigned int)lstep) {
				add_next_index_stringl(return_value, (const char *)&ch, 1, 1);
				if (((signed int)ch - lstep) < 0) {
					break;
				}
			}
		} else if (*high > *low) {	/* Positive Steps */
			unsigned char ch = *low;

			if (lstep <= 0) {
				err = 1;
				goto err;
			}
			for (; ch <= *high; ch += (unsigned int)lstep) {
				add_next_index_stringl(return_value, (const char *)&ch, 1, 1);
				if (((signed int)ch + lstep) > 255) {
					break;
				}
			}
		} else {
			add_next_index_stringl(return_value, (const char *)low, 1, 1);
		}

	} else if (Z_TYPE_P(zlow) == IS_DOUBLE || Z_TYPE_P(zhigh) == IS_DOUBLE || is_step_double) {
		double low, high, value;
		long i;
double_str:
		convert_to_double(zlow);
		convert_to_double(zhigh);
		low = Z_DVAL_P(zlow);
		high = Z_DVAL_P(zhigh);
		i = 0;

		if (low > high) { 		/* Negative steps */
			if (low - high < step || step <= 0) {
				err = 1;
				goto err;
			}

			for (value = low; value >= (high - DOUBLE_DRIFT_FIX); value = low - (++i * step)) {
				add_next_index_double(return_value, value);
			}
		} else if (high > low) { 	/* Positive steps */
			if (high - low < step || step <= 0) {
				err = 1;
				goto err;
			}

			for (value = low; value <= (high + DOUBLE_DRIFT_FIX); value = low + (++i * step)) {
				add_next_index_double(return_value, value);
			}
		} else {
			add_next_index_double(return_value, low);
		}
	} else {
		double low, high;
		long lstep;
long_str:
		convert_to_double(zlow);
		convert_to_double(zhigh);
		low = Z_DVAL_P(zlow);
		high = Z_DVAL_P(zhigh);
		lstep = (long) step;

		if (low > high) { 		/* Negative steps */
			if (low - high < lstep || lstep <= 0) {
				err = 1;
				goto err;
			}
			for (; low >= high; low -= lstep) {
				add_next_index_long(return_value, (long)low);
			}
		} else if (high > low) { 	/* Positive steps */
			if (high - low < lstep || lstep <= 0) {
				err = 1;
				goto err;
			}
			for (; low <= high; low += lstep) {
				add_next_index_long(return_value, (long)low);
			}
		} else {
			add_next_index_long(return_value, (long)low);
		}
	}
err:
	if (err) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "step exceeds the specified range");
		zval_dtor(return_value);
		RETURN_FALSE;
	}
}
/* }}} */

static void php_array_data_shuffle(zval *array TSRMLS_DC) /* {{{ */
{
	Bucket **elems, *temp;
	HashTable *hash;
	int j, n_elems, rnd_idx, n_left;

	n_elems = zend_hash_num_elements(Z_ARRVAL_P(array));

	if (n_elems < 1) {
		return;
	}

	elems = (Bucket **)safe_emalloc(n_elems, sizeof(Bucket *), 0);
	hash = Z_ARRVAL_P(array);
	n_left = n_elems;

	for (j = 0, temp = hash->pListHead; temp; temp = temp->pListNext)
		elems[j++] = temp;
	while (--n_left) {
		rnd_idx = php_rand(TSRMLS_C);
		RAND_RANGE(rnd_idx, 0, n_left, PHP_RAND_MAX);
		if (rnd_idx != n_left) {
			temp = elems[n_left];
			elems[n_left] = elems[rnd_idx];
			elems[rnd_idx] = temp;
		}
	}

	HANDLE_BLOCK_INTERRUPTIONS();
	hash->pListHead = elems[0];
	hash->pListTail = NULL;
	hash->pInternalPointer = hash->pListHead;

	for (j = 0; j < n_elems; j++) {
		if (hash->pListTail) {
			hash->pListTail->pListNext = elems[j];
		}
		elems[j]->pListLast = hash->pListTail;
		elems[j]->pListNext = NULL;
		hash->pListTail = elems[j];
	}
	temp = hash->pListHead;
	j = 0;
	while (temp != NULL) {
		temp->nKeyLength = 0;
		temp->h = j++;
		temp = temp->pListNext;
	}
	hash->nNextFreeElement = n_elems;
	zend_hash_rehash(hash);
	HANDLE_UNBLOCK_INTERRUPTIONS();

	efree(elems);
}
/* }}} */

/* {{{ proto bool shuffle(array array_arg)
   Randomly shuffle the contents of an array */
PHP_FUNCTION(shuffle)
{
	zval *array;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &array) == FAILURE) {
		RETURN_FALSE;
	}

	php_array_data_shuffle(array TSRMLS_CC);

	RETURN_TRUE;
}
/* }}} */

PHPAPI HashTable* php_splice(HashTable *in_hash, int offset, int length, zval ***list, int list_count, HashTable **removed) /* {{{ */
{
	HashTable 	*out_hash = NULL;	/* Output hashtable */
	int			 num_in,			/* Number of entries in the input hashtable */
				 pos,				/* Current position in the hashtable */
				 i;					/* Loop counter */
	Bucket		*p;					/* Pointer to hash bucket */
	zval		*entry;				/* Hash entry */

	/* If input hash doesn't exist, we have nothing to do */
	if (!in_hash) {
		return NULL;
	}

	/* Get number of entries in the input hash */
	num_in = zend_hash_num_elements(in_hash);

	/* Clamp the offset.. */
	if (offset > num_in) {
		offset = num_in;
	} else if (offset < 0 && (offset = (num_in + offset)) < 0) {
		offset = 0;
	}

	/* ..and the length */
	if (length < 0) {
		length = num_in - offset + length;
	} else if (((unsigned)offset + (unsigned)length) > (unsigned)num_in) {
		length = num_in - offset;
	}

	/* Create and initialize output hash */
	ALLOC_HASHTABLE(out_hash);
	zend_hash_init(out_hash, (length > 0 ? num_in - length : 0) + list_count, NULL, ZVAL_PTR_DTOR, 0);

	/* Start at the beginning of the input hash and copy entries to output hash until offset is reached */
	for (pos = 0, p = in_hash->pListHead; pos < offset && p ; pos++, p = p->pListNext) {
		/* Get entry and increase reference count */
		entry = *((zval **)p->pData);
		Z_ADDREF_P(entry);

		/* Update output hash depending on key type */
		if (p->nKeyLength == 0) {
			zend_hash_next_index_insert(out_hash, &entry, sizeof(zval *), NULL);
		} else {
			zend_hash_quick_update(out_hash, p->arKey, p->nKeyLength, p->h, &entry, sizeof(zval *), NULL);
		}
	}

	/* If hash for removed entries exists, go until offset+length and copy the entries to it */
	if (removed != NULL) {
		for ( ; pos < offset + length && p; pos++, p = p->pListNext) {
			entry = *((zval **)p->pData);
			Z_ADDREF_P(entry);
			if (p->nKeyLength == 0) {
				zend_hash_next_index_insert(*removed, &entry, sizeof(zval *), NULL);
			} else {
				zend_hash_quick_update(*removed, p->arKey, p->nKeyLength, p->h, &entry, sizeof(zval *), NULL);
			}
		}
	} else { /* otherwise just skip those entries */
		for ( ; pos < offset + length && p; pos++, p = p->pListNext);
	}

	/* If there are entries to insert.. */
	if (list != NULL) {
		/* ..for each one, create a new zval, copy entry into it and copy it into the output hash */
		for (i = 0; i < list_count; i++) {
			entry = *list[i];
			Z_ADDREF_P(entry);
			zend_hash_next_index_insert(out_hash, &entry, sizeof(zval *), NULL);
		}
	}

	/* Copy the remaining input hash entries to the output hash */
	for ( ; p ; p = p->pListNext) {
		entry = *((zval **)p->pData);
		Z_ADDREF_P(entry);
		if (p->nKeyLength == 0) {
			zend_hash_next_index_insert(out_hash, &entry, sizeof(zval *), NULL);
		} else {
			zend_hash_quick_update(out_hash, p->arKey, p->nKeyLength, p->h, &entry, sizeof(zval *), NULL);
		}
	}

	zend_hash_internal_pointer_reset(out_hash);
	return out_hash;
}
/* }}} */

/* {{{ proto int array_push(array stack, mixed var [, mixed ...])
   Pushes elements onto the end of the array */
PHP_FUNCTION(array_push)
{
	zval ***args,		/* Function arguments array */
		   *stack,		/* Input array */
		   *new_var;	/* Variable to be pushed */
	int i,				/* Loop counter */
		argc;			/* Number of function arguments */


	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a+", &stack, &args, &argc) == FAILURE) {
		return;
	}

	/* For each subsequent argument, make it a reference, increase refcount, and add it to the end of the array */
	for (i = 0; i < argc; i++) {
		new_var = *args[i];
		Z_ADDREF_P(new_var);

		if (zend_hash_next_index_insert(Z_ARRVAL_P(stack), &new_var, sizeof(zval *), NULL) == FAILURE) {
			Z_DELREF_P(new_var);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot add element to the array as the next element is already occupied");
			efree(args);
			RETURN_FALSE;
		}
	}

	/* Clean up and return the number of values in the stack */
	efree(args);
	RETVAL_LONG(zend_hash_num_elements(Z_ARRVAL_P(stack)));
}
/* }}} */

/* {{{ void _phpi_pop(INTERNAL_FUNCTION_PARAMETERS, int off_the_end) */
static void _phpi_pop(INTERNAL_FUNCTION_PARAMETERS, int off_the_end)
{
	zval *stack,	/* Input stack */
		 **val;		/* Value to be popped */
	char *key = NULL;
	uint key_len = 0;
	ulong index;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &stack) == FAILURE) {
		return;
	}

	if (zend_hash_num_elements(Z_ARRVAL_P(stack)) == 0) {
		return;
	}

	/* Get the first or last value and copy it into the return value */
	if (off_the_end) {
		zend_hash_internal_pointer_end(Z_ARRVAL_P(stack));
	} else {
		zend_hash_internal_pointer_reset(Z_ARRVAL_P(stack));
	}
	zend_hash_get_current_data(Z_ARRVAL_P(stack), (void **)&val);
	RETVAL_ZVAL_FAST(*val);

	/* Delete the first or last value */
	zend_hash_get_current_key_ex(Z_ARRVAL_P(stack), &key, &key_len, &index, 0, NULL);
	if (key && Z_ARRVAL_P(stack) == &EG(symbol_table)) {
		zend_delete_global_variable(key, key_len - 1 TSRMLS_CC);
	} else {
		zend_hash_del_key_or_index(Z_ARRVAL_P(stack), key, key_len, index, (key) ? HASH_DEL_KEY : HASH_DEL_INDEX);
	}

	/* If we did a shift... re-index like it did before */
	if (!off_the_end) {
		unsigned int k = 0;
		int should_rehash = 0;
		Bucket *p = Z_ARRVAL_P(stack)->pListHead;
		while (p != NULL) {
			if (p->nKeyLength == 0) {
				if (p->h != k) {
					p->h = k++;
					should_rehash = 1;
				} else {
					k++;
				}
			}
			p = p->pListNext;
		}
		Z_ARRVAL_P(stack)->nNextFreeElement = k;
		if (should_rehash) {
			zend_hash_rehash(Z_ARRVAL_P(stack));
		}
	} else if (!key_len && index >= Z_ARRVAL_P(stack)->nNextFreeElement - 1) {
		Z_ARRVAL_P(stack)->nNextFreeElement = Z_ARRVAL_P(stack)->nNextFreeElement - 1;
	}

	zend_hash_internal_pointer_reset(Z_ARRVAL_P(stack));
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
	zval ***args,			/* Function arguments array */
		   *stack;			/* Input stack */
	HashTable *new_hash;	/* New hashtable for the stack */
	HashTable  old_hash;
	int argc;				/* Number of function arguments */
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a+", &stack, &args, &argc) == FAILURE) {
		return;
	}

	/* Use splice to insert the elements at the beginning. Destroy old
	 * hashtable and replace it with new one */
	new_hash = php_splice(Z_ARRVAL_P(stack), 0, 0, &args[0], argc, NULL);
	old_hash = *Z_ARRVAL_P(stack);
	if (Z_ARRVAL_P(stack) == &EG(symbol_table)) {
		zend_reset_all_cv(&EG(symbol_table) TSRMLS_CC);
	}
	*Z_ARRVAL_P(stack) = *new_hash;
	FREE_HASHTABLE(new_hash);
	zend_hash_destroy(&old_hash);

	/* Clean up and return the number of elements in the stack */
	efree(args);
	RETVAL_LONG(zend_hash_num_elements(Z_ARRVAL_P(stack)));
}
/* }}} */

/* {{{ proto array array_splice(array input, int offset [, int length [, array replacement]])
   Removes the elements designated by offset and length and replace them with supplied array */
PHP_FUNCTION(array_splice)
{
	zval *array,				/* Input array */
		 *repl_array = NULL,	/* Replacement array */
		 ***repl = NULL;		/* Replacement elements */
	HashTable *new_hash = NULL,	/* Output array's hash */
	         **rem_hash = NULL;	/* Removed elements' hash */
	HashTable  old_hash;
	Bucket *p;					/* Bucket used for traversing hash */
	long	i,
			offset,
			length = 0,
			repl_num = 0;		/* Number of replacement elements */
	int		num_in;				/* Number of elements in the input array */

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "al|lz/", &array, &offset, &length, &repl_array) == FAILURE) {
		return;
	}

	num_in = zend_hash_num_elements(Z_ARRVAL_P(array));

	if (ZEND_NUM_ARGS() < 3) {
		length = num_in;
	}

	if (ZEND_NUM_ARGS() == 4) {
		/* Make sure the last argument, if passed, is an array */
		convert_to_array(repl_array);

		/* Create the array of replacement elements */
		repl_num = zend_hash_num_elements(Z_ARRVAL_P(repl_array));
		repl = (zval ***)safe_emalloc(repl_num, sizeof(zval **), 0);
		for (p = Z_ARRVAL_P(repl_array)->pListHead, i = 0; p; p = p->pListNext, i++) {
			repl[i] = ((zval **)p->pData);
		}
	}

	/* Don't create the array of removed elements if it's not going
	 * to be used; e.g. only removing and/or replacing elements */
	if (return_value_used) {
		int size = length;

		/* Clamp the offset.. */
		if (offset > num_in) {
			offset = num_in;
		} else if (offset < 0 && (offset = (num_in + offset)) < 0) {
			offset = 0;
		}

		/* ..and the length */
		if (length < 0) {
			size = num_in - offset + length;
		} else if (((unsigned long) offset + (unsigned long) length) > (unsigned) num_in) {
			size = num_in - offset;
		}

		/* Initialize return value */
		array_init_size(return_value, size > 0 ? size : 0);
		rem_hash = &Z_ARRVAL_P(return_value);
	}

	/* Perform splice */
	new_hash = php_splice(Z_ARRVAL_P(array), offset, length, repl, repl_num, rem_hash);

	/* Replace input array's hashtable with the new one */
	old_hash = *Z_ARRVAL_P(array);
	if (Z_ARRVAL_P(array) == &EG(symbol_table)) {
		zend_reset_all_cv(&EG(symbol_table) TSRMLS_CC);
	}
	*Z_ARRVAL_P(array) = *new_hash;
	FREE_HASHTABLE(new_hash);
	zend_hash_destroy(&old_hash);

	/* Clean up */
	if (ZEND_NUM_ARGS() == 4) {
		efree(repl);
	}
}
/* }}} */

/* {{{ proto array array_slice(array input, int offset [, int length [, bool preserve_keys]])
   Returns elements specified by offset and length */
PHP_FUNCTION(array_slice)
{
	zval	 *input,		/* Input array */
			**z_length = NULL, /* How many elements to get */ 
			**entry;		/* An array entry */
	long	 offset,		/* Offset to get elements from */
			 length = 0;	
	zend_bool preserve_keys = 0; /* Whether to preserve keys while copying to the new array or not */
	int		 num_in,		/* Number of elements in the input array */
			 pos;			/* Current position in the array */
	char *string_key;
	uint string_key_len;
	ulong num_key;
	HashPosition hpos;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "al|Zb", &input, &offset, &z_length, &preserve_keys) == FAILURE) {
		return;
	}

	/* Get number of entries in the input hash */
	num_in = zend_hash_num_elements(Z_ARRVAL_P(input));

	/* We want all entries from offset to the end if length is not passed or is null */
	if (ZEND_NUM_ARGS() < 3 || Z_TYPE_PP(z_length) == IS_NULL) {
		length = num_in;
	} else {
		convert_to_long_ex(z_length);
		length = Z_LVAL_PP(z_length);
	}

	/* Clamp the offset.. */
	if (offset > num_in) {
		array_init(return_value);
		return;
	} else if (offset < 0 && (offset = (num_in + offset)) < 0) {
		offset = 0;
	}

	/* ..and the length */
	if (length < 0) {
		length = num_in - offset + length;
	} else if (((unsigned long) offset + (unsigned long) length) > (unsigned) num_in) {
		length = num_in - offset;
	}

	/* Initialize returned array */
	array_init_size(return_value, length > 0 ? length : 0);

	if (length <= 0) {
		return;
	}

	/* Start at the beginning and go until we hit offset */
	pos = 0;
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(input), &hpos);
	while (pos < offset && zend_hash_get_current_data_ex(Z_ARRVAL_P(input), (void **)&entry, &hpos) == SUCCESS) {
		pos++;
		zend_hash_move_forward_ex(Z_ARRVAL_P(input), &hpos);
	}

	/* Copy elements from input array to the one that's returned */
	while (pos < offset + length && zend_hash_get_current_data_ex(Z_ARRVAL_P(input), (void **)&entry, &hpos) == SUCCESS) {

		zval_add_ref(entry);

		switch (zend_hash_get_current_key_ex(Z_ARRVAL_P(input), &string_key, &string_key_len, &num_key, 0, &hpos)) {
			case HASH_KEY_IS_STRING:
				zend_hash_update(Z_ARRVAL_P(return_value), string_key, string_key_len, entry, sizeof(zval *), NULL);
				break;

			case HASH_KEY_IS_LONG:
				if (preserve_keys) {
					zend_hash_index_update(Z_ARRVAL_P(return_value), num_key, entry, sizeof(zval *), NULL);
				} else {
					zend_hash_next_index_insert(Z_ARRVAL_P(return_value), entry, sizeof(zval *), NULL);
				}
				break;
		}
		pos++;
		zend_hash_move_forward_ex(Z_ARRVAL_P(input), &hpos);
	}
}
/* }}} */

PHPAPI int php_array_merge(HashTable *dest, HashTable *src, int recursive TSRMLS_DC) /* {{{ */
{
	zval **src_entry, **dest_entry;
	char *string_key;
	uint string_key_len;
	ulong num_key;
	HashPosition pos;

	zend_hash_internal_pointer_reset_ex(src, &pos);
	while (zend_hash_get_current_data_ex(src, (void **)&src_entry, &pos) == SUCCESS) {
		switch (zend_hash_get_current_key_ex(src, &string_key, &string_key_len, &num_key, 0, &pos)) {
			case HASH_KEY_IS_STRING:
				if (recursive && zend_hash_find(dest, string_key, string_key_len, (void **)&dest_entry) == SUCCESS) {
					HashTable *thash = Z_TYPE_PP(dest_entry) == IS_ARRAY ? Z_ARRVAL_PP(dest_entry) : NULL;
					zval *src_zval;
					zval *tmp = NULL;

					if ((thash && thash->nApplyCount > 1) || (*src_entry == *dest_entry && Z_ISREF_PP(dest_entry) && (Z_REFCOUNT_PP(dest_entry) % 2))) {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "recursion detected");
						return 0;
					}
					SEPARATE_ZVAL(dest_entry);

					if (Z_TYPE_PP(dest_entry) == IS_NULL) {
						convert_to_array_ex(dest_entry);
						add_next_index_null(*dest_entry);
					} else {
						convert_to_array_ex(dest_entry);
					}
					if (Z_TYPE_PP(src_entry) == IS_OBJECT) {
						ALLOC_ZVAL(src_zval);
						INIT_PZVAL_COPY(src_zval, *src_entry);
						zval_copy_ctor(src_zval);
						convert_to_array(src_zval);
						tmp = src_zval;
					} else {
						src_zval = *src_entry;
					}
					if (Z_TYPE_P(src_zval) == IS_ARRAY) {
						if (thash) {
							thash->nApplyCount++;
						}
						if (!php_array_merge(Z_ARRVAL_PP(dest_entry), Z_ARRVAL_P(src_zval), recursive TSRMLS_CC)) {
							if (thash) {
								thash->nApplyCount--;
							}
							return 0;
						}
						if (thash) {
							thash->nApplyCount--;
						}
					} else {
						Z_ADDREF_PP(src_entry);
						zend_hash_next_index_insert(Z_ARRVAL_PP(dest_entry), &src_zval, sizeof(zval *), NULL);
					}
					if (tmp) {
						zval_ptr_dtor(&tmp);
					}
				} else {
					Z_ADDREF_PP(src_entry);
					zend_hash_update(dest, string_key, string_key_len, src_entry, sizeof(zval *), NULL);
				}
				break;

			case HASH_KEY_IS_LONG:
				Z_ADDREF_PP(src_entry);
				zend_hash_next_index_insert(dest, src_entry, sizeof(zval *), NULL);
				break;
		}
		zend_hash_move_forward_ex(src, &pos);
	}
	return 1;
}
/* }}} */

PHPAPI int php_array_replace_recursive(HashTable *dest, HashTable *src TSRMLS_DC) /* {{{ */
{
	zval **src_entry, **dest_entry;
	char *string_key;
	uint string_key_len;
	ulong num_key;
	HashPosition pos;

	for (zend_hash_internal_pointer_reset_ex(src, &pos);
	     zend_hash_get_current_data_ex(src, (void **)&src_entry, &pos) == SUCCESS;
	     zend_hash_move_forward_ex(src, &pos)) {
		switch (zend_hash_get_current_key_ex(src, &string_key, &string_key_len, &num_key, 0, &pos)) {
			case HASH_KEY_IS_STRING:
				if (Z_TYPE_PP(src_entry) != IS_ARRAY ||
					zend_hash_find(dest, string_key, string_key_len, (void **)&dest_entry) == FAILURE ||
					Z_TYPE_PP(dest_entry) != IS_ARRAY) {

					Z_ADDREF_PP(src_entry);
					zend_hash_update(dest, string_key, string_key_len, src_entry, sizeof(zval *), NULL);

					continue;
				}
				break;

			case HASH_KEY_IS_LONG:
				if (Z_TYPE_PP(src_entry) != IS_ARRAY ||
					zend_hash_index_find(dest, num_key, (void **)&dest_entry) == FAILURE ||
					Z_TYPE_PP(dest_entry) != IS_ARRAY) {

					Z_ADDREF_PP(src_entry);
					zend_hash_index_update(dest, num_key, src_entry, sizeof(zval *), NULL);

					continue;
				}
				break;
		}

		if (Z_ARRVAL_PP(dest_entry)->nApplyCount > 1 || Z_ARRVAL_PP(src_entry)->nApplyCount > 1 || (*src_entry == *dest_entry && Z_ISREF_PP(dest_entry) && (Z_REFCOUNT_PP(dest_entry) % 2))) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "recursion detected");
			return 0;
		}
		SEPARATE_ZVAL(dest_entry);
		Z_ARRVAL_PP(dest_entry)->nApplyCount++;
		Z_ARRVAL_PP(src_entry)->nApplyCount++;
		

		if (!php_array_replace_recursive(Z_ARRVAL_PP(dest_entry), Z_ARRVAL_PP(src_entry) TSRMLS_CC)) {
			Z_ARRVAL_PP(dest_entry)->nApplyCount--;
			Z_ARRVAL_PP(src_entry)->nApplyCount--;
			return 0;
		}
		Z_ARRVAL_PP(dest_entry)->nApplyCount--;
		Z_ARRVAL_PP(src_entry)->nApplyCount--;
	}

	return 1;
}
/* }}} */

static void php_array_merge_or_replace_wrapper(INTERNAL_FUNCTION_PARAMETERS, int recursive, int replace) /* {{{ */
{
	zval ***args = NULL;
	int argc, i, init_size = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "+", &args, &argc) == FAILURE) {
		return;
	}

	for (i = 0; i < argc; i++) {
		if (Z_TYPE_PP(args[i]) != IS_ARRAY) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Argument #%d is not an array", i + 1);
			efree(args);
			RETURN_NULL();
		} else {
			int num = zend_hash_num_elements(Z_ARRVAL_PP(args[i]));

			if (num > init_size) {
				init_size = num;
			}
		}
	}

	array_init_size(return_value, init_size);

	for (i = 0; i < argc; i++) {
		if (!replace) {
			php_array_merge(Z_ARRVAL_P(return_value), Z_ARRVAL_PP(args[i]), recursive TSRMLS_CC);
		} else if (recursive && i > 0) { /* First array will be copied directly instead */
			php_array_replace_recursive(Z_ARRVAL_P(return_value), Z_ARRVAL_PP(args[i]) TSRMLS_CC);
		} else {
			zend_hash_merge(Z_ARRVAL_P(return_value), Z_ARRVAL_PP(args[i]), (copy_ctor_func_t) zval_add_ref, NULL, sizeof(zval *), 1);
		}
	}

	efree(args);
}
/* }}} */

/* {{{ proto array array_merge(array arr1, array arr2 [, array ...])
   Merges elements from passed arrays into one array */
PHP_FUNCTION(array_merge)
{
	php_array_merge_or_replace_wrapper(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, 0);
}
/* }}} */

/* {{{ proto array array_merge_recursive(array arr1, array arr2 [, array ...])
   Recursively merges elements from passed arrays into one array */
PHP_FUNCTION(array_merge_recursive)
{
	php_array_merge_or_replace_wrapper(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1, 0);
}
/* }}} */

/* {{{ proto array array_replace(array arr1, array arr2 [, array ...])
   Replaces elements from passed arrays into one array */
PHP_FUNCTION(array_replace)
{
	php_array_merge_or_replace_wrapper(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, 1);
}
/* }}} */

/* {{{ proto array array_replace_recursive(array arr1, array arr2 [, array ...])
   Recursively replaces elements from passed arrays into one array */
PHP_FUNCTION(array_replace_recursive)
{
	php_array_merge_or_replace_wrapper(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1, 1);
}
/* }}} */

/* {{{ proto array array_keys(array input [, mixed search_value[, bool strict]])
   Return just the keys from the input array, optionally only for the specified search_value */
PHP_FUNCTION(array_keys)
{
	zval *input,				/* Input array */
	     *search_value = NULL,	/* Value to search for */
	     **entry,				/* An entry in the input array */
	       res,					/* Result of comparison */
	      *new_val;				/* New value */
	int    add_key;				/* Flag to indicate whether a key should be added */
	zend_bool strict = 0;		/* do strict comparison */
	HashPosition pos;
	int (*is_equal_func)(zval *, zval *, zval * TSRMLS_DC) = is_equal_function;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|zb", &input, &search_value, &strict) == FAILURE) {
		return;
	}

	if (strict) {
		is_equal_func = is_identical_function;
	}

	/* Initialize return array */
	if (search_value != NULL) {
		array_init(return_value);
	} else {
		array_init_size(return_value, zend_hash_num_elements(Z_ARRVAL_P(input)));
	}
	add_key = 1;

	/* Go through input array and add keys to the return array */
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(input), &pos);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(input), (void **)&entry, &pos) == SUCCESS) {
		if (search_value != NULL) {
			is_equal_func(&res, search_value, *entry TSRMLS_CC);
			add_key = zval_is_true(&res);
		}

		if (add_key) {
			MAKE_STD_ZVAL(new_val);
			zend_hash_get_current_key_zval_ex(Z_ARRVAL_P(input), new_val, &pos);
			zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &new_val, sizeof(zval *), NULL);
		}

		zend_hash_move_forward_ex(Z_ARRVAL_P(input), &pos);
	}
}
/* }}} */

/* {{{ proto array array_values(array input)
   Return just the values from the input array */
PHP_FUNCTION(array_values)
{
	zval	 *input,		/* Input array */
			**entry;		/* An entry in the input array */
	HashPosition pos;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &input) == FAILURE) {
		return;
	}

	/* Initialize return array */
	array_init_size(return_value, zend_hash_num_elements(Z_ARRVAL_P(input)));

	/* Go through input array and add values to the return array */
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(input), &pos);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(input), (void **)&entry, &pos) == SUCCESS) {
		zval_add_ref(entry);
		zend_hash_next_index_insert(Z_ARRVAL_P(return_value), entry, sizeof(zval *), NULL);
		zend_hash_move_forward_ex(Z_ARRVAL_P(input), &pos);
	}
}
/* }}} */

/* {{{ proto array array_count_values(array input)
   Return the value as key and the frequency of that value in input as value */
PHP_FUNCTION(array_count_values)
{
	zval	*input,			/* Input array */
			**entry,		/* An entry in the input array */
			**tmp;
	HashTable *myht;
	HashPosition pos;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &input) == FAILURE) {
		return;
	}

	/* Initialize return array */
	array_init(return_value);

	/* Go through input array and add values to the return array */
	myht = Z_ARRVAL_P(input);
	zend_hash_internal_pointer_reset_ex(myht, &pos);
	while (zend_hash_get_current_data_ex(myht, (void **)&entry, &pos) == SUCCESS) {
		if (Z_TYPE_PP(entry) == IS_LONG) {
			if (zend_hash_index_find(Z_ARRVAL_P(return_value), Z_LVAL_PP(entry), (void **)&tmp) == FAILURE) {
				zval *data;
				MAKE_STD_ZVAL(data);
				ZVAL_LONG(data, 1);
				zend_hash_index_update(Z_ARRVAL_P(return_value), Z_LVAL_PP(entry), &data, sizeof(data), NULL);
			} else {
				Z_LVAL_PP(tmp)++;
			}
		} else if (Z_TYPE_PP(entry) == IS_STRING) {
			if (zend_symtable_find(Z_ARRVAL_P(return_value), Z_STRVAL_PP(entry), Z_STRLEN_PP(entry) + 1, (void**)&tmp) == FAILURE) {
				zval *data;
				MAKE_STD_ZVAL(data);
				ZVAL_LONG(data, 1);
				zend_symtable_update(Z_ARRVAL_P(return_value), Z_STRVAL_PP(entry), Z_STRLEN_PP(entry) + 1, &data, sizeof(data), NULL);
			} else {
				Z_LVAL_PP(tmp)++;
			}
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can only count STRING and INTEGER values!");
		}

		zend_hash_move_forward_ex(myht, &pos);
	}
}
/* }}} */

/* {{{ array_column_param_helper
 * Specialized conversion rules for array_column() function
 */
static inline
zend_bool array_column_param_helper(zval **param,
                                    const char *name TSRMLS_DC) {
	switch (Z_TYPE_PP(param)) {
		case IS_DOUBLE:
			convert_to_long_ex(param);
			/* fallthrough */
		case IS_LONG:
			return 1;

		case IS_OBJECT:
			convert_to_string_ex(param);
			/* fallthrough */
		case IS_STRING:
			return 1;

		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "The %s key should be either a string or an integer", name);
			return 0;
	}
}

/* {{{ proto array array_column(array input, mixed column_key[, mixed index_key])
   Return the values from a single column in the input array, identified by the
   value_key and optionally indexed by the index_key */
PHP_FUNCTION(array_column)
{
	zval **zcolumn = NULL, **zkey = NULL, **data;
	HashTable *arr_hash;
	HashPosition pointer;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "hZ!|Z!", &arr_hash, &zcolumn, &zkey) == FAILURE) {
		return;
	}

	if ((zcolumn && !array_column_param_helper(zcolumn, "column" TSRMLS_CC)) ||
	    (zkey && !array_column_param_helper(zkey, "index" TSRMLS_CC))) {
		RETURN_FALSE;
	}

	array_init(return_value);
	for (zend_hash_internal_pointer_reset_ex(arr_hash, &pointer);
			zend_hash_get_current_data_ex(arr_hash, (void**)&data, &pointer) == SUCCESS;
			zend_hash_move_forward_ex(arr_hash, &pointer)) {
		zval **zcolval, **zkeyval = NULL;
		HashTable *ht;

		if (Z_TYPE_PP(data) != IS_ARRAY) {
			/* Skip elemens which are not sub-arrays */
			continue;
		}
		ht = Z_ARRVAL_PP(data);

		if (!zcolumn) {
			/* NULL column ID means use entire subarray as data */
			zcolval = data;

			/* Otherwise, skip if the value doesn't exist in our subarray */
		} else if ((Z_TYPE_PP(zcolumn) == IS_STRING) &&
		    (zend_hash_find(ht, Z_STRVAL_PP(zcolumn), Z_STRLEN_PP(zcolumn) + 1, (void**)&zcolval) == FAILURE)) {
			continue;
		} else if ((Z_TYPE_PP(zcolumn) == IS_LONG) &&
		    (zend_hash_index_find(ht, Z_LVAL_PP(zcolumn), (void**)&zcolval) == FAILURE)) {
			continue;
		}

		/* Failure will leave zkeyval alone which will land us on the final else block below
		 * which is to append the value as next_index
		 */
		if (zkey && (Z_TYPE_PP(zkey) == IS_STRING)) {
			zend_hash_find(ht, Z_STRVAL_PP(zkey), Z_STRLEN_PP(zkey) + 1, (void**)&zkeyval);
		} else if (zkey && (Z_TYPE_PP(zkey) == IS_LONG)) {
			zend_hash_index_find(ht, Z_LVAL_PP(zkey), (void**)&zkeyval);
		}

		Z_ADDREF_PP(zcolval);
		if (zkeyval && Z_TYPE_PP(zkeyval) == IS_STRING) {
			add_assoc_zval(return_value, Z_STRVAL_PP(zkeyval), *zcolval);
		} else if (zkeyval && Z_TYPE_PP(zkeyval) == IS_LONG) {
			add_index_zval(return_value, Z_LVAL_PP(zkeyval), *zcolval);
		} else if (zkeyval && Z_TYPE_PP(zkeyval) == IS_OBJECT) {
			SEPARATE_ZVAL(zkeyval);
			convert_to_string(*zkeyval);
			add_assoc_zval(return_value, Z_STRVAL_PP(zkeyval), *zcolval);
		} else {
			add_next_index_zval(return_value, *zcolval);
		}
	}
}
/* }}} */

/* {{{ proto array array_reverse(array input [, bool preserve keys])
   Return input as a new array with the order of the entries reversed */
PHP_FUNCTION(array_reverse)
{
	zval	 *input,				/* Input array */
			**entry;				/* An entry in the input array */
	char	 *string_key;
	uint	  string_key_len;
	ulong	  num_key;
	zend_bool preserve_keys = 0;	/* whether to preserve keys */
	HashPosition pos;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|b", &input, &preserve_keys) == FAILURE) {
		return;
	}

	/* Initialize return array */
	array_init_size(return_value, zend_hash_num_elements(Z_ARRVAL_P(input)));

	zend_hash_internal_pointer_end_ex(Z_ARRVAL_P(input), &pos);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(input), (void **)&entry, &pos) == SUCCESS) {
		zval_add_ref(entry);

		switch (zend_hash_get_current_key_ex(Z_ARRVAL_P(input), &string_key, &string_key_len, &num_key, 0, &pos)) {
			case HASH_KEY_IS_STRING:
				zend_hash_update(Z_ARRVAL_P(return_value), string_key, string_key_len, entry, sizeof(zval *), NULL);
				break;

			case HASH_KEY_IS_LONG:
				if (preserve_keys) {
					zend_hash_index_update(Z_ARRVAL_P(return_value), num_key, entry, sizeof(zval *), NULL);
				} else {
					zend_hash_next_index_insert(Z_ARRVAL_P(return_value), entry, sizeof(zval *), NULL);
				}
				break;
		}

		zend_hash_move_backwards_ex(Z_ARRVAL_P(input), &pos);
	}
}
/* }}} */

/* {{{ proto array array_pad(array input, int pad_size, mixed pad_value)
   Returns a copy of input array padded with pad_value to size pad_size */
PHP_FUNCTION(array_pad)
{
	zval  *input;		/* Input array */
	zval  *pad_value;	/* Padding value obviously */
	zval ***pads;		/* Array to pass to splice */
	HashTable *new_hash;/* Return value from splice */
	HashTable  old_hash;
	long pad_size;		/* Size to pad to */
	long pad_size_abs;	/* Absolute value of pad_size */
	int	input_size;		/* Size of the input array */
	int	num_pads;		/* How many pads do we need */
	int	do_pad;			/* Whether we should do padding at all */
	int	i;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "alz", &input, &pad_size, &pad_value) == FAILURE) {
		return;
	}

	/* Do some initial calculations */
	input_size = zend_hash_num_elements(Z_ARRVAL_P(input));
	pad_size_abs = abs(pad_size);
	if (pad_size_abs < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "You may only pad up to 1048576 elements at a time");
		zval_dtor(return_value);
		RETURN_FALSE;
	}
	do_pad = (input_size >= pad_size_abs) ? 0 : 1;

	/* Copy the original array */
	RETVAL_ZVAL(input, 1, 0);

	/* If no need to pad, no need to continue */
	if (!do_pad) {
		return;
	}

	/* Populate the pads array */
	num_pads = pad_size_abs - input_size;
	if (num_pads > 1048576) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "You may only pad up to 1048576 elements at a time");
		zval_dtor(return_value);
		RETURN_FALSE;
	}
	pads = (zval ***)safe_emalloc(num_pads, sizeof(zval **), 0);
	for (i = 0; i < num_pads; i++) {
		pads[i] = &pad_value;
	}

	/* Pad on the right or on the left */
	if (pad_size > 0) {
		new_hash = php_splice(Z_ARRVAL_P(return_value), input_size, 0, pads, num_pads, NULL);
	} else {
		new_hash = php_splice(Z_ARRVAL_P(return_value), 0, 0, pads, num_pads, NULL);
	}

	/* Copy the result hash into return value */
	old_hash = *Z_ARRVAL_P(return_value);
	if (Z_ARRVAL_P(return_value) == &EG(symbol_table)) {
		zend_reset_all_cv(&EG(symbol_table) TSRMLS_CC);
	}
	*Z_ARRVAL_P(return_value) = *new_hash;
	FREE_HASHTABLE(new_hash);
	zend_hash_destroy(&old_hash);

	/* Clean up */
	efree(pads);
}
/* }}} */

/* {{{ proto array array_flip(array input)
   Return array with key <-> value flipped */
PHP_FUNCTION(array_flip)
{
	zval *array, **entry, *data;
	HashPosition pos;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &array) == FAILURE) {
		return;
	}

	array_init_size(return_value, zend_hash_num_elements(Z_ARRVAL_P(array)));

	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(array), &pos);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(array), (void **)&entry, &pos) == SUCCESS) {
		MAKE_STD_ZVAL(data);
		zend_hash_get_current_key_zval_ex(Z_ARRVAL_P(array), data, &pos);

		if (Z_TYPE_PP(entry) == IS_LONG) {
			zend_hash_index_update(Z_ARRVAL_P(return_value), Z_LVAL_PP(entry), &data, sizeof(data), NULL);
		} else if (Z_TYPE_PP(entry) == IS_STRING) {
			zend_symtable_update(Z_ARRVAL_P(return_value), Z_STRVAL_PP(entry), Z_STRLEN_PP(entry) + 1, &data, sizeof(data), NULL);
		} else {
			zval_ptr_dtor(&data); /* will free also zval structure */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can only flip STRING and INTEGER values!");
		}

		zend_hash_move_forward_ex(Z_ARRVAL_P(array), &pos);
	}
}
/* }}} */

/* {{{ proto array array_change_key_case(array input [, int case=CASE_LOWER])
   Retuns an array with all string keys lowercased [or uppercased] */
PHP_FUNCTION(array_change_key_case)
{
	zval *array, **entry;
	char *string_key;
	char *new_key;
	uint str_key_len;
	ulong num_key;
	long change_to_upper=0;
	HashPosition pos;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|l", &array, &change_to_upper) == FAILURE) {
		return;
	}

	array_init_size(return_value, zend_hash_num_elements(Z_ARRVAL_P(array)));

	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(array), &pos);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(array), (void **)&entry, &pos) == SUCCESS) {
		zval_add_ref(entry);

		switch (zend_hash_get_current_key_ex(Z_ARRVAL_P(array), &string_key, &str_key_len, &num_key, 0, &pos)) {
			case HASH_KEY_IS_LONG:
				zend_hash_index_update(Z_ARRVAL_P(return_value), num_key, entry, sizeof(entry), NULL);
				break;
			case HASH_KEY_IS_STRING:
				new_key = estrndup(string_key, str_key_len - 1);
				if (change_to_upper) {
					php_strtoupper(new_key, str_key_len - 1);
				} else {
					php_strtolower(new_key, str_key_len - 1);
				}
				zend_hash_update(Z_ARRVAL_P(return_value), new_key, str_key_len, entry, sizeof(entry), NULL);
				efree(new_key);
				break;
		}

		zend_hash_move_forward_ex(Z_ARRVAL_P(array), &pos);
	}
}
/* }}} */

/* {{{ proto array array_unique(array input [, int sort_flags])
   Removes duplicate values from array */
PHP_FUNCTION(array_unique)
{
	zval *array, *tmp;
	Bucket *p;
	struct bucketindex {
		Bucket *b;
		unsigned int i;
	};
	struct bucketindex *arTmp, *cmpdata, *lastkept;
	unsigned int i;
	long sort_type = PHP_SORT_STRING;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|l", &array, &sort_type) == FAILURE) {
		return;
	}

	php_set_compare_func(sort_type TSRMLS_CC);

	array_init_size(return_value, zend_hash_num_elements(Z_ARRVAL_P(array)));
	zend_hash_copy(Z_ARRVAL_P(return_value), Z_ARRVAL_P(array), (copy_ctor_func_t) zval_add_ref, (void *)&tmp, sizeof(zval*));

	if (Z_ARRVAL_P(array)->nNumOfElements <= 1) {	/* nothing to do */
		return;
	}

	/* create and sort array with pointers to the target_hash buckets */
	arTmp = (struct bucketindex *) pemalloc((Z_ARRVAL_P(array)->nNumOfElements + 1) * sizeof(struct bucketindex), Z_ARRVAL_P(array)->persistent);
	if (!arTmp) {
		zval_dtor(return_value);
		RETURN_FALSE;
	}
	for (i = 0, p = Z_ARRVAL_P(array)->pListHead; p; i++, p = p->pListNext) {
		arTmp[i].b = p;
		arTmp[i].i = i;
	}
	arTmp[i].b = NULL;
	zend_qsort((void *) arTmp, i, sizeof(struct bucketindex), php_array_data_compare TSRMLS_CC);

	/* go through the sorted array and delete duplicates from the copy */
	lastkept = arTmp;
	for (cmpdata = arTmp + 1; cmpdata->b; cmpdata++) {
		if (php_array_data_compare(lastkept, cmpdata TSRMLS_CC)) {
			lastkept = cmpdata;
		} else {
			if (lastkept->i > cmpdata->i) {
				p = lastkept->b;
				lastkept = cmpdata;
			} else {
				p = cmpdata->b;
			}
			if (p->nKeyLength == 0) {
				zend_hash_index_del(Z_ARRVAL_P(return_value), p->h);
			} else {
				if (Z_ARRVAL_P(return_value) == &EG(symbol_table)) {
					zend_delete_global_variable(p->arKey, p->nKeyLength - 1 TSRMLS_CC);
				} else {
					zend_hash_quick_del(Z_ARRVAL_P(return_value), p->arKey, p->nKeyLength, p->h);
				}
			}
		}
	}
	pefree(arTmp, Z_ARRVAL_P(array)->persistent);
}
/* }}} */

static int zval_compare(zval **a, zval **b TSRMLS_DC) /* {{{ */
{
	zval result;
	zval *first;
	zval *second;

	first = *((zval **) a);
	second = *((zval **) b);

	if (string_compare_function(&result, first, second TSRMLS_CC) == FAILURE) {
		return 0;
	}

	if (Z_TYPE(result) == IS_DOUBLE) {
		if (Z_DVAL(result) < 0) {
			return -1;
		} else if (Z_DVAL(result) > 0) {
			return 1;
		} else {
			return 0;
		}
	}

	convert_to_long(&result);

	if (Z_LVAL(result) < 0) {
		return -1;
	} else if (Z_LVAL(result) > 0) {
		return 1;
	}

	return 0;
}
/* }}} */

static int zval_user_compare(zval **a, zval **b TSRMLS_DC) /* {{{ */
{
	zval **args[2];
	zval *retval_ptr = NULL;

	args[0] = (zval **) a;
	args[1] = (zval **) b;

	BG(user_compare_fci).param_count = 2;
	BG(user_compare_fci).params = args;
	BG(user_compare_fci).retval_ptr_ptr = &retval_ptr;
	BG(user_compare_fci).no_separation = 0;

	if (zend_call_function(&BG(user_compare_fci), &BG(user_compare_fci_cache) TSRMLS_CC) == SUCCESS && retval_ptr) {
		long retval;

		convert_to_long_ex(&retval_ptr);
		retval = Z_LVAL_P(retval_ptr);
		zval_ptr_dtor(&retval_ptr);
		return retval < 0 ? -1 : retval > 0 ? 1 : 0;;
	} else {
		return 0;
	}
}
/* }}} */

static void php_array_intersect_key(INTERNAL_FUNCTION_PARAMETERS, int data_compare_type) /* {{{ */
{
	Bucket *p;
	int argc, i;
	zval ***args;
	int (*intersect_data_compare_func)(zval **, zval ** TSRMLS_DC) = NULL;
	zend_bool ok;
	zval **data;
	int req_args;
	char *param_spec;

	/* Get the argument count */
	argc = ZEND_NUM_ARGS();
	if (data_compare_type == INTERSECT_COMP_DATA_USER) {
		/* INTERSECT_COMP_DATA_USER - array_uintersect_assoc() */
		req_args = 3;
		param_spec = "+f";
		intersect_data_compare_func = zval_user_compare;
	} else {
		/* 	INTERSECT_COMP_DATA_NONE - array_intersect_key()
			INTERSECT_COMP_DATA_INTERNAL - array_intersect_assoc() */
		req_args = 2;
		param_spec = "+";
				
		if (data_compare_type == INTERSECT_COMP_DATA_INTERNAL) {
			intersect_data_compare_func = zval_compare;
		}
	}
	
	if (argc < req_args) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "at least %d parameters are required, %d given", req_args, argc);
		return;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, param_spec, &args, &argc, &BG(user_compare_fci), &BG(user_compare_fci_cache)) == FAILURE) {
		return;
	}

	for (i = 0; i < argc; i++) {
		if (Z_TYPE_PP(args[i]) != IS_ARRAY) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Argument #%d is not an array", i + 1);
			RETVAL_NULL();
			goto out;
		}
	}

	array_init(return_value);

	for (p = Z_ARRVAL_PP(args[0])->pListHead; p != NULL; p = p->pListNext) {
		if (p->nKeyLength == 0) {
			ok = 1;
			for (i = 1; i < argc; i++) {
				if (zend_hash_index_find(Z_ARRVAL_PP(args[i]), p->h, (void**)&data) == FAILURE ||
					(intersect_data_compare_func &&
					intersect_data_compare_func((zval**)p->pData, data TSRMLS_CC) != 0)
				) {
					ok = 0;
					break;
				}
			}
			if (ok) {
				Z_ADDREF_PP((zval**)p->pData);
				zend_hash_index_update(Z_ARRVAL_P(return_value), p->h, p->pData, sizeof(zval*), NULL);
			}
		} else {
			ok = 1;
			for (i = 1; i < argc; i++) {
				if (zend_hash_quick_find(Z_ARRVAL_PP(args[i]), p->arKey, p->nKeyLength, p->h, (void**)&data) == FAILURE ||
					(intersect_data_compare_func &&
					intersect_data_compare_func((zval**)p->pData, data TSRMLS_CC) != 0)
				) {
					ok = 0;
					break;
				}
			}
			if (ok) {
				Z_ADDREF_PP((zval**)p->pData);
				zend_hash_quick_update(Z_ARRVAL_P(return_value), p->arKey, p->nKeyLength, p->h, p->pData, sizeof(zval*), NULL);
			}
		}
	}
out:
	efree(args);
}
/* }}} */

static void php_array_intersect(INTERNAL_FUNCTION_PARAMETERS, int behavior, int data_compare_type, int key_compare_type) /* {{{ */
{
	zval ***args = NULL;
	HashTable *hash;
	int arr_argc, i, c = 0;
	Bucket ***lists, **list, ***ptrs, *p;
	int req_args;
	char *param_spec;
	zend_fcall_info fci1, fci2;
	zend_fcall_info_cache fci1_cache = empty_fcall_info_cache, fci2_cache = empty_fcall_info_cache;
	zend_fcall_info *fci_key = NULL, *fci_data;
	zend_fcall_info_cache *fci_key_cache = NULL, *fci_data_cache;
	PHP_ARRAY_CMP_FUNC_VARS;

	int (*intersect_key_compare_func)(const void *, const void * TSRMLS_DC);
	int (*intersect_data_compare_func)(const void *, const void * TSRMLS_DC);

	if (behavior == INTERSECT_NORMAL) {
		intersect_key_compare_func = php_array_key_compare;

		if (data_compare_type == INTERSECT_COMP_DATA_INTERNAL) {
			/* array_intersect() */
			req_args = 2;
			param_spec = "+";
			intersect_data_compare_func = php_array_data_compare;
		} else if (data_compare_type == INTERSECT_COMP_DATA_USER) {
			/* array_uintersect() */
			req_args = 3;
			param_spec = "+f";
			intersect_data_compare_func = php_array_user_compare;
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "data_compare_type is %d. This should never happen. Please report as a bug", data_compare_type);
			return;
		}

		if (ZEND_NUM_ARGS() < req_args) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "at least %d parameters are required, %d given", req_args, ZEND_NUM_ARGS());
			return;
		}

		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, param_spec, &args, &arr_argc, &fci1, &fci1_cache) == FAILURE) {
			return;
		}
		fci_data = &fci1;
		fci_data_cache = &fci1_cache;

	} else if (behavior & INTERSECT_ASSOC) { /* triggered also when INTERSECT_KEY */
		/* INTERSECT_KEY is subset of INTERSECT_ASSOC. When having the former
		 * no comparison of the data is done (part of INTERSECT_ASSOC) */
		intersect_key_compare_func = php_array_key_compare;

		if (data_compare_type == INTERSECT_COMP_DATA_INTERNAL && key_compare_type == INTERSECT_COMP_KEY_INTERNAL) {
			/* array_intersect_assoc() or array_intersect_key() */
			req_args = 2;
			param_spec = "+";
			intersect_key_compare_func = php_array_key_compare;
			intersect_data_compare_func = php_array_data_compare;
		} else if (data_compare_type == INTERSECT_COMP_DATA_USER && key_compare_type == INTERSECT_COMP_KEY_INTERNAL) {
			/* array_uintersect_assoc() */
			req_args = 3;
			param_spec = "+f";
			intersect_key_compare_func = php_array_key_compare;
			intersect_data_compare_func = php_array_user_compare;
			fci_data = &fci1;
			fci_data_cache = &fci1_cache;
		} else if (data_compare_type == INTERSECT_COMP_DATA_INTERNAL && key_compare_type == INTERSECT_COMP_KEY_USER) {
			/* array_intersect_uassoc() or array_intersect_ukey() */
			req_args = 3;
			param_spec = "+f";
			intersect_key_compare_func = php_array_user_key_compare;
			intersect_data_compare_func = php_array_data_compare;
			fci_key = &fci1;
			fci_key_cache = &fci1_cache;
		} else if (data_compare_type == INTERSECT_COMP_DATA_USER && key_compare_type == INTERSECT_COMP_KEY_USER) {
			/* array_uintersect_uassoc() */
			req_args = 4;
			param_spec = "+ff";
			intersect_key_compare_func = php_array_user_key_compare;
			intersect_data_compare_func = php_array_user_compare;
			fci_data = &fci1;
			fci_data_cache = &fci1_cache;
			fci_key = &fci2;
			fci_key_cache = &fci2_cache;
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "data_compare_type is %d. key_compare_type is %d. This should never happen. Please report as a bug", data_compare_type, key_compare_type);
			return;
		}

		if (ZEND_NUM_ARGS() < req_args) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "at least %d parameters are required, %d given", req_args, ZEND_NUM_ARGS());
			return;
		}

		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, param_spec, &args, &arr_argc, &fci1, &fci1_cache, &fci2, &fci2_cache) == FAILURE) {
			return;
		}

	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "behavior is %d. This should never happen. Please report as a bug", behavior);
		return;
	}

	PHP_ARRAY_CMP_FUNC_BACKUP();

	/* for each argument, create and sort list with pointers to the hash buckets */
	lists = (Bucket ***)safe_emalloc(arr_argc, sizeof(Bucket **), 0);
	ptrs = (Bucket ***)safe_emalloc(arr_argc, sizeof(Bucket **), 0);
	php_set_compare_func(PHP_SORT_STRING TSRMLS_CC);

	if (behavior == INTERSECT_NORMAL && data_compare_type == INTERSECT_COMP_DATA_USER) {
		BG(user_compare_fci) = *fci_data;
		BG(user_compare_fci_cache) = *fci_data_cache;
	} else if (behavior & INTERSECT_ASSOC && key_compare_type == INTERSECT_COMP_KEY_USER) {
		BG(user_compare_fci) = *fci_key;
		BG(user_compare_fci_cache) = *fci_key_cache;
	}

	for (i = 0; i < arr_argc; i++) {
		if (Z_TYPE_PP(args[i]) != IS_ARRAY) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Argument #%d is not an array", i + 1);
			arr_argc = i; /* only free up to i - 1 */
			goto out;
		}
		hash = Z_ARRVAL_PP(args[i]);
		list = (Bucket **) pemalloc((hash->nNumOfElements + 1) * sizeof(Bucket *), hash->persistent);
		if (!list) {
			PHP_ARRAY_CMP_FUNC_RESTORE();

			efree(ptrs);
			efree(lists);
			efree(args);
			RETURN_FALSE;
		}
		lists[i] = list;
		ptrs[i] = list;
		for (p = hash->pListHead; p; p = p->pListNext) {
			*list++ = p;
		}
		*list = NULL;
		if (behavior == INTERSECT_NORMAL) {
			zend_qsort((void *) lists[i], hash->nNumOfElements, sizeof(Bucket *), intersect_data_compare_func TSRMLS_CC);
		} else if (behavior & INTERSECT_ASSOC) { /* triggered also when INTERSECT_KEY */
			zend_qsort((void *) lists[i], hash->nNumOfElements, sizeof(Bucket *), intersect_key_compare_func TSRMLS_CC);
		}
	}

	/* copy the argument array */
	RETVAL_ZVAL(*args[0], 1, 0);
	if (return_value->value.ht == &EG(symbol_table)) {
		HashTable *ht;
		zval *tmp;

		ALLOC_HASHTABLE(ht);
		zend_hash_init(ht, zend_hash_num_elements(return_value->value.ht), NULL, ZVAL_PTR_DTOR, 0);
		zend_hash_copy(ht, return_value->value.ht, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
		return_value->value.ht = ht;
	}

	/* go through the lists and look for common values */
	while (*ptrs[0]) {
		if ((behavior & INTERSECT_ASSOC) /* triggered also when INTERSECT_KEY */
			&&
			key_compare_type == INTERSECT_COMP_KEY_USER) {

			BG(user_compare_fci) = *fci_key;
			BG(user_compare_fci_cache) = *fci_key_cache;
		}

		for (i = 1; i < arr_argc; i++) {
			if (behavior & INTERSECT_NORMAL) {
				while (*ptrs[i] && (0 < (c = intersect_data_compare_func(ptrs[0], ptrs[i] TSRMLS_CC)))) {
					ptrs[i]++;
				}
			} else if (behavior & INTERSECT_ASSOC) { /* triggered also when INTERSECT_KEY */
				while (*ptrs[i] && (0 < (c = intersect_key_compare_func(ptrs[0], ptrs[i] TSRMLS_CC)))) {
					ptrs[i]++;
				}
				if ((!c && *ptrs[i]) && (behavior == INTERSECT_ASSOC)) { /* only when INTERSECT_ASSOC */
					/* this means that ptrs[i] is not NULL so we can compare
					 * and "c==0" is from last operation
					 * in this branch of code we enter only when INTERSECT_ASSOC
					 * since when we have INTERSECT_KEY compare of data is not wanted. */
					if (data_compare_type == INTERSECT_COMP_DATA_USER) {
						BG(user_compare_fci) = *fci_data;
						BG(user_compare_fci_cache) = *fci_data_cache;
					}
					if (intersect_data_compare_func(ptrs[0], ptrs[i] TSRMLS_CC) != 0) {
						c = 1;
						if (key_compare_type == INTERSECT_COMP_KEY_USER) {
							BG(user_compare_fci) = *fci_key;
							BG(user_compare_fci_cache) = *fci_key_cache;
							/* When KEY_USER, the last parameter is always the callback */
						}
						/* we are going to the break */
					} else {
						/* continue looping */
					}
				}
			}
			if (!*ptrs[i]) {
				/* delete any values corresponding to remains of ptrs[0] */
				/* and exit because they do not present in at least one of */
				/* the other arguments */
				for (;;) {
					p = *ptrs[0]++;
					if (!p) {
						goto out;
					}
					if (p->nKeyLength == 0) {
						zend_hash_index_del(Z_ARRVAL_P(return_value), p->h);
					} else {
						zend_hash_quick_del(Z_ARRVAL_P(return_value), p->arKey, p->nKeyLength, p->h);
					}
				}
			}
			if (c) /* here we get if not all are equal */
				break;
			ptrs[i]++;
		}
		if (c) {
			/* Value of ptrs[0] not in all arguments, delete all entries */
			/* with value < value of ptrs[i] */
			for (;;) {
				p = *ptrs[0];
				if (p->nKeyLength == 0) {
					zend_hash_index_del(Z_ARRVAL_P(return_value), p->h);
				} else {
					zend_hash_quick_del(Z_ARRVAL_P(return_value), p->arKey, p->nKeyLength, p->h);
				}
				if (!*++ptrs[0]) {
					goto out;
				}
				if (behavior == INTERSECT_NORMAL) {
					if (0 <= intersect_data_compare_func(ptrs[0], ptrs[i] TSRMLS_CC)) {
						break;
					}
				} else if (behavior & INTERSECT_ASSOC) { /* triggered also when INTERSECT_KEY */
					/* no need of looping because indexes are unique */
					break;
				}
			}
		} else {
			/* ptrs[0] is present in all the arguments */
			/* Skip all entries with same value as ptrs[0] */
			for (;;) {
				if (!*++ptrs[0]) {
					goto out;
				}
				if (behavior == INTERSECT_NORMAL) {
					if (intersect_data_compare_func(ptrs[0] - 1, ptrs[0] TSRMLS_CC)) {
						break;
					}
				} else if (behavior & INTERSECT_ASSOC) { /* triggered also when INTERSECT_KEY */
					/* no need of looping because indexes are unique */
					break;
				}
			}
		}
	}
out:
	for (i = 0; i < arr_argc; i++) {
		hash = Z_ARRVAL_PP(args[i]);
		pefree(lists[i], hash->persistent);
	}

	PHP_ARRAY_CMP_FUNC_RESTORE();

	efree(ptrs);
	efree(lists);
	efree(args);
}
/* }}} */

/* {{{ proto array array_intersect_key(array arr1, array arr2 [, array ...])
   Returns the entries of arr1 that have keys which are present in all the other arguments. Kind of equivalent to array_diff(array_keys($arr1), array_keys($arr2)[,array_keys(...)]). Equivalent of array_intersect_assoc() but does not do compare of the data. */
PHP_FUNCTION(array_intersect_key)
{
	php_array_intersect_key(INTERNAL_FUNCTION_PARAM_PASSTHRU, INTERSECT_COMP_DATA_NONE);
}
/* }}} */

/* {{{ proto array array_intersect_ukey(array arr1, array arr2 [, array ...], callback key_compare_func)
   Returns the entries of arr1 that have keys which are present in all the other arguments. Kind of equivalent to array_diff(array_keys($arr1), array_keys($arr2)[,array_keys(...)]). The comparison of the keys is performed by a user supplied function. Equivalent of array_intersect_uassoc() but does not do compare of the data. */
PHP_FUNCTION(array_intersect_ukey)
{
	php_array_intersect(INTERNAL_FUNCTION_PARAM_PASSTHRU, INTERSECT_KEY, INTERSECT_COMP_DATA_INTERNAL, INTERSECT_COMP_KEY_USER);
}
/* }}} */

/* {{{ proto array array_intersect(array arr1, array arr2 [, array ...])
   Returns the entries of arr1 that have values which are present in all the other arguments */
PHP_FUNCTION(array_intersect)
{
	php_array_intersect(INTERNAL_FUNCTION_PARAM_PASSTHRU, INTERSECT_NORMAL, INTERSECT_COMP_DATA_INTERNAL, INTERSECT_COMP_KEY_INTERNAL);
}
/* }}} */

/* {{{ proto array array_uintersect(array arr1, array arr2 [, array ...], callback data_compare_func)
   Returns the entries of arr1 that have values which are present in all the other arguments. Data is compared by using an user-supplied callback. */
PHP_FUNCTION(array_uintersect)
{
	php_array_intersect(INTERNAL_FUNCTION_PARAM_PASSTHRU, INTERSECT_NORMAL, INTERSECT_COMP_DATA_USER, INTERSECT_COMP_KEY_INTERNAL);
}
/* }}} */

/* {{{ proto array array_intersect_assoc(array arr1, array arr2 [, array ...])
   Returns the entries of arr1 that have values which are present in all the other arguments. Keys are used to do more restrictive check */
PHP_FUNCTION(array_intersect_assoc)
{
	php_array_intersect_key(INTERNAL_FUNCTION_PARAM_PASSTHRU, INTERSECT_COMP_DATA_INTERNAL);
}
/* }}} */

/* {{{ proto array array_intersect_uassoc(array arr1, array arr2 [, array ...], callback key_compare_func) U
   Returns the entries of arr1 that have values which are present in all the other arguments. Keys are used to do more restrictive check and they are compared by using an user-supplied callback. */
PHP_FUNCTION(array_intersect_uassoc)
{
	php_array_intersect(INTERNAL_FUNCTION_PARAM_PASSTHRU, INTERSECT_ASSOC, INTERSECT_COMP_DATA_INTERNAL, INTERSECT_COMP_KEY_USER);
}
/* }}} */

/* {{{ proto array array_uintersect_assoc(array arr1, array arr2 [, array ...], callback data_compare_func) U
   Returns the entries of arr1 that have values which are present in all the other arguments. Keys are used to do more restrictive check. Data is compared by using an user-supplied callback. */
PHP_FUNCTION(array_uintersect_assoc)
{
	php_array_intersect_key(INTERNAL_FUNCTION_PARAM_PASSTHRU, INTERSECT_COMP_DATA_USER);
}
/* }}} */

/* {{{ proto array array_uintersect_uassoc(array arr1, array arr2 [, array ...], callback data_compare_func, callback key_compare_func)
   Returns the entries of arr1 that have values which are present in all the other arguments. Keys are used to do more restrictive check. Both data and keys are compared by using user-supplied callbacks. */
PHP_FUNCTION(array_uintersect_uassoc)
{
	php_array_intersect(INTERNAL_FUNCTION_PARAM_PASSTHRU, INTERSECT_ASSOC, INTERSECT_COMP_DATA_USER, INTERSECT_COMP_KEY_USER);
}
/* }}} */

static void php_array_diff_key(INTERNAL_FUNCTION_PARAMETERS, int data_compare_type) /* {{{ */
{
	Bucket *p;
	int argc, i;
	zval ***args;
	int (*diff_data_compare_func)(zval **, zval ** TSRMLS_DC) = NULL;
	zend_bool ok;
	zval **data;

	/* Get the argument count */
	argc = ZEND_NUM_ARGS();
	if (data_compare_type == DIFF_COMP_DATA_USER) {
		if (argc < 3) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "at least 3 parameters are required, %d given", ZEND_NUM_ARGS());
			return;
		}
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "+f", &args, &argc, &BG(user_compare_fci), &BG(user_compare_fci_cache)) == FAILURE) {
			return;
		}
		diff_data_compare_func = zval_user_compare;
	} else {
		if (argc < 2) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "at least 2 parameters are required, %d given", ZEND_NUM_ARGS());
			return;
		}
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "+", &args, &argc) == FAILURE) {
			return;
		}
		if (data_compare_type == DIFF_COMP_DATA_INTERNAL) {
			diff_data_compare_func = zval_compare;
		}
	}

	for (i = 0; i < argc; i++) {
		if (Z_TYPE_PP(args[i]) != IS_ARRAY) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Argument #%d is not an array", i + 1);
			RETVAL_NULL();
			goto out;
		}
	}

	array_init(return_value);

	for (p = Z_ARRVAL_PP(args[0])->pListHead; p != NULL; p = p->pListNext) {
		if (p->nKeyLength == 0) {
			ok = 1;
			for (i = 1; i < argc; i++) {
				if (zend_hash_index_find(Z_ARRVAL_PP(args[i]), p->h, (void**)&data) == SUCCESS &&
					(!diff_data_compare_func ||
					diff_data_compare_func((zval**)p->pData, data TSRMLS_CC) == 0)
				) {
					ok = 0;
					break;
				}
			}
			if (ok) {
				Z_ADDREF_PP((zval**)p->pData);
				zend_hash_index_update(Z_ARRVAL_P(return_value), p->h, p->pData, sizeof(zval*), NULL);
			}
		} else {
			ok = 1;
			for (i = 1; i < argc; i++) {
				if (zend_hash_quick_find(Z_ARRVAL_PP(args[i]), p->arKey, p->nKeyLength, p->h, (void**)&data) == SUCCESS &&
					(!diff_data_compare_func ||
					diff_data_compare_func((zval**)p->pData, data TSRMLS_CC) == 0)
				) {
					ok = 0;
					break;
				}
			}
			if (ok) {
				Z_ADDREF_PP((zval**)p->pData);
				zend_hash_quick_update(Z_ARRVAL_P(return_value), p->arKey, p->nKeyLength, p->h, p->pData, sizeof(zval*), NULL);
			}
		}
	}
out:
	efree(args);
}
/* }}} */

static void php_array_diff(INTERNAL_FUNCTION_PARAMETERS, int behavior, int data_compare_type, int key_compare_type) /* {{{ */
{
	zval ***args = NULL;
	HashTable *hash;
	int arr_argc, i, c;
	Bucket ***lists, **list, ***ptrs, *p;
	int req_args;
	char *param_spec;
	zend_fcall_info fci1, fci2;
	zend_fcall_info_cache fci1_cache = empty_fcall_info_cache, fci2_cache = empty_fcall_info_cache;
	zend_fcall_info *fci_key = NULL, *fci_data;
	zend_fcall_info_cache *fci_key_cache = NULL, *fci_data_cache;
	PHP_ARRAY_CMP_FUNC_VARS;

	int (*diff_key_compare_func)(const void *, const void * TSRMLS_DC);
	int (*diff_data_compare_func)(const void *, const void * TSRMLS_DC);

	if (behavior == DIFF_NORMAL) {
		diff_key_compare_func = php_array_key_compare;

		if (data_compare_type == DIFF_COMP_DATA_INTERNAL) {
			/* array_diff */
			req_args = 2;
			param_spec = "+";
			diff_data_compare_func = php_array_data_compare;
		} else if (data_compare_type == DIFF_COMP_DATA_USER) {
			/* array_udiff */
			req_args = 3;
			param_spec = "+f";
			diff_data_compare_func = php_array_user_compare;
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "data_compare_type is %d. This should never happen. Please report as a bug", data_compare_type);
			return;
		}

		if (ZEND_NUM_ARGS() < req_args) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "at least %d parameters are required, %d given", req_args, ZEND_NUM_ARGS());
			return;
		}

		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, param_spec, &args, &arr_argc, &fci1, &fci1_cache) == FAILURE) {
			return;
		}
		fci_data = &fci1;
		fci_data_cache = &fci1_cache;

	} else if (behavior & DIFF_ASSOC) { /* triggered also if DIFF_KEY */
		/* DIFF_KEY is subset of DIFF_ASSOC. When having the former
		 * no comparison of the data is done (part of DIFF_ASSOC) */

		if (data_compare_type == DIFF_COMP_DATA_INTERNAL && key_compare_type == DIFF_COMP_KEY_INTERNAL) {
			/* array_diff_assoc() or array_diff_key() */
			req_args = 2;
			param_spec = "+";
			diff_key_compare_func = php_array_key_compare;
			diff_data_compare_func = php_array_data_compare;
		} else if (data_compare_type == DIFF_COMP_DATA_USER && key_compare_type == DIFF_COMP_KEY_INTERNAL) {
			/* array_udiff_assoc() */
			req_args = 3;
			param_spec = "+f";
			diff_key_compare_func = php_array_key_compare;
			diff_data_compare_func = php_array_user_compare;
			fci_data = &fci1;
			fci_data_cache = &fci1_cache;
		} else if (data_compare_type == DIFF_COMP_DATA_INTERNAL && key_compare_type == DIFF_COMP_KEY_USER) {
			/* array_diff_uassoc() or array_diff_ukey() */
			req_args = 3;
			param_spec = "+f";
			diff_key_compare_func = php_array_user_key_compare;
			diff_data_compare_func = php_array_data_compare;
			fci_key = &fci1;
			fci_key_cache = &fci1_cache;
		} else if (data_compare_type == DIFF_COMP_DATA_USER && key_compare_type == DIFF_COMP_KEY_USER) {
			/* array_udiff_uassoc() */
			req_args = 4;
			param_spec = "+ff";
			diff_key_compare_func = php_array_user_key_compare;
			diff_data_compare_func = php_array_user_compare;
			fci_data = &fci1;
			fci_data_cache = &fci1_cache;
			fci_key = &fci2;
			fci_key_cache = &fci2_cache;
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "data_compare_type is %d. key_compare_type is %d. This should never happen. Please report as a bug", data_compare_type, key_compare_type);
			return;
		}

		if (ZEND_NUM_ARGS() < req_args) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "at least %d parameters are required, %d given", req_args, ZEND_NUM_ARGS());
			return;
		}

		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, param_spec, &args, &arr_argc, &fci1, &fci1_cache, &fci2, &fci2_cache) == FAILURE) {
			return;
		}

	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "behavior is %d. This should never happen. Please report as a bug", behavior);
		return;
	}

	PHP_ARRAY_CMP_FUNC_BACKUP();

	/* for each argument, create and sort list with pointers to the hash buckets */
	lists = (Bucket ***)safe_emalloc(arr_argc, sizeof(Bucket **), 0);
	ptrs = (Bucket ***)safe_emalloc(arr_argc, sizeof(Bucket **), 0);
	php_set_compare_func(PHP_SORT_STRING TSRMLS_CC);

	if (behavior == DIFF_NORMAL && data_compare_type == DIFF_COMP_DATA_USER) {
		BG(user_compare_fci) = *fci_data;
		BG(user_compare_fci_cache) = *fci_data_cache;
	} else if (behavior & DIFF_ASSOC && key_compare_type == DIFF_COMP_KEY_USER) {
		BG(user_compare_fci) = *fci_key;
		BG(user_compare_fci_cache) = *fci_key_cache;
	}

	for (i = 0; i < arr_argc; i++) {
		if (Z_TYPE_PP(args[i]) != IS_ARRAY) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Argument #%d is not an array", i + 1);
			arr_argc = i; /* only free up to i - 1 */
			goto out;
		}
		hash = Z_ARRVAL_PP(args[i]);
		list = (Bucket **) pemalloc((hash->nNumOfElements + 1) * sizeof(Bucket *), hash->persistent);
		if (!list) {
			PHP_ARRAY_CMP_FUNC_RESTORE();

			efree(ptrs);
			efree(lists);
			efree(args);
			RETURN_FALSE;
		}
		lists[i] = list;
		ptrs[i] = list;
		for (p = hash->pListHead; p; p = p->pListNext) {
			*list++ = p;
		}
		*list = NULL;
		if (behavior == DIFF_NORMAL) {
			zend_qsort((void *) lists[i], hash->nNumOfElements, sizeof(Bucket *), diff_data_compare_func TSRMLS_CC);
		} else if (behavior & DIFF_ASSOC) { /* triggered also when DIFF_KEY */
			zend_qsort((void *) lists[i], hash->nNumOfElements, sizeof(Bucket *), diff_key_compare_func TSRMLS_CC);
		}
	}

	/* copy the argument array */
	RETVAL_ZVAL(*args[0], 1, 0);
	if (return_value->value.ht == &EG(symbol_table)) {
		HashTable *ht;
		zval *tmp;

		ALLOC_HASHTABLE(ht);
		zend_hash_init(ht, zend_hash_num_elements(return_value->value.ht), NULL, ZVAL_PTR_DTOR, 0);
		zend_hash_copy(ht, return_value->value.ht, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
		return_value->value.ht = ht;
	}

	/* go through the lists and look for values of ptr[0] that are not in the others */
	while (*ptrs[0]) {
		if ((behavior & DIFF_ASSOC) /* triggered also when DIFF_KEY */
			&&
			key_compare_type == DIFF_COMP_KEY_USER
		) {
			BG(user_compare_fci) = *fci_key;
			BG(user_compare_fci_cache) = *fci_key_cache;
		}
		c = 1;
		for (i = 1; i < arr_argc; i++) {
			Bucket **ptr = ptrs[i];
			if (behavior == DIFF_NORMAL) {
				while (*ptrs[i] && (0 < (c = diff_data_compare_func(ptrs[0], ptrs[i] TSRMLS_CC)))) {
					ptrs[i]++;
				}
			} else if (behavior & DIFF_ASSOC) { /* triggered also when DIFF_KEY */
				while (*ptr && (0 != (c = diff_key_compare_func(ptrs[0], ptr TSRMLS_CC)))) {
					ptr++;
				}
			}
			if (!c) {
				if (behavior == DIFF_NORMAL) {
					if (*ptrs[i]) {
						ptrs[i]++;
					}
					break;
				} else if (behavior == DIFF_ASSOC) {  /* only when DIFF_ASSOC */
					/* In this branch is execute only when DIFF_ASSOC. If behavior == DIFF_KEY
					 * data comparison is not needed - skipped. */
					if (*ptr) {
						if (data_compare_type == DIFF_COMP_DATA_USER) {
							BG(user_compare_fci) = *fci_data;
							BG(user_compare_fci_cache) = *fci_data_cache;
						}
						if (diff_data_compare_func(ptrs[0], ptr TSRMLS_CC) != 0) {
							/* the data is not the same */
							c = -1;
							if (key_compare_type == DIFF_COMP_KEY_USER) {
								BG(user_compare_fci) = *fci_key;
								BG(user_compare_fci_cache) = *fci_key_cache;
							}
						} else {
							break;
							/* we have found the element in other arrays thus we don't want it
							 * in the return_value -> delete from there */
						}
					}
				} else if (behavior == DIFF_KEY) { /* only when DIFF_KEY */
					/* the behavior here differs from INTERSECT_KEY in php_intersect
					 * since in the "diff" case we have to remove the entry from
					 * return_value while when doing intersection the entry must not
					 * be deleted. */
					break; /* remove the key */
				}
			}
		}
		if (!c) {
			/* ptrs[0] in one of the other arguments */
			/* delete all entries with value as ptrs[0] */
			for (;;) {
				p = *ptrs[0];
				if (p->nKeyLength == 0) {
					zend_hash_index_del(Z_ARRVAL_P(return_value), p->h);
				} else {
					zend_hash_quick_del(Z_ARRVAL_P(return_value), p->arKey, p->nKeyLength, p->h);
				}
				if (!*++ptrs[0]) {
					goto out;
				}
				if (behavior == DIFF_NORMAL) {
					if (diff_data_compare_func(ptrs[0] - 1, ptrs[0] TSRMLS_CC)) {
						break;
					}
				} else if (behavior & DIFF_ASSOC) { /* triggered also when DIFF_KEY */
					/* in this case no array_key_compare is needed */
					break;
				}
			}
		} else {
			/* ptrs[0] in none of the other arguments */
			/* skip all entries with value as ptrs[0] */
			for (;;) {
				if (!*++ptrs[0]) {
					goto out;
				}
				if (behavior == DIFF_NORMAL) {
					if (diff_data_compare_func(ptrs[0] - 1, ptrs[0] TSRMLS_CC)) {
						break;
					}
				} else if (behavior & DIFF_ASSOC) { /* triggered also when DIFF_KEY */
					/* in this case no array_key_compare is needed */
					break;
				}
			}
		}
	}
out:
	for (i = 0; i < arr_argc; i++) {
		hash = Z_ARRVAL_PP(args[i]);
		pefree(lists[i], hash->persistent);
	}

	PHP_ARRAY_CMP_FUNC_RESTORE();

	efree(ptrs);
	efree(lists);
	efree(args);
}
/* }}} */

/* {{{ proto array array_diff_key(array arr1, array arr2 [, array ...])
   Returns the entries of arr1 that have keys which are not present in any of the others arguments. This function is like array_diff() but works on the keys instead of the values. The associativity is preserved. */
PHP_FUNCTION(array_diff_key)
{
	php_array_diff_key(INTERNAL_FUNCTION_PARAM_PASSTHRU, DIFF_COMP_DATA_NONE);
}
/* }}} */

/* {{{ proto array array_diff_ukey(array arr1, array arr2 [, array ...], callback key_comp_func)
   Returns the entries of arr1 that have keys which are not present in any of the others arguments. User supplied function is used for comparing the keys. This function is like array_udiff() but works on the keys instead of the values. The associativity is preserved. */
PHP_FUNCTION(array_diff_ukey)
{
	php_array_diff(INTERNAL_FUNCTION_PARAM_PASSTHRU, DIFF_KEY, DIFF_COMP_DATA_INTERNAL, DIFF_COMP_KEY_USER);
}
/* }}} */

/* {{{ proto array array_diff(array arr1, array arr2 [, array ...])
   Returns the entries of arr1 that have values which are not present in any of the others arguments. */
PHP_FUNCTION(array_diff)
{
	php_array_diff(INTERNAL_FUNCTION_PARAM_PASSTHRU, DIFF_NORMAL, DIFF_COMP_DATA_INTERNAL, DIFF_COMP_KEY_INTERNAL);
}
/* }}} */

/* {{{ proto array array_udiff(array arr1, array arr2 [, array ...], callback data_comp_func)
   Returns the entries of arr1 that have values which are not present in any of the others arguments. Elements are compared by user supplied function. */
PHP_FUNCTION(array_udiff)
{
	php_array_diff(INTERNAL_FUNCTION_PARAM_PASSTHRU, DIFF_NORMAL, DIFF_COMP_DATA_USER, DIFF_COMP_KEY_INTERNAL);
}
/* }}} */

/* {{{ proto array array_diff_assoc(array arr1, array arr2 [, array ...])
   Returns the entries of arr1 that have values which are not present in any of the others arguments but do additional checks whether the keys are equal */
PHP_FUNCTION(array_diff_assoc)
{
	php_array_diff_key(INTERNAL_FUNCTION_PARAM_PASSTHRU, DIFF_COMP_DATA_INTERNAL);
}
/* }}} */

/* {{{ proto array array_diff_uassoc(array arr1, array arr2 [, array ...], callback data_comp_func)
   Returns the entries of arr1 that have values which are not present in any of the others arguments but do additional checks whether the keys are equal. Elements are compared by user supplied function. */
PHP_FUNCTION(array_diff_uassoc)
{
	php_array_diff(INTERNAL_FUNCTION_PARAM_PASSTHRU, DIFF_ASSOC, DIFF_COMP_DATA_INTERNAL, DIFF_COMP_KEY_USER);
}
/* }}} */

/* {{{ proto array array_udiff_assoc(array arr1, array arr2 [, array ...], callback key_comp_func)
   Returns the entries of arr1 that have values which are not present in any of the others arguments but do additional checks whether the keys are equal. Keys are compared by user supplied function. */
PHP_FUNCTION(array_udiff_assoc)
{
	php_array_diff_key(INTERNAL_FUNCTION_PARAM_PASSTHRU, DIFF_COMP_DATA_USER);
}
/* }}} */

/* {{{ proto array array_udiff_uassoc(array arr1, array arr2 [, array ...], callback data_comp_func, callback key_comp_func)
   Returns the entries of arr1 that have values which are not present in any of the others arguments but do additional checks whether the keys are equal. Keys and elements are compared by user supplied functions. */
PHP_FUNCTION(array_udiff_uassoc)
{
	php_array_diff(INTERNAL_FUNCTION_PARAM_PASSTHRU, DIFF_ASSOC, DIFF_COMP_DATA_USER, DIFF_COMP_KEY_USER);
}
/* }}} */

#define MULTISORT_ORDER	0
#define MULTISORT_TYPE	1
#define MULTISORT_LAST	2

PHPAPI int php_multisort_compare(const void *a, const void *b TSRMLS_DC) /* {{{ */
{
	Bucket **ab = *(Bucket ***)a;
	Bucket **bb = *(Bucket ***)b;
	int r;
	int result = 0;
	zval temp;

	r = 0;
	do {
		php_set_compare_func(ARRAYG(multisort_flags)[MULTISORT_TYPE][r] TSRMLS_CC);

		ARRAYG(compare_func)(&temp, *((zval **)ab[r]->pData), *((zval **)bb[r]->pData) TSRMLS_CC);
		result = ARRAYG(multisort_flags)[MULTISORT_ORDER][r] * Z_LVAL(temp);
		if (result != 0) {
			return result;
		}
		r++;
	} while (ab[r] != NULL);

	return result;
}
/* }}} */

#define MULTISORT_ABORT						\
	for (k = 0; k < MULTISORT_LAST; k++)	\
		efree(ARRAYG(multisort_flags)[k]);	\
	efree(arrays);							\
	efree(args);							\
	RETURN_FALSE;

/* {{{ proto bool array_multisort(array ar1 [, SORT_ASC|SORT_DESC [, SORT_REGULAR|SORT_NUMERIC|SORT_STRING|SORT_NATURAL|SORT_FLAG_CASE]] [, array ar2 [, SORT_ASC|SORT_DESC [, SORT_REGULAR|SORT_NUMERIC|SORT_STRING|SORT_NATURAL|SORT_FLAG_CASE]], ...])
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
	int				parse_state[MULTISORT_LAST];   /* 0 - flag not allowed 1 - flag allowed */
	int				sort_order = PHP_SORT_ASC;
	int				sort_type  = PHP_SORT_REGULAR;
	int				i, k;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "+", &args, &argc) == FAILURE) {
		return;
	}

	/* Allocate space for storing pointers to input arrays and sort flags. */
	arrays = (zval ***)ecalloc(argc, sizeof(zval **));
	for (i = 0; i < MULTISORT_LAST; i++) {
		parse_state[i] = 0;
		ARRAYG(multisort_flags)[i] = (int *)ecalloc(argc, sizeof(int));
	}

	/* Here we go through the input arguments and parse them. Each one can
	 * be either an array or a sort flag which follows an array. If not
	 * specified, the sort flags defaults to PHP_SORT_ASC and PHP_SORT_REGULAR
	 * accordingly. There can't be two sort flags of the same type after an
	 * array, and the very first argument has to be an array. */
	for (i = 0; i < argc; i++) {
		if (Z_TYPE_PP(args[i]) == IS_ARRAY) {
			/* We see the next array, so we update the sort flags of
			 * the previous array and reset the sort flags. */
			if (i > 0) {
				ARRAYG(multisort_flags)[MULTISORT_ORDER][num_arrays - 1] = sort_order;
				ARRAYG(multisort_flags)[MULTISORT_TYPE][num_arrays - 1] = sort_type;
				sort_order = PHP_SORT_ASC;
				sort_type = PHP_SORT_REGULAR;
			}
			arrays[num_arrays++] = args[i];

			/* Next one may be an array or a list of sort flags. */
			for (k = 0; k < MULTISORT_LAST; k++) {
				parse_state[k] = 1;
			}
		} else if (Z_TYPE_PP(args[i]) == IS_LONG) {
			switch (Z_LVAL_PP(args[i]) & ~PHP_SORT_FLAG_CASE) {
				case PHP_SORT_ASC:
				case PHP_SORT_DESC:
					/* flag allowed here */
					if (parse_state[MULTISORT_ORDER] == 1) {
						/* Save the flag and make sure then next arg is not the current flag. */
						sort_order = Z_LVAL_PP(args[i]) == PHP_SORT_DESC ? -1 : 1;
						parse_state[MULTISORT_ORDER] = 0;
					} else {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Argument #%d is expected to be an array or sorting flag that has not already been specified", i + 1);
						MULTISORT_ABORT;
					}
					break;

				case PHP_SORT_REGULAR:
				case PHP_SORT_NUMERIC:
				case PHP_SORT_STRING:
				case PHP_SORT_NATURAL:
#if HAVE_STRCOLL
				case PHP_SORT_LOCALE_STRING:
#endif
					/* flag allowed here */
					if (parse_state[MULTISORT_TYPE] == 1) {
						/* Save the flag and make sure then next arg is not the current flag. */
						sort_type = Z_LVAL_PP(args[i]);
						parse_state[MULTISORT_TYPE] = 0;
					} else {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Argument #%d is expected to be an array or sorting flag that has not already been specified", i + 1);
						MULTISORT_ABORT;
					}
					break;

				default:
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Argument #%d is an unknown sort flag", i + 1);
					MULTISORT_ABORT;
					break;

			}
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Argument #%d is expected to be an array or a sort flag", i + 1);
			MULTISORT_ABORT;
		}
	}
	/* Take care of the last array sort flags. */
	ARRAYG(multisort_flags)[MULTISORT_ORDER][num_arrays - 1] = sort_order;
	ARRAYG(multisort_flags)[MULTISORT_TYPE][num_arrays - 1] = sort_type;

	/* Make sure the arrays are of the same size. */
	array_size = zend_hash_num_elements(Z_ARRVAL_PP(arrays[0]));
	for (i = 0; i < num_arrays; i++) {
		if (zend_hash_num_elements(Z_ARRVAL_PP(arrays[i])) != array_size) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Array sizes are inconsistent");
			MULTISORT_ABORT;
		}
	}

	/* If all arrays are empty we don't need to do anything. */
	if (array_size < 1) {
		for (k = 0; k < MULTISORT_LAST; k++) {
			efree(ARRAYG(multisort_flags)[k]);
		}
		efree(arrays);
		efree(args);
		RETURN_TRUE;
	}

	/* Create the indirection array. This array is of size MxN, where
	 * M is the number of entries in each input array and N is the number
	 * of the input arrays + 1. The last column is NULL to indicate the end
	 * of the row. */
	indirect = (Bucket ***)safe_emalloc(array_size, sizeof(Bucket **), 0);
	for (i = 0; i < array_size; i++) {
		indirect[i] = (Bucket **)safe_emalloc((num_arrays + 1), sizeof(Bucket *), 0);
	}
	for (i = 0; i < num_arrays; i++) {
		k = 0;
		for (p = Z_ARRVAL_PP(arrays[i])->pListHead; p; p = p->pListNext, k++) {
			indirect[k][i] = p;
		}
	}
	for (k = 0; k < array_size; k++) {
		indirect[k][num_arrays] = NULL;
	}

	/* Do the actual sort magic - bada-bim, bada-boom. */
	zend_qsort(indirect, array_size, sizeof(Bucket **), php_multisort_compare TSRMLS_CC);

	/* Restructure the arrays based on sorted indirect - this is mostly taken from zend_hash_sort() function. */
	HANDLE_BLOCK_INTERRUPTIONS();
	for (i = 0; i < num_arrays; i++) {
		hash = Z_ARRVAL_PP(arrays[i]);
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

	/* Clean up. */
	for (i = 0; i < array_size; i++) {
		efree(indirect[i]);
	}
	efree(indirect);
	for (k = 0; k < MULTISORT_LAST; k++) {
		efree(ARRAYG(multisort_flags)[k]);
	}
	efree(arrays);
	efree(args);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto mixed array_rand(array input [, int num_req])
   Return key/keys for random entry/entries in the array */
PHP_FUNCTION(array_rand)
{
	zval *input;
	long randval, num_req = 1;
	int num_avail, key_type;
	char *string_key;
	uint string_key_len;
	ulong num_key;
	HashPosition pos;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|l", &input, &num_req) == FAILURE) {
		return;
	}

	num_avail = zend_hash_num_elements(Z_ARRVAL_P(input));

	if (ZEND_NUM_ARGS() > 1) {
		if (num_req <= 0 || num_req > num_avail) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Second argument has to be between 1 and the number of elements in the array");
			return;
		}
	}

	/* Make the return value an array only if we need to pass back more than one result. */
	if (num_req > 1) {
		array_init_size(return_value, num_req);
	}

	/* We can't use zend_hash_index_find() because the array may have string keys or gaps. */
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(input), &pos);
	while (num_req && (key_type = zend_hash_get_current_key_ex(Z_ARRVAL_P(input), &string_key, &string_key_len, &num_key, 0, &pos)) != HASH_KEY_NON_EXISTENT) {

		randval = php_rand(TSRMLS_C);

		if ((double) (randval / (PHP_RAND_MAX + 1.0)) < (double) num_req / (double) num_avail) {
			/* If we are returning a single result, just do it. */
			if (Z_TYPE_P(return_value) != IS_ARRAY) {
				if (key_type == HASH_KEY_IS_STRING) {
					RETURN_STRINGL(string_key, string_key_len - 1, 1);
				} else {
					RETURN_LONG(num_key);
				}
			} else {
				/* Append the result to the return value. */
				if (key_type == HASH_KEY_IS_STRING) {
					add_next_index_stringl(return_value, string_key, string_key_len - 1, 1);
				} else {
					add_next_index_long(return_value, num_key);
				}
			}
			num_req--;
		}
		num_avail--;
		zend_hash_move_forward_ex(Z_ARRVAL_P(input), &pos);
	}
}
/* }}} */

/* {{{ proto mixed array_sum(array input)
   Returns the sum of the array entries */
PHP_FUNCTION(array_sum)
{
	zval *input,
		 **entry,
		 entry_n;
	HashPosition pos;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &input) == FAILURE) {
		return;
	}

	ZVAL_LONG(return_value, 0);

	for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(input), &pos);
		zend_hash_get_current_data_ex(Z_ARRVAL_P(input), (void **)&entry, &pos) == SUCCESS;
		zend_hash_move_forward_ex(Z_ARRVAL_P(input), &pos)
	) {
		if (Z_TYPE_PP(entry) == IS_ARRAY || Z_TYPE_PP(entry) == IS_OBJECT) {
			continue;
		}
		entry_n = **entry;
		zval_copy_ctor(&entry_n);
		convert_scalar_to_number(&entry_n TSRMLS_CC);
		fast_add_function(return_value, return_value, &entry_n TSRMLS_CC);
	}
}
/* }}} */

/* {{{ proto mixed array_product(array input)
   Returns the product of the array entries */
PHP_FUNCTION(array_product)
{
	zval *input,
		 **entry,
		 entry_n;
	HashPosition pos;
	double dval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &input) == FAILURE) {
		return;
	}

	ZVAL_LONG(return_value, 1);
	if (!zend_hash_num_elements(Z_ARRVAL_P(input))) {
		return;
	}

	for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(input), &pos);
		zend_hash_get_current_data_ex(Z_ARRVAL_P(input), (void **)&entry, &pos) == SUCCESS;
		zend_hash_move_forward_ex(Z_ARRVAL_P(input), &pos)
	) {
		if (Z_TYPE_PP(entry) == IS_ARRAY || Z_TYPE_PP(entry) == IS_OBJECT) {
			continue;
		}
		entry_n = **entry;
		zval_copy_ctor(&entry_n);
		convert_scalar_to_number(&entry_n TSRMLS_CC);

		if (Z_TYPE(entry_n) == IS_LONG && Z_TYPE_P(return_value) == IS_LONG) {
			dval = (double)Z_LVAL_P(return_value) * (double)Z_LVAL(entry_n);
			if ( (double)LONG_MIN <= dval && dval <= (double)LONG_MAX ) {
				Z_LVAL_P(return_value) *= Z_LVAL(entry_n);
				continue;
			}
		}
		convert_to_double(return_value);
		convert_to_double(&entry_n);
		Z_DVAL_P(return_value) *= Z_DVAL(entry_n);
	}
}
/* }}} */

/* {{{ proto mixed array_reduce(array input, mixed callback [, mixed initial])
   Iteratively reduce the array to a single value via the callback. */
PHP_FUNCTION(array_reduce)
{
	zval *input;
	zval **args[2];
	zval **operand;
	zval *result = NULL;
	zval *retval;
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache = empty_fcall_info_cache;
	zval *initial = NULL;
	HashPosition pos;
	HashTable *htbl;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "af|z", &input, &fci, &fci_cache, &initial) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS() > 2) {
		ALLOC_ZVAL(result);
		MAKE_COPY_ZVAL(&initial, result);
	} else {
		MAKE_STD_ZVAL(result);
		ZVAL_NULL(result);
	}

	/* (zval **)input points to an element of argument stack
	 * the base pointer of which is subject to change.
	 * thus we need to keep the pointer to the hashtable for safety */
	htbl = Z_ARRVAL_P(input);

	if (zend_hash_num_elements(htbl) == 0) {
		if (result) {
			RETVAL_ZVAL(result, 1, 1);
		}
		return;
	}

	fci.retval_ptr_ptr = &retval;
	fci.param_count = 2;
	fci.no_separation = 0;

	zend_hash_internal_pointer_reset_ex(htbl, &pos);
	while (zend_hash_get_current_data_ex(htbl, (void **)&operand, &pos) == SUCCESS) {

		if (result) {
			args[0] = &result;
			args[1] = operand;
			fci.params = args;

			if (zend_call_function(&fci, &fci_cache TSRMLS_CC) == SUCCESS && retval) {
				zval_ptr_dtor(&result);
				result = retval;
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "An error occurred while invoking the reduction callback");
				return;
			}
		} else {
			result = *operand;
			zval_add_ref(&result);
		}
		zend_hash_move_forward_ex(htbl, &pos);
	}
	RETVAL_ZVAL(result, 1, 1);
}
/* }}} */

/* {{{ proto array array_filter(array input [, mixed callback])
   Filters elements from the array via the callback. */
PHP_FUNCTION(array_filter)
{
	zval *array;
	zval **operand;
	zval **args[2];
	zval *retval = NULL;
    zval *key = NULL;
	zend_bool have_callback = 0;
	long use_type = 0;
	char *string_key;
	zend_fcall_info fci = empty_fcall_info;
	zend_fcall_info_cache fci_cache = empty_fcall_info_cache;
	uint string_key_len;
	ulong num_key;
	HashPosition pos;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|fl", &array, &fci, &fci_cache, &use_type) == FAILURE) {
		return;
	}

	array_init(return_value);
	if (zend_hash_num_elements(Z_ARRVAL_P(array)) == 0) {
		return;
	}

	if (ZEND_NUM_ARGS() > 1) {
		have_callback = 1;
		fci.no_separation = 0;
		fci.retval_ptr_ptr = &retval;

		if (use_type == ARRAY_FILTER_USE_BOTH) {
			fci.param_count = 2;
			args[1] = &key;
		} else {
			fci.param_count = 1;
			if (use_type == ARRAY_FILTER_USE_KEY) {
				args[0] = &key;
			}
		}
	}

	for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(array), &pos);
		zend_hash_get_current_data_ex(Z_ARRVAL_P(array), (void **)&operand, &pos) == SUCCESS;
		zend_hash_move_forward_ex(Z_ARRVAL_P(array), &pos)
	) {
		int key_type = zend_hash_get_current_key_ex(Z_ARRVAL_P(array), &string_key, &string_key_len, &num_key, 0, &pos);

		if (have_callback) {
			if (use_type) {
				MAKE_STD_ZVAL(key);
				/* Set up the key */
				switch (key_type) {
					case HASH_KEY_IS_LONG:
						Z_TYPE_P(key) = IS_LONG;
						Z_LVAL_P(key) = num_key;
						break;

					case HASH_KEY_IS_STRING:
						ZVAL_STRINGL(key, string_key, string_key_len - 1, 1);
						break;
				}
			}

			if (use_type != ARRAY_FILTER_USE_KEY) {
				args[0] = operand;
			}
			fci.params = args;

			if (zend_call_function(&fci, &fci_cache TSRMLS_CC) == SUCCESS && retval) {
				int retval_true = zend_is_true(retval);

				zval_ptr_dtor(&retval);
				if (use_type) {
					zval_ptr_dtor(&key);
				}
				if (!retval_true) {
					continue;
				}
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "An error occurred while invoking the filter callback");
				return;
			}
		} else if (!zend_is_true(*operand)) {
			continue;
		}

		zval_add_ref(operand);
		switch (key_type) {
			case HASH_KEY_IS_STRING:
				zend_hash_update(Z_ARRVAL_P(return_value), string_key, string_key_len, operand, sizeof(zval *), NULL);
				break;

			case HASH_KEY_IS_LONG:
				zend_hash_index_update(Z_ARRVAL_P(return_value), num_key, operand, sizeof(zval *), NULL);
				break;
		}
	}
}
/* }}} */

/* {{{ proto array array_map(mixed callback, array input1 [, array input2 ,...])
   Applies the callback to the elements in given arrays. */
PHP_FUNCTION(array_map)
{
	zval ***arrays = NULL;
	int n_arrays = 0;
	zval ***params;
	zval *result, *null;
	HashPosition *array_pos;
	zval **args;
	zend_fcall_info fci = empty_fcall_info;
	zend_fcall_info_cache fci_cache = empty_fcall_info_cache;
	int i, k, maxlen = 0;
	int *array_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "f!+", &fci, &fci_cache, &arrays, &n_arrays) == FAILURE) {
		return;
	}

	RETVAL_NULL();

	args = (zval **)safe_emalloc(n_arrays, sizeof(zval *), 0);
	array_len = (int *)safe_emalloc(n_arrays, sizeof(int), 0);
	array_pos = (HashPosition *)safe_emalloc(n_arrays, sizeof(HashPosition), 0);

	for (i = 0; i < n_arrays; i++) {
		if (Z_TYPE_PP(arrays[i]) != IS_ARRAY) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Argument #%d should be an array", i + 2);
			efree(arrays);
			efree(args);
			efree(array_len);
			efree(array_pos);
			return;
		}
		SEPARATE_ZVAL_IF_NOT_REF(arrays[i]);
		args[i] = *arrays[i];
		array_len[i] = zend_hash_num_elements(Z_ARRVAL_PP(arrays[i]));
		if (array_len[i] > maxlen) {
			maxlen = array_len[i];
		}
		zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(arrays[i]), &array_pos[i]);
	}

	efree(arrays);

	/* Short-circuit: if no callback and only one array, just return it. */
	if (!ZEND_FCI_INITIALIZED(fci) && n_arrays == 1) {
		RETVAL_ZVAL(args[0], 1, 0);
		efree(array_len);
		efree(array_pos);
		efree(args);
		return;
	}

	array_init_size(return_value, maxlen);
	params = (zval ***)safe_emalloc(n_arrays, sizeof(zval **), 0);
	MAKE_STD_ZVAL(null);
	ZVAL_NULL(null);

	/* We iterate through all the arrays at once. */
	for (k = 0; k < maxlen; k++) {
		uint str_key_len;
		ulong num_key;
		char *str_key;
		int key_type = 0;

		/* If no callback, the result will be an array, consisting of current
		 * entries from all arrays. */
		if (!ZEND_FCI_INITIALIZED(fci)) {
			MAKE_STD_ZVAL(result);
			array_init_size(result, n_arrays);
		}

		for (i = 0; i < n_arrays; i++) {
			/* If this array still has elements, add the current one to the
			 * parameter list, otherwise use null value. */
			if (k < array_len[i]) {
				zend_hash_get_current_data_ex(Z_ARRVAL_P(args[i]), (void **)&params[i], &array_pos[i]);

				/* It is safe to store only last value of key type, because
				 * this loop will run just once if there is only 1 array. */
				if (n_arrays == 1) {
					key_type = zend_hash_get_current_key_ex(Z_ARRVAL_P(args[0]), &str_key, &str_key_len, &num_key, 0, &array_pos[i]);
				}
				zend_hash_move_forward_ex(Z_ARRVAL_P(args[i]), &array_pos[i]);
			} else {
				params[i] = &null;
			}

			if (!ZEND_FCI_INITIALIZED(fci)) {
				zval_add_ref(params[i]);
				add_next_index_zval(result, *params[i]);
			}
		}

		if (ZEND_FCI_INITIALIZED(fci)) {
			fci.retval_ptr_ptr = &result;
			fci.param_count = n_arrays;
			fci.params = params;
			fci.no_separation = 0;

			if (zend_call_function(&fci, &fci_cache TSRMLS_CC) != SUCCESS || !result) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "An error occurred while invoking the map callback");
				efree(array_len);
				efree(args);
				efree(array_pos);
				zval_dtor(return_value);
				zval_ptr_dtor(&null);
				efree(params);
				RETURN_NULL();
			}
		}

		if (n_arrays > 1) {
			add_next_index_zval(return_value, result);
		} else {
			if (key_type == HASH_KEY_IS_STRING) {
				add_assoc_zval_ex(return_value, str_key, str_key_len, result);
			} else {
				add_index_zval(return_value, num_key, result);
			}
		}
	}

	zval_ptr_dtor(&null);
	efree(params);
	efree(array_len);
	efree(array_pos);
	efree(args);
}
/* }}} */

/* {{{ proto bool array_key_exists(mixed key, array search)
   Checks if the given key or index exists in the array */
PHP_FUNCTION(array_key_exists)
{
	zval *key;					/* key to check for */
	HashTable *array;			/* array to check in */

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zH", &key, &array) == FAILURE) {
		return;
	}

	switch (Z_TYPE_P(key)) {
		case IS_STRING:
			if (zend_symtable_exists(array, Z_STRVAL_P(key), Z_STRLEN_P(key) + 1)) {
				RETURN_TRUE;
			}
			RETURN_FALSE;
		case IS_LONG:
			if (zend_hash_index_exists(array, Z_LVAL_P(key))) {
				RETURN_TRUE;
			}
			RETURN_FALSE;
		case IS_NULL:
			if (zend_hash_exists(array, "", 1)) {
				RETURN_TRUE;
			}
			RETURN_FALSE;

		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "The first argument should be either a string or an integer");
			RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto array array_chunk(array input, int size [, bool preserve_keys])
   Split array into chunks */
PHP_FUNCTION(array_chunk)
{
	int argc = ZEND_NUM_ARGS(), key_type, num_in;
	long size, current = 0;
	char *str_key;
	uint str_key_len;
	ulong num_key;
	zend_bool preserve_keys = 0;
	zval *input = NULL;
	zval *chunk = NULL;
	zval **entry;
	HashPosition pos;

	if (zend_parse_parameters(argc TSRMLS_CC, "al|b", &input, &size, &preserve_keys) == FAILURE) {
		return;
	}
	/* Do bounds checking for size parameter. */
	if (size < 1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Size parameter expected to be greater than 0");
		return;
	}

	num_in = zend_hash_num_elements(Z_ARRVAL_P(input));

	if (size > num_in) {
		size = num_in > 0 ? num_in : 1;
	}

	array_init_size(return_value, ((num_in - 1) / size) + 1);

	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(input), &pos);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(input), (void**)&entry, &pos) == SUCCESS) {
		/* If new chunk, create and initialize it. */
		if (!chunk) {
			MAKE_STD_ZVAL(chunk);
			array_init_size(chunk, size);
		}

		/* Add entry to the chunk, preserving keys if necessary. */
		zval_add_ref(entry);

		if (preserve_keys) {
			key_type = zend_hash_get_current_key_ex(Z_ARRVAL_P(input), &str_key, &str_key_len, &num_key, 0, &pos);
			switch (key_type) {
				case HASH_KEY_IS_STRING:
					add_assoc_zval_ex(chunk, str_key, str_key_len, *entry);
					break;
				default:
					add_index_zval(chunk, num_key, *entry);
					break;
			}
		} else {
			add_next_index_zval(chunk, *entry);
		}

		/* If reached the chunk size, add it to the result array, and reset the
		 * pointer. */
		if (!(++current % size)) {
			add_next_index_zval(return_value, chunk);
			chunk = NULL;
		}

		zend_hash_move_forward_ex(Z_ARRVAL_P(input), &pos);
	}

	/* Add the final chunk if there is one. */
	if (chunk) {
		add_next_index_zval(return_value, chunk);
	}
}
/* }}} */

/* {{{ proto array array_combine(array keys, array values)
   Creates an array by using the elements of the first parameter as keys and the elements of the second as the corresponding values */
PHP_FUNCTION(array_combine)
{
	zval *values, *keys;
	HashPosition pos_values, pos_keys;
	zval **entry_keys, **entry_values;
	int num_keys, num_values;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "aa", &keys, &values) == FAILURE) {
		return;
	}

	num_keys = zend_hash_num_elements(Z_ARRVAL_P(keys));
	num_values = zend_hash_num_elements(Z_ARRVAL_P(values));

	if (num_keys != num_values) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Both parameters should have an equal number of elements");
		RETURN_FALSE;
	}

	array_init_size(return_value, num_keys);

	if (!num_keys) {
		return;
	}

	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(keys), &pos_keys);
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(values), &pos_values);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(keys), (void **)&entry_keys, &pos_keys) == SUCCESS &&
		zend_hash_get_current_data_ex(Z_ARRVAL_P(values), (void **)&entry_values, &pos_values) == SUCCESS
	) {
		if (Z_TYPE_PP(entry_keys) == IS_LONG) {
			zval_add_ref(entry_values);
			add_index_zval(return_value, Z_LVAL_PP(entry_keys), *entry_values);
		} else {
			zval key, *key_ptr = *entry_keys;

			if (Z_TYPE_PP(entry_keys) != IS_STRING) {
				key = **entry_keys;
				zval_copy_ctor(&key);
				convert_to_string(&key);
				key_ptr = &key;
			}

			zval_add_ref(entry_values);
			add_assoc_zval_ex(return_value, Z_STRVAL_P(key_ptr), Z_STRLEN_P(key_ptr) + 1, *entry_values);

			if (key_ptr != *entry_keys) {
				zval_dtor(&key);
			}
		}

		zend_hash_move_forward_ex(Z_ARRVAL_P(keys), &pos_keys);
		zend_hash_move_forward_ex(Z_ARRVAL_P(values), &pos_values);
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
