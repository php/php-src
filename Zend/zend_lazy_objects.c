/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Arnaud Le Blanc <arnaud.lb@gmail.com>                       |
   +----------------------------------------------------------------------+
*/

/* Lazy objects are standard zend_object whose initialization is deferred until
 * one of their properties backing store is accessed for the first time.
 *
 * This is implemented by using the same fallback mechanism as __get and __set
 * magic methods that is triggered when an undefined property is accessed.
 *
 * Execution of methods or virtual property hooks do not trigger initialization
 * until they access properties.
 *
 * A lazy object can be created via the Reflection API. The user specifies an
 * initializer function that is called when initialization is required.
 *
 * There are two kinds of lazy objects:
 *
 * - Ghosts: These are initialized in-place by the initializer function
 * - Proxy: The initializer returns a new instance. After initialization,
 *   interaction with the proxy object are proxied to the instance.
 *
 * Internal objects are not supported.
 */

#include "zend_API.h"
#include "zend_compile.h"
#include "zend_execute.h"
#include "zend_gc.h"
#include "zend_hash.h"
#include "zend_object_handlers.h"
#include "zend_objects_API.h"
#include "zend_operators.h"
#include "zend_types.h"
#include "zend_variables.h"
#include "zend_lazy_objects.h"

/**
 * Information about each lazy object is stored outside of zend_objects, in
 * EG(lazy_objects_store). For ghost objects, we can release this after the
 * object is initialized.
 */
typedef struct _zend_lazy_object_info {
	union {
		struct {
			zend_fcall_info_cache fcc;
			zval zv; /* ReflectionClass::getLazyInitializer() */
		} initializer;
		zend_object *instance; /* For initialized lazy proxy objects */
	} u;
	zend_lazy_object_flags_t flags;
	int lazy_properties_count;
} zend_lazy_object_info;

/* zend_hash dtor_func_t for zend_lazy_objects_store.infos */
static void zend_lazy_object_info_dtor_func(zval *pElement)
{
	zend_lazy_object_info *info = (zend_lazy_object_info*) Z_PTR_P(pElement);

	if (info->flags & ZEND_LAZY_OBJECT_INITIALIZED) {
		ZEND_ASSERT(info->flags & ZEND_LAZY_OBJECT_STRATEGY_PROXY);
		zend_object_release(info->u.instance);
	} else {
		zval_ptr_dtor(&info->u.initializer.zv);
		zend_fcc_dtor(&info->u.initializer.fcc);
	}

	efree(info);
}

void zend_lazy_objects_init(zend_lazy_objects_store *store)
{
	zend_hash_init(&store->infos, 8, NULL, zend_lazy_object_info_dtor_func, false);
}

void zend_lazy_objects_destroy(zend_lazy_objects_store *store)
{
	ZEND_ASSERT(zend_hash_num_elements(&store->infos) == 0 || CG(unclean_shutdown));
	zend_hash_destroy(&store->infos);
}

static void zend_lazy_object_set_info(zend_object *obj, zend_lazy_object_info *info)
{
	ZEND_ASSERT(zend_object_is_lazy(obj));

	zval *zv = zend_hash_index_add_new_ptr(&EG(lazy_objects_store).infos, obj->handle, info);
	ZEND_ASSERT(zv);
	(void)zv;
}

static zend_lazy_object_info* zend_lazy_object_get_info(zend_object *obj)
{
	ZEND_ASSERT(zend_object_is_lazy(obj));

	zend_lazy_object_info *info = zend_hash_index_find_ptr(&EG(lazy_objects_store).infos, obj->handle);
	ZEND_ASSERT(info);

	return info;
}

static bool zend_lazy_object_has_stale_info(zend_object *obj)
{
	return zend_hash_index_find_ptr(&EG(lazy_objects_store).infos, obj->handle);
}

zval* zend_lazy_object_get_initializer_zv(zend_object *obj)
{
	ZEND_ASSERT(!zend_lazy_object_initialized(obj));

	zend_lazy_object_info *info = zend_lazy_object_get_info(obj);

	ZEND_ASSERT(!(info->flags & ZEND_LAZY_OBJECT_INITIALIZED));

	return &info->u.initializer.zv;
}

static zend_fcall_info_cache* zend_lazy_object_get_initializer_fcc(zend_object *obj)
{
	ZEND_ASSERT(!zend_lazy_object_initialized(obj));

	zend_lazy_object_info *info = zend_lazy_object_get_info(obj);

	ZEND_ASSERT(!(info->flags & ZEND_LAZY_OBJECT_INITIALIZED));

	return &info->u.initializer.fcc;
}

zend_object* zend_lazy_object_get_instance(zend_object *obj)
{
	ZEND_ASSERT(zend_lazy_object_initialized(obj));

	if (zend_object_is_lazy_proxy(obj)) {
		zend_lazy_object_info *info = zend_lazy_object_get_info(obj);

		ZEND_ASSERT(info->flags & ZEND_LAZY_OBJECT_INITIALIZED);

		return info->u.instance;
	}

	return obj;
}

zend_lazy_object_flags_t zend_lazy_object_get_flags(zend_object *obj)
{
	return zend_lazy_object_get_info(obj)->flags;
}

void zend_lazy_object_del_info(zend_object *obj)
{
	zend_result res = zend_hash_index_del(&EG(lazy_objects_store).infos, obj->handle);
	ZEND_ASSERT(res == SUCCESS);
}

bool zend_lazy_object_decr_lazy_props(zend_object *obj)
{
	ZEND_ASSERT(zend_object_is_lazy(obj));
	ZEND_ASSERT(!zend_lazy_object_initialized(obj));

	zend_lazy_object_info *info = zend_lazy_object_get_info(obj);

	ZEND_ASSERT(info->lazy_properties_count > 0);

	info->lazy_properties_count--;

	return info->lazy_properties_count == 0;
}

/**
 * Making objects lazy
 */

ZEND_API bool zend_class_can_be_lazy(zend_class_entry *ce)
{
	/* Internal classes are not supported */
	if (UNEXPECTED(ce->type == ZEND_INTERNAL_CLASS && ce != zend_standard_class_def)) {
		return false;
	}

	for (zend_class_entry *parent = ce->parent; parent; parent = parent->parent) {
		if (UNEXPECTED(parent->type == ZEND_INTERNAL_CLASS && parent != zend_standard_class_def)) {
			return false;
		}
	}

	return true;
}

static int zlo_hash_remove_dyn_props_func(zval *pDest)
{
	if (Z_TYPE_P(pDest) == IS_INDIRECT) {
		return ZEND_HASH_APPLY_STOP;
	}

	return ZEND_HASH_APPLY_REMOVE;
}

static bool zlo_is_iterating(zend_object *object)
{
	if (object->properties && HT_ITERATORS_COUNT(object->properties)) {
		return true;
	}
	if (zend_object_is_lazy_proxy(object)
			&& zend_lazy_object_initialized(object)) {
		return zlo_is_iterating(zend_lazy_object_get_instance(object));
	}
	return false;
}

/* Make object 'obj' lazy. If 'obj' is NULL, create a lazy instance of
 * class 'reflection_ce' */
ZEND_API zend_object *zend_object_make_lazy(zend_object *obj,
		zend_class_entry *reflection_ce, zval *initializer_zv,
		zend_fcall_info_cache *initializer_fcc, zend_lazy_object_flags_t flags)
{
	ZEND_ASSERT(!(flags & ~(ZEND_LAZY_OBJECT_USER_MASK|ZEND_LAZY_OBJECT_STRATEGY_MASK)));
	ZEND_ASSERT((flags & ZEND_LAZY_OBJECT_STRATEGY_MASK) == ZEND_LAZY_OBJECT_STRATEGY_GHOST
			|| (flags & ZEND_LAZY_OBJECT_STRATEGY_MASK) == ZEND_LAZY_OBJECT_STRATEGY_PROXY);

	ZEND_ASSERT(!obj || (!zend_object_is_lazy(obj) || zend_lazy_object_initialized(obj)));
	ZEND_ASSERT(!obj || instanceof_function(obj->ce, reflection_ce));

	/* Internal classes are not supported */
	if (UNEXPECTED(reflection_ce->type == ZEND_INTERNAL_CLASS && reflection_ce != zend_standard_class_def)) {
		zend_throw_error(NULL, "Cannot make instance of internal class lazy: %s is internal", ZSTR_VAL(reflection_ce->name));
		return NULL;
	}

	for (zend_class_entry *parent = reflection_ce->parent; parent; parent = parent->parent) {
		if (UNEXPECTED(parent->type == ZEND_INTERNAL_CLASS && parent != zend_standard_class_def)) {
			zend_throw_error(NULL, "Cannot make instance of internal class lazy: %s inherits internal class %s",
				ZSTR_VAL(reflection_ce->name), ZSTR_VAL(parent->name));
			return NULL;
		}
	}

	int lazy_properties_count = 0;

	if (!obj) {
		if (UNEXPECTED(reflection_ce->ce_flags & ZEND_ACC_UNINSTANTIABLE)) {
			zval zobj;
			/* Call object_init_ex() for the generated exception */
			zend_result result = object_init_ex(&zobj, reflection_ce);
			ZEND_ASSERT(result == FAILURE && EG(exception));
			(void)result;
			return NULL;
		}

		if (UNEXPECTED(!(reflection_ce->ce_flags & ZEND_ACC_CONSTANTS_UPDATED))) {
			if (UNEXPECTED(zend_update_class_constants(reflection_ce) != SUCCESS)) {
				ZEND_ASSERT(EG(exception));
				return NULL;
			}
		}

		obj = zend_objects_new(reflection_ce);

		for (int i = 0; i < obj->ce->default_properties_count; i++) {
			zval *p = &obj->properties_table[i];
			ZVAL_UNDEF(p);
			if (EXPECTED(obj->ce->properties_info_table[i])) {
				Z_PROP_FLAG_P(p) = IS_PROP_UNINIT | IS_PROP_LAZY;
				lazy_properties_count++;
			} else {
				Z_PROP_FLAG_P(p) = 0;
			}
		}
	} else {
		if (zlo_is_iterating(obj)) {
			zend_throw_error(NULL, "Can not reset an object during property iteration");
			return NULL;
		}
		if (zend_object_is_lazy(obj)) {
			ZEND_ASSERT(zend_object_is_lazy_proxy(obj) && zend_lazy_object_initialized(obj));
			OBJ_EXTRA_FLAGS(obj) &= ~(IS_OBJ_LAZY_UNINITIALIZED|IS_OBJ_LAZY_PROXY);
			zend_lazy_object_del_info(obj);
		} else {
			if (zend_lazy_object_has_stale_info(obj)) {
				zend_throw_error(NULL, "Can not reset an object while it is being initialized");
				return NULL;
			}

			if (!(flags & ZEND_LAZY_OBJECT_SKIP_DESTRUCTOR)
				&& !(OBJ_FLAGS(obj) & IS_OBJ_DESTRUCTOR_CALLED)) {
				if (obj->handlers->dtor_obj != zend_objects_destroy_object
						|| obj->ce->destructor) {
					GC_ADD_FLAGS(obj, IS_OBJ_DESTRUCTOR_CALLED);
					GC_ADDREF(obj);
					obj->handlers->dtor_obj(obj);
					GC_DELREF(obj);
					if (EG(exception)) {
						return NULL;
					}
				}
			}
		}

		GC_DEL_FLAGS(obj, IS_OBJ_DESTRUCTOR_CALLED);

		/* unset() dynamic properties. Do not NULL out obj->properties, as this
		 * would be unexpected. */
		if (obj->properties) {
			if (UNEXPECTED(GC_REFCOUNT(obj->properties) > 1)) {
				if (EXPECTED(!(GC_FLAGS(obj->properties) & IS_ARRAY_IMMUTABLE))) {
					GC_DELREF(obj->properties);
				}
				obj->properties = zend_array_dup(obj->properties);
			}
			zend_hash_reverse_apply(obj->properties, zlo_hash_remove_dyn_props_func);
		}

		/* unset() declared properties */
		for (int i = 0; i < reflection_ce->default_properties_count; i++) {
			zend_property_info *prop_info = obj->ce->properties_info_table[i];
			if (EXPECTED(prop_info)) {
				zval *p = &obj->properties_table[i];
				if (Z_TYPE_P(p) != IS_UNDEF) {
					if ((prop_info->flags & ZEND_ACC_READONLY) && !(Z_PROP_FLAG_P(p) & IS_PROP_REINITABLE)
							/* TODO: test final property */
							&& ((obj->ce->ce_flags & ZEND_ACC_FINAL) || (prop_info->flags & ZEND_ACC_FINAL))) {
						continue;
					}
					zend_object_dtor_property(obj, p);
					ZVAL_UNDEF(p);
				}
				Z_PROP_FLAG_P(p) = IS_PROP_UNINIT | IS_PROP_LAZY;
				lazy_properties_count++;
			}
		}
	}

	/* Objects become non-lazy if all properties are made non-lazy before
	 * initialization is triggered. If the object has no properties to begin
	 * with, this happens immediately. */
	if (UNEXPECTED(!lazy_properties_count)) {
		return obj;
	}

	OBJ_EXTRA_FLAGS(obj) |= IS_OBJ_LAZY_UNINITIALIZED;

	if (flags & ZEND_LAZY_OBJECT_STRATEGY_PROXY) {
		OBJ_EXTRA_FLAGS(obj) |= IS_OBJ_LAZY_PROXY;
	} else {
		ZEND_ASSERT(flags & ZEND_LAZY_OBJECT_STRATEGY_GHOST);
	}

	zend_lazy_object_info *info = emalloc(sizeof(*info));
	zend_fcc_dup(&info->u.initializer.fcc, initializer_fcc);
	ZVAL_COPY(&info->u.initializer.zv, initializer_zv);
	info->flags = flags;
	info->lazy_properties_count = lazy_properties_count;
	zend_lazy_object_set_info(obj, info);

	return obj;
}

/**
 * Initialization of lazy objects
 */

/* Mark object as initialized. Lazy properties are initialized to their default
 * value and the initializer is not called. */
ZEND_API zend_object *zend_lazy_object_mark_as_initialized(zend_object *obj)
{
	ZEND_ASSERT(zend_object_is_lazy(obj));
	ZEND_ASSERT(!zend_lazy_object_initialized(obj));

	zend_class_entry *ce = obj->ce;

	ZEND_ASSERT(ce->ce_flags & ZEND_ACC_CONSTANTS_UPDATED);

	zval *default_properties_table = CE_DEFAULT_PROPERTIES_TABLE(ce);
	zval *properties_table = obj->properties_table;

	OBJ_EXTRA_FLAGS(obj) &= ~(IS_OBJ_LAZY_UNINITIALIZED|IS_OBJ_LAZY_PROXY);

	for (int i = 0; i < ce->default_properties_count; i++) {
		if (Z_PROP_FLAG_P(&properties_table[i]) & IS_PROP_LAZY) {
			ZVAL_COPY_PROP(&properties_table[i], &default_properties_table[i]);
		}
	}

	zend_lazy_object_del_info(obj);

	return obj;
}

/* Revert initializer effects */
static void zend_lazy_object_revert_init(zend_object *obj, zval *properties_table_snapshot, HashTable *properties_snapshot)
{
	zend_class_entry *ce = obj->ce;

	if (ce->default_properties_count) {
		ZEND_ASSERT(properties_table_snapshot);
		zval *properties_table = obj->properties_table;

		for (int i = 0; i < ce->default_properties_count; i++) {
			zval *p = &properties_table[i];
			zend_object_dtor_property(obj, p);
			ZVAL_COPY_VALUE_PROP(p, &properties_table_snapshot[i]);

			zend_property_info *prop_info = ce->properties_info_table[i];
			if (Z_ISREF_P(p) && prop_info && ZEND_TYPE_IS_SET(prop_info->type)) {
				ZEND_REF_ADD_TYPE_SOURCE(Z_REF_P(p), prop_info);
			}
		}

		efree(properties_table_snapshot);
	}
	if (properties_snapshot) {
		if (obj->properties != properties_snapshot) {
			ZEND_ASSERT((GC_FLAGS(properties_snapshot) & IS_ARRAY_IMMUTABLE) || GC_REFCOUNT(properties_snapshot) >= 1);
			zend_release_properties(obj->properties);
			obj->properties = properties_snapshot;
		} else {
			ZEND_ASSERT((GC_FLAGS(properties_snapshot) & IS_ARRAY_IMMUTABLE) || GC_REFCOUNT(properties_snapshot) > 1);
			zend_release_properties(properties_snapshot);
		}
	} else if (obj->properties) {
		zend_release_properties(obj->properties);
		obj->properties = NULL;
	}

	OBJ_EXTRA_FLAGS(obj) |= IS_OBJ_LAZY_UNINITIALIZED;
}

static bool zend_lazy_object_compatible(zend_object *real_object, zend_object *lazy_object)
{
	if (EXPECTED(real_object->ce == lazy_object->ce)) {
		return true;
	}

	if (!instanceof_function(lazy_object->ce, real_object->ce)) {
		return false;
	}

	/* zend_hash_num_elements(ce.properties_info) reports the actual number of
	 * properties. ce.default_properties_count is off by the number of property
	 * overrides. */
	if (zend_hash_num_elements(&lazy_object->ce->properties_info) != zend_hash_num_elements(&real_object->ce->properties_info)) {
		return false;
	}

	return lazy_object->ce->destructor == real_object->ce->destructor
		&& lazy_object->ce->clone == real_object->ce->clone;
}

/* Initialize a lazy proxy object */
static zend_object *zend_lazy_object_init_proxy(zend_object *obj)
{
	ZEND_ASSERT(zend_object_is_lazy_proxy(obj));
	ZEND_ASSERT(!zend_lazy_object_initialized(obj));

	/* Prevent object from being released during initialization */
	GC_ADDREF(obj);

	zend_lazy_object_info *info = zend_lazy_object_get_info(obj);

	/* prevent reentrant initialization */
	OBJ_EXTRA_FLAGS(obj) &= ~(IS_OBJ_LAZY_UNINITIALIZED|IS_OBJ_LAZY_PROXY);

	/* Call factory */
	zval retval;
	int argc = 1;
	zval zobj;
	HashTable *named_params = NULL;
	zend_fcall_info_cache *initializer = &info->u.initializer.fcc;
	zend_object *instance = NULL;

	ZVAL_OBJ(&zobj, obj);

	zend_call_known_fcc(initializer, &retval, argc, &zobj, named_params);

	if (UNEXPECTED(EG(exception))) {
		OBJ_EXTRA_FLAGS(obj) |= IS_OBJ_LAZY_UNINITIALIZED|IS_OBJ_LAZY_PROXY;
		goto exit;
	}

	if (UNEXPECTED(Z_TYPE(retval) != IS_OBJECT)) {
		OBJ_EXTRA_FLAGS(obj) |= IS_OBJ_LAZY_UNINITIALIZED|IS_OBJ_LAZY_PROXY;
		zend_type_error("Lazy proxy factory must return an instance of a class compatible with %s, %s returned",
				ZSTR_VAL(obj->ce->name),
				zend_zval_value_name(&retval));
		zval_ptr_dtor(&retval);
		goto exit;
	}

	if (UNEXPECTED(Z_TYPE(retval) != IS_OBJECT || !zend_lazy_object_compatible(Z_OBJ(retval), obj))) {
		OBJ_EXTRA_FLAGS(obj) |= IS_OBJ_LAZY_UNINITIALIZED|IS_OBJ_LAZY_PROXY;
		zend_type_error("The real instance class %s is not compatible with the proxy class %s. The proxy must be a instance of the same class as the real instance, or a sub-class with no additional properties, and no overrides of the __destructor or __clone methods.",
				zend_zval_value_name(&retval),
				ZSTR_VAL(obj->ce->name));
		zval_ptr_dtor(&retval);
		goto exit;
	}

	if (UNEXPECTED(Z_OBJ(retval) == obj || zend_object_is_lazy(Z_OBJ(retval)))) {
		OBJ_EXTRA_FLAGS(obj) |= IS_OBJ_LAZY_UNINITIALIZED|IS_OBJ_LAZY_PROXY;
		zend_throw_error(NULL, "Lazy proxy factory must return a non-lazy object");
		zval_ptr_dtor(&retval);
		goto exit;
	}

	zend_fcc_dtor(&info->u.initializer.fcc);
	zval_ptr_dtor(&info->u.initializer.zv);
	info->u.instance = Z_OBJ(retval);
	info->flags |= ZEND_LAZY_OBJECT_INITIALIZED;
	OBJ_EXTRA_FLAGS(obj) |= IS_OBJ_LAZY_PROXY;

	/* unset() properties of the proxy. This ensures that all accesses are be
	 * delegated to the backing instance from now on. */
	zend_object_dtor_dynamic_properties(obj);
	obj->properties = NULL;

	for (int i = 0; i < Z_OBJ(retval)->ce->default_properties_count; i++) {
		if (EXPECTED(Z_OBJ(retval)->ce->properties_info_table[i])) {
			zend_object_dtor_property(obj, &obj->properties_table[i]);
			ZVAL_UNDEF(&obj->properties_table[i]);
			Z_PROP_FLAG_P(&obj->properties_table[i]) = IS_PROP_UNINIT | IS_PROP_LAZY;
		}
	}

	instance = Z_OBJ(retval);

exit:
	if (UNEXPECTED(GC_DELREF(obj) == 0)) {
		zend_throw_error(NULL, "Lazy object was released during initialization");
		zend_objects_store_del(obj);
		instance = NULL;
	} else {
		gc_check_possible_root((zend_refcounted*) obj);
	}

	return instance;
}

/* Initialize a lazy object. */
ZEND_API zend_object *zend_lazy_object_init(zend_object *obj)
{
	ZEND_ASSERT(zend_object_is_lazy(obj));

	/* If obj is an initialized lazy proxy, return the real instance. This
	 * supports the following pattern:
	 * if (zend_lazy_object_must_init(obj)) {
	 *     instance = zend_lazy_object_init(obj);
	 * }
	 */
	if (zend_lazy_object_initialized(obj)) {
		ZEND_ASSERT(zend_object_is_lazy_proxy(obj));
		zend_lazy_object_info *info = zend_lazy_object_get_info(obj);
		ZEND_ASSERT(info->flags & ZEND_LAZY_OBJECT_INITIALIZED);
		if (zend_object_is_lazy(info->u.instance)) {
			return zend_lazy_object_init(info->u.instance);
		}
		return info->u.instance;
	}

	zend_class_entry *ce = obj->ce;

	ZEND_ASSERT(ce->ce_flags & ZEND_ACC_CONSTANTS_UPDATED);

	if (zend_object_is_lazy_proxy(obj)) {
		return zend_lazy_object_init_proxy(obj);
	}

	/* Prevent object from being released during initialization */
	GC_ADDREF(obj);

	zend_fcall_info_cache *initializer = zend_lazy_object_get_initializer_fcc(obj);

	/* Prevent reentrant initialization */
	OBJ_EXTRA_FLAGS(obj) &= ~IS_OBJ_LAZY_UNINITIALIZED;

	/* Snapshot dynamic properties */
	HashTable *properties_snapshot = obj->properties;
	if (properties_snapshot) {
		GC_TRY_ADDREF(properties_snapshot);
	}

	zval *properties_table_snapshot = NULL;

	/* Snapshot declared properties and initialize lazy properties to their
	 * default value */
	if (ce->default_properties_count) {
		zval *default_properties_table = CE_DEFAULT_PROPERTIES_TABLE(ce);
		zval *properties_table = obj->properties_table;
		properties_table_snapshot = emalloc(sizeof(*properties_table_snapshot) * ce->default_properties_count);

		for (int i = 0; i < ce->default_properties_count; i++) {
			ZVAL_COPY_PROP(&properties_table_snapshot[i], &properties_table[i]);
			if (Z_PROP_FLAG_P(&properties_table[i]) & IS_PROP_LAZY) {
				ZVAL_COPY_PROP(&properties_table[i], &default_properties_table[i]);
			}
		}
	}

	/* Call initializer */
	zval retval;
	int argc = 1;
	zval zobj;
	HashTable *named_params = NULL;
	zend_object *instance = NULL;

	ZVAL_OBJ(&zobj, obj);

	zend_call_known_fcc(initializer, &retval, argc, &zobj, named_params);

	if (EG(exception)) {
		zend_lazy_object_revert_init(obj, properties_table_snapshot, properties_snapshot);
		goto exit;
	}

	if (Z_TYPE(retval) != IS_NULL) {
		zend_lazy_object_revert_init(obj, properties_table_snapshot, properties_snapshot);
		zval_ptr_dtor(&retval);
		zend_type_error("Lazy object initializer must return NULL or no value");
		goto exit;
	}

	if (properties_table_snapshot) {
		for (int i = 0; i < obj->ce->default_properties_count; i++) {
			zval *p = &properties_table_snapshot[i];
			/* Use zval_ptr_dtor directly here (not zend_object_dtor_property),
			 * as any reference type_source will have already been deleted in
			 * case the prop is not bound to this value anymore. */
			i_zval_ptr_dtor(p);
		}
		efree(properties_table_snapshot);
	}

	if (properties_snapshot) {
		zend_release_properties(properties_snapshot);
	}

	/* Must be very last in this function, for the
	 * zend_lazy_object_has_stale_info() check */
	zend_lazy_object_del_info(obj);

	instance = obj;

exit:
	if (UNEXPECTED(GC_DELREF(obj) == 0)) {
		zend_throw_error(NULL, "Lazy object was released during initialization");
		zend_objects_store_del(obj);
		instance = NULL;
	} else {
		gc_check_possible_root((zend_refcounted*) obj);
	}

	return instance;
}

/* Mark an object as non-lazy (after all properties were initialized) */
void zend_lazy_object_realize(zend_object *obj)
{
	ZEND_ASSERT(zend_object_is_lazy(obj));
	ZEND_ASSERT(!zend_lazy_object_initialized(obj));

	zend_lazy_object_del_info(obj);

#if ZEND_DEBUG
	for (int i = 0; i < obj->ce->default_properties_count; i++) {
		ZEND_ASSERT(!(Z_PROP_FLAG_P(&obj->properties_table[i]) & IS_PROP_LAZY));
	}
#endif

	OBJ_EXTRA_FLAGS(obj) &= ~(IS_OBJ_LAZY_UNINITIALIZED | IS_OBJ_LAZY_PROXY);
}

ZEND_API HashTable *zend_lazy_object_get_properties(zend_object *object)
{
	ZEND_ASSERT(zend_object_is_lazy(object));

	zend_object *tmp = zend_lazy_object_init(object);
	if (UNEXPECTED(!tmp)) {
		if (object->properties) {
			return object->properties;
		}
		return object->properties = zend_new_array(0);
	}

	object = tmp;
	ZEND_ASSERT(!zend_lazy_object_must_init(object));

	return zend_std_get_properties_ex(object);
}

/* Initialize object and clone it. For proxies, we clone both the proxy and its
 * real instance, and we don't call __clone() on the proxy. */
zend_object *zend_lazy_object_clone(zend_object *old_obj)
{
	ZEND_ASSERT(zend_object_is_lazy(old_obj));

	if (UNEXPECTED(!zend_lazy_object_initialized(old_obj) && !zend_lazy_object_init(old_obj))) {
		ZEND_ASSERT(EG(exception));
		/* Clone handler must always return an object. It is discarded later due
		 * to the exception. */
		zval zv;
		object_init_ex(&zv, old_obj->ce);
		GC_ADD_FLAGS(Z_OBJ(zv), IS_OBJ_DESTRUCTOR_CALLED);
		return Z_OBJ(zv);
	}

	if (!zend_object_is_lazy_proxy(old_obj)) {
		return zend_objects_clone_obj(old_obj);
	}

	zend_lazy_object_info *info = zend_lazy_object_get_info(old_obj);
	zend_class_entry *ce = old_obj->ce;
	zend_object *new_proxy = zend_objects_new(ce);

	for (int i = 0; i < ce->default_properties_count; i++) {
		zval *p = &new_proxy->properties_table[i];
		ZVAL_UNDEF(p);
		if (EXPECTED(ce->properties_info_table[i])) {
			Z_PROP_FLAG_P(p) = IS_PROP_UNINIT | IS_PROP_LAZY;
		} else {
			Z_PROP_FLAG_P(p) = 0;
		}
	}

	OBJ_EXTRA_FLAGS(new_proxy) = OBJ_EXTRA_FLAGS(old_obj);

	zend_lazy_object_info *new_info = emalloc(sizeof(*info));
	*new_info = *info;
	new_info->u.instance = zend_objects_clone_obj(info->u.instance);

	zend_lazy_object_set_info(new_proxy, new_info);

	return new_proxy;
}

HashTable *zend_lazy_object_debug_info(zend_object *object, int *is_temp)
{
	ZEND_ASSERT(zend_object_is_lazy(object));

	if (zend_object_is_lazy_proxy(object)) {
		if (zend_lazy_object_initialized(object)) {
			HashTable *properties = zend_new_array(0);
			zval instance;
			ZVAL_OBJ(&instance, zend_lazy_object_get_instance(object));
			Z_ADDREF(instance);
			zend_hash_str_add(properties, "instance", strlen("instance"), &instance);
			*is_temp = 1;
			return properties;
		}
	}

	*is_temp = 0;
	return zend_get_properties_no_lazy_init(object);
}

HashTable *zend_lazy_object_get_gc(zend_object *zobj, zval **table, int *n)
{
	ZEND_ASSERT(zend_object_is_lazy(zobj));

	zend_lazy_object_info *info = zend_lazy_object_get_info(zobj);
	zend_get_gc_buffer *gc_buffer = zend_get_gc_buffer_create();

	if (zend_lazy_object_initialized(zobj)) {
		ZEND_ASSERT(zend_object_is_lazy_proxy(zobj));
		zend_get_gc_buffer_add_obj(gc_buffer, info->u.instance);
		zend_get_gc_buffer_use(gc_buffer, table, n);
		/* Initialized proxy object can not have properties */
		return NULL;
	}

	zend_fcall_info_cache *fcc = &info->u.initializer.fcc;
	if (fcc->object) {
		zend_get_gc_buffer_add_obj(gc_buffer, fcc->object);
	}
	if (fcc->closure) {
		zend_get_gc_buffer_add_obj(gc_buffer, fcc->closure);
	}
	zend_get_gc_buffer_add_zval(gc_buffer, &info->u.initializer.zv);

	/* Uninitialized lazy objects can not have dynamic properties, so we can
	 * ignore zobj->properties. */
	zval *prop = zobj->properties_table;
	zval *end = prop + zobj->ce->default_properties_count;
	for ( ; prop < end; prop++) {
		zend_get_gc_buffer_add_zval(gc_buffer, prop);
	}

	zend_get_gc_buffer_use(gc_buffer, table, n);
	return NULL;
}

zend_property_info *zend_lazy_object_get_property_info_for_slot(zend_object *obj, zval *slot)
{
	ZEND_ASSERT(zend_object_is_lazy_proxy(obj));

	zend_property_info **table = obj->ce->properties_info_table;
	intptr_t prop_num = slot - obj->properties_table;
	if (prop_num >= 0 && prop_num < obj->ce->default_properties_count) {
		return table[prop_num];
	}

	if (!zend_lazy_object_initialized(obj)) {
		return NULL;
	}

	obj = zend_lazy_object_get_instance(obj);
	return zend_get_property_info_for_slot(obj, slot);
}
