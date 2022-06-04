
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

