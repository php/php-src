/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: fa6eadc4164fb7b0d9f7d873d47205bbe5a46cd8 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dl_test_test1, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dl_test_test2, 0, 0, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, str, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

#define arginfo_class_DlTest_test arginfo_dl_test_test2

ZEND_FUNCTION(dl_test_test1);
ZEND_FUNCTION(dl_test_test2);
ZEND_METHOD(DlTest, test);

static const zend_function_entry ext_functions[] = {
	ZEND_FE(dl_test_test1, arginfo_dl_test_test1)
	ZEND_FE(dl_test_test2, arginfo_dl_test_test2)
	ZEND_FE_END
};

static const zend_function_entry class_DlTest_methods[] = {
	ZEND_ME(DlTest, test, arginfo_class_DlTest_test, ZEND_ACC_PUBLIC)
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
