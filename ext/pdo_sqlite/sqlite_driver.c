/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2013 The PHP Group                                |
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
		if (einfo->errmsg) {
			pefree(einfo->errmsg, dbh->is_persistent);
		}
		einfo->errmsg = pestrdup((char*)sqlite3_errmsg(H->db), dbh->is_persistent);
	} else { /* no error */
		strncpy(*pdo_err, PDO_ERR_NONE, sizeof(PDO_ERR_NONE));
		return 0;
	}
	switch (einfo->errcode) {
		case SQLITE_NOTFOUND:
			strncpy(*pdo_err, "42S02", sizeof("42S02"));
			break;	

		case SQLITE_INTERRUPT:
			strncpy(*pdo_err, "01002", sizeof("01002"));
			break;

		case SQLITE_NOLFS:
			strncpy(*pdo_err, "HYC00", sizeof("HYC00"));
			break;

		case SQLITE_TOOBIG:
			strncpy(*pdo_err, "22001", sizeof("22001"));
			break;
	
		case SQLITE_CONSTRAINT:
			strncpy(*pdo_err, "23000", sizeof("23000"));
			break;

		case SQLITE_ERROR:
		default:
			strncpy(*pdo_err, "HY000", sizeof("HY000"));
			break;
	}

	if (!dbh->methods) {
		zend_throw_exception_ex(php_pdo_get_exception(), einfo->errcode TSRMLS_CC, "SQLSTATE[%s] [%d] %s",
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

	while (H->collations) {
		struct pdo_sqlite_collation *collation;
		collation = H->collations;
		H->collations = collation->next;

		if (H->db) {
			/* delete the collation from the handle */
			sqlite3_create_collation(H->db,
				collation->name,
				SQLITE_UTF8,
				collation,
				NULL);
		}

		efree((char*)collation->name);
		if (collation->callback) {
			zval_ptr_dtor(&collation->callback);
		}
		efree(collation);
	}
}

static int sqlite_handle_closer(pdo_dbh_t *dbh TSRMLS_DC) /* {{{ */
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;
	
	if (H) {
		pdo_sqlite_error_info *einfo = &H->einfo;

		pdo_sqlite_cleanup_callbacks(H TSRMLS_CC);
		if (H->db) {
			sqlite3_close(H->db);
			H->db = NULL;
		}
		if (einfo->errmsg) {
			pefree(einfo->errmsg, dbh->is_persistent);
			einfo->errmsg = NULL;
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
		int argc, sqlite3_value **argv, sqlite3_context *context,
		int is_agg TSRMLS_DC)
{
	zval ***zargs = NULL;
	zval *retval = NULL;
	int i;
	int ret;
	int fake_argc;
	zval **agg_context = NULL;
	
	if (is_agg) {
		is_agg = 2;
	}
	
	fake_argc = argc + is_agg;
	
	fc->fci.size = sizeof(fc->fci);
	fc->fci.function_table = EG(function_table);
	fc->fci.function_name = cb;
	fc->fci.symbol_table = NULL;
	fc->fci.object_ptr = NULL;
	fc->fci.retval_ptr_ptr = &retval;
	fc->fci.param_count = fake_argc;
	
	/* build up the params */

	if (fake_argc) {
		zargs = (zval ***)safe_emalloc(fake_argc, sizeof(zval **), 0);
	}

	if (is_agg) {
		/* summon the aggregation context */
		agg_context = (zval**)sqlite3_aggregate_context(context, sizeof(zval*));
		if (!*agg_context) {
			MAKE_STD_ZVAL(*agg_context);
			ZVAL_NULL(*agg_context);
		}
		zargs[0] = agg_context;

		zargs[1] = emalloc(sizeof(zval*));
		MAKE_STD_ZVAL(*zargs[1]);
		ZVAL_LONG(*zargs[1], sqlite3_aggregate_count(context));
	}
	
	for (i = 0; i < argc; i++) {
		zargs[i + is_agg] = emalloc(sizeof(zval *));
		MAKE_STD_ZVAL(*zargs[i + is_agg]);

		/* get the value */
		switch (sqlite3_value_type(argv[i])) {
			case SQLITE_INTEGER:
				ZVAL_LONG(*zargs[i + is_agg], sqlite3_value_int(argv[i]));
				break;

			case SQLITE_FLOAT:
				ZVAL_DOUBLE(*zargs[i + is_agg], sqlite3_value_double(argv[i]));
				break;

			case SQLITE_NULL:
				ZVAL_NULL(*zargs[i + is_agg]);
				break;

			case SQLITE_BLOB:
			case SQLITE3_TEXT:
			default:
				ZVAL_STRINGL(*zargs[i + is_agg], (char*)sqlite3_value_text(argv[i]),
						sqlite3_value_bytes(argv[i]), 1);
				break;
		}
	}


	fc->fci.params = zargs;


	if ((ret = zend_call_function(&fc->fci, &fc->fcc TSRMLS_CC)) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "An error occurred while invoking the callback");
	}

	/* clean up the params */
	if (zargs) {
		for (i = is_agg; i < fake_argc; i++) {
			zval_ptr_dtor(zargs[i]);
			efree(zargs[i]);
		}
		if (is_agg) {
			zval_ptr_dtor(zargs[1]);
			efree(zargs[1]);
		}
		efree(zargs);
	}

	if (!is_agg || !argv) {
		/* only set the sqlite return value if we are a scalar function,
		 * or if we are finalizing an aggregate */
		if (retval) {
			switch (Z_TYPE_P(retval)) {
				case IS_LONG:
					sqlite3_result_int(context, Z_LVAL_P(retval));
					break;

				case IS_NULL:
					sqlite3_result_null(context);
					break;

				case IS_DOUBLE:
					sqlite3_result_double(context, Z_DVAL_P(retval));
					break;

				default:
					convert_to_string_ex(&retval);
					sqlite3_result_text(context, Z_STRVAL_P(retval),
						Z_STRLEN_P(retval), SQLITE_TRANSIENT);
					break;
			}
		} else {
			sqlite3_result_error(context, "failed to invoke callback", 0);
		}

		if (agg_context) {
			zval_ptr_dtor(agg_context);
		}
	} else {
		/* we're stepping in an aggregate; the return value goes into
		 * the context */
		if (agg_context) {
			zval_ptr_dtor(agg_context);
		}
		if (retval) {
			*agg_context = retval;
			retval = NULL;
		} else {
			*agg_context = NULL;
		}
	}

	if (retval) {
		zval_ptr_dtor(&retval);
	}

	return ret;
}

static void php_sqlite3_func_callback(sqlite3_context *context, int argc,
	sqlite3_value **argv)
{
	struct pdo_sqlite_func *func = (struct pdo_sqlite_func*)sqlite3_user_data(context);
	TSRMLS_FETCH();

	do_callback(&func->afunc, func->func, argc, argv, context, 0 TSRMLS_CC);
}

static void php_sqlite3_func_step_callback(sqlite3_context *context, int argc,
	sqlite3_value **argv)
{
	struct pdo_sqlite_func *func = (struct pdo_sqlite_func*)sqlite3_user_data(context);
	TSRMLS_FETCH();

	do_callback(&func->astep, func->step, argc, argv, context, 1 TSRMLS_CC);
}

static void php_sqlite3_func_final_callback(sqlite3_context *context)
{
	struct pdo_sqlite_func *func = (struct pdo_sqlite_func*)sqlite3_user_data(context);
	TSRMLS_FETCH();

	do_callback(&func->afini, func->fini, 0, NULL, context, 1 TSRMLS_CC);
}

static int php_sqlite3_collation_callback(void *context,
	int string1_len, const void *string1,
	int string2_len, const void *string2)
{
	int ret;
	zval *zstring1, *zstring2;
	zval **zargs[2];
	zval *retval = NULL;
	struct pdo_sqlite_collation *collation = (struct pdo_sqlite_collation*) context;
	TSRMLS_FETCH();

	collation->fc.fci.size = sizeof(collation->fc.fci);
	collation->fc.fci.function_table = EG(function_table);
	collation->fc.fci.function_name = collation->callback;
	collation->fc.fci.symbol_table = NULL;
	collation->fc.fci.object_ptr = NULL;
	collation->fc.fci.retval_ptr_ptr = &retval;

	// Prepare the arguments.
	MAKE_STD_ZVAL(zstring1);
	ZVAL_STRINGL(zstring1, (char *) string1, string1_len, 1);
	zargs[0] = &zstring1;
	MAKE_STD_ZVAL(zstring2);
	ZVAL_STRINGL(zstring2, (char *) string2, string2_len, 1);
	zargs[1] = &zstring2;
	collation->fc.fci.param_count = 2;
	collation->fc.fci.params = zargs;

	if ((ret = zend_call_function(&collation->fc.fci, &collation->fc.fcc TSRMLS_CC)) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "An error occurred while invoking the callback");
	}
	else if (retval) {
		if (Z_TYPE_P(retval) != IS_LONG) {
			convert_to_long_ex(&retval);
		}
		ret = 0;
		if (Z_LVAL_P(retval) > 0) {
			ret = 1;
		}
		else if (Z_LVAL_P(retval) < 0) {
			ret = -1;
		}
		zval_ptr_dtor(&retval);
	}

	zval_ptr_dtor(zargs[0]);
	zval_ptr_dtor(zargs[1]);

	return ret;
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
	PDO_CONSTRUCT_CHECK;

	if (!zend_is_callable(callback, 0, &cbname TSRMLS_CC)) {
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
		MAKE_COPY_ZVAL(&callback, func->func);
		
		func->argc = argc;

		func->next = H->funcs;
		H->funcs = func;

		RETURN_TRUE;
	}

	efree(func);
	RETURN_FALSE;
}
/* }}} */

/* {{{ bool SQLite::sqliteCreateAggregate(string name, mixed step, mixed fini [, int argcount])
   Registers a UDF with the sqlite db handle */

/* The step function should have the prototype:
   mixed step(mixed $context, int $rownumber, $value [, $value2 [, ...]])

   $context will be null for the first row; on subsequent rows it will have
   the value that was previously returned from the step function; you should
   use this to maintain state for the aggregate.

   The fini function should have the prototype:
   mixed fini(mixed $context, int $rownumber)

   $context will hold the return value from the very last call to the step function.
   rownumber will hold the number of rows over which the aggregate was performed.
   The return value of this function will be used as the return value for this
   aggregate UDF.
*/
   
static PHP_METHOD(SQLite, sqliteCreateAggregate)
{
	struct pdo_sqlite_func *func;
	zval *step_callback, *fini_callback;
	char *func_name;
	int func_name_len;
	long argc = -1;
	char *cbname = NULL;
	pdo_dbh_t *dbh;
	pdo_sqlite_db_handle *H;
	int ret;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "szz|l",
			&func_name, &func_name_len, &step_callback, &fini_callback, &argc)) {
		RETURN_FALSE;
	}
	
	dbh = zend_object_store_get_object(getThis() TSRMLS_CC);
	PDO_CONSTRUCT_CHECK;

	if (!zend_is_callable(step_callback, 0, &cbname TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "function '%s' is not callable", cbname);
		efree(cbname);
		RETURN_FALSE;
	}
	efree(cbname);
	if (!zend_is_callable(fini_callback, 0, &cbname TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "function '%s' is not callable", cbname);
		efree(cbname);
		RETURN_FALSE;
	}
	efree(cbname);
	
	H = (pdo_sqlite_db_handle *)dbh->driver_data;

	func = (struct pdo_sqlite_func*)ecalloc(1, sizeof(*func));

	ret = sqlite3_create_function(H->db, func_name, argc, SQLITE_UTF8,
			func, NULL, php_sqlite3_func_step_callback, php_sqlite3_func_final_callback);
	if (ret == SQLITE_OK) {
		func->funcname = estrdup(func_name);
		
		MAKE_STD_ZVAL(func->step);
		MAKE_COPY_ZVAL(&step_callback, func->step);

		MAKE_STD_ZVAL(func->fini);
		MAKE_COPY_ZVAL(&fini_callback, func->fini);
		
		func->argc = argc;

		func->next = H->funcs;
		H->funcs = func;

		RETURN_TRUE;
	}

	efree(func);
	RETURN_FALSE;
}
/* }}} */

/* {{{ bool SQLite::sqliteCreateCollation(string name, mixed callback)
   Registers a collation with the sqlite db handle */
static PHP_METHOD(SQLite, sqliteCreateCollation)
{
	struct pdo_sqlite_collation *collation;
	zval *callback;
	char *collation_name;
	int collation_name_len;
	char *cbname = NULL;
	pdo_dbh_t *dbh;
	pdo_sqlite_db_handle *H;
	int ret;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz",
		&collation_name, &collation_name_len, &callback)) {
		RETURN_FALSE;
	}

	dbh = zend_object_store_get_object(getThis() TSRMLS_CC);
	PDO_CONSTRUCT_CHECK;

	if (!zend_is_callable(callback, 0, &cbname TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "function '%s' is not callable", cbname);
		efree(cbname);
		RETURN_FALSE;
	}
	efree(cbname);

	H = (pdo_sqlite_db_handle *)dbh->driver_data;

	collation = (struct pdo_sqlite_collation*)ecalloc(1, sizeof(*collation));

	ret = sqlite3_create_collation(H->db, collation_name, SQLITE_UTF8, collation, php_sqlite3_collation_callback);
	if (ret == SQLITE_OK) {
		collation->name = estrdup(collation_name);

		MAKE_STD_ZVAL(collation->callback);
		MAKE_COPY_ZVAL(&callback, collation->callback);

		collation->next = H->collations;
		H->collations = collation;

		RETURN_TRUE;
	}

	efree(collation);
	RETURN_FALSE;
}
/* }}} */

static const zend_function_entry dbh_methods[] = {
	PHP_ME(SQLite, sqliteCreateFunction, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(SQLite, sqliteCreateAggregate, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(SQLite, sqliteCreateCollation, NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

static const zend_function_entry *get_driver_methods(pdo_dbh_t *dbh, int kind TSRMLS_DC)
{
	switch (kind) {
		case PDO_DBH_DRIVER_METHOD_KIND_DBH:
			return dbh_methods;

		default:
			return NULL;
	}
}

static void pdo_sqlite_request_shutdown(pdo_dbh_t *dbh TSRMLS_DC)
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;
	/* unregister functions, so that they don't linger for the next
	 * request */
	if (H) {
		pdo_sqlite_cleanup_callbacks(H TSRMLS_CC);
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
	get_driver_methods,
	pdo_sqlite_request_shutdown
};

static char *make_filename_safe(const char *filename TSRMLS_DC)
{
	if (*filename && memcmp(filename, ":memory:", sizeof(":memory:"))) {
		char *fullpath = expand_filepath(filename, NULL TSRMLS_CC);

		if (!fullpath) {
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
			"open_basedir prohibits opening %s",
			dbh->data_source);
		goto cleanup;
	}

	i = sqlite3_open(filename, &H->db);
	efree(filename);

	if (i != SQLITE_OK) {
		pdo_sqlite_error(dbh);
		goto cleanup;
	}

	if (PG(open_basedir) && *PG(open_basedir)) {
		sqlite3_set_authorizer(H->db, authorizer, NULL);
	}

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
