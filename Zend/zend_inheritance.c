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

static void add_dependency_obligation(zend_class_entry *ce, zend_class_entry *dependency_ce);
static void add_compatibility_obligation(
		zend_class_entry *ce, const zend_function *child_fn, zend_class_entry *child_scope,
		const zend_function *parent_fn, zend_class_entry *parent_scope);
static void add_property_compatibility_obligation(
		zend_class_entry *ce, const zend_property_info *child_prop,
		const zend_property_info *parent_prop);

static void zend_type_copy_ctor(zend_type *type, zend_bool persistent) {
	if (ZEND_TYPE_HAS_LIST(*type)) {
		zend_type_list *old_list = ZEND_TYPE_LIST(*type);
		size_t size = ZEND_TYPE_LIST_SIZE(old_list->num_types);
		zend_type_list *new_list = ZEND_TYPE_USES_ARENA(*type)
			? zend_arena_alloc(&CG(arena), size) : pemalloc(size, persistent);
		memcpy(new_list, old_list, ZEND_TYPE_LIST_SIZE(old_list->num_types));
		ZEND_TYPE_SET_PTR(*type, new_list);

		zend_type *list_type;
		ZEND_TYPE_LIST_FOREACH(new_list, list_type) {
			ZEND_ASSERT(ZEND_TYPE_HAS_NAME(*list_type));
			zend_string_addref(ZEND_TYPE_NAME(*list_type));
		} ZEND_TYPE_LIST_FOREACH_END();
	} else if (ZEND_TYPE_HAS_NAME(*type)) {
		zend_string_addref(ZEND_TYPE_NAME(*type));
	}
}

static zend_property_info *zend_duplicate_property_info_internal(zend_property_info *property_info) /* {{{ */
{
	zend_property_info* new_property_info = pemalloc(sizeof(zend_property_info), 1);
	memcpy(new_property_info, property_info, sizeof(zend_property_info));
	zend_string_addref(new_property_info->name);
	zend_type_copy_ctor(&new_property_info->type, /* persistent */ 1);

	return new_property_info;
}
/* }}} */

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

static zend_function *zend_duplicate_user_function(zend_function *func) /* {{{ */
{
	zend_function *new_function;

	new_function = zend_arena_alloc(&CG(arena), sizeof(zend_op_array));
	memcpy(new_function, func, sizeof(zend_op_array));

	if (CG(compiler_options) & ZEND_COMPILE_PRELOAD) {
		ZEND_ASSERT(new_function->op_array.fn_flags & ZEND_ACC_PRELOADED);
		ZEND_MAP_PTR_NEW(new_function->op_array.static_variables_ptr);
	} else {
		ZEND_MAP_PTR_INIT(new_function->op_array.static_variables_ptr, &new_function->op_array.static_variables);
	}

	HashTable *static_properties_ptr = ZEND_MAP_PTR_GET(func->op_array.static_variables_ptr);
	if (static_properties_ptr) {
		/* See: Zend/tests/method_static_var.phpt */
		ZEND_MAP_PTR_SET(new_function->op_array.static_variables_ptr, static_properties_ptr);
		GC_TRY_ADDREF(static_properties_ptr);
	} else {
		GC_TRY_ADDREF(new_function->op_array.static_variables);
	}

	return new_function;
}
/* }}} */

static zend_always_inline zend_function *zend_duplicate_function(zend_function *func, zend_class_entry *ce, zend_bool is_interface) /* {{{ */
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
		if (is_interface
		 || EXPECTED(!func->op_array.static_variables)) {
			/* reuse the same op_array structure */
			return func;
		}
		return zend_duplicate_user_function(func);
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

static zend_bool class_visible(zend_class_entry *ce) {
	if (ce->type == ZEND_INTERNAL_CLASS) {
		return !(CG(compiler_options) & ZEND_COMPILE_IGNORE_INTERNAL_CLASSES);
	} else {
		ZEND_ASSERT(ce->type == ZEND_USER_CLASS);
		return !(CG(compiler_options) & ZEND_COMPILE_IGNORE_OTHER_FILES)
			|| ce->info.user.filename == CG(compiled_filename);
	}
}

static zend_class_entry *lookup_class(
		zend_class_entry *scope, zend_string *name, zend_bool register_unresolved) {
	uint32_t flags = ZEND_FETCH_CLASS_ALLOW_UNLINKED | ZEND_FETCH_CLASS_NO_AUTOLOAD;
	zend_class_entry *ce = zend_lookup_class_ex(name, NULL, flags);
	if (!CG(in_compilation)) {
		if (ce) {
			return ce;
		}

		if (register_unresolved) {
			/* We'll autoload this class and process delayed variance obligations later. */
			if (!CG(delayed_autoloads)) {
				ALLOC_HASHTABLE(CG(delayed_autoloads));
				zend_hash_init(CG(delayed_autoloads), 0, NULL, NULL, 0);
			}
			zend_hash_add_empty_element(CG(delayed_autoloads), name);
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

/* Instanceof that's safe to use on unlinked classes. */
static zend_bool unlinked_instanceof(zend_class_entry *ce1, zend_class_entry *ce2) {
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
				/* Avoid recursing if class implements ifself. */
				if (ce && ce != ce1 && unlinked_instanceof(ce, ce2)) {
					return 1;
				}
			}
		}
	}

	return 0;
}

static zend_bool zend_type_contains_traversable(zend_type type) {
	zend_type *single_type;
	if (ZEND_TYPE_FULL_MASK(type) & MAY_BE_OBJECT) {
		return 1;
	}

	ZEND_TYPE_FOREACH(type, single_type) {
		if (ZEND_TYPE_HAS_NAME(*single_type)
				&& zend_string_equals_literal_ci(ZEND_TYPE_NAME(*single_type), "Traversable")) {
			return 1;
		}
	} ZEND_TYPE_FOREACH_END();
	return 0;
}

static zend_bool zend_type_permits_self(
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
			zend_class_entry *ce = lookup_class(self, name, /* register_unresolved */ 0);
			if (ce && unlinked_instanceof(self, ce)) {
				return 1;
			}
		}
	} ZEND_TYPE_FOREACH_END();
	return 0;
}

/* Unresolved means that class declarations that are currently not available are needed to
 * determine whether the inheritance is valid or not. At runtime UNRESOLVED should be treated
 * as an ERROR. */
typedef enum {
	INHERITANCE_UNRESOLVED = -1,
	INHERITANCE_ERROR = 0,
	INHERITANCE_SUCCESS = 1,
} inheritance_status;

static inheritance_status zend_perform_covariant_class_type_check(
		zend_class_entry *fe_scope, zend_string *fe_class_name, zend_class_entry *fe_ce,
		zend_class_entry *proto_scope, zend_type proto_type,
		zend_bool register_unresolved) {
	zend_bool have_unresolved = 0;
	if (ZEND_TYPE_FULL_MASK(proto_type) & MAY_BE_OBJECT) {
		/* Currently, any class name would be allowed here. We still perform a class lookup
		 * for forward-compatibility reasons, as we may have named types in the future that
		 * are not classes (such as enums or typedefs). */
		if (!fe_ce) fe_ce = lookup_class(fe_scope, fe_class_name, register_unresolved);
		if (!fe_ce) {
			have_unresolved = 1;
		} else {
			return INHERITANCE_SUCCESS;
		}
	}
	if (ZEND_TYPE_FULL_MASK(proto_type) & MAY_BE_ITERABLE) {
		if (!fe_ce) fe_ce = lookup_class(fe_scope, fe_class_name, register_unresolved);
		if (!fe_ce) {
			have_unresolved = 1;
		} else if (unlinked_instanceof(fe_ce, zend_ce_traversable)) {
			return INHERITANCE_SUCCESS;
		}
	}

	zend_type *single_type;
	ZEND_TYPE_FOREACH(proto_type, single_type) {
		zend_class_entry *proto_ce;
		if (ZEND_TYPE_HAS_NAME(*single_type)) {
			zend_string *proto_class_name =
				resolve_class_name(proto_scope, ZEND_TYPE_NAME(*single_type));
			if (zend_string_equals_ci(fe_class_name, proto_class_name)) {
				return INHERITANCE_SUCCESS;
			}

			if (!fe_ce) fe_ce = lookup_class(fe_scope, fe_class_name, register_unresolved);
			proto_ce =
				lookup_class(proto_scope, proto_class_name, register_unresolved);
		} else if (ZEND_TYPE_HAS_CE(*single_type)) {
			if (!fe_ce) fe_ce = lookup_class(fe_scope, fe_class_name, register_unresolved);
			proto_ce = ZEND_TYPE_CE(*single_type);
		} else {
			continue;
		}

		if (!fe_ce || !proto_ce) {
			have_unresolved = 1;
		} else if (unlinked_instanceof(fe_ce, proto_ce)) {
			return INHERITANCE_SUCCESS;
		}
	} ZEND_TYPE_FOREACH_END();

	return have_unresolved ? INHERITANCE_UNRESOLVED : INHERITANCE_ERROR;
}

static inheritance_status zend_perform_covariant_type_check(
		zend_class_entry *fe_scope, zend_type fe_type,
		zend_class_entry *proto_scope, zend_type proto_type) /* {{{ */
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
		// TODO: Make "iterable" an alias of "array|Traversable" instead,
		// so these special cases will be handled automatically.
		if ((added_types & MAY_BE_ITERABLE)
				&& (proto_type_mask & MAY_BE_ARRAY)
				&& zend_type_contains_traversable(proto_type)) {
			/* Replacing array|Traversable with iterable is okay */
			added_types &= ~MAY_BE_ITERABLE;
		}
		if ((added_types & MAY_BE_ARRAY) && (proto_type_mask & MAY_BE_ITERABLE)) {
			/* Replacing iterable with array is okay */
			added_types &= ~MAY_BE_ARRAY;
		}
		if ((added_types & MAY_BE_STATIC)
				&& zend_type_permits_self(proto_type, proto_scope, fe_scope)) {
			/* Replacing type that accepts self with static is okay */
			added_types &= ~MAY_BE_STATIC;
		}
		if (added_types) {
			/* Otherwise adding new types is illegal */
			return INHERITANCE_ERROR;
		}
	}

	zend_type *single_type;
	zend_bool all_success = 1;

	/* First try to check whether we can succeed without resolving anything */
	ZEND_TYPE_FOREACH(fe_type, single_type) {
		inheritance_status status;
		if (ZEND_TYPE_HAS_NAME(*single_type)) {
			zend_string *fe_class_name =
				resolve_class_name(fe_scope, ZEND_TYPE_NAME(*single_type));
			status = zend_perform_covariant_class_type_check(
				fe_scope, fe_class_name, NULL,
				proto_scope, proto_type, /* register_unresolved */ 0);
		} else if (ZEND_TYPE_HAS_CE(*single_type)) {
			zend_class_entry *fe_ce = ZEND_TYPE_CE(*single_type);
			status = zend_perform_covariant_class_type_check(
				fe_scope, fe_ce->name, fe_ce,
				proto_scope, proto_type, /* register_unresolved */ 0);
		} else {
			continue;
		}

		if (status == INHERITANCE_ERROR) {
			return INHERITANCE_ERROR;
		}
		if (status != INHERITANCE_SUCCESS) {
			all_success = 0;
		}
	} ZEND_TYPE_FOREACH_END();

	/* All individual checks succeeded, overall success */
	if (all_success) {
		return INHERITANCE_SUCCESS;
	}

	/* Register all classes that may have to be resolved */
	ZEND_TYPE_FOREACH(fe_type, single_type) {
		if (ZEND_TYPE_HAS_NAME(*single_type)) {
			zend_string *fe_class_name =
				resolve_class_name(fe_scope, ZEND_TYPE_NAME(*single_type));
			zend_perform_covariant_class_type_check(
				fe_scope, fe_class_name, NULL,
				proto_scope, proto_type, /* register_unresolved */ 1);
		} else if (ZEND_TYPE_HAS_CE(*single_type)) {
			zend_class_entry *fe_ce = ZEND_TYPE_CE(*single_type);
			zend_perform_covariant_class_type_check(
				fe_scope, fe_ce->name, fe_ce,
				proto_scope, proto_type, /* register_unresolved */ 1);
		}
	} ZEND_TYPE_FOREACH_END();
	return INHERITANCE_UNRESOLVED;
}
/* }}} */

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
	zend_bool proto_is_variadic, fe_is_variadic;

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
		/* Removing a return type is not valid. */
		if (!(fe->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE)) {
			return INHERITANCE_ERROR;
		}

		local_status = zend_perform_covariant_type_check(
			fe_scope, fe->common.arg_info[-1].type,
			proto_scope, proto->common.arg_info[-1].type);

		if (UNEXPECTED(local_status != INHERITANCE_SUCCESS)) {
			if (UNEXPECTED(local_status == INHERITANCE_ERROR)) {
				return INHERITANCE_ERROR;
			}
			ZEND_ASSERT(local_status == INHERITANCE_UNRESOLVED);
			status = INHERITANCE_UNRESOLVED;
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
		/* cut off on NULL byte ... class@anonymous */
		smart_str_appendl(&str, ZSTR_VAL(fptr->common.scope->name), strlen(ZSTR_VAL(fptr->common.scope->name)));
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

static zend_always_inline uint32_t func_lineno(const zend_function *fn) {
	return fn->common.type == ZEND_USER_FUNCTION ? fn->op_array.line_start : 0;
}

static void ZEND_COLD emit_incompatible_method_error(
		const zend_function *child, zend_class_entry *child_scope,
		const zend_function *parent, zend_class_entry *parent_scope,
		inheritance_status status) {
	zend_string *parent_prototype = zend_get_function_declaration(parent, parent_scope);
	zend_string *child_prototype = zend_get_function_declaration(child, child_scope);
	if (status == INHERITANCE_UNRESOLVED) {
		/* Fetch the first unresolved class from registered autoloads */
		zend_string *unresolved_class = NULL;
		ZEND_HASH_FOREACH_STR_KEY(CG(delayed_autoloads), unresolved_class) {
			break;
		} ZEND_HASH_FOREACH_END();
		ZEND_ASSERT(unresolved_class);

		zend_error_at(E_COMPILE_ERROR, NULL, func_lineno(child),
			"Could not check compatibility between %s and %s, because class %s is not available",
			ZSTR_VAL(child_prototype), ZSTR_VAL(parent_prototype), ZSTR_VAL(unresolved_class));
	} else {
		zend_error_at(E_COMPILE_ERROR, NULL, func_lineno(child),
			"Declaration of %s must be compatible with %s",
			ZSTR_VAL(child_prototype), ZSTR_VAL(parent_prototype));
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
			ZEND_ASSERT(status == INHERITANCE_ERROR);
			emit_incompatible_method_error(fe, fe_scope, proto, proto_scope, status);
		}
	}
}

static zend_always_inline inheritance_status do_inheritance_check_on_method_ex(
		zend_function *child, zend_class_entry *child_scope,
		zend_function *parent, zend_class_entry *parent_scope,
		zend_class_entry *ce, zval *child_zv,
		zend_bool check_visibility, zend_bool check_only, zend_bool checked) /* {{{ */
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
		zend_error_at_noreturn(E_COMPILE_ERROR, NULL, func_lineno(child),
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
			zend_error_at_noreturn(E_COMPILE_ERROR, NULL, func_lineno(child),
				"Cannot make non static method %s::%s() static in class %s",
				ZEND_FN_SCOPE_NAME(parent), ZSTR_VAL(child->common.function_name), ZEND_FN_SCOPE_NAME(child));
		} else {
			zend_error_at_noreturn(E_COMPILE_ERROR, NULL, func_lineno(child),
				"Cannot make static method %s::%s() non static in class %s",
				ZEND_FN_SCOPE_NAME(parent), ZSTR_VAL(child->common.function_name), ZEND_FN_SCOPE_NAME(child));
		}
	}

	/* Disallow making an inherited method abstract. */
	if (!checked && UNEXPECTED((child_flags & ZEND_ACC_ABSTRACT) > (parent_flags & ZEND_ACC_ABSTRACT))) {
		if (check_only) {
			return INHERITANCE_ERROR;
		}
		zend_error_at_noreturn(E_COMPILE_ERROR, NULL, func_lineno(child),
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
			if (child->common.scope != ce
			 && child->type == ZEND_USER_FUNCTION
			 && !child->op_array.static_variables) {
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
		zend_error_at_noreturn(E_COMPILE_ERROR, NULL, func_lineno(child),
			"Access level to %s::%s() must be %s (as in class %s)%s",
			ZEND_FN_SCOPE_NAME(child), ZSTR_VAL(child->common.function_name), zend_visibility_string(parent_flags), ZEND_FN_SCOPE_NAME(parent), (parent_flags&ZEND_ACC_PUBLIC) ? "" : " or weaker");
	}

	if (!checked) {
		if (check_only) {
			return zend_do_perform_implementation_check(child, child_scope, parent, parent_scope);
		}
		perform_delayable_implementation_check(ce, child, child_scope, parent, parent_scope);
	}
	return INHERITANCE_SUCCESS;
}
/* }}} */

static zend_never_inline void do_inheritance_check_on_method(
		zend_function *child, zend_class_entry *child_scope,
		zend_function *parent, zend_class_entry *parent_scope,
		zend_class_entry *ce, zval *child_zv, zend_bool check_visibility)
{
	do_inheritance_check_on_method_ex(child, child_scope, parent, parent_scope, ce, child_zv, check_visibility, 0, 0);
}

static zend_always_inline void do_inherit_method(zend_string *key, zend_function *parent, zend_class_entry *ce, zend_bool is_interface, zend_bool checked) /* {{{ */
{
	zval *child = zend_hash_find_ex(&ce->function_table, key, 1);

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

		parent = zend_duplicate_function(parent, ce, is_interface);

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
	zval *child = zend_hash_find_ex(&ce->properties_info, key, 1);
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
		if (UNEXPECTED(ce->type & ZEND_INTERNAL_CLASS)) {
			child_info = zend_duplicate_property_info_internal(parent_info);
		} else {
			child_info = parent_info;
		}
		_zend_hash_append_ptr(&ce->properties_info, key, child_info);
	}
}
/* }}} */

static inline void do_implement_interface(zend_class_entry *ce, zend_class_entry *iface) /* {{{ */
{
	if (!(ce->ce_flags & ZEND_ACC_INTERFACE) && iface->interface_gets_implemented && iface->interface_gets_implemented(iface, ce) == FAILURE) {
		zend_error_noreturn(E_CORE_ERROR, "Class %s could not implement interface %s", ZSTR_VAL(ce->name), ZSTR_VAL(iface->name));
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

static void do_inherit_class_constant(zend_string *name, zend_class_constant *parent_const, zend_class_entry *ce) /* {{{ */
{
	zval *zv = zend_hash_find_ex(&ce->constants_table, name, 1);
	zend_class_constant *c;

	if (zv != NULL) {
		c = (zend_class_constant*)Z_PTR_P(zv);
		if (UNEXPECTED((Z_ACCESS_FLAGS(c->value) & ZEND_ACC_PPP_MASK) > (Z_ACCESS_FLAGS(parent_const->value) & ZEND_ACC_PPP_MASK))) {
			zend_error_noreturn(E_COMPILE_ERROR, "Access level to %s::%s must be %s (as in class %s)%s",
				ZSTR_VAL(ce->name), ZSTR_VAL(name), zend_visibility_string(Z_ACCESS_FLAGS(parent_const->value)), ZSTR_VAL(parent_const->ce->name), (Z_ACCESS_FLAGS(parent_const->value) & ZEND_ACC_PUBLIC) ? "" : " or weaker");
		}
	} else if (!(Z_ACCESS_FLAGS(parent_const->value) & ZEND_ACC_PRIVATE)) {
		if (Z_TYPE(parent_const->value) == IS_CONSTANT_AST) {
			ce->ce_flags &= ~ZEND_ACC_CONSTANTS_UPDATED;
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

	ZEND_HASH_FOREACH_PTR(&ce->properties_info, prop) {
		if (prop->ce == ce && (prop->flags & ZEND_ACC_STATIC) == 0) {
			table[OBJ_PROP_TO_NUM(prop->offset)] = prop;
		}
	} ZEND_HASH_FOREACH_END();
}

ZEND_API void zend_do_inheritance_ex(zend_class_entry *ce, zend_class_entry *parent_ce, zend_bool checked) /* {{{ */
{
	zend_property_info *property_info;
	zend_function *func;
	zend_string *key;

	if (UNEXPECTED(ce->ce_flags & ZEND_ACC_INTERFACE)) {
		/* Interface can only inherit other interfaces */
		if (UNEXPECTED(!(parent_ce->ce_flags & ZEND_ACC_INTERFACE))) {
			zend_error_noreturn(E_COMPILE_ERROR, "Interface %s may not inherit from class (%s)", ZSTR_VAL(ce->name), ZSTR_VAL(parent_ce->name));
		}
	} else if (UNEXPECTED(parent_ce->ce_flags & (ZEND_ACC_INTERFACE|ZEND_ACC_TRAIT|ZEND_ACC_FINAL))) {
		/* Class declaration must not extend traits or interfaces */
		if (parent_ce->ce_flags & ZEND_ACC_INTERFACE) {
			zend_error_noreturn(E_COMPILE_ERROR, "Class %s cannot extend from interface %s", ZSTR_VAL(ce->name), ZSTR_VAL(parent_ce->name));
		} else if (parent_ce->ce_flags & ZEND_ACC_TRAIT) {
			zend_error_noreturn(E_COMPILE_ERROR, "Class %s cannot extend from trait %s", ZSTR_VAL(ce->name), ZSTR_VAL(parent_ce->name));
		}

		/* Class must not extend a final class */
		if (parent_ce->ce_flags & ZEND_ACC_FINAL) {
			zend_error_noreturn(E_COMPILE_ERROR, "Class %s may not inherit from final class (%s)", ZSTR_VAL(ce->name), ZSTR_VAL(parent_ce->name));
		}
	}

	if (ce->parent_name) {
		zend_string_release_ex(ce->parent_name, 0);
	}
	ce->parent = parent_ce;
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
		if (UNEXPECTED(parent_ce->type != ce->type)) {
			/* User class extends internal */
			if (CE_STATIC_MEMBERS(parent_ce) == NULL) {
				zend_class_init_statics(parent_ce);
			}
			if (UNEXPECTED(zend_update_class_constants(parent_ce) != SUCCESS)) {
				ZEND_UNREACHABLE();
			}
			src = CE_STATIC_MEMBERS(parent_ce) + parent_ce->default_static_members_count;
			do {
				dst--;
				src--;
				if (Z_TYPE_P(src) == IS_INDIRECT) {
					ZVAL_INDIRECT(dst, Z_INDIRECT_P(src));
				} else {
					ZVAL_INDIRECT(dst, src);
				}
			} while (dst != end);
		} else if (ce->type == ZEND_USER_CLASS) {
			if (CE_STATIC_MEMBERS(parent_ce) == NULL) {
				ZEND_ASSERT(parent_ce->ce_flags & (ZEND_ACC_IMMUTABLE|ZEND_ACC_PRELOADED));
				zend_class_init_statics(parent_ce);
			}
			src = CE_STATIC_MEMBERS(parent_ce) + parent_ce->default_static_members_count;
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
				}
			} while (dst != end);
		} else {
			src = parent_ce->default_static_members_table + parent_ce->default_static_members_count;
			do {
				dst--;
				src--;
				if (Z_TYPE_P(src) == IS_INDIRECT) {
					ZVAL_INDIRECT(dst, Z_INDIRECT_P(src));
				} else {
					ZVAL_INDIRECT(dst, src);
				}
			} while (dst != end);
		}
		ce->default_static_members_count += parent_ce->default_static_members_count;
		if (!ZEND_MAP_PTR(ce->static_members_table)) {
			ZEND_ASSERT(ce->type == ZEND_INTERNAL_CLASS);
			if (!EG(current_execute_data)) {
				ZEND_MAP_PTR_NEW(ce->static_members_table);
			} else {
				/* internal class loaded by dl() */
				ZEND_MAP_PTR_INIT(ce->static_members_table, &ce->default_static_members_table);
			}
		}
	}

	ZEND_HASH_FOREACH_PTR(&ce->properties_info, property_info) {
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

		ZEND_HASH_FOREACH_STR_KEY_PTR(&parent_ce->properties_info, key, property_info) {
			do_inherit_property(property_info, key, ce);
		} ZEND_HASH_FOREACH_END();
	}

	if (zend_hash_num_elements(&parent_ce->constants_table)) {
		zend_class_constant *c;

		zend_hash_extend(&ce->constants_table,
			zend_hash_num_elements(&ce->constants_table) +
			zend_hash_num_elements(&parent_ce->constants_table), 0);

		ZEND_HASH_FOREACH_STR_KEY_PTR(&parent_ce->constants_table, key, c) {
			do_inherit_class_constant(key, c, ce);
		} ZEND_HASH_FOREACH_END();
	}

	if (zend_hash_num_elements(&parent_ce->function_table)) {
		zend_hash_extend(&ce->function_table,
			zend_hash_num_elements(&ce->function_table) +
			zend_hash_num_elements(&parent_ce->function_table), 0);

		if (checked) {
			ZEND_HASH_FOREACH_STR_KEY_PTR(&parent_ce->function_table, key, func) {
				do_inherit_method(key, func, ce, 0, 1);
			} ZEND_HASH_FOREACH_END();
		} else {
			ZEND_HASH_FOREACH_STR_KEY_PTR(&parent_ce->function_table, key, func) {
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
	ce->ce_flags |= parent_ce->ce_flags & (ZEND_HAS_STATIC_IN_METHODS | ZEND_ACC_HAS_TYPE_HINTS | ZEND_ACC_USE_GUARDS);
}
/* }}} */

static zend_bool do_inherit_constant_check(HashTable *child_constants_table, zend_class_constant *parent_constant, zend_string *name, const zend_class_entry *iface) /* {{{ */
{
	zval *zv = zend_hash_find_ex(child_constants_table, name, 1);
	zend_class_constant *old_constant;

	if (zv != NULL) {
		old_constant = (zend_class_constant*)Z_PTR_P(zv);
		if (old_constant->ce != parent_constant->ce) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot inherit previously-inherited or override constant %s from interface %s", ZSTR_VAL(name), ZSTR_VAL(iface->name));
		}
		return 0;
	}
	return 1;
}
/* }}} */

static void do_inherit_iface_constant(zend_string *name, zend_class_constant *c, zend_class_entry *ce, zend_class_entry *iface) /* {{{ */
{
	if (do_inherit_constant_check(&ce->constants_table, c, name, iface)) {
		zend_class_constant *ct;
		if (Z_TYPE(c->value) == IS_CONSTANT_AST) {
			ce->ce_flags &= ~ZEND_ACC_CONSTANTS_UPDATED;
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

	ZEND_HASH_FOREACH_STR_KEY_PTR(&iface->constants_table, key, c) {
		do_inherit_iface_constant(key, c, ce, iface);
	} ZEND_HASH_FOREACH_END();

	ZEND_HASH_FOREACH_STR_KEY_PTR(&iface->function_table, key, func) {
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
		ZEND_HASH_FOREACH_STR_KEY_PTR(&ce->constants_table, key, c) {
			do_inherit_constant_check(&iface->constants_table, c, key, iface);
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
					zend_error_noreturn(E_COMPILE_ERROR, "Class %s cannot implement previously implemented interface %s", ZSTR_VAL(ce->name), ZSTR_VAL(iface->name));
					return;
				}
				/* skip duplications */
				ZEND_HASH_FOREACH_STR_KEY_PTR(&ce->constants_table, key, c) {
					do_inherit_constant_check(&iface->constants_table, c, key, iface);
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

	for (i = 0; i < ce->num_interfaces; i++) {
		zend_string_release_ex(ce->interface_names[i].name, 0);
		zend_string_release_ex(ce->interface_names[i].lc_name, 0);
	}
	efree(ce->interface_names);

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

				/* if it is 0, no modifieres has been changed */
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
			trait = zend_fetch_class(cur_method_ref->class_name,
							ZEND_FETCH_CLASS_TRAIT|ZEND_FETCH_CLASS_NO_AUTOLOAD);
			if (!trait) {
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
				zend_class_entry *exclude_ce = zend_fetch_class(class_name, ZEND_FETCH_CLASS_TRAIT |ZEND_FETCH_CLASS_NO_AUTOLOAD);
				uint32_t trait_num;

				if (!exclude_ce) {
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
				trait = zend_fetch_class(cur_method_ref->class_name, ZEND_FETCH_CLASS_TRAIT|ZEND_FETCH_CLASS_NO_AUTOLOAD);
				if (!trait) {
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

				/* TODO: try to avoid this assignment (it's necessary only for reflection) */
				cur_method_ref->class_name = zend_string_copy(trait->name);
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
				ZEND_HASH_FOREACH_STR_KEY_PTR(&traits[i]->function_table, key, fn) {
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
				ZEND_HASH_FOREACH_STR_KEY_PTR(&traits[i]->function_table, key, fn) {
					zend_traits_copy_functions(key, fn, ce, NULL, aliases);
				} ZEND_HASH_FOREACH_END();
			}
		}
	}

	ZEND_HASH_FOREACH_PTR(&ce->function_table, fn) {
		zend_fixup_trait_method(fn, ce);
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

static zend_class_entry* find_first_definition(zend_class_entry *ce, zend_class_entry **traits, size_t current_trait, zend_string *prop_name, zend_class_entry *coliding_ce) /* {{{ */
{
	size_t i;

	if (coliding_ce == ce) {
		for (i = 0; i < current_trait; i++) {
			if (traits[i]
			 && zend_hash_exists(&traits[i]->properties_info, prop_name)) {
				return traits[i];
			}
		}
	}

	return coliding_ce;
}
/* }}} */

static void zend_do_traits_property_binding(zend_class_entry *ce, zend_class_entry **traits) /* {{{ */
{
	size_t i;
	zend_property_info *property_info;
	zend_property_info *coliding_prop;
	zend_property_info *new_prop;
	zend_string* prop_name;
	const char* class_name_unused;
	zend_bool not_compatible;
	zval* prop_value;
	uint32_t flags;
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
		ZEND_HASH_FOREACH_PTR(&traits[i]->properties_info, property_info) {
			/* first get the unmangeld name if necessary,
			 * then check whether the property is already there
			 */
			flags = property_info->flags;
			if (flags & ZEND_ACC_PUBLIC) {
				prop_name = zend_string_copy(property_info->name);
			} else {
				const char *pname;
				size_t pname_len;

				/* for private and protected we need to unmangle the names */
				zend_unmangle_property_name_ex(property_info->name,
											&class_name_unused, &pname, &pname_len);
				prop_name = zend_string_init(pname, pname_len, 0);
			}

			/* next: check for conflicts with current class */
			if ((coliding_prop = zend_hash_find_ptr(&ce->properties_info, prop_name)) != NULL) {
				if ((coliding_prop->flags & ZEND_ACC_PRIVATE) && coliding_prop->ce != ce) {
					zend_hash_del(&ce->properties_info, prop_name);
					flags |= ZEND_ACC_CHANGED;
				} else {
					not_compatible = 1;

					if ((coliding_prop->flags & (ZEND_ACC_PPP_MASK | ZEND_ACC_STATIC))
						== (flags & (ZEND_ACC_PPP_MASK | ZEND_ACC_STATIC)) &&
						property_types_compatible(property_info, coliding_prop) == INHERITANCE_SUCCESS
					) {
						/* the flags are identical, thus, the properties may be compatible */
						zval *op1, *op2;
						zval op1_tmp, op2_tmp;

						if (flags & ZEND_ACC_STATIC) {
							op1 = &ce->default_static_members_table[coliding_prop->offset];
							op2 = &traits[i]->default_static_members_table[property_info->offset];
							ZVAL_DEINDIRECT(op1);
							ZVAL_DEINDIRECT(op2);
						} else {
							op1 = &ce->default_properties_table[OBJ_PROP_TO_NUM(coliding_prop->offset)];
							op2 = &traits[i]->default_properties_table[OBJ_PROP_TO_NUM(property_info->offset)];
						}

						/* if any of the values is a constant, we try to resolve it */
						if (UNEXPECTED(Z_TYPE_P(op1) == IS_CONSTANT_AST)) {
							ZVAL_COPY_OR_DUP(&op1_tmp, op1);
							zval_update_constant_ex(&op1_tmp, ce);
							op1 = &op1_tmp;
						}
						if (UNEXPECTED(Z_TYPE_P(op2) == IS_CONSTANT_AST)) {
							ZVAL_COPY_OR_DUP(&op2_tmp, op2);
							zval_update_constant_ex(&op2_tmp, ce);
							op2 = &op2_tmp;
						}

						not_compatible = fast_is_not_identical_function(op1, op2);

						if (op1 == &op1_tmp) {
							zval_ptr_dtor_nogc(&op1_tmp);
						}
						if (op2 == &op2_tmp) {
							zval_ptr_dtor_nogc(&op2_tmp);
						}
					}

					if (not_compatible) {
						zend_error_noreturn(E_COMPILE_ERROR,
							   "%s and %s define the same property ($%s) in the composition of %s. However, the definition differs and is considered incompatible. Class was composed",
								ZSTR_VAL(find_first_definition(ce, traits, i, prop_name, coliding_prop->ce)->name),
								ZSTR_VAL(property_info->ce->name),
								ZSTR_VAL(prop_name),
								ZSTR_VAL(ce->name));
					}

					zend_string_release_ex(prop_name, 0);
					continue;
				}
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
			zend_type_copy_ctor(&type, /* persistent */ 0);
			new_prop = zend_declare_typed_property(ce, prop_name, prop_value, flags, doc_comment, type);

			if (property_info->attributes) {
				new_prop->attributes = property_info->attributes;

				if (!(GC_FLAGS(new_prop->attributes) & IS_ARRAY_IMMUTABLE)) {
					GC_ADDREF(new_prop->attributes);
				}
			}

			zend_string_release_ex(prop_name, 0);
		} ZEND_HASH_FOREACH_END();
	}
}
/* }}} */

static void zend_do_bind_traits(zend_class_entry *ce) /* {{{ */
{
	HashTable **exclude_tables;
	zend_class_entry **aliases;
	zend_class_entry **traits, *trait;
	uint32_t i, j;

	ZEND_ASSERT(ce->num_traits > 0);

	traits = emalloc(sizeof(zend_class_entry*) * ce->num_traits);

	for (i = 0; i < ce->num_traits; i++) {
		trait = zend_fetch_class_by_name(ce->trait_names[i].name,
			ce->trait_names[i].lc_name, ZEND_FETCH_CLASS_TRAIT);
		if (UNEXPECTED(trait == NULL)) {
			return;
		}
		if (UNEXPECTED(!(trait->ce_flags & ZEND_ACC_TRAIT))) {
			zend_error_noreturn(E_ERROR, "%s cannot use %s - it is not a trait", ZSTR_VAL(ce->name), ZSTR_VAL(trait->name));
			return;
		}
		for (j = 0; j < i; j++) {
			if (traits[j] == trait) {
				/* skip duplications */
				trait = NULL;
				break;
			}
		}
		traits[i] = trait;
	}

	/* complete initialization of trait strutures in ce */
	zend_traits_init_trait_structures(ce, traits, &exclude_tables, &aliases);

	/* first care about all methods to be flattened into the class */
	zend_do_traits_method_binding(ce, traits, exclude_tables, aliases);

	if (aliases) {
		efree(aliases);
	}

	if (exclude_tables) {
		efree(exclude_tables);
	}

	/* then flatten the properties into it, to, mostly to notfiy developer about problems */
	zend_do_traits_property_binding(ce, traits);

	efree(traits);
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
	int ctor;
} zend_abstract_info;

static void zend_verify_abstract_class_function(zend_function *fn, zend_abstract_info *ai) /* {{{ */
{
	if (ai->cnt < MAX_ABSTRACT_INFO_CNT) {
		ai->afn[ai->cnt] = fn;
	}
	if (fn->common.fn_flags & ZEND_ACC_CTOR) {
		if (!ai->ctor) {
			ai->cnt++;
			ai->ctor = 1;
		} else {
			ai->afn[ai->cnt] = NULL;
		}
	} else {
		ai->cnt++;
	}
}
/* }}} */

void zend_verify_abstract_class(zend_class_entry *ce) /* {{{ */
{
	zend_function *func;
	zend_abstract_info ai;
	zend_bool is_explicit_abstract = (ce->ce_flags & ZEND_ACC_EXPLICIT_ABSTRACT_CLASS) != 0;
	memset(&ai, 0, sizeof(ai));

	ZEND_HASH_FOREACH_PTR(&ce->function_table, func) {
		if (func->common.fn_flags & ZEND_ACC_ABSTRACT) {
			/* If the class is explicitly abstract, we only check private abstract methods,
			 * because only they must be declared in the same class. */
			if (!is_explicit_abstract || (func->common.fn_flags & ZEND_ACC_PRIVATE)) {
				zend_verify_abstract_class_function(func, &ai);
			}
		}
	} ZEND_HASH_FOREACH_END();

	if (ai.cnt) {
		zend_error_noreturn(E_ERROR, !is_explicit_abstract
			? "Class %s contains %d abstract method%s and must therefore be declared abstract or implement the remaining methods (" MAX_ABSTRACT_INFO_FMT MAX_ABSTRACT_INFO_FMT MAX_ABSTRACT_INFO_FMT ")"
			: "Class %s must implement %d abstract private method%s (" MAX_ABSTRACT_INFO_FMT MAX_ABSTRACT_INFO_FMT MAX_ABSTRACT_INFO_FMT ")",
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
		OBLIGATION_PROPERTY_COMPATIBILITY
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

static void resolve_delayed_variance_obligations(zend_class_entry *ce);

static int check_variance_obligation(zval *zv) {
	variance_obligation *obligation = Z_PTR_P(zv);
	if (obligation->type == OBLIGATION_DEPENDENCY) {
		zend_class_entry *dependency_ce = obligation->dependency_ce;
		if (dependency_ce->ce_flags & ZEND_ACC_UNRESOLVED_VARIANCE) {
			resolve_delayed_variance_obligations(dependency_ce);
		}
		if (!(dependency_ce->ce_flags & ZEND_ACC_LINKED)) {
			return ZEND_HASH_APPLY_KEEP;
		}
	} else if (obligation->type == OBLIGATION_COMPATIBILITY) {
		inheritance_status status = zend_do_perform_implementation_check(
			&obligation->child_fn, obligation->child_scope,
			&obligation->parent_fn, obligation->parent_scope);
		if (UNEXPECTED(status != INHERITANCE_SUCCESS)) {
			if (EXPECTED(status == INHERITANCE_UNRESOLVED)) {
				return ZEND_HASH_APPLY_KEEP;
			}
			ZEND_ASSERT(status == INHERITANCE_ERROR);
			emit_incompatible_method_error(
				&obligation->child_fn, obligation->child_scope,
				&obligation->parent_fn, obligation->parent_scope, status);
		}
		/* Either the compatibility check was successful or only threw a warning. */
	} else {
		ZEND_ASSERT(obligation->type == OBLIGATION_PROPERTY_COMPATIBILITY);
		inheritance_status status =
			property_types_compatible(obligation->parent_prop, obligation->child_prop);
		if (status != INHERITANCE_SUCCESS) {
			if (status == INHERITANCE_UNRESOLVED) {
				return ZEND_HASH_APPLY_KEEP;
			}
			ZEND_ASSERT(status == INHERITANCE_ERROR);
			emit_incompatible_property_error(obligation->child_prop, obligation->parent_prop);
		}
	}
	return ZEND_HASH_APPLY_REMOVE;
}

static void load_delayed_classes() {
	HashTable *delayed_autoloads = CG(delayed_autoloads);
	zend_string *name;

	if (!delayed_autoloads) {
		return;
	}

	/* Take ownership of this HT, to avoid concurrent modification during autoloading. */
	CG(delayed_autoloads) = NULL;

	ZEND_HASH_FOREACH_STR_KEY(delayed_autoloads, name) {
		zend_lookup_class(name);
		if (EG(exception)) {
			break;
		}
	} ZEND_HASH_FOREACH_END();

	zend_hash_destroy(delayed_autoloads);
	FREE_HASHTABLE(delayed_autoloads);
}

static void resolve_delayed_variance_obligations(zend_class_entry *ce) {
	HashTable *all_obligations = CG(delayed_variance_obligations), *obligations;
	zend_ulong num_key = (zend_ulong) (uintptr_t) ce;

	ZEND_ASSERT(all_obligations != NULL);
	obligations = zend_hash_index_find_ptr(all_obligations, num_key);
	ZEND_ASSERT(obligations != NULL);

	zend_hash_apply(obligations, check_variance_obligation);
	if (zend_hash_num_elements(obligations) == 0) {
		ce->ce_flags &= ~ZEND_ACC_UNRESOLVED_VARIANCE;
		ce->ce_flags |= ZEND_ACC_LINKED;
		zend_hash_index_del(all_obligations, num_key);
	}
}

static void report_variance_errors(zend_class_entry *ce) {
	HashTable *all_obligations = CG(delayed_variance_obligations), *obligations;
	variance_obligation *obligation;
	zend_ulong num_key = (zend_ulong) (uintptr_t) ce;

	ZEND_ASSERT(all_obligations != NULL);
	obligations = zend_hash_index_find_ptr(all_obligations, num_key);
	ZEND_ASSERT(obligations != NULL);

	ZEND_HASH_FOREACH_PTR(obligations, obligation) {
		if (obligation->type == OBLIGATION_COMPATIBILITY) {
			/* Just used to populate the delayed_autoloads table,
			 * which will be used when printing the "unresolved" error. */
			inheritance_status status = zend_do_perform_implementation_check(
				&obligation->child_fn, obligation->child_scope,
				&obligation->parent_fn, obligation->parent_scope);
			ZEND_ASSERT(status == INHERITANCE_UNRESOLVED);
			emit_incompatible_method_error(
				&obligation->child_fn, obligation->child_scope,
				&obligation->parent_fn, obligation->parent_scope, status);
		} else if (obligation->type == OBLIGATION_PROPERTY_COMPATIBILITY) {
			emit_incompatible_property_error(obligation->child_prop, obligation->parent_prop);
		} else {
			zend_error_noreturn(E_CORE_ERROR, "Bug #78647");
		}
	} ZEND_HASH_FOREACH_END();

	/* Only warnings were thrown above -- that means that there are incompatibilities, but only
	 * ones that we permit. Mark all classes with open obligations as fully linked. */
	ce->ce_flags &= ~ZEND_ACC_UNRESOLVED_VARIANCE;
	ce->ce_flags |= ZEND_ACC_LINKED;
	zend_hash_index_del(all_obligations, num_key);
}

static void check_unrecoverable_load_failure(zend_class_entry *ce) {
	/* If this class has been used while unlinked through a variance obligation, it is not legal
	 * to remove the class from the class table and throw an exception, because there is already
	 * a dependence on the inheritance hierarchy of this specific class. Instead we fall back to
	 * a fatal error, as would happen if we did not allow exceptions in the first place. */
	if (ce->ce_flags & ZEND_ACC_HAS_UNLINKED_USES) {
		zend_string *exception_str;
		zval exception_zv;
		ZEND_ASSERT(EG(exception) && "Exception must have been thrown");
		ZVAL_OBJ_COPY(&exception_zv, EG(exception));
		zend_clear_exception();
		exception_str = zval_get_string(&exception_zv);
		zend_error_noreturn(E_ERROR,
			"During inheritance of %s with variance dependencies: Uncaught %s", ZSTR_VAL(ce->name), ZSTR_VAL(exception_str));
	}
}

ZEND_API zend_result zend_do_link_class(zend_class_entry *ce, zend_string *lc_parent_name) /* {{{ */
{
	/* Load parent/interface dependencies first, so we can still gracefully abort linking
	 * with an exception and remove the class from the class table. This is only possible
	 * if no variance obligations on the current class have been added during autoloading. */
	zend_class_entry *parent = NULL;
	zend_class_entry **interfaces = NULL;

	if (ce->parent_name) {
		parent = zend_fetch_class_by_name(
			ce->parent_name, lc_parent_name,
			ZEND_FETCH_CLASS_ALLOW_NEARLY_LINKED | ZEND_FETCH_CLASS_EXCEPTION);
		if (!parent) {
			check_unrecoverable_load_failure(ce);
			return FAILURE;
		}
	}

	if (ce->num_interfaces) {
		/* Also copy the parent interfaces here, so we don't need to reallocate later. */
		uint32_t i, num_parent_interfaces = parent ? parent->num_interfaces : 0;
		interfaces = emalloc(
			sizeof(zend_class_entry *) * (ce->num_interfaces + num_parent_interfaces));
		if (num_parent_interfaces) {
			memcpy(interfaces, parent->interfaces,
				sizeof(zend_class_entry *) * num_parent_interfaces);
		}
		for (i = 0; i < ce->num_interfaces; i++) {
			zend_class_entry *iface = zend_fetch_class_by_name(
				ce->interface_names[i].name, ce->interface_names[i].lc_name,
				ZEND_FETCH_CLASS_INTERFACE |
				ZEND_FETCH_CLASS_ALLOW_NEARLY_LINKED | ZEND_FETCH_CLASS_EXCEPTION);
			if (!iface) {
				check_unrecoverable_load_failure(ce);
				efree(interfaces);
				return FAILURE;
			}
			interfaces[num_parent_interfaces + i] = iface;
		}
	}

	if (parent) {
		if (!(parent->ce_flags & ZEND_ACC_LINKED)) {
			add_dependency_obligation(ce, parent);
		}
		zend_do_inheritance(ce, parent);
	}
	if (ce->num_traits) {
		zend_do_bind_traits(ce);
	}
	if (interfaces) {
		zend_do_implement_interfaces(ce, interfaces);
	} else if (parent && parent->num_interfaces) {
		zend_do_inherit_interfaces(ce, parent);
	}
	if (!(ce->ce_flags & (ZEND_ACC_INTERFACE|ZEND_ACC_TRAIT))
		&& (ce->ce_flags & (ZEND_ACC_IMPLICIT_ABSTRACT_CLASS|ZEND_ACC_EXPLICIT_ABSTRACT_CLASS))
	) {
		zend_verify_abstract_class(ce);
	}

	/* Normally Stringable is added during compilation. However, if it is imported from a trait,
	 * we need to explicilty add the interface here. */
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

	if (!(ce->ce_flags & ZEND_ACC_UNRESOLVED_VARIANCE)) {
		ce->ce_flags |= ZEND_ACC_LINKED;
		return SUCCESS;
	}

	ce->ce_flags |= ZEND_ACC_NEARLY_LINKED;
	load_delayed_classes();
	if (ce->ce_flags & ZEND_ACC_UNRESOLVED_VARIANCE) {
		resolve_delayed_variance_obligations(ce);
		if (!(ce->ce_flags & ZEND_ACC_LINKED)) {
			report_variance_errors(ce);
		}
	}

	return SUCCESS;
}
/* }}} */

/* Check whether early binding is prevented due to unresolved types in inheritance checks. */
static inheritance_status zend_can_early_bind(zend_class_entry *ce, zend_class_entry *parent_ce) /* {{{ */
{
	zend_string *key;
	zend_function *parent_func;
	zend_property_info *parent_info;

	ZEND_HASH_FOREACH_STR_KEY_PTR(&parent_ce->function_table, key, parent_func) {
		zval *zv = zend_hash_find_ex(&ce->function_table, key, 1);
		if (zv) {
			zend_function *child_func = Z_FUNC_P(zv);
			inheritance_status status =
				do_inheritance_check_on_method_ex(
					child_func, child_func->common.scope,
					parent_func, parent_func->common.scope,
					ce, NULL, /* check_visibility */ 1, 1, 0);

			if (UNEXPECTED(status != INHERITANCE_SUCCESS)) {
				return status;
			}
		}
	} ZEND_HASH_FOREACH_END();

	ZEND_HASH_FOREACH_STR_KEY_PTR(&parent_ce->properties_info, key, parent_info) {
		zval *zv;
		if ((parent_info->flags & ZEND_ACC_PRIVATE) || !ZEND_TYPE_IS_SET(parent_info->type)) {
			continue;
		}

		zv = zend_hash_find_ex(&ce->properties_info, key, 1);
		if (zv) {
			zend_property_info *child_info = Z_PTR_P(zv);
			if (ZEND_TYPE_IS_SET(child_info->type)) {
				inheritance_status status = property_types_compatible(parent_info, child_info);
				if (UNEXPECTED(status != INHERITANCE_SUCCESS)) {
					return status;
				}
			}
		}
	} ZEND_HASH_FOREACH_END();

	return INHERITANCE_SUCCESS;
}
/* }}} */

zend_bool zend_try_early_bind(zend_class_entry *ce, zend_class_entry *parent_ce, zend_string *lcname, zval *delayed_early_binding) /* {{{ */
{
	inheritance_status status = zend_can_early_bind(ce, parent_ce);

	if (EXPECTED(status != INHERITANCE_UNRESOLVED)) {
		if (delayed_early_binding) {
			if (UNEXPECTED(zend_hash_set_bucket_key(EG(class_table), (Bucket*)delayed_early_binding, lcname) == NULL)) {
				zend_error_noreturn(E_COMPILE_ERROR, "Cannot declare %s %s, because the name is already in use", zend_get_object_type(ce), ZSTR_VAL(ce->name));
				return 0;
			}
		} else {
			if (UNEXPECTED(zend_hash_add_ptr(CG(class_table), lcname, ce) == NULL)) {
				return 0;
			}
		}
		zend_do_inheritance_ex(ce, parent_ce, status == INHERITANCE_SUCCESS);
		if (parent_ce && parent_ce->num_interfaces) {
			zend_do_inherit_interfaces(ce, parent_ce);
		}
		zend_build_properties_info_table(ce);
		if ((ce->ce_flags & (ZEND_ACC_IMPLICIT_ABSTRACT_CLASS|ZEND_ACC_INTERFACE|ZEND_ACC_TRAIT|ZEND_ACC_EXPLICIT_ABSTRACT_CLASS)) == ZEND_ACC_IMPLICIT_ABSTRACT_CLASS) {
			zend_verify_abstract_class(ce);
		}
		ZEND_ASSERT(!(ce->ce_flags & ZEND_ACC_UNRESOLVED_VARIANCE));
		ce->ce_flags |= ZEND_ACC_LINKED;
		return 1;
	}
	return 0;
}
/* }}} */
