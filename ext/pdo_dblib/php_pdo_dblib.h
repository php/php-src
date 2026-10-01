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
  |         Frank M. Kromann <frank@kromann.info>                        |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_PDO_DBLIB_H
#define PHP_PDO_DBLIB_H

#ifdef PDO_DBLIB_IS_MSSQL
extern zend_module_entry pdo_mssql_module_entry;
#define phpext_pdo_mssql_ptr &pdo_mssql_module_entry
#else
extern zend_module_entry pdo_dblib_module_entry;
#define phpext_pdo_dblib_ptr &pdo_dblib_module_entry
#endif

#include "php_version.h"
#define PHP_PDO_DBLIB_VERSION PHP_VERSION

#ifdef ZTS
# include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(pdo_dblib);
PHP_MSHUTDOWN_FUNCTION(pdo_dblib);
PHP_MINFO_FUNCTION(pdo_dblib);
PHP_RSHUTDOWN_FUNCTION(pdo_dblib);

#endif
