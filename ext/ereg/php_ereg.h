/* 
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                        |
   +----------------------------------------------------------------------+
*/


/* $Id$ */

#ifndef EREG_H
#define EREG_H

#include "php_regex.h"

extern zend_module_entry ereg_module_entry;
#define phpext_ereg_ptr &ereg_module_entry

#ifdef PHP_WIN32
# define PHP_EREG_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
# define PHP_EREG_API __attribute__ ((visibility("default")))
#else
# define PHP_EREG_API
#endif

PHP_EREG_API char *php_ereg_replace(const char *pattern, const char *replace, const char *string, int icase, int extended TSRMLS_DC);

PHP_FUNCTION(ereg);
PHP_FUNCTION(eregi);
PHP_FUNCTION(eregi_replace);
PHP_FUNCTION(ereg_replace);
PHP_FUNCTION(split);
PHP_FUNCTION(spliti);
PHP_EREG_API PHP_FUNCTION(sql_regcase);

ZEND_BEGIN_MODULE_GLOBALS(ereg)
	HashTable ht_rc;
	unsigned int lru_counter;
ZEND_END_MODULE_GLOBALS(ereg)

/* Module functions */
PHP_MINFO_FUNCTION(ereg);

#ifdef ZTS
#define EREG(v) TSRMG(ereg_globals_id, zend_ereg_globals *, v)
#else
#define EREG(v) (ereg_globals.v)
#endif

ZEND_EXTERN_MODULE_GLOBALS(ereg)

#endif /* REG_H */
