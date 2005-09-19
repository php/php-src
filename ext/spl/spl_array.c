/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2004 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"

#include "php_spl.h"
#include "spl_functions.h"
#include "spl_engine.h"
#include "spl_iterators.h"
#include "spl_array.h"

SPL_METHOD(Array, __construct);
SPL_METHOD(Array, getIterator);
SPL_METHOD(Array, rewind);
SPL_METHOD(Array, current);
SPL_METHOD(Array, key);
SPL_METHOD(Array, next);
SPL_METHOD(Array, valid);
SPL_METHOD(Array, offsetExists);
SPL_METHOD(Array, offsetGet);
SPL_METHOD(Array, offsetSet);
SPL_METHOD(Array, offsetUnset);
SPL_METHOD(Array, append);
SPL_METHOD(Array, getArrayCopy);
SPL_METHOD(Array, seek);
SPL_METHOD(Array, count);

static
ZEND_BEGIN_ARG_INFO(arginfo_array___construct, 0)
	ZEND_ARG_INFO(0, array)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_array_offsetGet, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_array_offsetSet, 0, 0, 2)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_INFO(0, newval)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO(arginfo_array_append, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO(arginfo_array_seek, 0)
	ZEND_ARG_INFO(0, position)
ZEND_END_ARG_INFO();

static zend_function_entry spl_funcs_ArrayObject[] = {
	SPL_ME(Array, __construct,   arginfo_array___construct, ZEND_ACC_PUBLIC)
	SPL_ME(Array, offsetExists,  arginfo_array_offsetGet, ZEND_ACC_PUBLIC)
	SPL_ME(Array, offsetGet,     arginfo_array_offsetGet, ZEND_ACC_PUBLIC)
	SPL_ME(Array, offsetSet,     arginfo_array_offsetSet, ZEND_ACC_PUBLIC)
	SPL_ME(Array, offsetUnset,   arginfo_array_offsetGet, ZEND_ACC_PUBLIC)
	SPL_ME(Array, append,        arginfo_array_append,    ZEND_ACC_PUBLIC)
	SPL_ME(Array, getArrayCopy,  NULL, ZEND_ACC_PUBLIC)
	SPL_ME(Array, count,         NULL, ZEND_ACC_PUBLIC)
	/* ArrayObject specific */
	SPL_ME(Array, getIterator,   NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static zend_function_entry spl_funcs_ArrayIterator[] = {
	SPL_ME(Array, __construct,   arginfo_array___construct, ZEND_ACC_PUBLIC)
	SPL_ME(Array, offsetExists,  arginfo_array_offsetGet, ZEND_ACC_PUBLIC)
	SPL_ME(Array, offsetGet,     arginfo_array_offsetGet, ZEND_ACC_PUBLIC)
	SPL_ME(Array, offsetSet,     arginfo_array_offsetSet, ZEND_ACC_PUBLIC)
	SPL_ME(Array, offsetUnset,   arginfo_array_offsetGet, ZEND_ACC_PUBLIC)
	SPL_ME(Array, append,        arginfo_array_append,    ZEND_ACC_PUBLIC)
	SPL_ME(Array, getArrayCopy,  NULL, ZEND_ACC_PUBLIC)
	SPL_ME(Array, count,         NULL, ZEND_ACC_PUBLIC)
	/* ArrayIterator specific */
	SPL_ME(Array, rewind,        NULL, ZEND_ACC_PUBLIC)
	SPL_ME(Array, current,       NULL, ZEND_ACC_PUBLIC)
	SPL_ME(Array, key,           NULL, ZEND_ACC_PUBLIC)
	SPL_ME(Array, next,          NULL, ZEND_ACC_PUBLIC)
	SPL_ME(Array, valid,         NULL, ZEND_ACC_PUBLIC)
	SPL_ME(Array, seek,          arginfo_array_seek,        ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};


zend_object_handlers spl_handler_ArrayObject;
PHPAPI zend_class_entry  *spl_ce_ArrayObject;

zend_object_handlers spl_handler_ArrayIterator;
PHPAPI zend_class_entry *spl_ce_ArrayIterator;

typedef struct _spl_array_object {
	zend_object       std;
	zval              *array;
	HashPosition      pos;
	int               is_ref;
	zend_function *   fptr_offset_get;
	zend_function *   fptr_offset_set;
	zend_function *   fptr_offset_has;
	zend_function *   fptr_offset_del;
} spl_array_object;

SPL_API int spl_hash_verify_pos(spl_array_object * intern TSRMLS_DC) /* {{{ */
{
	HashTable *ht = HASH_OF(intern->array);
	Bucket *p;

/*	IS_CONSISTENT(ht);*/

/*	HASH_PROTECT_RECURSION(ht);*/
	p = ht->pListHead;
	while (p != NULL) {
		if (p == intern->pos) {
			return SUCCESS;
		}
		p = p->pListNext;
	}
/*	HASH_UNPROTECT_RECURSION(ht); */
	zend_hash_internal_pointer_reset_ex(HASH_OF(intern->array), &intern->pos);
	return FAILURE;
}
/* }}} */

/* {{{ spl_array_object_free_storage */
static void spl_array_object_free_storage(void *object TSRMLS_DC)
{
	spl_array_object *intern = (spl_array_object *)object;

	zend_hash_destroy(intern->std.properties);
	FREE_HASHTABLE(intern->std.properties);

	zval_ptr_dtor(&intern->array);

	efree(object);
}
/* }}} */

/* {{{ spl_array_object_new */
static zend_object_value spl_array_object_new_ex(zend_class_entry *class_type, spl_array_object **obj, spl_array_object *orig TSRMLS_DC)
{
	zend_object_value retval;
	spl_array_object *intern;
	zval *tmp;
	zend_class_entry * parent = class_type;
	int inherited = 0;

	intern = emalloc(sizeof(spl_array_object));
	memset(intern, 0, sizeof(spl_array_object));
	intern->std.ce = class_type;
	*obj = intern;

	ALLOC_HASHTABLE(intern->std.properties);
	zend_hash_init(intern->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	if (orig) {
		intern->array = orig->array;
		ZVAL_ADDREF(intern->array);
		intern->is_ref = 1;
	} else {
		MAKE_STD_ZVAL(intern->array);
		array_init(intern->array);
		intern->is_ref = 0;
	}
	zend_hash_internal_pointer_reset_ex(HASH_OF(intern->array), &intern->pos);

	retval.handle = zend_objects_store_put(intern, NULL, (zend_objects_free_object_storage_t) spl_array_object_free_storage, NULL TSRMLS_CC);
	while (parent) {
		if (parent == spl_ce_ArrayIterator) {
			retval.handlers = &spl_handler_ArrayIterator;
			break;
		} else if (parent == spl_ce_ArrayObject) {
			retval.handlers = &spl_handler_ArrayObject;
			break;
		}
		parent = parent->parent;
		inherited = 1;
	}
	if (!parent) { /* this must never happen */
		php_error_docref(NULL TSRMLS_CC, E_COMPILE_ERROR, "Internal compiler error, Class is not child of ArrayObject or arrayIterator");
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
	}
	return retval;
}
/* }}} */

/* {{{ spl_array_object_new */
static zend_object_value spl_array_object_new(zend_class_entry *class_type TSRMLS_DC)
{
	spl_array_object *tmp;
	return spl_array_object_new_ex(class_type, &tmp, NULL TSRMLS_CC);
}
/* }}} */

/* {{{ spl_array_object_clone */
static zend_object_value spl_array_object_clone(zval *zobject TSRMLS_DC)
{
	zend_object_value new_obj_val;
	zend_object *old_object;
	zend_object *new_object;
	zend_object_handle handle = Z_OBJ_HANDLE_P(zobject);
	spl_array_object *intern;

	old_object = zend_objects_get_address(zobject TSRMLS_CC);
	new_obj_val = spl_array_object_new_ex(old_object->ce, &intern, (spl_array_object*)old_object TSRMLS_CC);
	new_object = &intern->std;

	zend_objects_clone_members(new_object, new_obj_val, old_object, handle TSRMLS_CC);

	return new_obj_val;
}
/* }}} */

static zval *spl_array_read_dimension_ex(int check_inherited, zval *object, zval *offset, int type TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	zval **retval, *rv;
	long index;

	if (check_inherited && intern->fptr_offset_get) {
		return zend_call_method_with_1_params(&object, Z_OBJCE_P(object), &intern->fptr_offset_get, "offsetGet", &rv, offset);
	}
	
	switch(Z_TYPE_P(offset)) {
	case IS_STRING:
		if (zend_symtable_find(HASH_OF(intern->array), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, (void **) &retval) == FAILURE) {
			zend_error(E_NOTICE, "Undefined index:  %s", Z_STRVAL_P(offset));
			return EG(uninitialized_zval_ptr);
		} else {
			return *retval;
		}
	case IS_DOUBLE:
	case IS_RESOURCE:
	case IS_BOOL: 
	case IS_LONG: 
		if (offset->type == IS_DOUBLE) {
			index = (long)Z_DVAL_P(offset);
		} else {
			index = Z_LVAL_P(offset);
		}
		if (zend_hash_index_find(HASH_OF(intern->array), index, (void **) &retval) == FAILURE) {
			zend_error(E_NOTICE, "Undefined offset:  %ld", Z_LVAL_P(offset));
			return EG(uninitialized_zval_ptr);
		} else {
			return *retval;
		}
		break;
	default:
		zend_error(E_WARNING, "Illegal offset type");
		return EG(uninitialized_zval_ptr);
	}
} /* }}} */

static zval *spl_array_read_dimension(zval *object, zval *offset, int type TSRMLS_DC) /* {{{ */
{
	return spl_array_read_dimension_ex(1, object, offset, type TSRMLS_CC);
} /* }}} */

static void spl_array_write_dimension_ex(int check_inherited, zval *object, zval *offset, zval *value TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	long index;
	int free_offset;

	if (check_inherited && intern->fptr_offset_set) {
		if (!offset) {
			ALLOC_INIT_ZVAL(offset);
			free_offset = 1;
		} else {
			free_offset = 0;
		}
		zend_call_method_with_2_params(&object, Z_OBJCE_P(object), &intern->fptr_offset_set, "offsetSet", NULL, offset, value);
		if (free_offset) {
			zval_ptr_dtor(&offset);
		}
		return;
	}

	if (!offset) {
		value->refcount++;
		zend_hash_next_index_insert(HASH_OF(intern->array), (void**)&value, sizeof(void*), NULL);
		return;
	}
	switch(Z_TYPE_P(offset)) {
	case IS_STRING:
		value->refcount++;
		zend_symtable_update(HASH_OF(intern->array), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, (void**)&value, sizeof(void*), NULL);
		return;
	case IS_DOUBLE:
	case IS_RESOURCE:
	case IS_BOOL: 
	case IS_LONG: 
		if (offset->type == IS_DOUBLE) {
			index = (long)Z_DVAL_P(offset);
		} else {
			index = Z_LVAL_P(offset);
		}
		value->refcount++;
		zend_hash_index_update(HASH_OF(intern->array), index, (void**)&value, sizeof(void*), NULL);
		return;
    case IS_NULL:
		value->refcount++;
		zend_hash_next_index_insert(HASH_OF(intern->array), (void**)&value, sizeof(void*), NULL);
		return;						
	default:
		zend_error(E_WARNING, "Illegal offset type");
		return;
	}
} /* }}} */

static void spl_array_write_dimension(zval *object, zval *offset, zval *value TSRMLS_DC) /* {{{ */
{
	spl_array_write_dimension_ex(1, object, offset, value TSRMLS_CC);
} /* }}} */

static void spl_array_unset_dimension_ex(int check_inherited, zval *object, zval *offset TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	long index;
	zval *rv;

	if (check_inherited && intern->fptr_offset_del) {
		zend_call_method_with_1_params(&object, Z_OBJCE_P(object), &intern->fptr_offset_del, "offsetUnset", &rv, offset);
		return;
	}

	switch(Z_TYPE_P(offset)) {
	case IS_STRING:





		if (zend_symtable_del(HASH_OF(intern->array), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1) == FAILURE) {
			zend_error(E_NOTICE,"Undefined index:  %s", Z_STRVAL_P(offset));
		}
		break;
	case IS_DOUBLE:
	case IS_RESOURCE:
	case IS_BOOL: 
	case IS_LONG: 
		if (offset->type == IS_DOUBLE) {
			index = (long)Z_DVAL_P(offset);
		} else {
			index = Z_LVAL_P(offset);
		}
		if (zend_hash_index_del(HASH_OF(intern->array), index) == FAILURE) {
			zend_error(E_NOTICE,"Undefined offset:  %ld", Z_LVAL_P(offset));
		}
		break;
	default:
		zend_error(E_WARNING, "Illegal offset type");
		return;
	}
	spl_hash_verify_pos(intern TSRMLS_CC); /* call rewind on FAILURE */
} /* }}} */

static void spl_array_unset_dimension(zval *object, zval *offset TSRMLS_DC) /* {{{ */
{
	spl_array_unset_dimension_ex(1, object, offset TSRMLS_CC);
} /* }}} */

static int spl_array_has_dimension_ex(int check_inherited, zval *object, zval *offset, int check_empty TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	long index;
	zval *rv;

	if (check_inherited && intern->fptr_offset_has) {
		zend_call_method_with_1_params(&object, Z_OBJCE_P(object), &intern->fptr_offset_has, "offsetExists", &rv, offset);
		if (zend_is_true(rv)) {
			zval_ptr_dtor(&rv);
			return 1;
		}
		zval_ptr_dtor(&rv);
		return 0;
	}

	switch(Z_TYPE_P(offset)) {
	case IS_STRING:
		return zend_symtable_exists(HASH_OF(intern->array), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1);
	case IS_DOUBLE:
	case IS_RESOURCE:
	case IS_BOOL: 
	case IS_LONG: 
		if (offset->type == IS_DOUBLE) {
			index = (long)Z_DVAL_P(offset);
		} else {
			index = Z_LVAL_P(offset);
		}
		return zend_hash_index_exists(HASH_OF(intern->array), index);
	default:
		zend_error(E_WARNING, "Illegal offset type");
	}
	return 0;
} /* }}} */

static int spl_array_has_dimension(zval *object, zval *offset, int check_empty TSRMLS_DC) /* {{{ */
{
	return spl_array_has_dimension_ex(1, object, offset, check_empty TSRMLS_CC);
} /* }}} */

/* {{{ proto bool ArrayObject::offsetExists(mixed $index)
       proto bool ArrayIterator::offsetExists(mixed $index)
 Returns whether the requested $index exists. */
SPL_METHOD(Array, offsetExists)
{
	zval *index;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &index) == FAILURE) {
		return;
	}
	RETURN_BOOL(spl_array_has_dimension_ex(0, getThis(), index, 1 TSRMLS_CC));
} /* }}} */

/* {{{ proto bool ArrayObject::offsetGet(mixed $index)
       proto bool ArrayIterator::offsetGet(mixed $index)
 Returns the value at the specified $index. */
SPL_METHOD(Array, offsetGet)
{
	zval *index, *value;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &index) == FAILURE) {
		return;
	}
	value = spl_array_read_dimension_ex(0, getThis(), index, BP_VAR_R TSRMLS_CC);
	RETURN_ZVAL(value, 1, 0);
} /* }}} */

/* {{{ proto void ArrayObject::offsetSet(mixed $index, mixed $newval)
       proto void ArrayIterator::offsetSet(mixed $index, mixed $newval)
 Sets the value at the specified $index to $newval. */
SPL_METHOD(Array, offsetSet)
{
	zval *index, *value = NULL;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &index, &value) == FAILURE) {
		return;
	}
	spl_array_write_dimension_ex(0, getThis(), index, value TSRMLS_CC);
} /* }}} */


void spl_array_iterator_append(zval *object, zval *append_value TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	HashTable *aht = HASH_OF(intern->array);

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and is no longer an array");
		return;
	}
	
	if (Z_TYPE_P(intern->array) == IS_OBJECT) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Cannot append properties to objects, use %s::offsetSet() instead", Z_OBJCE_P(object)->name);
	}

	spl_array_write_dimension(object, NULL, append_value TSRMLS_CC);
	if (!intern->pos) {
		intern->pos = aht->pListTail;
	}
} /* }}} */

/* {{{ proto void ArrayObject::append(mixed $newval)
       proto void ArrayIterator::append(mixed $newval)
 Appends the value (cannot be called for objects). */
SPL_METHOD(Array, append)
{
	zval *value;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value) == FAILURE) {
		return;
	}
	spl_array_iterator_append(getThis(), value TSRMLS_CC);
} /* }}} */

/* {{{ proto void ArrayObject::offsetUnset(mixed $index)
       proto void ArrayIterator::offsetUnset(mixed $index)
 Unsets the value at the specified $index. */
SPL_METHOD(Array, offsetUnset)
{
	zval *index;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &index) == FAILURE) {
		return;
	}
	spl_array_unset_dimension_ex(0, getThis(), index TSRMLS_CC);
} /* }}} */

/* {{ proto array ArrayObject::getArrayCopy()
      proto array ArrayIterator::getArrayCopy()
 Return a copy of the contained array */
SPL_METHOD(Array, getArrayCopy)
{
	zval *object = getThis(), *tmp;
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
    
    array_init(return_value);
	zend_hash_copy(HASH_OF(return_value), HASH_OF(intern->array), (copy_ctor_func_t) zval_add_ref, &tmp, sizeof(zval*));
} /* }}} */

/* {{{ spl_array_get_properties */
static HashTable *spl_array_get_properties(zval *object TSRMLS_DC)
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);

	return HASH_OF(intern->array);
}
/* }}} */

static int spl_array_skip_protected(spl_array_object *intern TSRMLS_DC) /* {{{ */
{
	char *string_key;
	uint string_length;
	ulong num_key;
	HashTable *aht = HASH_OF(intern->array);

	if (Z_TYPE_P(intern->array) == IS_OBJECT) {
		do {
			if (zend_hash_get_current_key_ex(aht, &string_key, &string_length, &num_key, 0, &intern->pos) == HASH_KEY_IS_STRING) {
				if (!string_length || string_key[0]) {
					return SUCCESS;
				}
			} else {
				return SUCCESS;
			}
			if (zend_hash_has_more_elements_ex(aht, &intern->pos) != SUCCESS) {
				return FAILURE;
			}
			zend_hash_move_forward_ex(aht, &intern->pos);
		} while (1);
	}
	return FAILURE;
}
/* }}} */

static int spl_array_next(spl_array_object *intern TSRMLS_DC) /* {{{ */
{
	HashTable *aht = HASH_OF(intern->array);

	if (intern->is_ref && spl_hash_verify_pos(intern TSRMLS_CC) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and internal position is no longer valid");
		return FAILURE;
	} else {
		zend_hash_move_forward_ex(aht, &intern->pos);
		if (Z_TYPE_P(intern->array) == IS_OBJECT) {
			return spl_array_skip_protected(intern TSRMLS_CC);
		} else {
			return zend_hash_has_more_elements_ex(aht, &intern->pos);
		}
	}
} /* }}} */

/* define an overloaded iterator structure */
typedef struct {
	zend_object_iterator  intern;
	spl_array_object      *object;
} spl_array_it;

static void spl_array_it_dtor(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_array_it *iterator = (spl_array_it *)iter;

	zval_ptr_dtor((zval**)&iterator->intern.data);

	efree(iterator);
}
/* }}} */
	
static int spl_array_it_valid(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_array_it       *iterator = (spl_array_it *)iter;
	spl_array_object   *object   = iterator->object;
	HashTable          *aht      = HASH_OF(object->array);

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "ArrayIterator::valid(): Array was modified outside object and is no longer an array");
		return FAILURE;
	}

	if (object->pos && object->is_ref && spl_hash_verify_pos(object TSRMLS_CC) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "ArrayIterator::valid(): Array was modified outside object and internal position is no longer valid");
		return FAILURE;
	} else {
		return zend_hash_has_more_elements_ex(aht, &object->pos);
	}
}
/* }}} */

static void spl_array_it_get_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC) /* {{{ */
{
	spl_array_it       *iterator = (spl_array_it *)iter;
	spl_array_object   *object   = iterator->object;
	HashTable          *aht      = HASH_OF(object->array);
	
	if (zend_hash_get_current_data_ex(aht, (void**)data, &object->pos) == FAILURE) {
		*data = NULL;
	}
}
/* }}} */

static int spl_array_it_get_current_key(zend_object_iterator *iter, char **str_key, uint *str_key_len, ulong *int_key TSRMLS_DC) /* {{{ */
{
	spl_array_it       *iterator = (spl_array_it *)iter;
	spl_array_object   *object   = iterator->object;
	HashTable          *aht      = HASH_OF(object->array);

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "ArrayIterator::current(): Array was modified outside object and is no longer an array");
		return HASH_KEY_NON_EXISTANT;
	}

	if (object->is_ref && spl_hash_verify_pos(object TSRMLS_CC) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "ArrayIterator::current(): Array was modified outside object and internal position is no longer valid");
		return HASH_KEY_NON_EXISTANT;
	}

	return zend_hash_get_current_key_ex(aht, str_key, str_key_len, int_key, 1, &object->pos);
}
/* }}} */

static void spl_array_it_move_forward(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_array_it       *iterator = (spl_array_it *)iter;
	spl_array_object   *object   = iterator->object;

	HashTable          *aht      = HASH_OF(object->array);

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "ArrayIterator::current(): Array was modified outside object and is no longer an array");
		return;
	}

	if (object->is_ref && spl_hash_verify_pos(object TSRMLS_CC) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "ArrayIterator::next(): Array was modified outside object and internal position is no longer valid");
	} else {
		spl_array_next(object TSRMLS_CC);
	}
}
/* }}} */

static void spl_array_rewind(spl_array_object *intern TSRMLS_DC) /* {{{ */
{
	HashTable          *aht      = HASH_OF(intern->array);

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "ArrayIterator::rewind(): Array was modified outside object and is no longer an array");
		return;
	}

	zend_hash_internal_pointer_reset_ex(aht, &intern->pos);
	spl_array_skip_protected(intern TSRMLS_CC);
}
/* }}} */

static void spl_array_it_rewind(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_array_it       *iterator = (spl_array_it *)iter;
	spl_array_object   *object   = iterator->object;

	spl_array_rewind(object TSRMLS_CC);
}
/* }}} */

/* iterator handler table */
zend_object_iterator_funcs spl_array_it_funcs = {
	spl_array_it_dtor,
	spl_array_it_valid,
	spl_array_it_get_current_data,
	spl_array_it_get_current_key,
	spl_array_it_move_forward,
	spl_array_it_rewind
};

zend_object_iterator *spl_array_get_iterator(zend_class_entry *ce, zval *object TSRMLS_DC) /* {{{ */
{
	spl_array_it       *iterator     = emalloc(sizeof(spl_array_it));
	spl_array_object   *array_object = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);

	object->refcount++;
	iterator->intern.data = (void*)object;
	iterator->intern.funcs = &spl_array_it_funcs;
	iterator->object = array_object;
	
	return (zend_object_iterator*)iterator;
}
/* }}} */

/* {{{ proto void ArrayObject::__construct(array|object ar = array())
       proto void ArrayIterator::__construct(array|object ar = array())
 Cronstructs a new array iterator from a path. */
SPL_METHOD(Array, __construct)
{
	zval *object = getThis();
	spl_array_object *intern;
	zval **array;

	if (ZEND_NUM_ARGS() == 0) {
		return; /* nothing to do */
	}
	php_set_error_handling(EH_THROW, zend_exception_get_default() TSRMLS_CC);

	intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);

	if (ZEND_NUM_ARGS() > 1 || zend_get_parameters_ex(1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	if (Z_TYPE_PP(array) == IS_OBJECT && (Z_OBJ_HT_PP(array) == &spl_handler_ArrayObject || Z_OBJ_HT_PP(array) == &spl_handler_ArrayIterator)) {
		spl_array_object *other  = (spl_array_object*)zend_object_store_get_object(*array TSRMLS_CC);
		zval_dtor(intern->array);
		FREE_ZVAL(intern->array);
		intern->array = other->array;		
	} else {
		if (!HASH_OF(*array)) {
			php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
			zend_throw_exception(zend_exception_get_default(), "Passed variable is not an array or object, using empty array instead", 0 TSRMLS_CC);
			return;
		}
		zval_dtor(intern->array);
		FREE_ZVAL(intern->array);
		intern->array = *array;
	}
	ZVAL_ADDREF(intern->array);

	zend_hash_internal_pointer_reset_ex(HASH_OF(intern->array), &intern->pos);

	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
}
/* }}} */

/* {{{ proto ArrayIterator ArrayObject::getIterator()
   Create a new iterator from a ArrayObject instance */
SPL_METHOD(Array, getIterator)
{
	zval *object = getThis();
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	spl_array_object *iterator;
	HashTable *aht = HASH_OF(intern->array);

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and is no longer an array");
		return;
	}

	return_value->type = IS_OBJECT;
	return_value->value.obj = spl_array_object_new_ex(spl_ce_ArrayIterator, &iterator, intern TSRMLS_CC);
	return_value->refcount = 1;
	return_value->is_ref = 1;
}
/* }}} */

/* {{{ proto void ArrayIterator::rewind()
   Rewind array back to the start */
SPL_METHOD(Array, rewind)
{
	zval *object = getThis();
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);

	spl_array_rewind(intern TSRMLS_CC);
}
/* }}} */

/* {{{ proto void ArrayIterator::seek(int $position)
 Seek to position. */
SPL_METHOD(Array, seek)
{
	long opos, position;
	zval *object = getThis();
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	HashTable *aht = HASH_OF(intern->array);
	int result;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &position) == FAILURE) {
		return;
	}

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and is no longer an array");
		return;
	}

	opos = position;

	if (position >= 0) { /* negative values are not supported */
		zend_hash_internal_pointer_reset_ex(aht, &intern->pos);
		
		while (position-- > 0 && (result = spl_array_next(intern TSRMLS_CC)) == SUCCESS);
	
		if (intern->pos && intern->is_ref && spl_hash_verify_pos(intern TSRMLS_CC) == FAILURE) {
			/* fail */
		} else {
			if (zend_hash_has_more_elements_ex(aht, &intern->pos) == SUCCESS) {
				return; /* ok */
			}
		}
	}
	zend_throw_exception_ex(zend_exception_get_default(), 0 TSRMLS_CC, "Seek position %ld is out of range", opos);
} /* }}} */

int spl_array_object_count_elements(zval *object, long *count TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	HashTable *aht = HASH_OF(intern->array);
	HashPosition pos;

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and is no longer an array");
		*count = 0;
		return FAILURE;
	}

	if (Z_TYPE_P(intern->array) == IS_OBJECT) {
		/* We need to store the 'pos' since we'll modify it in the functions 
		 * we're going to call and which do not support 'pos' as parameter. */
		pos = intern->pos;
		*count = 0;
		zend_hash_internal_pointer_reset_ex(aht, &intern->pos);
		while(intern->pos && spl_array_next(intern TSRMLS_CC) == SUCCESS) {
			(*count)++;
		}
		intern->pos = pos;
		return SUCCESS;
	} else {
		*count = zend_hash_num_elements(aht);
		return SUCCESS;
	}
} /* }}} */

/* {{{ proto int ArrayObject::count()
       proto int ArrayIterator::count()
 Return the number of elements in the Iterator. */
SPL_METHOD(Array, count)
{
	long count;

	spl_array_object_count_elements(getThis(), &count TSRMLS_CC);
	RETURN_LONG(count);
} /* }}} */

/* {{{ proto mixed|NULL ArrayIterator::current()
   Return current array entry */
SPL_METHOD(Array, current)
{
	zval *object = getThis();
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	zval **entry;
	HashTable *aht = HASH_OF(intern->array);

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and is no longer an array");
		return;
	}

	if (intern->is_ref && spl_hash_verify_pos(intern TSRMLS_CC) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and internal position is no longer valid");
		return;
	}

	if (zend_hash_get_current_data_ex(aht, (void **) &entry, &intern->pos) == FAILURE) {
		return;
	}
	RETVAL_ZVAL(*entry, 1, 0);
}
/* }}} */

/* {{{ proto mixed|NULL ArrayIterator::key()
   Return current array key */
SPL_METHOD(Array, key)
{
	zval *object = getThis();
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	char *string_key;
	uint string_length;
	ulong num_key;
	HashTable *aht = HASH_OF(intern->array);

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and is no longer an array");
		return;
	}

	if (intern->is_ref && spl_hash_verify_pos(intern TSRMLS_CC) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and internal position is no longer valid");
		return;
	}

	switch (zend_hash_get_current_key_ex(aht, &string_key, &string_length, &num_key, 1, &intern->pos)) {
		case HASH_KEY_IS_STRING:
			RETVAL_STRINGL(string_key, string_length - 1, 0);
			break;
		case HASH_KEY_IS_LONG:
			RETVAL_LONG(num_key);
			break;
		case HASH_KEY_NON_EXISTANT:
			return;
	}
}
/* }}} */

/* {{{ proto void ArrayIterator::next()
   Move to next entry */
SPL_METHOD(Array, next)
{
	zval *object = getThis();
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	HashTable *aht = HASH_OF(intern->array);

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and is no longer an array");
		return;
	}

	spl_array_next(intern TSRMLS_CC);
}
/* }}} */

/* {{{ proto bool ArrayIterator::valid()
   Check whether array contains more entries */
SPL_METHOD(Array, valid)
{
	zval *object = getThis();
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	HashTable *aht = HASH_OF(intern->array);

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and is no longer an array");
		return;
	}

	if (intern->pos && intern->is_ref && spl_hash_verify_pos(intern TSRMLS_CC) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and internal position is no longer valid");
		RETURN_FALSE;
	} else {
		RETURN_BOOL(zend_hash_has_more_elements_ex(aht, &intern->pos) == SUCCESS);
	}
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION(spl_array) */
PHP_MINIT_FUNCTION(spl_array)
{
	REGISTER_SPL_STD_CLASS_EX(ArrayObject, spl_array_object_new, spl_funcs_ArrayObject);
	zend_class_implements(spl_ce_ArrayObject TSRMLS_CC, 1, zend_ce_aggregate);
	zend_class_implements(spl_ce_ArrayObject TSRMLS_CC, 1, zend_ce_arrayaccess);
	memcpy(&spl_handler_ArrayObject, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	spl_handler_ArrayObject.clone_obj = spl_array_object_clone;
	spl_handler_ArrayObject.read_dimension = spl_array_read_dimension;
	spl_handler_ArrayObject.write_dimension = spl_array_write_dimension;
	spl_handler_ArrayObject.unset_dimension = spl_array_unset_dimension;
	spl_handler_ArrayObject.has_dimension = spl_array_has_dimension;
	spl_handler_ArrayObject.get_properties = spl_array_get_properties;
	spl_handler_ArrayObject.count_elements = spl_array_object_count_elements;

	REGISTER_SPL_STD_CLASS_EX(ArrayIterator, spl_array_object_new, spl_funcs_ArrayIterator);
	zend_class_implements(spl_ce_ArrayIterator TSRMLS_CC, 1, zend_ce_iterator);
	zend_class_implements(spl_ce_ArrayIterator TSRMLS_CC, 1, zend_ce_arrayaccess);
	REGISTER_SPL_IMPLEMENTS(ArrayIterator, SeekableIterator);
	memcpy(&spl_handler_ArrayIterator, &spl_handler_ArrayObject, sizeof(zend_object_handlers));
	spl_ce_ArrayIterator->get_iterator = spl_array_get_iterator;

	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
