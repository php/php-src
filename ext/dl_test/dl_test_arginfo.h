/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 2dbacf5282b0f8e53923ac70495c2da43c7237e3 */

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

static zend_class_entry *register_class_DlTest(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DlTest", class_DlTest_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	return class_entry;
}
