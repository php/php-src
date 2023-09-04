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
   | Authors: Christian Seiler <chris_se@gmx.net>                         |
   |          Dmitry Stogov <dmitry@php.net>                              |
   |          Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_API.h"
#include "zend_closures.h"
#include "zend_exceptions.h"
#include "zend_interfaces.h"
#include "zend_objects.h"
#include "zend_objects_API.h"
#include "zend_globals.h"
#include "zend_closures_arginfo.h"

typedef struct _zend_closure {
	zend_object       std;
	zend_function     func;
	zval              this_ptr;
	zend_class_entry *called_scope;
	zif_handler       orig_internal_handler;
} zend_closure;

/* non-static since it needs to be referenced */
ZEND_API zend_class_entry *zend_ce_closure;
static zend_object_handlers closure_handlers;

ZEND_METHOD(Closure, __invoke) /* {{{ */
{
	zend_function *func = EX(func);
	zval *args;
	uint32_t num_args;
	HashTable *named_args;

	ZEND_PARSE_PARAMETERS_START(0, -1)
		Z_PARAM_VARIADIC_WITH_NAMED(args, num_args, named_args)
	ZEND_PARSE_PARAMETERS_END();

	if (call_user_function_named(CG(function_table), NULL, ZEND_THIS, return_value, num_args, args, named_args) == FAILURE) {
		RETVAL_FALSE;
	}

	/* destruct the function also, then - we have allocated it in get_method */
	zend_string_release_ex(func->internal_function.function_name, 0);
	efree(func);
#if ZEND_DEBUG
	execute_data->func = NULL;
#endif
}
/* }}} */

static bool zend_valid_closure_binding(
		zend_closure *closure, zval *newthis, zend_class_entry *scope) /* {{{ */
{
	zend_function *func = &closure->func;
	bool is_fake_closure = (func->common.fn_flags & ZEND_ACC_FAKE_CLOSURE) != 0;
	if (newthis) {
		if (func->common.fn_flags & ZEND_ACC_STATIC) {
			zend_error(E_WARNING, "Cannot bind an instance to a static closure");
			return 0;
		}

		if (is_fake_closure && func->common.scope &&
				!instanceof_function(Z_OBJCE_P(newthis), func->common.scope)) {
			/* Binding incompatible $this to an internal method is not supported. */
			zend_error(E_WARNING, "Cannot bind method %s::%s() to object of class %s",
					ZSTR_VAL(func->common.scope->name),
					ZSTR_VAL(func->common.function_name),
					ZSTR_VAL(Z_OBJCE_P(newthis)->name));
			return 0;
		}
	} else if (is_fake_closure && func->common.scope
			&& !(func->common.fn_flags & ZEND_ACC_STATIC)) {
		zend_error(E_WARNING, "Cannot unbind $this of method");
		return 0;
	} else if (!is_fake_closure && !Z_ISUNDEF(closure->this_ptr)
			&& (func->common.fn_flags & ZEND_ACC_USES_THIS)) {
		zend_error(E_WARNING, "Cannot unbind $this of closure using $this");
		return 0;
	}

	if (scope && scope != func->common.scope && scope->type == ZEND_INTERNAL_CLASS) {
		/* rebinding to internal class is not allowed */
		zend_error(E_WARNING, "Cannot bind closure to scope of internal class %s",
				ZSTR_VAL(scope->name));
		return 0;
	}

	if (is_fake_closure && scope != func->common.scope) {
		if (func->common.scope == NULL) {
			zend_error(E_WARNING, "Cannot rebind scope of closure created from function");
		} else {
			zend_error(E_WARNING, "Cannot rebind scope of closure created from method");
		}
		return 0;
	}

	return 1;
}
/* }}} */

/* {{{ Call closure, binding to a given object with its class as the scope */
ZEND_METHOD(Closure, call)
{
	zval *newthis, closure_result;
	zend_closure *closure;
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;
	zend_object *newobj;
	zend_class_entry *newclass;

	fci.param_count = 0;
	fci.params = NULL;

	ZEND_PARSE_PARAMETERS_START(1, -1)
		Z_PARAM_OBJECT(newthis)
		Z_PARAM_VARIADIC_WITH_NAMED(fci.params, fci.param_count, fci.named_params)
	ZEND_PARSE_PARAMETERS_END();

	closure = (zend_closure *) Z_OBJ_P(ZEND_THIS);

	newobj = Z_OBJ_P(newthis);
	newclass = newobj->ce;

	if (!zend_valid_closure_binding(closure, newthis, newclass)) {
		return;
	}

	fci_cache.called_scope = newclass;
	fci_cache.object = fci.object = newobj;

	fci.size = sizeof(fci);
	ZVAL_OBJ(&fci.function_name, &closure->std);
	ZVAL_UNDEF(&closure_result);
	fci.retval = &closure_result;

	if (closure->func.common.fn_flags & ZEND_ACC_GENERATOR) {
		zval new_closure;
		zend_create_closure(&new_closure, &closure->func, newclass, closure->called_scope, newthis);
		closure = (zend_closure *) Z_OBJ(new_closure);
		fci_cache.function_handler = &closure->func;

		zend_call_function(&fci, &fci_cache);

		/* copied upon generator creation */
		GC_DELREF(&closure->std);
	} else {
		zend_closure *fake_closure;
		zend_function *my_function;

		fake_closure = emalloc(sizeof(zend_closure));
		memset(&fake_closure->std, 0, sizeof(fake_closure->std));
		fake_closure->std.gc.refcount = 1;
		fake_closure->std.gc.u.type_info = GC_NULL;
		ZVAL_UNDEF(&fake_closure->this_ptr);
		fake_closure->called_scope = NULL;
		my_function = &fake_closure->func;
		if (ZEND_USER_CODE(closure->func.type)) {
			memcpy(my_function, &closure->func, sizeof(zend_op_array));
		} else {
			memcpy(my_function, &closure->func, sizeof(zend_internal_function));
		}
		/* use scope of passed object */
		my_function->common.scope = newclass;
		if (closure->func.type == ZEND_INTERNAL_FUNCTION) {
			my_function->internal_function.handler = closure->orig_internal_handler;
		}
		fci_cache.function_handler = my_function;

		/* Runtime cache relies on bound scope to be immutable, hence we need a separate rt cache in case scope changed */
		if (ZEND_USER_CODE(my_function->type)
		 && (closure->func.common.scope != newclass
		  || (closure->func.common.fn_flags & ZEND_ACC_HEAP_RT_CACHE))) {
			void *ptr;

			my_function->op_array.fn_flags |= ZEND_ACC_HEAP_RT_CACHE;
			ptr = emalloc(my_function->op_array.cache_size);
			ZEND_MAP_PTR_INIT(my_function->op_array.run_time_cache, ptr);
			memset(ptr, 0, my_function->op_array.cache_size);
		}

		zend_call_function(&fci, &fci_cache);

		if (ZEND_USER_CODE(my_function->type)) {
			if (fci_cache.function_handler->common.fn_flags & ZEND_ACC_HEAP_RT_CACHE) {
				efree(ZEND_MAP_PTR(my_function->op_array.run_time_cache));
			}
		}
		efree_size(fake_closure, sizeof(zend_closure));
	}

	if (Z_TYPE(closure_result) != IS_UNDEF) {
		if (Z_ISREF(closure_result)) {
			zend_unwrap_reference(&closure_result);
		}
		ZVAL_COPY_VALUE(return_value, &closure_result);
	}
}
/* }}} */

static void do_closure_bind(zval *return_value, zval *zclosure, zval *newthis, zend_object *scope_obj, zend_string *scope_str)
{
	zend_class_entry *ce, *called_scope;
	zend_closure *closure = (zend_closure *) Z_OBJ_P(zclosure);

	if (scope_obj) {
		ce = scope_obj->ce;
	} else if (scope_str) {
		if (zend_string_equals(scope_str, ZSTR_KNOWN(ZEND_STR_STATIC))) {
			ce = closure->func.common.scope;
		} else if ((ce = zend_lookup_class(scope_str)) == NULL) {
			zend_error(E_WARNING, "Class \"%s\" not found", ZSTR_VAL(scope_str));
			RETURN_NULL();
		}
	} else {
		ce = NULL;
	}

	if (!zend_valid_closure_binding(closure, newthis, ce)) {
		return;
	}

	if (newthis) {
		called_scope = Z_OBJCE_P(newthis);
	} else {
		called_scope = ce;
	}

	zend_create_closure(return_value, &closure->func, ce, called_scope, newthis);
}

/* {{{ Create a closure from another one and bind to another object and scope */
ZEND_METHOD(Closure, bind)
{
	zval *zclosure, *newthis;
	zend_object *scope_obj = NULL;
	zend_string *scope_str = ZSTR_KNOWN(ZEND_STR_STATIC);

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_OBJECT_OF_CLASS(zclosure, zend_ce_closure)
		Z_PARAM_OBJECT_OR_NULL(newthis)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJ_OR_STR_OR_NULL(scope_obj, scope_str)
	ZEND_PARSE_PARAMETERS_END();

	do_closure_bind(return_value, zclosure, newthis, scope_obj, scope_str);
}

/* {{{ Create a closure from another one and bind to another object and scope */
ZEND_METHOD(Closure, bindTo)
{
	zval *newthis;
	zend_object *scope_obj = NULL;
	zend_string *scope_str = ZSTR_KNOWN(ZEND_STR_STATIC);

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_OBJECT_OR_NULL(newthis)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJ_OR_STR_OR_NULL(scope_obj, scope_str)
	ZEND_PARSE_PARAMETERS_END();

	do_closure_bind(return_value, getThis(), newthis, scope_obj, scope_str);
}

static ZEND_NAMED_FUNCTION(zend_closure_call_magic) /* {{{ */ {
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	zval params[2];

	memset(&fci, 0, sizeof(zend_fcall_info));
	memset(&fcc, 0, sizeof(zend_fcall_info_cache));

	fci.size = sizeof(zend_fcall_info);
	fci.retval = return_value;

	fcc.function_handler = (EX(func)->internal_function.fn_flags & ZEND_ACC_STATIC) ?
		EX(func)->internal_function.scope->__callstatic : EX(func)->internal_function.scope->__call;
	fci.named_params = NULL;
	fci.params = params;
	fci.param_count = 2;
	ZVAL_STR(&fci.params[0], EX(func)->common.function_name);
	if (EX_CALL_INFO() & ZEND_CALL_HAS_EXTRA_NAMED_PARAMS) {
		zend_string *name;
		zval *named_param_zval;
		array_init_size(&fci.params[1], ZEND_NUM_ARGS() + zend_hash_num_elements(EX(extra_named_params)));
		/* Avoid conversion from packed to mixed later. */
		zend_hash_real_init_mixed(Z_ARRVAL(fci.params[1]));
		zend_copy_parameters_array(ZEND_NUM_ARGS(), &fci.params[1]);
		ZEND_HASH_MAP_FOREACH_STR_KEY_VAL(EX(extra_named_params), name, named_param_zval) {
			Z_TRY_ADDREF_P(named_param_zval);
			zend_hash_add_new(Z_ARRVAL(fci.params[1]), name, named_param_zval);
		} ZEND_HASH_FOREACH_END();
	} else if (ZEND_NUM_ARGS()) {
		array_init_size(&fci.params[1], ZEND_NUM_ARGS());
		zend_copy_parameters_array(ZEND_NUM_ARGS(), &fci.params[1]);
	} else {
		ZVAL_EMPTY_ARRAY(&fci.params[1]);
	}

	fcc.object = fci.object = Z_OBJ_P(ZEND_THIS);
	fcc.called_scope = zend_get_called_scope(EG(current_execute_data));

	zend_call_function(&fci, &fcc);

	zval_ptr_dtor(&fci.params[1]);
}
/* }}} */

static zend_result zend_create_closure_from_callable(zval *return_value, zval *callable, char **error) /* {{{ */ {
	zend_fcall_info_cache fcc;
	zend_function *mptr;
	zval instance;
	zend_internal_function call;

	if (!zend_is_callable_ex(callable, NULL, 0, NULL, &fcc, error)) {
		return FAILURE;
	}

	mptr = fcc.function_handler;
	if (mptr->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) {
		/* For Closure::fromCallable([$closure, "__invoke"]) return $closure. */
		if (fcc.object && fcc.object->ce == zend_ce_closure
				&& zend_string_equals(mptr->common.function_name, ZSTR_KNOWN(ZEND_STR_MAGIC_INVOKE))) {
			RETVAL_OBJ_COPY(fcc.object);
			zend_free_trampoline(mptr);
			return SUCCESS;
		}

		if (!mptr->common.scope) {
			return FAILURE;
		}
		if (mptr->common.fn_flags & ZEND_ACC_STATIC) {
			if (!mptr->common.scope->__callstatic) {
				return FAILURE;
			}
		} else {
			if (!mptr->common.scope->__call) {
				return FAILURE;
			}
		}

		memset(&call, 0, sizeof(zend_internal_function));
		call.type = ZEND_INTERNAL_FUNCTION;
		call.fn_flags = mptr->common.fn_flags & ZEND_ACC_STATIC;
		call.handler = zend_closure_call_magic;
		call.function_name = mptr->common.function_name;
		call.scope = mptr->common.scope;

		zend_free_trampoline(mptr);
		mptr = (zend_function *) &call;
	}

	if (fcc.object) {
		ZVAL_OBJ(&instance, fcc.object);
		zend_create_fake_closure(return_value, mptr, mptr->common.scope, fcc.called_scope, &instance);
	} else {
		zend_create_fake_closure(return_value, mptr, mptr->common.scope, fcc.called_scope, NULL);
	}

	if (&mptr->internal_function == &call) {
		zend_string_release(mptr->common.function_name);
	}

	return SUCCESS;
}
/* }}} */

/* {{{ Create a closure from a callable using the current scope. */
ZEND_METHOD(Closure, fromCallable)
{
	zval *callable;
	char *error = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(callable)
	ZEND_PARSE_PARAMETERS_END();

	if (Z_TYPE_P(callable) == IS_OBJECT && instanceof_function(Z_OBJCE_P(callable), zend_ce_closure)) {
		/* It's already a closure */
		RETURN_COPY(callable);
	}

	if (zend_create_closure_from_callable(return_value, callable, &error) == FAILURE) {
		if (error) {
			zend_type_error("Failed to create closure from callable: %s", error);
			efree(error);
		} else {
			zend_type_error("Failed to create closure from callable");
		}
	}
}
/* }}} */

static ZEND_COLD zend_function *zend_closure_get_constructor(zend_object *object) /* {{{ */
{
	zend_throw_error(NULL, "Instantiation of class Closure is not allowed");
	return NULL;
}
/* }}} */

/* int return due to Object Handler API */
static int zend_closure_compare(zval *o1, zval *o2) /* {{{ */
{
	ZEND_COMPARE_OBJECTS_FALLBACK(o1, o2);

	zend_closure *lhs = (zend_closure*) Z_OBJ_P(o1);
	zend_closure *rhs = (zend_closure*) Z_OBJ_P(o2);

	if (!((lhs->func.common.fn_flags & ZEND_ACC_FAKE_CLOSURE) && (rhs->func.common.fn_flags & ZEND_ACC_FAKE_CLOSURE))) {
		return ZEND_UNCOMPARABLE;
	}

	if (Z_TYPE(lhs->this_ptr) != Z_TYPE(rhs->this_ptr)) {
		return ZEND_UNCOMPARABLE;
	}

	if (Z_TYPE(lhs->this_ptr) == IS_OBJECT && Z_OBJ(lhs->this_ptr) != Z_OBJ(rhs->this_ptr)) {
		return ZEND_UNCOMPARABLE;
	}

	if (lhs->called_scope != rhs->called_scope) {
		return ZEND_UNCOMPARABLE;
	}

	if (lhs->func.type != rhs->func.type) {
		return ZEND_UNCOMPARABLE;
	}

	if (lhs->func.common.scope != rhs->func.common.scope) {
		return ZEND_UNCOMPARABLE;
	}

	if (!zend_string_equals(lhs->func.common.function_name, rhs->func.common.function_name)) {
		return ZEND_UNCOMPARABLE;
	}

	return 0;
}
/* }}} */

ZEND_API zend_function *zend_get_closure_invoke_method(zend_object *object) /* {{{ */
{
	zend_closure *closure = (zend_closure *)object;
	zend_function *invoke = (zend_function*)emalloc(sizeof(zend_function));
	const uint32_t keep_flags =
		ZEND_ACC_RETURN_REFERENCE | ZEND_ACC_VARIADIC | ZEND_ACC_HAS_RETURN_TYPE;

	invoke->common = closure->func.common;
	/* We return ZEND_INTERNAL_FUNCTION, but arg_info representation is the
	 * same as for ZEND_USER_FUNCTION (uses zend_string* instead of char*).
	 * This is not a problem, because ZEND_ACC_HAS_TYPE_HINTS is never set,
	 * and we won't check arguments on internal function. We also set
	 * ZEND_ACC_USER_ARG_INFO flag to prevent invalid usage by Reflection */
	invoke->type = ZEND_INTERNAL_FUNCTION;
	invoke->internal_function.fn_flags =
		ZEND_ACC_PUBLIC | ZEND_ACC_CALL_VIA_HANDLER | (closure->func.common.fn_flags & keep_flags);
	if (closure->func.type != ZEND_INTERNAL_FUNCTION || (closure->func.common.fn_flags & ZEND_ACC_USER_ARG_INFO)) {
		invoke->internal_function.fn_flags |=
			ZEND_ACC_USER_ARG_INFO;
	}
	invoke->internal_function.handler = ZEND_MN(Closure___invoke);
	invoke->internal_function.module = 0;
	invoke->internal_function.scope = zend_ce_closure;
	invoke->internal_function.function_name = ZSTR_KNOWN(ZEND_STR_MAGIC_INVOKE);
	return invoke;
}
/* }}} */

ZEND_API const zend_function *zend_get_closure_method_def(zend_object *obj) /* {{{ */
{
	zend_closure *closure = (zend_closure *) obj;
	return &closure->func;
}
/* }}} */

ZEND_API zval* zend_get_closure_this_ptr(zval *obj) /* {{{ */
{
	zend_closure *closure = (zend_closure *)Z_OBJ_P(obj);
	return &closure->this_ptr;
}
/* }}} */

static zend_function *zend_closure_get_method(zend_object **object, zend_string *method, const zval *key) /* {{{ */
{
	if (zend_string_equals_literal_ci(method, ZEND_INVOKE_FUNC_NAME)) {
		return zend_get_closure_invoke_method(*object);
	}

	return zend_std_get_method(object, method, key);
}
/* }}} */

static void zend_closure_free_storage(zend_object *object) /* {{{ */
{
	zend_closure *closure = (zend_closure *)object;

	zend_object_std_dtor(&closure->std);

	if (closure->func.type == ZEND_USER_FUNCTION) {
		/* We don't own the static variables of fake closures. */
		if (!(closure->func.op_array.fn_flags & ZEND_ACC_FAKE_CLOSURE)) {
			zend_destroy_static_vars(&closure->func.op_array);
			closure->func.op_array.static_variables = NULL;
		}
		destroy_op_array(&closure->func.op_array);
	} else if (closure->func.type == ZEND_INTERNAL_FUNCTION) {
		zend_string_release(closure->func.common.function_name);
	}

	if (Z_TYPE(closure->this_ptr) != IS_UNDEF) {
		zval_ptr_dtor(&closure->this_ptr);
	}
}
/* }}} */

static zend_object *zend_closure_new(zend_class_entry *class_type) /* {{{ */
{
	zend_closure *closure;

	closure = emalloc(sizeof(zend_closure));
	memset(closure, 0, sizeof(zend_closure));

	zend_object_std_init(&closure->std, class_type);

	return (zend_object*)closure;
}
/* }}} */

static zend_object *zend_closure_clone(zend_object *zobject) /* {{{ */
{
	zend_closure *closure = (zend_closure *)zobject;
	zval result;

	zend_create_closure(&result, &closure->func,
		closure->func.common.scope, closure->called_scope, &closure->this_ptr);
	return Z_OBJ(result);
}
/* }}} */

static zend_result zend_closure_get_closure(zend_object *obj, zend_class_entry **ce_ptr, zend_function **fptr_ptr, zend_object **obj_ptr, bool check_only) /* {{{ */
{
	zend_closure *closure = (zend_closure*)obj;

	*fptr_ptr = &closure->func;
	*ce_ptr = closure->called_scope;

	if (Z_TYPE(closure->this_ptr) != IS_UNDEF) {
		*obj_ptr = Z_OBJ(closure->this_ptr);
	} else {
		*obj_ptr = NULL;
	}

	return SUCCESS;
}
/* }}} */

/* *is_temp is int due to Object Handler API */
static HashTable *zend_closure_get_debug_info(zend_object *object, int *is_temp) /* {{{ */
{
	zend_closure *closure = (zend_closure *)object;
	zval val;
	struct _zend_arg_info *arg_info = closure->func.common.arg_info;
	HashTable *debug_info;
	bool zstr_args = (closure->func.type == ZEND_USER_FUNCTION) || (closure->func.common.fn_flags & ZEND_ACC_USER_ARG_INFO);

	*is_temp = 1;

	debug_info = zend_new_array(8);

	if (closure->func.op_array.fn_flags & ZEND_ACC_FAKE_CLOSURE) {
		if (closure->func.common.scope) {
			zend_string *class_name = closure->func.common.scope->name;
			zend_string *func_name = closure->func.common.function_name;
			zend_string *combined = zend_string_concat3(
				ZSTR_VAL(class_name), ZSTR_LEN(class_name),
				"::", strlen("::"),
				ZSTR_VAL(func_name), ZSTR_LEN(func_name)
			);
			ZVAL_STR(&val, combined);
		} else {
			ZVAL_STR_COPY(&val, closure->func.common.function_name);
		}
		zend_hash_update(debug_info, ZSTR_KNOWN(ZEND_STR_FUNCTION), &val);
	}

	if (closure->func.type == ZEND_USER_FUNCTION && closure->func.op_array.static_variables) {
		zval *var;
		zend_string *key;
		HashTable *static_variables = ZEND_MAP_PTR_GET(closure->func.op_array.static_variables_ptr);

		array_init(&val);

		ZEND_HASH_MAP_FOREACH_STR_KEY_VAL(static_variables, key, var) {
			zval copy;

			if (Z_ISREF_P(var) && Z_REFCOUNT_P(var) == 1) {
				var = Z_REFVAL_P(var);
			}
			ZVAL_COPY(&copy, var);

			zend_hash_add_new(Z_ARRVAL(val), key, &copy);
		} ZEND_HASH_FOREACH_END();

		if (zend_hash_num_elements(Z_ARRVAL(val))) {
			zend_hash_update(debug_info, ZSTR_KNOWN(ZEND_STR_STATIC), &val);
		} else {
			zval_ptr_dtor(&val);
		}
	}

	if (Z_TYPE(closure->this_ptr) != IS_UNDEF) {
		Z_ADDREF(closure->this_ptr);
		zend_hash_update(debug_info, ZSTR_KNOWN(ZEND_STR_THIS), &closure->this_ptr);
	}

	if (arg_info &&
		(closure->func.common.num_args ||
		 (closure->func.common.fn_flags & ZEND_ACC_VARIADIC))) {
		uint32_t i, num_args, required = closure->func.common.required_num_args;

		array_init(&val);

		num_args = closure->func.common.num_args;
		if (closure->func.common.fn_flags & ZEND_ACC_VARIADIC) {
			num_args++;
		}
		for (i = 0; i < num_args; i++) {
			zend_string *name;
			zval info;
			ZEND_ASSERT(arg_info->name && "Argument should have name");
			if (zstr_args) {
				name = zend_strpprintf(0, "%s$%s",
						ZEND_ARG_SEND_MODE(arg_info) ? "&" : "",
						ZSTR_VAL(arg_info->name));
			} else {
				name = zend_strpprintf(0, "%s$%s",
						ZEND_ARG_SEND_MODE(arg_info) ? "&" : "",
						((zend_internal_arg_info*)arg_info)->name);
			}
			ZVAL_NEW_STR(&info, zend_strpprintf(0, "%s", i >= required ? "<optional>" : "<required>"));
			zend_hash_update(Z_ARRVAL(val), name, &info);
			zend_string_release_ex(name, 0);
			arg_info++;
		}
		zend_hash_str_update(debug_info, "parameter", sizeof("parameter")-1, &val);
	}

	return debug_info;
}
/* }}} */

static HashTable *zend_closure_get_gc(zend_object *obj, zval **table, int *n) /* {{{ */
{
	zend_closure *closure = (zend_closure *)obj;

	*table = Z_TYPE(closure->this_ptr) != IS_NULL ? &closure->this_ptr : NULL;
	*n = Z_TYPE(closure->this_ptr) != IS_NULL ? 1 : 0;
	/* Fake closures don't own the static variables they reference. */
	return (closure->func.type == ZEND_USER_FUNCTION
			&& !(closure->func.op_array.fn_flags & ZEND_ACC_FAKE_CLOSURE)) ?
		ZEND_MAP_PTR_GET(closure->func.op_array.static_variables_ptr) : NULL;
}
/* }}} */

/* {{{ Private constructor preventing instantiation */
ZEND_COLD ZEND_METHOD(Closure, __construct)
{
	zend_throw_error(NULL, "Instantiation of class Closure is not allowed");
}
/* }}} */

void zend_register_closure_ce(void) /* {{{ */
{
	zend_ce_closure = register_class_Closure();
	zend_ce_closure->create_object = zend_closure_new;
	zend_ce_closure->default_object_handlers = &closure_handlers;

	memcpy(&closure_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	closure_handlers.free_obj = zend_closure_free_storage;
	closure_handlers.get_constructor = zend_closure_get_constructor;
	closure_handlers.get_method = zend_closure_get_method;
	closure_handlers.compare = zend_closure_compare;
	closure_handlers.clone_obj = zend_closure_clone;
	closure_handlers.get_debug_info = zend_closure_get_debug_info;
	closure_handlers.get_closure = zend_closure_get_closure;
	closure_handlers.get_gc = zend_closure_get_gc;
}
/* }}} */

static ZEND_NAMED_FUNCTION(zend_closure_internal_handler) /* {{{ */
{
	zend_closure *closure = (zend_closure*)ZEND_CLOSURE_OBJECT(EX(func));
	closure->orig_internal_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	// Assign to EX(this) so that it is released after observer checks etc.
	ZEND_ADD_CALL_FLAG(execute_data, ZEND_CALL_RELEASE_THIS);
	Z_OBJ(EX(This)) = &closure->std;
}
/* }}} */

static void zend_create_closure_ex(zval *res, zend_function *func, zend_class_entry *scope, zend_class_entry *called_scope, zval *this_ptr, bool is_fake) /* {{{ */
{
	zend_closure *closure;
	void *ptr;

	object_init_ex(res, zend_ce_closure);

	closure = (zend_closure *)Z_OBJ_P(res);

	if ((scope == NULL) && this_ptr && (Z_TYPE_P(this_ptr) != IS_UNDEF)) {
		/* use dummy scope if we're binding an object without specifying a scope */
		/* maybe it would be better to create one for this purpose */
		scope = zend_ce_closure;
	}

	if (func->type == ZEND_USER_FUNCTION) {
		memcpy(&closure->func, func, sizeof(zend_op_array));
		closure->func.common.fn_flags |= ZEND_ACC_CLOSURE;
		closure->func.common.fn_flags &= ~ZEND_ACC_IMMUTABLE;

		zend_string_addref(closure->func.op_array.function_name);
		if (closure->func.op_array.refcount) {
			(*closure->func.op_array.refcount)++;
		}

		/* For fake closures, we want to reuse the static variables of the original function. */
		if (!is_fake) {
			if (closure->func.op_array.static_variables) {
				closure->func.op_array.static_variables =
					zend_array_dup(closure->func.op_array.static_variables);
			}
			ZEND_MAP_PTR_INIT(closure->func.op_array.static_variables_ptr,
				closure->func.op_array.static_variables);
		} else if (func->op_array.static_variables) {
			HashTable *ht = ZEND_MAP_PTR_GET(func->op_array.static_variables_ptr);

			if (!ht) {
				ht = zend_array_dup(func->op_array.static_variables);
				ZEND_MAP_PTR_SET(func->op_array.static_variables_ptr, ht);
			}
			ZEND_MAP_PTR_INIT(closure->func.op_array.static_variables_ptr, ht);
		}

		/* Runtime cache is scope-dependent, so we cannot reuse it if the scope changed */
		ptr = ZEND_MAP_PTR_GET(func->op_array.run_time_cache);
		if (!ptr
			|| func->common.scope != scope
			|| (func->common.fn_flags & ZEND_ACC_HEAP_RT_CACHE)
		) {
			if (!ptr
			 && (func->common.fn_flags & ZEND_ACC_CLOSURE)
			 && (func->common.scope == scope ||
			     !(func->common.fn_flags & ZEND_ACC_IMMUTABLE))) {
				/* If a real closure is used for the first time, we create a shared runtime cache
				 * and remember which scope it is for. */
				if (func->common.scope != scope) {
					func->common.scope = scope;
				}
				ptr = zend_arena_alloc(&CG(arena), func->op_array.cache_size);
				ZEND_MAP_PTR_SET(func->op_array.run_time_cache, ptr);
				closure->func.op_array.fn_flags &= ~ZEND_ACC_HEAP_RT_CACHE;
			} else {
				/* Otherwise, we use a non-shared runtime cache */
				ptr = emalloc(func->op_array.cache_size);
				closure->func.op_array.fn_flags |= ZEND_ACC_HEAP_RT_CACHE;
			}
			memset(ptr, 0, func->op_array.cache_size);
		}
		ZEND_MAP_PTR_INIT(closure->func.op_array.run_time_cache, ptr);
	} else {
		memcpy(&closure->func, func, sizeof(zend_internal_function));
		closure->func.common.fn_flags |= ZEND_ACC_CLOSURE;
		/* wrap internal function handler to avoid memory leak */
		if (UNEXPECTED(closure->func.internal_function.handler == zend_closure_internal_handler)) {
			/* avoid infinity recursion, by taking handler from nested closure */
			zend_closure *nested = (zend_closure*)((char*)func - XtOffsetOf(zend_closure, func));
			ZEND_ASSERT(nested->std.ce == zend_ce_closure);
			closure->orig_internal_handler = nested->orig_internal_handler;
		} else {
			closure->orig_internal_handler = closure->func.internal_function.handler;
		}
		closure->func.internal_function.handler = zend_closure_internal_handler;
		zend_string_addref(closure->func.op_array.function_name);
		if (!func->common.scope) {
			/* if it's a free function, we won't set scope & this since they're meaningless */
			this_ptr = NULL;
			scope = NULL;
		}
	}

	ZVAL_UNDEF(&closure->this_ptr);
	/* Invariant:
	 * If the closure is unscoped or static, it has no bound object. */
	closure->func.common.scope = scope;
	closure->called_scope = called_scope;
	if (scope) {
		closure->func.common.fn_flags |= ZEND_ACC_PUBLIC;
		if (this_ptr && Z_TYPE_P(this_ptr) == IS_OBJECT && (closure->func.common.fn_flags & ZEND_ACC_STATIC) == 0) {
			ZVAL_OBJ_COPY(&closure->this_ptr, Z_OBJ_P(this_ptr));
		}
	}
}
/* }}} */

ZEND_API void zend_create_closure(zval *res, zend_function *func, zend_class_entry *scope, zend_class_entry *called_scope, zval *this_ptr)
{
	zend_create_closure_ex(res, func, scope, called_scope, this_ptr,
		/* is_fake */ (func->common.fn_flags & ZEND_ACC_FAKE_CLOSURE) != 0);
}

ZEND_API void zend_create_fake_closure(zval *res, zend_function *func, zend_class_entry *scope, zend_class_entry *called_scope, zval *this_ptr) /* {{{ */
{
	zend_closure *closure;

	zend_create_closure_ex(res, func, scope, called_scope, this_ptr, /* is_fake */ true);

	closure = (zend_closure *)Z_OBJ_P(res);
	closure->func.common.fn_flags |= ZEND_ACC_FAKE_CLOSURE;
}
/* }}} */

/* __call and __callStatic name the arguments "$arguments" in the docs. */
static zend_internal_arg_info trampoline_arg_info[] = {ZEND_ARG_VARIADIC_TYPE_INFO(false, arguments, IS_MIXED, false)};

void zend_closure_from_frame(zval *return_value, zend_execute_data *call) { /* {{{ */
	zval instance;
	zend_internal_function trampoline;
	zend_function *mptr = call->func;

	if (ZEND_CALL_INFO(call) & ZEND_CALL_CLOSURE) {
		RETURN_OBJ(ZEND_CLOSURE_OBJECT(mptr));
	}

	if (mptr->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) {
		if ((ZEND_CALL_INFO(call) & ZEND_CALL_HAS_THIS) &&
			(Z_OBJCE(call->This) == zend_ce_closure)
			&& zend_string_equals(mptr->common.function_name, ZSTR_KNOWN(ZEND_STR_MAGIC_INVOKE))) {
	        zend_free_trampoline(mptr);
	        RETURN_OBJ_COPY(Z_OBJ(call->This));
	    }

		memset(&trampoline, 0, sizeof(zend_internal_function));
		trampoline.type = ZEND_INTERNAL_FUNCTION;
		trampoline.fn_flags = mptr->common.fn_flags & (ZEND_ACC_STATIC | ZEND_ACC_VARIADIC);
		trampoline.handler = zend_closure_call_magic;
		trampoline.function_name = mptr->common.function_name;
		trampoline.scope = mptr->common.scope;
		if (trampoline.fn_flags & ZEND_ACC_VARIADIC) {
			trampoline.arg_info = trampoline_arg_info;
		}

		zend_free_trampoline(mptr);
		mptr = (zend_function *) &trampoline;
	}

	if (ZEND_CALL_INFO(call) & ZEND_CALL_HAS_THIS) {
		ZVAL_OBJ(&instance, Z_OBJ(call->This));

		zend_create_fake_closure(return_value, mptr, mptr->common.scope, Z_OBJCE(instance), &instance);
	} else {
		zend_create_fake_closure(return_value, mptr, mptr->common.scope, Z_CE(call->This), NULL);
	}

	if (&mptr->internal_function == &trampoline) {
		zend_string_release(mptr->common.function_name);
	}
} /* }}} */

void zend_closure_bind_var(zval *closure_zv, zend_string *var_name, zval *var) /* {{{ */
{
	zend_closure *closure = (zend_closure *) Z_OBJ_P(closure_zv);
	HashTable *static_variables = ZEND_MAP_PTR_GET(closure->func.op_array.static_variables_ptr);
	zend_hash_update(static_variables, var_name, var);
}
/* }}} */

void zend_closure_bind_var_ex(zval *closure_zv, uint32_t offset, zval *val) /* {{{ */
{
	zend_closure *closure = (zend_closure *) Z_OBJ_P(closure_zv);
	HashTable *static_variables = ZEND_MAP_PTR_GET(closure->func.op_array.static_variables_ptr);
	zval *var = (zval*)((char*)static_variables->arData + offset);
	zval_ptr_dtor(var);
	ZVAL_COPY_VALUE(var, val);
}
/* }}} */
