/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 4221e895bdb0c3e903b7688f79e2863fc0788cee */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_session_name, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, name, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_session_module_name, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, module, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_session_save_path, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, path, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_session_id, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, id, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_session_create_id, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, prefix, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_session_regenerate_id, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, delete_old_session, _IS_BOOL, 0, "false")
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
	ZEND_ARG_TYPE_INFO(0, read, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO(0, write, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO(0, destroy, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO(0, gc, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO(0, create_sid, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO(0, validate_sid, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO(0, update_timestamp, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_session_cache_limiter, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, value, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_session_cache_expire, 0, 0, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, value, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_session_set_cookie_params, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_MASK(0, lifetime_or_options, MAY_BE_ARRAY|MAY_BE_LONG, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, path, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, domain, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, secure, _IS_BOOL, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, httponly, _IS_BOOL, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_session_start, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SessionHandlerInterface_open, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SessionHandlerInterface_close, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SessionHandlerInterface_read, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, id, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SessionHandlerInterface_write, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, id, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SessionHandlerInterface_destroy arginfo_class_SessionHandlerInterface_read

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SessionHandlerInterface_gc, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, max_lifetime, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SessionIdInterface_create_sid arginfo_class_SessionHandlerInterface_close

#define arginfo_class_SessionUpdateTimestampHandlerInterface_validateId arginfo_class_SessionHandlerInterface_read

#define arginfo_class_SessionUpdateTimestampHandlerInterface_updateTimestamp arginfo_class_SessionHandlerInterface_write

#define arginfo_class_SessionHandler_open arginfo_class_SessionHandlerInterface_open

#define arginfo_class_SessionHandler_close arginfo_class_SessionHandlerInterface_close

#define arginfo_class_SessionHandler_read arginfo_class_SessionHandlerInterface_read

#define arginfo_class_SessionHandler_write arginfo_class_SessionHandlerInterface_write

#define arginfo_class_SessionHandler_destroy arginfo_class_SessionHandlerInterface_read

#define arginfo_class_SessionHandler_gc arginfo_class_SessionHandlerInterface_gc

#define arginfo_class_SessionHandler_create_sid arginfo_class_SessionHandlerInterface_close


ZEND_FUNCTION(session_name);
ZEND_FUNCTION(session_module_name);
ZEND_FUNCTION(session_save_path);
ZEND_FUNCTION(session_id);
ZEND_FUNCTION(session_create_id);
ZEND_FUNCTION(session_regenerate_id);
ZEND_FUNCTION(session_decode);
ZEND_FUNCTION(session_encode);
ZEND_FUNCTION(session_destroy);
ZEND_FUNCTION(session_unset);
ZEND_FUNCTION(session_gc);
ZEND_FUNCTION(session_get_cookie_params);
ZEND_FUNCTION(session_write_close);
ZEND_FUNCTION(session_abort);
ZEND_FUNCTION(session_reset);
ZEND_FUNCTION(session_status);
ZEND_FUNCTION(session_register_shutdown);
ZEND_FUNCTION(session_set_save_handler);
ZEND_FUNCTION(session_cache_limiter);
ZEND_FUNCTION(session_cache_expire);
ZEND_FUNCTION(session_set_cookie_params);
ZEND_FUNCTION(session_start);
ZEND_METHOD(SessionHandler, open);
ZEND_METHOD(SessionHandler, close);
ZEND_METHOD(SessionHandler, read);
ZEND_METHOD(SessionHandler, write);
ZEND_METHOD(SessionHandler, destroy);
ZEND_METHOD(SessionHandler, gc);
ZEND_METHOD(SessionHandler, create_sid);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(session_name, arginfo_session_name)
	ZEND_FE(session_module_name, arginfo_session_module_name)
	ZEND_FE(session_save_path, arginfo_session_save_path)
	ZEND_FE(session_id, arginfo_session_id)
	ZEND_FE(session_create_id, arginfo_session_create_id)
	ZEND_FE(session_regenerate_id, arginfo_session_regenerate_id)
	ZEND_FE(session_decode, arginfo_session_decode)
	ZEND_FE(session_encode, arginfo_session_encode)
	ZEND_FE(session_destroy, arginfo_session_destroy)
	ZEND_FE(session_unset, arginfo_session_unset)
	ZEND_FE(session_gc, arginfo_session_gc)
	ZEND_FE(session_get_cookie_params, arginfo_session_get_cookie_params)
	ZEND_FE(session_write_close, arginfo_session_write_close)
	ZEND_FE(session_abort, arginfo_session_abort)
	ZEND_FE(session_reset, arginfo_session_reset)
	ZEND_FE(session_status, arginfo_session_status)
	ZEND_FE(session_register_shutdown, arginfo_session_register_shutdown)
	ZEND_FALIAS(session_commit, session_write_close, arginfo_session_commit)
	ZEND_FE(session_set_save_handler, arginfo_session_set_save_handler)
	ZEND_FE(session_cache_limiter, arginfo_session_cache_limiter)
	ZEND_FE(session_cache_expire, arginfo_session_cache_expire)
	ZEND_FE(session_set_cookie_params, arginfo_session_set_cookie_params)
	ZEND_FE(session_start, arginfo_session_start)
	ZEND_FE_END
};


static const zend_function_entry class_SessionHandlerInterface_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(SessionHandlerInterface, open, arginfo_class_SessionHandlerInterface_open, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(SessionHandlerInterface, close, arginfo_class_SessionHandlerInterface_close, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(SessionHandlerInterface, read, arginfo_class_SessionHandlerInterface_read, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(SessionHandlerInterface, write, arginfo_class_SessionHandlerInterface_write, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(SessionHandlerInterface, destroy, arginfo_class_SessionHandlerInterface_destroy, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(SessionHandlerInterface, gc, arginfo_class_SessionHandlerInterface_gc, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};


static const zend_function_entry class_SessionIdInterface_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(SessionIdInterface, create_sid, arginfo_class_SessionIdInterface_create_sid, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};


static const zend_function_entry class_SessionUpdateTimestampHandlerInterface_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(SessionUpdateTimestampHandlerInterface, validateId, arginfo_class_SessionUpdateTimestampHandlerInterface_validateId, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(SessionUpdateTimestampHandlerInterface, updateTimestamp, arginfo_class_SessionUpdateTimestampHandlerInterface_updateTimestamp, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};


static const zend_function_entry class_SessionHandler_methods[] = {
	ZEND_ME(SessionHandler, open, arginfo_class_SessionHandler_open, ZEND_ACC_PUBLIC)
	ZEND_ME(SessionHandler, close, arginfo_class_SessionHandler_close, ZEND_ACC_PUBLIC)
	ZEND_ME(SessionHandler, read, arginfo_class_SessionHandler_read, ZEND_ACC_PUBLIC)
	ZEND_ME(SessionHandler, write, arginfo_class_SessionHandler_write, ZEND_ACC_PUBLIC)
	ZEND_ME(SessionHandler, destroy, arginfo_class_SessionHandler_destroy, ZEND_ACC_PUBLIC)
	ZEND_ME(SessionHandler, gc, arginfo_class_SessionHandler_gc, ZEND_ACC_PUBLIC)
	ZEND_ME(SessionHandler, create_sid, arginfo_class_SessionHandler_create_sid, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
