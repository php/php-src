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

#ifndef PHP_PDO_SQLITE_H
#define PHP_PDO_SQLITE_H

extern zend_module_entry pdo_sqlite_module_entry;
#define phpext_pdo_sqlite_ptr &pdo_sqlite_module_entry

#include "php_version.h"
#define PHP_PDO_SQLITE_VERSION PHP_VERSION

#ifdef ZTS
#include "TSRM.h"
#endif

enum pdo_sqlite_transaction_mode {
	PDO_SQLITE_TRANSACTION_MODE_DEFERRED = 0,
	PDO_SQLITE_TRANSACTION_MODE_IMMEDIATE = 1,
	PDO_SQLITE_TRANSACTION_MODE_EXCLUSIVE = 2
};

PHP_MINIT_FUNCTION(pdo_sqlite);
PHP_MSHUTDOWN_FUNCTION(pdo_sqlite);
PHP_RINIT_FUNCTION(pdo_sqlite);
PHP_RSHUTDOWN_FUNCTION(pdo_sqlite);
PHP_MINFO_FUNCTION(pdo_sqlite);

#endif	/* PHP_PDO_SQLITE_H */
