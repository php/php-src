/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
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
   | Authors: Scott MacVicar <scottmac@php.net>                           |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_SQLITE3_H
#define PHP_SQLITE3_H

#define PHP_SQLITE3_VERSION	PHP_VERSION

extern zend_module_entry sqlite3_module_entry;
#define phpext_sqlite3_ptr &sqlite3_module_entry

ZEND_BEGIN_MODULE_GLOBALS(sqlite3)
	char *extension_dir;
	int dbconfig_defensive;
ZEND_END_MODULE_GLOBALS(sqlite3)

#if defined(ZTS) && defined(COMPILE_DL_SQLITE3)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

ZEND_EXTERN_MODULE_GLOBALS(sqlite3)
#define SQLITE3G(v) ZEND_MODULE_GLOBALS_ACCESSOR(sqlite3, v)

#define PHP_SQLITE3_ASSOC	1<<0
#define PHP_SQLITE3_NUM		1<<1
#define PHP_SQLITE3_BOTH	(PHP_SQLITE3_ASSOC|PHP_SQLITE3_NUM)

#endif
