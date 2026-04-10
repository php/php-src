/* This is a generated file, edit test.stub.php instead.
 * Stub hash: 9a23b7d5305982930579428a345ded725ff5145f
 * Has decl header: yes */

ZEND_BEGIN_ARG_INFO_EX(arginfo_zend_trigger_bailout, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_zend_test_array_return arginfo_zend_trigger_bailout

#define arginfo_zend_test_nullable_array_return arginfo_zend_trigger_bailout

#define arginfo_zend_test_void_return arginfo_zend_trigger_bailout

ZEND_BEGIN_ARG_INFO_EX(arginfo_zend_test_compile_string, 0, 0, 3)
	ZEND_ARG_INFO(0, source_string)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, position)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_zend_test_deprecated, 0, 0, 0)
	ZEND_ARG_INFO(0, arg)
ZEND_END_ARG_INFO()

#define arginfo_zend_test_deprecated_attr arginfo_zend_trigger_bailout

#define arginfo_zend_test_nodiscard arginfo_zend_trigger_bailout

#define arginfo_zend_test_deprecated_nodiscard arginfo_zend_trigger_bailout

#define arginfo_zend_test_aliased arginfo_zend_trigger_bailout

#define arginfo_zend_test_deprecated_aliased arginfo_zend_trigger_bailout

ZEND_BEGIN_ARG_INFO_EX(arginfo_zend_create_unterminated_string, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_zend_terminate_string, 0, 0, 1)
	ZEND_ARG_INFO(1, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_zend_leak_variable, 0, 0, 1)
	ZEND_ARG_INFO(0, variable)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_zend_leak_bytes, 0, 0, 0)
	ZEND_ARG_INFO(0, bytes)
ZEND_END_ARG_INFO()

#define arginfo_zend_delref arginfo_zend_leak_variable

ZEND_BEGIN_ARG_INFO_EX(arginfo_zend_string_or_object, 0, 0, 1)
	ZEND_ARG_INFO(0, param)
ZEND_END_ARG_INFO()

#define arginfo_zend_string_or_object_or_null arginfo_zend_string_or_object

#define arginfo_zend_string_or_stdclass arginfo_zend_string_or_object

#define arginfo_zend_string_or_stdclass_or_null arginfo_zend_string_or_object

#define arginfo_zend_number_or_string arginfo_zend_string_or_object

#define arginfo_zend_number_or_string_or_null arginfo_zend_string_or_object

ZEND_BEGIN_ARG_INFO_EX(arginfo_zend_iterable, 0, 0, 1)
	ZEND_ARG_INFO(0, arg1)
	ZEND_ARG_INFO(0, arg2)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_zend_weakmap_attach, 0, 0, 2)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_zend_weakmap_remove, 0, 0, 1)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

#define arginfo_zend_weakmap_dump arginfo_zend_trigger_bailout

#define arginfo_zend_get_unit_enum arginfo_zend_trigger_bailout

ZEND_BEGIN_ARG_INFO_EX(arginfo_zend_test_parameter_with_attribute, 0, 0, 1)
	ZEND_ARG_INFO(0, parameter)
ZEND_END_ARG_INFO()

#define arginfo_zend_test_attribute_with_named_argument arginfo_zend_trigger_bailout

#define arginfo_zend_get_current_func_name arginfo_zend_trigger_bailout

ZEND_BEGIN_ARG_INFO_EX(arginfo_zend_call_method, 0, 0, 2)
	ZEND_ARG_INFO(0, obj_or_class)
	ZEND_ARG_INFO(0, method)
	ZEND_ARG_INFO(0, arg1)
	ZEND_ARG_INFO(0, arg2)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_zend_object_init_with_constructor, 0, 0, 1)
	ZEND_ARG_INFO(0, class)
	ZEND_ARG_VARIADIC_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_zend_call_method_if_exists, 0, 0, 2)
	ZEND_ARG_INFO(0, obj)
	ZEND_ARG_INFO(0, method)
	ZEND_ARG_VARIADIC_INFO(0, args)
ZEND_END_ARG_INFO()

#define arginfo_zend_test_zend_ini_parse_quantity arginfo_zend_create_unterminated_string

#define arginfo_zend_test_zend_ini_parse_uquantity arginfo_zend_create_unterminated_string

#define arginfo_zend_test_zend_ini_str arginfo_zend_trigger_bailout

#define arginfo_zend_test_zstr_init_literal arginfo_zend_trigger_bailout

#if defined(ZEND_CHECK_STACK_LIMIT)
ZEND_BEGIN_ARG_INFO_EX(arginfo_zend_test_zend_call_stack_get, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_zend_test_zend_call_stack_use_all arginfo_zend_test_zend_call_stack_get
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_zend_test_is_string_marked_as_valid_utf8, 0, 0, 1)
	ZEND_ARG_INFO(0, string)
ZEND_END_ARG_INFO()

#define arginfo_zend_get_map_ptr_last arginfo_zend_trigger_bailout

ZEND_BEGIN_ARG_INFO_EX(arginfo_zend_test_crash, 0, 0, 0)
	ZEND_ARG_INFO(0, message)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_zend_test_fill_packed_array, 0, 0, 1)
	ZEND_ARG_INFO(1, array)
ZEND_END_ARG_INFO()

#define arginfo_zend_test_create_throwing_resource arginfo_zend_trigger_bailout

#define arginfo_get_open_basedir arginfo_zend_trigger_bailout

#if defined(HAVE_LIBXML) && !defined(PHP_WIN32)
ZEND_BEGIN_ARG_INFO_EX(arginfo_zend_test_override_libxml_global_state, 0, 0, 0)
ZEND_END_ARG_INFO()
#endif

#define arginfo_zend_test_is_pcre_bundled arginfo_zend_trigger_bailout

#if defined(PHP_WIN32)
ZEND_BEGIN_ARG_INFO_EX(arginfo_zend_test_set_fmode, 0, 0, 1)
	ZEND_ARG_INFO(0, binary)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_zend_test_cast_fread, 0, 0, 1)
	ZEND_ARG_INFO(0, stream)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_zend_test_is_zend_ptr, 0, 0, 1)
	ZEND_ARG_INFO(0, addr)
ZEND_END_ARG_INFO()

#define arginfo_zend_test_log_err_debug arginfo_zend_create_unterminated_string

#define arginfo_zend_test_compile_to_ast arginfo_zend_create_unterminated_string

#define arginfo_zend_test_gh18756 arginfo_zend_trigger_bailout

#define arginfo_zend_test_opcache_preloading arginfo_zend_trigger_bailout

ZEND_BEGIN_ARG_INFO_EX(arginfo_zend_test_uri_parser, 0, 0, 2)
	ZEND_ARG_INFO(0, uri)
	ZEND_ARG_INFO(0, parser)
ZEND_END_ARG_INFO()

#define arginfo_zend_test_gh19792 arginfo_zend_trigger_bailout

#define arginfo_ZendTestNS2_namespaced_func arginfo_zend_trigger_bailout

#define arginfo_ZendTestNS2_namespaced_deprecated_func arginfo_zend_trigger_bailout

#define arginfo_ZendTestNS2_namespaced_aliased_func arginfo_zend_trigger_bailout

#define arginfo_ZendTestNS2_namespaced_deprecated_aliased_func arginfo_zend_trigger_bailout

#define arginfo_ZendTestNS2_ZendSubNS_namespaced_func arginfo_zend_trigger_bailout

#define arginfo_ZendTestNS2_ZendSubNS_namespaced_deprecated_func arginfo_zend_trigger_bailout

#define arginfo_ZendTestNS2_ZendSubNS_namespaced_aliased_func arginfo_zend_trigger_bailout

#define arginfo_ZendTestNS2_ZendSubNS_namespaced_deprecated_aliased_func arginfo_zend_trigger_bailout

#define arginfo_class__ZendTestClass_is_object arginfo_zend_trigger_bailout

#define arginfo_class__ZendTestClass___toString arginfo_zend_trigger_bailout

#define arginfo_class__ZendTestClass_returnsStatic arginfo_zend_trigger_bailout

#define arginfo_class__ZendTestClass_returnsThrowable arginfo_zend_trigger_bailout

ZEND_BEGIN_ARG_INFO_EX(arginfo_class__ZendTestClass_variadicTest, 0, 0, 0)
	ZEND_ARG_VARIADIC_INFO(0, elements)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class__ZendTestClass_takesUnionType, 0, 0, 1)
	ZEND_ARG_INFO(0, arg)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class__ZendTestMagicCall___call, 0, 0, 2)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, args)
ZEND_END_ARG_INFO()

#define arginfo_class__ZendTestMagicCallForward___call arginfo_class__ZendTestMagicCall___call

#define arginfo_class__ZendTestChildClass_returnsThrowable arginfo_zend_trigger_bailout

#define arginfo_class_ZendAttributeTest_testMethod arginfo_zend_trigger_bailout

#define arginfo_class__ZendTestTrait_testMethod arginfo_zend_trigger_bailout

#define arginfo_class_ZendTestAttributeWithArguments___construct arginfo_class__ZendTestClass_takesUnionType

#define arginfo_class_ZendTestParameterAttribute___construct arginfo_zend_test_parameter_with_attribute

#define arginfo_class_ZendTestPropertyAttribute___construct arginfo_zend_test_parameter_with_attribute

#define arginfo_class_ZendTestClassWithMethodWithParameterAttribute_no_override arginfo_zend_test_parameter_with_attribute

#define arginfo_class_ZendTestClassWithMethodWithParameterAttribute_override arginfo_zend_test_parameter_with_attribute

#define arginfo_class_ZendTestChildClassWithMethodWithParameterAttribute_override arginfo_zend_test_parameter_with_attribute

#define arginfo_class_ZendTestForbidDynamicCall_call arginfo_zend_trigger_bailout

#define arginfo_class_ZendTestForbidDynamicCall_callStatic arginfo_zend_trigger_bailout

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ZendTestNS_Foo_method, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ZendTestNS_UnlikelyCompileError_method arginfo_zend_trigger_bailout

#define arginfo_class_ZendTestNS_NotUnlikelyCompileError_method arginfo_zend_trigger_bailout

#define arginfo_class_ZendTestNS2_Foo_method arginfo_zend_trigger_bailout

#define arginfo_class_ZendTestNS2_ZendSubNS_Foo_method arginfo_zend_trigger_bailout

static ZEND_FUNCTION(zend_trigger_bailout);
static ZEND_FUNCTION(zend_test_array_return);
static ZEND_FUNCTION(zend_test_nullable_array_return);
static ZEND_FUNCTION(zend_test_void_return);
static ZEND_FUNCTION(zend_test_compile_string);
static ZEND_FUNCTION(zend_test_deprecated);
static ZEND_FUNCTION(zend_test_deprecated_attr);
static ZEND_FUNCTION(zend_test_nodiscard);
static ZEND_FUNCTION(zend_test_deprecated_nodiscard);
static ZEND_FUNCTION(zend_create_unterminated_string);
static ZEND_FUNCTION(zend_terminate_string);
static ZEND_FUNCTION(zend_leak_variable);
static ZEND_FUNCTION(zend_leak_bytes);
static ZEND_FUNCTION(zend_delref);
static ZEND_FUNCTION(zend_string_or_object);
static ZEND_FUNCTION(zend_string_or_object_or_null);
static ZEND_FUNCTION(zend_string_or_stdclass);
static ZEND_FUNCTION(zend_string_or_stdclass_or_null);
static ZEND_FUNCTION(zend_number_or_string);
static ZEND_FUNCTION(zend_number_or_string_or_null);
static ZEND_FUNCTION(zend_iterable);
static ZEND_FUNCTION(zend_weakmap_attach);
static ZEND_FUNCTION(zend_weakmap_remove);
static ZEND_FUNCTION(zend_weakmap_dump);
static ZEND_FUNCTION(zend_get_unit_enum);
static ZEND_FUNCTION(zend_test_parameter_with_attribute);
static ZEND_FUNCTION(zend_test_attribute_with_named_argument);
static ZEND_FUNCTION(zend_get_current_func_name);
static ZEND_FUNCTION(zend_call_method);
static ZEND_FUNCTION(zend_object_init_with_constructor);
static ZEND_FUNCTION(zend_call_method_if_exists);
static ZEND_FUNCTION(zend_test_zend_ini_parse_quantity);
static ZEND_FUNCTION(zend_test_zend_ini_parse_uquantity);
static ZEND_FUNCTION(zend_test_zend_ini_str);
static ZEND_FUNCTION(zend_test_zstr_init_literal);
#if defined(ZEND_CHECK_STACK_LIMIT)
static ZEND_FUNCTION(zend_test_zend_call_stack_get);
static ZEND_FUNCTION(zend_test_zend_call_stack_use_all);
#endif
static ZEND_FUNCTION(zend_test_is_string_marked_as_valid_utf8);
static ZEND_FUNCTION(zend_get_map_ptr_last);
static ZEND_FUNCTION(zend_test_crash);
static ZEND_FUNCTION(zend_test_fill_packed_array);
static ZEND_FUNCTION(zend_test_create_throwing_resource);
static ZEND_FUNCTION(get_open_basedir);
#if defined(HAVE_LIBXML) && !defined(PHP_WIN32)
static ZEND_FUNCTION(zend_test_override_libxml_global_state);
#endif
static ZEND_FUNCTION(zend_test_is_pcre_bundled);
#if defined(PHP_WIN32)
static ZEND_FUNCTION(zend_test_set_fmode);
#endif
static ZEND_FUNCTION(zend_test_cast_fread);
static ZEND_FUNCTION(zend_test_is_zend_ptr);
static ZEND_FUNCTION(zend_test_log_err_debug);
static ZEND_FUNCTION(zend_test_compile_to_ast);
static ZEND_FUNCTION(zend_test_gh18756);
static ZEND_FUNCTION(zend_test_opcache_preloading);
static ZEND_FUNCTION(zend_test_uri_parser);
static ZEND_FUNCTION(zend_test_gh19792);
static ZEND_FUNCTION(ZendTestNS2_namespaced_func);
static ZEND_FUNCTION(ZendTestNS2_namespaced_deprecated_func);
static ZEND_FUNCTION(ZendTestNS2_ZendSubNS_namespaced_func);
static ZEND_FUNCTION(ZendTestNS2_ZendSubNS_namespaced_deprecated_func);
static ZEND_METHOD(_ZendTestClass, is_object);
static ZEND_METHOD(_ZendTestClass, __toString);
static ZEND_METHOD(_ZendTestClass, returnsStatic);
static ZEND_METHOD(_ZendTestClass, returnsThrowable);
static ZEND_METHOD(_ZendTestClass, variadicTest);
static ZEND_METHOD(_ZendTestClass, takesUnionType);
static ZEND_METHOD(_ZendTestMagicCall, __call);
static ZEND_METHOD(_ZendTestMagicCallForward, __call);
static ZEND_METHOD(_ZendTestChildClass, returnsThrowable);
static ZEND_METHOD(ZendAttributeTest, testMethod);
static ZEND_METHOD(_ZendTestTrait, testMethod);
static ZEND_METHOD(ZendTestAttributeWithArguments, __construct);
static ZEND_METHOD(ZendTestParameterAttribute, __construct);
static ZEND_METHOD(ZendTestPropertyAttribute, __construct);
static ZEND_METHOD(ZendTestClassWithMethodWithParameterAttribute, no_override);
static ZEND_METHOD(ZendTestClassWithMethodWithParameterAttribute, override);
static ZEND_METHOD(ZendTestChildClassWithMethodWithParameterAttribute, override);
static ZEND_METHOD(ZendTestForbidDynamicCall, call);
static ZEND_METHOD(ZendTestForbidDynamicCall, callStatic);
static ZEND_METHOD(ZendTestNS_Foo, method);
static ZEND_METHOD(ZendTestNS_UnlikelyCompileError, method);
static ZEND_METHOD(ZendTestNS_NotUnlikelyCompileError, method);
static ZEND_METHOD(ZendTestNS2_Foo, method);
static ZEND_METHOD(ZendTestNS2_ZendSubNS_Foo, method);

static const zend_function_entry ext_functions[] = {
	ZEND_FE(zend_trigger_bailout, arginfo_zend_trigger_bailout)
	ZEND_FE(zend_test_array_return, arginfo_zend_test_array_return)
	ZEND_FE(zend_test_nullable_array_return, arginfo_zend_test_nullable_array_return)
	ZEND_FE(zend_test_void_return, arginfo_zend_test_void_return)
	ZEND_FE(zend_test_compile_string, arginfo_zend_test_compile_string)
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY("zend_test_deprecated", zif_zend_test_deprecated, arginfo_zend_test_deprecated, ZEND_ACC_DEPRECATED, NULL, NULL)
#else
	ZEND_RAW_FENTRY("zend_test_deprecated", zif_zend_test_deprecated, arginfo_zend_test_deprecated, ZEND_ACC_DEPRECATED)
#endif
	ZEND_FE(zend_test_deprecated_attr, arginfo_zend_test_deprecated_attr)
	ZEND_FE(zend_test_nodiscard, arginfo_zend_test_nodiscard)
	ZEND_FE(zend_test_deprecated_nodiscard, arginfo_zend_test_deprecated_nodiscard)
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY("zend_test_aliased", zif_zend_test_void_return, arginfo_zend_test_aliased, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY("zend_test_aliased", zif_zend_test_void_return, arginfo_zend_test_aliased, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY("zend_test_deprecated_aliased", zif_zend_test_void_return, arginfo_zend_test_deprecated_aliased, ZEND_ACC_DEPRECATED, NULL, NULL)
#else
	ZEND_RAW_FENTRY("zend_test_deprecated_aliased", zif_zend_test_void_return, arginfo_zend_test_deprecated_aliased, ZEND_ACC_DEPRECATED)
#endif
	ZEND_FE(zend_create_unterminated_string, arginfo_zend_create_unterminated_string)
	ZEND_FE(zend_terminate_string, arginfo_zend_terminate_string)
	ZEND_FE(zend_leak_variable, arginfo_zend_leak_variable)
	ZEND_FE(zend_leak_bytes, arginfo_zend_leak_bytes)
	ZEND_FE(zend_delref, arginfo_zend_delref)
	ZEND_FE(zend_string_or_object, arginfo_zend_string_or_object)
	ZEND_FE(zend_string_or_object_or_null, arginfo_zend_string_or_object_or_null)
	ZEND_FE(zend_string_or_stdclass, arginfo_zend_string_or_stdclass)
	ZEND_FE(zend_string_or_stdclass_or_null, arginfo_zend_string_or_stdclass_or_null)
	ZEND_FE(zend_number_or_string, arginfo_zend_number_or_string)
	ZEND_FE(zend_number_or_string_or_null, arginfo_zend_number_or_string_or_null)
	ZEND_FE(zend_iterable, arginfo_zend_iterable)
	ZEND_FE(zend_weakmap_attach, arginfo_zend_weakmap_attach)
	ZEND_FE(zend_weakmap_remove, arginfo_zend_weakmap_remove)
	ZEND_FE(zend_weakmap_dump, arginfo_zend_weakmap_dump)
	ZEND_FE(zend_get_unit_enum, arginfo_zend_get_unit_enum)
	ZEND_FE(zend_test_parameter_with_attribute, arginfo_zend_test_parameter_with_attribute)
	ZEND_FE(zend_test_attribute_with_named_argument, arginfo_zend_test_attribute_with_named_argument)
	ZEND_FE(zend_get_current_func_name, arginfo_zend_get_current_func_name)
	ZEND_FE(zend_call_method, arginfo_zend_call_method)
	ZEND_FE(zend_object_init_with_constructor, arginfo_zend_object_init_with_constructor)
	ZEND_FE(zend_call_method_if_exists, arginfo_zend_call_method_if_exists)
	ZEND_FE(zend_test_zend_ini_parse_quantity, arginfo_zend_test_zend_ini_parse_quantity)
	ZEND_FE(zend_test_zend_ini_parse_uquantity, arginfo_zend_test_zend_ini_parse_uquantity)
	ZEND_FE(zend_test_zend_ini_str, arginfo_zend_test_zend_ini_str)
	ZEND_FE(zend_test_zstr_init_literal, arginfo_zend_test_zstr_init_literal)
#if defined(ZEND_CHECK_STACK_LIMIT)
	ZEND_FE(zend_test_zend_call_stack_get, arginfo_zend_test_zend_call_stack_get)
	ZEND_FE(zend_test_zend_call_stack_use_all, arginfo_zend_test_zend_call_stack_use_all)
#endif
	ZEND_FE(zend_test_is_string_marked_as_valid_utf8, arginfo_zend_test_is_string_marked_as_valid_utf8)
	ZEND_FE(zend_get_map_ptr_last, arginfo_zend_get_map_ptr_last)
	ZEND_FE(zend_test_crash, arginfo_zend_test_crash)
	ZEND_FE(zend_test_fill_packed_array, arginfo_zend_test_fill_packed_array)
	ZEND_FE(zend_test_create_throwing_resource, arginfo_zend_test_create_throwing_resource)
	ZEND_FE(get_open_basedir, arginfo_get_open_basedir)
#if defined(HAVE_LIBXML) && !defined(PHP_WIN32)
	ZEND_FE(zend_test_override_libxml_global_state, arginfo_zend_test_override_libxml_global_state)
#endif
	ZEND_FE(zend_test_is_pcre_bundled, arginfo_zend_test_is_pcre_bundled)
#if defined(PHP_WIN32)
	ZEND_FE(zend_test_set_fmode, arginfo_zend_test_set_fmode)
#endif
	ZEND_FE(zend_test_cast_fread, arginfo_zend_test_cast_fread)
	ZEND_FE(zend_test_is_zend_ptr, arginfo_zend_test_is_zend_ptr)
	ZEND_FE(zend_test_log_err_debug, arginfo_zend_test_log_err_debug)
	ZEND_FE(zend_test_compile_to_ast, arginfo_zend_test_compile_to_ast)
	ZEND_FE(zend_test_gh18756, arginfo_zend_test_gh18756)
	ZEND_FE(zend_test_opcache_preloading, arginfo_zend_test_opcache_preloading)
	ZEND_FE(zend_test_uri_parser, arginfo_zend_test_uri_parser)
	ZEND_FE(zend_test_gh19792, arginfo_zend_test_gh19792)
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ZendTestNS2", "namespaced_func"), zif_ZendTestNS2_namespaced_func, arginfo_ZendTestNS2_namespaced_func, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ZendTestNS2", "namespaced_func"), zif_ZendTestNS2_namespaced_func, arginfo_ZendTestNS2_namespaced_func, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ZendTestNS2", "namespaced_deprecated_func"), zif_ZendTestNS2_namespaced_deprecated_func, arginfo_ZendTestNS2_namespaced_deprecated_func, ZEND_ACC_DEPRECATED, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ZendTestNS2", "namespaced_deprecated_func"), zif_ZendTestNS2_namespaced_deprecated_func, arginfo_ZendTestNS2_namespaced_deprecated_func, ZEND_ACC_DEPRECATED)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ZendTestNS2", "namespaced_aliased_func"), zif_zend_test_void_return, arginfo_ZendTestNS2_namespaced_aliased_func, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ZendTestNS2", "namespaced_aliased_func"), zif_zend_test_void_return, arginfo_ZendTestNS2_namespaced_aliased_func, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ZendTestNS2", "namespaced_deprecated_aliased_func"), zif_zend_test_void_return, arginfo_ZendTestNS2_namespaced_deprecated_aliased_func, ZEND_ACC_DEPRECATED, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ZendTestNS2", "namespaced_deprecated_aliased_func"), zif_zend_test_void_return, arginfo_ZendTestNS2_namespaced_deprecated_aliased_func, ZEND_ACC_DEPRECATED)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ZendTestNS2\\ZendSubNS", "namespaced_func"), zif_ZendTestNS2_ZendSubNS_namespaced_func, arginfo_ZendTestNS2_ZendSubNS_namespaced_func, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ZendTestNS2\\ZendSubNS", "namespaced_func"), zif_ZendTestNS2_ZendSubNS_namespaced_func, arginfo_ZendTestNS2_ZendSubNS_namespaced_func, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ZendTestNS2\\ZendSubNS", "namespaced_deprecated_func"), zif_ZendTestNS2_ZendSubNS_namespaced_deprecated_func, arginfo_ZendTestNS2_ZendSubNS_namespaced_deprecated_func, ZEND_ACC_DEPRECATED, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ZendTestNS2\\ZendSubNS", "namespaced_deprecated_func"), zif_ZendTestNS2_ZendSubNS_namespaced_deprecated_func, arginfo_ZendTestNS2_ZendSubNS_namespaced_deprecated_func, ZEND_ACC_DEPRECATED)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ZendTestNS2\\ZendSubNS", "namespaced_aliased_func"), zif_zend_test_void_return, arginfo_ZendTestNS2_ZendSubNS_namespaced_aliased_func, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ZendTestNS2\\ZendSubNS", "namespaced_aliased_func"), zif_zend_test_void_return, arginfo_ZendTestNS2_ZendSubNS_namespaced_aliased_func, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ZendTestNS2\\ZendSubNS", "namespaced_deprecated_aliased_func"), zif_zend_test_void_return, arginfo_ZendTestNS2_ZendSubNS_namespaced_deprecated_aliased_func, ZEND_ACC_DEPRECATED, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("ZendTestNS2\\ZendSubNS", "namespaced_deprecated_aliased_func"), zif_zend_test_void_return, arginfo_ZendTestNS2_ZendSubNS_namespaced_deprecated_aliased_func, ZEND_ACC_DEPRECATED)
#endif
	ZEND_FE_END
};

static const zend_function_entry class__ZendTestClass_methods[] = {
	ZEND_ME(_ZendTestClass, is_object, arginfo_class__ZendTestClass_is_object, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(_ZendTestClass, __toString, arginfo_class__ZendTestClass___toString, ZEND_ACC_PUBLIC|ZEND_ACC_DEPRECATED)
	ZEND_ME(_ZendTestClass, returnsStatic, arginfo_class__ZendTestClass_returnsStatic, ZEND_ACC_PUBLIC)
	ZEND_ME(_ZendTestClass, returnsThrowable, arginfo_class__ZendTestClass_returnsThrowable, ZEND_ACC_PUBLIC)
	ZEND_ME(_ZendTestClass, variadicTest, arginfo_class__ZendTestClass_variadicTest, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(_ZendTestClass, takesUnionType, arginfo_class__ZendTestClass_takesUnionType, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class__ZendTestMagicCall_methods[] = {
	ZEND_ME(_ZendTestMagicCall, __call, arginfo_class__ZendTestMagicCall___call, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class__ZendTestMagicCallForward_methods[] = {
	ZEND_ME(_ZendTestMagicCallForward, __call, arginfo_class__ZendTestMagicCallForward___call, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class__ZendTestChildClass_methods[] = {
	ZEND_ME(_ZendTestChildClass, returnsThrowable, arginfo_class__ZendTestChildClass_returnsThrowable, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_ZendAttributeTest_methods[] = {
	ZEND_ME(ZendAttributeTest, testMethod, arginfo_class_ZendAttributeTest_testMethod, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class__ZendTestTrait_methods[] = {
	ZEND_ME(_ZendTestTrait, testMethod, arginfo_class__ZendTestTrait_testMethod, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_ZendTestAttributeWithArguments_methods[] = {
	ZEND_ME(ZendTestAttributeWithArguments, __construct, arginfo_class_ZendTestAttributeWithArguments___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_ZendTestParameterAttribute_methods[] = {
	ZEND_ME(ZendTestParameterAttribute, __construct, arginfo_class_ZendTestParameterAttribute___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_ZendTestPropertyAttribute_methods[] = {
	ZEND_ME(ZendTestPropertyAttribute, __construct, arginfo_class_ZendTestPropertyAttribute___construct, ZEND_ACC_PUBLIC)
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

static const zend_function_entry class_ZendTestForbidDynamicCall_methods[] = {
	ZEND_ME(ZendTestForbidDynamicCall, call, arginfo_class_ZendTestForbidDynamicCall_call, ZEND_ACC_PUBLIC)
	ZEND_ME(ZendTestForbidDynamicCall, callStatic, arginfo_class_ZendTestForbidDynamicCall_callStatic, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};

static const zend_function_entry class_ZendTestNS_Foo_methods[] = {
	ZEND_ME(ZendTestNS_Foo, method, arginfo_class_ZendTestNS_Foo_method, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_ZendTestNS_UnlikelyCompileError_methods[] = {
	ZEND_ME(ZendTestNS_UnlikelyCompileError, method, arginfo_class_ZendTestNS_UnlikelyCompileError_method, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_ZendTestNS_NotUnlikelyCompileError_methods[] = {
	ZEND_ME(ZendTestNS_NotUnlikelyCompileError, method, arginfo_class_ZendTestNS_NotUnlikelyCompileError_method, ZEND_ACC_PUBLIC)
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

static void register_test_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("ZEND_TEST_DEPRECATED", 42, CONST_PERSISTENT | CONST_CS);
	REGISTER_STRING_CONSTANT("ZEND_CONSTANT_A", "global", CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("ZEND_TEST_ATTRIBUTED_CONSTANT", 42, CONST_PERSISTENT | CONST_CS);
	REGISTER_STRING_CONSTANT("ZendTestNS2\\ZEND_CONSTANT_A", "namespaced", CONST_PERSISTENT | CONST_CS);
	REGISTER_STRING_CONSTANT("ZendTestNS2\\ZendSubNS\\ZEND_CONSTANT_A", "namespaced", CONST_PERSISTENT | CONST_CS);
}

static zend_class_entry *register_class__ZendTestInterface(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "_ZendTestInterface", NULL);
	class_entry = zend_register_internal_interface(&ce);

	zval const_DUMMY_value;
	ZVAL_LONG(&const_DUMMY_value, 0);
	zend_string *const_DUMMY_name = zend_string_init_interned("DUMMY", sizeof("DUMMY") - 1, true);
	zend_string *const_DUMMY_comment = zend_string_init_interned("/**\n         * \"Lorem ipsum\"\n         * @see https://www.php.net\n         * @since 8.2\n         */", 98, 1);
	zend_declare_class_constant_ex(class_entry, const_DUMMY_name, &const_DUMMY_value, ZEND_ACC_PUBLIC, const_DUMMY_comment);
	zend_string_release_ex(const_DUMMY_name, true);

	return class_entry;
}

static zend_class_entry *register_class__ZendTestClass(zend_class_entry *class_entry__ZendTestInterface)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "_ZendTestClass", class__ZendTestClass_methods);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, 0);
#else
	class_entry = zend_register_internal_class_ex(&ce, NULL);
#endif
	zend_class_implements(class_entry, 1, class_entry__ZendTestInterface);
	zend_register_class_alias("_ZendTestClassAlias", class_entry);

	zval const_TYPED_CLASS_CONST1_value;
	ZVAL_EMPTY_ARRAY(&const_TYPED_CLASS_CONST1_value);
	zend_string *const_TYPED_CLASS_CONST1_name = zend_string_init_interned("TYPED_CLASS_CONST1", sizeof("TYPED_CLASS_CONST1") - 1, true);
	zend_declare_class_constant_ex(class_entry, const_TYPED_CLASS_CONST1_name, &const_TYPED_CLASS_CONST1_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release_ex(const_TYPED_CLASS_CONST1_name, true);

	zval const_TYPED_CLASS_CONST2_value;
	ZVAL_LONG(&const_TYPED_CLASS_CONST2_value, 42);
	zend_string *const_TYPED_CLASS_CONST2_name = zend_string_init_interned("TYPED_CLASS_CONST2", sizeof("TYPED_CLASS_CONST2") - 1, true);
	zend_declare_class_constant_ex(class_entry, const_TYPED_CLASS_CONST2_name, &const_TYPED_CLASS_CONST2_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release_ex(const_TYPED_CLASS_CONST2_name, true);

	zval const_TYPED_CLASS_CONST3_value;
	ZVAL_LONG(&const_TYPED_CLASS_CONST3_value, 1);
	zend_string *const_TYPED_CLASS_CONST3_name = zend_string_init_interned("TYPED_CLASS_CONST3", sizeof("TYPED_CLASS_CONST3") - 1, true);
	zend_declare_class_constant_ex(class_entry, const_TYPED_CLASS_CONST3_name, &const_TYPED_CLASS_CONST3_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release_ex(const_TYPED_CLASS_CONST3_name, true);

	zval const_ZEND_TEST_DEPRECATED_value;
	ZVAL_LONG(&const_ZEND_TEST_DEPRECATED_value, 42);
	zend_string *const_ZEND_TEST_DEPRECATED_name = zend_string_init_interned("ZEND_TEST_DEPRECATED", sizeof("ZEND_TEST_DEPRECATED") - 1, true);
	zend_declare_class_constant_ex(class_entry, const_ZEND_TEST_DEPRECATED_name, &const_ZEND_TEST_DEPRECATED_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release_ex(const_ZEND_TEST_DEPRECATED_name, true);

	zval const_ZEND_TEST_DEPRECATED_ATTR_value;
	ZVAL_LONG(&const_ZEND_TEST_DEPRECATED_ATTR_value, 42);
	zend_string *const_ZEND_TEST_DEPRECATED_ATTR_name = zend_string_init_interned("ZEND_TEST_DEPRECATED_ATTR", sizeof("ZEND_TEST_DEPRECATED_ATTR") - 1, true);
	zend_declare_class_constant_ex(class_entry, const_ZEND_TEST_DEPRECATED_ATTR_name, &const_ZEND_TEST_DEPRECATED_ATTR_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release_ex(const_ZEND_TEST_DEPRECATED_ATTR_name, true);

	zval property__StaticProp_default_value;
	ZVAL_NULL(&property__StaticProp_default_value);
	zend_string *property__StaticProp_name = zend_string_init("_StaticProp", sizeof("_StaticProp") - 1, true);
	zend_declare_property_ex(class_entry, property__StaticProp_name, &property__StaticProp_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL);
	zend_string_release_ex(property__StaticProp_name, true);

	zval property_staticIntProp_default_value;
	ZVAL_LONG(&property_staticIntProp_default_value, 123);
	zend_string *property_staticIntProp_name = zend_string_init("staticIntProp", sizeof("staticIntProp") - 1, true);
	zend_declare_property_ex(class_entry, property_staticIntProp_name, &property_staticIntProp_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC, NULL);
	zend_string_release_ex(property_staticIntProp_name, true);

	zval property_intProp_default_value;
	ZVAL_LONG(&property_intProp_default_value, 123);
	zend_string *property_intProp_name = zend_string_init("intProp", sizeof("intProp") - 1, true);
	zend_declare_property_ex(class_entry, property_intProp_name, &property_intProp_default_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release_ex(property_intProp_name, true);

	zval property_classProp_default_value;
	ZVAL_NULL(&property_classProp_default_value);
	zend_string *property_classProp_name = zend_string_init("classProp", sizeof("classProp") - 1, true);
	zend_declare_property_ex(class_entry, property_classProp_name, &property_classProp_default_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release_ex(property_classProp_name, true);

	zval property_classUnionProp_default_value;
	ZVAL_NULL(&property_classUnionProp_default_value);
	zend_string *property_classUnionProp_name = zend_string_init("classUnionProp", sizeof("classUnionProp") - 1, true);
	zend_declare_property_ex(class_entry, property_classUnionProp_name, &property_classUnionProp_default_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release_ex(property_classUnionProp_name, true);

	zval property_classIntersectionProp_default_value;
	ZVAL_NULL(&property_classIntersectionProp_default_value);
	zend_string *property_classIntersectionProp_name = zend_string_init("classIntersectionProp", sizeof("classIntersectionProp") - 1, true);
	zend_declare_property_ex(class_entry, property_classIntersectionProp_name, &property_classIntersectionProp_default_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release_ex(property_classIntersectionProp_name, true);

	zval property_readonlyProp_default_value;
	ZVAL_NULL(&property_readonlyProp_default_value);
	zend_string *property_readonlyProp_name = zend_string_init("readonlyProp", sizeof("readonlyProp") - 1, true);
	zend_declare_property_ex(class_entry, property_readonlyProp_name, &property_readonlyProp_default_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release_ex(property_readonlyProp_name, true);

	zval property_finalProp_default_value;
	ZVAL_NULL(&property_finalProp_default_value);
	zend_string *property_finalProp_name = zend_string_init("finalProp", sizeof("finalProp") - 1, true);
#if (PHP_VERSION_ID >= 80400)
	zend_declare_property_ex(class_entry, property_finalProp_name, &property_finalProp_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL, NULL);
#elif (PHP_VERSION_ID >= 70000)
	zend_declare_property_ex(class_entry, property_finalProp_name, &property_finalProp_default_value, ZEND_ACC_PUBLIC, NULL);
#endif
	zend_string_release_ex(property_finalProp_name, true);

	return class_entry;
}

static zend_class_entry *register_class__ZendTestMagicCall(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "_ZendTestMagicCall", class__ZendTestMagicCall_methods);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, 0);
#else
	class_entry = zend_register_internal_class_ex(&ce, NULL);
#endif

	return class_entry;
}

static zend_class_entry *register_class__ZendTestMagicCallForward(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "_ZendTestMagicCallForward", class__ZendTestMagicCallForward_methods);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, 0);
#else
	class_entry = zend_register_internal_class_ex(&ce, NULL);
#endif

	return class_entry;
}

static zend_class_entry *register_class__ZendTestChildClass(zend_class_entry *class_entry__ZendTestClass)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "_ZendTestChildClass", class__ZendTestChildClass_methods);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry__ZendTestClass, 0);
#else
	class_entry = zend_register_internal_class_ex(&ce, class_entry__ZendTestClass);
#endif

	return class_entry;
}

static zend_class_entry *register_class_ZendTestGenStubFlagCompatibilityTest(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ZendTestGenStubFlagCompatibilityTest", NULL);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);
#else
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
#endif

	return class_entry;
}

static zend_class_entry *register_class_ZendAttributeTest(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ZendAttributeTest", class_ZendAttributeTest_methods);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, 0);
#else
	class_entry = zend_register_internal_class_ex(&ce, NULL);
#endif

	zval const_TEST_CONST_value;
	ZVAL_LONG(&const_TEST_CONST_value, 1);
	zend_string *const_TEST_CONST_name = zend_string_init_interned("TEST_CONST", sizeof("TEST_CONST") - 1, true);
	zend_declare_class_constant_ex(class_entry, const_TEST_CONST_name, &const_TEST_CONST_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release_ex(const_TEST_CONST_name, true);

	zval property_testProp_default_value;
	ZVAL_NULL(&property_testProp_default_value);
	zend_string *property_testProp_name = zend_string_init("testProp", sizeof("testProp") - 1, true);
	zend_declare_property_ex(class_entry, property_testProp_name, &property_testProp_default_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release_ex(property_testProp_name, true);

	return class_entry;
}

static zend_class_entry *register_class__ZendTestTrait(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "_ZendTestTrait", class__ZendTestTrait_methods);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_TRAIT);
#else
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_TRAIT;
#endif

	zval property_testProp_default_value;
	ZVAL_NULL(&property_testProp_default_value);
	zend_string *property_testProp_name = zend_string_init("testProp", sizeof("testProp") - 1, true);
	zend_declare_property_ex(class_entry, property_testProp_name, &property_testProp_default_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release_ex(property_testProp_name, true);

	zval property_classUnionProp_default_value;
	ZVAL_NULL(&property_classUnionProp_default_value);
	zend_string *property_classUnionProp_name = zend_string_init("classUnionProp", sizeof("classUnionProp") - 1, true);
	zend_declare_property_ex(class_entry, property_classUnionProp_name, &property_classUnionProp_default_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release_ex(property_classUnionProp_name, true);

	return class_entry;
}

static zend_class_entry *register_class_ZendTestAttribute(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ZendTestAttribute", NULL);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);
#else
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
#endif

	return class_entry;
}

static zend_class_entry *register_class_ZendTestAttributeWithArguments(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ZendTestAttributeWithArguments", class_ZendTestAttributeWithArguments_methods);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);
#else
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
#endif

	zval property_arg_default_value;
	ZVAL_NULL(&property_arg_default_value);
	zend_string *property_arg_name = zend_string_init("arg", sizeof("arg") - 1, true);
	zend_declare_property_ex(class_entry, property_arg_name, &property_arg_default_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release_ex(property_arg_name, true);

	return class_entry;
}

static zend_class_entry *register_class_ZendTestRepeatableAttribute(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ZendTestRepeatableAttribute", NULL);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);
#else
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
#endif

	return class_entry;
}

static zend_class_entry *register_class_ZendTestParameterAttribute(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ZendTestParameterAttribute", class_ZendTestParameterAttribute_methods);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);
#else
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
#endif

	zval property_parameter_default_value;
	ZVAL_NULL(&property_parameter_default_value);
	zend_string *property_parameter_name = zend_string_init("parameter", sizeof("parameter") - 1, true);
	zend_declare_property_ex(class_entry, property_parameter_name, &property_parameter_default_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release_ex(property_parameter_name, true);

	return class_entry;
}

static zend_class_entry *register_class_ZendTestPropertyAttribute(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ZendTestPropertyAttribute", class_ZendTestPropertyAttribute_methods);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);
#else
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
#endif

	zval property_parameter_default_value;
	ZVAL_NULL(&property_parameter_default_value);
	zend_string *property_parameter_name = zend_string_init("parameter", sizeof("parameter") - 1, true);
	zend_string *property_parameter_comment = zend_string_init_interned("/**\n         * \"Lorem ipsum\"\n         * @see https://www.php.net\n         * @since 8.4\n         */", 98, 1);
	zend_declare_property_ex(class_entry, property_parameter_name, &property_parameter_default_value, ZEND_ACC_PUBLIC, property_parameter_comment);
	zend_string_release_ex(property_parameter_name, true);

	return class_entry;
}

static zend_class_entry *register_class_ZendTestClassWithMethodWithParameterAttribute(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ZendTestClassWithMethodWithParameterAttribute", class_ZendTestClassWithMethodWithParameterAttribute_methods);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, 0);
#else
	class_entry = zend_register_internal_class_ex(&ce, NULL);
#endif

	return class_entry;
}

static zend_class_entry *register_class_ZendTestChildClassWithMethodWithParameterAttribute(zend_class_entry *class_entry_ZendTestClassWithMethodWithParameterAttribute)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ZendTestChildClassWithMethodWithParameterAttribute", class_ZendTestChildClassWithMethodWithParameterAttribute_methods);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_ZendTestClassWithMethodWithParameterAttribute, 0);
#else
	class_entry = zend_register_internal_class_ex(&ce, class_entry_ZendTestClassWithMethodWithParameterAttribute);
#endif

	return class_entry;
}

static zend_class_entry *register_class_ZendTestClassWithPropertyAttribute(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ZendTestClassWithPropertyAttribute", NULL);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, 0);
#else
	class_entry = zend_register_internal_class_ex(&ce, NULL);
#endif

	zval property_attributed_default_value;
	ZVAL_NULL(&property_attributed_default_value);
	zend_string *property_attributed_name = zend_string_init("attributed", sizeof("attributed") - 1, true);
	zend_declare_property_ex(class_entry, property_attributed_name, &property_attributed_default_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release_ex(property_attributed_name, true);

	return class_entry;
}

static zend_class_entry *register_class_ZendTestForbidDynamicCall(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ZendTestForbidDynamicCall", class_ZendTestForbidDynamicCall_methods);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);
#else
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
#endif

	return class_entry;
}

#if (PHP_VERSION_ID >= 80100)
static zend_class_entry *register_class_ZendTestUnitEnum(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("ZendTestUnitEnum", IS_UNDEF, NULL);

	zend_enum_add_case_cstr(class_entry, "Foo", NULL);

	zend_enum_add_case_cstr(class_entry, "Bar", NULL);

	return class_entry;
}
#endif

#if (PHP_VERSION_ID >= 80100)
static zend_class_entry *register_class_ZendTestStringEnum(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("ZendTestStringEnum", IS_STRING, NULL);

	zval enum_case_Foo_value;
	zend_string *enum_case_Foo_value_str = zend_string_init("Test1", strlen("Test1"), 1);
	ZVAL_STR(&enum_case_Foo_value, enum_case_Foo_value_str);
	zend_enum_add_case_cstr(class_entry, "Foo", &enum_case_Foo_value);

	zval enum_case_Bar_value;
	zend_string *enum_case_Bar_value_str = zend_string_init("Test2", strlen("Test2"), 1);
	ZVAL_STR(&enum_case_Bar_value, enum_case_Bar_value_str);
	zend_enum_add_case_cstr(class_entry, "Bar", &enum_case_Bar_value);

	zval enum_case_Baz_value;
	zend_string *enum_case_Baz_value_str = zend_string_init("Test2\\a", strlen("Test2\\a"), 1);
	ZVAL_STR(&enum_case_Baz_value, enum_case_Baz_value_str);
	zend_enum_add_case_cstr(class_entry, "Baz", &enum_case_Baz_value);

	zval enum_case_FortyTwo_value;
	zend_string *enum_case_FortyTwo_value_str = zend_string_init("42", strlen("42"), 1);
	ZVAL_STR(&enum_case_FortyTwo_value, enum_case_FortyTwo_value_str);
	zend_enum_add_case_cstr(class_entry, "FortyTwo", &enum_case_FortyTwo_value);

	return class_entry;
}
#endif

#if (PHP_VERSION_ID >= 80100)
static zend_class_entry *register_class_ZendTestIntEnum(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("ZendTestIntEnum", IS_LONG, NULL);

	zval enum_case_Foo_value;
	ZVAL_LONG(&enum_case_Foo_value, 1);
	zend_enum_add_case_cstr(class_entry, "Foo", &enum_case_Foo_value);

	zval enum_case_Bar_value;
	ZVAL_LONG(&enum_case_Bar_value, 3);
	zend_enum_add_case_cstr(class_entry, "Bar", &enum_case_Bar_value);

	zval enum_case_Baz_value;
	ZVAL_LONG(&enum_case_Baz_value, -1);
	zend_enum_add_case_cstr(class_entry, "Baz", &enum_case_Baz_value);

	return class_entry;
}
#endif

#if (PHP_VERSION_ID >= 80100)
static zend_class_entry *register_class_ZendTestEnumWithInterface(zend_class_entry *class_entry__ZendTestInterface)
{
	zend_class_entry *class_entry = zend_register_internal_enum("ZendTestEnumWithInterface", IS_UNDEF, NULL);
	zend_class_implements(class_entry, 1, class_entry__ZendTestInterface);

	zend_enum_add_case_cstr(class_entry, "Foo", NULL);

	zend_enum_add_case_cstr(class_entry, "Bar", NULL);

	return class_entry;
}
#endif

static zend_class_entry *register_class_ZendTestNS_Foo(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "ZendTestNS", "Foo", class_ZendTestNS_Foo_methods);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, 0);
#else
	class_entry = zend_register_internal_class_ex(&ce, NULL);
#endif

	return class_entry;
}

static zend_class_entry *register_class_ZendTestNS_Bar(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "ZendTestNS", "Bar", NULL);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}

static zend_class_entry *register_class_ZendTestNS_UnlikelyCompileError(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "ZendTestNS", "UnlikelyCompileError", class_ZendTestNS_UnlikelyCompileError_methods);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, 0);
#else
	class_entry = zend_register_internal_class_ex(&ce, NULL);
#endif

	return class_entry;
}

static zend_class_entry *register_class_ZendTestNS_NotUnlikelyCompileError(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "ZendTestNS", "NotUnlikelyCompileError", class_ZendTestNS_NotUnlikelyCompileError_methods);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, 0);
#else
	class_entry = zend_register_internal_class_ex(&ce, NULL);
#endif

	return class_entry;
}

static zend_class_entry *register_class_ZendTestNS2_Foo(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "ZendTestNS2", "Foo", class_ZendTestNS2_Foo_methods);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, 0);
#else
	class_entry = zend_register_internal_class_ex(&ce, NULL);
#endif

	zval property_foo_default_value;
	ZVAL_NULL(&property_foo_default_value);
	zend_string *property_foo_name = zend_string_init("foo", sizeof("foo") - 1, true);
	zend_declare_property_ex(class_entry, property_foo_name, &property_foo_default_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release_ex(property_foo_name, true);

	zval property_intersectionProp_default_value;
	ZVAL_NULL(&property_intersectionProp_default_value);
	zend_string *property_intersectionProp_name = zend_string_init("intersectionProp", sizeof("intersectionProp") - 1, true);
	zend_declare_property_ex(class_entry, property_intersectionProp_name, &property_intersectionProp_default_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release_ex(property_intersectionProp_name, true);

	zval property_unionProp_default_value;
	ZVAL_NULL(&property_unionProp_default_value);
	zend_string *property_unionProp_name = zend_string_init("unionProp", sizeof("unionProp") - 1, true);
	zend_declare_property_ex(class_entry, property_unionProp_name, &property_unionProp_default_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release_ex(property_unionProp_name, true);

	return class_entry;
}

static zend_class_entry *register_class_ZendTestNS2_ZendSubNS_Foo(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "ZendTestNS2\\ZendSubNS", "Foo", class_ZendTestNS2_ZendSubNS_Foo_methods);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, 0);
#else
	class_entry = zend_register_internal_class_ex(&ce, NULL);
#endif

	return class_entry;
}
