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

void _odbc_error(pdo_dbh_t *dbh, char *what, PDO_ODBC_HSTMT stmt, const char *file, int line TSRMLS_DC) /* {{{ */
{
	RETCODE rc;
	SWORD	errmsgsize;
	pdo_odbc_db_handle *H = (pdo_odbc_db_handle*)dbh->driver_data;

	rc = SQLError(H->env, H->dbc, stmt, H->last_state, &H->last_error,
			H->last_err_msg, sizeof(H->last_err_msg)-1, &errmsgsize);

	H->last_err_msg[errmsgsize] = '\0';
	
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "(%s:%d) %s: %d %s [SQL State %s]",
			file, line, what, H->last_error, H->last_err_msg, H->last_state);
}
/* }}} */

static int odbc_handle_closer(pdo_dbh_t *dbh TSRMLS_DC)
{
	pdo_odbc_db_handle *H = (pdo_odbc_db_handle*)dbh->driver_data;
	
	if (H->dbc != SQL_NULL_HANDLE) {
		SQLEndTran(SQL_HANDLE_DBC, H->dbc, SQL_ROLLBACK);
	}

	SQLFreeHandle(SQL_HANDLE_STMT, H->dbc);
	SQLFreeHandle(SQL_HANDLE_ENV, H->env);
	pefree(H, dbh->is_persistent);

	return 0;
}

static int odbc_handle_preparer(pdo_dbh_t *dbh, const char *sql, long sql_len, pdo_stmt_t *stmt, long options, zval *driver_options TSRMLS_DC)
{
	RETCODE rc;
	pdo_odbc_db_handle *H = (pdo_odbc_db_handle *)dbh->driver_data;
	pdo_odbc_stmt *S = ecalloc(1, sizeof(*S));

	S->H = H;
	
	rc = SQLAllocHandle(SQL_HANDLE_STMT, H->dbc, &S->stmt);

	if (rc == SQL_INVALID_HANDLE || rc == SQL_ERROR) {
		efree(S);
		odbc_error(dbh, "SQLAllocStmt", SQL_NULL_HSTMT);
		return 0;
	}

	rc = SQLPrepare(S->stmt, (char*)sql, SQL_NTS);

	if (rc != SQL_SUCCESS) {
		odbc_error(dbh, "SQLPrepare", S->stmt);
	}

	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		SQLFreeHandle(SQL_HANDLE_STMT, S->stmt);
		return 0;
	}
	
	stmt->driver_data = S;
	stmt->methods = &odbc_stmt_methods;
	
	return 1;
}

static int odbc_handle_doer(pdo_dbh_t *dbh, const char *sql TSRMLS_DC)
{
	pdo_odbc_db_handle *H = (pdo_odbc_db_handle *)dbh->driver_data;

	return 0;
}

static int odbc_handle_quoter(pdo_dbh_t *dbh, const char *unquoted, int unquotedlen, char **quoted, int *quotedlen  TSRMLS_DC)
{
	pdo_odbc_db_handle *H = (pdo_odbc_db_handle *)dbh->driver_data;

	return 0;
}

static int odbc_handle_begin(pdo_dbh_t *dbh TSRMLS_DC)
{
	/* with ODBC, there is nothing special to be done */
	return 1;
}

static int odbc_handle_commit(pdo_dbh_t *dbh TSRMLS_DC)
{
	pdo_odbc_db_handle *H = (pdo_odbc_db_handle *)dbh->driver_data;
	RETCODE rc;

	rc = SQLEndTran(SQL_HANDLE_DBC, H->dbc, SQL_COMMIT);

	if (rc != SQL_SUCCESS) {
		odbc_error(dbh, "SQLEndTran: Commit", SQL_NULL_HSTMT);

		if (rc != SQL_SUCCESS_WITH_INFO) {
			return 0;
		}
	}
	return 1;
}

static int odbc_handle_rollback(pdo_dbh_t *dbh TSRMLS_DC)
{
	pdo_odbc_db_handle *H = (pdo_odbc_db_handle *)dbh->driver_data;
	RETCODE rc;

	rc = SQLEndTran(SQL_HANDLE_DBC, H->dbc, SQL_ROLLBACK);

	if (rc != SQL_SUCCESS) {
		odbc_error(dbh, "SQLEndTran: Rollback", SQL_NULL_HSTMT);

		if (rc != SQL_SUCCESS_WITH_INFO) {
			return 0;
		}
	}
	return 1;

}



static struct pdo_dbh_methods odbc_methods = {
	odbc_handle_closer,
	odbc_handle_preparer,
	odbc_handle_doer,
	odbc_handle_quoter,
	odbc_handle_begin,
	odbc_handle_commit,
	odbc_handle_rollback,
};

static int pdo_odbc_handle_factory(pdo_dbh_t *dbh, zval *driver_options TSRMLS_DC) /* {{{ */
{
	pdo_odbc_db_handle *H;
	RETCODE rc;
	int use_direct = 0;

	H = pecalloc(1, sizeof(*H), dbh->is_persistent);

	dbh->driver_data = H;
	
	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &H->env);
	rc = SQLSetEnvAttr(H->env, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);

	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		odbc_error(dbh, "SQLSetEnvAttr: ODBC3", SQL_NULL_HSTMT);
		odbc_handle_closer(dbh TSRMLS_CC);
		return 0;
	}
	
	rc = SQLAllocHandle(SQL_HANDLE_DBC, H->env, &H->dbc);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		odbc_error(dbh, "SQLAllocHandle (DBC)", SQL_NULL_HSTMT);
		odbc_handle_closer(dbh TSRMLS_CC);
		return 0;
	}

	if (!dbh->auto_commit) {
		rc = SQLSetConnectAttr(H->dbc, SQL_ATTR_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF, SQL_IS_UINTEGER);
		if (rc != SQL_SUCCESS) {
			odbc_error(dbh, "SQLSetConnectAttr AUTOCOMMIT = OFF", SQL_NULL_HSTMT);
			odbc_handle_closer(dbh TSRMLS_CC);
			return 0;
		}
	}
	
	if (strchr(dbh->data_source, ';')) {
		char dsnbuf[1024];
		short dsnbuflen;

		use_direct = 1;

		/* Force UID and PWD to be set in the DSN */
		if (*dbh->username && !strstr(dbh->data_source, "uid") && !strstr(dbh->data_source, "UID")) {
			char *dsn = pemalloc(strlen(dbh->data_source) + strlen(dbh->username) + strlen(dbh->password) + sizeof(";UID=;PWD="), dbh->is_persistent);
			sprintf(dsn, "%s;UID=%s;PWD=%s", dbh->data_source, dbh->username, dbh->password);
			pefree((char*)dbh->data_source, dbh->is_persistent);
			dbh->data_source = dsn;
		}

		rc = SQLDriverConnect(H->dbc, NULL, (char*)dbh->data_source, strlen(dbh->data_source),
				dsnbuf, sizeof(dsnbuf)-1, &dsnbuflen, SQL_DRIVER_NOPROMPT);
	}
	if (!use_direct) {
		rc = SQLConnect(H->dbc, (char*)dbh->data_source, SQL_NTS, dbh->username, SQL_NTS, dbh->password, SQL_NTS);
	}

	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		odbc_error(dbh, use_direct ? "SQLDriverConnect" : "SQLConnect", SQL_NULL_HSTMT);
		odbc_handle_closer(dbh TSRMLS_CC);
		return 0;
	}

	/* TODO: if we want to play nicely, we should check to see if the driver supports ODBC v3 or not */

	dbh->methods = &odbc_methods;
	dbh->alloc_own_columns = 1;
	dbh->supports_placeholders = 1;
	
	return 1;
}
/* }}} */

pdo_driver_t pdo_odbc_driver = {
	PDO_DRIVER_HEADER(odbc),
	pdo_odbc_handle_factory
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
