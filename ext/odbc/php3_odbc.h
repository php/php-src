/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Stig Sæther Bakken <ssb@guardian.no>                        |
   |          Andreas Karajannis <Andreas.Karajannis@gmd.de>              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef _PHP_ODBC_H
# define _PHP_ODBC_H

# if HAVE_UODBC

# ifndef MSVC5
#  define FAR
# endif

/* checking in the same order as in configure.in */

# if HAVE_SOLID /* Solid Server */

#  include <cli0core.h>
#  include <cli0ext1.h>
#  define HAVE_SQL_EXTENDED_FETCH 0
extern void php3_solid_fetch_prev(INTERNAL_FUNCTION_PARAMETERS);

# elif HAVE_EMPRESS /* Empress */

#  include <sql.h>
#  include <sqlext.h>
#  define HAVE_SQL_EXTENDED_FETCH 0

# elif HAVE_ADABAS /* Adabas D */

#  include <WINDOWS.H>
#  include <sql.h>
#  include <sqlext.h>
#  define HAVE_SQL_EXTENDED_FETCH 1

# elif HAVE_IODBC /* iODBC library */

#  include <isql.h>
#  include <isqlext.h>
#  include <odbc_types.h>
#  include <odbc_funcs.h>
#  define HAVE_SQL_EXTENDED_FETCH 1
#  define SQL_FD_FETCH_ABSOLUTE   0x00000010L
#  define SQL_CURSOR_DYNAMIC      2UL
#  define SQL_NO_TOTAL            (-4)
#  define SQL_SO_DYNAMIC          0x00000004L
#  define SQL_LEN_DATA_AT_EXEC_OFFSET  (-100)
#  define SQL_LEN_DATA_AT_EXEC(length) (-(length)+SQL_LEN_DATA_AT_EXEC_OFFSET)

# elif HAVE_OPENLINK /* OpenLink ODBC drivers */

#  include <iodbc.h>
#  include <isql.h>
#  include <isqlext.h>
#  include <udbcext.h>
#  define HAVE_SQL_EXTENDED_FETCH 1

# elif HAVE_VELOCIS /* Raima Velocis */

#  define UNIX
#  define HAVE_SQL_EXTENDED_FETCH 1
#  include <sql.h>
#  include <sqlext.h>

# elif HAVE_CODBC /* Custom ODBC */

#  define HAVE_SQL_EXTENDED_FETCH 1
#  include <odbc.h>

# elif HAVE_DB2 /* DB2 CLI */

#  define HAVE_SQL_EXTENDED_FETCH 1
#  include <sqlcli1.h>
#  ifdef DB268K
/* Need to include ASLM for 68K applications */
#   include <LibraryManager.h>
#  endif

# else /* MS ODBC */

#  define HAVE_SQL_EXTENDED_FETCH 1
#  include <WINDOWS.H>
#  include <sql.h>
#  include <sqlext.h>
# endif

extern php3_module_entry odbc_module_entry;
# define odbc_module_ptr &odbc_module_entry


/* user functions */
extern int php3_minit_odbc(INIT_FUNC_ARGS);
extern int php3_mshutdown_odbc(SHUTDOWN_FUNC_ARGS);
extern int php3_rinit_odbc(INIT_FUNC_ARGS);
extern void php3_info_odbc(ZEND_MODULE_INFO_FUNC_ARGS);
extern PHP_FUNCTION(odbc_setoption);
extern PHP_FUNCTION(odbc_autocommit);
extern PHP_FUNCTION(odbc_close);
extern PHP_FUNCTION(odbc_close_all);
extern PHP_FUNCTION(odbc_commit);
extern PHP_FUNCTION(odbc_connect);
extern PHP_FUNCTION(odbc_pconnect);
extern void php3_odbc_do_connect(INTERNAL_FUNCTION_PARAMETERS, int);
extern PHP_FUNCTION(odbc_cursor);
extern PHP_FUNCTION(odbc_exec);
extern PHP_FUNCTION(odbc_do);
extern PHP_FUNCTION(odbc_execute);
extern PHP_FUNCTION(odbc_fetch_into);
extern PHP_FUNCTION(odbc_fetch_row);
extern PHP_FUNCTION(odbc_field_len);
extern PHP_FUNCTION(odbc_field_name);
extern PHP_FUNCTION(odbc_field_type);
extern PHP_FUNCTION(odbc_field_num);
extern PHP_FUNCTION(odbc_free_result);
extern PHP_FUNCTION(odbc_num_fields);
extern PHP_FUNCTION(odbc_num_rows);
extern PHP_FUNCTION(odbc_prepare);
extern PHP_FUNCTION(odbc_result);
extern PHP_FUNCTION(odbc_result_all);
extern PHP_FUNCTION(odbc_rollback);
extern void php3_odbc_transact(INTERNAL_FUNCTION_PARAMETERS, int);
extern PHP_FUNCTION(odbc_binmode);
extern PHP_FUNCTION(odbc_longreadlen);

typedef struct odbc_connection {
#if HAVE_DB2
	SQLHANDLE hdbc;
#else
	HDBC hdbc;
#endif
	int open;
} odbc_connection;

typedef struct odbc_result_value {
	char name[32];
	char *value;
	long int vallen;
	SDWORD coltype;
} odbc_result_value;

typedef struct odbc_result {
#if HAVE_DB2
	SQLHANDLE stmt;
#else
	HSTMT stmt;
#endif
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
#if HAVE_DB2
	SQLHANDLE henv;
#else
	HENV henv;
#endif
	char *defDB;
	char *defUser;
	char *defPW;
	long allow_persistent;
	long max_persistent;
	long max_links;
	long num_persistent;
	long num_links;
	int defConn;
	int le_result, le_conn, le_pconn;
    long defaultlrl;
    long defaultbinmode;
	HashTable *resource_list;
	HashTable *resource_plist;
} php_odbc_globals;

# ifndef ZTS
extern php_odbc_globals odbc_globals;
# endif

int odbc_add_result(HashTable *list, odbc_result *result);
odbc_result *odbc_get_result(HashTable *list, int count);
void odbc_del_result(HashTable *list, int count);
int odbc_add_conn(HashTable *list, HDBC conn);
odbc_connection *odbc_get_conn(HashTable *list, int count);
void odbc_del_conn(HashTable *list, int ind);
#if HAVE_DB2
void odbc_sql_error(SQLHANDLE conn, SQLHANDLE stmt, char *func);
#else
void odbc_sql_error(HDBC conn, HSTMT stmt, char *func);
#endif
int odbc_bindcols(odbc_result *result);

#define IS_SQL_LONG(x) (x == SQL_LONGVARBINARY || x == SQL_LONGVARCHAR)
#define IS_SQL_BINARY(x) (x == SQL_BINARY || x == SQL_VARBINARY || x == SQL_LONGVARBINARY)

#ifdef ZTS
# define ODBCLS_D	php_odbc_globals *odbc_globals
# define ODBCLS_DC	, ODBCLS_D
# define ODBCLS_C	odbc_globals
# define ODBCLS_CC , ODBCLS_C
# define ODBCG(v) (odbc_globals->v)
# define ODBCLS_FETCH()	php_odbc_globals *odbc_globals = ts_resource(odbc_globals_id)
# define ODBC_TLS_VARS char *globals; ODBCLS_FETCH(); globals = (char *)odbc_globals;
#else
# define ODBCLS_D
# define ODBCLS_DC
# define ODBCLS_C
# define ODBCLS_CC
# define ODBCG(v) (odbc_globals.v)
# define ODBCLS_FETCH()
# define ODBC_TLS_VARS char *globals = (char *)&odbc_globals
extern ZEND_API php_odbc_globals odbc_globals;
#endif

#else

# define odbc_module_ptr NULL

#endif /* HAVE_UODBC */

#endif /* _PHP_ODBC_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
