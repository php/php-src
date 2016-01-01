/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wez Furlong <wez@php.net>                                    |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef PHP_WIN32
# define PDO_ODBC_TYPE	"Win32"
#endif

#ifndef PDO_ODBC_TYPE
# warning Please fix configure to give your ODBC libraries a name
# define PDO_ODBC_TYPE	"Unknown"
#endif

/* {{{ Roll a dice, pick a header at random... */
#if HAVE_SQLCLI1_H
# include <sqlcli1.h>
# if defined(DB268K) && HAVE_LIBRARYMANAGER_H
#  include <LibraryManager.h>
# endif
#endif

#if HAVE_ODBC_H
# include <odbc.h>
#endif

#if HAVE_IODBC_H
# include <iodbc.h>
#endif

#if HAVE_SQLUNIX_H && !defined(PHP_WIN32)
# include <sqlunix.h>
#endif

#if HAVE_SQLTYPES_H
# include <sqltypes.h>
#endif

#if HAVE_SQLUCODE_H
# include <sqlucode.h>
#endif

#if HAVE_SQL_H
# include <sql.h>
#endif

#if HAVE_ISQL_H
# include <isql.h>
#endif

#if HAVE_SQLEXT_H
# include <sqlext.h>
#endif

#if HAVE_ISQLEXT_H
# include <isqlext.h>
#endif

#if HAVE_UDBCEXT_H
# include <udbcext.h>
#endif

#if HAVE_CLI0CORE_H
# include <cli0core.h>
#endif

#if HAVE_CLI0EXT1_H
# include <cli0ext.h>
#endif

#if HAVE_CLI0CLI_H
# include <cli0cli.h>
#endif

#if HAVE_CLI0DEFS_H
# include <cli0defs.h>
#endif

#if HAVE_CLI0ENV_H
# include <cli0env.h>
#endif

#if HAVE_ODBCSDK_H
# include <odbcsdk.h>
#endif

/* }}} */

/* {{{ Figure out the type for handles */
#if !defined(HENV) && !defined(SQLHENV) && defined(SQLHANDLE)
# define PDO_ODBC_HENV		SQLHANDLE
# define PDO_ODBC_HDBC		SQLHANDLE
# define PDO_ODBC_HSTMT		SQLHANDLE
#elif !defined(HENV) && (defined(SQLHENV) || defined(DB2CLI_VER))
# define PDO_ODBC_HENV		SQLHENV
# define PDO_ODBC_HDBC		SQLHDBC
# define PDO_ODBC_HSTMT		SQLHSTMT
#else
# define PDO_ODBC_HENV		HENV
# define PDO_ODBC_HDBC		HDBC
# define PDO_ODBC_HSTMT		HSTMT
#endif
/* }}} */

typedef struct {
	char last_state[6];
	char last_err_msg[SQL_MAX_MESSAGE_LENGTH];
	SDWORD last_error;
	const char *file, *what;
	int line;
} pdo_odbc_errinfo;

typedef struct {
	PDO_ODBC_HENV	env;
	PDO_ODBC_HDBC	dbc;
	pdo_odbc_errinfo einfo;
	unsigned assume_utf8:1;
	unsigned _spare:31;
} pdo_odbc_db_handle;

typedef struct {
	char *data;
	unsigned long datalen;
	SQLLEN fetched_len;
	SWORD	coltype;
	char colname[128];
	unsigned is_long;
	unsigned is_unicode:1;
	unsigned _spare:31;
} pdo_odbc_column;

typedef struct {
	PDO_ODBC_HSTMT	stmt;
	pdo_odbc_column *cols;
	pdo_odbc_db_handle *H;
	pdo_odbc_errinfo einfo;
	char *convbuf;
	unsigned long convbufsize;
	unsigned going_long:1;
	unsigned assume_utf8:1;
	unsigned _spare:30;
} pdo_odbc_stmt;

typedef struct {
	SQLLEN len;
	SQLSMALLINT paramtype;
	char *outbuf;
	unsigned is_unicode:1;
	unsigned _spare:31;
} pdo_odbc_param;
	
extern pdo_driver_t pdo_odbc_driver;
extern struct pdo_stmt_methods odbc_stmt_methods;

void pdo_odbc_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt, PDO_ODBC_HSTMT statement, char *what, const char *file, int line TSRMLS_DC);
#define pdo_odbc_drv_error(what)	pdo_odbc_error(dbh, NULL, SQL_NULL_HSTMT, what, __FILE__, __LINE__ TSRMLS_CC)
#define pdo_odbc_stmt_error(what)	pdo_odbc_error(stmt->dbh, stmt, SQL_NULL_HSTMT, what, __FILE__, __LINE__ TSRMLS_CC)
#define pdo_odbc_doer_error(what)	pdo_odbc_error(dbh, NULL, stmt, what, __FILE__, __LINE__ TSRMLS_CC)

void pdo_odbc_init_error_table(void);
void pdo_odbc_fini_error_table(void);

#ifdef SQL_ATTR_CONNECTION_POOLING
extern SQLUINTEGER pdo_odbc_pool_on;
extern SQLUINTEGER pdo_odbc_pool_mode;
#endif

enum {
	PDO_ODBC_ATTR_USE_CURSOR_LIBRARY = PDO_ATTR_DRIVER_SPECIFIC,
	PDO_ODBC_ATTR_ASSUME_UTF8 /* assume that input strings are UTF-8 when feeding data to unicode columns */
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
