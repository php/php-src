/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2009 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
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
#include "spl_exceptions.h"

zend_object_handlers spl_handler_ArrayObject;
PHPAPI zend_class_entry  *spl_ce_ArrayObject;

zend_object_handlers spl_handler_ArrayIterator;
PHPAPI zend_class_entry  *spl_ce_ArrayIterator;
PHPAPI zend_class_entry  *spl_ce_RecursiveArrayIterator;

#define SPL_ARRAY_STD_PROP_LIST      0x00000001
#define SPL_ARRAY_ARRAY_AS_PROPS     0x00000002
#define SPL_ARRAY_OVERLOADED_REWIND  0x00010000
#define SPL_ARRAY_OVERLOADED_VALID   0x00020000
#define SPL_ARRAY_OVERLOADED_KEY     0x00040000
#define SPL_ARRAY_OVERLOADED_CURRENT 0x00080000
#define SPL_ARRAY_OVERLOADED_NEXT    0x00100000
#define SPL_ARRAY_IS_REF             0x01000000
#define SPL_ARRAY_IS_SELF            0x02000000
#define SPL_ARRAY_USE_OTHER          0x04000000
#define SPL_ARRAY_INT_MASK           0xFFFF0000
#define SPL_ARRAY_CLONE_MASK         0x03000007

typedef struct _spl_array_object {
	zend_object       std;
	zval              *array;
	zval              *retval;
	HashPosition      pos;
	int               ar_flags;
	int               is_self;
	zend_function *   fptr_offset_get;
	zend_function *   fptr_offset_set;
	zend_function *   fptr_offset_has;
	zend_function *   fptr_offset_del;
	zend_class_entry* ce_get_iterator;
} spl_array_object;

static inline HashTable *spl_array_get_hash_table(spl_array_object* intern, int check_std_props TSRMLS_DC) {
	if ((intern->ar_flags & SPL_ARRAY_IS_SELF) != 0) {
		return intern->std.properties;
	} else if ((intern->ar_flags & SPL_ARRAY_USE_OTHER) && (check_std_props == 0 || (intern->ar_flags & SPL_ARRAY_STD_PROP_LIST) == 0) && Z_TYPE_P(intern->array) == IS_OBJECT) {
		spl_array_object *other  = (spl_array_object*)zend_object_store_get_object(intern->array TSRMLS_CC);
		return spl_array_get_hash_table(other, check_std_props TSRMLS_CC);
	} else if ((intern->ar_flags & ((check_std_props ? SPL_ARRAY_STD_PROP_LIST : 0) | SPL_ARRAY_IS_SELF)) != 0) {
		return intern->std.properties;
	} else {
		return HASH_OF(intern->array);
	}
}

SPL_API int spl_hash_verify_pos(spl_array_object * intern TSRMLS_DC) /* {{{ */
{
	HashTable *ht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);
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
	zend_hash_internal_pointer_reset_ex(spl_array_get_hash_table(intern, 0 TSRMLS_CC), &intern->pos);
	return FAILURE;
}
/* }}} */

/* {{{ spl_array_object_free_storage */
static void spl_array_object_free_storage(void *object TSRMLS_DC)
{
	spl_array_object *intern = (spl_array_object *)object;

	zend_object_std_dtor(&intern->std TSRMLS_CC);

	zval_ptr_dtor(&intern->array);
	zval_ptr_dtor(&intern->retval);

	efree(object);
}
/* }}} */

zend_object_iterator *spl_array_get_iterator(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC);

/* {{{ spl_array_object_new */
static zend_object_value spl_array_object_new_ex(zend_class_entry *class_type, spl_array_object **obj, zval *orig, int clone_orig TSRMLS_DC)
{
	zend_object_value retval;
	spl_array_object *intern;
	zval *tmp;
	zend_class_entry * parent = class_type;
	int inherited = 0;

	intern = emalloc(sizeof(spl_array_object));
	memset(intern, 0, sizeof(spl_array_object));
	*obj = intern;
	ALLOC_INIT_ZVAL(intern->retval);

	zend_object_std_init(&intern->std, class_type TSRMLS_CC);
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	intern->ar_flags = 0;
	intern->ce_get_iterator = spl_ce_ArrayIterator;
	if (orig) {
		spl_array_object *other = (spl_array_object*)zend_object_store_get_object(orig TSRMLS_CC);

		intern->ar_flags &= ~ SPL_ARRAY_CLONE_MASK;
		intern->ar_flags |= (other->ar_flags & SPL_ARRAY_CLONE_MASK);
		intern->ce_get_iterator = other->ce_get_iterator;
		if (clone_orig) {
			intern->array = other->array;
			if (Z_OBJ_HT_P(orig) == &spl_handler_ArrayObject) {
				MAKE_STD_ZVAL(intern->array);
				array_init(intern->array);
				zend_hash_copy(HASH_OF(intern->array), HASH_OF(other->array), (copy_ctor_func_t) zval_add_ref, &tmp, sizeof(zval*));
			}
			if (Z_OBJ_HT_P(orig) == &spl_handler_ArrayIterator) {
				ZVAL_ADDREF(other->array);
			}
		} else {
			intern->array = orig;
			ZVAL_ADDREF(intern->array);
			intern->ar_flags |= SPL_ARRAY_IS_REF | SPL_ARRAY_USE_OTHER;
		}
	} else {
		MAKE_STD_ZVAL(intern->array);
		array_init(intern->array);
		intern->ar_flags &= ~SPL_ARRAY_IS_REF;
	}

	retval.handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t) spl_array_object_free_storage, NULL TSRMLS_CC);
	while (parent) {
		if (parent == spl_ce_ArrayIterator || parent == spl_ce_RecursiveArrayIterator) {
			retval.handlers = &spl_handler_ArrayIterator;
			class_type->get_iterator = spl_array_get_iterator;
			break;
		} else if (parent == spl_ce_ArrayObject) {
			retval.handlers = &spl_handler_ArrayObject;
			break;
		}
		parent = parent->parent;
		inherited = 1;
	}
	if (!parent) { /* this must never happen */
		php_error_docref(NULL TSRMLS_CC, E_COMPILE_ERROR, "Internal compiler error, Class is not child of ArrayObject or ArrayIterator");
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
	/* Cache iterator functions if ArrayIterator or derived. Check current's */
	/* cache since only current is always required */
	if (retval.handlers == &spl_handler_ArrayIterator) {
		if (!class_type->iterator_funcs.zf_current) {
			zend_hash_find(&class_type->function_table, "rewind",  sizeof("rewind"),  (void **) &class_type->iterator_funcs.zf_rewind);
			zend_hash_find(&class_type->function_table, "valid",   sizeof("valid"),   (void **) &class_type->iterator_funcs.zf_valid);
			zend_hash_find(&class_type->function_table, "key",     sizeof("key"),     (void **) &class_type->iterator_funcs.zf_key);
			zend_hash_find(&class_type->function_table, "current", sizeof("current"), (void **) &class_type->iterator_funcs.zf_current);
			zend_hash_find(&class_type->function_table, "next",    sizeof("next"),    (void **) &class_type->iterator_funcs.zf_next);
		}
		if (inherited) {
			if (class_type->iterator_funcs.zf_rewind->common.scope  != parent) intern->ar_flags |= SPL_ARRAY_OVERLOADED_REWIND;
			if (class_type->iterator_funcs.zf_valid->common.scope   != parent) intern->ar_flags |= SPL_ARRAY_OVERLOADED_VALID;
			if (class_type->iterator_funcs.zf_key->common.scope     != parent) intern->ar_flags |= SPL_ARRAY_OVERLOADED_KEY;
			if (class_type->iterator_funcs.zf_current->common.scope != parent) intern->ar_flags |= SPL_ARRAY_OVERLOADED_CURRENT;
			if (class_type->iterator_funcs.zf_next->common.scope    != parent) intern->ar_flags |= SPL_ARRAY_OVERLOADED_NEXT;
		}
	}

	zend_hash_internal_pointer_reset_ex(spl_array_get_hash_table(intern, 0 TSRMLS_CC), &intern->pos);
	return retval;
}
/* }}} */

/* {{{ spl_array_object_new */
static zend_object_value spl_array_object_new(zend_class_entry *class_type TSRMLS_DC)
{
	spl_array_object *tmp;
	return spl_array_object_new_ex(class_type, &tmp, NULL, 0 TSRMLS_CC);
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
	new_obj_val = spl_array_object_new_ex(old_object->ce, &intern, zobject, 1 TSRMLS_CC);
	new_object = &intern->std;

	zend_objects_clone_members(new_object, new_obj_val, old_object, handle TSRMLS_CC);

	return new_obj_val;
}
/* }}} */

static zval **spl_array_get_dimension_ptr_ptr(int check_inherited, zval *object, zval *offset, int type TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	zval **retval;
	long index;
	HashTable *ht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);

/*  We cannot get the pointer pointer so we don't allow it here for now
	if (check_inherited && intern->fptr_offset_get) {
		return zend_call_method_with_1_params(&object, Z_OBJCE_P(object), &intern->fptr_offset_get, "offsetGet", NULL, offset);
	}*/

	if (!offset) {
		return &EG(uninitialized_zval_ptr);
	}
	
	switch(Z_TYPE_P(offset)) {
	case IS_STRING:
		if (zend_symtable_find(ht, Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, (void **) &retval) == FAILURE) {
			if (type == BP_VAR_W || type == BP_VAR_RW) {
				zval *value;
				ALLOC_INIT_ZVAL(value);
				zend_symtable_update(ht, Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, (void**)&value, sizeof(void*), NULL);
				zend_symtable_find(ht, Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, (void **) &retval);
				return retval;
			} else {
				zend_error(E_NOTICE, "Undefined index:  %s", Z_STRVAL_P(offset));
				return &EG(uninitialized_zval_ptr);
			}
		} else {
			return retval;
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
		if (zend_hash_index_find(ht, index, (void **) &retval) == FAILURE) {
			if (type == BP_VAR_W || type == BP_VAR_RW) {
				zval *value;
				ALLOC_INIT_ZVAL(value);
				zend_hash_index_update(ht, index, (void**)&value, sizeof(void*), NULL);
				zend_hash_index_find(ht, index, (void **) &retval);
				return retval;
			} else {
				zend_error(E_NOTICE, "Undefined offset:  %ld", Z_LVAL_P(offset));
				return &EG(uninitialized_zval_ptr);
			}
		} else {
			return retval;
		}
		break;
	default:
		zend_error(E_WARNING, "Illegal offset type");
		return &EG(uninitialized_zval_ptr);
	}
} /* }}} */

static zval *spl_array_read_dimension_ex(int check_inherited, zval *object, zval *offset, int type TSRMLS_DC) /* {{{ */
{
	zval **ret;

	if (check_inherited) {
		spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
		if (intern->fptr_offset_get) {
			zval *rv;
			SEPARATE_ARG_IF_REF(offset);
			zend_call_method_with_1_params(&object, Z_OBJCE_P(object), &intern->fptr_offset_get, "offsetGet", &rv, offset);	
			zval_ptr_dtor(&offset);
			if (rv) {
				zval_ptr_dtor(&intern->retval);
				MAKE_STD_ZVAL(intern->retval);
				ZVAL_ZVAL(intern->retval, rv, 1, 1);
				return intern->retval;
			}
			return EG(uninitialized_zval_ptr);
		}
	}
	ret = spl_array_get_dimension_ptr_ptr(check_inherited, object, offset, type TSRMLS_CC);

	/* When in a write context,
	 * ZE has to be fooled into thinking this is in a reference set
	 * by separating (if necessary) and returning as an is_ref=1 zval (even if refcount == 1) */
	if ((type == BP_VAR_W || type == BP_VAR_RW) && !(*ret)->is_ref) {
		if ((*ret)->refcount > 1) {
			zval *newval;

			/* Separate */
			MAKE_STD_ZVAL(newval);
			*newval = **ret;
			zval_copy_ctor(newval);
			newval->refcount = 1;

			/* Replace */
			(*ret)->refcount--;
			*ret = newval;
		}

		(*ret)->is_ref = 1;
	}

	return *ret;
} /* }}} */

static zval *spl_array_read_dimension(zval *object, zval *offset, int type TSRMLS_DC) /* {{{ */
{
	return spl_array_read_dimension_ex(1, object, offset, type TSRMLS_CC);
} /* }}} */

static void spl_array_write_dimension_ex(int check_inherited, zval *object, zval *offset, zval *value TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	long index;

	if (check_inherited && intern->fptr_offset_set) {
		if (!offset) {
			ALLOC_INIT_ZVAL(offset);
		} else {
			SEPARATE_ARG_IF_REF(offset);
		}
		zend_call_method_with_2_params(&object, Z_OBJCE_P(object), &intern->fptr_offset_set, "offsetSet", NULL, offset, value);
		zval_ptr_dtor(&offset);
		return;
	}
	
	if (!offset) {
		value->refcount++;
		zend_hash_next_index_insert(spl_array_get_hash_table(intern, 0 TSRMLS_CC), (void**)&value, sizeof(void*), NULL);
		return;
	}
	switch(Z_TYPE_P(offset)) {
	case IS_STRING:
		if (*Z_STRVAL_P(offset) == '\0') {
			zend_throw_exception(spl_ce_InvalidArgumentException, "An offset must not begin with \\0 or be empty", 0 TSRMLS_CC);
			return;
		}
		value->refcount++;
		zend_symtable_update(spl_array_get_hash_table(intern, 0 TSRMLS_CC), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, (void**)&value, sizeof(void*), NULL);
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
		zend_hash_index_update(spl_array_get_hash_table(intern, 0 TSRMLS_CC), index, (void**)&value, sizeof(void*), NULL);
		return;
	case IS_NULL:
		value->refcount++;
		zend_hash_next_index_insert(spl_array_get_hash_table(intern, 0 TSRMLS_CC), (void**)&value, sizeof(void*), NULL);
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

	if (check_inherited && intern->fptr_offset_del) {
		SEPARATE_ARG_IF_REF(offset);
		zend_call_method_with_1_params(&object, Z_OBJCE_P(object), &intern->fptr_offset_del, "offsetUnset", NULL, offset);
		zval_ptr_dtor(&offset);
		return;
	}

	switch(Z_TYPE_P(offset)) {
	case IS_STRING:
		if (spl_array_get_hash_table(intern, 0 TSRMLS_CC) == &EG(symbol_table)) {
			if (zend_delete_global_variable(Z_STRVAL_P(offset), Z_STRLEN_P(offset) TSRMLS_CC)) {
				zend_error(E_NOTICE,"Undefined index:  %s", Z_STRVAL_P(offset));
			}
		} else {
			if (zend_symtable_del(spl_array_get_hash_table(intern, 0 TSRMLS_CC), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1) == FAILURE) {
				zend_error(E_NOTICE,"Undefined index:  %s", Z_STRVAL_P(offset));
			}
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
		if (zend_hash_index_del(spl_array_get_hash_table(intern, 0 TSRMLS_CC), index) == FAILURE) {
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
	zval *rv, **tmp;

	if (check_inherited && intern->fptr_offset_has) {
		SEPARATE_ARG_IF_REF(offset);
		zend_call_method_with_1_params(&object, Z_OBJCE_P(object), &intern->fptr_offset_has, "offsetExists", &rv, offset);
		zval_ptr_dtor(&offset);
		if (rv && zend_is_true(rv)) {
			zval_ptr_dtor(&rv);
			return 1;
		}
		if (rv) {
			zval_ptr_dtor(&rv);
		}
		return 0;
	}
	
	switch(Z_TYPE_P(offset)) {
	case IS_STRING:
		if (check_empty) {
			if (zend_symtable_find(spl_array_get_hash_table(intern, 0 TSRMLS_CC), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, (void **) &tmp) != FAILURE && zend_is_true(*tmp)) {
				return 1;
			}
			return 0;
		} else {
			return zend_symtable_exists(spl_array_get_hash_table(intern, 0 TSRMLS_CC), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1);
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
		if (check_empty) {
			HashTable *ht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);
			if (zend_hash_index_find(ht, index, (void **)&tmp) != FAILURE && zend_is_true(*tmp)) {
				return 1;
			}
			return 0;
		} else {
			return zend_hash_index_exists(spl_array_get_hash_table(intern, 0 TSRMLS_CC), index);
		}
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
	RETURN_BOOL(spl_array_has_dimension_ex(0, getThis(), index, 0 TSRMLS_CC));
} /* }}} */

/* {{{ proto mixed ArrayObject::offsetGet(mixed $index)
       proto mixed ArrayIterator::offsetGet(mixed $index)
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
	zval *index, *value;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &index, &value) == FAILURE) {
		return;
	}
	spl_array_write_dimension_ex(0, getThis(), index, value TSRMLS_CC);
} /* }}} */


void spl_array_iterator_append(zval *object, zval *append_value TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	HashTable *aht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and is no longer an array");
		return;
	}
	
	if (Z_TYPE_P(intern->array) == IS_OBJECT) {
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "Cannot append properties to objects, use %s::offsetSet() instead", Z_OBJCE_P(object)->name);
		return;
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
	zend_hash_copy(HASH_OF(return_value), spl_array_get_hash_table(intern, 0 TSRMLS_CC), (copy_ctor_func_t) zval_add_ref, &tmp, sizeof(zval*));
} /* }}} */

static HashTable *spl_array_get_properties(zval *object TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);

	return spl_array_get_hash_table(intern, 1 TSRMLS_CC);
} /* }}} */

static zval *spl_array_read_property(zval *object, zval *member, int type TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);

	if ((intern->ar_flags & SPL_ARRAY_ARRAY_AS_PROPS) != 0
	&& !std_object_handlers.has_property(object, member, 2 TSRMLS_CC)) {
		return spl_array_read_dimension(object, member, type TSRMLS_CC);
	}
	return std_object_handlers.read_property(object, member, type TSRMLS_CC);
} /* }}} */

static void spl_array_write_property(zval *object, zval *member, zval *value TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);

	if ((intern->ar_flags & SPL_ARRAY_ARRAY_AS_PROPS) != 0
	&& !std_object_handlers.has_property(object, member, 2 TSRMLS_CC)) {
		spl_array_write_dimension(object, member, value TSRMLS_CC);
		return;
	}
	std_object_handlers.write_property(object, member, value TSRMLS_CC);
} /* }}} */

static zval **spl_array_get_property_ptr_ptr(zval *object, zval *member TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);

	if ((intern->ar_flags & SPL_ARRAY_ARRAY_AS_PROPS) != 0
	&& !std_object_handlers.has_property(object, member, 2 TSRMLS_CC)) {
		return spl_array_get_dimension_ptr_ptr(1, object, member, 0 TSRMLS_CC);		
	}
	return std_object_handlers.get_property_ptr_ptr(object, member TSRMLS_CC);
} /* }}} */

static int spl_array_has_property(zval *object, zval *member, int has_set_exists TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);

	if ((intern->ar_flags & SPL_ARRAY_ARRAY_AS_PROPS) != 0) {
		if (!std_object_handlers.has_property(object, member, 2 TSRMLS_CC)) {
			return spl_array_has_dimension(object, member, has_set_exists TSRMLS_CC);
		}
		return 0; /* if prop doesn't exist at all mode 0/1 cannot return 1 */
	}
	return std_object_handlers.has_property(object, member, has_set_exists TSRMLS_CC);
} /* }}} */

static void spl_array_rewind(spl_array_object *intern TSRMLS_DC);

static void spl_array_unset_property(zval *object, zval *member TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);

	if ((intern->ar_flags & SPL_ARRAY_ARRAY_AS_PROPS) != 0
	&& !std_object_handlers.has_property(object, member, 2 TSRMLS_CC)) {
		spl_array_unset_dimension(object, member TSRMLS_CC);
		spl_array_rewind(intern TSRMLS_CC); /* because deletion might invalidate position */
		return;
	}
	std_object_handlers.unset_property(object, member TSRMLS_CC);
} /* }}} */

static int spl_array_skip_protected(spl_array_object *intern TSRMLS_DC) /* {{{ */
{
	char *string_key;
	uint string_length;
	ulong num_key;
	HashTable *aht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);

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
	HashTable *aht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);

	if ((intern->ar_flags & SPL_ARRAY_IS_REF) && spl_hash_verify_pos(intern TSRMLS_CC) == FAILURE) {
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
	zend_user_iterator    intern;
	spl_array_object      *object;
} spl_array_it;

static void spl_array_it_dtor(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_array_it *iterator = (spl_array_it *)iter;

	zend_user_it_invalidate_current(iter TSRMLS_CC);
	zval_ptr_dtor((zval**)&iterator->intern.it.data);

	efree(iterator);
}
/* }}} */
	
static int spl_array_it_valid(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_array_it       *iterator = (spl_array_it *)iter;
	spl_array_object   *object   = iterator->object;
	HashTable          *aht      = spl_array_get_hash_table(object, 0 TSRMLS_CC);

	if (object->ar_flags & SPL_ARRAY_OVERLOADED_VALID) {
		return zend_user_it_valid(iter TSRMLS_CC);
	} else {
		if (!aht) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "ArrayIterator::valid(): Array was modified outside object and is no longer an array");
			return FAILURE;
		}
	
		if (object->pos && (object->ar_flags & SPL_ARRAY_IS_REF) && spl_hash_verify_pos(object TSRMLS_CC) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "ArrayIterator::valid(): Array was modified outside object and internal position is no longer valid");
			return FAILURE;
		} else {
			return zend_hash_has_more_elements_ex(aht, &object->pos);
		}
	}
}
/* }}} */

static void spl_array_it_get_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC) /* {{{ */
{
	spl_array_it       *iterator = (spl_array_it *)iter;
	spl_array_object   *object   = iterator->object;
	HashTable          *aht      = spl_array_get_hash_table(object, 0 TSRMLS_CC);

	if (object->ar_flags & SPL_ARRAY_OVERLOADED_CURRENT) {
		zend_user_it_get_current_data(iter, data TSRMLS_CC);
	} else {
		if (zend_hash_get_current_data_ex(aht, (void**)data, &object->pos) == FAILURE) {
			*data = NULL;
		}
	}
}
/* }}} */

static int spl_array_it_get_current_key(zend_object_iterator *iter, char **str_key, uint *str_key_len, ulong *int_key TSRMLS_DC) /* {{{ */
{
	spl_array_it       *iterator = (spl_array_it *)iter;
	spl_array_object   *object   = iterator->object;
	HashTable          *aht      = spl_array_get_hash_table(object, 0 TSRMLS_CC);

	if (object->ar_flags & SPL_ARRAY_OVERLOADED_KEY) {
		return zend_user_it_get_current_key(iter, str_key, str_key_len, int_key TSRMLS_CC);
	} else {
		if (!aht) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "ArrayIterator::current(): Array was modified outside object and is no longer an array");
			return HASH_KEY_NON_EXISTANT;
		}
	
		if ((object->ar_flags & SPL_ARRAY_IS_REF) && spl_hash_verify_pos(object TSRMLS_CC) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "ArrayIterator::current(): Array was modified outside object and internal position is no longer valid");
			return HASH_KEY_NON_EXISTANT;
		}
	
		return zend_hash_get_current_key_ex(aht, str_key, str_key_len, int_key, 1, &object->pos);
	}
}
/* }}} */

static void spl_array_it_move_forward(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_array_it       *iterator = (spl_array_it *)iter;
	spl_array_object   *object   = iterator->object;
	HashTable          *aht      = spl_array_get_hash_table(object, 0 TSRMLS_CC);

	if (object->ar_flags & SPL_ARRAY_OVERLOADED_NEXT) {
		zend_user_it_move_forward(iter TSRMLS_CC);
	} else {
		zend_user_it_invalidate_current(iter TSRMLS_CC);
		if (!aht) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "ArrayIterator::current(): Array was modified outside object and is no longer an array");
			return;
		}
	
		if ((object->ar_flags & SPL_ARRAY_IS_REF) && spl_hash_verify_pos(object TSRMLS_CC) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "ArrayIterator::next(): Array was modified outside object and internal position is no longer valid");
		} else {
			spl_array_next(object TSRMLS_CC);
		}
	}
}
/* }}} */

static void spl_array_rewind(spl_array_object *intern TSRMLS_DC) /* {{{ */
{
	HashTable          *aht      = spl_array_get_hash_table(intern, 0 TSRMLS_CC);

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

	if (object->ar_flags & SPL_ARRAY_OVERLOADED_REWIND) {
		zend_user_it_rewind(iter TSRMLS_CC);
	} else {
		zend_user_it_invalidate_current(iter TSRMLS_CC);
		spl_array_rewind(object TSRMLS_CC);
	}
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

zend_object_iterator *spl_array_get_iterator(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC) /* {{{ */
{
	spl_array_it       *iterator;
	spl_array_object   *array_object = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);

	if (by_ref && (array_object->ar_flags & SPL_ARRAY_OVERLOADED_CURRENT)) {
		zend_error(E_ERROR, "An iterator cannot be used with foreach by reference");
	}

	iterator     = emalloc(sizeof(spl_array_it));

	object->refcount++;
	iterator->intern.it.data = (void*)object;
	iterator->intern.it.funcs = &spl_array_it_funcs;
	iterator->intern.ce = ce;
	iterator->intern.value = NULL;
	iterator->object = array_object;
	
	return (zend_object_iterator*)iterator;
}
/* }}} */

/* {{{ proto void ArrayObject::__construct(array|object ar = array() [, int flags = 0 [, string iterator_class = "ArrayIterator"]])
       proto void ArrayIterator::__construct(array|object ar = array() [, int flags = 0])
 Cronstructs a new array iterator from a path. */
SPL_METHOD(Array, __construct)
{
	zval *object = getThis();
	spl_array_object *intern;
	zval **array;
	long ar_flags = 0;
	zend_class_entry * ce_get_iterator = spl_ce_Iterator;

	if (ZEND_NUM_ARGS() == 0) {
		return; /* nothing to do */
	}
	php_set_error_handling(EH_THROW, spl_ce_InvalidArgumentException TSRMLS_CC);

	intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z|lC", &array, &ar_flags, &ce_get_iterator) == FAILURE) {
		php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
		return;
	}

	if (Z_TYPE_PP(array) == IS_ARRAY) {
		SEPARATE_ZVAL_IF_NOT_REF(array);
	}

	if (ZEND_NUM_ARGS() > 2) {
		intern->ce_get_iterator = ce_get_iterator;
	}

	ar_flags &= ~SPL_ARRAY_INT_MASK;

	if (Z_TYPE_PP(array) == IS_OBJECT && (Z_OBJ_HT_PP(array) == &spl_handler_ArrayObject || Z_OBJ_HT_PP(array) == &spl_handler_ArrayIterator)) {
		zval_ptr_dtor(&intern->array);
		if (ZEND_NUM_ARGS() == 1)
		{
			spl_array_object *other = (spl_array_object*)zend_object_store_get_object(*array TSRMLS_CC);
			ar_flags = other->ar_flags & ~SPL_ARRAY_INT_MASK;
		}		
		ar_flags |= SPL_ARRAY_USE_OTHER;
		intern->array = *array;
	} else {
		if (Z_TYPE_PP(array) != IS_OBJECT && Z_TYPE_PP(array) != IS_ARRAY) {
			php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
			zend_throw_exception(spl_ce_InvalidArgumentException, "Passed variable is not an array or object, using empty array instead", 0 TSRMLS_CC);
			return;
		}
		zval_ptr_dtor(&intern->array);
		intern->array = *array;
	}
	if (object == *array) {
		intern->ar_flags |= SPL_ARRAY_IS_SELF;
		intern->ar_flags &= ~SPL_ARRAY_USE_OTHER;
	} else {
		intern->ar_flags &= ~SPL_ARRAY_IS_SELF;
	}
	intern->ar_flags |= ar_flags;
	ZVAL_ADDREF(intern->array);
	if (Z_TYPE_PP(array) == IS_OBJECT) {
		zend_object_get_properties_t handler = Z_OBJ_HANDLER_PP(array, get_properties);
		if ((handler != std_object_handlers.get_properties && handler != spl_array_get_properties)
		|| !spl_array_get_hash_table(intern, 0 TSRMLS_CC)) {
			php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
			zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "Overloaded object of type %s is not compatible with %s", Z_OBJCE_PP(array)->name, intern->std.ce->name);
			return;
		}
	}

	spl_array_rewind(intern TSRMLS_CC);

	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
}
/* }}} */

/* {{{ proto void ArrayObject::setIteratorClass(string iterator_class)
   Set the class used in getIterator. */
SPL_METHOD(Array, setIteratorClass)
{
	zval *object = getThis();
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	zend_class_entry * ce_get_iterator = spl_ce_Iterator;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "C", &ce_get_iterator) == FAILURE) {
		php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
		return;
	}

	intern->ce_get_iterator = ce_get_iterator;
}
/* }}} */

/* {{{ proto string ArrayObject::getIteratorClass()
   Get the class used in getIterator. */
SPL_METHOD(Array, getIteratorClass)
{
	zval *object = getThis();
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);

	RETURN_STRING(intern->ce_get_iterator->name, 1);
}
/* }}} */

/* {{{ proto int ArrayObject::getFlags()
   Get flags */
SPL_METHOD(Array, getFlags)
{
	zval *object = getThis();
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	
	RETURN_LONG(intern->ar_flags & ~SPL_ARRAY_INT_MASK);
}
/* }}} */

/* {{{ proto void ArrayObject::setFlags(int flags)
   Set flags */
SPL_METHOD(Array, setFlags)
{
	zval *object = getThis();
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	long ar_flags = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &ar_flags) == FAILURE) {
		return;
	}
	
	intern->ar_flags = (intern->ar_flags & SPL_ARRAY_INT_MASK) | (ar_flags & ~SPL_ARRAY_INT_MASK);
}
/* }}} */

/* {{{ proto Array|Object ArrayObject::exchangeArray(Array|Object ar = array())
   Replace the referenced array or object with a new one and return the old one (right now copy - to be changed) */
SPL_METHOD(Array, exchangeArray)
{
	zval *object = getThis(), *tmp, **array;
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);

	array_init(return_value);
	zend_hash_copy(HASH_OF(return_value), spl_array_get_hash_table(intern, 0 TSRMLS_CC), (copy_ctor_func_t) zval_add_ref, &tmp, sizeof(zval*));
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &array) == FAILURE) {
		return;
	}
	if (Z_TYPE_PP(array) == IS_OBJECT && intern == (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC)) {
		zval_ptr_dtor(&intern->array);
		array = &object;
		intern->array = object;
	} else if (Z_TYPE_PP(array) == IS_OBJECT && (Z_OBJ_HT_PP(array) == &spl_handler_ArrayObject || Z_OBJ_HT_PP(array) == &spl_handler_ArrayIterator)) {
		spl_array_object *other  = (spl_array_object*)zend_object_store_get_object(*array TSRMLS_CC);
		zval_ptr_dtor(&intern->array);
		intern->array = other->array;
	} else {
		if (Z_TYPE_PP(array) != IS_OBJECT && !HASH_OF(*array)) {
			zend_throw_exception(spl_ce_InvalidArgumentException, "Passed variable is not an array or object, using empty array instead", 0 TSRMLS_CC);
			return;
		}
		zval_ptr_dtor(&intern->array);
		intern->array = *array;
		intern->ar_flags &= ~SPL_ARRAY_USE_OTHER;
	}
	if (object == *array) {
		intern->ar_flags |= SPL_ARRAY_IS_SELF;
	} else {
		intern->ar_flags &= ~SPL_ARRAY_IS_SELF;
	}
	ZVAL_ADDREF(intern->array);

	spl_array_rewind(intern TSRMLS_CC);
}
/* }}} */

/* {{{ proto ArrayIterator ArrayObject::getIterator()
   Create a new iterator from a ArrayObject instance */
SPL_METHOD(Array, getIterator)
{
	zval *object = getThis();
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	spl_array_object *iterator;
	HashTable *aht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and is no longer an array");
		return;
	}

	return_value->type = IS_OBJECT;
	return_value->value.obj = spl_array_object_new_ex(intern->ce_get_iterator, &iterator, object, 0 TSRMLS_CC);
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
	HashTable *aht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);
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
		result = SUCCESS;
		
		while (position-- > 0 && (result = spl_array_next(intern TSRMLS_CC)) == SUCCESS);
	
		if (result == SUCCESS && zend_hash_has_more_elements_ex(aht, &intern->pos) == SUCCESS) {
			return; /* ok */
		}
	}
	zend_throw_exception_ex(spl_ce_OutOfBoundsException, 0 TSRMLS_CC, "Seek position %ld is out of range", opos);
} /* }}} */

int spl_array_object_count_elements(zval *object, long *count TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	HashTable *aht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);
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

static void spl_array_method(INTERNAL_FUNCTION_PARAMETERS, char *fname, int fname_len, int use_arg)
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	HashTable *aht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);
	zval tmp, *arg;
	zval *retval_ptr = NULL;
	
	INIT_PZVAL(&tmp);
	Z_TYPE(tmp) = IS_ARRAY;
	Z_ARRVAL(tmp) = aht;
	
	if (use_arg) {
		if (ZEND_NUM_ARGS() != 1 || zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "z", &arg) == FAILURE) {
			zend_throw_exception(spl_ce_BadMethodCallException, "Function expects exactly one argument", 0 TSRMLS_CC);
			return;
		}
		zend_call_method(NULL, NULL, NULL, fname, fname_len, &retval_ptr, 2, &tmp, arg TSRMLS_CC);
	} else {
		zend_call_method(NULL, NULL, NULL, fname, fname_len, &retval_ptr, 1, &tmp, NULL TSRMLS_CC);
	}
	if (retval_ptr) {
		COPY_PZVAL_TO_ZVAL(*return_value, retval_ptr);
	}
}

#define SPL_ARRAY_METHOD(cname, fname, use_arg) \
SPL_METHOD(cname, fname) \
{ \
	spl_array_method(INTERNAL_FUNCTION_PARAM_PASSTHRU, #fname, sizeof(#fname)-1, use_arg); \
}

/* {{{ proto int ArrayObject::asort()
       proto int ArrayIterator::asort()
 Sort the entries by values. */
SPL_ARRAY_METHOD(Array, asort, 0)

/* {{{ proto int ArrayObject::ksort()
       proto int ArrayIterator::ksort()
 Sort the entries by key. */
SPL_ARRAY_METHOD(Array, ksort, 0)

/* {{{ proto int ArrayObject::uasort(callback cmp_function)
       proto int ArrayIterator::uasort(callback cmp_function)
 Sort the entries by values user defined function. */
SPL_ARRAY_METHOD(Array, uasort, 1)

/* {{{ proto int ArrayObject::uksort(callback cmp_function)
       proto int ArrayIterator::uksort(callback cmp_function)
 Sort the entries by key using user defined function. */
SPL_ARRAY_METHOD(Array, uksort, 1)

/* {{{ proto int ArrayObject::natsort()
       proto int ArrayIterator::natsort()
 Sort the entries by values using "natural order" algorithm. */
SPL_ARRAY_METHOD(Array, natsort, 0)

/* {{{ proto int ArrayObject::natcasesort()
       proto int ArrayIterator::natcasesort()
 Sort the entries by key using case insensitive "natural order" algorithm. */
SPL_ARRAY_METHOD(Array, natcasesort, 0)

/* {{{ proto mixed|NULL ArrayIterator::current()
   Return current array entry */
SPL_METHOD(Array, current)
{
	zval *object = getThis();
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	zval **entry;
	HashTable *aht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and is no longer an array");
		return;
	}

	if ((intern->ar_flags & SPL_ARRAY_IS_REF) && spl_hash_verify_pos(intern TSRMLS_CC) == FAILURE) {
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
	spl_array_iterator_key(getThis(), return_value TSRMLS_CC);
}

void spl_array_iterator_key(zval *object, zval *return_value TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	char *string_key;
	uint string_length;
	ulong num_key;
	HashTable *aht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and is no longer an array");
		return;
	}

	if ((intern->ar_flags & SPL_ARRAY_IS_REF) && spl_hash_verify_pos(intern TSRMLS_CC) == FAILURE) {
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
	HashTable *aht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);

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
	HashTable *aht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and is no longer an array");
		return;
	}

	if (intern->pos && (intern->ar_flags & SPL_ARRAY_IS_REF) && spl_hash_verify_pos(intern TSRMLS_CC) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and internal position is no longer valid");
		RETURN_FALSE;
	} else {
		RETURN_BOOL(zend_hash_has_more_elements_ex(aht, &intern->pos) == SUCCESS);
	}
}
/* }}} */

/* {{{ proto bool RecursiveArrayIterator::hasChildren()
   Check whether current element has children (e.g. is an array) */
SPL_METHOD(Array, hasChildren)
{
	zval *object = getThis(), **entry;
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	HashTable *aht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);
	
	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and is no longer an array");
		RETURN_FALSE;
	}

	if ((intern->ar_flags & SPL_ARRAY_IS_REF) && spl_hash_verify_pos(intern TSRMLS_CC) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and internal position is no longer valid");
		RETURN_FALSE;
	}

	if (zend_hash_get_current_data_ex(aht, (void **) &entry, &intern->pos) == FAILURE) {
		RETURN_FALSE;
	}

	RETURN_BOOL(Z_TYPE_PP(entry) == IS_ARRAY || Z_TYPE_PP(entry) == IS_OBJECT);
}
/* }}} */

/* {{{ proto object RecursiveArrayIterator::getChildren()
   Create a sub iterator for the current element (same class as $this) */
SPL_METHOD(Array, getChildren)
{
	zval *object = getThis(), **entry, *flags;
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	HashTable *aht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and is no longer an array");
		return;
	}

	if ((intern->ar_flags & SPL_ARRAY_IS_REF) && spl_hash_verify_pos(intern TSRMLS_CC) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and internal position is no longer valid");
		return;
	}

	if (zend_hash_get_current_data_ex(aht, (void **) &entry, &intern->pos) == FAILURE) {
		return;
	}

	if (Z_TYPE_PP(entry) == IS_OBJECT && instanceof_function(Z_OBJCE_PP(entry), Z_OBJCE_P(getThis()) TSRMLS_CC)) {
		RETURN_ZVAL(*entry, 0, 0);
	}

  MAKE_STD_ZVAL(flags);
  ZVAL_LONG(flags, SPL_ARRAY_USE_OTHER);
	spl_instantiate_arg_ex2(intern->std.ce, &return_value, 0, *entry, flags TSRMLS_CC);
	zval_ptr_dtor(&flags);
}
/* }}} */

static
ZEND_BEGIN_ARG_INFO(arginfo_array___construct, 0)
	ZEND_ARG_INFO(0, array)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_array_offsetGet, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_array_offsetSet, 0, 0, 2)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_INFO(0, newval)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_array_append, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_array_seek, 0)
	ZEND_ARG_INFO(0, position)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_array_exchangeArray, 0)
	ZEND_ARG_INFO(0, array)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_array_setFlags, 0)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_array_setIteratorClass, 0)
	ZEND_ARG_INFO(0, iteratorClass)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_array_uXsort, 0)
	ZEND_ARG_INFO(0, cmp_function )
ZEND_END_ARG_INFO();

static zend_function_entry spl_funcs_ArrayObject[] = {
	SPL_ME(Array, __construct,      arginfo_array___construct,      ZEND_ACC_PUBLIC)
	SPL_ME(Array, offsetExists,     arginfo_array_offsetGet,        ZEND_ACC_PUBLIC)
	SPL_ME(Array, offsetGet,        arginfo_array_offsetGet,        ZEND_ACC_PUBLIC)
	SPL_ME(Array, offsetSet,        arginfo_array_offsetSet,        ZEND_ACC_PUBLIC)
	SPL_ME(Array, offsetUnset,      arginfo_array_offsetGet,        ZEND_ACC_PUBLIC)
	SPL_ME(Array, append,           arginfo_array_append,           ZEND_ACC_PUBLIC)
	SPL_ME(Array, getArrayCopy,     NULL,                           ZEND_ACC_PUBLIC)
	SPL_ME(Array, count,            NULL,                           ZEND_ACC_PUBLIC)
	SPL_ME(Array, getFlags,         NULL,                           ZEND_ACC_PUBLIC)
	SPL_ME(Array, setFlags,         arginfo_array_setFlags,         ZEND_ACC_PUBLIC)
	SPL_ME(Array, asort,            NULL,                           ZEND_ACC_PUBLIC)
	SPL_ME(Array, ksort,            NULL,                           ZEND_ACC_PUBLIC)
	SPL_ME(Array, uasort,           arginfo_array_uXsort,           ZEND_ACC_PUBLIC)
	SPL_ME(Array, uksort,           arginfo_array_uXsort,           ZEND_ACC_PUBLIC)
	SPL_ME(Array, natsort,          NULL,                           ZEND_ACC_PUBLIC)
	SPL_ME(Array, natcasesort,      NULL,                           ZEND_ACC_PUBLIC)
	/* ArrayObject specific */
	SPL_ME(Array, getIterator,      NULL,                           ZEND_ACC_PUBLIC)
	SPL_ME(Array, exchangeArray,    arginfo_array_exchangeArray,    ZEND_ACC_PUBLIC)
	SPL_ME(Array, setIteratorClass, arginfo_array_setIteratorClass, ZEND_ACC_PUBLIC)
	SPL_ME(Array, getIteratorClass, NULL,                           ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static zend_function_entry spl_funcs_ArrayIterator[] = {
	SPL_ME(Array, __construct,      arginfo_array___construct,      ZEND_ACC_PUBLIC)
	SPL_ME(Array, offsetExists,     arginfo_array_offsetGet,        ZEND_ACC_PUBLIC)
	SPL_ME(Array, offsetGet,        arginfo_array_offsetGet,        ZEND_ACC_PUBLIC)
	SPL_ME(Array, offsetSet,        arginfo_array_offsetSet,        ZEND_ACC_PUBLIC)
	SPL_ME(Array, offsetUnset,      arginfo_array_offsetGet,        ZEND_ACC_PUBLIC)
	SPL_ME(Array, append,           arginfo_array_append,           ZEND_ACC_PUBLIC)
	SPL_ME(Array, getArrayCopy,     NULL,                           ZEND_ACC_PUBLIC)
	SPL_ME(Array, count,            NULL,                           ZEND_ACC_PUBLIC)
	SPL_ME(Array, getFlags,         NULL,                           ZEND_ACC_PUBLIC)
	SPL_ME(Array, setFlags,         arginfo_array_setFlags,         ZEND_ACC_PUBLIC)
	SPL_ME(Array, asort,            NULL,                           ZEND_ACC_PUBLIC)
	SPL_ME(Array, ksort,            NULL,                           ZEND_ACC_PUBLIC)
	SPL_ME(Array, uasort,           arginfo_array_uXsort,           ZEND_ACC_PUBLIC)
	SPL_ME(Array, uksort,           arginfo_array_uXsort,           ZEND_ACC_PUBLIC)
	SPL_ME(Array, natsort,          NULL,                           ZEND_ACC_PUBLIC)
	SPL_ME(Array, natcasesort,      NULL,                           ZEND_ACC_PUBLIC)
	/* ArrayIterator specific */
	SPL_ME(Array, rewind,           NULL,                           ZEND_ACC_PUBLIC)
	SPL_ME(Array, current,          NULL,                           ZEND_ACC_PUBLIC)
	SPL_ME(Array, key,              NULL,                           ZEND_ACC_PUBLIC)
	SPL_ME(Array, next,             NULL,                           ZEND_ACC_PUBLIC)
	SPL_ME(Array, valid,            NULL,                           ZEND_ACC_PUBLIC)
	SPL_ME(Array, seek,             arginfo_array_seek,             ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static zend_function_entry spl_funcs_RecursiveArrayIterator[] = {
	SPL_ME(Array, hasChildren,   NULL, ZEND_ACC_PUBLIC)
	SPL_ME(Array, getChildren,   NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

/* {{{ PHP_MINIT_FUNCTION(spl_array) */
PHP_MINIT_FUNCTION(spl_array)
{
	REGISTER_SPL_STD_CLASS_EX(ArrayObject, spl_array_object_new, spl_funcs_ArrayObject);
	REGISTER_SPL_IMPLEMENTS(ArrayObject, Aggregate);
	REGISTER_SPL_IMPLEMENTS(ArrayObject, ArrayAccess);
	memcpy(&spl_handler_ArrayObject, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

	spl_handler_ArrayObject.clone_obj = spl_array_object_clone;
	spl_handler_ArrayObject.read_dimension = spl_array_read_dimension;
	spl_handler_ArrayObject.write_dimension = spl_array_write_dimension;
	spl_handler_ArrayObject.unset_dimension = spl_array_unset_dimension;
	spl_handler_ArrayObject.has_dimension = spl_array_has_dimension;
	spl_handler_ArrayObject.count_elements = spl_array_object_count_elements;

	spl_handler_ArrayObject.get_properties = spl_array_get_properties;
	spl_handler_ArrayObject.read_property = spl_array_read_property;
	spl_handler_ArrayObject.write_property = spl_array_write_property;
	spl_handler_ArrayObject.get_property_ptr_ptr = spl_array_get_property_ptr_ptr;
	spl_handler_ArrayObject.has_property = spl_array_has_property;
	spl_handler_ArrayObject.unset_property = spl_array_unset_property;

	REGISTER_SPL_STD_CLASS_EX(ArrayIterator, spl_array_object_new, spl_funcs_ArrayIterator);
	REGISTER_SPL_IMPLEMENTS(ArrayIterator, Iterator);
	REGISTER_SPL_IMPLEMENTS(ArrayIterator, ArrayAccess);
	REGISTER_SPL_IMPLEMENTS(ArrayIterator, SeekableIterator);
	memcpy(&spl_handler_ArrayIterator, &spl_handler_ArrayObject, sizeof(zend_object_handlers));
	spl_ce_ArrayIterator->get_iterator = spl_array_get_iterator;
	
	REGISTER_SPL_SUB_CLASS_EX(RecursiveArrayIterator, ArrayIterator, spl_array_object_new, spl_funcs_RecursiveArrayIterator);
	REGISTER_SPL_IMPLEMENTS(RecursiveArrayIterator, RecursiveIterator);
	spl_ce_RecursiveArrayIterator->get_iterator = spl_array_get_iterator;

	REGISTER_SPL_IMPLEMENTS(ArrayObject, Countable);
	REGISTER_SPL_IMPLEMENTS(ArrayIterator, Countable);

	REGISTER_SPL_CLASS_CONST_LONG(ArrayObject,   "STD_PROP_LIST",    SPL_ARRAY_STD_PROP_LIST);
	REGISTER_SPL_CLASS_CONST_LONG(ArrayObject,   "ARRAY_AS_PROPS",   SPL_ARRAY_ARRAY_AS_PROPS);

	REGISTER_SPL_CLASS_CONST_LONG(ArrayIterator, "STD_PROP_LIST",    SPL_ARRAY_STD_PROP_LIST);
	REGISTER_SPL_CLASS_CONST_LONG(ArrayIterator, "ARRAY_AS_PROPS",   SPL_ARRAY_ARRAY_AS_PROPS);
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
