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
  | Author: Edin Kadribasic <edink@emini.dk>                             |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_PDO_PGSQL_H
#define PHP_PDO_PGSQL_H

#include <libpq-fe.h>

extern zend_module_entry pdo_pgsql_module_entry;
#define phpext_pdo_pgsql_ptr &pdo_pgsql_module_entry

#include "php_version.h"
#define PHP_PDO_PGSQL_VERSION PHP_VERSION

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(pdo_pgsql);
PHP_MSHUTDOWN_FUNCTION(pdo_pgsql);
PHP_MINFO_FUNCTION(pdo_pgsql);

#endif	/* PHP_PDO_PGSQL_H */
