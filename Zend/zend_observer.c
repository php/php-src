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
   |          Bob Weinand <bobwei9@hotmail.com>                           |
   +----------------------------------------------------------------------+
*/

#include "zend_observer.h"

#include "zend_extensions.h"
#include "zend_llist.h"
#include "zend_vm.h"

#define ZEND_OBSERVER_NOT_OBSERVED ((void *) 2)

zend_llist zend_observers_fcall_list;
zend_llist zend_observer_function_declared_callbacks;
zend_llist zend_observer_class_linked_callbacks;
zend_llist zend_observer_error_callbacks;
zend_llist zend_observer_fiber_init;
zend_llist zend_observer_fiber_switch;
zend_llist zend_observer_fiber_destroy;

int zend_observer_fcall_op_array_extension;
bool zend_observer_errors_observed;
bool zend_observer_function_declared_observed;
bool zend_observer_class_linked_observed;

// Call during minit/startup ONLY
ZEND_API void zend_observer_fcall_register(zend_observer_fcall_init init)
{
	zend_llist_add_element(&zend_observers_fcall_list, &init);
}

// Called by engine before MINITs
ZEND_API void zend_observer_startup(void)
{
	zend_llist_init(&zend_observers_fcall_list, sizeof(zend_observer_fcall_init), NULL, 1);
	zend_llist_init(&zend_observer_function_declared_callbacks, sizeof(zend_observer_function_declared_cb), NULL, 1);
	zend_llist_init(&zend_observer_class_linked_callbacks, sizeof(zend_observer_class_linked_cb), NULL, 1);
	zend_llist_init(&zend_observer_error_callbacks, sizeof(zend_observer_error_cb), NULL, 1);
	zend_llist_init(&zend_observer_fiber_init, sizeof(zend_observer_fiber_init_handler), NULL, 1);
	zend_llist_init(&zend_observer_fiber_switch, sizeof(zend_observer_fiber_switch_handler), NULL, 1);
	zend_llist_init(&zend_observer_fiber_destroy, sizeof(zend_observer_fiber_destroy_handler), NULL, 1);

	zend_observer_fcall_op_array_extension = -1;
}

ZEND_API void zend_observer_post_startup(void)
{
	if (zend_observers_fcall_list.count) {
		/* We don't want to get an extension handle unless an ext installs an observer
		 * Allocate each a begin and an end pointer */
		zend_observer_fcall_op_array_extension =
			zend_get_op_array_extension_handles("Zend Observer", (int) zend_observers_fcall_list.count * 2);

		/* ZEND_CALL_TRAMPOLINE has SPEC(OBSERVER) but zend_init_call_trampoline_op()
		 * is called before any extensions have registered as an observer. So we
		 * adjust the offset to the observed handler when we know we need to observe. */
		ZEND_VM_SET_OPCODE_HANDLER(&EG(call_trampoline_op));

		/* ZEND_HANDLE_EXCEPTION also has SPEC(OBSERVER) and no observer extensions
		 * exist when zend_init_exception_op() is called. */
		ZEND_VM_SET_OPCODE_HANDLER(EG(exception_op));
		ZEND_VM_SET_OPCODE_HANDLER(EG(exception_op) + 1);
		ZEND_VM_SET_OPCODE_HANDLER(EG(exception_op) + 2);

		// Add an observer temporary to store previous observed frames
		zend_internal_function *zif;
		ZEND_HASH_FOREACH_PTR(CG(function_table), zif) {
			++zif->T;
		} ZEND_HASH_FOREACH_END();
		zend_class_entry *ce;
		ZEND_HASH_MAP_FOREACH_PTR(CG(class_table), ce) {
			ZEND_HASH_MAP_FOREACH_PTR(&ce->function_table, zif) {
				++zif->T;
			} ZEND_HASH_FOREACH_END();
		} ZEND_HASH_FOREACH_END();
	}
}

ZEND_API void zend_observer_activate(void)
{
    EG(current_observed_frame) = NULL;
}

ZEND_API void zend_observer_shutdown(void)
{
	zend_llist_destroy(&zend_observers_fcall_list);
	zend_llist_destroy(&zend_observer_function_declared_callbacks);
	zend_llist_destroy(&zend_observer_class_linked_callbacks);
	zend_llist_destroy(&zend_observer_error_callbacks);
	zend_llist_destroy(&zend_observer_fiber_init);
	zend_llist_destroy(&zend_observer_fiber_switch);
	zend_llist_destroy(&zend_observer_fiber_destroy);
}

static void zend_observer_fcall_install(zend_execute_data *execute_data)
{
	zend_llist *list = &zend_observers_fcall_list;
	zend_function *function = execute_data->func;

	ZEND_ASSERT(RUN_TIME_CACHE(&function->common));
	zend_observer_fcall_begin_handler *begin_handlers = ZEND_OBSERVER_DATA(function), *begin_handlers_start = begin_handlers;
	zend_observer_fcall_end_handler *end_handlers = (zend_observer_fcall_end_handler *)begin_handlers + list->count, *end_handlers_start = end_handlers;

	*begin_handlers = ZEND_OBSERVER_NOT_OBSERVED;
	*end_handlers = ZEND_OBSERVER_NOT_OBSERVED;
	bool has_handlers = false;

	for (zend_llist_element *element = list->head; element; element = element->next) {
		zend_observer_fcall_init init;
		memcpy(&init, element->data, sizeof init);
		zend_observer_fcall_handlers handlers = init(execute_data);
		if (handlers.begin) {
			*(begin_handlers++) = handlers.begin;
			has_handlers = true;
		}
		if (handlers.end) {
			*(end_handlers++) = handlers.end;
			has_handlers = true;
		}
	}

	// end handlers are executed in reverse order
	for (--end_handlers; end_handlers_start < end_handlers; --end_handlers, ++end_handlers_start) {
		zend_observer_fcall_end_handler tmp = *end_handlers;
		*end_handlers = *end_handlers_start;
		*end_handlers_start = tmp;
	}

	if (!has_handlers) {
		*begin_handlers_start = ZEND_OBSERVER_NONE_OBSERVED;
	}
}

static bool zend_observer_remove_handler(void **first_handler, void *old_handler) {
	size_t registered_observers = zend_observers_fcall_list.count;

	void **last_handler = first_handler + registered_observers - 1;
	for (void **cur_handler = first_handler; cur_handler <= last_handler; ++cur_handler) {
		if (*cur_handler == old_handler) {
			if (registered_observers == 1 || (cur_handler == first_handler && cur_handler[1] == NULL)) {
				*cur_handler = ZEND_OBSERVER_NOT_OBSERVED;
			} else {
				if (cur_handler != last_handler) {
					memmove(cur_handler, cur_handler + 1, sizeof(cur_handler) * (last_handler - cur_handler));
				}
				*last_handler = NULL;
			}
			return true;
		}
	}
	return false;
}

ZEND_API void zend_observer_add_begin_handler(zend_function *function, zend_observer_fcall_begin_handler begin) {
	size_t registered_observers = zend_observers_fcall_list.count;
	zend_observer_fcall_begin_handler *first_handler = ZEND_OBSERVER_DATA(function), *last_handler = first_handler + registered_observers - 1;
	if (*first_handler == ZEND_OBSERVER_NOT_OBSERVED || *first_handler == ZEND_OBSERVER_NONE_OBSERVED) {
		*first_handler = begin;
	} else {
		for (zend_observer_fcall_begin_handler *cur_handler = first_handler + 1; cur_handler <= last_handler; ++cur_handler) {
			if (*cur_handler == NULL) {
				*cur_handler = begin;
				return;
			}
		}
		// there's no space for new handlers, then it's forbidden to call this function
		ZEND_UNREACHABLE();
	}
}

ZEND_API bool zend_observer_remove_begin_handler(zend_function *function, zend_observer_fcall_begin_handler begin) {
	void **begin_handlers = (void **)ZEND_OBSERVER_DATA(function);
	if (zend_observer_remove_handler(begin_handlers, begin)) {
		if (*begin_handlers == ZEND_OBSERVER_NOT_OBSERVED) {
			size_t registered_observers = zend_observers_fcall_list.count;
			if (begin_handlers[registered_observers] /* first end handler */ == ZEND_OBSERVER_NOT_OBSERVED) {
				*begin_handlers = ZEND_OBSERVER_NONE_OBSERVED;
			}
		}
		return true;
	}
	return false;
}

ZEND_API void zend_observer_add_end_handler(zend_function *function, zend_observer_fcall_end_handler end) {
	size_t registered_observers = zend_observers_fcall_list.count;
	void **begin_handler = (void **)ZEND_OBSERVER_DATA(function);
	zend_observer_fcall_end_handler *end_handler = (zend_observer_fcall_end_handler *)begin_handler + registered_observers;
	// to allow to preserve the invariant that end handlers are in reverse order of begin handlers, push the new end handler in front
	if (*end_handler != ZEND_OBSERVER_NOT_OBSERVED) {
		// there's no space for new handlers, then it's forbidden to call this function
		ZEND_ASSERT(end_handler[registered_observers - 1] == NULL);
		memmove(end_handler + 1, end_handler, sizeof(end_handler) * (registered_observers - 1));
	} else if (*begin_handler == ZEND_OBSERVER_NONE_OBSERVED) {
		*begin_handler = ZEND_OBSERVER_NOT_OBSERVED;
	}
	*end_handler = end;
}

ZEND_API bool zend_observer_remove_end_handler(zend_function *function, zend_observer_fcall_end_handler end) {
	size_t registered_observers = zend_observers_fcall_list.count;
	void **begin_handlers = (void **)ZEND_OBSERVER_DATA(function);
	void **end_handlers = begin_handlers + registered_observers;
	if (zend_observer_remove_handler(end_handlers, end)) {
		if (*begin_handlers == ZEND_OBSERVER_NOT_OBSERVED && *end_handlers == ZEND_OBSERVER_NOT_OBSERVED) {
			*begin_handlers = ZEND_OBSERVER_NONE_OBSERVED;
		}
		return true;
	}
	return false;
}

static inline zend_execute_data **prev_observed_frame(zend_execute_data *execute_data) {
	zend_function *func = EX(func);
	ZEND_ASSERT(func);
	return (zend_execute_data **)&Z_PTR_P(EX_VAR_NUM((ZEND_USER_CODE(func->type) ? func->op_array.last_var : ZEND_CALL_NUM_ARGS(execute_data)) + func->common.T - 1));
}

static void ZEND_FASTCALL _zend_observe_fcall_begin(zend_execute_data *execute_data) {
	if (!ZEND_OBSERVER_ENABLED) {
		return;
	}

	zend_observer_fcall_begin_specialized(execute_data, true);
}

void ZEND_FASTCALL zend_observer_fcall_begin_prechecked(zend_execute_data *execute_data, zend_observer_fcall_begin_handler *handler)
{
	zend_observer_fcall_begin_handler *possible_handlers_end = handler + zend_observers_fcall_list.count;

	if (!*handler) {
		zend_observer_fcall_install(execute_data);
		if (zend_observer_handler_is_unobserved(handler)) {
			return;
		}
	}

	zend_observer_fcall_end_handler *end_handler = (zend_observer_fcall_end_handler *)possible_handlers_end;
	if (*end_handler != ZEND_OBSERVER_NOT_OBSERVED) {
		*prev_observed_frame(execute_data) = EG(current_observed_frame);
		EG(current_observed_frame) = execute_data;

		if (*handler == ZEND_OBSERVER_NOT_OBSERVED) { // this function must not be called if ZEND_OBSERVER_NONE_OBSERVED, hence sufficient to check
			return;
		}
	}

	do {
		(*handler)(execute_data);
	} while (++handler != possible_handlers_end && *handler != NULL);
}

ZEND_API void ZEND_FASTCALL zend_observer_generator_resume(zend_execute_data *execute_data)
{
	_zend_observe_fcall_begin(execute_data);
}

ZEND_API void ZEND_FASTCALL zend_observer_fcall_begin(zend_execute_data *execute_data)
{
	ZEND_ASSUME(EX(func));
	if (!(EX(func)->common.fn_flags & ZEND_ACC_GENERATOR)) {
		_zend_observe_fcall_begin(execute_data);
	}
}

static inline void call_end_observers(zend_execute_data *execute_data, zval *return_value) {
	zend_function *func = EX(func);
	ZEND_ASSERT(func);

	zend_observer_fcall_end_handler *handler = (zend_observer_fcall_end_handler *)ZEND_OBSERVER_DATA(func) + zend_observers_fcall_list.count;
	// TODO: Fix exceptions from generators
	// ZEND_ASSERT(fcall_data);
	if (!*handler || *handler == ZEND_OBSERVER_NOT_OBSERVED) {
		return;
	}

	zend_observer_fcall_end_handler *possible_handlers_end = handler + zend_observers_fcall_list.count;
	do {
		(*handler)(execute_data, return_value);
	} while (++handler != possible_handlers_end && *handler != NULL);
}

ZEND_API void ZEND_FASTCALL zend_observer_fcall_end_prechecked(zend_execute_data *execute_data, zval *return_value)
{
	call_end_observers(execute_data, return_value);
	EG(current_observed_frame) = *prev_observed_frame(execute_data);
}

ZEND_API void zend_observer_fcall_end_all(void)
{
	zend_execute_data *execute_data = EG(current_observed_frame), *original_execute_data = EG(current_execute_data);
    EG(current_observed_frame) = NULL;
	while (execute_data) {
		EG(current_execute_data) = execute_data;
		call_end_observers(execute_data, NULL);
		execute_data = *prev_observed_frame(execute_data);
	}
	EG(current_execute_data) = original_execute_data;
}

ZEND_API void zend_observer_function_declared_register(zend_observer_function_declared_cb cb)
{
	zend_observer_function_declared_observed = true;
	zend_llist_add_element(&zend_observer_function_declared_callbacks, &cb);
}

ZEND_API void ZEND_FASTCALL _zend_observer_function_declared_notify(zend_op_array *op_array, zend_string *name)
{
	if (CG(compiler_options) & ZEND_COMPILE_IGNORE_OBSERVER) {
		return;
	}

	for (zend_llist_element *element = zend_observer_function_declared_callbacks.head; element; element = element->next) {
		zend_observer_function_declared_cb callback = *(zend_observer_function_declared_cb *) (element->data);
		callback(op_array, name);
	}
}

ZEND_API void zend_observer_class_linked_register(zend_observer_class_linked_cb cb)
{
	zend_observer_class_linked_observed = true;
	zend_llist_add_element(&zend_observer_class_linked_callbacks, &cb);
}

ZEND_API void ZEND_FASTCALL _zend_observer_class_linked_notify(zend_class_entry *ce, zend_string *name)
{
	if (CG(compiler_options) & ZEND_COMPILE_IGNORE_OBSERVER) {
		return;
	}

	for (zend_llist_element *element = zend_observer_class_linked_callbacks.head; element; element = element->next) {
		zend_observer_class_linked_cb callback = *(zend_observer_class_linked_cb *) (element->data);
		callback(ce, name);
	}
}

ZEND_API void zend_observer_error_register(zend_observer_error_cb cb)
{
	zend_observer_errors_observed = true;
	zend_llist_add_element(&zend_observer_error_callbacks, &cb);
}

ZEND_API void _zend_observer_error_notify(int type, zend_string *error_filename, uint32_t error_lineno, zend_string *message)
{
	for (zend_llist_element *element = zend_observer_error_callbacks.head; element; element = element->next) {
		zend_observer_error_cb callback = *(zend_observer_error_cb *) (element->data);
		callback(type, error_filename, error_lineno, message);
	}
}

ZEND_API void zend_observer_fiber_init_register(zend_observer_fiber_init_handler handler)
{
	zend_llist_add_element(&zend_observer_fiber_init, &handler);
}

ZEND_API void zend_observer_fiber_switch_register(zend_observer_fiber_switch_handler handler)
{
	zend_llist_add_element(&zend_observer_fiber_switch, &handler);
}

ZEND_API void zend_observer_fiber_destroy_register(zend_observer_fiber_destroy_handler handler)
{
	zend_llist_add_element(&zend_observer_fiber_destroy, &handler);
}

ZEND_API void ZEND_FASTCALL zend_observer_fiber_init_notify(zend_fiber_context *initializing)
{
	zend_llist_element *element;
	zend_observer_fiber_init_handler callback;

	initializing->top_observed_frame = NULL;

	for (element = zend_observer_fiber_init.head; element; element = element->next) {
		callback = *(zend_observer_fiber_init_handler *) element->data;
		callback(initializing);
	}
}

ZEND_API void ZEND_FASTCALL zend_observer_fiber_switch_notify(zend_fiber_context *from, zend_fiber_context *to)
{
	zend_llist_element *element;
	zend_observer_fiber_switch_handler callback;

	if (from->status == ZEND_FIBER_STATUS_DEAD) {
		zend_observer_fcall_end_all(); // fiber is either finished (call will do nothing) or has bailed out
	}

	for (element = zend_observer_fiber_switch.head; element; element = element->next) {
		callback = *(zend_observer_fiber_switch_handler *) element->data;
		callback(from, to);
	}

	from->top_observed_frame = EG(current_observed_frame);
    EG(current_observed_frame) = to->top_observed_frame;
}

ZEND_API void ZEND_FASTCALL zend_observer_fiber_destroy_notify(zend_fiber_context *destroying)
{
	zend_llist_element *element;
	zend_observer_fiber_destroy_handler callback;

	for (element = zend_observer_fiber_destroy.head; element; element = element->next) {
		callback = *(zend_observer_fiber_destroy_handler *) element->data;
		callback(destroying);
	}
}
