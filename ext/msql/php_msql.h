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
   | Author: Zeev Suraski <zeev@zend.com>                                 |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_MSQL_H
#define PHP_MSQL_H

#if HAVE_MSQL

extern zend_module_entry msql_module_entry;
#define msql_module_ptr &msql_module_entry

/* mSQL functions */
PHP_MINIT_FUNCTION(msql);
PHP_RINIT_FUNCTION(msql);
PHP_MSHUTDOWN_FUNCTION(msql);
PHP_MINFO_FUNCTION(msql);

PHP_FUNCTION(msql_connect);
PHP_FUNCTION(msql_pconnect);
PHP_FUNCTION(msql_close);
PHP_FUNCTION(msql_select_db);
PHP_FUNCTION(msql_create_db);
PHP_FUNCTION(msql_drop_db);
PHP_FUNCTION(msql_list_dbs);
PHP_FUNCTION(msql_list_tables);
PHP_FUNCTION(msql_list_fields);
PHP_FUNCTION(msql_error);
PHP_FUNCTION(msql_affected_rows);
PHP_FUNCTION(msql_query);
PHP_FUNCTION(msql_db_query);
PHP_FUNCTION(msql_result);
PHP_FUNCTION(msql_num_rows);
PHP_FUNCTION(msql_num_fields);
PHP_FUNCTION(msql_fetch_row);
PHP_FUNCTION(msql_data_seek);
PHP_FUNCTION(msql_fetch_field);
PHP_FUNCTION(msql_field_seek);
PHP_FUNCTION(msql_free_result);
PHP_FUNCTION(msql_field_name);
PHP_FUNCTION(msql_field_table);
PHP_FUNCTION(msql_field_len);
PHP_FUNCTION(msql_field_type);
PHP_FUNCTION(msql_field_flags);
PHP_FUNCTION(msql_fetch_array);
PHP_FUNCTION(msql_fetch_object);

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
