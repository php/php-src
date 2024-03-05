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
  | Author: Edin Kadribasic <edink@emini.dk>                             |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "pdo/php_pdo.h"
#include "pdo/php_pdo_int.h"
#include "pdo/php_pdo_driver.h"
#include "php_pdo_pgsql.h"
#include "php_pdo_pgsql_int.h"
#include "pdo_pgsql_arginfo.h"

static zend_class_entry *PdoPgsql_ce;

/* {{{ pdo_sqlite_deps */
static const zend_module_dep pdo_pgsql_deps[] = {
	ZEND_MOD_REQUIRED("pdo")
	ZEND_MOD_END
};
/* }}} */

/* {{{ pdo_pgsql_module_entry */
zend_module_entry pdo_pgsql_module_entry = {
	STANDARD_MODULE_HEADER_EX, NULL,
	pdo_pgsql_deps,
	"pdo_pgsql",
	NULL,
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

/* Escape an identifier for insertion into a text field	*/
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
	if (H->server == NULL) {
		zend_throw_error(NULL, "PostgreSQL connection has already been closed");
		RETURN_THROWS();
	}

	tmp = PQescapeIdentifier(H->server, ZSTR_VAL(from), ZSTR_LEN(from));
	if (!tmp) {
		pdo_pgsql_error(dbh, PGRES_FATAL_ERROR, NULL);
		PDO_HANDLE_DBH_ERR();
		RETURN_THROWS();
	}

	RETVAL_STRING(tmp);
	PQfreemem(tmp);
}

/* Returns true if the copy worked fine or false if error */
PHP_METHOD(PdoPgsql, copyFromArray)
{
	pgsqlCopyFromArray_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/* Returns true if the copy worked fine or false if error */
PHP_METHOD(PdoPgsql, copyFromFile)
{
	pgsqlCopyFromFile_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/* Returns true if the copy worked fine or false if error */
PHP_METHOD(PdoPgsql, copyToFile)
{
	pgsqlCopyToFile_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/* Returns true if the copy worked fine or false if error */
PHP_METHOD(PdoPgsql, copyToArray)
{
	pgsqlCopyToArray_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/* Creates a new large object, returning its identifier.  Must be called inside a transaction. */
PHP_METHOD(PdoPgsql, lobCreate)
{
	pgsqlLOBCreate_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/* Opens an existing large object stream.  Must be called inside a transaction. */
PHP_METHOD(PdoPgsql, lobOpen)
{
	pgsqlLOBOpen_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/* Deletes the large object identified by oid.  Must be called inside a transaction. */
PHP_METHOD(PdoPgsql, lobUnlink)
{
	pgsqlLOBUnlink_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/* Get asynchronous notification */
PHP_METHOD(PdoPgsql, getNotify)
{
	pgsqlGetNotify_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/* Get backend(server) pid */
PHP_METHOD(PdoPgsql, getPid)
{
	pgsqlGetPid_internal(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/* true global environment */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(pdo_pgsql)
{
	REGISTER_PDO_CLASS_CONST_LONG("PGSQL_ATTR_DISABLE_PREPARES", PDO_PGSQL_ATTR_DISABLE_PREPARES);
	REGISTER_PDO_CLASS_CONST_LONG("PGSQL_TRANSACTION_IDLE", (zend_long)PGSQL_TRANSACTION_IDLE);
	REGISTER_PDO_CLASS_CONST_LONG("PGSQL_TRANSACTION_ACTIVE", (zend_long)PGSQL_TRANSACTION_ACTIVE);
	REGISTER_PDO_CLASS_CONST_LONG("PGSQL_TRANSACTION_INTRANS", (zend_long)PGSQL_TRANSACTION_INTRANS);
	REGISTER_PDO_CLASS_CONST_LONG("PGSQL_TRANSACTION_INERROR", (zend_long)PGSQL_TRANSACTION_INERROR);
	REGISTER_PDO_CLASS_CONST_LONG("PGSQL_TRANSACTION_UNKNOWN", (zend_long)PGSQL_TRANSACTION_UNKNOWN);

	PdoPgsql_ce = register_class_PdoPgsql(pdo_dbh_ce);
	PdoPgsql_ce->create_object = pdo_dbh_new;

	if (php_pdo_register_driver(&pdo_pgsql_driver) == FAILURE) {
		return FAILURE;
	}

	return php_pdo_register_driver_specific_ce(&pdo_pgsql_driver, PdoPgsql_ce);
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(pdo_pgsql)
{
	php_pdo_unregister_driver(&pdo_pgsql_driver);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(pdo_pgsql)
{
	char buf[16];

	php_info_print_table_start();
	php_info_print_table_row(2, "PDO Driver for PostgreSQL", "enabled");
	pdo_libpq_version(buf, sizeof(buf));
	php_info_print_table_row(2, "PostgreSQL(libpq) Version", buf);
	php_info_print_table_end();
}
/* }}} */
