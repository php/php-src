/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2015 Zend Technologies Ltd. (http://www.zend.com) |
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
	zend_error(E_EXCEPTION | E_ERROR, "Closure object cannot have properties")

typedef struct _zend_closure {
	zend_object    std;
	zend_function  func;
	zval           this_ptr;
	HashTable     *debug_info;
} zend_closure;

/* non-static since it needs to be referenced */
ZEND_API zend_class_entry *zend_ce_closure;
static zend_object_handlers closure_handlers;

ZEND_METHOD(Closure, __invoke) /* {{{ */
{
	zend_function *func = EX(func);
	zval *arguments;

	arguments = emalloc(sizeof(zval) * ZEND_NUM_ARGS());
	if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), arguments) == FAILURE) {
		efree(arguments);
		zend_error(E_EXCEPTION | E_ERROR, "Cannot get arguments for calling closure");
		RETVAL_FALSE;
	} else if (call_user_function_ex(CG(function_table), NULL, getThis(), return_value, ZEND_NUM_ARGS(), arguments, 1, NULL) == FAILURE) {
		RETVAL_FALSE;
	}
	efree(arguments);

	/* destruct the function also, then - we have allocated it in get_method */
	zend_string_release(func->internal_function.function_name);
	efree(func);
#if ZEND_DEBUG
	execute_data->func = NULL;
#endif
}
/* }}} */

/* {{{ proto mixed Closure::call(object $to [, mixed $parameter] [, mixed $...] )
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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "o*", &newthis, &my_params, &my_param_count) == FAILURE) {
		return;
	}

	zclosure = getThis();
	closure = (zend_closure *)Z_OBJ_P(zclosure);

	if (closure->func.common.fn_flags & ZEND_ACC_STATIC) {
		zend_error(E_WARNING, "Cannot bind an instance to a static closure");
		return;
	}

	if (closure->func.type == ZEND_INTERNAL_FUNCTION) {
		/* verify that we aren't binding internal function to a wrong object */
		if ((closure->func.common.fn_flags & ZEND_ACC_STATIC) == 0 &&
				!instanceof_function(Z_OBJCE_P(newthis), closure->func.common.scope)) {
			zend_error(E_WARNING, "Cannot bind function %s::%s to object of class %s", closure->func.common.scope->name->val, closure->func.common.function_name->val, Z_OBJCE_P(newthis)->name->val);
			return;
		}
	}

	/* This should never happen as closures will always be callable */
	if (zend_fcall_info_init(zclosure, 0, &fci, &fci_cache, NULL, NULL) != SUCCESS) {
		ZEND_ASSERT(0);
	}

	fci.retval = &closure_result;
	fci.params = my_params;
	fci.param_count = my_param_count;
	fci.object = fci_cache.object = Z_OBJ_P(newthis);
	fci_cache.initialized = 1;

	my_function = *fci_cache.function_handler;
	/* use scope of passed object */
	my_function.common.scope = Z_OBJCE_P(newthis);
	fci_cache.function_handler = &my_function;

	if (zend_call_function(&fci, &fci_cache) == SUCCESS && Z_TYPE(closure_result) != IS_UNDEF) {
		ZVAL_COPY_VALUE(return_value, &closure_result);
	}
}
/* }}} */

/* {{{ proto Closure Closure::bind(Closure $old, object $to [, mixed $scope = "static" ] )
   Create a closure from another one and bind to another object and scope */
ZEND_METHOD(Closure, bind)
{
	zval *newthis, *zclosure, *scope_arg = NULL;
	zend_closure *closure;
	zend_class_entry *ce;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oo!|z", &zclosure, zend_ce_closure, &newthis, &scope_arg) == FAILURE) {
		RETURN_NULL();
	}

	closure = (zend_closure *)Z_OBJ_P(zclosure);

	if ((newthis != NULL) && (closure->func.common.fn_flags & ZEND_ACC_STATIC)) {
		zend_error(E_WARNING, "Cannot bind an instance to a static closure");
	}

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
				zend_error(E_WARNING, "Class '%s' not found", class_name->val);
				zend_string_release(class_name);
				RETURN_NULL();
			}
			zend_string_release(class_name);
		}
	} else { /* scope argument not given; do not change the scope by default */
		ce = closure->func.common.scope;
	}

	zend_create_closure(return_value, &closure->func, ce, newthis);
}
/* }}} */

static zend_function *zend_closure_get_constructor(zend_object *object) /* {{{ */
{
	zend_error(E_EXCEPTION | E_ERROR, "Instantiation of 'Closure' is not allowed");
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

	invoke->common = closure->func.common;
	/* TODO: return ZEND_INTERNAL_FUNCTION, but arg_info representation is suitable for ZEND_USER_FUNCTION ??? */
	invoke->type = ZEND_INTERNAL_FUNCTION;
	invoke->internal_function.fn_flags = ZEND_ACC_PUBLIC | ZEND_ACC_CALL_VIA_HANDLER | (closure->func.common.fn_flags & ZEND_ACC_RETURN_REFERENCE);
	invoke->internal_function.handler = ZEND_MN(Closure___invoke);
	invoke->internal_function.module = 0;
	invoke->internal_function.scope = zend_ce_closure;
	invoke->internal_function.function_name = zend_string_init(ZEND_INVOKE_FUNC_NAME, sizeof(ZEND_INVOKE_FUNC_NAME)-1, 0);
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
	zend_string *lc_name;

	lc_name = zend_string_tolower(method);
	if (zend_string_equals_literal(method, ZEND_INVOKE_FUNC_NAME)) {
		zend_string_release(lc_name);
		return zend_get_closure_invoke_method(*object);
	}
	zend_string_release(lc_name);
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
		zend_execute_data *ex = EG(current_execute_data);
		while (ex) {
			if (ex->func == &closure->func) {
				zend_error(E_ERROR, "Cannot destroy active lambda function");
			}
			ex = ex->prev_execute_data;
		}
		destroy_op_array(&closure->func.op_array);
	}

	if (closure->debug_info != NULL) {
		zend_hash_destroy(closure->debug_info);
		efree(closure->debug_info);
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

	zend_create_closure(&result, &closure->func, closure->func.common.scope, &closure->this_ptr);
	return Z_OBJ(result);
}
/* }}} */

int zend_closure_get_closure(zval *obj, zend_class_entry **ce_ptr, zend_function **fptr_ptr, zend_object **obj_ptr) /* {{{ */
{
	zend_closure *closure;

	if (Z_TYPE_P(obj) != IS_OBJECT) {
		return FAILURE;
	}

	closure = (zend_closure *)Z_OBJ_P(obj);
	*fptr_ptr = &closure->func;

	if (Z_TYPE(closure->this_ptr) != IS_UNDEF) {
		if (obj_ptr) {
			*obj_ptr = Z_OBJ(closure->this_ptr);
		}
		*ce_ptr = Z_OBJCE(closure->this_ptr);
	} else {
		if (obj_ptr) {
			*obj_ptr = NULL;
		}
		*ce_ptr = closure->func.common.scope;
	}
	return SUCCESS;
}
/* }}} */

static HashTable *zend_closure_get_debug_info(zval *object, int *is_temp) /* {{{ */
{
	zend_closure *closure = (zend_closure *)Z_OBJ_P(object);
	zval val;
	struct _zend_arg_info *arg_info = closure->func.common.arg_info;

	*is_temp = 0;

	if (closure->debug_info == NULL) {
		ALLOC_HASHTABLE(closure->debug_info);
		zend_hash_init(closure->debug_info, 8, NULL, ZVAL_PTR_DTOR, 0);
	}
	if (closure->debug_info->u.v.nApplyCount == 0) {
		if (closure->func.type == ZEND_USER_FUNCTION && closure->func.op_array.static_variables) {
			HashTable *static_variables = closure->func.op_array.static_variables;
			ZVAL_ARR(&val, zend_array_dup(static_variables));
			zend_hash_str_update(closure->debug_info, "static", sizeof("static")-1, &val);
		}

		if (Z_TYPE(closure->this_ptr) != IS_UNDEF) {
			Z_ADDREF(closure->this_ptr);
			zend_hash_str_update(closure->debug_info, "this", sizeof("this")-1, &closure->this_ptr);
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
							arg_info->name->val);
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
			zend_hash_str_update(closure->debug_info, "parameter", sizeof("parameter")-1, &val);
		}
	}

	return closure->debug_info;
}
/* }}} */

static HashTable *zend_closure_get_gc(zval *obj, zval **table, int *n) /* {{{ */
{
	zend_closure *closure = (zend_closure *)Z_OBJ_P(obj);

	if (closure->debug_info != NULL) {
		zend_hash_destroy(closure->debug_info);
		efree(closure->debug_info);
		closure->debug_info = NULL;
	}

	*table = Z_TYPE(closure->this_ptr) != IS_NULL ? &closure->this_ptr : NULL;
	*n = Z_TYPE(closure->this_ptr) != IS_NULL ? 1 : 0;
	return (closure->func.type == ZEND_USER_FUNCTION) ?
		closure->func.op_array.static_variables : NULL;
}
/* }}} */

/* {{{ proto Closure::__construct()
   Private constructor preventing instantiation */
ZEND_METHOD(Closure, __construct)
{
	zend_error(E_EXCEPTION | E_ERROR, "Instantiation of 'Closure' is not allowed");
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
	closure_handlers.clone_obj = NULL;
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

ZEND_API void zend_create_closure(zval *res, zend_function *func, zend_class_entry *scope, zval *this_ptr) /* {{{ */
{
	zend_closure *closure;

	object_init_ex(res, zend_ce_closure);

	closure = (zend_closure *)Z_OBJ_P(res);

	closure->func = *func;
	closure->func.common.prototype = (zend_function*)closure;
	closure->func.common.fn_flags |= ZEND_ACC_CLOSURE;

	if ((scope == NULL) && this_ptr && (Z_TYPE_P(this_ptr) != IS_UNDEF)) {
		/* use dummy scope if we're binding an object without specifying a scope */
		/* maybe it would be better to create one for this purpose */
		scope = zend_ce_closure;
	}

	if (closure->func.type == ZEND_USER_FUNCTION) {
		if (closure->func.op_array.static_variables) {
			HashTable *static_variables = closure->func.op_array.static_variables;

			ALLOC_HASHTABLE(closure->func.op_array.static_variables);
			zend_hash_init(closure->func.op_array.static_variables, zend_hash_num_elements(static_variables), NULL, ZVAL_PTR_DTOR, 0);
			zend_hash_apply_with_arguments(static_variables, zval_copy_static_var, 1, closure->func.op_array.static_variables);
		}
		closure->func.op_array.run_time_cache = NULL;
		if (closure->func.op_array.refcount) {
			(*closure->func.op_array.refcount)++;
		}
	} else {
		/* verify that we aren't binding internal function to a wrong scope */
		if(func->common.scope != NULL) {
			if(scope && !instanceof_function(scope, func->common.scope)) {
				zend_error(E_WARNING, "Cannot bind function %s::%s to scope class %s", func->common.scope->name->val, func->common.function_name->val, scope->name->val);
				scope = NULL;
			}
			if(scope && this_ptr && (func->common.fn_flags & ZEND_ACC_STATIC) == 0 &&
					!instanceof_function(Z_OBJCE_P(this_ptr), closure->func.common.scope)) {
				zend_error(E_WARNING, "Cannot bind function %s::%s to object of class %s", func->common.scope->name->val, func->common.function_name->val, Z_OBJCE_P(this_ptr)->name->val);
				scope = NULL;
				this_ptr = NULL;
			}
		} else {
			/* if it's a free function, we won't set scope & this since they're meaningless */
			this_ptr = NULL;
			scope = NULL;
		}
	}

	ZVAL_UNDEF(&closure->this_ptr);
	/* Invariants:
	 * If the closure is unscoped, it has no bound object.
	 * The the closure is scoped, it's either static or it's bound */
	closure->func.common.scope = scope;
	if (scope) {
		closure->func.common.fn_flags |= ZEND_ACC_PUBLIC;
		if (this_ptr && Z_TYPE_P(this_ptr) == IS_OBJECT && (closure->func.common.fn_flags & ZEND_ACC_STATIC) == 0) {
			ZVAL_COPY(&closure->this_ptr, this_ptr);
		} else {
			closure->func.common.fn_flags |= ZEND_ACC_STATIC;
		}
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
