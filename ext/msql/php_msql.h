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
