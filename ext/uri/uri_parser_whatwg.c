/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Máté Kocsis <kocsismate@php.net>                            |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "uri_parser_whatwg.h"
#include "php_uri_common.h"
#include "Zend/zend_enum.h"
#include "Zend/zend_smart_str.h"
#include "Zend/zend_exceptions.h"
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

ZEND_TLS lexbor_mraw_t lexbor_mraw = {0};
ZEND_TLS lxb_url_parser_t lexbor_parser = {0};
ZEND_TLS lxb_unicode_idna_t lexbor_idna = {0};

static const size_t lexbor_mraw_byte_size = 8192;

ZEND_ATTRIBUTE_NONNULL static zend_always_inline void zval_string_or_null_to_lexbor_str(const zval *value, lexbor_str_t *lexbor_str)
{
	if (Z_TYPE_P(value) == IS_STRING && Z_STRLEN_P(value) > 0) {
		lexbor_str->data = (lxb_char_t *) Z_STRVAL_P(value);
		lexbor_str->length = Z_STRLEN_P(value);
	} else {
		ZEND_ASSERT(Z_ISNULL_P(value) || (Z_TYPE_P(value) == IS_STRING && Z_STRLEN_P(value) == 0));
		lexbor_str->data = (lxb_char_t *) "";
		lexbor_str->length = 0;
	}
}

ZEND_ATTRIBUTE_NONNULL static zend_always_inline void zval_long_or_null_to_lexbor_str(const zval *value, lexbor_str_t *lexbor_str)
{
	if (Z_TYPE_P(value) == IS_LONG) {
		char buf[MAX_LENGTH_OF_LONG + 1];
		const char *res = zend_print_long_to_buf(buf + sizeof(buf) - 1, Z_LVAL_P(value));
		lexbor_str_init_append(lexbor_str, lexbor_parser.mraw, (const lxb_char_t *) res, strlen(res));
	} else {
		ZEND_ASSERT(Z_ISNULL_P(value));
		lexbor_str->data = (lxb_char_t *) "";
		lexbor_str->length = 0;
	}
}

/**
 * Creates a Uri\WhatWg\UrlValidationError class by mapping error codes listed in
 * https://url.spec.whatwg.org/#writing to a Uri\WhatWg\UrlValidationErrorType enum.
 * The result is passed by reference to the errors parameter.
 */
ZEND_ATTRIBUTE_NONNULL static const char *fill_errors_inner(zval *errors)
{
	const char *result = NULL;

	lexbor_plog_entry_t *lxb_error;
	while ((lxb_error = lexbor_array_obj_pop(&lexbor_parser.log->list)) != NULL) {
		zval error;
		object_init_ex(&error, php_uri_ce_whatwg_url_validation_error);
		zend_update_property_string(php_uri_ce_whatwg_url_validation_error, Z_OBJ(error), ZEND_STRL("context"), (const char *) lxb_error->data);

		const char *error_str;
		zval failure;
		switch (lxb_error->id) {
			case LXB_URL_ERROR_TYPE_DOMAIN_TO_ASCII:
				error_str = "DomainToAscii";
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_DOMAIN_TO_UNICODE:
				error_str = "DomainToUnicode";
				ZVAL_FALSE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_DOMAIN_INVALID_CODE_POINT:
				error_str = "DomainInvalidCodePoint";
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_HOST_INVALID_CODE_POINT:
				error_str = "HostInvalidCodePoint";
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_IPV4_EMPTY_PART:
				error_str = "Ipv4EmptyPart";
				ZVAL_FALSE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_IPV4_TOO_MANY_PARTS:
				error_str = "Ipv4TooManyParts";
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_IPV4_NON_NUMERIC_PART:
				error_str = "Ipv4NonNumericPart";
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_IPV4_NON_DECIMAL_PART:
				error_str = "Ipv4NonDecimalPart";
				ZVAL_FALSE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_IPV4_OUT_OF_RANGE_PART:
				error_str = "Ipv4OutOfRangePart";
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_IPV6_UNCLOSED:
				error_str = "Ipv6Unclosed";
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_IPV6_INVALID_COMPRESSION:
				error_str = "Ipv6InvalidCompression";
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_IPV6_TOO_MANY_PIECES:
				error_str = "Ipv6TooManyPieces";
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_IPV6_MULTIPLE_COMPRESSION:
				error_str = "Ipv6MultipleCompression";
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_IPV6_INVALID_CODE_POINT:
				error_str = "Ipv6InvalidCodePoint";
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_IPV6_TOO_FEW_PIECES:
				error_str = "Ipv6TooFewPieces";
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_IPV4_IN_IPV6_TOO_MANY_PIECES:
				error_str = "Ipv4InIpv6TooManyPieces";
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_IPV4_IN_IPV6_INVALID_CODE_POINT:
				error_str = "Ipv4InIpv6InvalidCodePoint";
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_IPV4_IN_IPV6_OUT_OF_RANGE_PART:
				error_str = "Ipv4InIpv6OutOfRangePart";
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_IPV4_IN_IPV6_TOO_FEW_PARTS:
				error_str = "Ipv4InIpv6TooFewParts";
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_INVALID_URL_UNIT:
				error_str = "InvalidUrlUnit";
				ZVAL_FALSE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_SPECIAL_SCHEME_MISSING_FOLLOWING_SOLIDUS:
				error_str = "SpecialSchemeMissingFollowingSolidus";
				ZVAL_FALSE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_MISSING_SCHEME_NON_RELATIVE_URL:
				error_str = "MissingSchemeNonRelativeUrl";
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_INVALID_REVERSE_SOLIDUS:
				error_str = "InvalidReverseSoldius";
				ZVAL_FALSE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_INVALID_CREDENTIALS:
				error_str = "InvalidCredentials";
				ZVAL_FALSE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_HOST_MISSING:
				error_str = "HostMissing";
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_PORT_OUT_OF_RANGE:
				error_str = "PortOutOfRange";
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_PORT_INVALID:
				error_str = "PortInvalid";
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER:
				error_str = "FileInvalidWindowsDriveLetter";
				ZVAL_FALSE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_HOST:
				error_str = "FileInvalidWindowsDriveLetterHost";
				ZVAL_FALSE(&failure);
				break;
			default: ZEND_UNREACHABLE();
		}

		zval error_type;
		ZVAL_OBJ(&error_type, zend_enum_get_case_cstr(php_uri_ce_whatwg_url_validation_error_type, error_str));
		zend_update_property_ex(php_uri_ce_whatwg_url_validation_error, Z_OBJ(error), ZSTR_KNOWN(ZEND_STR_TYPE), &error_type);

		zend_update_property(php_uri_ce_whatwg_url_validation_error, Z_OBJ(error), ZEND_STRL("failure"), &failure);

		if (Z_TYPE(failure) == IS_TRUE) {
			result = error_str;
		}

		add_next_index_zval(errors, &error);
	}

	return result;
}

/**
 * Creates a Uri\WhatWg\UrlValidationError class by mapping error codes listed in
 * https://url.spec.whatwg.org/#writing to a Uri\WhatWg\UrlValidationErrorType enum.
 * The result is passed by reference to the errors parameter.
 */
ZEND_ATTRIBUTE_NONNULL static const char *fill_errors(zval *errors)
{
	size_t log_len;
	if (lexbor_parser.log == NULL || (log_len = lexbor_plog_length(lexbor_parser.log)) == 0) {
		ZVAL_EMPTY_ARRAY(errors);
		return NULL;
	}

	array_init_size(errors, log_len);

	return fill_errors_inner(errors);
}

static void throw_invalid_url_exception_during_write(zval *errors, const char *component)
{
	zval err;
	const char *reason = fill_errors(&err);
	zend_object *exception = zend_throw_exception_ex(
		php_uri_ce_whatwg_invalid_url_exception,
		0,
		"The specified %s is malformed%s%s%s",
		component,
		reason ? " (" : "",
		reason ? reason : "",
		reason ? ")" : ""
	);
	zend_update_property(exception->ce, exception, ZEND_STRL("errors"), &err);
	if (errors) {
		zval_ptr_dtor(errors);
		ZVAL_COPY_VALUE(errors, &err);
	} else {
		zval_ptr_dtor(&err);
	}
}

static lxb_status_t serialize_to_smart_str_callback(const lxb_char_t *data, const size_t length, void *ctx)
{
	smart_str *uri_str = ctx;

	if (data != NULL && length > 0) {
		smart_str_appendl(uri_str, (const char *) data, length);
	}

	return LXB_STATUS_OK;
}

static zend_result php_uri_parser_whatwg_scheme_read(void *uri, const php_uri_component_read_mode read_mode, zval *retval)
{
	const lxb_url_t *lexbor_uri = uri;

	ZEND_ASSERT(lexbor_uri->scheme.type != LXB_URL_SCHEMEL_TYPE__UNDEF);

	ZVAL_STRINGL(retval, (const char *) lexbor_uri->scheme.name.data, lexbor_uri->scheme.name.length);

	return SUCCESS;
}

static zend_result php_uri_parser_whatwg_scheme_write(void *uri, const zval *value, zval *errors)
{
	lxb_url_t *lexbor_uri = uri;
	lexbor_str_t str = {0};

	zval_string_or_null_to_lexbor_str(value, &str);

	lxb_url_parser_clean(&lexbor_parser);

	if (lxb_url_api_protocol_set(lexbor_uri, &lexbor_parser, str.data, str.length) != LXB_STATUS_OK) {
		throw_invalid_url_exception_during_write(errors, "scheme");

		return FAILURE;
	}

	return SUCCESS;
}

ZEND_ATTRIBUTE_NONNULL bool php_uri_parser_whatwg_is_special(const lxb_url_t *lexbor_uri)
{
	return lxb_url_is_special(lexbor_uri);
}

/* 4.2. URL miscellaneous: A URL includes credentials if its username or password is not the empty string. */
static bool includes_credentials(const lxb_url_t *lexbor_uri)
{
	return lexbor_uri->username.length > 0 || lexbor_uri->password.length > 0;
}

static zend_result php_uri_parser_whatwg_username_read(void *uri, php_uri_component_read_mode read_mode, zval *retval)
{
	const lxb_url_t *lexbor_uri = uri;

	if (includes_credentials(lexbor_uri)) {
		ZVAL_STRINGL_FAST(retval, (const char *) lexbor_uri->username.data, lexbor_uri->username.length);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result php_uri_parser_whatwg_username_write(void *uri, const zval *value, zval *errors)
{
	lxb_url_t *lexbor_uri = uri;
	lexbor_str_t str = {0};

	zval_string_or_null_to_lexbor_str(value, &str);

	lxb_url_parser_clean(&lexbor_parser);

	if (lxb_url_api_username_set(lexbor_uri, str.data, str.length) != LXB_STATUS_OK) {
		throw_invalid_url_exception_during_write(errors, "username");

		return FAILURE;
	}

	return SUCCESS;
}

static zend_result php_uri_parser_whatwg_password_read(void *uri, php_uri_component_read_mode read_mode, zval *retval)
{
	const lxb_url_t *lexbor_uri = uri;

	if (includes_credentials(lexbor_uri)) {
		ZVAL_STRINGL_FAST(retval, (const char *) lexbor_uri->password.data, lexbor_uri->password.length);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result php_uri_parser_whatwg_password_write(void *uri, const zval *value, zval *errors)
{
	lxb_url_t *lexbor_uri = uri;
	lexbor_str_t str = {0};

	zval_string_or_null_to_lexbor_str(value, &str);

	lxb_url_parser_clean(&lexbor_parser);

	if (lxb_url_api_password_set(lexbor_uri, str.data, str.length) != LXB_STATUS_OK) {
		throw_invalid_url_exception_during_write(errors, "password");

		return FAILURE;
	}

	return SUCCESS;
}

static zend_result php_uri_parser_whatwg_host_read(void *uri, const php_uri_component_read_mode read_mode, zval *retval)
{
	const lxb_url_t *lexbor_uri = uri;

	if (lexbor_uri->host.type == LXB_URL_HOST_TYPE_IPV4) {
		smart_str host_str = {0};

		lxb_url_serialize_host_ipv4(lexbor_uri->host.u.ipv4, serialize_to_smart_str_callback, &host_str);

		ZVAL_NEW_STR(retval, smart_str_extract(&host_str));
	} else if (lexbor_uri->host.type == LXB_URL_HOST_TYPE_IPV6) {
		smart_str host_str = {0};

		smart_str_appendc(&host_str, '[');
		lxb_url_serialize_host_ipv6(lexbor_uri->host.u.ipv6, serialize_to_smart_str_callback, &host_str);
		smart_str_appendc(&host_str, ']');

		ZVAL_NEW_STR(retval, smart_str_extract(&host_str));
	} else if (lexbor_uri->host.type == LXB_URL_HOST_TYPE_EMPTY) {
		ZVAL_EMPTY_STRING(retval);
	} else if (lexbor_uri->host.type != LXB_URL_HOST_TYPE__UNDEF) {
		switch (read_mode) {
			case PHP_URI_COMPONENT_READ_MODE_NORMALIZED_UNICODE: {
				smart_str host_str = {0};
				lxb_url_serialize_host_unicode(&lexbor_idna, &lexbor_uri->host, serialize_to_smart_str_callback, &host_str);
				lxb_unicode_idna_clean(&lexbor_idna);

				ZVAL_STR(retval, smart_str_extract(&host_str));
				break;
			}
			case PHP_URI_COMPONENT_READ_MODE_NORMALIZED_ASCII:
				ZEND_FALLTHROUGH;
			case PHP_URI_COMPONENT_READ_MODE_RAW:
				ZVAL_STRINGL(retval, (const char *) lexbor_uri->host.u.domain.data, lexbor_uri->host.u.domain.length);
				break;
			default: ZEND_UNREACHABLE();
		}
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

ZEND_ATTRIBUTE_NONNULL void php_uri_parser_whatwg_host_type_read(const lxb_url_t *lexbor_uri, zval *retval)
{
	switch (lexbor_uri->host.type) {
		case LXB_URL_HOST_TYPE_IPV4:
			ZVAL_OBJ_COPY(retval, zend_enum_get_case_cstr(php_uri_ce_whatwg_url_host_type, "IPv4"));
			return;
		case LXB_URL_HOST_TYPE_IPV6:
			ZVAL_OBJ_COPY(retval, zend_enum_get_case_cstr(php_uri_ce_whatwg_url_host_type, "IPv6"));
			return;
		case LXB_URL_HOST_TYPE_DOMAIN:
			ZVAL_OBJ_COPY(retval, zend_enum_get_case_cstr(php_uri_ce_whatwg_url_host_type, "Domain"));
			return;
		case LXB_URL_HOST_TYPE_EMPTY:
			ZVAL_OBJ_COPY(retval, zend_enum_get_case_cstr(php_uri_ce_whatwg_url_host_type, "Empty"));
			return;
		case LXB_URL_HOST_TYPE_OPAQUE:
			ZVAL_OBJ_COPY(retval, zend_enum_get_case_cstr(php_uri_ce_whatwg_url_host_type, "Opaque"));
			return;
		case LXB_URL_HOST_TYPE__UNDEF:
			ZVAL_NULL(retval);
			return;
		default: ZEND_UNREACHABLE();
	}
}

static zend_result php_uri_parser_whatwg_host_write(void *uri, const zval *value, zval *errors)
{
	lxb_url_t *lexbor_uri = uri;
	lexbor_str_t str = {0};

	zval_string_or_null_to_lexbor_str(value, &str);

	lxb_url_parser_clean(&lexbor_parser);

	if (lxb_url_api_hostname_set(lexbor_uri, &lexbor_parser, str.data, str.length) != LXB_STATUS_OK) {
		throw_invalid_url_exception_during_write(errors, "host");

		return FAILURE;
	}

	return SUCCESS;
}

static zend_result php_uri_parser_whatwg_port_read(void *uri, const php_uri_component_read_mode read_mode, zval *retval)
{
	const lxb_url_t *lexbor_uri = uri;

	if (lexbor_uri->has_port) {
		ZVAL_LONG(retval, lexbor_uri->port);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result php_uri_parser_whatwg_port_write(void *uri, const zval *value, zval *errors)
{
	lxb_url_t *lexbor_uri = uri;
	lexbor_str_t str = {0};

	zval_long_or_null_to_lexbor_str(value, &str);

	lxb_url_parser_clean(&lexbor_parser);

	if (lxb_url_api_port_set(lexbor_uri, &lexbor_parser, str.data, str.length) != LXB_STATUS_OK) {
		throw_invalid_url_exception_during_write(errors, "port");

		return FAILURE;
	}

	return SUCCESS;
}

static zend_result php_uri_parser_whatwg_path_read(void *uri, const php_uri_component_read_mode read_mode, zval *retval)
{
	const lxb_url_t *lexbor_uri = uri;

	if (lexbor_uri->path.str.length > 0) {
		ZVAL_STRINGL(retval, (const char *) lexbor_uri->path.str.data, lexbor_uri->path.str.length);
	} else {
		ZVAL_EMPTY_STRING(retval);
	}

	return SUCCESS;
}

static zend_result php_uri_parser_whatwg_path_write(void *uri, const zval *value, zval *errors)
{
	lxb_url_t *lexbor_uri = uri;
	lexbor_str_t str = {0};

	zval_string_or_null_to_lexbor_str(value, &str);

	lxb_url_parser_clean(&lexbor_parser);

	if (lxb_url_api_pathname_set(lexbor_uri, &lexbor_parser, str.data, str.length) != LXB_STATUS_OK) {
		throw_invalid_url_exception_during_write(errors, "path");

		return FAILURE;
	}

	return SUCCESS;
}

static zend_result php_uri_parser_whatwg_query_read(void *uri, const php_uri_component_read_mode read_mode, zval *retval)
{
	const lxb_url_t *lexbor_uri = uri;

	if (lexbor_uri->query.data != NULL) {
		ZVAL_STRINGL(retval, (const char *) lexbor_uri->query.data, lexbor_uri->query.length);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result php_uri_parser_whatwg_query_write(void *uri, const zval *value, zval *errors)
{
	lxb_url_t *lexbor_uri = uri;
	lexbor_str_t str = {0};

	zval_string_or_null_to_lexbor_str(value, &str);

	lxb_url_parser_clean(&lexbor_parser);

	if (lxb_url_api_search_set(lexbor_uri, &lexbor_parser, str.data, str.length) != LXB_STATUS_OK) {
		throw_invalid_url_exception_during_write(errors, "query string");

		return FAILURE;
	}

	return SUCCESS;
}

static zend_result php_uri_parser_whatwg_fragment_read(void *uri, const php_uri_component_read_mode read_mode, zval *retval)
{
	const lxb_url_t *lexbor_uri = uri;

	if (lexbor_uri->fragment.data != NULL) {
		ZVAL_STRINGL(retval, (const char *) lexbor_uri->fragment.data, lexbor_uri->fragment.length);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result php_uri_parser_whatwg_fragment_write(void *uri, const zval *value, zval *errors)
{
	lxb_url_t *lexbor_uri = uri;
	lexbor_str_t str = {0};

	zval_string_or_null_to_lexbor_str(value, &str);

	lxb_url_parser_clean(&lexbor_parser);

	if (lxb_url_api_hash_set(lexbor_uri, &lexbor_parser, str.data, str.length) != LXB_STATUS_OK) {
		throw_invalid_url_exception_during_write(errors, "fragment");

		return FAILURE;
	}

	return SUCCESS;
}

PHP_RINIT_FUNCTION(uri_parser_whatwg)
{
	lxb_status_t status;
	
	status = lexbor_mraw_init(&lexbor_mraw, lexbor_mraw_byte_size);
	if (status != LXB_STATUS_OK) {
		goto fail;
	}

	status = lxb_url_parser_init(&lexbor_parser, &lexbor_mraw);
	if (status != LXB_STATUS_OK) {
		goto fail;
	}

	status = lxb_unicode_idna_init(&lexbor_idna);
	if (status != LXB_STATUS_OK) {
		goto fail;
	}

	return SUCCESS;

 fail:

	/* Unconditionally calling the _destroy() functions is
	 * safe on a zeroed structure. */
	lxb_unicode_idna_destroy(&lexbor_idna, false);
	memset(&lexbor_idna, 0, sizeof(lexbor_idna));
	lxb_url_parser_destroy(&lexbor_parser, false);
	memset(&lexbor_parser, 0, sizeof(lexbor_parser));
	lexbor_mraw_destroy(&lexbor_mraw, false);
	memset(&lexbor_mraw, 0, sizeof(lexbor_mraw));

	return FAILURE;
}

ZEND_MODULE_POST_ZEND_DEACTIVATE_D(uri_parser_whatwg)
{
	lxb_unicode_idna_destroy(&lexbor_idna, false);
	memset(&lexbor_idna, 0, sizeof(lexbor_idna));
	lxb_url_parser_destroy(&lexbor_parser, false);
	memset(&lexbor_parser, 0, sizeof(lexbor_parser));
	lexbor_mraw_destroy(&lexbor_mraw, false);
	memset(&lexbor_mraw, 0, sizeof(lexbor_mraw));

	return SUCCESS;
}

lxb_url_t *php_uri_parser_whatwg_parse_ex(const char *uri_str, const size_t uri_str_len, const lxb_url_t *lexbor_base_url, zval *errors, const bool silent)
{
	lxb_url_parser_clean(&lexbor_parser);

	lxb_url_t *url = lxb_url_parse(&lexbor_parser, lexbor_base_url, (unsigned char *) uri_str, uri_str_len);

	if ((url == NULL && !silent) || errors != NULL) {
		zval err;
		const char *reason = fill_errors(&err);
		if (url == NULL && !silent) {
			zend_object *exception = zend_throw_exception_ex(php_uri_ce_whatwg_invalid_url_exception, 0, "The specified URI is malformed%s%s%s", reason ? " (" : "", reason ? reason : "", reason ? ")" : "");
			zend_update_property(exception->ce, exception, ZEND_STRL("errors"), &err);
		}
		if (errors != NULL) {
			zval_ptr_dtor(errors);
			ZVAL_COPY_VALUE(errors, &err);
		} else {
			zval_ptr_dtor(&err);
		}
	}

	return url;
}

static void *php_uri_parser_whatwg_parse(const char *uri_str, const size_t uri_str_len, const void *base_url, zval *errors, const bool silent)
{
	return php_uri_parser_whatwg_parse_ex(uri_str, uri_str_len, base_url, errors, silent);
}

static void *php_uri_parser_whatwg_clone(void *uri)
{
	const lxb_url_t *lexbor_uri = uri;

	return lxb_url_clone(lexbor_parser.mraw, lexbor_uri);
}

static zend_string *php_uri_parser_whatwg_to_string(void *uri, const php_uri_recomposition_mode recomposition_mode, const bool exclude_fragment)
{
	const lxb_url_t *lexbor_uri = uri;
	smart_str uri_str = {0};

	switch (recomposition_mode) {
		case PHP_URI_RECOMPOSITION_MODE_RAW_UNICODE:
			ZEND_FALLTHROUGH;
		case PHP_URI_RECOMPOSITION_MODE_NORMALIZED_UNICODE:
			lxb_url_serialize_idna(&lexbor_idna, lexbor_uri, serialize_to_smart_str_callback, &uri_str, exclude_fragment);
			lxb_unicode_idna_clean(&lexbor_idna);
			break;
		case PHP_URI_RECOMPOSITION_MODE_RAW_ASCII:
			ZEND_FALLTHROUGH;
		case PHP_URI_RECOMPOSITION_MODE_NORMALIZED_ASCII:
			lxb_url_serialize(lexbor_uri, serialize_to_smart_str_callback, &uri_str, exclude_fragment);
			break;
		default: ZEND_UNREACHABLE();
	}

	return smart_str_extract(&uri_str);
}

static void php_uri_parser_whatwg_destroy(void *uri)
{
	lxb_url_t *lexbor_uri = uri;

	lxb_url_destroy(lexbor_uri);
}

static zend_always_inline zend_result php_uri_parser_whatwg_validate_component_result(const bool well_formed, const char *component_name)
{
	if (well_formed) {
		return SUCCESS;
	}

	zend_throw_exception_ex(php_uri_ce_whatwg_invalid_url_exception, 0, "The specified %s is malformed", component_name);
	return FAILURE;
}

ZEND_ATTRIBUTE_NONNULL zend_result php_uri_parser_whatwg_validate_none(const zend_string *component)
{
	return SUCCESS;
}

static zend_always_inline bool php_uri_parser_whatwg_is_alpha(unsigned char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static zend_always_inline bool php_uri_parser_whatwg_is_digit(unsigned char c)
{
	return c >= '0' && c <= '9';
}

ZEND_ATTRIBUTE_NONNULL zend_result php_uri_parser_whatwg_validate_scheme(const zend_string *scheme)
{
	const char *p = ZSTR_VAL(scheme);
	const size_t len = ZSTR_LEN(scheme);
	bool seen_first = false;

	for (size_t i = 0; i < len; i++) {
		const unsigned char c = (unsigned char)p[i];

		if (c == '\t' || c == '\n' || c == '\r') {
			continue;
		}

		if (!seen_first) {
			if (!php_uri_parser_whatwg_is_alpha(c)) {
				return php_uri_parser_whatwg_validate_component_result(false, "scheme");
			}
			seen_first = true;
		} else {
			if (!php_uri_parser_whatwg_is_alpha(c) && !php_uri_parser_whatwg_is_digit(c) && c != '+' && c != '-' && c != '.') {
				return php_uri_parser_whatwg_validate_component_result(false, "scheme");
			}
		}
	}

	if (!seen_first) {
		return php_uri_parser_whatwg_validate_component_result(false, "scheme");
	}

	return true;
}

ZEND_ATTRIBUTE_NONNULL zend_result php_uri_parser_whatwg_validate_port(const zend_long port)
{
	const bool well_formed = port >= 0 && port <= 65535;

	return php_uri_parser_whatwg_validate_component_result(well_formed, "port");
}

ZEND_ATTRIBUTE_NONNULL static bool php_uri_parser_whatwg_is_special_scheme(const zval *scheme)
{
	ZEND_ASSERT(Z_TYPE_P(scheme) == IS_STRING);

	const zend_string *str = Z_STR_P(scheme);

	switch (ZSTR_LEN(str)) {
		case 2:
			return zend_string_equals_literal_ci(str, "ws");
		case 3:
			return zend_string_equals_literal_ci(str, "ftp")
				|| zend_string_equals_literal_ci(str, "wss");
		case 4:
			return zend_string_equals_literal_ci(str, "http")
				|| zend_string_equals_literal_ci(str, "file");
		case 5:
			return zend_string_equals_literal_ci(str, "https");
		default:
			return false;
	}
}

ZEND_ATTRIBUTE_NONNULL static void php_uri_parser_whatwg_build_errors(zval *errors)
{
	size_t log_len;

	if (lexbor_parser.log == NULL || (log_len = lexbor_plog_length(lexbor_parser.log)) == 0) {
		return;
	}

	if (Z_TYPE_P(errors) != IS_ARRAY) {
		zval_ptr_dtor(errors);
		ZVAL_EMPTY_ARRAY(errors);
		array_init_size(errors, log_len);
	}

	fill_errors_inner(errors);
}

ZEND_ATTRIBUTE_NONNULL_ARGS(2, 3, 4, 5, 6, 7, 8, 9) lxb_url_t *php_uri_parser_whatwg_build_from_zval(
	lxb_url_t *lexbor_base_url, const zval *scheme, const zval *username, const zval *password,
	const zval *host, const zval *port, const zval *path, const zval *query, const zval *fragment,
	zval *errors_zv
) {
	const bool is_special_scheme = php_uri_parser_whatwg_is_special_scheme(scheme);

	/*
	 * The port must not be defined if the opaque URL does not contain a host.
	 * The extra validation is needed because ext/lexbor/lexbor/url/url.c:1964 would let the port updated even
	 * when the host is missing/empty, but this combination is rejected when parsing an URL string (e.g. scheme://:433).
	 * Is this an inconsistency between the behavior of the parser and port setter? https://github.com/lexbor/lexbor/issues/387
	 */
	if (!is_special_scheme && (Z_TYPE_P(host) == IS_NULL || Z_STRLEN_P(host) == 0) && Z_TYPE_P(port) != IS_NULL) {
		zend_throw_exception(php_uri_ce_whatwg_invalid_url_exception, "The specified URI is malformed (HostMissing)", 0);
		return NULL;
	}

	lxb_url_parser_clean(&lexbor_parser);

	lxb_url_t *lexbor_url = lexbor_mraw_calloc(lexbor_parser.mraw, sizeof(lxb_url_t));
	if (lexbor_url == NULL) {
		zend_throw_exception(php_uri_ce_whatwg_invalid_url_exception, "Memory allocation error", 0);
		return NULL;
	}

	lexbor_url->mraw = lexbor_parser.mraw;

	/*
	 * The URL is initialized as LXB_URL_SCHEMEL_TYPE__UNDEF but this would prevent from the scheme to be updated
	 * in case of non-special schemes due to https://github.com/php/php-src/blob/27d7b799c0a13578ee0506b428b8ddc209ffb010/ext/lexbor/lexbor/url/url.c#L1402
	 */
	if (is_special_scheme) {
		lexbor_url->scheme.type = LXB_URL_SCHEMEL_TYPE__UNDEF;
	} else {
		lexbor_url->scheme.type = LXB_URL_SCHEMEL_TYPE__UNKNOWN;
	}

	zval errors;
	ZVAL_UNDEF(&errors);

	zend_result result = php_uri_parser_whatwg_scheme_write(lexbor_url, scheme, NULL);
	php_uri_parser_whatwg_build_errors(&errors);
	if (result == FAILURE) {
		goto failure;
	}

	result = php_uri_parser_whatwg_host_write(lexbor_url, host, NULL);
	php_uri_parser_whatwg_build_errors(&errors);
	if (result == FAILURE) {
		goto failure;
	}

	/* Intentionally writing username after host to avoid error when the username is set but the host is missing */
	result = php_uri_parser_whatwg_username_write(lexbor_url, username, NULL);
	php_uri_parser_whatwg_build_errors(&errors);
	if (result == FAILURE) {
		goto failure;
	}

	/* Intentionally writing password after host to avoid error when the password is set but the password is missing */
	result = php_uri_parser_whatwg_password_write(lexbor_url, password, NULL);
	php_uri_parser_whatwg_build_errors(&errors);
	if (result == FAILURE) {
		goto failure;
	}

	/* Intentionally writing port after host to avoid error when the port is set but the host is missing */
	result = php_uri_parser_whatwg_port_write(lexbor_url, port, NULL);
	php_uri_parser_whatwg_build_errors(&errors);
	if (result == FAILURE) {
		goto failure;
	}

	result = php_uri_parser_whatwg_path_write(lexbor_url, path, NULL);
	php_uri_parser_whatwg_build_errors(&errors);
	if (result == FAILURE) {
		goto failure;
	}

	result = php_uri_parser_whatwg_query_write(lexbor_url, query, NULL);
	php_uri_parser_whatwg_build_errors(&errors);
	if (result == FAILURE) {
		goto failure;
	}

	result = php_uri_parser_whatwg_fragment_write(lexbor_url, fragment, NULL);
	php_uri_parser_whatwg_build_errors(&errors);
	if (result == FAILURE) {
		goto failure;
	}

	if (lexbor_base_url != NULL) {
		/* TODO */
	}

	if (php_uri_pass_errors_by_ref_and_free(errors_zv, &errors) == FAILURE) {
		goto failure;
	}

	return lexbor_url;

failure:
	zval_ptr_dtor(&errors);
	lxb_url_destroy(lexbor_url);
	return NULL;
}

PHPAPI const php_uri_parser php_uri_parser_whatwg = {
	.name = PHP_URI_PARSER_WHATWG,
	.parse = php_uri_parser_whatwg_parse,
	.clone = php_uri_parser_whatwg_clone,
	.to_string = php_uri_parser_whatwg_to_string,
	.destroy = php_uri_parser_whatwg_destroy,
	{
		.scheme = {.read = php_uri_parser_whatwg_scheme_read, .write = php_uri_parser_whatwg_scheme_write},
		.username = {.read = php_uri_parser_whatwg_username_read, .write = php_uri_parser_whatwg_username_write},
		.password = {.read = php_uri_parser_whatwg_password_read, .write = php_uri_parser_whatwg_password_write},
		.host = {.read = php_uri_parser_whatwg_host_read, .write = php_uri_parser_whatwg_host_write},
		.port = {.read = php_uri_parser_whatwg_port_read, .write = php_uri_parser_whatwg_port_write},
		.path = {.read = php_uri_parser_whatwg_path_read, .write = php_uri_parser_whatwg_path_write},
		.query = {.read = php_uri_parser_whatwg_query_read, .write = php_uri_parser_whatwg_query_write},
		.fragment = {.read = php_uri_parser_whatwg_fragment_read, .write = php_uri_parser_whatwg_fragment_write},
	}
};
