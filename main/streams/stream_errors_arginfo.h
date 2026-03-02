/* This is a generated file, edit stream_errors.stub.php instead.
 * Stub hash: d6862fd09e088b7eb6cff766e812bb7246525b76 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_StreamError_isIoError, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_StreamError_isFileSystemError arginfo_class_StreamError_isIoError

#define arginfo_class_StreamError_isWrapperError arginfo_class_StreamError_isIoError

#define arginfo_class_StreamError_isFilterError arginfo_class_StreamError_isIoError

#define arginfo_class_StreamError_isCastError arginfo_class_StreamError_isIoError

#define arginfo_class_StreamError_isNetworkError arginfo_class_StreamError_isIoError

#define arginfo_class_StreamError_isEncodingError arginfo_class_StreamError_isIoError

#define arginfo_class_StreamError_isResourceError arginfo_class_StreamError_isIoError

#define arginfo_class_StreamError_isLockError arginfo_class_StreamError_isIoError

#define arginfo_class_StreamError_isUserspaceError arginfo_class_StreamError_isIoError

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_StreamError_hasCode, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, code, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_StreamError_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_StreamException_getError, 0, 0, StreamError, 1)
ZEND_END_ARG_INFO()

static ZEND_METHOD(StreamError, isIoError);
static ZEND_METHOD(StreamError, isFileSystemError);
static ZEND_METHOD(StreamError, isWrapperError);
static ZEND_METHOD(StreamError, isFilterError);
static ZEND_METHOD(StreamError, isCastError);
static ZEND_METHOD(StreamError, isNetworkError);
static ZEND_METHOD(StreamError, isEncodingError);
static ZEND_METHOD(StreamError, isResourceError);
static ZEND_METHOD(StreamError, isLockError);
static ZEND_METHOD(StreamError, isUserspaceError);
static ZEND_METHOD(StreamError, hasCode);
static ZEND_METHOD(StreamError, count);
static ZEND_METHOD(StreamException, getError);

static const zend_function_entry class_StreamError_methods[] = {
	ZEND_ME(StreamError, isIoError, arginfo_class_StreamError_isIoError, ZEND_ACC_PUBLIC)
	ZEND_ME(StreamError, isFileSystemError, arginfo_class_StreamError_isFileSystemError, ZEND_ACC_PUBLIC)
	ZEND_ME(StreamError, isWrapperError, arginfo_class_StreamError_isWrapperError, ZEND_ACC_PUBLIC)
	ZEND_ME(StreamError, isFilterError, arginfo_class_StreamError_isFilterError, ZEND_ACC_PUBLIC)
	ZEND_ME(StreamError, isCastError, arginfo_class_StreamError_isCastError, ZEND_ACC_PUBLIC)
	ZEND_ME(StreamError, isNetworkError, arginfo_class_StreamError_isNetworkError, ZEND_ACC_PUBLIC)
	ZEND_ME(StreamError, isEncodingError, arginfo_class_StreamError_isEncodingError, ZEND_ACC_PUBLIC)
	ZEND_ME(StreamError, isResourceError, arginfo_class_StreamError_isResourceError, ZEND_ACC_PUBLIC)
	ZEND_ME(StreamError, isLockError, arginfo_class_StreamError_isLockError, ZEND_ACC_PUBLIC)
	ZEND_ME(StreamError, isUserspaceError, arginfo_class_StreamError_isUserspaceError, ZEND_ACC_PUBLIC)
	ZEND_ME(StreamError, hasCode, arginfo_class_StreamError_hasCode, ZEND_ACC_PUBLIC)
	ZEND_ME(StreamError, count, arginfo_class_StreamError_count, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_StreamException_methods[] = {
	ZEND_ME(StreamException, getError, arginfo_class_StreamException_getError, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

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

	INIT_CLASS_ENTRY(ce, "StreamError", class_StreamError_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_READONLY_CLASS);

	zval const_CODE_NONE_value;
	ZVAL_LONG(&const_CODE_NONE_value, STREAM_ERROR_CODE_NONE);
	zend_string *const_CODE_NONE_name = zend_string_init_interned("CODE_NONE", sizeof("CODE_NONE") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_NONE_name, &const_CODE_NONE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_NONE_name, true);

	zval const_CODE_GENERIC_value;
	ZVAL_LONG(&const_CODE_GENERIC_value, STREAM_ERROR_CODE_GENERIC);
	zend_string *const_CODE_GENERIC_name = zend_string_init_interned("CODE_GENERIC", sizeof("CODE_GENERIC") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_GENERIC_name, &const_CODE_GENERIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_GENERIC_name, true);

	zval const_CODE_READ_FAILED_value;
	ZVAL_LONG(&const_CODE_READ_FAILED_value, STREAM_ERROR_CODE_READ_FAILED);
	zend_string *const_CODE_READ_FAILED_name = zend_string_init_interned("CODE_READ_FAILED", sizeof("CODE_READ_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_READ_FAILED_name, &const_CODE_READ_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_READ_FAILED_name, true);

	zval const_CODE_WRITE_FAILED_value;
	ZVAL_LONG(&const_CODE_WRITE_FAILED_value, STREAM_ERROR_CODE_WRITE_FAILED);
	zend_string *const_CODE_WRITE_FAILED_name = zend_string_init_interned("CODE_WRITE_FAILED", sizeof("CODE_WRITE_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_WRITE_FAILED_name, &const_CODE_WRITE_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_WRITE_FAILED_name, true);

	zval const_CODE_SEEK_FAILED_value;
	ZVAL_LONG(&const_CODE_SEEK_FAILED_value, STREAM_ERROR_CODE_SEEK_FAILED);
	zend_string *const_CODE_SEEK_FAILED_name = zend_string_init_interned("CODE_SEEK_FAILED", sizeof("CODE_SEEK_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_SEEK_FAILED_name, &const_CODE_SEEK_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_SEEK_FAILED_name, true);

	zval const_CODE_SEEK_NOT_SUPPORTED_value;
	ZVAL_LONG(&const_CODE_SEEK_NOT_SUPPORTED_value, STREAM_ERROR_CODE_SEEK_NOT_SUPPORTED);
	zend_string *const_CODE_SEEK_NOT_SUPPORTED_name = zend_string_init_interned("CODE_SEEK_NOT_SUPPORTED", sizeof("CODE_SEEK_NOT_SUPPORTED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_SEEK_NOT_SUPPORTED_name, &const_CODE_SEEK_NOT_SUPPORTED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_SEEK_NOT_SUPPORTED_name, true);

	zval const_CODE_FLUSH_FAILED_value;
	ZVAL_LONG(&const_CODE_FLUSH_FAILED_value, STREAM_ERROR_CODE_FLUSH_FAILED);
	zend_string *const_CODE_FLUSH_FAILED_name = zend_string_init_interned("CODE_FLUSH_FAILED", sizeof("CODE_FLUSH_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_FLUSH_FAILED_name, &const_CODE_FLUSH_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_FLUSH_FAILED_name, true);

	zval const_CODE_TRUNCATE_FAILED_value;
	ZVAL_LONG(&const_CODE_TRUNCATE_FAILED_value, STREAM_ERROR_CODE_TRUNCATE_FAILED);
	zend_string *const_CODE_TRUNCATE_FAILED_name = zend_string_init_interned("CODE_TRUNCATE_FAILED", sizeof("CODE_TRUNCATE_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_TRUNCATE_FAILED_name, &const_CODE_TRUNCATE_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_TRUNCATE_FAILED_name, true);

	zval const_CODE_CONNECT_FAILED_value;
	ZVAL_LONG(&const_CODE_CONNECT_FAILED_value, STREAM_ERROR_CODE_CONNECT_FAILED);
	zend_string *const_CODE_CONNECT_FAILED_name = zend_string_init_interned("CODE_CONNECT_FAILED", sizeof("CODE_CONNECT_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_CONNECT_FAILED_name, &const_CODE_CONNECT_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_CONNECT_FAILED_name, true);

	zval const_CODE_BIND_FAILED_value;
	ZVAL_LONG(&const_CODE_BIND_FAILED_value, STREAM_ERROR_CODE_BIND_FAILED);
	zend_string *const_CODE_BIND_FAILED_name = zend_string_init_interned("CODE_BIND_FAILED", sizeof("CODE_BIND_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_BIND_FAILED_name, &const_CODE_BIND_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_BIND_FAILED_name, true);

	zval const_CODE_LISTEN_FAILED_value;
	ZVAL_LONG(&const_CODE_LISTEN_FAILED_value, STREAM_ERROR_CODE_LISTEN_FAILED);
	zend_string *const_CODE_LISTEN_FAILED_name = zend_string_init_interned("CODE_LISTEN_FAILED", sizeof("CODE_LISTEN_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_LISTEN_FAILED_name, &const_CODE_LISTEN_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_LISTEN_FAILED_name, true);

	zval const_CODE_NOT_WRITABLE_value;
	ZVAL_LONG(&const_CODE_NOT_WRITABLE_value, STREAM_ERROR_CODE_NOT_WRITABLE);
	zend_string *const_CODE_NOT_WRITABLE_name = zend_string_init_interned("CODE_NOT_WRITABLE", sizeof("CODE_NOT_WRITABLE") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_NOT_WRITABLE_name, &const_CODE_NOT_WRITABLE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_NOT_WRITABLE_name, true);

	zval const_CODE_NOT_READABLE_value;
	ZVAL_LONG(&const_CODE_NOT_READABLE_value, STREAM_ERROR_CODE_NOT_READABLE);
	zend_string *const_CODE_NOT_READABLE_name = zend_string_init_interned("CODE_NOT_READABLE", sizeof("CODE_NOT_READABLE") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_NOT_READABLE_name, &const_CODE_NOT_READABLE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_NOT_READABLE_name, true);

	zval const_CODE_DISABLED_value;
	ZVAL_LONG(&const_CODE_DISABLED_value, STREAM_ERROR_CODE_DISABLED);
	zend_string *const_CODE_DISABLED_name = zend_string_init_interned("CODE_DISABLED", sizeof("CODE_DISABLED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_DISABLED_name, &const_CODE_DISABLED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_DISABLED_name, true);

	zval const_CODE_NOT_FOUND_value;
	ZVAL_LONG(&const_CODE_NOT_FOUND_value, STREAM_ERROR_CODE_NOT_FOUND);
	zend_string *const_CODE_NOT_FOUND_name = zend_string_init_interned("CODE_NOT_FOUND", sizeof("CODE_NOT_FOUND") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_NOT_FOUND_name, &const_CODE_NOT_FOUND_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_NOT_FOUND_name, true);

	zval const_CODE_PERMISSION_DENIED_value;
	ZVAL_LONG(&const_CODE_PERMISSION_DENIED_value, STREAM_ERROR_CODE_PERMISSION_DENIED);
	zend_string *const_CODE_PERMISSION_DENIED_name = zend_string_init_interned("CODE_PERMISSION_DENIED", sizeof("CODE_PERMISSION_DENIED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_PERMISSION_DENIED_name, &const_CODE_PERMISSION_DENIED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_PERMISSION_DENIED_name, true);

	zval const_CODE_ALREADY_EXISTS_value;
	ZVAL_LONG(&const_CODE_ALREADY_EXISTS_value, STREAM_ERROR_CODE_ALREADY_EXISTS);
	zend_string *const_CODE_ALREADY_EXISTS_name = zend_string_init_interned("CODE_ALREADY_EXISTS", sizeof("CODE_ALREADY_EXISTS") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_ALREADY_EXISTS_name, &const_CODE_ALREADY_EXISTS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_ALREADY_EXISTS_name, true);

	zval const_CODE_INVALID_PATH_value;
	ZVAL_LONG(&const_CODE_INVALID_PATH_value, STREAM_ERROR_CODE_INVALID_PATH);
	zend_string *const_CODE_INVALID_PATH_name = zend_string_init_interned("CODE_INVALID_PATH", sizeof("CODE_INVALID_PATH") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_INVALID_PATH_name, &const_CODE_INVALID_PATH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_INVALID_PATH_name, true);

	zval const_CODE_PATH_TOO_LONG_value;
	ZVAL_LONG(&const_CODE_PATH_TOO_LONG_value, STREAM_ERROR_CODE_PATH_TOO_LONG);
	zend_string *const_CODE_PATH_TOO_LONG_name = zend_string_init_interned("CODE_PATH_TOO_LONG", sizeof("CODE_PATH_TOO_LONG") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_PATH_TOO_LONG_name, &const_CODE_PATH_TOO_LONG_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_PATH_TOO_LONG_name, true);

	zval const_CODE_OPEN_FAILED_value;
	ZVAL_LONG(&const_CODE_OPEN_FAILED_value, STREAM_ERROR_CODE_OPEN_FAILED);
	zend_string *const_CODE_OPEN_FAILED_name = zend_string_init_interned("CODE_OPEN_FAILED", sizeof("CODE_OPEN_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_OPEN_FAILED_name, &const_CODE_OPEN_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_OPEN_FAILED_name, true);

	zval const_CODE_CREATE_FAILED_value;
	ZVAL_LONG(&const_CODE_CREATE_FAILED_value, STREAM_ERROR_CODE_CREATE_FAILED);
	zend_string *const_CODE_CREATE_FAILED_name = zend_string_init_interned("CODE_CREATE_FAILED", sizeof("CODE_CREATE_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_CREATE_FAILED_name, &const_CODE_CREATE_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_CREATE_FAILED_name, true);

	zval const_CODE_DUP_FAILED_value;
	ZVAL_LONG(&const_CODE_DUP_FAILED_value, STREAM_ERROR_CODE_DUP_FAILED);
	zend_string *const_CODE_DUP_FAILED_name = zend_string_init_interned("CODE_DUP_FAILED", sizeof("CODE_DUP_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_DUP_FAILED_name, &const_CODE_DUP_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_DUP_FAILED_name, true);

	zval const_CODE_UNLINK_FAILED_value;
	ZVAL_LONG(&const_CODE_UNLINK_FAILED_value, STREAM_ERROR_CODE_UNLINK_FAILED);
	zend_string *const_CODE_UNLINK_FAILED_name = zend_string_init_interned("CODE_UNLINK_FAILED", sizeof("CODE_UNLINK_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_UNLINK_FAILED_name, &const_CODE_UNLINK_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_UNLINK_FAILED_name, true);

	zval const_CODE_RENAME_FAILED_value;
	ZVAL_LONG(&const_CODE_RENAME_FAILED_value, STREAM_ERROR_CODE_RENAME_FAILED);
	zend_string *const_CODE_RENAME_FAILED_name = zend_string_init_interned("CODE_RENAME_FAILED", sizeof("CODE_RENAME_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_RENAME_FAILED_name, &const_CODE_RENAME_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_RENAME_FAILED_name, true);

	zval const_CODE_MKDIR_FAILED_value;
	ZVAL_LONG(&const_CODE_MKDIR_FAILED_value, STREAM_ERROR_CODE_MKDIR_FAILED);
	zend_string *const_CODE_MKDIR_FAILED_name = zend_string_init_interned("CODE_MKDIR_FAILED", sizeof("CODE_MKDIR_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_MKDIR_FAILED_name, &const_CODE_MKDIR_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_MKDIR_FAILED_name, true);

	zval const_CODE_RMDIR_FAILED_value;
	ZVAL_LONG(&const_CODE_RMDIR_FAILED_value, STREAM_ERROR_CODE_RMDIR_FAILED);
	zend_string *const_CODE_RMDIR_FAILED_name = zend_string_init_interned("CODE_RMDIR_FAILED", sizeof("CODE_RMDIR_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_RMDIR_FAILED_name, &const_CODE_RMDIR_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_RMDIR_FAILED_name, true);

	zval const_CODE_STAT_FAILED_value;
	ZVAL_LONG(&const_CODE_STAT_FAILED_value, STREAM_ERROR_CODE_STAT_FAILED);
	zend_string *const_CODE_STAT_FAILED_name = zend_string_init_interned("CODE_STAT_FAILED", sizeof("CODE_STAT_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_STAT_FAILED_name, &const_CODE_STAT_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_STAT_FAILED_name, true);

	zval const_CODE_META_FAILED_value;
	ZVAL_LONG(&const_CODE_META_FAILED_value, STREAM_ERROR_CODE_META_FAILED);
	zend_string *const_CODE_META_FAILED_name = zend_string_init_interned("CODE_META_FAILED", sizeof("CODE_META_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_META_FAILED_name, &const_CODE_META_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_META_FAILED_name, true);

	zval const_CODE_CHMOD_FAILED_value;
	ZVAL_LONG(&const_CODE_CHMOD_FAILED_value, STREAM_ERROR_CODE_CHMOD_FAILED);
	zend_string *const_CODE_CHMOD_FAILED_name = zend_string_init_interned("CODE_CHMOD_FAILED", sizeof("CODE_CHMOD_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_CHMOD_FAILED_name, &const_CODE_CHMOD_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_CHMOD_FAILED_name, true);

	zval const_CODE_CHOWN_FAILED_value;
	ZVAL_LONG(&const_CODE_CHOWN_FAILED_value, STREAM_ERROR_CODE_CHOWN_FAILED);
	zend_string *const_CODE_CHOWN_FAILED_name = zend_string_init_interned("CODE_CHOWN_FAILED", sizeof("CODE_CHOWN_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_CHOWN_FAILED_name, &const_CODE_CHOWN_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_CHOWN_FAILED_name, true);

	zval const_CODE_COPY_FAILED_value;
	ZVAL_LONG(&const_CODE_COPY_FAILED_value, STREAM_ERROR_CODE_COPY_FAILED);
	zend_string *const_CODE_COPY_FAILED_name = zend_string_init_interned("CODE_COPY_FAILED", sizeof("CODE_COPY_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_COPY_FAILED_name, &const_CODE_COPY_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_COPY_FAILED_name, true);

	zval const_CODE_TOUCH_FAILED_value;
	ZVAL_LONG(&const_CODE_TOUCH_FAILED_value, STREAM_ERROR_CODE_TOUCH_FAILED);
	zend_string *const_CODE_TOUCH_FAILED_name = zend_string_init_interned("CODE_TOUCH_FAILED", sizeof("CODE_TOUCH_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_TOUCH_FAILED_name, &const_CODE_TOUCH_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_TOUCH_FAILED_name, true);

	zval const_CODE_INVALID_MODE_value;
	ZVAL_LONG(&const_CODE_INVALID_MODE_value, STREAM_ERROR_CODE_INVALID_MODE);
	zend_string *const_CODE_INVALID_MODE_name = zend_string_init_interned("CODE_INVALID_MODE", sizeof("CODE_INVALID_MODE") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_INVALID_MODE_name, &const_CODE_INVALID_MODE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_INVALID_MODE_name, true);

	zval const_CODE_INVALID_META_value;
	ZVAL_LONG(&const_CODE_INVALID_META_value, STREAM_ERROR_CODE_INVALID_META);
	zend_string *const_CODE_INVALID_META_name = zend_string_init_interned("CODE_INVALID_META", sizeof("CODE_INVALID_META") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_INVALID_META_name, &const_CODE_INVALID_META_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_INVALID_META_name, true);

	zval const_CODE_MODE_NOT_SUPPORTED_value;
	ZVAL_LONG(&const_CODE_MODE_NOT_SUPPORTED_value, STREAM_ERROR_CODE_MODE_NOT_SUPPORTED);
	zend_string *const_CODE_MODE_NOT_SUPPORTED_name = zend_string_init_interned("CODE_MODE_NOT_SUPPORTED", sizeof("CODE_MODE_NOT_SUPPORTED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_MODE_NOT_SUPPORTED_name, &const_CODE_MODE_NOT_SUPPORTED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_MODE_NOT_SUPPORTED_name, true);

	zval const_CODE_READONLY_value;
	ZVAL_LONG(&const_CODE_READONLY_value, STREAM_ERROR_CODE_READONLY);
	zend_string *const_CODE_READONLY_name = zend_string_init_interned("CODE_READONLY", sizeof("CODE_READONLY") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_READONLY_name, &const_CODE_READONLY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_READONLY_name, true);

	zval const_CODE_RECURSION_DETECTED_value;
	ZVAL_LONG(&const_CODE_RECURSION_DETECTED_value, STREAM_ERROR_CODE_RECURSION_DETECTED);
	zend_string *const_CODE_RECURSION_DETECTED_name = zend_string_init_interned("CODE_RECURSION_DETECTED", sizeof("CODE_RECURSION_DETECTED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_RECURSION_DETECTED_name, &const_CODE_RECURSION_DETECTED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_RECURSION_DETECTED_name, true);

	zval const_CODE_NOT_IMPLEMENTED_value;
	ZVAL_LONG(&const_CODE_NOT_IMPLEMENTED_value, STREAM_ERROR_CODE_NOT_IMPLEMENTED);
	zend_string *const_CODE_NOT_IMPLEMENTED_name = zend_string_init_interned("CODE_NOT_IMPLEMENTED", sizeof("CODE_NOT_IMPLEMENTED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_NOT_IMPLEMENTED_name, &const_CODE_NOT_IMPLEMENTED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_NOT_IMPLEMENTED_name, true);

	zval const_CODE_NO_OPENER_value;
	ZVAL_LONG(&const_CODE_NO_OPENER_value, STREAM_ERROR_CODE_NO_OPENER);
	zend_string *const_CODE_NO_OPENER_name = zend_string_init_interned("CODE_NO_OPENER", sizeof("CODE_NO_OPENER") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_NO_OPENER_name, &const_CODE_NO_OPENER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_NO_OPENER_name, true);

	zval const_CODE_PERSISTENT_NOT_SUPPORTED_value;
	ZVAL_LONG(&const_CODE_PERSISTENT_NOT_SUPPORTED_value, STREAM_ERROR_CODE_PERSISTENT_NOT_SUPPORTED);
	zend_string *const_CODE_PERSISTENT_NOT_SUPPORTED_name = zend_string_init_interned("CODE_PERSISTENT_NOT_SUPPORTED", sizeof("CODE_PERSISTENT_NOT_SUPPORTED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_PERSISTENT_NOT_SUPPORTED_name, &const_CODE_PERSISTENT_NOT_SUPPORTED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_PERSISTENT_NOT_SUPPORTED_name, true);

	zval const_CODE_WRAPPER_NOT_FOUND_value;
	ZVAL_LONG(&const_CODE_WRAPPER_NOT_FOUND_value, STREAM_ERROR_CODE_WRAPPER_NOT_FOUND);
	zend_string *const_CODE_WRAPPER_NOT_FOUND_name = zend_string_init_interned("CODE_WRAPPER_NOT_FOUND", sizeof("CODE_WRAPPER_NOT_FOUND") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_WRAPPER_NOT_FOUND_name, &const_CODE_WRAPPER_NOT_FOUND_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_WRAPPER_NOT_FOUND_name, true);

	zval const_CODE_WRAPPER_DISABLED_value;
	ZVAL_LONG(&const_CODE_WRAPPER_DISABLED_value, STREAM_ERROR_CODE_WRAPPER_DISABLED);
	zend_string *const_CODE_WRAPPER_DISABLED_name = zend_string_init_interned("CODE_WRAPPER_DISABLED", sizeof("CODE_WRAPPER_DISABLED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_WRAPPER_DISABLED_name, &const_CODE_WRAPPER_DISABLED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_WRAPPER_DISABLED_name, true);

	zval const_CODE_PROTOCOL_UNSUPPORTED_value;
	ZVAL_LONG(&const_CODE_PROTOCOL_UNSUPPORTED_value, STREAM_ERROR_CODE_PROTOCOL_UNSUPPORTED);
	zend_string *const_CODE_PROTOCOL_UNSUPPORTED_name = zend_string_init_interned("CODE_PROTOCOL_UNSUPPORTED", sizeof("CODE_PROTOCOL_UNSUPPORTED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_PROTOCOL_UNSUPPORTED_name, &const_CODE_PROTOCOL_UNSUPPORTED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_PROTOCOL_UNSUPPORTED_name, true);

	zval const_CODE_WRAPPER_REGISTRATION_FAILED_value;
	ZVAL_LONG(&const_CODE_WRAPPER_REGISTRATION_FAILED_value, STREAM_ERROR_CODE_WRAPPER_REGISTRATION_FAILED);
	zend_string *const_CODE_WRAPPER_REGISTRATION_FAILED_name = zend_string_init_interned("CODE_WRAPPER_REGISTRATION_FAILED", sizeof("CODE_WRAPPER_REGISTRATION_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_WRAPPER_REGISTRATION_FAILED_name, &const_CODE_WRAPPER_REGISTRATION_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_WRAPPER_REGISTRATION_FAILED_name, true);

	zval const_CODE_WRAPPER_UNREGISTRATION_FAILED_value;
	ZVAL_LONG(&const_CODE_WRAPPER_UNREGISTRATION_FAILED_value, STREAM_ERROR_CODE_WRAPPER_UNREGISTRATION_FAILED);
	zend_string *const_CODE_WRAPPER_UNREGISTRATION_FAILED_name = zend_string_init_interned("CODE_WRAPPER_UNREGISTRATION_FAILED", sizeof("CODE_WRAPPER_UNREGISTRATION_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_WRAPPER_UNREGISTRATION_FAILED_name, &const_CODE_WRAPPER_UNREGISTRATION_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_WRAPPER_UNREGISTRATION_FAILED_name, true);

	zval const_CODE_WRAPPER_RESTORATION_FAILED_value;
	ZVAL_LONG(&const_CODE_WRAPPER_RESTORATION_FAILED_value, STREAM_ERROR_CODE_WRAPPER_RESTORATION_FAILED);
	zend_string *const_CODE_WRAPPER_RESTORATION_FAILED_name = zend_string_init_interned("CODE_WRAPPER_RESTORATION_FAILED", sizeof("CODE_WRAPPER_RESTORATION_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_WRAPPER_RESTORATION_FAILED_name, &const_CODE_WRAPPER_RESTORATION_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_WRAPPER_RESTORATION_FAILED_name, true);

	zval const_CODE_FILTER_NOT_FOUND_value;
	ZVAL_LONG(&const_CODE_FILTER_NOT_FOUND_value, STREAM_ERROR_CODE_FILTER_NOT_FOUND);
	zend_string *const_CODE_FILTER_NOT_FOUND_name = zend_string_init_interned("CODE_FILTER_NOT_FOUND", sizeof("CODE_FILTER_NOT_FOUND") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_FILTER_NOT_FOUND_name, &const_CODE_FILTER_NOT_FOUND_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_FILTER_NOT_FOUND_name, true);

	zval const_CODE_FILTER_FAILED_value;
	ZVAL_LONG(&const_CODE_FILTER_FAILED_value, STREAM_ERROR_CODE_FILTER_FAILED);
	zend_string *const_CODE_FILTER_FAILED_name = zend_string_init_interned("CODE_FILTER_FAILED", sizeof("CODE_FILTER_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_FILTER_FAILED_name, &const_CODE_FILTER_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_FILTER_FAILED_name, true);

	zval const_CODE_CAST_FAILED_value;
	ZVAL_LONG(&const_CODE_CAST_FAILED_value, STREAM_ERROR_CODE_CAST_FAILED);
	zend_string *const_CODE_CAST_FAILED_name = zend_string_init_interned("CODE_CAST_FAILED", sizeof("CODE_CAST_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_CAST_FAILED_name, &const_CODE_CAST_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_CAST_FAILED_name, true);

	zval const_CODE_CAST_NOT_SUPPORTED_value;
	ZVAL_LONG(&const_CODE_CAST_NOT_SUPPORTED_value, STREAM_ERROR_CODE_CAST_NOT_SUPPORTED);
	zend_string *const_CODE_CAST_NOT_SUPPORTED_name = zend_string_init_interned("CODE_CAST_NOT_SUPPORTED", sizeof("CODE_CAST_NOT_SUPPORTED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_CAST_NOT_SUPPORTED_name, &const_CODE_CAST_NOT_SUPPORTED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_CAST_NOT_SUPPORTED_name, true);

	zval const_CODE_MAKE_SEEKABLE_FAILED_value;
	ZVAL_LONG(&const_CODE_MAKE_SEEKABLE_FAILED_value, STREAM_ERROR_CODE_MAKE_SEEKABLE_FAILED);
	zend_string *const_CODE_MAKE_SEEKABLE_FAILED_name = zend_string_init_interned("CODE_MAKE_SEEKABLE_FAILED", sizeof("CODE_MAKE_SEEKABLE_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_MAKE_SEEKABLE_FAILED_name, &const_CODE_MAKE_SEEKABLE_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_MAKE_SEEKABLE_FAILED_name, true);

	zval const_CODE_BUFFERED_DATA_LOST_value;
	ZVAL_LONG(&const_CODE_BUFFERED_DATA_LOST_value, STREAM_ERROR_CODE_BUFFERED_DATA_LOST);
	zend_string *const_CODE_BUFFERED_DATA_LOST_name = zend_string_init_interned("CODE_BUFFERED_DATA_LOST", sizeof("CODE_BUFFERED_DATA_LOST") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_BUFFERED_DATA_LOST_name, &const_CODE_BUFFERED_DATA_LOST_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_BUFFERED_DATA_LOST_name, true);

	zval const_CODE_NETWORK_SEND_FAILED_value;
	ZVAL_LONG(&const_CODE_NETWORK_SEND_FAILED_value, STREAM_ERROR_CODE_NETWORK_SEND_FAILED);
	zend_string *const_CODE_NETWORK_SEND_FAILED_name = zend_string_init_interned("CODE_NETWORK_SEND_FAILED", sizeof("CODE_NETWORK_SEND_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_NETWORK_SEND_FAILED_name, &const_CODE_NETWORK_SEND_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_NETWORK_SEND_FAILED_name, true);

	zval const_CODE_NETWORK_RECV_FAILED_value;
	ZVAL_LONG(&const_CODE_NETWORK_RECV_FAILED_value, STREAM_ERROR_CODE_NETWORK_RECV_FAILED);
	zend_string *const_CODE_NETWORK_RECV_FAILED_name = zend_string_init_interned("CODE_NETWORK_RECV_FAILED", sizeof("CODE_NETWORK_RECV_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_NETWORK_RECV_FAILED_name, &const_CODE_NETWORK_RECV_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_NETWORK_RECV_FAILED_name, true);

	zval const_CODE_SSL_NOT_SUPPORTED_value;
	ZVAL_LONG(&const_CODE_SSL_NOT_SUPPORTED_value, STREAM_ERROR_CODE_SSL_NOT_SUPPORTED);
	zend_string *const_CODE_SSL_NOT_SUPPORTED_name = zend_string_init_interned("CODE_SSL_NOT_SUPPORTED", sizeof("CODE_SSL_NOT_SUPPORTED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_SSL_NOT_SUPPORTED_name, &const_CODE_SSL_NOT_SUPPORTED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_SSL_NOT_SUPPORTED_name, true);

	zval const_CODE_RESUMPTION_FAILED_value;
	ZVAL_LONG(&const_CODE_RESUMPTION_FAILED_value, STREAM_ERROR_CODE_RESUMPTION_FAILED);
	zend_string *const_CODE_RESUMPTION_FAILED_name = zend_string_init_interned("CODE_RESUMPTION_FAILED", sizeof("CODE_RESUMPTION_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_RESUMPTION_FAILED_name, &const_CODE_RESUMPTION_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_RESUMPTION_FAILED_name, true);

	zval const_CODE_SOCKET_PATH_TOO_LONG_value;
	ZVAL_LONG(&const_CODE_SOCKET_PATH_TOO_LONG_value, STREAM_ERROR_CODE_SOCKET_PATH_TOO_LONG);
	zend_string *const_CODE_SOCKET_PATH_TOO_LONG_name = zend_string_init_interned("CODE_SOCKET_PATH_TOO_LONG", sizeof("CODE_SOCKET_PATH_TOO_LONG") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_SOCKET_PATH_TOO_LONG_name, &const_CODE_SOCKET_PATH_TOO_LONG_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_SOCKET_PATH_TOO_LONG_name, true);

	zval const_CODE_OOB_NOT_SUPPORTED_value;
	ZVAL_LONG(&const_CODE_OOB_NOT_SUPPORTED_value, STREAM_ERROR_CODE_OOB_NOT_SUPPORTED);
	zend_string *const_CODE_OOB_NOT_SUPPORTED_name = zend_string_init_interned("CODE_OOB_NOT_SUPPORTED", sizeof("CODE_OOB_NOT_SUPPORTED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_OOB_NOT_SUPPORTED_name, &const_CODE_OOB_NOT_SUPPORTED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_OOB_NOT_SUPPORTED_name, true);

	zval const_CODE_PROTOCOL_ERROR_value;
	ZVAL_LONG(&const_CODE_PROTOCOL_ERROR_value, STREAM_ERROR_CODE_PROTOCOL_ERROR);
	zend_string *const_CODE_PROTOCOL_ERROR_name = zend_string_init_interned("CODE_PROTOCOL_ERROR", sizeof("CODE_PROTOCOL_ERROR") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_PROTOCOL_ERROR_name, &const_CODE_PROTOCOL_ERROR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_PROTOCOL_ERROR_name, true);

	zval const_CODE_INVALID_URL_value;
	ZVAL_LONG(&const_CODE_INVALID_URL_value, STREAM_ERROR_CODE_INVALID_URL);
	zend_string *const_CODE_INVALID_URL_name = zend_string_init_interned("CODE_INVALID_URL", sizeof("CODE_INVALID_URL") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_INVALID_URL_name, &const_CODE_INVALID_URL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_INVALID_URL_name, true);

	zval const_CODE_INVALID_RESPONSE_value;
	ZVAL_LONG(&const_CODE_INVALID_RESPONSE_value, STREAM_ERROR_CODE_INVALID_RESPONSE);
	zend_string *const_CODE_INVALID_RESPONSE_name = zend_string_init_interned("CODE_INVALID_RESPONSE", sizeof("CODE_INVALID_RESPONSE") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_INVALID_RESPONSE_name, &const_CODE_INVALID_RESPONSE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_INVALID_RESPONSE_name, true);

	zval const_CODE_INVALID_HEADER_value;
	ZVAL_LONG(&const_CODE_INVALID_HEADER_value, STREAM_ERROR_CODE_INVALID_HEADER);
	zend_string *const_CODE_INVALID_HEADER_name = zend_string_init_interned("CODE_INVALID_HEADER", sizeof("CODE_INVALID_HEADER") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_INVALID_HEADER_name, &const_CODE_INVALID_HEADER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_INVALID_HEADER_name, true);

	zval const_CODE_INVALID_PARAM_value;
	ZVAL_LONG(&const_CODE_INVALID_PARAM_value, STREAM_ERROR_CODE_INVALID_PARAM);
	zend_string *const_CODE_INVALID_PARAM_name = zend_string_init_interned("CODE_INVALID_PARAM", sizeof("CODE_INVALID_PARAM") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_INVALID_PARAM_name, &const_CODE_INVALID_PARAM_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_INVALID_PARAM_name, true);

	zval const_CODE_REDIRECT_LIMIT_value;
	ZVAL_LONG(&const_CODE_REDIRECT_LIMIT_value, STREAM_ERROR_CODE_REDIRECT_LIMIT);
	zend_string *const_CODE_REDIRECT_LIMIT_name = zend_string_init_interned("CODE_REDIRECT_LIMIT", sizeof("CODE_REDIRECT_LIMIT") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_REDIRECT_LIMIT_name, &const_CODE_REDIRECT_LIMIT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_REDIRECT_LIMIT_name, true);

	zval const_CODE_AUTH_FAILED_value;
	ZVAL_LONG(&const_CODE_AUTH_FAILED_value, STREAM_ERROR_CODE_AUTH_FAILED);
	zend_string *const_CODE_AUTH_FAILED_name = zend_string_init_interned("CODE_AUTH_FAILED", sizeof("CODE_AUTH_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_AUTH_FAILED_name, &const_CODE_AUTH_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_AUTH_FAILED_name, true);

	zval const_CODE_ARCHIVING_FAILED_value;
	ZVAL_LONG(&const_CODE_ARCHIVING_FAILED_value, STREAM_ERROR_CODE_ARCHIVING_FAILED);
	zend_string *const_CODE_ARCHIVING_FAILED_name = zend_string_init_interned("CODE_ARCHIVING_FAILED", sizeof("CODE_ARCHIVING_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_ARCHIVING_FAILED_name, &const_CODE_ARCHIVING_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_ARCHIVING_FAILED_name, true);

	zval const_CODE_ENCODING_FAILED_value;
	ZVAL_LONG(&const_CODE_ENCODING_FAILED_value, STREAM_ERROR_CODE_ENCODING_FAILED);
	zend_string *const_CODE_ENCODING_FAILED_name = zend_string_init_interned("CODE_ENCODING_FAILED", sizeof("CODE_ENCODING_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_ENCODING_FAILED_name, &const_CODE_ENCODING_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_ENCODING_FAILED_name, true);

	zval const_CODE_DECODING_FAILED_value;
	ZVAL_LONG(&const_CODE_DECODING_FAILED_value, STREAM_ERROR_CODE_DECODING_FAILED);
	zend_string *const_CODE_DECODING_FAILED_name = zend_string_init_interned("CODE_DECODING_FAILED", sizeof("CODE_DECODING_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_DECODING_FAILED_name, &const_CODE_DECODING_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_DECODING_FAILED_name, true);

	zval const_CODE_INVALID_FORMAT_value;
	ZVAL_LONG(&const_CODE_INVALID_FORMAT_value, STREAM_ERROR_CODE_INVALID_FORMAT);
	zend_string *const_CODE_INVALID_FORMAT_name = zend_string_init_interned("CODE_INVALID_FORMAT", sizeof("CODE_INVALID_FORMAT") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_INVALID_FORMAT_name, &const_CODE_INVALID_FORMAT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_INVALID_FORMAT_name, true);

	zval const_CODE_ALLOCATION_FAILED_value;
	ZVAL_LONG(&const_CODE_ALLOCATION_FAILED_value, STREAM_ERROR_CODE_ALLOCATION_FAILED);
	zend_string *const_CODE_ALLOCATION_FAILED_name = zend_string_init_interned("CODE_ALLOCATION_FAILED", sizeof("CODE_ALLOCATION_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_ALLOCATION_FAILED_name, &const_CODE_ALLOCATION_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_ALLOCATION_FAILED_name, true);

	zval const_CODE_TEMPORARY_FILE_FAILED_value;
	ZVAL_LONG(&const_CODE_TEMPORARY_FILE_FAILED_value, STREAM_ERROR_CODE_TEMPORARY_FILE_FAILED);
	zend_string *const_CODE_TEMPORARY_FILE_FAILED_name = zend_string_init_interned("CODE_TEMPORARY_FILE_FAILED", sizeof("CODE_TEMPORARY_FILE_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_TEMPORARY_FILE_FAILED_name, &const_CODE_TEMPORARY_FILE_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_TEMPORARY_FILE_FAILED_name, true);

	zval const_CODE_LOCK_FAILED_value;
	ZVAL_LONG(&const_CODE_LOCK_FAILED_value, STREAM_ERROR_CODE_LOCK_FAILED);
	zend_string *const_CODE_LOCK_FAILED_name = zend_string_init_interned("CODE_LOCK_FAILED", sizeof("CODE_LOCK_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_LOCK_FAILED_name, &const_CODE_LOCK_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_LOCK_FAILED_name, true);

	zval const_CODE_LOCK_NOT_SUPPORTED_value;
	ZVAL_LONG(&const_CODE_LOCK_NOT_SUPPORTED_value, STREAM_ERROR_CODE_LOCK_NOT_SUPPORTED);
	zend_string *const_CODE_LOCK_NOT_SUPPORTED_name = zend_string_init_interned("CODE_LOCK_NOT_SUPPORTED", sizeof("CODE_LOCK_NOT_SUPPORTED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_LOCK_NOT_SUPPORTED_name, &const_CODE_LOCK_NOT_SUPPORTED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_LOCK_NOT_SUPPORTED_name, true);

	zval const_CODE_USERSPACE_NOT_IMPLEMENTED_value;
	ZVAL_LONG(&const_CODE_USERSPACE_NOT_IMPLEMENTED_value, STREAM_ERROR_CODE_USERSPACE_NOT_IMPLEMENTED);
	zend_string *const_CODE_USERSPACE_NOT_IMPLEMENTED_name = zend_string_init_interned("CODE_USERSPACE_NOT_IMPLEMENTED", sizeof("CODE_USERSPACE_NOT_IMPLEMENTED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_USERSPACE_NOT_IMPLEMENTED_name, &const_CODE_USERSPACE_NOT_IMPLEMENTED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_USERSPACE_NOT_IMPLEMENTED_name, true);

	zval const_CODE_USERSPACE_INVALID_RETURN_value;
	ZVAL_LONG(&const_CODE_USERSPACE_INVALID_RETURN_value, STREAM_ERROR_CODE_USERSPACE_INVALID_RETURN);
	zend_string *const_CODE_USERSPACE_INVALID_RETURN_name = zend_string_init_interned("CODE_USERSPACE_INVALID_RETURN", sizeof("CODE_USERSPACE_INVALID_RETURN") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_USERSPACE_INVALID_RETURN_name, &const_CODE_USERSPACE_INVALID_RETURN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_USERSPACE_INVALID_RETURN_name, true);

	zval const_CODE_USERSPACE_CALL_FAILED_value;
	ZVAL_LONG(&const_CODE_USERSPACE_CALL_FAILED_value, STREAM_ERROR_CODE_USERSPACE_CALL_FAILED);
	zend_string *const_CODE_USERSPACE_CALL_FAILED_name = zend_string_init_interned("CODE_USERSPACE_CALL_FAILED", sizeof("CODE_USERSPACE_CALL_FAILED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CODE_USERSPACE_CALL_FAILED_name, &const_CODE_USERSPACE_CALL_FAILED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CODE_USERSPACE_CALL_FAILED_name, true);

	zval property_code_default_value;
	ZVAL_UNDEF(&property_code_default_value);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_CODE), &property_code_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));

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
