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
   | Authors: Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */


/* $Id$ */

#ifndef _PHP3_MSQL_H
#define _PHP3_MSQL_H

#if COMPILE_DL
#undef HAVE_MSQL
#define HAVE_MSQL 1
#define php3_minit_msql dl_init
#endif

#if HAVE_MSQL

extern php3_module_entry msql_module_entry;
#define msql_module_ptr &msql_module_entry

/* mSQL functions */
extern DLEXPORT int php3_minit_msql(INIT_FUNC_ARGS);
extern DLEXPORT int php3_rinit_msql(INIT_FUNC_ARGS);
extern DLEXPORT int php3_mshutdown_msql(SHUTDOWN_FUNC_ARGS);
extern DLEXPORT void php3_info_msql(ZEND_MODULE_INFO_FUNC_ARGS);
extern DLEXPORT PHP_FUNCTION(msql_connect);
extern DLEXPORT PHP_FUNCTION(msql_pconnect);
extern DLEXPORT PHP_FUNCTION(msql_close);
extern DLEXPORT PHP_FUNCTION(msql_select_db);
extern DLEXPORT PHP_FUNCTION(msql_create_db);
extern DLEXPORT PHP_FUNCTION(msql_drop_db);
extern DLEXPORT PHP_FUNCTION(msql_list_dbs);
extern DLEXPORT PHP_FUNCTION(msql_list_tables);
extern DLEXPORT PHP_FUNCTION(msql_list_fields);
extern DLEXPORT PHP_FUNCTION(msql_error);
extern DLEXPORT PHP_FUNCTION(msql_affected_rows);
extern DLEXPORT PHP_FUNCTION(msql_query);
extern DLEXPORT PHP_FUNCTION(msql_db_query);
extern DLEXPORT PHP_FUNCTION(msql_result);
extern DLEXPORT PHP_FUNCTION(msql_num_rows);
extern DLEXPORT PHP_FUNCTION(msql_num_fields);
extern DLEXPORT PHP_FUNCTION(msql_fetch_row);
extern DLEXPORT PHP_FUNCTION(msql_data_seek);
extern DLEXPORT PHP_FUNCTION(msql_fetch_field);
extern DLEXPORT PHP_FUNCTION(msql_field_seek);
extern DLEXPORT PHP_FUNCTION(msql_free_result);
extern DLEXPORT PHP_FUNCTION(msql_field_name);
extern DLEXPORT PHP_FUNCTION(msql_field_table);
extern DLEXPORT PHP_FUNCTION(msql_field_len);
extern DLEXPORT PHP_FUNCTION(msql_field_type);
extern DLEXPORT PHP_FUNCTION(msql_field_flags);
extern DLEXPORT PHP_FUNCTION(msql_fetch_array);
extern DLEXPORT PHP_FUNCTION(msql_fetch_object);

typedef struct {
	long default_link;
	long num_links,num_persistent;
	long max_links,max_persistent;
	long allow_persistent;
	int le_query;
	int le_link;
	int le_plink;
} msql_module;

#ifndef THREAD_SAFE
extern msql_module php3_msql_module;
#endif
#else

#define msql_module_ptr NULL

#endif

#define phpext_msql_ptr msql_module_ptr

#endif /* _PHP3_MSQL_H */
