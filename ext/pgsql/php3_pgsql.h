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
extern void php3_pgsql_connect(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pgsql_pconnect(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pgsql_close(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pgsql_dbname(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pgsql_error_message(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pgsql_options(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pgsql_port(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pgsql_tty(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pgsql_host(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pgsql_exec(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pgsql_num_rows(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pgsql_num_fields(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pgsql_cmdtuples(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pgsql_field_name(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pgsql_field_size(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pgsql_field_type(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pgsql_field_number(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pgsql_result(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pgsql_fetch_row(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pgsql_fetch_array(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pgsql_fetch_object(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pgsql_data_length(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pgsql_data_isnull(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pgsql_free_result(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pgsql_last_oid(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pgsql_lo_create(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pgsql_lo_unlink(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pgsql_lo_open(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pgsql_lo_close(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pgsql_lo_read(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pgsql_lo_write(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pgsql_lo_readall(INTERNAL_FUNCTION_PARAMETERS);

void php3_pgsql_do_connect(INTERNAL_FUNCTION_PARAMETERS,int persistent);
int php3_pgsql_get_default_link(INTERNAL_FUNCTION_PARAMETERS);
void php3_pgsql_get_link_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type);
void php3_pgsql_get_result_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type);
char *get_field_name(PGconn *pgsql, Oid oid, HashTable *list);
void php3_pgsql_get_field_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type);
void php3_pgsql_fetch_hash(INTERNAL_FUNCTION_PARAMETERS);
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

#endif /* _PHP3_PGSQL_H */
