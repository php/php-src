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

#ifndef ZEND_OBSERVER_H
#define ZEND_OBSERVER_H

#include "zend.h"
#include "zend_compile.h"

BEGIN_EXTERN_C()

extern ZEND_API int zend_observer_fcall_op_array_extension;

#define ZEND_OBSERVER_ENABLED (zend_observer_fcall_op_array_extension != -1)

#define ZEND_OBSERVER_HANDLERS(op_array) \
	ZEND_OP_ARRAY_EXTENSION(op_array, zend_observer_fcall_op_array_extension)

#define ZEND_OBSERVER_NOT_OBSERVED ((void *) 2)

#define ZEND_OBSERVABLE_FN(fn_flags) \
	(ZEND_OBSERVER_ENABLED && \
		!(fn_flags & (ZEND_ACC_CALL_VIA_TRAMPOLINE | ZEND_ACC_FAKE_CLOSURE)))

struct zend_observer_fcall {
	void (*begin)(zend_execute_data *execute_data);
	void (*end)(zend_execute_data *execute_data, zval *retval);
};
typedef struct zend_observer_fcall zend_observer_fcall;

struct zend_observer_fcall_cache {
	// points after the last handler
	zend_observer_fcall *end;
	// a variadic array using "struct hack"
	zend_observer_fcall handlers[1];
};
typedef struct zend_observer_fcall_cache zend_observer_fcall_cache;

/* If the fn should not be observed then return {NULL, NULL} */
typedef zend_observer_fcall(*zend_observer_fcall_init)(zend_function *func);

// Call during minit/startup ONLY
ZEND_API void zend_observer_fcall_register(zend_observer_fcall_init init);

ZEND_API void zend_observer_startup(void); // Called by engine before MINITs
ZEND_API void zend_observer_activate(void);
ZEND_API void zend_observer_deactivate(void);
ZEND_API void zend_observer_shutdown(void);

ZEND_API void zend_observer_fcall_install(zend_function *function);

ZEND_API void zend_observe_fcall_begin(
	zend_observer_fcall_cache *cache,
	zend_execute_data *execute_data);

ZEND_API void zend_observe_fcall_end(
	zend_observer_fcall_cache *cache,
	zend_execute_data *execute_data,
	zval *return_value);

ZEND_API void zend_observer_fcall_call_end_helper(
	zend_execute_data *execute_data,
	zval *return_value);

ZEND_API zend_always_inline void zend_observer_maybe_fcall_call_begin(
	zend_execute_data *execute_data)
{
	ZEND_ASSUME(execute_data->func);
	zend_op_array *op_array = &execute_data->func->op_array;
	uint32_t fn_flags = op_array->fn_flags;
	if (ZEND_OBSERVABLE_FN(fn_flags) && !(fn_flags & ZEND_ACC_GENERATOR)) {
		void *observer_handlers = ZEND_OBSERVER_HANDLERS(op_array);
		if (!observer_handlers) {
			zend_observer_fcall_install((zend_function *)op_array);
			observer_handlers = ZEND_OBSERVER_HANDLERS(op_array);
		}

		ZEND_ASSERT(observer_handlers);
		if (observer_handlers != ZEND_OBSERVER_NOT_OBSERVED) {
			zend_observe_fcall_begin(
				(zend_observer_fcall_cache *)observer_handlers,
				execute_data);
		}
	}
}

ZEND_API zend_always_inline void zend_observer_maybe_fcall_call_end(
	zend_execute_data *execute_data,
	zval *return_value)
{
	zend_function *func = execute_data->func;
	if (ZEND_OBSERVABLE_FN(func->common.fn_flags)) {
		zend_observer_fcall_call_end_helper(execute_data, return_value);
	}
}

typedef void (*zend_observer_error_cb)(int type, const char *error_filename, uint32_t error_lineno, zend_string *message);

ZEND_API void zend_observer_error_register(zend_observer_error_cb callback);
void zend_observer_error_notify(int type, const char *error_filename, uint32_t error_lineno, zend_string *message);

END_EXTERN_C()

#endif /* ZEND_OBSERVER_H */
