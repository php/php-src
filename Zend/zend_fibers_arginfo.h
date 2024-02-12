/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: e82bbc8e81fe98873a9a5697a4b38e63a24379da */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Fiber___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Fiber_start, 0, 0, IS_MIXED, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, args, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Fiber_resume, 0, 0, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, value, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Fiber_throw, 0, 1, IS_MIXED, 0)
	ZEND_ARG_OBJ_INFO(0, exception, Throwable, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Fiber_isStarted, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Fiber_isSuspended arginfo_class_Fiber_isStarted

#define arginfo_class_Fiber_isRunning arginfo_class_Fiber_isStarted

#define arginfo_class_Fiber_isTerminated arginfo_class_Fiber_isStarted

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Fiber_getReturn, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Fiber_getCurrent, 0, 0, Fiber, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Fiber_suspend arginfo_class_Fiber_resume

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_FiberError___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_METHOD(Fiber, __construct);
ZEND_METHOD(Fiber, start);
ZEND_METHOD(Fiber, resume);
ZEND_METHOD(Fiber, throw);
ZEND_METHOD(Fiber, isStarted);
ZEND_METHOD(Fiber, isSuspended);
ZEND_METHOD(Fiber, isRunning);
ZEND_METHOD(Fiber, isTerminated);
ZEND_METHOD(Fiber, getReturn);
ZEND_METHOD(Fiber, getCurrent);
ZEND_METHOD(Fiber, suspend);
ZEND_METHOD(FiberError, __construct);

static const zend_function_entry class_Fiber_methods[] = {
	ZEND_ME(Fiber, __construct, arginfo_class_Fiber___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Fiber, start, arginfo_class_Fiber_start, ZEND_ACC_PUBLIC)
	ZEND_ME(Fiber, resume, arginfo_class_Fiber_resume, ZEND_ACC_PUBLIC)
	ZEND_ME(Fiber, throw, arginfo_class_Fiber_throw, ZEND_ACC_PUBLIC)
	ZEND_ME(Fiber, isStarted, arginfo_class_Fiber_isStarted, ZEND_ACC_PUBLIC)
	ZEND_ME(Fiber, isSuspended, arginfo_class_Fiber_isSuspended, ZEND_ACC_PUBLIC)
	ZEND_ME(Fiber, isRunning, arginfo_class_Fiber_isRunning, ZEND_ACC_PUBLIC)
	ZEND_ME(Fiber, isTerminated, arginfo_class_Fiber_isTerminated, ZEND_ACC_PUBLIC)
	ZEND_ME(Fiber, getReturn, arginfo_class_Fiber_getReturn, ZEND_ACC_PUBLIC)
	ZEND_ME(Fiber, getCurrent, arginfo_class_Fiber_getCurrent, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Fiber, suspend, arginfo_class_Fiber_suspend, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};

static const zend_function_entry class_FiberError_methods[] = {
	ZEND_ME(FiberError, __construct, arginfo_class_FiberError___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Fiber(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "Fiber", class_Fiber_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}

static zend_class_entry *register_class_FiberError(zend_class_entry *class_entry_Error)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "FiberError", class_FiberError_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_Error);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	return class_entry;
}
