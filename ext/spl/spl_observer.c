/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
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
#include "ext/standard/php_array.h"
#include "ext/standard/php_var.h"
#include "zend_smart_str.h"
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
	HashTable         storage;
	zend_long         index;
	HashPosition      pos;
	zend_long         flags;
	zend_function    *fptr_get_hash;
	zval             *gcdata;
	size_t            gcdata_num;
	zend_object       std;
} spl_SplObjectStorage; /* }}} */

/* {{{ storage is an assoc aray of [zend_object*]=>[zval *obj, zval *inf] */
typedef struct _spl_SplObjectStorageElement {
	zval obj;
	zval inf;
} spl_SplObjectStorageElement; /* }}} */

static inline spl_SplObjectStorage *spl_object_storage_from_obj(zend_object *obj) /* {{{ */ {
	return (spl_SplObjectStorage*)((char*)(obj) - XtOffsetOf(spl_SplObjectStorage, std));
}
/* }}} */

#define Z_SPLOBJSTORAGE_P(zv)  spl_object_storage_from_obj(Z_OBJ_P((zv)))

void spl_SplObjectStorage_free_storage(zend_object *object) /* {{{ */
{
	spl_SplObjectStorage *intern = spl_object_storage_from_obj(object);

	zend_object_std_dtor(&intern->std);

	zend_hash_destroy(&intern->storage);

	if (intern->gcdata != NULL) {
		efree(intern->gcdata);
	}

} /* }}} */

static int spl_object_storage_get_hash(zend_hash_key *key, spl_SplObjectStorage *intern, zval *this, zval *obj) {
	if (intern->fptr_get_hash) {
		zval rv;
		zend_call_method_with_1_params(this, intern->std.ce, &intern->fptr_get_hash, "getHash", &rv, obj);
		if (!Z_ISUNDEF(rv)) {
			if (Z_TYPE(rv) == IS_STRING) {
				key->key = Z_STR(rv);
				return SUCCESS;
			} else {
				zend_throw_exception(spl_ce_RuntimeException, "Hash needs to be a string", 0);

				zval_ptr_dtor(&rv);
				return FAILURE;
			}
		} else {
			return FAILURE;
		}
	} else {
		key->key = NULL;
		key->h = Z_OBJ_HANDLE_P(obj);
		return SUCCESS;
	}
}

static void spl_object_storage_free_hash(spl_SplObjectStorage *intern, zend_hash_key *key) {
	if (key->key) {
		zend_string_release(key->key);
	}
}

static void spl_object_storage_dtor(zval *element) /* {{{ */
{
	spl_SplObjectStorageElement *el = Z_PTR_P(element);
	zval_ptr_dtor(&el->obj);
	zval_ptr_dtor(&el->inf);
	efree(el);
} /* }}} */

static spl_SplObjectStorageElement* spl_object_storage_get(spl_SplObjectStorage *intern, zend_hash_key *key) /* {{{ */
{
	if (key->key) {
		return zend_hash_find_ptr(&intern->storage, key->key);
	} else {
		return zend_hash_index_find_ptr(&intern->storage, key->h);
	}
} /* }}} */

spl_SplObjectStorageElement *spl_object_storage_attach(spl_SplObjectStorage *intern, zval *this, zval *obj, zval *inf) /* {{{ */
{
	spl_SplObjectStorageElement *pelement, element;
	zend_hash_key key;
	if (spl_object_storage_get_hash(&key, intern, this, obj) == FAILURE) {
		return NULL;
	}

	pelement = spl_object_storage_get(intern, &key);

	if (pelement) {
		zval_ptr_dtor(&pelement->inf);
		if (inf) {
			ZVAL_COPY(&pelement->inf, inf);
		} else {
			ZVAL_NULL(&pelement->inf);
		}
		spl_object_storage_free_hash(intern, &key);
		return pelement;
	}

	ZVAL_COPY(&element.obj, obj);
	if (inf) {
		ZVAL_COPY(&element.inf, inf);
	} else {
		ZVAL_NULL(&element.inf);
	}
	if (key.key) {
		pelement = zend_hash_update_mem(&intern->storage, key.key, &element, sizeof(spl_SplObjectStorageElement));
	} else {
		pelement = zend_hash_index_update_mem(&intern->storage, key.h, &element, sizeof(spl_SplObjectStorageElement));
	}
	spl_object_storage_free_hash(intern, &key);
	return pelement;
} /* }}} */

static int spl_object_storage_detach(spl_SplObjectStorage *intern, zval *this, zval *obj) /* {{{ */
{
	int ret = FAILURE;
	zend_hash_key key;
	if (spl_object_storage_get_hash(&key, intern, this, obj) == FAILURE) {
		return ret;
	}
	if (key.key) {
		ret = zend_hash_del(&intern->storage, key.key);
	} else {
		ret = zend_hash_index_del(&intern->storage, key.h);
	}
	spl_object_storage_free_hash(intern, &key);

	return ret;
} /* }}}*/

void spl_object_storage_addall(spl_SplObjectStorage *intern, zval *this, spl_SplObjectStorage *other) { /* {{{ */
	spl_SplObjectStorageElement *element;

	ZEND_HASH_FOREACH_PTR(&other->storage, element) {
		spl_object_storage_attach(intern, this, &element->obj, &element->inf);
	} ZEND_HASH_FOREACH_END();

	intern->index = 0;
} /* }}} */

static zend_object *spl_object_storage_new_ex(zend_class_entry *class_type, zval *orig) /* {{{ */
{
	spl_SplObjectStorage *intern;
	zend_class_entry *parent = class_type;

	intern = emalloc(sizeof(spl_SplObjectStorage) + zend_object_properties_size(parent));
	memset(intern, 0, sizeof(spl_SplObjectStorage) - sizeof(zval));
	intern->pos = HT_INVALID_IDX;

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

	zend_hash_init(&intern->storage, 0, NULL, spl_object_storage_dtor, 0);

	intern->std.handlers = &spl_handler_SplObjectStorage;

	while (parent) {
		if (parent == spl_ce_SplObjectStorage) {
			if (class_type != spl_ce_SplObjectStorage) {
				intern->fptr_get_hash = zend_hash_str_find_ptr(&class_type->function_table, "gethash", sizeof("gethash") - 1);
				if (intern->fptr_get_hash->common.scope == spl_ce_SplObjectStorage) {
					intern->fptr_get_hash = NULL;
				}
			}
			break;
		}

		parent = parent->parent;
	}

	if (orig) {
		spl_SplObjectStorage *other = Z_SPLOBJSTORAGE_P(orig);
		spl_object_storage_addall(intern, orig, other);
	}

	return &intern->std;
}
/* }}} */

/* {{{ spl_object_storage_clone */
static zend_object *spl_object_storage_clone(zval *zobject)
{
	zend_object *old_object;
	zend_object *new_object;

	old_object = Z_OBJ_P(zobject);
	new_object = spl_object_storage_new_ex(old_object->ce, zobject);

	zend_objects_clone_members(new_object, old_object);

	return new_object;
}
/* }}} */

static HashTable* spl_object_storage_debug_info(zval *obj, int *is_temp) /* {{{ */
{
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(obj);
	spl_SplObjectStorageElement *element;
	HashTable *props;
	zval tmp, storage;
	zend_string *md5str;
	zend_string *zname;
	HashTable *debug_info;

	*is_temp = 1;

	props = Z_OBJPROP_P(obj);

	ALLOC_HASHTABLE(debug_info);
	ZEND_INIT_SYMTABLE_EX(debug_info, zend_hash_num_elements(props) + 1, 0);
	zend_hash_copy(debug_info, props, (copy_ctor_func_t)zval_add_ref);

	array_init(&storage);

	ZEND_HASH_FOREACH_PTR(&intern->storage, element) {
		md5str = php_spl_object_hash(&element->obj);
		array_init(&tmp);
		/* Incrementing the refcount of obj and inf would confuse the garbage collector.
		 * Prefer to null the destructor */
		Z_ARRVAL_P(&tmp)->pDestructor = NULL;
		add_assoc_zval_ex(&tmp, "obj", sizeof("obj") - 1, &element->obj);
		add_assoc_zval_ex(&tmp, "inf", sizeof("inf") - 1, &element->inf);
		zend_hash_update(Z_ARRVAL(storage), md5str, &tmp);
		zend_string_release(md5str);
	} ZEND_HASH_FOREACH_END();

	zname = spl_gen_private_prop_name(spl_ce_SplObjectStorage, "storage", sizeof("storage")-1);
	zend_symtable_update(debug_info, zname, &storage);
	zend_string_release(zname);

	return debug_info;
}
/* }}} */

/* overriden for garbage collection */
static HashTable *spl_object_storage_get_gc(zval *obj, zval **table, int *n) /* {{{ */
{
	int i = 0;
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(obj);
	spl_SplObjectStorageElement *element;

	if (intern->storage.nNumOfElements * 2 > intern->gcdata_num) {
		intern->gcdata_num = intern->storage.nNumOfElements * 2;
		intern->gcdata = (zval*)erealloc(intern->gcdata, sizeof(zval) * intern->gcdata_num);
	}

	ZEND_HASH_FOREACH_PTR(&intern->storage, element) {
		ZVAL_COPY_VALUE(&intern->gcdata[i++], &element->obj);
		ZVAL_COPY_VALUE(&intern->gcdata[i++], &element->inf);
	} ZEND_HASH_FOREACH_END();

	*table = intern->gcdata;
	*n = i;

	return std_object_handlers.get_properties(obj);
}
/* }}} */

static int spl_object_storage_compare_info(zval *e1, zval *e2) /* {{{ */
{
	spl_SplObjectStorageElement *s1 = (spl_SplObjectStorageElement*)Z_PTR_P(e1);
	spl_SplObjectStorageElement *s2 = (spl_SplObjectStorageElement*)Z_PTR_P(e2);
	zval result;

	if (compare_function(&result, &s1->inf, &s2->inf) == FAILURE) {
		return 1;
	}

	return Z_LVAL(result) > 0 ? 1 : (Z_LVAL(result) < 0 ? -1 : 0);
}
/* }}} */

static int spl_object_storage_compare_objects(zval *o1, zval *o2) /* {{{ */
{
	zend_object *zo1 = (zend_object *)Z_OBJ_P(o1);
	zend_object *zo2 = (zend_object *)Z_OBJ_P(o2);

	if (zo1->ce != spl_ce_SplObjectStorage || zo2->ce != spl_ce_SplObjectStorage) {
		return 1;
	}

	return zend_hash_compare(&(Z_SPLOBJSTORAGE_P(o1))->storage, &(Z_SPLOBJSTORAGE_P(o2))->storage, (compare_func_t)spl_object_storage_compare_info, 0);
}
/* }}} */

/* {{{ spl_array_object_new */
static zend_object *spl_SplObjectStorage_new(zend_class_entry *class_type)
{
	return spl_object_storage_new_ex(class_type, NULL);
}
/* }}} */

int spl_object_storage_contains(spl_SplObjectStorage *intern, zval *this, zval *obj) /* {{{ */
{
	int found;
	zend_hash_key key;
	if (spl_object_storage_get_hash(&key, intern, this, obj) == FAILURE) {
		return 0;
	}

	if (key.key) {
		found = zend_hash_exists(&intern->storage, key.key);
	} else {
		found = zend_hash_index_exists(&intern->storage, key.h);
	}
	spl_object_storage_free_hash(intern, &key);
	return found;
} /* }}} */

/* {{{ proto void SplObjectStorage::attach(object obj, mixed inf = NULL)
 Attaches an object to the storage if not yet contained */
SPL_METHOD(SplObjectStorage, attach)
{
	zval *obj, *inf = NULL;

	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "o|z!", &obj, &inf) == FAILURE) {
		return;
	}
	spl_object_storage_attach(intern, getThis(), obj, inf);
} /* }}} */

/* {{{ proto void SplObjectStorage::detach(object obj)
 Detaches an object from the storage */
SPL_METHOD(SplObjectStorage, detach)
{
	zval *obj;
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "o", &obj) == FAILURE) {
		return;
	}
	spl_object_storage_detach(intern, getThis(), obj);

	zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
	intern->index = 0;
} /* }}} */

/* {{{ proto string SplObjectStorage::getHash(object obj)
 Returns the hash of an object */
SPL_METHOD(SplObjectStorage, getHash)
{
	zval *obj;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "o", &obj) == FAILURE) {
		return;
	}

	RETURN_NEW_STR(php_spl_object_hash(obj));

} /* }}} */

/* {{{ proto mixed SplObjectStorage::offsetGet(object obj)
 Returns associated information for a stored object */
SPL_METHOD(SplObjectStorage, offsetGet)
{
	zval *obj;
	spl_SplObjectStorageElement *element;
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(getThis());
	zend_hash_key key;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "o", &obj) == FAILURE) {
		return;
	}

	if (spl_object_storage_get_hash(&key, intern, getThis(), obj) == FAILURE) {
		return;
	}

	element = spl_object_storage_get(intern, &key);
	spl_object_storage_free_hash(intern, &key);

	if (!element) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0, "Object not found");
	} else {
		zval *value = &element->inf;

		ZVAL_DEREF(value);
		ZVAL_COPY(return_value, value);
	}
} /* }}} */

/* {{{ proto bool SplObjectStorage::addAll(SplObjectStorage $os)
 Add all elements contained in $os */
SPL_METHOD(SplObjectStorage, addAll)
{
	zval *obj;
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(getThis());
	spl_SplObjectStorage *other;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &obj, spl_ce_SplObjectStorage) == FAILURE) {
		return;
	}

	other = Z_SPLOBJSTORAGE_P(obj);

	spl_object_storage_addall(intern, getThis(), other);

	RETURN_LONG(zend_hash_num_elements(&intern->storage));
} /* }}} */

/* {{{ proto bool SplObjectStorage::removeAll(SplObjectStorage $os)
 Remove all elements contained in $os */
SPL_METHOD(SplObjectStorage, removeAll)
{
	zval *obj;
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(getThis());
	spl_SplObjectStorage *other;
	spl_SplObjectStorageElement *element;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &obj, spl_ce_SplObjectStorage) == FAILURE) {
		return;
	}

	other = Z_SPLOBJSTORAGE_P(obj);

	zend_hash_internal_pointer_reset(&other->storage);
	while ((element = zend_hash_get_current_data_ptr(&other->storage)) != NULL) {
		if (spl_object_storage_detach(intern, getThis(), &element->obj) == FAILURE) {
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
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(getThis());
	spl_SplObjectStorage *other;
	spl_SplObjectStorageElement *element;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &obj, spl_ce_SplObjectStorage) == FAILURE) {
		return;
	}

	other = Z_SPLOBJSTORAGE_P(obj);

	ZEND_HASH_FOREACH_PTR(&intern->storage, element) {
		if (!spl_object_storage_contains(other, getThis(), &element->obj)) {
			spl_object_storage_detach(intern, getThis(), &element->obj);
		}
	} ZEND_HASH_FOREACH_END();

	zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
	intern->index = 0;

	RETURN_LONG(zend_hash_num_elements(&intern->storage));
}
/* }}} */

/* {{{ proto bool SplObjectStorage::contains(object obj)
 Determine whethe an object is contained in the storage */
SPL_METHOD(SplObjectStorage, contains)
{
	zval *obj;
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "o", &obj) == FAILURE) {
		return;
	}
	RETURN_BOOL(spl_object_storage_contains(intern, getThis(), obj));
} /* }}} */

/* {{{ proto int SplObjectStorage::count()
 Determine number of objects in storage */
SPL_METHOD(SplObjectStorage, count)
{
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(getThis());
	zend_long mode = COUNT_NORMAL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &mode) == FAILURE) {
		return;
	}

	if (mode == COUNT_RECURSIVE) {
		zend_long ret = zend_hash_num_elements(&intern->storage);
		zval *element;

		ZEND_HASH_FOREACH_VAL(&intern->storage, element) {
			ret += php_count_recursive(element, mode);
		} ZEND_HASH_FOREACH_END();

		RETURN_LONG(ret);
		return;
	}

	RETURN_LONG(zend_hash_num_elements(&intern->storage));
} /* }}} */

/* {{{ proto void SplObjectStorage::rewind()
 Rewind to first position */
SPL_METHOD(SplObjectStorage, rewind)
{
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(getThis());

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
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(zend_hash_has_more_elements_ex(&intern->storage, &intern->pos) == SUCCESS);
} /* }}} */

/* {{{ proto mixed SplObjectStorage::key()
 Returns current key */
SPL_METHOD(SplObjectStorage, key)
{
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(getThis());

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
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if ((element = zend_hash_get_current_data_ptr_ex(&intern->storage, &intern->pos)) == NULL) {
		return;
	}
	ZVAL_COPY(return_value, &element->obj);
} /* }}} */

/* {{{ proto mixed SplObjectStorage::getInfo()
 Returns associated information to current element */
SPL_METHOD(SplObjectStorage, getInfo)
{
	spl_SplObjectStorageElement *element;
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if ((element = zend_hash_get_current_data_ptr_ex(&intern->storage, &intern->pos)) == NULL) {
		return;
	}
	ZVAL_COPY(return_value, &element->inf);
} /* }}} */

/* {{{ proto mixed SplObjectStorage::setInfo(mixed $inf)
 Sets associated information of current element to $inf */
SPL_METHOD(SplObjectStorage, setInfo)
{
	spl_SplObjectStorageElement *element;
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(getThis());
	zval *inf;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &inf) == FAILURE) {
		return;
	}

	if ((element = zend_hash_get_current_data_ptr_ex(&intern->storage, &intern->pos)) == NULL) {
		return;
	}
	zval_ptr_dtor(&element->inf);
	ZVAL_COPY(&element->inf, inf);
} /* }}} */

/* {{{ proto void SplObjectStorage::next()
 Moves position forward */
SPL_METHOD(SplObjectStorage, next)
{
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(getThis());

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
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(getThis());

	spl_SplObjectStorageElement *element;
	zval members, flags;
	HashPosition      pos;
	php_serialize_data_t var_hash;
	smart_str buf = {0};

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	PHP_VAR_SERIALIZE_INIT(var_hash);

	/* storage */
	smart_str_appendl(&buf, "x:", 2);
	ZVAL_LONG(&flags, zend_hash_num_elements(&intern->storage));
	php_var_serialize(&buf, &flags, &var_hash);
	zval_ptr_dtor(&flags);

	zend_hash_internal_pointer_reset_ex(&intern->storage, &pos);

	while (zend_hash_has_more_elements_ex(&intern->storage, &pos) == SUCCESS) {
		if ((element = zend_hash_get_current_data_ptr_ex(&intern->storage, &pos)) == NULL) {
			smart_str_free(&buf);
			PHP_VAR_SERIALIZE_DESTROY(var_hash);
			RETURN_NULL();
		}
		php_var_serialize(&buf, &element->obj, &var_hash);
		smart_str_appendc(&buf, ',');
		php_var_serialize(&buf, &element->inf, &var_hash);
		smart_str_appendc(&buf, ';');
		zend_hash_move_forward_ex(&intern->storage, &pos);
	}

	/* members */
	smart_str_appendl(&buf, "m:", 2);

	ZVAL_ARR(&members, zend_array_dup(zend_std_get_properties(getThis())));
	php_var_serialize(&buf, &members, &var_hash); /* finishes the string */
	zval_ptr_dtor(&members);

	/* done */
	PHP_VAR_SERIALIZE_DESTROY(var_hash);

	if (buf.s) {
		RETURN_NEW_STR(buf.s);
	} else {
		RETURN_NULL();
	}

} /* }}} */

/* {{{ proto void SplObjectStorage::unserialize(string serialized)
 Unserializes storage */
SPL_METHOD(SplObjectStorage, unserialize)
{
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(getThis());

	char *buf;
	size_t buf_len;
	const unsigned char *p, *s;
	php_unserialize_data_t var_hash;
	zval entry, inf;
	zval *pcount, *pmembers;
	spl_SplObjectStorageElement *element;
	zend_long count;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &buf, &buf_len) == FAILURE) {
		return;
	}

	if (buf_len == 0) {
		return;
	}

	/* storage */
	s = p = (const unsigned char*)buf;
	PHP_VAR_UNSERIALIZE_INIT(var_hash);

	if (*p!= 'x' || *++p != ':') {
		goto outexcept;
	}
	++p;

	pcount = var_tmp_var(&var_hash);
	if (!php_var_unserialize(pcount, &p, s + buf_len, &var_hash) || Z_TYPE_P(pcount) != IS_LONG) {
		goto outexcept;
	}

	--p; /* for ';' */
	count = Z_LVAL_P(pcount);

	while (count-- > 0) {
		spl_SplObjectStorageElement *pelement;
		zend_hash_key key;

		if (*p != ';') {
			goto outexcept;
		}
		++p;
		if(*p != 'O' && *p != 'C' && *p != 'r') {
			goto outexcept;
		}
		/* store reference to allow cross-references between different elements */
		if (!php_var_unserialize(&entry, &p, s + buf_len, &var_hash)) {
			goto outexcept;
		}
		if (Z_TYPE(entry) != IS_OBJECT) {
			zval_ptr_dtor(&entry);
			goto outexcept;
		}
		if (*p == ',') { /* new version has inf */
			++p;
			if (!php_var_unserialize(&inf, &p, s + buf_len, &var_hash)) {
				zval_ptr_dtor(&entry);
				goto outexcept;
			}
		} else {
			ZVAL_UNDEF(&inf);
		}

		if (spl_object_storage_get_hash(&key, intern, getThis(), &entry) == FAILURE) {
			zval_ptr_dtor(&entry);
			zval_ptr_dtor(&inf);
			goto outexcept;
		}
		pelement = spl_object_storage_get(intern, &key);
		spl_object_storage_free_hash(intern, &key);
		if (pelement) {
			if (!Z_ISUNDEF(pelement->inf)) {
				var_push_dtor(&var_hash, &pelement->inf);
			}
			if (!Z_ISUNDEF(pelement->obj)) {
				var_push_dtor(&var_hash, &pelement->obj);
			}
		}
		element = spl_object_storage_attach(intern, getThis(), &entry, Z_ISUNDEF(inf)?NULL:&inf);
		var_replace(&var_hash, &entry, &element->obj);
		var_replace(&var_hash, &inf, &element->inf);
		zval_ptr_dtor(&entry);
		ZVAL_UNDEF(&entry);
		zval_ptr_dtor(&inf);
		ZVAL_UNDEF(&inf);
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

	pmembers = var_tmp_var(&var_hash);
	if (!php_var_unserialize(pmembers, &p, s + buf_len, &var_hash) || Z_TYPE_P(pmembers) != IS_ARRAY) {
		goto outexcept;
	}

	/* copy members */
	object_properties_load(&intern->std, Z_ARRVAL_P(pmembers));

	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
	return;

outexcept:
	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
	zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0, "Error at offset %pd of %d bytes", (zend_long)((char*)p - buf), buf_len);
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
	zend_long               flags = MIT_NEED_ALL|MIT_KEYS_NUMERIC;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "|l", &flags) == FAILURE) {
		return;
	}

	intern = Z_SPLOBJSTORAGE_P(getThis());
	intern->flags = flags;
}
/* }}} */

/* {{{ proto int MultipleIterator::getFlags()
   Return current flags */
SPL_METHOD(MultipleIterator, getFlags)
{
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(getThis());

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
	intern = Z_SPLOBJSTORAGE_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &intern->flags) == FAILURE) {
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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|z!", &iterator, zend_ce_iterator, &info) == FAILURE) {
		return;
	}

	intern = Z_SPLOBJSTORAGE_P(getThis());

	if (info != NULL) {
		spl_SplObjectStorageElement *element;

		if (Z_TYPE_P(info) != IS_LONG && Z_TYPE_P(info) != IS_STRING) {
			zend_throw_exception(spl_ce_InvalidArgumentException, "Info must be NULL, integer or string", 0);
			return;
		}

		zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
		while ((element = zend_hash_get_current_data_ptr_ex(&intern->storage, &intern->pos)) != NULL) {
			if (fast_is_identical_function(info, &element->inf)) {
				zend_throw_exception(spl_ce_InvalidArgumentException, "Key duplication error", 0);
				return;
			}
			zend_hash_move_forward_ex(&intern->storage, &intern->pos);
		}
	}

	spl_object_storage_attach(intern, getThis(), iterator, info);
}
/* }}} */

/* {{{ proto void MultipleIterator::rewind()
   Rewind all attached iterator instances */
SPL_METHOD(MultipleIterator, rewind)
{
	spl_SplObjectStorage        *intern;
	spl_SplObjectStorageElement *element;
	zval                        *it;

	intern = Z_SPLOBJSTORAGE_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
	while ((element = zend_hash_get_current_data_ptr_ex(&intern->storage, &intern->pos)) != NULL && !EG(exception)) {
		it = &element->obj;
		zend_call_method_with_0_params(it, Z_OBJCE_P(it), &Z_OBJCE_P(it)->iterator_funcs.zf_rewind, "rewind", NULL);
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

	intern = Z_SPLOBJSTORAGE_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
	while ((element = zend_hash_get_current_data_ptr_ex(&intern->storage, &intern->pos)) != NULL && !EG(exception)) {
		it = &element->obj;
		zend_call_method_with_0_params(it, Z_OBJCE_P(it), &Z_OBJCE_P(it)->iterator_funcs.zf_next, "next", NULL);
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
	zval                        *it, retval;
	zend_long                         expect, valid;

	intern = Z_SPLOBJSTORAGE_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (!zend_hash_num_elements(&intern->storage)) {
		RETURN_FALSE;
	}

	expect = (intern->flags & MIT_NEED_ALL) ? 1 : 0;

	zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
	while ((element = zend_hash_get_current_data_ptr_ex(&intern->storage, &intern->pos)) != NULL && !EG(exception)) {
		it = &element->obj;
		zend_call_method_with_0_params(it, Z_OBJCE_P(it), &Z_OBJCE_P(it)->iterator_funcs.zf_valid, "valid", &retval);

		if (!Z_ISUNDEF(retval)) {
			valid = (Z_TYPE(retval) == IS_TRUE);
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

static void spl_multiple_iterator_get_all(spl_SplObjectStorage *intern, int get_type, zval *return_value) /* {{{ */
{
	spl_SplObjectStorageElement *element;
	zval                        *it, retval;
	int                          valid = 1, num_elements;

	num_elements = zend_hash_num_elements(&intern->storage);
	if (num_elements < 1) {
		RETURN_FALSE;
	}

	array_init_size(return_value, num_elements);

	zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
	while ((element = zend_hash_get_current_data_ptr_ex(&intern->storage, &intern->pos)) != NULL && !EG(exception)) {
		it = &element->obj;
		zend_call_method_with_0_params(it, Z_OBJCE_P(it), &Z_OBJCE_P(it)->iterator_funcs.zf_valid, "valid", &retval);

		if (!Z_ISUNDEF(retval)) {
			valid = Z_TYPE(retval) == IS_TRUE;
			zval_ptr_dtor(&retval);
		} else {
			valid = 0;
		}

		if (valid) {
			if (SPL_MULTIPLE_ITERATOR_GET_ALL_CURRENT == get_type) {
				zend_call_method_with_0_params(it, Z_OBJCE_P(it), &Z_OBJCE_P(it)->iterator_funcs.zf_current, "current", &retval);
			} else {
				zend_call_method_with_0_params(it, Z_OBJCE_P(it), &Z_OBJCE_P(it)->iterator_funcs.zf_key, "key", &retval);
			}
			if (Z_ISUNDEF(retval)) {
				zend_throw_exception(spl_ce_RuntimeException, "Failed to call sub iterator method", 0);
				return;
			}
		} else if (intern->flags & MIT_NEED_ALL) {
			if (SPL_MULTIPLE_ITERATOR_GET_ALL_CURRENT == get_type) {
				zend_throw_exception(spl_ce_RuntimeException, "Called current() with non valid sub iterator", 0);
			} else {
				zend_throw_exception(spl_ce_RuntimeException, "Called key() with non valid sub iterator", 0);
			}
			return;
		} else {
			ZVAL_NULL(&retval);
		}

		if (intern->flags & MIT_KEYS_ASSOC) {
			switch (Z_TYPE(element->inf)) {
				case IS_LONG:
					add_index_zval(return_value, Z_LVAL(element->inf), &retval);
					break;
				case IS_STRING:
					zend_symtable_update(Z_ARRVAL_P(return_value), Z_STR(element->inf), &retval);
					break;
				default:
					zval_ptr_dtor(&retval);
					zend_throw_exception(spl_ce_InvalidArgumentException, "Sub-Iterator is associated with NULL", 0);
					return;
			}
		} else {
			add_next_index_zval(return_value, &retval);
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
	intern = Z_SPLOBJSTORAGE_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	spl_multiple_iterator_get_all(intern, SPL_MULTIPLE_ITERATOR_GET_ALL_CURRENT, return_value);
}
/* }}} */

/* {{{ proto array MultipleIterator::key()
   Return an array of all registered Iterator instances key() result */
SPL_METHOD(MultipleIterator, key)
{
	spl_SplObjectStorage *intern;
	intern = Z_SPLOBJSTORAGE_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	spl_multiple_iterator_get_all(intern, SPL_MULTIPLE_ITERATOR_GET_ALL_KEY, return_value);
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

	spl_handler_SplObjectStorage.offset          = XtOffsetOf(spl_SplObjectStorage, std);
	spl_handler_SplObjectStorage.get_debug_info  = spl_object_storage_debug_info;
	spl_handler_SplObjectStorage.compare_objects = spl_object_storage_compare_objects;
	spl_handler_SplObjectStorage.clone_obj       = spl_object_storage_clone;
	spl_handler_SplObjectStorage.get_gc          = spl_object_storage_get_gc;
	spl_handler_SplObjectStorage.dtor_obj        = zend_objects_destroy_object;
	spl_handler_SplObjectStorage.free_obj        = spl_SplObjectStorage_free_storage;

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
