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

static void parse_url_decode_component(zval *zv, uri_component_read_mode_t read_mode)
{
	if (Z_TYPE_P(zv) != IS_STRING) {
		return;
	}

	if (read_mode == URI_COMPONENT_READ_RAW) {
		return;
	}

	php_raw_url_decode(Z_STRVAL_P(zv), Z_STRLEN_P(zv));
}

static zend_result parse_url_read_scheme(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	php_url *parse_url_uri = internal_uri->uri;

	if (parse_url_uri->scheme) {
		ZVAL_STR_COPY(retval, parse_url_uri->scheme);
		parse_url_decode_component(retval, read_mode);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result parse_url_read_username(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	php_url *parse_url_uri = internal_uri->uri;

	if (parse_url_uri->user) {
		ZVAL_STR_COPY(retval, parse_url_uri->user);
		parse_url_decode_component(retval, read_mode);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result parse_url_read_password(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	php_url *parse_url_uri = internal_uri->uri;

	if (parse_url_uri->pass) {
		ZVAL_STR_COPY(retval, parse_url_uri->pass);
		parse_url_decode_component(retval, read_mode);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result parse_url_read_host(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	php_url *parse_url_uri = internal_uri->uri;

	if (parse_url_uri->host) {
		ZVAL_STR_COPY(retval, parse_url_uri->host);
		parse_url_decode_component(retval, read_mode);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result parse_url_read_port(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	php_url *parse_url_uri = internal_uri->uri;

	if (parse_url_uri->port) {
		ZVAL_LONG(retval, parse_url_uri->port);
		parse_url_decode_component(retval, read_mode);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result parse_url_read_path(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	php_url *parse_url_uri = internal_uri->uri;

	if (parse_url_uri->path) {
		ZVAL_STR_COPY(retval, parse_url_uri->path);
		parse_url_decode_component(retval, read_mode);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result parse_url_read_query(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	php_url *parse_url_uri = internal_uri->uri;

	if (parse_url_uri->query) {
		ZVAL_STR_COPY(retval, parse_url_uri->query);
		parse_url_decode_component(retval, read_mode);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static zend_result parse_url_read_fragment(const uri_internal_t *internal_uri, uri_component_read_mode_t read_mode, zval *retval)
{
	php_url *parse_url_uri = internal_uri->uri;

	if (parse_url_uri->fragment) {
		ZVAL_STR_COPY(retval, parse_url_uri->fragment);
		parse_url_decode_component(retval, read_mode);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

static void throw_invalid_uri_exception(void)
{
	zend_throw_exception(uri_invalid_uri_exception_ce, "The specified URI is malformed", 0);
}

static void *parse_url_parse_uri(const char *uri_str, size_t uri_str_len, const void *base_url, zval *errors, bool silent)
{
	bool has_port;

	php_url *url = php_url_parse_ex2(uri_str, uri_str_len, &has_port);
	if (url == NULL && !silent) {
		throw_invalid_uri_exception();
	}

	return url;
}

static void parse_url_free_uri(void *uri)
{
	php_url *parse_url_uri = (php_url *) uri;

	php_url_free(parse_url_uri);
}

const uri_parser_t parse_url_uri_parser = {
	.name = URI_PARSER_PHP,
	.parse_uri = parse_url_parse_uri,
	.clone_uri = NULL,
	.uri_to_string = NULL,
	.free_uri = parse_url_free_uri,
	{
		.scheme = {.read_func = parse_url_read_scheme, .write_func = NULL},
		.username = {.read_func = parse_url_read_username, .write_func = NULL},
		.password = {.read_func = parse_url_read_password, .write_func = NULL},
		.host = {.read_func = parse_url_read_host, .write_func = NULL},
		.port = {.read_func = parse_url_read_port, .write_func = NULL},
		.path = {.read_func = parse_url_read_path, .write_func = NULL},
		.query = {.read_func = parse_url_read_query, .write_func = NULL},
		.fragment = {.read_func = parse_url_read_fragment, .write_func = NULL},
	}
};
