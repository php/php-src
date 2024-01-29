/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 6bbbdc8c4a33d1ff9984b3d81e4f5c9b76efcb14 */

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
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, create_sid, IS_CALLABLE, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, validate_sid, IS_CALLABLE, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, update_timestamp, IS_CALLABLE, 1, "null")
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

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SessionHandlerInterface_open, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SessionHandlerInterface_close, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_SessionHandlerInterface_read, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, id, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SessionHandlerInterface_write, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, id, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SessionHandlerInterface_destroy, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, id, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_SessionHandlerInterface_gc, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, max_lifetime, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SessionIdInterface_create_sid, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SessionUpdateTimestampHandlerInterface_validateId arginfo_class_SessionHandlerInterface_destroy

#define arginfo_class_SessionUpdateTimestampHandlerInterface_updateTimestamp arginfo_class_SessionHandlerInterface_write

#define arginfo_class_SessionHandler_open arginfo_class_SessionHandlerInterface_open

#define arginfo_class_SessionHandler_close arginfo_class_SessionHandlerInterface_close

#define arginfo_class_SessionHandler_read arginfo_class_SessionHandlerInterface_read

#define arginfo_class_SessionHandler_write arginfo_class_SessionHandlerInterface_write

#define arginfo_class_SessionHandler_destroy arginfo_class_SessionHandlerInterface_destroy

#define arginfo_class_SessionHandler_gc arginfo_class_SessionHandlerInterface_gc

#define arginfo_class_SessionHandler_create_sid arginfo_class_SessionIdInterface_create_sid

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
	ZEND_RAW_FENTRY("session_name", zif_session_name, arginfo_session_name, 0, NULL)
	ZEND_RAW_FENTRY("session_module_name", zif_session_module_name, arginfo_session_module_name, 0, NULL)
	ZEND_RAW_FENTRY("session_save_path", zif_session_save_path, arginfo_session_save_path, 0, NULL)
	ZEND_RAW_FENTRY("session_id", zif_session_id, arginfo_session_id, 0, NULL)
	ZEND_RAW_FENTRY("session_create_id", zif_session_create_id, arginfo_session_create_id, 0, NULL)
	ZEND_RAW_FENTRY("session_regenerate_id", zif_session_regenerate_id, arginfo_session_regenerate_id, 0, NULL)
	ZEND_RAW_FENTRY("session_decode", zif_session_decode, arginfo_session_decode, 0, NULL)
	ZEND_RAW_FENTRY("session_encode", zif_session_encode, arginfo_session_encode, 0, NULL)
	ZEND_RAW_FENTRY("session_destroy", zif_session_destroy, arginfo_session_destroy, 0, NULL)
	ZEND_RAW_FENTRY("session_unset", zif_session_unset, arginfo_session_unset, 0, NULL)
	ZEND_RAW_FENTRY("session_gc", zif_session_gc, arginfo_session_gc, 0, NULL)
	ZEND_RAW_FENTRY("session_get_cookie_params", zif_session_get_cookie_params, arginfo_session_get_cookie_params, 0, NULL)
	ZEND_RAW_FENTRY("session_write_close", zif_session_write_close, arginfo_session_write_close, 0, NULL)
	ZEND_RAW_FENTRY("session_abort", zif_session_abort, arginfo_session_abort, 0, NULL)
	ZEND_RAW_FENTRY("session_reset", zif_session_reset, arginfo_session_reset, 0, NULL)
	ZEND_RAW_FENTRY("session_status", zif_session_status, arginfo_session_status, 0, NULL)
	ZEND_RAW_FENTRY("session_register_shutdown", zif_session_register_shutdown, arginfo_session_register_shutdown, 0, NULL)
	ZEND_RAW_FENTRY("session_commit", zif_session_write_close, arginfo_session_commit, 0, NULL)
	ZEND_RAW_FENTRY("session_set_save_handler", zif_session_set_save_handler, arginfo_session_set_save_handler, 0, NULL)
	ZEND_RAW_FENTRY("session_cache_limiter", zif_session_cache_limiter, arginfo_session_cache_limiter, 0, NULL)
	ZEND_RAW_FENTRY("session_cache_expire", zif_session_cache_expire, arginfo_session_cache_expire, 0, NULL)
	ZEND_RAW_FENTRY("session_set_cookie_params", zif_session_set_cookie_params, arginfo_session_set_cookie_params, 0, NULL)
	ZEND_RAW_FENTRY("session_start", zif_session_start, arginfo_session_start, 0, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_SessionHandlerInterface_methods[] = {
	ZEND_RAW_FENTRY("open", NULL, arginfo_class_SessionHandlerInterface_open, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL)
	ZEND_RAW_FENTRY("close", NULL, arginfo_class_SessionHandlerInterface_close, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL)
	ZEND_RAW_FENTRY("read", NULL, arginfo_class_SessionHandlerInterface_read, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL)
	ZEND_RAW_FENTRY("write", NULL, arginfo_class_SessionHandlerInterface_write, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL)
	ZEND_RAW_FENTRY("destroy", NULL, arginfo_class_SessionHandlerInterface_destroy, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL)
	ZEND_RAW_FENTRY("gc", NULL, arginfo_class_SessionHandlerInterface_gc, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_SessionIdInterface_methods[] = {
	ZEND_RAW_FENTRY("create_sid", NULL, arginfo_class_SessionIdInterface_create_sid, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_SessionUpdateTimestampHandlerInterface_methods[] = {
	ZEND_RAW_FENTRY("validateId", NULL, arginfo_class_SessionUpdateTimestampHandlerInterface_validateId, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL)
	ZEND_RAW_FENTRY("updateTimestamp", NULL, arginfo_class_SessionUpdateTimestampHandlerInterface_updateTimestamp, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_SessionHandler_methods[] = {
	ZEND_RAW_FENTRY("open", zim_SessionHandler_open, arginfo_class_SessionHandler_open, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("close", zim_SessionHandler_close, arginfo_class_SessionHandler_close, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("read", zim_SessionHandler_read, arginfo_class_SessionHandler_read, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("write", zim_SessionHandler_write, arginfo_class_SessionHandler_write, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("destroy", zim_SessionHandler_destroy, arginfo_class_SessionHandler_destroy, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("gc", zim_SessionHandler_gc, arginfo_class_SessionHandler_gc, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("create_sid", zim_SessionHandler_create_sid, arginfo_class_SessionHandler_create_sid, ZEND_ACC_PUBLIC, NULL)
	ZEND_FE_END
};

static void register_session_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("PHP_SESSION_DISABLED", php_session_disabled, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_SESSION_NONE", php_session_none, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_SESSION_ACTIVE", php_session_active, CONST_PERSISTENT);
}

static zend_class_entry *register_class_SessionHandlerInterface(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SessionHandlerInterface", class_SessionHandlerInterface_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}

static zend_class_entry *register_class_SessionIdInterface(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SessionIdInterface", class_SessionIdInterface_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}

static zend_class_entry *register_class_SessionUpdateTimestampHandlerInterface(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SessionUpdateTimestampHandlerInterface", class_SessionUpdateTimestampHandlerInterface_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}

static zend_class_entry *register_class_SessionHandler(zend_class_entry *class_entry_SessionHandlerInterface, zend_class_entry *class_entry_SessionIdInterface)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SessionHandler", class_SessionHandler_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	zend_class_implements(class_entry, 2, class_entry_SessionHandlerInterface, class_entry_SessionIdInterface);

	return class_entry;
}
