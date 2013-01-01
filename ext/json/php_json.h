/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2013 The PHP Group                                |
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

#ifndef PHP_JSON_H
#define PHP_JSON_H

#define PHP_JSON_VERSION "1.2.1"
#include "ext/standard/php_smart_str.h"

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

ZEND_BEGIN_MODULE_GLOBALS(json)
	int encoder_depth;
	int error_code;
ZEND_END_MODULE_GLOBALS(json)

#ifdef ZTS
# define JSON_G(v) TSRMG(json_globals_id, zend_json_globals *, v)
#else
# define JSON_G(v) (json_globals.v)
#endif

PHP_JSON_API void php_json_encode(smart_str *buf, zval *val, int options TSRMLS_DC);
PHP_JSON_API void php_json_decode_ex(zval *return_value, char *str, int str_len, int options, long depth TSRMLS_DC);
extern zend_class_entry *php_json_serializable_ce;


/* json_encode() options */
#define PHP_JSON_HEX_TAG	(1<<0)
#define PHP_JSON_HEX_AMP	(1<<1)
#define PHP_JSON_HEX_APOS	(1<<2)
#define PHP_JSON_HEX_QUOT	(1<<3)
#define PHP_JSON_FORCE_OBJECT	(1<<4)
#define PHP_JSON_NUMERIC_CHECK	(1<<5)
#define PHP_JSON_UNESCAPED_SLASHES	(1<<6)
#define PHP_JSON_PRETTY_PRINT	(1<<7)
#define PHP_JSON_UNESCAPED_UNICODE	(1<<8)

/* Internal flags */
#define PHP_JSON_OUTPUT_ARRAY	0
#define PHP_JSON_OUTPUT_OBJECT	1

/* json_decode() options */
#define PHP_JSON_OBJECT_AS_ARRAY	(1<<0)
#define PHP_JSON_BIGINT_AS_STRING	(1<<1)

static inline void php_json_decode(zval *return_value, char *str, int str_len, zend_bool assoc, long depth TSRMLS_DC)
{
	php_json_decode_ex(return_value, str, str_len, assoc ? PHP_JSON_OBJECT_AS_ARRAY : 0, depth TSRMLS_CC);
}


#endif  /* PHP_JSON_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
