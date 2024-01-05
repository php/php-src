/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Marcus Boerger <helly@php.net>                              |
   |          Etienne Kneuss <colder@php.net>                             |
   +----------------------------------------------------------------------+
 */

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
#include "spl_observer_arginfo.h"
#include "spl_iterators.h"
#include "spl_array.h"
#include "spl_exceptions.h"

PHPAPI zend_class_entry     *spl_ce_SplObserver;
PHPAPI zend_class_entry     *spl_ce_SplSubject;
PHPAPI zend_class_entry     *spl_ce_SplObjectStorage;
PHPAPI zend_class_entry     *spl_ce_MultipleIterator;

PHPAPI zend_object_handlers spl_handler_SplObjectStorage;

/* Bit flags for marking internal functionality overridden by SplObjectStorage subclasses. */
#define SOS_OVERRIDDEN_READ_DIMENSION  1
#define SOS_OVERRIDDEN_WRITE_DIMENSION 2
#define SOS_OVERRIDDEN_UNSET_DIMENSION 4

typedef struct _spl_SplObjectStorage { /* {{{ */
	HashTable         storage;
	zend_long         index;
	HashPosition      pos;
	/* In SplObjectStorage, flags is a hidden implementation detail to optimize ArrayAccess handlers.
	 * In MultipleIterator on a different class hierarchy, flags is a user settable value controlling iteration behavior. */
	zend_long         flags;
	zend_function    *fptr_get_hash;
	zend_object       std;
} spl_SplObjectStorage; /* }}} */

/* {{{ storage is an assoc array of [zend_object*]=>[zval *obj, zval *inf] */
typedef struct _spl_SplObjectStorageElement {
	zend_object *obj;
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
} /* }}} */

static zend_result spl_object_storage_get_hash(zend_hash_key *key, spl_SplObjectStorage *intern, zend_object *obj) {
	if (UNEXPECTED(intern->fptr_get_hash)) {
		zval param;
		zval rv;
		ZVAL_OBJ(&param, obj);
		zend_call_method_with_1_params(
			&intern->std, intern->std.ce, &intern->fptr_get_hash, "getHash", &rv, &param);
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
		key->h = obj->handle;
		return SUCCESS;
	}
}

static void spl_object_storage_free_hash(spl_SplObjectStorage *intern, zend_hash_key *key) {
	if (key->key) {
		zend_string_release_ex(key->key, 0);
	}
}

static void spl_object_storage_dtor(zval *element) /* {{{ */
{
	spl_SplObjectStorageElement *el = Z_PTR_P(element);
	zend_object_release(el->obj);
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

static spl_SplObjectStorageElement *spl_object_storage_create_element(zend_object *obj, zval *inf) /* {{{ */
{
	spl_SplObjectStorageElement *pelement = emalloc(sizeof(spl_SplObjectStorageElement));
	pelement->obj = obj;
	GC_ADDREF(obj);
	if (inf) {
		ZVAL_COPY(&pelement->inf, inf);
	} else {
		ZVAL_NULL(&pelement->inf);
	}
	return pelement;
} /* }}} */

/* A faster version of spl_object_storage_attach used when neither SplObjectStorage->getHash nor SplObjectStorage->offsetSet is overridden. */
static spl_SplObjectStorageElement *spl_object_storage_attach_handle(spl_SplObjectStorage *intern, zend_object *obj, zval *inf) /* {{{ */
{
	uint32_t handle = obj->handle;
	zval *entry_zv = zend_hash_index_lookup(&intern->storage, handle);
	spl_SplObjectStorageElement *pelement;
	ZEND_ASSERT(!(intern->flags & SOS_OVERRIDDEN_WRITE_DIMENSION));

	if (Z_TYPE_P(entry_zv) != IS_NULL) {
		zval zv_inf;
		ZEND_ASSERT(Z_TYPE_P(entry_zv) == IS_PTR);
		pelement = Z_PTR_P(entry_zv);
		ZVAL_COPY_VALUE(&zv_inf, &pelement->inf);
		if (inf) {
			ZVAL_COPY(&pelement->inf, inf);
		} else {
			ZVAL_NULL(&pelement->inf);
		}
		/* Call the old value's destructor last, in case it moves the entry */
		zval_ptr_dtor(&zv_inf);
		return pelement;
	}

	pelement = spl_object_storage_create_element(obj, inf);
	ZVAL_PTR(entry_zv, pelement);
	return pelement;
} /* }}} */

static spl_SplObjectStorageElement *spl_object_storage_attach(spl_SplObjectStorage *intern, zend_object *obj, zval *inf) /* {{{ */
{
	if (EXPECTED(!(intern->flags & SOS_OVERRIDDEN_WRITE_DIMENSION))) {
		return spl_object_storage_attach_handle(intern, obj, inf);
	}
	/* getHash or offsetSet is overridden. */

	spl_SplObjectStorageElement *pelement, element;
	zend_hash_key key;
	if (spl_object_storage_get_hash(&key, intern, obj) == FAILURE) {
		return NULL;
	}

	pelement = spl_object_storage_get(intern, &key);

	if (pelement) {
		zval zv_inf;
		ZVAL_COPY_VALUE(&zv_inf, &pelement->inf);
		if (inf) {
			ZVAL_COPY(&pelement->inf, inf);
		} else {
			ZVAL_NULL(&pelement->inf);
		}
		spl_object_storage_free_hash(intern, &key);
		/* Call the old value's destructor last, in case it moves the entry */
		zval_ptr_dtor(&zv_inf);
		return pelement;
	}

	element.obj = obj;
	GC_ADDREF(obj);
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

static zend_result spl_object_storage_detach(spl_SplObjectStorage *intern, zend_object *obj) /* {{{ */
{
	if (EXPECTED(!(intern->flags & SOS_OVERRIDDEN_UNSET_DIMENSION))) {
		return zend_hash_index_del(&intern->storage, obj->handle);
	}
	zend_result ret = FAILURE;
	zend_hash_key key;
	if (spl_object_storage_get_hash(&key, intern, obj) == FAILURE) {
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

static void spl_object_storage_addall(spl_SplObjectStorage *intern, spl_SplObjectStorage *other) { /* {{{ */
	spl_SplObjectStorageElement *element;

	ZEND_HASH_FOREACH_PTR(&other->storage, element) {
		spl_object_storage_attach(intern, element->obj, &element->inf);
	} ZEND_HASH_FOREACH_END();

	intern->index = 0;
} /* }}} */

#define SPL_OBJECT_STORAGE_CLASS_HAS_OVERRIDE(class_type, zstr_method) \
	(class_type->arrayaccess_funcs_ptr && class_type->arrayaccess_funcs_ptr->zstr_method)

static zend_object *spl_object_storage_new_ex(zend_class_entry *class_type, zend_object *orig) /* {{{ */
{
	spl_SplObjectStorage *intern;

	intern = emalloc(sizeof(spl_SplObjectStorage) + zend_object_properties_size(class_type));
	memset(intern, 0, sizeof(spl_SplObjectStorage) - sizeof(zval));
	intern->pos = 0;

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

	zend_hash_init(&intern->storage, 0, NULL, spl_object_storage_dtor, 0);

	if (class_type != spl_ce_SplObjectStorage
			&& zend_class_entry_get_root(class_type) == spl_ce_SplObjectStorage) {
		/* Possible optimization: Cache these results with a map from class entry to IS_NULL/IS_PTR.
		 * Or maybe just a single item with the result for the most recently loaded subclass. */
		zend_function *get_hash = zend_hash_str_find_ptr(&class_type->function_table, "gethash", sizeof("gethash") - 1);
		if (get_hash->common.scope != spl_ce_SplObjectStorage) {
			intern->fptr_get_hash = get_hash;
		}
		if (intern->fptr_get_hash != NULL ||
			SPL_OBJECT_STORAGE_CLASS_HAS_OVERRIDE(class_type, zf_offsetget) ||
			SPL_OBJECT_STORAGE_CLASS_HAS_OVERRIDE(class_type, zf_offsetexists)) {
			intern->flags |= SOS_OVERRIDDEN_READ_DIMENSION;
		}

		if (intern->fptr_get_hash != NULL ||
			SPL_OBJECT_STORAGE_CLASS_HAS_OVERRIDE(class_type, zf_offsetset)) {
			intern->flags |= SOS_OVERRIDDEN_WRITE_DIMENSION;
		}

		if (intern->fptr_get_hash != NULL ||
			SPL_OBJECT_STORAGE_CLASS_HAS_OVERRIDE(class_type, zf_offsetunset)) {
			intern->flags |= SOS_OVERRIDDEN_UNSET_DIMENSION;
		}
	}

	if (orig) {
		spl_SplObjectStorage *other = spl_object_storage_from_obj(orig);
		spl_object_storage_addall(intern, other);
	}

	return &intern->std;
}
/* }}} */

/* {{{ spl_object_storage_clone */
static zend_object *spl_object_storage_clone(zend_object *old_object)
{
	zend_object *new_object;

	new_object = spl_object_storage_new_ex(old_object->ce, old_object);

	zend_objects_clone_members(new_object, old_object);

	return new_object;
}
/* }}} */

static inline HashTable* spl_object_storage_debug_info(zend_object *obj) /* {{{ */
{
	spl_SplObjectStorage *intern = spl_object_storage_from_obj(obj);
	spl_SplObjectStorageElement *element;
	HashTable *props;
	zval tmp, storage;
	zend_string *zname;
	HashTable *debug_info;

	props = obj->handlers->get_properties(obj);

	debug_info = zend_new_array(zend_hash_num_elements(props) + 1);
	zend_hash_copy(debug_info, props, (copy_ctor_func_t)zval_add_ref);

	array_init(&storage);

	ZEND_HASH_FOREACH_PTR(&intern->storage, element) {
		array_init(&tmp);
		/* Incrementing the refcount of obj and inf would confuse the garbage collector.
		 * Prefer to null the destructor */
		Z_ARRVAL_P(&tmp)->pDestructor = NULL;
		zval obj;
		ZVAL_OBJ(&obj, element->obj);
		add_assoc_zval_ex(&tmp, "obj", sizeof("obj") - 1, &obj);
		add_assoc_zval_ex(&tmp, "inf", sizeof("inf") - 1, &element->inf);
		zend_hash_next_index_insert(Z_ARRVAL(storage), &tmp);
	} ZEND_HASH_FOREACH_END();

	zname = spl_gen_private_prop_name(spl_ce_SplObjectStorage, "storage", sizeof("storage")-1);
	zend_symtable_update(debug_info, zname, &storage);
	zend_string_release_ex(zname, 0);

	return debug_info;
}
/* }}} */

/* overridden for garbage collection */
static HashTable *spl_object_storage_get_gc(zend_object *obj, zval **table, int *n) /* {{{ */
{
	spl_SplObjectStorage *intern = spl_object_storage_from_obj(obj);
	spl_SplObjectStorageElement *element;
	zend_get_gc_buffer *gc_buffer = zend_get_gc_buffer_create();

	ZEND_HASH_FOREACH_PTR(&intern->storage, element) {
		zend_get_gc_buffer_add_obj(gc_buffer, element->obj);
		zend_get_gc_buffer_add_zval(gc_buffer, &element->inf);
	} ZEND_HASH_FOREACH_END();

	zend_get_gc_buffer_use(gc_buffer, table, n);
	return zend_std_get_properties(obj);
}
/* }}} */

static int spl_object_storage_compare_info(zval *e1, zval *e2) /* {{{ */
{
	spl_SplObjectStorageElement *s1 = (spl_SplObjectStorageElement*)Z_PTR_P(e1);
	spl_SplObjectStorageElement *s2 = (spl_SplObjectStorageElement*)Z_PTR_P(e2);

	return zend_compare(&s1->inf, &s2->inf);
}
/* }}} */

static int spl_object_storage_compare_objects(zval *o1, zval *o2) /* {{{ */
{
	zend_object *zo1;
	zend_object *zo2;

	ZEND_COMPARE_OBJECTS_FALLBACK(o1, o2);

	zo1 = (zend_object *)Z_OBJ_P(o1);
	zo2 = (zend_object *)Z_OBJ_P(o2);

	if (zo1->ce != spl_ce_SplObjectStorage || zo2->ce != spl_ce_SplObjectStorage) {
		return ZEND_UNCOMPARABLE;
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

/* Returns true if the SplObjectStorage contains an entry for getHash(obj), even if the corresponding value is null. */
static bool spl_object_storage_contains(spl_SplObjectStorage *intern, zend_object *obj) /* {{{ */
{
	if (EXPECTED(!intern->fptr_get_hash)) {
		return zend_hash_index_find(&intern->storage, obj->handle) != NULL;
	}
	zend_hash_key key;
	if (spl_object_storage_get_hash(&key, intern, obj) == FAILURE) {
		return true;
	}

	ZEND_ASSERT(key.key);
	bool found = zend_hash_exists(&intern->storage, key.key);
	zend_string_release_ex(key.key, 0);

	return found;
} /* }}} */

/* {{{ Attaches an object to the storage if not yet contained */
PHP_METHOD(SplObjectStorage, attach)
{
	zend_object *obj;
	zval *inf = NULL;

	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_OBJ(obj)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(inf)
	ZEND_PARSE_PARAMETERS_END();
	spl_object_storage_attach(intern, obj, inf);
} /* }}} */

static int spl_object_storage_has_dimension(zend_object *object, zval *offset, int check_empty)
{
	spl_SplObjectStorage *intern = spl_object_storage_from_obj(object);
	if (UNEXPECTED(offset == NULL || Z_TYPE_P(offset) != IS_OBJECT || (intern->flags & SOS_OVERRIDDEN_READ_DIMENSION))) {
		/* Can't optimize empty()/isset() check if getHash, offsetExists, or offsetGet is overridden */
		return zend_std_has_dimension(object, offset, check_empty);
	}
	spl_SplObjectStorageElement *element = zend_hash_index_find_ptr(&intern->storage, Z_OBJ_HANDLE_P(offset));
	if (!element) {
		return 0;
	}

	if (check_empty) {
		return i_zend_is_true(&element->inf);
	}
	/* NOTE: SplObjectStorage->offsetExists() is an alias of SplObjectStorage->contains(), so this returns true even if the value is null. */
	return 1;
}

static zval *spl_object_storage_read_dimension(zend_object *object, zval *offset, int type, zval *rv)
{
	spl_SplObjectStorage *intern = spl_object_storage_from_obj(object);
	if (UNEXPECTED(offset == NULL || Z_TYPE_P(offset) != IS_OBJECT || (intern->flags & SOS_OVERRIDDEN_READ_DIMENSION))) {
		/* Can't optimize it if getHash, offsetExists, or offsetGet is overridden */
		return zend_std_read_dimension(object, offset, type, rv);
	}
	spl_SplObjectStorageElement *element = zend_hash_index_find_ptr(&intern->storage, Z_OBJ_HANDLE_P(offset));

	if (!element) {
		if (type == BP_VAR_IS) {
			return &EG(uninitialized_zval);
		}
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0, "Object not found");
		return NULL;
	} else {
		/* This deliberately returns a non-reference, even for BP_VAR_W and BP_VAR_RW, to behave the same way as SplObjectStorage did when using the default zend_std_read_dimension behavior.
		 * i.e. This prevents taking a reference to an entry of SplObjectStorage because offsetGet would return a non-reference. */
		ZVAL_COPY_DEREF(rv, &element->inf);
		return rv;
	}
}

static void spl_object_storage_write_dimension(zend_object *object, zval *offset, zval *inf)
{
	spl_SplObjectStorage *intern = spl_object_storage_from_obj(object);
	if (UNEXPECTED(offset == NULL || Z_TYPE_P(offset) != IS_OBJECT || (intern->flags & SOS_OVERRIDDEN_WRITE_DIMENSION))) {
		zend_std_write_dimension(object, offset, inf);
		return;
	}
	spl_object_storage_attach_handle(intern, Z_OBJ_P(offset), inf);
}

static void spl_object_storage_unset_dimension(zend_object *object, zval *offset)
{
	spl_SplObjectStorage *intern = spl_object_storage_from_obj(object);
	if (UNEXPECTED(Z_TYPE_P(offset) != IS_OBJECT || (intern->flags & SOS_OVERRIDDEN_UNSET_DIMENSION))) {
		zend_std_unset_dimension(object, offset);
		return;
	}
	zend_hash_index_del(&intern->storage, Z_OBJ_HANDLE_P(offset));
}

/* {{{ Detaches an object from the storage */
PHP_METHOD(SplObjectStorage, detach)
{
	zend_object *obj;
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJ(obj)
	ZEND_PARSE_PARAMETERS_END();
	spl_object_storage_detach(intern, obj);

	zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
	intern->index = 0;
} /* }}} */

/* {{{ Returns the hash of an object */
PHP_METHOD(SplObjectStorage, getHash)
{
	zend_object *obj;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJ(obj)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_NEW_STR(php_spl_object_hash(obj));

} /* }}} */

/* {{{ Returns associated information for a stored object */
PHP_METHOD(SplObjectStorage, offsetGet)
{
	zend_object *obj;
	spl_SplObjectStorageElement *element;
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(ZEND_THIS);
	zend_hash_key key;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJ(obj)
	ZEND_PARSE_PARAMETERS_END();

	if (spl_object_storage_get_hash(&key, intern, obj) == FAILURE) {
		RETURN_NULL();
	}

	element = spl_object_storage_get(intern, &key);
	spl_object_storage_free_hash(intern, &key);

	if (!element) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0, "Object not found");
	} else {
		RETURN_COPY_DEREF(&element->inf);
	}
} /* }}} */

/* {{{ Add all elements contained in $os */
PHP_METHOD(SplObjectStorage, addAll)
{
	zval *obj;
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(ZEND_THIS);
	spl_SplObjectStorage *other;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &obj, spl_ce_SplObjectStorage) == FAILURE) {
		RETURN_THROWS();
	}

	other = Z_SPLOBJSTORAGE_P(obj);

	spl_object_storage_addall(intern, other);

	RETURN_LONG(zend_hash_num_elements(&intern->storage));
} /* }}} */

/* {{{ Remove all elements contained in $os */
PHP_METHOD(SplObjectStorage, removeAll)
{
	zval *obj;
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(ZEND_THIS);
	spl_SplObjectStorage *other;
	spl_SplObjectStorageElement *element;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &obj, spl_ce_SplObjectStorage) == FAILURE) {
		RETURN_THROWS();
	}

	other = Z_SPLOBJSTORAGE_P(obj);

	zend_hash_internal_pointer_reset(&other->storage);
	while ((element = zend_hash_get_current_data_ptr(&other->storage)) != NULL) {
		if (spl_object_storage_detach(intern, element->obj) == FAILURE) {
			zend_hash_move_forward(&other->storage);
		}
	}

	zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
	intern->index = 0;

	RETURN_LONG(zend_hash_num_elements(&intern->storage));
} /* }}} */

/* {{{ Remove elements not common to both this SplObjectStorage instance and $os */
PHP_METHOD(SplObjectStorage, removeAllExcept)
{
	zval *obj;
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(ZEND_THIS);
	spl_SplObjectStorage *other;
	spl_SplObjectStorageElement *element;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &obj, spl_ce_SplObjectStorage) == FAILURE) {
		RETURN_THROWS();
	}

	other = Z_SPLOBJSTORAGE_P(obj);

	ZEND_HASH_FOREACH_PTR(&intern->storage, element) {
		if (!spl_object_storage_contains(other, element->obj)) {
			spl_object_storage_detach(intern, element->obj);
		}
	} ZEND_HASH_FOREACH_END();

	zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
	intern->index = 0;

	RETURN_LONG(zend_hash_num_elements(&intern->storage));
}
/* }}} */

/* {{{ Determine whether an object is contained in the storage */
PHP_METHOD(SplObjectStorage, contains)
{
	zend_object *obj;
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJ(obj)
	ZEND_PARSE_PARAMETERS_END();
	RETURN_BOOL(spl_object_storage_contains(intern, obj));
} /* }}} */

/* {{{ Determine number of objects in storage */
PHP_METHOD(SplObjectStorage, count)
{
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(ZEND_THIS);
	zend_long mode = PHP_COUNT_NORMAL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &mode) == FAILURE) {
		RETURN_THROWS();
	}

	if (mode == PHP_COUNT_RECURSIVE) {
		RETURN_LONG(php_count_recursive(&intern->storage));
	}

	RETURN_LONG(zend_hash_num_elements(&intern->storage));
} /* }}} */

/* {{{ Rewind to first position */
PHP_METHOD(SplObjectStorage, rewind)
{
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
	intern->index = 0;
} /* }}} */

/* {{{ Returns whether current position is valid */
PHP_METHOD(SplObjectStorage, valid)
{
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_BOOL(zend_hash_has_more_elements_ex(&intern->storage, &intern->pos) == SUCCESS);
} /* }}} */

/* {{{ Returns current key */
PHP_METHOD(SplObjectStorage, key)
{
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_LONG(intern->index);
} /* }}} */

/* {{{ Returns current element */
PHP_METHOD(SplObjectStorage, current)
{
	spl_SplObjectStorageElement *element;
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if ((element = zend_hash_get_current_data_ptr_ex(&intern->storage, &intern->pos)) == NULL) {
		zend_throw_exception(spl_ce_RuntimeException, "Called current() on invalid iterator", 0);
		RETURN_THROWS();
	}
	ZVAL_OBJ_COPY(return_value, element->obj);
} /* }}} */

/* {{{ Returns associated information to current element */
PHP_METHOD(SplObjectStorage, getInfo)
{
	spl_SplObjectStorageElement *element;
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if ((element = zend_hash_get_current_data_ptr_ex(&intern->storage, &intern->pos)) == NULL) {
		RETURN_NULL();
	}
	ZVAL_COPY(return_value, &element->inf);
} /* }}} */

/* {{{ Sets associated information of current element to $inf */
PHP_METHOD(SplObjectStorage, setInfo)
{
	spl_SplObjectStorageElement *element;
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(ZEND_THIS);
	zval *inf;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &inf) == FAILURE) {
		RETURN_THROWS();
	}

	if ((element = zend_hash_get_current_data_ptr_ex(&intern->storage, &intern->pos)) == NULL) {
		RETURN_NULL();
	}
	zval_ptr_dtor(&element->inf);
	ZVAL_COPY(&element->inf, inf);
} /* }}} */

/* {{{ Moves position forward */
PHP_METHOD(SplObjectStorage, next)
{
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	zend_hash_move_forward_ex(&intern->storage, &intern->pos);
	intern->index++;
} /* }}} */

/* {{{ Serializes storage */
PHP_METHOD(SplObjectStorage, serialize)
{
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(ZEND_THIS);

	spl_SplObjectStorageElement *element;
	zval members, flags;
	HashPosition      pos;
	php_serialize_data_t var_hash;
	smart_str buf = {0};

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	PHP_VAR_SERIALIZE_INIT(var_hash);

	/* storage */
	smart_str_appendl(&buf, "x:", 2);
	ZVAL_LONG(&flags, zend_hash_num_elements(&intern->storage));
	php_var_serialize(&buf, &flags, &var_hash);

	zend_hash_internal_pointer_reset_ex(&intern->storage, &pos);

	while (zend_hash_has_more_elements_ex(&intern->storage, &pos) == SUCCESS) {
		zval obj;
		if ((element = zend_hash_get_current_data_ptr_ex(&intern->storage, &pos)) == NULL) {
			smart_str_free(&buf);
			PHP_VAR_SERIALIZE_DESTROY(var_hash);
			RETURN_NULL();
		}
		ZVAL_OBJ(&obj, element->obj);
		php_var_serialize(&buf, &obj, &var_hash);
		smart_str_appendc(&buf, ',');
		php_var_serialize(&buf, &element->inf, &var_hash);
		smart_str_appendc(&buf, ';');
		zend_hash_move_forward_ex(&intern->storage, &pos);
	}

	/* members */
	smart_str_appendl(&buf, "m:", 2);

	ZVAL_ARR(&members, zend_array_dup(zend_std_get_properties(Z_OBJ_P(ZEND_THIS))));
	php_var_serialize(&buf, &members, &var_hash); /* finishes the string */
	zval_ptr_dtor(&members);

	/* done */
	PHP_VAR_SERIALIZE_DESTROY(var_hash);

	RETURN_STR(smart_str_extract(&buf));
} /* }}} */

/* {{{ Unserializes storage */
PHP_METHOD(SplObjectStorage, unserialize)
{
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(ZEND_THIS);

	char *buf;
	size_t buf_len;
	const unsigned char *p, *s;
	php_unserialize_data_t var_hash;
	zval *pcount, *pmembers;
	spl_SplObjectStorageElement *element;
	zend_long count;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &buf, &buf_len) == FAILURE) {
		RETURN_THROWS();
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
	if (count < 0) {
		goto outexcept;
	}

	while (count-- > 0) {
		spl_SplObjectStorageElement *pelement;
		zend_hash_key key;
		zval *entry = var_tmp_var(&var_hash);
		zval inf;
		ZVAL_UNDEF(&inf);

		if (*p != ';') {
			goto outexcept;
		}
		++p;
		if(*p != 'O' && *p != 'C' && *p != 'r') {
			goto outexcept;
		}
		/* store reference to allow cross-references between different elements */
		if (!php_var_unserialize(entry, &p, s + buf_len, &var_hash)) {
			goto outexcept;
		}
		if (*p == ',') { /* new version has inf */
			++p;
			if (!php_var_unserialize(&inf, &p, s + buf_len, &var_hash)) {
				zval_ptr_dtor(&inf);
				goto outexcept;
			}
		}
		if (Z_TYPE_P(entry) != IS_OBJECT) {
			zval_ptr_dtor(&inf);
			goto outexcept;
		}

		if (spl_object_storage_get_hash(&key, intern, Z_OBJ_P(entry)) == FAILURE) {
			zval_ptr_dtor(&inf);
			goto outexcept;
		}
		pelement = spl_object_storage_get(intern, &key);
		spl_object_storage_free_hash(intern, &key);
		if (pelement) {
			zval obj;
			if (!Z_ISUNDEF(pelement->inf)) {
				var_push_dtor(&var_hash, &pelement->inf);
			}
			ZVAL_OBJ(&obj, pelement->obj);
			var_push_dtor(&var_hash, &obj);
		}
		element = spl_object_storage_attach(intern, Z_OBJ_P(entry), Z_ISUNDEF(inf)?NULL:&inf);
		var_replace(&var_hash, &inf, &element->inf);
		zval_ptr_dtor(&inf);
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
	zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0, "Error at offset %zd of %zd bytes", ((char*)p - buf), buf_len);
	RETURN_THROWS();

} /* }}} */

/* {{{ */
PHP_METHOD(SplObjectStorage, __serialize)
{
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(ZEND_THIS);
	spl_SplObjectStorageElement *elem;
	zval tmp;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	array_init(return_value);

	/* storage */
	array_init_size(&tmp, 2 * zend_hash_num_elements(&intern->storage));
	ZEND_HASH_FOREACH_PTR(&intern->storage, elem) {
		zval obj;
		ZVAL_OBJ_COPY(&obj, elem->obj);
		zend_hash_next_index_insert(Z_ARRVAL(tmp), &obj);
		Z_TRY_ADDREF(elem->inf);
		zend_hash_next_index_insert(Z_ARRVAL(tmp), &elem->inf);
	} ZEND_HASH_FOREACH_END();
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &tmp);

	/* members */
	ZVAL_ARR(&tmp, zend_proptable_to_symtable(
		zend_std_get_properties(&intern->std), /* always_duplicate */ 1));
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &tmp);
} /* }}} */

/* {{{ */
PHP_METHOD(SplObjectStorage, __unserialize)
{
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(ZEND_THIS);
	HashTable *data;
	zval *storage_zv, *members_zv, *key, *val;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "h", &data) == FAILURE) {
		RETURN_THROWS();
	}

	storage_zv = zend_hash_index_find(data, 0);
	members_zv = zend_hash_index_find(data, 1);
	if (!storage_zv || !members_zv ||
			Z_TYPE_P(storage_zv) != IS_ARRAY || Z_TYPE_P(members_zv) != IS_ARRAY) {
		zend_throw_exception(spl_ce_UnexpectedValueException,
			"Incomplete or ill-typed serialization data", 0);
		RETURN_THROWS();
	}

	if (zend_hash_num_elements(Z_ARRVAL_P(storage_zv)) % 2 != 0) {
		zend_throw_exception(spl_ce_UnexpectedValueException, "Odd number of elements", 0);
		RETURN_THROWS();
	}

	key = NULL;
	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(storage_zv), val) {
		if (key) {
			if (Z_TYPE_P(key) != IS_OBJECT) {
				zend_throw_exception(spl_ce_UnexpectedValueException, "Non-object key", 0);
				RETURN_THROWS();
			}

			ZVAL_DEREF(val);
			spl_object_storage_attach(intern, Z_OBJ_P(key), val);
			key = NULL;
		} else {
			key = val;
		}
	} ZEND_HASH_FOREACH_END();

	object_properties_load(&intern->std, Z_ARRVAL_P(members_zv));
}

/* {{{ */
PHP_METHOD(SplObjectStorage, __debugInfo)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_ARR(spl_object_storage_debug_info(Z_OBJ_P(ZEND_THIS)));
}
/* }}} */

#define SPL_MULTIPLE_ITERATOR_GET_ALL_CURRENT   1
#define SPL_MULTIPLE_ITERATOR_GET_ALL_KEY       2

/* {{{ Iterator that iterates over several iterators one after the other */
PHP_METHOD(MultipleIterator, __construct)
{
	spl_SplObjectStorage   *intern;
	zend_long               flags = MIT_NEED_ALL|MIT_KEYS_NUMERIC;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &flags) == FAILURE) {
		RETURN_THROWS();
	}

	intern = Z_SPLOBJSTORAGE_P(ZEND_THIS);
	intern->flags = flags;
}
/* }}} */

/* {{{ Return current flags */
PHP_METHOD(MultipleIterator, getFlags)
{
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	RETURN_LONG(intern->flags);
}
/* }}} */

/* {{{ Set flags */
PHP_METHOD(MultipleIterator, setFlags)
{
	spl_SplObjectStorage *intern;
	intern = Z_SPLOBJSTORAGE_P(ZEND_THIS);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &intern->flags) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ Attach a new iterator */
PHP_METHOD(MultipleIterator, attachIterator)
{
	spl_SplObjectStorage *intern;
	zend_object *iterator = NULL;
	zval zinfo;
	zend_string *info_str;
	zend_long info_long;
	bool info_is_null = 1;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_OBJ_OF_CLASS(iterator, zend_ce_iterator)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_LONG_OR_NULL(info_str, info_long, info_is_null)
	ZEND_PARSE_PARAMETERS_END();

	intern = Z_SPLOBJSTORAGE_P(ZEND_THIS);

	if (!info_is_null) {
		spl_SplObjectStorageElement *element;

		if (info_str) {
			ZVAL_STR(&zinfo, info_str);
		} else {
			ZVAL_LONG(&zinfo, info_long);
		}

		zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
		while ((element = zend_hash_get_current_data_ptr_ex(&intern->storage, &intern->pos)) != NULL) {
			if (fast_is_identical_function(&zinfo, &element->inf)) {
				zend_throw_exception(spl_ce_InvalidArgumentException, "Key duplication error", 0);
				RETURN_THROWS();
			}
			zend_hash_move_forward_ex(&intern->storage, &intern->pos);
		}

		spl_object_storage_attach(intern, iterator, &zinfo);
	} else {
		spl_object_storage_attach(intern, iterator, NULL);
	}
}
/* }}} */

/* {{{ Detaches an iterator */
PHP_METHOD(MultipleIterator, detachIterator)
{
	zval *iterator;
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(ZEND_THIS);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &iterator, zend_ce_iterator) == FAILURE) {
		RETURN_THROWS();
	}
	spl_object_storage_detach(intern, Z_OBJ_P(iterator));

	zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
	intern->index = 0;
} /* }}} */

/* {{{ Determine whether the iterator exists */
PHP_METHOD(MultipleIterator, containsIterator)
{
	zval *iterator;
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(ZEND_THIS);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &iterator, zend_ce_iterator) == FAILURE) {
		RETURN_THROWS();
	}
	RETURN_BOOL(spl_object_storage_contains(intern, Z_OBJ_P(iterator)));
} /* }}} */

PHP_METHOD(MultipleIterator, countIterators)
{
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_LONG(zend_hash_num_elements(&intern->storage));
}

/* {{{ Rewind all attached iterator instances */
PHP_METHOD(MultipleIterator, rewind)
{
	spl_SplObjectStorage        *intern;
	spl_SplObjectStorageElement *element;

	intern = Z_SPLOBJSTORAGE_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
	while ((element = zend_hash_get_current_data_ptr_ex(&intern->storage, &intern->pos)) != NULL && !EG(exception)) {
		zend_object *it = element->obj;
		zend_call_known_instance_method_with_0_params(it->ce->iterator_funcs_ptr->zf_rewind, it, NULL);
		zend_hash_move_forward_ex(&intern->storage, &intern->pos);
	}
}
/* }}} */

/* {{{ Move all attached iterator instances forward */
PHP_METHOD(MultipleIterator, next)
{
	spl_SplObjectStorage        *intern;
	spl_SplObjectStorageElement *element;

	intern = Z_SPLOBJSTORAGE_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
	while ((element = zend_hash_get_current_data_ptr_ex(&intern->storage, &intern->pos)) != NULL && !EG(exception)) {
		zend_object *it = element->obj;
		zend_call_known_instance_method_with_0_params(it->ce->iterator_funcs_ptr->zf_next, it, NULL);
		zend_hash_move_forward_ex(&intern->storage, &intern->pos);
	}
}
/* }}} */

/* {{{ Return whether all or one sub iterator is valid depending on flags */
PHP_METHOD(MultipleIterator, valid)
{
	spl_SplObjectStorage        *intern;
	spl_SplObjectStorageElement *element;
	zval                         retval;
	zend_long                         expect, valid;

	intern = Z_SPLOBJSTORAGE_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (!zend_hash_num_elements(&intern->storage)) {
		RETURN_FALSE;
	}

	expect = (intern->flags & MIT_NEED_ALL) ? 1 : 0;

	zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
	while ((element = zend_hash_get_current_data_ptr_ex(&intern->storage, &intern->pos)) != NULL && !EG(exception)) {
		zend_object *it = element->obj;
		zend_call_known_instance_method_with_0_params(it->ce->iterator_funcs_ptr->zf_valid, it, &retval);

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
	zval                         retval;
	int                          valid = 1, num_elements;

	num_elements = zend_hash_num_elements(&intern->storage);
	if (num_elements < 1) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Called %s() on an invalid iterator",
			get_type == SPL_MULTIPLE_ITERATOR_GET_ALL_CURRENT ? "current" : "key");
		RETURN_THROWS();
	}

	array_init_size(return_value, num_elements);

	zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
	while ((element = zend_hash_get_current_data_ptr_ex(&intern->storage, &intern->pos)) != NULL && !EG(exception)) {
		zend_object *it = element->obj;
		zend_call_known_instance_method_with_0_params(it->ce->iterator_funcs_ptr->zf_valid, it, &retval);

		if (!Z_ISUNDEF(retval)) {
			valid = Z_TYPE(retval) == IS_TRUE;
			zval_ptr_dtor(&retval);
		} else {
			valid = 0;
		}

		if (valid) {
			if (SPL_MULTIPLE_ITERATOR_GET_ALL_CURRENT == get_type) {
				zend_call_known_instance_method_with_0_params(it->ce->iterator_funcs_ptr->zf_current, it, &retval);
			} else {
				zend_call_known_instance_method_with_0_params(it->ce->iterator_funcs_ptr->zf_key, it, &retval);
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

/* {{{ Return an array of all registered Iterator instances current() result */
PHP_METHOD(MultipleIterator, current)
{
	spl_SplObjectStorage        *intern;
	intern = Z_SPLOBJSTORAGE_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	spl_multiple_iterator_get_all(intern, SPL_MULTIPLE_ITERATOR_GET_ALL_CURRENT, return_value);
}
/* }}} */

/* {{{ Return an array of all registered Iterator instances key() result */
PHP_METHOD(MultipleIterator, key)
{
	spl_SplObjectStorage *intern;
	intern = Z_SPLOBJSTORAGE_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	spl_multiple_iterator_get_all(intern, SPL_MULTIPLE_ITERATOR_GET_ALL_KEY, return_value);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION(spl_observer) */
PHP_MINIT_FUNCTION(spl_observer)
{
	spl_ce_SplObserver = register_class_SplObserver();
	spl_ce_SplSubject = register_class_SplSubject();

	spl_ce_SplObjectStorage = register_class_SplObjectStorage(zend_ce_countable, zend_ce_iterator, zend_ce_serializable, zend_ce_arrayaccess);
	spl_ce_SplObjectStorage->create_object = spl_SplObjectStorage_new;
	spl_ce_SplObjectStorage->default_object_handlers = &spl_handler_SplObjectStorage;

	memcpy(&spl_handler_SplObjectStorage, &std_object_handlers, sizeof(zend_object_handlers));

	spl_handler_SplObjectStorage.offset          = XtOffsetOf(spl_SplObjectStorage, std);
	spl_handler_SplObjectStorage.compare         = spl_object_storage_compare_objects;
	spl_handler_SplObjectStorage.clone_obj       = spl_object_storage_clone;
	spl_handler_SplObjectStorage.get_gc          = spl_object_storage_get_gc;
	spl_handler_SplObjectStorage.free_obj        = spl_SplObjectStorage_free_storage;
	spl_handler_SplObjectStorage.read_dimension  = spl_object_storage_read_dimension;
	spl_handler_SplObjectStorage.write_dimension = spl_object_storage_write_dimension;
	spl_handler_SplObjectStorage.has_dimension   = spl_object_storage_has_dimension;
	spl_handler_SplObjectStorage.unset_dimension = spl_object_storage_unset_dimension;

	spl_ce_MultipleIterator = register_class_MultipleIterator(zend_ce_iterator);
	spl_ce_MultipleIterator->create_object = spl_SplObjectStorage_new;
	spl_ce_MultipleIterator->default_object_handlers = &spl_handler_SplObjectStorage;

	return SUCCESS;
}
/* }}} */
