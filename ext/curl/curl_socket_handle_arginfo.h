/* This is a generated file, edit curl_socket_handle.stub.php instead.
 * Stub hash: d348a6085540499aa70bf2acbf03f2a28f65f35b */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_CurlSocketPollWeakHandle___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_CurlSocketPollWeakHandle_isValid, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_METHOD(CurlSocketPollWeakHandle, __construct);
ZEND_METHOD(CurlSocketPollWeakHandle, isValid);

static const zend_function_entry class_CurlSocketPollWeakHandle_methods[] = {
	ZEND_ME(CurlSocketPollWeakHandle, __construct, arginfo_class_CurlSocketPollWeakHandle___construct, ZEND_ACC_PRIVATE)
	ZEND_ME(CurlSocketPollWeakHandle, isValid, arginfo_class_CurlSocketPollWeakHandle_isValid, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_CurlSocketPollWeakHandle(zend_class_entry *class_entry_Io_Poll_WeakHandle)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "CurlSocketPollWeakHandle", class_CurlSocketPollWeakHandle_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);
	zend_class_implements(class_entry, 1, class_entry_Io_Poll_WeakHandle);

	return class_entry;
}
