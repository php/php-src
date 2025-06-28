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
  | Author: Ard Biesheuvel <abies@php.net>                               |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_PDO_FIREBIRD_H
#define PHP_PDO_FIREBIRD_H

extern zend_module_entry pdo_firebird_module_entry;
#define phpext_pdo_firebird_ptr &pdo_firebird_module_entry

#include "php_version.h"
#define PHP_PDO_FIREBIRD_VERSION PHP_VERSION

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(pdo_firebird);
PHP_MSHUTDOWN_FUNCTION(pdo_firebird);
PHP_RINIT_FUNCTION(pdo_firebird);
PHP_RSHUTDOWN_FUNCTION(pdo_firebird);
PHP_MINFO_FUNCTION(pdo_firebird);

#endif	/* PHP_PDO_FIREBIRD_H */
