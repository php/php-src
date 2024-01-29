/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 547ddbc21e9aa853b491cb17e902bbbb9cc2df00 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dl_test_test1, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dl_test_test2, 0, 0, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, str, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_FUNCTION(dl_test_test1);
ZEND_FUNCTION(dl_test_test2);

static const zend_function_entry ext_functions[] = {
	ZEND_RAW_FENTRY("dl_test_test1", zif_dl_test_test1, arginfo_dl_test_test1, 0, NULL)
	ZEND_RAW_FENTRY("dl_test_test2", zif_dl_test_test2, arginfo_dl_test_test2, 0, NULL)
	ZEND_FE_END
};
