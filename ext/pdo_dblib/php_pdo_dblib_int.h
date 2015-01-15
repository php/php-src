/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
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
  |         Frank M. Kromann <frank@kromann.info>                        |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_PDO_DBLIB_INT_H
#define PHP_PDO_DBLIB_INT_H

#ifndef PDO_DBLIB_FLAVOUR
# define PDO_DBLIB_FLAVOUR "Generic DB-lib"
#endif

#if PHP_DBLIB_IS_MSSQL
# include <sqlfront.h>
# include <sqldb.h>

# define DBERRHANDLE(a, b)	dbprocerrhandle(a, b)
# define DBMSGHANDLE(a, b)	dbprocmsghandle(a, b)
# define EHANDLEFUNC		DBERRHANDLE_PROC
# define MHANDLEFUNC		DBMSGHANDLE_PROC
# define DBSETOPT(a, b, c)	dbsetopt(a, b, c)
# define SYBESMSG		SQLESMSG
# define SYBESEOF		SQLESEOF
# define SYBEFCON		SQLECONN		/* SQLEFCON does not exist in MS SQL Server. */
# define SYBEMEM		SQLEMEM
# define SYBEPWD		SQLEPWD

#else
# include <sybfront.h>
# include <sybdb.h>
# include <syberror.h>

/* alias some types */
# define SQLTEXT	SYBTEXT
# define SQLCHAR	SYBCHAR
# define SQLVARCHAR	SYBVARCHAR
# define SQLINT1	SYBINT1
# define SQLINT2	SYBINT2
# define SQLINT4	SYBINT4
# define SQLINTN	SYBINTN
# define SQLBIT		SYBBIT
# define SQLFLT4	SYBREAL
# define SQLFLT8	SYBFLT8
# define SQLFLTN	SYBFLTN
# define SQLDECIMAL	SYBDECIMAL
# define SQLNUMERIC	SYBNUMERIC
# define SQLDATETIME	SYBDATETIME
# define SQLDATETIM4	SYBDATETIME4
# define SQLDATETIMN	SYBDATETIMN
# define SQLMONEY		SYBMONEY
# define SQLMONEY4		SYBMONEY4
# define SQLMONEYN		SYBMONEYN
# define SQLIMAGE		SYBIMAGE
# define SQLBINARY		SYBBINARY
# define SQLVARBINARY	SYBVARBINARY
# ifdef SYBUNIQUE
#  define SQLUNIQUE		SYBUNIQUE
#else
#  define SQLUNIQUE		36 /* FreeTDS Hack */
# endif

# define DBERRHANDLE(a, b)	dberrhandle(b)
# define DBMSGHANDLE(a, b)	dbmsghandle(b)
# define DBSETOPT(a, b, c)	dbsetopt(a, b, c, -1)
# define NO_MORE_RPC_RESULTS	3
# define dbfreelogin		dbloginfree
# define dbrpcexec			dbrpcsend

typedef short TDS_SHORT;
# ifndef PHP_WIN32
typedef unsigned char *LPBYTE;
# endif
typedef float			DBFLT4;
#endif

int pdo_dblib_error_handler(DBPROCESS *dbproc, int severity, int dberr,
	int oserr, char *dberrstr, char *oserrstr);

int pdo_dblib_msg_handler(DBPROCESS *dbproc, DBINT msgno, int msgstate,
	int severity, char *msgtext, char *srvname, char *procname, DBUSMALLINT line);

extern pdo_driver_t pdo_dblib_driver;
extern struct pdo_stmt_methods dblib_stmt_methods;

typedef struct {
	int severity;
	int oserr;
	int dberr;
	char *oserrstr;
	char *dberrstr;
	char *sqlstate;
	char *lastmsg;
} pdo_dblib_err;

typedef struct {
	LOGINREC	*login;
	DBPROCESS	*link;

	pdo_dblib_err err;
} pdo_dblib_db_handle;

typedef struct {
	pdo_dblib_db_handle *H;
	pdo_dblib_err err;
} pdo_dblib_stmt;

typedef struct {
	const char* key;
	int value;
} pdo_dblib_keyval;


ZEND_BEGIN_MODULE_GLOBALS(dblib)
	pdo_dblib_err err;
	char sqlstate[6];
ZEND_END_MODULE_GLOBALS(dblib)

#ifdef ZTS
# define DBLIB_G(v) TSRMG(dblib_globals_id, zend_dblib_globals *, v)
#else
# define DBLIB_G(v) (dblib_globals.v)
#endif

ZEND_EXTERN_MODULE_GLOBALS(dblib);

#endif

