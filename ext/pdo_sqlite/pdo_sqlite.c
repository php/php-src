/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2004 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wez Furlong <wez@php.net>                                    |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "pdo/php_pdo.h"
#include "pdo/php_pdo_driver.h"
#include "php_pdo_sqlite.h"
#include "php_pdo_sqlite_int.h"
#include "zend_exceptions.h"

#define PHP_PDO_SQLITE_MODULE_VERSION	"0.1-dev"

/* {{{ pdo_sqlite_functions[] */
function_entry pdo_sqlite_functions[] = {
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ pdo_sqlite_module_entry
 */
zend_module_entry pdo_sqlite_module_entry = {
	STANDARD_MODULE_HEADER,
	"pdo_sqlite",
	pdo_sqlite_functions,
	PHP_MINIT(pdo_sqlite),
	PHP_MSHUTDOWN(pdo_sqlite),
	PHP_RINIT(pdo_sqlite),
	PHP_RSHUTDOWN(pdo_sqlite),
	PHP_MINFO(pdo_sqlite),
	PHP_PDO_SQLITE_MODULE_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PDO_SQLITE
ZEND_GET_MODULE(pdo_sqlite)
#endif

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(pdo_sqlite)
{
	/* If you have INI entries, uncomment these lines 
	ZEND_INIT_MODULE_GLOBALS(pdo_sqlite, php_pdo_sqlite_init_globals, NULL);
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(pdo_sqlite)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(pdo_sqlite)
{
	return php_pdo_register_driver(&pdo_sqlite_driver);
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(pdo_sqlite)
{
	php_pdo_unregister_driver(&pdo_sqlite_driver);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(pdo_sqlite)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "PDO Driver for SQLite 3.x", "enabled");
	php_info_print_table_row(2, "PECL Module version", PHP_PDO_SQLITE_MODULE_VERSION " $Id$");
	php_info_print_table_row(2, "SQLite Library", sqlite3_libversion());
	php_info_print_table_end();
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
