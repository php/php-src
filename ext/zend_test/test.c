/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://www.php.net/license/3_01.txt                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/php_var.h"
#include "php_test.h"
#include "test_arginfo.h"
#include "zend_attributes.h"
#include "zend_observer.h"
#include "zend_smart_str.h"
#include "zend_fibers.h"

ZEND_BEGIN_MODULE_GLOBALS(zend_test)
	int observer_enabled;
	int observer_show_output;
	int observer_observe_all;
	int observer_observe_includes;
	int observer_observe_functions;
	int observer_show_return_type;
	int observer_show_return_value;
	int observer_show_init_backtrace;
	int observer_show_opcode;
	char *observer_show_opcode_in_user_handler;
	int observer_nesting_depth;
	int observer_fiber_switch;
	int replace_zend_execute_ex;
ZEND_END_MODULE_GLOBALS(zend_test)

ZEND_DECLARE_MODULE_GLOBALS(zend_test)

#define ZT_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(zend_test, v)

static zend_class_entry *zend_test_interface;
static zend_class_entry *zend_test_class;
static zend_class_entry *zend_test_child_class;
static zend_class_entry *zend_test_trait;
static zend_class_entry *zend_test_attribute;
static zend_class_entry *zend_test_ns_foo_class;
static zend_class_entry *zend_test_ns2_foo_class;
static zend_class_entry *zend_test_ns2_ns_foo_class;
static zend_object_handlers zend_test_class_handlers;

static ZEND_FUNCTION(zend_test_func)
{
	RETVAL_STR_COPY(EX(func)->common.function_name);

	/* Cleanup trampoline */
	ZEND_ASSERT(EX(func)->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE);
	zend_string_release(EX(func)->common.function_name);
	zend_free_trampoline(EX(func));
	EX(func) = NULL;
}

static ZEND_FUNCTION(zend_test_array_return)
{
	ZEND_PARSE_PARAMETERS_NONE();
}

static ZEND_FUNCTION(zend_test_nullable_array_return)
{
	ZEND_PARSE_PARAMETERS_NONE();
}

static ZEND_FUNCTION(zend_test_void_return)
{
	/* dummy */
	ZEND_PARSE_PARAMETERS_NONE();
}

static ZEND_FUNCTION(zend_test_deprecated)
{
	zval *arg1;

	zend_parse_parameters(ZEND_NUM_ARGS(), "|z", &arg1);
}

/* Create a string without terminating null byte. Must be terminated with
 * zend_terminate_string() before destruction, otherwise a warning is issued
 * in debug builds. */
static ZEND_FUNCTION(zend_create_unterminated_string)
{
	zend_string *str, *res;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &str) == FAILURE) {
		RETURN_THROWS();
	}

	res = zend_string_alloc(ZSTR_LEN(str), 0);
	memcpy(ZSTR_VAL(res), ZSTR_VAL(str), ZSTR_LEN(str));
	/* No trailing null byte */

	RETURN_STR(res);
}

/* Enforce terminate null byte on string. This avoids a warning in debug builds. */
static ZEND_FUNCTION(zend_terminate_string)
{
	zend_string *str;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &str) == FAILURE) {
		RETURN_THROWS();
	}

	ZSTR_VAL(str)[ZSTR_LEN(str)] = '\0';
}

/* {{{ Cause an intentional memory leak, for testing/debugging purposes */
static ZEND_FUNCTION(zend_leak_bytes)
{
	zend_long leakbytes = 3;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &leakbytes) == FAILURE) {
		RETURN_THROWS();
	}

	emalloc(leakbytes);
}
/* }}} */

/* {{{ Leak a refcounted variable */
static ZEND_FUNCTION(zend_leak_variable)
{
	zval *zv;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &zv) == FAILURE) {
		RETURN_THROWS();
	}

	if (!Z_REFCOUNTED_P(zv)) {
		zend_error(E_WARNING, "Cannot leak variable that is not refcounted");
		return;
	}

	Z_ADDREF_P(zv);
}
/* }}} */

/* Tests Z_PARAM_OBJ_OR_STR */
static ZEND_FUNCTION(zend_string_or_object)
{
	zend_string *str;
	zend_object *object;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJ_OR_STR(object, str)
	ZEND_PARSE_PARAMETERS_END();

	if (str) {
		RETURN_STR_COPY(str);
	} else {
		RETURN_OBJ_COPY(object);
	}
}
/* }}} */

/* Tests Z_PARAM_OBJ_OR_STR_OR_NULL */
static ZEND_FUNCTION(zend_string_or_object_or_null)
{
	zend_string *str;
	zend_object *object;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJ_OR_STR_OR_NULL(object, str)
	ZEND_PARSE_PARAMETERS_END();

	if (str) {
		RETURN_STR_COPY(str);
	} else if (object) {
		RETURN_OBJ_COPY(object);
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/* Tests Z_PARAM_OBJ_OF_CLASS_OR_STR */
static ZEND_FUNCTION(zend_string_or_stdclass)
{
	zend_string *str;
	zend_object *object;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(object, zend_standard_class_def, str)
	ZEND_PARSE_PARAMETERS_END();

	if (str) {
		RETURN_STR_COPY(str);
	} else {
		RETURN_OBJ_COPY(object);
	}
}
/* }}} */

/* Tests Z_PARAM_OBJ_OF_CLASS_OR_STR_OR_NULL */
static ZEND_FUNCTION(zend_string_or_stdclass_or_null)
{
	zend_string *str;
	zend_object *object;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJ_OF_CLASS_OR_STR_OR_NULL(object, zend_standard_class_def, str)
	ZEND_PARSE_PARAMETERS_END();

	if (str) {
		RETURN_STR_COPY(str);
	} else if (object) {
		RETURN_OBJ_COPY(object);
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/* TESTS Z_PARAM_ITERABLE and Z_PARAM_ITERABLE_OR_NULL */
static ZEND_FUNCTION(zend_iterable)
{
	zval *arg1, *arg2;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ITERABLE(arg1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ITERABLE_OR_NULL(arg2)
	ZEND_PARSE_PARAMETERS_END();
}

static ZEND_FUNCTION(namespaced_func)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_TRUE;
}

static zend_object *zend_test_class_new(zend_class_entry *class_type) /* {{{ */ {
	zend_object *obj = zend_objects_new(class_type);
	object_properties_init(obj, class_type);
	obj->handlers = &zend_test_class_handlers;
	return obj;
}
/* }}} */

static zend_function *zend_test_class_method_get(zend_object **object, zend_string *name, const zval *key) /* {{{ */ {
	zend_internal_function *fptr;

	if (EXPECTED(EG(trampoline).common.function_name == NULL)) {
		fptr = (zend_internal_function *) &EG(trampoline);
	} else {
		fptr = emalloc(sizeof(zend_internal_function));
	}
	memset(fptr, 0, sizeof(zend_internal_function));
	fptr->type = ZEND_INTERNAL_FUNCTION;
	fptr->num_args = 1;
	fptr->scope = (*object)->ce;
	fptr->fn_flags = ZEND_ACC_CALL_VIA_HANDLER;
	fptr->function_name = zend_string_copy(name);
	fptr->handler = ZEND_FN(zend_test_func);

	return (zend_function*)fptr;
}
/* }}} */

static zend_function *zend_test_class_static_method_get(zend_class_entry *ce, zend_string *name) /* {{{ */ {
	if (zend_string_equals_literal_ci(name, "test")) {
		zend_internal_function *fptr;

		if (EXPECTED(EG(trampoline).common.function_name == NULL)) {
			fptr = (zend_internal_function *) &EG(trampoline);
		} else {
			fptr = emalloc(sizeof(zend_internal_function));
		}
		memset(fptr, 0, sizeof(zend_internal_function));
		fptr->type = ZEND_INTERNAL_FUNCTION;
		fptr->num_args = 1;
		fptr->scope = ce;
		fptr->fn_flags = ZEND_ACC_CALL_VIA_HANDLER|ZEND_ACC_STATIC;
		fptr->function_name = zend_string_copy(name);
		fptr->handler = ZEND_FN(zend_test_func);

		return (zend_function*)fptr;
	}
	return zend_std_get_static_method(ce, name, NULL);
}
/* }}} */

void zend_attribute_validate_zendtestattribute(zend_attribute *attr, uint32_t target, zend_class_entry *scope)
{
	if (target != ZEND_ATTRIBUTE_TARGET_CLASS) {
		zend_error(E_COMPILE_ERROR, "Only classes can be marked with #[ZendTestAttribute]");
	}
}

static ZEND_METHOD(_ZendTestClass, __toString) {
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_EMPTY_STRING();
}

/* Internal function returns bool, we return int. */
static ZEND_METHOD(_ZendTestClass, is_object) {
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_LONG(42);
}

static ZEND_METHOD(_ZendTestClass, returnsStatic) {
	ZEND_PARSE_PARAMETERS_NONE();
	object_init_ex(return_value, zend_get_called_scope(execute_data));
}

static ZEND_METHOD(_ZendTestTrait, testMethod) {
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_TRUE;
}

static ZEND_METHOD(ZendTestNS_Foo, method) {
	ZEND_PARSE_PARAMETERS_NONE();
}

static ZEND_METHOD(ZendTestNS2_Foo, method) {
	ZEND_PARSE_PARAMETERS_NONE();
}

static ZEND_METHOD(ZendTestNS2_ZendSubNS_Foo, method) {
	ZEND_PARSE_PARAMETERS_NONE();
}

PHP_INI_BEGIN()
	STD_PHP_INI_BOOLEAN("zend_test.observer.enabled", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_enabled, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.show_output", "1", PHP_INI_SYSTEM, OnUpdateBool, observer_show_output, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.observe_all", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_observe_all, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.observe_includes", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_observe_includes, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.observe_functions", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_observe_functions, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.show_return_type", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_show_return_type, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.show_return_value", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_show_return_value, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.show_init_backtrace", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_show_init_backtrace, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.show_opcode", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_show_opcode, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_ENTRY("zend_test.observer.show_opcode_in_user_handler", "", PHP_INI_SYSTEM, OnUpdateString, observer_show_opcode_in_user_handler, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.fiber_switch", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_fiber_switch, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.replace_zend_execute_ex", "0", PHP_INI_SYSTEM, OnUpdateBool, replace_zend_execute_ex, zend_zend_test_globals, zend_test_globals)
PHP_INI_END()

static zend_observer_fcall_handlers observer_fcall_init(zend_execute_data *execute_data);

void (*old_zend_execute_ex)(zend_execute_data *execute_data);
static void custom_zend_execute_ex(zend_execute_data *execute_data)
{
	old_zend_execute_ex(execute_data);
}

static int observer_show_opcode_in_user_handler(zend_execute_data *execute_data)
{
	if (ZT_G(observer_show_output)) {
		php_printf("%*s<!-- opcode: '%s' in user handler -->\n", 2 * ZT_G(observer_nesting_depth), "", zend_get_opcode_name(EX(opline)->opcode));
	}

	return ZEND_USER_OPCODE_DISPATCH;
}

static void observer_set_user_opcode_handler(const char *opcode_names, user_opcode_handler_t handler)
{
	const char *s = NULL, *e = opcode_names;

	while (1) {
		if (*e == ' ' || *e == ',' || *e == '\0') {
			if (s) {
				zend_uchar opcode = zend_get_opcode_id(s, e - s);
				if (opcode <= ZEND_VM_LAST_OPCODE) {
					zend_set_user_opcode_handler(opcode, handler);
				} else {
					zend_error(E_WARNING, "Invalid opcode name %.*s", (int) (e - s), e);
				}
				s = NULL;
			}
		} else {
			if (!s) {
				s = e;
			}
		}
		if (*e == '\0') {
			break;
		}
		e++;
	}
}

static void fiber_address_observer(zend_fiber *from, zend_fiber *to)
{
	if (ZT_G(observer_fiber_switch)) {
		php_printf("<!-- switching from fiber %p to %p -->\n", from, to);
	}
}

static void fiber_enter_observer(zend_fiber *from, zend_fiber *to)
{
	if (ZT_G(observer_fiber_switch)) {
		if (to) {
			if (to->status == ZEND_FIBER_STATUS_INIT) {
				php_printf("<init '%p'>\n", to);
			} else if (to->status == ZEND_FIBER_STATUS_RUNNING && (!from || from->status == ZEND_FIBER_STATUS_RUNNING)) {
				php_printf("<resume '%p'>\n", to);
			} else if (to->status == ZEND_FIBER_STATUS_SHUTDOWN) {
				php_printf("<destroying '%p'>\n", to);
			}
		}
	}
}

static void fiber_suspend_observer(zend_fiber *from, zend_fiber *to)
{
	if (ZT_G(observer_fiber_switch)) {
		if (from) {
			if (from->status == ZEND_FIBER_STATUS_SUSPENDED) {
				php_printf("<suspend '%p'>\n", from);
			} else if (from->status == ZEND_FIBER_STATUS_RETURNED) {
				php_printf("<returned '%p'>\n", from);
			} else if (from->status == ZEND_FIBER_STATUS_THREW) {
				php_printf("<threw '%p'>\n", from);
			} else if (from->status == ZEND_FIBER_STATUS_SHUTDOWN) {
				php_printf("<destroyed '%p'>\n", from);
			}
		}
	}
}

PHP_MINIT_FUNCTION(zend_test)
{
	zend_test_interface = register_class__ZendTestInterface();
	zend_declare_class_constant_long(zend_test_interface, ZEND_STRL("DUMMY"), 0);

	zend_test_class = register_class__ZendTestClass(zend_test_interface);
	zend_test_class->create_object = zend_test_class_new;
	zend_test_class->get_static_method = zend_test_class_static_method_get;

	zend_test_child_class = register_class__ZendTestChildClass(zend_test_class);

	memcpy(&zend_test_class_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	zend_test_class_handlers.get_method = zend_test_class_method_get;

	zend_test_trait = register_class__ZendTestTrait();

	REGISTER_LONG_CONSTANT("ZEND_TEST_DEPRECATED", 42, CONST_PERSISTENT | CONST_DEPRECATED);

	zend_test_attribute = register_class_ZendTestAttribute();
	{
		zend_internal_attribute *attr = zend_internal_attribute_register(zend_test_attribute, ZEND_ATTRIBUTE_TARGET_ALL);
		attr->validator = zend_attribute_validate_zendtestattribute;
	}

	zend_test_ns_foo_class = register_class_ZendTestNS_Foo();
	zend_test_ns2_foo_class = register_class_ZendTestNS2_Foo();
	zend_test_ns2_ns_foo_class = register_class_ZendTestNS2_ZendSubNS_Foo();

	// Loading via dl() not supported with the observer API
	if (type != MODULE_TEMPORARY) {
		REGISTER_INI_ENTRIES();
		if (ZT_G(observer_enabled)) {
			zend_observer_fcall_register(observer_fcall_init);
		}
	} else {
		(void)ini_entries;
	}

	if (ZT_G(replace_zend_execute_ex)) {
		old_zend_execute_ex = zend_execute_ex;
		zend_execute_ex = custom_zend_execute_ex;
	}

	if (ZT_G(observer_enabled) && ZT_G(observer_show_opcode_in_user_handler)) {
		observer_set_user_opcode_handler(ZT_G(observer_show_opcode_in_user_handler), observer_show_opcode_in_user_handler);
	}

	if (ZT_G(observer_enabled)) {
		zend_observer_fiber_switch_register(fiber_address_observer);
		zend_observer_fiber_switch_register(fiber_enter_observer);
		zend_observer_fiber_switch_register(fiber_suspend_observer);
	}

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(zend_test)
{
	if (type != MODULE_TEMPORARY) {
		UNREGISTER_INI_ENTRIES();
	}

	return SUCCESS;
}

static void observer_show_opcode(zend_execute_data *execute_data)
{
	if (!ZT_G(observer_show_opcode)) {
		return;
	}
	php_printf("%*s<!-- opcode: '%s' -->\n", 2 * ZT_G(observer_nesting_depth), "", zend_get_opcode_name(EX(opline)->opcode));
}

static void observer_begin(zend_execute_data *execute_data)
{
	if (!ZT_G(observer_show_output)) {
		return;
	}

	if (execute_data->func && execute_data->func->common.function_name) {
		if (execute_data->func->common.scope) {
			php_printf("%*s<%s::%s>\n", 2 * ZT_G(observer_nesting_depth), "", ZSTR_VAL(execute_data->func->common.scope->name), ZSTR_VAL(execute_data->func->common.function_name));
		} else {
			php_printf("%*s<%s>\n", 2 * ZT_G(observer_nesting_depth), "", ZSTR_VAL(execute_data->func->common.function_name));
		}
	} else {
		php_printf("%*s<file '%s'>\n", 2 * ZT_G(observer_nesting_depth), "", ZSTR_VAL(execute_data->func->op_array.filename));
	}
	ZT_G(observer_nesting_depth)++;
	observer_show_opcode(execute_data);
}

static void get_retval_info(zval *retval, smart_str *buf)
{
	if (!ZT_G(observer_show_return_type) && !ZT_G(observer_show_return_value)) {
		return;
	}

	smart_str_appendc(buf, ':');
	if (retval == NULL) {
		smart_str_appendl(buf, "NULL", 4);
	} else if (ZT_G(observer_show_return_value)) {
		if (Z_TYPE_P(retval) == IS_OBJECT) {
			smart_str_appendl(buf, "object(", 7);
			smart_str_append(buf, Z_OBJCE_P(retval)->name);
			smart_str_appendl(buf, ")#", 2);
			smart_str_append_long(buf, Z_OBJ_HANDLE_P(retval));
		} else {
			php_var_export_ex(retval, 2 * ZT_G(observer_nesting_depth) + 3, buf);
		}
	} else if (ZT_G(observer_show_return_type)) {
		smart_str_appends(buf, zend_zval_type_name(retval));
	}
	smart_str_0(buf);
}

static void observer_end(zend_execute_data *execute_data, zval *retval)
{
	if (!ZT_G(observer_show_output)) {
		return;
	}

	if (EG(exception)) {
		php_printf("%*s<!-- Exception: %s -->\n", 2 * ZT_G(observer_nesting_depth), "", ZSTR_VAL(EG(exception)->ce->name));
	}
	observer_show_opcode(execute_data);
	ZT_G(observer_nesting_depth)--;
	if (execute_data->func && execute_data->func->common.function_name) {
		smart_str retval_info = {0};
		get_retval_info(retval, &retval_info);
		if (execute_data->func->common.scope) {
			php_printf("%*s</%s::%s%s>\n", 2 * ZT_G(observer_nesting_depth), "", ZSTR_VAL(execute_data->func->common.scope->name), ZSTR_VAL(execute_data->func->common.function_name), retval_info.s ? ZSTR_VAL(retval_info.s) : "");
		} else {
			php_printf("%*s</%s%s>\n", 2 * ZT_G(observer_nesting_depth), "", ZSTR_VAL(execute_data->func->common.function_name), retval_info.s ? ZSTR_VAL(retval_info.s) : "");
		}
		smart_str_free(&retval_info);
	} else {
		php_printf("%*s</file '%s'>\n", 2 * ZT_G(observer_nesting_depth), "", ZSTR_VAL(execute_data->func->op_array.filename));
	}
}

static void observer_show_init(zend_function *fbc)
{
	if (fbc->common.function_name) {
		if (fbc->common.scope) {
			php_printf("%*s<!-- init %s::%s() -->\n", 2 * ZT_G(observer_nesting_depth), "", ZSTR_VAL(fbc->common.scope->name), ZSTR_VAL(fbc->common.function_name));
		} else {
			php_printf("%*s<!-- init %s() -->\n", 2 * ZT_G(observer_nesting_depth), "", ZSTR_VAL(fbc->common.function_name));
		}
	} else {
		php_printf("%*s<!-- init '%s' -->\n", 2 * ZT_G(observer_nesting_depth), "", ZSTR_VAL(fbc->op_array.filename));
	}
}

static void observer_show_init_backtrace(zend_execute_data *execute_data)
{
	zend_execute_data *ex = execute_data;
	php_printf("%*s<!--\n", 2 * ZT_G(observer_nesting_depth), "");
	do {
		zend_function *fbc = ex->func;
		int indent = 2 * ZT_G(observer_nesting_depth) + 4;
		if (fbc->common.function_name) {
			if (fbc->common.scope) {
				php_printf("%*s%s::%s()\n", indent, "", ZSTR_VAL(fbc->common.scope->name), ZSTR_VAL(fbc->common.function_name));
			} else {
				php_printf("%*s%s()\n", indent, "", ZSTR_VAL(fbc->common.function_name));
			}
		} else {
			php_printf("%*s{main} %s\n", indent, "", ZSTR_VAL(fbc->op_array.filename));
		}
	} while ((ex = ex->prev_execute_data) != NULL);
	php_printf("%*s-->\n", 2 * ZT_G(observer_nesting_depth), "");
}

static zend_observer_fcall_handlers observer_fcall_init(zend_execute_data *execute_data)
{
	zend_function *fbc = execute_data->func;
	if (ZT_G(observer_show_output)) {
		observer_show_init(fbc);
		if (ZT_G(observer_show_init_backtrace)) {
			observer_show_init_backtrace(execute_data);
		}
		observer_show_opcode(execute_data);
	}

	if (ZT_G(observer_observe_all)) {
		return (zend_observer_fcall_handlers){observer_begin, observer_end};
	} else if (ZT_G(observer_observe_includes) && !fbc->common.function_name) {
		return (zend_observer_fcall_handlers){observer_begin, observer_end};
	} else if (ZT_G(observer_observe_functions) && fbc->common.function_name) {
		return (zend_observer_fcall_handlers){observer_begin, observer_end};
	}
	return (zend_observer_fcall_handlers){NULL, NULL};
}

PHP_RINIT_FUNCTION(zend_test)
{
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(zend_test)
{
	return SUCCESS;
}

static PHP_GINIT_FUNCTION(zend_test)
{
#if defined(COMPILE_DL_ZEND_TEST) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	memset(zend_test_globals, 0, sizeof(*zend_test_globals));
}

PHP_MINFO_FUNCTION(zend_test)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "zend_test extension", "enabled");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}

zend_module_entry zend_test_module_entry = {
	STANDARD_MODULE_HEADER,
	"zend_test",
	ext_functions,
	PHP_MINIT(zend_test),
	PHP_MSHUTDOWN(zend_test),
	PHP_RINIT(zend_test),
	PHP_RSHUTDOWN(zend_test),
	PHP_MINFO(zend_test),
	PHP_ZEND_TEST_VERSION,
	PHP_MODULE_GLOBALS(zend_test),
	PHP_GINIT(zend_test),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_ZEND_TEST
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(zend_test)
#endif

PHP_ZEND_TEST_API struct bug79096 bug79096(void)
{
  struct bug79096 b;

  b.a = 1;
  b.b = 1;
  return b;
}

PHP_ZEND_TEST_API void bug79532(off_t *array, size_t elems)
{
	int i;
	for (i = 0; i < elems; i++) {
		array[i] = i;
	}
}

PHP_ZEND_TEST_API int *(*bug79177_cb)(void);
void bug79177(void)
{
	bug79177_cb();
}

typedef struct bug80847_01 {
	uint64_t b;
	double c;
} bug80847_01;
typedef struct bug80847_02 {
	bug80847_01 a;
} bug80847_02;

PHP_ZEND_TEST_API bug80847_02 ffi_bug80847(bug80847_02 s) {
	s.a.b += 10;
	s.a.c -= 10.0;
    return s;
}
