/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2004 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Edin Kadribasic <edink@emini.dk>                             |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_PDO_PGSQL_INT_H
#define PHP_PDO_PGSQL_INT_H

#if HAVE_PDO_PGSQL 

#include <libpq-fe.h>

/* stuff we use in a pgsql database handle */
typedef struct {
	PGconn		*server;
	char		*last_err;
	unsigned 	attached:1;
	unsigned 	_reserved:31;
} pdo_pgsql_db_handle;

typedef struct {
	char		*def;
} pdo_pgsql_column;

typedef struct {
	pdo_pgsql_db_handle     *H;
	PGresult                *result;
	int                     current_row;
	char                    *last_err;
	pdo_pgsql_column        *cols;
} pdo_pgsql_stmt;

typedef struct {
	char		*repr;
	long		repr_len;
	int		pgsql_type;
	void		*thing;	/* for LOBS, REFCURSORS etc. */
} pdo_pgsql_bound_param;

extern pdo_driver_t pdo_pgsql_driver;

extern int _pdo_pgsql_error(char *what, char *errmsg, const char *file, int line TSRMLS_DC);
#define pdo_pgsql_error(w,s)	_pdo_pgsql_error(w, s, __FILE__, __LINE__ TSRMLS_CC)
extern int pgsql_handle_error(pdo_dbh_t *dbh, pdo_pgsql_db_handle *H, int errcode);

extern struct pdo_stmt_methods pgsql_stmt_methods;

#endif /* HAVE_PDO_PGSQL */
#endif /* PHP_PDO_PGSQL_INT_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
