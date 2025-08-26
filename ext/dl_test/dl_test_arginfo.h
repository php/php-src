/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 3c47a0da41b4548eb68c4124bd54cbac22f60c01 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dl_test_test1, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dl_test_test2, 0, 0, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, str, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

#define arginfo_class_DlTest_test arginfo_dl_test_test2

#define arginfo_class_DlTestSuperClass_test arginfo_dl_test_test2

ZEND_FUNCTION(dl_test_test1);
ZEND_FUNCTION(dl_test_test2);
ZEND_METHOD(DlTest, test);
ZEND_METHOD(DlTestSuperClass, test);

static const zend_function_entry ext_functions[] = {
	ZEND_FE(dl_test_test1, arginfo_dl_test_test1)
	ZEND_FE(dl_test_test2, arginfo_dl_test_test2)
	ZEND_FE_END
};

static const zend_function_entry class_DlTest_methods[] = {
	ZEND_ME(DlTest, test, arginfo_class_DlTest_test, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_DlTestSuperClass_methods[] = {
	ZEND_ME(DlTestSuperClass, test, arginfo_class_DlTestSuperClass_test, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static void register_dl_test_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("DL_TEST_CONST", 42, CONST_PERSISTENT);
}

static zend_class_entry *register_class_DlTest(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DlTest", class_DlTest_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, 0);

	return class_entry;
}

static zend_class_entry *register_class_DlTestSuperClass(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DlTestSuperClass", class_DlTestSuperClass_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, 0);

	zval property_a_default_value;
	ZVAL_UNDEF(&property_a_default_value);
	zend_string *property_a_name = zend_string_init("a", sizeof("a") - 1, 1);
	zend_declare_typed_property(class_entry, property_a_name, &property_a_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_a_name);

	return class_entry;
}

static zend_class_entry *register_class_DlTestSubClass(zend_class_entry *class_entry_DlTestSuperClass)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DlTestSubClass", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_DlTestSuperClass, 0);

	return class_entry;
}

static zend_class_entry *register_class_DlTestAliasedClass(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DlTestAliasedClass", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, 0);

	return class_entry;
}
