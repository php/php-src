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
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend.h"
#include "zend_API.h"
#include "zend_builtin_functions.h"
#include "zend_constants.h"
#include "zend_ini.h"
#include "zend_extensions.h"

#undef ZEND_TEST_EXCEPTIONS

/* {{{ protos */
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
static ZEND_FUNCTION(get_called_class);
static ZEND_FUNCTION(get_parent_class);
static ZEND_FUNCTION(method_exists);
static ZEND_FUNCTION(property_exists);
static ZEND_FUNCTION(class_exists);
static ZEND_FUNCTION(interface_exists);
static ZEND_FUNCTION(function_exists);
static ZEND_FUNCTION(class_alias);
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
static ZEND_FUNCTION(gc_collect_cycles);
static ZEND_FUNCTION(gc_enabled);
static ZEND_FUNCTION(gc_enable);
static ZEND_FUNCTION(gc_disable);
/* }}} */

/* {{{ arginfo */
static
ZEND_BEGIN_ARG_INFO(arginfo_zend_version, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_func_num_args, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_func_get_arg, 0, 0, 1)
	ZEND_ARG_INFO(0, arg_num)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_func_get_args, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_strlen, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_strcmp, 0, 0, 2)
	ZEND_ARG_INFO(0, str1)
	ZEND_ARG_INFO(0, str2)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_strncmp, 0, 0, 3)
	ZEND_ARG_INFO(0, str1)
	ZEND_ARG_INFO(0, str2)
	ZEND_ARG_INFO(0, len)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_strcasecmp, 0, 0, 2)
	ZEND_ARG_INFO(0, str1)
	ZEND_ARG_INFO(0, str2)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_strncasecmp, 0, 0, 3)
	ZEND_ARG_INFO(0, str1)
	ZEND_ARG_INFO(0, str2)
	ZEND_ARG_INFO(0, len)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_each, 0, 0, 1)
	ZEND_ARG_INFO(1, arr)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_error_reporting, 0, 0, 1)
	ZEND_ARG_INFO(0, new_error_level)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_define, 0, 0, 3)
	ZEND_ARG_INFO(0, constant_name)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, case_sensitive)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_defined, 0, 0, 1)
	ZEND_ARG_INFO(0, constant_name)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_get_class, 0, 0, 0)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_get_called_class, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_get_parent_class, 0, 0, 0)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_is_subclass_of, 0, 0, 2)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, class_name)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_is_a, 0, 0, 2)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, class_name)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_get_class_vars, 0, 0, 1)
	ZEND_ARG_INFO(0, class_name)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_get_object_vars, 0, 0, 1)
	ZEND_ARG_INFO(0, obj)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_get_class_methods, 0, 0, 1)
	ZEND_ARG_INFO(0, class)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_method_exists, 0, 0, 2)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, method)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_property_exists, 0, 0, 2)
	ZEND_ARG_INFO(0, object_or_class)
	ZEND_ARG_INFO(0, property_name)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_exists, 0, 0, 1)
	ZEND_ARG_INFO(0, classname)
	ZEND_ARG_INFO(0, autoload)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_interface_exists, 0, 0, 1)
	ZEND_ARG_INFO(0, classname)
	ZEND_ARG_INFO(0, autoload)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_function_exists, 0, 0, 1)
	ZEND_ARG_INFO(0, function_name)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_alias, 0, 0, 2)
	ZEND_ARG_INFO(0, user_class_name)
	ZEND_ARG_INFO(0, alias_name)
	ZEND_ARG_INFO(0, autoload)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_get_included_files, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_trigger_error, 0, 0, 1)
	ZEND_ARG_INFO(0, messsage)
	ZEND_ARG_INFO(0, error_type)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_set_error_handler, 0, 0, 1)
	ZEND_ARG_INFO(0, error_handler)
	ZEND_ARG_INFO(0, error_types)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_restore_error_handler, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_set_exception_handler, 0, 0, 1)
	ZEND_ARG_INFO(0, exception_handler)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_restore_exception_handler, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_get_declared_classes, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_get_declared_interfaces, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_get_defined_functions, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_get_defined_vars, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_create_function, 0, 0, 2)
	ZEND_ARG_INFO(0, args)
	ZEND_ARG_INFO(0, code)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_get_resource_type, 0, 0, 1)
	ZEND_ARG_INFO(0, res)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_get_loaded_extensions, 0, 0, 0)
	ZEND_ARG_INFO(0, zend_extensions)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_get_defined_constants, 0, 0, 0)
	ZEND_ARG_INFO(0, categorize)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_debug_print_backtrace, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_debug_backtrace, 0, 0, 0)
	ZEND_ARG_INFO(0, provide_object)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_extension_loaded, 0, 0, 1)
	ZEND_ARG_INFO(0, extension_name)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_get_extension_funcs, 0, 0, 1)
	ZEND_ARG_INFO(0, extension_name)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_gc_collect_cycles, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_gc_enabled, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_gc_enable, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_gc_disable, 0)
ZEND_END_ARG_INFO()
/* }}} */

static const zend_function_entry builtin_functions[] = { /* {{{ */
	ZEND_FE(zend_version,		arginfo_zend_version)
	ZEND_FE(func_num_args,		arginfo_func_num_args)
	ZEND_FE(func_get_arg,		arginfo_func_get_arg)
	ZEND_FE(func_get_args,		arginfo_func_get_args)
	{ "strlen", zend_if_strlen, arginfo_strlen },
	ZEND_FE(strcmp,				arginfo_strcmp)
	ZEND_FE(strncmp,			arginfo_strncmp)
	ZEND_FE(strcasecmp,			arginfo_strcasecmp)
	ZEND_FE(strncasecmp,		arginfo_strncasecmp)
	ZEND_FE(each,				arginfo_each)
	ZEND_FE(error_reporting,	arginfo_error_reporting)
	ZEND_FE(define,				arginfo_define)
	ZEND_FE(defined,			arginfo_defined)
	ZEND_FE(get_class,			arginfo_get_class)
	ZEND_FE(get_called_class,	arginfo_get_called_class)
	ZEND_FE(get_parent_class,	arginfo_get_parent_class)
	ZEND_FE(method_exists,		arginfo_method_exists)
	ZEND_FE(property_exists,	arginfo_property_exists)
	ZEND_FE(class_exists,		arginfo_class_exists)
	ZEND_FE(interface_exists,	arginfo_interface_exists)
	ZEND_FE(function_exists,	arginfo_function_exists)
	ZEND_FE(class_alias,		arginfo_class_alias)
#if ZEND_DEBUG
	ZEND_FE(leak,				NULL)
#ifdef ZEND_TEST_EXCEPTIONS
	ZEND_FE(crash,				NULL)
#endif
#endif
	ZEND_FE(get_included_files,	arginfo_get_included_files)
	ZEND_FALIAS(get_required_files,	get_included_files,		arginfo_get_included_files)
	ZEND_FE(is_subclass_of,		arginfo_is_subclass_of)
	ZEND_FE(is_a,				arginfo_is_a)
	ZEND_FE(get_class_vars,		arginfo_get_class_vars)
	ZEND_FE(get_object_vars,	arginfo_get_object_vars)
	ZEND_FE(get_class_methods,	arginfo_get_class_methods)
	ZEND_FE(trigger_error,		arginfo_trigger_error)
	ZEND_FALIAS(user_error,		trigger_error,		arginfo_trigger_error)
	ZEND_FE(set_error_handler,		arginfo_set_error_handler)
	ZEND_FE(restore_error_handler,	arginfo_restore_error_handler)
	ZEND_FE(set_exception_handler,		arginfo_set_exception_handler)
	ZEND_FE(restore_exception_handler,	arginfo_restore_exception_handler)
	ZEND_FE(get_declared_classes, arginfo_get_declared_classes)
	ZEND_FE(get_declared_interfaces, arginfo_get_declared_interfaces)
	ZEND_FE(get_defined_functions, arginfo_get_defined_functions)
	ZEND_FE(get_defined_vars,	arginfo_get_defined_vars)
	ZEND_FE(create_function,	arginfo_create_function)
	ZEND_FE(get_resource_type,	arginfo_get_resource_type)
	ZEND_FE(get_loaded_extensions,		arginfo_get_loaded_extensions)
	ZEND_FE(extension_loaded,			arginfo_extension_loaded)
	ZEND_FE(get_extension_funcs,		arginfo_get_extension_funcs)
	ZEND_FE(get_defined_constants,		arginfo_get_defined_constants)
	ZEND_FE(debug_backtrace, arginfo_debug_backtrace)
	ZEND_FE(debug_print_backtrace, arginfo_debug_print_backtrace)
#if ZEND_DEBUG
	ZEND_FE(zend_test_func,		NULL)
#ifdef ZTS
	ZEND_FE(zend_thread_id,		NULL)
#endif
#endif
	ZEND_FE(gc_collect_cycles, arginfo_gc_collect_cycles)
	ZEND_FE(gc_enabled, arginfo_gc_enabled)
	ZEND_FE(gc_enable, arginfo_gc_enable)
	ZEND_FE(gc_disable, arginfo_gc_disable)
	{ NULL, NULL, NULL }
};
/* }}} */

int zend_startup_builtin_functions(TSRMLS_D) /* {{{ */
{
	return zend_register_functions(NULL, builtin_functions, NULL, MODULE_PERSISTENT TSRMLS_CC);
}
/* }}} */

void zend_shutdown_builtin_functions(TSRMLS_D) /* {{{ */
{
	zend_unregister_functions(builtin_functions, -1, NULL TSRMLS_CC);
}
/* }}} */

/* {{{ proto string zend_version(void) U
   Get the version of the Zend Engine */
ZEND_FUNCTION(zend_version)
{
	RETURN_ASCII_STRINGL(ZEND_VERSION, sizeof(ZEND_VERSION)-1, 1);
}
/* }}} */

/* {{{ proto int func_num_args(void) U
   Get the number of arguments that were passed to the function */
ZEND_FUNCTION(func_num_args)
{
	zend_execute_data *ex = EG(current_execute_data)->prev_execute_data;

	if (ex && ex->function_state.arguments) {
		RETURN_LONG((long)(zend_uintptr_t)*(ex->function_state.arguments));
	} else {
		zend_error(E_WARNING, "func_num_args():  Called from the global scope - no function context");
		RETURN_LONG(-1);
	}
}
/* }}} */

/* {{{ proto mixed func_get_arg(int arg_num) U
   Get the $arg_num'th argument that was passed to the function */
ZEND_FUNCTION(func_get_arg)
{
	void **p;
	int arg_count;
	zval *arg;
	long requested_offset;
	zend_execute_data *ex = EG(current_execute_data)->prev_execute_data;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &requested_offset) == FAILURE) {
		return;
	}

	if (requested_offset < 0) {
		zend_error(E_WARNING, "func_get_arg():  The argument number should be >= 0");
		RETURN_FALSE;
	}

	if (!ex || !ex->function_state.arguments) {
		zend_error(E_WARNING, "func_get_arg():  Called from the global scope - no function context");
		RETURN_FALSE;
	}

	p = ex->function_state.arguments;
	arg_count = (int)(zend_uintptr_t) *p;		/* this is the amount of arguments passed to func_get_arg(); */

	if (requested_offset >= arg_count) {
		zend_error(E_WARNING, "func_get_arg():  Argument %ld not passed to function", requested_offset);
		RETURN_FALSE;
	}

	arg = *(p-(arg_count-requested_offset));
	*return_value = *arg;
	zval_copy_ctor(return_value);
	INIT_PZVAL(return_value);
}
/* }}} */

/* {{{ proto array func_get_args() U
   Get an array of the arguments that were passed to the function */
ZEND_FUNCTION(func_get_args)
{
	void **p;
	int arg_count;
	int i;
	zend_execute_data *ex = EG(current_execute_data)->prev_execute_data;

	if (!ex || !ex->function_state.arguments) {
		zend_error(E_WARNING, "func_get_args():  Called from the global scope - no function context");
		RETURN_FALSE;
	}

	p = ex->function_state.arguments;
	arg_count = (int)(zend_uintptr_t) *p;		/* this is the amount of arguments passed to func_get_args(); */

	array_init_size(return_value, arg_count);
	for (i=0; i<arg_count; i++) {
		zval *element;

		ALLOC_ZVAL(element);
		*element = **((zval **) (p-(arg_count-i)));
		zval_copy_ctor(element);
		INIT_PZVAL(element);
		zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &element, sizeof(zval *), NULL);
	}
}
/* }}} */

/* {{{ proto int strlen(string str) U
   Get string length */
ZEND_NAMED_FUNCTION(zend_if_strlen)
{
	zstr s1;
	int s1_len;
	zend_uchar s1_type;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "t", &s1, &s1_len, &s1_type) == FAILURE) {
		return;
	}

	if(s1_type == IS_STRING) {
		RETVAL_LONG(s1_len);
	} else {
		RETVAL_LONG(u_countChar32(s1.u, s1_len));
	}
}
/* }}} */

/* {{{ proto int strcmp(string str1, string str2) U
   Binary safe string comparison */
ZEND_FUNCTION(strcmp)
{
	void *s1, *s2;
	int s1_len, s2_len;
	zend_uchar s1_type, s2_type;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "TT", &s1, &s1_len,
							  &s1_type, &s2, &s2_len, &s2_type) == FAILURE) {
		return;
	}
	if (s1_type == IS_UNICODE) {
		RETURN_LONG(zend_u_binary_strcmp(s1, s1_len, s2, s2_len));
	} else {
		RETURN_LONG(zend_binary_strcmp(s1, s1_len, s2, s2_len));
	}
}
/* }}} */

/* {{{ proto int strncmp(string str1, string str2, int len) U
   Binary safe string comparison */
ZEND_FUNCTION(strncmp)
{
	void *s1, *s2;
	int s1_len, s2_len;
	long count;
	zend_uchar s1_type, s2_type;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "TTl", &s1, &s1_len,
							  &s1_type, &s2, &s2_len, &s2_type, &count) == FAILURE) {
		return;
	}

	if (count < 0) {
		zend_error(E_WARNING, "Length must be greater than or equal to 0");
		RETURN_FALSE;
	}
	
	if (s1_type == IS_UNICODE) {
		RETURN_LONG(zend_u_binary_strncmp(s1, s1_len, s2, s2_len, count));
	} else {
		RETURN_LONG(zend_binary_strncmp(s1, s1_len, s2, s2_len, count));
	}
}
/* }}} */

/* {{{ proto int strcasecmp(string str1, string str2) U
   Binary safe case-insensitive string comparison */
ZEND_FUNCTION(strcasecmp)
{
	void *s1, *s2;
	int s1_len, s2_len;
	zend_uchar s1_type, s2_type;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "TT", &s1, &s1_len,
							  &s1_type, &s2, &s2_len, &s2_type) == FAILURE) {
		return;
	}
	if (s1_type == IS_UNICODE) {
		RETURN_LONG(zend_u_binary_strcasecmp(s1, s1_len, s2, s2_len));
	} else {
		RETURN_LONG(zend_binary_strcasecmp(s1, s1_len, s2, s2_len));
	}
}
/* }}} */

/* {{{ proto int strncasecmp(string str1, string str2, int len) U
   Binary safe string comparison */
ZEND_FUNCTION(strncasecmp)
{
	void *s1, *s2;
	int s1_len, s2_len;
	zend_uchar s1_type, s2_type;
	long len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "TTl", &s1, &s1_len,
							  &s1_type, &s2, &s2_len, &s2_type, &len) == FAILURE) {
		return;
	}

	if (len < 0) {
		zend_error(E_WARNING, "Length must be greater than or equal to 0");
		RETURN_FALSE;
	}

	if (s1_type == IS_UNICODE) {
		RETURN_LONG(zend_u_binary_strncasecmp(s1, s1_len, s2, s2_len, len));
	} else {
		RETURN_LONG(zend_binary_strcasecmp(s1, MIN(s1_len, len), s2, MIN(s2_len, len)));
	}
}
/* }}} */

/* {{{ proto array each(array arr) U
   Return the currently pointed key..value pair in the passed array, and advance the pointer to the next element */
ZEND_FUNCTION(each)
{
	zval *array, *entry, **entry_ptr, *tmp;
	zstr string_key;
	uint string_key_len;
	ulong num_key;
	zval **inserted_pointer;
	HashTable *target_hash;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &array) == FAILURE) {
		return;
	}

	target_hash = HASH_OF(array);
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
	if (Z_ISREF_P(entry)) {
		ALLOC_ZVAL(tmp);
		*tmp = *entry;
		zval_copy_ctor(tmp);
		Z_UNSET_ISREF_P(tmp);
		Z_SET_REFCOUNT_P(tmp, 0);
		entry=tmp;
	}
	zend_hash_index_update(Z_ARRVAL_P(return_value), 1, &entry, sizeof(zval *), NULL);
	Z_ADDREF_P(entry);
	zend_ascii_hash_update(Z_ARRVAL_P(return_value), "value", sizeof("value"), &entry, sizeof(zval *), NULL);
	Z_ADDREF_P(entry);

	/* add the key elements */
	switch (zend_hash_get_current_key_ex(target_hash, &string_key, &string_key_len, &num_key, 1, NULL)) {
		case HASH_KEY_IS_STRING:
			add_get_index_stringl(return_value, 0, string_key.s, string_key_len-1, (void **) &inserted_pointer, 0);
			break;
		case HASH_KEY_IS_UNICODE:
			add_get_index_unicodel(return_value, 0, string_key.u, string_key_len-1, (void **) &inserted_pointer, 0);
			break;
		case HASH_KEY_IS_LONG:
			add_get_index_long(return_value, 0, num_key, (void **) &inserted_pointer);
			break;
	}
	zend_ascii_hash_update(Z_ARRVAL_P(return_value), "key", sizeof("key"), inserted_pointer, sizeof(zval *), NULL);
	Z_ADDREF_PP(inserted_pointer);
	zend_hash_move_forward(target_hash);
}
/* }}} */

/* {{{ proto int error_reporting(int new_error_level=null) U
   Return the current error_reporting level, and if an argument was passed - change to the new level */
ZEND_FUNCTION(error_reporting)
{
	char *err;
	int err_len;
	int old_error_reporting;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &err, &err_len) == FAILURE) {
		return;
	}

	old_error_reporting = EG(error_reporting);
	if(ZEND_NUM_ARGS() != 0) {
		zend_alter_ini_entry("error_reporting", sizeof("error_reporting"), err, err_len, ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME);
	}

	RETVAL_LONG(old_error_reporting);
}
/* }}} */

/* {{{ proto bool define(string constant_name, mixed value, boolean case_sensitive=true) U
   Define a new constant */
ZEND_FUNCTION(define)
{
	zstr name;
	int name_len;
	zend_uchar name_type;
	zval *val, *val_free = NULL;
	zend_bool non_cs = 0;
	int case_sensitive = CONST_CS;
	zend_constant c;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "tz|b", &name, &name_len, &name_type, &val, &non_cs) == FAILURE) {
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
		case IS_UNICODE:
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
					if (Z_OBJ_HT_P(val)->cast_object(val, val_free, UG(unicode)?IS_UNICODE:IS_STRING, NULL TSRMLS_CC) == SUCCESS) {
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
	if (name_type == IS_UNICODE) {
		c.name.u = zend_ustrndup(name.u, name_len);
	} else {
		c.name.s = zend_strndup(name.s, name_len);
	}
	c.name_len = name_len+1;
	c.module_number = PHP_USER_CONSTANT;
	if (zend_u_register_constant(name_type, &c TSRMLS_CC) == SUCCESS) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool defined(string constant_name) U
   Check whether a constant exists */
ZEND_FUNCTION(defined)
{
	zstr name;
	int name_len;
	zend_uchar name_type;
	zval c;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "t", &name, &name_len, &name_type) == FAILURE) {
		return;
	}

	if (zend_u_get_constant_ex(name_type, name, name_len, &c, NULL, ZEND_FETCH_CLASS_SILENT TSRMLS_CC)) {
		zval_dtor(&c);
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string get_class([object object]) U
   Retrieves the class name */
ZEND_FUNCTION(get_class)
{
	zval *obj = NULL;
	zstr name = EMPTY_ZSTR;
	zend_uint name_len = 0;
	int dup;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|o", &obj) == FAILURE) {
		return;
	}

	if (!obj) {
		if (EG(scope)) {
			RETURN_TEXTL(EG(scope)->name, EG(scope)->name_length, 1);
		} else {
			zend_error(E_WARNING, "get_class() called without object from outside a class");
			RETURN_FALSE;
		}
	}

	dup = zend_get_object_classname(obj, &name, &name_len TSRMLS_CC);

	RETURN_TEXTL(name, name_len, dup);
}
/* }}} */

/* {{{ proto string get_called_class() U
   Retrieves the class name */
ZEND_FUNCTION(get_called_class)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (EG(called_scope)) {
		RETURN_TEXTL(EG(called_scope)->name, EG(called_scope)->name_length, 1);
	} else {
		zend_error(E_WARNING, "get_called_class() called from outside a class");
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto string get_parent_class([mixed object]) U
   Retrieves the parent class name for object or class or current scope. */
ZEND_FUNCTION(get_parent_class)
{
	zval *arg;
	zend_class_entry *ce = NULL;
	zstr name;
	zend_uint name_length;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z", &arg) == FAILURE) {
		return;
	}

	if (!ZEND_NUM_ARGS()) {
		ce = EG(scope);
		if (ce && ce->parent) {
			RETURN_TEXTL(ce->parent->name, ce->parent->name_length, 1);
		} else {
			RETURN_FALSE;
		}
	}

	if (Z_TYPE_P(arg) == IS_OBJECT) {
		if (Z_OBJ_HT_P(arg)->get_class_name
			&& Z_OBJ_HT_P(arg)->get_class_name(arg, &name, &name_length, 1 TSRMLS_CC) == SUCCESS) {
			RETURN_TEXTL(name, name_length, 0);
		} else {
			ce = zend_get_class_entry(arg TSRMLS_CC);
		}
	} else if (Z_TYPE_P(arg) == (UG(unicode)?IS_UNICODE:IS_STRING)) {
		zend_class_entry **pce;

		if (zend_u_lookup_class(Z_TYPE_P(arg), Z_UNIVAL_P(arg), Z_UNILEN_P(arg), &pce TSRMLS_CC) == SUCCESS) {
			ce = *pce;
		}
	}

	if (ce && ce->parent) {
		RETURN_TEXTL(ce->parent->name, ce->parent->name_length, 1);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

static void is_a_impl(INTERNAL_FUNCTION_PARAMETERS, zend_bool only_subclass) /* {{{ */
{
	zval *obj;
	zstr class_name;
	int class_name_len;
	zend_uchar class_name_type;
	zend_class_entry *instance_ce;
	zend_class_entry **ce;
	zend_bool retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zt", &obj, &class_name, &class_name_len, &class_name_type) == FAILURE) {
		return;
	}

	if (only_subclass && Z_TYPE_P(obj) == (UG(unicode)?IS_UNICODE:IS_STRING)) {
		zend_class_entry **the_ce;
		if (zend_u_lookup_class(Z_TYPE_P(obj), Z_UNIVAL_P(obj), Z_UNILEN_P(obj), &the_ce TSRMLS_CC) == FAILURE) {
			zend_error(E_WARNING, "Unknown class passed as parameter");
			RETURN_FALSE;
		}
		instance_ce = *the_ce;
	} else if (Z_TYPE_P(obj) != IS_OBJECT) {
		RETURN_FALSE;
	} else {
		instance_ce = NULL;
	}

	/* TBI!! new object handlers */
	if (Z_TYPE_P(obj) == IS_OBJECT && !HAS_CLASS_ENTRY(*obj)) {
		RETURN_FALSE;
	}

	if (zend_u_lookup_class_ex(class_name_type, class_name, class_name_len, NULL_ZSTR, 1, &ce TSRMLS_CC) == FAILURE) {
		retval = 0;
	} else {
		if (only_subclass) {
			if (!instance_ce) {
				instance_ce = Z_OBJCE_P(obj)->parent;
			} else {
				instance_ce = instance_ce->parent;
			}
		} else {
			instance_ce = Z_OBJCE_P(obj);
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
/* }}} */

/* {{{ proto bool is_subclass_of(object object, string class_name) U
   Returns true if the object has this class as one of its parents */
ZEND_FUNCTION(is_subclass_of)
{
	is_a_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto bool is_a(object object, string class_name) U
   Returns true if the object is of this class or has this class as one of its parents */
ZEND_FUNCTION(is_a)
{
	is_a_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ add_class_vars */
static void add_class_vars(zend_class_entry *ce, HashTable *properties, zval *return_value TSRMLS_DC)
{
	int instanceof = EG(scope) && instanceof_function(EG(scope), ce TSRMLS_CC);

	if (zend_hash_num_elements(properties) > 0) {
		HashPosition pos;
		zval **prop;

		zend_hash_internal_pointer_reset_ex(properties, &pos);
		while (zend_hash_get_current_data_ex(properties, (void **) &prop, &pos) == SUCCESS) {
			zstr key, class_name, prop_name;
			uint key_len;
			ulong num_index;
			zval *prop_copy;
			zend_uchar key_type;

			key_type = zend_hash_get_current_key_ex(properties, &key, &key_len, &num_index, 0, &pos);
			zend_hash_move_forward_ex(properties, &pos);
			zend_u_unmangle_property_name(key_type, key, key_len-1, &class_name, &prop_name);
			if (class_name.v) {
				if (class_name.s[0] != '*' && strcmp(class_name.s, ce->name.s)) {
					/* filter privates from base classes */
					continue;
				} else if (!instanceof) {
					/* filter protected if not inside class */
					continue;
				}
			}

			/* copy: enforce read only access */
			ALLOC_ZVAL(prop_copy);
			*prop_copy = **prop;
			zval_copy_ctor(prop_copy);
			INIT_PZVAL(prop_copy);

			/* this is necessary to make it able to work with default array
			* properties, returned to user */
			if (Z_TYPE_P(prop_copy) == IS_CONSTANT_ARRAY || (Z_TYPE_P(prop_copy) & IS_CONSTANT_TYPE_MASK) == IS_CONSTANT) {
				zval_update_constant(&prop_copy, 0 TSRMLS_CC);
			}

			add_u_assoc_zval(return_value, key_type==HASH_KEY_IS_UNICODE?IS_UNICODE:IS_STRING, prop_name, prop_copy);
		}
	}
}
/* }}} */

/* {{{ proto array get_class_vars(string class_name) U
   Returns an array of default properties of the class. */
ZEND_FUNCTION(get_class_vars)
{
	zstr class_name;
	int class_name_len;
	zend_uchar type;
	zend_class_entry **pce;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "t", &class_name, &class_name_len, &type) == FAILURE) {
		return;
	}

	if (zend_u_lookup_class(type, class_name, class_name_len, &pce TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	} else {
		array_init(return_value);
		zend_update_class_constants(*pce TSRMLS_CC);
		add_class_vars(*pce, &(*pce)->default_properties, return_value TSRMLS_CC);
		add_class_vars(*pce, CE_STATIC_MEMBERS(*pce), return_value TSRMLS_CC);
	}
}
/* }}} */

/* {{{ proto array get_object_vars(object obj) U
   Returns an array of object properties */
ZEND_FUNCTION(get_object_vars)
{
	zval *obj;
	zval **value;
	HashTable *properties;
	HashPosition pos;
	zstr key, prop_name, class_name;
	uint key_len;
	ulong num_index;
	zend_object *zobj;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &obj) == FAILURE) {
		return;
	}

	if (Z_OBJ_HT_P(obj)->get_properties == NULL) {
		RETURN_FALSE;
	}

	properties = Z_OBJ_HT_P(obj)->get_properties(obj TSRMLS_CC);

	if (properties == NULL) {
		RETURN_FALSE;
	}

	zobj = zend_objects_get_address(obj TSRMLS_CC);

	array_init(return_value);

	zend_hash_internal_pointer_reset_ex(properties, &pos);

	while (zend_hash_get_current_data_ex(properties, (void **) &value, &pos) == SUCCESS) {
		if (zend_hash_get_current_key_ex(properties, &key, &key_len, &num_index, 0, &pos) == (UG(unicode)?HASH_KEY_IS_UNICODE:HASH_KEY_IS_STRING)) {
			if (zend_check_property_access(zobj, ZEND_STR_TYPE, key, key_len-1 TSRMLS_CC) == SUCCESS) {
				zend_u_unmangle_property_name(ZEND_STR_TYPE, key, key_len-1, &class_name, &prop_name);
				/* Not separating references */
				Z_ADDREF_PP(value);
				add_u_assoc_zval(return_value, ZEND_STR_TYPE, prop_name, *value);
			}
		}
		zend_hash_move_forward_ex(properties, &pos);
	}
}
/* }}} */

/* {{{ proto array get_class_methods(mixed class) U
   Returns an array of method names for class or class instance. */
ZEND_FUNCTION(get_class_methods)
{
	zval *klass;
	zval *method_name;
	zend_class_entry *ce = NULL, **pce;
	HashPosition pos;
	zend_function *mptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &klass) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(klass) == IS_OBJECT) {
		/* TBI!! new object handlers */
		if (!HAS_CLASS_ENTRY(*klass)) {
			RETURN_FALSE;
		}
		ce = Z_OBJCE_P(klass);
	} else if (Z_TYPE_P(klass) == (UG(unicode)?IS_UNICODE:IS_STRING)) {
		if (zend_u_lookup_class(Z_TYPE_P(klass), Z_UNIVAL_P(klass), Z_UNILEN_P(klass), &pce TSRMLS_CC) == SUCCESS) {
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
			zstr key;
			uint key_len;
			ulong num_index;
			uint len = UG(unicode)?u_strlen(mptr->common.function_name.u):strlen(mptr->common.function_name.s);

			/* Do not display old-style inherited constructors */
			if ((mptr->common.fn_flags & ZEND_ACC_CTOR) == 0 ||
			    mptr->common.scope == ce ||
			    zend_hash_get_current_key_ex(&ce->function_table, &key, &key_len, &num_index, 0, &pos) != (UG(unicode)?HASH_KEY_IS_UNICODE:HASH_KEY_IS_STRING) ||
			    (UG(unicode) ? 
			     (zend_u_binary_strcasecmp(key.u, key_len-1, mptr->common.function_name.u, len) == 0) :
			     (zend_binary_strcasecmp(key.s, key_len-1, mptr->common.function_name.s, len) == 0))) {

				MAKE_STD_ZVAL(method_name);
				ZVAL_TEXT(method_name, mptr->common.function_name, 1);
				zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &method_name, sizeof(zval *), NULL);
			}
		}
		zend_hash_move_forward_ex(&ce->function_table, &pos);
	}
}
/* }}} */

/* {{{ proto bool method_exists(object object, string method) U
   Checks if the class method exists */
ZEND_FUNCTION(method_exists)
{
	zval *klass;
	zstr method_name;
	int method_len;
	zend_uchar method_type;
	unsigned int lcname_len;
	zstr lcname;
	zend_class_entry * ce, **pce;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zt", &klass, &method_name, &method_len, &method_type) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(klass) == IS_OBJECT) {
		ce = Z_OBJCE_P(klass);
	} else if (Z_TYPE_P(klass) == (UG(unicode)?IS_UNICODE:IS_STRING)) {
		if (zend_u_lookup_class(Z_TYPE_P(klass), Z_UNIVAL_P(klass), Z_UNILEN_P(klass), &pce TSRMLS_CC) == FAILURE) {
			RETURN_FALSE;
		}
		ce = *pce;
	} else {
		RETURN_FALSE;
	}

	lcname = zend_u_str_case_fold(method_type, method_name, method_len, 1, &lcname_len);
	if (zend_u_hash_exists(&ce->function_table, method_type, lcname, lcname_len+1)) {
		efree(lcname.v);
		RETURN_TRUE;
	} else {
		union _zend_function *func = NULL;
		efree(lcname.v);

		if (Z_TYPE_P(klass) == IS_OBJECT
		&& Z_OBJ_HT_P(klass)->get_method != NULL
		&& (func = Z_OBJ_HT_P(klass)->get_method(&klass, method_name, method_len TSRMLS_CC)) != NULL
		) {
			if (func->type == ZEND_INTERNAL_FUNCTION
			&& ((zend_internal_function*)func)->handler == zend_std_call_user_call
			) {
				efree(((zend_internal_function*)func)->function_name.v);
				efree(func);
				RETURN_FALSE;
			}
			RETURN_TRUE;
		}
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool property_exists(mixed object_or_class, string property_name) U
   Checks if the object or class has a property */
ZEND_FUNCTION(property_exists)
{
	zval *object;
	zstr property;
	int property_len;
	zend_uchar property_type;
	zend_class_entry *ce, **pce;
	zend_property_info *property_info;
	zstr prop_name, class_name;
	zval property_z;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zt", &object, &property, &property_len, &property_type) == FAILURE) {
		return;
	}

	if (property_len == 0) {
		RETURN_FALSE;
	}

	ZVAL_ZSTRL(&property_z, property_type, property, property_len, 0);

	switch(Z_TYPE_P(object)) {
	case IS_STRING:
	case IS_UNICODE:
		if (!Z_UNILEN_P(object)) {
			RETURN_FALSE;
		}
		if (zend_u_lookup_class(Z_TYPE_P(object), Z_UNIVAL_P(object), Z_UNILEN_P(object), &pce TSRMLS_CC) == SUCCESS) {
			ce = *pce;
		} else {
			RETURN_FALSE;
		}
		if (!ce) {
			RETURN_NULL();
		}
		if (!(property_info = zend_get_property_info(ce, &property_z, 1 TSRMLS_CC)) || property_info == &EG(std_property_info)) {
			RETURN_FALSE;
		}
		if (property_info->flags & ZEND_ACC_PUBLIC) {
			RETURN_TRUE;
		}
		zend_u_unmangle_property_name(property_type, property_info->name, property_info->name_length, &class_name, &prop_name);
		if (class_name.s[0] ==  '*') {
			if (instanceof_function(EG(scope), ce TSRMLS_CC) ||
				(EG(This) && instanceof_function(Z_OBJCE_P(EG(This)), ce TSRMLS_CC))) {
				RETURN_TRUE;
			}
			RETURN_FALSE;
		}
		if (zend_u_lookup_class(Z_TYPE_P(object), Z_UNIVAL_P(object), Z_UNILEN_P(object), &pce TSRMLS_CC) == SUCCESS) {
			ce = *pce;
		} else {
			RETURN_FALSE; /* shouldn't happen */
		}
		RETURN_BOOL(EG(scope) == ce);

	case IS_OBJECT:
		if (Z_OBJ_HANDLER_P(object, has_property) && Z_OBJ_HANDLER_P(object, has_property)(object, &property_z, 2 TSRMLS_CC)) {
			RETURN_TRUE;
		}
		RETURN_FALSE;

	default:
		zend_error(E_WARNING, "First parameter must either be an object or the name of an existing class");
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ proto bool class_exists(string classname [, bool autoload]) U
   Checks if the class exists */
ZEND_FUNCTION(class_exists)
{
	unsigned int lc_name_len;
	zstr class_name, lc_name;
	zend_class_entry **ce;
	int class_name_len;
	zend_bool autoload = 1;
	zend_uchar type;
	int found;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "t|b", &class_name, &class_name_len, &type, &autoload) == FAILURE) {
		return;
	}

	if (!autoload) {
		lc_name = zend_u_str_case_fold(type, class_name, class_name_len, 1, &lc_name_len);
		found = zend_u_hash_find(EG(class_table), type, lc_name, lc_name_len+1, (void **) &ce);
		efree(lc_name.v);
		RETURN_BOOL(found == SUCCESS && !((*ce)->ce_flags & ZEND_ACC_INTERFACE));
	}

 	if (zend_u_lookup_class(type, class_name, class_name_len, &ce TSRMLS_CC) == SUCCESS) {
 		RETURN_BOOL(((*ce)->ce_flags & ZEND_ACC_INTERFACE) == 0);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool interface_exists(string classname [, bool autoload]) U
   Checks if the class exists */
ZEND_FUNCTION(interface_exists)
{
	unsigned int lc_name_len;
	zstr iface_name, lc_name;
	zend_class_entry **ce;
	int iface_name_len;
	zend_uchar type;
	zend_bool autoload = 1;
	int found;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "t|b", &iface_name, &iface_name_len, &type, &autoload) == FAILURE) {
		return;
	}

	if (!autoload) {
		lc_name = zend_u_str_case_fold(type, iface_name, iface_name_len, 1, &lc_name_len);
		found = zend_u_hash_find(EG(class_table), type, lc_name, lc_name_len+1, (void **) &ce);
		efree(lc_name.v);
		RETURN_BOOL(found == SUCCESS && (*ce)->ce_flags & ZEND_ACC_INTERFACE);
	}

 	if (zend_u_lookup_class(type, iface_name, iface_name_len, &ce TSRMLS_CC) == SUCCESS) {
 		RETURN_BOOL(((*ce)->ce_flags & ZEND_ACC_INTERFACE) > 0);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool function_exists(string function_name) U
   Checks if the function exists */
ZEND_FUNCTION(function_exists)
{
	zend_function *func;
	unsigned int lcname_len;
	zstr lcname, function_name;
	int function_name_len;
	zend_uchar function_name_type;
	zend_bool retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "t", &function_name, &function_name_len, &function_name_type) == FAILURE) {
		return;
	}

	lcname = zend_u_str_case_fold(function_name_type, function_name, function_name_len, 1, &lcname_len);

	retval = (zend_u_hash_find(EG(function_table), function_name_type, lcname, lcname_len+1, (void **)&func) == SUCCESS);

	efree(lcname.v);

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

/* {{{ proto bool class_alias(string user_class_name , string alias_name [, bool autoload])
   Creates an alias for user defined class */
ZEND_FUNCTION(class_alias)
{
	zend_uchar class_type, alias_type;
	zstr class_name, lc_name, alias_name;
	zend_class_entry **ce;
	unsigned int lc_name_len;
	int class_name_len, alias_name_len;
	int found;
	zend_bool autoload = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "tt|b", &class_name, &class_name_len, &class_type, &alias_name, &alias_name_len, &alias_type, &autoload) == FAILURE) {
		return;
	}

	if (!autoload) {
		lc_name = zend_u_str_case_fold(class_type, class_name, class_name_len, 1, &lc_name_len);	
		found = zend_u_hash_find(EG(class_table), class_type, lc_name, lc_name_len+1, (void **) &ce);
		efree(lc_name.v);
	} else {
		found = zend_u_lookup_class(class_type, class_name, class_name_len, &ce TSRMLS_CC);
	}
	if (found == SUCCESS) {
		if ((*ce)->type == ZEND_USER_CLASS) { 
			if (zend_u_register_class_alias_ex(alias_type, alias_name, alias_name_len, *ce TSRMLS_CC) == SUCCESS) {
				RETURN_TRUE;
			} else {
				zend_error(E_WARNING, "Cannot redeclare class %R", alias_type, alias_name);
				RETURN_FALSE;
			}
		} else {
			zend_error(E_WARNING, "First argument of class_alias() must be a name of user defined class");
			RETURN_FALSE;
		}
	} else {
		zend_error(E_WARNING, "Class '%R' not found", class_type, class_name);
		RETURN_FALSE;
	}
}
/* }}} */

#if ZEND_DEBUG
/* {{{ proto void leak(int num_bytes=3) U
   Cause an intentional memory leak, for testing/debugging purposes */
ZEND_FUNCTION(leak)
{
	long leakbytes = 3;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &leakbytes) == FAILURE) {
		return;
	}

	emalloc(leakbytes);
}
/* }}} */

#ifdef ZEND_TEST_EXCEPTIONS
/* {{{ proto void crash(void) U
Cause the process to crash by copying data to an inaccesible location */
ZEND_FUNCTION(crash)
{
	char *nowhere=NULL;

	memcpy(nowhere, "something", sizeof("something"));
}
/* }}} */
#endif

#endif /* ZEND_DEBUG */

/* {{{ proto array get_included_files(void) U
   Returns an array with the file names that were included (includes require and once varieties) */
ZEND_FUNCTION(get_included_files)
{
	HashPosition pos;
	UChar *ustr;
	zstr entry;
	unsigned int len;
	int ustr_len;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	array_init(return_value);
	zend_hash_internal_pointer_reset_ex(&EG(included_files), &pos);
	while (zend_hash_get_current_key_ex(&EG(included_files), &entry, &len, NULL, 0, &pos) == HASH_KEY_IS_STRING) {
		if (UG(unicode) && SUCCESS == zend_path_decode(&ustr, &ustr_len, entry.s, len - 1 TSRMLS_CC)) {
			add_next_index_unicodel(return_value, ustr, ustr_len, 0);
		} else {
			add_next_index_stringl(return_value, entry.s, len - 1, 1);
		}
		zend_hash_move_forward_ex(&EG(included_files), &pos);
	}
}
/* }}} */

/* {{{ proto void trigger_error(string messsage [, int error_type]) U
   Generates a user-level error/warning/notice message */
ZEND_FUNCTION(trigger_error)
{
	long error_type = E_USER_NOTICE;
	zstr message;
	int message_len;
	zend_uchar message_type;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "t|l", &message, &message_len, &message_type, &error_type) == FAILURE) {
		return;
	}

	switch (error_type) {
		case E_USER_ERROR:
		case E_USER_WARNING:
		case E_USER_NOTICE:
		case E_USER_DEPRECATED:
			break;
		default:
			zend_error(E_WARNING, "Invalid error type specified");
			RETURN_FALSE;
			break;
	}

	zend_error((int)error_type, "%R", message_type, message);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string set_error_handler(string error_handler [, int error_types]) U
   Sets a user-defined error handler function.  Returns the previously defined error handler, or false on error */
ZEND_FUNCTION(set_error_handler)
{
	zval *error_handler;
	zend_bool had_orig_error_handler=0;
	zval error_handler_name;
	long error_type = E_ALL | E_STRICT;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|l", &error_handler, &error_type) == FAILURE) {
		return;
	}

	if (!zend_is_callable(error_handler, 0, &error_handler_name TSRMLS_CC)) {
		zend_error(E_WARNING, "%v() expects the argument (%R) to be a valid callback",
				   get_active_function_name(TSRMLS_C), Z_TYPE(error_handler_name), Z_UNIVAL(error_handler_name));
		zval_dtor(&error_handler_name);
		return;
	}
	zval_dtor(&error_handler_name);

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

/* {{{ proto void restore_error_handler(void) U
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

/* {{{ proto string set_exception_handler(callable exception_handler) U
   Sets a user-defined exception handler function.  Returns the previously defined exception handler, or false on error */
ZEND_FUNCTION(set_exception_handler)
{
	zval *exception_handler;
	zval exception_handler_name;
	zend_bool had_orig_exception_handler=0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &exception_handler) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(exception_handler) != IS_NULL) { /* NULL == unset */
		if (!zend_is_callable(exception_handler, 0, &exception_handler_name TSRMLS_CC)) {
			zend_error(E_WARNING, "%v() expects the argument (%R) to be a valid callback",
					   get_active_function_name(TSRMLS_C), Z_TYPE(exception_handler_name), Z_UNIVAL(exception_handler_name));
			zval_dtor(&exception_handler_name);
			return;
		}
		zval_dtor(&exception_handler_name);
	}

	if (EG(user_exception_handler)) {
		had_orig_exception_handler = 1;
		*return_value = *EG(user_exception_handler);
		zval_copy_ctor(return_value);
		zend_ptr_stack_push(&EG(user_exception_handlers), EG(user_exception_handler));
	}
	ALLOC_ZVAL(EG(user_exception_handler));

	if (Z_TYPE_P(exception_handler) == IS_NULL) { /* unset user-defined handler */
		FREE_ZVAL(EG(user_exception_handler));
		EG(user_exception_handler) = NULL;
		zval_dtor(return_value);
		RETURN_TRUE;
	}

	*EG(user_exception_handler) = *exception_handler;
	zval_copy_ctor(EG(user_exception_handler));

	if (!had_orig_exception_handler) {
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ proto void restore_exception_handler(void) U
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

static int copy_class_or_interface_name(zend_class_entry **pce TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */
{
	zval *array = va_arg(args, zval *);
	zend_uint mask = va_arg(args, zend_uint);
	zend_uint comply = va_arg(args, zend_uint);
	zend_uint comply_mask = (comply)? mask:0;
	zend_class_entry *ce  = *pce;

	if ((hash_key->nKeyLength==0 ||
	     (hash_key->type == IS_UNICODE && hash_key->arKey.u[0] != 0) ||
	     (hash_key->type == IS_STRING && hash_key->arKey.s[0] != 0))
		&& (comply_mask == (ce->ce_flags & mask))) {
		add_next_index_textl(array, ce->name, ce->name_length, 1);
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

/* {{{ proto array get_declared_classes() U
   Returns an array of all declared classes. */
ZEND_FUNCTION(get_declared_classes)
{
	zend_uint mask = ZEND_ACC_INTERFACE;
	zend_uint comply = 0;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	array_init(return_value);
	zend_hash_apply_with_arguments(EG(class_table) TSRMLS_CC, (apply_func_args_t) copy_class_or_interface_name, 3, return_value, mask, comply);
}
/* }}} */

/* {{{ proto array get_declared_interfaces() U
   Returns an array of all declared interfaces. */
ZEND_FUNCTION(get_declared_interfaces)
{
	zend_uint mask = ZEND_ACC_INTERFACE;
	zend_uint comply = 1;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	array_init(return_value);
	zend_hash_apply_with_arguments(EG(class_table) TSRMLS_CC, (apply_func_args_t) copy_class_or_interface_name, 3, return_value, mask, comply);
}
/* }}} */

static int copy_function_name(zend_function *func TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */
{
	zval *internal_ar = va_arg(args, zval *),
	     *user_ar     = va_arg(args, zval *);

	if (hash_key->nKeyLength == 0 ||
	    (hash_key->type == IS_UNICODE && hash_key->arKey.u[0] == 0) ||
	    (hash_key->type == IS_STRING && hash_key->arKey.s[0] == 0)) {
		return 0;
	}

	if (func->type == ZEND_INTERNAL_FUNCTION) {
		if (hash_key->type == IS_STRING) {
			add_next_index_stringl(internal_ar, hash_key->arKey.s, hash_key->nKeyLength-1, 1);
		} else {
			add_next_index_unicodel(internal_ar, hash_key->arKey.u, hash_key->nKeyLength-1, 1);
		}
	} else if (func->type == ZEND_USER_FUNCTION) {
		if (hash_key->type == IS_STRING) {
			add_next_index_stringl(user_ar, hash_key->arKey.s, hash_key->nKeyLength-1, 1);
		} else {
			add_next_index_unicodel(user_ar, hash_key->arKey.u, hash_key->nKeyLength-1, 1);
		}
	}

	return 0;
}
/* }}} */

/* {{{ proto array get_defined_functions(void) U
   Returns an array of all defined functions */
ZEND_FUNCTION(get_defined_functions)
{
	zval *internal;
	zval *user;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	MAKE_STD_ZVAL(internal);
	MAKE_STD_ZVAL(user);

	array_init(internal);
	array_init(user);
	array_init(return_value);

	zend_hash_apply_with_arguments(EG(function_table) TSRMLS_CC, (apply_func_args_t) copy_function_name, 2, internal, user);

	if (zend_ascii_hash_add(Z_ARRVAL_P(return_value), "internal", sizeof("internal"), (void **)&internal, sizeof(zval *), NULL) == FAILURE) {
		zval_ptr_dtor(&internal);
		zval_ptr_dtor(&user);
		zval_dtor(return_value);
		zend_error(E_WARNING, "Cannot add internal functions to return value from get_defined_functions()");
		RETURN_FALSE;
	}

	if (zend_ascii_hash_add(Z_ARRVAL_P(return_value), "user", sizeof("user"), (void **)&user, sizeof(zval *), NULL) == FAILURE) {
		zval_ptr_dtor(&user);
		zval_dtor(return_value);
		zend_error(E_WARNING, "Cannot add user functions to return value from get_defined_functions()");
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto array get_defined_vars(void) U
   Returns an associative array of names and values of all currently defined variable names (variables in the current scope) */
ZEND_FUNCTION(get_defined_vars)
{
	if (!EG(active_symbol_table)) {
		zend_rebuild_symbol_table(TSRMLS_C);
	}

	array_init_size(return_value, zend_hash_num_elements(EG(active_symbol_table)));

	zend_hash_copy(Z_ARRVAL_P(return_value), EG(active_symbol_table),
					(copy_ctor_func_t)zval_add_ref, NULL, sizeof(zval *));
}
/* }}} */

#define LAMBDA_DECLARE_ENCODING	"declare(encoding=\"utf8\"); "
#define LAMBDA_TEMP_FUNCNAME	"__lambda_func"
/* {{{ proto string create_function(string args, string code) U
   Creates an anonymous function, and returns its name (funny, eh?) */
ZEND_FUNCTION(create_function)
{
	char *eval_code, *function_name;
	int eval_code_length, function_name_length;
	zstr args, code;
	int args_len, code_len;
	zend_uchar type;
	int retval;
	char *eval_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "TT", &args, &args_len, &type, &code, &code_len, &type) == FAILURE) {
		return;
	}

	if (type == IS_UNICODE) {
		char *str;
		int len;
		UErrorCode status = U_ZERO_ERROR;

		/* Convert args */
		zend_unicode_to_string_ex(UG(utf8_conv), &str, &len, args.u, args_len, &status);
		if (U_FAILURE(status)) {
			if (str) {
				efree(str);
			}
			zend_error(E_ERROR, "Unexpected error converting arguments to utf8 for compiling [%d]", (int)status);
			RETURN_FALSE;
		}
		args.s = str;
		args_len = len;

		/* Convert code */
		status = U_ZERO_ERROR;
		zend_unicode_to_string_ex(UG(utf8_conv), &str, &len, code.u, code_len, &status);
		if (U_FAILURE(status)) {
			if (str) {
				efree(str);
			}
			efree(args.s);
			zend_error(E_ERROR, "Unexpected error converting code to utf8 for compiling [%d]", (int)status);
			RETURN_FALSE;
		}
		code.s = str;
		code_len = len;
	}

	eval_code_length = sizeof(LAMBDA_DECLARE_ENCODING "function " LAMBDA_TEMP_FUNCNAME)
			+args_len
			+2	/* for the args parentheses */
			+2	/* for the curly braces */
			+code_len;

	eval_code = (char *) emalloc(eval_code_length);
	sprintf(eval_code, "%sfunction " LAMBDA_TEMP_FUNCNAME "(%s){%s}",
		(type == IS_UNICODE) ? LAMBDA_DECLARE_ENCODING : "",
		args.s, code.s);

	if (type == IS_UNICODE) {
		efree(args.s);
		efree(code.s);
	}

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
		function_add_ref(&new_function TSRMLS_CC);

		function_name = (char *) emalloc(sizeof("0lambda_")+MAX_LENGTH_OF_LONG);

		do {
			sprintf(function_name, "%clambda_%d", 0, ++EG(lambda_count));
			function_name_length = strlen(function_name+1)+1;
		} while (zend_hash_add(EG(function_table), function_name, function_name_length+1, &new_function, sizeof(zend_function), NULL)==FAILURE);
		zend_hash_del(EG(function_table), LAMBDA_TEMP_FUNCNAME, sizeof(LAMBDA_TEMP_FUNCNAME));
		RETVAL_ASCII_STRINGL(function_name, function_name_length, 0);
		if (UG(unicode)) {
			efree(function_name);
		}
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

#if ZEND_DEBUG
/* {{{ proto void zend_test_func(mixed arg1, mixed arg2) U
Generic test function */
ZEND_FUNCTION(zend_test_func)
{
	zval *arg1, *arg2;

	zend_get_parameters(ht, 2, &arg1, &arg2);
}
/* }}} */

#ifdef ZTS
/* {{{ proto int zend_thread_id(void) U
Returns a unique identifier for the current thread */
ZEND_FUNCTION(zend_thread_id)
{
	RETURN_LONG((long)tsrm_thread_id());
}
/* }}} */
#endif
#endif

/* {{{ proto string get_resource_type(resource res) U
   Get the resource type name for a given resource */
ZEND_FUNCTION(get_resource_type)
{
	char *resource_type;
	zval *z_resource_type;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_resource_type) == FAILURE) {
		return;
	}

	resource_type = zend_rsrc_list_get_rsrc_type(Z_LVAL_P(z_resource_type) TSRMLS_CC);
	if (resource_type) {
		RETURN_ASCII_STRING(resource_type, 1);
	} else {
		RETURN_ASCII_STRING("Unknown", 1);
	}
}
/* }}} */

static int add_extension_info(zend_module_entry *module, void *arg TSRMLS_DC) /* {{{ */
{
	zval *name_array = (zval *)arg;
	add_next_index_ascii_string(name_array, module->name, 1);
	return 0;
}
/* }}} */

static int add_zendext_info(zend_extension *ext, void *arg TSRMLS_DC) /* {{{ */
{
	zval *name_array = (zval *)arg;
	add_next_index_ascii_string(name_array, ext->name, 1);
	return 0;
}
/* }}} */

static int add_constant_info(zend_constant *constant, void *arg TSRMLS_DC) /* {{{ */
{
	zval *name_array = (zval *)arg;
	zval *const_val;

	MAKE_STD_ZVAL(const_val);
	*const_val = constant->value;
	zval_copy_ctor(const_val);
	INIT_PZVAL(const_val);
	add_u_assoc_zval_ex(name_array, UG(unicode)?IS_UNICODE:IS_STRING, constant->name, constant->name_len, const_val);
	return 0;
}
/* }}} */

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

/* {{{ proto array get_defined_constants([bool categorize]) U
   Return an array containing the names and values of all defined constants */
ZEND_FUNCTION(get_defined_constants)
{
	int categorize = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &categorize) == FAILURE) {
		return;
	}

	array_init(return_value);

	if (categorize) {
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
			module_names[i++] = (char*)module->name;
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

			add_u_assoc_zval_ex(modules[module_number], UG(unicode)?IS_UNICODE:IS_STRING, val->name, val->name_len, const_val);

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

static zval *debug_backtrace_get_args(void **curpos TSRMLS_DC) /* {{{ */
{
	void **p = curpos;
	zval *arg_array, **arg;
	int arg_count = (int)(zend_uintptr_t) *p;

	MAKE_STD_ZVAL(arg_array);
	array_init_size(arg_array, arg_count);
	p -= arg_count;

	while (--arg_count >= 0) {
		arg = (zval **) p++;
		if (*arg) {
			if (Z_TYPE_PP(arg) != IS_OBJECT) {
				SEPARATE_ZVAL_TO_MAKE_IS_REF(arg);
			}
			Z_ADDREF_PP(arg);
			add_next_index_zval(arg_array, *arg);
		} else {
			add_next_index_null(arg_array);
		}
	}

	return arg_array;
}
/* }}} */

void debug_print_backtrace_args(zval *arg_array TSRMLS_DC) /* {{{ */
{
	zval **tmp;
	HashPosition iterator;
	int i = 0;

	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(arg_array), &iterator);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(arg_array), (void **) &tmp, &iterator) == SUCCESS) {
		if (i++) {
			ZEND_PUTS(", ");
		}
		zend_print_flat_zval_r(*tmp TSRMLS_CC);
		zend_hash_move_forward_ex(Z_ARRVAL_P(arg_array), &iterator);
	}
}
/* }}} */

/* {{{ proto void debug_print_backtrace(void) U */
ZEND_FUNCTION(debug_print_backtrace)
{
	zend_execute_data *ptr, *skip;
	int lineno;
	zstr function_name;
	char *filename;
	zstr class_name;
	char *call_type;
	char *include_filename = NULL;
	zval *arg_array = NULL;
	int indent = 0;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	ptr = EG(current_execute_data);

	/* skip debug_backtrace() */
	ptr = ptr->prev_execute_data;

	while (ptr) {
		zstr free_class_name = NULL_ZSTR;
		int	function_name_string = 1;

		class_name = NULL_ZSTR;
		call_type = NULL;
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

		if (function_name.v) {
			function_name_string = !UG(unicode);
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
				class_name = NULL_ZSTR;
				call_type = NULL;
			}
			if ((! ptr->opline) || ((ptr->opline->opcode == ZEND_DO_FCALL_BY_NAME) || (ptr->opline->opcode == ZEND_DO_FCALL))) {
				if (ptr->function_state.arguments) {
					arg_array = debug_backtrace_get_args(ptr->function_state.arguments TSRMLS_CC);
				}
			}
		} else {
			/* i know this is kinda ugly, but i'm trying to avoid extra cycles in the main execution loop */
			zend_bool build_filename_arg = 1;

			if (!ptr->opline || ptr->opline->opcode != ZEND_INCLUDE_OR_EVAL) {
				/* can happen when calling eval from a custom sapi */
				function_name.s = "unknown";
				build_filename_arg = 0;
			} else
			switch (Z_LVAL(ptr->opline->op2.u.constant)) {
				case ZEND_EVAL:
					function_name.s = "eval";
					build_filename_arg = 0;
					break;
				case ZEND_INCLUDE:
					function_name.s = "include";
					break;
				case ZEND_REQUIRE:
					function_name.s = "require";
					break;
				case ZEND_INCLUDE_ONCE:
					function_name.s = "include_once";
					break;
				case ZEND_REQUIRE_ONCE:
					function_name.s = "require_once";
					break;
				default:
					/* this can actually happen if you use debug_backtrace() in your error_handler and
					 * you're in the top-scope */
					function_name.s = "unknown";
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
		if (class_name.v) {
			if (UG(unicode)) {
				zend_printf("%r", class_name.u);
			} else {
				ZEND_PUTS(class_name.s);
			}
			ZEND_PUTS(call_type);
		}
		if (function_name_string) {
			zend_printf("%s(", function_name.s?function_name.s:"main");
		} else {
			zend_printf("%r(", function_name.u);
		}
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
		if (free_class_name.v) {
			efree(free_class_name.v);
		}
	}
}

/* }}} */

ZEND_API void zend_fetch_debug_backtrace(zval *return_value, int skip_last, int provide_object TSRMLS_DC) /* {{{ */
{
	zend_execute_data *ptr, *skip;
	int lineno;
	zstr function_name;
	char *filename;
	zstr class_name;
	char *include_filename = NULL;
	zval *stack_frame;

	ptr = EG(current_execute_data);

	/* skip "new Exception()" */
	if (ptr && (skip_last == 0) && ptr->opline && (ptr->opline->opcode == ZEND_NEW)) {
		ptr = ptr->prev_execute_data;
	}

	/* skip debug_backtrace() */
	if (skip_last-- && ptr) {
		ptr = ptr->prev_execute_data;
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
			add_ascii_assoc_rt_string_ex(stack_frame, "file", sizeof("file"), filename, 1);
			add_ascii_assoc_long_ex(stack_frame, "line", sizeof("line"), lineno);

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
					add_ascii_assoc_string_ex(stack_frame, "file", sizeof("file"), prev->op_array->filename, 1);
					add_ascii_assoc_long_ex(stack_frame, "line", sizeof("line"), prev->opline->lineno);
					break;
				}
				prev = prev->prev_execute_data;
			}
			filename = NULL;
		}

		function_name = ptr->function_state.function->common.function_name;

		if (function_name.v) {
			add_ascii_assoc_text_ex(stack_frame, "function", sizeof("function"), function_name, 1);

			if (ptr->object && Z_TYPE_P(ptr->object) == IS_OBJECT) {
				if (ptr->function_state.function->common.scope) {
					add_ascii_assoc_textl_ex(stack_frame, "class", sizeof("class"), ptr->function_state.function->common.scope->name, ptr->function_state.function->common.scope->name_length, 1);
				} else {
					zend_uint class_name_len;
					int dup;

					dup = zend_get_object_classname(ptr->object, &class_name, &class_name_len TSRMLS_CC);
					add_ascii_assoc_text_ex(stack_frame, "class", sizeof("class"), class_name, dup);
				}
				if (provide_object) {
					add_ascii_assoc_zval_ex(stack_frame, "object", sizeof("object"), ptr->object);
					Z_ADDREF_P(ptr->object);
				}

				add_ascii_assoc_ascii_string_ex(stack_frame, "type", sizeof("type"), "->", 1);
			} else if (ptr->function_state.function->common.scope) {
				add_ascii_assoc_textl_ex(stack_frame, "class", sizeof("class"), ptr->function_state.function->common.scope->name, ptr->function_state.function->common.scope->name_length, 1);
				add_ascii_assoc_ascii_string_ex(stack_frame, "type", sizeof("type"), "::", 1);
			}

			if ((! ptr->opline) || ((ptr->opline->opcode == ZEND_DO_FCALL_BY_NAME) || (ptr->opline->opcode == ZEND_DO_FCALL))) {
				if (ptr->function_state.arguments) {
					add_ascii_assoc_zval_ex(stack_frame, "args", sizeof("args"), debug_backtrace_get_args(ptr->function_state.arguments TSRMLS_CC));
				}
			}
		} else {
			/* i know this is kinda ugly, but i'm trying to avoid extra cycles in the main execution loop */
			zend_bool build_filename_arg = 1;

			if (!ptr->opline || ptr->opline->opcode != ZEND_INCLUDE_OR_EVAL) {
				/* can happen when calling eval from a custom sapi */
				function_name.s = "unknown";
				build_filename_arg = 0;
			} else
			switch (Z_LVAL(ptr->opline->op2.u.constant)) {
				case ZEND_EVAL:
					function_name.s = "eval";
					build_filename_arg = 0;
					break;
				case ZEND_INCLUDE:
					function_name.s = "include";
					break;
				case ZEND_REQUIRE:
					function_name.s = "require";
					break;
				case ZEND_INCLUDE_ONCE:
					function_name.s = "include_once";
					break;
				case ZEND_REQUIRE_ONCE:
					function_name.s = "require_once";
					break;
				default:
					/* this can actually happen if you use debug_backtrace() in your error_handler and
					 * you're in the top-scope */
					function_name.s = "unknown";
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

				add_next_index_rt_string(arg_array, include_filename, 1);
				add_ascii_assoc_zval_ex(stack_frame, "args", sizeof("args"), arg_array);
			}

			add_ascii_assoc_ascii_string_ex(stack_frame, "function", sizeof("function"), function_name.s, 1);
		}

		add_next_index_zval(return_value, stack_frame);

		include_filename = filename;

		ptr = skip->prev_execute_data;
	}
}
/* }}} */

/* {{{ proto array debug_backtrace([bool provide_object]) U
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

/* {{{ proto bool extension_loaded(string extension_name) U
   Returns true if the named extension is loaded */
ZEND_FUNCTION(extension_loaded)
{
	zstr ext;
	int ext_len;
	zend_uchar ext_type;
	char *name, *lcname;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "t", &ext, &ext_len, &ext_type) == FAILURE) {
		return;
	}

	if (ext_type == IS_UNICODE) {
		name = zend_unicode_to_ascii(ext.u, ext_len TSRMLS_CC);
		if (name == NULL) {
			zend_error(E_WARNING, "Extension name has to consist only of ASCII characters");
			RETURN_FALSE;
		}
	} else {
		name = ext.s;
	}

	lcname = zend_str_tolower_dup(name, ext_len);
	if (ext_type == IS_UNICODE) {
		efree(name);
	}
	if (zend_hash_exists(&module_registry, lcname, ext_len+1)) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}
	efree(lcname);
}
/* }}} */

/* {{{ proto array get_extension_funcs(string extension_name) U
   Returns an array with the names of functions belonging to the named extension */
ZEND_FUNCTION(get_extension_funcs)
{
	zstr ext;
	int ext_len;
	zend_uchar ext_type;
	char *name;
	zend_module_entry *module;
	const zend_function_entry *func;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "t", &ext, &ext_len, &ext_type) == FAILURE) {
		return;
	}

	if (ext_type == IS_UNICODE) {
		name = zend_unicode_to_ascii(ext.u, ext_len TSRMLS_CC);
		if (name == NULL) {
			zend_error(E_WARNING, "Extension name has to consist only of ASCII characters");
			RETURN_FALSE;
		}
	} else {
		name = ext.s;
	}

	if (strncasecmp(name, "zend", sizeof("zend"))) {
		char *lcname = zend_str_tolower_dup(name, ext_len);
		if (ext_type == IS_UNICODE) {
			efree(name);
		}
		if (zend_hash_find(&module_registry, lcname, ext_len+1, (void**)&module) == FAILURE) {
			efree(lcname);
			RETURN_FALSE;
		}
		efree(lcname);

		if (!(func = module->functions)) {
			RETURN_FALSE;
		}
	} else {
		if (ext_type == IS_UNICODE) {
			efree(name);
		}
		func = builtin_functions;
	}

	array_init(return_value);

	while (func->fname) {
		add_next_index_ascii_string(return_value, func->fname, 1);
		func++;
	}
}
/* }}} */

/* {{{ proto int gc_collect_cycles(void) U
   Forces collection of any existing garbage cycles.
   Returns number of freed zvals */
ZEND_FUNCTION(gc_collect_cycles)
{
	RETURN_LONG(gc_collect_cycles(TSRMLS_C));
}
/* }}} */

/* {{{ proto void gc_enabled(void) U
   Returns status of the circular reference collector */
ZEND_FUNCTION(gc_enabled)
{
	RETURN_BOOL(GC_G(gc_enabled));
}
/* }}} */

/* {{{ proto void gc_enable(void) U
   Activates the circular reference collector */
ZEND_FUNCTION(gc_enable)
{
	zend_alter_ini_entry("zend.enable_gc", sizeof("zend.enable_gc"), "1", sizeof("1")-1, ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME);
}
/* }}} */

/* {{{ proto void gc_disable(void) U
   Deactivates the circular reference collector */
ZEND_FUNCTION(gc_disable)
{
	zend_alter_ini_entry("zend.enable_gc", sizeof("zend.enable_gc"), "0", sizeof("0")-1, ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
