/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2014 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend.h"
#include "zend_globals.h"
#include "zend_variables.h"
#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"

ZEND_API void zend_object_std_init(zend_object *object, zend_class_entry *ce TSRMLS_DC)
{
	object->gc.refcount = 1;
	object->gc.u.v.type = IS_OBJECT;
	object->gc.u.v.flags = 0;
	object->gc.u.v.gc_info = 0;
	object->ce = ce;
	object->properties = NULL;
	object->guards = NULL;
	memset(object->properties_table, 0, sizeof(zval) * ce->default_properties_count);
	zend_objects_store_put(object);
}

ZEND_API void zend_object_std_dtor(zend_object *object TSRMLS_DC)
{
	int i;

	if (object->guards) {
		zend_hash_destroy(object->guards);
		FREE_HASHTABLE(object->guards);
	}
	if (object->properties) {
		zend_hash_destroy(object->properties);
		FREE_HASHTABLE(object->properties);
	}
	for (i = 0; i < object->ce->default_properties_count; i++) {
		zval_ptr_dtor(&object->properties_table[i]);
	}
}

ZEND_API void zend_objects_destroy_object(zend_object *object TSRMLS_DC)
{
	zend_function *destructor = object ? object->ce->destructor : NULL;

	if (destructor) {
		zend_object *old_exception;
		zval obj;

		if (destructor->op_array.fn_flags & (ZEND_ACC_PRIVATE|ZEND_ACC_PROTECTED)) {
			if (destructor->op_array.fn_flags & ZEND_ACC_PRIVATE) {
				/* Ensure that if we're calling a private function, we're allowed to do so.
				 */
				if (object->ce != EG(scope)) {
					zend_class_entry *ce = object->ce;

					zend_error(EG(in_execution) ? E_ERROR : E_WARNING,
						"Call to private %s::__destruct() from context '%s'%s",
						ce->name->val,
						EG(scope) ? EG(scope)->name->val : "",
						EG(in_execution) ? "" : " during shutdown ignored");
					return;
				}
			} else {
				/* Ensure that if we're calling a protected function, we're allowed to do so.
				 */
				if (!zend_check_protected(zend_get_function_root_class(destructor), EG(scope))) {
					zend_class_entry *ce = object->ce;

					zend_error(EG(in_execution) ? E_ERROR : E_WARNING,
						"Call to protected %s::__destruct() from context '%s'%s",
						ce->name->val,
						EG(scope) ? EG(scope)->name->val : "",
						EG(in_execution) ? "" : " during shutdown ignored");
					return;
				}
			}
		}

		ZVAL_OBJ(&obj, object);
		Z_ADDREF(obj);

		/* Make sure that destructors are protected from previously thrown exceptions.
		 * For example, if an exception was thrown in a function and when the function's
		 * local variable destruction results in a destructor being called.
		 */
		old_exception = NULL;
		if (EG(exception)) {
			if (EG(exception) == object) {
				zend_error(E_ERROR, "Attempt to destruct pending exception");
			} else {
				old_exception = EG(exception);
				EG(exception) = NULL;
			}
		}
		zend_call_method_with_0_params(&obj, object->ce, &destructor, ZEND_DESTRUCTOR_FUNC_NAME, NULL);
		if (old_exception) {
			if (EG(exception)) {
				zend_exception_set_previous(EG(exception), old_exception TSRMLS_CC);
			} else {
				EG(exception) = old_exception;
			}
		}
		zval_ptr_dtor(&obj);
	}
}

ZEND_API void zend_object_free(zend_object *object TSRMLS_DC)
{
	zend_object_std_dtor(object TSRMLS_CC);
	GC_REMOVE_FROM_BUFFER(object);
	efree(object);
}

ZEND_API zend_object *zend_objects_new(zend_class_entry *ce TSRMLS_DC)
{
	zend_object *object = emalloc(sizeof(zend_object) + sizeof(zval) * (ce->default_properties_count - 1));

	zend_object_std_init(object, ce);
	object->handlers = &std_object_handlers;
	return object;
}

ZEND_API void zend_objects_clone_members(zend_object *new_object, zend_object *old_object TSRMLS_DC)
{
	int i;

	if (old_object->ce->default_properties_count) {
		for (i = 0; i < old_object->ce->default_properties_count; i++) {
			zval_ptr_dtor(&new_object->properties_table[i]);
			ZVAL_COPY(&new_object->properties_table[i], &old_object->properties_table[i]);
		}
	}
	if (old_object->properties) {
		HashPosition pos;
		zval *prop, new_prop;
		ulong num_key;
		zend_string *key;

		if (!new_object->properties) {
			ALLOC_HASHTABLE(new_object->properties);
			zend_hash_init(new_object->properties, 0, NULL, ZVAL_PTR_DTOR, 0);
		}

		for (zend_hash_internal_pointer_reset_ex(old_object->properties, &pos);
		     (prop = zend_hash_get_current_data_ex(old_object->properties, &pos)) != NULL;
		     zend_hash_move_forward_ex(old_object->properties, &pos)) {
			if (Z_TYPE_P(prop) == IS_INDIRECT) {
				ZVAL_INDIRECT(&new_prop, new_object->properties_table + (Z_INDIRECT_P(prop) - old_object->properties_table));
			} else {
				ZVAL_COPY_VALUE(&new_prop, prop);
				zval_add_ref_unref(&new_prop);
			}
			switch (zend_hash_get_current_key_ex(old_object->properties, &key, &num_key, 0, &pos)) {
				case HASH_KEY_IS_STRING:
					zend_hash_update(new_object->properties, key, &new_prop);
					break;
				case HASH_KEY_IS_LONG:
					zend_hash_index_update(new_object->properties, num_key, &new_prop);
					break;
			}
		}
	}

	if (old_object->ce->clone) {
		zval new_obj;

		ZVAL_OBJ(&new_obj, new_object);
		zval_copy_ctor(&new_obj);
		zend_call_method_with_0_params(&new_obj, old_object->ce, &old_object->ce->clone, ZEND_CLONE_FUNC_NAME, NULL);
		zval_ptr_dtor(&new_obj);
	}
}

ZEND_API zend_object *zend_objects_clone_obj(zval *zobject TSRMLS_DC)
{
	zend_object *old_object;
	zend_object *new_object;

	/* assume that create isn't overwritten, so when clone depends on the
	 * overwritten one then it must itself be overwritten */
	old_object = Z_OBJ_P(zobject);
	new_object = zend_objects_new(old_object->ce TSRMLS_CC);

	zend_objects_clone_members(new_object, old_object TSRMLS_CC);

	return new_object;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
