/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zend_test_array_return, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zend_test_nullable_array_return, 0, 0, IS_ARRAY, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zend_test_void_return, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_zend_test_deprecated arginfo_zend_test_void_return

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zend_create_unterminated_string, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zend_terminate_string, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(1, str, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zend_leak_variable, 0, 1, IS_VOID, 0)
	ZEND_ARG_INFO(0, variable)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zend_leak_bytes, 0, 0, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, bytes, IS_LONG, 0, "3")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class__ZendTestClass_is_object, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class__ZendTestClass___toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class__ZendTestTrait_testMethod, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(zend_test_array_return);
ZEND_FUNCTION(zend_test_nullable_array_return);
ZEND_FUNCTION(zend_test_void_return);
ZEND_FUNCTION(zend_test_deprecated);
ZEND_FUNCTION(zend_create_unterminated_string);
ZEND_FUNCTION(zend_terminate_string);
ZEND_FUNCTION(zend_leak_variable);
ZEND_FUNCTION(zend_leak_bytes);
ZEND_METHOD(_ZendTestClass, is_object);
ZEND_METHOD(_ZendTestClass, __toString);
ZEND_METHOD(_ZendTestTrait, testMethod);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(zend_test_array_return, arginfo_zend_test_array_return)
	ZEND_FE(zend_test_nullable_array_return, arginfo_zend_test_nullable_array_return)
	ZEND_FE(zend_test_void_return, arginfo_zend_test_void_return)
	ZEND_DEP_FE(zend_test_deprecated, arginfo_zend_test_deprecated)
	ZEND_FE(zend_create_unterminated_string, arginfo_zend_create_unterminated_string)
	ZEND_FE(zend_terminate_string, arginfo_zend_terminate_string)
	ZEND_FE(zend_leak_variable, arginfo_zend_leak_variable)
	ZEND_FE(zend_leak_bytes, arginfo_zend_leak_bytes)
	ZEND_FE_END
};


static const zend_function_entry class__ZendTestClass_methods[] = {
	ZEND_ME(_ZendTestClass, is_object, arginfo_class__ZendTestClass_is_object, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(_ZendTestClass, __toString, arginfo_class__ZendTestClass___toString, ZEND_ACC_PUBLIC|ZEND_ACC_DEPRECATED)
	ZEND_FE_END
};


static const zend_function_entry class__ZendTestTrait_methods[] = {
	ZEND_ME(_ZendTestTrait, testMethod, arginfo_class__ZendTestTrait_testMethod, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
