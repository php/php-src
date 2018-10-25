/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2018 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wez Furlong <wez@php.net>                                    |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_PDO_SQLITE_H
#define PHP_PDO_SQLITE_H

extern zend_module_entry pdo_sqlite_module_entry;
#define phpext_pdo_sqlite_ptr &pdo_sqlite_module_entry

#include "php_version.h"
#define PHP_PDO_SQLITE_VERSION PHP_VERSION

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(pdo_sqlite);
PHP_MSHUTDOWN_FUNCTION(pdo_sqlite);
PHP_RINIT_FUNCTION(pdo_sqlite);
PHP_RSHUTDOWN_FUNCTION(pdo_sqlite);
PHP_MINFO_FUNCTION(pdo_sqlite);

/*
  	Declare any global variables you may need between the BEGIN
	and END macros here:

ZEND_BEGIN_MODULE_GLOBALS(pdo_sqlite)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(pdo_sqlite)
*/

#ifdef ZTS
#define PDO_SQLITE_G(v) TSRMG(pdo_sqlite_globals_id, zend_pdo_sqlite_globals *, v)
#else
#define PDO_SQLITE_G(v) (pdo_sqlite_globals.v)
#endif

#endif	/* PHP_PDO_SQLITE_H */
