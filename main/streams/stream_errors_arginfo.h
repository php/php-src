/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: c9a7e6c0b34de0c6ee76c2a9d80c50deafc8079a */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_StreamErrorCode_isIoError, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_StreamErrorCode_isFileSystemError arginfo_class_StreamErrorCode_isIoError

#define arginfo_class_StreamErrorCode_isWrapperError arginfo_class_StreamErrorCode_isIoError

#define arginfo_class_StreamErrorCode_isFilterError arginfo_class_StreamErrorCode_isIoError

#define arginfo_class_StreamErrorCode_isCastError arginfo_class_StreamErrorCode_isIoError

#define arginfo_class_StreamErrorCode_isNetworkError arginfo_class_StreamErrorCode_isIoError

#define arginfo_class_StreamErrorCode_isEncodingError arginfo_class_StreamErrorCode_isIoError

#define arginfo_class_StreamErrorCode_isResourceError arginfo_class_StreamErrorCode_isIoError

#define arginfo_class_StreamErrorCode_isLockError arginfo_class_StreamErrorCode_isIoError

#define arginfo_class_StreamErrorCode_isUserspaceError arginfo_class_StreamErrorCode_isIoError

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_StreamError_hasCode, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, code, StreamErrorCode, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_StreamError_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_StreamException_getError, 0, 0, StreamError, 1)
ZEND_END_ARG_INFO()

static ZEND_METHOD(StreamErrorCode, isIoError);
static ZEND_METHOD(StreamErrorCode, isFileSystemError);
static ZEND_METHOD(StreamErrorCode, isWrapperError);
static ZEND_METHOD(StreamErrorCode, isFilterError);
static ZEND_METHOD(StreamErrorCode, isCastError);
static ZEND_METHOD(StreamErrorCode, isNetworkError);
static ZEND_METHOD(StreamErrorCode, isEncodingError);
static ZEND_METHOD(StreamErrorCode, isResourceError);
static ZEND_METHOD(StreamErrorCode, isLockError);
static ZEND_METHOD(StreamErrorCode, isUserspaceError);
static ZEND_METHOD(StreamError, hasCode);
static ZEND_METHOD(StreamError, count);
static ZEND_METHOD(StreamException, getError);

static const zend_function_entry class_StreamErrorCode_methods[] = {
	ZEND_ME(StreamErrorCode, isIoError, arginfo_class_StreamErrorCode_isIoError, ZEND_ACC_PUBLIC)
	ZEND_ME(StreamErrorCode, isFileSystemError, arginfo_class_StreamErrorCode_isFileSystemError, ZEND_ACC_PUBLIC)
	ZEND_ME(StreamErrorCode, isWrapperError, arginfo_class_StreamErrorCode_isWrapperError, ZEND_ACC_PUBLIC)
	ZEND_ME(StreamErrorCode, isFilterError, arginfo_class_StreamErrorCode_isFilterError, ZEND_ACC_PUBLIC)
	ZEND_ME(StreamErrorCode, isCastError, arginfo_class_StreamErrorCode_isCastError, ZEND_ACC_PUBLIC)
	ZEND_ME(StreamErrorCode, isNetworkError, arginfo_class_StreamErrorCode_isNetworkError, ZEND_ACC_PUBLIC)
	ZEND_ME(StreamErrorCode, isEncodingError, arginfo_class_StreamErrorCode_isEncodingError, ZEND_ACC_PUBLIC)
	ZEND_ME(StreamErrorCode, isResourceError, arginfo_class_StreamErrorCode_isResourceError, ZEND_ACC_PUBLIC)
	ZEND_ME(StreamErrorCode, isLockError, arginfo_class_StreamErrorCode_isLockError, ZEND_ACC_PUBLIC)
	ZEND_ME(StreamErrorCode, isUserspaceError, arginfo_class_StreamErrorCode_isUserspaceError, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_StreamError_methods[] = {
	ZEND_ME(StreamError, hasCode, arginfo_class_StreamError_hasCode, ZEND_ACC_PUBLIC)
	ZEND_ME(StreamError, count, arginfo_class_StreamError_count, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_StreamException_methods[] = {
	ZEND_ME(StreamException, getError, arginfo_class_StreamException_getError, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_StreamErrorCode(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("StreamErrorCode", IS_LONG, class_StreamErrorCode_methods);

	return class_entry;
}

static zend_class_entry *register_class_StreamErrorMode(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("StreamErrorMode", IS_UNDEF, NULL);

	zend_enum_add_case_cstr(class_entry, "Error", NULL);

	zend_enum_add_case_cstr(class_entry, "Exception", NULL);

	zend_enum_add_case_cstr(class_entry, "Silent", NULL);

	return class_entry;
}

static zend_class_entry *register_class_StreamErrorStore(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("StreamErrorStore", IS_UNDEF, NULL);

	zend_enum_add_case_cstr(class_entry, "Auto", NULL);

	zend_enum_add_case_cstr(class_entry, "None", NULL);

	zend_enum_add_case_cstr(class_entry, "NonTerminal", NULL);

	zend_enum_add_case_cstr(class_entry, "Terminal", NULL);

	zend_enum_add_case_cstr(class_entry, "All", NULL);

	return class_entry;
}

static zend_class_entry *register_class_StreamError(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "StreamError", class_StreamError_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_READONLY_CLASS);

	zval property_code_default_value;
	ZVAL_UNDEF(&property_code_default_value);
	zend_string *property_code_class_StreamErrorCode = zend_string_init("StreamErrorCode", sizeof("StreamErrorCode")-1, 1);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_CODE), &property_code_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_code_class_StreamErrorCode, 0, 0));

	zval property_message_default_value;
	ZVAL_UNDEF(&property_message_default_value);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_MESSAGE), &property_message_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));

	zval property_wrapperName_default_value;
	ZVAL_UNDEF(&property_wrapperName_default_value);
	zend_string *property_wrapperName_name = zend_string_init("wrapperName", sizeof("wrapperName") - 1, true);
	zend_declare_typed_property(class_entry, property_wrapperName_name, &property_wrapperName_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release_ex(property_wrapperName_name, true);

	zval property_severity_default_value;
	ZVAL_UNDEF(&property_severity_default_value);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_SEVERITY), &property_severity_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));

	zval property_terminating_default_value;
	ZVAL_UNDEF(&property_terminating_default_value);
	zend_string *property_terminating_name = zend_string_init("terminating", sizeof("terminating") - 1, true);
	zend_declare_typed_property(class_entry, property_terminating_name, &property_terminating_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release_ex(property_terminating_name, true);

	zval property_param_default_value;
	ZVAL_UNDEF(&property_param_default_value);
	zend_string *property_param_name = zend_string_init("param", sizeof("param") - 1, true);
	zend_declare_typed_property(class_entry, property_param_name, &property_param_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release_ex(property_param_name, true);

	zval property_next_default_value;
	ZVAL_UNDEF(&property_next_default_value);
	zend_string *property_next_name = zend_string_init("next", sizeof("next") - 1, true);
	zend_string *property_next_class_StreamError = zend_string_init("StreamError", sizeof("StreamError")-1, 1);
	zend_declare_typed_property(class_entry, property_next_name, &property_next_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_next_class_StreamError, 0, MAY_BE_NULL));
	zend_string_release_ex(property_next_name, true);

	return class_entry;
}

static zend_class_entry *register_class_StreamException(zend_class_entry *class_entry_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "StreamException", class_StreamException_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Exception, 0);

	zval property_error_default_value;
	ZVAL_NULL(&property_error_default_value);
	zend_string *property_error_name = zend_string_init("error", sizeof("error") - 1, true);
	zend_string *property_error_class_StreamError = zend_string_init("StreamError", sizeof("StreamError")-1, 1);
	zend_declare_typed_property(class_entry, property_error_name, &property_error_default_value, ZEND_ACC_PRIVATE, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_error_class_StreamError, 0, MAY_BE_NULL));
	zend_string_release_ex(property_error_name, true);

	return class_entry;
}
