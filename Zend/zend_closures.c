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
	zend_error(E_RECOVERABLE_ERROR, "Closure object cannot have properties")

typedef struct _zend_closure {
	zend_object    std;
	zend_function  func;
	zval          *this_ptr;
	HashTable     *debug_info;
} zend_closure;

/* non-static since it needs to be referenced */
ZEND_API zend_class_entry *zend_ce_closure;
static zend_object_handlers closure_handlers;

ZEND_METHOD(Closure, __invoke) /* {{{ */
{
	zend_function *func = EG(current_execute_data)->function_state.function;
	zval ***arguments;
	zval *closure_result_ptr = NULL;

	arguments = emalloc(sizeof(zval**) * ZEND_NUM_ARGS());
	if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), arguments) == FAILURE) {
		efree(arguments);
		zend_error(E_RECOVERABLE_ERROR, "Cannot get arguments for calling closure");
		RETVAL_FALSE;
	} else if (call_user_function_ex(CG(function_table), NULL, this_ptr, &closure_result_ptr, ZEND_NUM_ARGS(), arguments, 1, NULL TSRMLS_CC) == FAILURE) {
		RETVAL_FALSE;
	} else if (closure_result_ptr) {
		zval_ptr_dtor(&return_value);
		*return_value_ptr = closure_result_ptr;
	}
	efree(arguments);

	/* destruct the function also, then - we have allocated it in get_method */
	efree((char*)func->internal_function.function_name);
	efree(func);
}
/* }}} */

/* {{{ proto Closure Closure::bind(Closure $old, object $to [, mixed $scope = "static" ] )
   Create a closure from another one and bind to another object and scope */
ZEND_METHOD(Closure, bind)
{
	zval *newthis, *zclosure, *scope_arg = NULL;
	zend_closure *closure;
	zend_class_entry *ce, **ce_p;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oo!|z", &zclosure, zend_ce_closure, &newthis, &scope_arg) == FAILURE) {
		RETURN_NULL();
	}

	closure = (zend_closure *)zend_object_store_get_object(zclosure TSRMLS_CC);

	if ((newthis != NULL) && (closure->func.common.fn_flags & ZEND_ACC_STATIC)) {
		zend_error(E_WARNING, "Cannot bind an instance to a static closure");
	}

	if (newthis == NULL && !(closure->func.common.fn_flags & ZEND_ACC_STATIC)
			&& closure->func.common.scope && closure->func.type == ZEND_INTERNAL_FUNCTION) {
		zend_error(E_WARNING, "Cannot unbind $this of internal method");
		return;
	}

	if (scope_arg != NULL) { /* scope argument was given */
		if (IS_ZEND_STD_OBJECT(*scope_arg)) {
			ce = Z_OBJCE_P(scope_arg);
		} else if (Z_TYPE_P(scope_arg) == IS_NULL) {
			ce = NULL;
		} else {
			char *class_name;
			int class_name_len;
			zval tmp_zval;
			INIT_ZVAL(tmp_zval);

			if (Z_TYPE_P(scope_arg) == IS_STRING) {
				class_name = Z_STRVAL_P(scope_arg);
				class_name_len = Z_STRLEN_P(scope_arg);
			} else {
				tmp_zval = *scope_arg;
				zval_copy_ctor(&tmp_zval);
				convert_to_string(&tmp_zval);
				class_name = Z_STRVAL(tmp_zval);
				class_name_len = Z_STRLEN(tmp_zval);
			}

			if ((class_name_len == sizeof("static") - 1) &&
				(memcmp("static", class_name, sizeof("static") - 1) == 0)) {
				ce = closure->func.common.scope;
			}
			else if (zend_lookup_class_ex(class_name, class_name_len, NULL, 1, &ce_p TSRMLS_CC) == FAILURE) {
				zend_error(E_WARNING, "Class '%s' not found", class_name);
				zval_dtor(&tmp_zval);
				RETURN_NULL();
			} else {
				ce = *ce_p;
			}
			zval_dtor(&tmp_zval);
		}
	} else { /* scope argument not given; do not change the scope by default */
		ce = closure->func.common.scope;
	}

	/* verify that we aren't binding internal function to a wrong scope */
	if (closure->func.type == ZEND_INTERNAL_FUNCTION && closure->func.common.scope != NULL) {
		if (ce && !instanceof_function(ce, closure->func.common.scope TSRMLS_CC)) {
			zend_error(E_WARNING, "Cannot bind function %s::%s to scope class %s", closure->func.common.scope->name, closure->func.common.function_name, ce->name);
			return;
		}
		if (ce && newthis && (closure->func.common.fn_flags & ZEND_ACC_STATIC) == 0 &&
				!instanceof_function(Z_OBJCE_P(newthis), closure->func.common.scope TSRMLS_CC)) {
			zend_error(E_WARNING, "Cannot bind internal method %s::%s() to object of class %s", closure->func.common.scope->name, closure->func.common.function_name, Z_OBJCE_P(newthis)->name);
			return;
		}
	}

	zend_create_closure(return_value, &closure->func, ce, newthis TSRMLS_CC);
}
/* }}} */

static zend_function *zend_closure_get_constructor(zval *object TSRMLS_DC) /* {{{ */
{
	zend_error(E_RECOVERABLE_ERROR, "Instantiation of 'Closure' is not allowed");
	return NULL;
}
/* }}} */

static int zend_closure_compare_objects(zval *o1, zval *o2 TSRMLS_DC) /* {{{ */
{
	return (Z_OBJ_HANDLE_P(o1) != Z_OBJ_HANDLE_P(o2));
}
/* }}} */

ZEND_API zend_function *zend_get_closure_invoke_method(zval *obj TSRMLS_DC) /* {{{ */
{
	zend_closure *closure = (zend_closure *)zend_object_store_get_object(obj TSRMLS_CC);
	zend_function *invoke = (zend_function*)emalloc(sizeof(zend_function));
	const zend_uint keep_flags = ZEND_ACC_RETURN_REFERENCE | ZEND_ACC_VARIADIC;

	invoke->common = closure->func.common;
	invoke->type = ZEND_INTERNAL_FUNCTION;
	invoke->internal_function.fn_flags =
		ZEND_ACC_PUBLIC | ZEND_ACC_CALL_VIA_HANDLER | (closure->func.common.fn_flags & keep_flags);
	invoke->internal_function.handler = ZEND_MN(Closure___invoke);
	invoke->internal_function.module = 0;
	invoke->internal_function.scope = zend_ce_closure;
	invoke->internal_function.function_name = estrndup(ZEND_INVOKE_FUNC_NAME, sizeof(ZEND_INVOKE_FUNC_NAME)-1);
	return invoke;
}
/* }}} */

ZEND_API const zend_function *zend_get_closure_method_def(zval *obj TSRMLS_DC) /* {{{ */
{
	zend_closure *closure = (zend_closure *)zend_object_store_get_object(obj TSRMLS_CC);
	return &closure->func;
}
/* }}} */

ZEND_API zval* zend_get_closure_this_ptr(zval *obj TSRMLS_DC) /* {{{ */
{
	zend_closure *closure = (zend_closure *)zend_object_store_get_object(obj TSRMLS_CC);
	return closure->this_ptr;
}
/* }}} */

static zend_function *zend_closure_get_method(zval **object_ptr, char *method_name, int method_len, const zend_literal *key TSRMLS_DC) /* {{{ */
{
	char *lc_name;
	ALLOCA_FLAG(use_heap)

	lc_name = do_alloca(method_len + 1, use_heap);
	zend_str_tolower_copy(lc_name, method_name, method_len);
	if ((method_len == sizeof(ZEND_INVOKE_FUNC_NAME)-1) &&
		memcmp(lc_name, ZEND_INVOKE_FUNC_NAME, sizeof(ZEND_INVOKE_FUNC_NAME)-1) == 0
	) {
		free_alloca(lc_name, use_heap);
		return zend_get_closure_invoke_method(*object_ptr TSRMLS_CC);
	}
	free_alloca(lc_name, use_heap);
	return std_object_handlers.get_method(object_ptr, method_name, method_len, key TSRMLS_CC);
}
/* }}} */

static zval *zend_closure_read_property(zval *object, zval *member, int type, const zend_literal *key TSRMLS_DC) /* {{{ */
{
	ZEND_CLOSURE_PROPERTY_ERROR();
	Z_ADDREF(EG(uninitialized_zval));
	return &EG(uninitialized_zval);
}
/* }}} */

static void zend_closure_write_property(zval *object, zval *member, zval *value, const zend_literal *key TSRMLS_DC) /* {{{ */
{
	ZEND_CLOSURE_PROPERTY_ERROR();
}
/* }}} */

static zval **zend_closure_get_property_ptr_ptr(zval *object, zval *member, int type, const zend_literal *key TSRMLS_DC) /* {{{ */
{
	ZEND_CLOSURE_PROPERTY_ERROR();
	return NULL;
}
/* }}} */

static int zend_closure_has_property(zval *object, zval *member, int has_set_exists, const zend_literal *key TSRMLS_DC) /* {{{ */
{
	if (has_set_exists != 2) {
		ZEND_CLOSURE_PROPERTY_ERROR();
	}
	return 0;
}
/* }}} */

static void zend_closure_unset_property(zval *object, zval *member, const zend_literal *key TSRMLS_DC) /* {{{ */
{
	ZEND_CLOSURE_PROPERTY_ERROR();
}
/* }}} */

static void zend_closure_free_storage(void *object TSRMLS_DC) /* {{{ */
{
	zend_closure *closure = (zend_closure *)object;

	zend_object_std_dtor(&closure->std TSRMLS_CC);

	if (closure->func.type == ZEND_USER_FUNCTION) {
		zend_execute_data *ex = EG(current_execute_data);
		while (ex) {
			if (ex->op_array == &closure->func.op_array) {
				zend_error(E_ERROR, "Cannot destroy active lambda function");
			}
			ex = ex->prev_execute_data;
		}
		destroy_op_array(&closure->func.op_array TSRMLS_CC);
	}

	if (closure->debug_info != NULL) {
		zend_hash_destroy(closure->debug_info);
		efree(closure->debug_info);
	}

	if (closure->this_ptr) {
		zval_ptr_dtor(&closure->this_ptr);
	}

	efree(closure);
}
/* }}} */

static zend_object_value zend_closure_new(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
	zend_closure *closure;
	zend_object_value object;

	closure = emalloc(sizeof(zend_closure));
	memset(closure, 0, sizeof(zend_closure));

	zend_object_std_init(&closure->std, class_type TSRMLS_CC);

	object.handle = zend_objects_store_put(closure, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t) zend_closure_free_storage, NULL TSRMLS_CC);
	object.handlers = &closure_handlers;

	return object;
}
/* }}} */

static zend_object_value zend_closure_clone(zval *zobject TSRMLS_DC) /* {{{ */
{
	zend_closure *closure = (zend_closure *)zend_object_store_get_object(zobject TSRMLS_CC);
	zval result;

	zend_create_closure(&result, &closure->func, closure->func.common.scope, closure->this_ptr TSRMLS_CC);
	return Z_OBJVAL(result);
}
/* }}} */

int zend_closure_get_closure(zval *obj, zend_class_entry **ce_ptr, zend_function **fptr_ptr, zval **zobj_ptr TSRMLS_DC) /* {{{ */
{
	zend_closure *closure;

	if (Z_TYPE_P(obj) != IS_OBJECT) {
		return FAILURE;
	}

	closure = (zend_closure *)zend_object_store_get_object(obj TSRMLS_CC);
	*fptr_ptr = &closure->func;

	if (closure->this_ptr) {
		if (zobj_ptr) {
			*zobj_ptr = closure->this_ptr;
		}
		*ce_ptr = Z_OBJCE_P(closure->this_ptr);
	} else {
		if (zobj_ptr) {
			*zobj_ptr = NULL;
		}
		*ce_ptr = closure->func.common.scope;
	}
	return SUCCESS;
}
/* }}} */

static HashTable *zend_closure_get_debug_info(zval *object, int *is_temp TSRMLS_DC) /* {{{ */
{
	zend_closure *closure = (zend_closure *)zend_object_store_get_object(object TSRMLS_CC);
	zval *val;
	struct _zend_arg_info *arg_info = closure->func.common.arg_info;

	*is_temp = 0;

	if (closure->debug_info == NULL) {
		ALLOC_HASHTABLE(closure->debug_info);
		zend_hash_init(closure->debug_info, 1, NULL, ZVAL_PTR_DTOR, 0);
	}
	if (closure->debug_info->nApplyCount == 0) {
		if (closure->func.type == ZEND_USER_FUNCTION && closure->func.op_array.static_variables) {
			HashTable *static_variables = closure->func.op_array.static_variables;
			MAKE_STD_ZVAL(val);
			array_init(val);
			zend_hash_copy(Z_ARRVAL_P(val), static_variables, (copy_ctor_func_t)zval_add_ref, NULL, sizeof(zval*));
			zend_hash_update(closure->debug_info, "static", sizeof("static"), (void *) &val, sizeof(zval *), NULL);
		}

		if (closure->this_ptr) {
			Z_ADDREF_P(closure->this_ptr);
			zend_symtable_update(closure->debug_info, "this", sizeof("this"), (void *) &closure->this_ptr, sizeof(zval *), NULL);
		}

		if (arg_info) {
			zend_uint i, required = closure->func.common.required_num_args;

			MAKE_STD_ZVAL(val);
			array_init(val);

			for (i = 0; i < closure->func.common.num_args; i++) {
				char *name, *info;
				int name_len, info_len;
				if (arg_info->name) {
					name_len = zend_spprintf(&name, 0, "%s$%s",
									arg_info->pass_by_reference ? "&" : "",
									arg_info->name);
				} else {
					name_len = zend_spprintf(&name, 0, "%s$param%d",
									arg_info->pass_by_reference ? "&" : "",
									i + 1);
				}
				info_len = zend_spprintf(&info, 0, "%s",
								i >= required ? "<optional>" : "<required>");
				add_assoc_stringl_ex(val, name, name_len + 1, info, info_len, 0);
				efree(name);
				arg_info++;
			}
			zend_hash_update(closure->debug_info, "parameter", sizeof("parameter"), (void *) &val, sizeof(zval *), NULL);
		}
	}

	return closure->debug_info;
}
/* }}} */

static HashTable *zend_closure_get_gc(zval *obj, zval ***table, int *n TSRMLS_DC) /* {{{ */
{
	zend_closure *closure = (zend_closure *)zend_object_store_get_object(obj TSRMLS_CC);

	*table = closure->this_ptr ? &closure->this_ptr : NULL;
	*n = closure->this_ptr ? 1 : 0;
	return (closure->func.type == ZEND_USER_FUNCTION) ?
		closure->func.op_array.static_variables : NULL;
}
/* }}} */

/* {{{ proto Closure::__construct()
   Private constructor preventing instantiation */
ZEND_METHOD(Closure, __construct)
{
	zend_error(E_RECOVERABLE_ERROR, "Instantiation of 'Closure' is not allowed");
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

static const zend_function_entry closure_functions[] = {
	ZEND_ME(Closure, __construct, NULL, ZEND_ACC_PRIVATE)
	ZEND_ME(Closure, bind, arginfo_closure_bind, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_MALIAS(Closure, bindTo, bind, arginfo_closure_bindto, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

void zend_register_closure_ce(TSRMLS_D) /* {{{ */
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "Closure", closure_functions);
	zend_ce_closure = zend_register_internal_class(&ce TSRMLS_CC);
	zend_ce_closure->ce_flags |= ZEND_ACC_FINAL_CLASS;
	zend_ce_closure->create_object = zend_closure_new;
	zend_ce_closure->serialize = zend_class_serialize_deny;
	zend_ce_closure->unserialize = zend_class_unserialize_deny;

	memcpy(&closure_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
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

ZEND_API void zend_create_closure(zval *res, zend_function *func, zend_class_entry *scope, zval *this_ptr TSRMLS_DC) /* {{{ */
{
	zend_closure *closure;

	object_init_ex(res, zend_ce_closure);

	closure = (zend_closure *)zend_object_store_get_object(res TSRMLS_CC);

	closure->func = *func;
	closure->func.common.prototype = NULL;
	closure->func.common.fn_flags |= ZEND_ACC_CLOSURE;

	if ((scope == NULL) && (this_ptr != NULL)) {
		/* use dummy scope if we're binding an object without specifying a scope */
		/* maybe it would be better to create one for this purpose */
		scope = zend_ce_closure;
	}

	if (closure->func.type == ZEND_USER_FUNCTION) {
		if (closure->func.op_array.static_variables) {
			HashTable *static_variables = closure->func.op_array.static_variables;

			ALLOC_HASHTABLE(closure->func.op_array.static_variables);
			zend_hash_init(closure->func.op_array.static_variables, zend_hash_num_elements(static_variables), NULL, ZVAL_PTR_DTOR, 0);
			zend_hash_apply_with_arguments(static_variables TSRMLS_CC, (apply_func_args_t)zval_copy_static_var, 1, closure->func.op_array.static_variables);
		}
		closure->func.op_array.run_time_cache = NULL;
		(*closure->func.op_array.refcount)++;
	} else {
		if (!func->common.scope) {
			/* if it's a free function, we won't set scope & this since they're meaningless */
			this_ptr = NULL;
			scope = NULL;
		}
	}

	closure->this_ptr = NULL;
	/* Invariants:
	 * If the closure is unscoped, it has no bound object.
	 * The the closure is scoped, it's either static or it's bound */
	closure->func.common.scope = scope;
	if (scope) {
		closure->func.common.fn_flags |= ZEND_ACC_PUBLIC;
		if (this_ptr && (closure->func.common.fn_flags & ZEND_ACC_STATIC) == 0) {
			closure->this_ptr = this_ptr;
			Z_ADDREF_P(this_ptr);
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
