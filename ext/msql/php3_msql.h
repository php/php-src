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
extern DLEXPORT void php3_msql_connect(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_msql_pconnect(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_msql_close(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_msql_select_db(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_msql_create_db(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_msql_drop_db(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_msql_list_dbs(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_msql_list_tables(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_msql_list_fields(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_msql_error(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_msql_affected_rows(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_msql_query(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_msql_db_query(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_msql_result(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_msql_num_rows(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_msql_num_fields(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_msql_fetch_row(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_msql_data_seek(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_msql_fetch_field(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_msql_field_seek(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_msql_free_result(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_msql_field_name(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_msql_field_table(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_msql_field_len(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_msql_field_type(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_msql_field_flags(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_msql_fetch_array(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_msql_fetch_object(INTERNAL_FUNCTION_PARAMETERS);

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

#endif /* _PHP3_MSQL_H */
