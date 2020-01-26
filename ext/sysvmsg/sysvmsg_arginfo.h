/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_msg_get_queue, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, key, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, perms, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_msg_send, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, queue)
	ZEND_ARG_TYPE_INFO(0, msgtype, IS_LONG, 0)
	ZEND_ARG_INFO(0, message)
	ZEND_ARG_TYPE_INFO(0, serialize, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, blocking, _IS_BOOL, 0)
	ZEND_ARG_INFO(1, errorcode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_msg_receive, 0, 5, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, queue)
	ZEND_ARG_TYPE_INFO(0, desiredmsgtype, IS_LONG, 0)
	ZEND_ARG_INFO(1, msgtype)
	ZEND_ARG_TYPE_INFO(0, maxsize, IS_LONG, 0)
	ZEND_ARG_INFO(1, message)
	ZEND_ARG_TYPE_INFO(0, unserialize, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
	ZEND_ARG_INFO(1, errorcode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_msg_remove_queue, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, queue)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_msg_stat_queue, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, queue)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_msg_set_queue, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, queue)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_msg_queue_exists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_LONG, 0)
ZEND_END_ARG_INFO()
