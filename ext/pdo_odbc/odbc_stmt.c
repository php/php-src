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

static void free_cols(pdo_stmt_t *stmt, pdo_odbc_stmt *S TSRMLS_DC)
{
	if (S->cols) {
		int i;

		for (i = 0; i < stmt->column_count; i++) {
			if (S->cols[i].data) {
				efree(S->cols[i].data);
			}
		}
		efree(S->cols);
		S->cols = NULL;
	}
}

static int odbc_stmt_dtor(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_odbc_stmt *S = (pdo_odbc_stmt*)stmt->driver_data;

	if (S->stmt != SQL_NULL_HANDLE) {
		if (stmt->executed) {
			SQLCancel(S->stmt);
		}
		SQLFreeHandle(SQL_HANDLE_STMT, S->stmt);
		S->stmt = SQL_NULL_HANDLE;
	}

	free_cols(stmt, S TSRMLS_CC);

	efree(S);

	return 1;
}

static int odbc_stmt_execute(pdo_stmt_t *stmt TSRMLS_DC)
{
	RETCODE rc;
	pdo_odbc_stmt *S = (pdo_odbc_stmt*)stmt->driver_data;
	char *buf = NULL;

	if (stmt->executed) {
		SQLCancel(S->stmt);
	}
	
	rc = SQLExecute(S->stmt);	

	while (rc == SQL_NEED_DATA) {
		struct pdo_bound_param_data *param;
		rc = SQLParamData(S->stmt, (SQLPOINTER*)&param);
		if (rc == SQL_NEED_DATA) {
			php_stream *stm;
			int len;

			if (Z_TYPE_P(param->parameter) != IS_RESOURCE) {
				/* they passed in a string */
				SQLPutData(S->stmt, Z_STRVAL_P(param->parameter), Z_STRLEN_P(param->parameter));
				continue;
			}

			php_stream_from_zval_no_verify(stm, &param->parameter);
			if (!stm) {
				/* shouldn't happen either */
				pdo_odbc_stmt_error("input LOB is no longer a stream");
				SQLCancel(S->stmt);
				if (buf) {
					efree(buf);
				}
				return 0;
			}

			/* now suck data from the stream and stick it into the database */
			if (buf == NULL) {
				buf = emalloc(8192);
			}

			do {
				len = php_stream_read(stm, buf, 8192);
				if (len == 0) {
					break;
				}
				SQLPutData(S->stmt, buf, len);
			} while (1);
		}
	}

	if (buf) {
		efree(buf);
	}

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
	pdo_odbc_param *P;
	
	/* we're only interested in parameters for prepared SQL right now */
	if (param->is_param) {

		switch (event_type) {
			case PDO_PARAM_EVT_FREE:
				P = param->driver_data;
				if (P) {
					efree(P);
				}
				break;

			case PDO_PARAM_EVT_ALLOC:
			{
				/* figure out what we're doing */
				switch (PDO_PARAM_TYPE(param->param_type)) {
					case PDO_PARAM_LOB:
						break;

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

				P = emalloc(sizeof(*P));
				param->driver_data = P;

				P->len = 0; /* is re-populated each EXEC_PRE */

				if ((param->param_type & PDO_PARAM_INPUT_OUTPUT) == PDO_PARAM_INPUT_OUTPUT) {
					P->paramtype = SQL_PARAM_INPUT_OUTPUT;
				} else if (param->max_value_len <= 0) {
					P->paramtype = SQL_PARAM_INPUT;
				} else {
					P->paramtype = SQL_PARAM_OUTPUT;
				}
				if (P->paramtype != SQL_PARAM_INPUT && PDO_PARAM_TYPE(param->param_type) != PDO_PARAM_LOB && param->max_value_len > Z_STRLEN_P(param->parameter)) {
					Z_STRVAL_P(param->parameter) = erealloc(Z_STRVAL_P(param->parameter), param->max_value_len + 1);
				}

				if (PDO_PARAM_TYPE(param->param_type) == PDO_PARAM_LOB && P->paramtype != SQL_PARAM_INPUT) {
					pdo_odbc_stmt_error("Can't bind a lob for output");
					return 0;
				}

				rc = SQLBindParameter(S->stmt, param->paramno+1,
						P->paramtype, ctype, sqltype, precision, scale,
						P->paramtype == SQL_PARAM_INPUT && 
							PDO_PARAM_TYPE(param->param_type) == PDO_PARAM_LOB ?
								(SQLPOINTER)param :
								Z_STRVAL_P(param->parameter),
						param->max_value_len <= 0 ? 0 : param->max_value_len,
						&P->len
						);
	
				if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) {
					return 1;
				}
				pdo_odbc_stmt_error("SQLBindParameter");
				return 0;
			}

			case PDO_PARAM_EVT_EXEC_PRE:
				P = param->driver_data;
				if (PDO_PARAM_TYPE(param->param_type) == PDO_PARAM_LOB) {
					if (Z_TYPE_P(param->parameter) == IS_RESOURCE) {
						php_stream *stm;
						php_stream_statbuf sb;

						php_stream_from_zval_no_verify(stm, &param->parameter);

						if (!stm) {
							return 0;
						}

						if (0 == php_stream_stat(stm, &sb)) {
							P->len = SQL_LEN_DATA_AT_EXEC(sb.sb.st_size);
						} else {
							P->len = SQL_LEN_DATA_AT_EXEC(0);
						}
					} else {
						convert_to_string(param->parameter);
						P->len = SQL_LEN_DATA_AT_EXEC(Z_STRLEN_P(param->parameter));
					}
				} else {
					P->len = Z_STRLEN_P(param->parameter);
				}
				return 1;
			
			case PDO_PARAM_EVT_EXEC_POST:
				if (Z_TYPE_P(param->parameter) == IS_STRING) {
					P = param->driver_data;
					Z_STRLEN_P(param->parameter) = P->len;
					Z_STRVAL_P(param->parameter)[P->len] = '\0';
				}
				return 1;
		}
	}
	return 1;
}

static int odbc_stmt_fetch(pdo_stmt_t *stmt,
	enum pdo_fetch_orientation ori, long offset TSRMLS_DC)
{
	pdo_odbc_stmt *S = (pdo_odbc_stmt*)stmt->driver_data;
	RETCODE rc;
	SQLSMALLINT odbcori;

	switch (ori) {
		case PDO_FETCH_ORI_NEXT:	odbcori = SQL_FETCH_NEXT; break;
		case PDO_FETCH_ORI_PRIOR:	odbcori = SQL_FETCH_PRIOR; break;
		case PDO_FETCH_ORI_FIRST:	odbcori = SQL_FETCH_FIRST; break;
		case PDO_FETCH_ORI_LAST:	odbcori = SQL_FETCH_LAST; break;
		case PDO_FETCH_ORI_ABS:		odbcori = SQL_FETCH_ABSOLUTE; break;
		case PDO_FETCH_ORI_REL:		odbcori = SQL_FETCH_RELATIVE; break;
		default: 
			strcpy(stmt->error_code, "HY106");
			return 0;
	}
	rc = SQLFetchScroll(S->stmt, odbcori, offset);

	if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) {
		return 1;
	}

	if (rc == SQL_NO_DATA) {
		return 0;
	}

	pdo_odbc_stmt_error("SQLFetchScroll");

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
	col->name = estrdup(S->cols[colno].colname);

	S->cols[colno].data = emalloc(colsize+1);

	/* returning data as a string */
	col->param_type = PDO_PARAM_STR;

	/* tell ODBC to put it straight into our buffer */
	rc = SQLBindCol(S->stmt, colno+1, SQL_C_CHAR, S->cols[colno].data,
			S->cols[colno].datalen+1, &S->cols[colno].fetched_len);
	
	return 1;
}

static int odbc_stmt_get_col(pdo_stmt_t *stmt, int colno, char **ptr, unsigned long *len, int *caller_frees TSRMLS_DC)
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
			strcpy(S->einfo.last_state, "IM0001");
			return -1;
	}
}

static int odbc_stmt_get_attr(pdo_stmt_t *stmt, long attr, zval *val TSRMLS_DC)
{
	SQLRETURN rc;
	pdo_odbc_stmt *S = (pdo_odbc_stmt*)stmt->driver_data;

	switch (attr) {
		case PDO_ATTR_CURSOR_NAME:
		{
			char buf[256];
			SQLSMALLINT len = 0;
			rc = SQLGetCursorName(S->stmt, buf, sizeof(buf), &len);

			if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) {
				ZVAL_STRINGL(val, buf, len, 1);
				return 1;
			}
			pdo_odbc_stmt_error("SQLGetCursorName");
			return 0;
		}

		default:
			strcpy(S->einfo.last_err_msg, "Unknown Attribute");
			S->einfo.what = "getAttribute";
			strcpy(S->einfo.last_state, "IM0001");
			return -1;
	}
}

static int odbc_stmt_next_rowset(pdo_stmt_t *stmt TSRMLS_DC)
{
	SQLRETURN rc;
	SQLSMALLINT colcount;
	pdo_odbc_stmt *S = (pdo_odbc_stmt*)stmt->driver_data;

	free_cols(stmt, S TSRMLS_CC);

	rc = SQLMoreResults(S->stmt);

	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		return 0;
	}

	/* how many columns do we have ? */
	SQLNumResultCols(S->stmt, &colcount);
	stmt->column_count = (int)colcount;
	S->cols = ecalloc(colcount, sizeof(pdo_odbc_column));

	return 1;
}

struct pdo_stmt_methods odbc_stmt_methods = {
	odbc_stmt_dtor,
	odbc_stmt_execute,
	odbc_stmt_fetch,
	odbc_stmt_describe,
	odbc_stmt_get_col,
	odbc_stmt_param_hook,
	odbc_stmt_set_param,
	odbc_stmt_get_attr, /* get attr */
	NULL, /* get column meta */
	odbc_stmt_next_rowset
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
