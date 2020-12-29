/*
  +----------------------------------------------------------------------+
  | simdjson                                                             |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.0 of the Apache license,    |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.apache.org/licenses/LICENSE-2.0.html                      |
  +----------------------------------------------------------------------+
  | Author: Jinxi Wang  <1054636713@qq.com>                              |
  | Author: Máté Kocsis <kocsismate@woohoolabs.com>                      |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "Zend/zend_interfaces.h"
#include "Zend/zend_exceptions.h"
#include "ext/standard/info.h"

#include "simdjson_arginfo.h"
#include "php_simdjson.h"
#include "src/bindings.h"

ZEND_DECLARE_MODULE_GLOBALS(simdjson);

PHPAPI zend_class_entry *json_parser_ce;
PHPAPI zend_class_entry *json_encoder_ce;

extern bool cplus_simdjson_is_valid(const char *json, size_t len);

extern void cplus_simdjson_parse(const char *json, size_t len, zval *return_value, unsigned char assoc, u_short depth);

extern void cplus_simdjson_key_value(const char *json, size_t len, const char *key, zval *return_value, unsigned char assoc, u_short depth);

extern u_short cplus_simdjson_key_exists(const char *json, size_t len, const char *key, u_short depth);

extern void cplus_simdjson_key_count(const char *json, size_t len, const char *key, zval *return_value, u_short depth);

PHP_METHOD(JsonParser, isValid)
{
	zend_string *json;
	zend_bool is_valid;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(json)
	ZEND_PARSE_PARAMETERS_END();

    is_valid = cplus_simdjson_is_valid(ZSTR_VAL(json), ZSTR_LEN(json));

    RETURN_BOOL(is_valid);
}

PHP_METHOD(JsonParser, parse)
{
	zend_string *json;
	size_t str_len;
	zend_bool assoc = 0;
	zend_long depth = SIMDJSON_PARSE_DEFAULT_DEPTH;
	zend_long flags = 0;

	ZEND_PARSE_PARAMETERS_START(1, 4)
		Z_PARAM_STR(json)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(assoc)
		Z_PARAM_LONG(depth)
		Z_PARAM_LONG(flags)
	ZEND_PARSE_PARAMETERS_END();

    cplus_simdjson_parse(ZSTR_VAL(json), ZSTR_LEN(json), return_value, assoc, depth + 1);
}

PHP_METHOD(JsonParser, getKeyValue)
{
	zend_string *json;
	zend_string *key;
	zend_bool assoc = 0;
	zend_long depth = SIMDJSON_PARSE_DEFAULT_DEPTH;
	zend_long flags = 0;

	ZEND_PARSE_PARAMETERS_START(2, 5)
		Z_PARAM_STR(json)
		Z_PARAM_STR(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(assoc)
		Z_PARAM_LONG(depth)
		Z_PARAM_LONG(flags)
	ZEND_PARSE_PARAMETERS_END();

    cplus_simdjson_key_value(ZSTR_VAL(json), ZSTR_LEN(json), ZSTR_VAL(key), return_value, assoc, depth + 1);
}

PHP_METHOD(JsonParser, getKeyCount)
{
	zend_string *json;
	zend_string *key;
	zend_long depth = SIMDJSON_PARSE_DEFAULT_DEPTH;
	zend_long options = 0;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(json)
		Z_PARAM_STR(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(depth)
	ZEND_PARSE_PARAMETERS_END();

	cplus_simdjson_key_count(ZSTR_VAL(json), ZSTR_LEN(json), ZSTR_VAL(key), return_value, depth + 1);
}

PHP_METHOD(JsonParser, keyExists)
{
	zend_string *json;
	zend_string *key;
	zend_long depth = SIMDJSON_PARSE_DEFAULT_DEPTH;
	zend_long options = 0;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(json)
		Z_PARAM_STR(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(depth)
	ZEND_PARSE_PARAMETERS_END();

    u_short stats = SIMDJSON_PARSE_FAIL;
    stats = cplus_simdjson_key_exists(ZSTR_VAL(json), ZSTR_LEN(json), ZSTR_VAL(key), depth + 1);

    switch (stats) {
    	case SIMDJSON_PARSE_FAIL:
    		RETURN_NULL();
		case SIMDJSON_PARSE_KEY_EXISTS:
    		RETURN_TRUE;
		case SIMDJSON_PARSE_KEY_NOEXISTS:
    		RETURN_FALSE;
    	EMPTY_SWITCH_DEFAULT_CASE()
    }
}

PHP_GINIT_FUNCTION(simdjson)
{
}

PHP_MINIT_FUNCTION(simdjson)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "JsonParser", class_JsonParser_methods);
	json_parser_ce = zend_register_internal_class_ex(&ce, NULL);

	INIT_CLASS_ENTRY(ce, "JsonEncoder", NULL);
	json_encoder_ce = zend_register_internal_class_ex(&ce, NULL);

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(simdjson)
{
    return SUCCESS;
}

PHP_RINIT_FUNCTION(simdjson)
{
    return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(simdjson)
{
    return SUCCESS;
}

PHP_MINFO_FUNCTION(simdjson)
{
    php_info_print_table_start();
    php_info_print_table_row(2, "simdjson support", "enabled");
    php_info_print_table_row(2, "simdjson version", "0.4.1");

    php_info_print_table_end();
}

#ifdef COMPILE_DL_SIMDJSON
ZEND_GET_MODULE(simdjson)
#endif

zend_module_dep simdjson_deps[] = {
	ZEND_MOD_REQUIRED("json")
	ZEND_MOD_END
};

zend_module_entry simdjson_module_entry = {
    STANDARD_MODULE_HEADER_EX,
    NULL,
    simdjson_deps,
    "simdjson",
    NULL,
    PHP_MINIT(simdjson),
    PHP_MSHUTDOWN(simdjson),
    PHP_RINIT(simdjson),
    PHP_RSHUTDOWN(simdjson),
    PHP_MINFO(simdjson),
    PHP_SIMDJSON_VERSION,
    PHP_MODULE_GLOBALS(simdjson),
    PHP_GINIT(simdjson),
    NULL,
    NULL,
    STANDARD_MODULE_PROPERTIES_EX
};
