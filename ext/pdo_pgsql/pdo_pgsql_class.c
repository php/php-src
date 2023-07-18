
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
#include "../pdo/php_pdo.h"
#include "../pdo/php_pdo_driver.h"
#include "../pdo/php_pdo_int.h"
#include "php_pdo_pgsql.h"
#include "php_pdo_pgsql_int.h"
#include "zend_exceptions.h"

/* {{{ Escape a identifier for insertion into a text field	*/
PHP_METHOD(PdoPgsql, escapeIdentifier)
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
		// TODO - currently this has no test, as it is unclear on what strings PQescapeIdentifier
		// can fail.
		// Also, "On error, PQescapeIdentifier returns NULL and a suitable message
		// is stored in the conn object." using the suitable message might be nice.
		zend_throw_exception_ex(php_pdo_get_exception(), 0, "Failed to escape identifier");
		RETURN_THROWS();
	}

	RETVAL_STRING(tmp);
	PQfreemem(tmp);
}
/* }}} */


/* {{{ Returns true if the copy worked fine or false if error */
PHP_METHOD(PdoPgsql, copyFromArray)
{
	pgsqlCopyFromArray_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */


/* {{{ Returns true if the copy worked fine or false if error */
PHP_METHOD(PdoPgsql, copyFromFile)
{
	pgsqlCopyFromFile_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Returns true if the copy worked fine or false if error */
PHP_METHOD(PdoPgsql, copyToFile)
{
	pgsqlCopyToFile_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);

}
/* }}} */

/* {{{ Returns true if the copy worked fine or false if error */
PHP_METHOD(PdoPgsql, copyToArray)
{
	pgsqlCopyToArray_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Creates a new large object, returning its identifier.  Must be called inside a transaction. */
PHP_METHOD(PdoPgsql, lobCreate)
{
	pgsqlLOBCreate_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Opens an existing large object stream.  Must be called inside a transaction. */
PHP_METHOD(PdoPgsql, lobOpen)
{
	pgsqlLOBOpen_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Deletes the large object identified by oid.  Must be called inside a transaction. */
PHP_METHOD(PdoPgsql, lobUnlink)
{
	pgsqlLOBUnlink_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Get asynchronous notification */
PHP_METHOD(PdoPgsql, getNotify)
{
	pgsqlGetNotify_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Get backend(server) pid */
PHP_METHOD(PdoPgsql, getPid)
{
	pgsqlGetPid_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */
