/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
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
 * velocis_fetch_into(),
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
# include "php_config.h"
# define PHPAPI
# define THREAD_LS
#endif

#ifdef HAVE_VELOCIS
#include "php_velocis.h"
#include "ext/standard/info.h"

function_entry velocis_functions[] = {
	PHP_FE(velocis_connect,								NULL)
	PHP_FE(velocis_close,								NULL)
	PHP_FE(velocis_exec,								NULL)
	PHP_FE(velocis_fetch,								NULL)
	PHP_FE(velocis_result,								NULL)
	PHP_FE(velocis_freeresult,							NULL)
	PHP_FE(velocis_autocommit,							NULL)
	PHP_FE(velocis_off_autocommit,						NULL)
	PHP_FE(velocis_commit,								NULL)
	PHP_FE(velocis_rollback,							NULL)
	PHP_FE(velocis_fieldnum,							NULL)
	PHP_FE(velocis_fieldname,							NULL)
	{NULL, NULL, NULL}
};

zend_module_entry velocis_module_entry = {
	STANDARD_MODULE_HEADER,
	"velocis", velocis_functions, PHP_MINIT(velocis), PHP_MSHUTDOWN(velocis),
		PHP_RINIT(velocis), NULL, PHP_MINFO(velocis), NO_VERSION_YET,
        STANDARD_MODULE_PROPERTIES
};


#ifdef COMPILE_DL_ODBC
ZEND_GET_MODULE(velocis)
#endif

THREAD_LS velocis_module php_velocis_module;
THREAD_LS static HENV henv;

static void _close_velocis_link(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	VConn *conn = (VConn *)rsrc->ptr;

	if ( conn ) {
		efree(conn);
	}
}

static void _free_velocis_result(zend_rsrc_list_entry *rsrc TSRMLS_DC)
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

PHP_MINIT_FUNCTION(velocis)
{
	SQLAllocEnv(&henv);

	if ( cfg_get_long("velocis.max_links",&php_velocis_module.max_links) == FAILURE ) {
		php_velocis_module.max_links = -1;
	}
	php_velocis_module.num_links = 0;
	php_velocis_module.le_link   = zend_register_list_destructors_ex(_close_velocis_link, NULL, "velocis link", module_number);
	php_velocis_module.le_result = zend_register_list_destructors_ex(_free_velocis_result, NULL, "velocis result", module_number);

	return SUCCESS;
}

PHP_RINIT_FUNCTION(velocis)
{
	return SUCCESS;
}


PHP_MINFO_FUNCTION(velocis)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "RAIMA Velocis Support", "enabled" );
	php_info_print_table_end();
}

PHP_MSHUTDOWN_FUNCTION(velocis)
{
	SQLFreeEnv(henv);
	return SUCCESS;
}

/* Some internal functions. Connections and result manupulate */

static int
velocis_add_conn(HashTable *list,VConn *conn,HDBC hdbc)
{
	int ind;

	ind = zend_list_insert(conn,php_velocis_module.le_link);
	conn->hdbc = hdbc;
	conn->index = ind;

	return(ind);
}

static VConn *
velocis_find_conn(HashTable *list,int ind)
{
	VConn *conn;
	int type;

	conn = zend_list_find(ind,&type);
	if ( !conn || type != php_velocis_module.le_link ) {
		return(NULL);
	}
	return(conn);
}

static void
velocis_del_conn(HashTable *list,int ind)
{
	zend_list_delete(ind);
}

static int
velocis_add_result(HashTable *list,Vresult *res,VConn *conn)
{
	int ind;

	ind = zend_list_insert(res,php_velocis_module.le_result);
	res->conn = conn;
	res->index = ind;

	return(ind);
}

static Vresult *
velocis_find_result(HashTable *list,int ind)
{
	Vresult *res;
	int type;

	res = zend_list_find(ind,&type);
	if ( !res || type != php_velocis_module.le_result ) {
		return(NULL);
	}
	return(res);
}

static void
velocis_del_result(HashTable *list,int ind)
{
	zend_list_delete(ind);
}

/* Users functions */

/* {{{ proto long velocis_connect(string server, string user, sting pass)
 */
PHP_FUNCTION(velocis_connect)
{
	pval *serv,*user,*pass;
	char *Serv = NULL;
	char *User = NULL;
	char *Pass = NULL;
	RETCODE stat;
	HDBC hdbc;
	VConn *new;
	long ind;

	if ( php_velocis_module.max_links != -1 && php_velocis_module.num_links == php_velocis_module.max_links ) {
		php_error(E_WARNING,"Velocis: Too many open connections (%d)",php_velocis_module.num_links);
		RETURN_FALSE;
	}
	if ( ZEND_NUM_ARGS() != 3 ||
	   getParameters(ht,3,&serv,&user,&pass) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(serv);
	convert_to_string(user);
	convert_to_string(pass);
	Serv = Z_STRVAL_P(serv);
	User = Z_STRVAL_P(user);
	Pass = Z_STRVAL_P(pass);
	stat = SQLAllocConnect(henv,&hdbc);
	if ( stat != SQL_SUCCESS ) {
		php_error(E_WARNING,"Velocis: Could not allocate connection handle");
		RETURN_FALSE;
	}
	stat = SQLConnect(hdbc,Serv,SQL_NTS,User,SQL_NTS,Pass,SQL_NTS);
	if ( stat != SQL_SUCCESS && stat != SQL_SUCCESS_WITH_INFO ) {
		php_error(E_WARNING,"Velocis: Could not connect to server \"%s\" for %s",Serv,User);
		SQLFreeConnect(hdbc);
		RETURN_FALSE;
	}
	new = (VConn *)emalloc(sizeof(VConn));
	if ( new == NULL ) {
		php_error(E_WARNING,"Velocis: Out of memory for store connection");
		SQLFreeConnect(hdbc);
		RETURN_FALSE;
	}
	ind = velocis_add_conn(list,new,hdbc);
	php_velocis_module.num_links++;
	RETURN_LONG(ind);
}
/* }}} */

/* {{{ proto bool velocis_close(int id)
 */
PHP_FUNCTION(velocis_close)
{
	pval *id;
	VConn *conn;

	if ( ZEND_NUM_ARGS() != 1 || getParameters(ht,1,&id) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(id);
	conn = velocis_find_conn(list,Z_LVAL_P(id));
	if ( !conn ) {
		php_error(E_WARNING,"Velocis: Not connection index (%d)",Z_LVAL_P(id));
		RETURN_FALSE;
	}
	SQLDisconnect(conn->hdbc);
	SQLFreeConnect(conn->hdbc);
	velocis_del_conn(list,Z_LVAL_P(id));
	php_velocis_module.num_links--;
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int velocis(int index, string exec_str)
 */
PHP_FUNCTION(velocis_exec)
{
	pval *ind,*exec_str;
	char *query = NULL;
	int indx;
	VConn *conn;
	Vresult *res;
	RETCODE stat;
	SWORD cols,i,colnamelen;
	SDWORD rows,coldesc;

	if ( ZEND_NUM_ARGS() != 2 || getParameters(ht,2,&ind,&exec_str) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(ind);
	conn = velocis_find_conn(list,Z_LVAL_P(ind));
	if ( !conn ) {
		php_error(E_WARNING,"Velocis: Not connection index (%d)",Z_LVAL_P(ind));
		RETURN_FALSE;
	}
	convert_to_string(exec_str);
	query = Z_STRVAL_P(exec_str);

	res = (Vresult *)emalloc(sizeof(Vresult));
	if ( res == NULL ) {
		php_error(E_WARNING,"Velocis: Out of memory for result");
		RETURN_FALSE;
	}
	stat = SQLAllocStmt(conn->hdbc,&res->hstmt);
	if ( stat != SQL_SUCCESS && stat != SQL_SUCCESS_WITH_INFO ) {
		php_error(E_WARNING,"Velocis: SQLAllocStmt return %d",stat);
		efree(res);
		RETURN_FALSE;
	}
	stat = SQLExecDirect(res->hstmt,query,SQL_NTS);
	if ( stat != SQL_SUCCESS && stat != SQL_SUCCESS_WITH_INFO ) {
		php_error(E_WARNING,"Velocis: Can not execute \"%s\" query",query);
		SQLFreeStmt(res->hstmt,SQL_DROP);
		efree(res);
		RETURN_FALSE;
	}
	/* Success query */
	stat = SQLNumResultCols(res->hstmt,&cols);
	if ( stat != SQL_SUCCESS ) {
		php_error(E_WARNING,"Velocis: SQLNumResultCols return %d",stat);
		SQLFreeStmt(res->hstmt,SQL_DROP);
		efree(res);
		RETURN_FALSE;
	}
	if ( !cols ) { /* Was INSERT, UPDATE, DELETE, etc. query */
		stat = SQLRowCount(res->hstmt,&rows);
		if ( stat != SQL_SUCCESS ) {
			php_error(E_WARNING,"Velocis: SQLNumResultCols return %d",stat);
			SQLFreeStmt(res->hstmt,SQL_DROP);
			efree(res);
			RETURN_FALSE;
		}
		SQLFreeStmt(res->hstmt,SQL_DROP);
		efree(res);
		RETURN_LONG(rows);
	} else {  /* Was SELECT query */
		res->values = (VResVal *)emalloc(sizeof(VResVal)*cols);
		if ( res->values == NULL ) {
			php_error(E_WARNING,"Velocis: Out of memory for result columns");
			SQLFreeStmt(res->hstmt,SQL_DROP);
			efree(res);
			RETURN_FALSE;
		}
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
			if ( res->values[i].value != NULL ) {
				SQLBindCol(res->hstmt,i+1,SQL_C_CHAR,
				   res->values[i].value,coldesc+1,
				   &res->values[i].vallen);
			}
		}
	}
	res->fetched = 0;
	indx = velocis_add_result(list,res,conn);
	RETURN_LONG(indx);
}
/* }}} */

/* {{{ proto bool velocis_fetch(int index)
 */
PHP_FUNCTION(velocis_fetch)
{
	pval *ind;
	Vresult *res;
	RETCODE stat;
	UDWORD  row;
	UWORD   RowStat[1];

	if ( ZEND_NUM_ARGS() != 1 || getParameters(ht,1,&ind) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(ind);
	res = velocis_find_result(list,Z_LVAL_P(ind));
	if ( !res ) {
		php_error(E_WARNING,"Velocis: Not result index (%d)",Z_LVAL_P(ind));
		RETURN_FALSE;
	}
	stat = SQLExtendedFetch(res->hstmt,SQL_FETCH_NEXT,1,&row,RowStat);
	if ( stat == SQL_NO_DATA_FOUND ) {
		SQLFreeStmt(res->hstmt,SQL_DROP);
		velocis_del_result(list,Z_LVAL_P(ind));
		RETURN_FALSE;
	}
	if ( stat != SQL_SUCCESS && stat != SQL_SUCCESS_WITH_INFO ) {
		php_error(E_WARNING,"Velocis: SQLFetch return error");
		SQLFreeStmt(res->hstmt,SQL_DROP);
		velocis_del_result(list,Z_LVAL_P(ind));
		RETURN_FALSE;
	}
	res->fetched = 1;
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto mixed velocis_result(int index, int col)
 */
PHP_FUNCTION(velocis_result)
{
	pval *ind,*col;
	Vresult *res;
	RETCODE stat;
	int i,sql_c_type;
	UDWORD row;
	UWORD RowStat[1];
	SWORD indx = -1;
	char *field = NULL;

	if ( ZEND_NUM_ARGS() != 2 || getParameters(ht,2,&ind,&col) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(ind);
	res = velocis_find_result(list,Z_LVAL_P(ind));
	if ( !res ) {
		php_error(E_WARNING,"Velocis: Not result index (%d),Z_LVAL_P(ind)");
		RETURN_FALSE;
	}
	if ( Z_TYPE_P(col) == IS_STRING ) {
		field = Z_STRVAL_P(col);
	} else {
		convert_to_long(col);
		indx = Z_LVAL_P(col);
	}
	if ( field ) {
		for ( i = 0; i < res->numcols; i++ ) {
			if ( !strcasecmp(res->values[i].name,field)) {
				indx = i;
				break;
			}
		}
		if ( indx < 0 ) {
			php_error(E_WARNING, "Field %s not found",field);
			RETURN_FALSE;
		}
	} else {
		if ( indx < 0 || indx >= res->numcols ) {
			php_error(E_WARNING,"Velocis: Field index not in range");
			RETURN_FALSE;
		}
	}
	if ( !res->fetched ) {
		stat = SQLExtendedFetch(res->hstmt,SQL_FETCH_NEXT,1,&row,RowStat);
		if ( stat == SQL_NO_DATA_FOUND ) {
			SQLFreeStmt(res->hstmt,SQL_DROP);
			velocis_del_result(list,Z_LVAL_P(ind));
			RETURN_FALSE;
		}
		if ( stat != SQL_SUCCESS && stat != SQL_SUCCESS_WITH_INFO ) {
			php_error(E_WARNING,"Velocis: SQLFetch return error");
			SQLFreeStmt(res->hstmt,SQL_DROP);
			velocis_del_result(list,Z_LVAL_P(ind));
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
				if ( !res->values[indx].value ) {
					php_error(E_WARNING,"Out of memory");
					RETURN_FALSE;
				}
			}
			stat = SQLGetData(res->hstmt,indx+1,sql_c_type,
				res->values[indx].value,4095,&res->values[indx].vallen);
			if ( stat == SQL_NO_DATA_FOUND ) {
				SQLFreeStmt(res->hstmt,SQL_DROP);
				velocis_del_result(list,Z_LVAL_P(ind));
				RETURN_FALSE;
			}
			if ( stat != SQL_SUCCESS && stat != SQL_SUCCESS_WITH_INFO ) {
				php_error(E_WARNING,"Velocis: SQLGetData return error");
				SQLFreeStmt(res->hstmt,SQL_DROP);
				velocis_del_result(list,Z_LVAL_P(ind));
				RETURN_FALSE;
			}
			if ( res->values[indx].valtype == SQL_LONGVARCHAR ) {
				RETURN_STRING(res->values[indx].value,TRUE);
			} else {
				RETURN_LONG((long)res->values[indx].value);
			}
		default:
			if ( res->values[indx].value != NULL ) {
				RETURN_STRING(res->values[indx].value,TRUE);
			}
	}
}
/* }}} */

/* {{{ proto bool velocis_freeresult(int index)
 */
PHP_FUNCTION(velocis_freeresult)
{
	pval *ind;
	Vresult *res;

	if ( ZEND_NUM_ARGS() != 1 || getParameters(ht,1,&ind) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(ind);
	res = velocis_find_result(list,Z_LVAL_P(ind));
	if ( !res ) {
		php_error(E_WARNING,"Velocis: Not result index (%d)",Z_LVAL_P(ind));
		RETURN_FALSE;
	}
	SQLFreeStmt(res->hstmt,SQL_DROP);
	velocis_del_result(list,Z_LVAL_P(ind));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool velocis_on_autocommit(int index)
 */
PHP_FUNCTION(velocis_autocommit)
{
	pval *id;
	RETCODE stat;
	VConn *conn;

	if ( ZEND_NUM_ARGS() != 1 || getParameters(ht,1,&id) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(id);
	conn = velocis_find_conn(list,Z_LVAL_P(id));
	if ( !conn ) {
		php_error(E_WARNING,"Velocis: Not connection index (%d)",Z_LVAL_P(id));
		RETURN_FALSE;
	}
	stat = SQLSetConnectOption(conn->hdbc,SQL_AUTOCOMMIT,SQL_AUTOCOMMIT_ON);
	if ( stat != SQL_SUCCESS && stat != SQL_SUCCESS_WITH_INFO ) {
		php_error(E_WARNING,"Velocis: Set autocommit_on option failure");
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool velocis_off_autocommit(int index)
 */
PHP_FUNCTION(velocis_off_autocommit)
{
	pval *id;
	RETCODE stat;
	VConn *conn;

	if ( ZEND_NUM_ARGS() != 1 || getParameters(ht,1,&id) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(id);
	conn = velocis_find_conn(list,Z_LVAL_P(id));
	if ( !conn ) {
		php_error(E_WARNING,"Velocis: Not connection index (%d)",Z_LVAL_P(id));
		RETURN_FALSE;
	}
	stat = SQLSetConnectOption(conn->hdbc,SQL_AUTOCOMMIT,SQL_AUTOCOMMIT_OFF);
	if ( stat != SQL_SUCCESS && stat != SQL_SUCCESS_WITH_INFO ) {
		php_error(E_WARNING,"Velocis: Set autocommit_off option failure");
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool velocis_commit(int index)
 */
PHP_FUNCTION(velocis_commit)
{
	pval *id;
	RETCODE stat;
	VConn *conn;

	if ( ZEND_NUM_ARGS() != 1 || getParameters(ht,1,&id) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(id);
	conn = velocis_find_conn(list,Z_LVAL_P(id));
	if ( !conn ) {
		php_error(E_WARNING,"Velocis: Not connection index (%d)",Z_LVAL_P(id));
		RETURN_FALSE;
	}
	stat = SQLTransact(NULL,conn->hdbc,SQL_COMMIT);
	if ( stat != SQL_SUCCESS ) {
		php_error(E_WARNING,"Velocis: Commit failure");
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool velocis_rollback(int index)
 */
PHP_FUNCTION(velocis_rollback)
{
	pval *id;
	RETCODE stat;
	VConn *conn;

	if ( ZEND_NUM_ARGS() != 1 || getParameters(ht,1,&id) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(id);
	conn = velocis_find_conn(list,Z_LVAL_P(id));
	if ( !conn ) {
		php_error(E_WARNING,"Velocis: Not connection index (%d)",Z_LVAL_P(id));
		RETURN_FALSE;
	}
	stat = SQLTransact(NULL,conn->hdbc,SQL_ROLLBACK);
	if ( stat != SQL_SUCCESS ) {
		php_error(E_WARNING,"Velocis: Rollback failure");
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string velocis_fieldname(int index, int col)
 */
PHP_FUNCTION(velocis_fieldname)
{
	pval *ind,*col;
	Vresult *res;
	SWORD indx;

	if ( ZEND_NUM_ARGS() != 2 || getParameters(ht,2,&ind,&col) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(ind);
	res = velocis_find_result(list,Z_LVAL_P(ind));
	if ( !res ) {
		php_error(E_WARNING,"Velocis: Not result index (%d),Z_LVAL_P(ind)");
		RETURN_FALSE;
	}
	convert_to_long(col);
	indx = Z_LVAL_P(col);
	if ( indx < 0 || indx >= res->numcols ) {
		php_error(E_WARNING,"Velocis: Field index not in range");
		RETURN_FALSE;
	}
	RETURN_STRING(res->values[indx].name,TRUE);
}
/* }}} */

/* {{{ proto int velocis_fieldnum(int index)
 */
PHP_FUNCTION(velocis_fieldnum)
{
	pval *ind;
	Vresult *res;

	if ( ZEND_NUM_ARGS() != 1 || getParameters(ht,1,&ind) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(ind);
	res = velocis_find_result(list,Z_LVAL_P(ind));
	if ( !res ) {
		php_error(E_WARNING,"Velocis: Not result index (%d),Z_LVAL_P(ind)");
		RETURN_FALSE;
	}
	RETURN_LONG(res->numcols);
}
/* }}} */

#endif /* HAVE_VELOCIS */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
