
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
  | Author: Danack
  +----------------------------------------------------------------------+
*/

/* The PDO Database Handle Class */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_pdo.h"
#include "php_pdo_driver.h"
#include "php_pdo_int.h"
#include "zend_exceptions.h"
#include "zend_object_handlers.h"
#include "zend_hash.h"

#include "../pdo_sqlite/php_pdo_sqlite.h"
#include "../pdo_sqlite/php_pdo_sqlite_int.h"

// TODO - define this in appropriate header, not here.
extern void php_sqlite3_func_callback(sqlite3_context *context, int argc,
       	sqlite3_value **argv);

/* {{{ proto bool PDOSqlite::createFunction(string $function_name, callable $callback, int $num_args = -1, int $flags = 0)
    Creates a function that can be used
*/
PHP_METHOD(PDOSqlite, createFunction)
{
	//copied from sqlite_driver.c

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

#ifndef SQLITE_OMIT_LOAD_EXTENSION
/* {{{ Attempts to load an SQLite extension library. */
PHP_METHOD(PDOSqlite, loadExtension)
{
//	php_sqlite3_db_object *db_obj;
//	zval *object = ZEND_THIS;
	char *extension, *lib_path, *extension_dir, *errtext = NULL;
	char fullpath[MAXPATHLEN];
	size_t extension_len;// , extension_dir_len;
//	db_obj = Z_SQLITE3_DB_P(object);

	pdo_dbh_t *dbh;
	pdo_sqlite_db_handle *db_handle;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "s", &extension, &extension_len)) {
		RETURN_THROWS();
	}

	dbh = Z_PDO_DBH_P(ZEND_THIS);
	PDO_CONSTRUCT_CHECK;

	db_handle = (pdo_sqlite_db_handle *)dbh->driver_data;

	// SQLITE3_CHECK_INITIALIZED(db_obj, db_obj->initialised, SQLite3)

#ifdef ZTS
	if ((strncmp(sapi_module.name, "cgi", 3) != 0) &&
		(strcmp(sapi_module.name, "cli") != 0) &&
		(strncmp(sapi_module.name, "embed", 5) != 0)
	) {
		php_sqlite3_error(db_obj, "Not supported in multithreaded Web servers");
		RETURN_FALSE;
	}
#endif

//	if (!SQLITE3G(extension_dir)) {
//		php_sqlite3_error(db_obj, "SQLite Extension are disabled");
//		RETURN_FALSE;
//	}
//
//	if (extension_len == 0) {
//		php_sqlite3_error(db_obj, "Empty string as an extension");
//		RETURN_FALSE;
//	}

//	extension_dir = SQLITE3G(extension_dir);
//	extension_dir_len = strlen(SQLITE3G(extension_dir));

//	if (IS_SLASH(extension_dir[extension_dir_len-1])) {
//		spprintf(&lib_path, 0, "%s%s", extension_dir, extension);
//	} else {
//		spprintf(&lib_path, 0, "%s%c%s", extension_dir, DEFAULT_SLASH, extension);
//	}

	spprintf(&lib_path, 0, "%s",extension);

	if (!VCWD_REALPATH(lib_path, fullpath)) {
//		php_sqlite3_error(db_obj, "Unable to load extension at '%s'", lib_path);
		// TODO - enable this error
//		zend_throw_exception_ex(php_pdo_get_exception(), 0, "There is no active transaction");
		efree(lib_path);
		RETURN_FALSE;
	}

	efree(lib_path);

//	if (strncmp(fullpath, extension_dir, extension_dir_len) != 0) {
//		php_sqlite3_error(db_obj, "Unable to open extensions outside the defined directory");
//		RETURN_FALSE;
//	}

// note: expected 'sqlite3 *' but argument is of type 'pdo_sqlite_db_handle *'
	sqlite3 *sqlite_handle;

	sqlite_handle = db_handle->db;

	/* Extension loading should only be enabled for when we attempt to load */
	sqlite3_enable_load_extension(sqlite_handle, 1);
	if (sqlite3_load_extension(sqlite_handle, fullpath, 0, &errtext) != SQLITE_OK) {
		//php_sqlite3_error(db_obj, "%s", errtext);
		zend_throw_exception_ex(php_pdo_get_exception(), 0, errtext);
		sqlite3_free(errtext);
		sqlite3_enable_load_extension(sqlite_handle, 0);
		RETURN_FALSE;
	}
	sqlite3_enable_load_extension(sqlite_handle, 0);

	RETURN_TRUE;
}
/* }}} */
#endif
