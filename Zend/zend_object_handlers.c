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
#include "zend_objects.h"
#include "zend_objects_API.h"
#include "zend_object_handlers.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"
#include "zend_closures.h"
#include "zend_compile.h"
#include "zend_hash.h"

#define DEBUG_OBJECT_HANDLERS 0

#define ZEND_WRONG_PROPERTY_OFFSET   0

/* guard flags */
#define IN_GET		(1<<0)
#define IN_SET		(1<<1)
#define IN_UNSET	(1<<2)
#define IN_ISSET	(1<<3)

/*
  __X accessors explanation:

  if we have __get and property that is not part of the properties array is
  requested, we call __get handler. If it fails, we return uninitialized.

  if we have __set and property that is not part of the properties array is
  set, we call __set handler. If it fails, we do not change the array.

  for both handlers above, when we are inside __get/__set, no further calls for
  __get/__set for this property of this object will be made, to prevent endless
  recursion and enable accessors to change properties array.

  if we have __call and method which is not part of the class function table is
  called, we cal __call handler.
*/

ZEND_API void rebuild_object_properties(zend_object *zobj) /* {{{ */
{
	if (!zobj->properties) {
		zend_property_info *prop_info;
		zend_class_entry *ce = zobj->ce;
		int i;

		zobj->properties = zend_new_array(ce->default_properties_count);
		if (ce->default_properties_count) {
			zend_hash_real_init_mixed(zobj->properties);
			for (i = 0; i < ce->default_properties_count; i++) {
				prop_info = ce->properties_info_table[i];

				if (!prop_info) {
					continue;
				}

				if (UNEXPECTED(Z_TYPE_P(OBJ_PROP(zobj, prop_info->offset)) == IS_UNDEF)) {
					HT_FLAGS(zobj->properties) |= HASH_FLAG_HAS_EMPTY_IND;
				}

				_zend_hash_append_ind(zobj->properties, prop_info->name,
					OBJ_PROP(zobj, prop_info->offset));
			}
		}
	}
}
/* }}} */

ZEND_API HashTable *zend_std_build_object_properties_array(zend_object *zobj) /* {{{ */
{
	zend_property_info *prop_info;
	zend_class_entry *ce = zobj->ce;
	HashTable *ht;
	zval* prop;
	int i;

	ZEND_ASSERT(!zobj->properties);
	ht = zend_new_array(ce->default_properties_count);
	if (ce->default_properties_count) {
		zend_hash_real_init_mixed(ht);
		for (i = 0; i < ce->default_properties_count; i++) {
			prop_info = ce->properties_info_table[i];

			if (!prop_info) {
				continue;
			}

			prop = OBJ_PROP(zobj, prop_info->offset);
			if (UNEXPECTED(Z_TYPE_P(prop) == IS_UNDEF)) {
				continue;
			}

			if (Z_ISREF_P(prop) && Z_REFCOUNT_P(prop) == 1) {
				prop = Z_REFVAL_P(prop);
			}

			Z_TRY_ADDREF_P(prop);
			_zend_hash_append(ht, prop_info->name, prop);
		}
	}
	return ht;
}
/* }}} */

ZEND_API HashTable *zend_std_get_properties(zend_object *zobj) /* {{{ */
{
	if (!zobj->properties) {
		rebuild_object_properties(zobj);
	}
	return zobj->properties;
}
/* }}} */

ZEND_API HashTable *zend_std_get_gc(zend_object *zobj, zval **table, int *n) /* {{{ */
{
	if (zobj->handlers->get_properties != zend_std_get_properties) {
		*table = NULL;
		*n = 0;
		return zobj->handlers->get_properties(zobj);
	} else {
		if (zobj->properties) {
			*table = NULL;
			*n = 0;
			return zobj->properties;
		} else {
			*table = zobj->properties_table;
			*n = zobj->ce->default_properties_count;
			return NULL;
		}
	}
}
/* }}} */

ZEND_API HashTable *zend_std_get_debug_info(zend_object *object, int *is_temp) /* {{{ */
{
	zend_class_entry *ce = object->ce;
	zval retval;
	HashTable *ht;

	if (!ce->__debugInfo) {
		*is_temp = 0;
		return object->handlers->get_properties(object);
	}

	zend_call_known_instance_method_with_0_params(ce->__debugInfo, object, &retval);
	if (Z_TYPE(retval) == IS_ARRAY) {
		if (!Z_REFCOUNTED(retval)) {
			*is_temp = 1;
			return zend_array_dup(Z_ARRVAL(retval));
		} else if (Z_REFCOUNT(retval) <= 1) {
			*is_temp = 1;
			ht = Z_ARR(retval);
			return ht;
		} else {
			*is_temp = 0;
			zval_ptr_dtor(&retval);
			return Z_ARRVAL(retval);
		}
	} else if (Z_TYPE(retval) == IS_NULL) {
		*is_temp = 1;
		ht = zend_new_array(0);
		return ht;
	}

	zend_error_noreturn(E_ERROR, ZEND_DEBUGINFO_FUNC_NAME "() must return an array");

	return NULL; /* Compilers are dumb and don't understand that noreturn means that the function does NOT need a return value... */
}
/* }}} */

static void zend_std_call_getter(zend_object *zobj, zend_string *prop_name, zval *retval) /* {{{ */
{
	zval member;
	ZVAL_STR(&member, prop_name);
	zend_call_known_instance_method_with_1_params(zobj->ce->__get, zobj, retval, &member);
}
/* }}} */

static void zend_std_call_setter(zend_object *zobj, zend_string *prop_name, zval *value) /* {{{ */
{
	zval args[2];
	ZVAL_STR(&args[0], prop_name);
	ZVAL_COPY_VALUE(&args[1], value);
	zend_call_known_instance_method(zobj->ce->__set, zobj, NULL, 2, args);
}
/* }}} */

static void zend_std_call_unsetter(zend_object *zobj, zend_string *prop_name) /* {{{ */
{
	zval member;
	ZVAL_STR(&member, prop_name);
	zend_call_known_instance_method_with_1_params(zobj->ce->__unset, zobj, NULL, &member);
}
/* }}} */

static void zend_std_call_issetter(zend_object *zobj, zend_string *prop_name, zval *retval) /* {{{ */
{
	zval member;
	ZVAL_STR(&member, prop_name);
	zend_call_known_instance_method_with_1_params(zobj->ce->__isset, zobj, retval, &member);
}
/* }}} */


static zend_always_inline bool is_derived_class(zend_class_entry *child_class, zend_class_entry *parent_class) /* {{{ */
{
	child_class = child_class->parent;
	while (child_class) {
		if (child_class == parent_class) {
			return 1;
		}
		child_class = child_class->parent;
	}

	return 0;
}
/* }}} */

static zend_never_inline int is_protected_compatible_scope(zend_class_entry *ce, zend_class_entry *scope) /* {{{ */
{
	return scope &&
		(is_derived_class(ce, scope) || is_derived_class(scope, ce));
}
/* }}} */

static zend_never_inline zend_property_info *zend_get_parent_private_property(zend_class_entry *scope, zend_class_entry *ce, zend_string *member) /* {{{ */
{
	zval *zv;
	zend_property_info *prop_info;

	if (scope != ce && scope && is_derived_class(ce, scope)) {
		zv = zend_hash_find(&scope->properties_info, member);
		if (zv != NULL) {
			prop_info = (zend_property_info*)Z_PTR_P(zv);
			if ((prop_info->flags & ZEND_ACC_PRIVATE)
			 && prop_info->ce == scope) {
				return prop_info;
			}
		}
	}
	return NULL;
}
/* }}} */

static ZEND_COLD zend_never_inline void zend_bad_property_access(zend_property_info *property_info, zend_class_entry *ce, zend_string *member) /* {{{ */
{
	zend_throw_error(NULL, "Cannot access %s property %s::$%s", zend_visibility_string(property_info->flags), ZSTR_VAL(ce->name), ZSTR_VAL(member));
}
/* }}} */

static ZEND_COLD zend_never_inline void zend_bad_property_name(void) /* {{{ */
{
	zend_throw_error(NULL, "Cannot access property starting with \"\\0\"");
}
/* }}} */

static ZEND_COLD zend_never_inline void zend_forbidden_dynamic_property(
		zend_class_entry *ce, zend_string *member) {
	zend_throw_error(NULL, "Cannot create dynamic property %s::$%s",
		ZSTR_VAL(ce->name), ZSTR_VAL(member));
}

static ZEND_COLD zend_never_inline bool zend_deprecated_dynamic_property(
		zend_object *obj, zend_string *member) {
	GC_ADDREF(obj);
	zend_error(E_DEPRECATED, "Creation of dynamic property %s::$%s is deprecated",
		ZSTR_VAL(obj->ce->name), ZSTR_VAL(member));
	if (UNEXPECTED(GC_DELREF(obj) == 0)) {
		zend_class_entry *ce = obj->ce;
		zend_objects_store_del(obj);
		if (!EG(exception)) {
			/* We cannot continue execution and have to throw an exception */
			zend_throw_error(NULL, "Cannot create dynamic property %s::$%s",
				ZSTR_VAL(ce->name), ZSTR_VAL(member));
		}
		return 0;
	}
	return 1;
}

static ZEND_COLD zend_never_inline void zend_readonly_property_modification_scope_error(
		zend_class_entry *ce, zend_string *member, zend_class_entry *scope, const char *operation) {
	zend_throw_error(NULL, "Cannot %s readonly property %s::$%s from %s%s",
		operation, ZSTR_VAL(ce->name), ZSTR_VAL(member),
		scope ? "scope " : "global scope", scope ? ZSTR_VAL(scope->name) : "");
}

static ZEND_COLD zend_never_inline void zend_readonly_property_unset_error(
		zend_class_entry *ce, zend_string *member) {
	zend_throw_error(NULL, "Cannot unset readonly property %s::$%s",
		ZSTR_VAL(ce->name), ZSTR_VAL(member));
}

static zend_always_inline uintptr_t zend_get_property_offset(zend_class_entry *ce, zend_string *member, int silent, void **cache_slot, zend_property_info **info_ptr) /* {{{ */
{
	zval *zv;
	zend_property_info *property_info;
	uint32_t flags;
	zend_class_entry *scope;
	uintptr_t offset;

	if (cache_slot && EXPECTED(ce == CACHED_PTR_EX(cache_slot))) {
		*info_ptr = CACHED_PTR_EX(cache_slot + 2);
		return (uintptr_t)CACHED_PTR_EX(cache_slot + 1);
	}

	if (UNEXPECTED(zend_hash_num_elements(&ce->properties_info) == 0)
	 || UNEXPECTED((zv = zend_hash_find(&ce->properties_info, member)) == NULL)) {
		if (UNEXPECTED(ZSTR_VAL(member)[0] == '\0') && ZSTR_LEN(member) != 0) {
			if (!silent) {
				zend_bad_property_name();
			}
			return ZEND_WRONG_PROPERTY_OFFSET;
		}
dynamic:
		if (cache_slot) {
			CACHE_POLYMORPHIC_PTR_EX(cache_slot, ce, (void*)ZEND_DYNAMIC_PROPERTY_OFFSET);
			CACHE_PTR_EX(cache_slot + 2, NULL);
		}
		return ZEND_DYNAMIC_PROPERTY_OFFSET;
	}

	property_info = (zend_property_info*)Z_PTR_P(zv);
	flags = property_info->flags;

	if (flags & (ZEND_ACC_CHANGED|ZEND_ACC_PRIVATE|ZEND_ACC_PROTECTED)) {
		if (UNEXPECTED(EG(fake_scope))) {
			scope = EG(fake_scope);
		} else {
			scope = zend_get_executed_scope();
		}

		if (property_info->ce != scope) {
			if (flags & ZEND_ACC_CHANGED) {
				zend_property_info *p = zend_get_parent_private_property(scope, ce, member);

				/* If there is a public/protected instance property on ce, don't try to use a
				 * private static property on scope. If both are static, prefer the static
				 * property on scope. This will throw a static property notice, rather than
				 * a visibility error. */
				if (p && (!(p->flags & ZEND_ACC_STATIC) || (flags & ZEND_ACC_STATIC))) {
					property_info = p;
					flags = property_info->flags;
					goto found;
				} else if (flags & ZEND_ACC_PUBLIC) {
					goto found;
				}
			}
			if (flags & ZEND_ACC_PRIVATE) {
				if (property_info->ce != ce) {
					goto dynamic;
				} else {
wrong:
					/* Information was available, but we were denied access.  Error out. */
					if (!silent) {
						zend_bad_property_access(property_info, ce, member);
					}
					return ZEND_WRONG_PROPERTY_OFFSET;
				}
			} else {
				ZEND_ASSERT(flags & ZEND_ACC_PROTECTED);
				if (UNEXPECTED(!is_protected_compatible_scope(property_info->ce, scope))) {
					goto wrong;
				}
			}
		}
	}

found:
	if (UNEXPECTED(flags & ZEND_ACC_STATIC)) {
		if (!silent) {
			zend_error(E_NOTICE, "Accessing static property %s::$%s as non static", ZSTR_VAL(ce->name), ZSTR_VAL(member));
		}
		return ZEND_DYNAMIC_PROPERTY_OFFSET;
	}

	offset = property_info->offset;
	if (EXPECTED(!ZEND_TYPE_IS_SET(property_info->type))) {
		property_info = NULL;
	} else {
		*info_ptr = property_info;
	}
	if (cache_slot) {
		CACHE_POLYMORPHIC_PTR_EX(cache_slot, ce, (void*)(uintptr_t)offset);
		CACHE_PTR_EX(cache_slot + 2, property_info);
	}
	return offset;
}
/* }}} */

static ZEND_COLD void zend_wrong_offset(zend_class_entry *ce, zend_string *member) /* {{{ */
{
	zend_property_info *dummy;

	/* Trigger the correct error */
	zend_get_property_offset(ce, member, 0, NULL, &dummy);
}
/* }}} */

ZEND_API zend_property_info *zend_get_property_info(zend_class_entry *ce, zend_string *member, int silent) /* {{{ */
{
	zval *zv;
	zend_property_info *property_info;
	uint32_t flags;
	zend_class_entry *scope;

	if (UNEXPECTED(zend_hash_num_elements(&ce->properties_info) == 0)
	 || EXPECTED((zv = zend_hash_find(&ce->properties_info, member)) == NULL)) {
		if (UNEXPECTED(ZSTR_VAL(member)[0] == '\0') && ZSTR_LEN(member) != 0) {
			if (!silent) {
				zend_bad_property_name();
			}
			return ZEND_WRONG_PROPERTY_INFO;
		}
dynamic:
		return NULL;
	}

	property_info = (zend_property_info*)Z_PTR_P(zv);
	flags = property_info->flags;

	if (flags & (ZEND_ACC_CHANGED|ZEND_ACC_PRIVATE|ZEND_ACC_PROTECTED)) {
		if (UNEXPECTED(EG(fake_scope))) {
			scope = EG(fake_scope);
		} else {
			scope = zend_get_executed_scope();
		}
		if (property_info->ce != scope) {
			if (flags & ZEND_ACC_CHANGED) {
				zend_property_info *p = zend_get_parent_private_property(scope, ce, member);

				if (p) {
					property_info = p;
					flags = property_info->flags;
					goto found;
				} else if (flags & ZEND_ACC_PUBLIC) {
					goto found;
				}
			}
			if (flags & ZEND_ACC_PRIVATE) {
				if (property_info->ce != ce) {
					goto dynamic;
				} else {
wrong:
					/* Information was available, but we were denied access.  Error out. */
					if (!silent) {
						zend_bad_property_access(property_info, ce, member);
					}
					return ZEND_WRONG_PROPERTY_INFO;
				}
			} else {
				ZEND_ASSERT(flags & ZEND_ACC_PROTECTED);
				if (UNEXPECTED(!is_protected_compatible_scope(property_info->ce, scope))) {
					goto wrong;
				}
			}
		}
	}

found:
	if (UNEXPECTED(flags & ZEND_ACC_STATIC)) {
		if (!silent) {
			zend_error(E_NOTICE, "Accessing static property %s::$%s as non static", ZSTR_VAL(ce->name), ZSTR_VAL(member));
		}
	}
	return property_info;
}
/* }}} */

ZEND_API int zend_check_property_access(zend_object *zobj, zend_string *prop_info_name, bool is_dynamic) /* {{{ */
{
	zend_property_info *property_info;
	const char *class_name = NULL;
	const char *prop_name;
	zend_string *member;
	size_t prop_name_len;

	if (ZSTR_VAL(prop_info_name)[0] == 0) {
		if (is_dynamic) {
			return SUCCESS;
		}

		zend_unmangle_property_name_ex(prop_info_name, &class_name, &prop_name, &prop_name_len);
		member = zend_string_init(prop_name, prop_name_len, 0);
		property_info = zend_get_property_info(zobj->ce, member, 1);
		zend_string_release_ex(member, 0);
		if (property_info == NULL || property_info == ZEND_WRONG_PROPERTY_INFO) {
			return FAILURE;
		}

		if (class_name[0] != '*') {
			if (!(property_info->flags & ZEND_ACC_PRIVATE)) {
				/* we we're looking for a private prop but found a non private one of the same name */
				return FAILURE;
			} else if (strcmp(ZSTR_VAL(prop_info_name)+1, ZSTR_VAL(property_info->name)+1)) {
				/* we we're looking for a private prop but found a private one of the same name but another class */
				return FAILURE;
			}
		} else {
			ZEND_ASSERT(property_info->flags & ZEND_ACC_PROTECTED);
		}
		return SUCCESS;
	} else {
		property_info = zend_get_property_info(zobj->ce, prop_info_name, 1);
		if (property_info == NULL) {
			ZEND_ASSERT(is_dynamic);
			return SUCCESS;
		} else if (property_info == ZEND_WRONG_PROPERTY_INFO) {
			return FAILURE;
		}
		return (property_info->flags & ZEND_ACC_PUBLIC) ? SUCCESS : FAILURE;
	}
}
/* }}} */

static void zend_property_guard_dtor(zval *el) /* {{{ */ {
	uint32_t *ptr = (uint32_t*)Z_PTR_P(el);
	if (EXPECTED(!(((zend_uintptr_t)ptr) & 1))) {
		efree_size(ptr, sizeof(uint32_t));
	}
}
/* }}} */

ZEND_API uint32_t *zend_get_property_guard(zend_object *zobj, zend_string *member) /* {{{ */
{
	HashTable *guards;
	zval *zv;
	uint32_t *ptr;

	ZEND_ASSERT(zobj->ce->ce_flags & ZEND_ACC_USE_GUARDS);
	zv = zobj->properties_table + zobj->ce->default_properties_count;
	if (EXPECTED(Z_TYPE_P(zv) == IS_STRING)) {
		zend_string *str = Z_STR_P(zv);
		if (EXPECTED(str == member) ||
		    /* str and member don't necessarily have a pre-calculated hash value here */
		    EXPECTED(zend_string_equal_content(str, member))) {
			return &Z_PROPERTY_GUARD_P(zv);
		} else if (EXPECTED(Z_PROPERTY_GUARD_P(zv) == 0)) {
			zval_ptr_dtor_str(zv);
			ZVAL_STR_COPY(zv, member);
			return &Z_PROPERTY_GUARD_P(zv);
		} else {
			ALLOC_HASHTABLE(guards);
			zend_hash_init(guards, 8, NULL, zend_property_guard_dtor, 0);
			/* mark pointer as "special" using low bit */
			zend_hash_add_new_ptr(guards, str,
				(void*)(((zend_uintptr_t)&Z_PROPERTY_GUARD_P(zv)) | 1));
			zval_ptr_dtor_str(zv);
			ZVAL_ARR(zv, guards);
		}
	} else if (EXPECTED(Z_TYPE_P(zv) == IS_ARRAY)) {
		guards = Z_ARRVAL_P(zv);
		ZEND_ASSERT(guards != NULL);
		zv = zend_hash_find(guards, member);
		if (zv != NULL) {
			return (uint32_t*)(((zend_uintptr_t)Z_PTR_P(zv)) & ~1);
		}
	} else {
		ZEND_ASSERT(Z_TYPE_P(zv) == IS_UNDEF);
		ZVAL_STR_COPY(zv, member);
		Z_PROPERTY_GUARD_P(zv) = 0;
		return &Z_PROPERTY_GUARD_P(zv);
	}
	/* we have to allocate uint32_t separately because ht->arData may be reallocated */
	ptr = (uint32_t*)emalloc(sizeof(uint32_t));
	*ptr = 0;
	return (uint32_t*)zend_hash_add_new_ptr(guards, member, ptr);
}
/* }}} */

ZEND_API zval *zend_std_read_property(zend_object *zobj, zend_string *name, int type, void **cache_slot, zval *rv) /* {{{ */
{
	zval *retval;
	uintptr_t property_offset;
	zend_property_info *prop_info = NULL;
	uint32_t *guard = NULL;
	zend_string *tmp_name = NULL;

#if DEBUG_OBJECT_HANDLERS
	fprintf(stderr, "Read object #%d property: %s\n", zobj->handle, ZSTR_VAL(name));
#endif

	/* make zend_get_property_info silent if we have getter - we may want to use it */
	property_offset = zend_get_property_offset(zobj->ce, name, (type == BP_VAR_IS) || (zobj->ce->__get != NULL), cache_slot, &prop_info);

	if (EXPECTED(IS_VALID_PROPERTY_OFFSET(property_offset))) {
		retval = OBJ_PROP(zobj, property_offset);
		if (EXPECTED(Z_TYPE_P(retval) != IS_UNDEF)) {
			if (prop_info && UNEXPECTED(prop_info->flags & ZEND_ACC_READONLY)
					&& (type == BP_VAR_W || type == BP_VAR_RW || type == BP_VAR_UNSET)) {
				if (Z_TYPE_P(retval) == IS_OBJECT) {
					/* For objects, W/RW/UNSET fetch modes might not actually modify object.
					 * Similar as with magic __get() allow them, but return the value as a copy
					 * to make sure no actual modification is possible. */
					ZVAL_COPY(rv, retval);
					retval = rv;
				} else {
					zend_readonly_property_modification_error(prop_info);
					retval = &EG(uninitialized_zval);
				}
			}
			goto exit;
		} else {
			if (prop_info && UNEXPECTED(prop_info->flags & ZEND_ACC_READONLY)) {
				if (type == BP_VAR_W || type == BP_VAR_RW) {
					zend_readonly_property_indirect_modification_error(prop_info);
					retval = &EG(uninitialized_zval);
					goto exit;
				} else if (type == BP_VAR_UNSET) {
					retval = &EG(uninitialized_zval);
					goto exit;
				}
			}
		}
		if (UNEXPECTED(Z_PROP_FLAG_P(retval) == IS_PROP_UNINIT)) {
			/* Skip __get() for uninitialized typed properties */
			goto uninit_error;
		}
	} else if (EXPECTED(IS_DYNAMIC_PROPERTY_OFFSET(property_offset))) {
		if (EXPECTED(zobj->properties != NULL)) {
			if (!IS_UNKNOWN_DYNAMIC_PROPERTY_OFFSET(property_offset)) {
				uintptr_t idx = ZEND_DECODE_DYN_PROP_OFFSET(property_offset);

				if (EXPECTED(idx < zobj->properties->nNumUsed * sizeof(Bucket))) {
					Bucket *p = (Bucket*)((char*)zobj->properties->arData + idx);

					if (EXPECTED(p->key == name) ||
				        (EXPECTED(p->h == ZSTR_H(name)) &&
				         EXPECTED(p->key != NULL) &&
				         EXPECTED(zend_string_equal_content(p->key, name)))) {
						retval = &p->val;
						goto exit;
					}
				}
				CACHE_PTR_EX(cache_slot + 1, (void*)ZEND_DYNAMIC_PROPERTY_OFFSET);
			}
			retval = zend_hash_find(zobj->properties, name);
			if (EXPECTED(retval)) {
				if (cache_slot) {
					uintptr_t idx = (char*)retval - (char*)zobj->properties->arData;
					CACHE_PTR_EX(cache_slot + 1, (void*)ZEND_ENCODE_DYN_PROP_OFFSET(idx));
				}
				goto exit;
			}
		}
	} else if (UNEXPECTED(EG(exception))) {
		retval = &EG(uninitialized_zval);
		goto exit;
	}

	/* magic isset */
	if ((type == BP_VAR_IS) && zobj->ce->__isset) {
		zval tmp_result;
		guard = zend_get_property_guard(zobj, name);

		if (!((*guard) & IN_ISSET)) {
			if (!tmp_name && !ZSTR_IS_INTERNED(name)) {
				tmp_name = zend_string_copy(name);
			}
			GC_ADDREF(zobj);
			ZVAL_UNDEF(&tmp_result);

			*guard |= IN_ISSET;
			zend_std_call_issetter(zobj, name, &tmp_result);
			*guard &= ~IN_ISSET;

			if (!zend_is_true(&tmp_result)) {
				retval = &EG(uninitialized_zval);
				OBJ_RELEASE(zobj);
				zval_ptr_dtor(&tmp_result);
				goto exit;
			}

			zval_ptr_dtor(&tmp_result);
			if (zobj->ce->__get && !((*guard) & IN_GET)) {
				goto call_getter;
			}
			OBJ_RELEASE(zobj);
		} else if (zobj->ce->__get && !((*guard) & IN_GET)) {
			goto call_getter_addref;
		}
	} else if (zobj->ce->__get) {
		/* magic get */
		guard = zend_get_property_guard(zobj, name);
		if (!((*guard) & IN_GET)) {
			/* have getter - try with it! */
call_getter_addref:
			GC_ADDREF(zobj);
call_getter:
			*guard |= IN_GET; /* prevent circular getting */
			zend_std_call_getter(zobj, name, rv);
			*guard &= ~IN_GET;

			if (Z_TYPE_P(rv) != IS_UNDEF) {
				retval = rv;
				if (!Z_ISREF_P(rv) &&
				    (type == BP_VAR_W || type == BP_VAR_RW  || type == BP_VAR_UNSET)) {
					if (UNEXPECTED(Z_TYPE_P(rv) != IS_OBJECT)) {
						zend_error(E_NOTICE, "Indirect modification of overloaded property %s::$%s has no effect", ZSTR_VAL(zobj->ce->name), ZSTR_VAL(name));
					}
				}
			} else {
				retval = &EG(uninitialized_zval);
			}

			if (UNEXPECTED(prop_info)) {
				zend_verify_prop_assignable_by_ref(prop_info, retval, (zobj->ce->__get->common.fn_flags & ZEND_ACC_STRICT_TYPES) != 0);
			}

			OBJ_RELEASE(zobj);
			goto exit;
		} else if (UNEXPECTED(IS_WRONG_PROPERTY_OFFSET(property_offset))) {
			/* Trigger the correct error */
			zend_get_property_offset(zobj->ce, name, 0, NULL, &prop_info);
			ZEND_ASSERT(EG(exception));
			retval = &EG(uninitialized_zval);
			goto exit;
		}
	}

uninit_error:
	if (type != BP_VAR_IS) {
		if (UNEXPECTED(prop_info)) {
			zend_throw_error(NULL, "Typed property %s::$%s must not be accessed before initialization",
				ZSTR_VAL(prop_info->ce->name),
				ZSTR_VAL(name));
		} else {
			zend_error(E_WARNING, "Undefined property: %s::$%s", ZSTR_VAL(zobj->ce->name), ZSTR_VAL(name));
		}
	}
	retval = &EG(uninitialized_zval);

exit:
	zend_tmp_string_release(tmp_name);

	return retval;
}
/* }}} */

static zend_always_inline bool property_uses_strict_types(void) {
	zend_execute_data *execute_data = EG(current_execute_data);
	return execute_data
		&& execute_data->func
		&& ZEND_CALL_USES_STRICT_TYPES(EG(current_execute_data));
}

static bool verify_readonly_initialization_access(
		zend_property_info *prop_info, zend_class_entry *ce,
		zend_string *name, const char *operation) {
	zend_class_entry *scope;
	if (UNEXPECTED(EG(fake_scope))) {
		scope = EG(fake_scope);
	} else {
		scope = zend_get_executed_scope();
	}
	if (prop_info->ce == scope) {
		return true;
	}

	/* We may have redeclared a parent property. In that case the parent should still be
	 * allowed to initialize it. */
	if (scope && is_derived_class(ce, scope)) {
		zend_property_info *prop_info = zend_hash_find_ptr(&scope->properties_info, name);
		if (prop_info) {
			/* This should be ensured by inheritance. */
			ZEND_ASSERT(prop_info->flags & ZEND_ACC_READONLY);
			if (prop_info->ce == scope) {
				return true;
			}
		}
	}

	zend_readonly_property_modification_scope_error(prop_info->ce, name, scope, operation);
	return false;
}

ZEND_API zval *zend_std_write_property(zend_object *zobj, zend_string *name, zval *value, void **cache_slot) /* {{{ */
{
	zval *variable_ptr, tmp;
	uintptr_t property_offset;
	zend_property_info *prop_info = NULL;
	ZEND_ASSERT(!Z_ISREF_P(value));

	property_offset = zend_get_property_offset(zobj->ce, name, (zobj->ce->__set != NULL), cache_slot, &prop_info);

	if (EXPECTED(IS_VALID_PROPERTY_OFFSET(property_offset))) {
		variable_ptr = OBJ_PROP(zobj, property_offset);
		if (Z_TYPE_P(variable_ptr) != IS_UNDEF) {
			Z_TRY_ADDREF_P(value);

			if (UNEXPECTED(prop_info)) {
				if (UNEXPECTED(prop_info->flags & ZEND_ACC_READONLY)) {
					Z_TRY_DELREF_P(value);
					zend_readonly_property_modification_error(prop_info);
					variable_ptr = &EG(error_zval);
					goto exit;
				}

				ZVAL_COPY_VALUE(&tmp, value);
				if (UNEXPECTED(!zend_verify_property_type(prop_info, &tmp, property_uses_strict_types()))) {
					Z_TRY_DELREF_P(value);
					variable_ptr = &EG(error_zval);
					goto exit;
				}
				value = &tmp;
			}

found:
			variable_ptr = zend_assign_to_variable(
				variable_ptr, value, IS_TMP_VAR, property_uses_strict_types());
			goto exit;
		}
		if (Z_PROP_FLAG_P(variable_ptr) == IS_PROP_UNINIT) {
			/* Writes to uninitialized typed properties bypass __set(). */
			goto write_std_property;
		}
	} else if (EXPECTED(IS_DYNAMIC_PROPERTY_OFFSET(property_offset))) {
		if (EXPECTED(zobj->properties != NULL)) {
			if (UNEXPECTED(GC_REFCOUNT(zobj->properties) > 1)) {
				if (EXPECTED(!(GC_FLAGS(zobj->properties) & IS_ARRAY_IMMUTABLE))) {
					GC_DELREF(zobj->properties);
				}
				zobj->properties = zend_array_dup(zobj->properties);
			}
			if ((variable_ptr = zend_hash_find(zobj->properties, name)) != NULL) {
				Z_TRY_ADDREF_P(value);
				goto found;
			}
		}
	} else if (UNEXPECTED(EG(exception))) {
		variable_ptr = &EG(error_zval);
		goto exit;
	}

	/* magic set */
	if (zobj->ce->__set) {
		uint32_t *guard = zend_get_property_guard(zobj, name);

		if (!((*guard) & IN_SET)) {
			GC_ADDREF(zobj);
			(*guard) |= IN_SET; /* prevent circular setting */
			zend_std_call_setter(zobj, name, value);
			(*guard) &= ~IN_SET;
			OBJ_RELEASE(zobj);
			variable_ptr = value;
		} else if (EXPECTED(!IS_WRONG_PROPERTY_OFFSET(property_offset))) {
			goto write_std_property;
		} else {
			/* Trigger the correct error */
			zend_wrong_offset(zobj->ce, name);
			ZEND_ASSERT(EG(exception));
			variable_ptr = &EG(error_zval);
			goto exit;
		}
	} else {
		ZEND_ASSERT(!IS_WRONG_PROPERTY_OFFSET(property_offset));
write_std_property:
		if (EXPECTED(IS_VALID_PROPERTY_OFFSET(property_offset))) {
			variable_ptr = OBJ_PROP(zobj, property_offset);

			Z_TRY_ADDREF_P(value);
			if (UNEXPECTED(prop_info)) {
				if (UNEXPECTED((prop_info->flags & ZEND_ACC_READONLY)
						&& !verify_readonly_initialization_access(prop_info, zobj->ce, name, "initialize"))) {
					Z_TRY_DELREF_P(value);
					variable_ptr = &EG(error_zval);
					goto exit;
				}

				ZVAL_COPY_VALUE(&tmp, value);
				if (UNEXPECTED(!zend_verify_property_type(prop_info, &tmp, property_uses_strict_types()))) {
					zval_ptr_dtor(value);
					goto exit;
				}
				value = &tmp;
				Z_PROP_FLAG_P(variable_ptr) = 0;
				goto found; /* might have been updated via e.g. __toString() */
			}

			ZVAL_COPY_VALUE(variable_ptr, value);
		} else {
			if (UNEXPECTED(zobj->ce->ce_flags & ZEND_ACC_NO_DYNAMIC_PROPERTIES)) {
				zend_forbidden_dynamic_property(zobj->ce, name);
				variable_ptr = &EG(error_zval);
				goto exit;
			}
			if (UNEXPECTED(!(zobj->ce->ce_flags & ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES))) {
				if (UNEXPECTED(!zend_deprecated_dynamic_property(zobj, name))) {
					variable_ptr = &EG(error_zval);
					goto exit;
				}
			}

			Z_TRY_ADDREF_P(value);
			if (!zobj->properties) {
				rebuild_object_properties(zobj);
			}
			variable_ptr = zend_hash_add_new(zobj->properties, name, value);
		}
	}

exit:
	return variable_ptr;
}
/* }}} */

static ZEND_COLD zend_never_inline void zend_bad_array_access(zend_class_entry *ce) /* {{{ */
{
	zend_throw_error(NULL, "Cannot use object of type %s as array", ZSTR_VAL(ce->name));
}
/* }}} */

ZEND_API zval *zend_std_read_dimension(zend_object *object, zval *offset, int type, zval *rv) /* {{{ */
{
	zend_class_entry *ce = object->ce;
	zval tmp_offset;

	/* arrayaccess_funcs_ptr is set if (and only if) the class implements zend_ce_arrayaccess */
	zend_class_arrayaccess_funcs *funcs = ce->arrayaccess_funcs_ptr;
	if (EXPECTED(funcs)) {
		if (offset == NULL) {
			/* [] construct */
			ZVAL_NULL(&tmp_offset);
		} else {
			ZVAL_COPY_DEREF(&tmp_offset, offset);
		}

		GC_ADDREF(object);
		if (type == BP_VAR_IS) {
			zend_call_known_instance_method_with_1_params(funcs->zf_offsetexists, object, rv, &tmp_offset);
			if (UNEXPECTED(Z_ISUNDEF_P(rv))) {
				OBJ_RELEASE(object);
				zval_ptr_dtor(&tmp_offset);
				return NULL;
			}
			if (!i_zend_is_true(rv)) {
				OBJ_RELEASE(object);
				zval_ptr_dtor(&tmp_offset);
				zval_ptr_dtor(rv);
				return &EG(uninitialized_zval);
			}
			zval_ptr_dtor(rv);
		}

		zend_call_known_instance_method_with_1_params(funcs->zf_offsetget, object, rv, &tmp_offset);

		OBJ_RELEASE(object);
		zval_ptr_dtor(&tmp_offset);

		if (UNEXPECTED(Z_TYPE_P(rv) == IS_UNDEF)) {
			if (UNEXPECTED(!EG(exception))) {
				zend_throw_error(NULL, "Undefined offset for object of type %s used as array", ZSTR_VAL(ce->name));
			}
			return NULL;
		}
		return rv;
	} else {
	    zend_bad_array_access(ce);
		return NULL;
	}
}
/* }}} */

ZEND_API void zend_std_write_dimension(zend_object *object, zval *offset, zval *value) /* {{{ */
{
	zend_class_entry *ce = object->ce;
	zval tmp_offset;

	zend_class_arrayaccess_funcs *funcs = ce->arrayaccess_funcs_ptr;
	if (EXPECTED(funcs)) {
		if (!offset) {
			ZVAL_NULL(&tmp_offset);
		} else {
			ZVAL_COPY_DEREF(&tmp_offset, offset);
		}
		GC_ADDREF(object);
		zend_call_known_instance_method_with_2_params(funcs->zf_offsetset, object, NULL, &tmp_offset, value);
		OBJ_RELEASE(object);
		zval_ptr_dtor(&tmp_offset);
	} else {
	    zend_bad_array_access(ce);
	}
}
/* }}} */

ZEND_API int zend_std_has_dimension(zend_object *object, zval *offset, int check_empty) /* {{{ */
{
	zend_class_entry *ce = object->ce;
	zval retval, tmp_offset;
	int result;

	zend_class_arrayaccess_funcs *funcs = ce->arrayaccess_funcs_ptr;
	if (EXPECTED(funcs)) {
		ZVAL_COPY_DEREF(&tmp_offset, offset);
		GC_ADDREF(object);
		zend_call_known_instance_method_with_1_params(funcs->zf_offsetexists, object, &retval, &tmp_offset);
		result = i_zend_is_true(&retval);
		zval_ptr_dtor(&retval);
		if (check_empty && result && EXPECTED(!EG(exception))) {
			zend_call_known_instance_method_with_1_params(funcs->zf_offsetget, object, &retval, &tmp_offset);
			result = i_zend_is_true(&retval);
			zval_ptr_dtor(&retval);
		}
		OBJ_RELEASE(object);
		zval_ptr_dtor(&tmp_offset);
	} else {
	    zend_bad_array_access(ce);
		return 0;
	}
	return result;
}
/* }}} */

ZEND_API zval *zend_std_get_property_ptr_ptr(zend_object *zobj, zend_string *name, int type, void **cache_slot) /* {{{ */
{
	zval *retval = NULL;
	uintptr_t property_offset;
	zend_property_info *prop_info = NULL;

#if DEBUG_OBJECT_HANDLERS
	fprintf(stderr, "Ptr object #%d property: %s\n", zobj->handle, ZSTR_VAL(name));
#endif

	property_offset = zend_get_property_offset(zobj->ce, name, (zobj->ce->__get != NULL), cache_slot, &prop_info);

	if (EXPECTED(IS_VALID_PROPERTY_OFFSET(property_offset))) {
		retval = OBJ_PROP(zobj, property_offset);
		if (UNEXPECTED(Z_TYPE_P(retval) == IS_UNDEF)) {
			if (EXPECTED(!zobj->ce->__get) ||
			    UNEXPECTED((*zend_get_property_guard(zobj, name)) & IN_GET) ||
			    UNEXPECTED(prop_info && Z_PROP_FLAG_P(retval) == IS_PROP_UNINIT)) {
				if (UNEXPECTED(type == BP_VAR_RW || type == BP_VAR_R)) {
					if (UNEXPECTED(prop_info)) {
						zend_throw_error(NULL,
							"Typed property %s::$%s must not be accessed before initialization",
							ZSTR_VAL(prop_info->ce->name),
							ZSTR_VAL(name));
						retval = &EG(error_zval);
					} else {
						ZVAL_NULL(retval);
						zend_error(E_WARNING, "Undefined property: %s::$%s", ZSTR_VAL(zobj->ce->name), ZSTR_VAL(name));
					}
				} else if (prop_info && UNEXPECTED(prop_info->flags & ZEND_ACC_READONLY)) {
					/* Readonly property, delegate to read_property + write_property. */
					retval = NULL;
				} else if (!prop_info || !ZEND_TYPE_IS_SET(prop_info->type)) {
					ZVAL_NULL(retval);
				}
			} else {
				/* we do have getter - fail and let it try again with usual get/set */
				retval = NULL;
			}
		} else if (prop_info && UNEXPECTED(prop_info->flags & ZEND_ACC_READONLY)) {
			/* Readonly property, delegate to read_property + write_property. */
			retval = NULL;
		}
	} else if (EXPECTED(IS_DYNAMIC_PROPERTY_OFFSET(property_offset))) {
		if (EXPECTED(zobj->properties)) {
			if (UNEXPECTED(GC_REFCOUNT(zobj->properties) > 1)) {
				if (EXPECTED(!(GC_FLAGS(zobj->properties) & IS_ARRAY_IMMUTABLE))) {
					GC_DELREF(zobj->properties);
				}
				zobj->properties = zend_array_dup(zobj->properties);
			}
		    if (EXPECTED((retval = zend_hash_find(zobj->properties, name)) != NULL)) {
				return retval;
		    }
		}
		if (EXPECTED(!zobj->ce->__get) ||
		    UNEXPECTED((*zend_get_property_guard(zobj, name)) & IN_GET)) {
			if (UNEXPECTED(zobj->ce->ce_flags & ZEND_ACC_NO_DYNAMIC_PROPERTIES)) {
				zend_forbidden_dynamic_property(zobj->ce, name);
				return &EG(error_zval);
			}
			if (UNEXPECTED(!(zobj->ce->ce_flags & ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES))) {
				if (UNEXPECTED(!zend_deprecated_dynamic_property(zobj, name))) {
					return &EG(error_zval);
				}
			}
			if (UNEXPECTED(!zobj->properties)) {
				rebuild_object_properties(zobj);
			}
			retval = zend_hash_update(zobj->properties, name, &EG(uninitialized_zval));
			/* Notice is thrown after creation of the property, to avoid EG(std_property_info)
			 * being overwritten in an error handler. */
			if (UNEXPECTED(type == BP_VAR_RW || type == BP_VAR_R)) {
				zend_error(E_WARNING, "Undefined property: %s::$%s", ZSTR_VAL(zobj->ce->name), ZSTR_VAL(name));
			}
		}
	} else if (zobj->ce->__get == NULL) {
		retval = &EG(error_zval);
	}

	return retval;
}
/* }}} */

ZEND_API void zend_std_unset_property(zend_object *zobj, zend_string *name, void **cache_slot) /* {{{ */
{
	uintptr_t property_offset;
	zend_property_info *prop_info = NULL;

	property_offset = zend_get_property_offset(zobj->ce, name, (zobj->ce->__unset != NULL), cache_slot, &prop_info);

	if (EXPECTED(IS_VALID_PROPERTY_OFFSET(property_offset))) {
		zval *slot = OBJ_PROP(zobj, property_offset);

		if (Z_TYPE_P(slot) != IS_UNDEF) {
			if (UNEXPECTED(prop_info && (prop_info->flags & ZEND_ACC_READONLY))) {
				zend_readonly_property_unset_error(prop_info->ce, name);
				return;
			}
			if (UNEXPECTED(Z_ISREF_P(slot)) &&
					(ZEND_DEBUG || ZEND_REF_HAS_TYPE_SOURCES(Z_REF_P(slot)))) {
				if (prop_info) {
					ZEND_REF_DEL_TYPE_SOURCE(Z_REF_P(slot), prop_info);
				}
			}
			zval tmp;
			ZVAL_COPY_VALUE(&tmp, slot);
			ZVAL_UNDEF(slot);
			zval_ptr_dtor(&tmp);
			if (zobj->properties) {
				HT_FLAGS(zobj->properties) |= HASH_FLAG_HAS_EMPTY_IND;
			}
			return;
		}
		if (UNEXPECTED(Z_PROP_FLAG_P(slot) == IS_PROP_UNINIT)) {
			if (UNEXPECTED(prop_info && (prop_info->flags & ZEND_ACC_READONLY)
					&& !verify_readonly_initialization_access(prop_info, zobj->ce, name, "unset"))) {
				return;
			}

			/* Reset the IS_PROP_UNINIT flag, if it exists and bypass __unset(). */
			Z_PROP_FLAG_P(slot) = 0;
			return;
		}
	} else if (EXPECTED(IS_DYNAMIC_PROPERTY_OFFSET(property_offset))
	 && EXPECTED(zobj->properties != NULL)) {
		if (UNEXPECTED(GC_REFCOUNT(zobj->properties) > 1)) {
			if (EXPECTED(!(GC_FLAGS(zobj->properties) & IS_ARRAY_IMMUTABLE))) {
				GC_DELREF(zobj->properties);
			}
			zobj->properties = zend_array_dup(zobj->properties);
		}
		if (EXPECTED(zend_hash_del(zobj->properties, name) != FAILURE)) {
			return;
		}
	} else if (UNEXPECTED(EG(exception))) {
		return;
	}

	/* magic unset */
	if (zobj->ce->__unset) {
		uint32_t *guard = zend_get_property_guard(zobj, name);
		if (!((*guard) & IN_UNSET)) {
			/* have unseter - try with it! */
			(*guard) |= IN_UNSET; /* prevent circular unsetting */
			zend_std_call_unsetter(zobj, name);
			(*guard) &= ~IN_UNSET;
		} else if (UNEXPECTED(IS_WRONG_PROPERTY_OFFSET(property_offset))) {
			/* Trigger the correct error */
			zend_wrong_offset(zobj->ce, name);
			ZEND_ASSERT(EG(exception));
			return;
		} else {
			/* Nothing to do: The property already does not exist. */
		}
	}
}
/* }}} */

ZEND_API void zend_std_unset_dimension(zend_object *object, zval *offset) /* {{{ */
{
	zend_class_entry *ce = object->ce;
	zval tmp_offset;

	zend_class_arrayaccess_funcs *funcs = ce->arrayaccess_funcs_ptr;
	if (EXPECTED(funcs)) {
		ZVAL_COPY_DEREF(&tmp_offset, offset);
		GC_ADDREF(object);
		zend_call_known_instance_method_with_1_params(funcs->zf_offsetunset, object, NULL, &tmp_offset);
		OBJ_RELEASE(object);
		zval_ptr_dtor(&tmp_offset);
	} else {
	    zend_bad_array_access(ce);
	}
}
/* }}} */

static zend_never_inline zend_function *zend_get_parent_private_method(zend_class_entry *scope, zend_class_entry *ce, zend_string *function_name) /* {{{ */
{
	zval *func;
	zend_function *fbc;

	if (scope != ce && scope && is_derived_class(ce, scope)) {
		func = zend_hash_find(&scope->function_table, function_name);
		if (func != NULL) {
			fbc = Z_FUNC_P(func);
			if ((fbc->common.fn_flags & ZEND_ACC_PRIVATE)
			 && fbc->common.scope == scope) {
				return fbc;
			}
		}
	}
	return NULL;
}
/* }}} */

/* Ensures that we're allowed to call a protected method.
 */
ZEND_API int zend_check_protected(zend_class_entry *ce, zend_class_entry *scope) /* {{{ */
{
	zend_class_entry *fbc_scope = ce;

	/* Is the context that's calling the function, the same as one of
	 * the function's parents?
	 */
	while (fbc_scope) {
		if (fbc_scope==scope) {
			return 1;
		}
		fbc_scope = fbc_scope->parent;
	}

	/* Is the function's scope the same as our current object context,
	 * or any of the parents of our context?
	 */
	while (scope) {
		if (scope==ce) {
			return 1;
		}
		scope = scope->parent;
	}
	return 0;
}
/* }}} */

ZEND_API zend_function *zend_get_call_trampoline_func(zend_class_entry *ce, zend_string *method_name, int is_static) /* {{{ */
{
	size_t mname_len;
	zend_op_array *func;
	zend_function *fbc = is_static ? ce->__callstatic : ce->__call;
	/* We use non-NULL value to avoid useless run_time_cache allocation.
	 * The low bit must be zero, to not be interpreted as a MAP_PTR offset.
	 */
	static const void *dummy = (void*)(intptr_t)2;
	static const zend_arg_info arg_info[1] = {{0}};

	ZEND_ASSERT(fbc);

	if (EXPECTED(EG(trampoline).common.function_name == NULL)) {
		func = &EG(trampoline).op_array;
	} else {
		func = ecalloc(1, sizeof(zend_op_array));
	}

	func->type = ZEND_USER_FUNCTION;
	func->arg_flags[0] = 0;
	func->arg_flags[1] = 0;
	func->arg_flags[2] = 0;
	func->fn_flags = ZEND_ACC_CALL_VIA_TRAMPOLINE | ZEND_ACC_PUBLIC | ZEND_ACC_VARIADIC;
	if (is_static) {
		func->fn_flags |= ZEND_ACC_STATIC;
	}
	func->opcodes = &EG(call_trampoline_op);
	ZEND_MAP_PTR_INIT(func->run_time_cache, (void**)dummy);
	func->scope = fbc->common.scope;
	/* reserve space for arguments, local and temporary variables */
	/* EG(trampoline) is reused from other places, like FFI (e.g. zend_ffi_cdata_get_closure()) where
	 * it is used as an internal function. It may set fields that don't belong to common, thus
	 * modifying zend_op_array specific data, most significantly last_var. We need to reset this
	 * value so that it doesn't contain garbage when the engine allocates space for the next stack
	 * frame. This didn't cause any issues until now due to "lucky" structure layout. */
	func->last_var = 0;
	func->T = (fbc->type == ZEND_USER_FUNCTION)? MAX(fbc->op_array.last_var + fbc->op_array.T, 2) : 2;
	func->filename = (fbc->type == ZEND_USER_FUNCTION)? fbc->op_array.filename : ZSTR_EMPTY_ALLOC();
	func->line_start = (fbc->type == ZEND_USER_FUNCTION)? fbc->op_array.line_start : 0;
	func->line_end = (fbc->type == ZEND_USER_FUNCTION)? fbc->op_array.line_end : 0;

	//??? keep compatibility for "\0" characters
	//??? see: Zend/tests/bug46238.phpt
	if (UNEXPECTED((mname_len = strlen(ZSTR_VAL(method_name))) != ZSTR_LEN(method_name))) {
		func->function_name = zend_string_init(ZSTR_VAL(method_name), mname_len, 0);
	} else {
		func->function_name = zend_string_copy(method_name);
	}

	func->prototype = NULL;
	func->num_args = 0;
	func->required_num_args = 0;
	func->arg_info = (zend_arg_info *) arg_info;

	return (zend_function*)func;
}
/* }}} */

static zend_always_inline zend_function *zend_get_user_call_function(zend_class_entry *ce, zend_string *method_name) /* {{{ */
{
	return zend_get_call_trampoline_func(ce, method_name, 0);
}
/* }}} */

static ZEND_COLD zend_never_inline void zend_bad_method_call(zend_function *fbc, zend_string *method_name, zend_class_entry *scope) /* {{{ */
{
	zend_throw_error(NULL, "Call to %s method %s::%s() from %s%s",
		zend_visibility_string(fbc->common.fn_flags), ZEND_FN_SCOPE_NAME(fbc), ZSTR_VAL(method_name),
		scope ? "scope " : "global scope",
		scope ? ZSTR_VAL(scope->name) : ""
	);
}
/* }}} */

static ZEND_COLD zend_never_inline void zend_abstract_method_call(zend_function *fbc) /* {{{ */
{
	zend_throw_error(NULL, "Cannot call abstract method %s::%s()",
		ZSTR_VAL(fbc->common.scope->name), ZSTR_VAL(fbc->common.function_name));
}
/* }}} */

ZEND_API zend_function *zend_std_get_method(zend_object **obj_ptr, zend_string *method_name, const zval *key) /* {{{ */
{
	zend_object *zobj = *obj_ptr;
	zval *func;
	zend_function *fbc;
	zend_string *lc_method_name;
	zend_class_entry *scope;
	ALLOCA_FLAG(use_heap);

	if (EXPECTED(key != NULL)) {
		lc_method_name = Z_STR_P(key);
#ifdef ZEND_ALLOCA_MAX_SIZE
		use_heap = 0;
#endif
	} else {
		ZSTR_ALLOCA_ALLOC(lc_method_name, ZSTR_LEN(method_name), use_heap);
		zend_str_tolower_copy(ZSTR_VAL(lc_method_name), ZSTR_VAL(method_name), ZSTR_LEN(method_name));
	}

	if (UNEXPECTED((func = zend_hash_find(&zobj->ce->function_table, lc_method_name)) == NULL)) {
		if (UNEXPECTED(!key)) {
			ZSTR_ALLOCA_FREE(lc_method_name, use_heap);
		}
		if (zobj->ce->__call) {
			return zend_get_user_call_function(zobj->ce, method_name);
		} else {
			return NULL;
		}
	}

	fbc = Z_FUNC_P(func);

	/* Check access level */
	if (fbc->op_array.fn_flags & (ZEND_ACC_CHANGED|ZEND_ACC_PRIVATE|ZEND_ACC_PROTECTED)) {
		scope = zend_get_executed_scope();

		if (fbc->common.scope != scope) {
			if (fbc->op_array.fn_flags & ZEND_ACC_CHANGED) {
				zend_function *updated_fbc = zend_get_parent_private_method(scope, zobj->ce, lc_method_name);

				if (EXPECTED(updated_fbc != NULL)) {
					fbc = updated_fbc;
					goto exit;
				} else if (fbc->op_array.fn_flags & ZEND_ACC_PUBLIC) {
					goto exit;
				}
			}
			if (UNEXPECTED(fbc->op_array.fn_flags & ZEND_ACC_PRIVATE)
			 || UNEXPECTED(!zend_check_protected(zend_get_function_root_class(fbc), scope))) {
				if (zobj->ce->__call) {
					fbc = zend_get_user_call_function(zobj->ce, method_name);
				} else {
					zend_bad_method_call(fbc, method_name, scope);
					fbc = NULL;
				}
			}
		}
	}

exit:
	if (fbc && UNEXPECTED(fbc->common.fn_flags & ZEND_ACC_ABSTRACT)) {
		zend_abstract_method_call(fbc);
		fbc = NULL;
	}
	if (UNEXPECTED(!key)) {
		ZSTR_ALLOCA_FREE(lc_method_name, use_heap);
	}
	return fbc;
}
/* }}} */

static zend_always_inline zend_function *zend_get_user_callstatic_function(zend_class_entry *ce, zend_string *method_name) /* {{{ */
{
	return zend_get_call_trampoline_func(ce, method_name, 1);
}
/* }}} */

static zend_always_inline zend_function *get_static_method_fallback(
		zend_class_entry *ce, zend_string *function_name)
{
	zend_object *object;
	if (ce->__call &&
		(object = zend_get_this_object(EG(current_execute_data))) != NULL &&
		instanceof_function(object->ce, ce)) {
		/* Call the top-level defined __call().
		 * see: tests/classes/__call_004.phpt  */

		ZEND_ASSERT(object->ce->__call);
		return zend_get_user_call_function(object->ce, function_name);
	} else if (ce->__callstatic) {
		return zend_get_user_callstatic_function(ce, function_name);
	} else {
		return NULL;
	}
}

ZEND_API zend_function *zend_std_get_static_method(zend_class_entry *ce, zend_string *function_name, const zval *key) /* {{{ */
{
	zend_string *lc_function_name;
	if (EXPECTED(key != NULL)) {
		lc_function_name = Z_STR_P(key);
	} else {
		lc_function_name = zend_string_tolower(function_name);
	}

	zend_function *fbc;
	zval *func = zend_hash_find(&ce->function_table, lc_function_name);
	if (EXPECTED(func)) {
		fbc = Z_FUNC_P(func);
		if (!(fbc->op_array.fn_flags & ZEND_ACC_PUBLIC)) {
			zend_class_entry *scope = zend_get_executed_scope();
			if (UNEXPECTED(fbc->common.scope != scope)) {
				if (UNEXPECTED(fbc->op_array.fn_flags & ZEND_ACC_PRIVATE)
				 || UNEXPECTED(!zend_check_protected(zend_get_function_root_class(fbc), scope))) {
					zend_function *fallback_fbc = get_static_method_fallback(ce, function_name);
					if (!fallback_fbc) {
						zend_bad_method_call(fbc, function_name, scope);
					}
					fbc = fallback_fbc;
				}
			}
		}
	} else {
		fbc = get_static_method_fallback(ce, function_name);
	}

	if (UNEXPECTED(!key)) {
		zend_string_release_ex(lc_function_name, 0);
	}

	if (EXPECTED(fbc)) {
		if (UNEXPECTED(fbc->common.fn_flags & ZEND_ACC_ABSTRACT)) {
			zend_abstract_method_call(fbc);
			fbc = NULL;
		} else if (UNEXPECTED(fbc->common.scope->ce_flags & ZEND_ACC_TRAIT)) {
			zend_error(E_DEPRECATED,
				"Calling static trait method %s::%s is deprecated, "
				"it should only be called on a class using the trait",
				ZSTR_VAL(fbc->common.scope->name), ZSTR_VAL(fbc->common.function_name));
			if (EG(exception)) {
				return NULL;
			}
		}
	}

	return fbc;
}
/* }}} */

ZEND_API void zend_class_init_statics(zend_class_entry *class_type) /* {{{ */
{
	int i;
	zval *p;

	if (class_type->default_static_members_count && !CE_STATIC_MEMBERS(class_type)) {
		if (class_type->parent) {
			zend_class_init_statics(class_type->parent);
		}

		ZEND_MAP_PTR_SET(class_type->static_members_table, emalloc(sizeof(zval) * class_type->default_static_members_count));
		for (i = 0; i < class_type->default_static_members_count; i++) {
			p = &class_type->default_static_members_table[i];
			if (Z_TYPE_P(p) == IS_INDIRECT) {
				zval *q = &CE_STATIC_MEMBERS(class_type->parent)[i];
				ZVAL_DEINDIRECT(q);
				ZVAL_INDIRECT(&CE_STATIC_MEMBERS(class_type)[i], q);
			} else {
				ZVAL_COPY_OR_DUP(&CE_STATIC_MEMBERS(class_type)[i], p);
			}
		}
	}
} /* }}} */

ZEND_API zval *zend_std_get_static_property_with_info(zend_class_entry *ce, zend_string *property_name, int type, zend_property_info **property_info_ptr) /* {{{ */
{
	zval *ret;
	zend_class_entry *scope;
	zend_property_info *property_info = zend_hash_find_ptr(&ce->properties_info, property_name);
	*property_info_ptr = property_info;

	if (UNEXPECTED(property_info == NULL)) {
		goto undeclared_property;
	}

	if (!(property_info->flags & ZEND_ACC_PUBLIC)) {
		if (UNEXPECTED(EG(fake_scope))) {
			scope = EG(fake_scope);
		} else {
			scope = zend_get_executed_scope();
		}
		if (property_info->ce != scope) {
			if (UNEXPECTED(property_info->flags & ZEND_ACC_PRIVATE)
			 || UNEXPECTED(!is_protected_compatible_scope(property_info->ce, scope))) {
				if (type != BP_VAR_IS) {
					zend_bad_property_access(property_info, ce, property_name);
				}
				return NULL;
			}
		}
	}

	if (UNEXPECTED((property_info->flags & ZEND_ACC_STATIC) == 0)) {
undeclared_property:
		if (type != BP_VAR_IS) {
			zend_throw_error(NULL, "Access to undeclared static property %s::$%s", ZSTR_VAL(ce->name), ZSTR_VAL(property_name));
		}
		return NULL;
	}

	if (UNEXPECTED(!(ce->ce_flags & ZEND_ACC_CONSTANTS_UPDATED))) {
		if (UNEXPECTED(zend_update_class_constants(ce)) != SUCCESS) {
			return NULL;
		}
	}

	/* Ensure static properties are initialized. */
	if (UNEXPECTED(CE_STATIC_MEMBERS(ce) == NULL)) {
		zend_class_init_statics(ce);
	}

	ret = CE_STATIC_MEMBERS(ce) + property_info->offset;
	ZVAL_DEINDIRECT(ret);

	if (UNEXPECTED((type == BP_VAR_R || type == BP_VAR_RW)
				&& Z_TYPE_P(ret) == IS_UNDEF && ZEND_TYPE_IS_SET(property_info->type))) {
		zend_throw_error(NULL, "Typed static property %s::$%s must not be accessed before initialization",
			ZSTR_VAL(property_info->ce->name), ZSTR_VAL(property_name));
		return NULL;
	}

	if (UNEXPECTED(ce->ce_flags & ZEND_ACC_TRAIT)) {
		zend_error(E_DEPRECATED,
			"Accessing static trait property %s::$%s is deprecated, "
			"it should only be accessed on a class using the trait",
			ZSTR_VAL(property_info->ce->name), ZSTR_VAL(property_name));
	}

	return ret;
}
/* }}} */

ZEND_API zval *zend_std_get_static_property(zend_class_entry *ce, zend_string *property_name, int type) /* {{{ */
{
	zend_property_info *prop_info;
	return zend_std_get_static_property_with_info(ce, property_name, type, &prop_info);
}

ZEND_API ZEND_COLD bool zend_std_unset_static_property(zend_class_entry *ce, zend_string *property_name) /* {{{ */
{
	zend_throw_error(NULL, "Attempt to unset static property %s::$%s", ZSTR_VAL(ce->name), ZSTR_VAL(property_name));
	return 0;
}
/* }}} */

static ZEND_COLD zend_never_inline void zend_bad_constructor_call(zend_function *constructor, zend_class_entry *scope) /* {{{ */
{
	if (scope) {
		zend_throw_error(NULL, "Call to %s %s::%s() from scope %s",
			zend_visibility_string(constructor->common.fn_flags), ZSTR_VAL(constructor->common.scope->name),
			ZSTR_VAL(constructor->common.function_name), ZSTR_VAL(scope->name)
		);
	} else {
		zend_throw_error(NULL, "Call to %s %s::%s() from global scope", zend_visibility_string(constructor->common.fn_flags), ZSTR_VAL(constructor->common.scope->name), ZSTR_VAL(constructor->common.function_name));
	}
}
/* }}} */

ZEND_API zend_function *zend_std_get_constructor(zend_object *zobj) /* {{{ */
{
	zend_function *constructor = zobj->ce->constructor;
	zend_class_entry *scope;

	if (constructor) {
		if (UNEXPECTED(!(constructor->op_array.fn_flags & ZEND_ACC_PUBLIC))) {
			if (UNEXPECTED(EG(fake_scope))) {
				scope = EG(fake_scope);
			} else {
				scope = zend_get_executed_scope();
			}
			if (UNEXPECTED(constructor->common.scope != scope)) {
				if (UNEXPECTED(constructor->op_array.fn_flags & ZEND_ACC_PRIVATE)
				 || UNEXPECTED(!zend_check_protected(zend_get_function_root_class(constructor), scope))) {
					zend_bad_constructor_call(constructor, scope);
					constructor = NULL;
				}
			}
		}
	}

	return constructor;
}
/* }}} */

ZEND_API int zend_std_compare_objects(zval *o1, zval *o2) /* {{{ */
{
	zend_object *zobj1, *zobj2;

	if (Z_TYPE_P(o1) != Z_TYPE_P(o2)) {
		/* Object and non-object */
		zval *object;
		zval *value;
		zval casted;
		bool object_lhs;
		if (Z_TYPE_P(o1) == IS_OBJECT) {
			object = o1;
			value = o2;
			object_lhs = true;
		} else {
			object = o2;
			value = o1;
			object_lhs = false;
		}
		ZEND_ASSERT(Z_TYPE_P(value) != IS_OBJECT);
		zend_uchar target_type = (Z_TYPE_P(value) == IS_FALSE || Z_TYPE_P(value) == IS_TRUE)
								 ? _IS_BOOL : Z_TYPE_P(value);
		if (Z_OBJ_HT_P(object)->cast_object(Z_OBJ_P(object), &casted, target_type) == FAILURE) {
			// TODO: Less crazy.
			if (target_type == IS_LONG || target_type == IS_DOUBLE) {
				zend_error(E_NOTICE, "Object of class %s could not be converted to %s",
						   ZSTR_VAL(Z_OBJCE_P(object)->name), zend_get_type_by_const(target_type));
				if (target_type == IS_LONG) {
					ZVAL_LONG(&casted, 1);
				} else {
					ZVAL_DOUBLE(&casted, 1.0);
				}
			} else {
				return object_lhs ? 1 : -1;
			}
		}
		int ret = object_lhs ? zend_compare(&casted, value) : zend_compare(value, &casted);
		zval_ptr_dtor(&casted);
		return ret;
	}

	zobj1 = Z_OBJ_P(o1);
	zobj2 = Z_OBJ_P(o2);

	if (zobj1 == zobj2) {
		return 0; /* the same object */
	}
	if (zobj1->ce != zobj2->ce) {
		return ZEND_UNCOMPARABLE; /* different classes */
	}
	if (!zobj1->properties && !zobj2->properties) {
		zend_property_info *info;
		int i;

		if (!zobj1->ce->default_properties_count) {
			return 0;
		}

		/* It's enough to protect only one of the objects.
		 * The second one may be referenced from the first and this may cause
		 * false recursion detection.
		 */
		/* use bitwise OR to make only one conditional jump */
		if (UNEXPECTED(Z_IS_RECURSIVE_P(o1))) {
			zend_error_noreturn(E_ERROR, "Nesting level too deep - recursive dependency?");
		}
		Z_PROTECT_RECURSION_P(o1);

		for (i = 0; i < zobj1->ce->default_properties_count; i++) {
			zval *p1, *p2;

			info = zobj1->ce->properties_info_table[i];

			if (!info) {
				continue;
			}

			p1 = OBJ_PROP(zobj1, info->offset);
			p2 = OBJ_PROP(zobj2, info->offset);

			if (Z_TYPE_P(p1) != IS_UNDEF) {
				if (Z_TYPE_P(p2) != IS_UNDEF) {
					int ret;

					ret = zend_compare(p1, p2);
					if (ret != 0) {
						Z_UNPROTECT_RECURSION_P(o1);
						return ret;
					}
				} else {
					Z_UNPROTECT_RECURSION_P(o1);
					return 1;
				}
			} else {
				if (Z_TYPE_P(p2) != IS_UNDEF) {
					Z_UNPROTECT_RECURSION_P(o1);
					return 1;
				}
			}
		}

		Z_UNPROTECT_RECURSION_P(o1);
		return 0;
	} else {
		if (!zobj1->properties) {
			rebuild_object_properties(zobj1);
		}
		if (!zobj2->properties) {
			rebuild_object_properties(zobj2);
		}
		return zend_compare_symbol_tables(zobj1->properties, zobj2->properties);
	}
}
/* }}} */

ZEND_API int zend_objects_not_comparable(zval *o1, zval *o2)
{
	return ZEND_UNCOMPARABLE;
}

ZEND_API int zend_std_has_property(zend_object *zobj, zend_string *name, int has_set_exists, void **cache_slot) /* {{{ */
{
	int result;
	zval *value = NULL;
	uintptr_t property_offset;
	zend_property_info *prop_info = NULL;
	zend_string *tmp_name = NULL;

	property_offset = zend_get_property_offset(zobj->ce, name, 1, cache_slot, &prop_info);

	if (EXPECTED(IS_VALID_PROPERTY_OFFSET(property_offset))) {
		value = OBJ_PROP(zobj, property_offset);
		if (Z_TYPE_P(value) != IS_UNDEF) {
			goto found;
		}
		if (UNEXPECTED(Z_PROP_FLAG_P(value) == IS_PROP_UNINIT)) {
			/* Skip __isset() for uninitialized typed properties */
			result = 0;
			goto exit;
		}
	} else if (EXPECTED(IS_DYNAMIC_PROPERTY_OFFSET(property_offset))) {
		if (EXPECTED(zobj->properties != NULL)) {
			if (!IS_UNKNOWN_DYNAMIC_PROPERTY_OFFSET(property_offset)) {
				uintptr_t idx = ZEND_DECODE_DYN_PROP_OFFSET(property_offset);

				if (EXPECTED(idx < zobj->properties->nNumUsed * sizeof(Bucket))) {
					Bucket *p = (Bucket*)((char*)zobj->properties->arData + idx);

					if (EXPECTED(p->key == name) ||
				        (EXPECTED(p->h == ZSTR_H(name)) &&
				         EXPECTED(p->key != NULL) &&
				         EXPECTED(zend_string_equal_content(p->key, name)))) {
						value = &p->val;
						goto found;
					}
				}
				CACHE_PTR_EX(cache_slot + 1, (void*)ZEND_DYNAMIC_PROPERTY_OFFSET);
			}
			value = zend_hash_find(zobj->properties, name);
			if (value) {
				if (cache_slot) {
					uintptr_t idx = (char*)value - (char*)zobj->properties->arData;
					CACHE_PTR_EX(cache_slot + 1, (void*)ZEND_ENCODE_DYN_PROP_OFFSET(idx));
				}
found:
				if (has_set_exists == ZEND_PROPERTY_NOT_EMPTY) {
					result = zend_is_true(value);
				} else if (has_set_exists < ZEND_PROPERTY_NOT_EMPTY) {
					ZEND_ASSERT(has_set_exists == ZEND_PROPERTY_ISSET);
					ZVAL_DEREF(value);
					result = (Z_TYPE_P(value) != IS_NULL);
				} else {
					ZEND_ASSERT(has_set_exists == ZEND_PROPERTY_EXISTS);
					result = 1;
				}
				goto exit;
			}
		}
	} else if (UNEXPECTED(EG(exception))) {
		result = 0;
		goto exit;
	}

	result = 0;
	if ((has_set_exists != ZEND_PROPERTY_EXISTS) && zobj->ce->__isset) {
		uint32_t *guard = zend_get_property_guard(zobj, name);

		if (!((*guard) & IN_ISSET)) {
			zval rv;

			/* have issetter - try with it! */
			if (!tmp_name && !ZSTR_IS_INTERNED(name)) {
				tmp_name = zend_string_copy(name);
			}
			GC_ADDREF(zobj);
			(*guard) |= IN_ISSET; /* prevent circular getting */
			zend_std_call_issetter(zobj, name, &rv);
			result = zend_is_true(&rv);
			zval_ptr_dtor(&rv);
			if (has_set_exists == ZEND_PROPERTY_NOT_EMPTY && result) {
				if (EXPECTED(!EG(exception)) && zobj->ce->__get && !((*guard) & IN_GET)) {
					(*guard) |= IN_GET;
					zend_std_call_getter(zobj, name, &rv);
					(*guard) &= ~IN_GET;
					result = i_zend_is_true(&rv);
					zval_ptr_dtor(&rv);
				} else {
					result = 0;
				}
			}
			(*guard) &= ~IN_ISSET;
			OBJ_RELEASE(zobj);
		}
	}

exit:
	zend_tmp_string_release(tmp_name);
	return result;
}
/* }}} */

ZEND_API zend_string *zend_std_get_class_name(const zend_object *zobj) /* {{{ */
{
	return zend_string_copy(zobj->ce->name);
}
/* }}} */

ZEND_API zend_result zend_std_cast_object_tostring(zend_object *readobj, zval *writeobj, int type) /* {{{ */
{
	switch (type) {
		case IS_STRING: {
			zend_class_entry *ce = readobj->ce;
			if (ce->__tostring) {
				zval retval;
				GC_ADDREF(readobj);
				zend_call_known_instance_method_with_0_params(ce->__tostring, readobj, &retval);
				zend_object_release(readobj);
				if (EXPECTED(Z_TYPE(retval) == IS_STRING)) {
					ZVAL_COPY_VALUE(writeobj, &retval);
					return SUCCESS;
				}
				zval_ptr_dtor(&retval);
				if (!EG(exception)) {
					zend_throw_error(NULL, "Method %s::__toString() must return a string value", ZSTR_VAL(ce->name));
				}
			}
			return FAILURE;
		}
		case _IS_BOOL:
			ZVAL_TRUE(writeobj);
			return SUCCESS;
		default:
			return FAILURE;
	}
}
/* }}} */

ZEND_API zend_result zend_std_get_closure(zend_object *obj, zend_class_entry **ce_ptr, zend_function **fptr_ptr, zend_object **obj_ptr, bool check_only) /* {{{ */
{
	zend_class_entry *ce = obj->ce;
	zval *func = zend_hash_find_known_hash(&ce->function_table, ZSTR_KNOWN(ZEND_STR_MAGIC_INVOKE));

	if (func == NULL) {
		return FAILURE;
	}
	*fptr_ptr = Z_FUNC_P(func);

	*ce_ptr = ce;
	if ((*fptr_ptr)->common.fn_flags & ZEND_ACC_STATIC) {
		if (obj_ptr) {
			*obj_ptr = NULL;
		}
	} else {
		if (obj_ptr) {
			*obj_ptr = obj;
		}
	}
	return SUCCESS;
}
/* }}} */

ZEND_API HashTable *zend_std_get_properties_for(zend_object *obj, zend_prop_purpose purpose) {
	HashTable *ht;
	switch (purpose) {
		case ZEND_PROP_PURPOSE_DEBUG:
			if (obj->handlers->get_debug_info) {
				int is_temp;
				ht = obj->handlers->get_debug_info(obj, &is_temp);
				if (ht && !is_temp) {
					GC_TRY_ADDREF(ht);
				}
				return ht;
			}
			ZEND_FALLTHROUGH;
		case ZEND_PROP_PURPOSE_ARRAY_CAST:
		case ZEND_PROP_PURPOSE_SERIALIZE:
		case ZEND_PROP_PURPOSE_VAR_EXPORT:
		case ZEND_PROP_PURPOSE_JSON:
			ht = obj->handlers->get_properties(obj);
			if (ht) {
				GC_TRY_ADDREF(ht);
			}
			return ht;
		default:
			ZEND_UNREACHABLE();
			return NULL;
	}
}

ZEND_API HashTable *zend_get_properties_for(zval *obj, zend_prop_purpose purpose) {
	zend_object *zobj = Z_OBJ_P(obj);

	if (zobj->handlers->get_properties_for) {
		return zobj->handlers->get_properties_for(zobj, purpose);
	}

	return zend_std_get_properties_for(zobj, purpose);
}

ZEND_API const zend_object_handlers std_object_handlers = {
	0,										/* offset */

	zend_object_std_dtor,					/* free_obj */
	zend_objects_destroy_object,			/* dtor_obj */
	zend_objects_clone_obj,					/* clone_obj */

	zend_std_read_property,					/* read_property */
	zend_std_write_property,				/* write_property */
	zend_std_read_dimension,				/* read_dimension */
	zend_std_write_dimension,				/* write_dimension */
	zend_std_get_property_ptr_ptr,			/* get_property_ptr_ptr */
	zend_std_has_property,					/* has_property */
	zend_std_unset_property,				/* unset_property */
	zend_std_has_dimension,					/* has_dimension */
	zend_std_unset_dimension,				/* unset_dimension */
	zend_std_get_properties,				/* get_properties */
	zend_std_get_method,					/* get_method */
	zend_std_get_constructor,				/* get_constructor */
	zend_std_get_class_name,				/* get_class_name */
	zend_std_cast_object_tostring,			/* cast_object */
	NULL,									/* count_elements */
	zend_std_get_debug_info,				/* get_debug_info */
	zend_std_get_closure,					/* get_closure */
	zend_std_get_gc,						/* get_gc */
	NULL,									/* do_operation */
	zend_std_compare_objects,				/* compare */
	NULL,									/* get_properties_for */
};
