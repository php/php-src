/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 30e0888832979a13a1121f2bab45ca50462d6d94 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_JsonParser_parse, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, json, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, associative, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, depth, IS_LONG, 0, "512")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_JsonParser_isValid, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, json, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_JsonParser_getKeyValue, 0, 2, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, json, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, associative, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, depth, IS_LONG, 0, "512")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_JsonParser_getKeyCount, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, json, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, depth, IS_LONG, 0, "512")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_JsonParser_keyExists, 0, 2, _IS_BOOL, 1)
	ZEND_ARG_TYPE_INFO(0, json, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, depth, IS_LONG, 0, "512")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_JsonEncoder_encode, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, depth, IS_LONG, 0, "512")
ZEND_END_ARG_INFO()


ZEND_METHOD(JsonParser, parse);
ZEND_METHOD(JsonParser, isValid);
ZEND_METHOD(JsonParser, getKeyValue);
ZEND_METHOD(JsonParser, getKeyCount);
ZEND_METHOD(JsonParser, keyExists);
ZEND_METHOD(JsonEncoder, encode);


static const zend_function_entry class_JsonParser_methods[] = {
	ZEND_ME(JsonParser, parse, arginfo_class_JsonParser_parse, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(JsonParser, isValid, arginfo_class_JsonParser_isValid, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(JsonParser, getKeyValue, arginfo_class_JsonParser_getKeyValue, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(JsonParser, getKeyCount, arginfo_class_JsonParser_getKeyCount, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(JsonParser, keyExists, arginfo_class_JsonParser_keyExists, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};


static const zend_function_entry class_JsonEncoder_methods[] = {
	ZEND_ME(JsonEncoder, encode, arginfo_class_JsonEncoder_encode, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};
