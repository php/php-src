/*
   +----------------------------------------------------------------------+
   | PHP Version 6                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andrei Zmievski <andrei@php.net>                            |
   +----------------------------------------------------------------------+
*/

/* $Id$ */ 

#ifndef PHP_UNICODE_H
#define PHP_UNICODE_H

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>

#include <php_ini.h>
#include <SAPI.h>
#include <ext/standard/info.h>

extern zend_module_entry unicode_module_entry;
#define phpext_unicode_ptr &unicode_module_entry

#ifdef PHP_WIN32
#	define PHP_UNICODE_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_UNICODE_API __attribute__ ((visibility("default")))
#else
#	define PHP_UNICODE_API
#endif

PHP_MINIT_FUNCTION(unicode);
PHP_MSHUTDOWN_FUNCTION(unicode);
PHP_RINIT_FUNCTION(unicode);
PHP_RSHUTDOWN_FUNCTION(unicode);
PHP_MINFO_FUNCTION(unicode);

#ifdef ZTS
#include "TSRM.h"
#endif

/*
 * Because of ICU they are expanded to have _<version> suffix
 * We don't want that.
 */
#undef locale_get_default
#undef locale_set_default

PHP_FUNCTION(locale_get_default);
PHP_FUNCTION(locale_set_default);
PHP_FUNCTION(collator_create);
PHP_FUNCTION(collator_compare);
PHP_FUNCTION(collator_sort);
PHP_FUNCTION(collator_set_strength);
PHP_FUNCTION(collator_set_attribute);
PHP_FUNCTION(collator_get_strength);
PHP_FUNCTION(collator_get_attribute);
PHP_FUNCTION(collator_get_default);
PHP_FUNCTION(collator_set_default);
PHP_METHOD(collator, __construct);

void php_init_collation(TSRMLS_D);
void php_register_unicode_constants(TSRMLS_D);

PHPAPI zend_class_entry *u_const_ce;

#ifdef  __cplusplus
} // extern "C" 
#endif

#include <zend_unicode.h>

#endif /* PHP_UNICODE_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
