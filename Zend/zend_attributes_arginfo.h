/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 9cd28e1ad07c22dc16341b5ee781f0c29f689307 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Attribute___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "Attribute::TARGET_ALL")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Deprecated___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, message, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()


ZEND_METHOD(Attribute, __construct);
ZEND_METHOD(Deprecated, __construct);


static const zend_function_entry class_Attribute_methods[] = {
	ZEND_ME(Attribute, __construct, arginfo_class_Attribute___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_Deprecated_methods[] = {
	ZEND_ME(Deprecated, __construct, arginfo_class_Deprecated___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
