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
#include <config.h>
#endif

#include "php.h"
#include "php_globals.h"
#include "zend_attributes.h"

#include "ext/standard/info.h"
#include "Zend/zend_interfaces.h"
#include "zend_smart_str.h"

#include "php_odbc.h"
#include "php_odbc_includes.h"

/* actually lives in main/ */
#include "php_odbc_utils.h"

#ifdef HAVE_UODBC

#include <fcntl.h>
#include "php_ini.h"

#define PHP_ODBC_BINMODE_PASSTHRU 0
#define PHP_ODBC_BINMODE_RETURN 1
#define PHP_ODBC_BINMODE_CONVERT 2

#include "odbc_arginfo.h"

#define CHECK_ODBC_CONNECTION(conn) \
	if (conn == NULL) { \
		zend_throw_error(NULL, "ODBC connection has already been closed"); \
		RETURN_THROWS(); \
	}

#define CHECK_ODBC_RESULT(result) \
	if (result->conn_ptr == NULL) { \
		zend_throw_error(NULL, "ODBC result has already been closed"); \
		RETURN_THROWS(); \
	}

void odbc_do_connect(INTERNAL_FUNCTION_PARAMETERS, int persistent);
static void safe_odbc_disconnect(void *handle);
static void close_results_with_connection(odbc_connection *conn);
static inline odbc_result *odbc_result_from_obj(zend_object *obj);

static int le_pconn;

static zend_class_entry *odbc_connection_ce, *odbc_result_ce;
static zend_object_handlers odbc_connection_object_handlers, odbc_result_object_handlers;

#define Z_ODBC_LINK_P(zv) odbc_link_from_obj(Z_OBJ_P(zv))
#define Z_ODBC_CONNECTION_P(zv) Z_ODBC_LINK_P(zv)->connection
#define Z_ODBC_RESULT_P(zv) odbc_result_from_obj(Z_OBJ_P(zv))

static void odbc_insert_new_result(odbc_connection *connection, zval *result)
{
	ZEND_ASSERT(Z_TYPE_P(result) == IS_OBJECT);
#if ZEND_DEBUG
	ZEND_ASSERT(instanceof_function(Z_OBJCE_P(result), odbc_result_ce));
#endif

	odbc_result *res = Z_ODBC_RESULT_P(result);

	res->index = connection->results.nNextFreeElement;
	zend_hash_index_add_new(&connection->results, res->index, result);
	Z_ADDREF_P(result);
}

static inline odbc_link *odbc_link_from_obj(zend_object *obj)
{
	return (odbc_link *)((char *)(obj) - XtOffsetOf(odbc_link, std));
}

static int _close_pconn_with_res(zval *zv, void *p)
{
	zend_resource *le = Z_RES_P(zv);

	if (le->ptr == p) {
		return ZEND_HASH_APPLY_REMOVE;
	}

	return ZEND_HASH_APPLY_KEEP;
}

static int _close_pconn(zval *zv)
{
	zend_resource *le = Z_RES_P(zv);
	if (le->type == le_pconn) {
		return ZEND_HASH_APPLY_REMOVE;
	} else {
		return ZEND_HASH_APPLY_KEEP;
	}
}

/* disconnect, and if it fails, then issue a rollback for any pending transaction (lurcher) */
static void safe_odbc_disconnect( void *handle )
{
	int ret = SQLDisconnect( handle );

	if ( ret == SQL_ERROR )
	{
		SQLTransact( NULL, handle, SQL_ROLLBACK );
		SQLDisconnect( handle );
	}
}

static void free_connection(odbc_connection *conn, bool persistent)
{
	/* If aborted via timer expiration, don't try to call any unixODBC function */
	if (!(PG(connection_status) & PHP_CONNECTION_TIMEOUT)) {
		safe_odbc_disconnect(conn->hdbc);
		SQLFreeConnect(conn->hdbc);
		SQLFreeEnv(conn->henv);
	}

	conn->hdbc = NULL;
	conn->henv = NULL;

	zend_hash_destroy(&conn->results);

	pefree(conn, persistent);

	ODBCG(num_links)--;
	if (persistent) {
		ODBCG(num_persistent)--;
	}
}

static void odbc_link_free(odbc_link *link)
{
	ZEND_ASSERT(link->connection && "link has already been closed");

	close_results_with_connection(link->connection);

	if (!link->persistent) {
		free_connection(link->connection, link->persistent);
	}

	link->connection = NULL;

	if (link->hash) {
		zend_hash_del(&ODBCG(connections), link->hash);
		zend_string_release_ex(link->hash, link->persistent);
		link->hash = NULL;
	}
}

static zend_object *odbc_connection_create_object(zend_class_entry *class_type)
{
	odbc_link *intern = zend_object_alloc(sizeof(odbc_link), class_type);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

	return &intern->std;
}

static zend_function *odbc_connection_get_constructor(zend_object *object)
{
	zend_throw_error(NULL, "Cannot directly construct Odbc\\Connection, use odbc_connect() or odbc_pconnect() instead");
	return NULL;
}

static zend_result odbc_connection_cast_object(zend_object *obj, zval *result, int type)
{
	if (type == IS_LONG) {
		ZVAL_LONG(result, obj->handle);

		return SUCCESS;
	}

	return zend_std_cast_object_tostring(obj, result, type);
}

static void odbc_connection_free_obj(zend_object *obj)
{
	odbc_link *link = odbc_link_from_obj(obj);

	if (link->connection) {
		odbc_link_free(link);
	}

	zend_object_std_dtor(&link->std);
}

static inline odbc_result *odbc_result_from_obj(zend_object *obj)
{
	return (odbc_result *)((char *)(obj) - XtOffsetOf(odbc_result, std));
}

static zend_object *odbc_result_create_object(zend_class_entry *class_type)
{
	odbc_result *intern = zend_object_alloc(sizeof(odbc_result), class_type);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

	return &intern->std;
}

static zend_function *odbc_result_get_constructor(zend_object *object)
{
	zend_throw_error(NULL, "Cannot directly construct Odbc\\Result, use an appropriate odbc_* function instead");
	return NULL;
}

static zend_result odbc_result_cast_object(zend_object *obj, zval *result, int type)
{
	if (type == IS_LONG) {
		ZVAL_LONG(result, obj->handle);

		return SUCCESS;
	}

	return zend_std_cast_object_tostring(obj, result, type);
}

static void odbc_result_free(odbc_result *res)
{
	ZEND_ASSERT(res->conn_ptr && "result has already been closed");

	if (res->values) {
		for (int i = 0; i < res->numcols; i++) {
			if (res->values[i].value) {
				efree(res->values[i].value);
			}
		}
		efree(res->values);
		res->values = NULL;
		res->numcols = 0;
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
		 */
		res->stmt = NULL;
	}
	if (res->param_info) {
		efree(res->param_info);
		res->param_info = NULL;
	}

	HashTable *results = &res->conn_ptr->results;
	res->conn_ptr = NULL;
	zend_result status = zend_hash_index_del(results, res->index);
	ZEND_ASSERT(status == SUCCESS);
}

static void odbc_result_free_obj(zend_object *obj)
{
	odbc_result *result = odbc_result_from_obj(obj);

	if (result->conn_ptr) {
		odbc_result_free(result);
	}

	zend_object_std_dtor(&result->std);
}

#define SAFE_SQL_NTS(n) ((SQLSMALLINT) ((n)?(SQL_NTS):0))

PHP_ODBC_API ZEND_DECLARE_MODULE_GLOBALS(odbc)
static PHP_GINIT_FUNCTION(odbc);
static PHP_GSHUTDOWN_FUNCTION(odbc);

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
	PHP_GSHUTDOWN(odbc),
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

static void close_results_with_connection(odbc_connection *conn)
{
	zval *p;

	ZEND_HASH_FOREACH_VAL(&conn->results, p) {
		odbc_result *result = Z_ODBC_RESULT_P(p);
		if (result->conn_ptr) {
			odbc_result_free(result);
		}
	} ZEND_HASH_FOREACH_END();

	zend_hash_clean(&conn->results);
}

/* {{{ void _close_odbc_pconn */
static void _close_odbc_pconn(zend_resource *rsrc)
{
	odbc_connection *conn = (odbc_connection *)rsrc->ptr;

	close_results_with_connection(conn);
	free_connection(conn, true);

	rsrc->ptr = NULL;
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
	STD_PHP_INI_BOOLEAN("odbc.allow_persistent", "1", PHP_INI_SYSTEM, OnUpdateBool,
			allow_persistent, zend_odbc_globals, odbc_globals)
	STD_PHP_INI_ENTRY_EX("odbc.max_persistent",  "-1", PHP_INI_SYSTEM, OnUpdateLong,
			max_persistent, zend_odbc_globals, odbc_globals, display_link_nums)
	STD_PHP_INI_ENTRY_EX("odbc.max_links", "-1", PHP_INI_SYSTEM, OnUpdateLong,
			max_links, zend_odbc_globals, odbc_globals, display_link_nums)
	STD_PHP_INI_ENTRY_EX("odbc.defaultlrl", "4096", PHP_INI_ALL, OnUpdateLong,
			defaultlrl, zend_odbc_globals, odbc_globals, display_lrl)
	STD_PHP_INI_ENTRY_EX("odbc.defaultbinmode", "1", PHP_INI_ALL, OnUpdateLong,
			defaultbinmode, zend_odbc_globals, odbc_globals, display_binmode)
	STD_PHP_INI_BOOLEAN("odbc.check_persistent", "1", PHP_INI_SYSTEM, OnUpdateBool,
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
	zend_hash_init(&odbc_globals->connections, 0, NULL, NULL, true);
	GC_MAKE_PERSISTENT_LOCAL(&odbc_globals->connections);
}

static PHP_GSHUTDOWN_FUNCTION(odbc)
{
	zend_hash_destroy(&odbc_globals->connections);
}

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(odbc)
{
#ifdef SQLANY_BUG
	ODBC_SQL_CONN_T foobar;
	RETCODE rc;
#endif

	REGISTER_INI_ENTRIES();
	le_pconn = zend_register_list_destructors_ex(NULL, _close_odbc_pconn, "odbc link persistent", module_number);
	odbc_module_entry.type = type;

	register_odbc_symbols(module_number);

	odbc_connection_ce = register_class_Odbc_Connection();
	odbc_connection_ce->create_object = odbc_connection_create_object;
	odbc_connection_ce->default_object_handlers = &odbc_connection_object_handlers;

	memcpy(&odbc_connection_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	odbc_connection_object_handlers.offset = XtOffsetOf(odbc_link, std);
	odbc_connection_object_handlers.free_obj = odbc_connection_free_obj;
	odbc_connection_object_handlers.get_constructor = odbc_connection_get_constructor;
	odbc_connection_object_handlers.clone_obj = NULL;
	odbc_connection_object_handlers.cast_object = odbc_connection_cast_object;
	odbc_connection_object_handlers.compare = zend_objects_not_comparable;

	odbc_result_ce = register_class_Odbc_Result();
	odbc_result_ce->create_object = odbc_result_create_object;
	odbc_result_ce->default_object_handlers = &odbc_result_object_handlers;

	memcpy(&odbc_result_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	odbc_result_object_handlers.offset = XtOffsetOf(odbc_result, std);
	odbc_result_object_handlers.free_obj = odbc_result_free_obj;
	odbc_result_object_handlers.get_constructor = odbc_result_get_constructor;
	odbc_result_object_handlers.clone_obj = NULL;
	odbc_result_object_handlers.cast_object = odbc_result_cast_object;
	odbc_result_object_handlers.compare = zend_objects_not_comparable;

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
	php_info_print_table_row(2, "ODBC Support", "enabled");
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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ol", &pv_res, odbc_result_ce, &flag) == FAILURE) {
		RETURN_THROWS();
	}

	result = Z_ODBC_RESULT_P(pv_res);
	CHECK_ODBC_RESULT(result);

	if (mode) {
		result->longreadlen = flag;
	} else {
		result->binmode = flag;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ odbc_bindcols */
void odbc_bindcols(odbc_result *result)
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
}
/* }}} */

/* {{{ odbc_transact */
void odbc_transact(INTERNAL_FUNCTION_PARAMETERS, int type)
{
	RETCODE rc;
	zval *pv_conn;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &pv_conn, odbc_connection_ce) == FAILURE) {
		RETURN_THROWS();
	}

	odbc_connection *conn = Z_ODBC_CONNECTION_P(pv_conn);
	CHECK_ODBC_CONNECTION(conn);

	rc = SQLTransact(conn->henv, conn->hdbc, (SQLUSMALLINT)((type)?SQL_COMMIT:SQL_ROLLBACK));
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLTransact");
		RETURN_FALSE;
	}

	RETURN_TRUE;
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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ol", &pv_res, odbc_result_ce, &pv_num) == FAILURE) {
		RETURN_THROWS();
	}

	result = Z_ODBC_RESULT_P(pv_res);
	CHECK_ODBC_RESULT(result);

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
	zval *zv;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	/* Loop through the link list, now close all links and their results */
	ZEND_HASH_FOREACH_VAL(&ODBCG(connections), zv) {
		odbc_link *link = Z_ODBC_LINK_P(zv);
		if (link->connection) {
			odbc_link_free(link);
		}
	} ZEND_HASH_FOREACH_END();

	zend_hash_clean(&ODBCG(connections));

	zend_hash_apply(&EG(persistent_list), _close_pconn);
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
	RETCODE rc;
	int i;
#ifdef HAVE_SQL_EXTENDED_FETCH
	SQLUINTEGER      scrollopts;
#endif

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Os", &pv_conn, odbc_connection_ce, &query, &query_len) == FAILURE) {
		RETURN_THROWS();
	}

	odbc_connection *conn = Z_ODBC_CONNECTION_P(pv_conn);
	CHECK_ODBC_CONNECTION(conn);

	object_init_ex(return_value, odbc_result_ce);
	result = Z_ODBC_RESULT_P(return_value);

	result->numparams = 0;
	result->param_info = NULL;

	rc = PHP_ODBC_SQLALLOCSTMT(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		php_error_docref(NULL, E_WARNING, "SQLAllocStmt error 'Invalid Handle'");
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLAllocStmt");
		zval_ptr_dtor(return_value);
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
			zval_ptr_dtor(return_value);
			RETURN_FALSE;
	}

	SQLNumParams(result->stmt, &(result->numparams));
	SQLNumResultCols(result->stmt, &(result->numcols));

	if (result->numcols > 0) {
		odbc_bindcols(result);
	} else {
		result->values = NULL;
	}
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
			zval_ptr_dtor(return_value);
			RETURN_FALSE;
		}
	}

	odbc_insert_new_result(conn, return_value);
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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|h", &pv_res, odbc_result_ce, &pv_param_ht) == FAILURE) {
		RETURN_THROWS();
	}

	result = Z_ODBC_RESULT_P(pv_res);
	CHECK_ODBC_RESULT(result);

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
			odbc_bindcols(result);
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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &pv_res, odbc_result_ce) == FAILURE) {
		RETURN_THROWS();
	}

	result = Z_ODBC_RESULT_P(pv_res);
	CHECK_ODBC_RESULT(result);

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
	UCHAR server_name[100], desc[200];
	SQLSMALLINT len1=0, len2=0, fetch_type;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ol", &zv_conn, odbc_connection_ce, &zv_fetch_type) == FAILURE) {
		RETURN_THROWS();
	}

	fetch_type = (SQLSMALLINT) zv_fetch_type;

	if (!(fetch_type == SQL_FETCH_FIRST || fetch_type == SQL_FETCH_NEXT)) {
		zend_argument_value_error(2, "must be either SQL_FETCH_FIRST or SQL_FETCH_NEXT");
		RETURN_THROWS();
	}

	odbc_connection *conn = Z_ODBC_CONNECTION_P(zv_conn);
	CHECK_ODBC_CONNECTION(conn);

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
	RETCODE rc;
#ifdef HAVE_SQL_EXTENDED_FETCH
	SQLUINTEGER      scrollopts;
#endif

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Os", &pv_conn, odbc_connection_ce, &query, &query_len) == FAILURE) {
		RETURN_THROWS();
	}

	odbc_connection *conn = Z_ODBC_CONNECTION_P(pv_conn);
	CHECK_ODBC_CONNECTION(conn);

	object_init_ex(return_value, odbc_result_ce);
	result = Z_ODBC_RESULT_P(return_value);

	rc = PHP_ODBC_SQLALLOCSTMT(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		php_error_docref(NULL, E_WARNING, "SQLAllocStmt error 'Invalid Handle'");
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLAllocStmt");
		zval_ptr_dtor(return_value);
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
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	SQLNumResultCols(result->stmt, &(result->numcols));

	/* For insert, update etc. cols == 0 */
	if (result->numcols > 0) {
		odbc_bindcols(result);
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;

	odbc_insert_new_result(conn, return_value);
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
	zend_long pv_row = 0;
	bool pv_row_is_null = true;
	zval *pv_res, tmp;
#ifdef HAVE_SQL_EXTENDED_FETCH
	SQLULEN crow;
	SQLUSMALLINT RowStatus[1];
#endif

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|l!", &pv_res, odbc_result_ce, &pv_row, &pv_row_is_null) == FAILURE) {
		RETURN_THROWS();
	}

	result = Z_ODBC_RESULT_P(pv_res);
	CHECK_ODBC_RESULT(result);

	/* TODO deprecate $row argument values less than 1 after PHP 8.4 */

#ifndef HAVE_SQL_EXTENDED_FETCH
	if (!pv_row_is_null && pv_row > 0) {
		php_error_docref(NULL, E_WARNING, "Extended fetch functionality is not available, argument #3 ($row) is ignored");
	}
#endif

	if (result->numcols == 0) {
		php_error_docref(NULL, E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}

#ifdef HAVE_SQL_EXTENDED_FETCH
	if (result->fetch_abs) {
		if (!pv_row_is_null && pv_row > 0) {
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_ABSOLUTE,(SQLLEN)pv_row,&crow,RowStatus);
		} else {
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
		}
	} else
#endif
	rc = SQLFetch(result->stmt);

	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		if (rc == SQL_ERROR) {
#ifdef HAVE_SQL_EXTENDED_FETCH
		odbc_sql_error(result->conn_ptr, result->stmt, "SQLExtendedFetch");
#else
		odbc_sql_error(result->conn_ptr, result->stmt, "SQLFetch");
#endif
		}
		RETURN_FALSE;
	}

	array_init(return_value);

#ifdef HAVE_SQL_EXTENDED_FETCH
	if (!pv_row_is_null && pv_row > 0 && result->fetch_abs)
		result->fetched = (SQLLEN)pv_row;
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
	zend_long pv_row = 0;
	bool pv_row_is_null = true;
#ifdef HAVE_SQL_EXTENDED_FETCH
	SQLULEN crow;
	SQLUSMALLINT RowStatus[1];
#endif /* HAVE_SQL_EXTENDED_FETCH */

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Oz|l!", &pv_res, odbc_result_ce, &pv_res_arr, &pv_row, &pv_row_is_null) == FAILURE) {
		RETURN_THROWS();
	}

	result = Z_ODBC_RESULT_P(pv_res);
	CHECK_ODBC_RESULT(result);

	/* TODO deprecate $row argument values less than 1 after PHP 8.4 */

#ifndef HAVE_SQL_EXTENDED_FETCH
	if (!pv_row_is_null && pv_row > 0) {
		php_error_docref(NULL, E_WARNING, "Extended fetch functionality is not available, argument #3 ($row) is ignored");
	}
#endif

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
		if (!pv_row_is_null && pv_row > 0) {
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_ABSOLUTE,(SQLLEN)pv_row,&crow,RowStatus);
		} else {
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
		}
	} else
#endif
		rc = SQLFetch(result->stmt);

	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		if (rc == SQL_ERROR) {
#ifdef HAVE_SQL_EXTENDED_FETCH
		odbc_sql_error(result->conn_ptr, result->stmt, "SQLExtendedFetch");
#else
		odbc_sql_error(result->conn_ptr, result->stmt, "SQLFetch");
#endif
		}
		RETURN_FALSE;
	}

#ifdef HAVE_SQL_EXTENDED_FETCH
	if (!pv_row_is_null && pv_row > 0 && result->fetch_abs)
		result->fetched = (SQLLEN)pv_row;
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

/* {{{ Fetch a row */
PHP_FUNCTION(odbc_fetch_row)
{
	odbc_result *result;
	RETCODE rc;
	zval *pv_res;
	zend_long pv_row = 0;
	bool pv_row_is_null = true;
#ifdef HAVE_SQL_EXTENDED_FETCH
	SQLULEN crow;
	SQLUSMALLINT RowStatus[1];
#endif

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|l!", &pv_res, odbc_result_ce, &pv_row, &pv_row_is_null) == FAILURE) {
		RETURN_THROWS();
	}

	result = Z_ODBC_RESULT_P(pv_res);
	CHECK_ODBC_RESULT(result);

#ifndef HAVE_SQL_EXTENDED_FETCH
	if (!pv_row_is_null) {
		php_error_docref(NULL, E_WARNING, "Extended fetch functionality is not available, argument #3 ($row) is ignored");
	}
#else
	if (!pv_row_is_null && pv_row < 1) {
		php_error_docref(NULL, E_WARNING, "Argument #3 ($row) must be greater than or equal to 1");
		RETURN_FALSE;
	}
#endif

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
		if (rc == SQL_ERROR) {
#ifdef HAVE_SQL_EXTENDED_FETCH
		odbc_sql_error(result->conn_ptr, result->stmt, "SQLExtendedFetch");
#else
		odbc_sql_error(result->conn_ptr, result->stmt, "SQLFetch");
#endif
		}
		RETURN_FALSE;
	}

#ifdef HAVE_SQL_EXTENDED_FETCH
	if (!pv_row_is_null) {
		result->fetched = (SQLLEN)pv_row;
	} else
#endif
		result->fetched++;

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
		Z_PARAM_OBJECT_OF_CLASS(pv_res, odbc_result_ce)
		Z_PARAM_STR_OR_LONG(pv_field_str, pv_field_long)
	ZEND_PARSE_PARAMETERS_END();

	if (pv_field_str) {
		field = ZSTR_VAL(pv_field_str);
		field_ind = -1;
	} else {
		field = NULL;
		field_ind = (int) pv_field_long - 1;
	}

	result = Z_ODBC_RESULT_P(pv_res);
	CHECK_ODBC_RESULT(result);

	if (result->numcols == 0) {
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
			if (rc == SQL_ERROR) {
#ifdef HAVE_SQL_EXTENDED_FETCH
			odbc_sql_error(result->conn_ptr, result->stmt, "SQLExtendedFetch");
#else
			odbc_sql_error(result->conn_ptr, result->stmt, "SQLFetch");
#endif
			}
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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|s", &pv_res, odbc_result_ce, &pv_format, &pv_format_len) == FAILURE) {
		RETURN_THROWS();
	}

	result = Z_ODBC_RESULT_P(pv_res);
	CHECK_ODBC_RESULT(result);

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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &pv_res, odbc_result_ce) == FAILURE) {
		RETURN_THROWS();
	}

	result = Z_ODBC_RESULT_P(pv_res);
	CHECK_ODBC_RESULT(result);

	odbc_result_free(result);

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
bool odbc_sqlconnect(zval *zv, char *db, char *uid, char *pwd, int cur_opt, bool persistent, char *hash, int hash_len)
{
	RETCODE rc;
	SQLRETURN ret;
	odbc_link *link;

	object_init_ex(zv, odbc_connection_ce);
	link = Z_ODBC_LINK_P(zv);
	link->connection = pecalloc(1, sizeof(odbc_connection), persistent);
	zend_hash_init(&link->connection->results, 0, NULL, ZVAL_PTR_DTOR, true);
	link->persistent = persistent;
	link->hash = zend_string_init(hash, hash_len, persistent);
	if (persistent) {
		GC_MAKE_PERSISTENT_LOCAL(link->hash);
	}
	ret = SQLAllocEnv(&link->connection->henv);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		odbc_sql_error(link->connection, SQL_NULL_HSTMT, "SQLAllocEnv");
		return false;
	}

	ret = SQLAllocConnect(link->connection->henv, &link->connection->hdbc);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		odbc_sql_error(link->connection, SQL_NULL_HSTMT, "SQLAllocConnect");
		return false;
	}

#if defined(HAVE_SOLID) || defined(HAVE_SOLID_30)
	SQLSetConnectOption((link->connection->hdbc, SQL_TRANSLATE_OPTION,
			SQL_SOLID_XLATOPT_NOCNV);
#endif
#ifdef HAVE_OPENLINK
	{
		char dsnbuf[1024];
		short dsnbuflen;

		rc = SQLDriverConnect(link->connection->hdbc, NULL, db, SQL_NTS,	dsnbuf, sizeof(dsnbuf) - 1, &dsnbuflen, SQL_DRIVER_NOPROMPT);
	}
#else
	if (cur_opt != SQL_CUR_DEFAULT) {
		rc = SQLSetConnectOption(link->connection->hdbc, SQL_ODBC_CURSORS, cur_opt);
		if (rc != SQL_SUCCESS) {  /* && rc != SQL_SUCCESS_WITH_INFO ? */
			odbc_sql_error(link->connection, SQL_NULL_HSTMT, "SQLSetConnectOption");
			return false;
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

			/* This should be identical to the code in the PDO driver and vice versa. */
			size_t db_len = strlen(db);
			char *db_end = db + db_len;
			bool use_uid_arg = uid != NULL && !php_memnistr(db, "uid=", strlen("uid="), db_end);
			bool use_pwd_arg = pwd != NULL && !php_memnistr(db, "pwd=", strlen("pwd="), db_end);

			/* Force UID and PWD to be set in the DSN */
			if (use_uid_arg || use_pwd_arg) {
				db_end--;
				if ((unsigned char)*(db_end) == ';') {
					*db_end = '\0';
				}

				char *uid_quoted = NULL, *pwd_quoted = NULL;
				bool should_quote_uid, should_quote_pwd;
				if (use_uid_arg) {
					should_quote_uid = !php_odbc_connstr_is_quoted(uid) && php_odbc_connstr_should_quote(uid);
					if (should_quote_uid) {
						size_t estimated_length = php_odbc_connstr_estimate_quote_length(uid);
						uid_quoted = emalloc(estimated_length);
						php_odbc_connstr_quote(uid_quoted, uid, estimated_length);
					} else {
						uid_quoted = uid;
					}

					if (!use_pwd_arg) {
						spprintf(&ldb, 0, "%s;UID=%s;", db, uid_quoted);
					}
				}

				if (use_pwd_arg) {
					should_quote_pwd = !php_odbc_connstr_is_quoted(pwd) && php_odbc_connstr_should_quote(pwd);
					if (should_quote_pwd) {
						size_t estimated_length = php_odbc_connstr_estimate_quote_length(pwd);
						pwd_quoted = emalloc(estimated_length);
						php_odbc_connstr_quote(pwd_quoted, pwd, estimated_length);
					} else {
						pwd_quoted = pwd;
					}

					if (!use_uid_arg) {
						spprintf(&ldb, 0, "%s;PWD=%s;", db, pwd_quoted);
					}
				}

				if (use_uid_arg && use_pwd_arg) {
					spprintf(&ldb, 0, "%s;UID=%s;PWD=%s;", db, uid_quoted, pwd_quoted);
				}

				if (uid_quoted && should_quote_uid) {
					efree(uid_quoted);
				}
				if (pwd_quoted && should_quote_pwd) {
					efree(pwd_quoted);
				}
			} else {
				ldb_len = strlen(db)+1;
				ldb = (char*) emalloc(ldb_len);
				memcpy(ldb, db, ldb_len);
			}
		}

		if (direct) {
			rc = SQLDriverConnect(link->connection->hdbc, NULL, (SQLCHAR *) ldb, strlen(ldb), dsnbuf, sizeof(dsnbuf) - 1, &dsnbuflen, SQL_DRIVER_NOPROMPT);
		} else {
			rc = SQLConnect(link->connection->hdbc, (SQLCHAR *) db, SQL_NTS, (SQLCHAR *) uid, SQL_NTS, (SQLCHAR *) pwd, SQL_NTS);
		}

		if (ldb) {
			efree(ldb);
		}
	}
#else
	rc = SQLConnect(link->connection->hdbc, (SQLCHAR *) db, SQL_NTS, uid, SQL_NTS, pwd, SQL_NTS);
#endif
#endif
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		odbc_sql_error(link->connection, SQL_NULL_HSTMT, "SQLConnect");
		return false;
	}
	return true;
}
/* }}} */

/* {{{ odbc_do_connect */
void odbc_do_connect(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	char *db, *uid=NULL, *pwd=NULL;
	size_t db_len, uid_len, pwd_len;
	zend_long pv_opt = SQL_CUR_DEFAULT;
	odbc_connection *db_conn;
	int cur_opt;

	ZEND_PARSE_PARAMETERS_START(1, 4)
		Z_PARAM_STRING(db, db_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING_OR_NULL(uid, uid_len)
		Z_PARAM_STRING_OR_NULL(pwd, pwd_len)
		Z_PARAM_LONG(pv_opt)
	ZEND_PARSE_PARAMETERS_END();

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

	if (!ODBCG(allow_persistent)) {
		persistent = 0;
	}

	char *hashed_details;
	size_t hashed_details_len = spprintf(&hashed_details, 0, "odbc_%d_%s_%s_%s_%s_%d", persistent, ODBC_TYPE, db, uid, pwd, cur_opt);

try_and_get_another_connection:

	if (persistent) {
		zend_resource *le;

		/* the link is not in the persistent list */
		if ((le = zend_hash_str_find_ptr(&EG(persistent_list), hashed_details, hashed_details_len)) == NULL) {
			if (ODBCG(max_links) != -1 && ODBCG(num_links) >= ODBCG(max_links)) {
				php_error_docref(NULL, E_WARNING, "Too many open links (" ZEND_LONG_FMT ")", ODBCG(num_links));
				efree(hashed_details);
				RETURN_FALSE;
			}
			if (ODBCG(max_persistent) != -1 && ODBCG(num_persistent) >= ODBCG(max_persistent)) {
				php_error_docref(NULL, E_WARNING,"Too many open persistent links (" ZEND_LONG_FMT ")", ODBCG(num_persistent));
				efree(hashed_details);
				RETURN_FALSE;
			}

			if (!odbc_sqlconnect(return_value, db, uid, pwd, cur_opt, true, hashed_details, hashed_details_len)) {
				efree(hashed_details);
				zval_ptr_dtor(return_value);
				RETURN_FALSE;
			}

			db_conn = Z_ODBC_CONNECTION_P(return_value);

			if (zend_register_persistent_resource(hashed_details, hashed_details_len, db_conn, le_pconn) == NULL) {
				efree(hashed_details);
				zval_ptr_dtor(return_value);
				RETURN_FALSE;
			}

			zend_hash_str_add_new(&ODBCG(connections), hashed_details, hashed_details_len, return_value);

			ODBCG(num_persistent)++;
			ODBCG(num_links)++;
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
				SQLUINTEGER dead = SQL_CD_FALSE;

				ret = SQLGetConnectAttr(db_conn->hdbc,
					SQL_ATTR_CONNECTION_DEAD,
					&dead, 0, NULL);
				if (ret == SQL_SUCCESS && dead == SQL_CD_TRUE) {
					/* Bail early here, since we know it's gone */
					zend_hash_str_del(&EG(persistent_list), hashed_details, hashed_details_len);
					goto try_and_get_another_connection;
				}
				/* If the driver doesn't support it, or returns
				 * false (could be a false positive), fall back
				 * to the old heuristic.
				 */
				ret = SQLGetInfo(db_conn->hdbc,
					SQL_DATA_SOURCE_READ_ONLY,
					d_name, sizeof(d_name), &len);

				if(ret != SQL_SUCCESS || len == 0) {
					zend_hash_str_del(&EG(persistent_list), hashed_details, hashed_details_len);
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

			zval *link_zval;
			if ((link_zval = zend_hash_str_find(&ODBCG(connections), hashed_details, hashed_details_len)) == NULL) {
				object_init_ex(return_value, odbc_connection_ce);
				odbc_link *link = Z_ODBC_LINK_P(return_value);
				link->connection = db_conn;
				link->hash = zend_string_init(hashed_details, hashed_details_len, persistent);
				link->persistent = true;
			} else {
				ZVAL_COPY(return_value, link_zval);

				ZEND_ASSERT(Z_ODBC_CONNECTION_P(return_value) == db_conn && "Persistent connection has changed");
			}
		}
	} else { /* non-persistent */
		zval *link_zval;
		if ((link_zval = zend_hash_str_find(&ODBCG(connections), hashed_details, hashed_details_len)) == NULL) { /* non-persistent, new */
			if (ODBCG(max_links) != -1 && ODBCG(num_links) >= ODBCG(max_links)) {
				php_error_docref(NULL, E_WARNING, "Too many open connections (" ZEND_LONG_FMT ")", ODBCG(num_links));
				efree(hashed_details);
				RETURN_FALSE;
			}

			if (!odbc_sqlconnect(return_value, db, uid, pwd, cur_opt, false, hashed_details, hashed_details_len)) {
				efree(hashed_details);
				zval_ptr_dtor(return_value);
				RETURN_FALSE;
			}
			ODBCG(num_links)++;

			zend_hash_str_add_new(&ODBCG(connections), hashed_details, hashed_details_len, return_value);
		} else { /* non-persistent, pre-existing */
			ZVAL_COPY(return_value, link_zval);
		}
	}
	efree(hashed_details);
}
/* }}} */

/* {{{ Close an ODBC connection */
PHP_FUNCTION(odbc_close)
{
	zval *pv_conn;
	odbc_link *link;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &pv_conn, odbc_connection_ce) == FAILURE) {
		RETURN_THROWS();
	}

	link = Z_ODBC_LINK_P(pv_conn);
	odbc_connection *connection = Z_ODBC_CONNECTION_P(pv_conn);
	CHECK_ODBC_CONNECTION(connection);

	odbc_link_free(link);

	if (link->persistent) {
		zend_hash_apply_with_argument(&EG(persistent_list), _close_pconn_with_res, (void *) connection);
	}
}
/* }}} */

/* {{{ Get number of rows in a result */
PHP_FUNCTION(odbc_num_rows)
{
	odbc_result *result;
	SQLLEN rows;
	zval *pv_res;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &pv_res, odbc_result_ce) == FAILURE) {
		RETURN_THROWS();
	}

	result = Z_ODBC_RESULT_P(pv_res);
	CHECK_ODBC_RESULT(result);

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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &pv_res, odbc_result_ce) == FAILURE) {
		RETURN_THROWS();
	}

	result = Z_ODBC_RESULT_P(pv_res);
	CHECK_ODBC_RESULT(result);

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
			odbc_bindcols(result);
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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &pv_res, odbc_result_ce) == FAILURE) {
		RETURN_THROWS();
	}

	result = Z_ODBC_RESULT_P(pv_res);
	CHECK_ODBC_RESULT(result);

	RETURN_LONG(result->numcols);
}
/* }}} */

/* {{{ Get a column name */
PHP_FUNCTION(odbc_field_name)
{
	odbc_result *result;
	zval *pv_res;
	zend_long pv_num;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ol", &pv_res, odbc_result_ce, &pv_num) == FAILURE) {
		RETURN_THROWS();
	}

	result = Z_ODBC_RESULT_P(pv_res);
	CHECK_ODBC_RESULT(result);

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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ol", &pv_res, odbc_result_ce, &pv_num) == FAILURE) {
		RETURN_THROWS();
	}

	result = Z_ODBC_RESULT_P(pv_res);
	CHECK_ODBC_RESULT(result);

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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Os", &pv_res, odbc_result_ce, &fname, &fname_len) == FAILURE) {
		RETURN_THROWS();
	}

	result = Z_ODBC_RESULT_P(pv_res);
	CHECK_ODBC_RESULT(result);

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
	RETCODE rc;
	zval *pv_conn;
	bool pv_onoff = 0;
	bool pv_onoff_is_null = true;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|b!", &pv_conn, odbc_connection_ce, &pv_onoff, &pv_onoff_is_null) == FAILURE) {
		RETURN_THROWS();
	}

	odbc_connection *conn = Z_ODBC_CONNECTION_P(pv_conn);
	CHECK_ODBC_CONNECTION(conn);

	if (!pv_onoff_is_null) {
		rc = SQLSetConnectOption(conn->hdbc, SQL_AUTOCOMMIT, pv_onoff ? SQL_AUTOCOMMIT_ON : SQL_AUTOCOMMIT_OFF);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			odbc_sql_error(conn, SQL_NULL_HSTMT, "Set autocommit");
			RETURN_FALSE;
		}
		RETURN_TRUE;
	} else {
		SQLINTEGER status;

		rc = SQLGetConnectOption(conn->hdbc, SQL_AUTOCOMMIT, (PTR)&status);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			odbc_sql_error(conn, SQL_NULL_HSTMT, "Get commit status");
			RETURN_FALSE;
		}
		RETURN_LONG((zend_long)status);
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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|O!", &pv_handle, odbc_connection_ce) == FAILURE) {
		RETURN_THROWS();
	}

	if (pv_handle) {
		conn = Z_ODBC_CONNECTION_P(pv_handle);
		CHECK_ODBC_CONNECTION(conn);

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
	odbc_link *link;
	odbc_result	*result;
	RETCODE rc;
	zval *pv_handle;
	zend_long pv_which, pv_opt, pv_val;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "olll", &pv_handle, &pv_which, &pv_opt, &pv_val) == FAILURE) {
		RETURN_THROWS();
	}

	switch (pv_which) {
		case 1:		/* SQLSetConnectOption */
			if (!instanceof_function(Z_OBJCE_P(pv_handle), odbc_connection_ce)) {
				zend_argument_type_error(1, "must be of type Odbc\\Connection for SQLSetConnectOption()");
				RETURN_THROWS();
			}
			link = Z_ODBC_LINK_P(pv_handle);
			CHECK_ODBC_CONNECTION(link->connection);

			if (link->persistent) {
				php_error_docref(NULL, E_WARNING, "Unable to set option for persistent connection");
				RETURN_FALSE;
			}
			rc = SQLSetConnectOption(link->connection->hdbc, (unsigned short) pv_opt, pv_val);
			if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
				odbc_sql_error(link->connection, SQL_NULL_HSTMT, "SetConnectOption");
				RETURN_FALSE;
			}
			break;
		case 2:		/* SQLSetStmtOption */
			if (!instanceof_function(Z_OBJCE_P(pv_handle), odbc_result_ce)) {
				zend_argument_type_error(1, "must be of type Odbc\\Result for SQLSetStmtOption()");
				RETURN_THROWS();
			}
			result = Z_ODBC_RESULT_P(pv_handle);
			CHECK_ODBC_RESULT(result);

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
	char *cat = NULL, *schema = NULL, *table = NULL, *type = NULL;
	size_t cat_len = 0, schema_len = 0, table_len = 0, type_len = 0;
	RETCODE rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|s!s!s!s!", &pv_conn, odbc_connection_ce, &cat, &cat_len, &schema, &schema_len,
		&table, &table_len, &type, &type_len) == FAILURE) {
		RETURN_THROWS();
	}

	odbc_connection *conn = Z_ODBC_CONNECTION_P(pv_conn);
	CHECK_ODBC_CONNECTION(conn);

	object_init_ex(return_value, odbc_result_ce);
	result = Z_ODBC_RESULT_P(return_value);

	rc = PHP_ODBC_SQLALLOCSTMT(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		php_error_docref(NULL, E_WARNING, "SQLAllocStmt error 'Invalid Handle'");
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLAllocStmt");
		zval_ptr_dtor(return_value);
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
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	result->numparams = 0;
	SQLNumResultCols(result->stmt, &(result->numcols));

	if (result->numcols > 0) {
		odbc_bindcols(result);
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;

	odbc_insert_new_result(conn, return_value);
}
/* }}} */

/* {{{ Returns a result identifier that can be used to fetch a list of column names in specified tables */
PHP_FUNCTION(odbc_columns)
{
	zval *pv_conn;
	odbc_result *result = NULL;
	char *cat = NULL, *schema = NULL, *table = NULL, *column = NULL;
	size_t cat_len = 0, schema_len = 0, table_len = 0, column_len = 0;
	RETCODE rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|s!s!s!s!", &pv_conn, odbc_connection_ce, &cat, &cat_len, &schema, &schema_len,
		&table, &table_len, &column, &column_len) == FAILURE) {
		RETURN_THROWS();
	}

	odbc_connection *conn = Z_ODBC_CONNECTION_P(pv_conn);
	CHECK_ODBC_CONNECTION(conn);

	object_init_ex(return_value, odbc_result_ce);
	result = Z_ODBC_RESULT_P(return_value);

	rc = PHP_ODBC_SQLALLOCSTMT(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		php_error_docref(NULL, E_WARNING, "SQLAllocStmt error 'Invalid Handle'");
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLAllocStmt");
		zval_ptr_dtor(return_value);
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
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	result->numparams = 0;
	SQLNumResultCols(result->stmt, &(result->numcols));

	if (result->numcols > 0) {
		odbc_bindcols(result);
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;

	odbc_insert_new_result(conn, return_value);
}
/* }}} */

#if !defined(HAVE_DBMAKER) && !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35)
/* {{{ Returns a result identifier that can be used to fetch a list of columns and associated privileges for the specified table */
PHP_FUNCTION(odbc_columnprivileges)
{
	zval *pv_conn;
	odbc_result *result = NULL;
	char *cat = NULL, *schema, *table, *column;
	size_t cat_len = 0, schema_len, table_len, column_len;
	RETCODE rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Os!sss", &pv_conn, odbc_connection_ce, &cat, &cat_len, &schema, &schema_len,
		&table, &table_len, &column, &column_len) == FAILURE) {
		RETURN_THROWS();
	}

	odbc_connection *conn = Z_ODBC_CONNECTION_P(pv_conn);
	CHECK_ODBC_CONNECTION(conn);

	object_init_ex(return_value, odbc_result_ce);
	result = Z_ODBC_RESULT_P(return_value);

	rc = PHP_ODBC_SQLALLOCSTMT(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		php_error_docref(NULL, E_WARNING, "SQLAllocStmt error 'Invalid Handle'");
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLAllocStmt");
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	rc = SQLColumnPrivileges(result->stmt,
			(SQLCHAR *) cat, SAFE_SQL_NTS(cat),
			(SQLCHAR *) schema, SAFE_SQL_NTS(schema),
			(SQLCHAR *) table, SAFE_SQL_NTS(table),
			(SQLCHAR *) column, SAFE_SQL_NTS(column));

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, result->stmt, "SQLColumnPrivileges");
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	result->numparams = 0;
	SQLNumResultCols(result->stmt, &(result->numcols));

	if (result->numcols > 0) {
		odbc_bindcols(result);
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;

	odbc_insert_new_result(conn, return_value);
}
/* }}} */
#endif /* HAVE_DBMAKER || HAVE_SOLID*/

#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35)
/* {{{ Returns a result identifier to either a list of foreign keys in the specified table or a list of foreign keys in other tables that refer to the primary key in the specified table */
PHP_FUNCTION(odbc_foreignkeys)
{
	zval *pv_conn;
	odbc_result *result = NULL;
	char *pcat = NULL, *pschema, *ptable, *fcat, *fschema, *ftable;
	size_t pcat_len = 0, pschema_len, ptable_len, fcat_len, fschema_len, ftable_len;
	RETCODE rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Os!sssss", &pv_conn, odbc_connection_ce, &pcat, &pcat_len, &pschema, &pschema_len,
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

	odbc_connection *conn = Z_ODBC_CONNECTION_P(pv_conn);
	CHECK_ODBC_CONNECTION(conn);

	object_init_ex(return_value, odbc_result_ce);
	result = Z_ODBC_RESULT_P(return_value);

	rc = PHP_ODBC_SQLALLOCSTMT(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		php_error_docref(NULL, E_WARNING, "SQLAllocStmt error 'Invalid Handle'");
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLAllocStmt");
		zval_ptr_dtor(return_value);
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
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	result->numparams = 0;
	SQLNumResultCols(result->stmt, &(result->numcols));

	if (result->numcols > 0) {
		odbc_bindcols(result);
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;

	odbc_insert_new_result(conn, return_value);
}
/* }}} */
#endif /* HAVE_SOLID */

/* {{{ Returns a result identifier containing information about data types supported by the data source */
PHP_FUNCTION(odbc_gettypeinfo)
{
	zval *pv_conn;
	zend_long pv_data_type = SQL_ALL_TYPES;
	odbc_result *result = NULL;
	RETCODE rc;
	SQLSMALLINT data_type;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|l", &pv_conn, odbc_connection_ce, &pv_data_type) == FAILURE) {
		RETURN_THROWS();
	}

	data_type = (SQLSMALLINT) pv_data_type;

	odbc_connection *conn = Z_ODBC_CONNECTION_P(pv_conn);
	CHECK_ODBC_CONNECTION(conn);

	object_init_ex(return_value, odbc_result_ce);
	result = Z_ODBC_RESULT_P(return_value);

	rc = PHP_ODBC_SQLALLOCSTMT(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		php_error_docref(NULL, E_WARNING, "SQLAllocStmt error 'Invalid Handle'");
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLAllocStmt");
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	rc = SQLGetTypeInfo(result->stmt, data_type );

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, result->stmt, "SQLGetTypeInfo");
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	result->numparams = 0;
	SQLNumResultCols(result->stmt, &(result->numcols));

	if (result->numcols > 0) {
		odbc_bindcols(result);
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;

	odbc_insert_new_result(conn, return_value);
}
/* }}} */

/* {{{ Returns a result identifier listing the column names that comprise the primary key for a table */
PHP_FUNCTION(odbc_primarykeys)
{
	zval *pv_conn;
	odbc_result   *result = NULL;
	char *cat = NULL, *schema = NULL, *table = NULL;
	size_t cat_len = 0, schema_len, table_len;
	RETCODE rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Os!ss", &pv_conn, odbc_connection_ce, &cat, &cat_len, &schema, &schema_len, &table, &table_len) == FAILURE) {
		RETURN_THROWS();
	}

	odbc_connection *conn = Z_ODBC_CONNECTION_P(pv_conn);
	CHECK_ODBC_CONNECTION(conn);

	object_init_ex(return_value, odbc_result_ce);
	result = Z_ODBC_RESULT_P(return_value);

	rc = PHP_ODBC_SQLALLOCSTMT(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		php_error_docref(NULL, E_WARNING, "SQLAllocStmt error 'Invalid Handle'");
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLAllocStmt");
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	rc = SQLPrimaryKeys(result->stmt,
			(SQLCHAR *) cat, SAFE_SQL_NTS(cat),
			(SQLCHAR *) schema, SAFE_SQL_NTS(schema),
			(SQLCHAR *) table, SAFE_SQL_NTS(table) );

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, result->stmt, "SQLPrimaryKeys");
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	result->numparams = 0;
	SQLNumResultCols(result->stmt, &(result->numcols));

	if (result->numcols > 0) {
		odbc_bindcols(result);
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;

	odbc_insert_new_result(conn, return_value);
}
/* }}} */

#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35)
/* {{{ Returns a result identifier containing the list of input and output parameters, as well as the columns that make up the result set for the specified procedures */
PHP_FUNCTION(odbc_procedurecolumns)
{
	zval *pv_conn;
	odbc_result *result = NULL;
	char *cat = NULL, *schema = NULL, *proc = NULL, *col = NULL;
	size_t cat_len = 0, schema_len = 0, proc_len = 0, col_len = 0;
	RETCODE rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|s!s!s!s!", &pv_conn, odbc_connection_ce, &cat, &cat_len, &schema, &schema_len,
		&proc, &proc_len, &col, &col_len) == FAILURE) {
		RETURN_THROWS();
	}

	odbc_connection *conn = Z_ODBC_CONNECTION_P(pv_conn);
	CHECK_ODBC_CONNECTION(conn);

	object_init_ex(return_value, odbc_result_ce);
	result = Z_ODBC_RESULT_P(return_value);

	rc = PHP_ODBC_SQLALLOCSTMT(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		php_error_docref(NULL, E_WARNING, "SQLAllocStmt error 'Invalid Handle'");
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLAllocStmt");
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	rc = SQLProcedureColumns(result->stmt,
			(SQLCHAR *) cat, SAFE_SQL_NTS(cat),
			(SQLCHAR *) schema, SAFE_SQL_NTS(schema),
			(SQLCHAR *) proc, SAFE_SQL_NTS(proc),
			(SQLCHAR *) col, SAFE_SQL_NTS(col) );

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, result->stmt, "SQLProcedureColumns");
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	result->numparams = 0;
	SQLNumResultCols(result->stmt, &(result->numcols));

	if (result->numcols > 0) {
		odbc_bindcols(result);
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;

	odbc_insert_new_result(conn, return_value);
}
/* }}} */
#endif /* HAVE_SOLID */

#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35)
/* {{{ Returns a result identifier containing the list of procedure names in a datasource */
PHP_FUNCTION(odbc_procedures)
{
	zval *pv_conn;
	odbc_result   *result = NULL;
	char *cat = NULL, *schema = NULL, *proc = NULL;
	size_t cat_len = 0, schema_len = 0, proc_len = 0;
	RETCODE rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|s!s!s!", &pv_conn, odbc_connection_ce, &cat, &cat_len, &schema, &schema_len, &proc, &proc_len) == FAILURE) {
		RETURN_THROWS();
	}

	odbc_connection *conn = Z_ODBC_CONNECTION_P(pv_conn);
	CHECK_ODBC_CONNECTION(conn);

	object_init_ex(return_value, odbc_result_ce);
	result = Z_ODBC_RESULT_P(return_value);

	rc = PHP_ODBC_SQLALLOCSTMT(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		php_error_docref(NULL, E_WARNING, "SQLAllocStmt error 'Invalid Handle'");
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLAllocStmt");
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	rc = SQLProcedures(result->stmt,
			(SQLCHAR *) cat, SAFE_SQL_NTS(cat),
			(SQLCHAR *) schema, SAFE_SQL_NTS(schema),
			(SQLCHAR *) proc, SAFE_SQL_NTS(proc) );

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, result->stmt, "SQLProcedures");
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	result->numparams = 0;
	SQLNumResultCols(result->stmt, &(result->numcols));

	if (result->numcols > 0) {
		odbc_bindcols(result);
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;

	odbc_insert_new_result(conn, return_value);
}
/* }}} */
#endif /* HAVE_SOLID */

/* {{{ Returns a result identifier containing either the optimal set of columns that uniquely identifies a row in the table or columns that are automatically updated when any value in the row is updated by a transaction */
PHP_FUNCTION(odbc_specialcolumns)
{
	zval *pv_conn;
	zend_long vtype, vscope, vnullable;
	odbc_result *result = NULL;
	char *cat = NULL, *schema = NULL, *name = NULL;
	size_t cat_len = 0, schema_len, name_len;
	SQLUSMALLINT type, scope, nullable;
	RETCODE rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ols!ssll", &pv_conn, odbc_connection_ce, &vtype, &cat, &cat_len, &schema, &schema_len,
		&name, &name_len, &vscope, &vnullable) == FAILURE) {
		RETURN_THROWS();
	}

	type = (SQLUSMALLINT) vtype;
	scope = (SQLUSMALLINT) vscope;
	nullable = (SQLUSMALLINT) vnullable;

	odbc_connection *conn = Z_ODBC_CONNECTION_P(pv_conn);
	CHECK_ODBC_CONNECTION(conn);

	object_init_ex(return_value, odbc_result_ce);
	result = Z_ODBC_RESULT_P(return_value);

	rc = PHP_ODBC_SQLALLOCSTMT(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		php_error_docref(NULL, E_WARNING, "SQLAllocStmt error 'Invalid Handle'");
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLAllocStmt");
		zval_ptr_dtor(return_value);
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
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	result->numparams = 0;
	SQLNumResultCols(result->stmt, &(result->numcols));

	if (result->numcols > 0) {
		odbc_bindcols(result);
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;

	odbc_insert_new_result(conn, return_value);
}
/* }}} */

/* {{{ Returns a result identifier that contains statistics about a single table and the indexes associated with the table */
PHP_FUNCTION(odbc_statistics)
{
	zval *pv_conn;
	zend_long vunique, vreserved;
	odbc_result *result = NULL;
	char *cat = NULL, *schema, *name;
	size_t cat_len = 0, schema_len, name_len;
	SQLUSMALLINT unique, reserved;
	RETCODE rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Os!ssll", &pv_conn, odbc_connection_ce, &cat, &cat_len, &schema, &schema_len,
		&name, &name_len, &vunique, &vreserved) == FAILURE) {
		RETURN_THROWS();
	}

	unique = (SQLUSMALLINT) vunique;
	reserved = (SQLUSMALLINT) vreserved;

	odbc_connection *conn = Z_ODBC_CONNECTION_P(pv_conn);
	CHECK_ODBC_CONNECTION(conn);

	object_init_ex(return_value, odbc_result_ce);
	result = Z_ODBC_RESULT_P(return_value);

	rc = PHP_ODBC_SQLALLOCSTMT(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		php_error_docref(NULL, E_WARNING, "SQLAllocStmt error 'Invalid Handle'");
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLAllocStmt");
		zval_ptr_dtor(return_value);
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
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	result->numparams = 0;
	SQLNumResultCols(result->stmt, &(result->numcols));

	if (result->numcols > 0) {
		odbc_bindcols(result);
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;

	odbc_insert_new_result(conn, return_value);
}
/* }}} */

#if !defined(HAVE_DBMAKER) && !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35)
/* {{{ Returns a result identifier containing a list of tables and the privileges associated with each table */
PHP_FUNCTION(odbc_tableprivileges)
{
	zval *pv_conn;
	odbc_result   *result = NULL;
	char *cat = NULL, *schema = NULL, *table = NULL;
	size_t cat_len = 0, schema_len, table_len;
	RETCODE rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Os!ss", &pv_conn, odbc_connection_ce, &cat, &cat_len, &schema, &schema_len, &table, &table_len) == FAILURE) {
		RETURN_THROWS();
	}

	odbc_connection *conn = Z_ODBC_CONNECTION_P(pv_conn);
	CHECK_ODBC_CONNECTION(conn);

	object_init_ex(return_value, odbc_result_ce);
	result = Z_ODBC_RESULT_P(return_value);

	rc = PHP_ODBC_SQLALLOCSTMT(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		php_error_docref(NULL, E_WARNING, "SQLAllocStmt error 'Invalid Handle'");
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLAllocStmt");
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	rc = SQLTablePrivileges(result->stmt,
			(SQLCHAR *) cat, SAFE_SQL_NTS(cat),
			(SQLCHAR *) schema, SAFE_SQL_NTS(schema),
			(SQLCHAR *) table, SAFE_SQL_NTS(table));

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, result->stmt, "SQLTablePrivileges");
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	result->numparams = 0;
	SQLNumResultCols(result->stmt, &(result->numcols));

	if (result->numcols > 0) {
		odbc_bindcols(result);
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;

	odbc_insert_new_result(conn, return_value);
}
/* }}} */
#endif /* HAVE_DBMAKER */

#endif /* HAVE_UODBC */
