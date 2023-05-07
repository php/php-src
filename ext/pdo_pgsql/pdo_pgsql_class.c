
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
#include "php_pdo_pgsql.h"
#include "php_pdo_pgsql_int.h"

//static void php_pgsql_escape_internal(INTERNAL_FUNCTION_PARAMETERS, int escape_literal) /* {{{ */ {
//	zend_string *from = NULL;
//	zval *pgsql_link = NULL;
//	PGconn *pgsql;
//	char *tmp;
//	pgsql_link_handle *link;
//
//	switch (ZEND_NUM_ARGS()) {
//		case 1:
//			if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &from) == FAILURE) {
//				RETURN_THROWS();
//			}
//			link = FETCH_DEFAULT_LINK();
//			CHECK_DEFAULT_LINK(link);
//			break;
//
//		default:
//			if (zend_parse_parameters(ZEND_NUM_ARGS(), "OS", &pgsql_link, pgsql_link_ce, &from) == FAILURE) {
//				RETURN_THROWS();
//			}
//			link = Z_PGSQL_LINK_P(pgsql_link);
//			CHECK_PGSQL_LINK(link);
//			break;
//	}
//
//	pgsql = link->conn;
//
//	if (escape_literal) {
//		tmp = PQescapeLiteral(pgsql, ZSTR_VAL(from), ZSTR_LEN(from));
//	} else {
//		tmp = PQescapeIdentifier(pgsql, ZSTR_VAL(from), ZSTR_LEN(from));
//	}
//	if (!tmp) {
//		php_error_docref(NULL, E_WARNING,"Failed to escape");
//		RETURN_FALSE;
//	}
//
//	RETVAL_STRING(tmp);
//	PQfreemem(tmp);
//}
///* }}} */

///* {{{ Escape parameter as string literal (i.e. parameter)	*/
//PHP_FUNCTION(pg_escape_literal)
//{
//	php_pgsql_escape_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
//}
///* }}} */

/* {{{ Escape a identifier for insertion into a text field	*/
PHP_METHOD(PDOPgSql, escapeIdentifier)
{
	zend_string *from = NULL;
	char *tmp;
	pdo_dbh_t *dbh;
	pdo_pgsql_db_handle *H;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &from) == FAILURE) {
		RETURN_THROWS();
	}

	dbh = Z_PDO_DBH_P(ZEND_THIS);
	PDO_CONSTRUCT_CHECK;
	PDO_DBH_CLEAR_ERR();

	/* Obtain db Handle */
	H = (pdo_pgsql_db_handle *)dbh->driver_data;

	tmp = PQescapeIdentifier(H->server, ZSTR_VAL(from), ZSTR_LEN(from));

	if (!tmp) {
		// TODO - exception
		php_error_docref(NULL, E_WARNING,"Failed to escape identifier");
		RETURN_FALSE;
	}

	RETVAL_STRING(tmp);
	PQfreemem(tmp);
}
/* }}} */


/* {{{ Returns true if the copy worked fine or false if error */
PHP_METHOD(PDOPgSql, copyFromArray)
{
	pgsqlCopyFromArray_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */


/* {{{ Returns true if the copy worked fine or false if error */
PHP_METHOD(PDOPgSql, copyFromFile)
{
	pgsqlCopyFromFile_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Returns true if the copy worked fine or false if error */
PHP_METHOD(PDOPgSql, copyToFile)
{
	pgsqlCopyToFile_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);

}
/* }}} */

/* {{{ Returns true if the copy worked fine or false if error */
PHP_METHOD(PDOPgSql, copyToArray)
{
	pgsqlCopyToArray_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Creates a new large object, returning its identifier.  Must be called inside a transaction. */
PHP_METHOD(PDOPgSql, lobCreate)
{
	pgsqlLOBCreate_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Opens an existing large object stream.  Must be called inside a transaction. */
PHP_METHOD(PDOPgSql, lobOpen)
{
	pgsqlLOBOpen_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Deletes the large object identified by oid.  Must be called inside a transaction. */
PHP_METHOD(PDOPgSql, lobUnlink)
{
	pgsqlLOBUnlink_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Get asynchronous notification */
PHP_METHOD(PDOPgSql, getNotify)
{
	pgsqlGetNotify_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Get backend(server) pid */
PHP_METHOD(PDOPgSql, getPid)
{
	pgsqlGetPid_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */
