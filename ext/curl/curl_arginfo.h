/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_close, 0, 1, IS_VOID, 0)
	ZEND_ARG_INFO(0, handle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_curl_copy_handle, 0, 0, 1)
	ZEND_ARG_INFO(0, handle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_errno, 0, 1, IS_LONG, 0)
	ZEND_ARG_INFO(0, handle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_error, 0, 1, IS_STRING, 0)
	ZEND_ARG_INFO(0, handle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_curl_escape, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, handle)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_curl_exec, 0, 1, MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_INFO(0, handle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_curl_file_create, 0, 1, CURLFile, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, mimetype, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, postname, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_curl_getinfo, 0, 0, 1)
	ZEND_ARG_INFO(0, handle)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_curl_init, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, url, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_multi_add_handle, 0, 2, IS_LONG, 0)
	ZEND_ARG_INFO(0, multi_handle)
	ZEND_ARG_INFO(0, handle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_multi_close, 0, 1, IS_VOID, 0)
	ZEND_ARG_INFO(0, multi_handle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_multi_errno, 0, 1, IS_LONG, 0)
	ZEND_ARG_INFO(0, multi_handle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_multi_exec, 0, 2, IS_LONG, 0)
	ZEND_ARG_INFO(0, multi_handle)
	ZEND_ARG_INFO(1, still_running)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_multi_getcontent, 0, 1, IS_STRING, 1)
	ZEND_ARG_INFO(0, multi_handle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_curl_multi_info_read, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, multi_handle)
	ZEND_ARG_INFO(1, msgs_in_queue)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_curl_multi_init, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_curl_multi_remove_handle arginfo_curl_multi_add_handle

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_multi_select, 0, 1, IS_LONG, 0)
	ZEND_ARG_INFO(0, multi_handle)
	ZEND_ARG_TYPE_INFO(0, timeout, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_multi_setopt, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, multi_handle)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_multi_strerror, 0, 1, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, error_number, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if LIBCURL_VERSION_NUM >= 0x071200 /* 7.18.0 */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_pause, 0, 2, IS_LONG, 0)
	ZEND_ARG_INFO(0, handle)
	ZEND_ARG_TYPE_INFO(0, bitmask, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#define arginfo_curl_reset arginfo_curl_close

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_setopt_array, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, handle)
	ZEND_ARG_TYPE_INFO(0, options, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_setopt, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, handle)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_share_close, 0, 1, IS_VOID, 0)
	ZEND_ARG_INFO(0, share_handle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_share_errno, 0, 1, IS_LONG, 0)
	ZEND_ARG_INFO(0, share_handle)
ZEND_END_ARG_INFO()

#define arginfo_curl_share_init arginfo_curl_multi_init

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_share_setopt, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, share_handle)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

#define arginfo_curl_share_strerror arginfo_curl_multi_strerror

#define arginfo_curl_strerror arginfo_curl_multi_strerror

#define arginfo_curl_unescape arginfo_curl_escape

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_curl_version, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, age, IS_LONG, 0)
ZEND_END_ARG_INFO()
