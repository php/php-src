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

#include "zend.h"
#include "zend_API.h"
#include "zend_compile.h"
#include "zend_execute.h"
#include "zend_inheritance.h"
#include "zend_smart_str.h"

static void ptr_dtor(zval *zv) /* {{{ */
{
	efree(Z_PTR_P(zv));
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

static void do_inherit_parent_constructor(zend_class_entry *ce) /* {{{ */
{
	zend_function *function, *new_function;

	if (!ce->parent) {
		return;
	}

	/* You cannot change create_object */
	ce->create_object = ce->parent->create_object;

	/* Inherit special functions if needed */
	if (!ce->get_iterator) {
		ce->get_iterator = ce->parent->get_iterator;
	}
	if (!ce->iterator_funcs.funcs) {
		ce->iterator_funcs.funcs = ce->parent->iterator_funcs.funcs;
	}
	if (!ce->__get) {
		ce->__get   = ce->parent->__get;
	}
	if (!ce->__set) {
		ce->__set = ce->parent->__set;
	}
	if (!ce->__unset) {
		ce->__unset = ce->parent->__unset;
	}
	if (!ce->__isset) {
		ce->__isset = ce->parent->__isset;
	}
	if (!ce->__call) {
		ce->__call = ce->parent->__call;
	}
	if (!ce->__callstatic) {
		ce->__callstatic = ce->parent->__callstatic;
	}
	if (!ce->__tostring) {
		ce->__tostring = ce->parent->__tostring;
	}
	if (!ce->clone) {
		ce->clone = ce->parent->clone;
	}
	if(!ce->serialize) {
		ce->serialize = ce->parent->serialize;
	}
	if(!ce->unserialize) {
		ce->unserialize = ce->parent->unserialize;
	}
	if (!ce->destructor) {
		ce->destructor   = ce->parent->destructor;
	}
	if (!ce->__debugInfo) {
		ce->__debugInfo = ce->parent->__debugInfo;
	}
	if (ce->constructor) {
		if (ce->parent->constructor && ce->parent->constructor->common.fn_flags & ZEND_ACC_FINAL) {
			zend_error(E_ERROR, "Cannot override final %s::%s() with %s::%s()",
				ce->parent->name->val, ce->parent->constructor->common.function_name->val,
				ce->name->val, ce->constructor->common.function_name->val
				);
		}
		return;
	}

	if ((function = zend_hash_str_find_ptr(&ce->parent->function_table, ZEND_CONSTRUCTOR_FUNC_NAME, sizeof(ZEND_CONSTRUCTOR_FUNC_NAME)-1)) != NULL) {
		/* inherit parent's constructor */
		if (function->type == ZEND_INTERNAL_FUNCTION) {
			if (ce->type & ZEND_INTERNAL_CLASS) {
				new_function = pemalloc(sizeof(zend_internal_function), 1);
				memcpy(new_function, function, sizeof(zend_internal_function));
			} else {
				new_function = zend_arena_alloc(&CG(arena), sizeof(zend_internal_function));
				memcpy(new_function, function, sizeof(zend_internal_function));
				new_function->common.fn_flags |= ZEND_ACC_ARENA_ALLOCATED;
			}
		} else {
			new_function = zend_arena_alloc(&CG(arena), sizeof(zend_op_array));
			memcpy(new_function, function, sizeof(zend_op_array));
		}
		zend_hash_str_update_ptr(&ce->function_table, ZEND_CONSTRUCTOR_FUNC_NAME, sizeof(ZEND_CONSTRUCTOR_FUNC_NAME)-1, new_function);
		function_add_ref(new_function);
	} else {
		/* Don't inherit the old style constructor if we already have the new style constructor */
		zend_string *lc_class_name;
		zend_string *lc_parent_class_name;

		lc_class_name = zend_string_tolower(ce->name);
		if (!zend_hash_exists(&ce->function_table, lc_class_name)) {
			lc_parent_class_name = zend_string_tolower(ce->parent->name);
			if (!zend_hash_exists(&ce->function_table, lc_parent_class_name) &&
					(function = zend_hash_find_ptr(&ce->parent->function_table, lc_parent_class_name)) != NULL) {
				if (function->common.fn_flags & ZEND_ACC_CTOR) {
					/* inherit parent's constructor */
					if (function->type == ZEND_INTERNAL_FUNCTION) {
						if (ce->type & ZEND_INTERNAL_CLASS) {
							new_function = pemalloc(sizeof(zend_internal_function), 1);
							memcpy(new_function, function, sizeof(zend_internal_function));
						} else {
							new_function = zend_arena_alloc(&CG(arena), sizeof(zend_internal_function));
							memcpy(new_function, function, sizeof(zend_internal_function));
							new_function->common.fn_flags |= ZEND_ACC_ARENA_ALLOCATED;
						}
					} else {
						new_function = zend_arena_alloc(&CG(arena), sizeof(zend_op_array));
						memcpy(new_function, function, sizeof(zend_op_array));
					}
					zend_hash_update_ptr(&ce->function_table, lc_parent_class_name, new_function);
					function_add_ref(new_function);
				}
			}
			zend_string_release(lc_parent_class_name);
		}
		zend_string_release(lc_class_name);
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

static zend_function *do_inherit_method(zend_function *old_function, zend_class_entry *ce) /* {{{ */
{
	zend_function *new_function;

	if (old_function->type == ZEND_INTERNAL_FUNCTION) {
		if (ce->type & ZEND_INTERNAL_CLASS) {
			new_function = pemalloc(sizeof(zend_internal_function), 1);
			memcpy(new_function, old_function, sizeof(zend_internal_function));
		} else {
			new_function = zend_arena_alloc(&CG(arena), sizeof(zend_internal_function));
			memcpy(new_function, old_function, sizeof(zend_internal_function));
			new_function->common.fn_flags |= ZEND_ACC_ARENA_ALLOCATED;
		}
	} else {
		new_function = zend_arena_alloc(&CG(arena), sizeof(zend_op_array));
		memcpy(new_function, old_function, sizeof(zend_op_array));
	}
	/* The class entry of the derived function intentionally remains the same
	 * as that of the parent class.  That allows us to know in which context
	 * we're running, and handle private method calls properly.
	 */
	function_add_ref(new_function);
	return new_function;
}
/* }}} */

static int zend_do_perform_type_hint_check(const zend_function *fe, zend_arg_info *fe_arg_info, const zend_function *proto, zend_arg_info *proto_arg_info) /* {{{ */
{
	if (ZEND_LOG_XOR(fe_arg_info->class_name, proto_arg_info->class_name)) {
		/* Only one has a type hint and the other one doesn't */
		return 0;
	}

	if (fe_arg_info->class_name) {
		zend_string *fe_class_name, *proto_class_name;
		const char *class_name;

		if (fe->type == ZEND_INTERNAL_FUNCTION) {				
			fe_class_name = NULL;
			class_name = ((zend_internal_arg_info*)fe_arg_info)->class_name;
		} else {
			fe_class_name = fe_arg_info->class_name;
			class_name = fe_arg_info->class_name->val;
		}
		if (!strcasecmp(class_name, "parent") && proto->common.scope) {
			fe_class_name = zend_string_copy(proto->common.scope->name);
		} else if (!strcasecmp(class_name, "self") && fe->common.scope) {
			fe_class_name = zend_string_copy(fe->common.scope->name);
		} else if (fe_class_name) {
			zend_string_addref(fe_class_name);
		} else {
			fe_class_name = zend_string_init(class_name, strlen(class_name), 0);
		}

		if (proto->type == ZEND_INTERNAL_FUNCTION) {				
			proto_class_name = NULL;
			class_name = ((zend_internal_arg_info*)proto_arg_info)->class_name;
		} else {
			proto_class_name = proto_arg_info->class_name;
			class_name = proto_arg_info->class_name->val;
		}
		if (!strcasecmp(class_name, "parent") && proto->common.scope && proto->common.scope->parent) {
			proto_class_name = zend_string_copy(proto->common.scope->parent->name);
		} else if (!strcasecmp(class_name, "self") && proto->common.scope) {
			proto_class_name = zend_string_copy(proto->common.scope->name);
		} else if (proto_class_name) {
			zend_string_addref(proto_class_name);
		} else {
			proto_class_name = zend_string_init(class_name, strlen(class_name), 0);
		}

		if (strcasecmp(fe_class_name->val, proto_class_name->val) != 0) {
			const char *colon;

			if (fe->common.type != ZEND_USER_FUNCTION) {
				zend_string_release(proto_class_name);
				zend_string_release(fe_class_name);
				return 0;
			} else if (strchr(proto_class_name->val, '\\') != NULL ||
					(colon = zend_memrchr(fe_class_name->val, '\\', fe_class_name->len)) == NULL ||
					strcasecmp(colon+1, proto_class_name->val) != 0) {
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
	}

	if (fe_arg_info->type_hint != proto_arg_info->type_hint) {
		/* Incompatible type hint */
		return 0;
	}

	return 1;
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

	/* If both methods are private do not enforce a signature */
    if ((fe->common.fn_flags & ZEND_ACC_PRIVATE) && (proto->common.fn_flags & ZEND_ACC_PRIVATE)) {
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

		if (!zend_do_perform_type_hint_check(fe, fe_arg_info, proto, proto_arg_info)) {
			return 0;
		}

		/* by-ref constraints on arguments are invariant */
		if (fe_arg_info->pass_by_reference != proto_arg_info->pass_by_reference) {
			return 0;
		}
	}

	/* check return type compataibility */
	if ((proto->common.fn_flags | fe->common.fn_flags) & ZEND_ACC_HAS_RETURN_TYPE) {
		if ((proto->common.fn_flags ^ fe->common.fn_flags) & ZEND_ACC_HAS_RETURN_TYPE) {
			return 0;
		}
		if (!zend_do_perform_type_hint_check(fe, fe->common.arg_info - 1, proto, proto->common.arg_info - 1)) {
			return 0;
		}
	}
	return 1;
}
/* }}} */

static void zend_append_type_hint(smart_str *str, zend_function *fptr, zend_arg_info *arg_info, int return_hint) /* {{{ */
{
	if (arg_info->class_name) {
		const char *class_name;
		size_t class_name_len;

		if (fptr->type == ZEND_INTERNAL_FUNCTION) {
			class_name = ((zend_internal_arg_info*)arg_info)->class_name;
			class_name_len = strlen(class_name);
		} else {
			class_name = arg_info->class_name->val;
			class_name_len = arg_info->class_name->len;
		}

		if (!strcasecmp(class_name, "self") && fptr->common.scope) {
			class_name = fptr->common.scope->name->val;
			class_name_len = fptr->common.scope->name->len;
		} else if (!strcasecmp(class_name, "parent") && fptr->common.scope && fptr->common.scope->parent) {
			class_name = fptr->common.scope->parent->name->val;
			class_name_len = fptr->common.scope->parent->name->len;
		}

		smart_str_appendl(str, class_name, class_name_len);
		if (!return_hint) {
			smart_str_appendc(str, ' ');
		}
	} else if (arg_info->type_hint) {
		const char *type_name = zend_get_type_by_const(arg_info->type_hint);
		smart_str_appends(str, type_name);
		if (!return_hint) {
			smart_str_appendc(str, ' ');
		}
	}
}
/* }}} */

static zend_string *zend_get_function_declaration(zend_function *fptr) /* {{{ */
{
	smart_str str = {0};

	if (fptr->op_array.fn_flags & ZEND_ACC_RETURN_REFERENCE) {
		smart_str_appends(&str, "& ");
	}

	if (fptr->common.scope) {
		smart_str_append(&str, fptr->common.scope->name);
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
					smart_str_appendl(&str, arg_info->name->val, arg_info->name->len);
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
						zval *zv = RT_CONSTANT(&fptr->op_array, precv->op2);

						if (Z_TYPE_P(zv) == IS_CONSTANT) {
							smart_str_append(&str, Z_STR_P(zv));
						} else if (Z_TYPE_P(zv) == IS_FALSE) {
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
							smart_str_appends(&str, "<expression>");
						} else {
							zend_string *zv_str = zval_get_string(zv);
							smart_str_append(&str, zv_str);
							zend_string_release(zv_str);
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

	if ((parent->common.scope->ce_flags & ZEND_ACC_INTERFACE) == 0
		&& parent->common.fn_flags & ZEND_ACC_ABSTRACT
		&& parent->common.scope != (child->common.prototype ? child->common.prototype->common.scope : child->common.scope)
		&& child->common.fn_flags & (ZEND_ACC_ABSTRACT|ZEND_ACC_IMPLEMENTED_ABSTRACT)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Can't inherit abstract function %s::%s() (previously declared abstract in %s)",
			parent->common.scope->name->val,
			child->common.function_name->val,
			child->common.prototype ? child->common.prototype->common.scope->name->val : child->common.scope->name->val);
	}

	if (parent_flags & ZEND_ACC_FINAL) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot override final method %s::%s()", ZEND_FN_SCOPE_NAME(parent), child->common.function_name->val);
	}

	child_flags	= child->common.fn_flags;
	/* You cannot change from static to non static and vice versa.
	 */
	if ((child_flags & ZEND_ACC_STATIC) != (parent_flags & ZEND_ACC_STATIC)) {
		if (child->common.fn_flags & ZEND_ACC_STATIC) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot make non static method %s::%s() static in class %s", ZEND_FN_SCOPE_NAME(parent), child->common.function_name->val, ZEND_FN_SCOPE_NAME(child));
		} else {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot make static method %s::%s() non static in class %s", ZEND_FN_SCOPE_NAME(parent), child->common.function_name->val, ZEND_FN_SCOPE_NAME(child));
		}
	}

	/* Disallow making an inherited method abstract. */
	if ((child_flags & ZEND_ACC_ABSTRACT) && !(parent_flags & ZEND_ACC_ABSTRACT)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot make non abstract method %s::%s() abstract in class %s", ZEND_FN_SCOPE_NAME(parent), child->common.function_name->val, ZEND_FN_SCOPE_NAME(child));
	}

	if (parent_flags & ZEND_ACC_CHANGED) {
		child->common.fn_flags |= ZEND_ACC_CHANGED;
	} else {
		/* Prevent derived classes from restricting access that was available in parent classes
		 */
		if ((child_flags & ZEND_ACC_PPP_MASK) > (parent_flags & ZEND_ACC_PPP_MASK)) {
			zend_error_noreturn(E_COMPILE_ERROR, "Access level to %s::%s() must be %s (as in class %s)%s", ZEND_FN_SCOPE_NAME(child), child->common.function_name->val, zend_visibility_string(parent_flags), ZEND_FN_SCOPE_NAME(parent), (parent_flags&ZEND_ACC_PUBLIC) ? "" : " or weaker");
		} else if (((child_flags & ZEND_ACC_PPP_MASK) < (parent_flags & ZEND_ACC_PPP_MASK))
			&& ((parent_flags & ZEND_ACC_PPP_MASK) & ZEND_ACC_PRIVATE)) {
			child->common.fn_flags |= ZEND_ACC_CHANGED;
		}
	}

	if (parent_flags & ZEND_ACC_PRIVATE) {
		child->common.prototype = NULL;
	} else if (parent_flags & ZEND_ACC_ABSTRACT) {
		child->common.fn_flags |= ZEND_ACC_IMPLEMENTED_ABSTRACT;
		child->common.prototype = parent;
	} else if (!(parent->common.fn_flags & ZEND_ACC_CTOR) || (parent->common.prototype && (parent->common.prototype->common.scope->ce_flags & ZEND_ACC_INTERFACE))) {
		/* ctors only have a prototype if it comes from an interface */
		child->common.prototype = parent->common.prototype ? parent->common.prototype : parent;
	}

	if (child->common.prototype && (
		child->common.prototype->common.fn_flags & (ZEND_ACC_ABSTRACT | ZEND_ACC_HAS_RETURN_TYPE)
	)) {
		if (!zend_do_perform_implementation_check(child, child->common.prototype)) {
			zend_error_noreturn(E_COMPILE_ERROR, "Declaration of %s::%s() must be compatible with %s", ZEND_FN_SCOPE_NAME(child), child->common.function_name->val, zend_get_function_declaration(child->common.prototype)->val);
		}
	} else if (EG(error_reporting) & E_STRICT || Z_TYPE(EG(user_error_handler)) != IS_UNDEF) { /* Check E_STRICT (or custom error handler) before the check so that we save some time */
		if (!zend_do_perform_implementation_check(child, parent)) {
			zend_string *method_prototype = zend_get_function_declaration(parent);
			zend_error(E_STRICT, "Declaration of %s::%s() should be compatible with %s", ZEND_FN_SCOPE_NAME(child), child->common.function_name->val, method_prototype->val);
			zend_string_free(method_prototype);
		}
	}
}
/* }}} */

static zend_bool do_inherit_method_check(HashTable *child_function_table, zend_function *parent, zend_string *key, zend_class_entry *child_ce) /* {{{ */
{
	uint32_t parent_flags = parent->common.fn_flags;
	zend_function *child;

	if ((child = zend_hash_find_ptr(child_function_table, key)) == NULL) {
		if (parent_flags & (ZEND_ACC_ABSTRACT)) {
			child_ce->ce_flags |= ZEND_ACC_IMPLICIT_ABSTRACT_CLASS;
		}
		return 1; /* method doesn't exist in child, copy from parent */
	}

	do_inheritance_check_on_method(child, parent);

	return 0;
}
/* }}} */

static zend_bool do_inherit_property_access_check(zend_property_info *parent_info, zend_string *key, zend_class_entry *ce) /* {{{ */
{
	zend_property_info *child_info;
	zend_class_entry *parent_ce = ce->parent;

	if (parent_info->flags & (ZEND_ACC_PRIVATE|ZEND_ACC_SHADOW)) {
		if ((child_info = zend_hash_find_ptr(&ce->properties_info, key)) != NULL) {
			child_info->flags |= ZEND_ACC_CHANGED;
		} else {
			if(ce->type & ZEND_INTERNAL_CLASS) {
				child_info = zend_duplicate_property_info_internal(parent_info);
			} else {
				child_info = zend_duplicate_property_info(parent_info);
			}
			zend_hash_update_ptr(&ce->properties_info, key, child_info);
			child_info->flags &= ~ZEND_ACC_PRIVATE; /* it's not private anymore */
			child_info->flags |= ZEND_ACC_SHADOW; /* but it's a shadow of private */
		}
		return 0; /* don't copy access information to child */
	}

	if ((child_info = zend_hash_find_ptr(&ce->properties_info, key)) != NULL) {
		if ((parent_info->flags & ZEND_ACC_STATIC) != (child_info->flags & ZEND_ACC_STATIC)) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot redeclare %s%s::$%s as %s%s::$%s",
				(parent_info->flags & ZEND_ACC_STATIC) ? "static " : "non static ", parent_ce->name->val, key->val,
				(child_info->flags & ZEND_ACC_STATIC) ? "static " : "non static ", ce->name->val, key->val);

		}

		if(parent_info->flags & ZEND_ACC_CHANGED) {
			child_info->flags |= ZEND_ACC_CHANGED;
		}

		if ((child_info->flags & ZEND_ACC_PPP_MASK) > (parent_info->flags & ZEND_ACC_PPP_MASK)) {
			zend_error_noreturn(E_COMPILE_ERROR, "Access level to %s::$%s must be %s (as in class %s)%s", ce->name->val, key->val, zend_visibility_string(parent_info->flags), parent_ce->name->val, (parent_info->flags&ZEND_ACC_PUBLIC) ? "" : " or weaker");
		} else if ((child_info->flags & ZEND_ACC_STATIC) == 0) {
			int parent_num = OBJ_PROP_TO_NUM(parent_info->offset);
			int child_num = OBJ_PROP_TO_NUM(child_info->offset);

			zval_ptr_dtor(&(ce->default_properties_table[parent_num]));
			ce->default_properties_table[parent_num] = ce->default_properties_table[child_num];
			ZVAL_UNDEF(&ce->default_properties_table[child_num]);
			child_info->offset = parent_info->offset;
		}
		return 0;	/* Don't copy from parent */
	} else {
		return 1;	/* Copy from parent */
	}
}
/* }}} */

static inline void do_implement_interface(zend_class_entry *ce, zend_class_entry *iface) /* {{{ */
{
	if (!(ce->ce_flags & ZEND_ACC_INTERFACE) && iface->interface_gets_implemented && iface->interface_gets_implemented(iface, ce) == FAILURE) {
		zend_error(E_CORE_ERROR, "Class %s could not implement interface %s", ce->name->val, iface->name->val);
	}
	if (ce == iface) {
		zend_error(E_ERROR, "Interface %s cannot implement itself", ce->name->val);
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

#ifdef ZTS
# define zval_property_ctor(parent_ce, ce) \
	(((parent_ce)->type != (ce)->type) ? ZVAL_COPY_CTOR : zval_add_ref)
#else
# define zval_property_ctor(parent_ce, ce) \
	zval_add_ref
#endif

static void do_inherit_class_constant(zend_string *name, zval *zv, zend_class_entry *ce, zend_class_entry *parent_ce) /* {{{ */
{
	if (!Z_ISREF_P(zv)) {
		if (parent_ce->type == ZEND_INTERNAL_CLASS) {
			ZVAL_NEW_PERSISTENT_REF(zv, zv);
		} else {
			ZVAL_NEW_REF(zv, zv);
		}
	}
	if (Z_CONSTANT_P(Z_REFVAL_P(zv))) {
		ce->ce_flags &= ~ZEND_ACC_CONSTANTS_UPDATED;
	}
	if (zend_hash_add(&ce->constants_table, name, zv)) {
		Z_ADDREF_P(zv);
	}
}
/* }}} */

ZEND_API void zend_do_inheritance(zend_class_entry *ce, zend_class_entry *parent_ce) /* {{{ */
{
	zend_property_info *property_info;
	zend_function *func;
	zend_string *key;
	zval *zv;

	if ((ce->ce_flags & ZEND_ACC_INTERFACE)
		&& !(parent_ce->ce_flags & ZEND_ACC_INTERFACE)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Interface %s may not inherit from class (%s)", ce->name->val, parent_ce->name->val);
	}
	if (parent_ce->ce_flags & ZEND_ACC_FINAL) {
		zend_error_noreturn(E_COMPILE_ERROR, "Class %s may not inherit from final class (%s)", ce->name->val, parent_ce->name->val);
	}

	ce->parent = parent_ce;
	/* Copy serialize/unserialize callbacks */
	if (!ce->serialize) {
		ce->serialize   = parent_ce->serialize;
	}
	if (!ce->unserialize) {
		ce->unserialize = parent_ce->unserialize;
	}

	/* Inherit interfaces */
	zend_do_inherit_interfaces(ce, parent_ce);

	/* Inherit properties */
	if (parent_ce->default_properties_count) {
		int i = ce->default_properties_count + parent_ce->default_properties_count;

		ce->default_properties_table = perealloc(ce->default_properties_table, sizeof(zval) * i, ce->type == ZEND_INTERNAL_CLASS);
		if (ce->default_properties_count) {
			while (i-- > parent_ce->default_properties_count) {
				ce->default_properties_table[i] = ce->default_properties_table[i - parent_ce->default_properties_count];
			}
		}
		for (i = 0; i < parent_ce->default_properties_count; i++) {
#ifdef ZTS
			if (parent_ce->type != ce->type) {
				ZVAL_DUP(&ce->default_properties_table[i], &parent_ce->default_properties_table[i]);
				if (Z_OPT_CONSTANT(ce->default_properties_table[i])) {
					ce->ce_flags &= ~ZEND_ACC_CONSTANTS_UPDATED;
				}
				continue;
			}
#endif

			ZVAL_COPY(&ce->default_properties_table[i], &parent_ce->default_properties_table[i]);
			if (Z_OPT_CONSTANT(ce->default_properties_table[i])) {
				ce->ce_flags &= ~ZEND_ACC_CONSTANTS_UPDATED;
			}
		}
		ce->default_properties_count += parent_ce->default_properties_count;
	}

	if (parent_ce->type != ce->type) {
		/* User class extends internal class */
		zend_update_class_constants(parent_ce );
		if (parent_ce->default_static_members_count) {
			int i = ce->default_static_members_count + parent_ce->default_static_members_count;

			ce->default_static_members_table = erealloc(ce->default_static_members_table, sizeof(zval) * i);
			if (ce->default_static_members_count) {
				while (i-- > parent_ce->default_static_members_count) {
					ce->default_static_members_table[i] = ce->default_static_members_table[i - parent_ce->default_static_members_count];
				}
			}
			for (i = 0; i < parent_ce->default_static_members_count; i++) {
				ZVAL_MAKE_REF(&CE_STATIC_MEMBERS(parent_ce)[i]);
				ce->default_static_members_table[i] = CE_STATIC_MEMBERS(parent_ce)[i];
				Z_ADDREF(ce->default_static_members_table[i]);
				if (Z_CONSTANT_P(Z_REFVAL(ce->default_static_members_table[i]))) {
					ce->ce_flags &= ~ZEND_ACC_CONSTANTS_UPDATED;
				}
			}
			ce->default_static_members_count += parent_ce->default_static_members_count;
			ce->static_members_table = ce->default_static_members_table;
		}
	} else {
		if (parent_ce->default_static_members_count) {
			int i = ce->default_static_members_count + parent_ce->default_static_members_count;

			ce->default_static_members_table = perealloc(ce->default_static_members_table, sizeof(zval) * i, ce->type == ZEND_INTERNAL_CLASS);
			if (ce->default_static_members_count) {
				while (i-- > parent_ce->default_static_members_count) {
					ce->default_static_members_table[i] = ce->default_static_members_table[i - parent_ce->default_static_members_count];
				}
			}
			for (i = 0; i < parent_ce->default_static_members_count; i++) {
				ZVAL_MAKE_REF(&parent_ce->default_static_members_table[i]);
				ce->default_static_members_table[i] = parent_ce->default_static_members_table[i];
				Z_ADDREF(ce->default_static_members_table[i]);
				if (Z_CONSTANT_P(Z_REFVAL(ce->default_static_members_table[i]))) {
					ce->ce_flags &= ~ZEND_ACC_CONSTANTS_UPDATED;
				}
			}
			ce->default_static_members_count += parent_ce->default_static_members_count;
			if (ce->type == ZEND_USER_CLASS) {
				ce->static_members_table = ce->default_static_members_table;
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

	ZEND_HASH_FOREACH_STR_KEY_PTR(&parent_ce->properties_info, key, property_info) {
		if (do_inherit_property_access_check(property_info, key, ce)) {
			if (ce->type & ZEND_INTERNAL_CLASS) {
				property_info = zend_duplicate_property_info_internal(property_info);
			} else {
				property_info = zend_duplicate_property_info(property_info);
			}
			zend_hash_add_new_ptr(&ce->properties_info, key, property_info);
		}
	} ZEND_HASH_FOREACH_END();

	ZEND_HASH_FOREACH_STR_KEY_VAL(&parent_ce->constants_table, key, zv) {
		do_inherit_class_constant(key, zv, ce, parent_ce);
	} ZEND_HASH_FOREACH_END();

	ZEND_HASH_FOREACH_STR_KEY_PTR(&parent_ce->function_table, key, func) {
		if (do_inherit_method_check(&ce->function_table, func, key, ce)) {
			zend_function *new_func = do_inherit_method(func, ce);
			zend_hash_add_new_ptr(&ce->function_table, key, new_func);
		}
	} ZEND_HASH_FOREACH_END();

	do_inherit_parent_constructor(ce);

	if (ce->ce_flags & ZEND_ACC_IMPLICIT_ABSTRACT_CLASS && ce->type == ZEND_INTERNAL_CLASS) {
		ce->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;
	} else if (!(ce->ce_flags & (ZEND_ACC_IMPLEMENT_INTERFACES|ZEND_ACC_IMPLEMENT_TRAITS))) {
		/* The verification will be done in runtime by ZEND_VERIFY_ABSTRACT_CLASS */
		zend_verify_abstract_class(ce);
	}
	ce->ce_flags |= parent_ce->ce_flags & ZEND_HAS_STATIC_IN_METHODS;
}
/* }}} */

static zend_bool do_inherit_constant_check(HashTable *child_constants_table, zval *parent_constant, zend_string *name, const zend_class_entry *iface) /* {{{ */
{
	zval *old_constant;

	if ((old_constant = zend_hash_find(child_constants_table, name)) != NULL) {
		if (!Z_ISREF_P(old_constant) ||
		    !Z_ISREF_P(parent_constant) ||
		    Z_REFVAL_P(old_constant) != Z_REFVAL_P(parent_constant)) {
			zend_error_noreturn(E_COMPILE_ERROR, "Cannot inherit previously-inherited or override constant %s from interface %s", name->val, iface->name->val);
		}
		return 0;
	}
	return 1;
}
/* }}} */

static void do_inherit_iface_constant(zend_string *name, zval *zv, zend_class_entry *ce, zend_class_entry *iface) /* {{{ */
{
	if (do_inherit_constant_check(&ce->constants_table, zv, name, iface)) {
		ZVAL_MAKE_REF(zv);
		Z_ADDREF_P(zv);
		if (Z_CONSTANT_P(Z_REFVAL_P(zv))) {
			ce->ce_flags &= ~ZEND_ACC_CONSTANTS_UPDATED;
		}
		zend_hash_update(&ce->constants_table, name, zv);
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
	zval *zv;

	for (i = 0; i < ce->num_interfaces; i++) {
		if (ce->interfaces[i] == NULL) {
			memmove(ce->interfaces + i, ce->interfaces + i + 1, sizeof(zend_class_entry*) * (--ce->num_interfaces - i));
			i--;
		} else if (ce->interfaces[i] == iface) {
			if (i < parent_iface_num) {
				ignore = 1;
			} else {
				zend_error_noreturn(E_COMPILE_ERROR, "Class %s cannot implement previously implemented interface %s", ce->name->val, iface->name->val);
			}
		}
	}
	if (ignore) {
		/* Check for attempt to redeclare interface constants */
		ZEND_HASH_FOREACH_STR_KEY_VAL(&ce->constants_table, key, zv) {
			do_inherit_constant_check(&iface->constants_table, zv, key, iface);
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

		ZEND_HASH_FOREACH_STR_KEY_VAL(&iface->constants_table, key, zv) {
			do_inherit_iface_constant(key, zv, ce, iface);
		} ZEND_HASH_FOREACH_END();

		ZEND_HASH_FOREACH_STR_KEY_PTR(&iface->function_table, key, func) {
			if (do_inherit_method_check(&ce->function_table, func, key, ce)) {
				zend_function *new_func = do_inherit_method(func, ce);
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
		&& ((other_fn->common.scope->ce_flags & ZEND_ACC_INTERFACE) || zend_do_perform_implementation_check(other_fn, fn))
		&& ((fn_flags & (ZEND_ACC_FINAL|ZEND_ACC_STATIC)) ==
		    (other_flags & (ZEND_ACC_FINAL|ZEND_ACC_STATIC))); /* equal final and static qualifier */
}
/* }}} */

static void zend_add_magic_methods(zend_class_entry* ce, zend_string* mname, zend_function* fe) /* {{{ */
{
	if (!strncmp(mname->val, ZEND_CLONE_FUNC_NAME, mname->len)) {
		ce->clone = fe; fe->common.fn_flags |= ZEND_ACC_CLONE;
	} else if (!strncmp(mname->val, ZEND_CONSTRUCTOR_FUNC_NAME, mname->len)) {
		if (ce->constructor && (!ce->parent || ce->constructor != ce->parent->constructor)) {
			zend_error_noreturn(E_COMPILE_ERROR, "%s has colliding constructor definitions coming from traits", ce->name->val);
		}
		ce->constructor = fe; fe->common.fn_flags |= ZEND_ACC_CTOR;
	} else if (!strncmp(mname->val, ZEND_DESTRUCTOR_FUNC_NAME,  mname->len)) {
		ce->destructor = fe; fe->common.fn_flags |= ZEND_ACC_DTOR;
	} else if (!strncmp(mname->val, ZEND_GET_FUNC_NAME, mname->len)) {
		ce->__get = fe;
	} else if (!strncmp(mname->val, ZEND_SET_FUNC_NAME, mname->len)) {
		ce->__set = fe;
	} else if (!strncmp(mname->val, ZEND_CALL_FUNC_NAME, mname->len)) {
		ce->__call = fe;
	} else if (!strncmp(mname->val, ZEND_UNSET_FUNC_NAME, mname->len)) {
		ce->__unset = fe;
	} else if (!strncmp(mname->val, ZEND_ISSET_FUNC_NAME, mname->len)) {
		ce->__isset = fe;
	} else if (!strncmp(mname->val, ZEND_CALLSTATIC_FUNC_NAME, mname->len)) {
		ce->__callstatic = fe;
	} else if (!strncmp(mname->val, ZEND_TOSTRING_FUNC_NAME, mname->len)) {
		ce->__tostring = fe;
	} else if (!strncmp(mname->val, ZEND_DEBUGINFO_FUNC_NAME, mname->len)) {
		ce->__debugInfo = fe;
	} else if (ce->name->len == mname->len) {
		zend_string *lowercase_name = zend_string_tolower(ce->name);
		lowercase_name = zend_new_interned_string(lowercase_name);
		if (!memcmp(mname->val, lowercase_name->val, mname->len)) {
			if (ce->constructor  && (!ce->parent || ce->constructor != ce->parent->constructor)) {
				zend_error_noreturn(E_COMPILE_ERROR, "%s has colliding constructor definitions coming from traits", ce->name->val);
			}
			ce->constructor = fe;
			fe->common.fn_flags |= ZEND_ACC_CTOR;
		}
		zend_string_release(lowercase_name);
	}
}
/* }}} */

static void zend_add_trait_method(zend_class_entry *ce, const char *name, zend_string *key, zend_function *fn, HashTable **overriden) /* {{{ */
{
	zend_function *existing_fn = NULL;
	zend_function *new_fn;

	if ((existing_fn = zend_hash_find_ptr(&ce->function_table, key)) != NULL) {
		if (existing_fn->common.scope == ce) {
			/* members from the current class override trait methods */
			/* use temporary *overriden HashTable to detect hidden conflict */
			if (*overriden) {
				if ((existing_fn = zend_hash_find_ptr(*overriden, key)) != NULL) {
					if (existing_fn->common.fn_flags & ZEND_ACC_ABSTRACT) {
						/* Make sure the trait method is compatible with previosly declared abstract method */
						if (!zend_traits_method_compatibility_check(fn, existing_fn)) {
							zend_error_noreturn(E_COMPILE_ERROR, "Declaration of %s must be compatible with %s",
								zend_get_function_declaration(fn)->val,
								zend_get_function_declaration(existing_fn)->val);
						}
					} else if (fn->common.fn_flags & ZEND_ACC_ABSTRACT) {
						/* Make sure the abstract declaration is compatible with previous declaration */
						if (!zend_traits_method_compatibility_check(existing_fn, fn)) {
							zend_error_noreturn(E_COMPILE_ERROR, "Declaration of %s must be compatible with %s",
								zend_get_function_declaration(fn)->val,
								zend_get_function_declaration(existing_fn)->val);
						}
						return;
					}
				}
			} else {
				ALLOC_HASHTABLE(*overriden);
				zend_hash_init_ex(*overriden, 8, NULL, ptr_dtor, 0, 0);
			}
			zend_hash_update_mem(*overriden, key, fn, sizeof(zend_function));
			return;
		} else if (existing_fn->common.fn_flags & ZEND_ACC_ABSTRACT) {
			/* Make sure the trait method is compatible with previosly declared abstract method */
			if (!zend_traits_method_compatibility_check(fn, existing_fn)) {
				zend_error_noreturn(E_COMPILE_ERROR, "Declaration of %s must be compatible with %s",
					zend_get_function_declaration(fn)->val,
					zend_get_function_declaration(existing_fn)->val);
			}
		} else if (fn->common.fn_flags & ZEND_ACC_ABSTRACT) {
			/* Make sure the abstract declaration is compatible with previous declaration */
			if (!zend_traits_method_compatibility_check(existing_fn, fn)) {
				zend_error_noreturn(E_COMPILE_ERROR, "Declaration of %s must be compatible with %s",
					zend_get_function_declaration(fn)->val,
					zend_get_function_declaration(existing_fn)->val);
			}
			return;
		} else if ((existing_fn->common.scope->ce_flags & ZEND_ACC_TRAIT) == ZEND_ACC_TRAIT) {
			/* two traits can't define the same non-abstract method */
#if 1
			zend_error_noreturn(E_COMPILE_ERROR, "Trait method %s has not been applied, because there are collisions with other trait methods on %s",
				name, ce->name->val);
#else		/* TODO: better error message */
			zend_error_noreturn(E_COMPILE_ERROR, "Trait method %s::%s has not been applied as %s::%s, because of collision with %s::%s",
				fn->common.scope->name->val, fn->common.function_name->val,
				ce->name->val, name,
				existing_fn->common.scope->name->val, existing_fn->common.function_name->val);
#endif
		} else {
			/* inherited members are overridden by members inserted by traits */
			/* check whether the trait method fulfills the inheritance requirements */
			do_inheritance_check_on_method(fn, existing_fn);
		}
	}

	function_add_ref(fn);
	new_fn = zend_arena_alloc(&CG(arena), sizeof(zend_op_array));
	memcpy(new_fn, fn, sizeof(zend_op_array));
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
		if (fn->op_array.static_variables) {
			ce->ce_flags |= ZEND_HAS_STATIC_IN_METHODS;
		}
	}
}
/* }}} */

static int zend_traits_copy_functions(zend_string *fnname, zend_function *fn, zend_class_entry *ce, HashTable **overriden, HashTable *exclude_table) /* {{{ */
{
	zend_trait_alias  *alias, **alias_ptr;
	zend_string       *lcname;
	zend_function      fn_copy;

	/* apply aliases which are qualified with a class name, there should not be any ambiguity */
	if (ce->trait_aliases) {
		alias_ptr = ce->trait_aliases;
		alias = *alias_ptr;
		while (alias) {
			/* Scope unset or equal to the function we compare to, and the alias applies to fn */
			if (alias->alias != NULL
				&& (!alias->trait_method->ce || fn->common.scope == alias->trait_method->ce)
				&& alias->trait_method->method_name->len == fnname->len
				&& (zend_binary_strcasecmp(alias->trait_method->method_name->val, alias->trait_method->method_name->len, fnname->val, fnname->len) == 0)) {
				fn_copy = *fn;

				/* if it is 0, no modifieres has been changed */
				if (alias->modifiers) {
					fn_copy.common.fn_flags = alias->modifiers | (fn->common.fn_flags ^ (fn->common.fn_flags & ZEND_ACC_PPP_MASK));
				}

				lcname = zend_string_tolower(alias->alias);
				zend_add_trait_method(ce, alias->alias->val, lcname, &fn_copy, overriden);
				zend_string_release(lcname);

				/* Record the trait from which this alias was resolved. */
				if (!alias->trait_method->ce) {
					alias->trait_method->ce = fn->common.scope;
				}
			}
			alias_ptr++;
			alias = *alias_ptr;
		}
	}

	if (exclude_table == NULL || zend_hash_find(exclude_table, fnname) == NULL) {
		/* is not in hashtable, thus, function is not to be excluded */
		fn_copy = *fn;

		/* apply aliases which have not alias name, just setting visibility */
		if (ce->trait_aliases) {
			alias_ptr = ce->trait_aliases;
			alias = *alias_ptr;
			while (alias) {
				/* Scope unset or equal to the function we compare to, and the alias applies to fn */
				if (alias->alias == NULL && alias->modifiers != 0
					&& (!alias->trait_method->ce || fn->common.scope == alias->trait_method->ce)
					&& (alias->trait_method->method_name->len == fnname->len)
					&& (zend_binary_strcasecmp(alias->trait_method->method_name->val, alias->trait_method->method_name->len, fnname->val, fnname->len) == 0)) {

					fn_copy.common.fn_flags = alias->modifiers | (fn->common.fn_flags ^ (fn->common.fn_flags & ZEND_ACC_PPP_MASK));

					/** Record the trait from which this alias was resolved. */
					if (!alias->trait_method->ce) {
						alias->trait_method->ce = fn->common.scope;
					}
				}
				alias_ptr++;
				alias = *alias_ptr;
			}
		}

		zend_add_trait_method(ce, fn->common.function_name->val, fnname, &fn_copy, overriden);
	}

	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

static void zend_check_trait_usage(zend_class_entry *ce, zend_class_entry *trait) /* {{{ */
{
	uint32_t i;

	if ((trait->ce_flags & ZEND_ACC_TRAIT) != ZEND_ACC_TRAIT) {
		zend_error_noreturn(E_COMPILE_ERROR, "Class %s is not a trait, Only traits may be used in 'as' and 'insteadof' statements", trait->name->val);
	}

	for (i = 0; i < ce->num_traits; i++) {
		if (ce->traits[i] == trait) {
			return;
		}
	}
	zend_error_noreturn(E_COMPILE_ERROR, "Required Trait %s wasn't added to %s", trait->name->val, ce->name->val);
}
/* }}} */

static void zend_traits_init_trait_structures(zend_class_entry *ce) /* {{{ */
{
	size_t i, j = 0;
	zend_trait_precedence *cur_precedence;
	zend_trait_method_reference *cur_method_ref;
	zend_string *lcname;
	zend_bool method_exists;

	/* resolve class references */
	if (ce->trait_precedences) {
		i = 0;
		while ((cur_precedence = ce->trait_precedences[i])) {
			/** Resolve classes for all precedence operations. */
			if (cur_precedence->exclude_from_classes) {
				cur_method_ref = cur_precedence->trait_method;
				if (!(cur_precedence->trait_method->ce = zend_fetch_class(cur_method_ref->class_name,
								ZEND_FETCH_CLASS_TRAIT|ZEND_FETCH_CLASS_NO_AUTOLOAD))) {
					zend_error_noreturn(E_COMPILE_ERROR, "Could not find trait %s", cur_method_ref->class_name->val);
				}
				zend_check_trait_usage(ce, cur_precedence->trait_method->ce);

				/** Ensure that the preferred method is actually available. */
				lcname = zend_string_tolower(cur_method_ref->method_name);
				method_exists = zend_hash_exists(&cur_method_ref->ce->function_table,
												 lcname);
				zend_string_release(lcname);
				if (!method_exists) {
					zend_error_noreturn(E_COMPILE_ERROR,
							   "A precedence rule was defined for %s::%s but this method does not exist",
							   cur_method_ref->ce->name->val,
							   cur_method_ref->method_name->val);
				}

				/** With the other traits, we are more permissive.
					We do not give errors for those. This allows to be more
					defensive in such definitions.
					However, we want to make sure that the insteadof declaration
					is consistent in itself.
				 */
				j = 0;
				while (cur_precedence->exclude_from_classes[j].class_name) {
					zend_string* class_name = cur_precedence->exclude_from_classes[j].class_name;

					if (!(cur_precedence->exclude_from_classes[j].ce = zend_fetch_class(class_name, ZEND_FETCH_CLASS_TRAIT |ZEND_FETCH_CLASS_NO_AUTOLOAD))) {
						zend_error_noreturn(E_COMPILE_ERROR, "Could not find trait %s", class_name->val);
					}
					zend_check_trait_usage(ce, cur_precedence->exclude_from_classes[j].ce);

					/* make sure that the trait method is not from a class mentioned in
					 exclude_from_classes, for consistency */
					if (cur_precedence->trait_method->ce == cur_precedence->exclude_from_classes[j].ce) {
						zend_error_noreturn(E_COMPILE_ERROR,
								   "Inconsistent insteadof definition. "
								   "The method %s is to be used from %s, but %s is also on the exclude list",
								   cur_method_ref->method_name->val,
								   cur_precedence->trait_method->ce->name->val,
								   cur_precedence->trait_method->ce->name->val);
					}

					zend_string_release(class_name);
					j++;
				}
			}
			i++;
		}
	}

	if (ce->trait_aliases) {
		i = 0;
		while (ce->trait_aliases[i]) {
			/** For all aliases with an explicit class name, resolve the class now. */
			if (ce->trait_aliases[i]->trait_method->class_name) {
				cur_method_ref = ce->trait_aliases[i]->trait_method;
				if (!(cur_method_ref->ce = zend_fetch_class(cur_method_ref->class_name, ZEND_FETCH_CLASS_TRAIT|ZEND_FETCH_CLASS_NO_AUTOLOAD))) {
					zend_error_noreturn(E_COMPILE_ERROR, "Could not find trait %s", cur_method_ref->class_name->val);
				}
				zend_check_trait_usage(ce, cur_method_ref->ce);

				/** And, ensure that the referenced method is resolvable, too. */
				lcname = zend_string_tolower(cur_method_ref->method_name);
				method_exists = zend_hash_exists(&cur_method_ref->ce->function_table,
						lcname);
				zend_string_release(lcname);

				if (!method_exists) {
					zend_error_noreturn(E_COMPILE_ERROR, "An alias was defined for %s::%s but this method does not exist", cur_method_ref->ce->name->val, cur_method_ref->method_name->val);
				}
			}
			i++;
		}
	}
}
/* }}} */

static void zend_traits_compile_exclude_table(HashTable* exclude_table, zend_trait_precedence **precedences, zend_class_entry *trait) /* {{{ */
{
	size_t i = 0, j;

	if (!precedences) {
		return;
	}
	while (precedences[i]) {
		if (precedences[i]->exclude_from_classes) {
			j = 0;
			while (precedences[i]->exclude_from_classes[j].ce) {
				if (precedences[i]->exclude_from_classes[j].ce == trait) {
					zend_string *lcname =
						zend_string_tolower(precedences[i]->trait_method->method_name);
					if (zend_hash_add_empty_element(exclude_table, lcname) == NULL) {
						zend_string_release(lcname);
						zend_error_noreturn(E_COMPILE_ERROR, "Failed to evaluate a trait precedence (%s). Method of trait %s was defined to be excluded multiple times", precedences[i]->trait_method->method_name->val, trait->name->val);
					}
					zend_string_release(lcname);
				}
				++j;
			}
		}
		++i;
	}
}
/* }}} */

static void zend_do_traits_method_binding(zend_class_entry *ce) /* {{{ */
{
	uint32_t i;
	HashTable *overriden = NULL;
	zend_string *key;
	zend_function *fn;

	for (i = 0; i < ce->num_traits; i++) {
		if (ce->trait_precedences) {
			HashTable exclude_table;

			/* TODO: revisit this start size, may be its not optimal */
			zend_hash_init_ex(&exclude_table, 8, NULL, NULL, 0, 0);

			zend_traits_compile_exclude_table(&exclude_table, ce->trait_precedences, ce->traits[i]);

			/* copies functions, applies defined aliasing, and excludes unused trait methods */
			ZEND_HASH_FOREACH_STR_KEY_PTR(&ce->traits[i]->function_table, key, fn) {
				zend_traits_copy_functions(key, fn, ce, &overriden, &exclude_table);
			} ZEND_HASH_FOREACH_END();

			zend_hash_destroy(&exclude_table);
		} else {
			ZEND_HASH_FOREACH_STR_KEY_PTR(&ce->traits[i]->function_table, key, fn) {
				zend_traits_copy_functions(key, fn, ce, &overriden, NULL);
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
	zval compare_result;
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
			if ((flags & ZEND_ACC_PPP_MASK) == ZEND_ACC_PUBLIC) {
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
					zend_hash_del(&ce->properties_info, prop_name);
					flags |= ZEND_ACC_CHANGED;
				} else {
					if ((coliding_prop->flags & (ZEND_ACC_PPP_MASK | ZEND_ACC_STATIC))
						== (flags & (ZEND_ACC_PPP_MASK | ZEND_ACC_STATIC))) {
						/* flags are identical, now the value needs to be checked */
						if (flags & ZEND_ACC_STATIC) {
							not_compatible = (FAILURE == compare_function(&compare_result,
											  &ce->default_static_members_table[coliding_prop->offset],
											  &ce->traits[i]->default_static_members_table[property_info->offset]))
								  || (Z_LVAL(compare_result) != 0);
						} else {
							not_compatible = (FAILURE == compare_function(&compare_result,
											  &ce->default_properties_table[OBJ_PROP_TO_NUM(coliding_prop->offset)],
											  &ce->traits[i]->default_properties_table[OBJ_PROP_TO_NUM(property_info->offset)]))
								  || (Z_LVAL(compare_result) != 0);
						}
					} else {
						/* the flags are not identical, thus, we assume properties are not compatible */
						not_compatible = 1;
					}

					if (not_compatible) {
						zend_error_noreturn(E_COMPILE_ERROR,
							   "%s and %s define the same property ($%s) in the composition of %s. However, the definition differs and is considered incompatible. Class was composed",
								find_first_definition(ce, i, prop_name, coliding_prop->ce)->name->val,
								property_info->ce->name->val,
								prop_name->val,
								ce->name->val);
					} else {
						zend_error(E_STRICT,
							   "%s and %s define the same property ($%s) in the composition of %s. This might be incompatible, to improve maintainability consider using accessor methods in traits instead. Class was composed",
								find_first_definition(ce, i, prop_name, coliding_prop->ce)->name->val,
								property_info->ce->name->val,
								prop_name->val,
								ce->name->val);
						zend_string_release(prop_name);
						continue;
					}
				}
			}

			/* property not found, so lets add it */
			if (flags & ZEND_ACC_STATIC) {
				prop_value = &ce->traits[i]->default_static_members_table[property_info->offset];
			} else {
				prop_value = &ce->traits[i]->default_properties_table[OBJ_PROP_TO_NUM(property_info->offset)];
			}
			if (Z_REFCOUNTED_P(prop_value)) Z_ADDREF_P(prop_value);

			doc_comment = property_info->doc_comment ? zend_string_copy(property_info->doc_comment) : NULL;
			zend_declare_property_ex(ce, prop_name,
									 prop_value, flags,
								     doc_comment);
			zend_string_release(prop_name);
		} ZEND_HASH_FOREACH_END();
	}
}
/* }}} */

static void zend_do_check_for_inconsistent_traits_aliasing(zend_class_entry *ce) /* {{{ */
{
	int i = 0;
	zend_trait_alias* cur_alias;
	zend_string* lc_method_name;

	if (ce->trait_aliases) {
		while (ce->trait_aliases[i]) {
			cur_alias = ce->trait_aliases[i];
			/** The trait for this alias has not been resolved, this means, this
				alias was not applied. Abort with an error. */
			if (!cur_alias->trait_method->ce) {
				if (cur_alias->alias) {
					/** Plain old inconsistency/typo/bug */
					zend_error_noreturn(E_COMPILE_ERROR,
							   "An alias (%s) was defined for method %s(), but this method does not exist",
							   cur_alias->alias->val,
							   cur_alias->trait_method->method_name->val);
				} else {
					/** Here are two possible cases:
						1) this is an attempt to modifiy the visibility
						   of a method introduce as part of another alias.
						   Since that seems to violate the DRY principle,
						   we check against it and abort.
						2) it is just a plain old inconsitency/typo/bug
						   as in the case where alias is set. */

					lc_method_name = zend_string_tolower(
						cur_alias->trait_method->method_name);
					if (zend_hash_exists(&ce->function_table,
										 lc_method_name)) {
						zend_string_release(lc_method_name);
						zend_error_noreturn(E_COMPILE_ERROR,
								   "The modifiers for the trait alias %s() need to be changed in the same statement in which the alias is defined. Error",
								   cur_alias->trait_method->method_name->val);
					} else {
						zend_string_release(lc_method_name);
						zend_error_noreturn(E_COMPILE_ERROR,
								   "The modifiers of the trait method %s() are changed, but this method does not exist. Error",
								   cur_alias->trait_method->method_name->val);

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

	if (ce->num_traits <= 0) {
		return;
	}

	/* complete initialization of trait strutures in ce */
	zend_traits_init_trait_structures(ce);

	/* first care about all methods to be flattened into the class */
	zend_do_traits_method_binding(ce);

	/* Aliases which have not been applied indicate typos/bugs. */
	zend_do_check_for_inconsistent_traits_aliasing(ce);

	/* then flatten the properties into it, to, mostly to notfiy developer about problems */
	zend_do_traits_property_binding(ce);

	/* verify that all abstract methods from traits have been implemented */
	zend_verify_abstract_class(ce);

	/* now everything should be fine and an added ZEND_ACC_IMPLICIT_ABSTRACT_CLASS should be removed */
	if (ce->ce_flags & ZEND_ACC_IMPLICIT_ABSTRACT_CLASS) {
		ce->ce_flags -= ZEND_ACC_IMPLICIT_ABSTRACT_CLASS;
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
