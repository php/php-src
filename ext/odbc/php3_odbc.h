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
   | Authors: Stig Sæther Bakken <ssb@guardian.no>                        |
   |          Andreas Karajannis <Andreas.Karajannis@gmd.de>              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef _UNIFIED_ODBC_H
#define _UNIFIED_ODBC_H

#if HAVE_UODBC

/*these defines are used to seperate the different
database modules that use the uodbc interface so that
two or more of them can be compiled into the same
php binary*/

/*
*
*  Defines for ODBC
*
*/
#if HAVE_UODBC
/* win32 vars for thread local storage */
#define UODBC_MUTEX uodbc_mutex
#define UODBC_TLS UODBCTls
#define UODBC_GLOBAL_STRUCT uodbc_global_struct
#define UODBC_GLOBALS uodbc_globals
/* defines for variables, structs, etc */
#define UODBC_MODULE_NAME "ODBC (uODBC)"
#define UODBC_FUNCTIONS uodbc_functions
#define UODBC_MODULE_ENTRY uodbc_module_entry
#define UODBC_CONNECTION uodbc_connection
#define UODBC_RESULT_VALUE uodbc_result_value
#define UODBC_RESULT uodbc_result
#define UODBC_MODULE uodbc_module
#define PHP3_UODBC_MODULE php3_uodbc_module

/* function defines */
#define PHP3_MINIT_UODBC php3_minit_uodbc
#define PHP3_MSHUTDOWN_UODBC php3_mshutdown_uodbc
#define PHP3_RINIT_UODBC php3_rinit_uodbc
#define PHP3_INFO_UODBC php3_info_uodbc
#define PHP3_UODBC_SETOPTION php3_uodbc_setoption
#define PHP3_UODBC_AUTOCOMMIT php3_uodbc_autocommit
#define PHP3_UODBC_CLOSE php3_uodbc_close
#define PHP3_UDOBC_CLOSE_ALL php3_uodbc_close_all
#define PHP3_UODBC_COMMIT php3_uodbc_commit
#define PHP3_UODBC_CONNECT php3_uodbc_connect
#define PHP3_UODBC_PCONNECT php3_uodbc_pconnect
#define PHP3_UODBC_DO_CONNECT php3_uodbc_do_connect
#define PHP3_UODBC_CURSOR php3_uodbc_cursor
#define PHP3_UODBC_DO php3_uodbc_do
#define PHP3_UODBC_EXECUTE php3_uodbc_execute
#define PHP3_UODBC_FETCH_INTO php3_uodbc_fetch_into
#define PHP3_UODBC_FETCH_ROW php3_uodbc_fetch_row
#define PHP3_UODBC_FIELD_LEN php3_uodbc_field_len
#define PHP3_UODBC_FIELD_NAME php3_uodbc_field_name
#define PHP3_UODBC_FIELD_TYPE php3_uodbc_field_type
#define PHP3_UODBC_FIELD_NUM php3_uodbc_field_num
#define PHP3_UODBC_FREE_RESULT php3_uodbc_free_result
#define PHP3_UODBC_NUM_FIELDS php3_uodbc_num_fields
#define PHP3_UODBC_NUM_ROWS php3_uodbc_num_rows
#define PHP3_UODBC_PREPARE php3_uodbc_prepare
#define PHP3_UODBC_RESULT php3_uodbc_result
#define PHP3_UODBC_RESULT_ALL php3_uodbc_result_all
#define PHP3_UODBC_ROLLBACK php3_uodbc_rollback
#define PHP3_UODBC_TRANSACT php3_uodbc_transact
#define PHP3_UODBC_BINMODE php3_uodbc_binmode
#define PHP3_UODBC_LONGREADLEN php3_uodbc_longreadlen

/* internal function defines */
#define UODBC_ADD_RESULT uodbc_add_result
#define UODBC_GET_RESULT uodbc_get_result
#define UODBC_DEL_RESULT uodbc_del_result
#define UODBC_ADD_CONN uodbc_add_conn
#define UODBC_GET_CONN uodbc_get_conn
#define UODBC_DEL_CONN uodbc_del_conn
#define UODBC_SQL_ERROR uodbc_sql_error
#define UODBC_BINDCOLS uodbc_bindcols
#define _FREE_UODBC_RESULT _free_uodbc_result
#define _CLOSE_UODBC_CONNECTION _close_uodbc_connection
#define _CLOSE_UODBC_PCONNECTION _close_uodbc_pconnection
/* function name defines */
#define ODBC_SETOPTION "odbc_setoption"
#define ODBC_AUTOCOMMIT "odbc_autocommit"
#define ODBC_CLOSE "odbc_close"
#define ODBC_CLOSE_ALL "odbc_close_all"
#define ODBC_COMMIT "odbc_commit"
#define ODBC_CONNECT "odbc_connect"
#define ODBC_PCONNECT "odbc_pconnect"
#define ODBC_CURSOR "odbc_cursor"
#define ODBC_DO "odbc_do"
#define ODBC_EXEC "odbc_exec"
#define ODBC_PREPARE "odbc_prepare"
#define ODBC_EXECUTE "odbc_execute"
#define ODBC_FETCH_ROW "odbc_fetch_row"
#define ODBC_FETCH_INTO "odbc_fetch_into"
#define ODBC_FIELD_LEN "odbc_field_len"
#define ODBC_FIELD_NAME "odbc_field_name"
#define ODBC_FIELD_TYPE "odbc_field_type"
#define ODBC_FIELD_NUM "odbc_field_num"
#define ODBC_FREE_RESULT "odbc_free_result"
#define ODBC_NUM_FIELDS "odbc_num_fields"
#define ODBC_NUM_ROWS "odbc_num_rows"
#define ODBC_RESULT "odbc_result"
#define ODBC_RESULT_ALL "odbc_result_all"
#define ODBC_ROLLBACK "odbc_rollback"
#define ODBC_TRANSACT "odbc_transact"
#define ODBC_DO_CONNECT "odbc_do_connect"
#define ODBC_LONGREADLEN "odbc_longreadlen"
#define ODBC_BINMODE "odbc_binmode"

/* ini variable defines */
#define ODBC_INI_DEFAULTDB "uodbc.default_db"
#define ODBC_INI_DEFAULTUSER "uodbc.default_user"
#define ODBC_INI_DEFAULTPW "uodbc.default_pw"
#define ODBC_INI_ALLOWPERSISTENT "uodbc.allow_persistent"
#define ODBC_INI_MAXPERSISTENT "uodbc.max_persistent"
#define ODBC_INI_MAXLINKS "uodbc.max_links"
#define ODBC_INI_DEFAULTLRL "uodbc.defaultlrl"
#define ODBC_INI_DEFAULTBINMODE "uodbc.defaultbinmode"
#endif

/*
*
*  Defines for SOLID
*
*/


#if 0 /* HAVE_SOLID turned off for now,
		will use above uodbc defines for now */
/* win32 vars for thread local storage */
#define UODBC_MUTEX solid_mutex
#define UODBC_TLS SOLIDTls
#define UODBC_GLOBAL_STRUCT solid_global_struct
#define UODBC_GLOBALS solid_globals
/* defines for variables, structs, etc */
#define UODBC_MODULE_NAME "SOLID (uODBC)"
#define UODBC_FUNCTIONS solid_functions
#define UODBC_MODULE_ENTRY solid_module_entry
#define UODBC_CONNECTION solid_connection
#define UODBC_RESULT_VALUE solid_result_value
#define UODBC_RESULT solid_result
#define UODBC_MODULE solid_module
#define PHP3_UODBC_MODULE php3_solid_module

/* function defines */
#define PHP3_MINIT_UODBC php3_minit_solid
#define PHP3_MSHUTDOWN_UODBC php3_mshutdown_solid
#define PHP3_RINIT_UODBC php3_rinit_solid
#define PHP3_INFO_UODBC php3_info_solid
#define PHP3_UODBC_SETOPTION php3_solid_setoption
#define PHP3_UODBC_AUTOCOMMIT php3_solid_autocommit
#define PHP3_UODBC_CLOSE php3_solid_close
#define PHP3_UDOBC_CLOSE_ALL php3_solid_close_all
#define PHP3_UODBC_COMMIT php3_solid_commit
#define PHP3_UODBC_CONNECT php3_solid_connect
#define PHP3_UODBC_PCONNECT php3_solid_pconnect
#define PHP3_UODBC_DO_CONNECT php3_solid_do_connect
#define PHP3_UODBC_CURSOR php3_solid_cursor
#define PHP3_UODBC_DO php3_solid_do
#define PHP3_UODBC_EXECUTE php3_solid_execute
#define PHP3_UODBC_FETCH_INTO php3_solid_fetch_into
#define PHP3_UODBC_FETCH_ROW php3_solid_fetch_row
#define PHP3_UODBC_FIELD_LEN php3_solid_field_len
#define PHP3_UODBC_FIELD_NAME php3_solid_field_name
#define PHP3_UODBC_FIELD_TYPE php3_solid_field_type
#define PHP3_UODBC_FREE_RESULT php3_solid_free_result
#define PHP3_UODBC_NUM_FIELDS php3_solid_num_fields
#define PHP3_UODBC_NUM_ROWS php3_solid_num_rows
#define PHP3_UODBC_PREPARE php3_solid_prepare
#define PHP3_UODBC_RESULT php3_solid_result
#define PHP3_UODBC_RESULT_ALL php3_solid_result_all
#define PHP3_UODBC_ROLLBACK php3_solid_rollback
#define PHP3_UODBC_TRANSACT php3_solid_transact

/* internal function defines */
#define UODBC_ADD_RESULT solid_add_result
#define UODBC_GET_RESULT solid_get_result
#define UODBC_DEL_RESULT solid_del_result
#define UODBC_ADD_CONN solid_add_conn
#define UODBC_GET_CONN solid_get_conn
#define UODBC_DEL_CONN solid_del_conn
#define UODBC_SQL_ERROR solid_sql_error
#define UODBC_BINDCOLS solid_bindcols
#define _FREE_UODBC_RESULT _free_solid_result
#define _CLOSE_UODBC_CONNECTION _close_solid_connection
#define _CLOSE_UODBC_PCONNECTION _close_solid_pconnection
/* function name defines */
#define ODBC_SETOPTION "solid_setoption"
#define ODBC_AUTOCOMMIT "solid_autocommit"
#define ODBC_CLOSE "solid_close"
#define ODBC_CLOSE_ALL "solid_close_all"
#define ODBC_COMMIT "solid_commit"
#define ODBC_CONNECT "solid_connect"
#define ODBC_PCONNECT "solid_pconnect"
#define ODBC_CURSOR "solid_cursor"
#define ODBC_DO "solid_do"
#define ODBC_EXEC "solid_exec"
#define ODBC_PREPARE "solid_prepare"
#define ODBC_EXECUTE "solid_execute"
#define ODBC_FETCH_ROW "solid_fetch_row"
#define ODBC_FETCH_INTO "solid_fetch_into"
#define ODBC_FIELD_LEN "solid_field_len"
#define ODBC_FIELD_NAME "solid_field_name"
#define ODBC_FIELD_TYPE "solid_field_type"
#define ODBC_FREE_RESULT "solid_free_result"
#define ODBC_NUM_FIELDS "solid_num_fields"
#define ODBC_NUM_ROWS "solid_num_rows"
#define ODBC_RESULT "solid_result"
#define ODBC_RESULT_ALL "solid_result_all"
#define ODBC_ROLLBACK "solid_rollback"
#define ODBC_TRANSACT "solid_transact"
#define ODBC_DO_CONNECT "solid_do_connect"

/* ini variable defines */
#define ODBC_INI_DEFAULTDB "solid.default_db"
#define ODBC_INI_DEFAULTUSER "solid.default_user"
#define ODBC_INI_DEFAULTPW "solid.default_pw"
#define ODBC_INI_ALLOWPERSISTENT "solid.allow_persistent"
#define ODBC_INI_MAXPERSISTENT "solid.max_persistent"
#define ODBC_INI_MAXLINKS "solid.max_links"
#endif


/*
*
*  Defines for ADABAS
*
*/


#if 0 /* HAVE_ADABAS turned for for now, uses
      defines from the ODBC section */
/* win32 vars for thread local storage */
#define UODBC_MUTEX adabas_mutex
#define UODBC_TLS ADABASTls
#define UODBC_GLOBAL_STRUCT adabas_global_struct
#define UODBC_GLOBALS adabas_globals
/* defines for variables, structs, etc */
#define UODBC_MODULE_NAME "ADABAS D (uODBC)"
#define UODBC_FUNCTIONS adabas_functions
#define UODBC_MODULE_ENTRY adabas_module_entry
#define UODBC_CONNECTION adabas_connection
#define UODBC_RESULT_VALUE adabas_result_value
#define UODBC_RESULT adabas_result
#define UODBC_MODULE adabas_module
#define PHP3_UODBC_MODULE php3_adabas_module

/* function defines */
#define PHP3_MINIT_UODBC php3_minit_adabas
#define PHP3_MSHUTDOWN_UODBC php3_mshutdown_adabas
#define PHP3_RINIT_UODBC php3_rinit_adabas
#define PHP3_INFO_UODBC php3_info_adabas
#define PHP3_UODBC_SETOPTION php3_adabas_setoption
#define PHP3_UODBC_AUTOCOMMIT php3_adabas_autocommit
#define PHP3_UODBC_CLOSE php3_adabas_close
#define PHP3_UDOBC_CLOSE_ALL php3_adabas_close_all
#define PHP3_UODBC_COMMIT php3_adabas_commit
#define PHP3_UODBC_CONNECT php3_adabas_connect
#define PHP3_UODBC_PCONNECT php3_adabas_pconnect
#define PHP3_UODBC_DO_CONNECT php3_adabas_do_connect
#define PHP3_UODBC_CURSOR php3_adabas_cursor
#define PHP3_UODBC_DO php3_adabas_do
#define PHP3_UODBC_EXECUTE php3_adabas_execute
#define PHP3_UODBC_FETCH_INTO php3_adabas_fetch_into
#define PHP3_UODBC_FETCH_ROW php3_adabas_fetch_row
#define PHP3_UODBC_FIELD_LEN php3_adabas_field_len
#define PHP3_UODBC_FIELD_NAME php3_adabas_field_name
#define PHP3_UODBC_FIELD_TYPE php3_adabas_field_type
#define PHP3_UODBC_FREE_RESULT php3_adabas_free_result
#define PHP3_UODBC_NUM_FIELDS php3_adabas_num_fields
#define PHP3_UODBC_NUM_ROWS php3_adabas_num_rows
#define PHP3_UODBC_PREPARE php3_adabas_prepare
#define PHP3_UODBC_RESULT php3_adabas_result
#define PHP3_UODBC_RESULT_ALL php3_adabas_result_all
#define PHP3_UODBC_ROLLBACK php3_adabas_rollback
#define PHP3_UODBC_TRANSACT php3_adabas_transact

/* internal function defines */
#define UODBC_ADD_RESULT adabas_add_result
#define UODBC_GET_RESULT adabas_get_result
#define UODBC_DEL_RESULT adabas_del_result
#define UODBC_ADD_CONN adabas_add_conn
#define UODBC_GET_CONN adabas_get_conn
#define UODBC_DEL_CONN adabas_del_conn
#define UODBC_SQL_ERROR adabas_sql_error
#define UODBC_BINDCOLS adabas_bindcols
#define _FREE_UODBC_RESULT _free_adabas_result
#define _CLOSE_UODBC_CONNECTION _close_adabas_connection
#define _CLOSE_UODBC_PCONNECTION _close_adabas_pconnection
/* function name defines */
#define ODBC_SETOPTION "adabas_setoption"
#define ODBC_AUTOCOMMIT "adabas_autocommit"
#define ODBC_CLOSE "adabas_close"
#define ODBC_CLOSE_ALL "adabas_close_all"
#define ODBC_COMMIT "adabas_commit"
#define ODBC_CONNECT "adabas_connect"
#define ODBC_PCONNECT "adabas_pconnect"
#define ODBC_CURSOR "adabas_cursor"
#define ODBC_DO "adabas_do"
#define ODBC_EXEC "adabas_exec"
#define ODBC_PREPARE "adabas_prepare"
#define ODBC_EXECUTE "adabas_execute"
#define ODBC_FETCH_ROW "adabas_fetch_row"
#define ODBC_FETCH_INTO "adabas_fetch_into"
#define ODBC_FIELD_LEN "adabas_field_len"
#define ODBC_FIELD_NAME "adabas_field_name"
#define ODBC_FIELD_TYPE "adabas_field_type"
#define ODBC_FREE_RESULT "adabas_free_result"
#define ODBC_NUM_FIELDS "adabas_num_fields"
#define ODBC_NUM_ROWS "adabas_num_rows"
#define ODBC_RESULT "adabas_result"
#define ODBC_RESULT_ALL "adabas_result_all"
#define ODBC_ROLLBACK "adabas_rollback"
#define ODBC_TRANSACT "adabas_transact"
#define ODBC_DO_CONNECT "adabas_do_connect"

/* ini variable defines */
#define ODBC_INI_DEFAULTDB "adabas.default_db"
#define ODBC_INI_DEFAULTUSER "adabas.default_user"
#define ODBC_INI_DEFAULTPW "adabas.default_pw"
#define ODBC_INI_ALLOWPERSISTENT "adabas.allow_persistent"
#define ODBC_INI_MAXPERSISTENT "adabas.max_persistent"
#define ODBC_INI_MAXLINKS "adabas.max_links"
#endif

/* checking in the same order as in configure.in */
# if HAVE_SOLID
#  include <cli0core.h>
#  include <cli0ext1.h>
#  define HAVE_SQL_EXTENDED_FETCH 0
extern void php3_solid_fetch_prev(INTERNAL_FUNCTION_PARAMETERS);
#  if defined(UODBC_UNIQUE_NAMES)
#   define UODBC_TYPE solid
#   define UODBC_NAME "solid"
#   define UODBC_FE(name, arg_types) UODBC_NAMED_FE(solid_##name, php3_solid_##name, arg_types)
#   define UODBC_FE_ALIAS(php_name, name, arg_types) UODBC_NAMED_FE(solid_##php_name, php3_solid_##name, arg_types)
#   define UODBC_FUNCTION(name) UODBC_NAMED_FUNCTION(php3_solid_##name)
#   define UODBC_FNAME(name) php3i_solid_##name
#   define ODBC_INI_VAR_NAME(name) #name
#   define ODBC_INI_VAR(a) ODBC_INI_VAR_NAME(solid.##a)
#   define UODBC_VAR_NAME(name) name
#   define UODBC_VAR(a) UODBC_VAR_NAME(solid_##a)
#	define PHP3_UODBC_VAR(a) UODBC_VAR_NAME(php3_solid_##a)
#   define UODBC_MODULE_ENTRY UODBC_VAR(module_entry)
#   define solid_module_ptr &UODBC_MODULE_ENTRY
#  endif

#elif HAVE_EMPRESS
#  include <sql.h>
#  include <sqlext.h>
#  define HAVE_SQL_EXTENDED_FETCH 0

#elif HAVE_ADABAS
#include <WINDOWS.H>
#include <sql.h>
#include <sqlext.h>
#  define HAVE_SQL_EXTENDED_FETCH 1
#  if defined(UODBC_UNIQUE_NAMES)
#   define UODBC_TYPE adabas
#   define UODBC_NAME "adabas"
#   define UODBC_FE(name, arg_types) UODBC_NAMED_FE(ada_##name, php3_ada_##name, arg_types)
#   define UODBC_FE_ALIAS(php_name, name, arg_types) UODBC_NAMED_FE(ada_##php_name, php3_ada_##name, arg_types)
#   define UODBC_FUNCTION(name) UODBC_NAMED_FUNCTION(php3_ada_##name)
#   define UODBC_FNAME(name) php3i_ada_##name
#   define ODBC_INI_VAR_NAME(name) #name
#   define ODBC_INI_VAR(a) ODBC_INI_VAR_NAME(ada.##a)
#   define UODBC_VAR_NAME(name) name
#   define UODBC_VAR(a) UODBC_VAR_NAME(ada_##a)
#       define PHP3_UODBC_VAR(a) UODBC_VAR_NAME(php3_ada_##a)
#   define UODBC_MODULE_ENTRY UODBC_VAR(module_entry)
#   define ada_module_ptr &UODBC_MODULE_ENTRY
#  endif

# elif HAVE_IODBC  && !(WIN32|WINNT)
#  include <isql.h>
#  include <isqlext.h>
#  include <odbc_types.h>
#  include <odbc_funcs.h>
#  define HAVE_SQL_EXTENDED_FETCH 1
#  define SQL_FD_FETCH_ABSOLUTE   0x00000010L
#  define SQL_CURSOR_DYNAMIC      2UL
#  define SQL_NO_TOTAL            (-4)
#  define SQL_SO_DYNAMIC          0x00000004L
#  define SQL_LEN_DATA_AT_EXEC_OFFSET  (-100)
#  define SQL_LEN_DATA_AT_EXEC(length) (-(length)+SQL_LEN_DATA_AT_EXEC_OFFSET)
#  if defined(UODBC_UNIQUE_NAMES)
#   define UODBC_TYPE iodbc
#   define UODBC_NAME "iodbc"
#   define UODBC_FE(name, arg_types) UODBC_NAMED_FE(iodbc_##name, php3_iodbc_##name, arg_types)
#   define UODBC_FE_ALIAS(php_name, name, arg_types) UODBC_NAMED_FE(iodbc_##php_name, php3_iodbc_##name, arg_types)
#   define UODBC_FUNCTION(name) UODBC_NAMED_FUNCTION(php3_iodbc_##name)
#   define UODBC_FNAME(name) php3i_iodbc_##name
#   define ODBC_INI_VAR_NAME(name) #name
#   define ODBC_INI_VAR(a) ODBC_INI_VAR_NAME(iodbc.##a)
#   define UODBC_VAR_NAME(name) name
#   define UODBC_VAR(a) UODBC_VAR_NAME(iodbc_##a)
#	define PHP3_UODBC_VAR(a) UODBC_VAR_NAME(php3_iodbc_##a)
#   define UODBC_MODULE_ENTRY UODBC_VAR(module_entry)
#   define iodbc_module_ptr &UODBC_MODULE_ENTRY
#  endif

#elif HAVE_OPENLINK

#  include <iodbc.h>
#  include <isql.h>
#  include <isqlext.h>
#  include <udbcext.h>
#  define HAVE_SQL_EXTENDED_FETCH 1
#  if defined(UODBC_UNIQUE_NAMES)
#   define UODBC_TYPE opnlnk
#   define UODBC_NAME "opnlnk"
#   define UODBC_FE(name, arg_types) UODBC_NAMED_FE(opnlnk_##name, php3_opnlnk_##name, arg_types)
#   define UODBC_FE_ALIAS(php_name, name, arg_types) UODBC_NAMED_FE(opnlnk_##php_name, php3_opnlnk_##name, arg_types)
#   define UODBC_FUNCTION(name) UODBC_NAMED_FUNCTION(php3_opnlnk_##name)
#   define UODBC_FNAME(name) php3i_opnlnk_##name
#   define ODBC_INI_VAR_NAME(name) #name
#   define ODBC_INI_VAR(a) ODBC_INI_VAR_NAME(opnlnk.##a)
#   define UODBC_VAR_NAME(name) name
#   define UODBC_VAR(a) UODBC_VAR_NAME(opnlnk_##a)
#	define PHP3_UODBC_VAR(a) UODBC_VAR_NAME(php3_opnlnk_##a)
#   define UODBC_MODULE_ENTRY UODBC_VAR(module_entry)
#   define opnlnk_module_ptr &UODBC_MODULE_ENTRY
#  endif

#elif HAVE_VELOCIS
/* Nothing ??? */
#  define UNIX
#  define HAVE_SQL_EXTENDED_FETCH 1
#  include <sql.h>
#  include <sqlext.h>
#  if defined(UODBC_UNIQUE_NAMES)
#   define UODBC_TYPE velocis
#   define UODBC_NAME "velocis"
#   define UODBC_FE(name, arg_types) UODBC_NAMED_FE(velocis_##name, php3_velocis_##name, arg_types)
#   define UODBC_FE_ALIAS(php_name, name, arg_types) UODBC_NAMED_FE(velocis_##php_name, php3_velocis_##name, arg_types)
#   define UODBC_FUNCTION(name) UODBC_NAMED_FUNCTION(php3_velocis_##name)
#   define UODBC_FNAME(name) php3i_velocis_##name
#   define ODBC_INI_VAR_NAME(name) #name
#   define ODBC_INI_VAR(a) ODBC_INI_VAR_NAME(velocis.##a)
#   define UODBC_VAR_NAME(name) name
#   define UODBC_VAR(a) UODBC_VAR_NAME(velocis_##a)
#	define PHP3_UODBC_VAR(a) UODBC_VAR_NAME(php3_velocis_##a)
#   define UODBC_MODULE_ENTRY UODBC_VAR(module_entry)
#   define velocis_module_ptr &UODBC_MODULE_ENTRY
#  endif

#elif HAVE_CODBC
#  define HAVE_SQL_EXTENDED_FETCH 1
#  include <odbc.h>
#  if defined(UODBC_UNIQUE_NAMES)
#   define UODBC_TYPE codbc
#   define UODBC_NAME "codbc"
#   define UODBC_FE(name, arg_types) UODBC_NAMED_FE(codbc_##name, php3_codbc_##name, arg_types)
#   define UODBC_FE_ALIAS(php_name, name, arg_types) UODBC_NAMED_FE(codbc_##php_name, php3_codbc_##name, arg_types)
#   define UODBC_FUNCTION(name) UODBC_NAMED_FUNCTION(php3_codbc_##name)
#   define UODBC_FNAME(name) php3i_codbc_##name
#   define ODBC_INI_VAR_NAME(name) #name
#   define ODBC_INI_VAR(a) ODBC_INI_VAR_NAME(codbc.##a)
#   define UODBC_VAR_NAME(name) name
#   define UODBC_VAR(a) UODBC_VAR_NAME(codbc_##a)
#	define PHP3_UODBC_VAR(a) UODBC_VAR_NAME(php3_codbc_##a)
#   define UODBC_MODULE_ENTRY UODBC_VAR(module_entry)
#   define codbc_module_ptr &UODBC_MODULE_ENTRY
#  endif

#elif HAVE_DB2
#  define HAVE_SQL_EXTENDED_FETCH 1
#  include <sqlcli1.h>
#  ifdef DB268K
/* Need to include ASLM for 68K applications */
#   include <LibraryManager.h>
#  endif
#  if defined(UODBC_UNIQUE_NAMES)
#   define UODBC_TYPE db2
#   define UODBC_NAME "db2"
#   define UODBC_FE(name, arg_types) UODBC_NAMED_FE(db2_##name, php3_db2_##name, arg_types)
#   define UODBC_FE_ALIAS(php_name, name, arg_types) UODBC_NAMED_FE(db2_##php_name, php3_db2_##name, arg_types)
#   define UODBC_FUNCTION(name) UODBC_NAMED_FUNCTION(php3_db2_##name)
#   define UODBC_FNAME(name) php3i_db2_##name
#   define ODBC_INI_VAR_NAME(name) #name
#   define ODBC_INI_VAR(a) ODBC_INI_VAR_NAME(db2.##a)
#   define UODBC_VAR_NAME(name) name
#   define UODBC_VAR(a) UODBC_VAR_NAME(db2_##a)
#   define PHP3_UODBC_VAR(a) UODBC_VAR_NAME(php3_db2_##a)
#   define UODBC_MODULE_ENTRY UODBC_VAR(module_entry)
#   define db2_module_ptr &UODBC_MODULE_ENTRY
#  endif

# else /* MS ODBC */
#  define HAVE_SQL_EXTENDED_FETCH 1
#  include <WINDOWS.H>
#  include <sql.h>
#  include <sqlext.h>
#  if defined(UODBC_UNIQUE_NAMES)
#   define UODBC_TYPE wodbc
#   define UODBC_NAME "wodbc"
#   define UODBC_FE(name, arg_types) UODBC_NAMED_FE(wodbc_##name, php3_wodbc_##name, arg_types)
#   define UODBC_FE_ALIAS(php_name, name, arg_types) UODBC_NAMED_FE(wodbc_##php_name, php3_wodbc_##name, arg_types)
#   define UODBC_FUNCTION(name) UODBC_NAMED_FUNCTION(php3_wodbc_##name)
#   define UODBC_FNAME(name) php3i_wodbc_##name
#   define ODBC_INI_VAR_NAME(name) #name
#   define ODBC_INI_VAR(a) ODBC_INI_VAR_NAME(wuodbc.##a)
#   define UODBC_VAR_NAME(name) name
#   define UODBC_VAR(a) UODBC_VAR_NAME(wodbc_##a)
#   define PHP3_UODBC_VAR(a) UODBC_VAR_NAME(php3_wuodbc_##a)
#   define UODBC_MODULE_ENTRY UODBC_VAR(module_entry)
#   define wodbc_module_ptr &UODBC_MODULE_ENTRY
#  endif
# endif


#if !defined(UODBC_UNIQUE_NAMES)
#define UODBC_TYPE odbc
#define UODBC_NAME "odbc"
#define UODBC_FE(name, arg_types) UODBC_NAMED_FE(odbc_##name, php3_odbc_##name, arg_types)
#define UODBC_FE_ALIAS(php_name, name, arg_types) UODBC_NAMED_FE(odbc_##php_name, php3_odbc_##name, arg_types)
#define UODBC_FUNCTION(name) UODBC_NAMED_FUNCTION(php3_odbc_##name)
#define UODBC_FNAME(name) php3i_odbc_##name
#define ODBC_INI_VAR_NAME(name) #name
#define ODBC_INI_VAR(a) ODBC_INI_VAR_NAME(uodbc.##a)
#define UODBC_VAR_NAME(name) name
#define UODBC_VAR(a) UODBC_VAR_NAME(uodbc_##a)
#define PHP3_UODBC_VAR(a) UODBC_VAR_NAME(php3_uodbc_##a)
#define UODBC_MODULE_ENTRY UODBC_VAR(module_entry)
#define uodbc_module_ptr &UODBC_MODULE_ENTRY
#else
#define UODBC_ALIAS(name, arg_types) UODBC_NAMED_FE(odbc_##name, php3_odbc_##name, arg_types)
#define UODBC_ALIAS_FE(php_name, name, arg_types) UODBC_NAMED_FE(odbc_##php_name, php3_odbc_##name, arg_types)
#endif



/*
*
*  Defines for ODBC
*
*/

/* win32 vars for thread local storage */
#define UODBC_MUTEX UODBC_VAR(mutex)
#define UODBC_TLS UODBC_VAR(Tls)
#define UODBC_GLOBAL_STRUCT UODBC_VAR(global_struct)
#define UODBC_GLOBALS UODBC_VAR(globals)
/* defines for variables, structs, etc */
#define UODBC_MODULE_NAME "ODBC"
#define UODBC_FUNCTIONS UODBC_VAR(functions)
#define UODBC_CONNECTION UODBC_VAR(connection)
#define UODBC_RESULT_VALUE UODBC_VAR(result_value)
#define UODBC_RESULT UODBC_VAR(result)
#define UODBC_MODULE UODBC_VAR(module)
#define UODBC_MODULE_PTR UODBC_VAR(module_ptr)
#define PHP3_UODBC_MODULE PHP3_UODBC_VAR(module)

/* function defines */
#define PHP3_MINIT_UODBC UODBC_FNAME(minit)
#define PHP3_MSHUTDOWN_UODBC UODBC_FNAME(mshutdown)
#define PHP3_RINIT_UODBC UODBC_FNAME(rinit)
#define PHP3_INFO_UODBC UODBC_FNAME(info)

/* internal function defines */
#define UODBC_ADD_RESULT UODBC_FNAME(add_result)
#define UODBC_GET_RESULT UODBC_FNAME(get_result)
#define UODBC_DEL_RESULT UODBC_FNAME(del_result)
#define UODBC_ADD_CONN UODBC_FNAME(add_conn)
#define UODBC_GET_CONN UODBC_FNAME(get_conn)
#define UODBC_DEL_CONN UODBC_FNAME(del_conn)
#define UODBC_SQL_ERROR UODBC_FNAME(sql_error)
#define UODBC_BINDCOLS UODBC_FNAME(bindcols)
#define PHP3_UODBC_DO_CONNECT UODBC_FNAME(_do_connect)
#define PHP3_UODBC_TRANSACT UODBC_FNAME(_do_transact)

# ifndef MSVC5
#  define FAR
# endif

extern php3_module_entry UODBC_MODULE_ENTRY;

/* uODBC functions */
extern int PHP3_MINIT_UODBC(INIT_FUNC_ARGS);
extern int PHP3_MSHUTDOWN_UODBC(void);
extern int PHP3_RINIT_UODBC(INIT_FUNC_ARGS);
extern void PHP3_INFO_UODBC(void);
extern UODBC_FUNCTION(setoption);
extern UODBC_FUNCTION(autocommit);
extern UODBC_FUNCTION(close);
extern UODBC_FUNCTION(close_all);
extern UODBC_FUNCTION(commit);
extern UODBC_FUNCTION(connect);
extern UODBC_FUNCTION(pconnect);
extern void PHP3_UODBC_DO_CONNECT(INTERNAL_FUNCTION_PARAMETERS, int);
extern UODBC_FUNCTION(cursor);
extern UODBC_FUNCTION(exec);
/*extern UODBC_FUNCTION(do);*/
extern UODBC_FUNCTION(execute);
extern UODBC_FUNCTION(fetch_into);
extern UODBC_FUNCTION(fetch_row);
extern UODBC_FUNCTION(field_len);
extern UODBC_FUNCTION(field_name);
extern UODBC_FUNCTION(field_type);
extern UODBC_FUNCTION(field_num);
extern UODBC_FUNCTION(free_result);
extern UODBC_FUNCTION(num_fields);
extern UODBC_FUNCTION(num_rows);
extern UODBC_FUNCTION(prepare);
extern UODBC_FUNCTION(result);
extern UODBC_FUNCTION(result_all);
extern UODBC_FUNCTION(rollback);
extern void PHP3_UODBC_TRANSACT(INTERNAL_FUNCTION_PARAMETERS, int);
extern UODBC_FUNCTION(binmode);
extern UODBC_FUNCTION(longreadlen);

typedef struct UODBC_CONNECTION {
#if HAVE_DB2
	SQLHANDLE hdbc;
#else
	HDBC hdbc;
#endif
	int open;
} UODBC_CONNECTION;

typedef struct UODBC_RESULT_VALUE {
	char name[32];
	char *value;
	long int vallen;
	SDWORD coltype;
} UODBC_RESULT_VALUE;

typedef struct UODBC_RESULT {
#if HAVE_DB2
	SQLHANDLE stmt;
#else
	HSTMT stmt;
#endif
	UODBC_RESULT_VALUE *values;
	SWORD numcols;
	SWORD numparams;
# if HAVE_SQL_EXTENDED_FETCH
	int fetch_abs;
# endif
    long longreadlen;
    int binmode;
	int fetched;
	UODBC_CONNECTION *conn_ptr;
} UODBC_RESULT;

typedef struct {
#if HAVE_DB2
	SQLHANDLE henv;
#else
	HENV henv;
#endif
	char *defDB;
	char *defUser;
	char *defPW;
	long allow_persistent;
	long max_persistent;
	long max_links;
	long num_persistent;
	long num_links;
	int defConn;
	int le_result, le_conn, le_pconn;
    long defaultlrl;
    long defaultbinmode;
	HashTable *resource_list;
	HashTable *resource_plist;
} UODBC_MODULE;

# ifndef THREAD_SAFE
extern UODBC_MODULE PHP3_UODBC_MODULE;
# endif

int UODBC_ADD_RESULT(HashTable *list, UODBC_RESULT *result);
UODBC_RESULT *UODBC_GET_RESULT(HashTable *list, int count);
void UODBC_DEL_RESULT(HashTable *list, int count);
int UODBC_ADD_CONN(HashTable *list, HDBC conn);
UODBC_CONNECTION *UODBC_GET_CONN(HashTable *list, int count);
void UODBC_DEL_CONN(HashTable *list, int ind);
#if HAVE_DB2
void UODBC_SQL_ERROR(SQLHANDLE conn, SQLHANDLE stmt, char *func);
#else
void UODBC_SQL_ERROR(HDBC conn, HSTMT stmt, char *func);
#endif
int UODBC_BINDCOLS(UODBC_RESULT *result);

#define IS_SQL_LONG(x) (x == SQL_LONGVARBINARY || x == SQL_LONGVARCHAR)
#define IS_SQL_BINARY(x) (x == SQL_BINARY || x == SQL_VARBINARY || x == SQL_LONGVARBINARY)

#else
# define uodbc_module_ptr NULL

#endif /* HAVE_UODBC || HAVE_DB2 */

#endif /* _UNIFIED_ODBC_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
