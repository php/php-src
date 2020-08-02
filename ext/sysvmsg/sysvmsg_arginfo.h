/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 4954f0d4dd0515dea5e4305b9db9d3344bdf40a4 */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_msg_get_queue, 0, 1, SysvMessageQueue, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, perms, IS_LONG, 0, "0666")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_msg_send, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, queue, SysvMessageQueue, 0)
	ZEND_ARG_TYPE_INFO(0, msgtype, IS_LONG, 0)
	ZEND_ARG_INFO(0, message)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, serialize, _IS_BOOL, 0, "true")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, blocking, _IS_BOOL, 0, "true")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, errorcode, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_msg_receive, 0, 5, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, queue, SysvMessageQueue, 0)
	ZEND_ARG_TYPE_INFO(0, desiredmsgtype, IS_LONG, 0)
	ZEND_ARG_INFO(1, msgtype)
	ZEND_ARG_TYPE_INFO(0, maxsize, IS_LONG, 0)
	ZEND_ARG_INFO(1, message)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, unserialize, _IS_BOOL, 0, "true")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, errorcode, "null")
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


static const zend_function_entry class_SysvMessageQueue_methods[] = {
	ZEND_FE_END
};
