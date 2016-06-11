/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 Zend Technologies Ltd. (http://www.zend.com) |
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
   |          Dmitry Stogov <dmitry@zend.com>                             |
   |          Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend.h"
#include "zend_API.h"
#include "zend_closures.h"
#include "zend_exceptions.h"
#include "zend_interfaces.h"
#include "zend_objects.h"
#include "zend_objects_API.h"
#include "zend_globals.h"

#define ZEND_CLOSURE_PRINT_NAME "Closure object"

#define ZEND_CLOSURE_PROPERTY_ERROR() \
	zend_throw_error(NULL, "Closure object cannot have properties")

/* reuse bit to mark "fake" closures (it wasn't used for functions before) */
#define ZEND_ACC_FAKE_CLOSURE ZEND_ACC_INTERFACE

typedef struct _zend_closure {
	zend_object       std;
	zend_function     func;
	zval              this_ptr;
	zend_class_entry *called_scope;
	void (*orig_internal_handler)(INTERNAL_FUNCTION_PARAMETERS);
} zend_closure;

/* non-static since it needs to be referenced */
ZEND_API zend_class_entry *zend_ce_closure;
static zend_object_handlers closure_handlers;

ZEND_METHOD(Closure, __invoke) /* {{{ */
{
	zend_function *func = EX(func);
	zval *arguments = ZEND_CALL_ARG(execute_data, 1);

	if (call_user_function_ex(CG(function_table), NULL, getThis(), return_value, ZEND_NUM_ARGS(), arguments, 1, NULL) == FAILURE) {
		RETVAL_FALSE;
	}

	/* destruct the function also, then - we have allocated it in get_method */
	zend_string_release(func->internal_function.function_name);
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
	} else if (!(func->common.fn_flags & ZEND_ACC_STATIC) && func->common.scope
			&& func->type == ZEND_INTERNAL_FUNCTION) {
		zend_error(E_WARNING, "Cannot unbind $this of internal method");
		return 0;
	}

	if (scope && scope != func->common.scope && scope->type == ZEND_INTERNAL_CLASS) {
		/* rebinding to internal class is not allowed */
		zend_error(E_WARNING, "Cannot bind closure to scope of internal class %s",
				ZSTR_VAL(scope->name));
		return 0;
	}

	if (is_fake_closure && scope != func->common.scope) {
		zend_error(E_WARNING, "Cannot rebind scope of closure created by ReflectionFunctionAbstract::getClosure()");
		return 0;
	}

	return 1;
}
/* }}} */

/* {{{ proto mixed Closure::call(object to [, mixed parameter] [, mixed ...] )
   Call closure, binding to a given object with its class as the scope */
ZEND_METHOD(Closure, call)
{
	zval *zclosure, *newthis, closure_result;
	zend_closure *closure;
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;
	zval *my_params;
	int my_param_count = 0;
	zend_function my_function;
	zend_object *newobj;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "o*", &newthis, &my_params, &my_param_count) == FAILURE) {
		return;
	}

	zclosure = getThis();
	closure = (zend_closure *) Z_OBJ_P(zclosure);

	newobj = Z_OBJ_P(newthis);

	if (!zend_valid_closure_binding(closure, newthis, Z_OBJCE_P(newthis))) {
		return;
	}

	/* This should never happen as closures will always be callable */
	if (zend_fcall_info_init(zclosure, 0, &fci, &fci_cache, NULL, NULL) != SUCCESS) {
		ZEND_ASSERT(0);
	}

	fci.retval = &closure_result;
	fci.params = my_params;
	fci.param_count = my_param_count;
	fci.object = fci_cache.object = newobj;
	fci_cache.initialized = 1;
	fci_cache.called_scope = Z_OBJCE_P(newthis);

	if (fci_cache.function_handler->common.fn_flags & ZEND_ACC_GENERATOR) {
		zval new_closure;
		zend_create_closure(&new_closure, fci_cache.function_handler, Z_OBJCE_P(newthis), closure->called_scope, newthis);
		closure = (zend_closure *) Z_OBJ(new_closure);
		fci_cache.function_handler = &closure->func;
	} else {
		memcpy(&my_function, fci_cache.function_handler, fci_cache.function_handler->type == ZEND_USER_FUNCTION ? sizeof(zend_op_array) : sizeof(zend_internal_function));
		/* use scope of passed object */
		my_function.common.scope = Z_OBJCE_P(newthis);
		fci_cache.function_handler = &my_function;

		/* Runtime cache relies on bound scope to be immutable, hence we need a separate rt cache in case scope changed */
		if (ZEND_USER_CODE(my_function.type) && closure->func.common.scope != Z_OBJCE_P(newthis)) {
			my_function.op_array.run_time_cache = emalloc(my_function.op_array.cache_size);
			memset(my_function.op_array.run_time_cache, 0, my_function.op_array.cache_size);
		}
	}

	if (zend_call_function(&fci, &fci_cache) == SUCCESS && Z_TYPE(closure_result) != IS_UNDEF) {
		ZVAL_COPY_VALUE(return_value, &closure_result);
	}

	if (fci_cache.function_handler->common.fn_flags & ZEND_ACC_GENERATOR) {
		/* copied upon generator creation */
		--GC_REFCOUNT(&closure->std);
	} else if (ZEND_USER_CODE(my_function.type) && closure->func.common.scope != Z_OBJCE_P(newthis)) {
		efree(my_function.op_array.run_time_cache);
	}
}
/* }}} */

/* {{{ proto Closure Closure::bind(callable old, object to [, mixed scope])
   Create a closure from another one and bind to another object and scope */
ZEND_METHOD(Closure, bind)
{
	zval *newthis, *zclosure, *scope_arg = NULL;
	zend_closure *closure, *new_closure;
	zend_class_entry *ce, *called_scope;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oo!|z", &zclosure, zend_ce_closure, &newthis, &scope_arg) == FAILURE) {
		return;
	}

	closure = (zend_closure *)Z_OBJ_P(zclosure);

	if (scope_arg != NULL) { /* scope argument was given */
		if (Z_TYPE_P(scope_arg) == IS_OBJECT) {
			ce = Z_OBJCE_P(scope_arg);
		} else if (Z_TYPE_P(scope_arg) == IS_NULL) {
			ce = NULL;
		} else {
			zend_string *class_name = zval_get_string(scope_arg);
			if (zend_string_equals_literal(class_name, "static")) {
				ce = closure->func.common.scope;
			} else if ((ce = zend_lookup_class_ex(class_name, NULL, 1)) == NULL) {
				zend_error(E_WARNING, "Class '%s' not found", ZSTR_VAL(class_name));
				zend_string_release(class_name);
				RETURN_NULL();
			}
			zend_string_release(class_name);
		}
	} else { /* scope argument not given; do not change the scope by default */
		ce = closure->func.common.scope;
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
	new_closure = (zend_closure *) Z_OBJ_P(return_value);

	/* Runtime cache relies on bound scope to be immutable, hence we need a separate rt cache in case scope changed */
	if (ZEND_USER_CODE(closure->func.type) && (closure->func.common.scope != new_closure->func.common.scope || (closure->func.op_array.fn_flags & ZEND_ACC_NO_RT_ARENA))) {
		new_closure->func.op_array.run_time_cache = emalloc(new_closure->func.op_array.cache_size);
		memset(new_closure->func.op_array.run_time_cache, 0, new_closure->func.op_array.cache_size);

		new_closure->func.op_array.fn_flags |= ZEND_ACC_NO_RT_ARENA;
	}
}
/* }}} */

static ZEND_COLD zend_function *zend_closure_get_constructor(zend_object *object) /* {{{ */
{
	zend_throw_error(NULL, "Instantiation of 'Closure' is not allowed");
	return NULL;
}
/* }}} */

static int zend_closure_compare_objects(zval *o1, zval *o2) /* {{{ */
{
	return (Z_OBJ_P(o1) != Z_OBJ_P(o2));
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
	invoke->internal_function.function_name = CG(known_strings)[ZEND_STR_MAGIC_INVOKE];
	return invoke;
}
/* }}} */

ZEND_API const zend_function *zend_get_closure_method_def(zval *obj) /* {{{ */
{
	zend_closure *closure = (zend_closure *)Z_OBJ_P(obj);
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

	return std_object_handlers.get_method(object, method, key);
}
/* }}} */

static zval *zend_closure_read_property(zval *object, zval *member, int type, void **cache_slot, zval *rv) /* {{{ */
{
	ZEND_CLOSURE_PROPERTY_ERROR();
	return &EG(uninitialized_zval);
}
/* }}} */

static void zend_closure_write_property(zval *object, zval *member, zval *value, void **cache_slot) /* {{{ */
{
	ZEND_CLOSURE_PROPERTY_ERROR();
}
/* }}} */

static zval *zend_closure_get_property_ptr_ptr(zval *object, zval *member, int type, void **cache_slot) /* {{{ */
{
	ZEND_CLOSURE_PROPERTY_ERROR();
	return NULL;
}
/* }}} */

static int zend_closure_has_property(zval *object, zval *member, int has_set_exists, void **cache_slot) /* {{{ */
{
	if (has_set_exists != 2) {
		ZEND_CLOSURE_PROPERTY_ERROR();
	}
	return 0;
}
/* }}} */

static void zend_closure_unset_property(zval *object, zval *member, void **cache_slot) /* {{{ */
{
	ZEND_CLOSURE_PROPERTY_ERROR();
}
/* }}} */

static void zend_closure_free_storage(zend_object *object) /* {{{ */
{
	zend_closure *closure = (zend_closure *)object;

	zend_object_std_dtor(&closure->std);

	if (closure->func.type == ZEND_USER_FUNCTION) {
		if (closure->func.op_array.fn_flags & ZEND_ACC_NO_RT_ARENA) {
			efree(closure->func.op_array.run_time_cache);
			closure->func.op_array.run_time_cache = NULL;
		}
		destroy_op_array(&closure->func.op_array);
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

static zend_object *zend_closure_clone(zval *zobject) /* {{{ */
{
	zend_closure *closure = (zend_closure *)Z_OBJ_P(zobject);
	zval result;

	zend_create_closure(&result, &closure->func,
		closure->func.common.scope, closure->called_scope, &closure->this_ptr);
	return Z_OBJ(result);
}
/* }}} */

int zend_closure_get_closure(zval *obj, zend_class_entry **ce_ptr, zend_function **fptr_ptr, zend_object **obj_ptr) /* {{{ */
{
	zend_closure *closure = (zend_closure *)Z_OBJ_P(obj);
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

static HashTable *zend_closure_get_debug_info(zval *object, int *is_temp) /* {{{ */
{
	zend_closure *closure = (zend_closure *)Z_OBJ_P(object);
	zval val;
	struct _zend_arg_info *arg_info = closure->func.common.arg_info;
	HashTable *debug_info;

	*is_temp = 1;

	ALLOC_HASHTABLE(debug_info);
	zend_hash_init(debug_info, 8, NULL, ZVAL_PTR_DTOR, 0);

	if (closure->func.type == ZEND_USER_FUNCTION && closure->func.op_array.static_variables) {
		HashTable *static_variables = closure->func.op_array.static_variables;
		ZVAL_ARR(&val, zend_array_dup(static_variables));
		zend_hash_update(debug_info, CG(known_strings)[ZEND_STR_STATIC], &val);
	}

	if (Z_TYPE(closure->this_ptr) != IS_UNDEF) {
		Z_ADDREF(closure->this_ptr);
		zend_hash_update(debug_info, CG(known_strings)[ZEND_STR_THIS], &closure->this_ptr);
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
				name = zend_strpprintf(0, "%s$%s",
						arg_info->pass_by_reference ? "&" : "",
						ZSTR_VAL(arg_info->name));
			} else {
				name = zend_strpprintf(0, "%s$param%d",
						arg_info->pass_by_reference ? "&" : "",
						i + 1);
			}
			ZVAL_NEW_STR(&info, zend_strpprintf(0, "%s", i >= required ? "<optional>" : "<required>"));
			zend_hash_update(Z_ARRVAL(val), name, &info);
			zend_string_release(name);
			arg_info++;
		}
		zend_hash_str_update(debug_info, "parameter", sizeof("parameter")-1, &val);
	}

	return debug_info;
}
/* }}} */

static HashTable *zend_closure_get_gc(zval *obj, zval **table, int *n) /* {{{ */
{
	zend_closure *closure = (zend_closure *)Z_OBJ_P(obj);

	*table = Z_TYPE(closure->this_ptr) != IS_NULL ? &closure->this_ptr : NULL;
	*n = Z_TYPE(closure->this_ptr) != IS_NULL ? 1 : 0;
	return (closure->func.type == ZEND_USER_FUNCTION) ?
		closure->func.op_array.static_variables : NULL;
}
/* }}} */

/* {{{ proto Closure::__construct()
   Private constructor preventing instantiation */
ZEND_COLD ZEND_METHOD(Closure, __construct)
{
	zend_throw_error(NULL, "Instantiation of 'Closure' is not allowed");
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo_closure_bindto, 0, 0, 1)
	ZEND_ARG_INFO(0, newthis)
	ZEND_ARG_INFO(0, newscope)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_closure_bind, 0, 0, 2)
	ZEND_ARG_INFO(0, closure)
	ZEND_ARG_INFO(0, newthis)
	ZEND_ARG_INFO(0, newscope)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_closure_call, 0, 0, 1)
	ZEND_ARG_INFO(0, newthis)
	ZEND_ARG_VARIADIC_INFO(0, parameters)
ZEND_END_ARG_INFO()

static const zend_function_entry closure_functions[] = {
	ZEND_ME(Closure, __construct, NULL, ZEND_ACC_PRIVATE)
	ZEND_ME(Closure, bind, arginfo_closure_bind, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_MALIAS(Closure, bindTo, bind, arginfo_closure_bindto, ZEND_ACC_PUBLIC)
	ZEND_ME(Closure, call, arginfo_closure_call, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

void zend_register_closure_ce(void) /* {{{ */
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "Closure", closure_functions);
	zend_ce_closure = zend_register_internal_class(&ce);
	zend_ce_closure->ce_flags |= ZEND_ACC_FINAL;
	zend_ce_closure->create_object = zend_closure_new;
	zend_ce_closure->serialize = zend_class_serialize_deny;
	zend_ce_closure->unserialize = zend_class_unserialize_deny;

	memcpy(&closure_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	closure_handlers.free_obj = zend_closure_free_storage;
	closure_handlers.get_constructor = zend_closure_get_constructor;
	closure_handlers.get_method = zend_closure_get_method;
	closure_handlers.write_property = zend_closure_write_property;
	closure_handlers.read_property = zend_closure_read_property;
	closure_handlers.get_property_ptr_ptr = zend_closure_get_property_ptr_ptr;
	closure_handlers.has_property = zend_closure_has_property;
	closure_handlers.unset_property = zend_closure_unset_property;
	closure_handlers.compare_objects = zend_closure_compare_objects;
	closure_handlers.clone_obj = zend_closure_clone;
	closure_handlers.get_debug_info = zend_closure_get_debug_info;
	closure_handlers.get_closure = zend_closure_get_closure;
	closure_handlers.get_gc = zend_closure_get_gc;
}
/* }}} */

static void zend_closure_internal_handler(INTERNAL_FUNCTION_PARAMETERS) /* {{{ */
{
	zend_closure *closure = (zend_closure*)EX(func)->common.prototype;
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
		closure->func.common.prototype = (zend_function*)closure;
		closure->func.common.fn_flags |= ZEND_ACC_CLOSURE;
		if (closure->func.op_array.static_variables) {
			closure->func.op_array.static_variables =
				zend_array_dup(closure->func.op_array.static_variables);
		}
		if (UNEXPECTED(!closure->func.op_array.run_time_cache)) {
			closure->func.op_array.run_time_cache = func->op_array.run_time_cache = zend_arena_alloc(&CG(arena), func->op_array.cache_size);
			memset(func->op_array.run_time_cache, 0, func->op_array.cache_size);
		}
		if (closure->func.op_array.refcount) {
			(*closure->func.op_array.refcount)++;
		}
	} else {
		memcpy(&closure->func, func, sizeof(zend_internal_function));
		closure->func.common.prototype = (zend_function*)closure;
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
			ZVAL_COPY(&closure->this_ptr, this_ptr);
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
	HashTable *static_variables = closure->func.op_array.static_variables;
	zend_hash_update(static_variables, var_name, var);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
