/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2018 Zend Technologies Ltd. (http://www.zend.com) |
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
#include "zend_smart_str.h"
#include "zend_operators.h"

static void overriden_ptr_dtor(zval *zv) /* {{{ */
{
	efree_size(Z_PTR_P(zv), sizeof(zend_function));
}
/* }}} */

static zend_property_info *zend_duplicate_property_info(zend_property_info *property_info) /* {{{ */
{
	zend_property_info* new_property_info;

	new_property_info = zend_arena_alloc(&CG(arena), sizeof(zend_property_info));
	memcpy(new_property_info, property_info, sizeof(zend_property_info));
	zend_string_addref(new_property_info->name);
	if (new_property_info->doc_comment) {
		zend_string_addref(new_property_info->doc_comment);
	}
	return new_property_info;
}
/* }}} */

static zend_property_info *zend_duplicate_property_info_internal(zend_property_info *property_info) /* {{{ */
{
	zend_property_info* new_property_info = pemalloc(sizeof(zend_property_info), 1);
	memcpy(new_property_info, property_info, sizeof(zend_property_info));
	zend_string_addref(new_property_info->name);
	return new_property_info;
}
/* }}} */

static zend_function *zend_duplicate_function(zend_function *func, zend_class_entry *ce) /* {{{ */
{
	zend_function *new_function;

	if (UNEXPECTED(func->type == ZEND_INTERNAL_FUNCTION)) {
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
	} else {
		if (func->op_array.refcount) {
			(*func->op_array.refcount)++;
		}
		if (EXPECTED(!func->op_array.static_variables)) {
			/* reuse the same op_array structure */
			return func;
		}
		if (!(GC_FLAGS(func->op_array.static_variables) & IS_ARRAY_IMMUTABLE)) {
			GC_ADDREF(func->op_array.static_variables);
		}
		new_function = zend_arena_alloc(&CG(arena), sizeof(zend_op_array));
		memcpy(new_function, func, sizeof(zend_op_array));
	}
	return new_function;
}
/* }}} */

static void do_inherit_parent_constructor(zend_class_entry *ce) /* {{{ */
{
	ZEND_ASSERT(ce->parent != NULL);

	/* You cannot change create_object */
	ce->create_object = ce->parent->create_object;

	/* Inherit special functions if needed */
	if (EXPECTED(!ce->get_iterator)) {
		ce->get_iterator = ce->parent->get_iterator;
	}
	if (EXPECTED(!ce->iterator_funcs_ptr) && UNEXPECTED(ce->parent->iterator_funcs_ptr)) {
		if (ce->type == ZEND_INTERNAL_CLASS) {
			ce->iterator_funcs_ptr = calloc(1, sizeof(zend_class_iterator_funcs));
			if (ce->parent->iterator_funcs_ptr->zf_new_iterator) {
				ce->iterator_funcs_ptr->zf_new_iterator = zend_hash_str_find_ptr(&ce->function_table, "getiterator", sizeof("getiterator") - 1);
			}
			if (ce->parent->iterator_funcs_ptr->zf_current) {
				ce->iterator_funcs_ptr->zf_rewind = zend_hash_str_find_ptr(&ce->function_table, "rewind", sizeof("rewind") - 1);
				ce->iterator_funcs_ptr->zf_valid = zend_hash_str_find_ptr(&ce->function_table, "valid", sizeof("valid") - 1);
				ce->iterator_funcs_ptr->zf_key = zend_hash_str_find_ptr(&ce->function_table, "key", sizeof("key") - 1);
				ce->iterator_funcs_ptr->zf_current = zend_hash_str_find_ptr(&ce->function_table, "current", sizeof("current") - 1);
				ce->iterator_funcs_ptr->zf_next = zend_hash_str_find_ptr(&ce->function_table, "next", sizeof("next") - 1);
			}
		} else {
			ce->iterator_funcs_ptr = zend_arena_alloc(&CG(arena), sizeof(zend_class_iterator_funcs));
			memset(ce->iterator_funcs_ptr, 0, sizeof(zend_class_iterator_funcs));
		}
	}
	if (EXPECTED(!ce->__get)) {
		ce->__get = ce->parent->__get;
	}
	if (EXPECTED(!ce->__set)) {
		ce->__set = ce->parent->__set;
	}
	if (EXPECTED(!ce->__unset)) {
		ce->__unset = ce->parent->__unset;
	}
	if (EXPECTED(!ce->__isset)) {
		ce->__isset = ce->parent->__isset;
	}
	if (EXPECTED(!ce->__call)) {
		ce->__call = ce->parent->__call;
	}
	if (EXPECTED(!ce->__callstatic)) {
		ce->__callstatic = ce->parent->__callstatic;
	}
	if (EXPECTED(!ce->__tostring)) {
		ce->__tostring = ce->parent->__tostring;
	}
	if (EXPECTED(!ce->clone)) {
		ce->clone = ce->parent->clone;
	}
	if (EXPECTED(!ce->serialize)) {
		ce->serialize = ce->parent->serialize;
	}
	if (EXPECTED(!ce->unserialize)) {
		ce->unserialize = ce->parent->unserialize;
	}
	if (!ce->destructor) {
		ce->destructor = ce->parent->destructor;
	}
	if (EXPECTED(!ce->__debugInfo)) {
		ce->__debugInfo = ce->parent->__debugInfo;
	}

	if (ce->constructor) {
		if (ce->parent->constructor && UNEXPECTED(ce->parent->constructor->common.fn_flags & ZEND_ACC_FINAL)) {
			zend_error_noreturn(E_ERROR, "Cannot override final %s::%s() with %s::%s()",
				ZSTR_VAL(ce->parent->name), ZSTR_VAL(ce->parent->constructor->common.function_name),
				ZSTR_VAL(ce->name), ZSTR_VAL(ce->constructor->common.function_name));
		}
		return;
	}

	ce->constructor = ce->parent->constructor;
}
/* }}} */

char *zend_visibility_string(uint32_t fn_flags) /* {{{ */
{
	if (fn_flags & ZEND_ACC_PRIVATE) {
		return "private";
	}
	if (fn_flags & ZEND_ACC_PROTECTED) {
		return "protected";
	}
	if (fn_flags & ZEND_ACC_PUBLIC) {
		return "public";
	}
	return "";
}
/* }}} */

static zend_always_inline zend_bool zend_iterable_compatibility_check(zend_arg_info *arg_info) /* {{{ */
{
	if (ZEND_TYPE_CODE(arg_info->type) == IS_ARRAY) {
		return 1;
	}

	if (ZEND_TYPE_IS_CLASS(arg_info->type) && zend_string_equals_literal_ci(ZEND_TYPE_NAME(arg_info->type), "Traversable")) {
		return 1;
	}

	return 0;
}
/* }}} */

static int zend_do_perform_type_hint_check(const zend_function *fe, zend_arg_info *fe_arg_info, const zend_function *proto, zend_arg_info *proto_arg_info) /* {{{ */
{
	ZEND_ASSERT(ZEND_TYPE_IS_SET(fe_arg_info->type) && ZEND_TYPE_IS_SET(proto_arg_info->type));

	if (ZEND_TYPE_IS_CLASS(fe_arg_info->type) && ZEND_TYPE_IS_CLASS(proto_arg_info->type)) {
		zend_string *fe_class_name, *proto_class_name;
		const char *class_name;
		size_t class_name_len;

		fe_class_name = ZEND_TYPE_NAME(fe_arg_info->type);
		class_name = ZSTR_VAL(fe_class_name);
		class_name_len = ZSTR_LEN(fe_class_name);
		if (class_name_len == sizeof("parent")-1 && !strcasecmp(class_name, "parent") && proto->common.scope) {
			fe_class_name = zend_string_copy(proto->common.scope->name);
		} else if (class_name_len == sizeof("self")-1 && !strcasecmp(class_name, "self") && fe->common.scope) {
			fe_class_name = zend_string_copy(fe->common.scope->name);
		} else {
			zend_string_addref(fe_class_name);
		}

		proto_class_name = ZEND_TYPE_NAME(proto_arg_info->type);
		class_name = ZSTR_VAL(proto_class_name);
		class_name_len = ZSTR_LEN(proto_class_name);
		if (class_name_len == sizeof("parent")-1 && !strcasecmp(class_name, "parent") && proto->common.scope && proto->common.scope->parent) {
			proto_class_name = zend_string_copy(proto->common.scope->parent->name);
		} else if (class_name_len == sizeof("self")-1 && !strcasecmp(class_name, "self") && proto->common.scope) {
			proto_class_name = zend_string_copy(proto->common.scope->name);
		} else {
			zend_string_addref(proto_class_name);
		}

		if (fe_class_name != proto_class_name && strcasecmp(ZSTR_VAL(fe_class_name), ZSTR_VAL(proto_class_name)) != 0) {
			if (fe->common.type != ZEND_USER_FUNCTION) {
				zend_string_release(proto_class_name);
				zend_string_release(fe_class_name);
				return 0;
			} else {
				zend_class_entry *fe_ce, *proto_ce;

				fe_ce = zend_lookup_class(fe_class_name);
				proto_ce = zend_lookup_class(proto_class_name);

				/* Check for class alias */
				if (!fe_ce || !proto_ce ||
						fe_ce->type == ZEND_INTERNAL_CLASS ||
						proto_ce->type == ZEND_INTERNAL_CLASS ||
						fe_ce != proto_ce) {
					zend_string_release(proto_class_name);
					zend_string_release(fe_class_name);
					return 0;
				}
			}
		}
		zend_string_release(proto_class_name);
		zend_string_release(fe_class_name);
	} else if (ZEND_TYPE_CODE(fe_arg_info->type) != ZEND_TYPE_CODE(proto_arg_info->type)) {
		/* Incompatible built-in types */
		return 0;
	}

	return 1;
}
/* }}} */

static int zend_do_perform_arg_type_hint_check(const zend_function *fe, zend_arg_info *fe_arg_info, const zend_function *proto, zend_arg_info *proto_arg_info) /* {{{ */
{
	if (!ZEND_TYPE_IS_SET(fe_arg_info->type)) {
		/* Child with no type is always compatible */
		return 1;
	}

	if (!ZEND_TYPE_IS_SET(proto_arg_info->type)) {
		/* Child defines a type, but parent doesn't, violates LSP */
		return 0;
	}

	return zend_do_perform_type_hint_check(fe, fe_arg_info, proto, proto_arg_info);
}
/* }}} */

static zend_bool zend_do_perform_implementation_check(const zend_function *fe, const zend_function *proto) /* {{{ */
{
	uint32_t i, num_args;

	/* If it's a user function then arg_info == NULL means we don't have any parameters but
	 * we still need to do the arg number checks.  We are only willing to ignore this for internal
	 * functions because extensions don't always define arg_info.
	 */
	if (!proto || (!proto->common.arg_info && proto->common.type != ZEND_USER_FUNCTION)) {
		return 1;
	}

	/* Checks for constructors only if they are declared in an interface,
	 * or explicitly marked as abstract
	 */
	if ((fe->common.fn_flags & ZEND_ACC_CTOR)
		&& ((proto->common.scope->ce_flags & ZEND_ACC_INTERFACE) == 0
			&& (proto->common.fn_flags & ZEND_ACC_ABSTRACT) == 0)) {
		return 1;
	}

	/* If the prototype method is private do not enforce a signature */
	if (proto->common.fn_flags & ZEND_ACC_PRIVATE) {
		return 1;
	}

	/* check number of arguments */
	if (proto->common.required_num_args < fe->common.required_num_args
		|| proto->common.num_args > fe->common.num_args) {
		return 0;
	}

	/* by-ref constraints on return values are covariant */
	if ((proto->common.fn_flags & ZEND_ACC_RETURN_REFERENCE)
		&& !(fe->common.fn_flags & ZEND_ACC_RETURN_REFERENCE)) {
		return 0;
	}

	if ((proto->common.fn_flags & ZEND_ACC_VARIADIC)
		&& !(fe->common.fn_flags & ZEND_ACC_VARIADIC)) {
		return 0;
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

	for (i = 0; i < num_args; i++) {
		zend_arg_info *fe_arg_info = &fe->common.arg_info[i];

		zend_arg_info *proto_arg_info;
		if (i < proto->common.num_args) {
			proto_arg_info = &proto->common.arg_info[i];
		} else {
			proto_arg_info = &proto->common.arg_info[proto->common.num_args];
		}

		if (!zend_do_perform_arg_type_hint_check(fe, fe_arg_info, proto, proto_arg_info)) {
			switch (ZEND_TYPE_CODE(fe_arg_info->type)) {
				case IS_ITERABLE:
					if (!zend_iterable_compatibility_check(proto_arg_info)) {
						return 0;
					}
					break;

				default:
					return 0;
			}
		}

		// This introduces BC break described at https://bugs.php.net/bug.php?id=72119
		if (ZEND_TYPE_IS_SET(proto_arg_info->type) && ZEND_TYPE_ALLOW_NULL(proto_arg_info->type) && !ZEND_TYPE_ALLOW_NULL(fe_arg_info->type)) {
			/* incompatible nullability */
			return 0;
		}

		/* by-ref constraints on arguments are invariant */
		if (fe_arg_info->pass_by_reference != proto_arg_info->pass_by_reference) {
			return 0;
		}
	}

	/* Check return type compatibility, but only if the prototype already specifies
	 * a return type. Adding a new return type is always valid. */
	if (proto->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE) {
		/* Removing a return type is not valid. */
		if (!(fe->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE)) {
			return 0;
		}

		if (!zend_do_perform_type_hint_check(fe, fe->common.arg_info - 1, proto, proto->common.arg_info - 1)) {
			switch (ZEND_TYPE_CODE(proto->common.arg_info[-1].type)) {
				case IS_ITERABLE:
					if (!zend_iterable_compatibility_check(fe->common.arg_info - 1)) {
						return 0;
					}
					break;

				default:
					return 0;
			}
		}

		if (ZEND_TYPE_ALLOW_NULL(fe->common.arg_info[-1].type) && !ZEND_TYPE_ALLOW_NULL(proto->common.arg_info[-1].type)) {
			return 0;
		}
	}
	return 1;
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

static void do_inheritance_check_on_method(zend_function *child, zend_function *parent) /* {{{ */
{
	uint32_t child_flags;
	uint32_t parent_flags = parent->common.fn_flags;

	if (UNEXPECTED(parent_flags & ZEND_ACC_FINAL)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot override final method %s::%s()", ZEND_FN_SCOPE_NAME(parent), ZSTR_VAL(child->common.function_name));
	}

	child_flags	= child->common.fn_flags;
	/* You cannot change from static to non static and vice versa.
	 */
	if (UNEXPECTED((child_flags & ZEND_ACC_STATIC) != (parent_flags & ZEND_ACC_STATIC))) {
		if (child_flags & ZEND_ACC_STATIC) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot make non static method %s::%s() static in class %s", ZEND_FN_SCOPE_NAME(parent), ZSTR_VAL(child->common.function_name), ZEND_FN_SCOPE_NAME(child));
		} else {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot make static method %s::%s() non static in class %s", ZEND_FN_SCOPE_NAME(parent), ZSTR_VAL(child->common.function_name), ZEND_FN_SCOPE_NAME(child));
		}
	}

	/* Disallow making an inherited method abstract. */
	if (UNEXPECTED((child_flags & ZEND_ACC_ABSTRACT) > (parent_flags & ZEND_ACC_ABSTRACT))) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot make non abstract method %s::%s() abstract in class %s", ZEND_FN_SCOPE_NAME(parent), ZSTR_VAL(child->common.function_name), ZEND_FN_SCOPE_NAME(child));
	}

	/* Prevent derived classes from restricting access that was available in parent classes (except deriving from non-abstract ctors) */
	if (UNEXPECTED((!(child_flags & ZEND_ACC_CTOR) || (parent_flags & (ZEND_ACC_ABSTRACT | ZEND_ACC_IMPLEMENTED_ABSTRACT))) &&
		(child_flags & ZEND_ACC_PPP_MASK) > (parent_flags & ZEND_ACC_PPP_MASK))) {
		zend_error_noreturn(E_COMPILE_ERROR, "Access level to %s::%s() must be %s (as in class %s)%s", ZEND_FN_SCOPE_NAME(child), ZSTR_VAL(child->common.function_name), zend_visibility_string(parent_flags), ZEND_FN_SCOPE_NAME(parent), (parent_flags&ZEND_ACC_PUBLIC) ? "" : " or weaker");
	}

	if ((child_flags & ZEND_ACC_PRIVATE) < (parent_flags & (ZEND_ACC_PRIVATE|ZEND_ACC_CHANGED))) {
		child->common.fn_flags |= ZEND_ACC_CHANGED;
	}

	if (parent_flags & ZEND_ACC_PRIVATE) {
		child->common.prototype = NULL;
	} else if (parent_flags & ZEND_ACC_ABSTRACT) {
		child->common.fn_flags |= ZEND_ACC_IMPLEMENTED_ABSTRACT;
		child->common.prototype = parent;
	} else if (!(parent->common.fn_flags & ZEND_ACC_CTOR)) {
		child->common.prototype = parent->common.prototype ? parent->common.prototype : parent;
	} else if (parent->common.prototype && (parent->common.prototype->common.scope->ce_flags & ZEND_ACC_INTERFACE)) {
		/* ctors only have a prototype if it comes from an interface */
		child->common.prototype = parent->common.prototype ? parent->common.prototype : parent;
		/* and if that is the case, we want to check inheritance against it */
		parent = child->common.prototype;
	}

	if (UNEXPECTED(!zend_do_perform_implementation_check(child, parent))) {
		int error_level;
		const char *error_verb;
		zend_string *method_prototype = zend_get_function_declaration(parent);
		zend_string *child_prototype = zend_get_function_declaration(child);

		if (child->common.prototype && (
			child->common.prototype->common.fn_flags & ZEND_ACC_ABSTRACT
		)) {
			error_level = E_COMPILE_ERROR;
			error_verb = "must";
		} else if ((parent->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE) &&
                   (!(child->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE) ||
		            !zend_do_perform_type_hint_check(child, child->common.arg_info - 1, parent, parent->common.arg_info - 1) ||
		            (ZEND_TYPE_ALLOW_NULL(child->common.arg_info[-1].type) && !ZEND_TYPE_ALLOW_NULL(parent->common.arg_info[-1].type)))) {
			error_level = E_COMPILE_ERROR;
			error_verb = "must";
		} else {
			error_level = E_WARNING;
			error_verb = "should";
		}
		zend_error(error_level, "Declaration of %s %s be compatible with %s", ZSTR_VAL(child_prototype), error_verb, ZSTR_VAL(method_prototype));
		zend_string_efree(child_prototype);
		zend_string_efree(method_prototype);
	}
}
/* }}} */

static zend_function *do_inherit_method(zend_string *key, zend_function *parent, zend_class_entry *ce) /* {{{ */
{
	zval *child = zend_hash_find_ex(&ce->function_table, key, 1);

	if (child) {
		zend_function *func = (zend_function*)Z_PTR_P(child);
		zend_function *orig_prototype = func->common.prototype;

		do_inheritance_check_on_method(func, parent);
		if (func->common.prototype != orig_prototype &&
		    func->type == ZEND_USER_FUNCTION &&
		    func->common.scope != ce &&
		    !func->op_array.static_variables) {
			/* Lazy duplication */
			zend_function *new_function = zend_arena_alloc(&CG(arena), sizeof(zend_op_array));
			memcpy(new_function, func, sizeof(zend_op_array));
			Z_PTR_P(child) = new_function;
			func->common.prototype = orig_prototype;
		}
		return NULL;
	}

	if (parent->common.fn_flags & (ZEND_ACC_ABSTRACT)) {
		ce->ce_flags |= ZEND_ACC_IMPLICIT_ABSTRACT_CLASS;
	}

	return zend_duplicate_function(parent, ce);
}
/* }}} */

static void do_inherit_property(zend_property_info *parent_info, zend_string *key, zend_class_entry *ce) /* {{{ */
{
	zval *child = zend_hash_find_ex(&ce->properties_info, key, 1);
	zend_property_info *child_info;

	if (UNEXPECTED(child)) {
		child_info = Z_PTR_P(child);
		if (UNEXPECTED(parent_info->flags & (ZEND_ACC_PRIVATE|ZEND_ACC_SHADOW))) {
			child_info->flags |= ZEND_ACC_CHANGED;
		} else {
			if (UNEXPECTED((parent_info->flags & ZEND_ACC_STATIC) != (child_info->flags & ZEND_ACC_STATIC))) {
				zend_error_noreturn(E_COMPILE_ERROR, "Cannot redeclare %s%s::$%s as %s%s::$%s",
					(parent_info->flags & ZEND_ACC_STATIC) ? "static " : "non static ", ZSTR_VAL(ce->parent->name), ZSTR_VAL(key),
					(child_info->flags & ZEND_ACC_STATIC) ? "static " : "non static ", ZSTR_VAL(ce->name), ZSTR_VAL(key));
			}

			if (parent_info->flags & ZEND_ACC_CHANGED) {
				child_info->flags |= ZEND_ACC_CHANGED;
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
		}
	} else {
		if (UNEXPECTED(parent_info->flags & ZEND_ACC_PRIVATE)) {
			if (UNEXPECTED(ce->type & ZEND_INTERNAL_CLASS)) {
				child_info = zend_duplicate_property_info_internal(parent_info);
			} else {
				child_info = zend_duplicate_property_info(parent_info);
			}
			child_info->flags &= ~ZEND_ACC_PRIVATE; /* it's not private anymore */
			child_info->flags |= ZEND_ACC_SHADOW; /* but it's a shadow of private */
		} else {
			if (UNEXPECTED(ce->type & ZEND_INTERNAL_CLASS)) {
				child_info = zend_duplicate_property_info_internal(parent_info);
			} else {
				child_info = parent_info;
			}
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
	if (UNEXPECTED(ce == iface)) {
		zend_error_noreturn(E_ERROR, "Interface %s cannot implement itself", ZSTR_VAL(ce->name));
	}
}
/* }}} */

ZEND_API void zend_do_inherit_interfaces(zend_class_entry *ce, const zend_class_entry *iface) /* {{{ */
{
	/* expects interface to be contained in ce's interface list already */
	uint32_t i, ce_num, if_num = iface->num_interfaces;
	zend_class_entry *entry;

	if (if_num==0) {
		return;
	}
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

ZEND_API void zend_do_inheritance(zend_class_entry *ce, zend_class_entry *parent_ce) /* {{{ */
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

	ce->parent = parent_ce;

	/* Inherit interfaces */
	zend_do_inherit_interfaces(ce, parent_ce);

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
				ZVAL_COPY_VALUE(dst, src);
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
				ZVAL_COPY_OR_DUP(dst, src);
				if (Z_OPT_TYPE_P(dst) == IS_CONSTANT_AST) {
					ce->ce_flags &= ~ZEND_ACC_CONSTANTS_UPDATED;
				}
				continue;
			} while (dst != end);
		} else {
			do {
				dst--;
				src--;
				ZVAL_COPY(dst, src);
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
		if (ce->type == ZEND_USER_CLASS) {
			ce->static_members_table = ce->default_static_members_table;
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

		ZEND_HASH_FOREACH_STR_KEY_PTR(&parent_ce->function_table, key, func) {
			zend_function *new_func = do_inherit_method(key, func, ce);

			if (new_func) {
				_zend_hash_append_ptr(&ce->function_table, key, new_func);
			}
		} ZEND_HASH_FOREACH_END();
	}

	do_inherit_parent_constructor(ce);

	if (ce->ce_flags & ZEND_ACC_IMPLICIT_ABSTRACT_CLASS && ce->type == ZEND_INTERNAL_CLASS) {
		ce->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;
	} else if (!(ce->ce_flags & (ZEND_ACC_IMPLEMENT_INTERFACES|ZEND_ACC_IMPLEMENT_TRAITS))) {
		/* The verification will be done in runtime by ZEND_VERIFY_ABSTRACT_CLASS */
		zend_verify_abstract_class(ce);
	}
	ce->ce_flags |= parent_ce->ce_flags & (ZEND_HAS_STATIC_IN_METHODS | ZEND_ACC_USE_GUARDS);
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

ZEND_API void zend_do_implement_interface(zend_class_entry *ce, zend_class_entry *iface) /* {{{ */
{
	uint32_t i, ignore = 0;
	uint32_t current_iface_num = ce->num_interfaces;
	uint32_t parent_iface_num  = ce->parent ? ce->parent->num_interfaces : 0;
	zend_function *func;
	zend_string *key;
	zend_class_constant *c;

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

		ZEND_HASH_FOREACH_STR_KEY_PTR(&iface->constants_table, key, c) {
			do_inherit_iface_constant(key, c, ce, iface);
		} ZEND_HASH_FOREACH_END();

		ZEND_HASH_FOREACH_STR_KEY_PTR(&iface->function_table, key, func) {
			zend_function *new_func = do_inherit_method(key, func, ce);

			if (new_func) {
				zend_hash_add_new_ptr(&ce->function_table, key, new_func);
			}
		} ZEND_HASH_FOREACH_END();

		do_implement_interface(ce, iface);
		zend_do_inherit_interfaces(ce, iface);
	}
}
/* }}} */

ZEND_API void zend_do_implement_trait(zend_class_entry *ce, zend_class_entry *trait) /* {{{ */
{
	uint32_t i, ignore = 0;
	uint32_t current_trait_num = ce->num_traits;
	uint32_t parent_trait_num  = ce->parent ? ce->parent->num_traits : 0;

	for (i = 0; i < ce->num_traits; i++) {
		if (ce->traits[i] == NULL) {
			memmove(ce->traits + i, ce->traits + i + 1, sizeof(zend_class_entry*) * (--ce->num_traits - i));
			i--;
		} else if (ce->traits[i] == trait) {
			if (i < parent_trait_num) {
				ignore = 1;
			}
		}
	}
	if (!ignore) {
		if (ce->num_traits >= current_trait_num) {
			if (ce->type == ZEND_INTERNAL_CLASS) {
				ce->traits = (zend_class_entry **) realloc(ce->traits, sizeof(zend_class_entry *) * (++current_trait_num));
			} else {
				ce->traits = (zend_class_entry **) erealloc(ce->traits, sizeof(zend_class_entry *) * (++current_trait_num));
			}
		}
		ce->traits[ce->num_traits++] = trait;
	}
}
/* }}} */

static zend_bool zend_traits_method_compatibility_check(zend_function *fn, zend_function *other_fn) /* {{{ */
{
	uint32_t    fn_flags = fn->common.scope->ce_flags;
	uint32_t other_flags = other_fn->common.scope->ce_flags;

	return zend_do_perform_implementation_check(fn, other_fn)
		&& ((fn_flags & (ZEND_ACC_FINAL|ZEND_ACC_STATIC)) ==
		    (other_flags & (ZEND_ACC_FINAL|ZEND_ACC_STATIC))); /* equal final and static qualifier */
}
/* }}} */

static void zend_add_magic_methods(zend_class_entry* ce, zend_string* mname, zend_function* fe) /* {{{ */
{
	if (ZSTR_LEN(ce->name) == ZSTR_LEN(mname)) {
		zend_string *lowercase_name = zend_string_tolower(ce->name);
		lowercase_name = zend_new_interned_string(lowercase_name);
		if (!memcmp(ZSTR_VAL(mname), ZSTR_VAL(lowercase_name), ZSTR_LEN(mname))) {
			if (ce->constructor  && (!ce->parent || ce->constructor != ce->parent->constructor)) {
				zend_error_noreturn(E_COMPILE_ERROR, "%s has colliding constructor definitions coming from traits", ZSTR_VAL(ce->name));
			}
			ce->constructor = fe;
			fe->common.fn_flags |= ZEND_ACC_CTOR;
		}
		zend_string_release_ex(lowercase_name, 0);
	} else if (ZSTR_VAL(mname)[0] != '_' || ZSTR_VAL(mname)[1] != '_') {
		/* pass */
	} else if (zend_string_equals_literal(mname, ZEND_CLONE_FUNC_NAME)) {
		ce->clone = fe;
	} else if (zend_string_equals_literal(mname, ZEND_CONSTRUCTOR_FUNC_NAME)) {
		if (ce->constructor && (!ce->parent || ce->constructor != ce->parent->constructor)) {
			zend_error_noreturn(E_COMPILE_ERROR, "%s has colliding constructor definitions coming from traits", ZSTR_VAL(ce->name));
		}
		ce->constructor = fe; fe->common.fn_flags |= ZEND_ACC_CTOR;
	} else if (zend_string_equals_literal(mname, ZEND_DESTRUCTOR_FUNC_NAME)) {
		ce->destructor = fe; fe->common.fn_flags |= ZEND_ACC_DTOR;
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
	}
}
/* }}} */

static void zend_add_trait_method(zend_class_entry *ce, const char *name, zend_string *key, zend_function *fn, HashTable **overriden) /* {{{ */
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
			/* use temporary *overriden HashTable to detect hidden conflict */
			if (*overriden) {
				if ((existing_fn = zend_hash_find_ptr(*overriden, key)) != NULL) {
					if (existing_fn->common.fn_flags & ZEND_ACC_ABSTRACT) {
						/* Make sure the trait method is compatible with previosly declared abstract method */
						if (UNEXPECTED(!zend_traits_method_compatibility_check(fn, existing_fn))) {
							zend_error_noreturn(E_COMPILE_ERROR, "Declaration of %s must be compatible with %s",
								ZSTR_VAL(zend_get_function_declaration(fn)),
								ZSTR_VAL(zend_get_function_declaration(existing_fn)));
						}
					}
					if (fn->common.fn_flags & ZEND_ACC_ABSTRACT) {
						/* Make sure the abstract declaration is compatible with previous declaration */
						if (UNEXPECTED(!zend_traits_method_compatibility_check(existing_fn, fn))) {
							zend_error_noreturn(E_COMPILE_ERROR, "Declaration of %s must be compatible with %s",
								ZSTR_VAL(zend_get_function_declaration(existing_fn)),
								ZSTR_VAL(zend_get_function_declaration(fn)));
						}
						return;
					}
				}
			} else {
				ALLOC_HASHTABLE(*overriden);
				zend_hash_init_ex(*overriden, 8, NULL, overriden_ptr_dtor, 0, 0);
			}
			zend_hash_update_mem(*overriden, key, fn, sizeof(zend_function));
			return;
		} else if (existing_fn->common.fn_flags & ZEND_ACC_ABSTRACT &&
				(existing_fn->common.scope->ce_flags & ZEND_ACC_INTERFACE) == 0) {
			/* Make sure the trait method is compatible with previosly declared abstract method */
			if (UNEXPECTED(!zend_traits_method_compatibility_check(fn, existing_fn))) {
				zend_error_noreturn(E_COMPILE_ERROR, "Declaration of %s must be compatible with %s",
					ZSTR_VAL(zend_get_function_declaration(fn)),
					ZSTR_VAL(zend_get_function_declaration(existing_fn)));
			}
		} else if (fn->common.fn_flags & ZEND_ACC_ABSTRACT) {
			/* Make sure the abstract declaration is compatible with previous declaration */
			if (UNEXPECTED(!zend_traits_method_compatibility_check(existing_fn, fn))) {
				zend_error_noreturn(E_COMPILE_ERROR, "Declaration of %s must be compatible with %s",
					ZSTR_VAL(zend_get_function_declaration(existing_fn)),
					ZSTR_VAL(zend_get_function_declaration(fn)));
			}
			return;
		} else if (UNEXPECTED(existing_fn->common.scope->ce_flags & ZEND_ACC_TRAIT)) {
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
			do_inheritance_check_on_method(fn, existing_fn);
			fn->common.prototype = NULL;
		}
	}

	function_add_ref(fn);
	if (UNEXPECTED(fn->type == ZEND_INTERNAL_FUNCTION)) {
		new_fn = zend_arena_alloc(&CG(arena), sizeof(zend_internal_function));
		memcpy(new_fn, fn, sizeof(zend_internal_function));
		new_fn->common.fn_flags |= ZEND_ACC_ARENA_ALLOCATED;
	} else {
		new_fn = zend_arena_alloc(&CG(arena), sizeof(zend_op_array));
		memcpy(new_fn, fn, sizeof(zend_op_array));
	}
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

static int zend_traits_copy_functions(zend_string *fnname, zend_function *fn, zend_class_entry *ce, HashTable **overriden, HashTable *exclude_table, zend_class_entry **aliases) /* {{{ */
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
				zend_add_trait_method(ce, ZSTR_VAL(alias->alias), lcname, &fn_copy, overriden);
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

		zend_add_trait_method(ce, ZSTR_VAL(fn->common.function_name), fnname, &fn_copy, overriden);
	}

	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

static uint32_t zend_check_trait_usage(zend_class_entry *ce, zend_class_entry *trait) /* {{{ */
{
	uint32_t i;

	if (UNEXPECTED((trait->ce_flags & ZEND_ACC_TRAIT) != ZEND_ACC_TRAIT)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Class %s is not a trait, Only traits may be used in 'as' and 'insteadof' statements", ZSTR_VAL(trait->name));
		return 0;
	}

	for (i = 0; i < ce->num_traits; i++) {
		if (ce->traits[i] == trait) {
			return i;
		}
	}
	zend_error_noreturn(E_COMPILE_ERROR, "Required Trait %s wasn't added to %s", ZSTR_VAL(trait->name), ZSTR_VAL(ce->name));
	return 0;
}
/* }}} */

static void zend_traits_init_trait_structures(zend_class_entry *ce, HashTable ***exclude_tables_ptr, zend_class_entry ***aliases_ptr) /* {{{ */
{
	size_t i, j = 0;
	zend_trait_precedence **precedences;
	zend_trait_precedence *cur_precedence;
	zend_trait_method_reference *cur_method_ref;
	zend_string *lcname;
	zend_bool method_exists;
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
			zend_check_trait_usage(ce, trait);

			/** Ensure that the preferred method is actually available. */
			lcname = zend_string_tolower(cur_method_ref->method_name);
			method_exists = zend_hash_exists(&trait->function_table, lcname);
			if (!method_exists) {
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
				trait_num = zend_check_trait_usage(ce, exclude_ce);
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
				zend_check_trait_usage(ce, trait);
				aliases[i] = trait;

				/** And, ensure that the referenced method is resolvable, too. */
				lcname = zend_string_tolower(cur_method_ref->method_name);
				method_exists = zend_hash_exists(&trait->function_table, lcname);
				zend_string_release_ex(lcname, 0);

				if (!method_exists) {
					zend_error_noreturn(E_COMPILE_ERROR, "An alias was defined for %s::%s but this method does not exist", ZSTR_VAL(trait->name), ZSTR_VAL(cur_method_ref->method_name));
				}
			}
			i++;
		}
	}

	*exclude_tables_ptr = exclude_tables;
	*aliases_ptr = aliases;
}
/* }}} */

static void zend_do_traits_method_binding(zend_class_entry *ce, HashTable **exclude_tables, zend_class_entry **aliases) /* {{{ */
{
	uint32_t i;
	HashTable *overriden = NULL;
	zend_string *key;
	zend_function *fn;

	if (exclude_tables) {
		for (i = 0; i < ce->num_traits; i++) {
			/* copies functions, applies defined aliasing, and excludes unused trait methods */
			ZEND_HASH_FOREACH_STR_KEY_PTR(&ce->traits[i]->function_table, key, fn) {
				zend_traits_copy_functions(key, fn, ce, &overriden, exclude_tables[i], aliases);
			} ZEND_HASH_FOREACH_END();

			if (exclude_tables[i]) {
				zend_hash_destroy(exclude_tables[i]);
				FREE_HASHTABLE(exclude_tables[i]);
				exclude_tables[i] = NULL;
			}
		}
	} else {
		for (i = 0; i < ce->num_traits; i++) {
			ZEND_HASH_FOREACH_STR_KEY_PTR(&ce->traits[i]->function_table, key, fn) {
				zend_traits_copy_functions(key, fn, ce, &overriden, NULL, aliases);
			} ZEND_HASH_FOREACH_END();
		}
	}

	ZEND_HASH_FOREACH_PTR(&ce->function_table, fn) {
		zend_fixup_trait_method(fn, ce);
	} ZEND_HASH_FOREACH_END();

	if (overriden) {
		zend_hash_destroy(overriden);
		FREE_HASHTABLE(overriden);
	}
}
/* }}} */

static zend_class_entry* find_first_definition(zend_class_entry *ce, size_t current_trait, zend_string *prop_name, zend_class_entry *coliding_ce) /* {{{ */
{
	size_t i;

	if (coliding_ce == ce) {
		for (i = 0; i < current_trait; i++) {
			if (zend_hash_exists(&ce->traits[i]->properties_info, prop_name)) {
				return ce->traits[i];
			}
		}
	}

	return coliding_ce;
}
/* }}} */

static void zend_do_traits_property_binding(zend_class_entry *ce) /* {{{ */
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
		ZEND_HASH_FOREACH_PTR(&ce->traits[i]->properties_info, property_info) {
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
				if (coliding_prop->flags & ZEND_ACC_SHADOW) {
					zend_string_release_ex(coliding_prop->name, 0);
					if (coliding_prop->doc_comment) {
						zend_string_release_ex(coliding_prop->doc_comment, 0);
                    }
					zend_hash_del(&ce->properties_info, prop_name);
					flags |= ZEND_ACC_CHANGED;
				} else {
					not_compatible = 1;

					if ((coliding_prop->flags & (ZEND_ACC_PPP_MASK | ZEND_ACC_STATIC))
						== (flags & (ZEND_ACC_PPP_MASK | ZEND_ACC_STATIC))) {
						/* the flags are identical, thus, the properties may be compatible */
						zval *op1, *op2;
						zval op1_tmp, op2_tmp;

						if (flags & ZEND_ACC_STATIC) {
							op1 = &ce->default_static_members_table[coliding_prop->offset];
							op2 = &ce->traits[i]->default_static_members_table[property_info->offset];
							ZVAL_DEINDIRECT(op1);
							ZVAL_DEINDIRECT(op2);
						} else {
							op1 = &ce->default_properties_table[OBJ_PROP_TO_NUM(coliding_prop->offset)];
							op2 = &ce->traits[i]->default_properties_table[OBJ_PROP_TO_NUM(property_info->offset)];
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
								ZSTR_VAL(find_first_definition(ce, i, prop_name, coliding_prop->ce)->name),
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
				prop_value = &ce->traits[i]->default_static_members_table[property_info->offset];
				ZEND_ASSERT(Z_TYPE_P(prop_value) != IS_INDIRECT);
			} else {
				prop_value = &ce->traits[i]->default_properties_table[OBJ_PROP_TO_NUM(property_info->offset)];
			}

			Z_TRY_ADDREF_P(prop_value);
			doc_comment = property_info->doc_comment ? zend_string_copy(property_info->doc_comment) : NULL;
			zend_declare_property_ex(ce, prop_name,
									 prop_value, flags,
								     doc_comment);
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
						1) this is an attempt to modifiy the visibility
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

ZEND_API void zend_do_bind_traits(zend_class_entry *ce) /* {{{ */
{
	HashTable **exclude_tables;
	zend_class_entry **aliases;

	if (ce->num_traits == 0) {
		return;
	}

	/* complete initialization of trait strutures in ce */
	zend_traits_init_trait_structures(ce, &exclude_tables, &aliases);

	/* first care about all methods to be flattened into the class */
	zend_do_traits_method_binding(ce, exclude_tables, aliases);

	/* Aliases which have not been applied indicate typos/bugs. */
	zend_do_check_for_inconsistent_traits_aliasing(ce, aliases);

	if (aliases) {
		efree(aliases);
	}

	if (exclude_tables) {
		efree(exclude_tables);
	}

	/* then flatten the properties into it, to, mostly to notfiy developer about problems */
	zend_do_traits_property_binding(ce);

	/* verify that all abstract methods from traits have been implemented */
	zend_verify_abstract_class(ce);

	/* Emit E_DEPRECATED for PHP 4 constructors */
	zend_check_deprecated_constructor(ce);

	/* now everything should be fine and an added ZEND_ACC_IMPLICIT_ABSTRACT_CLASS should be removed */
	if (ce->ce_flags & ZEND_ACC_IMPLICIT_ABSTRACT_CLASS) {
		ce->ce_flags -= ZEND_ACC_IMPLICIT_ABSTRACT_CLASS;
	}
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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
