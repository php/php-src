/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stig Sæther Bakken <ssb@php.net>                            |
   |          Andreas Karajannis <Andreas.Karajannis@gmd.de>              |
   |          Frank M. Kromann <frank@kromann.info>  Support for DB/2 CLI |
   |          Kevin N. Shallow <kshallow@tampabay.rr.com> Birdstep Support|
   |          Daniel R. Kalowsky <kalowsky@php.net>                       |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
 
#include "php.h"
#include "php_globals.h"

#include "ext/standard/info.h"
#include "ext/standard/php_string.h"
#include "ext/standard/php_standard.h"

#include "php_odbc.h"
#include "php_odbc_includes.h"
#include "php_globals.h"

#if HAVE_UODBC

#include <fcntl.h>
#include "ext/standard/head.h"
#include "php_ini.h"

#ifdef PHP_WIN32
#include <winsock2.h>

#define ODBC_TYPE "Win32"
#define PHP_ODBC_TYPE ODBC_TYPE

#endif

/*
 * not defined elsewhere
 */

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

void odbc_do_connect(INTERNAL_FUNCTION_PARAMETERS, int persistent);

static int le_result, le_conn, le_pconn;

#define SAFE_SQL_NTS(n) ((SQLSMALLINT) ((n)?(SQL_NTS):0))

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO(arginfo_odbc_close_all, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_binmode, 0, 0, 2)
	ZEND_ARG_INFO(0, result_id)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_longreadlen, 0, 0, 2)
	ZEND_ARG_INFO(0, result_id)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_prepare, 0, 0, 2)
	ZEND_ARG_INFO(0, connection_id)
	ZEND_ARG_INFO(0, query)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_execute, 0, 0, 1)
	ZEND_ARG_INFO(0, result_id)
	ZEND_ARG_INFO(0, parameters_array)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_cursor, 0, 0, 1)
	ZEND_ARG_INFO(0, result_id)
ZEND_END_ARG_INFO()

#ifdef HAVE_SQLDATASOURCES
ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_data_source, 0, 0, 2)
	ZEND_ARG_INFO(0, connection_id)
	ZEND_ARG_INFO(0, fetch_type)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_exec, 0, 0, 2)
	ZEND_ARG_INFO(0, connection_id)
	ZEND_ARG_INFO(0, query)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

#ifdef PHP_ODBC_HAVE_FETCH_HASH
ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_fetch_object, 0, 0, 1)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, rownumber)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_fetch_array, 0, 0, 1)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, rownumber)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_fetch_into, 0, 0, 2)
	ZEND_ARG_INFO(0, result_id)
	ZEND_ARG_INFO(1, result_array)
	ZEND_ARG_INFO(0, rownumber)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_fetch_row, 0, 0, 1)
	ZEND_ARG_INFO(0, result_id)
	ZEND_ARG_INFO(0, row_number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_result, 0, 0, 2)
	ZEND_ARG_INFO(0, result_id)
	ZEND_ARG_INFO(0, field)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_result_all, 0, 0, 1)
	ZEND_ARG_INFO(0, result_id)
	ZEND_ARG_INFO(0, format)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_free_result, 0, 0, 1)
	ZEND_ARG_INFO(0, result_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_connect, 0, 0, 3)
	ZEND_ARG_INFO(0, dsn)
	ZEND_ARG_INFO(0, user)
	ZEND_ARG_INFO(0, password)
	ZEND_ARG_INFO(0, cursor_option)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_pconnect, 0, 0, 3)
	ZEND_ARG_INFO(0, dsn)
	ZEND_ARG_INFO(0, user)
	ZEND_ARG_INFO(0, password)
	ZEND_ARG_INFO(0, cursor_option)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_close, 0, 0, 1)
	ZEND_ARG_INFO(0, connection_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_num_rows, 0, 0, 1)
	ZEND_ARG_INFO(0, result_id)
ZEND_END_ARG_INFO()

#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30)
ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_next_result, 0, 0, 1)
	ZEND_ARG_INFO(0, result_id)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_num_fields, 0, 0, 1)
	ZEND_ARG_INFO(0, result_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_field_name, 0, 0, 2)
	ZEND_ARG_INFO(0, result_id)
	ZEND_ARG_INFO(0, field_number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_field_type, 0, 0, 2)
	ZEND_ARG_INFO(0, result_id)
	ZEND_ARG_INFO(0, field_number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_field_len, 0, 0, 2)
	ZEND_ARG_INFO(0, result_id)
	ZEND_ARG_INFO(0, field_number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_field_scale, 0, 0, 2)
	ZEND_ARG_INFO(0, result_id)
	ZEND_ARG_INFO(0, field_number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_field_num, 0, 0, 2)
	ZEND_ARG_INFO(0, result_id)
	ZEND_ARG_INFO(0, field_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_autocommit, 0, 0, 1)
	ZEND_ARG_INFO(0, connection_id)
	ZEND_ARG_INFO(0, onoff)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_commit, 0, 0, 1)
	ZEND_ARG_INFO(0, connection_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_rollback, 0, 0, 1)
	ZEND_ARG_INFO(0, connection_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_error, 0, 0, 0)
	ZEND_ARG_INFO(0, connection_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_errormsg, 0, 0, 0)
	ZEND_ARG_INFO(0, connection_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_setoption, 0, 0, 4)
	ZEND_ARG_INFO(0, conn_id)
	ZEND_ARG_INFO(0, which)
	ZEND_ARG_INFO(0, option)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_tables, 0, 0, 1)
	ZEND_ARG_INFO(0, connection_id)
	ZEND_ARG_INFO(0, qualifier)
	ZEND_ARG_INFO(0, owner)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, table_types)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_columns, 0, 0, 1)
	ZEND_ARG_INFO(0, connection_id)
	ZEND_ARG_INFO(0, qualifier)
	ZEND_ARG_INFO(0, owner)
	ZEND_ARG_INFO(0, table_name)
	ZEND_ARG_INFO(0, column_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_gettypeinfo, 0, 0, 1)
	ZEND_ARG_INFO(0, connection_id)
	ZEND_ARG_INFO(0, data_type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_primarykeys, 0, 0, 4)
	ZEND_ARG_INFO(0, connection_id)
	ZEND_ARG_INFO(0, qualifier)
	ZEND_ARG_INFO(0, owner)
	ZEND_ARG_INFO(0, table)
ZEND_END_ARG_INFO()

#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35)
#if !defined(HAVE_BIRDSTEP)
ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_procedurecolumns, 0, 0, 1)
	ZEND_ARG_INFO(0, connection_id)
	ZEND_ARG_INFO(0, qualifier)
	ZEND_ARG_INFO(0, owner)
	ZEND_ARG_INFO(0, proc)
	ZEND_ARG_INFO(0, column)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_procedures, 0, 0, 1)
	ZEND_ARG_INFO(0, connection_id)
	ZEND_ARG_INFO(0, qualifier)
	ZEND_ARG_INFO(0, owner)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_foreignkeys, 0, 0, 7)
	ZEND_ARG_INFO(0, connection_id)
	ZEND_ARG_INFO(0, pk_qualifier)
	ZEND_ARG_INFO(0, pk_owner)
	ZEND_ARG_INFO(0, pk_table)
	ZEND_ARG_INFO(0, fk_qualifier)
	ZEND_ARG_INFO(0, fk_owner)
	ZEND_ARG_INFO(0, fk_table)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_specialcolumns, 0, 0, 7)
	ZEND_ARG_INFO(0, connection_id)
	ZEND_ARG_INFO(0, type)
	ZEND_ARG_INFO(0, qualifier)
	ZEND_ARG_INFO(0, owner)
	ZEND_ARG_INFO(0, table)
	ZEND_ARG_INFO(0, scope)
	ZEND_ARG_INFO(0, nullable)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_statistics, 0, 0, 6)
	ZEND_ARG_INFO(0, connection_id)
	ZEND_ARG_INFO(0, qualifier)
	ZEND_ARG_INFO(0, owner)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, unique)
	ZEND_ARG_INFO(0, accuracy)
ZEND_END_ARG_INFO()

#if !defined(HAVE_DBMAKER) && !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) &&!defined(HAVE_SOLID_35) && !defined(HAVE_BIRDSTEP)
ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_tableprivileges, 0, 0, 4)
	ZEND_ARG_INFO(0, connection_id)
	ZEND_ARG_INFO(0, qualifier)
	ZEND_ARG_INFO(0, owner)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_columnprivileges, 0, 0, 5)
	ZEND_ARG_INFO(0, connection_id)
	ZEND_ARG_INFO(0, catalog)
	ZEND_ARG_INFO(0, schema)
	ZEND_ARG_INFO(0, table)
	ZEND_ARG_INFO(0, column)
ZEND_END_ARG_INFO()
#endif
/* }}} */

/* {{{ odbc_functions[]
 */
const zend_function_entry odbc_functions[] = {
	PHP_FE(odbc_autocommit, arginfo_odbc_autocommit)
	PHP_FE(odbc_binmode, arginfo_odbc_binmode)
	PHP_FE(odbc_close, arginfo_odbc_close)
	PHP_FE(odbc_close_all, arginfo_odbc_close_all)
	PHP_FE(odbc_columns, arginfo_odbc_columns)
	PHP_FE(odbc_commit, arginfo_odbc_commit)
	PHP_FE(odbc_connect, arginfo_odbc_connect)
	PHP_FE(odbc_cursor, arginfo_odbc_cursor)
#ifdef HAVE_SQLDATASOURCES
	PHP_FE(odbc_data_source, arginfo_odbc_data_source)
#endif
	PHP_FE(odbc_execute, arginfo_odbc_execute)
	PHP_FE(odbc_error, arginfo_odbc_error)
	PHP_FE(odbc_errormsg, arginfo_odbc_errormsg)
	PHP_FE(odbc_exec, arginfo_odbc_exec)
#ifdef PHP_ODBC_HAVE_FETCH_HASH
	PHP_FE(odbc_fetch_array, arginfo_odbc_fetch_array)
	PHP_FE(odbc_fetch_object, arginfo_odbc_fetch_object)
#endif
	PHP_FE(odbc_fetch_row, arginfo_odbc_fetch_row)
	PHP_FE(odbc_fetch_into, arginfo_odbc_fetch_into)
	PHP_FE(odbc_field_len, arginfo_odbc_field_len)
	PHP_FE(odbc_field_scale, arginfo_odbc_field_scale)
	PHP_FE(odbc_field_name, arginfo_odbc_field_name)
	PHP_FE(odbc_field_type, arginfo_odbc_field_type)
	PHP_FE(odbc_field_num, arginfo_odbc_field_num)
	PHP_FE(odbc_free_result, arginfo_odbc_free_result)
	PHP_FE(odbc_gettypeinfo, arginfo_odbc_gettypeinfo)
	PHP_FE(odbc_longreadlen, arginfo_odbc_longreadlen)
#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30)
	PHP_FE(odbc_next_result, arginfo_odbc_next_result)
#endif
	PHP_FE(odbc_num_fields, arginfo_odbc_num_fields)
	PHP_FE(odbc_num_rows, arginfo_odbc_num_rows)
	PHP_FE(odbc_pconnect, arginfo_odbc_pconnect)
	PHP_FE(odbc_prepare, arginfo_odbc_prepare)
	PHP_FE(odbc_result, arginfo_odbc_result)
	PHP_FE(odbc_result_all, arginfo_odbc_result_all)
	PHP_FE(odbc_rollback, arginfo_odbc_rollback)
	PHP_FE(odbc_setoption, arginfo_odbc_setoption)
	PHP_FE(odbc_specialcolumns, arginfo_odbc_specialcolumns)
	PHP_FE(odbc_statistics, arginfo_odbc_statistics)
	PHP_FE(odbc_tables, arginfo_odbc_tables)
	PHP_FE(odbc_primarykeys, arginfo_odbc_primarykeys)
#if !defined(HAVE_DBMAKER) && !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) &&!defined(HAVE_SOLID_35) && !defined(HAVE_BIRDSTEP)    /* not supported now */
	PHP_FE(odbc_columnprivileges, arginfo_odbc_columnprivileges)
	PHP_FE(odbc_tableprivileges, arginfo_odbc_tableprivileges)
#endif
#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35) /* not supported */
	PHP_FE(odbc_foreignkeys, arginfo_odbc_foreignkeys)
	PHP_FE(odbc_procedures, arginfo_odbc_procedures)
#if !defined(HAVE_BIRDSTEP)
	PHP_FE(odbc_procedurecolumns, arginfo_odbc_procedurecolumns)
#endif
#endif
	PHP_FALIAS(odbc_do, odbc_exec, arginfo_odbc_exec)
	PHP_FALIAS(odbc_field_precision, odbc_field_len, arginfo_odbc_field_len)
	PHP_FE_END
};
/* }}} */

ZEND_DECLARE_MODULE_GLOBALS(odbc)
static PHP_GINIT_FUNCTION(odbc);

/* {{{ odbc_module_entry
 */
zend_module_entry odbc_module_entry = {
	STANDARD_MODULE_HEADER,
	"odbc", 
	odbc_functions, 
	PHP_MINIT(odbc), 
	PHP_MSHUTDOWN(odbc),
	PHP_RINIT(odbc), 
	PHP_RSHUTDOWN(odbc), 
	PHP_MINFO(odbc), 
	"1.0",
	PHP_MODULE_GLOBALS(odbc),
	PHP_GINIT(odbc),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_ODBC
ZEND_GET_MODULE(odbc)
#endif

/* {{{ _free_odbc_result
 */
static void _free_odbc_result(zend_resource *rsrc)
{
	odbc_result *res = (odbc_result *)rsrc->ptr;
	int i;
	RETCODE rc;
	
	if (res) {
		if (res->values) {
			for(i = 0; i < res->numcols; i++) {
				if (res->values[i].value)
					efree(res->values[i].value);
			}
			efree(res->values);
			res->values = NULL;
		}
		if (res->stmt) {
#if defined(HAVE_SOLID) || defined(HAVE_SOLID_30) || defined(HAVE_SOLID_35)
			SQLTransact(res->conn_ptr->henv, res->conn_ptr->hdbc,
						(SQLUSMALLINT) SQL_COMMIT);
#endif
			rc = SQLFreeStmt(res->stmt,SQL_DROP);
			/* We don't want the connection to be closed after the last statement has been closed
			 * Connections will be closed on shutdown
			 * zend_list_delete(res->conn_ptr->id);
			 */
		}
		efree(res);
	}
}
/* }}} */

/* {{{ safe_odbc_disconnect
 * disconnect, and if it fails, then issue a rollback for any pending transaction (lurcher)
 */
static void safe_odbc_disconnect( void *handle )
{
	int ret;

	ret = SQLDisconnect( handle );
	if ( ret == SQL_ERROR )
	{
		SQLTransact( NULL, handle, SQL_ROLLBACK );
		SQLDisconnect( handle );
	}
}
/* }}} */

/* {{{ _close_odbc_conn
 */
static void _close_odbc_conn(zend_resource *rsrc)
{
	zend_resource *p;
	odbc_result *res;

	odbc_connection *conn = (odbc_connection *)rsrc->ptr;

	ZEND_HASH_FOREACH_PTR(&EG(regular_list), p) {
		if (p->ptr && (p->type == le_result)) {
			res = (odbc_result *)p->ptr;
			if (res->conn_ptr == conn) {
				zend_list_close(p);
			}
		}
	} ZEND_HASH_FOREACH_END();

   	safe_odbc_disconnect(conn->hdbc);
	SQLFreeConnect(conn->hdbc);
	SQLFreeEnv(conn->henv);
	efree(conn);
	ODBCG(num_links)--;
}
/* }}} */

/* {{{ void _close_odbc_pconn
 */
static void _close_odbc_pconn(zend_resource *rsrc)
{
	zend_resource *p;
	odbc_result *res;
	odbc_connection *conn = (odbc_connection *)rsrc->ptr;

	ZEND_HASH_FOREACH_PTR(&EG(regular_list), p) {
		if (p->ptr && (p->type == le_result)) {
			res = (odbc_result *)p->ptr;
			if (res->conn_ptr == conn) {
				zend_list_close(p);
			}
		}
	} ZEND_HASH_FOREACH_END();

	safe_odbc_disconnect(conn->hdbc);
	SQLFreeConnect(conn->hdbc);
	SQLFreeEnv(conn->henv);
	free(conn);

	ODBCG(num_links)--;
	ODBCG(num_persistent)--;
}
/* }}} */

/* {{{ PHP_INI_DISP(display_link_nums)
 */
static PHP_INI_DISP(display_link_nums)
{
	char *value;

	if (type == PHP_INI_DISPLAY_ORIG && ini_entry->modified) {
		value = ini_entry->orig_value->val;
	} else if (ini_entry->value) {
		value = ini_entry->value->val;
	} else {
		value = NULL;
	}

	if (value) {
		if (atoi(value) == -1) {
			PUTS("Unlimited");
		} else {
			php_printf("%s", value);
		}
	}
}
/* }}} */

/* {{{ PHP_INI_DISP(display_defPW)
 */
static PHP_INI_DISP(display_defPW)
{
	char *value;

	if (type == PHP_INI_DISPLAY_ORIG && ini_entry->modified) {
		value = ini_entry->orig_value->val;
	} else if (ini_entry->value) {
		value = ini_entry->value->val;
	} else {
		value = NULL;
	}

	if (value) {
#if PHP_DEBUG
		php_printf("%s", value);
#else
		PUTS("********");
#endif
	} else {
		if (PG(html_errors)) {
			PUTS("<i>no value</i>");
		} else {
			PUTS("no value");
		}
	}
}
/* }}} */

/* {{{ PHP_INI_DISP(display_binmode)
 */
static PHP_INI_DISP(display_binmode)
{
	char *value;
	
	if (type == PHP_INI_DISPLAY_ORIG && ini_entry->modified) {
		value = ini_entry->orig_value->val;
	} else if (ini_entry->value) {
		value = ini_entry->value->val;
	} else {
		value = NULL;
	}

	if (value) {
		switch(atoi(value)) {
			case 0:
				PUTS("passthru");
				break;
			case 1:
				PUTS("return as is");
				break;
			case 2:
				PUTS("return as char");
				break;
		}
	}
}
/* }}} */

/* {{{ PHP_INI_DISP(display_lrl)
 */
static PHP_INI_DISP(display_lrl)
{
	char *value;

	if (type == PHP_INI_DISPLAY_ORIG && ini_entry->modified) {
		value = ini_entry->orig_value->val;
	} else if (ini_entry->value) {
		value = ini_entry->value->val;
	} else {
		value = NULL;
	}

	if (value) {
		if (atoi(value) <= 0) {
			PUTS("Passthru");
		} else {
			php_printf("return up to %s bytes", value);
		}
	}
}
/* }}} */


/* {{{ PHP_INI_DISP(display_cursortype)
 */
static PHP_INI_DISP(display_cursortype)
{
	char *value;

	if (type == PHP_INI_DISPLAY_ORIG && ini_entry->modified) {
		value = ini_entry->orig_value->val;
	} else if (ini_entry->value) {
		value = ini_entry->value->val;
	} else {
		value = NULL;
	}

	if (value) {
		switch (atoi (value))
		  {
		    case SQL_CURSOR_FORWARD_ONLY:
				PUTS ("Forward Only cursor");
				break;

			case SQL_CURSOR_STATIC:
			    PUTS ("Static cursor");
				break;

			case SQL_CURSOR_KEYSET_DRIVEN:
				PUTS ("Keyset driven cursor");
				break;

			case SQL_CURSOR_DYNAMIC:
				PUTS ("Dynamic cursor");
				break;

			default:
				php_printf("Unknown cursor model %s", value);
				break;
		  }
	}
}

/* }}} */

/* {{{ PHP_INI_BEGIN 
 */
PHP_INI_BEGIN()
	STD_PHP_INI_BOOLEAN("odbc.allow_persistent", "1", PHP_INI_SYSTEM, OnUpdateLong,
			allow_persistent, zend_odbc_globals, odbc_globals)
	STD_PHP_INI_ENTRY_EX("odbc.max_persistent",  "-1", PHP_INI_SYSTEM, OnUpdateLong,
			max_persistent, zend_odbc_globals, odbc_globals, display_link_nums)
	STD_PHP_INI_ENTRY_EX("odbc.max_links", "-1", PHP_INI_SYSTEM, OnUpdateLong,
			max_links, zend_odbc_globals, odbc_globals, display_link_nums)
	STD_PHP_INI_ENTRY("odbc.default_db", NULL, PHP_INI_ALL, OnUpdateString,
			defDB, zend_odbc_globals, odbc_globals)
	STD_PHP_INI_ENTRY("odbc.default_user", NULL, PHP_INI_ALL, OnUpdateString,
			defUser, zend_odbc_globals, odbc_globals)
	STD_PHP_INI_ENTRY_EX("odbc.default_pw", NULL, PHP_INI_ALL, OnUpdateString,
			defPW, zend_odbc_globals, odbc_globals, display_defPW)
	STD_PHP_INI_ENTRY_EX("odbc.defaultlrl", "4096", PHP_INI_ALL, OnUpdateLong,
			defaultlrl, zend_odbc_globals, odbc_globals, display_lrl)
	STD_PHP_INI_ENTRY_EX("odbc.defaultbinmode", "1", PHP_INI_ALL, OnUpdateLong,
			defaultbinmode, zend_odbc_globals, odbc_globals, display_binmode)
	STD_PHP_INI_BOOLEAN("odbc.check_persistent", "1", PHP_INI_SYSTEM, OnUpdateLong,
			check_persistent, zend_odbc_globals, odbc_globals)
	STD_PHP_INI_ENTRY_EX("odbc.default_cursortype", "3", PHP_INI_ALL, OnUpdateLong, 
			default_cursortype, zend_odbc_globals, odbc_globals, display_cursortype)
PHP_INI_END()
/* }}} */

static PHP_GINIT_FUNCTION(odbc)
{
	odbc_globals->num_persistent = 0;
}

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(odbc)
{
#ifdef SQLANY_BUG
	ODBC_SQL_CONN_T foobar;
	RETCODE rc;
#endif

	REGISTER_INI_ENTRIES();
	le_result = zend_register_list_destructors_ex(_free_odbc_result, NULL, "odbc result", module_number);
	le_conn = zend_register_list_destructors_ex(_close_odbc_conn, NULL, "odbc link", module_number);
	le_pconn = zend_register_list_destructors_ex(NULL, _close_odbc_pconn, "odbc link persistent", module_number);
	odbc_module_entry.type = type;
	
	REGISTER_STRING_CONSTANT("ODBC_TYPE", PHP_ODBC_TYPE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ODBC_BINMODE_PASSTHRU", 0, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ODBC_BINMODE_RETURN", 1, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ODBC_BINMODE_CONVERT", 2, CONST_CS | CONST_PERSISTENT);
	/* Define Constants for options
	   these Constants are defined in <sqlext.h>
	*/
	REGISTER_LONG_CONSTANT("SQL_ODBC_CURSORS", SQL_ODBC_CURSORS, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_CUR_USE_DRIVER", SQL_CUR_USE_DRIVER, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_CUR_USE_IF_NEEDED", SQL_CUR_USE_IF_NEEDED, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_CUR_USE_ODBC", SQL_CUR_USE_ODBC, CONST_PERSISTENT | CONST_CS);


	REGISTER_LONG_CONSTANT("SQL_CONCURRENCY", SQL_CONCURRENCY, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_CONCUR_READ_ONLY", SQL_CONCUR_READ_ONLY, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_CONCUR_LOCK", SQL_CONCUR_LOCK, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_CONCUR_ROWVER", SQL_CONCUR_ROWVER, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_CONCUR_VALUES", SQL_CONCUR_VALUES, CONST_PERSISTENT | CONST_CS);

	REGISTER_LONG_CONSTANT("SQL_CURSOR_TYPE", SQL_CURSOR_TYPE, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_CURSOR_FORWARD_ONLY", SQL_CURSOR_FORWARD_ONLY, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_CURSOR_KEYSET_DRIVEN", SQL_CURSOR_KEYSET_DRIVEN, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_CURSOR_DYNAMIC", SQL_CURSOR_DYNAMIC, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_CURSOR_STATIC", SQL_CURSOR_STATIC, CONST_PERSISTENT | CONST_CS);
	
	REGISTER_LONG_CONSTANT("SQL_KEYSET_SIZE", SQL_KEYSET_SIZE, CONST_PERSISTENT | CONST_CS);

	/* these are for the Data Source type */
	REGISTER_LONG_CONSTANT("SQL_FETCH_FIRST", SQL_FETCH_FIRST, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_FETCH_NEXT", SQL_FETCH_NEXT, CONST_PERSISTENT | CONST_CS);

	/*
	 * register the standard data types
	 */
	REGISTER_LONG_CONSTANT("SQL_CHAR", SQL_CHAR, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_VARCHAR", SQL_VARCHAR, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_LONGVARCHAR", SQL_LONGVARCHAR, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_DECIMAL", SQL_DECIMAL, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_NUMERIC", SQL_NUMERIC, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_BIT", SQL_BIT, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_TINYINT", SQL_TINYINT, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_SMALLINT", SQL_SMALLINT, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_INTEGER", SQL_INTEGER, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_BIGINT", SQL_BIGINT, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_REAL", SQL_REAL, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_FLOAT", SQL_FLOAT, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_DOUBLE", SQL_DOUBLE, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_BINARY", SQL_BINARY, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_VARBINARY", SQL_VARBINARY, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_LONGVARBINARY", SQL_LONGVARBINARY, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_DATE", SQL_DATE, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_TIME", SQL_TIME, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_TIMESTAMP", SQL_TIMESTAMP, CONST_PERSISTENT | CONST_CS);
#if defined(ODBCVER) && (ODBCVER >= 0x0300)
	REGISTER_LONG_CONSTANT("SQL_TYPE_DATE", SQL_TYPE_DATE, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_TYPE_TIME", SQL_TYPE_TIME, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_TYPE_TIMESTAMP", SQL_TYPE_TIMESTAMP, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_WCHAR", SQL_WCHAR, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_WVARCHAR", SQL_WVARCHAR, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_WLONGVARCHAR", SQL_WLONGVARCHAR, CONST_PERSISTENT | CONST_CS);

	/*
	 * SQLSpecialColumns values
	 */
	REGISTER_LONG_CONSTANT("SQL_BEST_ROWID", SQL_BEST_ROWID, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_ROWVER", SQL_ROWVER, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_SCOPE_CURROW", SQL_SCOPE_CURROW, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_SCOPE_TRANSACTION", SQL_SCOPE_TRANSACTION, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_SCOPE_SESSION", SQL_SCOPE_SESSION, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_NO_NULLS", SQL_NO_NULLS, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_NULLABLE", SQL_NULLABLE, CONST_PERSISTENT | CONST_CS);

	/*
	 * SQLStatistics values
	 */
	REGISTER_LONG_CONSTANT("SQL_INDEX_UNIQUE", SQL_INDEX_UNIQUE, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_INDEX_ALL", SQL_INDEX_ALL, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_ENSURE", SQL_ENSURE, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_QUICK", SQL_QUICK, CONST_PERSISTENT | CONST_CS);
#endif

#if defined(HAVE_IBMDB2) && defined(_AIX)
	/* atexit() handler in the DB2/AIX library segfaults in PHP CLI */
	/* DB2NOEXITLIST env variable prevents DB2 from invoking atexit() */
	putenv("DB2NOEXITLIST=TRUE");
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(odbc)
{
	ODBCG(defConn) = -1;
	ODBCG(num_links) = ODBCG(num_persistent);
	memset(ODBCG(laststate), '\0', 6);
	memset(ODBCG(lasterrormsg), '\0', SQL_MAX_MESSAGE_LENGTH);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION */
PHP_RSHUTDOWN_FUNCTION(odbc)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(odbc)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(odbc)
{
	char buf[32];

	php_info_print_table_start();
	php_info_print_table_header(2, "ODBC Support", "enabled");
	snprintf(buf, sizeof(buf), ZEND_LONG_FMT, ODBCG(num_persistent));
	php_info_print_table_row(2, "Active Persistent Links", buf);
	snprintf(buf, sizeof(buf), ZEND_LONG_FMT, ODBCG(num_links));
	php_info_print_table_row(2, "Active Links", buf);
	php_info_print_table_row(2, "ODBC library", PHP_ODBC_TYPE);
#ifdef ODBCVER
	snprintf(buf, sizeof(buf), "0x%0.4x", ODBCVER);
	php_info_print_table_row(2, "ODBCVER", buf);
#endif
#ifndef PHP_WIN32
	php_info_print_table_row(2, "ODBC_INCLUDE", PHP_ODBC_INCLUDE);
	php_info_print_table_row(2, "ODBC_LFLAGS", PHP_ODBC_LFLAGS);
	php_info_print_table_row(2, "ODBC_LIBS", PHP_ODBC_LIBS);
#endif
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();

}	 
/* }}} */

/* {{{ odbc_sql_error */
void odbc_sql_error(ODBC_SQL_ERROR_PARAMS)
{
	char   	    state[6];
	SQLINTEGER	error;        /* Not used */
	char   	    errormsg[SQL_MAX_MESSAGE_LENGTH];
	SQLSMALLINT	errormsgsize; /* Not used */
	RETCODE rc;
	ODBC_SQL_ENV_T henv;
	ODBC_SQL_CONN_T conn;

	if (conn_resource) {
		henv = conn_resource->henv;
		conn = conn_resource->hdbc;
	} else {
		henv = SQL_NULL_HENV;
		conn = SQL_NULL_HDBC;
	}

	/* This leads to an endless loop in many drivers! 
	 *
	   while(henv != SQL_NULL_HENV){
		do {
	 */
	rc = SQLError(henv, conn, stmt, state, &error, errormsg, sizeof(errormsg)-1, &errormsgsize);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		snprintf(state, sizeof(state), "HY000");
		snprintf(errormsg, sizeof(errormsg), "Failed to fetch error message");
	}
	if (conn_resource) {
		memcpy(conn_resource->laststate, state, sizeof(state));
		memcpy(conn_resource->lasterrormsg, errormsg, sizeof(errormsg));
	}
	memcpy(ODBCG(laststate), state, sizeof(state));
	memcpy(ODBCG(lasterrormsg), errormsg, sizeof(errormsg));
	if (func) {
		php_error_docref(NULL, E_WARNING, "SQL error: %s, SQL state %s in %s", errormsg, state, func);
	} else {
		php_error_docref(NULL, E_WARNING, "SQL error: %s, SQL state %s", errormsg, state);
	}
	/*		
		} while (SQL_SUCCEEDED(rc));
	}
	*/
}
/* }}} */

/* {{{ php_odbc_fetch_attribs */
void php_odbc_fetch_attribs(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	odbc_result *result;
	zval *pv_res;
	zend_long flag;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &pv_res, &flag) == FAILURE) {
		return;
	}

	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_FALSE;
	}
	
	if (mode) {
		result->longreadlen = flag;
	} else {
		result->binmode = flag;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ odbc_bindcols */
int odbc_bindcols(odbc_result *result)
{
	RETCODE rc;
	int i;
	SQLSMALLINT 	colnamelen; /* Not used */
	SQLLEN      	displaysize;
	SQLUSMALLINT	colfieldid;
	int		charextraalloc;

	result->values = (odbc_result_value *) safe_emalloc(sizeof(odbc_result_value), result->numcols, 0);

	result->longreadlen = ODBCG(defaultlrl);
	result->binmode = ODBCG(defaultbinmode);

	for(i = 0; i < result->numcols; i++) {
		charextraalloc = 0;
		colfieldid = SQL_COLUMN_DISPLAY_SIZE;

		rc = SQLColAttributes(result->stmt, (SQLUSMALLINT)(i+1), SQL_COLUMN_NAME, 
				result->values[i].name, sizeof(result->values[i].name), &colnamelen, 0);
		rc = SQLColAttributes(result->stmt, (SQLUSMALLINT)(i+1), SQL_COLUMN_TYPE, 
				NULL, 0, NULL, &result->values[i].coltype);
		
		/* Don't bind LONG / BINARY columns, so that fetch behaviour can
		 * be controlled by odbc_binmode() / odbc_longreadlen()
		 */
		
		switch(result->values[i].coltype) {
			case SQL_BINARY:
			case SQL_VARBINARY:
			case SQL_LONGVARBINARY:
			case SQL_LONGVARCHAR:
#if defined(ODBCVER) && (ODBCVER >= 0x0300)
			case SQL_WLONGVARCHAR:
#endif
				result->values[i].value = NULL;
				break;
				
#ifdef HAVE_ADABAS
			case SQL_TIMESTAMP:
				result->values[i].value = (char *)emalloc(27);
				SQLBindCol(result->stmt, (SQLUSMALLINT)(i+1), SQL_C_CHAR, result->values[i].value,
							27, &result->values[i].vallen);
				break;
#endif /* HAVE_ADABAS */
			case SQL_CHAR:
			case SQL_VARCHAR:
#if defined(ODBCVER) && (ODBCVER >= 0x0300)
			case SQL_WCHAR:
			case SQL_WVARCHAR:
				colfieldid = SQL_DESC_OCTET_LENGTH;
#else
				charextraalloc = 1;
#endif
			default:
				rc = SQLColAttributes(result->stmt, (SQLUSMALLINT)(i+1), colfieldid,
								NULL, 0, NULL, &displaysize);
				/* Workaround for Oracle ODBC Driver bug (#50162) when fetching TIMESTAMP column */
				if (result->values[i].coltype == SQL_TIMESTAMP) {
					displaysize += 3;
				}

				if (charextraalloc) {
					/* Since we don't know the exact # of bytes, allocate extra */
					displaysize *= 4;
				}
				result->values[i].value = (char *)emalloc(displaysize + 1);
				rc = SQLBindCol(result->stmt, (SQLUSMALLINT)(i+1), SQL_C_CHAR, result->values[i].value,
							displaysize + 1, &result->values[i].vallen);
				break;
		}
	}
	return 1;
}
/* }}} */

/* {{{ odbc_transact */
void odbc_transact(INTERNAL_FUNCTION_PARAMETERS, int type)
{
	odbc_connection *conn;
	RETCODE rc;
	zval *pv_conn;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pv_conn) == FAILURE) {
		return;
	}

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_FALSE;
	}
	
	rc = SQLTransact(conn->henv, conn->hdbc, (SQLUSMALLINT)((type)?SQL_COMMIT:SQL_ROLLBACK));
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLTransact");
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ _close_pconn_with_res */
static int _close_pconn_with_res(zend_resource *le, zend_resource *res)
{
	if (le->type == le_pconn && (((odbc_connection *)(le->ptr))->res == res)){
		return 1;
	}else{
		return 0;
	}
}
/* }}} */

/* {{{ odbc_column_lengths */
void odbc_column_lengths(INTERNAL_FUNCTION_PARAMETERS, int type)
{
	odbc_result *result;
#if defined(HAVE_SOLID) || defined(HAVE_SOLID_30)
	/* this seems to be necessary for Solid2.3 ( tested by 
	 * tammy@synchronis.com) and Solid 3.0 (tested by eric@terra.telemediair.nl)
	 * Solid does not seem to declare a SQLINTEGER, but it does declare a
	 * SQL_INTEGER which does not work (despite being the same type as a SDWORD.
	 * Solid 3.5 does not have this issue.
	 */
	SDWORD len;
#else
	SQLLEN len;
#endif
	zval *pv_res;
	zend_long pv_num;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &pv_res, &pv_num) == FAILURE) {
		return;
	}

	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_FALSE;
	}

	if (result->numcols == 0) {
		php_error_docref(NULL, E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}

	if (pv_num > result->numcols) {
		php_error_docref(NULL, E_WARNING, "Field index larger than number of fields");
		RETURN_FALSE;
	}

	if (pv_num < 1) {
		php_error_docref(NULL, E_WARNING, "Field numbering starts at 1");
		RETURN_FALSE;
	}

	SQLColAttributes(result->stmt, (SQLUSMALLINT)pv_num, (SQLUSMALLINT) (type?SQL_COLUMN_SCALE:SQL_COLUMN_PRECISION), NULL, 0, NULL, &len);

	RETURN_LONG(len);
}
/* }}} */

/* Main User Functions */

/* {{{ proto void odbc_close_all(void)
   Close all ODBC connections */
PHP_FUNCTION(odbc_close_all)
{
	zend_resource *p;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	/* Loop through list and close all statements */
	ZEND_HASH_FOREACH_PTR(&EG(regular_list), p) {
		if (p->ptr && (p->type == le_result)) {
			zend_list_close(p);
		}
	} ZEND_HASH_FOREACH_END();

	/* Second loop through list, now close all connections */
	ZEND_HASH_FOREACH_PTR(&EG(regular_list), p) {
		if (p->ptr) {
			if (p->type == le_conn){
				zend_list_close(p);
			} else if (p->type == le_pconn){
				zend_list_close(p);
				/* Delete the persistent connection */
				zend_hash_apply_with_argument(&EG(persistent_list), 
					(apply_func_arg_t) _close_pconn_with_res, (void *)p);
			}
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ proto bool odbc_binmode(int result_id, int mode)
   Handle binary column data */
PHP_FUNCTION(odbc_binmode)
{
	php_odbc_fetch_attribs(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto bool odbc_longreadlen(int result_id, int length)
   Handle LONG columns */
PHP_FUNCTION(odbc_longreadlen)
{
	php_odbc_fetch_attribs(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto resource odbc_prepare(resource connection_id, string query)
   Prepares a statement for execution */
PHP_FUNCTION(odbc_prepare)
{
	zval *pv_conn;
	char *query;
	size_t query_len;
	odbc_result *result = NULL;
	odbc_connection *conn;
	RETCODE rc;
#ifdef HAVE_SQL_EXTENDED_FETCH
	SQLUINTEGER      scrollopts;
#endif

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs", &pv_conn, &query, &query_len) == FAILURE) {
		return;
	}

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_FALSE;
	}

	result = (odbc_result *)ecalloc(1, sizeof(odbc_result));
	
	result->numparams = 0;
	
	rc = SQLAllocStmt(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		efree(result);
		php_error_docref(NULL, E_WARNING, "SQLAllocStmt error 'Invalid Handle'");
		RETURN_FALSE;
	}

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLAllocStmt");
		efree(result);
		RETURN_FALSE;
	}

#ifdef HAVE_SQL_EXTENDED_FETCH
	/* Solid doesn't have ExtendedFetch, if DriverManager is used, get Info,
	   whether Driver supports ExtendedFetch */
	rc = SQLGetInfo(conn->hdbc, SQL_FETCH_DIRECTION, (void *) &scrollopts, sizeof(scrollopts), NULL);
	if (rc == SQL_SUCCESS) {
		if ((result->fetch_abs = (scrollopts & SQL_FD_FETCH_ABSOLUTE))) {
			/* Try to set CURSOR_TYPE to dynamic. Driver will replace this with other
			   type if not possible.
			*/
			SQLSetStmtOption(result->stmt, SQL_CURSOR_TYPE, ODBCG(default_cursortype));
		}
	} else {
		result->fetch_abs = 0;
	}
#endif

	rc = SQLPrepare(result->stmt, query, SQL_NTS);
	switch (rc) {
		case SQL_SUCCESS:
			break;
		case SQL_SUCCESS_WITH_INFO:
			odbc_sql_error(conn, result->stmt, "SQLPrepare");
			break;
		default:
			odbc_sql_error(conn, result->stmt, "SQLPrepare");
			RETURN_FALSE;
	}
	
	SQLNumParams(result->stmt, &(result->numparams));
	SQLNumResultCols(result->stmt, &(result->numcols));

	if (result->numcols > 0) {
		if (!odbc_bindcols(result)) {
			efree(result);
			RETURN_FALSE;
		}
	} else {
		result->values = NULL;
	}
	Z_ADDREF_P(pv_conn);
	result->conn_ptr = conn;
	result->fetched = 0;
	RETURN_RES(zend_register_resource(result, le_result));
}
/* }}} */

/*
 * Execute prepared SQL statement. Supports only input parameters.
 */

/* {{{ proto bool odbc_execute(resource result_id [, array parameters_array])
   Execute a prepared statement */
PHP_FUNCTION(odbc_execute)
{ 
	zval *pv_res, *pv_param_arr, *tmp;
	typedef struct params_t {
		SQLLEN vallen;
		int fp;
	} params_t;
	params_t *params = NULL;
	char *filename;
	unsigned char otype;
   	SQLSMALLINT sqltype, ctype, scale;
	SQLSMALLINT nullable;
	SQLULEN precision;
   	odbc_result *result;
	int numArgs, i, ne;
	RETCODE rc;
	
	numArgs = ZEND_NUM_ARGS();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|a", &pv_res, &pv_param_arr) == FAILURE) {
		return;
	}

	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_FALSE;
	}
	
	/* XXX check for already bound parameters*/
	if (result->numparams > 0 && numArgs == 1) {
		php_error_docref(NULL, E_WARNING, "No parameters to SQL statement given");
		RETURN_FALSE;
	}

	if (result->numparams > 0) {
		if ((ne = zend_hash_num_elements(Z_ARRVAL_P(pv_param_arr))) < result->numparams) {
			php_error_docref(NULL, E_WARNING,"Not enough parameters (%d should be %d) given", ne, result->numparams);
			RETURN_FALSE;
		}

		zend_hash_internal_pointer_reset(Z_ARRVAL_P(pv_param_arr));
		params = (params_t *)safe_emalloc(sizeof(params_t), result->numparams, 0);
		for(i = 0; i < result->numparams; i++) {
			params[i].fp = -1;
		}
		
		for(i = 1; i <= result->numparams; i++) {
			if ((tmp = zend_hash_get_current_data(Z_ARRVAL_P(pv_param_arr))) == NULL) {
				php_error_docref(NULL, E_WARNING,"Error getting parameter");
				SQLFreeStmt(result->stmt,SQL_RESET_PARAMS);
				for (i = 0; i < result->numparams; i++) {
					if (params[i].fp != -1) {
						close(params[i].fp);
					}
				}
				efree(params);
				RETURN_FALSE;
			}

			otype = Z_TYPE_P(tmp);
			convert_to_string_ex(tmp);
			if (Z_TYPE_P(tmp) != IS_STRING) {
				php_error_docref(NULL, E_WARNING,"Error converting parameter");
				SQLFreeStmt(result->stmt, SQL_RESET_PARAMS);
				for (i = 0; i < result->numparams; i++) {
					if (params[i].fp != -1) {
						close(params[i].fp);
					}
				}
				efree(params);
				RETURN_FALSE;
			}
			
			rc = SQLDescribeParam(result->stmt, (SQLUSMALLINT)i, &sqltype, &precision, &scale, &nullable);
			params[i-1].vallen = Z_STRLEN_P(tmp);
			params[i-1].fp = -1;
			if (rc == SQL_ERROR) {
				odbc_sql_error(result->conn_ptr, result->stmt, "SQLDescribeParameter");	
				SQLFreeStmt(result->stmt, SQL_RESET_PARAMS);
				for (i = 0; i < result->numparams; i++) {
					if (params[i].fp != -1) {
						close(params[i].fp);
					}
				}
				efree(params);
				RETURN_FALSE;
			}

			if (IS_SQL_BINARY(sqltype)) {
				ctype = SQL_C_BINARY;
			} else {
				ctype = SQL_C_CHAR;
			}

			if (Z_STRLEN_P(tmp) > 2 &&
				Z_STRVAL_P(tmp)[0] == '\'' &&
				Z_STRVAL_P(tmp)[Z_STRLEN_P(tmp) - 1] == '\'') {
					
				if (CHECK_ZVAL_NULL_PATH(tmp)) {
					RETURN_FALSE;
				}
				filename = estrndup(&Z_STRVAL_P(tmp)[1], Z_STRLEN_P(tmp) - 2);
				filename[strlen(filename)] = '\0';

				/* Check the basedir */
				if (php_check_open_basedir(filename)) {
					efree(filename);
					SQLFreeStmt(result->stmt, SQL_RESET_PARAMS);
					for (i = 0; i < result->numparams; i++) {
						if (params[i].fp != -1) {
							close(params[i].fp);
						}
					}
					efree(params);
					RETURN_FALSE;
				}

				if ((params[i-1].fp = open(filename,O_RDONLY)) == -1) {
					php_error_docref(NULL, E_WARNING,"Can't open file %s", filename);
					SQLFreeStmt(result->stmt, SQL_RESET_PARAMS);
					for (i = 0; i < result->numparams; i++) {
						if (params[i].fp != -1) {
							close(params[i].fp);
						}
					}
					efree(params);
					efree(filename);
					RETURN_FALSE;
				}

				efree(filename);

				params[i-1].vallen = SQL_LEN_DATA_AT_EXEC(0);

				rc = SQLBindParameter(result->stmt, (SQLUSMALLINT)i, SQL_PARAM_INPUT,
									  ctype, sqltype, precision, scale,
									  (void *)(intptr_t)params[i-1].fp, 0,
									  &params[i-1].vallen);
			} else {
#ifdef HAVE_DBMAKER
				precision = params[i-1].vallen;
#endif
				if (otype == IS_NULL) {
					params[i-1].vallen = SQL_NULL_DATA;
				}

				rc = SQLBindParameter(result->stmt, (SQLUSMALLINT)i, SQL_PARAM_INPUT,
									  ctype, sqltype, precision, scale,
									  Z_STRVAL_P(tmp), 0,
									  &params[i-1].vallen);
			}
			if (rc == SQL_ERROR) {
				odbc_sql_error(result->conn_ptr, result->stmt, "SQLBindParameter");	
				SQLFreeStmt(result->stmt, SQL_RESET_PARAMS);
				for (i = 0; i < result->numparams; i++) {
					if (params[i].fp != -1) {
						close(params[i].fp);
					}
				}
				efree(params);
				RETURN_FALSE;
			}
			zend_hash_move_forward(Z_ARRVAL_P(pv_param_arr));
		}
	}
	/* Close cursor, needed for doing multiple selects */
	rc = SQLFreeStmt(result->stmt, SQL_CLOSE);

	if (rc == SQL_ERROR) {
		odbc_sql_error(result->conn_ptr, result->stmt, "SQLFreeStmt");	
	}

	rc = SQLExecute(result->stmt);

	result->fetched = 0;
	if (rc == SQL_NEED_DATA) {
		char buf[4096];
		int fp, nbytes;
		while (rc == SQL_NEED_DATA) {
			rc = SQLParamData(result->stmt, (void*)&fp);
			if (rc == SQL_NEED_DATA) {
				while ((nbytes = read(fp, &buf, 4096)) > 0) {
					SQLPutData(result->stmt, (void*)&buf, nbytes);
				}
			}
		}
	} else {
		switch (rc) {
			case SQL_SUCCESS:
				break;
			case SQL_NO_DATA_FOUND:
			case SQL_SUCCESS_WITH_INFO:
				odbc_sql_error(result->conn_ptr, result->stmt, "SQLExecute");
				break;
			default:
				odbc_sql_error(result->conn_ptr, result->stmt, "SQLExecute");
				RETVAL_FALSE;
		}
	}	
	
	if (result->numparams > 0) {
		SQLFreeStmt(result->stmt, SQL_RESET_PARAMS);
		for(i = 0; i < result->numparams; i++) {
			if (params[i].fp != -1) {
				close(params[i].fp);
			}
		}
		efree(params);
	}

	if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO || rc == SQL_NO_DATA_FOUND) {
		RETVAL_TRUE;
	}

	if (result->numcols == 0) {
		SQLNumResultCols(result->stmt, &(result->numcols));

		if (result->numcols > 0) {
			if (!odbc_bindcols(result)) {
				efree(result);
				RETVAL_FALSE;
			}
		} else {
			result->values = NULL;
		}
	}
}
/* }}} */

/* {{{ proto string odbc_cursor(resource result_id)
   Get cursor name */
PHP_FUNCTION(odbc_cursor)
{
	zval *pv_res;
	SQLUSMALLINT max_len;
	SQLSMALLINT len;
	char *cursorname;
   	odbc_result *result;
	RETCODE rc;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pv_res) == FAILURE) {
		return;
	}

	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_FALSE;
	}

	rc = SQLGetInfo(result->conn_ptr->hdbc,SQL_MAX_CURSOR_NAME_LEN, (void *)&max_len,sizeof(max_len),&len);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		RETURN_FALSE;
	}
	
	if (max_len > 0) {
		cursorname = emalloc(max_len + 1);
		rc = SQLGetCursorName(result->stmt,cursorname,(SQLSMALLINT)max_len,&len);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			char        state[6];     /* Not used */
	 		SQLINTEGER  error;        /* Not used */
			char        errormsg[SQL_MAX_MESSAGE_LENGTH];
			SQLSMALLINT errormsgsize; /* Not used */

			SQLError( result->conn_ptr->henv, result->conn_ptr->hdbc,
						result->stmt, state, &error, errormsg,
						sizeof(errormsg)-1, &errormsgsize);
			if (!strncmp(state,"S1015",5)) {
				snprintf(cursorname, max_len+1, "php_curs_%d", (int)result->stmt);
				if (SQLSetCursorName(result->stmt,cursorname,SQL_NTS) != SQL_SUCCESS) {
					odbc_sql_error(result->conn_ptr, result->stmt, "SQLSetCursorName");
					RETVAL_FALSE;
				} else {
					RETVAL_STRING(cursorname);
				}
			} else {
				php_error_docref(NULL, E_WARNING, "SQL error: %s, SQL state %s", errormsg, state);
				RETVAL_FALSE;
			}
		} else {
			RETVAL_STRING(cursorname);
		}
		efree(cursorname);
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

#ifdef HAVE_SQLDATASOURCES
/* {{{ proto array odbc_data_source(resource connection_id, int fetch_type)
   Return information about the currently connected data source */
PHP_FUNCTION(odbc_data_source)
{
	zval *zv_conn;
	zend_long zv_fetch_type;
	RETCODE rc = 0; /* assume all is good */
	odbc_connection *conn;
	UCHAR server_name[100], desc[200];
	SQLSMALLINT len1=0, len2=0, fetch_type;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &zv_conn, &zv_fetch_type) == FAILURE) {
		return;
	}

	fetch_type = (SQLSMALLINT) zv_fetch_type;

	if (!(fetch_type == SQL_FETCH_FIRST || fetch_type == SQL_FETCH_NEXT)) {
		php_error_docref(NULL, E_WARNING, "Invalid fetch type (%d)", fetch_type);
		RETURN_FALSE;
	}

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(zv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_FALSE;
	}

	/* now we have the "connection" lets call the DataSource object */
	rc = SQLDataSources(conn->henv, 
			fetch_type,
			server_name,
			(SQLSMALLINT)sizeof(server_name),
			&len1,
			desc, 
			(SQLSMALLINT)sizeof(desc),
			&len2);

	if (rc != SQL_SUCCESS) {
		/* ummm.... he did it */
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLDataSources");
		RETURN_FALSE;
	}

	if (len1 == 0 || len2 == 0) {
		/* we have a non-valid entry... so stop the looping */
		RETURN_FALSE;
	}

	array_init(return_value);

	add_assoc_string_ex(return_value, "server", sizeof("server")-1, server_name);
	add_assoc_string_ex(return_value, "description", sizeof("description")-1, desc);

}
/* }}} */
#endif /* HAVE_SQLDATASOURCES */

/* {{{ proto resource odbc_exec(resource connection_id, string query [, int flags])
   Prepare and execute an SQL statement */
/* XXX Use flags */
PHP_FUNCTION(odbc_exec)
{
	zval *pv_conn;
	zend_long pv_flags;
	int numArgs;
	char *query;
	size_t query_len;
	odbc_result *result = NULL;
	odbc_connection *conn;
	RETCODE rc;
#ifdef HAVE_SQL_EXTENDED_FETCH
	SQLUINTEGER      scrollopts;
#endif

	numArgs = ZEND_NUM_ARGS();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs|l", &pv_conn, &query, &query_len, &pv_flags) == FAILURE) {
		return;
	}

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_FALSE;
	}
		
	result = (odbc_result *)ecalloc(1, sizeof(odbc_result));

	rc = SQLAllocStmt(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		php_error_docref(NULL, E_WARNING, "SQLAllocStmt error 'Invalid Handle'");
		efree(result);
		RETURN_FALSE;
	}

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLAllocStmt");
		efree(result);
		RETURN_FALSE;
	}
	
#ifdef HAVE_SQL_EXTENDED_FETCH
	/* Solid doesn't have ExtendedFetch, if DriverManager is used, get Info,
	   whether Driver supports ExtendedFetch */
	rc = SQLGetInfo(conn->hdbc, SQL_FETCH_DIRECTION, (void *) &scrollopts, sizeof(scrollopts), NULL);
	if (rc == SQL_SUCCESS) {
		if ((result->fetch_abs = (scrollopts & SQL_FD_FETCH_ABSOLUTE))) {
			/* Try to set CURSOR_TYPE to dynamic. Driver will replace this with other
			   type if not possible.
			 */
			SQLSetStmtOption(result->stmt, SQL_CURSOR_TYPE, ODBCG(default_cursortype));
		}
	} else {
		result->fetch_abs = 0;
	}
#endif

	rc = SQLExecDirect(result->stmt, query, SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO && rc != SQL_NO_DATA_FOUND) { 
		/* XXX FIXME we should really check out SQLSTATE with SQLError
		 * in case rc is SQL_SUCCESS_WITH_INFO here.
		 */
		odbc_sql_error(conn, result->stmt, "SQLExecDirect"); 
		SQLFreeStmt(result->stmt, SQL_DROP);
		efree(result);
		RETURN_FALSE;
	}

	SQLNumResultCols(result->stmt, &(result->numcols));
	
	/* For insert, update etc. cols == 0 */
	if (result->numcols > 0) {
		if (!odbc_bindcols(result)) {
			efree(result);
			RETURN_FALSE;
		}
	} else {
		result->values = NULL;
	}
	Z_ADDREF_P(pv_conn);
	result->conn_ptr = conn;
	result->fetched = 0;
	RETURN_RES(zend_register_resource(result, le_result));
}
/* }}} */

#ifdef PHP_ODBC_HAVE_FETCH_HASH
#define ODBC_NUM  1
#define ODBC_OBJECT  2

/* {{{ php_odbc_fetch_hash */
static void php_odbc_fetch_hash(INTERNAL_FUNCTION_PARAMETERS, int result_type)
{
	int i;
	odbc_result *result;
	RETCODE rc;
	SQLSMALLINT sql_c_type;
	char *buf = NULL;
#ifdef HAVE_SQL_EXTENDED_FETCH
	SQLULEN crow;
	SQLUSMALLINT RowStatus[1];
	SQLLEN rownum;
	zval *pv_res, tmp;
	zend_long pv_row = -1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|l", &pv_res, &pv_row) == FAILURE) {
		return;
	}
	
	rownum = pv_row;
#else
	zval *pv_res, tmp;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pv_res) == FAILURE) {
		return;
	}
#endif

	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_FALSE;
	}

	if (result->numcols == 0) {
		php_error_docref(NULL, E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}

#ifdef HAVE_SQL_EXTENDED_FETCH
	if (result->fetch_abs) {
		if (rownum > 0) {
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_ABSOLUTE,rownum,&crow,RowStatus);
		} else {
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
		}
	} else
#endif
	rc = SQLFetch(result->stmt);

	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		RETURN_FALSE;
	}
	
	array_init(return_value);
	
#ifdef HAVE_SQL_EXTENDED_FETCH
	if (rownum > 0 && result->fetch_abs)
		result->fetched = rownum;
	else
#endif
		result->fetched++;

	for(i = 0; i < result->numcols; i++) {
		sql_c_type = SQL_C_CHAR;

		switch(result->values[i].coltype) {
			case SQL_BINARY:
			case SQL_VARBINARY:
			case SQL_LONGVARBINARY:
				if (result->binmode <= 0) {
					ZVAL_EMPTY_STRING(&tmp);
					break;
				}
				if (result->binmode == 1) {
					sql_c_type = SQL_C_BINARY;
				}
			case SQL_LONGVARCHAR:
#if defined(ODBCVER) && (ODBCVER >= 0x0300)
			case SQL_WLONGVARCHAR:
#endif
				if (IS_SQL_LONG(result->values[i].coltype) && result->longreadlen <= 0) {
					ZVAL_EMPTY_STRING(&tmp);
					break;
				}				
				if (buf == NULL) {
					buf = emalloc(result->longreadlen + 1);
				}
				
				rc = SQLGetData(result->stmt, (SQLUSMALLINT)(i + 1), sql_c_type, buf, result->longreadlen + 1, &result->values[i].vallen);

				if (rc == SQL_ERROR) {
					odbc_sql_error(result->conn_ptr, result->stmt, "SQLGetData");
					efree(buf);
					RETURN_FALSE;
				}

				if (rc == SQL_SUCCESS_WITH_INFO) {
					ZVAL_STRINGL(&tmp, buf, result->longreadlen);
				} else if (result->values[i].vallen == SQL_NULL_DATA) {
					ZVAL_NULL(&tmp);
					break;
				} else {
					ZVAL_STRINGL(&tmp, buf, result->values[i].vallen);
				}
				break;

			default:
				if (result->values[i].vallen == SQL_NULL_DATA) {
					ZVAL_NULL(&tmp);
					break;
				}
				ZVAL_STRINGL(&tmp, result->values[i].value, result->values[i].vallen);
				break;
		}

		if (result_type & ODBC_NUM) {
			zend_hash_index_update(Z_ARRVAL_P(return_value), i, &tmp);
		} else {
			if (!*(result->values[i].name) && Z_TYPE(tmp) == IS_STRING) {
				zend_hash_update(Z_ARRVAL_P(return_value), Z_STR(tmp), &tmp);
			} else {
				zend_hash_str_update(Z_ARRVAL_P(return_value), result->values[i].name, strlen(result->values[i].name), &tmp);
			}
		}
	}
	if (buf) {
		efree(buf);
	}
}
/* }}} */


/* {{{ proto object odbc_fetch_object(int result [, int rownumber])
   Fetch a result row as an object */
PHP_FUNCTION(odbc_fetch_object)
{
	php_odbc_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, ODBC_OBJECT);
	if (Z_TYPE_P(return_value) == IS_ARRAY) {
		object_and_properties_init(return_value, ZEND_STANDARD_CLASS_DEF_PTR, Z_ARRVAL_P(return_value));
	}
}
/* }}} */

/* {{{ proto array odbc_fetch_array(int result [, int rownumber])
   Fetch a result row as an associative array */
PHP_FUNCTION(odbc_fetch_array)
{
	php_odbc_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, ODBC_OBJECT);
}
/* }}} */
#endif

/* {{{ proto int odbc_fetch_into(resource result_id, array &result_array, [, int rownumber])
   Fetch one result row into an array */ 
PHP_FUNCTION(odbc_fetch_into)
{
	int i;
	odbc_result *result;
	RETCODE rc;
	SQLSMALLINT sql_c_type;
	char *buf = NULL;
	zval *pv_res, *pv_res_arr, tmp;
#ifdef HAVE_SQL_EXTENDED_FETCH
	zend_long pv_row = 0;
	SQLULEN crow;
	SQLUSMALLINT RowStatus[1];
	SQLLEN rownum = -1;
#endif /* HAVE_SQL_EXTENDED_FETCH */

#ifdef HAVE_SQL_EXTENDED_FETCH
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rz/|l", &pv_res, &pv_res_arr, &pv_row) == FAILURE) {
		return;
	}
	
	rownum = pv_row;
#else
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rz/", &pv_res, &pv_res_arr) == FAILURE) {
		return;
	}
#endif /* HAVE_SQL_EXTENDED_FETCH */

	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_FALSE;
	}
	
	if (result->numcols == 0) {
		php_error_docref(NULL, E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
	
	if (Z_TYPE_P(pv_res_arr) != IS_ARRAY) {
		array_init(pv_res_arr);
	}

#ifdef HAVE_SQL_EXTENDED_FETCH
	if (result->fetch_abs) {
		if (rownum > 0) {
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_ABSOLUTE,rownum,&crow,RowStatus);
		} else {
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
		}
	} else
#endif
		rc = SQLFetch(result->stmt);

	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		RETURN_FALSE;
	}

#ifdef HAVE_SQL_EXTENDED_FETCH
	if (rownum > 0 && result->fetch_abs)
		result->fetched = rownum;
	else
#endif
		result->fetched++;

	for(i = 0; i < result->numcols; i++) {
		sql_c_type = SQL_C_CHAR;

		switch(result->values[i].coltype) {
			case SQL_BINARY:
			case SQL_VARBINARY:
			case SQL_LONGVARBINARY:
				if (result->binmode <= 0) {
					ZVAL_EMPTY_STRING(&tmp);
					break;
				}
				if (result->binmode == 1) sql_c_type = SQL_C_BINARY; 

			case SQL_LONGVARCHAR:
#if defined(ODBCVER) && (ODBCVER >= 0x0300)
			case SQL_WLONGVARCHAR:
#endif
				if (IS_SQL_LONG(result->values[i].coltype) && result->longreadlen <= 0) {
					ZVAL_EMPTY_STRING(&tmp);
					break;
				}

				if (buf == NULL) {
					buf = emalloc(result->longreadlen + 1);
				}
				rc = SQLGetData(result->stmt, (SQLUSMALLINT)(i + 1),sql_c_type, buf, result->longreadlen + 1, &result->values[i].vallen);

				if (rc == SQL_ERROR) {
					odbc_sql_error(result->conn_ptr, result->stmt, "SQLGetData");
					efree(buf);
					RETURN_FALSE;
				}
				if (rc == SQL_SUCCESS_WITH_INFO) {
					ZVAL_STRINGL(&tmp, buf, result->longreadlen);
				} else if (result->values[i].vallen == SQL_NULL_DATA) {
					ZVAL_NULL(&tmp);
					break;
				} else {
					ZVAL_STRINGL(&tmp, buf, result->values[i].vallen);
				}
				break;

			default:
				if (result->values[i].vallen == SQL_NULL_DATA) {
					ZVAL_NULL(&tmp);
					break;
				}
				ZVAL_STRINGL(&tmp, result->values[i].value, result->values[i].vallen);
				break;
		}
		zend_hash_index_update(Z_ARRVAL_P(pv_res_arr), i, &tmp);
	}
	if (buf) efree(buf);
	RETURN_LONG(result->numcols);	
}
/* }}} */

/* {{{ proto bool solid_fetch_prev(resource result_id)
   */ 
#if defined(HAVE_SOLID) || defined(HAVE_SOLID_30) || defined(HAVE_SOLID_35)
PHP_FUNCTION(solid_fetch_prev)
{
	odbc_result *result;
	RETCODE rc;
	zval *pv_res;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pv_res) == FAILURE) {
		return;
	}
	
	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_FALSE;
	}
	if (result->numcols == 0) {
		php_error_docref(NULL, E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
	rc = SQLFetchPrev(result->stmt);

	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		RETURN_FALSE;
	}

	if (result->fetched > 1) {
		result->fetched--;
	}

	RETURN_TRUE;
}
#endif
/* }}} */

/* {{{ proto bool odbc_fetch_row(resource result_id [, int row_number])
   Fetch a row */
PHP_FUNCTION(odbc_fetch_row)
{
	SQLLEN rownum;
	odbc_result *result;
	RETCODE rc;
	zval *pv_res;
	zend_long pv_row = 1;
#ifdef HAVE_SQL_EXTENDED_FETCH
	SQLULEN crow;
	SQLUSMALLINT RowStatus[1];
#endif

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|l", &pv_res, &pv_row) == FAILURE) {
		return;
	}
	
	rownum = pv_row;
	
	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_FALSE;
	}
	
	if (result->numcols == 0) {
		php_error_docref(NULL, E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}

#ifdef HAVE_SQL_EXTENDED_FETCH
	if (result->fetch_abs) {
		if (ZEND_NUM_ARGS() > 1) {
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_ABSOLUTE,rownum,&crow,RowStatus);
		} else {
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
		}
	} else
#endif
		rc = SQLFetch(result->stmt);

	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		RETURN_FALSE;
	}
	
	if (ZEND_NUM_ARGS() > 1) {
		result->fetched = rownum;
	} else {
		result->fetched++;
	}
	
	RETURN_TRUE;
}	
/* }}} */

/* {{{ proto mixed odbc_result(resource result_id, mixed field)
   Get result data */ 
PHP_FUNCTION(odbc_result)
{
	char *field;
	int field_ind;
	SQLSMALLINT sql_c_type = SQL_C_CHAR;
	odbc_result *result;
	int i = 0;
	RETCODE rc;
	SQLLEN	fieldsize;
	zval *pv_res, *pv_field;
#ifdef HAVE_SQL_EXTENDED_FETCH
	SQLULEN crow;
	SQLUSMALLINT RowStatus[1];
#endif

	field_ind = -1;
	field = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rz", &pv_res, &pv_field) == FAILURE) {
		return;
	}
	
	if (Z_TYPE_P(pv_field) == IS_STRING) {
		field = Z_STRVAL_P(pv_field);
	} else {
		convert_to_long_ex(pv_field);
		field_ind = Z_LVAL_P(pv_field) - 1;
	}
	
	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_FALSE;
	}
	
	if ((result->numcols == 0)) {
		php_error_docref(NULL, E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
	
	/* get field index if the field parameter was a string */
	if (field != NULL) {
		if (result->values == NULL) {
			php_error_docref(NULL, E_WARNING, "Result set contains no data");
			RETURN_FALSE;
		}

		for(i = 0; i < result->numcols; i++) {
			if (!strcasecmp(result->values[i].name, field)) {
				field_ind = i;
				break;
			}
		}

		if (field_ind < 0) {
			php_error_docref(NULL, E_WARNING, "Field %s not found", field);
			RETURN_FALSE;
		}
	} else {
		/* check for limits of field_ind if the field parameter was an int */
		if (field_ind >= result->numcols || field_ind < 0) {
			php_error_docref(NULL, E_WARNING, "Field index is larger than the number of fields");
			RETURN_FALSE;
		}
	}

	if (result->fetched == 0) {
		/* User forgot to call odbc_fetch_row(), or wants to reload the results, do it now */
#ifdef HAVE_SQL_EXTENDED_FETCH
		if (result->fetch_abs)
			rc = SQLExtendedFetch(result->stmt, SQL_FETCH_NEXT, 1, &crow,RowStatus);
		else
#endif
			rc = SQLFetch(result->stmt);

		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			RETURN_FALSE;
		}
		
		result->fetched++;
	}

	switch(result->values[field_ind].coltype) {
		case SQL_BINARY:
		case SQL_VARBINARY:
		case SQL_LONGVARBINARY:
			if (result->binmode <= 1) {
				sql_c_type = SQL_C_BINARY;
			}
			if (result->binmode <= 0) {
				break; 
			}
		case SQL_LONGVARCHAR:
#if defined(ODBCVER) && (ODBCVER >= 0x0300)
		case SQL_WLONGVARCHAR:
#endif
			if (IS_SQL_LONG(result->values[field_ind].coltype)) {
				if (result->longreadlen <= 0) {
				   break;
				} else {
				   fieldsize = result->longreadlen;
				}
			} else {
			   SQLColAttributes(result->stmt, (SQLUSMALLINT)(field_ind + 1), 
					   			(SQLUSMALLINT)((sql_c_type == SQL_C_BINARY) ? SQL_COLUMN_LENGTH :
					   			SQL_COLUMN_DISPLAY_SIZE),
					   			NULL, 0, NULL, &fieldsize);
			}
			/* For char data, the length of the returned string will be longreadlen - 1 */
			fieldsize = (result->longreadlen <= 0) ? 4096 : result->longreadlen;
			field = emalloc(fieldsize);

		/* SQLGetData will truncate CHAR data to fieldsize - 1 bytes and append \0.
		 * For binary data it is truncated to fieldsize bytes. 
		 */
			rc = SQLGetData(result->stmt, (SQLUSMALLINT)(field_ind + 1), sql_c_type,
							field, fieldsize, &result->values[field_ind].vallen);

			if (rc == SQL_ERROR) {
				odbc_sql_error(result->conn_ptr, result->stmt, "SQLGetData");
				efree(field);
				RETURN_FALSE;
			}

			if (result->values[field_ind].vallen == SQL_NULL_DATA) {
				efree(field);
				RETURN_NULL();
			} else if (rc == SQL_NO_DATA_FOUND) {
				efree(field);
				RETURN_FALSE;
			}
			/* Reduce fieldlen by 1 if we have char data. One day we might 
			   have binary strings... */
			if ((result->values[field_ind].coltype == SQL_LONGVARCHAR)
#if defined(ODBCVER) && (ODBCVER >= 0x0300)
			    || (result->values[field_ind].coltype == SQL_WLONGVARCHAR)
#endif
			) {
				fieldsize -= 1;
			}
			/* Don't duplicate result, saves one emalloc.
			   For SQL_SUCCESS, the length is in vallen.
			 */
			RETVAL_STRINGL(field, (rc == SQL_SUCCESS_WITH_INFO) ? fieldsize : result->values[field_ind].vallen);
			// TODO: avoid dpouble reallocation ???
			efree(field);
			return;
			break;
			
		default:
			if (result->values[field_ind].vallen == SQL_NULL_DATA) {
				RETURN_NULL();
			} else {
				RETURN_STRINGL(result->values[field_ind].value, result->values[field_ind].vallen);
			}
			break;
	}

/* If we come here, output unbound LONG and/or BINARY column data to the client */
	
	/* We emalloc 1 byte more for SQL_C_CHAR (trailing \0) */
	fieldsize = (sql_c_type == SQL_C_CHAR) ? 4096 : 4095;
	field = emalloc(fieldsize);
	
	/* Call SQLGetData() until SQL_SUCCESS is returned */
	while(1) {
		rc = SQLGetData(result->stmt, (SQLUSMALLINT)(field_ind + 1),sql_c_type, field, fieldsize, &result->values[field_ind].vallen);

		if (rc == SQL_ERROR) {
			odbc_sql_error(result->conn_ptr, result->stmt, "SQLGetData");
			efree(field);
			RETURN_FALSE;
		}
		
		if (result->values[field_ind].vallen == SQL_NULL_DATA) {
			efree(field);
			RETURN_NULL();
		}
		/* chop the trailing \0 by outputing only 4095 bytes */
		PHPWRITE(field,(rc == SQL_SUCCESS_WITH_INFO) ? 4095 : result->values[field_ind].vallen);

		if (rc == SQL_SUCCESS) { /* no more data avail */
			efree(field);
			RETURN_TRUE;
		}
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int odbc_result_all(resource result_id [, string format])
   Print result as HTML table */
PHP_FUNCTION(odbc_result_all)
{
	char *buf = NULL;
	odbc_result *result;
	RETCODE rc;
	zval *pv_res;
	char *pv_format = NULL;
	size_t i, pv_format_len = 0;
	SQLSMALLINT sql_c_type;
#ifdef HAVE_SQL_EXTENDED_FETCH
	SQLULEN crow;
	SQLUSMALLINT RowStatus[1];
#endif

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|s", &pv_res, &pv_format, &pv_format_len) == FAILURE) {
		return;
	}
				
	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_FALSE;
	}
	
	if (result->numcols == 0) {
		php_error_docref(NULL, E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
#ifdef HAVE_SQL_EXTENDED_FETCH
	if (result->fetch_abs)
		rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
	else
#endif	
		rc = SQLFetch(result->stmt);

	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		php_printf("<h2>No rows found</h2>\n");
		RETURN_LONG(0);
	}
	
	/* Start table tag */
	if (ZEND_NUM_ARGS() == 1) {
		php_printf("<table><tr>");
	} else {
		php_printf("<table %s ><tr>", pv_format);
	}
	
	for (i = 0; i < result->numcols; i++) {
		php_printf("<th>%s</th>", result->values[i].name);
	}

	php_printf("</tr>\n");

	while(rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) {
		result->fetched++;
		php_printf("<tr>");
		for(i = 0; i < result->numcols; i++) {
			sql_c_type = SQL_C_CHAR;
			switch(result->values[i].coltype) {
				case SQL_BINARY:
				case SQL_VARBINARY:
				case SQL_LONGVARBINARY:
					if (result->binmode <= 0) {
						php_printf("<td>Not printable</td>");
						break;
					}
					if (result->binmode <= 1) sql_c_type = SQL_C_BINARY; 
				case SQL_LONGVARCHAR:
#if defined(ODBCVER) && (ODBCVER >= 0x0300)
				case SQL_WLONGVARCHAR:
#endif
					if (IS_SQL_LONG(result->values[i].coltype) && 
						result->longreadlen <= 0) {
						php_printf("<td>Not printable</td>"); 
						break;
					}

					if (buf == NULL) {
						buf = emalloc(result->longreadlen);
					}

					rc = SQLGetData(result->stmt, (SQLUSMALLINT)(i + 1),sql_c_type, buf, result->longreadlen, &result->values[i].vallen);
 
					php_printf("<td>");

					if (rc == SQL_ERROR) {
						odbc_sql_error(result->conn_ptr, result->stmt, "SQLGetData");
						php_printf("</td></tr></table>");
						efree(buf);
						RETURN_FALSE;
					}
					if (rc == SQL_SUCCESS_WITH_INFO) {
						PHPWRITE(buf, result->longreadlen);
					} else if (result->values[i].vallen == SQL_NULL_DATA) {
						php_printf("<td>NULL</td>");
						break;
					} else {
						PHPWRITE(buf, result->values[i].vallen);
					}
					php_printf("</td>");
					break;
				default:
					if (result->values[i].vallen == SQL_NULL_DATA) {
						php_printf("<td>NULL</td>");
					} else {
						php_printf("<td>%s</td>", result->values[i].value);
					}
					break;
			}
		}
   		php_printf("</tr>\n");

#ifdef HAVE_SQL_EXTENDED_FETCH
		if (result->fetch_abs)
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
		else
#endif
			rc = SQLFetch(result->stmt);		
	}
	php_printf("</table>\n");
	if (buf) efree(buf);
	RETURN_LONG(result->fetched);
}
/* }}} */

/* {{{ proto bool odbc_free_result(resource result_id)
   Free resources associated with a result */
PHP_FUNCTION(odbc_free_result)
{
	zval *pv_res;
	odbc_result *result;
	int i;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pv_res) == FAILURE) {
		return;
	}

	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_FALSE;
	}

	if (result->values) {
		for (i = 0; i < result->numcols; i++) {
			if (result->values[i].value) {
				efree(result->values[i].value);
			}
		}
		efree(result->values);
		result->values = NULL;
	}
			
	zend_list_close(Z_RES_P(pv_res));
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto resource odbc_connect(string DSN, string user, string password [, int cursor_option])
   Connect to a datasource */
PHP_FUNCTION(odbc_connect)
{
	odbc_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto resource odbc_pconnect(string DSN, string user, string password [, int cursor_option])
   Establish a persistent connection to a datasource */
PHP_FUNCTION(odbc_pconnect)
{
	odbc_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ odbc_sqlconnect */
int odbc_sqlconnect(odbc_connection **conn, char *db, char *uid, char *pwd, int cur_opt, int persistent)
{
	RETCODE rc;
	
	*conn = (odbc_connection *)pemalloc(sizeof(odbc_connection), persistent);
	(*conn)->persistent = persistent;
	SQLAllocEnv(&((*conn)->henv));
	SQLAllocConnect((*conn)->henv, &((*conn)->hdbc));
	
#if defined(HAVE_SOLID) || defined(HAVE_SOLID_30) 
	SQLSetConnectOption((*conn)->hdbc, SQL_TRANSLATE_OPTION,
			SQL_SOLID_XLATOPT_NOCNV);
#endif
#ifdef HAVE_ODBC_ROUTER
	{
#define CONNSTRSIZE 2048
	 char *lpszConnStr = emalloc(CONNSTRSIZE);
	 if (lpszConnStr && db) {
		 short cbszConnStr;
		 if (strstr(db, ";")) {
			 /* the caller has apparently passed a connection-string */
			 if (strstr(db, "uid") || strstr(db, "UID")) {
				 uid = NULL;
			 }
			 if (strstr(db, "pwd") || strstr(db, "PWD")) {
				 pwd = NULL;
			 }
			 strlcpy( lpszConnStr, db, CONNSTRSIZE);
		 }
		 else {
			 strcpy(lpszConnStr, "DSN=");
			 strlcat(lpszConnStr, db, CONNSTRSIZE);
		 }
		 if (uid) {
			 if (uid[0]) {
				 strlcat(lpszConnStr, ";UID=", CONNSTRSIZE);
				 strlcat(lpszConnStr, uid, CONNSTRSIZE);
				 strlcat(lpszConnStr, ";", CONNSTRSIZE);
			 }
			 if (pwd) {
				 if (pwd[0]) {
					 strlcat(lpszConnStr, "PWD=", CONNSTRSIZE);
					 strlcat(lpszConnStr, pwd, CONNSTRSIZE);
					 strlcat(lpszConnStr, ";", CONNSTRSIZE);
				 }
			 }
		 }
		 rc = SQLDriverConnect((*conn)->hdbc, NULL, lpszConnStr, SQL_NTS, lpszConnStr, CONNSTRSIZE, &cbszConnStr, SQL_DRIVER_NOPROMPT);
		 efree(lpszConnStr);
	 }
	}
#else
#ifdef HAVE_OPENLINK
	{
		char dsnbuf[1024];
		short dsnbuflen;

		rc = SQLDriverConnect((*conn)->hdbc, NULL, db, SQL_NTS,	dsnbuf, sizeof(dsnbuf) - 1, &dsnbuflen, SQL_DRIVER_NOPROMPT);
	}
#else
	if (cur_opt != SQL_CUR_DEFAULT) {
		rc = SQLSetConnectOption((*conn)->hdbc, SQL_ODBC_CURSORS, cur_opt);
		if (rc != SQL_SUCCESS) {  /* && rc != SQL_SUCCESS_WITH_INFO ? */
			odbc_sql_error(*conn, SQL_NULL_HSTMT, "SQLSetConnectOption");
			SQLFreeConnect((*conn)->hdbc);
			pefree(*conn, persistent);
			return FALSE;
		}
	}
/*  Possible fix for bug #10250
 *  Needs testing on UnixODBC < 2.0.5 though. */
#if defined(HAVE_EMPRESS) || defined(HAVE_UNIXODBC) || defined(PHP_WIN32) || defined (HAVE_IODBC)
/* *  Uncomment the line above, and comment line below to fully test 
 * #ifdef HAVE_EMPRESS */
	{
		int     direct = 0;
		char    dsnbuf[1024];
		short   dsnbuflen;
		char    *ldb = 0;
		int		ldb_len = 0;

		if (strstr((char*)db, ";")) {
			direct = 1;
			if (uid && !strstr ((char*)db, "uid") && !strstr((char*)db, "UID")) {
				spprintf(&ldb, 0, "%s;UID=%s;PWD=%s", db, uid, pwd);
			} else {
				ldb_len = strlen(db)+1;
				ldb = (char*) emalloc(ldb_len);
				memcpy(ldb, db, ldb_len);
			}
		}

		if (direct) {
			rc = SQLDriverConnect((*conn)->hdbc, NULL, ldb, strlen(ldb), dsnbuf, sizeof(dsnbuf) - 1, &dsnbuflen, SQL_DRIVER_NOPROMPT);
		} else {
			rc = SQLConnect((*conn)->hdbc, db, SQL_NTS, uid, SQL_NTS, pwd, SQL_NTS);
		}

		if (ldb) {
			efree(ldb);
		}
	}
#else
	rc = SQLConnect((*conn)->hdbc, db, SQL_NTS, uid, SQL_NTS, pwd, SQL_NTS);
#endif
#endif
#endif
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		odbc_sql_error(*conn, SQL_NULL_HSTMT, "SQLConnect");
		SQLFreeConnect((*conn)->hdbc);
		pefree((*conn), persistent);
		return FALSE;
	}
/*	(*conn)->open = 1;*/
	return TRUE;
}
/* }}} */

/* Persistent connections: two list-types le_pconn, le_conn and a plist
 * where hashed connection info is stored together with index pointer to
 * the actual link of type le_pconn in the list. Only persistent 
 * connections get hashed up. Normal connections use existing pconnections.
 * Maybe this has to change with regard to transactions on pconnections?
 * Possibly set autocommit to on on request shutdown.
 *
 * We do have to hash non-persistent connections, and reuse connections.
 * In the case where two connects were being made, without closing the first
 * connect, access violations were occurring.  This is because some of the
 * "globals" in this module should actually be per-connection variables.  I
 * simply fixed things to get them working for now.  Shane
 */
/* {{{ odbc_do_connect */
void odbc_do_connect(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	char *db, *uid, *pwd;
	size_t db_len, uid_len, pwd_len;
	zend_long pv_opt = SQL_CUR_DEFAULT;
	odbc_connection *db_conn;
	char *hashed_details;
	int hashed_len, cur_opt;

	/*  Now an optional 4th parameter specifying the cursor type
	 *  defaulting to the cursors default
	 */
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sss|l", &db, &db_len, &uid, &uid_len, &pwd, &pwd_len, &pv_opt) == FAILURE) {
		return;
	}
	
	cur_opt = pv_opt;
	
	if (ZEND_NUM_ARGS() > 3) {
		/* Confirm the cur_opt range */
		if (! (cur_opt == SQL_CUR_USE_IF_NEEDED || 
			cur_opt == SQL_CUR_USE_ODBC || 
			cur_opt == SQL_CUR_USE_DRIVER || 
			cur_opt == SQL_CUR_DEFAULT) ) {
			php_error_docref(NULL, E_WARNING, "Invalid Cursor type (%d)", cur_opt);
			RETURN_FALSE;
		}
	}

	if (ODBCG(allow_persistent) <= 0) {
		persistent = 0;
	}

	hashed_len = spprintf(&hashed_details, 0, "%s_%s_%s_%s_%d", ODBC_TYPE, db, uid, pwd, cur_opt);

	/* FIXME the idea of checking to see if our connection is already persistent
		is good, but it adds a lot of overhead to non-persistent connections.  We
		should look and see if we can fix that somehow */
	/* try to find if we already have this link in our persistent list,
	 * no matter if it is to be persistent or not
	 */

try_and_get_another_connection:

	if (persistent) {
		zend_resource *le;
		
		/* the link is not in the persistent list */
		if ((le = zend_hash_str_find_ptr(&EG(persistent_list), hashed_details, hashed_len)) == NULL) {
			zend_resource new_le;
			
			if (ODBCG(max_links) != -1 && ODBCG(num_links) >= ODBCG(max_links)) {
				php_error_docref(NULL, E_WARNING, "Too many open links (%ld)", ODBCG(num_links));
				efree(hashed_details);
				RETURN_FALSE;
			}
			if (ODBCG(max_persistent) != -1 && ODBCG(num_persistent) >= ODBCG(max_persistent)) {
				php_error_docref(NULL, E_WARNING,"Too many open persistent links (%ld)", ODBCG(num_persistent));
				efree(hashed_details);
				RETURN_FALSE;
			}
			
			if (!odbc_sqlconnect(&db_conn, db, uid, pwd, cur_opt, 1)) {
				efree(hashed_details);
				RETURN_FALSE;
			}
			
			new_le.type = le_pconn;
			new_le.ptr = db_conn;
			new_le.handle = -1;
			if (zend_hash_str_update_mem(&EG(persistent_list), hashed_details, hashed_len, &new_le,
						sizeof(zend_resource)) == NULL) {
				free(db_conn);
				efree(hashed_details);
				RETURN_FALSE;
			}
			ODBCG(num_persistent)++;
			ODBCG(num_links)++;
			db_conn->res = zend_register_resource(db_conn, le_pconn);
			RETVAL_RES(db_conn->res);
		} else { /* found connection */
			if (le->type != le_pconn) {
				RETURN_FALSE;
			}
			/*
			 * check to see if the connection is still valid
			 */
			db_conn = (odbc_connection *)le->ptr;

			/*
			 * check to see if the connection is still in place (lurcher)
			 */
			if(ODBCG(check_persistent)){
				RETCODE ret;
				UCHAR d_name[32];
				SQLSMALLINT len;

				ret = SQLGetInfo(db_conn->hdbc, 
					SQL_DATA_SOURCE_READ_ONLY, 
					d_name, sizeof(d_name), &len);

				if(ret != SQL_SUCCESS || len == 0) {
					zend_hash_str_del(&EG(persistent_list), hashed_details, hashed_len);
					/* Commented out to fix a possible double closure error 
					 * when working with persistent connections as submitted by
					 * bug #15758
					 *
					 * safe_odbc_disconnect(db_conn->hdbc);
					 * SQLFreeConnect(db_conn->hdbc);
					 */
					goto try_and_get_another_connection;
				}
			}
		}
		db_conn->res = zend_register_resource(db_conn, le_pconn);
		RETVAL_RES(db_conn->res);
	} else { /* non persistent */
		zend_resource *index_ptr, new_index_ptr;
		
		if ((index_ptr = zend_hash_str_find_ptr(&EG(regular_list), hashed_details, hashed_len)) != NULL) {
			zend_ulong conn_id;
			zend_resource *p;

			if (index_ptr->type != le_index_ptr) {
				RETURN_FALSE;
			}
			conn_id = (zend_ulong)index_ptr->ptr;
			p = zend_hash_index_find_ptr(&EG(regular_list), conn_id);   /* check if the connection is still there */

			if (p && p->ptr && (p->type == le_conn || p->type == le_pconn)) {
				GC_REFCOUNT(p)++;
				RETVAL_RES(p);
				efree(hashed_details);
				return;
			} else {
				zend_hash_str_del(&EG(regular_list), hashed_details, hashed_len);
			}
		}
		if (ODBCG(max_links) != -1 && ODBCG(num_links) >= ODBCG(max_links)) {
			php_error_docref(NULL, E_WARNING,"Too many open connections (%ld)",ODBCG(num_links));
			efree(hashed_details);
			RETURN_FALSE;
		}

		if (!odbc_sqlconnect(&db_conn, db, uid, pwd, cur_opt, 0)) {
			efree(hashed_details);
			RETURN_FALSE;
		}
		db_conn->res = zend_register_resource(db_conn, le_conn);
		RETVAL_RES(db_conn->res);
		new_index_ptr.ptr = (void *)(zend_uintptr_t)Z_RES_HANDLE_P(return_value);
		new_index_ptr.type = le_index_ptr;

		if (zend_hash_str_update_mem(&EG(regular_list), hashed_details, hashed_len, (void *) &new_index_ptr,
				   sizeof(zend_resource)) == NULL) {
			efree(hashed_details);
			RETURN_FALSE;
			/* XXX Free Connection */
		}
		ODBCG(num_links)++;
	}
	efree(hashed_details);
}
/* }}} */

/* {{{ proto void odbc_close(resource connection_id)
   Close an ODBC connection */
PHP_FUNCTION(odbc_close)
{
	zval *pv_conn;
	zend_resource *p;
	odbc_connection *conn;
	odbc_result *res;
	int is_pconn = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pv_conn) == FAILURE) {
		return;
	}

	conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_conn), "ODBC-Link", le_conn, le_pconn);
	if (Z_RES_P(pv_conn)->type == le_pconn) {
		is_pconn = 1;
	}

	ZEND_HASH_FOREACH_PTR(&EG(regular_list), p) {
		if (p->ptr && (p->type == le_result)) {
			res = (odbc_result *)p->ptr;
			if (res->conn_ptr == conn) {
				zend_list_close(p);
			}
		}
	} ZEND_HASH_FOREACH_END();
	
	zend_list_close(Z_RES_P(pv_conn));
	
	if(is_pconn){
		zend_hash_apply_with_argument(&EG(persistent_list),	(apply_func_arg_t) _close_pconn_with_res, (void *) Z_RES_P(pv_conn));	
	}
}
/* }}} */

/* {{{ proto int odbc_num_rows(resource result_id)
   Get number of rows in a result */
PHP_FUNCTION(odbc_num_rows)
{
	odbc_result *result;
	SQLLEN rows;
	zval *pv_res;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pv_res) == FAILURE) {
		return;
	}

	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_FALSE;
	}

	SQLRowCount(result->stmt, &rows);
	RETURN_LONG(rows);
}
/* }}} */

#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30)
/* {{{ proto bool odbc_next_result(resource result_id)
   Checks if multiple results are available */
PHP_FUNCTION(odbc_next_result)
{
	odbc_result *result;
	zval *pv_res;
	int rc, i;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pv_res) == FAILURE) {
		return;
	}

	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_FALSE;
	}

	if (result->values) {
		for(i = 0; i < result->numcols; i++) {
			if (result->values[i].value) {
				efree(result->values[i].value);
			}
		}
		efree(result->values);
		result->values = NULL;
	}

	result->fetched = 0;
	rc = SQLMoreResults(result->stmt);
	if (rc == SQL_SUCCESS_WITH_INFO || rc == SQL_SUCCESS) {
		rc = SQLFreeStmt(result->stmt, SQL_UNBIND);
		SQLNumParams(result->stmt, &(result->numparams));
		SQLNumResultCols(result->stmt, &(result->numcols));

		if (result->numcols > 0) {
			if (!odbc_bindcols(result)) {
				efree(result);
				RETVAL_FALSE;
			}
		} else {
			result->values = NULL;
		}
		RETURN_TRUE;
	} else if (rc == SQL_NO_DATA_FOUND) {
		RETURN_FALSE;
	} else {
		odbc_sql_error(result->conn_ptr, result->stmt, "SQLMoreResults");
		RETURN_FALSE;
	}
}
/* }}} */
#endif

/* {{{ proto int odbc_num_fields(resource result_id)
   Get number of columns in a result */
PHP_FUNCTION(odbc_num_fields)
{
	odbc_result *result;
	zval *pv_res;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pv_res) == FAILURE) {
		return;
	}

	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_FALSE;
	}

	RETURN_LONG(result->numcols);
}
/* }}} */

/* {{{ proto string odbc_field_name(resource result_id, int field_number)
   Get a column name */
PHP_FUNCTION(odbc_field_name)
{
	odbc_result *result;
	zval *pv_res;
	zend_long pv_num;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &pv_res, &pv_num) == FAILURE) {
		return;
	}
	
	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_FALSE;
	}
	
	if (result->numcols == 0) {
		php_error_docref(NULL, E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
	
	if (pv_num > result->numcols) {
		php_error_docref(NULL, E_WARNING, "Field index larger than number of fields");
		RETURN_FALSE;
	}
	
	if (pv_num < 1) {
		php_error_docref(NULL, E_WARNING, "Field numbering starts at 1");
		RETURN_FALSE;
	}
	
	RETURN_STRING(result->values[pv_num - 1].name);
}
/* }}} */

/* {{{ proto string odbc_field_type(resource result_id, int field_number)
   Get the datatype of a column */
PHP_FUNCTION(odbc_field_type)
{
	odbc_result	*result;
	char    	tmp[32];
	SQLSMALLINT	tmplen;
	zval		*pv_res;
	zend_long		pv_num;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &pv_res, &pv_num) == FAILURE) {
		return;
	}

	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_FALSE;
	}

	if (result->numcols == 0) {
		php_error_docref(NULL, E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
	
	if (pv_num > result->numcols) {
		php_error_docref(NULL, E_WARNING, "Field index larger than number of fields");
		RETURN_FALSE;
	}

	if (pv_num < 1) {
		php_error_docref(NULL, E_WARNING, "Field numbering starts at 1");
		RETURN_FALSE;
	}

	SQLColAttributes(result->stmt, (SQLUSMALLINT)pv_num, SQL_COLUMN_TYPE_NAME, tmp, 31, &tmplen, NULL);
	RETURN_STRING(tmp)
}
/* }}} */

/* {{{ proto int odbc_field_len(resource result_id, int field_number)
   Get the length (precision) of a column */
PHP_FUNCTION(odbc_field_len)
{
	odbc_column_lengths(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int odbc_field_scale(resource result_id, int field_number)
   Get the scale of a column */
PHP_FUNCTION(odbc_field_scale)
{
	odbc_column_lengths(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);	
}
/* }}} */

/* {{{ proto int odbc_field_num(resource result_id, string field_name)
   Return column number */
PHP_FUNCTION(odbc_field_num)
{
	char *fname;
	size_t i, field_ind, fname_len;
	odbc_result *result;
	zval *pv_res;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs", &pv_res, &fname, &fname_len) == FAILURE) {
		return;
	}
	
	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_FALSE;
	}
	
	if (result->numcols == 0) {
		php_error_docref(NULL, E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}

	field_ind = -1;
	for(i = 0; i < result->numcols; i++) {
		if (strcasecmp(result->values[i].name, fname) == 0) {
			field_ind = i + 1;
		}
	}

	if (field_ind == -1) {
		RETURN_FALSE;
	}
	RETURN_LONG(field_ind);
}
/* }}} */

/* {{{ proto mixed odbc_autocommit(resource connection_id [, int OnOff])
   Toggle autocommit mode or get status */
/* There can be problems with pconnections!*/
PHP_FUNCTION(odbc_autocommit)
{
	odbc_connection *conn;
	RETCODE rc;
	zval *pv_conn;
	zend_long pv_onoff = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|l", &pv_conn, &pv_onoff) == FAILURE) {
		return;
	}

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_FALSE;
	}
	
	if (ZEND_NUM_ARGS() > 1) {
		rc = SQLSetConnectOption(conn->hdbc, SQL_AUTOCOMMIT, (pv_onoff) ? SQL_AUTOCOMMIT_ON : SQL_AUTOCOMMIT_OFF);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			odbc_sql_error(conn, SQL_NULL_HSTMT, "Set autocommit");
			RETURN_FALSE;
		}
		RETVAL_TRUE;
	} else {
		SQLINTEGER status;

		rc = SQLGetConnectOption(conn->hdbc, SQL_AUTOCOMMIT, (PTR)&status);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			odbc_sql_error(conn, SQL_NULL_HSTMT, "Get commit status");
			RETURN_FALSE;
		}
		RETVAL_LONG((zend_long)status);
	}
}
/* }}} */

/* {{{ proto bool odbc_commit(resource connection_id)
   Commit an ODBC transaction */
PHP_FUNCTION(odbc_commit)
{
	odbc_transact(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto bool odbc_rollback(resource connection_id)
   Rollback a transaction */
PHP_FUNCTION(odbc_rollback)
{
	odbc_transact(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ php_odbc_lasterror */
static void php_odbc_lasterror(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	odbc_connection *conn;
	zval *pv_handle;
	zend_string *ptr;
	int len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|r", &pv_handle) == FAILURE) {
		return;
	}

	if (mode == 0) {  /* last state */
		len = 6;
	} else { /* last error message */
		len = SQL_MAX_MESSAGE_LENGTH;
	}

	if (ZEND_NUM_ARGS() == 1) {
		if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_handle), "ODBC-Link", le_conn, le_pconn))) {
			RETURN_FALSE;
		}
		ptr = zend_string_alloc(len + 1, 0);
		if (mode == 0) {
			strlcpy(ptr->val, conn->laststate, len+1);
		} else {
			strlcpy(ptr->val, conn->lasterrormsg, len+1);
		}
	} else {
		ptr = zend_string_alloc(len, 0);
		if (mode == 0) {
			strlcpy(ptr->val, ODBCG(laststate), len+1);
		} else {
			strlcpy(ptr->val, ODBCG(lasterrormsg), len+1);
		}
	}
	RETVAL_STR(ptr);
}
/* }}} */

/* {{{ proto string odbc_error([resource connection_id])
   Get the last error code */
PHP_FUNCTION(odbc_error)
{
	php_odbc_lasterror(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto string odbc_errormsg([resource connection_id])
   Get the last error message */
PHP_FUNCTION(odbc_errormsg)
{
	php_odbc_lasterror(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto bool odbc_setoption(resource conn_id|result_id, int which, int option, int value)
   Sets connection or statement options */
/* This one has to be used carefully. We can't allow to set connection options for
   persistent connections. I think that SetStmtOption is of little use, since most
   of those can only be specified before preparing/executing statements.
   On the other hand, they can be made connection wide default through SetConnectOption
   - but will be overidden by calls to SetStmtOption() in odbc_prepare/odbc_do
*/
PHP_FUNCTION(odbc_setoption)
{
	odbc_connection *conn;
	odbc_result	*result;
	RETCODE rc;
	zval *pv_handle;
	zend_long pv_which, pv_opt, pv_val;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rlll", &pv_handle, &pv_which, &pv_opt, &pv_val) == FAILURE) {
		return;
	}

	switch (pv_which) {
		case 1:		/* SQLSetConnectOption */
			if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_handle), "ODBC-Link", le_conn, le_pconn))) {
				RETURN_FALSE;
			}

			if (conn->persistent) {
				php_error_docref(NULL, E_WARNING, "Unable to set option for persistent connection");
				RETURN_FALSE;
			}
			rc = SQLSetConnectOption(conn->hdbc, (unsigned short) pv_opt, pv_val);
			if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
				odbc_sql_error(conn, SQL_NULL_HSTMT, "SetConnectOption");
				RETURN_FALSE;
			}
			break;
		case 2:		/* SQLSetStmtOption */
			if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_handle), "ODBC result", le_result)) == NULL) {
				RETURN_FALSE;
			}
			
			rc = SQLSetStmtOption(result->stmt, (unsigned short) pv_opt, pv_val);

			if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
				odbc_sql_error(result->conn_ptr, result->stmt, "SetStmtOption");
				RETURN_FALSE;
			}
			break;
		default:
			php_error_docref(NULL, E_WARNING, "Unknown option type");
			RETURN_FALSE;
			break;
	}

	RETURN_TRUE;
}
/* }}} */

/*
 * metadata functions
 */

/* {{{ proto resource odbc_tables(resource connection_id [, string qualifier [, string owner [, string name [, string table_types]]]])
   Call the SQLTables function */
PHP_FUNCTION(odbc_tables)
{
	zval *pv_conn;
	odbc_result   *result = NULL;
	odbc_connection *conn;
	char *cat = NULL, *schema = NULL, *table = NULL, *type = NULL;
	size_t cat_len = 0, schema_len = 0, table_len = 0, type_len = 0;
	RETCODE rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|s!sss", &pv_conn, &cat, &cat_len, &schema, &schema_len, 
		&table, &table_len, &type, &type_len) == FAILURE) {
		return;
	}

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_FALSE;
	}

	result = (odbc_result *)ecalloc(1, sizeof(odbc_result));
	
	rc = SQLAllocStmt(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		efree(result);
		php_error_docref(NULL, E_WARNING, "SQLAllocStmt error 'Invalid Handle'");
		RETURN_FALSE;
	}

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLAllocStmt");
		efree(result);
		RETURN_FALSE;
	}

	/* This hack is needed to access table information in Access databases (fmk) */
	if (table && table_len && schema && schema_len == 0) {
		schema = NULL;
	}

	rc = SQLTables(result->stmt, 
			cat, SAFE_SQL_NTS(cat), 
			schema,	SAFE_SQL_NTS(schema), 
			table, SAFE_SQL_NTS(table),
			type, SAFE_SQL_NTS(type));

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLTables");
		efree(result);
		RETURN_FALSE;
	}

	result->numparams = 0;
	SQLNumResultCols(result->stmt, &(result->numcols));

	if (result->numcols > 0) {
		if (!odbc_bindcols(result)) {
			efree(result);
			RETURN_FALSE;
		}
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;
	RETURN_RES(zend_register_resource(result, le_result));
}
/* }}} */

/* {{{ proto resource odbc_columns(resource connection_id [, string qualifier [, string owner [, string table_name [, string column_name]]]])
   Returns a result identifier that can be used to fetch a list of column names in specified tables */
PHP_FUNCTION(odbc_columns)
{
	zval *pv_conn;
	odbc_result *result = NULL;
	odbc_connection *conn;
	char *cat = NULL, *schema = NULL, *table = NULL, *column = NULL;
	size_t cat_len = 0, schema_len = 0, table_len = 0, column_len = 0;
	RETCODE rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|s!sss", &pv_conn, &cat, &cat_len, &schema, &schema_len,
		&table, &table_len, &column, &column_len) == FAILURE) {
		return;
	}

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_FALSE;
	}

	result = (odbc_result *)ecalloc(1, sizeof(odbc_result));
	
	rc = SQLAllocStmt(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		efree(result);
		php_error_docref(NULL, E_WARNING, "SQLAllocStmt error 'Invalid Handle'");
		RETURN_FALSE;
	}

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLAllocStmt");
		efree(result);
		RETURN_FALSE;
	}

	/* 
	 * Needed to make MS Access happy
	 */
	if (table && table_len && schema && schema_len == 0) {
		schema = NULL;
	}

	rc = SQLColumns(result->stmt, 
			cat, (SQLSMALLINT) cat_len,
			schema, (SQLSMALLINT) schema_len,
			table, (SQLSMALLINT) table_len,
			column, (SQLSMALLINT) column_len);

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLColumns");
		efree(result);
		RETURN_FALSE;
	}

	result->numparams = 0;
	SQLNumResultCols(result->stmt, &(result->numcols));

	if (result->numcols > 0) {
		if (!odbc_bindcols(result)) {
			efree(result);
			RETURN_FALSE;
		}
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;
	RETURN_RES(zend_register_resource(result, le_result));
}
/* }}} */

#if !defined(HAVE_DBMAKER) && !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35) && !defined(HAVE_BIRDSTEP)
/* {{{ proto resource odbc_columnprivileges(resource connection_id, string catalog, string schema, string table, string column)
   Returns a result identifier that can be used to fetch a list of columns and associated privileges for the specified table */
PHP_FUNCTION(odbc_columnprivileges)
{
	zval *pv_conn;
	odbc_result *result = NULL;
	odbc_connection *conn;
	char *cat = NULL, *schema, *table, *column;
	size_t cat_len = 0, schema_len, table_len, column_len;
	RETCODE rc;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs!sss", &pv_conn, &cat, &cat_len, &schema, &schema_len,
		&table, &table_len, &column, &column_len) == FAILURE) {
		return;
	}

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_FALSE;
	}

	result = (odbc_result *)ecalloc(1, sizeof(odbc_result));
	
	rc = SQLAllocStmt(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		efree(result);
		php_error_docref(NULL, E_WARNING, "SQLAllocStmt error 'Invalid Handle'");
		RETURN_FALSE;
	}

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLAllocStmt");
		efree(result);
		RETURN_FALSE;
	}

	rc = SQLColumnPrivileges(result->stmt, 
			cat, SAFE_SQL_NTS(cat),
			schema, SAFE_SQL_NTS(schema),
			table, SAFE_SQL_NTS(table),
			column, SAFE_SQL_NTS(column));

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLColumnPrivileges");
		efree(result);
		RETURN_FALSE;
	}

	result->numparams = 0;
	SQLNumResultCols(result->stmt, &(result->numcols));

	if (result->numcols > 0) {
		if (!odbc_bindcols(result)) {
			efree(result);
			RETURN_FALSE;
		}
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;
	RETURN_RES(zend_register_resource(result, le_result));
}
/* }}} */
#endif /* HAVE_DBMAKER || HAVE_SOLID*/

#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35)
/* {{{ proto resource odbc_foreignkeys(resource connection_id, string pk_qualifier, string pk_owner, string pk_table, string fk_qualifier, string fk_owner, string fk_table)
   Returns a result identifier to either a list of foreign keys in the specified table or a list of foreign keys in other tables that refer to the primary key in the specified table */
PHP_FUNCTION(odbc_foreignkeys)
{
	zval *pv_conn;
	odbc_result *result = NULL;
	odbc_connection *conn;
	char *pcat = NULL, *pschema, *ptable, *fcat, *fschema, *ftable;
	size_t pcat_len = 0, pschema_len, ptable_len, fcat_len, fschema_len, ftable_len;
	RETCODE rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs!sssss", &pv_conn, &pcat, &pcat_len, &pschema, &pschema_len, 
		&ptable, &ptable_len, &fcat, &fcat_len, &fschema, &fschema_len, &ftable, &ftable_len) == FAILURE) {
		return;
	}

#if defined(HAVE_DBMAKER) || defined(HAVE_IBMDB2)
#define EMPTY_TO_NULL(xstr) \
	if ((int)strlen((xstr)) == 0) (xstr) = NULL

		EMPTY_TO_NULL(pcat);
		EMPTY_TO_NULL(pschema);
		EMPTY_TO_NULL(ptable);
		EMPTY_TO_NULL(fcat);
		EMPTY_TO_NULL(fschema);
		EMPTY_TO_NULL(ftable);
#endif

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_FALSE;
	}

	result = (odbc_result *)ecalloc(1, sizeof(odbc_result));
	
	rc = SQLAllocStmt(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		efree(result);
		php_error_docref(NULL, E_WARNING, "SQLAllocStmt error 'Invalid Handle'");
		RETURN_FALSE;
	}

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLAllocStmt");
		efree(result);
		RETURN_FALSE;
	}

	rc = SQLForeignKeys(result->stmt, 
			pcat, SAFE_SQL_NTS(pcat), 
			pschema, SAFE_SQL_NTS(pschema), 
			ptable, SAFE_SQL_NTS(ptable), 
			fcat, SAFE_SQL_NTS(fcat), 
			fschema, SAFE_SQL_NTS(fschema), 
			ftable, SAFE_SQL_NTS(ftable) );

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLForeignKeys");
		efree(result);
		RETURN_FALSE;
	}

	result->numparams = 0;
	SQLNumResultCols(result->stmt, &(result->numcols));

	if (result->numcols > 0) {
		if (!odbc_bindcols(result)) {
			efree(result);
			RETURN_FALSE;
		}
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;
	RETURN_RES(zend_register_resource(result, le_result));
}
/* }}} */
#endif /* HAVE_SOLID */

/* {{{ proto resource odbc_gettypeinfo(resource connection_id [, int data_type])
   Returns a result identifier containing information about data types supported by the data source */
PHP_FUNCTION(odbc_gettypeinfo)
{
	zval *pv_conn;
	zend_long pv_data_type = SQL_ALL_TYPES;
	odbc_result *result = NULL;
	odbc_connection *conn;
	RETCODE rc;
	SQLSMALLINT data_type;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|l", &pv_conn, &pv_data_type) == FAILURE) {
		return;
	}
	
	data_type = (SQLSMALLINT) pv_data_type;

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_FALSE;
	}

	result = (odbc_result *)ecalloc(1, sizeof(odbc_result));
	
	rc = SQLAllocStmt(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		efree(result);
		php_error_docref(NULL, E_WARNING, "SQLAllocStmt error 'Invalid Handle'");
		RETURN_FALSE;
	}

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLAllocStmt");
		efree(result);
		RETURN_FALSE;
	}

	rc = SQLGetTypeInfo(result->stmt, data_type );

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLGetTypeInfo");
		efree(result);
		RETURN_FALSE;
	}

	result->numparams = 0;
	SQLNumResultCols(result->stmt, &(result->numcols));

	if (result->numcols > 0) {
		if (!odbc_bindcols(result)) {
			efree(result);
			RETURN_FALSE;
		}
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;
	RETURN_RES(zend_register_resource(result, le_result));
}
/* }}} */

/* {{{ proto resource odbc_primarykeys(resource connection_id, string qualifier, string owner, string table)
   Returns a result identifier listing the column names that comprise the primary key for a table */
PHP_FUNCTION(odbc_primarykeys)
{
	zval *pv_conn;
	odbc_result   *result = NULL;
	odbc_connection *conn;
	char *cat = NULL, *schema = NULL, *table = NULL;
	size_t cat_len = 0, schema_len, table_len;
	RETCODE rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs!ss", &pv_conn, &cat, &cat_len, &schema, &schema_len, &table, &table_len) == FAILURE) {
		return;
	}

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_FALSE;
	}

	result = (odbc_result *)ecalloc(1, sizeof(odbc_result));
	
	rc = SQLAllocStmt(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		efree(result);
		php_error_docref(NULL, E_WARNING, "SQLAllocStmt error 'Invalid Handle'");
		RETURN_FALSE;
	}

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLAllocStmt");
		efree(result);
		RETURN_FALSE;
	}

	rc = SQLPrimaryKeys(result->stmt, 
			cat, SAFE_SQL_NTS(cat), 
			schema, SAFE_SQL_NTS(schema), 
			table, SAFE_SQL_NTS(table) );

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLPrimaryKeys");
		efree(result);
		RETURN_FALSE;
	}

	result->numparams = 0;
	SQLNumResultCols(result->stmt, &(result->numcols));

	if (result->numcols > 0) {
		if (!odbc_bindcols(result)) {
			efree(result);
			RETURN_FALSE;
		}
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;
	RETURN_RES(zend_register_resource(result, le_result));
}
/* }}} */

#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35) && !defined(HAVE_BIRDSTEP)
/* {{{ proto resource odbc_procedurecolumns(resource connection_id [, string qualifier, string owner, string proc, string column])
   Returns a result identifier containing the list of input and output parameters, as well as the columns that make up the result set for the specified procedures */
PHP_FUNCTION(odbc_procedurecolumns)
{
	zval *pv_conn;
	odbc_result *result = NULL;
	odbc_connection *conn;
	char *cat = NULL, *schema = NULL, *proc = NULL, *col = NULL;
	size_t cat_len = 0, schema_len = 0, proc_len = 0, col_len = 0;
	RETCODE rc;
	
	if (ZEND_NUM_ARGS() != 1 && ZEND_NUM_ARGS() != 5) {
		WRONG_PARAM_COUNT;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|s!sss", &pv_conn, &cat, &cat_len, &schema, &schema_len, 
		&proc, &proc_len, &col, &col_len) == FAILURE) {
		return;
	}

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_FALSE;
	}

	result = (odbc_result *)ecalloc(1, sizeof(odbc_result));
	
	rc = SQLAllocStmt(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		efree(result);
		php_error_docref(NULL, E_WARNING, "SQLAllocStmt error 'Invalid Handle'");
		RETURN_FALSE;
	}

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLAllocStmt");
		efree(result);
		RETURN_FALSE;
	}

	rc = SQLProcedureColumns(result->stmt, 
			cat, SAFE_SQL_NTS(cat), 
			schema, SAFE_SQL_NTS(schema), 
			proc, SAFE_SQL_NTS(proc), 
			col, SAFE_SQL_NTS(col) );

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLProcedureColumns");
		efree(result);
		RETURN_FALSE;
	}

	result->numparams = 0;
	SQLNumResultCols(result->stmt, &(result->numcols));

	if (result->numcols > 0) {
		if (!odbc_bindcols(result)) {
			efree(result);
			RETURN_FALSE;
		}
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;
	RETURN_RES(zend_register_resource(result, le_result));
}
/* }}} */
#endif /* HAVE_SOLID */

#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35)
/* {{{ proto resource odbc_procedures(resource connection_id [, string qualifier, string owner, string name])
   Returns a result identifier containg the list of procedure names in a datasource */
PHP_FUNCTION(odbc_procedures)
{
	zval *pv_conn;
	odbc_result   *result = NULL;
	odbc_connection *conn;
	char *cat = NULL, *schema = NULL, *proc = NULL;
	size_t cat_len = 0, schema_len = 0, proc_len = 0;
	RETCODE rc;

	if (ZEND_NUM_ARGS() != 1 && ZEND_NUM_ARGS() != 4) {
		WRONG_PARAM_COUNT;
	}
	
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|s!ss", &pv_conn, &cat, &cat_len, &schema, &schema_len, &proc, &proc_len) == FAILURE) {
		return;
	}

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_FALSE;
	}

	result = (odbc_result *)ecalloc(1, sizeof(odbc_result));
	
	rc = SQLAllocStmt(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		efree(result);
		php_error_docref(NULL, E_WARNING, "SQLAllocStmt error 'Invalid Handle'");
		RETURN_FALSE;
	}

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLAllocStmt");
		efree(result);
		RETURN_FALSE;
	}

	rc = SQLProcedures(result->stmt, 
			cat, SAFE_SQL_NTS(cat), 
			schema, SAFE_SQL_NTS(schema), 
			proc, SAFE_SQL_NTS(proc) );

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLProcedures");
		efree(result);
		RETURN_FALSE;
	}

	result->numparams = 0;
	SQLNumResultCols(result->stmt, &(result->numcols));

	if (result->numcols > 0) {
		if (!odbc_bindcols(result)) {
			efree(result);
			RETURN_FALSE;
		}
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;
	RETURN_RES(zend_register_resource(result, le_result));
}
/* }}} */
#endif /* HAVE_SOLID */

/* {{{ proto resource odbc_specialcolumns(resource connection_id, int type, string qualifier, string owner, string table, int scope, int nullable)
   Returns a result identifier containing either the optimal set of columns that uniquely identifies a row in the table or columns that are automatically updated when any value in the row is updated by a transaction */
PHP_FUNCTION(odbc_specialcolumns)
{
	zval *pv_conn;
	zend_long vtype, vscope, vnullable;
	odbc_result *result = NULL;
	odbc_connection *conn;
	char *cat = NULL, *schema = NULL, *name = NULL;
	size_t cat_len = 0, schema_len, name_len;
	SQLUSMALLINT type, scope, nullable;
	RETCODE rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rls!ssl", &pv_conn, &vtype, &cat, &cat_len, &schema, &schema_len,
		&name, &name_len, &vscope, &vnullable) == FAILURE) {
		return;
	}
	
	type = (SQLUSMALLINT) vtype;
	scope = (SQLUSMALLINT) vscope;
	nullable = (SQLUSMALLINT) vnullable;

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_FALSE;
	}

	result = (odbc_result *)ecalloc(1, sizeof(odbc_result));
	
	rc = SQLAllocStmt(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		efree(result);
		php_error_docref(NULL, E_WARNING, "SQLAllocStmt error 'Invalid Handle'");
		RETURN_FALSE;
	}

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLAllocStmt");
		efree(result);
		RETURN_FALSE;
	}

	rc = SQLSpecialColumns(result->stmt, 
			type,
			cat, SAFE_SQL_NTS(cat), 
			schema, SAFE_SQL_NTS(schema), 
			name, SAFE_SQL_NTS(name),
			scope,
			nullable);

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLSpecialColumns");
		efree(result);
		RETURN_FALSE;
	}

	result->numparams = 0;
	SQLNumResultCols(result->stmt, &(result->numcols));

	if (result->numcols > 0) {
		if (!odbc_bindcols(result)) {
			efree(result);
			RETURN_FALSE;
		}
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;
	RETURN_RES(zend_register_resource(result, le_result));
}
/* }}} */

/* {{{ proto resource odbc_statistics(resource connection_id, string qualifier, string owner, string name, int unique, int accuracy)
   Returns a result identifier that contains statistics about a single table and the indexes associated with the table */
PHP_FUNCTION(odbc_statistics)
{
	zval *pv_conn;
	zend_long vunique, vreserved;
	odbc_result *result = NULL;
	odbc_connection *conn;
	char *cat = NULL, *schema, *name;
	size_t cat_len = 0, schema_len, name_len;
	SQLUSMALLINT unique, reserved;
	RETCODE rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs!ssll", &pv_conn, &cat, &cat_len, &schema, &schema_len,
		&name, &name_len, &vunique, &vreserved) == FAILURE) {
		return;
	}
	
	unique = (SQLUSMALLINT) vunique;
	reserved = (SQLUSMALLINT) vreserved;

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_FALSE;
	}

	result = (odbc_result *)ecalloc(1, sizeof(odbc_result));
	
	rc = SQLAllocStmt(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		efree(result);
		php_error_docref(NULL, E_WARNING, "SQLAllocStmt error 'Invalid Handle'");
		RETURN_FALSE;
	}

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLAllocStmt");
		efree(result);
		RETURN_FALSE;
	}

	rc = SQLStatistics(result->stmt, 
			cat, SAFE_SQL_NTS(cat),
			schema, SAFE_SQL_NTS(schema), 
			name, SAFE_SQL_NTS(name),
			unique,
			reserved);

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLStatistics");
		efree(result);
		RETURN_FALSE;
	}

	result->numparams = 0;
	SQLNumResultCols(result->stmt, &(result->numcols));

	if (result->numcols > 0) {
		if (!odbc_bindcols(result)) {
			efree(result);
			RETURN_FALSE;
		}
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;
	RETURN_RES(zend_register_resource(result, le_result));
}
/* }}} */

#if !defined(HAVE_DBMAKER) && !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35) && !defined(HAVE_BIRDSTEP)
/* {{{ proto resource odbc_tableprivileges(resource connection_id, string qualifier, string owner, string name)
   Returns a result identifier containing a list of tables and the privileges associated with each table */
PHP_FUNCTION(odbc_tableprivileges)
{
	zval *pv_conn;
	odbc_result   *result = NULL;
	odbc_connection *conn;
	char *cat = NULL, *schema = NULL, *table = NULL;
	size_t cat_len = 0, schema_len, table_len;
	RETCODE rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs!ss", &pv_conn, &cat, &cat_len, &schema, &schema_len, &table, &table_len) == FAILURE) {
		return;
	}

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_FALSE;
	}

	result = (odbc_result *)ecalloc(1, sizeof(odbc_result));
	
	rc = SQLAllocStmt(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		efree(result);
		php_error_docref(NULL, E_WARNING, "SQLAllocStmt error 'Invalid Handle'");
		RETURN_FALSE;
	}

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLAllocStmt");
		efree(result);
		RETURN_FALSE;
	}

	rc = SQLTablePrivileges(result->stmt, 
			cat, SAFE_SQL_NTS(cat), 
			schema, SAFE_SQL_NTS(schema), 
			table, SAFE_SQL_NTS(table));

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLTablePrivileges");
		efree(result);
		RETURN_FALSE;
	}

	result->numparams = 0;
	SQLNumResultCols(result->stmt, &(result->numcols));

	if (result->numcols > 0) {
		if (!odbc_bindcols(result)) {
			efree(result);
			RETURN_FALSE;
		}
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;
	RETURN_RES(zend_register_resource(result, le_result));
}
/* }}} */
#endif /* HAVE_DBMAKER */

#endif /* HAVE_UODBC */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
