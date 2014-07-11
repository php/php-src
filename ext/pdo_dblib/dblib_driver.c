/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2014 The PHP Group                                |
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

/* Cache of the server supported datatypes, initialized in handle_factory */
zval* pdo_dblib_datatypes;

static int dblib_fetch_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt, zval *info TSRMLS_DC)
{
	pdo_dblib_db_handle *H = (pdo_dblib_db_handle *)dbh->driver_data;
	pdo_dblib_err *einfo = &H->err;
	pdo_dblib_stmt *S = NULL;
	char *message;
	char *msg;

	if (stmt) {
		S = (pdo_dblib_stmt*)stmt->driver_data;
		einfo = &S->err;
	}

	if (einfo->dberr == SYBESMSG && einfo->lastmsg) {
		msg = einfo->lastmsg;
	} else if (einfo->dberr == SYBESMSG && DBLIB_G(err).lastmsg) {
		msg = DBLIB_G(err).lastmsg;
		DBLIB_G(err).lastmsg = NULL;
	} else {
		msg = einfo->dberrstr;
	}

	spprintf(&message, 0, "%s [%d] (severity %d) [%s]",
		msg, einfo->dberr, einfo->severity, stmt ? stmt->active_query_string : "");

	add_next_index_long(info, einfo->dberr);
	add_next_index_string(info, message, 0);
	add_next_index_long(info, einfo->oserr);
	add_next_index_long(info, einfo->severity);
	if (einfo->oserrstr) {
		add_next_index_string(info, einfo->oserrstr, 1);
	}

	return 1;
}


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

static int dblib_handle_preparer(pdo_dbh_t *dbh, const char *sql, long sql_len, pdo_stmt_t *stmt, zval *driver_options TSRMLS_DC)
{
	pdo_dblib_db_handle *H = (pdo_dblib_db_handle *)dbh->driver_data;
	pdo_dblib_stmt *S = ecalloc(1, sizeof(*S));
	
	S->H = H;
	stmt->driver_data = S;
	stmt->methods = &dblib_stmt_methods;
	stmt->supports_placeholders = PDO_PLACEHOLDER_NONE;
	S->err.sqlstate = stmt->error_code;

	return 1;
}

static long dblib_handle_doer(pdo_dbh_t *dbh, const char *sql, long sql_len TSRMLS_DC)
{
	pdo_dblib_db_handle *H = (pdo_dblib_db_handle *)dbh->driver_data;
	RETCODE ret, resret;

	dbsetuserdata(H->link, (BYTE*)&H->err);

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

static int dblib_handle_quoter(pdo_dbh_t *dbh, const char *unquoted, int unquotedlen, char **quoted, int *quotedlen, enum pdo_param_type paramtype TSRMLS_DC)
{
	pdo_dblib_db_handle *H = (pdo_dblib_db_handle *)dbh->driver_data;
	char *q;
	int l = 1;

	*quoted = q = safe_emalloc(2, unquotedlen, 3);
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

static int pdo_dblib_transaction_cmd(const char *cmd, pdo_dbh_t *dbh TSRMLS_DC)
{
	pdo_dblib_db_handle *H = (pdo_dblib_db_handle *)dbh->driver_data;
	RETCODE ret;
	
	if (FAIL == dbcmd(H->link, cmd)) {
		return 0;
	}
	
	if (FAIL == dbsqlexec(H->link)) {
		return 0;
	}
	
	return 1;
}

static int dblib_handle_begin(pdo_dbh_t *dbh TSRMLS_DC)
{
	return pdo_dblib_transaction_cmd("BEGIN TRANSACTION", dbh TSRMLS_CC);
}

static int dblib_handle_commit(pdo_dbh_t *dbh TSRMLS_DC)
{
	return pdo_dblib_transaction_cmd("COMMIT TRANSACTION", dbh TSRMLS_CC);
}

static int dblib_handle_rollback(pdo_dbh_t *dbh TSRMLS_DC)
{
	return pdo_dblib_transaction_cmd("ROLLBACK TRANSACTION", dbh TSRMLS_CC);
}

char *dblib_handle_last_id(pdo_dbh_t *dbh, const char *name, unsigned int *len TSRMLS_DC) 
{
	pdo_dblib_db_handle *H = (pdo_dblib_db_handle *)dbh->driver_data;

	RETCODE ret;
	char *id = NULL;

	/* 
	 * Would use scope_identity() but it's not implemented on Sybase
	 */
	
	if (FAIL == dbcmd(H->link, "SELECT @@IDENTITY")) {
		return NULL;
	}
	
	if (FAIL == dbsqlexec(H->link)) {
		return NULL;
	}
	
	ret = dbresults(H->link);
	if (ret == FAIL || ret == NO_MORE_RESULTS) {
		dbcancel(H->link);
		return NULL;
	}

	ret = dbnextrow(H->link);
	
	if (ret == FAIL || ret == NO_MORE_ROWS) {
		dbcancel(H->link);
		return NULL;
	}

	if (dbdatlen(H->link, 1) == 0) {
		dbcancel(H->link);
		return NULL;
	}

	id = emalloc(32);
	*len = dbconvert(NULL, (dbcoltype(H->link, 1)) , (dbdata(H->link, 1)) , (dbdatlen(H->link, 1)), SQLCHAR, id, (DBINT)-1);
		
	dbcancel(H->link);
	return id;
}

static struct pdo_dbh_methods dblib_methods = {
	dblib_handle_closer,
	dblib_handle_preparer,
	dblib_handle_doer,
	dblib_handle_quoter,
	dblib_handle_begin, /* begin */
	dblib_handle_commit, /* commit */
	dblib_handle_rollback, /* rollback */
	NULL, /*set attr */
	dblib_handle_last_id, /* last insert id */
	dblib_fetch_error, /* fetch error */
	NULL, /* get attr */
	NULL, /* check liveness */
	NULL, /* get driver methods */
	NULL, /* request shutdown */
	NULL  /* in transaction */
};

static int pdo_dblib_handle_factory(pdo_dbh_t *dbh, zval *driver_options TSRMLS_DC)
{
	pdo_dblib_db_handle *H;
	int i, nvars, nvers, ret = 0;
	int *val;
	
	const pdo_dblib_keyval tdsver[] = {
		 {"4.2",DBVERSION_42}
		,{"4.6",DBVERSION_46}
		,{"5.0",DBVERSION_70} /* FIXME: This does not work with Sybase, but environ will */
		,{"6.0",DBVERSION_70}
		,{"7.0",DBVERSION_70}
#ifdef DBVERSION_71
		,{"7.1",DBVERSION_71}
#endif
#ifdef DBVERSION_72
		,{"7.2",DBVERSION_72}
		,{"8.0",DBVERSION_72}
#endif
		,{"10.0",DBVERSION_100}
		,{"auto",0} /* Only works with FreeTDS. Other drivers will bork */
		
	};
	
	nvers = sizeof(tdsver)/sizeof(tdsver[0]);
	
	struct pdo_data_src_parser vars[] = {
		{ "charset",	NULL,	0 }
		,{ "appname",	"PHP " PDO_DBLIB_FLAVOUR,	0 }
		,{ "host",		"127.0.0.1", 0 }
		,{ "dbname",	NULL,	0 }
		,{ "secure",	NULL,	0 } /* DBSETLSECURE */
		,{ "version",	NULL,	0 } /* DBSETLVERSION */
	};
	
	nvars = sizeof(vars)/sizeof(vars[0]);
	
	php_pdo_parse_data_source(dbh->data_source, dbh->data_source_len, vars, nvars);

	H = pecalloc(1, sizeof(*H), dbh->is_persistent);
	H->login = dblogin();
	H->err.sqlstate = dbh->error_code;

	if (!H->login) {
		goto cleanup;
	}

	DBERRHANDLE(H->login, (EHANDLEFUNC) error_handler);
	DBMSGHANDLE(H->login, (MHANDLEFUNC) msg_handler);
	
	if(vars[5].optval) {
		for(i=0;i<nvers;i++) {
			if(strcmp(vars[5].optval,tdsver[i].key) == 0) {
				if(FAIL==dbsetlversion(H->login, tdsver[i].value)) {
					pdo_raise_impl_error(dbh, NULL, "HY000", "PDO_DBLIB: Failed to set version specified in connection string." TSRMLS_CC);		
					goto cleanup;
				}
				break;
			}
		}
		
		if (i==nvers) {
			printf("Invalid version '%s'\n", vars[5].optval);
			pdo_raise_impl_error(dbh, NULL, "HY000", "PDO_DBLIB: Invalid version specified in connection string." TSRMLS_CC);		
			goto cleanup; /* unknown version specified */
		}
	}

	if (dbh->username) {
		if(FAIL == DBSETLUSER(H->login, dbh->username)) {
			goto cleanup;
		}
	}

	if (dbh->password) {
		if(FAIL == DBSETLPWD(H->login, dbh->password)) {
			goto cleanup;
		}
	}
	
#if !PHP_DBLIB_IS_MSSQL
	if (vars[0].optval) {
		DBSETLCHARSET(H->login, vars[0].optval);
	}
#endif

	DBSETLAPP(H->login, vars[1].optval);

/* DBSETLDBNAME is only available in FreeTDS 0.92 or above */
#ifdef DBSETLDBNAME
	if (vars[3].optval) {
		if(FAIL == DBSETLDBNAME(H->login, vars[3].optval)) goto cleanup;
	}
#endif

	H->link = dbopen(H->login, vars[2].optval);

	if (!H->link) {
		goto cleanup;
	}

/*
 * FreeTDS < 0.92 does not support the DBSETLDBNAME option
 * Send use database here after login (Will not work with SQL Azure)
 */
#ifndef DBSETLDBNAME
	if (vars[3].optval) {
		if(FAIL == dbuse(H->link, vars[3].optval)) goto cleanup;
	}
#endif

#if PHP_DBLIB_IS_MSSQL
	/* dblib do not return more than this length from text/image */
	DBSETOPT(H->link, DBTEXTLIMIT, "2147483647");
#endif

	/* limit text/image from network */
	DBSETOPT(H->link, DBTEXTSIZE, "2147483647");

	/* allow double quoted indentifiers */
	DBSETOPT(H->link, DBQUOTEDIDENT, "1");

	ret = 1;
	dbh->max_escaped_char_length = 2;
	dbh->alloc_own_columns = 1;

cleanup:
	for (i = 0; i < nvars; i++) {
		if (vars[i].freeme) {
			efree(vars[i].optval);
		}
	}

	dbh->methods = &dblib_methods;
	dbh->driver_data = H;

	if (!ret) {
		zend_throw_exception_ex(php_pdo_get_exception(), DBLIB_G(err).dberr TSRMLS_CC,
			"SQLSTATE[%s] %s (severity %d)",
			DBLIB_G(err).sqlstate,
			DBLIB_G(err).dberrstr,
			DBLIB_G(err).severity);
	}

	return ret;
}

pdo_driver_t pdo_dblib_driver = {
#if PDO_DBLIB_IS_MSSQL
	PDO_DRIVER_HEADER(mssql),
#elif defined(PHP_WIN32)
#define PDO_DBLIB_IS_SYBASE
	PDO_DRIVER_HEADER(sybase),
#else
	PDO_DRIVER_HEADER(dblib),
#endif
	pdo_dblib_handle_factory
};

