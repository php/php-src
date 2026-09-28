/* This is a generated file, edit io_hooks.stub.php instead.
 * Stub hash: 7fdd8e3ac6f1ef25d6613d10042947e939ac494f
 * Has decl header: yes */

#include "zend_enum.h"

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_Io_Hooks_set_hooks, 0, 1, Io\\Hooks\\Hooks, 1)
	ZEND_ARG_OBJ_INFO(0, hooks, Io\\Hooks\\Hooks, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_Io_Hooks_get_hooks, 0, 0, Io\\Hooks\\Hooks, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Io_Hooks_is_active, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Io_Operation___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Io_Operation_getHandle, 0, 0, Io\\Poll\\Handle, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Io_Operation_getRegistration, 0, 0, Io\\Registration, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Operation_getEvents, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Io_Operation_getTimeout, 0, 0, Time\\Duration, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Io_Operation_isValid arginfo_Io_Hooks_is_active

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Io_Operation_complete, 0, 1, Io\\Completion, 0)
	ZEND_ARG_OBJ_INFO(0, status, Io\\CompletionStatus, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, result, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, error, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Io_Operation_completeReady, 0, 1, Io\\Completion, 0)
	ZEND_ARG_TYPE_INFO(0, events, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Io_Completion___construct arginfo_class_Io_Operation___construct

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Io_Completion_getOperation, 0, 0, Io\\Operation, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Io_Completion_getStatus, 0, 0, Io\\CompletionStatus, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Completion_getResult, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Io_Completion_getEvents arginfo_class_Io_Operation_getEvents

#define arginfo_class_Io_Completion_getError arginfo_class_Io_Completion_getResult

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Completion_getData, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Io_Completion_getCompletions arginfo_class_Io_Operation_getEvents

#define arginfo_class_Io_Registration___construct arginfo_class_Io_Operation___construct

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Io_Registration_getHandle, 0, 0, Io\\Poll\\WeakHandle, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Io_Registration_getEvent, 0, 0, Io\\Poll\\Event, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Io_Registration_getTrigger, 0, 0, Io\\Poll\\Trigger, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Io_Registration_isValid arginfo_Io_Hooks_is_active

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_OperationQueue_submit, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, op, Io\\Operation, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, data, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_OperationQueue_cancel, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, op, Io\\Operation, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_OperationQueue_add, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, registration, Io\\Registration, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Io_OperationQueue_remove arginfo_class_Io_OperationQueue_add

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_OperationQueue_waitCompletions, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, timeout, Time\\Duration, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, max, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_Io_OperationQueue_countPending arginfo_class_Io_Completion_getResult

#define arginfo_class_Io_OperationQueue_getHookCapabilities arginfo_class_Io_Operation_getEvents

#define arginfo_class_Io_Operation_Read_getLength arginfo_class_Io_Completion_getResult

#define arginfo_class_Io_Operation_Read_getOffset arginfo_class_Io_Completion_getResult

#define arginfo_class_Io_Operation_Write_getLength arginfo_class_Io_Completion_getResult

#define arginfo_class_Io_Operation_Write_getOffset arginfo_class_Io_Completion_getResult

#define arginfo_class_Io_Operation_Recv_getLength arginfo_class_Io_Completion_getResult

#define arginfo_class_Io_Operation_Recv_getFlags arginfo_class_Io_Completion_getResult

#define arginfo_class_Io_Operation_Send_getLength arginfo_class_Io_Completion_getResult

#define arginfo_class_Io_Operation_Send_getFlags arginfo_class_Io_Completion_getResult

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Operation_Connect_getAddress, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Io_Operation_Fsync_isDataOnly arginfo_Io_Hooks_is_active

#define arginfo_class_Io_Operation_WaitPid_getPid arginfo_class_Io_Completion_getResult

#define arginfo_class_Io_Operation_SigWait_getSignals arginfo_class_Io_Operation_getEvents

#define arginfo_class_Io_Operation_GetAddrInfo_getHost arginfo_class_Io_Operation_Connect_getAddress

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Io_Operation_GetAddrInfo_getService, 0, 0, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Io_Operation_GetAddrInfo_completeWithAddresses, 0, 1, Io\\Completion, 0)
	ZEND_ARG_TYPE_INFO(0, addresses, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Io_Operation_GetNameInfo_getAddress arginfo_class_Io_Operation_Connect_getAddress

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Io_Operation_GetNameInfo_completeWithName, 0, 1, Io\\Completion, 0)
	ZEND_ARG_TYPE_INFO(0, host, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, service, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_Io_Operation_Any_getOperations arginfo_class_Io_Operation_getEvents

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Io_Operation_Any_completeWith, 0, 1, Io\\Completion, 0)
	ZEND_ARG_TYPE_INFO(0, completions, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Io_Poll_OperationQueue___construct, 0, 0, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, context, Io\\Poll\\Context, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Io_Poll_OperationQueue_getContext, 0, 0, Io\\Poll\\Context, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Io_Poll_OperationQueue_submit arginfo_class_Io_OperationQueue_submit

#define arginfo_class_Io_Poll_OperationQueue_cancel arginfo_class_Io_OperationQueue_cancel

#define arginfo_class_Io_Poll_OperationQueue_add arginfo_class_Io_OperationQueue_add

#define arginfo_class_Io_Poll_OperationQueue_remove arginfo_class_Io_OperationQueue_add

#define arginfo_class_Io_Poll_OperationQueue_waitCompletions arginfo_class_Io_OperationQueue_waitCompletions

#define arginfo_class_Io_Poll_OperationQueue_countPending arginfo_class_Io_Completion_getResult

#define arginfo_class_Io_Poll_OperationQueue_getHookCapabilities arginfo_class_Io_Operation_getEvents

#define arginfo_class_Io_Hooks_Hooks_getCapabilities arginfo_class_Io_Operation_getEvents

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Io_Hooks_Hooks_run, 0, 1, Io\\Completion, 0)
	ZEND_ARG_OBJ_INFO(0, op, Io\\Operation, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Io_Hooks_Hooks_add arginfo_class_Io_OperationQueue_add

#define arginfo_class_Io_Hooks_Hooks_remove arginfo_class_Io_OperationQueue_add

ZEND_FUNCTION(Io_Hooks_set_hooks);
ZEND_FUNCTION(Io_Hooks_get_hooks);
ZEND_FUNCTION(Io_Hooks_is_active);
ZEND_METHOD(Io_Operation, __construct);
ZEND_METHOD(Io_Operation, getHandle);
ZEND_METHOD(Io_Operation, getRegistration);
ZEND_METHOD(Io_Operation, getEvents);
ZEND_METHOD(Io_Operation, getTimeout);
ZEND_METHOD(Io_Operation, isValid);
ZEND_METHOD(Io_Operation, complete);
ZEND_METHOD(Io_Operation, completeReady);
ZEND_METHOD(Io_Completion, __construct);
ZEND_METHOD(Io_Completion, getOperation);
ZEND_METHOD(Io_Completion, getStatus);
ZEND_METHOD(Io_Completion, getResult);
ZEND_METHOD(Io_Completion, getEvents);
ZEND_METHOD(Io_Completion, getError);
ZEND_METHOD(Io_Completion, getData);
ZEND_METHOD(Io_Completion, getCompletions);
ZEND_METHOD(Io_Registration, __construct);
ZEND_METHOD(Io_Registration, getHandle);
ZEND_METHOD(Io_Registration, getEvent);
ZEND_METHOD(Io_Registration, getTrigger);
ZEND_METHOD(Io_Registration, isValid);
ZEND_METHOD(Io_Operation_Read, getLength);
ZEND_METHOD(Io_Operation_Read, getOffset);
ZEND_METHOD(Io_Operation_Write, getLength);
ZEND_METHOD(Io_Operation_Write, getOffset);
ZEND_METHOD(Io_Operation_Recv, getLength);
ZEND_METHOD(Io_Operation_Recv, getFlags);
ZEND_METHOD(Io_Operation_Send, getLength);
ZEND_METHOD(Io_Operation_Send, getFlags);
ZEND_METHOD(Io_Operation_Connect, getAddress);
ZEND_METHOD(Io_Operation_Fsync, isDataOnly);
ZEND_METHOD(Io_Operation_WaitPid, getPid);
ZEND_METHOD(Io_Operation_SigWait, getSignals);
ZEND_METHOD(Io_Operation_GetAddrInfo, getHost);
ZEND_METHOD(Io_Operation_GetAddrInfo, getService);
ZEND_METHOD(Io_Operation_GetAddrInfo, completeWithAddresses);
ZEND_METHOD(Io_Operation_GetNameInfo, getAddress);
ZEND_METHOD(Io_Operation_GetNameInfo, completeWithName);
ZEND_METHOD(Io_Operation_Any, getOperations);
ZEND_METHOD(Io_Operation_Any, completeWith);
ZEND_METHOD(Io_Poll_OperationQueue, __construct);
ZEND_METHOD(Io_Poll_OperationQueue, getContext);
ZEND_METHOD(Io_Poll_OperationQueue, submit);
ZEND_METHOD(Io_Poll_OperationQueue, cancel);
ZEND_METHOD(Io_Poll_OperationQueue, add);
ZEND_METHOD(Io_Poll_OperationQueue, remove);
ZEND_METHOD(Io_Poll_OperationQueue, waitCompletions);
ZEND_METHOD(Io_Poll_OperationQueue, countPending);
ZEND_METHOD(Io_Poll_OperationQueue, getHookCapabilities);

static const zend_function_entry ext_functions[] = {
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Io\\Hooks", "set_hooks"), zif_Io_Hooks_set_hooks, arginfo_Io_Hooks_set_hooks, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Io\\Hooks", "get_hooks"), zif_Io_Hooks_get_hooks, arginfo_Io_Hooks_get_hooks, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Io\\Hooks", "is_active"), zif_Io_Hooks_is_active, arginfo_Io_Hooks_is_active, 0, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_Io_Operation_methods[] = {
	ZEND_ME(Io_Operation, __construct, arginfo_class_Io_Operation___construct, ZEND_ACC_PRIVATE)
	ZEND_ME(Io_Operation, getHandle, arginfo_class_Io_Operation_getHandle, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Operation, getRegistration, arginfo_class_Io_Operation_getRegistration, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Operation, getEvents, arginfo_class_Io_Operation_getEvents, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Operation, getTimeout, arginfo_class_Io_Operation_getTimeout, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Operation, isValid, arginfo_class_Io_Operation_isValid, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Operation, complete, arginfo_class_Io_Operation_complete, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Operation, completeReady, arginfo_class_Io_Operation_completeReady, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Io_Completion_methods[] = {
	ZEND_ME(Io_Completion, __construct, arginfo_class_Io_Completion___construct, ZEND_ACC_PRIVATE)
	ZEND_ME(Io_Completion, getOperation, arginfo_class_Io_Completion_getOperation, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Completion, getStatus, arginfo_class_Io_Completion_getStatus, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Completion, getResult, arginfo_class_Io_Completion_getResult, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Completion, getEvents, arginfo_class_Io_Completion_getEvents, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Completion, getError, arginfo_class_Io_Completion_getError, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Completion, getData, arginfo_class_Io_Completion_getData, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Completion, getCompletions, arginfo_class_Io_Completion_getCompletions, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Io_Registration_methods[] = {
	ZEND_ME(Io_Registration, __construct, arginfo_class_Io_Registration___construct, ZEND_ACC_PRIVATE)
	ZEND_ME(Io_Registration, getHandle, arginfo_class_Io_Registration_getHandle, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Registration, getEvent, arginfo_class_Io_Registration_getEvent, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Registration, getTrigger, arginfo_class_Io_Registration_getTrigger, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Registration, isValid, arginfo_class_Io_Registration_isValid, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Io_OperationQueue_methods[] = {
	ZEND_RAW_FENTRY("submit", NULL, arginfo_class_Io_OperationQueue_submit, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("cancel", NULL, arginfo_class_Io_OperationQueue_cancel, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("add", NULL, arginfo_class_Io_OperationQueue_add, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("remove", NULL, arginfo_class_Io_OperationQueue_remove, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("waitCompletions", NULL, arginfo_class_Io_OperationQueue_waitCompletions, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("countPending", NULL, arginfo_class_Io_OperationQueue_countPending, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("getHookCapabilities", NULL, arginfo_class_Io_OperationQueue_getHookCapabilities, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_Io_Operation_Read_methods[] = {
	ZEND_ME(Io_Operation_Read, getLength, arginfo_class_Io_Operation_Read_getLength, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Operation_Read, getOffset, arginfo_class_Io_Operation_Read_getOffset, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Io_Operation_Write_methods[] = {
	ZEND_ME(Io_Operation_Write, getLength, arginfo_class_Io_Operation_Write_getLength, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Operation_Write, getOffset, arginfo_class_Io_Operation_Write_getOffset, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Io_Operation_Recv_methods[] = {
	ZEND_ME(Io_Operation_Recv, getLength, arginfo_class_Io_Operation_Recv_getLength, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Operation_Recv, getFlags, arginfo_class_Io_Operation_Recv_getFlags, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Io_Operation_Send_methods[] = {
	ZEND_ME(Io_Operation_Send, getLength, arginfo_class_Io_Operation_Send_getLength, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Operation_Send, getFlags, arginfo_class_Io_Operation_Send_getFlags, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Io_Operation_Connect_methods[] = {
	ZEND_ME(Io_Operation_Connect, getAddress, arginfo_class_Io_Operation_Connect_getAddress, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Io_Operation_Fsync_methods[] = {
	ZEND_ME(Io_Operation_Fsync, isDataOnly, arginfo_class_Io_Operation_Fsync_isDataOnly, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Io_Operation_WaitPid_methods[] = {
	ZEND_ME(Io_Operation_WaitPid, getPid, arginfo_class_Io_Operation_WaitPid_getPid, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Io_Operation_SigWait_methods[] = {
	ZEND_ME(Io_Operation_SigWait, getSignals, arginfo_class_Io_Operation_SigWait_getSignals, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Io_Operation_GetAddrInfo_methods[] = {
	ZEND_ME(Io_Operation_GetAddrInfo, getHost, arginfo_class_Io_Operation_GetAddrInfo_getHost, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Operation_GetAddrInfo, getService, arginfo_class_Io_Operation_GetAddrInfo_getService, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Operation_GetAddrInfo, completeWithAddresses, arginfo_class_Io_Operation_GetAddrInfo_completeWithAddresses, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Io_Operation_GetNameInfo_methods[] = {
	ZEND_ME(Io_Operation_GetNameInfo, getAddress, arginfo_class_Io_Operation_GetNameInfo_getAddress, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Operation_GetNameInfo, completeWithName, arginfo_class_Io_Operation_GetNameInfo_completeWithName, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Io_Operation_Any_methods[] = {
	ZEND_ME(Io_Operation_Any, getOperations, arginfo_class_Io_Operation_Any_getOperations, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Operation_Any, completeWith, arginfo_class_Io_Operation_Any_completeWith, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Io_Poll_OperationQueue_methods[] = {
	ZEND_ME(Io_Poll_OperationQueue, __construct, arginfo_class_Io_Poll_OperationQueue___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Poll_OperationQueue, getContext, arginfo_class_Io_Poll_OperationQueue_getContext, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Poll_OperationQueue, submit, arginfo_class_Io_Poll_OperationQueue_submit, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Poll_OperationQueue, cancel, arginfo_class_Io_Poll_OperationQueue_cancel, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Poll_OperationQueue, add, arginfo_class_Io_Poll_OperationQueue_add, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Poll_OperationQueue, remove, arginfo_class_Io_Poll_OperationQueue_remove, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Poll_OperationQueue, waitCompletions, arginfo_class_Io_Poll_OperationQueue_waitCompletions, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Poll_OperationQueue, countPending, arginfo_class_Io_Poll_OperationQueue_countPending, ZEND_ACC_PUBLIC)
	ZEND_ME(Io_Poll_OperationQueue, getHookCapabilities, arginfo_class_Io_Poll_OperationQueue_getHookCapabilities, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Io_Hooks_Hooks_methods[] = {
	ZEND_RAW_FENTRY("getCapabilities", NULL, arginfo_class_Io_Hooks_Hooks_getCapabilities, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("run", NULL, arginfo_class_Io_Hooks_Hooks_run, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("add", NULL, arginfo_class_Io_Hooks_Hooks_add, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("remove", NULL, arginfo_class_Io_Hooks_Hooks_remove, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_FE_END
};

static zend_class_entry *register_class_Io_CompletionStatus(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("Io\\CompletionStatus", IS_UNDEF, NULL);

	zend_enum_add_case_cstr(class_entry, "Done", NULL);

	zend_enum_add_case_cstr(class_entry, "Ready", NULL);

	zend_enum_add_case_cstr(class_entry, "Timeout", NULL);

	zend_enum_add_case_cstr(class_entry, "Interrupted", NULL);

	zend_enum_add_case_cstr(class_entry, "Cancelled", NULL);

	zend_enum_add_case_cstr(class_entry, "Unsupported", NULL);

	return class_entry;
}

static zend_class_entry *register_class_Io_Operation(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io", "Operation", class_Io_Operation_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_ABSTRACT|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	return class_entry;
}

static zend_class_entry *register_class_Io_Completion(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io", "Completion", class_Io_Completion_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	return class_entry;
}

static zend_class_entry *register_class_Io_InvalidOperationException(zend_class_entry *class_entry_Io_IoException)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io", "InvalidOperationException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Io_IoException, 0);

	return class_entry;
}

static zend_class_entry *register_class_Io_Registration(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io", "Registration", class_Io_Registration_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	return class_entry;
}

static zend_class_entry *register_class_Io_InvalidRegistrationException(zend_class_entry *class_entry_Io_IoException)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io", "InvalidRegistrationException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Io_IoException, 0);

	return class_entry;
}

static zend_class_entry *register_class_Io_OperationQueue(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io", "OperationQueue", class_Io_OperationQueue_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}

static zend_class_entry *register_class_Io_Operation_Poll(zend_class_entry *class_entry_Io_Operation)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Operation", "Poll", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Io_Operation, ZEND_ACC_FINAL);

	return class_entry;
}

static zend_class_entry *register_class_Io_Operation_Timer(zend_class_entry *class_entry_Io_Operation)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Operation", "Timer", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Io_Operation, ZEND_ACC_FINAL);

	return class_entry;
}

static zend_class_entry *register_class_Io_Operation_Read(zend_class_entry *class_entry_Io_Operation)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Operation", "Read", class_Io_Operation_Read_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Io_Operation, ZEND_ACC_FINAL);

	return class_entry;
}

static zend_class_entry *register_class_Io_Operation_Write(zend_class_entry *class_entry_Io_Operation)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Operation", "Write", class_Io_Operation_Write_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Io_Operation, ZEND_ACC_FINAL);

	return class_entry;
}

static zend_class_entry *register_class_Io_Operation_Recv(zend_class_entry *class_entry_Io_Operation)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Operation", "Recv", class_Io_Operation_Recv_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Io_Operation, ZEND_ACC_FINAL);

	return class_entry;
}

static zend_class_entry *register_class_Io_Operation_Send(zend_class_entry *class_entry_Io_Operation)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Operation", "Send", class_Io_Operation_Send_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Io_Operation, ZEND_ACC_FINAL);

	return class_entry;
}

static zend_class_entry *register_class_Io_Operation_Accept(zend_class_entry *class_entry_Io_Operation)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Operation", "Accept", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Io_Operation, ZEND_ACC_FINAL);

	return class_entry;
}

static zend_class_entry *register_class_Io_Operation_Connect(zend_class_entry *class_entry_Io_Operation)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Operation", "Connect", class_Io_Operation_Connect_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Io_Operation, ZEND_ACC_FINAL);

	return class_entry;
}

static zend_class_entry *register_class_Io_Operation_Fsync(zend_class_entry *class_entry_Io_Operation)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Operation", "Fsync", class_Io_Operation_Fsync_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Io_Operation, ZEND_ACC_FINAL);

	return class_entry;
}

static zend_class_entry *register_class_Io_Operation_WaitPid(zend_class_entry *class_entry_Io_Operation)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Operation", "WaitPid", class_Io_Operation_WaitPid_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Io_Operation, ZEND_ACC_FINAL);

	return class_entry;
}

static zend_class_entry *register_class_Io_Operation_SigWait(zend_class_entry *class_entry_Io_Operation)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Operation", "SigWait", class_Io_Operation_SigWait_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Io_Operation, ZEND_ACC_FINAL);

	return class_entry;
}

static zend_class_entry *register_class_Io_Operation_GetAddrInfo(zend_class_entry *class_entry_Io_Operation)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Operation", "GetAddrInfo", class_Io_Operation_GetAddrInfo_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Io_Operation, ZEND_ACC_FINAL);

	return class_entry;
}

static zend_class_entry *register_class_Io_Operation_GetNameInfo(zend_class_entry *class_entry_Io_Operation)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Operation", "GetNameInfo", class_Io_Operation_GetNameInfo_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Io_Operation, ZEND_ACC_FINAL);

	return class_entry;
}

static zend_class_entry *register_class_Io_Operation_Any(zend_class_entry *class_entry_Io_Operation)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Operation", "Any", class_Io_Operation_Any_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Io_Operation, ZEND_ACC_FINAL);

	return class_entry;
}

static zend_class_entry *register_class_Io_Poll_Trigger(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("Io\\Poll\\Trigger", IS_UNDEF, NULL);

	zend_enum_add_case_cstr(class_entry, "Edge", NULL);

	zend_enum_add_case_cstr(class_entry, "Level", NULL);

	return class_entry;
}

static zend_class_entry *register_class_Io_Poll_OperationQueue(zend_class_entry *class_entry_Io_OperationQueue)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Poll", "OperationQueue", class_Io_Poll_OperationQueue_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);
	zend_class_implements(class_entry, 1, class_entry_Io_OperationQueue);

	return class_entry;
}

static zend_class_entry *register_class_Io_Hooks_Hooks(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Io\\Hooks", "Hooks", class_Io_Hooks_Hooks_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}

static zend_class_entry *register_class_Io_Hooks_Capability(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("Io\\Hooks\\Capability", IS_UNDEF, NULL);

	zend_enum_add_case_cstr(class_entry, "Files", NULL);

	zend_enum_add_case_cstr(class_entry, "DirectData", NULL);

	zend_enum_add_case_cstr(class_entry, "DirectAccept", NULL);

	zend_enum_add_case_cstr(class_entry, "EdgeRegistrations", NULL);

	zend_enum_add_case_cstr(class_entry, "LevelRegistrations", NULL);

	return class_entry;
}
