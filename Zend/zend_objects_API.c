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
#include "zend_exceptions.h"
#include "zend_fibers.h"
#include "zend_weakrefs.h"
#include "zend_gc.h"

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

ZEND_API void ZEND_FASTCALL zend_defer_dtor(zend_object *object)
{
	zend_dtor_buf *buf = &EG(deferred_dtors);
	zend_function *destructor = object->ce->destructor;

	ZEND_ASSERT(GC_REFCOUNT(object) == 0);
	GC_ADDREF(object);
	OBJ_EXTRA_FLAGS(object) |= IS_OBJ_DTOR_PENDING;
	if (destructor
	 && UNEXPECTED(destructor->common.fn_flags & (ZEND_ACC_PRIVATE|ZEND_ACC_PROTECTED))
	 && !zend_check_method_accessible(destructor, zend_get_executed_scope())) {
		OBJ_EXTRA_FLAGS(object) |= IS_OBJ_DTOR_VIS_FAIL;
	}
	GC_REMOVE_FROM_BUFFER(object);

	if (UNEXPECTED(GC_FLAGS(object) & IS_OBJ_WEAKLY_REFERENCED)) {
		zend_weakrefs_notify_detach(object);
	}

	if (UNEXPECTED(buf->count == buf->capacity)) {
		uint32_t capacity = buf->capacity ? buf->capacity + (buf->capacity >> 1) : 8;
		buf->objects = erealloc(buf->objects, sizeof(zend_object*) * capacity);
		buf->capacity = capacity;
	}
	buf->objects[buf->count++] = object;

	zend_atomic_bool_store_ex(&EG(vm_interrupt), true);
}

static zend_always_inline void zend_dtor_flush_stash_exception(zend_object **pending, const zend_op **first_throw_opline)
{
	if (UNEXPECTED(EG(exception))) {
		if (*pending) {
			if (zend_is_unwind_exit(*pending) || zend_is_graceful_exit(*pending)) {
				OBJ_RELEASE(EG(exception));
				EG(exception) = NULL;
				return;
			}
			if (zend_is_unwind_exit(EG(exception)) || zend_is_graceful_exit(EG(exception))) {
				OBJ_RELEASE(*pending);
			} else {
				zend_exception_set_previous(EG(exception), *pending);
			}
		}
		*pending = EG(exception);
		EG(exception) = NULL;
		if (!*first_throw_opline) {
			*first_throw_opline = EG(opline_before_exception);
		}
	}
}

static zend_always_inline bool zend_dtor_flush_get_op_num(
		const zend_op_array *op_array, const zend_op *opline, uint32_t *op_num)
{
	if (!opline || (uintptr_t) opline < (uintptr_t) op_array->opcodes) {
		return false;
	}

	uintptr_t offset = (uintptr_t) opline - (uintptr_t) op_array->opcodes;
	if (offset >= (uintptr_t) op_array->last * sizeof(zend_op)
	 || offset % sizeof(zend_op) != 0) {
		return false;
	}

	*op_num = offset / sizeof(zend_op);
	return true;
}

static zend_always_inline zend_execute_data *zend_dtor_flush_find_frame(
		zend_execute_data *execute_data, const zend_op *opline, uint32_t *op_num)
{
	for (; execute_data; execute_data = execute_data->prev_execute_data) {
		if (!execute_data->func
		 || !ZEND_USER_CODE(execute_data->func->common.type)) {
			continue;
		}

		if (zend_dtor_flush_get_op_num(
				&execute_data->func->op_array, opline, op_num)) {
			return execute_data;
		}
	}

	return NULL;
}

static zend_always_inline bool zend_dtor_flush_exception_is_catchable(
		zend_execute_data *execute_data, const zend_op *first_throw_opline,
		const zend_op *flush_opline)
{
	uint32_t throw_op_num;
	zend_execute_data *throw_frame = zend_dtor_flush_find_frame(
		execute_data, first_throw_opline, &throw_op_num);

	if (!throw_frame) {
		throw_frame = zend_dtor_flush_find_frame(
			execute_data, flush_opline, &throw_op_num);
	}
	if (!throw_frame) {
		for (zend_execute_data *ex = execute_data; ex; ex = ex->prev_execute_data) {
			if (ex->func && ZEND_USER_CODE(ex->func->common.type) && ex->opline
			 && zend_dtor_flush_get_op_num(&ex->func->op_array, ex->opline, &throw_op_num)) {
				throw_frame = ex;
				break;
			}
		}
	}
	if (!throw_frame) {
		return false;
	}

	execute_data = throw_frame;
	for (; execute_data; execute_data = execute_data->prev_execute_data) {
		if (!execute_data->func
		 || !ZEND_USER_CODE(execute_data->func->common.type)
		 || !execute_data->opline) {
			continue;
		}

		const zend_op_array *op_array = &execute_data->func->op_array;
		uint32_t op_num;

		if (execute_data == throw_frame) {
			op_num = throw_op_num;
		} else if (!zend_dtor_flush_get_op_num(
				op_array, execute_data->opline, &op_num)) {
			continue;
		}

		const zend_op *opline = &op_array->opcodes[op_num];
		if ((opline->opcode == ZEND_FREE || opline->opcode == ZEND_FE_FREE)
		 && (opline->extended_value & ZEND_FREE_ON_RETURN)) {
			op_num += opline->op2.opline_num;
		}

		for (uint32_t i = 0; i < op_array->last_try_catch; i++) {
			const zend_try_catch_element *try_catch = &op_array->try_catch_array[i];

			if (try_catch->try_op > op_num) {
				break;
			}
			if (op_num < try_catch->catch_op || op_num < try_catch->finally_end) {
				return true;
			}
		}
	}

	return false;
}

static zend_always_inline bool zend_dtor_flush_release_throw_operand_at(
		zend_execute_data *execute_data, const zend_op *flush_opline)
{
	uint32_t op_num;
	zend_execute_data *frame = zend_dtor_flush_find_frame(
		execute_data, flush_opline, &op_num);

	if (!frame) {
		return false;
	}

	const zend_op_array *op_array = &frame->func->op_array;
	if (op_num + 1 >= op_array->last
	 || op_array->opcodes[op_num].opcode != ZEND_FLUSH_DEFERRED_DTORS) {
		return false;
	}

	const zend_op *throw_op = &op_array->opcodes[op_num + 1];
	if (throw_op->opcode != ZEND_THROW) {
		return false;
	}

	if (throw_op->op1_type & (IS_TMP_VAR | IS_VAR)) {
		zval *operand = ZEND_CALL_VAR(frame, throw_op->op1.var);

		EG(frame_teardown)++;
		zval_ptr_dtor_nogc(operand);
		EG(frame_teardown)--;
		ZVAL_UNDEF(operand);
	}

	return true;
}

static zend_always_inline void zend_dtor_flush_release_throw_operand(
		zend_execute_data *execute_data, const zend_op *first_throw_opline,
		const zend_op *flush_opline)
{
	if (zend_dtor_flush_release_throw_operand_at(
			execute_data, first_throw_opline)) {
		return;
	}

	zend_dtor_flush_release_throw_operand_at(
		execute_data, flush_opline);
}

static zend_always_inline void zend_flush_deferred_dtors_ex(bool surface_uncatchable)
{
	zend_dtor_buf *buf = &EG(deferred_dtors);
	zend_execute_data *execute_data = EG(current_execute_data);
	const zend_op *flush_opline = execute_data && execute_data->func
		&& ZEND_USER_CODE(execute_data->func->common.type) ? execute_data->opline : NULL;
	const zend_op *saved_opline_before_exception = EG(opline_before_exception);
	zend_object *pending = EG(exception);
	const zend_op *first_throw_opline = NULL;
	bool had_exception = pending != NULL;
	bool opline_rewound = false;

	if (UNEXPECTED(pending != NULL)) {
		EG(exception) = NULL;
		if (flush_opline == EG(exception_op)
		 && saved_opline_before_exception) {
			execute_data->opline = saved_opline_before_exception;
			opline_rewound = true;
		}
	}

	EG(frame_teardown)++;
	while (buf->count || buf->values_count) {
		zend_dtor_buf batch = *buf;
		memset(buf, 0, sizeof(*buf));

		for (uint32_t i = 0; i < batch.count; i++) {
			zend_object *object = batch.objects[i];
			OBJ_RELEASE(object);
			zend_dtor_flush_stash_exception(&pending, &first_throw_opline);
		}
		if (batch.objects) {
			efree(batch.objects);
		}

		for (uint32_t j = 0; j < batch.values_count; j++) {
			zval_ptr_dtor(&batch.values[j]);
			zend_dtor_flush_stash_exception(&pending, &first_throw_opline);
		}
		if (batch.values) {
			efree(batch.values);
		}
	}
	EG(frame_teardown)--;

	if (opline_rewound) {
		execute_data->opline = EG(exception_op);
	}

	if (EXPECTED(pending == NULL)) {
		return;
	}

	bool catchable = !had_exception
		&& zend_dtor_flush_exception_is_catchable(
			execute_data, first_throw_opline, flush_opline);

	if (had_exception
	 || catchable
	 || surface_uncatchable
	 || zend_is_unwind_exit(pending)
	 || zend_is_graceful_exit(pending)) {
		zend_execute_data *ex = EG(current_execute_data);

		EG(exception) = pending;
		if (had_exception) {
			EG(opline_before_exception) = saved_opline_before_exception;
		} else if (first_throw_opline
		 && ex && ex->func && ZEND_USER_CODE(ex->func->common.type)
		 && ex->opline == EG(exception_op)) {
			EG(opline_before_exception) = first_throw_opline;
		}

		zend_dtor_flush_release_throw_operand(
			execute_data, first_throw_opline, flush_opline);
		return;
	}

	if (EG(deferred_dtor_exception)) {
		zend_object *previous = EG(deferred_dtor_exception);
		EG(deferred_dtor_exception) = NULL;
		zend_exception_set_previous(pending, previous);
	}
	EG(deferred_dtor_exception) = pending;

	if (flush_opline && execute_data->opline == EG(exception_op)) {
		execute_data->opline = flush_opline;
	}
	EG(opline_before_exception) = saved_opline_before_exception;
}

ZEND_API void ZEND_FASTCALL zend_flush_deferred_dtors(void)
{
	zend_flush_deferred_dtors_ex(false);
}

ZEND_API void ZEND_FASTCALL zend_flush_deferred_dtors_surface(void)
{
	zend_flush_deferred_dtors_ex(true);
}

ZEND_API void ZEND_FASTCALL zend_surface_deferred_dtor_exception(void)
{
	zend_object *exception = EG(deferred_dtor_exception);

	if (!exception) {
		return;
	}
	if (EG(exception)
	 && (zend_is_unwind_exit(EG(exception)) || zend_is_graceful_exit(EG(exception)))) {
		return;
	}

	EG(deferred_dtor_exception) = NULL;
	if (EG(exception)) {
		zend_exception_set_previous(EG(exception), exception);
	} else {
		EG(exception) = exception;
	}
}

ZEND_API void ZEND_FASTCALL zend_release_deferred_dtors(void)
{
	zend_dtor_buf *buf = &EG(deferred_dtors);

	for (uint32_t i = 0; i < buf->count; i++) {
		zend_object *object = buf->objects[i];
		OBJ_EXTRA_FLAGS(object) &= ~(IS_OBJ_DTOR_PENDING|IS_OBJ_DTOR_VIS_FAIL);
		GC_DELREF(object);
	}
	for (uint32_t i = 0; i < buf->values_count; i++) {
		zval_ptr_dtor(&buf->values[i]);
	}
	if (buf->objects) {
		efree(buf->objects);
	}
	if (buf->values) {
		efree(buf->values);
	}
	memset(buf, 0, sizeof(*buf));
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

static zend_always_inline bool zend_can_defer_dtor(zend_object *object)
{
	if (!EG(dtor_defer_gate) || EG(frame_teardown)) {
		return false;
	}
	if (OBJ_EXTRA_FLAGS(object) & IS_OBJ_DTOR_PENDING) {
		return false;
	}
	if (CG(in_compilation) || !EG(current_execute_data) || !EG(current_execute_data)->func) {
		return false;
	}
	zend_execute_data *ex = EG(current_execute_data);
	if (!ZEND_USER_CODE(ex->func->type) || !ex->opline) {
		return false;
	}
	switch (ex->opline->opcode) {
		case ZEND_DECLARE_CLASS:
		case ZEND_DECLARE_CLASS_DELAYED:
		case ZEND_DECLARE_ANON_CLASS:
		case ZEND_INIT_FCALL:
		case ZEND_INIT_FCALL_BY_NAME:
		case ZEND_INIT_NS_FCALL_BY_NAME:
		case ZEND_INIT_METHOD_CALL:
		case ZEND_INIT_STATIC_METHOD_CALL:
		case ZEND_INIT_USER_CALL:
		case ZEND_INIT_DYNAMIC_CALL:
		case ZEND_INIT_PARENT_PROPERTY_HOOK_CALL:
		case ZEND_DO_FCALL:
		case ZEND_DO_FCALL_BY_NAME:
		case ZEND_DO_UCALL:
		case ZEND_DO_ICALL:
		case ZEND_FETCH_CONSTANT:
		case ZEND_FETCH_CLASS_CONSTANT:
		case ZEND_INCLUDE_OR_EVAL:
			return false;
	}
	return true;
}

ZEND_API void ZEND_FASTCALL zend_objects_store_del(zend_object *object) /* {{{ */
{
	ZEND_ASSERT(GC_REFCOUNT(object) == 0);

	/* GC might have released this object already. */
	if (UNEXPECTED(GC_TYPE(object) == IS_NULL)) {
		return;
	}

	if (!(OBJ_FLAGS(object) & IS_OBJ_DESTRUCTOR_CALLED)) {
		bool has_dtor = object->handlers->dtor_obj != zend_objects_destroy_object
			|| object->ce->destructor;

		if (has_dtor
				&& zend_can_defer_dtor(object)
				&& !zend_gc_is_condemned((zend_refcounted*)object)) {
			zend_defer_dtor(object);
			return;
		}

		/*	Make sure we hold a reference count during the destructor call
			otherwise, when the destructor ends the storage might be freed
			when the refcount reaches 0 a second time
		 */
		GC_ADD_FLAGS(object, IS_OBJ_DESTRUCTOR_CALLED);

		if (has_dtor) {
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
