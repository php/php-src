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

void parseUrlComponentsArray(zend_string *url, zval *return_value)
{
	std::string_view protocol, hostname, username, password, port, path, query, fragment;

	ada::result<ada::url_aggregator> ada_url = ada::parse<ada::url_aggregator>(ZSTR_VAL(url));
	if (!ada_url) {
		RETURN_NULL();
	}

	protocol = ada_url->get_protocol();
	hostname = ada_url->get_hostname();
	port = ada_url->get_port();
	username = ada_url->get_username();
	password = ada_url->get_password();
	path = ada_url->get_protocol();
	query = ada_url->get_search();
	fragment = ada_url->get_hash();

	array_init(return_value);
	add_assoc_stringl(return_value,"scheme",protocol.data(), protocol.length()-1);
	add_assoc_stringl(return_value,"host", hostname.data(), hostname.length());
	add_assoc_stringl(return_value, "port", port.data(), port.length());
	add_assoc_stringl(return_value, "user", username.data(), username.length());
	add_assoc_stringl(return_value, "password", password.data(), password.length());
	add_assoc_stringl(return_value, "path", path.data(), path.length());
	add_assoc_stringl(return_value, "query", query.data(), query.length());
	add_assoc_stringl(return_value, "fragment", fragment.data(), fragment.length());
}

void parseUrlComponentsObject(zend_string *url, zval *return_value)
{
	std::string_view protocol, hostname, username, password, port, path, query, fragment;

	ada::result<ada::url_aggregator> ada_url = ada::parse<ada::url_aggregator>(ZSTR_VAL(url));
	if (!ada_url) {
		RETURN_NULL();
	}

	protocol = ada_url->get_protocol();
	hostname = ada_url->get_hostname();
	port = ada_url->get_port();
	username = ada_url->get_username();
	password = ada_url->get_password();
	path = ada_url->get_protocol();
	query = ada_url->get_search();
	fragment = ada_url->get_hash();

	object_init_ex(return_value, url_ce);
	zend_update_property_stringl(
		url_ce, Z_OBJ_P(return_value),
		"scheme", sizeof("scheme")-1,
		protocol.data(), protocol.length()-1
	);
	zend_update_property_stringl(
		url_ce, Z_OBJ_P(return_value),
		"host", sizeof("host")-1,
		hostname.data(), hostname.length()
	);
	zend_update_property_stringl(
		url_ce, Z_OBJ_P(return_value),
		"port", sizeof("port")-1,
		port.data(), port.length()
	);
	zend_update_property_stringl(
		url_ce, Z_OBJ_P(return_value),
		"user", sizeof("user")-1,
		username.data(), username.length()
	);
	zend_update_property_stringl(
		url_ce, Z_OBJ_P(return_value),
		"password", sizeof("password")-1,
		password.data(), password.length()
	);
	zend_update_property_stringl(
		url_ce, Z_OBJ_P(return_value),
		"path", sizeof("path")-1,
		path.data(), path.length()
	);
	zend_update_property_stringl(
		url_ce, Z_OBJ_P(return_value),
		"query", sizeof("query")-1,
		query.data(), query.length()
	);
	zend_update_property_stringl(
		url_ce, Z_OBJ_P(return_value),
		"fragment", sizeof("fragment")-1,
		fragment.data(), fragment.length()
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
