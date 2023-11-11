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
  | Author: Omar Kilani <omar@php.net>                                   |
  |         Jakub Zelenka <bukka@php.net>                                |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/html.h"
#include "zend_smart_str.h"
#include "php_json.h"
#include "php_json_encoder.h"
#include "php_json_parser.h"
#include "json_arginfo.h"
#include <zend_exceptions.h>

static PHP_MINFO_FUNCTION(json);

PHP_JSON_API zend_class_entry *php_json_serializable_ce;
PHP_JSON_API zend_class_entry *php_json_exception_ce;

PHP_JSON_API ZEND_DECLARE_MODULE_GLOBALS(json)

/* {{{ MINIT */
static PHP_MINIT_FUNCTION(json)
{
	php_json_serializable_ce = register_class_JsonSerializable();

	php_json_exception_ce = register_class_JsonException(zend_ce_exception);

	register_json_symbols(module_number);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_GINIT_FUNCTION */
static PHP_GINIT_FUNCTION(json)
{
#if defined(COMPILE_DL_JSON) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	json_globals->encoder_depth = 0;
	json_globals->error_code = 0;
	json_globals->encode_max_depth = PHP_JSON_PARSER_DEFAULT_DEPTH;
}
/* }}} */

static PHP_RINIT_FUNCTION(json)
{
	JSON_G(error_code) = 0;
	return SUCCESS;
}

/* {{{ json_module_entry */
zend_module_entry json_module_entry = {
	STANDARD_MODULE_HEADER,
	"json",
	ext_functions,
	PHP_MINIT(json),
	NULL,
	PHP_RINIT(json),
	NULL,
	PHP_MINFO(json),
	PHP_JSON_VERSION,
	PHP_MODULE_GLOBALS(json),
	PHP_GINIT(json),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_JSON
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(json)
#endif

/* {{{ PHP_MINFO_FUNCTION */
static PHP_MINFO_FUNCTION(json)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "json support", "enabled");
	php_info_print_table_end();
}
/* }}} */

PHP_JSON_API zend_string *php_json_encode_string(const char *s, size_t len, int options)
{
	smart_str buf = {0};
	php_json_encoder encoder;

	php_json_encode_init(&encoder);

	if (php_json_escape_string(&buf, s, len, options, &encoder) == FAILURE) {
		smart_str_free(&buf);
		return NULL;
	}

	return smart_str_extract(&buf);
}

PHP_JSON_API zend_result php_json_encode_ex(smart_str *buf, zval *val, int options, zend_long depth) /* {{{ */
{
	php_json_encoder encoder;
	zend_result return_code;

	php_json_encode_init(&encoder);
	encoder.max_depth = depth;

	return_code = php_json_encode_zval(buf, val, options, &encoder);
	JSON_G(error_code) = encoder.error_code;

	return return_code;
}
/* }}} */

PHP_JSON_API zend_result php_json_encode(smart_str *buf, zval *val, int options) /* {{{ */
{
	return php_json_encode_ex(buf, val, options, JSON_G(encode_max_depth));
}
/* }}} */

static const char *php_json_get_error_msg(php_json_error_code error_code) /* {{{ */
{
	switch(error_code) {
		case PHP_JSON_ERROR_NONE:
			return "No error";
		case PHP_JSON_ERROR_DEPTH:
			return "Maximum stack depth exceeded";
		case PHP_JSON_ERROR_STATE_MISMATCH:
			return "State mismatch (invalid or malformed JSON)";
		case PHP_JSON_ERROR_CTRL_CHAR:
			return "Control character error, possibly incorrectly encoded";
		case PHP_JSON_ERROR_SYNTAX:
			return "Syntax error";
		case PHP_JSON_ERROR_UTF8:
			return "Malformed UTF-8 characters, possibly incorrectly encoded";
		case PHP_JSON_ERROR_RECURSION:
			return "Recursion detected";
		case PHP_JSON_ERROR_INF_OR_NAN:
			return "Inf and NaN cannot be JSON encoded";
		case PHP_JSON_ERROR_UNSUPPORTED_TYPE:
			return "Type is not supported";
		case PHP_JSON_ERROR_INVALID_PROPERTY_NAME:
			return "The decoded property name is invalid";
		case PHP_JSON_ERROR_UTF16:
			return "Single unpaired UTF-16 surrogate in unicode escape";
		case PHP_JSON_ERROR_NON_BACKED_ENUM:
			return "Non-backed enums have no default serialization";
		default:
			return "Unknown error";
	}
}
/* }}} */

PHP_JSON_API zend_result php_json_decode_ex(zval *return_value, const char *str, size_t str_len, zend_long options, zend_long depth) /* {{{ */
{
	php_json_parser parser;

	php_json_parser_init(&parser, return_value, str, str_len, (int)options, (int)depth);

	if (php_json_yyparse(&parser)) {
		php_json_error_code error_code = php_json_parser_error_code(&parser);
		if (!(options & PHP_JSON_THROW_ON_ERROR)) {
			JSON_G(error_code) = error_code;
		} else {
			zend_throw_exception(php_json_exception_ce, php_json_get_error_msg(error_code), error_code);
		}
		RETVAL_NULL();
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

/* {{{ Returns the JSON representation of a value */
PHP_FUNCTION(json_encode)
{
	zval *parameter;
	php_json_encoder encoder;
	smart_str buf = {0};
	zend_long options = 0;
	zend_long depth = PHP_JSON_PARSER_DEFAULT_DEPTH;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_ZVAL(parameter)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(options)
		Z_PARAM_LONG(depth)
	ZEND_PARSE_PARAMETERS_END();

	php_json_encode_init(&encoder);
	encoder.max_depth = (int)depth;
	php_json_encode_zval(&buf, parameter, (int)options, &encoder);

	if (!(options & PHP_JSON_THROW_ON_ERROR) || (options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR)) {
		JSON_G(error_code) = encoder.error_code;
		if (encoder.error_code != PHP_JSON_ERROR_NONE && !(options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR)) {
			smart_str_free(&buf);
			RETURN_FALSE;
		}
	} else {
		if (encoder.error_code != PHP_JSON_ERROR_NONE) {
			smart_str_free(&buf);
			zend_throw_exception(php_json_exception_ce, php_json_get_error_msg(encoder.error_code), encoder.error_code);
			RETURN_THROWS();
		}
	}

	RETURN_STR(smart_str_extract(&buf));
}
/* }}} */

/* {{{ Decodes the JSON representation into a PHP value */
PHP_FUNCTION(json_decode)
{
	char *str;
	size_t str_len;
	bool assoc = 0; /* return JS objects as PHP objects by default */
	bool assoc_null = 1;
	zend_long depth = PHP_JSON_PARSER_DEFAULT_DEPTH;
	zend_long options = 0;

	ZEND_PARSE_PARAMETERS_START(1, 4)
		Z_PARAM_STRING(str, str_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL_OR_NULL(assoc, assoc_null)
		Z_PARAM_LONG(depth)
		Z_PARAM_LONG(options)
	ZEND_PARSE_PARAMETERS_END();

	if (!(options & PHP_JSON_THROW_ON_ERROR)) {
		JSON_G(error_code) = PHP_JSON_ERROR_NONE;
	}

	if (!str_len) {
		if (!(options & PHP_JSON_THROW_ON_ERROR)) {
			JSON_G(error_code) = PHP_JSON_ERROR_SYNTAX;
		} else {
			zend_throw_exception(php_json_exception_ce, php_json_get_error_msg(PHP_JSON_ERROR_SYNTAX), PHP_JSON_ERROR_SYNTAX);
		}
		RETURN_NULL();
	}

	if (depth <= 0) {
		zend_argument_value_error(3, "must be greater than 0");
		RETURN_THROWS();
	}

	if (depth > INT_MAX) {
		zend_argument_value_error(3, "must be less than %d", INT_MAX);
		RETURN_THROWS();
	}

	/* For BC reasons, the bool $assoc overrides the long $options bit for PHP_JSON_OBJECT_AS_ARRAY */
	if (!assoc_null) {
		if (assoc) {
			options |=  PHP_JSON_OBJECT_AS_ARRAY;
		} else {
			options &= ~PHP_JSON_OBJECT_AS_ARRAY;
		}
	}

	php_json_decode_ex(return_value, str, str_len, options, depth);
}
/* }}} */

/* {{{ Returns the error code of the last json_encode() or json_decode() call. */
PHP_FUNCTION(json_last_error)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_LONG(JSON_G(error_code));
}
/* }}} */

/* {{{ Returns the error string of the last json_encode() or json_decode() call. */
PHP_FUNCTION(json_last_error_msg)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_STRING(php_json_get_error_msg(JSON_G(error_code)));
}
/* }}} */
