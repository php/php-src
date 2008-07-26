/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
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

#ifndef PHP_SQLITE_H
#define PHP_SQLITE_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

/* Include PHP Standard Header */
#include "php.h"

/* Include headers */
#include <sqlite3.h>

#define PHP_SQLITE3_VERSION	 "0.7-dev"

extern zend_module_entry sqlite3_module_entry;
#define phpext_sqlite3_ptr &sqlite3_module_entry

ZEND_BEGIN_MODULE_GLOBALS(sqlite3)
	char *extension_dir;
ZEND_END_MODULE_GLOBALS(sqlite3)

#ifdef ZTS
# define SQLITE3G(v) TSRMG(sqlite3_globals_id, zend_sqlite3_globals *, v)
#else
# define SQLITE3G(v) (sqlite3_globals.v)
#endif

#define PHP_SQLITE3_ASSOC	1<<0
#define PHP_SQLITE3_NUM	1<<1
#define PHP_SQLITE3_BOTH	(PHP_SQLITE3_ASSOC|PHP_SQLITE3_NUM)

/* for backwards compatability reasons */
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
	long param_number;
	char *name;
	int name_len;
	int type;

	zval *parameter;
};

struct php_sqlite3_fci {
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
};

/* Structure for SQLite function. */
typedef struct _php_sqlite3_func {
	struct _php_sqlite3_func *next;

	const char *func_name;
	int argc;

	zval *func, *step, *fini;
	struct php_sqlite3_fci afunc, astep, afini;
} php_sqlite3_func;

/* Structure for SQLite Database object. */
typedef struct _php_sqlite3_db_object  {
	zend_object zo;
	int initialised;
	sqlite3 *db;
	php_sqlite3_func *funcs;
} php_sqlite3_db_object;

/*typedef struct _php_sqlite3_stmt {
	sqlite3_stmt *stmt;
	int initialised;
} php_sqlite3_stmt;*/

typedef struct _php_sqlite3_stmt_object php_sqlite3_stmt;
typedef struct _php_sqlite3_result_object php_sqlite3_result;

/* sqlite3 objects to be destroyed */
typedef struct _php_sqlite3_stmt_free_list {
	sqlite3_stmt *stmt;

	zval *statement_object;
	zval *result_object;
} php_sqlite3_stmt_free_list;

/* Structure for SQLite Result object. */
struct _php_sqlite3_result_object  {
	zend_object zo;
	php_sqlite3_db_object *db_obj;
	php_sqlite3_stmt *stmt_obj;
	zval *stmt_obj_zval;

	int initialised;
	
	int is_prepared_statement;
	int complete;
};

/* Structure for SQLite Statement object. */
struct _php_sqlite3_stmt_object  {
	zend_object zo;
	sqlite3_stmt *stmt;
	php_sqlite3_db_object *db_obj;
	zval *db_obj_zval;

	int initialised;

	/* Keep track of the zvals for bound parameters */
	HashTable *bound_params;
};

#endif


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
