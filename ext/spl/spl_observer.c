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
   | Authors: Marcus Boerger <helly@php.net>                              |
   |          Etienne Kneuss <colder@php.net>                             |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/php_var.h"
#include "ext/standard/php_smart_str.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"

#include "php_spl.h"
#include "spl_functions.h"
#include "spl_engine.h"
#include "spl_observer.h"
#include "spl_iterators.h"
#include "spl_array.h"
#include "spl_exceptions.h"

SPL_METHOD(SplObserver, update);
SPL_METHOD(SplSubject, attach);
SPL_METHOD(SplSubject, detach);
SPL_METHOD(SplSubject, notify);

ZEND_BEGIN_ARG_INFO(arginfo_SplObserver_update, 0)
	ZEND_ARG_OBJ_INFO(0, SplSubject, SplSubject, 0)
ZEND_END_ARG_INFO();

static const zend_function_entry spl_funcs_SplObserver[] = {
	SPL_ABSTRACT_ME(SplObserver, update,   arginfo_SplObserver_update)
	{NULL, NULL, NULL}
};

ZEND_BEGIN_ARG_INFO(arginfo_SplSubject_attach, 0)
	ZEND_ARG_OBJ_INFO(0, SplObserver, SplObserver, 0)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO(arginfo_SplSubject_void, 0)
ZEND_END_ARG_INFO();

/*ZEND_BEGIN_ARG_INFO_EX(arginfo_SplSubject_notify, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, ignore, SplObserver, 1)
ZEND_END_ARG_INFO();*/

static const zend_function_entry spl_funcs_SplSubject[] = {
	SPL_ABSTRACT_ME(SplSubject,  attach,   arginfo_SplSubject_attach)
	SPL_ABSTRACT_ME(SplSubject,  detach,   arginfo_SplSubject_attach)
	SPL_ABSTRACT_ME(SplSubject,  notify,   arginfo_SplSubject_void)
	{NULL, NULL, NULL}
};

PHPAPI zend_class_entry     *spl_ce_SplObserver;
PHPAPI zend_class_entry     *spl_ce_SplSubject;
PHPAPI zend_class_entry     *spl_ce_SplObjectStorage;
PHPAPI zend_class_entry     *spl_ce_MultipleIterator;

PHPAPI zend_object_handlers spl_handler_SplObjectStorage;

typedef struct _spl_SplObjectStorage { /* {{{ */
	zend_object       std;
	HashTable         storage;
	long              index;
	HashPosition      pos;
	long              flags;
	zend_function    *fptr_get_hash;
	HashTable        *debug_info;
} spl_SplObjectStorage; /* }}} */

/* {{{ storage is an assoc aray of [zend_object_value]=>[zval *obj, zval *inf] */
typedef struct _spl_SplObjectStorageElement {
	zval* obj;
	zval* inf;
} spl_SplObjectStorageElement; /* }}} */

void spl_SplOjectStorage_free_storage(void *object TSRMLS_DC) /* {{{ */
{
	spl_SplObjectStorage *intern = (spl_SplObjectStorage *)object;

	zend_object_std_dtor(&intern->std TSRMLS_CC);
	
	zend_hash_destroy(&intern->storage);
	
	if (intern->debug_info != NULL) {
		zend_hash_destroy(intern->debug_info);
		efree(intern->debug_info);
	}

	efree(object);
} /* }}} */

static char *spl_object_storage_get_hash(spl_SplObjectStorage *intern, zval *this,  zval *obj, int *hash_len_ptr TSRMLS_DC) {
	if (intern->fptr_get_hash) {
		zval *rv;
		zend_call_method_with_1_params(&this, intern->std.ce, &intern->fptr_get_hash, "getHash", &rv, obj);
		if (rv) {
			if (Z_TYPE_P(rv) == IS_STRING) {
				int hash_len = Z_STRLEN_P(rv);
				char *hash = emalloc((hash_len+1)*sizeof(char));
				strncpy(hash, Z_STRVAL_P(rv), hash_len);
				hash[hash_len] = 0;

				zval_ptr_dtor(&rv);
				if (hash_len_ptr) {
					*hash_len_ptr = hash_len;
				}
				return hash;
			} else {
				zend_throw_exception(spl_ce_RuntimeException, "Hash needs to be a string", 0 TSRMLS_CC);

				zval_ptr_dtor(&rv);
				return NULL;
			}
		} else {
			return NULL;
		}
	} else {
		int hash_len = sizeof(zend_object_value);

#if HAVE_PACKED_OBJECT_VALUE

		if (hash_len_ptr) {
			*hash_len_ptr = hash_len;
		}

		return (char*)&Z_OBJVAL_P(obj);
#else
		char *hash = emalloc(hash_len + 1);

		zend_object_value zvalue;
		memset(&zvalue, 0, sizeof(zend_object_value));
		zvalue.handle = Z_OBJ_HANDLE_P(obj);
		zvalue.handlers = Z_OBJ_HT_P(obj);

		memcpy(hash, (char *)&zvalue, hash_len);
		hash[hash_len] = 0;

		if (hash_len_ptr) {
			*hash_len_ptr = hash_len;
		}

		return hash;
#endif
	}
}

static void spl_object_storage_free_hash(spl_SplObjectStorage *intern, char *hash) {
	if (intern->fptr_get_hash) {
		efree(hash);
	} else {
#if HAVE_PACKED_OBJECT_VALUE
		/* Nothing to do */
#else
		efree(hash);
#endif
	}
}

static void spl_object_storage_dtor(spl_SplObjectStorageElement *element) /* {{{ */
{
	zval_ptr_dtor(&element->obj);
	zval_ptr_dtor(&element->inf);
} /* }}} */

spl_SplObjectStorageElement* spl_object_storage_get(spl_SplObjectStorage *intern, char *hash, int hash_len TSRMLS_DC) /* {{{ */
{
	spl_SplObjectStorageElement *element;
	if (zend_hash_find(&intern->storage, hash, hash_len, (void**)&element) == SUCCESS) {
		return element;
	} else {
		return NULL;
	}
} /* }}} */

void spl_object_storage_attach(spl_SplObjectStorage *intern, zval *this, zval *obj, zval *inf TSRMLS_DC) /* {{{ */
{
	spl_SplObjectStorageElement *pelement, element;
	
	int hash_len;
	char *hash = spl_object_storage_get_hash(intern, this, obj, &hash_len TSRMLS_CC);
	if (!hash) {
		return;
	}

	pelement = spl_object_storage_get(intern, hash, hash_len TSRMLS_CC);

	if (inf) {
		Z_ADDREF_P(inf);
	} else {
		ALLOC_INIT_ZVAL(inf);
	}
	if (pelement) {
		zval_ptr_dtor(&pelement->inf);
		pelement->inf = inf;
		spl_object_storage_free_hash(intern, hash);
		return;
	}
	Z_ADDREF_P(obj);
	element.obj = obj;
	element.inf = inf;
	zend_hash_update(&intern->storage, hash, hash_len, &element, sizeof(spl_SplObjectStorageElement), NULL);
	spl_object_storage_free_hash(intern, hash);
} /* }}} */

int spl_object_storage_detach(spl_SplObjectStorage *intern, zval *this, zval *obj TSRMLS_DC) /* {{{ */
{
	int hash_len, ret = FAILURE;
	char *hash = spl_object_storage_get_hash(intern, this, obj, &hash_len TSRMLS_CC);
	if (!hash) {
		return ret;
	}
	ret = zend_hash_del(&intern->storage, hash, hash_len);
	spl_object_storage_free_hash(intern, hash);
	
	return ret;
} /* }}}*/

void spl_object_storage_addall(spl_SplObjectStorage *intern, zval *this, spl_SplObjectStorage *other TSRMLS_DC) { /* {{{ */
	HashPosition pos;
	spl_SplObjectStorageElement *element;

	zend_hash_internal_pointer_reset_ex(&other->storage, &pos);
	while (zend_hash_get_current_data_ex(&other->storage, (void **)&element, &pos) == SUCCESS) {
		spl_object_storage_attach(intern, this, element->obj, element->inf TSRMLS_CC);
		zend_hash_move_forward_ex(&other->storage, &pos);
	}

	zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
	intern->index = 0;
} /* }}} */

static zend_object_value spl_object_storage_new_ex(zend_class_entry *class_type, spl_SplObjectStorage **obj, zval *orig TSRMLS_DC) /* {{{ */
{
	zend_object_value     retval;
	spl_SplObjectStorage *intern;
	zend_class_entry     *parent = class_type;

	intern = emalloc(sizeof(spl_SplObjectStorage));
	memset(intern, 0, sizeof(spl_SplObjectStorage));
	*obj = intern;

	zend_object_std_init(&intern->std, class_type TSRMLS_CC);
	object_properties_init(&intern->std, class_type);

	zend_hash_init(&intern->storage, 0, NULL, (void (*)(void *))spl_object_storage_dtor, 0);

	retval.handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t) spl_SplOjectStorage_free_storage, NULL TSRMLS_CC);
	retval.handlers = &spl_handler_SplObjectStorage;

	if (orig) {
		spl_SplObjectStorage *other = (spl_SplObjectStorage*)zend_object_store_get_object(orig TSRMLS_CC);
		spl_object_storage_addall(intern, orig, other TSRMLS_CC);
	}

	while (parent) {
		if (parent == spl_ce_SplObjectStorage) {
			if (class_type != spl_ce_SplObjectStorage) {
				zend_hash_find(&class_type->function_table, "gethash",    sizeof("gethash"),    (void **) &intern->fptr_get_hash);
				if (intern->fptr_get_hash->common.scope == spl_ce_SplObjectStorage) {
					intern->fptr_get_hash = NULL;
				}
			}
			break;
		}

		parent = parent->parent;
	}

	return retval;
}
/* }}} */

/* {{{ spl_object_storage_clone */
static zend_object_value spl_object_storage_clone(zval *zobject TSRMLS_DC)
{
	zend_object_value new_obj_val;
	zend_object *old_object;
	zend_object *new_object;
	zend_object_handle handle = Z_OBJ_HANDLE_P(zobject);
	spl_SplObjectStorage *intern;

	old_object = zend_objects_get_address(zobject TSRMLS_CC);
	new_obj_val = spl_object_storage_new_ex(old_object->ce, &intern, zobject TSRMLS_CC);
	new_object = &intern->std;

	zend_objects_clone_members(new_object, new_obj_val, old_object, handle TSRMLS_CC);

	return new_obj_val;
}
/* }}} */

static HashTable* spl_object_storage_debug_info(zval *obj, int *is_temp TSRMLS_DC) /* {{{ */
{
	spl_SplObjectStorage *intern = (spl_SplObjectStorage*)zend_object_store_get_object(obj TSRMLS_CC);
	spl_SplObjectStorageElement *element;
	HashTable *props;
	HashPosition pos;
	zval *tmp, *storage;
	char md5str[33];
	int name_len;
	char *zname;

	*is_temp = 0;

	props = Z_OBJPROP_P(obj);
	zend_hash_del(props, "\x00gcdata", sizeof("\x00gcdata"));

	if (intern->debug_info == NULL) {
		ALLOC_HASHTABLE(intern->debug_info);
		ZEND_INIT_SYMTABLE_EX(intern->debug_info, zend_hash_num_elements(props) + 1, 0);
	}

	if (intern->debug_info->nApplyCount == 0) {
		zend_hash_copy(intern->debug_info, props, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

		MAKE_STD_ZVAL(storage);
		array_init(storage);

		zend_hash_internal_pointer_reset_ex(&intern->storage, &pos);
		while (zend_hash_get_current_data_ex(&intern->storage, (void **)&element, &pos) == SUCCESS) {
				php_spl_object_hash(element->obj, md5str TSRMLS_CC);
				MAKE_STD_ZVAL(tmp);
				array_init(tmp);
				/* Incrementing the refcount of obj and inf would confuse the garbage collector.
				 * Prefer to null the destructor */
				Z_ARRVAL_P(tmp)->pDestructor = NULL;
				add_assoc_zval_ex(tmp, "obj", sizeof("obj"), element->obj);
				add_assoc_zval_ex(tmp, "inf", sizeof("inf"), element->inf);
				add_assoc_zval_ex(storage, md5str, 33, tmp);
				zend_hash_move_forward_ex(&intern->storage, &pos);
		}

		zname = spl_gen_private_prop_name(spl_ce_SplObjectStorage, "storage", sizeof("storage")-1, &name_len TSRMLS_CC);
		zend_symtable_update(intern->debug_info, zname, name_len+1, &storage, sizeof(zval *), NULL);
		efree(zname);
	}

	return intern->debug_info;
}
/* }}} */

/* overriden for garbage collection
 * This is very hacky */
static HashTable *spl_object_storage_get_gc(zval *obj, zval ***table, int *n TSRMLS_DC) /* {{{ */
{
	spl_SplObjectStorage *intern = (spl_SplObjectStorage*)zend_object_store_get_object(obj TSRMLS_CC);
	spl_SplObjectStorageElement *element;
	HashTable *props;
	HashPosition pos;
	zval *gcdata_arr = NULL,
		 **gcdata_arr_pp;

	props = std_object_handlers.get_properties(obj TSRMLS_CC);
	
	*table = NULL;
	*n = 0;

	/* clean \x00gcdata, as it may be out of date */
	if (zend_hash_find(props, "\x00gcdata", sizeof("\x00gcdata"), (void**) &gcdata_arr_pp) == SUCCESS) {
		gcdata_arr = *gcdata_arr_pp;
		zend_hash_clean(Z_ARRVAL_P(gcdata_arr));
	}

	if (gcdata_arr == NULL) {
		MAKE_STD_ZVAL(gcdata_arr);
		array_init(gcdata_arr);
		/* don't decrease refcount of members when destroying */
		Z_ARRVAL_P(gcdata_arr)->pDestructor = NULL;

		/* name starts with \x00 to make tampering in user-land more difficult */
		zend_hash_add(props, "\x00gcdata", sizeof("\x00gcdata"), &gcdata_arr, sizeof(gcdata_arr), NULL);
	}

	zend_hash_internal_pointer_reset_ex(&intern->storage, &pos);
	while (zend_hash_get_current_data_ex(&intern->storage, (void **)&element, &pos) == SUCCESS) {
		add_next_index_zval(gcdata_arr, element->obj);
		add_next_index_zval(gcdata_arr, element->inf);
		zend_hash_move_forward_ex(&intern->storage, &pos);
	}

	return props;
}
/* }}} */

static int spl_object_storage_compare_info(spl_SplObjectStorageElement *e1, spl_SplObjectStorageElement *e2 TSRMLS_DC) /* {{{ */
{
	zval result;

	if (compare_function(&result, e1->inf, e2->inf TSRMLS_CC) == FAILURE) {
		return 1;
	}

	return Z_LVAL(result);
}
/* }}} */

static int spl_object_storage_compare_objects(zval *o1, zval *o2 TSRMLS_DC) /* {{{ */
{
	zend_object *zo1 = (zend_object *)zend_object_store_get_object(o1 TSRMLS_CC);
	zend_object *zo2 = (zend_object *)zend_object_store_get_object(o2 TSRMLS_CC);

	if (zo1->ce != spl_ce_SplObjectStorage || zo2->ce != spl_ce_SplObjectStorage) {
		return 1;
	}

	return zend_hash_compare(&((spl_SplObjectStorage *)zo1)->storage, &((spl_SplObjectStorage *)zo2)->storage, (compare_func_t) spl_object_storage_compare_info, 0 TSRMLS_CC);
}
/* }}} */

/* {{{ spl_array_object_new */
static zend_object_value spl_SplObjectStorage_new(zend_class_entry *class_type TSRMLS_DC)
{
	spl_SplObjectStorage *tmp;
	return spl_object_storage_new_ex(class_type, &tmp, NULL TSRMLS_CC);
}
/* }}} */

int spl_object_storage_contains(spl_SplObjectStorage *intern, zval *this, zval *obj TSRMLS_DC) /* {{{ */
{
	int hash_len, found;
	char *hash = spl_object_storage_get_hash(intern, this, obj, &hash_len TSRMLS_CC);
	if (!hash) {
		return 0;
	}

	found = zend_hash_exists(&intern->storage, hash, hash_len);
	spl_object_storage_free_hash(intern, hash);
	return found;
} /* }}} */

/* {{{ proto void SplObjectStorage::attach($obj, $inf = NULL)
 Attaches an object to the storage if not yet contained */
SPL_METHOD(SplObjectStorage, attach)
{
	zval *obj, *inf = NULL;

	spl_SplObjectStorage *intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o|z!", &obj, &inf) == FAILURE) {
		return;
	}
	spl_object_storage_attach(intern, getThis(), obj, inf TSRMLS_CC);
} /* }}} */

/* {{{ proto void SplObjectStorage::detach($obj)
 Detaches an object from the storage */
SPL_METHOD(SplObjectStorage, detach)
{
	zval *obj;
	spl_SplObjectStorage *intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &obj) == FAILURE) {
		return;
	}
	spl_object_storage_detach(intern, getThis(), obj TSRMLS_CC);

	zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
	intern->index = 0;
} /* }}} */

/* {{{ proto string SplObjectStorage::getHash($object)
 Returns the hash of an object */
SPL_METHOD(SplObjectStorage, getHash)
{
	zval *obj;
	char *hash;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &obj) == FAILURE) {
		return;
	}

	hash = emalloc(33);
	php_spl_object_hash(obj, hash TSRMLS_CC);
	
	RETVAL_STRING(hash, 0);

} /* }}} */

/* {{{ proto mixed SplObjectStorage::offsetGet($object)
 Returns associated information for a stored object */
SPL_METHOD(SplObjectStorage, offsetGet)
{
	zval *obj;
	spl_SplObjectStorageElement *element;
	spl_SplObjectStorage *intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);
	char *hash;
	int hash_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &obj) == FAILURE) {
		return;
	}

	hash = spl_object_storage_get_hash(intern, getThis(), obj, &hash_len TSRMLS_CC);
	if (!hash) {
		return;
	}

	element = spl_object_storage_get(intern, hash, hash_len TSRMLS_CC);
	spl_object_storage_free_hash(intern, hash);

	if (!element) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Object not found");
	} else {
		RETURN_ZVAL(element->inf,1, 0);
	}
} /* }}} */

/* {{{ proto bool SplObjectStorage::addAll(SplObjectStorage $os)
 Add all elements contained in $os */
SPL_METHOD(SplObjectStorage, addAll)
{
	zval *obj;
	spl_SplObjectStorage *intern = (spl_SplObjectStorage *)zend_object_store_get_object(getThis() TSRMLS_CC);
	spl_SplObjectStorage *other;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &obj, spl_ce_SplObjectStorage) == FAILURE) {
		return;
	}

	other = (spl_SplObjectStorage *)zend_object_store_get_object(obj TSRMLS_CC);

	spl_object_storage_addall(intern, getThis(),  other TSRMLS_CC);

	RETURN_LONG(zend_hash_num_elements(&intern->storage));
} /* }}} */

/* {{{ proto bool SplObjectStorage::removeAll(SplObjectStorage $os)
 Remove all elements contained in $os */
SPL_METHOD(SplObjectStorage, removeAll)
{
	zval *obj;
	spl_SplObjectStorage *intern = (spl_SplObjectStorage *)zend_object_store_get_object(getThis() TSRMLS_CC);
	spl_SplObjectStorage *other;
	spl_SplObjectStorageElement *element;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &obj, spl_ce_SplObjectStorage) == FAILURE) {
		return;
	}

	other = (spl_SplObjectStorage *)zend_object_store_get_object(obj TSRMLS_CC);

	zend_hash_internal_pointer_reset(&other->storage);
	while (zend_hash_get_current_data(&other->storage, (void **)&element) == SUCCESS) {
		if (spl_object_storage_detach(intern, getThis(), element->obj TSRMLS_CC) == FAILURE) {
			zend_hash_move_forward(&other->storage);
		}
	}

	zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
	intern->index = 0;

	RETURN_LONG(zend_hash_num_elements(&intern->storage));
} /* }}} */

/* {{{ proto bool SplObjectStorage::removeAllExcept(SplObjectStorage $os)
 Remove elements not common to both this SplObjectStorage instance and $os */
SPL_METHOD(SplObjectStorage, removeAllExcept)
{
	zval *obj;
	spl_SplObjectStorage *intern = (spl_SplObjectStorage *)zend_object_store_get_object(getThis() TSRMLS_CC);
	spl_SplObjectStorage *other;
	spl_SplObjectStorageElement *element;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &obj, spl_ce_SplObjectStorage) == FAILURE) {
		return;
	}

	other = (spl_SplObjectStorage *)zend_object_store_get_object(obj TSRMLS_CC);

	zend_hash_internal_pointer_reset(&intern->storage);
	while (zend_hash_get_current_data(&intern->storage, (void **)&element) == SUCCESS) {
		if (!spl_object_storage_contains(other, getThis(), element->obj TSRMLS_CC)) {
			spl_object_storage_detach(intern, getThis(), element->obj TSRMLS_CC);
		}
		zend_hash_move_forward(&intern->storage);
	}

	zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
	intern->index = 0;

	RETURN_LONG(zend_hash_num_elements(&intern->storage));
}
/* }}} */

/* {{{ proto bool SplObjectStorage::contains($obj)
 Determine whethe an object is contained in the storage */
SPL_METHOD(SplObjectStorage, contains)
{
	zval *obj;
	spl_SplObjectStorage *intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &obj) == FAILURE) {
		return;
	}
	RETURN_BOOL(spl_object_storage_contains(intern, getThis(), obj TSRMLS_CC));
} /* }}} */

/* {{{ proto int SplObjectStorage::count()
 Determine number of objects in storage */
SPL_METHOD(SplObjectStorage, count)
{
	spl_SplObjectStorage *intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	
	RETURN_LONG(zend_hash_num_elements(&intern->storage));
} /* }}} */

/* {{{ proto void SplObjectStorage::rewind()
 Rewind to first position */
SPL_METHOD(SplObjectStorage, rewind)
{
	spl_SplObjectStorage *intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	
	zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
	intern->index = 0;
} /* }}} */

/* {{{ proto bool SplObjectStorage::valid()
 Returns whether current position is valid */
SPL_METHOD(SplObjectStorage, valid)
{
	spl_SplObjectStorage *intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	
	RETURN_BOOL(zend_hash_has_more_elements_ex(&intern->storage, &intern->pos) == SUCCESS);
} /* }}} */

/* {{{ proto mixed SplObjectStorage::key()
 Returns current key */
SPL_METHOD(SplObjectStorage, key)
{
	spl_SplObjectStorage *intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	
	RETURN_LONG(intern->index);
} /* }}} */

/* {{{ proto mixed SplObjectStorage::current()
 Returns current element */
SPL_METHOD(SplObjectStorage, current)
{
	spl_SplObjectStorageElement *element;
	spl_SplObjectStorage *intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	
	if (zend_hash_get_current_data_ex(&intern->storage, (void**)&element, &intern->pos) == FAILURE) {
		return;
	}
	RETVAL_ZVAL(element->obj, 1, 0);
} /* }}} */

/* {{{ proto mixed SplObjectStorage::getInfo()
 Returns associated information to current element */
SPL_METHOD(SplObjectStorage, getInfo)
{
	spl_SplObjectStorageElement *element;
	spl_SplObjectStorage *intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	
	if (zend_hash_get_current_data_ex(&intern->storage, (void**)&element, &intern->pos) == FAILURE) {
		return;
	}
	RETVAL_ZVAL(element->inf, 1, 0);
} /* }}} */

/* {{{ proto mixed SplObjectStorage::setInfo(mixed $inf)
 Sets associated information of current element to $inf */
SPL_METHOD(SplObjectStorage, setInfo)
{
	spl_SplObjectStorageElement *element;
	spl_SplObjectStorage *intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);
	zval *inf;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &inf) == FAILURE) {
		return;
	}

	if (zend_hash_get_current_data_ex(&intern->storage, (void**)&element, &intern->pos) == FAILURE) {
		return;
	}
	zval_ptr_dtor(&element->inf);
	element->inf = inf;
	Z_ADDREF_P(inf);
} /* }}} */

/* {{{ proto void SplObjectStorage::next()
 Moves position forward */
SPL_METHOD(SplObjectStorage, next)
{
	spl_SplObjectStorage *intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	
	zend_hash_move_forward_ex(&intern->storage, &intern->pos);
	intern->index++;
} /* }}} */

/* {{{ proto string SplObjectStorage::serialize()
 Serializes storage */
SPL_METHOD(SplObjectStorage, serialize)
{
	spl_SplObjectStorage *intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);

	spl_SplObjectStorageElement *element;
	zval members, *pmembers, *flags;
	HashPosition      pos;
	php_serialize_data_t var_hash;
	smart_str buf = {0};

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	PHP_VAR_SERIALIZE_INIT(var_hash);
	
	/* storage */
	smart_str_appendl(&buf, "x:", 2);
	MAKE_STD_ZVAL(flags);
	ZVAL_LONG(flags, zend_hash_num_elements(&intern->storage));
	php_var_serialize(&buf, &flags, &var_hash TSRMLS_CC);
	zval_ptr_dtor(&flags);

	zend_hash_internal_pointer_reset_ex(&intern->storage, &pos);

	while(zend_hash_has_more_elements_ex(&intern->storage, &pos) == SUCCESS) {
		if (zend_hash_get_current_data_ex(&intern->storage, (void**)&element, &pos) == FAILURE) {
			smart_str_free(&buf);
			PHP_VAR_SERIALIZE_DESTROY(var_hash);
			RETURN_NULL();
		}
		php_var_serialize(&buf, &element->obj, &var_hash TSRMLS_CC);
		smart_str_appendc(&buf, ',');
		php_var_serialize(&buf, &element->inf, &var_hash TSRMLS_CC);
		smart_str_appendc(&buf, ';');
		zend_hash_move_forward_ex(&intern->storage, &pos);
	}

	/* members */
	smart_str_appendl(&buf, "m:", 2);
	INIT_PZVAL(&members);
	Z_ARRVAL(members) = zend_std_get_properties(getThis() TSRMLS_CC);
	Z_TYPE(members) = IS_ARRAY;
	pmembers = &members;
	php_var_serialize(&buf, &pmembers, &var_hash TSRMLS_CC); /* finishes the string */

	/* done */
	PHP_VAR_SERIALIZE_DESTROY(var_hash);

	if (buf.c) {
		RETURN_STRINGL(buf.c, buf.len, 0);
	} else {
		RETURN_NULL();
	}
	
} /* }}} */

/* {{{ proto void SplObjectStorage::unserialize(string serialized)
 Unserializes storage */
SPL_METHOD(SplObjectStorage, unserialize)
{
	spl_SplObjectStorage *intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);

	char *buf;
	int buf_len;
	const unsigned char *p, *s;
	php_unserialize_data_t var_hash;
	zval *pentry, *pmembers, *pcount = NULL, *pinf;
	long count;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &buf, &buf_len) == FAILURE) {
		return;
	}

	if (buf_len == 0) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Empty serialized string cannot be empty");
		return;
	}

	/* storage */
	s = p = (const unsigned char*)buf;
	PHP_VAR_UNSERIALIZE_INIT(var_hash);

	if (*p!= 'x' || *++p != ':') {
		goto outexcept;
	}
	++p;

	ALLOC_INIT_ZVAL(pcount);
	if (!php_var_unserialize(&pcount, &p, s + buf_len, &var_hash TSRMLS_CC) || Z_TYPE_P(pcount) != IS_LONG) {
		goto outexcept;
	}

	--p; /* for ';' */
	count = Z_LVAL_P(pcount);
		
	while(count-- > 0) {
		spl_SplObjectStorageElement *pelement;
		char *hash;
		int hash_len;
		
		if (*p != ';') {
			goto outexcept;
		}
		++p;
		if(*p != 'O' && *p != 'C' && *p != 'r') {
			goto outexcept;
		}
		ALLOC_INIT_ZVAL(pentry);
		if (!php_var_unserialize(&pentry, &p, s + buf_len, &var_hash TSRMLS_CC)) {
			zval_ptr_dtor(&pentry);
			goto outexcept;
		}
		if(Z_TYPE_P(pentry) != IS_OBJECT) {
			zval_ptr_dtor(&pentry);
			goto outexcept;
		}
		ALLOC_INIT_ZVAL(pinf);
		if (*p == ',') { /* new version has inf */
			++p;
			if (!php_var_unserialize(&pinf, &p, s + buf_len, &var_hash TSRMLS_CC)) {
				zval_ptr_dtor(&pinf);
				goto outexcept;
			}
		}

		hash = spl_object_storage_get_hash(intern, getThis(), pentry, &hash_len TSRMLS_CC);
		if (!hash) {
			zval_ptr_dtor(&pentry);
			zval_ptr_dtor(&pinf);
			goto outexcept;
		}
		pelement = spl_object_storage_get(intern, hash, hash_len TSRMLS_CC);
		spl_object_storage_free_hash(intern, hash);
		if(pelement) {
			if(pelement->inf) {
				var_push_dtor(&var_hash, &pelement->inf);
			}
			if(pelement->obj) {
				var_push_dtor(&var_hash, &pelement->obj);
			}
		} 
		spl_object_storage_attach(intern, getThis(), pentry, pinf TSRMLS_CC);
		zval_ptr_dtor(&pentry);
		zval_ptr_dtor(&pinf);
	}

	if (*p != ';') {
		goto outexcept;
	}
	++p;

	/* members */
	if (*p!= 'm' || *++p != ':') {
		goto outexcept;
	}
	++p;

	ALLOC_INIT_ZVAL(pmembers);
	if (!php_var_unserialize(&pmembers, &p, s + buf_len, &var_hash TSRMLS_CC)) {
		zval_ptr_dtor(&pmembers);
		goto outexcept;
	}

	/* copy members */
	if (!intern->std.properties) {
		rebuild_object_properties(&intern->std);
	}
	zend_hash_copy(intern->std.properties, Z_ARRVAL_P(pmembers), (copy_ctor_func_t) zval_add_ref, (void *) NULL, sizeof(zval *));
	zval_ptr_dtor(&pmembers);

	/* done reading $serialized */
	if (pcount) {
		zval_ptr_dtor(&pcount);
	}
	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
	return;

outexcept:
	if (pcount) {
		zval_ptr_dtor(&pcount);
	}
	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
	zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Error at offset %ld of %d bytes", (long)((char*)p - buf), buf_len);
	return;

} /* }}} */

ZEND_BEGIN_ARG_INFO(arginfo_Object, 0)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_attach, 0, 0, 1)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, inf)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO(arginfo_Serialized, 0)
	ZEND_ARG_INFO(0, serialized)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO(arginfo_setInfo, 0)
	ZEND_ARG_INFO(0, info)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO(arginfo_getHash, 0)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_offsetGet, 0, 0, 1)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_splobject_void, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry spl_funcs_SplObjectStorage[] = {
	SPL_ME(SplObjectStorage,  attach,      arginfo_attach,        0)
	SPL_ME(SplObjectStorage,  detach,      arginfo_Object,        0)
	SPL_ME(SplObjectStorage,  contains,    arginfo_Object,        0)
	SPL_ME(SplObjectStorage,  addAll,      arginfo_Object,        0)
	SPL_ME(SplObjectStorage,  removeAll,   arginfo_Object,        0)
	SPL_ME(SplObjectStorage,  removeAllExcept,   arginfo_Object,  0)
	SPL_ME(SplObjectStorage,  getInfo,     arginfo_splobject_void,0)
	SPL_ME(SplObjectStorage,  setInfo,     arginfo_setInfo,       0)
	SPL_ME(SplObjectStorage,  getHash,     arginfo_getHash,       0)
	/* Countable */
	SPL_ME(SplObjectStorage,  count,       arginfo_splobject_void,0)
	/* Iterator */
	SPL_ME(SplObjectStorage,  rewind,      arginfo_splobject_void,0)
	SPL_ME(SplObjectStorage,  valid,       arginfo_splobject_void,0)
	SPL_ME(SplObjectStorage,  key,         arginfo_splobject_void,0)
	SPL_ME(SplObjectStorage,  current,     arginfo_splobject_void,0)
	SPL_ME(SplObjectStorage,  next,        arginfo_splobject_void,0)
	/* Serializable */
	SPL_ME(SplObjectStorage,  unserialize, arginfo_Serialized,    0)
	SPL_ME(SplObjectStorage,  serialize,   arginfo_splobject_void,0)
	/* ArrayAccess */
	SPL_MA(SplObjectStorage, offsetExists, SplObjectStorage, contains, arginfo_offsetGet, 0)
	SPL_MA(SplObjectStorage, offsetSet,    SplObjectStorage, attach,   arginfo_attach, 0)
	SPL_MA(SplObjectStorage, offsetUnset,  SplObjectStorage, detach,   arginfo_offsetGet, 0)
	SPL_ME(SplObjectStorage, offsetGet,    arginfo_offsetGet,     0)
	{NULL, NULL, NULL}
};

typedef enum {
	MIT_NEED_ANY     = 0,
	MIT_NEED_ALL     = 1,
	MIT_KEYS_NUMERIC = 0,
	MIT_KEYS_ASSOC   = 2
} MultipleIteratorFlags;

#define SPL_MULTIPLE_ITERATOR_GET_ALL_CURRENT   1
#define SPL_MULTIPLE_ITERATOR_GET_ALL_KEY       2

/* {{{ proto void MultipleIterator::__construct([int flags = MIT_NEED_ALL|MIT_KEYS_NUMERIC])
   Iterator that iterates over several iterators one after the other */
SPL_METHOD(MultipleIterator, __construct)
{
	spl_SplObjectStorage   *intern;
	long                    flags = MIT_NEED_ALL|MIT_KEYS_NUMERIC;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, spl_ce_InvalidArgumentException, &error_handling TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &flags) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}

	intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);
	intern->flags = flags;
	zend_restore_error_handling(&error_handling TSRMLS_CC);
}
/* }}} */

/* {{{ proto int MultipleIterator::getFlags()
   Return current flags */
SPL_METHOD(MultipleIterator, getFlags)
{
	spl_SplObjectStorage *intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	RETURN_LONG(intern->flags);
}
/* }}} */

/* {{{ proto int MultipleIterator::setFlags(int flags)
   Set flags */
SPL_METHOD(MultipleIterator, setFlags)
{
	spl_SplObjectStorage *intern;
	intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &intern->flags) == FAILURE) {
		return;
	}
}
/* }}} */

/* {{{ proto void attachIterator(Iterator iterator[, mixed info]) throws InvalidArgumentException
   Attach a new iterator */
SPL_METHOD(MultipleIterator, attachIterator)
{
	spl_SplObjectStorage        *intern;
	zval                        *iterator = NULL, *info = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O|z!", &iterator, zend_ce_iterator, &info) == FAILURE) {
		return;
	}

	intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (info != NULL) {
		spl_SplObjectStorageElement *element;
		zval                         compare_result;

		if (Z_TYPE_P(info) != IS_LONG && Z_TYPE_P(info) != IS_STRING) {
			zend_throw_exception(spl_ce_InvalidArgumentException, "Info must be NULL, integer or string", 0 TSRMLS_CC);
			return;
		}

		zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
		while (zend_hash_get_current_data_ex(&intern->storage, (void**)&element, &intern->pos) == SUCCESS) {
			is_identical_function(&compare_result, info, element->inf TSRMLS_CC);
			if (Z_LVAL(compare_result)) {
				zend_throw_exception(spl_ce_InvalidArgumentException, "Key duplication error", 0 TSRMLS_CC);
				return;
			}
			zend_hash_move_forward_ex(&intern->storage, &intern->pos);
		}
	}

	spl_object_storage_attach(intern, getThis(), iterator, info TSRMLS_CC);
}
/* }}} */

/* {{{ proto void MultipleIterator::rewind()
   Rewind all attached iterator instances */
SPL_METHOD(MultipleIterator, rewind)
{
	spl_SplObjectStorage        *intern;
	spl_SplObjectStorageElement *element;
	zval                        *it;

	intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
	while (zend_hash_get_current_data_ex(&intern->storage, (void**)&element, &intern->pos) == SUCCESS && !EG(exception)) {
		it = element->obj;
		zend_call_method_with_0_params(&it, Z_OBJCE_P(it), &Z_OBJCE_P(it)->iterator_funcs.zf_rewind, "rewind", NULL);
		zend_hash_move_forward_ex(&intern->storage, &intern->pos);
	}
}
/* }}} */

/* {{{ proto void MultipleIterator::next()
   Move all attached iterator instances forward */
SPL_METHOD(MultipleIterator, next)
{
	spl_SplObjectStorage        *intern;
	spl_SplObjectStorageElement *element;
	zval                        *it;

	intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
	while (zend_hash_get_current_data_ex(&intern->storage, (void**)&element, &intern->pos) == SUCCESS && !EG(exception)) {
		it = element->obj;
		zend_call_method_with_0_params(&it, Z_OBJCE_P(it), &Z_OBJCE_P(it)->iterator_funcs.zf_next, "next", NULL);
		zend_hash_move_forward_ex(&intern->storage, &intern->pos);
	}
}
/* }}} */

/* {{{ proto bool MultipleIterator::valid()
   Return whether all or one sub iterator is valid depending on flags */
SPL_METHOD(MultipleIterator, valid)
{
	spl_SplObjectStorage        *intern;
	spl_SplObjectStorageElement *element;
	zval                        *it, *retval = NULL;
	long                         expect, valid;

	intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (!zend_hash_num_elements(&intern->storage)) {
		RETURN_FALSE;
	}

	expect = (intern->flags & MIT_NEED_ALL) ? 1 : 0;

	zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
	while (zend_hash_get_current_data_ex(&intern->storage, (void**)&element, &intern->pos) == SUCCESS && !EG(exception)) {
		it = element->obj;
		zend_call_method_with_0_params(&it, Z_OBJCE_P(it), &Z_OBJCE_P(it)->iterator_funcs.zf_valid, "valid", &retval);

		if (retval) {
			valid = Z_LVAL_P(retval);
			zval_ptr_dtor(&retval);
		} else {
			valid = 0;
		}

		if (expect != valid) {
			RETURN_BOOL(!expect);
		}

		zend_hash_move_forward_ex(&intern->storage, &intern->pos);
	}

	RETURN_BOOL(expect);
}
/* }}} */

static void spl_multiple_iterator_get_all(spl_SplObjectStorage *intern, int get_type, zval *return_value TSRMLS_DC) /* {{{ */
{
	spl_SplObjectStorageElement *element;
	zval                        *it, *retval = NULL;
	int                          valid = 1, num_elements;

	num_elements = zend_hash_num_elements(&intern->storage);
	if (num_elements < 1) {
		RETURN_FALSE;
	}

	array_init_size(return_value, num_elements);
	
	zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
	while (zend_hash_get_current_data_ex(&intern->storage, (void**)&element, &intern->pos) == SUCCESS && !EG(exception)) {
		it = element->obj;
		zend_call_method_with_0_params(&it, Z_OBJCE_P(it), &Z_OBJCE_P(it)->iterator_funcs.zf_valid, "valid", &retval);

		if (retval) {
			valid = Z_LVAL_P(retval);
			zval_ptr_dtor(&retval);
		} else {
			valid = 0;
		}

		if (valid) {
			if (SPL_MULTIPLE_ITERATOR_GET_ALL_CURRENT == get_type) {
				zend_call_method_with_0_params(&it, Z_OBJCE_P(it), &Z_OBJCE_P(it)->iterator_funcs.zf_current, "current", &retval);
			} else {
				zend_call_method_with_0_params(&it, Z_OBJCE_P(it), &Z_OBJCE_P(it)->iterator_funcs.zf_key,     "key",     &retval);
			}
			if (!retval) {
				zend_throw_exception(spl_ce_RuntimeException, "Failed to call sub iterator method", 0 TSRMLS_CC);
				return;
			}
		} else if (intern->flags & MIT_NEED_ALL) {
			if (SPL_MULTIPLE_ITERATOR_GET_ALL_CURRENT == get_type) {
				zend_throw_exception(spl_ce_RuntimeException, "Called current() with non valid sub iterator", 0 TSRMLS_CC);
			} else {
				zend_throw_exception(spl_ce_RuntimeException, "Called key() with non valid sub iterator", 0 TSRMLS_CC);
			}
			return;
		} else {
			ALLOC_INIT_ZVAL(retval);
		}

		if (intern->flags & MIT_KEYS_ASSOC) {
			switch (Z_TYPE_P(element->inf)) {
				case IS_LONG:
					add_index_zval(return_value, Z_LVAL_P(element->inf), retval);
					break;
				case IS_STRING:
					add_assoc_zval_ex(return_value, Z_STRVAL_P(element->inf), Z_STRLEN_P(element->inf)+1U, retval);
					break;
				default:
					zval_ptr_dtor(&retval);
					zend_throw_exception(spl_ce_InvalidArgumentException, "Sub-Iterator is associated with NULL", 0 TSRMLS_CC);
					return;
			}
		} else {
			add_next_index_zval(return_value, retval);
		}

		zend_hash_move_forward_ex(&intern->storage, &intern->pos);
	}
}
/* }}} */

/* {{{ proto array current() throws RuntimeException throws InvalidArgumentException
   Return an array of all registered Iterator instances current() result */
SPL_METHOD(MultipleIterator, current)
{
	spl_SplObjectStorage        *intern;
	intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	spl_multiple_iterator_get_all(intern, SPL_MULTIPLE_ITERATOR_GET_ALL_CURRENT, return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto array MultipleIterator::key()
   Return an array of all registered Iterator instances key() result */
SPL_METHOD(MultipleIterator, key)
{
	spl_SplObjectStorage        *intern;
	intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	spl_multiple_iterator_get_all(intern, SPL_MULTIPLE_ITERATOR_GET_ALL_KEY, return_value TSRMLS_CC);
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo_MultipleIterator_attachIterator, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, iterator, Iterator, 0)
	ZEND_ARG_INFO(0, infos)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_MultipleIterator_detachIterator, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, iterator, Iterator, 0)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_MultipleIterator_containsIterator, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, iterator, Iterator, 0)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_MultipleIterator_setflags, 0, 0, 1)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO();

static const zend_function_entry spl_funcs_MultipleIterator[] = {
	SPL_ME(MultipleIterator,  __construct,            arginfo_MultipleIterator_setflags,          0)
	SPL_ME(MultipleIterator,  getFlags,               arginfo_splobject_void,                     0)
	SPL_ME(MultipleIterator,  setFlags,               arginfo_MultipleIterator_setflags,          0)
	SPL_ME(MultipleIterator,  attachIterator,         arginfo_MultipleIterator_attachIterator,    0)
	SPL_MA(MultipleIterator,  detachIterator,         SplObjectStorage, detach,   arginfo_MultipleIterator_detachIterator,   0)
	SPL_MA(MultipleIterator,  containsIterator,       SplObjectStorage, contains, arginfo_MultipleIterator_containsIterator, 0)
	SPL_MA(MultipleIterator,  countIterators,         SplObjectStorage, count,    arginfo_splobject_void,                    0)
	/* Iterator */
	SPL_ME(MultipleIterator,  rewind,                 arginfo_splobject_void,                     0)
	SPL_ME(MultipleIterator,  valid,                  arginfo_splobject_void,                     0)
	SPL_ME(MultipleIterator,  key,                    arginfo_splobject_void,                     0)
	SPL_ME(MultipleIterator,  current,                arginfo_splobject_void,                     0)
	SPL_ME(MultipleIterator,  next,                   arginfo_splobject_void,                     0)
	{NULL, NULL, NULL}
};

/* {{{ PHP_MINIT_FUNCTION(spl_observer) */
PHP_MINIT_FUNCTION(spl_observer)
{
	REGISTER_SPL_INTERFACE(SplObserver);
	REGISTER_SPL_INTERFACE(SplSubject);

	REGISTER_SPL_STD_CLASS_EX(SplObjectStorage, spl_SplObjectStorage_new, spl_funcs_SplObjectStorage);
	memcpy(&spl_handler_SplObjectStorage, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

	spl_handler_SplObjectStorage.get_debug_info  = spl_object_storage_debug_info;
	spl_handler_SplObjectStorage.compare_objects = spl_object_storage_compare_objects;
	spl_handler_SplObjectStorage.clone_obj       = spl_object_storage_clone;
	spl_handler_SplObjectStorage.get_gc          = spl_object_storage_get_gc;

	REGISTER_SPL_IMPLEMENTS(SplObjectStorage, Countable);
	REGISTER_SPL_IMPLEMENTS(SplObjectStorage, Iterator);
	REGISTER_SPL_IMPLEMENTS(SplObjectStorage, Serializable);
	REGISTER_SPL_IMPLEMENTS(SplObjectStorage, ArrayAccess);

	REGISTER_SPL_STD_CLASS_EX(MultipleIterator, spl_SplObjectStorage_new, spl_funcs_MultipleIterator);
	REGISTER_SPL_ITERATOR(MultipleIterator);

	REGISTER_SPL_CLASS_CONST_LONG(MultipleIterator, "MIT_NEED_ANY",     MIT_NEED_ANY);
	REGISTER_SPL_CLASS_CONST_LONG(MultipleIterator, "MIT_NEED_ALL",     MIT_NEED_ALL);
	REGISTER_SPL_CLASS_CONST_LONG(MultipleIterator, "MIT_KEYS_NUMERIC", MIT_KEYS_NUMERIC);
	REGISTER_SPL_CLASS_CONST_LONG(MultipleIterator, "MIT_KEYS_ASSOC",   MIT_KEYS_ASSOC);

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
