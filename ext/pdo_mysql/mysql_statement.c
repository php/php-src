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
  | Author: George Schlossnagle <george@omniti.com>                      |
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
#include "php_pdo_mysql.h"
#include "php_pdo_mysql_int.h"


static int mysql_stmt_dtor(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_mysql_stmt *S = (pdo_mysql_stmt*)stmt->driver_data;
	int i;

	if (S->result) {
		/* free the resource */
		mysql_free_result(S->result);
		S->result = NULL;
	}
	if(S->cols) {
		efree(S->cols);
		S->cols = NULL;
	}
	efree(S);
	return 1;
}

static int mysql_stmt_execute(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_dbh_t *dbh = stmt->dbh;
	pdo_mysql_stmt *S = (pdo_mysql_stmt*)stmt->driver_data;
	pdo_mysql_db_handle *H = S->H;

	if (stmt->executed) {
		/* ensure that we free any previous unfetched results */
		if(S->result) {
			mysql_free_result(S->result);
			S->result = NULL;
		}
	}
	if(mysql_real_query(H->server, stmt->active_query_string, 
           stmt->active_query_stringlen) != 0) 
	{
		H->last_err = S->last_err = mysql_errno(H->server);
		pdo_mysql_error("execute failed", S->last_err);
		return 0;
	}
	if((S->result = mysql_use_result(H->server)) == NULL) {
		H->last_err = S->last_err = mysql_errno(H->server);
		pdo_mysql_error("mysql_use_result() failed", S->last_err);
		return 0;
	}
	if(!stmt->executed) { 
		stmt->column_count = (int) mysql_num_fields(S->result);
		S->cols = ecalloc(stmt->column_count, sizeof(pdo_mysql_column));
	}
	stmt->row_count = (long)mysql_num_rows(S->result);
	return 1;
}

static int mysql_stmt_param_hook(pdo_stmt_t *stmt, struct pdo_bound_param_data *param,
		enum pdo_param_event event_type TSRMLS_DC)
{
	return 1;
}

static int mysql_stmt_fetch(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_mysql_stmt *S = (pdo_mysql_stmt*)stmt->driver_data;

	if((S->current_data = mysql_fetch_row(S->result)) == NULL) {
		/* there seems to be no way of distinguishing 'no data' from 'error' */
		return 0;
	} 
	S->current_lengths = mysql_fetch_lengths(S->result);
	return 1;	
}

static int mysql_stmt_describe(pdo_stmt_t *stmt, int colno TSRMLS_DC)
{
	pdo_mysql_stmt *S = (pdo_mysql_stmt*)stmt->driver_data;
	MYSQL_RES *R = S->result;
	MYSQL_FIELD *F;
	struct pdo_column_data *cols = stmt->columns;
	unsigned int num_fields, i;

	/* fetch all on demand, this seems easiest 
	** if we've been here before bail out 
	*/
	if(cols[0].name) {
		return 1;
	}
	num_fields = mysql_num_fields(R);
	F = mysql_fetch_fields(R);
	for(i=0; i < num_fields; i++) {
		int namelen;
		namelen = strlen(F[i].name);
		cols[i].precision = F[i].decimals;
		cols[i].maxlen = F[i].length;
		cols[i].namelen = namelen;
		/* FIXME where does this get freed? */
		cols[i].name = estrndup(F[i].name, namelen + 1);
		cols[i].param_type = PDO_PARAM_STR;
	}
	return 1;
}

static int mysql_stmt_get_col(pdo_stmt_t *stmt, int colno, char **ptr, unsigned long *len TSRMLS_DC)
{
	pdo_mysql_stmt *S = (pdo_mysql_stmt*)stmt->driver_data;
	if(S->current_data == NULL) {
		return 0;
	}
	if(colno >= mysql_num_fields(S->result)) {
		/* error invalid column */
		pdo_mysql_error("invalid column", 0);
		return 0;
	}
	*ptr = estrndup(S->current_data[colno], S->current_lengths[colno] +1);
	*len = S->current_lengths[colno];
	return 1;
}

struct pdo_stmt_methods mysql_stmt_methods = {
	mysql_stmt_dtor,
	mysql_stmt_execute,
	mysql_stmt_fetch,
	mysql_stmt_describe,
	mysql_stmt_get_col,
	mysql_stmt_param_hook
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
