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
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Dmitry Stogov <dmitry@zend.com>                             |
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
	zval *p, *end;

	GC_REFCOUNT(object) = 1;
	GC_TYPE_INFO(object) = IS_OBJECT;
	object->ce = ce;
	object->properties = NULL;
	zend_objects_store_put(object);
	p = object->properties_table;
	if (EXPECTED(ce->default_properties_count != 0)) {
		end = p + ce->default_properties_count;
		do {
			ZVAL_UNDEF(p);
			p++;
		} while (p != end);
	}
	if (UNEXPECTED(ce->ce_flags & ZEND_ACC_USE_GUARDS)) {
		GC_FLAGS(object) |= IS_OBJ_USE_GUARDS;
		Z_PTR_P(p) = NULL;
		ZVAL_UNDEF(p);
	}
}

ZEND_API void zend_object_std_dtor(zend_object *object)
{
	zval *p, *end;

	if (object->properties) {
		if (EXPECTED(!(GC_FLAGS(object->properties) & IS_ARRAY_IMMUTABLE))) {
			if (EXPECTED(--GC_REFCOUNT(object->properties) == 0)) {
				zend_array_destroy(object->properties);
			}
		}
	}
	p = object->properties_table;
	if (EXPECTED(object->ce->default_properties_count)) {
		end = p + object->ce->default_properties_count;
		do {
			i_zval_ptr_dtor(p ZEND_FILE_LINE_CC);
			p++;
		} while (p != end);
	}
	if (UNEXPECTED(GC_FLAGS(object) & IS_OBJ_HAS_GUARDS)) {
		HashTable *guards = Z_PTR_P(p);

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

					if (EG(current_execute_data)) {
						zend_throw_error(NULL,
							"Call to private %s::__destruct() from context '%s'",
							ZSTR_VAL(ce->name),
							EG(scope) ? ZSTR_VAL(EG(scope)->name) : "");
					} else {
						zend_error(E_WARNING,
							"Call to private %s::__destruct() from context '%s' during shutdown ignored",
							ZSTR_VAL(ce->name),
							EG(scope) ? ZSTR_VAL(EG(scope)->name) : "");
					}
					return;
				}
			} else {
				/* Ensure that if we're calling a protected function, we're allowed to do so.
				 */
				if (!zend_check_protected(zend_get_function_root_class(destructor), EG(scope))) {
					zend_class_entry *ce = object->ce;

					if (EG(current_execute_data)) {
						zend_throw_error(NULL,
							"Call to protected %s::__destruct() from context '%s'",
							ZSTR_VAL(ce->name),
							EG(scope) ? ZSTR_VAL(EG(scope)->name) : "");
					} else {
						zend_error(E_WARNING,
							"Call to protected %s::__destruct() from context '%s' during shutdown ignored",
							ZSTR_VAL(ce->name),
							EG(scope) ? ZSTR_VAL(EG(scope)->name) : "");
					}
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
				zend_error_noreturn(E_CORE_ERROR, "Attempt to destruct pending exception");
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
	if (old_object->ce->default_properties_count) {
		zval *src = old_object->properties_table;
		zval *dst = new_object->properties_table;
		zval *end = src + old_object->ce->default_properties_count;

		do {
			i_zval_ptr_dtor(dst ZEND_FILE_LINE_CC);
			ZVAL_COPY_VALUE(dst, src);
			zval_add_ref(dst);
			src++;
			dst++;
		} while (src != end);
	} else if (old_object->properties && !old_object->ce->clone) {
		/* fast copy */
		if (EXPECTED(old_object->handlers == &std_object_handlers)) {
			if (EXPECTED(!(GC_FLAGS(old_object->properties) & IS_ARRAY_IMMUTABLE))) {
				GC_REFCOUNT(old_object->properties)++;
			}
			new_object->properties = old_object->properties;
			return;
		}
	}

	if (old_object->properties &&
	    EXPECTED(zend_hash_num_elements(old_object->properties))) {
		zval *prop, new_prop;
		zend_ulong num_key;
		zend_string *key;

		if (!new_object->properties) {
			ALLOC_HASHTABLE(new_object->properties);
			zend_hash_init(new_object->properties, zend_hash_num_elements(old_object->properties), NULL, ZVAL_PTR_DTOR, 0);
			zend_hash_real_init(new_object->properties, 0);
		} else {
			zend_hash_extend(new_object->properties, new_object->properties->nNumUsed + zend_hash_num_elements(old_object->properties), 0);
		}

		new_object->properties->u.v.flags |=
			old_object->properties->u.v.flags & HASH_FLAG_HAS_EMPTY_IND;

		ZEND_HASH_FOREACH_KEY_VAL(old_object->properties, num_key, key, prop) {
			if (Z_TYPE_P(prop) == IS_INDIRECT) {
				ZVAL_INDIRECT(&new_prop, new_object->properties_table + (Z_INDIRECT_P(prop) - old_object->properties_table));
			} else {
				ZVAL_COPY_VALUE(&new_prop, prop);
				zval_add_ref(&new_prop);
			}
			if (EXPECTED(key)) {
				_zend_hash_append(new_object->properties, key, &new_prop);
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
