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
  | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
  |          Derick Rethans <derick@php.net>                             |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

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

PHP_MINIT_FUNCTION(filter);
PHP_MSHUTDOWN_FUNCTION(filter);
PHP_RINIT_FUNCTION(filter);
PHP_RSHUTDOWN_FUNCTION(filter);
PHP_MINFO_FUNCTION(filter);

PHP_FUNCTION(filter_input);
PHP_FUNCTION(filter_var);
PHP_FUNCTION(filter_input_array);
PHP_FUNCTION(filter_var_array);
PHP_FUNCTION(filter_list);
PHP_FUNCTION(filter_has_var);
PHP_FUNCTION(filter_id);

ZEND_BEGIN_MODULE_GLOBALS(filter)
	zval post_array;
	zval get_array;
	zval cookie_array;
	zval env_array;
	zval server_array;
	zval session_array;
	zend_long default_filter;
	zend_long default_filter_flags;
ZEND_END_MODULE_GLOBALS(filter)

#ifdef ZTS
#define IF_G(v) ZEND_TSRMG(filter_globals_id, zend_filter_globals *, v)
ZEND_TSRMLS_CACHE_EXTERN;
#else
#define IF_G(v) (filter_globals.v)
#endif


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
void php_filter_magic_quotes(PHP_INPUT_FILTER_PARAM_DECL);

void php_filter_callback(PHP_INPUT_FILTER_PARAM_DECL);

#endif	/* FILTER_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
