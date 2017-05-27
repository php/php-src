/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2017 Zend Technologies Ltd. (http://www.zend.com) |
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
   |          Andrei Zmievski <andrei@php.net>                            |
   |          Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend.h"
#include "zend_execute.h"
#include "zend_API.h"
#include "zend_modules.h"
#include "zend_extensions.h"
#include "zend_constants.h"
#include "zend_exceptions.h"
#include "zend_closures.h"
#include "zend_inheritance.h"

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

/* these variables are true statics/globals, and have to be mutex'ed on every access */
ZEND_API HashTable module_registry;

static zend_module_entry **module_request_startup_handlers;
static zend_module_entry **module_request_shutdown_handlers;
static zend_module_entry **module_post_deactivate_handlers;

static zend_class_entry  **class_cleanup_handlers;

/* this function doesn't check for too many parameters */
ZEND_API int zend_get_parameters(int ht, int param_count, ...) /* {{{ */
{
	int arg_count;
	va_list ptr;
	zval **param, *param_ptr;

	param_ptr = ZEND_CALL_ARG(EG(current_execute_data), 1);
	arg_count = ZEND_CALL_NUM_ARGS(EG(current_execute_data));

	if (param_count>arg_count) {
		return FAILURE;
	}

	va_start(ptr, param_count);

	while (param_count-->0) {
		param = va_arg(ptr, zval **);
		if (!Z_ISREF_P(param_ptr) && Z_REFCOUNT_P(param_ptr) > 1) {
			zval new_tmp;

			ZVAL_DUP(&new_tmp, param_ptr);
			Z_DELREF_P(param_ptr);
			ZVAL_COPY_VALUE(param_ptr, &new_tmp);
		}
		*param = param_ptr;
		param_ptr++;
	}
	va_end(ptr);

	return SUCCESS;
}
/* }}} */

/* Zend-optimized Extended functions */
/* this function doesn't check for too many parameters */
ZEND_API int zend_get_parameters_ex(int param_count, ...) /* {{{ */
{
	int arg_count;
	va_list ptr;
	zval **param, *param_ptr;

	param_ptr = ZEND_CALL_ARG(EG(current_execute_data), 1);
	arg_count = ZEND_CALL_NUM_ARGS(EG(current_execute_data));

	if (param_count>arg_count) {
		return FAILURE;
	}

	va_start(ptr, param_count);
	while (param_count-->0) {
		param = va_arg(ptr, zval **);
		*param = param_ptr;
		param_ptr++;
	}
	va_end(ptr);

	return SUCCESS;
}
/* }}} */

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
		if (Z_REFCOUNTED_P(param_ptr)) {
			Z_ADDREF_P(param_ptr);
		}
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

	zend_internal_type_error(ZEND_ARG_USES_STRICT_TYPES(), "Wrong parameter count for %s%s%s()", class_name, space, get_active_function_name());
}
/* }}} */

/* Argument parsing API -- andrei */
ZEND_API char *zend_get_type_by_const(int type) /* {{{ */
{
	switch(type) {
		case IS_FALSE:
		case IS_TRUE:
		case _IS_BOOL:
			return "boolean";
		case IS_LONG:
			return "integer";
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
		case IS_ARRAY:
			return "array";
		default:
			return "unknown";
	}
}
/* }}} */

ZEND_API char *zend_zval_type_name(const zval *arg) /* {{{ */
{
	ZVAL_DEREF(arg);
	return zend_get_type_by_const(Z_TYPE_P(arg));
}
/* }}} */

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_paramers_count_error(int num_args, int min_num_args, int max_num_args) /* {{{ */
{
	zend_function *active_function = EG(current_execute_data)->func;
	const char *class_name = active_function->common.scope ? ZSTR_VAL(active_function->common.scope->name) : "";

	zend_internal_type_error(ZEND_ARG_USES_STRICT_TYPES(), "%s%s%s() expects %s %d parameter%s, %d given",
		class_name, \
		class_name[0] ? "::" : "", \
		ZSTR_VAL(active_function->common.function_name),
		min_num_args == max_num_args ? "exactly" : num_args < min_num_args ? "at least" : "at most",
		num_args < min_num_args ? min_num_args : max_num_args,
		(num_args < min_num_args ? min_num_args : max_num_args) == 1 ? "" : "s",
		num_args);
}
/* }}} */

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_paramer_type_error(int num, zend_expected_type expected_type, zval *arg) /* {{{ */
{
	const char *space;
	const char *class_name = get_active_class_name(&space);
	static const char * const expected_error[] = {
		Z_EXPECTED_TYPES(Z_EXPECTED_TYPE_STR)
		NULL
	};

	zend_internal_type_error(ZEND_ARG_USES_STRICT_TYPES(), "%s%s%s() expects parameter %d to be %s, %s given",
		class_name, space, get_active_function_name(), num, expected_error[expected_type], zend_zval_type_name(arg));
}
/* }}} */

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_paramer_class_error(int num, char *name, zval *arg) /* {{{ */
{
	const char *space;
	const char *class_name = get_active_class_name(&space);

	zend_internal_type_error(ZEND_ARG_USES_STRICT_TYPES(), "%s%s%s() expects parameter %d to be %s, %s given",
		class_name, space, get_active_function_name(), num, name, zend_zval_type_name(arg));
}
/* }}} */

ZEND_API ZEND_COLD void ZEND_FASTCALL zend_wrong_callback_error(int severity, int num, char *error) /* {{{ */
{
	const char *space;
	const char *class_name = get_active_class_name(&space);

	if (severity == E_WARNING) {
		zend_internal_type_error(ZEND_ARG_USES_STRICT_TYPES(), "%s%s%s() expects parameter %d to be a valid callback, %s",
			class_name, space, get_active_function_name(), num, error);
	} else if (severity == E_ERROR) {
		zend_throw_error(zend_ce_type_error, "%s%s%s() expects parameter %d to be a valid callback, %s",
			class_name, space, get_active_function_name(), num, error);
	} else {
		zend_error(severity, "%s%s%s() expects parameter %d to be a valid callback, %s",
			class_name, space, get_active_function_name(), num, error);
	}
	efree(error);
}
/* }}} */

ZEND_API int ZEND_FASTCALL zend_parse_arg_class(zval *arg, zend_class_entry **pce, int num, int check_null) /* {{{ */
{
	zend_class_entry *ce_base = *pce;

	if (check_null && Z_TYPE_P(arg) == IS_NULL) {
		*pce = NULL;
		return 1;
	}
	convert_to_string_ex(arg);
	*pce = zend_lookup_class(Z_STR_P(arg));
	if (ce_base) {
		if ((!*pce || !instanceof_function(*pce, ce_base))) {
			const char *space;
			const char *class_name = get_active_class_name(&space);

			zend_internal_type_error(ZEND_ARG_USES_STRICT_TYPES(), "%s%s%s() expects parameter %d to be a class name derived from %s, '%s' given",
				class_name, space, get_active_function_name(), num,
				ZSTR_VAL(ce_base->name), Z_STRVAL_P(arg));
			*pce = NULL;
			return 0;
		}
	}
	if (!*pce) {
		const char *space;
		const char *class_name = get_active_class_name(&space);

		zend_internal_type_error(ZEND_ARG_USES_STRICT_TYPES(), "%s%s%s() expects parameter %d to be a valid class name, '%s' given",
			class_name, space, get_active_function_name(), num,
			Z_STRVAL_P(arg));
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

ZEND_API int ZEND_FASTCALL zend_parse_arg_long_cap_weak(zval *arg, zend_long *dest) /* {{{ */
{
	if (EXPECTED(Z_TYPE_P(arg) == IS_DOUBLE)) {
		if (UNEXPECTED(zend_isnan(Z_DVAL_P(arg)))) {
			return 0;
		}
		if (UNEXPECTED(!ZEND_DOUBLE_FITS_LONG(Z_DVAL_P(arg)))) {
			*dest = (Z_DVAL_P(arg) > 0) ? ZEND_LONG_MAX : ZEND_LONG_MIN;
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
					*dest = (d > 0) ? ZEND_LONG_MAX : ZEND_LONG_MIN;
				} else {
					*dest = zend_dval_to_lval(d);
				}
			} else {
				return 0;
			}
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

ZEND_API int ZEND_FASTCALL zend_parse_arg_long_cap_slow(zval *arg, zend_long *dest) /* {{{ */
{
	if (UNEXPECTED(ZEND_ARG_USES_STRICT_TYPES())) {
		return 0;
	}
	return zend_parse_arg_long_cap_weak(arg, dest);
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

ZEND_API int ZEND_FASTCALL zend_parse_arg_str_weak(zval *arg, zend_string **dest) /* {{{ */
{
	if (EXPECTED(Z_TYPE_P(arg) < IS_STRING)) {
		convert_to_string(arg);
		*dest = Z_STR_P(arg);
	} else if (UNEXPECTED(Z_TYPE_P(arg) == IS_OBJECT)) {
		if (Z_OBJ_HANDLER_P(arg, cast_object)) {
			zval obj;
			if (Z_OBJ_HANDLER_P(arg, cast_object)(arg, &obj, IS_STRING) == SUCCESS) {
				zval_ptr_dtor(arg);
				ZVAL_COPY_VALUE(arg, &obj);
				*dest = Z_STR_P(arg);
				return 1;
			}
		} else if (Z_OBJ_HANDLER_P(arg, get)) {
			zval rv;
			zval *z = Z_OBJ_HANDLER_P(arg, get)(arg, &rv);

			Z_ADDREF_P(z);
			if (Z_TYPE_P(z) != IS_OBJECT) {
				zval_dtor(arg);
				ZVAL_NULL(arg);
				if (!zend_make_printable_zval(z, arg)) {
					ZVAL_COPY_VALUE(arg, z);
				}
				*dest = Z_STR_P(arg);
				return 1;
			}
			zval_ptr_dtor(z);
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

static const char *zend_parse_arg_impl(int arg_num, zval *arg, va_list *va, const char **spec, char **error, int *severity) /* {{{ */
{
	const char *spec_walk = *spec;
	char c = *spec_walk++;
	int check_null = 0;
	zval *real_arg = arg;

	/* scan through modifiers */
	ZVAL_DEREF(arg);
	while (1) {
		if (*spec_walk == '/') {
			SEPARATE_ZVAL_NOREF(arg);
			real_arg = arg;
		} else if (*spec_walk == '!') {
			check_null = 1;
		} else {
			break;
		}
		spec_walk++;
	}

	switch (c) {
		case 'l':
		case 'L':
			{
				zend_long *p = va_arg(*va, zend_long *);
				zend_bool *is_null = NULL;

				if (check_null) {
					is_null = va_arg(*va, zend_bool *);
				}

				if (!zend_parse_arg_long(arg, p, is_null, check_null, c == 'L')) {
					return "integer";
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
					return "float";
				}
			}
			break;

		case 's':
			{
				char **p = va_arg(*va, char **);
				size_t *pl = va_arg(*va, size_t *);
				if (!zend_parse_arg_string(arg, p, pl, check_null)) {
					return "string";
				}
			}
			break;

		case 'p':
			{
				char **p = va_arg(*va, char **);
				size_t *pl = va_arg(*va, size_t *);
				if (!zend_parse_arg_path(arg, p, pl, check_null)) {
					return "a valid path";
				}
			}
			break;

		case 'P':
			{
				zend_string **str = va_arg(*va, zend_string **);
				if (!zend_parse_arg_path_str(arg, str, check_null)) {
					return "a valid path";
				}
			}
			break;

		case 'S':
			{
				zend_string **str = va_arg(*va, zend_string **);
				if (!zend_parse_arg_str(arg, str, check_null)) {
					return "string";
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
					return "boolean";
				}
			}
			break;

		case 'r':
			{
				zval **p = va_arg(*va, zval **);

				if (!zend_parse_arg_resource(arg, p, check_null)) {
					return "resource";
				}
			}
			break;

		case 'A':
		case 'a':
			{
				zval **p = va_arg(*va, zval **);

				if (!zend_parse_arg_array(arg, p, check_null, c == 'A')) {
					return "array";
				}
			}
			break;

		case 'H':
		case 'h':
			{
				HashTable **p = va_arg(*va, HashTable **);

				if (!zend_parse_arg_array_ht(arg, p, check_null, c == 'H')) {
					return "array";
				}
			}
			break;

		case 'o':
			{
				zval **p = va_arg(*va, zval **);

				if (!zend_parse_arg_object(arg, p, NULL, check_null)) {
					return "object";
				}
			}
			break;

		case 'O':
			{
				zval **p = va_arg(*va, zval **);
				zend_class_entry *ce = va_arg(*va, zend_class_entry *);

				if (!zend_parse_arg_object(arg, p, ce, check_null)) {
					if (ce) {
						return ZSTR_VAL(ce->name);
					} else {
						return "object";
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
				convert_to_string_ex(arg);
				if ((lookup = zend_lookup_class(Z_STR_P(arg))) == NULL) {
					*pce = NULL;
				} else {
					*pce = lookup;
				}
				if (ce_base) {
					if ((!*pce || !instanceof_function(*pce, ce_base))) {
						zend_spprintf(error, 0, "to be a class name derived from %s, '%s' given",
							ZSTR_VAL(ce_base->name), Z_STRVAL_P(arg));
						*pce = NULL;
						return "";
					}
				}
				if (!*pce) {
					zend_spprintf(error, 0, "to be a valid class name, '%s' given",
						Z_STRVAL_P(arg));
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
					fcc->initialized = 0;
					break;
				}

				if (zend_fcall_info_init(arg, 0, fci, fcc, NULL, &is_callable_error) == SUCCESS) {
					if (is_callable_error) {
						*severity = E_DEPRECATED;
						zend_spprintf(error, 0, "to be a valid callback, %s", is_callable_error);
						efree(is_callable_error);
						*spec = spec_walk;
						return "";
					}
					break;
				} else {
					if (is_callable_error) {
						*severity = E_ERROR;
						zend_spprintf(error, 0, "to be a valid callback, %s", is_callable_error);
						efree(is_callable_error);
						return "";
					} else {
						return "valid callback";
					}
				}
			}

		case 'z':
			{
				zval **p = va_arg(*va, zval **);

				zend_parse_arg_zval_deref(real_arg, p, check_null);
			}
			break;

		case 'Z':
			/* 'Z' iz not supported anymore and should be replaced with 'z' */
			ZEND_ASSERT(c != 'Z');
		default:
			return "unknown";
	}

	*spec = spec_walk;

	return NULL;
}
/* }}} */

static int zend_parse_arg(int arg_num, zval *arg, va_list *va, const char **spec, int flags) /* {{{ */
{
	const char *expected_type = NULL;
	char *error = NULL;
	int severity = 0;

	expected_type = zend_parse_arg_impl(arg_num, arg, va, spec, &error, &severity);
	if (expected_type) {
		if (!(flags & ZEND_PARSE_PARAMS_QUIET) && (*expected_type || error)) {
			const char *space;
			const char *class_name = get_active_class_name(&space);
			zend_bool throw_exception =
				ZEND_ARG_USES_STRICT_TYPES() || (flags & ZEND_PARSE_PARAMS_THROW);

			if (error) {
				zend_internal_type_error(throw_exception, "%s%s%s() expects parameter %d %s",
						class_name, space, get_active_function_name(), arg_num, error);
				efree(error);
			} else {
				zend_internal_type_error(throw_exception,
						"%s%s%s() expects parameter %d to be %s, %s given",
						class_name, space, get_active_function_name(), arg_num, expected_type,
						zend_zval_type_name(arg));
			}
		}
		if (severity != E_DEPRECATED) {
			return FAILURE;
		}
	}

	return SUCCESS;
}
/* }}} */

ZEND_API int zend_parse_parameter(int flags, int arg_num, zval *arg, const char *spec, ...)
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

static int zend_parse_va_args(int num_args, const char *type_spec, va_list *va, int flags) /* {{{ */
{
	const  char *spec_walk;
	int c, i;
	int min_num_args = -1;
	int max_num_args = 0;
	int post_varargs = 0;
	zval *arg;
	int arg_count;
	zend_bool have_varargs = 0;
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
			case 'L':
				max_num_args++;
				break;

			case '|':
				min_num_args = max_num_args;
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

	if (min_num_args < 0) {
		min_num_args = max_num_args;
	}

	if (have_varargs) {
		/* calculate how many required args are at the end of the specifier list */
		post_varargs = max_num_args - post_varargs;
		max_num_args = -1;
	}

	if (num_args < min_num_args || (num_args > max_num_args && max_num_args >= 0)) {
		if (!(flags & ZEND_PARSE_PARAMS_QUIET)) {
			zend_function *active_function = EG(current_execute_data)->func;
			const char *class_name = active_function->common.scope ? ZSTR_VAL(active_function->common.scope->name) : "";
			zend_bool throw_exception = ZEND_ARG_USES_STRICT_TYPES() || (flags & ZEND_PARSE_PARAMS_THROW);
			zend_internal_type_error(throw_exception, "%s%s%s() expects %s %d parameter%s, %d given",
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

	arg_count = ZEND_CALL_NUM_ARGS(EG(current_execute_data));

	if (num_args > arg_count) {
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

ZEND_API int zend_parse_parameters_ex(int flags, int num_args, const char *type_spec, ...) /* {{{ */
{
	va_list va;
	int retval;

	va_start(va, type_spec);
	retval = zend_parse_va_args(num_args, type_spec, &va, flags);
	va_end(va);

	return retval;
}
/* }}} */

ZEND_API int zend_parse_parameters(int num_args, const char *type_spec, ...) /* {{{ */
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

ZEND_API int zend_parse_parameters_throw(int num_args, const char *type_spec, ...) /* {{{ */
{
	va_list va;
	int retval;
	int flags = ZEND_PARSE_PARAMS_THROW;

	va_start(va, type_spec);
	retval = zend_parse_va_args(num_args, type_spec, &va, flags);
	va_end(va);

	return retval;
}
/* }}} */

ZEND_API int zend_parse_method_parameters(int num_args, zval *this_ptr, const char *type_spec, ...) /* {{{ */
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

ZEND_API int zend_parse_method_parameters_ex(int flags, int num_args, zval *this_ptr, const char *type_spec, ...) /* {{{ */
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

/* Argument parsing API -- andrei */
ZEND_API int _array_init(zval *arg, uint32_t size ZEND_FILE_LINE_DC) /* {{{ */
{
	ZVAL_NEW_ARR(arg);
	_zend_hash_init(Z_ARRVAL_P(arg), size, ZVAL_PTR_DTOR, 0 ZEND_FILE_LINE_RELAY_CC);
	return SUCCESS;
}
/* }}} */

/* This function should be called after the constructor has been called
 * because it may call __set from the uninitialized object otherwise. */
ZEND_API void zend_merge_properties(zval *obj, HashTable *properties) /* {{{ */
{
	const zend_object_handlers *obj_ht = Z_OBJ_HT_P(obj);
	zend_class_entry *old_scope = EG(scope);
	zend_string *key;
	zval *value;

	EG(scope) = Z_OBJCE_P(obj);
	ZEND_HASH_FOREACH_STR_KEY_VAL(properties, key, value) {
		if (key) {
			zval member;

			ZVAL_STR(&member, key);
			obj_ht->write_property(obj, &member, value, NULL);
		}
	} ZEND_HASH_FOREACH_END();
	EG(scope) = old_scope;
}
/* }}} */

ZEND_API int zend_update_class_constants(zend_class_entry *class_type) /* {{{ */
{
	if (!(class_type->ce_flags & ZEND_ACC_CONSTANTS_UPDATED)) {
		if (class_type->parent) {
			if (UNEXPECTED(zend_update_class_constants(class_type->parent) != SUCCESS)) {
				return FAILURE;
			}
		}

		if (!CE_STATIC_MEMBERS(class_type) && class_type->default_static_members_count) {
			/* initialize static members of internal class */
			int i;
			zval *p;

#if ZTS
			CG(static_members_table)[(zend_intptr_t)(class_type->static_members_table)] = emalloc(sizeof(zval) * class_type->default_static_members_count);
#else
			class_type->static_members_table = emalloc(sizeof(zval) * class_type->default_static_members_count);
#endif
			for (i = 0; i < class_type->default_static_members_count; i++) {
				p = &class_type->default_static_members_table[i];
				if (Z_ISREF_P(p) &&
					class_type->parent &&
					i < class_type->parent->default_static_members_count &&
					p == &class_type->parent->default_static_members_table[i] &&
					Z_TYPE(CE_STATIC_MEMBERS(class_type->parent)[i]) != IS_UNDEF
				) {
					zval *q = &CE_STATIC_MEMBERS(class_type->parent)[i];

					ZVAL_NEW_REF(q, q);
					ZVAL_COPY_VALUE(&CE_STATIC_MEMBERS(class_type)[i], q);
					Z_ADDREF_P(q);
				} else {
					ZVAL_DUP(&CE_STATIC_MEMBERS(class_type)[i], p);
				}
			}
		} else {
			zend_class_entry **scope = EG(current_execute_data) ? &EG(scope) : &CG(active_class_entry);
			zend_class_entry *old_scope = *scope;
			zend_class_entry *ce;
			zval *val;
			zend_property_info *prop_info;

			*scope = class_type;
			ZEND_HASH_FOREACH_VAL(&class_type->constants_table, val) {
				ZVAL_DEREF(val);
				if (Z_CONSTANT_P(val)) {
					if (UNEXPECTED(zval_update_constant_ex(val, 1, class_type) != SUCCESS)) {
						return FAILURE;
					}
				}
			} ZEND_HASH_FOREACH_END();

			ce = class_type;
			while (ce) {
				ZEND_HASH_FOREACH_PTR(&ce->properties_info, prop_info) {
					if (prop_info->ce == ce) {
						if (prop_info->flags & ZEND_ACC_STATIC) {
							val = CE_STATIC_MEMBERS(class_type) + prop_info->offset;
						} else {
							val = (zval*)((char*)class_type->default_properties_table + prop_info->offset - OBJ_PROP_TO_OFFSET(0));
						}
						ZVAL_DEREF(val);
						if (Z_CONSTANT_P(val)) {
							*scope = ce;
							if (UNEXPECTED(zval_update_constant_ex(val, 1, NULL) != SUCCESS)) {
								return FAILURE;
							}
						}
					}
				} ZEND_HASH_FOREACH_END();
				ce = ce->parent;
			}

			*scope = old_scope;
		}
		class_type->ce_flags |= ZEND_ACC_CONSTANTS_UPDATED;
	}

	return SUCCESS;
}
/* }}} */

ZEND_API void object_properties_init(zend_object *object, zend_class_entry *class_type) /* {{{ */
{
	if (class_type->default_properties_count) {
		zval *src = class_type->default_properties_table;
		zval *dst = object->properties_table;
		zval *end = src + class_type->default_properties_count;

		do {
#if ZTS
			ZVAL_DUP(dst, src);
#else
			ZVAL_COPY(dst, src);
#endif
			src++;
			dst++;
		} while (src != end);
		object->properties = NULL;
	}
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
				ZVAL_COPY_VALUE(slot, prop);
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
					zend_class_entry *prev_scope = EG(scope);
					if (class_name && class_name[0] != '*') {
						zend_string *cname = zend_string_init(class_name, strlen(class_name), 0);
						EG(scope) = zend_lookup_class(cname);
						zend_string_release(cname);
					}
					property_info = zend_get_property_info(object->ce, pname, 1);
					zend_string_release(pname);
					EG(scope) = prev_scope;
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
ZEND_API int _object_and_properties_init(zval *arg, zend_class_entry *class_type, HashTable *properties ZEND_FILE_LINE_DC) /* {{{ */
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
		ZVAL_OBJ(arg, zend_objects_new(class_type));
		if (properties) {
			object_properties_init_ex(Z_OBJ_P(arg), properties);
		} else {
			object_properties_init(Z_OBJ_P(arg), class_type);
		}
	} else {
		ZVAL_OBJ(arg, class_type->create_object(class_type));
	}
	return SUCCESS;
}
/* }}} */

ZEND_API int _object_init_ex(zval *arg, zend_class_entry *class_type ZEND_FILE_LINE_DC) /* {{{ */
{
	return _object_and_properties_init(arg, class_type, 0 ZEND_FILE_LINE_RELAY_CC);
}
/* }}} */

ZEND_API int _object_init(zval *arg ZEND_FILE_LINE_DC) /* {{{ */
{
	return _object_init_ex(arg, zend_standard_class_def ZEND_FILE_LINE_RELAY_CC);
}
/* }}} */

ZEND_API int add_assoc_long_ex(zval *arg, const char *key, size_t key_len, zend_long n) /* {{{ */
{
	zval *ret, tmp;

	ZVAL_LONG(&tmp, n);
	ret = zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, &tmp);
	return ret ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API int add_assoc_null_ex(zval *arg, const char *key, size_t key_len) /* {{{ */
{
	zval *ret, tmp;

	ZVAL_NULL(&tmp);
	ret = zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, &tmp);
	return ret ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API int add_assoc_bool_ex(zval *arg, const char *key, size_t key_len, int b) /* {{{ */
{
	zval *ret, tmp;

	ZVAL_BOOL(&tmp, b);
	ret = zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, &tmp);
	return ret ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API int add_assoc_resource_ex(zval *arg, const char *key, size_t key_len, zend_resource *r) /* {{{ */
{
	zval *ret, tmp;

	ZVAL_RES(&tmp, r);
	ret = zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, &tmp);
	return ret ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API int add_assoc_double_ex(zval *arg, const char *key, size_t key_len, double d) /* {{{ */
{
	zval *ret, tmp;

	ZVAL_DOUBLE(&tmp, d);
	ret = zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, &tmp);
	return ret ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API int add_assoc_str_ex(zval *arg, const char *key, size_t key_len, zend_string *str) /* {{{ */
{
	zval *ret, tmp;

	ZVAL_STR(&tmp, str);
	ret = zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, &tmp);
	return ret ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API int add_assoc_string_ex(zval *arg, const char *key, size_t key_len, char *str) /* {{{ */
{
	zval *ret, tmp;

	ZVAL_STRING(&tmp, str);
	ret = zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, &tmp);
	return ret ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API int add_assoc_stringl_ex(zval *arg, const char *key, size_t key_len, char *str, size_t length) /* {{{ */
{
	zval *ret, tmp;

	ZVAL_STRINGL(&tmp, str, length);
	ret = zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, &tmp);
	return ret ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API int add_assoc_zval_ex(zval *arg, const char *key, size_t key_len, zval *value) /* {{{ */
{
	zval *ret;

	ret = zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, value);
	return ret ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API int add_index_long(zval *arg, zend_ulong index, zend_long n) /* {{{ */
{
	zval tmp;

	ZVAL_LONG(&tmp, n);
	return zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp) ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API int add_index_null(zval *arg, zend_ulong index) /* {{{ */
{
	zval tmp;

	ZVAL_NULL(&tmp);
	return zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp) ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API int add_index_bool(zval *arg, zend_ulong index, int b) /* {{{ */
{
	zval tmp;

	ZVAL_BOOL(&tmp, b);
	return zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp) ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API int add_index_resource(zval *arg, zend_ulong index, zend_resource *r) /* {{{ */
{
	zval tmp;

	ZVAL_RES(&tmp, r);
	return zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp) ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API int add_index_double(zval *arg, zend_ulong index, double d) /* {{{ */
{
	zval tmp;

	ZVAL_DOUBLE(&tmp, d);
	return zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp) ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API int add_index_str(zval *arg, zend_ulong index, zend_string *str) /* {{{ */
{
	zval tmp;

	ZVAL_STR(&tmp, str);
	return zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp) ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API int add_index_string(zval *arg, zend_ulong index, const char *str) /* {{{ */
{
	zval tmp;

	ZVAL_STRING(&tmp, str);
	return zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp) ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API int add_index_stringl(zval *arg, zend_ulong index, const char *str, size_t length) /* {{{ */
{
	zval tmp;

	ZVAL_STRINGL(&tmp, str, length);
	return zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp) ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API int add_index_zval(zval *arg, zend_ulong index, zval *value) /* {{{ */
{
	return zend_hash_index_update(Z_ARRVAL_P(arg), index, value) ? SUCCESS : FAILURE;
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

ZEND_API int add_next_index_zval(zval *arg, zval *value) /* {{{ */
{
	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), value) ? SUCCESS : FAILURE;
}
/* }}} */

ZEND_API zval *add_get_assoc_string_ex(zval *arg, const char *key, uint key_len, const char *str) /* {{{ */
{
	zval tmp, *ret;

	ZVAL_STRING(&tmp, str);
	ret = zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, &tmp);
	return ret;
}
/* }}} */

ZEND_API zval *add_get_assoc_stringl_ex(zval *arg, const char *key, uint key_len, const char *str, size_t length) /* {{{ */
{
	zval tmp, *ret;

	ZVAL_STRINGL(&tmp, str, length);
	ret = zend_symtable_str_update(Z_ARRVAL_P(arg), key, key_len, &tmp);
	return ret;
}
/* }}} */

ZEND_API zval *add_get_index_long(zval *arg, zend_ulong index, zend_long l) /* {{{ */
{
	zval tmp;

	ZVAL_LONG(&tmp, l);
	return zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp);
}
/* }}} */

ZEND_API zval *add_get_index_double(zval *arg, zend_ulong index, double d) /* {{{ */
{
	zval tmp;

	ZVAL_DOUBLE(&tmp, d);
	return zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp);
}
/* }}} */

ZEND_API zval *add_get_index_str(zval *arg, zend_ulong index, zend_string *str) /* {{{ */
{
	zval tmp;

	ZVAL_STR(&tmp, str);
	return zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp);
}
/* }}} */

ZEND_API zval *add_get_index_string(zval *arg, zend_ulong index, const char *str) /* {{{ */
{
	zval tmp;

	ZVAL_STRING(&tmp, str);
	return zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp);
}
/* }}} */

ZEND_API zval *add_get_index_stringl(zval *arg, zend_ulong index, const char *str, size_t length) /* {{{ */
{
	zval tmp;

	ZVAL_STRINGL(&tmp, str, length);
	return zend_hash_index_update(Z_ARRVAL_P(arg), index, &tmp);
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
			result = zend_symtable_update(ht, ZSTR_EMPTY_ALLOC(), value);
			break;
		case IS_RESOURCE:
			zend_error(E_NOTICE, "Resource ID#%d used as offset, casting to integer (%d)", Z_RES_HANDLE_P(key), Z_RES_HANDLE_P(key));
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
			zend_error(E_WARNING, "Illegal offset type");
			result = NULL;
	}

	if (result) {
		if (Z_REFCOUNTED_P(result)) {
			Z_ADDREF_P(result);
		}
		return SUCCESS;
	} else {
		return FAILURE;
	}
}
/* }}} */

ZEND_API int add_property_long_ex(zval *arg, const char *key, size_t key_len, zend_long n) /* {{{ */
{
	zval tmp;
	zval z_key;

	ZVAL_LONG(&tmp, n);
	ZVAL_STRINGL(&z_key, key, key_len);
	Z_OBJ_HANDLER_P(arg, write_property)(arg, &z_key, &tmp, NULL);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_bool_ex(zval *arg, const char *key, size_t key_len, zend_long b) /* {{{ */
{
	zval tmp;
	zval z_key;

	ZVAL_BOOL(&tmp, b);
	ZVAL_STRINGL(&z_key, key, key_len);
	Z_OBJ_HANDLER_P(arg, write_property)(arg, &z_key, &tmp, NULL);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_null_ex(zval *arg, const char *key, size_t key_len) /* {{{ */
{
	zval tmp;
	zval z_key;

	ZVAL_NULL(&tmp);
	ZVAL_STRINGL(&z_key, key, key_len);
	Z_OBJ_HANDLER_P(arg, write_property)(arg, &z_key, &tmp, NULL);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_resource_ex(zval *arg, const char *key, size_t key_len, zend_resource *r) /* {{{ */
{
	zval tmp;
	zval z_key;

	ZVAL_RES(&tmp, r);
	ZVAL_STRINGL(&z_key, key, key_len);
	Z_OBJ_HANDLER_P(arg, write_property)(arg, &z_key, &tmp, NULL);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_double_ex(zval *arg, const char *key, size_t key_len, double d) /* {{{ */
{
	zval tmp;
	zval z_key;

	ZVAL_DOUBLE(&tmp, d);
	ZVAL_STRINGL(&z_key, key, key_len);
	Z_OBJ_HANDLER_P(arg, write_property)(arg, &z_key, &tmp, NULL);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_str_ex(zval *arg, const char *key, size_t key_len, zend_string *str) /* {{{ */
{
	zval tmp;
	zval z_key;

	ZVAL_STR(&tmp, str);
	ZVAL_STRINGL(&z_key, key, key_len);
	Z_OBJ_HANDLER_P(arg, write_property)(arg, &z_key, &tmp, NULL);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_string_ex(zval *arg, const char *key, size_t key_len, const char *str) /* {{{ */
{
	zval tmp;
	zval z_key;

	ZVAL_STRING(&tmp, str);
	ZVAL_STRINGL(&z_key, key, key_len);
	Z_OBJ_HANDLER_P(arg, write_property)(arg, &z_key, &tmp, NULL);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_stringl_ex(zval *arg, const char *key, size_t key_len, const char *str, size_t length) /* {{{ */
{
	zval tmp;
	zval z_key;

	ZVAL_STRINGL(&tmp, str, length);
	ZVAL_STRINGL(&z_key, key, key_len);
	Z_OBJ_HANDLER_P(arg, write_property)(arg, &z_key, &tmp, NULL);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_zval_ex(zval *arg, const char *key, size_t key_len, zval *value) /* {{{ */
{
	zval z_key;

	ZVAL_STRINGL(&z_key, key, key_len);
	Z_OBJ_HANDLER_P(arg, write_property)(arg, &z_key, value, NULL);
	zval_ptr_dtor(&z_key);
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
					zend_string_free(lcname);
					/* TODO: Check version relationship */
					zend_error(E_CORE_WARNING, "Cannot load module '%s' because required module '%s' is not loaded", module->name, dep->name);
					module->module_started = 0;
					return FAILURE;
				}
				zend_string_free(lcname);
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
					zend_string_free(lcname);
					/* TODO: Check version relationship */
					zend_error(E_CORE_WARNING, "Cannot load module '%s' because conflicting module '%s' is already loaded", module->name, dep->name);
					return NULL;
				}
				zend_string_free(lcname);
			}
			++dep;
		}
	}

	name_len = strlen(module->name);
	lcname = zend_string_alloc(name_len, 1);
	zend_str_tolower_copy(ZSTR_VAL(lcname), module->name, name_len);

	if ((module_ptr = zend_hash_add_mem(&module_registry, lcname, module, sizeof(zend_module_entry))) == NULL) {
		zend_error(E_CORE_WARNING, "Module '%s' already loaded", module->name);
		zend_string_release(lcname);
		return NULL;
	}
	zend_string_release(lcname);
	module = module_ptr;
	EG(current_module) = module;

	if (module->functions && zend_register_functions(NULL, module->functions, NULL, module->type)==FAILURE) {
		EG(current_module) = NULL;
		zend_error(E_CORE_WARNING,"%s: Unable to register functions, unable to load", module->name);
		return NULL;
	}

	EG(current_module) = NULL;
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

ZEND_API void zend_check_magic_method_implementation(const zend_class_entry *ce, const zend_function *fptr, int error_type) /* {{{ */
{
	char lcname[16];
	size_t name_len;

	/* we don't care if the function name is longer, in fact lowercasing only
	 * the beginning of the name speeds up the check process */
	name_len = ZSTR_LEN(fptr->common.function_name);
	zend_str_tolower_copy(lcname, ZSTR_VAL(fptr->common.function_name), MIN(name_len, sizeof(lcname)-1));
	lcname[sizeof(lcname)-1] = '\0'; /* zend_str_tolower_copy won't necessarily set the zero byte */

	if (name_len == sizeof(ZEND_DESTRUCTOR_FUNC_NAME) - 1 && !memcmp(lcname, ZEND_DESTRUCTOR_FUNC_NAME, sizeof(ZEND_DESTRUCTOR_FUNC_NAME) - 1) && fptr->common.num_args != 0) {
		zend_error(error_type, "Destructor %s::%s() cannot take arguments", ZSTR_VAL(ce->name), ZEND_DESTRUCTOR_FUNC_NAME);
	} else if (name_len == sizeof(ZEND_CLONE_FUNC_NAME) - 1 && !memcmp(lcname, ZEND_CLONE_FUNC_NAME, sizeof(ZEND_CLONE_FUNC_NAME) - 1) && fptr->common.num_args != 0) {
		zend_error(error_type, "Method %s::%s() cannot accept any arguments", ZSTR_VAL(ce->name), ZEND_CLONE_FUNC_NAME);
	} else if (name_len == sizeof(ZEND_GET_FUNC_NAME) - 1 && !memcmp(lcname, ZEND_GET_FUNC_NAME, sizeof(ZEND_GET_FUNC_NAME) - 1)) {
		if (fptr->common.num_args != 1) {
			zend_error(error_type, "Method %s::%s() must take exactly 1 argument", ZSTR_VAL(ce->name), ZEND_GET_FUNC_NAME);
		} else if (ARG_SHOULD_BE_SENT_BY_REF(fptr, 1)) {
			zend_error(error_type, "Method %s::%s() cannot take arguments by reference", ZSTR_VAL(ce->name), ZEND_GET_FUNC_NAME);
		}
	} else if (name_len == sizeof(ZEND_SET_FUNC_NAME) - 1 && !memcmp(lcname, ZEND_SET_FUNC_NAME, sizeof(ZEND_SET_FUNC_NAME) - 1)) {
		if (fptr->common.num_args != 2) {
			zend_error(error_type, "Method %s::%s() must take exactly 2 arguments", ZSTR_VAL(ce->name), ZEND_SET_FUNC_NAME);
		} else if (ARG_SHOULD_BE_SENT_BY_REF(fptr, 1) || ARG_SHOULD_BE_SENT_BY_REF(fptr, 2)) {
			zend_error(error_type, "Method %s::%s() cannot take arguments by reference", ZSTR_VAL(ce->name), ZEND_SET_FUNC_NAME);
		}
	} else if (name_len == sizeof(ZEND_UNSET_FUNC_NAME) - 1 && !memcmp(lcname, ZEND_UNSET_FUNC_NAME, sizeof(ZEND_UNSET_FUNC_NAME) - 1)) {
		if (fptr->common.num_args != 1) {
			zend_error(error_type, "Method %s::%s() must take exactly 1 argument", ZSTR_VAL(ce->name), ZEND_UNSET_FUNC_NAME);
		} else if (ARG_SHOULD_BE_SENT_BY_REF(fptr, 1)) {
			zend_error(error_type, "Method %s::%s() cannot take arguments by reference", ZSTR_VAL(ce->name), ZEND_UNSET_FUNC_NAME);
		}
	} else if (name_len == sizeof(ZEND_ISSET_FUNC_NAME) - 1 && !memcmp(lcname, ZEND_ISSET_FUNC_NAME, sizeof(ZEND_ISSET_FUNC_NAME) - 1)) {
		if (fptr->common.num_args != 1) {
			zend_error(error_type, "Method %s::%s() must take exactly 1 argument", ZSTR_VAL(ce->name), ZEND_ISSET_FUNC_NAME);
		} else if (ARG_SHOULD_BE_SENT_BY_REF(fptr, 1)) {
			zend_error(error_type, "Method %s::%s() cannot take arguments by reference", ZSTR_VAL(ce->name), ZEND_ISSET_FUNC_NAME);
		}
	} else if (name_len == sizeof(ZEND_CALL_FUNC_NAME) - 1 && !memcmp(lcname, ZEND_CALL_FUNC_NAME, sizeof(ZEND_CALL_FUNC_NAME) - 1)) {
		if (fptr->common.num_args != 2) {
			zend_error(error_type, "Method %s::%s() must take exactly 2 arguments", ZSTR_VAL(ce->name), ZEND_CALL_FUNC_NAME);
		} else if (ARG_SHOULD_BE_SENT_BY_REF(fptr, 1) || ARG_SHOULD_BE_SENT_BY_REF(fptr, 2)) {
			zend_error(error_type, "Method %s::%s() cannot take arguments by reference", ZSTR_VAL(ce->name), ZEND_CALL_FUNC_NAME);
		}
	} else if (name_len == sizeof(ZEND_CALLSTATIC_FUNC_NAME) - 1 &&
		!memcmp(lcname, ZEND_CALLSTATIC_FUNC_NAME, sizeof(ZEND_CALLSTATIC_FUNC_NAME)-1)
	) {
		if (fptr->common.num_args != 2) {
			zend_error(error_type, "Method %s::__callStatic() must take exactly 2 arguments", ZSTR_VAL(ce->name));
		} else if (ARG_SHOULD_BE_SENT_BY_REF(fptr, 1) || ARG_SHOULD_BE_SENT_BY_REF(fptr, 2)) {
			zend_error(error_type, "Method %s::__callStatic() cannot take arguments by reference", ZSTR_VAL(ce->name));
		}
 	} else if (name_len == sizeof(ZEND_TOSTRING_FUNC_NAME) - 1 &&
 		!memcmp(lcname, ZEND_TOSTRING_FUNC_NAME, sizeof(ZEND_TOSTRING_FUNC_NAME)-1) && fptr->common.num_args != 0
	) {
		zend_error(error_type, "Method %s::%s() cannot take arguments", ZSTR_VAL(ce->name), ZEND_TOSTRING_FUNC_NAME);
	} else if (name_len == sizeof(ZEND_DEBUGINFO_FUNC_NAME) - 1 &&
		!memcmp(lcname, ZEND_DEBUGINFO_FUNC_NAME, sizeof(ZEND_DEBUGINFO_FUNC_NAME)-1) && fptr->common.num_args != 0) {
		zend_error(error_type, "Method %s::%s() cannot take arguments", ZSTR_VAL(ce->name), ZEND_DEBUGINFO_FUNC_NAME);
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
	zend_function *ctor = NULL, *dtor = NULL, *clone = NULL, *__get = NULL, *__set = NULL, *__unset = NULL, *__isset = NULL, *__call = NULL, *__callstatic = NULL, *__tostring = NULL, *__debugInfo = NULL;
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
		internal_function->function_name = zend_new_interned_string(zend_string_init(ptr->fname, fname_len, 1));
		internal_function->scope = scope;
		internal_function->prototype = NULL;
		if (ptr->flags) {
			if (!(ptr->flags & ZEND_ACC_PPP_MASK)) {
				if (ptr->flags != ZEND_ACC_DEPRECATED || scope) {
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
			if (info->return_reference) {
				internal_function->fn_flags |= ZEND_ACC_RETURN_REFERENCE;
			}
			if (ptr->arg_info[ptr->num_args].is_variadic) {
				internal_function->fn_flags |= ZEND_ACC_VARIADIC;
				/* Don't count the variadic argument */
				internal_function->num_args--;
			}
			if (info->type_hint) {
				if (info->class_name) {
					ZEND_ASSERT(info->type_hint == IS_OBJECT);
					if (!scope && (!strcasecmp(info->class_name, "self") || !strcasecmp(info->class_name, "parent"))) {
						zend_error_noreturn(E_CORE_ERROR, "Cannot declare a return type of %s outside of a class scope", info->class_name);
					}
				}

				internal_function->fn_flags |= ZEND_ACC_HAS_RETURN_TYPE;
			}
		} else {
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
			if (ptr->flags & ZEND_ACC_STATIC && (!scope || !(scope->ce_flags & ZEND_ACC_INTERFACE))) {
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
		lowercase_name = zend_string_alloc(fname_len, 1);
		zend_str_tolower_copy(ZSTR_VAL(lowercase_name), ptr->fname, fname_len);
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
				if (reg_function->common.arg_info[i].class_name ||
				    reg_function->common.arg_info[i].type_hint) {
				    reg_function->common.fn_flags |= ZEND_ACC_HAS_TYPE_HINTS;
					break;
				}
			}
		}

		if (scope) {
			/* Look for ctor, dtor, clone
			 * If it's an old-style constructor, store it only if we don't have
			 * a constructor already.
			 */
			if ((fname_len == class_name_len) && !ctor && !memcmp(ZSTR_VAL(lowercase_name), lc_class_name, class_name_len+1)) {
				ctor = reg_function;
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
				zend_check_magic_method_implementation(scope, reg_function, error_type);
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
			zend_string_free(lowercase_name);
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
		if (ctor) {
			ctor->common.fn_flags |= ZEND_ACC_CTOR;
			if (ctor->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Constructor %s::%s() cannot be static", ZSTR_VAL(scope->name), ZSTR_VAL(ctor->common.function_name));
			}
			ctor->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (dtor) {
			dtor->common.fn_flags |= ZEND_ACC_DTOR;
			if (dtor->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Destructor %s::%s() cannot be static", ZSTR_VAL(scope->name), ZSTR_VAL(dtor->common.function_name));
			}
			dtor->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (clone) {
			clone->common.fn_flags |= ZEND_ACC_CLONE;
			if (clone->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Constructor %s::%s() cannot be static", ZSTR_VAL(scope->name), ZSTR_VAL(clone->common.function_name));
			}
			clone->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (__call) {
			if (__call->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %s::%s() cannot be static", ZSTR_VAL(scope->name), ZSTR_VAL(__call->common.function_name));
			}
			__call->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
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
			__tostring->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (__get) {
			if (__get->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %s::%s() cannot be static", ZSTR_VAL(scope->name), ZSTR_VAL(__get->common.function_name));
			}
			__get->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (__set) {
			if (__set->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %s::%s() cannot be static", ZSTR_VAL(scope->name), ZSTR_VAL(__set->common.function_name));
			}
			__set->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (__unset) {
			if (__unset->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %s::%s() cannot be static", ZSTR_VAL(scope->name), ZSTR_VAL(__unset->common.function_name));
			}
			__unset->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (__isset) {
			if (__isset->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %s::%s() cannot be static", ZSTR_VAL(scope->name), ZSTR_VAL(__isset->common.function_name));
			}
			__isset->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (__debugInfo) {
			if (__debugInfo->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %s::%s() cannot be static", ZSTR_VAL(scope->name), ZSTR_VAL(__debugInfo->common.function_name));
			}
		}

		if (ctor && ctor->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE && ctor->common.fn_flags & ZEND_ACC_CTOR) {
			zend_error_noreturn(E_CORE_ERROR, "Constructor %s::%s() cannot declare a return type", ZSTR_VAL(scope->name), ZSTR_VAL(ctor->common.function_name));
		}

		if (dtor && dtor->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE && dtor->common.fn_flags & ZEND_ACC_DTOR) {
			zend_error_noreturn(E_CORE_ERROR, "Destructor %s::%s() cannot declare a return type", ZSTR_VAL(scope->name), ZSTR_VAL(dtor->common.function_name));
		}

		if (clone && clone->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE && dtor->common.fn_flags & ZEND_ACC_DTOR) {
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
		zend_string_free(lowercase_name);
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
	if (module->functions) {
		zend_unregister_functions(module->functions, -1, NULL);
	}

#if HAVE_LIBDL
#if !(defined(NETWARE) && defined(APACHE_1_BUILD))
	if (module->handle && !getenv("ZEND_DONT_UNLOAD_MODULES")) {
		DL_UNLOAD(module->handle);
	}
#endif
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

/* call request shutdown for all modules */
static int module_registry_cleanup(zval *zv) /* {{{ */
{
	zend_module_entry *module = Z_PTR_P(zv);

	if (module->request_shutdown_func) {
#if 0
		zend_printf("%s: Request shutdown\n", module->name);
#endif
		module->request_shutdown_func(module->type, module->module_number);
	}
	return 0;
}
/* }}} */

ZEND_API void zend_deactivate_modules(void) /* {{{ */
{
	EG(current_execute_data) = NULL; /* we're no longer executing anything */

	zend_try {
		if (EG(full_tables_cleanup)) {
			zend_hash_reverse_apply(&module_registry, module_registry_cleanup);
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

int module_registry_unload_temp(const zend_module_entry *module) /* {{{ */
{
	return (module->type == MODULE_TEMPORARY) ? ZEND_HASH_APPLY_REMOVE : ZEND_HASH_APPLY_STOP;
}
/* }}} */

static int module_registry_unload_temp_wrapper(zval *el) /* {{{ */
{
	zend_module_entry *module = (zend_module_entry *)Z_PTR_P(el);
	return module_registry_unload_temp((const zend_module_entry *)module);
}
/* }}} */

static int exec_done_cb(zval *el) /* {{{ */
{
	zend_module_entry *module = (zend_module_entry *)Z_PTR_P(el);
	if (module->post_deactivate_func) {
		module->post_deactivate_func();
	}
	return 0;
}
/* }}} */

ZEND_API void zend_post_deactivate_modules(void) /* {{{ */
{
	if (EG(full_tables_cleanup)) {
		zend_hash_apply(&module_registry, exec_done_cb);
		zend_hash_reverse_apply(&module_registry, module_registry_unload_temp_wrapper);
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
	zend_string *lowercase_name = zend_string_alloc(ZSTR_LEN(orig_class_entry->name), 1);
	*class_entry = *orig_class_entry;

	class_entry->type = ZEND_INTERNAL_CLASS;
	zend_initialize_class_data(class_entry, 0);
	class_entry->ce_flags = ce_flags | ZEND_ACC_CONSTANTS_UPDATED;
	class_entry->info.internal.module = EG(current_module);

	if (class_entry->info.internal.builtin_functions) {
		zend_register_functions(class_entry, class_entry->info.internal.builtin_functions, &class_entry->function_table, MODULE_PERSISTENT);
	}

	zend_str_tolower_copy(ZSTR_VAL(lowercase_name), ZSTR_VAL(orig_class_entry->name), ZSTR_LEN(class_entry->name));
	lowercase_name = zend_new_interned_string(lowercase_name);
	zend_hash_update_ptr(CG(class_table), lowercase_name, class_entry);
	zend_string_release(lowercase_name);
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

ZEND_API int zend_register_class_alias_ex(const char *name, size_t name_len, zend_class_entry *ce) /* {{{ */
{
	zend_string *lcname;

	if (name[0] == '\\') {
		lcname = zend_string_alloc(name_len-1, 1);
		zend_str_tolower_copy(ZSTR_VAL(lcname), name+1, name_len-1);
	} else {
		lcname = zend_string_alloc(name_len, 1);
		zend_str_tolower_copy(ZSTR_VAL(lcname), name, name_len);
	}

	zend_assert_valid_class_name(lcname);

	ce = zend_hash_add_ptr(CG(class_table), lcname, ce);
	zend_string_release(lcname);
	if (ce) {
		ce->refcount++;
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
		if (Z_REFCOUNTED_P(symbol)) {
			Z_ADDREF_P(symbol);
		}
	}
	va_end(symbol_table_list);
	return SUCCESS;
}
/* }}} */

/* Disabled functions support */

/* {{{ proto void display_disabled_function(void)
Dummy function which displays an error when a disabled function is called. */
ZEND_API ZEND_FUNCTION(display_disabled_function)
{
	zend_error(E_WARNING, "%s() has been disabled for security reasons", get_active_function_name());
}
/* }}} */

ZEND_API int zend_disable_function(char *function_name, size_t function_name_length) /* {{{ */
{
	zend_internal_function *func;
	if ((func = zend_hash_str_find_ptr(CG(function_table), function_name, function_name_length))) {
	    func->fn_flags &= ~(ZEND_ACC_VARIADIC | ZEND_ACC_HAS_TYPE_HINTS);
		func->num_args = 0;
		func->arg_info = NULL;
		func->handler = ZEND_FN(display_disabled_function);
		return SUCCESS;
	}
	return FAILURE;
}
/* }}} */

#ifdef ZEND_WIN32
#pragma optimize("", off)
#endif
static zend_object *display_disabled_class(zend_class_entry *class_type) /* {{{ */
{
	zend_object *intern;

	intern = zend_objects_new(class_type);
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

ZEND_API int zend_disable_class(char *class_name, size_t class_name_length) /* {{{ */
{
	zend_class_entry *disabled_class;
	zend_string *key;

	key = zend_string_alloc(class_name_length, 0);
	zend_str_tolower_copy(ZSTR_VAL(key), class_name, class_name_length);
	disabled_class = zend_hash_find_ptr(CG(class_table), key);
	if (!disabled_class) {
		return FAILURE;
	}
	INIT_CLASS_ENTRY_INIT_METHODS((*disabled_class), disabled_class_new, NULL, NULL, NULL, NULL, NULL);
	disabled_class->create_object = display_disabled_class;
	zend_hash_clean(&disabled_class->function_table);
	return SUCCESS;
}
/* }}} */

static int zend_is_callable_check_class(zend_string *name, zend_fcall_info_cache *fcc, int *strict_class, char **error) /* {{{ */
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
		if (!EG(scope)) {
			if (error) *error = estrdup("cannot access self:: when no class scope is active");
		} else {
			fcc->called_scope = zend_get_called_scope(EG(current_execute_data));
			fcc->calling_scope = EG(scope);
			if (!fcc->object) {
				fcc->object = zend_get_this_object(EG(current_execute_data));
			}
			ret = 1;
		}
	} else if (zend_string_equals_literal(lcname, "parent")) {
		if (!EG(scope)) {
			if (error) *error = estrdup("cannot access parent:: when no class scope is active");
		} else if (!EG(scope)->parent) {
			if (error) *error = estrdup("cannot access parent:: when current class scope has no parent");
		} else {
			fcc->called_scope = zend_get_called_scope(EG(current_execute_data));
			fcc->calling_scope = EG(scope)->parent;
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
	} else if ((ce = zend_lookup_class_ex(name, NULL, 1)) != NULL) {
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
			    instanceof_function(scope, fcc->calling_scope)) {
				fcc->object = object;
				fcc->called_scope = object->ce;
			} else {
				fcc->called_scope = fcc->calling_scope;
			}
		} else {
			fcc->called_scope = fcc->object ? fcc->object->ce : fcc->calling_scope;
		}
		*strict_class = 1;
		ret = 1;
	} else {
		if (error) zend_spprintf(error, 0, "class '%.*s' not found", name_len, ZSTR_VAL(name));
	}
	ZSTR_ALLOCA_FREE(lcname, use_heap);
	return ret;
}
/* }}} */

static int zend_is_callable_check_func(int check_flags, zval *callable, zend_fcall_info_cache *fcc, int strict_class, char **error) /* {{{ */
{
	zend_class_entry *ce_org = fcc->calling_scope;
	int retval = 0;
	zend_string *mname, *cname;
	zend_string *lmname;
	const char *colon;
	size_t clen, mlen;
	zend_class_entry *last_scope;
	HashTable *ftable;
	int call_via_handler = 0;
	ALLOCA_FLAG(use_heap)

	if (error) {
		*error = NULL;
	}

	fcc->calling_scope = NULL;
	fcc->function_handler = NULL;

	if (!ce_org) {
		zend_string *lmname;

		/* Skip leading \ */
		if (UNEXPECTED(Z_STRVAL_P(callable)[0] == '\\')) {
			ZSTR_ALLOCA_INIT(lmname, Z_STRVAL_P(callable) + 1, Z_STRLEN_P(callable) - 1, use_heap);
		} else {
			lmname = Z_STR_P(callable);
		}
		/* Check if function with given name exists.
		 * This may be a compound name that includes namespace name */
		if (EXPECTED((fcc->function_handler = zend_hash_find_ptr(EG(function_table), lmname)) != NULL)) {
			if (lmname != Z_STR_P(callable)) {
				ZSTR_ALLOCA_FREE(lmname, use_heap);
			}
			fcc->initialized = 1;
			return 1;
		} else {
			if (lmname == Z_STR_P(callable)) {
				ZSTR_ALLOCA_INIT(lmname, Z_STRVAL_P(callable), Z_STRLEN_P(callable), use_heap);
			} else {
				zend_string_forget_hash_val(lmname);
			}
			zend_str_tolower(ZSTR_VAL(lmname), ZSTR_LEN(lmname));
			if ((fcc->function_handler = zend_hash_find_ptr(EG(function_table), lmname)) != NULL) {
				ZSTR_ALLOCA_FREE(lmname, use_heap);
				fcc->initialized = 1;
				return 1;
			}
		}
		if (lmname != Z_STR_P(callable)) {
			ZSTR_ALLOCA_FREE(lmname, use_heap);
		}
	}

	/* Split name into class/namespace and method/function names */
	if ((colon = zend_memrchr(Z_STRVAL_P(callable), ':', Z_STRLEN_P(callable))) != NULL &&
		colon > Z_STRVAL_P(callable) &&
		*(colon-1) == ':'
	) {
		colon--;
		clen = colon - Z_STRVAL_P(callable);
		mlen = Z_STRLEN_P(callable) - clen - 2;

		if (colon == Z_STRVAL_P(callable)) {
			if (error) zend_spprintf(error, 0, "invalid function name");
			return 0;
		}

		/* This is a compound name.
		 * Try to fetch class and then find static method. */
		last_scope = EG(scope);
		if (ce_org) {
			EG(scope) = ce_org;
		}

		cname = zend_string_init(Z_STRVAL_P(callable), clen, 0);
		if (!zend_is_callable_check_class(cname, fcc, &strict_class, error)) {
			zend_string_release(cname);
			EG(scope) = last_scope;
			return 0;
		}
		zend_string_release(cname);
		EG(scope) = last_scope;

		ftable = &fcc->calling_scope->function_table;
		if (ce_org && !instanceof_function(ce_org, fcc->calling_scope)) {
			if (error) zend_spprintf(error, 0, "class '%s' is not a subclass of '%s'", ZSTR_VAL(ce_org->name), ZSTR_VAL(fcc->calling_scope->name));
			return 0;
		}
		mname = zend_string_init(Z_STRVAL_P(callable) + clen + 2, mlen, 0);
	} else if (ce_org) {
		/* Try to fetch find static method of given class. */
		mlen = Z_STRLEN_P(callable);
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
	} else if ((fcc->function_handler = zend_hash_find_ptr(ftable, lmname)) != NULL) {
		retval = 1;
		if ((fcc->function_handler->op_array.fn_flags & ZEND_ACC_CHANGED) &&
		    !strict_class && EG(scope) &&
		    instanceof_function(fcc->function_handler->common.scope, EG(scope))) {
			zend_function *priv_fbc;

			if ((priv_fbc = zend_hash_find_ptr(&EG(scope)->function_table, lmname)) != NULL
				&& priv_fbc->common.fn_flags & ZEND_ACC_PRIVATE
				&& priv_fbc->common.scope == EG(scope)) {
				fcc->function_handler = priv_fbc;
			}
		}
		if ((check_flags & IS_CALLABLE_CHECK_NO_ACCESS) == 0 &&
		    (fcc->calling_scope &&
		     ((fcc->object && fcc->calling_scope->__call) ||
		      (!fcc->object && fcc->calling_scope->__callstatic)))) {
			if (fcc->function_handler->op_array.fn_flags & ZEND_ACC_PRIVATE) {
				if (!zend_check_private(fcc->function_handler, fcc->object ? fcc->object->ce : EG(scope), lmname)) {
					retval = 0;
					fcc->function_handler = NULL;
					goto get_function_via_handler;
				}
			} else if (fcc->function_handler->common.fn_flags & ZEND_ACC_PROTECTED) {
				if (!zend_check_protected(fcc->function_handler->common.scope, EG(scope))) {
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
			} else if (fcc->object->handlers->get_method) {
				fcc->function_handler = fcc->object->handlers->get_method(&fcc->object, mname, NULL);
				if (fcc->function_handler) {
					if (strict_class &&
					    (!fcc->function_handler->common.scope ||
					     !instanceof_function(ce_org, fcc->function_handler->common.scope))) {
						if (fcc->function_handler->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) {
							if (fcc->function_handler->type != ZEND_OVERLOADED_FUNCTION) {
								zend_string_release(fcc->function_handler->common.function_name);
							}
							zend_free_trampoline(fcc->function_handler);
						}
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
				if (error) {
					zend_spprintf(error, 0, "cannot call abstract method %s::%s()", ZSTR_VAL(fcc->calling_scope->name), ZSTR_VAL(fcc->function_handler->common.function_name));
					retval = 0;
				} else {
					zend_throw_error(NULL, "Cannot call abstract method %s::%s()", ZSTR_VAL(fcc->calling_scope->name), ZSTR_VAL(fcc->function_handler->common.function_name));
					retval = 0;
				}
			} else if (!fcc->object && !(fcc->function_handler->common.fn_flags & ZEND_ACC_STATIC)) {
				int severity;
				char *verb;
				if (fcc->function_handler->common.fn_flags & ZEND_ACC_ALLOW_STATIC) {
					severity = E_DEPRECATED;
					verb = "should not";
				} else {
					/* An internal function assumes $this is present and won't check that. So PHP would crash by allowing the call. */
					severity = E_ERROR;
					verb = "cannot";
				}
				if ((check_flags & IS_CALLABLE_CHECK_IS_STATIC) != 0) {
					retval = 0;
				}
				if (error) {
					zend_spprintf(error, 0, "non-static method %s::%s() %s be called statically", ZSTR_VAL(fcc->calling_scope->name), ZSTR_VAL(fcc->function_handler->common.function_name), verb);
					if (severity != E_DEPRECATED) {
						retval = 0;
					}
				} else if (retval) {
					if (severity == E_ERROR) {
						zend_throw_error(NULL, "Non-static method %s::%s() %s be called statically", ZSTR_VAL(fcc->calling_scope->name), ZSTR_VAL(fcc->function_handler->common.function_name), verb);
					} else {
						zend_error(severity, "Non-static method %s::%s() %s be called statically", ZSTR_VAL(fcc->calling_scope->name), ZSTR_VAL(fcc->function_handler->common.function_name), verb);
					}
				}
			}
			if (retval && (check_flags & IS_CALLABLE_CHECK_NO_ACCESS) == 0) {
				if (fcc->function_handler->op_array.fn_flags & ZEND_ACC_PRIVATE) {
					if (!zend_check_private(fcc->function_handler, fcc->object ? fcc->object->ce : EG(scope), lmname)) {
						if (error) {
							if (*error) {
								efree(*error);
							}
							zend_spprintf(error, 0, "cannot access private method %s::%s()", ZSTR_VAL(fcc->calling_scope->name), ZSTR_VAL(fcc->function_handler->common.function_name));
						}
						retval = 0;
					}
				} else if ((fcc->function_handler->common.fn_flags & ZEND_ACC_PROTECTED)) {
					if (!zend_check_protected(fcc->function_handler->common.scope, EG(scope))) {
						if (error) {
							if (*error) {
								efree(*error);
							}
							zend_spprintf(error, 0, "cannot access protected method %s::%s()", ZSTR_VAL(fcc->calling_scope->name), ZSTR_VAL(fcc->function_handler->common.function_name));
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
	zend_string_release(lmname);
	zend_string_release(mname);

	if (fcc->object) {
		fcc->called_scope = fcc->object->ce;
	}
	if (retval) {
		fcc->initialized = 1;
	}
	return retval;
}
/* }}} */

ZEND_API zend_bool zend_is_callable_ex(zval *callable, zend_object *object, uint check_flags, zend_string **callable_name, zend_fcall_info_cache *fcc, char **error) /* {{{ */
{
	zend_bool ret;
	zend_fcall_info_cache fcc_local;

	if (callable_name) {
		*callable_name = NULL;
	}
	if (fcc == NULL) {
		fcc = &fcc_local;
	}
	if (error) {
		*error = NULL;
	}

	fcc->initialized = 0;
	fcc->calling_scope = NULL;
	fcc->called_scope = NULL;
	fcc->function_handler = NULL;
	fcc->object = NULL;

	if (object &&
	    (!EG(objects_store).object_buckets ||
	     !IS_OBJ_VALID(EG(objects_store).object_buckets[object->handle]))) {
		return 0;
	}

again:
	switch (Z_TYPE_P(callable)) {
		case IS_STRING:
			if (object) {
				fcc->object = object;
				fcc->calling_scope = object->ce;
				if (callable_name) {
					char *ptr;

					*callable_name = zend_string_alloc(ZSTR_LEN(fcc->calling_scope->name) + Z_STRLEN_P(callable) + sizeof("::") - 1, 0);
					ptr = ZSTR_VAL(*callable_name);
					memcpy(ptr, ZSTR_VAL(fcc->calling_scope->name), ZSTR_LEN(fcc->calling_scope->name));
					ptr += ZSTR_LEN(fcc->calling_scope->name);
					memcpy(ptr, "::", sizeof("::") - 1);
					ptr += sizeof("::") - 1;
					memcpy(ptr, Z_STRVAL_P(callable), Z_STRLEN_P(callable) + 1);
				}
			} else if (callable_name) {
				*callable_name = zend_string_copy(Z_STR_P(callable));
			}
			if (check_flags & IS_CALLABLE_CHECK_SYNTAX_ONLY) {
				fcc->called_scope = fcc->calling_scope;
				return 1;
			}

			ret = zend_is_callable_check_func(check_flags, callable, fcc, 0, error);
			if (fcc == &fcc_local &&
			    fcc->function_handler &&
				((fcc->function_handler->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) ||
			     fcc->function_handler->type == ZEND_OVERLOADED_FUNCTION_TEMPORARY ||
			     fcc->function_handler->type == ZEND_OVERLOADED_FUNCTION)) {
				if (fcc->function_handler->type != ZEND_OVERLOADED_FUNCTION) {
					zend_string_release(fcc->function_handler->common.function_name);
				}
				zend_free_trampoline(fcc->function_handler);
			}
			return ret;

		case IS_ARRAY:
			{
				zval *method = NULL;
				zval *obj = NULL;
				int strict_class = 0;

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
						if (callable_name) {
							char *ptr;


							*callable_name = zend_string_alloc(Z_STRLEN_P(obj) + Z_STRLEN_P(method) + sizeof("::") - 1, 0);
							ptr = ZSTR_VAL(*callable_name);
							memcpy(ptr, Z_STRVAL_P(obj), Z_STRLEN_P(obj));
							ptr += Z_STRLEN_P(obj);
							memcpy(ptr, "::", sizeof("::") - 1);
							ptr += sizeof("::") - 1;
							memcpy(ptr, Z_STRVAL_P(method), Z_STRLEN_P(method) + 1);
						}

						if (check_flags & IS_CALLABLE_CHECK_SYNTAX_ONLY) {
							return 1;
						}

						if (!zend_is_callable_check_class(Z_STR_P(obj), fcc, &strict_class, error)) {
							return 0;
						}

					} else if (Z_TYPE_P(obj) == IS_OBJECT) {
						if (!EG(objects_store).object_buckets ||
						    !IS_OBJ_VALID(EG(objects_store).object_buckets[Z_OBJ_HANDLE_P(obj)])) {
							return 0;
						}

						fcc->calling_scope = Z_OBJCE_P(obj); /* TBFixed: what if it's overloaded? */

						fcc->object = Z_OBJ_P(obj);

						if (callable_name) {
							char *ptr;

							*callable_name = zend_string_alloc(ZSTR_LEN(fcc->calling_scope->name) + Z_STRLEN_P(method) + sizeof("::") - 1, 0);
							ptr = ZSTR_VAL(*callable_name);
							memcpy(ptr, ZSTR_VAL(fcc->calling_scope->name), ZSTR_LEN(fcc->calling_scope->name));
							ptr += ZSTR_LEN(fcc->calling_scope->name);
							memcpy(ptr, "::", sizeof("::") - 1);
							ptr += sizeof("::") - 1;
							memcpy(ptr, Z_STRVAL_P(method), Z_STRLEN_P(method) + 1);
						}

						if (check_flags & IS_CALLABLE_CHECK_SYNTAX_ONLY) {
							fcc->called_scope = fcc->calling_scope;
							return 1;
						}
					} else {
						break;
					}

					ret = zend_is_callable_check_func(check_flags, method, fcc, strict_class, error);
					if (fcc == &fcc_local &&
					    fcc->function_handler &&
						((fcc->function_handler->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) ||
					     fcc->function_handler->type == ZEND_OVERLOADED_FUNCTION_TEMPORARY ||
					     fcc->function_handler->type == ZEND_OVERLOADED_FUNCTION)) {
						if (fcc->function_handler->type != ZEND_OVERLOADED_FUNCTION) {
							zend_string_release(fcc->function_handler->common.function_name);
						}
						zend_free_trampoline(fcc->function_handler);
					}
					return ret;

				} while (0);
				if (zend_hash_num_elements(Z_ARRVAL_P(callable)) == 2) {
					if (!obj || (!Z_ISREF_P(obj)?
								(Z_TYPE_P(obj) != IS_STRING && Z_TYPE_P(obj) != IS_OBJECT) :
								(Z_TYPE_P(Z_REFVAL_P(obj)) != IS_STRING && Z_TYPE_P(Z_REFVAL_P(obj)) != IS_OBJECT))) {
						if (error) zend_spprintf(error, 0, "first array member is not a valid class name or object");
					} else {
						if (error) zend_spprintf(error, 0, "second array member is not a valid method");
					}
				} else {
					if (error) zend_spprintf(error, 0, "array must have exactly two members");
				}
				if (callable_name) {
					*callable_name = zend_string_init("Array", sizeof("Array")-1, 0);
				}
			}
			return 0;
		case IS_OBJECT:
			if (Z_OBJ_HANDLER_P(callable, get_closure) && Z_OBJ_HANDLER_P(callable, get_closure)(callable, &fcc->calling_scope, &fcc->function_handler, &fcc->object) == SUCCESS) {
				fcc->called_scope = fcc->calling_scope;
				if (callable_name) {
					zend_class_entry *ce = Z_OBJCE_P(callable); /* TBFixed: what if it's overloaded? */

					*callable_name = zend_string_alloc(ZSTR_LEN(ce->name) + sizeof("::__invoke") - 1, 0);
					memcpy(ZSTR_VAL(*callable_name), ZSTR_VAL(ce->name), ZSTR_LEN(ce->name));
					memcpy(ZSTR_VAL(*callable_name) + ZSTR_LEN(ce->name), "::__invoke", sizeof("::__invoke"));
				}
				fcc->initialized = 1;
				return 1;
			}
			if (callable_name) {
				*callable_name = zval_get_string(callable);
			}
			if (error) zend_spprintf(error, 0, "no array or string given");
			return 0;
		case IS_REFERENCE:
			callable = Z_REFVAL_P(callable);
			goto again;
		default:
			if (callable_name) {
				*callable_name = zval_get_string(callable);
			}
			if (error) zend_spprintf(error, 0, "no array or string given");
			return 0;
	}
}
/* }}} */

ZEND_API zend_bool zend_is_callable(zval *callable, uint check_flags, zend_string **callable_name) /* {{{ */
{
	return zend_is_callable_ex(callable, NULL, check_flags, callable_name, NULL, NULL);
}
/* }}} */

ZEND_API zend_bool zend_make_callable(zval *callable, zend_string **callable_name) /* {{{ */
{
	zend_fcall_info_cache fcc;

	if (zend_is_callable_ex(callable, NULL, IS_CALLABLE_STRICT, callable_name, &fcc, NULL)) {
		if (Z_TYPE_P(callable) == IS_STRING && fcc.calling_scope) {
			zval_dtor(callable);
			array_init(callable);
			add_next_index_str(callable, zend_string_copy(fcc.calling_scope->name));
			add_next_index_str(callable, zend_string_copy(fcc.function_handler->common.function_name));
		}
		if (fcc.function_handler &&
			((fcc.function_handler->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) ||
		     fcc.function_handler->type == ZEND_OVERLOADED_FUNCTION_TEMPORARY ||
		     fcc.function_handler->type == ZEND_OVERLOADED_FUNCTION)) {
			if (fcc.function_handler->type != ZEND_OVERLOADED_FUNCTION) {
				zend_string_release(fcc.function_handler->common.function_name);
			}
			zend_free_trampoline(fcc.function_handler);
		}
		return 1;
	}
	return 0;
}
/* }}} */

ZEND_API int zend_fcall_info_init(zval *callable, uint check_flags, zend_fcall_info *fci, zend_fcall_info_cache *fcc, zend_string **callable_name, char **error) /* {{{ */
{
	if (!zend_is_callable_ex(callable, NULL, check_flags, callable_name, fcc, error)) {
		return FAILURE;
	}

	fci->size = sizeof(*fci);
	fci->function_table = fcc->calling_scope ? &fcc->calling_scope->function_table : EG(function_table);
	fci->object = fcc->object;
	ZVAL_COPY_VALUE(&fci->function_name, callable);
	fci->retval = NULL;
	fci->param_count = 0;
	fci->params = NULL;
	fci->no_separation = 1;
	fci->symbol_table = NULL;

	return SUCCESS;
}
/* }}} */

ZEND_API void zend_fcall_info_args_clear(zend_fcall_info *fci, int free_mem) /* {{{ */
{
	if (fci->params) {
		zval *p = fci->params;
		zval *end = p + fci->param_count;

		while (p != end) {
			i_zval_ptr_dtor(p ZEND_FILE_LINE_CC);
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
			if (Z_REFCOUNTED_P(arg)) {
				Z_ADDREF_P(arg);
			}
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
	zend_string_free(lname);
	return module ? module->version : NULL;
}
/* }}} */

ZEND_API int zend_declare_property_ex(zend_class_entry *ce, zend_string *name, zval *property, int access_type, zend_string *doc_comment) /* {{{ */
{
	zend_property_info *property_info, *property_info_ptr;

	if (ce->type == ZEND_INTERNAL_CLASS) {
		property_info = pemalloc(sizeof(zend_property_info), 1);
		if ((access_type & ZEND_ACC_STATIC) || Z_CONSTANT_P(property)) {
			ce->ce_flags &= ~ZEND_ACC_CONSTANTS_UPDATED;
		}
	} else {
		property_info = zend_arena_alloc(&CG(arena), sizeof(zend_property_info));
		if (Z_CONSTANT_P(property)) {
			ce->ce_flags &= ~ZEND_ACC_CONSTANTS_UPDATED;
		}
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
		if (ce->type == ZEND_USER_CLASS) {
			ce->static_members_table = ce->default_static_members_table;
		}
	} else {
		if ((property_info_ptr = zend_hash_find_ptr(&ce->properties_info, name)) != NULL &&
		    (property_info_ptr->flags & ZEND_ACC_STATIC) == 0) {
			property_info->offset = property_info_ptr->offset;
			zval_ptr_dtor(&ce->default_properties_table[OBJ_PROP_TO_NUM(property_info->offset)]);
			zend_hash_del(&ce->properties_info, name);
		} else {
			property_info->offset = OBJ_PROP_TO_OFFSET(ce->default_properties_count);
			ce->default_properties_count++;
			ce->default_properties_table = perealloc(ce->default_properties_table, sizeof(zval) * ce->default_properties_count, ce->type == ZEND_INTERNAL_CLASS);
		}
		ZVAL_COPY_VALUE(&ce->default_properties_table[OBJ_PROP_TO_NUM(property_info->offset)], property);
	}
	if (ce->type & ZEND_INTERNAL_CLASS) {
		switch(Z_TYPE_P(property)) {
			case IS_ARRAY:
			case IS_OBJECT:
			case IS_RESOURCE:
				zend_error_noreturn(E_CORE_ERROR, "Internal zval's can't be arrays, objects or resources");
				break;
			default:
				break;
		}
	}
	if (access_type & ZEND_ACC_PUBLIC) {
		property_info->name = zend_string_copy(name);
	} else if (access_type & ZEND_ACC_PRIVATE) {
		property_info->name = zend_mangle_property_name(ZSTR_VAL(ce->name), ZSTR_LEN(ce->name), ZSTR_VAL(name), ZSTR_LEN(name), ce->type & ZEND_INTERNAL_CLASS);
	} else {
		ZEND_ASSERT(access_type & ZEND_ACC_PROTECTED);
		property_info->name = zend_mangle_property_name("*", 1, ZSTR_VAL(name), ZSTR_LEN(name), ce->type & ZEND_INTERNAL_CLASS);
	}

	property_info->name = zend_new_interned_string(property_info->name);
	property_info->flags = access_type;
	property_info->doc_comment = doc_comment;
	property_info->ce = ce;
	zend_hash_update_ptr(&ce->properties_info, name, property_info);

	return SUCCESS;
}
/* }}} */

ZEND_API int zend_declare_property(zend_class_entry *ce, const char *name, size_t name_length, zval *property, int access_type) /* {{{ */
{
	zend_string *key = zend_string_init(name, name_length, ce->type & ZEND_INTERNAL_CLASS);
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

ZEND_API int zend_declare_class_constant(zend_class_entry *ce, const char *name, size_t name_length, zval *value) /* {{{ */
{
	if (Z_CONSTANT_P(value)) {
		ce->ce_flags &= ~ZEND_ACC_CONSTANTS_UPDATED;
	}
	return zend_hash_str_update(&ce->constants_table, name, name_length, value) ?
		SUCCESS : FAILURE;
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
	zval property;
	zend_class_entry *old_scope = EG(scope);

	EG(scope) = scope;

	if (!Z_OBJ_HT_P(object)->write_property) {
		zend_error_noreturn(E_CORE_ERROR, "Property %s of class %s cannot be updated", name, ZSTR_VAL(Z_OBJCE_P(object)->name));
	}
	ZVAL_STR(&property, name);
	Z_OBJ_HT_P(object)->write_property(object, &property, value, NULL);

	EG(scope) = old_scope;
}
/* }}} */

ZEND_API void zend_update_property(zend_class_entry *scope, zval *object, const char *name, size_t name_length, zval *value) /* {{{ */
{
	zval property;
	zend_class_entry *old_scope = EG(scope);

	EG(scope) = scope;

	if (!Z_OBJ_HT_P(object)->write_property) {
		zend_error_noreturn(E_CORE_ERROR, "Property %s of class %s cannot be updated", name, ZSTR_VAL(Z_OBJCE_P(object)->name));
	}
	ZVAL_STRINGL(&property, name, name_length);
	Z_OBJ_HT_P(object)->write_property(object, &property, value, NULL);
	zval_ptr_dtor(&property);

	EG(scope) = old_scope;
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
	zval property;
	zend_class_entry *old_scope = EG(scope);

	EG(scope) = scope;

	if (!Z_OBJ_HT_P(object)->unset_property) {
		zend_error_noreturn(E_CORE_ERROR, "Property %s of class %s cannot be unset", name, ZSTR_VAL(Z_OBJCE_P(object)->name));
	}
	ZVAL_STRINGL(&property, name, name_length);
	Z_OBJ_HT_P(object)->unset_property(object, &property, 0);
	zval_ptr_dtor(&property);

	EG(scope) = old_scope;
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

ZEND_API int zend_update_static_property(zend_class_entry *scope, const char *name, size_t name_length, zval *value) /* {{{ */
{
	zval *property;
	zend_class_entry *old_scope = EG(scope);
	zend_string *key = zend_string_init(name, name_length, 0);

	EG(scope) = scope;
	property = zend_std_get_static_property(scope, key, 0);
	EG(scope) = old_scope;
	zend_string_free(key);
	if (!property) {
		return FAILURE;
	} else {
		if (property != value) {
			if (Z_ISREF_P(property)) {
				zval_dtor(property);
				ZVAL_COPY_VALUE(property, value);
				if (Z_REFCOUNTED_P(value) && Z_REFCOUNT_P(value) > 0) {
					zval_opt_copy_ctor(property);
				}
			} else {
				zval garbage;

				ZVAL_COPY_VALUE(&garbage, property);
				if (Z_REFCOUNTED_P(value)) {
					Z_ADDREF_P(value);
					if (Z_ISREF_P(value)) {
						SEPARATE_ZVAL(value);
					}
				}
				ZVAL_COPY_VALUE(property, value);
				zval_ptr_dtor(&garbage);
			}
		}
		return SUCCESS;
	}
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

ZEND_API zval *zend_read_property(zend_class_entry *scope, zval *object, const char *name, size_t name_length, zend_bool silent, zval *rv) /* {{{ */
{
	zval property, *value;
	zend_class_entry *old_scope = EG(scope);

	EG(scope) = scope;

	if (!Z_OBJ_HT_P(object)->read_property) {
		zend_error_noreturn(E_CORE_ERROR, "Property %s of class %s cannot be read", name, ZSTR_VAL(Z_OBJCE_P(object)->name));
	}

	ZVAL_STRINGL(&property, name, name_length);
	value = Z_OBJ_HT_P(object)->read_property(object, &property, silent?BP_VAR_IS:BP_VAR_R, NULL, rv);
	zval_ptr_dtor(&property);

	EG(scope) = old_scope;
	return value;
}
/* }}} */

ZEND_API zval *zend_read_static_property(zend_class_entry *scope, const char *name, size_t name_length, zend_bool silent) /* {{{ */
{
	zval *property;
	zend_class_entry *old_scope = EG(scope);
	zend_string *key = zend_string_init(name, name_length, 0);

	EG(scope) = scope;
	property = zend_std_get_static_property(scope, key, silent);
	EG(scope) = old_scope;
	zend_string_free(key);

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

ZEND_API zend_string* zend_find_alias_name(zend_class_entry *ce, zend_string *name) /* {{{ */
{
	zend_trait_alias *alias, **alias_ptr;

	if ((alias_ptr = ce->trait_aliases)) {
		alias = *alias_ptr;
		while (alias) {
			if (alias->alias && zend_string_equals_ci(alias->alias, name)) {
				return alias->alias;
			}
			alias_ptr++;
			alias = *alias_ptr;
		}
	}

	return name;
}
/* }}} */

ZEND_API zend_string *zend_resolve_method_name(zend_class_entry *ce, zend_function *f) /* {{{ */
{
	zend_function *func;
	HashTable *function_table;
	zend_string *name;

	if (f->common.type != ZEND_USER_FUNCTION ||
	    (f->op_array.refcount && *(f->op_array.refcount) < 2) ||
	    !f->common.scope ||
	    !f->common.scope->trait_aliases) {
		return f->common.function_name;
	}

	function_table = &ce->function_table;
	ZEND_HASH_FOREACH_STR_KEY_PTR(function_table, name, func) {
		if (func == f) {
			if (!name) {
				return f->common.function_name;
			}
			if (ZSTR_LEN(name) == ZSTR_LEN(f->common.function_name) &&
			    !strncasecmp(ZSTR_VAL(name), ZSTR_VAL(f->common.function_name), ZSTR_LEN(f->common.function_name))) {
				return f->common.function_name;
			}
			return zend_find_alias_name(f->common.scope, name);
		}
	} ZEND_HASH_FOREACH_END();
	return f->common.function_name;
}
/* }}} */

ZEND_API const char *zend_get_object_type(const zend_class_entry *ce) /* {{{ */
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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
