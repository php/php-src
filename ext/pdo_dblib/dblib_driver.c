/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2005 The PHP Group                                |
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

static int dblib_handle_closer(pdo_dbh_t *dbh TSRMLS_DC)
{
	pdo_dblib_db_handle *H = (pdo_dblib_db_handle *)dbh->driver_data;

	if (H) {
		if (H->link) {
			dbclose(H->link);
			H->link = NULL;
		}
		if (H->login) {
			dbfreelogin(H->login);
			H->login = NULL;
		}
		pefree(H, dbh->is_persistent);
		dbh->driver_data = NULL;
	}
	return 0;
}

static int dblib_handle_preparer(pdo_dbh_t *dbh, const char *sql, long sql_len, pdo_stmt_t *stmt, long options, zval *driver_options TSRMLS_DC)
{
	pdo_dblib_db_handle *H = (pdo_dblib_db_handle *)dbh->driver_data;
	pdo_dblib_stmt *S = ecalloc(1, sizeof(*S));
	
	S->H = H;
	stmt->driver_data = S;
	stmt->methods = &dblib_stmt_methods;

	return 1;
}

static long dblib_handle_doer(pdo_dbh_t *dbh, const char *sql, long sql_len TSRMLS_DC)
{
	pdo_dblib_db_handle *H = (pdo_dblib_db_handle *)dbh->driver_data;
	RETCODE ret, resret;

	if (FAIL == dbcmd(H->link, sql)) {
		return -1;
	}

	if (FAIL == dbsqlexec(H->link)) {
		return -1;
	}
	
	resret = dbresults(H->link);

	if (resret == FAIL) {
		return -1;
	}

	ret = dbnextrow(H->link);
	if (ret == FAIL) {
		return -1;
	}

	if (dbnumcols(H->link) <= 0) {
		return DBCOUNT(H->link);
	}

	/* throw away any rows it might have returned */
	dbcanquery(H->link);

	return DBCOUNT(H->link);
}

static int dblib_handle_quoter(pdo_dbh_t *dbh, const char *unquoted, int unquotedlen, char **quoted, int *quotedlen  TSRMLS_DC)
{
	pdo_dblib_db_handle *H = (pdo_dblib_db_handle *)dbh->driver_data;
	char *q;
	int l = 1;

	*quoted = q = emalloc(2 * unquotedlen + 3);
	*q++ = '\'';

	while (unquotedlen--) {
		if (*unquoted == '\'') {
			*q++ = '\'';
			*q++ = '\'';
			l += 2;
		} else {
			*q++ = *unquoted;
			++l;
		}
		unquoted++;
	}

	*q++ = '\'';
	*q++ = '\0';
	*quotedlen = l+1;
	
	return 1;
}

static struct pdo_dbh_methods dblib_methods = {
	dblib_handle_closer,
	dblib_handle_preparer,
	dblib_handle_doer,
	dblib_handle_quoter,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL, /* last insert */
	NULL, /* fetch error */
	NULL, /* get attr */
	NULL, /* check liveness */
};

static int pdo_dblib_handle_factory(pdo_dbh_t *dbh, zval *driver_options TSRMLS_DC)
{
	pdo_dblib_db_handle *H;
	int i, ret = 0;
	struct pdo_data_src_parser vars[] = {
		{ "charset",	NULL,	0 },
		{ "appname",	"PHP " PDO_DBLIB_FLAVOUR,	0 },
		{ "host",		"127.0.0.1", 0 },
		{ "dbname",		NULL,	0 },
		{ "secure",		NULL,	0 }, /* DBSETLSECURE */
		/* TODO: DBSETLVERSION ? */
	};

	php_pdo_parse_data_source(dbh->data_source, dbh->data_source_len, vars, 5);

	H = pecalloc(1, sizeof(*H), dbh->is_persistent);
	H->login = dblogin();

	if (!H->login) {
		goto cleanup;
	}

	if (dbh->username) {
		DBSETLUSER(H->login, dbh->username);
	}
	if (dbh->password) {
		DBSETLPWD(H->login, dbh->password);
	}
	if (vars[0].optval) {
		DBSETLCHARSET(H->login, vars[0].optval);
	}
	DBSETLAPP(H->login, vars[1].optval);

	H->link = dbopen(H->login, vars[2].optval);

	if (H->link == NULL) {
		goto cleanup;
	}
	
	if (vars[3].optval && FAIL == dbuse(H->link, vars[3].optval)) {
		goto cleanup;
	}

	ret = 1;
	dbh->supports_placeholders = 0;
	dbh->max_escaped_char_length = 2;
	dbh->alloc_own_columns = 1;

cleanup:
	for (i = 0; i < sizeof(vars)/sizeof(vars[0]); i++) {
		if (vars[i].freeme) {
			efree(vars[i].optval);
		}
	}

	dbh->methods = &dblib_methods;
	dbh->driver_data = H;

	return ret;
}

pdo_driver_t pdo_dblib_driver = {
#if PDO_DBLIB_IS_MSSQL
	PDO_DRIVER_HEADER(mssql),
#else
	PDO_DRIVER_HEADER(sybase),
#endif
	pdo_dblib_handle_factory
};

