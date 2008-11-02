/*
   +----------------------------------------------------------------------+
   | PHP Version 6                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2008 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Scott MacVicar <scottmac@php.net>                           |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/file.h"
#include "php_sqlite3.h"
#include "php_sqlite3_structs.h"

#include <sqlite3.h>

#include "zend_exceptions.h"
#include "zend_interfaces.h"

ZEND_DECLARE_MODULE_GLOBALS(sqlite3)

static PHP_GINIT_FUNCTION(sqlite3);
static int php_sqlite3_authorizer(void *autharg, int access_type, const char *arg3, const char *arg4, const char *arg5, const char *arg6);
static void sqlite3_param_dtor(void *data);
static int php_sqlite3_compare_stmt_zval_free( php_sqlite3_free_list **free_list, zval *statement );

#define SQLITE3_CHECK_INITIALIZED(member, class_name) \
	if (!(member)) { \
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "The " #class_name " object has not been correctly initialised"); \
		RETURN_FALSE; \
	}

/* {{{ PHP_INI
*/
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("sqlite3.extension_dir",  NULL, PHP_INI_SYSTEM, OnUpdateString, extension_dir, zend_sqlite3_globals, sqlite3_globals)
PHP_INI_END()
/* }}} */

/* Handlers */
static zend_object_handlers sqlite3_object_handlers;
static zend_object_handlers sqlite3_stmt_object_handlers;
static zend_object_handlers sqlite3_result_object_handlers;

/* Class entries */
zend_class_entry *php_sqlite3_sc_entry;
zend_class_entry *php_sqlite3_stmt_entry;
zend_class_entry *php_sqlite3_result_entry;

/* {{{ proto bool SQLite3::open(String filename [, int Flags [, string Encryption Key]]) U
   Opens a SQLite 3 Database, if the build includes encryption then it will attempt to use the key */
PHP_METHOD(sqlite3, open)
{
	php_sqlite3_db_object *db_obj;
	zval *object = getThis();
	char *filename, *encryption_key, *fullpath;
	zend_uchar filename_type;
	int filename_len, encryption_key_len;
	long flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
	db_obj = (php_sqlite3_db_object *)zend_object_store_get_object(object TSRMLS_CC);

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "t|lS", &filename, &filename_len, &filename_type, &flags, &encryption_key, &encryption_key_len)) {
		return;
	}

	if (db_obj->initialised) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Already initialised DB Object");
	}

	if (filename_type == IS_UNICODE) {
		if (php_stream_path_encode(NULL, &filename, &filename_len, (UChar *)filename, filename_len, REPORT_ERRORS, FG(default_context)) == FAILURE) {
			RETURN_FALSE;
		}
	}

	if (strncmp(filename, ":memory:", 8) != 0) {
		if (!(fullpath = expand_filepath(filename, NULL TSRMLS_CC))) {
			if (filename_type == IS_UNICODE) {
				efree(filename);
			}
			RETURN_FALSE;
		}

		if (php_check_open_basedir(fullpath TSRMLS_CC)) {
			if (filename_type == IS_UNICODE) {
				efree(filename);
			}
			efree(fullpath);
			RETURN_FALSE;
		}
	} else {
		fullpath = estrdup(filename);
	}

	if (filename_type == IS_UNICODE) {
		efree(filename);
	}

#if SQLITE_VERSION_NUMBER >= 3005000
	if (sqlite3_open_v2(fullpath, &(db_obj->db), flags, NULL) != SQLITE_OK) {
#else
	/* Todo: utf-16 = sqlite3_open16 */
	if (sqlite3_open(fullpath, &(db_obj->db)) != SQLITE_OK) {
#endif
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Unable to open database: %s", sqlite3_errmsg(db_obj->db));
		if (fullpath) {
			efree(fullpath);
		}
		return;
	}

	db_obj->initialised = 1;

#if SQLITE_HAS_CODEC
	if (encryption_key_len > 0) {
		if (sqlite3_key(db_obj->db, encryption_key, encryption_key_len) != SQLITE_OK) {
			return;
		}
	}
#endif

	if ((PG(open_basedir) && *PG(open_basedir))) {
		sqlite3_set_authorizer(db_obj->db, php_sqlite3_authorizer, NULL);
	}

	if (fullpath) {
		efree(fullpath);
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool SQLite3::close() U
   Close a SQLite 3 Database. */
PHP_METHOD(sqlite3, close)
{
	php_sqlite3_db_object *db_obj;
	zval *object = getThis();
	int errcode;
	db_obj = (php_sqlite3_db_object *)zend_object_store_get_object(object TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (db_obj->initialised) {
		zend_llist_clean(&(db_obj->free_list));
		errcode = sqlite3_close(db_obj->db);
		if (errcode != SQLITE_OK) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to close database: %d, %s", errcode, sqlite3_errmsg(db_obj->db));
			RETURN_FALSE;
		}
		db_obj->initialised = 0;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool SQLite3::exec(String Query) U
   Executes a result-less query against a given database */
PHP_METHOD(sqlite3, exec)
{
	php_sqlite3_db_object *db_obj;
	zval *object = getThis();
	char *sql, *errtext = NULL;
	int sql_len;
	db_obj = (php_sqlite3_db_object *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(db_obj->initialised, SQLite3)

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s&", &sql, &sql_len, UG(utf8_conv))) {
		return;
	}

	if (sqlite3_exec(db_obj->db, sql, NULL, NULL, &errtext) != SQLITE_OK) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", errtext);
		sqlite3_free(errtext);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto Array SQLite3::version() U
   Returns the SQLite3 Library version as a string constant and as a number. */
PHP_METHOD(sqlite3, version)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	array_init(return_value);

	add_assoc_utf8_string(return_value, "versionString", (char*)sqlite3_libversion(), 1);
	add_assoc_long(return_value, "versionNumber", sqlite3_libversion_number());

	return;
}
/* }}} */

/* {{{ proto int SQLite3::lastInsertRowID() U
   Returns the rowid of the most recent INSERT into the database from the database connection. */
PHP_METHOD(sqlite3, lastInsertRowID)
{
	php_sqlite3_db_object *db_obj;
	zval *object = getThis();
	db_obj = (php_sqlite3_db_object *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(db_obj->initialised, SQLite3)

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(sqlite3_last_insert_rowid(db_obj->db));
}
/* }}} */

/* {{{ proto int SQLite3::lastErrorCode() U
   Returns the numeric result code of the most recent failed sqlite API call for the database connection. */
PHP_METHOD(sqlite3, lastErrorCode)
{
	php_sqlite3_db_object *db_obj;
	zval *object = getThis();
	db_obj = (php_sqlite3_db_object *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(db_obj->initialised, SQLite3)

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(sqlite3_errcode(db_obj->db));
}
/* }}} */

/* {{{ proto string SQLite3::lastErrorMsg() U
   Returns english text describing the most recent failed sqlite API call for the database connection. */
PHP_METHOD(sqlite3, lastErrorMsg)
{
	php_sqlite3_db_object *db_obj;
	zval *object = getThis();
	db_obj = (php_sqlite3_db_object *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(db_obj->initialised, SQLite3)

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETVAL_UNICODE((UChar *)sqlite3_errmsg16(db_obj->db), 1);
}
/* }}} */

/* {{{ proto bool SQLite3::loadExtension(String Shared Library)
   Attempts to load an SQLite extension library */
PHP_METHOD(sqlite3, loadExtension)
{
	php_sqlite3_db_object *db_obj;
	zval *object = getThis();
	char *extension, *lib_path, *extension_dir, *errtext = NULL;
	char fullpath[MAXPATHLEN];
	int extension_len, extension_dir_len;
	db_obj = (php_sqlite3_db_object *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(db_obj->initialised, SQLite3)

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &extension, &extension_len)) {
		return;
	}

	if (!SQLITE3G(extension_dir)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "SQLite Extension are disabled");
		RETURN_FALSE;
	}

	if (extension_len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty string as an extension");
		RETURN_FALSE;
	}

	extension_dir = SQLITE3G(extension_dir);
	extension_dir_len = strlen(SQLITE3G(extension_dir));

	if (IS_SLASH(extension_dir[extension_dir_len-1])) {
		spprintf(&lib_path, 0, "%s%s", extension_dir, extension);
	} else {
		spprintf(&lib_path, 0, "%s%c%s", extension_dir, DEFAULT_SLASH, extension);
	}

	if (!VCWD_REALPATH(lib_path, fullpath)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to load extension at '%s'", lib_path);
		efree(lib_path);
		RETURN_FALSE;
	}

	efree(lib_path);

	if (strncmp(fullpath, extension_dir, extension_dir_len) != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to open extensions outside the defined directory");
		RETURN_FALSE;
	}

	/* Extension loading should only be enabled for when we attempt to load */
	sqlite3_enable_load_extension(db_obj->db, 1);
	if (sqlite3_load_extension(db_obj->db, fullpath, 0, &errtext) != SQLITE_OK) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", errtext);
		sqlite3_free(errtext);
		sqlite3_enable_load_extension(db_obj->db, 0);
		RETURN_FALSE;
	}
	sqlite3_enable_load_extension(db_obj->db, 0);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int SQLite3::changes() U
  Returns the number of database rows that were changed (or inserted or deleted) by the most recent SQL statement. */
PHP_METHOD(sqlite3, changes)
{
	php_sqlite3_db_object *db_obj;
	zval *object = getThis();
	db_obj = (php_sqlite3_db_object *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(db_obj->initialised, SQLite3)

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(sqlite3_changes(db_obj->db));
}
/* }}} */

/* {{{ proto String SQLite3::escapeString(String value) U
   Returns a string that has been properly escaped. */
PHP_METHOD(sqlite3, escapeString)
{
	char *sql, *ret;
	int sql_len;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s&", &sql, &sql_len, UG(utf8_conv))) {
		return;
	}

	if (sql_len) {
		ret = sqlite3_mprintf("%q", sql);
		if (ret) {
			RETVAL_STRING(ret, 1);
			sqlite3_free(ret);
		}
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ proto SQLite3Stmt SQLite3::prepare(String Query)
   Returns a prepared SQL statement for execution. */
PHP_METHOD(sqlite3, prepare)
{
	php_sqlite3_db_object *db_obj;
	php_sqlite3_stmt *stmt_obj;
	zval *object = getThis();
	char *sql;
	int sql_len, errcode;
	db_obj = (php_sqlite3_db_object *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(db_obj->initialised, SQLite3)

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &sql, &sql_len)) {
		return;
	}

	if (!sql_len) {
		RETURN_FALSE;
	}

	object_init_ex(return_value, php_sqlite3_stmt_entry);
	stmt_obj = (php_sqlite3_stmt *)zend_object_store_get_object(return_value TSRMLS_CC);
	stmt_obj->db_obj = db_obj;
	stmt_obj->db_obj_zval = getThis();

	Z_ADDREF_P(object);
	
	/* Todo: utf-8 or utf-16 = sqlite3_prepare16_v2 */
	errcode = sqlite3_prepare_v2(db_obj->db, sql, sql_len, &(stmt_obj->stmt), NULL);
	if (errcode != SQLITE_OK) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to prepare statement: %d, %s", errcode, sqlite3_errmsg(db_obj->db));
		zval_dtor(return_value);
		RETURN_FALSE;
	}
	stmt_obj->initialised = 1;
}
/* }}} */

/* {{{ proto SQLite3Result SQLite3::query(String Query)
   Returns true or false, for queries that return data it will return a SQLite3Result object. */
PHP_METHOD(sqlite3, query)
{
	php_sqlite3_db_object *db_obj;
	php_sqlite3_result *result;
	php_sqlite3_stmt *stmt_obj;
	zval *object = getThis();
	zval *stmt = NULL;
	char *sql, *errtext = NULL;
	int sql_len, return_code;
	db_obj = (php_sqlite3_db_object *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(db_obj->initialised, SQLite3)

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &sql, &sql_len)) {
		return;
	}

	if (!sql_len) {
		RETURN_FALSE;
	}

	/* If there was no return value then just execute the query */
	if (!return_value_used) {
		/* Todo: utf-8 */
		if (sqlite3_exec(db_obj->db, sql, NULL, NULL, &errtext) != SQLITE_OK) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", errtext);
			sqlite3_free(errtext);
		}
		return;
	}

	MAKE_STD_ZVAL(stmt);

	object_init_ex(stmt, php_sqlite3_stmt_entry);
	stmt_obj = (php_sqlite3_stmt *)zend_object_store_get_object(stmt TSRMLS_CC);
	stmt_obj->db_obj = db_obj;
	stmt_obj->db_obj_zval = getThis();

	Z_ADDREF_P(object);

	/* Todo: utf-8 or utf-16 = sqlite3_prepare16_v2 */
	return_code = sqlite3_prepare_v2(db_obj->db, sql, sql_len, &(stmt_obj->stmt), NULL);
	if (return_code != SQLITE_OK) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to prepare statement: %d, %s", return_code, sqlite3_errmsg(db_obj->db));
		zval_ptr_dtor(&stmt);
		RETURN_FALSE;
	}

	stmt_obj->initialised = 1;

	object_init_ex(return_value, php_sqlite3_result_entry);
	result = (php_sqlite3_result *)zend_object_store_get_object(return_value TSRMLS_CC);
	result->db_obj = db_obj;
	result->stmt_obj = stmt_obj;
	result->stmt_obj_zval = stmt;

	return_code = sqlite3_step(result->stmt_obj->stmt);

	switch (return_code) {
		case SQLITE_ROW: /* Valid Row */
		case SQLITE_DONE: /* Valid but no results */
		{
			php_sqlite3_free_list *free_item;
			free_item = emalloc(sizeof(php_sqlite3_free_list));
			free_item->stmt_obj = stmt_obj;
			free_item->stmt_obj_zval = stmt;
			zend_llist_add_element(&(db_obj->free_list), &free_item);
			sqlite3_reset(result->stmt_obj->stmt);
			break;
		}
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to execute statement: %s", sqlite3_errmsg(db_obj->db));
			sqlite3_finalize(stmt_obj->stmt);
			stmt_obj->initialised = 0;
			zval_dtor(return_value);
			RETURN_FALSE;
	}
}
/* }}} */

static zval* sqlite_value_to_zval(sqlite3_stmt *stmt, int column) /* {{{ */
{
	zval *data;
	MAKE_STD_ZVAL(data);
	switch (sqlite3_column_type(stmt, column)) {
		case SQLITE_INTEGER:
			if ((sqlite3_column_int64(stmt, column)) >= INT_MAX || sqlite3_column_int64(stmt, column) <= INT_MIN) {
				ZVAL_STRINGL(data, (char *)sqlite3_column_text(stmt, column), sqlite3_column_bytes(stmt, column), 1);
			} else {
				ZVAL_LONG(data, sqlite3_column_int64(stmt, column));
			}
			break;

		case SQLITE_FLOAT:
			ZVAL_DOUBLE(data, sqlite3_column_double(stmt, column));
			break;

		case SQLITE_NULL:
			ZVAL_NULL(data);
			break;

		case SQLITE3_TEXT:
			ZVAL_UNICODE(data, (UChar*)sqlite3_column_text16(stmt, column), 1);
			/*ZVAL_STRING(data, (char*)sqlite3_column_text(stmt, column), 1);*/
			break;

		case SQLITE_BLOB:
		default:
			ZVAL_STRINGL(data, (char*)sqlite3_column_blob(stmt, column), sqlite3_column_bytes(stmt, column), 1);
	}
	return data;
}
/* }}} */

/* {{{ proto SQLite3Result SQLite3::querySingle(String Query [, entire_row = false])
   Returns a string of the first column, or an array of the entire row. */
PHP_METHOD(sqlite3, querySingle)
{
	php_sqlite3_db_object *db_obj;
	zval *object = getThis();
	char *sql, *errtext = NULL;
	int sql_len, return_code;
	zend_bool entire_row = 0;
	sqlite3_stmt *stmt;
	db_obj = (php_sqlite3_db_object *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(db_obj->initialised, SQLite3)

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|b", &sql, &sql_len, &entire_row)) {
		return;
	}

	if (!sql_len) {
		RETURN_FALSE;
	}

	/* If there was no return value then just execute the query */
	if (!return_value_used) {
		/* Todo: utf-8 */
		if (sqlite3_exec(db_obj->db, sql, NULL, NULL, &errtext) != SQLITE_OK) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", errtext);
			sqlite3_free(errtext);
		}
		return;
	}

	/* Todo: utf-8 or utf-16 = sqlite3_prepare16_v2 */
	return_code = sqlite3_prepare_v2(db_obj->db, sql, sql_len, &stmt, NULL);
	if (return_code != SQLITE_OK) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to prepare statement: %d, %s", return_code, sqlite3_errmsg(db_obj->db));
		RETURN_FALSE;
	}

	return_code = sqlite3_step(stmt);

	switch (return_code) {
		case SQLITE_ROW: /* Valid Row */
		{
			if (!entire_row) {
				zval *data;
				data = sqlite_value_to_zval(stmt, 0);
				*return_value = *data;
				zval_copy_ctor(return_value);
				zval_dtor(data);
				FREE_ZVAL(data);
			} else {
				int i = 0;
				array_init(return_value);
				for (i = 0; i < sqlite3_data_count(stmt); i++) {
					zval *data;
					UChar *ustr = (UChar *)sqlite3_column_name16(stmt, i);
					data = sqlite_value_to_zval(stmt, i);
					
					add_u_assoc_zval(return_value, IS_UNICODE, ZSTR(ustr), data);
					/*add_assoc_zval(return_value, (char*)sqlite3_column_name(stmt, i), data);*/
				}
			}
			break;
		}
		case SQLITE_DONE: /* Valid but no results */
		{
			if (!entire_row) {
				RETVAL_NULL();
			} else {
				array_init(return_value);
			}
			break;
		}
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to execute statement: %s", sqlite3_errmsg(db_obj->db));
			RETVAL_FALSE;
	}
	sqlite3_finalize(stmt);
}
/* }}} */

static int sqlite3_do_callback(struct php_sqlite3_fci *fc, zval *cb, int argc, sqlite3_value **argv, sqlite3_context *context, int is_agg TSRMLS_DC) /* {{{ */
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
	fc->fci.object_pp = NULL;
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
				/* Todo: utf-8 / utf-16 and proper blob stuff */
				ZVAL_STRINGL(*zargs[i + is_agg], (char*)sqlite3_value_text(argv[i]), sqlite3_value_bytes(argv[i]), 1);
				break;
		}
	}

	fc->fci.params = zargs;

	if ((ret = zend_call_function(&fc->fci, &fc->fcc TSRMLS_CC)) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "An error occurred while invoking the callback");
	}

	/* clean up the params */
	if (fake_argc) {
		for (i = is_agg; i < argc + is_agg; i++) {
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
					/* Todo: utf-8 or utf-16 = sqlite3_result_text16 */
					convert_to_string_ex(&retval);
					sqlite3_result_text(context, Z_STRVAL_P(retval), Z_STRLEN_P(retval), SQLITE_TRANSIENT);
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
/* }}}*/

static void php_sqlite3_callback_func(sqlite3_context *context, int argc, sqlite3_value **argv) /* {{{ */
{
	php_sqlite3_func *func = (php_sqlite3_func *)sqlite3_user_data(context);
	TSRMLS_FETCH();

	sqlite3_do_callback(&func->afunc, func->func, argc, argv, context, 0 TSRMLS_CC);
}
/* }}}*/

static void php_sqlite3_callback_step(sqlite3_context *context, int argc, sqlite3_value **argv) /* {{{ */
{
	php_sqlite3_func *func = (php_sqlite3_func *)sqlite3_user_data(context);
	TSRMLS_FETCH();

	sqlite3_do_callback(&func->astep, func->step, argc, argv, context, 1 TSRMLS_CC);
}
/* }}} */

static void php_sqlite3_callback_final(sqlite3_context *context) /* {{{ */
{
	php_sqlite3_func *func = (php_sqlite3_func *)sqlite3_user_data(context);
	TSRMLS_FETCH();

	sqlite3_do_callback(&func->afini, func->fini, 0, NULL, context, 1 TSRMLS_CC);
}
/* }}} */

/* {{{ proto bool SQLite3::createFunction(string name, mixed callback [, int argcount]))
   Allows registration of a PHP function as a SQLite UDF that can be called within SQL statements. */
PHP_METHOD(sqlite3, createFunction)
{
	php_sqlite3_db_object *db_obj;
	zval *object = getThis();
	php_sqlite3_func *func;
	char *sql_func;
	int sql_func_len;
	zval *callback_func, callback_name;
	long sql_func_num_args = -1;
	db_obj = (php_sqlite3_db_object *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(db_obj->initialised, SQLite3)

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz|l", &sql_func, &sql_func_len, &callback_func, &sql_func_num_args) == FAILURE) {
		return;
	}

	if (!sql_func_len) {
		RETURN_FALSE;
	}

	if (!zend_is_callable(callback_func, 0, &callback_name TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Not a valid callback function %R", Z_TYPE(callback_name), Z_UNIVAL(callback_name));
		zval_dtor(&callback_name);
		RETURN_FALSE;
	}
	zval_dtor(&callback_name);

	func = (php_sqlite3_func *)ecalloc(1, sizeof(*func));

	/* Todo: utf-8 or utf-16 = sqlite3_create_function16 */
	if (sqlite3_create_function(db_obj->db, sql_func, sql_func_num_args, SQLITE_UTF8, func, php_sqlite3_callback_func, NULL, NULL) == SQLITE_OK) {
		func->func_name = estrdup(sql_func);

		MAKE_STD_ZVAL(func->func);
		*(func->func) = *callback_func;
		zval_copy_ctor(func->func);

		func->argc = sql_func_num_args;
		func->next = db_obj->funcs;
		db_obj->funcs = func;

		RETURN_TRUE;
	}
	efree(func);

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool SQLite3::createAggregate(string name, mixed step, mixed final [, int argcount]))
   Allows registration of a PHP function for use as an aggregate. */
PHP_METHOD(sqlite3, createAggregate)
{
	php_sqlite3_db_object *db_obj;
	zval *object = getThis();
	php_sqlite3_func *func;
	char *sql_func;
	int sql_func_len;
	zval *step_callback, *fini_callback, callback_name;
	long sql_func_num_args = -1;
	db_obj = (php_sqlite3_db_object *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(db_obj->initialised, SQLite3)

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "szz|l", &sql_func, &sql_func_len, &step_callback, &fini_callback, &sql_func_num_args) == FAILURE) {
		return;
	}

	if (!sql_func_len) {
		RETURN_FALSE;
	}

	if (!zend_is_callable(step_callback, 0, &callback_name TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Not a valid callback function %R", Z_TYPE(callback_name), Z_UNIVAL(callback_name));
		zval_dtor(&callback_name);
		RETURN_FALSE;
	}
	zval_dtor(&callback_name);

	if (!zend_is_callable(fini_callback, 0, &callback_name TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Not a valid callback function %R", Z_TYPE(callback_name), Z_UNIVAL(callback_name));
		zval_dtor(&callback_name);
		RETURN_FALSE;
	}
	zval_dtor(&callback_name);

	func = (php_sqlite3_func *)ecalloc(1, sizeof(*func));

	/* Todo: utf-8 or utf-16 = sqlite3_create_function16 */
	if (sqlite3_create_function(db_obj->db, sql_func, sql_func_num_args, SQLITE_UTF8, func, NULL, php_sqlite3_callback_step, php_sqlite3_callback_final) == SQLITE_OK) {
		func->func_name = estrdup(sql_func);

		MAKE_STD_ZVAL(func->step);
		*(func->step) = *step_callback;
		zval_copy_ctor(func->step);

		MAKE_STD_ZVAL(func->fini);
		*(func->fini) = *fini_callback;
		zval_copy_ctor(func->fini);

		func->argc = sql_func_num_args;
		func->next = db_obj->funcs;
		db_obj->funcs = func;

		RETURN_TRUE;
	}
	efree(func);

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int SQLite3Stmt::paramCount() U
   Returns the number of parameters within the prepared statement. */
PHP_METHOD(sqlite3stmt, paramCount)
{
	php_sqlite3_stmt *stmt_obj;
	zval *object = getThis();
	stmt_obj = (php_sqlite3_stmt *)zend_object_store_get_object(object TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(sqlite3_bind_parameter_count(stmt_obj->stmt));
}
/* }}} */

/* {{{ proto bool SQLite3Stmt::close() U
   Closes the prepared statement. */
PHP_METHOD(sqlite3stmt, close)
{
	php_sqlite3_stmt *stmt_obj;
	zval *object = getThis();
	stmt_obj = (php_sqlite3_stmt *)zend_object_store_get_object(object TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	zend_llist_del_element(&(stmt_obj->db_obj->free_list), object, (int (*)(void *, void *)) php_sqlite3_compare_stmt_zval_free);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool SQLite3Stmt::reset() U
   Reset the prepared statement to the state before it was executed, bindings still remain. */
PHP_METHOD(sqlite3stmt, reset)
{
	php_sqlite3_stmt *stmt_obj;
	zval *object = getThis();
	stmt_obj = (php_sqlite3_stmt *)zend_object_store_get_object(object TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (sqlite3_reset(stmt_obj->stmt) != SQLITE_OK) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to reset statement: %s", sqlite3_errmsg(sqlite3_db_handle(stmt_obj->stmt)));
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool SQLite3Stmt::clear() U
   Clear all current bound parameters. */
PHP_METHOD(sqlite3stmt, clear)
{
	php_sqlite3_stmt *stmt_obj;
	zval *object = getThis();
	stmt_obj = (php_sqlite3_stmt *)zend_object_store_get_object(object TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (sqlite3_clear_bindings(stmt_obj->stmt) != SQLITE_OK) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to execute statement: %s", sqlite3_errmsg(sqlite3_db_handle(stmt_obj->stmt)));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

static int register_bound_parameter_to_sqlite(struct php_sqlite3_bound_param *param, php_sqlite3_stmt *stmt TSRMLS_DC) /* {{{ */
{
	HashTable *hash;
	hash = stmt->bound_params;

	if (!hash) {
		ALLOC_HASHTABLE(hash);
		zend_hash_init(hash, 13, NULL, sqlite3_param_dtor, 0);
		stmt->bound_params = hash;
	}

	/* We need a : prefix to resolve a name to a parameter number */
	if (param->name) {
		if (param->name[0] != ':') {
			/* pre-increment for character + 1 for null */
			char *temp = emalloc(++param->name_len + 1);
			temp[0] = ':';
			memmove(temp+1, param->name, param->name_len);
			param->name = temp;
		} else {
			param->name = estrndup(param->name, param->name_len);
		}
		/* do lookup*/
		param->param_number = sqlite3_bind_parameter_index(stmt->stmt, param->name);
	}

	if (param->param_number < 1) {
		efree(param->name);
		return 0;
	}

	if (param->param_number >= 1) {
		zend_hash_index_del(hash, param->param_number);
	}

	if (param->name) {
		zend_hash_update(hash, param->name, param->name_len, param, sizeof(*param), NULL);
	} else {
		zend_hash_index_update(hash, param->param_number, param, sizeof(*param), NULL);
	}

	return 1;
}
/* }}} */

/* {{{ proto bool SQLite3Stmt::bindParam(int parameter_number, mixed parameter [, int type])
   Bind Paramater to a stmt variable. */
PHP_METHOD(sqlite3stmt, bindParam)
{
	php_sqlite3_stmt *stmt_obj;
	zval *object = getThis();
	struct php_sqlite3_bound_param param = {0};
	stmt_obj = (php_sqlite3_stmt *)zend_object_store_get_object(object TSRMLS_CC);

	param.param_number = -1;
	param.type = SQLITE3_TEXT;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "lz|l", &param.param_number, &param.parameter, &param.type) == FAILURE) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz|l", &param.name, &param.name_len, &param.parameter, &param.type) == FAILURE) {
			return;
		}
	}

	Z_ADDREF_P(param.parameter);

	if (!register_bound_parameter_to_sqlite(&param, stmt_obj TSRMLS_CC)) {
		if (param.parameter) {
			zval_ptr_dtor(&(param.parameter));
			param.parameter = NULL;
		}
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool SQLite3Stmt::bindValue(inte parameter_number, mixed parameter [, int type])
   Bind Value of a parameter to a stmt variable. */
PHP_METHOD(sqlite3stmt, bindValue)
{
	php_sqlite3_stmt *stmt_obj;
	zval *object = getThis();
	struct php_sqlite3_bound_param param = {0};
	stmt_obj = (php_sqlite3_stmt *)zend_object_store_get_object(object TSRMLS_CC);

	param.param_number = -1;
	param.type = SQLITE3_TEXT;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "lz/|l", &param.param_number, &param.parameter, &param.type) == FAILURE) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz/|l", &param.name, &param.name_len, &param.parameter, &param.type) == FAILURE) {
			return;
		}
	}

	Z_ADDREF_P(param.parameter);

	if (!register_bound_parameter_to_sqlite(&param, stmt_obj TSRMLS_CC)) {
		if (param.parameter) {
			zval_ptr_dtor(&(param.parameter));
			param.parameter = NULL;
		}
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto SQLite3Result SQLite3Stmt::execute()
   Executes a prepared statement and returns a result set object. */
PHP_METHOD(sqlite3stmt, execute)
{
	php_sqlite3_stmt *stmt_obj;
	php_sqlite3_result *result;
	zval *object = getThis();
	int return_code = 0;
	struct php_sqlite3_bound_param *param;
	stmt_obj = (php_sqlite3_stmt *)zend_object_store_get_object(object TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (stmt_obj->bound_params) {
		zend_hash_internal_pointer_reset(stmt_obj->bound_params);
		while (zend_hash_get_current_data(stmt_obj->bound_params, (void **)&param) == SUCCESS) {
			/* If the ZVAL is null then it should be bound as that */
			if (Z_TYPE_P(param->parameter) == IS_NULL) {
				sqlite3_bind_null(stmt_obj->stmt, param->param_number);
				zend_hash_move_forward(stmt_obj->bound_params);
				continue;
			}

			switch (param->type) {
				case SQLITE_INTEGER:
					convert_to_long(param->parameter);
					sqlite3_bind_int(stmt_obj->stmt, param->param_number, Z_LVAL_P(param->parameter));
					break;

				case SQLITE_FLOAT:
					/* convert_to_double(param->parameter);*/
					sqlite3_bind_double(stmt_obj->stmt, param->param_number, Z_DVAL_P(param->parameter));
					break;

				case SQLITE_BLOB:
				{
					php_stream *stream = NULL;
					int blength;
					char *buffer = NULL;
					if (Z_TYPE_P(param->parameter) == IS_RESOURCE) {
						php_stream_from_zval_no_verify(stream, &param->parameter);
						if (stream == NULL) {
							php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to read stream for parameter %ld", param->param_number);
							RETURN_FALSE;
						}
						blength = php_stream_copy_to_mem(stream, (void *)&buffer, PHP_STREAM_COPY_ALL, 0);
					} else {
						convert_to_string(param->parameter);
						blength =  Z_STRLEN_P(param->parameter);
						buffer = Z_STRVAL_P(param->parameter);
					}

					sqlite3_bind_blob(stmt_obj->stmt, param->param_number, buffer, blength, SQLITE_TRANSIENT);

					if (stream) {
						pefree(buffer, 0);
					}
					break;
				}

				case SQLITE3_TEXT:
					convert_to_string(param->parameter);
					/* Todo: utf-8 or utf-16 = sqlite3_bind_text16 */
					sqlite3_bind_text(stmt_obj->stmt, param->param_number, Z_STRVAL_P(param->parameter), Z_STRLEN_P(param->parameter), SQLITE_STATIC);
					break;

				case SQLITE_NULL:
					sqlite3_bind_null(stmt_obj->stmt, param->param_number);
					break;

				default:
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown parameter type: %ld for parameter %ld", param->type, param->param_number);
					RETURN_FALSE;
			}
			zend_hash_move_forward(stmt_obj->bound_params);
		}
	}

	return_code = sqlite3_step(stmt_obj->stmt);

	switch (return_code) {
		case SQLITE_ROW: /* Valid Row */
		case SQLITE_DONE: /* Valid but no results */
		{
			php_sqlite3_free_list *free_item;

			sqlite3_reset(stmt_obj->stmt);
			object_init_ex(return_value, php_sqlite3_result_entry);
			result = (php_sqlite3_result *)zend_object_store_get_object(return_value TSRMLS_CC);

			Z_ADDREF_P(object);
	
			result->is_prepared_statement = 1;
			result->db_obj = stmt_obj->db_obj;
			result->stmt_obj = stmt_obj;
			result->stmt_obj_zval = getThis();

			free_item = emalloc(sizeof(php_sqlite3_free_list));
			free_item->stmt_obj = stmt_obj;
			free_item->stmt_obj_zval = getThis();

			zend_llist_add_element(&(stmt_obj->db_obj->free_list), &free_item);

			break;
		}
		case SQLITE_ERROR:
			sqlite3_reset(stmt_obj->stmt);

		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to execute statement: %s", sqlite3_errmsg(sqlite3_db_handle(stmt_obj->stmt)));
			zval_dtor(return_value);
			RETURN_FALSE;
	}

	return;
}
/* }}} */

/* {{{ proto int SQLite3Result::numColumns() U
   Number of columns in the result set. */
PHP_METHOD(sqlite3result, numColumns)
{
	php_sqlite3_result *result_obj;
	zval *object = getThis();
	result_obj = (php_sqlite3_result *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(result_obj->stmt_obj->initialised, SQLite3Result)

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(sqlite3_column_count(result_obj->stmt_obj->stmt));
}
/* }}} */

/* {{{ proto string SQLite3Result::columnName(int column) U
   Returns the name of the nth column. */
PHP_METHOD(sqlite3result, columnName)
{
	php_sqlite3_result *result_obj;
	zval *object = getThis();
	long column = 0;
	result_obj = (php_sqlite3_result *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(result_obj->stmt_obj->initialised, SQLite3Result)

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &column) == FAILURE) {
		return;
	}

	RETVAL_UNICODE((UChar*)sqlite3_column_name16(result_obj->stmt_obj->stmt, column), 1);
}
/* }}} */

/* {{{ proto int SQLite3Result::columnType(int column) U
   Returns the type of the nth column. */
PHP_METHOD(sqlite3result, columnType)
{
	php_sqlite3_result *result_obj;
	zval *object = getThis();
	long column = 0;
	result_obj = (php_sqlite3_result *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(result_obj->stmt_obj->initialised, SQLite3Result)

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &column) == FAILURE) {
		return;
	}

	RETURN_LONG(sqlite3_column_type(result_obj->stmt_obj->stmt, column));
}
/* }}} */

/* {{{ proto array SQLite3Result::fetchArray([int mode]) U
   Fetch a result row as both an associative or numerically indexed array or both. */
PHP_METHOD(sqlite3result, fetchArray)
{
	php_sqlite3_result *result_obj;
	zval *object = getThis();
	int i, ret;
	long mode = PHP_SQLITE3_BOTH;
	result_obj = (php_sqlite3_result *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(result_obj->stmt_obj->initialised, SQLite3Result)

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &mode) == FAILURE) {
		return;
	}

	ret = sqlite3_step(result_obj->stmt_obj->stmt);
	switch (ret) {
		case SQLITE_ROW:
			/* If there was no return value then just skip fetching */
			if (!return_value_used) {
				return;
			}

			array_init(return_value);

			for (i = 0; i < sqlite3_data_count(result_obj->stmt_obj->stmt); i++) {
				zval *data;

				data = sqlite_value_to_zval(result_obj->stmt_obj->stmt, i);

				if (mode & PHP_SQLITE3_NUM) {
					add_index_zval(return_value, i, data);
				}

				if (mode & PHP_SQLITE3_ASSOC) {
					UChar *ustr = (UChar *)sqlite3_column_name16(result_obj->stmt_obj->stmt, i);
					if (mode & PHP_SQLITE3_NUM) {
						Z_ADDREF_P(data);
					}
					add_u_assoc_zval(return_value, IS_UNICODE, ZSTR(ustr), data);
				}
			}
			break;

		case SQLITE_DONE:
			RETURN_FALSE;
			break;

		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to execute statement: %s", sqlite3_errmsg(sqlite3_db_handle(result_obj->stmt_obj->stmt)));
	}
}
/* }}} */

/* {{{ proto bool SQLite3Result::reset() U
   Resets the result set back to the first row. */
PHP_METHOD(sqlite3result, reset)
{
	php_sqlite3_result *result_obj;
	zval *object = getThis();
	result_obj = (php_sqlite3_result *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(result_obj->stmt_obj->initialised, SQLite3Result)

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (sqlite3_reset(result_obj->stmt_obj->stmt) != SQLITE_OK) {
		RETURN_FALSE;
	}

	result_obj->complete = 0;

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool SQLite3Result::finalize() U
   Closes the result set. */
PHP_METHOD(sqlite3result, finalize)
{
	php_sqlite3_result *result_obj;
	zval *object = getThis();
	result_obj = (php_sqlite3_result *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(result_obj->stmt_obj->initialised, SQLite3Result)

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	/* We need to finalize an internal statement */
	if (result_obj->is_prepared_statement == 0) {
		zend_llist_del_element(&(result_obj->db_obj->free_list), result_obj->stmt_obj_zval,
			(int (*)(void *, void *)) php_sqlite3_compare_stmt_zval_free);
	} else {
		sqlite3_reset(result_obj->stmt_obj->stmt);
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ arginfo */
static
ZEND_BEGIN_ARG_INFO(arginfo_sqlite3_open, 0)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(0, encryption_key)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_sqlite3_close, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_sqlite3_exec, 0)
	ZEND_ARG_INFO(0, query)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_sqlite3_version, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_sqlite3_lastinsertrowid, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_sqlite3_lasterrorcode, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_sqlite3_lasterrormsg, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_sqlite3_loadextension, 0)
	ZEND_ARG_INFO(0, shared_library)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_sqlite3_changes, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3_escapestring, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3_prepare, 0, 0, 1)
	ZEND_ARG_INFO(0, query)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3_query, 0, 0, 1)
	ZEND_ARG_INFO(0, query)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3_querysingle, 0, 0, 1)
	ZEND_ARG_INFO(0, query)
	ZEND_ARG_INFO(0, entire_row)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3_createfunction, 0, 0, 2)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, callback)
	ZEND_ARG_INFO(0, argument_count)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3_createaggregate, 0, 0, 3)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, step_callback)
	ZEND_ARG_INFO(0, final_callback)
	ZEND_ARG_INFO(0, argument_count)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_sqlite3stmt_paramcount, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_sqlite3stmt_close, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_sqlite3stmt_reset, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_sqlite3stmt_clear, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_sqlite3stmt_execute, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3stmt_bindparam, 0, 0, 2)
	ZEND_ARG_INFO(0, param_number)
	ZEND_ARG_INFO(1, param)
	ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3stmt_bindvalue, 0, 0, 2)
	ZEND_ARG_INFO(0, param_number)
	ZEND_ARG_INFO(0, param)
	ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_sqlite3result_numcolumns, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3result_columnname, 0, 0, 1)
	ZEND_ARG_INFO(0, column_number)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3result_columntype, 0, 0, 1)
	ZEND_ARG_INFO(0, column_number)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3result_fetcharray, 0, 0, 1)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_sqlite3result_reset, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_sqlite3result_finalize, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ php_sqlite3_class_methods */
static zend_function_entry php_sqlite3_class_methods[] = {
	PHP_ME(sqlite3,		open,				arginfo_sqlite3_open, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		close,				arginfo_sqlite3_close, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		exec,				arginfo_sqlite3_exec, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		version,			arginfo_sqlite3_version, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(sqlite3,		lastInsertRowID,	arginfo_sqlite3_lastinsertrowid, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		lastErrorCode,		arginfo_sqlite3_lasterrorcode, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		lastErrorMsg,		arginfo_sqlite3_lasterrormsg, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		loadExtension,		arginfo_sqlite3_loadextension, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		changes,			arginfo_sqlite3_changes, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		escapeString,		arginfo_sqlite3_escapestring, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(sqlite3,		prepare,			arginfo_sqlite3_prepare, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		query,				arginfo_sqlite3_query, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		querySingle,		arginfo_sqlite3_querysingle, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		createFunction,		arginfo_sqlite3_createfunction, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		createAggregate,	arginfo_sqlite3_createaggregate, ZEND_ACC_PUBLIC)
	/* Aliases */
	PHP_MALIAS(sqlite3,	__construct, open, arginfo_sqlite3_open, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ php_sqlite3_stmt_class_methods */
static zend_function_entry php_sqlite3_stmt_class_methods[] = {
	PHP_ME(sqlite3stmt, paramCount,		arginfo_sqlite3stmt_paramcount, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3stmt, close,			arginfo_sqlite3stmt_close, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3stmt, reset,			arginfo_sqlite3stmt_reset, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3stmt, clear,			arginfo_sqlite3stmt_clear, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3stmt, execute,		arginfo_sqlite3stmt_execute, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3stmt, bindParam,		arginfo_sqlite3stmt_bindparam, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3stmt, bindValue,		arginfo_sqlite3stmt_bindvalue, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ php_sqlite3_result_class_methods */
static zend_function_entry php_sqlite3_result_class_methods[] = {
	PHP_ME(sqlite3result, numColumns,		arginfo_sqlite3result_numcolumns, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3result, columnName,		arginfo_sqlite3result_columnname, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3result, columnType,		arginfo_sqlite3result_columntype, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3result, fetchArray,		arginfo_sqlite3result_fetcharray, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3result, reset,			arginfo_sqlite3result_reset, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3result, finalize,			arginfo_sqlite3result_finalize, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ Authorization Callback 
*/
static int php_sqlite3_authorizer(void *autharg, int access_type, const char *arg3, const char *arg4, const char *arg5, const char *arg6)
{
	TSRMLS_FETCH();
	switch (access_type) {
		case SQLITE_ATTACH:
		{
			if (strncmp(arg3, ":memory:", sizeof(":memory:")-1)) {
				if (php_check_open_basedir(arg3 TSRMLS_CC)) {
					return SQLITE_DENY;
				}
			}
			return SQLITE_OK;
		}

		default:
			/* access allowed */
			return SQLITE_OK;
	}
}
/* }}} */

/* {{{ php_sqlite3_free_list_dtor
*/
static void php_sqlite3_free_list_dtor(void **item)
{
	php_sqlite3_free_list *free_item = (php_sqlite3_free_list *)*item;

	if (free_item->stmt_obj && free_item->stmt_obj->initialised) {
		sqlite3_finalize(free_item->stmt_obj->stmt);
		free_item->stmt_obj->initialised = 0;
	}
	efree(*item);
}
/* }}} */

static int php_sqlite3_compare_stmt_zval_free( php_sqlite3_free_list **free_list, zval *statement ) /* {{{ */
{
	return ((*free_list)->stmt_obj->initialised && statement == (*free_list)->stmt_obj_zval);
}
/* }}} */

static int php_sqlite3_compare_stmt_free( php_sqlite3_free_list **free_list, sqlite3_stmt *statement ) /* {{{ */
{
	return ((*free_list)->stmt_obj->initialised && statement == (*free_list)->stmt_obj->stmt);
}
/* }}} */

static void php_sqlite3_object_free_storage(void *object TSRMLS_DC) /* {{{ */
{
	php_sqlite3_db_object *intern = (php_sqlite3_db_object *)object;
	php_sqlite3_func *func;

	if (!intern) {
		return;
	}

	while (intern->funcs) {
		func = intern->funcs;
		intern->funcs = func->next;
		if (intern->initialised && intern->db) {
			sqlite3_create_function(intern->db, func->func_name, func->argc, SQLITE_UTF8, func, NULL, NULL, NULL);
		}

		efree((char*)func->func_name);

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

	if (intern->initialised && intern->db) {
		sqlite3_close(intern->db);
		intern->initialised = 0;
	}

	zend_object_std_dtor(&intern->zo TSRMLS_CC);
	efree(intern);
}
/* }}} */

static void php_sqlite3_stmt_object_free_storage(void *object TSRMLS_DC) /* {{{ */
{
	php_sqlite3_stmt *intern = (php_sqlite3_stmt *)object;

	if (!intern) {
		return;
	}

	if (intern->bound_params) {
		zend_hash_destroy(intern->bound_params);
		FREE_HASHTABLE(intern->bound_params);
		intern->bound_params = NULL;
	}

	if (intern->initialised) {
		zend_llist_del_element(&(intern->db_obj->free_list), intern->stmt,
			(int (*)(void *, void *)) php_sqlite3_compare_stmt_free);
	}

	if (intern->db_obj_zval) {
		Z_DELREF_P(intern->db_obj_zval);
	}

	zend_object_std_dtor(&intern->zo TSRMLS_CC);
	efree(intern);
}
/* }}} */

static void php_sqlite3_result_object_free_storage(void *object TSRMLS_DC) /* {{{ */
{
	php_sqlite3_result *intern = (php_sqlite3_result *)object;

	if (!intern) {
		return;
	}
	if (intern->stmt_obj_zval) {
		if (intern->stmt_obj->initialised) {
			sqlite3_reset(intern->stmt_obj->stmt);
		}
		
		if (intern->is_prepared_statement == 0) {
			zval_dtor(intern->stmt_obj_zval);
			FREE_ZVAL(intern->stmt_obj_zval);
		} else {
			zval_ptr_dtor(&intern->stmt_obj_zval);
		}
	}

	zend_object_std_dtor(&intern->zo TSRMLS_CC);
	efree(intern);
}
/* }}} */

static zend_object_value php_sqlite3_object_new(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
	zval *tmp;
	zend_object_value retval;
	php_sqlite3_db_object *intern;

	/* Allocate memory for it */
	intern = emalloc(sizeof(php_sqlite3_db_object));
	memset(&intern->zo, 0, sizeof(php_sqlite3_db_object));

	/* Need to keep track of things to free */
	zend_llist_init(&(intern->free_list),   sizeof(php_sqlite3_free_list *), (llist_dtor_func_t)php_sqlite3_free_list_dtor, 0);

	zend_object_std_init(&intern->zo, class_type TSRMLS_CC);
	zend_hash_copy(intern->zo.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref,(void *) &tmp, sizeof(zval *));

	retval.handle = zend_objects_store_put(intern, NULL, (zend_objects_free_object_storage_t) php_sqlite3_object_free_storage, NULL TSRMLS_CC);
	retval.handlers = (zend_object_handlers *) &sqlite3_object_handlers;

	return retval;
}
/* }}} */

static zend_object_value php_sqlite3_stmt_object_new(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
	zval *tmp;
	zend_object_value retval;
	php_sqlite3_stmt *intern;

	/* Allocate memory for it */
	intern = emalloc(sizeof(php_sqlite3_stmt));
	memset(&intern->zo, 0, sizeof(php_sqlite3_stmt));

	intern->db_obj_zval = NULL;

	zend_object_std_init(&intern->zo, class_type TSRMLS_CC);
	zend_hash_copy(intern->zo.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref,(void *) &tmp, sizeof(zval *));

	retval.handle = zend_objects_store_put(intern, NULL, (zend_objects_free_object_storage_t) php_sqlite3_stmt_object_free_storage, NULL TSRMLS_CC);
	retval.handlers = (zend_object_handlers *) &sqlite3_stmt_object_handlers;

	return retval;
}
/* }}} */

static zend_object_value php_sqlite3_result_object_new(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
	zval *tmp;
	zend_object_value retval;
	php_sqlite3_result *intern;

	/* Allocate memory for it */
	intern = emalloc(sizeof(php_sqlite3_result));
	memset(&intern->zo, 0, sizeof(php_sqlite3_result));

	intern->complete = 0;
	intern->is_prepared_statement = 0;
	intern->stmt_obj_zval = NULL;

	zend_object_std_init(&intern->zo, class_type TSRMLS_CC);
	zend_hash_copy(intern->zo.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref,(void *) &tmp, sizeof(zval *));

	retval.handle = zend_objects_store_put(intern, NULL, (zend_objects_free_object_storage_t) php_sqlite3_result_object_free_storage, NULL TSRMLS_CC);
	retval.handlers = (zend_object_handlers *) &sqlite3_result_object_handlers;

	return retval;
}
/* }}} */

static void sqlite3_param_dtor(void *data) /* {{{ */
{
	struct php_sqlite3_bound_param *param = (struct php_sqlite3_bound_param*)data;

	if (param->name) {
		efree(param->name);
	}

	if (param->parameter) {
		zval_ptr_dtor(&(param->parameter));
		param->parameter = NULL;
	}
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
*/
PHP_MINIT_FUNCTION(sqlite3)
{
	zend_class_entry ce;

#if defined(ZTS)
	/* Refuse to load if this wasn't a threasafe library loaded */
	if (!sqlite3_threadsafe()) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "A thread safe version of SQLite is required when using a thread safe version of PHP.");
		return FAILURE;
	}
#endif

	memcpy(&sqlite3_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	memcpy(&sqlite3_stmt_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	memcpy(&sqlite3_result_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

	/* Register SQLite 3 Class */
	INIT_CLASS_ENTRY(ce, "SQLite3", php_sqlite3_class_methods);
	ce.create_object = php_sqlite3_object_new;
	sqlite3_object_handlers.clone_obj = NULL;
	php_sqlite3_sc_entry = zend_register_internal_class(&ce TSRMLS_CC);

	/* Register SQLite 3 Prepared Statement Class */
	INIT_CLASS_ENTRY(ce, "SQLite3Stmt", php_sqlite3_stmt_class_methods);
	ce.create_object = php_sqlite3_stmt_object_new;
	sqlite3_stmt_object_handlers.clone_obj = NULL;
	php_sqlite3_stmt_entry = zend_register_internal_class(&ce TSRMLS_CC);

	/* Register SQLite 3 Result Class */
	INIT_CLASS_ENTRY(ce, "SQLite3Result", php_sqlite3_result_class_methods);
	ce.create_object = php_sqlite3_result_object_new;
	sqlite3_result_object_handlers.clone_obj = NULL;
	php_sqlite3_result_entry = zend_register_internal_class(&ce TSRMLS_CC);

	REGISTER_INI_ENTRIES();

	REGISTER_LONG_CONSTANT("SQLITE3_ASSOC", PHP_SQLITE3_ASSOC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE3_NUM", PHP_SQLITE3_NUM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE3_BOTH", PHP_SQLITE3_BOTH, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("SQLITE3_INTEGER", SQLITE_INTEGER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE3_FLOAT", SQLITE_FLOAT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE3_TEXT", SQLITE3_TEXT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE3_BLOB", SQLITE_BLOB, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE3_NULL", SQLITE_NULL, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("SQLITE3_OPEN_READONLY", SQLITE_OPEN_READONLY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE3_OPEN_READWRITE", SQLITE_OPEN_READWRITE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE3_OPEN_CREATE", SQLITE_OPEN_CREATE, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
*/
PHP_MSHUTDOWN_FUNCTION(sqlite3)
{
	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
*/
PHP_MINFO_FUNCTION(sqlite3)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "SQLite3 support", "enabled");
	php_info_print_table_row(2, "SQLite3 module version", PHP_SQLITE3_VERSION);
	php_info_print_table_row(2, "SQLite Library", sqlite3_libversion());
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ PHP_GINIT_FUNCTION
*/
static PHP_GINIT_FUNCTION(sqlite3)
{
	memset(sqlite3_globals, 0, sizeof(*sqlite3_globals));
}
/* }}} */

/* {{{ sqlite3_module_entry
*/
zend_module_entry sqlite3_module_entry = {
	STANDARD_MODULE_HEADER,
	"sqlite3",
	NULL,
	PHP_MINIT(sqlite3),
	PHP_MSHUTDOWN(sqlite3),
	NULL,
	NULL,
	PHP_MINFO(sqlite3),
	PHP_SQLITE3_VERSION,
	PHP_MODULE_GLOBALS(sqlite3),
	PHP_GINIT(sqlite3),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_SQLITE3
ZEND_GET_MODULE(sqlite3)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
