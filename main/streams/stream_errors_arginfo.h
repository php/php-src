/* This is a generated file, edit stream_errors.stub.php instead.
 * Stub hash: 425ea51156dba004bcc2a04ef916439e83e74f4a
 * Has decl header: yes */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_StreamException_getErrors, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

static ZEND_METHOD(StreamException, getErrors);

static const zend_function_entry class_StreamException_methods[] = {
	ZEND_ME(StreamException, getErrors, arginfo_class_StreamException_getErrors, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_StreamErrorCode(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("StreamErrorCode", IS_UNDEF, NULL);

	zend_enum_add_case_cstr(class_entry, "None", NULL);

	zend_enum_add_case_cstr(class_entry, "Generic", NULL);

	zend_enum_add_case_cstr(class_entry, "ReadFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "WriteFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "SeekFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "SeekNotSupported", NULL);

	zend_enum_add_case_cstr(class_entry, "FlushFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "TruncateFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "ConnectFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "BindFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "ListenFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "NotWritable", NULL);

	zend_enum_add_case_cstr(class_entry, "NotReadable", NULL);

	zend_enum_add_case_cstr(class_entry, "Disabled", NULL);

	zend_enum_add_case_cstr(class_entry, "NotFound", NULL);

	zend_enum_add_case_cstr(class_entry, "PermissionDenied", NULL);

	zend_enum_add_case_cstr(class_entry, "AlreadyExists", NULL);

	zend_enum_add_case_cstr(class_entry, "InvalidPath", NULL);

	zend_enum_add_case_cstr(class_entry, "PathTooLong", NULL);

	zend_enum_add_case_cstr(class_entry, "OpenFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "CreateFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "DupFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "UnlinkFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "RenameFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "MkdirFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "RmdirFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "StatFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "MetaFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "ChmodFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "ChownFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "CopyFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "TouchFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "InvalidMode", NULL);

	zend_enum_add_case_cstr(class_entry, "InvalidMeta", NULL);

	zend_enum_add_case_cstr(class_entry, "ModeNotSupported", NULL);

	zend_enum_add_case_cstr(class_entry, "Readonly", NULL);

	zend_enum_add_case_cstr(class_entry, "RecursionDetected", NULL);

	zend_enum_add_case_cstr(class_entry, "NotImplemented", NULL);

	zend_enum_add_case_cstr(class_entry, "NoOpener", NULL);

	zend_enum_add_case_cstr(class_entry, "PersistentNotSupported", NULL);

	zend_enum_add_case_cstr(class_entry, "WrapperNotFound", NULL);

	zend_enum_add_case_cstr(class_entry, "WrapperDisabled", NULL);

	zend_enum_add_case_cstr(class_entry, "ProtocolUnsupported", NULL);

	zend_enum_add_case_cstr(class_entry, "WrapperRegistrationFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "WrapperUnregistrationFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "WrapperRestorationFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "FilterNotFound", NULL);

	zend_enum_add_case_cstr(class_entry, "FilterFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "CastFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "CastNotSupported", NULL);

	zend_enum_add_case_cstr(class_entry, "MakeSeekableFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "BufferedDataLost", NULL);

	zend_enum_add_case_cstr(class_entry, "NetworkSendFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "NetworkRecvFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "SslNotSupported", NULL);

	zend_enum_add_case_cstr(class_entry, "ResumptionFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "SocketPathTooLong", NULL);

	zend_enum_add_case_cstr(class_entry, "OobNotSupported", NULL);

	zend_enum_add_case_cstr(class_entry, "ProtocolError", NULL);

	zend_enum_add_case_cstr(class_entry, "InvalidUrl", NULL);

	zend_enum_add_case_cstr(class_entry, "InvalidResponse", NULL);

	zend_enum_add_case_cstr(class_entry, "InvalidHeader", NULL);

	zend_enum_add_case_cstr(class_entry, "InvalidParam", NULL);

	zend_enum_add_case_cstr(class_entry, "RedirectLimit", NULL);

	zend_enum_add_case_cstr(class_entry, "AuthFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "TimeOut", NULL);

	zend_enum_add_case_cstr(class_entry, "ArchivingFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "EncodingFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "DecodingFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "InvalidFormat", NULL);

	zend_enum_add_case_cstr(class_entry, "AllocationFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "TemporaryFileFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "LockFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "LockNotSupported", NULL);

	zend_enum_add_case_cstr(class_entry, "UserspaceNotImplemented", NULL);

	zend_enum_add_case_cstr(class_entry, "UserspaceInvalidReturn", NULL);

	zend_enum_add_case_cstr(class_entry, "UserspaceCallFailed", NULL);

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

	zend_enum_add_case_cstr(class_entry, "NonTerminating", NULL);

	zend_enum_add_case_cstr(class_entry, "Terminating", NULL);

	zend_enum_add_case_cstr(class_entry, "All", NULL);

	return class_entry;
}

static zend_class_entry *register_class_StreamError(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "StreamError", NULL);
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

	return class_entry;
}

static zend_class_entry *register_class_StreamException(zend_class_entry *class_entry_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "StreamException", class_StreamException_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Exception, 0);

	zval property_errors_default_value;
	ZVAL_EMPTY_ARRAY(&property_errors_default_value);
	zend_string *property_errors_name = zend_string_init("errors", sizeof("errors") - 1, true);
	zend_declare_typed_property(class_entry, property_errors_name, &property_errors_default_value, ZEND_ACC_PRIVATE, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ARRAY));
	zend_string_release_ex(property_errors_name, true);

	return class_entry;
}
