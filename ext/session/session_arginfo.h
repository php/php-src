/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_session_name, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_session_module_name, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, module, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_session_save_path, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_session_id, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, id, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_session_create_id, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_session_regenerate_id, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, delete_old_session, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_session_decode, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_session_encode, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_session_destroy, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_session_unset arginfo_session_destroy

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_session_gc, 0, 0, MAY_BE_LONG|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_session_get_cookie_params, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_session_write_close arginfo_session_destroy

#define arginfo_session_abort arginfo_session_destroy

#define arginfo_session_reset arginfo_session_destroy

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_session_status, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_session_register_shutdown, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_session_commit arginfo_session_destroy

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_session_set_save_handler, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, open)
	ZEND_ARG_INFO(0, close)
	ZEND_ARG_INFO(0, read)
	ZEND_ARG_INFO(0, write)
	ZEND_ARG_INFO(0, destroy)
	ZEND_ARG_INFO(0, gc)
	ZEND_ARG_INFO(0, create_sid)
	ZEND_ARG_INFO(0, validate_sid)
	ZEND_ARG_INFO(0, update_timestamp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_session_cache_limiter, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, cache_limiter, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_session_cache_expire, 0, 0, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, new_cache_expire, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_session_set_cookie_params, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, lifetime_or_options)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, domain, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, secure, _IS_BOOL, 1)
	ZEND_ARG_TYPE_INFO(0, httponly, _IS_BOOL, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_session_start, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SessionHandlerInterface_open, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, save_path, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, session_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SessionHandlerInterface_close, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SessionHandlerInterface_read, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SessionHandlerInterface_write, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, val, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SessionHandlerInterface_destroy arginfo_class_SessionHandlerInterface_read

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SessionHandlerInterface_gc, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, maxlifetime, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SessionIdInterface_create_sid arginfo_class_SessionHandlerInterface_close

#define arginfo_class_SessionUpdateTimestampHandlerInterface_validateId arginfo_class_SessionHandlerInterface_read

#define arginfo_class_SessionUpdateTimestampHandlerInterface_updateTimestamp arginfo_class_SessionHandlerInterface_write
