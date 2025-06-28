/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
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
