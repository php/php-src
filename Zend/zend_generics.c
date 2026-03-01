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
*/

#include "zend.h"
#include "zend_generics.h"
#include "zend_type_info.h"
#include "zend_compile.h"
#include "zend_execute.h"
#include "zend_smart_str.h"

ZEND_API zend_generic_params_info *zend_alloc_generic_params_info(uint32_t num_params)
{
	zend_generic_params_info *info = emalloc(ZEND_GENERIC_PARAMS_INFO_SIZE(num_params));
	info->num_params = num_params;
	for (uint32_t i = 0; i < num_params; i++) {
		info->params[i].name = NULL;
		info->params[i].constraint = (zend_type) ZEND_TYPE_INIT_NONE(0);
		info->params[i].default_type = (zend_type) ZEND_TYPE_INIT_NONE(0);
		info->params[i].variance = ZEND_GENERIC_VARIANCE_INVARIANT;
	}
	/* Initialize param-to-arg map to unmapped */
	int16_t *map = ZEND_GENERIC_PARAMS_ARG_MAP(info);
	for (uint32_t i = 0; i < num_params; i++) {
		map[i] = ZEND_GENERIC_ARG_UNMAPPED;
	}
	return info;
}

ZEND_API zend_generic_args *zend_alloc_generic_args(uint32_t num_args)
{
	zend_generic_args *args = emalloc(ZEND_GENERIC_ARGS_SIZE(num_args));
	args->refcount = 1;
	args->num_args = num_args;
	for (uint32_t i = 0; i < num_args; i++) {
		args->args[i] = (zend_type) ZEND_TYPE_INIT_NONE(0);
	}
	memset(ZEND_GENERIC_ARGS_MASKS(args), 0, num_args * sizeof(uint32_t));
	return args;
}

ZEND_API void zend_generic_args_compute_masks(zend_generic_args *args)
{
	uint32_t *masks = ZEND_GENERIC_ARGS_MASKS(args);
	for (uint32_t i = 0; i < args->num_args; i++) {
		zend_type t = args->args[i];
		if (!ZEND_TYPE_HAS_NAME(t) && !ZEND_TYPE_IS_GENERIC_CLASS(t)
		 && !ZEND_TYPE_HAS_LIST(t) && !ZEND_TYPE_IS_GENERIC_PARAM(t)) {
			masks[i] = ZEND_TYPE_PURE_MASK(t) & MAY_BE_ANY;
		} else {
			masks[i] = 0;  /* Complex type — requires slow path */
		}
	}
}

ZEND_API zend_type zend_copy_generic_type(zend_type src)
{
	if (ZEND_TYPE_IS_GENERIC_CLASS(src)) {
		/* Deep-copy the generic class ref */
		zend_generic_class_ref *orig = ZEND_TYPE_GENERIC_CLASS_REF(src);
		zend_generic_class_ref *ref = emalloc(sizeof(zend_generic_class_ref));
		ref->class_name = zend_string_copy(orig->class_name);
		ref->type_args = orig->type_args ? zend_copy_generic_args(orig->type_args) : NULL;
		if (orig->wildcard_bounds && orig->type_args) {
			ref->wildcard_bounds = emalloc(orig->type_args->num_args * sizeof(zend_generic_bound));
			memcpy(ref->wildcard_bounds, orig->wildcard_bounds, orig->type_args->num_args * sizeof(zend_generic_bound));
		} else {
			ref->wildcard_bounds = NULL;
		}
		return (zend_type) ZEND_TYPE_INIT_GENERIC_CLASS(ref, 0, 0);
	}
	if (ZEND_TYPE_IS_GENERIC_PARAM(src)) {
		/* Deep-copy the generic type ref */
		zend_generic_type_ref *orig = ZEND_TYPE_GENERIC_PARAM_REF(src);
		zend_generic_type_ref *ref = emalloc(sizeof(zend_generic_type_ref));
		ref->name = zend_string_copy(orig->name);
		ref->param_index = orig->param_index;
		ref->variance = orig->variance;
		return (zend_type) ZEND_TYPE_INIT_GENERIC_PARAM(ref, 0);
	}
	if (ZEND_TYPE_HAS_NAME(src)) {
		zend_string_addref(ZEND_TYPE_NAME(src));
	}
	return src;
}

ZEND_API zend_generic_args *zend_copy_generic_args(const zend_generic_args *src)
{
	uint32_t num_args = src->num_args;
	zend_generic_args *copy = emalloc(ZEND_GENERIC_ARGS_SIZE(num_args));
	copy->refcount = 1;
	copy->num_args = num_args;
	for (uint32_t i = 0; i < num_args; i++) {
		copy->args[i] = zend_copy_generic_type(src->args[i]);
	}
	memcpy(ZEND_GENERIC_ARGS_MASKS(copy), ZEND_GENERIC_ARGS_MASKS(src),
		num_args * sizeof(uint32_t));
	return copy;
}

ZEND_API void zend_generic_params_info_dtor(zend_generic_params_info *info)
{
	for (uint32_t i = 0; i < info->num_params; i++) {
		if (info->params[i].name) {
			zend_string_release(info->params[i].name);
		}
		zend_type_release(info->params[i].constraint, /* persistent */ 0);
		zend_type_release(info->params[i].default_type, /* persistent */ 0);
	}
	efree(info);
}

ZEND_API void zend_generic_args_dtor(zend_generic_args *args)
{
	for (uint32_t i = 0; i < args->num_args; i++) {
		zend_type_release(args->args[i], /* persistent */ 0);
	}
	efree(args);
}

ZEND_API void zend_generic_type_ref_dtor(zend_generic_type_ref *ref)
{
	if (ref->name) {
		zend_string_release(ref->name);
	}
	efree(ref);
}

ZEND_API void zend_generic_class_ref_dtor(zend_generic_class_ref *ref)
{
	if (ref->class_name) {
		zend_string_release(ref->class_name);
	}
	if (ref->type_args) {
		zend_generic_args_release(ref->type_args);
	}
	if (ref->wildcard_bounds) {
		efree(ref->wildcard_bounds);
	}
	efree(ref);
}

ZEND_API zend_type zend_resolve_generic_type(zend_type type, const zend_generic_args *args)
{
	if (ZEND_TYPE_IS_GENERIC_PARAM(type)) {
		zend_generic_type_ref *ref = ZEND_TYPE_GENERIC_PARAM_REF(type);
		if (args && ref->param_index < args->num_args) {
			return args->args[ref->param_index];
		}
	}
	return type;
}

ZEND_API zend_generic_args *zend_resolve_generic_args_with_context(
	const zend_generic_args *args, const zend_generic_args *context)
{
	if (!context) {
		return NULL;
	}

	/* Check if any args need resolution */
	bool needs_resolution = false;
	for (uint32_t i = 0; i < args->num_args; i++) {
		if (ZEND_TYPE_IS_GENERIC_PARAM(args->args[i])) {
			needs_resolution = true;
			break;
		}
	}
	if (!needs_resolution) {
		return NULL;
	}

	/* Create resolved copy */
	zend_generic_args *resolved = zend_alloc_generic_args(args->num_args);
	for (uint32_t i = 0; i < args->num_args; i++) {
		if (ZEND_TYPE_IS_GENERIC_PARAM(args->args[i])) {
			zend_type resolved_type = zend_resolve_generic_type(args->args[i], context);
			/* Copy the resolved type (may still be a param ref if context doesn't have it) */
			resolved->args[i] = zend_copy_generic_type(resolved_type);
		} else {
			resolved->args[i] = zend_copy_generic_type(args->args[i]);
		}
	}
	zend_generic_args_compute_masks(resolved);
	return resolved;
}

ZEND_API zend_generic_args *zend_expand_generic_args_with_defaults(
	const zend_generic_params_info *params, const zend_generic_args *args)
{
	if (args->num_args >= params->num_params) {
		return NULL; /* No expansion needed */
	}

	/* Verify that all missing args have defaults */
	for (uint32_t i = args->num_args; i < params->num_params; i++) {
		if (!ZEND_TYPE_IS_SET(params->params[i].default_type)) {
			return NULL; /* Missing arg without default — can't expand */
		}
	}

	/* Create expanded args with defaults filled in */
	zend_generic_args *expanded = zend_alloc_generic_args(params->num_params);
	for (uint32_t i = 0; i < args->num_args; i++) {
		expanded->args[i] = zend_copy_generic_type(args->args[i]);
	}
	for (uint32_t i = args->num_args; i < params->num_params; i++) {
		expanded->args[i] = zend_copy_generic_type(params->params[i].default_type);
	}
	zend_generic_args_compute_masks(expanded);
	return expanded;
}

ZEND_API bool zend_verify_generic_args(
	const zend_generic_params_info *params, const zend_generic_args *args)
{
	if (args->num_args != params->num_params) {
		/* Check if the missing args all have defaults */
		if (args->num_args < params->num_params) {
			bool all_have_defaults = true;
			for (uint32_t i = args->num_args; i < params->num_params; i++) {
				if (!ZEND_TYPE_IS_SET(params->params[i].default_type)) {
					all_have_defaults = false;
					break;
				}
			}
			if (!all_have_defaults) {
				zend_error(E_ERROR,
					"Generic type expects %u type argument(s), %u given",
					params->num_params, args->num_args);
				return 0;
			}
			/* Fewer args but all remaining have defaults — OK, skip validation for defaulted args */
		} else {
			zend_error(E_ERROR,
				"Generic type expects %u type argument(s), %u given",
				params->num_params, args->num_args);
			return 0;
		}
	}

	for (uint32_t i = 0; i < args->num_args; i++) {
		zend_type constraint = params->params[i].constraint;
		if (!ZEND_TYPE_IS_SET(constraint)) {
			continue; /* No constraint — any type is allowed */
		}

		/* Constraint is a class type — the type argument must be a class
		 * that implements/extends the constraint class. */
		if (ZEND_TYPE_HAS_NAME(constraint)) {
			zend_string *constraint_name = ZEND_TYPE_NAME(constraint);
			zend_class_entry *constraint_ce = zend_lookup_class(constraint_name);
			if (!constraint_ce) {
				zend_error(E_ERROR,
					"Generic constraint class \"%s\" not found",
					ZSTR_VAL(constraint_name));
				return 0;
			}

			zend_type arg = args->args[i];
			/* The type arg must be a class type that satisfies the constraint */
			if (ZEND_TYPE_HAS_NAME(arg)) {
				zend_class_entry *arg_ce = zend_lookup_class(ZEND_TYPE_NAME(arg));
				if (!arg_ce || !instanceof_function(arg_ce, constraint_ce)) {
					zend_error(E_ERROR,
						"Generic type argument #%u must implement %s, %s given",
						i + 1, ZSTR_VAL(constraint_name), ZSTR_VAL(ZEND_TYPE_NAME(arg)));
					return 0;
				}
			} else {
				/* Primitive type (int, string, etc.) — cannot satisfy a class constraint */
				zend_string *arg_str = zend_type_to_string(arg);
				zend_error(E_ERROR,
					"Generic type argument #%u must implement %s, %s given",
					i + 1, ZSTR_VAL(constraint_name), ZSTR_VAL(arg_str));
				zend_string_release(arg_str);
				return 0;
			}
		}
	}
	return 1;
}

/* Check if act_ce is a subclass of exp_ce (for covariant checks) */
static bool zend_generic_type_is_subtype(zend_type expected, zend_type actual)
{
	/* Both are class types */
	if (ZEND_TYPE_HAS_NAME(expected) && ZEND_TYPE_HAS_NAME(actual)) {
		zend_class_entry *exp_ce = zend_lookup_class(ZEND_TYPE_NAME(expected));
		zend_class_entry *act_ce = zend_lookup_class(ZEND_TYPE_NAME(actual));
		if (exp_ce && act_ce) {
			return instanceof_function(act_ce, exp_ce);
		}
	}
	return false;
}

ZEND_API bool zend_generic_args_compatible(
	const zend_generic_args *expected, const zend_generic_args *actual,
	const zend_generic_params_info *params_info,
	const zend_generic_bound *wildcard_bounds)
{
	if (expected == NULL && actual == NULL) {
		return 1;
	}
	if (expected == NULL || actual == NULL) {
		return 0;
	}
	if (expected->num_args != actual->num_args) {
		return 0;
	}

	for (uint32_t i = 0; i < expected->num_args; i++) {
		zend_type exp_type = expected->args[i];
		zend_type act_type = actual->args[i];

		/* Check wildcard bounds first */
		zend_generic_bound bound = (wildcard_bounds) ? wildcard_bounds[i] : ZEND_GENERIC_BOUND_NONE;

		if (bound == ZEND_GENERIC_BOUND_UNBOUND) {
			/* Unbounded wildcard (?) — matches any type */
			continue;
		}

		if (bound == ZEND_GENERIC_BOUND_UPPER) {
			/* ? extends Foo — actual must be a subtype of the bound */
			if (ZEND_TYPE_HAS_NAME(exp_type) && ZEND_TYPE_HAS_NAME(act_type)) {
				if (zend_generic_type_is_subtype(exp_type, act_type)) {
					continue;
				}
				/* Also allow exact match */
				if (zend_string_equals_ci(ZEND_TYPE_NAME(exp_type), ZEND_TYPE_NAME(act_type))) {
					continue;
				}
			} else if (ZEND_TYPE_HAS_NAME(exp_type)) {
				/* Bound is a class but actual is a primitive — never a subtype */
				return 0;
			}
			/* For primitive bounds (unlikely but handle gracefully): exact match */
			uint32_t exp_mask = ZEND_TYPE_PURE_MASK(exp_type);
			uint32_t act_mask = ZEND_TYPE_PURE_MASK(act_type);
			if (exp_mask == act_mask && !ZEND_TYPE_HAS_NAME(exp_type)) {
				continue;
			}
			return 0;
		}

		if (bound == ZEND_GENERIC_BOUND_LOWER) {
			/* ? super Foo — actual must be a supertype of the bound */
			if (ZEND_TYPE_HAS_NAME(exp_type) && ZEND_TYPE_HAS_NAME(act_type)) {
				if (zend_generic_type_is_subtype(act_type, exp_type)) {
					continue;
				}
				/* Also allow exact match */
				if (zend_string_equals_ci(ZEND_TYPE_NAME(exp_type), ZEND_TYPE_NAME(act_type))) {
					continue;
				}
			} else if (ZEND_TYPE_HAS_NAME(exp_type)) {
				/* Bound is a class but actual is a primitive — never a supertype */
				return 0;
			}
			/* For primitive bounds: exact match */
			uint32_t exp_mask = ZEND_TYPE_PURE_MASK(exp_type);
			uint32_t act_mask = ZEND_TYPE_PURE_MASK(act_type);
			if (exp_mask == act_mask && !ZEND_TYPE_HAS_NAME(exp_type)) {
				continue;
			}
			return 0;
		}

		/* ZEND_GENERIC_BOUND_NONE — exact match (with variance support) */
		uint32_t exp_mask = ZEND_TYPE_PURE_MASK(exp_type);
		uint32_t act_mask = ZEND_TYPE_PURE_MASK(act_type);

		if (exp_mask == act_mask) {
			/* Same type code — check class names if present */
			if (ZEND_TYPE_HAS_NAME(exp_type) && ZEND_TYPE_HAS_NAME(act_type)) {
				if (zend_string_equals_ci(ZEND_TYPE_NAME(exp_type), ZEND_TYPE_NAME(act_type))) {
					continue; /* exact match */
				}
				/* Names differ — check variance */
				zend_generic_variance variance = (params_info && i < params_info->num_params)
					? params_info->params[i].variance : ZEND_GENERIC_VARIANCE_INVARIANT;

				if (variance == ZEND_GENERIC_VARIANCE_COVARIANT) {
					/* out T: actual must be subtype of expected (e.g., Dog is subtype of Animal) */
					if (zend_generic_type_is_subtype(exp_type, act_type)) {
						continue;
					}
				} else if (variance == ZEND_GENERIC_VARIANCE_CONTRAVARIANT) {
					/* in T: actual must be supertype of expected (e.g., Animal is supertype of Dog) */
					if (zend_generic_type_is_subtype(act_type, exp_type)) {
						continue;
					}
				}
				return 0;
			} else if (ZEND_TYPE_HAS_NAME(exp_type) != ZEND_TYPE_HAS_NAME(act_type)) {
				return 0;
			}
		} else {
			return 0;
		}
	}
	return 1;
}

ZEND_API zend_type zend_infer_type_from_zval(const zval *value)
{
	switch (Z_TYPE_P(value)) {
		case IS_LONG:
			return (zend_type) ZEND_TYPE_INIT_CODE(IS_LONG, 0, 0);
		case IS_DOUBLE:
			return (zend_type) ZEND_TYPE_INIT_CODE(IS_DOUBLE, 0, 0);
		case IS_STRING:
			return (zend_type) ZEND_TYPE_INIT_CODE(IS_STRING, 0, 0);
		case IS_TRUE:
		case IS_FALSE:
			return (zend_type) ZEND_TYPE_INIT_CODE(_IS_BOOL, 0, 0);
		case IS_ARRAY:
			return (zend_type) ZEND_TYPE_INIT_CODE(IS_ARRAY, 0, 0);
		case IS_OBJECT: {
			zend_string *class_name = zend_string_copy(Z_OBJCE_P(value)->name);
			return (zend_type) ZEND_TYPE_INIT_CLASS(class_name, 0, 0);
		}
		case IS_NULL:
			return (zend_type) ZEND_TYPE_INIT_CODE(IS_NULL, 0, 0);
		default:
			return (zend_type) ZEND_TYPE_INIT_NONE(0);
	}
}

ZEND_API bool zend_infer_generic_args_from_constructor(zend_object *obj, zend_execute_data *call)
{
	zend_class_entry *ce = obj->ce;
	if (!ce->generic_params_info || obj->generic_args) {
		return false; /* Not a generic class or already has args */
	}

	zend_function *ctor = ce->constructor;
	if (!ctor) {
		return false;
	}

	uint32_t num_params = ce->generic_params_info->num_params;
	zend_generic_args *inferred = zend_alloc_generic_args(num_params);

	/* Scan constructor parameters for generic type params and infer from actual args */
	uint32_t num_ctor_args = ctor->common.num_args;
	uint32_t num_call_args = ZEND_CALL_NUM_ARGS(call);

	for (uint32_t i = 0; i < num_ctor_args && i < num_call_args; i++) {
		zend_arg_info *arg_info = &ctor->common.arg_info[i];
		if (ZEND_TYPE_IS_GENERIC_PARAM(arg_info->type)) {
			zend_generic_type_ref *gref = ZEND_TYPE_GENERIC_PARAM_REF(arg_info->type);
			if (gref->param_index < num_params && !ZEND_TYPE_IS_SET(inferred->args[gref->param_index])) {
				zval *arg_val = ZEND_CALL_VAR_NUM(call, i);
				zend_type inferred_type = zend_infer_type_from_zval(arg_val);
				if (ZEND_TYPE_IS_SET(inferred_type)) {
					inferred->args[gref->param_index] = inferred_type;
				}
			}
		}
	}

	/* Check that all type params got inferred */
	bool all_inferred = true;
	for (uint32_t i = 0; i < num_params; i++) {
		if (!ZEND_TYPE_IS_SET(inferred->args[i])) {
			all_inferred = false;
			break;
		}
	}

	if (!all_inferred) {
		/* Could not infer all type params — free and leave unbound */
		zend_generic_args_release(inferred);
		return false;
	}

	/* Validate against constraints */
	if (!zend_verify_generic_args(ce->generic_params_info, inferred)) {
		zend_generic_args_release(inferred);
		return false;
	}

	zend_generic_args_compute_masks(inferred);
	obj->generic_args = inferred;
	return true;
}

ZEND_API zend_generic_args *zend_infer_generic_args_from_call(zend_execute_data *ex)
{
	zend_function *func = ex->func;
	if (!func || func->type != ZEND_USER_FUNCTION || !func->op_array.generic_params_info) {
		return NULL;
	}

	zend_generic_params_info *gpi = func->op_array.generic_params_info;
	uint32_t num_params = gpi->num_params;
	zend_generic_args *inferred = zend_alloc_generic_args(num_params);

	uint32_t num_func_args = func->common.num_args;
	uint32_t num_call_args = ZEND_CALL_NUM_ARGS(ex);

	for (uint32_t i = 0; i < num_func_args && i < num_call_args; i++) {
		zend_arg_info *arg_info = &func->common.arg_info[i];
		if (ZEND_TYPE_IS_GENERIC_PARAM(arg_info->type)) {
			zend_generic_type_ref *gref = ZEND_TYPE_GENERIC_PARAM_REF(arg_info->type);
			if (gref->param_index < num_params && !ZEND_TYPE_IS_SET(inferred->args[gref->param_index])) {
				zval *arg_val = ZEND_CALL_VAR_NUM(ex, i);
				zend_type inferred_type = zend_infer_type_from_zval(arg_val);
				if (ZEND_TYPE_IS_SET(inferred_type)) {
					inferred->args[gref->param_index] = inferred_type;
				}
			}
		}
	}

	/* Handle variadic parameter: if it's a generic param, infer from first variadic arg.
	 * The variadic arg_info is at index num_func_args (after regular params).
	 * Extra args (beyond num_func_args) are moved by zend_copy_extra_args() to
	 * EX_VAR_NUM(last_var + T), so we read from there. */
	if (func->common.fn_flags & ZEND_ACC_VARIADIC) {
		zend_arg_info *variadic_info = &func->common.arg_info[num_func_args];
		if (ZEND_TYPE_IS_GENERIC_PARAM(variadic_info->type)) {
			zend_generic_type_ref *gref = ZEND_TYPE_GENERIC_PARAM_REF(variadic_info->type);
			if (gref->param_index < num_params && !ZEND_TYPE_IS_SET(inferred->args[gref->param_index])
					&& num_call_args > num_func_args) {
				/* Extra args were relocated after CVs+TMPs by zend_copy_extra_args() */
				zval *arg_val = ZEND_CALL_VAR_NUM(ex,
					func->op_array.last_var + func->op_array.T);
				zend_type inferred_type = zend_infer_type_from_zval(arg_val);
				if (ZEND_TYPE_IS_SET(inferred_type)) {
					inferred->args[gref->param_index] = inferred_type;
				}
			}
		}
	}

	/* Check that all type params got inferred */
	for (uint32_t i = 0; i < num_params; i++) {
		if (!ZEND_TYPE_IS_SET(inferred->args[i])) {
			zend_generic_args_release(inferred);
			return NULL;
		}
	}

	zend_generic_args_compute_masks(inferred);
	return inferred;
}

ZEND_API zend_generic_args *zend_get_current_generic_args(void)
{
	zend_execute_data *ex = EG(current_execute_data);
	if (!ex) {
		return NULL;
	}
	if (Z_TYPE(ex->This) == IS_OBJECT) {
		zend_object *obj = Z_OBJ(ex->This);
		if (obj->generic_args) {
			return obj->generic_args;
		}
		if (obj->ce->bound_generic_args) {
			return obj->ce->bound_generic_args;
		}
	}
	if (EG(static_generic_args)) {
		return EG(static_generic_args);
	}
	return NULL;
}

ZEND_API zend_string *zend_generic_args_to_string(const zend_generic_args *args)
{
	smart_str buf = {0};
	smart_str_appendc(&buf, '<');
	for (uint32_t i = 0; i < args->num_args; i++) {
		if (i > 0) {
			smart_str_appends(&buf, ", ");
		}
		zend_string *type_str = zend_type_to_string(args->args[i]);
		smart_str_append(&buf, type_str);
		zend_string_release(type_str);
	}
	smart_str_appendc(&buf, '>');
	smart_str_0(&buf);
	return buf.s;
}

static zend_type zend_type_from_string(const char *str, size_t len)
{
	/* Map common type names to zend_type */
	if (len == 3 && memcmp(str, "int", 3) == 0) {
		return (zend_type) ZEND_TYPE_INIT_CODE(IS_LONG, 0, 0);
	} else if (len == 5 && memcmp(str, "float", 5) == 0) {
		return (zend_type) ZEND_TYPE_INIT_CODE(IS_DOUBLE, 0, 0);
	} else if (len == 6 && memcmp(str, "string", 6) == 0) {
		return (zend_type) ZEND_TYPE_INIT_CODE(IS_STRING, 0, 0);
	} else if (len == 4 && memcmp(str, "bool", 4) == 0) {
		return (zend_type) ZEND_TYPE_INIT_CODE(_IS_BOOL, 0, 0);
	} else if (len == 5 && memcmp(str, "array", 5) == 0) {
		return (zend_type) ZEND_TYPE_INIT_CODE(IS_ARRAY, 0, 0);
	} else if (len == 4 && memcmp(str, "null", 4) == 0) {
		return (zend_type) ZEND_TYPE_INIT_CODE(IS_NULL, 0, 0);
	} else if (len == 5 && memcmp(str, "mixed", 5) == 0) {
		return (zend_type) ZEND_TYPE_INIT_CODE(IS_MIXED, 0, 0);
	} else {
		/* Assume it's a class name */
		zend_string *name = zend_string_init(str, len, 0);
		return (zend_type) ZEND_TYPE_INIT_CLASS(name, 0, 0);
	}
}

ZEND_API zend_generic_args *zend_generic_args_from_string(const char *str, size_t len)
{
	/* Parse "<Type1, Type2, ...>" format */
	if (len < 3 || str[0] != '<' || str[len - 1] != '>') {
		return NULL;
	}

	/* Count commas to determine number of args (handling nesting) */
	uint32_t num_args = 1;
	int depth = 0;
	for (size_t i = 1; i < len - 1; i++) {
		if (str[i] == '<') depth++;
		else if (str[i] == '>') depth--;
		else if (str[i] == ',' && depth == 0) num_args++;
	}

	zend_generic_args *args = zend_alloc_generic_args(num_args);

	/* Parse each type arg */
	uint32_t arg_idx = 0;
	const char *arg_start = str + 1;
	depth = 0;
	for (size_t i = 1; i < len - 1; i++) {
		if (str[i] == '<') depth++;
		else if (str[i] == '>') depth--;
		else if ((str[i] == ',' && depth == 0) || i == len - 2) {
			const char *arg_end = (str[i] == ',' || i == len - 2) ? str + i + (i == len - 2 ? 0 : 0) : str + i;
			if (i == len - 2) {
				arg_end = str + i + 1; /* Include last char before '>' */
			}

			/* Trim whitespace */
			while (arg_start < arg_end && *arg_start == ' ') arg_start++;
			const char *trimmed_end = (str[i] == ',') ? str + i : arg_end;
			while (trimmed_end > arg_start && *(trimmed_end - 1) == ' ') trimmed_end--;

			if (trimmed_end > arg_start && arg_idx < num_args) {
				args->args[arg_idx] = zend_type_from_string(arg_start, trimmed_end - arg_start);
				arg_idx++;
			}

			arg_start = str + i + 1;
		}
	}

	zend_generic_args_compute_masks(args);
	return args;
}

ZEND_API zend_string *zend_object_get_class_name_with_generics(const zend_object *obj)
{
	if (obj->generic_args) {
		zend_string *generic_str = zend_generic_args_to_string(obj->generic_args);
		zend_string *result = zend_string_concat2(
			ZSTR_VAL(obj->ce->name), ZSTR_LEN(obj->ce->name),
			ZSTR_VAL(generic_str), ZSTR_LEN(generic_str));
		zend_string_release(generic_str);
		return result;
	}
	if (obj->ce->bound_generic_args) {
		zend_string *generic_str = zend_generic_args_to_string(obj->ce->bound_generic_args);
		zend_string *result = zend_string_concat2(
			ZSTR_VAL(obj->ce->name), ZSTR_LEN(obj->ce->name),
			ZSTR_VAL(generic_str), ZSTR_LEN(generic_str));
		zend_string_release(generic_str);
		return result;
	}
	return zend_string_copy(obj->ce->name);
}
