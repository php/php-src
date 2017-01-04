/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2017 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Edin Kadribasic <edink@emini.dk>                             |
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
#include "php_pdo_pgsql.h"
#include "php_pdo_pgsql_int.h"

#ifdef HAVE_PG_CONFIG_H
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#include <pg_config.h>
#endif

/* {{{ pdo_pgsql_functions[] */
const zend_function_entry pdo_pgsql_functions[] = {
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ pdo_sqlite_deps
 */
#if ZEND_MODULE_API_NO >= 20050922
static const zend_module_dep pdo_pgsql_deps[] = {
	ZEND_MOD_REQUIRED("pdo")
	ZEND_MOD_END
};
#endif
/* }}} */

/* {{{ pdo_pgsql_module_entry */
zend_module_entry pdo_pgsql_module_entry = {
	STANDARD_MODULE_HEADER_EX, NULL,
	pdo_pgsql_deps,
	"pdo_pgsql",
	pdo_pgsql_functions,
	PHP_MINIT(pdo_pgsql),
	PHP_MSHUTDOWN(pdo_pgsql),
	NULL,
	NULL,
	PHP_MINFO(pdo_pgsql),
	PHP_PDO_PGSQL_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PDO_PGSQL
ZEND_GET_MODULE(pdo_pgsql)
#endif

/* true global environment */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(pdo_pgsql)
{
	REGISTER_PDO_CLASS_CONST_LONG("PGSQL_ATTR_DISABLE_PREPARES", PDO_PGSQL_ATTR_DISABLE_PREPARES);
	REGISTER_PDO_CLASS_CONST_LONG("PGSQL_TRANSACTION_IDLE", (zend_long)PGSQL_TRANSACTION_IDLE);
	REGISTER_PDO_CLASS_CONST_LONG("PGSQL_TRANSACTION_ACTIVE", (zend_long)PGSQL_TRANSACTION_ACTIVE);
	REGISTER_PDO_CLASS_CONST_LONG("PGSQL_TRANSACTION_INTRANS", (zend_long)PGSQL_TRANSACTION_INTRANS);
	REGISTER_PDO_CLASS_CONST_LONG("PGSQL_TRANSACTION_INERROR", (zend_long)PGSQL_TRANSACTION_INERROR);
	REGISTER_PDO_CLASS_CONST_LONG("PGSQL_TRANSACTION_UNKNOWN", (zend_long)PGSQL_TRANSACTION_UNKNOWN);

	php_pdo_register_driver(&pdo_pgsql_driver);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(pdo_pgsql)
{
	php_pdo_unregister_driver(&pdo_pgsql_driver);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(pdo_pgsql)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "PDO Driver for PostgreSQL", "enabled");
#ifdef HAVE_PG_CONFIG_H
	php_info_print_table_row(2, "PostgreSQL(libpq) Version", PG_VERSION);
#endif
	php_info_print_table_row(2, "Module version", pdo_pgsql_module_entry.version);
	php_info_print_table_row(2, "Revision", " $Id$ ");

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
