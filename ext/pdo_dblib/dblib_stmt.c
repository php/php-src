/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2010 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wez Furlong <wez@php.net>                                    |
  |         Frank M. Kromann <frank@kromann.info>                        |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "pdo/php_pdo.h"
#include "pdo/php_pdo_driver.h"
#include "php_pdo_dblib.h"
#include "php_pdo_dblib_int.h"
#include "zend_exceptions.h"

static int dblib_dblib_stmt_cursor_closer(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_dblib_stmt *S = (pdo_dblib_stmt*)stmt->driver_data;
	pdo_dblib_db_handle *H = S->H;

	/* Cancel any pending results */
	dbcancel(H->link);

	efree(stmt->columns);
	stmt->columns = NULL;
	
	return 1;
}

static int pdo_dblib_stmt_dtor(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_dblib_stmt *S = (pdo_dblib_stmt*)stmt->driver_data;

	dblib_dblib_stmt_cursor_closer(stmt TSRMLS_CC);

	efree(S);
		
	return 1;
}

static int pdo_dblib_stmt_next_rowset(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_dblib_stmt *S = (pdo_dblib_stmt*)stmt->driver_data;
	pdo_dblib_db_handle *H = S->H;
	RETCODE ret;
	
	ret = dbresults(H->link);
	
	if (ret == FAIL || ret == NO_MORE_RESULTS) {
		return 0;
	}
	
	stmt->row_count = DBCOUNT(H->link);
	stmt->column_count = dbnumcols(H->link);
	
	return 1;
}

static int pdo_dblib_stmt_execute(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_dblib_stmt *S = (pdo_dblib_stmt*)stmt->driver_data;
	pdo_dblib_db_handle *H = S->H;
	RETCODE ret;
	
	dbsetuserdata(H->link, (BYTE*) &S->err);
	
	if (FAIL == dbcmd(H->link, stmt->active_query_string)) {
		return 0;
	}
	
	if (FAIL == dbsqlexec(H->link)) {
		return 0;
	}
	
	ret = pdo_dblib_stmt_next_rowset(stmt TSRMLS_CC);
	
	if (ret == 0) {
		return 0;
	}
	
	stmt->row_count = DBCOUNT(H->link);
	stmt->column_count = dbnumcols(H->link);
	
	return 1;
}

static int pdo_dblib_stmt_fetch(pdo_stmt_t *stmt,
	enum pdo_fetch_orientation ori, long offset TSRMLS_DC)
{
	
	RETCODE ret;
	
	pdo_dblib_stmt *S = (pdo_dblib_stmt*)stmt->driver_data;
	pdo_dblib_db_handle *H = S->H;
	
	ret = dbnextrow(H->link);
	
	if (ret == FAIL || ret == NO_MORE_ROWS) {
		return 0;
	}
	
	return 1;	
}

static int pdo_dblib_stmt_describe(pdo_stmt_t *stmt, int colno TSRMLS_DC)
{
	pdo_dblib_stmt *S = (pdo_dblib_stmt*)stmt->driver_data;
	pdo_dblib_db_handle *H = S->H;
	
	struct pdo_column_data *col = &stmt->columns[colno];
	
	col->name = (char*)dbcolname(H->link, colno+1);
	col->maxlen = dbcollen(H->link, colno+1);
	col->namelen = strlen(col->name);
	col->param_type = PDO_PARAM_STR;
		
	return 1;
}

static int pdo_dblib_stmt_get_col(pdo_stmt_t *stmt, int colno, char **ptr,
	 unsigned long *len, int *caller_frees TSRMLS_DC)
{
	
	pdo_dblib_stmt *S = (pdo_dblib_stmt*)stmt->driver_data;
	pdo_dblib_db_handle *H = S->H;
	
	int coltype;
	unsigned int tmp_len;
	char *tmp_ptr = NULL;
	
	coltype = dbcoltype(H->link, colno+1);
	
	*len = dbdatlen(H->link, colno+1);
	*ptr = dbdata(H->link, colno+1);
	
	if (*len == 0 && *ptr == NULL) {
		return 1;
	}
	
	switch (coltype) {
		case SQLCHAR:
		case SQLTEXT:
		case SQLVARBINARY:
		case SQLBINARY:
		case SQLIMAGE:
		case SQLVARCHAR:
			tmp_ptr = emalloc(*len + 1);
			memcpy(tmp_ptr, *ptr, *len);
			tmp_ptr[*len] = '\0';
			*ptr = tmp_ptr;
			++(*len);
			break;
		case SQLMONEY:
		case SQLMONEY4:
		case SQLMONEYN: {
			DBFLT8 money_value;
			dbconvert(NULL, coltype, *ptr, *len, SQLFLT8, (LPBYTE)&money_value, tmp_len);
			*len = spprintf(&tmp_ptr, 0, "%.4f", money_value);
			*ptr = tmp_ptr;
			break;
		}
		default:
			if (dbwillconvert(coltype, SQLCHAR)) {
				tmp_len = 32 + (2 * (*len));
				tmp_ptr = emalloc(tmp_len);

				*len = dbconvert(NULL, coltype, *ptr, *len, SQLCHAR, tmp_ptr, tmp_len);

				if (*len >= 0) {
					tmp_ptr[*len] = '\0';
				}
				*ptr = tmp_ptr;
				*len = tmp_len;
		} else {
			*len = 0;
			*ptr = NULL;
		}
	}

	*caller_frees = 1;

	return 1;
}

static int pdo_dblib_stmt_param_hook(pdo_stmt_t *stmt, struct pdo_bound_param_data *param,
		enum pdo_param_event event_type TSRMLS_DC)
{
	return 1;
}


struct pdo_stmt_methods dblib_stmt_methods = {
	pdo_dblib_stmt_dtor,
	pdo_dblib_stmt_execute,
	pdo_dblib_stmt_fetch,
	pdo_dblib_stmt_describe,
	pdo_dblib_stmt_get_col,
	pdo_dblib_stmt_param_hook,
	NULL, /* set attr */
	NULL, /* get attr */
	NULL, /* meta */
	pdo_dblib_stmt_next_rowset, /* nextrow */
	dblib_dblib_stmt_cursor_closer
};

