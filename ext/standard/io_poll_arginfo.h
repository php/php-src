/* This is a generated file, edit io_poll.stub.php instead.
 * Stub hash: aff40643f80d7f9b5a60bd676e83d48a4651875d */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Poll_Backend_getAvailableBackends, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Poll_Backend_isAvailable, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Io_Poll_Backend_supportsEdgeTriggering arginfo_class_Io_Poll_Backend_isAvailable

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Poll_Handle_getFileDescriptor, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Io_Poll_Watcher___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Io_Poll_Watcher_getHandle, 0, 0, Io\\Poll\\Handle, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Io_Poll_Watcher_getWatchedEvents arginfo_class_Io_Poll_Backend_getAvailableBackends

#define arginfo_class_Io_Poll_Watcher_getTriggeredEvents arginfo_class_Io_Poll_Backend_getAvailableBackends

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Poll_Watcher_getData, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Poll_Watcher_hasTriggered, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, event, Io\\Poll\\Event, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Io_Poll_Watcher_isActive arginfo_class_Io_Poll_Backend_isAvailable

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Poll_Watcher_modify, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, events, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, data, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Poll_Watcher_modifyEvents, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, events, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Poll_Watcher_modifyData, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Poll_Watcher_remove, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Io_Poll_Context___construct, 0, 0, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, backend, Io\\Poll\\Backend, 0, "Io\\Poll\\Backend::Auto")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Io_Poll_Context_add, 0, 2, Io\\Poll\\Watcher, 0)
	ZEND_ARG_OBJ_INFO(0, handle, Io\\Poll\\Handle, 0)
	ZEND_ARG_TYPE_INFO(0, events, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, data, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Poll_Context_wait, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeoutSeconds, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeoutMicroseconds, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, maxEvents, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Io_Poll_Context_getBackend, 0, 0, Io\\Poll\\Backend, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_StreamPollHandle___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, stream)
ZEND_END_ARG_INFO()

#define arginfo_class_StreamPollHandle_getStream arginfo_class_Io_Poll_Watcher___construct

#define arginfo_class_StreamPollHandle_isValid arginfo_class_Io_Poll_Backend_isAvailable

#define arginfo_class_StreamPollHandle_getFileDescriptor arginfo_class_Io_Poll_Handle_getFileDescriptor

ZEND_METHOD(Io_Poll_Backend, getAvailableBackends);
ZEND_METHOD(Io_Poll_Backend, isAvailable);
ZEND_METHOD(Io_Poll_Backend, supportsEdgeTriggering);
ZEND_METHOD(Io_Poll_Watcher, __construct);
ZEND_METHOD(Io_Poll_Watcher, getHandle);
ZEND_METHOD(Io_Poll_Watcher, getWatchedEvents);
ZEND_METHOD(Io_Poll_Watcher, getTriggeredEvents);
ZEND_METHOD(Io_Poll_Watcher, getData);
ZEND_METHOD(Io_Poll_Watcher, hasTriggered);
ZEND_METHOD(Io_Poll_Watcher, isActive);
ZEND_METHOD(Io_Poll_Watcher, modify);
ZEND_METHOD(Io_Poll_Watcher, modifyEvents);
ZEND_METHOD(Io_Poll_Watcher, modifyData);
ZEND_METHOD(Io_Poll_Watcher, remove);
ZEND_METHOD(Io_Poll_Context, __construct);
ZEND_METHOD(Io_Poll_Context, add);
ZEND_METHOD(Io_Poll_Context, wait);
ZEND_METHOD(Io_Poll_Context, getBackend);
ZEND_METHOD(StreamPollHandle, __construct);
ZEND_METHOD(StreamPollHandle, getStream);
ZEND_METHOD(StreamPollHandle, isValid);
ZEND_METHOD(StreamPollHandle, getFileDescriptor);

static const zend_function_entry class_Io_Poll_Backend_methods[] = {
	ZEND_ME(Io_Poll_Backend, getAvailableBackends, arginfo_class_Io_Poll_Backend_getAvailableBackends, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Io_Poll_Backend, isAvailable, arginfo_class_Io_Poll_Backend_isAvailable, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Poll_Backend, supportsEdgeTriggering, arginfo_class_Io_Poll_Backend_supportsEdgeTriggering, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Io_Poll_Handle_methods[] = {
	ZEND_RAW_FENTRY("getFileDescriptor", NULL, arginfo_class_Io_Poll_Handle_getFileDescriptor, ZEND_ACC_PROTECTED|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_Io_Poll_Watcher_methods[] = {
	ZEND_ME(Io_Poll_Watcher, __construct, arginfo_class_Io_Poll_Watcher___construct, ZEND_ACC_PRIVATE|ZEND_ACC_FINAL)
	ZEND_ME(Io_Poll_Watcher, getHandle, arginfo_class_Io_Poll_Watcher_getHandle, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Poll_Watcher, getWatchedEvents, arginfo_class_Io_Poll_Watcher_getWatchedEvents, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Poll_Watcher, getTriggeredEvents, arginfo_class_Io_Poll_Watcher_getTriggeredEvents, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Poll_Watcher, getData, arginfo_class_Io_Poll_Watcher_getData, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Poll_Watcher, hasTriggered, arginfo_class_Io_Poll_Watcher_hasTriggered, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Poll_Watcher, isActive, arginfo_class_Io_Poll_Watcher_isActive, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Poll_Watcher, modify, arginfo_class_Io_Poll_Watcher_modify, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Poll_Watcher, modifyEvents, arginfo_class_Io_Poll_Watcher_modifyEvents, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Poll_Watcher, modifyData, arginfo_class_Io_Poll_Watcher_modifyData, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Poll_Watcher, remove, arginfo_class_Io_Poll_Watcher_remove, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Io_Poll_Context_methods[] = {
	ZEND_ME(Io_Poll_Context, __construct, arginfo_class_Io_Poll_Context___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Poll_Context, add, arginfo_class_Io_Poll_Context_add, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Poll_Context, wait, arginfo_class_Io_Poll_Context_wait, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Poll_Context, getBackend, arginfo_class_Io_Poll_Context_getBackend, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_StreamPollHandle_methods[] = {
	ZEND_ME(StreamPollHandle, __construct, arginfo_class_StreamPollHandle___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(StreamPollHandle, getStream, arginfo_class_StreamPollHandle_getStream, ZEND_ACC_PUBLIC)
	ZEND_ME(StreamPollHandle, isValid, arginfo_class_StreamPollHandle_isValid, ZEND_ACC_PUBLIC)
	ZEND_ME(StreamPollHandle, getFileDescriptor, arginfo_class_StreamPollHandle_getFileDescriptor, ZEND_ACC_PROTECTED)
	ZEND_FE_END
};

static zend_class_entry *register_class_Io_IoException(zend_class_entry *class_entry_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io", "IoException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Exception, 0);

	return class_entry;
}

static zend_class_entry *register_class_Io_Poll_Backend(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("Io\\Poll\\Backend", IS_UNDEF, class_Io_Poll_Backend_methods);

	zend_enum_add_case_cstr(class_entry, "Auto", NULL);

	zend_enum_add_case_cstr(class_entry, "Poll", NULL);

	zend_enum_add_case_cstr(class_entry, "Epoll", NULL);

	zend_enum_add_case_cstr(class_entry, "Kqueue", NULL);

	zend_enum_add_case_cstr(class_entry, "EventPorts", NULL);

	zend_enum_add_case_cstr(class_entry, "WSAPoll", NULL);

	return class_entry;
}

static zend_class_entry *register_class_Io_Poll_Event(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("Io\\Poll\\Event", IS_UNDEF, NULL);

	zend_enum_add_case_cstr(class_entry, "Read", NULL);

	zend_enum_add_case_cstr(class_entry, "Write", NULL);

	zend_enum_add_case_cstr(class_entry, "Error", NULL);

	zend_enum_add_case_cstr(class_entry, "HangUp", NULL);

	zend_enum_add_case_cstr(class_entry, "ReadHangUp", NULL);

	zend_enum_add_case_cstr(class_entry, "OneShot", NULL);

	zend_enum_add_case_cstr(class_entry, "EdgeTriggered", NULL);

	return class_entry;
}

static zend_class_entry *register_class_Io_Poll_Handle(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Poll", "Handle", class_Io_Poll_Handle_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_ABSTRACT);

	return class_entry;
}

static zend_class_entry *register_class_Io_Poll_Watcher(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Poll", "Watcher", class_Io_Poll_Watcher_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);

	return class_entry;
}

static zend_class_entry *register_class_Io_Poll_Context(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Poll", "Context", class_Io_Poll_Context_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);

	return class_entry;
}

static zend_class_entry *register_class_Io_Poll_PollException(zend_class_entry *class_entry_Io_IoException)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Poll", "PollException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Io_IoException, 0);

	return class_entry;
}

static zend_class_entry *register_class_Io_Poll_FailedPollOperationException(zend_class_entry *class_entry_Io_Poll_PollException)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Poll", "FailedPollOperationException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Io_Poll_PollException, ZEND_ACC_ABSTRACT);

	zval const_ERROR_NONE_value;
	ZVAL_LONG(&const_ERROR_NONE_value, PHP_POLL_ERROR_CODE_NONE);
	zend_string *const_ERROR_NONE_name = zend_string_init_interned("ERROR_NONE", sizeof("ERROR_NONE") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_ERROR_NONE_name, &const_ERROR_NONE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_ERROR_NONE_name, true);

	zval const_ERROR_SYSTEM_value;
	ZVAL_LONG(&const_ERROR_SYSTEM_value, PHP_POLL_ERROR_CODE_SYSTEM);
	zend_string *const_ERROR_SYSTEM_name = zend_string_init_interned("ERROR_SYSTEM", sizeof("ERROR_SYSTEM") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_ERROR_SYSTEM_name, &const_ERROR_SYSTEM_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_ERROR_SYSTEM_name, true);

	zval const_ERROR_NOMEM_value;
	ZVAL_LONG(&const_ERROR_NOMEM_value, PHP_POLL_ERROR_CODE_NOMEM);
	zend_string *const_ERROR_NOMEM_name = zend_string_init_interned("ERROR_NOMEM", sizeof("ERROR_NOMEM") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_ERROR_NOMEM_name, &const_ERROR_NOMEM_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_ERROR_NOMEM_name, true);

	zval const_ERROR_INVALID_value;
	ZVAL_LONG(&const_ERROR_INVALID_value, PHP_POLL_ERROR_CODE_INVALID);
	zend_string *const_ERROR_INVALID_name = zend_string_init_interned("ERROR_INVALID", sizeof("ERROR_INVALID") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_ERROR_INVALID_name, &const_ERROR_INVALID_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_ERROR_INVALID_name, true);

	zval const_ERROR_EXISTS_value;
	ZVAL_LONG(&const_ERROR_EXISTS_value, PHP_POLL_ERROR_CODE_EXISTS);
	zend_string *const_ERROR_EXISTS_name = zend_string_init_interned("ERROR_EXISTS", sizeof("ERROR_EXISTS") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_ERROR_EXISTS_name, &const_ERROR_EXISTS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_ERROR_EXISTS_name, true);

	zval const_ERROR_NOTFOUND_value;
	ZVAL_LONG(&const_ERROR_NOTFOUND_value, PHP_POLL_ERROR_CODE_NOTFOUND);
	zend_string *const_ERROR_NOTFOUND_name = zend_string_init_interned("ERROR_NOTFOUND", sizeof("ERROR_NOTFOUND") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_ERROR_NOTFOUND_name, &const_ERROR_NOTFOUND_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_ERROR_NOTFOUND_name, true);

	zval const_ERROR_TIMEOUT_value;
	ZVAL_LONG(&const_ERROR_TIMEOUT_value, PHP_POLL_ERROR_CODE_TIMEOUT);
	zend_string *const_ERROR_TIMEOUT_name = zend_string_init_interned("ERROR_TIMEOUT", sizeof("ERROR_TIMEOUT") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TIMEOUT_name, &const_ERROR_TIMEOUT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_ERROR_TIMEOUT_name, true);

	zval const_ERROR_INTERRUPTED_value;
	ZVAL_LONG(&const_ERROR_INTERRUPTED_value, PHP_POLL_ERROR_CODE_INTERRUPTED);
	zend_string *const_ERROR_INTERRUPTED_name = zend_string_init_interned("ERROR_INTERRUPTED", sizeof("ERROR_INTERRUPTED") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_ERROR_INTERRUPTED_name, &const_ERROR_INTERRUPTED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_ERROR_INTERRUPTED_name, true);

	zval const_ERROR_PERMISSION_value;
	ZVAL_LONG(&const_ERROR_PERMISSION_value, PHP_POLL_ERROR_CODE_PERMISSION);
	zend_string *const_ERROR_PERMISSION_name = zend_string_init_interned("ERROR_PERMISSION", sizeof("ERROR_PERMISSION") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_ERROR_PERMISSION_name, &const_ERROR_PERMISSION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_ERROR_PERMISSION_name, true);

	zval const_ERROR_TOOBIG_value;
	ZVAL_LONG(&const_ERROR_TOOBIG_value, PHP_POLL_ERROR_CODE_TOOBIG);
	zend_string *const_ERROR_TOOBIG_name = zend_string_init_interned("ERROR_TOOBIG", sizeof("ERROR_TOOBIG") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TOOBIG_name, &const_ERROR_TOOBIG_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_ERROR_TOOBIG_name, true);

	zval const_ERROR_AGAIN_value;
	ZVAL_LONG(&const_ERROR_AGAIN_value, PHP_POLL_ERROR_CODE_AGAIN);
	zend_string *const_ERROR_AGAIN_name = zend_string_init_interned("ERROR_AGAIN", sizeof("ERROR_AGAIN") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_ERROR_AGAIN_name, &const_ERROR_AGAIN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_ERROR_AGAIN_name, true);

	zval const_ERROR_NOSUPPORT_value;
	ZVAL_LONG(&const_ERROR_NOSUPPORT_value, PHP_POLL_ERROR_CODE_NOSUPPORT);
	zend_string *const_ERROR_NOSUPPORT_name = zend_string_init_interned("ERROR_NOSUPPORT", sizeof("ERROR_NOSUPPORT") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_ERROR_NOSUPPORT_name, &const_ERROR_NOSUPPORT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_ERROR_NOSUPPORT_name, true);

	return class_entry;
}

static zend_class_entry *register_class_Io_Poll_FailedContextInitializationException(zend_class_entry *class_entry_Io_Poll_FailedPollOperationException)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Poll", "FailedContextInitializationException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Io_Poll_FailedPollOperationException, 0);

	return class_entry;
}

static zend_class_entry *register_class_Io_Poll_FailedHandleAddException(zend_class_entry *class_entry_Io_Poll_FailedPollOperationException)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Poll", "FailedHandleAddException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Io_Poll_FailedPollOperationException, 0);

	return class_entry;
}

static zend_class_entry *register_class_Io_Poll_FailedWatcherModificationException(zend_class_entry *class_entry_Io_Poll_FailedPollOperationException)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Poll", "FailedWatcherModificationException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Io_Poll_FailedPollOperationException, 0);

	return class_entry;
}

static zend_class_entry *register_class_Io_Poll_FailedPollWaitException(zend_class_entry *class_entry_Io_Poll_FailedPollOperationException)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Poll", "FailedPollWaitException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Io_Poll_FailedPollOperationException, 0);

	return class_entry;
}

static zend_class_entry *register_class_Io_Poll_InactiveWatcherException(zend_class_entry *class_entry_Io_Poll_PollException)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Poll", "InactiveWatcherException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Io_Poll_PollException, 0);

	return class_entry;
}

static zend_class_entry *register_class_Io_Poll_HandleAlreadyWatchedException(zend_class_entry *class_entry_Io_Poll_PollException)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Poll", "HandleAlreadyWatchedException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Io_Poll_PollException, 0);

	return class_entry;
}

static zend_class_entry *register_class_Io_Poll_InvalidHandleException(zend_class_entry *class_entry_Io_Poll_PollException)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Poll", "InvalidHandleException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Io_Poll_PollException, 0);

	return class_entry;
}

static zend_class_entry *register_class_StreamPollHandle(zend_class_entry *class_entry_Io_Poll_Handle)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "StreamPollHandle", class_StreamPollHandle_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Io_Poll_Handle, ZEND_ACC_FINAL);

	return class_entry;
}
