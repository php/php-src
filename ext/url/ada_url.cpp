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
}

#include <ada/ada.h>

static zend_class_entry *url_ce;
static zend_class_entry *url_component_ce;
static zend_class_entry *url_parser_ce;

static void parseUrlScheme(zend_string *url, ada::url_components components, zval *scheme) {
	zend_string_init(ZSTR_VAL(url), components.protocol_end - 1, false);
	ZVAL_STRINGL(scheme, ZSTR_VAL(url), components.protocol_end - 1);
}

static void parseUrlHost(zend_string *url, ada::url_components components, zval *host) {
	if (components.host_start < components.host_end) {
		ZVAL_STRINGL(host, ZSTR_VAL(url) + components.host_start, components.host_end - components.host_start);
	} else {
		ZVAL_EMPTY_STRING(host);
	}
}

static void parseUrlPort(ada::result<ada::url_aggregator> ada_url, ada::url_components components, zval *port) {
	if (ada_url->has_port()) {
		ZVAL_LONG(port, ada_url->has_port() ? components.port : 0);
	} else {
		ZVAL_NULL(port);
	}
}

static void parseUrlUser(zend_string *url, ada::url_components components, zval *user) {
	if (components.protocol_end + 2 < components.username_end) {
		ZVAL_STRINGL(user, ZSTR_VAL(url) + components.protocol_end + 2, components.username_end - components.protocol_end - 2);
	} else {
		ZVAL_NULL(user);
	}
}

static void parseUrlPassword(zend_string *url, ada::url_components components, zval *password) {
	if (components.host_start - components.username_end > 0) {
		ZVAL_STRINGL(password, ZSTR_VAL(url) + components.username_end + 1, components.host_start - components.username_end - 1);
	} else {
		ZVAL_NULL(password);
	}
}

static void parseUrlPath(zend_string *url, ada::result<ada::url_aggregator> ada_url, ada::url_components components, zval *path) {
	size_t ends_at;

	if (ada_url->has_search()) {
		ends_at = components.search_start;
	} else if (ada_url->has_hash()) {
		ends_at = components.hash_start;
	} else {
		ends_at = components.pathname_start;
	}

	ZVAL_STRINGL(path, ZSTR_VAL(url) + components.pathname_start, ends_at - components.pathname_start);
}

static void parseUrlQuery(zend_string *url, ada::result<ada::url_aggregator> ada_url, ada::url_components components, zval *query) {
	if (!ada_url->has_search()) {
		ZVAL_NULL(query);
	} else {
		size_t ends_at = ada_url->has_hash() ? components.hash_start : ZSTR_LEN(url);
		if (ends_at - components.search_start <= 1) {
			ZVAL_NULL(query);
		} else {
			ZVAL_STRINGL(query, ZSTR_VAL(url) + components.search_start, ends_at - components.search_start);
		}
	}
}

static void parseUrlFragment(zend_string *url, ada::result<ada::url_aggregator> ada_url, ada::url_components components, zval *fragment) {
	if (ada_url->has_hash() && (ZSTR_LEN(url) - components.hash_start > 1)) {
		ZVAL_STRINGL(fragment, ZSTR_VAL(url) + components.hash_start + 1, ZSTR_LEN(url) - components.hash_start - 1);
	} else {
		ZVAL_NULL(fragment);
	}
}

void parseUrlComponentsArray(zend_string *url, zval *return_value)
{
	zval tmp;

	ada::result<ada::url_aggregator> ada_url = ada::parse<ada::url_aggregator>(ZSTR_VAL(url));
	if (!ada_url) {
		RETURN_NULL();
	}

	ada::url_components ada_components = ada_url->get_components();

	array_init(return_value);

	parseUrlScheme(url, ada_components, &tmp);
	if (Z_STRLEN(tmp) > 0) {
		zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_SCHEME), &tmp);
	}

	parseUrlHost(url, ada_components, &tmp);
	if (Z_STRLEN(tmp) > 0) {
		zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_HOST), &tmp);
	}

	parseUrlPort(ada_url, ada_components, &tmp);
	if (Z_TYPE(tmp) == IS_LONG) {
		zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_PORT), &tmp);
	}

	parseUrlUser(url, ada_components, &tmp);
	if (Z_TYPE(tmp) == IS_STRING) {
		zend_hash_add(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_USER), &tmp);
	}

	parseUrlPassword(url, ada_components, &tmp);
	if (Z_TYPE(tmp) == IS_STRING) {
		zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_PASS), &tmp);
	}

	parseUrlPath(url, ada_url, ada_components, &tmp);
	if (Z_TYPE(tmp) == IS_STRING) {
		zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_PATH), &tmp);
	}

	parseUrlQuery(url, ada_url, ada_components, &tmp);
	if (Z_TYPE(tmp) == IS_STRING) {
		zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_QUERY), &tmp);
	}

	parseUrlFragment(url, ada_url, ada_components, &tmp);
	if (Z_TYPE(tmp) == IS_STRING) {
		zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_FRAGMENT), &tmp);
	}
}

void parseUrlComponentsObject(zend_string *url, zval *return_value)
{
	zval tmp;

	ada::result<ada::url_aggregator> ada_url = ada::parse<ada::url_aggregator>(ZSTR_VAL(url));
	if (!ada_url) {
		RETURN_NULL();
	}

	ada::url_components ada_components = ada_url->get_components();

	object_init_ex(return_value, url_ce);

	parseUrlScheme(url, ada_components, &tmp);
	zend_update_property(
		url_ce, Z_OBJ_P(return_value),
		ZSTR_VAL(ZSTR_KNOWN(ZEND_STR_SCHEME)), ZSTR_LEN(ZSTR_KNOWN(ZEND_STR_SCHEME)),
		&tmp
	);

	parseUrlHost(url, ada_components, &tmp);
	zend_update_property(
		url_ce, Z_OBJ_P(return_value),
		ZSTR_VAL(ZSTR_KNOWN(ZEND_STR_HOST)), ZSTR_LEN(ZSTR_KNOWN(ZEND_STR_HOST)),
		&tmp
	);

	parseUrlPort(ada_url, ada_components, &tmp);
	zend_update_property(
		url_ce, Z_OBJ_P(return_value),
		ZSTR_VAL(ZSTR_KNOWN(ZEND_STR_PORT)), ZSTR_LEN(ZSTR_KNOWN(ZEND_STR_PORT)),
		&tmp
	);

	parseUrlUser(url, ada_components, &tmp);
	zend_update_property(
		url_ce, Z_OBJ_P(return_value),
		ZSTR_VAL(ZSTR_KNOWN(ZEND_STR_USER)), ZSTR_LEN(ZSTR_KNOWN(ZEND_STR_USER)),
		&tmp
	);

	parseUrlPassword(url, ada_components, &tmp);
	zend_update_property(
		url_ce, Z_OBJ_P(return_value),
		ZSTR_VAL(ZSTR_KNOWN(ZEND_STR_PASSWORD)), ZSTR_LEN(ZSTR_KNOWN(ZEND_STR_PASSWORD)),
		&tmp
	);

	parseUrlPath(url, ada_url, ada_components, &tmp);
	zend_update_property(
		url_ce, Z_OBJ_P(return_value),
		ZSTR_VAL(ZSTR_KNOWN(ZEND_STR_PATH)), ZSTR_LEN(ZSTR_KNOWN(ZEND_STR_PATH)),
		&tmp
	);

	parseUrlQuery(url, ada_url, ada_components, &tmp);
	zend_update_property(
		url_ce, Z_OBJ_P(return_value),
		ZSTR_VAL(ZSTR_KNOWN(ZEND_STR_QUERY)), ZSTR_LEN(ZSTR_KNOWN(ZEND_STR_QUERY)),
		&tmp
	);

	parseUrlFragment(url, ada_url, ada_components, &tmp);
	zend_update_property(
		url_ce, Z_OBJ_P(return_value),
		ZSTR_VAL(ZSTR_KNOWN(ZEND_STR_FRAGMENT)), ZSTR_LEN(ZSTR_KNOWN(ZEND_STR_FRAGMENT)),
		&tmp
	);
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
