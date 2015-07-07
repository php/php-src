/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Antony Dovgal <tony@daylessday.org>                          |
  |         Etienne Kneuss <colder@php.net>                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"

#include "php_spl.h"
#include "spl_functions.h"
#include "spl_engine.h"
#include "spl_fixedarray.h"
#include "spl_exceptions.h"
#include "spl_iterators.h"

zend_object_handlers spl_handler_SplFixedArray;
PHPAPI zend_class_entry *spl_ce_SplFixedArray;

#ifdef COMPILE_DL_SPL_FIXEDARRAY
ZEND_GET_MODULE(spl_fixedarray)
#endif

typedef struct _spl_fixedarray { /* {{{ */
	zend_long size;
	zval *elements;
} spl_fixedarray;
/* }}} */

typedef struct _spl_fixedarray_object { /* {{{ */
	spl_fixedarray        *array;
	zend_function         *fptr_offset_get;
	zend_function         *fptr_offset_set;
	zend_function         *fptr_offset_has;
	zend_function         *fptr_offset_del;
	zend_function         *fptr_count;
	int                    current;
	int                    flags;
	zend_class_entry      *ce_get_iterator;
	zend_object            std;
} spl_fixedarray_object;
/* }}} */

typedef struct _spl_fixedarray_it { /* {{{ */
	zend_user_iterator     intern;
} spl_fixedarray_it;
/* }}} */

#define SPL_FIXEDARRAY_OVERLOADED_REWIND  0x0001
#define SPL_FIXEDARRAY_OVERLOADED_VALID   0x0002
#define SPL_FIXEDARRAY_OVERLOADED_KEY     0x0004
#define SPL_FIXEDARRAY_OVERLOADED_CURRENT 0x0008
#define SPL_FIXEDARRAY_OVERLOADED_NEXT    0x0010

static inline spl_fixedarray_object *spl_fixed_array_from_obj(zend_object *obj) /* {{{ */ {
	return (spl_fixedarray_object*)((char*)(obj) - XtOffsetOf(spl_fixedarray_object, std));
}
/* }}} */

#define Z_SPLFIXEDARRAY_P(zv)  spl_fixed_array_from_obj(Z_OBJ_P((zv)))

static void spl_fixedarray_init(spl_fixedarray *array, zend_long size) /* {{{ */
{
	if (size > 0) {
		array->size = 0; /* reset size in case ecalloc() fails */
		array->elements = ecalloc(size, sizeof(zval));
		array->size = size;
	} else {
		array->elements = NULL;
		array->size = 0;
	}
}
/* }}} */

static void spl_fixedarray_resize(spl_fixedarray *array, zend_long size) /* {{{ */
{
	if (size == array->size) {
		/* nothing to do */
		return;
	}

	/* first initialization */
	if (array->size == 0) {
		spl_fixedarray_init(array, size);
		return;
	}

	/* clearing the array */
	if (size == 0) {
		zend_long i;

		for (i = 0; i < array->size; i++) {
			zval_ptr_dtor(&(array->elements[i]));
		}

		if (array->elements) {
			efree(array->elements);
			array->elements = NULL;
		}
	} else if (size > array->size) {
		array->elements = safe_erealloc(array->elements, size, sizeof(zval), 0);
		memset(array->elements + array->size, '\0', sizeof(zval) * (size - array->size));
	} else { /* size < array->size */
		zend_long i;

		for (i = size; i < array->size; i++) {
			zval_ptr_dtor(&(array->elements[i]));
		}
		array->elements = erealloc(array->elements, sizeof(zval) * size);
	}

	array->size = size;
}
/* }}} */

static void spl_fixedarray_copy(spl_fixedarray *to, spl_fixedarray *from) /* {{{ */
{
	int i;
	for (i = 0; i < from->size; i++) {
		ZVAL_COPY(&to->elements[i], &from->elements[i]);
	}
}
/* }}} */

static HashTable* spl_fixedarray_object_get_gc(zval *obj, zval **table, int *n) /* {{{{ */
{
	spl_fixedarray_object *intern  = Z_SPLFIXEDARRAY_P(obj);
	HashTable *ht = zend_std_get_properties(obj);

	if (intern->array) {
		*table = intern->array->elements;
		*n = (int)intern->array->size;
	} else {
		*table = NULL;
		*n = 0;
	}

	return ht;
}
/* }}}} */

static HashTable* spl_fixedarray_object_get_properties(zval *obj) /* {{{{ */
{
	spl_fixedarray_object *intern  = Z_SPLFIXEDARRAY_P(obj);
	HashTable *ht = zend_std_get_properties(obj);
	zend_long  i = 0;

	if (intern->array) {
		zend_long j = zend_hash_num_elements(ht);

		for (i = 0; i < intern->array->size; i++) {
			if (!Z_ISUNDEF(intern->array->elements[i])) {
				zend_hash_index_update(ht, i, &intern->array->elements[i]);
				if (Z_REFCOUNTED(intern->array->elements[i])){
					Z_ADDREF(intern->array->elements[i]);
				}
			} else {
				zend_hash_index_update(ht, i, &EG(uninitialized_zval));
			}
		}
		if (j > intern->array->size) {
			for (i = intern->array->size; i < j; ++i) {
				zend_hash_index_del(ht, i);
			}
		}
	}

	return ht;
}
/* }}}} */

static void spl_fixedarray_object_free_storage(zend_object *object) /* {{{ */
{
	spl_fixedarray_object *intern = spl_fixed_array_from_obj(object);
	zend_long i;

	if (intern->array) {
		for (i = 0; i < intern->array->size; i++) {
			zval_ptr_dtor(&(intern->array->elements[i]));
		}

		if (intern->array->size > 0 && intern->array->elements) {
			efree(intern->array->elements);
		}
		efree(intern->array);
	}

	zend_object_std_dtor(&intern->std);
}
/* }}} */

zend_object_iterator *spl_fixedarray_get_iterator(zend_class_entry *ce, zval *object, int by_ref);

static zend_object *spl_fixedarray_object_new_ex(zend_class_entry *class_type, zval *orig, int clone_orig) /* {{{ */
{
	spl_fixedarray_object *intern;
	zend_class_entry     *parent = class_type;
	int                   inherited = 0;

	intern = ecalloc(1, sizeof(spl_fixedarray_object) + zend_object_properties_size(parent));

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

	intern->current = 0;
	intern->flags = 0;

	if (orig && clone_orig) {
		spl_fixedarray_object *other = Z_SPLFIXEDARRAY_P(orig);
		intern->ce_get_iterator = other->ce_get_iterator;
		if (!other->array) {
			/* leave a empty object, will be dtor later by CLONE handler */
			zend_throw_exception(spl_ce_RuntimeException, "The instance wasn't initialized properly", 0);
		} else {
			intern->array = emalloc(sizeof(spl_fixedarray));
			spl_fixedarray_init(intern->array, other->array->size);
			spl_fixedarray_copy(intern->array, other->array);
		}
	}

	while (parent) {
		if (parent == spl_ce_SplFixedArray) {
			intern->std.handlers = &spl_handler_SplFixedArray;
			class_type->get_iterator = spl_fixedarray_get_iterator;
			break;
		}

		parent = parent->parent;
		inherited = 1;
	}

	if (!parent) { /* this must never happen */
		php_error_docref(NULL, E_COMPILE_ERROR, "Internal compiler error, Class is not child of SplFixedArray");
	}

	if (!class_type->iterator_funcs.zf_current) {
		class_type->iterator_funcs.zf_rewind = zend_hash_str_find_ptr(&class_type->function_table, "rewind", sizeof("rewind") - 1);
		class_type->iterator_funcs.zf_valid = zend_hash_str_find_ptr(&class_type->function_table, "valid", sizeof("valid") - 1);
		class_type->iterator_funcs.zf_key = zend_hash_str_find_ptr(&class_type->function_table, "key", sizeof("key") - 1);
		class_type->iterator_funcs.zf_current = zend_hash_str_find_ptr(&class_type->function_table, "current", sizeof("current") - 1);
		class_type->iterator_funcs.zf_next = zend_hash_str_find_ptr(&class_type->function_table, "next", sizeof("next") - 1);
	}
	if (inherited) {
		if (class_type->iterator_funcs.zf_rewind->common.scope  != parent) {
			intern->flags |= SPL_FIXEDARRAY_OVERLOADED_REWIND;
		}
		if (class_type->iterator_funcs.zf_valid->common.scope   != parent) {
			intern->flags |= SPL_FIXEDARRAY_OVERLOADED_VALID;
		}
		if (class_type->iterator_funcs.zf_key->common.scope     != parent) {
			intern->flags |= SPL_FIXEDARRAY_OVERLOADED_KEY;
		}
		if (class_type->iterator_funcs.zf_current->common.scope != parent) {
			intern->flags |= SPL_FIXEDARRAY_OVERLOADED_CURRENT;
		}
		if (class_type->iterator_funcs.zf_next->common.scope    != parent) {
			intern->flags |= SPL_FIXEDARRAY_OVERLOADED_NEXT;
		}

		intern->fptr_offset_get = zend_hash_str_find_ptr(&class_type->function_table, "offsetget", sizeof("offsetget") - 1);
		if (intern->fptr_offset_get->common.scope == parent) {
			intern->fptr_offset_get = NULL;
		}
		intern->fptr_offset_set = zend_hash_str_find_ptr(&class_type->function_table, "offsetset", sizeof("offsetset") - 1);
		if (intern->fptr_offset_set->common.scope == parent) {
			intern->fptr_offset_set = NULL;
		}
		intern->fptr_offset_has = zend_hash_str_find_ptr(&class_type->function_table, "offsetexists", sizeof("offsetexists") - 1);
		if (intern->fptr_offset_has->common.scope == parent) {
			intern->fptr_offset_has = NULL;
		}
		intern->fptr_offset_del = zend_hash_str_find_ptr(&class_type->function_table, "offsetunset", sizeof("offsetunset") - 1);
		if (intern->fptr_offset_del->common.scope == parent) {
			intern->fptr_offset_del = NULL;
		}
		intern->fptr_count = zend_hash_str_find_ptr(&class_type->function_table, "count", sizeof("count") - 1);
		if (intern->fptr_count->common.scope == parent) {
			intern->fptr_count = NULL;
		}
	}

	return &intern->std;
}
/* }}} */

static zend_object *spl_fixedarray_new(zend_class_entry *class_type) /* {{{ */
{
	return spl_fixedarray_object_new_ex(class_type, NULL, 0);
}
/* }}} */

static zend_object *spl_fixedarray_object_clone(zval *zobject) /* {{{ */
{
	zend_object *old_object;
	zend_object *new_object;

	old_object  = Z_OBJ_P(zobject);
	new_object = spl_fixedarray_object_new_ex(old_object->ce, zobject, 1);

	zend_objects_clone_members(new_object, old_object);

	return new_object;
}
/* }}} */

static inline zval *spl_fixedarray_object_read_dimension_helper(spl_fixedarray_object *intern, zval *offset) /* {{{ */
{
	zend_long index;

	/* we have to return NULL on error here to avoid memleak because of
	 * ZE duplicating uninitialized_zval_ptr */
	if (!offset) {
		zend_throw_exception(spl_ce_RuntimeException, "Index invalid or out of range", 0);
		return NULL;
	}

	if (Z_TYPE_P(offset) != IS_LONG) {
		index = spl_offset_convert_to_long(offset);
	} else {
		index = Z_LVAL_P(offset);
	}

	if (index < 0 || intern->array == NULL || index >= intern->array->size) {
		zend_throw_exception(spl_ce_RuntimeException, "Index invalid or out of range", 0);
		return NULL;
	} else if (Z_ISUNDEF(intern->array->elements[index])) {
		return NULL;
	} else {
		return &intern->array->elements[index];
	}
}
/* }}} */

static zval *spl_fixedarray_object_read_dimension(zval *object, zval *offset, int type, zval *rv) /* {{{ */
{
	spl_fixedarray_object *intern;

	intern = Z_SPLFIXEDARRAY_P(object);

	if (intern->fptr_offset_get) {
		zval tmp;
		if (!offset) {
			ZVAL_UNDEF(&tmp);
			offset = &tmp;
		} else {
			SEPARATE_ARG_IF_REF(offset);
		}
		zend_call_method_with_1_params(object, intern->std.ce, &intern->fptr_offset_get, "offsetGet", rv, offset);
		zval_ptr_dtor(offset);
		if (!Z_ISUNDEF_P(rv)) {
			return rv;
		}
		return &EG(uninitialized_zval);
	}

	return spl_fixedarray_object_read_dimension_helper(intern, offset);
}
/* }}} */

static inline void spl_fixedarray_object_write_dimension_helper(spl_fixedarray_object *intern, zval *offset, zval *value) /* {{{ */
{
	zend_long index;

	if (!offset) {
		/* '$array[] = value' syntax is not supported */
		zend_throw_exception(spl_ce_RuntimeException, "Index invalid or out of range", 0);
		return;
	}

	if (Z_TYPE_P(offset) != IS_LONG) {
		index = spl_offset_convert_to_long(offset);
	} else {
		index = Z_LVAL_P(offset);
	}

	if (index < 0 || intern->array == NULL || index >= intern->array->size) {
		zend_throw_exception(spl_ce_RuntimeException, "Index invalid or out of range", 0);
		return;
	} else {
		if (!Z_ISUNDEF(intern->array->elements[index])) {
			zval_ptr_dtor(&(intern->array->elements[index]));
		}
		ZVAL_DEREF(value);
		ZVAL_COPY(&intern->array->elements[index], value);
	}
}
/* }}} */

static void spl_fixedarray_object_write_dimension(zval *object, zval *offset, zval *value) /* {{{ */
{
	spl_fixedarray_object *intern;

	intern = Z_SPLFIXEDARRAY_P(object);

	if (intern->fptr_offset_set) {
		zval tmp;
		if (!offset) {
			ZVAL_NULL(&tmp);
			offset = &tmp;
		} else {
			SEPARATE_ARG_IF_REF(offset);
		}
		SEPARATE_ARG_IF_REF(value);
		zend_call_method_with_2_params(object, intern->std.ce, &intern->fptr_offset_set, "offsetSet", NULL, offset, value);
		zval_ptr_dtor(value);
		zval_ptr_dtor(offset);
		return;
	}

	spl_fixedarray_object_write_dimension_helper(intern, offset, value);
}
/* }}} */

static inline void spl_fixedarray_object_unset_dimension_helper(spl_fixedarray_object *intern, zval *offset) /* {{{ */
{
	zend_long index;

	if (Z_TYPE_P(offset) != IS_LONG) {
		index = spl_offset_convert_to_long(offset);
	} else {
		index = Z_LVAL_P(offset);
	}

	if (index < 0 || intern->array == NULL || index >= intern->array->size) {
		zend_throw_exception(spl_ce_RuntimeException, "Index invalid or out of range", 0);
		return;
	} else {
		zval_ptr_dtor(&(intern->array->elements[index]));
		ZVAL_UNDEF(&intern->array->elements[index]);
	}
}
/* }}} */

static void spl_fixedarray_object_unset_dimension(zval *object, zval *offset) /* {{{ */
{
	spl_fixedarray_object *intern;

	intern = Z_SPLFIXEDARRAY_P(object);

	if (intern->fptr_offset_del) {
		SEPARATE_ARG_IF_REF(offset);
		zend_call_method_with_1_params(object, intern->std.ce, &intern->fptr_offset_del, "offsetUnset", NULL, offset);
		zval_ptr_dtor(offset);
		return;
	}

	spl_fixedarray_object_unset_dimension_helper(intern, offset);

}
/* }}} */

static inline int spl_fixedarray_object_has_dimension_helper(spl_fixedarray_object *intern, zval *offset, int check_empty) /* {{{ */
{
	zend_long index;
	int retval;

	if (Z_TYPE_P(offset) != IS_LONG) {
		index = spl_offset_convert_to_long(offset);
	} else {
		index = Z_LVAL_P(offset);
	}

	if (index < 0 || intern->array == NULL || index >= intern->array->size) {
		retval = 0;
	} else {
		if (Z_ISUNDEF(intern->array->elements[index])) {
			retval = 0;
		} else if (check_empty) {
			if (zend_is_true(&intern->array->elements[index])) {
				retval = 1;
			} else {
				retval = 0;
			}
		} else { /* != NULL and !check_empty */
			retval = 1;
		}
	}

	return retval;
}
/* }}} */

static int spl_fixedarray_object_has_dimension(zval *object, zval *offset, int check_empty) /* {{{ */
{
	spl_fixedarray_object *intern;

	intern = Z_SPLFIXEDARRAY_P(object);

	if (intern->fptr_offset_get) {
		zval rv;
		SEPARATE_ARG_IF_REF(offset);
		zend_call_method_with_1_params(object, intern->std.ce, &intern->fptr_offset_has, "offsetExists", &rv, offset);
		zval_ptr_dtor(offset);
		if (!Z_ISUNDEF(rv)) {
			zend_bool result = zend_is_true(&rv);
			zval_ptr_dtor(&rv);
			return result;
		}
		return 0;
	}

	return spl_fixedarray_object_has_dimension_helper(intern, offset, check_empty);
}
/* }}} */

static int spl_fixedarray_object_count_elements(zval *object, zend_long *count) /* {{{ */
{
	spl_fixedarray_object *intern;

	intern = Z_SPLFIXEDARRAY_P(object);
	if (intern->fptr_count) {
		zval rv;
		zend_call_method_with_0_params(object, intern->std.ce, &intern->fptr_count, "count", &rv);
		if (!Z_ISUNDEF(rv)) {
			*count = zval_get_long(&rv);
			zval_ptr_dtor(&rv);
			return SUCCESS;
		}
	} else if (intern->array) {
		*count = intern->array->size;
		return SUCCESS;
	}

	*count = 0;
	return SUCCESS;
}
/* }}} */

/* {{{ proto void SplFixedArray::__construct([int size])
*/
SPL_METHOD(SplFixedArray, __construct)
{
	zval *object = getThis();
	spl_fixedarray_object *intern;
	zend_long size = 0;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "|l", &size) == FAILURE) {
		return;
	}

	if (size < 0) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "array size cannot be less than zero");
		return;
	}

	intern = Z_SPLFIXEDARRAY_P(object);

	if (intern->array) {
		/* called __construct() twice, bail out */
		return;
	}

	intern->array = emalloc(sizeof(spl_fixedarray));
	spl_fixedarray_init(intern->array, size);
}
/* }}} */

/* {{{ proto void SplFixedArray::__wakeup()
*/
SPL_METHOD(SplFixedArray, __wakeup)
{
	spl_fixedarray_object *intern = Z_SPLFIXEDARRAY_P(getThis());
	HashTable *intern_ht = zend_std_get_properties(getThis());
	zval *data;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (!intern->array) {
		int index = 0;
		int size = zend_hash_num_elements(intern_ht);

		intern->array = emalloc(sizeof(spl_fixedarray));
		spl_fixedarray_init(intern->array, size);

		ZEND_HASH_FOREACH_VAL(intern_ht, data) {
			if (Z_REFCOUNTED_P(data)) {
				Z_ADDREF_P(data);
			}
			ZVAL_COPY_VALUE(&intern->array->elements[index], data);
			index++;
		} ZEND_HASH_FOREACH_END();

		/* Remove the unserialised properties, since we now have the elements
		 * within the spl_fixedarray_object structure. */
		zend_hash_clean(intern_ht);
	}
}
/* }}} */

/* {{{ proto int SplFixedArray::count(void)
*/
SPL_METHOD(SplFixedArray, count)
{
	zval *object = getThis();
	spl_fixedarray_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern = Z_SPLFIXEDARRAY_P(object);
	if (intern->array) {
		RETURN_LONG(intern->array->size);
	}
	RETURN_LONG(0);
}
/* }}} */

/* {{{ proto object SplFixedArray::toArray()
*/
SPL_METHOD(SplFixedArray, toArray)
{
	spl_fixedarray_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern = Z_SPLFIXEDARRAY_P(getThis());

	array_init(return_value);
	if (intern->array) {
		int i = 0;
		for (; i < intern->array->size; i++) {
			if (!Z_ISUNDEF(intern->array->elements[i])) {
				zend_hash_index_update(Z_ARRVAL_P(return_value), i, &intern->array->elements[i]);
				if (Z_REFCOUNTED(intern->array->elements[i])) {
					Z_ADDREF(intern->array->elements[i]);
				}
			} else {
				zend_hash_index_update(Z_ARRVAL_P(return_value), i, &EG(uninitialized_zval));
			}
		}
	}
}
/* }}} */

/* {{{ proto object SplFixedArray::fromArray(array data[, bool save_indexes])
*/
SPL_METHOD(SplFixedArray, fromArray)
{
	zval *data;
	spl_fixedarray *array;
	spl_fixedarray_object *intern;
	int num;
	zend_bool save_indexes = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "a|b", &data, &save_indexes) == FAILURE) {
		return;
	}

	array = ecalloc(1, sizeof(spl_fixedarray));
	num = zend_hash_num_elements(Z_ARRVAL_P(data));

	if (num > 0 && save_indexes) {
		zval *element;
		zend_string *str_index;
		zend_ulong num_index, max_index = 0;
		zend_long tmp;

		ZEND_HASH_FOREACH_KEY(Z_ARRVAL_P(data), num_index, str_index) {
			if (str_index != NULL || (zend_long)num_index < 0) {
				efree(array);
				zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "array must contain only positive integer keys");
				return;
			}

			if (num_index > max_index) {
				max_index = num_index;
			}
		} ZEND_HASH_FOREACH_END();

		tmp = max_index + 1;
		if (tmp <= 0) {
			efree(array);
			zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "integer overflow detected");
			return;
		}
		spl_fixedarray_init(array, tmp);

		ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(data), num_index, str_index, element) {
			ZVAL_DEREF(element);
			ZVAL_COPY(&array->elements[num_index], element);
		} ZEND_HASH_FOREACH_END();

	} else if (num > 0 && !save_indexes) {
		zval *element;
		zend_long i = 0;

		spl_fixedarray_init(array, num);

		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(data), element) {
			ZVAL_DEREF(element);
			ZVAL_COPY(&array->elements[i], element);
			i++;
		} ZEND_HASH_FOREACH_END();
	} else {
		spl_fixedarray_init(array, 0);
	}

	object_init_ex(return_value, spl_ce_SplFixedArray);

	intern = Z_SPLFIXEDARRAY_P(return_value);
	intern->array = array;
}
/* }}} */

/* {{{ proto int SplFixedArray::getSize(void)
*/
SPL_METHOD(SplFixedArray, getSize)
{
	zval *object = getThis();
	spl_fixedarray_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern = Z_SPLFIXEDARRAY_P(object);
	if (intern->array) {
		RETURN_LONG(intern->array->size);
	}
	RETURN_LONG(0);
}
/* }}} */

/* {{{ proto bool SplFixedArray::setSize(int size)
*/
SPL_METHOD(SplFixedArray, setSize)
{
	zval *object = getThis();
	spl_fixedarray_object *intern;
	zend_long size;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &size) == FAILURE) {
		return;
	}

	if (size < 0) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "array size cannot be less than zero");
		return;
	}

	intern = Z_SPLFIXEDARRAY_P(object);
	if (!intern->array) {
		intern->array = ecalloc(1, sizeof(spl_fixedarray));
	}

	spl_fixedarray_resize(intern->array, size);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool SplFixedArray::offsetExists(mixed $index)
 Returns whether the requested $index exists. */
SPL_METHOD(SplFixedArray, offsetExists)
{
	zval                  *zindex;
	spl_fixedarray_object  *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &zindex) == FAILURE) {
		return;
	}

	intern = Z_SPLFIXEDARRAY_P(getThis());

	RETURN_BOOL(spl_fixedarray_object_has_dimension_helper(intern, zindex, 0));
} /* }}} */

/* {{{ proto mixed SplFixedArray::offsetGet(mixed $index)
 Returns the value at the specified $index. */
SPL_METHOD(SplFixedArray, offsetGet)
{
	zval *zindex, *value;
	spl_fixedarray_object  *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &zindex) == FAILURE) {
		return;
	}

	intern = Z_SPLFIXEDARRAY_P(getThis());
	value = spl_fixedarray_object_read_dimension_helper(intern, zindex);

	if (value) {
		ZVAL_DEREF(value);
		ZVAL_COPY(return_value, value);
	} else {
		RETURN_NULL();
	}
} /* }}} */

/* {{{ proto void SplFixedArray::offsetSet(mixed $index, mixed $newval)
 Sets the value at the specified $index to $newval. */
SPL_METHOD(SplFixedArray, offsetSet)
{
	zval                  *zindex, *value;
	spl_fixedarray_object  *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &zindex, &value) == FAILURE) {
		return;
	}

	intern = Z_SPLFIXEDARRAY_P(getThis());
	spl_fixedarray_object_write_dimension_helper(intern, zindex, value);

} /* }}} */

/* {{{ proto void SplFixedArray::offsetUnset(mixed $index)
 Unsets the value at the specified $index. */
SPL_METHOD(SplFixedArray, offsetUnset)
{
	zval                  *zindex;
	spl_fixedarray_object  *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &zindex) == FAILURE) {
		return;
	}

	intern = Z_SPLFIXEDARRAY_P(getThis());
	spl_fixedarray_object_unset_dimension_helper(intern, zindex);

} /* }}} */

static void spl_fixedarray_it_dtor(zend_object_iterator *iter) /* {{{ */
{
	spl_fixedarray_it  *iterator = (spl_fixedarray_it *)iter;

	zend_user_it_invalidate_current(iter);
	zval_ptr_dtor(&iterator->intern.it.data);
}
/* }}} */

static void spl_fixedarray_it_rewind(zend_object_iterator *iter) /* {{{ */
{
	spl_fixedarray_object *object = Z_SPLFIXEDARRAY_P(&iter->data);

	if (object->flags & SPL_FIXEDARRAY_OVERLOADED_REWIND) {
		zend_user_it_rewind(iter);
	} else {
		object->current = 0;
	}
}
/* }}} */

static int spl_fixedarray_it_valid(zend_object_iterator *iter) /* {{{ */
{
	spl_fixedarray_object *object = Z_SPLFIXEDARRAY_P(&iter->data);

	if (object->flags & SPL_FIXEDARRAY_OVERLOADED_VALID) {
		return zend_user_it_valid(iter);
	}

	if (object->current >= 0 && object->array && object->current < object->array->size) {
		return SUCCESS;
	}

	return FAILURE;
}
/* }}} */

static zval *spl_fixedarray_it_get_current_data(zend_object_iterator *iter) /* {{{ */
{
	zval zindex;
	spl_fixedarray_object *object = Z_SPLFIXEDARRAY_P(&iter->data);

	if (object->flags & SPL_FIXEDARRAY_OVERLOADED_CURRENT) {
		return zend_user_it_get_current_data(iter);
	} else {
		zval *data;

		ZVAL_LONG(&zindex, object->current);

		data = spl_fixedarray_object_read_dimension_helper(object, &zindex);
		zval_ptr_dtor(&zindex);

		if (data == NULL) {
			data = &EG(uninitialized_zval);
		}
		return data;
	}
}
/* }}} */

static void spl_fixedarray_it_get_current_key(zend_object_iterator *iter, zval *key) /* {{{ */
{
	spl_fixedarray_object *object = Z_SPLFIXEDARRAY_P(&iter->data);

	if (object->flags & SPL_FIXEDARRAY_OVERLOADED_KEY) {
		zend_user_it_get_current_key(iter, key);
	} else {
		ZVAL_LONG(key, object->current);
	}
}
/* }}} */

static void spl_fixedarray_it_move_forward(zend_object_iterator *iter) /* {{{ */
{
	spl_fixedarray_object *object = Z_SPLFIXEDARRAY_P(&iter->data);

	if (object->flags & SPL_FIXEDARRAY_OVERLOADED_NEXT) {
		zend_user_it_move_forward(iter);
	} else {
		zend_user_it_invalidate_current(iter);
		object->current++;
	}
}
/* }}} */

/* {{{  proto int SplFixedArray::key()
   Return current array key */
SPL_METHOD(SplFixedArray, key)
{
	spl_fixedarray_object *intern = Z_SPLFIXEDARRAY_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(intern->current);
}
/* }}} */

/* {{{ proto void SplFixedArray::next()
   Move to next entry */
SPL_METHOD(SplFixedArray, next)
{
	spl_fixedarray_object *intern = Z_SPLFIXEDARRAY_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern->current++;
}
/* }}} */

/* {{{ proto bool SplFixedArray::valid()
   Check whether the datastructure contains more entries */
SPL_METHOD(SplFixedArray, valid)
{
	spl_fixedarray_object *intern = Z_SPLFIXEDARRAY_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(intern->current >= 0 && intern->array && intern->current < intern->array->size);
}
/* }}} */

/* {{{ proto void SplFixedArray::rewind()
   Rewind the datastructure back to the start */
SPL_METHOD(SplFixedArray, rewind)
{
	spl_fixedarray_object *intern = Z_SPLFIXEDARRAY_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern->current = 0;
}
/* }}} */

/* {{{ proto mixed|NULL SplFixedArray::current()
   Return current datastructure entry */
SPL_METHOD(SplFixedArray, current)
{
	zval zindex, *value;
	spl_fixedarray_object *intern  = Z_SPLFIXEDARRAY_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	ZVAL_LONG(&zindex, intern->current);

	value = spl_fixedarray_object_read_dimension_helper(intern, &zindex);

	zval_ptr_dtor(&zindex);

	if (value) {
		ZVAL_DEREF(value);
		ZVAL_COPY(return_value, value);
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/* iterator handler table */
zend_object_iterator_funcs spl_fixedarray_it_funcs = {
	spl_fixedarray_it_dtor,
	spl_fixedarray_it_valid,
	spl_fixedarray_it_get_current_data,
	spl_fixedarray_it_get_current_key,
	spl_fixedarray_it_move_forward,
	spl_fixedarray_it_rewind
};

zend_object_iterator *spl_fixedarray_get_iterator(zend_class_entry *ce, zval *object, int by_ref) /* {{{ */
{
	spl_fixedarray_it *iterator;

	if (by_ref) {
		zend_throw_exception(spl_ce_RuntimeException, "An iterator cannot be used with foreach by reference", 0);
		return NULL;
	}

	iterator = emalloc(sizeof(spl_fixedarray_it));

	zend_iterator_init((zend_object_iterator*)iterator);

	ZVAL_COPY(&iterator->intern.it.data, object);
	iterator->intern.it.funcs = &spl_fixedarray_it_funcs;
	iterator->intern.ce = ce;
	ZVAL_UNDEF(&iterator->intern.value);

	return &iterator->intern.it;
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo_splfixedarray_construct, 0, 0, 0)
	ZEND_ARG_INFO(0, size)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fixedarray_offsetGet, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fixedarray_offsetSet, 0, 0, 2)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_INFO(0, newval)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fixedarray_setSize, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fixedarray_fromArray, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, save_indexes)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_splfixedarray_void, 0)
ZEND_END_ARG_INFO()

static zend_function_entry spl_funcs_SplFixedArray[] = { /* {{{ */
	SPL_ME(SplFixedArray, __construct,     arginfo_splfixedarray_construct,ZEND_ACC_PUBLIC)
	SPL_ME(SplFixedArray, __wakeup,        arginfo_splfixedarray_void,     ZEND_ACC_PUBLIC)
	SPL_ME(SplFixedArray, count,           arginfo_splfixedarray_void,     ZEND_ACC_PUBLIC)
	SPL_ME(SplFixedArray, toArray,         arginfo_splfixedarray_void,     ZEND_ACC_PUBLIC)
	SPL_ME(SplFixedArray, fromArray,       arginfo_fixedarray_fromArray,   ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	SPL_ME(SplFixedArray, getSize,         arginfo_splfixedarray_void,     ZEND_ACC_PUBLIC)
	SPL_ME(SplFixedArray, setSize,         arginfo_fixedarray_setSize,     ZEND_ACC_PUBLIC)
	SPL_ME(SplFixedArray, offsetExists,    arginfo_fixedarray_offsetGet,   ZEND_ACC_PUBLIC)
	SPL_ME(SplFixedArray, offsetGet,       arginfo_fixedarray_offsetGet,   ZEND_ACC_PUBLIC)
	SPL_ME(SplFixedArray, offsetSet,       arginfo_fixedarray_offsetSet,   ZEND_ACC_PUBLIC)
	SPL_ME(SplFixedArray, offsetUnset,     arginfo_fixedarray_offsetGet,   ZEND_ACC_PUBLIC)
	SPL_ME(SplFixedArray, rewind,          arginfo_splfixedarray_void,     ZEND_ACC_PUBLIC)
	SPL_ME(SplFixedArray, current,         arginfo_splfixedarray_void,     ZEND_ACC_PUBLIC)
	SPL_ME(SplFixedArray, key,             arginfo_splfixedarray_void,     ZEND_ACC_PUBLIC)
	SPL_ME(SplFixedArray, next,            arginfo_splfixedarray_void,     ZEND_ACC_PUBLIC)
	SPL_ME(SplFixedArray, valid,           arginfo_splfixedarray_void,     ZEND_ACC_PUBLIC)
	PHP_FE_END
};
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(spl_fixedarray)
{
	REGISTER_SPL_STD_CLASS_EX(SplFixedArray, spl_fixedarray_new, spl_funcs_SplFixedArray);
	memcpy(&spl_handler_SplFixedArray, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

	spl_handler_SplFixedArray.offset          = XtOffsetOf(spl_fixedarray_object, std);
	spl_handler_SplFixedArray.clone_obj       = spl_fixedarray_object_clone;
	spl_handler_SplFixedArray.read_dimension  = spl_fixedarray_object_read_dimension;
	spl_handler_SplFixedArray.write_dimension = spl_fixedarray_object_write_dimension;
	spl_handler_SplFixedArray.unset_dimension = spl_fixedarray_object_unset_dimension;
	spl_handler_SplFixedArray.has_dimension   = spl_fixedarray_object_has_dimension;
	spl_handler_SplFixedArray.count_elements  = spl_fixedarray_object_count_elements;
	spl_handler_SplFixedArray.get_properties  = spl_fixedarray_object_get_properties;
	spl_handler_SplFixedArray.get_gc          = spl_fixedarray_object_get_gc;
	spl_handler_SplFixedArray.dtor_obj        = zend_objects_destroy_object;
	spl_handler_SplFixedArray.free_obj        = spl_fixedarray_object_free_storage;

	REGISTER_SPL_IMPLEMENTS(SplFixedArray, Iterator);
	REGISTER_SPL_IMPLEMENTS(SplFixedArray, ArrayAccess);
	REGISTER_SPL_IMPLEMENTS(SplFixedArray, Countable);

	spl_ce_SplFixedArray->get_iterator = spl_fixedarray_get_iterator;

	return SUCCESS;
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
