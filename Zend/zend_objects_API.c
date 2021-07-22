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
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_globals.h"
#include "zend_variables.h"
#include "zend_API.h"
#include "zend_objects_API.h"
#include "zend_fibers.h"

static HashTable zend_objects_store_persistent = {
	.nTableSize = 0,
	.nNumUsed   = 0
};

ZEND_TLS HashTable zend_objects_store_unpersist;

struct _zend_object_persistent {
	zend_class_entry *class;
	HashTable         constructors;
	uint32_t          handle;
};

typedef struct {
	zend_object_persistent_constructor function;
	void *data;
	size_t size;
} zend_object_persistent_constructor_call;

ZEND_API void ZEND_FASTCALL zend_objects_store_init(zend_objects_store *objects, uint32_t init_size)
{
	uint32_t min_init_size = MAX(init_size, zend_objects_store_persistent.nNumUsed);

	objects->object_buckets = (zend_object **) emalloc(min_init_size * sizeof(zend_object*));
	objects->top = 1; /* Skip 0 so that handles are true */
	objects->size = min_init_size;
	objects->free_list_head = -1;
	memset(&objects->object_buckets[0], 0, sizeof(zend_object*));

	if (UNEXPECTED(zend_objects_store_persistent.nNumUsed)) {
		zend_object_persistent *persistent;

		zend_hash_init(&zend_objects_store_unpersist,
			zend_objects_store_persistent.nNumUsed, NULL, zval_ptr_dtor, 0);

		ZEND_HASH_FOREACH_PTR(&zend_objects_store_persistent, persistent) {
			zval zv;

			object_init_ex(&zv, persistent->class);

			if (persistent->constructors.nNumUsed) {
				zend_object_persistent_constructor_call *call;

				ZEND_HASH_FOREACH_PTR(&persistent->constructors, call) {
					call->function(Z_OBJ(zv), call->data);
				} ZEND_HASH_FOREACH_END();
			}

			zend_hash_next_index_insert(&zend_objects_store_unpersist, &zv);
		} ZEND_HASH_FOREACH_END();
	}
}

ZEND_API void ZEND_FASTCALL zend_objects_store_destroy(zend_objects_store *objects)
{
	efree(objects->object_buckets);
	objects->object_buckets = NULL;
}

ZEND_API void ZEND_FASTCALL zend_objects_store_call_destructors(zend_objects_store *objects)
{
	EG(flags) |= EG_FLAGS_OBJECT_STORE_NO_REUSE;
	if (objects->top > 1) {
		zend_fiber_switch_block();

		uint32_t i;
		for (i = 1; i < objects->top; i++) {
			zend_object *obj = objects->object_buckets[i];
			if (IS_OBJ_VALID(obj)) {
				if (!(OBJ_FLAGS(obj) & IS_OBJ_DESTRUCTOR_CALLED)) {
					GC_ADD_FLAGS(obj, IS_OBJ_DESTRUCTOR_CALLED);

					if (obj->handlers->dtor_obj != zend_objects_destroy_object
							|| obj->ce->destructor) {
						GC_ADDREF(obj);
						obj->handlers->dtor_obj(obj);
						GC_DELREF(obj);
					}
				}
			}
		}

		zend_fiber_switch_unblock();
	}
}

ZEND_API void ZEND_FASTCALL zend_objects_store_mark_destructed(zend_objects_store *objects)
{
	if (objects->object_buckets && objects->top > 1) {
		zend_object **obj_ptr = objects->object_buckets + 1;
		zend_object **end = objects->object_buckets + objects->top;

		do {
			zend_object *obj = *obj_ptr;

			if (IS_OBJ_VALID(obj)) {
				GC_ADD_FLAGS(obj, IS_OBJ_DESTRUCTOR_CALLED);
			}
			obj_ptr++;
		} while (obj_ptr != end);
	}
}

ZEND_API void ZEND_FASTCALL zend_objects_store_free_object_storage(zend_objects_store *objects, bool fast_shutdown)
{
	zend_object **obj_ptr, **end, *obj;

	if (objects->top <= 1) {
		return;
	}

	if (UNEXPECTED(zend_objects_store_unpersist.nNumUsed)) {
		zend_hash_destroy(&zend_objects_store_unpersist);
	}

	/* Free object contents, but don't free objects themselves, so they show up as leaks.
	 * Also add a ref to all objects, so the object can't be freed by something else later. */
	end = objects->object_buckets + 1;
	obj_ptr = objects->object_buckets + objects->top;

	if (fast_shutdown) {
		do {
			obj_ptr--;
			obj = *obj_ptr;
			if (IS_OBJ_VALID(obj)) {
				if (!(OBJ_FLAGS(obj) & IS_OBJ_FREE_CALLED)) {
					GC_ADD_FLAGS(obj, IS_OBJ_FREE_CALLED);
					if (obj->handlers->free_obj != zend_object_std_dtor) {
						GC_ADDREF(obj);
						obj->handlers->free_obj(obj);
					}
				}
			}
		} while (obj_ptr != end);
	} else {
		do {
			obj_ptr--;
			obj = *obj_ptr;
			if (IS_OBJ_VALID(obj)) {
				if (!(OBJ_FLAGS(obj) & IS_OBJ_FREE_CALLED)) {
					GC_ADD_FLAGS(obj, IS_OBJ_FREE_CALLED);
					GC_ADDREF(obj);
					obj->handlers->free_obj(obj);
				}
			}
		} while (obj_ptr != end);
	}
}


/* Store objects API */
static ZEND_COLD zend_never_inline void ZEND_FASTCALL zend_objects_store_put_cold(zend_object *object)
{
	int handle;
	uint32_t new_size = 2 * EG(objects_store).size;

	EG(objects_store).object_buckets = (zend_object **) erealloc(EG(objects_store).object_buckets, new_size * sizeof(zend_object*));
	/* Assign size after realloc, in case it fails */
	EG(objects_store).size = new_size;
	handle = EG(objects_store).top++;
	object->handle = handle;
	EG(objects_store).object_buckets[handle] = object;
}

ZEND_API void ZEND_FASTCALL zend_objects_store_put(zend_object *object)
{
	int handle;

	/* When in shutdown sequence - do not reuse previously freed handles, to make sure
	 * the dtors for newly created objects are called in zend_objects_store_call_destructors() loop
	 */
	if (EG(objects_store).free_list_head != -1 && EXPECTED(!(EG(flags) & EG_FLAGS_OBJECT_STORE_NO_REUSE))) {
		handle = EG(objects_store).free_list_head;
		EG(objects_store).free_list_head = GET_OBJ_BUCKET_NUMBER(EG(objects_store).object_buckets[handle]);
	} else if (UNEXPECTED(EG(objects_store).top == EG(objects_store).size)) {
		zend_objects_store_put_cold(object);
		return;
	} else {
		handle = EG(objects_store).top++;
	}
	object->handle = handle;
	EG(objects_store).object_buckets[handle] = object;
}

static void zend_object_persistent_dtor(zval *zv)
{
	zend_object_persistent *persistent = Z_PTR_P(zv);

	if (persistent->constructors.nNumUsed) {
		zend_hash_destroy(&persistent->constructors);
	}

	pefree(persistent, 1);
}

ZEND_API zend_object_persistent* ZEND_FASTCALL zend_objects_store_persist(zend_class_entry *ce)
{
	ZEND_ASSERT(ce->type == ZEND_INTERNAL_CLASS);

	if (UNEXPECTED(zend_objects_store_persistent.nTableSize == 0)) {
		zend_hash_init(&zend_objects_store_persistent, 8, NULL, zend_object_persistent_dtor, 1);
	}

	zend_object_persistent persistent = {
		.class = ce,
		.constructors = {
			.nTableSize = 0,
			.nNumUsed   = 0
		},
		.handle = zend_hash_num_elements(&zend_objects_store_persistent)
	};

	return zend_hash_next_index_insert_mem(&zend_objects_store_persistent, &persistent, sizeof(persistent));
}

static void zend_object_persistent_constructor_dtor(zval *zv)
{
	zend_object_persistent_constructor_call *call = Z_PTR_P(zv);

	if (EXPECTED(call->size && call->size != ZEND_OBJECT_PERSISTENT_PTR)) {
		pefree(call->data, 1);
	}

	pefree(call, 1);
}

ZEND_API void zend_object_persistent_construct(zend_object_persistent *object, zend_object_persistent_constructor constructor, void *data, size_t size)
{
	if (UNEXPECTED(object->constructors.nTableSize == 0)) {
		zend_hash_init(&object->constructors, 2, NULL, zend_object_persistent_constructor_dtor, 1);
	}

	zend_object_persistent_constructor_call construct = {
		.function = constructor,
		.size     = size,
		.data     = NULL
	};

	if (size == ZEND_OBJECT_PERSISTENT_PTR) {
		construct.data = data;
	} else if (size) {
		construct.data = pemalloc(size, 1);

		memcpy(construct.data, data, size);
	}

	zend_hash_next_index_insert_mem(&object->constructors, &construct, sizeof(construct));
}

ZEND_API uint32_t zend_object_persistent_handle(zend_object_persistent *persistent)
{
	return persistent->handle;
}

ZEND_API void ZEND_FASTCALL zend_objects_store_del(zend_object *object) /* {{{ */
{
	ZEND_ASSERT(GC_REFCOUNT(object) == 0);

	/* GC might have released this object already. */
	if (UNEXPECTED(GC_TYPE(object) == IS_NULL)) {
		return;
	}

	/*	Make sure we hold a reference count during the destructor call
		otherwise, when the destructor ends the storage might be freed
		when the refcount reaches 0 a second time
	 */
	if (!(OBJ_FLAGS(object) & IS_OBJ_DESTRUCTOR_CALLED)) {
		GC_ADD_FLAGS(object, IS_OBJ_DESTRUCTOR_CALLED);

		if (object->handlers->dtor_obj != zend_objects_destroy_object
				|| object->ce->destructor) {
			zend_fiber_switch_block();
			GC_SET_REFCOUNT(object, 1);
			object->handlers->dtor_obj(object);
			GC_DELREF(object);
			zend_fiber_switch_unblock();
		}
	}

	if (GC_REFCOUNT(object) == 0) {
		uint32_t handle = object->handle;
		void *ptr;

		ZEND_ASSERT(EG(objects_store).object_buckets != NULL);
		ZEND_ASSERT(IS_OBJ_VALID(EG(objects_store).object_buckets[handle]));
		EG(objects_store).object_buckets[handle] = SET_OBJ_INVALID(object);
		if (!(OBJ_FLAGS(object) & IS_OBJ_FREE_CALLED)) {
			GC_ADD_FLAGS(object, IS_OBJ_FREE_CALLED);
			GC_SET_REFCOUNT(object, 1);
			object->handlers->free_obj(object);
		}
		ptr = ((char*)object) - object->handlers->offset;
		GC_REMOVE_FROM_BUFFER(object);
		efree(ptr);
		ZEND_OBJECTS_STORE_ADD_TO_FREE_LIST(handle);
	}
}

ZEND_API void zend_objects_store_persistent_shutdown(void)
{
	zend_hash_destroy(&zend_objects_store_persistent);

	memset(&zend_objects_store_persistent, 0, sizeof(zend_objects_store_persistent));
}
/* }}} */
