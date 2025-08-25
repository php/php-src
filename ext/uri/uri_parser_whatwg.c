/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
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

ZEND_TLS lxb_url_parser_t lexbor_parser;
ZEND_TLS unsigned short int parsed_urls;

static const unsigned short int maximum_parses_before_cleanup = 500;
static const size_t lexbor_mraw_byte_size = 8192;

static zend_always_inline void zval_string_or_null_to_lexbor_str(zval *value, lexbor_str_t *lexbor_str)
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

static zend_always_inline void zval_long_or_null_to_lexbor_str(zval *value, lexbor_str_t *lexbor_str)
{
	if (Z_TYPE_P(value) == IS_LONG) {
		ZVAL_STR(value, zend_long_to_str(Z_LVAL_P(value)));
		lexbor_str_init_append(lexbor_str, lexbor_parser.mraw, (const lxb_char_t *) Z_STRVAL_P(value), Z_STRLEN_P(value));
		zval_ptr_dtor_str(value);
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
 *
 * When errors is NULL, the caller is not interested in the additional error information,
 * so the function does nothing.
 */
static const char *fill_errors(zval *errors)
{
	if (errors == NULL) {
		return NULL;
	}

	ZEND_ASSERT(Z_ISUNDEF_P(errors));

	array_init(errors);

	if (lexbor_parser.log == NULL) {
		return NULL;
	}

	const char *result = NULL;
	lexbor_plog_entry_t *lxb_error;
	while ((lxb_error = lexbor_array_obj_pop(&lexbor_parser.log->list)) != NULL) {
		zval error;
		object_init_ex(&error, uri_whatwg_url_validation_error_ce);
		zend_update_property_string(uri_whatwg_url_validation_error_ce, Z_OBJ(error), ZEND_STRL("context"), (const char *) lxb_error->data);

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
			EMPTY_SWITCH_DEFAULT_CASE()
		}

		zval error_type;
		ZVAL_OBJ(&error_type, zend_enum_get_case_cstr(uri_whatwg_url_validation_error_type_ce, error_str));
		zend_update_property_ex(uri_whatwg_url_validation_error_ce, Z_OBJ(error), ZSTR_KNOWN(ZEND_STR_TYPE), &error_type);

		zend_update_property(uri_whatwg_url_validation_error_ce, Z_OBJ(error), ZEND_STRL("failure"), &failure);

		if (Z_TYPE(failure) == IS_TRUE) {
			result = error_str;
		}

		add_next_index_zval(errors, &error);
	}

	return result;
}

static void throw_invalid_url_exception_during_write(zval *errors, const char *component)
{
	const char *reason = fill_errors(errors);
	zend_object *exception = zend_throw_exception_ex(
		uri_whatwg_invalid_url_exception_ce,
		0,
		"The specified %s is malformed%s%s%s",
		component,
		reason ? " (" : "",
		reason ? reason : "",
		reason ? ")" : ""
	);
	zend_update_property(exception->ce, exception, ZEND_STRL("errors"), errors);
}

static lxb_status_t serialize_to_smart_str_callback(const lxb_char_t *data, size_t length, void *ctx)
{
	smart_str *uri_str = ctx;

	if (data != NULL && length > 0) {
		smart_str_appendl(uri_str, (const char *) data, length);
	}

	return LXB_STATUS_OK;
}

static zend_result php_uri_parser_whatwg_scheme_read(const struct uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	const lxb_url_t *lexbor_uri = internal_uri->uri;

	ZEND_ASSERT(lexbor_uri->scheme.type != LXB_URL_SCHEMEL_TYPE__UNDEF);

	ZVAL_STRINGL(retval, (const char *) lexbor_uri->scheme.name.data, lexbor_uri->scheme.name.length);

	return SUCCESS;
}

static zend_result php_uri_parser_whatwg_scheme_write(struct uri_internal_t *internal_uri, zval *value, zval *errors)
{
	lxb_url_t *lexbor_uri = internal_uri->uri;
	lexbor_str_t str = {0};

	zval_string_or_null_to_lexbor_str(value, &str);

	if (lxb_url_api_protocol_set(lexbor_uri, &lexbor_parser, str.data, str.length) != LXB_STATUS_OK) {
		throw_invalid_url_exception_during_write(errors, "scheme");

		return FAILURE;
	}

	return SUCCESS;
}

static zend_result php_uri_parser_whatwg_username_read(const struct uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	const lxb_url_t *lexbor_uri = internal_uri->uri;

	if (lexbor_uri->username.length) {
		ZVAL_STRINGL(retval, (const char *) lexbor_uri->username.data, lexbor_uri->username.length);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result php_uri_parser_whatwg_username_write(uri_internal_t *internal_uri, zval *value, zval *errors)
{
	lxb_url_t *lexbor_uri = internal_uri->uri;
	lexbor_str_t str = {0};

	zval_string_or_null_to_lexbor_str(value, &str);

	if (lxb_url_api_username_set(lexbor_uri, str.data, str.length) != LXB_STATUS_OK) {
		throw_invalid_url_exception_during_write(errors, "username");

		return FAILURE;
	}

	return SUCCESS;
}

static zend_result php_uri_parser_whatwg_password_read(const struct uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	const lxb_url_t *lexbor_uri = internal_uri->uri;

	if (lexbor_uri->password.length > 0) {
		ZVAL_STRINGL(retval, (const char *) lexbor_uri->password.data, lexbor_uri->password.length);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result php_uri_parser_whatwg_password_write(struct uri_internal_t *internal_uri, zval *value, zval *errors)
{
	lxb_url_t *lexbor_uri = internal_uri->uri;
	lexbor_str_t str = {0};

	zval_string_or_null_to_lexbor_str(value, &str);

	if (lxb_url_api_password_set(lexbor_uri, str.data, str.length) != LXB_STATUS_OK) {
		throw_invalid_url_exception_during_write(errors, "password");

		return FAILURE;
	}

	return SUCCESS;
}

static zend_result init_idna(void)
{
	if (lexbor_parser.idna != NULL) {
		return SUCCESS;
	}

	lexbor_parser.idna = lxb_unicode_idna_create();

	return lxb_unicode_idna_init(lexbor_parser.idna) == LXB_STATUS_OK ? SUCCESS : FAILURE;
}

static zend_result php_uri_parser_whatwg_host_read(const struct uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	const lxb_url_t *lexbor_uri = internal_uri->uri;

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
			case URI_COMPONENT_READ_NORMALIZED_UNICODE: {
				smart_str host_str = {0};
				if (init_idna() == FAILURE) {
					return FAILURE;
				}
				lxb_url_serialize_host_unicode(lexbor_parser.idna, &lexbor_uri->host, serialize_to_smart_str_callback, &host_str);
				lxb_unicode_idna_clean(lexbor_parser.idna);

				ZVAL_NEW_STR(retval, smart_str_extract(&host_str));
				break;
			}
			case URI_COMPONENT_READ_NORMALIZED_ASCII:
				ZEND_FALLTHROUGH;
			case URI_COMPONENT_READ_RAW:
				ZVAL_STRINGL(retval, (const char *) lexbor_uri->host.u.domain.data, lexbor_uri->host.u.domain.length);
				break;
			EMPTY_SWITCH_DEFAULT_CASE()
		}
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result php_uri_parser_whatwg_host_write(struct uri_internal_t *internal_uri, zval *value, zval *errors)
{
	lxb_url_t *lexbor_uri = internal_uri->uri;
	lexbor_str_t str = {0};

	zval_string_or_null_to_lexbor_str(value, &str);

	if (lxb_url_api_hostname_set(lexbor_uri, &lexbor_parser, str.data, str.length) != LXB_STATUS_OK) {
		throw_invalid_url_exception_during_write(errors, "host");

		return FAILURE;
	}

	return SUCCESS;
}

static zend_result php_uri_parser_whatwg_port_read(const struct uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	const lxb_url_t *lexbor_uri = internal_uri->uri;

	if (lexbor_uri->has_port) {
		ZVAL_LONG(retval, lexbor_uri->port);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result php_uri_parser_whatwg_port_write(struct uri_internal_t *internal_uri, zval *value, zval *errors)
{
	lxb_url_t *lexbor_uri = internal_uri->uri;
	lexbor_str_t str = {0};

	zval_long_or_null_to_lexbor_str(value, &str);

	if (lxb_url_api_port_set(lexbor_uri, &lexbor_parser, str.data, str.length) != LXB_STATUS_OK) {
		throw_invalid_url_exception_during_write(errors, "port");

		return FAILURE;
	}

	return SUCCESS;
}

static zend_result php_uri_parser_whatwg_path_read(const struct uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	const lxb_url_t *lexbor_uri = internal_uri->uri;

	if (lexbor_uri->path.str.length) {
		ZVAL_STRINGL(retval, (const char *) lexbor_uri->path.str.data, lexbor_uri->path.str.length);
	} else {
		ZVAL_EMPTY_STRING(retval);
	}

	return SUCCESS;
}

static zend_result php_uri_parser_whatwg_path_write(struct uri_internal_t *internal_uri, zval *value, zval *errors)
{
	lxb_url_t *lexbor_uri = internal_uri->uri;
	lexbor_str_t str = {0};

	zval_string_or_null_to_lexbor_str(value, &str);

	if (lxb_url_api_pathname_set(lexbor_uri, &lexbor_parser, str.data, str.length) != LXB_STATUS_OK) {
		throw_invalid_url_exception_during_write(errors, "path");

		return FAILURE;
	}

	return SUCCESS;
}

static zend_result php_uri_parser_whatwg_query_read(const struct uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	const lxb_url_t *lexbor_uri = internal_uri->uri;

	if (lexbor_uri->query.length) {
		ZVAL_STRINGL(retval, (const char *) lexbor_uri->query.data, lexbor_uri->query.length);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result php_uri_parser_whatwg_query_write(struct uri_internal_t *internal_uri, zval *value, zval *errors)
{
	lxb_url_t *lexbor_uri = internal_uri->uri;
	lexbor_str_t str = {0};

	zval_string_or_null_to_lexbor_str(value, &str);

	if (lxb_url_api_search_set(lexbor_uri, &lexbor_parser, str.data, str.length) != LXB_STATUS_OK) {
		throw_invalid_url_exception_during_write(errors, "query string");

		return FAILURE;
	}

	return SUCCESS;
}

static zend_result php_uri_parser_whatwg_fragment_read(const struct uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	const lxb_url_t *lexbor_uri = internal_uri->uri;

	if (lexbor_uri->fragment.length) {
		ZVAL_STRINGL(retval, (const char *) lexbor_uri->fragment.data, lexbor_uri->fragment.length);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result php_uri_parser_whatwg_fragment_write(struct uri_internal_t *internal_uri, zval *value, zval *errors)
{
	lxb_url_t *lexbor_uri = internal_uri->uri;
	lexbor_str_t str = {0};

	zval_string_or_null_to_lexbor_str(value, &str);

	if (lxb_url_api_hash_set(lexbor_uri, &lexbor_parser, str.data, str.length) != LXB_STATUS_OK) {
		throw_invalid_url_exception_during_write(errors, "fragment");

		return FAILURE;
	}

	return SUCCESS;
}

PHP_RINIT_FUNCTION(uri_parser_whatwg)
{
	lexbor_mraw_t *mraw = lexbor_mraw_create();
	lxb_status_t status = lexbor_mraw_init(mraw, lexbor_mraw_byte_size);
	if (status != LXB_STATUS_OK) {
		lexbor_mraw_destroy(mraw, true);
		return FAILURE;
	}

	status = lxb_url_parser_init(&lexbor_parser, mraw);
	if (status != LXB_STATUS_OK) {
		lxb_url_parser_destroy(&lexbor_parser, false);
		lexbor_mraw_destroy(mraw, true);
		return FAILURE;
	}

	parsed_urls = 0;

	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(uri_parser_whatwg)
{
	lxb_url_parser_memory_destroy(&lexbor_parser);
	lxb_url_parser_destroy(&lexbor_parser, false);

	parsed_urls = 0;

	return SUCCESS;
}

static void reset_parser_state(void)
{
	if (++parsed_urls % maximum_parses_before_cleanup == 0) {
		lexbor_mraw_clean(lexbor_parser.mraw);
		parsed_urls = 0;
	}

	lxb_url_parser_clean(&lexbor_parser);
}

lxb_url_t *php_uri_parser_whatwg_parse_ex(const char *uri_str, size_t uri_str_len, const lxb_url_t *lexbor_base_url, zval *errors, bool silent)
{
	reset_parser_state();

	lxb_url_t *url = lxb_url_parse(&lexbor_parser, lexbor_base_url, (unsigned char *) uri_str, uri_str_len);
	const char *reason = fill_errors(errors);

	if (url == NULL && !silent) {
		zend_object *exception = zend_throw_exception_ex(uri_whatwg_invalid_url_exception_ce, 0, "The specified URI is malformed%s%s%s", reason ? " (" : "", reason ? reason : "", reason ? ")" : "");
		zend_update_property(exception->ce, exception, ZEND_STRL("errors"), errors);
	}

	return url;
}

static void *php_uri_parser_whatwg_parse(const char *uri_str, size_t uri_str_len, const void *base_url, zval *errors, bool silent)
{
	return php_uri_parser_whatwg_parse_ex(uri_str, uri_str_len, base_url, errors, silent);
}

static void *php_uri_parser_whatwg_clone(void *uri)
{
	const lxb_url_t *lexbor_uri = uri;

	return lxb_url_clone(lexbor_parser.mraw, lexbor_uri);
}

static zend_string *php_uri_parser_whatwg_to_string(void *uri, uri_recomposition_mode_t recomposition_mode, bool exclude_fragment)
{
	const lxb_url_t *lexbor_uri = uri;
	smart_str uri_str = {0};

	switch (recomposition_mode) {
		case URI_RECOMPOSITION_RAW_UNICODE:
			ZEND_FALLTHROUGH;
		case URI_RECOMPOSITION_NORMALIZED_UNICODE:
			if (init_idna() == FAILURE) {
				return NULL;
			}
			lxb_url_serialize_idna(lexbor_parser.idna, lexbor_uri, serialize_to_smart_str_callback, &uri_str, exclude_fragment);
			lxb_unicode_idna_clean(lexbor_parser.idna);
			break;
		case URI_RECOMPOSITION_RAW_ASCII:
			ZEND_FALLTHROUGH;
		case URI_RECOMPOSITION_NORMALIZED_ASCII:
			lxb_url_serialize(lexbor_uri, serialize_to_smart_str_callback, &uri_str, exclude_fragment);
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}

	return smart_str_extract(&uri_str);
}

static void php_uri_parser_whatwg_free(void *uri)
{
}

const uri_parser_t php_uri_parser_whatwg = {
	.name = PHP_URI_PARSER_WHATWG,
	.parse_uri = php_uri_parser_whatwg_parse,
	.clone_uri = php_uri_parser_whatwg_clone,
	.uri_to_string = php_uri_parser_whatwg_to_string,
	.free_uri = php_uri_parser_whatwg_free,
	{
		.scheme = {.read_func = php_uri_parser_whatwg_scheme_read, .write_func = php_uri_parser_whatwg_scheme_write},
		.username = {.read_func = php_uri_parser_whatwg_username_read, .write_func = php_uri_parser_whatwg_username_write},
		.password = {.read_func = php_uri_parser_whatwg_password_read, .write_func = php_uri_parser_whatwg_password_write},
		.host = {.read_func = php_uri_parser_whatwg_host_read, .write_func = php_uri_parser_whatwg_host_write},
		.port = {.read_func = php_uri_parser_whatwg_port_read, .write_func = php_uri_parser_whatwg_port_write},
		.path = {.read_func = php_uri_parser_whatwg_path_read, .write_func = php_uri_parser_whatwg_path_write},
		.query = {.read_func = php_uri_parser_whatwg_query_read, .write_func = php_uri_parser_whatwg_query_write},
		.fragment = {.read_func = php_uri_parser_whatwg_fragment_read, .write_func = php_uri_parser_whatwg_fragment_write},
	}
};
