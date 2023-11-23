/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: a57c9d1fc85dbea853f4cc910b9495a7a0c72eb3 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zend_test_array_return, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zend_test_nullable_array_return, 0, 0, IS_ARRAY, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zend_test_void_return, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zend_test_deprecated, 0, 0, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, arg, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zend_create_unterminated_string, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zend_terminate_string, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(1, str, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zend_leak_variable, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, variable, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zend_leak_bytes, 0, 0, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, bytes, IS_LONG, 0, "3")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_zend_string_or_object, 0, 1, MAY_BE_OBJECT|MAY_BE_STRING)
	ZEND_ARG_TYPE_MASK(0, param, MAY_BE_OBJECT|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_zend_string_or_object_or_null, 0, 1, MAY_BE_OBJECT|MAY_BE_STRING|MAY_BE_NULL)
	ZEND_ARG_TYPE_MASK(0, param, MAY_BE_OBJECT|MAY_BE_STRING|MAY_BE_NULL, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_zend_string_or_stdclass, 0, 1, stdClass, MAY_BE_STRING)
	ZEND_ARG_INFO(0, param)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_zend_string_or_stdclass_or_null, 0, 1, stdClass, MAY_BE_STRING|MAY_BE_NULL)
	ZEND_ARG_INFO(0, param)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zend_iterable, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, arg1, IS_ITERABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, arg2, IS_ITERABLE, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zend_weakmap_attach, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, object, IS_OBJECT, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zend_weakmap_remove, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, object, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

#define arginfo_zend_weakmap_dump arginfo_zend_test_array_return

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_zend_get_unit_enum, 0, 0, ZendTestUnitEnum, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zend_test_parameter_with_attribute, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, parameter, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zend_get_current_func_name, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zend_call_method, 0, 2, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, class, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, method, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, arg1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, arg2, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zend_get_map_ptr_last, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zend_test_crash, 0, 0, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, message, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_zend_test_observe_opline_in_zendmm arginfo_zend_test_void_return

#define arginfo_zend_test_unobserve_opline_in_zendmm arginfo_zend_test_void_return

#if defined(HAVE_LIBXML) && !defined(PHP_WIN32)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zend_test_override_libxml_global_state, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zend_test_is_pcre_bundled, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_ZendTestNS2_ZendSubNS_namespaced_func arginfo_zend_test_is_pcre_bundled

#define arginfo_class__ZendTestClass_is_object arginfo_zend_get_map_ptr_last

ZEND_BEGIN_ARG_INFO_EX(arginfo_class__ZendTestClass___toString, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class__ZendTestClass_returnsStatic, 0, 0, IS_STATIC, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class__ZendTestClass_returnsThrowable, 0, 0, Throwable, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class__ZendTestClass_variadicTest, 0, 0, IS_STATIC, 0)
	ZEND_ARG_VARIADIC_OBJ_TYPE_MASK(0, elements, Iterator, MAY_BE_STRING)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class__ZendTestChildClass_returnsThrowable, 0, 0, Exception, 0)
ZEND_END_ARG_INFO()

#define arginfo_class__ZendTestTrait_testMethod arginfo_zend_test_is_pcre_bundled

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZendTestParameterAttribute___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, parameter, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ZendTestClassWithMethodWithParameterAttribute_no_override arginfo_zend_test_parameter_with_attribute

#define arginfo_class_ZendTestClassWithMethodWithParameterAttribute_override arginfo_zend_test_parameter_with_attribute

#define arginfo_class_ZendTestChildClassWithMethodWithParameterAttribute_override arginfo_zend_test_parameter_with_attribute

#define arginfo_class_ZendTestNS_Foo_method arginfo_zend_test_void_return

#define arginfo_class_ZendTestNS2_Foo_method arginfo_zend_test_void_return

#define arginfo_class_ZendTestNS2_ZendSubNS_Foo_method arginfo_zend_test_void_return


static ZEND_FUNCTION(zend_test_array_return);
static ZEND_FUNCTION(zend_test_nullable_array_return);
static ZEND_FUNCTION(zend_test_void_return);
static ZEND_FUNCTION(zend_test_deprecated);
static ZEND_FUNCTION(zend_create_unterminated_string);
static ZEND_FUNCTION(zend_terminate_string);
static ZEND_FUNCTION(zend_leak_variable);
static ZEND_FUNCTION(zend_leak_bytes);
static ZEND_FUNCTION(zend_string_or_object);
static ZEND_FUNCTION(zend_string_or_object_or_null);
static ZEND_FUNCTION(zend_string_or_stdclass);
static ZEND_FUNCTION(zend_string_or_stdclass_or_null);
static ZEND_FUNCTION(zend_iterable);
static ZEND_FUNCTION(zend_weakmap_attach);
static ZEND_FUNCTION(zend_weakmap_remove);
static ZEND_FUNCTION(zend_weakmap_dump);
static ZEND_FUNCTION(zend_get_unit_enum);
static ZEND_FUNCTION(zend_test_parameter_with_attribute);
static ZEND_FUNCTION(zend_get_current_func_name);
static ZEND_FUNCTION(zend_call_method);
static ZEND_FUNCTION(zend_get_map_ptr_last);
static ZEND_FUNCTION(zend_test_crash);
static ZEND_FUNCTION(zend_test_observe_opline_in_zendmm);
static ZEND_FUNCTION(zend_test_unobserve_opline_in_zendmm);
#if defined(HAVE_LIBXML) && !defined(PHP_WIN32)
static ZEND_FUNCTION(zend_test_override_libxml_global_state);
#endif
static ZEND_FUNCTION(zend_test_is_pcre_bundled);
static ZEND_FUNCTION(namespaced_func);
static ZEND_METHOD(_ZendTestClass, is_object);
static ZEND_METHOD(_ZendTestClass, __toString);
static ZEND_METHOD(_ZendTestClass, returnsStatic);
static ZEND_METHOD(_ZendTestClass, returnsThrowable);
static ZEND_METHOD(_ZendTestClass, variadicTest);
static ZEND_METHOD(_ZendTestChildClass, returnsThrowable);
static ZEND_METHOD(_ZendTestTrait, testMethod);
static ZEND_METHOD(ZendTestParameterAttribute, __construct);
static ZEND_METHOD(ZendTestClassWithMethodWithParameterAttribute, no_override);
static ZEND_METHOD(ZendTestClassWithMethodWithParameterAttribute, override);
static ZEND_METHOD(ZendTestChildClassWithMethodWithParameterAttribute, override);
static ZEND_METHOD(ZendTestNS_Foo, method);
static ZEND_METHOD(ZendTestNS2_Foo, method);
static ZEND_METHOD(ZendTestNS2_ZendSubNS_Foo, method);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(zend_test_array_return, arginfo_zend_test_array_return)
	ZEND_FE(zend_test_nullable_array_return, arginfo_zend_test_nullable_array_return)
	ZEND_FE(zend_test_void_return, arginfo_zend_test_void_return)
	ZEND_DEP_FE(zend_test_deprecated, arginfo_zend_test_deprecated)
	ZEND_FE(zend_create_unterminated_string, arginfo_zend_create_unterminated_string)
	ZEND_FE(zend_terminate_string, arginfo_zend_terminate_string)
	ZEND_FE(zend_leak_variable, arginfo_zend_leak_variable)
	ZEND_FE(zend_leak_bytes, arginfo_zend_leak_bytes)
	ZEND_FE(zend_string_or_object, arginfo_zend_string_or_object)
	ZEND_FE(zend_string_or_object_or_null, arginfo_zend_string_or_object_or_null)
	ZEND_FE(zend_string_or_stdclass, arginfo_zend_string_or_stdclass)
	ZEND_FE(zend_string_or_stdclass_or_null, arginfo_zend_string_or_stdclass_or_null)
	ZEND_FE(zend_iterable, arginfo_zend_iterable)
	ZEND_FE(zend_weakmap_attach, arginfo_zend_weakmap_attach)
	ZEND_FE(zend_weakmap_remove, arginfo_zend_weakmap_remove)
	ZEND_FE(zend_weakmap_dump, arginfo_zend_weakmap_dump)
	ZEND_FE(zend_get_unit_enum, arginfo_zend_get_unit_enum)
	ZEND_FE(zend_test_parameter_with_attribute, arginfo_zend_test_parameter_with_attribute)
	ZEND_FE(zend_get_current_func_name, arginfo_zend_get_current_func_name)
	ZEND_FE(zend_call_method, arginfo_zend_call_method)
	ZEND_FE(zend_get_map_ptr_last, arginfo_zend_get_map_ptr_last)
	ZEND_FE(zend_test_crash, arginfo_zend_test_crash)
	ZEND_FE(zend_test_observe_opline_in_zendmm, arginfo_zend_test_observe_opline_in_zendmm)
	ZEND_FE(zend_test_unobserve_opline_in_zendmm, arginfo_zend_test_unobserve_opline_in_zendmm)
#if defined(HAVE_LIBXML) && !defined(PHP_WIN32)
	ZEND_FE(zend_test_override_libxml_global_state, arginfo_zend_test_override_libxml_global_state)
#endif
	ZEND_FE(zend_test_is_pcre_bundled, arginfo_zend_test_is_pcre_bundled)
	ZEND_NS_FE("ZendTestNS2\\ZendSubNS", namespaced_func, arginfo_ZendTestNS2_ZendSubNS_namespaced_func)
	ZEND_FE_END
};


static const zend_function_entry class__ZendTestInterface_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class__ZendTestClass_methods[] = {
	ZEND_ME(_ZendTestClass, is_object, arginfo_class__ZendTestClass_is_object, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(_ZendTestClass, __toString, arginfo_class__ZendTestClass___toString, ZEND_ACC_PUBLIC|ZEND_ACC_DEPRECATED)
	ZEND_ME(_ZendTestClass, returnsStatic, arginfo_class__ZendTestClass_returnsStatic, ZEND_ACC_PUBLIC)
	ZEND_ME(_ZendTestClass, returnsThrowable, arginfo_class__ZendTestClass_returnsThrowable, ZEND_ACC_PUBLIC)
	ZEND_ME(_ZendTestClass, variadicTest, arginfo_class__ZendTestClass_variadicTest, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};


static const zend_function_entry class__ZendTestChildClass_methods[] = {
	ZEND_ME(_ZendTestChildClass, returnsThrowable, arginfo_class__ZendTestChildClass_returnsThrowable, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class__ZendTestTrait_methods[] = {
	ZEND_ME(_ZendTestTrait, testMethod, arginfo_class__ZendTestTrait_testMethod, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_ZendTestAttribute_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_ZendTestParameterAttribute_methods[] = {
	ZEND_ME(ZendTestParameterAttribute, __construct, arginfo_class_ZendTestParameterAttribute___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_ZendTestClassWithMethodWithParameterAttribute_methods[] = {
	ZEND_ME(ZendTestClassWithMethodWithParameterAttribute, no_override, arginfo_class_ZendTestClassWithMethodWithParameterAttribute_no_override, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_ME(ZendTestClassWithMethodWithParameterAttribute, override, arginfo_class_ZendTestClassWithMethodWithParameterAttribute_override, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_ZendTestChildClassWithMethodWithParameterAttribute_methods[] = {
	ZEND_ME(ZendTestChildClassWithMethodWithParameterAttribute, override, arginfo_class_ZendTestChildClassWithMethodWithParameterAttribute_override, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_ZendTestUnitEnum_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_ZendTestStringEnum_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_ZendTestNS_Foo_methods[] = {
	ZEND_ME(ZendTestNS_Foo, method, arginfo_class_ZendTestNS_Foo_method, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_ZendTestNS2_Foo_methods[] = {
	ZEND_ME(ZendTestNS2_Foo, method, arginfo_class_ZendTestNS2_Foo_method, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_ZendTestNS2_ZendSubNS_Foo_methods[] = {
	ZEND_ME(ZendTestNS2_ZendSubNS_Foo, method, arginfo_class_ZendTestNS2_ZendSubNS_Foo_method, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class__ZendTestInterface(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "_ZendTestInterface", class__ZendTestInterface_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}

static zend_class_entry *register_class__ZendTestClass(zend_class_entry *class_entry__ZendTestInterface)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "_ZendTestClass", class__ZendTestClass_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	zend_class_implements(class_entry, 1, class_entry__ZendTestInterface);
	zend_register_class_alias("_ZendTestClassAlias", class_entry);

	zval property__StaticProp_default_value;
	ZVAL_NULL(&property__StaticProp_default_value);
	zend_string *property__StaticProp_name = zend_string_init("_StaticProp", sizeof("_StaticProp") - 1, 1);
	zend_declare_property_ex(class_entry, property__StaticProp_name, &property__StaticProp_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL);
	zend_string_release(property__StaticProp_name);

	zval property_staticIntProp_default_value;
	ZVAL_LONG(&property_staticIntProp_default_value, 123);
	zend_string *property_staticIntProp_name = zend_string_init("staticIntProp", sizeof("staticIntProp") - 1, 1);
	zend_declare_typed_property(class_entry, property_staticIntProp_name, &property_staticIntProp_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_staticIntProp_name);

	zval property_intProp_default_value;
	ZVAL_LONG(&property_intProp_default_value, 123);
	zend_string *property_intProp_name = zend_string_init("intProp", sizeof("intProp") - 1, 1);
	zend_declare_typed_property(class_entry, property_intProp_name, &property_intProp_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_intProp_name);

	zend_string *property_classProp_class_stdClass = zend_string_init("stdClass", sizeof("stdClass")-1, 1);
	zval property_classProp_default_value;
	ZVAL_NULL(&property_classProp_default_value);
	zend_string *property_classProp_name = zend_string_init("classProp", sizeof("classProp") - 1, 1);
	zend_declare_typed_property(class_entry, property_classProp_name, &property_classProp_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_classProp_class_stdClass, 0, MAY_BE_NULL));
	zend_string_release(property_classProp_name);

	zend_string *property_classUnionProp_class_stdClass = zend_string_init("stdClass", sizeof("stdClass") - 1, 1);
	zend_string *property_classUnionProp_class_Iterator = zend_string_init("Iterator", sizeof("Iterator") - 1, 1);
	zend_type_list *property_classUnionProp_type_list = malloc(ZEND_TYPE_LIST_SIZE(2));
	property_classUnionProp_type_list->num_types = 2;
	property_classUnionProp_type_list->types[0] = (zend_type) ZEND_TYPE_INIT_CLASS(property_classUnionProp_class_stdClass, 0, 0);
	property_classUnionProp_type_list->types[1] = (zend_type) ZEND_TYPE_INIT_CLASS(property_classUnionProp_class_Iterator, 0, 0);
	zend_type property_classUnionProp_type = ZEND_TYPE_INIT_UNION(property_classUnionProp_type_list, MAY_BE_NULL);
	zval property_classUnionProp_default_value;
	ZVAL_NULL(&property_classUnionProp_default_value);
	zend_string *property_classUnionProp_name = zend_string_init("classUnionProp", sizeof("classUnionProp") - 1, 1);
	zend_declare_typed_property(class_entry, property_classUnionProp_name, &property_classUnionProp_default_value, ZEND_ACC_PUBLIC, NULL, property_classUnionProp_type);
	zend_string_release(property_classUnionProp_name);

	zval property_readonlyProp_default_value;
	ZVAL_UNDEF(&property_readonlyProp_default_value);
	zend_string *property_readonlyProp_name = zend_string_init("readonlyProp", sizeof("readonlyProp") - 1, 1);
	zend_declare_typed_property(class_entry, property_readonlyProp_name, &property_readonlyProp_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_readonlyProp_name);

	return class_entry;
}

static zend_class_entry *register_class__ZendTestChildClass(zend_class_entry *class_entry__ZendTestClass)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "_ZendTestChildClass", class__ZendTestChildClass_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry__ZendTestClass);

	return class_entry;
}

static zend_class_entry *register_class__ZendTestTrait(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "_ZendTestTrait", class__ZendTestTrait_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_TRAIT;

	zval property_testProp_default_value;
	ZVAL_NULL(&property_testProp_default_value);
	zend_string *property_testProp_name = zend_string_init("testProp", sizeof("testProp") - 1, 1);
	zend_declare_property_ex(class_entry, property_testProp_name, &property_testProp_default_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(property_testProp_name);

	return class_entry;
}

static zend_class_entry *register_class_ZendTestAttribute(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ZendTestAttribute", class_ZendTestAttribute_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	return class_entry;
}

static zend_class_entry *register_class_ZendTestParameterAttribute(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ZendTestParameterAttribute", class_ZendTestParameterAttribute_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	zval property_parameter_default_value;
	ZVAL_UNDEF(&property_parameter_default_value);
	zend_string *property_parameter_name = zend_string_init("parameter", sizeof("parameter") - 1, 1);
	zend_declare_typed_property(class_entry, property_parameter_name, &property_parameter_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_parameter_name);

	return class_entry;
}

static zend_class_entry *register_class_ZendTestClassWithMethodWithParameterAttribute(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ZendTestClassWithMethodWithParameterAttribute", class_ZendTestClassWithMethodWithParameterAttribute_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	return class_entry;
}

static zend_class_entry *register_class_ZendTestChildClassWithMethodWithParameterAttribute(zend_class_entry *class_entry_ZendTestClassWithMethodWithParameterAttribute)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ZendTestChildClassWithMethodWithParameterAttribute", class_ZendTestChildClassWithMethodWithParameterAttribute_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_ZendTestClassWithMethodWithParameterAttribute);

	return class_entry;
}

static zend_class_entry *register_class_ZendTestUnitEnum(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("ZendTestUnitEnum", IS_UNDEF, class_ZendTestUnitEnum_methods);

	zend_enum_add_case_cstr(class_entry, "Foo", NULL);

	zend_enum_add_case_cstr(class_entry, "Bar", NULL);

	return class_entry;
}

static zend_class_entry *register_class_ZendTestStringEnum(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("ZendTestStringEnum", IS_STRING, class_ZendTestStringEnum_methods);

	zval enum_case_Foo_value;
	zend_string *enum_case_Foo_value_str = zend_string_init("Test1", sizeof("Test1") - 1, 1);
	ZVAL_STR(&enum_case_Foo_value, enum_case_Foo_value_str);
	zend_enum_add_case_cstr(class_entry, "Foo", &enum_case_Foo_value);

	zval enum_case_Bar_value;
	zend_string *enum_case_Bar_value_str = zend_string_init("Test2", sizeof("Test2") - 1, 1);
	ZVAL_STR(&enum_case_Bar_value, enum_case_Bar_value_str);
	zend_enum_add_case_cstr(class_entry, "Bar", &enum_case_Bar_value);

	zval enum_case_Baz_value;
	zend_string *enum_case_Baz_value_str = zend_string_init("Test2\\a", sizeof("Test2\\a") - 1, 1);
	ZVAL_STR(&enum_case_Baz_value, enum_case_Baz_value_str);
	zend_enum_add_case_cstr(class_entry, "Baz", &enum_case_Baz_value);

	zval enum_case_FortyTwo_value;
	zend_string *enum_case_FortyTwo_value_str = zend_string_init("42", sizeof("42") - 1, 1);
	ZVAL_STR(&enum_case_FortyTwo_value, enum_case_FortyTwo_value_str);
	zend_enum_add_case_cstr(class_entry, "FortyTwo", &enum_case_FortyTwo_value);

	return class_entry;
}

static zend_class_entry *register_class_ZendTestNS_Foo(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "ZendTestNS", "Foo", class_ZendTestNS_Foo_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	return class_entry;
}

static zend_class_entry *register_class_ZendTestNS2_Foo(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "ZendTestNS2", "Foo", class_ZendTestNS2_Foo_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	zend_string *property_foo_class_ZendTestNS2_ZendSubNS_Foo = zend_string_init("ZendTestNS2\\ZendSubNS\\Foo", sizeof("ZendTestNS2\\ZendSubNS\\Foo")-1, 1);
	zval property_foo_default_value;
	ZVAL_UNDEF(&property_foo_default_value);
	zend_string *property_foo_name = zend_string_init("foo", sizeof("foo") - 1, 1);
	zend_declare_typed_property(class_entry, property_foo_name, &property_foo_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_foo_class_ZendTestNS2_ZendSubNS_Foo, 0, 0));
	zend_string_release(property_foo_name);

	return class_entry;
}

static zend_class_entry *register_class_ZendTestNS2_ZendSubNS_Foo(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "ZendTestNS2\\ZendSubNS", "Foo", class_ZendTestNS2_ZendSubNS_Foo_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	return class_entry;
}
