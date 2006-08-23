/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2006 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is SplSubject to version 3.01 of the PHP license,      |
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
#include "spl_observer.h"
#include "spl_iterators.h"
#include "spl_array.h"

SPL_METHOD(SplObserver, update);
SPL_METHOD(SplSubject, attach);
SPL_METHOD(SplSubject, detach);
SPL_METHOD(SplSubject, notify);

static
ZEND_BEGIN_ARG_INFO(arginfo_SplObserver_update, 0)
	ZEND_ARG_OBJ_INFO(0, SplSubject, SplSubject, 0)
ZEND_END_ARG_INFO();

static zend_function_entry spl_funcs_SplObserver[] = {
	SPL_ABSTRACT_ME(SplObserver, update,   arginfo_SplObserver_update)
	{NULL, NULL, NULL}
};

static
ZEND_BEGIN_ARG_INFO(arginfo_SplSubject_attach, 0)
	ZEND_ARG_OBJ_INFO(0, SplObserver, SplObserver, 0)
ZEND_END_ARG_INFO();

/*static
ZEND_BEGIN_ARG_INFO_EX(arginfo_SplSubject_notify, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, ignore, SplObserver, 1)
ZEND_END_ARG_INFO();*/

static zend_function_entry spl_funcs_SplSubject[] = {
	SPL_ABSTRACT_ME(SplSubject,  attach,   arginfo_SplSubject_attach)
	SPL_ABSTRACT_ME(SplSubject,  detach,   arginfo_SplSubject_attach)
	SPL_ABSTRACT_ME(SplSubject,  notify,   NULL)
	{NULL, NULL, NULL}
};

PHPAPI zend_class_entry     *spl_ce_SplObserver;
PHPAPI zend_class_entry     *spl_ce_SplSubject;
PHPAPI zend_class_entry     *spl_ce_SplObjectStorage;
PHPAPI zend_object_handlers spl_handler_SplObjectStorage;

typedef struct _spl_SplObjectStorage {
	zend_object       std;
	HashTable         storage;
	long              index;
	HashPosition      pos;
} spl_SplObjectStorage;

/* storage is an assoc aray of [zend_object_value]=>[zval*] */

void spl_SplOjectStorage_free_storage(void *object TSRMLS_DC) /* {{{ */
{
	spl_SplObjectStorage *intern = (spl_SplObjectStorage *)object;

	zend_object_std_dtor(&intern->std TSRMLS_CC);
	
	zend_hash_destroy(&intern->storage);

	efree(object);
} /* }}} */

static zend_object_value spl_object_storage_new_ex(zend_class_entry *class_type, spl_SplObjectStorage **obj, zval *orig TSRMLS_DC) /* {{{ */
{
	zend_object_value retval;
	spl_SplObjectStorage *intern;
	zval *tmp;

	intern = emalloc(sizeof(spl_SplObjectStorage));
	memset(intern, 0, sizeof(spl_SplObjectStorage));
	*obj = intern;

	zend_object_std_init(&intern->std, class_type TSRMLS_CC);
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	zend_hash_init(&intern->storage, 0, NULL, ZVAL_PTR_DTOR, 0);

	retval.handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t) spl_SplOjectStorage_free_storage, NULL TSRMLS_CC);
	retval.handlers = &spl_handler_SplObjectStorage;
	return retval;
}
/* }}} */

/* {{{ spl_array_object_new */
static zend_object_value spl_SplObjectStorage_new(zend_class_entry *class_type TSRMLS_DC)
{
	spl_SplObjectStorage *tmp;
	return spl_object_storage_new_ex(class_type, &tmp, NULL TSRMLS_CC);
}
/* }}} */

/* {{{ proto void SplObjectStorage::attach($obj)
 Attaches an object to the storage if not yet contained */
SPL_METHOD(SplObjectStorage, attach)
{
	zval *obj;

	spl_SplObjectStorage *intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &obj) == FAILURE) {
		return;
	}

#if HAVE_PACKED_OBJECT_VALUE
	zend_hash_update(&intern->storage, (char*)&Z_OBJVAL_P(obj), sizeof(zend_object_value), &obj, sizeof(zval*), NULL);	
#else
	{
		zend_object_value zvalue;
		memset(&zvalue, 0, sizeof(zend_object_value));
		zvalue.handle = Z_OBJ_HANDLE_P(obj);
		zvalue.handlers = Z_OBJ_HT_P(obj);
		zend_hash_update(&intern->storage, (char*)&zvalue, sizeof(zend_object_value), &obj, sizeof(zval*), NULL);
	}
#endif

	obj->refcount++;
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

#if HAVE_PACKED_OBJECT_VALUE
	zend_hash_del(&intern->storage, (char*)&Z_OBJVAL_P(obj), sizeof(zend_object_value));
#else
	{
		zend_object_value zvalue;
		memset(&zvalue, 0, sizeof(zend_object_value));
		zvalue.handle = Z_OBJ_HANDLE_P(obj);
		zvalue.handlers = Z_OBJ_HT_P(obj);
		zend_hash_del(&intern->storage, (char*)&zvalue, sizeof(zend_object_value));
	}
#endif

	zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
	intern->index = 0;
} /* }}} */

/* {{{ proto bool SplObjectStorage::contains($obj)
 Determine whethe an object is contained in the storage */
SPL_METHOD(SplObjectStorage, contains)
{
	zval *obj;
	spl_SplObjectStorage *intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &obj) == FAILURE) {
		return;
	}

#if HAVE_PACKED_OBJECT_VALUE
	RETURN_BOOL(zend_hash_exists(&intern->storage, (char*)&Z_OBJVAL_P(obj), sizeof(zend_object_value)));
#else
	{
		zend_object_value zvalue;
		memset(&zvalue, 0, sizeof(zend_object_value));
		zvalue.handle = Z_OBJ_HANDLE_P(obj);
		zvalue.handlers = Z_OBJ_HT_P(obj);
		RETURN_BOOL(zend_hash_exists(&intern->storage, (char*)&zvalue, sizeof(zend_object_value)));
	}
#endif
} /* }}} */

/* {{{ proto int SplObjectStorage::count()
 Determine number of objects in storage */
SPL_METHOD(SplObjectStorage, count)
{
	spl_SplObjectStorage *intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	RETURN_LONG(zend_hash_num_elements(&intern->storage));
} /* }}} */

/* {{{ proto void SplObjectStorage::rewind()
 */
SPL_METHOD(SplObjectStorage, rewind)
{
	spl_SplObjectStorage *intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
	intern->index = 0;
} /* }}} */

/* {{{ proto bool SplObjectStorage::valid()
 */
SPL_METHOD(SplObjectStorage, valid)
{
	spl_SplObjectStorage *intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	RETURN_BOOL(zend_hash_has_more_elements_ex(&intern->storage, &intern->pos) == SUCCESS);
} /* }}} */

/* {{{ proto mixed SplObjectStorage::key()
 */
SPL_METHOD(SplObjectStorage, key)
{
	spl_SplObjectStorage *intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	RETURN_LONG(intern->index);
} /* }}} */

/* {{{ proto mixed SplObjectStorage::current()
 */
SPL_METHOD(SplObjectStorage, current)
{
	zval **entry;
	spl_SplObjectStorage *intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (zend_hash_get_current_data_ex(&intern->storage, (void**)&entry, &intern->pos) == FAILURE) {
		return;
	}
	RETVAL_ZVAL(*entry, 1, 0);
} /* }}} */

/* {{{ proto void SplObjectStorage::next()
 */
SPL_METHOD(SplObjectStorage, next)
{
	spl_SplObjectStorage *intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	zend_hash_move_forward_ex(&intern->storage, &intern->pos);
	intern->index++;
} /* }}} */

static
ZEND_BEGIN_ARG_INFO(arginfo_Object, 0)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO();

static zend_function_entry spl_funcs_SplObjectStorage[] = {
	SPL_ME(SplObjectStorage,  attach,      arginfo_Object,        0)
	SPL_ME(SplObjectStorage,  detach,      arginfo_Object,        0)
	SPL_ME(SplObjectStorage,  contains,    arginfo_Object,        0)
	SPL_ME(SplObjectStorage,  count,       NULL,                  0)
	SPL_ME(SplObjectStorage,  rewind,      NULL,                  0)
	SPL_ME(SplObjectStorage,  valid,       NULL,                  0)
	SPL_ME(SplObjectStorage,  key,         NULL,                  0)
	SPL_ME(SplObjectStorage,  current,     NULL,                  0)
	SPL_ME(SplObjectStorage,  next,        NULL,                  0)
	{NULL, NULL, NULL}
};

/* {{{ PHP_MINIT_FUNCTION(spl_observer) */
PHP_MINIT_FUNCTION(spl_observer)
{
	REGISTER_SPL_INTERFACE(SplObserver);
	REGISTER_SPL_INTERFACE(SplSubject);

	REGISTER_SPL_STD_CLASS_EX(SplObjectStorage, spl_SplObjectStorage_new, spl_funcs_SplObjectStorage);
	memcpy(&spl_handler_SplObjectStorage, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

	REGISTER_SPL_IMPLEMENTS(SplObjectStorage, Countable);
	REGISTER_SPL_IMPLEMENTS(SplObjectStorage, Iterator);
	
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
