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
		zend_class_entry *ce, const zend_function *child_fn, const zend_function *parent_fn,
		zend_bool always_error);
static void add_property_compatibility_obligation(
		zend_class_entry *ce, const zend_property_info *child_prop,
		const zend_property_info *parent_prop);

static void overridden_ptr_dtor(zval *zv) /* {{{ */
{
	efree_size(Z_PTR_P(zv), sizeof(zend_function));
}
/* }}} */

static zend_property_info *zend_duplicate_property_info_internal(zend_property_info *property_info) /* {{{ */
{
	zend_property_info* new_property_info = pemalloc(sizeof(zend_property_info), 1);
	memcpy(new_property_info, property_info, sizeof(zend_property_info));
	zend_string_addref(new_property_info->name);
	if (ZEND_TYPE_IS_NAME(new_property_info->type)) {
		zend_string_addref(ZEND_TYPE_NAME(new_property_info->type));
	}

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
	if (ZEND_MAP_PTR_GET(func->op_array.static_variables_ptr)) {
		/* See: Zend/tests/method_static_var.phpt */
		new_function->op_array.static_variables = ZEND_MAP_PTR_GET(func->op_array.static_variables_ptr);
	}
	if (!(GC_FLAGS(new_function->op_array.static_variables) & IS_ARRAY_IMMUTABLE)) {
		GC_ADDREF(new_function->op_array.static_variables);
	}
	ZEND_MAP_PTR_INIT(new_function->op_array.static_variables_ptr, &new_function->op_array.static_variables);
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
	if (parent->iterator_funcs_ptr) {
		/* Must be initialized through iface->interface_gets_implemented() */
		ZEND_ASSERT(ce->iterator_funcs_ptr);
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
	if (EXPECTED(!ce->serialize_func)) {
		ce->serialize_func = parent->serialize_func;
	}
	if (EXPECTED(!ce->serialize)) {
		ce->serialize = parent->serialize;
	}
	if (EXPECTED(!ce->unserialize_func)) {
		ce->unserialize_func = parent->unserialize_func;
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

static zend_class_entry *lookup_class(zend_class_entry *scope, zend_string *name) {
	zend_class_entry *ce;
	if (!CG(in_compilation)) {
		uint32_t flags = ZEND_FETCH_CLASS_ALLOW_UNLINKED | ZEND_FETCH_CLASS_NO_AUTOLOAD;
		ce = zend_lookup_class_ex(name, NULL, flags);
		if (ce) {
			return ce;
		}

		/* We'll autoload this class and process delayed variance obligations later. */
		if (!CG(delayed_autoloads)) {
			ALLOC_HASHTABLE(CG(delayed_autoloads));
			zend_hash_init(CG(delayed_autoloads), 0, NULL, NULL, 0);
		}
		zend_hash_add_empty_element(CG(delayed_autoloads), name);
	} else {
		ce = zend_lookup_class_ex(name, NULL, ZEND_FETCH_CLASS_NO_AUTOLOAD);
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
	zend_class_entry *ce;

	if (ce1 == ce2) {
		return 1;
	}

	if (ce1->ce_flags & ZEND_ACC_LINKED) {
		return instanceof_function(ce1, ce2);
	}

	ce = ce1;
	while (ce->parent) {
		if (ce->ce_flags & ZEND_ACC_RESOLVED_PARENT) {
			ce = ce->parent;
		} else {
			ce = zend_lookup_class_ex(ce->parent_name, NULL,
				ZEND_FETCH_CLASS_ALLOW_UNLINKED | ZEND_FETCH_CLASS_NO_AUTOLOAD);
			if (!ce) {
				break;
			}
		}
		if (ce == ce2) {
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
				ce = zend_lookup_class_ex(
					ce1->interface_names[i].name, ce1->interface_names[i].lc_name,
					ZEND_FETCH_CLASS_ALLOW_UNLINKED | ZEND_FETCH_CLASS_NO_AUTOLOAD);
				if (ce && unlinked_instanceof(ce, ce2)) {
					return 1;
				}
			}
		}
	}

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

static inheritance_status zend_perform_covariant_type_check(
		zend_string **unresolved_class,
		const zend_function *fe, zend_arg_info *fe_arg_info,
		const zend_function *proto, zend_arg_info *proto_arg_info) /* {{{ */
{
	zend_type fe_type = fe_arg_info->type, proto_type = proto_arg_info->type;
	ZEND_ASSERT(ZEND_TYPE_IS_SET(fe_type) && ZEND_TYPE_IS_SET(proto_type));

	if (ZEND_TYPE_ALLOW_NULL(fe_type) && !ZEND_TYPE_ALLOW_NULL(proto_type)) {
		return INHERITANCE_ERROR;
	}

	if (ZEND_TYPE_IS_CLASS(proto_type)) {
		zend_string *fe_class_name, *proto_class_name;
		zend_class_entry *fe_ce, *proto_ce;
		if (!ZEND_TYPE_IS_CLASS(fe_type)) {
			return INHERITANCE_ERROR;
		}

		fe_class_name = resolve_class_name(fe->common.scope, ZEND_TYPE_NAME(fe_type));
		proto_class_name = resolve_class_name(proto->common.scope, ZEND_TYPE_NAME(proto_type));
		if (zend_string_equals_ci(fe_class_name, proto_class_name)) {
			return INHERITANCE_SUCCESS;
		}

		/* Make sure to always load both classes, to avoid only registering one of them as
		 * a delayed autoload. */
		fe_ce = lookup_class(fe->common.scope, fe_class_name);
		proto_ce = lookup_class(proto->common.scope, proto_class_name);
		if (!fe_ce) {
			*unresolved_class = fe_class_name;
			return INHERITANCE_UNRESOLVED;
		}
		if (!proto_ce) {
			*unresolved_class = proto_class_name;
			return INHERITANCE_UNRESOLVED;
		}

		return unlinked_instanceof(fe_ce, proto_ce) ? INHERITANCE_SUCCESS : INHERITANCE_ERROR;
	} else if (ZEND_TYPE_CODE(proto_type) == IS_ITERABLE) {
		if (ZEND_TYPE_IS_CLASS(fe_type)) {
			zend_string *fe_class_name =
				resolve_class_name(fe->common.scope, ZEND_TYPE_NAME(fe_type));
			zend_class_entry *fe_ce = lookup_class(fe->common.scope, fe_class_name);
			if (!fe_ce) {
				*unresolved_class = fe_class_name;
				return INHERITANCE_UNRESOLVED;
			}
			return unlinked_instanceof(fe_ce, zend_ce_traversable)
				? INHERITANCE_SUCCESS : INHERITANCE_ERROR;
		}

		return ZEND_TYPE_CODE(fe_type) == IS_ITERABLE || ZEND_TYPE_CODE(fe_type) == IS_ARRAY
			? INHERITANCE_SUCCESS : INHERITANCE_ERROR;
	} else if (ZEND_TYPE_CODE(proto_type) == IS_OBJECT) {
		if (ZEND_TYPE_IS_CLASS(fe_type)) {
			/* Currently, any class name would be allowed here. We still perform a class lookup
			 * for forward-compatibility reasons, as we may have named types in the future that
			 * are not classes (such as enums or typedefs). */
			zend_string *fe_class_name =
				resolve_class_name(fe->common.scope, ZEND_TYPE_NAME(fe_type));
			zend_class_entry *fe_ce = lookup_class(fe->common.scope, fe_class_name);
			if (!fe_ce) {
				*unresolved_class = fe_class_name;
				return INHERITANCE_UNRESOLVED;
			}
			return INHERITANCE_SUCCESS;
		}

		return ZEND_TYPE_CODE(fe_type) == IS_OBJECT ? INHERITANCE_SUCCESS : INHERITANCE_ERROR;
	} else {
		return ZEND_TYPE_CODE(fe_type) == ZEND_TYPE_CODE(proto_type)
			? INHERITANCE_SUCCESS : INHERITANCE_ERROR;
	}
}
/* }}} */

static inheritance_status zend_do_perform_arg_type_hint_check(
		zend_string **unresolved_class,
		const zend_function *fe, zend_arg_info *fe_arg_info,
		const zend_function *proto, zend_arg_info *proto_arg_info) /* {{{ */
{
	if (!ZEND_TYPE_IS_SET(fe_arg_info->type)) {
		/* Child with no type is always compatible */
		return INHERITANCE_SUCCESS;
	}

	if (!ZEND_TYPE_IS_SET(proto_arg_info->type)) {
		/* Child defines a type, but parent doesn't, violates LSP */
		return INHERITANCE_ERROR;
	}

	/* Contravariant type check is performed as a covariant type check with swapped
	 * argument order. */
	return zend_perform_covariant_type_check(
		unresolved_class, proto, proto_arg_info, fe, fe_arg_info);
}
/* }}} */

static inheritance_status zend_do_perform_implementation_check(
		zend_string **unresolved_class, const zend_function *fe, const zend_function *proto) /* {{{ */
{
	uint32_t i, num_args;
	inheritance_status status, local_status;

	/* If it's a user function then arg_info == NULL means we don't have any parameters but
	 * we still need to do the arg number checks.  We are only willing to ignore this for internal
	 * functions because extensions don't always define arg_info.
	 */
	if (!proto->common.arg_info && proto->common.type != ZEND_USER_FUNCTION) {
		return INHERITANCE_SUCCESS;
	}

	/* Checks for constructors only if they are declared in an interface,
	 * or explicitly marked as abstract
	 */
	ZEND_ASSERT(!((fe->common.fn_flags & ZEND_ACC_CTOR)
		&& ((proto->common.scope->ce_flags & ZEND_ACC_INTERFACE) == 0
			&& (proto->common.fn_flags & ZEND_ACC_ABSTRACT) == 0)));

	/* If the prototype method is private do not enforce a signature */
	ZEND_ASSERT(!(proto->common.fn_flags & ZEND_ACC_PRIVATE));

	/* check number of arguments */
	if (proto->common.required_num_args < fe->common.required_num_args
		|| proto->common.num_args > fe->common.num_args) {
		return INHERITANCE_ERROR;
	}

	/* by-ref constraints on return values are covariant */
	if ((proto->common.fn_flags & ZEND_ACC_RETURN_REFERENCE)
		&& !(fe->common.fn_flags & ZEND_ACC_RETURN_REFERENCE)) {
		return INHERITANCE_ERROR;
	}

	if ((proto->common.fn_flags & ZEND_ACC_VARIADIC)
		&& !(fe->common.fn_flags & ZEND_ACC_VARIADIC)) {
		return INHERITANCE_ERROR;
	}

	/* For variadic functions any additional (optional) arguments that were added must be
	 * checked against the signature of the variadic argument, so in this case we have to
	 * go through all the parameters of the function and not just those present in the
	 * prototype. */
	num_args = proto->common.num_args;
	if (proto->common.fn_flags & ZEND_ACC_VARIADIC) {
		num_args++;
        if (fe->common.num_args >= proto->common.num_args) {
			num_args = fe->common.num_args;
			if (fe->common.fn_flags & ZEND_ACC_VARIADIC) {
				num_args++;
			}
		}
	}

	status = INHERITANCE_SUCCESS;
	for (i = 0; i < num_args; i++) {
		zend_arg_info *fe_arg_info = &fe->common.arg_info[i];

		zend_arg_info *proto_arg_info;
		if (i < proto->common.num_args) {
			proto_arg_info = &proto->common.arg_info[i];
		} else {
			proto_arg_info = &proto->common.arg_info[proto->common.num_args];
		}

		local_status = zend_do_perform_arg_type_hint_check(
			unresolved_class, fe, fe_arg_info, proto, proto_arg_info);

		if (UNEXPECTED(local_status != INHERITANCE_SUCCESS)) {
			if (UNEXPECTED(local_status == INHERITANCE_ERROR)) {
				return INHERITANCE_ERROR;
			}
			ZEND_ASSERT(local_status == INHERITANCE_UNRESOLVED);
			status = INHERITANCE_UNRESOLVED;
		}

		/* by-ref constraints on arguments are invariant */
		if (fe_arg_info->pass_by_reference != proto_arg_info->pass_by_reference) {
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
			unresolved_class, fe, fe->common.arg_info - 1, proto, proto->common.arg_info - 1);

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

static ZEND_COLD void zend_append_type_hint(smart_str *str, const zend_function *fptr, zend_arg_info *arg_info, int return_hint) /* {{{ */
{

	if (ZEND_TYPE_IS_SET(arg_info->type) && ZEND_TYPE_ALLOW_NULL(arg_info->type)) {
		smart_str_appendc(str, '?');
	}

	if (ZEND_TYPE_IS_CLASS(arg_info->type)) {
		const char *class_name;
		size_t class_name_len;

		class_name = ZSTR_VAL(ZEND_TYPE_NAME(arg_info->type));
		class_name_len = ZSTR_LEN(ZEND_TYPE_NAME(arg_info->type));

		if (!strcasecmp(class_name, "self") && fptr->common.scope) {
			class_name = ZSTR_VAL(fptr->common.scope->name);
			class_name_len = ZSTR_LEN(fptr->common.scope->name);
		} else if (!strcasecmp(class_name, "parent") && fptr->common.scope && fptr->common.scope->parent) {
			class_name = ZSTR_VAL(fptr->common.scope->parent->name);
			class_name_len = ZSTR_LEN(fptr->common.scope->parent->name);
		}

		smart_str_appendl(str, class_name, class_name_len);
		if (!return_hint) {
			smart_str_appendc(str, ' ');
		}
	} else if (ZEND_TYPE_IS_CODE(arg_info->type)) {
		const char *type_name = zend_get_type_by_const(ZEND_TYPE_CODE(arg_info->type));
		smart_str_appends(str, type_name);
		if (!return_hint) {
			smart_str_appendc(str, ' ');
		}
	}
}
/* }}} */

static ZEND_COLD zend_string *zend_get_function_declaration(const zend_function *fptr) /* {{{ */
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
			zend_append_type_hint(&str, fptr, arg_info, 0);

			if (arg_info->pass_by_reference) {
				smart_str_appendc(&str, '&');
			}

			if (arg_info->is_variadic) {
				smart_str_appends(&str, "...");
			}

			smart_str_appendc(&str, '$');

			if (arg_info->name) {
				if (fptr->type == ZEND_INTERNAL_FUNCTION) {
					smart_str_appends(&str, ((zend_internal_arg_info*)arg_info)->name);
				} else {
					smart_str_appendl(&str, ZSTR_VAL(arg_info->name), ZSTR_LEN(arg_info->name));
				}
			} else {
				smart_str_appends(&str, "param");
				smart_str_append_unsigned(&str, i);
			}

			if (i >= required && !arg_info->is_variadic) {
				smart_str_appends(&str, " = ");
				if (fptr->type == ZEND_USER_FUNCTION) {
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
							smart_str_appends(&str, "NULL");
						} else if (Z_TYPE_P(zv) == IS_STRING) {
							smart_str_appendc(&str, '\'');
							smart_str_appendl(&str, Z_STRVAL_P(zv), MIN(Z_STRLEN_P(zv), 10));
							if (Z_STRLEN_P(zv) > 10) {
								smart_str_appends(&str, "...");
							}
							smart_str_appendc(&str, '\'');
						} else if (Z_TYPE_P(zv) == IS_ARRAY) {
							smart_str_appends(&str, "Array");
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
				} else {
					smart_str_appends(&str, "NULL");
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
		zend_append_type_hint(&str, fptr, fptr->common.arg_info - 1, 1);
	}
	smart_str_0(&str);

	return str.s;
}
/* }}} */

static zend_always_inline uint32_t func_lineno(const zend_function *fn) {
	return fn->common.type == ZEND_USER_FUNCTION ? fn->op_array.line_start : 0;
}

static void ZEND_COLD emit_incompatible_method_error(
		int error_level, const char *error_verb,
		const zend_function *child, const zend_function *parent,
		inheritance_status status, zend_string *unresolved_class) {
	zend_string *parent_prototype = zend_get_function_declaration(parent);
	zend_string *child_prototype = zend_get_function_declaration(child);
	if (status == INHERITANCE_UNRESOLVED) {
		zend_error_at(error_level, NULL, func_lineno(child),
			"Could not check compatibility between %s and %s, because class %s is not available",
			ZSTR_VAL(child_prototype), ZSTR_VAL(parent_prototype), ZSTR_VAL(unresolved_class));
	} else {
		zend_error_at(error_level, NULL, func_lineno(child),
			"Declaration of %s %s be compatible with %s",
			ZSTR_VAL(child_prototype), error_verb, ZSTR_VAL(parent_prototype));
	}
	zend_string_efree(child_prototype);
	zend_string_efree(parent_prototype);
}

static void ZEND_COLD emit_incompatible_method_error_or_warning(
		const zend_function *child, const zend_function *parent,
		inheritance_status status, zend_string *unresolved_class, zend_bool always_error) {
	int error_level;
	const char *error_verb;
	if (always_error ||
		(child->common.prototype &&
		 (child->common.prototype->common.fn_flags & ZEND_ACC_ABSTRACT)) ||
		((parent->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE) &&
		 (!(child->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE) ||
		  zend_perform_covariant_type_check(&unresolved_class, child, child->common.arg_info - 1, parent, parent->common.arg_info - 1) != INHERITANCE_SUCCESS))
	) {
		error_level = E_COMPILE_ERROR;
		error_verb = "must";
	} else {
		error_level = E_WARNING;
		error_verb = "should";
	}
	emit_incompatible_method_error(
		error_level, error_verb, child, parent, status, unresolved_class);
}

static void perform_delayable_implementation_check(
		zend_class_entry *ce, const zend_function *fe,
		const zend_function *proto, zend_bool always_error)
{
	zend_string *unresolved_class;
	inheritance_status status = zend_do_perform_implementation_check(
		&unresolved_class, fe, proto);

	if (UNEXPECTED(status != INHERITANCE_SUCCESS)) {
		if (EXPECTED(status == INHERITANCE_UNRESOLVED)) {
			add_compatibility_obligation(ce, fe, proto, always_error);
		} else {
			ZEND_ASSERT(status == INHERITANCE_ERROR);
			if (always_error) {
				emit_incompatible_method_error(
					E_COMPILE_ERROR, "must", fe, proto, status, unresolved_class);
			} else {
				emit_incompatible_method_error_or_warning(
					fe, proto, status, unresolved_class, always_error);
			}
		}
	}
}

static zend_always_inline inheritance_status do_inheritance_check_on_method_ex(zend_function *child, zend_function *parent, zend_class_entry *ce, zval *child_zv, zend_bool check_only, zend_bool checked) /* {{{ */
{
	uint32_t child_flags;
	uint32_t parent_flags = parent->common.fn_flags;
	zend_function *proto;

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

	if (parent_flags & ZEND_ACC_PRIVATE) {
		return INHERITANCE_SUCCESS;
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

	if (!check_only && child->common.prototype != proto) {
		do {
			if (child->common.scope != ce
			 && child->type == ZEND_USER_FUNCTION
			 && !child->op_array.static_variables) {
				if (ce->ce_flags & ZEND_ACC_INTERFACE) {
					/* Few parent interfaces contain the same method */
					break;
				} else if (child_zv) {
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
	if (!checked && (child_flags & ZEND_ACC_PPP_MASK) > (parent_flags & ZEND_ACC_PPP_MASK)) {
		if (check_only) {
			return INHERITANCE_ERROR;
		}
		zend_error_at_noreturn(E_COMPILE_ERROR, NULL, func_lineno(child),
			"Access level to %s::%s() must be %s (as in class %s)%s",
			ZEND_FN_SCOPE_NAME(child), ZSTR_VAL(child->common.function_name), zend_visibility_string(parent_flags), ZEND_FN_SCOPE_NAME(parent), (parent_flags&ZEND_ACC_PUBLIC) ? "" : " or weaker");
	}

	if (!checked) {
		if (check_only) {
			zend_string *unresolved_class;

			return zend_do_perform_implementation_check(
				&unresolved_class, child, parent);
		}
		perform_delayable_implementation_check(
			ce, child, parent, /*always_error*/0);
	}
	return INHERITANCE_SUCCESS;
}
/* }}} */

static zend_never_inline void do_inheritance_check_on_method(zend_function *child, zend_function *parent, zend_class_entry *ce, zval *child_zv) /* {{{ */
{
	do_inheritance_check_on_method_ex(child, parent, ce, child_zv, 0, 0);
}
/* }}} */

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
			do_inheritance_check_on_method_ex(func, parent, ce, child, 0, checked);
		} else {
			do_inheritance_check_on_method(func, parent, ce, child);
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
	zend_string *parent_name, *child_name;
	zend_class_entry *parent_type_ce, *child_type_ce;
	if (parent_info->type == child_info->type) {
		return INHERITANCE_SUCCESS;
	}

	if (!ZEND_TYPE_IS_CLASS(parent_info->type) || !ZEND_TYPE_IS_CLASS(child_info->type) ||
			ZEND_TYPE_ALLOW_NULL(parent_info->type) != ZEND_TYPE_ALLOW_NULL(child_info->type)) {
		return INHERITANCE_ERROR;
	}

	parent_name = ZEND_TYPE_IS_CE(parent_info->type)
		? ZEND_TYPE_CE(parent_info->type)->name
		: resolve_class_name(parent_info->ce, ZEND_TYPE_NAME(parent_info->type));
	child_name = ZEND_TYPE_IS_CE(child_info->type)
		? ZEND_TYPE_CE(child_info->type)->name
		: resolve_class_name(child_info->ce, ZEND_TYPE_NAME(child_info->type));
	if (zend_string_equals_ci(parent_name, child_name)) {
		return INHERITANCE_SUCCESS;
	}

	/* Check for class aliases */
	parent_type_ce = ZEND_TYPE_IS_CE(parent_info->type)
		? ZEND_TYPE_CE(parent_info->type)
		: lookup_class(parent_info->ce, parent_name);
	child_type_ce = ZEND_TYPE_IS_CE(child_info->type)
		? ZEND_TYPE_CE(child_info->type)
		: lookup_class(child_info->ce, child_name);
	if (!parent_type_ce || !child_type_ce) {
		return INHERITANCE_UNRESOLVED;
	}
	return parent_type_ce == child_type_ce ? INHERITANCE_SUCCESS : INHERITANCE_ERROR;
}

static void emit_incompatible_property_error(
		const zend_property_info *child, const zend_property_info *parent) {
	zend_error_noreturn(E_COMPILE_ERROR,
		"Type of %s::$%s must be %s%s (as in class %s)",
		ZSTR_VAL(child->ce->name),
		zend_get_unmangled_property_name(child->name),
		ZEND_TYPE_ALLOW_NULL(parent->type) ? "?" : "",
		ZEND_TYPE_IS_CLASS(parent->type)
			? ZSTR_VAL(ZEND_TYPE_IS_CE(parent->type) ? ZEND_TYPE_CE(parent->type)->name : resolve_class_name(parent->ce, ZEND_TYPE_NAME(parent->type)))
			: zend_get_type_by_const(ZEND_TYPE_CODE(parent->type)),
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
					(parent_info->flags & ZEND_ACC_STATIC) ? "static " : "non static ", ZSTR_VAL(ce->parent->name), ZSTR_VAL(key),
					(child_info->flags & ZEND_ACC_STATIC) ? "static " : "non static ", ZSTR_VAL(ce->name), ZSTR_VAL(key));
			}

			if (UNEXPECTED((child_info->flags & ZEND_ACC_PPP_MASK) > (parent_info->flags & ZEND_ACC_PPP_MASK))) {
				zend_error_noreturn(E_COMPILE_ERROR, "Access level to %s::$%s must be %s (as in class %s)%s", ZSTR_VAL(ce->name), ZSTR_VAL(key), zend_visibility_string(parent_info->flags), ZSTR_VAL(ce->parent->name), (parent_info->flags&ZEND_ACC_PUBLIC) ? "" : " or weaker");
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
						ZSTR_VAL(ce->parent->name));
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
				ZSTR_VAL(ce->name), ZSTR_VAL(name), zend_visibility_string(Z_ACCESS_FLAGS(parent_const->value)), ZSTR_VAL(ce->parent->name), (Z_ACCESS_FLAGS(parent_const->value) & ZEND_ACC_PUBLIC) ? "" : " or weaker");
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

	/* Inherit interfaces */
	if (parent_ce->num_interfaces) {
		if (!(ce->ce_flags & ZEND_ACC_IMPLEMENT_INTERFACES)) {
			zend_do_inherit_interfaces(ce, parent_ce);
		} else {
			uint32_t i;

			for (i = 0; i < parent_ce->num_interfaces; i++) {
				do_implement_interface(ce, parent_ce->interfaces[i]);
			}
		}
	}

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
				ZEND_ASSERT(0);
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

	i = num_parent_interfaces;
	for (; i < ce->num_interfaces; i++) {
		do_interface_implementation(ce, ce->interfaces[i]);
	}
}
/* }}} */

static void zend_add_magic_methods(zend_class_entry* ce, zend_string* mname, zend_function* fe) /* {{{ */
{
	if (zend_string_equals_literal(mname, "serialize")) {
		ce->serialize_func = fe;
	} else if (zend_string_equals_literal(mname, "unserialize")) {
		ce->unserialize_func = fe;
	} else if (ZSTR_LEN(ce->name) != ZSTR_LEN(mname) && (ZSTR_VAL(mname)[0] != '_' || ZSTR_VAL(mname)[1] != '_')) {
		/* pass */
	} else if (zend_string_equals_literal(mname, ZEND_CLONE_FUNC_NAME)) {
		ce->clone = fe;
	} else if (zend_string_equals_literal(mname, ZEND_CONSTRUCTOR_FUNC_NAME)) {
		if (ce->constructor && (!ce->parent || ce->constructor != ce->parent->constructor)) {
			zend_error_noreturn(E_COMPILE_ERROR, "%s has colliding constructor definitions coming from traits", ZSTR_VAL(ce->name));
		}
		ce->constructor = fe;
	} else if (zend_string_equals_literal(mname, ZEND_DESTRUCTOR_FUNC_NAME)) {
		ce->destructor = fe;
	} else if (zend_string_equals_literal(mname, ZEND_GET_FUNC_NAME)) {
		ce->__get = fe;
		ce->ce_flags |= ZEND_ACC_USE_GUARDS;
	} else if (zend_string_equals_literal(mname, ZEND_SET_FUNC_NAME)) {
		ce->__set = fe;
		ce->ce_flags |= ZEND_ACC_USE_GUARDS;
	} else if (zend_string_equals_literal(mname, ZEND_CALL_FUNC_NAME)) {
		ce->__call = fe;
	} else if (zend_string_equals_literal(mname, ZEND_UNSET_FUNC_NAME)) {
		ce->__unset = fe;
		ce->ce_flags |= ZEND_ACC_USE_GUARDS;
	} else if (zend_string_equals_literal(mname, ZEND_ISSET_FUNC_NAME)) {
		ce->__isset = fe;
		ce->ce_flags |= ZEND_ACC_USE_GUARDS;
	} else if (zend_string_equals_literal(mname, ZEND_CALLSTATIC_FUNC_NAME)) {
		ce->__callstatic = fe;
	} else if (zend_string_equals_literal(mname, ZEND_TOSTRING_FUNC_NAME)) {
		ce->__tostring = fe;
	} else if (zend_string_equals_literal(mname, ZEND_DEBUGINFO_FUNC_NAME)) {
		ce->__debugInfo = fe;
	} else if (ZSTR_LEN(ce->name) == ZSTR_LEN(mname)) {
		zend_string *lowercase_name = zend_string_tolower(ce->name);
		lowercase_name = zend_new_interned_string(lowercase_name);
		if (!memcmp(ZSTR_VAL(mname), ZSTR_VAL(lowercase_name), ZSTR_LEN(mname))) {
			if (ce->constructor && (!ce->parent || ce->constructor != ce->parent->constructor)) {
				zend_error_noreturn(E_COMPILE_ERROR, "%s has colliding constructor definitions coming from traits", ZSTR_VAL(ce->name));
			}
			ce->constructor = fe;
			fe->common.fn_flags |= ZEND_ACC_CTOR;
		}
		zend_string_release_ex(lowercase_name, 0);
	}
}
/* }}} */

static void zend_add_trait_method(zend_class_entry *ce, const char *name, zend_string *key, zend_function *fn, HashTable **overridden) /* {{{ */
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

		if (existing_fn->common.scope == ce) {
			/* members from the current class override trait methods */
			/* use temporary *overridden HashTable to detect hidden conflict */
			if (*overridden) {
				if ((existing_fn = zend_hash_find_ptr(*overridden, key)) != NULL) {
					if (existing_fn->common.fn_flags & ZEND_ACC_ABSTRACT) {
						/* Make sure the trait method is compatible with previosly declared abstract method */
						perform_delayable_implementation_check(
							ce, fn, existing_fn, /*always_error*/ 1);
					}
					if (fn->common.fn_flags & ZEND_ACC_ABSTRACT) {
						/* Make sure the abstract declaration is compatible with previous declaration */
						perform_delayable_implementation_check(
							ce, existing_fn, fn, /*always_error*/ 1);
						return;
					}
				}
			} else {
				ALLOC_HASHTABLE(*overridden);
				zend_hash_init_ex(*overridden, 8, NULL, overridden_ptr_dtor, 0, 0);
			}
			zend_hash_update_mem(*overridden, key, fn, sizeof(zend_function));
			return;
		} else if ((fn->common.fn_flags & ZEND_ACC_ABSTRACT)
				&& !(existing_fn->common.fn_flags & ZEND_ACC_ABSTRACT)) {
			/* Make sure the abstract declaration is compatible with previous declaration */
			perform_delayable_implementation_check(
				ce, existing_fn, fn, /*always_error*/ 1);
			return;
		} else if (UNEXPECTED((existing_fn->common.scope->ce_flags & ZEND_ACC_TRAIT)
				&& !(existing_fn->common.fn_flags & ZEND_ACC_ABSTRACT))) {
			/* two traits can't define the same non-abstract method */
#if 1
			zend_error_noreturn(E_COMPILE_ERROR, "Trait method %s has not been applied, because there are collisions with other trait methods on %s",
				name, ZSTR_VAL(ce->name));
#else		/* TODO: better error message */
			zend_error_noreturn(E_COMPILE_ERROR, "Trait method %s::%s has not been applied as %s::%s, because of collision with %s::%s",
				ZSTR_VAL(fn->common.scope->name), ZSTR_VAL(fn->common.function_name),
				ZSTR_VAL(ce->name), name,
				ZSTR_VAL(existing_fn->common.scope->name), ZSTR_VAL(existing_fn->common.function_name));
#endif
		} else {
			/* inherited members are overridden by members inserted by traits */
			/* check whether the trait method fulfills the inheritance requirements */
			do_inheritance_check_on_method(fn, existing_fn, ce, NULL);
			fn->common.prototype = NULL;
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
	function_add_ref(new_fn);
	fn = zend_hash_update_ptr(&ce->function_table, key, new_fn);
	zend_add_magic_methods(ce, key, fn);
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

static void zend_traits_copy_functions(zend_string *fnname, zend_function *fn, zend_class_entry *ce, HashTable **overridden, HashTable *exclude_table, zend_class_entry **aliases) /* {{{ */
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
				&& (!aliases[i] || fn->common.scope == aliases[i])
				&& ZSTR_LEN(alias->trait_method.method_name) == ZSTR_LEN(fnname)
				&& (zend_binary_strcasecmp(ZSTR_VAL(alias->trait_method.method_name), ZSTR_LEN(alias->trait_method.method_name), ZSTR_VAL(fnname), ZSTR_LEN(fnname)) == 0)) {
				fn_copy = *fn;

				/* if it is 0, no modifieres has been changed */
				if (alias->modifiers) {
					fn_copy.common.fn_flags = alias->modifiers | (fn->common.fn_flags ^ (fn->common.fn_flags & ZEND_ACC_PPP_MASK));
				}

				lcname = zend_string_tolower(alias->alias);
				zend_add_trait_method(ce, ZSTR_VAL(alias->alias), lcname, &fn_copy, overridden);
				zend_string_release_ex(lcname, 0);

				/* Record the trait from which this alias was resolved. */
				if (!aliases[i]) {
					aliases[i] = fn->common.scope;
				}
				if (!alias->trait_method.class_name) {
					/* TODO: try to avoid this assignment (it's necessary only for reflection) */
					alias->trait_method.class_name = zend_string_copy(fn->common.scope->name);
				}
			}
			alias_ptr++;
			alias = *alias_ptr;
			i++;
		}
	}

	if (exclude_table == NULL || zend_hash_find(exclude_table, fnname) == NULL) {
		/* is not in hashtable, thus, function is not to be excluded */
		/* And how about ZEND_OVERLOADED_FUNCTION? */
		memcpy(&fn_copy, fn, fn->type == ZEND_USER_FUNCTION? sizeof(zend_op_array) : sizeof(zend_internal_function));

		/* apply aliases which have not alias name, just setting visibility */
		if (ce->trait_aliases) {
			alias_ptr = ce->trait_aliases;
			alias = *alias_ptr;
			i = 0;
			while (alias) {
				/* Scope unset or equal to the function we compare to, and the alias applies to fn */
				if (alias->alias == NULL && alias->modifiers != 0
					&& (!aliases[i] || fn->common.scope == aliases[i])
					&& (ZSTR_LEN(alias->trait_method.method_name) == ZSTR_LEN(fnname))
					&& (zend_binary_strcasecmp(ZSTR_VAL(alias->trait_method.method_name), ZSTR_LEN(alias->trait_method.method_name), ZSTR_VAL(fnname), ZSTR_LEN(fnname)) == 0)) {

					fn_copy.common.fn_flags = alias->modifiers | (fn->common.fn_flags ^ (fn->common.fn_flags & ZEND_ACC_PPP_MASK));

					/** Record the trait from which this alias was resolved. */
					if (!aliases[i]) {
						aliases[i] = fn->common.scope;
					}
					if (!alias->trait_method.class_name) {
						/* TODO: try to avoid this assignment (it's necessary only for reflection) */
						alias->trait_method.class_name = zend_string_copy(fn->common.scope->name);
					}
				}
				alias_ptr++;
				alias = *alias_ptr;
				i++;
			}
		}

		zend_add_trait_method(ce, ZSTR_VAL(fn->common.function_name), fnname, &fn_copy, overridden);
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
			/** For all aliases with an explicit class name, resolve the class now. */
			if (ce->trait_aliases[i]->trait_method.class_name) {
				cur_method_ref = &ce->trait_aliases[i]->trait_method;
				trait = zend_fetch_class(cur_method_ref->class_name, ZEND_FETCH_CLASS_TRAIT|ZEND_FETCH_CLASS_NO_AUTOLOAD);
				if (!trait) {
					zend_error_noreturn(E_COMPILE_ERROR, "Could not find trait %s", ZSTR_VAL(cur_method_ref->class_name));
				}
				zend_check_trait_usage(ce, trait, traits);
				aliases[i] = trait;

				/** And, ensure that the referenced method is resolvable, too. */
				lcname = zend_string_tolower(cur_method_ref->method_name);
				if (!zend_hash_exists(&trait->function_table, lcname)) {
					zend_error_noreturn(E_COMPILE_ERROR, "An alias was defined for %s::%s but this method does not exist", ZSTR_VAL(trait->name), ZSTR_VAL(cur_method_ref->method_name));
				}
				zend_string_release_ex(lcname, 0);
			}
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
	HashTable *overridden = NULL;
	zend_string *key;
	zend_function *fn;

	if (exclude_tables) {
		for (i = 0; i < ce->num_traits; i++) {
			if (traits[i]) {
				/* copies functions, applies defined aliasing, and excludes unused trait methods */
				ZEND_HASH_FOREACH_STR_KEY_PTR(&traits[i]->function_table, key, fn) {
					zend_traits_copy_functions(key, fn, ce, &overridden, exclude_tables[i], aliases);
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
					zend_traits_copy_functions(key, fn, ce, &overridden, NULL, aliases);
				} ZEND_HASH_FOREACH_END();
			}
		}
	}

	ZEND_HASH_FOREACH_PTR(&ce->function_table, fn) {
		zend_fixup_trait_method(fn, ce);
	} ZEND_HASH_FOREACH_END();

	if (overridden) {
		zend_hash_destroy(overridden);
		FREE_HASHTABLE(overridden);
	}
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
			if (ZEND_TYPE_IS_NAME(property_info->type)) {
				zend_string_addref(ZEND_TYPE_NAME(property_info->type));
			}
			zend_declare_typed_property(ce, prop_name, prop_value, flags, doc_comment, property_info->type);
			zend_string_release_ex(prop_name, 0);
		} ZEND_HASH_FOREACH_END();
	}
}
/* }}} */

static void zend_do_check_for_inconsistent_traits_aliasing(zend_class_entry *ce, zend_class_entry **aliases) /* {{{ */
{
	int i = 0;
	zend_trait_alias* cur_alias;
	zend_string* lc_method_name;

	if (ce->trait_aliases) {
		while (ce->trait_aliases[i]) {
			cur_alias = ce->trait_aliases[i];
			/** The trait for this alias has not been resolved, this means, this
				alias was not applied. Abort with an error. */
			if (!aliases[i]) {
				if (cur_alias->alias) {
					/** Plain old inconsistency/typo/bug */
					zend_error_noreturn(E_COMPILE_ERROR,
							   "An alias (%s) was defined for method %s(), but this method does not exist",
							   ZSTR_VAL(cur_alias->alias),
							   ZSTR_VAL(cur_alias->trait_method.method_name));
				} else {
					/** Here are two possible cases:
						1) this is an attempt to modify the visibility
						   of a method introduce as part of another alias.
						   Since that seems to violate the DRY principle,
						   we check against it and abort.
						2) it is just a plain old inconsitency/typo/bug
						   as in the case where alias is set. */

					lc_method_name = zend_string_tolower(
						cur_alias->trait_method.method_name);
					if (zend_hash_exists(&ce->function_table,
										 lc_method_name)) {
						zend_string_release_ex(lc_method_name, 0);
						zend_error_noreturn(E_COMPILE_ERROR,
								   "The modifiers for the trait alias %s() need to be changed in the same statement in which the alias is defined. Error",
								   ZSTR_VAL(cur_alias->trait_method.method_name));
					} else {
						zend_string_release_ex(lc_method_name, 0);
						zend_error_noreturn(E_COMPILE_ERROR,
								   "The modifiers of the trait method %s() are changed, but this method does not exist. Error",
								   ZSTR_VAL(cur_alias->trait_method.method_name));

					}
				}
			}
			i++;
		}
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

	/* Aliases which have not been applied indicate typos/bugs. */
	zend_do_check_for_inconsistent_traits_aliasing(ce, aliases);

	if (aliases) {
		efree(aliases);
	}

	if (exclude_tables) {
		efree(exclude_tables);
	}

	/* then flatten the properties into it, to, mostly to notfiy developer about problems */
	zend_do_traits_property_binding(ce, traits);

	efree(traits);

	/* Emit E_DEPRECATED for PHP 4 constructors */
	zend_check_deprecated_constructor(ce);
}
/* }}} */


static zend_bool zend_has_deprecated_constructor(const zend_class_entry *ce) /* {{{ */
{
	const zend_string *constructor_name;
	if (!ce->constructor) {
		return 0;
	}
	constructor_name = ce->constructor->common.function_name;
	return !zend_binary_strcasecmp(
		ZSTR_VAL(ce->name), ZSTR_LEN(ce->name),
		ZSTR_VAL(constructor_name), ZSTR_LEN(constructor_name)
	);
}
/* }}} */

void zend_check_deprecated_constructor(const zend_class_entry *ce) /* {{{ */
{
	if (zend_has_deprecated_constructor(ce)) {
		zend_error(E_DEPRECATED, "Methods with the same name as their class will not be constructors in a future version of PHP; %s has a deprecated constructor", ZSTR_VAL(ce->name));
	}
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
	if (fn->common.fn_flags & ZEND_ACC_ABSTRACT) {
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
}
/* }}} */

void zend_verify_abstract_class(zend_class_entry *ce) /* {{{ */
{
	zend_function *func;
	zend_abstract_info ai;

	ZEND_ASSERT((ce->ce_flags & (ZEND_ACC_IMPLICIT_ABSTRACT_CLASS|ZEND_ACC_INTERFACE|ZEND_ACC_TRAIT|ZEND_ACC_EXPLICIT_ABSTRACT_CLASS)) == ZEND_ACC_IMPLICIT_ABSTRACT_CLASS);
	memset(&ai, 0, sizeof(ai));

	ZEND_HASH_FOREACH_PTR(&ce->function_table, func) {
		zend_verify_abstract_class_function(func, &ai);
	} ZEND_HASH_FOREACH_END();

	if (ai.cnt) {
		zend_error_noreturn(E_ERROR, "Class %s contains %d abstract method%s and must therefore be declared abstract or implement the remaining methods (" MAX_ABSTRACT_INFO_FMT MAX_ABSTRACT_INFO_FMT MAX_ABSTRACT_INFO_FMT ")",
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
			zend_bool always_error;
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
		zend_class_entry *ce, const zend_function *child_fn, const zend_function *parent_fn,
		zend_bool always_error) {
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
	obligation->always_error = always_error;
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
		zend_string *unresolved_class;
		inheritance_status status = zend_do_perform_implementation_check(
			&unresolved_class, &obligation->child_fn, &obligation->parent_fn);

		if (UNEXPECTED(status != INHERITANCE_SUCCESS)) {
			if (EXPECTED(status == INHERITANCE_UNRESOLVED)) {
				return ZEND_HASH_APPLY_KEEP;
			}
			ZEND_ASSERT(status == INHERITANCE_ERROR);
			emit_incompatible_method_error_or_warning(
				&obligation->child_fn, &obligation->parent_fn, status, unresolved_class,
				obligation->always_error);
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
		inheritance_status status;
		zend_string *unresolved_class;

		if (obligation->type == OBLIGATION_COMPATIBILITY) {
			/* Just used to fetch the unresolved_class in this case. */
			status = zend_do_perform_implementation_check(
				&unresolved_class, &obligation->child_fn, &obligation->parent_fn);
			ZEND_ASSERT(status == INHERITANCE_UNRESOLVED);
			emit_incompatible_method_error_or_warning(
				&obligation->child_fn, &obligation->parent_fn,
				status, unresolved_class, obligation->always_error);
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
		ZVAL_OBJ(&exception_zv, EG(exception));
		Z_ADDREF(exception_zv);
		zend_clear_exception();
		exception_str = zval_get_string(&exception_zv);
		zend_error_noreturn(E_ERROR,
			"During inheritance of %s with variance dependencies: Uncaught %s", ZSTR_VAL(ce->name), ZSTR_VAL(exception_str));
	}
}

ZEND_API int zend_do_link_class(zend_class_entry *ce, zend_string *lc_parent_name) /* {{{ */
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
	if (ce->ce_flags & ZEND_ACC_IMPLEMENT_TRAITS) {
		zend_do_bind_traits(ce);
	}
	if (ce->ce_flags & ZEND_ACC_IMPLEMENT_INTERFACES) {
		zend_do_implement_interfaces(ce, interfaces);
	}
	if ((ce->ce_flags & (ZEND_ACC_IMPLICIT_ABSTRACT_CLASS|ZEND_ACC_INTERFACE|ZEND_ACC_TRAIT|ZEND_ACC_EXPLICIT_ABSTRACT_CLASS)) == ZEND_ACC_IMPLICIT_ABSTRACT_CLASS) {
		zend_verify_abstract_class(ce);
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
	inheritance_status ret = INHERITANCE_SUCCESS;
	zend_string *key;
	zend_function *parent_func;
	zend_property_info *parent_info;

	ZEND_HASH_FOREACH_STR_KEY_PTR(&parent_ce->function_table, key, parent_func) {
		zval *zv = zend_hash_find_ex(&ce->function_table, key, 1);
		if (zv) {
			zend_function *child_func = Z_FUNC_P(zv);
			inheritance_status status =
				do_inheritance_check_on_method_ex(child_func, parent_func, ce, NULL, 1, 0);

			if (UNEXPECTED(status != INHERITANCE_SUCCESS)) {
				if (EXPECTED(status == INHERITANCE_UNRESOLVED)) {
					return INHERITANCE_UNRESOLVED;
				}
				ZEND_ASSERT(status == INHERITANCE_ERROR);
				ret = INHERITANCE_ERROR;
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
					if (EXPECTED(status == INHERITANCE_UNRESOLVED)) {
						return INHERITANCE_UNRESOLVED;
					}
					ZEND_ASSERT(status == INHERITANCE_ERROR);
					ret = INHERITANCE_ERROR;
				}
			}
		}
	} ZEND_HASH_FOREACH_END();

	return ret;
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
