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
#include "php_pdo_oci.h"
#include "php_pdo_oci_int.h"

ub4 _oci_error(OCIError *err, char *what, sword status, const char *file, int line TSRMLS_DC) /* {{{ */
{
	text errbuf[1024] = "<<Unknown>>";
	sb4 errcode = 0;

	switch (status) {
		case OCI_SUCCESS:
			break;
		case OCI_ERROR:
			OCIErrorGet(err, (ub4)1, NULL, &errcode, errbuf, (ub4)sizeof(errbuf), OCI_HTYPE_ERROR);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "(%s:%d) %s: %d %s", file, line, what, errcode, errbuf);
			break;
		case OCI_SUCCESS_WITH_INFO:
			OCIErrorGet(err, (ub4)1, NULL, &errcode, errbuf, (ub4)sizeof(errbuf), OCI_HTYPE_ERROR);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "(%s:%d) %s: OCI_SUCCESS_WITH_INFO: %s", file, line, what, errbuf);
			break;
		case OCI_NEED_DATA:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "(%s:%d) %s: OCI_NEED_DATA",  file, line, what);
			break;
		case OCI_NO_DATA:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "(%s:%d) %s: OCI_NO_DATA", file, line,  what);
			break;
		case OCI_INVALID_HANDLE:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "(%s:%d) %s: OCI_INVALID_HANDLE", file, line,  what);
			break;
		case OCI_STILL_EXECUTING:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "(%s:%d) %s: OCI_STILL_EXECUTING", file, line,  what);
			break;
		case OCI_CONTINUE:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "(%s:%d) %s: OCI_CONTINUE", file, line,  what);
			break;
	}
	return errcode;
}
/* }}} */

ub4 oci_handle_error(pdo_dbh_t *dbh, pdo_oci_db_handle *H, ub4 errcode) /* {{{ */
{
	switch (errcode) {
		case 1013:	/* user requested cancel of current operation */
			zend_bailout();
			break;
		case 22:	/* ORA-00022: invalid session id */
		case 1012:	/* ORA-01012: */
		case 3113:	/* ORA-03133: end of file on communication channel */
		case 604:
		case 1041:
			/* consider the connection closed */
			dbh->is_closed = 1;
			H->attached = 0;
			return 1;
	}
	return 0;
}
/* }}} */

static int oci_handle_closer(pdo_dbh_t *dbh TSRMLS_DC) /* {{{ */
{
	pdo_oci_db_handle *H = (pdo_oci_db_handle *)dbh->driver_data;
	
	if (H->svc) {
		/* rollback any outstanding work */
		OCITransRollback(H->svc, H->err, 0);
	}

	if (H->session) {
		OCIHandleFree(H->session, OCI_HTYPE_SESSION);
		H->session = NULL;
	}

	if (H->svc) {
		OCIHandleFree(H->svc, OCI_HTYPE_SVCCTX);
		H->svc = NULL;
	}
	
	if (H->server && H->attached) {
		H->last_err = OCIServerDetach(H->server, H->err, OCI_DEFAULT);
		if (H->last_err) {
			oci_error(H->err, "OCIServerDetach", H->last_err);
		}
		H->attached = 0;
	}

	if (H->server) {
		OCIHandleFree(H->server, OCI_HTYPE_SERVER);
		H->server = NULL;
	}

	OCIHandleFree(H->err, OCI_HTYPE_ERROR);
	H->err = NULL;

	if (H->charset && H->env) {
		OCIHandleFree(H->env, OCI_HTYPE_ENV);
		H->env = NULL;
	}

	pefree(H, dbh->is_persistent);

	return 0;
}
/* }}} */

static int oci_handle_preparer(pdo_dbh_t *dbh, const char *sql, long sql_len, pdo_stmt_t *stmt, long options, zval *driver_options TSRMLS_DC)
{
	pdo_oci_db_handle *H = (pdo_oci_db_handle *)dbh->driver_data;
	pdo_oci_stmt *S = ecalloc(1, sizeof(*S));

	S->H = H;
	
	/* create an OCI statement handle */
	OCIHandleAlloc(H->env, &S->stmt, OCI_HTYPE_STMT, 0, NULL);

	/* and our own private error handle */
	OCIHandleAlloc(H->env, &S->err, OCI_HTYPE_ERROR, 0, NULL);

	if (sql_len) {
		H->last_err = OCIStmtPrepare(S->stmt, H->err, (text*)sql, sql_len, OCI_NTV_SYNTAX, OCI_DEFAULT);

		if (H->last_err) {
			H->last_err = oci_error(H->err, "OCIStmtPrepare", H->last_err);
			OCIHandleFree(S->stmt, OCI_HTYPE_STMT);
			OCIHandleFree(S->err, OCI_HTYPE_ERROR);
			efree(S);
			oci_handle_error(dbh, H, H->last_err);
			return 0;
		}
	}

	stmt->driver_data = S;
	stmt->methods = &oci_stmt_methods;
	
	return 1;
}

static int oci_handle_doer(pdo_dbh_t *dbh, const char *sql TSRMLS_DC)
{
	pdo_oci_db_handle *H = (pdo_oci_db_handle *)dbh->driver_data;

	return 0;
}

static int oci_handle_quoter(pdo_dbh_t *dbh, const char *unquoted, int unquotedlen, char **quoted, int *quotedlen  TSRMLS_DC)
{
	pdo_oci_db_handle *H = (pdo_oci_db_handle *)dbh->driver_data;

	return 0;
}

static int oci_handle_begin(pdo_dbh_t *dbh TSRMLS_DC)
{
	/* with Oracle, there is nothing special to be done */
	return 1;
}

static int oci_handle_commit(pdo_dbh_t *dbh TSRMLS_DC)
{
	pdo_oci_db_handle *H = (pdo_oci_db_handle *)dbh->driver_data;

	H->last_err = OCITransCommit(H->svc, H->err, 0);

	if (H->last_err) {
		H->last_err = oci_error(H->err, "OCITransCommit", H->last_err);
		oci_handle_error(dbh, H, H->last_err);
		return 0;
	}
	return 1;
}

static int oci_handle_rollback(pdo_dbh_t *dbh TSRMLS_DC)
{
	pdo_oci_db_handle *H = (pdo_oci_db_handle *)dbh->driver_data;

	H->last_err = OCITransRollback(H->svc, H->err, 0);

	if (H->last_err) {
		H->last_err = oci_error(H->err, "OCITransRollback", H->last_err);
		oci_handle_error(dbh, H, H->last_err);
		return 0;
	}
	return 1;
}

static int oci_handle_set_attribute(pdo_dbh_t *dbh, long attr, zval *val TSRMLS_DC)
{
	pdo_oci_db_handle *H = (pdo_oci_db_handle *)dbh->driver_data;

	if (dbh->in_txn) {
		/* Assume they want to commit whatever is outstanding */
		H->last_err = OCITransCommit(H->svc, H->err, 0);

		if (H->last_err) {
			H->last_err = oci_error(H->err, "OCITransCommit", H->last_err);
			oci_handle_error(dbh, H, H->last_err);
			return 0;
		}
		dbh->in_txn = 0;
	}
	
	convert_to_long(val);

	dbh->auto_commit = Z_LVAL_P(val);
	
	return 1;
}

static struct pdo_dbh_methods oci_methods = {
	oci_handle_closer,
	oci_handle_preparer,
	oci_handle_doer,
	oci_handle_quoter,
	oci_handle_begin,
	oci_handle_commit,
	oci_handle_rollback,
	oci_handle_set_attribute
};

static int pdo_oci_handle_factory(pdo_dbh_t *dbh, zval *driver_options TSRMLS_DC) /* {{{ */
{
	pdo_oci_db_handle *H;
	int i, ret = 0;
	struct pdo_data_src_parser vars[] = {
		{ "charset",  NULL,	0 },
		{ "dbname",   "",	0 }
	};

	php_pdo_parse_data_source(dbh->data_source, dbh->data_source_len, vars, 4);
	
	H = pecalloc(1, sizeof(*H), dbh->is_persistent);
	
	/* allocate an environment */
#if HAVE_OCIENVNLSCREATE
	if (vars[0].optval) {
		H->charset = OCINlsCharSetNameToId(pdo_oci_Env, vars[0].optval);
		if (H->charset) {
			OCIEnvNlsCreate(&H->env, PDO_OCI_INIT_MODE, 0, NULL, NULL, NULL, 0, NULL, H->charset, H->charset);
		}
	}
#endif
	if (H->env == NULL) {
		/* use the global environment */
		H->env = pdo_oci_Env;
	}

	/* something to hold errors */
	OCIHandleAlloc(H->env, (dvoid **)&H->err, OCI_HTYPE_ERROR, 0, NULL);
	
	/* handle for the server */
	OCIHandleAlloc(H->env, (dvoid **)&H->server, OCI_HTYPE_SERVER, 0, NULL);
	
	H->last_err = OCIServerAttach(H->server, H->err, (text*)vars[1].optval,
		   	strlen(vars[1].optval), OCI_DEFAULT);

	if (H->last_err) {
		oci_error(H->err, "pdo_oci_handle_factory", H->last_err);
		goto cleanup;
	}

	H->attached = 1;

	/* create a service context */
	H->last_err = OCIHandleAlloc(H->env, (dvoid**)&H->svc, OCI_HTYPE_SVCCTX, 0, NULL);
	if (H->last_err) {
		oci_error(H->err, "OCIHandleAlloc: OCI_HTYPE_SVCCTX", H->last_err);
		goto cleanup;
	}

	H->last_err = OCIHandleAlloc(H->env, (dvoid**)&H->session, OCI_HTYPE_SESSION, 0, NULL);
	if (H->last_err) {
		oci_error(H->err, "OCIHandleAlloc: OCI_HTYPE_SESSION", H->last_err);
		goto cleanup;
	}

	/* set server handle into service handle */
	H->last_err = OCIAttrSet(H->svc, OCI_HTYPE_SVCCTX, H->server, 0, OCI_ATTR_SERVER, H->err);
	if (H->last_err) {
		oci_error(H->err, "OCIAttrSet: OCI_ATTR_SERVER", H->last_err);
		goto cleanup;
	}

	/* username */
	H->last_err = OCIAttrSet(H->session, OCI_HTYPE_SESSION,
		   	dbh->username, strlen(dbh->username),
			OCI_ATTR_USERNAME, H->err);
	if (H->last_err) {
		oci_error(H->err, "OCIAttrSet: OCI_ATTR_USERNAME", H->last_err);
		goto cleanup;
	}

	/* password */
	H->last_err = OCIAttrSet(H->session, OCI_HTYPE_SESSION,
		   	dbh->password, strlen(dbh->password),
			OCI_ATTR_PASSWORD, H->err);
	if (H->last_err) {
		oci_error(H->err, "OCIAttrSet: OCI_ATTR_PASSWORD", H->last_err);
		goto cleanup;
	}

	/* Now fire up the session */
	H->last_err = OCISessionBegin(H->svc, H->err, H->session, OCI_CRED_RDBMS, OCI_DEFAULT);	
	if (H->last_err) {
		oci_error(H->err, "OCISessionBegin:", H->last_err);
		goto cleanup;
	}

	/* set the server handle into service handle */
	H->last_err = OCIAttrSet(H->svc, OCI_HTYPE_SVCCTX, H->session, 0, OCI_ATTR_SESSION, H->err);
	if (H->last_err) {
		oci_error(H->err, "OCIAttrSet: OCI_ATTR_SESSION:", H->last_err);
		goto cleanup;
	}
	
	dbh->driver_data = H;
	dbh->methods = &oci_methods;
	dbh->alloc_own_columns = 1;
	dbh->supports_placeholders = 1;
	dbh->placeholders_can_be_strings = 1;

	ret = 1;
	
cleanup:
	for (i = 0; i < sizeof(vars)/sizeof(vars[0]); i++) {
		if (vars[i].freeme) {
			efree(vars[i].optval);
		}
	}

	if (!ret) {
		oci_handle_closer(dbh TSRMLS_CC);
	}

	return ret;
}
/* }}} */

pdo_driver_t pdo_oci_driver = {
	PDO_DRIVER_HEADER(oci),
	pdo_oci_handle_factory
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
