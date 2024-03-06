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
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_globals.h"
#include "zend_variables.h"
#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"
#include "zend_weakrefs.h"

static zend_always_inline void _zend_object_std_init(zend_object *object, zend_class_entry *ce)
{
	GC_SET_REFCOUNT(object, 1);
	GC_TYPE_INFO(object) = GC_OBJECT;
	object->ce = ce;
	object->properties = NULL;
	zend_objects_store_put(object);
	if (UNEXPECTED(ce->ce_flags & ZEND_ACC_USE_GUARDS)) {
		ZVAL_UNDEF(object->properties_table + object->ce->default_properties_count);
	}
}

ZEND_API void ZEND_FASTCALL zend_object_std_init(zend_object *object, zend_class_entry *ce)
{
	_zend_object_std_init(object, ce);
}

ZEND_API void zend_object_std_dtor(zend_object *object)
{
	zval *p, *end;

	if (UNEXPECTED(GC_FLAGS(object) & IS_OBJ_WEAKLY_REFERENCED)) {
		zend_weakrefs_notify(object);
	}

	if (object->properties) {
		if (EXPECTED(!(GC_FLAGS(object->properties) & IS_ARRAY_IMMUTABLE))) {
			if (EXPECTED(GC_DELREF(object->properties) == 0)
					&& EXPECTED(GC_TYPE(object->properties) != IS_NULL)) {
				zend_array_destroy(object->properties);
			}
		}
	}
	p = object->properties_table;
	if (EXPECTED(object->ce->default_properties_count)) {
		end = p + object->ce->default_properties_count;
		do {
			if (Z_REFCOUNTED_P(p)) {
				if (UNEXPECTED(Z_ISREF_P(p)) &&
						(ZEND_DEBUG || ZEND_REF_HAS_TYPE_SOURCES(Z_REF_P(p)))) {
					zend_property_info *prop_info = zend_get_property_info_for_slot(object, p);
					if (ZEND_TYPE_IS_SET(prop_info->type)) {
						ZEND_REF_DEL_TYPE_SOURCE(Z_REF_P(p), prop_info);
					}
				}
				i_zval_ptr_dtor(p);
			}
			p++;
		} while (p != end);
	}

	if (UNEXPECTED(object->ce->ce_flags & ZEND_ACC_USE_GUARDS)) {
		if (EXPECTED(Z_TYPE_P(p) == IS_STRING)) {
			zval_ptr_dtor_str(p);
		} else if (Z_TYPE_P(p) == IS_ARRAY) {
			HashTable *guards;

			guards = Z_ARRVAL_P(p);
			ZEND_ASSERT(guards != NULL);
			zend_hash_destroy(guards);
			FREE_HASHTABLE(guards);
		}
	}
}

ZEND_API void zend_objects_destroy_object(zend_object *object)
{
	zend_function *destructor = object->ce->destructor;

	if (destructor) {
		zend_object *old_exception;
		const zend_op *old_opline_before_exception;

		if (destructor->op_array.fn_flags & (ZEND_ACC_PRIVATE|ZEND_ACC_PROTECTED)) {
			if (destructor->op_array.fn_flags & ZEND_ACC_PRIVATE) {
				/* Ensure that if we're calling a private function, we're allowed to do so.
				 */
				if (EG(current_execute_data)) {
					zend_class_entry *scope = zend_get_executed_scope();

					if (object->ce != scope) {
						zend_throw_error(NULL,
							"Call to private %s::__destruct() from %s%s",
							ZSTR_VAL(object->ce->name),
							scope ? "scope " : "global scope",
							scope ? ZSTR_VAL(scope->name) : ""
						);
						return;
					}
				} else {
					zend_error(E_WARNING,
						"Call to private %s::__destruct() from global scope during shutdown ignored",
						ZSTR_VAL(object->ce->name));
					return;
				}
			} else {
				/* Ensure that if we're calling a protected function, we're allowed to do so.
				 */
				if (EG(current_execute_data)) {
					zend_class_entry *scope = zend_get_executed_scope();

					if (!zend_check_protected(zend_get_function_root_class(destructor), scope)) {
						zend_throw_error(NULL,
							"Call to protected %s::__destruct() from %s%s",
							ZSTR_VAL(object->ce->name),
							scope ? "scope " : "global scope",
							scope ? ZSTR_VAL(scope->name) : ""
						);
						return;
					}
				} else {
					zend_error(E_WARNING,
						"Call to protected %s::__destruct() from global scope during shutdown ignored",
						ZSTR_VAL(object->ce->name));
					return;
				}
			}
		}

		GC_ADDREF(object);

		/* Make sure that destructors are protected from previously thrown exceptions.
		 * For example, if an exception was thrown in a function and when the function's
		 * local variable destruction results in a destructor being called.
		 */
		old_exception = NULL;
		if (EG(exception)) {
			if (EG(exception) == object) {
				zend_error_noreturn(E_CORE_ERROR, "Attempt to destruct pending exception");
			} else {
				if (EG(current_execute_data)
				 && EG(current_execute_data)->func
				 && ZEND_USER_CODE(EG(current_execute_data)->func->common.type)) {
					zend_rethrow_exception(EG(current_execute_data));
				}
				old_exception = EG(exception);
				old_opline_before_exception = EG(opline_before_exception);
				EG(exception) = NULL;
			}
		}

		zend_call_known_instance_method_with_0_params(destructor, object, NULL);

		if (old_exception) {
			EG(opline_before_exception) = old_opline_before_exception;
			if (EG(exception)) {
				zend_exception_set_previous(EG(exception), old_exception);
			} else {
				EG(exception) = old_exception;
			}
		}
		OBJ_RELEASE(object);
	}
}

ZEND_API zend_object* ZEND_FASTCALL zend_objects_new(zend_class_entry *ce)
{
	zend_object *object = emalloc(sizeof(zend_object) + zend_object_properties_size(ce));

	_zend_object_std_init(object, ce);
	object->handlers = &std_object_handlers;
	return object;
}

ZEND_API void ZEND_FASTCALL zend_objects_clone_members(zend_object *new_object, zend_object *old_object)
{
	if (old_object->ce->default_properties_count) {
		zval *src = old_object->properties_table;
		zval *dst = new_object->properties_table;
		zval *end = src + old_object->ce->default_properties_count;

		do {
			i_zval_ptr_dtor(dst);
			ZVAL_COPY_VALUE_PROP(dst, src);
			zval_add_ref(dst);
			if (UNEXPECTED(Z_ISREF_P(dst)) &&
					(ZEND_DEBUG || ZEND_REF_HAS_TYPE_SOURCES(Z_REF_P(dst)))) {
				zend_property_info *prop_info = zend_get_property_info_for_slot(new_object, dst);
				if (ZEND_TYPE_IS_SET(prop_info->type)) {
					ZEND_REF_ADD_TYPE_SOURCE(Z_REF_P(dst), prop_info);
				}
			}
			src++;
			dst++;
		} while (src != end);
	} else if (old_object->properties && !old_object->ce->clone) {
		/* fast copy */
		if (EXPECTED(old_object->handlers == &std_object_handlers)) {
			if (EXPECTED(!(GC_FLAGS(old_object->properties) & IS_ARRAY_IMMUTABLE))) {
				GC_ADDREF(old_object->properties);
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
			new_object->properties = zend_new_array(zend_hash_num_elements(old_object->properties));
			zend_hash_real_init_mixed(new_object->properties);
		} else {
			zend_hash_extend(new_object->properties, new_object->properties->nNumUsed + zend_hash_num_elements(old_object->properties), 0);
		}

		HT_FLAGS(new_object->properties) |=
			HT_FLAGS(old_object->properties) & HASH_FLAG_HAS_EMPTY_IND;

		ZEND_HASH_MAP_FOREACH_KEY_VAL(old_object->properties, num_key, key, prop) {
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
		GC_ADDREF(new_object);
		zend_call_known_instance_method_with_0_params(new_object->ce->clone, new_object, NULL);
		OBJ_RELEASE(new_object);
	}
}

ZEND_API zend_object *zend_objects_clone_obj(zend_object *old_object)
{
	zend_object *new_object;

	/* assume that create isn't overwritten, so when clone depends on the
	 * overwritten one then it must itself be overwritten */
	new_object = zend_objects_new(old_object->ce);

	/* zend_objects_clone_members() expect the properties to be initialized. */
	if (new_object->ce->default_properties_count) {
		zval *p = new_object->properties_table;
		zval *end = p + new_object->ce->default_properties_count;
		do {
			ZVAL_UNDEF(p);
			p++;
		} while (p != end);
	}

	zend_objects_clone_members(new_object, old_object);

	return new_object;
}
