/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright © Zend Technologies Ltd., a subsidiary company of          |
   |     Perforce Software, Inc., and Contributors.                       |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
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
#include "zend_async_API.h"
#include "zend_execute.h"

ZEND_API void ZEND_FASTCALL zend_objects_store_init(zend_objects_store *objects, uint32_t init_size)
{
	objects->object_buckets = (zend_object **) emalloc(init_size * sizeof(zend_object*));
	objects->top = 1; /* Skip 0 so that handles are true */
	objects->size = init_size;
	objects->free_list_head = -1;
	memset(&objects->object_buckets[0], 0, sizeof(zend_object*));
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
		for (uint32_t i = 1; i < objects->top; i++) {
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
	}
}

/* Continue the pass in a fresh coroutine, picking up wherever the
 * interrupted one left off. */
static void zend_objects_store_call_destructors_async_iterator_entry(void)
{
	zend_objects_store_call_destructors_async(&EG(objects_store));
}

/* Reset the shutdown cursor if the iterator coroutine is torn down mid-pass
 * (cancelled/errored) instead of finishing normally — otherwise the pass
 * stays marked in flight forever. Dispose may run inside a bailout: no user
 * code from here, the remaining destructors are given up. */
static void zend_objects_store_call_destructors_async_coroutine_dtor(zend_coroutine_t *coroutine)
{
	if (EG(shutdown_context).coroutine == coroutine) {
		EG(shutdown_context).coroutine = NULL;
		EG(shutdown_context).pass = ZEND_SHUTDOWN_PASS_NONE;
		zend_error(E_WARNING, "Object store destructors coroutine was not finished properly");
		zend_objects_store_mark_destructed(&EG(objects_store));
	}
}

/* On the driving coroutine's leave (a destructor suspended), continue the
 * pass in a fresh iterator. Synchronous, not a microtask: this runs late in
 * shutdown, where a next tick may never come. */
static bool zend_objects_store_call_destructors_async_switch_handler(zend_coroutine_t *coroutine, bool is_enter)
{
	(void) coroutine;

	if (is_enter) {
		return true;
	}

	if (EG(shutdown_context).pass != ZEND_SHUTDOWN_PASS_OBJECTS) {
		return false;
	}

	zend_coroutine_t *iterator = ZEND_ASYNC_GC_NEW_COROUTINE();

	if (UNEXPECTED(iterator == NULL)) {
		return false;
	}

	iterator->internal_entry = zend_objects_store_call_destructors_async_iterator_entry;
	iterator->extended_dispose = zend_objects_store_call_destructors_async_coroutine_dtor;

	ZEND_ASYNC_ENQUEUE_COROUTINE(iterator);

	return false;
}

ZEND_API void ZEND_FASTCALL zend_objects_store_call_destructors_async(zend_objects_store *objects)
{
	if (objects->top <= 1) {
		return;
	}

	EG(flags) |= EG_FLAGS_OBJECT_STORE_NO_REUSE;

	zend_coroutine_t *coroutine = ZEND_ASYNC_IS_ACTIVE ? ZEND_ASYNC_CURRENT_COROUTINE : NULL;

	if (coroutine != NULL) {
		ZEND_ASYNC_ADD_SWITCH_HANDLER(
				coroutine, zend_objects_store_call_destructors_async_switch_handler);
	}

	if (EG(shutdown_context).pass != ZEND_SHUTDOWN_PASS_OBJECTS) {
		EG(shutdown_context).pass = ZEND_SHUTDOWN_PASS_OBJECTS;
		EG(shutdown_context).coroutine = coroutine;
		EG(shutdown_context).idx = 1;
	}

	/* Under a scheduler, skip live fibers and coroutine objects: a destructor
	 * may spawn a fiber and await it, and destroying that still-parked fiber
	 * would break the destructor about to resume it. The drain tears them down
	 * later. Without a scheduler, fibers take the upstream path: the dtor
	 * force-closes them (the parked GC destructor fiber relies on it). */
	const bool skip_flows = ZEND_ASYNC_IS_ACTIVE;
	zend_class_entry *coroutine_ce =
			skip_flows ? ZEND_ASYNC_GET_CE(ZEND_ASYNC_CLASS_COROUTINE) : NULL;

	for (uint32_t i = EG(shutdown_context).idx; i < objects->top; i++) {
		zend_object *obj = objects->object_buckets[i];

		if (IS_OBJ_VALID(obj)
				&& (!skip_flows || (obj->ce != zend_ce_fiber && obj->ce != coroutine_ce))) {
			if (!(OBJ_FLAGS(obj) & IS_OBJ_DESTRUCTOR_CALLED)) {
				GC_ADD_FLAGS(obj, IS_OBJ_DESTRUCTOR_CALLED);

				if (obj->handlers->dtor_obj != zend_objects_destroy_object
						|| obj->ce->destructor) {
					EG(shutdown_context).idx = i;
					GC_ADDREF(obj);
					obj->handlers->dtor_obj(obj);
					GC_DELREF(obj);

					/* Destructor suspended (current coroutine changed): stop —
					 * the switch handler spawned an iterator to carry on. */
					if (coroutine != NULL && coroutine != ZEND_ASYNC_CURRENT_COROUTINE) {
						return;
					}
				}
			}
		}
	}

	EG(shutdown_context).pass = ZEND_SHUTDOWN_PASS_NONE;
	EG(shutdown_context).coroutine = NULL;
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
					if (obj->handlers->free_obj != zend_object_std_dtor
					 || (OBJ_FLAGS(obj) & IS_OBJ_WEAKLY_REFERENCED)
					) {
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
	uint32_t new_size = 2 * EG(objects_store).size;

	EG(objects_store).object_buckets = (zend_object **) erealloc(EG(objects_store).object_buckets, new_size * sizeof(zend_object*));
	/* Assign size after realloc, in case it fails */
	EG(objects_store).size = new_size;
	uint32_t handle = EG(objects_store).top++;
	object->handle = handle;
	EG(objects_store).object_buckets[handle] = object;
}

ZEND_API void ZEND_FASTCALL zend_objects_store_put(zend_object *object)
{
	uint32_t handle;

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
			GC_SET_REFCOUNT(object, 1);
			object->handlers->dtor_obj(object);
			GC_DELREF(object);
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
/* }}} */

ZEND_API ZEND_COLD zend_property_info *zend_get_property_info_for_slot_slow(zend_object *obj, zval *slot)
{
	uintptr_t offset = OBJ_PROP_SLOT_TO_OFFSET(obj, slot);
	zend_property_info *prop_info;
	ZEND_HASH_MAP_FOREACH_PTR(&obj->ce->properties_info, prop_info) {
		if (prop_info->offset == offset) {
			return prop_info;
		}
	} ZEND_HASH_FOREACH_END();
	return NULL;
}
