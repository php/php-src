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


static int pgsql_stmt_dtor(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_pgsql_stmt *S = (pdo_pgsql_stmt*)stmt->driver_data;
	int i;

	if (S->result) {
		/* free the resource */
		PQclear(S->result);
		S->result = NULL;
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

	S->result = PQexec(H->server, stmt->active_query_string);
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

static int pgsql_stmt_fetch(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_pgsql_stmt *S = (pdo_pgsql_stmt*)stmt->driver_data;

	if (S->current_row < stmt->row_count) {
		S->current_row++;
		return 1;
	} else {
		return 0;
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
	cols[colno].param_type = PDO_PARAM_STR;

	return 1;
}

static int pgsql_stmt_get_col(pdo_stmt_t *stmt, int colno, char **ptr, unsigned long *len TSRMLS_DC)
{
	pdo_pgsql_stmt *S = (pdo_pgsql_stmt*)stmt->driver_data;

	if (!S->result) {
		return 0;
	}

	/* We have already increased count by 1 in pgsql_stmt_fetch() */
	*ptr = PQgetvalue(S->result, S->current_row - 1, colno);
	*len = PQgetlength(S->result, S->current_row - 1, colno);

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
