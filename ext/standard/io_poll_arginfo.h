/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 2da72c26a7a62878ca8fb80cbe02c0f1887ee5f4 */

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
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, maxEvents, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Io_Poll_Context_getBackend, 0, 0, Io\\Poll\\Backend, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_StreamPollHandle___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, stream)
ZEND_END_ARG_INFO()

#define arginfo_class_StreamPollHandle_getStream arginfo_class_Io_Poll_Watcher___construct

#define arginfo_class_StreamPollHandle_isValid arginfo_class_Io_Poll_Backend_isAvailable

ZEND_METHOD(Io_Poll_Backend, getAvailableBackends);
ZEND_METHOD(Io_Poll_Backend, isAvailable);
ZEND_METHOD(Io_Poll_Backend, supportsEdgeTriggering);
ZEND_METHOD(Io_Poll_Handle, getFileDescriptor);
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

static const zend_function_entry class_Io_Poll_Backend_methods[] = {
	ZEND_ME(Io_Poll_Backend, getAvailableBackends, arginfo_class_Io_Poll_Backend_getAvailableBackends, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Io_Poll_Backend, isAvailable, arginfo_class_Io_Poll_Backend_isAvailable, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Poll_Backend, supportsEdgeTriggering, arginfo_class_Io_Poll_Backend_supportsEdgeTriggering, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Io_Poll_Handle_methods[] = {
	ZEND_ME(Io_Poll_Handle, getFileDescriptor, arginfo_class_Io_Poll_Handle_getFileDescriptor, ZEND_ACC_PROTECTED)
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
	ZEND_FE_END
};

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

static zend_class_entry *register_class_Io_Poll_PollException(zend_class_entry *class_entry_Io_Poll_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Poll", "PollException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Io_Poll_Exception, 0);

	return class_entry;
}

static zend_class_entry *register_class_StreamPollHandle(zend_class_entry *class_entry_PollHandle)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "StreamPollHandle", class_StreamPollHandle_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_PollHandle, ZEND_ACC_FINAL);

	return class_entry;
}
