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
  | Author: Wez Furlong <wez@php.net>                                    |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_PDO_SQLITE_INT_H
#define PHP_PDO_SQLITE_INT_H

#include <sqlite3.h>

typedef struct {
	const char *file;
	int line;
	unsigned int errcode;
	char *errmsg;
} pdo_sqlite_error_info;

struct pdo_sqlite_fci {
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
};

struct pdo_sqlite_func {
	struct pdo_sqlite_func *next;

	zval func, step, fini;
	int argc;
	const char *funcname;

	/* accelerated callback references */
	struct pdo_sqlite_fci afunc, astep, afini;
};

struct pdo_sqlite_collation {
	struct pdo_sqlite_collation *next;

	const char *name;
	zval callback;
	struct pdo_sqlite_fci fc;
};

typedef struct {
	sqlite3 *db;
	pdo_sqlite_error_info einfo;
	struct pdo_sqlite_func *funcs;
	struct pdo_sqlite_collation *collations;
} pdo_sqlite_db_handle;

typedef struct {
	pdo_sqlite_db_handle 	*H;
	sqlite3_stmt *stmt;
	unsigned pre_fetched:1;
	unsigned done:1;
} pdo_sqlite_stmt;

extern const pdo_driver_t pdo_sqlite_driver;

extern int _pdo_sqlite_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt, const char *file, int line);
#define pdo_sqlite_error(s) _pdo_sqlite_error(s, NULL, __FILE__, __LINE__)
#define pdo_sqlite_error_stmt(s) _pdo_sqlite_error(stmt->dbh, stmt, __FILE__, __LINE__)

extern const struct pdo_stmt_methods sqlite_stmt_methods;

enum {
	PDO_SQLITE_ATTR_OPEN_FLAGS = PDO_ATTR_DRIVER_SPECIFIC,
	PDO_SQLITE_ATTR_READONLY_STATEMENT,
	PDO_SQLITE_ATTR_EXTENDED_RESULT_CODES
};

#endif
