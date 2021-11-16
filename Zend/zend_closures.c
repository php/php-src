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

#define ZEND_CLOSURE_PRINT_NAME "Closure object"

#define ZEND_CLOSURE_PROPERTY_ERROR() \
	zend_throw_error(NULL, "Closure object cannot have properties")

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

static zend_bool zend_valid_closure_binding(
		zend_closure *closure, zval *newthis, zend_class_entry *scope) /* {{{ */
{
	zend_function *func = &closure->func;
	zend_bool is_fake_closure = (func->common.fn_flags & ZEND_ACC_FAKE_CLOSURE) != 0;
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
	zend_function my_function;
	zend_object *newobj;

	fci.param_count = 0;
	fci.params = NULL;

	ZEND_PARSE_PARAMETERS_START(1, -1)
		Z_PARAM_OBJECT(newthis)
		Z_PARAM_VARIADIC_WITH_NAMED(fci.params, fci.param_count, fci.named_params)
	ZEND_PARSE_PARAMETERS_END();

	closure = (zend_closure *) Z_OBJ_P(ZEND_THIS);

	newobj = Z_OBJ_P(newthis);

	if (!zend_valid_closure_binding(closure, newthis, Z_OBJCE_P(newthis))) {
		return;
	}

	if (closure->func.common.fn_flags & ZEND_ACC_GENERATOR) {
		zval new_closure;
		zend_create_closure(&new_closure, &closure->func, Z_OBJCE_P(newthis), closure->called_scope, newthis);
		closure = (zend_closure *) Z_OBJ(new_closure);
		fci_cache.function_handler = &closure->func;
	} else {
		memcpy(&my_function, &closure->func, closure->func.type == ZEND_USER_FUNCTION ? sizeof(zend_op_array) : sizeof(zend_internal_function));
		my_function.common.fn_flags &= ~ZEND_ACC_CLOSURE;
		/* use scope of passed object */
		my_function.common.scope = Z_OBJCE_P(newthis);
		if (closure->func.type == ZEND_INTERNAL_FUNCTION) {
			my_function.internal_function.handler = closure->orig_internal_handler;
		}
		fci_cache.function_handler = &my_function;

		/* Runtime cache relies on bound scope to be immutable, hence we need a separate rt cache in case scope changed */
		if (ZEND_USER_CODE(my_function.type)
		 && (closure->func.common.scope != Z_OBJCE_P(newthis)
		  || (closure->func.common.fn_flags & ZEND_ACC_HEAP_RT_CACHE))) {
			void *ptr;

			my_function.op_array.fn_flags |= ZEND_ACC_HEAP_RT_CACHE;
			ptr = emalloc(sizeof(void*) + my_function.op_array.cache_size);
			ZEND_MAP_PTR_INIT(my_function.op_array.run_time_cache, ptr);
			ptr = (char*)ptr + sizeof(void*);
			ZEND_MAP_PTR_SET(my_function.op_array.run_time_cache, ptr);
			memset(ptr, 0, my_function.op_array.cache_size);
		}
	}

	fci_cache.called_scope = newobj->ce;
	fci_cache.object = fci.object = newobj;

	fci.size = sizeof(fci);
	ZVAL_OBJ(&fci.function_name, &closure->std);
	fci.retval = &closure_result;

	if (zend_call_function(&fci, &fci_cache) == SUCCESS && Z_TYPE(closure_result) != IS_UNDEF) {
		if (Z_ISREF(closure_result)) {
			zend_unwrap_reference(&closure_result);
		}
		ZVAL_COPY_VALUE(return_value, &closure_result);
	}

	if (fci_cache.function_handler->common.fn_flags & ZEND_ACC_GENERATOR) {
		/* copied upon generator creation */
		GC_DELREF(&closure->std);
	} else if (ZEND_USER_CODE(my_function.type)
	 && (fci_cache.function_handler->common.fn_flags & ZEND_ACC_HEAP_RT_CACHE)) {
		efree(ZEND_MAP_PTR(my_function.op_array.run_time_cache));
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
	if (ZEND_NUM_ARGS()) {
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
				&& zend_string_equals_literal(mptr->common.function_name, "__invoke")) {
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

	return SUCCESS;
}
/* }}} */

/* {{{ Create a closure from a callable using the current scope. */
ZEND_METHOD(Closure, fromCallable)
{
	zval *callable;
	char *error = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &callable) == FAILURE) {
		RETURN_THROWS();
	}

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
	return Z_OBJ_P(o1) != Z_OBJ_P(o2);
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

static ZEND_COLD zval *zend_closure_read_property(zend_object *object, zend_string *member, int type, void **cache_slot, zval *rv) /* {{{ */
{
	ZEND_CLOSURE_PROPERTY_ERROR();
	return &EG(uninitialized_zval);
}
/* }}} */

static ZEND_COLD zval *zend_closure_write_property(zend_object *object, zend_string *member, zval *value, void **cache_slot) /* {{{ */
{
	ZEND_CLOSURE_PROPERTY_ERROR();
	return &EG(error_zval);
}
/* }}} */

static ZEND_COLD zval *zend_closure_get_property_ptr_ptr(zend_object *object, zend_string *member, int type, void **cache_slot) /* {{{ */
{
	ZEND_CLOSURE_PROPERTY_ERROR();
	return NULL;
}
/* }}} */

static ZEND_COLD int zend_closure_has_property(zend_object *object, zend_string *member, int has_set_exists, void **cache_slot) /* {{{ */
{
	if (has_set_exists != ZEND_PROPERTY_EXISTS) {
		ZEND_CLOSURE_PROPERTY_ERROR();
	}
	return 0;
}
/* }}} */

static ZEND_COLD void zend_closure_unset_property(zend_object *object, zend_string *member, void **cache_slot) /* {{{ */
{
	ZEND_CLOSURE_PROPERTY_ERROR();
}
/* }}} */

static void zend_closure_free_storage(zend_object *object) /* {{{ */
{
	zend_closure *closure = (zend_closure *)object;

	zend_object_std_dtor(&closure->std);

	if (closure->func.type == ZEND_USER_FUNCTION) {
		destroy_op_array(&closure->func.op_array);
	} else if (closure->orig_internal_handler == zend_closure_call_magic) {
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
	closure->std.handlers = &closure_handlers;

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

int zend_closure_get_closure(zend_object *obj, zend_class_entry **ce_ptr, zend_function **fptr_ptr, zend_object **obj_ptr, zend_bool check_only) /* {{{ */
{
	zend_closure *closure = (zend_closure *)obj;
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
	zend_bool zstr_args = (closure->func.type == ZEND_USER_FUNCTION) || (closure->func.common.fn_flags & ZEND_ACC_USER_ARG_INFO);

	*is_temp = 1;

	debug_info = zend_new_array(8);

	if (closure->func.type == ZEND_USER_FUNCTION && closure->func.op_array.static_variables) {
		zval *var;
		zend_string *key;
		HashTable *static_variables = ZEND_MAP_PTR_GET(closure->func.op_array.static_variables_ptr);

		array_init(&val);

		ZEND_HASH_FOREACH_STR_KEY_VAL(static_variables, key, var) {
			zval copy;

			if (Z_TYPE_P(var) == IS_CONSTANT_AST) {
				ZVAL_STRING(&copy, "<constant ast>");
			} else {
				if (Z_ISREF_P(var) && Z_REFCOUNT_P(var) == 1) {
					var = Z_REFVAL_P(var);
				}
				ZVAL_COPY(&copy, var);
			}

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
			if (arg_info->name) {
				if (zstr_args) {
					name = zend_strpprintf(0, "%s$%s",
							ZEND_ARG_SEND_MODE(arg_info) ? "&" : "",
							ZSTR_VAL(arg_info->name));
				} else {
					name = zend_strpprintf(0, "%s$%s",
							ZEND_ARG_SEND_MODE(arg_info) ? "&" : "",
							((zend_internal_arg_info*)arg_info)->name);
				}
			} else {
				name = zend_strpprintf(0, "%s$param%d",
						ZEND_ARG_SEND_MODE(arg_info) ? "&" : "",
						i + 1);
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
	return (closure->func.type == ZEND_USER_FUNCTION) ?
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
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "Closure", class_Closure_methods);
	zend_ce_closure = zend_register_internal_class(&ce);
	zend_ce_closure->ce_flags |= ZEND_ACC_FINAL;
	zend_ce_closure->create_object = zend_closure_new;
	zend_ce_closure->serialize = zend_class_serialize_deny;
	zend_ce_closure->unserialize = zend_class_unserialize_deny;

	memcpy(&closure_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	closure_handlers.free_obj = zend_closure_free_storage;
	closure_handlers.get_constructor = zend_closure_get_constructor;
	closure_handlers.get_method = zend_closure_get_method;
	closure_handlers.write_property = zend_closure_write_property;
	closure_handlers.read_property = zend_closure_read_property;
	closure_handlers.get_property_ptr_ptr = zend_closure_get_property_ptr_ptr;
	closure_handlers.has_property = zend_closure_has_property;
	closure_handlers.unset_property = zend_closure_unset_property;
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
	OBJ_RELEASE((zend_object*)closure);
	EX(func) = NULL;
}
/* }}} */

ZEND_API void zend_create_closure(zval *res, zend_function *func, zend_class_entry *scope, zend_class_entry *called_scope, zval *this_ptr) /* {{{ */
{
	zend_closure *closure;

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

		if (closure->func.op_array.static_variables) {
			closure->func.op_array.static_variables =
				zend_array_dup(closure->func.op_array.static_variables);
		}
		ZEND_MAP_PTR_INIT(closure->func.op_array.static_variables_ptr,
			&closure->func.op_array.static_variables);

		/* Runtime cache is scope-dependent, so we cannot reuse it if the scope changed */
		if (!ZEND_MAP_PTR_GET(closure->func.op_array.run_time_cache)
			|| func->common.scope != scope
			|| (func->common.fn_flags & ZEND_ACC_HEAP_RT_CACHE)
		) {
			void *ptr;

			if (!ZEND_MAP_PTR_GET(func->op_array.run_time_cache)
			 && (func->common.fn_flags & ZEND_ACC_CLOSURE)
			 && (func->common.scope == scope ||
			     !(func->common.fn_flags & ZEND_ACC_IMMUTABLE))) {
				/* If a real closure is used for the first time, we create a shared runtime cache
				 * and remember which scope it is for. */
				if (func->common.scope != scope) {
					func->common.scope = scope;
				}
				closure->func.op_array.fn_flags &= ~ZEND_ACC_HEAP_RT_CACHE;
				ptr = zend_arena_alloc(&CG(arena), func->op_array.cache_size);
				ZEND_MAP_PTR_SET(func->op_array.run_time_cache, ptr);
				ZEND_MAP_PTR_SET(closure->func.op_array.run_time_cache, ptr);
			} else {
				/* Otherwise, we use a non-shared runtime cache */
				closure->func.op_array.fn_flags |= ZEND_ACC_HEAP_RT_CACHE;
				ptr = emalloc(sizeof(void*) + func->op_array.cache_size);
				ZEND_MAP_PTR_INIT(closure->func.op_array.run_time_cache, ptr);
				ptr = (char*)ptr + sizeof(void*);
				ZEND_MAP_PTR_SET(closure->func.op_array.run_time_cache, ptr);
			}
			memset(ptr, 0, func->op_array.cache_size);
		}
		zend_string_addref(closure->func.op_array.function_name);
		if (closure->func.op_array.refcount) {
			(*closure->func.op_array.refcount)++;
		}
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

ZEND_API void zend_create_fake_closure(zval *res, zend_function *func, zend_class_entry *scope, zend_class_entry *called_scope, zval *this_ptr) /* {{{ */
{
	zend_closure *closure;

	zend_create_closure(res, func, scope, called_scope, this_ptr);

	closure = (zend_closure *)Z_OBJ_P(res);
	closure->func.common.fn_flags |= ZEND_ACC_FAKE_CLOSURE;
}
/* }}} */

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
