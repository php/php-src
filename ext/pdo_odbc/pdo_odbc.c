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
#include "php_pdo_odbc.h"
#include "php_pdo_odbc_int.h"

#ifdef PHP_WIN32
# define PDO_ODBC_TYPE	"Win32"
#endif

#ifndef PDO_ODBC_TYPE
# warning Please fix configure to give your ODBC libraries a name
# define PDO_ODBC_TYPE	"Unknown"
#endif

/* {{{ pdo_odbc_functions[] */
function_entry pdo_odbc_functions[] = {
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ pdo_odbc_module_entry */
zend_module_entry pdo_odbc_module_entry = {
	STANDARD_MODULE_HEADER,
	"PDO_ODBC",
	pdo_odbc_functions,
	PHP_MINIT(pdo_odbc),
	PHP_MSHUTDOWN(pdo_odbc),
	NULL,
	NULL,
	PHP_MINFO(pdo_odbc),
	"0.1",
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PDO_ODBC
ZEND_GET_MODULE(pdo_odbc)
#endif

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(pdo_odbc)
{
	php_pdo_register_driver(&pdo_odbc_driver);
	pdo_odbc_init_error_table();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(pdo_odbc)
{
	php_pdo_unregister_driver(&pdo_odbc_driver);
	pdo_odbc_fini_error_table();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(pdo_odbc)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "PDO Driver for ODBC (" PDO_ODBC_TYPE ")" , "enabled");
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
