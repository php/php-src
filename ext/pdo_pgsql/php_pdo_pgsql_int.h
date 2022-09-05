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
  | Authors: Edin Kadribasic <edink@emini.dk>                            |
  |          Ilia Alshanestsky <ilia@prohost.org>                        |
  |          Wez Furlong <wez@php.net>                                   |
  +----------------------------------------------------------------------+
*/

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
	unsigned int	stmt_counter;
	/* The following two variables have the same purpose. Unfortunately we need
	   to keep track of two different attributes having the same effect. */
	bool		emulate_prepares;
	bool		disable_native_prepares; /* deprecated since 5.6 */
	bool		disable_prepares;
	HashTable       *lob_streams;
} pdo_pgsql_db_handle;

typedef struct {
	Oid          pgsql_type;
} pdo_pgsql_column;

typedef struct {
	pdo_pgsql_db_handle     *H;
	PGresult                *result;
	pdo_pgsql_column        *cols;
	char *cursor_name;
	char *stmt_name;
	zend_string *query;
	char **param_values;
	int *param_lengths;
	int *param_formats;
	Oid *param_types;
	int                     current_row;
	bool is_prepared;
} pdo_pgsql_stmt;

typedef struct {
	Oid     oid;
} pdo_pgsql_bound_param;

extern const pdo_driver_t pdo_pgsql_driver;

extern int _pdo_pgsql_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt, int errcode, const char *sqlstate, const char *msg, const char *file, int line);
#define pdo_pgsql_error(d,e,z)	_pdo_pgsql_error(d, NULL, e, z, NULL, __FILE__, __LINE__)
#define pdo_pgsql_error_msg(d,e,m)	_pdo_pgsql_error(d, NULL, e, NULL, m, __FILE__, __LINE__)
#define pdo_pgsql_error_stmt(s,e,z)	_pdo_pgsql_error(s->dbh, s, e, z, NULL, __FILE__, __LINE__)
#define pdo_pgsql_error_stmt_msg(stmt, e, sqlstate, msg) \
	_pdo_pgsql_error(stmt->dbh, stmt, e, sqlstate, msg, __FILE__, __LINE__)

extern const struct pdo_stmt_methods pgsql_stmt_methods;

#define pdo_pgsql_sqlstate(r) PQresultErrorField(r, PG_DIAG_SQLSTATE)

enum {
	PDO_PGSQL_ATTR_DISABLE_PREPARES = PDO_ATTR_DRIVER_SPECIFIC,
};

struct pdo_pgsql_lob_self {
	zval dbh;
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

php_stream *pdo_pgsql_create_lob_stream(zval *pdh, int lfd, Oid oid);
extern const php_stream_ops pdo_pgsql_lob_stream_ops;

void pdo_libpq_version(char *buf, size_t len);
void pdo_pgsql_close_lob_streams(pdo_dbh_t *dbh);

#endif /* PHP_PDO_PGSQL_INT_H */
