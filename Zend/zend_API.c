/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2003 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
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


#include "zend.h"
#include "zend_execute.h"
#include "zend_API.h"
#include "zend_modules.h"
#include "zend_constants.h"

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

/* these variables are true statics/globals, and have to be mutex'ed on every access */
static int module_count=0;
ZEND_API HashTable module_registry;

/* this function doesn't check for too many parameters */
ZEND_API int zend_get_parameters(int ht, int param_count, ...)
{
	void **p;
	int arg_count;
	va_list ptr;
	zval **param, *param_ptr;
	TSRMLS_FETCH();

	p = EG(argument_stack).top_element-2;
	arg_count = (ulong) *p;

	if (param_count>arg_count) {
		return FAILURE;
	}

	va_start(ptr, param_count);

	while (param_count-->0) {
		param = va_arg(ptr, zval **);
		param_ptr = *(p-arg_count);
		if (!PZVAL_IS_REF(param_ptr) && param_ptr->refcount>1) {
			zval *new_tmp;

			ALLOC_ZVAL(new_tmp);
			*new_tmp = *param_ptr;
			zval_copy_ctor(new_tmp);
			INIT_PZVAL(new_tmp);
			param_ptr = new_tmp;
			((zval *) *(p-arg_count))->refcount--;
			*(p-arg_count) = param_ptr;
		}
		*param = param_ptr;
		arg_count--;
	}
	va_end(ptr);

	return SUCCESS;
}


ZEND_API int _zend_get_parameters_array(int ht, int param_count, zval **argument_array TSRMLS_DC)
{
	void **p;
	int arg_count;
	zval *param_ptr;

	p = EG(argument_stack).top_element-2;
	arg_count = (ulong) *p;

	if (param_count>arg_count) {
		return FAILURE;
	}

	while (param_count-->0) {
		param_ptr = *(p-arg_count);
		if (!PZVAL_IS_REF(param_ptr) && param_ptr->refcount>1) {
			zval *new_tmp;

			ALLOC_ZVAL(new_tmp);
			*new_tmp = *param_ptr;
			zval_copy_ctor(new_tmp);
			INIT_PZVAL(new_tmp);
			param_ptr = new_tmp;
			((zval *) *(p-arg_count))->refcount--;
			*(p-arg_count) = param_ptr;
		}
		*(argument_array++) = param_ptr;
		arg_count--;
	}

	return SUCCESS;
}




/* Zend-optimized Extended functions */
/* this function doesn't check for too many parameters */
ZEND_API int zend_get_parameters_ex(int param_count, ...)
{
	void **p;
	int arg_count;
	va_list ptr;
	zval ***param;
	TSRMLS_FETCH();

	p = EG(argument_stack).top_element-2;
	arg_count = (ulong) *p;

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


ZEND_API int _zend_get_parameters_array_ex(int param_count, zval ***argument_array TSRMLS_DC)
{
	void **p;
	int arg_count;

	p = EG(argument_stack).top_element-2;
	arg_count = (ulong) *p;

	if (param_count>arg_count) {
		return FAILURE;
	}

	while (param_count-->0) {
		*(argument_array++) = (zval **) p-(arg_count--);
	}

	return SUCCESS;
}


ZEND_API void zend_wrong_param_count(TSRMLS_D)
{
	zend_error(E_WARNING, "Wrong parameter count for %s()", get_active_function_name(TSRMLS_C));
}


/* Argument parsing API -- andrei */

ZEND_API char *zend_zval_type_name(zval *arg)
{
	switch (Z_TYPE_P(arg)) {
		case IS_NULL:
			return "null";

		case IS_LONG:
			return "integer";

		case IS_DOUBLE:
			return "double";

		case IS_STRING:
			return "string";

		case IS_ARRAY:
			return "array";

		case IS_OBJECT:
			return "object";

		case IS_BOOL:
			return "boolean";

		case IS_RESOURCE:
			return "resource";

		default:
			return "unknown";
	}
}

ZEND_API zend_class_entry *zend_get_class_entry(zval *zobject)
{
	if (Z_OBJ_HT_P(zobject)->get_class_entry) {
		TSRMLS_FETCH();
		return Z_OBJ_HT_P(zobject)->get_class_entry(zobject TSRMLS_CC);
	} else {
		zend_error(E_ERROR, "Class entry requested for an object without PHP class");
		return NULL;
	}
}

static int zend_check_class(zval *obj, zend_class_entry *expected_ce)
{
	zend_class_entry *ce;

	if (Z_TYPE_P(obj) != IS_OBJECT) {
		return 0;
	}

	for (ce = Z_OBJCE_P(obj); ce != NULL; ce = ce->parent) {
		if (ce == expected_ce) {
			return 1;
		}
	}

	return 0;
}

static char *zend_parse_arg_impl(zval **arg, va_list *va, char **spec)
{
	char *spec_walk = *spec;
	char c = *spec_walk++;
	int return_null = 0;

	while (*spec_walk == '/' || *spec_walk == '!') {
		if (*spec_walk == '/') {
			SEPARATE_ZVAL_IF_NOT_REF(arg);
		} else if (*spec_walk == '!' && Z_TYPE_PP(arg) == IS_NULL) {
			return_null = 1;
		}
		spec_walk++;
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

							if ((type = is_numeric_string(Z_STRVAL_PP(arg), Z_STRLEN_PP(arg), p, &d, 0)) == 0) {
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

							if ((type = is_numeric_string(Z_STRVAL_PP(arg), Z_STRLEN_PP(arg), &l, p, 0)) == 0) {
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
						*p = Z_STRVAL_PP(arg);
						*pl = Z_STRLEN_PP(arg);
						break;

					case IS_ARRAY:
					case IS_OBJECT:
					case IS_RESOURCE:
					default:
						return "string";
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
				if (Z_TYPE_PP(arg) != IS_RESOURCE) {
					if (Z_TYPE_PP(arg) == IS_NULL && return_null) {
						*p = NULL;
					} else {
						return "resource";
					}
				} else
					*p = *arg;
			}
			break;

		case 'a':
			{
				zval **p = va_arg(*va, zval **);
				if (Z_TYPE_PP(arg) != IS_ARRAY) {
					if (Z_TYPE_PP(arg) == IS_NULL && return_null) {
						*p = NULL;
					} else {
						return "array";
					}
				} else
					*p = *arg;
			}
			break;

		case 'o':
			{
				zval **p = va_arg(*va, zval **);
				if (Z_TYPE_PP(arg) != IS_OBJECT) {
					if (Z_TYPE_PP(arg) == IS_NULL && return_null) {
						*p = NULL;
					} else {
						return "object";
					}
				} else
					*p = *arg;
			}
			break;

		case 'O':
			{
				zval **p = va_arg(*va, zval **);
				zend_class_entry *ce = va_arg(*va, zend_class_entry *);
				if (!zend_check_class(*arg, ce)) {
					if (Z_TYPE_PP(arg) == IS_NULL && return_null) {
						*p = NULL;
					} else {
						return ce->name;
					}
				} else
					*p = *arg;
			}
			break;

		case 'z':
			{
				zval **p = va_arg(*va, zval **);
				if (Z_TYPE_PP(arg) == IS_NULL && return_null) {
					*p = NULL;
				} else {
					*p = *arg;
				}
			}
			break;

		default:
			return "unknown";
	}

	*spec = spec_walk;

	return NULL;
}

static int zend_parse_arg(int arg_num, zval **arg, va_list *va, char **spec, int quiet TSRMLS_DC)
{
	char *expected_type = NULL;
	char buf[1024];

	expected_type = zend_parse_arg_impl(arg, va, spec);
	if (expected_type) {
		if (!quiet) {
			snprintf(buf, sizeof(buf)-1, "%s() expects parameter %d to be %s, %s given",
					get_active_function_name(TSRMLS_C), arg_num, expected_type,
					zend_zval_type_name(*arg));
			buf[sizeof(buf)-1] = '\0';
			zend_error(E_WARNING, buf);
		}
		return FAILURE;
	}
	
	return SUCCESS;
}

static int zend_parse_va_args(int num_args, char *type_spec, va_list *va, int flags TSRMLS_DC)
{
	char *spec_walk;
	char buf[1024];
	int c, i;
	int min_num_args = -1;
	int max_num_args = 0;
	zval **arg;
	void **p;
	int arg_count;
	int quiet = flags & ZEND_PARSE_PARAMS_QUIET;

	for (spec_walk = type_spec; *spec_walk; spec_walk++) {
		c = *spec_walk;
		switch (c) {
			case 'l': case 'd':
			case 's': case 'b':
			case 'r': case 'a':
			case 'o': case 'O':
			case 'z':
				max_num_args++;
				break;

			case '|':
				min_num_args = max_num_args;
				break;

			case '/':
			case '!':
				/* Pass */
				break;

			default:
				if (!quiet) {
					zend_error(E_WARNING, "%s(): bad type specifier while parsing parameters", get_active_function_name(TSRMLS_C));
				}
				return FAILURE;
		}
	}

	if (min_num_args < 0) {
		min_num_args = max_num_args;
	}

	if (num_args < min_num_args || num_args > max_num_args) {
		if (!quiet) {
			sprintf(buf, "%s() expects %s %d parameter%s, %d given",
					get_active_function_name(TSRMLS_C),
					min_num_args == max_num_args ? "exactly" : num_args < min_num_args ? "at least" : "at most",
					num_args < min_num_args ? min_num_args : max_num_args,
					(num_args < min_num_args ? min_num_args : max_num_args) == 1 ? "" : "s",
					num_args);
			zend_error(E_WARNING, buf);
		}
		return FAILURE;
	}

	p = EG(argument_stack).top_element-2;
	arg_count = (ulong) *p;

	if (num_args > arg_count) {
		zend_error(E_WARNING, "%s(): could not obtain parameters for parsing",
				   get_active_function_name(TSRMLS_C));
		return FAILURE;
	}

	i = 0;
	while (num_args-- > 0) {
		arg = (zval **) p - (arg_count-i);
		if (*type_spec == '|') {
			type_spec++;
		}
		if (zend_parse_arg(i+1, arg, va, &type_spec, quiet TSRMLS_CC) == FAILURE) {
			return FAILURE;
		}
		i++;
	}

	return SUCCESS;
}

ZEND_API int zend_parse_parameters_ex(int flags, int num_args TSRMLS_DC, char *type_spec, ...)
{
	va_list va;
	int retval;
	
	va_start(va, type_spec);
	retval = zend_parse_va_args(num_args, type_spec, &va, flags TSRMLS_CC);
	va_end(va);

	return retval;
}

ZEND_API int zend_parse_parameters(int num_args TSRMLS_DC, char *type_spec, ...)
{
	va_list va;
	int retval;
	
	va_start(va, type_spec);
	retval = zend_parse_va_args(num_args, type_spec, &va, 0 TSRMLS_CC);
	va_end(va);

	return retval;
}

/* Argument parsing API -- andrei */


ZEND_API int _array_init(zval *arg ZEND_FILE_LINE_DC)
{
	ALLOC_HASHTABLE_REL(arg->value.ht);

	zend_hash_init(arg->value.ht, 0, NULL, ZVAL_PTR_DTOR, 0);
	arg->type = IS_ARRAY;
	return SUCCESS;
}


ZEND_API int _object_and_properties_init(zval *arg, zend_class_entry *class_type, HashTable *properties ZEND_FILE_LINE_DC TSRMLS_DC)
{
	zval *tmp;
	zend_object *object;

	if (!class_type->constants_updated) {
		zend_hash_apply_with_argument(&class_type->default_properties, (apply_func_arg_t) zval_update_constant, (void *) 1 TSRMLS_CC);
		zend_hash_apply_with_argument(class_type->static_members, (apply_func_arg_t) zval_update_constant, (void *) 1 TSRMLS_CC);
		class_type->constants_updated = 1;
	}
	
	arg->type = IS_OBJECT;
	if (class_type->create_object == NULL) {
		arg->value.obj = zend_objects_new(&object, class_type);
		if (properties) {
			object->properties = properties;
		} else {
			ALLOC_HASHTABLE_REL(object->properties);
			zend_hash_init(object->properties, 0, NULL, ZVAL_PTR_DTOR, 0);
			zend_hash_copy(object->properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
		}
	} else {
		arg->value.obj = class_type->create_object(class_type TSRMLS_CC);
	}
	return SUCCESS;
}

ZEND_API int _object_init_ex(zval *arg, zend_class_entry *class_type ZEND_FILE_LINE_DC TSRMLS_DC)
{
	return _object_and_properties_init(arg, class_type, 0 ZEND_FILE_LINE_RELAY_CC TSRMLS_CC);
}

ZEND_API int _object_init(zval *arg ZEND_FILE_LINE_DC TSRMLS_DC)
{
	return _object_init_ex(arg, zend_standard_class_def ZEND_FILE_LINE_RELAY_CC TSRMLS_CC);
}


ZEND_API int add_assoc_function(zval *arg, char *key, void (*function_ptr)(INTERNAL_FUNCTION_PARAMETERS))
{
	zend_error(E_WARNING, "add_assoc_function() is no longer supported");
	return FAILURE;
}


ZEND_API int add_assoc_long_ex(zval *arg, char *key, uint key_len, long n)
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_LONG(tmp, n);
	
	return zend_hash_update(Z_ARRVAL_P(arg), key, key_len, (void *) &tmp, sizeof(zval *), NULL);
}

ZEND_API int add_assoc_null_ex(zval *arg, char *key, uint key_len)
{
	zval *tmp;
	
	MAKE_STD_ZVAL(tmp);
	ZVAL_NULL(tmp);
	
	return zend_hash_update(Z_ARRVAL_P(arg), key, key_len, (void *) &tmp, sizeof(zval *), NULL);
}

ZEND_API int add_assoc_bool_ex(zval *arg, char *key, uint key_len, int b)
{
	zval *tmp;
	
	MAKE_STD_ZVAL(tmp);
	ZVAL_BOOL(tmp, b);

	return zend_hash_update(Z_ARRVAL_P(arg), key, key_len, (void *) &tmp, sizeof(zval *), NULL);
}

ZEND_API int add_assoc_resource_ex(zval *arg, char *key, uint key_len, int r)
{
	zval *tmp;
	
	MAKE_STD_ZVAL(tmp);
	ZVAL_RESOURCE(tmp, r);
	
	return zend_hash_update(Z_ARRVAL_P(arg), key, key_len, (void *) &tmp, sizeof(zval *), NULL);
}


ZEND_API int add_assoc_double_ex(zval *arg, char *key, uint key_len, double d)
{
	zval *tmp;
	
	MAKE_STD_ZVAL(tmp);
	ZVAL_DOUBLE(tmp, d);

	return zend_hash_update(Z_ARRVAL_P(arg), key, key_len, (void *) &tmp, sizeof(zval *), NULL);
}


ZEND_API int add_assoc_string_ex(zval *arg, char *key, uint key_len, char *str, int duplicate)
{
	zval *tmp;
	
	MAKE_STD_ZVAL(tmp);
	ZVAL_STRING(tmp, str, duplicate);

	return zend_hash_update(Z_ARRVAL_P(arg), key, key_len, (void *) &tmp, sizeof(zval *), NULL);
}


ZEND_API int add_assoc_stringl_ex(zval *arg, char *key, uint key_len, char *str, uint length, int duplicate)
{
	zval *tmp;
	
	MAKE_STD_ZVAL(tmp);
	ZVAL_STRINGL(tmp, str, length, duplicate);

	return zend_hash_update(Z_ARRVAL_P(arg), key, key_len, (void *) &tmp, sizeof(zval *), NULL);
}

ZEND_API int add_assoc_zval_ex(zval *arg, char *key, uint key_len, zval *value)
{
	return zend_hash_update(Z_ARRVAL_P(arg), key, key_len, (void *) &value, sizeof(zval *), NULL);
}


ZEND_API int add_index_long(zval *arg, uint index, long n)
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_LONG(tmp, n);

	return zend_hash_index_update(Z_ARRVAL_P(arg), index, (void *) &tmp, sizeof(zval *), NULL);
}


ZEND_API int add_index_null(zval *arg, uint index)
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_NULL(tmp);

	return zend_hash_index_update(Z_ARRVAL_P(arg), index, (void *) &tmp, sizeof(zval *), NULL);
}

ZEND_API int add_index_bool(zval *arg, uint index, int b)
{
	zval *tmp;
	
	MAKE_STD_ZVAL(tmp);
	ZVAL_BOOL(tmp, b);
	
	return zend_hash_index_update(Z_ARRVAL_P(arg), index, (void *) &tmp, sizeof(zval *), NULL);
}


ZEND_API int add_index_resource(zval *arg, uint index, int r)
{
	zval *tmp;
	
	MAKE_STD_ZVAL(tmp);
	ZVAL_RESOURCE(tmp, r);
	
	return zend_hash_index_update(Z_ARRVAL_P(arg), index, (void *) &tmp, sizeof(zval *), NULL);
}


ZEND_API int add_index_double(zval *arg, uint index, double d)
{
	zval *tmp;
	
	MAKE_STD_ZVAL(tmp);
	ZVAL_DOUBLE(tmp, d);
	
	return zend_hash_index_update(Z_ARRVAL_P(arg), index, (void *) &tmp, sizeof(zval *), NULL);
}


ZEND_API int add_index_string(zval *arg, uint index, char *str, int duplicate)
{
	zval *tmp;
	
	MAKE_STD_ZVAL(tmp);
	ZVAL_STRING(tmp, str, duplicate);

	return zend_hash_index_update(Z_ARRVAL_P(arg), index, (void *) &tmp, sizeof(zval *), NULL);
}


ZEND_API int add_index_stringl(zval *arg, uint index, char *str, uint length, int duplicate)
{
	zval *tmp;
	
	MAKE_STD_ZVAL(tmp);
	ZVAL_STRINGL(tmp, str, length, duplicate);
	
	return zend_hash_index_update(Z_ARRVAL_P(arg), index, (void *) &tmp, sizeof(zval *), NULL);
}


ZEND_API int add_index_zval(zval *arg, uint index, zval *value)
{
	return zend_hash_index_update(Z_ARRVAL_P(arg), index, (void *) &value, sizeof(zval *), NULL);
}


ZEND_API int add_next_index_long(zval *arg, long n)
{
	zval *tmp;
	
	MAKE_STD_ZVAL(tmp);
	ZVAL_LONG(tmp, n);
	
	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp, sizeof(zval *), NULL);
}


ZEND_API int add_next_index_null(zval *arg)
{
	zval *tmp;
	
	MAKE_STD_ZVAL(tmp);
	ZVAL_NULL(tmp);
	
	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp, sizeof(zval *), NULL);
}


ZEND_API int add_next_index_bool(zval *arg, int b)
{
	zval *tmp;
	
	MAKE_STD_ZVAL(tmp);
	ZVAL_BOOL(tmp, b);
	
	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp, sizeof(zval *), NULL);
}


ZEND_API int add_next_index_resource(zval *arg, int r)
{
	zval *tmp;
	
	MAKE_STD_ZVAL(tmp);
	ZVAL_RESOURCE(tmp, r);
	
	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp, sizeof(zval *), NULL);
}


ZEND_API int add_next_index_double(zval *arg, double d)
{
	zval *tmp;
	
	MAKE_STD_ZVAL(tmp);
	ZVAL_DOUBLE(tmp, d);
	
	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp, sizeof(zval *), NULL);
}


ZEND_API int add_next_index_string(zval *arg, char *str, int duplicate)
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_STRING(tmp, str, duplicate);

	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp, sizeof(zval *), NULL);
}


ZEND_API int add_next_index_stringl(zval *arg, char *str, uint length, int duplicate)
{
	zval *tmp;

	MAKE_STD_ZVAL(tmp);
	ZVAL_STRINGL(tmp, str, length, duplicate);

	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &tmp, sizeof(zval *), NULL);
}


ZEND_API int add_next_index_zval(zval *arg, zval *value)
{
	return zend_hash_next_index_insert(Z_ARRVAL_P(arg), &value, sizeof(zval *), NULL);
}


ZEND_API int add_get_assoc_string_ex(zval *arg, char *key, uint key_len, char *str, void **dest, int duplicate)
{
	zval *tmp;
	
	MAKE_STD_ZVAL(tmp);
	ZVAL_STRING(tmp, str, duplicate);
	
	return zend_hash_update(Z_ARRVAL_P(arg), key, key_len, (void *) &tmp, sizeof(zval *), dest);
}


ZEND_API int add_get_assoc_stringl_ex(zval *arg, char *key, uint key_len, char *str, uint length, void **dest, int duplicate)
{
	zval *tmp;
	
	MAKE_STD_ZVAL(tmp);
	ZVAL_STRINGL(tmp, str, length, duplicate);

	return zend_hash_update(Z_ARRVAL_P(arg), key, key_len, (void *) &tmp, sizeof(zval *), dest);
}


ZEND_API int add_get_index_long(zval *arg, uint index, long l, void **dest)
{
	zval *tmp;
	
	MAKE_STD_ZVAL(tmp);
	ZVAL_LONG(tmp, l);
	
	return zend_hash_index_update(Z_ARRVAL_P(arg), index, (void *) &tmp, sizeof(zval *), dest);
}


ZEND_API int add_get_index_double(zval *arg, uint index, double d, void **dest)
{
	zval *tmp;
	
	MAKE_STD_ZVAL(tmp);
	ZVAL_DOUBLE(tmp, d);
	
	return zend_hash_index_update(Z_ARRVAL_P(arg), index, (void *) &tmp, sizeof(zval *), dest);
}


ZEND_API int add_get_index_string(zval *arg, uint index, char *str, void **dest, int duplicate)
{
	zval *tmp;
	
	MAKE_STD_ZVAL(tmp);
	ZVAL_STRING(tmp, str, duplicate);
	
	return zend_hash_index_update(Z_ARRVAL_P(arg), index, (void *) &tmp, sizeof(zval *), dest);
}


ZEND_API int add_get_index_stringl(zval *arg, uint index, char *str, uint length, void **dest, int duplicate)
{
	zval *tmp;
	
	MAKE_STD_ZVAL(tmp);
	ZVAL_STRINGL(tmp, str, length, duplicate);
	
	return zend_hash_index_update(Z_ARRVAL_P(arg), index, (void *) &tmp, sizeof(zval *), dest);
}


ZEND_API int add_property_long_ex(zval *arg, char *key, uint key_len, long n TSRMLS_DC)
{
	zval *tmp;
	zval z_key;

	MAKE_STD_ZVAL(tmp);
	ZVAL_LONG(tmp, n);
	ZVAL_STRINGL(&z_key, key, key_len-1, 0);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, &z_key, tmp TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	return SUCCESS;
}

ZEND_API int add_property_bool_ex(zval *arg, char *key, uint key_len, int b TSRMLS_DC)
{
	zval *tmp;
	zval z_key;

	MAKE_STD_ZVAL(tmp);
	ZVAL_BOOL(tmp, b);

	ZVAL_STRINGL(&z_key, key, key_len-1, 0);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, &z_key, tmp TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	return SUCCESS;
}

ZEND_API int add_property_null_ex(zval *arg, char *key, uint key_len TSRMLS_DC)
{
	zval *tmp;
	zval z_key;
	
	MAKE_STD_ZVAL(tmp);
	ZVAL_NULL(tmp);
	
	ZVAL_STRINGL(&z_key, key, key_len-1, 0);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, &z_key, tmp TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	return SUCCESS;
}

ZEND_API int add_property_resource_ex(zval *arg, char *key, uint key_len, long n TSRMLS_DC)
{
	zval *tmp;
	zval z_key;
	
	MAKE_STD_ZVAL(tmp);
	ZVAL_RESOURCE(tmp, n);
	ZVAL_STRINGL(&z_key, key, key_len-1, 0);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, &z_key, tmp TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	return SUCCESS;
}


ZEND_API int add_property_double_ex(zval *arg, char *key, uint key_len, double d TSRMLS_DC)
{
	zval *tmp;
	zval z_key;

	MAKE_STD_ZVAL(tmp);
	ZVAL_DOUBLE(tmp, d);
	ZVAL_STRINGL(&z_key, key, key_len-1, 0);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, &z_key, tmp TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	return SUCCESS;
}


ZEND_API int add_property_string_ex(zval *arg, char *key, uint key_len, char *str, int duplicate TSRMLS_DC)
{
	zval *tmp;
	zval z_key;

	MAKE_STD_ZVAL(tmp);
	ZVAL_STRING(tmp, str, duplicate);

	ZVAL_STRINGL(&z_key, key, key_len-1, 0);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, &z_key, tmp TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	return SUCCESS;
}

ZEND_API int add_property_stringl_ex(zval *arg, char *key, uint key_len, char *str, uint length, int duplicate TSRMLS_DC)
{
	zval *tmp;
	zval z_key;

	MAKE_STD_ZVAL(tmp);
	ZVAL_STRINGL(tmp, str, length, duplicate);

	ZVAL_STRINGL(&z_key, key, key_len-1, 0);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, &z_key, tmp TSRMLS_CC);
	zval_ptr_dtor(&tmp); /* write_property will add 1 to refcount */
	return SUCCESS;
}

ZEND_API int add_property_zval_ex(zval *arg, char *key, uint key_len, zval *value TSRMLS_DC)
{
	zval z_key;

	ZVAL_STRINGL(&z_key, key, key_len-1, 0);

	Z_OBJ_HANDLER_P(arg, write_property)(arg, &z_key, value TSRMLS_CC);
	return SUCCESS;
}

ZEND_API int zend_startup_module(zend_module_entry *module)
{
	if (module) {
		module->module_number = zend_next_free_module();
		if (module->module_startup_func) {
			TSRMLS_FETCH();

			if (module->module_startup_func(MODULE_PERSISTENT, module->module_number TSRMLS_CC)==FAILURE) {
				zend_error(E_CORE_ERROR,"Unable to start %s module", module->name);
				return FAILURE;
			}
		}
		module->type = MODULE_PERSISTENT;
		zend_register_module(module);
	}
	return SUCCESS;
}


/* registers all functions in *library_functions in the function hash */
int zend_register_functions(zend_class_entry *scope, zend_function_entry *functions, HashTable *function_table, int type TSRMLS_DC)
{
	zend_function_entry *ptr = functions;
	zend_function function, *reg_function;
	zend_internal_function *internal_function = (zend_internal_function *)&function;
	int count=0, unload=0;
	HashTable *target_function_table = function_table;
	int error_type;
	zend_function *ctor = NULL, *dtor = NULL, *clone = NULL;

	if (type==MODULE_PERSISTENT) {
		error_type = E_CORE_WARNING;
	} else {
		error_type = E_WARNING;
	}

	if (!target_function_table) {
		target_function_table = CG(function_table);
	}
	internal_function->type = ZEND_INTERNAL_FUNCTION;
	
	while (ptr->fname) {
		internal_function->handler = ptr->handler;
		internal_function->arg_types = ptr->func_arg_types;
		internal_function->function_name = ptr->fname;
		internal_function->scope = scope;
		internal_function->fn_flags = ZEND_ACC_PUBLIC;
		if (!internal_function->handler) {
			zend_error(error_type, "Null function defined as active function");
			zend_unregister_functions(functions, count, target_function_table TSRMLS_CC);
			return FAILURE;
		}
		if (zend_hash_add(target_function_table, ptr->fname, strlen(ptr->fname)+1, &function, sizeof(zend_function), (void**)&reg_function) == FAILURE) {
			unload=1;
			break;
		}
		if (scope) {
			/*
			 * If it's an old-style constructor, store it only if we don't have
			 * a constructor already.
			 */
			if (!strcmp(ptr->fname, scope->name) && !ctor) {
				ctor = reg_function;
			} else if (!strcmp(ptr->fname, ZEND_CONSTRUCTOR_FUNC_NAME)) {
				ctor = reg_function;
			} else if (!strcmp(ptr->fname, ZEND_DESTRUCTOR_FUNC_NAME)) {
				dtor = reg_function;
			} else if (!strcmp(ptr->fname, ZEND_CLONE_FUNC_NAME)) {
				clone = reg_function;
			}
		}
		ptr++;
		count++;
	}
	if (unload) { /* before unloading, display all remaining bad function in the module */
		while (ptr->fname) {
			if (zend_hash_exists(target_function_table, ptr->fname, strlen(ptr->fname)+1)) {
				zend_error(error_type, "Function registration failed - duplicate name - %s", ptr->fname);
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
	}
	return SUCCESS;
}

/* count=-1 means erase all functions, otherwise, 
 * erase the first count functions
 */
void zend_unregister_functions(zend_function_entry *functions, int count, HashTable *function_table TSRMLS_DC)
{
	zend_function_entry *ptr = functions;
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


ZEND_API int zend_register_module(zend_module_entry *module)
{
	TSRMLS_FETCH();

#if 0
	zend_printf("%s:  Registering module %d\n", module->name, module->module_number);
#endif
	if (module->functions && zend_register_functions(NULL, module->functions, NULL, module->type TSRMLS_CC)==FAILURE) {
		zend_error(E_CORE_WARNING,"%s:  Unable to register functions, unable to load", module->name);
		return FAILURE;
	}
	module->module_started=1;
	return zend_hash_add(&module_registry, module->name, strlen(module->name)+1, (void *)module, sizeof(zend_module_entry), NULL);
}


void module_destructor(zend_module_entry *module)
{
	TSRMLS_FETCH();

	if (module->type == MODULE_TEMPORARY) {
		zend_clean_module_rsrc_dtors(module->module_number TSRMLS_CC);
		clean_module_constants(module->module_number TSRMLS_CC);
		if (module->request_shutdown_func)
			module->request_shutdown_func(module->type, module->module_number TSRMLS_CC);
	}

	if (module->module_started && module->module_shutdown_func) {
#if 0
		zend_printf("%s:  Module shutdown\n", module->name);
#endif
		module->module_shutdown_func(module->type, module->module_number TSRMLS_CC);
	}
	module->module_started=0;
	if (module->functions) {
		zend_unregister_functions(module->functions, -1, NULL TSRMLS_CC);
	}

#if HAVE_LIBDL
	if (module->handle) {
		dlclose(module->handle);
	}
#endif
}


/* call request startup for all modules */
int module_registry_request_startup(zend_module_entry *module TSRMLS_DC)
{
	if (module->request_startup_func) {
#if 0
		zend_printf("%s:  Request startup\n", module->name);
#endif
		if (module->request_startup_func(module->type, module->module_number TSRMLS_CC)==FAILURE) {
			zend_error(E_WARNING, "request_startup() for %s module failed", module->name);
			exit(1);
		}
	}
	return 0;
}


/* for persistent modules - call request shutdown and flag NOT to erase
 * for temporary modules - do nothing, and flag to erase
 */
int module_registry_cleanup(zend_module_entry *module TSRMLS_DC)
{
	switch (module->type) {
		case MODULE_PERSISTENT:
			if (module->request_shutdown_func) {
#if 0
				zend_printf("%s:  Request shutdown\n", module->name);
#endif
				module->request_shutdown_func(module->type, module->module_number TSRMLS_CC);
			}
			return 0;
			break;
		case MODULE_TEMPORARY:
			return 1;
			break;
	}
	return 0;
}


/* return the next free module number */
int zend_next_free_module(void)
{
	return ++module_count;
}

/* If parent_ce is not NULL then it inherits from parent_ce
 * If parent_ce is NULL and parent_name isn't then it looks for the parent and inherits from it
 * If both parent_ce and parent_name are NULL it does a regular class registration
 * If parent_name is specified but not found NULL is returned
 */
ZEND_API zend_class_entry *zend_register_internal_class_ex(zend_class_entry *class_entry, zend_class_entry *parent_ce, char *parent_name TSRMLS_DC)
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
		zend_do_inheritance(register_class, parent_ce);
	}
	return register_class;
}

ZEND_API zend_class_entry *zend_register_internal_class(zend_class_entry *orig_class_entry TSRMLS_DC)
{
	zend_class_entry *class_entry = malloc(sizeof(zend_class_entry));
	char *lowercase_name = zend_strndup(orig_class_entry->name, orig_class_entry->name_length);
	*class_entry = *orig_class_entry;

	zend_str_tolower(lowercase_name, class_entry->name_length);

	class_entry->type = ZEND_INTERNAL_CLASS;
	class_entry->parent = NULL;
	class_entry->refcount = 1;
	class_entry->constants_updated = 0;
	class_entry->ce_flags = 0;
	zend_hash_init(&class_entry->default_properties, 0, NULL, ZVAL_PTR_DTOR, 1);
	zend_hash_init(&class_entry->private_properties, 0, NULL, ZVAL_PTR_DTOR, 1);
	zend_hash_init(&class_entry->protected_properties, 0, NULL, ZVAL_PTR_DTOR, 1);
	class_entry->static_members = (HashTable *) malloc(sizeof(HashTable));
	zend_hash_init(class_entry->static_members, 0, NULL, ZVAL_PTR_DTOR, 1);
	zend_hash_init(&class_entry->constants_table, 0, NULL, ZVAL_PTR_DTOR, 1);
	zend_hash_init(&class_entry->function_table, 0, NULL, ZEND_FUNCTION_DTOR, 1);
	zend_hash_init(&class_entry->class_table, 10, NULL, ZEND_CLASS_DTOR, 1);

	if (class_entry->builtin_functions) {
		zend_register_functions(class_entry, class_entry->builtin_functions, &class_entry->function_table, MODULE_PERSISTENT TSRMLS_CC);
	}

	zend_hash_update(CG(class_table), lowercase_name, class_entry->name_length+1, &class_entry, sizeof(zend_class_entry *), NULL);
	free(lowercase_name);
	return class_entry;
}


ZEND_API zend_module_entry *zend_get_module(int module_number)
{
	zend_module_entry *module;

	if (zend_hash_index_find(&module_registry, module_number, (void **) &module)==SUCCESS) {
		return module;
	} else {
		return NULL;
	}
}

ZEND_API int zend_set_hash_symbol(zval *symbol, char *name, int name_length,
                                  zend_bool is_ref, int num_symbol_tables, ...)
{
    HashTable  *symbol_table;
    va_list     symbol_table_list;

    if (num_symbol_tables <= 0) return FAILURE;

    symbol->is_ref = is_ref;

    va_start(symbol_table_list, num_symbol_tables);
    while (num_symbol_tables-- > 0) {
        symbol_table = va_arg(symbol_table_list, HashTable *);
        zend_hash_update(symbol_table, name, name_length + 1, &symbol, sizeof(zval *), NULL);
        zval_add_ref(&symbol);
    }
    va_end(symbol_table_list);
    return SUCCESS;
}




/* Disabled functions support */

ZEND_API ZEND_FUNCTION(display_disabled_function)
{
	zend_error(E_WARNING, "%s() has been disabled for security reasons", get_active_function_name(TSRMLS_C));
}


static zend_function_entry disabled_function[] =  {
	ZEND_FE(display_disabled_function,			NULL)
	{ NULL, NULL, NULL }
};


ZEND_API int zend_disable_function(char *function_name, uint function_name_length TSRMLS_DC)
{
	if (zend_hash_del(CG(function_table), function_name, function_name_length+1)==FAILURE) {
		return FAILURE;
	}
	disabled_function[0].fname = function_name;
	return zend_register_functions(NULL, disabled_function, CG(function_table), MODULE_PERSISTENT TSRMLS_CC);
}

zend_bool zend_is_callable(zval *callable, zend_bool syntax_only, char **callable_name)
{
	char *lcname;
	zend_bool retval = 0;
	TSRMLS_FETCH();

	switch (Z_TYPE_P(callable)) {
		case IS_STRING:
			if (callable_name)
				*callable_name = estrndup(Z_STRVAL_P(callable), Z_STRLEN_P(callable));

			if (syntax_only)
				return 1;

			lcname = estrndup(Z_STRVAL_P(callable), Z_STRLEN_P(callable));
			zend_str_tolower(lcname, Z_STRLEN_P(callable));
			if (zend_hash_exists(EG(function_table), lcname, Z_STRLEN_P(callable)+1)) 
				retval = 1;
			efree(lcname);
			break;

		case IS_ARRAY:
			{
				zval **method;
				zval **obj;
				zend_class_entry *ce = NULL, **pce;
				zend_uint callable_name_len;
				
				if (zend_hash_num_elements(Z_ARRVAL_P(callable)) == 2 &&
					zend_hash_index_find(Z_ARRVAL_P(callable), 0, (void **) &obj) == SUCCESS &&
					zend_hash_index_find(Z_ARRVAL_P(callable), 1, (void **) &method) == SUCCESS &&
					(Z_TYPE_PP(obj) == IS_OBJECT || Z_TYPE_PP(obj) == IS_STRING) &&
					Z_TYPE_PP(method) == IS_STRING) {

					if (Z_TYPE_PP(obj) == IS_STRING) {
						if (callable_name) {
							char *ptr;

							callable_name_len = Z_STRLEN_PP(obj) + Z_STRLEN_PP(method) + sizeof("::");
							ptr = *callable_name = emalloc(callable_name_len);
							memcpy(ptr, Z_STRVAL_PP(obj), Z_STRLEN_PP(obj));
							ptr += Z_STRLEN_PP(obj);
							memcpy(ptr, "::", sizeof("::") - 1);
							ptr += sizeof("::") - 1;
							memcpy(ptr, Z_STRVAL_PP(method), Z_STRLEN_PP(method) + 1);
						}

						if (syntax_only)
							return 1;

						lcname = estrndup(Z_STRVAL_PP(obj), Z_STRLEN_PP(obj));
						zend_str_tolower(lcname, Z_STRLEN_PP(obj));
						if (zend_lookup_class(lcname, Z_STRLEN_PP(obj), &pce TSRMLS_CC) == SUCCESS) {
							ce = *pce;
						}
						
						efree(lcname);
					} else {
						ce = Z_OBJCE_PP(obj); /* ??? */

						if (callable_name) {
							char *ptr;

							callable_name_len = ce->name_length + Z_STRLEN_PP(method) + sizeof("::");
							ptr = *callable_name = emalloc(callable_name_len);
							memcpy(ptr, ce->name, ce->name_length);
							ptr += ce->name_length;
							memcpy(ptr, "::", sizeof("::") - 1);
							ptr += sizeof("::") - 1;
							memcpy(ptr, Z_STRVAL_PP(method), Z_STRLEN_PP(method) + 1);
						}

						if (syntax_only)
							return 1;
					}

					if (ce) {
						lcname = estrndup(Z_STRVAL_PP(method), Z_STRLEN_PP(method));
						zend_str_tolower(lcname, Z_STRLEN_PP(method));
						if (zend_hash_exists(&ce->function_table, lcname, Z_STRLEN_PP(method)+1))
							retval = 1;
						efree(lcname);
					}
				} else if (callable_name)
					*callable_name = estrndup("Array", sizeof("Array")-1);
			}
			break;

		default:
			if (callable_name) {
				zval expr_copy;
				int use_copy;

				zend_make_printable_zval(callable, &expr_copy, &use_copy);
				*callable_name = estrndup(Z_STRVAL(expr_copy), Z_STRLEN(expr_copy));
				zval_dtor(&expr_copy);
			}
			break;
	}

	return retval;
}

ZEND_API char *zend_get_module_version(char *module_name)
{
	zend_module_entry *module;

	if (zend_hash_find(&module_registry, module_name, strlen(module_name) + 1,
                       (void**)&module) == FAILURE) {
		return NULL;
	}
    return module->version;
}
