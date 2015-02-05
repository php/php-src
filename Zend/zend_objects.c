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

ZEND_API void zend_object_std_init(zend_object *object, zend_class_entry *ce)
{
	GC_REFCOUNT(object) = 1;
	GC_TYPE_INFO(object) = IS_OBJECT;
	object->ce = ce;
	object->properties = NULL;
	zend_objects_store_put(object);
	if (EXPECTED(ce->default_properties_count != 0)) {
		zval *p = object->properties_table;
		zval *end = p + ce->default_properties_count;

		do {
			ZVAL_UNDEF(p);
			p++;
		} while (p != end);
	}
	if (ce->ce_flags & ZEND_ACC_USE_GUARDS) {
		GC_FLAGS(object) |= IS_OBJ_USE_GUARDS;
		ZVAL_UNDEF(&object->properties_table[ce->default_properties_count]);
		Z_PTR(object->properties_table[ce->default_properties_count]) = NULL;
	}
}

ZEND_API void zend_object_std_dtor(zend_object *object)
{
	int i, count;

	if (object->properties) {
		zend_array_destroy(object->properties);
		FREE_HASHTABLE(object->properties);
	}
	count = object->ce->default_properties_count;
	for (i = 0; i < count; i++) {
		i_zval_ptr_dtor(&object->properties_table[i] ZEND_FILE_LINE_CC);
	}
	if (GC_FLAGS(object) & IS_OBJ_HAS_GUARDS) {
		HashTable *guards = Z_PTR(object->properties_table[count]);

		ZEND_ASSERT(guards != NULL);
		zend_hash_destroy(guards);
		FREE_HASHTABLE(guards);
	}
}

ZEND_API void zend_objects_destroy_object(zend_object *object)
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

					zend_error(EG(current_execute_data) ? E_ERROR : E_WARNING,
						"Call to private %s::__destruct() from context '%s'%s",
						ce->name->val,
						EG(scope) ? EG(scope)->name->val : "",
						EG(current_execute_data) ? "" : " during shutdown ignored");
					return;
				}
			} else {
				/* Ensure that if we're calling a protected function, we're allowed to do so.
				 */
				if (!zend_check_protected(zend_get_function_root_class(destructor), EG(scope))) {
					zend_class_entry *ce = object->ce;

					zend_error(EG(current_execute_data) ? E_ERROR : E_WARNING,
						"Call to protected %s::__destruct() from context '%s'%s",
						ce->name->val,
						EG(scope) ? EG(scope)->name->val : "",
						EG(current_execute_data) ? "" : " during shutdown ignored");
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
				zend_exception_set_previous(EG(exception), old_exception);
			} else {
				EG(exception) = old_exception;
			}
		}
		zval_ptr_dtor(&obj);
	}
}

ZEND_API zend_object *zend_objects_new(zend_class_entry *ce)
{
	zend_object *object = emalloc(sizeof(zend_object) + zend_object_properties_size(ce));

	zend_object_std_init(object, ce);
	object->handlers = &std_object_handlers;
	return object;
}

ZEND_API void zend_objects_clone_members(zend_object *new_object, zend_object *old_object)
{
	int i;

	if (old_object->ce->default_properties_count) {
		for (i = 0; i < old_object->ce->default_properties_count; i++) {
			zval_ptr_dtor(&new_object->properties_table[i]);
			ZVAL_COPY_VALUE(&new_object->properties_table[i], &old_object->properties_table[i]);
			zval_add_ref(&new_object->properties_table[i]);
		}
	}
	if (old_object->properties) {
		zval *prop, new_prop;
		zend_ulong num_key;
		zend_string *key;

		if (!new_object->properties) {
			ALLOC_HASHTABLE(new_object->properties);
			zend_hash_init(new_object->properties, zend_hash_num_elements(old_object->properties), NULL, ZVAL_PTR_DTOR, 0);
		}

		ZEND_HASH_FOREACH_KEY_VAL(old_object->properties, num_key, key, prop) {
			if (Z_TYPE_P(prop) == IS_INDIRECT) {
				ZVAL_INDIRECT(&new_prop, new_object->properties_table + (Z_INDIRECT_P(prop) - old_object->properties_table));
			} else {
				ZVAL_COPY_VALUE(&new_prop, prop);
				zval_add_ref(&new_prop);
			}
			if (key) {
				zend_hash_add_new(new_object->properties, key, &new_prop);
			} else {
				zend_hash_index_add_new(new_object->properties, num_key, &new_prop);
			}
		} ZEND_HASH_FOREACH_END();
	}

	if (old_object->ce->clone) {
		zval new_obj;

		ZVAL_OBJ(&new_obj, new_object);
		zval_copy_ctor(&new_obj);
		zend_call_method_with_0_params(&new_obj, old_object->ce, &old_object->ce->clone, ZEND_CLONE_FUNC_NAME, NULL);
		zval_ptr_dtor(&new_obj);
	}
}

ZEND_API zend_object *zend_objects_clone_obj(zval *zobject)
{
	zend_object *old_object;
	zend_object *new_object;

	/* assume that create isn't overwritten, so when clone depends on the
	 * overwritten one then it must itself be overwritten */
	old_object = Z_OBJ_P(zobject);
	new_object = zend_objects_new(old_object->ce);

	zend_objects_clone_members(new_object, old_object);

	return new_object;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
