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
  | Author: George Schlossnagle <george@omniti.com>                      |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_PDO_MYSQL_INT_H
#define PHP_PDO_MYSQL_INT_H

#include <mysql.h>

/* stuff we use in a mySQL database handle */
typedef struct {
	MYSQL 		*server;
	unsigned int	last_err;
	unsigned attached:1;
	unsigned _reserved:31;
} pdo_mysql_db_handle;

typedef struct {
	MYSQL_FIELD		*def;
} pdo_mysql_column;

typedef struct {
	pdo_mysql_db_handle 	*H;
	MYSQL_RES		*result;
	MYSQL_ROW		current_data;
	long			*current_lengths;
	int		last_err;
	pdo_mysql_column 	*cols;
} pdo_mysql_stmt;

typedef struct {
	char		*repr;
	long		repr_len;
	int		mysql_type;
	void		*thing;	/* for LOBS, REFCURSORS etc. */
} pdo_mysql_bound_param;

extern pdo_driver_t pdo_mysql_driver;

extern int _mysql_error(char *what, int errno, const char *file, int line TSRMLS_DC);
#define pdo_mysql_error(w,s)	_mysql_error(w, s, __FILE__, __LINE__ TSRMLS_CC)
extern int mysql_handle_error(pdo_dbh_t *dbh, pdo_mysql_db_handle *H, int errcode);

extern struct pdo_stmt_methods mysql_stmt_methods;
#endif
