/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2008 The PHP Group                                |
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
#include "spl_fastarray.h"
#include "spl_exceptions.h"
#include "spl_iterators.h"

zend_object_handlers spl_handler_SplFastArray;
PHPAPI zend_class_entry *spl_ce_SplFastArray;

#ifdef COMPILE_DL_SPL_FASTARRAY
ZEND_GET_MODULE(spl_fastarray)
#endif

typedef struct _spl_fastarray { /* {{{ */
	long size;
	zval **elements;
} spl_fastarray;
/* }}} */

typedef struct _spl_fastarray_object { /* {{{ */
	zend_object            std;
	spl_fastarray         *array;
	zval                  *retval;
	zend_function         *fptr_offset_get;
	zend_function         *fptr_offset_set;
	zend_function         *fptr_offset_has;
	zend_function         *fptr_offset_del;
	zend_function         *fptr_it_next;
	zend_function         *fptr_it_rewind;
	zend_function         *fptr_it_current;
	zend_function         *fptr_it_key;
	zend_function         *fptr_it_valid;
	int                    current;
	zend_class_entry      *ce_get_iterator;
} spl_fastarray_object;
/* }}} */

typedef struct _spl_fastarray_it { /* {{{ */
	zend_user_iterator     intern;
	spl_fastarray_object  *object;
} spl_fastarray_it;
/* }}} */

static void spl_fastarray_init(spl_fastarray *array, long size TSRMLS_DC) /* {{{ */
{
	if (size > 0) {
		array->elements = ecalloc(size, sizeof(zval *));
		array->size = size;
	} else {
		array->elements = NULL;
		array->size = 0;
	}
}
/* }}} */

static void spl_fastarray_resize(spl_fastarray *array, long size TSRMLS_DC) /* {{{ */
{
	if (size == array->size) {
		/* nothing to do */
		return;
	}

	/* first initialization */
	if (array->size == 0) {
		spl_fastarray_init(array, size TSRMLS_CC);
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

static void spl_fastarray_copy(spl_fastarray *to, spl_fastarray *from TSRMLS_DC) /* {{{ */
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

static void spl_fastarray_object_free_storage(void *object TSRMLS_DC) /* {{{ */
{
	spl_fastarray_object *intern = (spl_fastarray_object *)object;
	long i;

	if (intern->array) {
		for (i = 0; i < intern->array->size; i++) {
			if (intern->array->elements[i]) {
				zval_ptr_dtor(&(intern->array->elements[i]));
			}
		}

		if (intern->array->elements) {
			efree(intern->array->elements);
		}
		efree(intern->array);
	}

	zend_object_std_dtor(&intern->std TSRMLS_CC);
	zval_ptr_dtor(&intern->retval);

	efree(object);
}
/* }}} */

zend_object_iterator *spl_fastarray_get_iterator(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC);

static zend_object_value spl_fastarray_object_new_ex(zend_class_entry *class_type, spl_fastarray_object **obj, zval *orig, int clone_orig TSRMLS_DC) /* {{{ */
{
	zend_object_value     retval;
	spl_fastarray_object *intern;
	zval                 *tmp;
	zend_class_entry     *parent = class_type;
	int                   inherited = 0;

	intern = ecalloc(1, sizeof(spl_fastarray_object));
	*obj = intern;
	ALLOC_INIT_ZVAL(intern->retval);

	zend_object_std_init(&intern->std, class_type TSRMLS_CC);
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	intern->current = 0;

	if (orig && clone_orig) {
		spl_fastarray_object *other = (spl_fastarray_object*)zend_object_store_get_object(orig TSRMLS_CC);
		intern->ce_get_iterator = other->ce_get_iterator;

		intern->array = emalloc(sizeof(spl_fastarray));
		spl_fastarray_init(intern->array, other->array->size TSRMLS_CC);
		spl_fastarray_copy(intern->array, other->array TSRMLS_CC);
	}

	while (parent) {
		if (parent == spl_ce_SplFastArray) {
			retval.handlers = &spl_handler_SplFastArray;
			break;
		}

		parent = parent->parent;
		inherited = 1;
	}

	retval.handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, spl_fastarray_object_free_storage, NULL TSRMLS_CC);

	if (!parent) { /* this must never happen */
		php_error_docref(NULL TSRMLS_CC, E_COMPILE_ERROR, "Internal compiler error, Class is not child of SplFastArray");
	}
	if (inherited) {
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
		zend_hash_find(&class_type->function_table, "next",         sizeof("next"),         (void **) &intern->fptr_it_next);
		if (intern->fptr_it_next->common.scope == parent) {
			intern->fptr_it_next = NULL;
		}
		zend_hash_find(&class_type->function_table, "rewind",       sizeof("rewind"),       (void **) &intern->fptr_it_rewind);
		if (intern->fptr_it_rewind->common.scope == parent) {
			intern->fptr_it_rewind = NULL;
		}
		zend_hash_find(&class_type->function_table, "current",      sizeof("current"),      (void **) &intern->fptr_it_current);
		if (intern->fptr_it_current->common.scope == parent) {
			intern->fptr_it_current = NULL;
		}
		zend_hash_find(&class_type->function_table, "key",          sizeof("key"),          (void **) &intern->fptr_it_key);
		if (intern->fptr_it_key->common.scope == parent) {
			intern->fptr_it_key = NULL;
		}
		zend_hash_find(&class_type->function_table, "valid",        sizeof("valid"),        (void **) &intern->fptr_it_valid);
		if (intern->fptr_it_valid->common.scope == parent) {
			intern->fptr_it_valid = NULL;
		}
	}

	return retval;
}
/* }}} */

static zend_object_value spl_fastarray_new(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
	spl_fastarray_object *tmp;
	return spl_fastarray_object_new_ex(class_type, &tmp, NULL, 0 TSRMLS_CC);
}
/* }}} */

static zend_object_value spl_fastarray_object_clone(zval *zobject TSRMLS_DC) /* {{{ */
{
	zend_object_value      new_obj_val;
	zend_object           *old_object;
	zend_object           *new_object;
	zend_object_handle     handle = Z_OBJ_HANDLE_P(zobject);
	spl_fastarray_object  *intern;

	old_object  = zend_objects_get_address(zobject TSRMLS_CC);
	new_obj_val = spl_fastarray_object_new_ex(old_object->ce, &intern, zobject, 1 TSRMLS_CC);
	new_object  = &intern->std;

	zend_objects_clone_members(new_object, new_obj_val, old_object, handle TSRMLS_CC);

	return new_obj_val;
}
/* }}} */

static inline zval **spl_fastarray_object_read_dimension_helper(spl_fastarray_object *intern, zval *offset TSRMLS_DC) /* {{{ */
{
	long index;

	/* we have to return NULL on error here to avoid memleak because of 
	 * ZE duplicating uninitialized_zval_ptr */
	if (!offset) {
		zend_throw_exception(spl_ce_RuntimeException, "Index invalid or out of range", 0 TSRMLS_CC);
		return NULL;
	}

	index = spl_offset_convert_to_long(offset TSRMLS_CC);
	
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

static zval *spl_fastarray_object_read_dimension(zval *object, zval *offset, int type TSRMLS_DC) /* {{{ */
{
	spl_fastarray_object *intern;
	zval **retval;

	intern = (spl_fastarray_object *)zend_object_store_get_object(object TSRMLS_CC);

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

	retval = spl_fastarray_object_read_dimension_helper(intern, offset TSRMLS_CC);
	if (retval) {
		return *retval;
	}
	return NULL;
}
/* }}} */

static inline void spl_fastarray_object_write_dimension_helper(spl_fastarray_object *intern, zval *offset, zval *value TSRMLS_DC) /* {{{ */
{
	long index;

	if (!offset) {
		/* '$array[] = value' syntax is not supported */
		zend_throw_exception(spl_ce_RuntimeException, "Index invalid or out of range", 0 TSRMLS_CC);
		return;
	}

	index = spl_offset_convert_to_long(offset TSRMLS_CC);

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

static void spl_fastarray_object_write_dimension(zval *object, zval *offset, zval *value TSRMLS_DC) /* {{{ */
{
	spl_fastarray_object *intern;

	intern = (spl_fastarray_object *)zend_object_store_get_object(object TSRMLS_CC);

	if (intern->fptr_offset_set) {
		SEPARATE_ARG_IF_REF(offset);
		SEPARATE_ARG_IF_REF(value);
		zend_call_method_with_2_params(&object, intern->std.ce, &intern->fptr_offset_set, "offsetSet", NULL, offset, value);
		zval_ptr_dtor(&value);
		zval_ptr_dtor(&offset);
		return;
	}

	spl_fastarray_object_write_dimension_helper(intern, offset, value TSRMLS_CC);
}
/* }}} */

static inline void spl_fastarray_object_unset_dimension_helper(spl_fastarray_object *intern, zval *offset TSRMLS_DC) /* {{{ */
{
	long index;
	
	index = spl_offset_convert_to_long(offset TSRMLS_CC);
	
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

static void spl_fastarray_object_unset_dimension(zval *object, zval *offset TSRMLS_DC) /* {{{ */
{
	spl_fastarray_object *intern;

	intern = (spl_fastarray_object *)zend_object_store_get_object(object TSRMLS_CC);

	if (intern->fptr_offset_del) {
		SEPARATE_ARG_IF_REF(offset);
		zend_call_method_with_1_params(&object, intern->std.ce, &intern->fptr_offset_del, "offsetUnset", NULL, offset);
		zval_ptr_dtor(&offset);
		return;
	}

	spl_fastarray_object_unset_dimension_helper(intern, offset TSRMLS_CC);

}
/* }}} */

static inline int spl_fastarray_object_has_dimension_helper(spl_fastarray_object *intern, zval *offset, int check_empty TSRMLS_DC) /* {{{ */
{
	long index;
	int retval;
	
	index = spl_offset_convert_to_long(offset TSRMLS_CC);
	
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

static int spl_fastarray_object_has_dimension(zval *object, zval *offset, int check_empty TSRMLS_DC) /* {{{ */
{
	spl_fastarray_object *intern;

	intern = (spl_fastarray_object *)zend_object_store_get_object(object TSRMLS_CC);

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

	return spl_fastarray_object_has_dimension_helper(intern, offset, check_empty TSRMLS_CC);
}
/* }}} */

static int spl_fastarray_object_count_elements(zval *object, long *count TSRMLS_DC) /* {{{ */
{
	spl_fastarray_object *intern;
	
	intern = (spl_fastarray_object *)zend_object_store_get_object(object TSRMLS_CC);
	if (intern->array) {
		*count = intern->array->size;
	} else {
		*count = 0;
	}

	return SUCCESS;
}
/* }}} */

/* {{{ proto void SplFastArray::__construct([int size])
*/
SPL_METHOD(SplFastArray, __construct)
{
	zval *object = getThis();
	spl_fastarray_object *intern;
	long size = 0;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &size)) {
		return;
	}

	if (size < 0) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "array size cannot be less than zero");
		return;
	}

	intern = (spl_fastarray_object *)zend_object_store_get_object(object TSRMLS_CC);

	if (intern->array) {
		/* called __construct() twice, bail out */
		return;
	}

	intern->array = emalloc(sizeof(spl_fastarray));
	spl_fastarray_init(intern->array, size TSRMLS_CC);
}
/* }}} */

/* {{{ proto int SplFastArray::count(void)
*/
SPL_METHOD(SplFastArray, count)
{
	zval *object = getThis();
	spl_fastarray_object *intern;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "")) {
		return;
	}

	intern = (spl_fastarray_object *)zend_object_store_get_object(object TSRMLS_CC);
	if (intern->array) {
		RETURN_LONG(intern->array->size);
	}
	RETURN_LONG(0);
}
/* }}} */

/* {{{ proto int SplFastArray::getSize(void)
*/
SPL_METHOD(SplFastArray, getSize)
{
	zval *object = getThis();
	spl_fastarray_object *intern;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "")) {
		return;
	}

	intern = (spl_fastarray_object *)zend_object_store_get_object(object TSRMLS_CC);
	if (intern->array) {
		RETURN_LONG(intern->array->size);
	}
	RETURN_LONG(0);
}
/* }}} */

/* {{{ proto bool SplFastArray::setSize(int size)
*/
SPL_METHOD(SplFastArray, setSize)
{
	zval *object = getThis();
	spl_fastarray_object *intern;
	long size;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &size)) {
		return;
	}

	if (size < 0) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "array size cannot be less than zero");
		return;
	}

	intern = (spl_fastarray_object *)zend_object_store_get_object(object TSRMLS_CC);
	if (!intern->array) {
		intern->array = ecalloc(1, sizeof(spl_fastarray));
	}

	spl_fastarray_resize(intern->array, size TSRMLS_CC);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool SplFastArray::offsetExists(mixed $index) U
 Returns whether the requested $index exists. */
SPL_METHOD(SplFastArray, offsetExists)
{
	zval                  *zindex;
	spl_fastarray_object  *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zindex) == FAILURE) {
		return;
	}

	intern = (spl_fastarray_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_BOOL(spl_fastarray_object_has_dimension_helper(intern, zindex, 0 TSRMLS_CC));
} /* }}} */

/* {{{ proto mixed SplFastArray::offsetGet(mixed $index) U
 Returns the value at the specified $index. */
SPL_METHOD(SplFastArray, offsetGet)
{
	zval                  *zindex, **value_pp;
	spl_fastarray_object  *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zindex) == FAILURE) {
		return;
	}

	intern    = (spl_fastarray_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
	value_pp  = spl_fastarray_object_read_dimension_helper(intern, zindex TSRMLS_CC);

	if (value_pp) {
		RETURN_ZVAL(*value_pp, 1, 0);
	}
	RETURN_NULL();
} /* }}} */

/* {{{ proto void SplFastArray::offsetSet(mixed $index, mixed $newval) U
 Sets the value at the specified $index to $newval. */
SPL_METHOD(SplFastArray, offsetSet)
{
	zval                  *zindex, *value;
	spl_fastarray_object  *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &zindex, &value) == FAILURE) {
		return;
	}

	intern = (spl_fastarray_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
	spl_fastarray_object_write_dimension_helper(intern, zindex, value TSRMLS_CC);

} /* }}} */

/* {{{ proto void SplFastArray::offsetUnset(mixed $index) U
 Unsets the value at the specified $index. */
SPL_METHOD(SplFastArray, offsetUnset)
{
	zval                  *zindex;
	spl_fastarray_object  *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zindex) == FAILURE) {
		return;
	}

	intern = (spl_fastarray_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
	spl_fastarray_object_unset_dimension_helper(intern, zindex TSRMLS_CC);

} /* }}} */

static void spl_fastarray_it_dtor(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_fastarray_it  *iterator = (spl_fastarray_it *)iter;

	zend_user_it_invalidate_current(iter TSRMLS_CC);
	zval_ptr_dtor((zval**)&iterator->intern.it.data);

	efree(iterator);
}
/* }}} */

static void spl_fastarray_it_rewind(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_fastarray_it     *iterator = (spl_fastarray_it *)iter;
	spl_fastarray_object *intern   = iterator->object;
	zval                 *object   = (zval *)&iterator->intern.it.data;

	if (intern->fptr_it_rewind) {
		zend_call_method_with_0_params(&object, intern->std.ce, &intern->fptr_it_rewind, "rewind", NULL);
	}
	iterator->object->current = 0;
}
/* }}} */

static int spl_fastarray_it_valid(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_fastarray_it     *iterator = (spl_fastarray_it *)iter;
	spl_fastarray_object *intern   = iterator->object;
	zval                 *object   = (zval *)&iterator->intern.it.data;

	if (intern->fptr_it_valid) {
		zval *rv;
		zend_call_method_with_0_params(&object, intern->std.ce, &intern->fptr_it_valid, "valid", &rv);
		if (rv) {
			zval_ptr_dtor(&intern->retval);
			MAKE_STD_ZVAL(intern->retval);
			ZVAL_ZVAL(intern->retval, rv, 1, 1);
			return zend_is_true(intern->retval) ? SUCCESS : FAILURE;
		}
		return FAILURE;
	}

	if (iterator->object->current >= 0 && iterator->object->array && iterator->object->current < iterator->object->array->size) {
		return SUCCESS;
	}

	return FAILURE;
}
/* }}} */

static void spl_fastarray_it_get_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC) /* {{{ */
{
	zval                 *zindex;
	spl_fastarray_it     *iterator = (spl_fastarray_it *)iter;
	spl_fastarray_object *intern   = iterator->object;
	zval                 *object   = (zval *)&iterator->intern.it.data;

	if (intern->fptr_it_current) {
		zval *rv;
		zend_call_method_with_0_params(&object, intern->std.ce, &intern->fptr_it_current, "current", &rv);
		if (rv) {
			zval_ptr_dtor(&intern->retval);
			MAKE_STD_ZVAL(intern->retval);
			ZVAL_ZVAL(intern->retval, rv, 1, 1);
			*data = &intern->retval;
			return;
		}
		*data = NULL;
		return;
	}

	ALLOC_INIT_ZVAL(zindex);
	ZVAL_LONG(zindex, iterator->object->current);

	*data = spl_fastarray_object_read_dimension_helper(intern, zindex TSRMLS_CC);

	if (*data == NULL) {
		*data = &EG(uninitialized_zval_ptr);
	}

	zval_ptr_dtor(&zindex);
}
/* }}} */

static int spl_fastarray_it_get_current_key(zend_object_iterator *iter, char **str_key, uint *str_key_len, ulong *int_key TSRMLS_DC) /* {{{ */
{
	spl_fastarray_it     *iterator = (spl_fastarray_it *)iter;
	spl_fastarray_object *intern   = iterator->object;
	zval                 *object   = (zval *)&iterator->intern.it.data;

	if (intern->fptr_it_key) {
		zval *rv;
		zend_call_method_with_0_params(&object, intern->std.ce, &intern->fptr_it_key, "key", &rv);
		if (rv) {
			zval_ptr_dtor(&intern->retval);
			MAKE_STD_ZVAL(intern->retval);
			ZVAL_ZVAL(intern->retval, rv, 1, 1);
			convert_to_long(intern->retval);
			*int_key = (ulong) Z_LVAL_P(intern->retval);
		}
		*int_key = (ulong) 0;
	} else {
		*int_key = (ulong) iterator->object->current;
	}

	return HASH_KEY_IS_LONG;
}
/* }}} */

static void spl_fastarray_it_move_forward(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_fastarray_it     *iterator = (spl_fastarray_it *)iter;
	spl_fastarray_object *intern   = iterator->object;
	zval                 *object   = (zval *)&iterator->intern.it.data;

	if (intern->fptr_it_next) {
		zend_call_method_with_0_params(&object, intern->std.ce, &intern->fptr_it_next, "next", NULL);
	}

	zend_user_it_invalidate_current(iter TSRMLS_CC);

	iterator->object->current++;
}
/* }}} */

/* {{{  proto int SplFastArray::key() U
   Return current array key */
SPL_METHOD(SplFastArray, key)
{
	spl_fastarray_object *intern = (spl_fastarray_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_LONG(intern->current);
}
/* }}} */

/* {{{ proto void SplFastArray::next() U
   Move to next entry */
SPL_METHOD(SplFastArray, next)
{
	spl_fastarray_object *intern = (spl_fastarray_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	intern->current++;
}
/* }}} */

/* {{{ proto bool SplFastArray::valid() U
   Check whether the datastructure contains more entries */
SPL_METHOD(SplFastArray, valid)
{
	spl_fastarray_object *intern = (spl_fastarray_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	RETURN_BOOL(intern->current >= 0 && intern->array && intern->current < intern->array->size);
}
/* }}} */

/* {{{ proto void SplFastArray::rewind() U
   Rewind the datastructure back to the start */
SPL_METHOD(SplFastArray, rewind)
{
	spl_fastarray_object *intern = (spl_fastarray_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	intern->current = 0;
}
/* }}} */

/* {{{ proto mixed|NULL SplFastArray::current() U
   Return current datastructure entry */
SPL_METHOD(SplFastArray, current)
{
	zval                 *zindex, **value_pp;
	spl_fastarray_object *intern  = (spl_fastarray_object*)zend_object_store_get_object(getThis() TSRMLS_CC);


	ALLOC_INIT_ZVAL(zindex);
	ZVAL_LONG(zindex, intern->current);

	value_pp  = spl_fastarray_object_read_dimension_helper(intern, zindex TSRMLS_CC);

	zval_ptr_dtor(&zindex);

	if (value_pp) {
		RETURN_ZVAL(*value_pp, 1, 0);
	}
	RETURN_NULL();
}
/* }}} */

/* iterator handler table */
zend_object_iterator_funcs spl_fastarray_it_funcs = {
	spl_fastarray_it_dtor,
	spl_fastarray_it_valid,
	spl_fastarray_it_get_current_data,
	spl_fastarray_it_get_current_key,
	spl_fastarray_it_move_forward,
	spl_fastarray_it_rewind
};

zend_object_iterator *spl_fastarray_get_iterator(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC) /* {{{ */
{
	spl_fastarray_it      *iterator;
	spl_fastarray_object  *fastarray_object = (spl_fastarray_object*)zend_object_store_get_object(object TSRMLS_CC);

	if (by_ref) {
		zend_throw_exception(spl_ce_RuntimeException, "An iterator cannot be used with foreach by reference", 0 TSRMLS_CC);
		return NULL;
	}

	Z_ADDREF_P(object);

	iterator                     = emalloc(sizeof(spl_fastarray_it));
	iterator->intern.it.data     = (void*)object;
	iterator->intern.it.funcs    = &spl_fastarray_it_funcs;
	iterator->intern.ce          = ce;
	iterator->intern.value       = NULL;
	iterator->object             = fastarray_object;

	return (zend_object_iterator*)iterator;
}
/* }}} */

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fastarray_offsetGet, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_fastarray_offsetSet, 0, 0, 2)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_INFO(0, newval)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_fastarray_setSize, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

static zend_function_entry spl_funcs_SplFastArray[] = { /* {{{ */
	SPL_ME(SplFastArray, __construct,     NULL, ZEND_ACC_PUBLIC)
	SPL_ME(SplFastArray, count,           NULL,                           ZEND_ACC_PUBLIC)
	SPL_ME(SplFastArray, getSize,         NULL,                           ZEND_ACC_PUBLIC)
	SPL_ME(SplFastArray, setSize,         arginfo_fastarray_setSize,      ZEND_ACC_PUBLIC)
	SPL_ME(SplFastArray, offsetExists,    arginfo_fastarray_offsetGet,    ZEND_ACC_PUBLIC)
	SPL_ME(SplFastArray, offsetGet,       arginfo_fastarray_offsetGet,    ZEND_ACC_PUBLIC)
	SPL_ME(SplFastArray, offsetSet,       arginfo_fastarray_offsetSet,    ZEND_ACC_PUBLIC)
	SPL_ME(SplFastArray, offsetUnset,     arginfo_fastarray_offsetGet,    ZEND_ACC_PUBLIC)
	SPL_ME(SplFastArray, rewind,          NULL,                           ZEND_ACC_PUBLIC)
	SPL_ME(SplFastArray, current,         NULL,                           ZEND_ACC_PUBLIC)
	SPL_ME(SplFastArray, key,             NULL,                           ZEND_ACC_PUBLIC)
	SPL_ME(SplFastArray, next,            NULL,                           ZEND_ACC_PUBLIC)
	SPL_ME(SplFastArray, valid,           NULL,                           ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(spl_fastarray)
{
	REGISTER_SPL_STD_CLASS_EX(SplFastArray, spl_fastarray_new, spl_funcs_SplFastArray);
	memcpy(&spl_handler_SplFastArray, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

	spl_handler_SplFastArray.clone_obj       = spl_fastarray_object_clone;
	spl_handler_SplFastArray.count_elements  = spl_fastarray_object_count_elements;
	spl_handler_SplFastArray.read_dimension  = spl_fastarray_object_read_dimension;
	spl_handler_SplFastArray.write_dimension = spl_fastarray_object_write_dimension;
	spl_handler_SplFastArray.unset_dimension = spl_fastarray_object_unset_dimension;
	spl_handler_SplFastArray.has_dimension   = spl_fastarray_object_has_dimension;
	spl_handler_SplFastArray.count_elements  = spl_fastarray_object_count_elements;

	REGISTER_SPL_IMPLEMENTS(SplFastArray, Iterator);
	REGISTER_SPL_IMPLEMENTS(SplFastArray, ArrayAccess);
	REGISTER_SPL_IMPLEMENTS(SplFastArray, Countable);

	spl_ce_SplFastArray->get_iterator = spl_fastarray_get_iterator;

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
