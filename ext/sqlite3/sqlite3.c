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
#include "php_sqlite3.h"
#include "php_sqlite3_structs.h"
#include "main/SAPI.h"

#include <sqlite3.h>

#include "zend_exceptions.h"
#include "zend_interfaces.h"
#include "SAPI.h"

ZEND_DECLARE_MODULE_GLOBALS(sqlite3)

static PHP_GINIT_FUNCTION(sqlite3);
static int php_sqlite3_authorizer(void *autharg, int access_type, const char *arg3, const char *arg4, const char *arg5, const char *arg6);
static void sqlite3_param_dtor(void *data);
static int php_sqlite3_compare_stmt_zval_free(php_sqlite3_free_list **free_list, zval *statement);

/* {{{ Error Handler
*/
static void php_sqlite3_error(php_sqlite3_db_object *db_obj, char *format, ...)
{
	va_list arg;
	char 	*message;
	TSRMLS_FETCH();

	va_start(arg, format); 
	vspprintf(&message, 0, format, arg);
	va_end(arg);

	if (db_obj->exception) {
		zend_throw_exception(zend_exception_get_default(TSRMLS_C), message, 0 TSRMLS_CC);
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", message);
	}
	
	if (message) {
		efree(message);
	}
}
/* }}} */

#define SQLITE3_CHECK_INITIALIZED(db_obj, member, class_name) \
	if (!(member)) { \
		php_sqlite3_error(db_obj, "The " #class_name " object has not been correctly initialised"); \
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

/* {{{ proto void SQLite3::open(String filename [, int Flags [, string Encryption Key]])
   Opens a SQLite 3 Database, if the build includes encryption then it will attempt to use the key. */
PHP_METHOD(sqlite3, open)
{
	php_sqlite3_db_object *db_obj;
	zval *object = getThis();
	char *filename, *encryption_key, *fullpath;
	int filename_len, encryption_key_len = 0;
	long flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
	zend_error_handling error_handling;

	db_obj = (php_sqlite3_db_object *)zend_object_store_get_object(object TSRMLS_CC);
	zend_replace_error_handling(EH_THROW, NULL, &error_handling TSRMLS_CC);

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p|ls", &filename, &filename_len, &flags, &encryption_key, &encryption_key_len)) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}

	zend_restore_error_handling(&error_handling TSRMLS_CC);

	if (db_obj->initialised) {
		zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Already initialised DB Object", 0 TSRMLS_CC);
	}

	if (strlen(filename) != filename_len) {
		return;
	}
	if (memcmp(filename, ":memory:", sizeof(":memory:")) != 0) {
		if (!(fullpath = expand_filepath(filename, NULL TSRMLS_CC))) {
			zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Unable to expand filepath", 0 TSRMLS_CC);
			return;
		}

#if PHP_API_VERSION < 20100412
		if (PG(safe_mode) && (!php_checkuid(fullpath, NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
			zend_throw_exception_ex(zend_exception_get_default(TSRMLS_C), 0 TSRMLS_CC, "safe_mode prohibits opening %s", fullpath);
			efree(fullpath);
			return;
		}
#endif

		if (php_check_open_basedir(fullpath TSRMLS_CC)) {
			zend_throw_exception_ex(zend_exception_get_default(TSRMLS_C), 0 TSRMLS_CC, "open_basedir prohibits opening %s", fullpath);
			efree(fullpath);
			return;
		}
	} else {
		fullpath = estrdup(filename);
	}

#if SQLITE_VERSION_NUMBER >= 3005000
	if (sqlite3_open_v2(fullpath, &(db_obj->db), flags, NULL) != SQLITE_OK) {
#else
	if (sqlite3_open(fullpath, &(db_obj->db)) != SQLITE_OK) {
#endif
		zend_throw_exception_ex(zend_exception_get_default(TSRMLS_C), 0 TSRMLS_CC, "Unable to open database: %s", sqlite3_errmsg(db_obj->db));
		if (fullpath) {
			efree(fullpath);
		}
		return;
	}

#if SQLITE_HAS_CODEC
	if (encryption_key_len > 0) {
		if (sqlite3_key(db_obj->db, encryption_key, encryption_key_len) != SQLITE_OK) {
			zend_throw_exception_ex(zend_exception_get_default(TSRMLS_C), 0 TSRMLS_CC, "Unable to open database: %s", sqlite3_errmsg(db_obj->db));
			return;
		}
	}
#endif

	db_obj->initialised = 1;

#if PHP_API_VERSION < 20100412
	if (PG(safe_mode) || (PG(open_basedir) && *PG(open_basedir))) {
#else
	if (PG(open_basedir) && *PG(open_basedir)) {
#endif
		sqlite3_set_authorizer(db_obj->db, php_sqlite3_authorizer, NULL);
	}

	if (fullpath) {
		efree(fullpath);
	}
}
/* }}} */

/* {{{ proto bool SQLite3::close()
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
			php_sqlite3_error(db_obj, "Unable to close database: %d, %s", errcode, sqlite3_errmsg(db_obj->db));
			RETURN_FALSE;
		}
		db_obj->initialised = 0;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool SQLite3::exec(String Query)
   Executes a result-less query against a given database. */
PHP_METHOD(sqlite3, exec)
{
	php_sqlite3_db_object *db_obj;
	zval *object = getThis();
	char *sql, *errtext = NULL;
	int sql_len;
	db_obj = (php_sqlite3_db_object *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->initialised, SQLite3)

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &sql, &sql_len)) {
		return;
	}

	if (sqlite3_exec(db_obj->db, sql, NULL, NULL, &errtext) != SQLITE_OK) {
		php_sqlite3_error(db_obj, "%s", errtext);
		sqlite3_free(errtext);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto Array SQLite3::version()
   Returns the SQLite3 Library version as a string constant and as a number. */
PHP_METHOD(sqlite3, version)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	array_init(return_value);

	add_assoc_string(return_value, "versionString", (char*)sqlite3_libversion(), 1);
	add_assoc_long(return_value, "versionNumber", sqlite3_libversion_number());

	return;
}
/* }}} */

/* {{{ proto int SQLite3::lastInsertRowID()
   Returns the rowid of the most recent INSERT into the database from the database connection. */
PHP_METHOD(sqlite3, lastInsertRowID)
{
	php_sqlite3_db_object *db_obj;
	zval *object = getThis();
	db_obj = (php_sqlite3_db_object *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->initialised, SQLite3)

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(sqlite3_last_insert_rowid(db_obj->db));
}
/* }}} */

/* {{{ proto int SQLite3::lastErrorCode()
   Returns the numeric result code of the most recent failed sqlite API call for the database connection. */
PHP_METHOD(sqlite3, lastErrorCode)
{
	php_sqlite3_db_object *db_obj;
	zval *object = getThis();
	db_obj = (php_sqlite3_db_object *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->db, SQLite3)

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(sqlite3_errcode(db_obj->db));
}
/* }}} */

/* {{{ proto string SQLite3::lastErrorMsg()
   Returns english text describing the most recent failed sqlite API call for the database connection. */
PHP_METHOD(sqlite3, lastErrorMsg)
{
	php_sqlite3_db_object *db_obj;
	zval *object = getThis();
	db_obj = (php_sqlite3_db_object *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->db, SQLite3)

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETVAL_STRING((char *)sqlite3_errmsg(db_obj->db), 1);
}
/* }}} */

/* {{{ proto bool SQLite3::busyTimeout(int msecs)
   Sets a busy handler that will sleep until database is not locked or timeout is reached. Passing a value less than or equal to zero turns off all busy handlers. */
PHP_METHOD(sqlite3, busyTimeout)
{
	php_sqlite3_db_object *db_obj;
	zval *object = getThis();
	long ms;
	int return_code;
	db_obj = (php_sqlite3_db_object *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->initialised, SQLite3)

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &ms)) {
		return;
	}

	return_code = sqlite3_busy_timeout(db_obj->db, ms);
	if (return_code != SQLITE_OK) {
		php_sqlite3_error(db_obj, "Unable to set busy timeout: %d, %s", return_code, sqlite3_errmsg(db_obj->db));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */


#ifndef SQLITE_OMIT_LOAD_EXTENSION
/* {{{ proto bool SQLite3::loadExtension(String Shared Library)
   Attempts to load an SQLite extension library. */
PHP_METHOD(sqlite3, loadExtension)
{
	php_sqlite3_db_object *db_obj;
	zval *object = getThis();
	char *extension, *lib_path, *extension_dir, *errtext = NULL;
	char fullpath[MAXPATHLEN];
	int extension_len, extension_dir_len;
	db_obj = (php_sqlite3_db_object *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->initialised, SQLite3)

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &extension, &extension_len)) {
		return;
	}

#ifdef ZTS
	if ((strncmp(sapi_module.name, "cgi", 3) != 0) &&
		(strcmp(sapi_module.name, "cli") != 0) &&
		(strncmp(sapi_module.name, "embed", 5) != 0)
	) {		php_sqlite3_error(db_obj, "Not supported in multithreaded Web servers");
		RETURN_FALSE;
	}
#endif

	if (!SQLITE3G(extension_dir)) {
		php_sqlite3_error(db_obj, "SQLite Extension are disabled");
		RETURN_FALSE;
	}

	if (extension_len == 0) {
		php_sqlite3_error(db_obj, "Empty string as an extension");
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
		php_sqlite3_error(db_obj, "Unable to load extension at '%s'", lib_path);
		efree(lib_path);
		RETURN_FALSE;
	}

	efree(lib_path);

	if (strncmp(fullpath, extension_dir, extension_dir_len) != 0) {
		php_sqlite3_error(db_obj, "Unable to open extensions outside the defined directory");
		RETURN_FALSE;
	}

	/* Extension loading should only be enabled for when we attempt to load */
	sqlite3_enable_load_extension(db_obj->db, 1);
	if (sqlite3_load_extension(db_obj->db, fullpath, 0, &errtext) != SQLITE_OK) {
		php_sqlite3_error(db_obj, "%s", errtext);
		sqlite3_free(errtext);
		sqlite3_enable_load_extension(db_obj->db, 0);
		RETURN_FALSE;
	}
	sqlite3_enable_load_extension(db_obj->db, 0);

	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ proto int SQLite3::changes()
  Returns the number of database rows that were changed (or inserted or deleted) by the most recent SQL statement. */
PHP_METHOD(sqlite3, changes)
{
	php_sqlite3_db_object *db_obj;
	zval *object = getThis();
	db_obj = (php_sqlite3_db_object *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->initialised, SQLite3)

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(sqlite3_changes(db_obj->db));
}
/* }}} */

/* {{{ proto String SQLite3::escapeString(String value)
   Returns a string that has been properly escaped. */
PHP_METHOD(sqlite3, escapeString)
{
	char *sql, *ret;
	int sql_len;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &sql, &sql_len)) {
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
	php_sqlite3_free_list *free_item;

	db_obj = (php_sqlite3_db_object *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->initialised, SQLite3)

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

	errcode = sqlite3_prepare_v2(db_obj->db, sql, sql_len, &(stmt_obj->stmt), NULL);
	if (errcode != SQLITE_OK) {
		php_sqlite3_error(db_obj, "Unable to prepare statement: %d, %s", errcode, sqlite3_errmsg(db_obj->db));
		zval_dtor(return_value);
		RETURN_FALSE;
	}

	stmt_obj->initialised = 1;

	free_item = emalloc(sizeof(php_sqlite3_free_list));
	free_item->stmt_obj = stmt_obj;
	free_item->stmt_obj_zval = return_value;

	zend_llist_add_element(&(db_obj->free_list), &free_item);
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

	SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->initialised, SQLite3)

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &sql, &sql_len)) {
		return;
	}

	if (!sql_len) {
		RETURN_FALSE;
	}

	/* If there was no return value then just execute the query */
	if (!return_value_used) {
		if (sqlite3_exec(db_obj->db, sql, NULL, NULL, &errtext) != SQLITE_OK) {
			php_sqlite3_error(db_obj, "%s", errtext);
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

	return_code = sqlite3_prepare_v2(db_obj->db, sql, sql_len, &(stmt_obj->stmt), NULL);
	if (return_code != SQLITE_OK) {
		php_sqlite3_error(db_obj, "Unable to prepare statement: %d, %s", return_code, sqlite3_errmsg(db_obj->db));
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
			php_sqlite3_error(db_obj, "Unable to execute statement: %s", sqlite3_errmsg(db_obj->db));
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
			ZVAL_STRING(data, (char*)sqlite3_column_text(stmt, column), 1);
			break;

		case SQLITE_BLOB:
		default:
			ZVAL_STRINGL(data, (char*)sqlite3_column_blob(stmt, column), sqlite3_column_bytes(stmt, column), 1);
	}
	return data;
}
/* }}} */

/* {{{ proto SQLite3Result SQLite3::querySingle(String Query [, bool entire_row = false])
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

	SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->initialised, SQLite3)

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|b", &sql, &sql_len, &entire_row)) {
		return;
	}

	if (!sql_len) {
		RETURN_FALSE;
	}

	/* If there was no return value then just execute the query */
	if (!return_value_used) {
		if (sqlite3_exec(db_obj->db, sql, NULL, NULL, &errtext) != SQLITE_OK) {
			php_sqlite3_error(db_obj, "%s", errtext);
			sqlite3_free(errtext);
		}
		return;
	}

	return_code = sqlite3_prepare_v2(db_obj->db, sql, sql_len, &stmt, NULL);
	if (return_code != SQLITE_OK) {
		php_sqlite3_error(db_obj, "Unable to prepare statement: %d, %s", return_code, sqlite3_errmsg(db_obj->db));
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
					data = sqlite_value_to_zval(stmt, i);
					add_assoc_zval(return_value, (char*)sqlite3_column_name(stmt, i), data);
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
			php_sqlite3_error(db_obj, "Unable to execute statement: %s", sqlite3_errmsg(db_obj->db));
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
	php_sqlite3_agg_context *agg_context = NULL;

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
		agg_context = (php_sqlite3_agg_context *)sqlite3_aggregate_context(context, sizeof(php_sqlite3_agg_context));

		if (!agg_context->zval_context) {
			MAKE_STD_ZVAL(agg_context->zval_context);
			ZVAL_NULL(agg_context->zval_context);
		}
		zargs[0] = &agg_context->zval_context;

		zargs[1] = emalloc(sizeof(zval*));
		MAKE_STD_ZVAL(*zargs[1]);
		ZVAL_LONG(*zargs[1], agg_context->row_count);
	}

	for (i = 0; i < argc; i++) {
		zargs[i + is_agg] = emalloc(sizeof(zval *));
		MAKE_STD_ZVAL(*zargs[i + is_agg]);

		switch (sqlite3_value_type(argv[i])) {
			case SQLITE_INTEGER:
#if LONG_MAX > 2147483647
				ZVAL_LONG(*zargs[i + is_agg], sqlite3_value_int64(argv[i]));
#else
				ZVAL_LONG(*zargs[i + is_agg], sqlite3_value_int(argv[i]));
#endif
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
#if LONG_MAX > 2147483647
					sqlite3_result_int64(context, Z_LVAL_P(retval));
#else
					sqlite3_result_int(context, Z_LVAL_P(retval));
#endif
					break;

				case IS_NULL:
					sqlite3_result_null(context);
					break;

				case IS_DOUBLE:
					sqlite3_result_double(context, Z_DVAL_P(retval));
					break;

				default:
					convert_to_string_ex(&retval);
					sqlite3_result_text(context, Z_STRVAL_P(retval), Z_STRLEN_P(retval), SQLITE_TRANSIENT);
					break;
			}
		} else {
			sqlite3_result_error(context, "failed to invoke callback", 0);
		}

		if (agg_context && agg_context->zval_context) {
			zval_ptr_dtor(&agg_context->zval_context);
		}
	} else {
		/* we're stepping in an aggregate; the return value goes into
		 * the context */
		if (agg_context && agg_context->zval_context) {
			zval_ptr_dtor(&agg_context->zval_context);
		}
		if (retval) {
			agg_context->zval_context = retval;
			retval = NULL;
		} else {
			agg_context->zval_context = NULL;
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
	php_sqlite3_agg_context *agg_context = (php_sqlite3_agg_context *)sqlite3_aggregate_context(context, sizeof(php_sqlite3_agg_context));

	TSRMLS_FETCH();
	agg_context->row_count++;

	sqlite3_do_callback(&func->astep, func->step, argc, argv, context, 1 TSRMLS_CC);
}
/* }}} */

static void php_sqlite3_callback_final(sqlite3_context *context) /* {{{ */
{
	php_sqlite3_func *func = (php_sqlite3_func *)sqlite3_user_data(context);
	php_sqlite3_agg_context *agg_context = (php_sqlite3_agg_context *)sqlite3_aggregate_context(context, sizeof(php_sqlite3_agg_context));

	TSRMLS_FETCH();
	agg_context->row_count = 0;

	sqlite3_do_callback(&func->afini, func->fini, 0, NULL, context, 1 TSRMLS_CC);
}
/* }}} */

static int php_sqlite3_callback_compare(void *coll, int a_len, const void *a, int b_len, const void* b) /* {{{ */
{
	php_sqlite3_collation *collation = (php_sqlite3_collation*)coll;
	zval ***zargs = NULL;
	zval *retval = NULL;
	int ret;

	TSRMLS_FETCH();

	collation->fci.fci.size = (sizeof(collation->fci.fci));
	collation->fci.fci.function_table = EG(function_table);
	collation->fci.fci.function_name = collation->cmp_func;
	collation->fci.fci.symbol_table = NULL;
	collation->fci.fci.object_ptr = NULL;
	collation->fci.fci.retval_ptr_ptr = &retval;
	collation->fci.fci.param_count = 2;

	zargs = (zval***)safe_emalloc(2, sizeof(zval**), 0);
	zargs[0] = emalloc(sizeof(zval*));
	zargs[1] = emalloc(sizeof(zval*));

	MAKE_STD_ZVAL(*zargs[0]);
	ZVAL_STRINGL(*zargs[0], a, a_len, 1);

	MAKE_STD_ZVAL(*zargs[1]);
	ZVAL_STRINGL(*zargs[1], b, b_len, 1);
 
	collation->fci.fci.params = zargs;

	if ((ret = zend_call_function(&collation->fci.fci, &collation->fci.fcc TSRMLS_CC)) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "An error occurred while invoking the compare callback");
	}

	zval_ptr_dtor(zargs[0]);
	zval_ptr_dtor(zargs[1]);
	efree(zargs[0]);
	efree(zargs[1]);
	efree(zargs);

	//retval ought to contain a ZVAL_LONG by now
	// (the result of a comparison, i.e. most likely -1, 0, or 1)
	//I suppose we could accept any scalar return type, though.
	if (Z_TYPE_P(retval) != IS_LONG){
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "An error occurred while invoking the compare callback (invalid return type).  Collation behaviour is undefined.");
	}else{
		ret = Z_LVAL_P(retval);
	}

	zval_ptr_dtor(&retval);

	return ret;
}
/* }}} */

/* {{{ proto bool SQLite3::createFunction(string name, mixed callback [, int argcount])
   Allows registration of a PHP function as a SQLite UDF that can be called within SQL statements. */
PHP_METHOD(sqlite3, createFunction)
{
	php_sqlite3_db_object *db_obj;
	zval *object = getThis();
	php_sqlite3_func *func;
	char *sql_func, *callback_name;
	int sql_func_len;
	zval *callback_func;
	long sql_func_num_args = -1;
	db_obj = (php_sqlite3_db_object *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->initialised, SQLite3)

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz|l", &sql_func, &sql_func_len, &callback_func, &sql_func_num_args) == FAILURE) {
		return;
	}

	if (!sql_func_len) {
		RETURN_FALSE;
	}

	if (!zend_is_callable(callback_func, 0, &callback_name TSRMLS_CC)) {
		php_sqlite3_error(db_obj, "Not a valid callback function %s", callback_name);
		efree(callback_name);
		RETURN_FALSE;
	}
	efree(callback_name);

	func = (php_sqlite3_func *)ecalloc(1, sizeof(*func));

	if (sqlite3_create_function(db_obj->db, sql_func, sql_func_num_args, SQLITE_UTF8, func, php_sqlite3_callback_func, NULL, NULL) == SQLITE_OK) {
		func->func_name = estrdup(sql_func);

		MAKE_STD_ZVAL(func->func);
		MAKE_COPY_ZVAL(&callback_func, func->func);

		func->argc = sql_func_num_args;
		func->next = db_obj->funcs;
		db_obj->funcs = func;

		RETURN_TRUE;
	}
	efree(func);

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool SQLite3::createAggregate(string name, mixed step, mixed final [, int argcount])
   Allows registration of a PHP function for use as an aggregate. */
PHP_METHOD(sqlite3, createAggregate)
{
	php_sqlite3_db_object *db_obj;
	zval *object = getThis();
	php_sqlite3_func *func;
	char *sql_func, *callback_name;
	int sql_func_len;
	zval *step_callback, *fini_callback;
	long sql_func_num_args = -1;
	db_obj = (php_sqlite3_db_object *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->initialised, SQLite3)

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "szz|l", &sql_func, &sql_func_len, &step_callback, &fini_callback, &sql_func_num_args) == FAILURE) {
		return;
	}

	if (!sql_func_len) {
		RETURN_FALSE;
	}

	if (!zend_is_callable(step_callback, 0, &callback_name TSRMLS_CC)) {
		php_sqlite3_error(db_obj, "Not a valid callback function %s", callback_name);
		efree(callback_name);
		RETURN_FALSE;
	}
	efree(callback_name);

	if (!zend_is_callable(fini_callback, 0, &callback_name TSRMLS_CC)) {
		php_sqlite3_error(db_obj, "Not a valid callback function %s", callback_name);
		efree(callback_name);
		RETURN_FALSE;
	}
	efree(callback_name);

	func = (php_sqlite3_func *)ecalloc(1, sizeof(*func));

	if (sqlite3_create_function(db_obj->db, sql_func, sql_func_num_args, SQLITE_UTF8, func, NULL, php_sqlite3_callback_step, php_sqlite3_callback_final) == SQLITE_OK) {
		func->func_name = estrdup(sql_func);

		MAKE_STD_ZVAL(func->step);
		MAKE_COPY_ZVAL(&step_callback, func->step);

		MAKE_STD_ZVAL(func->fini);
		MAKE_COPY_ZVAL(&fini_callback, func->fini);

		func->argc = sql_func_num_args;
		func->next = db_obj->funcs;
		db_obj->funcs = func;

		RETURN_TRUE;
	}
	efree(func);

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool SQLite3::createCollation(string name, mixed callback)
   Registers a PHP function as a comparator that can be used with the SQL COLLATE operator. Callback must accept two strings and return an integer (as strcmp()). */
PHP_METHOD(sqlite3, createCollation)
{
	php_sqlite3_db_object *db_obj;
	zval *object = getThis();
	php_sqlite3_collation *collation;
	char *collation_name, *callback_name;
	int collation_name_len;
	zval *callback_func;
	db_obj = (php_sqlite3_db_object *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->initialised, SQLite3)

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &collation_name, &collation_name_len, &callback_func) == FAILURE) {
		RETURN_FALSE;
	}

	if (!collation_name_len) {
		RETURN_FALSE;
	}

	if (!zend_is_callable(callback_func, 0, &callback_name TSRMLS_CC)) {
		php_sqlite3_error(db_obj, "Not a valid callback function %s", callback_name);
		efree(callback_name);
		RETURN_FALSE;
	}
	efree(callback_name);

	collation = (php_sqlite3_collation *)ecalloc(1, sizeof(*collation));
	if (sqlite3_create_collation(db_obj->db, collation_name, SQLITE_UTF8, collation, php_sqlite3_callback_compare) == SQLITE_OK) {
		collation->collation_name = estrdup(collation_name);

		MAKE_STD_ZVAL(collation->cmp_func);
		MAKE_COPY_ZVAL(&callback_func, collation->cmp_func);

		collation->next = db_obj->collations;
		db_obj->collations = collation;

		RETURN_TRUE;
	}
	efree(collation);

	RETURN_FALSE;
}
/* }}} */

typedef struct {
	sqlite3_blob *blob;
	size_t		 position;
	size_t       size;
} php_stream_sqlite3_data;

static size_t php_sqlite3_stream_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC)
{
/*	php_stream_sqlite3_data *sqlite3_stream = (php_stream_sqlite3_data *) stream->abstract; */
	
	return 0;
}

static size_t php_sqlite3_stream_read(php_stream *stream, char *buf, size_t count TSRMLS_DC)
{
	php_stream_sqlite3_data *sqlite3_stream = (php_stream_sqlite3_data *) stream->abstract;

	if (sqlite3_stream->position + count >= sqlite3_stream->size) {
		count = sqlite3_stream->size - sqlite3_stream->position;
		stream->eof = 1;
	}
	if (count) {
		if (sqlite3_blob_read(sqlite3_stream->blob, buf, count, sqlite3_stream->position) != SQLITE_OK) {
			return 0;
		}
		sqlite3_stream->position += count;
	}
	return count;
}

static int php_sqlite3_stream_close(php_stream *stream, int close_handle TSRMLS_DC)
{
	php_stream_sqlite3_data *sqlite3_stream = (php_stream_sqlite3_data *) stream->abstract;
	
	if (sqlite3_blob_close(sqlite3_stream->blob) != SQLITE_OK) {
		/* Error occurred, but it still closed */
	}

	efree(sqlite3_stream);
	
	return 0;
}

static int php_sqlite3_stream_flush(php_stream *stream TSRMLS_DC)
{
	/* do nothing */
	return 0;
}

/* {{{ */
static int php_sqlite3_stream_seek(php_stream *stream, off_t offset, int whence, off_t *newoffs TSRMLS_DC)
{
	php_stream_sqlite3_data *sqlite3_stream = (php_stream_sqlite3_data *) stream->abstract;

	switch(whence) {
		case SEEK_CUR:
			if (offset < 0) {
				if (sqlite3_stream->position < (size_t)(-offset)) {
					sqlite3_stream->position = 0;
					*newoffs = -1;
					return -1;
				} else {
					sqlite3_stream->position = sqlite3_stream->position + offset;
					*newoffs = sqlite3_stream->position;
					stream->eof = 0;
					return 0;
				}
			} else {
				if (sqlite3_stream->position + (size_t)(offset) > sqlite3_stream->size) {
					sqlite3_stream->position = sqlite3_stream->size;
					*newoffs = -1;
					return -1;
				} else {
					sqlite3_stream->position = sqlite3_stream->position + offset;
					*newoffs = sqlite3_stream->position;
					stream->eof = 0;
					return 0;
				}
			}
		case SEEK_SET:
			if (sqlite3_stream->size < (size_t)(offset)) {
				sqlite3_stream->position = sqlite3_stream->size;
				*newoffs = -1;
				return -1;
			} else {
				sqlite3_stream->position = offset;
				*newoffs = sqlite3_stream->position;
				stream->eof = 0;
				return 0;
			}
		case SEEK_END:
			if (offset > 0) {
				sqlite3_stream->position = sqlite3_stream->size;
				*newoffs = -1;
				return -1;
			} else if (sqlite3_stream->size < (size_t)(-offset)) {
				sqlite3_stream->position = 0;
				*newoffs = -1;
				return -1;
			} else {
				sqlite3_stream->position = sqlite3_stream->size + offset;
				*newoffs = sqlite3_stream->position;
				stream->eof = 0;
				return 0;
			}
		default:
			*newoffs = sqlite3_stream->position;
			return -1;
	}
}
/* }}} */


static int php_sqlite3_stream_cast(php_stream *stream, int castas, void **ret TSRMLS_DC)
{
	return FAILURE;
}

static int php_sqlite3_stream_stat(php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC)
{
	php_stream_sqlite3_data *sqlite3_stream = (php_stream_sqlite3_data *) stream->abstract;
	ssb->sb.st_size = sqlite3_stream->size;
	return 0;
}

static php_stream_ops php_stream_sqlite3_ops = {
	php_sqlite3_stream_write,
	php_sqlite3_stream_read,
	php_sqlite3_stream_close,
	php_sqlite3_stream_flush,
	"SQLite3",
	php_sqlite3_stream_seek,
	php_sqlite3_stream_cast,
	php_sqlite3_stream_stat
};

/* {{{ proto resource SQLite3::openBlob(string table, string column, int rowid [, string dbname])
   Open a blob as a stream which we can read / write to. */
PHP_METHOD(sqlite3, openBlob)
{
	php_sqlite3_db_object *db_obj;
	zval *object = getThis();
	char *table, *column, *dbname = "main";
	int table_len, column_len, dbname_len;
	long rowid, flags = 0;
	sqlite3_blob *blob = NULL;
	php_stream_sqlite3_data *sqlite3_stream;
	php_stream *stream;

	db_obj = (php_sqlite3_db_object *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->initialised, SQLite3)

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssl|s", &table, &table_len, &column, &column_len, &rowid, &dbname, &dbname_len) == FAILURE) {
		return;
	}

	if (sqlite3_blob_open(db_obj->db, dbname, table, column, rowid, flags, &blob) != SQLITE_OK) {
		php_sqlite3_error(db_obj, "Unable to open blob: %s", sqlite3_errmsg(db_obj->db));
		RETURN_FALSE;
	}

	sqlite3_stream = emalloc(sizeof(php_stream_sqlite3_data));
	sqlite3_stream->blob = blob;
	sqlite3_stream->position = 0;
	sqlite3_stream->size = sqlite3_blob_bytes(blob);
	
	stream = php_stream_alloc(&php_stream_sqlite3_ops, sqlite3_stream, 0, "rb");

	if (stream) {
		php_stream_to_zval(stream, return_value);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool SQLite3::enableExceptions([bool enableExceptions = false])
   Enables an exception error mode. */
PHP_METHOD(sqlite3, enableExceptions)
{
	php_sqlite3_db_object *db_obj;
	zval *object = getThis();
	zend_bool enableExceptions = 0;

	db_obj = (php_sqlite3_db_object *)zend_object_store_get_object(object TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &enableExceptions) == FAILURE) {
		return;
	}

	RETVAL_BOOL(db_obj->exception);

	db_obj->exception = enableExceptions;
}
/* }}} */

/* {{{ proto int SQLite3Stmt::paramCount()
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

/* {{{ proto bool SQLite3Stmt::close()
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

/* {{{ proto bool SQLite3Stmt::reset()
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
		php_sqlite3_error(stmt_obj->db_obj, "Unable to reset statement: %s", sqlite3_errmsg(sqlite3_db_handle(stmt_obj->stmt)));
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool SQLite3Stmt::clear()
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
		php_sqlite3_error(stmt_obj->db_obj, "Unable to clear statement: %s", sqlite3_errmsg(sqlite3_db_handle(stmt_obj->stmt)));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool SQLite3Stmt::readOnly()
   Returns true if a statement is definitely read only */
PHP_METHOD(sqlite3stmt, readOnly)
{
	php_sqlite3_stmt *stmt_obj;
	zval *object = getThis();
	stmt_obj = (php_sqlite3_stmt *)zend_object_store_get_object(object TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

#if SQLITE_VERSION_NUMBER >= 3007004
	if (sqlite3_stmt_readonly(stmt_obj->stmt)) {
		RETURN_TRUE;
	}
#endif
	RETURN_FALSE;
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
   Bind Parameter to a stmt variable. */
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

/* {{{ proto bool SQLite3Stmt::bindValue(int parameter_number, mixed parameter [, int type])
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

	SQLITE3_CHECK_INITIALIZED(stmt_obj->db_obj, stmt_obj->initialised, SQLite3)

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
#if LONG_MAX > 2147483647
					sqlite3_bind_int64(stmt_obj->stmt, param->param_number, Z_LVAL_P(param->parameter));
#else
					sqlite3_bind_int(stmt_obj->stmt, param->param_number, Z_LVAL_P(param->parameter));
#endif
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
							php_sqlite3_error(stmt_obj->db_obj, "Unable to read stream for parameter %ld", param->param_number);
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
					sqlite3_bind_text(stmt_obj->stmt, param->param_number, Z_STRVAL_P(param->parameter), Z_STRLEN_P(param->parameter), SQLITE_STATIC);
					break;

				case SQLITE_NULL:
					sqlite3_bind_null(stmt_obj->stmt, param->param_number);
					break;

				default:
					php_sqlite3_error(stmt_obj->db_obj, "Unknown parameter type: %ld for parameter %ld", param->type, param->param_number);
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
			sqlite3_reset(stmt_obj->stmt);
			object_init_ex(return_value, php_sqlite3_result_entry);
			result = (php_sqlite3_result *)zend_object_store_get_object(return_value TSRMLS_CC);

			Z_ADDREF_P(object);
	
			result->is_prepared_statement = 1;
			result->db_obj = stmt_obj->db_obj;
			result->stmt_obj = stmt_obj;
			result->stmt_obj_zval = getThis();

			break;
		}
		case SQLITE_ERROR:
			sqlite3_reset(stmt_obj->stmt);

		default:
			php_sqlite3_error(stmt_obj->db_obj, "Unable to execute statement: %s", sqlite3_errmsg(sqlite3_db_handle(stmt_obj->stmt)));
			zval_dtor(return_value);
			RETURN_FALSE;
	}

	return;
}
/* }}} */

/* {{{ proto int SQLite3Stmt::__construct(SQLite3 dbobject, String Statement)
   __constructor for SQLite3Stmt. */
PHP_METHOD(sqlite3stmt, __construct)
{
	php_sqlite3_stmt *stmt_obj;
	php_sqlite3_db_object *db_obj;
	zval *object = getThis();
	zval *db_zval;
	char *sql;
	int sql_len, errcode;
	zend_error_handling error_handling;
	php_sqlite3_free_list *free_item;

	stmt_obj = (php_sqlite3_stmt *)zend_object_store_get_object(object TSRMLS_CC);
	zend_replace_error_handling(EH_THROW, NULL, &error_handling TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os", &db_zval, php_sqlite3_sc_entry, &sql, &sql_len) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}

	db_obj = (php_sqlite3_db_object *)zend_object_store_get_object(db_zval TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->initialised, SQLite3)

	zend_restore_error_handling(&error_handling TSRMLS_CC);

	if (!sql_len) {
		RETURN_FALSE;
	}

	stmt_obj->db_obj = db_obj;
	stmt_obj->db_obj_zval = db_zval;

	Z_ADDREF_P(db_zval);
	
	errcode = sqlite3_prepare_v2(db_obj->db, sql, sql_len, &(stmt_obj->stmt), NULL);
	if (errcode != SQLITE_OK) {
		php_sqlite3_error(db_obj, "Unable to prepare statement: %d, %s", errcode, sqlite3_errmsg(db_obj->db));
		zval_dtor(return_value);
		RETURN_FALSE;
	}
	stmt_obj->initialised = 1;

	free_item = emalloc(sizeof(php_sqlite3_free_list));
	free_item->stmt_obj = stmt_obj;
	free_item->stmt_obj_zval = getThis();

	zend_llist_add_element(&(db_obj->free_list), &free_item);
}
/* }}} */

/* {{{ proto int SQLite3Result::numColumns()
   Number of columns in the result set. */
PHP_METHOD(sqlite3result, numColumns)
{
	php_sqlite3_result *result_obj;
	zval *object = getThis();
	result_obj = (php_sqlite3_result *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(result_obj->db_obj, result_obj->stmt_obj->initialised, SQLite3Result)

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(sqlite3_column_count(result_obj->stmt_obj->stmt));
}
/* }}} */

/* {{{ proto string SQLite3Result::columnName(int column)
   Returns the name of the nth column. */
PHP_METHOD(sqlite3result, columnName)
{
	php_sqlite3_result *result_obj;
	zval *object = getThis();
	long column = 0;
	char *column_name;
	result_obj = (php_sqlite3_result *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(result_obj->db_obj, result_obj->stmt_obj->initialised, SQLite3Result)

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &column) == FAILURE) {
		return;
	}
	column_name = (char*) sqlite3_column_name(result_obj->stmt_obj->stmt, column);

	if (column_name == NULL) {
		RETURN_FALSE;
	}
		
	RETVAL_STRING(column_name, 1);
}
/* }}} */

/* {{{ proto int SQLite3Result::columnType(int column)
   Returns the type of the nth column. */
PHP_METHOD(sqlite3result, columnType)
{
	php_sqlite3_result *result_obj;
	zval *object = getThis();
	long column = 0;
	result_obj = (php_sqlite3_result *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(result_obj->db_obj, result_obj->stmt_obj->initialised, SQLite3Result)

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &column) == FAILURE) {
		return;
	}

	if (result_obj->complete) {
		RETURN_FALSE;
	}

	RETURN_LONG(sqlite3_column_type(result_obj->stmt_obj->stmt, column));
}
/* }}} */

/* {{{ proto array SQLite3Result::fetchArray([int mode])
   Fetch a result row as both an associative or numerically indexed array or both. */
PHP_METHOD(sqlite3result, fetchArray)
{
	php_sqlite3_result *result_obj;
	zval *object = getThis();
	int i, ret;
	long mode = PHP_SQLITE3_BOTH;
	result_obj = (php_sqlite3_result *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(result_obj->db_obj, result_obj->stmt_obj->initialised, SQLite3Result)

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
					if (mode & PHP_SQLITE3_NUM) {
						Z_ADDREF_P(data);
					}
					add_assoc_zval(return_value, (char*)sqlite3_column_name(result_obj->stmt_obj->stmt, i), data);
				}
			}
			break;

		case SQLITE_DONE:
			result_obj->complete = 1;
			RETURN_FALSE;
			break;

		default:
			php_sqlite3_error(result_obj->db_obj, "Unable to execute statement: %s", sqlite3_errmsg(sqlite3_db_handle(result_obj->stmt_obj->stmt)));
	}
}
/* }}} */

/* {{{ proto bool SQLite3Result::reset()
   Resets the result set back to the first row. */
PHP_METHOD(sqlite3result, reset)
{
	php_sqlite3_result *result_obj;
	zval *object = getThis();
	result_obj = (php_sqlite3_result *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(result_obj->db_obj, result_obj->stmt_obj->initialised, SQLite3Result)

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

/* {{{ proto bool SQLite3Result::finalize()
   Closes the result set. */
PHP_METHOD(sqlite3result, finalize)
{
	php_sqlite3_result *result_obj;
	zval *object = getThis();
	result_obj = (php_sqlite3_result *)zend_object_store_get_object(object TSRMLS_CC);

	SQLITE3_CHECK_INITIALIZED(result_obj->db_obj, result_obj->stmt_obj->initialised, SQLite3Result)

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

/* {{{ proto int SQLite3Result::__construct()
   __constructor for SQLite3Result. */
PHP_METHOD(sqlite3result, __construct)
{
	zend_throw_exception(zend_exception_get_default(TSRMLS_C), "SQLite3Result cannot be directly instantiated", 0 TSRMLS_CC);
}
/* }}} */

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO(arginfo_sqlite3_open, 0)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(0, encryption_key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_sqlite3_busytimeout, 0)
	ZEND_ARG_INFO(0, ms)
ZEND_END_ARG_INFO()

#ifndef SQLITE_OMIT_LOAD_EXTENSION
ZEND_BEGIN_ARG_INFO(arginfo_sqlite3_loadextension, 0)
	ZEND_ARG_INFO(0, shared_library)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3_escapestring, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3_query, 0, 0, 1)
	ZEND_ARG_INFO(0, query)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3_querysingle, 0, 0, 1)
	ZEND_ARG_INFO(0, query)
	ZEND_ARG_INFO(0, entire_row)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3_createfunction, 0, 0, 2)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, callback)
	ZEND_ARG_INFO(0, argument_count)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3_createaggregate, 0, 0, 3)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, step_callback)
	ZEND_ARG_INFO(0, final_callback)
	ZEND_ARG_INFO(0, argument_count)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3_createcollation, 0, 0, 2)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(argingo_sqlite3_openblob, 0, 0, 3)
	ZEND_ARG_INFO(0, table)
	ZEND_ARG_INFO(0, column)
	ZEND_ARG_INFO(0, rowid)
	ZEND_ARG_INFO(0, dbname)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(argingo_sqlite3_enableexceptions, 0, 0, 1)
	ZEND_ARG_INFO(0, enableExceptions)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3stmt_bindparam, 0, 0, 2)
	ZEND_ARG_INFO(0, param_number)
	ZEND_ARG_INFO(1, param)
	ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3stmt_bindvalue, 0, 0, 2)
	ZEND_ARG_INFO(0, param_number)
	ZEND_ARG_INFO(0, param)
	ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_sqlite3stmt_construct, 1)
	ZEND_ARG_INFO(0, sqlite3)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3result_columnname, 0, 0, 1)
	ZEND_ARG_INFO(0, column_number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3result_columntype, 0, 0, 1)
	ZEND_ARG_INFO(0, column_number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3result_fetcharray, 0, 0, 1)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_sqlite3_void, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ php_sqlite3_class_methods */
static zend_function_entry php_sqlite3_class_methods[] = {
	PHP_ME(sqlite3,		open,				arginfo_sqlite3_open, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		close,				arginfo_sqlite3_void, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		exec,				arginfo_sqlite3_query, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		version,			arginfo_sqlite3_void, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(sqlite3,		lastInsertRowID,	arginfo_sqlite3_void, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		lastErrorCode,		arginfo_sqlite3_void, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		lastErrorMsg,		arginfo_sqlite3_void, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		busyTimeout,		arginfo_sqlite3_busytimeout, ZEND_ACC_PUBLIC)
#ifndef SQLITE_OMIT_LOAD_EXTENSION
	PHP_ME(sqlite3,		loadExtension,		arginfo_sqlite3_loadextension, ZEND_ACC_PUBLIC)
#endif
	PHP_ME(sqlite3,		changes,			arginfo_sqlite3_void, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		escapeString,		arginfo_sqlite3_escapestring, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(sqlite3,		prepare,			arginfo_sqlite3_query, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		query,				arginfo_sqlite3_query, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		querySingle,		arginfo_sqlite3_querysingle, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		createFunction,		arginfo_sqlite3_createfunction, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		createAggregate,	arginfo_sqlite3_createaggregate, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		createCollation,	arginfo_sqlite3_createcollation, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		openBlob,			argingo_sqlite3_openblob, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		enableExceptions,	argingo_sqlite3_enableexceptions, ZEND_ACC_PUBLIC)
	/* Aliases */
	PHP_MALIAS(sqlite3,	__construct, open, arginfo_sqlite3_open, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_FE_END
};
/* }}} */

/* {{{ php_sqlite3_stmt_class_methods */
static zend_function_entry php_sqlite3_stmt_class_methods[] = {
	PHP_ME(sqlite3stmt, paramCount,	arginfo_sqlite3_void, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3stmt, close,		arginfo_sqlite3_void, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3stmt, reset,		arginfo_sqlite3_void, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3stmt, clear,		arginfo_sqlite3_void, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3stmt, execute,	arginfo_sqlite3_void, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3stmt, bindParam,	arginfo_sqlite3stmt_bindparam, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3stmt, bindValue,	arginfo_sqlite3stmt_bindvalue, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3stmt, readOnly,	arginfo_sqlite3_void, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3stmt, __construct, arginfo_sqlite3stmt_construct, ZEND_ACC_PRIVATE|ZEND_ACC_CTOR)
	PHP_FE_END
};
/* }}} */

/* {{{ php_sqlite3_result_class_methods */
static zend_function_entry php_sqlite3_result_class_methods[] = {
	PHP_ME(sqlite3result, numColumns,		arginfo_sqlite3_void, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3result, columnName,		arginfo_sqlite3result_columnname, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3result, columnType,		arginfo_sqlite3result_columntype, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3result, fetchArray,		arginfo_sqlite3result_fetcharray, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3result, reset,			arginfo_sqlite3_void, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3result, finalize,			arginfo_sqlite3_void, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3result, __construct, 		arginfo_sqlite3_void, ZEND_ACC_PRIVATE|ZEND_ACC_CTOR)
	PHP_FE_END
};
/* }}} */

/* {{{ Authorization Callback 
*/
static int php_sqlite3_authorizer(void *autharg, int access_type, const char *arg3, const char *arg4, const char *arg5, const char *arg6)
{
	switch (access_type) {
		case SQLITE_ATTACH:
		{
			if (memcmp(arg3, ":memory:", sizeof(":memory:")) && *arg3) {
				TSRMLS_FETCH();

#if PHP_API_VERSION < 20100412
				if (PG(safe_mode) && (!php_checkuid(arg3, NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
					return SQLITE_DENY;
				}
#endif

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
	php_sqlite3_collation *collation;

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

	while (intern->collations){
		collation = intern->collations;
		intern->collations = collation->next;
		if (intern->initialised && intern->db){
			sqlite3_create_collation(intern->db, collation->collation_name, SQLITE_UTF8, NULL, NULL);
		}
		efree((char*)collation->collation_name);
		if (collation->cmp_func){
			zval_ptr_dtor(&collation->cmp_func);
		}
		efree(collation);
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
		zval_ptr_dtor(&intern->db_obj_zval);
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
	zend_object_value retval;
	php_sqlite3_db_object *intern;

	/* Allocate memory for it */
	intern = emalloc(sizeof(php_sqlite3_db_object));
	memset(intern, 0, sizeof(php_sqlite3_db_object));
	intern->exception = 0;

	/* Need to keep track of things to free */
	zend_llist_init(&(intern->free_list),   sizeof(php_sqlite3_free_list *), (llist_dtor_func_t)php_sqlite3_free_list_dtor, 0);

	zend_object_std_init(&intern->zo, class_type TSRMLS_CC);
	object_properties_init(&intern->zo, class_type);

	retval.handle = zend_objects_store_put(intern, NULL, (zend_objects_free_object_storage_t) php_sqlite3_object_free_storage, NULL TSRMLS_CC);
	retval.handlers = (zend_object_handlers *) &sqlite3_object_handlers;

	return retval;
}
/* }}} */

static zend_object_value php_sqlite3_stmt_object_new(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
	zend_object_value retval;
	php_sqlite3_stmt *intern;

	/* Allocate memory for it */
	intern = emalloc(sizeof(php_sqlite3_stmt));
	memset(intern, 0, sizeof(php_sqlite3_stmt));

	intern->db_obj_zval = NULL;

	zend_object_std_init(&intern->zo, class_type TSRMLS_CC);
	object_properties_init(&intern->zo, class_type);

	retval.handle = zend_objects_store_put(intern, NULL, (zend_objects_free_object_storage_t) php_sqlite3_stmt_object_free_storage, NULL TSRMLS_CC);
	retval.handlers = (zend_object_handlers *) &sqlite3_stmt_object_handlers;

	return retval;
}
/* }}} */

static zend_object_value php_sqlite3_result_object_new(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
	zend_object_value retval;
	php_sqlite3_result *intern;

	/* Allocate memory for it */
	intern = emalloc(sizeof(php_sqlite3_result));
	memset(intern, 0, sizeof(php_sqlite3_result));

	intern->complete = 0;
	intern->is_prepared_statement = 0;
	intern->stmt_obj_zval = NULL;

	zend_object_std_init(&intern->zo, class_type TSRMLS_CC);
	object_properties_init(&intern->zo, class_type);

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
