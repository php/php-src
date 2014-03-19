/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2014 Zend Technologies Ltd. (http://www.zend.com) |
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

//???typedef void (*zend_objects_store_dtor_t)(zend_object *object TSRMLS_DC);
//???typedef void (*zend_objects_free_object_storage_t)(void *object TSRMLS_DC);
//???typedef void (*zend_objects_store_clone_t)(zend_object *object, zend_object **object_clone TSRMLS_DC);

//???typedef union _zend_object_store_bucket {
//???	zend_object *object;
//???	int next_free;
//???	zend_bool destructor_called;
//???	zend_bool valid;
//???	zend_uchar apply_count;
//???	union _store_bucket {
//???		struct _store_object {
//???			zend_object *object;
//???			zend_objects_store_dtor_t dtor;
//???			zend_objects_free_object_storage_t free_storage;
//???			zend_objects_store_clone_t clone;
//???			const zend_object_handlers *handlers;
//???			zend_uint refcount;
//???			gc_root_buffer *buffered;
//???		} obj;
//???		zend_object *obj;
//???		struct {
//???			int next;
//???		} free_list;
//???	} bucket;
//???} zend_object_store_bucket;

#define FREE_BUCKET				1

#define IS_VALID(o)				(!(((zend_uintptr_t)(o)) & FREE_BUCKET))

#define SET_INVALID(o)			((zend_object*)((((zend_uintptr_t)(o)) | FREE_BUCKET)))

#define GET_BUCKET_NUMBER(o)	(((zend_intptr_t)(o)) >> 1)

#define SET_BUCKET_NUMBER(o, n)	do { \
		(o) = (zend_object*)((((zend_uintptr_t)(n)) << 1) | FREE_BUCKET); \
	} while (0)


#define OBJ_RELEASE(obj) do { \
		zend_object *_obj = (obj); \
		if (--_obj->gc.refcount == 0) { \
			zend_objects_store_del(_obj TSRMLS_CC); \
		} else { \
			gc_possible_root(_obj TSRMLS_CC); \
		} \
	} while (0)

typedef struct _zend_objects_store {
	zend_object **object_buckets;
	zend_uint top;
	zend_uint size;
	int free_list_head;
} zend_objects_store;

/* Global store handling functions */
BEGIN_EXTERN_C()
ZEND_API void zend_objects_store_init(zend_objects_store *objects, zend_uint init_size);
ZEND_API void zend_objects_store_call_destructors(zend_objects_store *objects TSRMLS_DC);
ZEND_API void zend_objects_store_mark_destructed(zend_objects_store *objects TSRMLS_DC);
ZEND_API void zend_objects_store_destroy(zend_objects_store *objects);

/* Store API functions */
ZEND_API void zend_objects_store_put(zend_object *object TSRMLS_DC);
ZEND_API void zend_objects_store_del(zend_object *object TSRMLS_DC);
ZEND_API void zend_objects_store_free(zend_object *object TSRMLS_DC);

/* See comment in zend_objects_API.c before you use this */
ZEND_API void zend_object_store_set_object(zval *zobject, zend_object *object TSRMLS_DC);
ZEND_API void zend_object_store_ctor_failed(zval *zobject TSRMLS_DC);

ZEND_API void zend_objects_store_free_object_storage(zend_objects_store *objects TSRMLS_DC);

#define ZEND_OBJECTS_STORE_HANDLERS zend_object_free, zend_object_std_dtor, zend_objects_clone_obj

ZEND_API zend_object *zend_object_create_proxy(zval *object, zval *member TSRMLS_DC);

ZEND_API zend_object_handlers *zend_get_std_object_handlers(void);
END_EXTERN_C()

#endif /* ZEND_OBJECTS_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
