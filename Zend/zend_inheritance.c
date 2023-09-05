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
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_API.h"
#include "zend_compile.h"
#include "zend_execute.h"
#include "zend_inheritance.h"
#include "zend_interfaces.h"
#include "zend_smart_str.h"
#include "zend_operators.h"
#include "zend_exceptions.h"
#include "zend_enum.h"
#include "zend_attributes.h"
#include "zend_constants.h"
#include "zend_observer.h"

ZEND_API zend_class_entry* (*zend_inheritance_cache_get)(zend_class_entry *ce, zend_class_entry *parent, zend_class_entry **traits_and_interfaces) = NULL;
ZEND_API zend_class_entry* (*zend_inheritance_cache_add)(zend_class_entry *ce, zend_class_entry *proto, zend_class_entry *parent, zend_class_entry **traits_and_interfaces, HashTable *dependencies) = NULL;

/* Unresolved means that class declarations that are currently not available are needed to
 * determine whether the inheritance is valid or not. At runtime UNRESOLVED should be treated
 * as an ERROR. */
typedef enum {
	INHERITANCE_UNRESOLVED = -1,
	INHERITANCE_ERROR = 0,
	INHERITANCE_WARNING = 1,
	INHERITANCE_SUCCESS = 2,
} inheritance_status;

static void add_dependency_obligation(zend_class_entry *ce, zend_class_entry *dependency_ce);
static void add_compatibility_obligation(
		zend_class_entry *ce, const zend_function *child_fn, zend_class_entry *child_scope,
		const zend_function *parent_fn, zend_class_entry *parent_scope);
static void add_property_compatibility_obligation(
		zend_class_entry *ce, const zend_property_info *child_prop,
		const zend_property_info *parent_prop);
static void add_class_constant_compatibility_obligation(
		zend_class_entry *ce, const zend_class_constant *child_const,
		const zend_class_constant *parent_const, const zend_string *const_name);

static void ZEND_COLD emit_incompatible_method_error(
		const zend_class_entry *ce,
		const zend_function *child, zend_class_entry *child_scope,
		const zend_function *parent, zend_class_entry *parent_scope,
		inheritance_status status);

static void zend_type_copy_ctor(zend_type *const type, bool use_arena, bool persistent);

static void zend_type_list_copy_ctor(
	zend_type *const parent_type,
	bool use_arena,
	bool persistent
) {
	const zend_type_list *const old_list = ZEND_TYPE_LIST(*parent_type);
	size_t size = ZEND_TYPE_LIST_SIZE(old_list->num_types);
	zend_type_list *new_list = use_arena
		? zend_arena_alloc(&CG(arena), size) : pemalloc(size, persistent);

	memcpy(new_list, old_list, size);
	ZEND_TYPE_SET_LIST(*parent_type, new_list);
	if (use_arena) {
		ZEND_TYPE_FULL_MASK(*parent_type) |= _ZEND_TYPE_ARENA_BIT;
	}

	zend_type *list_type;
	ZEND_TYPE_LIST_FOREACH(new_list, list_type) {
		zend_type_copy_ctor(list_type, use_arena, persistent);
	} ZEND_TYPE_LIST_FOREACH_END();
}

static void zend_type_copy_ctor(zend_type *const type, bool use_arena, bool persistent) {
	if (ZEND_TYPE_HAS_LIST(*type)) {
		zend_type_list_copy_ctor(type, use_arena, persistent);
	} else if (ZEND_TYPE_HAS_NAME(*type)) {
		zend_string_addref(ZEND_TYPE_NAME(*type));
	}
}

static zend_function *zend_duplicate_internal_function(zend_function *func, zend_class_entry *ce) /* {{{ */
{
	zend_function *new_function;

	if (UNEXPECTED(ce->type & ZEND_INTERNAL_CLASS)) {
		new_function = pemalloc(sizeof(zend_internal_function), 1);
		memcpy(new_function, func, sizeof(zend_internal_function));
	} else {
		new_function = zend_arena_alloc(&CG(arena), sizeof(zend_internal_function));
		memcpy(new_function, func, sizeof(zend_internal_function));
		new_function->common.fn_flags |= ZEND_ACC_ARENA_ALLOCATED;
	}
	if (EXPECTED(new_function->common.function_name)) {
		zend_string_addref(new_function->common.function_name);
	}
	return new_function;
}
/* }}} */

static zend_always_inline zend_function *zend_duplicate_function(zend_function *func, zend_class_entry *ce) /* {{{ */
{
	if (UNEXPECTED(func->type == ZEND_INTERNAL_FUNCTION)) {
		return zend_duplicate_internal_function(func, ce);
	} else {
		if (func->op_array.refcount) {
			(*func->op_array.refcount)++;
		}
		if (EXPECTED(func->op_array.function_name)) {
			zend_string_addref(func->op_array.function_name);
		}
		return func;
	}
}
/* }}} */

static void do_inherit_parent_constructor(zend_class_entry *ce) /* {{{ */
{
	zend_class_entry *parent = ce->parent;

	ZEND_ASSERT(parent != NULL);

	/* You cannot change create_object */
	ce->create_object = parent->create_object;

	/* Inherit special functions if needed */
	if (EXPECTED(!ce->get_iterator)) {
		ce->get_iterator = parent->get_iterator;
	}
	if (EXPECTED(!ce->__get)) {
		ce->__get = parent->__get;
	}
	if (EXPECTED(!ce->__set)) {
		ce->__set = parent->__set;
	}
	if (EXPECTED(!ce->__unset)) {
		ce->__unset = parent->__unset;
	}
	if (EXPECTED(!ce->__isset)) {
		ce->__isset = parent->__isset;
	}
	if (EXPECTED(!ce->__call)) {
		ce->__call = parent->__call;
	}
	if (EXPECTED(!ce->__callstatic)) {
		ce->__callstatic = parent->__callstatic;
	}
	if (EXPECTED(!ce->__tostring)) {
		ce->__tostring = parent->__tostring;
	}
	if (EXPECTED(!ce->clone)) {
		ce->clone = parent->clone;
	}
	if (EXPECTED(!ce->__serialize)) {
		ce->__serialize = parent->__serialize;
	}
	if (EXPECTED(!ce->__unserialize)) {
		ce->__unserialize = parent->__unserialize;
	}
	if (EXPECTED(!ce->serialize)) {
		ce->serialize = parent->serialize;
	}
	if (EXPECTED(!ce->unserialize)) {
		ce->unserialize = parent->unserialize;
	}
	if (!ce->destructor) {
		ce->destructor = parent->destructor;
	}
	if (EXPECTED(!ce->__debugInfo)) {
		ce->__debugInfo = parent->__debugInfo;
	}

	if (ce->constructor) {
		if (parent->constructor && UNEXPECTED(parent->constructor->common.fn_flags & ZEND_ACC_FINAL)) {
			zend_error_noreturn(E_ERROR, "Cannot override final %s::%s() with %s::%s()",
				ZSTR_VAL(parent->name), ZSTR_VAL(parent->constructor->common.function_name),
				ZSTR_VAL(ce->name), ZSTR_VAL(ce->constructor->common.function_name));
		}
		return;
	}

	ce->constructor = parent->constructor;
}
/* }}} */

char *zend_visibility_string(uint32_t fn_flags) /* {{{ */
{
	if (fn_flags & ZEND_ACC_PUBLIC) {
		return "public";
	} else if (fn_flags & ZEND_ACC_PRIVATE) {
		return "private";
	} else {
		ZEND_ASSERT(fn_flags & ZEND_ACC_PROTECTED);
		return "protected";
	}
}
/* }}} */

static zend_string *resolve_class_name(zend_class_entry *scope, zend_string *name) {
	ZEND_ASSERT(scope);
	if (zend_string_equals_literal_ci(name, "parent") && scope->parent) {
		if (scope->ce_flags & ZEND_ACC_RESOLVED_PARENT) {
			return scope->parent->name;
		} else {
			return scope->parent_name;
		}
	} else if (zend_string_equals_literal_ci(name, "self")) {
		return scope->name;
	} else {
		return name;
	}
}

static bool class_visible(zend_class_entry *ce) {
	if (ce->type == ZEND_INTERNAL_CLASS) {
		return !(CG(compiler_options) & ZEND_COMPILE_IGNORE_INTERNAL_CLASSES);
	} else {
		ZEND_ASSERT(ce->type == ZEND_USER_CLASS);
		return !(CG(compiler_options) & ZEND_COMPILE_IGNORE_OTHER_FILES)
			|| ce->info.user.filename == CG(compiled_filename);
	}
}

static zend_always_inline void register_unresolved_class(zend_string *name) {
	/* We'll autoload this class and process delayed variance obligations later. */
	if (!CG(delayed_autoloads)) {
		ALLOC_HASHTABLE(CG(delayed_autoloads));
		zend_hash_init(CG(delayed_autoloads), 0, NULL, NULL, 0);
	}
	zend_hash_add_empty_element(CG(delayed_autoloads), name);
}

static zend_class_entry *lookup_class_ex(
		zend_class_entry *scope, zend_string *name, bool register_unresolved) {
	zend_class_entry *ce;
	bool in_preload = CG(compiler_options) & ZEND_COMPILE_PRELOAD;

	if (UNEXPECTED(!EG(active) && !in_preload)) {
		zend_string *lc_name = zend_string_tolower(name);

		ce = zend_hash_find_ptr(CG(class_table), lc_name);

		zend_string_release(lc_name);

		if (register_unresolved && !ce) {
			zend_error_noreturn(
				E_COMPILE_ERROR, "%s must be registered before %s",
				ZSTR_VAL(name), ZSTR_VAL(scope->name));
	    }

		return ce;
	}

	ce = zend_lookup_class_ex(
	    name, NULL, ZEND_FETCH_CLASS_ALLOW_UNLINKED | ZEND_FETCH_CLASS_NO_AUTOLOAD);

	if (!CG(in_compilation) || in_preload) {
		if (ce) {
			return ce;
		}

		if (register_unresolved) {
			register_unresolved_class(name);
		}
	} else {
		if (ce && class_visible(ce)) {
			return ce;
		}

		/* The current class may not be registered yet, so check for it explicitly. */
		if (zend_string_equals_ci(scope->name, name)) {
			return scope;
		}
	}

	return NULL;
}

static zend_class_entry *lookup_class(zend_class_entry *scope, zend_string *name) {
	return lookup_class_ex(scope, name, /* register_unresolved */ false);
}

/* Instanceof that's safe to use on unlinked classes. */
static bool unlinked_instanceof(zend_class_entry *ce1, zend_class_entry *ce2) {
	if (ce1 == ce2) {
		return 1;
	}

	if (ce1->ce_flags & ZEND_ACC_LINKED) {
		return instanceof_function(ce1, ce2);
	}

	if (ce1->parent) {
		zend_class_entry *parent_ce;
		if (ce1->ce_flags & ZEND_ACC_RESOLVED_PARENT) {
			parent_ce = ce1->parent;
		} else {
			parent_ce = zend_lookup_class_ex(ce1->parent_name, NULL,
				ZEND_FETCH_CLASS_ALLOW_UNLINKED | ZEND_FETCH_CLASS_NO_AUTOLOAD);
		}

		/* It's not sufficient to only check the parent chain itself, as need to do a full
		 * recursive instanceof in case the parent interfaces haven't been copied yet. */
		if (parent_ce && unlinked_instanceof(parent_ce, ce2)) {
			return 1;
		}
	}

	if (ce1->num_interfaces) {
		uint32_t i;
		if (ce1->ce_flags & ZEND_ACC_RESOLVED_INTERFACES) {
			/* Unlike the normal instanceof_function(), we have to perform a recursive
			 * check here, as the parent interfaces might not have been fully copied yet. */
			for (i = 0; i < ce1->num_interfaces; i++) {
				if (unlinked_instanceof(ce1->interfaces[i], ce2)) {
					return 1;
				}
			}
		} else {
			for (i = 0; i < ce1->num_interfaces; i++) {
				zend_class_entry *ce = zend_lookup_class_ex(
					ce1->interface_names[i].name, ce1->interface_names[i].lc_name,
					ZEND_FETCH_CLASS_ALLOW_UNLINKED | ZEND_FETCH_CLASS_NO_AUTOLOAD);
				/* Avoid recursing if class implements itself. */
				if (ce && ce != ce1 && unlinked_instanceof(ce, ce2)) {
					return 1;
				}
			}
		}
	}

	return 0;
}

static bool zend_type_permits_self(
		zend_type type, zend_class_entry *scope, zend_class_entry *self) {
	if (ZEND_TYPE_FULL_MASK(type) & MAY_BE_OBJECT) {
		return 1;
	}

	/* Any types that may satisfy self must have already been loaded at this point
	 * (as a parent or interface), so we never need to register delayed variance obligations
	 * for this case. */
	zend_type *single_type;
	ZEND_TYPE_FOREACH(type, single_type) {
		if (ZEND_TYPE_HAS_NAME(*single_type)) {
			zend_string *name = resolve_class_name(scope, ZEND_TYPE_NAME(*single_type));
			zend_class_entry *ce = lookup_class(self, name);
			if (ce && unlinked_instanceof(self, ce)) {
				return 1;
			}
		}
	} ZEND_TYPE_FOREACH_END();
	return 0;
}

static void track_class_dependency(zend_class_entry *ce, zend_string *class_name)
{
	HashTable *ht;

	ZEND_ASSERT(class_name);
	if (!CG(current_linking_class) || ce == CG(current_linking_class)) {
		return;
	} else if (zend_string_equals_literal_ci(class_name, "self")
	        || zend_string_equals_literal_ci(class_name, "parent")) {
		return;
	}

#ifndef ZEND_WIN32
	/* On non-Windows systems, internal classes are always the same,
	 * so there is no need to explicitly track them. */
	if (ce->type == ZEND_INTERNAL_CLASS) {
		return;
	}
#endif

	ht = (HashTable*)CG(current_linking_class)->inheritance_cache;

	if (!(ce->ce_flags & ZEND_ACC_IMMUTABLE)) {
		// TODO: dependency on not-immutable class ???
		if (ht) {
			zend_hash_destroy(ht);
			FREE_HASHTABLE(ht);
			CG(current_linking_class)->inheritance_cache = NULL;
		}
		CG(current_linking_class)->ce_flags &= ~ZEND_ACC_CACHEABLE;
		CG(current_linking_class) = NULL;
		return;
	}

	/* Record dependency */
	if (!ht) {
		ALLOC_HASHTABLE(ht);
		zend_hash_init(ht, 0, NULL, NULL, 0);
		CG(current_linking_class)->inheritance_cache = (zend_inheritance_cache_entry*)ht;
	}
	zend_hash_add_ptr(ht, class_name, ce);
}

/* Check whether any type in the fe_type intersection type is a subtype of the proto class. */
static inheritance_status zend_is_intersection_subtype_of_class(
		zend_class_entry *fe_scope, zend_type fe_type,
		zend_class_entry *proto_scope, zend_string *proto_class_name, zend_class_entry *proto_ce)
{
	ZEND_ASSERT(ZEND_TYPE_IS_INTERSECTION(fe_type));
	bool have_unresolved = false;
	zend_type *single_type;

	/* Traverse the list of child types and check that at least one is
	 * a subtype of the parent type being checked */
	ZEND_TYPE_FOREACH(fe_type, single_type) {
		zend_class_entry *fe_ce;
		zend_string *fe_class_name = NULL;
		if (ZEND_TYPE_HAS_NAME(*single_type)) {
			fe_class_name =
				resolve_class_name(fe_scope, ZEND_TYPE_NAME(*single_type));
			if (zend_string_equals_ci(fe_class_name, proto_class_name)) {
				return INHERITANCE_SUCCESS;
			}

			if (!proto_ce) proto_ce = lookup_class(proto_scope, proto_class_name);
			fe_ce = lookup_class(fe_scope, fe_class_name);
		} else {
			/* standard type in an intersection type is impossible,
			 * because it would be a fatal compile error */
			ZEND_UNREACHABLE();
			continue;
		}

		if (!fe_ce || !proto_ce) {
			have_unresolved = true;
			continue;
		}
		if (unlinked_instanceof(fe_ce, proto_ce)) {
			track_class_dependency(fe_ce, fe_class_name);
			track_class_dependency(proto_ce, proto_class_name);
			return INHERITANCE_SUCCESS;
		}
	} ZEND_TYPE_FOREACH_END();

	return have_unresolved ? INHERITANCE_UNRESOLVED : INHERITANCE_ERROR;
}

/* Check whether a single class proto type is a subtype of a potentially complex fe_type. */
static inheritance_status zend_is_class_subtype_of_type(
		zend_class_entry *fe_scope, zend_string *fe_class_name,
		zend_class_entry *proto_scope, zend_type proto_type) {
	zend_class_entry *fe_ce = NULL;
	bool have_unresolved = 0;

	/* If the parent has 'object' as a return type, any class satisfies the co-variant check */
	if (ZEND_TYPE_FULL_MASK(proto_type) & MAY_BE_OBJECT) {
		/* Currently, any class name would be allowed here. We still perform a class lookup
		 * for forward-compatibility reasons, as we may have named types in the future that
		 * are not classes (such as typedefs). */
		if (!fe_ce) fe_ce = lookup_class(fe_scope, fe_class_name);
		if (!fe_ce) {
			have_unresolved = 1;
		} else {
			track_class_dependency(fe_ce, fe_class_name);
			return INHERITANCE_SUCCESS;
		}
	}

	zend_type *single_type;

	/* Traverse the list of parent types and check if the current child (FE)
	 * class is the subtype of at least one of them (union) or all of them (intersection). */
	bool is_intersection = ZEND_TYPE_IS_INTERSECTION(proto_type);
	ZEND_TYPE_FOREACH(proto_type, single_type) {
		if (ZEND_TYPE_IS_INTERSECTION(*single_type)) {
			inheritance_status subtype_status = zend_is_class_subtype_of_type(
				fe_scope, fe_class_name, proto_scope, *single_type);

			switch (subtype_status) {
				case INHERITANCE_ERROR:
					if (is_intersection) {
						return INHERITANCE_ERROR;
					}
					continue;
				case INHERITANCE_UNRESOLVED:
					have_unresolved = 1;
					continue;
				case INHERITANCE_SUCCESS:
					if (!is_intersection) {
						return INHERITANCE_SUCCESS;
					}
					continue;
				EMPTY_SWITCH_DEFAULT_CASE();
			}
		}

		zend_class_entry *proto_ce;
		zend_string *proto_class_name = NULL;
		if (ZEND_TYPE_HAS_NAME(*single_type)) {
			proto_class_name =
				resolve_class_name(proto_scope, ZEND_TYPE_NAME(*single_type));
			if (zend_string_equals_ci(fe_class_name, proto_class_name)) {
				if (!is_intersection) {
					return INHERITANCE_SUCCESS;
				}
				continue;
			}

			if (!fe_ce) fe_ce = lookup_class(fe_scope, fe_class_name);
			proto_ce = lookup_class(proto_scope, proto_class_name);
		} else {
			/* standard type */
			ZEND_ASSERT(!is_intersection);
			continue;
		}

		if (!fe_ce || !proto_ce) {
			have_unresolved = 1;
			continue;
		}
		if (unlinked_instanceof(fe_ce, proto_ce)) {
			track_class_dependency(fe_ce, fe_class_name);
			track_class_dependency(proto_ce, proto_class_name);
			if (!is_intersection) {
				return INHERITANCE_SUCCESS;
			}
		} else {
			if (is_intersection) {
				return INHERITANCE_ERROR;
			}
		}
	} ZEND_TYPE_FOREACH_END();

	if (have_unresolved) {
		return INHERITANCE_UNRESOLVED;
	}
	return is_intersection ? INHERITANCE_SUCCESS : INHERITANCE_ERROR;
}

static zend_string *get_class_from_type(zend_class_entry *scope, zend_type single_type) {
	if (ZEND_TYPE_HAS_NAME(single_type)) {
		return resolve_class_name(scope, ZEND_TYPE_NAME(single_type));
	}
	return NULL;
}

static void register_unresolved_classes(zend_class_entry *scope, zend_type type) {
	zend_type *single_type;
	ZEND_TYPE_FOREACH(type, single_type) {
		if (ZEND_TYPE_HAS_LIST(*single_type)) {
			register_unresolved_classes(scope, *single_type);
			continue;
		}
		if (ZEND_TYPE_HAS_NAME(*single_type)) {
			zend_string *class_name = resolve_class_name(scope, ZEND_TYPE_NAME(*single_type));
			lookup_class_ex(scope, class_name, /* register_unresolved */ true);
		}
	} ZEND_TYPE_FOREACH_END();
}

static inheritance_status zend_is_intersection_subtype_of_type(
	zend_class_entry *fe_scope, zend_type fe_type,
	zend_class_entry *proto_scope, zend_type proto_type)
{
	bool have_unresolved = false;
	zend_type *single_type;
	uint32_t proto_type_mask = ZEND_TYPE_PURE_MASK(proto_type);

	/* Currently, for object type any class name would be allowed here.
	 * We still perform a class lookup for forward-compatibility reasons,
	 * as we may have named types in the future that are not classes
	 * (such as typedefs). */
	if (proto_type_mask & MAY_BE_OBJECT) {
		ZEND_TYPE_FOREACH(fe_type, single_type) {
			zend_string *fe_class_name = get_class_from_type(fe_scope, *single_type);
			if (!fe_class_name) {
				continue;
			}
			zend_class_entry *fe_ce = lookup_class(fe_scope, fe_class_name);
			if (fe_ce) {
				track_class_dependency(fe_ce, fe_class_name);
				return INHERITANCE_SUCCESS;
			} else {
				have_unresolved = true;
			}
		} ZEND_TYPE_FOREACH_END();
	}

	/* U_1&...&U_n < V_1&...&V_m if forall V_j. exists U_i. U_i < V_j.
	 * U_1&...&U_n < V_1|...|V_m if exists V_j. exists U_i. U_i < V_j.
	 * As such, we need to iterate over proto_type (V_j) first and use a different
	 * quantifier depending on whether fe_type is a union or an intersection. */
	inheritance_status early_exit_status =
		ZEND_TYPE_IS_INTERSECTION(proto_type) ? INHERITANCE_ERROR : INHERITANCE_SUCCESS;
	ZEND_TYPE_FOREACH(proto_type, single_type) {
		inheritance_status status;

		if (ZEND_TYPE_IS_INTERSECTION(*single_type)) {
			status = zend_is_intersection_subtype_of_type(
				fe_scope, fe_type, proto_scope, *single_type);
		} else {
			zend_string *proto_class_name = get_class_from_type(proto_scope, *single_type);
			if (!proto_class_name) {
				continue;
			}

			zend_class_entry *proto_ce = NULL;
			status = zend_is_intersection_subtype_of_class(
				fe_scope, fe_type, proto_scope, proto_class_name, proto_ce);
		}

		if (status == early_exit_status) {
			return status;
		}
		if (status == INHERITANCE_UNRESOLVED) {
			have_unresolved = true;
		}
	} ZEND_TYPE_FOREACH_END();

	if (have_unresolved) {
		return INHERITANCE_UNRESOLVED;
	}

	return early_exit_status == INHERITANCE_ERROR ? INHERITANCE_SUCCESS : INHERITANCE_ERROR;
}

static inheritance_status zend_perform_covariant_type_check(
		zend_class_entry *fe_scope, zend_type fe_type,
		zend_class_entry *proto_scope, zend_type proto_type)
{
	ZEND_ASSERT(ZEND_TYPE_IS_SET(fe_type) && ZEND_TYPE_IS_SET(proto_type));

	/* Apart from void, everything is trivially covariant to the mixed type.
	 * Handle this case separately to ensure it never requires class loading. */
	if (ZEND_TYPE_PURE_MASK(proto_type) == MAY_BE_ANY &&
			!ZEND_TYPE_CONTAINS_CODE(fe_type, IS_VOID)) {
		return INHERITANCE_SUCCESS;
	}

	/* Builtin types may be removed, but not added */
	uint32_t fe_type_mask = ZEND_TYPE_PURE_MASK(fe_type);
	uint32_t proto_type_mask = ZEND_TYPE_PURE_MASK(proto_type);
	uint32_t added_types = fe_type_mask & ~proto_type_mask;
	if (added_types) {
		if ((added_types & MAY_BE_STATIC)
				&& zend_type_permits_self(proto_type, proto_scope, fe_scope)) {
			/* Replacing type that accepts self with static is okay */
			added_types &= ~MAY_BE_STATIC;
		}

		if (added_types == MAY_BE_NEVER) {
			/* never is the bottom type */
			return INHERITANCE_SUCCESS;
		}

		if (added_types) {
			/* Otherwise adding new types is illegal */
			return INHERITANCE_ERROR;
		}
	}

	zend_type *single_type;
	inheritance_status early_exit_status;
	bool have_unresolved = false;

	if (ZEND_TYPE_IS_INTERSECTION(fe_type)) {
		early_exit_status =
			ZEND_TYPE_IS_INTERSECTION(proto_type) ? INHERITANCE_ERROR : INHERITANCE_SUCCESS;
		inheritance_status status = zend_is_intersection_subtype_of_type(
			fe_scope, fe_type, proto_scope, proto_type);

		if (status == early_exit_status) {
			return status;
		}
		if (status == INHERITANCE_UNRESOLVED) {
			have_unresolved = true;
		}
	} else {
		/* U_1|...|U_n < V_1|...|V_m if forall U_i. exists V_j. U_i < V_j.
		 * U_1|...|U_n < V_1&...&V_m if forall U_i. forall V_j. U_i < V_j.
		 * We need to iterate over fe_type (U_i) first and the logic is independent of
		 * whether proto_type is a union or intersection (only the inner check differs). */
		early_exit_status = INHERITANCE_ERROR;
		ZEND_TYPE_FOREACH(fe_type, single_type) {
			inheritance_status status;
			/* Union has an intersection type as it's member */
			if (ZEND_TYPE_IS_INTERSECTION(*single_type)) {
				status = zend_is_intersection_subtype_of_type(
					fe_scope, *single_type, proto_scope, proto_type);
			} else {
				zend_string *fe_class_name = get_class_from_type(fe_scope, *single_type);
				if (!fe_class_name) {
					continue;
				}

				status = zend_is_class_subtype_of_type(
					fe_scope, fe_class_name, proto_scope, proto_type);
			}

			if (status == early_exit_status) {
				return status;
			}
			if (status == INHERITANCE_UNRESOLVED) {
				have_unresolved = true;
			}
		} ZEND_TYPE_FOREACH_END();
	}

	if (!have_unresolved) {
		return early_exit_status == INHERITANCE_ERROR ? INHERITANCE_SUCCESS : INHERITANCE_ERROR;
	}

	register_unresolved_classes(fe_scope, fe_type);
	register_unresolved_classes(proto_scope, proto_type);
	return INHERITANCE_UNRESOLVED;
}

static inheritance_status zend_do_perform_arg_type_hint_check(
		zend_class_entry *fe_scope, zend_arg_info *fe_arg_info,
		zend_class_entry *proto_scope, zend_arg_info *proto_arg_info) /* {{{ */
{
	if (!ZEND_TYPE_IS_SET(fe_arg_info->type) || ZEND_TYPE_PURE_MASK(fe_arg_info->type) == MAY_BE_ANY) {
		/* Child with no type or mixed type is always compatible */
		return INHERITANCE_SUCCESS;
	}

	if (!ZEND_TYPE_IS_SET(proto_arg_info->type)) {
		/* Child defines a type, but parent doesn't, violates LSP */
		return INHERITANCE_ERROR;
	}

	/* Contravariant type check is performed as a covariant type check with swapped
	 * argument order. */
	return zend_perform_covariant_type_check(
		proto_scope, proto_arg_info->type, fe_scope, fe_arg_info->type);
}
/* }}} */

/* For trait methods, fe_scope/proto_scope may differ from fe/proto->common.scope,
 * as self will refer to the self of the class the trait is used in, not the trait
 * the method was declared in. */
static inheritance_status zend_do_perform_implementation_check(
		const zend_function *fe, zend_class_entry *fe_scope,
		const zend_function *proto, zend_class_entry *proto_scope) /* {{{ */
{
	uint32_t i, num_args, proto_num_args, fe_num_args;
	inheritance_status status, local_status;
	bool proto_is_variadic, fe_is_variadic;

	/* Checks for constructors only if they are declared in an interface,
	 * or explicitly marked as abstract
	 */
	ZEND_ASSERT(!((fe->common.fn_flags & ZEND_ACC_CTOR)
		&& ((proto->common.scope->ce_flags & ZEND_ACC_INTERFACE) == 0
			&& (proto->common.fn_flags & ZEND_ACC_ABSTRACT) == 0)));

	/* If the prototype method is private and not abstract, we do not enforce a signature.
	 * private abstract methods can only occur in traits. */
	ZEND_ASSERT(!(proto->common.fn_flags & ZEND_ACC_PRIVATE)
			|| (proto->common.fn_flags & ZEND_ACC_ABSTRACT));

	/* The number of required arguments cannot increase. */
	if (proto->common.required_num_args < fe->common.required_num_args) {
		return INHERITANCE_ERROR;
	}

	/* by-ref constraints on return values are covariant */
	if ((proto->common.fn_flags & ZEND_ACC_RETURN_REFERENCE)
		&& !(fe->common.fn_flags & ZEND_ACC_RETURN_REFERENCE)) {
		return INHERITANCE_ERROR;
	}

	proto_is_variadic = (proto->common.fn_flags & ZEND_ACC_VARIADIC) != 0;
	fe_is_variadic = (fe->common.fn_flags & ZEND_ACC_VARIADIC) != 0;

	/* A variadic function cannot become non-variadic */
	if (proto_is_variadic && !fe_is_variadic) {
		return INHERITANCE_ERROR;
	}

	/* The variadic argument is not included in the stored argument count. */
	proto_num_args = proto->common.num_args + proto_is_variadic;
	fe_num_args = fe->common.num_args + fe_is_variadic;
	num_args = MAX(proto_num_args, fe_num_args);

	status = INHERITANCE_SUCCESS;
	for (i = 0; i < num_args; i++) {
		zend_arg_info *proto_arg_info =
			i < proto_num_args ? &proto->common.arg_info[i] :
			proto_is_variadic ? &proto->common.arg_info[proto_num_args - 1] : NULL;
		zend_arg_info *fe_arg_info =
			i < fe_num_args ? &fe->common.arg_info[i] :
			fe_is_variadic ? &fe->common.arg_info[fe_num_args - 1] : NULL;
		if (!proto_arg_info) {
			/* A new (optional) argument has been added, which is fine. */
			continue;
		}
		if (!fe_arg_info) {
			/* An argument has been removed. This is considered illegal, because arity checks
			 * work based on a model where passing more than the declared number of parameters
			 * to a function is an error. */
			return INHERITANCE_ERROR;
		}

		local_status = zend_do_perform_arg_type_hint_check(
			fe_scope, fe_arg_info, proto_scope, proto_arg_info);

		if (UNEXPECTED(local_status != INHERITANCE_SUCCESS)) {
			if (UNEXPECTED(local_status == INHERITANCE_ERROR)) {
				return INHERITANCE_ERROR;
			}
			ZEND_ASSERT(local_status == INHERITANCE_UNRESOLVED);
			status = INHERITANCE_UNRESOLVED;
		}

		/* by-ref constraints on arguments are invariant */
		if (ZEND_ARG_SEND_MODE(fe_arg_info) != ZEND_ARG_SEND_MODE(proto_arg_info)) {
			return INHERITANCE_ERROR;
		}
	}

	/* Check return type compatibility, but only if the prototype already specifies
	 * a return type. Adding a new return type is always valid. */
	if (proto->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE) {
		/* Removing a return type is not valid, unless the parent return type is tentative. */
		if (!(fe->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE)) {
			if (!ZEND_ARG_TYPE_IS_TENTATIVE(&proto->common.arg_info[-1])) {
				return INHERITANCE_ERROR;
			}
			if (status == INHERITANCE_SUCCESS) {
				return INHERITANCE_WARNING;
			}
			return status;
		}

		local_status = zend_perform_covariant_type_check(
			fe_scope, fe->common.arg_info[-1].type, proto_scope, proto->common.arg_info[-1].type);

		if (UNEXPECTED(local_status != INHERITANCE_SUCCESS)) {
			if (local_status == INHERITANCE_ERROR
					&& ZEND_ARG_TYPE_IS_TENTATIVE(&proto->common.arg_info[-1])) {
				local_status = INHERITANCE_WARNING;
			}
			return local_status;
		}
	}

	return status;
}
/* }}} */

static ZEND_COLD void zend_append_type_hint(
		smart_str *str, zend_class_entry *scope, zend_arg_info *arg_info, bool return_hint) /* {{{ */
{
	if (ZEND_TYPE_IS_SET(arg_info->type)) {
		zend_string *type_str = zend_type_to_string_resolved(arg_info->type, scope);
		smart_str_append(str, type_str);
		zend_string_release(type_str);
		if (!return_hint) {
			smart_str_appendc(str, ' ');
		}
	}
}
/* }}} */

static ZEND_COLD zend_string *zend_get_function_declaration(
		const zend_function *fptr, zend_class_entry *scope) /* {{{ */
{
	smart_str str = {0};

	if (fptr->op_array.fn_flags & ZEND_ACC_RETURN_REFERENCE) {
		smart_str_appends(&str, "& ");
	}

	if (fptr->common.scope) {
		if (fptr->common.scope->ce_flags & ZEND_ACC_ANON_CLASS) {
			/* cut off on NULL byte ... class@anonymous */
			smart_str_appends(&str, ZSTR_VAL(fptr->common.scope->name));
		} else {
			smart_str_appendl(&str, ZSTR_VAL(fptr->common.scope->name), ZSTR_LEN(fptr->common.scope->name));
		}
		smart_str_appends(&str, "::");
	}

	smart_str_append(&str, fptr->common.function_name);
	smart_str_appendc(&str, '(');

	if (fptr->common.arg_info) {
		uint32_t i, num_args, required;
		zend_arg_info *arg_info = fptr->common.arg_info;

		required = fptr->common.required_num_args;
		num_args = fptr->common.num_args;
		if (fptr->common.fn_flags & ZEND_ACC_VARIADIC) {
			num_args++;
		}
		for (i = 0; i < num_args;) {
			zend_append_type_hint(&str, scope, arg_info, 0);

			if (ZEND_ARG_SEND_MODE(arg_info)) {
				smart_str_appendc(&str, '&');
			}

			if (ZEND_ARG_IS_VARIADIC(arg_info)) {
				smart_str_appends(&str, "...");
			}

			smart_str_appendc(&str, '$');
			if (fptr->type == ZEND_INTERNAL_FUNCTION) {
				smart_str_appends(&str, ((zend_internal_arg_info*)arg_info)->name);
			} else {
				smart_str_appendl(&str, ZSTR_VAL(arg_info->name), ZSTR_LEN(arg_info->name));
			}

			if (i >= required && !ZEND_ARG_IS_VARIADIC(arg_info)) {
				smart_str_appends(&str, " = ");

				if (fptr->type == ZEND_INTERNAL_FUNCTION) {
					if (((zend_internal_arg_info*)arg_info)->default_value) {
						smart_str_appends(&str, ((zend_internal_arg_info*)arg_info)->default_value);
					} else {
						smart_str_appends(&str, "<default>");
					}
				} else {
					zend_op *precv = NULL;
					{
						uint32_t idx  = i;
						zend_op *op = fptr->op_array.opcodes;
						zend_op *end = op + fptr->op_array.last;

						++idx;
						while (op < end) {
							if ((op->opcode == ZEND_RECV || op->opcode == ZEND_RECV_INIT)
									&& op->op1.num == (zend_ulong)idx)
							{
								precv = op;
							}
							++op;
						}
					}
					if (precv && precv->opcode == ZEND_RECV_INIT && precv->op2_type != IS_UNUSED) {
						zval *zv = RT_CONSTANT(precv, precv->op2);

						if (Z_TYPE_P(zv) == IS_FALSE) {
							smart_str_appends(&str, "false");
						} else if (Z_TYPE_P(zv) == IS_TRUE) {
							smart_str_appends(&str, "true");
						} else if (Z_TYPE_P(zv) == IS_NULL) {
							smart_str_appends(&str, "null");
						} else if (Z_TYPE_P(zv) == IS_STRING) {
							smart_str_appendc(&str, '\'');
							smart_str_appendl(&str, Z_STRVAL_P(zv), MIN(Z_STRLEN_P(zv), 10));
							if (Z_STRLEN_P(zv) > 10) {
								smart_str_appends(&str, "...");
							}
							smart_str_appendc(&str, '\'');
						} else if (Z_TYPE_P(zv) == IS_ARRAY) {
							if (zend_hash_num_elements(Z_ARRVAL_P(zv)) == 0) {
								smart_str_appends(&str, "[]");
							} else {
								smart_str_appends(&str, "[...]");
							}
						} else if (Z_TYPE_P(zv) == IS_CONSTANT_AST) {
							zend_ast *ast = Z_ASTVAL_P(zv);
							if (ast->kind == ZEND_AST_CONSTANT) {
								smart_str_append(&str, zend_ast_get_constant_name(ast));
							} else if (ast->kind == ZEND_AST_CLASS_CONST) {
								smart_str_append(&str, zend_ast_get_str(ast->child[0]));
								smart_str_appends(&str, "::");
								smart_str_append(&str, zend_ast_get_str(ast->child[1]));
							} else {
								smart_str_appends(&str, "<expression>");
							}
						} else {
							zend_string *tmp_zv_str;
							zend_string *zv_str = zval_get_tmp_string(zv, &tmp_zv_str);
							smart_str_append(&str, zv_str);
							zend_tmp_string_release(tmp_zv_str);
						}
					}
				}
			}

			if (++i < num_args) {
				smart_str_appends(&str, ", ");
			}
			arg_info++;
		}
	}

	smart_str_appendc(&str, ')');

	if (fptr->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE) {
		smart_str_appends(&str, ": ");
		zend_append_type_hint(&str, scope, fptr->common.arg_info - 1, 1);
	}
	smart_str_0(&str);

	return str.s;
}
/* }}} */

static zend_always_inline zend_string *func_filename(const zend_function *fn) {
	return fn->common.type == ZEND_USER_FUNCTION ? fn->op_array.filename : NULL;
}

static zend_always_inline uint32_t func_lineno(const zend_function *fn) {
	return fn->common.type == ZEND_USER_FUNCTION ? fn->op_array.line_start : 0;
}

static void ZEND_COLD emit_incompatible_method_error(
		const zend_class_entry *ce,
		const zend_function *child, zend_class_entry *child_scope,
		const zend_function *parent, zend_class_entry *parent_scope,
		inheritance_status status) {
	zend_string *parent_prototype = zend_get_function_declaration(parent, parent_scope);
	zend_string *child_prototype = zend_get_function_declaration(child, child_scope);
	if (status == INHERITANCE_UNRESOLVED) {
		// TODO Improve error message if first unresolved class is present in child and parent?
		/* Fetch the first unresolved class from registered autoloads */
		zend_string *unresolved_class = NULL;
		ZEND_HASH_MAP_FOREACH_STR_KEY(CG(delayed_autoloads), unresolved_class) {
			break;
		} ZEND_HASH_FOREACH_END();
		ZEND_ASSERT(unresolved_class);

		zend_error_at(E_COMPILE_ERROR, func_filename(child), func_lineno(child),
			"Could not check compatibility between %s and %s, because class %s is not available",
			ZSTR_VAL(child_prototype), ZSTR_VAL(parent_prototype), ZSTR_VAL(unresolved_class));
	} else if (status == INHERITANCE_WARNING) {
		zend_attribute *return_type_will_change_attribute = zend_get_attribute_str(
			child->common.attributes,
			"returntypewillchange",
			sizeof("returntypewillchange")-1
		);

		if (!return_type_will_change_attribute) {
			zend_error_at(E_DEPRECATED, func_filename(child), func_lineno(child),
				"Return type of %s should either be compatible with %s, "
				"or the #[\\ReturnTypeWillChange] attribute should be used to temporarily suppress the notice",
				ZSTR_VAL(child_prototype), ZSTR_VAL(parent_prototype));
			if (EG(exception)) {
				zend_exception_uncaught_error(
					"During inheritance of %s", ZSTR_VAL(parent_scope->name));
			}
		}
	} else {
		if (child->op_array.scope->num_traits) {
			zend_error_at(E_COMPILE_ERROR, func_filename(child), func_lineno(child),
				"Declaration of %s (from %s) must be compatible with %s",
				ZSTR_VAL(child_prototype), ZSTR_VAL(child->op_array.scope->trait_names[0].name), ZSTR_VAL(parent_prototype));
		} else if (ce && ce != child_scope && ce != parent_scope) {
			zend_error_at(E_COMPILE_ERROR, func_filename(child), func_lineno(child),
				"While materializing %s: Declaration of %s must be compatible with %s",
				ZSTR_VAL(ce->name), ZSTR_VAL(child_prototype), ZSTR_VAL(parent_prototype));
		} else {
			zend_error_at(E_COMPILE_ERROR, func_filename(child), func_lineno(child),
				"Declaration of %s must be compatible with %s",
				ZSTR_VAL(child_prototype), ZSTR_VAL(parent_prototype));
		}
	}
	zend_string_efree(child_prototype);
	zend_string_efree(parent_prototype);
}

static void perform_delayable_implementation_check(
		zend_class_entry *ce,
		const zend_function *fe, zend_class_entry *fe_scope,
		const zend_function *proto, zend_class_entry *proto_scope)
{
	inheritance_status status =
		zend_do_perform_implementation_check(fe, fe_scope, proto, proto_scope);
	if (UNEXPECTED(status != INHERITANCE_SUCCESS)) {
		if (EXPECTED(status == INHERITANCE_UNRESOLVED)) {
			add_compatibility_obligation(ce, fe, fe_scope, proto, proto_scope);
		} else {
			ZEND_ASSERT(status == INHERITANCE_ERROR || status == INHERITANCE_WARNING);
			emit_incompatible_method_error(ce, fe, fe_scope, proto, proto_scope, status);
		}
	}
}

/**
 * @param check_only Set to false to throw compile errors on incompatible methods, or true to return INHERITANCE_ERROR.
 * @param checked Whether the compatibility check has already succeeded in zend_can_early_bind().
 */
static zend_always_inline inheritance_status do_inheritance_check_on_method_ex(
		zend_function *child, zend_class_entry *child_scope,
		zend_function *parent, zend_class_entry *parent_scope,
		zend_class_entry *ce, zval *child_zv,
		bool check_visibility, bool check_only, bool checked) /* {{{ */
{
	uint32_t child_flags;
	uint32_t parent_flags = parent->common.fn_flags;
	zend_function *proto;

	if (UNEXPECTED((parent_flags & ZEND_ACC_PRIVATE) && !(parent_flags & ZEND_ACC_ABSTRACT) && !(parent_flags & ZEND_ACC_CTOR))) {
		if (!check_only) {
			child->common.fn_flags |= ZEND_ACC_CHANGED;
		}
		/* The parent method is private and not an abstract so we don't need to check any inheritance rules */
		return INHERITANCE_SUCCESS;
	}

	if (!checked && UNEXPECTED(parent_flags & ZEND_ACC_FINAL)) {
		if (check_only) {
			return INHERITANCE_ERROR;
		}
		zend_error_at_noreturn(E_COMPILE_ERROR, func_filename(child), func_lineno(child),
			"Cannot override final method %s::%s()",
			ZEND_FN_SCOPE_NAME(parent), ZSTR_VAL(child->common.function_name));
	}

	child_flags	= child->common.fn_flags;
	/* You cannot change from static to non static and vice versa.
	 */
	if (!checked && UNEXPECTED((child_flags & ZEND_ACC_STATIC) != (parent_flags & ZEND_ACC_STATIC))) {
		if (check_only) {
			return INHERITANCE_ERROR;
		}
		if (child_flags & ZEND_ACC_STATIC) {
			zend_error_at_noreturn(E_COMPILE_ERROR, func_filename(child), func_lineno(child),
				"Cannot make non static method %s::%s() static in class %s",
				ZEND_FN_SCOPE_NAME(parent), ZSTR_VAL(child->common.function_name), ZEND_FN_SCOPE_NAME(child));
		} else {
			zend_error_at_noreturn(E_COMPILE_ERROR, func_filename(child), func_lineno(child),
				"Cannot make static method %s::%s() non static in class %s",
				ZEND_FN_SCOPE_NAME(parent), ZSTR_VAL(child->common.function_name), ZEND_FN_SCOPE_NAME(child));
		}
	}

	/* Disallow making an inherited method abstract. */
	if (!checked && UNEXPECTED((child_flags & ZEND_ACC_ABSTRACT) > (parent_flags & ZEND_ACC_ABSTRACT))) {
		if (check_only) {
			return INHERITANCE_ERROR;
		}
		zend_error_at_noreturn(E_COMPILE_ERROR, func_filename(child), func_lineno(child),
			"Cannot make non abstract method %s::%s() abstract in class %s",
			ZEND_FN_SCOPE_NAME(parent), ZSTR_VAL(child->common.function_name), ZEND_FN_SCOPE_NAME(child));
	}

	if (!check_only && (parent_flags & (ZEND_ACC_PRIVATE|ZEND_ACC_CHANGED))) {
		child->common.fn_flags |= ZEND_ACC_CHANGED;
	}

	proto = parent->common.prototype ?
		parent->common.prototype : parent;

	if (parent_flags & ZEND_ACC_CTOR) {
		/* ctors only have a prototype if is abstract (or comes from an interface) */
		/* and if that is the case, we want to check inheritance against it */
		if (!(proto->common.fn_flags & ZEND_ACC_ABSTRACT)) {
			return INHERITANCE_SUCCESS;
		}
		parent = proto;
	}

	if (!check_only && child->common.prototype != proto && child_zv) {
		do {
			if (child->common.scope != ce && child->type == ZEND_USER_FUNCTION) {
				if (ce->ce_flags & ZEND_ACC_INTERFACE) {
					/* Few parent interfaces contain the same method */
					break;
				} else {
					/* op_array wasn't duplicated yet */
					zend_function *new_function = zend_arena_alloc(&CG(arena), sizeof(zend_op_array));
					memcpy(new_function, child, sizeof(zend_op_array));
					Z_PTR_P(child_zv) = child = new_function;
				}
			}
			child->common.prototype = proto;
		} while (0);
	}

	/* Prevent derived classes from restricting access that was available in parent classes (except deriving from non-abstract ctors) */
	if (!checked && check_visibility
			&& (child_flags & ZEND_ACC_PPP_MASK) > (parent_flags & ZEND_ACC_PPP_MASK)) {
		if (check_only) {
			return INHERITANCE_ERROR;
		}
		zend_error_at_noreturn(E_COMPILE_ERROR, func_filename(child), func_lineno(child),
			"Access level to %s::%s() must be %s (as in class %s)%s",
			ZEND_FN_SCOPE_NAME(child), ZSTR_VAL(child->common.function_name), zend_visibility_string(parent_flags), ZEND_FN_SCOPE_NAME(parent), (parent_flags&ZEND_ACC_PUBLIC) ? "" : " or weaker");
	}

	if (!checked) {
		if (check_only) {
			return zend_do_perform_implementation_check(child, child_scope, parent, parent_scope);
		}
		perform_delayable_implementation_check(ce, child, child_scope, parent, parent_scope);
	}

	if (!check_only && child->common.scope == ce) {
		child->common.fn_flags &= ~ZEND_ACC_OVERRIDE;
	}

	return INHERITANCE_SUCCESS;
}
/* }}} */

static zend_never_inline void do_inheritance_check_on_method(
		zend_function *child, zend_class_entry *child_scope,
		zend_function *parent, zend_class_entry *parent_scope,
		zend_class_entry *ce, zval *child_zv, bool check_visibility)
{
	do_inheritance_check_on_method_ex(child, child_scope, parent, parent_scope, ce, child_zv, check_visibility, 0, 0);
}

static zend_always_inline void do_inherit_method(zend_string *key, zend_function *parent, zend_class_entry *ce, bool is_interface, bool checked) /* {{{ */
{
	zval *child = zend_hash_find_known_hash(&ce->function_table, key);

	if (child) {
		zend_function *func = (zend_function*)Z_PTR_P(child);

		if (is_interface && UNEXPECTED(func == parent)) {
			/* The same method in interface may be inherited few times */
			return;
		}

		if (checked) {
			do_inheritance_check_on_method_ex(
				func, func->common.scope, parent, parent->common.scope, ce, child,
				/* check_visibility */ 1, 0, checked);
		} else {
			do_inheritance_check_on_method(
				func, func->common.scope, parent, parent->common.scope, ce, child,
				/* check_visibility */ 1);
		}
	} else {

		if (is_interface || (parent->common.fn_flags & (ZEND_ACC_ABSTRACT))) {
			ce->ce_flags |= ZEND_ACC_IMPLICIT_ABSTRACT_CLASS;
		}

		parent = zend_duplicate_function(parent, ce);

		if (!is_interface) {
			_zend_hash_append_ptr(&ce->function_table, key, parent);
		} else {
			zend_hash_add_new_ptr(&ce->function_table, key, parent);
		}
	}
}
/* }}} */

inheritance_status property_types_compatible(
		const zend_property_info *parent_info, const zend_property_info *child_info) {
	if (ZEND_TYPE_PURE_MASK(parent_info->type) == ZEND_TYPE_PURE_MASK(child_info->type)
			&& ZEND_TYPE_NAME(parent_info->type) == ZEND_TYPE_NAME(child_info->type)) {
		return INHERITANCE_SUCCESS;
	}

	if (ZEND_TYPE_IS_SET(parent_info->type) != ZEND_TYPE_IS_SET(child_info->type)) {
		return INHERITANCE_ERROR;
	}

	/* Perform a covariant type check in both directions to determined invariance. */
	inheritance_status status1 = zend_perform_covariant_type_check(
		child_info->ce, child_info->type, parent_info->ce, parent_info->type);
	inheritance_status status2 = zend_perform_covariant_type_check(
		parent_info->ce, parent_info->type, child_info->ce, child_info->type);
	if (status1 == INHERITANCE_SUCCESS && status2 == INHERITANCE_SUCCESS) {
		return INHERITANCE_SUCCESS;
	}
	if (status1 == INHERITANCE_ERROR || status2 == INHERITANCE_ERROR) {
		return INHERITANCE_ERROR;
	}
	ZEND_ASSERT(status1 == INHERITANCE_UNRESOLVED || status2 == INHERITANCE_UNRESOLVED);
	return INHERITANCE_UNRESOLVED;
}

static void emit_incompatible_property_error(
		const zend_property_info *child, const zend_property_info *parent) {
	zend_string *type_str = zend_type_to_string_resolved(parent->type, parent->ce);
	zend_error_noreturn(E_COMPILE_ERROR,
		"Type of %s::$%s must be %s (as in class %s)",
		ZSTR_VAL(child->ce->name),
		zend_get_unmangled_property_name(child->name),
		ZSTR_VAL(type_str),
		ZSTR_VAL(parent->ce->name));
}

static void do_inherit_property(zend_property_info *parent_info, zend_string *key, zend_class_entry *ce) /* {{{ */
{
	zval *child = zend_hash_find_known_hash(&ce->properties_info, key);
	zend_property_info *child_info;

	if (UNEXPECTED(child)) {
		child_info = Z_PTR_P(child);
		if (parent_info->flags & (ZEND_ACC_PRIVATE|ZEND_ACC_CHANGED)) {
			child_info->flags |= ZEND_ACC_CHANGED;
		}
		if (!(parent_info->flags & ZEND_ACC_PRIVATE)) {
			if (UNEXPECTED((parent_info->flags & ZEND_ACC_STATIC) != (child_info->flags & ZEND_ACC_STATIC))) {
				zend_error_noreturn(E_COMPILE_ERROR, "Cannot redeclare %s%s::$%s as %s%s::$%s",
					(parent_info->flags & ZEND_ACC_STATIC) ? "static " : "non static ", ZSTR_VAL(parent_info->ce->name), ZSTR_VAL(key),
					(child_info->flags & ZEND_ACC_STATIC) ? "static " : "non static ", ZSTR_VAL(ce->name), ZSTR_VAL(key));
			}
			if (UNEXPECTED((child_info->flags & ZEND_ACC_READONLY) != (parent_info->flags & ZEND_ACC_READONLY))) {
				zend_error_noreturn(E_COMPILE_ERROR,
					"Cannot redeclare %s property %s::$%s as %s %s::$%s",
					parent_info->flags & ZEND_ACC_READONLY ? "readonly" : "non-readonly",
					ZSTR_VAL(parent_info->ce->name), ZSTR_VAL(key),
					child_info->flags & ZEND_ACC_READONLY ? "readonly" : "non-readonly",
					ZSTR_VAL(ce->name), ZSTR_VAL(key));
			}

			if (UNEXPECTED((child_info->flags & ZEND_ACC_PPP_MASK) > (parent_info->flags & ZEND_ACC_PPP_MASK))) {
				zend_error_noreturn(E_COMPILE_ERROR, "Access level to %s::$%s must be %s (as in class %s)%s", ZSTR_VAL(ce->name), ZSTR_VAL(key), zend_visibility_string(parent_info->flags), ZSTR_VAL(parent_info->ce->name), (parent_info->flags&ZEND_ACC_PUBLIC) ? "" : " or weaker");
			} else if ((child_info->flags & ZEND_ACC_STATIC) == 0) {
				int parent_num = OBJ_PROP_TO_NUM(parent_info->offset);
				int child_num = OBJ_PROP_TO_NUM(child_info->offset);

				/* Don't keep default properties in GC (they may be freed by opcache) */
				zval_ptr_dtor_nogc(&(ce->default_properties_table[parent_num]));
				ce->default_properties_table[parent_num] = ce->default_properties_table[child_num];
				ZVAL_UNDEF(&ce->default_properties_table[child_num]);
				child_info->offset = parent_info->offset;
			}

			if (UNEXPECTED(ZEND_TYPE_IS_SET(parent_info->type))) {
				inheritance_status status = property_types_compatible(parent_info, child_info);
				if (status == INHERITANCE_ERROR) {
					emit_incompatible_property_error(child_info, parent_info);
				}
				if (status == INHERITANCE_UNRESOLVED) {
					add_property_compatibility_obligation(ce, child_info, parent_info);
				}
			} else if (UNEXPECTED(ZEND_TYPE_IS_SET(child_info->type) && !ZEND_TYPE_IS_SET(parent_info->type))) {
				zend_error_noreturn(E_COMPILE_ERROR,
						"Type of %s::$%s must not be defined (as in class %s)",
						ZSTR_VAL(ce->name),
						ZSTR_VAL(key),
						ZSTR_VAL(parent_info->ce->name));
			}
		}
	} else {
		_zend_hash_append_ptr(&ce->properties_info, key, parent_info);
	}
}
/* }}} */

static inline void do_implement_interface(zend_class_entry *ce, zend_class_entry *iface) /* {{{ */
{
	if (!(ce->ce_flags & ZEND_ACC_INTERFACE) && iface->interface_gets_implemented && iface->interface_gets_implemented(iface, ce) == FAILURE) {
		zend_error_noreturn(E_CORE_ERROR, "%s %s could not implement interface %s", zend_get_object_type_uc(ce), ZSTR_VAL(ce->name), ZSTR_VAL(iface->name));
	}
	/* This should be prevented by the class lookup logic. */
	ZEND_ASSERT(ce != iface);
}
/* }}} */

static void zend_do_inherit_interfaces(zend_class_entry *ce, const zend_class_entry *iface) /* {{{ */
{
	/* expects interface to be contained in ce's interface list already */
	uint32_t i, ce_num, if_num = iface->num_interfaces;
	zend_class_entry *entry;

	ce_num = ce->num_interfaces;

	if (ce->type == ZEND_INTERNAL_CLASS) {
		ce->interfaces = (zend_class_entry **) realloc(ce->interfaces, sizeof(zend_class_entry *) * (ce_num + if_num));
	} else {
		ce->interfaces = (zend_class_entry **) erealloc(ce->interfaces, sizeof(zend_class_entry *) * (ce_num + if_num));
	}

	/* Inherit the interfaces, only if they're not already inherited by the class */
	while (if_num--) {
		entry = iface->interfaces[if_num];
		for (i = 0; i < ce_num; i++) {
			if (ce->interfaces[i] == entry) {
				break;
			}
		}
		if (i == ce_num) {
			ce->interfaces[ce->num_interfaces++] = entry;
		}
	}
	ce->ce_flags |= ZEND_ACC_RESOLVED_INTERFACES;

	/* and now call the implementing handlers */
	while (ce_num < ce->num_interfaces) {
		do_implement_interface(ce, ce->interfaces[ce_num++]);
	}
}
/* }}} */

static void emit_incompatible_class_constant_error(
		const zend_class_constant *child, const zend_class_constant *parent, const zend_string *const_name) {
	zend_string *type_str = zend_type_to_string_resolved(parent->type, parent->ce);
	zend_error_noreturn(E_COMPILE_ERROR,
		"Type of %s::%s must be compatible with %s::%s of type %s",
		ZSTR_VAL(child->ce->name),
		ZSTR_VAL(const_name),
		ZSTR_VAL(parent->ce->name),
		ZSTR_VAL(const_name),
		ZSTR_VAL(type_str));
}

static inheritance_status class_constant_types_compatible(const zend_class_constant *parent, const zend_class_constant *child)
{
	ZEND_ASSERT(ZEND_TYPE_IS_SET(parent->type));

	if (!ZEND_TYPE_IS_SET(child->type)) {
		return INHERITANCE_ERROR;
	}

	return zend_perform_covariant_type_check(child->ce, child->type, parent->ce, parent->type);
}

static bool do_inherit_constant_check(
	zend_class_entry *ce, zend_class_constant *parent_constant, zend_string *name);

static void do_inherit_class_constant(zend_string *name, zend_class_constant *parent_const, zend_class_entry *ce) /* {{{ */
{
	zval *zv = zend_hash_find_known_hash(&ce->constants_table, name);
	zend_class_constant *c;

	if (zv != NULL) {
		c = (zend_class_constant*)Z_PTR_P(zv);
		bool inherit = do_inherit_constant_check(ce, parent_const, name);
		ZEND_ASSERT(!inherit);
	} else if (!(ZEND_CLASS_CONST_FLAGS(parent_const) & ZEND_ACC_PRIVATE)) {
		if (Z_TYPE(parent_const->value) == IS_CONSTANT_AST) {
			ce->ce_flags &= ~ZEND_ACC_CONSTANTS_UPDATED;
			ce->ce_flags |= ZEND_ACC_HAS_AST_CONSTANTS;
			if (ce->parent->ce_flags & ZEND_ACC_IMMUTABLE) {
				c = zend_arena_alloc(&CG(arena), sizeof(zend_class_constant));
				memcpy(c, parent_const, sizeof(zend_class_constant));
				parent_const = c;
				Z_CONSTANT_FLAGS(c->value) |= CONST_OWNED;
			}
		}
		if (ce->type & ZEND_INTERNAL_CLASS) {
			c = pemalloc(sizeof(zend_class_constant), 1);
			memcpy(c, parent_const, sizeof(zend_class_constant));
			parent_const = c;
		}
		_zend_hash_append_ptr(&ce->constants_table, name, parent_const);
	}
}
/* }}} */

void zend_build_properties_info_table(zend_class_entry *ce)
{
	zend_property_info **table, *prop;
	size_t size;
	if (ce->default_properties_count == 0) {
		return;
	}

	ZEND_ASSERT(ce->properties_info_table == NULL);
	size = sizeof(zend_property_info *) * ce->default_properties_count;
	if (ce->type == ZEND_USER_CLASS) {
		ce->properties_info_table = table = zend_arena_alloc(&CG(arena), size);
	} else {
		ce->properties_info_table = table = pemalloc(size, 1);
	}

	/* Dead slots may be left behind during inheritance. Make sure these are NULLed out. */
	memset(table, 0, size);

	if (ce->parent && ce->parent->default_properties_count != 0) {
		zend_property_info **parent_table = ce->parent->properties_info_table;
		memcpy(
			table, parent_table,
			sizeof(zend_property_info *) * ce->parent->default_properties_count
		);

		/* Child did not add any new properties, we are done */
		if (ce->default_properties_count == ce->parent->default_properties_count) {
			return;
		}
	}

	ZEND_HASH_MAP_FOREACH_PTR(&ce->properties_info, prop) {
		if (prop->ce == ce && (prop->flags & ZEND_ACC_STATIC) == 0) {
			table[OBJ_PROP_TO_NUM(prop->offset)] = prop;
		}
	} ZEND_HASH_FOREACH_END();
}

ZEND_API void zend_do_inheritance_ex(zend_class_entry *ce, zend_class_entry *parent_ce, bool checked) /* {{{ */
{
	zend_property_info *property_info;
	zend_function *func;
	zend_string *key;

	if (UNEXPECTED(ce->ce_flags & ZEND_ACC_INTERFACE)) {
		/* Interface can only inherit other interfaces */
		if (UNEXPECTED(!(parent_ce->ce_flags & ZEND_ACC_INTERFACE))) {
			zend_error_noreturn(E_COMPILE_ERROR, "Interface %s cannot extend class %s", ZSTR_VAL(ce->name), ZSTR_VAL(parent_ce->name));
		}
	} else if (UNEXPECTED(parent_ce->ce_flags & (ZEND_ACC_INTERFACE|ZEND_ACC_TRAIT|ZEND_ACC_FINAL))) {
		/* Class must not extend a final class */
		if (parent_ce->ce_flags & ZEND_ACC_FINAL) {
			zend_error_noreturn(E_COMPILE_ERROR, "Class %s cannot extend final class %s", ZSTR_VAL(ce->name), ZSTR_VAL(parent_ce->name));
		}

		/* Class declaration must not extend traits or interfaces */
		if ((parent_ce->ce_flags & ZEND_ACC_INTERFACE) || (parent_ce->ce_flags & ZEND_ACC_TRAIT)) {
			zend_error_noreturn(E_COMPILE_ERROR, "Class %s cannot extend %s %s",
				ZSTR_VAL(ce->name), parent_ce->ce_flags & ZEND_ACC_INTERFACE ? "interface" : "trait", ZSTR_VAL(parent_ce->name)
			);
		}
	}

	if (UNEXPECTED((ce->ce_flags & ZEND_ACC_READONLY_CLASS) != (parent_ce->ce_flags & ZEND_ACC_READONLY_CLASS))) {
		zend_error_noreturn(E_COMPILE_ERROR, "%s class %s cannot extend %s class %s",
			ce->ce_flags & ZEND_ACC_READONLY_CLASS ? "Readonly" : "Non-readonly", ZSTR_VAL(ce->name),
			parent_ce->ce_flags & ZEND_ACC_READONLY_CLASS ? "readonly" : "non-readonly", ZSTR_VAL(parent_ce->name)
		);
	}

	if (ce->parent_name) {
		zend_string_release_ex(ce->parent_name, 0);
	}
	ce->parent = parent_ce;
	ce->default_object_handlers = parent_ce->default_object_handlers;
	ce->ce_flags |= ZEND_ACC_RESOLVED_PARENT;

	/* Inherit properties */
	if (parent_ce->default_properties_count) {
		zval *src, *dst, *end;

		if (ce->default_properties_count) {
			zval *table = pemalloc(sizeof(zval) * (ce->default_properties_count + parent_ce->default_properties_count), ce->type == ZEND_INTERNAL_CLASS);
			src = ce->default_properties_table + ce->default_properties_count;
			end = table + parent_ce->default_properties_count;
			dst = end + ce->default_properties_count;
			ce->default_properties_table = table;
			do {
				dst--;
				src--;
				ZVAL_COPY_VALUE_PROP(dst, src);
			} while (dst != end);
			pefree(src, ce->type == ZEND_INTERNAL_CLASS);
			end = ce->default_properties_table;
		} else {
			end = pemalloc(sizeof(zval) * parent_ce->default_properties_count, ce->type == ZEND_INTERNAL_CLASS);
			dst = end + parent_ce->default_properties_count;
			ce->default_properties_table = end;
		}
		src = parent_ce->default_properties_table + parent_ce->default_properties_count;
		if (UNEXPECTED(parent_ce->type != ce->type)) {
			/* User class extends internal */
			do {
				dst--;
				src--;
				ZVAL_COPY_OR_DUP_PROP(dst, src);
				if (Z_OPT_TYPE_P(dst) == IS_CONSTANT_AST) {
					ce->ce_flags &= ~ZEND_ACC_CONSTANTS_UPDATED;
					ce->ce_flags |= ZEND_ACC_HAS_AST_PROPERTIES;
				}
				continue;
			} while (dst != end);
		} else {
			do {
				dst--;
				src--;
				ZVAL_COPY_PROP(dst, src);
				if (Z_OPT_TYPE_P(dst) == IS_CONSTANT_AST) {
					ce->ce_flags &= ~ZEND_ACC_CONSTANTS_UPDATED;
					ce->ce_flags |= ZEND_ACC_HAS_AST_PROPERTIES;
				}
				continue;
			} while (dst != end);
		}
		ce->default_properties_count += parent_ce->default_properties_count;
	}

	if (parent_ce->default_static_members_count) {
		zval *src, *dst, *end;

		if (ce->default_static_members_count) {
			zval *table = pemalloc(sizeof(zval) * (ce->default_static_members_count + parent_ce->default_static_members_count), ce->type == ZEND_INTERNAL_CLASS);
			src = ce->default_static_members_table + ce->default_static_members_count;
			end = table + parent_ce->default_static_members_count;
			dst = end + ce->default_static_members_count;
			ce->default_static_members_table = table;
			do {
				dst--;
				src--;
				ZVAL_COPY_VALUE(dst, src);
			} while (dst != end);
			pefree(src, ce->type == ZEND_INTERNAL_CLASS);
			end = ce->default_static_members_table;
		} else {
			end = pemalloc(sizeof(zval) * parent_ce->default_static_members_count, ce->type == ZEND_INTERNAL_CLASS);
			dst = end + parent_ce->default_static_members_count;
			ce->default_static_members_table = end;
		}
		src = parent_ce->default_static_members_table + parent_ce->default_static_members_count;
		do {
			dst--;
			src--;
			if (Z_TYPE_P(src) == IS_INDIRECT) {
				ZVAL_INDIRECT(dst, Z_INDIRECT_P(src));
			} else {
				ZVAL_INDIRECT(dst, src);
			}
			if (Z_TYPE_P(Z_INDIRECT_P(dst)) == IS_CONSTANT_AST) {
				ce->ce_flags &= ~ZEND_ACC_CONSTANTS_UPDATED;
				ce->ce_flags |= ZEND_ACC_HAS_AST_STATICS;
			}
		} while (dst != end);
		ce->default_static_members_count += parent_ce->default_static_members_count;
		if (!ZEND_MAP_PTR(ce->static_members_table)) {
			if (ce->type == ZEND_INTERNAL_CLASS &&
					ce->info.internal.module->type == MODULE_PERSISTENT) {
				ZEND_MAP_PTR_NEW(ce->static_members_table);
			}
		}
	}

	ZEND_HASH_MAP_FOREACH_PTR(&ce->properties_info, property_info) {
		if (property_info->ce == ce) {
			if (property_info->flags & ZEND_ACC_STATIC) {
				property_info->offset += parent_ce->default_static_members_count;
			} else {
				property_info->offset += parent_ce->default_properties_count * sizeof(zval);
			}
		}
	} ZEND_HASH_FOREACH_END();

	if (zend_hash_num_elements(&parent_ce->properties_info)) {
		zend_hash_extend(&ce->properties_info,
			zend_hash_num_elements(&ce->properties_info) +
			zend_hash_num_elements(&parent_ce->properties_info), 0);

		ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&parent_ce->properties_info, key, property_info) {
			do_inherit_property(property_info, key, ce);
		} ZEND_HASH_FOREACH_END();
	}

	if (zend_hash_num_elements(&parent_ce->constants_table)) {
		zend_class_constant *c;

		zend_hash_extend(&ce->constants_table,
			zend_hash_num_elements(&ce->constants_table) +
			zend_hash_num_elements(&parent_ce->constants_table), 0);

		ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&parent_ce->constants_table, key, c) {
			do_inherit_class_constant(key, c, ce);
		} ZEND_HASH_FOREACH_END();
	}

	if (zend_hash_num_elements(&parent_ce->function_table)) {
		zend_hash_extend(&ce->function_table,
			zend_hash_num_elements(&ce->function_table) +
			zend_hash_num_elements(&parent_ce->function_table), 0);

		if (checked) {
			ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&parent_ce->function_table, key, func) {
				do_inherit_method(key, func, ce, 0, 1);
			} ZEND_HASH_FOREACH_END();
		} else {
			ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&parent_ce->function_table, key, func) {
				do_inherit_method(key, func, ce, 0, 0);
			} ZEND_HASH_FOREACH_END();
		}
	}

	do_inherit_parent_constructor(ce);

	if (ce->type == ZEND_INTERNAL_CLASS) {
		if (parent_ce->num_interfaces) {
			zend_do_inherit_interfaces(ce, parent_ce);
		}

		if (ce->ce_flags & ZEND_ACC_IMPLICIT_ABSTRACT_CLASS) {
			ce->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;
		}
	}
	ce->ce_flags |= parent_ce->ce_flags & (ZEND_HAS_STATIC_IN_METHODS | ZEND_ACC_HAS_TYPE_HINTS | ZEND_ACC_HAS_READONLY_PROPS | ZEND_ACC_USE_GUARDS | ZEND_ACC_NOT_SERIALIZABLE | ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES);
}
/* }}} */

static zend_always_inline bool check_trait_property_or_constant_value_compatibility(zend_class_entry *ce, zval *op1, zval *op2) /* {{{ */
{
	bool is_compatible;
	zval op1_tmp, op2_tmp;

	/* if any of the values is a constant, we try to resolve it */
	if (UNEXPECTED(Z_TYPE_P(op1) == IS_CONSTANT_AST)) {
		ZVAL_COPY_OR_DUP(&op1_tmp, op1);
		if (UNEXPECTED(zval_update_constant_ex(&op1_tmp, ce) != SUCCESS)) {
			zval_ptr_dtor(&op1_tmp);
			return false;
		}
		op1 = &op1_tmp;
	}
	if (UNEXPECTED(Z_TYPE_P(op2) == IS_CONSTANT_AST)) {
		ZVAL_COPY_OR_DUP(&op2_tmp, op2);
		if (UNEXPECTED(zval_update_constant_ex(&op2_tmp, ce) != SUCCESS)) {
			zval_ptr_dtor(&op2_tmp);
			return false;
		}
		op2 = &op2_tmp;
	}

	is_compatible = fast_is_identical_function(op1, op2);

	if (op1 == &op1_tmp) {
		zval_ptr_dtor_nogc(&op1_tmp);
	}
	if (op2 == &op2_tmp) {
		zval_ptr_dtor_nogc(&op2_tmp);
	}

	return is_compatible;
}
/* }}} */

/** @return bool Returns true if the class constant should be inherited, i.e. whether it doesn't already exist. */
static bool do_inherit_constant_check(
	zend_class_entry *ce, zend_class_constant *parent_constant, zend_string *name
) {
	zval *zv = zend_hash_find_known_hash(&ce->constants_table, name);
	if (zv == NULL) {
		return true;
	}

	zend_class_constant *child_constant = Z_PTR_P(zv);
	if (parent_constant->ce != child_constant->ce && (ZEND_CLASS_CONST_FLAGS(parent_constant) & ZEND_ACC_FINAL)) {
		zend_error_noreturn(E_COMPILE_ERROR, "%s::%s cannot override final constant %s::%s",
			ZSTR_VAL(child_constant->ce->name), ZSTR_VAL(name),
			ZSTR_VAL(parent_constant->ce->name), ZSTR_VAL(name)
		);
	}

	if (child_constant->ce != parent_constant->ce && child_constant->ce != ce) {
		zend_error_noreturn(E_COMPILE_ERROR,
			"%s %s inherits both %s::%s and %s::%s, which is ambiguous",
			zend_get_object_type_uc(ce),
			ZSTR_VAL(ce->name),
			ZSTR_VAL(child_constant->ce->name), ZSTR_VAL(name),
			ZSTR_VAL(parent_constant->ce->name), ZSTR_VAL(name));
	}

	if (UNEXPECTED((ZEND_CLASS_CONST_FLAGS(child_constant) & ZEND_ACC_PPP_MASK) > (ZEND_CLASS_CONST_FLAGS(parent_constant) & ZEND_ACC_PPP_MASK))) {
		zend_error_noreturn(E_COMPILE_ERROR, "Access level to %s::%s must be %s (as in %s %s)%s",
			ZSTR_VAL(ce->name), ZSTR_VAL(name),
			zend_visibility_string(ZEND_CLASS_CONST_FLAGS(parent_constant)),
			zend_get_object_type(parent_constant->ce),
			ZSTR_VAL(parent_constant->ce->name),
			(ZEND_CLASS_CONST_FLAGS(parent_constant) & ZEND_ACC_PUBLIC) ? "" : " or weaker"
		);
	}

	if (!(ZEND_CLASS_CONST_FLAGS(parent_constant) & ZEND_ACC_PRIVATE) && UNEXPECTED(ZEND_TYPE_IS_SET(parent_constant->type))) {
		inheritance_status status = class_constant_types_compatible(parent_constant, child_constant);
		if (status == INHERITANCE_ERROR) {
			emit_incompatible_class_constant_error(child_constant, parent_constant, name);
		} else if (status == INHERITANCE_UNRESOLVED) {
			add_class_constant_compatibility_obligation(ce, child_constant, parent_constant, name);
		}
	}

	return false;
}
/* }}} */

static void do_inherit_iface_constant(zend_string *name, zend_class_constant *c, zend_class_entry *ce, zend_class_entry *iface) /* {{{ */
{
	if (do_inherit_constant_check(ce, c, name)) {
		zend_class_constant *ct;
		if (Z_TYPE(c->value) == IS_CONSTANT_AST) {
			ce->ce_flags &= ~ZEND_ACC_CONSTANTS_UPDATED;
			ce->ce_flags |= ZEND_ACC_HAS_AST_CONSTANTS;
			if (iface->ce_flags & ZEND_ACC_IMMUTABLE) {
				ct = zend_arena_alloc(&CG(arena), sizeof(zend_class_constant));
				memcpy(ct, c, sizeof(zend_class_constant));
				c = ct;
				Z_CONSTANT_FLAGS(c->value) |= CONST_OWNED;
			}
		}
		if (ce->type & ZEND_INTERNAL_CLASS) {
			ct = pemalloc(sizeof(zend_class_constant), 1);
			memcpy(ct, c, sizeof(zend_class_constant));
			c = ct;
		}
		zend_hash_update_ptr(&ce->constants_table, name, c);
	}
}
/* }}} */

static void do_interface_implementation(zend_class_entry *ce, zend_class_entry *iface) /* {{{ */
{
	zend_function *func;
	zend_string *key;
	zend_class_constant *c;

	ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&iface->constants_table, key, c) {
		do_inherit_iface_constant(key, c, ce, iface);
	} ZEND_HASH_FOREACH_END();

	ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&iface->function_table, key, func) {
		do_inherit_method(key, func, ce, 1, 0);
	} ZEND_HASH_FOREACH_END();

	do_implement_interface(ce, iface);
	if (iface->num_interfaces) {
		zend_do_inherit_interfaces(ce, iface);
	}
}
/* }}} */

ZEND_API void zend_do_implement_interface(zend_class_entry *ce, zend_class_entry *iface) /* {{{ */
{
	uint32_t i, ignore = 0;
	uint32_t current_iface_num = ce->num_interfaces;
	uint32_t parent_iface_num  = ce->parent ? ce->parent->num_interfaces : 0;
	zend_string *key;
	zend_class_constant *c;

	ZEND_ASSERT(ce->ce_flags & ZEND_ACC_LINKED);

	for (i = 0; i < ce->num_interfaces; i++) {
		if (ce->interfaces[i] == NULL) {
			memmove(ce->interfaces + i, ce->interfaces + i + 1, sizeof(zend_class_entry*) * (--ce->num_interfaces - i));
			i--;
		} else if (ce->interfaces[i] == iface) {
			if (EXPECTED(i < parent_iface_num)) {
				ignore = 1;
			} else {
				zend_error_noreturn(E_COMPILE_ERROR, "Class %s cannot implement previously implemented interface %s", ZSTR_VAL(ce->name), ZSTR_VAL(iface->name));
			}
		}
	}
	if (ignore) {
		/* Check for attempt to redeclare interface constants */
		ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&iface->constants_table, key, c) {
			do_inherit_constant_check(ce, c, key);
		} ZEND_HASH_FOREACH_END();
	} else {
		if (ce->num_interfaces >= current_iface_num) {
			if (ce->type == ZEND_INTERNAL_CLASS) {
				ce->interfaces = (zend_class_entry **) realloc(ce->interfaces, sizeof(zend_class_entry *) * (++current_iface_num));
			} else {
				ce->interfaces = (zend_class_entry **) erealloc(ce->interfaces, sizeof(zend_class_entry *) * (++current_iface_num));
			}
		}
		ce->interfaces[ce->num_interfaces++] = iface;

		do_interface_implementation(ce, iface);
	}
}
/* }}} */

static void zend_do_implement_interfaces(zend_class_entry *ce, zend_class_entry **interfaces) /* {{{ */
{
	zend_class_entry *iface;
	uint32_t num_parent_interfaces = ce->parent ? ce->parent->num_interfaces : 0;
	uint32_t num_interfaces = num_parent_interfaces;
	zend_string *key;
	zend_class_constant *c;
	uint32_t i, j;

	for (i = 0; i < ce->num_interfaces; i++) {
		iface = interfaces[num_parent_interfaces + i];
		if (!(iface->ce_flags & ZEND_ACC_LINKED)) {
			add_dependency_obligation(ce, iface);
		}
		if (UNEXPECTED(!(iface->ce_flags & ZEND_ACC_INTERFACE))) {
			efree(interfaces);
			zend_error_noreturn(E_ERROR, "%s cannot implement %s - it is not an interface", ZSTR_VAL(ce->name), ZSTR_VAL(iface->name));
			return;
		}
		for (j = 0; j < num_interfaces; j++) {
			if (interfaces[j] == iface) {
				if (j >= num_parent_interfaces) {
					efree(interfaces);
					zend_error_noreturn(E_COMPILE_ERROR, "%s %s cannot implement previously implemented interface %s",
						zend_get_object_type_uc(ce),
						ZSTR_VAL(ce->name),
						ZSTR_VAL(iface->name));
					return;
				}
				/* skip duplications */
				ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&iface->constants_table, key, c) {
					do_inherit_constant_check(ce, c, key);
				} ZEND_HASH_FOREACH_END();

				iface = NULL;
				break;
			}
		}
		if (iface) {
			interfaces[num_interfaces] = iface;
			num_interfaces++;
		}
	}

	if (!(ce->ce_flags & ZEND_ACC_CACHED)) {
		for (i = 0; i < ce->num_interfaces; i++) {
			zend_string_release_ex(ce->interface_names[i].name, 0);
			zend_string_release_ex(ce->interface_names[i].lc_name, 0);
		}
		efree(ce->interface_names);
	}

	ce->num_interfaces = num_interfaces;
	ce->interfaces = interfaces;
	ce->ce_flags |= ZEND_ACC_RESOLVED_INTERFACES;

	for (i = 0; i < num_parent_interfaces; i++) {
		do_implement_interface(ce, ce->interfaces[i]);
	}
	/* Note that new interfaces can be added during this loop due to interface inheritance.
	 * Use num_interfaces rather than ce->num_interfaces to not re-process the new ones. */
	for (; i < num_interfaces; i++) {
		do_interface_implementation(ce, ce->interfaces[i]);
	}
}
/* }}} */


void zend_inheritance_check_override(zend_class_entry *ce)
{
	zend_function *f;

	if (ce->ce_flags & ZEND_ACC_TRAIT) {
		return;
	}

	ZEND_HASH_MAP_FOREACH_PTR(&ce->function_table, f) {
		if (f->common.fn_flags & ZEND_ACC_OVERRIDE) {
			ZEND_ASSERT(f->type != ZEND_INTERNAL_FUNCTION);

			zend_error_at_noreturn(
				E_COMPILE_ERROR, f->op_array.filename, f->op_array.line_start,
				"%s::%s() has #[\\Override] attribute, but no matching parent method exists",
				ZEND_FN_SCOPE_NAME(f), ZSTR_VAL(f->common.function_name));
		}
	} ZEND_HASH_FOREACH_END();
}


static zend_class_entry *fixup_trait_scope(const zend_function *fn, zend_class_entry *ce)
{
	/* self in trait methods should be resolved to the using class, not the trait. */
	return fn->common.scope->ce_flags & ZEND_ACC_TRAIT ? ce : fn->common.scope;
}

static void zend_add_trait_method(zend_class_entry *ce, zend_string *name, zend_string *key, zend_function *fn) /* {{{ */
{
	zend_function *existing_fn = NULL;
	zend_function *new_fn;

	if ((existing_fn = zend_hash_find_ptr(&ce->function_table, key)) != NULL) {
		/* if it is the same function with the same visibility and has not been assigned a class scope yet, regardless
		 * of where it is coming from there is no conflict and we do not need to add it again */
		if (existing_fn->op_array.opcodes == fn->op_array.opcodes &&
			(existing_fn->common.fn_flags & ZEND_ACC_PPP_MASK) == (fn->common.fn_flags & ZEND_ACC_PPP_MASK) &&
			(existing_fn->common.scope->ce_flags & ZEND_ACC_TRAIT) == ZEND_ACC_TRAIT) {
			return;
		}

		/* Abstract method signatures from the trait must be satisfied. */
		if (fn->common.fn_flags & ZEND_ACC_ABSTRACT) {
			/* "abstract private" methods in traits were not available prior to PHP 8.
			 * As such, "abstract protected" was sometimes used to indicate trait requirements,
			 * even though the "implementing" method was private. Do not check visibility
			 * requirements to maintain backwards-compatibility with such usage.
			 */
			do_inheritance_check_on_method(
				existing_fn, fixup_trait_scope(existing_fn, ce), fn, fixup_trait_scope(fn, ce),
				ce, NULL, /* check_visibility */ 0);
			return;
		}

		if (existing_fn->common.scope == ce) {
			/* members from the current class override trait methods */
			return;
		} else if (UNEXPECTED((existing_fn->common.scope->ce_flags & ZEND_ACC_TRAIT)
				&& !(existing_fn->common.fn_flags & ZEND_ACC_ABSTRACT))) {
			/* two traits can't define the same non-abstract method */
			zend_error_noreturn(E_COMPILE_ERROR, "Trait method %s::%s has not been applied as %s::%s, because of collision with %s::%s",
				ZSTR_VAL(fn->common.scope->name), ZSTR_VAL(fn->common.function_name),
				ZSTR_VAL(ce->name), ZSTR_VAL(name),
				ZSTR_VAL(existing_fn->common.scope->name), ZSTR_VAL(existing_fn->common.function_name));
		} else {
			/* Inherited members are overridden by members inserted by traits.
			 * Check whether the trait method fulfills the inheritance requirements. */
			do_inheritance_check_on_method(
				fn, fixup_trait_scope(fn, ce), existing_fn, fixup_trait_scope(existing_fn, ce),
				ce, NULL, /* check_visibility */ 1);
		}
	}

	if (UNEXPECTED(fn->type == ZEND_INTERNAL_FUNCTION)) {
		new_fn = zend_arena_alloc(&CG(arena), sizeof(zend_internal_function));
		memcpy(new_fn, fn, sizeof(zend_internal_function));
		new_fn->common.fn_flags |= ZEND_ACC_ARENA_ALLOCATED;
	} else {
		new_fn = zend_arena_alloc(&CG(arena), sizeof(zend_op_array));
		memcpy(new_fn, fn, sizeof(zend_op_array));
		new_fn->op_array.fn_flags |= ZEND_ACC_TRAIT_CLONE;
		new_fn->op_array.fn_flags &= ~ZEND_ACC_IMMUTABLE;
	}

	/* Reassign method name, in case it is an alias. */
	new_fn->common.function_name = name;
	function_add_ref(new_fn);
	fn = zend_hash_update_ptr(&ce->function_table, key, new_fn);
	zend_add_magic_method(ce, fn, key);
}
/* }}} */

static void zend_fixup_trait_method(zend_function *fn, zend_class_entry *ce) /* {{{ */
{
	if ((fn->common.scope->ce_flags & ZEND_ACC_TRAIT) == ZEND_ACC_TRAIT) {

		fn->common.scope = ce;

		if (fn->common.fn_flags & ZEND_ACC_ABSTRACT) {
			ce->ce_flags |= ZEND_ACC_IMPLICIT_ABSTRACT_CLASS;
		}
		if (fn->type == ZEND_USER_FUNCTION && fn->op_array.static_variables) {
			ce->ce_flags |= ZEND_HAS_STATIC_IN_METHODS;
		}
	}
}
/* }}} */

static void zend_traits_copy_functions(zend_string *fnname, zend_function *fn, zend_class_entry *ce, HashTable *exclude_table, zend_class_entry **aliases) /* {{{ */
{
	zend_trait_alias  *alias, **alias_ptr;
	zend_string       *lcname;
	zend_function      fn_copy;
	int                i;

	/* apply aliases which are qualified with a class name, there should not be any ambiguity */
	if (ce->trait_aliases) {
		alias_ptr = ce->trait_aliases;
		alias = *alias_ptr;
		i = 0;
		while (alias) {
			/* Scope unset or equal to the function we compare to, and the alias applies to fn */
			if (alias->alias != NULL
				&& fn->common.scope == aliases[i]
				&& zend_string_equals_ci(alias->trait_method.method_name, fnname)
			) {
				fn_copy = *fn;

				/* if it is 0, no modifiers have been changed */
				if (alias->modifiers) {
					fn_copy.common.fn_flags = alias->modifiers | (fn->common.fn_flags & ~ZEND_ACC_PPP_MASK);
				}

				lcname = zend_string_tolower(alias->alias);
				zend_add_trait_method(ce, alias->alias, lcname, &fn_copy);
				zend_string_release_ex(lcname, 0);
			}
			alias_ptr++;
			alias = *alias_ptr;
			i++;
		}
	}

	if (exclude_table == NULL || zend_hash_find(exclude_table, fnname) == NULL) {
		/* is not in hashtable, thus, function is not to be excluded */
		memcpy(&fn_copy, fn, fn->type == ZEND_USER_FUNCTION ? sizeof(zend_op_array) : sizeof(zend_internal_function));

		/* apply aliases which have not alias name, just setting visibility */
		if (ce->trait_aliases) {
			alias_ptr = ce->trait_aliases;
			alias = *alias_ptr;
			i = 0;
			while (alias) {
				/* Scope unset or equal to the function we compare to, and the alias applies to fn */
				if (alias->alias == NULL && alias->modifiers != 0
					&& fn->common.scope == aliases[i]
					&& zend_string_equals_ci(alias->trait_method.method_name, fnname)
				) {
					fn_copy.common.fn_flags = alias->modifiers | (fn->common.fn_flags & ~ZEND_ACC_PPP_MASK);
				}
				alias_ptr++;
				alias = *alias_ptr;
				i++;
			}
		}

		zend_add_trait_method(ce, fn->common.function_name, fnname, &fn_copy);
	}
}
/* }}} */

static uint32_t zend_check_trait_usage(zend_class_entry *ce, zend_class_entry *trait, zend_class_entry **traits) /* {{{ */
{
	uint32_t i;

	if (UNEXPECTED((trait->ce_flags & ZEND_ACC_TRAIT) != ZEND_ACC_TRAIT)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Class %s is not a trait, Only traits may be used in 'as' and 'insteadof' statements", ZSTR_VAL(trait->name));
		return 0;
	}

	for (i = 0; i < ce->num_traits; i++) {
		if (traits[i] == trait) {
			return i;
		}
	}
	zend_error_noreturn(E_COMPILE_ERROR, "Required Trait %s wasn't added to %s", ZSTR_VAL(trait->name), ZSTR_VAL(ce->name));
	return 0;
}
/* }}} */

static void zend_traits_init_trait_structures(zend_class_entry *ce, zend_class_entry **traits, HashTable ***exclude_tables_ptr, zend_class_entry ***aliases_ptr) /* {{{ */
{
	size_t i, j = 0;
	zend_trait_precedence **precedences;
	zend_trait_precedence *cur_precedence;
	zend_trait_method_reference *cur_method_ref;
	zend_string *lc_trait_name;
	zend_string *lcname;
	HashTable **exclude_tables = NULL;
	zend_class_entry **aliases = NULL;
	zend_class_entry *trait;

	/* resolve class references */
	if (ce->trait_precedences) {
		exclude_tables = ecalloc(ce->num_traits, sizeof(HashTable*));
		i = 0;
		precedences = ce->trait_precedences;
		ce->trait_precedences = NULL;
		while ((cur_precedence = precedences[i])) {
			/** Resolve classes for all precedence operations. */
			cur_method_ref = &cur_precedence->trait_method;
			lc_trait_name = zend_string_tolower(cur_method_ref->class_name);
			trait = zend_hash_find_ptr(EG(class_table), lc_trait_name);
			zend_string_release_ex(lc_trait_name, 0);
			if (!trait || !(trait->ce_flags & ZEND_ACC_LINKED)) {
				zend_error_noreturn(E_COMPILE_ERROR, "Could not find trait %s", ZSTR_VAL(cur_method_ref->class_name));
			}
			zend_check_trait_usage(ce, trait, traits);

			/** Ensure that the preferred method is actually available. */
			lcname = zend_string_tolower(cur_method_ref->method_name);
			if (!zend_hash_exists(&trait->function_table, lcname)) {
				zend_error_noreturn(E_COMPILE_ERROR,
						   "A precedence rule was defined for %s::%s but this method does not exist",
						   ZSTR_VAL(trait->name),
						   ZSTR_VAL(cur_method_ref->method_name));
			}

			/** With the other traits, we are more permissive.
				We do not give errors for those. This allows to be more
				defensive in such definitions.
				However, we want to make sure that the insteadof declaration
				is consistent in itself.
			 */

			for (j = 0; j < cur_precedence->num_excludes; j++) {
				zend_string* class_name = cur_precedence->exclude_class_names[j];
				zend_class_entry *exclude_ce;
				uint32_t trait_num;

				lc_trait_name = zend_string_tolower(class_name);
				exclude_ce = zend_hash_find_ptr(EG(class_table), lc_trait_name);
				zend_string_release_ex(lc_trait_name, 0);
				if (!exclude_ce || !(exclude_ce->ce_flags & ZEND_ACC_LINKED)) {
					zend_error_noreturn(E_COMPILE_ERROR, "Could not find trait %s", ZSTR_VAL(class_name));
				}
				trait_num = zend_check_trait_usage(ce, exclude_ce, traits);
				if (!exclude_tables[trait_num]) {
					ALLOC_HASHTABLE(exclude_tables[trait_num]);
					zend_hash_init(exclude_tables[trait_num], 0, NULL, NULL, 0);
				}
				if (zend_hash_add_empty_element(exclude_tables[trait_num], lcname) == NULL) {
					zend_error_noreturn(E_COMPILE_ERROR, "Failed to evaluate a trait precedence (%s). Method of trait %s was defined to be excluded multiple times", ZSTR_VAL(precedences[i]->trait_method.method_name), ZSTR_VAL(exclude_ce->name));
				}

				/* make sure that the trait method is not from a class mentioned in
				 exclude_from_classes, for consistency */
				if (trait == exclude_ce) {
					zend_error_noreturn(E_COMPILE_ERROR,
							   "Inconsistent insteadof definition. "
							   "The method %s is to be used from %s, but %s is also on the exclude list",
							   ZSTR_VAL(cur_method_ref->method_name),
							   ZSTR_VAL(trait->name),
							   ZSTR_VAL(trait->name));
				}
			}
			zend_string_release_ex(lcname, 0);
			i++;
		}
		ce->trait_precedences = precedences;
	}

	if (ce->trait_aliases) {
		i = 0;
		while (ce->trait_aliases[i]) {
			i++;
		}
		aliases = ecalloc(i, sizeof(zend_class_entry*));
		i = 0;
		while (ce->trait_aliases[i]) {
			zend_trait_alias *cur_alias = ce->trait_aliases[i];
			cur_method_ref = &ce->trait_aliases[i]->trait_method;
			lcname = zend_string_tolower(cur_method_ref->method_name);
			if (cur_method_ref->class_name) {
				/* For all aliases with an explicit class name, resolve the class now. */
				lc_trait_name = zend_string_tolower(cur_method_ref->class_name);
				trait = zend_hash_find_ptr(EG(class_table), lc_trait_name);
				zend_string_release_ex(lc_trait_name, 0);
				if (!trait || !(trait->ce_flags & ZEND_ACC_LINKED)) {
					zend_error_noreturn(E_COMPILE_ERROR, "Could not find trait %s", ZSTR_VAL(cur_method_ref->class_name));
				}
				zend_check_trait_usage(ce, trait, traits);
				aliases[i] = trait;

				/* And, ensure that the referenced method is resolvable, too. */
				if (!zend_hash_exists(&trait->function_table, lcname)) {
					zend_error_noreturn(E_COMPILE_ERROR, "An alias was defined for %s::%s but this method does not exist", ZSTR_VAL(trait->name), ZSTR_VAL(cur_method_ref->method_name));
				}
			} else {
				/* Find out which trait this method refers to. */
				trait = NULL;
				for (j = 0; j < ce->num_traits; j++) {
					if (traits[j]) {
						if (zend_hash_exists(&traits[j]->function_table, lcname)) {
							if (!trait) {
								trait = traits[j];
								continue;
							}

							zend_error_noreturn(E_COMPILE_ERROR,
								"An alias was defined for method %s(), which exists in both %s and %s. Use %s::%s or %s::%s to resolve the ambiguity",
								ZSTR_VAL(cur_method_ref->method_name),
								ZSTR_VAL(trait->name), ZSTR_VAL(traits[j]->name),
								ZSTR_VAL(trait->name), ZSTR_VAL(cur_method_ref->method_name),
								ZSTR_VAL(traits[j]->name), ZSTR_VAL(cur_method_ref->method_name));
						}
					}
				}

				/* Non-absolute method reference refers to method that does not exist. */
				if (!trait) {
					if (cur_alias->alias) {
						zend_error_noreturn(E_COMPILE_ERROR,
							"An alias (%s) was defined for method %s(), but this method does not exist",
							ZSTR_VAL(cur_alias->alias),
							ZSTR_VAL(cur_alias->trait_method.method_name));
					} else {
						zend_error_noreturn(E_COMPILE_ERROR,
							"The modifiers of the trait method %s() are changed, but this method does not exist. Error",
							ZSTR_VAL(cur_alias->trait_method.method_name));
					}
				}

				aliases[i] = trait;
			}
			zend_string_release_ex(lcname, 0);
			i++;
		}
	}

	*exclude_tables_ptr = exclude_tables;
	*aliases_ptr = aliases;
}
/* }}} */

static void zend_do_traits_method_binding(zend_class_entry *ce, zend_class_entry **traits, HashTable **exclude_tables, zend_class_entry **aliases) /* {{{ */
{
	uint32_t i;
	zend_string *key;
	zend_function *fn;

	if (exclude_tables) {
		for (i = 0; i < ce->num_traits; i++) {
			if (traits[i]) {
				/* copies functions, applies defined aliasing, and excludes unused trait methods */
				ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&traits[i]->function_table, key, fn) {
					zend_traits_copy_functions(key, fn, ce, exclude_tables[i], aliases);
				} ZEND_HASH_FOREACH_END();

				if (exclude_tables[i]) {
					zend_hash_destroy(exclude_tables[i]);
					FREE_HASHTABLE(exclude_tables[i]);
					exclude_tables[i] = NULL;
				}
			}
		}
	} else {
		for (i = 0; i < ce->num_traits; i++) {
			if (traits[i]) {
				ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&traits[i]->function_table, key, fn) {
					zend_traits_copy_functions(key, fn, ce, NULL, aliases);
				} ZEND_HASH_FOREACH_END();
			}
		}
	}

	ZEND_HASH_MAP_FOREACH_PTR(&ce->function_table, fn) {
		zend_fixup_trait_method(fn, ce);
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

static zend_class_entry* find_first_constant_definition(zend_class_entry *ce, zend_class_entry **traits, size_t current_trait, zend_string *constant_name, zend_class_entry *colliding_ce) /* {{{ */
{
	/* This function is used to show the place of the existing conflicting
	 * definition in error messages when conflicts occur. Since trait constants
	 * are flattened into the constants table of the composing class, and thus
	 * we lose information about which constant was defined in which trait, a
	 * process like this is needed to find the location of the first definition
	 * of the constant from traits.
	 */
	size_t i;

	if (colliding_ce == ce) {
		for (i = 0; i < current_trait; i++) {
			if (traits[i]
				&& zend_hash_exists(&traits[i]->constants_table, constant_name)) {
				return traits[i];
			}
		}
	}
	/* Traits don't have it, then the composing class (or trait) itself has it. */
	return colliding_ce;
}
/* }}} */

static void emit_incompatible_trait_constant_error(
	zend_class_entry *ce, zend_class_constant *existing_constant, zend_class_constant *trait_constant, zend_string *name,
	zend_class_entry **traits, size_t current_trait
) {
	zend_error_noreturn(E_COMPILE_ERROR,
		"%s and %s define the same constant (%s) in the composition of %s. However, the definition differs and is considered incompatible. Class was composed",
		ZSTR_VAL(find_first_constant_definition(ce, traits, current_trait, name, existing_constant->ce)->name),
		ZSTR_VAL(trait_constant->ce->name),
		ZSTR_VAL(name),
		ZSTR_VAL(ce->name)
	);
}

static bool do_trait_constant_check(
	zend_class_entry *ce, zend_class_constant *trait_constant, zend_string *name, zend_class_entry **traits, size_t current_trait
) {
	uint32_t flags_mask = ZEND_ACC_PPP_MASK | ZEND_ACC_FINAL;

	zval *zv = zend_hash_find_known_hash(&ce->constants_table, name);
	if (zv == NULL) {
		/* No existing constant of the same name, so this one can be added */
		return true;
	}

	zend_class_constant *existing_constant = Z_PTR_P(zv);

	if ((ZEND_CLASS_CONST_FLAGS(trait_constant) & flags_mask) != (ZEND_CLASS_CONST_FLAGS(existing_constant) & flags_mask)) {
		emit_incompatible_trait_constant_error(ce, existing_constant, trait_constant, name, traits, current_trait);
		return false;
	}

	if (ZEND_TYPE_IS_SET(trait_constant->type) != ZEND_TYPE_IS_SET(existing_constant->type)) {
		emit_incompatible_trait_constant_error(ce, existing_constant, trait_constant, name, traits, current_trait);
		return false;
	} else if (ZEND_TYPE_IS_SET(trait_constant->type)) {
		inheritance_status status1 = zend_perform_covariant_type_check(ce, existing_constant->type, traits[current_trait], trait_constant->type);
		inheritance_status status2 = zend_perform_covariant_type_check(traits[current_trait], trait_constant->type, ce, existing_constant->type);
		if (status1 == INHERITANCE_ERROR || status2 == INHERITANCE_ERROR) {
			emit_incompatible_trait_constant_error(ce, existing_constant, trait_constant, name, traits, current_trait);
			return false;
		}
	}

	if (!check_trait_property_or_constant_value_compatibility(ce, &trait_constant->value, &existing_constant->value)) {
		/* There is an existing constant of the same name, and it conflicts with the new one, so let's throw a fatal error */
		emit_incompatible_trait_constant_error(ce, existing_constant, trait_constant, name, traits, current_trait);
		return false;
	}

	/* There is an existing constant which is compatible with the new one, so no need to add it */
	return false;
}

static void zend_do_traits_constant_binding(zend_class_entry *ce, zend_class_entry **traits) /* {{{ */
{
	size_t i;

	for (i = 0; i < ce->num_traits; i++) {
		zend_string *constant_name;
		zend_class_constant *constant;

		if (!traits[i]) {
			continue;
		}

		ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&traits[i]->constants_table, constant_name, constant) {
			if (do_trait_constant_check(ce, constant, constant_name, traits, i)) {
				zend_class_constant *ct = NULL;

				ct = zend_arena_alloc(&CG(arena),sizeof(zend_class_constant));
				memcpy(ct, constant, sizeof(zend_class_constant));
				constant = ct;

				if (Z_TYPE(constant->value) == IS_CONSTANT_AST) {
					ce->ce_flags &= ~ZEND_ACC_CONSTANTS_UPDATED;
					ce->ce_flags |= ZEND_ACC_HAS_AST_CONSTANTS;
				}

				/* Unlike interface implementations and class inheritances,
				 * access control of the trait constants is done by the scope
				 * of the composing class. So let's replace the ce here.
				 */
				constant->ce = ce;

				Z_TRY_ADDREF(constant->value);
				constant->doc_comment = constant->doc_comment ? zend_string_copy(constant->doc_comment) : NULL;
				if (constant->attributes && (!(GC_FLAGS(constant->attributes) & IS_ARRAY_IMMUTABLE))) {
					GC_ADDREF(constant->attributes);
				}

				zend_hash_update_ptr(&ce->constants_table, constant_name, constant);
			}
		} ZEND_HASH_FOREACH_END();
	}
}
/* }}} */

static zend_class_entry* find_first_property_definition(zend_class_entry *ce, zend_class_entry **traits, size_t current_trait, zend_string *prop_name, zend_class_entry *colliding_ce) /* {{{ */
{
	size_t i;

	if (colliding_ce == ce) {
		for (i = 0; i < current_trait; i++) {
			if (traits[i]
			 && zend_hash_exists(&traits[i]->properties_info, prop_name)) {
				return traits[i];
			}
		}
	}

	return colliding_ce;
}
/* }}} */

static void zend_do_traits_property_binding(zend_class_entry *ce, zend_class_entry **traits) /* {{{ */
{
	size_t i;
	zend_property_info *property_info;
	zend_property_info *colliding_prop;
	zend_property_info *new_prop;
	zend_string* prop_name;
	zval* prop_value;
	zend_string *doc_comment;

	/* In the following steps the properties are inserted into the property table
	 * for that, a very strict approach is applied:
	 * - check for compatibility, if not compatible with any property in class -> fatal
	 * - if compatible, then strict notice
	 */
	for (i = 0; i < ce->num_traits; i++) {
		if (!traits[i]) {
			continue;
		}
		ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&traits[i]->properties_info, prop_name, property_info) {
			uint32_t flags = property_info->flags;

			/* next: check for conflicts with current class */
			if ((colliding_prop = zend_hash_find_ptr(&ce->properties_info, prop_name)) != NULL) {
				if ((colliding_prop->flags & ZEND_ACC_PRIVATE) && colliding_prop->ce != ce) {
					zend_hash_del(&ce->properties_info, prop_name);
					flags |= ZEND_ACC_CHANGED;
				} else {
					bool is_compatible = false;
					uint32_t flags_mask = ZEND_ACC_PPP_MASK | ZEND_ACC_STATIC | ZEND_ACC_READONLY;

					if ((colliding_prop->flags & flags_mask) == (flags & flags_mask) &&
						property_types_compatible(property_info, colliding_prop) == INHERITANCE_SUCCESS
					) {
						/* the flags are identical, thus, the properties may be compatible */
						zval *op1, *op2;

						if (flags & ZEND_ACC_STATIC) {
							op1 = &ce->default_static_members_table[colliding_prop->offset];
							op2 = &traits[i]->default_static_members_table[property_info->offset];
							ZVAL_DEINDIRECT(op1);
							ZVAL_DEINDIRECT(op2);
						} else {
							op1 = &ce->default_properties_table[OBJ_PROP_TO_NUM(colliding_prop->offset)];
							op2 = &traits[i]->default_properties_table[OBJ_PROP_TO_NUM(property_info->offset)];
						}
						is_compatible = check_trait_property_or_constant_value_compatibility(ce, op1, op2);
					}

					if (!is_compatible) {
						zend_error_noreturn(E_COMPILE_ERROR,
								"%s and %s define the same property ($%s) in the composition of %s. However, the definition differs and is considered incompatible. Class was composed",
								ZSTR_VAL(find_first_property_definition(ce, traits, i, prop_name, colliding_prop->ce)->name),
								ZSTR_VAL(property_info->ce->name),
								ZSTR_VAL(prop_name),
								ZSTR_VAL(ce->name));
					}
					if (!(flags & ZEND_ACC_STATIC)) {
						continue;
					}
				}
			}

			if ((ce->ce_flags & ZEND_ACC_READONLY_CLASS) && !(property_info->flags & ZEND_ACC_READONLY)) {
				zend_error_noreturn(E_COMPILE_ERROR,
					"Readonly class %s cannot use trait with a non-readonly property %s::$%s",
					ZSTR_VAL(ce->name),
					ZSTR_VAL(property_info->ce->name),
					ZSTR_VAL(prop_name)
				);
			}

			/* property not found, so lets add it */
			if (flags & ZEND_ACC_STATIC) {
				prop_value = &traits[i]->default_static_members_table[property_info->offset];
				ZEND_ASSERT(Z_TYPE_P(prop_value) != IS_INDIRECT);
			} else {
				prop_value = &traits[i]->default_properties_table[OBJ_PROP_TO_NUM(property_info->offset)];
			}

			Z_TRY_ADDREF_P(prop_value);
			doc_comment = property_info->doc_comment ? zend_string_copy(property_info->doc_comment) : NULL;

			zend_type type = property_info->type;
			/* Assumption: only userland classes can use traits, as such the type must be arena allocated */
			zend_type_copy_ctor(&type, /* use arena */ true, /* persistent */ false);
			new_prop = zend_declare_typed_property(ce, prop_name, prop_value, flags, doc_comment, type);

			if (property_info->attributes) {
				new_prop->attributes = property_info->attributes;

				if (!(GC_FLAGS(new_prop->attributes) & IS_ARRAY_IMMUTABLE)) {
					GC_ADDREF(new_prop->attributes);
				}
			}
		} ZEND_HASH_FOREACH_END();
	}
}
/* }}} */

static void zend_do_bind_traits(zend_class_entry *ce, zend_class_entry **traits) /* {{{ */
{
	HashTable **exclude_tables;
	zend_class_entry **aliases;

	ZEND_ASSERT(ce->num_traits > 0);

	/* complete initialization of trait structures in ce */
	zend_traits_init_trait_structures(ce, traits, &exclude_tables, &aliases);

	/* first care about all methods to be flattened into the class */
	zend_do_traits_method_binding(ce, traits, exclude_tables, aliases);

	if (aliases) {
		efree(aliases);
	}

	if (exclude_tables) {
		efree(exclude_tables);
	}

	/* then flatten the constants and properties into it, to, mostly to notify developer about problems */
	zend_do_traits_constant_binding(ce, traits);
	zend_do_traits_property_binding(ce, traits);
}
/* }}} */

#define MAX_ABSTRACT_INFO_CNT 3
#define MAX_ABSTRACT_INFO_FMT "%s%s%s%s"
#define DISPLAY_ABSTRACT_FN(idx) \
	ai.afn[idx] ? ZEND_FN_SCOPE_NAME(ai.afn[idx]) : "", \
	ai.afn[idx] ? "::" : "", \
	ai.afn[idx] ? ZSTR_VAL(ai.afn[idx]->common.function_name) : "", \
	ai.afn[idx] && ai.afn[idx + 1] ? ", " : (ai.afn[idx] && ai.cnt > MAX_ABSTRACT_INFO_CNT ? ", ..." : "")

typedef struct _zend_abstract_info {
	zend_function *afn[MAX_ABSTRACT_INFO_CNT + 1];
	int cnt;
} zend_abstract_info;

static void zend_verify_abstract_class_function(zend_function *fn, zend_abstract_info *ai) /* {{{ */
{
	if (ai->cnt < MAX_ABSTRACT_INFO_CNT) {
		ai->afn[ai->cnt] = fn;
	}
	ai->cnt++;
}
/* }}} */

void zend_verify_abstract_class(zend_class_entry *ce) /* {{{ */
{
	zend_function *func;
	zend_abstract_info ai;
	bool is_explicit_abstract = (ce->ce_flags & ZEND_ACC_EXPLICIT_ABSTRACT_CLASS) != 0;
	bool can_be_abstract = (ce->ce_flags & ZEND_ACC_ENUM) == 0;
	memset(&ai, 0, sizeof(ai));

	ZEND_HASH_MAP_FOREACH_PTR(&ce->function_table, func) {
		if (func->common.fn_flags & ZEND_ACC_ABSTRACT) {
			/* If the class is explicitly abstract, we only check private abstract methods,
			 * because only they must be declared in the same class. */
			if (!is_explicit_abstract || (func->common.fn_flags & ZEND_ACC_PRIVATE)) {
				zend_verify_abstract_class_function(func, &ai);
			}
		}
	} ZEND_HASH_FOREACH_END();

	if (ai.cnt) {
		zend_error_noreturn(E_ERROR, !is_explicit_abstract && can_be_abstract
			? "%s %s contains %d abstract method%s and must therefore be declared abstract or implement the remaining methods (" MAX_ABSTRACT_INFO_FMT MAX_ABSTRACT_INFO_FMT MAX_ABSTRACT_INFO_FMT ")"
			: "%s %s must implement %d abstract private method%s (" MAX_ABSTRACT_INFO_FMT MAX_ABSTRACT_INFO_FMT MAX_ABSTRACT_INFO_FMT ")",
			zend_get_object_type_uc(ce),
			ZSTR_VAL(ce->name), ai.cnt,
			ai.cnt > 1 ? "s" : "",
			DISPLAY_ABSTRACT_FN(0),
			DISPLAY_ABSTRACT_FN(1),
			DISPLAY_ABSTRACT_FN(2)
			);
	} else {
		/* now everything should be fine and an added ZEND_ACC_IMPLICIT_ABSTRACT_CLASS should be removed */
		ce->ce_flags &= ~ZEND_ACC_IMPLICIT_ABSTRACT_CLASS;
	}
}
/* }}} */

typedef struct {
	enum {
		OBLIGATION_DEPENDENCY,
		OBLIGATION_COMPATIBILITY,
		OBLIGATION_PROPERTY_COMPATIBILITY,
		OBLIGATION_CLASS_CONSTANT_COMPATIBILITY
	} type;
	union {
		zend_class_entry *dependency_ce;
		struct {
			/* Traits may use temporary on-stack functions during inheritance checks,
			 * so use copies of functions here as well. */
			zend_function parent_fn;
			zend_function child_fn;
			zend_class_entry *child_scope;
			zend_class_entry *parent_scope;
		};
		struct {
			const zend_property_info *parent_prop;
			const zend_property_info *child_prop;
		};
		struct {
			const zend_string *const_name;
			const zend_class_constant *parent_const;
			const zend_class_constant *child_const;
		};
	};
} variance_obligation;

static void variance_obligation_dtor(zval *zv) {
	efree(Z_PTR_P(zv));
}

static void variance_obligation_ht_dtor(zval *zv) {
	zend_hash_destroy(Z_PTR_P(zv));
	FREE_HASHTABLE(Z_PTR_P(zv));
}

static HashTable *get_or_init_obligations_for_class(zend_class_entry *ce) {
	HashTable *ht;
	zend_ulong key;
	if (!CG(delayed_variance_obligations)) {
		ALLOC_HASHTABLE(CG(delayed_variance_obligations));
		zend_hash_init(CG(delayed_variance_obligations), 0, NULL, variance_obligation_ht_dtor, 0);
	}

	key = (zend_ulong) (uintptr_t) ce;
	ht = zend_hash_index_find_ptr(CG(delayed_variance_obligations), key);
	if (ht) {
		return ht;
	}

	ALLOC_HASHTABLE(ht);
	zend_hash_init(ht, 0, NULL, variance_obligation_dtor, 0);
	zend_hash_index_add_new_ptr(CG(delayed_variance_obligations), key, ht);
	ce->ce_flags |= ZEND_ACC_UNRESOLVED_VARIANCE;
	return ht;
}

static void add_dependency_obligation(zend_class_entry *ce, zend_class_entry *dependency_ce) {
	HashTable *obligations = get_or_init_obligations_for_class(ce);
	variance_obligation *obligation = emalloc(sizeof(variance_obligation));
	obligation->type = OBLIGATION_DEPENDENCY;
	obligation->dependency_ce = dependency_ce;
	zend_hash_next_index_insert_ptr(obligations, obligation);
}

static void add_compatibility_obligation(
		zend_class_entry *ce,
		const zend_function *child_fn, zend_class_entry *child_scope,
		const zend_function *parent_fn, zend_class_entry *parent_scope) {
	HashTable *obligations = get_or_init_obligations_for_class(ce);
	variance_obligation *obligation = emalloc(sizeof(variance_obligation));
	obligation->type = OBLIGATION_COMPATIBILITY;
	/* Copy functions, because they may be stack-allocated in the case of traits. */
	if (child_fn->common.type == ZEND_INTERNAL_FUNCTION) {
		memcpy(&obligation->child_fn, child_fn, sizeof(zend_internal_function));
	} else {
		memcpy(&obligation->child_fn, child_fn, sizeof(zend_op_array));
	}
	if (parent_fn->common.type == ZEND_INTERNAL_FUNCTION) {
		memcpy(&obligation->parent_fn, parent_fn, sizeof(zend_internal_function));
	} else {
		memcpy(&obligation->parent_fn, parent_fn, sizeof(zend_op_array));
	}
	obligation->child_scope = child_scope;
	obligation->parent_scope = parent_scope;
	zend_hash_next_index_insert_ptr(obligations, obligation);
}

static void add_property_compatibility_obligation(
		zend_class_entry *ce, const zend_property_info *child_prop,
		const zend_property_info *parent_prop) {
	HashTable *obligations = get_or_init_obligations_for_class(ce);
	variance_obligation *obligation = emalloc(sizeof(variance_obligation));
	obligation->type = OBLIGATION_PROPERTY_COMPATIBILITY;
	obligation->child_prop = child_prop;
	obligation->parent_prop = parent_prop;
	zend_hash_next_index_insert_ptr(obligations, obligation);
}

static void add_class_constant_compatibility_obligation(
		zend_class_entry *ce, const zend_class_constant *child_const,
		const zend_class_constant *parent_const, const zend_string *const_name) {
	HashTable *obligations = get_or_init_obligations_for_class(ce);
	variance_obligation *obligation = emalloc(sizeof(variance_obligation));
	obligation->type = OBLIGATION_CLASS_CONSTANT_COMPATIBILITY;
	obligation->const_name = const_name;
	obligation->child_const = child_const;
	obligation->parent_const = parent_const;
	zend_hash_next_index_insert_ptr(obligations, obligation);
}

static void resolve_delayed_variance_obligations(zend_class_entry *ce);

static void check_variance_obligation(variance_obligation *obligation) {
	if (obligation->type == OBLIGATION_DEPENDENCY) {
		zend_class_entry *dependency_ce = obligation->dependency_ce;
		if (dependency_ce->ce_flags & ZEND_ACC_UNRESOLVED_VARIANCE) {
			zend_class_entry *orig_linking_class = CG(current_linking_class);

			CG(current_linking_class) =
				(dependency_ce->ce_flags & ZEND_ACC_CACHEABLE) ? dependency_ce : NULL;
			resolve_delayed_variance_obligations(dependency_ce);
			CG(current_linking_class) = orig_linking_class;
		}
	} else if (obligation->type == OBLIGATION_COMPATIBILITY) {
		inheritance_status status = zend_do_perform_implementation_check(
			&obligation->child_fn, obligation->child_scope,
			&obligation->parent_fn, obligation->parent_scope);
		if (UNEXPECTED(status != INHERITANCE_SUCCESS)) {
			emit_incompatible_method_error(
				obligation->parent_scope,
				&obligation->child_fn, obligation->child_scope,
				&obligation->parent_fn, obligation->parent_scope, status);
		}
		/* Either the compatibility check was successful or only threw a warning. */
	} else if (obligation->type == OBLIGATION_PROPERTY_COMPATIBILITY) {
		inheritance_status status =
			property_types_compatible(obligation->parent_prop, obligation->child_prop);
		if (status != INHERITANCE_SUCCESS) {
			emit_incompatible_property_error(obligation->child_prop, obligation->parent_prop);
		}
	} else {
		ZEND_ASSERT(obligation->type == OBLIGATION_CLASS_CONSTANT_COMPATIBILITY);
		inheritance_status status =
		class_constant_types_compatible(obligation->parent_const, obligation->child_const);
		if (status != INHERITANCE_SUCCESS) {
			emit_incompatible_class_constant_error(obligation->child_const, obligation->parent_const, obligation->const_name);
		}
	}
}

static void load_delayed_classes(zend_class_entry *ce) {
	HashTable *delayed_autoloads = CG(delayed_autoloads);
	if (!delayed_autoloads) {
		return;
	}

	/* Autoloading can trigger linking of another class, which may register new delayed autoloads.
	 * For that reason, this code uses a loop that pops and loads the first element of the HT. If
	 * this triggers linking, then the remaining classes may get loaded when linking the newly
	 * loaded class. This is important, as otherwise necessary dependencies may not be available
	 * if the new class is lower in the hierarchy than the current one. */
	HashPosition pos = 0;
	zend_string *name;
	zend_ulong idx;
	while (zend_hash_get_current_key_ex(delayed_autoloads, &name, &idx, &pos)
			!= HASH_KEY_NON_EXISTENT) {
		zend_string_addref(name);
		zend_hash_del(delayed_autoloads, name);
		zend_lookup_class(name);
		zend_string_release(name);
		if (EG(exception)) {
			zend_exception_uncaught_error(
				"During inheritance of %s, while autoloading %s",
				ZSTR_VAL(ce->name), ZSTR_VAL(name));
		}
	}
}

static void resolve_delayed_variance_obligations(zend_class_entry *ce) {
	HashTable *all_obligations = CG(delayed_variance_obligations), *obligations;
	zend_ulong num_key = (zend_ulong) (uintptr_t) ce;

	ZEND_ASSERT(all_obligations != NULL);
	obligations = zend_hash_index_find_ptr(all_obligations, num_key);
	ZEND_ASSERT(obligations != NULL);

	variance_obligation *obligation;
	ZEND_HASH_FOREACH_PTR(obligations, obligation) {
		check_variance_obligation(obligation);
	} ZEND_HASH_FOREACH_END();

	zend_inheritance_check_override(ce);

	ce->ce_flags &= ~ZEND_ACC_UNRESOLVED_VARIANCE;
	ce->ce_flags |= ZEND_ACC_LINKED;
	zend_hash_index_del(all_obligations, num_key);
}

static void check_unrecoverable_load_failure(zend_class_entry *ce) {
	/* If this class has been used while unlinked through a variance obligation, it is not legal
	 * to remove the class from the class table and throw an exception, because there is already
	 * a dependence on the inheritance hierarchy of this specific class. Instead we fall back to
	 * a fatal error, as would happen if we did not allow exceptions in the first place. */
	if (CG(unlinked_uses)
			&& zend_hash_index_del(CG(unlinked_uses), (zend_long)(uintptr_t)ce) == SUCCESS) {
		zend_exception_uncaught_error(
			"During inheritance of %s with variance dependencies", ZSTR_VAL(ce->name));
	}
}

#define zend_update_inherited_handler(handler) do { \
		if (ce->handler == (zend_function*)op_array) { \
			ce->handler = (zend_function*)new_op_array; \
		} \
	} while (0)

static zend_class_entry *zend_lazy_class_load(zend_class_entry *pce)
{
	zend_class_entry *ce;
	Bucket *p, *end;

	ce = zend_arena_alloc(&CG(arena), sizeof(zend_class_entry));
	memcpy(ce, pce, sizeof(zend_class_entry));
	ce->ce_flags &= ~ZEND_ACC_IMMUTABLE;
	ce->refcount = 1;
	ce->inheritance_cache = NULL;
	if (CG(compiler_options) & ZEND_COMPILE_PRELOAD) {
		ZEND_MAP_PTR_NEW(ce->mutable_data);
	} else {
		ZEND_MAP_PTR_INIT(ce->mutable_data, NULL);
	}

	/* properties */
	if (ce->default_properties_table) {
		zval *dst = emalloc(sizeof(zval) * ce->default_properties_count);
		zval *src = ce->default_properties_table;
		zval *end = src + ce->default_properties_count;

		ce->default_properties_table = dst;
		for (; src != end; src++, dst++) {
			ZVAL_COPY_VALUE_PROP(dst, src);
		}
	}

	/* methods */
	ce->function_table.pDestructor = ZEND_FUNCTION_DTOR;
	if (!(HT_FLAGS(&ce->function_table) & HASH_FLAG_UNINITIALIZED)) {
		p = emalloc(HT_SIZE(&ce->function_table));
		memcpy(p, HT_GET_DATA_ADDR(&ce->function_table), HT_USED_SIZE(&ce->function_table));
		HT_SET_DATA_ADDR(&ce->function_table, p);
		p = ce->function_table.arData;
		end = p + ce->function_table.nNumUsed;
		for (; p != end; p++) {
			zend_op_array *op_array, *new_op_array;

			op_array = Z_PTR(p->val);
			ZEND_ASSERT(op_array->type == ZEND_USER_FUNCTION);
			ZEND_ASSERT(op_array->scope == pce);
			ZEND_ASSERT(op_array->prototype == NULL);
			new_op_array = zend_arena_alloc(&CG(arena), sizeof(zend_op_array));
			Z_PTR(p->val) = new_op_array;
			memcpy(new_op_array, op_array, sizeof(zend_op_array));
			new_op_array->fn_flags &= ~ZEND_ACC_IMMUTABLE;
			new_op_array->scope = ce;
			ZEND_MAP_PTR_INIT(new_op_array->run_time_cache, NULL);
			ZEND_MAP_PTR_INIT(new_op_array->static_variables_ptr, NULL);

			zend_update_inherited_handler(constructor);
			zend_update_inherited_handler(destructor);
			zend_update_inherited_handler(clone);
			zend_update_inherited_handler(__get);
			zend_update_inherited_handler(__set);
			zend_update_inherited_handler(__call);
			zend_update_inherited_handler(__isset);
			zend_update_inherited_handler(__unset);
			zend_update_inherited_handler(__tostring);
			zend_update_inherited_handler(__callstatic);
			zend_update_inherited_handler(__debugInfo);
			zend_update_inherited_handler(__serialize);
			zend_update_inherited_handler(__unserialize);
		}
	}

	/* static members */
	if (ce->default_static_members_table) {
		zval *dst = emalloc(sizeof(zval) * ce->default_static_members_count);
		zval *src = ce->default_static_members_table;
		zval *end = src + ce->default_static_members_count;

		ce->default_static_members_table = dst;
		for (; src != end; src++, dst++) {
			ZVAL_COPY_VALUE(dst, src);
		}
	}
	ZEND_MAP_PTR_INIT(ce->static_members_table, NULL);

	/* properties_info */
	if (!(HT_FLAGS(&ce->properties_info) & HASH_FLAG_UNINITIALIZED)) {
		p = emalloc(HT_SIZE(&ce->properties_info));
		memcpy(p, HT_GET_DATA_ADDR(&ce->properties_info), HT_USED_SIZE(&ce->properties_info));
		HT_SET_DATA_ADDR(&ce->properties_info, p);
		p = ce->properties_info.arData;
		end = p + ce->properties_info.nNumUsed;
		for (; p != end; p++) {
			zend_property_info *prop_info, *new_prop_info;

			prop_info = Z_PTR(p->val);
			ZEND_ASSERT(prop_info->ce == pce);
			new_prop_info= zend_arena_alloc(&CG(arena), sizeof(zend_property_info));
			Z_PTR(p->val) = new_prop_info;
			memcpy(new_prop_info, prop_info, sizeof(zend_property_info));
			new_prop_info->ce = ce;
			/* Deep copy the type information */
			zend_type_copy_ctor(&new_prop_info->type, /* use_arena */ true, /* persistent */ false);
		}
	}

	/* constants table */
	if (!(HT_FLAGS(&ce->constants_table) & HASH_FLAG_UNINITIALIZED)) {
		p = emalloc(HT_SIZE(&ce->constants_table));
		memcpy(p, HT_GET_DATA_ADDR(&ce->constants_table), HT_USED_SIZE(&ce->constants_table));
		HT_SET_DATA_ADDR(&ce->constants_table, p);
		p = ce->constants_table.arData;
		end = p + ce->constants_table.nNumUsed;
		for (; p != end; p++) {
			zend_class_constant *c, *new_c;

			c = Z_PTR(p->val);
			ZEND_ASSERT(c->ce == pce);
			new_c = zend_arena_alloc(&CG(arena), sizeof(zend_class_constant));
			Z_PTR(p->val) = new_c;
			memcpy(new_c, c, sizeof(zend_class_constant));
			new_c->ce = ce;
		}
	}

	return ce;
}

#ifndef ZEND_WIN32
# define UPDATE_IS_CACHEABLE(ce) do { \
			if ((ce)->type == ZEND_USER_CLASS) { \
				is_cacheable &= (ce)->ce_flags; \
			} \
		} while (0)
#else
// TODO: ASLR may cause different addresses in different workers ???
# define UPDATE_IS_CACHEABLE(ce) do { \
			is_cacheable &= (ce)->ce_flags; \
		} while (0)
#endif

ZEND_API zend_class_entry *zend_do_link_class(zend_class_entry *ce, zend_string *lc_parent_name, zend_string *key) /* {{{ */
{
	/* Load parent/interface dependencies first, so we can still gracefully abort linking
	 * with an exception and remove the class from the class table. This is only possible
	 * if no variance obligations on the current class have been added during autoloading. */
	zend_class_entry *parent = NULL;
	zend_class_entry **traits_and_interfaces = NULL;
	zend_class_entry *proto = NULL;
	zend_class_entry *orig_linking_class;
	uint32_t is_cacheable = ce->ce_flags & ZEND_ACC_IMMUTABLE;
	uint32_t i, j;
	zval *zv;
	ALLOCA_FLAG(use_heap)

	SET_ALLOCA_FLAG(use_heap);
	ZEND_ASSERT(!(ce->ce_flags & ZEND_ACC_LINKED));

	if (ce->parent_name) {
		parent = zend_fetch_class_by_name(
			ce->parent_name, lc_parent_name,
			ZEND_FETCH_CLASS_ALLOW_NEARLY_LINKED | ZEND_FETCH_CLASS_EXCEPTION);
		if (!parent) {
			check_unrecoverable_load_failure(ce);
			return NULL;
		}
		UPDATE_IS_CACHEABLE(parent);
	}

	if (ce->num_traits || ce->num_interfaces) {
		traits_and_interfaces = do_alloca(sizeof(zend_class_entry*) * (ce->num_traits + ce->num_interfaces), use_heap);

		for (i = 0; i < ce->num_traits; i++) {
			zend_class_entry *trait = zend_fetch_class_by_name(ce->trait_names[i].name,
				ce->trait_names[i].lc_name, ZEND_FETCH_CLASS_TRAIT);
			if (UNEXPECTED(trait == NULL)) {
				free_alloca(traits_and_interfaces, use_heap);
				return NULL;
			}
			if (UNEXPECTED(!(trait->ce_flags & ZEND_ACC_TRAIT))) {
				zend_error_noreturn(E_ERROR, "%s cannot use %s - it is not a trait", ZSTR_VAL(ce->name), ZSTR_VAL(trait->name));
				free_alloca(traits_and_interfaces, use_heap);
				return NULL;
			}
			for (j = 0; j < i; j++) {
				if (traits_and_interfaces[j] == trait) {
					/* skip duplications */
					trait = NULL;
					break;
				}
			}
			traits_and_interfaces[i] = trait;
			if (trait) {
				UPDATE_IS_CACHEABLE(trait);
			}
		}
	}

	if (ce->num_interfaces) {
		for (i = 0; i < ce->num_interfaces; i++) {
			zend_class_entry *iface = zend_fetch_class_by_name(
				ce->interface_names[i].name, ce->interface_names[i].lc_name,
				ZEND_FETCH_CLASS_INTERFACE |
				ZEND_FETCH_CLASS_ALLOW_NEARLY_LINKED | ZEND_FETCH_CLASS_EXCEPTION);
			if (!iface) {
				check_unrecoverable_load_failure(ce);
				free_alloca(traits_and_interfaces, use_heap);
				return NULL;
			}
			traits_and_interfaces[ce->num_traits + i] = iface;
			if (iface) {
				UPDATE_IS_CACHEABLE(iface);
			}
		}
	}

#ifndef ZEND_WIN32
	if (ce->ce_flags & ZEND_ACC_ENUM) {
		/* We will add internal methods. */
		is_cacheable = false;
	}
#endif

	bool orig_record_errors = EG(record_errors);

	if (ce->ce_flags & ZEND_ACC_IMMUTABLE && is_cacheable) {
		if (zend_inheritance_cache_get && zend_inheritance_cache_add) {
			zend_class_entry *ret = zend_inheritance_cache_get(ce, parent, traits_and_interfaces);
			if (ret) {
				if (traits_and_interfaces) {
					free_alloca(traits_and_interfaces, use_heap);
				}
				zv = zend_hash_find_known_hash(CG(class_table), key);
				Z_CE_P(zv) = ret;
				return ret;
			}

			/* Make sure warnings (such as deprecations) thrown during inheritance
			 * will be recorded in the inheritance cache. */
			zend_begin_record_errors();
		} else {
			is_cacheable = 0;
		}
		proto = ce;
	}

	zend_try {
		if (ce->ce_flags & ZEND_ACC_IMMUTABLE) {
			/* Lazy class loading */
			ce = zend_lazy_class_load(ce);
			zv = zend_hash_find_known_hash(CG(class_table), key);
			Z_CE_P(zv) = ce;
		} else if (ce->ce_flags & ZEND_ACC_FILE_CACHED) {
			/* Lazy class loading */
			ce = zend_lazy_class_load(ce);
			ce->ce_flags &= ~ZEND_ACC_FILE_CACHED;
			zv = zend_hash_find_known_hash(CG(class_table), key);
			Z_CE_P(zv) = ce;
		}

		if (CG(unlinked_uses)) {
			zend_hash_index_del(CG(unlinked_uses), (zend_long)(uintptr_t) ce);
		}

		orig_linking_class = CG(current_linking_class);
		CG(current_linking_class) = is_cacheable ? ce : NULL;

		if (ce->ce_flags & ZEND_ACC_ENUM) {
			/* Only register builtin enum methods during inheritance to avoid persisting them in
			 * opcache. */
			zend_enum_register_funcs(ce);
		}

		if (parent) {
			if (!(parent->ce_flags & ZEND_ACC_LINKED)) {
				add_dependency_obligation(ce, parent);
			}
			zend_do_inheritance(ce, parent);
		}
		if (ce->num_traits) {
			zend_do_bind_traits(ce, traits_and_interfaces);
		}
		if (ce->num_interfaces) {
			/* Also copy the parent interfaces here, so we don't need to reallocate later. */
			uint32_t num_parent_interfaces = parent ? parent->num_interfaces : 0;
			zend_class_entry **interfaces = emalloc(
					sizeof(zend_class_entry *) * (ce->num_interfaces + num_parent_interfaces));

			if (num_parent_interfaces) {
				memcpy(interfaces, parent->interfaces,
					   sizeof(zend_class_entry *) * num_parent_interfaces);
			}
			memcpy(interfaces + num_parent_interfaces, traits_and_interfaces + ce->num_traits,
				   sizeof(zend_class_entry *) * ce->num_interfaces);

			zend_do_implement_interfaces(ce, interfaces);
		} else if (parent && parent->num_interfaces) {
			zend_do_inherit_interfaces(ce, parent);
		}
		if (!(ce->ce_flags & (ZEND_ACC_INTERFACE|ZEND_ACC_TRAIT))
			&& (ce->ce_flags & (ZEND_ACC_IMPLICIT_ABSTRACT_CLASS|ZEND_ACC_EXPLICIT_ABSTRACT_CLASS))
				) {
			zend_verify_abstract_class(ce);
		}
		if (ce->ce_flags & ZEND_ACC_ENUM) {
			zend_verify_enum(ce);
		}

		/* Normally Stringable is added during compilation. However, if it is imported from a trait,
		 * we need to explicitly add the interface here. */
		if (ce->__tostring && !(ce->ce_flags & ZEND_ACC_TRAIT)
			&& !zend_class_implements_interface(ce, zend_ce_stringable)) {
			ZEND_ASSERT(ce->__tostring->common.fn_flags & ZEND_ACC_TRAIT_CLONE);
			ce->ce_flags |= ZEND_ACC_RESOLVED_INTERFACES;
			ce->num_interfaces++;
			ce->interfaces = perealloc(ce->interfaces,
									   sizeof(zend_class_entry *) * ce->num_interfaces, ce->type == ZEND_INTERNAL_CLASS);
			ce->interfaces[ce->num_interfaces - 1] = zend_ce_stringable;
			do_interface_implementation(ce, zend_ce_stringable);
		}

		zend_build_properties_info_table(ce);
	} zend_catch {
		/* Do not leak recorded errors to the next linked class. */
		if (!orig_record_errors) {
			EG(record_errors) = false;
			zend_free_recorded_errors();
		}
		zend_bailout();
	} zend_end_try();

	EG(record_errors) = orig_record_errors;

	if (!(ce->ce_flags & ZEND_ACC_UNRESOLVED_VARIANCE)) {
		zend_inheritance_check_override(ce);
		ce->ce_flags |= ZEND_ACC_LINKED;
	} else {
		ce->ce_flags |= ZEND_ACC_NEARLY_LINKED;
		if (CG(current_linking_class)) {
			ce->ce_flags |= ZEND_ACC_CACHEABLE;
		}
		load_delayed_classes(ce);
		if (ce->ce_flags & ZEND_ACC_UNRESOLVED_VARIANCE) {
			resolve_delayed_variance_obligations(ce);
		}
		if (ce->ce_flags & ZEND_ACC_CACHEABLE) {
			ce->ce_flags &= ~ZEND_ACC_CACHEABLE;
		} else {
			CG(current_linking_class) = NULL;
		}
	}

	if (!CG(current_linking_class)) {
		is_cacheable = 0;
	}
	CG(current_linking_class) = orig_linking_class;

	if (is_cacheable) {
		HashTable *ht = (HashTable*)ce->inheritance_cache;
		zend_class_entry *new_ce;

		ce->inheritance_cache = NULL;
		new_ce = zend_inheritance_cache_add(ce, proto, parent, traits_and_interfaces, ht);
		if (new_ce) {
			zv = zend_hash_find_known_hash(CG(class_table), key);
			ce = new_ce;
			Z_CE_P(zv) = ce;
		}
		if (ht) {
			zend_hash_destroy(ht);
			FREE_HASHTABLE(ht);
		}
	}

	if (!orig_record_errors) {
		zend_free_recorded_errors();
	}
	if (traits_and_interfaces) {
		free_alloca(traits_and_interfaces, use_heap);
	}

	if (ZSTR_HAS_CE_CACHE(ce->name)) {
		ZSTR_SET_CE_CACHE(ce->name, ce);
	}

	return ce;
}
/* }}} */

/* Check whether early binding is prevented due to unresolved types in inheritance checks. */
static inheritance_status zend_can_early_bind(zend_class_entry *ce, zend_class_entry *parent_ce) /* {{{ */
{
	zend_string *key;
	zend_function *parent_func;
	zend_property_info *parent_info;
	zend_class_constant *parent_const;
	inheritance_status overall_status = INHERITANCE_SUCCESS;

	ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&parent_ce->function_table, key, parent_func) {
		zval *zv = zend_hash_find_known_hash(&ce->function_table, key);
		if (zv) {
			zend_function *child_func = Z_FUNC_P(zv);
			inheritance_status status =
				do_inheritance_check_on_method_ex(
					child_func, child_func->common.scope,
					parent_func, parent_func->common.scope,
					ce, NULL, /* check_visibility */ 1, 1, 0);
			if (UNEXPECTED(status == INHERITANCE_WARNING)) {
				overall_status = INHERITANCE_WARNING;
			} else if (UNEXPECTED(status != INHERITANCE_SUCCESS)) {
				return status;
			}
		}
	} ZEND_HASH_FOREACH_END();

	ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&parent_ce->properties_info, key, parent_info) {
		zval *zv;
		if ((parent_info->flags & ZEND_ACC_PRIVATE) || !ZEND_TYPE_IS_SET(parent_info->type)) {
			continue;
		}

		zv = zend_hash_find_known_hash(&ce->properties_info, key);
		if (zv) {
			zend_property_info *child_info = Z_PTR_P(zv);
			if (ZEND_TYPE_IS_SET(child_info->type)) {
				inheritance_status status = property_types_compatible(parent_info, child_info);
				ZEND_ASSERT(status != INHERITANCE_WARNING);
				if (UNEXPECTED(status != INHERITANCE_SUCCESS)) {
					return status;
				}
			}
		}
	} ZEND_HASH_FOREACH_END();

	ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&parent_ce->constants_table, key, parent_const) {
		zval *zv;
		if ((ZEND_CLASS_CONST_FLAGS(parent_const) & ZEND_ACC_PRIVATE) || !ZEND_TYPE_IS_SET(parent_const->type)) {
			continue;
		}

		zv = zend_hash_find_known_hash(&ce->constants_table, key);
		if (zv) {
			zend_class_constant *child_const = Z_PTR_P(zv);
			if (ZEND_TYPE_IS_SET(child_const->type)) {
				inheritance_status status = class_constant_types_compatible(parent_const, child_const);
				ZEND_ASSERT(status != INHERITANCE_WARNING);
				if (UNEXPECTED(status != INHERITANCE_SUCCESS)) {
					return status;
				}
			}
		}
	} ZEND_HASH_FOREACH_END();

	return overall_status;
}
/* }}} */

static zend_always_inline bool register_early_bound_ce(zval *delayed_early_binding, zend_string *lcname, zend_class_entry *ce) {
	if (delayed_early_binding) {
		if (EXPECTED(!(ce->ce_flags & ZEND_ACC_PRELOADED))) {
			if (zend_hash_set_bucket_key(EG(class_table), (Bucket *)delayed_early_binding, lcname) != NULL) {
				Z_CE_P(delayed_early_binding) = ce;
				return true;
			}
		} else {
			/* If preloading is used, don't replace the existing bucket, add a new one. */
			if (zend_hash_add_ptr(EG(class_table), lcname, ce) != NULL) {
				return true;
			}
		}
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot declare %s %s, because the name is already in use", zend_get_object_type(ce), ZSTR_VAL(ce->name));
		return false;
	}
	if (zend_hash_add_ptr(CG(class_table), lcname, ce) != NULL) {
		return true;
	}
	return false;
}

ZEND_API zend_class_entry *zend_try_early_bind(zend_class_entry *ce, zend_class_entry *parent_ce, zend_string *lcname, zval *delayed_early_binding) /* {{{ */
{
	inheritance_status status;
	zend_class_entry *proto = NULL;
	zend_class_entry *orig_linking_class;

	if (ce->ce_flags & ZEND_ACC_LINKED) {
		ZEND_ASSERT(ce->parent == NULL);
		if (UNEXPECTED(!register_early_bound_ce(delayed_early_binding, lcname, ce))) {
			return NULL;
		}
		zend_observer_class_linked_notify(ce, lcname);
		return ce;
	}

	uint32_t is_cacheable = ce->ce_flags & ZEND_ACC_IMMUTABLE;
	UPDATE_IS_CACHEABLE(parent_ce);
	if (is_cacheable) {
		if (zend_inheritance_cache_get && zend_inheritance_cache_add) {
			zend_class_entry *ret = zend_inheritance_cache_get(ce, parent_ce, NULL);
			if (ret) {
				if (UNEXPECTED(!register_early_bound_ce(delayed_early_binding, lcname, ret))) {
					return NULL;
				}
				zend_observer_class_linked_notify(ret, lcname);
				return ret;
			}
		} else {
			is_cacheable = 0;
		}
		proto = ce;
	}

	orig_linking_class = CG(current_linking_class);
	CG(current_linking_class) = NULL;
	status = zend_can_early_bind(ce, parent_ce);
	CG(current_linking_class) = orig_linking_class;
	if (EXPECTED(status != INHERITANCE_UNRESOLVED)) {
		if (ce->ce_flags & ZEND_ACC_IMMUTABLE) {
			/* Lazy class loading */
			ce = zend_lazy_class_load(ce);
		} else if (ce->ce_flags & ZEND_ACC_FILE_CACHED) {
			/* Lazy class loading */
			ce = zend_lazy_class_load(ce);
			ce->ce_flags &= ~ZEND_ACC_FILE_CACHED;
		}

		if (UNEXPECTED(!register_early_bound_ce(delayed_early_binding, lcname, ce))) {
			return NULL;
		}

		orig_linking_class = CG(current_linking_class);
		CG(current_linking_class) = is_cacheable ? ce : NULL;

		zend_try{
			if (is_cacheable) {
				zend_begin_record_errors();
			}

			zend_do_inheritance_ex(ce, parent_ce, status == INHERITANCE_SUCCESS);
			if (parent_ce && parent_ce->num_interfaces) {
				zend_do_inherit_interfaces(ce, parent_ce);
			}
			zend_build_properties_info_table(ce);
			if ((ce->ce_flags & (ZEND_ACC_IMPLICIT_ABSTRACT_CLASS|ZEND_ACC_INTERFACE|ZEND_ACC_TRAIT|ZEND_ACC_EXPLICIT_ABSTRACT_CLASS)) == ZEND_ACC_IMPLICIT_ABSTRACT_CLASS) {
				zend_verify_abstract_class(ce);
			}
			zend_inheritance_check_override(ce);
			ZEND_ASSERT(!(ce->ce_flags & ZEND_ACC_UNRESOLVED_VARIANCE));
			ce->ce_flags |= ZEND_ACC_LINKED;

			CG(current_linking_class) = orig_linking_class;
		} zend_catch {
			EG(record_errors) = false;
			zend_free_recorded_errors();
			zend_bailout();
		} zend_end_try();

		EG(record_errors) = false;

		if (is_cacheable) {
			HashTable *ht = (HashTable*)ce->inheritance_cache;
			zend_class_entry *new_ce;

			ce->inheritance_cache = NULL;
			new_ce = zend_inheritance_cache_add(ce, proto, parent_ce, NULL, ht);
			if (new_ce) {
				zval *zv = zend_hash_find_known_hash(CG(class_table), lcname);
				ce = new_ce;
				Z_CE_P(zv) = ce;
			}
			if (ht) {
				zend_hash_destroy(ht);
				FREE_HASHTABLE(ht);
			}
		}

		if (ZSTR_HAS_CE_CACHE(ce->name)) {
			ZSTR_SET_CE_CACHE(ce->name, ce);
		}
		zend_observer_class_linked_notify(ce, lcname);

		return ce;
	}
	return NULL;
}
/* }}} */
