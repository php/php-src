/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
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
  |         Jakub Zelenka <bukka@php.net>                                |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_JSON_H
#define PHP_JSON_H

#include "php_version.h"
#include "zend_smart_str_public.h"

#define PHP_JSON_VERSION PHP_VERSION

extern zend_module_entry json_module_entry;
#define phpext_json_ptr &json_module_entry

#if defined(PHP_WIN32) && defined(JSON_EXPORTS)
#define PHP_JSON_API __declspec(dllexport)
#else
#define PHP_JSON_API PHPAPI
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

extern PHP_JSON_API zend_class_entry *php_json_serializable_ce;

/* error codes */
typedef enum {
	PHP_JSON_ERROR_NONE = 0,
	PHP_JSON_ERROR_DEPTH,
	PHP_JSON_ERROR_STATE_MISMATCH,
	PHP_JSON_ERROR_CTRL_CHAR,
	PHP_JSON_ERROR_SYNTAX,
	PHP_JSON_ERROR_UTF8,
	PHP_JSON_ERROR_RECURSION,
	PHP_JSON_ERROR_INF_OR_NAN,
	PHP_JSON_ERROR_UNSUPPORTED_TYPE,
	PHP_JSON_ERROR_INVALID_PROPERTY_NAME,
	PHP_JSON_ERROR_UTF16
} php_json_error_code;

/* json_decode() options */
#define PHP_JSON_OBJECT_AS_ARRAY            (1<<0)
#define PHP_JSON_BIGINT_AS_STRING           (1<<1)

/* json_encode() options */
#define PHP_JSON_HEX_TAG                    (1<<0)
#define PHP_JSON_HEX_AMP                    (1<<1)
#define PHP_JSON_HEX_APOS                   (1<<2)
#define PHP_JSON_HEX_QUOT                   (1<<3)
#define PHP_JSON_FORCE_OBJECT               (1<<4)
#define PHP_JSON_NUMERIC_CHECK              (1<<5)
#define PHP_JSON_UNESCAPED_SLASHES          (1<<6)
#define PHP_JSON_PRETTY_PRINT               (1<<7)
#define PHP_JSON_UNESCAPED_UNICODE          (1<<8)
#define PHP_JSON_PARTIAL_OUTPUT_ON_ERROR    (1<<9)
#define PHP_JSON_PRESERVE_ZERO_FRACTION     (1<<10)
#define PHP_JSON_UNESCAPED_LINE_TERMINATORS (1<<11)

/* json_decode() and json_encode() common options */
#define PHP_JSON_INVALID_UTF8_IGNORE        (1<<20)
#define PHP_JSON_INVALID_UTF8_SUBSTITUTE    (1<<21)
#define PHP_JSON_THROW_ON_ERROR             (1<<22)

/* Internal flags */
#define PHP_JSON_OUTPUT_ARRAY	0
#define PHP_JSON_OUTPUT_OBJECT	1

/* default depth */
#define PHP_JSON_PARSER_DEFAULT_DEPTH 512

ZEND_BEGIN_MODULE_GLOBALS(json)
	int encoder_depth;
	int encode_max_depth;
	php_json_error_code error_code;
ZEND_END_MODULE_GLOBALS(json)

PHP_JSON_API ZEND_EXTERN_MODULE_GLOBALS(json)
#define JSON_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(json, v)

#if defined(ZTS) && defined(COMPILE_DL_JSON)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

PHP_JSON_API int php_json_encode_ex(smart_str *buf, zval *val, int options, zend_long depth);
PHP_JSON_API int php_json_encode(smart_str *buf, zval *val, int options);
PHP_JSON_API int php_json_decode_ex(zval *return_value, const char *str, size_t str_len, zend_long options, zend_long depth);

static inline int php_json_decode(zval *return_value, const char *str, int str_len, zend_bool assoc, zend_long depth)
{
	return php_json_decode_ex(return_value, str, str_len, assoc ? PHP_JSON_OBJECT_AS_ARRAY : 0, depth);
}

#endif  /* PHP_JSON_H */
