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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <cstdlib>
#include <string.h>
#include <cctype>
#include <sys/types.h>

extern "C" {
	#include "php.h"
	#include "ada_url.h"
	#include "Zend/zend_enum.h"
	#include "Zend/zend_interfaces.h"
	#include "url_arginfo.h"
	#include "ext/standard/url.h"
}

#include <ada/ada.h>

static zend_class_entry *url_ce;
static zend_class_entry *url_component_ce;
static zend_class_entry *url_parser_ce;

static php_url *parseUrlComponents(zend_string *url) {
	ada::result<ada::url> ada_url = ada::parse<ada::url>(ZSTR_VAL(url));
	if (!ada_url) {
		return NULL;
	}

	ada::url_components components = ada_url->get_components();

	php_url *result = (php_url*) ecalloc(1, sizeof(php_url));

	result->scheme = zend_string_init(ZSTR_VAL(url), components.protocol_end - 1, false);

	if (components.host_start < components.host_end) {
		result->host = zend_string_init(ZSTR_VAL(url) + components.host_start, components.host_end - components.host_start + 1, false);
	} else {
		result->host = NULL;
	}

	result->port = ada_url->has_port() ? components.port : 0;

	if (components.protocol_end + 2 < components.username_end) {
		result->user = zend_string_init(ZSTR_VAL(url) + components.protocol_end + 2, components.username_end - components.protocol_end - 2, false);
	} else {
		result->user = NULL;
	}

	if (components.host_start - components.username_end > 0) {
		result->pass = zend_string_init(ZSTR_VAL(url) + components.username_end + 1, components.host_start - components.username_end - 1, false);
	} else {
		result->pass = NULL;
	}

	size_t ends_at;
	if (ada_url->has_search()) {
		ends_at = components.search_start;
	} else if (ada_url->has_hash()) {
		ends_at = components.hash_start;
	} else {
		ends_at = components.pathname_start;
	}
	result->path = zend_string_init(ZSTR_VAL(url) + components.pathname_start, ends_at - components.pathname_start, false);

	if (!ada_url->has_search()) {
		result->query = NULL;
	} else {
		ends_at = ada_url->has_hash() ? components.hash_start : ZSTR_LEN(url);
		if (ends_at - components.search_start <= 1) {
			result->query = NULL;
		} else {
			result->query = zend_string_init(ZSTR_VAL(url) + components.search_start, ends_at - components.search_start, false);
		}
	}

	if (ada_url->has_hash() && (ZSTR_LEN(url) - components.hash_start > 1)) {
		result->fragment = zend_string_init(ZSTR_VAL(url) + components.hash_start + 1, ZSTR_LEN(url) - components.hash_start - 1, false);
	} else {
		result->fragment = NULL;
	}

	return result;
}

void parseUrlComponentsArray(zend_string *url, zval *return_value)
{
	php_url *components;
	zval tmp;

	components = parseUrlComponents(url);
	if (!components) {
		RETURN_NULL();
	}

	array_init(return_value);

	if (components->scheme != NULL) {
		ZVAL_STR_COPY(&tmp, components->scheme);
		zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_SCHEME), &tmp);
	}

	if (components->host != NULL) {
		ZVAL_STR_COPY(&tmp, components->host);
		zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_HOST), &tmp);
	}

	if (components->port > 0) {
		ZVAL_LONG(&tmp, components->port);
		zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_PORT), &tmp);
	}

	if (components->user != NULL) {
		ZVAL_STR_COPY(&tmp, components->user);
		zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_USER), &tmp);
	}

	if (components->pass != NULL) {
		ZVAL_STR_COPY(&tmp, components->pass);
		zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_PASS), &tmp);
	}

	if (ZSTR_LEN(components->path)) {
		ZVAL_STR_COPY(&tmp, components->path);
		zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_PATH), &tmp);
	}

	if (components->query != NULL) {
		ZVAL_STR_COPY(&tmp, components->query);
		zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_QUERY), &tmp);
	}

	if (components->fragment != NULL) {
		ZVAL_STR_COPY(&tmp, components->fragment);
		zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_FRAGMENT), &tmp);
	}

	php_url_free(components);
}

void parseUrlComponentsObject(zend_string *url, zval *return_value)
{
	php_url *components;

	components = parseUrlComponents(url);
	if (!components) {
		RETURN_NULL();
	}

	object_init_ex(return_value, url_ce);
	zend_update_property_str(
		url_ce, Z_OBJ_P(return_value),
		ZSTR_VAL(ZSTR_KNOWN(ZEND_STR_SCHEME)), ZSTR_LEN(ZSTR_KNOWN(ZEND_STR_SCHEME)),
		components->scheme ? components->scheme : ZSTR_EMPTY_ALLOC()
	);
	zend_update_property_str(
		url_ce, Z_OBJ_P(return_value),
		ZSTR_VAL(ZSTR_KNOWN(ZEND_STR_HOST)), ZSTR_LEN(ZSTR_KNOWN(ZEND_STR_HOST)),
		components->host ? components->host : ZSTR_EMPTY_ALLOC()
	);
	if (components->port > 0) {
		zend_update_property_long(
			url_ce, Z_OBJ_P(return_value),
			ZSTR_VAL(ZSTR_KNOWN(ZEND_STR_PORT)), ZSTR_LEN(ZSTR_KNOWN(ZEND_STR_PORT)),
			components->port
		);
	} else {
		zend_update_property_null(
			url_ce, Z_OBJ_P(return_value),
			ZSTR_VAL(ZSTR_KNOWN(ZEND_STR_PORT)), ZSTR_LEN(ZSTR_KNOWN(ZEND_STR_PORT))
		);
	}

	zend_update_property_str(
		url_ce, Z_OBJ_P(return_value),
		ZSTR_VAL(ZSTR_KNOWN(ZEND_STR_USER)), ZSTR_LEN(ZSTR_KNOWN(ZEND_STR_USER)),
		components->user ? components->user : ZSTR_EMPTY_ALLOC()
	);

	zend_update_property_str(
		url_ce, Z_OBJ_P(return_value),
		ZSTR_VAL(ZSTR_KNOWN(ZEND_STR_PASSWORD)), ZSTR_LEN(ZSTR_KNOWN(ZEND_STR_PASSWORD)),
		components->pass ? components->pass : ZSTR_EMPTY_ALLOC()
	);

	zend_update_property_str(
		url_ce, Z_OBJ_P(return_value),
		ZSTR_VAL(ZSTR_KNOWN(ZEND_STR_PATH)), ZSTR_LEN(ZSTR_KNOWN(ZEND_STR_PATH)),
		components->path ? components->path : ZSTR_EMPTY_ALLOC()
	);

	zend_update_property_str(
		url_ce, Z_OBJ_P(return_value),
		ZSTR_VAL(ZSTR_KNOWN(ZEND_STR_QUERY)), ZSTR_LEN(ZSTR_KNOWN(ZEND_STR_QUERY)),
		components->query ? components->query : ZSTR_EMPTY_ALLOC()
	);

	zend_update_property_str(
		url_ce, Z_OBJ_P(return_value),
		ZSTR_VAL(ZSTR_KNOWN(ZEND_STR_FRAGMENT)), ZSTR_LEN(ZSTR_KNOWN(ZEND_STR_FRAGMENT)),
		components->fragment ? components->fragment : ZSTR_EMPTY_ALLOC()
	);

	php_url_free(components);
}

PHP_METHOD(Url_Url, __construct)
{
	zend_string *scheme = NULL, *host = NULL, *port = NULL, *user = NULL, *password = NULL, *path = NULL, *query = NULL, *fragment = NULL;

	ZEND_PARSE_PARAMETERS_START(8, 8)
		Z_PARAM_STR_OR_NULL(scheme)
		Z_PARAM_STR_OR_NULL(host)
		Z_PARAM_STR_OR_NULL(port)
		Z_PARAM_STR_OR_NULL(user)
		Z_PARAM_STR(password)
		Z_PARAM_STR(path)
		Z_PARAM_STR(query)
		Z_PARAM_STR(fragment)
	ZEND_PARSE_PARAMETERS_END();

	zend_update_property_str(
		url_ce,
		Z_OBJ_P(getThis()),
		"scheme",
		sizeof("scheme")-1,
		scheme
	);
}

PHP_METHOD(Url_Url, __toString)
{
	ZEND_PARSE_PARAMETERS_NONE();
}

PHP_METHOD(Url_UrlParser, parseUrlComponent)
{
	zend_string *url;
	zend_object *component;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR(url)
		Z_PARAM_OBJ_OF_CLASS(component, url_component_ce)
	ZEND_PARSE_PARAMETERS_END();
}

PHP_METHOD(Url_UrlParser, parseUrl)
{
	zend_string *url;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_PATH_STR(url)
	ZEND_PARSE_PARAMETERS_END();

	parseUrlComponentsObject(url, return_value);
}

PHP_METHOD(Url_UrlParser, parseUrlArray)
{
	zend_string *url;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_PATH_STR(url)
	ZEND_PARSE_PARAMETERS_END();

	parseUrlComponentsArray(url, return_value);
}

void url_register_symbols(void)
{
	url_ce = register_class_Url_Url(zend_ce_stringable);
	url_component_ce = register_class_Url_UrlComponent();
	url_parser_ce = register_class_Url_UrlParser();
}
