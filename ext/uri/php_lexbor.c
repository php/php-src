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
static void *lexbor_parse_uri(const zend_string *url_str, const zend_string *base_url_str, zval *errors);
static zend_class_entry *lexbor_get_uri_ce(void);
static void *lexbor_clone_uri(void *uri);
static zend_string *lexbor_uri_to_string(void *uri, bool exclude_fragment);
static void lexbor_free_uri(void *uri);
static zend_result lexbor_destroy_parser(void);

HashTable lexbor_property_handlers;

lxb_url_parser_t *lexbor_parser;
int lexbor_urls;

const uri_handler_t lexbor_uri_handler = {
	URI_PARSER_WHATWG,
	lexbor_init_parser,
	lexbor_parse_uri,
	lexbor_get_uri_ce,
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

static void lexbor_cleanup_parser(void)
{
	if (++lexbor_urls % 500 == 0) {
		lexbor_mraw_clean(lexbor_parser->mraw);
	}

	lxb_url_parser_clean(lexbor_parser);
}

static void fill_errors(zval *errors)
{
	if (!errors || lexbor_parser->log == NULL) {
		return;
	}

	array_init(errors);

	lexbor_plog_entry_t *lxb_error;
	while ((lxb_error = lexbor_array_obj_pop(&lexbor_parser->log->list)) != NULL) {
		zval error;
		object_init_ex(&error, whatwg_error_ce);
		zend_update_property_string(whatwg_error_ce, Z_OBJ(error), "position", sizeof("position") - 1, (const char *) lxb_error->data);
		zend_update_property_long(whatwg_error_ce, Z_OBJ(error), "errorCode", sizeof("errorCode") - 1, lxb_error->id);

		add_next_index_zval(errors, &error);
	}
}

static zend_result lexbor_read_scheme(const uri_internal_t *internal_uri, zval *retval)
{
	lxb_url_t *lexbor_uri = (lxb_url_t *) internal_uri->uri;

	if (lexbor_uri->scheme.type != LXB_URL_SCHEMEL_TYPE__UNDEF) {
		ZVAL_STRINGL(retval, (const char *) lexbor_uri->scheme.name.data, lexbor_uri->scheme.name.length);
	} else {
		ZVAL_NULL(retval);
	}

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

static zend_result lexbor_read_user(const uri_internal_t *internal_uri, zval *retval)
{
	lxb_url_t *lexbor_uri = (lxb_url_t *) internal_uri->uri;

	if (lexbor_uri->username.length) {
		ZVAL_STRINGL(retval, (const char *) lexbor_uri->username.data, lexbor_uri->username.length);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result lexbor_write_user(uri_internal_t *internal_uri, zval *value, zval *errors)
{
	lxb_url_t *lexbor_uri = (lxb_url_t *) internal_uri->uri;
	lexbor_str_t str = {0};

	ZVAL_TO_LEXBOR_STR(value, str);

	lxb_status_t status = lxb_url_api_username_set(lexbor_uri, str.data, str.length);

	CHECK_WRITE_RESULT(status, lexbor_uri, str, errors);
}

static zend_result lexbor_read_password(const uri_internal_t *internal_uri, zval *retval)
{
	lxb_url_t *lexbor_uri = (lxb_url_t *) internal_uri->uri;

	if (lexbor_uri->password.length) {
		ZVAL_STRINGL(retval, (const char *) lexbor_uri->password.data, lexbor_uri->password.length);
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

static zend_result lexbor_read_host(const uri_internal_t *internal_uri, zval *retval)
{
	lxb_url_t *lexbor_uri = (lxb_url_t *) internal_uri->uri;

	if (lexbor_uri->host.type == LXB_URL_HOST_TYPE_IPV4) {
		char buffer[17] = {0};

		if (!inet_ntop(AF_INET, (const void *) &lexbor_uri->host.u.ipv4, buffer, sizeof(buffer))) {
			ZVAL_NULL(retval);
		}

		ZVAL_STRING(retval, buffer);
	} else if (lexbor_uri->host.type == LXB_URL_HOST_TYPE_IPV6) {
		char buffer[46] = {0};

		php_sprintf(buffer, "%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu", lexbor_uri->host.u.ipv6[0], lexbor_uri->host.u.ipv6[1],
			lexbor_uri->host.u.ipv6[2], lexbor_uri->host.u.ipv6[3], lexbor_uri->host.u.ipv6[4], lexbor_uri->host.u.ipv6[5],
			lexbor_uri->host.u.ipv6[6], lexbor_uri->host.u.ipv6[7]);

		ZVAL_STRING(retval, buffer);
	} else if (lexbor_uri->host.type != LXB_URL_HOST_TYPE_EMPTY && lexbor_uri->host.type != LXB_URL_HOST_TYPE__UNDEF) {
		ZVAL_STRINGL(retval, (const char *) lexbor_uri->host.u.domain.data, lexbor_uri->host.u.domain.length);
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

	lxb_status_t status = lxb_url_api_host_set(lexbor_uri, lexbor_parser, str.data, str.length);

	CHECK_WRITE_RESULT(status, lexbor_uri, str, errors);
}

static zend_result lexbor_read_port(const uri_internal_t *internal_uri, zval *retval)
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

static zend_result lexbor_read_path(const uri_internal_t *internal_uri, zval *retval)
{
	lxb_url_t *lexbor_uri = (lxb_url_t *) internal_uri->uri;

	if (lexbor_uri->path.opaque) {
		ZVAL_STRINGL(retval, (const char *) lexbor_uri->path.str.data, lexbor_uri->path.str.length);
	} else if (lexbor_uri->path.str.length > 1) {
		ZVAL_STRINGL(retval, (const char *) lexbor_uri->path.str.data + 1, lexbor_uri->path.str.length - 1);
	} else {
		ZVAL_NULL(retval);
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

static zend_result lexbor_read_query(const uri_internal_t *internal_uri, zval *retval)
{
	lxb_url_t *lexbor_uri = (lxb_url_t *) internal_uri->uri;

	if (lexbor_uri->query.length) {
		ZVAL_STRINGL(retval, (const char *) lexbor_uri->query.data, lexbor_uri->query.length);
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

static zend_result lexbor_read_fragment(const uri_internal_t *internal_uri, zval *retval)
{
	lxb_url_t *lexbor_uri = (lxb_url_t *) internal_uri->uri;

	if (lexbor_uri->fragment.length) {
		ZVAL_STRINGL(retval, (const char *) lexbor_uri->fragment.data, lexbor_uri->fragment.length);
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
	URI_REGISTER_PROPERTY_READ_WRITE_HANDLER(&lexbor_property_handlers, ZSTR_KNOWN(ZEND_STR_USER), lexbor_read_user, lexbor_write_user);
	URI_REGISTER_PROPERTY_READ_WRITE_HANDLER(&lexbor_property_handlers, ZSTR_KNOWN(ZEND_STR_PASSWORD), lexbor_read_password, lexbor_write_password);
	URI_REGISTER_PROPERTY_READ_WRITE_HANDLER(&lexbor_property_handlers, ZSTR_KNOWN(ZEND_STR_HOST), lexbor_read_host, lexbor_write_host);
	URI_REGISTER_PROPERTY_READ_WRITE_HANDLER(&lexbor_property_handlers, ZSTR_KNOWN(ZEND_STR_PORT), lexbor_read_port, lexbor_write_port);
	URI_REGISTER_PROPERTY_READ_WRITE_HANDLER(&lexbor_property_handlers, ZSTR_KNOWN(ZEND_STR_PATH), lexbor_read_path, lexbor_write_path);
	URI_REGISTER_PROPERTY_READ_WRITE_HANDLER(&lexbor_property_handlers, ZSTR_KNOWN(ZEND_STR_QUERY), lexbor_read_query, lexbor_write_query);
	URI_REGISTER_PROPERTY_READ_WRITE_HANDLER(&lexbor_property_handlers, ZSTR_KNOWN(ZEND_STR_FRAGMENT), lexbor_read_fragment, lexbor_write_fragment);

	return SUCCESS;
}

static void *lexbor_parse_uri(const zend_string *url_str, const zend_string *base_url_str, zval *errors)
{
	lexbor_cleanup_parser();

	lxb_url_t *url = NULL, *base_url = NULL;

	if (base_url_str) {
		if ((base_url = lxb_url_parse(lexbor_parser, NULL, (unsigned char *) ZSTR_VAL(base_url_str), ZSTR_LEN(base_url_str))) == NULL) {
			fill_errors(errors);
			return NULL;
		}

		base_url = lexbor_parser->url;
	}

	if ((url = lxb_url_parse(lexbor_parser, base_url, (unsigned char *) ZSTR_VAL(url_str), ZSTR_LEN(url_str))) == NULL) {
		fill_errors(errors);
		return NULL;
	}

	return url;
}

static zend_class_entry *lexbor_get_uri_ce(void)
{
	return whatwg_uri_ce;
}

static void *lexbor_clone_uri(void *uri)
{
	lxb_url_t *lexbor_uri = (lxb_url_t *) uri;

	return lxb_url_clone(lexbor_parser->mraw, lexbor_uri);
}

static lxb_status_t lexbor_serialize_callback(const lxb_char_t *data, size_t length, void *ctx)
{
	smart_str *uri_str = (smart_str *) ctx;

	smart_str_appendl(uri_str, (const char *) data, length);

	return LXB_STATUS_OK;
}

static zend_string *lexbor_uri_to_string(void *uri, bool exclude_fragment)
{
	lxb_url_t *lexbor_uri = (lxb_url_t *) uri;
	smart_str uri_str = {0};

	lxb_url_serialize(lexbor_uri, lexbor_serialize_callback, (void *) &uri_str, exclude_fragment);

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
