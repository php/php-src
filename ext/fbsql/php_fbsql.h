/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Frank M. Kromann frank@frontbase.com>                       |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#define HAVE_FBSQL 1

#ifndef _PHP_FBSQL_H
#define _PHP_FBSQL_H

#if COMPILE_DL_FBSQL
#undef HAVE_FBSQL
#define HAVE_FBSQL 1
#endif
#if HAVE_FBSQL

extern zend_module_entry fbsql_module_entry;
#define fbsql_module_ptr &fbsql_module_entry

#include <FBCAccess/FBCAccess.h>

extern PHP_MINIT_FUNCTION(fbsql);
extern PHP_MSHUTDOWN_FUNCTION(fbsql);
extern PHP_RINIT_FUNCTION(fbsql);
extern PHP_RSHUTDOWN_FUNCTION(fbsql);
PHP_MINFO_FUNCTION(fbsql);
PHP_FUNCTION(fbsql_connect);
PHP_FUNCTION(fbsql_pconnect);
PHP_FUNCTION(fbsql_close);
PHP_FUNCTION(fbsql_select_db);
PHP_FUNCTION(fbsql_change_user);
PHP_FUNCTION(fbsql_create_db);
PHP_FUNCTION(fbsql_drop_db);
PHP_FUNCTION(fbsql_start_db);
PHP_FUNCTION(fbsql_stop_db);
PHP_FUNCTION(fbsql_db_status);
PHP_FUNCTION(fbsql_query);
PHP_FUNCTION(fbsql_db_query);
PHP_FUNCTION(fbsql_list_dbs);
PHP_FUNCTION(fbsql_list_tables);
PHP_FUNCTION(fbsql_list_fields);
PHP_FUNCTION(fbsql_error);
PHP_FUNCTION(fbsql_errno);
PHP_FUNCTION(fbsql_affected_rows);
PHP_FUNCTION(fbsql_insert_id);
PHP_FUNCTION(fbsql_result);
PHP_FUNCTION(fbsql_next_result);
PHP_FUNCTION(fbsql_num_rows);
PHP_FUNCTION(fbsql_num_fields);
PHP_FUNCTION(fbsql_fetch_row);
PHP_FUNCTION(fbsql_fetch_array);
PHP_FUNCTION(fbsql_fetch_assoc);
PHP_FUNCTION(fbsql_fetch_object);
PHP_FUNCTION(fbsql_data_seek);
PHP_FUNCTION(fbsql_fetch_lengths);
PHP_FUNCTION(fbsql_fetch_field);
PHP_FUNCTION(fbsql_field_seek);
PHP_FUNCTION(fbsql_free_result);
PHP_FUNCTION(fbsql_field_name);
PHP_FUNCTION(fbsql_field_table);
PHP_FUNCTION(fbsql_field_len);
PHP_FUNCTION(fbsql_field_type);
PHP_FUNCTION(fbsql_field_flags);
PHP_FUNCTION(fbsql_autocommit);
PHP_FUNCTION(fbsql_commit);
PHP_FUNCTION(fbsql_rollback);
PHP_FUNCTION(fbsql_hostname);
PHP_FUNCTION(fbsql_database);
PHP_FUNCTION(fbsql_database_password);
PHP_FUNCTION(fbsql_username);
PHP_FUNCTION(fbsql_password);
PHP_FUNCTION(fbsql_warnings);

static void php_fbsql_fetch_hash(INTERNAL_FUNCTION_PARAMETERS, int result_type);

ZEND_BEGIN_MODULE_GLOBALS(fbsql)
   long allowPersistent;
   long generateWarnings;
   long autoCommit;
   long maxPersistant;
   long maxLinks;
   long maxConnections;
   long maxResults;
   long batchSize;
   char *hostName;
   char *databaseName;
   char *databasePassword;
   char *userName;
   char *userPassword;
   long persistantCount;
   long linkCount;
   long databaseCount;
   long resultCount;

   unsigned int linkIndex;
   int databaseIndex;
   int resultIndex;
ZEND_END_MODULE_GLOBALS(fbsql)

#ifdef ZTS
# define FBSQLLS_D		zend_fbsql_globals *fbsql_globals
# define FBSQLLS_DC		, FBSQLLS_D
# define FBSQLLS_C		fbsql_globals
# define FBSQLLS_CC		, FBSQLLS_C
# define FB_SQL_G(v)	(fbsql_globals->v)
# define FBSQLLS_FETCH()	zend_fbsql_globals *fbsql_globals = ts_resource(fbsql_globals_id)
#else
# define FBSQLLS_D
# define FBSQLLS_DC
# define FBSQLLS_C
# define FBSQLLS_CC
# define FB_SQL_G(v)	(fbsql_globals.v)
# define FBSQLLS_FETCH()
#endif



//#ifndef ZTS  // No need for external definitions
//extern fbsql_module* phpfbModule;
//#endif

#else  //  HAVE_FBSQL

#define fbsql_module_ptr NULL
#error not ok
#endif //  HAVE_FBSQL

#define phpext_fbsql_ptr fbsql_module_ptr

#endif /* _PHP_FBSQL_H */
