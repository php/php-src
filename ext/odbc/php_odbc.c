/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stig Sæther Bakken <ssb@fast.no>                            |
   |          Andreas Karajannis <Andreas.Karajannis@gmd.de>              |
   |          Frank M. Kromann <frank@frontbase.com> Support for DB/2 CLI |
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
#include <winsock.h>

#define ODBC_TYPE "Win32"
#define PHP_ODBC_TYPE ODBC_TYPE

#else
#include "build-defs.h"
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

#define SAFE_SQL_NTS(n) ((SWORD) ((n)?(SQL_NTS):0))

static unsigned char a3_arg3_and_3_force_ref[] = { 3, BYREF_NONE, BYREF_FORCE, BYREF_ALLOW};

/* {{{ odbc_functions[]
 */
function_entry odbc_functions[] = {
	PHP_FE(odbc_autocommit, NULL)
	PHP_FE(odbc_binmode, NULL)
	PHP_FE(odbc_close, NULL)
	PHP_FE(odbc_close_all, NULL)
	PHP_FE(odbc_columns, NULL)
	PHP_FE(odbc_commit, NULL)
	PHP_FE(odbc_connect, NULL)
	PHP_FE(odbc_cursor, NULL)
	PHP_FE(odbc_data_source, NULL)
	PHP_FE(odbc_execute, NULL)
	PHP_FE(odbc_error, NULL)
	PHP_FE(odbc_errormsg, NULL)
	PHP_FE(odbc_exec, NULL)
#ifdef HAVE_DBMAKER
	PHP_FE(odbc_fetch_array, NULL)
	PHP_FE(odbc_fetch_object, NULL)
#endif
	PHP_FE(odbc_fetch_row, NULL)
	PHP_FE(odbc_fetch_into, a3_arg3_and_3_force_ref)
	PHP_FE(odbc_field_len, NULL)
	PHP_FE(odbc_field_scale, NULL)
	PHP_FE(odbc_field_name, NULL)
	PHP_FE(odbc_field_type, NULL)
	PHP_FE(odbc_field_num, NULL)
	PHP_FE(odbc_free_result, NULL)
	PHP_FE(odbc_gettypeinfo, NULL)
	PHP_FE(odbc_longreadlen, NULL)
#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30)
	PHP_FE(odbc_next_result, NULL)
#endif
	PHP_FE(odbc_num_fields, NULL)
	PHP_FE(odbc_num_rows, NULL)
	PHP_FE(odbc_pconnect, NULL)
	PHP_FE(odbc_prepare, NULL)
	PHP_FE(odbc_result, NULL)
	PHP_FE(odbc_result_all, NULL)
	PHP_FE(odbc_rollback, NULL)
	PHP_FE(odbc_setoption, NULL)
	PHP_FE(odbc_specialcolumns, NULL)
	PHP_FE(odbc_statistics, NULL)
	PHP_FE(odbc_tables, NULL)
	PHP_FE(odbc_primarykeys, NULL)
#if !defined(HAVE_DBMAKER) && !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) &&!defined(HAVE_SOLID_35) && !defined(HAVE_BIRDSTEP)    /* not supported now */
	PHP_FE(odbc_columnprivileges, NULL)
	PHP_FE(odbc_tableprivileges, NULL)
#endif
#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35) /* not supported */
	PHP_FE(odbc_foreignkeys, NULL)
	PHP_FE(odbc_procedures, NULL)
#if !defined(HAVE_BIRDSTEP)
	PHP_FE(odbc_procedurecolumns, NULL)
#endif
#endif
	PHP_FALIAS(odbc_do, odbc_exec, NULL)
	PHP_FALIAS(odbc_field_precision, odbc_field_len, NULL)
	{ NULL, NULL, NULL }
};
/* }}} */

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
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef ZTS
int odbc_globals_id;
#else
ZEND_API php_odbc_globals odbc_globals;
#endif

#ifdef COMPILE_DL_ODBC
ZEND_GET_MODULE(odbc)
#endif

/* {{{ _free_odbc_result
 */
static void _free_odbc_result(zend_rsrc_list_entry *rsrc TSRMLS_DC)
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
						(UWORD)SQL_COMMIT);
#endif
			rc = SQLFreeStmt(res->stmt,SQL_DROP);
			/* We don't want the connection to be closed after the last statment has been closed
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
static void _close_odbc_conn(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	odbc_connection *conn = (odbc_connection *)rsrc->ptr;

   	safe_odbc_disconnect(conn->hdbc);
	SQLFreeConnect(conn->hdbc);
	SQLFreeEnv(conn->henv);
	efree(conn);
	ODBCG(num_links)--;
}
/* }}} */

/* {{{ void _close_odbc_pcon
 */
static void _close_odbc_pconn(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	odbc_connection *conn = (odbc_connection *)rsrc->ptr;
	
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
	TSRMLS_FETCH();

	if (type == PHP_INI_DISPLAY_ORIG && ini_entry->modified) {
		value = ini_entry->orig_value;
	} else if (ini_entry->value) {
		value = ini_entry->value;
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
	TSRMLS_FETCH();

	if (type == PHP_INI_DISPLAY_ORIG && ini_entry->modified) {
		value = ini_entry->orig_value;
	} else if (ini_entry->value) {
		value = ini_entry->value;
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
		PUTS("<i>no value</i>");
	}
}
/* }}} */

/* {{{ PHP_INI_DISP(display_binmode)
 */
static PHP_INI_DISP(display_binmode)
{
	char *value;
	TSRMLS_FETCH();
	
	if (type == PHP_INI_DISPLAY_ORIG && ini_entry->modified) {
		value = ini_entry->orig_value;
	} else if (ini_entry->value) {
		value = ini_entry->value;
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
	TSRMLS_FETCH();

	if (type == PHP_INI_DISPLAY_ORIG && ini_entry->modified) {
		value = ini_entry->orig_value;
	} else if (ini_entry->value) {
		value = ini_entry->value;
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

/* {{{ PHP_INI_BEGIN 
 */
PHP_INI_BEGIN()
	STD_PHP_INI_BOOLEAN("odbc.allow_persistent", "1", PHP_INI_SYSTEM, OnUpdateInt,
			allow_persistent, php_odbc_globals, odbc_globals)
	STD_PHP_INI_ENTRY_EX("odbc.max_persistent",  "-1", PHP_INI_SYSTEM, OnUpdateInt,
			max_persistent, php_odbc_globals, odbc_globals, display_link_nums)
	STD_PHP_INI_ENTRY_EX("odbc.max_links", "-1", PHP_INI_SYSTEM, OnUpdateInt,
			max_links, php_odbc_globals, odbc_globals, display_link_nums)
	STD_PHP_INI_ENTRY("odbc.default_db", NULL, PHP_INI_ALL, OnUpdateString,
			defDB, php_odbc_globals, odbc_globals)
	STD_PHP_INI_ENTRY("odbc.default_user", NULL, PHP_INI_ALL, OnUpdateString,
			defUser, php_odbc_globals, odbc_globals)
	STD_PHP_INI_ENTRY_EX("odbc.default_pw", NULL, PHP_INI_ALL, OnUpdateString,
			defPW, php_odbc_globals, odbc_globals, display_defPW)
	STD_PHP_INI_ENTRY_EX("odbc.defaultlrl", "4096", PHP_INI_ALL, OnUpdateInt,
			defaultlrl, php_odbc_globals, odbc_globals, display_lrl)
	STD_PHP_INI_ENTRY_EX("odbc.defaultbinmode", "1", PHP_INI_ALL, OnUpdateInt,
			defaultbinmode, php_odbc_globals, odbc_globals, display_binmode)
	STD_PHP_INI_BOOLEAN("odbc.check_persistent", "1", PHP_INI_SYSTEM, OnUpdateInt,
			check_persistent, php_odbc_globals, odbc_globals)
PHP_INI_END()
/* }}} */

#ifdef ZTS
static void php_odbc_init_globals(php_odbc_globals *odbc_globals_p TSRMLS_DC)
{
	ODBCG(num_persistent) = 0;
}
#endif

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(odbc)
{
#ifdef SQLANY_BUG
	ODBC_SQL_CONN_T foobar;
	RETCODE rc;
#endif

#ifdef ZTS
	ts_allocate_id(&odbc_globals_id, sizeof(php_odbc_globals), php_odbc_init_globals, NULL);
#else
	ODBCG(num_persistent) = 0;
#endif

	REGISTER_INI_ENTRIES();
	le_result = zend_register_list_destructors_ex(_free_odbc_result, NULL, "odbc result", module_number);
	le_conn = zend_register_list_destructors_ex(_close_odbc_conn, NULL, "odbc link", module_number);
	le_pconn = zend_register_list_destructors_ex(NULL, _close_odbc_pconn, "odbc link persistent", module_number);
	Z_TYPE(odbc_module_entry) = type;
	
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
	sprintf(buf, "%ld", ODBCG(num_persistent));
	php_info_print_table_row(2, "Active Persistent Links", buf);
	sprintf(buf, "%ld", ODBCG(num_links));
	php_info_print_table_row(2, "Active Links", buf);
	php_info_print_table_row(2, "ODBC library", PHP_ODBC_TYPE);
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
	char	state[6];
	SDWORD	error;        /* Not used */
	char	errormsg[SQL_MAX_MESSAGE_LENGTH];
	SWORD	errormsgsize; /* Not used */
	RETCODE rc;
	ODBC_SQL_ENV_T henv;
	ODBC_SQL_CONN_T conn;
	TSRMLS_FETCH();

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
	if (conn_resource) {
		memcpy(conn_resource->laststate, state, sizeof(state));
		memcpy(conn_resource->lasterrormsg, errormsg, sizeof(errormsg));
	}
	memcpy(ODBCG(laststate), state, sizeof(state));
	memcpy(ODBCG(lasterrormsg), errormsg, sizeof(errormsg));
	if (func) {
		php_error(E_WARNING, "SQL error: %s, SQL state %s in %s", errormsg, state, func);
	} else {
		php_error(E_WARNING, "SQL error: %s, SQL state %s", errormsg, state);
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
	pval **pv_res, **pv_flag;

	if (zend_get_parameters_ex(2, &pv_res, &pv_flag) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long_ex(pv_flag);
	
	if (Z_LVAL_PP(pv_res)) {
		ZEND_FETCH_RESOURCE(result, odbc_result *, pv_res, -1, "ODBC result", le_result);
        if (mode)
            result->longreadlen = Z_LVAL_PP(pv_flag);	
        else
            result->binmode = Z_LVAL_PP(pv_flag);
	} else {
        if (mode)
            ODBCG(defaultlrl) = Z_LVAL_PP(pv_flag);
        else
            ODBCG(defaultbinmode) = Z_LVAL_PP(pv_flag);
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ odbc_bindcols */
int odbc_bindcols(odbc_result *result TSRMLS_DC)
{
	RETCODE rc;
	int i;
	SWORD       colnamelen; /* Not used */
	SDWORD      displaysize;

	result->values = (odbc_result_value *) emalloc(sizeof(odbc_result_value)*result->numcols);

	if (result->values == NULL) {
		php_error(E_WARNING, "Out of memory");
		SQLFreeStmt(result->stmt, SQL_DROP);
		return 0;
	}

	result->longreadlen = ODBCG(defaultlrl);
	result->binmode = ODBCG(defaultbinmode);

	for(i = 0; i < result->numcols; i++) {
		rc = SQLColAttributes(result->stmt, (UWORD)(i+1), SQL_COLUMN_NAME, 
				result->values[i].name, sizeof(result->values[i].name), &colnamelen, 0);
		rc = SQLColAttributes(result->stmt, (UWORD)(i+1), SQL_COLUMN_TYPE, 
				NULL, 0, NULL, &result->values[i].coltype);
		
		/* Don't bind LONG / BINARY columns, so that fetch behaviour can
		 * be controlled by odbc_binmode() / odbc_longreadlen()
		 */
		
		switch(result->values[i].coltype) {
			case SQL_BINARY:
			case SQL_VARBINARY:
			case SQL_LONGVARBINARY:
			case SQL_LONGVARCHAR:
				result->values[i].value = NULL;
				break;
				
#ifdef HAVE_ADABAS
			case SQL_TIMESTAMP:
				result->values[i].value = (char *)emalloc(27);
				SQLBindCol(result->stmt, (UWORD)(i+1), SQL_C_CHAR, result->values[i].value,
							27, &result->values[i].vallen);
				break;
#endif /* HAVE_ADABAS */
			default:
				rc = SQLColAttributes(result->stmt, (UWORD)(i+1), SQL_COLUMN_DISPLAY_SIZE,
									NULL, 0, NULL, &displaysize);
				displaysize = displaysize <= result->longreadlen ? displaysize : 
								result->longreadlen;
				result->values[i].value = (char *)emalloc(displaysize + 1);
				rc = SQLBindCol(result->stmt, (UWORD)(i+1), SQL_C_CHAR, result->values[i].value,
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
	pval **pv_conn;
	
 	if (zend_get_parameters_ex(1, &pv_conn) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE2(conn, odbc_connection *, pv_conn, -1, "ODBC-Link", le_conn, le_pconn);
	
	rc = SQLTransact(conn->henv, conn->hdbc, (UWORD)((type)?SQL_COMMIT:SQL_ROLLBACK));
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLTransact");
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ _close_pconn_with_id */
static int _close_pconn_with_id(list_entry *le, int *id TSRMLS_DC)
{
	if(Z_TYPE_P(le) == le_pconn && (((odbc_connection *)(le->ptr))->id == *id)){
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
#if defined(HAVE_SOLID) || defined(HAVE_SOLID_30) || defined(HAVE_OPENLINK)
	/* this seems to be necessary for Solid2.3 ( tested by 
	 * tammy@synchronis.com) and Solid 3.0 (tested by eric@terra.telemediair.nl)
	 * Solid does not seem to declare a SQLINTEGER, but it does declare a
	 * SQL_INTEGER which does not work (despite being the same type as a SDWORD.
	 * Solid 3.5 does not have this issue.
	 */
	SDWORD len;
#else
	SQLINTEGER len;
#endif
	pval **pv_res, **pv_num;

	if (zend_get_parameters_ex(2, &pv_res, &pv_num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(pv_num);

	ZEND_FETCH_RESOURCE(result, odbc_result *, pv_res, -1, "ODBC result", le_result);

	if (result->numcols == 0) {
		php_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}

	if (Z_LVAL_PP(pv_num) > result->numcols) {
		php_error(E_WARNING, "Field index larger than number of fields");
		RETURN_FALSE;
	}

	if (Z_LVAL_PP(pv_num) < 1) {
		php_error(E_WARNING, "Field numbering starts at 1");
		RETURN_FALSE;
	}

	SQLColAttributes(result->stmt, (UWORD)Z_LVAL_PP(pv_num), 
					(SQLUSMALLINT) (type?SQL_COLUMN_SCALE:SQL_COLUMN_PRECISION),
					NULL, 0, NULL, &len);

	RETURN_LONG(len);
}
/* }}} */

/* Main User Functions */

/* {{{ proto void odbc_close_all(void)
   Close all ODBC connections */
PHP_FUNCTION(odbc_close_all)
{
	void *ptr;
	int type;
	int i;
	int nument;

	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	nument = zend_hash_next_free_element(&EG(regular_list));
	
	/* Loop through list and close all statements */
	for(i = 1; i < nument; i++) {
		ptr = zend_list_find(i, &type);
		if (ptr && (type == le_result)){
			zend_list_delete(i);
		}
	}

	/* Second loop through list, now close all connections */
	nument = zend_hash_next_free_element(&EG(regular_list));
	
	for(i = 1; i < nument; i++) {
		ptr = zend_list_find(i, &type);
		if (ptr){
			if(type == le_conn){
				zend_list_delete(i);
			}else if(type == le_pconn){
				zend_list_delete(i);
				/* Delete the persistent connection */
				zend_hash_apply_with_argument(&EG(persistent_list), 
					(apply_func_arg_t) _close_pconn_with_id, (void *) &i TSRMLS_CC);
			}
		}
	}
}
/* }}} */

/* {{{ proto int odbc_binmode(int result_id, int mode)
   Handle binary column data */
PHP_FUNCTION(odbc_binmode)
{
	php_odbc_fetch_attribs(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int odbc_longreadlen(int result_id, int length)
   Handle LONG columns */
PHP_FUNCTION(odbc_longreadlen)
{
	php_odbc_fetch_attribs(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto int odbc_prepare(int connection_id, string query)
   Prepares a statement for execution */
PHP_FUNCTION(odbc_prepare)
{
	pval **pv_conn, **pv_query;
	char *query;
	odbc_result *result = NULL;
	odbc_connection *conn;
	RETCODE rc;
#ifdef HAVE_SQL_EXTENDED_FETCH
	UDWORD      scrollopts;
#endif

	if (zend_get_parameters_ex(2, &pv_conn, &pv_query) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE2(conn, odbc_connection *, pv_conn, -1, "ODBC-Link", le_conn, le_pconn);

	convert_to_string_ex(pv_query);
	query = Z_STRVAL_PP(pv_query);

	result = (odbc_result *)emalloc(sizeof(odbc_result));
	if (result == NULL) {
		php_error(E_WARNING, "Out of memory");
		RETURN_FALSE;
	}
	
	result->numparams = 0;
	
	rc = SQLAllocStmt(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		efree(result);
		php_error_docref("odbc-prepare" TSRMLS_CC, E_WARNING, "%s(): SQLAllocStmt error 'Invalid Handle'");
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
			if (SQLSetStmtOption(result->stmt, SQL_CURSOR_TYPE, SQL_CURSOR_DYNAMIC)
				== SQL_ERROR) {
				odbc_sql_error(conn, result->stmt, " SQLSetStmtOption");
				SQLFreeStmt(result->stmt, SQL_DROP);
				efree(result);
				RETURN_FALSE;
			}
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
		if (!odbc_bindcols(result TSRMLS_CC)) {
			efree(result);
			RETURN_FALSE;
		}
	} else {
		result->values = NULL;
	}
	result->id = zend_list_insert(result, le_result);	
	zend_list_addref(conn->id);
	result->conn_ptr = conn;
	result->fetched = 0;
	RETURN_RESOURCE(result->id);
}
/* }}} */

/*
 * Execute prepared SQL statement. Supports only input parameters.
 */

/* {{{ proto int odbc_execute(int result_id [, array parameters_array])
   Execute a prepared statement */
PHP_FUNCTION(odbc_execute)
{ 
	pval **pv_res, **pv_param_arr, **tmp;
	typedef struct params_t {
		SDWORD vallen;
		int fp;
	} params_t;
	params_t *params = NULL;
	char *filename;
	unsigned char otype;
   	SWORD sqltype, ctype, scale;
	SWORD nullable;
	UDWORD precision;
   	odbc_result   *result;
	int numArgs, i, ne;
	RETCODE rc;
	
	numArgs = ZEND_NUM_ARGS();
	switch(numArgs) {
		case 1:
			if (zend_get_parameters_ex(1, &pv_res) == FAILURE)
				WRONG_PARAM_COUNT;
			break;
		case 2:
			if (zend_get_parameters_ex(2, &pv_res, &pv_param_arr) == FAILURE)
				WRONG_PARAM_COUNT;
			if (Z_TYPE_PP(pv_param_arr) != IS_ARRAY) {
				php_error_docref("odbc-execute" TSRMLS_CC, E_WARNING, "%s(): No array passed");
				return;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(result, odbc_result *, pv_res, -1, "ODBC result", le_result);
	
	/* XXX check for already bound parameters*/
	if (result->numparams > 0 && numArgs == 1) {
		php_error(E_WARNING, "No parameters to SQL statement given");
		RETURN_FALSE;
	}

	if (result->numparams > 0) {
		if ((ne = zend_hash_num_elements(Z_ARRVAL_PP(pv_param_arr))) < result->numparams) {
			php_error(E_WARNING,"Not enough parameters (%d should be %d) given", ne, result->numparams);
			RETURN_FALSE;
		}

		zend_hash_internal_pointer_reset(Z_ARRVAL_PP(pv_param_arr));
		params = (params_t *)emalloc(sizeof(params_t) * result->numparams);
		
		for(i = 1; i <= result->numparams; i++) {
			if (zend_hash_get_current_data(Z_ARRVAL_PP(pv_param_arr), (void **) &tmp) == FAILURE) {
				php_error(E_WARNING,"Error getting parameter");
				SQLFreeStmt(result->stmt,SQL_RESET_PARAMS);
				efree(params);
				RETURN_FALSE;
			}

			otype = (*tmp)->type;
			convert_to_string(*tmp);
			if (Z_TYPE_PP(tmp) != IS_STRING) {
				php_error(E_WARNING,"Error converting parameter");
				SQLFreeStmt(result->stmt, SQL_RESET_PARAMS);
				efree(params);
				RETURN_FALSE;
			}
			
			SQLDescribeParam(result->stmt, (UWORD)i, &sqltype, &precision,
							 &scale, &nullable);
			params[i-1].vallen = Z_STRLEN_PP(tmp);
			params[i-1].fp = -1;

			if (IS_SQL_BINARY(sqltype))
				ctype = SQL_C_BINARY;
			else
				ctype = SQL_C_CHAR;

			if (Z_STRLEN_PP(tmp) > 2 &&
				Z_STRVAL_PP(tmp)[0] == '\'' &&
				Z_STRVAL_PP(tmp)[Z_STRLEN_PP(tmp) - 1] == '\'') {
				filename = estrndup(&Z_STRVAL_PP(tmp)[1], Z_STRLEN_PP(tmp) - 2);
				filename[strlen(filename)] = '\0';

				/* Check for safe mode. */
				if (PG(safe_mode) && (!php_checkuid(filename, NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
						RETURN_FALSE;
					}

				/* Check the basedir */
				if (php_check_open_basedir(filename TSRMLS_CC)) {
					RETURN_FALSE;
				}

				if ((params[i-1].fp = open(filename,O_RDONLY)) == -1) {
					php_error(E_WARNING,"Can't open file %s", filename);
					SQLFreeStmt(result->stmt, SQL_RESET_PARAMS);
					for(i = 0; i < result->numparams; i++) {
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

				rc = SQLBindParameter(result->stmt, (UWORD)i, SQL_PARAM_INPUT,
									  ctype, sqltype, precision, scale,
									  (void *)params[i-1].fp, 0,
									  &params[i-1].vallen);
			} else {
#ifdef HAVE_DBMAKER
				precision = params[i-1].vallen;
#endif
				if (otype == IS_NULL) {
					params[i-1].vallen = SQL_NULL_DATA;
				}

				rc = SQLBindParameter(result->stmt, (UWORD)i, SQL_PARAM_INPUT,
									  ctype, sqltype, precision, scale,
									  Z_STRVAL_PP(tmp), 0,
									  &params[i-1].vallen);
			}
			zend_hash_move_forward(Z_ARRVAL_PP(pv_param_arr));
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
		while(rc == SQL_NEED_DATA) {
			rc = SQLParamData(result->stmt, (void*)&fp);
			if (rc == SQL_NEED_DATA) {
				while((nbytes = read(fp, &buf, 4096)) > 0)
					SQLPutData(result->stmt, (void*)&buf, nbytes);
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
			if (params[i].fp != -1)
				close(params[i].fp);
		}
		efree(params);
	}

	if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO || rc == SQL_NO_DATA_FOUND) {
		RETVAL_TRUE;
	}

	if (result->numcols == 0) {
		SQLNumResultCols(result->stmt, &(result->numcols));

		if (result->numcols > 0) {
			if (!odbc_bindcols(result TSRMLS_CC)) {
				efree(result);
				RETVAL_FALSE;
			}
		} else {
			result->values = NULL;
		}
	}
}
/* }}} */

/* {{{ proto string odbc_cursor(int result_id)
   Get cursor name */
PHP_FUNCTION(odbc_cursor)
{
	pval **pv_res;
	SWORD len, max_len;
	char *cursorname;
   	odbc_result *result;
	RETCODE rc;

	if (zend_get_parameters_ex(1, &pv_res) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(result, odbc_result *, pv_res, -1, "ODBC result", le_result);

	rc = SQLGetInfo(result->conn_ptr->hdbc,SQL_MAX_CURSOR_NAME_LEN,
					(void *)&max_len,sizeof(max_len),&len);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		RETURN_FALSE;
	}
	
	if (max_len > 0) {
		cursorname = emalloc(max_len + 1);
		if (cursorname == NULL) {
			php_error(E_WARNING,"Out of memory");
			RETURN_FALSE;
		}
		rc = SQLGetCursorName(result->stmt,cursorname,(SWORD)max_len,&len);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			char    state[6];     /* Not used */
	 		SDWORD  error;        /* Not used */
			char    errormsg[255];
			SWORD   errormsgsize; /* Not used */

			SQLError( result->conn_ptr->henv, result->conn_ptr->hdbc,
						result->stmt, state, &error, errormsg,
						sizeof(errormsg)-1, &errormsgsize);
			if (!strncmp(state,"S1015",5)) {
				sprintf(cursorname,"php_curs_%d", (int)result->stmt);
				if (SQLSetCursorName(result->stmt,cursorname,SQL_NTS) != SQL_SUCCESS) {
					odbc_sql_error(result->conn_ptr, result->stmt, "SQLSetCursorName");
					RETVAL_FALSE;
				} else {
					RETVAL_STRING(cursorname,1);
				}
			} else {
				php_error(E_WARNING, "SQL error: %s, SQL state %s", errormsg, state);
				RETVAL_FALSE;
			}
		} else {
			RETVAL_STRING(cursorname,1);
		}
		efree(cursorname);
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto array odbc_data_source(int connection_id, int fetch_type)
   Return information about the currently connected data source */
PHP_FUNCTION(odbc_data_source)
{
	zval **zv_fetch_type, **zv_conn;
	RETCODE rc = 0; /* assume all is good */
	odbc_connection *conn;
	int num_args = ZEND_NUM_ARGS();
	UCHAR server_name[100],
		  desc[200];
	SQLSMALLINT len1=0, len2=0, fetch_type;

	if (num_args != 2) {
		WRONG_PARAM_COUNT;
	}

	if (zend_get_parameters_ex(2, &zv_conn, &zv_fetch_type) == FAILURE) {
		php_error(E_WARNING, "%s(): Unable to get parameters", get_active_function_name(TSRMLS_C));
	}

	convert_to_long_ex(zv_fetch_type);
	fetch_type = Z_LVAL_PP(zv_fetch_type);

	if (!(fetch_type == SQL_FETCH_FIRST ||
	      fetch_type == SQL_FETCH_NEXT)) {
		php_error_docref("odbc-data-source" TSRMLS_CC, E_WARNING, "%s(): Invalid fetch type (%d)", fetch_type);
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE2(conn, odbc_connection *, zv_conn, -1, "ODBC-Link", le_conn, le_pconn);

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
		odbc_sql_error(conn, NULL, "SQLDataSources");
		RETURN_FALSE;
	}

	if (len1 == 0 || len2 == 0) {
		/* we have a non-valid entry... so stop the looping */
		RETURN_FALSE;
	}

	array_init(return_value);

	add_assoc_stringl_ex(return_value, "server", sizeof("server"), server_name, len1, 1);
	add_assoc_stringl_ex(return_value, "description", sizeof("description"), desc, len2, 1);

}
/* }}} */

/* {{{ proto int odbc_exec(int connection_id, string query [, int flags])
   Prepare and execute an SQL statement */
/* XXX Use flags */
PHP_FUNCTION(odbc_exec)
{
	pval **pv_conn, **pv_query, **pv_flags;
	int numArgs;
	char *query;
	odbc_result *result = NULL;
	odbc_connection *conn;
	RETCODE rc;
#ifdef HAVE_SQL_EXTENDED_FETCH
	UDWORD      scrollopts;
#endif

	numArgs = ZEND_NUM_ARGS();
	if (numArgs > 2) {
		if (zend_get_parameters_ex(3, &pv_conn, &pv_query, &pv_flags) == FAILURE)
			WRONG_PARAM_COUNT;
		convert_to_long_ex(pv_flags);
	} else {
		if (zend_get_parameters_ex(2, &pv_conn, &pv_query) == FAILURE)
			WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE2(conn, odbc_connection *, pv_conn, -1, "ODBC-Link", le_conn, le_pconn);
	
	convert_to_string_ex(pv_query);
	query = Z_STRVAL_PP(pv_query);
	
	result = (odbc_result *)emalloc(sizeof(odbc_result));
	if (result == NULL) {
		php_error(E_WARNING, "Out of memory");
		RETURN_FALSE;
	}

	rc = SQLAllocStmt(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		php_error_docref("odbc-exec" TSRMLS_CC, E_WARNING, "%s(): SQLAllocStmt error 'Invalid Handle'");
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
			if (SQLSetStmtOption(result->stmt, SQL_CURSOR_TYPE, SQL_CURSOR_DYNAMIC)
				== SQL_ERROR) {
				odbc_sql_error(conn, result->stmt, " SQLSetStmtOption");
				SQLFreeStmt(result->stmt, SQL_DROP);
				efree(result);
				RETURN_FALSE;
			}
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
		if (!odbc_bindcols(result TSRMLS_CC)) {
			efree(result);
			RETURN_FALSE;
		}
	} else {
		result->values = NULL;
	}
	result->id = zend_list_insert(result, le_result);
	zend_list_addref(conn->id);
	result->conn_ptr = conn;
	result->fetched = 0;
	
	RETURN_RESOURCE(result->id);
}
/* }}} */

#ifdef HAVE_DBMAKER
#define ODBC_NUM  1
#define ODBC_OBJECT  2

/* {{{ php_odbc_fetch_hash */
static void php_odbc_fetch_hash(INTERNAL_FUNCTION_PARAMETERS, int result_type)
{
	int i;
	odbc_result *result;
	RETCODE rc;
	SWORD sql_c_type;
	char *buf = NULL;
#ifdef HAVE_SQL_EXTENDED_FETCH
	UDWORD crow;
	UWORD  RowStatus[1];
	SDWORD rownum = -1;
	pval **pv_res, **pv_row, *tmp;

	switch(ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &pv_res) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &pv_res, &pv_row) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_long_ex(pv_row);
			rownum = Z_LVAL_PP(pv_row);
			break;
		default:
			WRONG_PARAM_COUNT;
	}

#else
	pval **pv_res, *tmp;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &pv_res) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
#endif

	ZEND_FETCH_RESOURCE(result, odbc_result *, pv_res, -1, "ODBC result", le_result);

	if (result->numcols == 0) {
		php_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}

	if (array_init(return_value)==FAILURE) {
		RETURN_FALSE;
	}

#ifdef HAVE_SQL_EXTENDED_FETCH
	if (result->fetch_abs) {
		if (rownum > 0)
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_ABSOLUTE,rownum,&crow,RowStatus);
		else
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
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
		ALLOC_ZVAL(tmp);
		tmp->refcount = 1;
		Z_TYPE_P(tmp) = IS_STRING;
		Z_STRLEN_P(tmp) = 0;
		sql_c_type = SQL_C_CHAR;

		switch(result->values[i].coltype) {
			case SQL_BINARY:
			case SQL_VARBINARY:
			case SQL_LONGVARBINARY:
				if (result->binmode <= 0) {
					Z_STRVAL_P(tmp) = empty_string;
					break;
				}
				if (result->binmode == 1) sql_c_type = SQL_C_BINARY;
			case SQL_LONGVARCHAR:
				if (IS_SQL_LONG(result->values[i].coltype) && result->longreadlen <= 0) {
					Z_STRVAL_P(tmp) = empty_string;
					break;
				}
				if (buf == NULL) buf = emalloc(result->longreadlen + 1);
				rc = SQLGetData(result->stmt, (UWORD)(i + 1), sql_c_type,
						buf, result->longreadlen + 1, &result->values[i].vallen);

				if (rc == SQL_ERROR) {
					odbc_sql_error(result->conn_ptr, result->stmt, "SQLGetData");
					efree(buf);
					RETURN_FALSE;
				}

				if (rc == SQL_SUCCESS_WITH_INFO) {
					Z_STRLEN_P(tmp) = result->longreadlen;
				} else if (result->values[i].vallen == SQL_NULL_DATA) {
					Z_STRVAL_P(tmp) = empty_string;
					break;
				} else {
					Z_STRLEN_P(tmp) = result->values[i].vallen;
				}
				Z_STRVAL_P(tmp) = estrndup(buf, Z_STRLEN_P(tmp));
				break;

			default:
				if (result->values[i].vallen == SQL_NULL_DATA) {
					Z_STRVAL_P(tmp) = empty_string;
					break;
				}
				Z_STRLEN_P(tmp) = result->values[i].vallen;
				Z_STRVAL_P(tmp) = estrndup(result->values[i].value,Z_STRLEN_P(tmp));
				break;
		}

		if (result_type & ODBC_NUM) {
			zend_hash_index_update(Z_ARRVAL_P(return_value), i, &tmp, sizeof(pval *), NULL);
		} else {
			zend_hash_update(Z_ARRVAL_P(return_value), result->values[i].name, 
					strlen(result->values[i].name)+1, &tmp, sizeof(pval *), NULL);
		}
	}
	if (buf) efree(buf);
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

/* {{{ proto int odbc_fetch_into(int result_id, array result_array, [, int rownumber])
   Fetch one result row into an array */ 
PHP_FUNCTION(odbc_fetch_into)
{
	int numArgs, i;
	odbc_result *result;
	RETCODE rc;
	SWORD sql_c_type;
	char *buf = NULL;
	pval **pv_res, **pv_res_arr, *tmp;
#ifdef HAVE_SQL_EXTENDED_FETCH
	pval **pv_row;
	UDWORD crow;
	UWORD  RowStatus[1];
	SDWORD rownum = -1;
#endif /* HAVE_SQL_EXTENDED_FETCH */
	
	numArgs = ZEND_NUM_ARGS();

	switch(numArgs) {
		case 2:
			if (zend_get_parameters_ex(2, &pv_res, &pv_res_arr) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
#ifdef HAVE_SQL_EXTENDED_FETCH
		case 3:
			if (zend_get_parameters_ex(3, &pv_res, &pv_res_arr, &pv_row) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			SEPARATE_ZVAL(pv_row);
			convert_to_long_ex(pv_row);
			rownum = Z_LVAL_PP(pv_row);
			break;
#endif /* HAVE_SQL_EXTENDED_FETCH */
		default:
			WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(result, odbc_result *, pv_res, -1, "ODBC result", le_result);
	
	if (result->numcols == 0) {
		php_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
	
	if (Z_TYPE_PP(pv_res_arr) != IS_ARRAY) {
		if (array_init(*pv_res_arr) == FAILURE) {
			php_error(E_WARNING, "Can't convert to type Array");
			RETURN_FALSE;
		}
	}

#ifdef HAVE_SQL_EXTENDED_FETCH
	if (result->fetch_abs) {
		if (rownum > 0)
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_ABSOLUTE,rownum,&crow,RowStatus);
		else
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
	} else
#endif
		rc = SQLFetch(result->stmt);

	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
		RETURN_FALSE;

#ifdef HAVE_SQL_EXTENDED_FETCH
	if (rownum > 0 && result->fetch_abs)
		result->fetched = rownum;
	else
#endif
		result->fetched++;

	for(i = 0; i < result->numcols; i++) {
		ALLOC_ZVAL(tmp);
		tmp->refcount = 1;
		Z_TYPE_P(tmp) = IS_STRING;
		Z_STRLEN_P(tmp) = 0;
		sql_c_type = SQL_C_CHAR;

		switch(result->values[i].coltype) {
			case SQL_BINARY:
			case SQL_VARBINARY:
			case SQL_LONGVARBINARY:
				if (result->binmode <= 0) {
					Z_STRVAL_P(tmp) = empty_string;
					break;
				}
				if (result->binmode == 1) sql_c_type = SQL_C_BINARY; 
			case SQL_LONGVARCHAR:
				if (IS_SQL_LONG(result->values[i].coltype) && result->longreadlen <= 0) {
					Z_STRVAL_P(tmp) = empty_string;
					break;
				}

				if (buf == NULL) buf = emalloc(result->longreadlen + 1);
				rc = SQLGetData(result->stmt, (UWORD)(i + 1),sql_c_type,
								buf, result->longreadlen + 1, &result->values[i].vallen);

					if (rc == SQL_ERROR) {
					odbc_sql_error(result->conn_ptr, result->stmt, "SQLGetData");
					efree(buf);
					RETURN_FALSE;
				}
				if (rc == SQL_SUCCESS_WITH_INFO) {
					Z_STRLEN_P(tmp) = result->longreadlen;
				} else if (result->values[i].vallen == SQL_NULL_DATA) {
					Z_STRVAL_P(tmp) = empty_string;
					break;
				} else {
					Z_STRLEN_P(tmp) = result->values[i].vallen;
				}
				Z_STRVAL_P(tmp) = estrndup(buf, Z_STRLEN_P(tmp));
				break;

			default:
				if (result->values[i].vallen == SQL_NULL_DATA) {
					Z_STRVAL_P(tmp) = empty_string;
					break;
				}
				Z_STRLEN_P(tmp) = result->values[i].vallen;
				Z_STRVAL_P(tmp) = estrndup(result->values[i].value,Z_STRLEN_P(tmp));
				break;
		}
		zend_hash_index_update(Z_ARRVAL_PP(pv_res_arr), i, &tmp, sizeof(pval *), NULL);
	}
	if (buf) efree(buf);
	RETURN_LONG(result->numcols);	
}
/* }}} */

/* {{{ solid_fetch_prev */
#if defined(HAVE_SOLID) || defined(HAVE_SOLID_30) || defined(HAVE_SOLID_35)
PHP_FUNCTION(solid_fetch_prev)
{
	odbc_result *result;
	RETCODE rc;
	pval **pv_res;
	
	if (zend_get_parameters_ex(1, &pv_res) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(result, odbc_result *, pv_res, -1, "ODBC result", le_result);
	if (result->numcols == 0) {
		php_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
	rc = SQLFetchPrev(result->stmt);

	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		RETURN_FALSE;
	}

	if (result->fetched > 1) result->fetched--;

	RETURN_TRUE;
}
#endif
/* }}} */

/* {{{ proto int odbc_fetch_row(int result_id [, int row_number])
   Fetch a row */
PHP_FUNCTION(odbc_fetch_row)
{
	int numArgs;
	SDWORD rownum = 1;
	odbc_result *result;
	RETCODE rc;
	pval **pv_res, **pv_row;
#ifdef HAVE_SQL_EXTENDED_FETCH
	UDWORD crow;
	UWORD RowStatus[1];
#endif

	numArgs = ZEND_NUM_ARGS();
	switch (numArgs) {
		case 1:
			if (zend_get_parameters_ex(1, &pv_res) == FAILURE)
				WRONG_PARAM_COUNT;
			break;
		case 2:
			if (zend_get_parameters_ex(2, &pv_res, &pv_row) == FAILURE)
				WRONG_PARAM_COUNT;
			convert_to_long_ex(pv_row);
			rownum = Z_LVAL_PP(pv_row);
			break;
		default:
			WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(result, odbc_result *, pv_res, -1, "ODBC result", le_result);
	
	if (result->numcols == 0) {
		php_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}

#ifdef HAVE_SQL_EXTENDED_FETCH
	if (result->fetch_abs) {
		if (numArgs > 1)
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_ABSOLUTE,rownum,&crow,RowStatus);
		else
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
	} else
#endif
		rc = SQLFetch(result->stmt);

	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		RETURN_FALSE;
	}
	
	if (numArgs > 1) {
		result->fetched = rownum;
	} else {
		result->fetched++;
	}
	
	RETURN_TRUE;
}	
/* }}} */

/* {{{ proto string odbc_result(int result_id, mixed field)
   Get result data */ 
PHP_FUNCTION(odbc_result)
{
	char *field;
	int field_ind;
	SWORD sql_c_type = SQL_C_CHAR;
	odbc_result *result;
	int i = 0;
	RETCODE rc;
	SDWORD	fieldsize;
	pval **pv_res, **pv_field;
#ifdef HAVE_SQL_EXTENDED_FETCH
	UDWORD crow;
	UWORD RowStatus[1];
#endif

	field_ind = -1;
	field = NULL;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2 , &pv_res, &pv_field) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	if (Z_TYPE_PP(pv_field) == IS_STRING) {
		field = Z_STRVAL_PP(pv_field);
	} else {
		convert_to_long_ex(pv_field);
		field_ind = Z_LVAL_PP(pv_field) - 1;
	}
	
	ZEND_FETCH_RESOURCE(result, odbc_result *, pv_res, -1, "ODBC result", le_result);
	
	if ((result->numcols == 0)) {
		php_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
	
	/* get field index if the field parameter was a string */
	if (field != NULL) {
		if (result->values == NULL) {
			php_error(E_WARNING, "Result set contains no data");
			RETURN_FALSE;
		}

		for(i = 0; i < result->numcols; i++) {
			if (!strcasecmp(result->values[i].name, field)) {
				field_ind = i;
				break;
			}
		}

		if (field_ind < 0) {
			php_error(E_WARNING, "Field %s not found", field);
			RETURN_FALSE;
		}
	} else {
		/* check for limits of field_ind if the field parameter was an int */
		if (field_ind >= result->numcols || field_ind < 0) {
			php_error(E_WARNING, "Field index is larger than the number of fields");
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

		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
			RETURN_FALSE;
		
		result->fetched++;
	}

	switch(result->values[field_ind].coltype) {
		case SQL_BINARY:
		case SQL_VARBINARY:
		case SQL_LONGVARBINARY:
			if (result->binmode <= 1) sql_c_type = SQL_C_BINARY;
			if (result->binmode <= 0) break; 
		case SQL_LONGVARCHAR:
			if (IS_SQL_LONG(result->values[field_ind].coltype)) {
			   if (result->longreadlen <= 0) 
				   break;
			   else 
				   fieldsize = result->longreadlen;
			} else {
			   SQLColAttributes(result->stmt, (UWORD)(field_ind + 1), 
					   			(UWORD)((sql_c_type == SQL_C_BINARY) ? SQL_COLUMN_LENGTH :
					   			SQL_COLUMN_DISPLAY_SIZE),
					   			NULL, 0, NULL, &fieldsize);
			}
			/* For char data, the length of the returned string will be longreadlen - 1 */
			fieldsize = (result->longreadlen <= 0) ? 4096 : result->longreadlen;
			field = emalloc(fieldsize);
			if (!field) {
				php_error(E_WARNING, "Out of memory");
				RETURN_FALSE;
			}

		/* SQLGetData will truncate CHAR data to fieldsize - 1 bytes and append \0.
		 * For binary data it is truncated to fieldsize bytes. 
		 */
			rc = SQLGetData(result->stmt, (UWORD)(field_ind + 1), sql_c_type,
							field, fieldsize, &result->values[field_ind].vallen);

			if (rc == SQL_ERROR) {
				odbc_sql_error(result->conn_ptr, result->stmt, "SQLGetData");
				efree(field);
				RETURN_FALSE;
			}

			if (result->values[field_ind].vallen == SQL_NULL_DATA || rc == SQL_NO_DATA_FOUND) {
				efree(field);
				RETURN_FALSE;
			}
			/* Reduce fieldlen by 1 if we have char data. One day we might 
			   have binary strings... */
			if (result->values[field_ind].coltype == SQL_LONGVARCHAR) fieldsize -= 1;
			/* Don't duplicate result, saves one emalloc.
			   For SQL_SUCCESS, the length is in vallen.
			 */
			RETURN_STRINGL(field, (rc == SQL_SUCCESS_WITH_INFO) ? fieldsize :
						   result->values[field_ind].vallen, 0);
			break;
			
		default:
			if (result->values[field_ind].vallen == SQL_NULL_DATA) {
				RETURN_FALSE;
			} else {
				RETURN_STRINGL(result->values[field_ind].value, result->values[field_ind].vallen, 1);
			}
			break;
	}

/* If we come here, output unbound LONG and/or BINARY column data to the client */
	
	/* We emalloc 1 byte more for SQL_C_CHAR (trailing \0) */
	fieldsize = (sql_c_type == SQL_C_CHAR) ? 4096 : 4095;
	if ((field = emalloc(fieldsize)) == NULL) {
		php_error(E_WARNING,"Out of memory");
		RETURN_FALSE;
	}
	
	/* Call SQLGetData() until SQL_SUCCESS is returned */
	while(1) {
		rc = SQLGetData(result->stmt, (UWORD)(field_ind + 1),sql_c_type,
							field, fieldsize, &result->values[field_ind].vallen);

		if (rc == SQL_ERROR) {
			odbc_sql_error(result->conn_ptr, result->stmt, "SQLGetData");
			efree(field);
			RETURN_FALSE;
		}
		
		if (result->values[field_ind].vallen == SQL_NULL_DATA) {
			efree(field);
			RETURN_FALSE;
		}
		/* chop the trailing \0 by outputing only 4095 bytes */
		PHPWRITE(field,(rc == SQL_SUCCESS_WITH_INFO) ? 4095 :
						   result->values[field_ind].vallen);

		if (rc == SQL_SUCCESS) { /* no more data avail */
			efree(field);
			RETURN_TRUE;
		}
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int odbc_result_all(int result_id [, string format])
   Print result as HTML table */
PHP_FUNCTION(odbc_result_all)
{
	char *buf = NULL;
	int i, numArgs;
	odbc_result *result;
	RETCODE rc;
	pval **pv_res, **pv_format;
	SWORD sql_c_type;
#ifdef HAVE_SQL_EXTENDED_FETCH
	UDWORD crow;
	UWORD RowStatus[1];
#endif

	numArgs = ZEND_NUM_ARGS();
	if (numArgs ==  1) {
		if (zend_get_parameters_ex(1, &pv_res) == FAILURE)
			WRONG_PARAM_COUNT;
	} else {
		if (zend_get_parameters_ex(2, &pv_res, &pv_format) == FAILURE)
			WRONG_PARAM_COUNT;
	}
				
	ZEND_FETCH_RESOURCE(result, odbc_result *, pv_res, -1, "ODBC result", le_result);
	
	if (result->numcols == 0) {
		php_error(E_WARNING, "No tuples available at this result index");
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
	if (numArgs == 1) {
		php_printf("<table><tr>");
	} else {
		convert_to_string_ex(pv_format);	
		php_printf("<table %s ><tr>",Z_STRVAL_PP(pv_format)); 
	}
	
	for(i = 0; i < result->numcols; i++)
		php_printf("<th>%s</th>", result->values[i].name);

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
					if (IS_SQL_LONG(result->values[i].coltype) && 
						result->longreadlen <= 0) {
						php_printf("<td>Not printable</td>"); 
						break;
					}

					if (buf == NULL) buf = emalloc(result->longreadlen);

					rc = SQLGetData(result->stmt, (UWORD)(i + 1),sql_c_type,
								buf, result->longreadlen, &result->values[i].vallen);
 
					php_printf("<td>");

					if (rc == SQL_ERROR) {
						odbc_sql_error(result->conn_ptr, result->stmt, "SQLGetData");
						php_printf("</td></tr></table>");
						efree(buf);
						RETURN_FALSE;
					}
					if (rc == SQL_SUCCESS_WITH_INFO)
						php_printf(buf,result->longreadlen);
					else if (result->values[i].vallen == SQL_NULL_DATA) {
						php_printf("&nbsp;</td>");
						break;
					} else {
						php_printf(buf, result->values[i].vallen);
					}
					php_printf("</td>");
					break;
				default:
					if (result->values[i].vallen == SQL_NULL_DATA) {
						php_printf("<td>&nbsp;</td>");
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

/* {{{ proto int odbc_free_result(int result_id)
   Free resources associated with a result */
PHP_FUNCTION(odbc_free_result)
{
	pval **pv_res;
	odbc_result *result;
	int i;

	if (zend_get_parameters_ex(1, &pv_res) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(result, odbc_result *, pv_res, -1, "ODBC result", le_result);
	if (result->values) {
		for (i = 0; i < result->numcols; i++) {
			if (result->values[i].value) {
				efree(result->values[i].value);
			}
		}
		efree(result->values);
		result->values = NULL;
	}
			
	zend_list_delete(result->id);
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int odbc_connect(string DSN, string user, string password [, int cursor_option])
   Connect to a datasource */
PHP_FUNCTION(odbc_connect)
{
	odbc_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int odbc_pconnect(string DSN, string user, string password [, int cursor_option])
   Establish a persistent connection to a datasource */
PHP_FUNCTION(odbc_pconnect)
{
	odbc_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ odbc_sqlconnect */
int odbc_sqlconnect(odbc_connection **conn, char *db, char *uid, char *pwd, int cur_opt, int persistent TSRMLS_DC)
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
#ifdef HAVE_OPENLINK
	{
		char dsnbuf[300];
		short dsnbuflen;

		rc = SQLDriverConnect((*conn)->hdbc, NULL, db, SQL_NTS,
							dsnbuf, sizeof(dsnbuf) - 1, &dsnbuflen,
							SQL_DRIVER_NOPROMPT);
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
#if defined(HAVE_EMPRESS) || defined(HAVE_UNIXODBC)
/* *  Uncomment the line above, and comment line below to fully test 
 * #ifdef HAVE_EMPRESS */
	{
		int     direct = 0;
		char    dsnbuf[300];
		short   dsnbuflen;
		char    *ldb = 0;
		int		ldb_len = 0;

		if (strstr((char*)db, ";")) {
			direct = 1;
			if (uid && !strstr ((char*)db, "uid") && !strstr((char*)db, "UID")) {
				ldb = (char*) emalloc(strlen(db) + strlen(uid) + strlen(pwd) + 12);
				sprintf(ldb, "%s;UID=%s;PWD=%s", db, uid, pwd);
			} else {
				ldb_len = strlen(db)+1;
				ldb = (char*) emalloc(ldb_len);
				memcpy(ldb, db, ldb_len);
			}
		}

		if (direct)
			rc = SQLDriverConnect((*conn)->hdbc, NULL, ldb, strlen(ldb), dsnbuf, 300,
									&dsnbuflen, SQL_DRIVER_NOPROMPT);
		else
			rc = SQLConnect((*conn)->hdbc, db, SQL_NTS, uid, SQL_NTS, pwd, SQL_NTS);
		if (ldb)
			efree(ldb);
	}
#else
	rc = SQLConnect((*conn)->hdbc, db, SQL_NTS, uid, SQL_NTS, pwd, SQL_NTS);
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
 * connect, access violations were occuring.  This is because some of the
 * "globals" in this module should actualy be per-connection variables.  I
 * simply fixed things to get them working for now.  Shane
 */
/* {{{ odbc_do_connect */
void odbc_do_connect(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	char    *db = NULL;
	char    *uid = NULL;
	char    *pwd = NULL;
	pval **pv_db, **pv_uid, **pv_pwd, **pv_opt;
	odbc_connection *db_conn;
	char *hashed_details;
	int hashed_len, len, cur_opt;

	/*  Now an optional 4th parameter specifying the cursor type
	 *  defaulting to the cursors default
	 */
	switch(ZEND_NUM_ARGS()) {
		case 3:	
			if (zend_get_parameters_ex(3, &pv_db, &pv_uid, &pv_pwd) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			/* Use Default: Probably a better way to do this */
			cur_opt = SQL_CUR_DEFAULT;
			break;
		case 4:
			if (zend_get_parameters_ex(4, &pv_db, &pv_uid, &pv_pwd, &pv_opt) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_long_ex(pv_opt);
			cur_opt = Z_LVAL_PP(pv_opt);

			/* Confirm the cur_opt range */
			if (! (cur_opt == SQL_CUR_USE_IF_NEEDED || 
				cur_opt == SQL_CUR_USE_ODBC || 
				cur_opt == SQL_CUR_USE_DRIVER || 
				cur_opt == SQL_CUR_DEFAULT) ) {
				php_error_docref("odbc-connect" TSRMLS_CC, E_WARNING, "%s(): Invalid Cursor type (%d)", cur_opt);
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	convert_to_string_ex(pv_db);
	convert_to_string_ex(pv_uid);
	convert_to_string_ex(pv_pwd);

	db = Z_STRVAL_PP(pv_db);
	uid = Z_STRVAL_PP(pv_uid);
	pwd = Z_STRVAL_PP(pv_pwd);

	if (ODBCG(allow_persistent) <= 0) {
		persistent = 0;
	}

	len = strlen(db) + strlen(uid) + strlen(pwd) + sizeof(ODBC_TYPE) + 5;
	hashed_details = emalloc(len);

	if (hashed_details == NULL) {
		php_error(E_WARNING, "Out of memory");
		RETURN_FALSE;
	}

	hashed_len = sprintf(hashed_details, "%s_%s_%s_%s_%d", ODBC_TYPE, db, uid, pwd, cur_opt);

	/* FIXME the idea of checking to see if our connection is already persistent
		is good, but it adds a lot of overhead to non-persistent connections.  We
		should look and see if we can fix that somehow */
	/* try to find if we already have this link in our persistent list,
	 * no matter if it is to be persistent or not
	 */

try_and_get_another_connection:

	if (persistent) {
		list_entry *le;
		
		if (zend_hash_find(&EG(persistent_list), hashed_details, hashed_len + 1, (void **) &le)
		  					== FAILURE) { /* the link is not in the persistent list */
			list_entry new_le;
			
			if (ODBCG(max_links) != -1 && ODBCG(num_links) >= ODBCG(max_links)) {
				php_error_docref("odbc-exec" TSRMLS_CC, E_WARNING, "%s(): Too many open links (%d)", ODBCG(num_links));
				efree(hashed_details);
				RETURN_FALSE;
			}
			if (ODBCG(max_persistent) != -1 && ODBCG(num_persistent) >= ODBCG(max_persistent)) {
				php_error_docref("odbc-pconnect" TSRMLS_CC, E_WARNING,"%s(): Too many open persistent links (%d)", ODBCG(num_persistent));
				efree(hashed_details);
				RETURN_FALSE;
			}
			
			if (!odbc_sqlconnect(&db_conn, db, uid, pwd, cur_opt, 1 TSRMLS_CC)) {
				efree(hashed_details);
				RETURN_FALSE;
			}
			
			Z_TYPE(new_le) = le_pconn;
			new_le.ptr = db_conn;
			if (zend_hash_update(&EG(persistent_list), hashed_details, hashed_len + 1, &new_le,
						sizeof(list_entry), NULL) == FAILURE) {
				free(db_conn);
				efree(hashed_details);
				RETURN_FALSE;
			}
			ODBCG(num_persistent)++;
			ODBCG(num_links)++;
			db_conn->id = ZEND_REGISTER_RESOURCE(return_value, db_conn, le_pconn);
		} else { /* found connection */
			if (Z_TYPE_P(le) != le_pconn) {
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
				SWORD len;

				ret = SQLGetInfo(db_conn->hdbc, 
					SQL_DATA_SOURCE_READ_ONLY, 
					d_name, sizeof(d_name), &len);

				if(ret != SQL_SUCCESS || len == 0) {
					zend_hash_del(&EG(persistent_list), hashed_details, hashed_len + 1);
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
		db_conn->id = ZEND_REGISTER_RESOURCE(return_value, db_conn, le_pconn);
	} else { /* non persistent */
		list_entry *index_ptr, new_index_ptr;
		
		if (zend_hash_find(&EG(regular_list), hashed_details, hashed_len + 1, 
					(void **) &index_ptr) == SUCCESS) {
			int type, conn_id;
			void *ptr;		
			if (Z_TYPE_P(index_ptr) != le_index_ptr) {
				RETURN_FALSE;
			}
			conn_id = (int)index_ptr->ptr;
			ptr = zend_list_find(conn_id, &type);   /* check if the connection is still there */
			if (ptr && (type == le_conn || type == le_pconn)) {
				zend_list_addref(conn_id);
				Z_LVAL_P(return_value) = conn_id;
				Z_TYPE_P(return_value) = IS_RESOURCE;
				efree(hashed_details);
				return;
			} else {
				zend_hash_del(&EG(regular_list), hashed_details, hashed_len + 1);
			}
		}
		if (ODBCG(max_links) != -1 && ODBCG(num_links) >= ODBCG(max_links)) {
			php_error_docref("odbc-connect" TSRMLS_CC, E_WARNING,"%s(): Too many open connections (%d)",ODBCG(num_links));
			efree(hashed_details);
			RETURN_FALSE;
		}

		if (!odbc_sqlconnect(&db_conn, db, uid, pwd, cur_opt, 0 TSRMLS_CC)) {
			efree(hashed_details);
			RETURN_FALSE;
		}
		db_conn->id = ZEND_REGISTER_RESOURCE(return_value, db_conn, le_conn);
		new_index_ptr.ptr = (void *) Z_LVAL_P(return_value);
		Z_TYPE(new_index_ptr) = le_index_ptr;
		if (zend_hash_update(&EG(regular_list), hashed_details, hashed_len + 1, (void *) &new_index_ptr,
				   sizeof(list_entry), NULL) == FAILURE) {
			efree(hashed_details);
			RETURN_FALSE;
			/* XXX Free Connection */
		}
		ODBCG(num_links)++;
	}
	efree(hashed_details);
}
/* }}} */

/* {{{ proto void odbc_close(int connection_id)
   Close an ODBC connection */
PHP_FUNCTION(odbc_close)
{
	pval **pv_conn;
	void *ptr;
	odbc_connection *conn;
	odbc_result *res;
	int nument;
	int i;
	int type;
	int is_pconn = 0;
	int found_resource_type = le_conn;

	if (zend_get_parameters_ex(1, &pv_conn) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	conn = (odbc_connection *) zend_fetch_resource(pv_conn TSRMLS_CC, -1, "ODBC-Link", &found_resource_type, 2, le_conn, le_pconn);
	if (found_resource_type==le_pconn) {
		is_pconn = 1;
	}

	nument = zend_hash_next_free_element(&EG(regular_list));

	for(i = 1; i < nument; i++){
		ptr = zend_list_find(i, &type);
		if(ptr && (type == le_result)){
			res = (odbc_result *)ptr;
			if(res->conn_ptr == conn){
				zend_list_delete(i);
			}
		}
	}
	
	zend_list_delete(Z_LVAL_PP(pv_conn));
	
	if(is_pconn){
		zend_hash_apply_with_argument(&EG(persistent_list),
			(apply_func_arg_t) _close_pconn_with_id, (void *) &(Z_LVAL_PP(pv_conn)) TSRMLS_CC);	
	}
}
/* }}} */

/* {{{ proto int odbc_num_rows(int result_id)
   Get number of rows in a result */
PHP_FUNCTION(odbc_num_rows)
{
	odbc_result *result;
	SDWORD rows;
	pval **pv_res;
	
	if (zend_get_parameters_ex(1, &pv_res) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(result, odbc_result *, pv_res, -1, "ODBC result", le_result);
	SQLRowCount(result->stmt, &rows);
	RETURN_LONG(rows);
}
/* }}} */

#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30)
/* {{{ proto bool odbc_next_result(int result_id)
   Checks if multiple results are avaiable */
PHP_FUNCTION(odbc_next_result)
{
	odbc_result   *result;
	pval     **pv_res;
	int rc, i;

	if (zend_get_parameters_ex(1, &pv_res) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(result, odbc_result *, pv_res, -1, "ODBC result", le_result); 

	if (result->values) {
		for(i = 0; i < result->numcols; i++) {
			if (result->values[i].value)
				efree(result->values[i].value);
		}
		efree(result->values);
		result->values = NULL;
	}

	result->fetched = 0;
	rc = SQLMoreResults(result->stmt);
	if (rc == SQL_SUCCESS) {
		RETURN_TRUE;
	}
	else if (rc == SQL_SUCCESS_WITH_INFO) {
		rc = SQLFreeStmt(result->stmt, SQL_UNBIND);
		SQLNumParams(result->stmt, &(result->numparams));
		SQLNumResultCols(result->stmt, &(result->numcols));

		if (result->numcols > 0) {
			if (!odbc_bindcols(result TSRMLS_CC)) {
				efree(result);
				RETVAL_FALSE;
			}
		} else {
			result->values = NULL;
		}
		RETURN_TRUE;
	}
	else {
		RETURN_FALSE;
	}
}
/* }}} */
#endif

/* {{{ proto int odbc_num_fields(int result_id)
   Get number of columns in a result */
PHP_FUNCTION(odbc_num_fields)
{
	odbc_result *result;
	pval **pv_res;

	if (zend_get_parameters_ex(1, &pv_res) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(result, odbc_result *, pv_res, -1, "ODBC result", le_result); 
	RETURN_LONG(result->numcols);
}
/* }}} */

/* {{{ proto string odbc_field_name(int result_id, int field_number)
   Get a column name */
PHP_FUNCTION(odbc_field_name)
{
	odbc_result *result;
	pval **pv_res, **pv_num;
	
	if (zend_get_parameters_ex(2, &pv_res, &pv_num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(pv_num);
	
	ZEND_FETCH_RESOURCE(result, odbc_result *, pv_res, -1, "ODBC result", le_result);
	
	if (result->numcols == 0) {
		php_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
	
	if (Z_LVAL_PP(pv_num) > result->numcols) {
		php_error(E_WARNING, "Field index larger than number of fields");
		RETURN_FALSE;
	}
	
	if (Z_LVAL_PP(pv_num) < 1) {
		php_error(E_WARNING, "Field numbering starts at 1");
		RETURN_FALSE;
	}
	
	RETURN_STRING(result->values[Z_LVAL_PP(pv_num) - 1].name, 1)
}
/* }}} */

/* {{{ proto string odbc_field_type(int result_id, int field_number)
   Get the datatype of a column */
PHP_FUNCTION(odbc_field_type)
{
	odbc_result	*result;
	char    	tmp[32];
	SWORD   	tmplen;
	pval     **pv_res, **pv_num;

	if (zend_get_parameters_ex(2, &pv_res, &pv_num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(pv_num);

	ZEND_FETCH_RESOURCE(result, odbc_result *, pv_res, -1, "ODBC result", le_result);

	if (result->numcols == 0) {
		php_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
	
	if (Z_LVAL_PP(pv_num) > result->numcols) {
		php_error(E_WARNING, "Field index larger than number of fields");
		RETURN_FALSE;
	}

	if (Z_LVAL_PP(pv_num) < 1) {
		php_error(E_WARNING, "Field numbering starts at 1");
		RETURN_FALSE;
	}

	SQLColAttributes(result->stmt, (UWORD)Z_LVAL_PP(pv_num),
					 SQL_COLUMN_TYPE_NAME, tmp, 31, &tmplen, NULL);
	RETURN_STRING(tmp,1)
}
/* }}} */

/* {{{ proto int odbc_field_len(int result_id, int field_number)
   Get the length (precision) of a column */
PHP_FUNCTION(odbc_field_len)
{
	odbc_column_lengths(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int odbc_field_scale(int result_id, int field_number)
   Get the scale of a column */
PHP_FUNCTION(odbc_field_scale)
{
	odbc_column_lengths(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);	
}
/* }}} */

/* {{{ proto int odbc_field_num(int result_id, string field_name)
   Return column number */
PHP_FUNCTION(odbc_field_num)
{
	int field_ind;
	char *fname;
	odbc_result *result;
	int i;
	pval **pv_res, **pv_name;

	if (zend_get_parameters_ex(2, &pv_res, &pv_name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(result, odbc_result *, pv_res, -1, "ODBC result", le_result);
	
	if (result->numcols == 0) {
		php_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}

	convert_to_string_ex(pv_name);
	fname = Z_STRVAL_PP(pv_name);

	field_ind = -1;
	for(i = 0; i < result->numcols; i++) {
		if (strcasecmp(result->values[i].name, fname) == 0)
			field_ind = i + 1;
		}
	
	if (field_ind == -1)
		RETURN_FALSE;
	RETURN_LONG(field_ind);
}
/* }}} */

/* {{{ proto int odbc_autocommit(int connection_id [, int OnOff])
   Toggle autocommit mode or get status */
/* There can be problems with pconnections!*/
PHP_FUNCTION(odbc_autocommit)
{
	odbc_connection *conn;
	RETCODE rc;
	pval **pv_conn, **pv_onoff = NULL;
	int argc;

	argc = ZEND_NUM_ARGS();
	if (argc == 2) {
		if (zend_get_parameters_ex(2, &pv_conn, &pv_onoff) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else if (argc == 1) {
		if (zend_get_parameters_ex(1, &pv_conn) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE2(conn, odbc_connection *, pv_conn, -1, "ODBC-Link", le_conn, le_pconn);
	
	if (pv_onoff && (*pv_onoff)) {
		convert_to_long_ex(pv_onoff);
		rc = SQLSetConnectOption(conn->hdbc, SQL_AUTOCOMMIT,
								 (Z_LVAL_PP(pv_onoff)) ?
								 SQL_AUTOCOMMIT_ON : SQL_AUTOCOMMIT_OFF);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			odbc_sql_error(conn, SQL_NULL_HSTMT, "Set autocommit");
			RETURN_FALSE;
		}
		RETVAL_TRUE;
	} else {
		SDWORD status;

		rc = SQLGetConnectOption(conn->hdbc, SQL_AUTOCOMMIT, (PTR)&status);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			odbc_sql_error(conn, SQL_NULL_HSTMT, "Get commit status");
			RETURN_FALSE;
		}
		RETVAL_LONG((long)status);
	}
}
/* }}} */

/* {{{ proto int odbc_commit(int connection_id)
   Commit an ODBC transaction */
PHP_FUNCTION(odbc_commit)
{
	odbc_transact(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto int odbc_rollback(int connection_id)
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
	pval **pv_handle;
	char *ptr;
	int argc, len;

	argc = ZEND_NUM_ARGS();

	if (argc > 1 || zend_get_parameters_ex(argc, &pv_handle)) {
		WRONG_PARAM_COUNT;
	}

	if (mode == 0) {  /* last state */
		len = 6;
	} else { /* last error message */
		len = SQL_MAX_MESSAGE_LENGTH;
	}
	ptr = ecalloc(len + 1, 1);
	if (argc == 1) {
		ZEND_FETCH_RESOURCE2(conn, odbc_connection *, pv_handle, -1, "ODBC-Link", le_conn, le_pconn);
		if (mode == 0) {
			strlcpy(ptr, conn->laststate, len+1);
		} else {
			strlcpy(ptr, conn->lasterrormsg, len+1);
		}
	} else {
		if (mode == 0) {
			strlcpy(ptr, ODBCG(laststate), len+1);
		} else {
			strlcpy(ptr, ODBCG(lasterrormsg), len+1);
		}
	}
	RETVAL_STRING(ptr, 0);
}
/* }}} */

/* {{{ proto string odbc_error([int connection_id])
   Get the last error code */
PHP_FUNCTION(odbc_error)
{
	php_odbc_lasterror(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto string odbc_errormsg([int connection_id])
   Get the last error message */
PHP_FUNCTION(odbc_errormsg)
{
	php_odbc_lasterror(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto int odbc_setoption(int conn_id|result_id, int which, int option, int value)
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
	pval **pv_handle, **pv_which, **pv_opt, **pv_val;

	if (zend_get_parameters_ex(4, &pv_handle, &pv_which, &pv_opt, &pv_val) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(pv_which);
	convert_to_long_ex(pv_opt);
	convert_to_long_ex(pv_val);

	switch (Z_LVAL_PP(pv_which)) {
		case 1:		/* SQLSetConnectOption */
			ZEND_FETCH_RESOURCE2(conn, odbc_connection *, pv_handle, -1, "ODBC-Link", le_conn, le_pconn);
			if (conn->persistent) {
				php_error(E_WARNING, "Unable to set option for persistent connection");
				RETURN_FALSE;
			}
			rc = SQLSetConnectOption(conn->hdbc, (unsigned short)(Z_LVAL_PP(pv_opt)), Z_LVAL_PP(pv_val));
			if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
				odbc_sql_error(conn, SQL_NULL_HSTMT, "SetConnectOption");
				RETURN_FALSE;
			}
			break;
		case 2:		/* SQLSetStmtOption */
			ZEND_FETCH_RESOURCE(result, odbc_result *, pv_handle, -1, "ODBC result", le_result);
			
			rc = SQLSetStmtOption(result->stmt, (unsigned short)(Z_LVAL_PP(pv_opt)), (Z_LVAL_PP(pv_val)));

			if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
				odbc_sql_error(result->conn_ptr, result->stmt, "SetStmtOption");
				RETURN_FALSE;
			}
			break;
		default:
			php_error_docref("odbc-setoption" TSRMLS_CC, E_WARNING, "%s(): Unknown option type");
			RETURN_FALSE;
			break;
	}

	RETURN_TRUE;
}
/* }}} */

/*
 * metadata functions
 */

/* {{{ proto int odbc_tables(int connection_id [, string qualifier, string owner, string name, string table_types])
   Call the SQLTables function */
PHP_FUNCTION(odbc_tables)
{
	pval **pv_conn, **pv_cat, **pv_schema, **pv_table, **pv_type;
	odbc_result   *result = NULL;
	odbc_connection *conn;
	char *cat = NULL, *schema = NULL, *table = NULL, *type = NULL;
	RETCODE rc;
	int argc;

	argc = ZEND_NUM_ARGS();
	if (argc < 1 || argc > 5 || zend_get_parameters_ex(argc, &pv_conn, &pv_cat, &pv_schema, &pv_table, &pv_type) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	switch (argc) {
		case 5:
			convert_to_string_ex(pv_type);
			type = Z_STRVAL_PP(pv_type);
		case 4:
			convert_to_string_ex(pv_table);
			table = Z_STRVAL_PP(pv_table);
		case 3:
			convert_to_string_ex(pv_schema);
			schema = Z_STRVAL_PP(pv_schema);
		case 2:
			convert_to_string_ex(pv_cat);
			cat = Z_STRVAL_PP(pv_cat);
	}

	ZEND_FETCH_RESOURCE2(conn, odbc_connection *, pv_conn, -1, "ODBC-Link", le_conn, le_pconn);

	result = (odbc_result *)emalloc(sizeof(odbc_result));
	if (result == NULL) {
		php_error(E_WARNING, "Out of memory");
		RETURN_FALSE;
	}
	
	rc = SQLAllocStmt(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		efree(result);
		php_error_docref("odbc-tables" TSRMLS_CC, E_WARNING, "%s(): SQLAllocStmt error 'Invalid Handle'");
		RETURN_FALSE;
	}

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLAllocStmt");
		efree(result);
		RETURN_FALSE;
	}

	/* This hack is needed to access table information in Access databases (fmk) */
	if (table && strlen(table) && schema && !strlen(schema)) schema = NULL;

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
		if (!odbc_bindcols(result TSRMLS_CC)) {
			efree(result);
			RETURN_FALSE;
		}
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;
	ZEND_REGISTER_RESOURCE(return_value, result, le_result);
}
/* }}} */

/* {{{ proto int odbc_columns(int connection_id, string qualifier, string owner, string table_name, string column_name)
   Returns a result identifier that can be used to fetch a list of column names in specified tables */
PHP_FUNCTION(odbc_columns)
{
	pval **pv_conn, **pv_cat, **pv_schema, **pv_table, **pv_column;
	odbc_result   *result = NULL;
	odbc_connection *conn;
	char *cat = NULL, *schema = NULL, *table = NULL, *column = NULL;
	SWORD cat_len=0, schema_len=0, table_len=0, column_len=0;
	RETCODE rc;
	int argc;

	argc = ZEND_NUM_ARGS();
	if (argc < 1 || argc > 5 || zend_get_parameters_ex(argc, &pv_conn, &pv_cat, &pv_schema, &pv_table, &pv_column) == FAILURE) {
			WRONG_PARAM_COUNT;
	}
	switch (argc) {
		case 5:
			convert_to_string_ex(pv_column);
			column = Z_STRVAL_PP(pv_column);
			column_len = Z_STRLEN_PP(pv_column);
		case 4:
			convert_to_string_ex(pv_table);
			table = Z_STRVAL_PP(pv_table);
			table_len = Z_STRLEN_PP(pv_table);
		case 3:
			convert_to_string_ex(pv_schema);
			schema = Z_STRVAL_PP(pv_schema);
			schema_len = Z_STRLEN_PP(pv_schema);
		case 2:
			convert_to_string_ex(pv_cat);
			cat = Z_STRVAL_PP(pv_cat);
			cat_len = Z_STRLEN_PP(pv_cat);
	}

	ZEND_FETCH_RESOURCE2(conn, odbc_connection *, pv_conn, -1, "ODBC-Link", le_conn, le_pconn);

	result = (odbc_result *)emalloc(sizeof(odbc_result));
	if (result == NULL) {
		php_error(E_WARNING, "Out of memory");
		RETURN_FALSE;
	}
	
	rc = SQLAllocStmt(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		efree(result);
		php_error_docref("odbc-columns" TSRMLS_CC, E_WARNING, "%s(): SQLAllocStmt error 'Invalid Handle'");
		RETURN_FALSE;
	}

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLAllocStmt");
		efree(result);
		RETURN_FALSE;
	}

	rc = SQLColumns(result->stmt, 
			cat, cat_len,
			schema, schema_len,
			table, table_len,
			column, column_len);

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLColumns");
		efree(result);
		RETURN_FALSE;
	}

	result->numparams = 0;
	SQLNumResultCols(result->stmt, &(result->numcols));

	if (result->numcols > 0) {
		if (!odbc_bindcols(result TSRMLS_CC)) {
			efree(result);
			RETURN_FALSE;
		}
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;
	ZEND_REGISTER_RESOURCE(return_value, result, le_result);
}
/* }}} */

#if !defined(HAVE_DBMAKER) && !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35) && !defined(HAVE_BIRDSTEP)
/* {{{ proto int odbc_columnprivileges(int connection_id, string catalog, string schema, string table, string column)
   Returns a result identifier that can be used to fetch a list of columns and associated privileges for the specified table */
PHP_FUNCTION(odbc_columnprivileges)
{
	pval **pv_conn, **pv_cat, **pv_schema, **pv_table, **pv_column;
	odbc_result   *result = NULL;
	odbc_connection *conn;
	char *cat = NULL, *schema = NULL, *table = NULL, *column = NULL;
	RETCODE rc;
	int argc;

	argc = ZEND_NUM_ARGS();
	if (argc == 5) {
		if (zend_get_parameters_ex(5, &pv_conn, &pv_cat, &pv_schema, &pv_table, &pv_column) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_string_ex(pv_cat);
		cat = Z_STRVAL_PP(pv_cat);
		convert_to_string_ex(pv_schema);
		schema = Z_STRVAL_PP(pv_schema);
		convert_to_string_ex(pv_table);
		table = Z_STRVAL_PP(pv_table);
		convert_to_string_ex(pv_column);
		column = Z_STRVAL_PP(pv_column);
	} else {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE2(conn, odbc_connection *, pv_conn, -1, "ODBC-Link", le_conn, le_pconn);

	result = (odbc_result *)emalloc(sizeof(odbc_result));
	if (result == NULL) {
		php_error(E_WARNING, "Out of memory");
		RETURN_FALSE;
	}
	
	rc = SQLAllocStmt(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		efree(result);
		php_error_docref("odbc-columnprivileges" TSRMLS_CC, E_WARNING, "%s(): SQLAllocStmt error 'Invalid Handle'");
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
		if (!odbc_bindcols(result TSRMLS_CC)) {
			efree(result);
			RETURN_FALSE;
		}
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;
	ZEND_REGISTER_RESOURCE(return_value, result, le_result);
}
/* }}} */
#endif /* HAVE_DBMAKER || HAVE_SOLID*/

#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35)
/* {{{ proto int odbc_foreignkeys(int connection_id, string pk_qualifier, string pk_owner, string pk_table, string fk_qualifier, string fk_owner, string fk_table)
   Returns a result identifier to either a list of foreign keys in the specified table or a list of foreign keys in other tables that refer to the primary key in the specified table */
PHP_FUNCTION(odbc_foreignkeys)
{
	pval **pv_conn, **pv_pcat, **pv_pschema, **pv_ptable;
	pval **pv_fcat, **pv_fschema, **pv_ftable;
	odbc_result   *result = NULL;
	odbc_connection *conn;
	char *pcat = NULL, *pschema = NULL, *ptable = NULL;
	char *fcat = NULL, *fschema = NULL, *ftable = NULL;
	RETCODE rc;
	int argc;

	argc = ZEND_NUM_ARGS();
	if (argc == 7) {
		if (zend_get_parameters_ex(7, &pv_conn, &pv_pcat, &pv_pschema, &pv_ptable, 
					&pv_fcat, &pv_fschema, &pv_ftable) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_string_ex(pv_pcat);
		pcat = Z_STRVAL_PP(pv_pcat);
		convert_to_string_ex(pv_pschema);
		pschema = Z_STRVAL_PP(pv_pschema);
		convert_to_string_ex(pv_ptable);
		ptable = Z_STRVAL_PP(pv_ptable);
		convert_to_string_ex(pv_fcat);
		fcat = Z_STRVAL_PP(pv_fcat);
		convert_to_string_ex(pv_fschema);
		fschema = Z_STRVAL_PP(pv_fschema);
		convert_to_string_ex(pv_ftable);
		ftable = Z_STRVAL_PP(pv_ftable);
#ifdef HAVE_DBMAKER
#define EMPTY_TO_NULL(xstr) \
	if ((int)strlen((xstr)) == 0) (xstr) = NULL

		EMPTY_TO_NULL(pcat);
		EMPTY_TO_NULL(pschema);
		EMPTY_TO_NULL(ptable);
		EMPTY_TO_NULL(fcat);
		EMPTY_TO_NULL(fschema);
		EMPTY_TO_NULL(ftable);
#endif
	} else {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE2(conn, odbc_connection *, pv_conn, -1, "ODBC-Link", le_conn, le_pconn);

	result = (odbc_result *)emalloc(sizeof(odbc_result));
	if (result == NULL) {
		php_error(E_WARNING, "Out of memory");
		RETURN_FALSE;
	}
	
	rc = SQLAllocStmt(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		efree(result);
		php_error_docref("odbc-foreignkeys" TSRMLS_CC, E_WARNING, "%s(): SQLAllocStmt error 'Invalid Handle'");
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
		if (!odbc_bindcols(result TSRMLS_CC)) {
			efree(result);
			RETURN_FALSE;
		}
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;
	ZEND_REGISTER_RESOURCE(return_value, result, le_result);
}
/* }}} */
#endif /* HAVE_SOLID */

/* {{{ proto int odbc_gettypeinfo(int connection_id [, int data_type])
   Returns a result identifier containing information about data types supported by the data source */
PHP_FUNCTION(odbc_gettypeinfo)
{
	pval **pv_conn, **pv_data_type;
	odbc_result   *result = NULL;
	odbc_connection *conn;
	RETCODE rc;
	int argc;
	SWORD data_type = SQL_ALL_TYPES;

	argc = ZEND_NUM_ARGS();
	if (argc == 1) {
		if (zend_get_parameters_ex(1, &pv_conn) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else if (argc == 2) {
		if (zend_get_parameters_ex(2, &pv_conn, &pv_data_type) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(pv_data_type);
		data_type = (SWORD) Z_LVAL_PP(pv_data_type);
	} else {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE2(conn, odbc_connection *, pv_conn, -1, "ODBC-Link", le_conn, le_pconn);

	result = (odbc_result *)emalloc(sizeof(odbc_result));
	if (result == NULL) {
		php_error(E_WARNING, "Out of memory");
		RETURN_FALSE;
	}
	
	rc = SQLAllocStmt(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		efree(result);
		php_error_docref("odbc-gettypeinfo" TSRMLS_CC, E_WARNING, "%s(): SQLAllocStmt error 'Invalid Handle'");
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
		if (!odbc_bindcols(result TSRMLS_CC)) {
			efree(result);
			RETURN_FALSE;
		}
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;
	ZEND_REGISTER_RESOURCE(return_value, result, le_result);
}
/* }}} */

/* {{{ proto int odbc_primarykeys(int connection_id, string qualifier, string owner, string table)
   Returns a result identifier listing the column names that comprise the primary key for a table */
PHP_FUNCTION(odbc_primarykeys)
{
	pval **pv_conn, **pv_cat, **pv_schema, **pv_table;
	odbc_result   *result = NULL;
	odbc_connection *conn;
	char *cat = NULL, *schema = NULL, *table = NULL;
	RETCODE rc;
	int argc;

	argc = ZEND_NUM_ARGS();
	if (argc == 4) {
		if (zend_get_parameters_ex(4, &pv_conn, &pv_cat, &pv_schema, &pv_table) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_string_ex(pv_cat);
		cat = Z_STRVAL_PP(pv_cat);
		convert_to_string_ex(pv_schema);
		schema = Z_STRVAL_PP(pv_schema);
		convert_to_string_ex(pv_table);
		table = Z_STRVAL_PP(pv_table);
	} else {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE2(conn, odbc_connection *, pv_conn, -1, "ODBC-Link", le_conn, le_pconn);

	result = (odbc_result *)emalloc(sizeof(odbc_result));
	if (result == NULL) {
		php_error(E_WARNING, "Out of memory");
		RETURN_FALSE;
	}
	
	rc = SQLAllocStmt(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		efree(result);
		php_error_docref("odbc-primarykeys" TSRMLS_CC, E_WARNING, "%s(): SQLAllocStmt error 'Invalid Handle'");
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
		if (!odbc_bindcols(result TSRMLS_CC)) {
			efree(result);
			RETURN_FALSE;
		}
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;
	ZEND_REGISTER_RESOURCE(return_value, result, le_result);
}
/* }}} */

#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35) && !defined(HAVE_BIRDSTEP)
/* {{{ proto int odbc_procedurecolumns(int connection_id [, string qualifier, string owner, string proc, string column])
   Returns a result identifier containing the list of input and output parameters, as well as the columns that make up the result set for the specified procedures */
PHP_FUNCTION(odbc_procedurecolumns)
{
	pval **pv_conn, **pv_cat, **pv_schema, **pv_proc, **pv_col;
	odbc_result   *result = NULL;
	odbc_connection *conn;
	char *cat = NULL, *schema = NULL, *proc = NULL, *col = NULL;
	RETCODE rc;
	int argc;

	argc = ZEND_NUM_ARGS();
	if (argc == 1) {
		if (zend_get_parameters_ex(1, &pv_conn) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else if (argc == 5) {
		if (zend_get_parameters_ex(5, &pv_conn, &pv_cat, &pv_schema, &pv_proc, &pv_col) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_string_ex(pv_cat);
		cat = Z_STRVAL_PP(pv_cat);
		convert_to_string_ex(pv_schema);
		schema = Z_STRVAL_PP(pv_schema);
		convert_to_string_ex(pv_proc);
		proc = Z_STRVAL_PP(pv_proc);
		convert_to_string_ex(pv_col);
		col = Z_STRVAL_PP(pv_col);
	} else {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE2(conn, odbc_connection *, pv_conn, -1, "ODBC-Link", le_conn, le_pconn);

	result = (odbc_result *)emalloc(sizeof(odbc_result));
	if (result == NULL) {
		php_error(E_WARNING, "Out of memory");
		RETURN_FALSE;
	}
	
	rc = SQLAllocStmt(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		efree(result);
		php_error_docref("odbc-procedurecolumns" TSRMLS_CC, E_WARNING, "%s(): SQLAllocStmt error 'Invalid Handle'");
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
		if (!odbc_bindcols(result TSRMLS_CC)) {
			efree(result);
			RETURN_FALSE;
		}
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;
	ZEND_REGISTER_RESOURCE(return_value, result, le_result);
}
/* }}} */
#endif /* HAVE_SOLID */

#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35)
/* {{{ proto int odbc_procedures(int connection_id [, string qualifier, string owner, string name])
   Returns a result identifier containg the list of procedure names in a datasource */
PHP_FUNCTION(odbc_procedures)
{
	pval **pv_conn, **pv_cat, **pv_schema, **pv_proc;
	odbc_result   *result = NULL;
	odbc_connection *conn;
	char *cat = NULL, *schema = NULL, *proc = NULL;
	RETCODE rc;
	int argc;

	argc = ZEND_NUM_ARGS();
	if (argc == 1) {
		if (zend_get_parameters_ex(1, &pv_conn) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else if (argc == 4) {
		if (zend_get_parameters_ex(4, &pv_conn, &pv_cat, &pv_schema, &pv_proc) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_string_ex(pv_cat);
		cat = Z_STRVAL_PP(pv_cat);
		convert_to_string_ex(pv_schema);
		schema = Z_STRVAL_PP(pv_schema);
		convert_to_string_ex(pv_proc);
		proc = Z_STRVAL_PP(pv_proc);
	} else {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE2(conn, odbc_connection *, pv_conn, -1, "ODBC-Link", le_conn, le_pconn);

	result = (odbc_result *)emalloc(sizeof(odbc_result));
	if (result == NULL) {
		php_error(E_WARNING, "Out of memory");
		RETURN_FALSE;
	}
	
	rc = SQLAllocStmt(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		efree(result);
		php_error_docref("odbc-procedures" TSRMLS_CC, E_WARNING, "%s(): SQLAllocStmt error 'Invalid Handle'");
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
		if (!odbc_bindcols(result TSRMLS_CC)) {
			efree(result);
			RETURN_FALSE;
		}
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;
	ZEND_REGISTER_RESOURCE(return_value, result, le_result);
}
/* }}} */
#endif /* HAVE_SOLID */

/* {{{ proto int odbc_specialcolumns(int connection_id, int type, string qualifier, string owner, string table, int scope, int nullable)
   Returns a result identifier containing either the optimal set of columns that uniquely identifies a row in the table or columns that are automatically updated when any value in the row is updated by a transaction */
PHP_FUNCTION(odbc_specialcolumns)
{
	pval **pv_conn, **pv_type, **pv_cat, **pv_schema, **pv_name;
	pval **pv_scope, **pv_nullable;
	odbc_result   *result = NULL;
	odbc_connection *conn;
	char *cat = NULL, *schema = NULL, *name = NULL;
	UWORD type;
	UWORD scope, nullable;
	RETCODE rc;
	int argc;

	argc = ZEND_NUM_ARGS();
	if (argc == 7) {
		if (zend_get_parameters_ex(7, &pv_conn, &pv_type, &pv_cat, &pv_schema, 
					&pv_name, &pv_scope, &pv_nullable) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(pv_type);
		type = (UWORD) Z_LVAL_PP(pv_type);
		convert_to_string_ex(pv_cat);
		cat = Z_STRVAL_PP(pv_cat);
		convert_to_string_ex(pv_schema);
		schema = Z_STRVAL_PP(pv_schema);
		convert_to_string_ex(pv_name);
		name = Z_STRVAL_PP(pv_name);
		convert_to_long_ex(pv_scope);
		scope = (UWORD) Z_LVAL_PP(pv_scope);
		convert_to_long_ex(pv_nullable);
		nullable = (UWORD) Z_LVAL_PP(pv_nullable);
	} else {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE2(conn, odbc_connection *, pv_conn, -1, "ODBC-Link", le_conn, le_pconn);

	result = (odbc_result *)emalloc(sizeof(odbc_result));
	if (result == NULL) {
		php_error(E_WARNING, "Out of memory");
		RETURN_FALSE;
	}
	
	rc = SQLAllocStmt(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		efree(result);
		php_error_docref("odbc-specialcolumns" TSRMLS_CC, E_WARNING, "%s(): SQLAllocStmt error 'Invalid Handle'");
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
		if (!odbc_bindcols(result TSRMLS_CC)) {
			efree(result);
			RETURN_FALSE;
		}
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;
	ZEND_REGISTER_RESOURCE(return_value, result, le_result);
}
/* }}} */

/* {{{ proto int odbc_statistics(int connection_id, string qualifier, string owner, string name, int unique, int accuracy)
   Returns a result identifier that contains statistics about a single table and the indexes associated with the table */
PHP_FUNCTION(odbc_statistics)
{
	pval **pv_conn, **pv_cat, **pv_schema, **pv_name;
	pval **pv_unique, **pv_reserved;
	odbc_result   *result = NULL;
	odbc_connection *conn;
	char *cat = NULL, *schema = NULL, *name = NULL;
	UWORD unique, reserved;
	RETCODE rc;
	int argc;

	argc = ZEND_NUM_ARGS();
	if (argc == 6) {
		if (zend_get_parameters_ex(6, &pv_conn, &pv_cat, &pv_schema, 
					&pv_name, &pv_unique, &pv_reserved) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_string_ex(pv_cat);
		cat = Z_STRVAL_PP(pv_cat);
		convert_to_string_ex(pv_schema);
		schema = Z_STRVAL_PP(pv_schema);
		convert_to_string_ex(pv_name);
		name = Z_STRVAL_PP(pv_name);
		convert_to_long_ex(pv_unique);
		unique = (UWORD) Z_LVAL_PP(pv_unique);
		convert_to_long_ex(pv_reserved);
		reserved = (UWORD) Z_LVAL_PP(pv_reserved);
	} else {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE2(conn, odbc_connection *, pv_conn, -1, "ODBC-Link", le_conn, le_pconn);

	result = (odbc_result *)emalloc(sizeof(odbc_result));
	if (result == NULL) {
		php_error(E_WARNING, "Out of memory");
		RETURN_FALSE;
	}
	
	rc = SQLAllocStmt(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		efree(result);
		php_error_docref("odbc-statistics" TSRMLS_CC, E_WARNING, "%s(): SQLAllocStmt error 'Invalid Handle'");
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
		if (!odbc_bindcols(result TSRMLS_CC)) {
			efree(result);
			RETURN_FALSE;
		}
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;
	ZEND_REGISTER_RESOURCE(return_value, result, le_result);
}
/* }}} */

#if !defined(HAVE_DBMAKER) && !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35) && !defined(HAVE_BIRDSTEP)
/* {{{ proto int odbc_tableprivileges(int connection_id, string qualifier, string owner, string name)
   Returns a result identifier containing a list of tables and the privileges associated with each table */
PHP_FUNCTION(odbc_tableprivileges)
{
	pval **pv_conn, **pv_cat, **pv_schema, **pv_table;
	odbc_result   *result = NULL;
	odbc_connection *conn;
	char *cat = NULL, *schema = NULL, *table = NULL;
	RETCODE rc;
	int argc;

	argc = ZEND_NUM_ARGS();
	if (argc == 4) {
		if (zend_get_parameters_ex(4, &pv_conn, &pv_cat, &pv_schema, &pv_table) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_string_ex(pv_cat);
		cat = Z_STRVAL_PP(pv_cat);
		convert_to_string_ex(pv_schema);
		schema = Z_STRVAL_PP(pv_schema);
		convert_to_string_ex(pv_table);
		table = Z_STRVAL_PP(pv_table);
	} else {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE2(conn, odbc_connection *, pv_conn, -1, "ODBC-Link", le_conn, le_pconn);

	result = (odbc_result *)emalloc(sizeof(odbc_result));
	if (result == NULL) {
		php_error(E_WARNING, "Out of memory");
		RETURN_FALSE;
	}
	
	rc = SQLAllocStmt(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		efree(result);
		php_error_docref("odbc-tableprivileges" TSRMLS_CC, E_WARNING, "%s(): SQLAllocStmt error 'Invalid Handle'");
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
		if (!odbc_bindcols(result TSRMLS_CC)) {
			efree(result);
			RETURN_FALSE;
		}
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;
	ZEND_REGISTER_RESOURCE(return_value, result, le_result);
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
