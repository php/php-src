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

#ifndef _PHP3_MYSQL_H
#define _PHP3_MYSQL_H

#if COMPILE_DL
#undef HAVE_MYSQL
#define HAVE_MYSQL 1
#	if WIN32||WINNT
#	define PHP_MYSQL_API __declspec(dllexport)
#	endif
#endif

#if HAVE_MYSQL
#ifndef DLEXPORT
#define DLEXPORT
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

extern php3_module_entry mysql_module_entry;
#define mysql_module_ptr &mysql_module_entry

extern int php3_minit_mysql(INIT_FUNC_ARGS);
extern int php3_rinit_mysql(INIT_FUNC_ARGS);
extern int php3_mshutdown_mysql(SHUTDOWN_FUNC_ARGS);
void php3_info_mysql(ZEND_MODULE_INFO_FUNC_ARGS);
PHP_FUNCTION(mysql_connect);
PHP_FUNCTION(mysql_pconnect);
PHP_FUNCTION(mysql_close);
PHP_FUNCTION(mysql_select_db);
PHP_FUNCTION(mysql_create_db);
PHP_FUNCTION(mysql_drop_db);
PHP_FUNCTION(mysql_query);
PHP_FUNCTION(mysql_db_query);
PHP_FUNCTION(mysql_list_dbs);
PHP_FUNCTION(mysql_list_tables);
PHP_FUNCTION(mysql_list_fields);
PHP_FUNCTION(mysql_error);
PHP_FUNCTION(mysql_errno);
PHP_FUNCTION(mysql_affected_rows);
PHP_FUNCTION(mysql_insert_id);
PHP_FUNCTION(mysql_result);
PHP_FUNCTION(mysql_num_rows);
PHP_FUNCTION(mysql_num_fields);
PHP_FUNCTION(mysql_fetch_row);
PHP_FUNCTION(mysql_fetch_array);
PHP_FUNCTION(mysql_fetch_object);
PHP_FUNCTION(mysql_data_seek);
PHP_FUNCTION(mysql_fetch_lengths);
PHP_FUNCTION(mysql_fetch_field);
PHP_FUNCTION(mysql_field_seek);
PHP_FUNCTION(mysql_free_result);
PHP_FUNCTION(mysql_field_name);
PHP_FUNCTION(mysql_field_table);
PHP_FUNCTION(mysql_field_len);
PHP_FUNCTION(mysql_field_type);
PHP_FUNCTION(mysql_field_flags);

typedef struct {
	long default_link;
	long num_links,num_persistent;
	long max_links,max_persistent;
	long allow_persistent;
	long default_port;
	char *default_host, *default_user, *default_password;
} php_mysql_globals;


#ifdef ZTS
# define MySLS_D	php_mysql_globals *mysql_globals
# define MySLS_DC	, MySLS_D
# define MySLS_C	mysql_globals
# define MySLS_CC , MySLS_C
# define MySG(v) (mysql_globals->v)
# define MySLS_FETCH()	php_mysql_globals *mysql_globals = ts_resource(mysql_globals_id)
#else
# define MySLS_D
# define MySLS_DC
# define MySLS_C
# define MySLS_CC
# define MySG(v) (mysql_globals.v)
# define MySLS_FETCH()
extern PHP_MYSQL_API php_mysql_globals mysql_globals;
#endif


#else

#define mysql_module_ptr NULL

#endif

#define phpext_mysql_ptr mysql_module_ptr

#endif /* _PHP3_MYSQL_H */
