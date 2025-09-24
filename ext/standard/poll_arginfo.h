/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 900953be32e526a0bdba88d0a1b78eb11f809609 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_PollHandle_getFileDescriptor, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_StreamPollHandle___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, stream)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_StreamPollHandle_getStream, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_StreamPollHandle_isValid, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PollWatcher___construct, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, handle, PollHandle, 0)
	ZEND_ARG_TYPE_INFO(0, events, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, data, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_PollWatcher_getHandle, 0, 0, PollHandle, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_PollWatcher_getWatchedEvents arginfo_class_PollHandle_getFileDescriptor

#define arginfo_class_PollWatcher_getTriggeredEvents arginfo_class_PollHandle_getFileDescriptor

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_PollWatcher_getData, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_PollWatcher_hasTriggered, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, events, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_PollWatcher_isActive arginfo_class_StreamPollHandle_isValid

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_PollWatcher_modify, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, events, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, data, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_PollWatcher_modifyEvents, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, events, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_PollWatcher_modifyData, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_PollWatcher_remove, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PollContext___construct, 0, 0, 0)
	ZEND_ARG_TYPE_MASK(0, backend, MAY_BE_LONG|MAY_BE_STRING, "POLL_BACKEND_AUTO")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_PollContext_add, 0, 2, PollWatcher, 0)
	ZEND_ARG_OBJ_INFO(0, handle, PollHandle, 0)
	ZEND_ARG_TYPE_INFO(0, events, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, data, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_PollContext_wait, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, maxEvents, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_PollContext_getBackendName, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_METHOD(PollHandle, getFileDescriptor);
ZEND_METHOD(StreamPollHandle, __construct);
ZEND_METHOD(StreamPollHandle, getStream);
ZEND_METHOD(StreamPollHandle, isValid);
ZEND_METHOD(PollWatcher, __construct);
ZEND_METHOD(PollWatcher, getHandle);
ZEND_METHOD(PollWatcher, getWatchedEvents);
ZEND_METHOD(PollWatcher, getTriggeredEvents);
ZEND_METHOD(PollWatcher, getData);
ZEND_METHOD(PollWatcher, hasTriggered);
ZEND_METHOD(PollWatcher, isActive);
ZEND_METHOD(PollWatcher, modify);
ZEND_METHOD(PollWatcher, modifyEvents);
ZEND_METHOD(PollWatcher, modifyData);
ZEND_METHOD(PollWatcher, remove);
ZEND_METHOD(PollContext, __construct);
ZEND_METHOD(PollContext, add);
ZEND_METHOD(PollContext, wait);
ZEND_METHOD(PollContext, getBackendName);

static const zend_function_entry class_PollHandle_methods[] = {
	ZEND_ME(PollHandle, getFileDescriptor, arginfo_class_PollHandle_getFileDescriptor, ZEND_ACC_PROTECTED)
	ZEND_FE_END
};

static const zend_function_entry class_StreamPollHandle_methods[] = {
	ZEND_ME(StreamPollHandle, __construct, arginfo_class_StreamPollHandle___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(StreamPollHandle, getStream, arginfo_class_StreamPollHandle_getStream, ZEND_ACC_PUBLIC)
	ZEND_ME(StreamPollHandle, isValid, arginfo_class_StreamPollHandle_isValid, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_PollWatcher_methods[] = {
	ZEND_ME(PollWatcher, __construct, arginfo_class_PollWatcher___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(PollWatcher, getHandle, arginfo_class_PollWatcher_getHandle, ZEND_ACC_PUBLIC)
	ZEND_ME(PollWatcher, getWatchedEvents, arginfo_class_PollWatcher_getWatchedEvents, ZEND_ACC_PUBLIC)
	ZEND_ME(PollWatcher, getTriggeredEvents, arginfo_class_PollWatcher_getTriggeredEvents, ZEND_ACC_PUBLIC)
	ZEND_ME(PollWatcher, getData, arginfo_class_PollWatcher_getData, ZEND_ACC_PUBLIC)
	ZEND_ME(PollWatcher, hasTriggered, arginfo_class_PollWatcher_hasTriggered, ZEND_ACC_PUBLIC)
	ZEND_ME(PollWatcher, isActive, arginfo_class_PollWatcher_isActive, ZEND_ACC_PUBLIC)
	ZEND_ME(PollWatcher, modify, arginfo_class_PollWatcher_modify, ZEND_ACC_PUBLIC)
	ZEND_ME(PollWatcher, modifyEvents, arginfo_class_PollWatcher_modifyEvents, ZEND_ACC_PUBLIC)
	ZEND_ME(PollWatcher, modifyData, arginfo_class_PollWatcher_modifyData, ZEND_ACC_PUBLIC)
	ZEND_ME(PollWatcher, remove, arginfo_class_PollWatcher_remove, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_PollContext_methods[] = {
	ZEND_ME(PollContext, __construct, arginfo_class_PollContext___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(PollContext, add, arginfo_class_PollContext_add, ZEND_ACC_PUBLIC)
	ZEND_ME(PollContext, wait, arginfo_class_PollContext_wait, ZEND_ACC_PUBLIC)
	ZEND_ME(PollContext, getBackendName, arginfo_class_PollContext_getBackendName, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static void register_poll_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("POLL_EVENT_READ", PHP_POLL_READ, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("POLL_EVENT_WRITE", PHP_POLL_WRITE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("POLL_EVENT_ERROR", PHP_POLL_ERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("POLL_EVENT_HUP", PHP_POLL_HUP, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("POLL_EVENT_RDHUP", PHP_POLL_RDHUP, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("POLL_EVENT_ONESHOT", PHP_POLL_ONESHOT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("POLL_EVENT_ET", PHP_POLL_ET, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("POLL_BACKEND_AUTO", PHP_POLL_BACKEND_AUTO, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("POLL_BACKEND_POLL", PHP_POLL_BACKEND_POLL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("POLL_BACKEND_EPOLL", PHP_POLL_BACKEND_EPOLL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("POLL_BACKEND_KQUEUE", PHP_POLL_BACKEND_KQUEUE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("POLL_BACKEND_EVENTPORT", PHP_POLL_BACKEND_EVENTPORT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("POLL_BACKEND_WSAPOLL", PHP_POLL_BACKEND_WSAPOLL, CONST_PERSISTENT);
}

static zend_class_entry *register_class_PollHandle(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "PollHandle", class_PollHandle_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_ABSTRACT);

	return class_entry;
}

static zend_class_entry *register_class_StreamPollHandle(zend_class_entry *class_entry_PollHandle)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "StreamPollHandle", class_StreamPollHandle_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_PollHandle, ZEND_ACC_FINAL);

	return class_entry;
}

static zend_class_entry *register_class_PollWatcher(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "PollWatcher", class_PollWatcher_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);

	return class_entry;
}

static zend_class_entry *register_class_PollContext(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "PollContext", class_PollContext_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);

	return class_entry;
}

static zend_class_entry *register_class_PollException(zend_class_entry *class_entry_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "PollException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Exception, 0);

	return class_entry;
}
