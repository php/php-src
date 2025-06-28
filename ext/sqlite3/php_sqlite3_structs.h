/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
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

	zend_string *func_name;
	int argc;

	zend_fcall_info_cache func;
	zend_fcall_info_cache step;
	zend_fcall_info_cache fini;
} php_sqlite3_func;

/* Structure for SQLite collation function */
typedef struct _php_sqlite3_collation {
	struct _php_sqlite3_collation *next;

	zend_string *collation_name;
	zend_fcall_info_cache cmp_func;
} php_sqlite3_collation;

/* Structure for SQLite Database object. */
typedef struct _php_sqlite3_db_object  {
	bool initialised;
	bool exception;

	sqlite3 *db;
	php_sqlite3_func *funcs;
	php_sqlite3_collation *collations;
	zend_fcall_info_cache authorizer_fcc;

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

/* Structure for SQLite Result object. */
struct _php_sqlite3_result_object  {
	php_sqlite3_db_object *db_obj;
	php_sqlite3_stmt *stmt_obj;

	bool is_prepared_statement;
	/* Cache of column names to speed up repeated fetchArray(SQLITE3_ASSOC) calls.
	 * Cache is cleared on reset() and finalize() calls. */
	int column_count;
	zend_string **column_names;

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

	bool initialised;

	/* Keep track of the zvals for bound parameters */
	HashTable *bound_params;
	zend_object zo;
};

static inline php_sqlite3_stmt *php_sqlite3_stmt_from_obj(zend_object *obj) {
	return (php_sqlite3_stmt*)((char*)(obj) - XtOffsetOf(php_sqlite3_stmt, zo));
}

#define Z_SQLITE3_STMT_P(zv)  php_sqlite3_stmt_from_obj(Z_OBJ_P((zv)))

#endif
