/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 39ccf8141b0eb785cafd490b420f2e99d6a7a66d */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dl_test_test1, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dl_test_test2, 0, 0, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, str, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()


ZEND_FUNCTION(dl_test_test1);
ZEND_FUNCTION(dl_test_test2);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(dl_test_test1, arginfo_dl_test_test1)
	ZEND_FE(dl_test_test2, arginfo_dl_test_test2)
	ZEND_FE_END
};
