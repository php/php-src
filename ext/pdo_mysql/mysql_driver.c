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

int _pdo_mysql_error(char *what, int mysql_errno, const char *file, int line TSRMLS_DC) /* {{{ */
{
	switch (mysql_errno) {
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "(%s:%d) %s: %d", file, line, what, mysql_errno);
			break;
	}
	return mysql_errno;
}
/* }}} */

int mysql_handle_error(pdo_dbh_t *dbh, pdo_mysql_db_handle *H, int errcode) /* {{{ */
{
	return 0;
}
/* }}} */

static int mysql_handle_closer(pdo_dbh_t *dbh TSRMLS_DC) /* {{{ */
{
	pdo_mysql_db_handle *H = (pdo_mysql_db_handle *)dbh->driver_data;

	if (H->server) {
		mysql_close(H->server);
		H->server = NULL;
	}
	if (H->mysql_error) {
		efree(H->mysql_error);
		H->mysql_error = NULL;
	}
	return 0;
}
/* }}} */

static int mysql_handle_preparer(pdo_dbh_t *dbh, const char *sql, long sql_len, pdo_stmt_t *stmt, long options, zval *driver_options TSRMLS_DC)
{
	pdo_mysql_db_handle *H = (pdo_mysql_db_handle *)dbh->driver_data;
	pdo_mysql_stmt *S = ecalloc(1, sizeof(pdo_mysql_stmt));

	S->H = H;
	stmt->driver_data = S;
	stmt->methods = &mysql_stmt_methods;
	
	return 1;
}

static long mysql_handle_doer(pdo_dbh_t *dbh, const char *sql, long sql_len TSRMLS_DC)
{
	pdo_mysql_db_handle *H = (pdo_mysql_db_handle *)dbh->driver_data;

	if (mysql_real_query(H->server, sql, sql_len)) {
		pdo_mysql_error(H);
		return -1;
	} else {
		return mysql_affected_rows(H->server);
	}
}

static long pdo_mysql_last_insert_id(pdo_dbh_t *dbh TSRMLS_DC)
{
	pdo_mysql_db_handle *H = (pdo_mysql_db_handle *)dbh->driver_data;

	return (long) mysql_insert_id(H->server);
}

static int mysql_handle_quoter(pdo_dbh_t *dbh, const char *unquoted, int unquotedlen, char **quoted, int *quotedlen  TSRMLS_DC)
{
	pdo_mysql_db_handle *H = (pdo_mysql_db_handle *)dbh->driver_data;
	*quoted = emalloc(2*unquotedlen + 3);
	(*quoted)[0] = '"';
	*quotedlen = mysql_real_escape_string(H->server, *quoted + 1, unquoted, unquotedlen);
	(*quoted)[*quotedlen + 1] = '"';
	(*quoted)[*quotedlen + 2] = '\0';
	*quotedlen += 2;
	return 1;
}


static struct pdo_dbh_methods mysql_methods = {
	mysql_handle_closer,
	mysql_handle_preparer,
	mysql_handle_doer,
	mysql_handle_quoter,
	NULL,
	NULL,
	NULL,
	NULL,
	pdo_mysql_last_insert_id
};

static int pdo_mysql_handle_factory(pdo_dbh_t *dbh, zval *driver_options TSRMLS_DC) /* {{{ */
{
	pdo_mysql_db_handle *H;
	int i, ret = 0;
	char *host = NULL, *unix_socket = NULL;
	unsigned int port;
	char *dbname;
	struct pdo_data_src_parser vars[] = {
		{ "charset",  NULL,	0 },
		{ "dbname",   "",	0 },
		{ "host",   "localhost",	0 },
		{ "port",   "3306",	0 },
		{ "unix_socket",  PDO_MYSQL_UNIX_ADDR,	0 },
	};

	php_pdo_parse_data_source(dbh->data_source, dbh->data_source_len, vars, 4);

	H = pecalloc(1, sizeof(pdo_mysql_db_handle), dbh->is_persistent);
	
	/* allocate an environment */
	
	/* handle for the server */
	H->server = mysql_init(NULL);
	dbh->driver_data = H;
	if(vars[2].optval && strcmp("localhost", vars[2].optval)) {
		host = vars[2].optval;
		port = atoi(vars[3].optval); 
	} else {
		host = NULL;
		unix_socket = vars[4].optval;  
	}
	dbname = vars[1].optval;
	if(mysql_real_connect(H->server, host, dbh->username, dbh->password, dbname, port, unix_socket, 0) == NULL) 
	{
		pdo_mysql_error(H);
		goto cleanup;
	}

	H->attached = 1;

	dbh->methods = &mysql_methods;
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

	return ret;
}
/* }}} */

pdo_driver_t pdo_mysql_driver = {
	PDO_DRIVER_HEADER(mysql),
	pdo_mysql_handle_factory
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
