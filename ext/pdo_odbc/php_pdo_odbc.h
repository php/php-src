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
