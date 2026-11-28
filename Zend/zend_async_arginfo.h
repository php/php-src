/* This is a generated file, edit zend_async.stub.php instead.
 * Stub hash: ee6fd8ce0ff655620e4d87bdccfd50c5be7271f9 */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_Async_async, 0, 1, Async\\Future, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, args, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Async_wait, 0, 1, IS_MIXED, 0)
	ZEND_ARG_OBJ_INFO(0, future, Async\\Future, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_Async_delay, 0, 1, Async\\Future, 0)
	ZEND_ARG_TYPE_INFO(0, seconds, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Async_suspend, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Async_Future___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Async_Future_isDone, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_FUNCTION(Async_async);
ZEND_FUNCTION(Async_wait);
ZEND_FUNCTION(Async_delay);
ZEND_FUNCTION(Async_suspend);
ZEND_METHOD(Async_Future, __construct);
ZEND_METHOD(Async_Future, isDone);

static const zend_function_entry ext_functions[] = {
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Async", "async"), zif_Async_async, arginfo_Async_async, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Async", "wait"), zif_Async_wait, arginfo_Async_wait, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Async", "delay"), zif_Async_delay, arginfo_Async_delay, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Async", "suspend"), zif_Async_suspend, arginfo_Async_suspend, 0, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_Async_Future_methods[] = {
	ZEND_ME(Async_Future, __construct, arginfo_class_Async_Future___construct, ZEND_ACC_PRIVATE)
	ZEND_ME(Async_Future, isDone, arginfo_class_Async_Future_isDone, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Async_Future(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Async", "Future", class_Async_Future_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	return class_entry;
}
