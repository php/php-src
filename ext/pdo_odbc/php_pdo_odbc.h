/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://www.php.net/license/3_01.txt                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wez Furlong <wez@php.net>                                    |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_PDO_ODBC_H
#define PHP_PDO_ODBC_H

extern zend_module_entry pdo_odbc_module_entry;
#define phpext_pdo_odbc_ptr &pdo_odbc_module_entry

#include "php_version.h"
#define PHP_PDO_ODBC_VERSION PHP_VERSION

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(pdo_odbc);
PHP_MSHUTDOWN_FUNCTION(pdo_odbc);
PHP_RINIT_FUNCTION(pdo_odbc);
PHP_RSHUTDOWN_FUNCTION(pdo_odbc);
PHP_MINFO_FUNCTION(pdo_odbc);

#endif	/* PHP_PDO_ODBC_H */
