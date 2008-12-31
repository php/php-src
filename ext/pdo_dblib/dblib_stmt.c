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

static void free_rows(pdo_dblib_stmt *S TSRMLS_DC)
{
	int i, j;
	
	for (i = 0; i < S->nrows; i++) {
		for (j = 0; j < S->ncols; j++) {
			pdo_dblib_colval *val = &S->rows[i] + j;
			if (val->data) {
				efree(val->data);
				val->data = NULL;
			}
		}
	}
	efree(S->rows);
	S->rows = NULL;
	S->nrows = 0;
}

static int pdo_dblib_stmt_dtor(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_dblib_stmt *S = (pdo_dblib_stmt*)stmt->driver_data;

	if (S->rows) {
		free_rows(S TSRMLS_CC);
	}
	if (S->cols) {
		efree(S->cols);
	}
	efree(S);

	return 1;
}

static int pdo_dblib_stmt_execute(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_dbh_t *dbh = stmt->dbh;
	pdo_dblib_stmt *S = (pdo_dblib_stmt*)stmt->driver_data;
	pdo_dblib_db_handle *H = S->H;
	RETCODE resret, ret;
	int i, j;
	int arows;
	unsigned int size;
	
	dbsetuserdata(H->link, &S->err);

	if (S->rows) {
		/* clean them up */
		free_rows(S TSRMLS_CC);
	}

	if (FAIL == dbcmd(H->link, stmt->active_query_string)) {
		return 0;
	}
	if (FAIL == dbsqlexec(H->link)) {
		return 0;
	}
	
	resret = dbresults(H->link);
	if (resret == FAIL) {
		return 0;
	}

	ret = dbnextrow(H->link);

	stmt->row_count = DBCOUNT(H->link);

    if (ret == NO_MORE_ROWS) {
       return 1;
    }
    
	if (!S->cols) {
		S->ncols = dbnumcols(H->link);

		if (S->ncols <= 0) {
			return 1;
		}

		S->cols = ecalloc(S->ncols, sizeof(pdo_dblib_col));
		stmt->column_count = S->ncols;
	
		for (i = 0, j = 0; i < S->ncols; i++) {
			S->cols[i].coltype = dbcoltype(H->link, i+1);
			S->cols[i].name = dbcolname(H->link, i+1);
			if (S->cols[i].name) {
				S->cols[i].name = estrdup(S->cols[i].name);
			} else if (j) {
				spprintf(&S->cols[i].name, 0, "computed%d", j++);
			} else {
				S->cols[i].name = estrdup("computed");
				j++;
			}
			S->cols[i].source = dbcolsource(H->link, i+1);
			S->cols[i].source = estrdup(S->cols[i].source ? S->cols[i].source : "");
			S->cols[i].maxlen = dbcollen(H->link, i+1);
		}
	}

	arows = 100;
	size = S->ncols * sizeof(pdo_dblib_colval);
	S->rows = safe_emalloc(arows, size, 0);

	/* let's fetch all the data */
	do {
		if (S->nrows >= arows) {
			arows *= 2;
			S->rows = erealloc(S->rows, arows * size);
		}
		for (i = 0; i < S->ncols; i++) {
			pdo_dblib_colval *val = &S->rows[S->nrows * S->ncols + i];

			if (dbdatlen(H->link, i+1) == 0 && dbdata(H->link, i+1) == NULL) {
				val->len = 0;
				val->data = NULL;
			} else {
				switch (S->cols[i].coltype) {
					case SQLCHAR:
					case SQLTEXT:
					case SQLVARBINARY:
					case SQLBINARY:
					case SQLIMAGE:
						val->len = dbdatlen(H->link, i+1);
						val->data = emalloc(val->len + 1);
						memcpy(val->data, dbdata(H->link, i+1), val->len);
						val->data[val->len] = '\0';
						break;

					default:
						if (dbwillconvert(S->cols[i].coltype, SQLCHAR)) {
							val->len = 32 + (2 * dbdatlen(H->link, i+1));
							val->data = emalloc(val->len);

							val->len = dbconvert(NULL, S->cols[i].coltype, dbdata(H->link, i+1),
									dbdatlen(H->link, i+1), SQLCHAR, val->data, val->len);

							if (val->len >= 0) {
								val->data[val->len] = '\0';
							}
						} else {
							val->len = 0;
							val->data = NULL;
						}
				}
			}
		}

		S->nrows++;

		ret = dbnextrow(H->link);

		if (ret == BUF_FULL) {
			dbclrbuf(H->link, DBLASTROW(H->link)-1);
		}
	} while (ret != FAIL && ret != NO_MORE_ROWS);

	if (resret != NO_MORE_RESULTS) {
		/* there are additional result sets available */
		dbresults(H->link);
		/* cancel pending rows */
		dbcanquery(H->link);

		/* TODO: figure out a sane solution */
	}

	S->current = -1;
		
	return 1;	
}

static int pdo_dblib_stmt_fetch(pdo_stmt_t *stmt,
	enum pdo_fetch_orientation ori, long offset TSRMLS_DC)
{
	pdo_dblib_stmt *S = (pdo_dblib_stmt*)stmt->driver_data;

	if (!S->rows) {
		return 0;
	}
	
	if (++S->current < S->nrows) {
		return 1;
	}

	return 0;
}

static int pdo_dblib_stmt_describe(pdo_stmt_t *stmt, int colno TSRMLS_DC)
{
	pdo_dblib_stmt *S = (pdo_dblib_stmt*)stmt->driver_data;
	struct pdo_column_data *col = &stmt->columns[colno];

	if (!S->rows) {
		return 0;
	}

	col->maxlen = S->cols[colno].maxlen;	
	col->namelen = strlen(S->cols[colno].name);	
	col->name = estrdup(S->cols[colno].name);
	col->param_type = PDO_PARAM_STR;
		
	return 1;
}

static int pdo_dblib_stmt_get_col(pdo_stmt_t *stmt, int colno, char **ptr,
	 unsigned long *len, int *caller_frees TSRMLS_DC)
{
	pdo_dblib_stmt *S = (pdo_dblib_stmt*)stmt->driver_data;
	pdo_dblib_colval *val = &S->rows[S->current * S->ncols + colno];

	*ptr = val->data;
	*len = val->len;
	return 1;
}

static int pdo_dblib_stmt_param_hook(pdo_stmt_t *stmt, struct pdo_bound_param_data *param,
		enum pdo_param_event event_type TSRMLS_DC)
{
	return 1;
}

static int dblib_dblib_stmt_cursor_closer(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_dblib_stmt *S = (pdo_dblib_stmt*)stmt->driver_data;

	if (S->rows) {
		free_rows(S TSRMLS_CC);
		S->rows = NULL;
	}

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
	NULL, /* nextrow */
	dblib_dblib_stmt_cursor_closer
};

