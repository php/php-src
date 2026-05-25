/* This is a generated file, edit sysvmsg.stub.php instead.
 * Stub hash: 43f34e025ca7978faedc724af45952fe3fd16d4f */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_msg_get_queue, 0, 1, SysvMessageQueue, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, permissions, IS_LONG, 0, "0666")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_msg_send, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, queue, SysvMessageQueue, 0)
	ZEND_ARG_TYPE_INFO(0, message_type, IS_LONG, 0)
	ZEND_ARG_INFO(0, message)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, serialize, _IS_BOOL, 0, "true")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, blocking, _IS_BOOL, 0, "true")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, error_code, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_msg_receive, 0, 5, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, queue, SysvMessageQueue, 0)
	ZEND_ARG_TYPE_INFO(0, desired_message_type, IS_LONG, 0)
	ZEND_ARG_INFO(1, received_message_type)
	ZEND_ARG_TYPE_INFO(0, max_message_size, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(1, message, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, unserialize, _IS_BOOL, 0, "true")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, error_code, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_msg_remove_queue, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, queue, SysvMessageQueue, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_msg_stat_queue, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, queue, SysvMessageQueue, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_msg_set_queue, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, queue, SysvMessageQueue, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_msg_queue_exists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_FUNCTION(msg_get_queue);
ZEND_FUNCTION(msg_send);
ZEND_FUNCTION(msg_receive);
ZEND_FUNCTION(msg_remove_queue);
ZEND_FUNCTION(msg_stat_queue);
ZEND_FUNCTION(msg_set_queue);
ZEND_FUNCTION(msg_queue_exists);

static const zend_function_entry ext_functions[] = {
	ZEND_FE(msg_get_queue, arginfo_msg_get_queue)
	ZEND_FE(msg_send, arginfo_msg_send)
	ZEND_FE(msg_receive, arginfo_msg_receive)
	ZEND_FE(msg_remove_queue, arginfo_msg_remove_queue)
	ZEND_FE(msg_stat_queue, arginfo_msg_stat_queue)
	ZEND_FE(msg_set_queue, arginfo_msg_set_queue)
	ZEND_FE(msg_queue_exists, arginfo_msg_queue_exists)
	ZEND_FE_END
};

static void register_sysvmsg_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("MSG_IPC_NOWAIT", PHP_MSG_IPC_NOWAIT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MSG_EAGAIN", EAGAIN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MSG_ENOMSG", ENOMSG, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MSG_NOERROR", PHP_MSG_NOERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MSG_EXCEPT", PHP_MSG_EXCEPT, CONST_PERSISTENT);
}

static zend_class_entry *register_class_SysvMessageQueue(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SysvMessageQueue", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	zend_string *attribute_name_NonInstantiableClass_class_SysvMessageQueue_0 = zend_string_init_interned("NonInstantiableClass", sizeof("NonInstantiableClass") - 1, true);
	zend_attribute *attribute_NonInstantiableClass_class_SysvMessageQueue_0 = zend_add_class_attribute(class_entry, attribute_name_NonInstantiableClass_class_SysvMessageQueue_0, 1);
	zend_string_release_ex(attribute_name_NonInstantiableClass_class_SysvMessageQueue_0, true);
	zend_string *attribute_NonInstantiableClass_class_SysvMessageQueue_0_arg0_str = zend_string_init("Cannot directly construct SysvMessageQueue, use msg_get_queue() instead", strlen("Cannot directly construct SysvMessageQueue, use msg_get_queue() instead"), 1);
	ZVAL_STR(&attribute_NonInstantiableClass_class_SysvMessageQueue_0->args[0].value, attribute_NonInstantiableClass_class_SysvMessageQueue_0_arg0_str);

	class_entry->constructor = (zend_function *) &zend_non_instantiable_constructor;

	return class_entry;
}
