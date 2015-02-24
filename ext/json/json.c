/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Omar Kilani <omar@php.net>                                   |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

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
#include <zend_exceptions.h>

#include <float.h>
#if defined(DBL_MANT_DIG) && defined(DBL_MIN_EXP)
#define NUM_BUF_SIZE (3 + DBL_MANT_DIG - DBL_MIN_EXP)
#else
#define NUM_BUF_SIZE 1080
#endif


static PHP_MINFO_FUNCTION(json);
static PHP_FUNCTION(json_encode);
static PHP_FUNCTION(json_decode);
static PHP_FUNCTION(json_last_error);
static PHP_FUNCTION(json_last_error_msg);

PHP_JSON_API zend_class_entry *php_json_serializable_ce;

ZEND_DECLARE_MODULE_GLOBALS(json)

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_json_encode, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, options)
	ZEND_ARG_INFO(0, depth)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_json_decode, 0, 0, 1)
	ZEND_ARG_INFO(0, json)
	ZEND_ARG_INFO(0, assoc)
	ZEND_ARG_INFO(0, depth)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_json_last_error, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_json_last_error_msg, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ json_functions[] */
static const zend_function_entry json_functions[] = {
	PHP_FE(json_encode, arginfo_json_encode)
	PHP_FE(json_decode, arginfo_json_decode)
	PHP_FE(json_last_error, arginfo_json_last_error)
	PHP_FE(json_last_error_msg, arginfo_json_last_error_msg)
	PHP_FE_END
};
/* }}} */

/* {{{ JsonSerializable methods */
ZEND_BEGIN_ARG_INFO(json_serialize_arginfo, 0)
	/* No arguments */
ZEND_END_ARG_INFO();

static const zend_function_entry json_serializable_interface[] = {
	PHP_ABSTRACT_ME(JsonSerializable, jsonSerialize, json_serialize_arginfo)
	PHP_FE_END
};
/* }}} */

/* {{{ MINIT */
static PHP_MINIT_FUNCTION(json)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "JsonSerializable", json_serializable_interface);
	php_json_serializable_ce = zend_register_internal_interface(&ce);

	REGISTER_LONG_CONSTANT("JSON_HEX_TAG",  PHP_JSON_HEX_TAG,  CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_HEX_AMP",  PHP_JSON_HEX_AMP,  CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_HEX_APOS", PHP_JSON_HEX_APOS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_HEX_QUOT", PHP_JSON_HEX_QUOT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_FORCE_OBJECT", PHP_JSON_FORCE_OBJECT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_NUMERIC_CHECK", PHP_JSON_NUMERIC_CHECK, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_UNESCAPED_SLASHES", PHP_JSON_UNESCAPED_SLASHES, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_PRETTY_PRINT", PHP_JSON_PRETTY_PRINT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_UNESCAPED_UNICODE", PHP_JSON_UNESCAPED_UNICODE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_PARTIAL_OUTPUT_ON_ERROR", PHP_JSON_PARTIAL_OUTPUT_ON_ERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_PRESERVE_ZERO_FRACTION", PHP_JSON_PRESERVE_ZERO_FRACTION, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("JSON_ERROR_NONE", PHP_JSON_ERROR_NONE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_ERROR_DEPTH", PHP_JSON_ERROR_DEPTH, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_ERROR_STATE_MISMATCH", PHP_JSON_ERROR_STATE_MISMATCH, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_ERROR_CTRL_CHAR", PHP_JSON_ERROR_CTRL_CHAR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_ERROR_SYNTAX", PHP_JSON_ERROR_SYNTAX, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_ERROR_UTF8", PHP_JSON_ERROR_UTF8, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_ERROR_RECURSION", PHP_JSON_ERROR_RECURSION, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_ERROR_INF_OR_NAN", PHP_JSON_ERROR_INF_OR_NAN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_ERROR_UNSUPPORTED_TYPE", PHP_JSON_ERROR_UNSUPPORTED_TYPE, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("JSON_OBJECT_AS_ARRAY",		PHP_JSON_OBJECT_AS_ARRAY,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_BIGINT_AS_STRING",		PHP_JSON_BIGINT_AS_STRING,		CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_GINIT_FUNCTION
*/
static PHP_GINIT_FUNCTION(json)
{
#if defined(COMPILE_DL_JSON) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	json_globals->encoder_depth = 0;
	json_globals->error_code = 0;
	json_globals->encode_max_depth = 0;
}
/* }}} */


/* {{{ json_module_entry
 */
zend_module_entry json_module_entry = {
	STANDARD_MODULE_HEADER,
	"json",
	json_functions,
	PHP_MINIT(json),
	NULL,
	NULL,
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
ZEND_TSRMLS_CACHE_DEFINE();
#endif
ZEND_GET_MODULE(json)
#endif

/* {{{ PHP_MINFO_FUNCTION
 */
static PHP_MINFO_FUNCTION(json)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "json support", "enabled");
	php_info_print_table_row(2, "json version", PHP_JSON_VERSION);
	php_info_print_table_end();
}
/* }}} */

PHP_JSON_API void php_json_encode(smart_str *buf, zval *val, int options) /* {{{ */
{
	php_json_encode_zval(buf, val, options);
}
/* }}} */

PHP_JSON_API void php_json_decode_ex(zval *return_value, char *str, size_t str_len, zend_long options, zend_long depth) /* {{{ */
{
	php_json_parser parser;

	php_json_parser_init(&parser, return_value, str, str_len, (int)options, (int)depth);

	if (php_json_yyparse(&parser)) {
		JSON_G(error_code) = php_json_parser_error_code(&parser);
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ proto string json_encode(mixed data [, int options[, int depth]])
   Returns the JSON representation of a value */
static PHP_FUNCTION(json_encode)
{
	zval *parameter;
	smart_str buf = {0};
	zend_long options = 0;
	zend_long depth = PHP_JSON_PARSER_DEFAULT_DEPTH;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|ll", &parameter, &options, &depth) == FAILURE) {
		return;
	}

	JSON_G(error_code) = PHP_JSON_ERROR_NONE;

	JSON_G(encode_max_depth) = (int)depth;

	php_json_encode(&buf, parameter, (int)options);

	if (JSON_G(error_code) != PHP_JSON_ERROR_NONE && !(options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR)) {
		smart_str_free(&buf);
		ZVAL_FALSE(return_value);
	} else {
		smart_str_0(&buf); /* copy? */
		ZVAL_NEW_STR(return_value, buf.s);
	}
}
/* }}} */

/* {{{ proto mixed json_decode(string json [, bool assoc [, long depth]])
   Decodes the JSON representation into a PHP value */
static PHP_FUNCTION(json_decode)
{
	char *str;
	size_t str_len;
	zend_bool assoc = 0; /* return JS objects as PHP objects by default */
	zend_long depth = PHP_JSON_PARSER_DEFAULT_DEPTH;
	zend_long options = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|bll", &str, &str_len, &assoc, &depth, &options) == FAILURE) {
		return;
	}

	JSON_G(error_code) = 0;

	if (!str_len) {
		JSON_G(error_code) = PHP_JSON_ERROR_SYNTAX;
		RETURN_NULL();
	}

	/* For BC reasons, the bool $assoc overrides the long $options bit for PHP_JSON_OBJECT_AS_ARRAY */
	if (assoc) {
		options |=  PHP_JSON_OBJECT_AS_ARRAY;
	} else {
		options &= ~PHP_JSON_OBJECT_AS_ARRAY;
	}

	php_json_decode_ex(return_value, str, str_len, options, depth);
}
/* }}} */

/* {{{ proto int json_last_error()
   Returns the error code of the last json_encode() or json_decode() call. */
static PHP_FUNCTION(json_last_error)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(JSON_G(error_code));
}
/* }}} */

/* {{{ proto string json_last_error_msg()
   Returns the error string of the last json_encode() or json_decode() call. */
static PHP_FUNCTION(json_last_error_msg)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	switch(JSON_G(error_code)) {
		case PHP_JSON_ERROR_NONE:
			RETURN_STRING("No error");
		case PHP_JSON_ERROR_DEPTH:
			RETURN_STRING("Maximum stack depth exceeded");
		case PHP_JSON_ERROR_STATE_MISMATCH:
			RETURN_STRING("State mismatch (invalid or malformed JSON)");
		case PHP_JSON_ERROR_CTRL_CHAR:
			RETURN_STRING("Control character error, possibly incorrectly encoded");
		case PHP_JSON_ERROR_SYNTAX:
			RETURN_STRING("Syntax error");
		case PHP_JSON_ERROR_UTF8:
			RETURN_STRING("Malformed UTF-8 characters, possibly incorrectly encoded");
		case PHP_JSON_ERROR_RECURSION:
			RETURN_STRING("Recursion detected");
		case PHP_JSON_ERROR_INF_OR_NAN:
			RETURN_STRING("Inf and NaN cannot be JSON encoded");
		case PHP_JSON_ERROR_UNSUPPORTED_TYPE:
			RETURN_STRING("Type is not supported");
		default:
			RETURN_STRING("Unknown error");
	}

}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
