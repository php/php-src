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

#ifndef ZEND_OBSERVER_H
#define ZEND_OBSERVER_H

#include "zend.h"
#include "zend_compile.h"
#include "zend_fibers.h"

BEGIN_EXTERN_C()

extern ZEND_API int zend_observer_fcall_op_array_extension;
extern ZEND_API bool zend_observer_errors_observed;
extern ZEND_API bool zend_observer_function_declared_observed;
extern ZEND_API bool zend_observer_class_linked_observed;

typedef void (*zend_observer_fcall_begin_handler)(zend_execute_data *execute_data);
typedef void (*zend_observer_fcall_end_handler)(zend_execute_data *execute_data, zval *retval);

typedef struct _zend_observer_fcall_handlers {
	zend_observer_fcall_begin_handler begin;
	zend_observer_fcall_end_handler end;
} zend_observer_fcall_handlers;

#define ZEND_OBSERVER_DATA(function) \
	((zend_observer_fcall_begin_handler *)&ZEND_OP_ARRAY_EXTENSION((&(function)->common), zend_observer_fcall_op_array_extension))

#define ZEND_OBSERVER_NONE_OBSERVED ((void *) 3)

#define ZEND_OBSERVER_ENABLED (zend_observer_fcall_op_array_extension != -1)

#define ZEND_OBSERVER_FCALL_BEGIN(execute_data) do { \
		if (ZEND_OBSERVER_ENABLED) { \
			zend_observer_fcall_begin(execute_data); \
		} \
	} while (0)

#define ZEND_OBSERVER_FCALL_END(execute_data, return_value) do { \
		if (ZEND_OBSERVER_ENABLED) { \
			zend_observer_fcall_end(execute_data, return_value); \
		} \
	} while (0)

/* If the fn should not be observed then return {NULL, NULL} */
typedef zend_observer_fcall_handlers (*zend_observer_fcall_init)(zend_execute_data *execute_data);

// Call during minit/startup ONLY
ZEND_API void zend_observer_fcall_register(zend_observer_fcall_init);

// Call during runtime, but only if you have used zend_observer_fcall_register.
// You must not have more than one begin and one end handler active at the same time. Remove the old one first, if there is an existing one.
ZEND_API void zend_observer_add_begin_handler(zend_function *function, zend_observer_fcall_begin_handler begin);
ZEND_API bool zend_observer_remove_begin_handler(zend_function *function, zend_observer_fcall_begin_handler begin);
ZEND_API void zend_observer_add_end_handler(zend_function *function, zend_observer_fcall_end_handler end);
ZEND_API bool zend_observer_remove_end_handler(zend_function *function, zend_observer_fcall_end_handler end);

ZEND_API void zend_observer_startup(void); // Called by engine before MINITs
ZEND_API void zend_observer_post_startup(void); // Called by engine after MINITs
ZEND_API void zend_observer_activate(void);
ZEND_API void zend_observer_shutdown(void);

ZEND_API void ZEND_FASTCALL zend_observer_fcall_begin(zend_execute_data *execute_data);
void ZEND_FASTCALL zend_observer_fcall_begin_prechecked(zend_execute_data *execute_data, zend_observer_fcall_begin_handler *observer_data);

static zend_always_inline bool zend_observer_handler_is_unobserved(zend_observer_fcall_begin_handler *handler) {
	return *handler == ZEND_OBSERVER_NONE_OBSERVED;
}

static zend_always_inline bool zend_observer_fcall_has_no_observers(zend_execute_data *execute_data, bool allow_generator, zend_observer_fcall_begin_handler **handler) {
	zend_function *function = EX(func);
	void *ZEND_MAP_PTR(runtime_cache) = ZEND_MAP_PTR(function->common.run_time_cache);

	if (function->common.fn_flags & (ZEND_ACC_CALL_VIA_TRAMPOLINE | (allow_generator ? 0 : ZEND_ACC_GENERATOR))) {
		return true;
	}

	if (!ZEND_MAP_PTR(runtime_cache)) {
		return true;
	}

	*handler = (zend_observer_fcall_begin_handler *)ZEND_MAP_PTR_GET(runtime_cache) + zend_observer_fcall_op_array_extension;
	return zend_observer_handler_is_unobserved(*handler);
}

static zend_always_inline void zend_observer_fcall_begin_specialized(zend_execute_data *execute_data, bool allow_generator) {
	zend_observer_fcall_begin_handler *handler;
	if (!zend_observer_fcall_has_no_observers(execute_data, allow_generator, &handler)) {
		zend_observer_fcall_begin_prechecked(execute_data, handler);
	}
}

ZEND_API void ZEND_FASTCALL zend_observer_generator_resume(zend_execute_data *execute_data);

ZEND_API void ZEND_FASTCALL zend_observer_fcall_end_prechecked(zend_execute_data *execute_data, zval *return_value);
static zend_always_inline void zend_observer_fcall_end(zend_execute_data *execute_data, zval *return_value) {
	if (execute_data == EG(current_observed_frame)) {
		zend_observer_fcall_end_prechecked(execute_data, return_value);
	}
}

ZEND_API void zend_observer_fcall_end_all(void);

typedef void (*zend_observer_function_declared_cb)(zend_op_array *op_array, zend_string *name);

ZEND_API void zend_observer_function_declared_register(zend_observer_function_declared_cb cb);
ZEND_API void ZEND_FASTCALL _zend_observer_function_declared_notify(zend_op_array *op_array, zend_string *name);
static inline void zend_observer_function_declared_notify(zend_op_array *op_array, zend_string *name) {
    if (UNEXPECTED(zend_observer_function_declared_observed)) {
		_zend_observer_function_declared_notify(op_array, name);
	}
}

typedef void (*zend_observer_class_linked_cb)(zend_class_entry *ce, zend_string *name);

ZEND_API void zend_observer_class_linked_register(zend_observer_class_linked_cb cb);
ZEND_API void ZEND_FASTCALL _zend_observer_class_linked_notify(zend_class_entry *ce, zend_string *name);
static inline void zend_observer_class_linked_notify(zend_class_entry *ce, zend_string *name) {
	if (UNEXPECTED(zend_observer_class_linked_observed)) {
		_zend_observer_class_linked_notify(ce, name);
	}
}

typedef void (*zend_observer_error_cb)(int type, zend_string *error_filename, uint32_t error_lineno, zend_string *message);

ZEND_API void zend_observer_error_register(zend_observer_error_cb callback);
ZEND_API void _zend_observer_error_notify(int type, zend_string *error_filename, uint32_t error_lineno, zend_string *message);
static inline void zend_observer_error_notify(int type, zend_string *error_filename, uint32_t error_lineno, zend_string *message) {
	if (UNEXPECTED(zend_observer_errors_observed)) {
		_zend_observer_error_notify(type, error_filename, error_lineno, message);
	}
}

typedef void (*zend_observer_fiber_init_handler)(zend_fiber_context *initializing);
typedef void (*zend_observer_fiber_switch_handler)(zend_fiber_context *from, zend_fiber_context *to);
typedef void (*zend_observer_fiber_destroy_handler)(zend_fiber_context *destroying);

ZEND_API void zend_observer_fiber_init_register(zend_observer_fiber_init_handler handler);
ZEND_API void zend_observer_fiber_switch_register(zend_observer_fiber_switch_handler handler);
ZEND_API void zend_observer_fiber_destroy_register(zend_observer_fiber_destroy_handler handler);

ZEND_API void ZEND_FASTCALL zend_observer_fiber_init_notify(zend_fiber_context *initializing);
ZEND_API void ZEND_FASTCALL zend_observer_fiber_switch_notify(zend_fiber_context *from, zend_fiber_context *to);
ZEND_API void ZEND_FASTCALL zend_observer_fiber_destroy_notify(zend_fiber_context *destroying);

END_EXTERN_C()

#endif /* ZEND_OBSERVER_H */
