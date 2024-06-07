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

#include "php.h"
#include "Zend/zend_enum.h"
#include "Zend/zend_interfaces.h"

#include <lexbor/url/url.h>
#include "php_url.h"
#include "php_url_arginfo.h"

static zend_class_entry *url_ce;
static zend_class_entry *url_component_ce;
static zend_class_entry *url_parser_ce;

ZEND_BEGIN_MODULE_GLOBALS(url)
	lxb_url_parser_t *parser;
	zend_long urls;
ZEND_END_MODULE_GLOBALS(url)

ZEND_DECLARE_MODULE_GLOBALS(url)

#define URL_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(url, v)

static lxb_url_parser_t *init_parser(void)
{
	lexbor_mraw_t *mraw = lexbor_mraw_create();
	lxb_status_t status = lexbor_mraw_init(mraw, 4096 * 2);
	if (status != LXB_STATUS_OK) {
		lexbor_mraw_destroy(mraw, true);
		return NULL;
	}

	lxb_url_parser_t *parser = lxb_url_parser_create();
	status = lxb_url_parser_init(parser, mraw);
	if (status != LXB_STATUS_OK) {
		lxb_url_parser_destroy(parser, true);
		return NULL;
	}

	return parser;
}

static void destroy_parser(lxb_url_parser_t *parser)
{
	lxb_url_parser_memory_destroy(parser);
	lxb_url_parser_destroy(parser, true);
}

#define PARSE_URL(url) \
	do { \
		if (ZSTR_LEN(url) == 0 || (lxb_url = lxb_url_parse(URL_G(parser), NULL, (unsigned char *) ZSTR_VAL(url), ZSTR_LEN(url))) == NULL) { \
			zend_argument_value_error(1, "is not a valid URL"); \
			RETURN_THROWS(); \
		} \
	} while(0)

static void url_scheme_to_zval(lxb_url_t *lxb_url, zval *scheme)
{
	if (lxb_url->scheme.type != LXB_URL_SCHEMEL_TYPE__UNDEF &&
		lxb_url->scheme.type != LXB_URL_SCHEMEL_TYPE__UNKNOWN &&
		lxb_url->scheme.type != LXB_URL_SCHEMEL_TYPE__LAST_ENTRY
	) {
		ZVAL_STRINGL(scheme, (const char *) lxb_url->scheme.name.data, lxb_url->scheme.name.length);
	} else {
		ZVAL_NULL(scheme);
	}
}

static void url_host_to_zval(lxb_url_t *lxb_url, zval *host)
{
	if (lxb_url->host.type != LXB_URL_HOST_TYPE_EMPTY) {
		ZVAL_STRINGL(host, (const char *) lxb_url->host.u.domain.data, lxb_url->host.u.domain.length);
	} else {
		ZVAL_NULL(host);
	}
}

static void url_port_to_zval(lxb_url_t *lxb_url, zval *port)
{
	if (lxb_url->has_port) {
		ZVAL_LONG(port, lxb_url->port);
	} else {
		ZVAL_NULL(port);
	}
}

static void url_user_to_zval(lxb_url_t *lxb_url, zval *user)
{
	if (lxb_url->username.length) {
		ZVAL_STRINGL(user, (const char *) lxb_url->username.data, lxb_url->username.length);
	} else {
		ZVAL_NULL(user);
	}
}

static void url_password_to_zval(lxb_url_t *lxb_url, zval *password)
{
	if (lxb_url->password.length) {
		ZVAL_STRINGL(password, (const char *) lxb_url->password.data, lxb_url->password.length);
	} else {
		ZVAL_NULL(password);
	}
}

static void url_path_to_zval(lxb_url_t *lxb_url, zval *path)
{
	if (lxb_url->path.length && lxb_url->path.list[0]->length) {
		ZVAL_STRINGL(path, (const char *) lxb_url->path.list[0]->data, lxb_url->path.list[0]->length);
	} else {
		ZVAL_NULL(path);
	}
}

static void url_query_string_to_zval(lxb_url_t *lxb_url, zval *query)
{
	if (lxb_url->query.length) {
		ZVAL_STRINGL(query, (const char *) lxb_url->query.data, lxb_url->query.length);
	} else {
		ZVAL_NULL(query);
	}
}

static void url_fragment_to_zval(lxb_url_t *lxb_url, zval *fragment)
{
	if (lxb_url->fragment.length) {
		ZVAL_STRINGL(fragment, (const char *) lxb_url->fragment.data, lxb_url->fragment.length);
	} else {
		ZVAL_NULL(fragment);
	}
}

static void cleanup_parser(void)
{
	if (++URL_G(urls) % 500 == 0) {
		lexbor_mraw_clean(URL_G(parser)->mraw);
	}

	lxb_url_parser_clean(URL_G(parser));
}

void parse_url_to_array(zend_string *url, zval *return_value)
{
	zval tmp;
	lxb_url_t *lxb_url = NULL;

	PARSE_URL(url);

	array_init(return_value);

	url_scheme_to_zval(lxb_url, &tmp);
	if (Z_TYPE(tmp) == IS_STRING && Z_STRLEN(tmp) > 0) {
		zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_SCHEME), &tmp);
	}

	url_host_to_zval(lxb_url, &tmp);
	if (Z_TYPE(tmp) == IS_STRING && Z_STRLEN(tmp) > 0) {
		zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_HOST), &tmp);
	}

	url_port_to_zval(lxb_url, &tmp);
	if (Z_TYPE(tmp) == IS_LONG) {
		zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_PORT), &tmp);
	}

	url_user_to_zval(lxb_url, &tmp);
	if (Z_TYPE(tmp) == IS_STRING) {
		zend_hash_add(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_USER), &tmp);
	}

	url_password_to_zval(lxb_url, &tmp);
	if (Z_TYPE(tmp) == IS_STRING) {
		zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_PASSWORD), &tmp);
	}

	url_path_to_zval(lxb_url, &tmp);
	if (Z_TYPE(tmp) == IS_STRING) {
		zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_PATH), &tmp);
	}

	url_query_string_to_zval(lxb_url, &tmp);
	if (Z_TYPE(tmp) == IS_STRING) {
		zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_QUERY), &tmp);
	}

	url_fragment_to_zval(lxb_url, &tmp);
	if (Z_TYPE(tmp) == IS_STRING) {
		zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_FRAGMENT), &tmp);
	}

	cleanup_parser();
}

void parse_url_to_object(zend_string *url, zval *return_value)
{
	zval tmp;
	lxb_url_t *lxb_url = NULL;

	PARSE_URL(url);

	object_init_ex(return_value, url_ce);

	url_scheme_to_zval(lxb_url, &tmp);
	zend_update_property_ex(url_ce, Z_OBJ_P(return_value), ZSTR_KNOWN(ZEND_STR_SCHEME), &tmp);
	zval_ptr_dtor(&tmp);

	url_host_to_zval(lxb_url, &tmp);
	zend_update_property_ex(url_ce, Z_OBJ_P(return_value), ZSTR_KNOWN(ZEND_STR_HOST), &tmp);
	zval_ptr_dtor(&tmp);

	url_port_to_zval(lxb_url, &tmp);
	zend_update_property_ex(url_ce, Z_OBJ_P(return_value), ZSTR_KNOWN(ZEND_STR_PORT), &tmp);
	zval_ptr_dtor(&tmp);

	url_user_to_zval(lxb_url, &tmp);
	zend_update_property_ex(url_ce, Z_OBJ_P(return_value), ZSTR_KNOWN(ZEND_STR_USER), &tmp);
	zval_ptr_dtor(&tmp);

	url_password_to_zval(lxb_url, &tmp);
	zend_update_property_ex(url_ce, Z_OBJ_P(return_value), ZSTR_KNOWN(ZEND_STR_PASSWORD), &tmp);
	zval_ptr_dtor(&tmp);

	url_path_to_zval(lxb_url, &tmp);
	zend_update_property_ex(url_ce, Z_OBJ_P(return_value), ZSTR_KNOWN(ZEND_STR_PATH), &tmp);
	zval_ptr_dtor(&tmp);

	url_query_string_to_zval(lxb_url, &tmp);
	zend_update_property_ex(url_ce, Z_OBJ_P(return_value), ZSTR_KNOWN(ZEND_STR_QUERY), &tmp);
	zval_ptr_dtor(&tmp);

	url_fragment_to_zval(lxb_url, &tmp);
	zend_update_property_ex(url_ce, Z_OBJ_P(return_value), ZSTR_KNOWN(ZEND_STR_FRAGMENT), &tmp);
	zval_ptr_dtor(&tmp);

	cleanup_parser();
}

void url_update_str_property(const zend_execute_data *execute_data, zend_string *name, zend_string *value)
{
	zval zv;

	if (value) {
		ZVAL_STR(&zv, value);
	} else {
		ZVAL_NULL(&zv);
	}

	zend_update_property_ex(url_ce, Z_OBJ_P(getThis()), name, &zv);
}

void url_update_long_property(const zend_execute_data *execute_data, zend_string *name, zend_long value, bool value_is_null)
{
	zval zv;

	if (!value_is_null) {
		ZVAL_LONG(&zv, value);
	} else {
		ZVAL_NULL(&zv);
	}

	zend_update_property_ex(url_ce, Z_OBJ_P(getThis()), name, &zv);
}

#define UPDATE_STR_PROPERTY(e, n, v) \
	do { \
		url_update_str_property(e, n, v); \
		if (EG(exception)) { \
			RETURN_THROWS(); \
		} \
	} while(0)

#define UPDATE_LONG_PROPERTY(e, n, v, vn) \
	do { \
		url_update_long_property(e, n, v, vn); \
		if (EG(exception)) { \
			RETURN_THROWS(); \
		} \
	} while(0)

PHP_METHOD(Url_Url, __construct)
{
	zend_string *scheme = NULL, *host = NULL, *user = NULL, *password = NULL, *path = NULL, *query = NULL, *fragment = NULL;
	zend_long port;
	bool port_is_null = true;

	ZEND_PARSE_PARAMETERS_START(8, 8)
		Z_PARAM_STR_OR_NULL(scheme)
		Z_PARAM_STR_OR_NULL(host)
		Z_PARAM_LONG_OR_NULL(port, port_is_null)
		Z_PARAM_STR_OR_NULL(user)
		Z_PARAM_STR_OR_NULL(password)
		Z_PARAM_STR_OR_NULL(path)
		Z_PARAM_STR_OR_NULL(query)
		Z_PARAM_STR_OR_NULL(fragment)
	ZEND_PARSE_PARAMETERS_END();

	UPDATE_STR_PROPERTY(execute_data, ZSTR_KNOWN(ZEND_STR_SCHEME), scheme);
	UPDATE_STR_PROPERTY(execute_data, ZSTR_KNOWN(ZEND_STR_HOST), host);
	UPDATE_LONG_PROPERTY(execute_data, ZSTR_KNOWN(ZEND_STR_PORT), port, port_is_null);
	UPDATE_STR_PROPERTY(execute_data, ZSTR_KNOWN(ZEND_STR_USER), user);
	UPDATE_STR_PROPERTY(execute_data, ZSTR_KNOWN(ZEND_STR_PASSWORD), password);
	UPDATE_STR_PROPERTY(execute_data, ZSTR_KNOWN(ZEND_STR_PATH), path);
	UPDATE_STR_PROPERTY(execute_data, ZSTR_KNOWN(ZEND_STR_QUERY), query);
	UPDATE_STR_PROPERTY(execute_data, ZSTR_KNOWN(ZEND_STR_FRAGMENT), fragment);
}

PHP_METHOD(Url_Url, __toString)
{
	ZEND_PARSE_PARAMETERS_NONE();

	// TODO Not implemented yet
	RETURN_EMPTY_STRING();
}

PHP_METHOD(Url_UrlParser, parseUrlComponent)
{
	zend_string *url;
	zend_object *component;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR(url)
		Z_PARAM_OBJ_OF_CLASS(component, url_component_ce)
	ZEND_PARSE_PARAMETERS_END();

	// TODO Not implemented yet
	RETURN_EMPTY_STRING();
}

PHP_METHOD(Url_UrlParser, parseUrl)
{
	zend_string *url;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_PATH_STR(url)
	ZEND_PARSE_PARAMETERS_END();

	parse_url_to_object(url, return_value);
}

PHP_METHOD(Url_UrlParser, parseUrlArray)
{
	zend_string *url;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_PATH_STR(url)
	ZEND_PARSE_PARAMETERS_END();

	parse_url_to_array(url, return_value);
}

void url_register_symbols(void)
{
	url_ce = register_class_Url_Url(zend_ce_stringable);
	url_component_ce = register_class_Url_UrlComponent();
	url_parser_ce = register_class_Url_UrlParser();
}

PHP_MINIT_FUNCTION(url)
{
	url_register_symbols();

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(url)
{
	return SUCCESS;
}

PHP_RINIT_FUNCTION(url)
{
#if defined(COMPILE_DL_URL) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	URL_G(parser) = init_parser();
	URL_G(urls) = 0;

	return URL_G(parser) == NULL ? FAILURE : SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(url)
{
	destroy_parser(URL_G(parser));

	return SUCCESS;
}

zend_module_entry url_module_entry = {
	STANDARD_MODULE_HEADER,
	"url",                             /* Extension name */
	NULL,                           /* zend_function_entry */
	PHP_MINIT(url),         /* PHP_MINIT - Module initialization */
	PHP_MSHUTDOWN(url),   /* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(url),         /* PHP_RINIT - Request initialization */
	PHP_RSHUTDOWN(url),   /* PHP_RSHUTDOWN - Request shutdown */
	NULL,                            /* PHP_MINFO - Module info */
	PHP_VERSION,                      /* Version */
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_URL
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(url)
#endif
