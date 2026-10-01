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
  | Author: George Schlossnagle <george@omniti.com>                      |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_PDO_MYSQL_H
#define PHP_PDO_MYSQL_H

extern zend_module_entry pdo_mysql_module_entry;
#define phpext_pdo_mysql_ptr &pdo_mysql_module_entry

#include "php_version.h"
#define PHP_PDO_MYSQL_VERSION PHP_VERSION

#ifdef PHP_WIN32
#define PHP_PDO_MYSQL_API __declspec(dllexport)
#else
#define PHP_PDO_MYSQL_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif


#endif	/* PHP_PDO_MYSQL_H */
