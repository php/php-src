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

PHP_MINIT_FUNCTION(fbsql);
PHP_MSHUTDOWN_FUNCTION(fbsql);
PHP_RINIT_FUNCTION(fbsql);
PHP_RSHUTDOWN_FUNCTION(fbsql);
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
PHP_FUNCTION(fbsql_table_name);

PHP_FUNCTION(fbsql_set_transaction);
PHP_FUNCTION(fbsql_autocommit);
PHP_FUNCTION(fbsql_commit);
PHP_FUNCTION(fbsql_rollback);

PHP_FUNCTION(fbsql_create_blob);
PHP_FUNCTION(fbsql_create_clob);
PHP_FUNCTION(fbsql_set_lob_mode);
PHP_FUNCTION(fbsql_read_blob);
PHP_FUNCTION(fbsql_read_clob);
PHP_FUNCTION(fbsql_blob_size);
PHP_FUNCTION(fbsql_clob_size);

PHP_FUNCTION(fbsql_hostname);
PHP_FUNCTION(fbsql_database);
PHP_FUNCTION(fbsql_database_password);
PHP_FUNCTION(fbsql_username);
PHP_FUNCTION(fbsql_password);
PHP_FUNCTION(fbsql_warnings);

PHP_FUNCTION(fbsql_get_autostart_info);
/* PHP_FUNCTION(fbsql_set_autostart_info); */

static void php_fbsql_fetch_hash(INTERNAL_FUNCTION_PARAMETERS, int result_type);
static void php_fbsql_do_connect(INTERNAL_FUNCTION_PARAMETERS, int persistent);

ZEND_BEGIN_MODULE_GLOBALS(fbsql)
   zend_bool allowPersistent;
   zend_bool generateWarnings;
   zend_bool autoCommit;
   long maxPersistent;
   long maxLinks;
   long maxConnections;
   long maxResults;
   long batchSize;
   char *hostName;
   char *databaseName;
   char *databasePassword;
   char *userName;
   char *userPassword;
   long persistentCount;
   long linkCount;
   long linkIndex;

ZEND_END_MODULE_GLOBALS(fbsql)

#ifdef ZTS
# define FB_SQL_G(v) TSRMG(fbsql_globals_id, zend_fbsql_globals *, v)
#else
# define FB_SQL_G(v)	(fbsql_globals.v)
#endif

/*#ifndef ZTS  /* No need for external definitions */
/*extern fbsql_module* phpfbModule; */
/*#endif */

#else  /*  HAVE_FBSQL */

#define fbsql_module_ptr NULL
#error not ok
#endif /*  HAVE_FBSQL */

#define phpext_fbsql_ptr fbsql_module_ptr

#endif /* _PHP_FBSQL_H */
