/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2015 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

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


#define OBJ_RELEASE(obj) zend_object_release(obj)

typedef struct _zend_objects_store {
	zend_object **object_buckets;
	uint32_t top;
	uint32_t size;
	int free_list_head;
} zend_objects_store;

/* Global store handling functions */
BEGIN_EXTERN_C()
ZEND_API void zend_objects_store_init(zend_objects_store *objects, uint32_t init_size);
ZEND_API void zend_objects_store_call_destructors(zend_objects_store *objects);
ZEND_API void zend_objects_store_mark_destructed(zend_objects_store *objects);
ZEND_API void zend_objects_store_destroy(zend_objects_store *objects);

/* Store API functions */
ZEND_API void zend_objects_store_put(zend_object *object);
ZEND_API void zend_objects_store_del(zend_object *object);
ZEND_API void zend_objects_store_free(zend_object *object);

/* See comment in zend_objects_API.c before you use this */
ZEND_API void zend_object_store_set_object(zval *zobject, zend_object *object);
ZEND_API void zend_object_store_ctor_failed(zend_object *object);

ZEND_API void zend_objects_store_free_object_storage(zend_objects_store *objects);

#define ZEND_OBJECTS_STORE_HANDLERS 0, zend_object_std_dtor, zend_objects_destroy_object, zend_objects_clone_obj

ZEND_API zend_object *zend_object_create_proxy(zval *object, zval *member);

ZEND_API zend_object_handlers *zend_get_std_object_handlers(void);
END_EXTERN_C()

static zend_always_inline void zend_object_release(zend_object *obj)
{
	if (--GC_REFCOUNT(obj) == 0) {
		zend_objects_store_del(obj);
	} else if (UNEXPECTED(!GC_INFO(obj))) {
		gc_possible_root((zend_refcounted*)obj);
	}
}

static zend_always_inline size_t zend_object_properties_size(zend_class_entry *ce)
{
	return sizeof(zval) *
		(ce->default_properties_count -
			((ce->ce_flags & ZEND_ACC_USE_GUARDS) ? 0 : 1));
}

#endif /* ZEND_OBJECTS_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
