/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
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
	long size;
	zval **elements;
} spl_fixedarray;
/* }}} */

typedef struct _spl_fixedarray_object { /* {{{ */
	zend_object            std;
	spl_fixedarray         *array;
	zval                  *retval;
	zend_function         *fptr_offset_get;
	zend_function         *fptr_offset_set;
	zend_function         *fptr_offset_has;
	zend_function         *fptr_offset_del;
	zend_function         *fptr_count;
	int                    current;
	int                    flags;
	zend_class_entry      *ce_get_iterator;
} spl_fixedarray_object;
/* }}} */

typedef struct _spl_fixedarray_it { /* {{{ */
	zend_user_iterator     intern;
	spl_fixedarray_object  *object;
} spl_fixedarray_it;
/* }}} */

#define SPL_FIXEDARRAY_OVERLOADED_REWIND  0x0001
#define SPL_FIXEDARRAY_OVERLOADED_VALID   0x0002
#define SPL_FIXEDARRAY_OVERLOADED_KEY     0x0004
#define SPL_FIXEDARRAY_OVERLOADED_CURRENT 0x0008
#define SPL_FIXEDARRAY_OVERLOADED_NEXT    0x0010

static void spl_fixedarray_init(spl_fixedarray *array, long size TSRMLS_DC) /* {{{ */
{
	if (size > 0) {
		array->size = 0; /* reset size in case ecalloc() fails */
		array->elements = ecalloc(size, sizeof(zval *));
		array->size = size;
	} else {
		array->elements = NULL;
		array->size = 0;
	}
}
/* }}} */

static void spl_fixedarray_resize(spl_fixedarray *array, long size TSRMLS_DC) /* {{{ */
{
	if (size == array->size) {
		/* nothing to do */
		return;
	}

	/* first initialization */
	if (array->size == 0) {
		spl_fixedarray_init(array, size TSRMLS_CC);
		return;
	}

	/* clearing the array */
	if (size == 0) {
		long i;

		for (i = 0; i < array->size; i++) {
			if (array->elements[i]) {
				zval_ptr_dtor(&(array->elements[i]));
			}
		}

		if (array->elements) {
			efree(array->elements);
			array->elements = NULL;
		}
	} else if (size > array->size) {
		array->elements = erealloc(array->elements, sizeof(zval *) * size);
		memset(array->elements + array->size, '\0', sizeof(zval *) * (size - array->size));
	} else { /* size < array->size */
		long i;

		for (i = size; i < array->size; i++) {
			if (array->elements[i]) {
				zval_ptr_dtor(&(array->elements[i]));
			}
		}
		array->elements = erealloc(array->elements, sizeof(zval *) * size);
	}

	array->size = size;
}
/* }}} */

static void spl_fixedarray_copy(spl_fixedarray *to, spl_fixedarray *from TSRMLS_DC) /* {{{ */
{
	int i;
	for (i = 0; i < from->size; i++) {
		if (from->elements[i]) {
			Z_ADDREF_P(from->elements[i]);
			to->elements[i] = from->elements[i];
		} else {
			to->elements[i] = NULL;
		}
	}
}
/* }}} */

static HashTable* spl_fixedarray_object_get_properties(zval *obj TSRMLS_DC) /* {{{{ */
{
	spl_fixedarray_object *intern  = (spl_fixedarray_object*)zend_object_store_get_object(obj TSRMLS_CC);
	HashTable *ht = zend_std_get_properties(obj TSRMLS_CC);
	int  i = 0;

	if (intern->array && !GC_G(gc_active)) {
		int j = zend_hash_num_elements(ht);

		for (i = 0; i < intern->array->size; i++) {
			if (intern->array->elements[i]) {
				zend_hash_index_update(ht, i, (void *)&intern->array->elements[i], sizeof(zval *), NULL);
				Z_ADDREF_P(intern->array->elements[i]);
			} else {
				zend_hash_index_update(ht, i, (void *)&EG(uninitialized_zval_ptr), sizeof(zval *), NULL);
				Z_ADDREF_P(EG(uninitialized_zval_ptr));
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

static void spl_fixedarray_object_free_storage(void *object TSRMLS_DC) /* {{{ */
{
	spl_fixedarray_object *intern = (spl_fixedarray_object *)object;
	long i;

	if (intern->array) {
		for (i = 0; i < intern->array->size; i++) {
			if (intern->array->elements[i]) {
				zval_ptr_dtor(&(intern->array->elements[i]));
			}
		}

		if (intern->array->size > 0 && intern->array->elements) {
			efree(intern->array->elements);
		}
		efree(intern->array);
	}

	zend_object_std_dtor(&intern->std TSRMLS_CC);
	zval_ptr_dtor(&intern->retval);

	efree(object);
}
/* }}} */

zend_object_iterator *spl_fixedarray_get_iterator(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC);

static zend_object_value spl_fixedarray_object_new_ex(zend_class_entry *class_type, spl_fixedarray_object **obj, zval *orig, int clone_orig TSRMLS_DC) /* {{{ */
{
	zend_object_value     retval;
	spl_fixedarray_object *intern;
	zend_class_entry     *parent = class_type;
	int                   inherited = 0;

	intern = ecalloc(1, sizeof(spl_fixedarray_object));
	*obj = intern;
	ALLOC_INIT_ZVAL(intern->retval);

	zend_object_std_init(&intern->std, class_type TSRMLS_CC);
	object_properties_init(&intern->std, class_type);

	intern->current = 0;
	intern->flags = 0;

	if (orig && clone_orig) {
		spl_fixedarray_object *other = (spl_fixedarray_object*)zend_object_store_get_object(orig TSRMLS_CC);
		intern->ce_get_iterator = other->ce_get_iterator;
		if (!other->array) {
			/* leave a empty object, will be dtor later by CLONE handler */
			zend_throw_exception(spl_ce_RuntimeException, "The instance wasn't initialized properly", 0 TSRMLS_CC);
		} else {
			intern->array = emalloc(sizeof(spl_fixedarray));
			spl_fixedarray_init(intern->array, other->array->size TSRMLS_CC);
			spl_fixedarray_copy(intern->array, other->array TSRMLS_CC);
		}
	}

	while (parent) {
		if (parent == spl_ce_SplFixedArray) {
			retval.handlers = &spl_handler_SplFixedArray;
			class_type->get_iterator = spl_fixedarray_get_iterator;
			break;
		}

		parent = parent->parent;
		inherited = 1;
	}

	retval.handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, spl_fixedarray_object_free_storage, NULL TSRMLS_CC);

	if (!parent) { /* this must never happen */
		php_error_docref(NULL TSRMLS_CC, E_COMPILE_ERROR, "Internal compiler error, Class is not child of SplFixedArray");
	}
	if (!class_type->iterator_funcs.zf_current) {
		zend_hash_find(&class_type->function_table, "rewind",  sizeof("rewind"),  (void **) &class_type->iterator_funcs.zf_rewind);
		zend_hash_find(&class_type->function_table, "valid",   sizeof("valid"),   (void **) &class_type->iterator_funcs.zf_valid);
		zend_hash_find(&class_type->function_table, "key",     sizeof("key"),     (void **) &class_type->iterator_funcs.zf_key);
		zend_hash_find(&class_type->function_table, "current", sizeof("current"), (void **) &class_type->iterator_funcs.zf_current);
		zend_hash_find(&class_type->function_table, "next",    sizeof("next"),    (void **) &class_type->iterator_funcs.zf_next);
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

		zend_hash_find(&class_type->function_table, "offsetget",    sizeof("offsetget"),    (void **) &intern->fptr_offset_get);
		if (intern->fptr_offset_get->common.scope == parent) {
			intern->fptr_offset_get = NULL;
		}
		zend_hash_find(&class_type->function_table, "offsetset",    sizeof("offsetset"),    (void **) &intern->fptr_offset_set);
		if (intern->fptr_offset_set->common.scope == parent) {
			intern->fptr_offset_set = NULL;
		}
		zend_hash_find(&class_type->function_table, "offsetexists", sizeof("offsetexists"), (void **) &intern->fptr_offset_has);
		if (intern->fptr_offset_has->common.scope == parent) {
			intern->fptr_offset_has = NULL;
		}
		zend_hash_find(&class_type->function_table, "offsetunset",  sizeof("offsetunset"),  (void **) &intern->fptr_offset_del);
		if (intern->fptr_offset_del->common.scope == parent) {
			intern->fptr_offset_del = NULL;
		}
		zend_hash_find(&class_type->function_table, "count",        sizeof("count"),        (void **) &intern->fptr_count);
		if (intern->fptr_count->common.scope == parent) {
			intern->fptr_count = NULL;
		}
	}

	return retval;
}
/* }}} */

static zend_object_value spl_fixedarray_new(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
	spl_fixedarray_object *tmp;
	return spl_fixedarray_object_new_ex(class_type, &tmp, NULL, 0 TSRMLS_CC);
}
/* }}} */

static zend_object_value spl_fixedarray_object_clone(zval *zobject TSRMLS_DC) /* {{{ */
{
	zend_object_value      new_obj_val;
	zend_object           *old_object;
	zend_object           *new_object;
	zend_object_handle     handle = Z_OBJ_HANDLE_P(zobject);
	spl_fixedarray_object  *intern;

	old_object  = zend_objects_get_address(zobject TSRMLS_CC);
	new_obj_val = spl_fixedarray_object_new_ex(old_object->ce, &intern, zobject, 1 TSRMLS_CC);
	new_object  = &intern->std;

	zend_objects_clone_members(new_object, new_obj_val, old_object, handle TSRMLS_CC);

	return new_obj_val;
}
/* }}} */

static inline zval **spl_fixedarray_object_read_dimension_helper(spl_fixedarray_object *intern, zval *offset TSRMLS_DC) /* {{{ */
{
	long index;

	/* we have to return NULL on error here to avoid memleak because of 
	 * ZE duplicating uninitialized_zval_ptr */
	if (!offset) {
		zend_throw_exception(spl_ce_RuntimeException, "Index invalid or out of range", 0 TSRMLS_CC);
		return NULL;
	}

	if (Z_TYPE_P(offset) != IS_LONG) {
		index = spl_offset_convert_to_long(offset TSRMLS_CC);
	} else {
		index = Z_LVAL_P(offset);
	}
	
	if (index < 0 || intern->array == NULL || index >= intern->array->size) {
		zend_throw_exception(spl_ce_RuntimeException, "Index invalid or out of range", 0 TSRMLS_CC);
		return NULL;
	} else if(!intern->array->elements[index]) {
		return NULL;
	} else {
		return &intern->array->elements[index];
	}
}
/* }}} */

static zval *spl_fixedarray_object_read_dimension(zval *object, zval *offset, int type TSRMLS_DC) /* {{{ */
{
	spl_fixedarray_object *intern;
	zval **retval;

	intern = (spl_fixedarray_object *)zend_object_store_get_object(object TSRMLS_CC);

	if (intern->fptr_offset_get) {
		zval *rv;
		SEPARATE_ARG_IF_REF(offset);
		zend_call_method_with_1_params(&object, intern->std.ce, &intern->fptr_offset_get, "offsetGet", &rv, offset);
		zval_ptr_dtor(&offset);
		if (rv) {
			zval_ptr_dtor(&intern->retval);
			MAKE_STD_ZVAL(intern->retval);
			ZVAL_ZVAL(intern->retval, rv, 1, 1);
			return intern->retval;
		}
		return EG(uninitialized_zval_ptr);
	}

	retval = spl_fixedarray_object_read_dimension_helper(intern, offset TSRMLS_CC);
	if (retval) {
		return *retval;
	}
	return NULL;
}
/* }}} */

static inline void spl_fixedarray_object_write_dimension_helper(spl_fixedarray_object *intern, zval *offset, zval *value TSRMLS_DC) /* {{{ */
{
	long index;

	if (!offset) {
		/* '$array[] = value' syntax is not supported */
		zend_throw_exception(spl_ce_RuntimeException, "Index invalid or out of range", 0 TSRMLS_CC);
		return;
	}

	if (Z_TYPE_P(offset) != IS_LONG) {
		index = spl_offset_convert_to_long(offset TSRMLS_CC);
	} else {
		index = Z_LVAL_P(offset);
	}

	if (index < 0 || intern->array == NULL || index >= intern->array->size) {
		zend_throw_exception(spl_ce_RuntimeException, "Index invalid or out of range", 0 TSRMLS_CC);
		return;
	} else {
		if (intern->array->elements[index]) {
			zval_ptr_dtor(&(intern->array->elements[index]));
		}
		SEPARATE_ARG_IF_REF(value);
		intern->array->elements[index] = value;
	}
}
/* }}} */

static void spl_fixedarray_object_write_dimension(zval *object, zval *offset, zval *value TSRMLS_DC) /* {{{ */
{
	spl_fixedarray_object *intern;

	intern = (spl_fixedarray_object *)zend_object_store_get_object(object TSRMLS_CC);

	if (intern->fptr_offset_set) {
		if (!offset) {
			ALLOC_INIT_ZVAL(offset);
		} else {
			SEPARATE_ARG_IF_REF(offset);
		}
		SEPARATE_ARG_IF_REF(value);
		zend_call_method_with_2_params(&object, intern->std.ce, &intern->fptr_offset_set, "offsetSet", NULL, offset, value);
		zval_ptr_dtor(&value);
		zval_ptr_dtor(&offset);
		return;
	}

	spl_fixedarray_object_write_dimension_helper(intern, offset, value TSRMLS_CC);
}
/* }}} */

static inline void spl_fixedarray_object_unset_dimension_helper(spl_fixedarray_object *intern, zval *offset TSRMLS_DC) /* {{{ */
{
	long index;
	
	if (Z_TYPE_P(offset) != IS_LONG) {
		index = spl_offset_convert_to_long(offset TSRMLS_CC);
	} else {
		index = Z_LVAL_P(offset);
	}
	
	if (index < 0 || intern->array == NULL || index >= intern->array->size) {
		zend_throw_exception(spl_ce_RuntimeException, "Index invalid or out of range", 0 TSRMLS_CC);
		return;
	} else {
		if (intern->array->elements[index]) {
			zval_ptr_dtor(&(intern->array->elements[index]));
		}
		intern->array->elements[index] = NULL;
	}
}
/* }}} */

static void spl_fixedarray_object_unset_dimension(zval *object, zval *offset TSRMLS_DC) /* {{{ */
{
	spl_fixedarray_object *intern;

	intern = (spl_fixedarray_object *)zend_object_store_get_object(object TSRMLS_CC);

	if (intern->fptr_offset_del) {
		SEPARATE_ARG_IF_REF(offset);
		zend_call_method_with_1_params(&object, intern->std.ce, &intern->fptr_offset_del, "offsetUnset", NULL, offset);
		zval_ptr_dtor(&offset);
		return;
	}

	spl_fixedarray_object_unset_dimension_helper(intern, offset TSRMLS_CC);

}
/* }}} */

static inline int spl_fixedarray_object_has_dimension_helper(spl_fixedarray_object *intern, zval *offset, int check_empty TSRMLS_DC) /* {{{ */
{
	long index;
	int retval;
	
	if (Z_TYPE_P(offset) != IS_LONG) {
		index = spl_offset_convert_to_long(offset TSRMLS_CC);
	} else {
		index = Z_LVAL_P(offset);
	}
	
	if (index < 0 || intern->array == NULL || index >= intern->array->size) {
		retval = 0;
	} else {
		if (!intern->array->elements[index]) {
			retval = 0;
		} else if (check_empty) {
			if (zend_is_true(intern->array->elements[index])) {
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

static int spl_fixedarray_object_has_dimension(zval *object, zval *offset, int check_empty TSRMLS_DC) /* {{{ */
{
	spl_fixedarray_object *intern;

	intern = (spl_fixedarray_object *)zend_object_store_get_object(object TSRMLS_CC);

	if (intern->fptr_offset_get) {
		zval *rv;
		SEPARATE_ARG_IF_REF(offset);
		zend_call_method_with_1_params(&object, intern->std.ce, &intern->fptr_offset_has, "offsetExists", &rv, offset);
		zval_ptr_dtor(&offset);
		if (rv) {
			zval_ptr_dtor(&intern->retval);
			MAKE_STD_ZVAL(intern->retval);
			ZVAL_ZVAL(intern->retval, rv, 1, 1);
			return zend_is_true(intern->retval);
		}
		return 0;
	}

	return spl_fixedarray_object_has_dimension_helper(intern, offset, check_empty TSRMLS_CC);
}
/* }}} */

static int spl_fixedarray_object_count_elements(zval *object, long *count TSRMLS_DC) /* {{{ */
{
	spl_fixedarray_object *intern;
	
	intern = (spl_fixedarray_object *)zend_object_store_get_object(object TSRMLS_CC);
	if (intern->fptr_count) {
		zval *rv;
		zend_call_method_with_0_params(&object, intern->std.ce, &intern->fptr_count, "count", &rv);
		if (rv) {
			zval_ptr_dtor(&intern->retval);
			MAKE_STD_ZVAL(intern->retval);
			ZVAL_ZVAL(intern->retval, rv, 1, 1);
			convert_to_long(intern->retval);
			*count = (long) Z_LVAL_P(intern->retval);
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
	long size = 0;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &size)) {
		return;
	}

	if (size < 0) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "array size cannot be less than zero");
		return;
	}

	intern = (spl_fixedarray_object *)zend_object_store_get_object(object TSRMLS_CC);

	if (intern->array) {
		/* called __construct() twice, bail out */
		return;
	}

	intern->array = emalloc(sizeof(spl_fixedarray));
	spl_fixedarray_init(intern->array, size TSRMLS_CC);
}
/* }}} */

/* {{{ proto int SplFixedArray::count(void)
*/
SPL_METHOD(SplFixedArray, count)
{
	zval *object = getThis();
	spl_fixedarray_object *intern;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "")) {
		return;
	}

	intern = (spl_fixedarray_object *)zend_object_store_get_object(object TSRMLS_CC);
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
	zval *ret, *tmp;
	HashTable *ret_ht, *obj_ht;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "")) {
		return;
	}

	ALLOC_HASHTABLE(ret_ht);
	zend_hash_init(ret_ht, 0, NULL, ZVAL_PTR_DTOR, 0);
	ALLOC_INIT_ZVAL(ret);
	Z_TYPE_P(ret) = IS_ARRAY;
	obj_ht = spl_fixedarray_object_get_properties(getThis() TSRMLS_CC);
	zend_hash_copy(ret_ht, obj_ht, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
	Z_ARRVAL_P(ret) = ret_ht;

	RETURN_ZVAL(ret, 1, 1);
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

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|b", &data, &save_indexes)) {
		return;
	}

	array = ecalloc(1, sizeof(*array));
	num = zend_hash_num_elements(Z_ARRVAL_P(data));
	
	if (num > 0 && save_indexes) {
		zval **element, *value;
		char *str_index;
		ulong num_index, max_index = 0;
		long tmp;

		for (zend_hash_internal_pointer_reset(Z_ARRVAL_P(data));
			zend_hash_get_current_data(Z_ARRVAL_P(data), (void **) &element) == SUCCESS;
			zend_hash_move_forward(Z_ARRVAL_P(data))
			) {
			if (zend_hash_get_current_key(Z_ARRVAL_P(data), &str_index, &num_index, 0) != HASH_KEY_IS_LONG || (long)num_index < 0) {
				efree(array);
				zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "array must contain only positive integer keys");
				return;
			}

			if (num_index > max_index) {
				max_index = num_index;
			}
		}

		tmp = max_index + 1;
		if (tmp <= 0) {
			efree(array);
			zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "integer overflow detected");
			return;
		}
		spl_fixedarray_init(array, tmp TSRMLS_CC);

		for (zend_hash_internal_pointer_reset(Z_ARRVAL_P(data));
			zend_hash_get_current_data(Z_ARRVAL_P(data), (void **) &element) == SUCCESS;
			zend_hash_move_forward(Z_ARRVAL_P(data))
			) {
			
			zend_hash_get_current_key(Z_ARRVAL_P(data), &str_index, &num_index, 0);
			value = *element;

			SEPARATE_ARG_IF_REF(value);
			array->elements[num_index] = value;
		}

	} else if (num > 0 && !save_indexes) {
		zval **element, *value;
		long i = 0;
		
		spl_fixedarray_init(array, num TSRMLS_CC);
		
		for (zend_hash_internal_pointer_reset(Z_ARRVAL_P(data));
			zend_hash_get_current_data(Z_ARRVAL_P(data), (void **) &element) == SUCCESS;
			zend_hash_move_forward(Z_ARRVAL_P(data))
			) {
			
			value = *element;

			SEPARATE_ARG_IF_REF(value);
			array->elements[i] = value;
			i++;
		}
	} else {
		spl_fixedarray_init(array, 0 TSRMLS_CC);
	}

	object_init_ex(return_value, spl_ce_SplFixedArray);
	Z_TYPE_P(return_value) = IS_OBJECT;

	intern = (spl_fixedarray_object *)zend_object_store_get_object(return_value TSRMLS_CC);
	intern->array = array;
}
/* }}} */

/* {{{ proto int SplFixedArray::getSize(void)
*/
SPL_METHOD(SplFixedArray, getSize)
{
	zval *object = getThis();
	spl_fixedarray_object *intern;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "")) {
		return;
	}

	intern = (spl_fixedarray_object *)zend_object_store_get_object(object TSRMLS_CC);
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
	long size;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &size)) {
		return;
	}

	if (size < 0) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "array size cannot be less than zero");
		return;
	}

	intern = (spl_fixedarray_object *)zend_object_store_get_object(object TSRMLS_CC);
	if (!intern->array) {
		intern->array = ecalloc(1, sizeof(spl_fixedarray));
	}

	spl_fixedarray_resize(intern->array, size TSRMLS_CC);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool SplFixedArray::offsetExists(mixed $index) U
 Returns whether the requested $index exists. */
SPL_METHOD(SplFixedArray, offsetExists)
{
	zval                  *zindex;
	spl_fixedarray_object  *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zindex) == FAILURE) {
		return;
	}

	intern = (spl_fixedarray_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_BOOL(spl_fixedarray_object_has_dimension_helper(intern, zindex, 0 TSRMLS_CC));
} /* }}} */

/* {{{ proto mixed SplFixedArray::offsetGet(mixed $index) U
 Returns the value at the specified $index. */
SPL_METHOD(SplFixedArray, offsetGet)
{
	zval                  *zindex, **value_pp;
	spl_fixedarray_object  *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zindex) == FAILURE) {
		return;
	}

	intern    = (spl_fixedarray_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
	value_pp  = spl_fixedarray_object_read_dimension_helper(intern, zindex TSRMLS_CC);

	if (value_pp) {
		RETURN_ZVAL(*value_pp, 1, 0);
	}
	RETURN_NULL();
} /* }}} */

/* {{{ proto void SplFixedArray::offsetSet(mixed $index, mixed $newval) U
 Sets the value at the specified $index to $newval. */
SPL_METHOD(SplFixedArray, offsetSet)
{
	zval                  *zindex, *value;
	spl_fixedarray_object  *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &zindex, &value) == FAILURE) {
		return;
	}

	intern = (spl_fixedarray_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
	spl_fixedarray_object_write_dimension_helper(intern, zindex, value TSRMLS_CC);

} /* }}} */

/* {{{ proto void SplFixedArray::offsetUnset(mixed $index) U
 Unsets the value at the specified $index. */
SPL_METHOD(SplFixedArray, offsetUnset)
{
	zval                  *zindex;
	spl_fixedarray_object  *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zindex) == FAILURE) {
		return;
	}

	intern = (spl_fixedarray_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
	spl_fixedarray_object_unset_dimension_helper(intern, zindex TSRMLS_CC);

} /* }}} */

static void spl_fixedarray_it_dtor(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_fixedarray_it  *iterator = (spl_fixedarray_it *)iter;

	zend_user_it_invalidate_current(iter TSRMLS_CC);
	zval_ptr_dtor((zval**)&iterator->intern.it.data);

	efree(iterator);
}
/* }}} */

static void spl_fixedarray_it_rewind(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_fixedarray_it     *iterator = (spl_fixedarray_it *)iter;
	spl_fixedarray_object *intern   = iterator->object;

	if (intern->flags & SPL_FIXEDARRAY_OVERLOADED_REWIND) {
		zend_user_it_rewind(iter TSRMLS_CC);
	} else {
		iterator->object->current = 0;
	}
}
/* }}} */

static int spl_fixedarray_it_valid(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_fixedarray_it     *iterator = (spl_fixedarray_it *)iter;
	spl_fixedarray_object *intern   = iterator->object;

	if (intern->flags & SPL_FIXEDARRAY_OVERLOADED_VALID) {
		return zend_user_it_valid(iter TSRMLS_CC);
	}

	if (iterator->object->current >= 0 && iterator->object->array && iterator->object->current < iterator->object->array->size) {
		return SUCCESS;
	}

	return FAILURE;
}
/* }}} */

static void spl_fixedarray_it_get_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC) /* {{{ */
{
	zval                  *zindex;
	spl_fixedarray_it     *iterator = (spl_fixedarray_it *)iter;
	spl_fixedarray_object *intern   = iterator->object;

	if (intern->flags & SPL_FIXEDARRAY_OVERLOADED_CURRENT) {
		zend_user_it_get_current_data(iter, data TSRMLS_CC);
	} else {
		ALLOC_INIT_ZVAL(zindex);
		ZVAL_LONG(zindex, iterator->object->current);

		*data = spl_fixedarray_object_read_dimension_helper(intern, zindex TSRMLS_CC);

		if (*data == NULL) {
			*data = &EG(uninitialized_zval_ptr);
		}

		zval_ptr_dtor(&zindex);
	}
}
/* }}} */

static int spl_fixedarray_it_get_current_key(zend_object_iterator *iter, char **str_key, uint *str_key_len, ulong *int_key TSRMLS_DC) /* {{{ */
{
	spl_fixedarray_it     *iterator = (spl_fixedarray_it *)iter;
	spl_fixedarray_object *intern   = iterator->object;

	if (intern->flags & SPL_FIXEDARRAY_OVERLOADED_KEY) {
		return zend_user_it_get_current_key(iter, str_key, str_key_len, int_key TSRMLS_CC);
	} else {
		*int_key = (ulong) iterator->object->current;
		return HASH_KEY_IS_LONG;
	}

}
/* }}} */

static void spl_fixedarray_it_move_forward(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_fixedarray_it     *iterator = (spl_fixedarray_it *)iter;
	spl_fixedarray_object *intern   = iterator->object;

	if (intern->flags & SPL_FIXEDARRAY_OVERLOADED_NEXT) {
		zend_user_it_move_forward(iter TSRMLS_CC);
	} else {
		zend_user_it_invalidate_current(iter TSRMLS_CC);
		iterator->object->current++;
	}
}
/* }}} */

/* {{{  proto int SplFixedArray::key() U
   Return current array key */
SPL_METHOD(SplFixedArray, key)
{
	spl_fixedarray_object *intern = (spl_fixedarray_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(intern->current);
}
/* }}} */

/* {{{ proto void SplFixedArray::next() U
   Move to next entry */
SPL_METHOD(SplFixedArray, next)
{
	spl_fixedarray_object *intern = (spl_fixedarray_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern->current++;
}
/* }}} */

/* {{{ proto bool SplFixedArray::valid() U
   Check whether the datastructure contains more entries */
SPL_METHOD(SplFixedArray, valid)
{
	spl_fixedarray_object *intern = (spl_fixedarray_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(intern->current >= 0 && intern->array && intern->current < intern->array->size);
}
/* }}} */

/* {{{ proto void SplFixedArray::rewind() U
   Rewind the datastructure back to the start */
SPL_METHOD(SplFixedArray, rewind)
{
	spl_fixedarray_object *intern = (spl_fixedarray_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern->current = 0;
}
/* }}} */

/* {{{ proto mixed|NULL SplFixedArray::current() U
   Return current datastructure entry */
SPL_METHOD(SplFixedArray, current)
{
	zval                 *zindex, **value_pp;
	spl_fixedarray_object *intern  = (spl_fixedarray_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	ALLOC_INIT_ZVAL(zindex);
	ZVAL_LONG(zindex, intern->current);

	value_pp  = spl_fixedarray_object_read_dimension_helper(intern, zindex TSRMLS_CC);

	zval_ptr_dtor(&zindex);

	if (value_pp) {
		RETURN_ZVAL(*value_pp, 1, 0);
	}
	RETURN_NULL();
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

zend_object_iterator *spl_fixedarray_get_iterator(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC) /* {{{ */
{
	spl_fixedarray_it      *iterator;
	spl_fixedarray_object  *fixedarray_object = (spl_fixedarray_object*)zend_object_store_get_object(object TSRMLS_CC);

	if (by_ref) {
		zend_throw_exception(spl_ce_RuntimeException, "An iterator cannot be used with foreach by reference", 0 TSRMLS_CC);
		return NULL;
	}

	Z_ADDREF_P(object);

	iterator                     = emalloc(sizeof(spl_fixedarray_it));
	iterator->intern.it.data     = (void*)object;
	iterator->intern.it.funcs    = &spl_fixedarray_it_funcs;
	iterator->intern.ce          = ce;
	iterator->intern.value       = NULL;
	iterator->object             = fixedarray_object;

	return (zend_object_iterator*)iterator;
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

	spl_handler_SplFixedArray.clone_obj       = spl_fixedarray_object_clone;
	spl_handler_SplFixedArray.read_dimension  = spl_fixedarray_object_read_dimension;
	spl_handler_SplFixedArray.write_dimension = spl_fixedarray_object_write_dimension;
	spl_handler_SplFixedArray.unset_dimension = spl_fixedarray_object_unset_dimension;
	spl_handler_SplFixedArray.has_dimension   = spl_fixedarray_object_has_dimension;
	spl_handler_SplFixedArray.count_elements  = spl_fixedarray_object_count_elements;
	spl_handler_SplFixedArray.get_properties  = spl_fixedarray_object_get_properties;

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
