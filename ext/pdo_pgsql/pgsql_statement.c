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
  | Author: Edin Kadribasic <edink@emini.dk>                             |
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
#include "php_pdo_pgsql.h"
#include "php_pdo_pgsql_int.h"

/* from postgresql/src/include/catalog/pg_type.h */
#define BOOLOID			16
#define INT8OID			20
#define INT2OID			21
#define INT4OID			23


static int pgsql_stmt_dtor(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_pgsql_stmt *S = (pdo_pgsql_stmt*)stmt->driver_data;
	int i;

	if (S->result) {
		/* free the resource */
		PQclear(S->result);
		S->result = NULL;
	}

	if (S->cursor_name) {
		pdo_pgsql_db_handle *H = S->H;
		char *q = NULL;
		PGresult *res;

		spprintf(&q, 0, "CLOSE %s", S->cursor_name);
		res = PQexec(H->server, q);
		efree(q);
		if (res) PQclear(res);
		efree(S->cursor_name);
		S->cursor_name = NULL;
	}
	
	if(S->cols) {
		efree(S->cols);
		S->cols = NULL;
	}
	efree(S);
	return 1;
}

static int pgsql_stmt_execute(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_dbh_t *dbh = stmt->dbh;
	pdo_pgsql_stmt *S = (pdo_pgsql_stmt*)stmt->driver_data;
	pdo_pgsql_db_handle *H = S->H;
	ExecStatusType status;

	if (stmt->executed) {
		/* ensure that we free any previous unfetched results */
		if(S->result) {
			PQclear(S->result);
			S->result = NULL;
		}
	}

	if (S->cursor_name) {
		char *q = NULL;
		spprintf(&q, 0, "DECLARE %s FOR %s", S->cursor_name, stmt->active_query_string);
		S->result = PQexec(H->server, q);
		efree(q);
	} else {
		S->result = PQexec(H->server, stmt->active_query_string);
	}
	status = PQresultStatus(S->result);

	if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK) {
		pdo_pgsql_error_stmt(stmt, status);
		return 0;
	}

	if(!stmt->executed) {
		stmt->column_count = (int) PQnfields(S->result);
		S->cols = ecalloc(stmt->column_count, sizeof(pdo_pgsql_column));
	}

	if (status == PGRES_COMMAND_OK) {
		stmt->row_count = (long)atoi(PQcmdTuples(S->result));
	} else {
		stmt->row_count = (long)PQntuples(S->result);
	}

	return 1;
}

static int pgsql_stmt_param_hook(pdo_stmt_t *stmt, struct pdo_bound_param_data *param,
		enum pdo_param_event event_type TSRMLS_DC)
{
	return 1;
}

static int pgsql_stmt_fetch(pdo_stmt_t *stmt,
	enum pdo_fetch_orientation ori, long offset TSRMLS_DC)
{
	pdo_pgsql_stmt *S = (pdo_pgsql_stmt*)stmt->driver_data;

	if (S->cursor_name) {
		char *ori_str = NULL;
		char *q = NULL;
		ExecStatusType status;

		switch (ori) {
			case PDO_FETCH_ORI_NEXT: 	ori_str = "NEXT"; break;
			case PDO_FETCH_ORI_PRIOR:	ori_str = "PRIOR"; break;
			case PDO_FETCH_ORI_REL:		ori_str = "RELATIVE"; break;
		}
		if (!ori_str) {
			return 0;
		}
		
		spprintf(&q, 0, "FETCH %s %d FROM %s", ori_str, offset, S->cursor_name);
		S->result = PQexec(S->H->server, q);
		status = PQresultStatus(S->result);

		if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK) {
			pdo_pgsql_error_stmt(stmt, status);
			return 0;
		}

		S->current_row = 1;
		return 1;	
		
	} else {
		if (S->current_row < stmt->row_count) {
			S->current_row++;
			return 1;
		} else {
			return 0;
		}
	}
}

static int pgsql_stmt_describe(pdo_stmt_t *stmt, int colno TSRMLS_DC)
{
	pdo_pgsql_stmt *S = (pdo_pgsql_stmt*)stmt->driver_data;
	struct pdo_column_data *cols = stmt->columns;
	
	if (!S->result) {
		return 0;
	}

	cols[colno].name = estrdup(PQfname(S->result, colno));
	cols[colno].namelen = strlen(cols[colno].name);
	cols[colno].maxlen = PQfsize(S->result, colno);
	cols[colno].precision = PQfmod(S->result, colno);
	S->cols[colno].pgsql_type = PQftype(S->result, colno);
	
	switch(S->cols[colno].pgsql_type) {

		case BOOLOID:
			cols[colno].param_type = PDO_PARAM_BOOL;
			break;

		case INT2OID:
		case INT4OID:
			cols[colno].param_type = PDO_PARAM_INT;
			break;

		case INT8OID:
			if (sizeof(long)>=8) {
				cols[colno].param_type = PDO_PARAM_INT;
			}
			break;

		default:
			cols[colno].param_type = PDO_PARAM_STR;
	}

	return 1;
}

static int pgsql_stmt_get_col(pdo_stmt_t *stmt, int colno, char **ptr, unsigned long *len TSRMLS_DC)
{
	pdo_pgsql_stmt *S = (pdo_pgsql_stmt*)stmt->driver_data;
	struct pdo_column_data *cols = stmt->columns;

	if (!S->result) {
		return 0;
	}

	/* We have already increased count by 1 in pgsql_stmt_fetch() */
	if (PQgetisnull(S->result, S->current_row - 1, colno)) { /* Check if we got NULL */
		*ptr = NULL;
		*len = 0;
	} else {
		*ptr = PQgetvalue(S->result, S->current_row - 1, colno);
		*len = PQgetlength(S->result, S->current_row - 1, colno);
		
		switch(cols[colno].param_type) {

			case PDO_PARAM_INT:
				S->cols[colno].intval = atol(*ptr);
				*ptr = (char *) &(S->cols[colno].intval);
				*len = sizeof(long);
				break;

			case PDO_PARAM_BOOL:
				S->cols[colno].boolval = **ptr == 't' ? 1: 0;
				*ptr = (char *) &(S->cols[colno].boolval);
				*len = sizeof(zend_bool);
				break;
		}
	}

	return 1;
}

struct pdo_stmt_methods pgsql_stmt_methods = {
	pgsql_stmt_dtor,
	pgsql_stmt_execute,
	pgsql_stmt_fetch,
	pgsql_stmt_describe,
	pgsql_stmt_get_col,
	pgsql_stmt_param_hook
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
