/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Ard Biesheuvel <abies@php.net>                               |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_PDO_FIREBIRD_H
#define PHP_PDO_FIREBIRD_H

extern zend_module_entry pdo_firebird_module_entry;
#define phpext_pdo_firebird_ptr &pdo_firebird_module_entry

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(pdo_firebird);
PHP_MSHUTDOWN_FUNCTION(pdo_firebird);
PHP_RINIT_FUNCTION(pdo_firebird);
PHP_RSHUTDOWN_FUNCTION(pdo_firebird);
PHP_MINFO_FUNCTION(pdo_firebird);

#endif	/* PHP_PDO_FIREBIRD_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
