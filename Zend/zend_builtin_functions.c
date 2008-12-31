/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2009 Zend Technologies Ltd. (http://www.zend.com) |
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

/* $Id$ */

#include "zend.h"
#include "zend_API.h"
#include "zend_builtin_functions.h"
#include "zend_constants.h"
#include "zend_ini.h"
#include "zend_exceptions.h"
#include "zend_extensions.h"

#undef ZEND_TEST_EXCEPTIONS

static ZEND_FUNCTION(zend_version);
static ZEND_FUNCTION(func_num_args);
static ZEND_FUNCTION(func_get_arg);
static ZEND_FUNCTION(func_get_args);
static ZEND_NAMED_FUNCTION(zend_if_strlen);
static ZEND_FUNCTION(strcmp);
static ZEND_FUNCTION(strncmp);
static ZEND_FUNCTION(strcasecmp);
static ZEND_FUNCTION(strncasecmp);
static ZEND_FUNCTION(each);
static ZEND_FUNCTION(error_reporting);
static ZEND_FUNCTION(define);
static ZEND_FUNCTION(defined);
static ZEND_FUNCTION(get_class);
static ZEND_FUNCTION(get_parent_class);
static ZEND_FUNCTION(method_exists);
static ZEND_FUNCTION(property_exists);
static ZEND_FUNCTION(class_exists);
static ZEND_FUNCTION(interface_exists);
static ZEND_FUNCTION(function_exists);
#if ZEND_DEBUG
static ZEND_FUNCTION(leak);
#ifdef ZEND_TEST_EXCEPTIONS
static ZEND_FUNCTION(crash);
#endif
#endif
static ZEND_FUNCTION(get_included_files);
static ZEND_FUNCTION(is_subclass_of);
static ZEND_FUNCTION(is_a);
static ZEND_FUNCTION(get_class_vars);
static ZEND_FUNCTION(get_object_vars);
static ZEND_FUNCTION(get_class_methods);
static ZEND_FUNCTION(trigger_error);
static ZEND_FUNCTION(set_error_handler);
static ZEND_FUNCTION(restore_error_handler);
static ZEND_FUNCTION(set_exception_handler);
static ZEND_FUNCTION(restore_exception_handler);
static ZEND_FUNCTION(get_declared_classes);
static ZEND_FUNCTION(get_declared_interfaces);
static ZEND_FUNCTION(get_defined_functions);
static ZEND_FUNCTION(get_defined_vars);
static ZEND_FUNCTION(create_function);
static ZEND_FUNCTION(get_resource_type);
static ZEND_FUNCTION(get_loaded_extensions);
static ZEND_FUNCTION(extension_loaded);
static ZEND_FUNCTION(get_extension_funcs);
static ZEND_FUNCTION(get_defined_constants);
static ZEND_FUNCTION(debug_backtrace);
static ZEND_FUNCTION(debug_print_backtrace);
#if ZEND_DEBUG
static ZEND_FUNCTION(zend_test_func);
#ifdef ZTS
static ZEND_FUNCTION(zend_thread_id);
#endif
#endif

#include "zend_arg_defs.c"


static zend_function_entry builtin_functions[] = {
	ZEND_FE(zend_version,		NULL)
	ZEND_FE(func_num_args,		NULL)
	ZEND_FE(func_get_arg,		NULL)
	ZEND_FE(func_get_args,		NULL)
	{ "strlen", zend_if_strlen, NULL },
	ZEND_FE(strcmp,				NULL)
	ZEND_FE(strncmp,			NULL)
	ZEND_FE(strcasecmp,			NULL)
	ZEND_FE(strncasecmp,		NULL)
	ZEND_FE(each,				first_arg_force_ref)
	ZEND_FE(error_reporting,	NULL)
	ZEND_FE(define,				NULL)
	ZEND_FE(defined,			NULL)
	ZEND_FE(get_class,			NULL)
	ZEND_FE(get_parent_class,	NULL)
	ZEND_FE(method_exists,		NULL)
	ZEND_FE(property_exists,	NULL)
	ZEND_FE(class_exists,		NULL)
	ZEND_FE(interface_exists,	NULL)
	ZEND_FE(function_exists,	NULL)
#if ZEND_DEBUG
	ZEND_FE(leak,				NULL)
#ifdef ZEND_TEST_EXCEPTIONS
	ZEND_FE(crash,				NULL)
#endif
#endif
	ZEND_FE(get_included_files,	NULL)
	ZEND_FALIAS(get_required_files,	get_included_files,		NULL)
	ZEND_FE(is_subclass_of,		NULL)
	ZEND_FE(is_a,				NULL)
	ZEND_FE(get_class_vars,		NULL)
	ZEND_FE(get_object_vars,	NULL)
	ZEND_FE(get_class_methods,	NULL)
	ZEND_FE(trigger_error,		NULL)
	ZEND_FALIAS(user_error,		trigger_error,		NULL)
	ZEND_FE(set_error_handler,		NULL)
	ZEND_FE(restore_error_handler,	NULL)
	ZEND_FE(set_exception_handler,		NULL)
	ZEND_FE(restore_exception_handler,	NULL)
	ZEND_FE(get_declared_classes, NULL)
	ZEND_FE(get_declared_interfaces, NULL)
	ZEND_FE(get_defined_functions, NULL)
	ZEND_FE(get_defined_vars,	NULL)
	ZEND_FE(create_function,	NULL)
	ZEND_FE(get_resource_type,	NULL)
	ZEND_FE(get_loaded_extensions,		NULL)
	ZEND_FE(extension_loaded,			NULL)
	ZEND_FE(get_extension_funcs,		NULL)
	ZEND_FE(get_defined_constants,		NULL)
	ZEND_FE(debug_backtrace, NULL)
	ZEND_FE(debug_print_backtrace, NULL)
#if ZEND_DEBUG
	ZEND_FE(zend_test_func,		NULL)
#ifdef ZTS
	ZEND_FE(zend_thread_id,		NULL)
#endif
#endif
	{ NULL, NULL, NULL }
};


int zend_startup_builtin_functions(TSRMLS_D)
{
	return zend_register_functions(NULL, builtin_functions, NULL, MODULE_PERSISTENT TSRMLS_CC);
}


/* {{{ proto string zend_version(void)
   Get the version of the Zend Engine */
ZEND_FUNCTION(zend_version)
{
	RETURN_STRINGL(ZEND_VERSION, sizeof(ZEND_VERSION)-1, 1);
}
/* }}} */


/* {{{ proto int func_num_args(void)
   Get the number of arguments that were passed to the function */
ZEND_FUNCTION(func_num_args)
{
	void **p;
	int arg_count;

	p = EG(argument_stack).top_element-1-1;
	arg_count = (int)(zend_uintptr_t) *p;		/* this is the amount of arguments passed to func_num_args(); */
	p -= 1+arg_count;
	if (*p) {
		zend_error(E_ERROR, "func_num_args(): Can't be used as a function parameter");
	}
	--p;
	if (p>=EG(argument_stack).elements) {
		RETURN_LONG((long)(zend_uintptr_t) *p);
	} else {
		zend_error(E_WARNING, "func_num_args():  Called from the global scope - no function context");
		RETURN_LONG(-1);
	}
}
/* }}} */


/* {{{ proto mixed func_get_arg(int arg_num)
   Get the $arg_num'th argument that was passed to the function */
ZEND_FUNCTION(func_get_arg)
{
	void **p;
	int arg_count;
	zval **z_requested_offset;
	zval *arg;
	long requested_offset;

	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &z_requested_offset)==FAILURE) {
		RETURN_FALSE;
	}
	convert_to_long_ex(z_requested_offset);
	requested_offset = Z_LVAL_PP(z_requested_offset);

	if (requested_offset < 0) {
		zend_error(E_WARNING, "func_get_arg():  The argument number should be >= 0");
		RETURN_FALSE;
	}

	p = EG(argument_stack).top_element-1-1;
	arg_count = (int)(zend_uintptr_t) *p;		/* this is the amount of arguments passed to func_get_arg(); */
	p -= 1+arg_count;
	if (*p) {
		zend_error(E_ERROR, "func_get_arg(): Can't be used as a function parameter");
	}
	--p;
	if (p<EG(argument_stack).elements) {
		zend_error(E_WARNING, "func_get_arg():  Called from the global scope - no function context");
		RETURN_FALSE;
	}
	arg_count = (int)(zend_uintptr_t) *p;

	if (requested_offset>=arg_count) {
		zend_error(E_WARNING, "func_get_arg():  Argument %ld not passed to function", requested_offset);
		RETURN_FALSE;
	}

	arg = *(p-(arg_count-requested_offset));
	*return_value = *arg;
	zval_copy_ctor(return_value);
	INIT_PZVAL(return_value);
}
/* }}} */


/* {{{ proto array func_get_args()
   Get an array of the arguments that were passed to the function */
ZEND_FUNCTION(func_get_args)
{
	void **p;
	int arg_count;
	int i;

	p = EG(argument_stack).top_element-1-1;
	arg_count = (int)(zend_uintptr_t) *p;		/* this is the amount of arguments passed to func_get_args(); */
	p -= 1+arg_count;
	if (*p) {
		zend_error(E_ERROR, "func_get_args(): Can't be used as a function parameter");
	}
	--p;

	if (p<EG(argument_stack).elements) {
		zend_error(E_WARNING, "func_get_args():  Called from the global scope - no function context");
		RETURN_FALSE;
	}
	arg_count = (int)(zend_uintptr_t) *p;


	array_init(return_value);
	for (i=0; i<arg_count; i++) {
		zval *element;

		ALLOC_ZVAL(element);
		*element = **((zval **) (p-(arg_count-i)));
		zval_copy_ctor(element);
		INIT_PZVAL(element);
		zend_hash_next_index_insert(return_value->value.ht, &element, sizeof(zval *), NULL);
	}
}
/* }}} */


/* {{{ proto int strlen(string str)
   Get string length */
ZEND_NAMED_FUNCTION(zend_if_strlen)
{
	zval **str;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &str) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	convert_to_string_ex(str);
	RETVAL_LONG(Z_STRLEN_PP(str));
}
/* }}} */


/* {{{ proto int strcmp(string str1, string str2)
   Binary safe string comparison */
ZEND_FUNCTION(strcmp)
{
	zval **s1, **s2;
	
	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &s1, &s2) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	convert_to_string_ex(s1);
	convert_to_string_ex(s2);
	RETURN_LONG(zend_binary_zval_strcmp(*s1, *s2));
}
/* }}} */


/* {{{ proto int strncmp(string str1, string str2, int len)
   Binary safe string comparison */
ZEND_FUNCTION(strncmp)
{
	zval **s1, **s2, **s3;
	
	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &s1, &s2, &s3) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	convert_to_string_ex(s1);
	convert_to_string_ex(s2);
	convert_to_long_ex(s3);

	if (Z_LVAL_PP(s3) < 0) {
		zend_error(E_WARNING, "Length must be greater than or equal to 0");
		RETURN_FALSE;
	}
	
	RETURN_LONG(zend_binary_zval_strncmp(*s1, *s2, *s3));
}
/* }}} */


/* {{{ proto int strcasecmp(string str1, string str2)
   Binary safe case-insensitive string comparison */
ZEND_FUNCTION(strcasecmp)
{
	zval **s1, **s2;
	
	if (ZEND_NUM_ARGS()!=2 || zend_get_parameters_ex(2, &s1, &s2) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	convert_to_string_ex(s1);
	convert_to_string_ex(s2);
	RETURN_LONG(zend_binary_zval_strcasecmp(*s1, *s2));
}
/* }}} */


/* {{{ proto int strncasecmp(string str1, string str2, int len)
   Binary safe string comparison */
ZEND_FUNCTION(strncasecmp)
{
	zval **s1, **s2, **s3;
	
	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &s1, &s2, &s3) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	convert_to_string_ex(s1);
	convert_to_string_ex(s2);
	convert_to_long_ex(s3);

	if (Z_LVAL_PP(s3) < 0) {
		zend_error(E_WARNING, "Length must be greater than or equal to 0");
		RETURN_FALSE;
	}

	RETURN_LONG(zend_binary_zval_strncasecmp(*s1, *s2, *s3));
}
/* }}} */


/* {{{ proto array each(array arr)
   Return the currently pointed key..value pair in the passed array, and advance the pointer to the next element */
ZEND_FUNCTION(each)
{
	zval **array, *entry, **entry_ptr, *tmp;
	char *string_key;
	uint string_key_len;
	ulong num_key;
	zval **inserted_pointer;
	HashTable *target_hash;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &array) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	target_hash = HASH_OF(*array);
	if (!target_hash) {
		zend_error(E_WARNING,"Variable passed to each() is not an array or object");
		return;
	}
	if (zend_hash_get_current_data(target_hash, (void **) &entry_ptr)==FAILURE) {
		RETURN_FALSE;
	}
	array_init(return_value);
	entry = *entry_ptr;

	/* add value elements */
	if (entry->is_ref) {
		ALLOC_ZVAL(tmp);
		*tmp = *entry;
		zval_copy_ctor(tmp);
		tmp->is_ref=0;
		tmp->refcount=0;
		entry=tmp;
	}
	zend_hash_index_update(return_value->value.ht, 1, &entry, sizeof(zval *), NULL);
	entry->refcount++;
	zend_hash_update(return_value->value.ht, "value", sizeof("value"), &entry, sizeof(zval *), NULL);
	entry->refcount++;

	/* add the key elements */
	switch (zend_hash_get_current_key_ex(target_hash, &string_key, &string_key_len, &num_key, 1, NULL)) {
		case HASH_KEY_IS_STRING:
			add_get_index_stringl(return_value, 0, string_key, string_key_len-1, (void **) &inserted_pointer, 0);
			break;
		case HASH_KEY_IS_LONG:
			add_get_index_long(return_value, 0, num_key, (void **) &inserted_pointer);
			break;
	}
	zend_hash_update(return_value->value.ht, "key", sizeof("key"), inserted_pointer, sizeof(zval *), NULL);
	(*inserted_pointer)->refcount++;
	zend_hash_move_forward(target_hash);
}
/* }}} */


/* {{{ proto int error_reporting(int new_error_level=null)
   Return the current error_reporting level, and if an argument was passed - change to the new level */
ZEND_FUNCTION(error_reporting)
{
	zval **arg;
	int old_error_reporting;

	old_error_reporting = EG(error_reporting);
	switch (ZEND_NUM_ARGS()) {
		case 0:
			break;
		case 1:
			if (zend_get_parameters_ex(1, &arg) == FAILURE) {
				RETURN_FALSE;
			}
			convert_to_string_ex(arg);
			zend_alter_ini_entry("error_reporting", sizeof("error_reporting"), Z_STRVAL_PP(arg), Z_STRLEN_PP(arg), ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME);
			break;
		default:
			ZEND_WRONG_PARAM_COUNT();
			break;
	}

	RETVAL_LONG(old_error_reporting);
}
/* }}} */


/* {{{ proto bool define(string constant_name, mixed value, boolean case_sensitive=true)
   Define a new constant */
ZEND_FUNCTION(define)
{
	char *name;
	int name_len;
	zval *val;
	zval *val_free = NULL;
	zend_bool non_cs = 0;
	int case_sensitive = CONST_CS;
	zend_constant c;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz|b", &name, &name_len, &val, &non_cs) == FAILURE) {
		return;
	}

	if(non_cs) {
		case_sensitive = 0;
	}

repeat:
	switch (Z_TYPE_P(val)) {
		case IS_LONG:
		case IS_DOUBLE:
		case IS_STRING:
		case IS_BOOL:
		case IS_RESOURCE:
		case IS_NULL:
			break;
		case IS_OBJECT:
			if (!val_free) {
				if (Z_OBJ_HT_P(val)->get) {
					val_free = val = Z_OBJ_HT_P(val)->get(val TSRMLS_CC);
					goto repeat;
				} else if (Z_OBJ_HT_P(val)->cast_object) {
					ALLOC_INIT_ZVAL(val_free);
					if (Z_OBJ_HT_P(val)->cast_object(val, val_free, IS_STRING TSRMLS_CC) == SUCCESS) {
						val = val_free;
						break;
					}
				}
			}
			/* no break */
		default:
			zend_error(E_WARNING,"Constants may only evaluate to scalar values");
			if (val_free) {
				zval_ptr_dtor(&val_free);
			}
			RETURN_FALSE;
	}
	
	c.value = *val;
	zval_copy_ctor(&c.value);
	if (val_free) {
		zval_ptr_dtor(&val_free);
	}
	c.flags = case_sensitive; /* non persistent */
	c.name = zend_strndup(name, name_len);
	c.name_len = name_len+1;
	c.module_number = PHP_USER_CONSTANT;
	if (zend_register_constant(&c TSRMLS_CC) == SUCCESS) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto bool defined(string constant_name)
   Check whether a constant exists */
ZEND_FUNCTION(defined)
{
	zval **var;
	zval c;

	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &var)==FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	convert_to_string_ex(var);
	if (zend_get_constant(Z_STRVAL_PP(var), Z_STRLEN_PP(var), &c TSRMLS_CC)) {
		zval_dtor(&c);
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto string get_class([object object])
   Retrieves the class name */
ZEND_FUNCTION(get_class)
{
	zval **arg;
	char *name = "";
	zend_uint name_len = 0;
	int dup;

	if (!ZEND_NUM_ARGS()) {
		if (EG(scope)) {
			RETURN_STRINGL(EG(scope)->name, EG(scope)->name_length, 1);
		} else {
			zend_error(E_WARNING, "get_class() called without object from outside a class");
			RETURN_FALSE;
		}
	}
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &arg)==FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	if (Z_TYPE_PP(arg) != IS_OBJECT) {
		RETURN_FALSE;
	}

	dup = zend_get_object_classname(*arg, &name, &name_len TSRMLS_CC);

	RETURN_STRINGL(name, name_len, dup);
}
/* }}} */


/* {{{ proto string get_parent_class([mixed object])
   Retrieves the parent class name for object or class or current scope. */
ZEND_FUNCTION(get_parent_class)
{
	zval **arg;
	zend_class_entry *ce = NULL;
	char *name;
	zend_uint name_length;
	
	if (!ZEND_NUM_ARGS()) {
		ce = EG(scope);
		if (ce && ce->parent) {
			RETURN_STRINGL(ce->parent->name, ce->parent->name_length, 1);
		} else {
			RETURN_FALSE;
		}
	}
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &arg)==FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	if (Z_TYPE_PP(arg) == IS_OBJECT) {
		if (Z_OBJ_HT_PP(arg)->get_class_name
			&& Z_OBJ_HT_PP(arg)->get_class_name(*arg, &name, &name_length, 1 TSRMLS_CC) == SUCCESS) {
			RETURN_STRINGL(name, name_length, 0);
		} else {
			ce = zend_get_class_entry(*arg TSRMLS_CC);
		}
	} else if (Z_TYPE_PP(arg) == IS_STRING) {
		zend_class_entry **pce;
		
		if (zend_lookup_class(Z_STRVAL_PP(arg), Z_STRLEN_PP(arg), &pce TSRMLS_CC) == SUCCESS) {
			ce = *pce;
		}
	}

	if (ce && ce->parent) {
		RETURN_STRINGL(ce->parent->name, ce->parent->name_length, 1);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */


static void is_a_impl(INTERNAL_FUNCTION_PARAMETERS, zend_bool only_subclass)
{
	zval **obj, **class_name;
	zend_class_entry *instance_ce;
	zend_class_entry **ce;
	zend_bool retval;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &obj, &class_name)==FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	if (only_subclass && Z_TYPE_PP(obj) == IS_STRING) {
		zend_class_entry **the_ce;
		if (zend_lookup_class(Z_STRVAL_PP(obj), Z_STRLEN_PP(obj), &the_ce TSRMLS_CC) == FAILURE) {
			zend_error(E_WARNING, "Unknown class passed as parameter");
			RETURN_FALSE;
		}
		instance_ce = *the_ce;
	} else if (Z_TYPE_PP(obj) != IS_OBJECT) {
		RETURN_FALSE;
	} else {
		instance_ce = NULL;
	}

	/* TBI!! new object handlers */
	if (Z_TYPE_PP(obj) == IS_OBJECT && !HAS_CLASS_ENTRY(**obj)) {
		RETURN_FALSE;
	}

	convert_to_string_ex(class_name);

	if (zend_lookup_class_ex(Z_STRVAL_PP(class_name), Z_STRLEN_PP(class_name), 0, &ce TSRMLS_CC) == FAILURE) {
		retval = 0;
	} else {
		if (only_subclass) {
			if (!instance_ce) {
				instance_ce = Z_OBJCE_PP(obj)->parent;
			} else {
				instance_ce = instance_ce->parent;
			}
		} else {
			instance_ce = Z_OBJCE_PP(obj);
		}

		if (!instance_ce) {
			RETURN_FALSE;
 		}

		if (instanceof_function(instance_ce, *ce TSRMLS_CC)) {
			retval = 1;
		} else {
			retval = 0;
		}
	}

	RETURN_BOOL(retval);
}


/* {{{ proto bool is_subclass_of(object object, string class_name)
   Returns true if the object has this class as one of its parents */
ZEND_FUNCTION(is_subclass_of)
{
	is_a_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */


/* {{{ proto bool is_a(object object, string class_name)
   Returns true if the object is of this class or has this class as one of its parents */
ZEND_FUNCTION(is_a)
{
	zend_error(E_STRICT, "is_a(): Deprecated. Please use the instanceof operator");
	is_a_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */


/* {{{ add_class_vars */
static void add_class_vars(zend_class_entry *ce, HashTable *properties, zval *return_value TSRMLS_DC)
{
	if (zend_hash_num_elements(properties) > 0) {
		HashPosition pos;
		zval **prop;

		zend_hash_internal_pointer_reset_ex(properties, &pos);
		while (zend_hash_get_current_data_ex(properties, (void **) &prop, &pos) == SUCCESS) {
			char *key, *class_name, *prop_name;
			uint key_len;
			ulong num_index, h;
			int prop_name_len = 0;			
			zval *prop_copy;
			zend_property_info *property_info;

			zend_hash_get_current_key_ex(properties, &key, &key_len, &num_index, 0, &pos);
			zend_hash_move_forward_ex(properties, &pos);

			zend_unmangle_property_name(key, key_len-1, &class_name, &prop_name);
			prop_name_len = strlen(prop_name);
			
			h = zend_get_hash_value(prop_name, prop_name_len+1);
			if (zend_hash_quick_find(&ce->properties_info, prop_name, prop_name_len+1, h, (void **) &property_info) == FAILURE) {
				continue;
			}
			
			if (property_info->flags & ZEND_ACC_SHADOW) {
				continue;
			} else if ((property_info->flags & ZEND_ACC_PRIVATE) && EG(scope) != ce) {
				continue;
			} else if ((property_info->flags & ZEND_ACC_PROTECTED) && zend_check_protected(ce, EG(scope)) == 0) {
				continue;
			}

			/* copy: enforce read only access */
			ALLOC_ZVAL(prop_copy);
			*prop_copy = **prop;
			zval_copy_ctor(prop_copy);
			INIT_PZVAL(prop_copy);

			/* this is necessary to make it able to work with default array 
			* properties, returned to user */
			if (Z_TYPE_P(prop_copy) == IS_CONSTANT_ARRAY || Z_TYPE_P(prop_copy) == IS_CONSTANT) {
				zval_update_constant(&prop_copy, 0 TSRMLS_CC);
			}

			add_assoc_zval(return_value, prop_name, prop_copy);
		}
	}
}
/* }}} */


/* {{{ proto array get_class_vars(string class_name)
   Returns an array of default properties of the class. */
ZEND_FUNCTION(get_class_vars)
{
	char *class_name;
	int class_name_len;
	zend_class_entry **pce;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &class_name, &class_name_len) == FAILURE) {
		return;
	}

	if (zend_lookup_class(class_name, class_name_len, &pce TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	} else {
		array_init(return_value);
		zend_update_class_constants(*pce TSRMLS_CC);
		add_class_vars(*pce, &(*pce)->default_properties, return_value TSRMLS_CC);
		add_class_vars(*pce, CE_STATIC_MEMBERS(*pce), return_value TSRMLS_CC);
	}
}
/* }}} */


/* {{{ proto array get_object_vars(object obj)
   Returns an array of object properties */
ZEND_FUNCTION(get_object_vars)
{
	zval **obj;
	zval **value;
	HashTable *properties;
	HashPosition pos;
	char *key, *prop_name, *class_name;
	uint key_len;
	ulong num_index;
	zend_object *zobj;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &obj) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	if (Z_TYPE_PP(obj) != IS_OBJECT) {
		RETURN_FALSE;
	}
	if (Z_OBJ_HT_PP(obj)->get_properties == NULL) {
		RETURN_FALSE;
	}

	properties = Z_OBJ_HT_PP(obj)->get_properties(*obj TSRMLS_CC);

	if (properties == NULL) {
		RETURN_FALSE;
	}

	zobj = zend_objects_get_address(*obj TSRMLS_CC);

	array_init(return_value);

	zend_hash_internal_pointer_reset_ex(properties, &pos);

	while (zend_hash_get_current_data_ex(properties, (void **) &value, &pos) == SUCCESS) {
		if (zend_hash_get_current_key_ex(properties, &key, &key_len, &num_index, 0, &pos) == HASH_KEY_IS_STRING) {
			if (zend_check_property_access(zobj, key, key_len-1 TSRMLS_CC) == SUCCESS) {
				zend_unmangle_property_name(key, key_len-1, &class_name, &prop_name);
				/* Not separating references */
				(*value)->refcount++;
				add_assoc_zval_ex(return_value, prop_name, strlen(prop_name)+1, *value);
			}
		}
		zend_hash_move_forward_ex(properties, &pos);
	}
}
/* }}} */


/* {{{ proto array get_class_methods(mixed class)
   Returns an array of method names for class or class instance. */
ZEND_FUNCTION(get_class_methods)
{
	zval **class;
	zval *method_name;
	zend_class_entry *ce = NULL, **pce;
	HashPosition pos;
	zend_function *mptr;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &class)==FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	if (Z_TYPE_PP(class) == IS_OBJECT) {
		/* TBI!! new object handlers */
		if (!HAS_CLASS_ENTRY(**class)) {
			RETURN_FALSE;
		}
		ce = Z_OBJCE_PP(class);
	} else if (Z_TYPE_PP(class) == IS_STRING) {
		if (zend_lookup_class(Z_STRVAL_PP(class), Z_STRLEN_PP(class), &pce TSRMLS_CC) == SUCCESS) {
			ce = *pce;
		}
	}

	if (!ce) {
		RETURN_NULL();
	}

	array_init(return_value);
	zend_hash_internal_pointer_reset_ex(&ce->function_table, &pos);

	while (zend_hash_get_current_data_ex(&ce->function_table, (void **) &mptr, &pos) == SUCCESS) {
		if ((mptr->common.fn_flags & ZEND_ACC_PUBLIC) 
		 || (EG(scope) &&
		     (((mptr->common.fn_flags & ZEND_ACC_PROTECTED) &&
		       zend_check_protected(mptr->common.scope, EG(scope)))
		   || ((mptr->common.fn_flags & ZEND_ACC_PRIVATE) &&
		       EG(scope) == mptr->common.scope)))) {
			char *key;
			uint key_len;
			ulong num_index;
			uint len = strlen(mptr->common.function_name);

			/* Do not display old-style inherited constructors */
			if ((mptr->common.fn_flags & ZEND_ACC_CTOR) == 0 ||
			    mptr->common.scope == ce ||
			    zend_hash_get_current_key_ex(&ce->function_table, &key, &key_len, &num_index, 0, &pos) != HASH_KEY_IS_STRING ||
			    zend_binary_strcasecmp(key, key_len-1, mptr->common.function_name, len) == 0) {

				MAKE_STD_ZVAL(method_name);
				ZVAL_STRINGL(method_name, mptr->common.function_name, len, 1);
				zend_hash_next_index_insert(return_value->value.ht, &method_name, sizeof(zval *), NULL);
			}
		}
		zend_hash_move_forward_ex(&ce->function_table, &pos);
	}
}
/* }}} */


/* {{{ proto bool method_exists(object object, string method)
   Checks if the class method exists */
ZEND_FUNCTION(method_exists)
{
	zval **klass, **method_name;
	char *lcname;
	zend_class_entry * ce, **pce;

	if (ZEND_NUM_ARGS()!=2 || zend_get_parameters_ex(2, &klass, &method_name)==FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	if (Z_TYPE_PP(klass) == IS_OBJECT) {
		ce = Z_OBJCE_PP(klass);
	} else if (Z_TYPE_PP(klass) == IS_STRING) {
		if (zend_lookup_class(Z_STRVAL_PP(klass), Z_STRLEN_PP(klass), &pce TSRMLS_CC) == FAILURE) {
			RETURN_FALSE;
		}
		ce = *pce;
	} else {
		RETURN_FALSE;
	}

	convert_to_string_ex(method_name);
	lcname = zend_str_tolower_dup(Z_STRVAL_PP(method_name), Z_STRLEN_PP(method_name));
	if (zend_hash_exists(&ce->function_table, lcname, Z_STRLEN_PP(method_name)+1)) {
		efree(lcname);
		RETURN_TRUE;
	} else {
		union _zend_function *func = NULL;
		efree(lcname);

		if (Z_TYPE_PP(klass) == IS_OBJECT 
		&& Z_OBJ_HT_PP(klass)->get_method != NULL
		&& (func = Z_OBJ_HT_PP(klass)->get_method(klass, Z_STRVAL_PP(method_name), Z_STRLEN_PP(method_name) TSRMLS_CC)) != NULL
		) {
			if (func->type == ZEND_INTERNAL_FUNCTION 
			&& ((zend_internal_function*)func)->handler == zend_std_call_user_call
			) {
				efree(((zend_internal_function*)func)->function_name);
				efree(func);
				RETURN_FALSE;
			}
			RETURN_TRUE;
		}
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool property_exists(mixed object_or_class, string property_name)
   Checks if the object or class has a property */
ZEND_FUNCTION(property_exists)
{
	zval **object, **property;
	zend_class_entry *ce, **pce;
	zend_property_info *property_info;
	char *prop_name, *class_name;

	if (ZEND_NUM_ARGS()!= 2 || zend_get_parameters_ex(2, &object, &property)==FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	convert_to_string_ex(property);
	if (!Z_STRLEN_PP(property)) {
		RETURN_FALSE;
	}

	switch((*object)->type) {
	case IS_STRING:
		if (!Z_STRLEN_PP(object)) {
			RETURN_FALSE;
		}
		if (zend_lookup_class(Z_STRVAL_PP(object), Z_STRLEN_PP(object), &pce TSRMLS_CC) == SUCCESS) {
			ce = *pce;
		} else {
			RETURN_FALSE;
		}
		if (!ce) {
			RETURN_NULL();
		}
		if (!(property_info = zend_get_property_info(ce, *property, 1 TSRMLS_CC)) || property_info == &EG(std_property_info)) {
			RETURN_FALSE;
		}
		if (property_info->flags & ZEND_ACC_PUBLIC) {
			RETURN_TRUE;
		}
		zend_unmangle_property_name(property_info->name, property_info->name_length, &class_name, &prop_name);
		if (!strncmp(class_name, "*", 1)) {
			if (instanceof_function(EG(scope), ce TSRMLS_CC) ||
				(EG(This) && instanceof_function(Z_OBJCE_P(EG(This)), ce TSRMLS_CC))) {
				RETURN_TRUE;
			}
			RETURN_FALSE;
		}
		if (zend_lookup_class(Z_STRVAL_PP(object), Z_STRLEN_PP(object), &pce TSRMLS_CC) == SUCCESS) {
			ce = *pce;
		} else {
			RETURN_FALSE; /* shouldn't happen */
		}
		RETURN_BOOL(EG(scope) == ce);

	case IS_OBJECT:
		if (Z_OBJ_HANDLER_PP(object, has_property) && Z_OBJ_HANDLER_PP(object, has_property)(*object, *property, 2 TSRMLS_CC)) {
			RETURN_TRUE;
		}
		RETURN_FALSE;

	default:
		zend_error(E_WARNING, "First parameter must either be an object or the name of an existing class");
		RETURN_NULL();
	}
}
/* }}} */


/* {{{ proto bool class_exists(string classname [, bool autoload])
   Checks if the class exists */
ZEND_FUNCTION(class_exists)
{
	char *class_name, *lc_name;
	zend_class_entry **ce;
	int class_name_len;
	int found;
	zend_bool autoload = 1;
	ALLOCA_FLAG(use_heap)

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|b", &class_name, &class_name_len, &autoload) == FAILURE) {
		return;
	}

	if (!autoload) {
		lc_name = do_alloca_with_limit(class_name_len + 1, use_heap);
		zend_str_tolower_copy(lc_name, class_name, class_name_len);
	
		found = zend_hash_find(EG(class_table), lc_name, class_name_len+1, (void **) &ce);
		free_alloca_with_limit(lc_name, use_heap);
		RETURN_BOOL(found == SUCCESS && !((*ce)->ce_flags & ZEND_ACC_INTERFACE));
	}

 	if (zend_lookup_class(class_name, class_name_len, &ce TSRMLS_CC) == SUCCESS) {
 		RETURN_BOOL(((*ce)->ce_flags & ZEND_ACC_INTERFACE) == 0);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool interface_exists(string classname [, bool autoload])
   Checks if the class exists */
ZEND_FUNCTION(interface_exists)
{
	char *iface_name, *lc_name;
	zend_class_entry **ce;
	int iface_name_len;
	int found;
	zend_bool autoload = 1;
	ALLOCA_FLAG(use_heap)

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|b", &iface_name, &iface_name_len, &autoload) == FAILURE) {
		return;
	}

	if (!autoload) {
		lc_name = do_alloca_with_limit(iface_name_len + 1, use_heap);
		zend_str_tolower_copy(lc_name, iface_name, iface_name_len);
	
		found = zend_hash_find(EG(class_table), lc_name, iface_name_len+1, (void **) &ce);
		free_alloca_with_limit(lc_name, use_heap);
		RETURN_BOOL(found == SUCCESS && (*ce)->ce_flags & ZEND_ACC_INTERFACE);
	}

 	if (zend_lookup_class(iface_name, iface_name_len, &ce TSRMLS_CC) == SUCCESS) {
 		RETURN_BOOL(((*ce)->ce_flags & ZEND_ACC_INTERFACE) > 0);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto bool function_exists(string function_name) 
   Checks if the function exists */
ZEND_FUNCTION(function_exists)
{
	zval **function_name;
	zend_function *func;
	char *lcname;
	zend_bool retval;
	
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &function_name)==FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	convert_to_string_ex(function_name);
	lcname = zend_str_tolower_dup(Z_STRVAL_PP(function_name), Z_STRLEN_PP(function_name));

	retval = (zend_hash_find(EG(function_table), lcname, Z_STRLEN_PP(function_name)+1, (void **)&func) == SUCCESS);
	
	efree(lcname);

	/*
	 * A bit of a hack, but not a bad one: we see if the handler of the function
	 * is actually one that displays "function is disabled" message.
	 */
	if (retval && func->type == ZEND_INTERNAL_FUNCTION &&
		func->internal_function.handler == zif_display_disabled_function) {
		retval = 0;
	}

	RETURN_BOOL(retval);
}
/* }}} */

#if ZEND_DEBUG
/* {{{ proto void leak(int num_bytes=3)
   Cause an intentional memory leak, for testing/debugging purposes */
ZEND_FUNCTION(leak)
{
	int leakbytes=3;
	zval **leak;

	if (ZEND_NUM_ARGS()>=1) {
		if (zend_get_parameters_ex(1, &leak)==SUCCESS) {
			convert_to_long_ex(leak);
			leakbytes = Z_LVAL_PP(leak);
		}
	}

	emalloc(leakbytes);
}
/* }}} */


#ifdef ZEND_TEST_EXCEPTIONS
ZEND_FUNCTION(crash)
{
	char *nowhere=NULL;

	memcpy(nowhere, "something", sizeof("something"));
}
#endif

#endif /* ZEND_DEBUG */

/* {{{ proto array get_included_files(void)
   Returns an array with the file names that were include_once()'d */
ZEND_FUNCTION(get_included_files)
{
	char *entry;
	if (ZEND_NUM_ARGS() != 0) {
		ZEND_WRONG_PARAM_COUNT();
	}

	array_init(return_value);
	zend_hash_internal_pointer_reset(&EG(included_files));
	while (zend_hash_get_current_key(&EG(included_files), &entry, NULL, 1) == HASH_KEY_IS_STRING) {
		add_next_index_string(return_value, entry, 0);
		zend_hash_move_forward(&EG(included_files));
	}
}
/* }}} */


/* {{{ proto void trigger_error(string messsage [, int error_type])
   Generates a user-level error/warning/notice message */
ZEND_FUNCTION(trigger_error)
{
	int error_type = E_USER_NOTICE;
	zval **z_error_type, **z_error_message;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &z_error_message)==FAILURE) {
				ZEND_WRONG_PARAM_COUNT();
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &z_error_message, &z_error_type)==FAILURE) {
				ZEND_WRONG_PARAM_COUNT();
			}
			convert_to_long_ex(z_error_type);
			error_type = Z_LVAL_PP(z_error_type);
			switch (error_type) {
				case E_USER_ERROR:
				case E_USER_WARNING:
				case E_USER_NOTICE:
					break;
				default:
					zend_error(E_WARNING, "Invalid error type specified");
					RETURN_FALSE;
					break;
			}
			break;
		default:
			ZEND_WRONG_PARAM_COUNT();	
	}
	convert_to_string_ex(z_error_message);
	zend_error(error_type, "%s", Z_STRVAL_PP(z_error_message));
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto string set_error_handler(string error_handler [, int error_types])
   Sets a user-defined error handler function.  Returns the previously defined error handler, or false on error */
ZEND_FUNCTION(set_error_handler)
{
	zval *error_handler;
	zend_bool had_orig_error_handler=0;
	char *error_handler_name = NULL;
	long error_type = E_ALL | E_STRICT;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|l", &error_handler, &error_type) == FAILURE) {
		return;
	}

	if (!zend_is_callable(error_handler, 0, &error_handler_name)) {
		zend_error(E_WARNING, "%s() expects the argument (%s) to be a valid callback",
				   get_active_function_name(TSRMLS_C), error_handler_name?error_handler_name:"unknown");
		efree(error_handler_name);
		return;
	}
	efree(error_handler_name);

	if (EG(user_error_handler)) {
		had_orig_error_handler = 1;
		*return_value = *EG(user_error_handler);
		zval_copy_ctor(return_value);
		INIT_PZVAL(return_value);
		zend_stack_push(&EG(user_error_handlers_error_reporting), &EG(user_error_handler_error_reporting), sizeof(EG(user_error_handler_error_reporting)));
		zend_ptr_stack_push(&EG(user_error_handlers), EG(user_error_handler));
	}
	ALLOC_ZVAL(EG(user_error_handler));

	if (!zend_is_true(error_handler)) { /* unset user-defined handler */
		FREE_ZVAL(EG(user_error_handler));
		EG(user_error_handler) = NULL;
		RETURN_TRUE;
	}

	EG(user_error_handler_error_reporting) = (int)error_type;
	*EG(user_error_handler) = *error_handler;
	zval_copy_ctor(EG(user_error_handler));
	INIT_PZVAL(EG(user_error_handler));

	if (!had_orig_error_handler) {
		RETURN_NULL();
	}
}
/* }}} */


/* {{{ proto void restore_error_handler(void)
   Restores the previously defined error handler function */
ZEND_FUNCTION(restore_error_handler)
{
	if (EG(user_error_handler)) {
		zval *zeh = EG(user_error_handler);

		EG(user_error_handler) = NULL;
		zval_ptr_dtor(&zeh);
	}

	if (zend_ptr_stack_num_elements(&EG(user_error_handlers))==0) {
		EG(user_error_handler) = NULL;
	} else {
		EG(user_error_handler_error_reporting) = zend_stack_int_top(&EG(user_error_handlers_error_reporting));
		zend_stack_del_top(&EG(user_error_handlers_error_reporting));
		EG(user_error_handler) = zend_ptr_stack_pop(&EG(user_error_handlers));
	}
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto string set_exception_handler(callable exception_handler)
   Sets a user-defined exception handler function.  Returns the previously defined exception handler, or false on error */
ZEND_FUNCTION(set_exception_handler)
{
	zval **exception_handler;
	char *exception_handler_name = NULL;
	zend_bool had_orig_exception_handler=0;

	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &exception_handler)==FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	if (Z_TYPE_PP(exception_handler) != IS_NULL) { /* NULL == unset */
		if (!zend_is_callable(*exception_handler, 0, &exception_handler_name)) {
			zend_error(E_WARNING, "%s() expects the argument (%s) to be a valid callback",
					   get_active_function_name(TSRMLS_C), exception_handler_name?exception_handler_name:"unknown");
			efree(exception_handler_name);
			return;
		}
		efree(exception_handler_name);
	}

	if (EG(user_exception_handler)) {
		had_orig_exception_handler = 1;
		*return_value = *EG(user_exception_handler);
		zval_copy_ctor(return_value);
		zend_ptr_stack_push(&EG(user_exception_handlers), EG(user_exception_handler));
	}
	ALLOC_ZVAL(EG(user_exception_handler));

	if (Z_TYPE_PP(exception_handler) == IS_NULL) { /* unset user-defined handler */
		FREE_ZVAL(EG(user_exception_handler));
		EG(user_exception_handler) = NULL;
		RETURN_TRUE;
	}

	*EG(user_exception_handler) = **exception_handler;
	zval_copy_ctor(EG(user_exception_handler));

	if (!had_orig_exception_handler) {
		RETURN_NULL();
	}
}
/* }}} */


/* {{{ proto void restore_exception_handler(void)
   Restores the previously defined exception handler function */
ZEND_FUNCTION(restore_exception_handler)
{
	if (EG(user_exception_handler)) {
		zval_ptr_dtor(&EG(user_exception_handler));
	}
	if (zend_ptr_stack_num_elements(&EG(user_exception_handlers))==0) {
		EG(user_exception_handler) = NULL;
	} else {
		EG(user_exception_handler) = zend_ptr_stack_pop(&EG(user_exception_handlers));
	}
	RETURN_TRUE;
}
/* }}} */


static int copy_class_or_interface_name(zend_class_entry **pce, int num_args, va_list args, zend_hash_key *hash_key)
{
	zval *array = va_arg(args, zval *);
	zend_uint mask = va_arg(args, zend_uint);
	zend_uint comply = va_arg(args, zend_uint);
	zend_uint comply_mask = (comply)? mask:0;
	zend_class_entry *ce  = *pce;

	if ((hash_key->nKeyLength==0 || hash_key->arKey[0]!=0)
		&& (comply_mask == (ce->ce_flags & mask))) {
		add_next_index_stringl(array, ce->name, ce->name_length, 1);
	}
	return ZEND_HASH_APPLY_KEEP;
}


/* {{{ proto array get_declared_classes()
   Returns an array of all declared classes. */
ZEND_FUNCTION(get_declared_classes)
{
	zend_uint mask = ZEND_ACC_INTERFACE;
	zend_uint comply = 0;

	if (ZEND_NUM_ARGS() != 0) {
		ZEND_WRONG_PARAM_COUNT();
	}

	array_init(return_value);
	zend_hash_apply_with_arguments(EG(class_table), (apply_func_args_t) copy_class_or_interface_name, 3, return_value, mask, comply);
}
/* }}} */

/* {{{ proto array get_declared_interfaces()
   Returns an array of all declared interfaces. */
ZEND_FUNCTION(get_declared_interfaces)
{
	zend_uint mask = ZEND_ACC_INTERFACE;
	zend_uint comply = 1;

	if (ZEND_NUM_ARGS() != 0) {
		ZEND_WRONG_PARAM_COUNT();
	}

	array_init(return_value);
	zend_hash_apply_with_arguments(EG(class_table), (apply_func_args_t) copy_class_or_interface_name, 3, return_value, mask, comply);
}
/* }}} */


static int copy_function_name(zend_function *func, int num_args, va_list args, zend_hash_key *hash_key)
{
	zval *internal_ar = va_arg(args, zval *),
	     *user_ar     = va_arg(args, zval *);

	if (hash_key->nKeyLength == 0 || hash_key->arKey[0] == 0) {
		return 0;
	}

	if (func->type == ZEND_INTERNAL_FUNCTION) {
		add_next_index_stringl(internal_ar, hash_key->arKey, hash_key->nKeyLength-1, 1);
	} else if (func->type == ZEND_USER_FUNCTION) {
		add_next_index_stringl(user_ar, hash_key->arKey, hash_key->nKeyLength-1, 1);
	}

	return 0;
}


/* {{{ proto array get_defined_functions(void)
   Returns an array of all defined functions */
ZEND_FUNCTION(get_defined_functions)
{
	zval *internal;
	zval *user;

	if (ZEND_NUM_ARGS() != 0) {
		ZEND_WRONG_PARAM_COUNT();
	}

	MAKE_STD_ZVAL(internal);
	MAKE_STD_ZVAL(user);

	array_init(internal);
	array_init(user);
	array_init(return_value);

	zend_hash_apply_with_arguments(EG(function_table), (apply_func_args_t) copy_function_name, 2, internal, user);

	if (zend_hash_add(Z_ARRVAL_P(return_value), "internal", sizeof("internal"), (void **)&internal, sizeof(zval *), NULL) == FAILURE) {
		zval_ptr_dtor(&internal);
		zval_ptr_dtor(&user);
		zval_dtor(return_value);
		zend_error(E_WARNING, "Cannot add internal functions to return value from get_defined_functions()");
		RETURN_FALSE;
	}

	if (zend_hash_add(Z_ARRVAL_P(return_value), "user", sizeof("user"), (void **)&user, sizeof(zval *), NULL) == FAILURE) {
		zval_ptr_dtor(&user);
		zval_dtor(return_value);
		zend_error(E_WARNING, "Cannot add user functions to return value from get_defined_functions()");
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto array get_defined_vars(void)
   Returns an associative array of names and values of all currently defined variable names (variables in the current scope) */
ZEND_FUNCTION(get_defined_vars)
{
	zval *tmp;

	array_init(return_value);

	zend_hash_copy(Z_ARRVAL_P(return_value), EG(active_symbol_table),
					(copy_ctor_func_t)zval_add_ref, &tmp, sizeof(zval *));
}
/* }}} */


#define LAMBDA_TEMP_FUNCNAME	"__lambda_func"
/* {{{ proto string create_function(string args, string code)
   Creates an anonymous function, and returns its name (funny, eh?) */
ZEND_FUNCTION(create_function)
{
	char *eval_code, *function_name;
	int eval_code_length, function_name_length;
	zval **z_function_args, **z_function_code;
	int retval;
	char *eval_name;

	if (ZEND_NUM_ARGS()!=2 || zend_get_parameters_ex(2, &z_function_args, &z_function_code)==FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_string_ex(z_function_args);
	convert_to_string_ex(z_function_code);

	eval_code_length = sizeof("function " LAMBDA_TEMP_FUNCNAME)
			+Z_STRLEN_PP(z_function_args)
			+2	/* for the args parentheses */
			+2	/* for the curly braces */
			+Z_STRLEN_PP(z_function_code);

	zend_spprintf(&eval_code, 0, "function " LAMBDA_TEMP_FUNCNAME "(%s){%s}", Z_STRVAL_PP(z_function_args), Z_STRVAL_PP(z_function_code));

	eval_name = zend_make_compiled_string_description("runtime-created function" TSRMLS_CC);
	retval = zend_eval_string(eval_code, NULL, eval_name TSRMLS_CC);
	efree(eval_code);
	efree(eval_name);

	if (retval==SUCCESS) {
		zend_function new_function, *func;

		if (zend_hash_find(EG(function_table), LAMBDA_TEMP_FUNCNAME, sizeof(LAMBDA_TEMP_FUNCNAME), (void **) &func)==FAILURE) {
			zend_error(E_ERROR, "Unexpected inconsistency in create_function()");
			RETURN_FALSE;
		}
		new_function = *func;
		function_add_ref(&new_function);

		function_name = (char *) emalloc(sizeof("0lambda_")+MAX_LENGTH_OF_LONG);

		do {
			sprintf(function_name, "%clambda_%d", 0, ++EG(lambda_count));
			function_name_length = strlen(function_name+1)+1;
		} while (zend_hash_add(EG(function_table), function_name, function_name_length+1, &new_function, sizeof(zend_function), NULL)==FAILURE);
		zend_hash_del(EG(function_table), LAMBDA_TEMP_FUNCNAME, sizeof(LAMBDA_TEMP_FUNCNAME));
		RETURN_STRINGL(function_name, function_name_length, 0);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */


#if ZEND_DEBUG
ZEND_FUNCTION(zend_test_func)
{
	zval *arg1, *arg2;

	zend_get_parameters(ht, 2, &arg1, &arg2);
}


#ifdef ZTS
ZEND_FUNCTION(zend_thread_id)
{
	RETURN_LONG(tsrm_thread_id());
}
#endif
#endif

/* {{{ proto string get_resource_type(resource res)
   Get the resource type name for a given resource */
ZEND_FUNCTION(get_resource_type)
{
	char *resource_type;
	zval **z_resource_type;

	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &z_resource_type)==FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	if (Z_TYPE_PP(z_resource_type) != IS_RESOURCE) {
		zend_error(E_WARNING, "Supplied argument is not a valid resource handle");
		RETURN_FALSE;
	}

	resource_type = zend_rsrc_list_get_rsrc_type(Z_LVAL_PP(z_resource_type) TSRMLS_CC);
	if (resource_type) {
		RETURN_STRING(resource_type, 1);
	} else {
		RETURN_STRING("Unknown", 1);
	}
}
/* }}} */


static int add_extension_info(zend_module_entry *module, void *arg TSRMLS_DC)
{
	zval *name_array = (zval *)arg;
	add_next_index_string(name_array, module->name, 1);
	return 0;
}

static int add_zendext_info(zend_extension *ext, void *arg TSRMLS_DC)
{
	zval *name_array = (zval *)arg;
	add_next_index_string(name_array, ext->name, 1);
	return 0;
}

static int add_constant_info(zend_constant *constant, void *arg TSRMLS_DC)
{
	zval *name_array = (zval *)arg;
	zval *const_val;

	MAKE_STD_ZVAL(const_val);
	*const_val = constant->value;
	zval_copy_ctor(const_val);
	INIT_PZVAL(const_val);
	add_assoc_zval_ex(name_array, constant->name, constant->name_len, const_val);
	return 0;
}


/* {{{ proto array get_loaded_extensions([bool zend_extensions]) U
   Return an array containing names of loaded extensions */
ZEND_FUNCTION(get_loaded_extensions)
{
	zend_bool zendext = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &zendext) == FAILURE) {
		return;
	}

	array_init(return_value);

	if (zendext) {
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) add_zendext_info, return_value TSRMLS_CC);
	} else {
		zend_hash_apply_with_argument(&module_registry, (apply_func_arg_t) add_extension_info, return_value TSRMLS_CC);
	}
}
/* }}} */


/* {{{ proto array get_defined_constants([mixed categorize])
   Return an array containing the names and values of all defined constants */
ZEND_FUNCTION(get_defined_constants)
{
	int argc = ZEND_NUM_ARGS();

	if (argc != 0 && argc != 1) {
		ZEND_WRONG_PARAM_COUNT();
	}

	array_init(return_value);

	if (argc) {
		HashPosition pos;
		zend_constant *val;
		int module_number;
		zval **modules;
		char **module_names;
		zend_module_entry *module;
		int i = 1;

		modules = ecalloc(zend_hash_num_elements(&module_registry) + 2, sizeof(zval *));
		module_names = emalloc((zend_hash_num_elements(&module_registry) + 2) * sizeof(char *));

		module_names[0] = "internal";
		zend_hash_internal_pointer_reset_ex(&module_registry, &pos);
		while (zend_hash_get_current_data_ex(&module_registry, (void *) &module, &pos) != FAILURE) {
			module_names[i++] = module->name;
			zend_hash_move_forward_ex(&module_registry, &pos);
		}
		module_names[i] = "user";

		zend_hash_internal_pointer_reset_ex(EG(zend_constants), &pos);
		while (zend_hash_get_current_data_ex(EG(zend_constants), (void **) &val, &pos) != FAILURE) {
			zval *const_val;

			if (val->module_number == PHP_USER_CONSTANT) {
				module_number = i;
			} else if (val->module_number > i || val->module_number < 0) {
				/* should not happen */
				goto bad_module_id;
			} else {
				module_number = val->module_number;
			}

			if (!modules[module_number]) {
				MAKE_STD_ZVAL(modules[module_number]);
				array_init(modules[module_number]);
				add_assoc_zval(return_value, module_names[module_number], modules[module_number]);
			}

			MAKE_STD_ZVAL(const_val);
			*const_val = val->value;
			zval_copy_ctor(const_val);
			INIT_PZVAL(const_val);

			add_assoc_zval_ex(modules[module_number], val->name, val->name_len, const_val);
bad_module_id:
			zend_hash_move_forward_ex(EG(zend_constants), &pos);
		}
		efree(module_names);
		efree(modules);
	} else {
		zend_hash_apply_with_argument(EG(zend_constants), (apply_func_arg_t) add_constant_info, return_value TSRMLS_CC);
	}
}
/* }}} */


static zval *debug_backtrace_get_args(void ***curpos TSRMLS_DC)
{
	void **p = *curpos - 2;
	zval *arg_array, **arg;
	int arg_count = (int)(zend_uintptr_t) *p;

	*curpos -= (arg_count+2);

	MAKE_STD_ZVAL(arg_array);
	array_init(arg_array);
	p -= arg_count;

	while (--arg_count >= 0) {
		arg = (zval **) p++;
		if (*arg) {
			if (Z_TYPE_PP(arg) != IS_OBJECT) {
				SEPARATE_ZVAL_TO_MAKE_IS_REF(arg);
			}
			(*arg)->refcount++;
			add_next_index_zval(arg_array, *arg);
		} else {
			add_next_index_null(arg_array);
		}
	}

	/* skip args from incomplete frames */
	while ((((*curpos)-1) > EG(argument_stack).elements) && *((*curpos)-1)) {
		(*curpos)--;
	}

	return arg_array;
}

void debug_print_backtrace_args(zval *arg_array TSRMLS_DC)
{
	zval **tmp;
	HashPosition iterator;
	int i = 0;

	zend_hash_internal_pointer_reset_ex(arg_array->value.ht, &iterator);
	while (zend_hash_get_current_data_ex(arg_array->value.ht, (void **) &tmp, &iterator) == SUCCESS) {
		if (i++) {
			ZEND_PUTS(", ");
		}
		zend_print_flat_zval_r(*tmp TSRMLS_CC);
		zend_hash_move_forward_ex(arg_array->value.ht, &iterator);
	}
}

/* {{{ proto void debug_print_backtrace(void) */
ZEND_FUNCTION(debug_print_backtrace)
{
	zend_execute_data *ptr, *skip;
	int lineno;
	char *function_name;
	char *filename;
	char *class_name = NULL;
	char *call_type;
	char *include_filename = NULL;
	zval *arg_array = NULL;
	void **cur_arg_pos = EG(argument_stack).top_element;
	void **args = cur_arg_pos;
	int arg_stack_consistent = 0;
	int frames_on_stack = 0;
	int indent = 0;

	if (ZEND_NUM_ARGS()) {
		ZEND_WRONG_PARAM_COUNT();
	}

	while (--args > EG(argument_stack).elements) {
		if (*args--) {
			break;
		}
		args -= *(ulong*)args;
		frames_on_stack++;

		/* skip args from incomplete frames */
		while (((args-1) > EG(argument_stack).elements) && *(args-1)) {
			args--;
		}

		if ((args-1) == EG(argument_stack).elements) {
			arg_stack_consistent = 1;
			break;
		}
	}

	ptr = EG(current_execute_data);

	/* skip debug_backtrace() */
	ptr = ptr->prev_execute_data;
	cur_arg_pos -= 2;
	frames_on_stack--;

	if (arg_stack_consistent) {
		/* skip args from incomplete frames */
		while (((cur_arg_pos-1) > EG(argument_stack).elements) && *(cur_arg_pos-1)) {
			cur_arg_pos--;
		}
	}

	while (ptr) {
		char *free_class_name = NULL;

		class_name = call_type = NULL;   
		arg_array = NULL;

		skip = ptr;
		/* skip internal handler */
		if (!skip->op_array &&
		    skip->prev_execute_data &&
		    skip->prev_execute_data->opline &&
		    skip->prev_execute_data->opline->opcode != ZEND_DO_FCALL &&
		    skip->prev_execute_data->opline->opcode != ZEND_DO_FCALL_BY_NAME &&
		    skip->prev_execute_data->opline->opcode != ZEND_INCLUDE_OR_EVAL) {
		  skip = skip->prev_execute_data;
		}

		if (skip->op_array) {
			filename = skip->op_array->filename;
			lineno = skip->opline->lineno;
		} else {
			filename = NULL;
			lineno = 0;
		}

		function_name = ptr->function_state.function->common.function_name;

		if (function_name) {
			if (ptr->object) {
				if (ptr->function_state.function->common.scope) {
					class_name = ptr->function_state.function->common.scope->name;
				} else {
					zend_uint class_name_len;
					int dup;

					dup = zend_get_object_classname(ptr->object, &class_name, &class_name_len TSRMLS_CC);
					if(!dup) {
						free_class_name = class_name;
					}
				}

				call_type = "->";
			} else if (ptr->function_state.function->common.scope) {
				class_name = ptr->function_state.function->common.scope->name;
				call_type = "::";
			} else {
				class_name = NULL;
				call_type = NULL;
			}
			if ((! ptr->opline) || ((ptr->opline->opcode == ZEND_DO_FCALL_BY_NAME) || (ptr->opline->opcode == ZEND_DO_FCALL))) {
				if (arg_stack_consistent && (frames_on_stack > 0)) {
					arg_array = debug_backtrace_get_args(&cur_arg_pos TSRMLS_CC);
					frames_on_stack--;
				}
			}
		} else {
			/* i know this is kinda ugly, but i'm trying to avoid extra cycles in the main execution loop */
			zend_bool build_filename_arg = 1;

			if (!ptr->opline || ptr->opline->opcode != ZEND_INCLUDE_OR_EVAL) {
				/* can happen when calling eval from a custom sapi */
				function_name = "unknown";
				build_filename_arg = 0;
			} else
			switch (Z_LVAL(ptr->opline->op2.u.constant)) {
				case ZEND_EVAL:
					function_name = "eval";
					build_filename_arg = 0;
					break;
				case ZEND_INCLUDE:
					function_name = "include";
					break;
				case ZEND_REQUIRE:
					function_name = "require";
					break;
				case ZEND_INCLUDE_ONCE:
					function_name = "include_once";
					break;
				case ZEND_REQUIRE_ONCE:
					function_name = "require_once";
					break;
				default:
					/* this can actually happen if you use debug_backtrace() in your error_handler and 
					 * you're in the top-scope */
					function_name = "unknown"; 
					build_filename_arg = 0;
					break;
			}

			if (build_filename_arg && include_filename) {
				MAKE_STD_ZVAL(arg_array);
				array_init(arg_array);
				add_next_index_string(arg_array, include_filename, 1);
			}
			call_type = NULL;
		}
		zend_printf("#%-2d ", indent);
		if (class_name) {
			ZEND_PUTS(class_name);
			ZEND_PUTS(call_type);
		}
		zend_printf("%s(", function_name?function_name:"main");
		if (arg_array) {
			debug_print_backtrace_args(arg_array TSRMLS_CC);
			zval_ptr_dtor(&arg_array);
		}
		if (filename) {
			zend_printf(") called at [%s:%d]\n", filename, lineno);
		} else {
			zend_execute_data *prev = skip->prev_execute_data;

			while (prev) {
				if (prev->function_state.function &&
					prev->function_state.function->common.type != ZEND_USER_FUNCTION) {
					prev = NULL;
					break;
				}				    
				if (prev->op_array) {
					zend_printf(") called at [%s:%d]\n", prev->op_array->filename, prev->opline->lineno);
					break;
				}
				prev = prev->prev_execute_data;
			}
			if (!prev) {
				ZEND_PUTS(")\n");
			}
		}
		include_filename = filename;
		ptr = skip->prev_execute_data;
		++indent;
		if (free_class_name) {
			efree(free_class_name);
		}
	}
}

/* }}} */

ZEND_API void zend_fetch_debug_backtrace(zval *return_value, int skip_last, int provide_object TSRMLS_DC)
{
	zend_execute_data *ptr, *skip;
	int lineno;
	char *function_name;
	char *filename;
	char *class_name;
	char *include_filename = NULL;
	zval *stack_frame;
	void **cur_arg_pos = EG(argument_stack).top_element;
	void **args = cur_arg_pos;
	int arg_stack_consistent = 0;
	int frames_on_stack = 0;

	while (--args > EG(argument_stack).elements) {
		if (*args--) {
			break;
		}
		args -= *(ulong*)args;
		frames_on_stack++;

		/* skip args from incomplete frames */
		while (((args-1) > EG(argument_stack).elements) && *(args-1)) {
			args--;
		}

		if ((args-1) == EG(argument_stack).elements) {
			arg_stack_consistent = 1;
			break;
		}
	}

	ptr = EG(current_execute_data);

	/* skip "new Exception()" */
	if (ptr && (skip_last == 0) && ptr->opline && (ptr->opline->opcode == ZEND_NEW)) {
		ptr = ptr->prev_execute_data;
	}

	/* skip debug_backtrace() */
	if (skip_last-- && ptr) {
		int arg_count = *((ulong*)(cur_arg_pos - 2));
		cur_arg_pos -= (arg_count + 2);
		frames_on_stack--;
		ptr = ptr->prev_execute_data;

		if (arg_stack_consistent) {
			/* skip args from incomplete frames */
			while (((cur_arg_pos-1) > EG(argument_stack).elements) && *(cur_arg_pos-1)) {
				cur_arg_pos--;
			}
		}
	}

	array_init(return_value);

	while (ptr) {
		MAKE_STD_ZVAL(stack_frame);
		array_init(stack_frame);

		skip = ptr;
		/* skip internal handler */
		if (!skip->op_array &&
		    skip->prev_execute_data &&
		    skip->prev_execute_data->opline &&
		    skip->prev_execute_data->opline->opcode != ZEND_DO_FCALL &&
		    skip->prev_execute_data->opline->opcode != ZEND_DO_FCALL_BY_NAME &&
		    skip->prev_execute_data->opline->opcode != ZEND_INCLUDE_OR_EVAL) {
			skip = skip->prev_execute_data;
		}

		if (skip->op_array) {
			filename = skip->op_array->filename;
			lineno = skip->opline->lineno;
			add_assoc_string_ex(stack_frame, "file", sizeof("file"), filename, 1);
			add_assoc_long_ex(stack_frame, "line", sizeof("line"), lineno);

			/* try to fetch args only if an FCALL was just made - elsewise we're in the middle of a function
			 * and debug_baktrace() might have been called by the error_handler. in this case we don't 
			 * want to pop anything of the argument-stack */
		} else {
			zend_execute_data *prev = skip->prev_execute_data;

			while (prev) {
				if (prev->function_state.function &&
					prev->function_state.function->common.type != ZEND_USER_FUNCTION) {
					break;
				}				    
				if (prev->op_array) {
					add_assoc_string_ex(stack_frame, "file", sizeof("file"), prev->op_array->filename, 1);
					add_assoc_long_ex(stack_frame, "line", sizeof("line"), prev->opline->lineno);
					break;
				}
				prev = prev->prev_execute_data;
			}
			filename = NULL;
		}

		function_name = ptr->function_state.function->common.function_name;

		if (function_name) {
			add_assoc_string_ex(stack_frame, "function", sizeof("function"), function_name, 1);

			if (ptr->object && Z_TYPE_P(ptr->object) == IS_OBJECT) {
				if (ptr->function_state.function->common.scope) {
					add_assoc_string_ex(stack_frame, "class", sizeof("class"), ptr->function_state.function->common.scope->name, 1);
				} else {
					zend_uint class_name_len;
					int dup;

					dup = zend_get_object_classname(ptr->object, &class_name, &class_name_len TSRMLS_CC);
					add_assoc_string_ex(stack_frame, "class", sizeof("class"), class_name, dup);
					
				}
				if (provide_object) {
					add_assoc_zval_ex(stack_frame, "object", sizeof("object"), ptr->object);
					ptr->object->refcount++;
				}

				add_assoc_string_ex(stack_frame, "type", sizeof("type"), "->", 1);
			} else if (ptr->function_state.function->common.scope) {
				add_assoc_string_ex(stack_frame, "class", sizeof("class"), ptr->function_state.function->common.scope->name, 1);
				add_assoc_string_ex(stack_frame, "type", sizeof("type"), "::", 1);
			}

			if ((! ptr->opline) || ((ptr->opline->opcode == ZEND_DO_FCALL_BY_NAME) || (ptr->opline->opcode == ZEND_DO_FCALL))) {
				if (arg_stack_consistent && (frames_on_stack > 0)) {
					add_assoc_zval_ex(stack_frame, "args", sizeof("args"), debug_backtrace_get_args(&cur_arg_pos TSRMLS_CC));
					frames_on_stack--;
				}
			}
		} else {
			/* i know this is kinda ugly, but i'm trying to avoid extra cycles in the main execution loop */
			zend_bool build_filename_arg = 1;

			if (!ptr->opline || ptr->opline->opcode != ZEND_INCLUDE_OR_EVAL) {
				/* can happen when calling eval from a custom sapi */
				function_name = "unknown";
				build_filename_arg = 0;
			} else
			switch (ptr->opline->op2.u.constant.value.lval) {
				case ZEND_EVAL:
					function_name = "eval";
					build_filename_arg = 0;
					break;
				case ZEND_INCLUDE:
					function_name = "include";
					break;
				case ZEND_REQUIRE:
					function_name = "require";
					break;
				case ZEND_INCLUDE_ONCE:
					function_name = "include_once";
					break;
				case ZEND_REQUIRE_ONCE:
					function_name = "require_once";
					break;
				default:
					/* this can actually happen if you use debug_backtrace() in your error_handler and 
					 * you're in the top-scope */
					function_name = "unknown"; 
					build_filename_arg = 0;
					break;
			}

			if (build_filename_arg && include_filename) {
				zval *arg_array;

				MAKE_STD_ZVAL(arg_array);
				array_init(arg_array);

				/* include_filename always points to the last filename of the last last called-fuction.
				   if we have called include in the frame above - this is the file we have included.
				 */

				add_next_index_string(arg_array, include_filename, 1);
				add_assoc_zval_ex(stack_frame, "args", sizeof("args"), arg_array);
			}

			add_assoc_string_ex(stack_frame, "function", sizeof("function"), function_name, 1);
		}

		add_next_index_zval(return_value, stack_frame);

		include_filename = filename; 

		ptr = skip->prev_execute_data;
	}
}
/* }}} */


/* {{{ proto array debug_backtrace([bool provide_object])
   Return backtrace as array */
ZEND_FUNCTION(debug_backtrace)
{
	zend_bool provide_object = 1;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &provide_object) == FAILURE) {
		return;
	}

	zend_fetch_debug_backtrace(return_value, 1, provide_object TSRMLS_CC);
}
/* }}} */

/* {{{ proto bool extension_loaded(string extension_name)
   Returns true if the named extension is loaded */
ZEND_FUNCTION(extension_loaded)
{
	zval **extension_name;
	char *lcname;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &extension_name)) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_string_ex(extension_name);
	lcname = zend_str_tolower_dup(Z_STRVAL_PP(extension_name), Z_STRLEN_PP(extension_name));
	if (zend_hash_exists(&module_registry, lcname, Z_STRLEN_PP(extension_name)+1)) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}
	efree(lcname);
}
/* }}} */


/* {{{ proto array get_extension_funcs(string extension_name)
   Returns an array with the names of functions belonging to the named extension */
ZEND_FUNCTION(get_extension_funcs)
{
	zval **extension_name;
	zend_module_entry *module;
	zend_function_entry *func;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &extension_name)) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_string_ex(extension_name);
	if (strncasecmp(Z_STRVAL_PP(extension_name), "zend", sizeof("zend"))) {
		char *lcname = zend_str_tolower_dup(Z_STRVAL_PP(extension_name), Z_STRLEN_PP(extension_name));
		if (zend_hash_find(&module_registry, lcname,
			Z_STRLEN_PP(extension_name)+1, (void**)&module) == FAILURE) {
			efree(lcname);
			RETURN_FALSE;
		}
		efree(lcname);

		if (!(func = module->functions)) {
			RETURN_FALSE;
		}
	} else {
		func = builtin_functions;
	}

	array_init(return_value);

	while (func->fname) {
		add_next_index_string(return_value, func->fname, 1);
		func++;
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
