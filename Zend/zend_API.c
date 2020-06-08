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

#include <stdarg.h>

/* these variables are true statics/globals, and have to be mutex'ed on every access */
ZEND_API HashTable module_registry;

static zend_module_entry **module_request_startup_handlers;
static zend_module_entry **module_request_shutdown_handlers;
static zend_module_entry **module_post_deactivate_handlers;

static zend_class_entry  **class_cleanup_handlers;

ZEND_API int _zend_get_parameters_array_ex(int param_count, zval *argument_array) /* {{{ */
{
	zval *param_ptr;
	int arg_count;

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

ZEND_API int zend_copy_parameters_array(int param_count, zval *argument_array) /* {{{ */
{
	zval *param_ptr;
	int arg_count;

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
			return "number";
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}
/* }}} */

ZEND_API const char *zend_zval_type_name(const zval *arg) /* {{{ */
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
/* }}} */

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

ZEND_API ZEND_COLD int ZEND_FASTCALL zend_wrong_parameters_none_error(void) /* {{{ */
{
	int num_args = ZEND_CALL_NUM_ARGS(EG(current_execute_data));
	zend_function *active_function = EG(current_execute_data)->func;
	const char *class_name = active_function->common.scope ? ZSTR_VAL(active_function->common.scope->name) : "";

	zend_argument_count_error(
				"%s%s%s() expects %s %d parameter%s, %d given",
				class_name, \
				class_name[0] ? "::" : "", \
				ZSTR_VAL(active_function->common.function_name),
				"exactly",
				0,
				"s",
				num_args);
	return FAILURE;
}
/* }}} */

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_parameters_count_error(int min_num_args, int max_num_args) /* {{{ */
{
	int num_args = ZEND_CALL_NUM_ARGS(EG(current_execute_data));
	zend_function *active_function = EG(current_execute_data)->func;
	const char *class_name = active_function->common.scope ? ZSTR_VAL(active_function->common.scope->name) : "";

	zend_argument_count_error(
				"%s%s%s() expects %s %d parameter%s, %d given",
				class_name, \
				class_name[0] ? "::" : "", \
				ZSTR_VAL(active_function->common.function_name),
				min_num_args == max_num_args ? "exactly" : num_args < min_num_args ? "at least" : "at most",
				num_args < min_num_args ? min_num_args : max_num_args,
				(num_args < min_num_args ? min_num_args : max_num_args) == 1 ? "" : "s",
				num_args);
}
/* }}} */

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_parameter_type_error(int num, zend_expected_type expected_type, zval *arg) /* {{{ */
{
	static const char * const expected_error[] = {
		Z_EXPECTED_TYPES(Z_EXPECTED_TYPE_STR)
		NULL
	};

	if (EG(exception)) {
		return;
	}

	zend_argument_type_error(num, "must be %s, %s given", expected_error[expected_type], zend_zval_type_name(arg));
}
/* }}} */

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_parameter_class_error(int num, const char *name, zval *arg) /* {{{ */
{
	if (EG(exception)) {
		return;
	}

	zend_argument_type_error(num, "must be of type %s, %s given", name, zend_zval_type_name(arg));
}
/* }}} */

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_parameter_class_or_null_error(int num, const char *name, zval *arg) /* {{{ */
{
	if (EG(exception)) {
		return;
	}

	zend_argument_type_error(num, "must be of type ?%s, %s given", name, zend_zval_type_name(arg));
}
/* }}} */

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_callback_error(int num, char *error) /* {{{ */
{
	if (EG(exception)) {
		return;
	}

	zend_argument_type_error(num, "must be a valid callback, %s", error);
	efree(error);
}
/* }}} */

static ZEND_COLD void ZEND_FASTCALL zend_argument_error_variadic(zend_class_entry *error_ce, uint32_t arg_num, const char *format, va_list va) /* {{{ */
{
	const char *space;
	const char *class_name;
	const char *arg_name;
	char *message = NULL;
	if (EG(exception)) {
		return;
	}

	class_name = get_active_class_name(&space);
	arg_name = get_active_function_arg_name(arg_num);

	zend_vspprintf(&message, 0, format, va);
	zend_throw_error(error_ce, "%s%s%s(): Argument #%d%s%s%s %s",
		class_name, space, get_active_function_name(), arg_num,
		arg_name ? " ($" : "", arg_name ? arg_name : "", arg_name ? ")" : "", message
    );
	efree(message);
}
/* }}} */

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_argument_error(zend_class_entry *error_ce, uint32_t arg_num, const char *format, ...) /* {{{ */
{
	va_list va;

	va_start(va, format);
	zend_argument_error_variadic(error_ce, arg_num, format, va);
	va_end(va);
}
/* }}} */

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_argument_type_error(uint32_t arg_num, const char *format, ...) /* {{{ */
{
	va_list va;

	va_start(va, format);
	zend_argument_error_variadic(zend_ce_type_error, arg_num, format, va);
	va_end(va);
}
/* }}} */

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_argument_value_error(uint32_t arg_num, const char *format, ...) /* {{{ */
{
	va_list va;

	va_start(va, format);
	zend_argument_error_variadic(zend_ce_value_error, arg_num, format, va);
	va_end(va);
}
/* }}} */

ZEND_API int ZEND_FASTCALL zend_parse_arg_class(zval *arg, zend_class_entry **pce, int num, int check_null) /* {{{ */
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
			zend_argument_type_error(num, "must be a class name derived from %s, '%s' given", ZSTR_VAL(ce_base->name), Z_STRVAL_P(arg));
			*pce = NULL;
			return 0;
		}
	}
	if (!*pce) {
		zend_argument_type_error(num, "must be a valid class name, '%s' given", Z_STRVAL_P(arg));
		return 0;
	}
	return 1;
}
/* }}} */

ZEND_API int ZEND_FASTCALL zend_parse_arg_bool_weak(zval *arg, zend_bool *dest) /* {{{ */
{
	if (EXPECTED(Z_TYPE_P(arg) <= IS_STRING)) {
		*dest = zend_is_true(arg);
	} else {
		return 0;
	}
	return 1;
}
/* }}} */

ZEND_API int ZEND_FASTCALL zend_parse_arg_bool_slow(zval *arg, zend_bool *dest) /* {{{ */
{
	if (UNEXPECTED(ZEND_ARG_USES_STRICT_TYPES())) {
		return 0;
	}
	return zend_parse_arg_bool_weak(arg, dest);
}
/* }}} */

ZEND_API int ZEND_FASTCALL zend_parse_arg_long_weak(zval *arg, zend_long *dest) /* {{{ */
{
	if (EXPECTED(Z_TYPE_P(arg) == IS_DOUBLE)) {
		if (UNEXPECTED(zend_isnan(Z_DVAL_P(arg)))) {
			return 0;
		}
		if (UNEXPECTED(!ZEND_DOUBLE_FITS_LONG(Z_DVAL_P(arg)))) {
			return 0;
		} else {
			*dest = zend_dval_to_lval(Z_DVAL_P(arg));
		}
	} else if (EXPECTED(Z_TYPE_P(arg) == IS_STRING)) {
		double d;
		int type;

		if (UNEXPECTED((type = is_numeric_str_function(Z_STR_P(arg), dest, &d)) != IS_LONG)) {
			if (EXPECTED(type != 0)) {
				if (UNEXPECTED(zend_isnan(d))) {
					return 0;
				}
				if (UNEXPECTED(!ZEND_DOUBLE_FITS_LONG(d))) {
					return 0;
				} else {
					*dest = zend_dval_to_lval(d);
				}
			} else {
				return 0;
			}
		}
		if (UNEXPECTED(EG(exception))) {
			return 0;
		}
	} else if (EXPECTED(Z_TYPE_P(arg) < IS_TRUE)) {
		*dest = 0;
	} else if (EXPECTED(Z_TYPE_P(arg) == IS_TRUE)) {
		*dest = 1;
	} else {
		return 0;
	}
	return 1;
}
/* }}} */

ZEND_API int ZEND_FASTCALL zend_parse_arg_long_slow(zval *arg, zend_long *dest) /* {{{ */
{
	if (UNEXPECTED(ZEND_ARG_USES_STRICT_TYPES())) {
		return 0;
	}
	return zend_parse_arg_long_weak(arg, dest);
}
/* }}} */

ZEND_API int ZEND_FASTCALL zend_parse_arg_double_weak(zval *arg, double *dest) /* {{{ */
{
	if (EXPECTED(Z_TYPE_P(arg) == IS_LONG)) {
		*dest = (double)Z_LVAL_P(arg);
	} else if (EXPECTED(Z_TYPE_P(arg) == IS_STRING)) {
		zend_long l;
		int type;

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
		*dest = 0.0;
	} else if (EXPECTED(Z_TYPE_P(arg) == IS_TRUE)) {
		*dest = 1.0;
	} else {
		return 0;
	}
	return 1;
}
/* }}} */

ZEND_API int ZEND_FASTCALL zend_parse_arg_double_slow(zval *arg, double *dest) /* {{{ */
{
	if (EXPECTED(Z_TYPE_P(arg) == IS_LONG)) {
		/* SSTH Exception: IS_LONG may be accepted instead as IS_DOUBLE */
		*dest = (double)Z_LVAL_P(arg);
	} else if (UNEXPECTED(ZEND_ARG_USES_STRICT_TYPES())) {
		return 0;
	}
	return zend_parse_arg_double_weak(arg, dest);
}
/* }}} */

ZEND_API int ZEND_FASTCALL zend_parse_arg_number_slow(zval *arg, zval **dest) /* {{{ */
{
	if (UNEXPECTED(ZEND_ARG_USES_STRICT_TYPES())) {
		return 0;
	}
	if (Z_TYPE_P(arg) == IS_STRING) {
		zend_string *str = Z_STR_P(arg);
		zend_long lval;
		double dval;
		zend_uchar type = is_numeric_string(ZSTR_VAL(str), ZSTR_LEN(str), &lval, &dval, -1);
		if (type == IS_LONG) {
			ZVAL_LONG(arg, lval);
		} else if (type == IS_DOUBLE) {
			ZVAL_DOUBLE(arg, dval);
		} else {
			return 0;
		}
		zend_string_release(str);
	} else if (Z_TYPE_P(arg) < IS_TRUE) {
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

ZEND_API int ZEND_FASTCALL zend_parse_arg_str_weak(zval *arg, zend_string **dest) /* {{{ */
{
	if (EXPECTED(Z_TYPE_P(arg) < IS_STRING)) {
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

ZEND_API int ZEND_FASTCALL zend_parse_arg_str_slow(zval *arg, zend_string **dest) /* {{{ */
{
	if (UNEXPECTED(ZEND_ARG_USES_STRICT_TYPES())) {
		return 0;
	}
	return zend_parse_arg_str_weak(arg, dest);
}
/* }}} */

ZEND_API int ZEND_FASTCALL zend_parse_arg_str_or_long_slow(zval *arg, zend_string **dest_str, zend_long *dest_long) /* {{{ */
{
	if (UNEXPECTED(ZEND_ARG_USES_STRICT_TYPES())) {
		return 0;
	}
	if (zend_parse_arg_long_weak(arg, dest_long)) {
		*dest_str = NULL;
		return 1;
	} else if (zend_parse_arg_str_weak(arg, dest_str)) {
		*dest_long = 0;
		return 1;
	} else {
		return 0;
	}
}
/* }}} */

static const char *zend_parse_arg_impl(zval *arg, va_list *va, const char **spec, char **error) /* {{{ */
{
	const char *spec_walk = *spec;
	char c = *spec_walk++;
	int check_null = 0;
	int separate = 0;
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
				zend_bool *is_null = NULL;

				if (check_null) {
					is_null = va_arg(*va, zend_bool *);
				}

				if (!zend_parse_arg_long(arg, p, is_null, check_null)) {
					return check_null ? "?int" : "int";
				}
			}
			break;

		case 'd':
			{
				double *p = va_arg(*va, double *);
				zend_bool *is_null = NULL;

				if (check_null) {
					is_null = va_arg(*va, zend_bool *);
				}

				if (!zend_parse_arg_double(arg, p, is_null, check_null)) {
					return check_null ? "?float" : "float";
				}
			}
			break;

		case 'n':
			{
				zval **p = va_arg(*va, zval **);

				if (!zend_parse_arg_number(arg, p, check_null)) {
					return check_null ? "int|float|null" : "int|float";
				}
			}
			break;

		case 's':
			{
				char **p = va_arg(*va, char **);
				size_t *pl = va_arg(*va, size_t *);
				if (!zend_parse_arg_string(arg, p, pl, check_null)) {
					return check_null ? "?string" : "string";
				}
			}
			break;

		case 'p':
			{
				char **p = va_arg(*va, char **);
				size_t *pl = va_arg(*va, size_t *);
				if (!zend_parse_arg_path(arg, p, pl, check_null)) {
					zend_spprintf(error, 0, "a valid path%s, %s given",
						check_null ? " or null" : "", zend_zval_type_name(arg)
					);
					return "";
				}
			}
			break;

		case 'P':
			{
				zend_string **str = va_arg(*va, zend_string **);
				if (!zend_parse_arg_path_str(arg, str, check_null)) {
					zend_spprintf(error, 0, "a valid path%s, %s given",
						check_null ? " or null" : "", zend_zval_type_name(arg)
					);
					return "";
				}
			}
			break;

		case 'S':
			{
				zend_string **str = va_arg(*va, zend_string **);
				if (!zend_parse_arg_str(arg, str, check_null)) {
					return check_null ? "?string" : "string";
				}
			}
			break;

		case 'b':
			{
				zend_bool *p = va_arg(*va, zend_bool *);
				zend_bool *is_null = NULL;

				if (check_null) {
					is_null = va_arg(*va, zend_bool *);
				}

				if (!zend_parse_arg_bool(arg, p, is_null, check_null)) {
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
							zend_spprintf(error, 0, "of type ?%s, %s given", ZSTR_VAL(ce->name), zend_zval_type_name(arg));
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
						zend_spprintf(error, 0, "a class name derived from %s%s, '%s' given",
							ZSTR_VAL(ce_base->name), check_null ? " or null" : "", Z_STRVAL_P(arg));
						*pce = NULL;
						return "";
					}
				}
				if (!*pce) {
					zend_spprintf(error, 0, "a valid class name%s, '%s' given",
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
					break;
				}

				if (is_callable_error) {
					zend_spprintf(error, 0, "a valid callback%s, %s", check_null ? " or null" : "", is_callable_error);
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
		default:
			return "unknown";
	}

	*spec = spec_walk;

	return NULL;
}
/* }}} */

static int zend_parse_arg(uint32_t arg_num, zval *arg, va_list *va, const char **spec, int flags) /* {{{ */
{
	const char *expected_type = NULL;
	char *error = NULL;

	expected_type = zend_parse_arg_impl(arg, va, spec, &error);
	if (expected_type) {
		if (EG(exception)) {
			return FAILURE;
		}
		if (!(flags & ZEND_PARSE_PARAMS_QUIET) && (*expected_type || error)) {
			if (error) {
				zend_argument_type_error(arg_num, "must be %s", error);
				efree(error);
			} else {
				zend_argument_type_error(arg_num, "must be of type %s, %s given", expected_type, zend_zval_type_name(arg));
			}
		} else if (error) {
			efree(error);
		}

		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

ZEND_API int zend_parse_parameter(int flags, uint32_t arg_num, zval *arg, const char *spec, ...)
{
	va_list va;
	int ret;

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

static int zend_parse_va_args(uint32_t num_args, const char *type_spec, va_list *va, int flags) /* {{{ */
{
	const  char *spec_walk;
	char c;
	uint32_t i;
	uint32_t min_num_args = 0;
	uint32_t max_num_args = 0;
	uint32_t post_varargs = 0;
	zval *arg;
	zend_bool have_varargs = 0;
	zend_bool have_optional_args = 0;
	zval **varargs = NULL;
	int *n_varargs = NULL;

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
			zend_function *active_function = EG(current_execute_data)->func;
			const char *class_name = active_function->common.scope ? ZSTR_VAL(active_function->common.scope->name) : "";
			zend_argument_count_error("%s%s%s() expects %s %d parameter%s, %d given",
					class_name,
					class_name[0] ? "::" : "",
					ZSTR_VAL(active_function->common.function_name),
					min_num_args == max_num_args ? "exactly" : num_args < min_num_args ? "at least" : "at most",
					num_args < min_num_args ? min_num_args : max_num_args,
					(num_args < min_num_args ? min_num_args : max_num_args) == 1 ? "" : "s",
					num_args);
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
			int num_varargs = num_args + 1 - post_varargs;

			/* eat up the passed in storage even if it won't be filled in with varargs */
			varargs = va_arg(*va, zval **);
			n_varargs = va_arg(*va, int *);
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

ZEND_API int zend_parse_parameters_ex(int flags, uint32_t num_args, const char *type_spec, ...) /* {{{ */
{
	va_list va;
	int retval;

	va_start(va, type_spec);
	retval = zend_parse_va_args(num_args, type_spec, &va, flags);
	va_end(va);

	return retval;
}
/* }}} */

ZEND_API int zend_parse_parameters(uint32_t num_args, const char *type_spec, ...) /* {{{ */
{
	va_list va;
	int retval;
	int flags = 0;

	va_start(va, type_spec);
	retval = zend_parse_va_args(num_args, type_spec, &va, flags);
	va_end(va);

	return retval;
}
/* }}} */

ZEND_API int zend_parse_method_parameters(uint32_t num_args, zval *this_ptr, const char *type_spec, ...) /* {{{ */
{
	va_list va;
	int retval;
	int flags = 0;
	const char *p = type_spec;
	zval **object;
	zend_class_entry *ce;

	/* Just checking this_ptr is not enough, because fcall_common_helper does not set
	 * Z_OBJ(EG(This)) to NULL when calling an internal function with common.scope == NULL.
	 * In that case EG(This) would still be the $this from the calling code and we'd take the
	 * wrong branch here. */
	zend_bool is_method = EG(current_execute_data)->func->common.scope != NULL;

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
			zend_error_noreturn(E_CORE_ERROR, "%s::%s() must be derived from %s::%s",
				ZSTR_VAL(Z_OBJCE_P(this_ptr)->name), get_active_function_name(), ZSTR_VAL(ce->name), get_active_function_name());
		}

		retval = zend_parse_va_args(num_args, p, &va, flags);
		va_end(va);
	}
	return retval;
}
/* }}} */

ZEND_API int zend_parse_method_parameters_ex(int flags, uint32_t num_args, zval *this_ptr, const char *type_spec, ...) /* {{{ */
{
	va_list va;
	int retval;
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
				zend_error_noreturn(E_CORE_ERROR, "%s::%s() must be derived from %s::%s",
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

	EG(fake_scope) = Z_OBJCE_P(obj);
	ZEND_HASH_FOREACH_STR_KEY_VAL(properties, key, value) {
		if (key) {
			write_property(zobj, key, value, NULL);
		}
	} ZEND_HASH_FOREACH_END();
	EG(fake_scope) = old_scope;
}
/* }}} */

ZEND_API int zend_update_class_constants(zend_class_entry *class_type) /* {{{ */
{
	if (!(class_type->ce_flags & ZEND_ACC_CONSTANTS_UPDATED)) {
		zend_class_constant *c;
		zval *val;
		zend_property_info *prop_info;

		if (class_type->parent) {
			if (UNEXPECTED(zend_update_class_constants(class_type->parent) != SUCCESS)) {
				return FAILURE;
			}
		}

		ZEND_HASH_FOREACH_PTR(&class_type->constants_table, c) {
			val = &c->value;
			if (Z_TYPE_P(val) == IS_CONSTANT_AST) {
				if (UNEXPECTED(zval_update_constant_ex(val, c->ce) != SUCCESS)) {
					return FAILURE;
				}
			}
		} ZEND_HASH_FOREACH_END();

		if (class_type->default_static_members_count && !CE_STATIC_MEMBERS(class_type)) {
			if (class_type->type == ZEND_INTERNAL_CLASS || (class_type->ce_flags & (ZEND_ACC_IMMUTABLE|ZEND_ACC_PRELOADED))) {
				zend_class_init_statics(class_type);
			}
		}

		ZEND_HASH_FOREACH_PTR(&class_type->properties_info, prop_info) {
			if (prop_info->flags & ZEND_ACC_STATIC) {
				val = CE_STATIC_MEMBERS(class_type) + prop_info->offset;
			} else {
				val = (zval*)((char*)class_type->default_properties_table + prop_info->offset - OBJ_PROP_TO_OFFSET(0));
			}
			if (Z_TYPE_P(val) == IS_CONSTANT_AST) {
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
				} else if (UNEXPECTED(zval_update_constant_ex(val, prop_info->ce) != SUCCESS)) {
					return FAILURE;
				}
			}
		} ZEND_HASH_FOREACH_END();

		class_type->ce_flags |= ZEND_ACC_CONSTANTS_UPDATED;
	}

	return SUCCESS;
}
/* }}} */

static zend_always_inline void _object_properties_init(zend_object *object, zend_class_entry *class_type) /* {{{ */
{
	if (class_type->default_properties_count) {
		zval *src = class_type->default_properties_table;
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

		ZEND_HASH_FOREACH_STR_KEY_VAL(properties, key, prop) {
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
				if (!object->properties) {
					rebuild_object_properties(object);
				}
				prop = zend_hash_update(object->properties, key, prop);
				zval_add_ref(prop);
			}
		} else {
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
static zend_always_inline int _object_and_properties_init(zval *arg, zend_class_entry *class_type, HashTable *properties) /* {{{ */
{
	if (UNEXPECTED(class_type->ce_flags & (ZEND_ACC_INTERFACE|ZEND_ACC_TRAIT|ZEND_ACC_IMPLICIT_ABSTRACT_CLASS|ZEND_ACC_EXPLICIT_ABSTRACT_CLASS))) {
		if (class_type->ce_flags & ZEND_ACC_INTERFACE) {
			zend_throw_error(NULL, "Cannot instantiate interface %s", ZSTR_VAL(class_type->name));
		} else if (class_type->ce_flags & ZEND_ACC_TRAIT) {
			zend_throw_error(NULL, "Cannot instantiate trait %s", ZSTR_VAL(class_type->name));
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

ZEND_API int object_and_properties_init(zval *arg, zend_class_entry *class_type, HashTable *properties) /* {{{ */
{
	return _object_and_properties_init(arg, class_type, properties);
}
/* }}} */

ZEND_API int object_init_ex(zval *arg, zend_class_entry *class_type) /* {{{ */
{
	return _object_and_properties_init(arg, class_type, NULL);
}
/* }}} */

ZEND_API int object_init(zval *arg) /* {{{ */
{
	ZVAL_OBJ(arg, zend_objects_new(zend_standard_class_def));
	return SUCCESS;
}
/* }}} */

ZEND_API int add_assoc_long_ex(zval *arg, const char *key, size_t key_len, zend_long n) /* {{{ */
{
	zval tmp;

	ZVAL_LONG(&tmp, n);
	zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, &tmp);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_assoc_null_ex(zval *arg, const char *key, size_t key_len) /* {{{ */
{
	zval tmp;

	ZVAL_NULL(&tmp);
	zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, &tmp);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_assoc_bool_ex(zval *arg, const char *key, size_t key_len, int b) /* {{{ */
{
	zval tmp;

	ZVAL_BOOL(&tmp, b);
	zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, &tmp);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_assoc_resource_ex(zval *arg, const char *key, size_t key_len, zend_resource *r) /* {{{ */
{
	zval tmp;

	ZVAL_RES(&tmp, r);
	zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, &tmp);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_assoc_double_ex(zval *arg, const char *key, size_t key_len, double d) /* {{{ */
{
	zval tmp;

	ZVAL_DOUBLE(&tmp, d);
	zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, &tmp);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_assoc_str_ex(zval *arg, const char *key, size_t key_len, zend_string *str) /* {{{ */
{
	zval tmp;

	ZVAL_STR(&tmp, str);
	zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, &tmp);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_assoc_string_ex(zval *arg, const char *key, size_t key_len, const char *str) /* {{{ */
{
	zval tmp;

	ZVAL_STRING(&tmp, str);
	zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, &tmp);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_assoc_stringl_ex(zval *arg, const char *key, size_t key_len, const char *str, size_t length) /* {{{ */
{
	zval tmp;

	ZVAL_STRINGL(&tmp, str, length);
	zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, &tmp);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_assoc_zval_ex(zval *arg, const char *key, size_t key_len, zval *value) /* {{{ */
{
	zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, value);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_index_long(zval *arg, zend_ulong index, zend_long n) /* {{{ */
{
	zval tmp;

	ZVAL_LONG(&tmp, n);
	zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_index_null(zval *arg, zend_ulong index) /* {{{ */
{
	zval tmp;

	ZVAL_NULL(&tmp);
	zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_index_bool(zval *arg, zend_ulong index, int b) /* {{{ */
{
	zval tmp;

	ZVAL_BOOL(&tmp, b);
	zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_index_resource(zval *arg, zend_ulong index, zend_resource *r) /* {{{ */
{
	zval tmp;

	ZVAL_RES(&tmp, r);
	zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_index_double(zval *arg, zend_ulong index, double d) /* {{{ */
{
	zval tmp;

	ZVAL_DOUBLE(&tmp, d);
	zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_index_str(zval *arg, zend_ulong index, zend_string *str) /* {{{ */
{
	zval tmp;

	ZVAL_STR(&tmp, str);
	zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_index_string(zval *arg, zend_ulong index, const char *str) /* {{{ */
{
	zval tmp;

	ZVAL_STRING(&tmp, str);
	zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_index_stringl(zval *arg, zend_ulong index, const char *str, size_t length) /* {{{ */
{
	zval tmp;

	ZVAL_STRINGL(&tmp, str, length);
	zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_next_index_long(zval *arg, zend_long n) /* {{{ */
{
	zval tmp;

	ZVAL_LONG(&tmp, n);
	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp) ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API int add_next_index_null(zval *arg) /* {{{ */
{
	zval tmp;

	ZVAL_NULL(&tmp);
	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp) ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API int add_next_index_bool(zval *arg, int b) /* {{{ */
{
	zval tmp;

	ZVAL_BOOL(&tmp, b);
	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp) ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API int add_next_index_resource(zval *arg, zend_resource *r) /* {{{ */
{
	zval tmp;

	ZVAL_RES(&tmp, r);
	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp) ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API int add_next_index_double(zval *arg, double d) /* {{{ */
{
	zval tmp;

	ZVAL_DOUBLE(&tmp, d);
	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp) ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API int add_next_index_str(zval *arg, zend_string *str) /* {{{ */
{
	zval tmp;

	ZVAL_STR(&tmp, str);
	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp) ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API int add_next_index_string(zval *arg, const char *str) /* {{{ */
{
	zval tmp;

	ZVAL_STRING(&tmp, str);
	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp) ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API int add_next_index_stringl(zval *arg, const char *str, size_t length) /* {{{ */
{
	zval tmp;

	ZVAL_STRINGL(&tmp, str, length);
	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp) ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API int array_set_zval_key(HashTable *ht, zval *key, zval *value) /* {{{ */
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
			zend_error(E_WARNING, "Resource ID#%d used as offset, casting to integer (%d)", Z_RES_HANDLE_P(key), Z_RES_HANDLE_P(key));
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
			result = zend_hash_index_update(ht, zend_dval_to_lval(Z_DVAL_P(key)), value);
			break;
		default:
			zend_type_error("Illegal offset type");
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

ZEND_API int add_property_long_ex(zval *arg, const char *key, size_t key_len, zend_long n) /* {{{ */
{
	zval tmp;

	ZVAL_LONG(&tmp, n);
	return add_property_zval_ex(arg, key, key_len, &tmp);
}
/* }}} */

ZEND_API int add_property_bool_ex(zval *arg, const char *key, size_t key_len, zend_long b) /* {{{ */
{
	zval tmp;

	ZVAL_BOOL(&tmp, b);
	return add_property_zval_ex(arg, key, key_len, &tmp);
}
/* }}} */

ZEND_API int add_property_null_ex(zval *arg, const char *key, size_t key_len) /* {{{ */
{
	zval tmp;

	ZVAL_NULL(&tmp);
	return add_property_zval_ex(arg, key, key_len, &tmp);
}
/* }}} */

ZEND_API int add_property_resource_ex(zval *arg, const char *key, size_t key_len, zend_resource *r) /* {{{ */
{
	zval tmp;

	ZVAL_RES(&tmp, r);
	add_property_zval_ex(arg, key, key_len, &tmp);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_double_ex(zval *arg, const char *key, size_t key_len, double d) /* {{{ */
{
	zval tmp;

	ZVAL_DOUBLE(&tmp, d);
	return add_property_zval_ex(arg, key, key_len, &tmp);
}
/* }}} */

ZEND_API int add_property_str_ex(zval *arg, const char *key, size_t key_len, zend_string *str) /* {{{ */
{
	zval tmp;

	ZVAL_STR(&tmp, str);
	add_property_zval_ex(arg, key, key_len, &tmp);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_string_ex(zval *arg, const char *key, size_t key_len, const char *str) /* {{{ */
{
	zval tmp;

	ZVAL_STRING(&tmp, str);
	add_property_zval_ex(arg, key, key_len, &tmp);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_stringl_ex(zval *arg, const char *key, size_t key_len, const char *str, size_t length) /* {{{ */
{
	zval tmp;

	ZVAL_STRINGL(&tmp, str, length);
	add_property_zval_ex(arg, key, key_len, &tmp);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_zval_ex(zval *arg, const char *key, size_t key_len, zval *value) /* {{{ */
{
	zend_string *str;

	str = zend_string_init(key, key_len, 0);
	Z_OBJ_HANDLER_P(arg, write_property)(Z_OBJ_P(arg), str, value, NULL);
	zend_string_release_ex(str, 0);
	return SUCCESS;
}
/* }}} */

ZEND_API int zend_startup_module_ex(zend_module_entry *module) /* {{{ */
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
					zend_error(E_CORE_WARNING, "Cannot load module '%s' because required module '%s' is not loaded", module->name, dep->name);
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
	ZEND_HASH_FOREACH_PTR(&module_registry, module) {
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

	ZEND_HASH_FOREACH_PTR(&module_registry, module) {
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
	ZEND_HASH_FOREACH_PTR(CG(class_table), ce) {
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
		ZEND_HASH_FOREACH_PTR(CG(class_table), ce) {
			if (ce->type == ZEND_INTERNAL_CLASS &&
			    ce->default_static_members_count > 0) {
			    class_cleanup_handlers[--class_count] = ce;
			}
		} ZEND_HASH_FOREACH_END();
	}
}
/* }}} */

ZEND_API int zend_startup_modules(void) /* {{{ */
{
	zend_hash_sort_ex(&module_registry, zend_sort_modules, NULL, 0);
	zend_hash_apply(&module_registry, zend_startup_module_zval);
	return SUCCESS;
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
					zend_error(E_CORE_WARNING, "Cannot load module '%s' because conflicting module '%s' is already loaded", module->name, dep->name);
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
	if ((module_ptr = zend_hash_add_mem(&module_registry, lcname, module, sizeof(zend_module_entry))) == NULL) {
		zend_error(E_CORE_WARNING, "Module '%s' already loaded", module->name);
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

ZEND_API void zend_check_magic_method_implementation(const zend_class_entry *ce, const zend_function *fptr, zend_string *lcname, int error_type) /* {{{ */
{
	if (ZSTR_VAL(fptr->common.function_name)[0] != '_'
	 || ZSTR_VAL(fptr->common.function_name)[1] != '_') {
		return;
	}

	if (zend_string_equals_literal(lcname, ZEND_DESTRUCTOR_FUNC_NAME) && fptr->common.num_args != 0) {
		zend_error(error_type, "Destructor %s::%s() cannot take arguments", ZSTR_VAL(ce->name), ZEND_DESTRUCTOR_FUNC_NAME);
	} else if (zend_string_equals_literal(lcname, ZEND_CLONE_FUNC_NAME) && fptr->common.num_args != 0) {
		zend_error(error_type, "Method %s::%s() cannot accept any arguments", ZSTR_VAL(ce->name), ZEND_CLONE_FUNC_NAME);
	} else if (zend_string_equals_literal(lcname, ZEND_GET_FUNC_NAME)) {
		if (fptr->common.num_args != 1) {
			zend_error(error_type, "Method %s::%s() must take exactly 1 argument", ZSTR_VAL(ce->name), ZEND_GET_FUNC_NAME);
		} else if (QUICK_ARG_SHOULD_BE_SENT_BY_REF(fptr, 1)) {
			zend_error(error_type, "Method %s::%s() cannot take arguments by reference", ZSTR_VAL(ce->name), ZEND_GET_FUNC_NAME);
		}
	} else if (zend_string_equals_literal(lcname, ZEND_SET_FUNC_NAME)) {
		if (fptr->common.num_args != 2) {
			zend_error(error_type, "Method %s::%s() must take exactly 2 arguments", ZSTR_VAL(ce->name), ZEND_SET_FUNC_NAME);
		} else if (QUICK_ARG_SHOULD_BE_SENT_BY_REF(fptr, 1) || QUICK_ARG_SHOULD_BE_SENT_BY_REF(fptr, 2)) {
			zend_error(error_type, "Method %s::%s() cannot take arguments by reference", ZSTR_VAL(ce->name), ZEND_SET_FUNC_NAME);
		}
	} else if (zend_string_equals_literal(lcname, ZEND_UNSET_FUNC_NAME)) {
		if (fptr->common.num_args != 1) {
			zend_error(error_type, "Method %s::%s() must take exactly 1 argument", ZSTR_VAL(ce->name), ZEND_UNSET_FUNC_NAME);
		} else if (QUICK_ARG_SHOULD_BE_SENT_BY_REF(fptr, 1)) {
			zend_error(error_type, "Method %s::%s() cannot take arguments by reference", ZSTR_VAL(ce->name), ZEND_UNSET_FUNC_NAME);
		}
	} else if (zend_string_equals_literal(lcname, ZEND_ISSET_FUNC_NAME)) {
		if (fptr->common.num_args != 1) {
			zend_error(error_type, "Method %s::%s() must take exactly 1 argument", ZSTR_VAL(ce->name), ZEND_ISSET_FUNC_NAME);
		} else if (QUICK_ARG_SHOULD_BE_SENT_BY_REF(fptr, 1)) {
			zend_error(error_type, "Method %s::%s() cannot take arguments by reference", ZSTR_VAL(ce->name), ZEND_ISSET_FUNC_NAME);
		}
	} else if (zend_string_equals_literal(lcname, ZEND_CALL_FUNC_NAME)) {
		if (fptr->common.num_args != 2) {
			zend_error(error_type, "Method %s::%s() must take exactly 2 arguments", ZSTR_VAL(ce->name), ZEND_CALL_FUNC_NAME);
		} else if (QUICK_ARG_SHOULD_BE_SENT_BY_REF(fptr, 1) || QUICK_ARG_SHOULD_BE_SENT_BY_REF(fptr, 2)) {
			zend_error(error_type, "Method %s::%s() cannot take arguments by reference", ZSTR_VAL(ce->name), ZEND_CALL_FUNC_NAME);
		}
	} else if (zend_string_equals_literal(lcname, ZEND_CALLSTATIC_FUNC_NAME)) {
		if (fptr->common.num_args != 2) {
			zend_error(error_type, "Method %s::__callStatic() must take exactly 2 arguments", ZSTR_VAL(ce->name));
		} else if (QUICK_ARG_SHOULD_BE_SENT_BY_REF(fptr, 1) || QUICK_ARG_SHOULD_BE_SENT_BY_REF(fptr, 2)) {
			zend_error(error_type, "Method %s::__callStatic() cannot take arguments by reference", ZSTR_VAL(ce->name));
		}
	} else if (zend_string_equals_literal(lcname, ZEND_TOSTRING_FUNC_NAME) && fptr->common.num_args != 0) {
		zend_error(error_type, "Method %s::__toString() cannot take arguments", ZSTR_VAL(ce->name));
	} else if (zend_string_equals_literal(lcname, ZEND_DEBUGINFO_FUNC_NAME) && fptr->common.num_args != 0) {
		zend_error(error_type, "Method %s::__debugInfo() cannot take arguments", ZSTR_VAL(ce->name));
	} else if (zend_string_equals_literal(lcname, "__serialize") && fptr->common.num_args != 0) {
		zend_error(error_type, "Method %s::__serialize() cannot take arguments", ZSTR_VAL(ce->name));
	} else if (zend_string_equals_literal(lcname, "__unserialize") && fptr->common.num_args != 1) {
		zend_error(error_type, "Method %s::__unserialize() must take exactly 1 argument", ZSTR_VAL(ce->name));
	}
}
/* }}} */

/* registers all functions in *library_functions in the function hash */
ZEND_API int zend_register_functions(zend_class_entry *scope, const zend_function_entry *functions, HashTable *function_table, int type) /* {{{ */
{
	const zend_function_entry *ptr = functions;
	zend_function function, *reg_function;
	zend_internal_function *internal_function = (zend_internal_function *)&function;
	int count=0, unload=0;
	HashTable *target_function_table = function_table;
	int error_type;
	zend_function *ctor = NULL, *dtor = NULL, *clone = NULL, *__get = NULL, *__set = NULL, *__unset = NULL, *__isset = NULL, *__call = NULL, *__callstatic = NULL, *__tostring = NULL, *__debugInfo = NULL, *serialize_func = NULL, *unserialize_func = NULL;
	zend_string *lowercase_name;
	size_t fname_len;
	const char *lc_class_name = NULL;
	size_t class_name_len = 0;

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
	memset(internal_function->reserved, 0, ZEND_MAX_RESERVED_RESOURCES * sizeof(void*));

	if (scope) {
		class_name_len = ZSTR_LEN(scope->name);
		if ((lc_class_name = zend_memrchr(ZSTR_VAL(scope->name), '\\', class_name_len))) {
			++lc_class_name;
			class_name_len -= (lc_class_name - ZSTR_VAL(scope->name));
			lc_class_name = zend_str_tolower_dup(lc_class_name, class_name_len);
		} else {
			lc_class_name = zend_str_tolower_dup(ZSTR_VAL(scope->name), class_name_len);
		}
	}

	while (ptr->fname) {
		fname_len = strlen(ptr->fname);
		internal_function->handler = ptr->handler;
		internal_function->function_name = zend_string_init_interned(ptr->fname, fname_len, 1);
		internal_function->scope = scope;
		internal_function->prototype = NULL;
		if (ptr->flags) {
			if (!(ptr->flags & ZEND_ACC_PPP_MASK)) {
				if (ptr->flags != ZEND_ACC_DEPRECATED && scope) {
					zend_error(error_type, "Invalid access level for %s%s%s() - access must be exactly one of public, protected or private", scope ? ZSTR_VAL(scope->name) : "", scope ? "::" : "", ptr->fname);
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
			if (info->required_num_args == (zend_uintptr_t)-1) {
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
				efree((char*)lc_class_name);
				zend_error(error_type, "Interface %s cannot contain non abstract method %s()", ZSTR_VAL(scope->name), ptr->fname);
				return FAILURE;
			}
			if (!internal_function->handler) {
				if (scope) {
					efree((char*)lc_class_name);
				}
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

		/* If types of arguments have to be checked */
		if (reg_function->common.arg_info && reg_function->common.num_args) {
			uint32_t i;
			for (i = 0; i < reg_function->common.num_args; i++) {
				zend_arg_info *arg_info = &reg_function->common.arg_info[i];
				ZEND_ASSERT(arg_info->name && "Parameter must have a name");
				if (ZEND_TYPE_IS_SET(arg_info->type)) {
				    reg_function->common.fn_flags |= ZEND_ACC_HAS_TYPE_HINTS;
				}
			}
		}

		if (reg_function->common.arg_info &&
		    (reg_function->common.fn_flags & (ZEND_ACC_HAS_RETURN_TYPE|ZEND_ACC_HAS_TYPE_HINTS))) {
			/* convert "const char*" class type names into "zend_string*" */
			uint32_t i;
			uint32_t num_args = reg_function->common.num_args + 1;
			zend_arg_info *arg_info = reg_function->common.arg_info - 1;
			zend_arg_info *new_arg_info;

			if (reg_function->common.fn_flags & ZEND_ACC_VARIADIC) {
				num_args++;
			}
			new_arg_info = malloc(sizeof(zend_arg_info) * num_args);
			memcpy(new_arg_info, arg_info, sizeof(zend_arg_info) * num_args);
			reg_function->common.arg_info = new_arg_info + 1;
			for (i = 0; i < num_args; i++) {
				if (ZEND_TYPE_HAS_CLASS(new_arg_info[i].type)) {
					ZEND_ASSERT(ZEND_TYPE_HAS_NAME(new_arg_info[i].type)
						&& "Only simple classes are currently supported");
					const char *class_name = ZEND_TYPE_LITERAL_NAME(new_arg_info[i].type);
					ZEND_TYPE_SET_PTR(new_arg_info[i].type,
						zend_string_init_interned(class_name, strlen(class_name), 1));
				}
			}
		}

		if (scope) {
			/* Look for ctor, dtor, clone */
			if (zend_string_equals_literal(lowercase_name, "serialize")) {
				serialize_func = reg_function;
			} else if (zend_string_equals_literal(lowercase_name, "unserialize")) {
				unserialize_func = reg_function;
			} else if (ZSTR_VAL(lowercase_name)[0] != '_' || ZSTR_VAL(lowercase_name)[1] != '_') {
				reg_function = NULL;
			} else if (zend_string_equals_literal(lowercase_name, ZEND_CONSTRUCTOR_FUNC_NAME)) {
				ctor = reg_function;
			} else if (zend_string_equals_literal(lowercase_name, ZEND_DESTRUCTOR_FUNC_NAME)) {
				dtor = reg_function;
				if (internal_function->num_args) {
					zend_error(error_type, "Destructor %s::%s() cannot take arguments", ZSTR_VAL(scope->name), ptr->fname);
				}
			} else if (zend_string_equals_literal(lowercase_name, ZEND_CLONE_FUNC_NAME)) {
				clone = reg_function;
			} else if (zend_string_equals_literal(lowercase_name, ZEND_CALL_FUNC_NAME)) {
				__call = reg_function;
			} else if (zend_string_equals_literal(lowercase_name, ZEND_CALLSTATIC_FUNC_NAME)) {
				__callstatic = reg_function;
			} else if (zend_string_equals_literal(lowercase_name, ZEND_TOSTRING_FUNC_NAME)) {
				__tostring = reg_function;
			} else if (zend_string_equals_literal(lowercase_name, ZEND_GET_FUNC_NAME)) {
				__get = reg_function;
				scope->ce_flags |= ZEND_ACC_USE_GUARDS;
			} else if (zend_string_equals_literal(lowercase_name, ZEND_SET_FUNC_NAME)) {
				__set = reg_function;
				scope->ce_flags |= ZEND_ACC_USE_GUARDS;
			} else if (zend_string_equals_literal(lowercase_name, ZEND_UNSET_FUNC_NAME)) {
				__unset = reg_function;
				scope->ce_flags |= ZEND_ACC_USE_GUARDS;
			} else if (zend_string_equals_literal(lowercase_name, ZEND_ISSET_FUNC_NAME)) {
				__isset = reg_function;
				scope->ce_flags |= ZEND_ACC_USE_GUARDS;
			} else if (zend_string_equals_literal(lowercase_name, ZEND_DEBUGINFO_FUNC_NAME)) {
				__debugInfo = reg_function;
			} else {
				reg_function = NULL;
			}
			if (reg_function) {
				zend_check_magic_method_implementation(
					scope, reg_function, lowercase_name, error_type);
			}
		}
		ptr++;
		count++;
		zend_string_release(lowercase_name);
	}
	if (unload) { /* before unloading, display all remaining bad function in the module */
		if (scope) {
			efree((char*)lc_class_name);
		}
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
	if (scope) {
		scope->constructor = ctor;
		scope->destructor = dtor;
		scope->clone = clone;
		scope->__call = __call;
		scope->__callstatic = __callstatic;
		scope->__tostring = __tostring;
		scope->__get = __get;
		scope->__set = __set;
		scope->__unset = __unset;
		scope->__isset = __isset;
		scope->__debugInfo = __debugInfo;
		scope->serialize_func = serialize_func;
		scope->unserialize_func = unserialize_func;
		if (ctor) {
			ctor->common.fn_flags |= ZEND_ACC_CTOR;
			if (ctor->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Constructor %s::%s() cannot be static", ZSTR_VAL(scope->name), ZSTR_VAL(ctor->common.function_name));
			}
		}
		if (dtor) {
			if (dtor->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Destructor %s::%s() cannot be static", ZSTR_VAL(scope->name), ZSTR_VAL(dtor->common.function_name));
			}
		}
		if (clone) {
			if (clone->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "%s::%s() cannot be static", ZSTR_VAL(scope->name), ZSTR_VAL(clone->common.function_name));
			}
		}
		if (__call) {
			if (__call->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %s::%s() cannot be static", ZSTR_VAL(scope->name), ZSTR_VAL(__call->common.function_name));
			}
		}
		if (__callstatic) {
			if (!(__callstatic->common.fn_flags & ZEND_ACC_STATIC)) {
				zend_error(error_type, "Method %s::%s() must be static", ZSTR_VAL(scope->name), ZSTR_VAL(__callstatic->common.function_name));
			}
			__callstatic->common.fn_flags |= ZEND_ACC_STATIC;
		}
		if (__tostring) {
			if (__tostring->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %s::%s() cannot be static", ZSTR_VAL(scope->name), ZSTR_VAL(__tostring->common.function_name));
			}
		}
		if (__get) {
			if (__get->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %s::%s() cannot be static", ZSTR_VAL(scope->name), ZSTR_VAL(__get->common.function_name));
			}
		}
		if (__set) {
			if (__set->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %s::%s() cannot be static", ZSTR_VAL(scope->name), ZSTR_VAL(__set->common.function_name));
			}
		}
		if (__unset) {
			if (__unset->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %s::%s() cannot be static", ZSTR_VAL(scope->name), ZSTR_VAL(__unset->common.function_name));
			}
		}
		if (__isset) {
			if (__isset->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %s::%s() cannot be static", ZSTR_VAL(scope->name), ZSTR_VAL(__isset->common.function_name));
			}
		}
		if (__debugInfo) {
			if (__debugInfo->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %s::%s() cannot be static", ZSTR_VAL(scope->name), ZSTR_VAL(__debugInfo->common.function_name));
			}
		}

		if (ctor && (ctor->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE)) {
			zend_error_noreturn(E_CORE_ERROR, "Constructor %s::%s() cannot declare a return type", ZSTR_VAL(scope->name), ZSTR_VAL(ctor->common.function_name));
		}

		if (dtor && (dtor->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE)) {
			zend_error_noreturn(E_CORE_ERROR, "Destructor %s::%s() cannot declare a return type", ZSTR_VAL(scope->name), ZSTR_VAL(dtor->common.function_name));
		}

		if (clone && (clone->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE)) {
			zend_error_noreturn(E_CORE_ERROR, "%s::%s() cannot declare a return type", ZSTR_VAL(scope->name), ZSTR_VAL(clone->common.function_name));
		}
		efree((char*)lc_class_name);
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

ZEND_API int zend_startup_module(zend_module_entry *module) /* {{{ */
{
	if ((module = zend_register_internal_module(module)) != NULL && zend_startup_module_ex(module) == SUCCESS) {
		return SUCCESS;
	}
	return FAILURE;
}
/* }}} */

ZEND_API int zend_get_module_started(const char *module_name) /* {{{ */
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

void module_destructor(zend_module_entry *module) /* {{{ */
{

	if (module->type == MODULE_TEMPORARY) {
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
		zend_unregister_ini_entries(module->module_number);
	}

	/* Deinitilaise module globals */
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
	}

#if HAVE_LIBDL
	if (module->handle && !getenv("ZEND_DONT_UNLOAD_MODULES")) {
		DL_UNLOAD(module->handle);
	}
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

	zend_try {
		if (EG(full_tables_cleanup)) {
			zend_module_entry *module;

			ZEND_HASH_REVERSE_FOREACH_PTR(&module_registry, module) {
				if (module->request_shutdown_func) {
#if 0
					zend_printf("%s: Request shutdown\n", module->name);
#endif
					module->request_shutdown_func(module->type, module->module_number);
				}
			} ZEND_HASH_FOREACH_END();
		} else {
			zend_module_entry **p = module_request_shutdown_handlers;

			while (*p) {
				zend_module_entry *module = *p;

				module->request_shutdown_func(module->type, module->module_number);
				p++;
			}
		}
	} zend_end_try();
}
/* }}} */

ZEND_API void zend_cleanup_internal_classes(void) /* {{{ */
{
	zend_class_entry **p = class_cleanup_handlers;

	while (*p) {
		zend_cleanup_internal_class_data(*p);
		p++;
	}
}
/* }}} */

ZEND_API void zend_post_deactivate_modules(void) /* {{{ */
{
	if (EG(full_tables_cleanup)) {
		zend_module_entry *module;
		zval *zv;
		zend_string *key;

		ZEND_HASH_FOREACH_PTR(&module_registry, module) {
			if (module->post_deactivate_func) {
				module->post_deactivate_func();
			}
		} ZEND_HASH_FOREACH_END();
		ZEND_HASH_REVERSE_FOREACH_STR_KEY_VAL(&module_registry, key, zv) {
			module = Z_PTR_P(zv);
			if (module->type != MODULE_TEMPORARY) {
				break;
			}
			module_destructor(module);
			free(module);
			zend_string_release_ex(key, 0);
		} ZEND_HASH_FOREACH_END_DEL();
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
	class_entry->ce_flags = ce_flags | ZEND_ACC_CONSTANTS_UPDATED | ZEND_ACC_LINKED | ZEND_ACC_RESOLVED_PARENT | ZEND_ACC_RESOLVED_INTERFACES;
	class_entry->info.internal.module = EG(current_module);

	if (class_entry->info.internal.builtin_functions) {
		zend_register_functions(class_entry, class_entry->info.internal.builtin_functions, &class_entry->function_table, EG(current_module)->type);
	}

	lowercase_name = zend_string_tolower_ex(orig_class_entry->name, EG(current_module)->type == MODULE_PERSISTENT);
	lowercase_name = zend_new_interned_string(lowercase_name);
	zend_hash_update_ptr(CG(class_table), lowercase_name, class_entry);
	zend_string_release_ex(lowercase_name, 1);
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

ZEND_API int zend_register_class_alias_ex(const char *name, size_t name_len, zend_class_entry *ce, int persistent) /* {{{ */
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

	ZVAL_ALIAS_PTR(&zv, ce);
	ret = zend_hash_add(CG(class_table), lcname, &zv);
	zend_string_release_ex(lcname, 0);
	if (ret) {
		if (!(ce->ce_flags & ZEND_ACC_IMMUTABLE)) {
			ce->refcount++;
		}
		return SUCCESS;
	}
	return FAILURE;
}
/* }}} */

ZEND_API int zend_set_hash_symbol(zval *symbol, const char *name, int name_length, zend_bool is_ref, int num_symbol_tables, ...) /* {{{ */
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

ZEND_API int zend_disable_function(const char *function_name, size_t function_name_length) /* {{{ */
{
	return zend_hash_str_del(CG(function_table), function_name, function_name_length);
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

ZEND_API int zend_disable_class(const char *class_name, size_t class_name_length) /* {{{ */
{
	zend_class_entry *disabled_class;
	zend_string *key;
	zend_function *fn;

	key = zend_string_alloc(class_name_length, 0);
	zend_str_tolower_copy(ZSTR_VAL(key), class_name, class_name_length);
	disabled_class = zend_hash_find_ptr(CG(class_table), key);
	zend_string_release_ex(key, 0);
	if (!disabled_class) {
		return FAILURE;
	}

	INIT_CLASS_ENTRY_INIT_METHODS((*disabled_class), disabled_class_new);
	disabled_class->create_object = display_disabled_class;

	ZEND_HASH_FOREACH_PTR(&disabled_class->function_table, fn) {
		if ((fn->common.fn_flags & (ZEND_ACC_HAS_RETURN_TYPE|ZEND_ACC_HAS_TYPE_HINTS)) &&
			fn->common.scope == disabled_class) {
			zend_free_internal_arg_info(&fn->internal_function);
		}
	} ZEND_HASH_FOREACH_END();
	zend_hash_clean(&disabled_class->function_table);
	return SUCCESS;
}
/* }}} */

static int zend_is_callable_check_class(zend_string *name, zend_class_entry *scope, zend_fcall_info_cache *fcc, int *strict_class, char **error) /* {{{ */
{
	int ret = 0;
	zend_class_entry *ce;
	size_t name_len = ZSTR_LEN(name);
	zend_string *lcname;
	ALLOCA_FLAG(use_heap);

	ZSTR_ALLOCA_ALLOC(lcname, name_len, use_heap);
	zend_str_tolower_copy(ZSTR_VAL(lcname), ZSTR_VAL(name), name_len);

	*strict_class = 0;
	if (zend_string_equals_literal(lcname, "self")) {
		if (!scope) {
			if (error) *error = estrdup("cannot access self:: when no class scope is active");
		} else {
			fcc->called_scope = zend_get_called_scope(EG(current_execute_data));
			fcc->calling_scope = scope;
			if (!fcc->object) {
				fcc->object = zend_get_this_object(EG(current_execute_data));
			}
			ret = 1;
		}
	} else if (zend_string_equals_literal(lcname, "parent")) {
		if (!scope) {
			if (error) *error = estrdup("cannot access parent:: when no class scope is active");
		} else if (!scope->parent) {
			if (error) *error = estrdup("cannot access parent:: when current class scope has no parent");
		} else {
			fcc->called_scope = zend_get_called_scope(EG(current_execute_data));
			fcc->calling_scope = scope->parent;
			if (!fcc->object) {
				fcc->object = zend_get_this_object(EG(current_execute_data));
			}
			*strict_class = 1;
			ret = 1;
		}
	} else if (zend_string_equals_literal(lcname, "static")) {
		zend_class_entry *called_scope = zend_get_called_scope(EG(current_execute_data));

		if (!called_scope) {
			if (error) *error = estrdup("cannot access static:: when no class scope is active");
		} else {
			fcc->called_scope = called_scope;
			fcc->calling_scope = called_scope;
			if (!fcc->object) {
				fcc->object = zend_get_this_object(EG(current_execute_data));
			}
			*strict_class = 1;
			ret = 1;
		}
	} else if ((ce = zend_lookup_class(name)) != NULL) {
		zend_class_entry *scope;
		zend_execute_data *ex = EG(current_execute_data);

		while (ex && (!ex->func || !ZEND_USER_CODE(ex->func->type))) {
			ex = ex->prev_execute_data;
		}
		scope = ex ? ex->func->common.scope : NULL;
		fcc->calling_scope = ce;
		if (scope && !fcc->object) {
			zend_object *object = zend_get_this_object(EG(current_execute_data));

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
		if (error) zend_spprintf(error, 0, "class '%.*s' not found", (int)name_len, ZSTR_VAL(name));
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
	}
	fcc->function_handler = NULL;
}

static zend_always_inline int zend_is_callable_check_func(int check_flags, zval *callable, zend_fcall_info_cache *fcc, int strict_class, char **error) /* {{{ */
{
	zend_class_entry *ce_org = fcc->calling_scope;
	int retval = 0;
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
			scope = zend_get_executed_scope();
		}

		cname = zend_string_init(Z_STRVAL_P(callable), clen, 0);
		if (!zend_is_callable_check_class(cname, scope, fcc, &strict_class, error)) {
			zend_string_release_ex(cname, 0);
			return 0;
		}
		zend_string_release_ex(cname, 0);

		ftable = &fcc->calling_scope->function_table;
		if (ce_org && !instanceof_function(ce_org, fcc->calling_scope)) {
			if (error) zend_spprintf(error, 0, "class '%s' is not a subclass of '%s'", ZSTR_VAL(ce_org->name), ZSTR_VAL(fcc->calling_scope->name));
			return 0;
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
		if (error && !(check_flags & IS_CALLABLE_CHECK_SILENT)) {
			zend_spprintf(error, 0, "function '%s' not found or invalid function name", Z_STRVAL_P(callable));
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
			scope = zend_get_executed_scope();
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
			scope = zend_get_executed_scope();
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
					zend_object *object = zend_get_this_object(EG(current_execute_data));
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
				scope = zend_get_executed_scope();
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
	} else if (error && !(check_flags & IS_CALLABLE_CHECK_SILENT)) {
		if (fcc->calling_scope) {
			if (error) zend_spprintf(error, 0, "class '%s' does not have a method '%s'", ZSTR_VAL(fcc->calling_scope->name), ZSTR_VAL(mname));
		} else {
			if (error) zend_spprintf(error, 0, "function '%s' does not exist", ZSTR_VAL(mname));
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

static zend_always_inline zend_bool zend_is_callable_impl(zval *callable, zend_object *object, uint32_t check_flags, zend_fcall_info_cache *fcc, char **error) /* {{{ */
{
	zend_bool ret;
	zend_fcall_info_cache fcc_local;
	int strict_class = 0;

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
			ret = zend_is_callable_check_func(check_flags, callable, fcc, strict_class, error);
			if (fcc == &fcc_local) {
				zend_release_fcall_info_cache(fcc);
			}
			return ret;

		case IS_ARRAY:
			{
				zval *method = NULL;
				zval *obj = NULL;

				if (zend_hash_num_elements(Z_ARRVAL_P(callable)) == 2) {
					obj = zend_hash_index_find(Z_ARRVAL_P(callable), 0);
					method = zend_hash_index_find(Z_ARRVAL_P(callable), 1);
				}

				do {
					if (obj == NULL || method == NULL) {
						break;
					}

					ZVAL_DEREF(method);
					if (Z_TYPE_P(method) != IS_STRING) {
						break;
					}

					ZVAL_DEREF(obj);
					if (Z_TYPE_P(obj) == IS_STRING) {
						if (check_flags & IS_CALLABLE_CHECK_SYNTAX_ONLY) {
							return 1;
						}

						if (!zend_is_callable_check_class(Z_STR_P(obj), zend_get_executed_scope(), fcc, &strict_class, error)) {
							return 0;
						}

					} else if (Z_TYPE_P(obj) == IS_OBJECT) {

						fcc->calling_scope = Z_OBJCE_P(obj); /* TBFixed: what if it's overloaded? */

						fcc->object = Z_OBJ_P(obj);

						if (check_flags & IS_CALLABLE_CHECK_SYNTAX_ONLY) {
							fcc->called_scope = fcc->calling_scope;
							return 1;
						}
					} else {
						break;
					}

					callable = method;
					goto check_func;

				} while (0);
				if (zend_hash_num_elements(Z_ARRVAL_P(callable)) == 2) {
					if (!obj || (!Z_ISREF_P(obj)?
								(Z_TYPE_P(obj) != IS_STRING && Z_TYPE_P(obj) != IS_OBJECT) :
								(Z_TYPE_P(Z_REFVAL_P(obj)) != IS_STRING && Z_TYPE_P(Z_REFVAL_P(obj)) != IS_OBJECT))) {
						if (error) *error = estrdup("first array member is not a valid class name or object");
					} else {
						if (error) *error = estrdup("second array member is not a valid method");
					}
				} else {
					if (error) *error = estrdup("array must have exactly two members");
				}
			}
			return 0;
		case IS_OBJECT:
			if (Z_OBJ_HANDLER_P(callable, get_closure) && Z_OBJ_HANDLER_P(callable, get_closure)(Z_OBJ_P(callable), &fcc->calling_scope, &fcc->function_handler, &fcc->object, 1) == SUCCESS) {
				fcc->called_scope = fcc->calling_scope;
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

ZEND_API zend_bool zend_is_callable_ex(zval *callable, zend_object *object, uint32_t check_flags, zend_string **callable_name, zend_fcall_info_cache *fcc, char **error) /* {{{ */
{
	zend_bool ret = zend_is_callable_impl(callable, object, check_flags, fcc, error);
	if (callable_name) {
		*callable_name = zend_get_callable_name_ex(callable, object);
	}
	return ret;
}

ZEND_API zend_bool zend_is_callable(zval *callable, uint32_t check_flags, zend_string **callable_name) /* {{{ */
{
	return zend_is_callable_ex(callable, NULL, check_flags, callable_name, NULL, NULL);
}
/* }}} */

ZEND_API zend_bool zend_make_callable(zval *callable, zend_string **callable_name) /* {{{ */
{
	zend_fcall_info_cache fcc;

	if (zend_is_callable_ex(callable, NULL, 0, callable_name, &fcc, NULL)) {
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

ZEND_API int zend_fcall_info_init(zval *callable, uint32_t check_flags, zend_fcall_info *fci, zend_fcall_info_cache *fcc, zend_string **callable_name, char **error) /* {{{ */
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
	fci->no_separation = 1;

	return SUCCESS;
}
/* }}} */

ZEND_API void zend_fcall_info_args_clear(zend_fcall_info *fci, int free_mem) /* {{{ */
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

ZEND_API void zend_fcall_info_args_save(zend_fcall_info *fci, int *param_count, zval **params) /* {{{ */
{
	*param_count = fci->param_count;
	*params = fci->params;
	fci->param_count = 0;
	fci->params = NULL;
}
/* }}} */

ZEND_API void zend_fcall_info_args_restore(zend_fcall_info *fci, int param_count, zval *params) /* {{{ */
{
	zend_fcall_info_args_clear(fci, 1);
	fci->param_count = param_count;
	fci->params = params;
}
/* }}} */

ZEND_API int zend_fcall_info_args_ex(zend_fcall_info *fci, zend_function *func, zval *args) /* {{{ */
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

ZEND_API int zend_fcall_info_args(zend_fcall_info *fci, zval *args) /* {{{ */
{
	return zend_fcall_info_args_ex(fci, NULL, args);
}
/* }}} */

ZEND_API int zend_fcall_info_argp(zend_fcall_info *fci, int argc, zval *argv) /* {{{ */
{
	int i;

	if (argc < 0) {
		return FAILURE;
	}

	zend_fcall_info_args_clear(fci, !argc);

	if (argc) {
		fci->param_count = argc;
		fci->params = (zval *) erealloc(fci->params, fci->param_count * sizeof(zval));

		for (i = 0; i < argc; ++i) {
			ZVAL_COPY(&fci->params[i], &argv[i]);
		}
	}

	return SUCCESS;
}
/* }}} */

ZEND_API int zend_fcall_info_argv(zend_fcall_info *fci, int argc, va_list *argv) /* {{{ */
{
	int i;
	zval *arg;

	if (argc < 0) {
		return FAILURE;
	}

	zend_fcall_info_args_clear(fci, !argc);

	if (argc) {
		fci->param_count = argc;
		fci->params = (zval *) erealloc(fci->params, fci->param_count * sizeof(zval));

		for (i = 0; i < argc; ++i) {
			arg = va_arg(*argv, zval *);
			ZVAL_COPY(&fci->params[i], arg);
		}
	}

	return SUCCESS;
}
/* }}} */

ZEND_API int zend_fcall_info_argn(zend_fcall_info *fci, int argc, ...) /* {{{ */
{
	int ret;
	va_list argv;

	va_start(argv, argc);
	ret = zend_fcall_info_argv(fci, argc, &argv);
	va_end(argv);

	return ret;
}
/* }}} */

ZEND_API int zend_fcall_info_call(zend_fcall_info *fci, zend_fcall_info_cache *fcc, zval *retval_ptr, zval *args) /* {{{ */
{
	zval retval, *org_params = NULL;
	int result, org_count = 0;

	fci->retval = retval_ptr ? retval_ptr : &retval;
	if (args) {
		zend_fcall_info_args_save(fci, &org_count, &org_params);
		zend_fcall_info_args(fci, args);
	}
	result = zend_call_function(fci, fcc);

	if (!retval_ptr && Z_TYPE(retval) != IS_UNDEF) {
		zval_ptr_dtor(&retval);
	}
	if (args) {
		zend_fcall_info_args_restore(fci, org_count, org_params);
	}
	return result;
}
/* }}} */

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

static inline zend_string *zval_make_interned_string(zval *zv) /* {{{ */
{
	ZEND_ASSERT(Z_TYPE_P(zv) == IS_STRING);
	Z_STR_P(zv) = zend_new_interned_string(Z_STR_P(zv));
	if (ZSTR_IS_INTERNED(Z_STR_P(zv))) {
		Z_TYPE_FLAGS_P(zv) = 0;
	}
	return Z_STR_P(zv);
}

static zend_always_inline zend_bool is_persistent_class(zend_class_entry *ce) {
	return (ce->type & ZEND_INTERNAL_CLASS)
		&& ce->info.internal.module->type == MODULE_PERSISTENT;
}

ZEND_API zend_property_info *zend_declare_typed_property(zend_class_entry *ce, zend_string *name, zval *property, int access_type, zend_string *doc_comment, zend_type type) /* {{{ */
{
	zend_property_info *property_info, *property_info_ptr;

	if (ZEND_TYPE_IS_SET(type)) {
		ce->ce_flags |= ZEND_ACC_HAS_TYPE_HINTS;
	}

	if (ce->type == ZEND_INTERNAL_CLASS) {
		property_info = pemalloc(sizeof(zend_property_info), 1);
	} else {
		property_info = zend_arena_alloc(&CG(arena), sizeof(zend_property_info));
		if (Z_TYPE_P(property) == IS_CONSTANT_AST) {
			ce->ce_flags &= ~ZEND_ACC_CONSTANTS_UPDATED;
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
			ZEND_ASSERT(ce->type == ZEND_INTERNAL_CLASS);
			if (!EG(current_execute_data)) {
				ZEND_MAP_PTR_NEW(ce->static_members_table);
			} else {
				/* internal class loaded by dl() */
				ZEND_MAP_PTR_INIT(ce->static_members_table, &ce->default_static_members_table);
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

	zend_hash_update_ptr(&ce->properties_info, name, property_info);

	return property_info;
}
/* }}} */

ZEND_API int zend_try_assign_typed_ref_ex(zend_reference *ref, zval *val, zend_bool strict) /* {{{ */
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

ZEND_API int zend_try_assign_typed_ref(zend_reference *ref, zval *val) /* {{{ */
{
	return zend_try_assign_typed_ref_ex(ref, val, ZEND_ARG_USES_STRICT_TYPES());
}
/* }}} */

ZEND_API int zend_try_assign_typed_ref_null(zend_reference *ref) /* {{{ */
{
	zval tmp;

	ZVAL_NULL(&tmp);
	return zend_try_assign_typed_ref(ref, &tmp);
}
/* }}} */

ZEND_API int zend_try_assign_typed_ref_bool(zend_reference *ref, zend_bool val) /* {{{ */
{
	zval tmp;

	ZVAL_BOOL(&tmp, val);
	return zend_try_assign_typed_ref(ref, &tmp);
}
/* }}} */

ZEND_API int zend_try_assign_typed_ref_long(zend_reference *ref, zend_long lval) /* {{{ */
{
	zval tmp;

	ZVAL_LONG(&tmp, lval);
	return zend_try_assign_typed_ref(ref, &tmp);
}
/* }}} */

ZEND_API int zend_try_assign_typed_ref_double(zend_reference *ref, double dval) /* {{{ */
{
	zval tmp;

	ZVAL_DOUBLE(&tmp, dval);
	return zend_try_assign_typed_ref(ref, &tmp);
}
/* }}} */

ZEND_API int zend_try_assign_typed_ref_empty_string(zend_reference *ref) /* {{{ */
{
	zval tmp;

	ZVAL_EMPTY_STRING(&tmp);
	return zend_try_assign_typed_ref(ref, &tmp);
}
/* }}} */

ZEND_API int zend_try_assign_typed_ref_str(zend_reference *ref, zend_string *str) /* {{{ */
{
	zval tmp;

	ZVAL_STR(&tmp, str);
	return zend_try_assign_typed_ref(ref, &tmp);
}
/* }}} */

ZEND_API int zend_try_assign_typed_ref_string(zend_reference *ref, const char *string) /* {{{ */
{
	zval tmp;

	ZVAL_STRING(&tmp, string);
	return zend_try_assign_typed_ref(ref, &tmp);
}
/* }}} */

ZEND_API int zend_try_assign_typed_ref_stringl(zend_reference *ref, const char *string, size_t len) /* {{{ */
{
	zval tmp;

	ZVAL_STRINGL(&tmp, string, len);
	return zend_try_assign_typed_ref(ref, &tmp);
}
/* }}} */

ZEND_API int zend_try_assign_typed_ref_arr(zend_reference *ref, zend_array *arr) /* {{{ */
{
	zval tmp;

	ZVAL_ARR(&tmp, arr);
	return zend_try_assign_typed_ref(ref, &tmp);
}
/* }}} */

ZEND_API int zend_try_assign_typed_ref_res(zend_reference *ref, zend_resource *res) /* {{{ */
{
	zval tmp;

	ZVAL_RES(&tmp, res);
	return zend_try_assign_typed_ref(ref, &tmp);
}
/* }}} */

ZEND_API int zend_try_assign_typed_ref_zval(zend_reference *ref, zval *zv) /* {{{ */
{
	zval tmp;

	ZVAL_COPY_VALUE(&tmp, zv);
	return zend_try_assign_typed_ref(ref, &tmp);
}
/* }}} */

ZEND_API int zend_try_assign_typed_ref_zval_ex(zend_reference *ref, zval *zv, zend_bool strict) /* {{{ */
{
	zval tmp;

	ZVAL_COPY_VALUE(&tmp, zv);
	return zend_try_assign_typed_ref_ex(ref, &tmp, strict);
}
/* }}} */

ZEND_API int zend_declare_property_ex(zend_class_entry *ce, zend_string *name, zval *property, int access_type, zend_string *doc_comment) /* {{{ */
{
	zend_declare_typed_property(ce, name, property, access_type, doc_comment, (zend_type) ZEND_TYPE_INIT_NONE(0));
	return SUCCESS;
}
/* }}} */

ZEND_API int zend_declare_property(zend_class_entry *ce, const char *name, size_t name_length, zval *property, int access_type) /* {{{ */
{
	zend_string *key = zend_string_init(name, name_length, is_persistent_class(ce));
	int ret = zend_declare_property_ex(ce, key, property, access_type, NULL);
	zend_string_release(key);
	return ret;
}
/* }}} */

ZEND_API int zend_declare_property_null(zend_class_entry *ce, const char *name, size_t name_length, int access_type) /* {{{ */
{
	zval property;

	ZVAL_NULL(&property);
	return zend_declare_property(ce, name, name_length, &property, access_type);
}
/* }}} */

ZEND_API int zend_declare_property_bool(zend_class_entry *ce, const char *name, size_t name_length, zend_long value, int access_type) /* {{{ */
{
	zval property;

	ZVAL_BOOL(&property, value);
	return zend_declare_property(ce, name, name_length, &property, access_type);
}
/* }}} */

ZEND_API int zend_declare_property_long(zend_class_entry *ce, const char *name, size_t name_length, zend_long value, int access_type) /* {{{ */
{
	zval property;

	ZVAL_LONG(&property, value);
	return zend_declare_property(ce, name, name_length, &property, access_type);
}
/* }}} */

ZEND_API int zend_declare_property_double(zend_class_entry *ce, const char *name, size_t name_length, double value, int access_type) /* {{{ */
{
	zval property;

	ZVAL_DOUBLE(&property, value);
	return zend_declare_property(ce, name, name_length, &property, access_type);
}
/* }}} */

ZEND_API int zend_declare_property_string(zend_class_entry *ce, const char *name, size_t name_length, const char *value, int access_type) /* {{{ */
{
	zval property;

	ZVAL_NEW_STR(&property, zend_string_init(value, strlen(value), ce->type & ZEND_INTERNAL_CLASS));
	return zend_declare_property(ce, name, name_length, &property, access_type);
}
/* }}} */

ZEND_API int zend_declare_property_stringl(zend_class_entry *ce, const char *name, size_t name_length, const char *value, size_t value_len, int access_type) /* {{{ */
{
	zval property;

	ZVAL_NEW_STR(&property, zend_string_init(value, value_len, ce->type & ZEND_INTERNAL_CLASS));
	return zend_declare_property(ce, name, name_length, &property, access_type);
}
/* }}} */

ZEND_API zend_class_constant *zend_declare_class_constant_ex(zend_class_entry *ce, zend_string *name, zval *value, int access_type, zend_string *doc_comment) /* {{{ */
{
	zend_class_constant *c;

	if (ce->ce_flags & ZEND_ACC_INTERFACE) {
		if (access_type != ZEND_ACC_PUBLIC) {
			zend_error_noreturn(E_COMPILE_ERROR, "Access type for interface constant %s::%s must be public", ZSTR_VAL(ce->name), ZSTR_VAL(name));
		}
	}

	if (zend_string_equals_literal_ci(name, "class")) {
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
	Z_ACCESS_FLAGS(c->value) = access_type;
	c->doc_comment = doc_comment;
	c->attributes = NULL;
	c->ce = ce;
	if (Z_TYPE_P(value) == IS_CONSTANT_AST) {
		ce->ce_flags &= ~ZEND_ACC_CONSTANTS_UPDATED;
	}

	if (!zend_hash_add_ptr(&ce->constants_table, name, c)) {
		zend_error_noreturn(ce->type == ZEND_INTERNAL_CLASS ? E_CORE_ERROR : E_COMPILE_ERROR,
			"Cannot redefine class constant %s::%s", ZSTR_VAL(ce->name), ZSTR_VAL(name));
	}

	return c;
}
/* }}} */

ZEND_API int zend_declare_class_constant(zend_class_entry *ce, const char *name, size_t name_length, zval *value) /* {{{ */
{
	zend_string *key;

	if (ce->type == ZEND_INTERNAL_CLASS) {
		key = zend_string_init_interned(name, name_length, 1);
	} else {
		key = zend_string_init(name, name_length, 0);
	}
	zend_declare_class_constant_ex(ce, key, value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(key);
	return SUCCESS;
}
/* }}} */

ZEND_API int zend_declare_class_constant_null(zend_class_entry *ce, const char *name, size_t name_length) /* {{{ */
{
	zval constant;

	ZVAL_NULL(&constant);
	return zend_declare_class_constant(ce, name, name_length, &constant);
}
/* }}} */

ZEND_API int zend_declare_class_constant_long(zend_class_entry *ce, const char *name, size_t name_length, zend_long value) /* {{{ */
{
	zval constant;

	ZVAL_LONG(&constant, value);
	return zend_declare_class_constant(ce, name, name_length, &constant);
}
/* }}} */

ZEND_API int zend_declare_class_constant_bool(zend_class_entry *ce, const char *name, size_t name_length, zend_bool value) /* {{{ */
{
	zval constant;

	ZVAL_BOOL(&constant, value);
	return zend_declare_class_constant(ce, name, name_length, &constant);
}
/* }}} */

ZEND_API int zend_declare_class_constant_double(zend_class_entry *ce, const char *name, size_t name_length, double value) /* {{{ */
{
	zval constant;

	ZVAL_DOUBLE(&constant, value);
	return zend_declare_class_constant(ce, name, name_length, &constant);
}
/* }}} */

ZEND_API int zend_declare_class_constant_stringl(zend_class_entry *ce, const char *name, size_t name_length, const char *value, size_t value_length) /* {{{ */
{
	zval constant;

	ZVAL_NEW_STR(&constant, zend_string_init(value, value_length, ce->type & ZEND_INTERNAL_CLASS));
	return zend_declare_class_constant(ce, name, name_length, &constant);
}
/* }}} */

ZEND_API int zend_declare_class_constant_string(zend_class_entry *ce, const char *name, size_t name_length, const char *value) /* {{{ */
{
	return zend_declare_class_constant_stringl(ce, name, name_length, value, strlen(value));
}
/* }}} */

ZEND_API void zend_update_property_ex(zend_class_entry *scope, zval *object, zend_string *name, zval *value) /* {{{ */
{
	zend_class_entry *old_scope = EG(fake_scope);

	EG(fake_scope) = scope;

	Z_OBJ_HT_P(object)->write_property(Z_OBJ_P(object), name, value, NULL);

	EG(fake_scope) = old_scope;
}
/* }}} */

ZEND_API void zend_update_property(zend_class_entry *scope, zval *object, const char *name, size_t name_length, zval *value) /* {{{ */
{
	zend_string *property;
	zend_class_entry *old_scope = EG(fake_scope);

	EG(fake_scope) = scope;

	property = zend_string_init(name, name_length, 0);
	Z_OBJ_HT_P(object)->write_property(Z_OBJ_P(object), property, value, NULL);
	zend_string_release_ex(property, 0);

	EG(fake_scope) = old_scope;
}
/* }}} */

ZEND_API void zend_update_property_null(zend_class_entry *scope, zval *object, const char *name, size_t name_length) /* {{{ */
{
	zval tmp;

	ZVAL_NULL(&tmp);
	zend_update_property(scope, object, name, name_length, &tmp);
}
/* }}} */

ZEND_API void zend_unset_property(zend_class_entry *scope, zval *object, const char *name, size_t name_length) /* {{{ */
{
	zend_string *property;
	zend_class_entry *old_scope = EG(fake_scope);

	EG(fake_scope) = scope;

	property = zend_string_init(name, name_length, 0);
	Z_OBJ_HT_P(object)->unset_property(Z_OBJ_P(object), property, 0);
	zend_string_release_ex(property, 0);

	EG(fake_scope) = old_scope;
}
/* }}} */

ZEND_API void zend_update_property_bool(zend_class_entry *scope, zval *object, const char *name, size_t name_length, zend_long value) /* {{{ */
{
	zval tmp;

	ZVAL_BOOL(&tmp, value);
	zend_update_property(scope, object, name, name_length, &tmp);
}
/* }}} */

ZEND_API void zend_update_property_long(zend_class_entry *scope, zval *object, const char *name, size_t name_length, zend_long value) /* {{{ */
{
	zval tmp;

	ZVAL_LONG(&tmp, value);
	zend_update_property(scope, object, name, name_length, &tmp);
}
/* }}} */

ZEND_API void zend_update_property_double(zend_class_entry *scope, zval *object, const char *name, size_t name_length, double value) /* {{{ */
{
	zval tmp;

	ZVAL_DOUBLE(&tmp, value);
	zend_update_property(scope, object, name, name_length, &tmp);
}
/* }}} */

ZEND_API void zend_update_property_str(zend_class_entry *scope, zval *object, const char *name, size_t name_length, zend_string *value) /* {{{ */
{
	zval tmp;

	ZVAL_STR(&tmp, value);
	zend_update_property(scope, object, name, name_length, &tmp);
}
/* }}} */

ZEND_API void zend_update_property_string(zend_class_entry *scope, zval *object, const char *name, size_t name_length, const char *value) /* {{{ */
{
	zval tmp;

	ZVAL_STRING(&tmp, value);
	Z_SET_REFCOUNT(tmp, 0);
	zend_update_property(scope, object, name, name_length, &tmp);
}
/* }}} */

ZEND_API void zend_update_property_stringl(zend_class_entry *scope, zval *object, const char *name, size_t name_length, const char *value, size_t value_len) /* {{{ */
{
	zval tmp;

	ZVAL_STRINGL(&tmp, value, value_len);
	Z_SET_REFCOUNT(tmp, 0);
	zend_update_property(scope, object, name, name_length, &tmp);
}
/* }}} */

ZEND_API int zend_update_static_property_ex(zend_class_entry *scope, zend_string *name, zval *value) /* {{{ */
{
	zval *property, tmp;
	zend_property_info *prop_info;
	zend_class_entry *old_scope = EG(fake_scope);

	if (UNEXPECTED(!(scope->ce_flags & ZEND_ACC_CONSTANTS_UPDATED))) {
		if (UNEXPECTED(zend_update_class_constants(scope)) != SUCCESS) {
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

ZEND_API int zend_update_static_property(zend_class_entry *scope, const char *name, size_t name_length, zval *value) /* {{{ */
{
	zend_string *key = zend_string_init(name, name_length, 0);
	int retval = zend_update_static_property_ex(scope, key, value);
	zend_string_efree(key);
	return retval;
}
/* }}} */

ZEND_API int zend_update_static_property_null(zend_class_entry *scope, const char *name, size_t name_length) /* {{{ */
{
	zval tmp;

	ZVAL_NULL(&tmp);
	return zend_update_static_property(scope, name, name_length, &tmp);
}
/* }}} */

ZEND_API int zend_update_static_property_bool(zend_class_entry *scope, const char *name, size_t name_length, zend_long value) /* {{{ */
{
	zval tmp;

	ZVAL_BOOL(&tmp, value);
	return zend_update_static_property(scope, name, name_length, &tmp);
}
/* }}} */

ZEND_API int zend_update_static_property_long(zend_class_entry *scope, const char *name, size_t name_length, zend_long value) /* {{{ */
{
	zval tmp;

	ZVAL_LONG(&tmp, value);
	return zend_update_static_property(scope, name, name_length, &tmp);
}
/* }}} */

ZEND_API int zend_update_static_property_double(zend_class_entry *scope, const char *name, size_t name_length, double value) /* {{{ */
{
	zval tmp;

	ZVAL_DOUBLE(&tmp, value);
	return zend_update_static_property(scope, name, name_length, &tmp);
}
/* }}} */

ZEND_API int zend_update_static_property_string(zend_class_entry *scope, const char *name, size_t name_length, const char *value) /* {{{ */
{
	zval tmp;

	ZVAL_STRING(&tmp, value);
	Z_SET_REFCOUNT(tmp, 0);
	return zend_update_static_property(scope, name, name_length, &tmp);
}
/* }}} */

ZEND_API int zend_update_static_property_stringl(zend_class_entry *scope, const char *name, size_t name_length, const char *value, size_t value_len) /* {{{ */
{
	zval tmp;

	ZVAL_STRINGL(&tmp, value, value_len);
	Z_SET_REFCOUNT(tmp, 0);
	return zend_update_static_property(scope, name, name_length, &tmp);
}
/* }}} */

ZEND_API zval *zend_read_property_ex(zend_class_entry *scope, zval *object, zend_string *name, zend_bool silent, zval *rv) /* {{{ */
{
	zval *value;
	zend_class_entry *old_scope = EG(fake_scope);

	EG(fake_scope) = scope;

	value = Z_OBJ_HT_P(object)->read_property(Z_OBJ_P(object), name, silent?BP_VAR_IS:BP_VAR_R, NULL, rv);

	EG(fake_scope) = old_scope;
	return value;
}
/* }}} */

ZEND_API zval *zend_read_property(zend_class_entry *scope, zval *object, const char *name, size_t name_length, zend_bool silent, zval *rv) /* {{{ */
{
	zval *value;
	zend_string *str;

	str = zend_string_init(name, name_length, 0);
	value = zend_read_property_ex(scope, object, str, silent, rv);
	zend_string_release_ex(str, 0);
	return value;
}
/* }}} */

ZEND_API zval *zend_read_static_property_ex(zend_class_entry *scope, zend_string *name, zend_bool silent) /* {{{ */
{
	zval *property;
	zend_class_entry *old_scope = EG(fake_scope);

	EG(fake_scope) = scope;
	property = zend_std_get_static_property(scope, name, silent ? BP_VAR_IS : BP_VAR_R);
	EG(fake_scope) = old_scope;

	return property;
}
/* }}} */

ZEND_API zval *zend_read_static_property(zend_class_entry *scope, const char *name, size_t name_length, zend_bool silent) /* {{{ */
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
	ZVAL_COPY(&current->user_handler, &EG(user_error_handler));
}
/* }}} */

ZEND_API void zend_replace_error_handling(zend_error_handling_t error_handling, zend_class_entry *exception_class, zend_error_handling *current) /* {{{ */
{
	if (current) {
		zend_save_error_handling(current);
		if (error_handling != EH_NORMAL && Z_TYPE(EG(user_error_handler)) != IS_UNDEF) {
			zval_ptr_dtor(&EG(user_error_handler));
			ZVAL_UNDEF(&EG(user_error_handler));
		}
	}
	EG(error_handling) = error_handling;
	EG(exception_class) = error_handling == EH_THROW ? exception_class : NULL;
}
/* }}} */

static int same_zval(zval *zv1, zval *zv2)  /* {{{ */
{
	if (Z_TYPE_P(zv1) != Z_TYPE_P(zv2)) {
		return 0;
	}
	switch (Z_TYPE_P(zv1)) {
		case IS_UNDEF:
		case IS_NULL:
		case IS_FALSE:
		case IS_TRUE:
			return 1;
		case IS_LONG:
			return Z_LVAL_P(zv1) == Z_LVAL_P(zv2);
		case IS_DOUBLE:
			return Z_LVAL_P(zv1) == Z_LVAL_P(zv2);
		case IS_STRING:
		case IS_ARRAY:
		case IS_OBJECT:
		case IS_RESOURCE:
			return Z_COUNTED_P(zv1) == Z_COUNTED_P(zv2);
		default:
			return 0;
	}
}
/* }}} */

ZEND_API void zend_restore_error_handling(zend_error_handling *saved) /* {{{ */
{
	EG(error_handling) = saved->handling;
	EG(exception_class) = saved->handling == EH_THROW ? saved->exception : NULL;
	if (Z_TYPE(saved->user_handler) != IS_UNDEF
		&& !same_zval(&saved->user_handler, &EG(user_error_handler))) {
		zval_ptr_dtor(&EG(user_error_handler));
		ZVAL_COPY_VALUE(&EG(user_error_handler), &saved->user_handler);
	} else if (Z_TYPE(saved->user_handler)) {
		zval_ptr_dtor(&saved->user_handler);
	}
	ZVAL_UNDEF(&saved->user_handler);
}
/* }}} */

ZEND_API ZEND_COLD const char *zend_get_object_type(const zend_class_entry *ce) /* {{{ */
{
	if(ce->ce_flags & ZEND_ACC_TRAIT) {
		return "trait";
	} else if (ce->ce_flags & ZEND_ACC_INTERFACE) {
		return "interface";
	} else {
		return "class";
	}
}
/* }}} */

ZEND_API zend_bool zend_is_iterable(zval *iterable) /* {{{ */
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

ZEND_API zend_bool zend_is_countable(zval *countable) /* {{{ */
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

static int get_default_via_ast(zval *default_value_zval, const char *default_value) {
	zend_ast *ast;
	zend_arena *ast_arena;

	zend_string *code = zend_string_concat3(
		"<?php ", sizeof("<?php ") - 1, default_value, strlen(default_value), ";", 1);

	ast = zend_compile_string_to_ast(code, &ast_arena, "");
	zend_string_release(code);

	if (!ast) {
		return FAILURE;
	}

	zend_ast_list *statement_list = zend_ast_get_list(ast);
	zend_ast *const_expression_ast = statement_list->child[0];

	zend_arena *original_ast_arena = CG(ast_arena);
	uint32_t original_compiler_options = CG(compiler_options);
	zend_file_context original_file_context;
	CG(ast_arena) = ast_arena;
	/* Disable constant substitution, to make getDefaultValueConstant() work. */
	CG(compiler_options) |= ZEND_COMPILE_NO_CONSTANT_SUBSTITUTION | ZEND_COMPILE_NO_PERSISTENT_CONSTANT_SUBSTITUTION;
	zend_file_context_begin(&original_file_context);
	zend_const_expr_to_zval(default_value_zval, const_expression_ast);
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

ZEND_API int zend_get_default_from_internal_arg_info(
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
