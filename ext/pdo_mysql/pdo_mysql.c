/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2009 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: George Schlossnagle <george@omniti.com>                      |
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
#include "php_pdo_mysql.h"
#include "php_pdo_mysql_int.h"

/* {{{ pdo_mysql_functions[] */
zend_function_entry pdo_mysql_functions[] = {
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ pdo_mysql_functions[] */
#if ZEND_MODULE_API_NO >= 20050922
static zend_module_dep pdo_mysql_deps[] = {
	ZEND_MOD_REQUIRED("pdo")
	{NULL, NULL, NULL}
};
#endif
/* }}} */

/* {{{ pdo_mysql_module_entry */
zend_module_entry pdo_mysql_module_entry = {
#if ZEND_MODULE_API_NO >= 20050922
	STANDARD_MODULE_HEADER_EX, NULL,
	pdo_mysql_deps,
#else
	STANDARD_MODULE_HEADER,
#endif
	"pdo_mysql",
	pdo_mysql_functions,
	PHP_MINIT(pdo_mysql),
	PHP_MSHUTDOWN(pdo_mysql),
	NULL,
	NULL,
	PHP_MINFO(pdo_mysql),
	"1.0.2",
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PDO_MYSQL
ZEND_GET_MODULE(pdo_mysql)
#endif

/* true global environment */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(pdo_mysql)
{
	REGISTER_PDO_CLASS_CONST_LONG("MYSQL_ATTR_USE_BUFFERED_QUERY", (long)PDO_MYSQL_ATTR_USE_BUFFERED_QUERY);
	REGISTER_PDO_CLASS_CONST_LONG("MYSQL_ATTR_LOCAL_INFILE", (long)PDO_MYSQL_ATTR_LOCAL_INFILE);
	REGISTER_PDO_CLASS_CONST_LONG("MYSQL_ATTR_INIT_COMMAND", (long)PDO_MYSQL_ATTR_INIT_COMMAND);
	REGISTER_PDO_CLASS_CONST_LONG("MYSQL_ATTR_READ_DEFAULT_FILE", (long)PDO_MYSQL_ATTR_READ_DEFAULT_FILE);
	REGISTER_PDO_CLASS_CONST_LONG("MYSQL_ATTR_READ_DEFAULT_GROUP", (long)PDO_MYSQL_ATTR_READ_DEFAULT_GROUP);
	REGISTER_PDO_CLASS_CONST_LONG("MYSQL_ATTR_MAX_BUFFER_SIZE", (long)PDO_MYSQL_ATTR_MAX_BUFFER_SIZE);
	REGISTER_PDO_CLASS_CONST_LONG("MYSQL_ATTR_DIRECT_QUERY", (long)PDO_MYSQL_ATTR_DIRECT_QUERY);

	return php_pdo_register_driver(&pdo_mysql_driver);
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(pdo_mysql)
{
	php_pdo_unregister_driver(&pdo_mysql_driver);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(pdo_mysql)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "PDO Driver for MySQL, client library version", mysql_get_client_info());
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
