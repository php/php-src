/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: f1d616c644ad366405816cde0384f6f391773ebf */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_close, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, handle, CurlHandle, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_curl_copy_handle, 0, 1, CurlHandle, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, handle, CurlHandle, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_errno, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, handle, CurlHandle, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_error, 0, 1, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, handle, CurlHandle, 0)
ZEND_END_ARG_INFO()

#if LIBCURL_VERSION_NUM >= 0x070f04 /* 7.15.4 */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_curl_escape, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, handle, CurlHandle, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if LIBCURL_VERSION_NUM >= 0x070f04 /* 7.15.4 */
#define arginfo_curl_unescape arginfo_curl_escape
#endif

#if LIBCURL_VERSION_NUM >= 0x070f04 /* 7.15.4 */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_multi_setopt, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, multi_handle, CurlMultiHandle, 0)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_curl_exec, 0, 1, MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_OBJ_INFO(0, handle, CurlHandle, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_curl_file_create, 0, 1, CURLFile, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mime_type, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, posted_filename, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_getinfo, 0, 1, IS_MIXED, 0)
	ZEND_ARG_OBJ_INFO(0, handle, CurlHandle, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, option, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_curl_init, 0, 0, CurlHandle, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, url, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_multi_add_handle, 0, 2, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, multi_handle, CurlMultiHandle, 0)
	ZEND_ARG_OBJ_INFO(0, handle, CurlHandle, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_multi_close, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, multi_handle, CurlMultiHandle, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_multi_errno, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, multi_handle, CurlMultiHandle, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_multi_exec, 0, 2, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, multi_handle, CurlMultiHandle, 0)
	ZEND_ARG_INFO(1, still_running)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_multi_getcontent, 0, 1, IS_STRING, 1)
	ZEND_ARG_OBJ_INFO(0, handle, CurlHandle, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_curl_multi_info_read, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, multi_handle, CurlMultiHandle, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, queued_messages, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_curl_multi_init, 0, 0, CurlMultiHandle, 0)
ZEND_END_ARG_INFO()

#define arginfo_curl_multi_remove_handle arginfo_curl_multi_add_handle

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_multi_select, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, multi_handle, CurlMultiHandle, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_DOUBLE, 0, "1.0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_multi_strerror, 0, 1, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, error_code, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if LIBCURL_VERSION_NUM >= 0x071200 /* 7.18.0 */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_pause, 0, 2, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, handle, CurlHandle, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#define arginfo_curl_reset arginfo_curl_close

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_setopt_array, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, handle, CurlHandle, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_setopt, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, handle, CurlHandle, 0)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_share_close, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, share_handle, CurlShareHandle, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_share_errno, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, share_handle, CurlShareHandle, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_curl_share_init, 0, 0, CurlShareHandle, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_curl_share_setopt, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, share_handle, CurlShareHandle, 0)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_curl_share_strerror arginfo_curl_multi_strerror

#define arginfo_curl_strerror arginfo_curl_multi_strerror

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_curl_version, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(curl_close);
ZEND_FUNCTION(curl_copy_handle);
ZEND_FUNCTION(curl_errno);
ZEND_FUNCTION(curl_error);
#if LIBCURL_VERSION_NUM >= 0x070f04 /* 7.15.4 */
ZEND_FUNCTION(curl_escape);
#endif
#if LIBCURL_VERSION_NUM >= 0x070f04 /* 7.15.4 */
ZEND_FUNCTION(curl_unescape);
#endif
#if LIBCURL_VERSION_NUM >= 0x070f04 /* 7.15.4 */
ZEND_FUNCTION(curl_multi_setopt);
#endif
ZEND_FUNCTION(curl_exec);
ZEND_FUNCTION(curl_file_create);
ZEND_FUNCTION(curl_getinfo);
ZEND_FUNCTION(curl_init);
ZEND_FUNCTION(curl_multi_add_handle);
ZEND_FUNCTION(curl_multi_close);
ZEND_FUNCTION(curl_multi_errno);
ZEND_FUNCTION(curl_multi_exec);
ZEND_FUNCTION(curl_multi_getcontent);
ZEND_FUNCTION(curl_multi_info_read);
ZEND_FUNCTION(curl_multi_init);
ZEND_FUNCTION(curl_multi_remove_handle);
ZEND_FUNCTION(curl_multi_select);
ZEND_FUNCTION(curl_multi_strerror);
#if LIBCURL_VERSION_NUM >= 0x071200 /* 7.18.0 */
ZEND_FUNCTION(curl_pause);
#endif
ZEND_FUNCTION(curl_reset);
ZEND_FUNCTION(curl_setopt_array);
ZEND_FUNCTION(curl_setopt);
ZEND_FUNCTION(curl_share_close);
ZEND_FUNCTION(curl_share_errno);
ZEND_FUNCTION(curl_share_init);
ZEND_FUNCTION(curl_share_setopt);
ZEND_FUNCTION(curl_share_strerror);
ZEND_FUNCTION(curl_strerror);
ZEND_FUNCTION(curl_version);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(curl_close, arginfo_curl_close)
	ZEND_FE(curl_copy_handle, arginfo_curl_copy_handle)
	ZEND_FE(curl_errno, arginfo_curl_errno)
	ZEND_FE(curl_error, arginfo_curl_error)
#if LIBCURL_VERSION_NUM >= 0x070f04 /* 7.15.4 */
	ZEND_FE(curl_escape, arginfo_curl_escape)
#endif
#if LIBCURL_VERSION_NUM >= 0x070f04 /* 7.15.4 */
	ZEND_FE(curl_unescape, arginfo_curl_unescape)
#endif
#if LIBCURL_VERSION_NUM >= 0x070f04 /* 7.15.4 */
	ZEND_FE(curl_multi_setopt, arginfo_curl_multi_setopt)
#endif
	ZEND_FE(curl_exec, arginfo_curl_exec)
	ZEND_FE(curl_file_create, arginfo_curl_file_create)
	ZEND_FE(curl_getinfo, arginfo_curl_getinfo)
	ZEND_FE(curl_init, arginfo_curl_init)
	ZEND_FE(curl_multi_add_handle, arginfo_curl_multi_add_handle)
	ZEND_FE(curl_multi_close, arginfo_curl_multi_close)
	ZEND_FE(curl_multi_errno, arginfo_curl_multi_errno)
	ZEND_FE(curl_multi_exec, arginfo_curl_multi_exec)
	ZEND_FE(curl_multi_getcontent, arginfo_curl_multi_getcontent)
	ZEND_FE(curl_multi_info_read, arginfo_curl_multi_info_read)
	ZEND_FE(curl_multi_init, arginfo_curl_multi_init)
	ZEND_FE(curl_multi_remove_handle, arginfo_curl_multi_remove_handle)
	ZEND_FE(curl_multi_select, arginfo_curl_multi_select)
	ZEND_FE(curl_multi_strerror, arginfo_curl_multi_strerror)
#if LIBCURL_VERSION_NUM >= 0x071200 /* 7.18.0 */
	ZEND_FE(curl_pause, arginfo_curl_pause)
#endif
	ZEND_FE(curl_reset, arginfo_curl_reset)
	ZEND_FE(curl_setopt_array, arginfo_curl_setopt_array)
	ZEND_FE(curl_setopt, arginfo_curl_setopt)
	ZEND_FE(curl_share_close, arginfo_curl_share_close)
	ZEND_FE(curl_share_errno, arginfo_curl_share_errno)
	ZEND_FE(curl_share_init, arginfo_curl_share_init)
	ZEND_FE(curl_share_setopt, arginfo_curl_share_setopt)
	ZEND_FE(curl_share_strerror, arginfo_curl_share_strerror)
	ZEND_FE(curl_strerror, arginfo_curl_strerror)
	ZEND_FE(curl_version, arginfo_curl_version)
	ZEND_FE_END
};


static const zend_function_entry class_CurlHandle_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_CurlMultiHandle_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_CurlShareHandle_methods[] = {
	ZEND_FE_END
};
