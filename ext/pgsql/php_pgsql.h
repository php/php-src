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

#ifndef _PHP_PGSQL_H
#define _PHP_PGSQL_H

#if COMPILE_DL
#undef HAVE_PGSQL
#define HAVE_PGSQL 1
#endif

#if HAVE_PGSQL

extern zend_module_entry pgsql_module_entry;
#define pgsql_module_ptr &pgsql_module_entry


#include <libpq-fe.h>

#if (WIN32||WINNT)
#define INV_WRITE            0x00020000
#define INV_READ             0x00040000
#else
#include <libpq/libpq-fs.h>
#endif

#if WIN32||WINNT
#define PHP_PGSQL_API __declspec(dllexport)
#else
#define PHP_PGSQL_API
#endif

PHP_MINIT_FUNCTION(pgsql);
PHP_RINIT_FUNCTION(pgsql);
PHP_FUNCTION(pg_connect);
PHP_FUNCTION(pg_pconnect);
PHP_FUNCTION(pg_close);
PHP_FUNCTION(pg_dbname);
PHP_FUNCTION(pg_error_message);
PHP_FUNCTION(pg_options);
PHP_FUNCTION(pg_port);
PHP_FUNCTION(pg_tty);
PHP_FUNCTION(pg_host);
PHP_FUNCTION(pg_exec);
PHP_FUNCTION(pg_num_rows);
PHP_FUNCTION(pg_num_fields);
PHP_FUNCTION(pg_cmdtuples);
PHP_FUNCTION(pg_field_name);
PHP_FUNCTION(pg_field_size);
PHP_FUNCTION(pg_field_type);
PHP_FUNCTION(pg_field_number);
PHP_FUNCTION(pg_result);
PHP_FUNCTION(pg_fetch_row);
PHP_FUNCTION(pg_fetch_array);
PHP_FUNCTION(pg_fetch_object);
PHP_FUNCTION(pg_data_length);
PHP_FUNCTION(pg_data_isnull);
PHP_FUNCTION(pg_free_result);
PHP_FUNCTION(pg_last_oid);
PHP_FUNCTION(pg_lo_create);
PHP_FUNCTION(pg_lo_unlink);
PHP_FUNCTION(pg_lo_open);
PHP_FUNCTION(pg_lo_close);
PHP_FUNCTION(pg_lo_read);
PHP_FUNCTION(pg_lo_write);
PHP_FUNCTION(pg_lo_readall);

void php_pgsql_do_connect(INTERNAL_FUNCTION_PARAMETERS,int persistent);
int php_pgsql_get_default_link(INTERNAL_FUNCTION_PARAMETERS);
void php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type);
void php_pgsql_get_result_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type);
char *get_field_name(PGconn *pgsql, Oid oid, HashTable *list);
void php_pgsql_get_field_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type);
void php_pgsql_data_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type);


typedef struct pgLofp {
        PGconn *conn;
	int lofd;
} pgLofp;

typedef struct _php_pgsql_result_handle {
	PGconn *conn;
	PGresult *result;
} pgsql_result_handle;

typedef struct {
	long default_link;
	long num_links,num_persistent;
	long max_links,max_persistent;
	long allow_persistent;
	int le_lofp,le_string;
} php_pgsql_globals;


#ifdef ZTS
# define PGLS_D	php_pgsql_globals *pgsql_globals
# define PGLS_DC	, PGLS_D
# define PGLS_C	pgsql_globals
# define PGLS_CC , PGLS_C
# define PGG(v) (pgsql_globals->v)
# define PGLS_FETCH()	php_pgsql_globals *pgsql_globals = ts_resource(pgsql_globals_id)
#else
# define PGLS_D
# define PGLS_DC
# define PGLS_C
# define PGLS_CC
# define PGG(v) (pgsql_globals.v)
# define PGLS_FETCH()
extern PHP_PGSQL_API php_pgsql_globals pgsql_globals;
#endif

#else

#define pgsql_module_ptr NULL

#endif

#define phpext_pgsql_ptr pgsql_module_ptr

#endif /* _PHP_PGSQL_H */
