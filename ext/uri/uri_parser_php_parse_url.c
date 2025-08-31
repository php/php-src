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
   |          Tim Düsterhus <timwolla@php.net>                            |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "uri_parser_php_parse_url.h"
#include "php_uri_common.h"
#include "Zend/zend_exceptions.h"
#include "ext/standard/url.h"

static zend_string *decode_component(zend_string *in, uri_component_read_mode_t read_mode)
{
	switch (read_mode) {
	case URI_COMPONENT_READ_RAW:
		return zend_string_copy(in);
	case URI_COMPONENT_READ_NORMALIZED_ASCII:
	case URI_COMPONENT_READ_NORMALIZED_UNICODE: {
		zend_string *out = zend_string_alloc(ZSTR_LEN(in), false);

		ZSTR_LEN(out) = php_raw_url_decode_ex(ZSTR_VAL(out), ZSTR_VAL(in), ZSTR_LEN(in));

		return out;
	}
	EMPTY_SWITCH_DEFAULT_CASE();
	}
}

static zend_result uri_parser_php_parse_url_scheme_read(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	const php_url *parse_url_uri = internal_uri->uri;

	if (parse_url_uri->scheme) {
		ZVAL_STR(retval, decode_component(parse_url_uri->scheme, read_mode));
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result uri_parser_php_parse_url_username_read(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	const php_url *parse_url_uri = internal_uri->uri;

	if (parse_url_uri->user) {
		ZVAL_STR(retval, decode_component(parse_url_uri->user, read_mode));
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result uri_parser_php_parse_url_password_read(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	const php_url *parse_url_uri = internal_uri->uri;

	if (parse_url_uri->pass) {
		ZVAL_STR(retval, decode_component(parse_url_uri->pass, read_mode));
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result uri_parser_php_parse_url_host_read(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	const php_url *parse_url_uri = internal_uri->uri;

	if (parse_url_uri->host) {
		ZVAL_STR(retval, decode_component(parse_url_uri->host, read_mode));
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result uri_parser_php_parse_url_port_read(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	const php_url *parse_url_uri = internal_uri->uri;

	if (parse_url_uri->port) {
		ZVAL_LONG(retval, parse_url_uri->port);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result uri_parser_php_parse_url_path_read(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	const php_url *parse_url_uri = internal_uri->uri;

	if (parse_url_uri->path) {
		ZVAL_STR(retval, decode_component(parse_url_uri->path, read_mode));
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result uri_parser_php_parse_url_query_read(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	const php_url *parse_url_uri = internal_uri->uri;

	if (parse_url_uri->query) {
		ZVAL_STR(retval, decode_component(parse_url_uri->query, read_mode));
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result uri_parser_php_parse_url_fragment_read(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	const php_url *parse_url_uri = internal_uri->uri;

	if (parse_url_uri->fragment) {
		ZVAL_STR(retval, decode_component(parse_url_uri->fragment, read_mode));
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static void *uri_parser_php_parse_url_parse(const char *uri_str, size_t uri_str_len, const void *base_url, zval *errors, bool silent)
{
	bool has_port;

	php_url *url = php_url_parse_ex2(uri_str, uri_str_len, &has_port);
	if (url == NULL && !silent) {
		zend_throw_exception(uri_invalid_uri_exception_ce, "The specified URI is malformed", 0);
	}

	return url;
}

static void uri_parser_php_parse_url_free(void *uri)
{
	php_url *parse_url_uri = uri;

	php_url_free(parse_url_uri);
}

const uri_parser_t php_uri_parser_php_parse_url = {
	.name = PHP_URI_PARSER_PHP_PARSE_URL,
	.parse_uri = uri_parser_php_parse_url_parse,
	.clone_uri = NULL,
	.uri_to_string = NULL,
	.free_uri = uri_parser_php_parse_url_free,
	{
		.scheme = {.read_func = uri_parser_php_parse_url_scheme_read, .write_func = NULL},
		.username = {.read_func = uri_parser_php_parse_url_username_read, .write_func = NULL},
		.password = {.read_func = uri_parser_php_parse_url_password_read, .write_func = NULL},
		.host = {.read_func = uri_parser_php_parse_url_host_read, .write_func = NULL},
		.port = {.read_func = uri_parser_php_parse_url_port_read, .write_func = NULL},
		.path = {.read_func = uri_parser_php_parse_url_path_read, .write_func = NULL},
		.query = {.read_func = uri_parser_php_parse_url_query_read, .write_func = NULL},
		.fragment = {.read_func = uri_parser_php_parse_url_fragment_read, .write_func = NULL},
	}
};
