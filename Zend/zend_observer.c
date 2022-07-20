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

#define ZEND_OBSERVER_DATA(function) \
	ZEND_OP_ARRAY_EXTENSION((&(function)->common), zend_observer_fcall_op_array_extension)

#define ZEND_OBSERVER_NOT_OBSERVED ((void *) 2)

#define ZEND_OBSERVABLE_FN(function) \
	(ZEND_MAP_PTR(function->common.run_time_cache) && !(function->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE))

zend_llist zend_observers_fcall_list;
zend_llist zend_observer_error_callbacks;
zend_llist zend_observer_fiber_init;
zend_llist zend_observer_fiber_switch;
zend_llist zend_observer_fiber_destroy;

int zend_observer_fcall_op_array_extension;

ZEND_TLS zend_execute_data *first_observed_frame;
ZEND_TLS zend_execute_data *current_observed_frame;

// Call during minit/startup ONLY
ZEND_API void zend_observer_fcall_register(zend_observer_fcall_init init)
{
	zend_llist_add_element(&zend_observers_fcall_list, &init);
}

// Called by engine before MINITs
ZEND_API void zend_observer_startup(void)
{
	zend_llist_init(&zend_observers_fcall_list, sizeof(zend_observer_fcall_init), NULL, 1);
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
	}
}

ZEND_API void zend_observer_activate(void)
{
	first_observed_frame = NULL;
	current_observed_frame = NULL;
}

ZEND_API void zend_observer_shutdown(void)
{
	zend_llist_destroy(&zend_observers_fcall_list);
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
	zend_observer_fcall_begin_handler *begin_handlers = (zend_observer_fcall_begin_handler *)&ZEND_OBSERVER_DATA(function);
	zend_observer_fcall_end_handler *end_handlers = (zend_observer_fcall_end_handler *)begin_handlers + list->count, *end_handlers_start = end_handlers;

	*begin_handlers = ZEND_OBSERVER_NOT_OBSERVED;
	*end_handlers = ZEND_OBSERVER_NOT_OBSERVED;

	for (zend_llist_element *element = list->head; element; element = element->next) {
		zend_observer_fcall_init init;
		memcpy(&init, element->data, sizeof init);
		zend_observer_fcall_handlers handlers = init(execute_data);
		if (handlers.begin) {
			*(begin_handlers++) = handlers.begin;
		}
		if (handlers.end) {
			*(end_handlers++) = handlers.end;
		}
	}

	// end handlers are executed in reverse order
	for (--end_handlers; end_handlers_start < end_handlers; --end_handlers, ++end_handlers_start) {
		zend_observer_fcall_end_handler tmp = *end_handlers;
		*end_handlers = *end_handlers_start;
		*end_handlers_start = tmp;
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
				} else {
					*last_handler = NULL;
				}
			}
			return true;
		}
	}
	return false;
}

ZEND_API void zend_observer_add_begin_handler(zend_function *function, zend_observer_fcall_begin_handler begin) {
	size_t registered_observers = zend_observers_fcall_list.count;
	zend_observer_fcall_begin_handler *first_handler = (void *)&ZEND_OBSERVER_DATA(function), *last_handler = first_handler + registered_observers - 1;
	if (*first_handler == ZEND_OBSERVER_NOT_OBSERVED) {
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
	return zend_observer_remove_handler((void **)&ZEND_OBSERVER_DATA(function), begin);
}

ZEND_API void zend_observer_add_end_handler(zend_function *function, zend_observer_fcall_end_handler end) {
	size_t registered_observers = zend_observers_fcall_list.count;
	zend_observer_fcall_end_handler *end_handler = (zend_observer_fcall_end_handler *)&ZEND_OBSERVER_DATA(function) + registered_observers;
	// to allow to preserve the invariant that end handlers are in reverse order of begin handlers, push the new end handler in front
	if (*end_handler != ZEND_OBSERVER_NOT_OBSERVED) {
		// there's no space for new handlers, then it's forbidden to call this function
		ZEND_ASSERT(end_handler[registered_observers - 1] == NULL);
		memmove(end_handler + 1, end_handler, registered_observers - 1);
	}
	*end_handler = end;
}

ZEND_API bool zend_observer_remove_end_handler(zend_function *function, zend_observer_fcall_end_handler end) {
	size_t registered_observers = zend_observers_fcall_list.count;
	return zend_observer_remove_handler((void **)&ZEND_OBSERVER_DATA(function) + registered_observers, end);
}

static void ZEND_FASTCALL _zend_observe_fcall_begin(zend_execute_data *execute_data)
{
	if (!ZEND_OBSERVER_ENABLED) {
		return;
	}

	zend_function *function = execute_data->func;

	if (!ZEND_OBSERVABLE_FN(function)) {
		return;
	}

	zend_observer_fcall_begin_handler *handler = (zend_observer_fcall_begin_handler *)&ZEND_OBSERVER_DATA(function);
	if (!*handler) {
		zend_observer_fcall_install(execute_data);
	}

	zend_observer_fcall_begin_handler *possible_handlers_end = handler + zend_observers_fcall_list.count;

	zend_observer_fcall_end_handler *end_handler = (zend_observer_fcall_end_handler *)possible_handlers_end;
	if (*end_handler != ZEND_OBSERVER_NOT_OBSERVED) {
		if (first_observed_frame == NULL) {
			first_observed_frame = execute_data;
		}
		current_observed_frame = execute_data;
	}

	if (*handler == ZEND_OBSERVER_NOT_OBSERVED) {
		return;
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
	ZEND_ASSUME(execute_data->func);
	if (!(execute_data->func->common.fn_flags & ZEND_ACC_GENERATOR)) {
		_zend_observe_fcall_begin(execute_data);
	}
}

static inline bool zend_observer_is_skipped_frame(zend_execute_data *execute_data) {
	zend_function *func = execute_data->func;

	if (!func || !ZEND_OBSERVABLE_FN(func)) {
		return true;
	}

	zend_observer_fcall_end_handler end_handler = (&ZEND_OBSERVER_DATA(func))[zend_observers_fcall_list.count];
	if (end_handler == NULL || end_handler == ZEND_OBSERVER_NOT_OBSERVED) {
		return true;
	}

	return false;
}

ZEND_API void ZEND_FASTCALL zend_observer_fcall_end(zend_execute_data *execute_data, zval *return_value)
{
	zend_function *func = execute_data->func;

	if (!ZEND_OBSERVER_ENABLED || !ZEND_OBSERVABLE_FN(func)) {
		return;
	}

	zend_observer_fcall_end_handler *handler = (zend_observer_fcall_end_handler *)&ZEND_OBSERVER_DATA(func) + zend_observers_fcall_list.count;
	// TODO: Fix exceptions from generators
	// ZEND_ASSERT(fcall_data);
	if (!*handler || *handler == ZEND_OBSERVER_NOT_OBSERVED) {
		return;
	}

	zend_observer_fcall_end_handler *possible_handlers_end = handler + zend_observers_fcall_list.count;
	do {
		(*handler)(execute_data, return_value);
	} while (++handler != possible_handlers_end && *handler != NULL);

	if (first_observed_frame == execute_data) {
		first_observed_frame = NULL;
		current_observed_frame = NULL;
	} else {
		zend_execute_data *ex = execute_data->prev_execute_data;
		while (ex && zend_observer_is_skipped_frame(ex)) {
			ex = ex->prev_execute_data;
		}
		current_observed_frame = ex;
	}
}

ZEND_API void zend_observer_fcall_end_all(void)
{
	zend_execute_data *ex = current_observed_frame;
	while (ex != NULL) {
		if (ex->func) {
			zend_observer_fcall_end(ex, NULL);
		}
		ex = ex->prev_execute_data;
	}
}

ZEND_API void zend_observer_error_register(zend_observer_error_cb cb)
{
	zend_llist_add_element(&zend_observer_error_callbacks, &cb);
}

void zend_observer_error_notify(int type, zend_string *error_filename, uint32_t error_lineno, zend_string *message)
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

	for (element = zend_observer_fiber_init.head; element; element = element->next) {
		callback = *(zend_observer_fiber_init_handler *) element->data;
		callback(initializing);
	}
}

ZEND_API void ZEND_FASTCALL zend_observer_fiber_switch_notify(zend_fiber_context *from, zend_fiber_context *to)
{
	zend_llist_element *element;
	zend_observer_fiber_switch_handler callback;

	for (element = zend_observer_fiber_switch.head; element; element = element->next) {
		callback = *(zend_observer_fiber_switch_handler *) element->data;
		callback(from, to);
	}
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
