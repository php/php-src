/* This is a generated file, edit io_ring.stub.php instead.
 * Stub hash: 45f2478864f85549c4075b95c746c80aea3d552b
 * Has decl header: yes */

#include "zend_enum.h"

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Io_Ring_Engine___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, entries, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Io_Ring_Engine_getBackend, 0, 0, Io\\Ring\\Backend, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Io_Ring_Engine_getHandle, 0, 0, Io\\Poll\\\116otifyHandle, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Ring_Engine_submit, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, op, Io\\Operation, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, data, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Ring_Engine_cancel, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, op, Io\\Operation, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Ring_Engine_add, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, registration, Io\\Registration, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Io_Ring_Engine_remove arginfo_class_Io_Ring_Engine_add

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Ring_Engine_waitCompletions, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, timeout, Time\\Duration, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, max, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Ring_Engine_countPending, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Ring_Engine_getHookCapabilities, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Io_Ring_Engine_getSupportedHookCapabilities arginfo_class_Io_Ring_Engine_getHookCapabilities

ZEND_METHOD(Io_Ring_Engine, __construct);
ZEND_METHOD(Io_Ring_Engine, getBackend);
ZEND_METHOD(Io_Ring_Engine, getHandle);
ZEND_METHOD(Io_Ring_Engine, submit);
ZEND_METHOD(Io_Ring_Engine, cancel);
ZEND_METHOD(Io_Ring_Engine, add);
ZEND_METHOD(Io_Ring_Engine, remove);
ZEND_METHOD(Io_Ring_Engine, waitCompletions);
ZEND_METHOD(Io_Ring_Engine, countPending);
ZEND_METHOD(Io_Ring_Engine, getHookCapabilities);
ZEND_METHOD(Io_Ring_Engine, getSupportedHookCapabilities);

static const zend_function_entry class_Io_Ring_Engine_methods[] = {
	ZEND_ME(Io_Ring_Engine, __construct, arginfo_class_Io_Ring_Engine___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Ring_Engine, getBackend, arginfo_class_Io_Ring_Engine_getBackend, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Ring_Engine, getHandle, arginfo_class_Io_Ring_Engine_getHandle, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Ring_Engine, submit, arginfo_class_Io_Ring_Engine_submit, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Ring_Engine, cancel, arginfo_class_Io_Ring_Engine_cancel, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Ring_Engine, add, arginfo_class_Io_Ring_Engine_add, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Ring_Engine, remove, arginfo_class_Io_Ring_Engine_remove, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Ring_Engine, waitCompletions, arginfo_class_Io_Ring_Engine_waitCompletions, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Ring_Engine, countPending, arginfo_class_Io_Ring_Engine_countPending, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Ring_Engine, getHookCapabilities, arginfo_class_Io_Ring_Engine_getHookCapabilities, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Ring_Engine, getSupportedHookCapabilities, arginfo_class_Io_Ring_Engine_getSupportedHookCapabilities, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Io_Ring_Backend(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("Io\\Ring\\Backend", IS_UNDEF, NULL);

	zend_enum_add_case_cstr(class_entry, "IoUring", NULL);

	zend_enum_add_case_cstr(class_entry, "Iocp", NULL);

	zend_enum_add_case_cstr(class_entry, "Threads", NULL);

	return class_entry;
}

static zend_class_entry *register_class_Io_Ring_Engine(zend_class_entry *class_entry_Io_OperationQueue)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Ring", "Engine", class_Io_Ring_Engine_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);
	zend_class_implements(class_entry, 1, class_entry_Io_OperationQueue);

	return class_entry;
}

static zend_class_entry *register_class_Io_Ring_RingException(zend_class_entry *class_entry_Io_IoException)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Ring", "RingException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Io_IoException, 0);

	return class_entry;
}

static zend_class_entry *register_class_Io_Ring_FailedRingOperationException(zend_class_entry *class_entry_Io_Ring_RingException)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Ring", "FailedRingOperationException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Io_Ring_RingException, 0);

	return class_entry;
}
