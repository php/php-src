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
*/

#ifndef ZEND_CALLABLE_H
#define ZEND_CALLABLE_H

#include "zend_types.h"
#include "zend_compile.h"
#include "zend_alloc.h"
#include "zend_globals_macros.h" /* For EG() */
#include "zend_string.h" /* Needed for zend_call_known_fcc() implementation */

typedef struct _zend_fcall_info {
	size_t size;
	zval function_name;
	zval *retval;
	zval *params;
	zend_object *object;
	uint32_t param_count;
	/* This hashtable can also contain positional arguments (with integer keys),
	 * which will be appended to the normal params[]. This makes it easier to
	 * integrate APIs like call_user_func_array(). The usual restriction that
	 * there may not be position arguments after named arguments applies. */
	HashTable *named_params;
} zend_fcall_info;

typedef struct _zend_fcall_info_cache {
	zend_function *function_handler;
	zend_class_entry *calling_scope;
	zend_class_entry *called_scope;
	zend_object *object; /* Instance of object for method calls */
	zend_object *closure; /* Closure reference, only if the callable *is* the object */
} zend_fcall_info_cache;

BEGIN_EXTERN_C()

#define ZEND_FCI_INITIALIZED(fci) ((fci).size != 0)
#define ZEND_FCC_INITIALIZED(fcc) ((fcc).function_handler != NULL)

#define IS_CALLABLE_CHECK_SYNTAX_ONLY (1<<0)
#define IS_CALLABLE_SUPPRESS_DEPRECATIONS (1<<1)

ZEND_API zend_string *zend_get_callable_name_ex(zval *callable, zend_object *object);
ZEND_API zend_string *zend_get_callable_name(zval *callable);
ZEND_API bool zend_is_callable_at_frame(
		zval *callable, zend_object *object, zend_execute_data *frame,
		uint32_t check_flags, zend_fcall_info_cache *fcc, char **error);
ZEND_API bool zend_is_callable_ex(zval *callable, zend_object *object, uint32_t check_flags, zend_string **callable_name, zend_fcall_info_cache *fcc, char **error);
ZEND_API bool zend_is_callable(zval *callable, uint32_t check_flags, zend_string **callable_name);
ZEND_API bool zend_make_callable(zval *callable, zend_string **callable_name);

ZEND_API zend_result _call_user_function_impl(zval *object, zval *function_name, zval *retval_ptr, uint32_t param_count, zval params[], HashTable *named_params);

#define call_user_function(function_table, object, function_name, retval_ptr, param_count, params) \
	_call_user_function_impl(object, function_name, retval_ptr, param_count, params, NULL)

#define call_user_function_named(function_table, object, function_name, retval_ptr, param_count, params, named_params) \
	_call_user_function_impl(object, function_name, retval_ptr, param_count, params, named_params)

ZEND_API extern const zend_fcall_info empty_fcall_info;
ZEND_API extern const zend_fcall_info_cache empty_fcall_info_cache;

/** Build zend_call_info/cache from a zval*
 *
 * Caller is responsible to provide a return value (fci->retval), otherwise the we will crash.
 * In order to pass parameters the following members need to be set:
 * fci->param_count = 0;
 * fci->params = NULL;
 * The callable_name argument may be NULL.
 */
ZEND_API zend_result zend_fcall_info_init(zval *callable, uint32_t check_flags, zend_fcall_info *fci, zend_fcall_info_cache *fcc, zend_string **callable_name, char **error);

/* Zend FCC API to store and handle PHP userland functions */
static zend_always_inline bool zend_fcc_equals(const zend_fcall_info_cache* a, const zend_fcall_info_cache* b)
{
	if (UNEXPECTED((a->function_handler->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) &&
		(b->function_handler->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE))) {
		return a->object == b->object
			&& a->calling_scope == b->calling_scope
			&& a->closure == b->closure
			&& zend_string_equals(a->function_handler->common.function_name, b->function_handler->common.function_name)
		;
	}
	return a->function_handler == b->function_handler
		&& a->object == b->object
		&& a->calling_scope == b->calling_scope
		&& a->closure == b->closure
	;
}

static zend_always_inline void zend_fcc_addref(zend_fcall_info_cache *const fcc)
{
	ZEND_ASSERT(ZEND_FCC_INITIALIZED(*fcc) && "FCC Not initialized, possibly refetch trampoline freed by ZPP?");
	/* If the cached trampoline is set, free it */
	if (UNEXPECTED(fcc->function_handler == &EG(trampoline))) {
		zend_function *copy = (zend_function*)emalloc(sizeof(zend_function));

		memcpy(copy, fcc->function_handler, sizeof(zend_function));
		fcc->function_handler->common.function_name = NULL;
		fcc->function_handler = copy;
	}
	if (fcc->object) {
		GC_ADDREF(fcc->object);
	}
	if (fcc->closure) {
		GC_ADDREF(fcc->closure);
	}
}

static zend_always_inline void zend_fcc_dup(/* restrict */ zend_fcall_info_cache *dest, /* restrict */ const zend_fcall_info_cache *src)
{
	memcpy(dest, src, sizeof(zend_fcall_info_cache));
	zend_fcc_addref(dest);
}

/* Release trampoline stored in FCC */
ZEND_API void zend_release_fcall_info_cache(zend_fcall_info_cache *fcc);

ZEND_API void zend_fcc_dtor(zend_fcall_info_cache *fcc);

ZEND_API void zend_get_callable_zval_from_fcc(const zend_fcall_info_cache *fcc, zval *callable);

/* Moved out of zend_gc.h because zend_fcall_info_cache is an unknown type in that header */
static zend_always_inline void zend_get_gc_buffer_add_fcc(zend_get_gc_buffer *gc_buffer, zend_fcall_info_cache *fcc)
{
	ZEND_ASSERT(ZEND_FCC_INITIALIZED(*fcc));
	if (fcc->object) {
		zend_get_gc_buffer_add_obj(gc_buffer, fcc->object);
	}
	if (fcc->closure) {
		zend_get_gc_buffer_add_obj(gc_buffer, fcc->closure);
	}
}

/* Can only return FAILURE if EG(active) is false during late engine shutdown.
 * If the call or call setup throws, EG(exception) will be set and the retval
 * will be UNDEF. Otherwise, the retval will be a non-UNDEF value. */
ZEND_API zend_result zend_call_function(zend_fcall_info *fci, zend_fcall_info_cache *fci_cache);

/* Call the FCI/FCC pair while setting the call return value to the passed zval*. */
static zend_always_inline zend_result zend_call_function_with_return_value(
	zend_fcall_info *fci, zend_fcall_info_cache *fci_cache, zval *retval)
{
	ZEND_ASSERT(retval && "Use zend_call_function() directly if not providing a retval");
	fci->retval = retval;
	return zend_call_function(fci, fci_cache);
}

/* Call the provided zend_function with the given params.
 * If retval_ptr is NULL, the return value is discarded.
 * If object is NULL, this must be a free function or static call.
 * called_scope must be provided for instance and static method calls. */
ZEND_API void zend_call_known_function(
		zend_function *fn, zend_object *object, zend_class_entry *called_scope, zval *retval_ptr,
		uint32_t param_count, zval *params, HashTable *named_params);

static zend_always_inline void zend_call_known_fcc(
	zend_fcall_info_cache *fcc, zval *retval_ptr, uint32_t param_count, zval *params, HashTable *named_params)
{
	zend_function *func = fcc->function_handler;
	/* Need to copy trampolines as they get released after they are called */
	if (UNEXPECTED(func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE)) {
		func = (zend_function*) emalloc(sizeof(zend_function));
		memcpy(func, fcc->function_handler, sizeof(zend_function));
		zend_string_addref(func->op_array.function_name);
	}
	zend_call_known_function(func, fcc->object, fcc->called_scope, retval_ptr, param_count, params, named_params);
}

/* Call the provided zend_function instance method on an object. */
static zend_always_inline void zend_call_known_instance_method(
		zend_function *fn, zend_object *object, zval *retval_ptr,
		uint32_t param_count, zval *params)
{
	zend_call_known_function(fn, object, object->ce, retval_ptr, param_count, params, NULL);
}

static zend_always_inline void zend_call_known_instance_method_with_0_params(
		zend_function *fn, zend_object *object, zval *retval_ptr)
{
	zend_call_known_instance_method(fn, object, retval_ptr, 0, NULL);
}

static zend_always_inline void zend_call_known_instance_method_with_1_params(
		zend_function *fn, zend_object *object, zval *retval_ptr, zval *param)
{
	zend_call_known_instance_method(fn, object, retval_ptr, 1, param);
}

ZEND_API void zend_call_known_instance_method_with_2_params(
		zend_function *fn, zend_object *object, zval *retval_ptr, zval *param1, zval *param2);

/* Call method if it exists. Return FAILURE if method does not exist or call failed.
 * If FAILURE is returned, retval will be UNDEF. As such, destroying retval unconditionally
 * is legal. */
ZEND_API zend_result zend_call_method_if_exists(
		zend_object *object, zend_string *method_name, zval *retval,
		uint32_t param_count, zval *params);


/* Outdated and confusing FCI API */
/** Clear arguments connected with zend_fcall_info *fci
 * If free_mem is not zero then the params array gets free'd as well
 */
ZEND_API void zend_fcall_info_args_clear(zend_fcall_info *fci, bool free_mem);

/** Save current arguments from zend_fcall_info *fci
 * params array will be set to NULL
 */
ZEND_API void zend_fcall_info_args_save(zend_fcall_info *fci, uint32_t *param_count, zval **params);

/** Free arguments connected with zend_fcall_info *fci and set back saved ones.
 */
ZEND_API void zend_fcall_info_args_restore(zend_fcall_info *fci, uint32_t param_count, zval *params);

/** Set or clear the arguments in the zend_call_info struct taking care of
 * refcount. If args is NULL and arguments are set then those are cleared.
 */
ZEND_API zend_result zend_fcall_info_args(zend_fcall_info *fci, zval *args);
ZEND_API zend_result zend_fcall_info_args_ex(zend_fcall_info *fci, zend_function *func, zval *args);

/** Set arguments in the zend_fcall_info struct taking care of refcount.
 * If argc is 0 the arguments which are set will be cleared, else pass
 * a variable amount of zval** arguments.
 */
ZEND_API void zend_fcall_info_argp(zend_fcall_info *fci, uint32_t argc, zval *argv);

/** Set arguments in the zend_fcall_info struct taking care of refcount.
 * If argc is 0 the arguments which are set will be cleared, else pass
 * a variable amount of zval** arguments.
 */
ZEND_API void zend_fcall_info_argv(zend_fcall_info *fci, uint32_t argc, va_list *argv);

/** Set arguments in the zend_fcall_info struct taking care of refcount.
 * If argc is 0 the arguments which are set will be cleared, else pass
 * a variable amount of zval** arguments.
 */
ZEND_API void zend_fcall_info_argn(zend_fcall_info *fci, uint32_t argc, ...);

/** Call a function using information created by zend_fcall_info_init()/args().
 * If args is given then those replace the argument info in fci is temporarily.
 */
ZEND_API zend_result zend_fcall_info_call(zend_fcall_info *fci, zend_fcall_info_cache *fcc, zval *retval, zval *args);

END_EXTERN_C()

#endif /* ZEND_CALLABLE_H */
