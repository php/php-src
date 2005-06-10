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
	pdo_error_type *pdo_err = stmt ? &stmt->error_code : &dbh->error_code;
	pdo_sqlite_error_info *einfo = &H->einfo;

	einfo->errcode = sqlite3_errcode(H->db);
	einfo->file = file;
	einfo->line = line;

	if (einfo->errcode != SQLITE_OK) {
		einfo->errmsg = (char*)sqlite3_errmsg(H->db);
	} else { /* no error */
		strcpy(*pdo_err, PDO_ERR_NONE);
		return 0;
	}
	switch (einfo->errcode) {
		case SQLITE_NOTFOUND:
			strcpy(*pdo_err, "42S02");
			break;	

		case SQLITE_INTERRUPT:
			strcpy(*pdo_err, "01002");
			break;

		case SQLITE_NOLFS:
			strcpy(*pdo_err, "HYC00");
			break;

		case SQLITE_TOOBIG:
			strcpy(*pdo_err, "22001");
			break;
	
		case SQLITE_CONSTRAINT:
			strcpy(*pdo_err, "23000");
			break;

		case SQLITE_ERROR:
		default:
			strcpy(*pdo_err, "HY000");
			break;
	}

	if (!dbh->methods) {
		zend_throw_exception_ex(php_pdo_get_exception(), 0 TSRMLS_CC, "SQLSTATE[%s] [%d] %s",
				*pdo_err, einfo->errcode, einfo->errmsg);
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

static void pdo_sqlite_cleanup_callbacks(pdo_sqlite_db_handle *H TSRMLS_DC)
{
	struct pdo_sqlite_func *func;

	while (H->funcs) {
		func = H->funcs;
		H->funcs = func->next;

		if (H->db) {
			/* delete the function from the handle */
			sqlite3_create_function(H->db,
				func->funcname,
				func->argc,
				SQLITE_UTF8,
				func,
				NULL, NULL, NULL);
		}

		efree((char*)func->funcname);
		if (func->func) {
			zval_ptr_dtor(&func->func);
		}
		if (func->step) {
			zval_ptr_dtor(&func->step);
		}
		if (func->fini) {
			zval_ptr_dtor(&func->fini);
		}
		efree(func);
	}
}

static int sqlite_handle_closer(pdo_dbh_t *dbh TSRMLS_DC) /* {{{ */
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;
	
	if (H) {
		pdo_sqlite_cleanup_callbacks(H TSRMLS_CC);
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

static int sqlite_handle_preparer(pdo_dbh_t *dbh, const char *sql, long sql_len, pdo_stmt_t *stmt, zval *driver_options TSRMLS_DC)
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;
	pdo_sqlite_stmt *S = ecalloc(1, sizeof(pdo_sqlite_stmt));
	int i;
	const char *tail;

	S->H = H;
	stmt->driver_data = S;
	stmt->methods = &sqlite_stmt_methods;
	stmt->supports_placeholders = PDO_PLACEHOLDER_POSITIONAL|PDO_PLACEHOLDER_NAMED;

	if (PDO_CURSOR_FWDONLY != pdo_attr_lval(driver_options, PDO_ATTR_CURSOR, PDO_CURSOR_FWDONLY TSRMLS_CC)) {
		H->einfo.errcode = SQLITE_ERROR;
		pdo_sqlite_error(dbh);
		return 0;
	}

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

static char *pdo_sqlite_last_insert_id(pdo_dbh_t *dbh, const char *name, unsigned int *len TSRMLS_DC)
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;
	char *id;
	
	id = php_pdo_int64_to_str(sqlite3_last_insert_rowid(H->db) TSRMLS_CC);
	*len = strlen(id);
	return id;
}

/* NB: doesn't handle binary strings... use prepared stmts for that */
static int sqlite_handle_quoter(pdo_dbh_t *dbh, const char *unquoted, int unquotedlen, char **quoted, int *quotedlen, enum pdo_param_type paramtype  TSRMLS_DC)
{
	*quoted = safe_emalloc(2, unquotedlen, 3);
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

static int pdo_sqlite_set_attr(pdo_dbh_t *dbh, long attr, zval *val TSRMLS_DC)
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;

	switch (attr) {
		case PDO_ATTR_TIMEOUT:
			convert_to_long(val);
			sqlite3_busy_timeout(H->db, Z_LVAL_P(val) * 1000);
			return 1;
	}
	return 0;
}

static int do_callback(struct pdo_sqlite_fci *fc, zval *cb,
		int argc, sqlite3_value **argv, zval **retval TSRMLS_DC)
{
	zval ***zargs = NULL;
	int i;
	int ret;
	
	fc->fci.size = sizeof(fc->fci);
	fc->fci.function_table = EG(function_table);
	fc->fci.function_name = cb;
	fc->fci.symbol_table = NULL;
	fc->fci.object_pp = NULL;
	fc->fci.retval_ptr_ptr = retval;
	fc->fci.param_count = argc;
	
	/* build up the params */
	if (argc) {
		zargs = (zval ***)safe_emalloc(argc, sizeof(zval **), 0);

		for (i = 0; i < argc; i++) {
			zargs[i] = emalloc(sizeof(zval *));
			MAKE_STD_ZVAL(*zargs[i]);
		
			/* get the value */
			switch (sqlite3_value_type(argv[i])) {
				case SQLITE_INTEGER:
					ZVAL_LONG(*zargs[i], sqlite3_value_int(argv[i]));
					break;

				case SQLITE_FLOAT:
					ZVAL_DOUBLE(*zargs[i], sqlite3_value_double(argv[i]));
					break;
					
				case SQLITE_NULL:
					ZVAL_NULL(*zargs[i]);
					break;
					
				case SQLITE_BLOB:
				case SQLITE3_TEXT:
				default:
					ZVAL_STRINGL(*zargs[i], (char*)sqlite3_value_text(argv[i]),
						sqlite3_value_bytes(argv[i]), 1);
					break;
			}
		}
	}


	fc->fci.params = zargs;

	if ((ret = zend_call_function(&fc->fci, &fc->fcc TSRMLS_CC)) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "An error occurred while invoking the callback");
	}

	/* clean up the params */
	if (argc) {
		for (i = 0; i < argc; i++) {
			zval_ptr_dtor(zargs[i]);
			efree(zargs[i]);
		}
		efree(zargs);
	}

	return ret;
}

static void php_sqlite3_func_callback(sqlite3_context *context, int argc,
	sqlite3_value **argv)
{
	struct pdo_sqlite_func *func = (struct pdo_sqlite_func*)context;
	zval *retval = NULL;
	TSRMLS_FETCH();

	do_callback(&func->afunc, func->func, argc, argv, &retval TSRMLS_CC);
}

static void php_sqlite3_func_step_callback(sqlite3_context *context, int argc,
	sqlite3_value **argv)
{
	struct pdo_sqlite_func *func = (struct pdo_sqlite_func*)context;
	zval *retval = NULL;
	TSRMLS_FETCH();

	do_callback(&func->astep, func->step, argc, argv, &retval TSRMLS_CC);
}

static void php_sqlite3_func_final_callback(sqlite3_context *context)
{
	struct pdo_sqlite_func *func = (struct pdo_sqlite_func*)context;
	zval *retval = NULL;
	TSRMLS_FETCH();

	do_callback(&func->afini, func->fini, 0, NULL, &retval TSRMLS_CC);
}

/* {{{ bool SQLite::sqliteCreateFunction(string name, mixed callback [, int argcount])
   Registers a UDF with the sqlite db handle */
static PHP_METHOD(SQLite, sqliteCreateFunction)
{
	struct pdo_sqlite_func *func;
	zval *callback;
	char *func_name;
	int func_name_len;
	long argc = -1;
	char *cbname = NULL;
	pdo_dbh_t *dbh;
	pdo_sqlite_db_handle *H;
	int ret;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz|l",
			&func_name, &func_name_len, &callback, &argc)) {
		RETURN_FALSE;
	}
	
	dbh = zend_object_store_get_object(getThis() TSRMLS_CC);

	if (dbh->is_persistent) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "wont work on persistent handles");
		RETURN_FALSE;
	}

	if (!zend_is_callable(callback, 0, &cbname)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "function '%s' is not callable", cbname);
		efree(cbname);
		RETURN_FALSE;
	}
	efree(cbname);
	
	H = (pdo_sqlite_db_handle *)dbh->driver_data;

	func = (struct pdo_sqlite_func*)ecalloc(1, sizeof(*func));

	ret = sqlite3_create_function(H->db, func_name, argc, SQLITE_UTF8,
			func, php_sqlite3_func_callback, NULL, NULL);

	if (ret == SQLITE_OK) {
		func->funcname = estrdup(func_name);
		
		MAKE_STD_ZVAL(func->func);
		*(func->func) = *callback;
		zval_copy_ctor(func->func);
		
		func->argc = argc;

		func->next = H->funcs;
		H->funcs = func;

		RETURN_TRUE;
	}

	efree(func);
	RETURN_FALSE;
}
/* }}} */

static function_entry dbh_methods[] = {
	PHP_ME(SQLite, sqliteCreateFunction, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static function_entry *get_driver_methods(pdo_dbh_t *dbh, int kind TSRMLS_DC)
{
	printf("get_driver_methods\n");
	switch (kind) {
		case PDO_DBH_DRIVER_METHOD_KIND_DBH:
			return dbh_methods;

		default:
			return NULL;
	}
}

static struct pdo_dbh_methods sqlite_methods = {
	sqlite_handle_closer,
	sqlite_handle_preparer,
	sqlite_handle_doer,
	sqlite_handle_quoter,
	sqlite_handle_begin,
	sqlite_handle_commit,
	sqlite_handle_rollback,
	pdo_sqlite_set_attr,
	pdo_sqlite_last_insert_id,
	pdo_sqlite_fetch_error_func,
	pdo_sqlite_get_attribute,
	NULL,	/* check_liveness: not needed */
	get_driver_methods
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
		zend_throw_exception_ex(php_pdo_get_exception(), 0 TSRMLS_CC,
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
