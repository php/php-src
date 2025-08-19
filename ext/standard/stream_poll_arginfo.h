/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: a067ddaa03e0ae4f1d2dd2ac3715fd31bf86ad28 */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_stream_poll_create, 0, 0, StreamPollContext, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, max_events, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, backend, IS_LONG, 0, "STREAM_POLL_BACKEND_AUTO")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_stream_poll_add, 0, 3, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, poll_ctx, StreamPollContext, 0)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_TYPE_INFO(0, events, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, data, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

#define arginfo_stream_poll_modify arginfo_stream_poll_add

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_stream_poll_remove, 0, 2, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, poll_ctx, StreamPollContext, 0)
	ZEND_ARG_INFO(0, stream)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_stream_poll_wait, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_OBJ_INFO(0, poll_ctx, StreamPollContext, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_stream_poll_backend_name, 0, 1, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, poll_ctx, StreamPollContext, 0)
ZEND_END_ARG_INFO()

ZEND_FUNCTION(stream_poll_create);
ZEND_FUNCTION(stream_poll_add);
ZEND_FUNCTION(stream_poll_modify);
ZEND_FUNCTION(stream_poll_remove);
ZEND_FUNCTION(stream_poll_wait);
ZEND_FUNCTION(stream_poll_backend_name);

static const zend_function_entry ext_functions[] = {
	ZEND_FE(stream_poll_create, arginfo_stream_poll_create)
	ZEND_FE(stream_poll_add, arginfo_stream_poll_add)
	ZEND_FE(stream_poll_modify, arginfo_stream_poll_modify)
	ZEND_FE(stream_poll_remove, arginfo_stream_poll_remove)
	ZEND_FE(stream_poll_wait, arginfo_stream_poll_wait)
	ZEND_FE(stream_poll_backend_name, arginfo_stream_poll_backend_name)
	ZEND_FE_END
};

static void register_stream_poll_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("STREAM_POLL_READ", PHP_POLL_READ, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_POLL_WRITE", PHP_POLL_WRITE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_POLL_ERROR", PHP_POLL_ERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_POLL_HUP", PHP_POLL_HUP, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_POLL_RDHUP", PHP_POLL_RDHUP, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_POLL_ONESHOT", PHP_POLL_ONESHOT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_POLL_ET", PHP_POLL_ET, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_POLL_BACKEND_AUTO", PHP_POLL_BACKEND_AUTO, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_POLL_BACKEND_POLL", PHP_POLL_BACKEND_POLL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_POLL_BACKEND_EPOLL", PHP_POLL_BACKEND_EPOLL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_POLL_BACKEND_KQUEUE", PHP_POLL_BACKEND_KQUEUE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_POLL_BACKEND_EVENTPORT", PHP_POLL_BACKEND_EVENTPORT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_POLL_BACKEND_SELECT", PHP_POLL_BACKEND_SELECT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STREAM_POLL_BACKEND_IOCP", PHP_POLL_BACKEND_IOCP, CONST_PERSISTENT);
}

static zend_class_entry *register_class_StreamPollContext(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "StreamPollContext", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);

	return class_entry;
}

static zend_class_entry *register_class_StreamPollEvent(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "StreamPollEvent", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);

	zval property_stream_default_value;
	ZVAL_NULL(&property_stream_default_value);
	zend_string *property_stream_name = zend_string_init("stream", sizeof("stream") - 1, 1);
	zend_declare_typed_property(class_entry, property_stream_name, &property_stream_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_NONE(0));
	zend_string_release(property_stream_name);

	zval property_events_default_value;
	ZVAL_UNDEF(&property_events_default_value);
	zend_string *property_events_name = zend_string_init("events", sizeof("events") - 1, 1);
	zend_declare_typed_property(class_entry, property_events_name, &property_events_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_events_name);

	zval property_data_default_value;
	ZVAL_UNDEF(&property_data_default_value);
	zend_string *property_data_name = zend_string_init("data", sizeof("data") - 1, 1);
	zend_declare_typed_property(class_entry, property_data_name, &property_data_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ANY));
	zend_string_release(property_data_name);

	return class_entry;
}

static zend_class_entry *register_class_StreamPollException(zend_class_entry *class_entry_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "StreamPollException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Exception, 0);

	return class_entry;
}
