/* 
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2003 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
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

typedef void (*zend_objects_store_dtor_t)(void *object, zend_object_handle handle TSRMLS_DC);
typedef void (*zend_objects_store_clone_t)(void *object, void **object_clone TSRMLS_DC);

typedef struct _zend_object_store_bucket {
	zend_bool destructor_called;
	zend_bool valid;
	union _store_bucket {
		struct _store_object {
			void *object;
			zend_objects_store_dtor_t dtor;
			zend_objects_store_clone_t clone;
			zend_uint refcount;
		} obj;
		struct {
			int next;
		} free_list;
	} bucket;
} zend_object_store_bucket;

typedef struct _zend_objects_store {
	zend_object_store_bucket *object_buckets;
	zend_uint top;
	zend_uint size;
	int free_list_head;
} zend_objects_store;

/* Global store handling functions */
ZEND_API void zend_objects_store_init(zend_objects_store *objects, zend_uint init_size);
ZEND_API void zend_objects_store_call_destructors(zend_objects_store *objects TSRMLS_DC);
ZEND_API void zend_objects_store_destroy(zend_objects_store *objects);

/* Store API functions */
ZEND_API zend_object_handle zend_objects_store_put(void *object, zend_objects_store_dtor_t dtor, zend_objects_store_clone_t clone TSRMLS_DC);

ZEND_API void zend_objects_store_add_ref(zval *object TSRMLS_DC);
ZEND_API void zend_objects_store_del_ref(zval *object TSRMLS_DC);
ZEND_API void zend_objects_store_delete_obj(zval *object TSRMLS_DC);
ZEND_API zend_object_value zend_objects_store_clone_obj(zval *object TSRMLS_DC);
ZEND_API void *zend_object_store_get_object(zval *object TSRMLS_DC);
	
#define ZEND_OBJECTS_STORE_HANDLERS zend_objects_store_add_ref, zend_objects_store_del_ref, zend_objects_store_delete_obj, zend_objects_store_clone_obj

ZEND_API zval **zend_object_create_proxy(zval *object, zval *member TSRMLS_DC);

ZEND_API zend_object_handlers *zend_get_std_object_handlers();
#endif /* ZEND_OBJECTS_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
