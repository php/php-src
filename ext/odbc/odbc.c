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
   |          Frank M. Kromann <fmk@businessnet.dk> Support for DB/2 CLI  |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* This file is based on the Adabas D extension.
 * Adabas D will no longer be supported as separate module.
 */
#define IS_EXT_MODULE
#if !PHP_31
#undef THREAD_SAFE
#endif
#ifdef THREAD_SAFE
#include "tls.h"
#endif
#if !(WIN32|WINNT)
#include "config.h"
#include "build-defs.h"
#endif
#include <fcntl.h>

#include "php.h"
#include "dl/phpdl.h"
#include "ext/standard/php3_standard.h"
#include "php3_odbc.h"
#include "functions/head.h"
#include "snprintf.h"

#if HAVE_UODBC

#ifdef THREAD_SAFE

void *UODBC_MUTEX;
DWORD UODBC_TLS;
static int numthreads=0;

typedef struct UODBC_GLOBAL_STRUCT {
	UODBC_MODULE PHP3_UODBC_MODULE;
} UODBC_GLOBAL_STRUCT;

#define UODBC_GLOBAL(a) UODBC_GLOBALS->a

#define UODBC_TLS_VARS UODBC_GLOBAL_STRUCT *UODBC_GLOBALS = TlsGetValue(UODBC_TLS); 

#else
UODBC_MODULE PHP3_UODBC_MODULE;
#define UODBC_GLOBAL(a) a
#define UODBC_TLS_VARS
#endif

function_entry UODBC_FUNCTIONS[] = {
	UODBC_FE(setoption, NULL),
	UODBC_FE(autocommit, NULL),
	UODBC_FE(close, NULL),
	UODBC_FE(close_all, NULL),
	UODBC_FE(commit, NULL),
	UODBC_FE(connect, NULL),
	UODBC_FE(pconnect, NULL),
	UODBC_FE(cursor, NULL),
	UODBC_FE_ALIAS(do, exec, NULL),
	UODBC_FE(exec, NULL),
	UODBC_FE(prepare, NULL),
	UODBC_FE(execute, NULL),
	UODBC_FE(fetch_row, NULL),
	UODBC_FE(fetch_into, NULL),
	UODBC_FE(field_len, NULL),
	UODBC_FE(field_name, NULL),
	UODBC_FE(field_type, NULL),
	UODBC_FE(field_num, NULL),
	UODBC_FE(free_result, NULL),
	UODBC_FE(num_fields, NULL),
	UODBC_FE(num_rows, NULL),
	UODBC_FE(result, NULL),
	UODBC_FE(result_all, NULL),
	UODBC_FE(rollback, NULL),
	UODBC_FE(binmode, NULL),
	UODBC_FE(longreadlen, NULL),
#if defined(USE_ODBC_ALIAS) && defined(UODBC_UNIQUE_NAMES)
	UODBC_ALIAS(setoption, NULL),
	UODBC_ALIAS(autocommit, NULL),
	UODBC_ALIAS(close, NULL),
	UODBC_ALIAS(close_all, NULL),
	UODBC_ALIAS(commit, NULL),
	UODBC_ALIAS(connect, NULL),
	UODBC_ALIAS(pconnect, NULL),
	UODBC_ALIAS(cursor, NULL),
	UODBC_ALIAS_FE(do, exec, NULL),
	UODBC_ALIAS(exec, NULL),
	UODBC_ALIAS(prepare, NULL),
	UODBC_ALIAS(execute, NULL),
	UODBC_ALIAS(fetch_row, NULL),
	UODBC_ALIAS(fetch_into, NULL),
	UODBC_ALIAS(field_len, NULL),
	UODBC_ALIAS(field_name, NULL),
	UODBC_ALIAS(field_type, NULL),
	UODBC_ALIAS(field_num, NULL),
	UODBC_ALIAS(free_result, NULL),
	UODBC_ALIAS(num_fields, NULL),
	UODBC_ALIAS(num_rows, NULL),
	UODBC_ALIAS(result, NULL),
	UODBC_ALIAS(result_all, NULL),
	UODBC_ALIAS(rollback, NULL),
	UODBC_ALIAS(binmode, NULL),
	UODBC_ALIAS(longreadlen, NULL),
#endif
#if HAVE_SOLID
	{"solid_fetch_prev",php3_solid_fetch_prev,	NULL},
#endif
#if 0 && HAVE_IODBC /* this won't work with the new system */
/* for backwards compatibility with the older odbc module*/
	{"sqlconnect",		php3_uodbc_connect,		NULL},
	{"sqldisconnect",	php3_uodbc_close,		NULL},
	{"sqlfetch",		php3_uodbc_fetch_row,	NULL},
	{"sqlexecdirect",	php3_uodbc_do,			NULL},
	{"sqlgetdata",		php3_uodbc_result,		NULL},
	{"sqlfree",			php3_uodbc_free_result,	NULL},
	{"sqlrowcount",		php3_uodbc_num_rows,	NULL},
#endif
	{ NULL, NULL, NULL }
};

php3_module_entry UODBC_MODULE_ENTRY = {
    UODBC_MODULE_NAME, 
	UODBC_FUNCTIONS, 
	PHP3_MINIT_UODBC, 
	PHP3_MSHUTDOWN_UODBC,
    PHP3_RINIT_UODBC, 
	NULL, 
	PHP3_INFO_UODBC, 
	STANDARD_MODULE_PROPERTIES
};


#if COMPILE_DL
DLEXPORT php3_module_entry *get_module() { return &UODBC_MODULE_ENTRY; };
#endif


static void _free_result(UODBC_RESULT *res)
{
	int i;
	
	if (res){
		if (res->values) {
			for(i = 0; i < res->numcols; i++){
				if (res->values[i].value)
					efree(res->values[i].value);
			}
			efree(res->values);
			res->values = NULL;
		}
		if (res->stmt){
#if HAVE_SOLID
			SQLTransact(UODBC_GLOBAL(PHP3_UODBC_MODULE).henv, res->conn_ptr->hdbc,
						(UWORD)SQL_COMMIT);
#endif
			SQLFreeStmt(res->stmt,SQL_DROP);
#if !HAVE_DB2
			res->stmt = NULL;
#endif
		}
		efree(res);
	}
}

static int _results_cleanup(list_entry *le)
{
	UODBC_TLS_VARS;

	if (le->type == UODBC_GLOBAL(PHP3_UODBC_MODULE).le_result){
		UODBC_CONNECTION *conn = ((UODBC_RESULT *) le->ptr)->conn_ptr;
		if (!conn->open && ((UODBC_RESULT *) le->ptr)->stmt){
			SQLFreeStmt(((UODBC_RESULT *) le->ptr)->stmt,SQL_DROP);
#if !HAVE_DB2
			((UODBC_RESULT *) le->ptr)->stmt = NULL;
#endif
		}
	}
	return 0;
}

static void _close_connection(UODBC_CONNECTION *conn)
{
	/* FIXME
	 * Closing a connection will fail if there are
	 * pending transactions
	 */
	UODBC_TLS_VARS;

	conn->open = 0;
	_php3_hash_apply(UODBC_GLOBAL(PHP3_UODBC_MODULE).resource_list,
				(int (*)(void *))_results_cleanup);
	SQLDisconnect(conn->hdbc);
	SQLFreeConnect(conn->hdbc);
	efree(conn);
	UODBC_GLOBAL(PHP3_UODBC_MODULE).num_links--;
}


static void _close_pconnection(UODBC_CONNECTION *conn)
{
	UODBC_TLS_VARS;


	conn->open = 0;
	_php3_hash_apply(UODBC_GLOBAL(PHP3_UODBC_MODULE).resource_plist,
				(int (*)(void *))_results_cleanup);

	SQLDisconnect(conn->hdbc);
	SQLFreeConnect(conn->hdbc);
	free(conn);

	UODBC_GLOBAL(PHP3_UODBC_MODULE).num_links--;
	UODBC_GLOBAL(PHP3_UODBC_MODULE).num_persistent--;
}


int PHP3_MINIT_UODBC(INIT_FUNC_ARGS) 
{
#ifdef SQLANY_BUG
	HDBC    foobar;
	RETCODE rc;
#endif

#if defined(THREAD_SAFE)
	UODBC_GLOBAL_STRUCT *UODBC_GLOBALS;

	PHP3_MUTEX_ALLOC(UODBC_MUTEX);
	PHP3_MUTEX_LOCK(UODBC_MUTEX);
	numthreads++;
	if (numthreads==1){
		if (!PHP3_TLS_PROC_STARTUP(UODBC_TLS)){
			PHP3_MUTEX_UNLOCK(UODBC_MUTEX);
			PHP3_MUTEX_FREE(UODBC_MUTEX);
			return FAILURE;
		}
	}
	PHP3_MUTEX_UNLOCK(UODBC_MUTEX);
	if(!PHP3_TLS_THREAD_INIT(UODBC_TLS,UODBC_GLOBALS,UODBC_GLOBAL_STRUCT)){
		PHP3_MUTEX_FREE(UODBC_MUTEX);
		return FAILURE;
	}
#endif

	SQLAllocEnv(&UODBC_GLOBAL(PHP3_UODBC_MODULE).henv);
	
#if !PHP_31
	cfg_get_string(ODBC_INI_VAR(default_db),
				   &UODBC_GLOBAL(PHP3_UODBC_MODULE).defDB);
	cfg_get_string(ODBC_INI_VAR(default_user),
				   &UODBC_GLOBAL(PHP3_UODBC_MODULE).defUser);
	cfg_get_string(ODBC_INI_VAR(default_pw),
				   &UODBC_GLOBAL(PHP3_UODBC_MODULE).defPW);
	if (cfg_get_long(ODBC_INI_VAR(allow_persistent),
					 &UODBC_GLOBAL(PHP3_UODBC_MODULE).allow_persistent)
		== FAILURE) {
		UODBC_GLOBAL(PHP3_UODBC_MODULE).allow_persistent = -1;
	}
	if (cfg_get_long(ODBC_INI_VAR(max_persistent),
					 &UODBC_GLOBAL(PHP3_UODBC_MODULE).max_persistent)
		== FAILURE) {
		UODBC_GLOBAL(PHP3_UODBC_MODULE).max_persistent = -1;
	}
	if (cfg_get_long(ODBC_INI_VAR(max_links),
					 &UODBC_GLOBAL(PHP3_UODBC_MODULE).max_links)
		== FAILURE) {
		UODBC_GLOBAL(PHP3_UODBC_MODULE).max_links = -1;
	}
#else
	UODBC_GLOBAL(PHP3_UODBC_MODULE).allow_persistent = -1;
	UODBC_GLOBAL(PHP3_UODBC_MODULE).max_persistent = -1;
	UODBC_GLOBAL(PHP3_UODBC_MODULE).max_links = -1;
#endif

	UODBC_GLOBAL(PHP3_UODBC_MODULE).num_persistent = 0;

	UODBC_GLOBAL(PHP3_UODBC_MODULE).le_result =
		register_list_destructors(_free_result, NULL);
	UODBC_GLOBAL(PHP3_UODBC_MODULE).le_conn =
		register_list_destructors(_close_connection, NULL);
	UODBC_GLOBAL(PHP3_UODBC_MODULE).le_pconn =
		register_list_destructors(NULL, _close_pconnection);

#ifdef SQLANY_BUG
/* Make a dumb connection to avoid crash on SQLFreeEnv(),
 * then release it immediately.
 * This is required for SQL Anywhere 5.5.00 on QNX 4.24 at least.
 * The SQLANY_BUG should be defined in CFLAGS.
 */
	if ( SQLAllocConnect(UODBC_GLOBAL(PHP3_UODBC_MODULE).henv, &foobar) != SQL_SUCCESS )
			UODBC_SQL_ERROR(SQL_NULL_HDBC, SQL_NULL_HSTMT, "SQLAllocConnect");
	else
	{
		rc = SQLConnect(foobar, UODBC_GLOBAL(PHP3_UODBC_MODULE).defDB, SQL_NTS, UODBC_GLOBAL(PHP3_UODBC_MODULE).defUser, 
						SQL_NTS, UODBC_GLOBAL(PHP3_UODBC_MODULE).defPW, SQL_NTS);
		if(rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
			SQLDisconnect( foobar );
		SQLFreeConnect( foobar );
	}
#endif

	REGISTER_LONG_CONSTANT("ODBC_BINMODE_PASSTHRU", 0, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ODBC_BINMODE_RETURN", 1, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ODBC_BINMODE_CONVERT", 2, CONST_CS | CONST_PERSISTENT);
	/* Define Constants for different cursor options
	   these Constants are are defined in <sqlext.h>
	*/
	REGISTER_MAIN_LONG_CONSTANT("SQL_CUR_USE_IF_NEEDED", SQL_CUR_USE_IF_NEEDED, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("SQL_CUR_USE_ODBC", SQL_CUR_USE_ODBC, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("SQL_CUR_USE_DRIVER", SQL_CUR_USE_DRIVER, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("SQL_CUR_DEFAULT", SQL_CUR_DEFAULT, CONST_PERSISTENT | CONST_CS);
	return SUCCESS;
}


int PHP3_RINIT_UODBC(INIT_FUNC_ARGS)
{
	UODBC_TLS_VARS;

	UODBC_GLOBAL(PHP3_UODBC_MODULE).defConn = -1;
	UODBC_GLOBAL(PHP3_UODBC_MODULE).num_links = 
			UODBC_GLOBAL(PHP3_UODBC_MODULE).num_persistent;
#if PHP_31
		UODBC_GLOBAL(PHP3_UODBC_MODULE).defaultlrl = 4096;
		UODBC_GLOBAL(PHP3_UODBC_MODULE).defaultbinmode = 1;
#else
	if (cfg_get_long(ODBC_INI_VAR(defaultlrl), &UODBC_GLOBAL(PHP3_UODBC_MODULE).defaultlrl)
		== FAILURE){
		UODBC_GLOBAL(PHP3_UODBC_MODULE).defaultlrl = 4096;
	}
	if (cfg_get_long(ODBC_INI_VAR(defaultbinmode), &UODBC_GLOBAL(PHP3_UODBC_MODULE).defaultbinmode) == FAILURE){
		UODBC_GLOBAL(PHP3_UODBC_MODULE).defaultbinmode = 1;
	}
#endif
	return SUCCESS;
}

int PHP3_MSHUTDOWN_UODBC(SHUTDOWN_FUNC_ARGS)
{
	UODBC_TLS_VARS;

	SQLFreeEnv(UODBC_GLOBAL(PHP3_UODBC_MODULE).henv);
#ifdef THREAD_SAFE
	PHP3_TLS_THREAD_FREE(UODBC_GLOBALS);
	PHP3_MUTEX_LOCK(UODBC_MUTEX);
	numthreads--;
	if (!numthreads) {
		PHP3_TLS_PROC_SHUTDOWN(UODBC_TLS);
		PHP3_MUTEX_UNLOCK(UODBC_MUTEX);
		PHP3_MUTEX_FREE(UODBC_MUTEX);
		return SUCCESS;
	}
	PHP3_MUTEX_UNLOCK(UODBC_MUTEX);
#endif
	return SUCCESS;
}


void PHP3_INFO_UODBC(void)
{
	UODBC_TLS_VARS;

#if HAVE_SOLID
	php3_printf("Unified ODBC Support active (compiled with Solid)");
#elif HAVE_ADABAS
	php3_printf("Unified ODBC Support active (compiled with Adabas D)");
#elif HAVE_IODBC && !(WIN32|WINNT)
	php3_printf("Unified ODBC Support active (compiled with iODBC)");
#elif HAVE_VELOCIS
	php3_printf("Unified ODBC Support active (compiled with Velocis)");
#elif HAVE_DB2
	php3_printf("Unified ODBC Support active (compiled with IBM DB2)");
#elif WIN32|WINNT
	php3_printf("Unified ODBC Support active (compiled with win32 ODBC)");
#elif HAVE_EMPRESS
        PUTS("Unified ODBC Support active (compiled with Empress)");
#else
	php3_printf("Unified ODBC Support active (compiled with unknown library)");
#endif
	php3_printf("<BR>");
#if DEBUG
	php3_printf("default_db: %s<br>\n",UODBC_GLOBAL(PHP3_UODBC_MODULE).defDB);
	php3_printf("default_user: %s<br>\n",UODBC_GLOBAL(PHP3_UODBC_MODULE).defUser);
	php3_printf("default_pw: %s<br>\n",UODBC_GLOBAL(PHP3_UODBC_MODULE).defPW);
#endif
	php3_printf("allow_persistent: %d<br>\n",UODBC_GLOBAL(PHP3_UODBC_MODULE).allow_persistent);
	php3_printf("max_persistent: %d<br>\n",UODBC_GLOBAL(PHP3_UODBC_MODULE).max_persistent);
	php3_printf("max_links: %d<br>\n",UODBC_GLOBAL(PHP3_UODBC_MODULE).max_links);
}	 
	 
	 

/*
 * List management functions
 */

int UODBC_ADD_RESULT(HashTable *list,UODBC_RESULT *result)
{
	UODBC_TLS_VARS;
	return php3_list_insert(result, UODBC_GLOBAL(PHP3_UODBC_MODULE).le_result);
}

UODBC_RESULT *UODBC_GET_RESULT(HashTable *list, int ind)
{
	UODBC_RESULT *res;
	int type;
	UODBC_TLS_VARS;

	res = (UODBC_RESULT*)php3_list_find(ind, &type);
	if (!res || type != UODBC_GLOBAL(PHP3_UODBC_MODULE).le_result) {
		php3_error(E_WARNING, "Bad result index %d", ind);
		return NULL;
	}
	return res;
}

void UODBC_DEL_RESULT(HashTable *list, int ind)
{
	UODBC_RESULT *res;
	int type;
	UODBC_TLS_VARS;

	res = (UODBC_RESULT *)php3_list_find(ind, &type);
	if (!res || type != UODBC_GLOBAL(PHP3_UODBC_MODULE).le_result) {
		php3_error(E_WARNING,"Can't find result %d", ind);
		return;
	}
	php3_list_delete(ind);
}

#if 0
int UODBC_ADD_CONN(HashTable *list, HDBC conn)
{
	UODBC_TLS_VARS;
	return php3_list_insert(conn, UODBC_GLOBAL(PHP3_UODBC_MODULE).le_conn);
}

void uodbc_make_def_conn(HashTable *list)
{
	HDBC    new_conn;
	RETCODE rc;
	UODBC_TLS_VARS;

	SQLAllocConnect(UODBC_GLOBAL(PHP3_UODBC_MODULE).henv, &new_conn);
	rc = SQLConnect(new_conn, UODBC_GLOBAL(PHP3_UODBC_MODULE).defDB,
					SQL_NTS, UODBC_GLOBAL(PHP3_UODBC_MODULE).defUser, 
					SQL_NTS, UODBC_GLOBAL(PHP3_UODBC_MODULE).defPW, SQL_NTS);

	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
		UODBC_SQL_ERROR(new_conn, SQL_NULL_HSTMT, "SQLConnect"); 
	} else {
		UODBC_GLOBAL(PHP3_UODBC_MODULE).defConn = uodbc_add_conn(list, new_conn);
	}
}
#endif

UODBC_CONNECTION *UODBC_GET_CONN(HashTable *list, int ind)
{
	UODBC_CONNECTION *conn = NULL;
	int type;
	HashTable *plist;
	UODBC_TLS_VARS;
	
	plist = UODBC_GLOBAL(PHP3_UODBC_MODULE).resource_plist;

	conn = (UODBC_CONNECTION *)php3_list_find(ind, &type);
	if (conn && (type == UODBC_GLOBAL(PHP3_UODBC_MODULE).le_conn ||
				type == UODBC_GLOBAL(PHP3_UODBC_MODULE).le_pconn))
		return conn;

	conn = (UODBC_CONNECTION *)php3_plist_find(ind, &type);
	if (conn && (type == UODBC_GLOBAL(PHP3_UODBC_MODULE).le_conn ||
				type == UODBC_GLOBAL(PHP3_UODBC_MODULE).le_pconn))
		return conn;

	php3_error(E_WARNING,"Bad ODBC connection number (%d)", ind);
	return NULL;
}

#if HAVE_DB2
void UODBC_SQL_ERROR(SQLHANDLE conn, SQLHANDLE stmt, char *func)
#else
void UODBC_SQL_ERROR(HDBC conn, HSTMT stmt, char *func)
#endif
{
    char	state[6];     /* Not used */
	SDWORD	error;        /* Not used */
	char	errormsg[255];
	SWORD	errormsgsize; /* Not used */
	UODBC_TLS_VARS;
	
	SQLError(UODBC_GLOBAL(PHP3_UODBC_MODULE).henv, conn, stmt, state,
			 &error, errormsg, sizeof(errormsg)-1, &errormsgsize);
	if (func) {
		php3_error(E_WARNING, "SQL error: %s, SQL state %s in %s",
				   errormsg, state, func);
	} else {
		php3_error(E_WARNING, "SQL error: %s, SQL state %s",
				   errormsg, state);
	}
}

/* Main User Functions */
/* {{{ proto odbc_close_all(void)
   Close all ODBC connections */
UODBC_FUNCTION(close_all)
{
	void *ptr;
	int type;
	int i, nument = _php3_hash_next_free_element(list);
	UODBC_TLS_VARS;

	for (i = 1; i < nument; i++) {
		ptr = php3_list_find(i, &type);
		if (ptr && (type == UODBC_GLOBAL(PHP3_UODBC_MODULE).le_conn ||
				   type == UODBC_GLOBAL(PHP3_UODBC_MODULE).le_pconn)){
			php3_list_delete(i);
		}
	}
}
/* }}} */

void php3_uodbc_fetch_attribs(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	int         res_ind;
	UODBC_RESULT   *result;
	pval     *arg1, *arg2;
	UODBC_TLS_VARS;

	if (getParameters(ht, 2, &arg1, &arg2) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long(arg1);
    convert_to_long(arg2);
    
	res_ind = arg1->value.lval;

    if (res_ind){           
        if ((result = UODBC_GET_RESULT(list, res_ind)) == NULL){
            RETURN_FALSE;
        }
        if (mode)
            result->longreadlen = arg2->value.lval;	
        else
            result->binmode = arg2->value.lval;
	} else {
        if (mode)
            UODBC_GLOBAL(PHP3_UODBC_MODULE).defaultlrl = arg2->value.lval;
        else
            UODBC_GLOBAL(PHP3_UODBC_MODULE).defaultbinmode = arg2->value.lval;
    }
    
	RETURN_TRUE
}

/* {{{ proto odbc_binmode(int result_id, int mode)
   Handle binary column data */
UODBC_FUNCTION(binmode)
{
	php3_uodbc_fetch_attribs(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);
}
/* }}} */

/* {{{ proto odbc_longreadlen(int result_id, int length)
   Handle LONG columns */
UODBC_FUNCTION(longreadlen)
{
	php3_uodbc_fetch_attribs(INTERNAL_FUNCTION_PARAM_PASSTHRU,1);
}
/* }}} */

int UODBC_BINDCOLS(UODBC_RESULT *result)
{
    int i;
    SWORD       colnamelen; /* Not used */
	SDWORD      displaysize;
	UODBC_TLS_VARS;
	
    result->values = (UODBC_RESULT_VALUE *)
		emalloc(sizeof(UODBC_RESULT_VALUE)*result->numcols);

    if (result->values == NULL){
        php3_error(E_WARNING, "Out of memory");
        SQLFreeStmt(result->stmt, SQL_DROP);
        return 0;
    }

    result->longreadlen = UODBC_GLOBAL(PHP3_UODBC_MODULE).defaultlrl;
    result->binmode = UODBC_GLOBAL(PHP3_UODBC_MODULE).defaultbinmode;
        
    for(i = 0; i < result->numcols; i++){
        SQLColAttributes(result->stmt, (UWORD)(i+1), SQL_COLUMN_NAME,
                         result->values[i].name,
                         sizeof(result->values[i].name),
                         &colnamelen,
                         0);
		SQLColAttributes(result->stmt, (UWORD)(i+1), SQL_COLUMN_TYPE,
							NULL, 0, NULL, &result->values[i].coltype);
		
		/* Don't bind LONG / BINARY columns, so that fetch behaviour can
           be controlled by odbc_binmode() / odbc_longreadlen()
		 */
		
		switch(result->values[i].coltype){
            case SQL_BINARY:
            case SQL_VARBINARY:
			case SQL_LONGVARBINARY:
			case SQL_LONGVARCHAR:
				result->values[i].value = NULL;
				break;
				
#if HAVE_ADABAS
			case SQL_TIMESTAMP:
				result->values[i].value = (char *)emalloc(27);
				SQLBindCol(result->stmt, (UWORD)(i+1), SQL_C_CHAR, result->values[i].value,
							27, &result->values[i].vallen);
				break;
#endif /* HAVE_ADABAS */
			default:
				SQLColAttributes(result->stmt, (UWORD)(i+1), SQL_COLUMN_DISPLAY_SIZE,
									NULL, 0, NULL, &displaysize);
				result->values[i].value = (char *)emalloc(displaysize + 1);
				SQLBindCol(result->stmt, (UWORD)(i+1), SQL_C_CHAR, result->values[i].value,
							displaysize + 1, &result->values[i].vallen);
				break;
		}
    }
    return 1;
}

/* {{{ proto odbc_prepare(int connection_id, string query)
   Prepares a statement for execution */
UODBC_FUNCTION(prepare)
{
	pval     *arg1, *arg2;
	int         conn;
	char        *query;
	UODBC_RESULT   *result=NULL;
	UODBC_CONNECTION *curr_conn=NULL;
	RETCODE rc;

	if (getParameters(ht, 2, &arg1, &arg2) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_string(arg2);
	conn = arg1->value.lval;
	query = arg2->value.str.val;

	if ((curr_conn = UODBC_GET_CONN(list, conn)) == NULL){
		RETURN_FALSE;
	}

#if 0
	_php3_stripslashes(query,NULL);
#endif

	result = (UODBC_RESULT *)emalloc(sizeof(UODBC_RESULT));
	if (result == NULL){
		php3_error(E_WARNING, "Out of memory");
		RETURN_FALSE;
	}
	
	result->numparams = 0;
	
	rc = SQLAllocStmt(curr_conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE){
		efree(result);
		php3_error(E_WARNING, "SQLAllocStmt error 'Invalid Handle' in php3_uodbc_prepare");
		RETURN_FALSE;
	}

	if (rc == SQL_ERROR){
		UODBC_SQL_ERROR(curr_conn->hdbc, SQL_NULL_HSTMT, "SQLAllocStmt");
		efree(result);
		RETURN_FALSE;
	}

	if ((rc = SQLPrepare(result->stmt, query, SQL_NTS)) != SQL_SUCCESS){
		UODBC_SQL_ERROR(curr_conn->hdbc, result->stmt, "SQLPrepare");
		SQLFreeStmt(result->stmt, SQL_DROP);
		RETURN_FALSE;
	}
	
	SQLNumParams(result->stmt, &(result->numparams));
    SQLNumResultCols(result->stmt, &(result->numcols));

	if (result->numcols > 0){
        if (!UODBC_BINDCOLS(result)){
			efree(result);
            RETURN_FALSE;
		}
	} else {
		result->values = NULL;
	}
	result->conn_ptr = curr_conn;
	result->fetched = 0;
	RETURN_LONG(UODBC_ADD_RESULT(list, result));	
}
/* }}} */

/*
 * Execute prepared SQL statement. Supports only input parameters.
 */
/* {{{ proto odbc_execute(int result_id [, array parameters_array])
   Execute a prepared statement */
extern UODBC_FUNCTION(execute)
{ 
    pval *arg1, *arg2, arr, *tmp;
    typedef struct params_t {
		SDWORD vallen;
		int fp;
	} params_t;
	params_t *params = NULL;
	char *filename;
   	SWORD sqltype, scale, nullable;
	UDWORD precision;
   	UODBC_RESULT   *result=NULL;
	int res_ind, numArgs, i, ne;
	RETCODE rc;
	
	numArgs = ARG_COUNT(ht);
	if (numArgs == 1){
		if (getParameters(ht, 1, &arg1) == FAILURE)
			WRONG_PARAM_COUNT;
	} else {
		if (getParameters(ht, 2, &arg1, &arg2) == FAILURE)
			WRONG_PARAM_COUNT;

        arr = *arg2;
        if (arr.type != IS_ARRAY) {
            php3_error(E_WARNING, "No array passed to odbc_execute()");
            return;
        }
    }
    
	convert_to_long(arg1);
	res_ind = arg1->value.lval;
	
	/* check result */
	if ((result = UODBC_GET_RESULT(list, res_ind)) == NULL){
		RETURN_FALSE;
	}
	
	if (result->numparams > 0 && numArgs == 1) {
		php3_error(E_WARNING, "No parameters to SQL statement given");
		RETURN_FALSE;
	}

    if (result->numparams > 0){
		if ((ne = _php3_hash_num_elements(arr.value.ht)) < result->numparams){
			php3_error(E_WARNING,"Not enough parameters (%d should be %d) given",
					   ne, result->numparams);
			RETURN_FALSE;
		}

        pval_copy_constructor(arg2);
        _php3_hash_internal_pointer_reset(arr.value.ht);
        params = (params_t *)emalloc(sizeof(params_t) * result->numparams);
		
		for (i = 1; i <= result->numparams; i++) {
            if (_php3_hash_get_current_data(arr.value.ht, (void **) &tmp) == FAILURE) {
                php3_error(E_WARNING,"Error getting parameter");
                SQLFreeStmt(result->stmt,SQL_RESET_PARAMS);
				efree(params);
                RETURN_FALSE;
            }
            convert_to_string(tmp);
			if (tmp->type != IS_STRING) {
				php3_error(E_WARNING,"Error converting parameter");
				SQLFreeStmt(result->stmt, SQL_RESET_PARAMS);
				_php3_hash_destroy(arr.value.ht);
				efree(arr.value.ht);
				efree(params);
				RETURN_FALSE;
			}
			
            SQLDescribeParam(result->stmt, (UWORD)i, &sqltype, &precision,
							 &scale, &nullable);
			params[i-1].vallen = tmp->value.str.len;
			params[i-1].fp = -1;


			if (tmp->value.str.val[0] == '\'') {
				filename = &tmp->value.str.val[1];
				filename[tmp->value.str.len - 2] = '\0';

                if ((params[i-1].fp = open(filename,O_RDONLY)) == -1) {
					php3_error(E_WARNING,"Can't open file %s", filename);
					SQLFreeStmt(result->stmt, SQL_RESET_PARAMS);
					for (i = 0; i < result->numparams; i++) {
						if (params[i].fp != -1) {
							close(params[i].fp);
						}
					}
					_php3_hash_destroy(arr.value.ht);
					efree(arr.value.ht);
					efree(params);
					RETURN_FALSE;
				}

				params[i-1].vallen = SQL_LEN_DATA_AT_EXEC(0);

				rc = SQLBindParameter(result->stmt, (UWORD)i, SQL_PARAM_INPUT,
									  SQL_C_BINARY, sqltype, precision, scale,
									  (void *)params[i-1].fp, 0,
									  &params[i-1].vallen);
			} else {
				/*if (IS_SQL_BINARY(sqltype)){
	   				php3_error(E_WARNING,"No Filename for binary parameter");
					SQLFreeStmt(result->stmt,SQL_RESET_PARAMS);
					_php3_hash_destroy(arr.value.ht);
					efree(arr.value.ht);
					efree(params);
					RETURN_FALSE;
				}
*/
				rc = SQLBindParameter(result->stmt, (UWORD)i, SQL_PARAM_INPUT,
									  SQL_C_CHAR, sqltype, precision, scale,
									  tmp->value.str.val, 0,
									  &params[i-1].vallen);
			}
			_php3_hash_move_forward(arr.value.ht);
		}
	}
	/* Close cursor, needed for doing multiple selects */
	rc = SQLFreeStmt(result->stmt, SQL_CLOSE);

	if (rc == SQL_ERROR){
		UODBC_SQL_ERROR(result->conn_ptr->hdbc, result->stmt, "SQLFreeStmt");	
	}

	rc = SQLExecute(result->stmt);

	result->fetched = 0;
	if (rc == SQL_NEED_DATA){
		char buf[4096];
		int fp, nbytes;
		while(rc == SQL_NEED_DATA){
			rc = SQLParamData(result->stmt, (PTR FAR *)&fp);
			if (rc == SQL_NEED_DATA){
				while((nbytes = read(fp, &buf, 4096)) > 0)
					SQLPutData(result->stmt,(UCHAR FAR*) &buf, nbytes);
			}
		}
	} else {
		if (rc != SQL_SUCCESS){
			UODBC_SQL_ERROR(result->conn_ptr->hdbc, result->stmt, "SQLExecute");
			RETVAL_FALSE;
		}
	}	
	
	if (result->numparams > 0){
		SQLFreeStmt(result->stmt, SQL_RESET_PARAMS);
		for(i = 0; i < result->numparams; i++){
			if (params[i].fp != -1)
				close(params[i].fp);
		}
		_php3_hash_destroy(arr.value.ht);
		efree(arr.value.ht);
		efree(params);
	}

	if (rc == SQL_SUCCESS){
		RETVAL_TRUE;
	}
}
/* }}} */

/* odbc_cursor simply returns a cursor name for the given stmt
 * Adabas automagically generates cursor names, other drivers may not
 */
/* {{{ proto odbc_cursor(int result_id)
   Get cursor name */
UODBC_FUNCTION(cursor)
{
	pval     *arg1;
	int			res_ind;
	SWORD		len, max_len;
	char		*cursorname;
   	UODBC_RESULT *result;
	RETCODE 	rc;

	if (getParameters(ht, 1, &arg1) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	res_ind = arg1->value.lval;
	
	/* check result */
	if ((result = UODBC_GET_RESULT(list, res_ind)) == NULL) {
		RETURN_FALSE;
	}
	rc = SQLGetInfo(result->conn_ptr->hdbc,SQL_MAX_CURSOR_NAME_LEN,
					(void *)&max_len,0,&len);
	if (rc != SQL_SUCCESS){
		RETURN_FALSE;
	}
	
	if (max_len > 0){
		cursorname = emalloc(max_len + 1);
		if (cursorname == NULL){
			php3_error(E_WARNING,"Out of memory");
			RETURN_FALSE;
		}
		rc = SQLGetCursorName(result->stmt,cursorname,(SWORD)max_len,&len);
		if (rc != SQL_SUCCESS){
			char    state[6];     /* Not used */
	 		SDWORD  error;        /* Not used */
			char    errormsg[255];
			SWORD   errormsgsize; /* Not used */
			UODBC_TLS_VARS;

			SQLError(UODBC_GLOBAL(PHP3_UODBC_MODULE).henv, result->conn_ptr->hdbc,
						result->stmt, state, &error, errormsg,
						sizeof(errormsg)-1, &errormsgsize);
			if (!strncmp(state,"S1015",5)){
				sprintf(cursorname,"php3_curs_%d", (int)result->stmt);
				if (SQLSetCursorName(result->stmt,cursorname,SQL_NTS) != SQL_SUCCESS){
					UODBC_SQL_ERROR(result->conn_ptr->hdbc,result->stmt,
										"SQLSetCursorName");
					RETVAL_FALSE;
				} else {
					RETVAL_STRING(cursorname,1);
				}
			} else {
				php3_error(E_WARNING, "SQL error: %s, SQL state %s", errormsg, state);
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

/* {{{ proto odbc_exec(int connection_id, string query)
   Prepare and execute an SQL statement */
UODBC_FUNCTION(exec)
{
	pval 	*arg1, *arg2;
	int         conn;
	char        *query;
	UODBC_RESULT   *result=NULL;
	UODBC_CONNECTION *curr_conn=NULL;
	RETCODE     rc;
#if HAVE_SQL_EXTENDED_FETCH
	UDWORD      scrollopts;
#endif

	if (getParameters(ht, 2, &arg1, &arg2) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_string(arg2);
	conn = arg1->value.lval;
	query = arg2->value.str.val;
	
	if ((curr_conn = UODBC_GET_CONN(list, conn)) == NULL){
		RETURN_FALSE;
	}

#if 0
	_php3_stripslashes(query,NULL);
#endif
	
	result = (UODBC_RESULT *)emalloc(sizeof(UODBC_RESULT));
	if (result == NULL){
		php3_error(E_WARNING, "Out of memory");
		RETURN_FALSE;
	}

	rc = SQLAllocStmt(curr_conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE){
		php3_error(E_WARNING, "SQLAllocStmt error 'Invalid Handle' in PHP3_UODBC_DO");
		efree(result);
		RETURN_FALSE;
	}

	if (rc == SQL_ERROR){
		UODBC_SQL_ERROR(curr_conn->hdbc, SQL_NULL_HSTMT, "SQLAllocStmt");
		efree(result);
		RETURN_FALSE;
	}
	
#if HAVE_SQL_EXTENDED_FETCH
	/* Solid doesn't have ExtendedFetch, if DriverManager is used, get Info,
	   whether Driver supports ExtendedFetch */
	rc = SQLGetInfo(curr_conn->hdbc, SQL_FETCH_DIRECTION, (void *) &scrollopts, sizeof(scrollopts), NULL);
	if (rc == SQL_SUCCESS){
		if ((result->fetch_abs = (scrollopts & SQL_FD_FETCH_ABSOLUTE))){
			/* Try to set CURSOR_TYPE to dynamic. Driver will replace this with other
			   type if not possible.
			 */
			if (SQLSetStmtOption(result->stmt, SQL_CURSOR_TYPE, SQL_CURSOR_DYNAMIC)
				== SQL_ERROR){
				UODBC_SQL_ERROR(curr_conn->hdbc, result->stmt, " SQLSetStmtOption");
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
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		/* XXX FIXME we should really check out SQLSTATE with SQLError
		 * in case rc is SQL_SUCCESS_WITH_INFO here.
		 */
		UODBC_SQL_ERROR(curr_conn->hdbc, result->stmt, "SQLExecDirect"); 
		SQLFreeStmt(result->stmt, SQL_DROP);
		efree(result);
		RETURN_FALSE;
	}

	SQLNumResultCols(result->stmt, &(result->numcols));
	
	/* For insert, update etc. cols == 0 */
	if (result->numcols > 0){
        if (!UODBC_BINDCOLS(result)){
			efree(result);
            RETURN_FALSE;
		}
	} else {
		result->values = NULL;
	}
	result->conn_ptr = curr_conn;
	result->fetched = 0;
	RETURN_LONG(UODBC_ADD_RESULT(list, result));
}
/* }}} */

/* {{{ proto odbc_fetch_into(int result_id [, int rownumber], array result_array)
   Fetch one result row into an array */ 
UODBC_FUNCTION(fetch_into)
{
	int         res_ind, numArgs, i;
	UODBC_RESULT   *result;
	RETCODE     rc;
    SWORD sql_c_type;
	char *buf = NULL;
#if HAVE_SQL_EXTENDED_FETCH
	UDWORD      crow;
	UWORD       RowStatus[1];
	SDWORD      rownum = -1;
	pval     *arg1, *arg2, *arr, tmp;
	
	numArgs = ARG_COUNT(ht);

	switch(numArgs){
		case 2:
			if (getParameters(ht, 2, &arg1, &arr) == FAILURE)
				WRONG_PARAM_COUNT;
			break;
		case 3:
			if (getParameters(ht, 3, &arg1, &arg2, &arr) == FAILURE)
				WRONG_PARAM_COUNT;
			convert_to_long(arg2);
			rownum = arg2->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	if (!ParameterPassedByReference(ht, numArgs)){
		php3_error(E_WARNING, "Array not passed by reference in call to odbc_fetch_into()");
		RETURN_FALSE;
	}
#else
	pval     *arg1, *arr, tmp;

	numArgs = ARG_COUNT(ht);

	if (numArgs != 2 || getParameters(ht, 2, &arg1, &arr) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (!ParameterPassedByReference(ht, numArgs)){
		php3_error(E_WARNING, "Array not passed by reference in call to odbc_fetch_into()");
		RETURN_FALSE;
	}
#endif				
	
	convert_to_long(arg1);
	res_ind = arg1->value.lval;

	/* check result */
	if ((result = UODBC_GET_RESULT(list, res_ind)) == NULL){
		RETURN_FALSE;
	}

	if (result->numcols == 0) {
		php3_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
	
	if (arr->type != IS_ARRAY){
		if (array_init(arr) == FAILURE){
			php3_error(E_WARNING, "Can't convert to type Array");
			RETURN_FALSE;
		}
	}

#if HAVE_SQL_EXTENDED_FETCH
	if (result->fetch_abs){
		if (rownum > 0)
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_ABSOLUTE,rownum,&crow,RowStatus);
		else
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
	}else
#endif
		
		rc = SQLFetch(result->stmt);

	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
		RETURN_FALSE;

#if HAVE_SQL_EXTENDED_FETCH
	if (rownum > 0 && result->fetch_abs)
		result->fetched = rownum;
	else
#endif
		result->fetched++;

	for (i = 0; i < result->numcols; i++) {
		tmp.type = IS_STRING;
		tmp.value.str.len = 0;
        sql_c_type = SQL_C_CHAR;
       
        switch(result->values[i].coltype){
            case SQL_BINARY:
            case SQL_VARBINARY:
            case SQL_LONGVARBINARY:
                if (result->binmode <= 0){
                    tmp.value.str.val = empty_string;
                    break;
                }
                if (result->binmode == 1) sql_c_type = SQL_C_BINARY; 
            case SQL_LONGVARCHAR:
                if (IS_SQL_LONG(result->values[i].coltype) && 
                   result->longreadlen <= 0){
                    tmp.value.str.val = empty_string;
                    break;
                }
        
                if (buf == NULL) buf = emalloc(result->longreadlen + 1);
                rc = SQLGetData(result->stmt, (UWORD)(i + 1),sql_c_type,
								buf, result->longreadlen + 1, &result->values[i].vallen);

                if (rc == SQL_ERROR) {
					UODBC_SQL_ERROR(result->conn_ptr->hdbc, result->stmt, "SQLGetData");
					efree(buf);
					RETURN_FALSE;
				}
				if (rc == SQL_SUCCESS_WITH_INFO){
					tmp.value.str.len = result->longreadlen;
				} else if (result->values[i].vallen == SQL_NULL_DATA){
					tmp.value.str.val = empty_string;
					break;
				} else {
					tmp.value.str.len = result->values[i].vallen;
				}
				tmp.value.str.val = estrndup(buf, tmp.value.str.len);
				break;

			default:
				if (result->values[i].vallen == SQL_NULL_DATA){
					tmp.value.str.val = empty_string;
					break;
				}
				tmp.value.str.len = result->values[i].vallen;
				tmp.value.str.val = estrndup(result->values[i].value,tmp.value.str.len);
				break;
		}
		_php3_hash_index_update(arr->value.ht, i, (void *) &tmp, sizeof(pval), NULL);
	}
	if (buf) efree(buf);
	RETURN_LONG(result->numcols);	
}
/* }}} */

#if HAVE_SOLID
void php3_solid_fetch_prev(INTERNAL_FUNCTION_PARAMETERS)
{
	int         res_ind;
	UODBC_RESULT   *result;
	RETCODE     rc;
	pval     *arg1;
	
	if (getParameters(ht, 1, &arg1) == FAILURE)
		            WRONG_PARAM_COUNT;
	
	convert_to_long(arg1);
	res_ind = arg1->value.lval;

	/* check result */
	if ((result = UODBC_GET_RESULT(list, res_ind)) == NULL) {
		RETURN_FALSE;
	}

	if (result->numcols == 0) {
		php3_error(E_WARNING, "No tuples available at this result index");
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

/* {{{ proto odbc_fetch_row(int result_id [, int row_number])
   Fetch a row */
UODBC_FUNCTION(fetch_row)
{
	int         res_ind, numArgs;
	SDWORD      rownum = 1;
	UODBC_RESULT   *result;
	RETCODE     rc;
	pval		*arg1, *arg2;
#if HAVE_SQL_EXTENDED_FETCH
	UDWORD      crow;
	UWORD       RowStatus[1];
#endif

	numArgs = ARG_COUNT(ht);
	if (numArgs ==  1){
		if (getParameters(ht, 1, &arg1) == FAILURE)
			WRONG_PARAM_COUNT;
	} else {
		if (getParameters(ht, 2, &arg1, &arg2) == FAILURE)
			WRONG_PARAM_COUNT;
		convert_to_long(arg2);
		rownum = arg2->value.lval;
	}
	
	convert_to_long(arg1);
	res_ind = arg1->value.lval;
	
	/* check result */
	if ((result = UODBC_GET_RESULT(list, res_ind)) == NULL) {
		RETURN_FALSE;
	}
	
	if (result->numcols == 0) {
		php3_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}

#if HAVE_SQL_EXTENDED_FETCH
    if (result->fetch_abs){
		if (numArgs > 1)
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_ABSOLUTE,rownum,&crow,RowStatus);
		else
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
	}else
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

/* {{{ proto odbc_result(int result_id, mixed field)
   Get result data */ 
UODBC_FUNCTION(result)
{
	char        *field;
	int         res_ind;
	int         field_ind;
	SWORD 		sql_c_type = SQL_C_CHAR;
	UODBC_RESULT   *result;
	int         i = 0;
	RETCODE     rc;
	SDWORD		fieldsize;
	pval		*arg1, *arg2;
#if HAVE_SQL_EXTENDED_FETCH
	UDWORD      crow;
	UWORD       RowStatus[1];
#endif
#if !defined(COMPILE_DL) && defined(THREAD_SAFE)
	TLS_VARS;
#endif

	field_ind = -1;
	field = NULL;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2 , &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(arg1);
	res_ind = arg1->value.lval;

	if (arg2->type == IS_STRING){
		field = arg2->value.str.val;
	} else {
		convert_to_long(arg2);
		field_ind = arg2->value.lval - 1;
	}
	
	/* check result */
	if ((result = UODBC_GET_RESULT(list, res_ind)) == NULL) {
		RETURN_FALSE;
	}
	
	if ((result->numcols == 0)){
		php3_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
	
	/* get field index if the field parameter was a string */
	if (field != NULL){
		for(i = 0; i < result->numcols; i++){
			if (!strcasecmp(result->values[i].name, field)){
				field_ind = i;
				break;
			}
		}

		if (field_ind < 0){
			php3_error(E_WARNING, "Field %s not found", field);
			RETURN_FALSE;
		}
	} else {
		/* check for limits of field_ind if the field parameter was an int */
		if (field_ind >= result->numcols || field_ind < 0){
			php3_error(E_WARNING, "Field index is larger than the number of fields");
			RETURN_FALSE;
		}
	}

	if (result->fetched == 0){
		/* User forgot to call odbc_fetchrow(), let's do it here */
#if HAVE_SQL_EXTENDED_FETCH
		if (result->fetch_abs)
			rc = SQLExtendedFetch(result->stmt, SQL_FETCH_NEXT, 1, &crow,RowStatus);
		else
#endif
			rc = SQLFetch(result->stmt);

		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
			RETURN_FALSE;
		
		result->fetched++;
	}

	switch(result->values[field_ind].coltype){
        case SQL_BINARY:
        case SQL_VARBINARY:
        case SQL_LONGVARBINARY:
            if (result->binmode <= 1) sql_c_type = SQL_C_BINARY;
            if (result->binmode <= 0) break; 
        case SQL_LONGVARCHAR:
            if (IS_SQL_LONG(result->values[field_ind].coltype)){
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
            if (!field){
                php3_error(E_WARNING, "Out of memory");
                RETURN_FALSE;
            }
		/* SQLGetData will truncate CHAR data to fieldsize - 1 bytes and append \0.
		   For binary data it is truncated to fieldsize bytes. 
		 */
            rc = SQLGetData(result->stmt, (UWORD)(field_ind + 1), sql_c_type,
                            field, fieldsize, &result->values[field_ind].vallen);
            
            if (rc == SQL_ERROR) {
                UODBC_SQL_ERROR(result->conn_ptr->hdbc, result->stmt, "SQLGetData");
                efree(field);
                RETURN_FALSE;
            }
            
            if (result->values[field_ind].vallen == SQL_NULL_DATA || rc == SQL_NO_DATA_FOUND){
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
			if (result->values[field_ind].vallen == SQL_NULL_DATA){
				RETURN_FALSE;
			} else {
				RETURN_STRINGL(result->values[field_ind].value, result->values[field_ind].vallen, 1);
			}
			break;
	}

/* If we come here, output unbound LONG and/or BINARY column data to the client */

	
	/* We emalloc 1 byte more for SQL_C_CHAR (trailing \0) */
	fieldsize = (sql_c_type == SQL_C_CHAR) ? 4096 : 4095;
    if ((field = emalloc(fieldsize)) == NULL){
        php3_error(E_WARNING,"Out of memory");
        RETURN_FALSE;
    }
    
	/* Call SQLGetData() until SQL_SUCCESS is returned */
	while (1) {
        rc = SQLGetData(result->stmt, (UWORD)(field_ind + 1),sql_c_type,
                            field, fieldsize, &result->values[field_ind].vallen);

		if (rc == SQL_ERROR) {
			UODBC_SQL_ERROR(result->conn_ptr->hdbc, result->stmt, "SQLGetData");
            efree(field);
			RETURN_FALSE;
		}
        
        if (result->values[field_ind].vallen == SQL_NULL_DATA){
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

/* {{{ proto odbc_result_all(int result_id [, string format])
   Print result as HTML table */
UODBC_FUNCTION(result_all)
{
	char *buf = NULL;
	int         res_ind, numArgs;
	UODBC_RESULT   *result;
	int         i;
	RETCODE     rc;
	pval     *arg1, *arg2;
	SWORD sql_c_type;
#if HAVE_SQL_EXTENDED_FETCH
	UDWORD      crow;
	UWORD       RowStatus[1];
#endif
#if !defined(COMPILE_DL) && defined(THREAD_SAFE)
	TLS_VARS
#endif

	numArgs = ARG_COUNT(ht);
	if (numArgs ==  1){
		if (getParameters(ht, 1, &arg1) == FAILURE)
			WRONG_PARAM_COUNT;
	} else {
		if (getParameters(ht, 2, &arg1, &arg2) == FAILURE)
			WRONG_PARAM_COUNT;
	}
				
	convert_to_long(arg1);
	res_ind = arg1->value.lval;
	
	/* check result */
	if ((result = UODBC_GET_RESULT(list, res_ind)) == NULL) {
		RETURN_FALSE;
	}
	
	if (result->numcols == 0){
		php3_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
#if HAVE_SQL_EXTENDED_FETCH
	if (result->fetch_abs)
		rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
	else
#endif	
		rc = SQLFetch(result->stmt);

	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
		php3_printf("<h2>No rows found</h2>\n");
		RETURN_LONG(0);
	}
	
	/* Start table tag */
	if (numArgs == 1){
		php3_printf("<table><tr>");
	} else {
		convert_to_string(arg2);	
		php3_printf("<table %s ><tr>",arg2->value.str.val); 
	}
	
	for(i = 0; i < result->numcols; i++)
		php3_printf("<th>%s</th>", result->values[i].name);

	php3_printf("</tr>\n");

	while(rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO){
		result->fetched++;
		php3_printf("<tr>");
		for(i = 0; i < result->numcols; i++){
            sql_c_type = SQL_C_CHAR;
            switch(result->values[i].coltype){
                case SQL_BINARY:
                case SQL_VARBINARY:
                case SQL_LONGVARBINARY:
                    if (result->binmode <= 0){
                        php3_printf("<td>Not printable</td>");
                        break;
                    }
                    if (result->binmode <= 1) sql_c_type = SQL_C_BINARY; 
                case SQL_LONGVARCHAR:
                    if (IS_SQL_LONG(result->values[i].coltype) && 
                       result->longreadlen <= 0){
                        php3_printf("<td>Not printable</td>"); 
                        break;
                    }
        
                    if (buf == NULL) buf = emalloc(result->longreadlen);
                    
                    rc = SQLGetData(result->stmt, (UWORD)(i + 1),sql_c_type,
                                    buf, result->longreadlen, &result->values[i].vallen);
                    
                    php3_printf("<td>");

                    if (rc == SQL_ERROR) {
                        UODBC_SQL_ERROR(result->conn_ptr->hdbc, result->stmt, "SQLGetData");
                        php3_printf("</td></tr></table>");
                        efree(buf);
                        RETURN_FALSE;
                    }
                    if (rc == SQL_SUCCESS_WITH_INFO)
                        php3_printf(buf,result->longreadlen);
                    else if (result->values[i].vallen == SQL_NULL_DATA){
						php3_printf("&nbsp;</td>");
						break;
                    }
		    else
		    {
                    	php3_printf(buf, result->values[i].vallen);
		    }
                    php3_printf("</td>");
                    break;

                default:
                    if (result->values[i].vallen == SQL_NULL_DATA){
                        php3_printf("<td>&nbsp;</td>");
                    } else {
                        php3_printf("<td>%s</td>", result->values[i].value);
                    }
                    break;
            }
		}
   		php3_printf("</tr>\n");

#if HAVE_SQL_EXTENDED_FETCH
		if (result->fetch_abs)
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
		else
#endif
			rc = SQLFetch(result->stmt);		
	}
	php3_printf("</table>\n");
    if (buf) efree(buf);
	RETURN_LONG(result->fetched);
}
/* }}} */

/* {{{ proto odbc_free_result(int result_id)
   Free resources associated with a result */
UODBC_FUNCTION(free_result)
{
	pval *arg1;
	
	if ( getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	UODBC_DEL_RESULT(list, arg1->value.lval);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto odbc_connect(string DSN, string user, string password [, int cursor_option])
   Connect to a datasource */
UODBC_FUNCTION(connect)
{
	PHP3_UODBC_DO_CONNECT(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto odbc_connect(string DSN, string user, string password [, int cursor_option])
   Establish a persistant connection to a datasource */
UODBC_FUNCTION(pconnect)
{
	PHP3_UODBC_DO_CONNECT(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
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
void PHP3_UODBC_DO_CONNECT(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	char    *db = NULL;
	char    *uid = NULL;
	char    *pwd = NULL;
	pval *arg1, *arg2, *arg3, *arg4;
	UODBC_CONNECTION *db_conn;
	RETCODE rc;
	list_entry *index_ptr;
	char *hashed_details;
	int hashed_len, len, id, cur_opt;
	int type;
	UODBC_TLS_VARS;

	UODBC_GLOBAL(PHP3_UODBC_MODULE).resource_list = list;
	UODBC_GLOBAL(PHP3_UODBC_MODULE).resource_plist = plist;

	/*  Now an optional 4th parameter specifying the cursor type
	 *  defaulting to the cursors default
	 */
	switch(ARG_COUNT(ht)) {
		case 3:	
			if (getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			/* Use Default: Probably a better way to do this */
			cur_opt = SQL_CUR_DEFAULT;
			break;
		case 4:
			if (getParameters(ht, 4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_long(arg4);
			cur_opt = arg4->value.lval;

			/* Confirm the cur_opt range */
			if (! (cur_opt == SQL_CUR_USE_IF_NEEDED || 
				cur_opt == SQL_CUR_USE_ODBC || 
				cur_opt == SQL_CUR_USE_DRIVER || 
				cur_opt == SQL_CUR_DEFAULT) ) {
				php3_error(E_WARNING, "uODBC: Invalid Cursor type (%d)", cur_opt);
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	convert_to_string(arg1);
	convert_to_string(arg2);
	convert_to_string(arg3);

	db = arg1->value.str.val;
	uid = arg2->value.str.val;
	pwd = arg3->value.str.val;

	if (UODBC_GLOBAL(PHP3_UODBC_MODULE).allow_persistent <= 0) {
		persistent = 0;
	}

	if (UODBC_GLOBAL(PHP3_UODBC_MODULE).max_links != -1 &&
		UODBC_GLOBAL(PHP3_UODBC_MODULE).num_links >=
		UODBC_GLOBAL(PHP3_UODBC_MODULE).max_links) {
		php3_error(E_WARNING, "uODBC: Too many open links (%d)",
		UODBC_GLOBAL(PHP3_UODBC_MODULE).num_links);
		RETURN_FALSE;
	}

	/* the user requested a persistent connection */
	if (persistent && 
			UODBC_GLOBAL(PHP3_UODBC_MODULE).max_persistent != -1 &&
			UODBC_GLOBAL(PHP3_UODBC_MODULE).num_persistent >=
			UODBC_GLOBAL(PHP3_UODBC_MODULE).max_persistent) {
		php3_error(E_WARNING,"uODBC: Too many open persistent links (%d)",
					UODBC_GLOBAL(PHP3_UODBC_MODULE).num_persistent);
		RETURN_FALSE;
	}

	len = strlen(db) + strlen(uid) + strlen(pwd) + strlen(UODBC_NAME) + 5; 
	hashed_details = emalloc(len);

	if (hashed_details == NULL) {
		php3_error(E_WARNING, "Out of memory");
		RETURN_FALSE;
	}

	hashed_len = _php3_sprintf(hashed_details, "%s_%s_%s_%s_%d", UODBC_NAME, db, uid, pwd, cur_opt);

	/* FIXME the idea of checking to see if our connection is already persistent
		is good, but it adds a lot of overhead to non-persistent connections.  We
		should look and see if we can fix that somehow */
	/* try to find if we already have this link in our persistent list,
	 * no matter if it is to be persistent or not
	 */

	if ((persistent || (_php3_hash_find(list, hashed_details, hashed_len + 1,
		  (void **) &index_ptr) == FAILURE || !php3_list_find((int) index_ptr->ptr, &type))) &&
		  _php3_hash_find(plist, hashed_details, hashed_len + 1,
		  (void **) &index_ptr) == FAILURE) {
		/* the link is not in the persistent list */
		list_entry new_le, new_index_ptr;

		if (persistent) {
			db_conn = (UODBC_CONNECTION *)malloc(sizeof(UODBC_CONNECTION));
		} else {
			db_conn = (UODBC_CONNECTION *)emalloc(sizeof(UODBC_CONNECTION));
		}

		SQLAllocConnect(UODBC_GLOBAL(PHP3_UODBC_MODULE).henv, &db_conn->hdbc);
#if HAVE_SOLID
		SQLSetConnectOption(db_conn->hdbc, SQL_TRANSLATE_OPTION,
							SQL_SOLID_XLATOPT_NOCNV);
#endif
#if HAVE_OPENLINK
		{
			char dsnbuf[300];
			short dsnbuflen;

			rc = SQLDriverConnect(db_conn->hdbc, NULL, db, SQL_NTS,
								  dsnbuf, sizeof(dsnbuf)-1, &dsnbuflen,
								  SQL_DRIVER_COMPLETE);
		}
#else
		if(cur_opt != SQL_CUR_DEFAULT){
			rc = SQLSetConnectOption(db_conn->hdbc, SQL_ODBC_CURSORS, cur_opt);
			if (rc != SQL_SUCCESS ) {  /* && rc != SQL_SUCCESS_WITH_INFO ? */
				UODBC_SQL_ERROR(db_conn->hdbc, SQL_NULL_HSTMT, "SQLSetConnectOption");
				SQLFreeConnect(db_conn->hdbc);
				if (persistent)
					free(db_conn);
				else
					efree(db_conn);
				RETURN_FALSE;
			}
		}

#if HAVE_EMPRESS
                {
                        int     direct = 0;
                        char    dsnbuf[300];
                        short   dsnbuflen;
                        char    *ldb = 0;

                        if (strstr ((char*)db, ";"))
                        {
                                direct = 1;
                                if (uid && !strstr ((char*)db, "uid") &&
                                                !strstr ((char*)db, "UID"))
                                {
                                        ldb = (char*) emalloc (strlen(db) +
                                                        strlen (uid) +
                                                        strlen (pwd) + 12);
                                        sprintf (ldb, "%s;UID=%s;PWD=%s",
                                                                db, uid, pwd);
                                }
                                else
                                {
                                        ldb = (char*) emalloc (strlen (db) + 1);
                                        strcat (ldb, db);
                                }
                        }

                        if (direct)
                                rc = SQLDriverConnect (db_conn->hdbc, NULL,
                                        ldb, strlen (ldb), dsnbuf, 300,
                                        &dsnbuflen, SQL_DRIVER_NOPROMPT);

                        else
                                rc = SQLConnect(db_conn->hdbc, db, SQL_NTS,
                                                uid, SQL_NTS, pwd, SQL_NTS);

                        if (ldb)
                                efree (ldb);
                }
#else
		rc = SQLConnect(db_conn->hdbc, db, SQL_NTS, uid, SQL_NTS, pwd, SQL_NTS);

#endif

#endif
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			UODBC_SQL_ERROR(db_conn->hdbc, SQL_NULL_HSTMT, "SQLConnect");
			SQLFreeConnect(db_conn->hdbc);
			if (persistent)
				free(db_conn);
			else
				efree(db_conn);
			RETURN_FALSE;
		}
		db_conn->open = 1;
		if (persistent){
			new_le.type = UODBC_GLOBAL(PHP3_UODBC_MODULE).le_pconn;
			new_le.ptr = db_conn;
			return_value->value.lval = 
				php3_plist_insert(db_conn, UODBC_GLOBAL(PHP3_UODBC_MODULE).le_pconn);
			new_index_ptr.ptr = (void *) return_value->value.lval;
#ifdef THREAD_SAFE
			new_index_ptr.type = _php3_le_index_ptr();
#else
			new_index_ptr.type = le_index_ptr;
#endif
			if (_php3_hash_update(plist,hashed_details,hashed_len + 1,(void *) &new_index_ptr,
					sizeof(list_entry),NULL)==FAILURE) {
				SQLDisconnect(db_conn->hdbc);
				SQLFreeConnect(db_conn->hdbc);
				free(db_conn);
				efree(hashed_details);
				RETURN_FALSE;
			}
			UODBC_GLOBAL(PHP3_UODBC_MODULE).num_persistent++;
		} else {
			new_le.type = UODBC_GLOBAL(PHP3_UODBC_MODULE).le_conn;
			new_le.ptr = db_conn;
			return_value->value.lval = 
				php3_list_insert(db_conn, UODBC_GLOBAL(PHP3_UODBC_MODULE).le_conn);
			new_index_ptr.ptr = (void *) return_value->value.lval;
#ifdef THREAD_SAFE
			new_index_ptr.type = _php3_le_index_ptr();
#else
			new_index_ptr.type = le_index_ptr;
#endif
			if (_php3_hash_update(list,hashed_details,hashed_len + 1,(void *) &new_index_ptr,
					sizeof(list_entry),NULL)==FAILURE) {
				SQLDisconnect(db_conn->hdbc);
				SQLFreeConnect(db_conn->hdbc);
				efree(db_conn);
				efree(hashed_details);
				RETURN_FALSE;
			}
		}

		UODBC_GLOBAL(PHP3_UODBC_MODULE).num_links++;

	} else {
		/* we are already connected */
#ifdef THREAD_SAFE
		if (index_ptr->type != _php3_le_index_ptr()) {
#else
		if (index_ptr->type != le_index_ptr) {
#endif
			efree(hashed_details);
			RETURN_FALSE;
		}
		id = (int) index_ptr->ptr;

		/* first see if there is a persistent connection and use it,
			else, if we are making a non-persistent connect, check our
			non-persistent list */
		db_conn = (UODBC_CONNECTION *)php3_plist_find(id, &type);
		if(!db_conn && !persistent)
			db_conn = (UODBC_CONNECTION *)php3_list_find(id, &type);


		/* FIXME test if the connection is dead */
		/* For Adabas D and local db connections, a reconnect is performed
		 * implicitly when needed. Cool.
		 */

		if (db_conn && (type ==  UODBC_GLOBAL(PHP3_UODBC_MODULE).le_conn ||
					type == UODBC_GLOBAL(PHP3_UODBC_MODULE).le_pconn)){
			return_value->value.lval = id;
		} else {
			efree(hashed_details);
			RETURN_FALSE;
		}
	}
	efree(hashed_details);
	return_value->type = IS_LONG;
}

/* {{{ proto odbc_close(int connection_id)
   Close an ODBC connection */
UODBC_FUNCTION(close)
{
	pval *arg1;
	HDBC conn;
	int type, ind;
	UODBC_TLS_VARS;

    if (getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	ind = (int)arg1->value.lval;
	conn = (HDBC)php3_list_find(ind, &type);
	if (!conn ||
		(type != UODBC_GLOBAL(PHP3_UODBC_MODULE).le_conn &&
		 type != UODBC_GLOBAL(PHP3_UODBC_MODULE).le_pconn)) {
		return;
	}
	php3_list_delete(ind);
}
/* }}} */

/* {{{ proto odbc_num_rows(int result_id)
   Get number of rows in a result */
UODBC_FUNCTION(num_rows)
{
	UODBC_RESULT   *result;
	SDWORD      rows;
	pval 	*arg1;
	
	if ( getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}            

	convert_to_long(arg1);

	if ((result = UODBC_GET_RESULT(list, arg1->value.lval)) == NULL) {
		RETURN_FALSE;
	}

	SQLRowCount(result->stmt, &rows);
	RETURN_LONG(rows);
}
/* }}} */

/* {{{ proto odbc_num_fields(int result_id)
   Get number of columns in a result */
UODBC_FUNCTION(num_fields)
{
	UODBC_RESULT   *result;
	pval     *arg1;

 	if ( getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}                            
 
    convert_to_long(arg1);
	 
	if ((result = UODBC_GET_RESULT(list, arg1->value.lval)) == NULL) {
		RETURN_FALSE;
	}
	RETURN_LONG(result->numcols);
}
/* }}} */

/* {{{ proto odbc_field_name(int result_id, int field_number)
   Get a column name */
UODBC_FUNCTION(field_name)
{
	UODBC_RESULT       *result;
	pval     *arg1, *arg2;
	
	if (getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
		
	convert_to_long(arg1);
	convert_to_long(arg2);
	
    if ((result = UODBC_GET_RESULT(list, arg1->value.lval)) == NULL) {
		RETURN_FALSE;
	}
	
	if (result->numcols == 0){
		php3_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
	
	if (arg2->value.lval > result->numcols){
		php3_error(E_WARNING, "Field index larger than number of fields");
		RETURN_FALSE;
	}
	
	if (arg2->value.lval < 1){
		php3_error(E_WARNING, "Field numbering starts at 1");
		RETURN_FALSE;
	}
	
	RETURN_STRING(result->values[arg2->value.lval - 1].name,1)
}
/* }}} */

/* {{{ proto odbc_field_type(int result_id, int field_number)
   Get the datatype of a column */
UODBC_FUNCTION(field_type)
{
	UODBC_RESULT	*result;
	char    	tmp[32];
	SWORD   	tmplen;
	pval     *arg1, *arg2;

	if (getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_long(arg2);

	if ((result = UODBC_GET_RESULT(list, arg1->value.lval)) == NULL) {
		RETURN_FALSE;
	}               

	if (result->numcols == 0){
		php3_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}

	if (arg2->value.lval > result->numcols){
		php3_error(E_WARNING, "Field index larger than number of fields");
		RETURN_FALSE;
	}

	SQLColAttributes(result->stmt, (UWORD)arg2->value.lval,
					 SQL_COLUMN_TYPE_NAME, tmp, 31, &tmplen, NULL);
	RETURN_STRING(tmp,1)
}
/* }}} */

/* {{{ proto odbc_field_len(int result_id, int field_number)
   Get the length of a column */   
UODBC_FUNCTION(field_len)
{
	UODBC_RESULT       *result;
	SDWORD  len;
	pval     *arg1, *arg2;

	if (getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_long(arg2);
	
	if ((result = UODBC_GET_RESULT(list, arg1->value.lval)) == NULL) {
		RETURN_FALSE;
	}                                                                

	if (result->numcols == 0){
		php3_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
	
	if (arg2->value.lval > result->numcols){
		php3_error(E_WARNING, "Field index larger than number of fields");
		RETURN_FALSE;
	}
	SQLColAttributes(result->stmt, (UWORD)arg2->value.lval, 
					 SQL_COLUMN_PRECISION, NULL, 0, NULL, &len);
	
	RETURN_LONG(len);
}
/* }}} */

/* {{{ proto odbc_field_num(int result_id, string field_name)
   Return column number */
UODBC_FUNCTION(field_num)
{
	int         field_ind;
	char        *fname;
	UODBC_RESULT *result;
	int         i;
	pval     *arg1, *arg2;

	if (getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	fname = arg2->value.str.val;
	
	if (arg1->value.lval == 1){
		php3_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
	if ((result = UODBC_GET_RESULT(list, arg1->value.lval)) == NULL) {
		RETURN_FALSE;
	}
	
	field_ind = -1;
	for(i = 0; i < result->numcols; i++){
		if (strcasecmp(result->values[i].name, fname) == 0)
			field_ind = i + 1;
		}
	if (field_ind == -1)
		RETURN_FALSE;
	RETURN_LONG(field_ind);
}
/* }}} */

/* {{{ proto odbc_autocommit(int connection_id, int OnOff)
   Toggle autocommit mode */
UODBC_FUNCTION(autocommit)
{
	UODBC_CONNECTION *curr_conn;
	RETCODE rc;
	pval *arg1, *arg2 = NULL;
	int argc;

	argc = ARG_COUNT(ht);
	if (argc == 2) {
		if (getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else if (argc == 1) {
		if (getParameters(ht, 1, &arg1) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else {
		WRONG_PARAM_COUNT;
	}
 
    convert_to_long(arg1);
	if (arg2) {
		convert_to_long(arg2);
	}

	if ((curr_conn = UODBC_GET_CONN(list, arg1->value.lval)) == NULL) {
		RETURN_FALSE;
	}

	if (arg2) {
		rc = SQLSetConnectOption(curr_conn->hdbc, SQL_AUTOCOMMIT,
								 (arg2->value.lval) ?
								 SQL_AUTOCOMMIT_ON : SQL_AUTOCOMMIT_OFF);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
			UODBC_SQL_ERROR(curr_conn->hdbc, SQL_NULL_HSTMT, "Set autocommit");
			RETURN_FALSE;
		}
		RETVAL_TRUE;
	} else {
		SDWORD status;

		rc = SQLGetConnectOption(curr_conn->hdbc, SQL_AUTOCOMMIT, (PTR)&status);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
			UODBC_SQL_ERROR(curr_conn->hdbc, SQL_NULL_HSTMT, "Test autocommit");
			RETURN_FALSE;
		}
		RETVAL_LONG((long)status);
	}
}
/* }}} */

void PHP3_UODBC_TRANSACT(INTERNAL_FUNCTION_PARAMETERS, int type)
{
	UODBC_CONNECTION *curr_conn;
	RETCODE rc;
	pval *arg1;
	UODBC_TLS_VARS;

 	if ( getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}                            
 
    convert_to_long(arg1);

	if ((curr_conn = UODBC_GET_CONN(list, arg1->value.lval)) == NULL){
		RETURN_FALSE;
	}

	rc = SQLTransact(UODBC_GLOBAL(PHP3_UODBC_MODULE).henv, curr_conn->hdbc, (UWORD)((type)?SQL_COMMIT:SQL_ROLLBACK));
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
		UODBC_SQL_ERROR(curr_conn->hdbc, SQL_NULL_HSTMT, "SQLTransact");
		RETURN_FALSE;
	}

	RETURN_TRUE;
}

/* {{{ proto odbc_commit(int connection_id)
   Commit an ODBC transaction */
UODBC_FUNCTION(commit)
{
	PHP3_UODBC_TRANSACT(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto odbc_rollback(int connection_id)
   Rollback a transaction */
UODBC_FUNCTION(rollback)
{
	PHP3_UODBC_TRANSACT(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto odbc_setoption(??)
   ?? */
UODBC_FUNCTION(setoption)
{
	UODBC_CONNECTION *curr_conn;
	UODBC_RESULT	*result;
	RETCODE rc;
	pval *arg1, *arg2, *arg3, *arg4;

 	if ( getParameters(ht, 3, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}                            
 
	convert_to_long(arg1);
	convert_to_long(arg2);
	convert_to_long(arg3);
	convert_to_long(arg4);

	switch (arg2->value.lval) {
		case 1:		/* SQLSetConnectOption */
			if ((curr_conn = UODBC_GET_CONN(list, arg1->value.lval)) == NULL){
				RETURN_FALSE;
			}
			rc = SQLSetConnectOption(curr_conn->hdbc, (unsigned short)(arg3->value.lval), (arg4->value.lval));
			if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
				UODBC_SQL_ERROR(curr_conn->hdbc, SQL_NULL_HSTMT, "SetConnectOption");
				RETURN_FALSE;
			}
			break;
		case 2:		/* SQLSetStmtOption */
			if ((result = UODBC_GET_RESULT(list, arg1->value.lval)) == NULL) {
				RETURN_FALSE;
			}
			rc = SQLSetStmtOption(result->stmt, (unsigned short)(arg3->value.lval), (arg4->value.lval));
			if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
				UODBC_SQL_ERROR(result->conn_ptr->hdbc, result->stmt, "SetStmtOption");
				RETURN_FALSE;
			}
			break;
		default:
			php3_error(E_WARNING, "Unknown option type");
			RETURN_FALSE;
			break;
	}

	RETURN_TRUE;
}
/* }}} */

#endif /* HAVE_UODBC */



/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
