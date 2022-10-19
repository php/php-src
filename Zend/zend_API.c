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
   |          Andrei Zmievski <andrei@php.net>                            |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_execute.h"
#include "zend_API.h"
#include "zend_modules.h"
#include "zend_extensions.h"
#include "zend_constants.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"
#include "zend_closures.h"
#include "zend_inheritance.h"
#include "zend_ini.h"
#include "zend_enum.h"
#include "zend_observer.h"

#include <stdarg.h>

/* these variables are true statics/globals, and have to be mutex'ed on every access */
ZEND_API HashTable module_registry;

static zend_module_entry **module_request_startup_handlers;
static zend_module_entry **module_request_shutdown_handlers;
static zend_module_entry **module_post_deactivate_handlers;

static zend_class_entry  **class_cleanup_handlers;

ZEND_API zend_result zend_get_parameters_array_ex(uint32_t param_count, zval *argument_array) /* {{{ */
{
	zval *param_ptr;
	uint32_t arg_count;

	param_ptr = ZEND_CALL_ARG(EG(current_execute_data), 1);
	arg_count = ZEND_CALL_NUM_ARGS(EG(current_execute_data));

	if (param_count>arg_count) {
		return FAILURE;
	}

	while (param_count-->0) {
		ZVAL_COPY_VALUE(argument_array, param_ptr);
		argument_array++;
		param_ptr++;
	}

	return SUCCESS;
}
/* }}} */

ZEND_API zend_result zend_copy_parameters_array(uint32_t param_count, zval *argument_array) /* {{{ */
{
	zval *param_ptr;
	uint32_t arg_count;

	param_ptr = ZEND_CALL_ARG(EG(current_execute_data), 1);
	arg_count = ZEND_CALL_NUM_ARGS(EG(current_execute_data));

	if (param_count>arg_count) {
		return FAILURE;
	}

	while (param_count-->0) {
		Z_TRY_ADDREF_P(param_ptr);
		zend_hash_next_index_insert_new(Z_ARRVAL_P(argument_array), param_ptr);
		param_ptr++;
	}

	return SUCCESS;
}
/* }}} */

ZEND_API ZEND_COLD void zend_wrong_param_count(void) /* {{{ */
{
	const char *space;
	const char *class_name = get_active_class_name(&space);

	zend_argument_count_error("Wrong parameter count for %s%s%s()", class_name, space, get_active_function_name());
}
/* }}} */

ZEND_API ZEND_COLD void zend_wrong_property_read(zval *object, zval *property)
{
	zend_string *tmp_property_name;
	zend_string *property_name = zval_get_tmp_string(property, &tmp_property_name);
	zend_error(E_WARNING, "Attempt to read property \"%s\" on %s", ZSTR_VAL(property_name), zend_zval_value_name(object));
	zend_tmp_string_release(tmp_property_name);
}

/* Argument parsing API -- andrei */
ZEND_API const char *zend_get_type_by_const(int type) /* {{{ */
{
	switch(type) {
		case IS_FALSE:
		case IS_TRUE:
		case _IS_BOOL:
			return "bool";
		case IS_LONG:
			return "int";
		case IS_DOUBLE:
			return "float";
		case IS_STRING:
			return "string";
		case IS_OBJECT:
			return "object";
		case IS_RESOURCE:
			return "resource";
		case IS_NULL:
			return "null";
		case IS_CALLABLE:
			return "callable";
		case IS_ITERABLE:
			return "iterable";
		case IS_ARRAY:
			return "array";
		case IS_VOID:
			return "void";
		case IS_MIXED:
			return "mixed";
		case _IS_NUMBER:
			return "int|float";
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}
/* }}} */

ZEND_API const char *zend_zval_value_name(const zval *arg)
{
	ZVAL_DEREF(arg);

	if (Z_ISUNDEF_P(arg)) {
		return "null";
	}

	if (Z_TYPE_P(arg) == IS_OBJECT) {
		return ZSTR_VAL(Z_OBJCE_P(arg)->name);
	} else if (Z_TYPE_P(arg) == IS_FALSE) {
		return "false";
	} else if  (Z_TYPE_P(arg) == IS_TRUE) {
		return "true";
	}

	return zend_get_type_by_const(Z_TYPE_P(arg));
}

ZEND_API const char *zend_zval_type_name(const zval *arg)
{
	ZVAL_DEREF(arg);

	if (Z_ISUNDEF_P(arg)) {
		return "null";
	}

	if (Z_TYPE_P(arg) == IS_OBJECT) {
		return ZSTR_VAL(Z_OBJCE_P(arg)->name);
	}

	return zend_get_type_by_const(Z_TYPE_P(arg));
}

/* This API exists *only* for use in gettype().
 * For anything else, you likely want zend_zval_type_name(). */
ZEND_API zend_string *zend_zval_get_legacy_type(const zval *arg) /* {{{ */
{
	switch (Z_TYPE_P(arg)) {
		case IS_NULL:
			return ZSTR_KNOWN(ZEND_STR_NULL);
		case IS_FALSE:
		case IS_TRUE:
			return ZSTR_KNOWN(ZEND_STR_BOOLEAN);
		case IS_LONG:
			return ZSTR_KNOWN(ZEND_STR_INTEGER);
		case IS_DOUBLE:
			return ZSTR_KNOWN(ZEND_STR_DOUBLE);
		case IS_STRING:
			return ZSTR_KNOWN(ZEND_STR_STRING);
		case IS_ARRAY:
			return ZSTR_KNOWN(ZEND_STR_ARRAY);
		case IS_OBJECT:
			return ZSTR_KNOWN(ZEND_STR_OBJECT);
		case IS_RESOURCE:
			if (zend_rsrc_list_get_rsrc_type(Z_RES_P(arg))) {
				return ZSTR_KNOWN(ZEND_STR_RESOURCE);
			} else {
				return ZSTR_KNOWN(ZEND_STR_CLOSED_RESOURCE);
			}
		default:
			return NULL;
	}
}
/* }}} */

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_parameters_none_error(void) /* {{{ */
{
	int num_args = ZEND_CALL_NUM_ARGS(EG(current_execute_data));
	zend_string *func_name = get_active_function_or_method_name();

	zend_argument_count_error("%s() expects exactly 0 arguments, %d given", ZSTR_VAL(func_name), num_args);

	zend_string_release(func_name);
}
/* }}} */

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_parameters_count_error(uint32_t min_num_args, uint32_t max_num_args) /* {{{ */
{
	uint32_t num_args = ZEND_CALL_NUM_ARGS(EG(current_execute_data));
	zend_string *func_name = get_active_function_or_method_name();

	zend_argument_count_error(
		"%s() expects %s %d argument%s, %d given",
		ZSTR_VAL(func_name),
		min_num_args == max_num_args ? "exactly" : num_args < min_num_args ? "at least" : "at most",
		num_args < min_num_args ? min_num_args : max_num_args,
		(num_args < min_num_args ? min_num_args : max_num_args) == 1 ? "" : "s",
		num_args
	);

	zend_string_release(func_name);
}
/* }}} */

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_parameter_error(int error_code, uint32_t num, char *name, zend_expected_type expected_type, zval *arg) /* {{{ */
{
	switch (error_code) {
		case ZPP_ERROR_WRONG_CALLBACK:
			zend_wrong_callback_error(num, name);
			break;
		case ZPP_ERROR_WRONG_CALLBACK_OR_NULL:
			zend_wrong_callback_or_null_error(num, name);
			break;
		case ZPP_ERROR_WRONG_CLASS:
			zend_wrong_parameter_class_error(num, name, arg);
			break;
		case ZPP_ERROR_WRONG_CLASS_OR_NULL:
			zend_wrong_parameter_class_or_null_error(num, name, arg);
			break;
		case ZPP_ERROR_WRONG_CLASS_OR_STRING:
			zend_wrong_parameter_class_or_string_error(num, name, arg);
			break;
		case ZPP_ERROR_WRONG_CLASS_OR_STRING_OR_NULL:
			zend_wrong_parameter_class_or_string_or_null_error(num, name, arg);
			break;
		case ZPP_ERROR_WRONG_CLASS_OR_LONG:
			zend_wrong_parameter_class_or_long_error(num, name, arg);
			break;
		case ZPP_ERROR_WRONG_CLASS_OR_LONG_OR_NULL:
			zend_wrong_parameter_class_or_long_or_null_error(num, name, arg);
			break;
		case ZPP_ERROR_WRONG_ARG:
			zend_wrong_parameter_type_error(num, expected_type, arg);
			break;
		case ZPP_ERROR_UNEXPECTED_EXTRA_NAMED:
			zend_unexpected_extra_named_error();
			break;
		case ZPP_ERROR_FAILURE:
			ZEND_ASSERT(EG(exception) && "Should have produced an error already");
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}
/* }}} */

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_parameter_type_error(uint32_t num, zend_expected_type expected_type, zval *arg) /* {{{ */
{
	static const char * const expected_error[] = {
		Z_EXPECTED_TYPES(Z_EXPECTED_TYPE_STR)
		NULL
	};

	if (EG(exception)) {
		return;
	}

	if ((expected_type == Z_EXPECTED_PATH || expected_type == Z_EXPECTED_PATH_OR_NULL)
			&& Z_TYPE_P(arg) == IS_STRING) {
		zend_argument_value_error(num, "must not contain any null bytes");
		return;
	}

	zend_argument_type_error(num, "must be %s, %s given", expected_error[expected_type], zend_zval_value_name(arg));
}
/* }}} */

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_parameter_class_error(uint32_t num, const char *name, zval *arg) /* {{{ */
{
	if (EG(exception)) {
		return;
	}

	zend_argument_type_error(num, "must be of type %s, %s given", name, zend_zval_value_name(arg));
}
/* }}} */

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_parameter_class_or_null_error(uint32_t num, const char *name, zval *arg) /* {{{ */
{
	if (EG(exception)) {
		return;
	}

	zend_argument_type_error(num, "must be of type ?%s, %s given", name, zend_zval_value_name(arg));
}
/* }}} */

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_parameter_class_or_long_error(uint32_t num, const char *name, zval *arg) /* {{{ */
{
	if (EG(exception)) {
		return;
	}

	zend_argument_type_error(num, "must be of type %s|int, %s given", name, zend_zval_value_name(arg));
}
/* }}} */

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_parameter_class_or_long_or_null_error(uint32_t num, const char *name, zval *arg) /* {{{ */
{
	if (EG(exception)) {
		return;
	}

	zend_argument_type_error(num, "must be of type %s|int|null, %s given", name, zend_zval_value_name(arg));
}
/* }}} */

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_parameter_class_or_string_error(uint32_t num, const char *name, zval *arg) /* {{{ */
{
	if (EG(exception)) {
		return;
	}

	zend_argument_type_error(num, "must be of type %s|string, %s given", name, zend_zval_value_name(arg));
}
/* }}} */

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_parameter_class_or_string_or_null_error(uint32_t num, const char *name, zval *arg) /* {{{ */
{
	if (EG(exception)) {
		return;
	}

	zend_argument_type_error(num, "must be of type %s|string|null, %s given", name, zend_zval_value_name(arg));
}
/* }}} */

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_callback_error(uint32_t num, char *error) /* {{{ */
{
	if (!EG(exception)) {
		zend_argument_type_error(num, "must be a valid callback, %s", error);
	}
	efree(error);
}
/* }}} */

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_callback_or_null_error(uint32_t num, char *error) /* {{{ */
{
	if (!EG(exception)) {
		zend_argument_type_error(num, "must be a valid callback or null, %s", error);
	}
	efree(error);
}
/* }}} */

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_unexpected_extra_named_error(void)
{
	const char *space;
	const char *class_name = get_active_class_name(&space);
	zend_argument_count_error("%s%s%s() does not accept unknown named parameters",
		class_name, space, get_active_function_name());
}

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_argument_error_variadic(zend_class_entry *error_ce, uint32_t arg_num, const char *format, va_list va) /* {{{ */
{
	zend_string *func_name;
	const char *arg_name;
	char *message = NULL;
	if (EG(exception)) {
		return;
	}

	func_name = get_active_function_or_method_name();
	arg_name = get_active_function_arg_name(arg_num);

	zend_vspprintf(&message, 0, format, va);
	zend_throw_error(error_ce, "%s(): Argument #%d%s%s%s %s",
		ZSTR_VAL(func_name), arg_num,
		arg_name ? " ($" : "", arg_name ? arg_name : "", arg_name ? ")" : "", message
	);
	efree(message);
	zend_string_release(func_name);
}
/* }}} */

ZEND_API ZEND_COLD void zend_argument_error(zend_class_entry *error_ce, uint32_t arg_num, const char *format, ...) /* {{{ */
{
	va_list va;

	va_start(va, format);
	zend_argument_error_variadic(error_ce, arg_num, format, va);
	va_end(va);
}
/* }}} */

ZEND_API ZEND_COLD void zend_argument_type_error(uint32_t arg_num, const char *format, ...) /* {{{ */
{
	va_list va;

	va_start(va, format);
	zend_argument_error_variadic(zend_ce_type_error, arg_num, format, va);
	va_end(va);
}
/* }}} */

ZEND_API ZEND_COLD void zend_argument_value_error(uint32_t arg_num, const char *format, ...) /* {{{ */
{
	va_list va;

	va_start(va, format);
	zend_argument_error_variadic(zend_ce_value_error, arg_num, format, va);
	va_end(va);
}
/* }}} */

ZEND_API bool ZEND_FASTCALL zend_parse_arg_class(zval *arg, zend_class_entry **pce, uint32_t num, bool check_null) /* {{{ */
{
	zend_class_entry *ce_base = *pce;

	if (check_null && Z_TYPE_P(arg) == IS_NULL) {
		*pce = NULL;
		return 1;
	}
	if (!try_convert_to_string(arg)) {
		*pce = NULL;
		return 0;
	}

	*pce = zend_lookup_class(Z_STR_P(arg));
	if (ce_base) {
		if ((!*pce || !instanceof_function(*pce, ce_base))) {
			zend_argument_type_error(num, "must be a class name derived from %s, %s given", ZSTR_VAL(ce_base->name), Z_STRVAL_P(arg));
			*pce = NULL;
			return 0;
		}
	}
	if (!*pce) {
		zend_argument_type_error(num, "must be a valid class name, %s given", Z_STRVAL_P(arg));
		return 0;
	}
	return 1;
}
/* }}} */

static ZEND_COLD bool zend_null_arg_deprecated(const char *fallback_type, uint32_t arg_num) {
	zend_function *func = EG(current_execute_data)->func;
	ZEND_ASSERT(arg_num > 0);
	uint32_t arg_offset = arg_num - 1;
	if (arg_offset >= func->common.num_args) {
		ZEND_ASSERT(func->common.fn_flags & ZEND_ACC_VARIADIC);
		arg_offset = func->common.num_args;
	}

	zend_arg_info *arg_info = &func->common.arg_info[arg_offset];
	zend_string *func_name = get_active_function_or_method_name();
	const char *arg_name = get_active_function_arg_name(arg_num);

	/* If no type is specified in arginfo, use the specified fallback_type determined through
	 * zend_parse_parameters instead. */
	zend_string *type_str = zend_type_to_string(arg_info->type);
	const char *type = type_str ? ZSTR_VAL(type_str) : fallback_type;
	zend_error(E_DEPRECATED,
		"%s(): Passing null to parameter #%" PRIu32 "%s%s%s of type %s is deprecated",
		ZSTR_VAL(func_name), arg_num,
		arg_name ? " ($" : "", arg_name ? arg_name : "", arg_name ? ")" : "",
		type);
	zend_string_release(func_name);
	if (type_str) {
		zend_string_release(type_str);
	}
	return !EG(exception);
}

ZEND_API bool ZEND_FASTCALL zend_parse_arg_bool_weak(const zval *arg, bool *dest, uint32_t arg_num) /* {{{ */
{
	if (EXPECTED(Z_TYPE_P(arg) <= IS_STRING)) {
		if (UNEXPECTED(Z_TYPE_P(arg) == IS_NULL) && !zend_null_arg_deprecated("bool", arg_num)) {
			return 0;
		}
		*dest = zend_is_true(arg);
	} else {
		return 0;
	}
	return 1;
}
/* }}} */

ZEND_API bool ZEND_FASTCALL zend_parse_arg_bool_slow(const zval *arg, bool *dest, uint32_t arg_num) /* {{{ */
{
	if (UNEXPECTED(ZEND_ARG_USES_STRICT_TYPES())) {
		return 0;
	}
	return zend_parse_arg_bool_weak(arg, dest, arg_num);
}
/* }}} */

ZEND_API bool ZEND_FASTCALL zend_parse_arg_long_weak(const zval *arg, zend_long *dest, uint32_t arg_num) /* {{{ */
{
	if (EXPECTED(Z_TYPE_P(arg) == IS_DOUBLE)) {
		if (UNEXPECTED(zend_isnan(Z_DVAL_P(arg)))) {
			return 0;
		}
		if (UNEXPECTED(!ZEND_DOUBLE_FITS_LONG(Z_DVAL_P(arg)))) {
			return 0;
		} else {
			zend_long lval = zend_dval_to_lval(Z_DVAL_P(arg));
			if (UNEXPECTED(!zend_is_long_compatible(Z_DVAL_P(arg), lval))) {
				/* Check arg_num is not (uint32_t)-1, as otherwise its called by
				 * zend_verify_weak_scalar_type_hint_no_sideeffect() */
				if (arg_num != (uint32_t)-1) {
					zend_incompatible_double_to_long_error(Z_DVAL_P(arg));
				}
				if (UNEXPECTED(EG(exception))) {
					return 0;
				}
			}
			*dest = lval;
		}
	} else if (EXPECTED(Z_TYPE_P(arg) == IS_STRING)) {
		double d;
		uint8_t type;

		if (UNEXPECTED((type = is_numeric_str_function(Z_STR_P(arg), dest, &d)) != IS_LONG)) {
			if (EXPECTED(type != 0)) {
				zend_long lval;
				if (UNEXPECTED(zend_isnan(d))) {
					return 0;
				}
				if (UNEXPECTED(!ZEND_DOUBLE_FITS_LONG(d))) {
					return 0;
				}

				lval = zend_dval_to_lval(d);
				/* This only checks for a fractional part as if doesn't fit it already throws a TypeError */
				if (UNEXPECTED(!zend_is_long_compatible(d, lval))) {
					/* Check arg_num is not (uint32_t)-1, as otherwise its called by
					 * zend_verify_weak_scalar_type_hint_no_sideeffect() */
					if (arg_num != (uint32_t)-1) {
						zend_incompatible_string_to_long_error(Z_STR_P(arg));
					}
					if (UNEXPECTED(EG(exception))) {
						return 0;
					}
				}
				*dest = lval;
			} else {
				return 0;
			}
		}
		if (UNEXPECTED(EG(exception))) {
			return 0;
		}
	} else if (EXPECTED(Z_TYPE_P(arg) < IS_TRUE)) {
		if (UNEXPECTED(Z_TYPE_P(arg) == IS_NULL) && !zend_null_arg_deprecated("int", arg_num)) {
			return 0;
		}
		*dest = 0;
	} else if (EXPECTED(Z_TYPE_P(arg) == IS_TRUE)) {
		*dest = 1;
	} else {
		return 0;
	}
	return 1;
}
/* }}} */

ZEND_API bool ZEND_FASTCALL zend_parse_arg_long_slow(const zval *arg, zend_long *dest, uint32_t arg_num) /* {{{ */
{
	if (UNEXPECTED(ZEND_ARG_USES_STRICT_TYPES())) {
		return 0;
	}
	return zend_parse_arg_long_weak(arg, dest, arg_num);
}
/* }}} */

ZEND_API bool ZEND_FASTCALL zend_parse_arg_double_weak(const zval *arg, double *dest, uint32_t arg_num) /* {{{ */
{
	if (EXPECTED(Z_TYPE_P(arg) == IS_LONG)) {
		*dest = (double)Z_LVAL_P(arg);
	} else if (EXPECTED(Z_TYPE_P(arg) == IS_STRING)) {
		zend_long l;
		uint8_t type;

		if (UNEXPECTED((type = is_numeric_str_function(Z_STR_P(arg), &l, dest)) != IS_DOUBLE)) {
			if (EXPECTED(type != 0)) {
				*dest = (double)(l);
			} else {
				return 0;
			}
		}
		if (UNEXPECTED(EG(exception))) {
			return 0;
		}
	} else if (EXPECTED(Z_TYPE_P(arg) < IS_TRUE)) {
		if (UNEXPECTED(Z_TYPE_P(arg) == IS_NULL) && !zend_null_arg_deprecated("float", arg_num)) {
			return 0;
		}
		*dest = 0.0;
	} else if (EXPECTED(Z_TYPE_P(arg) == IS_TRUE)) {
		*dest = 1.0;
	} else {
		return 0;
	}
	return 1;
}
/* }}} */

ZEND_API bool ZEND_FASTCALL zend_parse_arg_double_slow(const zval *arg, double *dest, uint32_t arg_num) /* {{{ */
{
	if (EXPECTED(Z_TYPE_P(arg) == IS_LONG)) {
		/* SSTH Exception: IS_LONG may be accepted instead as IS_DOUBLE */
		*dest = (double)Z_LVAL_P(arg);
	} else if (UNEXPECTED(ZEND_ARG_USES_STRICT_TYPES())) {
		return 0;
	}
	return zend_parse_arg_double_weak(arg, dest, arg_num);
}
/* }}} */

ZEND_API bool ZEND_FASTCALL zend_parse_arg_number_slow(zval *arg, zval **dest, uint32_t arg_num) /* {{{ */
{
	if (UNEXPECTED(ZEND_ARG_USES_STRICT_TYPES())) {
		return 0;
	}
	if (Z_TYPE_P(arg) == IS_STRING) {
		zend_string *str = Z_STR_P(arg);
		zend_long lval;
		double dval;
		uint8_t type = is_numeric_str_function(str, &lval, &dval);
		if (type == IS_LONG) {
			ZVAL_LONG(arg, lval);
		} else if (type == IS_DOUBLE) {
			ZVAL_DOUBLE(arg, dval);
		} else {
			return 0;
		}
		zend_string_release(str);
	} else if (Z_TYPE_P(arg) < IS_TRUE) {
		if (UNEXPECTED(Z_TYPE_P(arg) == IS_NULL) && !zend_null_arg_deprecated("int|float", arg_num)) {
			return 0;
		}
		ZVAL_LONG(arg, 0);
	} else if (Z_TYPE_P(arg) == IS_TRUE) {
		ZVAL_LONG(arg, 1);
	} else {
		return 0;
	}
	*dest = arg;
	return 1;
}
/* }}} */


ZEND_API bool ZEND_FASTCALL zend_parse_arg_number_or_str_slow(zval *arg, zval **dest, uint32_t arg_num) /* {{{ */
{
	if (UNEXPECTED(ZEND_ARG_USES_STRICT_TYPES())) {
		return false;
	}
	if (Z_TYPE_P(arg) < IS_TRUE) {
		if (UNEXPECTED(Z_TYPE_P(arg) == IS_NULL) && !zend_null_arg_deprecated("string|int|float", arg_num)) {
			return false;
		}
		ZVAL_LONG(arg, 0);
	} else if (Z_TYPE_P(arg) == IS_TRUE) {
		ZVAL_LONG(arg, 1);
	} else if (UNEXPECTED(Z_TYPE_P(arg) == IS_OBJECT)) {
		zend_object *zobj = Z_OBJ_P(arg);
		zval obj;
		if (zobj->handlers->cast_object(zobj, &obj, IS_STRING) == SUCCESS) {
			OBJ_RELEASE(zobj);
			ZVAL_COPY_VALUE(arg, &obj);
			*dest = arg;
			return true;
		}
		return false;
	} else {
		return false;
	}
	*dest = arg;
	return true;
}

ZEND_API bool ZEND_FASTCALL zend_parse_arg_str_weak(zval *arg, zend_string **dest, uint32_t arg_num) /* {{{ */
{
	if (EXPECTED(Z_TYPE_P(arg) < IS_STRING)) {
		if (UNEXPECTED(Z_TYPE_P(arg) == IS_NULL) && !zend_null_arg_deprecated("string", arg_num)) {
			return 0;
		}
		convert_to_string(arg);
		*dest = Z_STR_P(arg);
	} else if (UNEXPECTED(Z_TYPE_P(arg) == IS_OBJECT)) {
		zend_object *zobj = Z_OBJ_P(arg);
		zval obj;
		if (zobj->handlers->cast_object(zobj, &obj, IS_STRING) == SUCCESS) {
			OBJ_RELEASE(zobj);
			ZVAL_COPY_VALUE(arg, &obj);
			*dest = Z_STR_P(arg);
			return 1;
		}
		return 0;
	} else {
		return 0;
	}
	return 1;
}
/* }}} */

ZEND_API bool ZEND_FASTCALL zend_parse_arg_str_slow(zval *arg, zend_string **dest, uint32_t arg_num) /* {{{ */
{
	if (UNEXPECTED(ZEND_ARG_USES_STRICT_TYPES())) {
		return 0;
	}
	return zend_parse_arg_str_weak(arg, dest, arg_num);
}
/* }}} */

ZEND_API bool ZEND_FASTCALL zend_parse_arg_str_or_long_slow(zval *arg, zend_string **dest_str, zend_long *dest_long, uint32_t arg_num) /* {{{ */
{
	if (UNEXPECTED(ZEND_ARG_USES_STRICT_TYPES())) {
		return 0;
	}
	if (zend_parse_arg_long_weak(arg, dest_long, arg_num)) {
		*dest_str = NULL;
		return 1;
	} else if (zend_parse_arg_str_weak(arg, dest_str, arg_num)) {
		*dest_long = 0;
		return 1;
	} else {
		return 0;
	}
}
/* }}} */

static const char *zend_parse_arg_impl(zval *arg, va_list *va, const char **spec, char **error, uint32_t arg_num) /* {{{ */
{
	const char *spec_walk = *spec;
	char c = *spec_walk++;
	bool check_null = 0;
	bool separate = 0;
	zval *real_arg = arg;

	/* scan through modifiers */
	ZVAL_DEREF(arg);
	while (1) {
		if (*spec_walk == '/') {
			SEPARATE_ZVAL_NOREF(arg);
			real_arg = arg;
			separate = 1;
		} else if (*spec_walk == '!') {
			check_null = 1;
		} else {
			break;
		}
		spec_walk++;
	}

	switch (c) {
		case 'l':
			{
				zend_long *p = va_arg(*va, zend_long *);
				bool *is_null = NULL;

				if (check_null) {
					is_null = va_arg(*va, bool *);
				}

				if (!zend_parse_arg_long(arg, p, is_null, check_null, arg_num)) {
					return check_null ? "?int" : "int";
				}
			}
			break;

		case 'd':
			{
				double *p = va_arg(*va, double *);
				bool *is_null = NULL;

				if (check_null) {
					is_null = va_arg(*va, bool *);
				}

				if (!zend_parse_arg_double(arg, p, is_null, check_null, arg_num)) {
					return check_null ? "?float" : "float";
				}
			}
			break;

		case 'n':
			{
				zval **p = va_arg(*va, zval **);

				if (!zend_parse_arg_number(arg, p, check_null, arg_num)) {
					return check_null ? "int|float|null" : "int|float";
				}
			}
			break;

		case 's':
			{
				char **p = va_arg(*va, char **);
				size_t *pl = va_arg(*va, size_t *);
				if (!zend_parse_arg_string(arg, p, pl, check_null, arg_num)) {
					return check_null ? "?string" : "string";
				}
			}
			break;

		case 'p':
			{
				char **p = va_arg(*va, char **);
				size_t *pl = va_arg(*va, size_t *);
				if (!zend_parse_arg_path(arg, p, pl, check_null, arg_num)) {
					if (Z_TYPE_P(arg) == IS_STRING) {
						zend_spprintf(error, 0, "must not contain any null bytes");
						return "";
					} else {
						return check_null ? "?string" : "string";
					}
				}
			}
			break;

		case 'P':
			{
				zend_string **str = va_arg(*va, zend_string **);
				if (!zend_parse_arg_path_str(arg, str, check_null, arg_num)) {
					if (Z_TYPE_P(arg) == IS_STRING) {
						zend_spprintf(error, 0, "must not contain any null bytes");
						return "";
					} else {
						return check_null ? "?string" : "string";
					}
				}
			}
			break;

		case 'S':
			{
				zend_string **str = va_arg(*va, zend_string **);
				if (!zend_parse_arg_str(arg, str, check_null, arg_num)) {
					return check_null ? "?string" : "string";
				}
			}
			break;

		case 'b':
			{
				bool *p = va_arg(*va, bool *);
				bool *is_null = NULL;

				if (check_null) {
					is_null = va_arg(*va, bool *);
				}

				if (!zend_parse_arg_bool(arg, p, is_null, check_null, arg_num)) {
					return check_null ? "?bool" : "bool";
				}
			}
			break;

		case 'r':
			{
				zval **p = va_arg(*va, zval **);

				if (!zend_parse_arg_resource(arg, p, check_null)) {
					return check_null ? "resource or null" : "resource";
				}
			}
			break;

		case 'A':
		case 'a':
			{
				zval **p = va_arg(*va, zval **);

				if (!zend_parse_arg_array(arg, p, check_null, c == 'A')) {
					return check_null ? "?array" : "array";
				}
			}
			break;

		case 'H':
		case 'h':
			{
				HashTable **p = va_arg(*va, HashTable **);

				if (!zend_parse_arg_array_ht(arg, p, check_null, c == 'H', separate)) {
					return check_null ? "?array" : "array";
				}
			}
			break;

		case 'o':
			{
				zval **p = va_arg(*va, zval **);

				if (!zend_parse_arg_object(arg, p, NULL, check_null)) {
					return check_null ? "?object" : "object";
				}
			}
			break;

		case 'O':
			{
				zval **p = va_arg(*va, zval **);
				zend_class_entry *ce = va_arg(*va, zend_class_entry *);

				if (!zend_parse_arg_object(arg, p, ce, check_null)) {
					if (ce) {
						if (check_null) {
							zend_spprintf(error, 0, "must be of type ?%s, %s given", ZSTR_VAL(ce->name), zend_zval_value_name(arg));
							return "";
						} else {
							return ZSTR_VAL(ce->name);
						}
					} else {
						return check_null ? "?object" : "object";
					}
				}
			}
			break;

		case 'C':
			{
				zend_class_entry *lookup, **pce = va_arg(*va, zend_class_entry **);
				zend_class_entry *ce_base = *pce;

				if (check_null && Z_TYPE_P(arg) == IS_NULL) {
					*pce = NULL;
					break;
				}
				if (!try_convert_to_string(arg)) {
					*pce = NULL;
					return ""; /* try_convert_to_string() throws an exception */
				}

				if ((lookup = zend_lookup_class(Z_STR_P(arg))) == NULL) {
					*pce = NULL;
				} else {
					*pce = lookup;
				}
				if (ce_base) {
					if ((!*pce || !instanceof_function(*pce, ce_base))) {
						zend_spprintf(error, 0, "must be a class name derived from %s%s, %s given",
							ZSTR_VAL(ce_base->name), check_null ? " or null" : "", Z_STRVAL_P(arg));
						*pce = NULL;
						return "";
					}
				}
				if (!*pce) {
					zend_spprintf(error, 0, "must be a valid class name%s, %s given",
						check_null ? " or null" : "", Z_STRVAL_P(arg));
					return "";
				}
				break;

			}
			break;

		case 'f':
			{
				zend_fcall_info *fci = va_arg(*va, zend_fcall_info *);
				zend_fcall_info_cache *fcc = va_arg(*va, zend_fcall_info_cache *);
				char *is_callable_error = NULL;

				if (check_null && Z_TYPE_P(arg) == IS_NULL) {
					fci->size = 0;
					fcc->function_handler = 0;
					break;
				}

				if (zend_fcall_info_init(arg, 0, fci, fcc, NULL, &is_callable_error) == SUCCESS) {
					ZEND_ASSERT(!is_callable_error);
					/* Release call trampolines: The function may not get called, in which case
					 * the trampoline will leak. Force it to be refetched during
					 * zend_call_function instead. */
					zend_release_fcall_info_cache(fcc);
					break;
				}

				if (is_callable_error) {
					zend_spprintf(error, 0, "must be a valid callback%s, %s", check_null ? " or null" : "", is_callable_error);
					efree(is_callable_error);
					return "";
				} else {
					return check_null ? "a valid callback or null" : "a valid callback";
				}
			}

		case 'z':
			{
				zval **p = va_arg(*va, zval **);

				zend_parse_arg_zval_deref(real_arg, p, check_null);
			}
			break;

		case 'Z': /* replace with 'z' */
		case 'L': /* replace with 'l' */
			ZEND_ASSERT(0 && "ZPP modifier no longer supported");
			ZEND_FALLTHROUGH;
		default:
			return "unknown";
	}

	*spec = spec_walk;

	return NULL;
}
/* }}} */

static zend_result zend_parse_arg(uint32_t arg_num, zval *arg, va_list *va, const char **spec, int flags) /* {{{ */
{
	const char *expected_type = NULL;
	char *error = NULL;

	expected_type = zend_parse_arg_impl(arg, va, spec, &error, arg_num);
	if (expected_type) {
		if (EG(exception)) {
			return FAILURE;
		}
		if (!(flags & ZEND_PARSE_PARAMS_QUIET) && (*expected_type || error)) {
			if (error) {
				if (strcmp(error, "must not contain any null bytes") == 0) {
					zend_argument_value_error(arg_num, "%s", error);
				} else {
					zend_argument_type_error(arg_num, "%s", error);
				}
				efree(error);
			} else {
				zend_argument_type_error(arg_num, "must be of type %s, %s given", expected_type, zend_zval_value_name(arg));
			}
		} else if (error) {
			efree(error);
		}

		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

ZEND_API zend_result zend_parse_parameter(int flags, uint32_t arg_num, zval *arg, const char *spec, ...)
{
	va_list va;
	zend_result ret;

	va_start(va, spec);
	ret = zend_parse_arg(arg_num, arg, &va, &spec, flags);
	va_end(va);

	return ret;
}

static ZEND_COLD void zend_parse_parameters_debug_error(const char *msg) {
	zend_function *active_function = EG(current_execute_data)->func;
	const char *class_name = active_function->common.scope
		? ZSTR_VAL(active_function->common.scope->name) : "";
	zend_error_noreturn(E_CORE_ERROR, "%s%s%s(): %s",
		class_name, class_name[0] ? "::" : "",
		ZSTR_VAL(active_function->common.function_name), msg);
}

static zend_result zend_parse_va_args(uint32_t num_args, const char *type_spec, va_list *va, int flags) /* {{{ */
{
	const  char *spec_walk;
	char c;
	uint32_t i;
	uint32_t min_num_args = 0;
	uint32_t max_num_args = 0;
	uint32_t post_varargs = 0;
	zval *arg;
	bool have_varargs = 0;
	bool have_optional_args = 0;
	zval **varargs = NULL;
	uint32_t *n_varargs = NULL;

	for (spec_walk = type_spec; *spec_walk; spec_walk++) {
		c = *spec_walk;
		switch (c) {
			case 'l': case 'd':
			case 's': case 'b':
			case 'r': case 'a':
			case 'o': case 'O':
			case 'z': case 'Z':
			case 'C': case 'h':
			case 'f': case 'A':
			case 'H': case 'p':
			case 'S': case 'P':
			case 'L': case 'n':
				max_num_args++;
				break;

			case '|':
				min_num_args = max_num_args;
				have_optional_args = 1;
				break;

			case '/':
			case '!':
				/* Pass */
				break;

			case '*':
			case '+':
				if (have_varargs) {
					zend_parse_parameters_debug_error(
						"only one varargs specifier (* or +) is permitted");
					return FAILURE;
				}
				have_varargs = 1;
				/* we expect at least one parameter in varargs */
				if (c == '+') {
					max_num_args++;
				}
				/* mark the beginning of varargs */
				post_varargs = max_num_args;

				if (ZEND_CALL_INFO(EG(current_execute_data)) & ZEND_CALL_HAS_EXTRA_NAMED_PARAMS) {
					zend_unexpected_extra_named_error();
					return FAILURE;
				}
				break;

			default:
				zend_parse_parameters_debug_error("bad type specifier while parsing parameters");
				return FAILURE;
		}
	}

	/* with no optional arguments the minimum number of arguments must be the same as the maximum */
	if (!have_optional_args) {
		min_num_args = max_num_args;
	}

	if (have_varargs) {
		/* calculate how many required args are at the end of the specifier list */
		post_varargs = max_num_args - post_varargs;
		max_num_args = UINT32_MAX;
	}

	if (num_args < min_num_args || num_args > max_num_args) {
		if (!(flags & ZEND_PARSE_PARAMS_QUIET)) {
			zend_string *func_name = get_active_function_or_method_name();

			zend_argument_count_error("%s() expects %s %d argument%s, %d given",
				ZSTR_VAL(func_name),
				min_num_args == max_num_args ? "exactly" : num_args < min_num_args ? "at least" : "at most",
				num_args < min_num_args ? min_num_args : max_num_args,
				(num_args < min_num_args ? min_num_args : max_num_args) == 1 ? "" : "s",
				num_args
			);

			zend_string_release(func_name);
		}
		return FAILURE;
	}

	if (num_args > ZEND_CALL_NUM_ARGS(EG(current_execute_data))) {
		zend_parse_parameters_debug_error("could not obtain parameters for parsing");
		return FAILURE;
	}

	i = 0;
	while (num_args-- > 0) {
		if (*type_spec == '|') {
			type_spec++;
		}

		if (*type_spec == '*' || *type_spec == '+') {
			uint32_t num_varargs = num_args + 1 - post_varargs;

			/* eat up the passed in storage even if it won't be filled in with varargs */
			varargs = va_arg(*va, zval **);
			n_varargs = va_arg(*va, uint32_t *);
			type_spec++;

			if (num_varargs > 0) {
				*n_varargs = num_varargs;
				*varargs = ZEND_CALL_ARG(EG(current_execute_data), i + 1);
				/* adjust how many args we have left and restart loop */
				num_args += 1 - num_varargs;
				i += num_varargs;
				continue;
			} else {
				*varargs = NULL;
				*n_varargs = 0;
			}
		}

		arg = ZEND_CALL_ARG(EG(current_execute_data), i + 1);

		if (zend_parse_arg(i+1, arg, va, &type_spec, flags) == FAILURE) {
			/* clean up varargs array if it was used */
			if (varargs && *varargs) {
				*varargs = NULL;
			}
			return FAILURE;
		}
		i++;
	}

	return SUCCESS;
}
/* }}} */

ZEND_API zend_result zend_parse_parameters_ex(int flags, uint32_t num_args, const char *type_spec, ...) /* {{{ */
{
	va_list va;
	zend_result retval;

	va_start(va, type_spec);
	retval = zend_parse_va_args(num_args, type_spec, &va, flags);
	va_end(va);

	return retval;
}
/* }}} */

ZEND_API zend_result zend_parse_parameters(uint32_t num_args, const char *type_spec, ...) /* {{{ */
{
	va_list va;
	zend_result retval;
	int flags = 0;

	va_start(va, type_spec);
	retval = zend_parse_va_args(num_args, type_spec, &va, flags);
	va_end(va);

	return retval;
}
/* }}} */

ZEND_API zend_result zend_parse_method_parameters(uint32_t num_args, zval *this_ptr, const char *type_spec, ...) /* {{{ */
{
	va_list va;
	zend_result retval;
	int flags = 0;
	const char *p = type_spec;
	zval **object;
	zend_class_entry *ce;

	/* Just checking this_ptr is not enough, because fcall_common_helper does not set
	 * Z_OBJ(EG(This)) to NULL when calling an internal function with common.scope == NULL.
	 * In that case EG(This) would still be the $this from the calling code and we'd take the
	 * wrong branch here. */
	bool is_method = EG(current_execute_data)->func->common.scope != NULL;

	if (!is_method || !this_ptr || Z_TYPE_P(this_ptr) != IS_OBJECT) {
		va_start(va, type_spec);
		retval = zend_parse_va_args(num_args, type_spec, &va, flags);
		va_end(va);
	} else {
		p++;

		va_start(va, type_spec);

		object = va_arg(va, zval **);
		ce = va_arg(va, zend_class_entry *);
		*object = this_ptr;

		if (ce && !instanceof_function(Z_OBJCE_P(this_ptr), ce)) {
			zend_error_noreturn(E_CORE_ERROR, "%s::%s() must be derived from %s::%s()",
				ZSTR_VAL(Z_OBJCE_P(this_ptr)->name), get_active_function_name(), ZSTR_VAL(ce->name), get_active_function_name());
		}

		retval = zend_parse_va_args(num_args, p, &va, flags);
		va_end(va);
	}
	return retval;
}
/* }}} */

ZEND_API zend_result zend_parse_method_parameters_ex(int flags, uint32_t num_args, zval *this_ptr, const char *type_spec, ...) /* {{{ */
{
	va_list va;
	zend_result retval;
	const char *p = type_spec;
	zval **object;
	zend_class_entry *ce;

	if (!this_ptr) {
		va_start(va, type_spec);
		retval = zend_parse_va_args(num_args, type_spec, &va, flags);
		va_end(va);
	} else {
		p++;
		va_start(va, type_spec);

		object = va_arg(va, zval **);
		ce = va_arg(va, zend_class_entry *);
		*object = this_ptr;

		if (ce && !instanceof_function(Z_OBJCE_P(this_ptr), ce)) {
			if (!(flags & ZEND_PARSE_PARAMS_QUIET)) {
				zend_error_noreturn(E_CORE_ERROR, "%s::%s() must be derived from %s::%s()",
					ZSTR_VAL(ce->name), get_active_function_name(), ZSTR_VAL(Z_OBJCE_P(this_ptr)->name), get_active_function_name());
			}
			va_end(va);
			return FAILURE;
		}

		retval = zend_parse_va_args(num_args, p, &va, flags);
		va_end(va);
	}
	return retval;
}
/* }}} */

/* This function should be called after the constructor has been called
 * because it may call __set from the uninitialized object otherwise. */
ZEND_API void zend_merge_properties(zval *obj, HashTable *properties) /* {{{ */
{
	zend_object *zobj = Z_OBJ_P(obj);
	zend_object_write_property_t write_property = zobj->handlers->write_property;
	zend_class_entry *old_scope = EG(fake_scope);
	zend_string *key;
	zval *value;

	if (HT_IS_PACKED(properties)) {
		return;
	}
	EG(fake_scope) = Z_OBJCE_P(obj);
	ZEND_HASH_MAP_FOREACH_STR_KEY_VAL(properties, key, value) {
		if (key) {
			write_property(zobj, key, value, NULL);
		}
	} ZEND_HASH_FOREACH_END();
	EG(fake_scope) = old_scope;
}
/* }}} */

static zend_class_mutable_data *zend_allocate_mutable_data(zend_class_entry *class_type) /* {{{ */
{
	zend_class_mutable_data *mutable_data;

	ZEND_ASSERT(ZEND_MAP_PTR(class_type->mutable_data) != NULL);
	ZEND_ASSERT(ZEND_MAP_PTR_GET_IMM(class_type->mutable_data) == NULL);

	mutable_data = zend_arena_alloc(&CG(arena), sizeof(zend_class_mutable_data));
	memset(mutable_data, 0, sizeof(zend_class_mutable_data));
	mutable_data->ce_flags = class_type->ce_flags;
	ZEND_MAP_PTR_SET_IMM(class_type->mutable_data, mutable_data);

	return mutable_data;
}
/* }}} */

ZEND_API HashTable *zend_separate_class_constants_table(zend_class_entry *class_type) /* {{{ */
{
	zend_class_mutable_data *mutable_data;
	HashTable *constants_table;
	zend_string *key;
	zend_class_constant *new_c, *c;

	constants_table = zend_arena_alloc(&CG(arena), sizeof(HashTable));
	zend_hash_init(constants_table, zend_hash_num_elements(&class_type->constants_table), NULL, NULL, 0);
	zend_hash_extend(constants_table, zend_hash_num_elements(&class_type->constants_table), 0);

	ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&class_type->constants_table, key, c) {
		if (c->ce == class_type) {
			if (Z_TYPE(c->value) == IS_CONSTANT_AST) {
				new_c = zend_arena_alloc(&CG(arena), sizeof(zend_class_constant));
				memcpy(new_c, c, sizeof(zend_class_constant));
				c = new_c;
			}
			Z_TRY_ADDREF(c->value);
		} else {
			if (Z_TYPE(c->value) == IS_CONSTANT_AST) {
				c = zend_hash_find_ptr(CE_CONSTANTS_TABLE(c->ce), key);
				ZEND_ASSERT(c);
			}
		}
		_zend_hash_append_ptr(constants_table, key, c);
	} ZEND_HASH_FOREACH_END();

	ZEND_ASSERT(ZEND_MAP_PTR(class_type->mutable_data) != NULL);

	mutable_data = ZEND_MAP_PTR_GET_IMM(class_type->mutable_data);
	if (!mutable_data) {
		mutable_data = zend_allocate_mutable_data(class_type);
	}

	mutable_data->constants_table = constants_table;

	return constants_table;
}

static zend_result update_property(zval *val, zend_property_info *prop_info) {
	if (ZEND_TYPE_IS_SET(prop_info->type)) {
		zval tmp;

		ZVAL_COPY(&tmp, val);
		if (UNEXPECTED(zval_update_constant_ex(&tmp, prop_info->ce) != SUCCESS)) {
			zval_ptr_dtor(&tmp);
			return FAILURE;
		}
		/* property initializers must always be evaluated with strict types */;
		if (UNEXPECTED(!zend_verify_property_type(prop_info, &tmp, /* strict */ 1))) {
			zval_ptr_dtor(&tmp);
			return FAILURE;
		}
		zval_ptr_dtor(val);
		ZVAL_COPY_VALUE(val, &tmp);
		return SUCCESS;
	}
	return zval_update_constant_ex(val, prop_info->ce);
}

ZEND_API zend_result zend_update_class_constant(zend_class_constant *c, const zend_string *name, zend_class_entry *scope)
{
	ZEND_ASSERT(Z_TYPE(c->value) == IS_CONSTANT_AST);

	if (EXPECTED(!ZEND_TYPE_IS_SET(c->type) || ZEND_TYPE_PURE_MASK(c->type) == MAY_BE_ANY)) {
		return zval_update_constant_ex(&c->value, scope);
	}

	zval tmp;

	ZVAL_COPY(&tmp, &c->value);
	zend_result result = zval_update_constant_ex(&tmp, scope);
	if (result == FAILURE) {
		zval_ptr_dtor(&tmp);
		return FAILURE;
	}

	if (UNEXPECTED(!zend_verify_class_constant_type(c, name, &tmp))) {
		zval_ptr_dtor(&tmp);
		return FAILURE;
	}

	zval_ptr_dtor(&c->value);
	ZVAL_COPY_VALUE(&c->value, &tmp);

	return SUCCESS;
}

ZEND_API zend_result zend_update_class_constants(zend_class_entry *class_type) /* {{{ */
{
	zend_class_mutable_data *mutable_data = NULL;
	zval *default_properties_table = NULL;
	zval *static_members_table = NULL;
	zend_class_constant *c;
	zval *val;
	uint32_t ce_flags;

	ce_flags = class_type->ce_flags;

	if (ce_flags & ZEND_ACC_CONSTANTS_UPDATED) {
		return SUCCESS;
	}

	bool uses_mutable_data = ZEND_MAP_PTR(class_type->mutable_data) != NULL;
	if (uses_mutable_data) {
		mutable_data = ZEND_MAP_PTR_GET_IMM(class_type->mutable_data);
		if (mutable_data) {
			ce_flags = mutable_data->ce_flags;
			if (ce_flags & ZEND_ACC_CONSTANTS_UPDATED) {
				return SUCCESS;
			}
		} else {
			mutable_data = zend_allocate_mutable_data(class_type);
		}
	}

	if (class_type->parent) {
		if (UNEXPECTED(zend_update_class_constants(class_type->parent) != SUCCESS)) {
			return FAILURE;
		}
	}

	if (ce_flags & ZEND_ACC_HAS_AST_CONSTANTS) {
		HashTable *constants_table;

		if (uses_mutable_data) {
			constants_table = mutable_data->constants_table;
			if (!constants_table) {
				constants_table = zend_separate_class_constants_table(class_type);
			}
		} else {
			constants_table = &class_type->constants_table;
		}

		zend_string *name;
		ZEND_HASH_MAP_FOREACH_STR_KEY_VAL(constants_table, name, val) {
			c = Z_PTR_P(val);
			if (Z_TYPE(c->value) == IS_CONSTANT_AST) {
				if (c->ce != class_type) {
					Z_PTR_P(val) = c = zend_hash_find_ptr(CE_CONSTANTS_TABLE(c->ce), name);
					if (Z_TYPE(c->value) != IS_CONSTANT_AST) {
						continue;
					}
				}

				val = &c->value;
				if (UNEXPECTED(zend_update_class_constant(c, name, c->ce) != SUCCESS)) {
					return FAILURE;
				}
			}
		} ZEND_HASH_FOREACH_END();
	}

	if (class_type->default_static_members_count) {
		static_members_table = CE_STATIC_MEMBERS(class_type);
		if (!static_members_table) {
			zend_class_init_statics(class_type);
			static_members_table = CE_STATIC_MEMBERS(class_type);
		}
	}

	default_properties_table = class_type->default_properties_table;
	if (uses_mutable_data && (ce_flags & ZEND_ACC_HAS_AST_PROPERTIES)) {
		zval *src, *dst, *end;

		default_properties_table = mutable_data->default_properties_table;
		if (!default_properties_table) {
			default_properties_table = zend_arena_alloc(&CG(arena), sizeof(zval) * class_type->default_properties_count);
			src = class_type->default_properties_table;
			dst = default_properties_table;
			end = dst + class_type->default_properties_count;
			do {
				ZVAL_COPY_PROP(dst, src);
				src++;
				dst++;
			} while (dst != end);
			mutable_data->default_properties_table = default_properties_table;
		}
	}

	if (ce_flags & (ZEND_ACC_HAS_AST_PROPERTIES|ZEND_ACC_HAS_AST_STATICS)) {
		zend_property_info *prop_info;

		/* Use the default properties table to also update initializers of private properties
		 * that have been shadowed in a child class. */
		for (uint32_t i = 0; i < class_type->default_properties_count; i++) {
			val = &default_properties_table[i];
			prop_info = class_type->properties_info_table[i];
			if (Z_TYPE_P(val) == IS_CONSTANT_AST
					&& UNEXPECTED(update_property(val, prop_info) != SUCCESS)) {
				return FAILURE;
			}
		}

		if (class_type->default_static_members_count) {
			ZEND_HASH_MAP_FOREACH_PTR(&class_type->properties_info, prop_info) {
				if (prop_info->flags & ZEND_ACC_STATIC) {
					val = static_members_table + prop_info->offset;
					if (Z_TYPE_P(val) == IS_CONSTANT_AST
							&& UNEXPECTED(update_property(val, prop_info) != SUCCESS)) {
						return FAILURE;
					}
				}
			} ZEND_HASH_FOREACH_END();
		}
	}

	if (class_type->type == ZEND_USER_CLASS && class_type->ce_flags & ZEND_ACC_ENUM && class_type->enum_backing_type != IS_UNDEF) {
		if (zend_enum_build_backed_enum_table(class_type) == FAILURE) {
			return FAILURE;
		}
	}

	ce_flags |= ZEND_ACC_CONSTANTS_UPDATED;
	ce_flags &= ~ZEND_ACC_HAS_AST_CONSTANTS;
	ce_flags &= ~ZEND_ACC_HAS_AST_PROPERTIES;
	ce_flags &= ~ZEND_ACC_HAS_AST_STATICS;
	if (uses_mutable_data) {
		mutable_data->ce_flags = ce_flags;
	} else {
		class_type->ce_flags = ce_flags;
	}

	return SUCCESS;
}
/* }}} */

static zend_always_inline void _object_properties_init(zend_object *object, zend_class_entry *class_type) /* {{{ */
{
	if (class_type->default_properties_count) {
		zval *src = CE_DEFAULT_PROPERTIES_TABLE(class_type);
		zval *dst = object->properties_table;
		zval *end = src + class_type->default_properties_count;

		if (UNEXPECTED(class_type->type == ZEND_INTERNAL_CLASS)) {
			do {
				ZVAL_COPY_OR_DUP_PROP(dst, src);
				src++;
				dst++;
			} while (src != end);
		} else {
			do {
				ZVAL_COPY_PROP(dst, src);
				src++;
				dst++;
			} while (src != end);
		}
	}
}
/* }}} */

ZEND_API void object_properties_init(zend_object *object, zend_class_entry *class_type) /* {{{ */
{
	object->properties = NULL;
	_object_properties_init(object, class_type);
}
/* }}} */

ZEND_API void object_properties_init_ex(zend_object *object, HashTable *properties) /* {{{ */
{
	object->properties = properties;
	if (object->ce->default_properties_count) {
		zval *prop;
		zend_string *key;
		zend_property_info *property_info;

		ZEND_HASH_MAP_FOREACH_STR_KEY_VAL(properties, key, prop) {
			property_info = zend_get_property_info(object->ce, key, 1);
			if (property_info != ZEND_WRONG_PROPERTY_INFO &&
			    property_info &&
			    (property_info->flags & ZEND_ACC_STATIC) == 0) {
				zval *slot = OBJ_PROP(object, property_info->offset);

				if (UNEXPECTED(ZEND_TYPE_IS_SET(property_info->type))) {
					zval tmp;

					ZVAL_COPY_VALUE(&tmp, prop);
					if (UNEXPECTED(!zend_verify_property_type(property_info, &tmp, 0))) {
						continue;
					}
					ZVAL_COPY_VALUE(slot, &tmp);
				} else {
					ZVAL_COPY_VALUE(slot, prop);
				}
				ZVAL_INDIRECT(prop, slot);
			}
		} ZEND_HASH_FOREACH_END();
	}
}
/* }}} */

ZEND_API void object_properties_load(zend_object *object, HashTable *properties) /* {{{ */
{
	zval *prop, tmp;
	zend_string *key;
	zend_long h;
	zend_property_info *property_info;

	ZEND_HASH_FOREACH_KEY_VAL(properties, h, key, prop) {
		if (key) {
			if (ZSTR_VAL(key)[0] == '\0') {
				const char *class_name, *prop_name;
				size_t prop_name_len;
				if (zend_unmangle_property_name_ex(key, &class_name, &prop_name, &prop_name_len) == SUCCESS) {
					zend_string *pname = zend_string_init(prop_name, prop_name_len, 0);
					zend_class_entry *prev_scope = EG(fake_scope);
					if (class_name && class_name[0] != '*') {
						zend_string *cname = zend_string_init(class_name, strlen(class_name), 0);
						EG(fake_scope) = zend_lookup_class(cname);
						zend_string_release_ex(cname, 0);
					}
					property_info = zend_get_property_info(object->ce, pname, 1);
					zend_string_release_ex(pname, 0);
					EG(fake_scope) = prev_scope;
				} else {
					property_info = ZEND_WRONG_PROPERTY_INFO;
				}
			} else {
				property_info = zend_get_property_info(object->ce, key, 1);
			}
			if (property_info != ZEND_WRONG_PROPERTY_INFO &&
				property_info &&
				(property_info->flags & ZEND_ACC_STATIC) == 0) {
				zval *slot = OBJ_PROP(object, property_info->offset);
				zval_ptr_dtor(slot);
				ZVAL_COPY_VALUE(slot, prop);
				zval_add_ref(slot);
				if (object->properties) {
					ZVAL_INDIRECT(&tmp, slot);
					zend_hash_update(object->properties, key, &tmp);
				}
			} else {
				if (UNEXPECTED(object->ce->ce_flags & ZEND_ACC_NO_DYNAMIC_PROPERTIES)) {
					zend_throw_error(NULL, "Cannot create dynamic property %s::$%s",
						ZSTR_VAL(object->ce->name), property_info != ZEND_WRONG_PROPERTY_INFO ? zend_get_unmangled_property_name(key): "");
					return;
				} else if (!(object->ce->ce_flags & ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES)) {
					zend_error(E_DEPRECATED, "Creation of dynamic property %s::$%s is deprecated",
						ZSTR_VAL(object->ce->name), property_info != ZEND_WRONG_PROPERTY_INFO ? zend_get_unmangled_property_name(key): "");
				}

				if (!object->properties) {
					rebuild_object_properties(object);
				}
				prop = zend_hash_update(object->properties, key, prop);
				zval_add_ref(prop);
			}
		} else {
			if (UNEXPECTED(object->ce->ce_flags & ZEND_ACC_NO_DYNAMIC_PROPERTIES)) {
				zend_throw_error(NULL, "Cannot create dynamic property %s::$" ZEND_LONG_FMT, ZSTR_VAL(object->ce->name), h);
				return;
			} else if (!(object->ce->ce_flags & ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES)) {
				zend_error(E_DEPRECATED, "Creation of dynamic property %s::$" ZEND_LONG_FMT " is deprecated",
					ZSTR_VAL(object->ce->name), h);
			}

			if (!object->properties) {
				rebuild_object_properties(object);
			}
			prop = zend_hash_index_update(object->properties, h, prop);
			zval_add_ref(prop);
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* This function requires 'properties' to contain all props declared in the
 * class and all props being public. If only a subset is given or the class
 * has protected members then you need to merge the properties separately by
 * calling zend_merge_properties(). */
static zend_always_inline zend_result _object_and_properties_init(zval *arg, zend_class_entry *class_type, HashTable *properties) /* {{{ */
{
	if (UNEXPECTED(class_type->ce_flags & (ZEND_ACC_INTERFACE|ZEND_ACC_TRAIT|ZEND_ACC_IMPLICIT_ABSTRACT_CLASS|ZEND_ACC_EXPLICIT_ABSTRACT_CLASS|ZEND_ACC_ENUM))) {
		if (class_type->ce_flags & ZEND_ACC_INTERFACE) {
			zend_throw_error(NULL, "Cannot instantiate interface %s", ZSTR_VAL(class_type->name));
		} else if (class_type->ce_flags & ZEND_ACC_TRAIT) {
			zend_throw_error(NULL, "Cannot instantiate trait %s", ZSTR_VAL(class_type->name));
		} else if (class_type->ce_flags & ZEND_ACC_ENUM) {
			zend_throw_error(NULL, "Cannot instantiate enum %s", ZSTR_VAL(class_type->name));
		} else {
			zend_throw_error(NULL, "Cannot instantiate abstract class %s", ZSTR_VAL(class_type->name));
		}
		ZVAL_NULL(arg);
		Z_OBJ_P(arg) = NULL;
		return FAILURE;
	}

	if (UNEXPECTED(!(class_type->ce_flags & ZEND_ACC_CONSTANTS_UPDATED))) {
		if (UNEXPECTED(zend_update_class_constants(class_type) != SUCCESS)) {
			ZVAL_NULL(arg);
			Z_OBJ_P(arg) = NULL;
			return FAILURE;
		}
	}

	if (class_type->create_object == NULL) {
		zend_object *obj = zend_objects_new(class_type);

		ZVAL_OBJ(arg, obj);
		if (properties) {
			object_properties_init_ex(obj, properties);
		} else {
			_object_properties_init(obj, class_type);
		}
	} else {
		ZVAL_OBJ(arg, class_type->create_object(class_type));
	}
	return SUCCESS;
}
/* }}} */

ZEND_API zend_result object_and_properties_init(zval *arg, zend_class_entry *class_type, HashTable *properties) /* {{{ */
{
	return _object_and_properties_init(arg, class_type, properties);
}
/* }}} */

ZEND_API zend_result object_init_ex(zval *arg, zend_class_entry *class_type) /* {{{ */
{
	return _object_and_properties_init(arg, class_type, NULL);
}
/* }}} */

ZEND_API void object_init(zval *arg) /* {{{ */
{
	ZVAL_OBJ(arg, zend_objects_new(zend_standard_class_def));
}
/* }}} */

ZEND_API void add_assoc_long_ex(zval *arg, const char *key, size_t key_len, zend_long n) /* {{{ */
{
	zval tmp;

	ZVAL_LONG(&tmp, n);
	zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, &tmp);
}
/* }}} */

ZEND_API void add_assoc_null_ex(zval *arg, const char *key, size_t key_len) /* {{{ */
{
	zval tmp;

	ZVAL_NULL(&tmp);
	zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, &tmp);
}
/* }}} */

ZEND_API void add_assoc_bool_ex(zval *arg, const char *key, size_t key_len, bool b) /* {{{ */
{
	zval tmp;

	ZVAL_BOOL(&tmp, b);
	zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, &tmp);
}
/* }}} */

ZEND_API void add_assoc_resource_ex(zval *arg, const char *key, size_t key_len, zend_resource *r) /* {{{ */
{
	zval tmp;

	ZVAL_RES(&tmp, r);
	zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, &tmp);
}
/* }}} */

ZEND_API void add_assoc_double_ex(zval *arg, const char *key, size_t key_len, double d) /* {{{ */
{
	zval tmp;

	ZVAL_DOUBLE(&tmp, d);
	zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, &tmp);
}
/* }}} */

ZEND_API void add_assoc_str_ex(zval *arg, const char *key, size_t key_len, zend_string *str) /* {{{ */
{
	zval tmp;

	ZVAL_STR(&tmp, str);
	zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, &tmp);
}
/* }}} */

ZEND_API void add_assoc_string_ex(zval *arg, const char *key, size_t key_len, const char *str) /* {{{ */
{
	zval tmp;

	ZVAL_STRING(&tmp, str);
	zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, &tmp);
}
/* }}} */

ZEND_API void add_assoc_stringl_ex(zval *arg, const char *key, size_t key_len, const char *str, size_t length) /* {{{ */
{
	zval tmp;

	ZVAL_STRINGL(&tmp, str, length);
	zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, &tmp);
}
/* }}} */

ZEND_API void add_assoc_array_ex(zval *arg, const char *key, size_t key_len, zend_array *arr) /* {{{ */
{
	zval tmp;

	ZVAL_ARR(&tmp, arr);
	zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, &tmp);
}
/* }}} */

ZEND_API void add_assoc_object_ex(zval *arg, const char *key, size_t key_len, zend_object *obj) /* {{{ */
{
	zval tmp;

	ZVAL_OBJ(&tmp, obj);
	zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, &tmp);
}
/* }}} */

ZEND_API void add_assoc_reference_ex(zval *arg, const char *key, size_t key_len, zend_reference *ref) /* {{{ */
{
	zval tmp;

	ZVAL_REF(&tmp, ref);
	zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, &tmp);
}
/* }}} */

ZEND_API void add_assoc_zval_ex(zval *arg, const char *key, size_t key_len, zval *value) /* {{{ */
{
	zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, value);
}
/* }}} */

ZEND_API void add_index_long(zval *arg, zend_ulong index, zend_long n) /* {{{ */
{
	zval tmp;

	ZVAL_LONG(&tmp, n);
	zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp);
}
/* }}} */

ZEND_API void add_index_null(zval *arg, zend_ulong index) /* {{{ */
{
	zval tmp;

	ZVAL_NULL(&tmp);
	zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp);
}
/* }}} */

ZEND_API void add_index_bool(zval *arg, zend_ulong index, bool b) /* {{{ */
{
	zval tmp;

	ZVAL_BOOL(&tmp, b);
	zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp);
}
/* }}} */

ZEND_API void add_index_resource(zval *arg, zend_ulong index, zend_resource *r) /* {{{ */
{
	zval tmp;

	ZVAL_RES(&tmp, r);
	zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp);
}
/* }}} */

ZEND_API void add_index_double(zval *arg, zend_ulong index, double d) /* {{{ */
{
	zval tmp;

	ZVAL_DOUBLE(&tmp, d);
	zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp);
}
/* }}} */

ZEND_API void add_index_str(zval *arg, zend_ulong index, zend_string *str) /* {{{ */
{
	zval tmp;

	ZVAL_STR(&tmp, str);
	zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp);
}
/* }}} */

ZEND_API void add_index_string(zval *arg, zend_ulong index, const char *str) /* {{{ */
{
	zval tmp;

	ZVAL_STRING(&tmp, str);
	zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp);
}
/* }}} */

ZEND_API void add_index_stringl(zval *arg, zend_ulong index, const char *str, size_t length) /* {{{ */
{
	zval tmp;

	ZVAL_STRINGL(&tmp, str, length);
	zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp);
}
/* }}} */

ZEND_API void add_index_array(zval *arg, zend_ulong index, zend_array *arr) /* {{{ */
{
	zval tmp;

	ZVAL_ARR(&tmp, arr);
	zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp);
}
/* }}} */

ZEND_API void add_index_object(zval *arg, zend_ulong index, zend_object *obj) /* {{{ */
{
	zval tmp;

	ZVAL_OBJ(&tmp, obj);
	zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp);
}
/* }}} */

ZEND_API void add_index_reference(zval *arg, zend_ulong index, zend_reference *ref) /* {{{ */
{
	zval tmp;

	ZVAL_REF(&tmp, ref);
	zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp);
}
/* }}} */

ZEND_API zend_result add_next_index_long(zval *arg, zend_long n) /* {{{ */
{
	zval tmp;

	ZVAL_LONG(&tmp, n);
	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp) ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API zend_result add_next_index_null(zval *arg) /* {{{ */
{
	zval tmp;

	ZVAL_NULL(&tmp);
	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp) ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API zend_result add_next_index_bool(zval *arg, bool b) /* {{{ */
{
	zval tmp;

	ZVAL_BOOL(&tmp, b);
	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp) ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API zend_result add_next_index_resource(zval *arg, zend_resource *r) /* {{{ */
{
	zval tmp;

	ZVAL_RES(&tmp, r);
	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp) ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API zend_result add_next_index_double(zval *arg, double d) /* {{{ */
{
	zval tmp;

	ZVAL_DOUBLE(&tmp, d);
	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp) ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API zend_result add_next_index_str(zval *arg, zend_string *str) /* {{{ */
{
	zval tmp;

	ZVAL_STR(&tmp, str);
	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp) ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API zend_result add_next_index_string(zval *arg, const char *str) /* {{{ */
{
	zval tmp;

	ZVAL_STRING(&tmp, str);
	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp) ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API zend_result add_next_index_stringl(zval *arg, const char *str, size_t length) /* {{{ */
{
	zval tmp;

	ZVAL_STRINGL(&tmp, str, length);
	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp) ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API zend_result add_next_index_array(zval *arg, zend_array *arr) /* {{{ */
{
	zval tmp;

	ZVAL_ARR(&tmp, arr);
	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp) ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API zend_result add_next_index_object(zval *arg, zend_object *obj) /* {{{ */
{
	zval tmp;

	ZVAL_OBJ(&tmp, obj);
	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp) ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API zend_result add_next_index_reference(zval *arg, zend_reference *ref) /* {{{ */
{
	zval tmp;

	ZVAL_REF(&tmp, ref);
	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp) ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API zend_result array_set_zval_key(HashTable *ht, zval *key, zval *value) /* {{{ */
{
	zval *result;

	switch (Z_TYPE_P(key)) {
		case IS_STRING:
			result = zend_symtable_update(ht, Z_STR_P(key), value);
			break;
		case IS_NULL:
			result = zend_hash_update(ht, ZSTR_EMPTY_ALLOC(), value);
			break;
		case IS_RESOURCE:
			zend_use_resource_as_offset(key);
			result = zend_hash_index_update(ht, Z_RES_HANDLE_P(key), value);
			break;
		case IS_FALSE:
			result = zend_hash_index_update(ht, 0, value);
			break;
		case IS_TRUE:
			result = zend_hash_index_update(ht, 1, value);
			break;
		case IS_LONG:
			result = zend_hash_index_update(ht, Z_LVAL_P(key), value);
			break;
		case IS_DOUBLE:
			result = zend_hash_index_update(ht, zend_dval_to_lval_safe(Z_DVAL_P(key)), value);
			break;
		default:
			zend_illegal_container_offset(ZSTR_KNOWN(ZEND_STR_ARRAY), key, BP_VAR_W);
			result = NULL;
	}

	if (result) {
		Z_TRY_ADDREF_P(result);
		return SUCCESS;
	} else {
		return FAILURE;
	}
}
/* }}} */

ZEND_API void add_property_long_ex(zval *arg, const char *key, size_t key_len, zend_long n) /* {{{ */
{
	zval tmp;

	ZVAL_LONG(&tmp, n);
	add_property_zval_ex(arg, key, key_len, &tmp);
}
/* }}} */

ZEND_API void add_property_bool_ex(zval *arg, const char *key, size_t key_len, zend_long b) /* {{{ */
{
	zval tmp;

	ZVAL_BOOL(&tmp, b);
	add_property_zval_ex(arg, key, key_len, &tmp);
}
/* }}} */

ZEND_API void add_property_null_ex(zval *arg, const char *key, size_t key_len) /* {{{ */
{
	zval tmp;

	ZVAL_NULL(&tmp);
	add_property_zval_ex(arg, key, key_len, &tmp);
}
/* }}} */

ZEND_API void add_property_resource_ex(zval *arg, const char *key, size_t key_len, zend_resource *r) /* {{{ */
{
	zval tmp;

	ZVAL_RES(&tmp, r);
	add_property_zval_ex(arg, key, key_len, &tmp);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
}
/* }}} */

ZEND_API void add_property_double_ex(zval *arg, const char *key, size_t key_len, double d) /* {{{ */
{
	zval tmp;

	ZVAL_DOUBLE(&tmp, d);
	add_property_zval_ex(arg, key, key_len, &tmp);
}
/* }}} */

ZEND_API void add_property_str_ex(zval *arg, const char *key, size_t key_len, zend_string *str) /* {{{ */
{
	zval tmp;

	ZVAL_STR(&tmp, str);
	add_property_zval_ex(arg, key, key_len, &tmp);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
}
/* }}} */

ZEND_API void add_property_string_ex(zval *arg, const char *key, size_t key_len, const char *str) /* {{{ */
{
	zval tmp;

	ZVAL_STRING(&tmp, str);
	add_property_zval_ex(arg, key, key_len, &tmp);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
}
/* }}} */

ZEND_API void add_property_stringl_ex(zval *arg, const char *key, size_t key_len, const char *str, size_t length) /* {{{ */
{
	zval tmp;

	ZVAL_STRINGL(&tmp, str, length);
	add_property_zval_ex(arg, key, key_len, &tmp);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
}
/* }}} */

ZEND_API void add_property_array_ex(zval *arg, const char *key, size_t key_len, zend_array *arr) /* {{{ */
{
	zval tmp;

	ZVAL_ARR(&tmp, arr);
	add_property_zval_ex(arg, key, key_len, &tmp);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
}
/* }}} */

ZEND_API void add_property_object_ex(zval *arg, const char *key, size_t key_len, zend_object *obj) /* {{{ */
{
	zval tmp;

	ZVAL_OBJ(&tmp, obj);
	add_property_zval_ex(arg, key, key_len, &tmp);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
}
/* }}} */

ZEND_API void add_property_reference_ex(zval *arg, const char *key, size_t key_len, zend_reference *ref) /* {{{ */
{
	zval tmp;

	ZVAL_REF(&tmp, ref);
	add_property_zval_ex(arg, key, key_len, &tmp);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
}
/* }}} */

ZEND_API void add_property_zval_ex(zval *arg, const char *key, size_t key_len, zval *value) /* {{{ */
{
	zend_string *str;

	str = zend_string_init(key, key_len, 0);
	Z_OBJ_HANDLER_P(arg, write_property)(Z_OBJ_P(arg), str, value, NULL);
	zend_string_release_ex(str, 0);
}
/* }}} */

ZEND_API zend_result zend_startup_module_ex(zend_module_entry *module) /* {{{ */
{
	size_t name_len;
	zend_string *lcname;

	if (module->module_started) {
		return SUCCESS;
	}
	module->module_started = 1;

	/* Check module dependencies */
	if (module->deps) {
		const zend_module_dep *dep = module->deps;

		while (dep->name) {
			if (dep->type == MODULE_DEP_REQUIRED) {
				zend_module_entry *req_mod;

				name_len = strlen(dep->name);
				lcname = zend_string_alloc(name_len, 0);
				zend_str_tolower_copy(ZSTR_VAL(lcname), dep->name, name_len);

				if ((req_mod = zend_hash_find_ptr(&module_registry, lcname)) == NULL || !req_mod->module_started) {
					zend_string_efree(lcname);
					/* TODO: Check version relationship */
					zend_error(E_CORE_WARNING, "Cannot load module \"%s\" because required module \"%s\" is not loaded", module->name, dep->name);
					module->module_started = 0;
					return FAILURE;
				}
				zend_string_efree(lcname);
			}
			++dep;
		}
	}

	/* Initialize module globals */
	if (module->globals_size) {
#ifdef ZTS
		ts_allocate_id(module->globals_id_ptr, module->globals_size, (ts_allocate_ctor) module->globals_ctor, (ts_allocate_dtor) module->globals_dtor);
#else
		if (module->globals_ctor) {
			module->globals_ctor(module->globals_ptr);
		}
#endif
	}
	if (module->module_startup_func) {
		EG(current_module) = module;
		if (module->module_startup_func(module->type, module->module_number)==FAILURE) {
			zend_error_noreturn(E_CORE_ERROR,"Unable to start %s module", module->name);
			EG(current_module) = NULL;
			return FAILURE;
		}
		EG(current_module) = NULL;
	}
	return SUCCESS;
}
/* }}} */

static int zend_startup_module_zval(zval *zv) /* {{{ */
{
	zend_module_entry *module = Z_PTR_P(zv);

	return (zend_startup_module_ex(module) == SUCCESS) ? ZEND_HASH_APPLY_KEEP : ZEND_HASH_APPLY_REMOVE;
}
/* }}} */

static void zend_sort_modules(void *base, size_t count, size_t siz, compare_func_t compare, swap_func_t swp) /* {{{ */
{
	Bucket *b1 = base;
	Bucket *b2;
	Bucket *end = b1 + count;
	Bucket tmp;
	zend_module_entry *m, *r;

	while (b1 < end) {
try_again:
		m = (zend_module_entry*)Z_PTR(b1->val);
		if (!m->module_started && m->deps) {
			const zend_module_dep *dep = m->deps;
			while (dep->name) {
				if (dep->type == MODULE_DEP_REQUIRED || dep->type == MODULE_DEP_OPTIONAL) {
					b2 = b1 + 1;
					while (b2 < end) {
						r = (zend_module_entry*)Z_PTR(b2->val);
						if (strcasecmp(dep->name, r->name) == 0) {
							tmp = *b1;
							*b1 = *b2;
							*b2 = tmp;
							goto try_again;
						}
						b2++;
					}
				}
				dep++;
			}
		}
		b1++;
	}
}
/* }}} */

ZEND_API void zend_collect_module_handlers(void) /* {{{ */
{
	zend_module_entry *module;
	int startup_count = 0;
	int shutdown_count = 0;
	int post_deactivate_count = 0;
	zend_class_entry *ce;
	int class_count = 0;

	/* Collect extensions with request startup/shutdown handlers */
	ZEND_HASH_MAP_FOREACH_PTR(&module_registry, module) {
		if (module->request_startup_func) {
			startup_count++;
		}
		if (module->request_shutdown_func) {
			shutdown_count++;
		}
		if (module->post_deactivate_func) {
			post_deactivate_count++;
		}
	} ZEND_HASH_FOREACH_END();
	module_request_startup_handlers = (zend_module_entry**)malloc(
	    sizeof(zend_module_entry*) *
		(startup_count + 1 +
		 shutdown_count + 1 +
		 post_deactivate_count + 1));
	module_request_startup_handlers[startup_count] = NULL;
	module_request_shutdown_handlers = module_request_startup_handlers + startup_count + 1;
	module_request_shutdown_handlers[shutdown_count] = NULL;
	module_post_deactivate_handlers = module_request_shutdown_handlers + shutdown_count + 1;
	module_post_deactivate_handlers[post_deactivate_count] = NULL;
	startup_count = 0;

	ZEND_HASH_MAP_FOREACH_PTR(&module_registry, module) {
		if (module->request_startup_func) {
			module_request_startup_handlers[startup_count++] = module;
		}
		if (module->request_shutdown_func) {
			module_request_shutdown_handlers[--shutdown_count] = module;
		}
		if (module->post_deactivate_func) {
			module_post_deactivate_handlers[--post_deactivate_count] = module;
		}
	} ZEND_HASH_FOREACH_END();

	/* Collect internal classes with static members */
	ZEND_HASH_MAP_FOREACH_PTR(CG(class_table), ce) {
		if (ce->type == ZEND_INTERNAL_CLASS &&
		    ce->default_static_members_count > 0) {
		    class_count++;
		}
	} ZEND_HASH_FOREACH_END();

	class_cleanup_handlers = (zend_class_entry**)malloc(
		sizeof(zend_class_entry*) *
		(class_count + 1));
	class_cleanup_handlers[class_count] = NULL;

	if (class_count) {
		ZEND_HASH_MAP_FOREACH_PTR(CG(class_table), ce) {
			if (ce->type == ZEND_INTERNAL_CLASS &&
			    ce->default_static_members_count > 0) {
			    class_cleanup_handlers[--class_count] = ce;
			}
		} ZEND_HASH_FOREACH_END();
	}
}
/* }}} */

ZEND_API void zend_startup_modules(void) /* {{{ */
{
	zend_hash_sort_ex(&module_registry, zend_sort_modules, NULL, 0);
	zend_hash_apply(&module_registry, zend_startup_module_zval);
}
/* }}} */

ZEND_API void zend_destroy_modules(void) /* {{{ */
{
	free(class_cleanup_handlers);
	free(module_request_startup_handlers);
	zend_hash_graceful_reverse_destroy(&module_registry);
}
/* }}} */

ZEND_API zend_module_entry* zend_register_module_ex(zend_module_entry *module) /* {{{ */
{
	size_t name_len;
	zend_string *lcname;
	zend_module_entry *module_ptr;

	if (!module) {
		return NULL;
	}

#if 0
	zend_printf("%s: Registering module %d\n", module->name, module->module_number);
#endif

	/* Check module dependencies */
	if (module->deps) {
		const zend_module_dep *dep = module->deps;

		while (dep->name) {
			if (dep->type == MODULE_DEP_CONFLICTS) {
				name_len = strlen(dep->name);
				lcname = zend_string_alloc(name_len, 0);
				zend_str_tolower_copy(ZSTR_VAL(lcname), dep->name, name_len);

				if (zend_hash_exists(&module_registry, lcname) || zend_get_extension(dep->name)) {
					zend_string_efree(lcname);
					/* TODO: Check version relationship */
					zend_error(E_CORE_WARNING, "Cannot load module \"%s\" because conflicting module \"%s\" is already loaded", module->name, dep->name);
					return NULL;
				}
				zend_string_efree(lcname);
			}
			++dep;
		}
	}

	name_len = strlen(module->name);
	lcname = zend_string_alloc(name_len, module->type == MODULE_PERSISTENT);
	zend_str_tolower_copy(ZSTR_VAL(lcname), module->name, name_len);

	lcname = zend_new_interned_string(lcname);
	if ((module_ptr = zend_hash_add_ptr(&module_registry, lcname, module)) == NULL) {
		zend_error(E_CORE_WARNING, "Module \"%s\" is already loaded", module->name);
		zend_string_release(lcname);
		return NULL;
	}
	module = module_ptr;
	EG(current_module) = module;

	if (module->functions && zend_register_functions(NULL, module->functions, NULL, module->type)==FAILURE) {
		zend_hash_del(&module_registry, lcname);
		zend_string_release(lcname);
		EG(current_module) = NULL;
		zend_error(E_CORE_WARNING,"%s: Unable to register functions, unable to load", module->name);
		return NULL;
	}

	EG(current_module) = NULL;
	zend_string_release(lcname);
	return module;
}
/* }}} */

ZEND_API zend_module_entry* zend_register_internal_module(zend_module_entry *module) /* {{{ */
{
	module->module_number = zend_next_free_module();
	module->type = MODULE_PERSISTENT;
	return zend_register_module_ex(module);
}
/* }}} */

static void zend_check_magic_method_args(
		uint32_t num_args, const zend_class_entry *ce, const zend_function *fptr, int error_type)
{
	if (fptr->common.num_args != num_args) {
		if (num_args == 0) {
			zend_error(error_type, "Method %s::%s() cannot take arguments",
				ZSTR_VAL(ce->name), ZSTR_VAL(fptr->common.function_name));
		} else if (num_args == 1) {
			zend_error(error_type, "Method %s::%s() must take exactly 1 argument",
				ZSTR_VAL(ce->name), ZSTR_VAL(fptr->common.function_name));
		} else {
			zend_error(error_type, "Method %s::%s() must take exactly %" PRIu32 " arguments",
				ZSTR_VAL(ce->name), ZSTR_VAL(fptr->common.function_name), num_args);
		}
		return;
	}
	for (uint32_t i = 0; i < num_args; i++) {
		if (QUICK_ARG_SHOULD_BE_SENT_BY_REF(fptr, i + 1)) {
			zend_error(error_type, "Method %s::%s() cannot take arguments by reference",
				ZSTR_VAL(ce->name), ZSTR_VAL(fptr->common.function_name));
			return;
		}
	}
}

static void zend_check_magic_method_arg_type(uint32_t arg_num, const zend_class_entry *ce, const zend_function *fptr, int error_type, int arg_type)
{
		if (
			ZEND_TYPE_IS_SET(fptr->common.arg_info[arg_num].type)
			 && !(ZEND_TYPE_FULL_MASK(fptr->common.arg_info[arg_num].type) & arg_type)
		) {
			zend_error(error_type, "%s::%s(): Parameter #%d ($%s) must be of type %s when declared",
				ZSTR_VAL(ce->name), ZSTR_VAL(fptr->common.function_name),
				arg_num + 1, ZSTR_VAL(fptr->common.arg_info[arg_num].name),
				ZSTR_VAL(zend_type_to_string((zend_type) ZEND_TYPE_INIT_MASK(arg_type))));
		}
}

static void zend_check_magic_method_return_type(const zend_class_entry *ce, const zend_function *fptr, int error_type, int return_type)
{
	if (!(fptr->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE)) {
		/* For backwards compatibility reasons, do not enforce the return type if it is not set. */
		return;
	}

	if (ZEND_TYPE_PURE_MASK(fptr->common.arg_info[-1].type) & MAY_BE_NEVER) {
		/* It is always legal to specify the never type. */
		return;
	}

	bool is_complex_type = ZEND_TYPE_IS_COMPLEX(fptr->common.arg_info[-1].type);
	uint32_t extra_types = ZEND_TYPE_PURE_MASK(fptr->common.arg_info[-1].type) & ~return_type;
	if (extra_types & MAY_BE_STATIC) {
		extra_types &= ~MAY_BE_STATIC;
		is_complex_type = true;
	}

	if (extra_types || (is_complex_type && return_type != MAY_BE_OBJECT)) {
		zend_error(error_type, "%s::%s(): Return type must be %s when declared",
			ZSTR_VAL(ce->name), ZSTR_VAL(fptr->common.function_name),
			ZSTR_VAL(zend_type_to_string((zend_type) ZEND_TYPE_INIT_MASK(return_type))));
	}
}

static void zend_check_magic_method_non_static(
		const zend_class_entry *ce, const zend_function *fptr, int error_type)
{
	if (fptr->common.fn_flags & ZEND_ACC_STATIC) {
		zend_error(error_type, "Method %s::%s() cannot be static",
			ZSTR_VAL(ce->name), ZSTR_VAL(fptr->common.function_name));
	}
}

static void zend_check_magic_method_static(
		const zend_class_entry *ce, const zend_function *fptr, int error_type)
{
	if (!(fptr->common.fn_flags & ZEND_ACC_STATIC)) {
		zend_error(error_type, "Method %s::%s() must be static",
			ZSTR_VAL(ce->name), ZSTR_VAL(fptr->common.function_name));
	}
}

static void zend_check_magic_method_public(
		const zend_class_entry *ce, const zend_function *fptr, int error_type)
{
	// TODO: Remove this warning after adding proper visibility handling.
	if (!(fptr->common.fn_flags & ZEND_ACC_PUBLIC)) {
		zend_error(E_WARNING, "The magic method %s::%s() must have public visibility",
			ZSTR_VAL(ce->name), ZSTR_VAL(fptr->common.function_name));
	}
}

static void zend_check_magic_method_no_return_type(
		const zend_class_entry *ce, const zend_function *fptr, int error_type)
{
	if (fptr->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE) {
		zend_error_noreturn(error_type, "Method %s::%s() cannot declare a return type",
			ZSTR_VAL(ce->name), ZSTR_VAL(fptr->common.function_name));
	}
}

ZEND_API void zend_check_magic_method_implementation(const zend_class_entry *ce, const zend_function *fptr, zend_string *lcname, int error_type) /* {{{ */
{
	if (ZSTR_VAL(fptr->common.function_name)[0] != '_'
	 || ZSTR_VAL(fptr->common.function_name)[1] != '_') {
		return;
	}

	if (zend_string_equals_literal(lcname, ZEND_CONSTRUCTOR_FUNC_NAME)) {
		zend_check_magic_method_non_static(ce, fptr, error_type);
		zend_check_magic_method_no_return_type(ce, fptr, error_type);
	} else if (zend_string_equals_literal(lcname, ZEND_DESTRUCTOR_FUNC_NAME)) {
		zend_check_magic_method_args(0, ce, fptr, error_type);
		zend_check_magic_method_non_static(ce, fptr, error_type);
		zend_check_magic_method_no_return_type(ce, fptr, error_type);
	} else if (zend_string_equals_literal(lcname, ZEND_CLONE_FUNC_NAME)) {
		zend_check_magic_method_args(0, ce, fptr, error_type);
		zend_check_magic_method_non_static(ce, fptr, error_type);
		zend_check_magic_method_return_type(ce, fptr, error_type, MAY_BE_VOID);
	} else if (zend_string_equals_literal(lcname, ZEND_GET_FUNC_NAME)) {
		zend_check_magic_method_args(1, ce, fptr, error_type);
		zend_check_magic_method_non_static(ce, fptr, error_type);
		zend_check_magic_method_public(ce, fptr, error_type);
		zend_check_magic_method_arg_type(0, ce, fptr, error_type, MAY_BE_STRING);
	} else if (zend_string_equals_literal(lcname, ZEND_SET_FUNC_NAME)) {
		zend_check_magic_method_args(2, ce, fptr, error_type);
		zend_check_magic_method_non_static(ce, fptr, error_type);
		zend_check_magic_method_public(ce, fptr, error_type);
		zend_check_magic_method_arg_type(0, ce, fptr, error_type, MAY_BE_STRING);
		zend_check_magic_method_return_type(ce, fptr, error_type, MAY_BE_VOID);
	} else if (zend_string_equals_literal(lcname, ZEND_UNSET_FUNC_NAME)) {
		zend_check_magic_method_args(1, ce, fptr, error_type);
		zend_check_magic_method_non_static(ce, fptr, error_type);
		zend_check_magic_method_public(ce, fptr, error_type);
		zend_check_magic_method_arg_type(0, ce, fptr, error_type, MAY_BE_STRING);
		zend_check_magic_method_return_type(ce, fptr, error_type, MAY_BE_VOID);
	} else if (zend_string_equals_literal(lcname, ZEND_ISSET_FUNC_NAME)) {
		zend_check_magic_method_args(1, ce, fptr, error_type);
		zend_check_magic_method_non_static(ce, fptr, error_type);
		zend_check_magic_method_public(ce, fptr, error_type);
		zend_check_magic_method_arg_type(0, ce, fptr, error_type, MAY_BE_STRING);
		zend_check_magic_method_return_type(ce, fptr, error_type, MAY_BE_BOOL);
	} else if (zend_string_equals_literal(lcname, ZEND_CALL_FUNC_NAME)) {
		zend_check_magic_method_args(2, ce, fptr, error_type);
		zend_check_magic_method_non_static(ce, fptr, error_type);
		zend_check_magic_method_public(ce, fptr, error_type);
		zend_check_magic_method_arg_type(0, ce, fptr, error_type, MAY_BE_STRING);
		zend_check_magic_method_arg_type(1, ce, fptr, error_type, MAY_BE_ARRAY);
	} else if (zend_string_equals_literal(lcname, ZEND_CALLSTATIC_FUNC_NAME)) {
		zend_check_magic_method_args(2, ce, fptr, error_type);
		zend_check_magic_method_static(ce, fptr, error_type);
		zend_check_magic_method_public(ce, fptr, error_type);
		zend_check_magic_method_arg_type(0, ce, fptr, error_type, MAY_BE_STRING);
		zend_check_magic_method_arg_type(1, ce, fptr, error_type, MAY_BE_ARRAY);
	} else if (zend_string_equals_literal(lcname, ZEND_TOSTRING_FUNC_NAME)) {
		zend_check_magic_method_args(0, ce, fptr, error_type);
		zend_check_magic_method_non_static(ce, fptr, error_type);
		zend_check_magic_method_public(ce, fptr, error_type);
		zend_check_magic_method_return_type(ce, fptr, error_type, MAY_BE_STRING);
	} else if (zend_string_equals_literal(lcname, ZEND_DEBUGINFO_FUNC_NAME)) {
		zend_check_magic_method_args(0, ce, fptr, error_type);
		zend_check_magic_method_non_static(ce, fptr, error_type);
		zend_check_magic_method_public(ce, fptr, error_type);
		zend_check_magic_method_return_type(ce, fptr, error_type, (MAY_BE_ARRAY | MAY_BE_NULL));
	} else if (zend_string_equals_literal(lcname, "__serialize")) {
		zend_check_magic_method_args(0, ce, fptr, error_type);
		zend_check_magic_method_non_static(ce, fptr, error_type);
		zend_check_magic_method_public(ce, fptr, error_type);
		zend_check_magic_method_return_type(ce, fptr, error_type, MAY_BE_ARRAY);
	} else if (zend_string_equals_literal(lcname, "__unserialize")) {
		zend_check_magic_method_args(1, ce, fptr, error_type);
		zend_check_magic_method_non_static(ce, fptr, error_type);
		zend_check_magic_method_public(ce, fptr, error_type);
		zend_check_magic_method_arg_type(0, ce, fptr, error_type, MAY_BE_ARRAY);
		zend_check_magic_method_return_type(ce, fptr, error_type, MAY_BE_VOID);
	} else if (zend_string_equals_literal(lcname, "__set_state")) {
		zend_check_magic_method_args(1, ce, fptr, error_type);
		zend_check_magic_method_static(ce, fptr, error_type);
		zend_check_magic_method_public(ce, fptr, error_type);
		zend_check_magic_method_arg_type(0, ce, fptr, error_type, MAY_BE_ARRAY);
		zend_check_magic_method_return_type(ce, fptr, error_type, MAY_BE_OBJECT);
	} else if (zend_string_equals(lcname, ZSTR_KNOWN(ZEND_STR_MAGIC_INVOKE))) {
		zend_check_magic_method_non_static(ce, fptr, error_type);
		zend_check_magic_method_public(ce, fptr, error_type);
	} else if (zend_string_equals(lcname, ZSTR_KNOWN(ZEND_STR_SLEEP))) {
		zend_check_magic_method_args(0, ce, fptr, error_type);
		zend_check_magic_method_non_static(ce, fptr, error_type);
		zend_check_magic_method_public(ce, fptr, error_type);
		zend_check_magic_method_return_type(ce, fptr, error_type, MAY_BE_ARRAY);
	} else if (zend_string_equals(lcname, ZSTR_KNOWN(ZEND_STR_WAKEUP))) {
		zend_check_magic_method_args(0, ce, fptr, error_type);
		zend_check_magic_method_non_static(ce, fptr, error_type);
		zend_check_magic_method_public(ce, fptr, error_type);
		zend_check_magic_method_return_type(ce, fptr, error_type, MAY_BE_VOID);
	}
}
/* }}} */

ZEND_API void zend_add_magic_method(zend_class_entry *ce, zend_function *fptr, zend_string *lcname)
{
	if (ZSTR_VAL(lcname)[0] != '_' || ZSTR_VAL(lcname)[1] != '_') {
		/* pass */
	} else if (zend_string_equals_literal(lcname, ZEND_CLONE_FUNC_NAME)) {
		ce->clone = fptr;
	} else if (zend_string_equals_literal(lcname, ZEND_CONSTRUCTOR_FUNC_NAME)) {
		ce->constructor = fptr;
		ce->constructor->common.fn_flags |= ZEND_ACC_CTOR;
	} else if (zend_string_equals_literal(lcname, ZEND_DESTRUCTOR_FUNC_NAME)) {
		ce->destructor = fptr;
	} else if (zend_string_equals_literal(lcname, ZEND_GET_FUNC_NAME)) {
		ce->__get = fptr;
		ce->ce_flags |= ZEND_ACC_USE_GUARDS;
	} else if (zend_string_equals_literal(lcname, ZEND_SET_FUNC_NAME)) {
		ce->__set = fptr;
		ce->ce_flags |= ZEND_ACC_USE_GUARDS;
	} else if (zend_string_equals_literal(lcname, ZEND_CALL_FUNC_NAME)) {
		ce->__call = fptr;
	} else if (zend_string_equals_literal(lcname, ZEND_UNSET_FUNC_NAME)) {
		ce->__unset = fptr;
		ce->ce_flags |= ZEND_ACC_USE_GUARDS;
	} else if (zend_string_equals_literal(lcname, ZEND_ISSET_FUNC_NAME)) {
		ce->__isset = fptr;
		ce->ce_flags |= ZEND_ACC_USE_GUARDS;
	} else if (zend_string_equals_literal(lcname, ZEND_CALLSTATIC_FUNC_NAME)) {
		ce->__callstatic = fptr;
	} else if (zend_string_equals_literal(lcname, ZEND_TOSTRING_FUNC_NAME)) {
		ce->__tostring = fptr;
	} else if (zend_string_equals_literal(lcname, ZEND_DEBUGINFO_FUNC_NAME)) {
		ce->__debugInfo = fptr;
		ce->ce_flags |= ZEND_ACC_USE_GUARDS;
	} else if (zend_string_equals_literal(lcname, "__serialize")) {
		ce->__serialize = fptr;
	} else if (zend_string_equals_literal(lcname, "__unserialize")) {
		ce->__unserialize = fptr;
	}
}

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arg_info_toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

static zend_always_inline void zend_normalize_internal_type(zend_type *type) {
	ZEND_ASSERT(!ZEND_TYPE_HAS_LITERAL_NAME(*type));
	zend_type *current;
	ZEND_TYPE_FOREACH(*type, current) {
		if (ZEND_TYPE_HAS_NAME(*current)) {
			zend_string *name = zend_new_interned_string(ZEND_TYPE_NAME(*current));
			zend_alloc_ce_cache(name);
			ZEND_TYPE_SET_PTR(*current, name);
		} else if (ZEND_TYPE_HAS_LIST(*current)) {
			zend_type *inner;
			ZEND_TYPE_FOREACH(*current, inner) {
				ZEND_ASSERT(!ZEND_TYPE_HAS_LITERAL_NAME(*inner) && !ZEND_TYPE_HAS_LIST(*inner));
				if (ZEND_TYPE_HAS_NAME(*inner)) {
					zend_string *name = zend_new_interned_string(ZEND_TYPE_NAME(*inner));
					zend_alloc_ce_cache(name);
					ZEND_TYPE_SET_PTR(*inner, name);
				}
			} ZEND_TYPE_FOREACH_END();
		}
	} ZEND_TYPE_FOREACH_END();
}

/* registers all functions in *library_functions in the function hash */
ZEND_API zend_result zend_register_functions(zend_class_entry *scope, const zend_function_entry *functions, HashTable *function_table, int type) /* {{{ */
{
	const zend_function_entry *ptr = functions;
	zend_function function;
	zend_internal_function *reg_function, *internal_function = (zend_internal_function *)&function;
	int count=0, unload=0;
	HashTable *target_function_table = function_table;
	int error_type;
	zend_string *lowercase_name;
	size_t fname_len;

	if (type==MODULE_PERSISTENT) {
		error_type = E_CORE_WARNING;
	} else {
		error_type = E_WARNING;
	}

	if (!target_function_table) {
		target_function_table = CG(function_table);
	}
	internal_function->type = ZEND_INTERNAL_FUNCTION;
	internal_function->module = EG(current_module);
	internal_function->T = 0;
	memset(internal_function->reserved, 0, ZEND_MAX_RESERVED_RESOURCES * sizeof(void*));

	while (ptr->fname) {
		fname_len = strlen(ptr->fname);
		internal_function->handler = ptr->handler;
		internal_function->function_name = zend_string_init_interned(ptr->fname, fname_len, 1);
		internal_function->scope = scope;
		internal_function->prototype = NULL;
		internal_function->attributes = NULL;
		if (EG(active)) { // at run-time: this ought to only happen if registered with dl() or somehow temporarily at runtime
			ZEND_MAP_PTR_INIT(internal_function->run_time_cache, zend_arena_calloc(&CG(arena), 1, zend_internal_run_time_cache_reserved_size()));
		} else {
			ZEND_MAP_PTR_NEW(internal_function->run_time_cache);
		}
		if (ptr->flags) {
			if (!(ptr->flags & ZEND_ACC_PPP_MASK)) {
				if (ptr->flags != ZEND_ACC_DEPRECATED && scope) {
					zend_error(error_type, "Invalid access level for %s::%s() - access must be exactly one of public, protected or private", ZSTR_VAL(scope->name), ptr->fname);
				}
				internal_function->fn_flags = ZEND_ACC_PUBLIC | ptr->flags;
			} else {
				internal_function->fn_flags = ptr->flags;
			}
		} else {
			internal_function->fn_flags = ZEND_ACC_PUBLIC;
		}

		if (ptr->arg_info) {
			zend_internal_function_info *info = (zend_internal_function_info*)ptr->arg_info;
			internal_function->arg_info = (zend_internal_arg_info*)ptr->arg_info+1;
			internal_function->num_args = ptr->num_args;
			/* Currently you cannot denote that the function can accept less arguments than num_args */
			if (info->required_num_args == (uintptr_t)-1) {
				internal_function->required_num_args = ptr->num_args;
			} else {
				internal_function->required_num_args = info->required_num_args;
			}
			if (ZEND_ARG_SEND_MODE(info)) {
				internal_function->fn_flags |= ZEND_ACC_RETURN_REFERENCE;
			}
			if (ZEND_ARG_IS_VARIADIC(&ptr->arg_info[ptr->num_args])) {
				internal_function->fn_flags |= ZEND_ACC_VARIADIC;
				/* Don't count the variadic argument */
				internal_function->num_args--;
			}
			if (ZEND_TYPE_IS_SET(info->type)) {
				if (ZEND_TYPE_HAS_NAME(info->type)) {
					const char *type_name = ZEND_TYPE_LITERAL_NAME(info->type);
					if (!scope && (!strcasecmp(type_name, "self") || !strcasecmp(type_name, "parent"))) {
						zend_error_noreturn(E_CORE_ERROR, "Cannot declare a return type of %s outside of a class scope", type_name);
					}
				}

				internal_function->fn_flags |= ZEND_ACC_HAS_RETURN_TYPE;
			}
		} else {
			zend_error(E_CORE_WARNING, "Missing arginfo for %s%s%s()",
				 scope ? ZSTR_VAL(scope->name) : "", scope ? "::" : "", ptr->fname);

			internal_function->arg_info = NULL;
			internal_function->num_args = 0;
			internal_function->required_num_args = 0;
		}

		/* If not specified, add __toString() return type for compatibility with Stringable
		 * interface. */
		if (scope && zend_string_equals_literal_ci(internal_function->function_name, "__tostring") &&
				!(internal_function->fn_flags & ZEND_ACC_HAS_RETURN_TYPE)) {
			zend_error(E_CORE_WARNING, "%s::__toString() implemented without string return type",
				ZSTR_VAL(scope->name));
			internal_function->arg_info = (zend_internal_arg_info *) arg_info_toString + 1;
			internal_function->fn_flags |= ZEND_ACC_HAS_RETURN_TYPE;
			internal_function->num_args = internal_function->required_num_args = 0;
		}


		zend_set_function_arg_flags((zend_function*)internal_function);
		if (ptr->flags & ZEND_ACC_ABSTRACT) {
			if (scope) {
				/* This is a class that must be abstract itself. Here we set the check info. */
				scope->ce_flags |= ZEND_ACC_IMPLICIT_ABSTRACT_CLASS;
				if (!(scope->ce_flags & ZEND_ACC_INTERFACE)) {
					/* Since the class is not an interface it needs to be declared as a abstract class. */
					/* Since here we are handling internal functions only we can add the keyword flag. */
					/* This time we set the flag for the keyword 'abstract'. */
					scope->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;
				}
			}
			if ((ptr->flags & ZEND_ACC_STATIC) && (!scope || !(scope->ce_flags & ZEND_ACC_INTERFACE))) {
				zend_error(error_type, "Static function %s%s%s() cannot be abstract", scope ? ZSTR_VAL(scope->name) : "", scope ? "::" : "", ptr->fname);
			}
		} else {
			if (scope && (scope->ce_flags & ZEND_ACC_INTERFACE)) {
				zend_error(error_type, "Interface %s cannot contain non abstract method %s()", ZSTR_VAL(scope->name), ptr->fname);
				return FAILURE;
			}
			if (!internal_function->handler) {
				zend_error(error_type, "Method %s%s%s() cannot be a NULL function", scope ? ZSTR_VAL(scope->name) : "", scope ? "::" : "", ptr->fname);
				zend_unregister_functions(functions, count, target_function_table);
				return FAILURE;
			}
		}
		lowercase_name = zend_string_tolower_ex(internal_function->function_name, type == MODULE_PERSISTENT);
		lowercase_name = zend_new_interned_string(lowercase_name);
		reg_function = malloc(sizeof(zend_internal_function));
		memcpy(reg_function, &function, sizeof(zend_internal_function));
		if (zend_hash_add_ptr(target_function_table, lowercase_name, reg_function) == NULL) {
			unload=1;
			free(reg_function);
			zend_string_release(lowercase_name);
			break;
		}

		/* Get parameter count including variadic parameter. */
		uint32_t num_args = reg_function->num_args;
		if (reg_function->fn_flags & ZEND_ACC_VARIADIC) {
			num_args++;
		}

		/* If types of arguments have to be checked */
		if (reg_function->arg_info && num_args) {
			uint32_t i;
			for (i = 0; i < num_args; i++) {
				zend_internal_arg_info *arg_info = &reg_function->arg_info[i];
				ZEND_ASSERT(arg_info->name && "Parameter must have a name");
				if (ZEND_TYPE_IS_SET(arg_info->type)) {
				    reg_function->fn_flags |= ZEND_ACC_HAS_TYPE_HINTS;
				}
#if ZEND_DEBUG
				for (uint32_t j = 0; j < i; j++) {
					if (!strcmp(arg_info->name, reg_function->arg_info[j].name)) {
						zend_error_noreturn(E_CORE_ERROR,
							"Duplicate parameter name $%s for function %s%s%s()", arg_info->name,
							scope ? ZSTR_VAL(scope->name) : "", scope ? "::" : "", ptr->fname);
					}
				}
#endif
			}
		}

		/* Rebuild arginfos if parameter/property types and/or a return type are used */
		if (reg_function->arg_info &&
		    (reg_function->fn_flags & (ZEND_ACC_HAS_RETURN_TYPE|ZEND_ACC_HAS_TYPE_HINTS))) {
			/* convert "const char*" class type names into "zend_string*" */
			uint32_t i;
			zend_internal_arg_info *arg_info = reg_function->arg_info - 1;
			zend_internal_arg_info *new_arg_info;

			/* Treat return type as an extra argument */
			num_args++;
			new_arg_info = malloc(sizeof(zend_internal_arg_info) * num_args);
			memcpy(new_arg_info, arg_info, sizeof(zend_internal_arg_info) * num_args);
			reg_function->arg_info = new_arg_info + 1;
			for (i = 0; i < num_args; i++) {
				if (ZEND_TYPE_HAS_LITERAL_NAME(new_arg_info[i].type)) {
					// gen_stubs.php does not support codegen for DNF types in arg infos.
					// As a temporary workaround, we split the type name on `|` characters,
					// converting it to an union type if necessary.
					const char *class_name = ZEND_TYPE_LITERAL_NAME(new_arg_info[i].type);
					new_arg_info[i].type.type_mask &= ~_ZEND_TYPE_LITERAL_NAME_BIT;

					size_t num_types = 1;
					const char *p = class_name;
					while ((p = strchr(p, '|'))) {
						num_types++;
						p++;
					}

					if (num_types == 1) {
						/* Simple class type */
						zend_string *str = zend_string_init_interned(class_name, strlen(class_name), 1);
						zend_alloc_ce_cache(str);
						ZEND_TYPE_SET_PTR(new_arg_info[i].type, str);
						new_arg_info[i].type.type_mask |= _ZEND_TYPE_NAME_BIT;
					} else {
						/* Union type */
						zend_type_list *list = malloc(ZEND_TYPE_LIST_SIZE(num_types));
						list->num_types = num_types;
						ZEND_TYPE_SET_LIST(new_arg_info[i].type, list);
						ZEND_TYPE_FULL_MASK(new_arg_info[i].type) |= _ZEND_TYPE_UNION_BIT;

						const char *start = class_name;
						uint32_t j = 0;
						while (true) {
							const char *end = strchr(start, '|');
							zend_string *str = zend_string_init_interned(start, end ? end - start : strlen(start), 1);
							zend_alloc_ce_cache(str);
							list->types[j] = (zend_type) ZEND_TYPE_INIT_CLASS(str, 0, 0);
							if (!end) {
								break;
							}
							start = end + 1;
							j++;
						}
					}
				}
				if (ZEND_TYPE_IS_ITERABLE_FALLBACK(new_arg_info[i].type)) {
					/* Warning generated an extension load warning which is emitted for every test
					zend_error(E_CORE_WARNING, "iterable type is now a compile time alias for array|Traversable,"
						" regenerate the argument info via the php-src gen_stub build script");
					*/
					zend_type legacy_iterable = ZEND_TYPE_INIT_CLASS_MASK(
						ZSTR_KNOWN(ZEND_STR_TRAVERSABLE),
						(new_arg_info[i].type.type_mask | MAY_BE_ARRAY)
					);
					new_arg_info[i].type = legacy_iterable;
				}

				zend_normalize_internal_type(&new_arg_info[i].type);
			}
		}

		if (scope) {
			zend_check_magic_method_implementation(
				scope, (zend_function *)reg_function, lowercase_name, E_CORE_ERROR);
			zend_add_magic_method(scope, (zend_function *)reg_function, lowercase_name);
		}
		ptr++;
		count++;
		zend_string_release(lowercase_name);
	}
	if (unload) { /* before unloading, display all remaining bad function in the module */
		while (ptr->fname) {
			fname_len = strlen(ptr->fname);
			lowercase_name = zend_string_alloc(fname_len, 0);
			zend_str_tolower_copy(ZSTR_VAL(lowercase_name), ptr->fname, fname_len);
			if (zend_hash_exists(target_function_table, lowercase_name)) {
				zend_error(error_type, "Function registration failed - duplicate name - %s%s%s", scope ? ZSTR_VAL(scope->name) : "", scope ? "::" : "", ptr->fname);
			}
			zend_string_efree(lowercase_name);
			ptr++;
		}
		zend_unregister_functions(functions, count, target_function_table);
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

/* count=-1 means erase all functions, otherwise,
 * erase the first count functions
 */
ZEND_API void zend_unregister_functions(const zend_function_entry *functions, int count, HashTable *function_table) /* {{{ */
{
	const zend_function_entry *ptr = functions;
	int i=0;
	HashTable *target_function_table = function_table;
	zend_string *lowercase_name;
	size_t fname_len;

	if (!target_function_table) {
		target_function_table = CG(function_table);
	}
	while (ptr->fname) {
		if (count!=-1 && i>=count) {
			break;
		}
		fname_len = strlen(ptr->fname);
		lowercase_name = zend_string_alloc(fname_len, 0);
		zend_str_tolower_copy(ZSTR_VAL(lowercase_name), ptr->fname, fname_len);
		zend_hash_del(target_function_table, lowercase_name);
		zend_string_efree(lowercase_name);
		ptr++;
		i++;
	}
}
/* }}} */

ZEND_API zend_result zend_startup_module(zend_module_entry *module) /* {{{ */
{
	if ((module = zend_register_internal_module(module)) != NULL && zend_startup_module_ex(module) == SUCCESS) {
		return SUCCESS;
	}
	return FAILURE;
}
/* }}} */

ZEND_API zend_result zend_get_module_started(const char *module_name) /* {{{ */
{
	zend_module_entry *module;

	module = zend_hash_str_find_ptr(&module_registry, module_name, strlen(module_name));
	return (module && module->module_started) ? SUCCESS : FAILURE;
}
/* }}} */

static int clean_module_class(zval *el, void *arg) /* {{{ */
{
	zend_class_entry *ce = (zend_class_entry *)Z_PTR_P(el);
	int module_number = *(int *)arg;
	if (ce->type == ZEND_INTERNAL_CLASS && ce->info.internal.module->module_number == module_number) {
		return ZEND_HASH_APPLY_REMOVE;
	} else {
		return ZEND_HASH_APPLY_KEEP;
	}
}
/* }}} */

static void clean_module_classes(int module_number) /* {{{ */
{
	zend_hash_apply_with_argument(EG(class_table), clean_module_class, (void *) &module_number);
}
/* }}} */

static int clean_module_function(zval *el, void *arg) /* {{{ */
{
	zend_function *fe = (zend_function *) Z_PTR_P(el);
	zend_module_entry *module = (zend_module_entry *) arg;
	if (fe->common.type == ZEND_INTERNAL_FUNCTION && fe->internal_function.module == module) {
		return ZEND_HASH_APPLY_REMOVE;
	} else {
		return ZEND_HASH_APPLY_KEEP;
	}
}
/* }}} */

static void clean_module_functions(zend_module_entry *module) /* {{{ */
{
	zend_hash_apply_with_argument(CG(function_table), clean_module_function, module);
}
/* }}} */

void module_destructor(zend_module_entry *module) /* {{{ */
{
#if ZEND_RC_DEBUG
	bool orig_rc_debug = zend_rc_debug;
#endif

	if (module->type == MODULE_TEMPORARY) {
#if ZEND_RC_DEBUG
		/* FIXME: Loading extensions during the request breaks some invariants.
		 * In particular, it will create persistent interned strings, which is
		 * not allowed at this stage. */
		zend_rc_debug = false;
#endif
		zend_clean_module_rsrc_dtors(module->module_number);
		clean_module_constants(module->module_number);
		clean_module_classes(module->module_number);
	}

	if (module->module_started && module->module_shutdown_func) {
#if 0
		zend_printf("%s: Module shutdown\n", module->name);
#endif
		module->module_shutdown_func(module->type, module->module_number);
	}

	if (module->module_started
	 && !module->module_shutdown_func
	 && module->type == MODULE_TEMPORARY) {
		zend_unregister_ini_entries_ex(module->module_number, module->type);
	}

	/* Deinitialize module globals */
	if (module->globals_size) {
#ifdef ZTS
		if (*module->globals_id_ptr) {
			ts_free_id(*module->globals_id_ptr);
		}
#else
		if (module->globals_dtor) {
			module->globals_dtor(module->globals_ptr);
		}
#endif
	}

	module->module_started=0;
	if (module->type == MODULE_TEMPORARY && module->functions) {
		zend_unregister_functions(module->functions, -1, NULL);
		/* Clean functions registered separately from module->functions */
		clean_module_functions(module);
	}

#if HAVE_LIBDL
	if (module->handle && !getenv("ZEND_DONT_UNLOAD_MODULES")) {
		DL_UNLOAD(module->handle);
	}
#endif

#if ZEND_RC_DEBUG
	zend_rc_debug = orig_rc_debug;
#endif
}
/* }}} */

ZEND_API void zend_activate_modules(void) /* {{{ */
{
	zend_module_entry **p = module_request_startup_handlers;

	while (*p) {
		zend_module_entry *module = *p;

		if (module->request_startup_func(module->type, module->module_number)==FAILURE) {
			zend_error(E_WARNING, "request_startup() for %s module failed", module->name);
			exit(1);
		}
		p++;
	}
}
/* }}} */

ZEND_API void zend_deactivate_modules(void) /* {{{ */
{
	EG(current_execute_data) = NULL; /* we're no longer executing anything */

	if (EG(full_tables_cleanup)) {
		zend_module_entry *module;

		ZEND_HASH_MAP_REVERSE_FOREACH_PTR(&module_registry, module) {
			if (module->request_shutdown_func) {
				zend_try {
					module->request_shutdown_func(module->type, module->module_number);
				} zend_end_try();
			}
		} ZEND_HASH_FOREACH_END();
	} else {
		zend_module_entry **p = module_request_shutdown_handlers;

		while (*p) {
			zend_module_entry *module = *p;
			zend_try {
				module->request_shutdown_func(module->type, module->module_number);
			} zend_end_try();
			p++;
		}
	}
}
/* }}} */

ZEND_API void zend_post_deactivate_modules(void) /* {{{ */
{
	if (EG(full_tables_cleanup)) {
		zend_module_entry *module;
		zval *zv;
		zend_string *key;

		ZEND_HASH_MAP_FOREACH_PTR(&module_registry, module) {
			if (module->post_deactivate_func) {
				module->post_deactivate_func();
			}
		} ZEND_HASH_FOREACH_END();
		ZEND_HASH_MAP_REVERSE_FOREACH_STR_KEY_VAL(&module_registry, key, zv) {
			module = Z_PTR_P(zv);
			if (module->type != MODULE_TEMPORARY) {
				break;
			}
			module_destructor(module);
			zend_string_release_ex(key, 0);
		} ZEND_HASH_MAP_FOREACH_END_DEL();
	} else {
		zend_module_entry **p = module_post_deactivate_handlers;

		while (*p) {
			zend_module_entry *module = *p;

			module->post_deactivate_func();
			p++;
		}
	}
}
/* }}} */

/* return the next free module number */
ZEND_API int zend_next_free_module(void) /* {{{ */
{
	return zend_hash_num_elements(&module_registry) + 1;
}
/* }}} */

static zend_class_entry *do_register_internal_class(zend_class_entry *orig_class_entry, uint32_t ce_flags) /* {{{ */
{
	zend_class_entry *class_entry = malloc(sizeof(zend_class_entry));
	zend_string *lowercase_name;
	*class_entry = *orig_class_entry;

	class_entry->type = ZEND_INTERNAL_CLASS;
	zend_initialize_class_data(class_entry, 0);
	zend_alloc_ce_cache(class_entry->name);
	class_entry->ce_flags = orig_class_entry->ce_flags | ce_flags | ZEND_ACC_CONSTANTS_UPDATED | ZEND_ACC_LINKED | ZEND_ACC_RESOLVED_PARENT | ZEND_ACC_RESOLVED_INTERFACES;
	class_entry->info.internal.module = EG(current_module);

	if (class_entry->info.internal.builtin_functions) {
		zend_register_functions(class_entry, class_entry->info.internal.builtin_functions, &class_entry->function_table, EG(current_module)->type);
	}

	lowercase_name = zend_string_tolower_ex(orig_class_entry->name, EG(current_module)->type == MODULE_PERSISTENT);
	lowercase_name = zend_new_interned_string(lowercase_name);
	zend_hash_update_ptr(CG(class_table), lowercase_name, class_entry);
	zend_string_release_ex(lowercase_name, 1);

	if (class_entry->__tostring && !zend_string_equals_literal(class_entry->name, "Stringable")
			&& !(class_entry->ce_flags & ZEND_ACC_TRAIT)) {
		ZEND_ASSERT(zend_ce_stringable
			&& "Should be registered before first class using __toString()");
		zend_do_implement_interface(class_entry, zend_ce_stringable);
	}
	return class_entry;
}
/* }}} */

/* If parent_ce is not NULL then it inherits from parent_ce
 * If parent_ce is NULL and parent_name isn't then it looks for the parent and inherits from it
 * If both parent_ce and parent_name are NULL it does a regular class registration
 * If parent_name is specified but not found NULL is returned
 */
ZEND_API zend_class_entry *zend_register_internal_class_ex(zend_class_entry *class_entry, zend_class_entry *parent_ce) /* {{{ */
{
	zend_class_entry *register_class;

	register_class = zend_register_internal_class(class_entry);

	if (parent_ce) {
		zend_do_inheritance(register_class, parent_ce);
		zend_build_properties_info_table(register_class);
	}

	return register_class;
}
/* }}} */

ZEND_API void zend_class_implements(zend_class_entry *class_entry, int num_interfaces, ...) /* {{{ */
{
	zend_class_entry *interface_entry;
	va_list interface_list;
	va_start(interface_list, num_interfaces);

	while (num_interfaces--) {
		interface_entry = va_arg(interface_list, zend_class_entry *);
		if (interface_entry == zend_ce_stringable
				&& zend_class_implements_interface(class_entry, zend_ce_stringable)) {
			/* Stringable is implemented automatically,
			 * silently ignore an explicit implementation. */
			continue;
		}

		zend_do_implement_interface(class_entry, interface_entry);
	}

	va_end(interface_list);
}
/* }}} */

/* A class that contains at least one abstract method automatically becomes an abstract class.
 */
ZEND_API zend_class_entry *zend_register_internal_class(zend_class_entry *orig_class_entry) /* {{{ */
{
	return do_register_internal_class(orig_class_entry, 0);
}
/* }}} */

ZEND_API zend_class_entry *zend_register_internal_interface(zend_class_entry *orig_class_entry) /* {{{ */
{
	return do_register_internal_class(orig_class_entry, ZEND_ACC_INTERFACE);
}
/* }}} */

ZEND_API zend_result zend_register_class_alias_ex(const char *name, size_t name_len, zend_class_entry *ce, bool persistent) /* {{{ */
{
	zend_string *lcname;
	zval zv, *ret;

	/* TODO: Move this out of here in 7.4. */
	if (persistent && EG(current_module) && EG(current_module)->type == MODULE_TEMPORARY) {
		persistent = 0;
	}

	if (name[0] == '\\') {
		lcname = zend_string_alloc(name_len-1, persistent);
		zend_str_tolower_copy(ZSTR_VAL(lcname), name+1, name_len-1);
	} else {
		lcname = zend_string_alloc(name_len, persistent);
		zend_str_tolower_copy(ZSTR_VAL(lcname), name, name_len);
	}

	zend_assert_valid_class_name(lcname);

	lcname = zend_new_interned_string(lcname);

	/* We cannot increase the refcount of an internal class during request time.
	 * Instead of having to deal with differentiating between class types and lifetimes,
	 * we simply don't increase the refcount of a class entry for aliases.
	 */
	ZVAL_ALIAS_PTR(&zv, ce);

	ret = zend_hash_add(CG(class_table), lcname, &zv);
	zend_string_release_ex(lcname, 0);
	if (ret) {
		// avoid notifying at MINIT time
		if (ce->type == ZEND_USER_CLASS) {
			zend_observer_class_linked_notify(ce, lcname);
		}
		return SUCCESS;
	}
	return FAILURE;
}
/* }}} */

// TODO num_symbol_tables as unsigned int?
ZEND_API zend_result zend_set_hash_symbol(zval *symbol, const char *name, size_t name_length, bool is_ref, int num_symbol_tables, ...) /* {{{ */
{
	HashTable *symbol_table;
	va_list symbol_table_list;

	if (num_symbol_tables <= 0) return FAILURE;

	if (is_ref) {
		ZVAL_MAKE_REF(symbol);
	}

	va_start(symbol_table_list, num_symbol_tables);
	while (num_symbol_tables-- > 0) {
		symbol_table = va_arg(symbol_table_list, HashTable *);
		zend_hash_str_update(symbol_table, name, name_length, symbol);
		Z_TRY_ADDREF_P(symbol);
	}
	va_end(symbol_table_list);
	return SUCCESS;
}
/* }}} */

/* Disabled functions support */

static void zend_disable_function(const char *function_name, size_t function_name_length)
{
	zend_hash_str_del(CG(function_table), function_name, function_name_length);
}

ZEND_API void zend_disable_functions(const char *function_list) /* {{{ */
{
	if (!function_list || !*function_list) {
		return;
	}

	const char *s = NULL, *e = function_list;
	while (*e) {
		switch (*e) {
			case ' ':
			case ',':
				if (s) {
					zend_disable_function(s, e - s);
					s = NULL;
				}
				break;
			default:
				if (!s) {
					s = e;
				}
				break;
		}
		e++;
	}
	if (s) {
		zend_disable_function(s, e - s);
	}

	/* Rehash the function table after deleting functions. This ensures that all internal
	 * functions are contiguous, which means we don't need to perform full table cleanup
	 * on shutdown. */
	zend_hash_rehash(CG(function_table));
}
/* }}} */

#ifdef ZEND_WIN32
#pragma optimize("", off)
#endif
static ZEND_COLD zend_object *display_disabled_class(zend_class_entry *class_type) /* {{{ */
{
	zend_object *intern;

	intern = zend_objects_new(class_type);

	/* Initialize default properties */
	if (EXPECTED(class_type->default_properties_count != 0)) {
		zval *p = intern->properties_table;
		zval *end = p + class_type->default_properties_count;
		do {
			ZVAL_UNDEF(p);
			p++;
		} while (p != end);
	}

	zend_error(E_WARNING, "%s() has been disabled for security reasons", ZSTR_VAL(class_type->name));
	return intern;
}
#ifdef ZEND_WIN32
#pragma optimize("", on)
#endif
/* }}} */

static const zend_function_entry disabled_class_new[] = {
	ZEND_FE_END
};

ZEND_API zend_result zend_disable_class(const char *class_name, size_t class_name_length) /* {{{ */
{
	zend_class_entry *disabled_class;
	zend_string *key;
	zend_function *fn;
	zend_property_info *prop;

	key = zend_string_alloc(class_name_length, 0);
	zend_str_tolower_copy(ZSTR_VAL(key), class_name, class_name_length);
	disabled_class = zend_hash_find_ptr(CG(class_table), key);
	zend_string_release_ex(key, 0);
	if (!disabled_class) {
		return FAILURE;
	}

	/* Will be reset by INIT_CLASS_ENTRY. */
	free(disabled_class->interfaces);

	INIT_CLASS_ENTRY_INIT_METHODS((*disabled_class), disabled_class_new);
	disabled_class->create_object = display_disabled_class;

	ZEND_HASH_MAP_FOREACH_PTR(&disabled_class->function_table, fn) {
		if ((fn->common.fn_flags & (ZEND_ACC_HAS_RETURN_TYPE|ZEND_ACC_HAS_TYPE_HINTS)) &&
			fn->common.scope == disabled_class) {
			zend_free_internal_arg_info(&fn->internal_function);
		}
	} ZEND_HASH_FOREACH_END();
	zend_hash_clean(&disabled_class->function_table);
	ZEND_HASH_MAP_FOREACH_PTR(&disabled_class->properties_info, prop) {
		if (prop->ce == disabled_class) {
			zend_string_release(prop->name);
			zend_type_release(prop->type, /* persistent */ 1);
			free(prop);
		}
	} ZEND_HASH_FOREACH_END();
	zend_hash_clean(&disabled_class->properties_info);
	return SUCCESS;
}
/* }}} */

static zend_always_inline zend_class_entry *get_scope(zend_execute_data *frame)
{
	return frame && frame->func ? frame->func->common.scope : NULL;
}

static bool zend_is_callable_check_class(zend_string *name, zend_class_entry *scope, zend_execute_data *frame, zend_fcall_info_cache *fcc, bool *strict_class, char **error, bool suppress_deprecation) /* {{{ */
{
	bool ret = 0;
	zend_class_entry *ce;
	size_t name_len = ZSTR_LEN(name);
	zend_string *lcname;
	ALLOCA_FLAG(use_heap);

	ZSTR_ALLOCA_ALLOC(lcname, name_len, use_heap);
	zend_str_tolower_copy(ZSTR_VAL(lcname), ZSTR_VAL(name), name_len);

	*strict_class = 0;
	if (zend_string_equals_literal(lcname, "self")) {
		if (!scope) {
			if (error) *error = estrdup("cannot access \"self\" when no class scope is active");
		} else {
			if (!suppress_deprecation) {
				zend_error(E_DEPRECATED, "Use of \"self\" in callables is deprecated");
			}
			fcc->called_scope = zend_get_called_scope(frame);
			if (!fcc->called_scope || !instanceof_function(fcc->called_scope, scope)) {
				fcc->called_scope = scope;
			}
			fcc->calling_scope = scope;
			if (!fcc->object) {
				fcc->object = zend_get_this_object(frame);
			}
			ret = 1;
		}
	} else if (zend_string_equals_literal(lcname, "parent")) {
		if (!scope) {
			if (error) *error = estrdup("cannot access \"parent\" when no class scope is active");
		} else if (!scope->parent) {
			if (error) *error = estrdup("cannot access \"parent\" when current class scope has no parent");
		} else {
			if (!suppress_deprecation) {
				zend_error(E_DEPRECATED, "Use of \"parent\" in callables is deprecated");
			}
			fcc->called_scope = zend_get_called_scope(frame);
			if (!fcc->called_scope || !instanceof_function(fcc->called_scope, scope->parent)) {
				fcc->called_scope = scope->parent;
			}
			fcc->calling_scope = scope->parent;
			if (!fcc->object) {
				fcc->object = zend_get_this_object(frame);
			}
			*strict_class = 1;
			ret = 1;
		}
	} else if (zend_string_equals(lcname, ZSTR_KNOWN(ZEND_STR_STATIC))) {
		zend_class_entry *called_scope = zend_get_called_scope(frame);

		if (!called_scope) {
			if (error) *error = estrdup("cannot access \"static\" when no class scope is active");
		} else {
			if (!suppress_deprecation) {
				zend_error(E_DEPRECATED, "Use of \"static\" in callables is deprecated");
			}
			fcc->called_scope = called_scope;
			fcc->calling_scope = called_scope;
			if (!fcc->object) {
				fcc->object = zend_get_this_object(frame);
			}
			*strict_class = 1;
			ret = 1;
		}
	} else if ((ce = zend_lookup_class(name)) != NULL) {
		zend_class_entry *scope = get_scope(frame);
		fcc->calling_scope = ce;
		if (scope && !fcc->object) {
			zend_object *object = zend_get_this_object(frame);

			if (object &&
			    instanceof_function(object->ce, scope) &&
			    instanceof_function(scope, ce)) {
				fcc->object = object;
				fcc->called_scope = object->ce;
			} else {
				fcc->called_scope = ce;
			}
		} else {
			fcc->called_scope = fcc->object ? fcc->object->ce : ce;
		}
		*strict_class = 1;
		ret = 1;
	} else {
		if (error) zend_spprintf(error, 0, "class \"%.*s\" not found", (int)name_len, ZSTR_VAL(name));
	}
	ZSTR_ALLOCA_FREE(lcname, use_heap);
	return ret;
}
/* }}} */

ZEND_API void zend_release_fcall_info_cache(zend_fcall_info_cache *fcc) {
	if (fcc->function_handler &&
		(fcc->function_handler->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE)) {
		if (fcc->function_handler->common.function_name) {
			zend_string_release_ex(fcc->function_handler->common.function_name, 0);
		}
		zend_free_trampoline(fcc->function_handler);
		fcc->function_handler = NULL;
	}
}

static zend_always_inline bool zend_is_callable_check_func(zval *callable, zend_execute_data *frame, zend_fcall_info_cache *fcc, bool strict_class, char **error, bool suppress_deprecation) /* {{{ */
{
	zend_class_entry *ce_org = fcc->calling_scope;
	bool retval = 0;
	zend_string *mname, *cname;
	zend_string *lmname;
	const char *colon;
	size_t clen;
	HashTable *ftable;
	int call_via_handler = 0;
	zend_class_entry *scope;
	zval *zv;
	ALLOCA_FLAG(use_heap)

	fcc->calling_scope = NULL;

	if (!ce_org) {
		zend_function *func;
		zend_string *lmname;

		/* Check if function with given name exists.
		 * This may be a compound name that includes namespace name */
		if (UNEXPECTED(Z_STRVAL_P(callable)[0] == '\\')) {
			/* Skip leading \ */
			ZSTR_ALLOCA_ALLOC(lmname, Z_STRLEN_P(callable) - 1, use_heap);
			zend_str_tolower_copy(ZSTR_VAL(lmname), Z_STRVAL_P(callable) + 1, Z_STRLEN_P(callable) - 1);
			func = zend_fetch_function(lmname);
			ZSTR_ALLOCA_FREE(lmname, use_heap);
		} else {
			lmname = Z_STR_P(callable);
			func = zend_fetch_function(lmname);
			if (!func) {
				ZSTR_ALLOCA_ALLOC(lmname, Z_STRLEN_P(callable), use_heap);
				zend_str_tolower_copy(ZSTR_VAL(lmname), Z_STRVAL_P(callable), Z_STRLEN_P(callable));
				func = zend_fetch_function(lmname);
				ZSTR_ALLOCA_FREE(lmname, use_heap);
			}
		}
		if (EXPECTED(func != NULL)) {
			fcc->function_handler = func;
			return 1;
		}
	}

	/* Split name into class/namespace and method/function names */
	if ((colon = zend_memrchr(Z_STRVAL_P(callable), ':', Z_STRLEN_P(callable))) != NULL &&
		colon > Z_STRVAL_P(callable) &&
		*(colon-1) == ':'
	) {
		size_t mlen;

		colon--;
		clen = colon - Z_STRVAL_P(callable);
		mlen = Z_STRLEN_P(callable) - clen - 2;

		if (colon == Z_STRVAL_P(callable)) {
			if (error) *error = estrdup("invalid function name");
			return 0;
		}

		/* This is a compound name.
		 * Try to fetch class and then find static method. */
		if (ce_org) {
			scope = ce_org;
		} else {
			scope = get_scope(frame);
		}

		cname = zend_string_init_interned(Z_STRVAL_P(callable), clen, 0);
		if (ZSTR_HAS_CE_CACHE(cname) && ZSTR_GET_CE_CACHE(cname)) {
			fcc->calling_scope = ZSTR_GET_CE_CACHE(cname);
			if (scope && !fcc->object) {
				zend_object *object = zend_get_this_object(frame);

				if (object &&
				    instanceof_function(object->ce, scope) &&
				    instanceof_function(scope, fcc->calling_scope)) {
					fcc->object = object;
					fcc->called_scope = object->ce;
				} else {
					fcc->called_scope = fcc->calling_scope;
				}
			} else {
				fcc->called_scope = fcc->object ? fcc->object->ce : fcc->calling_scope;
			}
			strict_class = 1;
		} else if (!zend_is_callable_check_class(cname, scope, frame, fcc, &strict_class, error, suppress_deprecation || ce_org != NULL)) {
			zend_string_release_ex(cname, 0);
			return 0;
		}
		zend_string_release_ex(cname, 0);

		ftable = &fcc->calling_scope->function_table;
		if (ce_org && !instanceof_function(ce_org, fcc->calling_scope)) {
			if (error) zend_spprintf(error, 0, "class %s is not a subclass of %s", ZSTR_VAL(ce_org->name), ZSTR_VAL(fcc->calling_scope->name));
			return 0;
		}
		if (ce_org && !suppress_deprecation) {
			zend_error(E_DEPRECATED,
				"Callables of the form [\"%s\", \"%s\"] are deprecated",
				ZSTR_VAL(ce_org->name), Z_STRVAL_P(callable));
		}
		mname = zend_string_init(Z_STRVAL_P(callable) + clen + 2, mlen, 0);
	} else if (ce_org) {
		/* Try to fetch find static method of given class. */
		mname = Z_STR_P(callable);
		zend_string_addref(mname);
		ftable = &ce_org->function_table;
		fcc->calling_scope = ce_org;
	} else {
		/* We already checked for plain function before. */
		if (error) {
			zend_spprintf(error, 0, "function \"%s\" not found or invalid function name", Z_STRVAL_P(callable));
		}
		return 0;
	}

	lmname = zend_string_tolower(mname);
	if (strict_class &&
	    fcc->calling_scope &&
		zend_string_equals_literal(lmname, ZEND_CONSTRUCTOR_FUNC_NAME)) {
		fcc->function_handler = fcc->calling_scope->constructor;
		if (fcc->function_handler) {
			retval = 1;
		}
	} else if ((zv = zend_hash_find(ftable, lmname)) != NULL) {
		fcc->function_handler = Z_PTR_P(zv);
		retval = 1;
		if ((fcc->function_handler->op_array.fn_flags & ZEND_ACC_CHANGED) &&
		    !strict_class) {
			scope = get_scope(frame);
			if (scope &&
			    instanceof_function(fcc->function_handler->common.scope, scope)) {

				zv = zend_hash_find(&scope->function_table, lmname);
				if (zv != NULL) {
					zend_function *priv_fbc = Z_PTR_P(zv);

					if ((priv_fbc->common.fn_flags & ZEND_ACC_PRIVATE)
					 && priv_fbc->common.scope == scope) {
						fcc->function_handler = priv_fbc;
					}
				}
			}
		}
		if (!(fcc->function_handler->common.fn_flags & ZEND_ACC_PUBLIC) &&
		    (fcc->calling_scope &&
		     ((fcc->object && fcc->calling_scope->__call) ||
		      (!fcc->object && fcc->calling_scope->__callstatic)))) {
			scope = get_scope(frame);
			if (fcc->function_handler->common.scope != scope) {
				if ((fcc->function_handler->common.fn_flags & ZEND_ACC_PRIVATE)
				 || !zend_check_protected(zend_get_function_root_class(fcc->function_handler), scope)) {
					retval = 0;
					fcc->function_handler = NULL;
					goto get_function_via_handler;
				}
			}
		}
	} else {
get_function_via_handler:
		if (fcc->object && fcc->calling_scope == ce_org) {
			if (strict_class && ce_org->__call) {
				fcc->function_handler = zend_get_call_trampoline_func(ce_org, mname, 0);
				call_via_handler = 1;
				retval = 1;
			} else {
				fcc->function_handler = fcc->object->handlers->get_method(&fcc->object, mname, NULL);
				if (fcc->function_handler) {
					if (strict_class &&
					    (!fcc->function_handler->common.scope ||
					     !instanceof_function(ce_org, fcc->function_handler->common.scope))) {
						zend_release_fcall_info_cache(fcc);
					} else {
						retval = 1;
						call_via_handler = (fcc->function_handler->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) != 0;
					}
				}
			}
		} else if (fcc->calling_scope) {
			if (fcc->calling_scope->get_static_method) {
				fcc->function_handler = fcc->calling_scope->get_static_method(fcc->calling_scope, mname);
			} else {
				fcc->function_handler = zend_std_get_static_method(fcc->calling_scope, mname, NULL);
			}
			if (fcc->function_handler) {
				retval = 1;
				call_via_handler = (fcc->function_handler->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) != 0;
				if (call_via_handler && !fcc->object) {
					zend_object *object = zend_get_this_object(frame);
					if (object &&
					    instanceof_function(object->ce, fcc->calling_scope)) {
						fcc->object = object;
					}
				}
			}
		}
	}

	if (retval) {
		if (fcc->calling_scope && !call_via_handler) {
			if (fcc->function_handler->common.fn_flags & ZEND_ACC_ABSTRACT) {
				retval = 0;
				if (error) {
					zend_spprintf(error, 0, "cannot call abstract method %s::%s()", ZSTR_VAL(fcc->calling_scope->name), ZSTR_VAL(fcc->function_handler->common.function_name));
				}
			} else if (!fcc->object && !(fcc->function_handler->common.fn_flags & ZEND_ACC_STATIC)) {
				retval = 0;
				if (error) {
					zend_spprintf(error, 0, "non-static method %s::%s() cannot be called statically", ZSTR_VAL(fcc->calling_scope->name), ZSTR_VAL(fcc->function_handler->common.function_name));
				}
			}
			if (retval
			 && !(fcc->function_handler->common.fn_flags & ZEND_ACC_PUBLIC)) {
				scope = get_scope(frame);
				if (fcc->function_handler->common.scope != scope) {
					if ((fcc->function_handler->common.fn_flags & ZEND_ACC_PRIVATE)
					 || (!zend_check_protected(zend_get_function_root_class(fcc->function_handler), scope))) {
						if (error) {
							if (*error) {
								efree(*error);
							}
							zend_spprintf(error, 0, "cannot access %s method %s::%s()", zend_visibility_string(fcc->function_handler->common.fn_flags), ZSTR_VAL(fcc->calling_scope->name), ZSTR_VAL(fcc->function_handler->common.function_name));
						}
						retval = 0;
					}
				}
			}
		}
	} else if (error) {
		if (fcc->calling_scope) {
			zend_spprintf(error, 0, "class %s does not have a method \"%s\"", ZSTR_VAL(fcc->calling_scope->name), ZSTR_VAL(mname));
		} else {
			zend_spprintf(error, 0, "function %s() does not exist", ZSTR_VAL(mname));
		}
	}
	zend_string_release_ex(lmname, 0);
	zend_string_release_ex(mname, 0);

	if (fcc->object) {
		fcc->called_scope = fcc->object->ce;
		if (fcc->function_handler
		 && (fcc->function_handler->common.fn_flags & ZEND_ACC_STATIC)) {
			fcc->object = NULL;
		}
	}
	return retval;
}
/* }}} */

ZEND_API zend_string *zend_get_callable_name_ex(zval *callable, zend_object *object) /* {{{ */
{
try_again:
	switch (Z_TYPE_P(callable)) {
		case IS_STRING:
			if (object) {
				return zend_create_member_string(object->ce->name, Z_STR_P(callable));
			}
			return zend_string_copy(Z_STR_P(callable));

		case IS_ARRAY:
		{
			zval *method = NULL;
			zval *obj = NULL;

			if (zend_hash_num_elements(Z_ARRVAL_P(callable)) == 2) {
				obj = zend_hash_index_find_deref(Z_ARRVAL_P(callable), 0);
				method = zend_hash_index_find_deref(Z_ARRVAL_P(callable), 1);
			}

			if (obj == NULL || method == NULL || Z_TYPE_P(method) != IS_STRING) {
				return ZSTR_KNOWN(ZEND_STR_ARRAY_CAPITALIZED);
			}

			if (Z_TYPE_P(obj) == IS_STRING) {
				return zend_create_member_string(Z_STR_P(obj), Z_STR_P(method));
			} else if (Z_TYPE_P(obj) == IS_OBJECT) {
				return zend_create_member_string(Z_OBJCE_P(obj)->name, Z_STR_P(method));
			} else {
				return ZSTR_KNOWN(ZEND_STR_ARRAY_CAPITALIZED);
			}
		}
		case IS_OBJECT:
		{
			zend_class_entry *ce = Z_OBJCE_P(callable);
			return zend_string_concat2(
				ZSTR_VAL(ce->name), ZSTR_LEN(ce->name),
				"::__invoke", sizeof("::__invoke") - 1);
		}
		case IS_REFERENCE:
			callable = Z_REFVAL_P(callable);
			goto try_again;
		default:
			return zval_get_string_func(callable);
	}
}
/* }}} */

ZEND_API zend_string *zend_get_callable_name(zval *callable) /* {{{ */
{
	return zend_get_callable_name_ex(callable, NULL);
}
/* }}} */

ZEND_API bool zend_is_callable_at_frame(
		zval *callable, zend_object *object, zend_execute_data *frame,
		uint32_t check_flags, zend_fcall_info_cache *fcc, char **error) /* {{{ */
{
	bool ret;
	zend_fcall_info_cache fcc_local;
	bool strict_class = 0;

	if (fcc == NULL) {
		fcc = &fcc_local;
	}
	if (error) {
		*error = NULL;
	}

	fcc->calling_scope = NULL;
	fcc->called_scope = NULL;
	fcc->function_handler = NULL;
	fcc->object = NULL;
	fcc->closure = NULL;

again:
	switch (Z_TYPE_P(callable)) {
		case IS_STRING:
			if (object) {
				fcc->object = object;
				fcc->calling_scope = object->ce;
			}

			if (check_flags & IS_CALLABLE_CHECK_SYNTAX_ONLY) {
				fcc->called_scope = fcc->calling_scope;
				return 1;
			}

check_func:
			ret = zend_is_callable_check_func(callable, frame, fcc, strict_class, error, check_flags & IS_CALLABLE_SUPPRESS_DEPRECATIONS);
			if (fcc == &fcc_local) {
				zend_release_fcall_info_cache(fcc);
			}
			return ret;

		case IS_ARRAY:
			{
				if (zend_hash_num_elements(Z_ARRVAL_P(callable)) != 2) {
					if (error) *error = estrdup("array callback must have exactly two members");
					return 0;
				}

				zval *obj = zend_hash_index_find(Z_ARRVAL_P(callable), 0);
				zval *method = zend_hash_index_find(Z_ARRVAL_P(callable), 1);
				if (!obj || !method) {
					if (error) *error = estrdup("array callback has to contain indices 0 and 1");
					return 0;
				}

				ZVAL_DEREF(obj);
				if (Z_TYPE_P(obj) != IS_STRING && Z_TYPE_P(obj) != IS_OBJECT) {
					if (error) *error = estrdup("first array member is not a valid class name or object");
					return 0;
				}

				ZVAL_DEREF(method);
				if (Z_TYPE_P(method) != IS_STRING) {
					if (error) *error = estrdup("second array member is not a valid method");
					return 0;
				}

				if (Z_TYPE_P(obj) == IS_STRING) {
					if (check_flags & IS_CALLABLE_CHECK_SYNTAX_ONLY) {
						return 1;
					}

					if (!zend_is_callable_check_class(Z_STR_P(obj), get_scope(frame), frame, fcc, &strict_class, error, check_flags & IS_CALLABLE_SUPPRESS_DEPRECATIONS)) {
						return 0;
					}
				} else {
					ZEND_ASSERT(Z_TYPE_P(obj) == IS_OBJECT);
					fcc->calling_scope = Z_OBJCE_P(obj); /* TBFixed: what if it's overloaded? */
					fcc->object = Z_OBJ_P(obj);

					if (check_flags & IS_CALLABLE_CHECK_SYNTAX_ONLY) {
						fcc->called_scope = fcc->calling_scope;
						return 1;
					}
				}

				callable = method;
				goto check_func;
			}
			return 0;
		case IS_OBJECT:
			if (Z_OBJ_HANDLER_P(callable, get_closure) && Z_OBJ_HANDLER_P(callable, get_closure)(Z_OBJ_P(callable), &fcc->calling_scope, &fcc->function_handler, &fcc->object, 1) == SUCCESS) {
				fcc->called_scope = fcc->calling_scope;
				fcc->closure = Z_OBJ_P(callable);
				if (fcc == &fcc_local) {
					zend_release_fcall_info_cache(fcc);
				}
				return 1;
			}
			if (error) *error = estrdup("no array or string given");
			return 0;
		case IS_REFERENCE:
			callable = Z_REFVAL_P(callable);
			goto again;
		default:
			if (error) *error = estrdup("no array or string given");
			return 0;
	}
}
/* }}} */

ZEND_API bool zend_is_callable_ex(zval *callable, zend_object *object, uint32_t check_flags, zend_string **callable_name, zend_fcall_info_cache *fcc, char **error) /* {{{ */
{
	/* Determine callability at the first parent user frame. */
	zend_execute_data *frame = EG(current_execute_data);
	while (frame && (!frame->func || !ZEND_USER_CODE(frame->func->type))) {
		frame = frame->prev_execute_data;
	}

	bool ret = zend_is_callable_at_frame(callable, object, frame, check_flags, fcc, error);
	if (callable_name) {
		*callable_name = zend_get_callable_name_ex(callable, object);
	}
	return ret;
}

ZEND_API bool zend_is_callable(zval *callable, uint32_t check_flags, zend_string **callable_name) /* {{{ */
{
	return zend_is_callable_ex(callable, NULL, check_flags, callable_name, NULL, NULL);
}
/* }}} */

ZEND_API bool zend_make_callable(zval *callable, zend_string **callable_name) /* {{{ */
{
	zend_fcall_info_cache fcc;

	if (zend_is_callable_ex(callable, NULL, IS_CALLABLE_SUPPRESS_DEPRECATIONS, callable_name, &fcc, NULL)) {
		if (Z_TYPE_P(callable) == IS_STRING && fcc.calling_scope) {
			zval_ptr_dtor_str(callable);
			array_init(callable);
			add_next_index_str(callable, zend_string_copy(fcc.calling_scope->name));
			add_next_index_str(callable, zend_string_copy(fcc.function_handler->common.function_name));
		}
		zend_release_fcall_info_cache(&fcc);
		return 1;
	}
	return 0;
}
/* }}} */

ZEND_API zend_result zend_fcall_info_init(zval *callable, uint32_t check_flags, zend_fcall_info *fci, zend_fcall_info_cache *fcc, zend_string **callable_name, char **error) /* {{{ */
{
	if (!zend_is_callable_ex(callable, NULL, check_flags, callable_name, fcc, error)) {
		return FAILURE;
	}

	fci->size = sizeof(*fci);
	fci->object = fcc->object;
	ZVAL_COPY_VALUE(&fci->function_name, callable);
	fci->retval = NULL;
	fci->param_count = 0;
	fci->params = NULL;
	fci->named_params = NULL;

	return SUCCESS;
}
/* }}} */

ZEND_API void zend_fcall_info_args_clear(zend_fcall_info *fci, bool free_mem) /* {{{ */
{
	if (fci->params) {
		zval *p = fci->params;
		zval *end = p + fci->param_count;

		while (p != end) {
			i_zval_ptr_dtor(p);
			p++;
		}
		if (free_mem) {
			efree(fci->params);
			fci->params = NULL;
		}
	}
	fci->param_count = 0;
}
/* }}} */

ZEND_API zend_result zend_fcall_info_args_ex(zend_fcall_info *fci, zend_function *func, zval *args) /* {{{ */
{
	zval *arg, *params;
	uint32_t n = 1;

	zend_fcall_info_args_clear(fci, !args);

	if (!args) {
		return SUCCESS;
	}

	if (Z_TYPE_P(args) != IS_ARRAY) {
		return FAILURE;
	}

	fci->param_count = zend_hash_num_elements(Z_ARRVAL_P(args));
	fci->params = params = (zval *) erealloc(fci->params, fci->param_count * sizeof(zval));

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(args), arg) {
		if (func && !Z_ISREF_P(arg) && ARG_SHOULD_BE_SENT_BY_REF(func, n)) {
			ZVAL_NEW_REF(params, arg);
			Z_TRY_ADDREF_P(arg);
		} else {
			ZVAL_COPY(params, arg);
		}
		params++;
		n++;
	} ZEND_HASH_FOREACH_END();

	return SUCCESS;
}
/* }}} */

ZEND_API zend_result zend_fcall_info_args(zend_fcall_info *fci, zval *args) /* {{{ */
{
	return zend_fcall_info_args_ex(fci, NULL, args);
}
/* }}} */

ZEND_API void zend_get_callable_zval_from_fcc(const zend_fcall_info_cache *fcc, zval *callable)
{
	if (fcc->closure) {
		ZVAL_OBJ_COPY(callable, fcc->closure);
	} else if (fcc->function_handler->common.scope) {
		array_init(callable);
		if (fcc->object) {
			GC_ADDREF(fcc->object);
			add_next_index_object(callable, fcc->object);
		} else {
			add_next_index_str(callable, zend_string_copy(fcc->calling_scope->name));
		}
		add_next_index_str(callable, zend_string_copy(fcc->function_handler->common.function_name));
	} else {
		ZVAL_STR_COPY(callable, fcc->function_handler->common.function_name);
	}
}

ZEND_API const char *zend_get_module_version(const char *module_name) /* {{{ */
{
	zend_string *lname;
	size_t name_len = strlen(module_name);
	zend_module_entry *module;

	lname = zend_string_alloc(name_len, 0);
	zend_str_tolower_copy(ZSTR_VAL(lname), module_name, name_len);
	module = zend_hash_find_ptr(&module_registry, lname);
	zend_string_efree(lname);
	return module ? module->version : NULL;
}
/* }}} */

static zend_always_inline bool is_persistent_class(zend_class_entry *ce) {
	return (ce->type & ZEND_INTERNAL_CLASS)
		&& ce->info.internal.module->type == MODULE_PERSISTENT;
}

ZEND_API zend_property_info *zend_declare_typed_property(zend_class_entry *ce, zend_string *name, zval *property, int access_type, zend_string *doc_comment, zend_type type) /* {{{ */
{
	zend_property_info *property_info, *property_info_ptr;

	if (ZEND_TYPE_IS_SET(type)) {
		ce->ce_flags |= ZEND_ACC_HAS_TYPE_HINTS;

		if (access_type & ZEND_ACC_READONLY) {
			ce->ce_flags |= ZEND_ACC_HAS_READONLY_PROPS;
		}
	}

	if (ce->type == ZEND_INTERNAL_CLASS) {
		property_info = pemalloc(sizeof(zend_property_info), 1);
	} else {
		property_info = zend_arena_alloc(&CG(arena), sizeof(zend_property_info));
		if (Z_TYPE_P(property) == IS_CONSTANT_AST) {
			ce->ce_flags &= ~ZEND_ACC_CONSTANTS_UPDATED;
			if (access_type & ZEND_ACC_STATIC) {
				ce->ce_flags |= ZEND_ACC_HAS_AST_STATICS;
			} else {
				ce->ce_flags |= ZEND_ACC_HAS_AST_PROPERTIES;
			}
		}
	}

	if (Z_TYPE_P(property) == IS_STRING && !ZSTR_IS_INTERNED(Z_STR_P(property))) {
		zval_make_interned_string(property);
	}

	if (!(access_type & ZEND_ACC_PPP_MASK)) {
		access_type |= ZEND_ACC_PUBLIC;
	}
	if (access_type & ZEND_ACC_STATIC) {
		if ((property_info_ptr = zend_hash_find_ptr(&ce->properties_info, name)) != NULL &&
		    (property_info_ptr->flags & ZEND_ACC_STATIC) != 0) {
			property_info->offset = property_info_ptr->offset;
			zval_ptr_dtor(&ce->default_static_members_table[property_info->offset]);
			zend_hash_del(&ce->properties_info, name);
		} else {
			property_info->offset = ce->default_static_members_count++;
			ce->default_static_members_table = perealloc(ce->default_static_members_table, sizeof(zval) * ce->default_static_members_count, ce->type == ZEND_INTERNAL_CLASS);
		}
		ZVAL_COPY_VALUE(&ce->default_static_members_table[property_info->offset], property);
		if (!ZEND_MAP_PTR(ce->static_members_table)) {
			if (ce->type == ZEND_INTERNAL_CLASS &&
					ce->info.internal.module->type == MODULE_PERSISTENT) {
				ZEND_MAP_PTR_NEW(ce->static_members_table);
			}
		}
	} else {
		zval *property_default_ptr;
		if ((property_info_ptr = zend_hash_find_ptr(&ce->properties_info, name)) != NULL &&
		    (property_info_ptr->flags & ZEND_ACC_STATIC) == 0) {
			property_info->offset = property_info_ptr->offset;
			zval_ptr_dtor(&ce->default_properties_table[OBJ_PROP_TO_NUM(property_info->offset)]);
			zend_hash_del(&ce->properties_info, name);

			ZEND_ASSERT(ce->type == ZEND_INTERNAL_CLASS);
			ZEND_ASSERT(ce->properties_info_table != NULL);
			ce->properties_info_table[OBJ_PROP_TO_NUM(property_info->offset)] = property_info;
		} else {
			property_info->offset = OBJ_PROP_TO_OFFSET(ce->default_properties_count);
			ce->default_properties_count++;
			ce->default_properties_table = perealloc(ce->default_properties_table, sizeof(zval) * ce->default_properties_count, ce->type == ZEND_INTERNAL_CLASS);

			/* For user classes this is handled during linking */
			if (ce->type == ZEND_INTERNAL_CLASS) {
				ce->properties_info_table = perealloc(ce->properties_info_table, sizeof(zend_property_info *) * ce->default_properties_count, 1);
				ce->properties_info_table[ce->default_properties_count - 1] = property_info;
			}
		}
		property_default_ptr = &ce->default_properties_table[OBJ_PROP_TO_NUM(property_info->offset)];
		ZVAL_COPY_VALUE(property_default_ptr, property);
		Z_PROP_FLAG_P(property_default_ptr) = Z_ISUNDEF_P(property) ? IS_PROP_UNINIT : 0;
	}
	if (ce->type & ZEND_INTERNAL_CLASS) {
		/* Must be interned to avoid ZTS data races */
		if (is_persistent_class(ce)) {
			name = zend_new_interned_string(zend_string_copy(name));
		}

		if (Z_REFCOUNTED_P(property)) {
			zend_error_noreturn(E_CORE_ERROR, "Internal zvals cannot be refcounted");
		}
	}

	if (access_type & ZEND_ACC_PUBLIC) {
		property_info->name = zend_string_copy(name);
	} else if (access_type & ZEND_ACC_PRIVATE) {
		property_info->name = zend_mangle_property_name(ZSTR_VAL(ce->name), ZSTR_LEN(ce->name), ZSTR_VAL(name), ZSTR_LEN(name), is_persistent_class(ce));
	} else {
		ZEND_ASSERT(access_type & ZEND_ACC_PROTECTED);
		property_info->name = zend_mangle_property_name("*", 1, ZSTR_VAL(name), ZSTR_LEN(name), is_persistent_class(ce));
	}

	property_info->name = zend_new_interned_string(property_info->name);
	property_info->flags = access_type;
	property_info->doc_comment = doc_comment;
	property_info->attributes = NULL;
	property_info->ce = ce;
	property_info->type = type;

	if (is_persistent_class(ce)) {
		zend_normalize_internal_type(&property_info->type);
	}

	zend_hash_update_ptr(&ce->properties_info, name, property_info);

	return property_info;
}
/* }}} */

ZEND_API zend_result zend_try_assign_typed_ref_ex(zend_reference *ref, zval *val, bool strict) /* {{{ */
{
	if (UNEXPECTED(!zend_verify_ref_assignable_zval(ref, val, strict))) {
		zval_ptr_dtor(val);
		return FAILURE;
	} else {
		zval_ptr_dtor(&ref->val);
		ZVAL_COPY_VALUE(&ref->val, val);
		return SUCCESS;
	}
}
/* }}} */

ZEND_API zend_result zend_try_assign_typed_ref(zend_reference *ref, zval *val) /* {{{ */
{
	return zend_try_assign_typed_ref_ex(ref, val, ZEND_ARG_USES_STRICT_TYPES());
}
/* }}} */

ZEND_API zend_result zend_try_assign_typed_ref_null(zend_reference *ref) /* {{{ */
{
	zval tmp;

	ZVAL_NULL(&tmp);
	return zend_try_assign_typed_ref(ref, &tmp);
}
/* }}} */

ZEND_API zend_result zend_try_assign_typed_ref_bool(zend_reference *ref, bool val) /* {{{ */
{
	zval tmp;

	ZVAL_BOOL(&tmp, val);
	return zend_try_assign_typed_ref(ref, &tmp);
}
/* }}} */

ZEND_API zend_result zend_try_assign_typed_ref_long(zend_reference *ref, zend_long lval) /* {{{ */
{
	zval tmp;

	ZVAL_LONG(&tmp, lval);
	return zend_try_assign_typed_ref(ref, &tmp);
}
/* }}} */

ZEND_API zend_result zend_try_assign_typed_ref_double(zend_reference *ref, double dval) /* {{{ */
{
	zval tmp;

	ZVAL_DOUBLE(&tmp, dval);
	return zend_try_assign_typed_ref(ref, &tmp);
}
/* }}} */

ZEND_API zend_result zend_try_assign_typed_ref_empty_string(zend_reference *ref) /* {{{ */
{
	zval tmp;

	ZVAL_EMPTY_STRING(&tmp);
	return zend_try_assign_typed_ref(ref, &tmp);
}
/* }}} */

ZEND_API zend_result zend_try_assign_typed_ref_str(zend_reference *ref, zend_string *str) /* {{{ */
{
	zval tmp;

	ZVAL_STR(&tmp, str);
	return zend_try_assign_typed_ref(ref, &tmp);
}
/* }}} */

ZEND_API zend_result zend_try_assign_typed_ref_string(zend_reference *ref, const char *string) /* {{{ */
{
	zval tmp;

	ZVAL_STRING(&tmp, string);
	return zend_try_assign_typed_ref(ref, &tmp);
}
/* }}} */

ZEND_API zend_result zend_try_assign_typed_ref_stringl(zend_reference *ref, const char *string, size_t len) /* {{{ */
{
	zval tmp;

	ZVAL_STRINGL(&tmp, string, len);
	return zend_try_assign_typed_ref(ref, &tmp);
}
/* }}} */

ZEND_API zend_result zend_try_assign_typed_ref_arr(zend_reference *ref, zend_array *arr) /* {{{ */
{
	zval tmp;

	ZVAL_ARR(&tmp, arr);
	return zend_try_assign_typed_ref(ref, &tmp);
}
/* }}} */

ZEND_API zend_result zend_try_assign_typed_ref_res(zend_reference *ref, zend_resource *res) /* {{{ */
{
	zval tmp;

	ZVAL_RES(&tmp, res);
	return zend_try_assign_typed_ref(ref, &tmp);
}
/* }}} */

ZEND_API zend_result zend_try_assign_typed_ref_zval(zend_reference *ref, zval *zv) /* {{{ */
{
	zval tmp;

	ZVAL_COPY_VALUE(&tmp, zv);
	return zend_try_assign_typed_ref(ref, &tmp);
}
/* }}} */

ZEND_API zend_result zend_try_assign_typed_ref_zval_ex(zend_reference *ref, zval *zv, bool strict) /* {{{ */
{
	zval tmp;

	ZVAL_COPY_VALUE(&tmp, zv);
	return zend_try_assign_typed_ref_ex(ref, &tmp, strict);
}
/* }}} */

ZEND_API void zend_declare_property_ex(zend_class_entry *ce, zend_string *name, zval *property, int access_type, zend_string *doc_comment) /* {{{ */
{
	zend_declare_typed_property(ce, name, property, access_type, doc_comment, (zend_type) ZEND_TYPE_INIT_NONE(0));
}
/* }}} */

ZEND_API void zend_declare_property(zend_class_entry *ce, const char *name, size_t name_length, zval *property, int access_type) /* {{{ */
{
	zend_string *key = zend_string_init(name, name_length, is_persistent_class(ce));
	zend_declare_property_ex(ce, key, property, access_type, NULL);
	zend_string_release(key);
}
/* }}} */

ZEND_API void zend_declare_property_null(zend_class_entry *ce, const char *name, size_t name_length, int access_type) /* {{{ */
{
	zval property;

	ZVAL_NULL(&property);
	zend_declare_property(ce, name, name_length, &property, access_type);
}
/* }}} */

ZEND_API void zend_declare_property_bool(zend_class_entry *ce, const char *name, size_t name_length, zend_long value, int access_type) /* {{{ */
{
	zval property;

	ZVAL_BOOL(&property, value);
	zend_declare_property(ce, name, name_length, &property, access_type);
}
/* }}} */

ZEND_API void zend_declare_property_long(zend_class_entry *ce, const char *name, size_t name_length, zend_long value, int access_type) /* {{{ */
{
	zval property;

	ZVAL_LONG(&property, value);
	zend_declare_property(ce, name, name_length, &property, access_type);
}
/* }}} */

ZEND_API void zend_declare_property_double(zend_class_entry *ce, const char *name, size_t name_length, double value, int access_type) /* {{{ */
{
	zval property;

	ZVAL_DOUBLE(&property, value);
	zend_declare_property(ce, name, name_length, &property, access_type);
}
/* }}} */

ZEND_API void zend_declare_property_string(zend_class_entry *ce, const char *name, size_t name_length, const char *value, int access_type) /* {{{ */
{
	zval property;

	ZVAL_NEW_STR(&property, zend_string_init(value, strlen(value), ce->type & ZEND_INTERNAL_CLASS));
	zend_declare_property(ce, name, name_length, &property, access_type);
}
/* }}} */

ZEND_API void zend_declare_property_stringl(zend_class_entry *ce, const char *name, size_t name_length, const char *value, size_t value_len, int access_type) /* {{{ */
{
	zval property;

	ZVAL_NEW_STR(&property, zend_string_init(value, value_len, ce->type & ZEND_INTERNAL_CLASS));
	zend_declare_property(ce, name, name_length, &property, access_type);
}
/* }}} */

ZEND_API zend_class_constant *zend_declare_typed_class_constant(zend_class_entry *ce, zend_string *name, zval *value, int flags, zend_string *doc_comment, zend_type type) /* {{{ */
{
	zend_class_constant *c;

	if (ce->ce_flags & ZEND_ACC_INTERFACE) {
		if (!(flags & ZEND_ACC_PUBLIC)) {
			zend_error_noreturn(E_COMPILE_ERROR, "Access type for interface constant %s::%s must be public", ZSTR_VAL(ce->name), ZSTR_VAL(name));
		}
	}

	if (zend_string_equals_ci(name, ZSTR_KNOWN(ZEND_STR_CLASS))) {
		zend_error_noreturn(ce->type == ZEND_INTERNAL_CLASS ? E_CORE_ERROR : E_COMPILE_ERROR,
				"A class constant must not be called 'class'; it is reserved for class name fetching");
	}

	if (Z_TYPE_P(value) == IS_STRING && !ZSTR_IS_INTERNED(Z_STR_P(value))) {
		zval_make_interned_string(value);
	}

	if (ce->type == ZEND_INTERNAL_CLASS) {
		c = pemalloc(sizeof(zend_class_constant), 1);
	} else {
		c = zend_arena_alloc(&CG(arena), sizeof(zend_class_constant));
	}
	ZVAL_COPY_VALUE(&c->value, value);
	ZEND_CLASS_CONST_FLAGS(c) = flags;
	c->doc_comment = doc_comment;
	c->attributes = NULL;
	c->ce = ce;
	c->type = type;

	if (Z_TYPE_P(value) == IS_CONSTANT_AST) {
		ce->ce_flags &= ~ZEND_ACC_CONSTANTS_UPDATED;
		ce->ce_flags |= ZEND_ACC_HAS_AST_CONSTANTS;
		if (ce->type == ZEND_INTERNAL_CLASS && !ZEND_MAP_PTR(ce->mutable_data)) {
			ZEND_MAP_PTR_NEW(ce->mutable_data);
		}
	}

	if (!zend_hash_add_ptr(&ce->constants_table, name, c)) {
		zend_error_noreturn(ce->type == ZEND_INTERNAL_CLASS ? E_CORE_ERROR : E_COMPILE_ERROR,
			"Cannot redefine class constant %s::%s", ZSTR_VAL(ce->name), ZSTR_VAL(name));
	}

	return c;
}

ZEND_API zend_class_constant *zend_declare_class_constant_ex(zend_class_entry *ce, zend_string *name, zval *value, int flags, zend_string *doc_comment)
{
	return zend_declare_typed_class_constant(ce, name, value, flags, doc_comment, (zend_type) ZEND_TYPE_INIT_NONE(0));
}

ZEND_API void zend_declare_class_constant(zend_class_entry *ce, const char *name, size_t name_length, zval *value) /* {{{ */
{
	zend_string *key;

	if (ce->type == ZEND_INTERNAL_CLASS) {
		key = zend_string_init_interned(name, name_length, 1);
	} else {
		key = zend_string_init(name, name_length, 0);
	}
	zend_declare_class_constant_ex(ce, key, value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(key);
}
/* }}} */

ZEND_API void zend_declare_class_constant_null(zend_class_entry *ce, const char *name, size_t name_length) /* {{{ */
{
	zval constant;

	ZVAL_NULL(&constant);
	zend_declare_class_constant(ce, name, name_length, &constant);
}
/* }}} */

ZEND_API void zend_declare_class_constant_long(zend_class_entry *ce, const char *name, size_t name_length, zend_long value) /* {{{ */
{
	zval constant;

	ZVAL_LONG(&constant, value);
	zend_declare_class_constant(ce, name, name_length, &constant);
}
/* }}} */

ZEND_API void zend_declare_class_constant_bool(zend_class_entry *ce, const char *name, size_t name_length, bool value) /* {{{ */
{
	zval constant;

	ZVAL_BOOL(&constant, value);
	zend_declare_class_constant(ce, name, name_length, &constant);
}
/* }}} */

ZEND_API void zend_declare_class_constant_double(zend_class_entry *ce, const char *name, size_t name_length, double value) /* {{{ */
{
	zval constant;

	ZVAL_DOUBLE(&constant, value);
	zend_declare_class_constant(ce, name, name_length, &constant);
}
/* }}} */

ZEND_API void zend_declare_class_constant_stringl(zend_class_entry *ce, const char *name, size_t name_length, const char *value, size_t value_length) /* {{{ */
{
	zval constant;

	ZVAL_NEW_STR(&constant, zend_string_init(value, value_length, ce->type & ZEND_INTERNAL_CLASS));
	zend_declare_class_constant(ce, name, name_length, &constant);
}
/* }}} */

ZEND_API void zend_declare_class_constant_string(zend_class_entry *ce, const char *name, size_t name_length, const char *value) /* {{{ */
{
	zend_declare_class_constant_stringl(ce, name, name_length, value, strlen(value));
}
/* }}} */

ZEND_API void zend_update_property_ex(zend_class_entry *scope, zend_object *object, zend_string *name, zval *value) /* {{{ */
{
	zend_class_entry *old_scope = EG(fake_scope);

	EG(fake_scope) = scope;

	object->handlers->write_property(object, name, value, NULL);

	EG(fake_scope) = old_scope;
}
/* }}} */

ZEND_API void zend_update_property(zend_class_entry *scope, zend_object *object, const char *name, size_t name_length, zval *value) /* {{{ */
{
	zend_string *property;
	zend_class_entry *old_scope = EG(fake_scope);

	EG(fake_scope) = scope;

	property = zend_string_init(name, name_length, 0);
	object->handlers->write_property(object, property, value, NULL);
	zend_string_release_ex(property, 0);

	EG(fake_scope) = old_scope;
}
/* }}} */

ZEND_API void zend_update_property_null(zend_class_entry *scope, zend_object *object, const char *name, size_t name_length) /* {{{ */
{
	zval tmp;

	ZVAL_NULL(&tmp);
	zend_update_property(scope, object, name, name_length, &tmp);
}
/* }}} */

ZEND_API void zend_unset_property(zend_class_entry *scope, zend_object *object, const char *name, size_t name_length) /* {{{ */
{
	zend_string *property;
	zend_class_entry *old_scope = EG(fake_scope);

	EG(fake_scope) = scope;

	property = zend_string_init(name, name_length, 0);
	object->handlers->unset_property(object, property, 0);
	zend_string_release_ex(property, 0);

	EG(fake_scope) = old_scope;
}
/* }}} */

ZEND_API void zend_update_property_bool(zend_class_entry *scope, zend_object *object, const char *name, size_t name_length, zend_long value) /* {{{ */
{
	zval tmp;

	ZVAL_BOOL(&tmp, value);
	zend_update_property(scope, object, name, name_length, &tmp);
}
/* }}} */

ZEND_API void zend_update_property_long(zend_class_entry *scope, zend_object *object, const char *name, size_t name_length, zend_long value) /* {{{ */
{
	zval tmp;

	ZVAL_LONG(&tmp, value);
	zend_update_property(scope, object, name, name_length, &tmp);
}
/* }}} */

ZEND_API void zend_update_property_double(zend_class_entry *scope, zend_object *object, const char *name, size_t name_length, double value) /* {{{ */
{
	zval tmp;

	ZVAL_DOUBLE(&tmp, value);
	zend_update_property(scope, object, name, name_length, &tmp);
}
/* }}} */

ZEND_API void zend_update_property_str(zend_class_entry *scope, zend_object *object, const char *name, size_t name_length, zend_string *value) /* {{{ */
{
	zval tmp;

	ZVAL_STR(&tmp, value);
	zend_update_property(scope, object, name, name_length, &tmp);
}
/* }}} */

ZEND_API void zend_update_property_string(zend_class_entry *scope, zend_object *object, const char *name, size_t name_length, const char *value) /* {{{ */
{
	zval tmp;

	ZVAL_STRING(&tmp, value);
	Z_SET_REFCOUNT(tmp, 0);
	zend_update_property(scope, object, name, name_length, &tmp);
}
/* }}} */

ZEND_API void zend_update_property_stringl(zend_class_entry *scope, zend_object *object, const char *name, size_t name_length, const char *value, size_t value_len) /* {{{ */
{
	zval tmp;

	ZVAL_STRINGL(&tmp, value, value_len);
	Z_SET_REFCOUNT(tmp, 0);
	zend_update_property(scope, object, name, name_length, &tmp);
}
/* }}} */

ZEND_API zend_result zend_update_static_property_ex(zend_class_entry *scope, zend_string *name, zval *value) /* {{{ */
{
	zval *property, tmp;
	zend_property_info *prop_info;
	zend_class_entry *old_scope = EG(fake_scope);

	if (UNEXPECTED(!(scope->ce_flags & ZEND_ACC_CONSTANTS_UPDATED))) {
		if (UNEXPECTED(zend_update_class_constants(scope) != SUCCESS)) {
			return FAILURE;
		}
	}

	EG(fake_scope) = scope;
	property = zend_std_get_static_property_with_info(scope, name, BP_VAR_W, &prop_info);
	EG(fake_scope) = old_scope;

	if (!property) {
		return FAILURE;
	}

	ZEND_ASSERT(!Z_ISREF_P(value));
	Z_TRY_ADDREF_P(value);
	if (ZEND_TYPE_IS_SET(prop_info->type)) {
		ZVAL_COPY_VALUE(&tmp, value);
		if (!zend_verify_property_type(prop_info, &tmp, /* strict */ 0)) {
			Z_TRY_DELREF_P(value);
			return FAILURE;
		}
		value = &tmp;
	}

	zend_assign_to_variable(property, value, IS_TMP_VAR, /* strict */ 0);
	return SUCCESS;
}
/* }}} */

ZEND_API zend_result zend_update_static_property(zend_class_entry *scope, const char *name, size_t name_length, zval *value) /* {{{ */
{
	zend_string *key = zend_string_init(name, name_length, 0);
	zend_result retval = zend_update_static_property_ex(scope, key, value);
	zend_string_efree(key);
	return retval;
}
/* }}} */

ZEND_API zend_result zend_update_static_property_null(zend_class_entry *scope, const char *name, size_t name_length) /* {{{ */
{
	zval tmp;

	ZVAL_NULL(&tmp);
	return zend_update_static_property(scope, name, name_length, &tmp);
}
/* }}} */

ZEND_API zend_result zend_update_static_property_bool(zend_class_entry *scope, const char *name, size_t name_length, zend_long value) /* {{{ */
{
	zval tmp;

	ZVAL_BOOL(&tmp, value);
	return zend_update_static_property(scope, name, name_length, &tmp);
}
/* }}} */

ZEND_API zend_result zend_update_static_property_long(zend_class_entry *scope, const char *name, size_t name_length, zend_long value) /* {{{ */
{
	zval tmp;

	ZVAL_LONG(&tmp, value);
	return zend_update_static_property(scope, name, name_length, &tmp);
}
/* }}} */

ZEND_API zend_result zend_update_static_property_double(zend_class_entry *scope, const char *name, size_t name_length, double value) /* {{{ */
{
	zval tmp;

	ZVAL_DOUBLE(&tmp, value);
	return zend_update_static_property(scope, name, name_length, &tmp);
}
/* }}} */

ZEND_API zend_result zend_update_static_property_string(zend_class_entry *scope, const char *name, size_t name_length, const char *value) /* {{{ */
{
	zval tmp;

	ZVAL_STRING(&tmp, value);
	Z_SET_REFCOUNT(tmp, 0);
	return zend_update_static_property(scope, name, name_length, &tmp);
}
/* }}} */

ZEND_API zend_result zend_update_static_property_stringl(zend_class_entry *scope, const char *name, size_t name_length, const char *value, size_t value_len) /* {{{ */
{
	zval tmp;

	ZVAL_STRINGL(&tmp, value, value_len);
	Z_SET_REFCOUNT(tmp, 0);
	return zend_update_static_property(scope, name, name_length, &tmp);
}
/* }}} */

ZEND_API zval *zend_read_property_ex(zend_class_entry *scope, zend_object *object, zend_string *name, bool silent, zval *rv) /* {{{ */
{
	zval *value;
	zend_class_entry *old_scope = EG(fake_scope);

	EG(fake_scope) = scope;

	value = object->handlers->read_property(object, name, silent?BP_VAR_IS:BP_VAR_R, NULL, rv);

	EG(fake_scope) = old_scope;
	return value;
}
/* }}} */

ZEND_API zval *zend_read_property(zend_class_entry *scope, zend_object *object, const char *name, size_t name_length, bool silent, zval *rv) /* {{{ */
{
	zval *value;
	zend_string *str;

	str = zend_string_init(name, name_length, 0);
	value = zend_read_property_ex(scope, object, str, silent, rv);
	zend_string_release_ex(str, 0);
	return value;
}
/* }}} */

ZEND_API zval *zend_read_static_property_ex(zend_class_entry *scope, zend_string *name, bool silent) /* {{{ */
{
	zval *property;
	zend_class_entry *old_scope = EG(fake_scope);

	EG(fake_scope) = scope;
	property = zend_std_get_static_property(scope, name, silent ? BP_VAR_IS : BP_VAR_R);
	EG(fake_scope) = old_scope;

	return property;
}
/* }}} */

ZEND_API zval *zend_read_static_property(zend_class_entry *scope, const char *name, size_t name_length, bool silent) /* {{{ */
{
	zend_string *key = zend_string_init(name, name_length, 0);
	zval *property = zend_read_static_property_ex(scope, key, silent);
	zend_string_efree(key);
	return property;
}
/* }}} */

ZEND_API void zend_save_error_handling(zend_error_handling *current) /* {{{ */
{
	current->handling = EG(error_handling);
	current->exception = EG(exception_class);
}
/* }}} */

ZEND_API void zend_replace_error_handling(zend_error_handling_t error_handling, zend_class_entry *exception_class, zend_error_handling *current) /* {{{ */
{
	if (current) {
		zend_save_error_handling(current);
	}
	ZEND_ASSERT(error_handling == EH_THROW || exception_class == NULL);
	EG(error_handling) = error_handling;
	EG(exception_class) = exception_class;
}
/* }}} */

ZEND_API void zend_restore_error_handling(zend_error_handling *saved) /* {{{ */
{
	EG(error_handling) = saved->handling;
	EG(exception_class) = saved->exception;
}
/* }}} */

ZEND_API ZEND_COLD const char *zend_get_object_type_case(const zend_class_entry *ce, bool upper_case) /* {{{ */
{
	if (ce->ce_flags & ZEND_ACC_TRAIT) {
		return upper_case ? "Trait" : "trait";
	} else if (ce->ce_flags & ZEND_ACC_INTERFACE) {
		return upper_case ? "Interface" : "interface";
	} else if (ce->ce_flags & ZEND_ACC_ENUM) {
		return upper_case ? "Enum" : "enum";
	} else {
		return upper_case ? "Class" : "class";
	}
}
/* }}} */

ZEND_API bool zend_is_iterable(const zval *iterable) /* {{{ */
{
	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			return 1;
		case IS_OBJECT:
			return zend_class_implements_interface(Z_OBJCE_P(iterable), zend_ce_traversable);
		default:
			return 0;
	}
}
/* }}} */

ZEND_API bool zend_is_countable(const zval *countable) /* {{{ */
{
	switch (Z_TYPE_P(countable)) {
		case IS_ARRAY:
			return 1;
		case IS_OBJECT:
			if (Z_OBJ_HT_P(countable)->count_elements) {
				return 1;
			}

			return zend_class_implements_interface(Z_OBJCE_P(countable), zend_ce_countable);
		default:
			return 0;
	}
}
/* }}} */

static zend_result get_default_via_ast(zval *default_value_zval, const char *default_value) {
	zend_ast *ast;
	zend_arena *ast_arena;

	zend_string *code = zend_string_concat3(
		"<?php ", sizeof("<?php ") - 1, default_value, strlen(default_value), ";", 1);

	ast = zend_compile_string_to_ast(code, &ast_arena, ZSTR_EMPTY_ALLOC());
	zend_string_release(code);

	if (!ast) {
		return FAILURE;
	}

	zend_ast_list *statement_list = zend_ast_get_list(ast);
	zend_ast **const_expr_ast_ptr = &statement_list->child[0];

	zend_arena *original_ast_arena = CG(ast_arena);
	uint32_t original_compiler_options = CG(compiler_options);
	zend_file_context original_file_context;
	CG(ast_arena) = ast_arena;
	/* Disable constant substitution, to make getDefaultValueConstant() work. */
	CG(compiler_options) |= ZEND_COMPILE_NO_CONSTANT_SUBSTITUTION | ZEND_COMPILE_NO_PERSISTENT_CONSTANT_SUBSTITUTION;
	zend_file_context_begin(&original_file_context);
	zend_const_expr_to_zval(default_value_zval, const_expr_ast_ptr, /* allow_dynamic */ true);
	CG(ast_arena) = original_ast_arena;
	CG(compiler_options) = original_compiler_options;
	zend_file_context_end(&original_file_context);

	zend_ast_destroy(ast);
	zend_arena_destroy(ast_arena);

	return SUCCESS;
}

static zend_string *try_parse_string(const char *str, size_t len, char quote) {
	if (len == 0) {
		return ZSTR_EMPTY_ALLOC();
	}

	for (size_t i = 0; i < len; i++) {
		if (str[i] == '\\' || str[i] == quote) {
			return NULL;
		}
	}
	return zend_string_init(str, len, 0);
}

ZEND_API zend_result zend_get_default_from_internal_arg_info(
		zval *default_value_zval, zend_internal_arg_info *arg_info)
{
	const char *default_value = arg_info->default_value;
	if (!default_value) {
		return FAILURE;
	}

	/* Avoid going through the full AST machinery for some simple and common cases. */
	size_t default_value_len = strlen(default_value);
	zend_ulong lval;
	if (default_value_len == sizeof("null")-1
			&& !memcmp(default_value, "null", sizeof("null")-1)) {
		ZVAL_NULL(default_value_zval);
		return SUCCESS;
	} else if (default_value_len == sizeof("true")-1
			&& !memcmp(default_value, "true", sizeof("true")-1)) {
		ZVAL_TRUE(default_value_zval);
		return SUCCESS;
	} else if (default_value_len == sizeof("false")-1
			&& !memcmp(default_value, "false", sizeof("false")-1)) {
		ZVAL_FALSE(default_value_zval);
		return SUCCESS;
	} else if (default_value_len >= 2
			&& (default_value[0] == '\'' || default_value[0] == '"')
			&& default_value[default_value_len - 1] == default_value[0]) {
		zend_string *str = try_parse_string(
			default_value + 1, default_value_len - 2, default_value[0]);
		if (str) {
			ZVAL_STR(default_value_zval, str);
			return SUCCESS;
		}
	} else if (default_value_len == sizeof("[]")-1
			&& !memcmp(default_value, "[]", sizeof("[]")-1)) {
		ZVAL_EMPTY_ARRAY(default_value_zval);
		return SUCCESS;
	} else if (ZEND_HANDLE_NUMERIC_STR(default_value, default_value_len, lval)) {
		ZVAL_LONG(default_value_zval, lval);
		return SUCCESS;
	}

#if 0
	fprintf(stderr, "Evaluating %s via AST\n", default_value);
#endif
	return get_default_via_ast(default_value_zval, default_value);
}
