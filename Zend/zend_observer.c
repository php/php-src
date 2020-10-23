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
   | Authors: Levi Morrison <levim@php.net>                               |
   |          Sammy Kaye Powers <sammyk@php.net>                          |
   +----------------------------------------------------------------------+
*/

#include "zend_observer.h"

#include "zend_extensions.h"
#include "zend_llist.h"
#include "zend_vm.h"

#define ZEND_OBSERVER_DATA(op_array) \
	ZEND_OP_ARRAY_EXTENSION(op_array, zend_observer_fcall_op_array_extension)

#define ZEND_OBSERVER_NOT_OBSERVED ((void *) 2)

#define ZEND_OBSERVABLE_FN(fn_flags) \
	(!(fn_flags & (ZEND_ACC_CALL_VIA_TRAMPOLINE | ZEND_ACC_FAKE_CLOSURE)))

typedef struct _zend_observer_fcall_data {
	// points after the last handler
	zend_observer_fcall_handlers *end;
	// a variadic array using "struct hack"
	zend_observer_fcall_handlers handlers[1];
} zend_observer_fcall_data;

typedef struct _zend_observer_frame {
	zend_execute_data *execute_data;
	struct _zend_observer_frame *prev;
} zend_observer_frame;

zend_llist zend_observers_fcall_list;
zend_llist zend_observer_error_callbacks;

int zend_observer_fcall_op_array_extension = -1;

ZEND_TLS zend_arena *fcall_handlers_arena = NULL;
ZEND_TLS zend_arena *observed_stack_arena = NULL;
ZEND_TLS zend_observer_frame *current_observed_frame = NULL;

// Call during minit/startup ONLY
ZEND_API void zend_observer_fcall_register(zend_observer_fcall_init init) {
	/* We don't want to get an extension handle unless an ext installs an observer */
	if (!ZEND_OBSERVER_ENABLED) {
		zend_observer_fcall_op_array_extension =
			zend_get_op_array_extension_handle("Zend Observer");

		/* ZEND_CALL_TRAMPOLINE has SPEC(OBSERVER) but zend_init_call_trampoline_op()
		 * is called before any extensions have registered as an observer. So we
		 * adjust the offset to the observed handler when we know we need to observe. */
		ZEND_VM_SET_OPCODE_HANDLER(&EG(call_trampoline_op));

		/* ZEND_HANDLE_EXCEPTION also has SPEC(OBSERVER) and no observer extensions
		 * exist when zend_init_exception_op() is called. */
		ZEND_VM_SET_OPCODE_HANDLER(EG(exception_op));
		ZEND_VM_SET_OPCODE_HANDLER(EG(exception_op)+1);
		ZEND_VM_SET_OPCODE_HANDLER(EG(exception_op)+2);
	}
	zend_llist_add_element(&zend_observers_fcall_list, &init);
}

// Called by engine before MINITs
ZEND_API void zend_observer_startup(void) {
	zend_llist_init(&zend_observers_fcall_list, sizeof(zend_observer_fcall_init), NULL, 1);
	zend_llist_init(&zend_observer_error_callbacks, sizeof(zend_observer_error_cb), NULL, 1);
}

ZEND_API void zend_observer_activate(void) {
	if (ZEND_OBSERVER_ENABLED) {
		fcall_handlers_arena = zend_arena_create(4096);
		// TODO: How big should the arena be?
		observed_stack_arena = zend_arena_create(2048);
	}
}

ZEND_API void zend_observer_deactivate(void) {
	if (fcall_handlers_arena) {
		zend_arena_destroy(fcall_handlers_arena);
		zend_arena_destroy(observed_stack_arena);
	}
}

ZEND_API void zend_observer_shutdown(void) {
	zend_llist_destroy(&zend_observers_fcall_list);
	zend_llist_destroy(&zend_observer_error_callbacks);
}

static void zend_observer_fcall_install(zend_execute_data *execute_data) {
	zend_llist_element *element;
	zend_llist *list = &zend_observers_fcall_list;
	zend_function *function = execute_data->func;
	zend_op_array *op_array = &function->op_array;

	if (fcall_handlers_arena == NULL) {
		return;
	}

	ZEND_ASSERT(function->type != ZEND_INTERNAL_FUNCTION);

	zend_llist handlers_list;
	zend_llist_init(&handlers_list, sizeof(zend_observer_fcall_handlers), NULL, 0);
	for (element = list->head; element; element = element->next) {
		zend_observer_fcall_init init;
		memcpy(&init, element->data, sizeof init);
		zend_observer_fcall_handlers handlers = init(execute_data);
		if (handlers.begin || handlers.end) {
			zend_llist_add_element(&handlers_list, &handlers);
		}
	}

	ZEND_ASSERT(RUN_TIME_CACHE(op_array));
	void *ext;
	if (handlers_list.count) {
		size_t size = sizeof(zend_observer_fcall_data) + (handlers_list.count - 1) * sizeof(zend_observer_fcall_handlers);
		zend_observer_fcall_data *fcall_data = zend_arena_alloc(&fcall_handlers_arena, size);
		zend_observer_fcall_handlers *handlers = fcall_data->handlers;
		for (element = handlers_list.head; element; element = element->next) {
			memcpy(handlers++, element->data, sizeof *handlers);
		}
		fcall_data->end = handlers;
		ext = fcall_data;
	} else {
		ext = ZEND_OBSERVER_NOT_OBSERVED;
	}

	ZEND_OBSERVER_DATA(op_array) = ext;
	zend_llist_destroy(&handlers_list);
}

static void ZEND_FASTCALL _zend_observe_fcall_begin(zend_execute_data *execute_data)
{
	zend_op_array *op_array;
	uint32_t fn_flags;
	zend_observer_fcall_data *fcall_data;
	zend_observer_fcall_handlers *handlers, *end;
	zend_observer_frame *frame;

	if (!ZEND_OBSERVER_ENABLED) {
		return;
	}

	op_array = &execute_data->func->op_array;
	fn_flags = op_array->fn_flags;

	if (!ZEND_OBSERVABLE_FN(fn_flags)) {
		return;
	}

	fcall_data = ZEND_OBSERVER_DATA(op_array);
	if (!fcall_data) {
		zend_observer_fcall_install(execute_data);
		fcall_data = ZEND_OBSERVER_DATA(op_array);
	}

	ZEND_ASSERT(fcall_data);
	if (fcall_data == ZEND_OBSERVER_NOT_OBSERVED) {
		return;
	}

	frame = zend_arena_alloc(&observed_stack_arena, sizeof(zend_observer_frame));
	frame->execute_data = execute_data;
	frame->prev = current_observed_frame;
	current_observed_frame = frame;

	end = fcall_data->end;
	for (handlers = fcall_data->handlers; handlers != end; ++handlers) {
		if (handlers->begin) {
			handlers->begin(execute_data);
		}
	}
}

ZEND_API void ZEND_FASTCALL zend_observer_generator_resume(zend_execute_data *execute_data)
{
	_zend_observe_fcall_begin(execute_data);
}

ZEND_API void ZEND_FASTCALL zend_observer_fcall_begin(zend_execute_data *execute_data)
{
	ZEND_ASSUME(execute_data->func);
	if (!(execute_data->func->common.fn_flags & ZEND_ACC_GENERATOR)) {
		_zend_observe_fcall_begin(execute_data);
	}
}

ZEND_API void ZEND_FASTCALL zend_observer_fcall_end(
	zend_execute_data *execute_data,
	zval *return_value)
{
	zend_function *func = execute_data->func;
	zend_observer_fcall_data *fcall_data;
	zend_observer_fcall_handlers *handlers, *end;
	zend_observer_frame *frame;

	if (!ZEND_OBSERVER_ENABLED
	 || !ZEND_OBSERVABLE_FN(func->common.fn_flags)) {
		return;
	}

	fcall_data = (zend_observer_fcall_data*)ZEND_OBSERVER_DATA(&func->op_array);
	// TODO: Fix exceptions from generators
	// ZEND_ASSERT(fcall_data);
	if (!fcall_data || fcall_data == ZEND_OBSERVER_NOT_OBSERVED) {
		return;
	}

	handlers = fcall_data->end;
	end = fcall_data->handlers;
	while (handlers-- != end) {
		if (handlers->end) {
			handlers->end(execute_data, return_value);
		}
	}

	frame = current_observed_frame;
	current_observed_frame = frame->prev;
	zend_arena_release(&observed_stack_arena, frame);
}

ZEND_API void zend_observer_fcall_end_all(void)
{
	while (current_observed_frame != NULL) {
		zend_observer_fcall_end(current_observed_frame->execute_data, NULL);
	}
}

ZEND_API void zend_observer_error_register(zend_observer_error_cb cb)
{
	zend_llist_add_element(&zend_observer_error_callbacks, &cb);
}

void zend_observer_error_notify(int type, const char *error_filename, uint32_t error_lineno, zend_string *message)
{
	zend_llist_element *element;
	zend_observer_error_cb callback;

	for (element = zend_observer_error_callbacks.head; element; element = element->next) {
		callback = *(zend_observer_error_cb *) (element->data);
		callback(type, error_filename, error_lineno, message);
	}
}
