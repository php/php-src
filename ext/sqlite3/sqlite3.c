/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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
static void sqlite3_param_dtor(zval *data);
static int php_sqlite3_compare_stmt_zval_free(php_sqlite3_free_list **free_list, zval *statement);

/* {{{ Error Handler
*/
static void php_sqlite3_error(php_sqlite3_db_object *db_obj, char *format, ...)
{
	va_list arg;
	char 	*message;

	va_start(arg, format);
	vspprintf(&message, 0, format, arg);
	va_end(arg);

	if (db_obj && db_obj->exception) {
		zend_throw_exception(zend_ce_exception, message, 0);
	} else {
		php_error_docref(NULL, E_WARNING, "%s", message);
	}

	if (message) {
		efree(message);
	}
}
/* }}} */

#define SQLITE3_CHECK_INITIALIZED(db_obj, member, class_name) \
	if (!(db_obj) || !(member)) { \
		php_sqlite3_error(db_obj, "The " #class_name " object has not been correctly initialised"); \
		RETURN_FALSE; \
	}

#define SQLITE3_CHECK_INITIALIZED_STMT(member, class_name) \
	if (!(member)) { \
		php_error_docref(NULL, E_WARNING, "The " #class_name " object has not been correctly initialised"); \
		RETURN_FALSE; \
	}

/* {{{ PHP_INI
*/
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("sqlite3.extension_dir",  NULL, PHP_INI_SYSTEM, OnUpdateString, extension_dir, zend_sqlite3_globals, sqlite3_globals)
#if SQLITE_VERSION_NUMBER >= 3026000
	STD_PHP_INI_ENTRY("sqlite3.defensive",  "1", PHP_INI_SYSTEM, OnUpdateBool, dbconfig_defensive, zend_sqlite3_globals, sqlite3_globals)
#endif
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
	zval *object = ZEND_THIS;
	char *filename, *encryption_key, *fullpath;
	size_t filename_len, encryption_key_len = 0;
	zend_long flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
	int rc;

	db_obj = Z_SQLITE3_DB_P(object);

	if (FAILURE == zend_parse_parameters_throw(ZEND_NUM_ARGS(), "p|ls", &filename, &filename_len, &flags, &encryption_key, &encryption_key_len)) {
		return;
	}

	if (db_obj->initialised) {
		zend_throw_exception(zend_ce_exception, "Already initialised DB Object", 0);
		return;
	}

	if (filename_len != 0 && (filename_len != sizeof(":memory:")-1 ||
			memcmp(filename, ":memory:", sizeof(":memory:")-1) != 0)) {
		if (!(fullpath = expand_filepath(filename, NULL))) {
			zend_throw_exception(zend_ce_exception, "Unable to expand filepath", 0);
			return;
		}

		if (php_check_open_basedir(fullpath)) {
			zend_throw_exception_ex(zend_ce_exception, 0, "open_basedir prohibits opening %s", fullpath);
			efree(fullpath);
			return;
		}
	} else {
		/* filename equals "" or ":memory:" */
		fullpath = filename;
	}

	rc = sqlite3_open_v2(fullpath, &(db_obj->db), flags, NULL);
	if (rc != SQLITE_OK) {
		zend_throw_exception_ex(zend_ce_exception, 0, "Unable to open database: %s",
#ifdef HAVE_SQLITE3_ERRSTR
				db_obj->db ? sqlite3_errmsg(db_obj->db) : sqlite3_errstr(rc));
#else
				db_obj->db ? sqlite3_errmsg(db_obj->db) : "");
#endif
		sqlite3_close(db_obj->db);
		if (fullpath != filename) {
			efree(fullpath);
		}
		return;
	}

#if SQLITE_HAS_CODEC
	if (encryption_key_len > 0) {
		if (sqlite3_key(db_obj->db, encryption_key, encryption_key_len) != SQLITE_OK) {
			zend_throw_exception_ex(zend_ce_exception, 0, "Unable to open database: %s", sqlite3_errmsg(db_obj->db));
			sqlite3_close(db_obj->db);
			return;
		}
	}
#endif

	db_obj->initialised = 1;

	if (PG(open_basedir) && *PG(open_basedir)) {
		sqlite3_set_authorizer(db_obj->db, php_sqlite3_authorizer, NULL);
	}

#if SQLITE_VERSION_NUMBER >= 3026000
	if (SQLITE3G(dbconfig_defensive)) {
		sqlite3_db_config(db_obj->db, SQLITE_DBCONFIG_DEFENSIVE, 1, NULL);
	}
#endif

	if (fullpath != filename) {
		efree(fullpath);
	}
}
/* }}} */

/* {{{ proto bool SQLite3::close()
   Close a SQLite 3 Database. */
PHP_METHOD(sqlite3, close)
{
	php_sqlite3_db_object *db_obj;
	zval *object = ZEND_THIS;
	int errcode;
	db_obj = Z_SQLITE3_DB_P(object);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (db_obj->initialised) {
        zend_llist_clean(&(db_obj->free_list));
		if(db_obj->db) {
            errcode = sqlite3_close(db_obj->db);
            if (errcode != SQLITE_OK) {
			    php_sqlite3_error(db_obj, "Unable to close database: %d, %s", errcode, sqlite3_errmsg(db_obj->db));
                RETURN_FALSE;
		    }
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
	zval *object = ZEND_THIS;
	zend_string *sql;
	char *errtext = NULL;
	db_obj = Z_SQLITE3_DB_P(object);

	SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->initialised, SQLite3)

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "S", &sql)) {
		return;
	}

	if (sqlite3_exec(db_obj->db, ZSTR_VAL(sql), NULL, NULL, &errtext) != SQLITE_OK) {
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

	add_assoc_string(return_value, "versionString", (char*)sqlite3_libversion());
	add_assoc_long(return_value, "versionNumber", sqlite3_libversion_number());

	return;
}
/* }}} */

/* {{{ proto int SQLite3::lastInsertRowID()
   Returns the rowid of the most recent INSERT into the database from the database connection. */
PHP_METHOD(sqlite3, lastInsertRowID)
{
	php_sqlite3_db_object *db_obj;
	zval *object = ZEND_THIS;
	db_obj = Z_SQLITE3_DB_P(object);

	SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->initialised, SQLite3)

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG((zend_long) sqlite3_last_insert_rowid(db_obj->db));
}
/* }}} */

/* {{{ proto int SQLite3::lastErrorCode()
   Returns the numeric result code of the most recent failed sqlite API call for the database connection. */
PHP_METHOD(sqlite3, lastErrorCode)
{
	php_sqlite3_db_object *db_obj;
	zval *object = ZEND_THIS;
	db_obj = Z_SQLITE3_DB_P(object);

	SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->db, SQLite3)

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (db_obj->initialised) {
		RETURN_LONG(sqlite3_errcode(db_obj->db));
	} else {
		RETURN_LONG(0);
	}
}
/* }}} */

/* {{{ proto int SQLite3::lastExtendedErrorCode()
   Returns the numeric extended result code of the most recent failed sqlite API call for the database connection. */
PHP_METHOD(sqlite3, lastExtendedErrorCode)
{
	php_sqlite3_db_object *db_obj;
	zval *object = ZEND_THIS;
	db_obj = Z_SQLITE3_DB_P(object);

	SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->db, SQLite3)

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (db_obj->initialised) {
		RETURN_LONG(sqlite3_extended_errcode(db_obj->db));
	} else {
		RETURN_LONG(0);
	}
}
/* }}} */

/* {{{ proto bool SQLite3::enableExtendedResultCodes([bool enable = true])
    Turns on or off the extended result codes feature of SQLite. */
PHP_METHOD(sqlite3, enableExtendedResultCodes)
{
	php_sqlite3_db_object *db_obj;
	zval *object = ZEND_THIS;
	zend_bool enable = 1;
	db_obj = Z_SQLITE3_DB_P(object);
	int ret;

	SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->db, SQLite3)

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &enable) == FAILURE) {
		return;
	}

	if (db_obj->initialised) {
		ret = sqlite3_extended_result_codes(db_obj->db, enable ? 1 : 0);
		if (ret == SQLITE_OK)
		{
			RETURN_TRUE;
		}
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string SQLite3::lastErrorMsg()
   Returns english text describing the most recent failed sqlite API call for the database connection. */
PHP_METHOD(sqlite3, lastErrorMsg)
{
	php_sqlite3_db_object *db_obj;
	zval *object = ZEND_THIS;
	db_obj = Z_SQLITE3_DB_P(object);

	SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->db, SQLite3)

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (db_obj->initialised) {
		RETURN_STRING((char *)sqlite3_errmsg(db_obj->db));
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ proto bool SQLite3::busyTimeout(int msecs)
   Sets a busy handler that will sleep until database is not locked or timeout is reached. Passing a value less than or equal to zero turns off all busy handlers. */
PHP_METHOD(sqlite3, busyTimeout)
{
	php_sqlite3_db_object *db_obj;
	zval *object = ZEND_THIS;
	zend_long ms;
#ifdef SQLITE_ENABLE_API_ARMOR
	int return_code;
#endif
	db_obj = Z_SQLITE3_DB_P(object);

	SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->initialised, SQLite3)

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "l", &ms)) {
		return;
	}

#ifdef SQLITE_ENABLE_API_ARMOR
	return_code = sqlite3_busy_timeout(db_obj->db, ms);
	if (return_code != SQLITE_OK) {
		php_sqlite3_error(db_obj, "Unable to set busy timeout: %d, %s", return_code, sqlite3_errmsg(db_obj->db));
		RETURN_FALSE;
	}
#else
	php_ignore_value(sqlite3_busy_timeout(db_obj->db, ms));
#endif

	RETURN_TRUE;
}
/* }}} */


#ifndef SQLITE_OMIT_LOAD_EXTENSION
/* {{{ proto bool SQLite3::loadExtension(String Shared Library)
   Attempts to load an SQLite extension library. */
PHP_METHOD(sqlite3, loadExtension)
{
	php_sqlite3_db_object *db_obj;
	zval *object = ZEND_THIS;
	char *extension, *lib_path, *extension_dir, *errtext = NULL;
	char fullpath[MAXPATHLEN];
	size_t extension_len, extension_dir_len;
	db_obj = Z_SQLITE3_DB_P(object);

	SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->initialised, SQLite3)

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "s", &extension, &extension_len)) {
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
	zval *object = ZEND_THIS;
	db_obj = Z_SQLITE3_DB_P(object);

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
	zend_string *sql;
	char *ret;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "S", &sql)) {
		return;
	}

	if (ZSTR_LEN(sql)) {
		ret = sqlite3_mprintf("%q", ZSTR_VAL(sql));
		if (ret) {
			RETVAL_STRING(ret);
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
	zval *object = ZEND_THIS;
	zend_string *sql;
	int errcode;
	php_sqlite3_free_list *free_item;

	db_obj = Z_SQLITE3_DB_P(object);

	SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->initialised, SQLite3)

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "S", &sql)) {
		return;
	}

	if (!ZSTR_LEN(sql)) {
		RETURN_FALSE;
	}

	object_init_ex(return_value, php_sqlite3_stmt_entry);
	stmt_obj = Z_SQLITE3_STMT_P(return_value);
	stmt_obj->db_obj = db_obj;
	Z_ADDREF_P(object);
	ZVAL_OBJ(&stmt_obj->db_obj_zval, Z_OBJ_P(object));

	errcode = sqlite3_prepare_v2(db_obj->db, ZSTR_VAL(sql), ZSTR_LEN(sql), &(stmt_obj->stmt), NULL);
	if (errcode != SQLITE_OK) {
		php_sqlite3_error(db_obj, "Unable to prepare statement: %d, %s", errcode, sqlite3_errmsg(db_obj->db));
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	stmt_obj->initialised = 1;

	free_item = emalloc(sizeof(php_sqlite3_free_list));
	free_item->stmt_obj = stmt_obj;
	ZVAL_OBJ(&free_item->stmt_obj_zval, Z_OBJ_P(return_value));

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
	zval *object = ZEND_THIS;
	zval stmt;
	zend_string *sql;
	char *errtext = NULL;
	int return_code;
	db_obj = Z_SQLITE3_DB_P(object);

	SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->initialised, SQLite3)

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "S", &sql)) {
		return;
	}

	if (!ZSTR_LEN(sql)) {
		RETURN_FALSE;
	}

	/* If there was no return value then just execute the query */
	if (!USED_RET()) {
		if (sqlite3_exec(db_obj->db, ZSTR_VAL(sql), NULL, NULL, &errtext) != SQLITE_OK) {
			php_sqlite3_error(db_obj, "%s", errtext);
			sqlite3_free(errtext);
		}
		return;
	}

	object_init_ex(&stmt, php_sqlite3_stmt_entry);
	stmt_obj = Z_SQLITE3_STMT_P(&stmt);
	stmt_obj->db_obj = db_obj;
	Z_ADDREF_P(object);
	ZVAL_OBJ(&stmt_obj->db_obj_zval, Z_OBJ_P(object));

	return_code = sqlite3_prepare_v2(db_obj->db, ZSTR_VAL(sql), ZSTR_LEN(sql), &(stmt_obj->stmt), NULL);
	if (return_code != SQLITE_OK) {
		php_sqlite3_error(db_obj, "Unable to prepare statement: %d, %s", return_code, sqlite3_errmsg(db_obj->db));
		zval_ptr_dtor(&stmt);
		RETURN_FALSE;
	}

	stmt_obj->initialised = 1;

	object_init_ex(return_value, php_sqlite3_result_entry);
	result = Z_SQLITE3_RESULT_P(return_value);
	result->db_obj = db_obj;
	result->stmt_obj = stmt_obj;
	ZVAL_OBJ(&result->stmt_obj_zval, Z_OBJ(stmt));

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
			if (!EG(exception)) {
				php_sqlite3_error(db_obj, "Unable to execute statement: %s", sqlite3_errmsg(db_obj->db));
			}
			sqlite3_finalize(stmt_obj->stmt);
			stmt_obj->initialised = 0;
			zval_ptr_dtor(return_value);
			RETURN_FALSE;
	}
}
/* }}} */

static void sqlite_value_to_zval(sqlite3_stmt *stmt, int column, zval *data) /* {{{ */
{
	sqlite3_int64 val;

	switch (sqlite3_column_type(stmt, column)) {
		case SQLITE_INTEGER:
			val = sqlite3_column_int64(stmt, column);
#if LONG_MAX <= 2147483647
			if (val > ZEND_LONG_MAX || val < ZEND_LONG_MIN) {
				ZVAL_STRINGL(data, (char *)sqlite3_column_text(stmt, column), sqlite3_column_bytes(stmt, column));
			} else {
#endif
				ZVAL_LONG(data, (zend_long) val);
#if LONG_MAX <= 2147483647
			}
#endif
			break;

		case SQLITE_FLOAT:
			ZVAL_DOUBLE(data, sqlite3_column_double(stmt, column));
			break;

		case SQLITE_NULL:
			ZVAL_NULL(data);
			break;

		case SQLITE3_TEXT:
			ZVAL_STRING(data, (char*)sqlite3_column_text(stmt, column));
			break;

		case SQLITE_BLOB:
		default:
			ZVAL_STRINGL(data, (char*)sqlite3_column_blob(stmt, column), sqlite3_column_bytes(stmt, column));
	}
}
/* }}} */

/* {{{ proto SQLite3Result SQLite3::querySingle(String Query [, bool entire_row = false])
   Returns a string of the first column, or an array of the entire row. */
PHP_METHOD(sqlite3, querySingle)
{
	php_sqlite3_db_object *db_obj;
	zval *object = ZEND_THIS;
	zend_string *sql;
	char *errtext = NULL;
	int return_code;
	zend_bool entire_row = 0;
	sqlite3_stmt *stmt;
	db_obj = Z_SQLITE3_DB_P(object);

	SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->initialised, SQLite3)

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "S|b", &sql, &entire_row)) {
		return;
	}

	if (!ZSTR_LEN(sql)) {
		RETURN_FALSE;
	}

	/* If there was no return value then just execute the query */
	if (!USED_RET()) {
		if (sqlite3_exec(db_obj->db, ZSTR_VAL(sql), NULL, NULL, &errtext) != SQLITE_OK) {
			php_sqlite3_error(db_obj, "%s", errtext);
			sqlite3_free(errtext);
		}
		return;
	}

	return_code = sqlite3_prepare_v2(db_obj->db, ZSTR_VAL(sql), ZSTR_LEN(sql), &stmt, NULL);
	if (return_code != SQLITE_OK) {
		php_sqlite3_error(db_obj, "Unable to prepare statement: %d, %s", return_code, sqlite3_errmsg(db_obj->db));
		RETURN_FALSE;
	}

	return_code = sqlite3_step(stmt);

	switch (return_code) {
		case SQLITE_ROW: /* Valid Row */
		{
			if (!entire_row) {
				sqlite_value_to_zval(stmt, 0, return_value);
			} else {
				int i = 0;
				array_init(return_value);
				for (i = 0; i < sqlite3_data_count(stmt); i++) {
					zval data;
					sqlite_value_to_zval(stmt, i, &data);
					add_assoc_zval(return_value, (char*)sqlite3_column_name(stmt, i), &data);
				}
			}
			break;
		}
		case SQLITE_DONE: /* Valid but no results */
		{
			if (!entire_row) {
				RETVAL_NULL();
			} else {
				RETVAL_EMPTY_ARRAY();
			}
			break;
		}
		default:
		if (!EG(exception)) {
			php_sqlite3_error(db_obj, "Unable to execute statement: %s", sqlite3_errmsg(db_obj->db));
		}
		RETVAL_FALSE;
	}
	sqlite3_finalize(stmt);
}
/* }}} */

static int sqlite3_do_callback(struct php_sqlite3_fci *fc, zval *cb, int argc, sqlite3_value **argv, sqlite3_context *context, int is_agg) /* {{{ */
{
	zval *zargs = NULL;
	zval retval;
	int i;
	int ret;
	int fake_argc;
	php_sqlite3_agg_context *agg_context = NULL;

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
		zargs = (zval *)safe_emalloc(fake_argc, sizeof(zval), 0);
	}

	if (is_agg) {
		/* summon the aggregation context */
		agg_context = (php_sqlite3_agg_context *)sqlite3_aggregate_context(context, sizeof(php_sqlite3_agg_context));

		if (Z_ISUNDEF(agg_context->zval_context)) {
			ZVAL_NULL(&agg_context->zval_context);
		}
		ZVAL_COPY(&zargs[0], &agg_context->zval_context);
		ZVAL_LONG(&zargs[1], agg_context->row_count);
	}

	for (i = 0; i < argc; i++) {
		switch (sqlite3_value_type(argv[i])) {
			case SQLITE_INTEGER:
#if ZEND_LONG_MAX > 2147483647
				ZVAL_LONG(&zargs[i + is_agg], sqlite3_value_int64(argv[i]));
#else
				ZVAL_LONG(&zargs[i + is_agg], sqlite3_value_int(argv[i]));
#endif
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

	if (is_agg) {
		zval_ptr_dtor(&zargs[0]);
	}

	/* clean up the params */
	if (fake_argc) {
		for (i = is_agg; i < argc + is_agg; i++) {
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
#if ZEND_LONG_MAX > 2147483647
					sqlite3_result_int64(context, Z_LVAL(retval));
#else
					sqlite3_result_int(context, Z_LVAL(retval));
#endif
					break;

				case IS_NULL:
					sqlite3_result_null(context);
					break;

				case IS_DOUBLE:
					sqlite3_result_double(context, Z_DVAL(retval));
					break;

				default: {
					zend_string *str = zval_try_get_string(&retval);
					if (UNEXPECTED(!str)) {
						ret = FAILURE;
						break;
					}
					sqlite3_result_text(context, ZSTR_VAL(str), ZSTR_LEN(str), SQLITE_TRANSIENT);
					zend_string_release(str);
					break;
				}
			}
		} else {
			sqlite3_result_error(context, "failed to invoke callback", 0);
		}

		if (agg_context && !Z_ISUNDEF(agg_context->zval_context)) {
			zval_ptr_dtor(&agg_context->zval_context);
		}
	} else {
		/* we're stepping in an aggregate; the return value goes into
		 * the context */
		if (agg_context && !Z_ISUNDEF(agg_context->zval_context)) {
			zval_ptr_dtor(&agg_context->zval_context);
		}
		ZVAL_COPY_VALUE(&agg_context->zval_context, &retval);
		ZVAL_UNDEF(&retval);
	}

	if (!Z_ISUNDEF(retval)) {
		zval_ptr_dtor(&retval);
	}
	return ret;
}
/* }}}*/

static void php_sqlite3_callback_func(sqlite3_context *context, int argc, sqlite3_value **argv) /* {{{ */
{
	php_sqlite3_func *func = (php_sqlite3_func *)sqlite3_user_data(context);

	sqlite3_do_callback(&func->afunc, &func->func, argc, argv, context, 0);
}
/* }}}*/

static void php_sqlite3_callback_step(sqlite3_context *context, int argc, sqlite3_value **argv) /* {{{ */
{
	php_sqlite3_func *func = (php_sqlite3_func *)sqlite3_user_data(context);
	php_sqlite3_agg_context *agg_context = (php_sqlite3_agg_context *)sqlite3_aggregate_context(context, sizeof(php_sqlite3_agg_context));

	agg_context->row_count++;

	sqlite3_do_callback(&func->astep, &func->step, argc, argv, context, 1);
}
/* }}} */

static void php_sqlite3_callback_final(sqlite3_context *context) /* {{{ */
{
	php_sqlite3_func *func = (php_sqlite3_func *)sqlite3_user_data(context);
	php_sqlite3_agg_context *agg_context = (php_sqlite3_agg_context *)sqlite3_aggregate_context(context, sizeof(php_sqlite3_agg_context));

	agg_context->row_count = 0;

	sqlite3_do_callback(&func->afini, &func->fini, 0, NULL, context, 1);
}
/* }}} */

static int php_sqlite3_callback_compare(void *coll, int a_len, const void *a, int b_len, const void* b) /* {{{ */
{
	php_sqlite3_collation *collation = (php_sqlite3_collation*)coll;
	zval zargs[2];
	zval retval;
	int ret;

	// Exception occurred on previous callback. Don't attempt to call function.
	if (EG(exception)) {
		return 0;
	}

	collation->fci.fci.size = (sizeof(collation->fci.fci));
	ZVAL_COPY_VALUE(&collation->fci.fci.function_name, &collation->cmp_func);
	collation->fci.fci.object = NULL;
	collation->fci.fci.retval = &retval;
	collation->fci.fci.param_count = 2;

	ZVAL_STRINGL(&zargs[0], a, a_len);
	ZVAL_STRINGL(&zargs[1], b, b_len);

	collation->fci.fci.params = zargs;

	if ((ret = zend_call_function(&collation->fci.fci, &collation->fci.fcc)) == FAILURE) {
		php_error_docref(NULL, E_WARNING, "An error occurred while invoking the compare callback");
	}

	zval_ptr_dtor(&zargs[0]);
	zval_ptr_dtor(&zargs[1]);

	if (EG(exception)) {
		ret = 0;
	} else if (Z_TYPE(retval) != IS_LONG){
		//retval ought to contain a ZVAL_LONG by now
		// (the result of a comparison, i.e. most likely -1, 0, or 1)
		//I suppose we could accept any scalar return type, though.
		php_error_docref(NULL, E_WARNING, "An error occurred while invoking the compare callback (invalid return type).  Collation behaviour is undefined.");
	} else {
		ret = Z_LVAL(retval);
	}

	zval_ptr_dtor(&retval);

	return ret;
}
/* }}} */

/* {{{ proto bool SQLite3::createFunction(string name, mixed callback [, int argcount, int flags])
   Allows registration of a PHP function as a SQLite UDF that can be called within SQL statements. */
PHP_METHOD(sqlite3, createFunction)
{
	php_sqlite3_db_object *db_obj;
	zval *object = ZEND_THIS;
	php_sqlite3_func *func;
	char *sql_func;
	size_t sql_func_len;
	zval *callback_func;
	zend_long sql_func_num_args = -1;
	zend_long flags = 0;
	db_obj = Z_SQLITE3_DB_P(object);

	SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->initialised, SQLite3)

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sz|ll", &sql_func, &sql_func_len, &callback_func, &sql_func_num_args, &flags) == FAILURE) {
		return;
	}

	if (!sql_func_len) {
		RETURN_FALSE;
	}

	if (!zend_is_callable(callback_func, 0, NULL)) {
		zend_string *callback_name = zend_get_callable_name(callback_func);
		php_sqlite3_error(db_obj, "Not a valid callback function %s", ZSTR_VAL(callback_name));
		zend_string_release_ex(callback_name, 0);
		RETURN_FALSE;
	}

	func = (php_sqlite3_func *)ecalloc(1, sizeof(*func));

	if (sqlite3_create_function(db_obj->db, sql_func, sql_func_num_args, flags | SQLITE_UTF8, func, php_sqlite3_callback_func, NULL, NULL) == SQLITE_OK) {
		func->func_name = estrdup(sql_func);

		ZVAL_COPY(&func->func, callback_func);

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
	zval *object = ZEND_THIS;
	php_sqlite3_func *func;
	char *sql_func;
	size_t sql_func_len;
	zval *step_callback, *fini_callback;
	zend_long sql_func_num_args = -1;
	db_obj = Z_SQLITE3_DB_P(object);

	SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->initialised, SQLite3)

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "szz|l", &sql_func, &sql_func_len, &step_callback, &fini_callback, &sql_func_num_args) == FAILURE) {
		return;
	}

	if (!sql_func_len) {
		RETURN_FALSE;
	}

	if (!zend_is_callable(step_callback, 0, NULL)) {
		zend_string *callback_name = zend_get_callable_name(step_callback);
		php_sqlite3_error(db_obj, "Not a valid callback function %s", ZSTR_VAL(callback_name));
		zend_string_release_ex(callback_name, 0);
		RETURN_FALSE;
	}

	if (!zend_is_callable(fini_callback, 0, NULL)) {
		zend_string *callback_name = zend_get_callable_name(fini_callback);
		php_sqlite3_error(db_obj, "Not a valid callback function %s", ZSTR_VAL(callback_name));
		zend_string_release_ex(callback_name, 0);
		RETURN_FALSE;
	}

	func = (php_sqlite3_func *)ecalloc(1, sizeof(*func));

	if (sqlite3_create_function(db_obj->db, sql_func, sql_func_num_args, SQLITE_UTF8, func, NULL, php_sqlite3_callback_step, php_sqlite3_callback_final) == SQLITE_OK) {
		func->func_name = estrdup(sql_func);

		ZVAL_COPY(&func->step, step_callback);
		ZVAL_COPY(&func->fini, fini_callback);

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
	zval *object = ZEND_THIS;
	php_sqlite3_collation *collation;
	char *collation_name;
	size_t collation_name_len;
	zval *callback_func;
	db_obj = Z_SQLITE3_DB_P(object);

	SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->initialised, SQLite3)

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sz", &collation_name, &collation_name_len, &callback_func) == FAILURE) {
		RETURN_FALSE;
	}

	if (!collation_name_len) {
		RETURN_FALSE;
	}

	if (!zend_is_callable(callback_func, 0, NULL)) {
		zend_string *callback_name = zend_get_callable_name(callback_func);
		php_sqlite3_error(db_obj, "Not a valid callback function %s", ZSTR_VAL(callback_name));
		zend_string_release_ex(callback_name, 0);
		RETURN_FALSE;
	}

	collation = (php_sqlite3_collation *)ecalloc(1, sizeof(*collation));
	if (sqlite3_create_collation(db_obj->db, collation_name, SQLITE_UTF8, collation, php_sqlite3_callback_compare) == SQLITE_OK) {
		collation->collation_name = estrdup(collation_name);

		ZVAL_COPY(&collation->cmp_func, callback_func);

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
	int          flags;
} php_stream_sqlite3_data;

static ssize_t php_sqlite3_stream_write(php_stream *stream, const char *buf, size_t count)
{
	php_stream_sqlite3_data *sqlite3_stream = (php_stream_sqlite3_data *) stream->abstract;

	if (sqlite3_stream->flags & SQLITE_OPEN_READONLY) {
		php_error_docref(NULL, E_WARNING, "Can't write to blob stream: is open as read only");
		return -1;
	}

	if (sqlite3_stream->position + count > sqlite3_stream->size) {
		php_error_docref(NULL, E_WARNING, "It is not possible to increase the size of a BLOB");
		return -1;
	}

	if (sqlite3_blob_write(sqlite3_stream->blob, buf, count, sqlite3_stream->position) != SQLITE_OK) {
		return -1;
	}

	if (sqlite3_stream->position + count >= sqlite3_stream->size) {
		stream->eof = 1;
		sqlite3_stream->position = sqlite3_stream->size;
	}
	else {
		sqlite3_stream->position += count;
	}

	return count;
}

static ssize_t php_sqlite3_stream_read(php_stream *stream, char *buf, size_t count)
{
	php_stream_sqlite3_data *sqlite3_stream = (php_stream_sqlite3_data *) stream->abstract;

	if (sqlite3_stream->position + count >= sqlite3_stream->size) {
		count = sqlite3_stream->size - sqlite3_stream->position;
		stream->eof = 1;
	}
	if (count) {
		if (sqlite3_blob_read(sqlite3_stream->blob, buf, count, sqlite3_stream->position) != SQLITE_OK) {
			return -1;
		}
		sqlite3_stream->position += count;
	}
	return count;
}

static int php_sqlite3_stream_close(php_stream *stream, int close_handle)
{
	php_stream_sqlite3_data *sqlite3_stream = (php_stream_sqlite3_data *) stream->abstract;

	if (sqlite3_blob_close(sqlite3_stream->blob) != SQLITE_OK) {
		/* Error occurred, but it still closed */
	}

	efree(sqlite3_stream);

	return 0;
}

static int php_sqlite3_stream_flush(php_stream *stream)
{
	/* do nothing */
	return 0;
}

/* {{{ */
static int php_sqlite3_stream_seek(php_stream *stream, zend_off_t offset, int whence, zend_off_t *newoffs)
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


static int php_sqlite3_stream_cast(php_stream *stream, int castas, void **ret)
{
	return FAILURE;
}

static int php_sqlite3_stream_stat(php_stream *stream, php_stream_statbuf *ssb)
{
	php_stream_sqlite3_data *sqlite3_stream = (php_stream_sqlite3_data *) stream->abstract;
	ssb->sb.st_size = sqlite3_stream->size;
	return 0;
}

static const php_stream_ops php_stream_sqlite3_ops = {
	php_sqlite3_stream_write,
	php_sqlite3_stream_read,
	php_sqlite3_stream_close,
	php_sqlite3_stream_flush,
	"SQLite3",
	php_sqlite3_stream_seek,
	php_sqlite3_stream_cast,
	php_sqlite3_stream_stat,
	NULL
};

/* {{{ proto resource SQLite3::openBlob(string table, string column, int rowid [, string dbname [, int flags]])
   Open a blob as a stream which we can read / write to. */
PHP_METHOD(sqlite3, openBlob)
{
	php_sqlite3_db_object *db_obj;
	zval *object = ZEND_THIS;
	char *table, *column, *dbname = "main", *mode = "rb";
	size_t table_len, column_len, dbname_len;
	zend_long rowid, flags = SQLITE_OPEN_READONLY, sqlite_flags = 0;
	sqlite3_blob *blob = NULL;
	php_stream_sqlite3_data *sqlite3_stream;
	php_stream *stream;

	db_obj = Z_SQLITE3_DB_P(object);

	SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->initialised, SQLite3)

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ssl|sl", &table, &table_len, &column, &column_len, &rowid, &dbname, &dbname_len, &flags) == FAILURE) {
		return;
	}

	sqlite_flags = (flags & SQLITE_OPEN_READWRITE) ? 1 : 0;

	if (sqlite3_blob_open(db_obj->db, dbname, table, column, rowid, sqlite_flags, &blob) != SQLITE_OK) {
		php_sqlite3_error(db_obj, "Unable to open blob: %s", sqlite3_errmsg(db_obj->db));
		RETURN_FALSE;
	}

	sqlite3_stream = emalloc(sizeof(php_stream_sqlite3_data));
	sqlite3_stream->blob = blob;
	sqlite3_stream->flags = flags;
	sqlite3_stream->position = 0;
	sqlite3_stream->size = sqlite3_blob_bytes(blob);

	if (sqlite_flags != 0) {
		mode = "r+b";
	}

	stream = php_stream_alloc(&php_stream_sqlite3_ops, sqlite3_stream, 0, mode);

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
	zval *object = ZEND_THIS;
	zend_bool enableExceptions = 0;

	db_obj = Z_SQLITE3_DB_P(object);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &enableExceptions) == FAILURE) {
		return;
	}

	RETVAL_BOOL(db_obj->exception);

	db_obj->exception = enableExceptions;
}
/* }}} */

#if SQLITE_VERSION_NUMBER >= 3006011
/* {{{ proto bool SQLite3::backup(SQLite3 destination_db[, string source_dbname = "main"[, string destination_dbname = "main"]])
   Backups the current database to another one. */
PHP_METHOD(sqlite3, backup)
{
	php_sqlite3_db_object *source_obj;
	php_sqlite3_db_object *destination_obj;
	char *source_dbname = "main", *destination_dbname = "main";
	size_t source_dbname_length, destination_dbname_length;
	zval *source_zval = ZEND_THIS;
	zval *destination_zval;
	sqlite3_backup *dbBackup;
	int rc; // Return code

	source_obj = Z_SQLITE3_DB_P(source_zval);
	SQLITE3_CHECK_INITIALIZED(source_obj, source_obj->initialised, SQLite3)

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|ss", &destination_zval, php_sqlite3_sc_entry, &source_dbname, &source_dbname_length, &destination_dbname, &destination_dbname_length) == FAILURE) {
		return;
	}

	destination_obj = Z_SQLITE3_DB_P(destination_zval);

	SQLITE3_CHECK_INITIALIZED(destination_obj, destination_obj->initialised, SQLite3)

	dbBackup = sqlite3_backup_init(destination_obj->db, destination_dbname, source_obj->db, source_dbname);

	if (dbBackup) {
		do {
			rc = sqlite3_backup_step(dbBackup, -1);
		} while (rc == SQLITE_OK);

		/* Release resources allocated by backup_init(). */
		rc = sqlite3_backup_finish(dbBackup);
	}
	else {
		rc = sqlite3_errcode(source_obj->db);
	}

	if (rc != SQLITE_OK) {
		if (rc == SQLITE_BUSY) {
			php_sqlite3_error(source_obj, "Backup failed: source database is busy");
		}
		else if (rc == SQLITE_LOCKED) {
			php_sqlite3_error(source_obj, "Backup failed: source database is locked");
		}
		else {
			php_sqlite3_error(source_obj, "Backup failed: %d, %s", rc, sqlite3_errmsg(source_obj->db));
		}
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ proto int SQLite3Stmt::paramCount()
   Returns the number of parameters within the prepared statement. */
PHP_METHOD(sqlite3stmt, paramCount)
{
	php_sqlite3_stmt *stmt_obj;
	zval *object = ZEND_THIS;
	stmt_obj = Z_SQLITE3_STMT_P(object);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	SQLITE3_CHECK_INITIALIZED(stmt_obj->db_obj, stmt_obj->initialised, SQLite3);
	SQLITE3_CHECK_INITIALIZED_STMT(stmt_obj->stmt, SQLite3Stmt);

	RETURN_LONG(sqlite3_bind_parameter_count(stmt_obj->stmt));
}
/* }}} */

/* {{{ proto bool SQLite3Stmt::close()
   Closes the prepared statement. */
PHP_METHOD(sqlite3stmt, close)
{
	php_sqlite3_stmt *stmt_obj;
	zval *object = ZEND_THIS;
	stmt_obj = Z_SQLITE3_STMT_P(object);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	SQLITE3_CHECK_INITIALIZED(stmt_obj->db_obj, stmt_obj->initialised, SQLite3);

	if(stmt_obj->db_obj) {
        	zend_llist_del_element(&(stmt_obj->db_obj->free_list), object, (int (*)(void *, void *)) php_sqlite3_compare_stmt_zval_free);
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool SQLite3Stmt::reset()
   Reset the prepared statement to the state before it was executed, bindings still remain. */
PHP_METHOD(sqlite3stmt, reset)
{
	php_sqlite3_stmt *stmt_obj;
	zval *object = ZEND_THIS;
	stmt_obj = Z_SQLITE3_STMT_P(object);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	SQLITE3_CHECK_INITIALIZED(stmt_obj->db_obj, stmt_obj->initialised, SQLite3);
	SQLITE3_CHECK_INITIALIZED_STMT(stmt_obj->stmt, SQLite3Stmt);

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
	zval *object = ZEND_THIS;
	stmt_obj = Z_SQLITE3_STMT_P(object);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	SQLITE3_CHECK_INITIALIZED(stmt_obj->db_obj, stmt_obj->initialised, SQLite3);
	SQLITE3_CHECK_INITIALIZED_STMT(stmt_obj->stmt, SQLite3Stmt);

	if (sqlite3_clear_bindings(stmt_obj->stmt) != SQLITE_OK) {
		php_sqlite3_error(stmt_obj->db_obj, "Unable to clear statement: %s", sqlite3_errmsg(sqlite3_db_handle(stmt_obj->stmt)));
		RETURN_FALSE;
	}

	if (stmt_obj->bound_params) {
		zend_hash_destroy(stmt_obj->bound_params);
		FREE_HASHTABLE(stmt_obj->bound_params);
		stmt_obj->bound_params = NULL;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool SQLite3Stmt::readOnly()
   Returns true if a statement is definitely read only */
PHP_METHOD(sqlite3stmt, readOnly)
{
	php_sqlite3_stmt *stmt_obj;
	zval *object = ZEND_THIS;
	stmt_obj = Z_SQLITE3_STMT_P(object);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	SQLITE3_CHECK_INITIALIZED(stmt_obj->db_obj, stmt_obj->initialised, SQLite3);
	SQLITE3_CHECK_INITIALIZED_STMT(stmt_obj->stmt, SQLite3Stmt);

	if (sqlite3_stmt_readonly(stmt_obj->stmt)) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* bind parameters to a statement before execution */
static int php_sqlite3_bind_params(php_sqlite3_stmt *stmt_obj) /* {{{ */
{
	struct php_sqlite3_bound_param *param;
	int return_code;

	if (stmt_obj->bound_params) {
		ZEND_HASH_FOREACH_PTR(stmt_obj->bound_params, param) {
			zval *parameter;
			/* parameter must be a reference? */
			if (Z_ISREF(param->parameter)) {
				parameter = Z_REFVAL(param->parameter);
			} else {
				parameter = &param->parameter;
			}

			/* If the ZVAL is null then it should be bound as that */
			if (Z_TYPE_P(parameter) == IS_NULL) {
				return_code = sqlite3_bind_null(stmt_obj->stmt, param->param_number);
				if (return_code != SQLITE_OK) {
					php_sqlite3_error(stmt_obj->db_obj, "Unable to bind parameter number " ZEND_LONG_FMT " (%d)", param->param_number, return_code);
				}
				continue;
			}

			switch (param->type) {
				case SQLITE_INTEGER:
					convert_to_long(parameter);
#if ZEND_LONG_MAX > 2147483647
					return_code = sqlite3_bind_int64(stmt_obj->stmt, param->param_number, Z_LVAL_P(parameter));
#else
					return_code = sqlite3_bind_int(stmt_obj->stmt, param->param_number, Z_LVAL_P(parameter));
#endif
					if (return_code != SQLITE_OK) {
						php_sqlite3_error(stmt_obj->db_obj, "Unable to bind parameter number " ZEND_LONG_FMT " (%d)", param->param_number, return_code);
					}
					break;

				case SQLITE_FLOAT:
					convert_to_double(parameter);
					return_code = sqlite3_bind_double(stmt_obj->stmt, param->param_number, Z_DVAL_P(parameter));
					if (return_code != SQLITE_OK) {
						php_sqlite3_error(stmt_obj->db_obj, "Unable to bind parameter number " ZEND_LONG_FMT " (%d)", param->param_number, return_code);
					}
					break;

				case SQLITE_BLOB:
				{
					php_stream *stream = NULL;
					zend_string *buffer = NULL;
					if (Z_TYPE_P(parameter) == IS_RESOURCE) {
						php_stream_from_zval_no_verify(stream, parameter);
						if (stream == NULL) {
							php_sqlite3_error(stmt_obj->db_obj, "Unable to read stream for parameter %ld", param->param_number);
							return FAILURE;
						}
						buffer = php_stream_copy_to_mem(stream, PHP_STREAM_COPY_ALL, 0);
					} else {
						buffer = zval_get_string(parameter);
					}

					if (buffer) {
						return_code = sqlite3_bind_blob(stmt_obj->stmt, param->param_number, ZSTR_VAL(buffer), ZSTR_LEN(buffer), SQLITE_TRANSIENT);
						zend_string_release_ex(buffer, 0);
						if (return_code != SQLITE_OK) {
							php_sqlite3_error(stmt_obj->db_obj, "Unable to bind parameter number " ZEND_LONG_FMT " (%d)", param->param_number, return_code);
						}
					} else {
						return_code = sqlite3_bind_null(stmt_obj->stmt, param->param_number);
						if (return_code != SQLITE_OK) {
							php_sqlite3_error(stmt_obj->db_obj, "Unable to bind parameter number " ZEND_LONG_FMT " (%d)", param->param_number, return_code);
						}
					}
					break;
				}

				case SQLITE3_TEXT: {
					zend_string *str = zval_try_get_string(parameter);
					if (UNEXPECTED(!str)) {
						return FAILURE;
					}
					return_code = sqlite3_bind_text(stmt_obj->stmt, param->param_number, ZSTR_VAL(str), ZSTR_LEN(str), SQLITE_TRANSIENT);
					if (return_code != SQLITE_OK) {
						php_sqlite3_error(stmt_obj->db_obj, "Unable to bind parameter number " ZEND_LONG_FMT " (%d)", param->param_number, return_code);
					}
					zend_string_release(str);
					break;
				}

				case SQLITE_NULL:
					return_code = sqlite3_bind_null(stmt_obj->stmt, param->param_number);
					if (return_code != SQLITE_OK) {
						php_sqlite3_error(stmt_obj->db_obj, "Unable to bind parameter number " ZEND_LONG_FMT " (%d)", param->param_number, return_code);
					}
					break;

				default:
					php_sqlite3_error(stmt_obj->db_obj, "Unknown parameter type: %pd for parameter %pd", param->type, param->param_number);
					return FAILURE;
			}
		} ZEND_HASH_FOREACH_END();
	}

	return SUCCESS;
}
/* }}} */


/* {{{ proto string SQLite3Stmt::getSQL([expanded = false])
   Returns the SQL statement used to prepare the query. If expanded is true, binded parameters and values will be expanded. */
PHP_METHOD(sqlite3stmt, getSQL)
{
	php_sqlite3_stmt *stmt_obj;
	zend_bool expanded = 0;
	zval *object = getThis();
	stmt_obj = Z_SQLITE3_STMT_P(object);
	int bind_rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &expanded) == FAILURE) {
		return;
	}

	SQLITE3_CHECK_INITIALIZED(stmt_obj->db_obj, stmt_obj->initialised, SQLite3);
	SQLITE3_CHECK_INITIALIZED_STMT(stmt_obj->stmt, SQLite3Stmt);

	bind_rc = php_sqlite3_bind_params(stmt_obj);

	if (bind_rc == FAILURE || EG(exception)) {
		RETURN_FALSE;
	}

	if (expanded) {
#if SQLITE_VERSION_NUMBER >= 3014000
		char *sql = sqlite3_expanded_sql(stmt_obj->stmt);
		RETVAL_STRING(sql);
		sqlite3_free(sql);
#else
		php_sqlite3_error(stmt_obj->db_obj, "The expanded parameter requires SQLite3 >= 3.14 and %s is installed", sqlite3_libversion());
		RETURN_FALSE;
#endif
	} else {
		const char *sql = sqlite3_sql(stmt_obj->stmt);
		RETVAL_STRING(sql);
	}
}
/* }}} */


static int register_bound_parameter_to_sqlite(struct php_sqlite3_bound_param *param, php_sqlite3_stmt *stmt) /* {{{ */
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
		if (ZSTR_VAL(param->name)[0] != ':' && ZSTR_VAL(param->name)[0] != '@') {
			/* pre-increment for character + 1 for null */
			zend_string *temp = zend_string_alloc(ZSTR_LEN(param->name) + 1, 0);
			ZSTR_VAL(temp)[0] = ':';
			memmove(ZSTR_VAL(temp) + 1, ZSTR_VAL(param->name), ZSTR_LEN(param->name) + 1);
			param->name = temp;
		} else {
			param->name = zend_string_copy(param->name);
		}
		/* do lookup*/
		param->param_number = sqlite3_bind_parameter_index(stmt->stmt, ZSTR_VAL(param->name));
	}

	if (param->param_number < 1) {
		if (param->name) {
			zend_string_release_ex(param->name, 0);
		}
		return 0;
	}

	if (param->param_number >= 1) {
		zend_hash_index_del(hash, param->param_number);
	}

	if (param->name) {
		zend_hash_update_mem(hash, param->name, param, sizeof(struct php_sqlite3_bound_param));
	} else {
		zend_hash_index_update_mem(hash, param->param_number, param, sizeof(struct php_sqlite3_bound_param));
	}

	return 1;
}
/* }}} */

/* {{{ Best try to map between PHP and SQLite. Default is still text. */
#define PHP_SQLITE3_SET_TYPE(z, p) \
	switch (Z_TYPE_P(z)) { \
		default: \
			(p).type = SQLITE_TEXT; \
			break; \
		case IS_LONG: \
		case IS_TRUE: \
		case IS_FALSE: \
			(p).type = SQLITE_INTEGER; \
			break; \
		case IS_DOUBLE: \
			(p).type = SQLITE_FLOAT; \
			break; \
		case IS_NULL: \
			(p).type = SQLITE_NULL; \
			break; \
	}
/* }}} */

/* {{{ proto bool SQLite3Stmt::bindParam(int parameter_number, mixed parameter [, int type])
   Bind Parameter to a stmt variable. */
PHP_METHOD(sqlite3stmt, bindParam)
{
	php_sqlite3_stmt *stmt_obj;
	zval *object = ZEND_THIS;
	struct php_sqlite3_bound_param param = {0};
	zval *parameter;
	stmt_obj = Z_SQLITE3_STMT_P(object);

	param.param_number = -1;
	param.type = SQLITE3_TEXT;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "lz|l", &param.param_number, &parameter, &param.type) == FAILURE) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz|l", &param.name, &parameter, &param.type) == FAILURE) {
			return;
		}
	}

	SQLITE3_CHECK_INITIALIZED(stmt_obj->db_obj, stmt_obj->initialised, SQLite3);
	SQLITE3_CHECK_INITIALIZED_STMT(stmt_obj->stmt, SQLite3Stmt);

	ZVAL_COPY(&param.parameter, parameter);

	if (ZEND_NUM_ARGS() < 3) {
		PHP_SQLITE3_SET_TYPE(parameter, param);
	}

	if (!register_bound_parameter_to_sqlite(&param, stmt_obj)) {
		if (!Z_ISUNDEF(param.parameter)) {
			zval_ptr_dtor(&(param.parameter));
			ZVAL_UNDEF(&param.parameter);
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
	zval *object = ZEND_THIS;
	struct php_sqlite3_bound_param param = {0};
	zval *parameter;
	stmt_obj = Z_SQLITE3_STMT_P(object);

	param.param_number = -1;
	param.type = SQLITE3_TEXT;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "lz|l", &param.param_number, &parameter, &param.type) == FAILURE) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz|l", &param.name, &parameter, &param.type) == FAILURE) {
			return;
		}
	}

	SQLITE3_CHECK_INITIALIZED(stmt_obj->db_obj, stmt_obj->initialised, SQLite3);
	SQLITE3_CHECK_INITIALIZED_STMT(stmt_obj->stmt, SQLite3Stmt);

	ZVAL_COPY(&param.parameter, parameter);

	if (ZEND_NUM_ARGS() < 3) {
		PHP_SQLITE3_SET_TYPE(parameter, param);
	}

	if (!register_bound_parameter_to_sqlite(&param, stmt_obj)) {
		if (!Z_ISUNDEF(param.parameter)) {
			zval_ptr_dtor(&(param.parameter));
			ZVAL_UNDEF(&param.parameter);
		}
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

#undef PHP_SQLITE3_SET_TYPE

/* {{{ proto SQLite3Result SQLite3Stmt::execute()
   Executes a prepared statement and returns a result set object. */
PHP_METHOD(sqlite3stmt, execute)
{
	php_sqlite3_stmt *stmt_obj;
	php_sqlite3_result *result;
	zval *object = ZEND_THIS;
	int return_code = 0;
	int bind_rc = 0;

	stmt_obj = Z_SQLITE3_STMT_P(object);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	SQLITE3_CHECK_INITIALIZED(stmt_obj->db_obj, stmt_obj->initialised, SQLite3);

	/* Always reset statement before execution, see bug #77051 */
	sqlite3_reset(stmt_obj->stmt);

	/* Bind parameters to the statement */
	bind_rc = php_sqlite3_bind_params(stmt_obj);

	if (bind_rc == FAILURE || EG(exception)) {
		RETURN_FALSE;
	}

	return_code = sqlite3_step(stmt_obj->stmt);

	switch (return_code) {
		case SQLITE_ROW: /* Valid Row */
		case SQLITE_DONE: /* Valid but no results */
		{
			sqlite3_reset(stmt_obj->stmt);
			object_init_ex(return_value, php_sqlite3_result_entry);
			result = Z_SQLITE3_RESULT_P(return_value);

			result->is_prepared_statement = 1;
			result->db_obj = stmt_obj->db_obj;
			result->stmt_obj = stmt_obj;
			Z_ADDREF_P(object);
			ZVAL_OBJ(&result->stmt_obj_zval, Z_OBJ_P(object));

			break;
		}
		case SQLITE_ERROR:
			sqlite3_reset(stmt_obj->stmt);

		default:
			if (!EG(exception)) {
				php_sqlite3_error(stmt_obj->db_obj, "Unable to execute statement: %s", sqlite3_errmsg(sqlite3_db_handle(stmt_obj->stmt)));
			}
			zval_ptr_dtor(return_value);
			RETURN_FALSE;
	}

	return;
}
/* }}} */

/* {{{ proto SQLite3Stmt::__construct(SQLite3 dbobject, String Statement)
   __constructor for SQLite3Stmt. */
PHP_METHOD(sqlite3stmt, __construct)
{
	php_sqlite3_stmt *stmt_obj;
	php_sqlite3_db_object *db_obj;
	zval *object = ZEND_THIS;
	zval *db_zval;
	zend_string *sql;
	int errcode;
	zend_error_handling error_handling;
	php_sqlite3_free_list *free_item;

	stmt_obj = Z_SQLITE3_STMT_P(object);

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "OS", &db_zval, php_sqlite3_sc_entry, &sql) == FAILURE) {
		return;
	}

	db_obj = Z_SQLITE3_DB_P(db_zval);

	zend_replace_error_handling(EH_THROW, NULL, &error_handling);
	SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->initialised, SQLite3)
	zend_restore_error_handling(&error_handling);

	if (!ZSTR_LEN(sql)) {
		RETURN_FALSE;
	}

	stmt_obj->db_obj = db_obj;
	Z_ADDREF_P(db_zval);
	ZVAL_OBJ(&stmt_obj->db_obj_zval, Z_OBJ_P(db_zval));

	errcode = sqlite3_prepare_v2(db_obj->db, ZSTR_VAL(sql), ZSTR_LEN(sql), &(stmt_obj->stmt), NULL);
	if (errcode != SQLITE_OK) {
		php_sqlite3_error(db_obj, "Unable to prepare statement: %d, %s", errcode, sqlite3_errmsg(db_obj->db));
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}
	stmt_obj->initialised = 1;

	free_item = emalloc(sizeof(php_sqlite3_free_list));
	free_item->stmt_obj = stmt_obj;
	//??  free_item->stmt_obj_zval = ZEND_THIS;
	ZVAL_OBJ(&free_item->stmt_obj_zval, Z_OBJ_P(object));

	zend_llist_add_element(&(db_obj->free_list), &free_item);
}
/* }}} */

/* {{{ proto int SQLite3Result::numColumns()
   Number of columns in the result set. */
PHP_METHOD(sqlite3result, numColumns)
{
	php_sqlite3_result *result_obj;
	zval *object = ZEND_THIS;
	result_obj = Z_SQLITE3_RESULT_P(object);

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
	zval *object = ZEND_THIS;
	zend_long column = 0;
	char *column_name;
	result_obj = Z_SQLITE3_RESULT_P(object);

	SQLITE3_CHECK_INITIALIZED(result_obj->db_obj, result_obj->stmt_obj->initialised, SQLite3Result)

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &column) == FAILURE) {
		return;
	}
	column_name = (char*) sqlite3_column_name(result_obj->stmt_obj->stmt, column);

	if (column_name == NULL) {
		RETURN_FALSE;
	}

	RETVAL_STRING(column_name);
}
/* }}} */

/* {{{ proto int SQLite3Result::columnType(int column)
   Returns the type of the nth column. */
PHP_METHOD(sqlite3result, columnType)
{
	php_sqlite3_result *result_obj;
	zval *object = ZEND_THIS;
	zend_long column = 0;
	result_obj = Z_SQLITE3_RESULT_P(object);

	SQLITE3_CHECK_INITIALIZED(result_obj->db_obj, result_obj->stmt_obj->initialised, SQLite3Result)

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &column) == FAILURE) {
		return;
	}

	if (!sqlite3_data_count(result_obj->stmt_obj->stmt)) {
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
	zval *object = ZEND_THIS;
	int i, ret;
	zend_long mode = PHP_SQLITE3_BOTH;
	result_obj = Z_SQLITE3_RESULT_P(object);

	SQLITE3_CHECK_INITIALIZED(result_obj->db_obj, result_obj->stmt_obj->initialised, SQLite3Result)

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &mode) == FAILURE) {
		return;
	}

	ret = sqlite3_step(result_obj->stmt_obj->stmt);
	switch (ret) {
		case SQLITE_ROW:
			/* If there was no return value then just skip fetching */
			if (!USED_RET()) {
				return;
			}

			array_init(return_value);

			for (i = 0; i < sqlite3_data_count(result_obj->stmt_obj->stmt); i++) {
				zval data;

				sqlite_value_to_zval(result_obj->stmt_obj->stmt, i, &data);

				if (mode & PHP_SQLITE3_NUM) {
					add_index_zval(return_value, i, &data);
				}

				if (mode & PHP_SQLITE3_ASSOC) {
					if (mode & PHP_SQLITE3_NUM) {
						if (Z_REFCOUNTED(data)) {
							Z_ADDREF(data);
						}
					}
					add_assoc_zval(return_value, (char*)sqlite3_column_name(result_obj->stmt_obj->stmt, i), &data);
				}
			}
			break;

		case SQLITE_DONE:
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
	zval *object = ZEND_THIS;
	result_obj = Z_SQLITE3_RESULT_P(object);

	SQLITE3_CHECK_INITIALIZED(result_obj->db_obj, result_obj->stmt_obj->initialised, SQLite3Result)

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (sqlite3_reset(result_obj->stmt_obj->stmt) != SQLITE_OK) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool SQLite3Result::finalize()
   Closes the result set. */
PHP_METHOD(sqlite3result, finalize)
{
	php_sqlite3_result *result_obj;
	zval *object = ZEND_THIS;
	result_obj = Z_SQLITE3_RESULT_P(object);

	SQLITE3_CHECK_INITIALIZED(result_obj->db_obj, result_obj->stmt_obj->initialised, SQLite3Result)

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	/* We need to finalize an internal statement */
	if (result_obj->is_prepared_statement == 0) {
		zend_llist_del_element(&(result_obj->db_obj->free_list), &result_obj->stmt_obj_zval,
			(int (*)(void *, void *)) php_sqlite3_compare_stmt_zval_free);
	} else {
		sqlite3_reset(result_obj->stmt_obj->stmt);
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto SQLite3Result::__construct()
   __constructor for SQLite3Result. */
PHP_METHOD(sqlite3result, __construct)
{
	zend_throw_exception(zend_ce_exception, "SQLite3Result cannot be directly instantiated", 0);
}
/* }}} */

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3_open, 0, 0, 1)
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
	ZEND_ARG_INFO(0, flags)
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3_openblob, 0, 0, 3)
	ZEND_ARG_INFO(0, table)
	ZEND_ARG_INFO(0, column)
	ZEND_ARG_INFO(0, rowid)
	ZEND_ARG_INFO(0, dbname)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3_enableexceptions, 0, 0, 0)
	ZEND_ARG_INFO(0, enableExceptions)
ZEND_END_ARG_INFO()

#if SQLITE_VERSION_NUMBER >= 3006011
ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3_backup, 0, 0, 1)
	ZEND_ARG_INFO(0, destination_db)
	ZEND_ARG_INFO(0, source_dbname)
	ZEND_ARG_INFO(0, destination_dbname)
ZEND_END_ARG_INFO()
#endif

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

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3stmt_construct, 0, 0, 1)
	ZEND_ARG_INFO(0, sqlite3)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3stmt_getsql, 0, 0, 0)
	ZEND_ARG_INFO(0, expanded)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3result_columnname, 0, 0, 1)
	ZEND_ARG_INFO(0, column_number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3result_columntype, 0, 0, 1)
	ZEND_ARG_INFO(0, column_number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3result_fetcharray, 0, 0, 0)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sqlite3_enableextended, 0, 0, 1)
	ZEND_ARG_INFO(0, enable)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_sqlite3_void, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ php_sqlite3_class_methods */
static const zend_function_entry php_sqlite3_class_methods[] = {
	PHP_ME(sqlite3,		open,						arginfo_sqlite3_open, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		close,						arginfo_sqlite3_void, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		exec,						arginfo_sqlite3_query, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		version,					arginfo_sqlite3_void, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(sqlite3,		lastInsertRowID,			arginfo_sqlite3_void, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		lastErrorCode,				arginfo_sqlite3_void, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		lastExtendedErrorCode,		arginfo_sqlite3_void, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		enableExtendedResultCodes,	arginfo_sqlite3_enableextended, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		lastErrorMsg,				arginfo_sqlite3_void, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		busyTimeout,				arginfo_sqlite3_busytimeout, ZEND_ACC_PUBLIC)
#ifndef SQLITE_OMIT_LOAD_EXTENSION
	PHP_ME(sqlite3,		loadExtension,				arginfo_sqlite3_loadextension, ZEND_ACC_PUBLIC)
#endif
	PHP_ME(sqlite3,		changes,					arginfo_sqlite3_void, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		escapeString,				arginfo_sqlite3_escapestring, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(sqlite3,		prepare,					arginfo_sqlite3_query, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		query,						arginfo_sqlite3_query, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		querySingle,				arginfo_sqlite3_querysingle, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		createFunction,				arginfo_sqlite3_createfunction, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		createAggregate,			arginfo_sqlite3_createaggregate, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		createCollation,			arginfo_sqlite3_createcollation, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		openBlob,					arginfo_sqlite3_openblob, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3,		enableExceptions,			arginfo_sqlite3_enableexceptions, ZEND_ACC_PUBLIC)
#if SQLITE_VERSION_NUMBER >= 3006011
	PHP_ME(sqlite3,		backup,						arginfo_sqlite3_backup, ZEND_ACC_PUBLIC)
#endif
	/* Aliases */
	PHP_MALIAS(sqlite3,	__construct, open, arginfo_sqlite3_open, ZEND_ACC_PUBLIC)
	PHP_FE_END
};
/* }}} */

/* {{{ php_sqlite3_stmt_class_methods */
static const zend_function_entry php_sqlite3_stmt_class_methods[] = {
	PHP_ME(sqlite3stmt, paramCount,	arginfo_sqlite3_void, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3stmt, close,		arginfo_sqlite3_void, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3stmt, reset,		arginfo_sqlite3_void, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3stmt, clear,		arginfo_sqlite3_void, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3stmt, execute,	arginfo_sqlite3_void, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3stmt, bindParam,	arginfo_sqlite3stmt_bindparam, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3stmt, bindValue,	arginfo_sqlite3stmt_bindvalue, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3stmt, readOnly,	arginfo_sqlite3_void, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3stmt, getSQL,		arginfo_sqlite3stmt_getsql, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3stmt, __construct, arginfo_sqlite3stmt_construct, ZEND_ACC_PRIVATE)
	PHP_FE_END
};
/* }}} */

/* {{{ php_sqlite3_result_class_methods */
static const zend_function_entry php_sqlite3_result_class_methods[] = {
	PHP_ME(sqlite3result, numColumns,		arginfo_sqlite3_void, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3result, columnName,		arginfo_sqlite3result_columnname, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3result, columnType,		arginfo_sqlite3result_columntype, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3result, fetchArray,		arginfo_sqlite3result_fetcharray, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3result, reset,			arginfo_sqlite3_void, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3result, finalize,			arginfo_sqlite3_void, ZEND_ACC_PUBLIC)
	PHP_ME(sqlite3result, __construct, 		arginfo_sqlite3_void, ZEND_ACC_PRIVATE)
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
				if (strncmp(arg3, "file:", 5) == 0) {
					/* starts with "file:" */
					if (!arg3[5]) {
						return SQLITE_DENY;
					}
					if (php_check_open_basedir(arg3 + 5)) {
						return SQLITE_DENY;
					}
				}
				if (php_check_open_basedir(arg3)) {
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

static int php_sqlite3_compare_stmt_zval_free(php_sqlite3_free_list **free_list, zval *statement ) /* {{{ */
{
	return  ((*free_list)->stmt_obj->initialised && Z_PTR_P(statement) == Z_PTR((*free_list)->stmt_obj_zval));
}
/* }}} */

static int php_sqlite3_compare_stmt_free( php_sqlite3_free_list **free_list, sqlite3_stmt *statement ) /* {{{ */
{
	return ((*free_list)->stmt_obj->initialised && statement == (*free_list)->stmt_obj->stmt);
}
/* }}} */

static void php_sqlite3_object_free_storage(zend_object *object) /* {{{ */
{
	php_sqlite3_db_object *intern = php_sqlite3_db_from_obj(object);
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

	while (intern->collations){
		collation = intern->collations;
		intern->collations = collation->next;
		if (intern->initialised && intern->db){
			sqlite3_create_collation(intern->db, collation->collation_name, SQLITE_UTF8, NULL, NULL);
		}
		efree((char*)collation->collation_name);
		if (!Z_ISUNDEF(collation->cmp_func)) {
			zval_ptr_dtor(&collation->cmp_func);
		}
		efree(collation);
	}

	if (intern->initialised && intern->db) {
		sqlite3_close(intern->db);
		intern->initialised = 0;
	}

	zend_object_std_dtor(&intern->zo);
}
/* }}} */

static void php_sqlite3_stmt_object_free_storage(zend_object *object) /* {{{ */
{
	php_sqlite3_stmt *intern = php_sqlite3_stmt_from_obj(object);

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

	if (!Z_ISUNDEF(intern->db_obj_zval)) {
		zval_ptr_dtor(&intern->db_obj_zval);
	}

	zend_object_std_dtor(&intern->zo);
}
/* }}} */

static void php_sqlite3_result_object_free_storage(zend_object *object) /* {{{ */
{
	php_sqlite3_result *intern = php_sqlite3_result_from_obj(object);

	if (!intern) {
		return;
	}

	if (!Z_ISNULL(intern->stmt_obj_zval)) {
		if (intern->stmt_obj && intern->stmt_obj->initialised) {
			sqlite3_reset(intern->stmt_obj->stmt);
		}

		zval_ptr_dtor(&intern->stmt_obj_zval);
	}

	zend_object_std_dtor(&intern->zo);
}
/* }}} */

static zend_object *php_sqlite3_object_new(zend_class_entry *class_type) /* {{{ */
{
	php_sqlite3_db_object *intern;

	/* Allocate memory for it */
	intern = zend_object_alloc(sizeof(php_sqlite3_db_object), class_type);

	/* Need to keep track of things to free */
	zend_llist_init(&(intern->free_list),  sizeof(php_sqlite3_free_list *), (llist_dtor_func_t)php_sqlite3_free_list_dtor, 0);

	zend_object_std_init(&intern->zo, class_type);
	object_properties_init(&intern->zo, class_type);

	intern->zo.handlers = &sqlite3_object_handlers;

	return &intern->zo;
}
/* }}} */

static zend_object *php_sqlite3_stmt_object_new(zend_class_entry *class_type) /* {{{ */
{
	php_sqlite3_stmt *intern;

	/* Allocate memory for it */
	intern = zend_object_alloc(sizeof(php_sqlite3_stmt), class_type);

	zend_object_std_init(&intern->zo, class_type);
	object_properties_init(&intern->zo, class_type);

	intern->zo.handlers = &sqlite3_stmt_object_handlers;

	return &intern->zo;
}
/* }}} */

static zend_object *php_sqlite3_result_object_new(zend_class_entry *class_type) /* {{{ */
{
	php_sqlite3_result *intern;

	/* Allocate memory for it */
	intern = zend_object_alloc(sizeof(php_sqlite3_result), class_type);

	zend_object_std_init(&intern->zo, class_type);
	object_properties_init(&intern->zo, class_type);

	intern->zo.handlers = &sqlite3_result_object_handlers;

	return &intern->zo;
}
/* }}} */

static void sqlite3_param_dtor(zval *data) /* {{{ */
{
	struct php_sqlite3_bound_param *param = (struct php_sqlite3_bound_param*)Z_PTR_P(data);

	if (param->name) {
		zend_string_release_ex(param->name, 0);
	}

	if (!Z_ISNULL(param->parameter)) {
		zval_ptr_dtor(&(param->parameter));
		ZVAL_UNDEF(&param->parameter);
	}
	efree(param);
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
		php_error_docref(NULL, E_WARNING, "A thread safe version of SQLite is required when using a thread safe version of PHP.");
		return FAILURE;
	}
#endif

	memcpy(&sqlite3_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	memcpy(&sqlite3_stmt_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	memcpy(&sqlite3_result_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));

	/* Register SQLite 3 Class */
	INIT_CLASS_ENTRY(ce, "SQLite3", php_sqlite3_class_methods);
	ce.create_object = php_sqlite3_object_new;
	sqlite3_object_handlers.offset = XtOffsetOf(php_sqlite3_db_object, zo);
	sqlite3_object_handlers.clone_obj = NULL;
	sqlite3_object_handlers.free_obj = php_sqlite3_object_free_storage;
	php_sqlite3_sc_entry = zend_register_internal_class(&ce);
	php_sqlite3_sc_entry->serialize = zend_class_serialize_deny;
	php_sqlite3_sc_entry->unserialize = zend_class_unserialize_deny;

	/* Register SQLite 3 Prepared Statement Class */
	INIT_CLASS_ENTRY(ce, "SQLite3Stmt", php_sqlite3_stmt_class_methods);
	ce.create_object = php_sqlite3_stmt_object_new;
	sqlite3_stmt_object_handlers.offset = XtOffsetOf(php_sqlite3_stmt, zo);
	sqlite3_stmt_object_handlers.clone_obj = NULL;
	sqlite3_stmt_object_handlers.free_obj = php_sqlite3_stmt_object_free_storage;
	php_sqlite3_stmt_entry = zend_register_internal_class(&ce);
	php_sqlite3_stmt_entry->serialize = zend_class_serialize_deny;
	php_sqlite3_stmt_entry->unserialize = zend_class_unserialize_deny;

	/* Register SQLite 3 Result Class */
	INIT_CLASS_ENTRY(ce, "SQLite3Result", php_sqlite3_result_class_methods);
	ce.create_object = php_sqlite3_result_object_new;
	sqlite3_result_object_handlers.offset = XtOffsetOf(php_sqlite3_result, zo);
	sqlite3_result_object_handlers.clone_obj = NULL;
	sqlite3_result_object_handlers.free_obj = php_sqlite3_result_object_free_storage;
	php_sqlite3_result_entry = zend_register_internal_class(&ce);
	php_sqlite3_result_entry->serialize = zend_class_serialize_deny;
	php_sqlite3_result_entry->unserialize = zend_class_unserialize_deny;

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

#ifdef SQLITE_DETERMINISTIC
	REGISTER_LONG_CONSTANT("SQLITE3_DETERMINISTIC", SQLITE_DETERMINISTIC, CONST_CS | CONST_PERSISTENT);
#endif

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
	php_info_print_table_row(2, "SQLite Library", sqlite3_libversion());
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ PHP_GINIT_FUNCTION
*/
static PHP_GINIT_FUNCTION(sqlite3)
{
#if defined(COMPILE_DL_SQLITE3) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
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
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(sqlite3)
#endif
