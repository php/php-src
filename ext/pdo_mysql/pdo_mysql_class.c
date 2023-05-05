
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
  | Author: Danack
  +----------------------------------------------------------------------+
*/

/* The PDO Database Handle Class */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "../pdo/php_pdo.h"
#include "../pdo/php_pdo_driver.h"
#include "../pdo/php_pdo_int.h"
#include "php_pdo_mysql.h"
#include "php_pdo_mysql_int.h"

/* {{{ proto string PDO::mysqlGetWarningCount()
    Returns the number of SQL warnings during the execution of the last statement
*/
PHP_METHOD(PDOMySql, getWarningCount)
{
	pdo_dbh_t *dbh;
	pdo_mysql_db_handle *H;

	dbh = Z_PDO_DBH_P(ZEND_THIS);
	PDO_CONSTRUCT_CHECK;

	H = (pdo_mysql_db_handle *)dbh->driver_data;
	RETURN_LONG(mysql_warning_count(H->server));
}
/* }}} */
