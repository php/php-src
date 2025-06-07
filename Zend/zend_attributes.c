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
   | Authors: Benjamin Eberlei <kontakt@beberlei.de>                      |
   |          Martin Schröder <m.schroeder2007@gmail.com>                 |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_API.h"
#include "zend_attributes.h"
#include "zend_attributes_arginfo.h"
#include "zend_exceptions.h"
#include "zend_smart_str.h"
#include "zend_class_alias.h"

ZEND_API zend_class_entry *zend_ce_attribute;
ZEND_API zend_class_entry *zend_ce_return_type_will_change_attribute;
ZEND_API zend_class_entry *zend_ce_allow_dynamic_properties;
ZEND_API zend_class_entry *zend_ce_sensitive_parameter;
ZEND_API zend_class_entry *zend_ce_sensitive_parameter_value;
ZEND_API zend_class_entry *zend_ce_override;
ZEND_API zend_class_entry *zend_ce_deprecated;
ZEND_API zend_class_entry *zend_ce_nodiscard;
ZEND_API zend_class_entry *zend_ce_delayed_target_validation;
ZEND_API zend_class_entry *zend_ce_class_alias;

static zend_object_handlers attributes_object_handlers_sensitive_parameter_value;

static HashTable internal_attributes;

// zend_compile.c not interface
zend_string *zend_resolve_class_name_ast(zend_ast *ast);

// Based on zend_compile.c but not part of the interface
void compile_alias_attributes(
	HashTable **attributes, zend_ast *ast
) /* {{{ */ {
	zend_attribute *attr;
	zend_internal_attribute *config;

	zend_ast_list *list = zend_ast_get_list(ast);
	uint32_t j;

	ZEND_ASSERT(ast->kind == ZEND_AST_ARRAY);

	for (uint32_t elem_idx = 0; elem_idx < list->children; elem_idx++) {
		zend_ast *array_elem = list->child[elem_idx];
		ZEND_ASSERT(array_elem->kind == ZEND_AST_ARRAY_ELEM);

		zend_ast *array_content = array_elem->child[0];
		if (array_content->kind != ZEND_AST_NEW) {
			zend_error_noreturn(E_COMPILE_ERROR,
				"Attribute must be declared with `new`");
		}

		if (array_content->child[1] &&
			array_content->child[1]->kind == ZEND_AST_CALLABLE_CONVERT) {
			zend_error_noreturn(E_COMPILE_ERROR,
				"Cannot create Closure as attribute argument");
		}

		zend_string *name = zend_resolve_class_name_ast(array_content->child[0]);
		zend_string *lcname = zend_string_tolower_ex(name, false);
		zend_ast_list *args = array_content->child[1] ? zend_ast_get_list(array_content->child[1]) : NULL;

		config = zend_internal_attribute_get(lcname);
		zend_string_release(lcname);

		uint32_t flags = (CG(active_op_array)->fn_flags & ZEND_ACC_STRICT_TYPES)
			? ZEND_ATTRIBUTE_STRICT_TYPES : 0;
		attr = zend_add_attribute(
			attributes, name, args ? args->children : 0, flags, 0, array_content->lineno);
		zend_string_release(name);

		/* Populate arguments */
		if (!args) {
			continue;
		}
		ZEND_ASSERT(args->kind == ZEND_AST_ARG_LIST);

		bool uses_named_args = 0;
		for (j = 0; j < args->children; j++) {
			zend_ast **arg_ast_ptr = &args->child[j];
			zend_ast *arg_ast = *arg_ast_ptr;

			if (arg_ast->kind == ZEND_AST_UNPACK) {
				zend_error_noreturn(E_COMPILE_ERROR,
					"Cannot use unpacking in attribute argument list");
			}

			if (arg_ast->kind == ZEND_AST_NAMED_ARG) {
				attr->args[j].name = zend_string_copy(zend_ast_get_str(arg_ast->child[0]));
				arg_ast_ptr = &arg_ast->child[1];
				uses_named_args = 1;

				for (uint32_t k = 0; k < j; k++) {
					if (attr->args[k].name &&
							zend_string_equals(attr->args[k].name, attr->args[j].name)) {
						zend_error_noreturn(E_COMPILE_ERROR, "Duplicate named parameter $%s",
							ZSTR_VAL(attr->args[j].name));
					}
				}
			} else if (uses_named_args) {
				zend_error_noreturn(E_COMPILE_ERROR,
					"Cannot use positional argument after named argument");
			}

			zend_const_expr_to_zval(
				&attr->args[j].value, arg_ast_ptr, /* allow_dynamic */ true);
		}
	}

	if (*attributes != NULL) {
		/* Validate attributes in a secondary loop (needed to detect repeated attributes). */
		ZEND_HASH_PACKED_FOREACH_PTR(*attributes, attr) {
			if (attr->offset != 0 || NULL == (config = zend_internal_attribute_get(attr->lcname))) {
				continue;
			}

			uint32_t target = ZEND_ATTRIBUTE_TARGET_CLASS_ALIAS;

			if (!(target & (config->flags & ZEND_ATTRIBUTE_TARGET_ALL))) {
				zend_string *location = zend_get_attribute_target_names(target);
				zend_string *allowed = zend_get_attribute_target_names(config->flags);

				zend_error_noreturn(E_ERROR, "Attribute \"%s\" cannot target %s (allowed targets: %s)",
					ZSTR_VAL(attr->name), ZSTR_VAL(location), ZSTR_VAL(allowed)
				);
			}

			if (!(config->flags & ZEND_ATTRIBUTE_IS_REPEATABLE)) {
				if (zend_is_attribute_repeated(*attributes, attr)) {
					zend_error_noreturn(E_ERROR, "Attribute \"%s\" must not be repeated", ZSTR_VAL(attr->name));
				}
			}

			if (config->validator != NULL) {
				config->validator(attr, target, CG(active_class_entry));
			}
		} ZEND_HASH_FOREACH_END();
	}
}
/* }}} */

uint32_t zend_attribute_attribute_get_flags(zend_attribute *attr, zend_class_entry *scope)
{
	// TODO: More proper signature validation: Too many args, incorrect arg names.
	if (attr->argc > 0) {
		zval flags;

		if (FAILURE == zend_get_attribute_value(&flags, attr, 0, scope)) {
			ZEND_ASSERT(EG(exception));
			return 0;
		}

		if (Z_TYPE(flags) != IS_LONG) {
			zend_throw_error(NULL,
				"Attribute::__construct(): Argument #1 ($flags) must be of type int, %s given",
				zend_zval_value_name(&flags)
			);
			zval_ptr_dtor(&flags);
			return 0;
		}

		uint32_t flags_l = Z_LVAL(flags);
		if (flags_l & ~ZEND_ATTRIBUTE_FLAGS) {
			zend_throw_error(NULL, "Invalid attribute flags specified");
			return 0;
		}

		return flags_l;
	}

	return ZEND_ATTRIBUTE_TARGET_ALL;
}

static zend_execute_data *setup_dummy_call_frame(zend_string *filename, zend_attribute *attribute_data) {
	/* Set up dummy call frame that makes it look like the attribute was invoked
		* from where it occurs in the code. */
	zend_function dummy_func;
	zend_op *opline;

	memset(&dummy_func, 0, sizeof(zend_function));

	zend_execute_data *call = zend_vm_stack_push_call_frame_ex(
		ZEND_MM_ALIGNED_SIZE_EX(sizeof(zend_execute_data), sizeof(zval)) +
		ZEND_MM_ALIGNED_SIZE_EX(sizeof(zend_op), sizeof(zval)) +
		ZEND_MM_ALIGNED_SIZE_EX(sizeof(zend_function), sizeof(zval)),
		0, &dummy_func, 0, NULL);

	opline = (zend_op*)(call + 1);
	memset(opline, 0, sizeof(zend_op));
	opline->opcode = ZEND_DO_FCALL;
	opline->lineno = attribute_data->lineno;

	call->opline = opline;
	call->call = NULL;
	call->return_value = NULL;
	call->func = (zend_function*)(call->opline + 1);
	call->prev_execute_data = EG(current_execute_data);

	memset(call->func, 0, sizeof(zend_function));
	call->func->type = ZEND_USER_FUNCTION;
	call->func->op_array.fn_flags =
		attribute_data->flags & ZEND_ATTRIBUTE_STRICT_TYPES ? ZEND_ACC_STRICT_TYPES : 0;
	call->func->op_array.fn_flags |= ZEND_ACC_CALL_VIA_TRAMPOLINE;
	call->func->op_array.filename = filename;

	return call;
}

static void validate_allow_dynamic_properties(
		zend_attribute *attr, uint32_t target, zend_class_entry *scope)
{
	ZEND_ASSERT(scope != NULL);
	const char *msg = NULL;
	if (scope->ce_flags & ZEND_ACC_TRAIT) {
		msg = "Cannot apply #[\\AllowDynamicProperties] to trait %s";
	} else if (scope->ce_flags & ZEND_ACC_INTERFACE) {
		msg = "Cannot apply #[\\AllowDynamicProperties] to interface %s";
	} else if (scope->ce_flags & ZEND_ACC_READONLY_CLASS) {
		msg = "Cannot apply #[\\AllowDynamicProperties] to readonly class %s";
	} else if (scope->ce_flags & ZEND_ACC_ENUM) {
		msg = "Cannot apply #[\\AllowDynamicProperties] to enum %s";
	}
	if (msg != NULL) {
		return zend_strpprintf(0, msg, ZSTR_VAL(scope->name));
	}
	scope->ce_flags |= ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES;
	return NULL;
}

static zend_string *validate_attribute(
	zend_attribute *attr, uint32_t target, zend_class_entry *scope)
{
	const char *msg = NULL;
	if (scope->ce_flags & ZEND_ACC_TRAIT) {
		msg = "Cannot apply #[\\Attribute] to trait %s";
	} else if (scope->ce_flags & ZEND_ACC_INTERFACE) {
		msg = "Cannot apply #[\\Attribute] to interface %s";
	} else if (scope->ce_flags & ZEND_ACC_ENUM) {
		msg = "Cannot apply #[\\Attribute] to enum %s";
	} else if (scope->ce_flags & ZEND_ACC_EXPLICIT_ABSTRACT_CLASS) {
		msg = "Cannot apply #[\\Attribute] to abstract class %s";
	}
	if (msg != NULL) {
		return zend_strpprintf(0, msg, ZSTR_VAL(scope->name));
	}
	return NULL;
}

static zend_string *validate_deprecated(
	zend_attribute *attr,
	uint32_t target,
	zend_class_entry *scope
) {
	if (target != ZEND_ATTRIBUTE_TARGET_CLASS) {
		/* Being used for a method or something, validation does not apply */
		return NULL;
	}
	if (!(scope->ce_flags & ZEND_ACC_TRAIT)) {
		const char *type = zend_get_object_type_case(scope, false);
		return zend_strpprintf(0, "Cannot apply #[\\Deprecated] to %s %s", type, ZSTR_VAL(scope->name));
	}

	scope->ce_flags |= ZEND_ACC_DEPRECATED;
	return NULL;

}

static void validate_class_alias(
		zend_attribute *attr, uint32_t target, zend_class_entry *scope)
{
	// Do NOT construct the attribute yet, that would require any of the
	// attributes that are used to exist; at this point, access the alias name
	// based on the arguments, and do our own validation
	zend_execute_data *call = setup_dummy_call_frame(scope->info.user.filename, attr);
	EG(current_execute_data) = call;

	zend_execute_data *constructor_call = zend_vm_stack_push_call_frame(
		ZEND_CALL_TOP_FUNCTION | ZEND_CALL_DYNAMIC | ZEND_CALL_HAS_THIS,
		zend_ce_class_alias->constructor,
		attr->argc,
		scope
	);
	constructor_call->prev_execute_data = EG(current_execute_data);
	EG(current_execute_data) = constructor_call;

	if (attr->argc < 1 || attr->argc > 2) {
		zend_wrong_parameters_count_error(1, 2);

		EG(current_execute_data) = constructor_call->prev_execute_data;
		zend_vm_stack_free_call_frame(constructor_call);
		
		EG(current_execute_data) = call->prev_execute_data;
		zend_vm_stack_free_call_frame(call);
		return;
	}

	zval *found = NULL;

	// Looking for either the first parameter, or if the first parameter
	// is named, the 'alias' parameter
	if (attr->args[0].name == NULL) {
		found = &( attr->args[0].value );
	} else {
		for (uint32_t i = 0; i < attr->argc; i++) {
			if (zend_string_equals_literal( attr->args[i].name, "alias")) {
				found = &( attr->args[i].value );
				break;
			}
		}
		if (found == NULL) {
			zend_argument_error(zend_ce_argument_count_error, 1, "not passed");
			EG(current_execute_data) = constructor_call->prev_execute_data;
			zend_vm_stack_free_call_frame(constructor_call);
			EG(current_execute_data) = call->prev_execute_data;
			zend_vm_stack_free_call_frame(call);
			return;
		}
	}

	zend_string *alias;
	if (UNEXPECTED(!zend_parse_arg_str(found, &alias, false, 0))) {
		zend_wrong_parameter_error(
			ZPP_ERROR_WRONG_ARG,
			1,
			"alias",
			Z_EXPECTED_STRING,
			found
		);
		EG(current_execute_data) = constructor_call->prev_execute_data;
		zend_vm_stack_free_call_frame(constructor_call);
		EG(current_execute_data) = call->prev_execute_data;
		zend_vm_stack_free_call_frame(call);
		return;
	}

	zend_result result = zend_register_class_alias_ex(
		ZSTR_VAL(alias),
		ZSTR_LEN(alias),
		scope,
		false
	);
	if (result == FAILURE) {
		zend_error_noreturn(E_ERROR, "Unable to declare alias '%s' for '%s'",
			ZSTR_VAL(alias),
			ZSTR_VAL(scope->name)
		);
		EG(current_execute_data) = constructor_call->prev_execute_data;
		zend_vm_stack_free_call_frame(constructor_call);
		EG(current_execute_data) = call->prev_execute_data;
		zend_vm_stack_free_call_frame(call);
		return;
	}

	zend_string *lc_name;
	if (ZSTR_VAL(alias)[0] == '\\') {
		lc_name = zend_string_alloc(ZSTR_LEN(alias) - 1, 0);
		zend_str_tolower_copy(ZSTR_VAL(lc_name), ZSTR_VAL(alias) + 1, ZSTR_LEN(alias) - 1);
	} else {
		lc_name = zend_string_tolower(alias);
	}

	zval *entry = zend_hash_find(EG(class_table), lc_name);
	zend_string_release_ex(lc_name, /* persistent */ false);
	ZEND_ASSERT(entry != NULL);
	ZEND_ASSERT(Z_TYPE_P(entry) == IS_ALIAS_PTR);

	zend_class_alias *alias_obj = Z_CLASS_ALIAS_P(entry);

	// Compile attributes
	if (attr->argc == 2) {
		zval *nested_attribs = NULL;
		if (attr->args[0].name == NULL && attr->args[1].name == NULL) {
			nested_attribs = &( attr->args[1].value );
		} else {
			for (uint32_t i = 0; i < attr->argc; i++) {
				if ( attr->args[i].name == NULL ) {
					continue;
				}
				if (zend_string_equals_literal( attr->args[i].name, "alias")) {
					continue;
				}
				if (zend_string_equals_literal( attr->args[i].name, "attributes")) {
					nested_attribs = &( attr->args[i].value );
					break;
				}
				zend_throw_error(NULL, "Unknown named parameter $%s", ZSTR_VAL(attr->args[i].name));
				EG(current_execute_data) = constructor_call->prev_execute_data;
				zend_vm_stack_free_call_frame(constructor_call);
				EG(current_execute_data) = call->prev_execute_data;
				zend_vm_stack_free_call_frame(call);
				return;
			}
		}
		ZEND_ASSERT(nested_attribs != NULL);
		if (UNEXPECTED(!Z_OPT_CONSTANT_P(nested_attribs))) {
			zend_wrong_parameter_error(
				ZPP_ERROR_WRONG_ARG,
				2,
				"attributes",
				Z_EXPECTED_ARRAY,
				nested_attribs
			);
			// Something with an invalid parameter
		} else {
			zend_ast *attributes_ast = Z_ASTVAL_P(nested_attribs);
			compile_alias_attributes(&( alias_obj->attributes), attributes_ast);
		}
	}

	// zval_ptr_dtor(alias_name);
	// zval_ptr_dtor(&alias_obj);
	EG(current_execute_data) = constructor_call->prev_execute_data;
	zend_vm_stack_free_call_frame(constructor_call);
	EG(current_execute_data) = call->prev_execute_data;
	zend_vm_stack_free_call_frame(call);
	return;
}

ZEND_METHOD(Attribute, __construct)
{
	zend_long flags = ZEND_ATTRIBUTE_TARGET_ALL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(flags)
	ZEND_PARSE_PARAMETERS_END();

	ZVAL_LONG(OBJ_PROP_NUM(Z_OBJ_P(ZEND_THIS), 0), flags);
}

ZEND_METHOD(ReturnTypeWillChange, __construct)
{
	ZEND_PARSE_PARAMETERS_NONE();
}

ZEND_METHOD(AllowDynamicProperties, __construct)
{
	ZEND_PARSE_PARAMETERS_NONE();
}

ZEND_METHOD(SensitiveParameter, __construct)
{
	ZEND_PARSE_PARAMETERS_NONE();
}

ZEND_METHOD(SensitiveParameterValue, __construct)
{
	zval *value;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	zend_update_property_ex(zend_ce_sensitive_parameter_value, Z_OBJ_P(ZEND_THIS), ZSTR_KNOWN(ZEND_STR_VALUE), value);
}

ZEND_METHOD(SensitiveParameterValue, getValue)
{
	ZEND_PARSE_PARAMETERS_NONE();

	ZVAL_COPY(return_value, OBJ_PROP_NUM(Z_OBJ_P(ZEND_THIS), 0));
}

ZEND_METHOD(SensitiveParameterValue, __debugInfo)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_EMPTY_ARRAY();
}

static HashTable *attributes_sensitive_parameter_value_get_properties_for(zend_object *zobj, zend_prop_purpose purpose)
{
	return NULL;
}

ZEND_METHOD(Override, __construct)
{
	ZEND_PARSE_PARAMETERS_NONE();
}

ZEND_METHOD(Deprecated, __construct)
{
	zend_string *message = NULL;
	zend_string *since = NULL;
	zval value;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(message)
		Z_PARAM_STR_OR_NULL(since)
	ZEND_PARSE_PARAMETERS_END();

	if (message) {
		ZVAL_STR(&value, message);
	} else {
		ZVAL_NULL(&value);
	}
	zend_update_property_ex(zend_ce_deprecated, Z_OBJ_P(ZEND_THIS), ZSTR_KNOWN(ZEND_STR_MESSAGE), &value);

	/* The assignment might fail due to 'readonly'. */
	if (UNEXPECTED(EG(exception))) {
		RETURN_THROWS();
	}

	if (since) {
		ZVAL_STR(&value, since);
	} else {
		ZVAL_NULL(&value);
	}
	zend_update_property_ex(zend_ce_deprecated, Z_OBJ_P(ZEND_THIS), ZSTR_KNOWN(ZEND_STR_SINCE), &value);

	/* The assignment might fail due to 'readonly'. */
	if (UNEXPECTED(EG(exception))) {
		RETURN_THROWS();
	}
}

static zend_string *validate_nodiscard(
	zend_attribute *attr, uint32_t target, zend_class_entry *scope)
{
	ZEND_ASSERT(CG(in_compilation));
	const zend_string *prop_info_name = CG(context).active_property_info_name;
	if (prop_info_name != NULL) {
		// Applied to a hook
		return ZSTR_INIT_LITERAL("#[\\NoDiscard] is not supported for property hooks", 0);
	}
	zend_op_array *op_array = CG(active_op_array);
	op_array->fn_flags |= ZEND_ACC_NODISCARD;
	return NULL;
}

ZEND_METHOD(NoDiscard, __construct)
{
	zend_string *message = NULL;
	zval value;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(message)
	ZEND_PARSE_PARAMETERS_END();

	if (message) {
		ZVAL_STR(&value, message);
	} else {
		ZVAL_NULL(&value);
	}
	zend_update_property_ex(zend_ce_nodiscard, Z_OBJ_P(ZEND_THIS), ZSTR_KNOWN(ZEND_STR_MESSAGE), &value);

	/* The assignment might fail due to 'readonly'. */
	if (UNEXPECTED(EG(exception))) {
		RETURN_THROWS();
	}
}

ZEND_METHOD(ClassAlias, __construct)
{
	zend_string *alias = NULL;
	HashTable *attributes = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(alias)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_HT(attributes)
	ZEND_PARSE_PARAMETERS_END();

	zval value;
	ZVAL_STR(&value, alias);
	zend_update_property(zend_ce_class_alias, Z_OBJ_P(ZEND_THIS), ZEND_STRL("alias"), &value);

	/* The assignment might fail due to 'readonly'. */
	if (UNEXPECTED(EG(exception))) {
		RETURN_THROWS();
	}

	if (attributes == NULL || zend_hash_num_elements(attributes) == 0) {
		return;
	}

	if (!zend_array_is_list(attributes)) {
		zend_throw_error(NULL,
			"ClassAlias::__construct(): Argument #2 ($attributes) must be a list, not an associative array"
		);
		RETURN_THROWS();
	}
}

static zend_attribute *get_attribute(const HashTable *attributes, const zend_string *lcname, uint32_t offset)
{
	if (attributes) {
		zend_attribute *attr;

		ZEND_HASH_PACKED_FOREACH_PTR(attributes, attr) {
			if (attr->offset == offset && zend_string_equals(attr->lcname, lcname)) {
				return attr;
			}
		} ZEND_HASH_FOREACH_END();
	}

	return NULL;
}

static zend_attribute *get_attribute_str(const HashTable *attributes, const char *str, size_t len, uint32_t offset)
{
	if (attributes) {
		zend_attribute *attr;

		ZEND_HASH_PACKED_FOREACH_PTR(attributes, attr) {
			if (attr->offset == offset && zend_string_equals_cstr(attr->lcname, str, len)) {
				return attr;
			}
		} ZEND_HASH_FOREACH_END();
	}

	return NULL;
}

ZEND_API zend_attribute *zend_get_attribute(const HashTable *attributes, const zend_string *lcname)
{
	return get_attribute(attributes, lcname, 0);
}

ZEND_API zend_attribute *zend_get_attribute_str(const HashTable *attributes, const char *str, size_t len)
{
	return get_attribute_str(attributes, str, len, 0);
}

ZEND_API zend_attribute *zend_get_parameter_attribute(const HashTable *attributes, const zend_string *lcname, uint32_t offset)
{
	return get_attribute(attributes, lcname, offset + 1);
}

ZEND_API zend_attribute *zend_get_parameter_attribute_str(const HashTable *attributes, const char *str, size_t len, uint32_t offset)
{
	return get_attribute_str(attributes, str, len, offset + 1);
}

ZEND_API zend_result zend_get_attribute_value(zval *ret, const zend_attribute *attr, uint32_t i, zend_class_entry *scope)
{
	if (i >= attr->argc) {
		return FAILURE;
	}

	ZVAL_COPY_OR_DUP(ret, &attr->args[i].value);

	if (Z_TYPE_P(ret) == IS_CONSTANT_AST) {
		// Delayed validation for attributes in class aliases
		if (CG(in_compilation) && i == 1 && zend_string_equals(attr->name, zend_ce_class_alias->name)) {
			return SUCCESS;
		}
		if (SUCCESS != zval_update_constant_ex(ret, scope)) {
			zval_ptr_dtor(ret);
			return FAILURE;
		}
	}

	return SUCCESS;
}

ZEND_API zend_result zend_get_attribute_object(zval *obj, zend_class_entry *attribute_ce, zend_attribute *attribute_data, zend_class_entry *scope, zend_string *filename)
{
	zend_execute_data *call = NULL;

	if (filename) {
		call = setup_dummy_call_frame(filename, attribute_data);
		EG(current_execute_data) = call;
	}

	zval *args = NULL;
	HashTable *named_params = NULL;

	zend_result result = FAILURE;

	uint32_t argc = 0;
	if (attribute_data->argc) {
		args = emalloc(attribute_data->argc * sizeof(zval));

		for (uint32_t i = 0; i < attribute_data->argc; i++) {
			zval val;
			if (FAILURE == zend_get_attribute_value(&val, attribute_data, i, scope)) {
				result = FAILURE;
				goto out;
			}
			if (attribute_data->args[i].name) {
				if (!named_params) {
					named_params = zend_new_array(0);
				}
				zend_hash_add_new(named_params, attribute_data->args[i].name, &val);
			} else {
				ZVAL_COPY_VALUE(&args[i], &val);
				argc++;
			}
		}
	}

	result = object_init_with_constructor(obj, attribute_ce, argc, args, named_params);

 out:
	for (uint32_t i = 0; i < argc; i++) {
		zval_ptr_dtor(&args[i]);
	}

	efree(args);

	if (named_params) {
		zend_array_destroy(named_params);
	}

	if (filename) {
		EG(current_execute_data) = call->prev_execute_data;
		zend_vm_stack_free_call_frame(call);
	}

	return result;
}

static const char *target_names[] = {
	"class",
	"function",
	"method",
	"property",
	"class constant",
	"parameter",
	"constant",
	"class alias"
};

ZEND_API zend_string *zend_get_attribute_target_names(uint32_t flags)
{
	smart_str str = { 0 };

	for (uint32_t i = 0; i < (sizeof(target_names) / sizeof(char *)); i++) {
		if (flags & (1 << i)) {
			if (smart_str_get_len(&str)) {
				smart_str_appends(&str, ", ");
			}

			smart_str_appends(&str, target_names[i]);
		}
	}

	return smart_str_extract(&str);
}

ZEND_API bool zend_is_attribute_repeated(const HashTable *attributes, const zend_attribute *attr)
{
	zend_attribute *other;

	ZEND_HASH_PACKED_FOREACH_PTR(attributes, other) {
		if (other != attr && other->offset == attr->offset) {
			if (zend_string_equals(other->lcname, attr->lcname)) {
				return 1;
			}
		}
	} ZEND_HASH_FOREACH_END();

	return 0;
}

static void attr_free(zval *v)
{
	zend_attribute *attr = Z_PTR_P(v);
	bool persistent = attr->flags & ZEND_ATTRIBUTE_PERSISTENT;

	zend_string_release(attr->name);
	zend_string_release(attr->lcname);
	if (attr->validation_error != NULL) {
		zend_string_release(attr->validation_error);
	}

	for (uint32_t i = 0; i < attr->argc; i++) {
		if (attr->args[i].name) {
			zend_string_release(attr->args[i].name);
		}
		if (persistent) {
			zval_internal_ptr_dtor(&attr->args[i].value);
		} else {
			zval_ptr_dtor(&attr->args[i].value);
		}
	}

	pefree(attr, persistent);
}

ZEND_API zend_attribute *zend_add_attribute(HashTable **attributes, zend_string *name, uint32_t argc, uint32_t flags, uint32_t offset, uint32_t lineno)
{
	bool persistent = flags & ZEND_ATTRIBUTE_PERSISTENT;
	if (*attributes == NULL) {
		*attributes = pemalloc(sizeof(HashTable), persistent);
		zend_hash_init(*attributes, 8, NULL, attr_free, persistent);
	}

	zend_attribute *attr = pemalloc(ZEND_ATTRIBUTE_SIZE(argc), persistent);

	if (persistent == ((GC_FLAGS(name) & IS_STR_PERSISTENT) != 0)) {
		attr->name = zend_string_copy(name);
	} else {
		attr->name = zend_string_dup(name, persistent);
	}

	attr->lcname = zend_string_tolower_ex(attr->name, persistent);
	attr->validation_error = NULL;
	attr->flags = flags;
	attr->lineno = lineno;
	attr->offset = offset;
	attr->argc = argc;

	/* Initialize arguments to avoid partial initialization in case of fatal errors. */
	for (uint32_t i = 0; i < argc; i++) {
		attr->args[i].name = NULL;
		ZVAL_UNDEF(&attr->args[i].value);
	}

	zend_hash_next_index_insert_ptr(*attributes, attr);

	return attr;
}

static void free_internal_attribute(zval *v)
{
	pefree(Z_PTR_P(v), 1);
}

ZEND_API zend_internal_attribute *zend_mark_internal_attribute(zend_class_entry *ce)
{
	zend_internal_attribute *internal_attr;
	zend_attribute *attr;

	if (ce->type != ZEND_INTERNAL_CLASS) {
		zend_error_noreturn(E_ERROR, "Only internal classes can be registered as compiler attribute");
	}

	ZEND_HASH_FOREACH_PTR(ce->attributes, attr) {
		if (zend_string_equals(attr->name, zend_ce_attribute->name)) {
			internal_attr = pemalloc(sizeof(zend_internal_attribute), 1);
			internal_attr->ce = ce;
			internal_attr->flags = Z_LVAL(attr->args[0].value);
			internal_attr->validator = NULL;

			zend_string *lcname = zend_string_tolower_ex(ce->name, 1);
			zend_hash_update_ptr(&internal_attributes, lcname, internal_attr);
			zend_string_release(lcname);

			return internal_attr;
		}
	} ZEND_HASH_FOREACH_END();

	zend_error_noreturn(E_ERROR, "Classes must be first marked as attribute before being able to be registered as internal attribute class");
}

ZEND_API zend_internal_attribute *zend_internal_attribute_register(zend_class_entry *ce, uint32_t flags)
{
	zend_attribute *attr = zend_add_class_attribute(ce, zend_ce_attribute->name, 1);
	ZVAL_LONG(&attr->args[0].value, flags);

	return zend_mark_internal_attribute(ce);
}

ZEND_API zend_internal_attribute *zend_internal_attribute_get(zend_string *lcname)
{
	return zend_hash_find_ptr(&internal_attributes, lcname);
}

void zend_register_attribute_ce(void)
{
	zend_internal_attribute *attr;

	zend_hash_init(&internal_attributes, 8, NULL, free_internal_attribute, 1);

	zend_ce_attribute = register_class_Attribute();
	attr = zend_mark_internal_attribute(zend_ce_attribute);
	attr->validator = validate_attribute;

	zend_ce_return_type_will_change_attribute = register_class_ReturnTypeWillChange();
	zend_mark_internal_attribute(zend_ce_return_type_will_change_attribute);

	zend_ce_allow_dynamic_properties = register_class_AllowDynamicProperties();
	attr = zend_mark_internal_attribute(zend_ce_allow_dynamic_properties);
	attr->validator = validate_allow_dynamic_properties;

	zend_ce_sensitive_parameter = register_class_SensitiveParameter();
	zend_mark_internal_attribute(zend_ce_sensitive_parameter);

	memcpy(&attributes_object_handlers_sensitive_parameter_value, &std_object_handlers, sizeof(zend_object_handlers));
	attributes_object_handlers_sensitive_parameter_value.get_properties_for = attributes_sensitive_parameter_value_get_properties_for;

	/* This is not an actual attribute, thus the zend_mark_internal_attribute() call is missing. */
	zend_ce_sensitive_parameter_value = register_class_SensitiveParameterValue();
	zend_ce_sensitive_parameter_value->default_object_handlers = &attributes_object_handlers_sensitive_parameter_value;

	zend_ce_override = register_class_Override();
	zend_mark_internal_attribute(zend_ce_override);

	zend_ce_deprecated = register_class_Deprecated();
	attr = zend_mark_internal_attribute(zend_ce_deprecated);
	attr->validator = validate_deprecated;

	zend_ce_nodiscard = register_class_NoDiscard();
	attr = zend_mark_internal_attribute(zend_ce_nodiscard);
	attr->validator = validate_nodiscard;

	zend_ce_delayed_target_validation = register_class_DelayedTargetValidation();
	attr = zend_mark_internal_attribute(zend_ce_delayed_target_validation);

	zend_ce_class_alias = register_class_ClassAlias();
	attr = zend_mark_internal_attribute(zend_ce_class_alias);
	attr->validator = validate_class_alias;
}

void zend_attributes_shutdown(void)
{
	zend_hash_destroy(&internal_attributes);
}
