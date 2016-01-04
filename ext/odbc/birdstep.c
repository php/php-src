/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Nikolay P. Romanyuk <mag@redcom.ru>                         |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/*
 * TODO:
 * birdstep_fetch_into(),
 * Check all on real life apps.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if WIN32
# include "config.w32.h"
# include "win95nt.h"
# ifdef PHP_EXPORTS
#  define PHPAPI __declspec(dllexport)
# else
#  define PHPAPI __declspec(dllimport)
# endif
#else
# include <php_config.h>
# define PHPAPI
# define THREAD_LS
#endif

#ifdef HAVE_BIRDSTEP
#include "php_birdstep.h"
#include "ext/standard/info.h"
#include "php_ini.h"

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_birdstep_connect, 0, 0, 3)
	ZEND_ARG_INFO(0, server)
	ZEND_ARG_INFO(0, user)
	ZEND_ARG_INFO(0, pass)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_birdstep_close, 0, 0, 1)
	ZEND_ARG_INFO(0, id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_birdstep_exec, 0, 0, 2)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_INFO(0, exec_str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_birdstep_fetch, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_birdstep_result, 0, 0, 2)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_INFO(0, col)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_birdstep_freeresult, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_birdstep_autocommit, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_birdstep_off_autocommit, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_birdstep_commit, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_birdstep_rollback, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_birdstep_fieldname, 0, 0, 2)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_INFO(0, col)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_birdstep_fieldnum, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()
/* }}} */

const zend_function_entry birdstep_functions[] = {
	PHP_FE(birdstep_connect,        arginfo_birdstep_connect)
	PHP_FE(birdstep_close,          arginfo_birdstep_close)
	PHP_FE(birdstep_exec,           arginfo_birdstep_exec)
	PHP_FE(birdstep_fetch,          arginfo_birdstep_fetch)
	PHP_FE(birdstep_result,         arginfo_birdstep_result)
	PHP_FE(birdstep_freeresult,     arginfo_birdstep_freeresult)
	PHP_FE(birdstep_autocommit,     arginfo_birdstep_autocommit)
	PHP_FE(birdstep_off_autocommit, arginfo_birdstep_off_autocommit)
	PHP_FE(birdstep_commit,         arginfo_birdstep_commit)
	PHP_FE(birdstep_rollback,       arginfo_birdstep_rollback)
	PHP_FE(birdstep_fieldnum,       arginfo_birdstep_fieldnum)
	PHP_FE(birdstep_fieldname,      arginfo_birdstep_fieldname)
/*
 * Temporary Function aliases until the next major upgrade to PHP.
 * These should allow users to continue to use their current scripts,
 * but should in reality warn the user that this functionality is
 * deprecated.
 */
	PHP_FALIAS(velocis_connect,        birdstep_connect,        arginfo_birdstep_connect)
	PHP_FALIAS(velocis_close,          birdstep_close,          arginfo_birdstep_close)
	PHP_FALIAS(velocis_exec,           birdstep_exec,           arginfo_birdstep_exec)
	PHP_FALIAS(velocis_fetch,          birdstep_fetch,          arginfo_birdstep_fetch)
	PHP_FALIAS(velocis_result,         birdstep_result,         arginfo_birdstep_result)
	PHP_FALIAS(velocis_freeresult,     birdstep_freeresult,     arginfo_birdstep_freeresult)
	PHP_FALIAS(velocis_autocommit,     birdstep_autocommit,     arginfo_birdstep_autocommit)
	PHP_FALIAS(velocis_off_autocommit, birdstep_off_autocommit, arginfo_birdstep_off_autocommit)
	PHP_FALIAS(velocis_commit,         birdstep_commit,         arginfo_birdstep_commit)
	PHP_FALIAS(velocis_rollback,       birdstep_rollback,       arginfo_birdstep_rollback)
	PHP_FALIAS(velocis_fieldnum,       birdstep_fieldnum,       arginfo_birdstep_fieldnum)
	PHP_FALIAS(velocis_fieldname,      birdstep_fieldname,      arginfo_birdstep_fieldname)
/* End temporary aliases */
	{NULL, NULL, NULL}
};

zend_module_entry birdstep_module_entry = {
	STANDARD_MODULE_HEADER,
	"birdstep",
	birdstep_functions,
	PHP_MINIT(birdstep),
	PHP_MSHUTDOWN(birdstep),
	PHP_RINIT(birdstep),
	NULL,
	PHP_MINFO(birdstep),
	PHP_BIRDSTEP_VERSION,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_ODBC
ZEND_GET_MODULE(birdstep)
#endif

THREAD_LS birdstep_module php_birdstep_module;
THREAD_LS static HENV henv;

#define PHP_GET_BIRDSTEP_RES_IDX(id) if (!(res = birdstep_find_result(list, id))) { php_error_docref(NULL, E_WARNING, "Birdstep: Not result index (%ld)", id); RETURN_FALSE; }
#define PHP_BIRDSTEP_CHK_LNK(id) if (!(conn = birdstep_find_conn(list, id))) { php_error_docref(NULL, E_WARNING, "Birdstep: Not connection index (%ld)", id); RETURN_FALSE; }


static void _close_birdstep_link(zend_rsrc_list_entry *rsrc)
{
	VConn *conn = (VConn *)rsrc->ptr;

	if ( conn ) {
		efree(conn);
	}
}

static void _free_birdstep_result(zend_rsrc_list_entry *rsrc)
{
	Vresult *res = (Vresult *)rsrc->ptr;

	if ( res && res->values ) {
		register int i;
		for ( i=0; i < res->numcols; i++ ) {
			if ( res->values[i].value )
				efree(res->values[i].value);
		}
		efree(res->values);
	}
	if ( res ) {
		efree(res);
	}
}

PHP_MINIT_FUNCTION(birdstep)
{
	SQLAllocEnv(&henv);

	if ( cfg_get_long("birdstep.max_links",&php_birdstep_module.max_links) == FAILURE ) {
		php_birdstep_module.max_links = -1;
	}
	php_birdstep_module.num_links = 0;
	php_birdstep_module.le_link   = zend_register_list_destructors_ex(_close_birdstep_link, NULL, "birdstep link", module_number);
	php_birdstep_module.le_result = zend_register_list_destructors_ex(_free_birdstep_result, NULL, "birdstep result", module_number);

	return SUCCESS;
}

PHP_RINIT_FUNCTION(birdstep)
{
	return SUCCESS;
}


PHP_MINFO_FUNCTION(birdstep)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "RAIMA Birdstep Support", "enabled" );
	php_info_print_table_end();
}

PHP_MSHUTDOWN_FUNCTION(birdstep)
{
	SQLFreeEnv(henv);
	return SUCCESS;
}

/* Some internal functions. Connections and result manupulate */

static int birdstep_add_conn(HashTable *list,VConn *conn,HDBC hdbc)
{
	int ind;

	ind = zend_list_insert(conn,php_birdstep_module.le_link);
	conn->hdbc = hdbc;
	conn->index = ind;

	return(ind);
}

static VConn * birdstep_find_conn(HashTable *list,int ind)
{
	VConn *conn;
	int type;

	conn = zend_list_find(ind,&type);
	if ( !conn || type != php_birdstep_module.le_link ) {
		return(NULL);
	}
	return(conn);
}

static void birdstep_del_conn(HashTable *list,int ind)
{
	zend_list_delete(ind);
}

static int birdstep_add_result(HashTable *list,Vresult *res,VConn *conn)
{
	int ind;

	ind = zend_list_insert(res,php_birdstep_module.le_result);
	res->conn = conn;
	res->index = ind;

	return(ind);
}

static Vresult * birdstep_find_result(HashTable *list,int ind)
{
	Vresult *res;
	int type;

	res = zend_list_find(ind,&type);
	if ( !res || type != php_birdstep_module.le_result ) {
		return(NULL);
	}
	return(res);
}

static void birdstep_del_result(HashTable *list,int ind)
{
	zend_list_delete(ind);
}

/* Users functions */

/* {{{ proto int birdstep_connect(string server, string user, string pass)
 */
PHP_FUNCTION(birdstep_connect)
{
	char *serv, *user, *pass;
	size_t serv_len, user_len, pass_len;
	RETCODE stat;
	HDBC hdbc;
	VConn *new;
	zend_long ind;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sss", &serv, &serv_len, &user, &user_len, &pass, &pass_len) == FAILURE) {
		return;
	}

	if ( php_birdstep_module.max_links != -1 && php_birdstep_module.num_links == php_birdstep_module.max_links ) {
		php_error_docref(NULL, E_WARNING, "Birdstep: Too many open connections (%d)",php_birdstep_module.num_links);
		RETURN_FALSE;
	}

	stat = SQLAllocConnect(henv,&hdbc);
	if ( stat != SQL_SUCCESS ) {
		php_error_docref(NULL, E_WARNING, "Birdstep: Could not allocate connection handle");
		RETURN_FALSE;
	}
	stat = SQLConnect(hdbc, serv, SQL_NTS, user, SQL_NTS, pass, SQL_NTS);
	if ( stat != SQL_SUCCESS && stat != SQL_SUCCESS_WITH_INFO ) {
		php_error_docref(NULL, E_WARNING, "Birdstep: Could not connect to server \"%s\" for %s", serv, user);
		SQLFreeConnect(hdbc);
		RETURN_FALSE;
	}
	new = (VConn *)emalloc(sizeof(VConn));
	ind = birdstep_add_conn(list,new,hdbc);
	php_birdstep_module.num_links++;
	RETURN_LONG(ind);
}
/* }}} */

/* {{{ proto bool birdstep_close(int id)
 */
PHP_FUNCTION(birdstep_close)
{
	zend_long id;
	VConn *conn;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &id) == FAILURE) {
		return;
	}

	PHP_BIRDSTEP_CHK_LNK(id);

	SQLDisconnect(conn->hdbc);
	SQLFreeConnect(conn->hdbc);
	birdstep_del_conn(list, id);
	php_birdstep_module.num_links--;
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int birdstep_exec(int index, string exec_str)
 */
PHP_FUNCTION(birdstep_exec)
{
	char *query;
	zend_long ind;
	size_t query_len, indx;
	VConn *conn;
	Vresult *res;
	RETCODE stat;
	SWORD cols,i,colnamelen;
	SDWORD rows,coldesc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ls", &ind, &query, &query_len) == FAILURE) {
		return;
	}

	PHP_BIRDSTEP_CHK_LNK(ind);

	res = (Vresult *)emalloc(sizeof(Vresult));
	stat = SQLAllocStmt(conn->hdbc,&res->hstmt);
	if ( stat != SQL_SUCCESS && stat != SQL_SUCCESS_WITH_INFO ) {
		php_error_docref(NULL, E_WARNING, "Birdstep: SQLAllocStmt return %d",stat);
		efree(res);
		RETURN_FALSE;
	}
	stat = SQLExecDirect(res->hstmt,query,SQL_NTS);
	if ( stat != SQL_SUCCESS && stat != SQL_SUCCESS_WITH_INFO ) {
		php_error_docref(NULL, E_WARNING, "Birdstep: Cannot execute \"%s\" query",query);
		SQLFreeStmt(res->hstmt,SQL_DROP);
		efree(res);
		RETURN_FALSE;
	}
	/* Success query */
	stat = SQLNumResultCols(res->hstmt,&cols);
	if ( stat != SQL_SUCCESS ) {
		php_error_docref(NULL, E_WARNING, "Birdstep: SQLNumResultCols return %d",stat);
		SQLFreeStmt(res->hstmt,SQL_DROP);
		efree(res);
		RETURN_FALSE;
	}
	if ( !cols ) { /* Was INSERT, UPDATE, DELETE, etc. query */
		stat = SQLRowCount(res->hstmt,&rows);
		if ( stat != SQL_SUCCESS ) {
			php_error_docref(NULL, E_WARNING, "Birdstep: SQLNumResultCols return %d",stat);
			SQLFreeStmt(res->hstmt,SQL_DROP);
			efree(res);
			RETURN_FALSE;
		}
		SQLFreeStmt(res->hstmt,SQL_DROP);
		efree(res);
		RETURN_LONG(rows);
	} else {  /* Was SELECT query */
		res->values = (VResVal *)safe_emalloc(sizeof(VResVal), cols, 0);
		res->numcols = cols;
		for ( i = 0; i < cols; i++ ) {
			SQLColAttributes(res->hstmt,i+1,SQL_COLUMN_NAME,
			   res->values[i].name,sizeof(res->values[i].name),
			   &colnamelen,NULL);
			SQLColAttributes(res->hstmt,i+1,SQL_COLUMN_TYPE,
			   NULL,0,NULL,&res->values[i].valtype);
			switch ( res->values[i].valtype ) {
				case SQL_LONGVARBINARY:
				case SQL_LONGVARCHAR:
					res->values[i].value = NULL;
					continue;
				default:
					break;
			}
			SQLColAttributes(res->hstmt,i+1,SQL_COLUMN_DISPLAY_SIZE,
			   NULL,0,NULL,&coldesc);
			res->values[i].value = (char *)emalloc(coldesc+1);
			SQLBindCol(res->hstmt,i+1,SQL_C_CHAR, res->values[i].value,coldesc+1, &res->values[i].vallen);
		}
	}
	res->fetched = 0;
	indx = birdstep_add_result(list,res,conn);
	RETURN_LONG(indx);
}
/* }}} */

/* {{{ proto bool birdstep_fetch(int index)
 */
PHP_FUNCTION(birdstep_fetch)
{
	zend_long ind;
	Vresult *res;
	RETCODE stat;
	UDWORD  row;
	UWORD   RowStat[1];

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &ind) == FAILURE) {
		return;
	}

	PHP_GET_BIRDSTEP_RES_IDX(ind);

	stat = SQLExtendedFetch(res->hstmt,SQL_FETCH_NEXT,1,&row,RowStat);
	if ( stat == SQL_NO_DATA_FOUND ) {
		SQLFreeStmt(res->hstmt,SQL_DROP);
		birdstep_del_result(list,Z_LVAL_PP(ind));
		RETURN_FALSE;
	}
	if ( stat != SQL_SUCCESS && stat != SQL_SUCCESS_WITH_INFO ) {
		php_error_docref(NULL, E_WARNING, "Birdstep: SQLFetch return error");
		SQLFreeStmt(res->hstmt,SQL_DROP);
		birdstep_del_result(list,Z_LVAL_PP(ind));
		RETURN_FALSE;
	}
	res->fetched = 1;
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto mixed birdstep_result(int index, mixed col)
 */
PHP_FUNCTION(birdstep_result)
{
	zval **col;
	zend_long ind;
	Vresult *res;
	RETCODE stat;
	int i,sql_c_type;
	UDWORD row;
	UWORD RowStat[1];
	SWORD indx = -1;
	char *field = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "lZ", &ind, &col) == FAILURE) {
		return;
	}

	PHP_GET_BIRDSTEP_RES_IDX(ind);

	if ( Z_TYPE_PP(col) == IS_STRING ) {
		field = Z_STRVAL_PP(col);
	} else {
		convert_to_long_ex(col);
		indx = Z_LVAL_PP(col);
	}
	if ( field ) {
		for ( i = 0; i < res->numcols; i++ ) {
			if ( !strcasecmp(res->values[i].name,field)) {
				indx = i;
				break;
			}
		}
		if ( indx < 0 ) {
			php_error_docref(NULL, E_WARNING,  "Field %s not found",field);
			RETURN_FALSE;
		}
	} else {
		if ( indx < 0 || indx >= res->numcols ) {
			php_error_docref(NULL, E_WARNING, "Birdstep: Field index not in range");
			RETURN_FALSE;
		}
	}
	if ( !res->fetched ) {
		stat = SQLExtendedFetch(res->hstmt,SQL_FETCH_NEXT,1,&row,RowStat);
		if ( stat == SQL_NO_DATA_FOUND ) {
			SQLFreeStmt(res->hstmt,SQL_DROP);
			birdstep_del_result(list,Z_LVAL_PP(ind));
			RETURN_FALSE;
		}
		if ( stat != SQL_SUCCESS && stat != SQL_SUCCESS_WITH_INFO ) {
			php_error_docref(NULL, E_WARNING, "Birdstep: SQLFetch return error");
			SQLFreeStmt(res->hstmt,SQL_DROP);
			birdstep_del_result(list,Z_LVAL_PP(ind));
			RETURN_FALSE;
		}
		res->fetched = 1;
	}
	switch ( res->values[indx].valtype ) {
		case SQL_LONGVARBINARY:
			sql_c_type = SQL_C_BINARY;
			goto l1;
		case SQL_LONGVARCHAR:
			sql_c_type = SQL_C_CHAR;
l1:
			if ( !res->values[indx].value ) {
				res->values[indx].value = emalloc(4096);
			}
			stat = SQLGetData(res->hstmt,indx+1,sql_c_type,
				res->values[indx].value,4095,&res->values[indx].vallen);
			if ( stat == SQL_NO_DATA_FOUND ) {
				SQLFreeStmt(res->hstmt,SQL_DROP);
				birdstep_del_result(list,Z_LVAL_PP(ind));
				RETURN_FALSE;
			}
			if ( stat != SQL_SUCCESS && stat != SQL_SUCCESS_WITH_INFO ) {
				php_error_docref(NULL, E_WARNING, "Birdstep: SQLGetData return error");
				SQLFreeStmt(res->hstmt,SQL_DROP);
				birdstep_del_result(list,Z_LVAL_PP(ind));
				RETURN_FALSE;
			}
			if ( res->values[indx].valtype == SQL_LONGVARCHAR ) {
				RETURN_STRING(res->values[indx].value,TRUE);
			} else {
				RETURN_LONG((zend_long)res->values[indx].value);
			}
		default:
			if ( res->values[indx].value != NULL ) {
				RETURN_STRING(res->values[indx].value,TRUE);
			}
	}
}
/* }}} */

/* {{{ proto bool birdstep_freeresult(int index)
 */
PHP_FUNCTION(birdstep_freeresult)
{
	zend_long ind;
	Vresult *res;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &ind) == FAILURE) {
		return;
	}

	PHP_GET_BIRDSTEP_RES_IDX(ind);

	SQLFreeStmt(res->hstmt,SQL_DROP);
	birdstep_del_result(list, ind);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool birdstep_autocommit(int index)
 */
PHP_FUNCTION(birdstep_autocommit)
{
	zend_long id;
	RETCODE stat;
	VConn *conn;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &id) == FAILURE) {
		return;
	}

	PHP_BIRDSTEP_CHK_LNK(id);

	stat = SQLSetConnectOption(conn->hdbc,SQL_AUTOCOMMIT,SQL_AUTOCOMMIT_ON);
	if ( stat != SQL_SUCCESS && stat != SQL_SUCCESS_WITH_INFO ) {
		php_error_docref(NULL, E_WARNING, "Birdstep: Set autocommit_on option failure");
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool birdstep_off_autocommit(int index)
 */
PHP_FUNCTION(birdstep_off_autocommit)
{
	zend_long id;
	RETCODE stat;
	VConn *conn;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &id) == FAILURE) {
		return;
	}

	PHP_BIRDSTEP_CHK_LNK(id);

	stat = SQLSetConnectOption(conn->hdbc,SQL_AUTOCOMMIT,SQL_AUTOCOMMIT_OFF);
	if ( stat != SQL_SUCCESS && stat != SQL_SUCCESS_WITH_INFO ) {
		php_error_docref(NULL, E_WARNING, "Birdstep: Set autocommit_off option failure");
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool birdstep_commit(int index)
 */
PHP_FUNCTION(birdstep_commit)
{
	zend_long id;
	RETCODE stat;
	VConn *conn;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &id) == FAILURE) {
		return;
	}

	PHP_BIRDSTEP_CHK_LNK(id)

	stat = SQLTransact(NULL,conn->hdbc,SQL_COMMIT);
	if ( stat != SQL_SUCCESS ) {
		php_error_docref(NULL, E_WARNING, "Birdstep: Commit failure");
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool birdstep_rollback(int index)
 */
PHP_FUNCTION(birdstep_rollback)
{
	zend_long id;
	RETCODE stat;
	VConn *conn;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &id) == FAILURE) {
		return;
	}

	PHP_BIRDSTEP_CHK_LNK(id);

	stat = SQLTransact(NULL,conn->hdbc,SQL_ROLLBACK);
	if ( stat != SQL_SUCCESS ) {
		php_error_docref(NULL, E_WARNING, "Birdstep: Rollback failure");
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string birdstep_fieldname(int index, int col)
 */
PHP_FUNCTION(birdstep_fieldname)
{
	zend_long ind, col;
	Vresult *res;
	SWORD indx;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ll", &ind, &col) == FAILURE) {
		return;
	}

	PHP_GET_BIRDSTEP_RES_IDX(ind);

	indx = col;
	if ( indx < 0 || indx >= res->numcols ) {
		php_error_docref(NULL, E_WARNING, "Birdstep: Field index not in range");
		RETURN_FALSE;
	}
	RETURN_STRING(res->values[indx].name,TRUE);
}
/* }}} */

/* {{{ proto int birdstep_fieldnum(int index)
 */
PHP_FUNCTION(birdstep_fieldnum)
{
	zend_long ind;
	Vresult *res;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &ind) == FAILURE) {
		return;
	}

	PHP_GET_BIRDSTEP_RES_IDX(ind);

	RETURN_LONG(res->numcols);
}
/* }}} */

#endif /* HAVE_BIRDSTEP */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
