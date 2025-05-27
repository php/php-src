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
#include "php_lexbor.h"
#include "php_uri_common.h"
#include "Zend/zend_enum.h"
#include "Zend/zend_smart_str.h"
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

static zend_result lexbor_init_parser(void);
static void *lexbor_parse_uri(const zend_string *uri_str, const void *base_url, zval *errors);
static void lexbor_create_invalid_uri_exception(zval *exception_zv, zval *errors);
static void *lexbor_clone_uri(void *uri);
static zend_string *lexbor_uri_to_string(void *uri, uri_recomposition_mode_t recomposition_mode, bool exclude_fragment);
static void lexbor_free_uri(void *uri);
static zend_result lexbor_destroy_parser(void);

HashTable lexbor_property_handlers;

lxb_url_parser_t *lexbor_parser;
int lexbor_urls;

const uri_handler_t lexbor_uri_handler = {
	URI_PARSER_WHATWG,
	lexbor_init_parser,
	lexbor_parse_uri,
	lexbor_create_invalid_uri_exception,
	lexbor_clone_uri,
	lexbor_uri_to_string,
	lexbor_free_uri,
	lexbor_destroy_parser,
	&lexbor_property_handlers
};

#define ZVAL_TO_LEXBOR_STR(value, str) do { \
	if (Z_TYPE_P(value) == IS_STRING && Z_STRLEN_P(value) > 0) { \
		lexbor_str_init_append(&str, lexbor_parser->mraw, (const lxb_char_t *) Z_STRVAL_P(value), Z_STRLEN_P(value)); \
	} else if (Z_TYPE_P(value) == IS_LONG && Z_LVAL_P(value) != 0) { \
	    ZVAL_STR(value, zend_long_to_str(Z_LVAL_P(value))); \
		lexbor_str_init_append(&str, lexbor_parser->mraw, (const lxb_char_t *) Z_STRVAL_P(value), Z_STRLEN_P(value)); \
	} else { \
		lexbor_str_init(&str, lexbor_parser->mraw, 0); \
	} \
} while (0)

#define CHECK_WRITE_RESULT(status, lexbor_uri, str, errors) do { \
	if (status != LXB_STATUS_OK) { \
		fill_errors(errors); \
		return FAILURE; \
	} \
	return SUCCESS; \
} while (0)

#define LEXBOR_READ_ASCII_URI_COMPONENT(start, len, read_mode, retval) do { \
	switch (read_mode) { \
		case URI_COMPONENT_READ_RAW: /* Intentional fallthrough */ \
		case URI_COMPONENT_READ_NORMALIZED_UNICODE: /* Intentional fallthrough */ \
		case URI_COMPONENT_READ_NORMALIZED_ASCII: { \
			ZVAL_STRINGL(retval, (const char *) start, len);  \
			break; \
		} \
		EMPTY_SWITCH_DEFAULT_CASE() \
	} \
} while (0)

static void lexbor_cleanup_parser(void)
{
	if (++lexbor_urls % 500 == 0) {
		lexbor_mraw_clean(lexbor_parser->mraw);
	}

	lxb_url_parser_clean(lexbor_parser);
}

static void fill_errors(zval *errors)
{
	if (errors == NULL || lexbor_parser->log == NULL) {
		return;
	}

	if (lexbor_array_obj_size(&lexbor_parser->log->list)) {
		array_init(errors);
	}

	lexbor_plog_entry_t *lxb_error;
	while ((lxb_error = lexbor_array_obj_pop(&lexbor_parser->log->list)) != NULL) {
		zval error;
		object_init_ex(&error, whatwg_url_validation_error_ce);
		zend_update_property_string(whatwg_url_validation_error_ce, Z_OBJ(error), "context", sizeof("context") - 1, (const char *) lxb_error->data);

		zend_string *error_str;
		zval failure;
		switch (lxb_error->id) {
			case LXB_URL_ERROR_TYPE_DOMAIN_TO_ASCII:
				error_str = zend_string_init("DomainToAscii", sizeof("DomainToAscii"), false);
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_DOMAIN_TO_UNICODE:
				error_str = zend_string_init("DomainToUnicode", sizeof("DomainToUnicode"), false);
				ZVAL_FALSE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_DOMAIN_INVALID_CODE_POINT:
				error_str = zend_string_init("DomainInvalidCodePoint", sizeof("DomainInvalidCodePoint"), false);
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_HOST_INVALID_CODE_POINT:
				error_str = zend_string_init("HostInvalidCodePoint", sizeof("HostInvalidCodePoint"), false);
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_IPV4_EMPTY_PART:
				error_str = zend_string_init("Ipv4EmptyPart", sizeof("Ipv4EmptyPart"), false);
				ZVAL_FALSE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_IPV4_TOO_MANY_PARTS:
				error_str = zend_string_init("Ipv4TooManyParts", sizeof("Ipv4TooManyParts"), false);
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_IPV4_NON_NUMERIC_PART:
				error_str = zend_string_init("Ipv4NonNumericPart", sizeof("Ipv4NonNumericPart"), false);
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_IPV4_NON_DECIMAL_PART:
				error_str = zend_string_init("Ipv4NonDecimalPart", sizeof("Ipv4NonDecimalPart"), false);
				ZVAL_FALSE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_IPV4_OUT_OF_RANGE_PART:
				error_str = zend_string_init("Ipv4OutOfRangePart", sizeof("Ipv4OutOfRangePart"), false);
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_IPV6_UNCLOSED:
				error_str = zend_string_init("Ipv6Unclosed", sizeof("Ipv6Unclosed"), false);
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_IPV6_INVALID_COMPRESSION:
				error_str = zend_string_init("Ipv6InvalidCompression", sizeof("Ipv6InvalidCompression"), false);
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_IPV6_TOO_MANY_PIECES:
				error_str = zend_string_init("Ipv6TooManyPieces", sizeof("Ipv6TooManyPieces"), false);
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_IPV6_MULTIPLE_COMPRESSION:
				error_str = zend_string_init("Ipv6MultipleCompression", sizeof("Ipv6MultipleCompression"), false);
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_IPV6_INVALID_CODE_POINT:
				error_str = zend_string_init("Ipv6InvalidCodePoint", sizeof("Ipv6InvalidCodePoint"), false);
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_IPV6_TOO_FEW_PIECES:
				error_str = zend_string_init("Ipv6TooFewPieces", sizeof("Ipv6TooFewPieces"), false);
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_IPV4_IN_IPV6_TOO_MANY_PIECES:
				error_str = zend_string_init("Ipv4InIpv6TooManyPieces", sizeof("Ipv4InIpv6TooManyPieces"), false);
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_IPV4_IN_IPV6_INVALID_CODE_POINT:
				error_str = zend_string_init("Ipv4InIpv6InvalidCodePoint", sizeof("Ipv4InIpv6InvalidCodePoint"), false);
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_IPV4_IN_IPV6_OUT_OF_RANGE_PART:
				error_str = zend_string_init("Ipv4InIpv6OutOfRangePart", sizeof("Ipv4InIpv6OutOfRangePart"), false);
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_IPV4_IN_IPV6_TOO_FEW_PARTS:
				error_str = zend_string_init("Ipv4InIpv6TooFewParts", sizeof("Ipv4InIpv6TooFewParts"), false);
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_INVALID_URL_UNIT:
				error_str = zend_string_init("InvalidUrlUnit", sizeof("InvalidUrlUnit"), false);
				ZVAL_FALSE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_SPECIAL_SCHEME_MISSING_FOLLOWING_SOLIDUS:
				error_str = zend_string_init("SpecialSchemeMissingFollowingSolidus", sizeof("SpecialSchemeMissingFollowingSolidus"), false);
				ZVAL_FALSE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_MISSING_SCHEME_NON_RELATIVE_URL:
				error_str = zend_string_init("MissingSchemeNonRelativeUrl", sizeof("MissingSchemeNonRelativeUrl"), false);
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_INVALID_REVERSE_SOLIDUS:
				error_str = zend_string_init("InvalidReverseSoldius", sizeof("InvalidReverseSoldius"), false);
				ZVAL_FALSE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_INVALID_CREDENTIALS:
				error_str = zend_string_init("InvalidCredentials", sizeof("InvalidCredentials"), false);
				ZVAL_FALSE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_HOST_MISSING:
				error_str = zend_string_init("HostMissing", sizeof("HostMissing"), false);
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_PORT_OUT_OF_RANGE:
				error_str = zend_string_init("PortOutOfRange", sizeof("PortOutOfRange"), false);
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_PORT_INVALID:
				error_str = zend_string_init("PortInvalid", sizeof("PortInvalid"), false);
				ZVAL_TRUE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER:
				error_str = zend_string_init("FileInvalidWindowsDriveLetter", sizeof("FileInvalidWindowsDriveLetter"), false);
				ZVAL_FALSE(&failure);
				break;
			case LXB_URL_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_HOST:
				error_str = zend_string_init("FileInvalidWindowsDriveLetterHost", sizeof("FileInvalidWindowsDriveLetterHost"), false);
				ZVAL_FALSE(&failure);
				break;
			EMPTY_SWITCH_DEFAULT_CASE()
		}

		zval error_type;
		zend_enum_new(&error_type, whatwg_url_validation_error_type_ce, error_str, NULL);
		zend_update_property(whatwg_url_validation_error_ce, Z_OBJ(error), "type", sizeof("type") - 1, &error_type);
		zend_string_release(error_str);
		zval_ptr_dtor(&error_type);

		zend_update_property(whatwg_url_validation_error_ce, Z_OBJ(error), "failure", sizeof("failure") - 1, &failure);
		zval_ptr_dtor(&failure);

		add_next_index_zval(errors, &error);
	}

	lexbor_array_obj_clean(&lexbor_parser->log->list); // check if needed
}

static lxb_status_t lexbor_serialize_callback(const lxb_char_t *data, size_t length, void *ctx)
{
	smart_str *uri_str = (smart_str *) ctx;

	if (data != NULL && length > 0) {
		smart_str_appendl(uri_str, (const char *) data, length);
	}

	return LXB_STATUS_OK;
}

static zend_result lexbor_read_scheme(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	lxb_url_t *lexbor_uri = (lxb_url_t *) internal_uri->uri;

	ZEND_ASSERT(lexbor_uri->scheme.type != LXB_URL_SCHEMEL_TYPE__UNDEF);

	ZVAL_STRINGL(retval, (const char *) lexbor_uri->scheme.name.data, lexbor_uri->scheme.name.length);

	return SUCCESS;
}

static zend_result lexbor_write_scheme(uri_internal_t *internal_uri, zval *value, zval *errors)
{
	lxb_url_t *lexbor_uri = (lxb_url_t *) internal_uri->uri;
	lexbor_str_t str = {0};

	ZVAL_TO_LEXBOR_STR(value, str);

	lxb_status_t status = lxb_url_api_protocol_set(lexbor_uri, lexbor_parser, str.data, str.length);

	CHECK_WRITE_RESULT(status, lexbor_uri, str, errors);
}

static zend_result lexbor_read_username(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	lxb_url_t *lexbor_uri = (lxb_url_t *) internal_uri->uri;

	if (lexbor_uri->username.length) {
		LEXBOR_READ_ASCII_URI_COMPONENT(lexbor_uri->username.data, lexbor_uri->username.length, read_mode, retval);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result lexbor_write_username(uri_internal_t *internal_uri, zval *value, zval *errors)
{
	lxb_url_t *lexbor_uri = (lxb_url_t *) internal_uri->uri;
	lexbor_str_t str = {0};

	ZVAL_TO_LEXBOR_STR(value, str);

	lxb_status_t status = lxb_url_api_username_set(lexbor_uri, str.data, str.length);

	CHECK_WRITE_RESULT(status, lexbor_uri, str, errors);
}

static zend_result lexbor_read_password(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	lxb_url_t *lexbor_uri = (lxb_url_t *) internal_uri->uri;

	if (lexbor_uri->password.length > 0) {
		LEXBOR_READ_ASCII_URI_COMPONENT(lexbor_uri->password.data, lexbor_uri->password.length, read_mode, retval);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result lexbor_write_password(uri_internal_t *internal_uri, zval *value, zval *errors)
{
	lxb_url_t *lexbor_uri = (lxb_url_t *) internal_uri->uri;
	lexbor_str_t str = {0};

	ZVAL_TO_LEXBOR_STR(value, str);

	lxb_status_t status = lxb_url_api_password_set(lexbor_uri, str.data, str.length);

	CHECK_WRITE_RESULT(status, lexbor_uri, str, errors);
}

static zend_result lexbor_read_host(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	lxb_url_t *lexbor_uri = (lxb_url_t *) internal_uri->uri;

	if (lexbor_uri->host.type == LXB_URL_HOST_TYPE_IPV4) {
		smart_str host_str = {0};

		lxb_url_serialize_host_ipv4((zend_ulong) lexbor_uri->host.u.ipv4, lexbor_serialize_callback, (void *) &host_str);

		ZVAL_STR(retval, smart_str_extract(&host_str));
	} else if (lexbor_uri->host.type == LXB_URL_HOST_TYPE_IPV6) {
		smart_str host_str = {0};

		smart_str_appendc(&host_str, '[');
		lxb_url_serialize_host_ipv6(lexbor_uri->host.u.ipv6, lexbor_serialize_callback, (void *) &host_str);
		smart_str_appendc(&host_str, ']');

		ZVAL_STR(retval, smart_str_extract(&host_str));
	} else if (lexbor_uri->host.type != LXB_URL_HOST_TYPE_EMPTY && lexbor_uri->host.type != LXB_URL_HOST_TYPE__UNDEF) {

		switch (read_mode) {
			case URI_COMPONENT_READ_NORMALIZED_UNICODE: {
				smart_str host_str = {0};
				if (lexbor_parser->idna == NULL) {
					lexbor_parser->idna = lxb_unicode_idna_create();
					lxb_status_t status = lxb_unicode_idna_init(lexbor_parser->idna);
					if (status != LXB_STATUS_OK) {
						return FAILURE;
					}
				}
				lxb_url_serialize_host_unicode(lexbor_parser->idna, &lexbor_uri->host, lexbor_serialize_callback, (void *) &host_str);
				lxb_unicode_idna_clean(lexbor_parser->idna);

				ZVAL_STR(retval, smart_str_extract(&host_str));
				break;
			}
			case URI_COMPONENT_READ_NORMALIZED_ASCII: /* Intentional fallthrough */
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

static zend_result lexbor_write_host(uri_internal_t *internal_uri, zval *value, zval *errors)
{
	lxb_url_t *lexbor_uri = (lxb_url_t *) internal_uri->uri;
	lexbor_str_t str = {0};

	ZVAL_TO_LEXBOR_STR(value, str);

	lxb_status_t status = lxb_url_api_hostname_set(lexbor_uri, lexbor_parser, str.data, str.length);

	CHECK_WRITE_RESULT(status, lexbor_uri, str, errors);
}

static zend_result lexbor_read_port(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	lxb_url_t *lexbor_uri = (lxb_url_t *) internal_uri->uri;

	if (lexbor_uri->has_port) {
		ZVAL_LONG(retval, lexbor_uri->port);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result lexbor_write_port(uri_internal_t *internal_uri, zval *value, zval *errors)
{
	lxb_url_t *lexbor_uri = (lxb_url_t *) internal_uri->uri;
	lexbor_str_t str = {0};

	ZVAL_TO_LEXBOR_STR(value, str);

	lxb_status_t status = lxb_url_api_port_set(lexbor_uri, lexbor_parser, str.data, str.length);

	CHECK_WRITE_RESULT(status, lexbor_uri, str, errors);
}

static zend_result lexbor_read_path(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	lxb_url_t *lexbor_uri = (lxb_url_t *) internal_uri->uri;

	if (lexbor_uri->path.str.length) {
		LEXBOR_READ_ASCII_URI_COMPONENT(lexbor_uri->path.str.data, lexbor_uri->path.str.length, read_mode, retval);
	} else {
		ZVAL_EMPTY_STRING(retval);
	}

	return SUCCESS;
}

static zend_result lexbor_write_path(uri_internal_t *internal_uri, zval *value, zval *errors)
{
	lxb_url_t *lexbor_uri = (lxb_url_t *) internal_uri->uri;
	lexbor_str_t str = {0};

	ZVAL_TO_LEXBOR_STR(value, str);

	lxb_status_t status = lxb_url_api_pathname_set(lexbor_uri, lexbor_parser, str.data, str.length);

	CHECK_WRITE_RESULT(status, lexbor_uri, str, errors);
}

static zend_result lexbor_read_query(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	lxb_url_t *lexbor_uri = (lxb_url_t *) internal_uri->uri;

	if (lexbor_uri->query.length) {
		LEXBOR_READ_ASCII_URI_COMPONENT(lexbor_uri->query.data, lexbor_uri->query.length, read_mode, retval);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result lexbor_write_query(uri_internal_t *internal_uri, zval *value, zval *errors)
{
	lxb_url_t *lexbor_uri = (lxb_url_t *) internal_uri->uri;
	lexbor_str_t str = {0};

	ZVAL_TO_LEXBOR_STR(value, str);

	lxb_status_t status = lxb_url_api_search_set(lexbor_uri, lexbor_parser, str.data, str.length);

	CHECK_WRITE_RESULT(status, lexbor_uri, str, errors);
}

static zend_result lexbor_read_fragment(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	lxb_url_t *lexbor_uri = (lxb_url_t *) internal_uri->uri;

	if (lexbor_uri->fragment.length) {
		LEXBOR_READ_ASCII_URI_COMPONENT(lexbor_uri->fragment.data, lexbor_uri->fragment.length, read_mode, retval);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result lexbor_write_fragment(uri_internal_t *internal_uri, zval *value, zval *errors)
{
	lxb_url_t *lexbor_uri = (lxb_url_t *) internal_uri->uri;
	lexbor_str_t str = {0};

	ZVAL_TO_LEXBOR_STR(value, str);

	lxb_status_t status = lxb_url_api_hash_set(lexbor_uri, lexbor_parser, str.data, str.length);

	CHECK_WRITE_RESULT(status, lexbor_uri, str, errors);
}

static zend_result lexbor_init_parser(void)
{
	lexbor_mraw_t *mraw = lexbor_mraw_create();
	lxb_status_t status = lexbor_mraw_init(mraw, 4096 * 2);
	if (status != LXB_STATUS_OK) {
		lexbor_mraw_destroy(mraw, true);
		return FAILURE;
	}

	lxb_url_parser_t *parser = lxb_url_parser_create();
	status = lxb_url_parser_init(parser, mraw);
	if (status != LXB_STATUS_OK) {
		lxb_url_parser_destroy(parser, true);
		return FAILURE;
	}

	lexbor_parser = parser;
	lexbor_urls = 0;

	zend_hash_init(&lexbor_property_handlers, 8, NULL, NULL, true);

	URI_REGISTER_PROPERTY_READ_WRITE_HANDLER(&lexbor_property_handlers, ZSTR_KNOWN(ZEND_STR_SCHEME), lexbor_read_scheme, lexbor_write_scheme);
	URI_REGISTER_PROPERTY_READ_WRITE_HANDLER(&lexbor_property_handlers, ZSTR_KNOWN(ZEND_STR_USERNAME), lexbor_read_username, lexbor_write_username);
	URI_REGISTER_PROPERTY_READ_WRITE_HANDLER(&lexbor_property_handlers, ZSTR_KNOWN(ZEND_STR_PASSWORD), lexbor_read_password, lexbor_write_password);
	URI_REGISTER_PROPERTY_READ_WRITE_HANDLER(&lexbor_property_handlers, ZSTR_KNOWN(ZEND_STR_HOST), lexbor_read_host, lexbor_write_host);
	URI_REGISTER_PROPERTY_READ_WRITE_HANDLER(&lexbor_property_handlers, ZSTR_KNOWN(ZEND_STR_PORT), lexbor_read_port, lexbor_write_port);
	URI_REGISTER_PROPERTY_READ_WRITE_HANDLER(&lexbor_property_handlers, ZSTR_KNOWN(ZEND_STR_PATH), lexbor_read_path, lexbor_write_path);
	URI_REGISTER_PROPERTY_READ_WRITE_HANDLER(&lexbor_property_handlers, ZSTR_KNOWN(ZEND_STR_QUERY), lexbor_read_query, lexbor_write_query);
	URI_REGISTER_PROPERTY_READ_WRITE_HANDLER(&lexbor_property_handlers, ZSTR_KNOWN(ZEND_STR_FRAGMENT), lexbor_read_fragment, lexbor_write_fragment);

	return SUCCESS;
}

static void *lexbor_parse_uri(const zend_string *uri_str, const void *base_url, zval *errors)
{
	lexbor_cleanup_parser();

	lxb_url_t *url, *lexbor_base_url = NULL;

	if (base_url) {
		lexbor_base_url = (lxb_url_t *) base_url;
	}

	url = lxb_url_parse(lexbor_parser, lexbor_base_url, (unsigned char *) ZSTR_VAL(uri_str), ZSTR_LEN(uri_str));
	fill_errors(errors);

	return url;
}

static void lexbor_create_invalid_uri_exception(zval *exception_zv, zval *errors)
{
	object_init_ex(exception_zv, whatwg_invalid_url_exception_ce);

	zend_update_property_string(
		whatwg_invalid_url_exception_ce,
		Z_OBJ_P(exception_zv),
		ZSTR_VAL(ZSTR_KNOWN(ZEND_STR_MESSAGE)),
		ZSTR_LEN(ZSTR_KNOWN(ZEND_STR_MESSAGE)),
		"URL parsing failed"
	);

	zend_update_property(whatwg_invalid_url_exception_ce, Z_OBJ_P(exception_zv), "errors", sizeof("errors") - 1, errors);
}


static void *lexbor_clone_uri(void *uri)
{
	lxb_url_t *lexbor_uri = (lxb_url_t *) uri;

	return lxb_url_clone(lexbor_parser->mraw, lexbor_uri);
}

static zend_string *lexbor_uri_to_string(void *uri, uri_recomposition_mode_t recomposition_mode, bool exclude_fragment)
{
	lxb_url_t *lexbor_uri = (lxb_url_t *) uri;
	smart_str uri_str = {0};

	switch (recomposition_mode) {
		case URI_RECOMPOSITION_RAW_UNICODE: /* Intentional fallthrough */
		case URI_RECOMPOSITION_NORMALIZED_UNICODE:
			if (lexbor_parser->idna == NULL) {
				lexbor_parser->idna = lxb_unicode_idna_create();
				lxb_status_t status = lxb_unicode_idna_init(lexbor_parser->idna);
				if (status != LXB_STATUS_OK) {
					return NULL;
				}
			}
			lxb_url_serialize_idna(lexbor_parser->idna, lexbor_uri, lexbor_serialize_callback, (void *) &uri_str, exclude_fragment);
			lxb_unicode_idna_clean(lexbor_parser->idna);
			break;
		case URI_RECOMPOSITION_RAW_ASCII: /* Intentional fallthrough */
		case URI_RECOMPOSITION_NORMALIZED_ASCII:
			lxb_url_serialize(lexbor_uri, lexbor_serialize_callback, (void *) &uri_str, exclude_fragment);
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}

	return smart_str_extract(&uri_str);
}

static void lexbor_free_uri(void *uri)
{
}

static zend_result lexbor_destroy_parser(void)
{
	lxb_url_parser_memory_destroy(lexbor_parser);
	lxb_url_parser_destroy(lexbor_parser, true);

	lexbor_parser = NULL;
	lexbor_urls = 0;

	zend_hash_destroy(&lexbor_property_handlers);

	return SUCCESS;
}
