/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://www.php.net/license/3_01.txt                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wez Furlong <wez@php.net>                                    |
  +----------------------------------------------------------------------+
*/

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
#include "sqlite_driver_arginfo.h"

int _pdo_sqlite_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt, const char *file, int line) /* {{{ */
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
		strncpy(*pdo_err, PDO_ERR_NONE, sizeof(*pdo_err));
		return 0;
	}
	switch (einfo->errcode) {
		case SQLITE_NOTFOUND:
			strncpy(*pdo_err, "42S02", sizeof(*pdo_err));
			break;

		case SQLITE_INTERRUPT:
			strncpy(*pdo_err, "01002", sizeof(*pdo_err));
			break;

		case SQLITE_NOLFS:
			strncpy(*pdo_err, "HYC00", sizeof(*pdo_err));
			break;

		case SQLITE_TOOBIG:
			strncpy(*pdo_err, "22001", sizeof(*pdo_err));
			break;

		case SQLITE_CONSTRAINT:
			strncpy(*pdo_err, "23000", sizeof(*pdo_err));
			break;

		case SQLITE_ERROR:
		default:
			strncpy(*pdo_err, "HY000", sizeof(*pdo_err));
			break;
	}

	if (!dbh->methods) {
		pdo_throw_exception(einfo->errcode, einfo->errmsg, pdo_err);
	}

	return einfo->errcode;
}
/* }}} */

static void pdo_sqlite_fetch_error_func(pdo_dbh_t *dbh, pdo_stmt_t *stmt, zval *info)
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;
	pdo_sqlite_error_info *einfo = &H->einfo;

	if (einfo->errcode) {
		add_next_index_long(info, einfo->errcode);
		add_next_index_string(info, einfo->errmsg);
	}
}

static void pdo_sqlite_cleanup_callbacks(pdo_sqlite_db_handle *H)
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
		if (!Z_ISUNDEF(func->func)) {
			zval_ptr_dtor(&func->func);
		}
		if (!Z_ISUNDEF(func->step)) {
			zval_ptr_dtor(&func->step);
		}
		if (!Z_ISUNDEF(func->fini)) {
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
		if (!Z_ISUNDEF(collation->callback)) {
			zval_ptr_dtor(&collation->callback);
		}
		efree(collation);
	}
}

static void sqlite_handle_closer(pdo_dbh_t *dbh) /* {{{ */
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;

	if (H) {
		pdo_sqlite_error_info *einfo = &H->einfo;

		pdo_sqlite_cleanup_callbacks(H);
		if (H->db) {
#ifdef HAVE_SQLITE3_CLOSE_V2
			sqlite3_close_v2(H->db);
#else
			sqlite3_close(H->db);
#endif
			H->db = NULL;
		}
		if (einfo->errmsg) {
			pefree(einfo->errmsg, dbh->is_persistent);
			einfo->errmsg = NULL;
		}
		pefree(H, dbh->is_persistent);
		dbh->driver_data = NULL;
	}
}
/* }}} */

static bool sqlite_handle_preparer(pdo_dbh_t *dbh, zend_string *sql, pdo_stmt_t *stmt, zval *driver_options)
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;
	pdo_sqlite_stmt *S = ecalloc(1, sizeof(pdo_sqlite_stmt));
	int i;
	const char *tail;

	S->H = H;
	stmt->driver_data = S;
	stmt->methods = &sqlite_stmt_methods;
	stmt->supports_placeholders = PDO_PLACEHOLDER_POSITIONAL|PDO_PLACEHOLDER_NAMED;

	if (PDO_CURSOR_FWDONLY != pdo_attr_lval(driver_options, PDO_ATTR_CURSOR, PDO_CURSOR_FWDONLY)) {
		H->einfo.errcode = SQLITE_ERROR;
		pdo_sqlite_error(dbh);
		return false;
	}

	i = sqlite3_prepare_v2(H->db, ZSTR_VAL(sql), ZSTR_LEN(sql), &S->stmt, &tail);
	if (i == SQLITE_OK) {
		return true;
	}

	pdo_sqlite_error(dbh);

	return false;
}

static zend_long sqlite_handle_doer(pdo_dbh_t *dbh, const zend_string *sql)
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;

	if (sqlite3_exec(H->db, ZSTR_VAL(sql), NULL, NULL, NULL) != SQLITE_OK) {
		pdo_sqlite_error(dbh);
		return -1;
	} else {
		return sqlite3_changes(H->db);
	}
}

static zend_string *pdo_sqlite_last_insert_id(pdo_dbh_t *dbh, const zend_string *name)
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;

	return zend_i64_to_str(sqlite3_last_insert_rowid(H->db));
}

/* NB: doesn't handle binary strings... use prepared stmts for that */
static zend_string* sqlite_handle_quoter(pdo_dbh_t *dbh, const zend_string *unquoted, enum pdo_param_type paramtype)
{
	char *quoted;
	if (ZSTR_LEN(unquoted) > (INT_MAX - 3) / 2) {
		return NULL;
	}
	quoted = safe_emalloc(2, ZSTR_LEN(unquoted), 3);
	/* TODO use %Q format? */
	sqlite3_snprintf(2*ZSTR_LEN(unquoted) + 3, quoted, "'%q'", ZSTR_VAL(unquoted));
	zend_string *quoted_str = zend_string_init(quoted, strlen(quoted), 0);
	efree(quoted);
	return quoted_str;
}

static bool sqlite_handle_begin(pdo_dbh_t *dbh)
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;

	if (sqlite3_exec(H->db, "BEGIN", NULL, NULL, NULL) != SQLITE_OK) {
		pdo_sqlite_error(dbh);
		return false;
	}
	return true;
}

static bool sqlite_handle_commit(pdo_dbh_t *dbh)
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;

	if (sqlite3_exec(H->db, "COMMIT", NULL, NULL, NULL) != SQLITE_OK) {
		pdo_sqlite_error(dbh);
		return false;
	}
	return true;
}

static bool sqlite_handle_rollback(pdo_dbh_t *dbh)
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;

	if (sqlite3_exec(H->db, "ROLLBACK", NULL, NULL, NULL) != SQLITE_OK) {
		pdo_sqlite_error(dbh);
		return false;
	}
	return true;
}

static int pdo_sqlite_get_attribute(pdo_dbh_t *dbh, zend_long attr, zval *return_value)
{
	switch (attr) {
		case PDO_ATTR_CLIENT_VERSION:
		case PDO_ATTR_SERVER_VERSION:
			ZVAL_STRING(return_value, (char *)sqlite3_libversion());
			break;

		default:
			return 0;
	}

	return 1;
}

static bool pdo_sqlite_set_attr(pdo_dbh_t *dbh, zend_long attr, zval *val)
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;
	zend_long lval;

	switch (attr) {
		case PDO_ATTR_TIMEOUT:
			if (!pdo_get_long_param(&lval, val)) {
				return false;
			}
			sqlite3_busy_timeout(H->db, lval * 1000);
			return true;
		case PDO_SQLITE_ATTR_EXTENDED_RESULT_CODES:
			if (!pdo_get_long_param(&lval, val)) {
				return false;
			}
			sqlite3_extended_result_codes(H->db, lval);
			return true;
	}
	return false;
}

typedef struct {
	zval val;
	zend_long row;
} aggregate_context;

static int do_callback(struct pdo_sqlite_fci *fc, zval *cb,
		int argc, sqlite3_value **argv, sqlite3_context *context,
		int is_agg)
{
	zval *zargs = NULL;
	zval retval;
	int i;
	int ret;
	int fake_argc;
	aggregate_context *agg_context = NULL;

	if (is_agg) {
		is_agg = 2;
	}

	fake_argc = argc + is_agg;

	fc->fci.size = sizeof(fc->fci);
	ZVAL_COPY_VALUE(&fc->fci.function_name, cb);
	fc->fci.object = NULL;
	fc->fci.retval = &retval;
	fc->fci.param_count = fake_argc;

	/* build up the params */

	if (fake_argc) {
		zargs = safe_emalloc(fake_argc, sizeof(zval), 0);
	}

	if (is_agg) {
		agg_context = sqlite3_aggregate_context(context, sizeof(aggregate_context));
		if (!agg_context) {
			efree(zargs);
			return FAILURE;
		}
		if (Z_ISUNDEF(agg_context->val)) {
			ZVAL_NEW_REF(&agg_context->val, &EG(uninitialized_zval));
		}
		ZVAL_COPY_VALUE(&zargs[0], &agg_context->val);
		ZVAL_LONG(&zargs[1], ++agg_context->row);
	}

	for (i = 0; i < argc; i++) {
		/* get the value */
		switch (sqlite3_value_type(argv[i])) {
			case SQLITE_INTEGER:
				ZVAL_LONG(&zargs[i + is_agg], sqlite3_value_int(argv[i]));
				break;

			case SQLITE_FLOAT:
				ZVAL_DOUBLE(&zargs[i + is_agg], sqlite3_value_double(argv[i]));
				break;

			case SQLITE_NULL:
				ZVAL_NULL(&zargs[i + is_agg]);
				break;

			case SQLITE_BLOB:
			case SQLITE3_TEXT:
			default:
				ZVAL_STRINGL(&zargs[i + is_agg], (char*)sqlite3_value_text(argv[i]), sqlite3_value_bytes(argv[i]));
				break;
		}
	}

	fc->fci.params = zargs;

	if ((ret = zend_call_function(&fc->fci, &fc->fcc)) == FAILURE) {
		php_error_docref(NULL, E_WARNING, "An error occurred while invoking the callback");
	}

	/* clean up the params */
	if (zargs) {
		for (i = is_agg; i < fake_argc; i++) {
			zval_ptr_dtor(&zargs[i]);
		}
		if (is_agg) {
			zval_ptr_dtor(&zargs[1]);
		}
		efree(zargs);
	}

	if (!is_agg || !argv) {
		/* only set the sqlite return value if we are a scalar function,
		 * or if we are finalizing an aggregate */
		if (!Z_ISUNDEF(retval)) {
			switch (Z_TYPE(retval)) {
				case IS_LONG:
					sqlite3_result_int(context, Z_LVAL(retval));
					break;

				case IS_NULL:
					sqlite3_result_null(context);
					break;

				case IS_DOUBLE:
					sqlite3_result_double(context, Z_DVAL(retval));
					break;

				default:
					if (!try_convert_to_string(&retval)) {
						ret = FAILURE;
						break;
					}
					sqlite3_result_text(context, Z_STRVAL(retval), Z_STRLEN(retval), SQLITE_TRANSIENT);
					break;
			}
		} else {
			sqlite3_result_error(context, "failed to invoke callback", 0);
		}

		if (agg_context) {
			zval_ptr_dtor(&agg_context->val);
		}
	} else {
		/* we're stepping in an aggregate; the return value goes into
		 * the context */
		if (agg_context) {
			if (Z_ISUNDEF(retval)) {
				zval_ptr_dtor(&agg_context->val);
				return FAILURE;
			}
			zval_ptr_dtor(Z_REFVAL(agg_context->val));
			ZVAL_COPY_VALUE(Z_REFVAL(agg_context->val), &retval);
			ZVAL_UNDEF(&retval);
		}
	}

	if (!Z_ISUNDEF(retval)) {
		zval_ptr_dtor(&retval);
	}

	return ret;
}

static void php_sqlite3_func_callback(sqlite3_context *context, int argc,
	sqlite3_value **argv)
{
	struct pdo_sqlite_func *func = (struct pdo_sqlite_func*)sqlite3_user_data(context);

	do_callback(&func->afunc, &func->func, argc, argv, context, 0);
}

static void php_sqlite3_func_step_callback(sqlite3_context *context, int argc,
	sqlite3_value **argv)
{
	struct pdo_sqlite_func *func = (struct pdo_sqlite_func*)sqlite3_user_data(context);

	do_callback(&func->astep, &func->step, argc, argv, context, 1);
}

static void php_sqlite3_func_final_callback(sqlite3_context *context)
{
	struct pdo_sqlite_func *func = (struct pdo_sqlite_func*)sqlite3_user_data(context);

	do_callback(&func->afini, &func->fini, 0, NULL, context, 1);
}

static int php_sqlite3_collation_callback(void *context,
	int string1_len, const void *string1,
	int string2_len, const void *string2)
{
	int ret;
	zval zargs[2];
	zval retval;
	struct pdo_sqlite_collation *collation = (struct pdo_sqlite_collation*) context;

	collation->fc.fci.size = sizeof(collation->fc.fci);
	ZVAL_COPY_VALUE(&collation->fc.fci.function_name, &collation->callback);
	collation->fc.fci.object = NULL;
	collation->fc.fci.retval = &retval;

	// Prepare the arguments.
	ZVAL_STRINGL(&zargs[0], (char *) string1, string1_len);
	ZVAL_STRINGL(&zargs[1], (char *) string2, string2_len);
	collation->fc.fci.param_count = 2;
	collation->fc.fci.params = zargs;

	if ((ret = zend_call_function(&collation->fc.fci, &collation->fc.fcc)) == FAILURE) {
		php_error_docref(NULL, E_WARNING, "An error occurred while invoking the callback");
	} else if (!Z_ISUNDEF(retval)) {
		if (Z_TYPE(retval) != IS_LONG) {
			convert_to_long(&retval);
		}
		ret = 0;
		if (Z_LVAL(retval) > 0) {
			ret = 1;
		} else if (Z_LVAL(retval) < 0) {
			ret = -1;
		}
		zval_ptr_dtor(&retval);
	}

	zval_ptr_dtor(&zargs[0]);
	zval_ptr_dtor(&zargs[1]);

	return ret;
}

/* {{{ bool SQLite::sqliteCreateFunction(string name, callable callback [, int argcount, int flags])
   Registers a UDF with the sqlite db handle */
PHP_METHOD(PDO_SQLite_Ext, sqliteCreateFunction)
{
	struct pdo_sqlite_func *func;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	char *func_name;
	size_t func_name_len;
	zend_long argc = -1;
	zend_long flags = 0;
	pdo_dbh_t *dbh;
	pdo_sqlite_db_handle *H;
	int ret;

	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_STRING(func_name, func_name_len)
		Z_PARAM_FUNC(fci, fcc)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(argc)
		Z_PARAM_LONG(flags)
	ZEND_PARSE_PARAMETERS_END();

	dbh = Z_PDO_DBH_P(ZEND_THIS);
	PDO_CONSTRUCT_CHECK;

	H = (pdo_sqlite_db_handle *)dbh->driver_data;

	func = (struct pdo_sqlite_func*)ecalloc(1, sizeof(*func));

	ret = sqlite3_create_function(H->db, func_name, argc, flags | SQLITE_UTF8,
			func, php_sqlite3_func_callback, NULL, NULL);
	if (ret == SQLITE_OK) {
		func->funcname = estrdup(func_name);

		ZVAL_COPY(&func->func, &fci.function_name);

		func->argc = argc;

		func->next = H->funcs;
		H->funcs = func;

		RETURN_TRUE;
	}

	efree(func);
	RETURN_FALSE;
}
/* }}} */

/* {{{ bool SQLite::sqliteCreateAggregate(string name, callable step, callable fini [, int argcount])
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

PHP_METHOD(PDO_SQLite_Ext, sqliteCreateAggregate)
{
	struct pdo_sqlite_func *func;
	zend_fcall_info step_fci, fini_fci;
	zend_fcall_info_cache step_fcc, fini_fcc;
	char *func_name;
	size_t func_name_len;
	zend_long argc = -1;
	pdo_dbh_t *dbh;
	pdo_sqlite_db_handle *H;
	int ret;

	ZEND_PARSE_PARAMETERS_START(3, 4)
		Z_PARAM_STRING(func_name, func_name_len)
		Z_PARAM_FUNC(step_fci, step_fcc)
		Z_PARAM_FUNC(fini_fci, fini_fcc)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(argc)
	ZEND_PARSE_PARAMETERS_END();

	dbh = Z_PDO_DBH_P(ZEND_THIS);
	PDO_CONSTRUCT_CHECK;

	H = (pdo_sqlite_db_handle *)dbh->driver_data;

	func = (struct pdo_sqlite_func*)ecalloc(1, sizeof(*func));

	ret = sqlite3_create_function(H->db, func_name, argc, SQLITE_UTF8,
			func, NULL, php_sqlite3_func_step_callback, php_sqlite3_func_final_callback);
	if (ret == SQLITE_OK) {
		func->funcname = estrdup(func_name);

		ZVAL_COPY(&func->step, &step_fci.function_name);

		ZVAL_COPY(&func->fini, &fini_fci.function_name);

		func->argc = argc;

		func->next = H->funcs;
		H->funcs = func;

		RETURN_TRUE;
	}

	efree(func);
	RETURN_FALSE;
}
/* }}} */

/* {{{ bool SQLite::sqliteCreateCollation(string name, callable callback)
   Registers a collation with the sqlite db handle */
PHP_METHOD(PDO_SQLite_Ext, sqliteCreateCollation)
{
	struct pdo_sqlite_collation *collation;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	char *collation_name;
	size_t collation_name_len;
	pdo_dbh_t *dbh;
	pdo_sqlite_db_handle *H;
	int ret;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STRING(collation_name, collation_name_len)
		Z_PARAM_FUNC(fci, fcc)
	ZEND_PARSE_PARAMETERS_END();

	dbh = Z_PDO_DBH_P(ZEND_THIS);
	PDO_CONSTRUCT_CHECK;

	H = (pdo_sqlite_db_handle *)dbh->driver_data;

	collation = (struct pdo_sqlite_collation*)ecalloc(1, sizeof(*collation));

	ret = sqlite3_create_collation(H->db, collation_name, SQLITE_UTF8, collation, php_sqlite3_collation_callback);
	if (ret == SQLITE_OK) {
		collation->name = estrdup(collation_name);

		ZVAL_COPY(&collation->callback, &fci.function_name);

		collation->next = H->collations;
		H->collations = collation;

		RETURN_TRUE;
	}

	efree(collation);
	RETURN_FALSE;
}
/* }}} */

static const zend_function_entry *get_driver_methods(pdo_dbh_t *dbh, int kind)
{
	switch (kind) {
		case PDO_DBH_DRIVER_METHOD_KIND_DBH:
			return class_PDO_SQLite_Ext_methods;

		default:
			return NULL;
	}
}

static void pdo_sqlite_request_shutdown(pdo_dbh_t *dbh)
{
	pdo_sqlite_db_handle *H = (pdo_sqlite_db_handle *)dbh->driver_data;
	/* unregister functions, so that they don't linger for the next
	 * request */
	if (H) {
		pdo_sqlite_cleanup_callbacks(H);
	}
}

static void pdo_sqlite_get_gc(pdo_dbh_t *dbh, zend_get_gc_buffer *gc_buffer)
{
	pdo_sqlite_db_handle *H = dbh->driver_data;

	struct pdo_sqlite_func *func = H->funcs;
	while (func) {
		zend_get_gc_buffer_add_zval(gc_buffer, &func->func);
		zend_get_gc_buffer_add_zval(gc_buffer, &func->step);
		zend_get_gc_buffer_add_zval(gc_buffer, &func->fini);
		func = func->next;
	}

	struct pdo_sqlite_collation *collation = H->collations;
	while (collation) {
		zend_get_gc_buffer_add_zval(gc_buffer, &collation->callback);
		collation = collation->next;
	}
}

static const struct pdo_dbh_methods sqlite_methods = {
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
	pdo_sqlite_request_shutdown,
	NULL, /* in transaction, use PDO's internal tracking mechanism */
	pdo_sqlite_get_gc
};

static char *make_filename_safe(const char *filename)
{
	if (!filename) {
		return NULL;
	}
	if (*filename && strncasecmp(filename, "file:", 5) == 0) {
		if (PG(open_basedir) && *PG(open_basedir)) {
			return NULL;
		}
		return estrdup(filename);
	}
	if (*filename && memcmp(filename, ":memory:", sizeof(":memory:"))) {
		char *fullpath = expand_filepath(filename, NULL);

		if (!fullpath) {
			return NULL;
		}

		if (php_check_open_basedir(fullpath)) {
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
		case SQLITE_ATTACH: {
			filename = make_filename_safe(arg3);
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

static int pdo_sqlite_handle_factory(pdo_dbh_t *dbh, zval *driver_options) /* {{{ */
{
	pdo_sqlite_db_handle *H;
	int i, ret = 0;
	zend_long timeout = 60, flags;
	char *filename;

	H = pecalloc(1, sizeof(pdo_sqlite_db_handle), dbh->is_persistent);

	H->einfo.errcode = 0;
	H->einfo.errmsg = NULL;
	dbh->driver_data = H;

	/* skip all but this one param event */
	dbh->skip_param_evt = 0x7F ^ (1 << PDO_PARAM_EVT_EXEC_PRE);

	filename = make_filename_safe(dbh->data_source);

	if (!filename) {
		zend_throw_exception_ex(php_pdo_get_exception(), 0,
			"open_basedir prohibits opening %s",
			dbh->data_source);
		goto cleanup;
	}

	flags = pdo_attr_lval(driver_options, PDO_SQLITE_ATTR_OPEN_FLAGS, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);

	if (!(PG(open_basedir) && *PG(open_basedir))) {
		flags |= SQLITE_OPEN_URI;
	}
	i = sqlite3_open_v2(filename, &H->db, flags, NULL);

	efree(filename);

	if (i != SQLITE_OK) {
		pdo_sqlite_error(dbh);
		goto cleanup;
	}

	if (PG(open_basedir) && *PG(open_basedir)) {
		sqlite3_set_authorizer(H->db, authorizer, NULL);
	}

	if (driver_options) {
		timeout = pdo_attr_lval(driver_options, PDO_ATTR_TIMEOUT, timeout);
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

const pdo_driver_t pdo_sqlite_driver = {
	PDO_DRIVER_HEADER(sqlite),
	pdo_sqlite_handle_factory
};
