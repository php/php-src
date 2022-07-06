/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 200044f4196cd8efd1ec9a2e93d4bbb63618b3c9 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_json_encode, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, depth, IS_LONG, 0, "512")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_json_decode, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, json, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, associative, _IS_BOOL, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, depth, IS_LONG, 0, "512")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_json_last_error, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_json_last_error_msg, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_JsonSerializable_jsonSerialize, 0, 0, 0)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(json_encode);
ZEND_FUNCTION(json_decode);
ZEND_FUNCTION(json_last_error);
ZEND_FUNCTION(json_last_error_msg);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(json_encode, arginfo_json_encode)
	ZEND_FE(json_decode, arginfo_json_decode)
	ZEND_FE(json_last_error, arginfo_json_last_error)
	ZEND_FE(json_last_error_msg, arginfo_json_last_error_msg)
	ZEND_FE_END
};


static const zend_function_entry class_JsonSerializable_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(JsonSerializable, jsonSerialize, arginfo_class_JsonSerializable_jsonSerialize, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};


static const zend_function_entry class_JsonException_methods[] = {
	ZEND_FE_END
};
