/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 Zend Technologies Ltd. (http://www.zend.com) |
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
#include "zend_closures.h"

#undef ZEND_TEST_EXCEPTIONS

static ZEND_FUNCTION(zend_version);
static ZEND_FUNCTION(func_num_args);
static ZEND_FUNCTION(func_get_arg);
static ZEND_FUNCTION(func_get_args);
static ZEND_FUNCTION(strlen);
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
static ZEND_FUNCTION(trait_exists);
static ZEND_FUNCTION(function_exists);
static ZEND_FUNCTION(class_alias);
#if ZEND_DEBUG
static ZEND_FUNCTION(leak);
static ZEND_FUNCTION(leak_variable);
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
static ZEND_FUNCTION(get_declared_traits);
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

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO(arginfo_zend__void, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_func_get_arg, 0, 0, 1)
	ZEND_ARG_INFO(0, arg_num)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_strlen, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_strcmp, 0, 0, 2)
	ZEND_ARG_INFO(0, str1)
	ZEND_ARG_INFO(0, str2)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_strncmp, 0, 0, 3)
	ZEND_ARG_INFO(0, str1)
	ZEND_ARG_INFO(0, str2)
	ZEND_ARG_INFO(0, len)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_each, 0, 0, 1)
	ZEND_ARG_INFO(1, arr)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_error_reporting, 0, 0, 0)
	ZEND_ARG_INFO(0, new_error_level)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_define, 0, 0, 3)
	ZEND_ARG_INFO(0, constant_name)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, case_insensitive)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_defined, 0, 0, 1)
	ZEND_ARG_INFO(0, constant_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_get_class, 0, 0, 0)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_is_subclass_of, 0, 0, 2)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, class_name)
	ZEND_ARG_INFO(0, allow_string)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_get_class_vars, 0, 0, 1)
	ZEND_ARG_INFO(0, class_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_get_object_vars, 0, 0, 1)
	ZEND_ARG_INFO(0, obj)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_get_class_methods, 0, 0, 1)
	ZEND_ARG_INFO(0, class)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_method_exists, 0, 0, 2)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, method)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_property_exists, 0, 0, 2)
	ZEND_ARG_INFO(0, object_or_class)
	ZEND_ARG_INFO(0, property_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_exists, 0, 0, 1)
	ZEND_ARG_INFO(0, classname)
	ZEND_ARG_INFO(0, autoload)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_trait_exists, 0, 0, 1)
	ZEND_ARG_INFO(0, traitname)
	ZEND_ARG_INFO(0, autoload)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_function_exists, 0, 0, 1)
	ZEND_ARG_INFO(0, function_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_alias, 0, 0, 2)
	ZEND_ARG_INFO(0, user_class_name)
	ZEND_ARG_INFO(0, alias_name)
	ZEND_ARG_INFO(0, autoload)
ZEND_END_ARG_INFO()

#if ZEND_DEBUG
ZEND_BEGIN_ARG_INFO_EX(arginfo_leak_variable, 0, 0, 1)
	ZEND_ARG_INFO(0, variable)
	ZEND_ARG_INFO(0, leak_data)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_trigger_error, 0, 0, 1)
	ZEND_ARG_INFO(0, message)
	ZEND_ARG_INFO(0, error_type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_set_error_handler, 0, 0, 1)
	ZEND_ARG_INFO(0, error_handler)
	ZEND_ARG_INFO(0, error_types)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_set_exception_handler, 0, 0, 1)
	ZEND_ARG_INFO(0, exception_handler)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_create_function, 0, 0, 2)
	ZEND_ARG_INFO(0, args)
	ZEND_ARG_INFO(0, code)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_get_resource_type, 0, 0, 1)
	ZEND_ARG_INFO(0, res)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_get_loaded_extensions, 0, 0, 0)
	ZEND_ARG_INFO(0, zend_extensions)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_get_defined_constants, 0, 0, 0)
	ZEND_ARG_INFO(0, categorize)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_debug_backtrace, 0, 0, 0)
	ZEND_ARG_INFO(0, options)
	ZEND_ARG_INFO(0, limit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_debug_print_backtrace, 0, 0, 0)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_extension_loaded, 0, 0, 1)
	ZEND_ARG_INFO(0, extension_name)
ZEND_END_ARG_INFO()
/* }}} */

static const zend_function_entry builtin_functions[] = { /* {{{ */
	ZEND_FE(zend_version,		arginfo_zend__void)
	ZEND_FE(func_num_args,		arginfo_zend__void)
	ZEND_FE(func_get_arg,		arginfo_func_get_arg)
	ZEND_FE(func_get_args,		arginfo_zend__void)
	ZEND_FE(strlen,			arginfo_strlen)
	ZEND_FE(strcmp,			arginfo_strcmp)
	ZEND_FE(strncmp,		arginfo_strncmp)
	ZEND_FE(strcasecmp,		arginfo_strcmp)
	ZEND_FE(strncasecmp,		arginfo_strncmp)
	ZEND_FE(each,			arginfo_each)
	ZEND_FE(error_reporting,	arginfo_error_reporting)
	ZEND_FE(define,			arginfo_define)
	ZEND_FE(defined,		arginfo_defined)
	ZEND_FE(get_class,		arginfo_get_class)
	ZEND_FE(get_called_class,	arginfo_zend__void)
	ZEND_FE(get_parent_class,	arginfo_get_class)
	ZEND_FE(method_exists,		arginfo_method_exists)
	ZEND_FE(property_exists,	arginfo_property_exists)
	ZEND_FE(class_exists,		arginfo_class_exists)
	ZEND_FE(interface_exists,	arginfo_class_exists)
	ZEND_FE(trait_exists,		arginfo_trait_exists)
	ZEND_FE(function_exists,	arginfo_function_exists)
	ZEND_FE(class_alias,		arginfo_class_alias)
#if ZEND_DEBUG
	ZEND_FE(leak,				NULL)
	ZEND_FE(leak_variable,		arginfo_leak_variable)
#ifdef ZEND_TEST_EXCEPTIONS
	ZEND_FE(crash,				NULL)
#endif
#endif
	ZEND_FE(get_included_files,	arginfo_zend__void)
	ZEND_FALIAS(get_required_files,	get_included_files,		arginfo_zend__void)
	ZEND_FE(is_subclass_of,		arginfo_is_subclass_of)
	ZEND_FE(is_a,			arginfo_is_subclass_of)
	ZEND_FE(get_class_vars,		arginfo_get_class_vars)
	ZEND_FE(get_object_vars,	arginfo_get_object_vars)
	ZEND_FE(get_class_methods,	arginfo_get_class_methods)
	ZEND_FE(trigger_error,		arginfo_trigger_error)
	ZEND_FALIAS(user_error,		trigger_error,		arginfo_trigger_error)
	ZEND_FE(set_error_handler,		arginfo_set_error_handler)
	ZEND_FE(restore_error_handler,		arginfo_zend__void)
	ZEND_FE(set_exception_handler,		arginfo_set_exception_handler)
	ZEND_FE(restore_exception_handler,	arginfo_zend__void)
	ZEND_FE(get_declared_classes, 		arginfo_zend__void)
	ZEND_FE(get_declared_traits, 		arginfo_zend__void)
	ZEND_FE(get_declared_interfaces, 	arginfo_zend__void)
	ZEND_FE(get_defined_functions, 		arginfo_zend__void)
	ZEND_FE(get_defined_vars,		arginfo_zend__void)
	ZEND_FE(create_function,		arginfo_create_function)
	ZEND_FE(get_resource_type,		arginfo_get_resource_type)
	ZEND_FE(get_loaded_extensions,		arginfo_get_loaded_extensions)
	ZEND_FE(extension_loaded,		arginfo_extension_loaded)
	ZEND_FE(get_extension_funcs,		arginfo_extension_loaded)
	ZEND_FE(get_defined_constants,		arginfo_get_defined_constants)
	ZEND_FE(debug_backtrace, 		arginfo_debug_backtrace)
	ZEND_FE(debug_print_backtrace, 		arginfo_debug_print_backtrace)
#if ZEND_DEBUG
	ZEND_FE(zend_test_func,		NULL)
#ifdef ZTS
	ZEND_FE(zend_thread_id,		NULL)
#endif
#endif
	ZEND_FE(gc_collect_cycles, 	arginfo_zend__void)
	ZEND_FE(gc_enabled, 		arginfo_zend__void)
	ZEND_FE(gc_enable, 		arginfo_zend__void)
	ZEND_FE(gc_disable, 		arginfo_zend__void)
	ZEND_FE_END
};
/* }}} */

ZEND_MINIT_FUNCTION(core) { /* {{{ */
	zend_class_entry class_entry;

	INIT_CLASS_ENTRY(class_entry, "stdClass", NULL);
	zend_standard_class_def = zend_register_internal_class(&class_entry TSRMLS_CC);

	zend_register_default_classes(TSRMLS_C);

	return SUCCESS;
}
/* }}} */

zend_module_entry zend_builtin_module = { /* {{{ */
    STANDARD_MODULE_HEADER,
	"Core",
	builtin_functions,
	ZEND_MINIT(core),
	NULL,
	NULL,
	NULL,
	NULL,
	ZEND_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

int zend_startup_builtin_functions(TSRMLS_D) /* {{{ */
{
	zend_builtin_module.module_number = 0;
	zend_builtin_module.type = MODULE_PERSISTENT;
	return (EG(current_module) = zend_register_module_ex(&zend_builtin_module TSRMLS_CC)) == NULL ? FAILURE : SUCCESS;
}
/* }}} */

/* {{{ proto string zend_version(void)
   Get the version of the Zend Engine */
ZEND_FUNCTION(zend_version)
{
	RETURN_STRINGL(ZEND_VERSION, sizeof(ZEND_VERSION)-1, 1);
}
/* }}} */

/* {{{ proto int gc_collect_cycles(void)
   Forces collection of any existing garbage cycles.
   Returns number of freed zvals */
ZEND_FUNCTION(gc_collect_cycles)
{
	RETURN_LONG(gc_collect_cycles(TSRMLS_C));
}
/* }}} */

/* {{{ proto void gc_enabled(void)
   Returns status of the circular reference collector */
ZEND_FUNCTION(gc_enabled)
{
	RETURN_BOOL(GC_G(gc_enabled));
}
/* }}} */

/* {{{ proto void gc_enable(void)
   Activates the circular reference collector */
ZEND_FUNCTION(gc_enable)
{
	zend_alter_ini_entry("zend.enable_gc", sizeof("zend.enable_gc"), "1", sizeof("1")-1, ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME);
}
/* }}} */

/* {{{ proto void gc_disable(void)
   Deactivates the circular reference collector */
ZEND_FUNCTION(gc_disable)
{
	zend_alter_ini_entry("zend.enable_gc", sizeof("zend.enable_gc"), "0", sizeof("0")-1, ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME);
}
/* }}} */

/* {{{ proto int func_num_args(void)
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


/* {{{ proto mixed func_get_arg(int arg_num)
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
	RETURN_ZVAL_FAST(arg);
}
/* }}} */


/* {{{ proto array func_get_args()
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
		zval *element, *arg;

		arg = *((zval **) (p-(arg_count-i)));
		if (!Z_ISREF_P(arg)) {
			element = arg;
			Z_ADDREF_P(element);
		} else {
			ALLOC_ZVAL(element);
			INIT_PZVAL_COPY(element, arg);
			zval_copy_ctor(element);
	    }
		zend_hash_next_index_insert(return_value->value.ht, &element, sizeof(zval *), NULL);
	}
}
/* }}} */


/* {{{ proto int strlen(string str)
   Get string length */
ZEND_FUNCTION(strlen)
{
	char *s1;
	int s1_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &s1, &s1_len) == FAILURE) {
		return;
	}

	RETVAL_LONG(s1_len);
}
/* }}} */


/* {{{ proto int strcmp(string str1, string str2)
   Binary safe string comparison */
ZEND_FUNCTION(strcmp)
{
	char *s1, *s2;
	int s1_len, s2_len;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &s1, &s1_len, &s2, &s2_len) == FAILURE) {
		return;
	}

	RETURN_LONG(zend_binary_strcmp(s1, s1_len, s2, s2_len));
}
/* }}} */


/* {{{ proto int strncmp(string str1, string str2, int len)
   Binary safe string comparison */
ZEND_FUNCTION(strncmp)
{
	char *s1, *s2;
	int s1_len, s2_len;
	long len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssl", &s1, &s1_len, &s2, &s2_len, &len) == FAILURE) {
		return;
	}

	if (len < 0) {
		zend_error(E_WARNING, "Length must be greater than or equal to 0");
		RETURN_FALSE;
	}

	RETURN_LONG(zend_binary_strncmp(s1, s1_len, s2, s2_len, len));
}
/* }}} */


/* {{{ proto int strcasecmp(string str1, string str2)
   Binary safe case-insensitive string comparison */
ZEND_FUNCTION(strcasecmp)
{
	char *s1, *s2;
	int s1_len, s2_len;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &s1, &s1_len, &s2, &s2_len) == FAILURE) {
		return;
	}

	RETURN_LONG(zend_binary_strcasecmp(s1, s1_len, s2, s2_len));
}
/* }}} */


/* {{{ proto int strncasecmp(string str1, string str2, int len)
   Binary safe string comparison */
ZEND_FUNCTION(strncasecmp)
{
	char *s1, *s2;
	int s1_len, s2_len;
	long len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssl", &s1, &s1_len, &s2, &s2_len, &len) == FAILURE) {
		return;
	}

	if (len < 0) {
		zend_error(E_WARNING, "Length must be greater than or equal to 0");
		RETURN_FALSE;
	}

	RETURN_LONG(zend_binary_strncasecmp(s1, s1_len, s2, s2_len, len));
}
/* }}} */


/* {{{ proto array each(array arr)
   Return the currently pointed key..value pair in the passed array, and advance the pointer to the next element */
ZEND_FUNCTION(each)
{
	zval *array, *entry, **entry_ptr, *tmp;
	char *string_key;
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
	zend_hash_index_update(return_value->value.ht, 1, &entry, sizeof(zval *), NULL);
	Z_ADDREF_P(entry);
	zend_hash_update(return_value->value.ht, "value", sizeof("value"), &entry, sizeof(zval *), NULL);
	Z_ADDREF_P(entry);

	/* add the key elements */
	switch (zend_hash_get_current_key_ex(target_hash, &string_key, &string_key_len, &num_key, 0, NULL)) {
		case HASH_KEY_IS_STRING:
			add_get_index_stringl(return_value, 0, string_key, string_key_len-1, (void **) &inserted_pointer, !IS_INTERNED(string_key));
			break;
		case HASH_KEY_IS_LONG:
			add_get_index_long(return_value, 0, num_key, (void **) &inserted_pointer);
			break;
	}
	zend_hash_update(return_value->value.ht, "key", sizeof("key"), inserted_pointer, sizeof(zval *), NULL);
	Z_ADDREF_PP(inserted_pointer);
	zend_hash_move_forward(target_hash);
}
/* }}} */


/* {{{ proto int error_reporting([int new_error_level])
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


/* {{{ proto bool define(string constant_name, mixed value, boolean case_insensitive=false)
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

	/* class constant, check if there is name and make sure class is valid & exists */
	if (zend_memnstr(name, "::", sizeof("::") - 1, name + name_len)) {
		zend_error(E_WARNING, "Class constants cannot be defined or redefined");
		RETURN_FALSE;
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
	c.name = str_strndup(name, name_len);
	if(c.name == NULL) {
		RETURN_FALSE;
	}
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
	char *name;
	int name_len;
	zval c;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
		return;
	}
	
	if (zend_get_constant_ex(name, name_len, &c, NULL, ZEND_FETCH_CLASS_SILENT TSRMLS_CC)) {
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
	zval *obj = NULL;
	const char *name = "";
	zend_uint name_len = 0;
	int dup;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|o!", &obj) == FAILURE) {
		RETURN_FALSE;
	}

	if (!obj) {
		if (EG(scope)) {
			RETURN_STRINGL(EG(scope)->name, EG(scope)->name_length, 1);
		} else {
			zend_error(E_WARNING, "get_class() called without object from outside a class");
			RETURN_FALSE;
		}
	}

	dup = zend_get_object_classname(obj, &name, &name_len TSRMLS_CC);

	RETURN_STRINGL(name, name_len, dup);
}
/* }}} */


/* {{{ proto string get_called_class()
   Retrieves the "Late Static Binding" class name */
ZEND_FUNCTION(get_called_class)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (EG(called_scope)) {
		RETURN_STRINGL(EG(called_scope)->name, EG(called_scope)->name_length, 1);
	} else if (!EG(scope))  {
		zend_error(E_WARNING, "get_called_class() called from outside a class");
	}
	RETURN_FALSE;
}
/* }}} */


/* {{{ proto string get_parent_class([mixed object])
   Retrieves the parent class name for object or class or current scope. */
ZEND_FUNCTION(get_parent_class)
{
	zval *arg;
	zend_class_entry *ce = NULL;
	const char *name;
	zend_uint name_length;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z", &arg) == FAILURE) {
		return;
	}

	if (!ZEND_NUM_ARGS()) {
		ce = EG(scope);
		if (ce && ce->parent) {
			RETURN_STRINGL(ce->parent->name, ce->parent->name_length, 1);
		} else {
			RETURN_FALSE;
		}
	}

	if (Z_TYPE_P(arg) == IS_OBJECT) {
		if (Z_OBJ_HT_P(arg)->get_class_name
			&& Z_OBJ_HT_P(arg)->get_class_name(arg, &name, &name_length, 1 TSRMLS_CC) == SUCCESS) {
			RETURN_STRINGL(name, name_length, 0);
		} else {
			ce = zend_get_class_entry(arg TSRMLS_CC);
		}
	} else if (Z_TYPE_P(arg) == IS_STRING) {
		zend_class_entry **pce;
		
		if (zend_lookup_class(Z_STRVAL_P(arg), Z_STRLEN_P(arg), &pce TSRMLS_CC) == SUCCESS) {
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
	zval *obj;
	char *class_name;
	int class_name_len;
	zend_class_entry *instance_ce;
	zend_class_entry **ce;
	zend_bool allow_string = only_subclass;
	zend_bool retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zs|b", &obj, &class_name, &class_name_len, &allow_string) == FAILURE) {
		return;
	}
	/*
	 * allow_string - is_a default is no, is_subclass_of is yes. 
	 *   if it's allowed, then the autoloader will be called if the class does not exist.
	 *   default behaviour is different, as 'is_a' used to be used to test mixed return values
	 *   and there is no easy way to deprecate this.
	 */

	if (allow_string && Z_TYPE_P(obj) == IS_STRING) {
		zend_class_entry **the_ce;
		if (zend_lookup_class(Z_STRVAL_P(obj), Z_STRLEN_P(obj), &the_ce TSRMLS_CC) == FAILURE) {
			RETURN_FALSE;
		}
		instance_ce = *the_ce;
	} else if (Z_TYPE_P(obj) == IS_OBJECT && HAS_CLASS_ENTRY(*obj)) {
		instance_ce = Z_OBJCE_P(obj);
	} else {
		RETURN_FALSE;
	}

	if (zend_lookup_class_ex(class_name, class_name_len, NULL, 0, &ce TSRMLS_CC) == FAILURE) {
		retval = 0;
	} else {
		if (only_subclass && instance_ce == *ce) {
			retval = 0;
 		} else {
			retval = instanceof_function(instance_ce, *ce TSRMLS_CC);
		}
	}

	RETURN_BOOL(retval);
}


/* {{{ proto bool is_subclass_of(mixed object_or_string, string class_name [, bool allow_string=true])
   Returns true if the object has this class as one of its parents */
ZEND_FUNCTION(is_subclass_of)
{
	is_a_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */


/* {{{ proto bool is_a(mixed object_or_string, string class_name [, bool allow_string=false])
   Returns true if the first argument is an object and is this class or has this class as one of its parents, */
ZEND_FUNCTION(is_a)
{
	is_a_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */


/* {{{ add_class_vars */
static void add_class_vars(zend_class_entry *ce, int statics, zval *return_value TSRMLS_DC)
{
	HashPosition pos;
	zend_property_info *prop_info;
	zval *prop, *prop_copy;
	char *key;
	uint key_len;
	ulong num_index;

	zend_hash_internal_pointer_reset_ex(&ce->properties_info, &pos);
	while (zend_hash_get_current_data_ex(&ce->properties_info, (void **) &prop_info, &pos) == SUCCESS) {
		zend_hash_get_current_key_ex(&ce->properties_info, &key, &key_len, &num_index, 0, &pos);
		zend_hash_move_forward_ex(&ce->properties_info, &pos);
		if (((prop_info->flags & ZEND_ACC_SHADOW) &&
		     prop_info->ce != EG(scope)) ||
		    ((prop_info->flags & ZEND_ACC_PROTECTED) &&
		     !zend_check_protected(prop_info->ce, EG(scope))) ||
		    ((prop_info->flags & ZEND_ACC_PRIVATE) &&
		      ce != EG(scope) &&
			  prop_info->ce != EG(scope))) {
			continue;
		}
		prop = NULL;
		if (prop_info->offset >= 0) {
			if (statics && (prop_info->flags & ZEND_ACC_STATIC) != 0) {
				prop = ce->default_static_members_table[prop_info->offset];
			} else if (!statics && (prop_info->flags & ZEND_ACC_STATIC) == 0) {
				prop = ce->default_properties_table[prop_info->offset];
 			}
		}
		if (!prop) {
			continue;
		}

		/* copy: enforce read only access */
		ALLOC_ZVAL(prop_copy);
		*prop_copy = *prop;
		zval_copy_ctor(prop_copy);
		INIT_PZVAL(prop_copy);

		/* this is necessary to make it able to work with default array
		 * properties, returned to user */
		if (IS_CONSTANT_TYPE(Z_TYPE_P(prop_copy))) {
			zval_update_constant(&prop_copy, 0 TSRMLS_CC);
		}

		zend_hash_update(Z_ARRVAL_P(return_value), key, key_len, &prop_copy, sizeof(zval*), NULL);
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
		add_class_vars(*pce, 0, return_value TSRMLS_CC);
		add_class_vars(*pce, 1, return_value TSRMLS_CC);
	}
}
/* }}} */


/* {{{ proto array get_object_vars(object obj)
   Returns an array of object properties */
ZEND_FUNCTION(get_object_vars)
{
	zval *obj;
	zval **value;
	HashTable *properties;
	HashPosition pos;
	char *key;
	const char *prop_name, *class_name;
	uint key_len, prop_len;
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
		if (zend_hash_get_current_key_ex(properties, &key, &key_len, &num_index, 0, &pos) == HASH_KEY_IS_STRING) {
			if (zend_check_property_access(zobj, key, key_len-1 TSRMLS_CC) == SUCCESS) {
				zend_unmangle_property_name_ex(key, key_len - 1, &class_name, &prop_name, (int*) &prop_len);
				/* Not separating references */
				Z_ADDREF_PP(value);
				if (IS_INTERNED(key) && prop_name != key) {
					/* we can't use substring of interned string as a new key */
					char *tmp = estrndup(prop_name, prop_len);
					add_assoc_zval_ex(return_value, tmp, prop_len + 1, *value);
					efree(tmp);
				} else {
					add_assoc_zval_ex(return_value, prop_name, prop_len + 1, *value);
				}
			}
		}
		zend_hash_move_forward_ex(properties, &pos);
	}
}
/* }}} */

static int same_name(const char *key, const char *name, zend_uint name_len)
{
	char *lcname = zend_str_tolower_dup(name, name_len);
	int ret = memcmp(lcname, key, name_len) == 0;
	efree(lcname);
	return ret;
}

/* {{{ proto array get_class_methods(mixed class)
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
	} else if (Z_TYPE_P(klass) == IS_STRING) {
		if (zend_lookup_class(Z_STRVAL_P(klass), Z_STRLEN_P(klass), &pce TSRMLS_CC) == SUCCESS) {
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
			if (zend_hash_get_current_key_ex(&ce->function_table, &key, &key_len, &num_index, 0, &pos) != HASH_KEY_IS_STRING) {
				MAKE_STD_ZVAL(method_name);
				ZVAL_STRINGL(method_name, mptr->common.function_name, len, 1);
				zend_hash_next_index_insert(return_value->value.ht, &method_name, sizeof(zval *), NULL);
			} else if ((mptr->common.fn_flags & ZEND_ACC_CTOR) == 0 ||
			    mptr->common.scope == ce ||
			    zend_binary_strcasecmp(key, key_len-1, mptr->common.function_name, len) == 0) {

				if (mptr->type == ZEND_USER_FUNCTION &&
				    *mptr->op_array.refcount > 1 &&
			    	(len != key_len - 1 ||
			    	 !same_name(key, mptr->common.function_name, len))) {
					MAKE_STD_ZVAL(method_name);
					ZVAL_STRINGL(method_name, zend_find_alias_name(mptr->common.scope, key, key_len - 1), key_len - 1, 1);
					zend_hash_next_index_insert(return_value->value.ht, &method_name, sizeof(zval *), NULL);
				} else {
					MAKE_STD_ZVAL(method_name);
					ZVAL_STRINGL(method_name, mptr->common.function_name, len, 1);
					zend_hash_next_index_insert(return_value->value.ht, &method_name, sizeof(zval *), NULL);
				}
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
	zval *klass; 
	char *method_name;
	int method_len;
	char *lcname;
	zend_class_entry * ce, **pce;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zs", &klass, &method_name, &method_len) == FAILURE) {
		return;
	}
	if (Z_TYPE_P(klass) == IS_OBJECT) {
		ce = Z_OBJCE_P(klass);
	} else if (Z_TYPE_P(klass) == IS_STRING) {
		if (zend_lookup_class(Z_STRVAL_P(klass), Z_STRLEN_P(klass), &pce TSRMLS_CC) == FAILURE) {
			RETURN_FALSE;
		}
		ce = *pce;
	} else {
		RETURN_FALSE;
	}

	lcname = zend_str_tolower_dup(method_name, method_len);
	if (zend_hash_exists(&ce->function_table, lcname, method_len+1)) {
		efree(lcname);
		RETURN_TRUE;
	} else {
		union _zend_function *func = NULL;

		if (Z_TYPE_P(klass) == IS_OBJECT 
		&& Z_OBJ_HT_P(klass)->get_method != NULL
		&& (func = Z_OBJ_HT_P(klass)->get_method(&klass, method_name, method_len, NULL TSRMLS_CC)) != NULL
		) {
			if (func->type == ZEND_INTERNAL_FUNCTION 
			&& (func->common.fn_flags & ZEND_ACC_CALL_VIA_HANDLER) != 0
			) {
				/* Returns true to the fake Closure's __invoke */
				RETVAL_BOOL((func->common.scope == zend_ce_closure
					&& (method_len == sizeof(ZEND_INVOKE_FUNC_NAME)-1)
					&& memcmp(lcname, ZEND_INVOKE_FUNC_NAME, sizeof(ZEND_INVOKE_FUNC_NAME)-1) == 0) ? 1 : 0);
					
				efree(lcname);
				efree((char*)((zend_internal_function*)func)->function_name);
				efree(func);
				return;
			}
			efree(lcname);
			RETURN_TRUE;
		}
	}
	efree(lcname);
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool property_exists(mixed object_or_class, string property_name)
   Checks if the object or class has a property */
ZEND_FUNCTION(property_exists)
{
	zval *object;
	char *property;
	int property_len;
	zend_class_entry *ce, **pce;
	zend_property_info *property_info;
	zval property_z;
	ulong h;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zs", &object, &property, &property_len) == FAILURE) {
		return;
	}

	if (property_len == 0) {
		RETURN_FALSE;
	}

	if (Z_TYPE_P(object) == IS_STRING) {
		if (zend_lookup_class(Z_STRVAL_P(object), Z_STRLEN_P(object), &pce TSRMLS_CC) == FAILURE) {
			RETURN_FALSE;
		}
		ce = *pce;
	} else if (Z_TYPE_P(object) == IS_OBJECT) {
		ce = Z_OBJCE_P(object);
	} else {
		zend_error(E_WARNING, "First parameter must either be an object or the name of an existing class");
		RETURN_NULL();
	}

	h = zend_get_hash_value(property, property_len+1);
	if (zend_hash_quick_find(&ce->properties_info, property, property_len+1, h, (void **) &property_info) == SUCCESS
		&& (property_info->flags & ZEND_ACC_SHADOW) == 0) {
		RETURN_TRUE;
	}

	ZVAL_STRINGL(&property_z, property, property_len, 0);

	if (Z_TYPE_P(object) ==  IS_OBJECT &&
		Z_OBJ_HANDLER_P(object, has_property) && 
		Z_OBJ_HANDLER_P(object, has_property)(object, &property_z, 2, 0 TSRMLS_CC)) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
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
		char *name;
		int len;

		lc_name = do_alloca(class_name_len + 1, use_heap);
		zend_str_tolower_copy(lc_name, class_name, class_name_len);

		/* Ignore leading "\" */
		name = lc_name;
		len = class_name_len;
		if (lc_name[0] == '\\') {
			name = &lc_name[1];
			len--;
		}
	
		found = zend_hash_find(EG(class_table), name, len+1, (void **) &ce);
		free_alloca(lc_name, use_heap);
		RETURN_BOOL(found == SUCCESS && !(((*ce)->ce_flags & (ZEND_ACC_INTERFACE | ZEND_ACC_TRAIT)) > ZEND_ACC_EXPLICIT_ABSTRACT_CLASS));
	}

 	if (zend_lookup_class(class_name, class_name_len, &ce TSRMLS_CC) == SUCCESS) {
 		RETURN_BOOL(((*ce)->ce_flags & (ZEND_ACC_INTERFACE | (ZEND_ACC_TRAIT - ZEND_ACC_EXPLICIT_ABSTRACT_CLASS))) == 0);
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
		char *name;
		int len;
		
		lc_name = do_alloca(iface_name_len + 1, use_heap);
		zend_str_tolower_copy(lc_name, iface_name, iface_name_len);
	
		/* Ignore leading "\" */
		name = lc_name;
		len = iface_name_len;
		if (lc_name[0] == '\\') {
			name = &lc_name[1];
			len--;
		}

		found = zend_hash_find(EG(class_table), name, len+1, (void **) &ce);
		free_alloca(lc_name, use_heap);
		RETURN_BOOL(found == SUCCESS && (*ce)->ce_flags & ZEND_ACC_INTERFACE);
	}

 	if (zend_lookup_class(iface_name, iface_name_len, &ce TSRMLS_CC) == SUCCESS) {
 		RETURN_BOOL(((*ce)->ce_flags & ZEND_ACC_INTERFACE) > 0);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool trait_exists(string traitname [, bool autoload])
 Checks if the trait exists */
ZEND_FUNCTION(trait_exists)
{
	char *trait_name, *lc_name;
	zend_class_entry **ce;
	int trait_name_len;
	int found;
	zend_bool autoload = 1;
	ALLOCA_FLAG(use_heap)
  
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|b", &trait_name, &trait_name_len, &autoload) == FAILURE) {
		return;
	}
  
	if (!autoload) {
		char *name;
		int len;
		
		lc_name = do_alloca(trait_name_len + 1, use_heap);
		zend_str_tolower_copy(lc_name, trait_name, trait_name_len);
    
		/* Ignore leading "\" */
		name = lc_name;
		len = trait_name_len;
		if (lc_name[0] == '\\') {
			name = &lc_name[1];
			len--;
		}
    
		found = zend_hash_find(EG(class_table), name, len+1, (void **) &ce);
		free_alloca(lc_name, use_heap);
		RETURN_BOOL(found == SUCCESS && (((*ce)->ce_flags & ZEND_ACC_TRAIT) > ZEND_ACC_EXPLICIT_ABSTRACT_CLASS));
	}
  
 	if (zend_lookup_class(trait_name, trait_name_len, &ce TSRMLS_CC) == SUCCESS) {
 		RETURN_BOOL(((*ce)->ce_flags & ZEND_ACC_TRAIT) > ZEND_ACC_EXPLICIT_ABSTRACT_CLASS);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto bool function_exists(string function_name) 
   Checks if the function exists */
ZEND_FUNCTION(function_exists)
{
	char *name;
	int name_len;
	zend_function *func;
	char *lcname;
	zend_bool retval;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
		return;
	}

	lcname = zend_str_tolower_dup(name, name_len);
	
	/* Ignore leading "\" */
	name = lcname;
	if (lcname[0] == '\\') {
		name = &lcname[1];
		name_len--;
	}

	retval = (zend_hash_find(EG(function_table), name, name_len+1, (void **)&func) == SUCCESS);
	
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

/* {{{ proto bool class_alias(string user_class_name , string alias_name [, bool autoload])
   Creates an alias for user defined class */
ZEND_FUNCTION(class_alias)
{
	char *class_name, *alias_name;
	zend_class_entry **ce;
	int class_name_len, alias_name_len;
	int found;
	zend_bool autoload = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|b", &class_name, &class_name_len, &alias_name, &alias_name_len, &autoload) == FAILURE) {
		return;
	}

	found = zend_lookup_class_ex(class_name, class_name_len, NULL, autoload, &ce TSRMLS_CC);
	
	if (found == SUCCESS) {
		if ((*ce)->type == ZEND_USER_CLASS) { 
			if (zend_register_class_alias_ex(alias_name, alias_name_len, *ce TSRMLS_CC) == SUCCESS) {
				RETURN_TRUE;
			} else {
				zend_error(E_WARNING, "Cannot redeclare class %s", alias_name);
				RETURN_FALSE;
			}
		} else {
			zend_error(E_WARNING, "First argument of class_alias() must be a name of user defined class");
			RETURN_FALSE;
		}
	} else {
		zend_error(E_WARNING, "Class '%s' not found", class_name);
		RETURN_FALSE;
	}
}
/* }}} */

#if ZEND_DEBUG
/* {{{ proto void leak(int num_bytes=3)
   Cause an intentional memory leak, for testing/debugging purposes */
ZEND_FUNCTION(leak)
{
	long leakbytes=3;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &leakbytes) == FAILURE) {
		return;
	}

	emalloc(leakbytes);
}
/* }}} */

/* {{{ proto leak_variable(mixed variable [, bool leak_data]) */
ZEND_FUNCTION(leak_variable)
{
	zval *zv;
	zend_bool leak_data = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|b", &zv, &leak_data) == FAILURE) {
		return;
	}

	if (!leak_data) {
		zval_add_ref(&zv);
	} else if (Z_TYPE_P(zv) == IS_RESOURCE) {
		zend_list_addref(Z_RESVAL_P(zv));
	} else if (Z_TYPE_P(zv) == IS_OBJECT) {
		Z_OBJ_HANDLER_P(zv, add_ref)(zv TSRMLS_CC);
	} else {
		zend_error(E_WARNING, "Leaking non-zval data is only applicable to resources and objects");
	}
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
	uint entry_len;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	array_init(return_value);
	zend_hash_internal_pointer_reset(&EG(included_files));
	while (zend_hash_get_current_key_ex(&EG(included_files), &entry, &entry_len, NULL, 0, NULL) == HASH_KEY_IS_STRING) {
		add_next_index_stringl(return_value, entry, entry_len-1, !IS_INTERNED(entry));
		zend_hash_move_forward(&EG(included_files));
	}
}
/* }}} */


/* {{{ proto void trigger_error(string message [, int error_type])
   Generates a user-level error/warning/notice message */
ZEND_FUNCTION(trigger_error)
{
	long error_type = E_USER_NOTICE;
	char *message;
	int message_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &message, &message_len, &error_type) == FAILURE) {
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

	zend_error((int)error_type, "%s", message);
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto string set_error_handler(string error_handler [, int error_types])
   Sets a user-defined error handler function.  Returns the previously defined error handler, or false on error */
ZEND_FUNCTION(set_error_handler)
{
	zval *error_handler;
	char *error_handler_name = NULL;
	long error_type = E_ALL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|l", &error_handler, &error_type) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(error_handler) != IS_NULL) { /* NULL == unset */
		if (!zend_is_callable(error_handler, 0, &error_handler_name TSRMLS_CC)) {
			zend_error(E_WARNING, "%s() expects the argument (%s) to be a valid callback",
					   get_active_function_name(TSRMLS_C), error_handler_name?error_handler_name:"unknown");
			efree(error_handler_name);
			return;
		}
		efree(error_handler_name);
	}

	if (EG(user_error_handler)) {
		RETVAL_ZVAL(EG(user_error_handler), 1, 0);

		zend_stack_push(&EG(user_error_handlers_error_reporting), &EG(user_error_handler_error_reporting), sizeof(EG(user_error_handler_error_reporting)));
		zend_ptr_stack_push(&EG(user_error_handlers), EG(user_error_handler));
	}

	if (Z_TYPE_P(error_handler) == IS_NULL) { /* unset user-defined handler */
		EG(user_error_handler) = NULL;
		return;
	}

	ALLOC_ZVAL(EG(user_error_handler));
	MAKE_COPY_ZVAL(&error_handler, EG(user_error_handler));
	EG(user_error_handler_error_reporting) = (int)error_type;
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
	zval *exception_handler;
	char *exception_handler_name = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &exception_handler) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(exception_handler) != IS_NULL) { /* NULL == unset */
		if (!zend_is_callable(exception_handler, 0, &exception_handler_name TSRMLS_CC)) {
			zend_error(E_WARNING, "%s() expects the argument (%s) to be a valid callback",
					   get_active_function_name(TSRMLS_C), exception_handler_name?exception_handler_name:"unknown");
			efree(exception_handler_name);
			return;
		}
		efree(exception_handler_name);
	}

	if (EG(user_exception_handler)) {
		RETVAL_ZVAL(EG(user_exception_handler), 1, 0);

		zend_ptr_stack_push(&EG(user_exception_handlers), EG(user_exception_handler));
	}

	if (Z_TYPE_P(exception_handler) == IS_NULL) { /* unset user-defined handler */
		EG(user_exception_handler) = NULL;
		return;
	}

	ALLOC_ZVAL(EG(user_exception_handler));
	MAKE_COPY_ZVAL(&exception_handler, EG(user_exception_handler))
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

static int copy_class_or_interface_name(zend_class_entry **pce TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key)
{
	zval *array = va_arg(args, zval *);
	zend_uint mask = va_arg(args, zend_uint);
	zend_uint comply = va_arg(args, zend_uint);
	zend_uint comply_mask = (comply)? mask:0;
	zend_class_entry *ce  = *pce;

	if ((hash_key->nKeyLength==0 || hash_key->arKey[0]!=0)
		&& (comply_mask == (ce->ce_flags & mask))) {
		if (ce->refcount > 1 && 
		    (ce->name_length != hash_key->nKeyLength - 1 || 
		     !same_name(hash_key->arKey, ce->name, ce->name_length))) {
			add_next_index_stringl(array, hash_key->arKey, hash_key->nKeyLength - 1, 1);
		} else {
			add_next_index_stringl(array, ce->name, ce->name_length, 1);
		}
	}
	return ZEND_HASH_APPLY_KEEP;
}

/* {{{ proto array get_declared_traits()
   Returns an array of all declared traits. */
ZEND_FUNCTION(get_declared_traits)
{
	zend_uint mask = ZEND_ACC_TRAIT;
	zend_uint comply = 1;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	array_init(return_value);
	zend_hash_apply_with_arguments(EG(class_table) TSRMLS_CC, (apply_func_args_t) copy_class_or_interface_name, 3, return_value, mask, comply);
}
/* }}} */


/* {{{ proto array get_declared_classes()
   Returns an array of all declared classes. */
ZEND_FUNCTION(get_declared_classes)
{
	zend_uint mask = ZEND_ACC_INTERFACE | (ZEND_ACC_TRAIT & ~ZEND_ACC_EXPLICIT_ABSTRACT_CLASS);
	zend_uint comply = 0;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	array_init(return_value);
	zend_hash_apply_with_arguments(EG(class_table) TSRMLS_CC, (apply_func_args_t) copy_class_or_interface_name, 3, return_value, mask, comply);
}
/* }}} */

/* {{{ proto array get_declared_interfaces()
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


static int copy_function_name(zend_function *func TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key)
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

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	MAKE_STD_ZVAL(internal);
	MAKE_STD_ZVAL(user);

	array_init(internal);
	array_init(user);
	array_init(return_value);

	zend_hash_apply_with_arguments(EG(function_table) TSRMLS_CC, (apply_func_args_t) copy_function_name, 2, internal, user);

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
	if (!EG(active_symbol_table)) {
		zend_rebuild_symbol_table(TSRMLS_C);
	}

	array_init_size(return_value, zend_hash_num_elements(EG(active_symbol_table)));

	zend_hash_copy(Z_ARRVAL_P(return_value), EG(active_symbol_table),
					(copy_ctor_func_t)zval_add_ref, NULL, sizeof(zval *));
}
/* }}} */


#define LAMBDA_TEMP_FUNCNAME	"__lambda_func"
/* {{{ proto string create_function(string args, string code)
   Creates an anonymous function, and returns its name (funny, eh?) */
ZEND_FUNCTION(create_function)
{
	char *eval_code, *function_name, *function_args, *function_code;
	int eval_code_length, function_name_length, function_args_len, function_code_len;
	int retval;
	char *eval_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &function_args, &function_args_len, &function_code, &function_code_len) == FAILURE) {
		return;
	}

	eval_code = (char *) emalloc(sizeof("function " LAMBDA_TEMP_FUNCNAME)
			+function_args_len
			+2	/* for the args parentheses */
			+2	/* for the curly braces */
			+function_code_len);

	eval_code_length = sizeof("function " LAMBDA_TEMP_FUNCNAME "(") - 1;
	memcpy(eval_code, "function " LAMBDA_TEMP_FUNCNAME "(", eval_code_length);

	memcpy(eval_code + eval_code_length, function_args, function_args_len);
	eval_code_length += function_args_len;

	eval_code[eval_code_length++] = ')';
	eval_code[eval_code_length++] = '{';

	memcpy(eval_code + eval_code_length, function_code, function_code_len);
	eval_code_length += function_code_len;

	eval_code[eval_code_length++] = '}';
	eval_code[eval_code_length] = '\0';

	eval_name = zend_make_compiled_string_description("runtime-created function" TSRMLS_CC);
	retval = zend_eval_stringl(eval_code, eval_code_length, NULL, eval_name TSRMLS_CC);
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
		function_name[0] = '\0';

		do {
			function_name_length = 1 + snprintf(function_name + 1, sizeof("lambda_")+MAX_LENGTH_OF_LONG, "lambda_%d", ++EG(lambda_count));
		} while (zend_hash_add(EG(function_table), function_name, function_name_length+1, &new_function, sizeof(zend_function), NULL)==FAILURE);
		zend_hash_del(EG(function_table), LAMBDA_TEMP_FUNCNAME, sizeof(LAMBDA_TEMP_FUNCNAME));
		RETURN_STRINGL(function_name, function_name_length, 0);
	} else {
		zend_hash_del(EG(function_table), LAMBDA_TEMP_FUNCNAME, sizeof(LAMBDA_TEMP_FUNCNAME));
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
	RETURN_LONG((long)tsrm_thread_id());
}
#endif
#endif

/* {{{ proto string get_resource_type(resource res)
   Get the resource type name for a given resource */
ZEND_FUNCTION(get_resource_type)
{
	const char *resource_type;
	zval *z_resource_type;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_resource_type) == FAILURE) {
		return;
	}

	resource_type = zend_rsrc_list_get_rsrc_type(Z_LVAL_P(z_resource_type) TSRMLS_CC);
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

	if (!constant->name) {
		/* skip special constants */
		return 0;
	}

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


/* {{{ proto array get_defined_constants([bool categorize])
   Return an array containing the names and values of all defined constants */
ZEND_FUNCTION(get_defined_constants)
{
	zend_bool categorize = 0;
	
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
			module_names[module->module_number] = (char *)module->name;
			i++;
			zend_hash_move_forward_ex(&module_registry, &pos);
		}
		module_names[i] = "user";

		zend_hash_internal_pointer_reset_ex(EG(zend_constants), &pos);
		while (zend_hash_get_current_data_ex(EG(zend_constants), (void **) &val, &pos) != FAILURE) {
			zval *const_val;

			if (!val->name) {
				/* skip special constants */
				goto next_constant;
			}

			if (val->module_number == PHP_USER_CONSTANT) {
				module_number = i;
			} else if (val->module_number > i || val->module_number < 0) {
				/* should not happen */
				goto next_constant;
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
next_constant:
			zend_hash_move_forward_ex(EG(zend_constants), &pos);
		}
		efree(module_names);
		efree(modules);
	} else {
		zend_hash_apply_with_argument(EG(zend_constants), (apply_func_arg_t) add_constant_info, return_value TSRMLS_CC);
	}
}
/* }}} */


static zval *debug_backtrace_get_args(void **curpos TSRMLS_DC)
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

/* {{{ proto void debug_print_backtrace([int options[, int limit]]) */
ZEND_FUNCTION(debug_print_backtrace)
{
	zend_execute_data *ptr, *skip;
	int lineno, frameno = 0;
	const char *function_name;
	const char *filename;
	const char *class_name = NULL;
	char *call_type;
	const char *include_filename = NULL;
	zval *arg_array = NULL;
	int indent = 0;
	long options = 0;
	long limit = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|ll", &options, &limit) == FAILURE) {
		return;
	}

	ptr = EG(current_execute_data);

	/* skip debug_backtrace() */
	ptr = ptr->prev_execute_data;

	while (ptr && (limit == 0 || frameno < limit)) {
		const char *free_class_name = NULL;

		frameno++;
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

		function_name = (ptr->function_state.function->common.scope &&
			ptr->function_state.function->common.scope->trait_aliases) ?
				zend_resolve_method_name(
					ptr->object ?
						Z_OBJCE_P(ptr->object) : 
						ptr->function_state.function->common.scope,
					ptr->function_state.function) :
				ptr->function_state.function->common.function_name;

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
				if (ptr->function_state.arguments && (options & DEBUG_BACKTRACE_IGNORE_ARGS) == 0) {
					arg_array = debug_backtrace_get_args(ptr->function_state.arguments TSRMLS_CC);
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
			switch (ptr->opline->extended_value) {
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
				add_next_index_string(arg_array, (char*)include_filename, 1);
			}
			call_type = NULL;
		}
		zend_printf("#%-2d ", indent);
		if (class_name) {
			ZEND_PUTS(class_name);
			ZEND_PUTS(call_type);
		}
		zend_printf("%s(", function_name);
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
			efree((char*)free_class_name);
		}
	}
}

/* }}} */

ZEND_API void zend_fetch_debug_backtrace(zval *return_value, int skip_last, int options, int limit TSRMLS_DC)
{
	zend_execute_data *ptr, *skip;
	int lineno, frameno = 0;
	const char *function_name;
	const char *filename;
	const char *class_name;
	const char *include_filename = NULL;
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

	while (ptr && (limit == 0 || frameno < limit)) {
		frameno++;
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
			add_assoc_string_ex(stack_frame, "file", sizeof("file"), (char*)filename, 1);
			add_assoc_long_ex(stack_frame, "line", sizeof("line"), lineno);

			/* try to fetch args only if an FCALL was just made - elsewise we're in the middle of a function
			 * and debug_baktrace() might have been called by the error_handler. in this case we don't 
			 * want to pop anything of the argument-stack */
		} else {
			zend_execute_data *prev = skip->prev_execute_data;

			while (prev) {
				if (prev->function_state.function &&
					prev->function_state.function->common.type != ZEND_USER_FUNCTION &&
					!(prev->function_state.function->common.type == ZEND_INTERNAL_FUNCTION &&
						(prev->function_state.function->common.fn_flags & ZEND_ACC_CALL_VIA_HANDLER))) {
					break;
				}				    
				if (prev->op_array) {
					add_assoc_string_ex(stack_frame, "file", sizeof("file"), (char*)prev->op_array->filename, 1);
					add_assoc_long_ex(stack_frame, "line", sizeof("line"), prev->opline->lineno);
					break;
				}
				prev = prev->prev_execute_data;
			}
			filename = NULL;
		}

		function_name = (ptr->function_state.function->common.scope &&
			ptr->function_state.function->common.scope->trait_aliases) ?
				zend_resolve_method_name(
					ptr->object ?
						Z_OBJCE_P(ptr->object) : 
						ptr->function_state.function->common.scope,
					ptr->function_state.function) :
				ptr->function_state.function->common.function_name;

		if (function_name) {
			add_assoc_string_ex(stack_frame, "function", sizeof("function"), (char*)function_name, 1);

			if (ptr->object && Z_TYPE_P(ptr->object) == IS_OBJECT) {
				if (ptr->function_state.function->common.scope) {
					add_assoc_string_ex(stack_frame, "class", sizeof("class"), (char*)ptr->function_state.function->common.scope->name, 1);
				} else {
					zend_uint class_name_len;
					int dup;

					dup = zend_get_object_classname(ptr->object, &class_name, &class_name_len TSRMLS_CC);
					add_assoc_string_ex(stack_frame, "class", sizeof("class"), (char*)class_name, dup);
					
				}
				if ((options & DEBUG_BACKTRACE_PROVIDE_OBJECT) != 0) {
					add_assoc_zval_ex(stack_frame, "object", sizeof("object"), ptr->object);
					Z_ADDREF_P(ptr->object);
				}

				add_assoc_string_ex(stack_frame, "type", sizeof("type"), "->", 1);
			} else if (ptr->function_state.function->common.scope) {
				add_assoc_string_ex(stack_frame, "class", sizeof("class"), (char*)ptr->function_state.function->common.scope->name, 1);
				add_assoc_string_ex(stack_frame, "type", sizeof("type"), "::", 1);
			}

			if ((options & DEBUG_BACKTRACE_IGNORE_ARGS) == 0 && 
				((! ptr->opline) || ((ptr->opline->opcode == ZEND_DO_FCALL_BY_NAME) || (ptr->opline->opcode == ZEND_DO_FCALL)))) {
				if (ptr->function_state.arguments) {
					add_assoc_zval_ex(stack_frame, "args", sizeof("args"), debug_backtrace_get_args(ptr->function_state.arguments TSRMLS_CC));
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
			switch (ptr->opline->extended_value) {
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

				/* include_filename always points to the last filename of the last last called-function.
				   if we have called include in the frame above - this is the file we have included.
				 */

				add_next_index_string(arg_array, (char*)include_filename, 1);
				add_assoc_zval_ex(stack_frame, "args", sizeof("args"), arg_array);
			}

			add_assoc_string_ex(stack_frame, "function", sizeof("function"), (char*)function_name, 1);
		}

		add_next_index_zval(return_value, stack_frame);

		include_filename = filename; 

		ptr = skip->prev_execute_data;
	}
}
/* }}} */


/* {{{ proto array debug_backtrace([int options[, int limit]])
   Return backtrace as array */
ZEND_FUNCTION(debug_backtrace)
{
	long options = DEBUG_BACKTRACE_PROVIDE_OBJECT;
	long limit = 0;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|ll", &options, &limit) == FAILURE) {
		return;
	}

	zend_fetch_debug_backtrace(return_value, 1, options, limit TSRMLS_CC);
}
/* }}} */

/* {{{ proto bool extension_loaded(string extension_name)
   Returns true if the named extension is loaded */
ZEND_FUNCTION(extension_loaded)
{
	char *extension_name;
	int extension_name_len;
	char *lcname;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &extension_name, &extension_name_len) == FAILURE) {
		return;
	}

	lcname = zend_str_tolower_dup(extension_name, extension_name_len);
	if (zend_hash_exists(&module_registry, lcname, extension_name_len+1)) {
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
	char *extension_name, *lcname;
	int extension_name_len, array;
	zend_module_entry *module;
	HashPosition iterator;
	zend_function *zif;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &extension_name, &extension_name_len) == FAILURE) {
		return;
	}
	if (strncasecmp(extension_name, "zend", sizeof("zend"))) {
		lcname = zend_str_tolower_dup(extension_name, extension_name_len);
	} else {
		lcname = estrdup("core");
	}
	if (zend_hash_find(&module_registry, lcname,
		extension_name_len+1, (void**)&module) == FAILURE) {
		efree(lcname);
		RETURN_FALSE;
	}

	zend_hash_internal_pointer_reset_ex(CG(function_table), &iterator);
	if (module->functions) {
		/* avoid BC break, if functions list is empty, will return an empty array */
		array_init(return_value);
		array = 1;
	} else {
		array = 0;
	}
	while (zend_hash_get_current_data_ex(CG(function_table), (void **) &zif, &iterator) == SUCCESS) {
		if (zif->common.type==ZEND_INTERNAL_FUNCTION
			&& zif->internal_function.module == module) {
			if (!array) {
				array_init(return_value);
				array = 1;
			}
			add_next_index_string(return_value, zif->common.function_name, 1);
		}
		zend_hash_move_forward_ex(CG(function_table), &iterator);
	}

	efree(lcname);

	if (!array) {
		RETURN_FALSE;
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
