/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_interfaces.h"

#include "php_spl.h"
#include "spl_functions.h"
#include "spl_engine.h"
#include "spl_array.h"


static
ZEND_BEGIN_ARG_INFO(arginfo_one_param, 0)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO();

static
ZEND_BEGIN_ARG_INFO(arginfo_two_params, 0)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO();

function_entry spl_funcs_ArrayRead[] = {
	SPL_ABSTRACT_ME(ArrayRead, get,     arginfo_one_param)
	SPL_ABSTRACT_ME(ArrayRead, exists,  arginfo_one_param)
	{NULL, NULL, NULL}
};

function_entry spl_funcs_ArrayAccess[] = {
	SPL_ABSTRACT_ME(ArrayAccess, set,  arginfo_two_params)
	SPL_ABSTRACT_ME(ArrayAccess, del,  arginfo_one_param)
	{NULL, NULL, NULL}
};

SPL_METHOD(Array, __construct);
SPL_METHOD(Array, getIterator);
SPL_METHOD(Array, rewind);
SPL_METHOD(Array, current);
SPL_METHOD(Array, key);
SPL_METHOD(Array, next);
SPL_METHOD(Array, hasMore);

static
ZEND_BEGIN_ARG_INFO(arginfo_array___construct, 0)
	ZEND_ARG_INFO(0, array)
ZEND_END_ARG_INFO();

static zend_function_entry spl_funcs_ArrayObject[] = {
	SPL_ME(Array, __construct,   arginfo_array___construct, ZEND_ACC_PUBLIC)
	SPL_ME(Array, getIterator,   NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static zend_function_entry spl_funcs_ArrayIterator[] = {
	SPL_ME(Array, __construct,   arginfo_array___construct, ZEND_ACC_PRIVATE)
	SPL_ME(Array, rewind,        NULL, ZEND_ACC_PUBLIC)
	SPL_ME(Array, current,       NULL, ZEND_ACC_PUBLIC)
	SPL_ME(Array, key,           NULL, ZEND_ACC_PUBLIC)
	SPL_ME(Array, next,          NULL, ZEND_ACC_PUBLIC)
	SPL_ME(Array, hasMore,       NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

zend_class_entry *   spl_ce_ArrayRead;
zend_class_entry *   spl_ce_ArrayAccess;

zend_object_handlers spl_handler_ArrayObject;
zend_class_entry *   spl_ce_ArrayObject;

zend_object_handlers spl_handler_ArrayIterator;
zend_class_entry *   spl_ce_ArrayIterator;

typedef struct _spl_array_object {
	zend_object       std;
	zval              *array;
	HashPosition      pos;
} spl_array_object;

/* {{{ spl_array_object_dtor */
static void spl_array_object_dtor(void *object, zend_object_handle handle TSRMLS_DC)
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
	} else {
		MAKE_STD_ZVAL(intern->array);
		array_init(intern->array);
	}
	zend_hash_internal_pointer_reset_ex(HASH_OF(intern->array), &intern->pos);

	retval.handle = zend_objects_store_put(intern, spl_array_object_dtor, NULL TSRMLS_CC);
	if (class_type == spl_ce_ArrayIterator) {
		retval.handlers = &spl_handler_ArrayIterator;
	} else {
		retval.handlers = &spl_handler_ArrayObject;
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

/* {{{ spl_array_read_dimension */
static zval *spl_array_read_dimension(zval *object, zval *offset TSRMLS_DC)
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	zval **retval;
	long index;

	switch(Z_TYPE_P(offset)) {
	case IS_STRING:
		if (zend_symtable_find(HASH_OF(intern->array), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1, (void **) &retval) == FAILURE) {
			zend_error(E_NOTICE,"Undefined index:  %s", Z_STRVAL_P(offset));
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
			zend_error(E_NOTICE,"Undefined offset:  %ld", Z_LVAL_P(offset));
			return EG(uninitialized_zval_ptr);
		} else {
			return *retval;
		}
		break;
	default:
		zend_error(E_WARNING, "Illegal offset type");
		return EG(uninitialized_zval_ptr);
	}
}
/* }}} */

/* {{{ spl_array_write_dimension */
static void spl_array_write_dimension(zval *object, zval *offset, zval *value TSRMLS_DC)
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	long index;

	switch(Z_TYPE_P(offset)) {
	case IS_STRING:
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
		add_index_zval(intern->array, index, value);
		return;
	default:
		zend_error(E_WARNING, "Illegal offset type");
		return;
	}
}
/* }}} */

/* {{{ spl_array_unset_dimension */
static void spl_array_unset_dimension(zval *object, zval *offset TSRMLS_DC)
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	long index;

	switch(Z_TYPE_P(offset)) {
	case IS_STRING:
		if (zend_symtable_del(HASH_OF(intern->array), Z_STRVAL_P(offset), Z_STRLEN_P(offset)+1) == FAILURE) {
			zend_error(E_NOTICE,"Undefined index:  %s", Z_STRVAL_P(offset));
		}
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
		if (zend_hash_index_del(HASH_OF(intern->array), index) == FAILURE) {
			zend_error(E_NOTICE,"Undefined offset:  %ld", Z_LVAL_P(offset));
		}
		return;
	default:
		zend_error(E_WARNING, "Illegal offset type");
		return;
	}
}
/* }}} */

/* {{{ spl_array_has_dimension */
static int spl_array_has_dimension(zval *object, zval *offset, int check_empty TSRMLS_DC)
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	long index;

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
}
/* }}} */

/* {{{ spl_array_get_properties */
static HashTable *spl_array_get_properties(zval *object TSRMLS_DC)
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);

	return HASH_OF(intern->array);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION(spl_array) */
PHP_MINIT_FUNCTION(spl_array)
{
	REGISTER_SPL_INTERFACE(ArrayRead);

	REGISTER_SPL_INTERFACE(ArrayAccess);
	zend_class_implements(spl_ce_ArrayAccess TSRMLS_CC, 1, spl_ce_ArrayRead);

	REGISTER_SPL_STD_CLASS_EX(ArrayObject, spl_array_object_new, spl_funcs_ArrayObject);
	zend_class_implements(spl_ce_ArrayObject TSRMLS_CC, 1, zend_ce_aggregate);
	memcpy(&spl_handler_ArrayObject, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	spl_handler_ArrayObject.clone_obj = spl_array_object_clone;
	spl_handler_ArrayObject.read_dimension = spl_array_read_dimension;
	spl_handler_ArrayObject.write_dimension = spl_array_write_dimension;
	spl_handler_ArrayObject.unset_dimension = spl_array_unset_dimension;
	spl_handler_ArrayObject.has_dimension = spl_array_has_dimension;
	spl_handler_ArrayObject.get_properties = spl_array_get_properties;

	REGISTER_SPL_STD_CLASS_EX(ArrayIterator, spl_array_object_new, spl_funcs_ArrayIterator);
	zend_class_implements(spl_ce_ArrayIterator TSRMLS_CC, 1, zend_ce_iterator);
	memcpy(&spl_handler_ArrayIterator, &spl_handler_ArrayObject, sizeof(zend_object_handlers));

	return SUCCESS;
}
/* }}} */

/* {{{ proto void spl_array::__construct(array|object ar = array())
       proto void spl_array_it::__construct(array|object ar = array())
 Cronstructs a new array iterator from a path. */
SPL_METHOD(Array, __construct)
{
	zval *object = getThis();
	spl_array_object *intern;
	zval **array;

	if (ZEND_NUM_ARGS() == 0) {
		return; /* nothing to do */
	}
/* exceptions do not work yet
	php_set_error_handling(EH_THROW, zend_exception_get_default() TSRMLS_CC);*/

	if (ZEND_NUM_ARGS() > 1 || zend_get_parameters_ex(1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	if (!HASH_OF(*array)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Passed variable is not an array or object, using empty array instead");
		php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
		return;
	}
	intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	zval_dtor(intern->array);
	FREE_ZVAL(intern->array);
	intern->array = *array;
	ZVAL_ADDREF(intern->array);

	zend_hash_internal_pointer_reset_ex(HASH_OF(intern->array), &intern->pos);

	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
}
/* }}} */

/* {{{ proto spl_array_it|NULL ArrayObject::getIterator()
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

/* {{{ spl_hash_pos_exists */
ZEND_API int spl_hash_pos_exists(spl_array_object * intern TSRMLS_DC)
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

/* {{{ proto void spl_array_it::rewind()
   Rewind array back to the start */
SPL_METHOD(Array, rewind)
{
	zval *object = getThis();
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	HashTable *aht = HASH_OF(intern->array);

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and is no longer an array");
		return;
	}

	zend_hash_internal_pointer_reset_ex(aht, &intern->pos);
}
/* }}} */

/* {{{ proto mixed|false spl_array_it::current()
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

	if (intern->array->is_ref && spl_hash_pos_exists(intern TSRMLS_CC) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and internal position is no longer valid");
		RETURN_FALSE;
	}

	if (zend_hash_get_current_data_ex(aht, (void **) &entry, &intern->pos) == FAILURE) {
		RETURN_FALSE;
	}
	*return_value = **entry;
	zval_copy_ctor(return_value);
}
/* }}} */

/* {{{ proto mixed|false spl_array_it::key()
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

	if (intern->array->is_ref && spl_hash_pos_exists(intern TSRMLS_CC) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and internal position is no longer valid");
		RETURN_FALSE;
	}

	switch (zend_hash_get_current_key_ex(aht, &string_key, &string_length, &num_key, 0, &intern->pos)) {
		case HASH_KEY_IS_STRING:
			RETVAL_STRINGL(string_key, string_length - 1, 1);
			break;
		case HASH_KEY_IS_LONG:
			RETVAL_LONG(num_key);
			break;
		case HASH_KEY_NON_EXISTANT:
			return;
	}
}
/* }}} */

/* {{{ proto void spl_array_it::next()
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

	if (intern->array->is_ref && spl_hash_pos_exists(intern TSRMLS_CC) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and internal position is no longer valid");
	} else {
		zend_hash_move_forward_ex(aht, &intern->pos);
	}
}
/* }}} */

/* {{{ proto bool spl_array_it::hasMore()
   Check whether array contains more entries */
SPL_METHOD(Array, hasMore)
{
	zval *object = getThis();
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);
	HashTable *aht = HASH_OF(intern->array);

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and is no longer an array");
		return;
	}

	if (intern->pos && intern->array->is_ref && spl_hash_pos_exists(intern TSRMLS_CC) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and internal position is no longer valid");
		RETURN_FALSE;
	} else {
		RETURN_BOOL(zend_hash_has_more_elements_ex(aht, &intern->pos) == SUCCESS);
	}
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
