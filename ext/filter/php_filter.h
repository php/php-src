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
  | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
  |          Derick Rethans <derick@php.net>                             |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_FILTER_H
#define PHP_FILTER_H

#include "SAPI.h"
#include "zend_API.h"
#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"
#include "ext/standard/html.h"
#include "php_variables.h"

extern zend_module_entry filter_module_entry;
#define phpext_filter_ptr &filter_module_entry

#ifdef ZTS
#include "TSRM.h"
#endif

#define PHP_FILTER_VERSION PHP_VERSION

PHP_MINIT_FUNCTION(filter);
PHP_MSHUTDOWN_FUNCTION(filter);
PHP_RINIT_FUNCTION(filter);
PHP_RSHUTDOWN_FUNCTION(filter);
PHP_MINFO_FUNCTION(filter);

ZEND_BEGIN_MODULE_GLOBALS(filter)
	zval post_array;
	zval get_array;
	zval cookie_array;
	zval env_array;
	zval server_array;
#if 0
	zval session_array;
#endif
	zend_long default_filter;
	zend_long default_filter_flags;
ZEND_END_MODULE_GLOBALS(filter)

#if defined(COMPILE_DL_FILTER) && defined(ZTS)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#define IF_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(filter, v)

#define PHP_INPUT_FILTER_PARAM_DECL zval *value, zend_long flags, zval *option_array, char *charset
void php_filter_int(PHP_INPUT_FILTER_PARAM_DECL);
void php_filter_boolean(PHP_INPUT_FILTER_PARAM_DECL);
void php_filter_float(PHP_INPUT_FILTER_PARAM_DECL);
void php_filter_validate_regexp(PHP_INPUT_FILTER_PARAM_DECL);
void php_filter_validate_domain(PHP_INPUT_FILTER_PARAM_DECL);
void php_filter_validate_url(PHP_INPUT_FILTER_PARAM_DECL);
void php_filter_validate_email(PHP_INPUT_FILTER_PARAM_DECL);
void php_filter_validate_ip(PHP_INPUT_FILTER_PARAM_DECL);
void php_filter_validate_mac(PHP_INPUT_FILTER_PARAM_DECL);

void php_filter_string(PHP_INPUT_FILTER_PARAM_DECL);
void php_filter_encoded(PHP_INPUT_FILTER_PARAM_DECL);
void php_filter_special_chars(PHP_INPUT_FILTER_PARAM_DECL);
void php_filter_full_special_chars(PHP_INPUT_FILTER_PARAM_DECL);
void php_filter_unsafe_raw(PHP_INPUT_FILTER_PARAM_DECL);
void php_filter_email(PHP_INPUT_FILTER_PARAM_DECL);
void php_filter_url(PHP_INPUT_FILTER_PARAM_DECL);
void php_filter_number_int(PHP_INPUT_FILTER_PARAM_DECL);
void php_filter_number_float(PHP_INPUT_FILTER_PARAM_DECL);
void php_filter_add_slashes(PHP_INPUT_FILTER_PARAM_DECL);

void php_filter_callback(PHP_INPUT_FILTER_PARAM_DECL);

#endif	/* FILTER_H */
