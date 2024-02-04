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

#include "zend_callables.h"
#include "zend_execute.h"
#include "zend_closures.h"
#include "zend_compile.h"
#include "zend_exceptions.h"
#include "zend_objects_API.h"
#include "zend_observer.h"
#include "zend_API.h" /* For array_init() and add_next_index_*() functions */


zend_result zend_call_function(zend_fcall_info *fci, zend_fcall_info_cache *fci_cache) /* {{{ */
{
	uint32_t i;
	zend_execute_data *call;
	zend_fcall_info_cache fci_cache_local;
	zend_function *func;
	uint32_t call_info;
	void *object_or_called_scope;
	zend_class_entry *orig_fake_scope;

	ZVAL_UNDEF(fci->retval);

	if (!EG(active)) {
		return FAILURE; /* executor is already inactive */
	}

	if (EG(exception)) {
		if (fci_cache) {
			zend_release_fcall_info_cache(fci_cache);
		}
		return SUCCESS; /* we would result in an instable executor otherwise */
	}

	ZEND_ASSERT(fci->size == sizeof(zend_fcall_info));

	if (!fci_cache || !fci_cache->function_handler) {
		char *error = NULL;

		if (!fci_cache) {
			fci_cache = &fci_cache_local;
		}

		if (!zend_is_callable_ex(&fci->function_name, fci->object, 0, NULL, fci_cache, &error)) {
			ZEND_ASSERT(error && "Should have error if not callable");
			zend_string *callable_name
				= zend_get_callable_name_ex(&fci->function_name, fci->object);
			zend_throw_error(NULL, "Invalid callback %s, %s", ZSTR_VAL(callable_name), error);
			efree(error);
			zend_string_release_ex(callable_name, 0);
			return SUCCESS;
		}

		ZEND_ASSERT(!error);
	}

	func = fci_cache->function_handler;
	if ((func->common.fn_flags & ZEND_ACC_STATIC) || !fci_cache->object) {
		object_or_called_scope = fci_cache->called_scope;
		call_info = ZEND_CALL_TOP_FUNCTION | ZEND_CALL_DYNAMIC;
	} else {
		object_or_called_scope = fci_cache->object;
		call_info = ZEND_CALL_TOP_FUNCTION | ZEND_CALL_DYNAMIC | ZEND_CALL_HAS_THIS;
	}

	call = zend_vm_stack_push_call_frame(call_info,
		func, fci->param_count, object_or_called_scope);

	if (UNEXPECTED(func->common.fn_flags & ZEND_ACC_DEPRECATED)) {
		zend_deprecated_function(func);

		if (UNEXPECTED(EG(exception))) {
			zend_vm_stack_free_call_frame(call);
			return SUCCESS;
		}
	}

	for (i=0; i<fci->param_count; i++) {
		zval *param = ZEND_CALL_ARG(call, i+1);
		zval *arg = &fci->params[i];
		bool must_wrap = 0;
		if (UNEXPECTED(Z_ISUNDEF_P(arg))) {
			/* Allow forwarding undef slots. This is only used by Closure::__invoke(). */
			ZVAL_UNDEF(param);
			ZEND_ADD_CALL_FLAG(call, ZEND_CALL_MAY_HAVE_UNDEF);
			continue;
		}

		if (ARG_SHOULD_BE_SENT_BY_REF(func, i + 1)) {
			if (UNEXPECTED(!Z_ISREF_P(arg))) {
				if (!ARG_MAY_BE_SENT_BY_REF(func, i + 1)) {
					/* By-value send is not allowed -- emit a warning,
					 * and perform the call with the value wrapped in a reference. */
					zend_param_must_be_ref(func, i + 1);
					must_wrap = 1;
					if (UNEXPECTED(EG(exception))) {
						ZEND_CALL_NUM_ARGS(call) = i;
cleanup_args:
						zend_vm_stack_free_args(call);
						zend_vm_stack_free_call_frame(call);
						return SUCCESS;
					}
				}
			}
		} else {
			if (Z_ISREF_P(arg) &&
			    !(func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE)) {
				/* don't separate references for __call */
				arg = Z_REFVAL_P(arg);
			}
		}

		if (EXPECTED(!must_wrap)) {
			ZVAL_COPY(param, arg);
		} else {
			Z_TRY_ADDREF_P(arg);
			ZVAL_NEW_REF(param, arg);
		}
	}

	if (fci->named_params) {
		zend_string *name;
		zval *arg;
		uint32_t arg_num = ZEND_CALL_NUM_ARGS(call) + 1;
		bool have_named_params = 0;
		ZEND_HASH_FOREACH_STR_KEY_VAL(fci->named_params, name, arg) {
			bool must_wrap = 0;
			zval *target;
			if (name) {
				void *cache_slot[2] = {NULL, NULL};
				have_named_params = 1;
				target = zend_handle_named_arg(&call, name, &arg_num, cache_slot);
				if (!target) {
					goto cleanup_args;
				}
			} else {
				if (have_named_params) {
					zend_throw_error(NULL,
						"Cannot use positional argument after named argument");
					goto cleanup_args;
				}

				zend_vm_stack_extend_call_frame(&call, arg_num - 1, 1);
				target = ZEND_CALL_ARG(call, arg_num);
			}

			if (ARG_SHOULD_BE_SENT_BY_REF(func, arg_num)) {
				if (UNEXPECTED(!Z_ISREF_P(arg))) {
					if (!ARG_MAY_BE_SENT_BY_REF(func, arg_num)) {
						/* By-value send is not allowed -- emit a warning,
						 * and perform the call with the value wrapped in a reference. */
						zend_param_must_be_ref(func, arg_num);
						must_wrap = 1;
						if (UNEXPECTED(EG(exception))) {
							goto cleanup_args;
						}
					}
				}
			} else {
				if (Z_ISREF_P(arg) &&
					!(func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE)) {
					/* don't separate references for __call */
					arg = Z_REFVAL_P(arg);
				}
			}

			if (EXPECTED(!must_wrap)) {
				ZVAL_COPY(target, arg);
			} else {
				Z_TRY_ADDREF_P(arg);
				ZVAL_NEW_REF(target, arg);
			}
			if (!name) {
				ZEND_CALL_NUM_ARGS(call)++;
				arg_num++;
			}
		} ZEND_HASH_FOREACH_END();
	}

	if (UNEXPECTED(ZEND_CALL_INFO(call) & ZEND_CALL_MAY_HAVE_UNDEF)) {
		/* zend_handle_undef_args assumes prev_execute_data is initialized. */
		call->prev_execute_data = NULL;
		if (zend_handle_undef_args(call) == FAILURE) {
			zend_vm_stack_free_args(call);
			zend_vm_stack_free_call_frame(call);
			return SUCCESS;
		}
	}

	if (UNEXPECTED(func->op_array.fn_flags & ZEND_ACC_CLOSURE)) {
		uint32_t call_info;

		GC_ADDREF(ZEND_CLOSURE_OBJECT(func));
		call_info = ZEND_CALL_CLOSURE;
		if (func->common.fn_flags & ZEND_ACC_FAKE_CLOSURE) {
			call_info |= ZEND_CALL_FAKE_CLOSURE;
		}
		ZEND_ADD_CALL_FLAG(call, call_info);
	}

	if (func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) {
		fci_cache->function_handler = NULL;
	}

	orig_fake_scope = EG(fake_scope);
	EG(fake_scope) = NULL;
	if (func->type == ZEND_USER_FUNCTION) {
		uint32_t orig_jit_trace_num = EG(jit_trace_num);

		zend_init_func_execute_data(call, &func->op_array, fci->retval);
		ZEND_OBSERVER_FCALL_BEGIN(call);
		zend_execute_ex(call);
		EG(jit_trace_num) = orig_jit_trace_num;
	} else {
		ZEND_ASSERT(func->type == ZEND_INTERNAL_FUNCTION);
		ZVAL_NULL(fci->retval);
		call->prev_execute_data = EG(current_execute_data);
		EG(current_execute_data) = call;
#if ZEND_DEBUG
		bool should_throw = zend_internal_call_should_throw(func, call);
#endif
		ZEND_OBSERVER_FCALL_BEGIN(call);
		if (EXPECTED(zend_execute_internal == NULL)) {
			/* saves one function call if zend_execute_internal is not used */
			func->internal_function.handler(call, fci->retval);
		} else {
			zend_execute_internal(call, fci->retval);
		}

#if ZEND_DEBUG
		if (!EG(exception) && call->func) {
			if (should_throw) {
				zend_internal_call_arginfo_violation(call->func);
			}
			ZEND_ASSERT(!(call->func->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE) ||
				zend_verify_internal_return_type(call->func, fci->retval));
			ZEND_ASSERT((call->func->common.fn_flags & ZEND_ACC_RETURN_REFERENCE)
				? Z_ISREF_P(fci->retval) : !Z_ISREF_P(fci->retval));
		}
#endif
		ZEND_OBSERVER_FCALL_END(call, fci->retval);
		EG(current_execute_data) = call->prev_execute_data;
		zend_vm_stack_free_args(call);
		if (UNEXPECTED(ZEND_CALL_INFO(call) & ZEND_CALL_HAS_EXTRA_NAMED_PARAMS)) {
			zend_array_release(call->extra_named_params);
		}

		if (EG(exception)) {
			zval_ptr_dtor(fci->retval);
			ZVAL_UNDEF(fci->retval);
		}

		/* This flag is regularly checked while running user functions, but not internal
		 * So see whether interrupt flag was set while the function was running... */
		if (zend_atomic_bool_exchange_ex(&EG(vm_interrupt), false)) {
			if (zend_atomic_bool_load_ex(&EG(timed_out))) {
				zend_timeout();
			} else if (zend_interrupt_function) {
				zend_interrupt_function(EG(current_execute_data));
			}
		}

		if (UNEXPECTED(ZEND_CALL_INFO(call) & ZEND_CALL_RELEASE_THIS)) {
			OBJ_RELEASE(Z_OBJ(call->This));
		}
	}
	EG(fake_scope) = orig_fake_scope;

	zend_vm_stack_free_call_frame(call);

	if (UNEXPECTED(EG(exception))) {
		if (UNEXPECTED(!EG(current_execute_data))) {
			zend_throw_exception_internal(NULL);
		} else if (EG(current_execute_data)->func &&
		           ZEND_USER_CODE(EG(current_execute_data)->func->common.type)) {
			zend_rethrow_exception(EG(current_execute_data));
		}
	}

	return SUCCESS;
}
/* }}} */

ZEND_API void zend_call_known_function(
		zend_function *fn, zend_object *object, zend_class_entry *called_scope, zval *retval_ptr,
		uint32_t param_count, zval *params, HashTable *named_params)
{
	zval retval;
	zend_fcall_info fci;
	zend_fcall_info_cache fcic;

	ZEND_ASSERT(fn && "zend_function must be passed!");

	fci.size = sizeof(fci);
	fci.object = object;
	fci.retval = retval_ptr ? retval_ptr : &retval;
	fci.param_count = param_count;
	fci.params = params;
	fci.named_params = named_params;
	ZVAL_UNDEF(&fci.function_name); /* Unused */

	fcic.function_handler = fn;
	fcic.object = object;
	fcic.called_scope = called_scope;

	zend_result result = zend_call_function(&fci, &fcic);
	if (UNEXPECTED(result == FAILURE)) {
		if (!EG(exception)) {
			zend_error_noreturn(E_CORE_ERROR, "Couldn't execute method %s%s%s",
				fn->common.scope ? ZSTR_VAL(fn->common.scope->name) : "",
				fn->common.scope ? "::" : "", ZSTR_VAL(fn->common.function_name));
		}
	}

	if (!retval_ptr) {
		zval_ptr_dtor(&retval);
	}
}

zend_result _call_user_function_impl(zval *object, zval *function_name, zval *retval_ptr, uint32_t param_count, zval params[], HashTable *named_params)
{
	zend_fcall_info fci;

	fci.size = sizeof(fci);
	if (object) {
		ZEND_ASSERT(Z_TYPE_P(object) == IS_OBJECT);
		fci.object = Z_OBJ_P(object);
	} else {
		fci.object = NULL;
	}
	ZVAL_COPY_VALUE(&fci.function_name, function_name);
	fci.retval = retval_ptr;
	fci.param_count = param_count;
	fci.params = params;
	fci.named_params = named_params;

	return zend_call_function(&fci, NULL);
}

ZEND_API void zend_release_fcall_info_cache(zend_fcall_info_cache *fcc) {
	if (fcc->function_handler &&
		(fcc->function_handler->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE)) {
		if (fcc->function_handler->common.function_name) {
			zend_string_release_ex(fcc->function_handler->common.function_name, 0);
		}
		zend_free_trampoline(fcc->function_handler);
		fcc->function_handler = NULL;
	}
}

ZEND_API void zend_fcc_dtor(zend_fcall_info_cache *fcc)
{
	ZEND_ASSERT(fcc->function_handler);
	if (fcc->object) {
		OBJ_RELEASE(fcc->object);
	}
	/* Need to free potential trampoline (__call/__callStatic) copied function handler before releasing the closure */
	zend_release_fcall_info_cache(fcc);
	if (fcc->closure) {
		OBJ_RELEASE(fcc->closure);
	}
	memcpy(fcc, &empty_fcall_info_cache, sizeof(zend_fcall_info_cache));
}

/* Check a zval is callable */
static zend_always_inline zend_class_entry *get_scope(zend_execute_data *frame)
{
	return frame && frame->func ? frame->func->common.scope : NULL;
}


static bool zend_is_callable_check_class(zend_string *name, zend_class_entry *scope, zend_execute_data *frame, zend_fcall_info_cache *fcc, bool *strict_class, char **error, bool suppress_deprecation) /* {{{ */
{
	bool ret = 0;
	zend_class_entry *ce;
	size_t name_len = ZSTR_LEN(name);
	zend_string *lcname;
	ALLOCA_FLAG(use_heap);

	ZSTR_ALLOCA_ALLOC(lcname, name_len, use_heap);
	zend_str_tolower_copy(ZSTR_VAL(lcname), ZSTR_VAL(name), name_len);

	*strict_class = 0;
	if (zend_string_equals_literal(lcname, "self")) {
		if (!scope) {
			if (error) *error = estrdup("cannot access \"self\" when no class scope is active");
		} else {
			if (!suppress_deprecation) {
				zend_error(E_DEPRECATED, "Use of \"self\" in callables is deprecated");
			}
			fcc->called_scope = zend_get_called_scope(frame);
			if (!fcc->called_scope || !instanceof_function(fcc->called_scope, scope)) {
				fcc->called_scope = scope;
			}
			fcc->calling_scope = scope;
			if (!fcc->object) {
				fcc->object = zend_get_this_object(frame);
			}
			ret = 1;
		}
	} else if (zend_string_equals_literal(lcname, "parent")) {
		if (!scope) {
			if (error) *error = estrdup("cannot access \"parent\" when no class scope is active");
		} else if (!scope->parent) {
			if (error) *error = estrdup("cannot access \"parent\" when current class scope has no parent");
		} else {
			if (!suppress_deprecation) {
				zend_error(E_DEPRECATED, "Use of \"parent\" in callables is deprecated");
			}
			fcc->called_scope = zend_get_called_scope(frame);
			if (!fcc->called_scope || !instanceof_function(fcc->called_scope, scope->parent)) {
				fcc->called_scope = scope->parent;
			}
			fcc->calling_scope = scope->parent;
			if (!fcc->object) {
				fcc->object = zend_get_this_object(frame);
			}
			*strict_class = 1;
			ret = 1;
		}
	} else if (zend_string_equals(lcname, ZSTR_KNOWN(ZEND_STR_STATIC))) {
		zend_class_entry *called_scope = zend_get_called_scope(frame);

		if (!called_scope) {
			if (error) *error = estrdup("cannot access \"static\" when no class scope is active");
		} else {
			if (!suppress_deprecation) {
				zend_error(E_DEPRECATED, "Use of \"static\" in callables is deprecated");
			}
			fcc->called_scope = called_scope;
			fcc->calling_scope = called_scope;
			if (!fcc->object) {
				fcc->object = zend_get_this_object(frame);
			}
			*strict_class = 1;
			ret = 1;
		}
	} else if ((ce = zend_lookup_class(name)) != NULL) {
		zend_class_entry *scope = get_scope(frame);
		fcc->calling_scope = ce;
		if (scope && !fcc->object) {
			zend_object *object = zend_get_this_object(frame);

			if (object &&
			    instanceof_function(object->ce, scope) &&
			    instanceof_function(scope, ce)) {
				fcc->object = object;
				fcc->called_scope = object->ce;
			} else {
				fcc->called_scope = ce;
			}
		} else {
			fcc->called_scope = fcc->object ? fcc->object->ce : ce;
		}
		*strict_class = 1;
		ret = 1;
	} else {
		if (error) zend_spprintf(error, 0, "class \"%.*s\" not found", (int)name_len, ZSTR_VAL(name));
	}
	ZSTR_ALLOCA_FREE(lcname, use_heap);
	return ret;
}
/* }}} */

static zend_always_inline bool zend_is_callable_check_func(zval *callable, zend_execute_data *frame, zend_fcall_info_cache *fcc, bool strict_class, char **error, bool suppress_deprecation) /* {{{ */
{
	zend_class_entry *ce_org = fcc->calling_scope;
	bool retval = 0;
	zend_string *mname, *cname;
	zend_string *lmname;
	const char *colon;
	size_t clen;
	HashTable *ftable;
	int call_via_handler = 0;
	zend_class_entry *scope;
	zval *zv;
	ALLOCA_FLAG(use_heap)

	fcc->calling_scope = NULL;

	if (!ce_org) {
		zend_function *func;
		zend_string *lmname;

		/* Check if function with given name exists.
		 * This may be a compound name that includes namespace name */
		if (UNEXPECTED(Z_STRVAL_P(callable)[0] == '\\')) {
			/* Skip leading \ */
			ZSTR_ALLOCA_ALLOC(lmname, Z_STRLEN_P(callable) - 1, use_heap);
			zend_str_tolower_copy(ZSTR_VAL(lmname), Z_STRVAL_P(callable) + 1, Z_STRLEN_P(callable) - 1);
			func = zend_fetch_function(lmname);
			ZSTR_ALLOCA_FREE(lmname, use_heap);
		} else {
			lmname = Z_STR_P(callable);
			func = zend_fetch_function(lmname);
			if (!func) {
				ZSTR_ALLOCA_ALLOC(lmname, Z_STRLEN_P(callable), use_heap);
				zend_str_tolower_copy(ZSTR_VAL(lmname), Z_STRVAL_P(callable), Z_STRLEN_P(callable));
				func = zend_fetch_function(lmname);
				ZSTR_ALLOCA_FREE(lmname, use_heap);
			}
		}
		if (EXPECTED(func != NULL)) {
			fcc->function_handler = func;
			return 1;
		}
	}

	/* Split name into class/namespace and method/function names */
	if ((colon = zend_memrchr(Z_STRVAL_P(callable), ':', Z_STRLEN_P(callable))) != NULL &&
		colon > Z_STRVAL_P(callable) &&
		*(colon-1) == ':'
	) {
		size_t mlen;

		colon--;
		clen = colon - Z_STRVAL_P(callable);
		mlen = Z_STRLEN_P(callable) - clen - 2;

		if (colon == Z_STRVAL_P(callable)) {
			if (error) *error = estrdup("invalid function name");
			return 0;
		}

		/* This is a compound name.
		 * Try to fetch class and then find static method. */
		if (ce_org) {
			scope = ce_org;
		} else {
			scope = get_scope(frame);
		}

		cname = zend_string_init_interned(Z_STRVAL_P(callable), clen, 0);
		if (ZSTR_HAS_CE_CACHE(cname) && ZSTR_GET_CE_CACHE(cname)) {
			fcc->calling_scope = ZSTR_GET_CE_CACHE(cname);
			if (scope && !fcc->object) {
				zend_object *object = zend_get_this_object(frame);

				if (object &&
				    instanceof_function(object->ce, scope) &&
				    instanceof_function(scope, fcc->calling_scope)) {
					fcc->object = object;
					fcc->called_scope = object->ce;
				} else {
					fcc->called_scope = fcc->calling_scope;
				}
			} else {
				fcc->called_scope = fcc->object ? fcc->object->ce : fcc->calling_scope;
			}
			strict_class = 1;
		} else if (!zend_is_callable_check_class(cname, scope, frame, fcc, &strict_class, error, suppress_deprecation || ce_org != NULL)) {
			zend_string_release_ex(cname, 0);
			return 0;
		}
		zend_string_release_ex(cname, 0);

		ftable = &fcc->calling_scope->function_table;
		if (ce_org && !instanceof_function(ce_org, fcc->calling_scope)) {
			if (error) zend_spprintf(error, 0, "class %s is not a subclass of %s", ZSTR_VAL(ce_org->name), ZSTR_VAL(fcc->calling_scope->name));
			return 0;
		}
		if (ce_org && !suppress_deprecation) {
			zend_error(E_DEPRECATED,
				"Callables of the form [\"%s\", \"%s\"] are deprecated",
				ZSTR_VAL(ce_org->name), Z_STRVAL_P(callable));
		}
		mname = zend_string_init(Z_STRVAL_P(callable) + clen + 2, mlen, 0);
	} else if (ce_org) {
		/* Try to fetch find static method of given class. */
		mname = Z_STR_P(callable);
		zend_string_addref(mname);
		ftable = &ce_org->function_table;
		fcc->calling_scope = ce_org;
	} else {
		/* We already checked for plain function before. */
		if (error) {
			zend_spprintf(error, 0, "function \"%s\" not found or invalid function name", Z_STRVAL_P(callable));
		}
		return 0;
	}

	lmname = zend_string_tolower(mname);
	if (strict_class &&
	    fcc->calling_scope &&
		zend_string_equals_literal(lmname, ZEND_CONSTRUCTOR_FUNC_NAME)) {
		fcc->function_handler = fcc->calling_scope->constructor;
		if (fcc->function_handler) {
			retval = 1;
		}
	} else if ((zv = zend_hash_find(ftable, lmname)) != NULL) {
		fcc->function_handler = Z_PTR_P(zv);
		retval = 1;
		if ((fcc->function_handler->op_array.fn_flags & ZEND_ACC_CHANGED) &&
		    !strict_class) {
			scope = get_scope(frame);
			if (scope &&
			    instanceof_function(fcc->function_handler->common.scope, scope)) {

				zv = zend_hash_find(&scope->function_table, lmname);
				if (zv != NULL) {
					zend_function *priv_fbc = Z_PTR_P(zv);

					if ((priv_fbc->common.fn_flags & ZEND_ACC_PRIVATE)
					 && priv_fbc->common.scope == scope) {
						fcc->function_handler = priv_fbc;
					}
				}
			}
		}
		if (!(fcc->function_handler->common.fn_flags & ZEND_ACC_PUBLIC) &&
		    (fcc->calling_scope &&
		     ((fcc->object && fcc->calling_scope->__call) ||
		      (!fcc->object && fcc->calling_scope->__callstatic)))) {
			scope = get_scope(frame);
			if (fcc->function_handler->common.scope != scope) {
				if ((fcc->function_handler->common.fn_flags & ZEND_ACC_PRIVATE)
				 || !zend_check_protected(zend_get_function_root_class(fcc->function_handler), scope)) {
					retval = 0;
					fcc->function_handler = NULL;
					goto get_function_via_handler;
				}
			}
		}
	} else {
get_function_via_handler:
		if (fcc->object && fcc->calling_scope == ce_org) {
			if (strict_class && ce_org->__call) {
				fcc->function_handler = zend_get_call_trampoline_func(ce_org, mname, 0);
				call_via_handler = 1;
				retval = 1;
			} else {
				fcc->function_handler = fcc->object->handlers->get_method(&fcc->object, mname, NULL);
				if (fcc->function_handler) {
					if (strict_class &&
					    (!fcc->function_handler->common.scope ||
					     !instanceof_function(ce_org, fcc->function_handler->common.scope))) {
						zend_release_fcall_info_cache(fcc);
					} else {
						retval = 1;
						call_via_handler = (fcc->function_handler->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) != 0;
					}
				}
			}
		} else if (fcc->calling_scope) {
			if (fcc->calling_scope->get_static_method) {
				fcc->function_handler = fcc->calling_scope->get_static_method(fcc->calling_scope, mname);
			} else {
				fcc->function_handler = zend_std_get_static_method(fcc->calling_scope, mname, NULL);
			}
			if (fcc->function_handler) {
				retval = 1;
				call_via_handler = (fcc->function_handler->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) != 0;
				if (call_via_handler && !fcc->object) {
					zend_object *object = zend_get_this_object(frame);
					if (object &&
					    instanceof_function(object->ce, fcc->calling_scope)) {
						fcc->object = object;
					}
				}
			}
		}
	}

	if (retval) {
		if (fcc->calling_scope && !call_via_handler) {
			if (fcc->function_handler->common.fn_flags & ZEND_ACC_ABSTRACT) {
				retval = 0;
				if (error) {
					zend_spprintf(error, 0, "cannot call abstract method %s::%s()", ZSTR_VAL(fcc->calling_scope->name), ZSTR_VAL(fcc->function_handler->common.function_name));
				}
			} else if (!fcc->object && !(fcc->function_handler->common.fn_flags & ZEND_ACC_STATIC)) {
				retval = 0;
				if (error) {
					zend_spprintf(error, 0, "non-static method %s::%s() cannot be called statically", ZSTR_VAL(fcc->calling_scope->name), ZSTR_VAL(fcc->function_handler->common.function_name));
				}
			}
			if (retval
			 && !(fcc->function_handler->common.fn_flags & ZEND_ACC_PUBLIC)) {
				scope = get_scope(frame);
				if (fcc->function_handler->common.scope != scope) {
					if ((fcc->function_handler->common.fn_flags & ZEND_ACC_PRIVATE)
					 || (!zend_check_protected(zend_get_function_root_class(fcc->function_handler), scope))) {
						if (error) {
							if (*error) {
								efree(*error);
							}
							zend_spprintf(error, 0, "cannot access %s method %s::%s()", zend_visibility_string(fcc->function_handler->common.fn_flags), ZSTR_VAL(fcc->calling_scope->name), ZSTR_VAL(fcc->function_handler->common.function_name));
						}
						retval = 0;
					}
				}
			}
		}
	} else if (error) {
		if (fcc->calling_scope) {
			zend_spprintf(error, 0, "class %s does not have a method \"%s\"", ZSTR_VAL(fcc->calling_scope->name), ZSTR_VAL(mname));
		} else {
			zend_spprintf(error, 0, "function %s() does not exist", ZSTR_VAL(mname));
		}
	}
	zend_string_release_ex(lmname, 0);
	zend_string_release_ex(mname, 0);

	if (fcc->object) {
		fcc->called_scope = fcc->object->ce;
		if (fcc->function_handler
		 && (fcc->function_handler->common.fn_flags & ZEND_ACC_STATIC)) {
			fcc->object = NULL;
		}
	}
	return retval;
}
/* }}} */

ZEND_API zend_string *zend_get_callable_name_ex(zval *callable, zend_object *object) /* {{{ */
{
try_again:
	switch (Z_TYPE_P(callable)) {
		case IS_STRING:
			if (object) {
				return zend_create_member_string(object->ce->name, Z_STR_P(callable));
			}
			return zend_string_copy(Z_STR_P(callable));

		case IS_ARRAY:
		{
			zval *method = NULL;
			zval *obj = NULL;

			if (zend_hash_num_elements(Z_ARRVAL_P(callable)) == 2) {
				obj = zend_hash_index_find_deref(Z_ARRVAL_P(callable), 0);
				method = zend_hash_index_find_deref(Z_ARRVAL_P(callable), 1);
			}

			if (obj == NULL || method == NULL || Z_TYPE_P(method) != IS_STRING) {
				return ZSTR_KNOWN(ZEND_STR_ARRAY_CAPITALIZED);
			}

			if (Z_TYPE_P(obj) == IS_STRING) {
				return zend_create_member_string(Z_STR_P(obj), Z_STR_P(method));
			} else if (Z_TYPE_P(obj) == IS_OBJECT) {
				return zend_create_member_string(Z_OBJCE_P(obj)->name, Z_STR_P(method));
			} else {
				return ZSTR_KNOWN(ZEND_STR_ARRAY_CAPITALIZED);
			}
		}
		case IS_OBJECT:
		{
			zend_class_entry *ce = Z_OBJCE_P(callable);
			return zend_string_concat2(
				ZSTR_VAL(ce->name), ZSTR_LEN(ce->name),
				"::__invoke", sizeof("::__invoke") - 1);
		}
		case IS_REFERENCE:
			callable = Z_REFVAL_P(callable);
			goto try_again;
		default:
			return zval_get_string_func(callable);
	}
}
/* }}} */

ZEND_API zend_string *zend_get_callable_name(zval *callable) /* {{{ */
{
	return zend_get_callable_name_ex(callable, NULL);
}
/* }}} */

ZEND_API bool zend_is_callable_at_frame(
		zval *callable, zend_object *object, zend_execute_data *frame,
		uint32_t check_flags, zend_fcall_info_cache *fcc, char **error) /* {{{ */
{
	bool ret;
	zend_fcall_info_cache fcc_local;
	bool strict_class = 0;

	if (fcc == NULL) {
		fcc = &fcc_local;
	}
	if (error) {
		*error = NULL;
	}

	fcc->calling_scope = NULL;
	fcc->called_scope = NULL;
	fcc->function_handler = NULL;
	fcc->object = NULL;
	fcc->closure = NULL;

again:
	switch (Z_TYPE_P(callable)) {
		case IS_STRING:
			if (object) {
				fcc->object = object;
				fcc->calling_scope = object->ce;
			}

			if (check_flags & IS_CALLABLE_CHECK_SYNTAX_ONLY) {
				fcc->called_scope = fcc->calling_scope;
				return 1;
			}

check_func:
			ret = zend_is_callable_check_func(callable, frame, fcc, strict_class, error, check_flags & IS_CALLABLE_SUPPRESS_DEPRECATIONS);
			if (fcc == &fcc_local) {
				zend_release_fcall_info_cache(fcc);
			}
			return ret;

		case IS_ARRAY:
			{
				if (zend_hash_num_elements(Z_ARRVAL_P(callable)) != 2) {
					if (error) *error = estrdup("array callback must have exactly two members");
					return 0;
				}

				zval *obj = zend_hash_index_find(Z_ARRVAL_P(callable), 0);
				zval *method = zend_hash_index_find(Z_ARRVAL_P(callable), 1);
				if (!obj || !method) {
					if (error) *error = estrdup("array callback has to contain indices 0 and 1");
					return 0;
				}

				ZVAL_DEREF(obj);
				if (Z_TYPE_P(obj) != IS_STRING && Z_TYPE_P(obj) != IS_OBJECT) {
					if (error) *error = estrdup("first array member is not a valid class name or object");
					return 0;
				}

				ZVAL_DEREF(method);
				if (Z_TYPE_P(method) != IS_STRING) {
					if (error) *error = estrdup("second array member is not a valid method");
					return 0;
				}

				if (Z_TYPE_P(obj) == IS_STRING) {
					if (check_flags & IS_CALLABLE_CHECK_SYNTAX_ONLY) {
						return 1;
					}

					if (!zend_is_callable_check_class(Z_STR_P(obj), get_scope(frame), frame, fcc, &strict_class, error, check_flags & IS_CALLABLE_SUPPRESS_DEPRECATIONS)) {
						return 0;
					}
				} else {
					ZEND_ASSERT(Z_TYPE_P(obj) == IS_OBJECT);
					fcc->calling_scope = Z_OBJCE_P(obj); /* TBFixed: what if it's overloaded? */
					fcc->object = Z_OBJ_P(obj);

					if (check_flags & IS_CALLABLE_CHECK_SYNTAX_ONLY) {
						fcc->called_scope = fcc->calling_scope;
						return 1;
					}
				}

				callable = method;
				goto check_func;
			}
			return 0;
		case IS_OBJECT:
			if (Z_OBJ_HANDLER_P(callable, get_closure) && Z_OBJ_HANDLER_P(callable, get_closure)(Z_OBJ_P(callable), &fcc->calling_scope, &fcc->function_handler, &fcc->object, 1) == SUCCESS) {
				fcc->called_scope = fcc->calling_scope;
				fcc->closure = Z_OBJ_P(callable);
				if (fcc == &fcc_local) {
					zend_release_fcall_info_cache(fcc);
				}
				return 1;
			}
			if (error) *error = estrdup("no array or string given");
			return 0;
		case IS_REFERENCE:
			callable = Z_REFVAL_P(callable);
			goto again;
		default:
			if (error) *error = estrdup("no array or string given");
			return 0;
	}
}
/* }}} */

ZEND_API bool zend_is_callable_ex(zval *callable, zend_object *object, uint32_t check_flags, zend_string **callable_name, zend_fcall_info_cache *fcc, char **error) /* {{{ */
{
	/* Determine callability at the first parent user frame. */
	zend_execute_data *frame = EG(current_execute_data);
	while (frame && (!frame->func || !ZEND_USER_CODE(frame->func->type))) {
		frame = frame->prev_execute_data;
	}

	bool ret = zend_is_callable_at_frame(callable, object, frame, check_flags, fcc, error);
	if (callable_name) {
		*callable_name = zend_get_callable_name_ex(callable, object);
	}
	return ret;
}

ZEND_API bool zend_is_callable(zval *callable, uint32_t check_flags, zend_string **callable_name) /* {{{ */
{
	return zend_is_callable_ex(callable, NULL, check_flags, callable_name, NULL, NULL);
}
/* }}} */

ZEND_API bool zend_make_callable(zval *callable, zend_string **callable_name) /* {{{ */
{
	zend_fcall_info_cache fcc;

	if (zend_is_callable_ex(callable, NULL, IS_CALLABLE_SUPPRESS_DEPRECATIONS, callable_name, &fcc, NULL)) {
		if (Z_TYPE_P(callable) == IS_STRING && fcc.calling_scope) {
			zval_ptr_dtor_str(callable);
			array_init(callable);
			add_next_index_str(callable, zend_string_copy(fcc.calling_scope->name));
			add_next_index_str(callable, zend_string_copy(fcc.function_handler->common.function_name));
		}
		zend_release_fcall_info_cache(&fcc);
		return 1;
	}
	return 0;
}
/* }}} */

ZEND_API zend_result zend_fcall_info_init(zval *callable, uint32_t check_flags, zend_fcall_info *fci, zend_fcall_info_cache *fcc, zend_string **callable_name, char **error) /* {{{ */
{
	if (!zend_is_callable_ex(callable, NULL, check_flags, callable_name, fcc, error)) {
		return FAILURE;
	}

	fci->size = sizeof(*fci);
	fci->object = fcc->object;
	ZVAL_COPY_VALUE(&fci->function_name, callable);
	fci->retval = NULL;
	fci->param_count = 0;
	fci->params = NULL;
	fci->named_params = NULL;

	return SUCCESS;
}
/* }}} */

ZEND_API void zend_get_callable_zval_from_fcc(const zend_fcall_info_cache *fcc, zval *callable)
{
	if (fcc->closure) {
		ZVAL_OBJ_COPY(callable, fcc->closure);
	} else if (fcc->function_handler->common.scope) {
		array_init(callable);
		if (fcc->object) {
			GC_ADDREF(fcc->object);
			add_next_index_object(callable, fcc->object);
		} else {
			add_next_index_str(callable, zend_string_copy(fcc->calling_scope->name));
		}
		add_next_index_str(callable, zend_string_copy(fcc->function_handler->common.function_name));
	} else {
		ZVAL_STR_COPY(callable, fcc->function_handler->common.function_name);
	}
}

ZEND_API void zend_call_known_instance_method_with_2_params(
		zend_function *fn, zend_object *object, zval *retval_ptr, zval *param1, zval *param2)
{
	zval params[2];
	ZVAL_COPY_VALUE(&params[0], param1);
	ZVAL_COPY_VALUE(&params[1], param2);
	zend_call_known_instance_method(fn, object, retval_ptr, 2, params);
}

ZEND_API zend_result zend_call_method_if_exists(
		zend_object *object, zend_string *method_name, zval *retval,
		uint32_t param_count, zval *params)
{
	zend_fcall_info fci;
	fci.size = sizeof(zend_fcall_info);
	fci.object = object;
	ZVAL_STR(&fci.function_name, method_name);
	fci.retval = retval;
	fci.param_count = param_count;
	fci.params = params;
	fci.named_params = NULL;

	zend_fcall_info_cache fcc;
	if (!zend_is_callable_ex(&fci.function_name, fci.object, IS_CALLABLE_SUPPRESS_DEPRECATIONS, NULL, &fcc, NULL)) {
		ZVAL_UNDEF(retval);
		return FAILURE;
	}

	return zend_call_function(&fci, &fcc);
}

/* Outdated and confusing FCI API */
ZEND_API void zend_fcall_info_args_clear(zend_fcall_info *fci, bool free_mem) /* {{{ */
{
	if (fci->params) {
		zval *p = fci->params;
		zval *end = p + fci->param_count;

		while (p != end) {
			i_zval_ptr_dtor(p);
			p++;
		}
		if (free_mem) {
			efree(fci->params);
			fci->params = NULL;
		}
	}
	fci->param_count = 0;
}
/* }}} */

ZEND_API void zend_fcall_info_args_save(zend_fcall_info *fci, uint32_t *param_count, zval **params) /* {{{ */
{
	*param_count = fci->param_count;
	*params = fci->params;
	fci->param_count = 0;
	fci->params = NULL;
}
/* }}} */

ZEND_API void zend_fcall_info_args_restore(zend_fcall_info *fci, uint32_t param_count, zval *params) /* {{{ */
{
	zend_fcall_info_args_clear(fci, 1);
	fci->param_count = param_count;
	fci->params = params;
}
/* }}} */

ZEND_API zend_result zend_fcall_info_args_ex(zend_fcall_info *fci, zend_function *func, zval *args) /* {{{ */
{
	zval *arg, *params;
	uint32_t n = 1;

	zend_fcall_info_args_clear(fci, !args);

	if (!args) {
		return SUCCESS;
	}

	if (Z_TYPE_P(args) != IS_ARRAY) {
		return FAILURE;
	}

	fci->param_count = zend_hash_num_elements(Z_ARRVAL_P(args));
	fci->params = params = (zval *) erealloc(fci->params, fci->param_count * sizeof(zval));

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(args), arg) {
		if (func && !Z_ISREF_P(arg) && ARG_SHOULD_BE_SENT_BY_REF(func, n)) {
			ZVAL_NEW_REF(params, arg);
			Z_TRY_ADDREF_P(arg);
		} else {
			ZVAL_COPY(params, arg);
		}
		params++;
		n++;
	} ZEND_HASH_FOREACH_END();

	return SUCCESS;
}
/* }}} */

ZEND_API zend_result zend_fcall_info_args(zend_fcall_info *fci, zval *args) /* {{{ */
{
	return zend_fcall_info_args_ex(fci, NULL, args);
}
/* }}} */

ZEND_API void zend_fcall_info_argp(zend_fcall_info *fci, uint32_t argc, zval *argv) /* {{{ */
{
	zend_fcall_info_args_clear(fci, !argc);

	if (argc) {
		fci->param_count = argc;
		fci->params = (zval *) erealloc(fci->params, fci->param_count * sizeof(zval));

		for (uint32_t i = 0; i < argc; ++i) {
			ZVAL_COPY(&fci->params[i], &argv[i]);
		}
	}
}
/* }}} */

ZEND_API void zend_fcall_info_argv(zend_fcall_info *fci, uint32_t argc, va_list *argv) /* {{{ */
{
	zend_fcall_info_args_clear(fci, !argc);

	if (argc) {
		zval *arg;
		fci->param_count = argc;
		fci->params = (zval *) erealloc(fci->params, fci->param_count * sizeof(zval));

		for (uint32_t i = 0; i < argc; ++i) {
			arg = va_arg(*argv, zval *);
			ZVAL_COPY(&fci->params[i], arg);
		}
	}
}
/* }}} */

ZEND_API void zend_fcall_info_argn(zend_fcall_info *fci, uint32_t argc, ...) /* {{{ */
{
	va_list argv;

	va_start(argv, argc);
	zend_fcall_info_argv(fci, argc, &argv);
	va_end(argv);
}
/* }}} */

ZEND_API zend_result zend_fcall_info_call(zend_fcall_info *fci, zend_fcall_info_cache *fcc, zval *retval_ptr, zval *args) /* {{{ */
{
	zval retval, *org_params = NULL;
	uint32_t org_count = 0;
	zend_result result;

	fci->retval = retval_ptr ? retval_ptr : &retval;
	if (args) {
		zend_fcall_info_args_save(fci, &org_count, &org_params);
		zend_fcall_info_args(fci, args);
	}
	result = zend_call_function(fci, fcc);

	if (!retval_ptr && Z_TYPE(retval) != IS_UNDEF) {
		zval_ptr_dtor(&retval);
	}
	if (args) {
		zend_fcall_info_args_restore(fci, org_count, org_params);
	}
	return result;
}
/* }}} */
