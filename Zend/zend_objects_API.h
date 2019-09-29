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
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_OBJECTS_API_H
#define ZEND_OBJECTS_API_H

#include "zend.h"
#include "zend_compile.h"

#define OBJ_BUCKET_INVALID			(1<<0)

#define IS_OBJ_VALID(o)				(!(((zend_uintptr_t)(o)) & OBJ_BUCKET_INVALID))

#define SET_OBJ_INVALID(o)			((zend_object*)((((zend_uintptr_t)(o)) | OBJ_BUCKET_INVALID)))

#define GET_OBJ_BUCKET_NUMBER(o)	(((zend_intptr_t)(o)) >> 1)

#define SET_OBJ_BUCKET_NUMBER(o, n)	do { \
		(o) = (zend_object*)((((zend_uintptr_t)(n)) << 1) | OBJ_BUCKET_INVALID); \
	} while (0)

#define ZEND_OBJECTS_STORE_ADD_TO_FREE_LIST(h) do { \
		SET_OBJ_BUCKET_NUMBER(EG(objects_store).object_buckets[(h)], EG(objects_store).free_list_head); \
		EG(objects_store).free_list_head = (h); \
	} while (0)

#define OBJ_RELEASE(obj) zend_object_release(obj)

typedef struct _zend_objects_store {
	zend_object **object_buckets;
	uint32_t top;
	uint32_t size;
	int free_list_head;
} zend_objects_store;

/* Global store handling functions */
BEGIN_EXTERN_C()
ZEND_API void ZEND_FASTCALL zend_objects_store_init(zend_objects_store *objects, uint32_t init_size);
ZEND_API void ZEND_FASTCALL zend_objects_store_call_destructors(zend_objects_store *objects);
ZEND_API void ZEND_FASTCALL zend_objects_store_mark_destructed(zend_objects_store *objects);
ZEND_API void ZEND_FASTCALL zend_objects_store_free_object_storage(zend_objects_store *objects, zend_bool fast_shutdown);
ZEND_API void ZEND_FASTCALL zend_objects_store_destroy(zend_objects_store *objects);

/* Store API functions */
ZEND_API void ZEND_FASTCALL zend_objects_store_put(zend_object *object);
ZEND_API void ZEND_FASTCALL zend_objects_store_del(zend_object *object);

/* Called when the ctor was terminated by an exception */
static zend_always_inline void zend_object_store_ctor_failed(zend_object *obj)
{
	GC_ADD_FLAGS(obj, IS_OBJ_DESTRUCTOR_CALLED);
}

END_EXTERN_C()

static zend_always_inline void zend_object_release(zend_object *obj)
{
	if (GC_DELREF(obj) == 0) {
		zend_objects_store_del(obj);
	} else if (UNEXPECTED(GC_MAY_LEAK((zend_refcounted*)obj))) {
		gc_possible_root((zend_refcounted*)obj);
	}
}

static zend_always_inline size_t zend_object_properties_size(zend_class_entry *ce)
{
	return sizeof(zval) *
		(ce->default_properties_count -
			((ce->ce_flags & ZEND_ACC_USE_GUARDS) ? 0 : 1));
}

/* Allocates object type and zeros it, but not the properties.
 * Properties MUST be initialized using object_properties_init(). */
static zend_always_inline void *zend_object_alloc(size_t obj_size, zend_class_entry *ce) {
	void *obj = emalloc(obj_size + zend_object_properties_size(ce));
	/* Subtraction of sizeof(zval) is necessary, because zend_object_properties_size() may be
	 * -sizeof(zval), if the object has no properties. */
	memset(obj, 0, obj_size - sizeof(zval));
	return obj;
}

static inline zend_property_info *zend_get_property_info_for_slot(zend_object *obj, zval *slot)
{
	zend_property_info **table = obj->ce->properties_info_table;
	intptr_t prop_num = slot - obj->properties_table;
	ZEND_ASSERT(prop_num >= 0 && prop_num < obj->ce->default_properties_count);
	return table[prop_num];
}

/* Helper for cases where we're only interested in property info of typed properties. */
static inline zend_property_info *zend_get_typed_property_info_for_slot(zend_object *obj, zval *slot)
{
	zend_property_info *prop_info = zend_get_property_info_for_slot(obj, slot);
	if (prop_info && ZEND_TYPE_IS_SET(prop_info->type)) {
		return prop_info;
	}
	return NULL;
}


#endif /* ZEND_OBJECTS_H */
