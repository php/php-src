/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
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

ZEND_BEGIN_MODULE_GLOBALS(zend_test)
	int observer_enabled;
	int observer_show_output;
	int observer_observe_all;
	int observer_observe_includes;
	int observer_observe_functions;
	int observer_show_return_type;
	int observer_show_return_value;
	int observer_nesting_depth;
ZEND_END_MODULE_GLOBALS(zend_test)

ZEND_DECLARE_MODULE_GLOBALS(zend_test)

#define ZT_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(zend_test, v)

static zend_class_entry *zend_test_interface;
static zend_class_entry *zend_test_class;
static zend_class_entry *zend_test_child_class;
static zend_class_entry *zend_test_trait;
static zend_class_entry *zend_test_attribute;
static zend_object_handlers zend_test_class_handlers;

ZEND_FUNCTION(zend_test_func)
{
	RETVAL_STR_COPY(EX(func)->common.function_name);

	/* Cleanup trampoline */
	ZEND_ASSERT(EX(func)->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE);
	zend_string_release(EX(func)->common.function_name);
	zend_free_trampoline(EX(func));
	EX(func) = NULL;
}

ZEND_FUNCTION(zend_test_array_return)
{
	ZEND_PARSE_PARAMETERS_NONE();
}

ZEND_FUNCTION(zend_test_nullable_array_return)
{
	ZEND_PARSE_PARAMETERS_NONE();
}

ZEND_FUNCTION(zend_test_void_return)
{
	/* dummy */
	ZEND_PARSE_PARAMETERS_NONE();
}

ZEND_FUNCTION(zend_test_deprecated)
{
	zval *arg1;

	zend_parse_parameters(ZEND_NUM_ARGS(), "|z", &arg1);
}

/* Create a string without terminating null byte. Must be terminated with
 * zend_terminate_string() before destruction, otherwise a warning is issued
 * in debug builds. */
ZEND_FUNCTION(zend_create_unterminated_string)
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
ZEND_FUNCTION(zend_terminate_string)
{
	zend_string *str;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &str) == FAILURE) {
		RETURN_THROWS();
	}

	ZSTR_VAL(str)[ZSTR_LEN(str)] = '\0';
}

/* {{{ Cause an intentional memory leak, for testing/debugging purposes */
ZEND_FUNCTION(zend_leak_bytes)
{
	zend_long leakbytes = 3;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &leakbytes) == FAILURE) {
		RETURN_THROWS();
	}

	emalloc(leakbytes);
}
/* }}} */

/* {{{ Leak a refcounted variable */
ZEND_FUNCTION(zend_leak_variable)
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
ZEND_FUNCTION(zend_string_or_object)
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
ZEND_FUNCTION(zend_string_or_object_or_null)
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
ZEND_FUNCTION(zend_string_or_stdclass)
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
ZEND_FUNCTION(zend_string_or_stdclass_or_null)
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
ZEND_FUNCTION(zend_iterable)
{
	zval *arg1, *arg2;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ITERABLE(arg1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ITERABLE_OR_NULL(arg2)
	ZEND_PARSE_PARAMETERS_END();
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

ZEND_METHOD(_ZendTestClass, __toString) /* {{{ */ {
	RETURN_EMPTY_STRING();
}
/* }}} */

/* Internal function returns bool, we return int. */
ZEND_METHOD(_ZendTestClass, is_object) /* {{{ */ {
	RETURN_LONG(42);
}
/* }}} */

ZEND_METHOD(_ZendTestTrait, testMethod) /* {{{ */ {
	RETURN_TRUE;
}
/* }}} */

PHP_INI_BEGIN()
	STD_PHP_INI_BOOLEAN("zend_test.observer.enabled", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_enabled, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.show_output", "1", PHP_INI_SYSTEM, OnUpdateBool, observer_show_output, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.observe_all", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_observe_all, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.observe_includes", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_observe_includes, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.observe_functions", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_observe_functions, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.show_return_type", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_show_return_type, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.show_return_value", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_show_return_value, zend_zend_test_globals, zend_test_globals)
PHP_INI_END()

static zend_observer_fcall observer_fcall_init(zend_function *fbc);

PHP_MINIT_FUNCTION(zend_test)
{
	zend_class_entry class_entry;

	INIT_CLASS_ENTRY(class_entry, "_ZendTestInterface", NULL);
	zend_test_interface = zend_register_internal_interface(&class_entry);
	zend_declare_class_constant_long(zend_test_interface, ZEND_STRL("DUMMY"), 0);
	INIT_CLASS_ENTRY(class_entry, "_ZendTestClass", class__ZendTestClass_methods);
	zend_test_class = zend_register_internal_class(&class_entry);
	zend_class_implements(zend_test_class, 1, zend_test_interface);
	zend_test_class->create_object = zend_test_class_new;
	zend_test_class->get_static_method = zend_test_class_static_method_get;

	zend_declare_property_null(zend_test_class, "_StaticProp", sizeof("_StaticProp") - 1, ZEND_ACC_STATIC);

	{
		zend_string *name = zend_string_init("intProp", sizeof("intProp") - 1, 1);
		zval val;
		ZVAL_LONG(&val, 123);
		zend_declare_typed_property(
			zend_test_class, name, &val, ZEND_ACC_PUBLIC, NULL,
			(zend_type) ZEND_TYPE_INIT_CODE(IS_LONG, 0, 0));
		zend_string_release(name);
	}

	{
		zend_string *name = zend_string_init("classProp", sizeof("classProp") - 1, 1);
		zend_string *class_name = zend_string_init("stdClass", sizeof("stdClass") - 1, 1);
		zval val;
		ZVAL_NULL(&val);
		zend_declare_typed_property(
			zend_test_class, name, &val, ZEND_ACC_PUBLIC, NULL,
			(zend_type) ZEND_TYPE_INIT_CLASS(class_name, 1, 0));
		zend_string_release(name);
	}

	{
		zend_string *name = zend_string_init("classUnionProp", sizeof("classUnionProp") - 1, 1);
		zend_string *class_name1 = zend_string_init("stdClass", sizeof("stdClass") - 1, 1);
		zend_string *class_name2 = zend_string_init("Iterator", sizeof("Iterator") - 1, 1);
		zend_type_list *type_list = malloc(ZEND_TYPE_LIST_SIZE(2));
		type_list->num_types = 2;
		type_list->types[0] = (zend_type) ZEND_TYPE_INIT_CLASS(class_name1, 0, 0);
		type_list->types[1] = (zend_type) ZEND_TYPE_INIT_CLASS(class_name2, 0, 0);
		zend_type type = ZEND_TYPE_INIT_PTR(type_list, _ZEND_TYPE_LIST_BIT, 1, 0);
		zval val;
		ZVAL_NULL(&val);
		zend_declare_typed_property(zend_test_class, name, &val, ZEND_ACC_PUBLIC, NULL, type);
		zend_string_release(name);
	}

	{
		zend_string *name = zend_string_init("staticIntProp", sizeof("staticIntProp") - 1, 1);
		zval val;
		ZVAL_LONG(&val, 123);
		zend_declare_typed_property(
			zend_test_class, name, &val, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC, NULL,
			(zend_type) ZEND_TYPE_INIT_CODE(IS_LONG, 0, 0));
		zend_string_release(name);
	}

	INIT_CLASS_ENTRY(class_entry, "_ZendTestChildClass", NULL);
	zend_test_child_class = zend_register_internal_class_ex(&class_entry, zend_test_class);

	memcpy(&zend_test_class_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	zend_test_class_handlers.get_method = zend_test_class_method_get;

	INIT_CLASS_ENTRY(class_entry, "_ZendTestTrait", class__ZendTestTrait_methods);
	zend_test_trait = zend_register_internal_class(&class_entry);
	zend_test_trait->ce_flags |= ZEND_ACC_TRAIT;
	zend_declare_property_null(zend_test_trait, "testProp", sizeof("testProp")-1, ZEND_ACC_PUBLIC);

	zend_register_class_alias("_ZendTestClassAlias", zend_test_class);

	REGISTER_LONG_CONSTANT("ZEND_TEST_DEPRECATED", 42, CONST_PERSISTENT | CONST_DEPRECATED);

	INIT_CLASS_ENTRY(class_entry, "ZendTestAttribute", NULL);
	zend_test_attribute = zend_register_internal_class(&class_entry);
	zend_test_attribute->ce_flags |= ZEND_ACC_FINAL;

	{
		zend_internal_attribute *attr = zend_internal_attribute_register(zend_test_attribute, ZEND_ATTRIBUTE_TARGET_ALL);
		attr->validator = zend_attribute_validate_zendtestattribute;
	}

	// Loading via dl() not supported with the observer API
	if (type != MODULE_TEMPORARY) {
		REGISTER_INI_ENTRIES();
		if (ZT_G(observer_enabled)) {
			zend_observer_fcall_register(observer_fcall_init);
		}
	} else {
		(void)ini_entries;
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
		php_var_export_ex(retval, 2 * ZT_G(observer_nesting_depth) + 3, buf);
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

static zend_observer_fcall observer_fcall_init(zend_function *fbc)
{
	if (ZT_G(observer_show_output)) {
		observer_show_init(fbc);
	}

	if (ZT_G(observer_observe_all)) {
		return (zend_observer_fcall){observer_begin, observer_end};
	} else if (ZT_G(observer_observe_includes) && !fbc->common.function_name) {
		return (zend_observer_fcall){observer_begin, observer_end};
	} else if (ZT_G(observer_observe_functions) && fbc->common.function_name) {
		return (zend_observer_fcall){observer_begin, observer_end};
	}
	return (zend_observer_fcall){NULL, NULL};
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
	php_info_print_table_header(2, "zend-test extension", "enabled");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}

zend_module_entry zend_test_module_entry = {
	STANDARD_MODULE_HEADER,
	"zend-test",
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
