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

int _pdo_pgsql_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt, int errcode, const char *file, int line TSRMLS_DC) /* {{{ */
{
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data;
	enum pdo_error_type *pdo_err = stmt ? &stmt->error_code : &dbh->error_code;
	pdo_pgsql_error_info *einfo = &H->einfo;
	char *errmsg = PQerrorMessage(H->server);

	einfo->errcode = errcode;
	einfo->file = file;
	einfo->line = line;

	if (einfo->errmsg) {
		efree(einfo->errmsg);
		einfo->errmsg = NULL;
	}

	switch (errcode) {
		case PGRES_EMPTY_QUERY:
			*pdo_err = PDO_ERR_SYNTAX;
			break;

		default:
			*pdo_err = PDO_ERR_CANT_MAP;
			break;
	}

	if (errmsg) {
		einfo->errmsg = estrdup(errmsg);
	}

	return errcode;
}
/* }}} */

static int pdo_pgsql_fetch_error_func(pdo_dbh_t *dbh, pdo_stmt_t *stmt, zval *info TSRMLS_DC) /* {{{ */
{
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data;
	pdo_pgsql_error_info *einfo = &H->einfo;

	if (einfo->errcode) {
		add_next_index_long(info, einfo->errcode);
		add_next_index_string(info, einfo->errmsg, 1);
	}

	return 1;
}
/* }}} */

static int pgsql_handle_closer(pdo_dbh_t *dbh TSRMLS_DC) /* {{{ */
{
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data;

	if (H->server) {
		PQfinish(H->server);
		H->server = NULL;
	}
	return 0;
}
/* }}} */

static int pgsql_handle_preparer(pdo_dbh_t *dbh, const char *sql, long sql_len, pdo_stmt_t *stmt, long options, zval *driver_options TSRMLS_DC)
{
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data;
	pdo_pgsql_stmt *S = ecalloc(1, sizeof(pdo_pgsql_stmt));

	S->H = H;
	stmt->driver_data = S;
	stmt->methods = &pgsql_stmt_methods;
	
	return 1;
}

static long pgsql_handle_doer(pdo_dbh_t *dbh, const char *sql, long sql_len TSRMLS_DC)
{
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data;
	PGresult *res;
	
	if (!(res = PQexec(H->server, sql))) {
		/* fatal error */
		pdo_pgsql_error(dbh, PGRES_FATAL_ERROR);
		return 0;
	} else {
		ExecStatusType qs = PQresultStatus(res);
		if (qs != PGRES_COMMAND_OK && qs != PGRES_TUPLES_OK) {
			pdo_pgsql_error(dbh, qs);
			return 0;
		}
		H->pgoid = PQoidValue(res);
	}

	return 1;
}

static int pgsql_handle_quoter(pdo_dbh_t *dbh, const char *unquoted, int unquotedlen, char **quoted, int *quotedlen  TSRMLS_DC)
{
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data;
	*quoted = emalloc(2*unquotedlen + 3);
	(*quoted)[0] = '"';
	*quotedlen = PQescapeString(*quoted + 1, unquoted, unquotedlen);
	(*quoted)[*quotedlen + 1] = '"';
	(*quoted)[*quotedlen + 2] = '\0';
	*quotedlen += 2;
	return 1;
}

static long pdo_pgsql_last_insert_id(pdo_dbh_t *dbh TSRMLS_DC)
{
	pdo_pgsql_db_handle *H = (pdo_pgsql_db_handle *)dbh->driver_data;
	
	if (H->pgoid == InvalidOid) {
		return -1;
	}

	return (long) H->pgoid;
}

static struct pdo_dbh_methods pgsql_methods = {
	pgsql_handle_closer,
	pgsql_handle_preparer,
	pgsql_handle_doer,
	pgsql_handle_quoter,
	NULL,
	NULL,
	NULL,
	NULL,
	pdo_pgsql_last_insert_id,
	pdo_pgsql_fetch_error_func
};

static int pdo_pgsql_handle_factory(pdo_dbh_t *dbh, zval *driver_options TSRMLS_DC) /* {{{ */
{
	pdo_pgsql_db_handle *H;
	int i, ret = 0;
	char *host, *port, *dbname;
	struct pdo_data_src_parser vars[] = {
		{ "host", "", 0 },
		{ "port", "", 0 },
		{ "dbname", "", 0 },
	};

	php_pdo_parse_data_source(dbh->data_source, dbh->data_source_len, vars, 3);

	H = pecalloc(1, sizeof(pdo_pgsql_db_handle), dbh->is_persistent);
	dbh->driver_data = H;
	
	/* handle for the server */
	host = vars[0].optval;
	port = vars[1].optval;
	dbname = vars[2].optval;

	H->server = 
		PQsetdbLogin(host,
					 port,
					 NULL, /* options */
					 NULL, /* tty */
					 dbname,
					 dbh->username,
					 dbh->password
					 );
	
	if (PQstatus(H->server) != CONNECTION_OK) {
		pdo_pgsql_error(dbh, PGRES_FATAL_ERROR);
		goto cleanup;
	}

	H->attached = 1;
	H->pgoid = -1;

	dbh->methods = &pgsql_methods;
	dbh->alloc_own_columns = 1;
	dbh->supports_placeholders = 1;
	dbh->emulate_prepare = 1;
	dbh->placeholders_can_be_strings = 1;
	dbh->max_escaped_char_length = 2;

	ret = 1;
	
cleanup:
	for (i = 0; i < sizeof(vars)/sizeof(vars[0]); i++) {
		if (vars[i].freeme) {
			efree(vars[i].optval);
		}
	}

	if (!ret) {
		pgsql_handle_closer(dbh TSRMLS_CC);
	}

	return ret;
}
/* }}} */

pdo_driver_t pdo_pgsql_driver = {
	PDO_DRIVER_HEADER(pgsql),
	pdo_pgsql_handle_factory
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
