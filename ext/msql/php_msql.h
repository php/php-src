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
   | Authors: Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_MSQL_H
#define PHP_MSQL_H

#if HAVE_MSQL

extern zend_module_entry msql_module_entry;
#define msql_module_ptr &msql_module_entry

/* mSQL functions */
DLEXPORT PHP_MINIT_FUNCTION(msql);
DLEXPORT PHP_RINIT_FUNCTION(msql);
DLEXPORT PHP_MSHUTDOWN_FUNCTION(msql);
DLEXPORT PHP_MINFO_FUNCTION(msql);

DLEXPORT PHP_FUNCTION(msql_connect);
DLEXPORT PHP_FUNCTION(msql_pconnect);
DLEXPORT PHP_FUNCTION(msql_close);
DLEXPORT PHP_FUNCTION(msql_select_db);
DLEXPORT PHP_FUNCTION(msql_create_db);
DLEXPORT PHP_FUNCTION(msql_drop_db);
DLEXPORT PHP_FUNCTION(msql_list_dbs);
DLEXPORT PHP_FUNCTION(msql_list_tables);
DLEXPORT PHP_FUNCTION(msql_list_fields);
DLEXPORT PHP_FUNCTION(msql_error);
DLEXPORT PHP_FUNCTION(msql_affected_rows);
DLEXPORT PHP_FUNCTION(msql_query);
DLEXPORT PHP_FUNCTION(msql_db_query);
DLEXPORT PHP_FUNCTION(msql_result);
DLEXPORT PHP_FUNCTION(msql_num_rows);
DLEXPORT PHP_FUNCTION(msql_num_fields);
DLEXPORT PHP_FUNCTION(msql_fetch_row);
DLEXPORT PHP_FUNCTION(msql_data_seek);
DLEXPORT PHP_FUNCTION(msql_fetch_field);
DLEXPORT PHP_FUNCTION(msql_field_seek);
DLEXPORT PHP_FUNCTION(msql_free_result);
DLEXPORT PHP_FUNCTION(msql_field_name);
DLEXPORT PHP_FUNCTION(msql_field_table);
DLEXPORT PHP_FUNCTION(msql_field_len);
DLEXPORT PHP_FUNCTION(msql_field_type);
DLEXPORT PHP_FUNCTION(msql_field_flags);
DLEXPORT PHP_FUNCTION(msql_fetch_array);
DLEXPORT PHP_FUNCTION(msql_fetch_object);

typedef struct {
	long default_link;
	long num_links,num_persistent;
	long max_links,max_persistent;
	long allow_persistent;
	int le_query;
	int le_link;
	int le_plink;
} php_msql_globals;

#else

#define msql_module_ptr NULL

#endif

#define phpext_msql_ptr msql_module_ptr

#endif /* PHP_MSQL_H */
