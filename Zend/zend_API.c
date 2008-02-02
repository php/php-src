/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2008 Zend Technologies Ltd. (http://www.zend.com) |
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

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

/* these variables are true statics/globals, and have to be mutex'ed on every access */
static int module_count=0;
ZEND_API HashTable module_registry;

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
	char *space;
	zstr class_name = get_active_class_name(&space TSRMLS_CC);

	zend_error(E_WARNING, "Wrong parameter count for %v%s%v()", class_name, space, get_active_function_name(TSRMLS_C));
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
		{
			TSRMLS_FETCH();

			if (UG(unicode)) {
				return "binary string";
			} else {
				return "string";
			}
		}
		case IS_OBJECT:
			return "object";
		case IS_RESOURCE:
			return "resource";
		case IS_NULL:
			return "null";
		case IS_ARRAY:
			return "array";
		case IS_UNICODE:
			return "Unicode string";
		default:
			return "unknown";
	}
}
/* }}} */

ZEND_API char *zend_zval_type_name(zval *arg) /* {{{ */
{
	return zend_get_type_by_const(Z_TYPE_P(arg));
}
/* }}} */

ZEND_API zend_class_entry *zend_get_class_entry(zval *zobject TSRMLS_DC) /* {{{ */
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
ZEND_API int zend_get_object_classname(zval *object, zstr *class_name, zend_uint *class_name_len TSRMLS_DC) /* {{{ */
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

#define RETURN_AS_STRING(arg, p, pl, type) \
	(*p).s = Z_STRVAL_PP(arg); \
	*pl = Z_STRLEN_PP(arg); \
	*type = IS_STRING;

#define RETURN_AS_UNICODE(arg, p, pl, type) \
	(*p).u = Z_USTRVAL_PP(arg); \
	*pl = Z_USTRLEN_PP(arg); \
	*type = IS_UNICODE;

static int parse_arg_object_to_string(zval **arg, char **p, int *pl, int type TSRMLS_DC) /* {{{ */
{
	if (Z_OBJ_HANDLER_PP(arg, cast_object)) {
		SEPARATE_ZVAL_IF_NOT_REF(arg);
		if (Z_OBJ_HANDLER_PP(arg, cast_object)(*arg, *arg, type, NULL TSRMLS_CC) == SUCCESS) {
			*pl = Z_STRLEN_PP(arg);
			*p = Z_STRVAL_PP(arg);
			return SUCCESS;
		}
	}
	/* Standard PHP objects */
	if (Z_OBJ_HT_PP(arg) == &std_object_handlers || !Z_OBJ_HANDLER_PP(arg, cast_object)) {
		SEPARATE_ZVAL_IF_NOT_REF(arg);
		if (zend_std_cast_object_tostring(*arg, *arg, type, NULL TSRMLS_CC) == SUCCESS) {
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
			if (type == IS_STRING) {
				zend_make_string_zval(z, *arg, &use_copy);
			} else {
				zend_make_unicode_zval(z, *arg, &use_copy);
			}
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

static char *zend_parse_arg_impl(int arg_num, zval **arg, va_list *va, char **spec, char T_arg_type, int* ret_type, char **error, int *severity TSRMLS_DC) /* {{{ */
{
	char *spec_walk = *spec;
	char c = *spec_walk++;
	int return_null = 0;
	int alternate_form = 0;
	int return_orig_type = 0;
	zend_uchar orig_type;

	/* scan through modifiers */
	while (1) {
		if (*spec_walk == '/') {
			SEPARATE_ZVAL_IF_NOT_REF(arg);
		} else if (*spec_walk == '&') {
			alternate_form = 1;
		} else if (*spec_walk == '!') {
			if (Z_TYPE_PP(arg) == IS_NULL) {
				return_null = 1;
			}
		} else if (*spec_walk == '^') {
			return_orig_type = 1;
			orig_type = Z_TYPE_PP(arg);
		} else {
			break;
		}
		spec_walk++;
	}

	if (c == 'x') {
		c = UG(unicode) ? 'u' : 's';
	}

	switch (c) {
		case 'l':
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
								*p = (long) d;
							}
						}
						break;

					case IS_UNICODE:
						{
							double d;
							int type;

							if ((type = is_numeric_unicode(Z_USTRVAL_PP(arg), Z_USTRLEN_PP(arg), p, &d, -1)) == 0) {
								return "long";
							} else if (type == IS_DOUBLE) {
								*p = (long) d;
							}
						}
						break;

					case IS_NULL:
					case IS_LONG:
					case IS_DOUBLE:
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

					case IS_UNICODE:
						{
							long l;
							int type;

							if ((type = is_numeric_unicode(Z_USTRVAL_PP(arg), Z_USTRLEN_PP(arg), &l, p, -1)) == 0) {
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

		case 's':
		case 'S':
			{
				char **p = va_arg(*va, char **);
				int *pl = va_arg(*va, int *);
				UConverter *conv = NULL;

				if (c == 's' && alternate_form) {
					conv = va_arg(*va, UConverter *);
				}

				if (return_orig_type) {
					zend_uchar *type = va_arg(*va, zend_uchar *);
					*type = orig_type;
				}

				switch (Z_TYPE_PP(arg)) {
					case IS_NULL:
						if (return_null) {
							*p = NULL;
							*pl = 0;
							break;
						}
						/* break omitted intentionally */

					case IS_UNICODE:
						/* handle conversion of Unicode to binary with a specific converter */
						if (conv != NULL) {
							SEPARATE_ZVAL_IF_NOT_REF(arg);
							if (convert_to_string_with_converter(*arg, conv) == FAILURE) {
								return "";
							}
							*p = Z_STRVAL_PP(arg);
							*pl = Z_STRLEN_PP(arg);
							break;
						} else if (c == 'S') {
							return "strictly a binary string";
						}
						/* fall through */
					case IS_STRING:
					case IS_LONG:
					case IS_DOUBLE:
					case IS_BOOL:
						convert_to_string_ex(arg);
						*p = Z_STRVAL_PP(arg);
						*pl = Z_STRLEN_PP(arg);
						break;

					case IS_OBJECT:
						if (parse_arg_object_to_string(arg, p, pl, IS_STRING TSRMLS_CC) == SUCCESS) {
							break;
						}

					case IS_ARRAY:
					case IS_RESOURCE:
					default:
						if (UG(unicode)) {
							return "binary string";
						} else {
							return "string";
						}
				}
			}
			break;

		case 'u':
		case 'U':
			{
				UChar **p = va_arg(*va, UChar **);
				int *pl = va_arg(*va, int *);

				if (return_orig_type) {
					zend_uchar *type = va_arg(*va, zend_uchar *);
					*type = orig_type;
				}

				switch (Z_TYPE_PP(arg)) {
					case IS_NULL:
						if (return_null) {
							*p = NULL;
							*pl = 0;
							break;
						}
						/* break omitted intentionally */

					case IS_STRING:
						if (c == 'U') {
							return "strictly a Unicode string";
						}
						/* fall through */
					case IS_LONG:
					case IS_DOUBLE:
					case IS_BOOL:
					case IS_UNICODE:
						convert_to_unicode_ex(arg);
						*p = Z_USTRVAL_PP(arg);
						*pl = Z_USTRLEN_PP(arg);
						break;

					case IS_OBJECT:
						if (parse_arg_object_to_string(arg, (char**)p, pl, IS_UNICODE TSRMLS_CC) == SUCCESS) {
							break;
						}

					case IS_ARRAY:
					case IS_RESOURCE:
					default:
						return "Unicode string";
				}
			}
			break;

		case 'T':
			if (T_arg_type != -1)
			{
				zstr *p = va_arg(*va, zstr *);
				int *pl = va_arg(*va, int *);
				zend_uchar *type = va_arg(*va, zend_uchar *);
				switch (Z_TYPE_PP(arg)) {
					case IS_NULL:
						if (return_null) {
							*p = NULL_ZSTR;
							*pl = 0;
							*type = T_arg_type;
							break;
						}
						/* break omitted intentionally */

					case IS_LONG:
					case IS_DOUBLE:
					case IS_BOOL:
					case IS_STRING:
					case IS_UNICODE:
						if (T_arg_type == IS_UNICODE) {
							convert_to_unicode_ex(arg);
							RETURN_AS_UNICODE(arg, p, pl, type);
						} else if (T_arg_type == IS_STRING) {
							convert_to_string_ex(arg);
							RETURN_AS_STRING(arg, p, pl, type);
						}
						break;

					case IS_OBJECT:
						if (parse_arg_object_to_string(arg, (char**)p, pl, T_arg_type TSRMLS_CC) == SUCCESS) {
							*type = Z_TYPE_PP(arg);
							break;
						}

					case IS_ARRAY:
					case IS_RESOURCE:
					default:
						return "string (Unicode or binary)";
				}

				break;
			}
			/* break omitted intentionally */

		case 't':
			{
				zstr *p = va_arg(*va, zstr *);
				int *pl = va_arg(*va, int *);
				zend_uchar *type = va_arg(*va, zend_uchar *);
				switch (Z_TYPE_PP(arg)) {
					case IS_NULL:
						if (return_null) {
							*p = NULL_ZSTR;
							*pl = 0;
							*type = UG(unicode)?IS_UNICODE:IS_STRING;
							break;
						}
						/* break omitted intentionally */

					case IS_LONG:
					case IS_DOUBLE:
					case IS_BOOL:
						if (UG(unicode)) {
							convert_to_unicode_ex(arg);
							RETURN_AS_UNICODE(arg, p, pl, type);
						} else {
							convert_to_string_ex(arg);
							RETURN_AS_STRING(arg, p, pl, type);
						}
						break;

					case IS_STRING:
						RETURN_AS_STRING(arg, p, pl, type);
						break;

					case IS_UNICODE:
						RETURN_AS_UNICODE(arg, p, pl, type);
						break;

					case IS_OBJECT:
						if (parse_arg_object_to_string(arg, (char**)p, pl, UG(unicode) ? IS_UNICODE : IS_STRING TSRMLS_CC) == SUCCESS) {
							*type = UG(unicode)?IS_UNICODE:IS_STRING;
							break;
						}

					case IS_ARRAY:
					case IS_RESOURCE:
					default:
						return "string (Unicode or binary)";
				}
			}
			break;

		case 'b':
			{
				zend_bool *p = va_arg(*va, zend_bool *);
				switch (Z_TYPE_PP(arg)) {
					case IS_NULL:
					case IS_STRING:
					case IS_UNICODE:
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

		case 'a':
			{
				zval **p = va_arg(*va, zval **);
				if (return_null) {
					*p = NULL;
					break;
				}
				if (Z_TYPE_PP(arg) == IS_ARRAY) {
					*p = *arg;
				} else {
					return "array";
				}
			}
			break;

		case 'h':
			{
				HashTable **p = va_arg(*va, HashTable **);
				if (return_null) {
					*p = NULL;
					break;
				}
				if (Z_TYPE_PP(arg) == IS_ARRAY) {
					*p = Z_ARRVAL_PP(arg);
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
						*ret_type = UG(unicode)?IS_UNICODE:IS_STRING;
						return ce->name.v;
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
				convert_to_text_ex(arg);
				if (zend_u_lookup_class(Z_TYPE_PP(arg), Z_UNIVAL_PP(arg), Z_UNILEN_PP(arg), &lookup TSRMLS_CC) == FAILURE) {
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

static int zend_parse_arg(int arg_num, zval **arg, va_list *va, char **spec, int quiet, char T_arg_type TSRMLS_DC) /* {{{ */
{
	char *expected_type = NULL, *error = NULL;
	int ret_type = IS_STRING;
	int severity = E_WARNING;

	expected_type = zend_parse_arg_impl(arg_num, arg, va, spec, T_arg_type, &ret_type, &error, &severity TSRMLS_CC);
	if (expected_type) {
		if (!quiet && (*expected_type || error)) {
			char *space;
			zstr class_name = get_active_class_name(&space TSRMLS_CC);

			if (error) {
				zend_error(severity, "%v%s%v() expects parameter %d %s",
						class_name, space, get_active_function_name(TSRMLS_C), arg_num, error);
				efree(error);
			} else {
				zend_error(severity, "%v%s%v() expects parameter %d to be %R, %s given",
						class_name, space, get_active_function_name(TSRMLS_C), arg_num, ret_type, expected_type,
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

static int zend_parse_va_args(int num_args, char *type_spec, va_list *va, int flags TSRMLS_DC) /* {{{ */
{
	char *spec_walk;
	int c, i;
	int min_num_args = -1;
	int max_num_args = 0;
	int post_varargs = 0;
	zval **arg;
	int arg_count;
	int quiet = flags & ZEND_PARSE_PARAMS_QUIET;
	zend_bool have_varargs = 0;
	zend_bool T_present = 0;
	char T_arg_type = -1;
	zval ****varargs = NULL;
	int *n_varargs = NULL;

	for (spec_walk = type_spec; *spec_walk; spec_walk++) {
		c = *spec_walk;
		switch (c) {
			case 'T':
				T_present++;
				/* break omitted intentionally */
			case 'l': case 'd':
			case 's': case 'b':
			case 'r': case 'a':
			case 'o': case 'O':
			case 'z': case 'Z':
			case 't': case 'u':
			case 'C': case 'h':
			case 'U': case 'S':
			case 'f': case 'x':
				max_num_args++;
				break;

			case '|':
				min_num_args = max_num_args;
				break;

			case '/': case '!':
			case '&': case '^':
				/* Pass */
				break;

			case '*':
			case '+':
				if (have_varargs) {
					if (!quiet) {
						char *space;
						zstr class_name = get_active_class_name(&space TSRMLS_CC);
						zend_error(E_WARNING, "%v%s%v(): only one varargs specifier (* or +) is permitted",
							class_name, space, get_active_function_name(TSRMLS_C));
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
					char *space;
					zstr class_name = get_active_class_name(&space TSRMLS_CC);
					zend_error(E_WARNING, "%v%s%v(): bad type specifier while parsing parameters",
							class_name, space, get_active_function_name(TSRMLS_C));
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
			char *space;
			zstr class_name = get_active_class_name(&space TSRMLS_CC);
			zend_error(E_WARNING, "%v%s%v() expects %s %d parameter%s, %d given",
					class_name, space,
					get_active_function_name(TSRMLS_C),
					min_num_args == max_num_args ? "exactly" : num_args < min_num_args ? "at least" : "at most",
					num_args < min_num_args ? min_num_args : max_num_args,
					(num_args < min_num_args ? min_num_args : max_num_args) == 1 ? "" : "s",
					num_args);
		}
		return FAILURE;
	}

	arg_count = (int)(zend_uintptr_t) *(zend_vm_stack_top(TSRMLS_C) - 1);

	if (num_args > arg_count) {
		zend_error(E_WARNING, "%v(): could not obtain parameters for parsing",
			get_active_function_name(TSRMLS_C));
		return FAILURE;
	}

	if (T_present > 1) {
		/* determine 'T' target argument type */
		for (spec_walk = type_spec, i = 0; *spec_walk && i < num_args; spec_walk++) {
			switch (*spec_walk) {
				case 'T':
					arg = (zval**)zend_vm_stack_top(TSRMLS_C) - 1 - (arg_count-i);
					if (Z_TYPE_PP(arg) == IS_UNICODE && (T_arg_type == -1 || T_arg_type == IS_STRING)) {
						/* we can upgrade from strings to Unicode */
						T_arg_type = IS_UNICODE;
					} else if (Z_TYPE_PP(arg) == IS_STRING && T_arg_type == -1) {
						T_arg_type = IS_STRING;
					}
					i++;
					break;

				case '|': case '!':
				case '/': case '&':
					/* pass */
					break;

				case '*':
				case '+':
					i = arg_count - post_varargs;
					break;

				default:
					i++;
					break;
			}
		}

		if (T_arg_type == -1) {
			T_arg_type = ZEND_STR_TYPE;
		}
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

		if (zend_parse_arg(i+1, arg, va, &type_spec, quiet, T_arg_type TSRMLS_CC) == FAILURE) {
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
		char *__space; \
		zstr __class_name = get_active_class_name(&__space TSRMLS_CC); \
		zend_error(E_WARNING, "%v%s%v() expects exactly 0 parameters, %d given", \
			__class_name, __space, \
			get_active_function_name(TSRMLS_C), __num_args); \
		return FAILURE; \
	}\
}

ZEND_API int zend_parse_parameters_ex(int flags, int num_args TSRMLS_DC, char *type_spec, ...) /* {{{ */
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

ZEND_API int zend_parse_parameters(int num_args TSRMLS_DC, char *type_spec, ...) /* {{{ */
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

ZEND_API int zend_parse_method_parameters(int num_args TSRMLS_DC, zval *this_ptr, char *type_spec, ...) /* {{{ */
{
	va_list va;
	int retval;
	char *p = type_spec;
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
			zend_error(E_CORE_ERROR, "%v::%v() must be derived from %v::%v",
				ce->name, get_active_function_name(TSRMLS_C), Z_OBJCE_P(this_ptr)->name, get_active_function_name(TSRMLS_C));
		}

		retval = zend_parse_va_args(num_args, p, &va, 0 TSRMLS_CC);
		va_end(va);
	}
	return retval;
}
/* }}} */

ZEND_API int zend_parse_method_parameters_ex(int flags, int num_args TSRMLS_DC, zval *this_ptr, char *type_spec, ...) /* {{{ */
{
	va_list va;
	int retval;
	char *p = type_spec;
	zval **object;
	zend_class_entry *ce;
	int quiet = flags & ZEND_PARSE_PARAMS_QUIET;

	if (!this_ptr) {
		RETURN_IF_ZERO_ARGS(num_args, p, quiet);

		va_start(va, type_spec);
		retval = zend_parse_va_args(num_args, type_spec, &va, 0 TSRMLS_CC);
		va_end(va);
	} else {
		p++;
		RETURN_IF_ZERO_ARGS(num_args-1, p, quiet);

		va_start(va, type_spec);

		object = va_arg(va, zval **);
		ce = va_arg(va, zend_class_entry *);
		*object = this_ptr;
		if (ce && !instanceof_function(Z_OBJCE_P(this_ptr), ce TSRMLS_CC)) {
			if (!quiet) {
				zend_error(E_CORE_ERROR, "%v::%v() must be derived from %v::%v",
					ce->name, get_active_function_name(TSRMLS_C), Z_OBJCE_P(this_ptr)->name, get_active_function_name(TSRMLS_C));
			}
			return FAILURE;
		}

		retval = zend_parse_va_args(num_args, p, &va, flags TSRMLS_CC);
		va_end(va);
	}
	return retval;
}
/* }}} */

/* Argument parsing API -- andrei */
ZEND_API int _array_init(zval *arg ZEND_FILE_LINE_DC) /* {{{ */
{
	ALLOC_HASHTABLE_REL(Z_ARRVAL_P(arg));

	zend_u_hash_init(Z_ARRVAL_P(arg), 0, NULL, ZVAL_PTR_DTOR, 0, 0);
	Z_TYPE_P(arg) = IS_ARRAY;
	return SUCCESS;
}
/* }}} */

static int zend_merge_property(zval **value, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */
{
	/* which name should a numeric property have ? */
	if (hash_key->nKeyLength) {
		zval *obj = va_arg(args, zval *);
		zend_object_handlers *obj_ht = va_arg(args, zend_object_handlers *);
		zval *member;
		TSRMLS_FETCH();

		MAKE_STD_ZVAL(member);
		if (hash_key->type == IS_STRING) {
			ZVAL_STRINGL(member, hash_key->arKey.s, hash_key->nKeyLength-1, 1);
		} else if (hash_key->type == IS_UNICODE) {
			ZVAL_UNICODEL(member, hash_key->arKey.u, hash_key->nKeyLength-1, 1);
		}

		obj_ht->write_property(obj, member, *value TSRMLS_CC);
		zval_ptr_dtor(&member);
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

/* This function should be called after the constructor has been called
 * because it may call __set from the uninitialized object otherwise. */
ZEND_API void zend_merge_properties(zval *obj, HashTable *properties, int destroy_ht TSRMLS_DC) /* {{{ */
{
	zend_object_handlers *obj_ht = Z_OBJ_HT_P(obj);
	zend_class_entry *old_scope = EG(scope);

	EG(scope) = Z_OBJCE_P(obj);
	zend_hash_apply_with_arguments(properties, (apply_func_args_t)zend_merge_property, 2, obj, obj_ht);
	EG(scope) = old_scope;

	if (destroy_ht) {
		zend_hash_destroy(properties);
		FREE_HASHTABLE(properties);
	}
}
/* }}} */

ZEND_API void zend_update_class_constants(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
	if (!class_type->constants_updated || !CE_STATIC_MEMBERS(class_type)) {
		zend_class_entry **scope = EG(in_execution)?&EG(scope):&CG(active_class_entry);
		zend_class_entry *old_scope = *scope;

		*scope = class_type;
		zend_hash_apply_with_argument(&class_type->constants_table, (apply_func_arg_t) zval_update_constant, (void*)1 TSRMLS_CC);
		zend_hash_apply_with_argument(&class_type->default_properties, (apply_func_arg_t) zval_update_constant, (void *) 1 TSRMLS_CC);

		if (!CE_STATIC_MEMBERS(class_type)) {
			HashPosition pos;
			zval **p;

			if (class_type->parent) {
				zend_update_class_constants(class_type->parent TSRMLS_CC);
			}
#if ZTS
			ALLOC_HASHTABLE(CG(static_members)[(zend_intptr_t)(class_type->static_members)]);
#else
			ALLOC_HASHTABLE(class_type->static_members);
#endif
			zend_u_hash_init(CE_STATIC_MEMBERS(class_type), zend_hash_num_elements(&class_type->default_static_members), NULL, ZVAL_PTR_DTOR, 0, UG(unicode));

			zend_hash_internal_pointer_reset_ex(&class_type->default_static_members, &pos);
			while (zend_hash_get_current_data_ex(&class_type->default_static_members, (void**)&p, &pos) == SUCCESS) {
				zstr str_index;
				uint str_length;
				ulong num_index;
				zend_uchar utype;
				zval **q;

				switch (zend_hash_get_current_key_ex(&class_type->default_static_members, &str_index, &str_length, &num_index, 0, &pos)) {
					case HASH_KEY_IS_UNICODE:
						utype = IS_UNICODE;
						break;
					case HASH_KEY_IS_STRING:
					default:
						utype = IS_STRING;
						break;
				}
				if (Z_ISREF_PP(p) &&
					class_type->parent &&
					zend_u_hash_find(&class_type->parent->default_static_members, utype, str_index, str_length, (void**)&q) == SUCCESS &&
					*p == *q &&
					zend_u_hash_find(CE_STATIC_MEMBERS(class_type->parent), utype, str_index, str_length, (void**)&q) == SUCCESS
				) {
					Z_ADDREF_PP(q);
					Z_SET_ISREF_PP(q);
					zend_u_hash_add(CE_STATIC_MEMBERS(class_type), utype, str_index, str_length, (void**)q, sizeof(zval*), NULL);
				} else {
					zval *q;

					ALLOC_ZVAL(q);
					*q = **p;
					INIT_PZVAL(q);
					zval_copy_ctor(q);
					zend_u_hash_add(CE_STATIC_MEMBERS(class_type), utype, str_index, str_length, (void**)&q, sizeof(zval*), NULL);
				}
				zend_hash_move_forward_ex(&class_type->default_static_members, &pos);
			}
		}
		zend_hash_apply_with_argument(CE_STATIC_MEMBERS(class_type), (apply_func_arg_t) zval_update_constant, (void *) 1 TSRMLS_CC);

		*scope = old_scope;
		class_type->constants_updated = 1;
	}
}
/* }}} */

/* This function requires 'properties' to contain all props declared in the
 * class and all props being public. If only a subset is given or the class
 * has protected members then you need to merge the properties seperately by
 * calling zend_merge_properties(). */
ZEND_API int _object_and_properties_init(zval *arg, zend_class_entry *class_type, HashTable *properties ZEND_FILE_LINE_DC TSRMLS_DC) /* {{{ */
{
	zval *tmp;
	zend_object *object;

	if (class_type->ce_flags & (ZEND_ACC_INTERFACE|ZEND_ACC_IMPLICIT_ABSTRACT_CLASS|ZEND_ACC_EXPLICIT_ABSTRACT_CLASS)) {
		char *what = class_type->ce_flags & ZEND_ACC_INTERFACE ? "interface" : "abstract class";
		zend_error(E_ERROR, "Cannot instantiate %s %v", what, class_type->name);
	}

	zend_update_class_constants(class_type TSRMLS_CC);

	Z_TYPE_P(arg) = IS_OBJECT;
	if (class_type->create_object == NULL) {
		Z_OBJVAL_P(arg) = zend_objects_new(&object, class_type TSRMLS_CC);
		if (properties) {
			object->properties = properties;
		} else {
			ALLOC_HASHTABLE_REL(object->properties);
			zend_u_hash_init(object->properties, zend_hash_num_elements(&class_type->default_properties), NULL, ZVAL_PTR_DTOR, 0, UG(unicode));
			zend_hash_copy(object->properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
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

ZEND_API int add_assoc_zval_ex(zval *arg, const char *key, uint key_len, zval *value) /* {{{ */
{
	return zend_symtable_update(Z_ARRVAL_P(arg), key, key_len, (void *) &value, sizeof(zval *), NULL);
}
/* }}} */

ZEND_API int add_ascii_assoc_zval_ex(zval *arg, const char *key, uint key_len, zval *value) /* {{{ */
{
	return zend_ascii_symtable_update(Z_ARRVAL_P(arg), key, key_len, (void*)&value, sizeof(zval*), NULL);
}
/* }}} */

ZEND_API int add_rt_assoc_zval_ex(zval *arg, const char *key, uint key_len, zval *value) /* {{{ */
{
	return zend_rt_symtable_update(Z_ARRVAL_P(arg), key, key_len, (void*)&value, sizeof(zval*), NULL);
}
/* }}} */

ZEND_API int add_utf8_assoc_zval_ex(zval *arg, const char *key, uint key_len, zval *value) /* {{{ */
{
	return zend_utf8_symtable_update(Z_ARRVAL_P(arg), key, key_len, (void*)&value, sizeof(zval*), NULL);
}
/* }}} */

ZEND_API int add_u_assoc_zval_ex(zval *arg, zend_uchar type, zstr key, uint key_len, zval *value) /* {{{ */
{
	return zend_u_symtable_update(Z_ARRVAL_P(arg), type, key, key_len, (void *) &value, sizeof(zval *), NULL);
}
/* }}} */

ZEND_API int add_index_zval(zval *arg, ulong index, zval *value) /* {{{ */
{
	return zend_hash_index_update(Z_ARRVAL_P(arg), index, (void *) &value, sizeof(zval *), NULL);
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

ZEND_API int add_get_index_unicode(zval *arg, ulong index, UChar *str, void **dest, int duplicate) /* {{{ */
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_UNICODE(tmp, str, duplicate);

	return zend_hash_index_update(Z_ARRVAL_P(arg), index, (void *) &tmp, sizeof(zval *), dest);
}
/* }}} */

ZEND_API int add_get_index_unicodel(zval *arg, ulong index, UChar *str, uint length, void **dest, int duplicate) /* {{{ */
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_UNICODEL(tmp, str, length, duplicate);

	return zend_hash_index_update(Z_ARRVAL_P(arg), index, (void *) &tmp, sizeof(zval *), dest);
}
/* }}} */

ZEND_API int add_property_long_ex(zval *arg, char *key, uint key_len, long n TSRMLS_DC) /* {{{ */
{
	zval *tmp;
	zval *z_key;

	MAKE_STD_ZVAL(tmp);
	ZVAL_LONG(tmp, n);

	MAKE_STD_ZVAL(z_key);
	ZVAL_ASCII_STRINGL(z_key, key, key_len-1, 1);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, z_key, tmp TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_bool_ex(zval *arg, char *key, uint key_len, int b TSRMLS_DC) /* {{{ */
{
	zval *tmp;
	zval *z_key;

	MAKE_STD_ZVAL(tmp);
	ZVAL_BOOL(tmp, b);

	MAKE_STD_ZVAL(z_key);
	ZVAL_ASCII_STRINGL(z_key, key, key_len-1, 1);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, z_key, tmp TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_null_ex(zval *arg, char *key, uint key_len TSRMLS_DC) /* {{{ */
{
	zval *tmp;
	zval *z_key;

	MAKE_STD_ZVAL(tmp);
	ZVAL_NULL(tmp);

	MAKE_STD_ZVAL(z_key);
	ZVAL_ASCII_STRINGL(z_key, key, key_len-1, 1);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, z_key, tmp TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_resource_ex(zval *arg, char *key, uint key_len, long n TSRMLS_DC) /* {{{ */
{
	zval *tmp;
	zval *z_key;

	MAKE_STD_ZVAL(tmp);
	ZVAL_RESOURCE(tmp, n);

	MAKE_STD_ZVAL(z_key);
	ZVAL_ASCII_STRINGL(z_key, key, key_len-1, 1);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, z_key, tmp TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_double_ex(zval *arg, char *key, uint key_len, double d TSRMLS_DC) /* {{{ */
{
	zval *tmp;
	zval *z_key;

	MAKE_STD_ZVAL(tmp);
	ZVAL_DOUBLE(tmp, d);

	MAKE_STD_ZVAL(z_key);
	ZVAL_ASCII_STRINGL(z_key, key, key_len-1, 1);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, z_key, tmp TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_string_ex(zval *arg, char *key, uint key_len, char *str, int duplicate TSRMLS_DC) /* {{{ */
{
	zval *tmp;
	zval *z_key;

	MAKE_STD_ZVAL(tmp);
	ZVAL_STRING(tmp, str, duplicate);

	MAKE_STD_ZVAL(z_key);
	ZVAL_ASCII_STRINGL(z_key, key, key_len-1, 1);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, z_key, tmp TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_stringl_ex(zval *arg, char *key, uint key_len, char *str, uint length, int duplicate TSRMLS_DC) /* {{{ */
{
	zval *tmp;
	zval *z_key;

	MAKE_STD_ZVAL(tmp);
	ZVAL_STRINGL(tmp, str, length, duplicate);

	MAKE_STD_ZVAL(z_key);
	ZVAL_ASCII_STRINGL(z_key, key, key_len-1, 1);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, z_key, tmp TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_ascii_string_ex(zval *arg, char *key, uint key_len, char *str, int duplicate TSRMLS_DC) /* {{{ */
{
	zval *tmp;
	zval *z_key;

	MAKE_STD_ZVAL(tmp);
	ZVAL_ASCII_STRING(tmp, str, duplicate);

	MAKE_STD_ZVAL(z_key);
	ZVAL_ASCII_STRINGL(z_key, key, key_len-1, 1);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, z_key, tmp TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_ascii_stringl_ex(zval *arg, char *key, uint key_len, char *str, uint length, int duplicate TSRMLS_DC) /* {{{ */
{
	zval *tmp;
	zval *z_key;

	MAKE_STD_ZVAL(tmp);
	ZVAL_ASCII_STRINGL(tmp, str, length, duplicate);

	MAKE_STD_ZVAL(z_key);
	ZVAL_ASCII_STRINGL(z_key, key, key_len-1, 1);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, z_key, tmp TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_rt_string_ex(zval *arg, char *key, uint key_len, char *str, int duplicate TSRMLS_DC) /* {{{ */
{
	zval *tmp;
	zval *z_key;

	MAKE_STD_ZVAL(tmp);
	ZVAL_RT_STRING(tmp, str, duplicate);

	MAKE_STD_ZVAL(z_key);
	ZVAL_ASCII_STRINGL(z_key, key, key_len-1, 1);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, z_key, tmp TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_rt_stringl_ex(zval *arg, char *key, uint key_len, char *str, uint length, int duplicate TSRMLS_DC) /* {{{ */
{
	zval *tmp;
	zval *z_key;

	MAKE_STD_ZVAL(tmp);
	ZVAL_RT_STRINGL(tmp, str, length, duplicate);

	MAKE_STD_ZVAL(z_key);
	ZVAL_ASCII_STRINGL(z_key, key, key_len-1, 1);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, z_key, tmp TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_utf8_string_ex(zval *arg, char *key, uint key_len, char *str, int duplicate TSRMLS_DC) /* {{{ */
{
	zval *tmp;
	zval *z_key;

	MAKE_STD_ZVAL(tmp);
	ZVAL_UTF8_STRING(tmp, str, duplicate);

	MAKE_STD_ZVAL(z_key);
	ZVAL_ASCII_STRINGL(z_key, key, key_len-1, 1);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, z_key, tmp TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_utf8_stringl_ex(zval *arg, char *key, uint key_len, char *str, uint length, int duplicate TSRMLS_DC) /* {{{ */
{
	zval *tmp;
	zval *z_key;

	MAKE_STD_ZVAL(tmp);
	ZVAL_UTF8_STRINGL(tmp, str, length, duplicate);

	MAKE_STD_ZVAL(z_key);
	ZVAL_ASCII_STRINGL(z_key, key, key_len-1, 1);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, z_key, tmp TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_unicode_ex(zval *arg, char *key, uint key_len, UChar *str, int duplicate TSRMLS_DC) /* {{{ */
{
	zval *tmp;
	zval *z_key;

	MAKE_STD_ZVAL(tmp);
	ZVAL_UNICODE(tmp, str, duplicate);

	MAKE_STD_ZVAL(z_key);
	ZVAL_ASCII_STRINGL(z_key, key, key_len-1, 1);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, z_key, tmp TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_unicodel_ex(zval *arg, char *key, uint key_len, UChar *str, uint length, int duplicate TSRMLS_DC) /* {{{ */
{
	zval *tmp;
	zval *z_key;

	MAKE_STD_ZVAL(tmp);
	ZVAL_UNICODEL(tmp, str, length, duplicate);

	MAKE_STD_ZVAL(z_key);
	ZVAL_ASCII_STRINGL(z_key, key, key_len-1, 1);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, z_key, tmp TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_zval_ex(zval *arg, char *key, uint key_len, zval *value TSRMLS_DC) /* {{{ */
{
	zval *z_key;

	MAKE_STD_ZVAL(z_key);
	ZVAL_ASCII_STRINGL(z_key, key, key_len-1, 1);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, z_key, value TSRMLS_CC);
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_utf8_property_zval_ex(zval *arg, char *key, uint key_len, zval *value TSRMLS_DC) /* {{{ */
{
	zval *z_key;

	MAKE_STD_ZVAL(z_key);
	ZVAL_UTF8_STRINGL(z_key, key, key_len-1, ZSTR_DUPLICATE);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, z_key, value TSRMLS_CC);
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_zstr_ex(zval *arg, char *key, uint key_len, zend_uchar type, zstr str, int duplicate TSRMLS_DC) /* {{{ */
{
	zval *tmp;
	zval *z_key;

	if (type == IS_UNICODE) {
		MAKE_STD_ZVAL(tmp);
		ZVAL_UNICODE(tmp, str.u, duplicate);
	} else {
		MAKE_STD_ZVAL(tmp);
		ZVAL_STRING(tmp, str.s, duplicate);
	}

	MAKE_STD_ZVAL(z_key);
	ZVAL_ASCII_STRINGL(z_key, key, key_len-1, 1);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, z_key, tmp TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	zval_ptr_dtor(&z_key);
	return SUCCESS;
}
/* }}} */

ZEND_API int add_property_zstrl_ex(zval *arg, char *key, uint key_len, zend_uchar type, zstr str, uint length, int duplicate TSRMLS_DC) /* {{{ */
{
	zval *tmp;
	zval *z_key;

	if (type == IS_UNICODE) {
		MAKE_STD_ZVAL(tmp);
		ZVAL_UNICODEL(tmp, str.u, length, duplicate);
	} else {
		MAKE_STD_ZVAL(tmp);
		ZVAL_STRINGL(tmp, str.s, length, duplicate);
	}

	MAKE_STD_ZVAL(z_key);
	ZVAL_ASCII_STRINGL(z_key, key, key_len-1, 1);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, z_key, tmp TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
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

ZEND_API int zend_startup_modules(TSRMLS_D) /* {{{ */
{
	zend_hash_sort(&module_registry, zend_sort_modules, NULL, 0 TSRMLS_CC);
	zend_hash_apply(&module_registry, (apply_func_t)zend_startup_module_ex TSRMLS_CC);
	return SUCCESS;
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

ZEND_API void zend_check_magic_method_implementation(zend_class_entry *ce, zend_function *fptr, int error_type TSRMLS_DC) /* {{{ */
{
	unsigned int lcname_len;
	zstr lcname;
	int name_len;
	zend_uchar utype = UG(unicode)?IS_UNICODE:IS_STRING;

	/* we don't care if the function name is longer, in fact lowercasing only
	 * the beginning of the name speeds up the check process */
	if (UG(unicode)) {
		name_len = u_strlen(fptr->common.function_name.u);
	} else {
		name_len = strlen(fptr->common.function_name.s);
	}
	lcname = zend_u_str_case_fold(utype, fptr->common.function_name, name_len, 0, &lcname_len);

	if (lcname_len == sizeof(ZEND_DESTRUCTOR_FUNC_NAME) - 1 &&
		ZEND_U_EQUAL(utype, lcname, lcname_len, ZEND_DESTRUCTOR_FUNC_NAME, sizeof(ZEND_DESTRUCTOR_FUNC_NAME)-1) && fptr->common.num_args != 0
	) {
		zend_error(error_type, "Destructor %v::%s() cannot take arguments", ce->name, ZEND_DESTRUCTOR_FUNC_NAME);
	} else if (lcname_len == sizeof(ZEND_CLONE_FUNC_NAME) - 1 &&
		ZEND_U_EQUAL(utype, lcname, lcname_len, ZEND_CLONE_FUNC_NAME, sizeof(ZEND_CLONE_FUNC_NAME)-1) && fptr->common.num_args != 0
	) {
		zend_error(error_type, "Method %v::%s() cannot accept any arguments", ce->name, ZEND_CLONE_FUNC_NAME);
	} else if (lcname_len == sizeof(ZEND_GET_FUNC_NAME) - 1 &&
		ZEND_U_EQUAL(utype, lcname, lcname_len, ZEND_GET_FUNC_NAME, sizeof(ZEND_GET_FUNC_NAME)-1)
	) {
		if (fptr->common.num_args != 1) {
			zend_error(error_type, "Method %v::%s() must take exactly 1 argument", ce->name, ZEND_GET_FUNC_NAME);
		} else if (ARG_SHOULD_BE_SENT_BY_REF(fptr, 1)) {
			zend_error(error_type, "Method %v::%s() cannot take arguments by reference", ce->name, ZEND_GET_FUNC_NAME);
		}
	} else if (lcname_len == sizeof(ZEND_SET_FUNC_NAME) - 1 &&
		ZEND_U_EQUAL(utype, lcname, lcname_len, ZEND_SET_FUNC_NAME, sizeof(ZEND_SET_FUNC_NAME)-1)
	) {
		if (fptr->common.num_args != 2) {
			zend_error(error_type, "Method %v::%s() must take exactly 2 arguments", ce->name, ZEND_SET_FUNC_NAME);
		} else if (ARG_SHOULD_BE_SENT_BY_REF(fptr, 1) || ARG_SHOULD_BE_SENT_BY_REF(fptr, 2)) {
			zend_error(error_type, "Method %v::%s() cannot take arguments by reference", ce->name, ZEND_SET_FUNC_NAME);
		}
	} else if (lcname_len == sizeof(ZEND_UNSET_FUNC_NAME) - 1 &&
		ZEND_U_EQUAL(utype, lcname, lcname_len, ZEND_UNSET_FUNC_NAME, sizeof(ZEND_UNSET_FUNC_NAME)-1)
	) {
		if (fptr->common.num_args != 1) {
			zend_error(error_type, "Method %v::%s() must take exactly 1 argument", ce->name, ZEND_UNSET_FUNC_NAME);
		} else if (ARG_SHOULD_BE_SENT_BY_REF(fptr, 1)) {
			zend_error(error_type, "Method %v::%s() cannot take arguments by reference", ce->name, ZEND_UNSET_FUNC_NAME);
		}
	} else if (lcname_len == sizeof(ZEND_ISSET_FUNC_NAME) - 1 &&
		ZEND_U_EQUAL(utype, lcname, lcname_len, ZEND_ISSET_FUNC_NAME, sizeof(ZEND_ISSET_FUNC_NAME)-1)
	) {
		if (fptr->common.num_args != 1) {
			zend_error(error_type, "Method %v::%s() must take exactly 1 argument", ce->name, ZEND_ISSET_FUNC_NAME);
		} else if (ARG_SHOULD_BE_SENT_BY_REF(fptr, 1)) {
			zend_error(error_type, "Method %v::%s() cannot take arguments by reference", ce->name, ZEND_ISSET_FUNC_NAME);
		}
	} else if (lcname_len == sizeof(ZEND_CALL_FUNC_NAME) - 1 &&
		ZEND_U_EQUAL(utype, lcname, lcname_len, ZEND_CALL_FUNC_NAME, sizeof(ZEND_CALL_FUNC_NAME)-1)
	) {
		if (fptr->common.num_args != 2) {
			zend_error(error_type, "Method %v::%s() must take exactly 2 arguments", ce->name, ZEND_CALL_FUNC_NAME);
		} else if (ARG_SHOULD_BE_SENT_BY_REF(fptr, 1) || ARG_SHOULD_BE_SENT_BY_REF(fptr, 2)) {
			zend_error(error_type, "Method %v::%s() cannot take arguments by reference", ce->name, ZEND_CALL_FUNC_NAME);
		}
	} else if (lcname_len == sizeof(ZEND_CALLSTATIC_FUNC_NAME) - 1 &&
		ZEND_U_EQUAL(utype, lcname, lcname_len, ZEND_CALLSTATIC_FUNC_NAME, sizeof(ZEND_CALLSTATIC_FUNC_NAME)-1)
	) {
		if (fptr->common.num_args != 2) {
			zend_error(error_type, "Method %v::%s() must take exactly 2 arguments", ce->name, ZEND_CALLSTATIC_FUNC_NAME);
		} else if (ARG_SHOULD_BE_SENT_BY_REF(fptr, 1) || ARG_SHOULD_BE_SENT_BY_REF(fptr, 2)) {
			zend_error(error_type, "Method %v::%s() cannot take arguments by reference", ce->name, ZEND_CALLSTATIC_FUNC_NAME);
		}
	} else if (lcname_len == sizeof(ZEND_TOSTRING_FUNC_NAME) - 1 &&
		ZEND_U_EQUAL(utype, lcname, lcname_len, ZEND_TOSTRING_FUNC_NAME, sizeof(ZEND_TOSTRING_FUNC_NAME)-1) && fptr->common.num_args != 0
	) {
		zend_error(error_type, "Method %v::%s() cannot take arguments", ce->name, ZEND_TOSTRING_FUNC_NAME);
	}
	efree(lcname.v);
}
/* }}} */

/* registers all functions in *library_functions in the function hash */
ZEND_API int zend_register_functions(zend_class_entry *scope, const zend_function_entry *functions, HashTable *function_table, int type TSRMLS_DC) /* {{{ */
{
	const zend_function_entry *ptr = functions;
	zend_function function, *reg_function;
	zend_internal_function *internal_function = (zend_internal_function *)&function;
	int count=0, unload=0;
	HashTable *target_function_table = function_table;
	int error_type;
	zend_function *ctor = NULL, *dtor = NULL, *clone = NULL, *__get = NULL, *__set = NULL, *__unset = NULL, *__isset = NULL, *__call = NULL, *__callstatic = NULL, *__tostring = NULL;
	char *lowercase_name;
	int fname_len;
	zstr lc_class_name = NULL_ZSTR;
	unsigned int lc_class_name_len = 0;

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
		lc_class_name = zend_u_str_case_fold(ZEND_STR_TYPE, scope->name, scope->name_length, 0, &lc_class_name_len);
	}

	while (ptr->fname) {
		internal_function->handler = ptr->handler;
		if (UG(unicode)) {
			int len = strlen(ptr->fname)+1;

			internal_function->function_name.u = malloc(UBYTES(len));
			u_charsToUChars(ptr->fname, internal_function->function_name.u, len);
		} else {
			internal_function->function_name.s = (char*)ptr->fname;
		}
		internal_function->scope = scope;
		internal_function->prototype = NULL;
		if (ptr->arg_info) {
			if (UG(unicode)) {
				zend_arg_info *args;
				int n = ptr->num_args;

				args = internal_function->arg_info = malloc((n + 1) * sizeof(zend_arg_info));
				memcpy(args, ptr->arg_info+1, (n + 1) * sizeof(zend_arg_info));
				while (n > 0) {
					--n;
					if (args[n].name.s) {
						UChar *uname = malloc(UBYTES(args[n].name_len + 1));
						u_charsToUChars(args[n].name.s, uname, args[n].name_len+1);
						args[n].name.u = uname;
					}
					if (args[n].class_name.s) {
						UChar *uname = malloc(UBYTES(args[n].class_name_len + 1));
						u_charsToUChars(args[n].class_name.s, uname, args[n].class_name_len+1);
						args[n].class_name.u = uname;
					}
				}
			} else {
				internal_function->arg_info = (zend_arg_info*)ptr->arg_info+1;
			}
			internal_function->num_args = ptr->num_args;
			/* Currently you cannot denote that the function can accept less arguments than num_args */
			if (ptr->arg_info[0].required_num_args == -1) {
				internal_function->required_num_args = ptr->num_args;
			} else {
				internal_function->required_num_args = ptr->arg_info[0].required_num_args;
			}
			internal_function->pass_rest_by_reference = ptr->arg_info[0].pass_by_reference;
			internal_function->return_reference = ptr->arg_info[0].return_reference;
		} else {
			internal_function->arg_info = NULL;
			internal_function->num_args = 0;
			internal_function->required_num_args = 0;
			internal_function->pass_rest_by_reference = 0;
			internal_function->return_reference = 0;
		}
		if (ptr->flags) {
			if (!(ptr->flags & ZEND_ACC_PPP_MASK)) {
				if (ptr->flags != ZEND_ACC_DEPRECATED || scope) {
					zend_error(error_type, "Invalid access level for %v%s%s() - access must be exactly one of public, protected or private", scope ? scope->name : EMPTY_ZSTR, scope ? "::" : "", ptr->fname);
				}
				internal_function->fn_flags = ZEND_ACC_PUBLIC | ptr->flags;
			} else {
				internal_function->fn_flags = ptr->flags;
			}
		} else {
			internal_function->fn_flags = ZEND_ACC_PUBLIC;
		}
		if (ptr->flags & ZEND_ACC_ABSTRACT) {
			if (scope) {
				/* This is a class that must be abstract itself. Here we set the check info. */
				scope->ce_flags |= ZEND_ACC_IMPLICIT_ABSTRACT_CLASS;
				if (!(scope->ce_flags & ZEND_ACC_INTERFACE)) {
					/* Since the class is not an interface it needs to be declared as a abstract class. */
					/* Since here we are handling internal functions only we can add the keyword flag. */
					/* This time we set the flag for the keyword 'abstratc'. */
					scope->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;
				}
			}
			if (ptr->flags & ZEND_ACC_STATIC && (!scope || !(scope->ce_flags & ZEND_ACC_INTERFACE))) {
				zend_error(error_type, "Static function %v%s%s() cannot be abstract", scope ? scope->name : EMPTY_ZSTR, scope ? "::" : "", ptr->fname);
			}
		} else {
			if (scope && (scope->ce_flags & ZEND_ACC_INTERFACE)) {
				efree(lc_class_name.v);
				zend_error(error_type, "Interface %v cannot contain non abstract method %s()", scope->name, ptr->fname);
				return FAILURE;
			}
			if (!internal_function->handler) {
				if (scope) {
					efree(lc_class_name.v);
				}
				zend_error(error_type, "Method %v%s%s() cannot be a NULL function", scope ? scope->name : EMPTY_ZSTR, scope ? "::" : "", ptr->fname);
				zend_unregister_functions(functions, count, target_function_table TSRMLS_CC);
				return FAILURE;
			}
		}
		fname_len = strlen(ptr->fname);
		lowercase_name = zend_str_tolower_dup(ptr->fname, fname_len);
		if (zend_ascii_hash_add(target_function_table, lowercase_name, fname_len+1, &function, sizeof(zend_function), (void**)&reg_function) == FAILURE) {
			unload=1;
			efree(lowercase_name);
			break;
		}
		if (scope) {
			/* Look for ctor, dtor, clone
			 * If it's an old-style constructor, store it only if we don't have
			 * a constructor already.
			 */
			unsigned int lc_func_name_len;
			zstr lc_func_name = zend_u_str_case_fold(ZEND_STR_TYPE, internal_function->function_name, fname_len, 0, &lc_func_name_len);

			if ((lc_func_name_len == lc_class_name_len) && !memcmp(lc_func_name.v, lc_class_name.v, UG(unicode)?UBYTES(lc_class_name_len):lc_class_name_len) && !ctor) {
				ctor = reg_function;
			} else if ((fname_len == sizeof(ZEND_CONSTRUCTOR_FUNC_NAME)-1) && !memcmp(lowercase_name, ZEND_CONSTRUCTOR_FUNC_NAME, sizeof(ZEND_CONSTRUCTOR_FUNC_NAME))) {
				ctor = reg_function;
			} else if ((fname_len == sizeof(ZEND_DESTRUCTOR_FUNC_NAME)-1) && !memcmp(lowercase_name, ZEND_DESTRUCTOR_FUNC_NAME, sizeof(ZEND_DESTRUCTOR_FUNC_NAME))) {
				dtor = reg_function;
				if (internal_function->num_args) {
					zend_error(error_type, "Destructor %v::%s() cannot take arguments", scope->name, ptr->fname);
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
			efree(lc_func_name.v);
			if (reg_function) {
				zend_check_magic_method_implementation(scope, reg_function, error_type TSRMLS_CC);
			}
		}
		ptr++;
		count++;
		efree(lowercase_name);
	}
	if (unload) { /* before unloading, display all remaining bad function in the module */
		if (scope) {
			efree(lc_class_name.v);
		}
		while (ptr->fname) {
			if (zend_hash_exists(target_function_table, ptr->fname, strlen(ptr->fname)+1)) {
				zend_error(error_type, "Function registration failed - duplicate name - %v%s%s", scope ? scope->name : EMPTY_ZSTR, scope ? "::" : "", ptr->fname);
			}
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
				zend_error(error_type, "Constructor %v::%v() cannot be static", scope->name, ctor->common.function_name);
			}
			ctor->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (dtor) {
			dtor->common.fn_flags |= ZEND_ACC_DTOR;
			if (dtor->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Destructor %v::%v() cannot be static", scope->name, dtor->common.function_name);
			}
			dtor->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (clone) {
			clone->common.fn_flags |= ZEND_ACC_CLONE;
			if (clone->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Constructor %v::%v() cannot be static", scope->name, clone->common.function_name);
			}
			clone->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (__call) {
			if (__call->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %v::%v() cannot be static", scope->name, __call->common.function_name);
			}
			__call->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (__callstatic) {
			if (!(__callstatic->common.fn_flags & ZEND_ACC_STATIC)) {
				zend_error(error_type, "Method %v::%v() must be static", scope->name, __callstatic->common.function_name);
			}
			__callstatic->common.fn_flags |= ZEND_ACC_STATIC;
		}
		if (__tostring) {
			if (__tostring->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %v::%v() cannot be static", scope->name, __tostring->common.function_name);
			}
			__tostring->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (__get) {
			if (__get->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %v::%v() cannot be static", scope->name, __get->common.function_name);
			}
			__get->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (__set) {
			if (__set->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %v::%v() cannot be static", scope->name, __set->common.function_name);
			}
			__set->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (__unset) {
			if (__unset->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %v::%v() cannot be static", scope->name, __unset->common.function_name);
			}
			__unset->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		if (__isset) {
			if (__isset->common.fn_flags & ZEND_ACC_STATIC) {
				zend_error(error_type, "Method %v::%v() cannot be static", scope->name, __isset->common.function_name);
			}
			__isset->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
		}
		efree(lc_class_name.v);
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
		zend_ascii_hash_del(target_function_table, ptr->fname, strlen(ptr->fname)+1);
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

ZEND_API int zend_get_module_started(char *module_name) /* {{{ */
{
	zend_module_entry *module;

	return (zend_hash_find(&module_registry, module_name, strlen(module_name)+1, (void**)&module) == SUCCESS && module->module_started) ? SUCCESS : FAILURE;
}
/* }}} */

void module_destructor(zend_module_entry *module) /* {{{ */
{
	TSRMLS_FETCH();

	if (module->type == MODULE_TEMPORARY) {
		zend_clean_module_rsrc_dtors(module->module_number TSRMLS_CC);
		clean_module_constants(module->module_number TSRMLS_CC);
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
		ts_free_id(*module->globals_id_ptr);
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

#if HAVE_LIBDL || defined(HAVE_MACH_O_DYLD_H)
#if !(defined(NETWARE) && defined(APACHE_1_BUILD))
	if (module->handle) {
		DL_UNLOAD(module->handle);
	}
#endif
#endif
}
/* }}} */

/* call request startup for all modules */
int module_registry_request_startup(zend_module_entry *module TSRMLS_DC) /* {{{ */
{
	if (module->request_startup_func) {
#if 0
		zend_printf("%s: Request startup\n", module->name);
#endif
		if (module->request_startup_func(module->type, module->module_number TSRMLS_CC)==FAILURE) {
			zend_error(E_WARNING, "request_startup() for %s module failed", module->name);
			exit(1);
		}
	}
	return 0;
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

int module_registry_unload_temp(zend_module_entry *module TSRMLS_DC) /* {{{ */
{
	return (module->type == MODULE_TEMPORARY) ? ZEND_HASH_APPLY_REMOVE : ZEND_HASH_APPLY_STOP;
}
/* }}} */

/* return the next free module number */
int zend_next_free_module(void) /* {{{ */
{
	return ++module_count;
}
/* }}} */

static zend_class_entry *do_register_internal_class(zend_class_entry *orig_class_entry, zend_uint ce_flags TSRMLS_DC) /* {{{ */
{
	zend_class_entry *class_entry = malloc(sizeof(zend_class_entry));
	zstr lcname;
	unsigned int lcname_len;

	*class_entry = *orig_class_entry;

	class_entry->type = ZEND_INTERNAL_CLASS;
	zend_initialize_class_data(class_entry, 0 TSRMLS_CC);
	class_entry->ce_flags = ce_flags;
	class_entry->module = EG(current_module);

	if (class_entry->builtin_functions) {
		zend_register_functions(class_entry, class_entry->builtin_functions, &class_entry->function_table, MODULE_PERSISTENT TSRMLS_CC);
	}

	lcname = zend_u_str_case_fold(ZEND_STR_TYPE, class_entry->name, class_entry->name_length, 0, &lcname_len);
	zend_u_hash_update(CG(class_table), ZEND_STR_TYPE, lcname, lcname_len+1, &class_entry, sizeof(zend_class_entry *), NULL);
	efree(lcname.v);
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
		if (zend_ascii_hash_find(CG(class_table), parent_name, strlen(parent_name)+1, (void **) &pce)==FAILURE) {
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

ZEND_API int zend_set_hash_symbol(zval *symbol, char *name, int name_length, zend_bool is_ref, int num_symbol_tables, ...) /* {{{ */
{
	HashTable *symbol_table;
	va_list symbol_table_list;

	if (num_symbol_tables <= 0) return FAILURE;

	Z_SET_ISREF_TO_P(symbol, is_ref);

	va_start(symbol_table_list, num_symbol_tables);
	while (num_symbol_tables-- > 0) {
		symbol_table = va_arg(symbol_table_list, HashTable *);
		zend_rt_hash_update(symbol_table, name, name_length + 1, &symbol, sizeof(zval *), NULL);
		zval_add_ref(&symbol);
	}
	va_end(symbol_table_list);
	return SUCCESS;
}
/* }}} */

/* Disabled functions support */

/* {{{ proto void display_disabled_function(void) U
Dummy function which displays an error when a disabled function is called. */
ZEND_API ZEND_FUNCTION(display_disabled_function)
{
	zend_error(E_WARNING, "%v() has been disabled for security reasons", get_active_function_name(TSRMLS_C));
}
/* }}} */

static zend_function_entry disabled_function[] = {
	ZEND_FE(display_disabled_function,			NULL)
	{ NULL, NULL, NULL }
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

static zend_object_value display_disabled_class(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
	zend_object_value retval;
	zend_object *intern;
	retval = zend_objects_new(&intern, class_type TSRMLS_CC);
	ALLOC_HASHTABLE(intern->properties);
	zend_u_hash_init(intern->properties, 0, NULL, ZVAL_PTR_DTOR, 0, UG(unicode));
	zend_error(E_WARNING, "%v() has been disabled for security reasons", class_type->name);
	return retval;
}
/* }}} */

static const zend_function_entry disabled_class_new[] = {
	{ NULL, NULL, NULL }
};

ZEND_API int zend_disable_class(char *class_name, uint class_name_length TSRMLS_DC) /* {{{ */
{
	zend_class_entry disabled_class;

	zend_str_tolower(class_name, class_name_length);
	if (zend_hash_del(CG(class_table), class_name, class_name_length+1)==FAILURE) {
		return FAILURE;
	}
	INIT_OVERLOADED_CLASS_ENTRY_EX(disabled_class, class_name, class_name_length, disabled_class_new, NULL, NULL, NULL, NULL, NULL);
	disabled_class.create_object = display_disabled_class;
	disabled_class.name_length = class_name_length;
	zend_register_internal_class(&disabled_class TSRMLS_CC);
	return SUCCESS;
}
/* }}} */

static int zend_is_callable_check_func(int check_flags, zval ***zobj_ptr_ptr, zend_class_entry *ce_org, zval *callable, zend_class_entry **ce_ptr, zend_function **fptr_ptr, char **error TSRMLS_DC) /* {{{ */
{
	int retval;
	zstr lmname, mname, colon = NULL_ZSTR;
	unsigned int clen = 0, mlen;
	zend_function *fptr;
	HashTable *ftable;

	*ce_ptr = NULL;
	*fptr_ptr = NULL;

	if (!ce_org) {
		/* Skip leading :: */
		if (Z_TYPE_P(callable) == IS_UNICODE &&
			Z_USTRVAL_P(callable)[0] == ':' &&
			Z_USTRVAL_P(callable)[1] == ':'
		) {
			zstr tmp;

			tmp.u = Z_USTRVAL_P(callable) + 2;
			lmname = zend_u_str_case_fold(IS_UNICODE, tmp, Z_USTRLEN_P(callable)-2, 1, &mlen);
		} else if (Z_TYPE_P(callable) == IS_STRING &&
			Z_STRVAL_P(callable)[0] == ':' &&
			Z_STRVAL_P(callable)[1] == ':'
		) {
			zstr tmp;

			tmp.s = Z_STRVAL_P(callable) + 2;
			lmname = zend_u_str_case_fold(IS_STRING, tmp, Z_STRLEN_P(callable)-2, 1, &mlen);
		} else {
			lmname = zend_u_str_case_fold(Z_TYPE_P(callable), Z_UNIVAL_P(callable), Z_UNILEN_P(callable), 1, &mlen);
		}
		/* Check if function with given name exists.
		 * This may be a compound name that includes namespace name */
		if (zend_u_hash_find(EG(function_table), Z_TYPE_P(callable), lmname, mlen+1, (void**)&fptr) == SUCCESS) {
			*fptr_ptr = fptr;
			efree(lmname.v);
			return 1;
		}
		efree(lmname.v);
	}
	/* Split name into class/namespace and method/function names */
	if (Z_TYPE_P(callable) == IS_UNICODE) {
		if ((colon.u = u_strrstr(Z_USTRVAL_P(callable), u_doublecolon)) != NULL) {
			if (colon.u == Z_USTRVAL_P(callable)) {
				return 0;
			}
			mlen = u_strlen(colon.u+2);
			clen = Z_USTRLEN_P(callable) - mlen - 2;
			mname.u = colon.u + 2;
		} else {
			colon.u = NULL;
		}
	} else {
		if ((colon.s = zend_memrchr(Z_STRVAL_P(callable), ':', Z_STRLEN_P(callable))) != NULL &&
			colon.s > Z_STRVAL_P(callable) &&
			*(colon.s-1) == ':'
		) {
			colon.s--;
			clen = colon.s - Z_STRVAL_P(callable);
			if (clen == 0) {
				if (error) zend_spprintf(error, 0, "invalid function name");
				return 0;
			}
			mlen = Z_STRLEN_P(callable) - clen - 2;
			mname.s = colon.s + 2;
		} else {
			colon.s = NULL;
		}
	}
	if (colon.v != NULL) {
		/* This is a compound name.
		 * Try to fetch class and then find static method. */
		*ce_ptr = zend_u_fetch_class(Z_TYPE_P(callable), Z_UNIVAL_P(callable), clen, ZEND_FETCH_CLASS_AUTO | ZEND_FETCH_CLASS_SILENT TSRMLS_CC);
		if (!*ce_ptr) {
			char *cname = estrndup(Z_STRVAL_P(callable), clen);
			if (error) zend_spprintf(error, 0, "class '%s' not found", cname);
			efree(cname);
			return 0;
		}
		ftable = &(*ce_ptr)->function_table;
		if (ce_org && !instanceof_function(ce_org, *ce_ptr TSRMLS_CC)) {
			if (error) zend_spprintf(error, 0, "class '%s' is not a subclass of '%s'", ce_org->name, (*ce_ptr)->name);
			return 0;
		}
		lmname = zend_u_str_case_fold(Z_TYPE_P(callable), mname, mlen, 1, &mlen);
	} else if (ce_org) {
		/* Try to fetch find static method of given class. */
		lmname = zend_u_str_case_fold(Z_TYPE_P(callable), Z_UNIVAL_P(callable), Z_UNILEN_P(callable), 1, &mlen);
		ftable = &ce_org->function_table;
		*ce_ptr = ce_org;
	} else {
		/* We already checked for plain function before. */
		if (error) zend_spprintf(error, 0, "function '%s' not found or invalid function name", Z_STRVAL_P(callable));
		return 0;
	}

	retval = zend_u_hash_find(ftable, Z_TYPE_P(callable), lmname, mlen+1, (void**)&fptr) == SUCCESS ? 1 : 0;

	if (!retval) {
		if (*zobj_ptr_ptr && *ce_ptr && (*ce_ptr)->__call != 0) {
			retval = (*ce_ptr)->__call != NULL;
			*fptr_ptr = (*ce_ptr)->__call;
		} else if (!*zobj_ptr_ptr && *ce_ptr && (*ce_ptr)->__callstatic) {
			retval = 1;
			*fptr_ptr = (*ce_ptr)->__callstatic;
		} else {
			if (*ce_ptr) {
				if (error) zend_spprintf(error, 0, "class '%s' does not have a method '%s'", (*ce_ptr)->name, lmname);
			} else {
				if (error) zend_spprintf(error, 0, "function '%s' does not exist", lmname);
			}
		}
	} else {
		*fptr_ptr = fptr;
		if (*ce_ptr) {
			if (!*zobj_ptr_ptr && !(fptr->common.fn_flags & ZEND_ACC_STATIC)) {
				int severity;
				char *verb;
				if (fptr->common.fn_flags & ZEND_ACC_ALLOW_STATIC) {
					severity = E_STRICT;
					verb = "should not";
				} else {
					severity = E_ERROR;
					verb = "cannot";
				}
				if ((check_flags & IS_CALLABLE_CHECK_IS_STATIC) != 0) {
					retval = 0;
				}
				if (EG(This) && instanceof_function(Z_OBJCE_P(EG(This)), *ce_ptr TSRMLS_CC)) {
					*zobj_ptr_ptr = &EG(This);
					if (error) {
						zend_spprintf(error, 0, "non-static method %s::%s() %s be called statically, assuming $this from compatible context %s", (*ce_ptr)->name, fptr->common.function_name, verb, Z_OBJCE_P(EG(This))->name);
					} else if (retval) {
						zend_error(severity, "Non-static method %s::%s() %s be called statically, assuming $this from compatible context %s", (*ce_ptr)->name, fptr->common.function_name, verb, Z_OBJCE_P(EG(This))->name);
					}
				} else {
					if (error) {
						zend_spprintf(error, 0, "non-static method %s::%s() %s be called statically", (*ce_ptr)->name, fptr->common.function_name, verb);
					} else if (retval) {
						zend_error(severity, "Non-static method %s::%s() %s be called statically", (*ce_ptr)->name, fptr->common.function_name, verb);
					}
				}
			}
			if (retval && (check_flags & IS_CALLABLE_CHECK_NO_ACCESS) == 0) {
				if (fptr->op_array.fn_flags & ZEND_ACC_PRIVATE) {
					if (!zend_check_private(fptr, *zobj_ptr_ptr ? Z_OBJCE_PP(*zobj_ptr_ptr) : EG(scope), lmname, mlen TSRMLS_CC)) {
						if (error) zend_spprintf(error, 0, "cannot access private method %s::%s()", (*ce_ptr)->name, fptr->common.function_name);
						retval = 0;
					}
				} else if ((fptr->common.fn_flags & ZEND_ACC_PROTECTED)) {
					if (!zend_check_protected(fptr->common.scope, EG(scope))) {
						if (error) zend_spprintf(error, 0, "cannot access protected method %s::%s()", (*ce_ptr)->name, fptr->common.function_name);
						retval = 0;
					}
				}
			}
		}
	}
	efree(lmname.v);
	return retval;
}
/* }}} */

ZEND_API zend_bool zend_is_callable_ex(zval *callable, uint check_flags, zval *callable_name, zend_class_entry **ce_ptr, zend_function **fptr_ptr, zval ***zobj_ptr_ptr, char **error TSRMLS_DC) /* {{{ */
{
	zstr lcname;
	unsigned int lcname_len;
	zend_class_entry *ce_local, **pce;
	zend_function *fptr_local;
	zval **zobj_ptr_local;

	if (callable_name) {
		ZVAL_NULL(callable_name);
	}
	if (ce_ptr == NULL) {
		ce_ptr = &ce_local;
	}
	if (fptr_ptr == NULL) {
		fptr_ptr = &fptr_local;
	}
	if (zobj_ptr_ptr == NULL) {
		zobj_ptr_ptr = &zobj_ptr_local;
	}
	if (error) {
		*error = NULL;
	}
	*ce_ptr = NULL;
	*fptr_ptr = NULL;
	*zobj_ptr_ptr = NULL;

	switch (Z_TYPE_P(callable)) {
		case IS_STRING:
		case IS_UNICODE:
			if (callable_name) {
				*callable_name = *callable;
				zval_copy_ctor(callable_name);
				convert_to_text(callable_name);
			}
			if (check_flags & IS_CALLABLE_CHECK_SYNTAX_ONLY) {
				return 1;
			}

			return zend_is_callable_check_func(check_flags, zobj_ptr_ptr, NULL, callable, ce_ptr, fptr_ptr, error TSRMLS_CC);

		case IS_ARRAY:
			{
				zend_class_entry *ce = NULL;
				zval **method;
				zval **obj;

				if (zend_hash_num_elements(Z_ARRVAL_P(callable)) == 2 &&
					zend_hash_index_find(Z_ARRVAL_P(callable), 0, (void **) &obj) == SUCCESS &&
					zend_hash_index_find(Z_ARRVAL_P(callable), 1, (void **) &method) == SUCCESS &&
					(Z_TYPE_PP(obj) == IS_OBJECT ||
					Z_TYPE_PP(obj) == IS_STRING ||
					Z_TYPE_PP(obj) == IS_UNICODE) &&
					(Z_TYPE_PP(method) == IS_STRING ||
					 Z_TYPE_PP(method) == IS_UNICODE)) {

					if (Z_TYPE_PP(obj) == IS_STRING || Z_TYPE_PP(obj) == IS_UNICODE) {
						if (callable_name) {
							if (UG(unicode)) {
								Z_TYPE_P(callable_name) = IS_UNICODE;
								Z_USTRLEN_P(callable_name) = Z_UNILEN_PP(obj) + Z_UNILEN_PP(method) + 2;
								Z_USTRVAL_P(callable_name) = eumalloc(Z_USTRLEN_P(callable_name)+1);
								if (Z_TYPE_PP(obj) == IS_UNICODE) {
									u_memcpy(Z_USTRVAL_P(callable_name), Z_USTRVAL_PP(obj), Z_USTRLEN_PP(obj));
								} else {
									zval copy;
									int use_copy;

									zend_make_unicode_zval(*obj, &copy, &use_copy);
									u_memcpy(Z_USTRVAL_P(callable_name), Z_USTRVAL(copy), Z_USTRLEN(copy));
									zval_dtor(&copy);
								}
								Z_USTRVAL_P(callable_name)[Z_UNILEN_PP(obj)] = ':';
								Z_USTRVAL_P(callable_name)[Z_UNILEN_PP(obj)+1] = ':';
								if (Z_TYPE_PP(method) == IS_UNICODE) {
									u_memcpy(Z_USTRVAL_P(callable_name)+Z_UNILEN_PP(obj)+2, Z_USTRVAL_PP(method), Z_USTRLEN_PP(method)+1);
								} else {
									zval copy;
									int use_copy;

									zend_make_unicode_zval(*method, &copy, &use_copy);
									u_memcpy(Z_USTRVAL_P(callable_name)+Z_UNILEN_PP(obj)+2, Z_USTRVAL(copy), Z_USTRLEN(copy)+1);
									zval_dtor(&copy);
								}
							} else {
								Z_TYPE_P(callable_name) = IS_STRING;
								Z_STRLEN_P(callable_name) = Z_UNILEN_PP(obj) + Z_UNILEN_PP(method) + 2;
								Z_STRVAL_P(callable_name) = emalloc(Z_STRLEN_P(callable_name)+1);
								if (Z_TYPE_PP(obj) == IS_STRING) {
									memcpy(Z_STRVAL_P(callable_name), Z_STRVAL_PP(obj), Z_STRLEN_PP(obj));
								} else {
									zval copy;
									int use_copy;

									zend_make_string_zval(*obj, &copy, &use_copy);
									memcpy(Z_STRVAL_P(callable_name), Z_STRVAL(copy), Z_STRLEN(copy));
									zval_dtor(&copy);
								}
								Z_STRVAL_P(callable_name)[Z_UNILEN_PP(obj)] = ':';
								Z_STRVAL_P(callable_name)[Z_UNILEN_PP(obj)+1] = ':';
								if (Z_TYPE_PP(method) == IS_STRING) {
									memcpy(Z_STRVAL_P(callable_name)+Z_UNILEN_PP(obj)+2, Z_STRVAL_PP(method), Z_STRLEN_PP(method)+1);
								} else {
									zval copy;
									int use_copy;

									zend_make_string_zval(*method, &copy, &use_copy);
									memcpy(Z_STRVAL_P(callable_name)+Z_UNILEN_PP(obj)+2, Z_STRVAL(copy), Z_STRLEN(copy)+1);
									zval_dtor(&copy);
								}
							}
						}

						if (check_flags & IS_CALLABLE_CHECK_SYNTAX_ONLY) {
							return 1;
						}

						lcname = zend_u_str_case_fold(Z_TYPE_PP(obj), Z_UNIVAL_PP(obj), Z_UNILEN_PP(obj), 1, &lcname_len);

						if (EG(active_op_array) &&
							lcname_len == sizeof("self")-1 &&
							ZEND_U_EQUAL(Z_TYPE_PP(obj), lcname, lcname_len, "self", sizeof("self")-1)) {
							ce = EG(active_op_array)->scope;
						} else if (EG(active_op_array) && EG(active_op_array)->scope &&
							lcname_len == sizeof("parent")-1 &&
							ZEND_U_EQUAL(Z_TYPE_PP(obj), lcname, lcname_len, "parent", sizeof("parent")-1)) {
							ce = EG(active_op_array)->scope->parent;
						} else if (lcname_len == sizeof("static")-1 &&
							ZEND_U_EQUAL(Z_TYPE_PP(obj), lcname, lcname_len, "static", sizeof("static")-1)) {
							ce = EG(called_scope);
						} else if (zend_u_lookup_class(Z_TYPE_PP(obj), Z_UNIVAL_PP(obj), Z_UNILEN_PP(obj), &pce TSRMLS_CC) == SUCCESS) {
							ce = *pce;
						}
						efree(lcname.v);
					} else {
						ce = Z_OBJCE_PP(obj); /* TBFixed: what if it's overloaded? */

						*zobj_ptr_ptr = obj;

						if (callable_name) {
							if (UG(unicode)) {
								Z_TYPE_P(callable_name) = IS_UNICODE;
								Z_USTRLEN_P(callable_name) = ce->name_length + Z_UNILEN_PP(method) + 2;
								Z_USTRVAL_P(callable_name) = eumalloc(Z_USTRLEN_P(callable_name)+1);
								memcpy(Z_USTRVAL_P(callable_name), ce->name.u, UBYTES(ce->name_length));
								Z_USTRVAL_P(callable_name)[ce->name_length] = ':';
								Z_USTRVAL_P(callable_name)[ce->name_length+1] = ':';
								if (Z_TYPE_PP(method) == IS_UNICODE) {
									u_memcpy(Z_USTRVAL_P(callable_name)+ce->name_length+2, Z_USTRVAL_PP(method), Z_USTRLEN_PP(method)+1);
								} else {
									zval copy;
									int use_copy;

									zend_make_unicode_zval(*method, &copy, &use_copy);
									u_memcpy(Z_USTRVAL_P(callable_name)+ce->name_length+2, Z_USTRVAL(copy), Z_USTRLEN(copy)+1);
									zval_dtor(&copy);
								}
							} else {
								Z_TYPE_P(callable_name) = IS_STRING;
								Z_STRLEN_P(callable_name) = ce->name_length + Z_UNILEN_PP(method) + 2;
								Z_STRVAL_P(callable_name) = emalloc(Z_STRLEN_P(callable_name)+1);
								memcpy(Z_STRVAL_P(callable_name), ce->name.s, ce->name_length);
								Z_STRVAL_P(callable_name)[ce->name_length] = ':';
								Z_STRVAL_P(callable_name)[ce->name_length+1] = ':';
								if (Z_TYPE_PP(method) == IS_STRING) {
									memcpy(Z_STRVAL_P(callable_name)+ce->name_length+2, Z_STRVAL_PP(method), Z_STRLEN_PP(method)+1);
								} else {
									zval copy;
									int use_copy;

									zend_make_string_zval(*method, &copy, &use_copy);
									memcpy(Z_STRVAL_P(callable_name)+ce->name_length+2, Z_STRVAL(copy), Z_STRLEN(copy)+1);
									zval_dtor(&copy);
								}
							}
						}

						if (check_flags & IS_CALLABLE_CHECK_SYNTAX_ONLY) {
							*ce_ptr = ce;
							return 1;
						}
					}

					if (ce) {
						return zend_is_callable_check_func(check_flags, zobj_ptr_ptr, ce, *method, ce_ptr, fptr_ptr, error TSRMLS_CC);
					} else {
						if (error) zend_spprintf(error, 0, "first array member is not a valid %s", Z_TYPE_PP(obj) == IS_STRING ? "class name" : "object");
					}
				} else {
					if (zend_hash_num_elements(Z_ARRVAL_P(callable)) == 2) {
						if (!obj) {
							if (error) zend_spprintf(error, 0, "first array member is not a valid class name or object");
						} else {
							if (error) zend_spprintf(error, 0, "second array member is not a valid method");
						}
					} else {
						if (error) zend_spprintf(error, 0, "array must have exactly two members");
					}
					if (callable_name) {
						ZVAL_ASCII_STRINGL(callable_name, "Array", sizeof("Array")-1, 1);
					}
				}
				*ce_ptr = ce;
			}
			return 0;

		default:
			if (callable_name) {
				*callable_name = *callable;
				zval_copy_ctor(callable_name);
				convert_to_text(callable_name);
			}
			if (error) zend_spprintf(error, 0, "no array or string given");
			return 0;
	}
}
/* }}} */

ZEND_API zend_bool zend_is_callable(zval *callable, uint check_flags, zval *callable_name) /* {{{ */
{
	TSRMLS_FETCH();

	return zend_is_callable_ex(callable, check_flags, callable_name, NULL, NULL, NULL, NULL TSRMLS_CC);
}
/* }}} */

ZEND_API zend_bool zend_make_callable(zval *callable, zval *callable_name TSRMLS_DC) /* {{{ */
{
	zend_class_entry *ce;
	zend_function *fptr;
	zval **zobj_ptr;

	if (zend_is_callable_ex(callable, IS_CALLABLE_STRICT, callable_name, &ce, &fptr, &zobj_ptr, NULL TSRMLS_CC)) {
		if ((Z_TYPE_P(callable) == IS_STRING || Z_TYPE_P(callable) == IS_UNICODE) && ce) {
			zval_dtor(callable);
			array_init(callable);
			add_next_index_text(callable, ce->name, 1);
			add_next_index_text(callable, fptr->common.function_name, 1);
		}
		return 1;
	}
	return 0;
}
/* }}} */

ZEND_API int zend_fcall_info_init(zval *callable, uint check_flags, zend_fcall_info *fci, zend_fcall_info_cache *fcc, zval *callable_name, char **error TSRMLS_DC) /* {{{ */
{
	zend_class_entry *ce;
	zend_function *func;
	zval **obj;

	if (!zend_is_callable_ex(callable, check_flags, callable_name, &ce, &func, &obj, error TSRMLS_CC)) {
		return FAILURE;
	}

	fci->size = sizeof(*fci);
	fci->function_table = ce ? &ce->function_table : EG(function_table);
	fci->object_pp = obj;
	fci->function_name = callable;
	fci->retval_ptr_ptr = NULL;
	fci->param_count = 0;
	fci->params = NULL;
	fci->no_separation = 1;
	fci->symbol_table = NULL;

	if (ZEND_U_CASE_EQUAL(ZEND_STR_TYPE, func->common.function_name, USTR_LEN(func->common.function_name), ZEND_CALL_FUNC_NAME, sizeof(ZEND_CALL_FUNC_NAME)-1) ||
		ZEND_U_CASE_EQUAL(ZEND_STR_TYPE, func->common.function_name, USTR_LEN(func->common.function_name), ZEND_CALLSTATIC_FUNC_NAME, sizeof(ZEND_CALLSTATIC_FUNC_NAME)-1)
	) {
		fcc->initialized = 0;
		fcc->function_handler = NULL;
		fcc->calling_scope = NULL;
		fcc->object_pp = NULL;
	} else {
		fcc->initialized = 1;
		fcc->function_handler = func;
		fcc->calling_scope = ce;
		fcc->object_pp = obj;
	}

	return SUCCESS;
}
/* }}} */

ZEND_API void zend_fcall_info_args_clear(zend_fcall_info *fci, int free_mem) /* {{{ */
{
	if (fci->params) {
		while (fci->param_count) {
			zval_ptr_dtor(fci->params[--fci->param_count]);
		}
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
		Z_ADDREF_P(*arg);
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
			Z_ADDREF_P(*(argv[i]));
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
			Z_ADDREF_P(*arg);
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

ZEND_API int zend_u_declare_property_ex(zend_class_entry *ce, zend_uchar type, zstr name, int name_length, zval *property, int access_type, zstr doc_comment, int doc_comment_len TSRMLS_DC) /* {{{ */
{
	zend_property_info property_info;
	HashTable *target_symbol_table;

	if (!(access_type & ZEND_ACC_PPP_MASK)) {
		access_type |= ZEND_ACC_PUBLIC;
	}
	if (access_type & ZEND_ACC_STATIC) {
		target_symbol_table = &ce->default_static_members;
	} else {
		target_symbol_table = &ce->default_properties;
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
				zstr priv_name;
				int priv_name_length;

				zend_u_mangle_property_name(&priv_name, &priv_name_length, type, ce->name, ce->name_length, name, name_length, ce->type & ZEND_INTERNAL_CLASS);
				zend_u_hash_update(target_symbol_table, type, priv_name, priv_name_length+1, &property, sizeof(zval *), NULL);
				property_info.name = priv_name;
				property_info.name_length = priv_name_length;
			}
			break;
		case ZEND_ACC_PROTECTED: {
				zstr prot_name;
				int prot_name_length;

				zend_u_mangle_property_name(&prot_name, &prot_name_length, type, ZSTR("*"), 1, name, name_length, ce->type & ZEND_INTERNAL_CLASS);
				zend_u_hash_update(target_symbol_table, type, prot_name, prot_name_length+1, &property, sizeof(zval *), NULL);
				property_info.name = prot_name;
				property_info.name_length = prot_name_length;
			}
			break;
		case ZEND_ACC_PUBLIC:
			if (ce->parent) {
				zstr prot_name;
				int prot_name_length;

				zend_u_mangle_property_name(&prot_name, &prot_name_length, type, ZSTR("*"), 1, name, name_length, ce->type & ZEND_INTERNAL_CLASS);
				zend_u_hash_del(target_symbol_table, type, prot_name, prot_name_length+1);
				pefree(prot_name.v, ce->type & ZEND_INTERNAL_CLASS);
			}
			zend_u_hash_update(target_symbol_table, type, name, name_length+1, &property, sizeof(zval *), NULL);
			property_info.name.s = ce->type & ZEND_INTERNAL_CLASS ?
				(type==IS_UNICODE?(char*)zend_ustrndup(name.u, name_length):zend_strndup(name.s, name_length)) :
				(type==IS_UNICODE?(char*)eustrndup(name.u, name_length):estrndup(name.s, name_length));
			property_info.name_length = name_length;
			break;
	}
	property_info.flags = access_type;
	property_info.h = zend_u_get_hash_value(type, property_info.name, property_info.name_length+1);

	property_info.doc_comment = doc_comment;
	property_info.doc_comment_len = doc_comment_len;

	property_info.ce = ce;

	zend_u_hash_update(&ce->properties_info, type, name, name_length + 1, &property_info, sizeof(zend_property_info), NULL);

	return SUCCESS;
}
/* }}} */

ZEND_API int zend_declare_property_ex(zend_class_entry *ce, char *name, int name_length, zval *property, int access_type, zstr doc_comment, int doc_comment_len TSRMLS_DC) /* {{{ */
{
	if (UG(unicode)) {
		zstr uname;
		int ret;
		ALLOCA_FLAG(use_heap)

		uname.u = do_alloca(UBYTES(name_length+1), use_heap);
		u_charsToUChars(name, uname.u, name_length+1);
		ret = zend_u_declare_property_ex(ce, IS_UNICODE, uname, name_length, property, access_type, doc_comment, doc_comment_len TSRMLS_CC);
		free_alloca(uname.u, use_heap);
		return ret;
	} else {
		return zend_u_declare_property_ex(ce, IS_STRING, ZSTR(name), name_length, property, access_type, doc_comment, doc_comment_len TSRMLS_CC);
	}
}
/* }}} */

ZEND_API int zend_u_declare_property(zend_class_entry *ce, zend_uchar type, zstr name, int name_length, zval *property, int access_type TSRMLS_DC) /* {{{ */
{
	return zend_u_declare_property_ex(ce, type, name, name_length, property, access_type, NULL_ZSTR, 0 TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_declare_property(zend_class_entry *ce, char *name, int name_length, zval *property, int access_type TSRMLS_DC) /* {{{ */
{
	if (UG(unicode)) {
		zstr uname;
		int ret;
		ALLOCA_FLAG(use_heap)

		uname.u = do_alloca(UBYTES(name_length+1), use_heap);
		u_charsToUChars(name, uname.u, name_length+1);
		ret = zend_u_declare_property_ex(ce, IS_UNICODE, uname, name_length, property, access_type, NULL_ZSTR, 0 TSRMLS_CC);
		free_alloca(uname.u, use_heap);
		return ret;
	} else {
		return zend_u_declare_property_ex(ce, IS_STRING, ZSTR(name), name_length, property, access_type, NULL_ZSTR, 0 TSRMLS_CC);
	}
}
/* }}} */

ZEND_API int zend_declare_property_null(zend_class_entry *ce, char *name, int name_length, int access_type TSRMLS_DC) /* {{{ */
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

ZEND_API int zend_declare_property_bool(zend_class_entry *ce, char *name, int name_length, long value, int access_type TSRMLS_DC) /* {{{ */
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

ZEND_API int zend_declare_property_long(zend_class_entry *ce, char *name, int name_length, long value, int access_type TSRMLS_DC) /* {{{ */
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

ZEND_API int zend_declare_property_double(zend_class_entry *ce, char *name, int name_length, double value, int access_type TSRMLS_DC) /* {{{ */
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

ZEND_API int zend_declare_property_string(zend_class_entry *ce, char *name, int name_length, char *value, int access_type TSRMLS_DC) /* {{{ */
{
	zval *property;
	int len = strlen(value);

	if (ce->type & ZEND_INTERNAL_CLASS) {
		ALLOC_PERMANENT_ZVAL(property);
		if (UG(unicode)) {
			Z_TYPE_P(property) = IS_UNICODE;
			Z_USTRVAL_P(property) = malloc(UBYTES(len+1));
			u_charsToUChars(value, Z_USTRVAL_P(property), len+1);
			Z_USTRLEN_P(property) = len;
		} else {
			ZVAL_STRINGL(property, zend_strndup(value, len), len, 0);
		}
	} else {
		ALLOC_ZVAL(property);
		ZVAL_ASCII_STRINGL(property, value, len, 1);
	}
	INIT_PZVAL(property);
	return zend_declare_property(ce, name, name_length, property, access_type TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_declare_property_stringl(zend_class_entry *ce, char *name, int name_length, char *value, int value_len, int access_type TSRMLS_DC) /* {{{ */
{
	zval *property;

	if (ce->type & ZEND_INTERNAL_CLASS) {
		ALLOC_PERMANENT_ZVAL(property);
		if (UG(unicode)) {
			Z_TYPE_P(property) = IS_UNICODE;
			Z_USTRVAL_P(property) = malloc(UBYTES(value_len+1));
			u_charsToUChars(value, Z_USTRVAL_P(property), value_len+1);
			Z_USTRLEN_P(property) = value_len;
		} else {
			ZVAL_STRINGL(property, zend_strndup(value, value_len), value_len, 0);
		}
	} else {
		ALLOC_ZVAL(property);
		ZVAL_ASCII_STRINGL(property, value, value_len, 1);
	}
	INIT_PZVAL(property);
	return zend_declare_property(ce, name, name_length, property, access_type TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_declare_class_constant(zend_class_entry *ce, char *name, size_t name_length, zval *value TSRMLS_DC) /* {{{ */
{
	return zend_ascii_hash_update(&ce->constants_table, name, name_length+1, &value, sizeof(zval *), NULL);
}
/* }}} */

ZEND_API int zend_declare_class_constant_null(zend_class_entry *ce, char *name, size_t name_length TSRMLS_DC) /* {{{ */
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

ZEND_API int zend_declare_class_constant_long(zend_class_entry *ce, char *name, size_t name_length, long value TSRMLS_DC) /* {{{ */
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

ZEND_API int zend_declare_class_constant_bool(zend_class_entry *ce, char *name, size_t name_length, zend_bool value TSRMLS_DC) /* {{{ */
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

ZEND_API int zend_declare_class_constant_double(zend_class_entry *ce, char *name, size_t name_length, double value TSRMLS_DC) /* {{{ */
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

ZEND_API int zend_declare_class_constant_stringl(zend_class_entry *ce, char *name, size_t name_length, char *value, size_t value_length TSRMLS_DC) /* {{{ */
{
	zval *constant;

	if (ce->type & ZEND_INTERNAL_CLASS) {
		ALLOC_PERMANENT_ZVAL(constant);
		if (UG(unicode)) {
			Z_TYPE_P(constant) = IS_UNICODE;
			Z_USTRVAL_P(constant) = malloc(UBYTES(value_length+1));
			u_charsToUChars(value, Z_USTRVAL_P(constant), value_length+1);
			Z_USTRLEN_P(constant) = value_length;
		} else {
			ZVAL_STRINGL(constant, zend_strndup(value, value_length), value_length, 0);
		}
	} else {
		ALLOC_ZVAL(constant);
		ZVAL_ASCII_STRINGL(constant, value, value_length, 1);
	}
	INIT_PZVAL(constant);
	return zend_declare_class_constant(ce, name, name_length, constant TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_declare_class_constant_string(zend_class_entry *ce, char *name, size_t name_length, char *value TSRMLS_DC) /* {{{ */
{
	return zend_declare_class_constant_stringl(ce, name, name_length, value, strlen(value) TSRMLS_CC);
}
/* }}} */

ZEND_API void zend_update_property(zend_class_entry *scope, zval *object, char *name, int name_length, zval *value TSRMLS_DC) /* {{{ */
{
	zval *property;
	zend_class_entry *old_scope = EG(scope);

	EG(scope) = scope;

	if (!Z_OBJ_HT_P(object)->write_property) {
		zstr class_name;
		zend_uint class_name_len;

		zend_get_object_classname(object, &class_name, &class_name_len TSRMLS_CC);

		zend_error(E_CORE_ERROR, "Property %s of class %v cannot be updated", name, class_name.v);
	}
	MAKE_STD_ZVAL(property);
	ZVAL_ASCII_STRINGL(property, name, name_length, 1);
	Z_OBJ_HT_P(object)->write_property(object, property, value TSRMLS_CC);
	zval_ptr_dtor(&property);

	EG(scope) = old_scope;
}
/* }}} */

ZEND_API void zend_update_property_null(zend_class_entry *scope, zval *object, char *name, int name_length TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_NULL(tmp);
	zend_update_property(scope, object, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API void zend_update_property_bool(zend_class_entry *scope, zval *object, char *name, int name_length, long value TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_BOOL(tmp, value);
	zend_update_property(scope, object, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API void zend_update_property_long(zend_class_entry *scope, zval *object, char *name, int name_length, long value TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_LONG(tmp, value);
	zend_update_property(scope, object, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API void zend_update_property_double(zend_class_entry *scope, zval *object, char *name, int name_length, double value TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_DOUBLE(tmp, value);
	zend_update_property(scope, object, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API void zend_update_property_string(zend_class_entry *scope, zval *object, char *name, int name_length, char *value TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_STRING(tmp, value, 1);
	zend_update_property(scope, object, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API void zend_update_property_stringl(zend_class_entry *scope, zval *object, char *name, int name_length, char *value, int value_len TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_STRINGL(tmp, value, value_len, 1);
	zend_update_property(scope, object, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API void zend_update_property_ascii_string(zend_class_entry *scope, zval *object, char *name, int name_length, char *value TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_ASCII_STRING(tmp, value, 1);
	zend_update_property(scope, object, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API void zend_update_property_ascii_stringl(zend_class_entry *scope, zval *object, char *name, int name_length, char *value, int value_len TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_ASCII_STRINGL(tmp, value, value_len, 1);
	zend_update_property(scope, object, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API void zend_update_property_rt_string(zend_class_entry *scope, zval *object, char *name, int name_length, char *value TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_RT_STRING(tmp, value, 1);
	zend_update_property(scope, object, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API void zend_update_property_rt_stringl(zend_class_entry *scope, zval *object, char *name, int name_length, char *value, int value_len TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_RT_STRINGL(tmp, value, value_len, 1);
	zend_update_property(scope, object, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API void zend_update_property_unicode(zend_class_entry *scope, zval *object, char *name, int name_length, UChar *value TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_UNICODE(tmp, value, 1);
	zend_update_property(scope, object, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API void zend_update_property_unicodel(zend_class_entry *scope, zval *object, char *name, int name_length, UChar *value, int value_len TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_UNICODEL(tmp, value, value_len, 1);
	zend_update_property(scope, object, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_update_static_property(zend_class_entry *scope, char *name, int name_length, zval *value TSRMLS_DC) /* {{{ */
{
	zval **property;
	zend_class_entry *old_scope = EG(scope);

	EG(scope) = scope;
	property = zend_std_get_static_property(scope, IS_STRING, ZSTR(name), name_length, 0 TSRMLS_CC);
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

ZEND_API int zend_update_static_property_null(zend_class_entry *scope, char *name, int name_length TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_NULL(tmp);
	return zend_update_static_property(scope, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_update_static_property_bool(zend_class_entry *scope, char *name, int name_length, long value TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_BOOL(tmp, value);
	return zend_update_static_property(scope, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_update_static_property_long(zend_class_entry *scope, char *name, int name_length, long value TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_LONG(tmp, value);
	return zend_update_static_property(scope, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_update_static_property_double(zend_class_entry *scope, char *name, int name_length, double value TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_DOUBLE(tmp, value);
	return zend_update_static_property(scope, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_update_static_property_string(zend_class_entry *scope, char *name, int name_length, char *value TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_STRING(tmp, value, 1);
	return zend_update_static_property(scope, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_update_static_property_stringl(zend_class_entry *scope, char *name, int name_length, char *value, int value_len TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_STRINGL(tmp, value, value_len, 1);
	return zend_update_static_property(scope, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_update_static_property_ascii_string(zend_class_entry *scope, char *name, int name_length, char *value TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_ASCII_STRING(tmp, value, 1);
	return zend_update_static_property(scope, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_update_static_property_ascii_stringl(zend_class_entry *scope, char *name, int name_length, char *value, int value_len TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_ASCII_STRINGL(tmp, value, value_len, 1);
	return zend_update_static_property(scope, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_update_static_property_rt_string(zend_class_entry *scope, char *name, int name_length, char *value TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_RT_STRING(tmp, value, 1);
	return zend_update_static_property(scope, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_update_static_property_rt_stringl(zend_class_entry *scope, char *name, int name_length, char *value, int value_len TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_RT_STRINGL(tmp, value, value_len, 1);
	return zend_update_static_property(scope, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_update_static_property_unicode(zend_class_entry *scope, char *name, int name_length, UChar *value TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_UNICODE(tmp, value, 1);
	return zend_update_static_property(scope, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API int zend_update_static_property_unicodel(zend_class_entry *scope, char *name, int name_length, UChar *value, int value_len TSRMLS_DC) /* {{{ */
{
	zval *tmp;

	ALLOC_ZVAL(tmp);
	Z_UNSET_ISREF_P(tmp);
	Z_SET_REFCOUNT_P(tmp, 0);
	ZVAL_UNICODEL(tmp, value, value_len, 1);
	return zend_update_static_property(scope, name, name_length, tmp TSRMLS_CC);
}
/* }}} */

ZEND_API zval *zend_read_property(zend_class_entry *scope, zval *object, char *name, int name_length, zend_bool silent TSRMLS_DC) /* {{{ */
{
	zval *property, *value;
	zend_class_entry *old_scope = EG(scope);

	EG(scope) = scope;

	if (!Z_OBJ_HT_P(object)->read_property) {
		zstr class_name;
		zend_uint class_name_len;

		zend_get_object_classname(object, &class_name, &class_name_len TSRMLS_CC);
		zend_error(E_CORE_ERROR, "Property %s of class %v cannot be read", name, class_name.v);
	}

	MAKE_STD_ZVAL(property);
	ZVAL_ASCII_STRINGL(property, name, name_length, 1);
	value = Z_OBJ_HT_P(object)->read_property(object, property, silent?BP_VAR_IS:BP_VAR_R TSRMLS_CC);
	zval_ptr_dtor(&property);

	EG(scope) = old_scope;
	return value;
}
/* }}} */

ZEND_API zval *zend_read_static_property(zend_class_entry *scope, char *name, int name_length, zend_bool silent TSRMLS_DC) /* {{{ */
{
	zval **property;
	zend_class_entry *old_scope = EG(scope);

	EG(scope) = scope;
	property = zend_std_get_static_property(scope, IS_STRING, ZSTR(name), name_length, silent TSRMLS_CC);
	EG(scope) = old_scope;

	return property?*property:NULL;
}
/* }}} */

/*
 * Return the string type that all the passed in types should be converted to.
 * If none of the types are string types, IS_UNICODE or IS_STRING is returned,
 * depending on the Unicode semantics switch.
 */
ZEND_API zend_uchar zend_get_unified_string_type(int num_args TSRMLS_DC, ...) /* {{{ */
{
	va_list ap;
	int best_type = ZEND_STR_TYPE;
	int type;

	if (num_args <= 0) return (zend_uchar)-1;

#ifdef ZTS
	va_start(ap, TSRMLS_C);
#else
	va_start(ap, num_args);
#endif
	while (num_args--) {
		type = va_arg(ap, int);
		if (type == IS_UNICODE) {
			best_type = IS_UNICODE;
			break;
		} else if (type == IS_STRING) {
			best_type = IS_STRING;
		}
	}
	va_end(ap);

	return best_type;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
