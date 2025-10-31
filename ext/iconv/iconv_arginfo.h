/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 2bab7d4e330ab8200a072a23d98816cef0c7ba9c */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_iconv_strlen, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_iconv_substr, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_iconv_strpos, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, haystack, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, needle, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_iconv_strrpos, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, haystack, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, needle, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_iconv_mime_encode, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, field_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, field_value, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_iconv_mime_decode, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_iconv_mime_decode_headers, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, headers, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_iconv, 0, 3, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, from_encoding, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, to_encoding, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_iconv_set_encoding, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, type, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_iconv_get_encoding, 0, 0, MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_STRING, 0, "\"all\"")
ZEND_END_ARG_INFO()

ZEND_FUNCTION(iconv_strlen);
ZEND_FUNCTION(iconv_substr);
ZEND_FUNCTION(iconv_strpos);
ZEND_FUNCTION(iconv_strrpos);
ZEND_FUNCTION(iconv_mime_encode);
ZEND_FUNCTION(iconv_mime_decode);
ZEND_FUNCTION(iconv_mime_decode_headers);
ZEND_FUNCTION(iconv);
ZEND_FUNCTION(iconv_set_encoding);
ZEND_FUNCTION(iconv_get_encoding);

static const zend_function_entry ext_functions[] = {
	ZEND_FE(iconv_strlen, arginfo_iconv_strlen)
	ZEND_FE(iconv_substr, arginfo_iconv_substr)
	ZEND_FE(iconv_strpos, arginfo_iconv_strpos)
	ZEND_FE(iconv_strrpos, arginfo_iconv_strrpos)
	ZEND_FE(iconv_mime_encode, arginfo_iconv_mime_encode)
	ZEND_FE(iconv_mime_decode, arginfo_iconv_mime_decode)
	ZEND_FE(iconv_mime_decode_headers, arginfo_iconv_mime_decode_headers)
	ZEND_FE(iconv, arginfo_iconv)
	ZEND_RAW_FENTRY("iconv_set_encoding", zif_iconv_set_encoding, arginfo_iconv_set_encoding, ZEND_ACC_DEPRECATED, NULL, NULL)
	ZEND_RAW_FENTRY("iconv_get_encoding", zif_iconv_get_encoding, arginfo_iconv_get_encoding, ZEND_ACC_DEPRECATED, NULL, NULL)
	ZEND_FE_END
};

static void register_iconv_symbols(int module_number)
{
	REGISTER_STRING_CONSTANT("ICONV_IMPL", PHP_ICONV_IMPL_VALUE, CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("ICONV_VERSION", get_iconv_version(), CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ICONV_MIME_DECODE_STRICT", PHP_ICONV_MIME_DECODE_STRICT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ICONV_MIME_DECODE_CONTINUE_ON_ERROR", PHP_ICONV_MIME_DECODE_CONTINUE_ON_ERROR, CONST_PERSISTENT);


	zend_attribute *attribute_Deprecated_func_iconv_set_encoding_0 = zend_add_function_attribute(zend_hash_str_find_ptr(CG(function_table), "iconv_set_encoding", sizeof("iconv_set_encoding") - 1), ZSTR_KNOWN(ZEND_STR_DEPRECATED_CAPITALIZED), 2);
	ZVAL_STR(&attribute_Deprecated_func_iconv_set_encoding_0->args[0].value, ZSTR_KNOWN(ZEND_STR_8_DOT_5));
	attribute_Deprecated_func_iconv_set_encoding_0->args[0].name = ZSTR_KNOWN(ZEND_STR_SINCE);
	zend_string *attribute_Deprecated_func_iconv_set_encoding_0_arg1_str = zend_string_init("use internal_encoding, input_encoding, and output_encoding INI settings instead", strlen("use internal_encoding, input_encoding, and output_encoding INI settings instead"), 1);
	ZVAL_STR(&attribute_Deprecated_func_iconv_set_encoding_0->args[1].value, attribute_Deprecated_func_iconv_set_encoding_0_arg1_str);
	attribute_Deprecated_func_iconv_set_encoding_0->args[1].name = ZSTR_KNOWN(ZEND_STR_MESSAGE);

	zend_attribute *attribute_Deprecated_func_iconv_get_encoding_0 = zend_add_function_attribute(zend_hash_str_find_ptr(CG(function_table), "iconv_get_encoding", sizeof("iconv_get_encoding") - 1), ZSTR_KNOWN(ZEND_STR_DEPRECATED_CAPITALIZED), 2);
	ZVAL_STR(&attribute_Deprecated_func_iconv_get_encoding_0->args[0].value, ZSTR_KNOWN(ZEND_STR_8_DOT_5));
	attribute_Deprecated_func_iconv_get_encoding_0->args[0].name = ZSTR_KNOWN(ZEND_STR_SINCE);
	ZVAL_STR_COPY(&attribute_Deprecated_func_iconv_get_encoding_0->args[1].value, attribute_Deprecated_func_iconv_set_encoding_0_arg1_str);
	attribute_Deprecated_func_iconv_get_encoding_0->args[1].name = ZSTR_KNOWN(ZEND_STR_MESSAGE);
}
