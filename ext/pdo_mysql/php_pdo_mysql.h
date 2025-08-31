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
