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

static int odbc_stmt_dtor(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_odbc_stmt *S = (pdo_odbc_stmt*)stmt->driver_data;
	int i;

	if (S->stmt != SQL_NULL_HANDLE) {
		if (stmt->executed) {
			SQLCancel(S->stmt);
		}
		SQLFreeHandle(SQL_HANDLE_STMT, S->stmt);
		S->stmt = SQL_NULL_HANDLE;
	}

	if (S->cols) {
		for (i = 0; i < stmt->column_count; i++) {
			if (S->cols[i].data) {
				efree(S->cols[i].data);
			}
		}
		efree(S->cols);
		S->cols = NULL;
	}
	efree(S);

	return 1;
}

static int odbc_stmt_execute(pdo_stmt_t *stmt TSRMLS_DC)
{
	RETCODE rc;
	pdo_odbc_stmt *S = (pdo_odbc_stmt*)stmt->driver_data;

	if (stmt->executed) {
		SQLCancel(S->stmt);
	}
	
	rc = SQLExecute(S->stmt);	

	switch (rc) {
		case SQL_SUCCESS:
			break;
		case SQL_NO_DATA_FOUND:
		case SQL_SUCCESS_WITH_INFO:
			pdo_odbc_stmt_error("SQLExecute");
			break;

		default:
			pdo_odbc_stmt_error("SQLExecute");
			return 0;
	}

	if (!stmt->executed) {
		/* do first-time-only definition of bind/mapping stuff */
		SQLSMALLINT colcount;

		/* how many columns do we have ? */
		SQLNumResultCols(S->stmt, &colcount);

		stmt->column_count = (int)colcount;

		S->cols = ecalloc(colcount, sizeof(pdo_odbc_column));
	}

	return 1;
}

static int odbc_stmt_param_hook(pdo_stmt_t *stmt, struct pdo_bound_param_data *param,
		enum pdo_param_event event_type TSRMLS_DC)
{
	pdo_odbc_stmt *S = (pdo_odbc_stmt*)stmt->driver_data;
	RETCODE rc;
	SWORD sqltype, ctype, scale, nullable;
	UDWORD precision;
	
	/* we're only interested in parameters for prepared SQL right now */
	if (param->is_param) {

		switch (event_type) {
			case PDO_PARAM_EVT_ALLOC:
			
				/* figure out what we're doing */
				switch (param->param_type) {
					case PDO_PARAM_LOB:
					case PDO_PARAM_STMT:
						return 0;

					case PDO_PARAM_STR:
					default:
						convert_to_string(param->parameter);
				}

				SQLDescribeParam(S->stmt, param->paramno+1, &sqltype, &precision, &scale, &nullable);
				if (sqltype == SQL_BINARY || sqltype == SQL_VARBINARY || sqltype == SQL_LONGVARBINARY) {
					ctype = SQL_C_BINARY;
				} else {
					ctype = SQL_C_CHAR;
				}

				rc = SQLBindParameter(S->stmt, param->paramno+1,
						SQL_PARAM_INPUT, ctype, sqltype, precision, scale,
						Z_STRVAL_P(param->parameter), 0, 
						/* XXX: this has the wrong type for DB2 */
						&Z_STRLEN_P(param->parameter)
						);
				
				return 1;
		}
	}
	return 1;
}

static int odbc_stmt_fetch(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_odbc_stmt *S = (pdo_odbc_stmt*)stmt->driver_data;
	RETCODE rc;

	rc = SQLFetch(S->stmt);

	if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) {
		return 1;
	}

	if (rc == SQL_NO_DATA) {
		return 0;
	}

	pdo_odbc_stmt_error("SQLFetch");

	return 0;
}

static int odbc_stmt_describe(pdo_stmt_t *stmt, int colno TSRMLS_DC)
{
	pdo_odbc_stmt *S = (pdo_odbc_stmt*)stmt->driver_data;
	struct pdo_column_data *col = &stmt->columns[colno];
	zend_bool dyn = FALSE;
	RETCODE rc;
	SWORD	colnamelen;
	SDWORD	colsize;

	rc = SQLDescribeCol(S->stmt, colno+1, S->cols[colno].colname,
			sizeof(S->cols[colno].colname)-1, &colnamelen,
			&S->cols[colno].coltype, &colsize, NULL, NULL);

	col->maxlen = S->cols[colno].datalen = colsize;
	col->namelen = colnamelen;
	col->name = S->cols[colno].colname;

	S->cols[colno].data = emalloc(colsize+1);

	/* returning data as a string */
	col->param_type = PDO_PARAM_STR;

	/* tell ODBC to put it straight into our buffer */
	rc = SQLBindCol(S->stmt, colno+1, SQL_C_CHAR, S->cols[colno].data,
			S->cols[colno].datalen, &S->cols[colno].fetched_len);
	
	return 1;
}

static int odbc_stmt_get_col(pdo_stmt_t *stmt, int colno, char **ptr, unsigned long *len TSRMLS_DC)
{
	pdo_odbc_stmt *S = (pdo_odbc_stmt*)stmt->driver_data;
	pdo_odbc_column *C = &S->cols[colno];

	/* check the indicator to ensure that the data is intact */
	if (C->fetched_len == SQL_NULL_DATA) {
		/* A NULL value */
		*ptr = NULL;
		*len = 0;
		return 1;
	} else if (C->fetched_len >= 0) {
		/* it was stored perfectly */
		*ptr = C->data;
		*len = C->fetched_len;
		return 1;
	} else {
		/* no data? */
		*ptr = NULL;
		*len = 0;
		return 1;
	}
}

static int odbc_stmt_set_param(pdo_stmt_t *stmt, long attr, zval *val TSRMLS_DC)
{
	SQLRETURN rc;
	pdo_odbc_stmt *S = (pdo_odbc_stmt*)stmt->driver_data;

	switch (attr) {
		case PDO_ATTR_CURSOR_NAME:
			convert_to_string(val);
			rc = SQLSetCursorName(S->stmt, Z_STRVAL_P(val), Z_STRLEN_P(val));

			if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) {
				return 1;
			}
			pdo_odbc_stmt_error("SQLSetCursorName");
			return 0;

		default:
			strcpy(S->einfo.last_err_msg, "Unknown Attribute");
			S->einfo.what = "setAttribute";
			stmt->error_code = PDO_ERR_NOT_IMPLEMENTED;
			S->einfo.last_state[0] = '\0';
			return -1;
	}


}

struct pdo_stmt_methods odbc_stmt_methods = {
	odbc_stmt_dtor,
	odbc_stmt_execute,
	odbc_stmt_fetch,
	odbc_stmt_describe,
	odbc_stmt_get_col,
	odbc_stmt_param_hook,
	odbc_stmt_set_param,
	NULL
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
