/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stig Sæther Bakken <ssb@guardian.no>                        |
   |          Andreas Karajannis <Andreas.Karajannis@gmd.de>              |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_ODBC_H
#define PHP_ODBC_H

#if HAVE_UODBC
#define ODBCVER 0x0250
/*#ifndef MSVC5
#define FAR
#endif
*/
#ifdef ZTS
#include "TSRM.h"
#endif

/* checking in the same order as in configure.in */

#if defined(HAVE_SOLID) || defined(HAVE_SOLID_35) /* Solid Server */

#define ODBC_TYPE "Solid"
#if defined(HAVE_SOLID)
 #include <cli0core.h>
 #include <cli0ext1.h>
 /*the following help for SOLID 3.0 */
 #include <cli0cli.h>
 #include <cli0env.h>
#elif defined(HAVE_SOLID_35)
 #if !defined(PHP_WIN32)
  #include <sqlunix.h>
 #endif
 #include <sql.h>
 #include <sqltypes.h>
 #include <sqlucode.h>
 #include <sqlext.h>
#endif
#undef HAVE_SQL_EXTENDED_FETCH
PHP_FUNCTION(solid_fetch_prev);
#define SQLSMALLINT SWORD
#define SQLUSMALLINT UWORD
#ifndef SQL_SUCCEEDED
#define SQL_SUCCEEDED(rc) (((rc)&(~1))==0)
#endif

#elif defined(HAVE_EMPRESS) /* Empress */

#define ODBC_TYPE "Empress"
#include <sql.h>
#include <sqlext.h>
#undef HAVE_SQL_EXTENDED_FETCH

#elif defined(HAVE_ADABAS) /* Adabas D */

#define ODBC_TYPE "Adabas D"
#include <WINDOWS.H>
#include <sql.h>
#include <sqlext.h>
#define HAVE_SQL_EXTENDED_FETCH 1
#define SQL_SUCCEEDED(rc) (((rc)&(~1))==0)

#elif defined(HAVE_IODBC) /* iODBC library */

#define ODBC_TYPE "iODBC"
#include <isql.h>
#include <isqlext.h>
#define HAVE_SQL_EXTENDED_FETCH 1
#define SQL_FD_FETCH_ABSOLUTE   0x00000010L
#define SQL_CURSOR_DYNAMIC      2UL
#define SQL_NO_TOTAL            (-4)
#define SQL_SO_DYNAMIC          0x00000004L
#define SQL_LEN_DATA_AT_EXEC_OFFSET  (-100)
#define SQL_LEN_DATA_AT_EXEC(length) (-(length)+SQL_LEN_DATA_AT_EXEC_OFFSET)
#ifndef SQL_SUCCEEDED
#define SQL_SUCCEEDED(rc) (((rc)&(~1))==0)
#endif

#elif defined(HAVE_UNIXODBC) /* unixODBC library */

#define ODBC_TYPE "unixODBC"
#include <sql.h>
#include <sqlext.h>
#define HAVE_SQL_EXTENDED_FETCH 1

#elif defined(HAVE_ESOOB) /* Easysoft ODBC-ODBC Bridge library */

#define ODBC_TYPE "ESOOB"
#include <sql.h>
#include <sqlext.h>
#define HAVE_SQL_EXTENDED_FETCH 1

#elif defined(HAVE_OPENLINK) /* OpenLink ODBC drivers */

#define ODBC_TYPE "Openlink"
#include <iodbc.h>
#include <isql.h>
#include <isqlext.h>
#include <udbcext.h>
#define HAVE_SQL_EXTENDED_FETCH 1
#define SQLSMALLINT SWORD
#define SQLUSMALLINT UWORD

#elif defined(HAVE_VELOCIS) /* Raima Velocis */

#define ODBC_TYPE "Velocis"
#define UNIX
#define HAVE_SQL_EXTENDED_FETCH 1
#include <sql.h>
#include <sqlext.h>

#elif defined(HAVE_DBMAKER) /* DBMaker */

#define ODBC_TYPE "DBMaker"
#undef ODBCVER
#define ODBCVER 0x0300
#define HAVE_SQL_EXTENDED_FETCH 1
#include <odbc.h>


#elif defined(HAVE_CODBC) /* Custom ODBC */

#define ODBC_TYPE "Custom ODBC"
#define HAVE_SQL_EXTENDED_FETCH 1
#include <odbc.h>

#elif defined(HAVE_IBMDB2) /* DB2 CLI */

#define ODBC_TYPE "IBM DB2 CLI"
#define HAVE_SQL_EXTENDED_FETCH 1
#include <sqlcli1.h>
#ifdef DB268K
/* Need to include ASLM for 68K applications */
#include <LibraryManager.h>
#endif

#else /* MS ODBC */

#define HAVE_SQL_EXTENDED_FETCH 1
#include <WINDOWS.H>
#include <sql.h>
#include <sqlext.h>
#endif

extern zend_module_entry odbc_module_entry;
#define odbc_module_ptr &odbc_module_entry


/* user functions */
extern PHP_MINIT_FUNCTION(odbc);
extern PHP_MSHUTDOWN_FUNCTION(odbc);
extern PHP_RINIT_FUNCTION(odbc);
extern PHP_RSHUTDOWN_FUNCTION(odbc);
PHP_MINFO_FUNCTION(odbc);

PHP_FUNCTION(odbc_setoption);
PHP_FUNCTION(odbc_autocommit);
PHP_FUNCTION(odbc_close);
PHP_FUNCTION(odbc_close_all);
PHP_FUNCTION(odbc_commit);
PHP_FUNCTION(odbc_connect);
PHP_FUNCTION(odbc_pconnect);
PHP_FUNCTION(odbc_cursor);
PHP_FUNCTION(odbc_exec);
PHP_FUNCTION(odbc_do);
PHP_FUNCTION(odbc_execute);
#ifdef HAVE_DBMAKER
PHP_FUNCTION(odbc_fetch_array);
PHP_FUNCTION(odbc_fetch_object);
#endif
PHP_FUNCTION(odbc_fetch_into);
PHP_FUNCTION(odbc_fetch_row);
PHP_FUNCTION(odbc_field_len);
PHP_FUNCTION(odbc_field_scale);
PHP_FUNCTION(odbc_field_name);
PHP_FUNCTION(odbc_field_type);
PHP_FUNCTION(odbc_field_num);
PHP_FUNCTION(odbc_free_result);
PHP_FUNCTION(odbc_num_fields);
PHP_FUNCTION(odbc_num_rows);
PHP_FUNCTION(odbc_prepare);
PHP_FUNCTION(odbc_result);
PHP_FUNCTION(odbc_result_all);
PHP_FUNCTION(odbc_rollback);
PHP_FUNCTION(odbc_binmode);
PHP_FUNCTION(odbc_longreadlen);
PHP_FUNCTION(odbc_tables);
PHP_FUNCTION(odbc_columns);
#if !defined(HAVE_DBMAKER) && !defined(HAVE_SOLID) && !defined(HAVE_SOLID_35)    /* not supported now */
PHP_FUNCTION(odbc_columnprivileges);
PHP_FUNCTION(odbc_tableprivileges);
#endif
#if !defined(HAVE_SOLID) || !defined(HAVE_SOLID_35)    /* not supported */
PHP_FUNCTION(odbc_foreignkeys);
PHP_FUNCTION(odbc_procedures);
PHP_FUNCTION(odbc_procedurecolumns);
#endif
PHP_FUNCTION(odbc_gettypeinfo);
PHP_FUNCTION(odbc_primarykeys);
PHP_FUNCTION(odbc_specialcolumns);
PHP_FUNCTION(odbc_statistics);

typedef struct odbc_connection {
#if defined( HAVE_IBMDB2 ) || defined( HAVE_UNIXODBC )
	SQLHANDLE henv;
	SQLHANDLE hdbc;
#else
	HENV henv;
	HDBC hdbc;
#endif
	int id;
	int persistent;
} odbc_connection;

typedef struct odbc_result_value {
	char name[32];
	char *value;
	long int vallen;
	SDWORD coltype;
} odbc_result_value;

typedef struct odbc_result {
#if defined( HAVE_IBMDB2 ) || defined( HAVE_UNIXODBC )
	SQLHANDLE stmt;
#else
	HSTMT stmt;
#endif
	int id;
	odbc_result_value *values;
	SWORD numcols;
	SWORD numparams;
# if HAVE_SQL_EXTENDED_FETCH
	int fetch_abs;
# endif
    long longreadlen;
    int binmode;
	int fetched;
	odbc_connection *conn_ptr;
} odbc_result;

typedef struct {
	char *defDB;
	char *defUser;
	char *defPW;
	long allow_persistent;
	long check_persistent;
	long max_persistent;
	long max_links;
	long num_persistent;
	long num_links;
	int defConn;
    long defaultlrl;
    long defaultbinmode;
	HashTable *resource_list;
	HashTable *resource_plist;
} php_odbc_globals;

int odbc_add_result(HashTable *list, odbc_result *result);
odbc_result *odbc_get_result(HashTable *list, int count);
void odbc_del_result(HashTable *list, int count);
int odbc_add_conn(HashTable *list, HDBC conn);
odbc_connection *odbc_get_conn(HashTable *list, int count);
void odbc_del_conn(HashTable *list, int ind);
int odbc_bindcols(odbc_result *result);

#if defined( HAVE_IBMDB2 ) || defined( HAVE_UNIXODBC )
#define ODBC_SQL_ERROR_PARAMS SQLHANDLE henv, SQLHANDLE conn, SQLHANDLE stmt, char *func
#else
#define ODBC_SQL_ERROR_PARAMS HENV henv, HDBC conn, HSTMT stmt, char *func
#endif

void odbc_sql_error(ODBC_SQL_ERROR_PARAMS);

#define IS_SQL_LONG(x) (x == SQL_LONGVARBINARY || x == SQL_LONGVARCHAR)
#define IS_SQL_BINARY(x) (x == SQL_BINARY || x == SQL_VARBINARY || x == SQL_LONGVARBINARY)

#ifdef ZTS
# define ODBCLS_D	php_odbc_globals *odbc_globals
# define ODBCLS_DC	, ODBCLS_D
# define ODBCLS_C	odbc_globals
# define ODBCLS_CC , ODBCLS_C
# define ODBCG(v) (odbc_globals->v)
# define ODBCLS_FETCH()	php_odbc_globals *odbc_globals = ts_resource(odbc_globals_id)
#else
# define ODBCLS_D
# define ODBCLS_DC
# define ODBCLS_C
# define ODBCLS_CC
# define ODBCG(v) (odbc_globals.v)
# define ODBCLS_FETCH()
extern ZEND_API php_odbc_globals odbc_globals;
#endif

#else

# define odbc_module_ptr NULL

#endif /* HAVE_UODBC */

#define phpext_odbc_ptr odbc_module_ptr

#endif /* PHP_ODBC_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
