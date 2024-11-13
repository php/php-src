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
   | Authors: Scott MacVicar <scottmac@php.net>                           |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_SQLITE_STRUCTS_H
#define PHP_SQLITE_STRUCTS_H

#include <sqlite3.h>

/* for backwards compatibility reasons */
#ifndef SQLITE_OPEN_READONLY
#define SQLITE_OPEN_READONLY 0x00000001
#endif

#ifndef SQLITE_OPEN_READWRITE
#define SQLITE_OPEN_READWRITE 0x00000002
#endif

#ifndef SQLITE_OPEN_CREATE
#define SQLITE_OPEN_CREATE 0x00000004
#endif

/* Structure for SQLite Statement Parameter. */
struct php_sqlite3_bound_param  {
	zend_long param_number;
	zend_string *name;
	zend_long type;
	zval parameter;
};

/* Structure for SQLite function. */
typedef struct _php_sqlite3_func {
	struct _php_sqlite3_func *next;

	const char *func_name;
	int argc;

	zend_fcall_info_cache func;
	zend_fcall_info_cache step;
	zend_fcall_info_cache fini;
} php_sqlite3_func;

/* Structure for SQLite collation function */
typedef struct _php_sqlite3_collation {
	struct _php_sqlite3_collation *next;

	const char *collation_name;
	zend_fcall_info_cache cmp_func;
} php_sqlite3_collation;

/* Structure for SQLite Database object. */
typedef struct _php_sqlite3_db_object  {
	int initialised;
	sqlite3 *db;
	php_sqlite3_func *funcs;
	php_sqlite3_collation *collations;
	zend_fcall_info_cache authorizer_fcc;

	bool exception;

	zend_llist free_list;
	zend_object zo;
} php_sqlite3_db_object;

static inline php_sqlite3_db_object *php_sqlite3_db_from_obj(zend_object *obj) {
	return (php_sqlite3_db_object*)((char*)(obj) - XtOffsetOf(php_sqlite3_db_object, zo));
}

#define Z_SQLITE3_DB_P(zv)  php_sqlite3_db_from_obj(Z_OBJ_P((zv)))

/* Structure for SQLite Database object. */
typedef struct _php_sqlite3_agg_context  {
	zval zval_context;
	zend_long row_count;
} php_sqlite3_agg_context;

typedef struct _php_sqlite3_stmt_object php_sqlite3_stmt;
typedef struct _php_sqlite3_result_object php_sqlite3_result;

/* sqlite3 objects to be destroyed */
typedef struct _php_sqlite3_free_list {
	zval stmt_obj_zval;
	php_sqlite3_stmt *stmt_obj;
} php_sqlite3_free_list;

/* Structure for SQLite Result object. */
struct _php_sqlite3_result_object  {
	php_sqlite3_db_object *db_obj;
	php_sqlite3_stmt *stmt_obj;
	zval stmt_obj_zval;

	/* Cache of column names to speed up repeated fetchArray(SQLITE3_ASSOC) calls.
	 * Cache is cleared on reset() and finalize() calls. */
	int column_count;
	zend_string **column_names;

	int is_prepared_statement;
	zend_object zo;
};

static inline php_sqlite3_result *php_sqlite3_result_from_obj(zend_object *obj) {
	return (php_sqlite3_result*)((char*)(obj) - XtOffsetOf(php_sqlite3_result, zo));
}

#define Z_SQLITE3_RESULT_P(zv)  php_sqlite3_result_from_obj(Z_OBJ_P((zv)))

/* Structure for SQLite Statement object. */
struct _php_sqlite3_stmt_object  {
	sqlite3_stmt *stmt;
	php_sqlite3_db_object *db_obj;
	zval db_obj_zval;

	int initialised;

	/* Keep track of the zvals for bound parameters */
	HashTable *bound_params;
	zend_object zo;
};

static inline php_sqlite3_stmt *php_sqlite3_stmt_from_obj(zend_object *obj) {
	return (php_sqlite3_stmt*)((char*)(obj) - XtOffsetOf(php_sqlite3_stmt, zo));
}

#define Z_SQLITE3_STMT_P(zv)  php_sqlite3_stmt_from_obj(Z_OBJ_P((zv)))

#endif
