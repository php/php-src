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

zend_llist zend_observers_fcall_list;
int zend_observer_fcall_op_array_extension = -1;

ZEND_TLS zend_arena *fcall_handlers_arena = NULL;

ZEND_API extern inline void zend_observer_maybe_fcall_call_begin(
	zend_execute_data *execute_data);
ZEND_API extern inline void zend_observer_maybe_fcall_call_end(
	zend_execute_data *execute_data,
	zval *return_value);

// Call during minit/startup ONLY
ZEND_API void zend_observer_fcall_register(zend_observer_fcall_init init) {
	/* We don't want to get an extension handle unless an ext installs an observer */
	if (!ZEND_OBSERVER_ENABLED) {
		zend_observer_fcall_op_array_extension =
			zend_get_op_array_extension_handle();

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
}

ZEND_API void zend_observer_activate(void) {
	if (ZEND_OBSERVER_ENABLED) {
		fcall_handlers_arena = zend_arena_create(4096);
	}
}

ZEND_API void zend_observer_deactivate(void) {
	if (fcall_handlers_arena) {
		zend_arena_destroy(fcall_handlers_arena);
	}
}

ZEND_API void zend_observer_shutdown(void) {
	zend_llist_destroy(&zend_observers_fcall_list);
}

ZEND_API void zend_observer_fcall_install(zend_function *function) {
	zend_llist_element *element;
	zend_llist *list = &zend_observers_fcall_list;
	zend_op_array *op_array = &function->op_array;

	if (fcall_handlers_arena == NULL) {
		return;
	}

	ZEND_ASSERT(function->type != ZEND_INTERNAL_FUNCTION);

	zend_llist handlers_list;
	zend_llist_init(&handlers_list, sizeof(zend_observer_fcall), NULL, 0);
	for (element = list->head; element; element = element->next) {
		zend_observer_fcall_init init;
		memcpy(&init, element->data, sizeof init);
		zend_observer_fcall handlers = init(function);
		if (handlers.begin || handlers.end) {
			zend_llist_add_element(&handlers_list, &handlers);
		}
	}

	ZEND_ASSERT(RUN_TIME_CACHE(op_array));
	void *ext;
	if (handlers_list.count) {
		size_t size = sizeof(zend_observer_fcall_cache) + (handlers_list.count - 1) * sizeof(zend_observer_fcall);
		zend_observer_fcall_cache *cache = zend_arena_alloc(&fcall_handlers_arena, size);
		zend_observer_fcall *handler = cache->handlers;
		for (element = handlers_list.head; element; element = element->next) {
			memcpy(handler++, element->data, sizeof *handler);
		}
		cache->end = handler;
		ext = cache;
	} else {
		ext = ZEND_OBSERVER_NOT_OBSERVED;
	}

	ZEND_OBSERVER_HANDLERS(op_array) = ext;
	zend_llist_destroy(&handlers_list);
}

ZEND_API void zend_observe_fcall_begin(
	zend_observer_fcall_cache *cache,
	zend_execute_data *execute_data)
{
	zend_observer_fcall *handler, *end = cache->end;
	for (handler = cache->handlers; handler != end; ++handler) {
		if (handler->begin) {
			handler->begin(execute_data);
		}
	}
}

ZEND_API void zend_observer_fcall_call_end_helper(
	zend_execute_data *execute_data,
	zval *return_value)
{
	zend_function *func = execute_data->func;
	ZEND_ASSUME(ZEND_OBSERVABLE_FN(func->common.fn_flags));
	void *observer_handlers = ZEND_OBSERVER_HANDLERS(&func->op_array);
	// TODO: Fix exceptions from generators
	// ZEND_ASSERT(observer_handlers);
	if (observer_handlers && observer_handlers != ZEND_OBSERVER_NOT_OBSERVED) {
		zend_observer_fcall_cache *cache = observer_handlers;
		zend_observe_fcall_end(cache, execute_data, return_value);
	}
}

ZEND_API void zend_observe_fcall_end(
	zend_observer_fcall_cache *cache,
	zend_execute_data *execute_data,
	zval *return_value)
{
	zend_observer_fcall *handler = cache->end, *end = cache->handlers;
	while (handler-- != end) {
		if (handler->end) {
			handler->end(execute_data, return_value);
		}
	}
}


