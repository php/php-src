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

ZEND_API zend_generic_params_info *zend_alloc_generic_params_info(uint32_t num_params)
{
	zend_generic_params_info *info = safe_emalloc(
		num_params - 1, sizeof(zend_generic_param),
		sizeof(zend_generic_params_info));
	info->num_params = num_params;
	for (uint32_t i = 0; i < num_params; i++) {
		info->params[i].name = NULL;
		info->params[i].constraint = (zend_type) ZEND_TYPE_INIT_NONE(0);
		info->params[i].variance = ZEND_GENERIC_VARIANCE_INVARIANT;
	}
	return info;
}

ZEND_API zend_generic_args *zend_alloc_generic_args(uint32_t num_args)
{
	zend_generic_args *args = safe_emalloc(
		num_args - 1, sizeof(zend_type),
		sizeof(zend_generic_args));
	args->num_args = num_args;
	for (uint32_t i = 0; i < num_args; i++) {
		args->args[i] = (zend_type) ZEND_TYPE_INIT_NONE(0);
	}
	return args;
}

ZEND_API zend_type zend_copy_generic_type(zend_type src)
{
	if (ZEND_TYPE_IS_GENERIC_CLASS(src)) {
		/* Deep-copy the generic class ref */
		zend_generic_class_ref *orig = ZEND_TYPE_GENERIC_CLASS_REF(src);
		zend_generic_class_ref *ref = emalloc(sizeof(zend_generic_class_ref));
		ref->class_name = zend_string_copy(orig->class_name);
		ref->type_args = orig->type_args ? zend_copy_generic_args(orig->type_args) : NULL;
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
	zend_generic_args *copy = safe_emalloc(
		num_args - 1, sizeof(zend_type),
		sizeof(zend_generic_args));
	copy->num_args = num_args;
	for (uint32_t i = 0; i < num_args; i++) {
		copy->args[i] = zend_copy_generic_type(src->args[i]);
	}
	return copy;
}

ZEND_API void zend_generic_params_info_dtor(zend_generic_params_info *info)
{
	for (uint32_t i = 0; i < info->num_params; i++) {
		if (info->params[i].name) {
			zend_string_release(info->params[i].name);
		}
		zend_type_release(info->params[i].constraint, /* persistent */ 0);
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
		zend_generic_args_dtor(ref->type_args);
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

ZEND_API bool zend_verify_generic_args(
	const zend_generic_params_info *params, const zend_generic_args *args)
{
	if (args->num_args != params->num_params) {
		zend_error(E_ERROR,
			"Generic type expects %u type argument(s), %u given",
			params->num_params, args->num_args);
		return 0;
	}

	for (uint32_t i = 0; i < params->num_params; i++) {
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
	const zend_generic_params_info *params_info)
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

		/* Compare type masks for simple types */
		uint32_t exp_mask = ZEND_TYPE_PURE_MASK(exp_type);
		uint32_t act_mask = ZEND_TYPE_PURE_MASK(act_type);

		if (exp_mask == act_mask) {
			/* Same type code — check class names if present */
			if (ZEND_TYPE_HAS_NAME(exp_type) && ZEND_TYPE_HAS_NAME(act_type)) {
				if (zend_string_equals_ci(ZEND_TYPE_NAME(exp_type), ZEND_TYPE_NAME(act_type))) {
					continue; /* exact match */
				}
				/* Names differ — check variance */
				uint8_t variance = (params_info && i < params_info->num_params)
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
		zend_generic_args_dtor(inferred);
		return false;
	}

	/* Validate against constraints */
	if (!zend_verify_generic_args(ce->generic_params_info, inferred)) {
		zend_generic_args_dtor(inferred);
		return false;
	}

	obj->generic_args = inferred;
	return true;
}

ZEND_API zend_generic_args *zend_get_current_generic_args(void)
{
	zend_execute_data *ex = EG(current_execute_data);

	if (!ex) {
		return NULL;
	}

	/* For method calls: get from the object instance */
	if (Z_TYPE(ex->This) == IS_OBJECT) {
		zend_object *obj = Z_OBJ(ex->This);
		if (obj->generic_args) {
			return obj->generic_args;
		}
		/* Fall back to class-level bound args (e.g., IntList extends Collection<int>) */
		if (obj->ce->bound_generic_args) {
			return obj->ce->bound_generic_args;
		}
	}

	/* For generic functions: stored in extra named args or similar mechanism */
	/* Phase 1: function-level generics use a simpler approach */

	return NULL;
}
