/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2018 Zend Technologies Ltd. (http://www.zend.com) |
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
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_API.h"
#include "zend_gc.h"
#include "zend_builtin_functions.h"
#include "zend_constants.h"
#include "zend_ini.h"
#include "zend_exceptions.h"
#include "zend_extensions.h"
#include "zend_closures.h"
#include "zend_generators.h"

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
static ZEND_FUNCTION(get_resources);
static ZEND_FUNCTION(get_loaded_extensions);
static ZEND_FUNCTION(extension_loaded);
static ZEND_FUNCTION(get_extension_funcs);
static ZEND_FUNCTION(get_defined_constants);
static ZEND_FUNCTION(debug_backtrace);
static ZEND_FUNCTION(debug_print_backtrace);
#if ZEND_DEBUG && defined(ZTS)
static ZEND_FUNCTION(zend_thread_id);
#endif
static ZEND_FUNCTION(gc_mem_caches);
static ZEND_FUNCTION(gc_collect_cycles);
static ZEND_FUNCTION(gc_enabled);
static ZEND_FUNCTION(gc_enable);
static ZEND_FUNCTION(gc_disable);
static ZEND_FUNCTION(gc_status);

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

ZEND_BEGIN_ARG_INFO_EX(arginfo_define, 0, 0, 2)
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_get_defined_functions, 0, 0, 0)
	ZEND_ARG_INFO(0, exclude_disabled)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_create_function, 0, 0, 2)
	ZEND_ARG_INFO(0, args)
	ZEND_ARG_INFO(0, code)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_get_resource_type, 0, 0, 1)
	ZEND_ARG_INFO(0, res)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_get_resources, 0, 0, 0)
	ZEND_ARG_INFO(0, type)
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
	ZEND_ARG_INFO(0, limit)
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
	ZEND_FE(get_defined_functions, 		arginfo_get_defined_functions)
	ZEND_FE(get_defined_vars,		arginfo_zend__void)
	ZEND_DEP_FE(create_function,		arginfo_create_function)
	ZEND_FE(get_resource_type,		arginfo_get_resource_type)
	ZEND_FE(get_resources,			arginfo_get_resources)
	ZEND_FE(get_loaded_extensions,		arginfo_get_loaded_extensions)
	ZEND_FE(extension_loaded,		arginfo_extension_loaded)
	ZEND_FE(get_extension_funcs,		arginfo_extension_loaded)
	ZEND_FE(get_defined_constants,		arginfo_get_defined_constants)
	ZEND_FE(debug_backtrace, 		arginfo_debug_backtrace)
	ZEND_FE(debug_print_backtrace, 		arginfo_debug_print_backtrace)
#if ZEND_DEBUG && defined(ZTS)
	ZEND_FE(zend_thread_id,		NULL)
#endif
	ZEND_FE(gc_mem_caches,      arginfo_zend__void)
	ZEND_FE(gc_collect_cycles, 	arginfo_zend__void)
	ZEND_FE(gc_enabled, 		arginfo_zend__void)
	ZEND_FE(gc_enable, 		arginfo_zend__void)
	ZEND_FE(gc_disable, 		arginfo_zend__void)
	ZEND_FE(gc_status, 		arginfo_zend__void)
	ZEND_FE_END
};
/* }}} */

ZEND_MINIT_FUNCTION(core) { /* {{{ */
	zend_class_entry class_entry;

	INIT_CLASS_ENTRY(class_entry, "stdClass", NULL);
	zend_standard_class_def = zend_register_internal_class(&class_entry);

	zend_register_default_classes();

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

int zend_startup_builtin_functions(void) /* {{{ */
{
	zend_builtin_module.module_number = 0;
	zend_builtin_module.type = MODULE_PERSISTENT;
	return (EG(current_module) = zend_register_module_ex(&zend_builtin_module)) == NULL ? FAILURE : SUCCESS;
}
/* }}} */

/* {{{ proto string zend_version(void)
   Get the version of the Zend Engine */
ZEND_FUNCTION(zend_version)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_STRINGL(ZEND_VERSION, sizeof(ZEND_VERSION)-1);
}
/* }}} */

/* {{{ proto int gc_mem_caches(void)
   Reclaims memory used by MM caches.
   Returns number of freed bytes */
ZEND_FUNCTION(gc_mem_caches)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(zend_mm_gc(zend_mm_get_heap()));
}
/* }}} */

/* {{{ proto int gc_collect_cycles(void)
   Forces collection of any existing garbage cycles.
   Returns number of freed zvals */
ZEND_FUNCTION(gc_collect_cycles)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(gc_collect_cycles());
}
/* }}} */

/* {{{ proto void gc_enabled(void)
   Returns status of the circular reference collector */
ZEND_FUNCTION(gc_enabled)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(gc_enabled());
}
/* }}} */

/* {{{ proto void gc_enable(void)
   Activates the circular reference collector */
ZEND_FUNCTION(gc_enable)
{
	zend_string *key;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	key = zend_string_init("zend.enable_gc", sizeof("zend.enable_gc")-1, 0);
	zend_alter_ini_entry_chars(key, "1", sizeof("1")-1, ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME);
	zend_string_release_ex(key, 0);
}
/* }}} */

/* {{{ proto void gc_disable(void)
   Deactivates the circular reference collector */
ZEND_FUNCTION(gc_disable)
{
	zend_string *key;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	key = zend_string_init("zend.enable_gc", sizeof("zend.enable_gc")-1, 0);
	zend_alter_ini_entry_chars(key, "0", sizeof("0")-1, ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME);
	zend_string_release_ex(key, 0);
}
/* }}} */

/* {{{ proto array gc_status(void)
   Returns current GC statistics */
ZEND_FUNCTION(gc_status)
{
	zend_gc_status status;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	zend_gc_get_status(&status);

	array_init_size(return_value, 3);

	add_assoc_long_ex(return_value, "runs", sizeof("runs")-1, (long)status.runs);
	add_assoc_long_ex(return_value, "collected", sizeof("collected")-1, (long)status.collected);
	add_assoc_long_ex(return_value, "threshold", sizeof("threshold")-1, (long)status.threshold);
	add_assoc_long_ex(return_value, "roots", sizeof("roots")-1, (long)status.num_roots);
}
/* }}} */

/* {{{ proto int func_num_args(void)
   Get the number of arguments that were passed to the function */
ZEND_FUNCTION(func_num_args)
{
	zend_execute_data *ex = EX(prev_execute_data);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (ZEND_CALL_INFO(ex) & ZEND_CALL_CODE) {
		zend_error(E_WARNING, "func_num_args():  Called from the global scope - no function context");
		RETURN_LONG(-1);
	}

	if (zend_forbid_dynamic_call("func_num_args()") == FAILURE) {
		RETURN_LONG(-1);
	}

	RETURN_LONG(ZEND_CALL_NUM_ARGS(ex));
}
/* }}} */

/* {{{ proto mixed func_get_arg(int arg_num)
   Get the $arg_num'th argument that was passed to the function */
ZEND_FUNCTION(func_get_arg)
{
	uint32_t arg_count, first_extra_arg;
	zval *arg;
	zend_long requested_offset;
	zend_execute_data *ex;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &requested_offset) == FAILURE) {
		return;
	}

	if (requested_offset < 0) {
		zend_error(E_WARNING, "func_get_arg():  The argument number should be >= 0");
		RETURN_FALSE;
	}

	ex = EX(prev_execute_data);
	if (ZEND_CALL_INFO(ex) & ZEND_CALL_CODE) {
		zend_error(E_WARNING, "func_get_arg():  Called from the global scope - no function context");
		RETURN_FALSE;
	}

	if (zend_forbid_dynamic_call("func_get_arg()") == FAILURE) {
		RETURN_FALSE;
	}

	arg_count = ZEND_CALL_NUM_ARGS(ex);

	if ((zend_ulong)requested_offset >= arg_count) {
		zend_error(E_WARNING, "func_get_arg():  Argument " ZEND_LONG_FMT " not passed to function", requested_offset);
		RETURN_FALSE;
	}

	first_extra_arg = ex->func->op_array.num_args;
	if ((zend_ulong)requested_offset >= first_extra_arg && (ZEND_CALL_NUM_ARGS(ex) > first_extra_arg)) {
		arg = ZEND_CALL_VAR_NUM(ex, ex->func->op_array.last_var + ex->func->op_array.T) + (requested_offset - first_extra_arg);
	} else {
		arg = ZEND_CALL_ARG(ex, requested_offset + 1);
	}
	if (EXPECTED(!Z_ISUNDEF_P(arg))) {
		ZVAL_COPY_DEREF(return_value, arg);
	}
}
/* }}} */

/* {{{ proto array func_get_args()
   Get an array of the arguments that were passed to the function */
ZEND_FUNCTION(func_get_args)
{
	zval *p, *q;
	uint32_t arg_count, first_extra_arg;
	uint32_t i;
	zend_execute_data *ex = EX(prev_execute_data);

	if (ZEND_CALL_INFO(ex) & ZEND_CALL_CODE) {
		zend_error(E_WARNING, "func_get_args():  Called from the global scope - no function context");
		RETURN_FALSE;
	}

	if (zend_forbid_dynamic_call("func_get_args()") == FAILURE) {
		RETURN_FALSE;
	}

	arg_count = ZEND_CALL_NUM_ARGS(ex);

	if (arg_count) {
		array_init_size(return_value, arg_count);
		first_extra_arg = ex->func->op_array.num_args;
		zend_hash_real_init_packed(Z_ARRVAL_P(return_value));
		ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(return_value)) {
			i = 0;
			p = ZEND_CALL_ARG(ex, 1);
			if (arg_count > first_extra_arg) {
				while (i < first_extra_arg) {
					q = p;
					if (EXPECTED(Z_TYPE_INFO_P(q) != IS_UNDEF)) {
						ZVAL_DEREF(q);
						if (Z_OPT_REFCOUNTED_P(q)) {
							Z_ADDREF_P(q);
						}
					} else {
						q = &EG(uninitialized_zval);
					}
					ZEND_HASH_FILL_ADD(q);
					p++;
					i++;
				}
				p = ZEND_CALL_VAR_NUM(ex, ex->func->op_array.last_var + ex->func->op_array.T);
			}
			while (i < arg_count) {
				q = p;
				if (EXPECTED(Z_TYPE_INFO_P(q) != IS_UNDEF)) {
					ZVAL_DEREF(q);
					if (Z_OPT_REFCOUNTED_P(q)) {
						Z_ADDREF_P(q);
					}
				} else {
					q = &EG(uninitialized_zval);
				}
				ZEND_HASH_FILL_ADD(q);
				p++;
				i++;
			}
		} ZEND_HASH_FILL_END();
		Z_ARRVAL_P(return_value)->nNumOfElements = arg_count;
	} else {
		ZVAL_EMPTY_ARRAY(return_value);
	}
}
/* }}} */

/* {{{ proto int strlen(string str)
   Get string length
   Warning: This function is special-cased by zend_compile.c and so is usually bypassed */
ZEND_FUNCTION(strlen)
{
	zend_string *s;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(s)
	ZEND_PARSE_PARAMETERS_END();

	RETVAL_LONG(ZSTR_LEN(s));
}
/* }}} */

/* {{{ proto int strcmp(string str1, string str2)
   Binary safe string comparison */
ZEND_FUNCTION(strcmp)
{
	zend_string *s1, *s2;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR(s1)
		Z_PARAM_STR(s2)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG(zend_binary_strcmp(ZSTR_VAL(s1), ZSTR_LEN(s1), ZSTR_VAL(s2), ZSTR_LEN(s2)));
}
/* }}} */

/* {{{ proto int strncmp(string str1, string str2, int len)
   Binary safe string comparison */
ZEND_FUNCTION(strncmp)
{
	zend_string *s1, *s2;
	zend_long len;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_STR(s1)
		Z_PARAM_STR(s2)
		Z_PARAM_LONG(len)
	ZEND_PARSE_PARAMETERS_END();

	if (len < 0) {
		zend_error(E_WARNING, "Length must be greater than or equal to 0");
		RETURN_FALSE;
	}

	RETURN_LONG(zend_binary_strncmp(ZSTR_VAL(s1), ZSTR_LEN(s1), ZSTR_VAL(s2), ZSTR_LEN(s2), len));
}
/* }}} */

/* {{{ proto int strcasecmp(string str1, string str2)
   Binary safe case-insensitive string comparison */
ZEND_FUNCTION(strcasecmp)
{
	zend_string *s1, *s2;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR(s1)
		Z_PARAM_STR(s2)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG(zend_binary_strcasecmp(ZSTR_VAL(s1), ZSTR_LEN(s1), ZSTR_VAL(s2), ZSTR_LEN(s2)));
}
/* }}} */

/* {{{ proto int strncasecmp(string str1, string str2, int len)
   Binary safe string comparison */
ZEND_FUNCTION(strncasecmp)
{
	zend_string *s1, *s2;
	zend_long len;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_STR(s1)
		Z_PARAM_STR(s2)
		Z_PARAM_LONG(len)
	ZEND_PARSE_PARAMETERS_END();

	if (len < 0) {
		zend_error(E_WARNING, "Length must be greater than or equal to 0");
		RETURN_FALSE;
	}

	RETURN_LONG(zend_binary_strncasecmp(ZSTR_VAL(s1), ZSTR_LEN(s1), ZSTR_VAL(s2), ZSTR_LEN(s2), len));
}
/* }}} */

/* {{{ proto mixed each(array &arr)
   Return the currently pointed key..value pair in the passed array, and advance the pointer to the next element, or false if there is no element at this place */
ZEND_FUNCTION(each)
{
	zval *array, *entry, tmp;
	zend_ulong num_key;
	HashTable *target_hash;
	zend_string *key;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z/", &array) == FAILURE) {
		return;
	}

	if (!EG(each_deprecation_thrown)) {
		zend_error(E_DEPRECATED, "The each() function is deprecated. This message will be suppressed on further calls");
		EG(each_deprecation_thrown) = 1;
	}

	target_hash = HASH_OF(array);
	if (!target_hash) {
		zend_error(E_WARNING,"Variable passed to each() is not an array or object");
		return;
	}
	while (1) {
		entry = zend_hash_get_current_data(target_hash);
		if (!entry) {
			RETURN_FALSE;
		} else if (Z_TYPE_P(entry) == IS_INDIRECT) {
			entry = Z_INDIRECT_P(entry);
			if (Z_TYPE_P(entry) == IS_UNDEF) {
				zend_hash_move_forward(target_hash);
				continue;
			}
		}
		break;
	}
	array_init_size(return_value, 4);
	zend_hash_real_init_mixed(Z_ARRVAL_P(return_value));

	/* add value elements */
	ZVAL_DEREF(entry);
	if (Z_REFCOUNTED_P(entry)) {
		GC_ADDREF_EX(Z_COUNTED_P(entry), 2);
	}
	zend_hash_index_add_new(Z_ARRVAL_P(return_value), 1, entry);
	zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_VALUE), entry);

	/* add the key elements */
	if (zend_hash_get_current_key(target_hash, &key, &num_key) == HASH_KEY_IS_STRING) {
		ZVAL_STR_COPY(&tmp, key);
		Z_TRY_ADDREF(tmp);
	} else {
		ZVAL_LONG(&tmp, num_key);
	}
	zend_hash_index_add_new(Z_ARRVAL_P(return_value), 0, &tmp);
	zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_KEY), &tmp);
	zend_hash_move_forward(target_hash);
}
/* }}} */

/* {{{ proto int error_reporting([int new_error_level])
   Return the current error_reporting level, and if an argument was passed - change to the new level */
ZEND_FUNCTION(error_reporting)
{
	zval *err = NULL;
	int old_error_reporting;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(err)
	ZEND_PARSE_PARAMETERS_END();

	old_error_reporting = EG(error_reporting);
	if (ZEND_NUM_ARGS() != 0) {
		zend_string *new_val = zval_get_string(err);
		do {
			zend_ini_entry *p = EG(error_reporting_ini_entry);

			if (!p) {
				zval *zv = zend_hash_find_ex(EG(ini_directives), ZSTR_KNOWN(ZEND_STR_ERROR_REPORTING), 1);
				if (zv) {
					p = EG(error_reporting_ini_entry) = (zend_ini_entry*)Z_PTR_P(zv);
				} else {
					break;
				}
			}
			if (!p->modified) {
				if (!EG(modified_ini_directives)) {
					ALLOC_HASHTABLE(EG(modified_ini_directives));
					zend_hash_init(EG(modified_ini_directives), 8, NULL, NULL, 0);
				}
				if (EXPECTED(zend_hash_add_ptr(EG(modified_ini_directives), ZSTR_KNOWN(ZEND_STR_ERROR_REPORTING), p) != NULL)) {
					p->orig_value = p->value;
					p->orig_modifiable = p->modifiable;
					p->modified = 1;
				}
			} else if (p->orig_value != p->value) {
				zend_string_release_ex(p->value, 0);
			}

			p->value = new_val;
			if (Z_TYPE_P(err) == IS_LONG) {
				EG(error_reporting) = Z_LVAL_P(err);
			} else {
				EG(error_reporting) = atoi(ZSTR_VAL(p->value));
			}
		} while (0);
	}

	RETVAL_LONG(old_error_reporting);
}
/* }}} */

static int validate_constant_array(HashTable *ht) /* {{{ */
{
	int ret = 1;
	zval *val;

	GC_PROTECT_RECURSION(ht);
	ZEND_HASH_FOREACH_VAL_IND(ht, val) {
		ZVAL_DEREF(val);
		if (Z_REFCOUNTED_P(val)) {
			if (Z_TYPE_P(val) == IS_ARRAY) {
				if (Z_REFCOUNTED_P(val)) {
					if (Z_IS_RECURSIVE_P(val)) {
						zend_error(E_WARNING, "Constants cannot be recursive arrays");
						ret = 0;
						break;
					} else if (!validate_constant_array(Z_ARRVAL_P(val))) {
						ret = 0;
						break;
					}
				}
			} else if (Z_TYPE_P(val) != IS_STRING && Z_TYPE_P(val) != IS_RESOURCE) {
				zend_error(E_WARNING, "Constants may only evaluate to scalar values, arrays or resources");
				ret = 0;
				break;
			}
		}
	} ZEND_HASH_FOREACH_END();
	GC_UNPROTECT_RECURSION(ht);
	return ret;
}
/* }}} */

static void copy_constant_array(zval *dst, zval *src) /* {{{ */
{
	zend_string *key;
	zend_ulong idx;
	zval *new_val, *val;

	array_init_size(dst, zend_hash_num_elements(Z_ARRVAL_P(src)));
	ZEND_HASH_FOREACH_KEY_VAL_IND(Z_ARRVAL_P(src), idx, key, val) {
		/* constant arrays can't contain references */
		ZVAL_DEREF(val);
		if (key) {
			new_val = zend_hash_add_new(Z_ARRVAL_P(dst), key, val);
		} else {
			new_val = zend_hash_index_add_new(Z_ARRVAL_P(dst), idx, val);
		}
		if (Z_TYPE_P(val) == IS_ARRAY) {
			if (Z_REFCOUNTED_P(val)) {
				copy_constant_array(new_val, val);
			}
		} else {
			Z_TRY_ADDREF_P(val);
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ proto bool define(string constant_name, mixed value[, bool case_insensitive])
   Define a new constant */
ZEND_FUNCTION(define)
{
	zend_string *name;
	zval *val, val_free;
	zend_bool non_cs = 0;
	int case_sensitive = CONST_CS;
	zend_constant c;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(name)
		Z_PARAM_ZVAL(val)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(non_cs)
	ZEND_PARSE_PARAMETERS_END();

	if (non_cs) {
		case_sensitive = 0;
	}

	if (zend_memnstr(ZSTR_VAL(name), "::", sizeof("::") - 1, ZSTR_VAL(name) + ZSTR_LEN(name))) {
		zend_error(E_WARNING, "Class constants cannot be defined or redefined");
		RETURN_FALSE;
	}

	ZVAL_UNDEF(&val_free);

repeat:
	switch (Z_TYPE_P(val)) {
		case IS_LONG:
		case IS_DOUBLE:
		case IS_STRING:
		case IS_FALSE:
		case IS_TRUE:
		case IS_NULL:
		case IS_RESOURCE:
			break;
		case IS_ARRAY:
			if (Z_REFCOUNTED_P(val)) {
				if (!validate_constant_array(Z_ARRVAL_P(val))) {
					RETURN_FALSE;
				} else {
					copy_constant_array(&c.value, val);
					goto register_constant;
				}
			}
			break;
		case IS_OBJECT:
			if (Z_TYPE(val_free) == IS_UNDEF) {
				if (Z_OBJ_HT_P(val)->get) {
					zval rv;
					val = Z_OBJ_HT_P(val)->get(val, &rv);
					ZVAL_COPY_VALUE(&val_free, val);
					goto repeat;
				} else if (Z_OBJ_HT_P(val)->cast_object) {
					if (Z_OBJ_HT_P(val)->cast_object(val, &val_free, IS_STRING) == SUCCESS) {
						val = &val_free;
						break;
					}
				}
			}
			/* no break */
		default:
			zend_error(E_WARNING, "Constants may only evaluate to scalar values, arrays or resources");
			zval_ptr_dtor(&val_free);
			RETURN_FALSE;
	}

	ZVAL_COPY(&c.value, val);
	zval_ptr_dtor(&val_free);

register_constant:
	if (non_cs) {
		zend_error(E_DEPRECATED,
			"define(): Declaration of case-insensitive constants is deprecated");
	}

	/* non persistent */
	ZEND_CONSTANT_SET_FLAGS(&c, case_sensitive, PHP_USER_CONSTANT);
	c.name = zend_string_copy(name);
	if (zend_register_constant(&c) == SUCCESS) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool defined(string constant_name)
   Check whether a constant exists
   Warning: This function is special-cased by zend_compile.c and so is usually bypassed */
ZEND_FUNCTION(defined)
{
	zend_string *name;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	if (zend_get_constant_ex(name, zend_get_executed_scope(), ZEND_FETCH_CLASS_SILENT | ZEND_GET_CONSTANT_NO_DEPRECATION_CHECK)) {
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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|o", &obj) == FAILURE) {
		RETURN_FALSE;
	}

	if (!obj) {
		zend_class_entry *scope = zend_get_executed_scope();

		if (scope) {
			RETURN_STR_COPY(scope->name);
		} else {
			zend_error(E_WARNING, "get_class() called without object from outside a class");
			RETURN_FALSE;
		}
	}

	RETURN_STR_COPY(Z_OBJCE_P(obj)->name);
}
/* }}} */

/* {{{ proto string get_called_class()
   Retrieves the "Late Static Binding" class name */
ZEND_FUNCTION(get_called_class)
{
	zend_class_entry *called_scope;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	called_scope = zend_get_called_scope(execute_data);
	if (called_scope) {
		RETURN_STR_COPY(called_scope->name);
	} else {
		zend_class_entry *scope = zend_get_executed_scope();
		if (!scope)  {
			zend_error(E_WARNING, "get_called_class() called from outside a class");
		}
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto mixed get_parent_class([mixed object])
   Retrieves the parent class name for object or class or current scope or false if not in a scope. */
ZEND_FUNCTION(get_parent_class)
{
	zval *arg;
	zend_class_entry *ce = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|z", &arg) == FAILURE) {
		return;
	}

	if (!ZEND_NUM_ARGS()) {
		ce = zend_get_executed_scope();
		if (ce && ce->parent) {
			RETURN_STR_COPY(ce->parent->name);
		} else {
			RETURN_FALSE;
		}
	}

	if (Z_TYPE_P(arg) == IS_OBJECT) {
		ce = Z_OBJ_P(arg)->ce;
	} else if (Z_TYPE_P(arg) == IS_STRING) {
	    ce = zend_lookup_class(Z_STR_P(arg));
	}

	if (ce && ce->parent) {
		RETURN_STR_COPY(ce->parent->name);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

static void is_a_impl(INTERNAL_FUNCTION_PARAMETERS, zend_bool only_subclass) /* {{{ */
{
	zval *obj;
	zend_string *class_name;
	zend_class_entry *instance_ce;
	zend_class_entry *ce;
	zend_bool allow_string = only_subclass;
	zend_bool retval;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_ZVAL(obj)
		Z_PARAM_STR(class_name)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(allow_string)
	ZEND_PARSE_PARAMETERS_END();
	/*
	 * allow_string - is_a default is no, is_subclass_of is yes.
	 *   if it's allowed, then the autoloader will be called if the class does not exist.
	 *   default behaviour is different, as 'is_a' used to be used to test mixed return values
	 *   and there is no easy way to deprecate this.
	 */

	if (allow_string && Z_TYPE_P(obj) == IS_STRING) {
		instance_ce = zend_lookup_class(Z_STR_P(obj));
		if (!instance_ce) {
			RETURN_FALSE;
		}
	} else if (Z_TYPE_P(obj) == IS_OBJECT) {
		instance_ce = Z_OBJCE_P(obj);
	} else {
		RETURN_FALSE;
	}

	if (!only_subclass && EXPECTED(zend_string_equals(instance_ce->name, class_name))) {
		retval = 1;
	} else {
		ce = zend_lookup_class_ex(class_name, NULL, 0);
		if (!ce) {
			retval = 0;
		} else {
			if (only_subclass && instance_ce == ce) {
				retval = 0;
			} else {
				retval = instanceof_function(instance_ce, ce);
			}
		}
	}

	RETURN_BOOL(retval);
}
/* }}} */

/* {{{ proto bool is_subclass_of(mixed object_or_string, string class_name [, bool allow_string])
   Returns true if the object has this class as one of its parents */
ZEND_FUNCTION(is_subclass_of)
{
	is_a_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto bool is_a(mixed object_or_string, string class_name [, bool allow_string])
   Returns true if the first argument is an object and is this class or has this class as one of its parents, */
ZEND_FUNCTION(is_a)
{
	is_a_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ add_class_vars */
static void add_class_vars(zend_class_entry *scope, zend_class_entry *ce, int statics, zval *return_value)
{
	zend_property_info *prop_info;
	zval *prop, prop_copy;
	zend_string *key;

	ZEND_HASH_FOREACH_STR_KEY_PTR(&ce->properties_info, key, prop_info) {
		if (((prop_info->flags & ZEND_ACC_SHADOW) &&
		     prop_info->ce != scope) ||
		    ((prop_info->flags & ZEND_ACC_PROTECTED) &&
		     !zend_check_protected(prop_info->ce, scope)) ||
		    ((prop_info->flags & ZEND_ACC_PRIVATE) &&
		      ce != scope &&
			  prop_info->ce != scope)) {
			continue;
		}
		prop = NULL;
		if (statics && (prop_info->flags & ZEND_ACC_STATIC) != 0) {
			prop = &ce->default_static_members_table[prop_info->offset];
			ZVAL_DEINDIRECT(prop);
		} else if (!statics && (prop_info->flags & ZEND_ACC_STATIC) == 0) {
			prop = &ce->default_properties_table[OBJ_PROP_TO_NUM(prop_info->offset)];
		}
		if (!prop || Z_TYPE_P(prop) == IS_UNDEF) {
			continue;
		}

		/* copy: enforce read only access */
		ZVAL_DEREF(prop);
		ZVAL_COPY_OR_DUP(&prop_copy, prop);
		prop = &prop_copy;

		/* this is necessary to make it able to work with default array
		 * properties, returned to user */
		if (Z_OPT_TYPE_P(prop) == IS_CONSTANT_AST) {
			if (UNEXPECTED(zval_update_constant_ex(prop, NULL) != SUCCESS)) {
				return;
			}
		}

		zend_hash_add_new(Z_ARRVAL_P(return_value), key, prop);
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ proto array get_class_vars(string class_name)
   Returns an array of default properties of the class. */
ZEND_FUNCTION(get_class_vars)
{
	zend_string *class_name;
	zend_class_entry *ce, *scope;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &class_name) == FAILURE) {
		return;
	}

	ce = zend_lookup_class(class_name);
	if (!ce) {
		RETURN_FALSE;
	} else {
		array_init(return_value);
		if (UNEXPECTED(!(ce->ce_flags & ZEND_ACC_CONSTANTS_UPDATED))) {
			if (UNEXPECTED(zend_update_class_constants(ce) != SUCCESS)) {
				return;
			}
		}
		scope = zend_get_executed_scope();
		add_class_vars(scope, ce, 0, return_value);
		add_class_vars(scope, ce, 1, return_value);
	}
}
/* }}} */

/* {{{ proto array get_object_vars(object obj)
   Returns an array of object properties */
ZEND_FUNCTION(get_object_vars)
{
	zval *obj;
	zval *value;
	HashTable *properties;
	zend_string *key;
	zend_object *zobj;
	zend_ulong num_key;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT(obj)
	ZEND_PARSE_PARAMETERS_END();

	if (Z_OBJ_HT_P(obj)->get_properties == NULL) {
		RETURN_FALSE;
	}

	properties = Z_OBJ_HT_P(obj)->get_properties(obj);

	if (properties == NULL) {
		RETURN_FALSE;
	}

	zobj = Z_OBJ_P(obj);

	if (!zobj->ce->default_properties_count && properties == zobj->properties && !GC_IS_RECURSIVE(properties)) {
		/* fast copy */
		if (EXPECTED(zobj->handlers == &std_object_handlers)) {
			RETURN_ARR(zend_proptable_to_symtable(properties, 0));
		}
		RETURN_ARR(zend_proptable_to_symtable(properties, 1));
	} else {
		array_init_size(return_value, zend_hash_num_elements(properties));

		ZEND_HASH_FOREACH_KEY_VAL(properties, num_key, key, value) {
			zend_bool unmangle = 0;
			if (Z_TYPE_P(value) == IS_INDIRECT) {
				value = Z_INDIRECT_P(value);
				if (UNEXPECTED(Z_ISUNDEF_P(value))) {
					continue;
				}

				ZEND_ASSERT(key);
				if (zend_check_property_access(zobj, key) == FAILURE) {
					continue;
				}
				unmangle = 1;
			}

			if (Z_ISREF_P(value) && Z_REFCOUNT_P(value) == 1) {
				value = Z_REFVAL_P(value);
			}
			Z_TRY_ADDREF_P(value);

			if (UNEXPECTED(!key)) {
				/* This case is only possible due to loopholes, e.g. ArrayObject */
				zend_hash_index_add(Z_ARRVAL_P(return_value), num_key, value);
			} else if (unmangle && ZSTR_VAL(key)[0] == 0) {
				const char *prop_name, *class_name;
				size_t prop_len;
				zend_unmangle_property_name_ex(key, &class_name, &prop_name, &prop_len);
				/* We assume here that a mangled property name is never
				 * numeric. This is probably a safe assumption, but
				 * theoretically someone might write an extension with
				 * private, numeric properties. Well, too bad.
				 */
				zend_hash_str_add_new(Z_ARRVAL_P(return_value), prop_name, prop_len, value);
			} else {
				zend_symtable_add_new(Z_ARRVAL_P(return_value), key, value);
			}
		} ZEND_HASH_FOREACH_END();
	}
}
/* }}} */

static int same_name(zend_string *key, zend_string *name) /* {{{ */
{
	zend_string *lcname;
	int ret;

	if (key == name) {
		return 1;
	}
	if (ZSTR_LEN(key) != ZSTR_LEN(name)) {
		return 0;
	}
	lcname = zend_string_tolower(name);
	ret = memcmp(ZSTR_VAL(lcname), ZSTR_VAL(key), ZSTR_LEN(key)) == 0;
	zend_string_release_ex(lcname, 0);
	return ret;
}
/* }}} */

/* {{{ proto array get_class_methods(mixed class)
   Returns an array of method names for class or class instance. */
ZEND_FUNCTION(get_class_methods)
{
	zval *klass;
	zval method_name;
	zend_class_entry *ce = NULL;
	zend_class_entry *scope;
	zend_function *mptr;
	zend_string *key;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &klass) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(klass) == IS_OBJECT) {
		ce = Z_OBJCE_P(klass);
	} else if (Z_TYPE_P(klass) == IS_STRING) {
	    ce = zend_lookup_class(Z_STR_P(klass));
	}

	if (!ce) {
		RETURN_NULL();
	}

	array_init(return_value);
	scope = zend_get_executed_scope();

	ZEND_HASH_FOREACH_STR_KEY_PTR(&ce->function_table, key, mptr) {

		if ((mptr->common.fn_flags & ZEND_ACC_PUBLIC)
		 || (scope &&
		     (((mptr->common.fn_flags & ZEND_ACC_PROTECTED) &&
		       zend_check_protected(mptr->common.scope, scope))
		   || ((mptr->common.fn_flags & ZEND_ACC_PRIVATE) &&
		       scope == mptr->common.scope)))) {
			size_t len = ZSTR_LEN(mptr->common.function_name);

			/* Do not display old-style inherited constructors */
			if (!key) {
				ZVAL_STR_COPY(&method_name, mptr->common.function_name);
				zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &method_name);
			} else if ((mptr->common.fn_flags & ZEND_ACC_CTOR) == 0 ||
			    mptr->common.scope == ce ||
			    zend_binary_strcasecmp(ZSTR_VAL(key), ZSTR_LEN(key), ZSTR_VAL(mptr->common.function_name), len) == 0) {

				if (mptr->type == ZEND_USER_FUNCTION &&
				    (!mptr->op_array.refcount || *mptr->op_array.refcount > 1) &&
			    	 !same_name(key, mptr->common.function_name)) {
					ZVAL_STR_COPY(&method_name, zend_find_alias_name(mptr->common.scope, key));
					zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &method_name);
				} else {
					ZVAL_STR_COPY(&method_name, mptr->common.function_name);
					zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &method_name);
				}
			}
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ proto bool method_exists(object object, string method)
   Checks if the class method exists */
ZEND_FUNCTION(method_exists)
{
	zval *klass;
	zend_string *method_name;
	zend_string *lcname;
	zend_class_entry * ce;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(klass)
		Z_PARAM_STR(method_name)
	ZEND_PARSE_PARAMETERS_END();

	if (Z_TYPE_P(klass) == IS_OBJECT) {
		ce = Z_OBJCE_P(klass);
	} else if (Z_TYPE_P(klass) == IS_STRING) {
		if ((ce = zend_lookup_class(Z_STR_P(klass))) == NULL) {
			RETURN_FALSE;
		}
	} else {
		RETURN_FALSE;
	}

	lcname = zend_string_tolower(method_name);
	if (zend_hash_exists(&ce->function_table, lcname)) {
		zend_string_release_ex(lcname, 0);
		RETURN_TRUE;
	} else if (Z_TYPE_P(klass) == IS_OBJECT && Z_OBJ_HT_P(klass)->get_method != NULL) {
		zend_object *obj = Z_OBJ_P(klass);
		zend_function *func = Z_OBJ_HT_P(klass)->get_method(&obj, method_name, NULL);
		if (func != NULL) {
			if (func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) {
				/* Returns true to the fake Closure's __invoke */
				RETVAL_BOOL(func->common.scope == zend_ce_closure
					&& zend_string_equals_literal(method_name, ZEND_INVOKE_FUNC_NAME));

				zend_string_release_ex(lcname, 0);
				zend_string_release_ex(func->common.function_name, 0);
				zend_free_trampoline(func);
				return;
			}
			zend_string_release_ex(lcname, 0);
			RETURN_TRUE;
		}
	}
	zend_string_release_ex(lcname, 0);
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool property_exists(mixed object_or_class, string property_name)
   Checks if the object or class has a property */
ZEND_FUNCTION(property_exists)
{
	zval *object;
	zend_string *property;
	zend_class_entry *ce;
	zend_property_info *property_info;
	zval property_z;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zS", &object, &property) == FAILURE) {
		return;
	}

	if (property == NULL) {
		RETURN_FALSE;
	}

	if (Z_TYPE_P(object) == IS_STRING) {
		ce = zend_lookup_class(Z_STR_P(object));
		if (!ce) {
			RETURN_FALSE;
		}
	} else if (Z_TYPE_P(object) == IS_OBJECT) {
		ce = Z_OBJCE_P(object);
	} else {
		zend_error(E_WARNING, "First parameter must either be an object or the name of an existing class");
		RETURN_NULL();
	}

	if ((property_info = zend_hash_find_ptr(&ce->properties_info, property)) != NULL
		&& (property_info->flags & ZEND_ACC_SHADOW) == 0) {
		RETURN_TRUE;
	}

	ZVAL_STR(&property_z, property);

	if (Z_TYPE_P(object) ==  IS_OBJECT &&
		Z_OBJ_HANDLER_P(object, has_property) &&
		Z_OBJ_HANDLER_P(object, has_property)(object, &property_z, 2, NULL)) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool class_exists(string classname [, bool autoload])
   Checks if the class exists */
ZEND_FUNCTION(class_exists)
{
	zend_string *class_name;
	zend_string *lc_name;
	zend_class_entry *ce;
	zend_bool autoload = 1;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(class_name)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(autoload)
	ZEND_PARSE_PARAMETERS_END();

	if (!autoload) {
		if (ZSTR_VAL(class_name)[0] == '\\') {
			/* Ignore leading "\" */
			lc_name = zend_string_alloc(ZSTR_LEN(class_name) - 1, 0);
			zend_str_tolower_copy(ZSTR_VAL(lc_name), ZSTR_VAL(class_name) + 1, ZSTR_LEN(class_name) - 1);
		} else {
			lc_name = zend_string_tolower(class_name);
		}

		ce = zend_hash_find_ptr(EG(class_table), lc_name);
		zend_string_release_ex(lc_name, 0);
	} else {
		ce = zend_lookup_class(class_name);
	}

 	if (ce) {
 		RETURN_BOOL((ce->ce_flags & (ZEND_ACC_INTERFACE | ZEND_ACC_TRAIT)) == 0);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool interface_exists(string classname [, bool autoload])
   Checks if the class exists */
ZEND_FUNCTION(interface_exists)
{
	zend_string *iface_name, *lc_name;
	zend_class_entry *ce;
	zend_bool autoload = 1;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(iface_name)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(autoload)
	ZEND_PARSE_PARAMETERS_END();

	if (!autoload) {
		if (ZSTR_VAL(iface_name)[0] == '\\') {
			/* Ignore leading "\" */
			lc_name = zend_string_alloc(ZSTR_LEN(iface_name) - 1, 0);
			zend_str_tolower_copy(ZSTR_VAL(lc_name), ZSTR_VAL(iface_name) + 1, ZSTR_LEN(iface_name) - 1);
		} else {
			lc_name = zend_string_tolower(iface_name);
		}
		ce = zend_hash_find_ptr(EG(class_table), lc_name);
		zend_string_release_ex(lc_name, 0);
		RETURN_BOOL(ce && ce->ce_flags & ZEND_ACC_INTERFACE);
	}

	ce = zend_lookup_class(iface_name);
	if (ce) {
 		RETURN_BOOL((ce->ce_flags & ZEND_ACC_INTERFACE) > 0);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool trait_exists(string traitname [, bool autoload])
 Checks if the trait exists */
ZEND_FUNCTION(trait_exists)
{
	zend_string *trait_name, *lc_name;
	zend_class_entry *ce;
	zend_bool autoload = 1;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(trait_name)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(autoload)
	ZEND_PARSE_PARAMETERS_END();

	if (!autoload) {
		if (ZSTR_VAL(trait_name)[0] == '\\') {
			/* Ignore leading "\" */
			lc_name = zend_string_alloc(ZSTR_LEN(trait_name) - 1, 0);
			zend_str_tolower_copy(ZSTR_VAL(lc_name), ZSTR_VAL(trait_name) + 1, ZSTR_LEN(trait_name) - 1);
		} else {
			lc_name = zend_string_tolower(trait_name);
		}

		ce = zend_hash_find_ptr(EG(class_table), lc_name);
		zend_string_release_ex(lc_name, 0);
	} else {
		ce = zend_lookup_class(trait_name);
	}

	if (ce) {
 		RETURN_BOOL((ce->ce_flags & ZEND_ACC_TRAIT) != 0);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool function_exists(string function_name)
   Checks if the function exists */
ZEND_FUNCTION(function_exists)
{
	zend_string *name;
	zend_function *func;
	zend_string *lcname;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	if (ZSTR_VAL(name)[0] == '\\') {
		/* Ignore leading "\" */
		lcname = zend_string_alloc(ZSTR_LEN(name) - 1, 0);
		zend_str_tolower_copy(ZSTR_VAL(lcname), ZSTR_VAL(name) + 1, ZSTR_LEN(name) - 1);
	} else {
		lcname = zend_string_tolower(name);
	}

	func = zend_hash_find_ptr(EG(function_table), lcname);
	zend_string_release_ex(lcname, 0);

	/*
	 * A bit of a hack, but not a bad one: we see if the handler of the function
	 * is actually one that displays "function is disabled" message.
	 */
	RETURN_BOOL(func && (func->type != ZEND_INTERNAL_FUNCTION ||
		func->internal_function.handler != zif_display_disabled_function));
}
/* }}} */

/* {{{ proto bool class_alias(string user_class_name , string alias_name [, bool autoload])
   Creates an alias for user defined class */
ZEND_FUNCTION(class_alias)
{
	zend_string *class_name;
	char *alias_name;
	zend_class_entry *ce;
	size_t alias_name_len;
	zend_bool autoload = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ss|b", &class_name, &alias_name, &alias_name_len, &autoload) == FAILURE) {
		return;
	}

	ce = zend_lookup_class_ex(class_name, NULL, autoload);

	if (ce) {
		if (ce->type == ZEND_USER_CLASS) {
			if (zend_register_class_alias_ex(alias_name, alias_name_len, ce, 0) == SUCCESS) {
				RETURN_TRUE;
			} else {
				zend_error(E_WARNING, "Cannot declare %s %s, because the name is already in use", zend_get_object_type(ce), alias_name);
				RETURN_FALSE;
			}
		} else {
			zend_error(E_WARNING, "First argument of class_alias() must be a name of user defined class");
			RETURN_FALSE;
		}
	} else {
		zend_error(E_WARNING, "Class '%s' not found", ZSTR_VAL(class_name));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto array get_included_files(void)
   Returns an array with the file names that were include_once()'d */
ZEND_FUNCTION(get_included_files)
{
	zend_string *entry;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	array_init(return_value);
	ZEND_HASH_FOREACH_STR_KEY(&EG(included_files), entry) {
		if (entry) {
			add_next_index_str(return_value, zend_string_copy(entry));
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ proto void trigger_error(string message [, int error_type])
   Generates a user-level error/warning/notice message */
ZEND_FUNCTION(trigger_error)
{
	zend_long error_type = E_USER_NOTICE;
	char *message;
	size_t message_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|l", &message, &message_len, &error_type) == FAILURE) {
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

/* {{{ proto string set_error_handler(callable error_handler [, int error_types])
   Sets a user-defined error handler function.  Returns the previously defined error handler, or false on error */
ZEND_FUNCTION(set_error_handler)
{
	zval *error_handler;
	zend_long error_type = E_ALL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|l", &error_handler, &error_type) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(error_handler) != IS_NULL) { /* NULL == unset */
		if (!zend_is_callable(error_handler, 0, NULL)) {
			zend_string *error_handler_name = zend_get_callable_name(error_handler);
			zend_error(E_WARNING, "%s() expects the argument (%s) to be a valid callback",
					   get_active_function_name(), error_handler_name?ZSTR_VAL(error_handler_name):"unknown");
			zend_string_release_ex(error_handler_name, 0);
			return;
		}
	}

	if (Z_TYPE(EG(user_error_handler)) != IS_UNDEF) {
		ZVAL_COPY(return_value, &EG(user_error_handler));

		zend_stack_push(&EG(user_error_handlers_error_reporting), &EG(user_error_handler_error_reporting));
		zend_stack_push(&EG(user_error_handlers), &EG(user_error_handler));
	}

	if (Z_TYPE_P(error_handler) == IS_NULL) { /* unset user-defined handler */
		ZVAL_UNDEF(&EG(user_error_handler));
		return;
	}

	ZVAL_COPY(&EG(user_error_handler), error_handler);
	EG(user_error_handler_error_reporting) = (int)error_type;
}
/* }}} */

/* {{{ proto void restore_error_handler(void)
   Restores the previously defined error handler function */
ZEND_FUNCTION(restore_error_handler)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (Z_TYPE(EG(user_error_handler)) != IS_UNDEF) {
		zval zeh;

		ZVAL_COPY_VALUE(&zeh, &EG(user_error_handler));
		ZVAL_UNDEF(&EG(user_error_handler));
		zval_ptr_dtor(&zeh);
	}

	if (zend_stack_is_empty(&EG(user_error_handlers))) {
		ZVAL_UNDEF(&EG(user_error_handler));
	} else {
		zval *tmp;
		EG(user_error_handler_error_reporting) = zend_stack_int_top(&EG(user_error_handlers_error_reporting));
		zend_stack_del_top(&EG(user_error_handlers_error_reporting));
		tmp = zend_stack_top(&EG(user_error_handlers));
		ZVAL_COPY_VALUE(&EG(user_error_handler), tmp);
		zend_stack_del_top(&EG(user_error_handlers));
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto mixed set_exception_handler(callable exception_handler)
   Sets a user-defined exception handler function. Returns the previously defined exception handler, or false on error */
ZEND_FUNCTION(set_exception_handler)
{
	zval *exception_handler;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &exception_handler) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(exception_handler) != IS_NULL) { /* NULL == unset */
		if (!zend_is_callable(exception_handler, 0, NULL)) {
		zend_string *exception_handler_name = zend_get_callable_name(exception_handler);
			zend_error(E_WARNING, "%s() expects the argument (%s) to be a valid callback",
					   get_active_function_name(), exception_handler_name?ZSTR_VAL(exception_handler_name):"unknown");
			zend_string_release_ex(exception_handler_name, 0);
			return;
		}
	}

	if (Z_TYPE(EG(user_exception_handler)) != IS_UNDEF) {
		ZVAL_COPY(return_value, &EG(user_exception_handler));

		zend_stack_push(&EG(user_exception_handlers), &EG(user_exception_handler));
	}

	if (Z_TYPE_P(exception_handler) == IS_NULL) { /* unset user-defined handler */
		ZVAL_UNDEF(&EG(user_exception_handler));
		return;
	}

	ZVAL_COPY(&EG(user_exception_handler), exception_handler);
}
/* }}} */

/* {{{ proto void restore_exception_handler(void)
   Restores the previously defined exception handler function */
ZEND_FUNCTION(restore_exception_handler)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (Z_TYPE(EG(user_exception_handler)) != IS_UNDEF) {
		zval_ptr_dtor(&EG(user_exception_handler));
	}
	if (zend_stack_is_empty(&EG(user_exception_handlers))) {
		ZVAL_UNDEF(&EG(user_exception_handler));
	} else {
		zval *tmp = zend_stack_top(&EG(user_exception_handlers));
		ZVAL_COPY_VALUE(&EG(user_exception_handler), tmp);
		zend_stack_del_top(&EG(user_exception_handlers));
	}
	RETURN_TRUE;
}
/* }}} */

static int copy_class_or_interface_name(zval *el, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */
{
	zend_class_entry *ce = (zend_class_entry *)Z_PTR_P(el);
	zval *array = va_arg(args, zval *);
	uint32_t mask = va_arg(args, uint32_t);
	uint32_t comply = va_arg(args, uint32_t);
	uint32_t comply_mask = (comply)? mask:0;

	if ((hash_key->key && ZSTR_VAL(hash_key->key)[0] != 0)
		&& (comply_mask == (ce->ce_flags & mask))) {
		if (ce->refcount > 1 &&
		    !same_name(hash_key->key, ce->name)) {
			add_next_index_str(array, zend_string_copy(hash_key->key));
		} else {
			add_next_index_str(array, zend_string_copy(ce->name));
		}
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

/* {{{ proto array get_declared_traits()
   Returns an array of all declared traits. */
ZEND_FUNCTION(get_declared_traits)
{
	uint32_t mask = ZEND_ACC_TRAIT;
	uint32_t comply = 1;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	array_init(return_value);
	zend_hash_apply_with_arguments(EG(class_table), copy_class_or_interface_name, 3, return_value, mask, comply);
}
/* }}} */

/* {{{ proto array get_declared_classes()
   Returns an array of all declared classes. */
ZEND_FUNCTION(get_declared_classes)
{
	uint32_t mask = ZEND_ACC_INTERFACE | ZEND_ACC_TRAIT;
	uint32_t comply = 0;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	array_init(return_value);
	zend_hash_apply_with_arguments(EG(class_table), copy_class_or_interface_name, 3, return_value, mask, comply);
}
/* }}} */

/* {{{ proto array get_declared_interfaces()
   Returns an array of all declared interfaces. */
ZEND_FUNCTION(get_declared_interfaces)
{
	uint32_t mask = ZEND_ACC_INTERFACE;
	uint32_t comply = 1;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	array_init(return_value);
	zend_hash_apply_with_arguments(EG(class_table), copy_class_or_interface_name, 3, return_value, mask, comply);
}
/* }}} */

static int copy_function_name(zval *zv, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */
{
	zend_function *func = Z_PTR_P(zv);
	zval *internal_ar = va_arg(args, zval *),
	     *user_ar     = va_arg(args, zval *);
	zend_bool *exclude_disabled = va_arg(args, zend_bool *);

	if (hash_key->key == NULL || ZSTR_VAL(hash_key->key)[0] == 0) {
		return 0;
	}

	if (func->type == ZEND_INTERNAL_FUNCTION) {
		char *disable_functions = INI_STR("disable_functions");

		if ((*exclude_disabled == 1) && (disable_functions != NULL)) {
			if (strstr(disable_functions, func->common.function_name->val) == NULL) {
				add_next_index_str(internal_ar, zend_string_copy(hash_key->key));
			}
		} else {
			add_next_index_str(internal_ar, zend_string_copy(hash_key->key));
		}
	} else if (func->type == ZEND_USER_FUNCTION) {
		add_next_index_str(user_ar, zend_string_copy(hash_key->key));
	}

	return 0;
}
/* }}} */

/* {{{ proto array get_defined_functions(void)
   Returns an array of all defined functions */
ZEND_FUNCTION(get_defined_functions)
{
	zval internal, user;
	zend_bool exclude_disabled = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &exclude_disabled) == FAILURE) {
		return;
	}

	array_init(&internal);
	array_init(&user);
	array_init(return_value);

	zend_hash_apply_with_arguments(EG(function_table), copy_function_name, 3, &internal, &user, &exclude_disabled);

	zend_hash_str_add_new(Z_ARRVAL_P(return_value), "internal", sizeof("internal")-1, &internal);
	zend_hash_str_add_new(Z_ARRVAL_P(return_value), "user", sizeof("user")-1, &user);
}
/* }}} */

/* {{{ proto array get_defined_vars(void)
   Returns an associative array of names and values of all currently defined variable names (variables in the current scope) */
ZEND_FUNCTION(get_defined_vars)
{
	zend_array *symbol_table;
	if (zend_forbid_dynamic_call("get_defined_vars()") == FAILURE) {
		return;
	}

	symbol_table = zend_rebuild_symbol_table();
	if (UNEXPECTED(symbol_table == NULL)) {
		return;
	}

	RETURN_ARR(zend_array_dup(symbol_table));
}
/* }}} */

#define LAMBDA_TEMP_FUNCNAME	"__lambda_func"
/* {{{ proto string create_function(string args, string code)
   Creates an anonymous function, and returns its name (funny, eh?) */
ZEND_FUNCTION(create_function)
{
    zend_string *function_name;
	char *eval_code, *function_args, *function_code;
	size_t eval_code_length, function_args_len, function_code_len;
	int retval;
	char *eval_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss", &function_args, &function_args_len, &function_code, &function_code_len) == FAILURE) {
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

	eval_name = zend_make_compiled_string_description("runtime-created function");
	retval = zend_eval_stringl(eval_code, eval_code_length, NULL, eval_name);
	efree(eval_code);
	efree(eval_name);

	if (retval==SUCCESS) {
		zend_op_array *func;
		HashTable *static_variables;

		func = zend_hash_str_find_ptr(EG(function_table), LAMBDA_TEMP_FUNCNAME, sizeof(LAMBDA_TEMP_FUNCNAME)-1);
		if (!func) {
			zend_error_noreturn(E_CORE_ERROR, "Unexpected inconsistency in create_function()");
			RETURN_FALSE;
		}
		if (func->refcount) {
			(*func->refcount)++;
		}
		static_variables = func->static_variables;
		func->static_variables = NULL;
		zend_hash_str_del(EG(function_table), LAMBDA_TEMP_FUNCNAME, sizeof(LAMBDA_TEMP_FUNCNAME)-1);
		func->static_variables = static_variables;

		function_name = zend_string_alloc(sizeof("0lambda_")+MAX_LENGTH_OF_LONG, 0);
		ZSTR_VAL(function_name)[0] = '\0';

		do {
			ZSTR_LEN(function_name) = snprintf(ZSTR_VAL(function_name) + 1, sizeof("lambda_")+MAX_LENGTH_OF_LONG, "lambda_%d", ++EG(lambda_count)) + 1;
		} while (zend_hash_add_ptr(EG(function_table), function_name, func) == NULL);
		RETURN_NEW_STR(function_name);
	} else {
		zend_hash_str_del(EG(function_table), LAMBDA_TEMP_FUNCNAME, sizeof(LAMBDA_TEMP_FUNCNAME)-1);
		RETURN_FALSE;
	}
}
/* }}} */

#if ZEND_DEBUG && defined(ZTS)
ZEND_FUNCTION(zend_thread_id)
{
	RETURN_LONG((zend_long)tsrm_thread_id());
}
#endif

/* {{{ proto string get_resource_type(resource res)
   Get the resource type name for a given resource */
ZEND_FUNCTION(get_resource_type)
{
	const char *resource_type;
	zval *z_resource_type;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &z_resource_type) == FAILURE) {
		return;
	}

	resource_type = zend_rsrc_list_get_rsrc_type(Z_RES_P(z_resource_type));
	if (resource_type) {
		RETURN_STRING(resource_type);
	} else {
		RETURN_STRING("Unknown");
	}
}
/* }}} */

/* {{{ proto array get_resources([string resouce_type])
   Get an array with all active resources */
ZEND_FUNCTION(get_resources)
{
	zend_string *type = NULL;
	zend_string *key;
	zend_ulong index;
	zval *val;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S", &type) == FAILURE) {
		return;
	}

	if (!type) {
		array_init(return_value);
		ZEND_HASH_FOREACH_KEY_VAL(&EG(regular_list), index, key, val) {
			if (!key) {
				Z_ADDREF_P(val);
				zend_hash_index_add_new(Z_ARRVAL_P(return_value), index, val);
			}
		} ZEND_HASH_FOREACH_END();
	} else if (zend_string_equals_literal(type, "Unknown")) {
		array_init(return_value);
		ZEND_HASH_FOREACH_KEY_VAL(&EG(regular_list), index, key, val) {
			if (!key && Z_RES_TYPE_P(val) <= 0) {
				Z_ADDREF_P(val);
				zend_hash_index_add_new(Z_ARRVAL_P(return_value), index, val);
			}
		} ZEND_HASH_FOREACH_END();
	} else {
		int id = zend_fetch_list_dtor_id(ZSTR_VAL(type));

		if (id <= 0) {
			zend_error(E_WARNING, "get_resources():  Unknown resource type '%s'", ZSTR_VAL(type));
			RETURN_FALSE;
		}

		array_init(return_value);
		ZEND_HASH_FOREACH_KEY_VAL(&EG(regular_list), index, key, val) {
			if (!key && Z_RES_TYPE_P(val) == id) {
				Z_ADDREF_P(val);
				zend_hash_index_add_new(Z_ARRVAL_P(return_value), index, val);
			}
		} ZEND_HASH_FOREACH_END();
	}
}
/* }}} */

static int add_extension_info(zval *item, void *arg) /* {{{ */
{
	zval *name_array = (zval *)arg;
	zend_module_entry *module = (zend_module_entry*)Z_PTR_P(item);
	add_next_index_string(name_array, module->name);
	return 0;
}
/* }}} */

static int add_zendext_info(zend_extension *ext, void *arg) /* {{{ */
{
	zval *name_array = (zval *)arg;
	add_next_index_string(name_array, ext->name);
	return 0;
}
/* }}} */

static int add_constant_info(zval *item, void *arg) /* {{{ */
{
	zval *name_array = (zval *)arg;
	zend_constant *constant = (zend_constant*)Z_PTR_P(item);
	zval const_val;

	if (!constant->name) {
		/* skip special constants */
		return 0;
	}

	ZVAL_COPY_OR_DUP(&const_val, &constant->value);
	zend_hash_add_new(Z_ARRVAL_P(name_array), constant->name, &const_val);
	return 0;
}
/* }}} */

/* {{{ proto array get_loaded_extensions([bool zend_extensions])
   Return an array containing names of loaded extensions */
ZEND_FUNCTION(get_loaded_extensions)
{
	zend_bool zendext = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &zendext) == FAILURE) {
		return;
	}

	array_init(return_value);

	if (zendext) {
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t)add_zendext_info, return_value);
	} else {
		zend_hash_apply_with_argument(&module_registry, add_extension_info, return_value);
	}
}
/* }}} */

/* {{{ proto array get_defined_constants([bool categorize])
   Return an array containing the names and values of all defined constants */
ZEND_FUNCTION(get_defined_constants)
{
	zend_bool categorize = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &categorize) == FAILURE) {
		return;
	}

	array_init(return_value);

	if (categorize) {
		zend_constant *val;
		int module_number;
		zval *modules, const_val;
		char **module_names;
		zend_module_entry *module;
		int i = 1;

		modules = ecalloc(zend_hash_num_elements(&module_registry) + 2, sizeof(zval));
		module_names = emalloc((zend_hash_num_elements(&module_registry) + 2) * sizeof(char *));

		module_names[0] = "internal";
		ZEND_HASH_FOREACH_PTR(&module_registry, module) {
			module_names[module->module_number] = (char *)module->name;
			i++;
		} ZEND_HASH_FOREACH_END();
		module_names[i] = "user";

		ZEND_HASH_FOREACH_PTR(EG(zend_constants), val) {
			if (!val->name) {
				/* skip special constants */
				continue;
			}

			if (ZEND_CONSTANT_MODULE_NUMBER(val) == PHP_USER_CONSTANT) {
				module_number = i;
			} else if (ZEND_CONSTANT_MODULE_NUMBER(val) > i) {
				/* should not happen */
				continue;
			} else {
				module_number = ZEND_CONSTANT_MODULE_NUMBER(val);
			}

			if (Z_TYPE(modules[module_number]) == IS_UNDEF) {
				array_init(&modules[module_number]);
				add_assoc_zval(return_value, module_names[module_number], &modules[module_number]);
			}

			ZVAL_COPY_OR_DUP(&const_val, &val->value);
			zend_hash_add_new(Z_ARRVAL(modules[module_number]), val->name, &const_val);
		} ZEND_HASH_FOREACH_END();

		efree(module_names);
		efree(modules);
	} else {
		zend_hash_apply_with_argument(EG(zend_constants), add_constant_info, return_value);
	}
}
/* }}} */

static void debug_backtrace_get_args(zend_execute_data *call, zval *arg_array) /* {{{ */
{
	uint32_t num_args = ZEND_CALL_NUM_ARGS(call);

	if (num_args) {
		uint32_t i = 0;
		zval *p = ZEND_CALL_ARG(call, 1);

		array_init_size(arg_array, num_args);
		zend_hash_real_init_packed(Z_ARRVAL_P(arg_array));
		ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(arg_array)) {
			if (call->func->type == ZEND_USER_FUNCTION) {
				uint32_t first_extra_arg = MIN(num_args, call->func->op_array.num_args);

				if (UNEXPECTED(ZEND_CALL_INFO(call) & ZEND_CALL_HAS_SYMBOL_TABLE)) {
					/* In case of attached symbol_table, values on stack may be invalid
					 * and we have to access them through symbol_table
					 * See: https://bugs.php.net/bug.php?id=73156
					 */
					zend_string *arg_name;
					zval *arg;

					while (i < first_extra_arg) {
						arg_name = call->func->op_array.vars[i];
						arg = zend_hash_find_ex_ind(call->symbol_table, arg_name, 1);
						if (arg) {
							if (Z_OPT_REFCOUNTED_P(arg)) {
								Z_ADDREF_P(arg);
							}
							ZEND_HASH_FILL_ADD(arg);
						} else {
							ZEND_HASH_FILL_ADD(&EG(uninitialized_zval));
						}
						i++;
					}
				} else {
					while (i < first_extra_arg) {
						if (EXPECTED(Z_TYPE_INFO_P(p) != IS_UNDEF)) {
							if (Z_OPT_REFCOUNTED_P(p)) {
								Z_ADDREF_P(p);
							}
							ZEND_HASH_FILL_ADD(p);
						} else {
							ZEND_HASH_FILL_ADD(&EG(uninitialized_zval));
						}
						p++;
						i++;
					}
				}
				p = ZEND_CALL_VAR_NUM(call, call->func->op_array.last_var + call->func->op_array.T);
			}

			while (i < num_args) {
				if (EXPECTED(Z_TYPE_INFO_P(p) != IS_UNDEF)) {
					if (Z_OPT_REFCOUNTED_P(p)) {
						Z_ADDREF_P(p);
					}
					ZEND_HASH_FILL_ADD(p);
				} else {
					ZEND_HASH_FILL_ADD(&EG(uninitialized_zval));
				}
				p++;
				i++;
			}
		} ZEND_HASH_FILL_END();
		Z_ARRVAL_P(arg_array)->nNumOfElements = num_args;
	} else {
		ZVAL_EMPTY_ARRAY(arg_array);
	}
}
/* }}} */

void debug_print_backtrace_args(zval *arg_array) /* {{{ */
{
	zval *tmp;
	int i = 0;

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(arg_array), tmp) {
		if (i++) {
			ZEND_PUTS(", ");
		}
		zend_print_flat_zval_r(tmp);
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ proto void debug_print_backtrace([int options[, int limit]]) */
ZEND_FUNCTION(debug_print_backtrace)
{
	zend_execute_data *call, *ptr, *skip;
	zend_object *object;
	int lineno, frameno = 0;
	zend_function *func;
	const char *function_name;
	const char *filename;
	zend_string *class_name = NULL;
	char *call_type;
	const char *include_filename = NULL;
	zval arg_array;
	int indent = 0;
	zend_long options = 0;
	zend_long limit = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|ll", &options, &limit) == FAILURE) {
		return;
	}

	ZVAL_UNDEF(&arg_array);
	ptr = EX(prev_execute_data);

	/* skip debug_backtrace() */
	call = ptr;
	ptr = ptr->prev_execute_data;

	while (ptr && (limit == 0 || frameno < limit)) {
		frameno++;
		class_name = NULL;
		call_type = NULL;
		ZVAL_UNDEF(&arg_array);

		ptr = zend_generator_check_placeholder_frame(ptr);

		skip = ptr;
		/* skip internal handler */
		if ((!skip->func || !ZEND_USER_CODE(skip->func->common.type)) &&
		    skip->prev_execute_data &&
		    skip->prev_execute_data->func &&
		    ZEND_USER_CODE(skip->prev_execute_data->func->common.type) &&
		    skip->prev_execute_data->opline->opcode != ZEND_DO_FCALL &&
		    skip->prev_execute_data->opline->opcode != ZEND_DO_ICALL &&
		    skip->prev_execute_data->opline->opcode != ZEND_DO_UCALL &&
		    skip->prev_execute_data->opline->opcode != ZEND_DO_FCALL_BY_NAME &&
		    skip->prev_execute_data->opline->opcode != ZEND_INCLUDE_OR_EVAL) {
			skip = skip->prev_execute_data;
		}

		if (skip->func && ZEND_USER_CODE(skip->func->common.type)) {
			filename = ZSTR_VAL(skip->func->op_array.filename);
			if (skip->opline->opcode == ZEND_HANDLE_EXCEPTION) {
				if (EG(opline_before_exception)) {
					lineno = EG(opline_before_exception)->lineno;
				} else {
					lineno = skip->func->op_array.line_end;
				}
			} else {
				lineno = skip->opline->lineno;
			}
		} else {
			filename = NULL;
			lineno = 0;
		}

		/* $this may be passed into regular internal functions */
		object = (Z_TYPE(call->This) == IS_OBJECT) ? Z_OBJ(call->This) : NULL;

		if (call->func) {
			zend_string *zend_function_name;

			func = call->func;
            if (func->common.scope && func->common.scope->trait_aliases) {
                zend_function_name = zend_resolve_method_name(object ? object->ce : func->common.scope, func);
            } else {
                zend_function_name = func->common.function_name;
            }
            if (zend_function_name != NULL) {
                function_name = ZSTR_VAL(zend_function_name);
            } else {
                function_name = NULL;
            }
		} else {
			func = NULL;
			function_name = NULL;
		}

		if (function_name) {
			if (object) {
				if (func->common.scope) {
					class_name = func->common.scope->name;
				} else if (object->handlers->get_class_name == zend_std_get_class_name) {
					class_name = object->ce->name;
				} else {
					class_name = object->handlers->get_class_name(object);
				}

				call_type = "->";
			} else if (func->common.scope) {
				class_name = func->common.scope->name;
				call_type = "::";
			} else {
				class_name = NULL;
				call_type = NULL;
			}
			if (func->type != ZEND_EVAL_CODE) {
				if ((options & DEBUG_BACKTRACE_IGNORE_ARGS) == 0) {
					debug_backtrace_get_args(call, &arg_array);
				}
			}
		} else {
			/* i know this is kinda ugly, but i'm trying to avoid extra cycles in the main execution loop */
			zend_bool build_filename_arg = 1;

			if (!ptr->func || !ZEND_USER_CODE(ptr->func->common.type) || ptr->opline->opcode != ZEND_INCLUDE_OR_EVAL) {
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
				array_init(&arg_array);
				add_next_index_string(&arg_array, (char*)include_filename);
			}
			call_type = NULL;
		}
		zend_printf("#%-2d ", indent);
		if (class_name) {
			ZEND_PUTS(ZSTR_VAL(class_name));
			ZEND_PUTS(call_type);
			if (object
			  && !func->common.scope
			  && object->handlers->get_class_name != zend_std_get_class_name) {
				zend_string_release_ex(class_name, 0);
			}
		}
		zend_printf("%s(", function_name);
		if (Z_TYPE(arg_array) != IS_UNDEF) {
			debug_print_backtrace_args(&arg_array);
			zval_ptr_dtor(&arg_array);
		}
		if (filename) {
			zend_printf(") called at [%s:%d]\n", filename, lineno);
		} else {
			zend_execute_data *prev_call = skip;
			zend_execute_data *prev = skip->prev_execute_data;

			while (prev) {
				if (prev_call &&
				    prev_call->func &&
					!ZEND_USER_CODE(prev_call->func->common.type)) {
					prev = NULL;
					break;
				}
				if (prev->func && ZEND_USER_CODE(prev->func->common.type)) {
					zend_printf(") called at [%s:%d]\n", ZSTR_VAL(prev->func->op_array.filename), prev->opline->lineno);
					break;
				}
				prev_call = prev;
				prev = prev->prev_execute_data;
			}
			if (!prev) {
				ZEND_PUTS(")\n");
			}
		}
		include_filename = filename;
		call = skip;
		ptr = skip->prev_execute_data;
		++indent;
	}
}

/* }}} */

ZEND_API void zend_fetch_debug_backtrace(zval *return_value, int skip_last, int options, int limit) /* {{{ */
{
	zend_execute_data *ptr, *skip, *call = NULL;
	zend_object *object;
	int lineno, frameno = 0;
	zend_function *func;
	zend_string *function_name;
	zend_string *filename;
	zend_string *include_filename = NULL;
	zval stack_frame, tmp;

	array_init(return_value);

	if (!(ptr = EG(current_execute_data))) {
		return;
	}

	if (!ptr->func || !ZEND_USER_CODE(ptr->func->common.type)) {
		call = ptr;
		ptr = ptr->prev_execute_data;
	}

	if (ptr) {
		if (skip_last) {
			/* skip debug_backtrace() */
			call = ptr;
			ptr = ptr->prev_execute_data;
		} else {
			/* skip "new Exception()" */
			if (ptr->func && ZEND_USER_CODE(ptr->func->common.type) && (ptr->opline->opcode == ZEND_NEW)) {
				call = ptr;
				ptr = ptr->prev_execute_data;
			}
		}
		if (!call) {
			call = ptr;
			ptr = ptr->prev_execute_data;
		}
	}

	while (ptr && (limit == 0 || frameno < limit)) {
		frameno++;
		array_init(&stack_frame);

		ptr = zend_generator_check_placeholder_frame(ptr);

		skip = ptr;
		/* skip internal handler */
		if ((!skip->func || !ZEND_USER_CODE(skip->func->common.type)) &&
		    skip->prev_execute_data &&
		    skip->prev_execute_data->func &&
		    ZEND_USER_CODE(skip->prev_execute_data->func->common.type) &&
		    skip->prev_execute_data->opline->opcode != ZEND_DO_FCALL &&
		    skip->prev_execute_data->opline->opcode != ZEND_DO_ICALL &&
		    skip->prev_execute_data->opline->opcode != ZEND_DO_UCALL &&
		    skip->prev_execute_data->opline->opcode != ZEND_DO_FCALL_BY_NAME &&
		    skip->prev_execute_data->opline->opcode != ZEND_INCLUDE_OR_EVAL) {
			skip = skip->prev_execute_data;
		}

		if (skip->func && ZEND_USER_CODE(skip->func->common.type)) {
			filename = skip->func->op_array.filename;
			if (skip->opline->opcode == ZEND_HANDLE_EXCEPTION) {
				if (EG(opline_before_exception)) {
					lineno = EG(opline_before_exception)->lineno;
				} else {
					lineno = skip->func->op_array.line_end;
				}
			} else {
				lineno = skip->opline->lineno;
			}
			ZVAL_STR_COPY(&tmp, filename);
			zend_hash_add_new(Z_ARRVAL(stack_frame), ZSTR_KNOWN(ZEND_STR_FILE), &tmp);
			ZVAL_LONG(&tmp, lineno);
			zend_hash_add_new(Z_ARRVAL(stack_frame), ZSTR_KNOWN(ZEND_STR_LINE), &tmp);

			/* try to fetch args only if an FCALL was just made - elsewise we're in the middle of a function
			 * and debug_baktrace() might have been called by the error_handler. in this case we don't
			 * want to pop anything of the argument-stack */
		} else {
			zend_execute_data *prev_call = skip;
			zend_execute_data *prev = skip->prev_execute_data;

			while (prev) {
				if (prev_call &&
				    prev_call->func &&
					!ZEND_USER_CODE(prev_call->func->common.type) &&
					!(prev_call->func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE)) {
					break;
				}
				if (prev->func && ZEND_USER_CODE(prev->func->common.type)) {
					ZVAL_STR_COPY(&tmp, prev->func->op_array.filename);
					zend_hash_add_new(Z_ARRVAL(stack_frame), ZSTR_KNOWN(ZEND_STR_FILE), &tmp);
					ZVAL_LONG(&tmp, prev->opline->lineno);
					zend_hash_add_new(Z_ARRVAL(stack_frame), ZSTR_KNOWN(ZEND_STR_LINE), &tmp);
					break;
				}
				prev_call = prev;
				prev = prev->prev_execute_data;
			}
			filename = NULL;
		}

		/* $this may be passed into regular internal functions */
		object = (call && (Z_TYPE(call->This) == IS_OBJECT)) ? Z_OBJ(call->This) : NULL;

		if (call && call->func) {
			func = call->func;
			function_name = (func->common.scope &&
			                 func->common.scope->trait_aliases) ?
				zend_resolve_method_name(
					(object ? object->ce : func->common.scope), func) :
				func->common.function_name;
		} else {
			func = NULL;
			function_name = NULL;
		}

		if (function_name) {
			ZVAL_STR_COPY(&tmp, function_name);
			zend_hash_add_new(Z_ARRVAL(stack_frame), ZSTR_KNOWN(ZEND_STR_FUNCTION), &tmp);

			if (object) {
				if (func->common.scope) {
					ZVAL_STR_COPY(&tmp, func->common.scope->name);
				} else if (object->handlers->get_class_name == zend_std_get_class_name) {
					ZVAL_STR_COPY(&tmp, object->ce->name);
				} else {
					ZVAL_STR(&tmp, object->handlers->get_class_name(object));
				}
				zend_hash_add_new(Z_ARRVAL(stack_frame), ZSTR_KNOWN(ZEND_STR_CLASS), &tmp);
				if ((options & DEBUG_BACKTRACE_PROVIDE_OBJECT) != 0) {
					ZVAL_OBJ(&tmp, object);
					zend_hash_add_new(Z_ARRVAL(stack_frame), ZSTR_KNOWN(ZEND_STR_OBJECT), &tmp);
					Z_ADDREF(tmp);
				}

				ZVAL_INTERNED_STR(&tmp, ZSTR_KNOWN(ZEND_STR_OBJECT_OPERATOR));
				zend_hash_add_new(Z_ARRVAL(stack_frame), ZSTR_KNOWN(ZEND_STR_TYPE), &tmp);
			} else if (func->common.scope) {
				ZVAL_STR_COPY(&tmp, func->common.scope->name);
				zend_hash_add_new(Z_ARRVAL(stack_frame), ZSTR_KNOWN(ZEND_STR_CLASS), &tmp);
				ZVAL_INTERNED_STR(&tmp, ZSTR_KNOWN(ZEND_STR_PAAMAYIM_NEKUDOTAYIM));
				zend_hash_add_new(Z_ARRVAL(stack_frame), ZSTR_KNOWN(ZEND_STR_TYPE), &tmp);
			}

			if ((options & DEBUG_BACKTRACE_IGNORE_ARGS) == 0 &&
				func->type != ZEND_EVAL_CODE) {

				debug_backtrace_get_args(call, &tmp);
				zend_hash_add_new(Z_ARRVAL(stack_frame), ZSTR_KNOWN(ZEND_STR_ARGS), &tmp);
			}
		} else {
			/* i know this is kinda ugly, but i'm trying to avoid extra cycles in the main execution loop */
			zend_bool build_filename_arg = 1;
			zend_string *pseudo_function_name;

			if (!ptr->func || !ZEND_USER_CODE(ptr->func->common.type) || ptr->opline->opcode != ZEND_INCLUDE_OR_EVAL) {
				/* can happen when calling eval from a custom sapi */
				pseudo_function_name = ZSTR_KNOWN(ZEND_STR_UNKNOWN);
				build_filename_arg = 0;
			} else
			switch (ptr->opline->extended_value) {
				case ZEND_EVAL:
					pseudo_function_name = ZSTR_KNOWN(ZEND_STR_EVAL);
					build_filename_arg = 0;
					break;
				case ZEND_INCLUDE:
					pseudo_function_name = ZSTR_KNOWN(ZEND_STR_INCLUDE);
					break;
				case ZEND_REQUIRE:
					pseudo_function_name = ZSTR_KNOWN(ZEND_STR_REQUIRE);
					break;
				case ZEND_INCLUDE_ONCE:
					pseudo_function_name = ZSTR_KNOWN(ZEND_STR_INCLUDE_ONCE);
					break;
				case ZEND_REQUIRE_ONCE:
					pseudo_function_name = ZSTR_KNOWN(ZEND_STR_REQUIRE_ONCE);
					break;
				default:
					/* this can actually happen if you use debug_backtrace() in your error_handler and
					 * you're in the top-scope */
					pseudo_function_name = ZSTR_KNOWN(ZEND_STR_UNKNOWN);
					build_filename_arg = 0;
					break;
			}

			if (build_filename_arg && include_filename) {
				zval arg_array;

				array_init(&arg_array);

				/* include_filename always points to the last filename of the last last called-function.
				   if we have called include in the frame above - this is the file we have included.
				 */

				ZVAL_STR_COPY(&tmp, include_filename);
				zend_hash_next_index_insert_new(Z_ARRVAL(arg_array), &tmp);
				zend_hash_add_new(Z_ARRVAL(stack_frame), ZSTR_KNOWN(ZEND_STR_ARGS), &arg_array);
			}

			ZVAL_INTERNED_STR(&tmp, pseudo_function_name);
			zend_hash_add_new(Z_ARRVAL(stack_frame), ZSTR_KNOWN(ZEND_STR_FUNCTION), &tmp);
		}

		zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &stack_frame);

		include_filename = filename;

		call = skip;
		ptr = skip->prev_execute_data;
	}
}
/* }}} */

/* {{{ proto array debug_backtrace([int options[, int limit]])
   Return backtrace as array */
ZEND_FUNCTION(debug_backtrace)
{
	zend_long options = DEBUG_BACKTRACE_PROVIDE_OBJECT;
	zend_long limit = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|ll", &options, &limit) == FAILURE) {
		return;
	}

	zend_fetch_debug_backtrace(return_value, 1, options, limit);
}
/* }}} */

/* {{{ proto bool extension_loaded(string extension_name)
   Returns true if the named extension is loaded */
ZEND_FUNCTION(extension_loaded)
{
	zend_string *extension_name;
	zend_string *lcname;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &extension_name) == FAILURE) {
		return;
	}

	lcname = zend_string_tolower(extension_name);
	if (zend_hash_exists(&module_registry, lcname)) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}
	zend_string_release_ex(lcname, 0);
}
/* }}} */

/* {{{ proto array get_extension_funcs(string extension_name)
   Returns an array with the names of functions belonging to the named extension */
ZEND_FUNCTION(get_extension_funcs)
{
	zend_string *extension_name;
	zend_string *lcname;
	int array;
	zend_module_entry *module;
	zend_function *zif;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &extension_name) == FAILURE) {
		return;
	}
	if (strncasecmp(ZSTR_VAL(extension_name), "zend", sizeof("zend"))) {
		lcname = zend_string_tolower(extension_name);
		module = zend_hash_find_ptr(&module_registry, lcname);
		zend_string_release_ex(lcname, 0);
	} else {
		module = zend_hash_str_find_ptr(&module_registry, "core", sizeof("core") - 1);
	}

	if (!module) {
		RETURN_FALSE;
	}

	if (module->functions) {
		/* avoid BC break, if functions list is empty, will return an empty array */
		array_init(return_value);
		array = 1;
	} else {
		array = 0;
	}

	ZEND_HASH_FOREACH_PTR(CG(function_table), zif) {
		if (zif->common.type == ZEND_INTERNAL_FUNCTION
			&& zif->internal_function.module == module) {
			if (!array) {
				array_init(return_value);
				array = 1;
			}
			add_next_index_str(return_value, zend_string_copy(zif->common.function_name));
		}
	} ZEND_HASH_FOREACH_END();

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
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
