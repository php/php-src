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
   | Authors: Jouni Ahto                                                  |
   +----------------------------------------------------------------------+
 */
 
/* $Id$ */

#ifndef _PHP3_PGSQL_H
#define _PHP3_PGSQL_H

#if COMPILE_DL
#undef HAVE_PGSQL
#define HAVE_PGSQL 1
#endif

#if HAVE_PGSQL

extern php3_module_entry pgsql_module_entry;
#define pgsql_module_ptr &pgsql_module_entry


#include <libpq-fe.h>
#include <libpq/libpq-fs.h>

extern int php3_minit_pgsql(INIT_FUNC_ARGS);
extern int php3_rinit_pgsql(INIT_FUNC_ARGS);
PHP_FUNCTION(pgsql_connect);
PHP_FUNCTION(pgsql_pconnect);
PHP_FUNCTION(pgsql_close);
PHP_FUNCTION(pgsql_dbname);
PHP_FUNCTION(pgsql_error_message);
PHP_FUNCTION(pgsql_options);
PHP_FUNCTION(pgsql_port);
PHP_FUNCTION(pgsql_tty);
PHP_FUNCTION(pgsql_host);
PHP_FUNCTION(pgsql_exec);
PHP_FUNCTION(pgsql_num_rows);
PHP_FUNCTION(pgsql_num_fields);
PHP_FUNCTION(pgsql_cmdtuples);
PHP_FUNCTION(pgsql_field_name);
PHP_FUNCTION(pgsql_field_size);
PHP_FUNCTION(pgsql_field_type);
PHP_FUNCTION(pgsql_field_number);
PHP_FUNCTION(pgsql_result);
PHP_FUNCTION(pgsql_fetch_row);
PHP_FUNCTION(pgsql_fetch_array);
PHP_FUNCTION(pgsql_fetch_object);
PHP_FUNCTION(pgsql_data_length);
PHP_FUNCTION(pgsql_data_isnull);
PHP_FUNCTION(pgsql_free_result);
PHP_FUNCTION(pgsql_last_oid);
PHP_FUNCTION(pgsql_lo_create);
PHP_FUNCTION(pgsql_lo_unlink);
PHP_FUNCTION(pgsql_lo_open);
PHP_FUNCTION(pgsql_lo_close);
PHP_FUNCTION(pgsql_lo_read);
PHP_FUNCTION(pgsql_lo_write);
PHP_FUNCTION(pgsql_lo_readall);

void php3_pgsql_do_connect(INTERNAL_FUNCTION_PARAMETERS,int persistent);
int php3_pgsql_get_default_link(INTERNAL_FUNCTION_PARAMETERS);
void php3_pgsql_get_link_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type);
void php3_pgsql_get_result_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type);
char *get_field_name(PGconn *pgsql, Oid oid, HashTable *list);
void php3_pgsql_get_field_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type);
void php3_pgsql_data_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type);


typedef struct pgLofp {
        PGconn *conn;
	int lofd;
} pgLofp;

typedef struct _php3_pgsql_result_handle {
	PGconn *conn;
	PGresult *result;
} pgsql_result_handle;

typedef struct {
	long default_link;
	long num_links,num_persistent;
	long max_links,max_persistent;
	long allow_persistent;
	int le_link,le_plink,le_result,le_lofp,le_string;
} pgsql_module;

extern pgsql_module php3_pgsql_module;

#else

#define pgsql_module_ptr NULL

#endif

#define phpext_pgsql_ptr pgsql_module_ptr

#endif /* _PHP3_PGSQL_H */
