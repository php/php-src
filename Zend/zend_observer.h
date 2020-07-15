// todo: license garbage

#ifndef ZEND_OBSERVER_H
#define ZEND_OBSERVER_H

#include "zend.h"
#include "zend_compile.h"

BEGIN_EXTERN_C()

extern int zend_observer_fcall_op_array_extension;

#define ZEND_OBSERVER_NOT_OBSERVED ((void *) 2)

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

void zend_observe_fcall_begin(
	zend_observer_fcall_cache *cache,
	zend_execute_data *execute_data);

void zend_observe_fcall_end(
	zend_observer_fcall_cache *cache,
	zend_execute_data *execute_data,
	zval *return_value);

ZEND_API inline void zend_observer_maybe_fcall_call_end(
	zend_execute_data *execute_data,
	zval *return_value)
{
	if (zend_observer_fcall_op_array_extension == -1) {
		return;
	}

	zend_function *func = execute_data->func;
	if (!(func->common.fn_flags & (ZEND_ACC_CALL_VIA_TRAMPOLINE | ZEND_ACC_FAKE_CLOSURE))) {
		void *observer_handlers =
			ZEND_OP_ARRAY_EXTENSION(&func->op_array, zend_observer_fcall_op_array_extension);
		if (observer_handlers != ZEND_OBSERVER_NOT_OBSERVED) {
			zend_observer_fcall_cache *cache = observer_handlers;
			zend_observe_fcall_end(cache, execute_data, return_value);
		}
	}
}

END_EXTERN_C()

#endif /* ZEND_OBSERVER_H */
