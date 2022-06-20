/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 36f24eb2bf05e40500e1c588f39652c34b16526f */

#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_CurlUrl___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, url, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_CurlUrl_get, 0, 0, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_CurlUrl_set, 0, 1, CurlUrl, 0)
	ZEND_ARG_TYPE_INFO(0, url, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_CurlUrl_getHost, 0, 0, IS_STRING, 1)
ZEND_END_ARG_INFO()
#endif

#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_CurlUrl_setHost, 0, 1, CurlUrl, 0)
	ZEND_ARG_TYPE_INFO(0, host, IS_STRING, 1)
ZEND_END_ARG_INFO()
#endif

#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
#define arginfo_class_CurlUrl_getScheme arginfo_class_CurlUrl_getHost
#endif

#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_CurlUrl_setScheme, 0, 1, CurlUrl, 0)
	ZEND_ARG_TYPE_INFO(0, scheme, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_CurlUrl_getPort, 0, 0, IS_LONG, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_CurlUrl_setPort, 0, 1, CurlUrl, 0)
	ZEND_ARG_TYPE_INFO(0, port, IS_LONG, 1)
ZEND_END_ARG_INFO()
#endif

#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
#define arginfo_class_CurlUrl_getPath arginfo_class_CurlUrl_get
#endif

#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
#define arginfo_class_CurlUrl_setPath arginfo_class_CurlUrl_setScheme
#endif

#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_CurlUrl_getQuery, 0, 0, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_CurlUrl_setQuery, 0, 1, CurlUrl, 0)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
#define arginfo_class_CurlUrl_getFragment arginfo_class_CurlUrl_getQuery
#endif

#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_CurlUrl_setFragment, 0, 1, CurlUrl, 0)
	ZEND_ARG_TYPE_INFO(0, fragment, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
#define arginfo_class_CurlUrl_getUser arginfo_class_CurlUrl_getQuery
#endif

#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_CurlUrl_setUser, 0, 1, CurlUrl, 0)
	ZEND_ARG_TYPE_INFO(0, user, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
#define arginfo_class_CurlUrl_getPassword arginfo_class_CurlUrl_getQuery
#endif

#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_CurlUrl_setPassword, 0, 1, CurlUrl, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
#define arginfo_class_CurlUrl_getOptions arginfo_class_CurlUrl_getQuery
#endif

#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_CurlUrl_setOptions, 0, 1, CurlUrl, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */ && LIBCURL_VERSION_NUM >= 0x074100 /* Available since 7.65.0 */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_CurlUrl_getZoneId, 0, 0, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */ && LIBCURL_VERSION_NUM >= 0x074100 /* Available since 7.65.0 */
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_CurlUrl_setZoneId, 0, 1, CurlUrl, 0)
	ZEND_ARG_TYPE_INFO(0, zoneid, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_CurlUrl___toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif


#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_METHOD(CurlUrl, __construct);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_METHOD(CurlUrl, get);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_METHOD(CurlUrl, set);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_METHOD(CurlUrl, getHost);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_METHOD(CurlUrl, setHost);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_METHOD(CurlUrl, getScheme);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_METHOD(CurlUrl, setScheme);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_METHOD(CurlUrl, getPort);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_METHOD(CurlUrl, setPort);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_METHOD(CurlUrl, getPath);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_METHOD(CurlUrl, setPath);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_METHOD(CurlUrl, getQuery);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_METHOD(CurlUrl, setQuery);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_METHOD(CurlUrl, getFragment);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_METHOD(CurlUrl, setFragment);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_METHOD(CurlUrl, getUser);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_METHOD(CurlUrl, setUser);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_METHOD(CurlUrl, getPassword);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_METHOD(CurlUrl, setPassword);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_METHOD(CurlUrl, getOptions);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_METHOD(CurlUrl, setOptions);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */ && LIBCURL_VERSION_NUM >= 0x074100 /* Available since 7.65.0 */
ZEND_METHOD(CurlUrl, getZoneId);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */ && LIBCURL_VERSION_NUM >= 0x074100 /* Available since 7.65.0 */
ZEND_METHOD(CurlUrl, setZoneId);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
ZEND_METHOD(CurlUrl, __toString);
#endif


#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
static const zend_function_entry class_CurlUrl_methods[] = {
	ZEND_ME(CurlUrl, __construct, arginfo_class_CurlUrl___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(CurlUrl, get, arginfo_class_CurlUrl_get, ZEND_ACC_PUBLIC)
	ZEND_ME(CurlUrl, set, arginfo_class_CurlUrl_set, ZEND_ACC_PUBLIC)
	ZEND_ME(CurlUrl, getHost, arginfo_class_CurlUrl_getHost, ZEND_ACC_PUBLIC)
	ZEND_ME(CurlUrl, setHost, arginfo_class_CurlUrl_setHost, ZEND_ACC_PUBLIC)
	ZEND_ME(CurlUrl, getScheme, arginfo_class_CurlUrl_getScheme, ZEND_ACC_PUBLIC)
	ZEND_ME(CurlUrl, setScheme, arginfo_class_CurlUrl_setScheme, ZEND_ACC_PUBLIC)
	ZEND_ME(CurlUrl, getPort, arginfo_class_CurlUrl_getPort, ZEND_ACC_PUBLIC)
	ZEND_ME(CurlUrl, setPort, arginfo_class_CurlUrl_setPort, ZEND_ACC_PUBLIC)
	ZEND_ME(CurlUrl, getPath, arginfo_class_CurlUrl_getPath, ZEND_ACC_PUBLIC)
	ZEND_ME(CurlUrl, setPath, arginfo_class_CurlUrl_setPath, ZEND_ACC_PUBLIC)
	ZEND_ME(CurlUrl, getQuery, arginfo_class_CurlUrl_getQuery, ZEND_ACC_PUBLIC)
	ZEND_ME(CurlUrl, setQuery, arginfo_class_CurlUrl_setQuery, ZEND_ACC_PUBLIC)
	ZEND_ME(CurlUrl, getFragment, arginfo_class_CurlUrl_getFragment, ZEND_ACC_PUBLIC)
	ZEND_ME(CurlUrl, setFragment, arginfo_class_CurlUrl_setFragment, ZEND_ACC_PUBLIC)
	ZEND_ME(CurlUrl, getUser, arginfo_class_CurlUrl_getUser, ZEND_ACC_PUBLIC)
	ZEND_ME(CurlUrl, setUser, arginfo_class_CurlUrl_setUser, ZEND_ACC_PUBLIC)
	ZEND_ME(CurlUrl, getPassword, arginfo_class_CurlUrl_getPassword, ZEND_ACC_PUBLIC)
	ZEND_ME(CurlUrl, setPassword, arginfo_class_CurlUrl_setPassword, ZEND_ACC_PUBLIC)
	ZEND_ME(CurlUrl, getOptions, arginfo_class_CurlUrl_getOptions, ZEND_ACC_PUBLIC)
	ZEND_ME(CurlUrl, setOptions, arginfo_class_CurlUrl_setOptions, ZEND_ACC_PUBLIC)
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */ && LIBCURL_VERSION_NUM >= 0x074100 /* Available since 7.65.0 */
	ZEND_ME(CurlUrl, getZoneId, arginfo_class_CurlUrl_getZoneId, ZEND_ACC_PUBLIC)
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */ && LIBCURL_VERSION_NUM >= 0x074100 /* Available since 7.65.0 */
	ZEND_ME(CurlUrl, setZoneId, arginfo_class_CurlUrl_setZoneId, ZEND_ACC_PUBLIC)
#endif
	ZEND_ME(CurlUrl, __toString, arginfo_class_CurlUrl___toString, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
#endif


#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
static const zend_function_entry class_CurlUrlException_methods[] = {
	ZEND_FE_END
};
#endif

#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
static zend_class_entry *register_class_CurlUrl(zend_class_entry *class_entry_Stringable)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "CurlUrl", class_CurlUrl_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;
	zend_class_implements(class_entry, 1, class_entry_Stringable);
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */

	zval const_APPEND_QUERY_value;
	ZVAL_LONG(&const_APPEND_QUERY_value, CURLU_APPENDQUERY);
	zend_string *const_APPEND_QUERY_name = zend_string_init_interned("APPEND_QUERY", sizeof("APPEND_QUERY") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_APPEND_QUERY_name, &const_APPEND_QUERY_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_APPEND_QUERY_name);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */

	zval const_DEFAULT_PORT_value;
	ZVAL_LONG(&const_DEFAULT_PORT_value, CURLU_DEFAULT_PORT);
	zend_string *const_DEFAULT_PORT_name = zend_string_init_interned("DEFAULT_PORT", sizeof("DEFAULT_PORT") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_DEFAULT_PORT_name, &const_DEFAULT_PORT_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_DEFAULT_PORT_name);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */

	zval const_DEFAULT_SCHEME_value;
	ZVAL_LONG(&const_DEFAULT_SCHEME_value, CURLU_DEFAULT_SCHEME);
	zend_string *const_DEFAULT_SCHEME_name = zend_string_init_interned("DEFAULT_SCHEME", sizeof("DEFAULT_SCHEME") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_DEFAULT_SCHEME_name, &const_DEFAULT_SCHEME_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_DEFAULT_SCHEME_name);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */

	zval const_DISALLOW_USER_value;
	ZVAL_LONG(&const_DISALLOW_USER_value, CURLU_DISALLOW_USER);
	zend_string *const_DISALLOW_USER_name = zend_string_init_interned("DISALLOW_USER", sizeof("DISALLOW_USER") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_DISALLOW_USER_name, &const_DISALLOW_USER_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_DISALLOW_USER_name);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */

	zval const_GUESS_SCHEME_value;
	ZVAL_LONG(&const_GUESS_SCHEME_value, CURLU_GUESS_SCHEME);
	zend_string *const_GUESS_SCHEME_name = zend_string_init_interned("GUESS_SCHEME", sizeof("GUESS_SCHEME") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_GUESS_SCHEME_name, &const_GUESS_SCHEME_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_GUESS_SCHEME_name);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */

	zval const_NO_DEFAULT_PORT_value;
	ZVAL_LONG(&const_NO_DEFAULT_PORT_value, CURLU_NO_DEFAULT_PORT);
	zend_string *const_NO_DEFAULT_PORT_name = zend_string_init_interned("NO_DEFAULT_PORT", sizeof("NO_DEFAULT_PORT") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_NO_DEFAULT_PORT_name, &const_NO_DEFAULT_PORT_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_NO_DEFAULT_PORT_name);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */

	zval const_ALLOW_UNSUPPORTED_SCHEME_value;
	ZVAL_LONG(&const_ALLOW_UNSUPPORTED_SCHEME_value, CURLU_NON_SUPPORT_SCHEME);
	zend_string *const_ALLOW_UNSUPPORTED_SCHEME_name = zend_string_init_interned("ALLOW_UNSUPPORTED_SCHEME", sizeof("ALLOW_UNSUPPORTED_SCHEME") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_ALLOW_UNSUPPORTED_SCHEME_name, &const_ALLOW_UNSUPPORTED_SCHEME_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_ALLOW_UNSUPPORTED_SCHEME_name);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */

	zval const_PATH_AS_IS_value;
	ZVAL_LONG(&const_PATH_AS_IS_value, CURLU_PATH_AS_IS);
	zend_string *const_PATH_AS_IS_name = zend_string_init_interned("PATH_AS_IS", sizeof("PATH_AS_IS") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_PATH_AS_IS_name, &const_PATH_AS_IS_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_PATH_AS_IS_name);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */

	zval const_URL_DECODE_value;
	ZVAL_LONG(&const_URL_DECODE_value, CURLU_URLDECODE);
	zend_string *const_URL_DECODE_name = zend_string_init_interned("URL_DECODE", sizeof("URL_DECODE") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_URL_DECODE_name, &const_URL_DECODE_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_URL_DECODE_name);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */

	zval const_URL_ENCODE_value;
	ZVAL_LONG(&const_URL_ENCODE_value, CURLU_URLENCODE);
	zend_string *const_URL_ENCODE_name = zend_string_init_interned("URL_ENCODE", sizeof("URL_ENCODE") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_URL_ENCODE_name, &const_URL_ENCODE_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_URL_ENCODE_name);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */ && LIBCURL_VERSION_NUM >= 0x074300 /* Available since 7.67.0 */

	zval const_NO_AUTHORITY_value;
	ZVAL_LONG(&const_NO_AUTHORITY_value, CURLU_NO_AUTHORITY);
	zend_string *const_NO_AUTHORITY_name = zend_string_init_interned("NO_AUTHORITY", sizeof("NO_AUTHORITY") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_NO_AUTHORITY_name, &const_NO_AUTHORITY_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_NO_AUTHORITY_name);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */ && LIBCURL_VERSION_NUM >= 0x074e00 /* Available since 7.78.0 */

	zval const_ALLOW_SPACE_value;
	ZVAL_LONG(&const_ALLOW_SPACE_value, CURLU_ALLOW_SPACE);
	zend_string *const_ALLOW_SPACE_name = zend_string_init_interned("ALLOW_SPACE", sizeof("ALLOW_SPACE") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_ALLOW_SPACE_name, &const_ALLOW_SPACE_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_ALLOW_SPACE_name);
#endif

	return class_entry;
}
#endif

#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
static zend_class_entry *register_class_CurlUrlException(zend_class_entry *class_entry_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "CurlUrlException", class_CurlUrlException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_Exception);
	class_entry->ce_flags |= ZEND_ACC_FINAL;
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */

	zval const_BAD_PORT_NUMBER_value;
	ZVAL_LONG(&const_BAD_PORT_NUMBER_value, CURLUE_BAD_PORT_NUMBER);
	zend_string *const_BAD_PORT_NUMBER_name = zend_string_init_interned("BAD_PORT_NUMBER", sizeof("BAD_PORT_NUMBER") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_BAD_PORT_NUMBER_name, &const_BAD_PORT_NUMBER_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_BAD_PORT_NUMBER_name);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */

	zval const_MALFORMED_INPUT_value;
	ZVAL_LONG(&const_MALFORMED_INPUT_value, CURLUE_MALFORMED_INPUT);
	zend_string *const_MALFORMED_INPUT_name = zend_string_init_interned("MALFORMED_INPUT", sizeof("MALFORMED_INPUT") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_MALFORMED_INPUT_name, &const_MALFORMED_INPUT_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_MALFORMED_INPUT_name);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */

	zval const_OUT_OF_MEMORY_value;
	ZVAL_LONG(&const_OUT_OF_MEMORY_value, CURLUE_OUT_OF_MEMORY);
	zend_string *const_OUT_OF_MEMORY_name = zend_string_init_interned("OUT_OF_MEMORY", sizeof("OUT_OF_MEMORY") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_OUT_OF_MEMORY_name, &const_OUT_OF_MEMORY_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_OUT_OF_MEMORY_name);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */

	zval const_UNSUPPORTED_SCHEME_value;
	ZVAL_LONG(&const_UNSUPPORTED_SCHEME_value, CURLUE_UNSUPPORTED_SCHEME);
	zend_string *const_UNSUPPORTED_SCHEME_name = zend_string_init_interned("UNSUPPORTED_SCHEME", sizeof("UNSUPPORTED_SCHEME") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_UNSUPPORTED_SCHEME_name, &const_UNSUPPORTED_SCHEME_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_UNSUPPORTED_SCHEME_name);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */

	zval const_URL_DECODING_FAILED_value;
	ZVAL_LONG(&const_URL_DECODING_FAILED_value, CURLUE_URLDECODE);
	zend_string *const_URL_DECODING_FAILED_name = zend_string_init_interned("URL_DECODING_FAILED", sizeof("URL_DECODING_FAILED") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_URL_DECODING_FAILED_name, &const_URL_DECODING_FAILED_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_URL_DECODING_FAILED_name);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */

	zval const_USER_NOT_ALLOWED_value;
	ZVAL_LONG(&const_USER_NOT_ALLOWED_value, CURLUE_USER_NOT_ALLOWED);
	zend_string *const_USER_NOT_ALLOWED_name = zend_string_init_interned("USER_NOT_ALLOWED", sizeof("USER_NOT_ALLOWED") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_USER_NOT_ALLOWED_name, &const_USER_NOT_ALLOWED_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_USER_NOT_ALLOWED_name);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */ && LIBCURL_VERSION_NUM >= 0x075100 /* Available since 7.81.0 */

	zval const_BAD_FILE_URL_value;
	ZVAL_LONG(&const_BAD_FILE_URL_value, CURLUE_BAD_FILE_URL);
	zend_string *const_BAD_FILE_URL_name = zend_string_init_interned("BAD_FILE_URL", sizeof("BAD_FILE_URL") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_BAD_FILE_URL_name, &const_BAD_FILE_URL_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_BAD_FILE_URL_name);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */ && LIBCURL_VERSION_NUM >= 0x075100 /* Available since 7.81.0 */

	zval const_BAD_FRAGMENT_value;
	ZVAL_LONG(&const_BAD_FRAGMENT_value, CURLUE_BAD_FRAGMENT);
	zend_string *const_BAD_FRAGMENT_name = zend_string_init_interned("BAD_FRAGMENT", sizeof("BAD_FRAGMENT") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_BAD_FRAGMENT_name, &const_BAD_FRAGMENT_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_BAD_FRAGMENT_name);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */ && LIBCURL_VERSION_NUM >= 0x075100 /* Available since 7.81.0 */

	zval const_BAD_HOSTNAME_value;
	ZVAL_LONG(&const_BAD_HOSTNAME_value, CURLUE_BAD_HOSTNAME);
	zend_string *const_BAD_HOSTNAME_name = zend_string_init_interned("BAD_HOSTNAME", sizeof("BAD_HOSTNAME") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_BAD_HOSTNAME_name, &const_BAD_HOSTNAME_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_BAD_HOSTNAME_name);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */ && LIBCURL_VERSION_NUM >= 0x075100 /* Available since 7.81.0 */

	zval const_BAD_IPV6_value;
	ZVAL_LONG(&const_BAD_IPV6_value, CURLUE_BAD_IPV6);
	zend_string *const_BAD_IPV6_name = zend_string_init_interned("BAD_IPV6", sizeof("BAD_IPV6") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_BAD_IPV6_name, &const_BAD_IPV6_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_BAD_IPV6_name);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */ && LIBCURL_VERSION_NUM >= 0x075100 /* Available since 7.81.0 */

	zval const_BAD_LOGIN_value;
	ZVAL_LONG(&const_BAD_LOGIN_value, CURLUE_BAD_LOGIN);
	zend_string *const_BAD_LOGIN_name = zend_string_init_interned("BAD_LOGIN", sizeof("BAD_LOGIN") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_BAD_LOGIN_name, &const_BAD_LOGIN_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_BAD_LOGIN_name);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */ && LIBCURL_VERSION_NUM >= 0x075100 /* Available since 7.81.0 */

	zval const_BAD_PASSWORD_value;
	ZVAL_LONG(&const_BAD_PASSWORD_value, CURLUE_BAD_PASSWORD);
	zend_string *const_BAD_PASSWORD_name = zend_string_init_interned("BAD_PASSWORD", sizeof("BAD_PASSWORD") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_BAD_PASSWORD_name, &const_BAD_PASSWORD_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_BAD_PASSWORD_name);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */ && LIBCURL_VERSION_NUM >= 0x075100 /* Available since 7.81.0 */

	zval const_BAD_PATH_value;
	ZVAL_LONG(&const_BAD_PATH_value, CURLUE_BAD_PATH);
	zend_string *const_BAD_PATH_name = zend_string_init_interned("BAD_PATH", sizeof("BAD_PATH") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_BAD_PATH_name, &const_BAD_PATH_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_BAD_PATH_name);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */ && LIBCURL_VERSION_NUM >= 0x075100 /* Available since 7.81.0 */

	zval const_BAD_QUERY_value;
	ZVAL_LONG(&const_BAD_QUERY_value, CURLUE_BAD_QUERY);
	zend_string *const_BAD_QUERY_name = zend_string_init_interned("BAD_QUERY", sizeof("BAD_QUERY") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_BAD_QUERY_name, &const_BAD_QUERY_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_BAD_QUERY_name);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */ && LIBCURL_VERSION_NUM >= 0x075100 /* Available since 7.81.0 */

	zval const_BAD_SCHEME_value;
	ZVAL_LONG(&const_BAD_SCHEME_value, CURLUE_BAD_SCHEME);
	zend_string *const_BAD_SCHEME_name = zend_string_init_interned("BAD_SCHEME", sizeof("BAD_SCHEME") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_BAD_SCHEME_name, &const_BAD_SCHEME_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_BAD_SCHEME_name);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */ && LIBCURL_VERSION_NUM >= 0x075100 /* Available since 7.81.0 */

	zval const_BAD_SLASHES_value;
	ZVAL_LONG(&const_BAD_SLASHES_value, CURLUE_BAD_SLASHES);
	zend_string *const_BAD_SLASHES_name = zend_string_init_interned("BAD_SLASHES", sizeof("BAD_SLASHES") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_BAD_SLASHES_name, &const_BAD_SLASHES_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_BAD_SLASHES_name);
#endif
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */ && LIBCURL_VERSION_NUM >= 0x075100 /* Available since 7.81.0 */

	zval const_BAD_USER_value;
	ZVAL_LONG(&const_BAD_USER_value, CURLUE_BAD_USER);
	zend_string *const_BAD_USER_name = zend_string_init_interned("BAD_USER", sizeof("BAD_USER") - 1, 1);
	zend_declare_class_constant_ex(class_entry, const_BAD_USER_name, &const_BAD_USER_value, ZEND_ACC_PUBLIC, NULL);
	zend_string_release(const_BAD_USER_name);
#endif

	return class_entry;
}
#endif
