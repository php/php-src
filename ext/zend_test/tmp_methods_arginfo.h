/* This is a generated file, edit tmp_methods.stub.php instead.
 * Stub hash: 7fd99c0b5a1957cb3a8c08a74421a720475bb46d */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ZendTestTmpMethods_testTmpMethodWithArgInfo, 0, 0, IS_VOID, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, tmpMethodParamName, Foo|Bar, MAY_BE_NULL, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, tmpMethodParamWithStringDefaultValue, IS_STRING, 0, "\"tmpMethodParamWithStringDefaultValue\"")
ZEND_END_ARG_INFO()

ZEND_METHOD(ZendTestTmpMethods, testTmpMethodWithArgInfo);

static const zend_function_entry class_ZendTestTmpMethods_methods[] = {
	ZEND_ME(ZendTestTmpMethods, testTmpMethodWithArgInfo, arginfo_class_ZendTestTmpMethods_testTmpMethodWithArgInfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
