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
  | Author: Ard Biesheuvel <abies@php.net>                               |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_PDO_FIREBIRD_INT_H
#define PHP_PDO_FIREBIRD_INT_H

#include <ibase.h>

#ifdef SQLDA_VERSION
#define PDO_FB_SQLDA_VERSION SQLDA_VERSION
#else
#define PDO_FB_SQLDA_VERSION 1
#endif

#define PDO_FB_DIALECT 3

#define PDO_FB_DEF_DATE_FMT "%Y-%m-%d"
#define PDO_FB_DEF_TIME_FMT "%H:%M:%S"
#define PDO_FB_DEF_TIMESTAMP_FMT PDO_FB_DEF_DATE_FMT " " PDO_FB_DEF_TIME_FMT

#define SHORT_MAX (1 << (8*sizeof(short)-1))

#if SIZEOF_ZEND_LONG == 8 && !defined(PHP_WIN32)
# define LL_LIT(lit) lit ## L
#else
# define LL_LIT(lit) lit ## LL
#endif
#define LL_MASK "ll"

/* Firebird API has a couple of missing const decls in its API */
#define const_cast(s) ((char*)(s))

#ifdef PHP_WIN32
typedef void (__stdcall *info_func_t)(char*);
#else
typedef void (*info_func_t)(char*);
#endif

#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif

#if defined(_LP64) || defined(__LP64__) || defined(__arch64__) || defined(_WIN64)
# define PDO_FIREBIRD_HANDLE_INITIALIZER 0U
#else
# define PDO_FIREBIRD_HANDLE_INITIALIZER NULL
#endif

extern bool _firebird_commit_transaction(pdo_dbh_t *dbh, bool retain);
#define firebird_commit_transaction(d,r) _firebird_commit_transaction(d, r)

extern bool _firebird_rollback_transaction(pdo_dbh_t *dbh, bool retain);
#define firebird_rollback_transaction(d,r) _firebird_rollback_transaction(d, r)

typedef struct {

	/* the result of the last API call */
	ISC_STATUS isc_status[20];

	/* the connection handle */
	isc_db_handle db;

	/* the transaction handle */
	isc_tr_handle tr;
	bool in_manually_txn:1;
	bool is_writable_txn:1;

	/* the last error that didn't come from the API */
	char const *last_app_error;

	/* date and time format strings, can be set by the set_attribute method */
	char *date_format;
	char *time_format;
	char *timestamp_format;

	unsigned sql_dialect:2;

	/* prepend table names on column names in fetch */
	unsigned fetch_table_names:1;

	unsigned _reserved:29;

	/* transaction isolation level */
	zend_ulong txn_isolation_level;
} pdo_firebird_db_handle;


typedef struct {

	/* the link that owns this statement */
	pdo_firebird_db_handle *H;

	/* the statement handle */
	isc_stmt_handle stmt;

	/* the name of the cursor (if it has one) */
	char name[32];

	/* the type of statement that was issued */
	char statement_type:8;

	/* whether EOF was reached for this statement */
	unsigned exhausted:1;

	/* successful isc_dsql_execute opens a cursor */
	unsigned cursor_open:1;

	unsigned _reserved:22;

	/* the named params that were converted to ?'s by the driver */
	HashTable *named_params;

	/* the input SQLDA */
	XSQLDA *in_sqlda;

	/* the output SQLDA */
	XSQLDA out_sqlda; /* last member */

} pdo_firebird_stmt;

extern const pdo_driver_t pdo_firebird_driver;

extern const struct pdo_stmt_methods firebird_stmt_methods;

void _firebird_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt, char const *file, zend_long line);

enum {
	PDO_FB_ATTR_DATE_FORMAT = PDO_ATTR_DRIVER_SPECIFIC,
	PDO_FB_ATTR_TIME_FORMAT,
	PDO_FB_ATTR_TIMESTAMP_FORMAT,

	/* transaction isolation level */
	PDO_FB_TRANSACTION_ISOLATION_LEVEL,
	PDO_FB_READ_COMMITTED,
	PDO_FB_REPEATABLE_READ,
	PDO_FB_SERIALIZABLE,

	/** transaction access mode */
	PDO_FB_WRITABLE_TRANSACTION,
};

#endif	/* PHP_PDO_FIREBIRD_INT_H */
