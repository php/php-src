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

#ifndef ZEND_LAZY_OBJECT_H
#define ZEND_LAZY_OBJECT_H

#include "Zend/zend_types.h"

/* Lazy object is a lazy proxy object */
#define ZEND_LAZY_OBJECT_STRATEGY_PROXY                     (1<<0)

/* Lazy object is a lazy ghost object */
#define ZEND_LAZY_OBJECT_STRATEGY_GHOST                     (1<<1)

/* Lazy object is initialized (info.u is an instance) */
#define ZEND_LAZY_OBJECT_INITIALIZED                        (1<<2)

/* Serialization skips initialization */
#define ZEND_LAZY_OBJECT_SKIP_INITIALIZATION_ON_SERIALIZE   (1<<3)

/* Do not call destructor when making existing object lazy */
#define ZEND_LAZY_OBJECT_SKIP_DESTRUCTOR                    (1<<4)

#define ZEND_LAZY_OBJECT_USER_MASK (                                        \
	ZEND_LAZY_OBJECT_SKIP_INITIALIZATION_ON_SERIALIZE |                     \
	ZEND_LAZY_OBJECT_SKIP_DESTRUCTOR                                        \
)

#define ZEND_LAZY_OBJECT_STRATEGY_MASK (                                    \
		ZEND_LAZY_OBJECT_STRATEGY_PROXY |                                   \
		ZEND_LAZY_OBJECT_STRATEGY_GHOST                                     \
)

typedef uint8_t zend_lazy_object_flags_t;

typedef struct _zend_lazy_objects_store {
	/* object handle -> *zend_lazy_object_info */
	HashTable infos;
} zend_lazy_objects_store;

typedef struct _zend_property_info zend_property_info;
typedef struct _zend_fcall_info zend_fcall_info;
typedef struct _zend_fcall_info_cache zend_fcall_info_cache;

ZEND_API bool zend_class_can_be_lazy(zend_class_entry *ce);
ZEND_API zend_object *zend_object_make_lazy(zend_object *obj,
		zend_class_entry *class_type, zval *initializer_zv,
		zend_fcall_info_cache *initializer_fcc, zend_lazy_object_flags_t flags);
ZEND_API zend_object *zend_lazy_object_init(zend_object *obj);
ZEND_API zend_object *zend_lazy_object_mark_as_initialized(zend_object *obj);

void zend_lazy_objects_init(zend_lazy_objects_store *store);
void zend_lazy_objects_destroy(zend_lazy_objects_store *store);
zval* zend_lazy_object_get_initializer_zv(zend_object *obj);
zend_object *zend_lazy_object_get_instance(zend_object *obj);
zend_lazy_object_flags_t zend_lazy_object_get_flags(zend_object *obj);
void zend_lazy_object_del_info(zend_object *obj);
ZEND_API HashTable *zend_lazy_object_get_properties(zend_object *object);
zend_object *zend_lazy_object_clone(zend_object *old_obj);
HashTable *zend_lazy_object_debug_info(zend_object *object, int *is_temp);
HashTable *zend_lazy_object_get_gc(zend_object *zobj, zval **table, int *n);
bool zend_lazy_object_decr_lazy_props(zend_object *obj);
void zend_lazy_object_realize(zend_object *obj);
ZEND_API zend_property_info *zend_lazy_object_get_property_info_for_slot(zend_object *obj, zval *slot);

static zend_always_inline bool zend_object_is_lazy(zend_object *obj)
{
	return (OBJ_EXTRA_FLAGS(obj) & (IS_OBJ_LAZY_UNINITIALIZED | IS_OBJ_LAZY_PROXY));
}

static zend_always_inline bool zend_object_is_lazy_proxy(zend_object *obj)
{
	return (OBJ_EXTRA_FLAGS(obj) & IS_OBJ_LAZY_PROXY);
}

static zend_always_inline bool zend_lazy_object_initialized(zend_object *obj)
{
	return !(OBJ_EXTRA_FLAGS(obj) & IS_OBJ_LAZY_UNINITIALIZED);
}

/* True if accessing a lazy prop on obj mandates a call to
 * zend_lazy_object_init() */
static zend_always_inline bool zend_lazy_object_must_init(zend_object *obj)
{
	return zend_object_is_lazy(obj);
}

static inline bool zend_lazy_object_initialize_on_serialize(zend_object *obj)
{
	return !(zend_lazy_object_get_flags(obj) & ZEND_LAZY_OBJECT_SKIP_INITIALIZATION_ON_SERIALIZE);
}

#endif /* ZEND_LAZY_OBJECT_H */
