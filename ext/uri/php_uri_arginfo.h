/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 20a7065d4fb21d878af78f4c21e36bdfd95b0656 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Uri_WhatWgError___construct, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, position, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, errorCode, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Uri_fromRfc3986, 0, 1, IS_STATIC, 1)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, baseUrl, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Uri_fromWhatWg, 0, 1, IS_STATIC, 1)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, baseUrl, IS_STRING, 1, "null")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, errors, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Uri_getScheme, 0, 0, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Uri_withScheme, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, scheme, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_Uri_getUser arginfo_class_Uri_Uri_getScheme

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Uri_withUser, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, user, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_Uri_getPassword arginfo_class_Uri_Uri_getScheme

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Uri_withPassword, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_Uri_getHost arginfo_class_Uri_Uri_getScheme

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Uri_withHost, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, host, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Uri_getPort, 0, 0, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Uri_withPort, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, port, IS_LONG, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_Uri_getPath arginfo_class_Uri_Uri_getScheme

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Uri_withPath, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_Uri_getQuery arginfo_class_Uri_Uri_getScheme

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Uri_withQuery, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Uri_Uri_getFragment arginfo_class_Uri_Uri_getScheme

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Uri_withFragment, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, fragment, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uri_Uri___toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Uri_Rfc3986Uri___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, baseUrl, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Uri_WhatWgUri___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, baseUrl, IS_STRING, 1, "null")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, errors, "null")
ZEND_END_ARG_INFO()

ZEND_METHOD(Uri_WhatWgError, __construct);
ZEND_METHOD(Uri_Uri, fromRfc3986);
ZEND_METHOD(Uri_Uri, fromWhatWg);
ZEND_METHOD(Uri_Uri, getScheme);
ZEND_METHOD(Uri_Uri, withScheme);
ZEND_METHOD(Uri_Uri, getUser);
ZEND_METHOD(Uri_Uri, withUser);
ZEND_METHOD(Uri_Uri, getPassword);
ZEND_METHOD(Uri_Uri, withPassword);
ZEND_METHOD(Uri_Uri, getHost);
ZEND_METHOD(Uri_Uri, withHost);
ZEND_METHOD(Uri_Uri, getPort);
ZEND_METHOD(Uri_Uri, withPort);
ZEND_METHOD(Uri_Uri, getPath);
ZEND_METHOD(Uri_Uri, withPath);
ZEND_METHOD(Uri_Uri, getQuery);
ZEND_METHOD(Uri_Uri, withQuery);
ZEND_METHOD(Uri_Uri, getFragment);
ZEND_METHOD(Uri_Uri, withFragment);
ZEND_METHOD(Uri_Uri, __toString);
ZEND_METHOD(Uri_Rfc3986Uri, __construct);
ZEND_METHOD(Uri_WhatWgUri, __construct);

static const zend_function_entry class_Uri_WhatWgError_methods[] = {
	ZEND_ME(Uri_WhatWgError, __construct, arginfo_class_Uri_WhatWgError___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Uri_Uri_methods[] = {
	ZEND_ME(Uri_Uri, fromRfc3986, arginfo_class_Uri_Uri_fromRfc3986, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Uri_Uri, fromWhatWg, arginfo_class_Uri_Uri_fromWhatWg, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Uri_Uri, getScheme, arginfo_class_Uri_Uri_getScheme, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Uri, withScheme, arginfo_class_Uri_Uri_withScheme, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Uri, getUser, arginfo_class_Uri_Uri_getUser, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Uri, withUser, arginfo_class_Uri_Uri_withUser, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Uri, getPassword, arginfo_class_Uri_Uri_getPassword, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Uri, withPassword, arginfo_class_Uri_Uri_withPassword, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Uri, getHost, arginfo_class_Uri_Uri_getHost, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Uri, withHost, arginfo_class_Uri_Uri_withHost, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Uri, getPort, arginfo_class_Uri_Uri_getPort, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Uri, withPort, arginfo_class_Uri_Uri_withPort, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Uri, getPath, arginfo_class_Uri_Uri_getPath, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Uri, withPath, arginfo_class_Uri_Uri_withPath, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Uri, getQuery, arginfo_class_Uri_Uri_getQuery, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Uri, withQuery, arginfo_class_Uri_Uri_withQuery, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Uri, getFragment, arginfo_class_Uri_Uri_getFragment, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Uri, withFragment, arginfo_class_Uri_Uri_withFragment, ZEND_ACC_PUBLIC)
	ZEND_ME(Uri_Uri, __toString, arginfo_class_Uri_Uri___toString, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Uri_Rfc3986Uri_methods[] = {
	ZEND_ME(Uri_Rfc3986Uri, __construct, arginfo_class_Uri_Rfc3986Uri___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_Uri_WhatWgUri_methods[] = {
	ZEND_ME(Uri_WhatWgUri, __construct, arginfo_class_Uri_WhatWgUri___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Uri_WhatWgError(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Uri", "WhatWgError", class_Uri_WhatWgError_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_READONLY_CLASS);

	zval const_ERROR_TYPE_DOMAIN_TO_ASCII_value;
	ZVAL_LONG(&const_ERROR_TYPE_DOMAIN_TO_ASCII_value, LXB_URL_ERROR_TYPE_DOMAIN_TO_ASCII);
	zend_string *const_ERROR_TYPE_DOMAIN_TO_ASCII_name = zend_string_init_interned("ERROR_TYPE_DOMAIN_TO_ASCII", sizeof("ERROR_TYPE_DOMAIN_TO_ASCII") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_DOMAIN_TO_ASCII_name, &const_ERROR_TYPE_DOMAIN_TO_ASCII_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_DOMAIN_TO_ASCII_name);

	zval const_ERROR_TYPE_DOMAIN_TO_UNICODE_value;
	ZVAL_LONG(&const_ERROR_TYPE_DOMAIN_TO_UNICODE_value, LXB_URL_ERROR_TYPE_DOMAIN_TO_UNICODE);
	zend_string *const_ERROR_TYPE_DOMAIN_TO_UNICODE_name = zend_string_init_interned("ERROR_TYPE_DOMAIN_TO_UNICODE", sizeof("ERROR_TYPE_DOMAIN_TO_UNICODE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_DOMAIN_TO_UNICODE_name, &const_ERROR_TYPE_DOMAIN_TO_UNICODE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_DOMAIN_TO_UNICODE_name);

	zval const_ERROR_TYPE_DOMAIN_INVALID_CODE_POINT_value;
	ZVAL_LONG(&const_ERROR_TYPE_DOMAIN_INVALID_CODE_POINT_value, LXB_URL_ERROR_TYPE_DOMAIN_INVALID_CODE_POINT);
	zend_string *const_ERROR_TYPE_DOMAIN_INVALID_CODE_POINT_name = zend_string_init_interned("ERROR_TYPE_DOMAIN_INVALID_CODE_POINT", sizeof("ERROR_TYPE_DOMAIN_INVALID_CODE_POINT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_DOMAIN_INVALID_CODE_POINT_name, &const_ERROR_TYPE_DOMAIN_INVALID_CODE_POINT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_DOMAIN_INVALID_CODE_POINT_name);

	zval const_ERROR_TYPE_HOST_INVALID_CODE_POINT_value;
	ZVAL_LONG(&const_ERROR_TYPE_HOST_INVALID_CODE_POINT_value, LXB_URL_ERROR_TYPE_HOST_INVALID_CODE_POINT);
	zend_string *const_ERROR_TYPE_HOST_INVALID_CODE_POINT_name = zend_string_init_interned("ERROR_TYPE_HOST_INVALID_CODE_POINT", sizeof("ERROR_TYPE_HOST_INVALID_CODE_POINT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_HOST_INVALID_CODE_POINT_name, &const_ERROR_TYPE_HOST_INVALID_CODE_POINT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_HOST_INVALID_CODE_POINT_name);

	zval const_ERROR_TYPE_IPV4_EMPTY_PART_value;
	ZVAL_LONG(&const_ERROR_TYPE_IPV4_EMPTY_PART_value, LXB_URL_ERROR_TYPE_IPV4_EMPTY_PART);
	zend_string *const_ERROR_TYPE_IPV4_EMPTY_PART_name = zend_string_init_interned("ERROR_TYPE_IPV4_EMPTY_PART", sizeof("ERROR_TYPE_IPV4_EMPTY_PART") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_IPV4_EMPTY_PART_name, &const_ERROR_TYPE_IPV4_EMPTY_PART_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_IPV4_EMPTY_PART_name);

	zval const_ERROR_TYPE_IPV4_TOO_MANY_PARTS_value;
	ZVAL_LONG(&const_ERROR_TYPE_IPV4_TOO_MANY_PARTS_value, LXB_URL_ERROR_TYPE_IPV4_TOO_MANY_PARTS);
	zend_string *const_ERROR_TYPE_IPV4_TOO_MANY_PARTS_name = zend_string_init_interned("ERROR_TYPE_IPV4_TOO_MANY_PARTS", sizeof("ERROR_TYPE_IPV4_TOO_MANY_PARTS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_IPV4_TOO_MANY_PARTS_name, &const_ERROR_TYPE_IPV4_TOO_MANY_PARTS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_IPV4_TOO_MANY_PARTS_name);

	zval const_ERROR_TYPE_IPV4_NON_NUMERIC_PART_value;
	ZVAL_LONG(&const_ERROR_TYPE_IPV4_NON_NUMERIC_PART_value, LXB_URL_ERROR_TYPE_IPV4_NON_NUMERIC_PART);
	zend_string *const_ERROR_TYPE_IPV4_NON_NUMERIC_PART_name = zend_string_init_interned("ERROR_TYPE_IPV4_NON_NUMERIC_PART", sizeof("ERROR_TYPE_IPV4_NON_NUMERIC_PART") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_IPV4_NON_NUMERIC_PART_name, &const_ERROR_TYPE_IPV4_NON_NUMERIC_PART_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_IPV4_NON_NUMERIC_PART_name);

	zval const_ERROR_TYPE_IPV4_NON_DECIMAL_PART_value;
	ZVAL_LONG(&const_ERROR_TYPE_IPV4_NON_DECIMAL_PART_value, LXB_URL_ERROR_TYPE_IPV4_NON_DECIMAL_PART);
	zend_string *const_ERROR_TYPE_IPV4_NON_DECIMAL_PART_name = zend_string_init_interned("ERROR_TYPE_IPV4_NON_DECIMAL_PART", sizeof("ERROR_TYPE_IPV4_NON_DECIMAL_PART") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_IPV4_NON_DECIMAL_PART_name, &const_ERROR_TYPE_IPV4_NON_DECIMAL_PART_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_IPV4_NON_DECIMAL_PART_name);

	zval const_ERROR_TYPE_IPV4_OUT_OF_RANGE_PART_value;
	ZVAL_LONG(&const_ERROR_TYPE_IPV4_OUT_OF_RANGE_PART_value, LXB_URL_ERROR_TYPE_IPV4_OUT_OF_RANGE_PART);
	zend_string *const_ERROR_TYPE_IPV4_OUT_OF_RANGE_PART_name = zend_string_init_interned("ERROR_TYPE_IPV4_OUT_OF_RANGE_PART", sizeof("ERROR_TYPE_IPV4_OUT_OF_RANGE_PART") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_IPV4_OUT_OF_RANGE_PART_name, &const_ERROR_TYPE_IPV4_OUT_OF_RANGE_PART_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_IPV4_OUT_OF_RANGE_PART_name);

	zval const_ERROR_TYPE_IPV6_UNCLOSED_value;
	ZVAL_LONG(&const_ERROR_TYPE_IPV6_UNCLOSED_value, LXB_URL_ERROR_TYPE_IPV6_UNCLOSED);
	zend_string *const_ERROR_TYPE_IPV6_UNCLOSED_name = zend_string_init_interned("ERROR_TYPE_IPV6_UNCLOSED", sizeof("ERROR_TYPE_IPV6_UNCLOSED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_IPV6_UNCLOSED_name, &const_ERROR_TYPE_IPV6_UNCLOSED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_IPV6_UNCLOSED_name);

	zval const_ERROR_TYPE_IPV6_INVALID_COMPRESSION_value;
	ZVAL_LONG(&const_ERROR_TYPE_IPV6_INVALID_COMPRESSION_value, LXB_URL_ERROR_TYPE_IPV6_INVALID_COMPRESSION);
	zend_string *const_ERROR_TYPE_IPV6_INVALID_COMPRESSION_name = zend_string_init_interned("ERROR_TYPE_IPV6_INVALID_COMPRESSION", sizeof("ERROR_TYPE_IPV6_INVALID_COMPRESSION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_IPV6_INVALID_COMPRESSION_name, &const_ERROR_TYPE_IPV6_INVALID_COMPRESSION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_IPV6_INVALID_COMPRESSION_name);

	zval const_ERROR_TYPE_IPV6_TOO_MANY_PIECES_value;
	ZVAL_LONG(&const_ERROR_TYPE_IPV6_TOO_MANY_PIECES_value, LXB_URL_ERROR_TYPE_IPV6_TOO_MANY_PIECES);
	zend_string *const_ERROR_TYPE_IPV6_TOO_MANY_PIECES_name = zend_string_init_interned("ERROR_TYPE_IPV6_TOO_MANY_PIECES", sizeof("ERROR_TYPE_IPV6_TOO_MANY_PIECES") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_IPV6_TOO_MANY_PIECES_name, &const_ERROR_TYPE_IPV6_TOO_MANY_PIECES_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_IPV6_TOO_MANY_PIECES_name);

	zval const_ERROR_TYPE_IPV6_MULTIPLE_COMPRESSION_value;
	ZVAL_LONG(&const_ERROR_TYPE_IPV6_MULTIPLE_COMPRESSION_value, LXB_URL_ERROR_TYPE_IPV6_MULTIPLE_COMPRESSION);
	zend_string *const_ERROR_TYPE_IPV6_MULTIPLE_COMPRESSION_name = zend_string_init_interned("ERROR_TYPE_IPV6_MULTIPLE_COMPRESSION", sizeof("ERROR_TYPE_IPV6_MULTIPLE_COMPRESSION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_IPV6_MULTIPLE_COMPRESSION_name, &const_ERROR_TYPE_IPV6_MULTIPLE_COMPRESSION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_IPV6_MULTIPLE_COMPRESSION_name);

	zval const_ERROR_TYPE_IPV6_INVALID_CODE_POINT_value;
	ZVAL_LONG(&const_ERROR_TYPE_IPV6_INVALID_CODE_POINT_value, LXB_URL_ERROR_TYPE_IPV6_INVALID_CODE_POINT);
	zend_string *const_ERROR_TYPE_IPV6_INVALID_CODE_POINT_name = zend_string_init_interned("ERROR_TYPE_IPV6_INVALID_CODE_POINT", sizeof("ERROR_TYPE_IPV6_INVALID_CODE_POINT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_IPV6_INVALID_CODE_POINT_name, &const_ERROR_TYPE_IPV6_INVALID_CODE_POINT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_IPV6_INVALID_CODE_POINT_name);

	zval const_ERROR_TYPE_IPV6_TOO_FEW_PIECES_value;
	ZVAL_LONG(&const_ERROR_TYPE_IPV6_TOO_FEW_PIECES_value, LXB_URL_ERROR_TYPE_IPV6_TOO_FEW_PIECES);
	zend_string *const_ERROR_TYPE_IPV6_TOO_FEW_PIECES_name = zend_string_init_interned("ERROR_TYPE_IPV6_TOO_FEW_PIECES", sizeof("ERROR_TYPE_IPV6_TOO_FEW_PIECES") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_IPV6_TOO_FEW_PIECES_name, &const_ERROR_TYPE_IPV6_TOO_FEW_PIECES_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_IPV6_TOO_FEW_PIECES_name);

	zval const_ERROR_TYPE_IPV4_IN_IPV6_TOO_MANY_PIECES_value;
	ZVAL_LONG(&const_ERROR_TYPE_IPV4_IN_IPV6_TOO_MANY_PIECES_value, LXB_URL_ERROR_TYPE_IPV4_IN_IPV6_TOO_MANY_PIECES);
	zend_string *const_ERROR_TYPE_IPV4_IN_IPV6_TOO_MANY_PIECES_name = zend_string_init_interned("ERROR_TYPE_IPV4_IN_IPV6_TOO_MANY_PIECES", sizeof("ERROR_TYPE_IPV4_IN_IPV6_TOO_MANY_PIECES") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_IPV4_IN_IPV6_TOO_MANY_PIECES_name, &const_ERROR_TYPE_IPV4_IN_IPV6_TOO_MANY_PIECES_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_IPV4_IN_IPV6_TOO_MANY_PIECES_name);

	zval const_ERROR_TYPE_IPV4_IN_IPV6_INVALID_CODE_POINT_value;
	ZVAL_LONG(&const_ERROR_TYPE_IPV4_IN_IPV6_INVALID_CODE_POINT_value, LXB_URL_ERROR_TYPE_IPV4_IN_IPV6_INVALID_CODE_POINT);
	zend_string *const_ERROR_TYPE_IPV4_IN_IPV6_INVALID_CODE_POINT_name = zend_string_init_interned("ERROR_TYPE_IPV4_IN_IPV6_INVALID_CODE_POINT", sizeof("ERROR_TYPE_IPV4_IN_IPV6_INVALID_CODE_POINT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_IPV4_IN_IPV6_INVALID_CODE_POINT_name, &const_ERROR_TYPE_IPV4_IN_IPV6_INVALID_CODE_POINT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_IPV4_IN_IPV6_INVALID_CODE_POINT_name);

	zval const_ERROR_TYPE_IPV4_IN_IPV6_OUT_OF_RANGE_PART_value;
	ZVAL_LONG(&const_ERROR_TYPE_IPV4_IN_IPV6_OUT_OF_RANGE_PART_value, LXB_URL_ERROR_TYPE_IPV4_IN_IPV6_OUT_OF_RANGE_PART);
	zend_string *const_ERROR_TYPE_IPV4_IN_IPV6_OUT_OF_RANGE_PART_name = zend_string_init_interned("ERROR_TYPE_IPV4_IN_IPV6_OUT_OF_RANGE_PART", sizeof("ERROR_TYPE_IPV4_IN_IPV6_OUT_OF_RANGE_PART") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_IPV4_IN_IPV6_OUT_OF_RANGE_PART_name, &const_ERROR_TYPE_IPV4_IN_IPV6_OUT_OF_RANGE_PART_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_IPV4_IN_IPV6_OUT_OF_RANGE_PART_name);

	zval const_ERROR_TYPE_IPV4_IN_IPV6_TOO_FEW_PARTS_value;
	ZVAL_LONG(&const_ERROR_TYPE_IPV4_IN_IPV6_TOO_FEW_PARTS_value, LXB_URL_ERROR_TYPE_IPV4_IN_IPV6_TOO_FEW_PARTS);
	zend_string *const_ERROR_TYPE_IPV4_IN_IPV6_TOO_FEW_PARTS_name = zend_string_init_interned("ERROR_TYPE_IPV4_IN_IPV6_TOO_FEW_PARTS", sizeof("ERROR_TYPE_IPV4_IN_IPV6_TOO_FEW_PARTS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_IPV4_IN_IPV6_TOO_FEW_PARTS_name, &const_ERROR_TYPE_IPV4_IN_IPV6_TOO_FEW_PARTS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_IPV4_IN_IPV6_TOO_FEW_PARTS_name);

	zval const_ERROR_TYPE_INVALID_URL_UNIT_value;
	ZVAL_LONG(&const_ERROR_TYPE_INVALID_URL_UNIT_value, LXB_URL_ERROR_TYPE_INVALID_URL_UNIT);
	zend_string *const_ERROR_TYPE_INVALID_URL_UNIT_name = zend_string_init_interned("ERROR_TYPE_INVALID_URL_UNIT", sizeof("ERROR_TYPE_INVALID_URL_UNIT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_INVALID_URL_UNIT_name, &const_ERROR_TYPE_INVALID_URL_UNIT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_INVALID_URL_UNIT_name);

	zval const_ERROR_TYPE_SPECIAL_SCHEME_MISSING_FOLLOWING_SOLIDUS_value;
	ZVAL_LONG(&const_ERROR_TYPE_SPECIAL_SCHEME_MISSING_FOLLOWING_SOLIDUS_value, LXB_URL_ERROR_TYPE_SPECIAL_SCHEME_MISSING_FOLLOWING_SOLIDUS);
	zend_string *const_ERROR_TYPE_SPECIAL_SCHEME_MISSING_FOLLOWING_SOLIDUS_name = zend_string_init_interned("ERROR_TYPE_SPECIAL_SCHEME_MISSING_FOLLOWING_SOLIDUS", sizeof("ERROR_TYPE_SPECIAL_SCHEME_MISSING_FOLLOWING_SOLIDUS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_SPECIAL_SCHEME_MISSING_FOLLOWING_SOLIDUS_name, &const_ERROR_TYPE_SPECIAL_SCHEME_MISSING_FOLLOWING_SOLIDUS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_SPECIAL_SCHEME_MISSING_FOLLOWING_SOLIDUS_name);

	zval const_ERROR_TYPE_MISSING_SCHEME_NON_RELATIVE_URL_value;
	ZVAL_LONG(&const_ERROR_TYPE_MISSING_SCHEME_NON_RELATIVE_URL_value, LXB_URL_ERROR_TYPE_MISSING_SCHEME_NON_RELATIVE_URL);
	zend_string *const_ERROR_TYPE_MISSING_SCHEME_NON_RELATIVE_URL_name = zend_string_init_interned("ERROR_TYPE_MISSING_SCHEME_NON_RELATIVE_URL", sizeof("ERROR_TYPE_MISSING_SCHEME_NON_RELATIVE_URL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_MISSING_SCHEME_NON_RELATIVE_URL_name, &const_ERROR_TYPE_MISSING_SCHEME_NON_RELATIVE_URL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_MISSING_SCHEME_NON_RELATIVE_URL_name);

	zval const_ERROR_TYPE_INVALID_REVERSE_SOLIDUS_value;
	ZVAL_LONG(&const_ERROR_TYPE_INVALID_REVERSE_SOLIDUS_value, LXB_URL_ERROR_TYPE_INVALID_REVERSE_SOLIDUS);
	zend_string *const_ERROR_TYPE_INVALID_REVERSE_SOLIDUS_name = zend_string_init_interned("ERROR_TYPE_INVALID_REVERSE_SOLIDUS", sizeof("ERROR_TYPE_INVALID_REVERSE_SOLIDUS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_INVALID_REVERSE_SOLIDUS_name, &const_ERROR_TYPE_INVALID_REVERSE_SOLIDUS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_INVALID_REVERSE_SOLIDUS_name);

	zval const_ERROR_TYPE_INVALID_CREDENTIALS_value;
	ZVAL_LONG(&const_ERROR_TYPE_INVALID_CREDENTIALS_value, LXB_URL_ERROR_TYPE_INVALID_CREDENTIALS);
	zend_string *const_ERROR_TYPE_INVALID_CREDENTIALS_name = zend_string_init_interned("ERROR_TYPE_INVALID_CREDENTIALS", sizeof("ERROR_TYPE_INVALID_CREDENTIALS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_INVALID_CREDENTIALS_name, &const_ERROR_TYPE_INVALID_CREDENTIALS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_INVALID_CREDENTIALS_name);

	zval const_ERROR_TYPE_HOST_MISSING_value;
	ZVAL_LONG(&const_ERROR_TYPE_HOST_MISSING_value, LXB_URL_ERROR_TYPE_HOST_MISSING);
	zend_string *const_ERROR_TYPE_HOST_MISSING_name = zend_string_init_interned("ERROR_TYPE_HOST_MISSING", sizeof("ERROR_TYPE_HOST_MISSING") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_HOST_MISSING_name, &const_ERROR_TYPE_HOST_MISSING_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_HOST_MISSING_name);

	zval const_ERROR_TYPE_PORT_OUT_OF_RANGE_value;
	ZVAL_LONG(&const_ERROR_TYPE_PORT_OUT_OF_RANGE_value, LXB_URL_ERROR_TYPE_PORT_OUT_OF_RANGE);
	zend_string *const_ERROR_TYPE_PORT_OUT_OF_RANGE_name = zend_string_init_interned("ERROR_TYPE_PORT_OUT_OF_RANGE", sizeof("ERROR_TYPE_PORT_OUT_OF_RANGE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_PORT_OUT_OF_RANGE_name, &const_ERROR_TYPE_PORT_OUT_OF_RANGE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_PORT_OUT_OF_RANGE_name);

	zval const_ERROR_TYPE_PORT_INVALID_value;
	ZVAL_LONG(&const_ERROR_TYPE_PORT_INVALID_value, LXB_URL_ERROR_TYPE_PORT_INVALID);
	zend_string *const_ERROR_TYPE_PORT_INVALID_name = zend_string_init_interned("ERROR_TYPE_PORT_INVALID", sizeof("ERROR_TYPE_PORT_INVALID") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_PORT_INVALID_name, &const_ERROR_TYPE_PORT_INVALID_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_PORT_INVALID_name);

	zval const_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_value;
	ZVAL_LONG(&const_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_value, LXB_URL_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER);
	zend_string *const_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_name = zend_string_init_interned("ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER", sizeof("ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_name, &const_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_name);

	zval const_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_HOST_value;
	ZVAL_LONG(&const_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_HOST_value, LXB_URL_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_HOST);
	zend_string *const_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_HOST_name = zend_string_init_interned("ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_HOST", sizeof("ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_HOST") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_HOST_name, &const_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_HOST_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_HOST_name);

	zval property_position_default_value;
	ZVAL_UNDEF(&property_position_default_value);
	zend_string *property_position_name = zend_string_init("position", sizeof("position") - 1, 1);
	zend_declare_typed_property(class_entry, property_position_name, &property_position_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_position_name);

	zval property_errorCode_default_value;
	ZVAL_UNDEF(&property_errorCode_default_value);
	zend_string *property_errorCode_name = zend_string_init("errorCode", sizeof("errorCode") - 1, 1);
	zend_declare_typed_property(class_entry, property_errorCode_name, &property_errorCode_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_errorCode_name);

	return class_entry;
}

static zend_class_entry *register_class_Uri_Uri(zend_class_entry *class_entry_Uri_Stringable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Uri", "Uri", class_Uri_Uri_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_ABSTRACT|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_READONLY_CLASS);
	zend_class_implements(class_entry, 1, class_entry_Uri_Stringable);

	zval property_scheme_default_value;
	ZVAL_UNDEF(&property_scheme_default_value);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_SCHEME), &property_scheme_default_value, ZEND_ACC_PRIVATE|ZEND_ACC_READONLY|ZEND_ACC_VIRTUAL, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));

	zval property_user_default_value;
	ZVAL_UNDEF(&property_user_default_value);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_USER), &property_user_default_value, ZEND_ACC_PRIVATE|ZEND_ACC_READONLY|ZEND_ACC_VIRTUAL, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));

	zval property_password_default_value;
	ZVAL_UNDEF(&property_password_default_value);
	zend_string *property_password_name = zend_string_init("password", sizeof("password") - 1, 1);
	zend_declare_typed_property(class_entry, property_password_name, &property_password_default_value, ZEND_ACC_PRIVATE|ZEND_ACC_READONLY|ZEND_ACC_VIRTUAL, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_password_name);

	zval property_host_default_value;
	ZVAL_UNDEF(&property_host_default_value);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_HOST), &property_host_default_value, ZEND_ACC_PRIVATE|ZEND_ACC_READONLY|ZEND_ACC_VIRTUAL, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));

	zval property_port_default_value;
	ZVAL_UNDEF(&property_port_default_value);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_PORT), &property_port_default_value, ZEND_ACC_PRIVATE|ZEND_ACC_READONLY|ZEND_ACC_VIRTUAL, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG|MAY_BE_NULL));

	zval property_path_default_value;
	ZVAL_UNDEF(&property_path_default_value);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_PATH), &property_path_default_value, ZEND_ACC_PRIVATE|ZEND_ACC_READONLY|ZEND_ACC_VIRTUAL, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));

	zval property_query_default_value;
	ZVAL_UNDEF(&property_query_default_value);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_QUERY), &property_query_default_value, ZEND_ACC_PRIVATE|ZEND_ACC_READONLY|ZEND_ACC_VIRTUAL, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));

	zval property_fragment_default_value;
	ZVAL_UNDEF(&property_fragment_default_value);
	zend_declare_typed_property(class_entry, ZSTR_KNOWN(ZEND_STR_FRAGMENT), &property_fragment_default_value, ZEND_ACC_PRIVATE|ZEND_ACC_READONLY|ZEND_ACC_VIRTUAL, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));

	return class_entry;
}

static zend_class_entry *register_class_Uri_Rfc3986Uri(zend_class_entry *class_entry_Uri_Uri)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Uri", "Rfc3986Uri", class_Uri_Rfc3986Uri_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Uri_Uri, ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_READONLY_CLASS);

	return class_entry;
}

static zend_class_entry *register_class_Uri_WhatWgUri(zend_class_entry *class_entry_Uri_Uri)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Uri", "WhatWgUri", class_Uri_WhatWgUri_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Uri_Uri, ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_READONLY_CLASS);

	return class_entry;
}
