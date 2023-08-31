/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stig Sæther Bakken <ssb@php.net>                            |
   |          Andreas Karajannis <Andreas.Karajannis@gmd.de>              |
   |          Frank M. Kromann <frank@kromann.info>  Support for DB/2 CLI |
   |          Kevin N. Shallow <kshallow@tampabay.rr.com>                 |
   |          Daniel R. Kalowsky <kalowsky@php.net>                       |
   +----------------------------------------------------------------------+
*/

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
#include "odbc_arginfo.h"

#ifdef HAVE_UODBC

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

PHP_ODBC_API ZEND_DECLARE_MODULE_GLOBALS(odbc)
static PHP_GINIT_FUNCTION(odbc);

/* {{{ odbc_module_entry */
zend_module_entry odbc_module_entry = {
	STANDARD_MODULE_HEADER,
	"odbc",
	ext_functions,
	PHP_MINIT(odbc),
	PHP_MSHUTDOWN(odbc),
	PHP_RINIT(odbc),
	PHP_RSHUTDOWN(odbc),
	PHP_MINFO(odbc),
	PHP_ODBC_VERSION,
	PHP_MODULE_GLOBALS(odbc),
	PHP_GINIT(odbc),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_ODBC
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(odbc)
#endif

/* {{{ _free_odbc_result */
static void _free_odbc_result(zend_resource *rsrc)
{
	odbc_result *res = (odbc_result *)rsrc->ptr;
	int i;

	if (res) {
		if (res->values) {
			for(i = 0; i < res->numcols; i++) {
				if (res->values[i].value)
					efree(res->values[i].value);
			}
			efree(res->values);
			res->values = NULL;
		}
		/* If aborted via timer expiration, don't try to call any unixODBC function */
		if (res->stmt && !(PG(connection_status) & PHP_CONNECTION_TIMEOUT)) {
#if defined(HAVE_SOLID) || defined(HAVE_SOLID_30) || defined(HAVE_SOLID_35)
			SQLTransact(res->conn_ptr->henv, res->conn_ptr->hdbc,
						(SQLUSMALLINT) SQL_COMMIT);
#endif
			SQLFreeStmt(res->stmt,SQL_DROP);
			/* We don't want the connection to be closed after the last statement has been closed
			 * Connections will be closed on shutdown
			 * zend_list_delete(res->conn_ptr->id);
			 */
		}
		if (res->param_info) {
			efree(res->param_info);
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

/* {{{ _close_odbc_conn */
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

	/* If aborted via timer expiration, don't try to call any unixODBC function */
	if (!(PG(connection_status) & PHP_CONNECTION_TIMEOUT)) {
		safe_odbc_disconnect(conn->hdbc);
		SQLFreeConnect(conn->hdbc);
		SQLFreeEnv(conn->henv);
	}
	efree(conn);
	ODBCG(num_links)--;
}
/* }}} */

/* {{{ void _close_odbc_pconn */
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

	/* If aborted via timer expiration, don't try to call any unixODBC function */
	if (!(PG(connection_status) & PHP_CONNECTION_TIMEOUT)) {
		safe_odbc_disconnect(conn->hdbc);
		SQLFreeConnect(conn->hdbc);
		SQLFreeEnv(conn->henv);
	}
	free(conn);

	ODBCG(num_links)--;
	ODBCG(num_persistent)--;
}
/* }}} */

/* {{{ PHP_INI_DISP(display_link_nums) */
static PHP_INI_DISP(display_link_nums)
{
	char *value;

	if (type == PHP_INI_DISPLAY_ORIG && ini_entry->modified) {
		value = ZSTR_VAL(ini_entry->orig_value);
	} else if (ini_entry->value) {
		value = ZSTR_VAL(ini_entry->value);
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

/* {{{ PHP_INI_DISP(display_defPW) */
static PHP_INI_DISP(display_defPW)
{
	char *value;

	if (type == PHP_INI_DISPLAY_ORIG && ini_entry->modified) {
		value = ZSTR_VAL(ini_entry->orig_value);
	} else if (ini_entry->value) {
		value = ZSTR_VAL(ini_entry->value);
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

/* {{{ PHP_INI_DISP(display_binmode) */
static PHP_INI_DISP(display_binmode)
{
	char *value;

	if (type == PHP_INI_DISPLAY_ORIG && ini_entry->modified) {
		value = ZSTR_VAL(ini_entry->orig_value);
	} else if (ini_entry->value) {
		value = ZSTR_VAL(ini_entry->value);
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

/* {{{ PHP_INI_DISP(display_lrl) */
static PHP_INI_DISP(display_lrl)
{
	char *value;

	if (type == PHP_INI_DISPLAY_ORIG && ini_entry->modified) {
		value = ZSTR_VAL(ini_entry->orig_value);
	} else if (ini_entry->value) {
		value = ZSTR_VAL(ini_entry->value);
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


/* {{{ PHP_INI_DISP(display_cursortype) */
static PHP_INI_DISP(display_cursortype)
{
	char *value;

	if (type == PHP_INI_DISPLAY_ORIG && ini_entry->modified) {
		value = ZSTR_VAL(ini_entry->orig_value);
	} else if (ini_entry->value) {
		value = ZSTR_VAL(ini_entry->value);
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

/* {{{ PHP_INI_BEGIN */
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
#if defined(COMPILE_DL_ODBC) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
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
	snprintf(buf, sizeof(buf), "0x%.4x", ODBCVER);
	php_info_print_table_row(2, "ODBCVER", buf);
#endif
#ifndef PHP_WIN32
	php_info_print_table_row(2, "ODBC_CFLAGS", PHP_ODBC_CFLAGS);
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
	SQLINTEGER	error;        /* Not used */
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
	rc = SQLError(henv, conn, stmt, (SQLCHAR *) ODBCG(laststate), &error, (SQLCHAR *) ODBCG(lasterrormsg), sizeof(ODBCG(lasterrormsg))-1, &errormsgsize);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		snprintf(ODBCG(laststate), sizeof(ODBCG(laststate)), "HY000");
		snprintf(ODBCG(lasterrormsg), sizeof(ODBCG(lasterrormsg)), "Failed to fetch error message");
	}
	if (conn_resource) {
		memcpy(conn_resource->laststate, ODBCG(laststate), sizeof(ODBCG(laststate)));
		memcpy(conn_resource->lasterrormsg, ODBCG(lasterrormsg), sizeof(ODBCG(lasterrormsg)));
	}
	if (func) {
		php_error_docref(NULL, E_WARNING, "SQL error: %s, SQL state %s in %s", ODBCG(lasterrormsg), ODBCG(laststate), func);
	} else {
		php_error_docref(NULL, E_WARNING, "SQL error: %s, SQL state %s", ODBCG(lasterrormsg), ODBCG(laststate));
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
		RETURN_THROWS();
	}

	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_THROWS();
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

		rc = PHP_ODBC_SQLCOLATTRIBUTE(result->stmt, (SQLUSMALLINT)(i+1), PHP_ODBC_SQL_DESC_NAME,
				result->values[i].name, sizeof(result->values[i].name), &colnamelen, 0);
		result->values[i].coltype = 0;
		rc = PHP_ODBC_SQLCOLATTRIBUTE(result->stmt, (SQLUSMALLINT)(i+1), SQL_COLUMN_TYPE,
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
				/* TODO: Check this is the intended behaviour */
				ZEND_FALLTHROUGH;
			default:
				rc = PHP_ODBC_SQLCOLATTRIBUTE(result->stmt, (SQLUSMALLINT)(i+1), colfieldid,
								NULL, 0, NULL, &displaysize);
				if (rc != SQL_SUCCESS) {
					displaysize = 0;
				}
#if defined(ODBCVER) && (ODBCVER >= 0x0300)
				if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO && colfieldid == SQL_DESC_OCTET_LENGTH) {
					SQLINTEGER err;
					SQLCHAR errtxt[128];
					SQLCHAR state[6];

					memset(errtxt, '\0', 128);
					memset(state, '\0', 6);

					if (SQL_SUCCESS == SQLGetDiagRec(SQL_HANDLE_STMT, result->stmt, 1, state, &err, errtxt, 128, NULL)) {
						errtxt[127] = '\0';
						state[5] = '\0';
						php_error_docref(NULL, E_WARNING, "SQLColAttribute can't handle SQL_DESC_OCTET_LENGTH: [%s] %s", state, errtxt);
					}
					 /* This is  a quirk for ODBC 2.0 compatibility for broken driver implementations.
					  */
					charextraalloc = 1;
					rc = SQLColAttributes(result->stmt, (SQLUSMALLINT)(i+1), SQL_COLUMN_DISPLAY_SIZE,
								NULL, 0, NULL, &displaysize);
					if (rc != SQL_SUCCESS) {
						displaysize = 0;
					}
				}

				/* Workaround for drivers that report NVARCHAR(MAX) columns as SQL_WVARCHAR with size 0 (bug #69975) */
				if (result->values[i].coltype == SQL_WVARCHAR && displaysize == 0) {
					result->values[i].coltype = SQL_WLONGVARCHAR;
					result->values[i].value = NULL;
					break;
				}
#endif
				/* Workaround for drivers that report VARCHAR(MAX) columns as SQL_VARCHAR (bug #73725) */
				if (SQL_VARCHAR == result->values[i].coltype && displaysize == 0) {
					result->values[i].coltype = SQL_LONGVARCHAR;
					result->values[i].value = NULL;
					break;
				}

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
		RETURN_THROWS();
	}

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_THROWS();
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
static int _close_pconn_with_res(zval *zv, void *p)
{
	zend_resource *le = Z_RES_P(zv);
	zend_resource *res = (zend_resource*)p;
	if (le->type == le_pconn && (((odbc_connection *)(le->ptr))->res == res)) {
		return ZEND_HASH_APPLY_REMOVE;
	} else{
		return ZEND_HASH_APPLY_KEEP;
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
		RETURN_THROWS();
	}

	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_THROWS();
	}

	if (pv_num < 1) {
		zend_argument_value_error(2, "must be greater than 0");
		RETURN_THROWS();
	}

	if (result->numcols == 0) {
		php_error_docref(NULL, E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}

	if (pv_num > result->numcols) {
		php_error_docref(NULL, E_WARNING, "Field index larger than number of fields");
		RETURN_FALSE;
	}

	PHP_ODBC_SQLCOLATTRIBUTE(result->stmt, (SQLUSMALLINT)pv_num, (SQLUSMALLINT) (type?SQL_COLUMN_SCALE:SQL_COLUMN_PRECISION), NULL, 0, NULL, &len);

	RETURN_LONG(len);
}
/* }}} */

/* Main User Functions */

/* {{{ Close all ODBC connections */
PHP_FUNCTION(odbc_close_all)
{
	zend_resource *p;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
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
					_close_pconn_with_res, (void *)p);
			}
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ Handle binary column data */
PHP_FUNCTION(odbc_binmode)
{
	php_odbc_fetch_attribs(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ Handle LONG columns */
PHP_FUNCTION(odbc_longreadlen)
{
	php_odbc_fetch_attribs(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ Prepares a statement for execution */
PHP_FUNCTION(odbc_prepare)
{
	zval *pv_conn;
	char *query;
	size_t query_len;
	odbc_result *result = NULL;
	odbc_connection *conn;
	RETCODE rc;
	int i;
#ifdef HAVE_SQL_EXTENDED_FETCH
	SQLUINTEGER      scrollopts;
#endif

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs", &pv_conn, &query, &query_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_THROWS();
	}

	result = (odbc_result *)ecalloc(1, sizeof(odbc_result));

	result->numparams = 0;
	result->param_info = NULL;

	rc = PHP_ODBC_SQLALLOCSTMT(conn->hdbc, &(result->stmt));
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

	rc = SQLPrepare(result->stmt, (SQLCHAR *) query, SQL_NTS);
	switch (rc) {
		case SQL_SUCCESS:
			break;
		case SQL_SUCCESS_WITH_INFO:
			odbc_sql_error(conn, result->stmt, "SQLPrepare");
			break;
		default:
			odbc_sql_error(conn, result->stmt, "SQLPrepare");
			efree(result);
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

	result->param_info = (odbc_param_info *) safe_emalloc(sizeof(odbc_param_info), result->numparams, 0);
	for (i=0;i<result->numparams;i++) {
		rc = SQLDescribeParam(result->stmt, (SQLUSMALLINT)(i+1), &result->param_info[i].sqltype, &result->param_info[i].precision,
													&result->param_info[i].scale, &result->param_info[i].nullable);
		if (rc == SQL_ERROR) {
			odbc_sql_error(result->conn_ptr, result->stmt, "SQLDescribeParameter");
			SQLFreeStmt(result->stmt, SQL_RESET_PARAMS);
			efree(result->param_info);
			efree(result);
			RETURN_FALSE;
		}
	}

	RETURN_RES(zend_register_resource(result, le_result));
}
/* }}} */

/*
 * Execute prepared SQL statement. Supports only input parameters.
 */

typedef struct odbc_params_t {
	SQLLEN vallen;
	int fp;
	zend_string *zstr;
} odbc_params_t;

static void odbc_release_params(odbc_result *result, odbc_params_t *params) {
	SQLFreeStmt(result->stmt, SQL_RESET_PARAMS);
	for (int i = 0; i < result->numparams; i++) {
		if (params[i].fp != -1) {
			close(params[i].fp);
		}
		if (params[i].zstr) {
			zend_string_release(params[i].zstr);
		}
	}
	efree(params);
}

/* {{{ Execute a prepared statement */
PHP_FUNCTION(odbc_execute)
{
	zval *pv_res, *tmp;
	HashTable *pv_param_ht = (HashTable *) &zend_empty_array;
	odbc_params_t *params = NULL;
	char *filename;
	SQLSMALLINT ctype;
	odbc_result *result;
	int i, ne;
	RETCODE rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|h", &pv_res, &pv_param_ht) == FAILURE) {
		RETURN_THROWS();
	}

	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_THROWS();
	}

	if (result->numparams > 0) {
		if ((ne = zend_hash_num_elements(pv_param_ht)) < result->numparams) {
			php_error_docref(NULL, E_WARNING, "Not enough parameters (%d should be %d) given", ne, result->numparams);
			RETURN_FALSE;
		}

		params = (odbc_params_t *)safe_emalloc(sizeof(odbc_params_t), result->numparams, 0);
		for(i = 0; i < result->numparams; i++) {
			params[i].fp = -1;
			params[i].zstr = NULL;
		}

		i = 1;
		ZEND_HASH_FOREACH_VAL(pv_param_ht, tmp) {
			unsigned char otype = Z_TYPE_P(tmp);
			zend_string *tmpstr = zval_try_get_string(tmp);
			if (!tmpstr) {
				odbc_release_params(result, params);
				RETURN_THROWS();
			}

			params[i-1].vallen = ZSTR_LEN(tmpstr);
			params[i-1].fp = -1;
			params[i-1].zstr = tmpstr;

			if (IS_SQL_BINARY(result->param_info[i-1].sqltype)) {
				ctype = SQL_C_BINARY;
			} else {
				ctype = SQL_C_CHAR;
			}

			if (ZSTR_LEN(tmpstr) > 2 &&
				ZSTR_VAL(tmpstr)[0] == '\'' &&
				ZSTR_VAL(tmpstr)[ZSTR_LEN(tmpstr) - 1] == '\'') {

				if (ZSTR_LEN(tmpstr) != strlen(ZSTR_VAL(tmpstr))) {
					odbc_release_params(result, params);
					RETURN_FALSE;
				}
				filename = estrndup(&ZSTR_VAL(tmpstr)[1], ZSTR_LEN(tmpstr) - 2);
				filename[strlen(filename)] = '\0';

				/* Check the basedir */
				if (php_check_open_basedir(filename)) {
					efree(filename);
					odbc_release_params(result, params);
					RETURN_FALSE;
				}

				if ((params[i-1].fp = open(filename,O_RDONLY)) == -1) {
					php_error_docref(NULL, E_WARNING,"Can't open file %s", filename);
					odbc_release_params(result, params);
					efree(filename);
					RETURN_FALSE;
				}

				efree(filename);

				params[i-1].vallen = SQL_LEN_DATA_AT_EXEC(0);

				rc = SQLBindParameter(result->stmt, (SQLUSMALLINT)i, SQL_PARAM_INPUT,
									  ctype, result->param_info[i-1].sqltype, result->param_info[i-1].precision, result->param_info[i-1].scale,
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
									  ctype, result->param_info[i-1].sqltype, result->param_info[i-1].precision, result->param_info[i-1].scale,
									  ZSTR_VAL(tmpstr), 0,
									  &params[i-1].vallen);
			}
			if (rc == SQL_ERROR) {
				odbc_sql_error(result->conn_ptr, result->stmt, "SQLBindParameter");
				odbc_release_params(result, params);
				RETURN_FALSE;
			}
			if (++i > result->numparams) break;
		} ZEND_HASH_FOREACH_END();
	}
	/* Close cursor, needed for doing multiple selects */
	rc = SQLFreeStmt(result->stmt, SQL_CLOSE);

	if (rc == SQL_ERROR) {
		odbc_sql_error(result->conn_ptr, result->stmt, "SQLFreeStmt");
	}

	result->fetched = 0;
	rc = SQLExecute(result->stmt);
	switch (rc) {
		case SQL_NEED_DATA: {
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
			break;
		}
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

	if (result->numparams > 0) {
		odbc_release_params(result, params);
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

/* {{{ Get cursor name */
PHP_FUNCTION(odbc_cursor)
{
	zval *pv_res;
	SQLUSMALLINT max_len;
	SQLSMALLINT len;
	char *cursorname;
	odbc_result *result;
	RETCODE rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pv_res) == FAILURE) {
		RETURN_THROWS();
	}

	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_THROWS();
	}

	rc = SQLGetInfo(result->conn_ptr->hdbc,SQL_MAX_CURSOR_NAME_LEN, (void *)&max_len,sizeof(max_len),&len);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		RETURN_FALSE;
	}

	if (max_len > 0) {
		cursorname = emalloc(max_len + 1);
		rc = SQLGetCursorName(result->stmt, (SQLCHAR *) cursorname, (SQLSMALLINT)max_len, &len);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			char        state[6];     /* Not used */
	 		SQLINTEGER  error;        /* Not used */
			char        errormsg[SQL_MAX_MESSAGE_LENGTH];
			SQLSMALLINT errormsgsize; /* Not used */

			SQLError( result->conn_ptr->henv, result->conn_ptr->hdbc,
						result->stmt, (SQLCHAR *) state, &error, (SQLCHAR *) errormsg,
						sizeof(errormsg)-1, &errormsgsize);
			if (!strncmp(state,"S1015",5)) {
				snprintf(cursorname, max_len+1, "php_curs_" ZEND_ULONG_FMT, (zend_ulong)result->stmt);
				if (SQLSetCursorName(result->stmt, (SQLCHAR *) cursorname, SQL_NTS) != SQL_SUCCESS) {
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
/* {{{ Return information about the currently connected data source */
PHP_FUNCTION(odbc_data_source)
{
	zval *zv_conn;
	zend_long zv_fetch_type;
	RETCODE rc = 0; /* assume all is good */
	odbc_connection *conn;
	UCHAR server_name[100], desc[200];
	SQLSMALLINT len1=0, len2=0, fetch_type;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &zv_conn, &zv_fetch_type) == FAILURE) {
		RETURN_THROWS();
	}

	fetch_type = (SQLSMALLINT) zv_fetch_type;

	if (!(fetch_type == SQL_FETCH_FIRST || fetch_type == SQL_FETCH_NEXT)) {
		zend_argument_value_error(2, "must be either SQL_FETCH_FIRST or SQL_FETCH_NEXT");
		RETURN_THROWS();
	}

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(zv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_THROWS();
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

	if (SQL_NO_DATA == rc) {
		/* System has no data sources, no error. Signal it by returning NULL,
			not false. */
		RETURN_NULL();
	} else if (rc != SQL_SUCCESS) {
		/* ummm.... he did it */
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLDataSources");
		RETURN_FALSE;
	}

	if (len1 == 0 || len2 == 0) {
		/* we have a non-valid entry... so stop the looping */
		RETURN_FALSE;
	}

	array_init(return_value);

	add_assoc_string_ex(return_value, "server", sizeof("server")-1, (char *) server_name);
	add_assoc_string_ex(return_value, "description", sizeof("description")-1, (char *) desc);

}
/* }}} */
#endif /* HAVE_SQLDATASOURCES */

/* {{{ Prepare and execute an SQL statement */
/* XXX Use flags */
PHP_FUNCTION(odbc_exec)
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
		RETURN_THROWS();
	}

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_THROWS();
	}

	result = (odbc_result *)ecalloc(1, sizeof(odbc_result));

	rc = PHP_ODBC_SQLALLOCSTMT(conn->hdbc, &(result->stmt));
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

	rc = SQLExecDirect(result->stmt, (SQLCHAR *) query, SQL_NTS);
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
		RETURN_THROWS();
	}

	rownum = pv_row;
#else
	zval *pv_res, tmp;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pv_res) == FAILURE) {
		RETURN_THROWS();
	}
#endif

	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_THROWS();
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
				ZEND_FALLTHROUGH;
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
				} else if (rc != SQL_SUCCESS) {
					php_error_docref(NULL, E_WARNING, "Cannot get data of column #%d (retcode %u)", i + 1, rc);
					ZVAL_FALSE(&tmp);
				} else if (result->values[i].vallen == SQL_NULL_DATA) {
					ZVAL_NULL(&tmp);
					break;
				} else if (result->values[i].vallen == SQL_NO_TOTAL) {
					php_error_docref(NULL, E_WARNING, "Cannot get data of column #%d (driver cannot determine length)", i + 1);
					ZVAL_FALSE(&tmp);
				} else {
					ZVAL_STRINGL(&tmp, buf, result->values[i].vallen);
				}
				break;

			default:
				if (result->values[i].vallen == SQL_NULL_DATA) {
					ZVAL_NULL(&tmp);
					break;
				} else if (result->values[i].vallen == SQL_NO_TOTAL) {
					php_error_docref(NULL, E_WARNING, "Cannot get data of column #%d (driver cannot determine length)", i + 1);
					ZVAL_FALSE(&tmp);
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


/* {{{ Fetch a result row as an object */
PHP_FUNCTION(odbc_fetch_object)
{
	php_odbc_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, ODBC_OBJECT);
	if (Z_TYPE_P(return_value) == IS_ARRAY) {
		object_and_properties_init(return_value, ZEND_STANDARD_CLASS_DEF_PTR, Z_ARRVAL_P(return_value));
	}
}
/* }}} */

/* {{{ Fetch a result row as an associative array */
PHP_FUNCTION(odbc_fetch_array)
{
	php_odbc_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, ODBC_OBJECT);
}
/* }}} */
#endif

/* {{{ Fetch one result row into an array */
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
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rz|l", &pv_res, &pv_res_arr, &pv_row) == FAILURE) {
		RETURN_THROWS();
	}

	rownum = pv_row;
#else
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rz", &pv_res, &pv_res_arr) == FAILURE) {
		RETURN_THROWS();
	}
#endif /* HAVE_SQL_EXTENDED_FETCH */

	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_THROWS();
	}

	if (result->numcols == 0) {
		php_error_docref(NULL, E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}

	pv_res_arr = zend_try_array_init(pv_res_arr);
	if (!pv_res_arr) {
		RETURN_THROWS();
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

				/* TODO: Check this is the intended behaviour */
				ZEND_FALLTHROUGH;
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
				} else if (rc != SQL_SUCCESS) {
					php_error_docref(NULL, E_WARNING, "Cannot get data of column #%d (retcode %u)", i + 1, rc);
					ZVAL_FALSE(&tmp);
				} else if (result->values[i].vallen == SQL_NULL_DATA) {
					ZVAL_NULL(&tmp);
					break;
				} else if (result->values[i].vallen == SQL_NO_TOTAL) {
					php_error_docref(NULL, E_WARNING, "Cannot get data of column #%d (driver cannot determine length)", i + 1);
					ZVAL_FALSE(&tmp);
				} else {
					ZVAL_STRINGL(&tmp, buf, result->values[i].vallen);
				}
				break;

			default:
				if (result->values[i].vallen == SQL_NULL_DATA) {
					ZVAL_NULL(&tmp);
					break;
				} else if (result->values[i].vallen == SQL_NO_TOTAL) {
					php_error_docref(NULL, E_WARNING, "Cannot get data of column #%d (driver cannot determine length)", i + 1);
					ZVAL_FALSE(&tmp);
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

/* {{{ */
#if defined(HAVE_SOLID) || defined(HAVE_SOLID_30) || defined(HAVE_SOLID_35)
PHP_FUNCTION(solid_fetch_prev)
{
	odbc_result *result;
	RETCODE rc;
	zval *pv_res;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pv_res) == FAILURE) {
		RETURN_THROWS();
	}

	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_THROWS();
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

/* {{{ Fetch a row */
PHP_FUNCTION(odbc_fetch_row)
{
	odbc_result *result;
	RETCODE rc;
	zval *pv_res;
	zend_long pv_row;
	bool pv_row_is_null = 1;
#ifdef HAVE_SQL_EXTENDED_FETCH
	SQLULEN crow;
	SQLUSMALLINT RowStatus[1];
#endif

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|l!", &pv_res, &pv_row, &pv_row_is_null) == FAILURE) {
		RETURN_THROWS();
	}

	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_THROWS();
	}

	if (result->numcols == 0) {
		php_error_docref(NULL, E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}

#ifdef HAVE_SQL_EXTENDED_FETCH
	if (result->fetch_abs) {
		if (!pv_row_is_null) {
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_ABSOLUTE,(SQLLEN)pv_row,&crow,RowStatus);
		} else {
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
		}
	} else
#endif
		rc = SQLFetch(result->stmt);

	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		RETURN_FALSE;
	}

	if (!pv_row_is_null) {
		result->fetched = (SQLLEN)pv_row;
	} else {
		result->fetched++;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Get result data */
PHP_FUNCTION(odbc_result)
{
	char *field;
	zend_string *field_str, *pv_field_str;
	zend_long pv_field_long;
	int field_ind;
	SQLSMALLINT sql_c_type = SQL_C_CHAR;
	odbc_result *result;
	int i = 0;
	RETCODE rc;
	SQLLEN	fieldsize;
	zval *pv_res;
#ifdef HAVE_SQL_EXTENDED_FETCH
	SQLULEN crow;
	SQLUSMALLINT RowStatus[1];
#endif

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_RESOURCE(pv_res)
		Z_PARAM_STR_OR_LONG(pv_field_str, pv_field_long)
	ZEND_PARSE_PARAMETERS_END();

	if (pv_field_str) {
		field = ZSTR_VAL(pv_field_str);
		field_ind = -1;
	} else {
		field = NULL;
		field_ind = (int) pv_field_long - 1;
	}

	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_THROWS();
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
			/* TODO: Check this is the intended behaviour */
			ZEND_FALLTHROUGH;

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
			   PHP_ODBC_SQLCOLATTRIBUTE(result->stmt, (SQLUSMALLINT)(field_ind + 1),
					   			(SQLUSMALLINT)((sql_c_type == SQL_C_BINARY) ? SQL_COLUMN_LENGTH :
					   			SQL_COLUMN_DISPLAY_SIZE),
					   			NULL, 0, NULL, &fieldsize);
			}
			/* For char data, the length of the returned string will be longreadlen - 1 */
			fieldsize = (result->longreadlen <= 0) ? 4096 : result->longreadlen;
			field_str = zend_string_alloc(fieldsize, 0);

		/* SQLGetData will truncate CHAR data to fieldsize - 1 bytes and append \0.
		 * For binary data it is truncated to fieldsize bytes.
		 */
			rc = SQLGetData(result->stmt, (SQLUSMALLINT)(field_ind + 1), sql_c_type,
							ZSTR_VAL(field_str), fieldsize, &result->values[field_ind].vallen);

			if (rc == SQL_ERROR) {
				odbc_sql_error(result->conn_ptr, result->stmt, "SQLGetData");
				zend_string_efree(field_str);
				RETURN_FALSE;
			}

			if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
				zend_string_efree(field_str);
				php_error_docref(NULL, E_WARNING, "Cannot get data of column #%d (retcode %u)", field_ind + 1, rc);
				RETURN_FALSE;
			} else if (result->values[field_ind].vallen == SQL_NULL_DATA) {
				zend_string_efree(field_str);
				RETURN_NULL();
			} else if (result->values[field_ind].vallen == SQL_NO_TOTAL) {
				zend_string_efree(field_str);
				php_error_docref(NULL, E_WARNING, "Cannot get data of column #%d (driver cannot determine length)", field_ind + 1);
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
			if (rc != SQL_SUCCESS_WITH_INFO) {
				field_str = zend_string_truncate(field_str, result->values[field_ind].vallen, 0);
			}
			ZSTR_VAL(field_str)[ZSTR_LEN(field_str)] = '\0';
			RETURN_NEW_STR(field_str);
			break;

		default:
			if (result->values[field_ind].vallen == SQL_NULL_DATA) {
				RETURN_NULL();
			} else if (result->values[field_ind].vallen == SQL_NO_TOTAL) {
				php_error_docref(NULL, E_WARNING, "Cannot get data of column #%d (driver cannot determine length)", field_ind + 1);
				RETURN_FALSE;
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

		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			php_error_docref(NULL, E_WARNING, "Cannot get data of column #%d (retcode %u)", field_ind + 1, rc);
			efree(field);
			RETURN_FALSE;
		}

		if (result->values[field_ind].vallen == SQL_NULL_DATA) {
			efree(field);
			RETURN_NULL();
		} else if (result->values[field_ind].vallen == SQL_NO_TOTAL) {
			php_error_docref(NULL, E_WARNING, "Cannot get data of column #%d (driver cannot determine length)", field_ind + 1);
			efree(field);
			RETURN_FALSE;
		}
		/* chop the trailing \0 by outputting only 4095 bytes */
		PHPWRITE(field,(rc == SQL_SUCCESS_WITH_INFO) ? 4095 : result->values[field_ind].vallen);

		if (rc == SQL_SUCCESS) { /* no more data avail */
			efree(field);
			RETURN_TRUE;
		}
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Print result as HTML table */
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
		RETURN_THROWS();
	}

	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_THROWS();
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

					/* TODO: Check this is the intended behaviour */
					ZEND_FALLTHROUGH;
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
						if (result->values[i].vallen == SQL_NO_TOTAL) {
							php_printf("</td></tr></table>");
							php_error_docref(NULL, E_WARNING, "Cannot get data of column #%zu (driver cannot determine length)", i + 1);
							efree(buf);
							RETURN_FALSE;
						} else {
							PHPWRITE(buf, result->longreadlen);
						}
					} else if (rc != SQL_SUCCESS) {
						php_printf("</td></tr></table>");
						php_error_docref(NULL, E_WARNING, "Cannot get data of column #%zu (retcode %u)", i + 1, rc);
						efree(buf);
						RETURN_FALSE;
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
					} else if (result->values[i].vallen == SQL_NO_TOTAL) {
						php_error_docref(NULL, E_WARNING, "Cannot get data of column #%zu (driver cannot determine length)", i + 1);
						php_printf("<td>FALSE</td>");
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

/* {{{ Free resources associated with a result */
PHP_FUNCTION(odbc_free_result)
{
	zval *pv_res;
	odbc_result *result;
	int i;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pv_res) == FAILURE) {
		RETURN_THROWS();
	}

	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_THROWS();
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

/* {{{ Connect to a datasource */
PHP_FUNCTION(odbc_connect)
{
	odbc_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ Establish a persistent connection to a datasource */
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
	memset(*conn, 0, sizeof(odbc_connection));
	(*conn)->persistent = persistent;
	SQLAllocEnv(&((*conn)->henv));
	SQLAllocConnect((*conn)->henv, &((*conn)->hdbc));

#if defined(HAVE_SOLID) || defined(HAVE_SOLID_30)
	SQLSetConnectOption((*conn)->hdbc, SQL_TRANSLATE_OPTION,
			SQL_SOLID_XLATOPT_NOCNV);
#endif
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
		SQLCHAR dsnbuf[1024];
		short   dsnbuflen;
		char    *ldb = 0;
		int		ldb_len = 0;

		/* a connection string may have = but not ; - i.e. "DSN=PHP" */
		if (strstr((char*)db, "=")) {
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
			rc = SQLDriverConnect((*conn)->hdbc, NULL, (SQLCHAR *) ldb, strlen(ldb), dsnbuf, sizeof(dsnbuf) - 1, &dsnbuflen, SQL_DRIVER_NOPROMPT);
		} else {
			rc = SQLConnect((*conn)->hdbc, (SQLCHAR *) db, SQL_NTS, (SQLCHAR *) uid, SQL_NTS, (SQLCHAR *) pwd, SQL_NTS);
		}

		if (ldb) {
			efree(ldb);
		}
	}
#else
	rc = SQLConnect((*conn)->hdbc, (SQLCHAR *) db, SQL_NTS, uid, SQL_NTS, pwd, SQL_NTS);
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
 * connections get hashed up.
 */
/* {{{ odbc_do_connect */
void odbc_do_connect(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	char *db, *uid, *pwd;
	size_t db_len, uid_len, pwd_len;
	zend_long pv_opt = SQL_CUR_DEFAULT;
	odbc_connection *db_conn;
	int cur_opt;

	/*  Now an optional 4th parameter specifying the cursor type
	 *  defaulting to the cursors default
	 */
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sss|l", &db, &db_len, &uid, &uid_len, &pwd, &pwd_len, &pv_opt) == FAILURE) {
		RETURN_THROWS();
	}

	cur_opt = pv_opt;

	if (ZEND_NUM_ARGS() > 3) {
		/* Confirm the cur_opt range */
		if (! (cur_opt == SQL_CUR_USE_IF_NEEDED ||
			cur_opt == SQL_CUR_USE_ODBC ||
			cur_opt == SQL_CUR_USE_DRIVER ||
			cur_opt == SQL_CUR_DEFAULT) ) {
			zend_argument_value_error(4, "must be one of SQL_CUR_USE_IF_NEEDED, "
				"SQL_CUR_USE_ODBC, or SQL_CUR_USE_DRIVER");
			RETURN_THROWS();
		}
	}

	if (ODBCG(allow_persistent) <= 0) {
		persistent = 0;
	}

try_and_get_another_connection:

	if (persistent) {
		char *hashed_details;
		int hashed_len;
		zend_resource *le;

		hashed_len = spprintf(&hashed_details, 0, "%s_%s_%s_%s_%d", ODBC_TYPE, db, uid, pwd, cur_opt);

		/* the link is not in the persistent list */
		if ((le = zend_hash_str_find_ptr(&EG(persistent_list), hashed_details, hashed_len)) == NULL) {
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

			if (zend_register_persistent_resource(hashed_details, hashed_len, db_conn, le_pconn) == NULL) {
				free(db_conn);
				efree(hashed_details);
				RETURN_FALSE;
			}
			ODBCG(num_persistent)++;
			ODBCG(num_links)++;
			db_conn->res = zend_register_resource(db_conn, le_pconn);
			RETVAL_RES(db_conn->res);
		} else { /* found connection */
			ZEND_ASSERT(le->type == le_pconn);

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
		efree(hashed_details);
		db_conn->res = zend_register_resource(db_conn, le_pconn);
		RETVAL_RES(db_conn->res);
	} else { /* non persistent */
		if (ODBCG(max_links) != -1 && ODBCG(num_links) >= ODBCG(max_links)) {
			php_error_docref(NULL, E_WARNING,"Too many open connections (%ld)",ODBCG(num_links));
			RETURN_FALSE;
		}

		if (!odbc_sqlconnect(&db_conn, db, uid, pwd, cur_opt, 0)) {
			RETURN_FALSE;
		}
		db_conn->res = zend_register_resource(db_conn, le_conn);
		RETVAL_RES(db_conn->res);
		ODBCG(num_links)++;
	}
}
/* }}} */

/* {{{ Close an ODBC connection */
PHP_FUNCTION(odbc_close)
{
	zval *pv_conn;
	zend_resource *p;
	odbc_connection *conn;
	odbc_result *res;
	int is_pconn = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pv_conn) == FAILURE) {
		RETURN_THROWS();
	}

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_THROWS();
	}

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
		zend_hash_apply_with_argument(&EG(persistent_list), _close_pconn_with_res, (void *) Z_RES_P(pv_conn));
	}
}
/* }}} */

/* {{{ Get number of rows in a result */
PHP_FUNCTION(odbc_num_rows)
{
	odbc_result *result;
	SQLLEN rows;
	zval *pv_res;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pv_res) == FAILURE) {
		RETURN_THROWS();
	}

	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_THROWS();
	}

	SQLRowCount(result->stmt, &rows);
	RETURN_LONG(rows);
}
/* }}} */

#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30)
/* {{{ Checks if multiple results are available */
PHP_FUNCTION(odbc_next_result)
{
	odbc_result *result;
	zval *pv_res;
	int rc, i;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pv_res) == FAILURE) {
		RETURN_THROWS();
	}

	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_THROWS();
	}

	if (result->values) {
		for(i = 0; i < result->numcols; i++) {
			if (result->values[i].value) {
				efree(result->values[i].value);
			}
		}
		efree(result->values);
		result->values = NULL;
		result->numcols = 0;
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

/* {{{ Get number of columns in a result */
PHP_FUNCTION(odbc_num_fields)
{
	odbc_result *result;
	zval *pv_res;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &pv_res) == FAILURE) {
		RETURN_THROWS();
	}

	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_THROWS();
	}

	RETURN_LONG(result->numcols);
}
/* }}} */

/* {{{ Get a column name */
PHP_FUNCTION(odbc_field_name)
{
	odbc_result *result;
	zval *pv_res;
	zend_long pv_num;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &pv_res, &pv_num) == FAILURE) {
		RETURN_THROWS();
	}

	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_THROWS();
	}

	if (pv_num < 1) {
		zend_argument_value_error(2, "must be greater than 0");
		RETURN_THROWS();
	}

	if (result->numcols == 0) {
		php_error_docref(NULL, E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}

	if (pv_num > result->numcols) {
		php_error_docref(NULL, E_WARNING, "Field index larger than number of fields");
		RETURN_FALSE;
	}

	RETURN_STRING(result->values[pv_num - 1].name);
}
/* }}} */

/* {{{ Get the datatype of a column */
PHP_FUNCTION(odbc_field_type)
{
	odbc_result	*result;
	char    	tmp[32];
	SQLSMALLINT	tmplen;
	zval		*pv_res;
	zend_long		pv_num;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &pv_res, &pv_num) == FAILURE) {
		RETURN_THROWS();
	}

	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_THROWS();
	}

	if (pv_num < 1) {
		zend_argument_value_error(2, "must be greater than 0");
		RETURN_THROWS();
	}

	if (result->numcols == 0) {
		php_error_docref(NULL, E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}

	if (pv_num > result->numcols) {
		php_error_docref(NULL, E_WARNING, "Field index larger than number of fields");
		RETURN_FALSE;
	}

	PHP_ODBC_SQLCOLATTRIBUTE(result->stmt, (SQLUSMALLINT)pv_num, SQL_COLUMN_TYPE_NAME, tmp, 31, &tmplen, NULL);
	RETURN_STRING(tmp);
}
/* }}} */

/* {{{ Get the length (precision) of a column */
PHP_FUNCTION(odbc_field_len)
{
	odbc_column_lengths(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ Get the scale of a column */
PHP_FUNCTION(odbc_field_scale)
{
	odbc_column_lengths(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ Return column number */
PHP_FUNCTION(odbc_field_num)
{
	char *fname;
	size_t i, field_ind, fname_len;
	odbc_result *result;
	zval *pv_res;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs", &pv_res, &fname, &fname_len) == FAILURE) {
		RETURN_THROWS();
	}

	if ((result = (odbc_result *)zend_fetch_resource(Z_RES_P(pv_res), "ODBC result", le_result)) == NULL) {
		RETURN_THROWS();
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

/* {{{ Toggle autocommit mode or get status */
/* There can be problems with pconnections!*/
PHP_FUNCTION(odbc_autocommit)
{
	odbc_connection *conn;
	RETCODE rc;
	zval *pv_conn;
	bool pv_onoff = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|b", &pv_conn, &pv_onoff) == FAILURE) {
		RETURN_THROWS();
	}

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_THROWS();
	}

	if (ZEND_NUM_ARGS() > 1) {
		rc = SQLSetConnectOption(conn->hdbc, SQL_AUTOCOMMIT, pv_onoff ? SQL_AUTOCOMMIT_ON : SQL_AUTOCOMMIT_OFF);
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

/* {{{ Commit an ODBC transaction */
PHP_FUNCTION(odbc_commit)
{
	odbc_transact(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ Rollback a transaction */
PHP_FUNCTION(odbc_rollback)
{
	odbc_transact(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ php_odbc_lasterror */
static void php_odbc_lasterror(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	odbc_connection *conn;
	zval *pv_handle = NULL;
	char *ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|r!", &pv_handle) == FAILURE) {
		RETURN_THROWS();
	}

	if (pv_handle) {
		if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_handle), "ODBC-Link", le_conn, le_pconn))) {
			RETURN_THROWS();
		}
		if (mode == 0) {
			ret = conn->laststate;
		} else {
			ret = conn->lasterrormsg;
		}
	} else {
		if (mode == 0) {
			ret = ODBCG(laststate);
		} else {
			ret = ODBCG(lasterrormsg);
		}
	}

	RETURN_STRING(ret);
}
/* }}} */

/* {{{ Get the last error code */
PHP_FUNCTION(odbc_error)
{
	php_odbc_lasterror(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ Get the last error message */
PHP_FUNCTION(odbc_errormsg)
{
	php_odbc_lasterror(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ Sets connection or statement options */
/* This one has to be used carefully. We can't allow to set connection options for
   persistent connections. I think that SetStmtOption is of little use, since most
   of those can only be specified before preparing/executing statements.
   On the other hand, they can be made connection wide default through SetConnectOption
   - but will be overridden by calls to SetStmtOption() in odbc_prepare/odbc_do
*/
PHP_FUNCTION(odbc_setoption)
{
	odbc_connection *conn;
	odbc_result	*result;
	RETCODE rc;
	zval *pv_handle;
	zend_long pv_which, pv_opt, pv_val;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rlll", &pv_handle, &pv_which, &pv_opt, &pv_val) == FAILURE) {
		RETURN_THROWS();
	}

	switch (pv_which) {
		case 1:		/* SQLSetConnectOption */
			if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_handle), "ODBC-Link", le_conn, le_pconn))) {
				RETURN_THROWS();
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
				RETURN_THROWS();
			}

			rc = SQLSetStmtOption(result->stmt, (unsigned short) pv_opt, pv_val);

			if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
				odbc_sql_error(result->conn_ptr, result->stmt, "SetStmtOption");
				RETURN_FALSE;
			}
			break;
		default:
			zend_argument_value_error(2, "must be 1 for SQLSetConnectOption(), or 2 for SQLSetStmtOption()");
			RETURN_THROWS();
	}

	RETURN_TRUE;
}
/* }}} */

/*
 * metadata functions
 */

/* {{{ Call the SQLTables function */
PHP_FUNCTION(odbc_tables)
{
	zval *pv_conn;
	odbc_result   *result = NULL;
	odbc_connection *conn;
	char *cat = NULL, *schema = NULL, *table = NULL, *type = NULL;
	size_t cat_len = 0, schema_len = 0, table_len = 0, type_len = 0;
	RETCODE rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|s!s!s!s!", &pv_conn, &cat, &cat_len, &schema, &schema_len,
		&table, &table_len, &type, &type_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_THROWS();
	}

	result = (odbc_result *)ecalloc(1, sizeof(odbc_result));

	rc = PHP_ODBC_SQLALLOCSTMT(conn->hdbc, &(result->stmt));
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
	if (schema && schema_len == 0 && table && table_len) {
		schema = NULL;
	}

	rc = SQLTables(result->stmt,
			(SQLCHAR *) cat, SAFE_SQL_NTS(cat),
			(SQLCHAR *) schema,	SAFE_SQL_NTS(schema),
			(SQLCHAR *) table, SAFE_SQL_NTS(table),
			(SQLCHAR *) type, SAFE_SQL_NTS(type));

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, result->stmt, "SQLTables");
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

/* {{{ Returns a result identifier that can be used to fetch a list of column names in specified tables */
PHP_FUNCTION(odbc_columns)
{
	zval *pv_conn;
	odbc_result *result = NULL;
	odbc_connection *conn;
	char *cat = NULL, *schema = NULL, *table = NULL, *column = NULL;
	size_t cat_len = 0, schema_len = 0, table_len = 0, column_len = 0;
	RETCODE rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|s!s!s!s!", &pv_conn, &cat, &cat_len, &schema, &schema_len,
		&table, &table_len, &column, &column_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_THROWS();
	}

	result = (odbc_result *)ecalloc(1, sizeof(odbc_result));

	rc = PHP_ODBC_SQLALLOCSTMT(conn->hdbc, &(result->stmt));
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
			(SQLCHAR *) cat, (SQLSMALLINT) cat_len,
			(SQLCHAR *) schema, (SQLSMALLINT) schema_len,
			(SQLCHAR *) table, (SQLSMALLINT) table_len,
			(SQLCHAR *) column, (SQLSMALLINT) column_len);

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, result->stmt, "SQLColumns");
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

#if !defined(HAVE_DBMAKER) && !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35)
/* {{{ Returns a result identifier that can be used to fetch a list of columns and associated privileges for the specified table */
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
		RETURN_THROWS();
	}

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_THROWS();
	}

	result = (odbc_result *)ecalloc(1, sizeof(odbc_result));

	rc = PHP_ODBC_SQLALLOCSTMT(conn->hdbc, &(result->stmt));
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
			(SQLCHAR *) cat, SAFE_SQL_NTS(cat),
			(SQLCHAR *) schema, SAFE_SQL_NTS(schema),
			(SQLCHAR *) table, SAFE_SQL_NTS(table),
			(SQLCHAR *) column, SAFE_SQL_NTS(column));

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, result->stmt, "SQLColumnPrivileges");
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
/* {{{ Returns a result identifier to either a list of foreign keys in the specified table or a list of foreign keys in other tables that refer to the primary key in the specified table */
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
		RETURN_THROWS();
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
		RETURN_THROWS();
	}

	result = (odbc_result *)ecalloc(1, sizeof(odbc_result));

	rc = PHP_ODBC_SQLALLOCSTMT(conn->hdbc, &(result->stmt));
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
			(SQLCHAR *) pcat, SAFE_SQL_NTS(pcat),
			(SQLCHAR *) pschema, SAFE_SQL_NTS(pschema),
			(SQLCHAR *) ptable, SAFE_SQL_NTS(ptable),
			(SQLCHAR *) fcat, SAFE_SQL_NTS(fcat),
			(SQLCHAR *) fschema, SAFE_SQL_NTS(fschema),
			(SQLCHAR *) ftable, SAFE_SQL_NTS(ftable) );

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, result->stmt, "SQLForeignKeys");
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

/* {{{ Returns a result identifier containing information about data types supported by the data source */
PHP_FUNCTION(odbc_gettypeinfo)
{
	zval *pv_conn;
	zend_long pv_data_type = SQL_ALL_TYPES;
	odbc_result *result = NULL;
	odbc_connection *conn;
	RETCODE rc;
	SQLSMALLINT data_type;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|l", &pv_conn, &pv_data_type) == FAILURE) {
		RETURN_THROWS();
	}

	data_type = (SQLSMALLINT) pv_data_type;

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_THROWS();
	}

	result = (odbc_result *)ecalloc(1, sizeof(odbc_result));

	rc = PHP_ODBC_SQLALLOCSTMT(conn->hdbc, &(result->stmt));
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
		odbc_sql_error(conn, result->stmt, "SQLGetTypeInfo");
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

/* {{{ Returns a result identifier listing the column names that comprise the primary key for a table */
PHP_FUNCTION(odbc_primarykeys)
{
	zval *pv_conn;
	odbc_result   *result = NULL;
	odbc_connection *conn;
	char *cat = NULL, *schema = NULL, *table = NULL;
	size_t cat_len = 0, schema_len, table_len;
	RETCODE rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs!ss", &pv_conn, &cat, &cat_len, &schema, &schema_len, &table, &table_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_THROWS();
	}

	result = (odbc_result *)ecalloc(1, sizeof(odbc_result));

	rc = PHP_ODBC_SQLALLOCSTMT(conn->hdbc, &(result->stmt));
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
			(SQLCHAR *) cat, SAFE_SQL_NTS(cat),
			(SQLCHAR *) schema, SAFE_SQL_NTS(schema),
			(SQLCHAR *) table, SAFE_SQL_NTS(table) );

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, result->stmt, "SQLPrimaryKeys");
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

#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35)
/* {{{ Returns a result identifier containing the list of input and output parameters, as well as the columns that make up the result set for the specified procedures */
PHP_FUNCTION(odbc_procedurecolumns)
{
	zval *pv_conn;
	odbc_result *result = NULL;
	odbc_connection *conn;
	char *cat = NULL, *schema = NULL, *proc = NULL, *col = NULL;
	size_t cat_len = 0, schema_len = 0, proc_len = 0, col_len = 0;
	RETCODE rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|s!s!s!s!", &pv_conn, &cat, &cat_len, &schema, &schema_len,
		&proc, &proc_len, &col, &col_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_THROWS();
	}

	result = (odbc_result *)ecalloc(1, sizeof(odbc_result));

	rc = PHP_ODBC_SQLALLOCSTMT(conn->hdbc, &(result->stmt));
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
			(SQLCHAR *) cat, SAFE_SQL_NTS(cat),
			(SQLCHAR *) schema, SAFE_SQL_NTS(schema),
			(SQLCHAR *) proc, SAFE_SQL_NTS(proc),
			(SQLCHAR *) col, SAFE_SQL_NTS(col) );

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, result->stmt, "SQLProcedureColumns");
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
/* {{{ Returns a result identifier containing the list of procedure names in a datasource */
PHP_FUNCTION(odbc_procedures)
{
	zval *pv_conn;
	odbc_result   *result = NULL;
	odbc_connection *conn;
	char *cat = NULL, *schema = NULL, *proc = NULL;
	size_t cat_len = 0, schema_len = 0, proc_len = 0;
	RETCODE rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|s!s!s!", &pv_conn, &cat, &cat_len, &schema, &schema_len, &proc, &proc_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_THROWS();
	}

	result = (odbc_result *)ecalloc(1, sizeof(odbc_result));

	rc = PHP_ODBC_SQLALLOCSTMT(conn->hdbc, &(result->stmt));
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
			(SQLCHAR *) cat, SAFE_SQL_NTS(cat),
			(SQLCHAR *) schema, SAFE_SQL_NTS(schema),
			(SQLCHAR *) proc, SAFE_SQL_NTS(proc) );

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, result->stmt, "SQLProcedures");
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

/* {{{ Returns a result identifier containing either the optimal set of columns that uniquely identifies a row in the table or columns that are automatically updated when any value in the row is updated by a transaction */
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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rls!ssll", &pv_conn, &vtype, &cat, &cat_len, &schema, &schema_len,
		&name, &name_len, &vscope, &vnullable) == FAILURE) {
		RETURN_THROWS();
	}

	type = (SQLUSMALLINT) vtype;
	scope = (SQLUSMALLINT) vscope;
	nullable = (SQLUSMALLINT) vnullable;

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_THROWS();
	}

	result = (odbc_result *)ecalloc(1, sizeof(odbc_result));

	rc = PHP_ODBC_SQLALLOCSTMT(conn->hdbc, &(result->stmt));
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

	rc = SQLSpecialColumns(result->stmt, type,
			(SQLCHAR *) cat, SAFE_SQL_NTS(cat),
			(SQLCHAR *) schema, SAFE_SQL_NTS(schema),
			(SQLCHAR *) name, SAFE_SQL_NTS(name),
			scope,
			nullable);

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, result->stmt, "SQLSpecialColumns");
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

/* {{{ Returns a result identifier that contains statistics about a single table and the indexes associated with the table */
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
		RETURN_THROWS();
	}

	unique = (SQLUSMALLINT) vunique;
	reserved = (SQLUSMALLINT) vreserved;

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_THROWS();
	}

	result = (odbc_result *)ecalloc(1, sizeof(odbc_result));

	rc = PHP_ODBC_SQLALLOCSTMT(conn->hdbc, &(result->stmt));
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
			(SQLCHAR *) cat, SAFE_SQL_NTS(cat),
			(SQLCHAR *) schema, SAFE_SQL_NTS(schema),
			(SQLCHAR *) name, SAFE_SQL_NTS(name),
			unique,
			reserved);

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, result->stmt, "SQLStatistics");
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

#if !defined(HAVE_DBMAKER) && !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35)
/* {{{ Returns a result identifier containing a list of tables and the privileges associated with each table */
PHP_FUNCTION(odbc_tableprivileges)
{
	zval *pv_conn;
	odbc_result   *result = NULL;
	odbc_connection *conn;
	char *cat = NULL, *schema = NULL, *table = NULL;
	size_t cat_len = 0, schema_len, table_len;
	RETCODE rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs!ss", &pv_conn, &cat, &cat_len, &schema, &schema_len, &table, &table_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (!(conn = (odbc_connection *)zend_fetch_resource2(Z_RES_P(pv_conn), "ODBC-Link", le_conn, le_pconn))) {
		RETURN_THROWS();
	}

	result = (odbc_result *)ecalloc(1, sizeof(odbc_result));

	rc = PHP_ODBC_SQLALLOCSTMT(conn->hdbc, &(result->stmt));
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
			(SQLCHAR *) cat, SAFE_SQL_NTS(cat),
			(SQLCHAR *) schema, SAFE_SQL_NTS(schema),
			(SQLCHAR *) table, SAFE_SQL_NTS(table));

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, result->stmt, "SQLTablePrivileges");
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
