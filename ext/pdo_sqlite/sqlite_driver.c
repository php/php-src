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
#include "php_pdo_sqlite.h"
#include "php_pdo_sqlite_int.h"
#include "zend_exceptions.h"

int _pdo_sqlite_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt, const char *file, int line TSRMLS_DC) /* {{{ */
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;
	enum pdo_error_type *pdo_err = stmt ? &stmt->error_code : &dbh->error_code;
	pdo_sqlite_error_info *einfo = &H->einfo;

	einfo->errcode = sqlite3_errcode(H->db);
	einfo->file = file;
	einfo->line = line;

	if (einfo->errcode != SQLITE_OK) {
		einfo->errmsg = (char*)sqlite3_errmsg(H->db);
	} else { /* no error */
		*pdo_err = PDO_ERR_NONE;
		return 0;
	}
	switch (einfo->errcode) {
		case SQLITE_NOTFOUND:
			*pdo_err = PDO_ERR_NOT_FOUND;
			break;	

		case SQLITE_INTERRUPT:
			*pdo_err = PDO_ERR_DISCONNECTED;
			break;

		case SQLITE_NOLFS:
			*pdo_err = PDO_ERR_NOT_IMPLEMENTED;
			break;

		case SQLITE_TOOBIG:
			*pdo_err = PDO_ERR_TRUNCATED;
			break;
	
		case SQLITE_CONSTRAINT:
			*pdo_err = PDO_ERR_CONSTRAINT;
			break;

		case SQLITE_ERROR: /* empty query */
			*pdo_err = PDO_ERR_SYNTAX;
			break;

		default:
			*pdo_err = PDO_ERR_CANT_MAP;
			break;
	}

	if (!dbh->methods) {
		zend_throw_exception_ex(php_pdo_get_exception(), *pdo_err TSRMLS_CC, "[%d] %s",
				einfo->errcode, einfo->errmsg);
	}
	
	return einfo->errcode;
}
/* }}} */

static int pdo_sqlite_fetch_error_func(pdo_dbh_t *dbh, pdo_stmt_t *stmt, zval *info TSRMLS_DC)
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;
	pdo_sqlite_error_info *einfo = &H->einfo;

	if (einfo->errcode) {
		add_next_index_long(info, einfo->errcode);
		add_next_index_string(info, einfo->errmsg, 1);
	}

	return 1;
}

static int sqlite_handle_closer(pdo_dbh_t *dbh TSRMLS_DC) /* {{{ */
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;
	
	if (H) {
		if (H->db) {
			sqlite3_close(H->db);
			H->db = NULL;
		}
		pefree(H, dbh->is_persistent);
		dbh->driver_data = NULL;
	}
	return 0;
}
/* }}} */

static int sqlite_handle_preparer(pdo_dbh_t *dbh, const char *sql, long sql_len, pdo_stmt_t *stmt, long options, zval *driver_options TSRMLS_DC)
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;
	pdo_sqlite_stmt *S = ecalloc(1, sizeof(pdo_sqlite_stmt));
	int i;
	const char *tail;

	S->H = H;
	stmt->driver_data = S;
	stmt->methods = &sqlite_stmt_methods;

	i = sqlite3_prepare(H->db, sql, sql_len, &S->stmt, &tail);
	if (i == SQLITE_OK) {
		return 1;
	}

	pdo_sqlite_error(dbh);

	return 0;
}

static long sqlite_handle_doer(pdo_dbh_t *dbh, const char *sql, long sql_len TSRMLS_DC)
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;
	char *errmsg = NULL;

	if (sqlite3_exec(H->db, sql, NULL, NULL, &errmsg) != SQLITE_OK) {
		pdo_sqlite_error(dbh);
		if (errmsg)
			sqlite3_free(errmsg);

		return -1;
	} else {
		return sqlite3_changes(H->db);
	}
}

static long pdo_sqlite_last_insert_id(pdo_dbh_t *dbh TSRMLS_DC)
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;

	return (long) sqlite3_last_insert_rowid(H->db);
}

/* NB: doesn't handle binary strings... use prepared stmts for that */
static int sqlite_handle_quoter(pdo_dbh_t *dbh, const char *unquoted, int unquotedlen, char **quoted, int *quotedlen  TSRMLS_DC)
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;
	*quoted = emalloc(2*unquotedlen + 3);
	sqlite3_snprintf(2*unquotedlen + 3, *quoted, "'%q'", unquoted);
	*quotedlen = strlen(*quoted);
	return 1;
}

static int sqlite_handle_begin(pdo_dbh_t *dbh TSRMLS_DC)
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;
	char *errmsg = NULL;

	if (sqlite3_exec(H->db, "BEGIN", NULL, NULL, &errmsg) != SQLITE_OK) {
		pdo_sqlite_error(dbh);
		if (errmsg)
			sqlite3_free(errmsg);
		return 0;
	}
	return 1;
}

static int sqlite_handle_commit(pdo_dbh_t *dbh TSRMLS_DC)
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;
	char *errmsg = NULL;

	if (sqlite3_exec(H->db, "COMMIT", NULL, NULL, &errmsg) != SQLITE_OK) {
		pdo_sqlite_error(dbh);
		if (errmsg)
			sqlite3_free(errmsg);
		return 0;
	}
	return 1;
}

static int sqlite_handle_rollback(pdo_dbh_t *dbh TSRMLS_DC)
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;
	char *errmsg = NULL;

	if (sqlite3_exec(H->db, "ROLLBACK", NULL, NULL, &errmsg) != SQLITE_OK) {
		pdo_sqlite_error(dbh);
		if (errmsg)
			sqlite3_free(errmsg);
		return 0;
	}
	return 1;
}

static int pdo_sqlite_get_attribute(pdo_dbh_t *dbh, long attr, zval *return_value TSRMLS_DC)
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;

	switch (attr) {
		case PDO_ATTR_CLIENT_VERSION:
		case PDO_ATTR_SERVER_VERSION:
			ZVAL_STRING(return_value, (char *)sqlite3_libversion(), 1);
			break;
		
		default:
			return 0;	
	}

	return 1;
}

static struct pdo_dbh_methods sqlite_methods = {
	sqlite_handle_closer,
	sqlite_handle_preparer,
	sqlite_handle_doer,
	sqlite_handle_quoter,
	sqlite_handle_begin,
	sqlite_handle_commit,
	sqlite_handle_rollback,
	NULL, /* set_attribute */
	pdo_sqlite_last_insert_id,
	pdo_sqlite_fetch_error_func,
	pdo_sqlite_get_attribute,
	NULL	/* check_liveness: not needed */
};

static char *make_filename_safe(const char *filename TSRMLS_DC)
{
	if (strncmp(filename, ":memory:", sizeof(":memory:")-1)) {
		char *fullpath = expand_filepath(filename, NULL TSRMLS_CC);

		if (PG(safe_mode) && (!php_checkuid(fullpath, NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
			efree(fullpath);
			return NULL;
		}

		if (php_check_open_basedir(fullpath TSRMLS_CC)) {
			efree(fullpath);
			return NULL;
		}
		return fullpath;
	}
	return estrdup(filename);
}

static int authorizer(void *autharg, int access_type, const char *arg3, const char *arg4,
		const char *arg5, const char *arg6)
{
	char *filename;
	switch (access_type) {
		case SQLITE_COPY: {
			TSRMLS_FETCH();
			filename = make_filename_safe(arg4 TSRMLS_CC);
			if (!filename) {
				return SQLITE_DENY;
			}
			efree(filename);
			return SQLITE_OK;
		}

		case SQLITE_ATTACH: {
			TSRMLS_FETCH();
			filename = make_filename_safe(arg3 TSRMLS_CC);
			if (!filename) {
				return SQLITE_DENY;
			}
			efree(filename);
			return SQLITE_OK;
		}

		default:
			/* access allowed */
			return SQLITE_OK;
	}
}

static int pdo_sqlite_handle_factory(pdo_dbh_t *dbh, zval *driver_options TSRMLS_DC) /* {{{ */
{
	pdo_sqlite_db_handle *H;
	int i, ret = 0;
	long timeout = 60;
	char *filename;

	H = pecalloc(1, sizeof(pdo_sqlite_db_handle), dbh->is_persistent);
	
	H->einfo.errcode = 0;
	H->einfo.errmsg = NULL;
	dbh->driver_data = H;

	filename = make_filename_safe(dbh->data_source TSRMLS_CC);

	if (!filename) {
		zend_throw_exception_ex(php_pdo_get_exception(), PDO_ERR_NO_PERM TSRMLS_CC,
			"safe_mode/open_basedir prohibits opening %s",
			dbh->data_source);
		goto cleanup;
	}

	i = sqlite3_open(filename, &H->db);
	efree(filename);

	if (i != SQLITE_OK) {
		pdo_sqlite_error(dbh);
		goto cleanup;
	}

	sqlite3_set_authorizer(H->db, authorizer, NULL);

	if (driver_options) {
		timeout = pdo_attr_lval(driver_options, PDO_ATTR_TIMEOUT, timeout TSRMLS_CC);
	}
	sqlite3_busy_timeout(H->db, timeout * 1000);

	dbh->alloc_own_columns = 1;
	dbh->supports_placeholders = PDO_PLACEHOLDER_POSITIONAL;
	dbh->max_escaped_char_length = 2;

	ret = 1;
	
cleanup:
	dbh->methods = &sqlite_methods;
	
	return ret;
}
/* }}} */

pdo_driver_t pdo_sqlite_driver = {
	PDO_DRIVER_HEADER(sqlite),
	pdo_sqlite_handle_factory
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
