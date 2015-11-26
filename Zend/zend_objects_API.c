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
   |          Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend.h"
#include "zend_globals.h"
#include "zend_variables.h"
#include "zend_API.h"
#include "zend_objects_API.h"

ZEND_API void zend_objects_store_init(zend_objects_store *objects, uint32_t init_size)
{
	objects->object_buckets = (zend_object **) emalloc(init_size * sizeof(zend_object*));
	objects->top = 1; /* Skip 0 so that handles are true */
	objects->size = init_size;
	objects->free_list_head = -1;
	memset(&objects->object_buckets[0], 0, sizeof(zend_object*));
}

ZEND_API void zend_objects_store_destroy(zend_objects_store *objects)
{
	efree(objects->object_buckets);
	objects->object_buckets = NULL;
}

ZEND_API void zend_objects_store_call_destructors(zend_objects_store *objects)
{
	if (objects->top > 1) {
		zend_object **obj_ptr = objects->object_buckets + 1;
		zend_object **end = objects->object_buckets + objects->top;

		do {
			zend_object *obj = *obj_ptr;

			if (IS_OBJ_VALID(obj)) {
				if (!(GC_FLAGS(obj) & IS_OBJ_DESTRUCTOR_CALLED)) {
					GC_FLAGS(obj) |= IS_OBJ_DESTRUCTOR_CALLED;
					GC_REFCOUNT(obj)++;
					obj->handlers->dtor_obj(obj);
					GC_REFCOUNT(obj)--;
				}
			}
			obj_ptr++;
		} while (obj_ptr != end);
	}
}

ZEND_API void zend_objects_store_mark_destructed(zend_objects_store *objects)
{
	if (objects->object_buckets && objects->top > 1) {
		zend_object **obj_ptr = objects->object_buckets + 1;
		zend_object **end = objects->object_buckets + objects->top;

		do {
			zend_object *obj = *obj_ptr;

			if (IS_OBJ_VALID(obj)) {
				GC_FLAGS(obj) |= IS_OBJ_DESTRUCTOR_CALLED;
			}
			obj_ptr++;
		} while (obj_ptr != end);
	}
}

ZEND_API void zend_objects_store_free_object_storage(zend_objects_store *objects)
{
	zend_object **obj_ptr, **end, *obj;

	if (objects->top <= 1) {
		return;
	}

	/* Free object contents, but don't free objects themselves, so they show up as leaks */
	end = objects->object_buckets + 1;
	obj_ptr = objects->object_buckets + objects->top;

	do {
		obj_ptr--;
		obj = *obj_ptr;
		if (IS_OBJ_VALID(obj)) {
			if (!(GC_FLAGS(obj) & IS_OBJ_FREE_CALLED)) {
				GC_FLAGS(obj) |= IS_OBJ_FREE_CALLED;
				if (obj->handlers->free_obj) {
					GC_REFCOUNT(obj)++;
					obj->handlers->free_obj(obj);
					GC_REFCOUNT(obj)--;
				}
			}
		}
	} while (obj_ptr != end);
}


/* Store objects API */

ZEND_API void zend_objects_store_put(zend_object *object)
{
	int handle;

	if (EG(objects_store).free_list_head != -1) {
		handle = EG(objects_store).free_list_head;
		EG(objects_store).free_list_head = GET_OBJ_BUCKET_NUMBER(EG(objects_store).object_buckets[handle]);
	} else {
		if (EG(objects_store).top == EG(objects_store).size) {
			EG(objects_store).size <<= 1;
			EG(objects_store).object_buckets = (zend_object **) erealloc(EG(objects_store).object_buckets, EG(objects_store).size * sizeof(zend_object*));
		}
		handle = EG(objects_store).top++;
	}
	object->handle = handle;
	EG(objects_store).object_buckets[handle] = object;
}

#define ZEND_OBJECTS_STORE_ADD_TO_FREE_LIST(handle)															\
            SET_OBJ_BUCKET_NUMBER(EG(objects_store).object_buckets[handle], EG(objects_store).free_list_head);	\
			EG(objects_store).free_list_head = handle;

ZEND_API void zend_objects_store_free(zend_object *object) /* {{{ */
{
	uint32_t handle = object->handle;
	void *ptr = ((char*)object) - object->handlers->offset;

	GC_REMOVE_FROM_BUFFER(object);
	efree(ptr);
	ZEND_OBJECTS_STORE_ADD_TO_FREE_LIST(handle);
}
/* }}} */

ZEND_API void zend_objects_store_del(zend_object *object) /* {{{ */
{
	/*	Make sure we hold a reference count during the destructor call
		otherwise, when the destructor ends the storage might be freed
		when the refcount reaches 0 a second time
	 */
	if (EG(objects_store).object_buckets &&
	    IS_OBJ_VALID(EG(objects_store).object_buckets[object->handle])) {
		if (GC_REFCOUNT(object) == 0) {
			int failure = 0;

			if (!(GC_FLAGS(object) & IS_OBJ_DESTRUCTOR_CALLED)) {
				GC_FLAGS(object) |= IS_OBJ_DESTRUCTOR_CALLED;

				if (object->handlers->dtor_obj) {
					GC_REFCOUNT(object)++;
					zend_try {
						object->handlers->dtor_obj(object);
					} zend_catch {
						failure = 1;
					} zend_end_try();
					GC_REFCOUNT(object)--;
				}
			}

			if (GC_REFCOUNT(object) == 0) {
				uint32_t handle = object->handle;
				void *ptr;

				EG(objects_store).object_buckets[handle] = SET_OBJ_INVALID(object);
				if (!(GC_FLAGS(object) & IS_OBJ_FREE_CALLED)) {
					GC_FLAGS(object) |= IS_OBJ_FREE_CALLED;
					if (object->handlers->free_obj) {
						zend_try {
							GC_REFCOUNT(object)++;
							object->handlers->free_obj(object);
							GC_REFCOUNT(object)--;
						} zend_catch {
							failure = 1;
						} zend_end_try();
					}
				}
				ptr = ((char*)object) - object->handlers->offset;
				GC_REMOVE_FROM_BUFFER(object);
				efree(ptr);
				ZEND_OBJECTS_STORE_ADD_TO_FREE_LIST(handle);
			}

			if (failure) {
				zend_bailout();
			}
		} else {
			GC_REFCOUNT(object)--;
		}
	}
}
/* }}} */

/* zend_object_store_set_object:
 * It is ONLY valid to call this function from within the constructor of an
 * overloaded object.  Its purpose is to set the object pointer for the object
 * when you can't possibly know its value until you have parsed the arguments
 * from the constructor function.  You MUST NOT use this function for any other
 * weird games, or call it at any other time after the object is constructed.
 * */
ZEND_API void zend_object_store_set_object(zval *zobject, zend_object *object)
{
	EG(objects_store).object_buckets[Z_OBJ_HANDLE_P(zobject)] = object;
}

/* Called when the ctor was terminated by an exception */
ZEND_API void zend_object_store_ctor_failed(zend_object *obj)
{
	GC_FLAGS(obj) |= IS_OBJ_DESTRUCTOR_CALLED;
}

ZEND_API zend_object_handlers *zend_get_std_object_handlers(void)
{
	return &std_object_handlers;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
