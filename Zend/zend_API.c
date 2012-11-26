/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2012 Zend Technologies Ltd. (http://www.zend.com) |
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
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend.h"
#include "zend_execute.h"
#include "zend_API.h"
#include "zend_modules.h"
#include "zend_constants.h"
#include "zend_exceptions.h"
#include "zend_closures.h"

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
	void **p;
	int arg_count;
	va_list ptr;
	zval **param, *param_ptr;
	TSRMLS_FETCH();

	p = zend_vm_stack_top(TSRMLS_C) - 1;
	arg_count = (int)(zend_uintptr_t) *p;

	if (param_count>arg_count) {
		return FAILURE;
	}

	va_start(ptr, param_count);

	while (param_count-->0) {
		param = va_arg(ptr, zval **);
		param_ptr = *(p-arg_count);
		if (!PZVAL_IS_REF(param_ptr) && Z_REFCOUNT_P(param_ptr) > 1) {
			zval *new_tmp;

			ALLOC_ZVAL(new_tmp);
			*new_tmp = *param_ptr;
			zval_copy_ctor(new_tmp);
			INIT_PZVAL(new_tmp);
			param_ptr = new_tmp;
			Z_DELREF_P((zval *) *(p-arg_count));
			*(p-arg_count) = param_ptr;
		}
		*param = param_ptr;
		arg_count--;
	}
	va_end(ptr);

	return SUCCESS;
}
/* }}} */

ZEND_API int _zend_get_parameters_array(int ht, int param_count, zval **argument_array TSRMLS_DC) /* {{{ */
{
	void **p;
	int arg_count;
	zval *param_ptr;

	p = zend_vm_stack_top(TSRMLS_C) - 1;
	arg_count = (int)(zend_uintptr_t) *p;

	if (param_count>arg_count) {
		return FAILURE;
	}

	while (param_count-->0) {
		param_ptr = *(p-arg_count);
		if (!PZVAL_IS_REF(param_ptr) && Z_REFCOUNT_P(param_ptr) > 1) {
			zval *new_tmp;

			ALLOC_ZVAL(new_tmp);
			*new_tmp = *param_ptr;
			zval_copy_ctor(new_tmp);
			INIT_PZVAL(new_tmp);
			param_ptr = new_tmp;
			Z_DELREF_P((zval *) *(p-arg_count));
			*(p-arg_count) = param_ptr;
		}
		*(argument_array++) = param_ptr;
		arg_count--;
	}

	return SUCCESS;
}
/* }}} */

/* Zend-optimized Extended functions */
/* this function doesn't check for too many parameters */
ZEND_API int zend_get_parameters_ex(int param_count, ...) /* {{{ */
{
	void **p;
	int arg_count;
	va_list ptr;
	zval ***param;
	TSRMLS_FETCH();

	p = zend_vm_stack_top(TSRMLS_C) - 1;
	arg_count = (int)(zend_uintptr_t) *p;

	if (param_count>arg_count) {
		return FAILURE;
	}

	va_start(ptr, param_count);
	while (param_count-->0) {
		param = va_arg(ptr, zval ***);
		*param = (zval **) p-(arg_count--);
	}
	va_end(ptr);

	return SUCCESS;
}
/* }}} */

ZEND_API int _zend_get_parameters_array_ex(int param_count, zval ***argument_array TSRMLS_DC) /* {{{ */
{
	void **p;
	int arg_count;

	p = zend_vm_stack_top(TSRMLS_C) - 1;
	arg_count = (int)(zend_uintptr_t) *p;

	if (param_count>arg_count) {
		return FAILURE;
	}

	while (param_count-->0) {
		zval **value = (zval**)(p-arg_count);

		*(argument_array++) = value;
		arg_count--;
	}

	return SUCCESS;
}
/* }}} */

ZEND_API int zend_copy_parameters_array(int param_count, zval *argument_array TSRMLS_DC) /* {{{ */
{
	void **p;
	int arg_count;

	p = zend_vm_stack_top(TSRMLS_C) - 1;
	arg_count = (int)(zend_uintptr_t) *p;

	if (param_count>arg_count) {
		return FAILURE;
	}

	while (param_count-->0) {
		zval **param = (zval **) p-(arg_count--);
		zval_add_ref(param);
		add_next_index_zval(argument_array, *param);
	}

	return SUCCESS;
}
/* }}} */

ZEND_API void zend_wrong_param_count(TSRMLS_D) /* {{{ */
{
	const char *space;
	const char *class_name = get_active_class_name(&space TSRMLS_CC);

	zend_error(E_WARNING, "Wrong parameter count for %s%s%s()", class_name, space, get_active_function_name(TSRMLS_C));
}
/* }}} */

/* Argument parsing API -- andrei */
ZEND_API char *zend_get_type_by_const(int type) /* {{{ */
{
	switch(type) {
		case IS_BOOL:
			return "boolean";
		case IS_LONG:
			return "integer";
		case IS_DOUBLE:
			return "double";
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
	return zend_get_type_by_const(Z_TYPE_P(arg));
}
/* }}} */

ZEND_API zend_class_entry *zend_get_class_entry(const zval *zobject TSRMLS_DC) /* {{{ */
{
	if (Z_OBJ_HT_P(zobject)->get_class_entry) {
		return Z_OBJ_HT_P(zobject)->get_class_entry(zobject TSRMLS_CC);
	} else {
		zend_error(E_ERROR, "Class entry requested for an object without PHP class");
		return NULL;
	}
}
/* }}} */

/* returns 1 if you need to copy result, 0 if it's already a copy */
ZEND_API int zend_get_object_classname(const zval *object, const char **class_name, zend_uint *class_name_len TSRMLS_DC) /* {{{ */
{
	if (Z_OBJ_HT_P(object)->get_class_name == NULL ||
		Z_OBJ_HT_P(object)->get_class_name(object, class_name, class_name_len, 0 TSRMLS_CC) != SUCCESS) {
		zend_class_entry *ce = Z_OBJCE_P(object);

		*class_name = ce->name;
		*class_name_len = ce->name_length;
		return 1;
	}
	return 0;
}
/* }}} */

static int parse_arg_object_to_string(zval **arg, char **p, int *pl, int type TSRMLS_DC) /* {{{ */
{
	if (Z_OBJ_HANDLER_PP(arg, cast_object)) {
		zval *obj;
		MAKE_STD_ZVAL(obj);
		if (Z_OBJ_HANDLER_P(*arg, cast_object)(*arg, obj, type TSRMLS_CC) == SUCCESS) {
			zval_ptr_dtor(arg);
			*arg = obj;
			*pl = Z_STRLEN_PP(arg);
			*p = Z_STRVAL_PP(arg);
			return SUCCESS;
		}
		efree(obj);
	}
	/* Standard PHP objects */
	if (Z_OBJ_HT_PP(arg) == &std_object_handlers || !Z_OBJ_HANDLER_PP(arg, cast_object)) {
		SEPARATE_ZVAL_IF_NOT_REF(arg);
		if (zend_std_cast_object_tostring(*arg, *arg, type TSRMLS_CC) == SUCCESS) {
			*pl = Z_STRLEN_PP(arg);
			*p = Z_STRVAL_PP(arg);
			return SUCCESS;
		}
	}
	if (!Z_OBJ_HANDLER_PP(arg, cast_object) && Z_OBJ_HANDLER_PP(arg, get)) {
		int use_copy;
		zval *z = Z_OBJ_HANDLER_PP(arg, get)(*arg TSRMLS_CC);
		Z_ADDREF_P(z);
		if(Z_TYPE_P(z) != IS_OBJECT) {
			zval_dtor(*arg);
			Z_TYPE_P(*arg) = IS_NULL;
			zend_make_printable_zval(z, *arg, &use_copy);
			if (!use_copy) {
				ZVAL_ZVAL(*arg, z, 1, 1);
			}
			*pl = Z_STRLEN_PP(arg);
			*p = Z_STRVAL_PP(arg);
			return SUCCESS;
		}
		zval_ptr_dtor(&z);
	}
	return FAILURE;
}
/* }}} */

static const char *zend_parse_arg_impl(int arg_num, zval **arg, va_list *va, const char **spec, char **error, int *severity TSRMLS_DC) /* {{{ */
{
	const char *spec_walk = *spec;
	char c = *spec_walk++;
	int return_null = 0;

	/* scan through modifiers */
	while (1) {
		if (*spec_walk == '/') {
			SEPARATE_ZVAL_IF_NOT_REF(arg);
		} else if (*spec_walk == '!') {
			if (Z_TYPE_PP(arg) == IS_NULL) {
				return_null = 1;
			}
		} else {
			break;
		}
		spec_walk++;
	}

	switch (c) {
		case 'l':
		case 'L':
			{
				long *p = va_arg(*va, long *);
				switch (Z_TYPE_PP(arg)) {
					case IS_STRING:
						{
							double d;
							int type;

							if ((type = is_numeric_string(Z_STRVAL_PP(arg), Z_STRLEN_PP(arg), p, &d, -1)) == 0) {
								return "long";
							} else if (type == IS_DOUBLE) {
								if (c == 'L') {
									if (d > LONG_MAX) {
										*p = LONG_MAX;
										break;
									} else if (d < LONG_MIN) {
										*p = LONG_MIN;
										break;
									}
								}

								*p = zend_dval_to_lval(d);
							}
						}
						break;

					case IS_DOUBLE:
						if (c == 'L') {
							if (Z_DVAL_PP(arg) > LONG_MAX) {
								*p = LONG_MAX;
								break;
							} else if (Z_DVAL_PP(arg) < LONG_MIN) {
								*p = LONG_MIN;
								break;
							}
						}
					case IS_NULL:
					case IS_LONG:
					case IS_BOOL:
						convert_to_long_ex(arg);
						*p = Z_LVAL_PP(arg);
						break;

					case IS_ARRAY:
					case IS_OBJECT:
					case IS_RESOURCE:
					default:
						return "long";
				}
			}
			break;

		case 'd':
			{
				double *p = va_arg(*va, double *);
				switch (Z_TYPE_PP(arg)) {
					case IS_STRING:
						{
							long l;
							int type;

							if ((type = is_numeric_string(Z_STRVAL_PP(arg), Z_STRLEN_PP(arg), &l, p, -1)) == 0) {
								return "double";
							} else if (type == IS_LONG) {
								*p = (double) l;
							}
						}
						break;

					case IS_NULL:
					case IS_LONG:
					case IS_DOUBLE:
					case IS_BOOL:
						convert_to_double_ex(arg);
						*p = Z_DVAL_PP(arg);
						break;

					case IS_ARRAY:
					case IS_OBJECT:
					case IS_RESOURCE:
					default:
						return "double";
				}
			}
			break;

		case 'p':
		case 's':
			{
				char **p = va_arg(*va, char **);
				int *pl = va_arg(*va, int *);
				switch (Z_TYPE_PP(arg)) {
					case IS_NULL:
						if (return_null) {
							*p = NULL;
							*pl = 0;
							break;
						}
						/* break omitted intentionally */

					case IS_STRING:
					case IS_LONG:
					case IS_DOUBLE:
					case IS_BOOL:
						convert_to_string_ex(arg);
						if (UNEXPECTED(Z_ISREF_PP(arg) != 0)) {
							/* it's dangerous to return pointers to string
							   buffer of referenced variable, because it can
							   be clobbered throug magic callbacks */
							SEPARATE_ZVAL(arg);
						}
						*p = Z_STRVAL_PP(arg);
						*pl = Z_STRLEN_PP(arg);
						if (c == 'p' && CHECK_ZVAL_NULL_PATH(*arg)) {
							return "a valid path";
						}
						break;

					case IS_OBJECT:
						if (parse_arg_object_to_string(arg, p, pl, IS_STRING TSRMLS_CC) == SUCCESS) {
							if (c == 'p' && CHECK_ZVAL_NULL_PATH(*arg)) {
								return "a valid path";
							}
							break;
						}

					case IS_ARRAY:
					case IS_RESOURCE:
					default:
						return c == 's' ? "string" : "a valid path";
				}
			}
			break;

		case 'b':
			{
				zend_bool *p = va_arg(*va, zend_bool *);
				switch (Z_TYPE_PP(arg)) {
					case IS_NULL:
					case IS_STRING:
					case IS_LONG:
					case IS_DOUBLE:
					case IS_BOOL:
						convert_to_boolean_ex(arg);
						*p = Z_BVAL_PP(arg);
						break;

					case IS_ARRAY:
					case IS_OBJECT:
					case IS_RESOURCE:
					default:
						return "boolean";
				}
			}
			break;

		case 'r':
			{
				zval **p = va_arg(*va, zval **);
				if (return_null) {
					*p = NULL;
					break;
				}
				if (Z_TYPE_PP(arg) == IS_RESOURCE) {
					*p = *arg;
				} else {
					return "resource";
				}
			}
			break;
		case 'A':
		case 'a':
			{
				zval **p = va_arg(*va, zval **);
				if (return_null) {
					*p = NULL;
					break;
				}
				if (Z_TYPE_PP(arg) == IS_ARRAY || (c == 'A' && Z_TYPE_PP(arg) == IS_OBJECT)) {
					*p = *arg;
				} else {
					return "array";
				}
			}
			break;
		case 'H':
		case 'h':
			{
				HashTable **p = va_arg(*va, HashTable **);
				if (return_null) {
					*p = NULL;
					break;
				}
				if (Z_TYPE_PP(arg) == IS_ARRAY) {
					*p = Z_ARRVAL_PP(arg);
				} else if(c == 'H' && Z_TYPE_PP(arg) == IS_OBJECT) {
					*p = HASH_OF(*arg);
					if(*p == NULL) {
						return "array";
					}
				} else {
					return "array";
				}
			}
			break;

		case 'o':
			{
				zval **p = va_arg(*va, zval **);
				if (return_null) {
					*p = NULL;
					break;
				}
				if (Z_TYPE_PP(arg) == IS_OBJECT) {
					*p = *arg;
				} else {
					return "object";
				}
			}
			break;

		case 'O':
			{
				zval **p = va_arg(*va, zval **);
				zend_class_entry *ce = va_arg(*va, zend_class_entry *);

				if (return_null) {
					*p = NULL;
					break;
				}
				if (Z_TYPE_PP(arg) == IS_OBJECT &&
						(!ce || instanceof_function(Z_OBJCE_PP(arg), ce TSRMLS_CC))) {
					*p = *arg;
				} else {
					if (ce) {
						return ce->name;
					} else {
						return "object";
					}
				}
			}
			break;

		case 'C':
			{
				zend_class_entry **lookup, **pce = va_arg(*va, zend_class_entry **);
				zend_class_entry *ce_base = *pce;

				if (return_null) {
					*pce = NULL;
					break;
				}
				convert_to_string_ex(arg);
				if (zend_lookup_class(Z_STRVAL_PP(arg), Z_STRLEN_PP(arg), &lookup TSRMLS_CC) == FAILURE) {
					*pce = NULL;
				} else {
					*pce = *lookup;
				}
				if (ce_base) {
					if ((!*pce || !instanceof_function(*pce, ce_base TSRMLS_CC))) {
						zend_spprintf(error, 0, "to be a class name derived from %s, '%s' given",
							ce_base->name, Z_STRVAL_PP(arg));
						*pce = NULL;
						return "";
					}
				}
				if (!*pce) {
					zend_spprintf(error, 0, "to be a valid class name, '%s' given",
						Z_STRVAL_PP(arg));
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

				if (return_null) {
					fci->size = 0;
					fcc->initialized = 0;
					break;
				}

				if (zend_fcall_info_init(*arg, 0, fci, fcc, NULL, &is_callable_error TSRMLS_CC) == SUCCESS) {
					if (is_callable_error) {
						*severity = E_STRICT;
						zend_spprintf(error, 0, "to be a valid callback, %s", is_callable_error);
						efree(is_callable_error);
						*spec = spec_walk;
						return "";
					}
					break;
				} else {
					if (is_callable_error) {
						*severity = E_WARNING;
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
				if (return_null) {
					*p = NULL;
				} else {
					*p = *arg;
				}
			}
			break;

		case 'Z':
			{
				zval ***p = va_arg(*va, zval ***);
				if (return_null) {
					*p = NULL;
				} else {
					*p = arg;
				}
			}
			break;

		default:
			return "unknown";
	}

	*spec = spec_walk;

	return NULL;
}
/* }}} */

static int zend_parse_arg(int arg_num, zval **arg, va_list *va, const char **spec, int quiet TSRMLS_DC) /* {{{ */
{
	const char *expected_type = NULL;
	char *error = NULL;
	int severity = E_WARNING;

	expected_type = zend_parse_arg_impl(arg_num, arg, va, spec, &error, &severity TSRMLS_CC);
	if (expected_type) {
		if (!quiet && (*expected_type || error)) {
			const char *space;
			const char *class_name = get_active_class_name(&space TSRMLS_CC);

			if (error) {
				zend_error(severity, "%s%s%s() expects parameter %d %s",
						class_name, space, get_active_function_name(TSRMLS_C), arg_num, error);
				efree(error);
			} else {
				zend_error(severity, "%s%s%s() expects parameter %d to be %s, %s given",
						class_name, space, get_active_function_name(TSRMLS_C), arg_num, expected_type,
						zend_zval_type_name(*arg));
			}
		}
		if (severity != E_STRICT) {
			return FAILURE;
		}
	}

	return SUCCESS;
}
/* }}} */

static int zend_parse_va_args(int num_args, const char *type_spec, va_list *va, int flags TSRMLS_DC) /* {{{ */
{
	const  char *spec_walk;
	int c, i;
	int min_num_args = -1;
	int max_num_args = 0;
	int post_varargs = 0;
	zval **arg;
	int arg_count;
	int quiet = flags & ZEND_PARSE_PARAMS_QUIET;
	zend_bool have_varargs = 0;
	zval ****varargs = NULL;
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
					if (!quiet) {
						zend_function *active_function = EG(current_execute_data)->function_state.function;
						const char *class_name = active_function->common.scope ? active_function->common.scope->name : "";
						zend_error(E_WARNING, "%s%s%s(): only one varargs specifier (* or +) is permitted",
								class_name,
								class_name[0] ? "::" : "",
								active_function->common.function_name);
					}
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
				if (!quiet) {
					zend_function *active_function = EG(current_execute_data)->function_state.function;
					const char *class_name = active_function->common.scope ? active_function->common.scope->name : "";
					zend_error(E_WARNING, "%s%s%s(): bad type specifier while parsing parameters",
							class_name,
							class_name[0] ? "::" : "",
							active_function->common.function_name);
				}
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

	if (num_args < min_num_args || (num_args > max_num_args && max_num_args > 0)) {
		if (!quiet) {
			zend_function *active_function = EG(current_execute_data)->function_state.function;
			const char *class_name = active_function->common.scope ? active_function->common.scope->name : "";
			zend_error(E_WARNING, "%s%s%s() expects %s %d parameter%s, %d given",
					class_name,
					class_name[0] ? "::" : "",
					active_function->common.function_name,
					min_num_args == max_num_args ? "exactly" : num_args < min_num_args ? "at least" : "at most",
					num_args < min_num_args ? min_num_args : max_num_args,
					(num_args < min_num_args ? min_num_args : max_num_args) == 1 ? "" : "s",
					num_args);
		}
		return FAILURE;
	}

	arg_count = (int)(zend_uintptr_t) *(zend_vm_stack_top(TSRMLS_C) - 1);

	if (num_args > arg_count) {
		zend_error(E_WARNING, "%s(): could not obtain parameters for parsing",
			get_active_function_name(TSRMLS_C));
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
			varargs = va_arg(*va, zval ****);
			n_varargs = va_arg(*va, int *);
			type_spec++;

			if (num_varargs > 0) {
				int iv = 0;
				zval **p = (zval **) (zend_vm_stack_top(TSRMLS_C) - 1 - (arg_count - i));

				*n_varargs = num_varargs;

				/* allocate space for array and store args */
				*varargs = safe_emalloc(num_varargs, sizeof(zval **), 0);
				while (num_varargs-- > 0) {
					(*varargs)[iv++] = p++;
				}

				/* adjust how many args we have left and restart loop */
				num_args = num_args + 1 - iv;
				i += iv;
				continue;
			} else {
				*varargs = NULL;
				*n_varargs = 0;
			}
		}

		arg = (zval **) (zend_vm_stack_top(TSRMLS_C) - 1 - (arg_count-i));

		if (zend_parse_arg(i+1, arg, va, &type_spec, quiet TSRMLS_CC) == FAILURE) {
			/* clean up varargs array if it was used */
			if (varargs && *varargs) {
				efree(*varargs);
				*varargs = NULL;
			}
			return FAILURE;
		}
		i++;
	}

	return SUCCESS;
}
/* }}} */

#define RETURN_IF_ZERO_ARGS(num_args, type_spec, quiet) { \
	int __num_args = (num_args); \
	\
	if (0 == (type_spec)[0] && 0 != __num_args && !(quiet)) { \
		const char *__space; \
		const char * __class_name = get_active_class_name(&__space TSRMLS_CC); \
		zend_error(E_WARNING, "%s%s%s() expects exactly 0 parameters, %d given", \
			__class_name, __space, \
			get_active_function_name(TSRMLS_C), __num_args); \
		return FAILURE; \
	}\
}

ZEND_API int zend_parse_parameters_ex(int flags, int num_args TSRMLS_DC, const char *type_spec, ...) /* {{{ */
{
	va_list va;
	int retval;

	RETURN_IF_ZERO_ARGS(num_args, type_spec, flags & ZEND_PARSE_PARAMS_QUIET);

	va_start(va, type_spec);
	retval = zend_parse_va_args(num_args, type_spec, &va, flags TSRMLS_CC);
	va_end(va);

	return retval;
}
/* }}} */

ZEND_API int zend_parse_parameters(int num_args TSRMLS_DC, const char *type_spec, ...) /* {{{ */
{
	va_list va;
	int retval;

	RETURN_IF_ZERO_ARGS(num_args, type_spec, 0);

	va_start(va, type_spec);
	retval = zend_parse_va_args(num_args, type_spec, &va, 0 TSRMLS_CC);
	va_end(va);

	return retval;
}
/* }}} */

ZEND_API int zend_parse_method_parameters(int num_args TSRMLS_DC, zval *this_ptr, const char *type_spec, ...) /* {{{ */
{
	va_list va;
	int retval;
	const char *p = type_spec;
	zval **object;
	zend_class_entry *ce;

	if (!this_ptr) {
		RETURN_IF_ZERO_ARGS(num_args, p, 0);

		va_start(va, type_spec);
		retval = zend_parse_va_args(num_args, type_spec, &va, 0 TSRMLS_CC);
		va_end(va);
	} else {
		p++;
		RETURN_IF_ZERO_ARGS(num_args, p, 0);

		va_start(va, type_spec);

		object = va_arg(va, zval **);
		ce = va_arg(va, zend_class_entry *);
		*object = this_ptr;

		if (ce && !instanceof_function(Z_OBJCE_P(this_ptr), ce TSRMLS_CC)) {
			zend_error(E_CORE_ERROR, "%s::%s() must be derived from %s::%s",
				ce->name, get_active_function_name(TSRMLS_C), Z_OBJCE_P(this_ptr)->name, get_active_function_name(TSRMLS_C));
		}

		retval = zend_parse_va_args(num_args, p, &va, 0 TSRMLS_CC);
		va_end(va);
	}
	return retval;
}
/* }}} */

ZEND_API int zend_parse_method_parameters_ex(int flags, int num_args TSRMLS_DC, zval *this_ptr, const char *type_spec, ...) /* {{{ */
{
	va_list va;
	int retval;
	const char *p = type_spec;
	zval **object;
	zend_class_entry *ce;
	int quiet = flags & ZEND_PARSE_PARAMS_QUIET;

	if (!this_ptr) {
		RETURN_IF_ZERO_ARGS(num_args, p, quiet);

		va_start(va, type_spec);
		retval = zend_parse_va_args(num_args, type_spec, &va, flags TSRMLS_CC);
		va_end(va);
	} else {
		p++;
		RETURN_IF_ZERO_ARGS(num_args, p, quiet);

		va_start(va, type_spec);

		object = va_arg(va, zval **);
		ce = va_arg(va, zend_class_entry *);
		*object = this_ptr;

		if (ce && !instanceof_function(Z_OBJCE_P(this_ptr), ce TSRMLS_CC)) {
			if (!quiet) {
				zend_error(E_CORE_ERROR, "%s::%s() must be derived from %s::%s",
					ce->name, get_active_function_name(TSRMLS_C), Z_OBJCE_P(this_ptr)->name, get_active_function_name(TSRMLS_C));
			}
			va_end(va);
			return FAILURE;
		}

		retval = zend_parse_va_args(num_args, p, &va, flags TSRMLS_CC);
		va_end(va);
	}
	return retval;
}
/* }}} */

/* Argument parsing API -- andrei */
ZEND_API int _array_init(zval *arg, uint size ZEND_FILE_LINE_DC) /* {{{ */
{
	ALLOC_HASHTABLE_REL(Z_ARRVAL_P(arg));

	_zend_hash_init(Z_ARRVAL_P(arg), size, NULL, ZVAL_PTR_DTOR, 0 ZEND_FILE_LINE_RELAY_CC);
	Z_TYPE_P(arg) = IS_ARRAY;
	return SUCCESS;
}
/* }}} */

static int zend_merge_property(zval **value TSRMLS_DC, int num_args, va_list args, const zend_hash_key *hash_key) /* {{{ */
{
	/* which name should a numeric property have ? */
	if (hash_key->nKeyLength) {
		zval *obj = va_arg(args, zval *);
		zend_object_handlers *obj_ht = va_arg(args, zend_object_handlers *);
		zval *member;

		MAKE_STD_ZVAL(member);
		ZVAL_STRINGL(member, hash_key->arKey, hash_key->nKeyLength-1, 1);
		obj_ht->write_property(obj, member, *value, 0 TSRMLS_CC);
		zval_ptr_dtor(&member);
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

/* This function should be called after the constructor has been called
 * because it may call __set from the uninitialized object otherwise. */
ZEND_API void zend_merge_properties(zval *obj, HashTable *properties, int destroy_ht TSRMLS_DC) /* {{{ */
{
	const zend_object_handlers *obj_ht = Z_OBJ_HT_P(obj);
	zend_class_entry *old_scope = EG(scope);

	EG(scope) = Z_OBJCE_P(obj);
	zend_hash_apply_with_arguments(properties TSRMLS_CC, (apply_func_args_t)zend_merge_property, 2, obj, obj_ht);
	EG(scope) = old_scope;

	if (destroy_ht) {
		zend_hash_destroy(properties);
		FREE_HASHTABLE(properties);
	}
}
/* }}} */

ZEND_API void zend_update_class_constants(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
	if ((class_type->ce_flags & ZEND_ACC_CONSTANTS_UPDATED) == 0 || (!CE_STATIC_MEMBERS(class_type) && class_type->default_static_members_count)) {
		zend_class_entry **scope = EG(in_execution)?&EG(scope):&CG(active_class_entry);
		zend_class_entry *old_scope = *scope;
		int i;

		*scope = class_type;
		zend_hash_apply_with_argument(&class_type->constants_table, (apply_func_arg_t) zval_update_constant, (void*)1 TSRMLS_CC);

		for (i = 0; i < class_type->default_properties_count; i++) {
			if (class_type->default_properties_table[i]) {
				zval_update_constant(&class_type->default_properties_table[i], (void**)1 TSRMLS_CC);
			}
		}

		if (!CE_STATIC_MEMBERS(class_type) && class_type->default_static_members_count) {
			zval **p;

			if (class_type->parent) {
				zend_update_class_constants(class_type->parent TSRMLS_CC);
			}
#if ZTS
			CG(static_members_table)[(zend_intptr_t)(class_type->static_members_table)] = emalloc(sizeof(zval*) * class_type->default_static_members_count);
#else
			class_type->static_members_table = emalloc(sizeof(zval*) * class_type->default_static_members_count);
#endif
			for (i = 0; i < class_type->default_static_members_count; i++) {
				p = &class_type->default_static_members_table[i];
				if (Z_ISREF_PP(p) &&
					class_type->parent &&
					i < class_type->parent->default_static_members_count &&
					*p == class_type->parent->default_static_members_table[i] &&
					CE_STATIC_MEMBERS(class_type->parent)[i]
				) {
					zval *q = CE_STATIC_MEMBERS(class_type->parent)[i];

					Z_ADDREF_P(q);
					Z_SET_ISREF_P(q);
					CE_STATIC_MEMBERS(class_type)[i] = q;
				} else {
					zval *r;

					ALLOC_ZVAL(r);
					*r = **p;
					INIT_PZVAL(r);
					zval_copy_ctor(r);
					CE_STATIC_MEMBERS(class_type)[i] = r;
				}
			}
		}

		for (i = 0; i < class_type->default_static_members_count; i++) {
			zval_update_constant(&CE_STATIC_MEMBERS(class_type)[i], (void**)1 TSRMLS_CC);
		}

		*scope = old_scope;
		class_type->ce_flags |= ZEND_ACC_CONSTANTS_UPDATED;
	}
}
/* }}} */

ZEND_API void object_properties_init(zend_object *object, zend_class_entry *class_type) /* {{{ */
{
	int i;

	if (class_type->default_properties_count) {
		object->properties_table = emalloc(sizeof(zval*) * class_type->default_properties_count);
		for (i = 0; i < class_type->default_properties_count; i++) {
			object->properties_table[i] = class_type->default_properties_table[i];
			if (class_type->default_properties_table[i]) {
#if ZTS
				ALLOC_ZVAL( object->properties_table[i]);
				MAKE_COPY_ZVAL(&class_type->default_properties_table[i], object->properties_table[i]);
#else
				Z_ADDREF_P(object->properties_table[i]);
#endif
			}
		}
		object->properties = NULL;
	}
}
/* }}} */

/* This function requires 'properties' to contain all props declared in the
 * class and all props being public. If only a subset is given or the class
 * has protected members then you need to merge the properties seperately by
 * calling zend_merge_properties(). */
ZEND_API int _object_and_properties_init(zval *arg, zend_class_entry *class_type, HashTable *properties ZEND_FILE_LINE_DC TSRMLS_DC) /* {{{ */
{
	zend_object *object;

	if (class_type->ce_flags & (ZEND_ACC_INTERFACE|ZEND_ACC_IMPLICIT_ABSTRACT_CLASS|ZEND_ACC_EXPLICIT_ABSTRACT_CLASS)) {
		char *what =   (class_type->ce_flags & ZEND_ACC_INTERFACE)                ? "interface"
					 :((class_type->ce_flags & ZEND_ACC_TRAIT) == ZEND_ACC_TRAIT) ? "trait"
					 :                                                              "abstract class";
		zend_error(E_ERROR, "Cannot instantiate %s %s", what, class_type->name);
	}

	zend_update_class_constants(class_type TSRMLS_CC);

	Z_TYPE_P(arg) = IS_OBJECT;
	if (class_type->create_object == NULL) {
		Z_OBJVAL_P(arg) = zend_objects_new(&object, class_type TSRMLS_CC);
		if (properties) {
			object->properties = properties;
			object->properties_table = NULL;
		} else {
			object_properties_init(object, class_type);
		}
	} else {
		Z_OBJVAL_P(arg) = class_type->create_object(class_type TSRMLS_CC);
	}
	return SUCCESS;
}
/* }}} */

ZEND_API int _object_init_ex(zval *arg, zend_class_entry *class_type ZEND_FILE_LINE_DC TSRMLS_DC) /* {{{ */
{
	return _object_and_properties_init(arg, class_type, 0 ZEND_FILE_LINE_RELAY_CC TSRMLS_CC);
}
/* }}} */

ZEND_API int _object_init(zval *arg ZEND_FILE_LINE_DC TSRMLS_DC) /* {{{ */
{
	return _object_init_ex(arg, zend_standard_class_def ZEND_FILE_LINE_RELAY_CC TSRMLS_CC);
}
/* }}} */

ZEND_API int add_assoc_function(zval *arg, const char *key, void (*function_ptr)(INTERNAL_FUNCTION_PARAMETERS)) /* {{{ */
{
	zend_error(E_WARNING, "add_assoc_function() is no longer supported");
	return FAILURE;
}
/* }}} */

ZEND_API int add_assoc_long_ex(zval *arg, const char *key, uint key_len, long n) /* {{{ */
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_LONG(tmp, n);

	return zend_symtable_update(Z_ARRVAL_P(arg), key, key_len, (void *) &tmp, sizeof(zval *), NULL);
}
/* }}} */

ZEND_API int add_assoc_null_ex(zval *arg, const char *key, uint key_len) /* {{{ */
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_NULL(tmp);

	return zend_symtable_update(Z_ARRVAL_P(arg), key, key_len, (void *) &tmp, sizeof(zval *), NULL);
}
/* }}} */

ZEND_API int add_assoc_bool_ex(zval *arg, const char *key, uint key_len, int b) /* {{{ */
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_BOOL(tmp, b);

	return zend_symtable_update(Z_ARRVAL_P(arg), key, key_len, (void *) &tmp, sizeof(zval *), NULL);
}
/* }}} */

ZEND_API int add_assoc_resource_ex(zval *arg, const char *key, uint key_len, int r) /* {{{ */
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_RESOURCE(tmp, r);

	return zend_symtable_update(Z_ARRVAL_P(arg), key, key_len, (void *) &tmp, sizeof(zval *), NULL);
}
/* }}} */

ZEND_API int add_assoc_double_ex(zval *arg, const char *key, uint key_len, double d) /* {{{ */
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_DOUBLE(tmp, d);

	return zend_symtable_update(Z_ARRVAL_P(arg), key, key_len, (void *) &tmp, sizeof(zval *), NULL);
}
/* }}} */

ZEND_API int add_assoc_string_ex(zval *arg, const char *key, uint key_len, char *str, int duplicate) /* {{{ */
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_STRING(tmp, str, duplicate);

	return zend_symtable_update(Z_ARRVAL_P(arg), key, key_len, (void *) &tmp, sizeof(zval *), NULL);
}
/* }}} */

ZEND_API int add_assoc_stringl_ex(zval *arg, const char *key, uint key_len, char *str, uint length, int duplicate) /* {{{ */
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_STRINGL(tmp, str, length, duplicate);

	return zend_symtable_update(Z_ARRVAL_P(arg), key, key_len, (void *) &tmp, sizeof(zval *), NULL);
}
/* }}} */

ZEND_API int add_assoc_zval_ex(zval *arg, const char *key, uint key_len, zval *value) /* {{{ */
{
	return zend_symtable_update(Z_ARRVAL_P(arg), key, key_len, (void *) &value, sizeof(zval *), NULL);
}
/* }}} */

ZEND_API int add_index_long(zval *arg, ulong index, long n) /* {{{ */
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_LONG(tmp, n);

	return zend_hash_index_update(Z_ARRVAL_P(arg), index, (void *) &tmp, sizeof(zval *), NULL);
}
/* }}} */

ZEND_API int add_index_null(zval *arg, ulong index) /* {{{ */
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_NULL(tmp);

	return zend_hash_index_update(Z_ARRVAL_P(arg), index, (void *) &tmp, sizeof(zval *), NULL);
}
/* }}} */

ZEND_API int add_index_bool(zval *arg, ulong index, int b) /* {{{ */
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_BOOL(tmp, b);

	return zend_hash_index_update(Z_ARRVAL_P(arg), index, (void *) &tmp, sizeof(zval *), NULL);
}
/* }}} */

ZEND_API int add_index_resource(zval *arg, ulong index, int r) /* {{{ */
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_RESOURCE(tmp, r);

	return zend_hash_index_update(Z_ARRVAL_P(arg), index, (void *) &tmp, sizeof(zval *), NULL);
}
/* }}} */

ZEND_API int add_index_double(zval *arg, ulong index, double d) /* {{{ */
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_DOUBLE(tmp, d);

	return zend_hash_index_update(Z_ARRVAL_P(arg), index, (void *) &tmp, sizeof(zval *), NULL);
}
/* }}} */

ZEND_API int add_index_string(zval *arg, ulong index, const char *str, int duplicate) /* {{{ */
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_STRING(tmp, str, duplicate);

	return zend_hash_index_update(Z_ARRVAL_P(arg), index, (void *) &tmp, sizeof(zval *), NULL);
}
/* }}} */

ZEND_API int add_index_stringl(zval *arg, ulong index, const char *str, uint length, int duplicate) /* {{{ */
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_STRINGL(tmp, str, length, duplicate);

	return zend_hash_index_update(Z_ARRVAL_P(arg), index, (void *) &tmp, sizeof(zval *), NULL);
}
/* }}} */

ZEND_API int add_index_zval(zval *arg, ulong index, zval *value) /* {{{ */
{
	return zend_hash_index_update(Z_ARRVAL_P(arg), index, (void *) &value, sizeof(zval *), NULL);
}
/* }}} */

ZEND_API int add_next_index_long(zval *arg, long n) /* {{{ */
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_LONG(tmp, n);

	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp, sizeof(zval *), NULL);
}
/* }}} */

ZEND_API int add_next_index_null(zval *arg) /* {{{ */
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_NULL(tmp);

	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp, sizeof(zval *), NULL);
}
/* }}} */

ZEND_API int add_next_index_bool(zval *arg, int b) /* {{{ */
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_BOOL(tmp, b);

	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp, sizeof(zval *), NULL);
}
/* }}} */

ZEND_API int add_next_index_resource(zval *arg, int r) /* {{{ */
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_RESOURCE(tmp, r);

	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp, sizeof(zval *), NULL);
}
/* }}} */

ZEND_API int add_next_index_double(zval *arg, double d) /* {{{ */
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_DOUBLE(tmp, d);

	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp, sizeof(zval *), NULL);
}
/* }}} */

ZEND_API int add_next_index_string(zval *arg, const char *str, int duplicate) /* {{{ */
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_STRING(tmp, str, duplicate);

	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp, sizeof(zval *), NULL);
}
/* }}} */

ZEND_API int add_next_index_stringl(zval *arg, const char *str, uint length, int duplicate) /* {{{ */
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_STRINGL(tmp, str, length, duplicate);

	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp, sizeof(zval *), NULL);
}
/* }}} */

ZEND_API int add_next_index_zval(zval *arg, zval *value) /* {{{ */
{
	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &value, sizeof(zval *), NULL);
}
/* }}} */

ZEND_API int add_get_assoc_string_ex(zval *arg, const char *key, uint key_len, const char *str, void **dest, int duplicate) /* {{{ */
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_STRING(tmp, str, duplicate);

	return zend_symtable_update(Z_ARRVAL_P(arg), key, key_len, (void *) &tmp, sizeof(zval *), dest);
}
/* }}} */

ZEND_API int add_get_assoc_stringl_ex(zval *arg, const char *key, uint key_len, const char *str, uint length, void **dest, int duplicate) /* {{{ */
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_STRINGL(tmp, str, length, duplicate);

	return zend_symtable_update(Z_ARRVAL_P(arg), key, key_len, (void *) &tmp, sizeof(zval *), dest);
}
/* }}} */

ZEND_API int add_get_index_long(zval *arg, ulong index, long l, void **dest) /* {{{ */
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_LONG(tmp, l);

	return zend_hash_index_update(Z_ARRVAL_P(arg), index, (void *) &tmp, sizeof(zval *), dest);
}
/* }}} */

ZEND_API int add_get_index_double(zval *arg, ulong index, double d, void **dest) /* {{{ */
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_DOUBLE(tmp, d);

	return zend_hash_index_update(Z_ARRVAL_P(arg), index, (void *) &tmp, sizeof(zval *), dest);
}
/* }}} */

ZEND_API int add_get_index_string(zval *arg, ulong index, const char *str, void **dest, int duplicate) /* {{{ */
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_STRING(tmp, str, duplicate);

	return zend_hash_index_update(Z_ARRVAL_P(arg), index, (void *) &tmp, sizeof(zval *), dest);
}
/* }}} */

ZEND_API int add_get_index_stringl(zval *arg, ulong index, const char *str, uint length, void **dest, int duplicate) /* {{{ */
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_STRINGL(tmp, str, length, duplicate);

	return zend_hash_index_update(Z_ARRVAL_P(arg), index, (void *) &tmp, sizeof(zval *), dest);
}
/* }}} */

ZEND_API int add_property_long_ex(zval *arg, const char *key, uint key_len, long n TSRMLS_DC) /* {{{ */
{
	zval *tmp;
	zval *z_key;

	MAKE_STD_ZVAL(tmp);
	ZVAL_LONG(tmp, n);

	MAKE_STD_ZVAL(z_key);
	ZVAL_STRINGL(z_key, key, key_len-1, 1);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, z_key, tmp, 0 TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_bool_ex(zval *arg, const char *key, uint key_len, int b TSRMLS_DC) /* {{{ */
{
	zval *tmp;
	zval *z_key;

	MAKE_STD_ZVAL(tmp);
	ZVAL_BOOL(tmp, b);

	MAKE_STD_ZVAL(z_key);
	ZVAL_STRINGL(z_key, key, key_len-1, 1);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, z_key, tmp, 0 TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_null_ex(zval *arg, const char *key, uint key_len TSRMLS_DC) /* {{{ */
{
	zval *tmp;
	zval *z_key;

	MAKE_STD_ZVAL(tmp);
	ZVAL_NULL(tmp);

	MAKE_STD_ZVAL(z_key);
	ZVAL_STRINGL(z_key, key, key_len-1, 1);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, z_key, tmp, 0 TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_resource_ex(zval *arg, const char *key, uint key_len, long n TSRMLS_DC) /* {{{ */
{
	zval *tmp;
	zval *z_key;

	MAKE_STD_ZVAL(tmp);
	ZVAL_RESOURCE(tmp, n);

	MAKE_STD_ZVAL(z_key);
	ZVAL_STRINGL(z_key, key, key_len-1, 1);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, z_key, tmp, 0 TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_double_ex(zval *arg, const char *key, uint key_len, double d TSRMLS_DC) /* {{{ */
{
	zval *tmp;
	zval *z_key;

	MAKE_STD_ZVAL(tmp);
	ZVAL_DOUBLE(tmp, d);

	MAKE_STD_ZVAL(z_key);
	ZVAL_STRINGL(z_key, key, key_len-1, 1);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, z_key, tmp, 0 TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_string_ex(zval *arg, const char *key, uint key_len, const char *str, int duplicate TSRMLS_DC) /* {{{ */
{
	zval *tmp;
	zval *z_key;

	MAKE_STD_ZVAL(tmp);
	ZVAL_STRING(tmp, str, duplicate);

	MAKE_STD_ZVAL(z_key);
	ZVAL_STRINGL(z_key, key, key_len-1, 1);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, z_key, tmp, 0 TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_stringl_ex(zval *arg, const char *key, uint key_len, const char *str, uint length, int duplicate TSRMLS_DC) /* {{{ */
{
	zval *tmp;
	zval *z_key;

	MAKE_STD_ZVAL(tmp);
	ZVAL_STRINGL(tmp, str, length, duplicate);

	MAKE_STD_ZVAL(z_key);
	ZVAL_STRINGL(z_key, key, key_len-1, 1);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, z_key, tmp, 0 TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_zval_ex(zval *arg, const char *key, uint key_len, zval *value TSRMLS_DC) /* {{{ */
{
	zval *z_key;

	MAKE_STD_ZVAL(z_key);
	ZVAL_STRINGL(z_key, key, key_len-1, 1);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, z_key, value, 0 TSRMLS_CC);
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int zend_startup_module_ex(zend_module_entry *module TSRMLS_DC) /* {{{ */
{
	int name_len;
	char *lcname;

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
				lcname = zend_str_tolower_dup(dep->name, name_len);

				if (zend_hash_find(&module_registry, lcname, name_len+1, (void**)&req_mod) == FAILURE || !req_mod->module_started) {
					efree(lcname);
					/* TODO: Check version relationship */
					zend_error(E_CORE_WARNING, "Cannot load module '%s' because required module '%s' is not loaded", module->name, dep->name);
					module->module_started = 0;
					return FAILURE;
				}
				efree(lcname);
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
			module->globals_ctor(module->globals_ptr TSRMLS_CC);
		}
#endif
	}
	if (module->module_startup_func) {
		EG(current_module) = module;
		if (module->module_startup_func(module->type, module->module_number TSRMLS_CC)==FAILURE) {
			zend_error(E_CORE_ERROR,"Unable to start %s module", module->name);
			EG(current_module) = NULL;
			return FAILURE;
		}
		EG(current_module) = NULL;
	}
	return SUCCESS;
}
/* }}} */

static void zend_sort_modules(void *base, size_t count, size_t siz, compare_func_t compare TSRMLS_DC) /* {{{ */
{
	Bucket **b1 = base;
	Bucket **b2;
	Bucket **end = b1 + count;
	Bucket *tmp;
	zend_module_entry *m, *r;

	while (b1 < end) {
try_again:
		m = (zend_module_entry*)(*b1)->pData;
		if (!m->module_started && m->deps) {
			const zend_module_dep *dep = m->deps;
			while (dep->name) {
				if (dep->type == MODULE_DEP_REQUIRED || dep->type == MODULE_DEP_OPTIONAL) {
					b2 = b1 + 1;
					while (b2 < end) {
						r = (zend_module_entry*)(*b2)->pData;
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

ZEND_API void zend_collect_module_handlers(TSRMLS_D) /* {{{ */
{
	HashPosition pos;
	zend_module_entry *module;
	int startup_count = 0;
	int shutdown_count = 0;
	int post_deactivate_count = 0;
	zend_class_entry **pce;
	int class_count = 0;

	/* Collect extensions with request startup/shutdown handlers */
	for (zend_hash_internal_pointer_reset_ex(&module_registry, &pos);
	     zend_hash_get_current_data_ex(&module_registry, (void *) &module, &pos) == SUCCESS;
	     zend_hash_move_forward_ex(&module_registry, &pos)) {
		if (module->request_startup_func) {
			startup_count++;
		}
		if (module->request_shutdown_func) {
			shutdown_count++;
		}
		if (module->post_deactivate_func) {
			post_deactivate_count++;
		}
	}
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
	
	for (zend_hash_internal_pointer_reset_ex(&module_registry, &pos);
	     zend_hash_get_current_data_ex(&module_registry, (void *) &module, &pos) == SUCCESS;
	     zend_hash_move_forward_ex(&module_registry, &pos)) {
		if (module->request_startup_func) {
			module_request_startup_handlers[startup_count++] = module;
		}
		if (module->request_shutdown_func) {
			module_request_shutdown_handlers[--shutdown_count] = module;
		}
		if (module->post_deactivate_func) {
			module_post_deactivate_handlers[--post_deactivate_count] = module;
		}
	}

	/* Collect internal classes with static members */
	for (zend_hash_internal_pointer_reset_ex(CG(class_table), &pos);
	     zend_hash_get_current_data_ex(CG(class_table), (void *) &pce, &pos) == SUCCESS;
	     zend_hash_move_forward_ex(CG(class_table), &pos)) {
		if ((*pce)->type == ZEND_INTERNAL_CLASS &&
		    (*pce)->default_static_members_count > 0) {
		    class_count++;
		}
	}

	class_cleanup_handlers = (zend_class_entry**)malloc(
		sizeof(zend_class_entry*) *
		(class_count + 1));
	class_cleanup_handlers[class_count] = NULL;

	if (class_count) {
		for (zend_hash_internal_pointer_reset_ex(CG(class_table), &pos);
		     zend_hash_get_current_data_ex(CG(class_table), (void *) &pce, &pos) == SUCCESS;
	    	 zend_hash_move_forward_ex(CG(class_table), &pos)) {
			if ((*pce)->type == ZEND_INTERNAL_CLASS &&
			    (*pce)->default_static_members_count > 0) {
			    class_cleanup_handlers[--class_count] = *pce;
			}
		}
	}
}
/* }}} */

ZEND_API int zend_startup_modules(TSRMLS_D) /* {{{ */
{
	zend_hash_sort(&module_registry, zend_sort_modules, NULL, 0 TSRMLS_CC);
	zend_hash_apply(&module_registry, (apply_func_t)zend_startup_module_ex TSRMLS_CC);
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

ZEND_API zend_module_entry* zend_register_module_ex(zend_module_entry *module TSRMLS_DC) /* {{{ */
{
	int name_len;
	char *lcname;
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
				lcname = zend_str_tolower_dup(dep->name, name_len);

				if (zend_hash_exists(&module_registry, lcname, name_len+1)) {
					efree(lcname);
					/* TODO: Check version relationship */
					zend_error(E_CORE_WARNING, "Cannot load module '%s' because conflicting module '%s' is already loaded", module->name, dep->name);
					return NULL;
				}
				efree(lcname);
			}
			++dep;
		}
	}

	name_len = strlen(module->name);
	lcname = zend_str_tolower_dup(module->name, name_len);

	if (zend_hash_add(&module_registry, lcname, name_len+1, (void *)module, sizeof(zend_module_entry), (void**)&module_ptr)==FAILURE) {
		zend_error(E_CORE_WARNING, "Module '%s' already loaded", module->name);
		efree(lcname);
		return NULL;
	}
	efree(lcname);
	module = module_ptr;
	EG(current_module) = module;

	if (module->functions && zend_register_functions(NULL, module->functions, NULL, module->type TSRMLS_CC)==FAILURE) {
		EG(current_module) = NULL;
		zend_error(E_CORE_WARNING,"%s: Unable to register functions, unable to load", module->name);
		return NULL;
	}

	EG(current_module) = NULL;
	return module;
}
/* }}} */

ZEND_API zend_module_entry* zend_register_internal_module(zend_module_entry *module TSRMLS_DC) /* {{{ */
{
	module->module_number = zend_next_free_module();
	module->type = MODULE_PERSISTENT;
	return zend_register_module_ex(module TSRMLS_CC);
}
/* }}} */

ZEND_API void zend_check_magic_method_implementation(const zend_class_entry *ce, const zend_function *fptr, int error_type TSRMLS_DC) /* {{{ */
{
	char lcname[16];
	int name_len;

	/* we don't care if the function name is longer, in fact lowercasing only
	 * the beginning of the name speeds up the check process */
	name_len = strlen(fptr->common.function_name);
	zend_str_tolower_copy(lcname, fptr->common.function_name, MIN(name_len, sizeof(lcname)-1));
	lcname[sizeof(lcname)-1] = '\0'; /* zend_str_tolower_copy won't necessarily set the zero byte */

	if (name_len == sizeof(ZEND_DESTRUCTOR_FUNC_NAME) - 1 && !memcmp(lcname, ZEND_DESTRUCTOR_FUNC_NAME, sizeof(ZEND_DESTRUCTOR_FUNC_NAME)) && fptr->common.num_args != 0) {
		zend_error(error_type, "Destructor %s::%s() cannot take arguments", ce->name, ZEND_DESTRUCTOR_FUNC_NAME);
	} else if (name_len == sizeof(ZEND_CLONE_FUNC_NAME) - 1 && !memcmp(lcname, ZEND_CLONE_FUNC_NAME, sizeof(ZEND_CLONE_FUNC_NAME)) && fptr->common.num_args != 0) {
		zend_error(error_type, "Method %s::%s() cannot accept any arguments", ce->name, ZEND_CLONE_FUNC_NAME);
	} else if (name_len == sizeof(ZEND_GET_FUNC_NAME) - 1 && !memcmp(lcname, ZEND_GET_FUNC_NAME, sizeof(ZEND_GET_FUNC_NAME))) {
		if (fptr->common.num_args != 1) {
			zend_error(error_type, "Method %s::%s() must take exactly 1 argument", ce->name, ZEND_GET_FUNC_NAME);
		} else if (ARG_SHOULD_BE_SENT_BY_REF(fptr, 1)) {
			zend_error(error_type, "Method %s::%s() cannot take arguments by reference", ce->name, ZEND_GET_FUNC_NAME);
		}
	} else if (name_len == sizeof(ZEND_SET_FUNC_NAME) - 1 && !memcmp(lcname, ZEND_SET_FUNC_NAME, sizeof(ZEND_SET_FUNC_NAME))) {
		if (fptr->common.num_args != 2) {
			zend_error(error_type, "Method %s::%s() must take exactly 2 arguments", ce->name, ZEND_SET_FUNC_NAME);
		} else if (ARG_SHOULD_BE_SENT_BY_REF(fptr, 1) || ARG_SHOULD_BE_SENT_BY_REF(fptr, 2)) {
			zend_error(error_type, "Method %s::%s() cannot take arguments by reference", ce->name, ZEND_SET_FUNC_NAME);
		}
	} else if (name_len == sizeof(ZEND_UNSET_FUNC_NAME) - 1 && !memcmp(lcname, ZEND_UNSET_FUNC_NAME, sizeof(ZEND_UNSET_FUNC_NAME))) {
		if (fptr->common.num_args != 1) {
			zend_error(error_type, "Method %s::%s() must take exactly 1 argument", ce->name, ZEND_UNSET_FUNC_NAME);
		} else if (ARG_SHOULD_BE_SENT_BY_REF(fptr, 1)) {
			zend_error(error_type, "Method %s::%s() cannot take arguments by reference", ce->name, ZEND_UNSET_FUNC_NAME);
		}
	} else if (name_len == sizeof(ZEND_ISSET_FUNC_NAME) - 1 && !memcmp(lcname, ZEND_ISSET_FUNC_NAME, sizeof(ZEND_ISSET_FUNC_NAME))) {
		if (fptr->common.num_args != 1) {
			zend_error(error_type, "Method %s::%s() must take exactly 1 argument", ce->name, ZEND_ISSET_FUNC_NAME);
		} else if (ARG_SHOULD_BE_SENT_BY_REF(fptr, 1)) {
			zend_error(error_type, "Method %s::%s() cannot take arguments by reference", ce->name, ZEND_ISSET_FUNC_NAME);
		}
	} else if (name_len == sizeof(ZEND_CALL_FUNC_NAME) - 1 && !memcmp(lcname, ZEND_CALL_FUNC_NAME, sizeof(ZEND_CALL_FUNC_NAME))) {
		if (fptr->common.num_args != 2) {
			zend_error(error_type, "Method %s::%s() must take exactly 2 arguments", ce->name, ZEND_CALL_FUNC_NAME);
		} else if (ARG_SHOULD_BE_SENT_BY_REF(fptr, 1) || ARG_SHOULD_BE_SENT_BY_REF(fptr, 2)) {
			zend_error(error_type, "Method %s::%s() cannot take arguments by reference", ce->name, ZEND_CALL_FUNC_NAME);
		}
	} else if (name_len == sizeof(ZEND_CALLSTATIC_FUNC_NAME) - 1 &&
		!memcmp(lcname, ZEND_CALLSTATIC_FUNC_NAME, sizeof(ZEND_CALLSTATIC_FUNC_NAME)-1)
	) {
		if (fptr->common.num_args != 2) {
			zend_error(error_type, "Method %s::%s() must take exactly 2 arguments", ce->name, ZEND_CALLSTATIC_FUNC_NAME);
		} else if (ARG_SHOULD_BE_SENT_BY_REF(fptr, 1) || ARG_SHOULD_BE_SENT_BY_REF(fptr, 2)) {
			zend_error(error_type, "Method %s::%s() cannot take arguments by reference", ce->name, ZEND_CALLSTATIC_FUNC_NAME);
		}
 	} else if (name_len == sizeof(ZEND_TOSTRING_FUNC_NAME) - 1 &&
 		!memcmp(lcname, ZEND_TOSTRING_FUNC_NAME, sizeof(ZEND_TOSTRING_FUNC_NAME)-1) && fptr->common.num_args != 0
	) {
		zend_error(error_type, "Method %s::%s() cannot take arguments", ce->name, ZEND_TOSTRING_FUNC_NAME);
	}
}
/* }}} */

/* registers all functions in *library_functions in the function hash */
ZEND_API int zend_register_functions(zend_class_entry *scope, const zend_function_entry *functions, HashTable *function_table, int type TSRMLS_DC) /* {{{ */
{
	const zend_function_entry *ptr = functions;
	zend_function function, *reg_function;
	zend_internal_function *internal_function = (zend_internal_function *)&function;
	int count=0, unload=0, result=0;
	HashTable *target_function_table = function_table;
	int error_type;
	zend_function *ctor = NULL, *dtor = NULL, *clone = NULL, *__get = NULL, *__set = NULL, *__unset = NULL, *__isset = NULL, *__call = NULL, *__callstatic = NULL, *__tostring = NULL;
	const char *lowercase_name;
	int fname_len;
	const char *lc_class_name = NULL;
	int class_name_len = 0;

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

	if (scope) {
		class_name_len = strlen(scope->name);
		if ((lc_class_name = zend_memrchr(scope->name, '\\', class_name_len))) {
			++lc_class_name;
			class_name_len -= (lc_class_name - scope->name);
			lc_class_name = zend_str_tolower_dup(lc_class_name, class_name_len);
		} else {
			lc_class_name = zend_str_tolower_dup(scope->name, class_name_len);
		}
	}

	while (ptr->fname) {
		internal_function->handler = ptr->handler;
		internal_function->function_name = (char*)ptr->fname;
		internal_function->scope = scope;
		internal_function->prototype = NULL;
		if (ptr->flags) {
			if (!(ptr->flags & ZEND_ACC_PPP_MASK)) {
				if (ptr->flags != ZEND_ACC_DEPRECATED || scope) {
					zend_error(error_type, "Invalid access level for %s%s%s() - access must be exactly one of public, protected or private", scope ? scope->name : "", scope ? "::" : "", ptr->fname);
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
			
			internal_function->arg_info = (zend_arg_info*)ptr->arg_info+1;
			internal_function->num_args = ptr->num_args;
			/* Currently you cannot denote that the function can accept less arguments than num_args */
			if (info->required_num_args == -1) {
				internal_function->required_num_args = ptr->num_args;
			} else {
				internal_function->required_num_args = info->required_num_args;
			}
			if (info->pass_rest_by_reference) {
				if (info->pass_rest_by_reference == ZEND_SEND_PREFER_REF) {
					internal_function->fn_flags |= ZEND_ACC_PASS_REST_PREFER_REF;
				} else {
					internal_function->fn_flags |= ZEND_ACC_PASS_REST_BY_REFERENCE;
				}
			}
			if (info->return_reference) {
				internal_function->fn_flags |= ZEND_ACC_RETURN_REFERENCE;
			}
		} else {
			internal_function->arg_info = NULL;
			internal_function->num_args = 0;
			internal_function->required_num_args = 0;
		}
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
				zend_error(error_type, "Static function %s%s%s() cannot be abstract", scope ? scope->name : "", scope ? "::" : "", ptr->fname);
			}
		} else {
			if (scope && (scope->ce_flags & ZEND_ACC_INTERFACE)) {
				efree((char*)lc_class_name);
				zend_error(error_type, "Interface %s cannot contain non abstract method %s()", scope->name, ptr->fname);
				return FAILURE;
			}
			if (!internal_function->handler) {
				if (scope) {
					efree((char*)lc_class_name);
				}
				zend_error(error_type, "Method %s%s%s() cannot be a NULL function", scope ? scope->name : "", scope ? "::" : "", ptr->fname);
				zend_unregister_functions(functions, count, target_function_table TSRMLS_CC);
				return FAILURE;
			}
		}
		fname_len = strlen(ptr->fname);
		lowercase_name = zend_new_interned_string(zend_str_tolower_dup(ptr->fname, fname_len), fname_len + 1, 1 TSRMLS_CC);
		if (IS_INTERNED(lowercase_name)) {
			result = zend_hash_quick_add(target_function_table, lowercase_name, fname_len+1, INTERNED_HASH(lowercase_name), &function, sizeof(zend_function), (void**)&reg_function);
		} else {
			result = zend_hash_add(target_function_table, lowercase_name, fname_len+1, &function, sizeof(zend_function), (void**)&reg_function);
		}
		if (result == FAILURE) {
			unload=1;
			str_efree(lowercase_name);
			break;
		}
		if (scope) {
			/* Look for ctor, dtor, clone
			 * If it's an old-style constructor, store it only if we don't have
			 * a constructor already.
			 */
			if ((fname_len == class_name_len) && !ctor && !memcmp(lowercase_name, lc_class_name, class_name_len+1)) {
				ctor = reg_function;
			} else if ((fname_len == sizeof(ZEND_CONSTRUCTOR_FUNC_NAME)-1) && !memcmp(lowercase_name, ZEND_CONSTRUCTOR_FUNC_NAME, sizeof(ZEND_CONSTRUCTOR_FUNC_NAME))) {
				ctor = reg_function;
			} else if ((fname_len == sizeof(ZEND_DESTRUCTOR_FUNC_NAME)-1) && !memcmp(lowercase_name, ZEND_DESTRUCTOR_FUNC_NAME, sizeof(ZEND_DESTRUCTOR_FUNC_NAME))) {
				dtor = reg_function;
				if (internal_function->num_args) {
					zend_error(error_type, "Destructor %s::%s() cannot take arguments", scope->name, ptr->fname);
				}
			} else if ((fname_len == sizeof(ZEND_CLONE_FUNC_NAME)-1) && !memcmp(lowercase_name, ZEND_CLONE_FUNC_NAME, sizeof(ZEND_CLONE_FUNC_NAME))) {
				clone = reg_function;
			} else if ((fname_len == sizeof(ZEND_CALL_FUNC_NAME)-1) && !memcmp(lowercase_name, ZEND_CALL_FUNC_NAME, sizeof(ZEND_CALL_FUNC_NAME))) {
				__call = reg_function;
			} else if ((fname_len == sizeof(ZEND_CALLSTATIC_FUNC_NAME)-1) && !memcmp(lowercase_name, ZEND_CALLSTATIC_FUNC_NAME, sizeof(ZEND_CALLSTATIC_FUNC_NAME))) {
				__callstatic = reg_function;
			} else if ((fname_len == sizeof(ZEND_TOSTRING_FUNC_NAME)-1) && !memcmp(lowercase_name, ZEND_TOSTRING_FUNC_NAME, sizeof(ZEND_TOSTRING_FUNC_NAME))) {
				__tostring = reg_function;
			} else if ((fname_len == sizeof(ZEND_GET_FUNC_NAME)-1) && !memcmp(lowercase_name, ZEND_GET_FUNC_NAME, sizeof(ZEND_GET_FUNC_NAME))) {
				__get = reg_function;
			} else if ((fname_len == sizeof(ZEND_SET_FUNC_NAME)-1) && !memcmp(lowercase_name, ZEND_SET_FUNC_NAME, sizeof(ZEND_SET_FUNC_NAME))) {
				__set = reg_function;
			} else if ((fname_len == sizeof(ZEND_UNSET_FUNC_NAME)-1) && !memcmp(lowercase_name, ZEND_UNSET_FUNC_NAME, sizeof(ZEND_UNSET_FUNC_NAME))) {
				__unset = reg_function;
			} else if ((fname_len == sizeof(ZEND_ISSET_FUNC_NAME)-1) && !memcmp(lowercase_name, ZEND_ISSET_FUNC_NAME, sizeof(ZEND_ISSET_FUNC_NAME))) {
				__isset = reg_function;
			} else {
				reg_function = NULL;
			}
			if (reg_function) {
				zend_check_magic_method_implementation(scope, reg_function, error_type TSRMLS_CC);
			}
		}
		ptr++;
		count++;
		str_efree(lowercase_name);
	}
	if (unload) { /* before unloading, display all remaining bad function in the module */
		if (scope) {
			efree((char*)lc_class_name);
		}
		while (ptr->fname) {
			fname_len = strlen(ptr->fname);
			lowercase_name = zend_str_tolower_dup(ptr->fname, fname_len);
			if (zend_hash_exists(target_function_table, lowercase_name, fname_len+1)) {
				zend_error(error_type, "Function registration failed - duplicate name - %s%s%s", scope ? scope->name : "", scope ? "::" : "", ptr->fname);
			}
			efree((char*)lowercase_name);
			ptr++;
		}
		zend_unregister_functions(functions, count, target_function_table TSRMLS_CC);
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
		if (ctor) {
			ctor->common.fn_flags |= ZEND_ACC_CTOR;
			if (ctor->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Constructor %s::%s() cannot be static", scope->name, ctor->common.function_name);
			}
			ctor->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (dtor) {
			dtor->common.fn_flags |= ZEND_ACC_DTOR;
			if (dtor->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Destructor %s::%s() cannot be static", scope->name, dtor->common.function_name);
			}
			dtor->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (clone) {
			clone->common.fn_flags |= ZEND_ACC_CLONE;
			if (clone->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Constructor %s::%s() cannot be static", scope->name, clone->common.function_name);
			}
			clone->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (__call) {
			if (__call->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %s::%s() cannot be static", scope->name, __call->common.function_name);
			}
			__call->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (__callstatic) {
			if (!(__callstatic->common.fn_flags & ZEND_ACC_STATIC)) {
				zend_error(error_type, "Method %s::%s() must be static", scope->name, __callstatic->common.function_name);
			}
			__callstatic->common.fn_flags |= ZEND_ACC_STATIC;
		}
		if (__tostring) {
			if (__tostring->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %s::%s() cannot be static", scope->name, __tostring->common.function_name);
			}
			__tostring->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (__get) {
			if (__get->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %s::%s() cannot be static", scope->name, __get->common.function_name);
			}
			__get->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (__set) {
			if (__set->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %s::%s() cannot be static", scope->name, __set->common.function_name);
			}
			__set->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (__unset) {
			if (__unset->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %s::%s() cannot be static", scope->name, __unset->common.function_name);
			}
			__unset->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (__isset) {
			if (__isset->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %s::%s() cannot be static", scope->name, __isset->common.function_name);
			}
			__isset->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		efree((char*)lc_class_name);
	}
	return SUCCESS;
}
/* }}} */

/* count=-1 means erase all functions, otherwise,
 * erase the first count functions
 */
ZEND_API void zend_unregister_functions(const zend_function_entry *functions, int count, HashTable *function_table TSRMLS_DC) /* {{{ */
{
	const zend_function_entry *ptr = functions;
	int i=0;
	HashTable *target_function_table = function_table;

	if (!target_function_table) {
		target_function_table = CG(function_table);
	}
	while (ptr->fname) {
		if (count!=-1 && i>=count) {
			break;
		}
#if 0
		zend_printf("Unregistering %s()\n", ptr->fname);
#endif
		zend_hash_del(target_function_table, ptr->fname, strlen(ptr->fname)+1);
		ptr++;
		i++;
	}
}
/* }}} */

ZEND_API int zend_startup_module(zend_module_entry *module) /* {{{ */
{
	TSRMLS_FETCH();

	if ((module = zend_register_internal_module(module TSRMLS_CC)) != NULL && zend_startup_module_ex(module TSRMLS_CC) == SUCCESS) {
		return SUCCESS;
	}
	return FAILURE;
}
/* }}} */

ZEND_API int zend_get_module_started(const char *module_name) /* {{{ */
{
	zend_module_entry *module;

	return (zend_hash_find(&module_registry, module_name, strlen(module_name)+1, (void**)&module) == SUCCESS && module->module_started) ? SUCCESS : FAILURE;
}
/* }}} */

static int clean_module_class(const zend_class_entry **ce, int *module_number TSRMLS_DC) /* {{{ */
{
	if ((*ce)->type == ZEND_INTERNAL_CLASS && (*ce)->info.internal.module->module_number == *module_number) {
		return ZEND_HASH_APPLY_REMOVE;
	} else {
		return ZEND_HASH_APPLY_KEEP;
	}
}
/* }}} */

static void clean_module_classes(int module_number TSRMLS_DC) /* {{{ */
{
	zend_hash_apply_with_argument(EG(class_table), (apply_func_arg_t) clean_module_class, (void *) &module_number TSRMLS_CC);
}
/* }}} */

void module_destructor(zend_module_entry *module) /* {{{ */
{
	TSRMLS_FETCH();

	if (module->type == MODULE_TEMPORARY) {
		zend_clean_module_rsrc_dtors(module->module_number TSRMLS_CC);
		clean_module_constants(module->module_number TSRMLS_CC);
		clean_module_classes(module->module_number TSRMLS_CC);
	}

	if (module->module_started && module->module_shutdown_func) {
#if 0
		zend_printf("%s: Module shutdown\n", module->name);
#endif
		module->module_shutdown_func(module->type, module->module_number TSRMLS_CC);
	}

	/* Deinitilaise module globals */
	if (module->globals_size) {
#ifdef ZTS
		if (*module->globals_id_ptr) {
			ts_free_id(*module->globals_id_ptr);
		}
#else
		if (module->globals_dtor) {
			module->globals_dtor(module->globals_ptr TSRMLS_CC);
		}
#endif
	}

	module->module_started=0;
	if (module->functions) {
		zend_unregister_functions(module->functions, -1, NULL TSRMLS_CC);
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

void zend_activate_modules(TSRMLS_D) /* {{{ */
{
	zend_module_entry **p = module_request_startup_handlers;

	while (*p) {
		zend_module_entry *module = *p;

		if (module->request_startup_func(module->type, module->module_number TSRMLS_CC)==FAILURE) {
			zend_error(E_WARNING, "request_startup() for %s module failed", module->name);
			exit(1);
		}
		p++;
	}
}
/* }}} */

/* call request shutdown for all modules */
int module_registry_cleanup(zend_module_entry *module TSRMLS_DC) /* {{{ */
{
	if (module->request_shutdown_func) {
#if 0
		zend_printf("%s: Request shutdown\n", module->name);
#endif
		module->request_shutdown_func(module->type, module->module_number TSRMLS_CC);
	}
	return 0;
}
/* }}} */

void zend_deactivate_modules(TSRMLS_D) /* {{{ */
{
	EG(opline_ptr) = NULL; /* we're no longer executing anything */

	zend_try {
		if (EG(full_tables_cleanup)) {
			zend_hash_reverse_apply(&module_registry, (apply_func_t) module_registry_cleanup TSRMLS_CC);
		} else {
			zend_module_entry **p = module_request_shutdown_handlers;

			while (*p) {
				zend_module_entry *module = *p;

				module->request_shutdown_func(module->type, module->module_number TSRMLS_CC);
				p++;
			}
		}
	} zend_end_try();
}
/* }}} */

ZEND_API void zend_cleanup_internal_classes(TSRMLS_D) /* {{{ */
{
	zend_class_entry **p = class_cleanup_handlers;

	while (*p) {
		zend_cleanup_internal_class_data(*p TSRMLS_CC);
		p++;
	}
}
/* }}} */

int module_registry_unload_temp(const zend_module_entry *module TSRMLS_DC) /* {{{ */
{
	return (module->type == MODULE_TEMPORARY) ? ZEND_HASH_APPLY_REMOVE : ZEND_HASH_APPLY_STOP;
}
/* }}} */

static int exec_done_cb(zend_module_entry *module TSRMLS_DC) /* {{{ */
{
	if (module->post_deactivate_func) {
		module->post_deactivate_func();
	}
	return 0;
}
/* }}} */

void zend_post_deactivate_modules(TSRMLS_D) /* {{{ */
{
	if (EG(full_tables_cleanup)) {
		zend_hash_apply(&module_registry, (apply_func_t) exec_done_cb TSRMLS_CC);
		zend_hash_reverse_apply(&module_registry, (apply_func_t) module_registry_unload_temp TSRMLS_CC);
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
int zend_next_free_module(void) /* {{{ */
{
	return zend_hash_num_elements(&module_registry) + 1;
}
/* }}} */

static zend_class_entry *do_register_internal_class(zend_class_entry *orig_class_entry, zend_uint ce_flags TSRMLS_DC) /* {{{ */
{
	zend_class_entry *class_entry = malloc(sizeof(zend_class_entry));
	char *lowercase_name = emalloc(orig_class_entry->name_length + 1);
	*class_entry = *orig_class_entry;

	class_entry->type = ZEND_INTERNAL_CLASS;
	zend_initialize_class_data(class_entry, 0 TSRMLS_CC);
	class_entry->ce_flags = ce_flags;
	class_entry->info.internal.module = EG(current_module);

	if (class_entry->info.internal.builtin_functions) {
		zend_register_functions(class_entry, class_entry->info.internal.builtin_functions, &class_entry->function_table, MODULE_PERSISTENT TSRMLS_CC);
	}

	zend_str_tolower_copy(lowercase_name, orig_class_entry->name, class_entry->name_length);
	lowercase_name = (char*)zend_new_interned_string(lowercase_name, class_entry->name_length + 1, 1 TSRMLS_CC);
	if (IS_INTERNED(lowercase_name)) {
		zend_hash_quick_update(CG(class_table), lowercase_name, class_entry->name_length+1, INTERNED_HASH(lowercase_name), &class_entry, sizeof(zend_class_entry *), NULL);
	} else {
		zend_hash_update(CG(class_table), lowercase_name, class_entry->name_length+1, &class_entry, sizeof(zend_class_entry *), NULL);
	}
	str_efree(lowercase_name);
	return class_entry;
}
/* }}} */

/* If parent_ce is not NULL then it inherits from parent_ce
 * If parent_ce is NULL and parent_name isn't then it looks for the parent and inherits from it
 * If both parent_ce and parent_name are NULL it does a regular class registration
 * If parent_name is specified but not found NULL is returned
 */
ZEND_API zend_class_entry *zend_register_internal_class_ex(zend_class_entry *class_entry, zend_class_entry *parent_ce, char *parent_name TSRMLS_DC) /* {{{ */
{
	zend_class_entry *register_class;

	if (!parent_ce && parent_name) {
		zend_class_entry **pce;
		if (zend_hash_find(CG(class_table), parent_name, strlen(parent_name)+1, (void **) &pce)==FAILURE) {
			return NULL;
		} else {
			parent_ce = *pce;
		}
	}

	register_class = zend_register_internal_class(class_entry TSRMLS_CC);

	if (parent_ce) {
		zend_do_inheritance(register_class, parent_ce TSRMLS_CC);
	}
	return register_class;
}
/* }}} */

ZEND_API void zend_class_implements(zend_class_entry *class_entry TSRMLS_DC, int num_interfaces, ...) /* {{{ */
{
	zend_class_entry *interface_entry;
	va_list interface_list;
	va_start(interface_list, num_interfaces);

	while (num_interfaces--) {
		interface_entry = va_arg(interface_list, zend_class_entry *);
		zend_do_implement_interface(class_entry, interface_entry TSRMLS_CC);
	}

	va_end(interface_list);
}
/* }}} */

/* A class that contains at least one abstract method automatically becomes an abstract class.
 */
ZEND_API zend_class_entry *zend_register_internal_class(zend_class_entry *orig_class_entry TSRMLS_DC) /* {{{ */
{
	return do_register_internal_class(orig_class_entry, 0 TSRMLS_CC);
}
/* }}} */

ZEND_API zend_class_entry *zend_register_internal_interface(zend_class_entry *orig_class_entry TSRMLS_DC) /* {{{ */
{
	return do_register_internal_class(orig_class_entry, ZEND_ACC_INTERFACE TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_register_class_alias_ex(const char *name, int name_len, zend_class_entry *ce TSRMLS_DC) /* {{{ */
{
	char *lcname = zend_str_tolower_dup(name, name_len);
	int ret;

	ret = zend_hash_add(CG(class_table), lcname, name_len+1, &ce, sizeof(zend_class_entry *), NULL);
	efree(lcname);
	if (ret == SUCCESS) {
		ce->refcount++;
	}
	return ret;
}
/* }}} */

ZEND_API int zend_set_hash_symbol(zval *symbol, const char *name, int name_length, zend_bool is_ref, int num_symbol_tables, ...) /* {{{ */
{
	HashTable *symbol_table;
	va_list symbol_table_list;

	if (num_symbol_tables <= 0) return FAILURE;

	Z_SET_ISREF_TO_P(symbol, is_ref);

	va_start(symbol_table_list, num_symbol_tables);
	while (num_symbol_tables-- > 0) {
		symbol_table = va_arg(symbol_table_list, HashTable *);
		zend_hash_update(symbol_table, name, name_length + 1, &symbol, sizeof(zval *), NULL);
		zval_add_ref(&symbol);
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
	zend_error(E_WARNING, "%s() has been disabled for security reasons", get_active_function_name(TSRMLS_C));
}
/* }}} */

static zend_function_entry disabled_function[] = {
	ZEND_FE(display_disabled_function,			NULL)
	ZEND_FE_END
};

ZEND_API int zend_disable_function(char *function_name, uint function_name_length TSRMLS_DC) /* {{{ */
{
	if (zend_hash_del(CG(function_table), function_name, function_name_length+1)==FAILURE) {
		return FAILURE;
	}
	disabled_function[0].fname = function_name;
	return zend_register_functions(NULL, disabled_function, CG(function_table), MODULE_PERSISTENT TSRMLS_CC);
}
/* }}} */

#ifdef ZEND_WIN32
#pragma optimize("", off)
#endif
static zend_object_value display_disabled_class(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
	zend_object_value retval;
	zend_object *intern;
	retval = zend_objects_new(&intern, class_type TSRMLS_CC);
	zend_error(E_WARNING, "%s() has been disabled for security reasons", class_type->name);
	return retval;
}
#ifdef ZEND_WIN32
#pragma optimize("", on)
#endif
/* }}} */

static const zend_function_entry disabled_class_new[] = {
	ZEND_FE_END
};

ZEND_API int zend_disable_class(char *class_name, uint class_name_length TSRMLS_DC) /* {{{ */
{
	zend_class_entry **disabled_class;

	zend_str_tolower(class_name, class_name_length);
	if (zend_hash_find(CG(class_table), class_name, class_name_length+1, (void **)&disabled_class)==FAILURE) {
		return FAILURE;
	}
	INIT_CLASS_ENTRY_INIT_METHODS((**disabled_class), disabled_class_new, NULL, NULL, NULL, NULL, NULL);
	(*disabled_class)->create_object = display_disabled_class;
	zend_hash_clean(&((*disabled_class)->function_table));
	return SUCCESS;
}
/* }}} */

static int zend_is_callable_check_class(const char *name, int name_len, zend_fcall_info_cache *fcc, int *strict_class, char **error TSRMLS_DC) /* {{{ */
{
	int ret = 0;
	zend_class_entry **pce;
	char *lcname = zend_str_tolower_dup(name, name_len);

	*strict_class = 0;
	if (name_len == sizeof("self") - 1 &&
	    !memcmp(lcname, "self", sizeof("self") - 1)) {
		if (!EG(scope)) {
			if (error) *error = estrdup("cannot access self:: when no class scope is active");
		} else {
			fcc->called_scope = EG(called_scope);
			fcc->calling_scope = EG(scope);
			if (!fcc->object_ptr) {
				fcc->object_ptr = EG(This);
			}
			ret = 1;
		}
	} else if (name_len == sizeof("parent") - 1 && 
		       !memcmp(lcname, "parent", sizeof("parent") - 1)) {
		if (!EG(scope)) {
			if (error) *error = estrdup("cannot access parent:: when no class scope is active");
		} else if (!EG(scope)->parent) {
			if (error) *error = estrdup("cannot access parent:: when current class scope has no parent");
		} else {
			fcc->called_scope = EG(called_scope);
			fcc->calling_scope = EG(scope)->parent;
			if (!fcc->object_ptr) {
				fcc->object_ptr = EG(This);
			}
			*strict_class = 1;
			ret = 1;
		}
	} else if (name_len == sizeof("static") - 1 &&
	           !memcmp(lcname, "static", sizeof("static") - 1)) {
		if (!EG(called_scope)) {
			if (error) *error = estrdup("cannot access static:: when no class scope is active");
		} else {
			fcc->called_scope = EG(called_scope);
			fcc->calling_scope = EG(called_scope);
			if (!fcc->object_ptr) {
				fcc->object_ptr = EG(This);
			}
			*strict_class = 1;
			ret = 1;
		}
	} else if (zend_lookup_class_ex(name, name_len, NULL, 1, &pce TSRMLS_CC) == SUCCESS) {
		zend_class_entry *scope = EG(active_op_array) ? EG(active_op_array)->scope : NULL;

		fcc->calling_scope = *pce;
		if (scope && !fcc->object_ptr && EG(This) &&
		    instanceof_function(Z_OBJCE_P(EG(This)), scope TSRMLS_CC) &&
		    instanceof_function(scope, fcc->calling_scope TSRMLS_CC)) {
			fcc->object_ptr = EG(This);
			fcc->called_scope = Z_OBJCE_P(fcc->object_ptr);
		} else {
			fcc->called_scope = fcc->object_ptr ? Z_OBJCE_P(fcc->object_ptr) : fcc->calling_scope;
		}
		*strict_class = 1;
		ret = 1;
	} else {
		if (error) zend_spprintf(error, 0, "class '%.*s' not found", name_len, name);
	}
	efree(lcname);
	return ret;
}
/* }}} */

static int zend_is_callable_check_func(int check_flags, zval *callable, zend_fcall_info_cache *fcc, int strict_class, char **error TSRMLS_DC) /* {{{ */
{
	zend_class_entry *ce_org = fcc->calling_scope;
	int retval = 0;
	char *mname, *lmname;
	const char *colon;
	int clen, mlen;
	zend_class_entry *last_scope;
	HashTable *ftable;
	int call_via_handler = 0;

	if (error) {
		*error = NULL;
	}

	fcc->calling_scope = NULL;
	fcc->function_handler = NULL;

	if (!ce_org) {
		/* Skip leading \ */
		if (Z_STRVAL_P(callable)[0] == '\\') {
			mlen = Z_STRLEN_P(callable) - 1;
			lmname = zend_str_tolower_dup(Z_STRVAL_P(callable) + 1, mlen);
		} else {
			mlen = Z_STRLEN_P(callable);
			lmname = zend_str_tolower_dup(Z_STRVAL_P(callable), mlen);
		}
		/* Check if function with given name exists.
		 * This may be a compound name that includes namespace name */
		if (zend_hash_find(EG(function_table), lmname, mlen+1, (void**)&fcc->function_handler) == SUCCESS) {
			efree(lmname);
			return 1;
		}
		efree(lmname);
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

		if (!zend_is_callable_check_class(Z_STRVAL_P(callable), clen, fcc, &strict_class, error TSRMLS_CC)) {
			EG(scope) = last_scope;
			return 0;
		}
		EG(scope) = last_scope;

		ftable = &fcc->calling_scope->function_table;
		if (ce_org && !instanceof_function(ce_org, fcc->calling_scope TSRMLS_CC)) {
			if (error) zend_spprintf(error, 0, "class '%s' is not a subclass of '%s'", ce_org->name, fcc->calling_scope->name);
			return 0;
		}
		mname = Z_STRVAL_P(callable) + clen + 2;
	} else if (ce_org) {
		/* Try to fetch find static method of given class. */
		mlen = Z_STRLEN_P(callable);
		mname = Z_STRVAL_P(callable);
		ftable = &ce_org->function_table;
		fcc->calling_scope = ce_org;
	} else {
		/* We already checked for plain function before. */
		if (error && !(check_flags & IS_CALLABLE_CHECK_SILENT)) {
			zend_spprintf(error, 0, "function '%s' not found or invalid function name", Z_STRVAL_P(callable));
		}
		return 0;
	}

	lmname = zend_str_tolower_dup(mname, mlen);
	if (strict_class &&
	    fcc->calling_scope &&
	    mlen == sizeof(ZEND_CONSTRUCTOR_FUNC_NAME)-1 &&
	    !memcmp(lmname, ZEND_CONSTRUCTOR_FUNC_NAME, sizeof(ZEND_CONSTRUCTOR_FUNC_NAME))) {
		fcc->function_handler = fcc->calling_scope->constructor;
		if (fcc->function_handler) {
			retval = 1;
		}
	} else if (zend_hash_find(ftable, lmname, mlen+1, (void**)&fcc->function_handler) == SUCCESS) {
		retval = 1;
		if ((fcc->function_handler->op_array.fn_flags & ZEND_ACC_CHANGED) &&
		    !strict_class && EG(scope) &&
		    instanceof_function(fcc->function_handler->common.scope, EG(scope) TSRMLS_CC)) {
			zend_function *priv_fbc;

			if (zend_hash_find(&EG(scope)->function_table, lmname, mlen+1, (void **) &priv_fbc)==SUCCESS
				&& priv_fbc->common.fn_flags & ZEND_ACC_PRIVATE
				&& priv_fbc->common.scope == EG(scope)) {
				fcc->function_handler = priv_fbc;
			}
		}
		if ((check_flags & IS_CALLABLE_CHECK_NO_ACCESS) == 0 &&
		    (fcc->calling_scope &&
		     (fcc->calling_scope->__call ||
		      fcc->calling_scope->__callstatic))) {
			if (fcc->function_handler->op_array.fn_flags & ZEND_ACC_PRIVATE) {
				if (!zend_check_private(fcc->function_handler, fcc->object_ptr ? Z_OBJCE_P(fcc->object_ptr) : EG(scope), lmname, mlen TSRMLS_CC)) {
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
		if (fcc->object_ptr && fcc->calling_scope == ce_org) {
			if (strict_class && ce_org->__call) {
				fcc->function_handler = emalloc(sizeof(zend_internal_function));
				fcc->function_handler->internal_function.type = ZEND_INTERNAL_FUNCTION;
				fcc->function_handler->internal_function.module = (ce_org->type == ZEND_INTERNAL_CLASS) ? ce_org->info.internal.module : NULL;
				fcc->function_handler->internal_function.handler = zend_std_call_user_call;
				fcc->function_handler->internal_function.arg_info = NULL;
				fcc->function_handler->internal_function.num_args = 0;
				fcc->function_handler->internal_function.scope = ce_org;
				fcc->function_handler->internal_function.fn_flags = ZEND_ACC_CALL_VIA_HANDLER;
				fcc->function_handler->internal_function.function_name = estrndup(mname, mlen);
				call_via_handler = 1;
				retval = 1;
			} else if (Z_OBJ_HT_P(fcc->object_ptr)->get_method) {
				fcc->function_handler = Z_OBJ_HT_P(fcc->object_ptr)->get_method(&fcc->object_ptr, mname, mlen, NULL TSRMLS_CC);
				if (fcc->function_handler) {
					if (strict_class &&
					    (!fcc->function_handler->common.scope ||
					     !instanceof_function(ce_org, fcc->function_handler->common.scope TSRMLS_CC))) {
						if ((fcc->function_handler->common.fn_flags & ZEND_ACC_CALL_VIA_HANDLER) != 0) {
							if (fcc->function_handler->type != ZEND_OVERLOADED_FUNCTION) {
								efree((char*)fcc->function_handler->common.function_name);
							}
							efree(fcc->function_handler);
						}
					} else {
						retval = 1;
						call_via_handler = (fcc->function_handler->common.fn_flags & ZEND_ACC_CALL_VIA_HANDLER) != 0;
					}
				}
			}
		} else if (fcc->calling_scope) {
			if (fcc->calling_scope->get_static_method) {
				fcc->function_handler = fcc->calling_scope->get_static_method(fcc->calling_scope, mname, mlen TSRMLS_CC);
			} else {
				fcc->function_handler = zend_std_get_static_method(fcc->calling_scope, mname, mlen, NULL TSRMLS_CC);
			}
			if (fcc->function_handler) {
				retval = 1;
				call_via_handler = (fcc->function_handler->common.fn_flags & ZEND_ACC_CALL_VIA_HANDLER) != 0;
				if (call_via_handler && !fcc->object_ptr && EG(This) &&
				    Z_OBJ_HT_P(EG(This))->get_class_entry &&
				    instanceof_function(Z_OBJCE_P(EG(This)), fcc->calling_scope TSRMLS_CC)) {
					fcc->object_ptr = EG(This);
				}
			}
		}
	}

	if (retval) {
		if (fcc->calling_scope && !call_via_handler) {
			if (!fcc->object_ptr && (fcc->function_handler->common.fn_flags & ZEND_ACC_ABSTRACT)) {
				if (error) {
					zend_spprintf(error, 0, "cannot call abstract method %s::%s()", fcc->calling_scope->name, fcc->function_handler->common.function_name);
					retval = 0;
				} else {
					zend_error(E_ERROR, "Cannot call abstract method %s::%s()", fcc->calling_scope->name, fcc->function_handler->common.function_name);
				}
			} else if (!fcc->object_ptr && !(fcc->function_handler->common.fn_flags & ZEND_ACC_STATIC)) {
				int severity;
				char *verb;
				if (fcc->function_handler->common.fn_flags & ZEND_ACC_ALLOW_STATIC) {
					severity = E_STRICT;
					verb = "should not";
				} else {
					/* An internal function assumes $this is present and won't check that. So PHP would crash by allowing the call. */
					severity = E_ERROR;
					verb = "cannot";
				}
				if ((check_flags & IS_CALLABLE_CHECK_IS_STATIC) != 0) {
					retval = 0;
				}
				if (EG(This) && instanceof_function(Z_OBJCE_P(EG(This)), fcc->calling_scope TSRMLS_CC)) {
					fcc->object_ptr = EG(This);
					if (error) {
						zend_spprintf(error, 0, "non-static method %s::%s() %s be called statically, assuming $this from compatible context %s", fcc->calling_scope->name, fcc->function_handler->common.function_name, verb, Z_OBJCE_P(EG(This))->name);
						if (severity == E_ERROR) {
							retval = 0;
						}
					} else if (retval) {
						zend_error(severity, "Non-static method %s::%s() %s be called statically, assuming $this from compatible context %s", fcc->calling_scope->name, fcc->function_handler->common.function_name, verb, Z_OBJCE_P(EG(This))->name);
					}
				} else {
					if (error) {
						zend_spprintf(error, 0, "non-static method %s::%s() %s be called statically", fcc->calling_scope->name, fcc->function_handler->common.function_name, verb);
						if (severity == E_ERROR) {
							retval = 0;
						}
					} else if (retval) {
						zend_error(severity, "Non-static method %s::%s() %s be called statically", fcc->calling_scope->name, fcc->function_handler->common.function_name, verb);
					}
				}
			}
			if (retval && (check_flags & IS_CALLABLE_CHECK_NO_ACCESS) == 0) {
				if (fcc->function_handler->op_array.fn_flags & ZEND_ACC_PRIVATE) {
					if (!zend_check_private(fcc->function_handler, fcc->object_ptr ? Z_OBJCE_P(fcc->object_ptr) : EG(scope), lmname, mlen TSRMLS_CC)) {
						if (error) {
							if (*error) {
								efree(*error);
							}
							zend_spprintf(error, 0, "cannot access private method %s::%s()", fcc->calling_scope->name, fcc->function_handler->common.function_name);
						}
						retval = 0;
					}
				} else if ((fcc->function_handler->common.fn_flags & ZEND_ACC_PROTECTED)) {
					if (!zend_check_protected(fcc->function_handler->common.scope, EG(scope))) {
						if (error) {
							if (*error) {
								efree(*error);
							}
							zend_spprintf(error, 0, "cannot access protected method %s::%s()", fcc->calling_scope->name, fcc->function_handler->common.function_name);
						}
						retval = 0;
					}
				}
			}
		}
	} else if (error && !(check_flags & IS_CALLABLE_CHECK_SILENT)) {
		if (fcc->calling_scope) {
			if (error) zend_spprintf(error, 0, "class '%s' does not have a method '%s'", fcc->calling_scope->name, mname);
		} else {
			if (error) zend_spprintf(error, 0, "function '%s' does not exist", mname);
		}
	}
	efree(lmname);

	if (fcc->object_ptr) {
		fcc->called_scope = Z_OBJCE_P(fcc->object_ptr);
	}
	if (retval) {
		fcc->initialized = 1;
	}
	return retval;
}
/* }}} */

ZEND_API zend_bool zend_is_callable_ex(zval *callable, zval *object_ptr, uint check_flags, char **callable_name, int *callable_name_len, zend_fcall_info_cache *fcc, char **error TSRMLS_DC) /* {{{ */
{
	zend_bool ret;
	int callable_name_len_local;
	zend_fcall_info_cache fcc_local;

	if (callable_name) {
		*callable_name = NULL;
	}
	if (callable_name_len == NULL) {
		callable_name_len = &callable_name_len_local;
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
	fcc->calling_scope = NULL;
	fcc->object_ptr = NULL;

	if (object_ptr && Z_TYPE_P(object_ptr) != IS_OBJECT) {
		object_ptr = NULL;
	}
	if (object_ptr &&
	    (!EG(objects_store).object_buckets || 
	     !EG(objects_store).object_buckets[Z_OBJ_HANDLE_P(object_ptr)].valid)) {
		return 0;
	}

	switch (Z_TYPE_P(callable)) {
		case IS_STRING:
			if (object_ptr) {
				fcc->object_ptr = object_ptr;
				fcc->calling_scope = Z_OBJCE_P(object_ptr);
				if (callable_name) {
					char *ptr;

					*callable_name_len = fcc->calling_scope->name_length + Z_STRLEN_P(callable) + sizeof("::") - 1;
					ptr = *callable_name = emalloc(*callable_name_len + 1);
					memcpy(ptr, fcc->calling_scope->name, fcc->calling_scope->name_length);
					ptr += fcc->calling_scope->name_length;
					memcpy(ptr, "::", sizeof("::") - 1);
					ptr += sizeof("::") - 1;
					memcpy(ptr, Z_STRVAL_P(callable), Z_STRLEN_P(callable) + 1);
				}
			} else if (callable_name) {
				*callable_name = estrndup(Z_STRVAL_P(callable), Z_STRLEN_P(callable));
				*callable_name_len = Z_STRLEN_P(callable);
			}
			if (check_flags & IS_CALLABLE_CHECK_SYNTAX_ONLY) {
				fcc->called_scope = fcc->calling_scope;
				return 1;
			}

			ret = zend_is_callable_check_func(check_flags, callable, fcc, 0, error TSRMLS_CC);
			if (fcc == &fcc_local &&
			    fcc->function_handler &&
				((fcc->function_handler->type == ZEND_INTERNAL_FUNCTION &&
			      (fcc->function_handler->common.fn_flags & ZEND_ACC_CALL_VIA_HANDLER)) ||
			     fcc->function_handler->type == ZEND_OVERLOADED_FUNCTION_TEMPORARY ||
			     fcc->function_handler->type == ZEND_OVERLOADED_FUNCTION)) {
				if (fcc->function_handler->type != ZEND_OVERLOADED_FUNCTION) {
					efree((char*)fcc->function_handler->common.function_name);
				}
				efree(fcc->function_handler);
			}
			return ret;

		case IS_ARRAY:
			{
				zval **method = NULL;
				zval **obj = NULL;
				int strict_class = 0;

				if (zend_hash_num_elements(Z_ARRVAL_P(callable)) == 2) {
					zend_hash_index_find(Z_ARRVAL_P(callable), 0, (void **) &obj);
					zend_hash_index_find(Z_ARRVAL_P(callable), 1, (void **) &method);
				}
				if (obj && method &&
					(Z_TYPE_PP(obj) == IS_OBJECT ||
					Z_TYPE_PP(obj) == IS_STRING) &&
					Z_TYPE_PP(method) == IS_STRING) {

					if (Z_TYPE_PP(obj) == IS_STRING) {
						if (callable_name) {
							char *ptr;

							*callable_name_len = Z_STRLEN_PP(obj) + Z_STRLEN_PP(method) + sizeof("::") - 1;
							ptr = *callable_name = emalloc(*callable_name_len + 1);
							memcpy(ptr, Z_STRVAL_PP(obj), Z_STRLEN_PP(obj));
							ptr += Z_STRLEN_PP(obj);
							memcpy(ptr, "::", sizeof("::") - 1);
							ptr += sizeof("::") - 1;
							memcpy(ptr, Z_STRVAL_PP(method), Z_STRLEN_PP(method) + 1);
						}

						if (check_flags & IS_CALLABLE_CHECK_SYNTAX_ONLY) {
							return 1;
						}

						if (!zend_is_callable_check_class(Z_STRVAL_PP(obj), Z_STRLEN_PP(obj), fcc, &strict_class, error TSRMLS_CC)) {
							return 0;
						}

					} else {
						if (!EG(objects_store).object_buckets || 
						    !EG(objects_store).object_buckets[Z_OBJ_HANDLE_PP(obj)].valid) {
							return 0;
						}

						fcc->calling_scope = Z_OBJCE_PP(obj); /* TBFixed: what if it's overloaded? */

						fcc->object_ptr = *obj;

						if (callable_name) {
							char *ptr;

							*callable_name_len = fcc->calling_scope->name_length + Z_STRLEN_PP(method) + sizeof("::") - 1;
							ptr = *callable_name = emalloc(*callable_name_len + 1);
							memcpy(ptr, fcc->calling_scope->name, fcc->calling_scope->name_length);
							ptr += fcc->calling_scope->name_length;
							memcpy(ptr, "::", sizeof("::") - 1);
							ptr += sizeof("::") - 1;
							memcpy(ptr, Z_STRVAL_PP(method), Z_STRLEN_PP(method) + 1);
						}

						if (check_flags & IS_CALLABLE_CHECK_SYNTAX_ONLY) {
							fcc->called_scope = fcc->calling_scope;
							return 1;
						}
					}

					ret = zend_is_callable_check_func(check_flags, *method, fcc, strict_class, error TSRMLS_CC);
					if (fcc == &fcc_local &&
					    fcc->function_handler &&
						((fcc->function_handler->type == ZEND_INTERNAL_FUNCTION &&
					      (fcc->function_handler->common.fn_flags & ZEND_ACC_CALL_VIA_HANDLER)) ||
					     fcc->function_handler->type == ZEND_OVERLOADED_FUNCTION_TEMPORARY ||
					     fcc->function_handler->type == ZEND_OVERLOADED_FUNCTION)) {
						if (fcc->function_handler->type != ZEND_OVERLOADED_FUNCTION) {
							efree((char*)fcc->function_handler->common.function_name);
						}
						efree(fcc->function_handler);
					}
					return ret;

				} else {
					if (zend_hash_num_elements(Z_ARRVAL_P(callable)) == 2) {
						if (!obj || (Z_TYPE_PP(obj) != IS_STRING && Z_TYPE_PP(obj) != IS_OBJECT)) {
							if (error) zend_spprintf(error, 0, "first array member is not a valid class name or object");
						} else {
							if (error) zend_spprintf(error, 0, "second array member is not a valid method");
						}
					} else {
						if (error) zend_spprintf(error, 0, "array must have exactly two members");
					}
					if (callable_name) {
						*callable_name = estrndup("Array", sizeof("Array")-1);
						*callable_name_len = sizeof("Array") - 1;
					}
				}
			}
			return 0;

		case IS_OBJECT:
			if (Z_OBJ_HANDLER_P(callable, get_closure) && Z_OBJ_HANDLER_P(callable, get_closure)(callable, &fcc->calling_scope, &fcc->function_handler, &fcc->object_ptr TSRMLS_CC) == SUCCESS) {
				fcc->called_scope = fcc->calling_scope;
				if (callable_name) {
					zend_class_entry *ce = Z_OBJCE_P(callable); /* TBFixed: what if it's overloaded? */

					*callable_name_len = ce->name_length + sizeof("::__invoke") - 1;
					*callable_name = emalloc(*callable_name_len + 1);
					memcpy(*callable_name, ce->name, ce->name_length);
					memcpy((*callable_name) + ce->name_length, "::__invoke", sizeof("::__invoke"));
				}									
				return 1;
			}
			/* break missing intentionally */

		default:
			if (callable_name) {
				zval expr_copy;
				int use_copy;

				zend_make_printable_zval(callable, &expr_copy, &use_copy);
				*callable_name = estrndup(Z_STRVAL(expr_copy), Z_STRLEN(expr_copy));
				*callable_name_len = Z_STRLEN(expr_copy);
				zval_dtor(&expr_copy);
			}
			if (error) zend_spprintf(error, 0, "no array or string given");
			return 0;
	}
}
/* }}} */

ZEND_API zend_bool zend_is_callable(zval *callable, uint check_flags, char **callable_name TSRMLS_DC) /* {{{ */
{
	return zend_is_callable_ex(callable, NULL, check_flags, callable_name, NULL, NULL, NULL TSRMLS_CC);
}
/* }}} */

ZEND_API zend_bool zend_make_callable(zval *callable, char **callable_name TSRMLS_DC) /* {{{ */
{
	zend_fcall_info_cache fcc;

	if (zend_is_callable_ex(callable, NULL, IS_CALLABLE_STRICT, callable_name, NULL, &fcc, NULL TSRMLS_CC)) {
		if (Z_TYPE_P(callable) == IS_STRING && fcc.calling_scope) {
			zval_dtor(callable);
			array_init(callable);
			add_next_index_string(callable, fcc.calling_scope->name, 1);
			add_next_index_string(callable, fcc.function_handler->common.function_name, 1);
		}
		if (fcc.function_handler &&
			((fcc.function_handler->type == ZEND_INTERNAL_FUNCTION &&
		      (fcc.function_handler->common.fn_flags & ZEND_ACC_CALL_VIA_HANDLER)) ||
		     fcc.function_handler->type == ZEND_OVERLOADED_FUNCTION_TEMPORARY ||
		     fcc.function_handler->type == ZEND_OVERLOADED_FUNCTION)) {
			if (fcc.function_handler->type != ZEND_OVERLOADED_FUNCTION) {
				efree((char*)fcc.function_handler->common.function_name);
			}
			efree(fcc.function_handler);
		}
		return 1;
	}
	return 0;
}
/* }}} */

ZEND_API int zend_fcall_info_init(zval *callable, uint check_flags, zend_fcall_info *fci, zend_fcall_info_cache *fcc, char **callable_name, char **error TSRMLS_DC) /* {{{ */
{
	if (!zend_is_callable_ex(callable, NULL, check_flags, callable_name, NULL, fcc, error TSRMLS_CC)) {
		return FAILURE;
	}

	fci->size = sizeof(*fci);
	fci->function_table = fcc->calling_scope ? &fcc->calling_scope->function_table : EG(function_table);
	fci->object_ptr = fcc->object_ptr;
	fci->function_name = callable;
	fci->retval_ptr_ptr = NULL;
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
		if (free_mem) {
			efree(fci->params);
			fci->params = NULL;
		}
	}
	fci->param_count = 0;
}
/* }}} */

ZEND_API void zend_fcall_info_args_save(zend_fcall_info *fci, int *param_count, zval ****params) /* {{{ */
{
	*param_count = fci->param_count;
	*params = fci->params;
	fci->param_count = 0;
	fci->params = NULL;
}
/* }}} */

ZEND_API void zend_fcall_info_args_restore(zend_fcall_info *fci, int param_count, zval ***params) /* {{{ */
{
	zend_fcall_info_args_clear(fci, 1);
	fci->param_count = param_count;
	fci->params = params;
}
/* }}} */

ZEND_API int zend_fcall_info_args(zend_fcall_info *fci, zval *args TSRMLS_DC) /* {{{ */
{
	HashPosition pos;
	zval **arg, ***params;

	zend_fcall_info_args_clear(fci, !args);

	if (!args) {
		return SUCCESS;
	}

	if (Z_TYPE_P(args) != IS_ARRAY) {
		return FAILURE;
	}

	fci->param_count = zend_hash_num_elements(Z_ARRVAL_P(args));
	fci->params = params = (zval ***) erealloc(fci->params, fci->param_count * sizeof(zval **));

	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(args), &pos);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(args), (void *) &arg, &pos) == SUCCESS) {
		*params++ = arg;
		zend_hash_move_forward_ex(Z_ARRVAL_P(args), &pos);
	}

	return SUCCESS;
}
/* }}} */

ZEND_API int zend_fcall_info_argp(zend_fcall_info *fci TSRMLS_DC, int argc, zval ***argv) /* {{{ */
{
	int i;

	if (argc < 0) {
		return FAILURE;
	}

	zend_fcall_info_args_clear(fci, !argc);

	if (argc) {
		fci->param_count = argc;
		fci->params = (zval ***) erealloc(fci->params, fci->param_count * sizeof(zval **));

		for (i = 0; i < argc; ++i) {
			fci->params[i] = argv[i];
		}
	}

	return SUCCESS;
}
/* }}} */

ZEND_API int zend_fcall_info_argv(zend_fcall_info *fci TSRMLS_DC, int argc, va_list *argv) /* {{{ */
{
	int i;
	zval **arg;

	if (argc < 0) {
		return FAILURE;
	}

	zend_fcall_info_args_clear(fci, !argc);

	if (argc) {
		fci->param_count = argc;
		fci->params = (zval ***) erealloc(fci->params, fci->param_count * sizeof(zval **));

		for (i = 0; i < argc; ++i) {
			arg = va_arg(*argv, zval **);
			fci->params[i] = arg;
		}
	}

	return SUCCESS;
}
/* }}} */

ZEND_API int zend_fcall_info_argn(zend_fcall_info *fci TSRMLS_DC, int argc, ...) /* {{{ */
{
	int ret;
	va_list argv;

	va_start(argv, argc);
	ret = zend_fcall_info_argv(fci TSRMLS_CC, argc, &argv);
	va_end(argv);

	return ret;
}
/* }}} */

ZEND_API int zend_fcall_info_call(zend_fcall_info *fci, zend_fcall_info_cache *fcc, zval **retval_ptr_ptr, zval *args TSRMLS_DC) /* {{{ */
{
	zval *retval, ***org_params = NULL;
	int result, org_count = 0;

	fci->retval_ptr_ptr = retval_ptr_ptr ? retval_ptr_ptr : &retval;
	if (args) {
		zend_fcall_info_args_save(fci, &org_count, &org_params);
		zend_fcall_info_args(fci, args TSRMLS_CC);
	}
	result = zend_call_function(fci, fcc TSRMLS_CC);

	if (!retval_ptr_ptr && retval) {
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
	char *lname;
	int name_len = strlen(module_name);
	zend_module_entry *module;

	lname = zend_str_tolower_dup(module_name, name_len);
	if (zend_hash_find(&module_registry, lname, name_len + 1, (void**)&module) == FAILURE) {
		efree(lname);
		return NULL;
	}
	efree(lname);
	return module->version;
}
/* }}} */

ZEND_API int zend_declare_property_ex(zend_class_entry *ce, const char *name, int name_length, zval *property, int access_type, const char *doc_comment, int doc_comment_len TSRMLS_DC) /* {{{ */
{
	zend_property_info property_info, *property_info_ptr;
	const char *interned_name;
	ulong h = zend_get_hash_value(name, name_length+1);

	if (!(access_type & ZEND_ACC_PPP_MASK)) {
		access_type |= ZEND_ACC_PUBLIC;
	}
	if (access_type & ZEND_ACC_STATIC) {
		if (zend_hash_quick_find(&ce->properties_info, name, name_length + 1, h, (void**)&property_info_ptr) == SUCCESS &&
		    (property_info_ptr->flags & ZEND_ACC_STATIC) != 0) {
			property_info.offset = property_info_ptr->offset;
			zval_ptr_dtor(&ce->default_static_members_table[property_info.offset]);
			zend_hash_quick_del(&ce->properties_info, name, name_length + 1, h);
		} else {
			property_info.offset = ce->default_static_members_count++;
			ce->default_static_members_table = perealloc(ce->default_static_members_table, sizeof(zval*) * ce->default_static_members_count, ce->type == ZEND_INTERNAL_CLASS);
		}
		ce->default_static_members_table[property_info.offset] = property;
		if (ce->type == ZEND_USER_CLASS) {
			ce->static_members_table = ce->default_static_members_table;
		}
	} else {
		if (zend_hash_quick_find(&ce->properties_info, name, name_length + 1, h, (void**)&property_info_ptr) == SUCCESS &&
		    (property_info_ptr->flags & ZEND_ACC_STATIC) == 0) {
			property_info.offset = property_info_ptr->offset;
			zval_ptr_dtor(&ce->default_properties_table[property_info.offset]);
			zend_hash_quick_del(&ce->properties_info, name, name_length + 1, h);
		} else {
			property_info.offset = ce->default_properties_count++;
			ce->default_properties_table = perealloc(ce->default_properties_table, sizeof(zval*) * ce->default_properties_count, ce->type == ZEND_INTERNAL_CLASS);
		}
		ce->default_properties_table[property_info.offset] = property;
	}
	if (ce->type & ZEND_INTERNAL_CLASS) {
		switch(Z_TYPE_P(property)) {
			case IS_ARRAY:
			case IS_CONSTANT_ARRAY:
			case IS_OBJECT:
			case IS_RESOURCE:
				zend_error(E_CORE_ERROR, "Internal zval's can't be arrays, objects or resources");
				break;
			default:
				break;
		}
	}
	switch (access_type & ZEND_ACC_PPP_MASK) {
		case ZEND_ACC_PRIVATE: {
				char *priv_name;
				int priv_name_length;

				zend_mangle_property_name(&priv_name, &priv_name_length, ce->name, ce->name_length, name, name_length, ce->type & ZEND_INTERNAL_CLASS);
				property_info.name = priv_name;
				property_info.name_length = priv_name_length;
			}
			break;
		case ZEND_ACC_PROTECTED: {
				char *prot_name;
				int prot_name_length;

				zend_mangle_property_name(&prot_name, &prot_name_length, "*", 1, name, name_length, ce->type & ZEND_INTERNAL_CLASS);
				property_info.name = prot_name;
				property_info.name_length = prot_name_length;
			}
			break;
		case ZEND_ACC_PUBLIC:
			if (IS_INTERNED(name)) {
				property_info.name = (char*)name;
			} else {
				property_info.name = ce->type & ZEND_INTERNAL_CLASS ? zend_strndup(name, name_length) : estrndup(name, name_length);
			}
			property_info.name_length = name_length;
			break;
	}

	interned_name = zend_new_interned_string(property_info.name, property_info.name_length+1, 0 TSRMLS_CC);
	if (interned_name != property_info.name) {
		if (ce->type == ZEND_USER_CLASS) {
			efree((char*)property_info.name);
		} else {
			free((char*)property_info.name);
		}
		property_info.name = interned_name;
	}

	property_info.flags = access_type;
	property_info.h = (access_type & ZEND_ACC_PUBLIC) ? h : zend_get_hash_value(property_info.name, property_info.name_length+1);

	property_info.doc_comment = doc_comment;
	property_info.doc_comment_len = doc_comment_len;

	property_info.ce = ce;

	zend_hash_quick_update(&ce->properties_info, name, name_length+1, h, &property_info, sizeof(zend_property_info), NULL);

	return SUCCESS;
}
/* }}} */

ZEND_API int zend_declare_property(zend_class_entry *ce, const char *name, int name_length, zval *property, int access_type TSRMLS_DC) /* {{{ */
{
	return zend_declare_property_ex(ce, name, name_length, property, access_type, NULL, 0 TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_declare_property_null(zend_class_entry *ce, const char *name, int name_length, int access_type TSRMLS_DC) /* {{{ */
{
	zval *property;

	if (ce->type & ZEND_INTERNAL_CLASS) {
		ALLOC_PERMANENT_ZVAL(property);
	} else {
		ALLOC_ZVAL(property);
	}
	INIT_ZVAL(*property);
	return zend_declare_property(ce, name, name_length, property, access_type TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_declare_property_bool(zend_class_entry *ce, const char *name, int name_length, long value, int access_type TSRMLS_DC) /* {{{ */
{
	zval *property;

	if (ce->type & ZEND_INTERNAL_CLASS) {
		ALLOC_PERMANENT_ZVAL(property);
	} else {
		ALLOC_ZVAL(property);
	}
	INIT_PZVAL(property);
	ZVAL_BOOL(property, value);
	return zend_declare_property(ce, name, name_length, property, access_type TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_declare_property_long(zend_class_entry *ce, const char *name, int name_length, long value, int access_type TSRMLS_DC) /* {{{ */
{
	zval *property;

	if (ce->type & ZEND_INTERNAL_CLASS) {
		ALLOC_PERMANENT_ZVAL(property);
	} else {
		ALLOC_ZVAL(property);
	}
	INIT_PZVAL(property);
	ZVAL_LONG(property, value);
	return zend_declare_property(ce, name, name_length, property, access_type TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_declare_property_double(zend_class_entry *ce, const char *name, int name_length, double value, int access_type TSRMLS_DC) /* {{{ */
{
	zval *property;

	if (ce->type & ZEND_INTERNAL_CLASS) {
		ALLOC_PERMANENT_ZVAL(property);
	} else {
		ALLOC_ZVAL(property);
	}
	INIT_PZVAL(property);
	ZVAL_DOUBLE(property, value);
	return zend_declare_property(ce, name, name_length, property, access_type TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_declare_property_string(zend_class_entry *ce, const char *name, int name_length, const char *value, int access_type TSRMLS_DC) /* {{{ */
{
	zval *property;
	int len = strlen(value);

	if (ce->type & ZEND_INTERNAL_CLASS) {
		ALLOC_PERMANENT_ZVAL(property);
		ZVAL_STRINGL(property, zend_strndup(value, len), len, 0);
	} else {
		ALLOC_ZVAL(property);
		ZVAL_STRINGL(property, value, len, 1);
	}
	INIT_PZVAL(property);
	return zend_declare_property(ce, name, name_length, property, access_type TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_declare_property_stringl(zend_class_entry *ce, const char *name, int name_length, const char *value, int value_len, int access_type TSRMLS_DC) /* {{{ */
{
	zval *property;

	if (ce->type & ZEND_INTERNAL_CLASS) {
		ALLOC_PERMANENT_ZVAL(property);
		ZVAL_STRINGL(property, zend_strndup(value, value_len), value_len, 0);
	} else {
		ALLOC_ZVAL(property);
		ZVAL_STRINGL(property, value, value_len, 1);
	}
	INIT_PZVAL(property);
	return zend_declare_property(ce, name, name_length, property, access_type TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_declare_class_constant(zend_class_entry *ce, const char *name, size_t name_length, zval *value TSRMLS_DC) /* {{{ */
{
	return zend_hash_update(&ce->constants_table, name, name_length+1, &value, sizeof(zval *), NULL);
}
/* }}} */

ZEND_API int zend_declare_class_constant_null(zend_class_entry *ce, const char *name, size_t name_length TSRMLS_DC) /* {{{ */
{
	zval *constant;

	if (ce->type & ZEND_INTERNAL_CLASS) {
		ALLOC_PERMANENT_ZVAL(constant);
	} else {
		ALLOC_ZVAL(constant);
	}
	ZVAL_NULL(constant);
	INIT_PZVAL(constant);
	return zend_declare_class_constant(ce, name, name_length, constant TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_declare_class_constant_long(zend_class_entry *ce, const char *name, size_t name_length, long value TSRMLS_DC) /* {{{ */
{
	zval *constant;

	if (ce->type & ZEND_INTERNAL_CLASS) {
		ALLOC_PERMANENT_ZVAL(constant);
	} else {
		ALLOC_ZVAL(constant);
	}
	ZVAL_LONG(constant, value);
	INIT_PZVAL(constant);
	return zend_declare_class_constant(ce, name, name_length, constant TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_declare_class_constant_bool(zend_class_entry *ce, const char *name, size_t name_length, zend_bool value TSRMLS_DC) /* {{{ */
{
	zval *constant;

	if (ce->type & ZEND_INTERNAL_CLASS) {
		ALLOC_PERMANENT_ZVAL(constant);
	} else {
		ALLOC_ZVAL(constant);
	}
	ZVAL_BOOL(constant, value);
	INIT_PZVAL(constant);
	return zend_declare_class_constant(ce, name, name_length, constant TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_declare_class_constant_double(zend_class_entry *ce, const char *name, size_t name_length, double value TSRMLS_DC) /* {{{ */
{
	zval *constant;

	if (ce->type & ZEND_INTERNAL_CLASS) {
		ALLOC_PERMANENT_ZVAL(constant);
	} else {
		ALLOC_ZVAL(constant);
	}
	ZVAL_DOUBLE(constant, value);
	INIT_PZVAL(constant);
	return zend_declare_class_constant(ce, name, name_length, constant TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_declare_class_constant_stringl(zend_class_entry *ce, const char *name, size_t name_length, const char *value, size_t value_length TSRMLS_DC) /* {{{ */
{
	zval *constant;

	if (ce->type & ZEND_INTERNAL_CLASS) {
		ALLOC_PERMANENT_ZVAL(constant);
		ZVAL_STRINGL(constant, zend_strndup(value, value_length), value_length, 0);
	} else {
		ALLOC_ZVAL(constant);
		ZVAL_STRINGL(constant, value, value_length, 1);
	}
	INIT_PZVAL(constant);
	return zend_declare_class_constant(ce, name, name_length, constant TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_declare_class_constant_string(zend_class_entry *ce, const char *name, size_t name_length, const char *value TSRMLS_DC) /* {{{ */
{
	return zend_declare_class_constant_stringl(ce, name, name_length, value, strlen(value) TSRMLS_CC);
}
/* }}} */

ZEND_API void zend_update_property(zend_class_entry *scope, zval *object, const char *name, int name_length, zval *value TSRMLS_DC) /* {{{ */
{
	zval *property;
	zend_class_entry *old_scope = EG(scope);

	EG(scope) = scope;

	if (!Z_OBJ_HT_P(object)->write_property) {
		const char *class_name;
		zend_uint class_name_len;

		zend_get_object_classname(object, &class_name, &class_name_len TSRMLS_CC);

		zend_error(E_CORE_ERROR, "Property %s of class %s cannot be updated", name, class_name);
	}
	MAKE_STD_ZVAL(property);
	ZVAL_STRINGL(property, name, name_length, 1);
	Z_OBJ_HT_P(object)->write_property(object, property, value, 0 TSRMLS_CC);
	zval_ptr_dtor(&property);

	EG(scope) = old_scope;
}
/* }}} */

ZEND_API void zend_update_property_null(zend_class_entry *scope, zval *object, const char *name, int name_length TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_NULL(tmp);
	zend_update_property(scope, object, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API void zend_update_property_bool(zend_class_entry *scope, zval *object, const char *name, int name_length, long value TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_BOOL(tmp, value);
	zend_update_property(scope, object, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API void zend_update_property_long(zend_class_entry *scope, zval *object, const char *name, int name_length, long value TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_LONG(tmp, value);
	zend_update_property(scope, object, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API void zend_update_property_double(zend_class_entry *scope, zval *object, const char *name, int name_length, double value TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_DOUBLE(tmp, value);
	zend_update_property(scope, object, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API void zend_update_property_string(zend_class_entry *scope, zval *object, const char *name, int name_length, const char *value TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_STRING(tmp, value, 1);
	zend_update_property(scope, object, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API void zend_update_property_stringl(zend_class_entry *scope, zval *object, const char *name, int name_length, const char *value, int value_len TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_STRINGL(tmp, value, value_len, 1);
	zend_update_property(scope, object, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_update_static_property(zend_class_entry *scope, const char *name, int name_length, zval *value TSRMLS_DC) /* {{{ */
{
	zval **property;
	zend_class_entry *old_scope = EG(scope);

	EG(scope) = scope;
	property = zend_std_get_static_property(scope, name, name_length, 0, NULL TSRMLS_CC);
	EG(scope) = old_scope;
	if (!property) {
		return FAILURE;
	} else {
		if (*property != value) {
			if (PZVAL_IS_REF(*property)) {
				zval_dtor(*property);
				Z_TYPE_PP(property) = Z_TYPE_P(value);
				(*property)->value = value->value;
				if (Z_REFCOUNT_P(value) > 0) {
					zval_copy_ctor(*property);
				}
			} else {
				zval *garbage = *property;

				Z_ADDREF_P(value);
				if (PZVAL_IS_REF(value)) {
					SEPARATE_ZVAL(&value);
				}
				*property = value;
				zval_ptr_dtor(&garbage);
			}
		}
		return SUCCESS;
	}
}
/* }}} */

ZEND_API int zend_update_static_property_null(zend_class_entry *scope, const char *name, int name_length TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_NULL(tmp);
	return zend_update_static_property(scope, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_update_static_property_bool(zend_class_entry *scope, const char *name, int name_length, long value TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_BOOL(tmp, value);
	return zend_update_static_property(scope, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_update_static_property_long(zend_class_entry *scope, const char *name, int name_length, long value TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_LONG(tmp, value);
	return zend_update_static_property(scope, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_update_static_property_double(zend_class_entry *scope, const char *name, int name_length, double value TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_DOUBLE(tmp, value);
	return zend_update_static_property(scope, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_update_static_property_string(zend_class_entry *scope, const char *name, int name_length, const char *value TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_STRING(tmp, value, 1);
	return zend_update_static_property(scope, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_update_static_property_stringl(zend_class_entry *scope, const char *name, int name_length, const char *value, int value_len TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_STRINGL(tmp, value, value_len, 1);
	return zend_update_static_property(scope, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API zval *zend_read_property(zend_class_entry *scope, zval *object, const char *name, int name_length, zend_bool silent TSRMLS_DC) /* {{{ */
{
	zval *property, *value;
	zend_class_entry *old_scope = EG(scope);

	EG(scope) = scope;

	if (!Z_OBJ_HT_P(object)->read_property) {
		const char *class_name;
		zend_uint class_name_len;

		zend_get_object_classname(object, &class_name, &class_name_len TSRMLS_CC);
		zend_error(E_CORE_ERROR, "Property %s of class %s cannot be read", name, class_name);
	}

	MAKE_STD_ZVAL(property);
	ZVAL_STRINGL(property, name, name_length, 1);
	value = Z_OBJ_HT_P(object)->read_property(object, property, silent?BP_VAR_IS:BP_VAR_R, 0 TSRMLS_CC);
	zval_ptr_dtor(&property);

	EG(scope) = old_scope;
	return value;
}
/* }}} */

ZEND_API zval *zend_read_static_property(zend_class_entry *scope, const char *name, int name_length, zend_bool silent TSRMLS_DC) /* {{{ */
{
	zval **property;
	zend_class_entry *old_scope = EG(scope);

	EG(scope) = scope;
	property = zend_std_get_static_property(scope, name, name_length, silent, NULL TSRMLS_CC);
	EG(scope) = old_scope;

	return property?*property:NULL;
}
/* }}} */

ZEND_API void zend_save_error_handling(zend_error_handling *current TSRMLS_DC) /* {{{ */
{
	current->handling = EG(error_handling);
	current->exception = EG(exception_class);
	current->user_handler = EG(user_error_handler);
	if (current->user_handler) {
		Z_ADDREF_P(current->user_handler);
	}
}
/* }}} */

ZEND_API void zend_replace_error_handling(zend_error_handling_t error_handling, zend_class_entry *exception_class, zend_error_handling *current TSRMLS_DC) /* {{{ */
{
	if (current) {
		zend_save_error_handling(current TSRMLS_CC);
		if (error_handling != EH_NORMAL && EG(user_error_handler)) {
			zval_ptr_dtor(&EG(user_error_handler));
			EG(user_error_handler) = NULL;
		}
	}
	EG(error_handling) = error_handling;
	EG(exception_class) = error_handling == EH_THROW ? exception_class : NULL;
}
/* }}} */

ZEND_API void zend_restore_error_handling(zend_error_handling *saved TSRMLS_DC) /* {{{ */
{
	EG(error_handling) = saved->handling;
	EG(exception_class) = saved->handling == EH_THROW ? saved->exception : NULL;
	if (saved->user_handler	&& saved->user_handler != EG(user_error_handler)) {
		if (EG(user_error_handler)) {
			zval_ptr_dtor(&EG(user_error_handler));
		}
		EG(user_error_handler) = saved->user_handler;
	} else if (saved->user_handler) {
		zval_ptr_dtor(&saved->user_handler);
	}
	saved->user_handler = NULL;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
