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
   | Authors: Stig Sæther Bakken <ssb@php.net>                            |
   |          Andreas Karajannis <Andreas.Karajannis@gmd.de>              |
   |	      Kevin N. Shallow <kshallow@tampabay.rr.com>                 |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_ODBC_INCLUDES_H
#define PHP_ODBC_INCLUDES_H

#ifdef HAVE_UODBC

/* checking in the same order as in configure.ac */

#if defined(HAVE_IODBC) /* iODBC library */

#define ODBC_TYPE "iODBC"
#include <sql.h>
#include <sqlext.h>
#include <iodbcext.h>

#elif defined(HAVE_UNIXODBC) /* unixODBC library */

#define ODBC_TYPE "unixODBC"
#include <sql.h>
#include <sqlext.h>

#elif defined(HAVE_CODBC) /* Custom ODBC */

#define ODBC_TYPE "Custom ODBC"
#include <odbc.h>

#elif defined(HAVE_IBMDB2) /* DB2 CLI */

#define ODBC_TYPE "IBM DB2 CLI"
#include <sqlcli1.h>

#else /* MS ODBC */

#include <WINDOWS.H>
#include <sql.h>
#include <sqlext.h>
#endif

#ifdef PHP_WIN32
#include <winsock2.h>

#define ODBC_TYPE "Win32"
#define PHP_ODBC_TYPE ODBC_TYPE

#endif

/* Common defines */

#if defined( HAVE_IBMDB2 ) || defined( HAVE_UNIXODBC ) || defined (HAVE_IODBC)
#define ODBC_SQL_ENV_T SQLHANDLE
#define ODBC_SQL_CONN_T SQLHANDLE
#define ODBC_SQL_STMT_T SQLHANDLE
#else
#define ODBC_SQL_ENV_T HENV
#define ODBC_SQL_CONN_T HDBC
#define ODBC_SQL_STMT_T HSTMT
#endif

typedef struct odbc_connection {
	ODBC_SQL_ENV_T henv;
	ODBC_SQL_CONN_T hdbc;
	char laststate[6];
	char lasterrormsg[SQL_MAX_MESSAGE_LENGTH];
	HashTable results;
} odbc_connection;

typedef struct odbc_link {
	odbc_connection *connection;
	zend_string *hash;
	bool persistent;
	zend_object std;
} odbc_link;

typedef struct odbc_result_value {
	char name[256];
	char *value;
	SQLLEN vallen;
	SQLLEN coltype;
} odbc_result_value;

typedef struct odbc_param_info {
	SQLSMALLINT sqltype;
	SQLSMALLINT scale;
	SQLSMALLINT nullable;
	SQLULEN precision;
} odbc_param_info;

typedef struct odbc_result {
	ODBC_SQL_STMT_T stmt;
	odbc_result_value *values;
	SQLSMALLINT numcols;
	SQLSMALLINT numparams;
	int fetch_abs;
	zend_long longreadlen;
	int binmode;
	int fetched;
	odbc_param_info *param_info;
	odbc_connection *conn_ptr;
	uint32_t index;
	zend_object std;
} odbc_result;

ZEND_BEGIN_MODULE_GLOBALS(odbc)
	bool allow_persistent;
	bool check_persistent;
	zend_long max_persistent;
	zend_long max_links;
	zend_long num_persistent;
	zend_long num_links;
	zend_long defaultlrl;
	zend_long defaultbinmode;
	zend_long default_cursortype;
	char laststate[6];
	char lasterrormsg[SQL_MAX_MESSAGE_LENGTH];
	/* Stores ODBC links throughout the duration of a request. The connection member may be either persistent or
	 * non-persistent. In the former case, it is a pointer to an item in EG(persistent_list). This solution makes it
	 * possible to properly free links during RSHUTDOWN (or when they are explicitly closed), while persistent
	 * connections themselves are going to be freed later during the shutdown process (or when they are explicitly
	 * closed).
	 */
	HashTable connections;
ZEND_END_MODULE_GLOBALS(odbc)

int odbc_add_result(HashTable *list, odbc_result *result);
odbc_result *odbc_get_result(HashTable *list, int count);
void odbc_del_result(HashTable *list, int count);
int odbc_add_conn(HashTable *list, HDBC conn);
odbc_connection *odbc_get_conn(HashTable *list, int count);
void odbc_del_conn(HashTable *list, int ind);
void odbc_bindcols(odbc_result *result);

#define ODBC_SQL_ERROR_PARAMS odbc_connection *conn_resource, ODBC_SQL_STMT_T stmt, char *func

void odbc_sql_error(ODBC_SQL_ERROR_PARAMS);

#define IS_SQL_LONG(x) (x == SQL_LONGVARBINARY || x == SQL_LONGVARCHAR || x == SQL_WLONGVARCHAR)

#define IS_SQL_BINARY(x) (x == SQL_BINARY || x == SQL_VARBINARY || x == SQL_LONGVARBINARY)

PHP_ODBC_API ZEND_EXTERN_MODULE_GLOBALS(odbc)
#define ODBCG(v) ZEND_MODULE_GLOBALS_ACCESSOR(odbc, v)

#if defined(ZTS) && defined(COMPILE_DL_ODBC)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif /* HAVE_UODBC */
#endif /* PHP_ODBC_INCLUDES_H */
