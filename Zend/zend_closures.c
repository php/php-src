/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2008 Zend Technologies Ltd. (http://www.zend.com) |
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
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend.h"
#include "zend_API.h"
#include "zend_closures.h"
#include "zend_objects.h"
#include "zend_objects_API.h"
#include "zend_globals.h"

typedef struct _zend_closure {
	zend_object    std;
	zend_function  func;
	zval          *this_ptr;
} zend_closure;

static zend_class_entry *zend_ce_closure;
static zend_object_handlers closure_handlers;

ZEND_METHOD(Closure, __invoke) /* {{{ */
{
	zval ***arguments;
	zval *closure_result_ptr = NULL;

	arguments = emalloc(sizeof(zval**) * ZEND_NUM_ARGS());
	if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), arguments) == FAILURE) {
		efree(arguments);
		zend_error(E_ERROR, "Cannot get arguments for calling closure");
		RETURN_FALSE;
	}

	if (!return_value_ptr) {
		return_value_ptr = &closure_result_ptr;
	}

	if (call_user_function_ex(CG(function_table), NULL, this_ptr, return_value_ptr, ZEND_NUM_ARGS(), arguments, 1, NULL TSRMLS_CC) == FAILURE) {
		efree(arguments);
		RETURN_FALSE;
	}

	efree(arguments);
	if (closure_result_ptr) {
		RETVAL_ZVAL(closure_result_ptr, 1, 1);
	}
}
/* }}} */

const static zend_function_entry closure_functions[] = { /* {{{ */
	ZEND_ME(Closure, __invoke, NULL, 0)
	{NULL, NULL, NULL}
};
/* }}} */

static zend_function *zend_closure_get_constructor(zval *object TSRMLS_DC) /* {{{ */
{
	zend_error(E_ERROR, "Instantiation of 'Closure' is not allowed");
	return NULL;
}
/* }}} */

static int zend_closure_compare_objects(zval *o1, zval *o2 TSRMLS_DC) /* {{{ */
{
	return (Z_OBJ_HANDLE_P(o1) != Z_OBJ_HANDLE_P(o2));
}
/* }}} */

static int zend_closure_cast_object_tostring(zval *readobj, zval *writeobj, int type, void *extra TSRMLS_DC) /* {{{ */
{
	zend_class_entry *ce;

	switch (type) {
		case IS_STRING:
		case IS_UNICODE:
			INIT_PZVAL(writeobj);
			ZVAL_ASCII_STRINGL(writeobj, "Closure object", sizeof("Closure object")-1, ZSTR_DUPLICATE);
			return SUCCESS;
		default:
			ce = Z_OBJCE_P(readobj);
			zend_error(E_NOTICE, "Object of class %v could not be converted to %s", ce->name, zend_get_type_by_const(type));
			INIT_PZVAL(writeobj);
			Z_TYPE_P(writeobj) = IS_NULL;
			break;
	}
	return FAILURE;
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

void zend_register_closure_ce(TSRMLS_D) /* {{{ */
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "Closure", closure_functions);
	zend_ce_closure = zend_register_internal_class(&ce TSRMLS_CC);
	zend_ce_closure->ce_flags |= ZEND_ACC_FINAL_CLASS;
	zend_ce_closure->create_object = zend_closure_new;

	memcpy(&closure_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	closure_handlers.get_constructor = zend_closure_get_constructor;
	closure_handlers.compare_objects = zend_closure_compare_objects;
	closure_handlers.cast_object = zend_closure_cast_object_tostring;
	closure_handlers.clone_obj = NULL;
}
/* }}} */

static int zval_copy_static_var(zval **p, int num_args, va_list args, zend_hash_key *key) /* {{{ */
{
	HashTable *target = va_arg(args, HashTable*);
	zend_bool is_ref;
	TSRMLS_FETCH();
	
	if (Z_TYPE_PP(p) & (IS_LEXICAL_VAR|IS_LEXICAL_REF)) {
		TSRMLS_FETCH();
		is_ref = Z_TYPE_PP(p) & IS_LEXICAL_REF;

		if (!EG(active_symbol_table)) {
			zend_rebuild_symbol_table(TSRMLS_C);
		}
		if (zend_u_hash_quick_find(EG(active_symbol_table), key->type, key->arKey, key->nKeyLength, key->h, (void **) &p) == FAILURE) {
			if (is_ref) {
				zval *tmp;

				ALLOC_INIT_ZVAL(tmp);
				Z_SET_ISREF_P(tmp);
				zend_u_hash_quick_add(EG(active_symbol_table), key->type, key->arKey, key->nKeyLength, key->h, &tmp, sizeof(zval*), (void**)&p);
			} else {
				p = &EG(uninitialized_zval_ptr);
				zend_error(E_NOTICE,"Undefined variable: %s", key->arKey);
			}
		} else {
			if (is_ref) {
				SEPARATE_ZVAL_TO_MAKE_IS_REF(p);
			} else if (Z_ISREF_PP(p)) {
				SEPARATE_ZVAL(p);
			}
		}
	}
	if (zend_u_hash_quick_add(target, key->type, key->arKey, key->nKeyLength, key->h, p, sizeof(zval*), NULL) == SUCCESS) {
		Z_ADDREF_PP(p);
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

ZEND_API void zend_create_closure(zval *res, zend_function *func, zend_class_entry *scope, zval *this_ptr TSRMLS_DC) /* {{{ */
{
	zend_closure *closure;

	object_init_ex(res, zend_ce_closure);

	closure = (zend_closure *)zend_object_store_get_object(res TSRMLS_CC);

	closure->func = *func;

	if (closure->func.type == ZEND_USER_FUNCTION) {
		if (closure->func.op_array.static_variables) {
			HashTable *static_variables = closure->func.op_array.static_variables;
	
			ALLOC_HASHTABLE(closure->func.op_array.static_variables);
			zend_u_hash_init(closure->func.op_array.static_variables, zend_hash_num_elements(static_variables), NULL, ZVAL_PTR_DTOR, 0, UG(unicode));
			zend_hash_apply_with_arguments(static_variables, (apply_func_args_t)zval_copy_static_var, 1, closure->func.op_array.static_variables);
		}
		(*closure->func.op_array.refcount)++;
	}

	closure->func.common.scope = scope;
	if (scope) {
		closure->func.common.fn_flags |= ZEND_ACC_PUBLIC;
		if (this_ptr && (closure->func.common.fn_flags & ZEND_ACC_STATIC) == 0) {
			closure->this_ptr = this_ptr;
			Z_ADDREF_P(this_ptr);
		} else {
			closure->this_ptr = NULL;
		}
	} else {
		closure->this_ptr = NULL;
	}
}
/* }}} */

ZEND_API int zend_get_closure(zval *obj, zend_class_entry **ce_ptr, zend_function **fptr_ptr, zval **zobj_ptr, zval ***zobj_ptr_ptr TSRMLS_DC) /* {{{ */
{
	zstr key;
	zend_uchar utype = UG(unicode)?IS_UNICODE:IS_STRING;

	if (utype == IS_UNICODE) {
		key.u = USTR_MAKE("__invoke");
	} else {
		key.s = "__invoke";
	}

	if (Z_TYPE_P(obj) == IS_OBJECT) {
		zend_class_entry *ce = Z_OBJCE_P(obj);

		if (ce == zend_ce_closure) {
			zend_closure *closure = (zend_closure *)zend_object_store_get_object(obj TSRMLS_CC);

			*fptr_ptr = &closure->func;
			if (closure->this_ptr) {
				if (zobj_ptr) {
					*zobj_ptr = closure->this_ptr;
				}
				if (zobj_ptr_ptr) {
					*zobj_ptr_ptr = &closure->this_ptr;
				}
				*ce_ptr = Z_OBJCE_P(closure->this_ptr);
			} else {
				if (zobj_ptr) {
					*zobj_ptr = NULL;
				}
				if (zobj_ptr_ptr) {
					*zobj_ptr_ptr = NULL;
				}
				*ce_ptr = closure->func.common.scope;
			}
			if (utype == IS_UNICODE) {
				efree(key.u);
			}
			return SUCCESS;
		} else if (zend_u_hash_find(&ce->function_table, utype, key, sizeof("__invoke"), (void**)fptr_ptr) == SUCCESS) {
			*ce_ptr = ce;
			if ((*fptr_ptr)->common.fn_flags & ZEND_ACC_STATIC) {
				if (zobj_ptr) {
					*zobj_ptr = NULL;
				}
				if (zobj_ptr_ptr) {
					*zobj_ptr_ptr = NULL;
				}
			} else {
				if (zobj_ptr) {
					*zobj_ptr = obj;
				}
				if (zobj_ptr_ptr) {
					*zobj_ptr_ptr = NULL;
				}
			}
			if (utype == IS_UNICODE) {
				efree(key.u);
			}
			return SUCCESS;
		}
	}
	if (utype == IS_UNICODE) {
		efree(key.u);
	}
	return FAILURE;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
