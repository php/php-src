/* 
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Frank M. Kromann <frank@frontbase.com>                       |
   +----------------------------------------------------------------------+
 */


/* $Id$ */

#ifndef PHP_MSSQL_H
#define PHP_MSSQL_H


#if HAVE_MSSQL
#define MSDBLIB
#ifdef PHP_WIN32
#define PHP_MSSQL_API __declspec(dllexport)
#else
#define PHP_MSSQL_API
#endif


#define MSSQL_VERSION "7.0"
#include "sqlfront.h"
#include "sqldb.h"

#if HAVE_FREETDS
#define SQLTEXT SYBTEXT
#define SQLCHAR SYBCHAR
#define SQLVARCHAR SYBVARCHAR
#define SQLINT1 SYBINT1
#define SQLINT2 SYBINT2
#define SQLINT4 SYBINT4
#define SQLINTN SYBINTN
#define SQLBIT SYBBIT
#define SQLFLT4 SYBREAL
#define SQLFLT8 SYBFLT8
#define SQLFLTN SYBFLTN
#define SQLDECIMAL SYBDECIMAL
#define SQLNUMERIC SYBNUMERIC
#define SQLDATETIME SYBDATETIME
#define SQLDATETIM4 SYBDATETIME4
#define SQLDATETIMN SYBDATETIMN
#define SQLMONEY SYBMONEY
#define SQLMONEY4 SYBMONEY4
#define SQLMONEYN SYBMONEYN
#define SQLIMAGE SYBIMAGE
#define SQLBINARY SYBBINARY
#define SQLVARBINARY SYBVARBINARY
#define DBERRHANDLE(a, b) dberrhandle(b)
#define DBMSGHANDLE(a, b) dbmsghandle(b)
#define DBSETOPT(a, b, c) dbsetopt(a, b, c, -1)
#define NO_MORE_RPC_RESULTS 3
#define dbfreelogin dbloginfree
#define dbrpcexec dbrpcsend
typedef unsigned char	*LPBYTE;
typedef float           DBFLT4;
#else
#define DBERRHANDLE(a, b) dbprocerrhandle(a, b)
#define DBMSGHANDLE(a, b) dbprocmsghandle(a, b)
#define EHANDLEFUNC DBERRHANDLE_PROC
#define MHANDLEFUNC DBMSGHANDLE_PROC
#define DBSETOPT(a, b, c) dbsetopt(a, b, c)
#endif

#define coltype(j) dbcoltype(mssql_ptr->link,j)
#define intcol(i) ((int) *(DBINT *) dbdata(mssql_ptr->link,i))
#define smallintcol(i) ((int) *(DBSMALLINT *) dbdata(mssql_ptr->link,i))
#define tinyintcol(i) ((int) *(DBTINYINT *) dbdata(mssql_ptr->link,i))
#define anyintcol(j) (coltype(j)==SQLINT4?intcol(j):(coltype(j)==SQLINT2?smallintcol(j):tinyintcol(j)))
#define charcol(i) ((DBCHAR *) dbdata(mssql_ptr->link,i))
#define floatcol4(i) (*(DBFLT4 *) dbdata(mssql_ptr->link,i))
#define floatcol8(i) (*(DBFLT8 *) dbdata(mssql_ptr->link,i))

#ifdef ZTS
#include "TSRM.h"
#endif

extern zend_module_entry mssql_module_entry;
#define mssql_module_ptr &mssql_module_entry

PHP_MINIT_FUNCTION(mssql);
PHP_MSHUTDOWN_FUNCTION(mssql);
PHP_RINIT_FUNCTION(mssql);
PHP_RSHUTDOWN_FUNCTION(mssql);
PHP_MINFO_FUNCTION(mssql);

PHP_FUNCTION(mssql_connect);
PHP_FUNCTION(mssql_pconnect);
PHP_FUNCTION(mssql_close);
PHP_FUNCTION(mssql_select_db);
PHP_FUNCTION(mssql_query);
PHP_FUNCTION(mssql_fetch_batch);
PHP_FUNCTION(mssql_rows_affected);
PHP_FUNCTION(mssql_free_result);
PHP_FUNCTION(mssql_get_last_message);
PHP_FUNCTION(mssql_num_rows);
PHP_FUNCTION(mssql_num_fields);
PHP_FUNCTION(mssql_fetch_field);
PHP_FUNCTION(mssql_fetch_row);
PHP_FUNCTION(mssql_fetch_array);
PHP_FUNCTION(mssql_fetch_assoc);
PHP_FUNCTION(mssql_fetch_object);
PHP_FUNCTION(mssql_field_length);
PHP_FUNCTION(mssql_field_name);
PHP_FUNCTION(mssql_field_type);
PHP_FUNCTION(mssql_data_seek);
PHP_FUNCTION(mssql_field_seek);
PHP_FUNCTION(mssql_result);
PHP_FUNCTION(mssql_next_result);
PHP_FUNCTION(mssql_min_error_severity);
PHP_FUNCTION(mssql_min_message_severity);
PHP_FUNCTION(mssql_init);
PHP_FUNCTION(mssql_bind);
PHP_FUNCTION(mssql_execute);
PHP_FUNCTION(mssql_free_statement);
PHP_FUNCTION(mssql_guid_string);

typedef struct mssql_link {
	LOGINREC *login;
	DBPROCESS *link;
	int valid;
} mssql_link;

typedef struct mssql_statement {
	int id;
	mssql_link *link;
	HashTable *binds;
	int executed;
} mssql_statement;

typedef struct {
	
	zval *zval;
	/* TODO: more data for special types (BLOBS, NUMERIC...) */
} mssql_bind;

ZEND_BEGIN_MODULE_GLOBALS(mssql)
	long default_link;
	long num_links,num_persistent;
	long max_links,max_persistent;
	zend_bool allow_persistent;
	char *appname;
	char *server_message;
	long min_error_severity, min_message_severity;
	long cfg_min_error_severity, cfg_min_message_severity;
	long connect_timeout, timeout;
	zend_bool compatability_mode;
	void (*get_column_content)(mssql_link *mssql_ptr,int offset,pval *result,int column_type  TSRMLS_DC);
	long textsize, textlimit, batchsize;
	zend_bool datetimeconvert;
	HashTable *resource_list, *resource_plist;
	zend_bool secure_connection;
	long max_procs;
ZEND_END_MODULE_GLOBALS(mssql)

#define MSSQL_ROWS_BLOCK 128

typedef struct mssql_field {
	char *name,*column_source;
	long max_length; 
	int numeric;
	int type;
} mssql_field;

typedef struct mssql_result {
	pval **data;
	mssql_field *fields;
	mssql_link *mssql_ptr;
	mssql_statement * statement;
	int batchsize;
	int lastresult;
	int blocks_initialized;
	int cur_row,cur_field;
	int num_rows,num_fields;
} mssql_result;


#ifdef ZTS
# define MS_SQL_G(v) TSRMG(mssql_globals_id, zend_mssql_globals *, v)
#else
# define MS_SQL_G(v)	(mssql_globals.v)
#endif

#else

#define mssql_module_ptr NULL

#endif /* HAVE_MSSQL */

#define phpext_mssql_ptr mssql_module_ptr

#endif /* PHP_MSSQL_H */
