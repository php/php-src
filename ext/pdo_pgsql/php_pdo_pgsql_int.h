/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Edin Kadribasic <edink@emini.dk>                            |
  |          Ilia Alshanestsky <ilia@prohost.org>                        |
  |          Wez Furlong <wez@php.net>                                   |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_PDO_PGSQL_INT_H
#define PHP_PDO_PGSQL_INT_H

#include <libpq-fe.h>
#include <libpq/libpq-fs.h>
#include <php.h>

#define PHP_PDO_PGSQL_CONNECTION_FAILURE_SQLSTATE "08006"

typedef struct {
	const char *file;
	int line;
	unsigned int errcode;
	char *errmsg;
} pdo_pgsql_error_info;

/* stuff we use in a pgsql database handle */
typedef struct {
	PGconn		*server;
	unsigned 	attached:1;
	unsigned 	_reserved:31;
	pdo_pgsql_error_info	einfo;
	Oid 		pgoid;
#if HAVE_PQPREPARE
	/* The following two variables have the same purpose. Unfortunately we need
	   to keep track of two different attributes having the same effect.
	   It might be worth to deprecate the driver specific one soon. */
	int		emulate_prepares;
	int		disable_native_prepares;
#endif
	unsigned int stmt_counter;
} pdo_pgsql_db_handle;

typedef struct {
	char         *def;
	Oid          pgsql_type;
	long         intval;
	zend_bool    boolval;
} pdo_pgsql_column;

typedef struct {
	pdo_pgsql_db_handle     *H;
	PGresult                *result;
	int                     current_row;
	pdo_pgsql_column        *cols;
	char *cursor_name;
#if HAVE_PQPREPARE
	char *stmt_name;
	char *query;
	char **param_values;
	int *param_lengths;
	int *param_formats;
	Oid *param_types;
#endif
	zend_bool is_prepared;
} pdo_pgsql_stmt;

typedef struct {
	Oid     oid;
} pdo_pgsql_bound_param;

extern pdo_driver_t pdo_pgsql_driver;

extern int _pdo_pgsql_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt, int errcode, const char *sqlstate, const char *file, int line TSRMLS_DC);
#define pdo_pgsql_error(d,e,z)	_pdo_pgsql_error(d, NULL, e, z, __FILE__, __LINE__ TSRMLS_CC)
#define pdo_pgsql_error_stmt(s,e,z)	_pdo_pgsql_error(s->dbh, s, e, z, __FILE__, __LINE__ TSRMLS_CC)

extern struct pdo_stmt_methods pgsql_stmt_methods;

#define pdo_pgsql_sqlstate(r) PQresultErrorField(r, PG_DIAG_SQLSTATE)

enum {
	PDO_PGSQL_ATTR_DISABLE_NATIVE_PREPARED_STATEMENT = PDO_ATTR_DRIVER_SPECIFIC,
};

struct pdo_pgsql_lob_self {
	pdo_dbh_t *dbh;
	PGconn *conn;
	int lfd;
	Oid oid;
};

enum pdo_pgsql_specific_constants {
	PGSQL_TRANSACTION_IDLE = PQTRANS_IDLE,
	PGSQL_TRANSACTION_ACTIVE = PQTRANS_ACTIVE,
	PGSQL_TRANSACTION_INTRANS = PQTRANS_INTRANS,
	PGSQL_TRANSACTION_INERROR = PQTRANS_INERROR,
	PGSQL_TRANSACTION_UNKNOWN = PQTRANS_UNKNOWN
};

php_stream *pdo_pgsql_create_lob_stream(pdo_dbh_t *stmt, int lfd, Oid oid TSRMLS_DC);
extern php_stream_ops pdo_pgsql_lob_stream_ops;

#endif /* PHP_PDO_PGSQL_INT_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
