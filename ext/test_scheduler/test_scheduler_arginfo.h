/* This is a generated file, edit test_scheduler.stub.php instead.
 * Stub hash: 76721ca7326a23b884519baa2b408b3d19b3d0e7 */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_TestScheduler_spawn, 0, 1, TestScheduler\\Coroutine, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, args, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_TestScheduler_suspend, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_TestScheduler_resume, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, coroutine, TestScheduler\\Coroutine, 0)
ZEND_END_ARG_INFO()

#define arginfo_TestScheduler_cancel arginfo_TestScheduler_resume

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_TestScheduler_await, 0, 1, IS_MIXED, 0)
	ZEND_ARG_OBJ_INFO(0, coroutine, TestScheduler\\Coroutine, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_TestScheduler_current, 0, 0, TestScheduler\\Coroutine, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_TestScheduler_Coroutine___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TestScheduler_Coroutine_isStarted, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_TestScheduler_Coroutine_isRunning arginfo_class_TestScheduler_Coroutine_isStarted

#define arginfo_class_TestScheduler_Coroutine_isSuspended arginfo_class_TestScheduler_Coroutine_isStarted

#define arginfo_class_TestScheduler_Coroutine_isFinished arginfo_class_TestScheduler_Coroutine_isStarted

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TestScheduler_Coroutine_getResult, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_TestScheduler_Coroutine_getException, 0, 0, Throwable, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TestScheduler_Coroutine_getAwaitingInfo, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_FUNCTION(TestScheduler_spawn);
ZEND_FUNCTION(TestScheduler_suspend);
ZEND_FUNCTION(TestScheduler_resume);
ZEND_FUNCTION(TestScheduler_cancel);
ZEND_FUNCTION(TestScheduler_await);
ZEND_FUNCTION(TestScheduler_current);
ZEND_METHOD(TestScheduler_Coroutine, __construct);
ZEND_METHOD(TestScheduler_Coroutine, isStarted);
ZEND_METHOD(TestScheduler_Coroutine, isRunning);
ZEND_METHOD(TestScheduler_Coroutine, isSuspended);
ZEND_METHOD(TestScheduler_Coroutine, isFinished);
ZEND_METHOD(TestScheduler_Coroutine, getResult);
ZEND_METHOD(TestScheduler_Coroutine, getException);
ZEND_METHOD(TestScheduler_Coroutine, getAwaitingInfo);

static const zend_function_entry ext_functions[] = {
	ZEND_RAW_FENTRY(ZEND_NS_NAME("TestScheduler", "spawn"), zif_TestScheduler_spawn, arginfo_TestScheduler_spawn, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("TestScheduler", "suspend"), zif_TestScheduler_suspend, arginfo_TestScheduler_suspend, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("TestScheduler", "resume"), zif_TestScheduler_resume, arginfo_TestScheduler_resume, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("TestScheduler", "cancel"), zif_TestScheduler_cancel, arginfo_TestScheduler_cancel, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("TestScheduler", "await"), zif_TestScheduler_await, arginfo_TestScheduler_await, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("TestScheduler", "current"), zif_TestScheduler_current, arginfo_TestScheduler_current, 0, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_TestScheduler_Coroutine_methods[] = {
	ZEND_ME(TestScheduler_Coroutine, __construct, arginfo_class_TestScheduler_Coroutine___construct, ZEND_ACC_PRIVATE)
	ZEND_ME(TestScheduler_Coroutine, isStarted, arginfo_class_TestScheduler_Coroutine_isStarted, ZEND_ACC_PUBLIC)
	ZEND_ME(TestScheduler_Coroutine, isRunning, arginfo_class_TestScheduler_Coroutine_isRunning, ZEND_ACC_PUBLIC)
	ZEND_ME(TestScheduler_Coroutine, isSuspended, arginfo_class_TestScheduler_Coroutine_isSuspended, ZEND_ACC_PUBLIC)
	ZEND_ME(TestScheduler_Coroutine, isFinished, arginfo_class_TestScheduler_Coroutine_isFinished, ZEND_ACC_PUBLIC)
	ZEND_ME(TestScheduler_Coroutine, getResult, arginfo_class_TestScheduler_Coroutine_getResult, ZEND_ACC_PUBLIC)
	ZEND_ME(TestScheduler_Coroutine, getException, arginfo_class_TestScheduler_Coroutine_getException, ZEND_ACC_PUBLIC)
	ZEND_ME(TestScheduler_Coroutine, getAwaitingInfo, arginfo_class_TestScheduler_Coroutine_getAwaitingInfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_TestScheduler_Coroutine(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "TestScheduler", "Coroutine", class_TestScheduler_Coroutine_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	return class_entry;
}

static zend_class_entry *register_class_TestScheduler_CancellationError(zend_class_entry *class_entry_Error)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "TestScheduler", "CancellationError", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Error, 0);

	return class_entry;
}

static zend_class_entry *register_class_TestScheduler_DeadlockError(zend_class_entry *class_entry_Error)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "TestScheduler", "DeadlockError", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Error, ZEND_ACC_FINAL);

	return class_entry;
}
