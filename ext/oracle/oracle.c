/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
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
   |          Mitch Golden <mgolden@interport.net>                        |
   |          Rasmus Lerdorf <rasmus@php.net>                             |
   |          Andreas Karajannis <Andreas.Karajannis@gmd.de>              |
   |          Thies C. Arntzen <thies@thieso.net>                         |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* comment out the next line if you're on Oracle 7.x and don't have the olog 
   call. */
 
#define HAS_OLOG 1

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#include "zend_globals.h"

#if HAVE_ORACLE

#include "php_oracle.h"
#define HASH_DTOR (void (*)(void *))

#include "ext/standard/info.h"


#ifndef min
#define min(a, b) ((a) > (b) ? (b) : (a))
#endif

#ifdef PHP_WIN32
#define PHP_ORA_API __declspec(dllexport)
#else
#define PHP_ORA_API
#endif                                   

#ifdef ZTS
int ora_globals_id;
#else
PHP_ORA_API php_ora_globals ora_globals;
#endif

#define DB_SIZE 65536

#define ORA_FETCHINTO_ASSOC (1<<0)
#define ORA_FETCHINTO_NULLS (1<<1)

static oraCursor *ora_get_cursor(HashTable *, pval ** TSRMLS_DC);
static char *ora_error(Cda_Def *);
static int ora_describe_define(oraCursor *);
static void _close_oraconn(zend_rsrc_list_entry *rsrc TSRMLS_DC);
static int _close_oracur(oraCursor *cur TSRMLS_DC);
static int _ora_ping(oraConnection *conn);
int ora_set_param_values(oraCursor *cursor, int isout TSRMLS_DC);

void ora_do_logon(INTERNAL_FUNCTION_PARAMETERS, int persistent);

static int le_conn, le_pconn, le_cursor; 

/* {{{ prototypes
 */
PHP_FUNCTION(ora_bind);
PHP_FUNCTION(ora_close);
PHP_FUNCTION(ora_commit);
PHP_FUNCTION(ora_commitoff);
PHP_FUNCTION(ora_commiton);
PHP_FUNCTION(ora_do);
PHP_FUNCTION(ora_error);
PHP_FUNCTION(ora_errorcode);
PHP_FUNCTION(ora_exec);
PHP_FUNCTION(ora_fetch);
PHP_FUNCTION(ora_fetch_into);
PHP_FUNCTION(ora_columntype);
PHP_FUNCTION(ora_columnname);
PHP_FUNCTION(ora_columnsize);
PHP_FUNCTION(ora_getcolumn);
PHP_FUNCTION(ora_numcols);
PHP_FUNCTION(ora_numrows);
PHP_FUNCTION(ora_logoff);
PHP_FUNCTION(ora_logon);
PHP_FUNCTION(ora_plogon);
PHP_FUNCTION(ora_open);
PHP_FUNCTION(ora_parse);
PHP_FUNCTION(ora_rollback);

PHP_MINIT_FUNCTION(oracle);
PHP_RINIT_FUNCTION(oracle);
PHP_MSHUTDOWN_FUNCTION(oracle);
PHP_RSHUTDOWN_FUNCTION(oracle);
PHP_MINFO_FUNCTION(oracle);
/* }}} */

static unsigned char second_args_force_ref[] = { 2, BYREF_NONE, BYREF_FORCE };

/* {{{ oracle_functions[]
 */
function_entry oracle_functions[] = {
	PHP_FE(ora_bind,								NULL)
	PHP_FE(ora_close,								NULL)
	PHP_FE(ora_commit,								NULL)
	PHP_FE(ora_commitoff,							NULL)
	PHP_FE(ora_commiton,							NULL)
	PHP_FE(ora_do,									NULL)
	PHP_FE(ora_error,								NULL)
	PHP_FE(ora_errorcode,							NULL)
	PHP_FE(ora_exec,								NULL)
	PHP_FE(ora_fetch,								NULL)
   	PHP_FE(ora_fetch_into,							second_args_force_ref)
	PHP_FE(ora_columntype,							NULL)
	PHP_FE(ora_columnname,							NULL)
	PHP_FE(ora_columnsize,							NULL)
	PHP_FE(ora_getcolumn,							NULL)
	PHP_FE(ora_numcols,								NULL)
	PHP_FE(ora_numrows,								NULL)
	PHP_FE(ora_logoff,								NULL)
	PHP_FE(ora_logon,								NULL)
	PHP_FE(ora_plogon,								NULL)
	PHP_FE(ora_open,								NULL)
	PHP_FE(ora_parse,								NULL)
	PHP_FE(ora_rollback,							NULL)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ oracle_module_entry
 */
zend_module_entry oracle_module_entry = {
    STANDARD_MODULE_HEADER,
	"oracle",
	oracle_functions,
	PHP_MINIT(oracle),       /* extension-wide startup function */
    PHP_MSHUTDOWN(oracle),   /* extension-wide shutdown function */
    PHP_RINIT(oracle),       /* per-request startup function */
    PHP_RSHUTDOWN(oracle),   /* per-request shutdown function */
    PHP_MINFO(oracle),
    NO_VERSION_YET,
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

/* {{{ ora_func_tab[]
 */
static const text *ora_func_tab[] =
{(text *) "unused",
/*  1, 2  */ (text *) "unused", (text *) "OSQL",
/*  3, 4  */ (text *) "unused", (text *) "OEXEC/OEXN",
/*  5, 6  */ (text *) "unused", (text *) "OBIND",
/*  7, 8  */ (text *) "unused", (text *) "ODEFIN",
/*  9, 10 */ (text *) "unused", (text *) "ODSRBN",
/* 11, 12 */ (text *) "unused", (text *) "OFETCH/OFEN",
/* 13, 14 */ (text *) "unused", (text *) "OOPEN",
/* 15, 16 */ (text *) "unused", (text *) "OCLOSE",
/* 17, 18 */ (text *) "unused", (text *) "unused",
/* 19, 20 */ (text *) "unused", (text *) "unused",
/* 21, 22 */ (text *) "unused", (text *) "ODSC",
/* 23, 24 */ (text *) "unused", (text *) "ONAME",
/* 25, 26 */ (text *) "unused", (text *) "OSQL3",
/* 27, 28 */ (text *) "unused", (text *) "OBNDRV",
/* 29, 30 */ (text *) "unused", (text *) "OBNDRN",
/* 31, 32 */ (text *) "unused", (text *) "unused",
/* 33, 34 */ (text *) "unused", (text *) "OOPT",
/* 35, 36 */ (text *) "unused", (text *) "unused",
/* 37, 38 */ (text *) "unused", (text *) "unused",
/* 39, 40 */ (text *) "unused", (text *) "unused",
/* 41, 42 */ (text *) "unused", (text *) "unused",
/* 43, 44 */ (text *) "unused", (text *) "unused",
/* 45, 46 */ (text *) "unused", (text *) "unused",
/* 47, 48 */ (text *) "unused", (text *) "unused",
/* 49, 50 */ (text *) "unused", (text *) "unused",
/* 51, 52 */ (text *) "unused", (text *) "OCAN",
/* 53, 54 */ (text *) "unused", (text *) "OPARSE",
/* 55, 56 */ (text *) "unused", (text *) "OEXFET",
/* 57, 58 */ (text *) "unused", (text *) "OFLNG",
/* 59, 60 */ (text *) "unused", (text *) "ODESCR",
/* 61, 62 */ (text *) "unused", (text *) "OBNDRA"
};
/* }}} */

#ifdef COMPILE_DL_ORACLE
ZEND_GET_MODULE(oracle)
#endif

/* {{{ _close_oraconn
 */
static void _close_oraconn(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	oraConnection *conn = (oraConnection *)rsrc->ptr;
	
	conn->open = 0;

	ologof(&conn->lda);
	ORA(num_links)--;

	zend_hash_del(ORA(conns),(void*)&conn,sizeof(void*));

	if (conn->persistent) {
		ORA(num_persistent)--;
		free(conn);
	} else {
		efree(conn);
	}
}
/* }}} */

/* {{{ pval_ora_param_destructor
 */
static void
pval_ora_param_destructor(oraParam *param)
{
	if (param->progv) {
		efree(param->progv);
	}
}
/* }}} */

/* {{{ _close_oracur
 */
static int _close_oracur(oraCursor *cur TSRMLS_DC)
{
	int i;

	if (cur){
		if (cur->query){
			efree(cur->query);
		}
		if (cur->params){
			zend_hash_destroy(cur->params);
			efree(cur->params);
			cur->params = NULL;
		}
		if (cur->columns){
			for(i = 0; i < cur->ncols; i++){
				if (cur->columns[i].buf)
					efree(cur->columns[i].buf);
			}
			efree(cur->columns);
			cur->columns = NULL;
		}

		if (cur->open){
			oraConnection *db_conn;

			if (zend_hash_find(ORA(conns),(void*)&(cur->conn_ptr),sizeof(void*),(void **)&db_conn) == SUCCESS) {
				oclose(&cur->cda);
			} 
		}

	   	efree(cur);
	}
	
	return 1;
}
/* }}} */

/* {{{ php_close_ora_cursor
 */
static void php_close_ora_cursor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	oraCursor *cur = (oraCursor *)rsrc->ptr;

	_close_oracur(cur TSRMLS_CC);
}
/* }}} */

/* {{{ php_ora_init_globals
 */
static void php_ora_init_globals(php_ora_globals *ora_globals_p TSRMLS_DC)
{
	if (cfg_get_long("oracle.allow_persistent",
			 &ORA(allow_persistent))
		== FAILURE) {
	  ORA(allow_persistent) = -1;
	}
	if (cfg_get_long("oracle.max_persistent",
					 &ORA(max_persistent))
	    == FAILURE) {
		ORA(max_persistent) = -1;
	}
	if (cfg_get_long("oracle.max_links",
					 &ORA(max_links))
	    == FAILURE) {
		ORA(max_links) = -1;
	}
	
	ORA(num_persistent) = 0;
	
	ORA(conns) = malloc(sizeof(HashTable));
	zend_hash_init(ORA(conns), 13, NULL, NULL, 1);
	
	memset((void*) &ORA(db_err_conn),0,sizeof(ORA(db_err_conn)));
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(oracle)
{
#ifdef ZTS
	ts_allocate_id(&ora_globals_id, sizeof(php_ora_globals), (ts_allocate_ctor) php_ora_init_globals, NULL);
#else
	php_ora_init_globals(&ora_globals TSRMLS_CC);
#endif

	le_cursor = zend_register_list_destructors_ex(php_close_ora_cursor, NULL, "oracle cursor", module_number);
	le_conn = zend_register_list_destructors_ex(_close_oraconn, NULL, "oracle link", module_number);
	le_pconn = zend_register_list_destructors_ex(NULL, _close_oraconn, "oracle link persistent", module_number);

	REGISTER_LONG_CONSTANT("ORA_BIND_INOUT", 0, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ORA_BIND_IN",    1, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ORA_BIND_OUT",   2, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("ORA_FETCHINTO_ASSOC",ORA_FETCHINTO_ASSOC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ORA_FETCHINTO_NULLS",ORA_FETCHINTO_NULLS, CONST_CS | CONST_PERSISTENT);

#ifdef ZTS
	opinit(OCI_EV_TSF); 
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(oracle)
{
	ORA(num_links) = 
		ORA(num_persistent);
	/*
	  ORA(defaultlrl) = 0;
	  ORA(defaultbinmode) = 0;
	  ORA(defaultconn) = 0;
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(oracle)
{
	zend_hash_destroy(ORA(conns));
	free(ORA(conns));
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(oracle)
{
	return SUCCESS;
}
/* }}} */

/* {{{ _ora_ping
 */
static int _ora_ping(oraConnection *conn)
{
	Cda_Def cda;

	if (oopen(&cda, &conn->lda, (text *) 0, -1, -1, (text *) 0, -1)) {
		return 0;
	}

	if (oparse(&cda, "select sysdate from dual", (sb4) - 1, 0, VERSION_7)) {
		oclose(&cda);
		return 0;
	}

	oclose(&cda);
	return 1;

}
/* }}} */

/*
   ** PHP functions
*/

/* {{{ proto int ora_logon(string user, string password)
   Open an Oracle connection */
PHP_FUNCTION(ora_logon)
{
	ora_do_logon(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int ora_plogon(string user, string password)
   Open a persistent Oracle connection */
PHP_FUNCTION(ora_plogon)
{
	ora_do_logon(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ ora_do_logon
 */
void ora_do_logon(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	char *user,*passwd;
	pval **arg1, **arg2;
	char *hashed_details;
	int hashed_details_length;
	oraConnection *db_conn;

	if (zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
  
	convert_to_string_ex(arg1);
	convert_to_string_ex(arg2);
  
	user = Z_STRVAL_PP(arg1);
	passwd = Z_STRVAL_PP(arg2);

	hashed_details_length = sizeof("oracle__")-1+strlen(user)+strlen(passwd);
	hashed_details = (char *) emalloc(hashed_details_length+1);
	sprintf(hashed_details,"oracle_%s_%s",user,passwd);

	if (!ORA(allow_persistent)) {
		persistent=0;
	}
	if (persistent) {
		list_entry *le;
		
		/* try to find if we already have this link in our persistent list */
		if (zend_hash_find(&EG(persistent_list), hashed_details, hashed_details_length+1, (void **) &le)==FAILURE) {  /* we don't */
			list_entry new_le;
			
			if (ORA(max_links)!=-1 && ORA(num_links)>=ORA(max_links)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Oracle:  Too many open links (%d)",ORA(num_links));
				efree(hashed_details);
				RETURN_FALSE;
			}
			if (ORA(max_persistent)!=-1 && ORA(num_persistent)>=ORA(max_persistent)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Oracle:  Too many open persistent links (%d)",ORA(num_persistent));
				efree(hashed_details);
				RETURN_FALSE;
			}
			/* create the link */
			db_conn = (oraConnection *)malloc(sizeof(oraConnection));
			memset((void *) db_conn,0,sizeof(oraConnection));	
			db_conn->persistent = 1;

			if (
#if HAS_OLOG
				olog(&db_conn->lda, db_conn->hda, user,strlen(user), passwd, strlen(passwd), 0, -1, OCI_LM_DEF)
#else
				orlon(&db_conn->lda, db_conn->hda, user,strlen(user), passwd, strlen(passwd), 0)
#endif
				) {
				ORA(db_err_conn) = *db_conn;
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to connect to ORACLE (%s)",ora_error(&db_conn->lda));
				
				if (persistent) {
					free(db_conn);
				} else {
					efree(db_conn);
				}
				
				efree(hashed_details);
				RETURN_FALSE;
			}

			/* hash it up */
			Z_TYPE(new_le) = le_pconn;
			new_le.ptr = db_conn;
			if (zend_hash_update(&EG(persistent_list), hashed_details, hashed_details_length+1, (void *) &new_le, sizeof(list_entry), NULL)==FAILURE) {
				free(db_conn);
				efree(hashed_details);
				RETURN_FALSE;
			}
			ORA(num_persistent)++;
			ORA(num_links)++;
			zend_hash_add(ORA(conns),(void*)&db_conn,sizeof(void*),(void*)&db_conn,sizeof(void*),NULL);
		} else {  /* we do */
			if (Z_TYPE_P(le) != le_pconn) {
				RETURN_FALSE;
			}

			db_conn = (oraConnection *) le->ptr;

			/* ensure that the link did not die */

			if (!_ora_ping(db_conn)) {
				if (
#if HAS_OLOG
					olog(&db_conn->lda, db_conn->hda, user,strlen(user), passwd, strlen(passwd), 0, -1, OCI_LM_DEF)
#else
					orlon(&db_conn->lda, db_conn->hda, user,strlen(user), passwd, strlen(passwd), 0)
#endif
					) {
					ORA(db_err_conn) = *db_conn;
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Oracle: Link to server lost, unable to reconnect",ora_error(&db_conn->lda));
					zend_hash_del(&EG(persistent_list), hashed_details, hashed_details_length+1);
					efree(hashed_details);
					RETURN_FALSE;
				}
			}
		}
		ZEND_REGISTER_RESOURCE(return_value, db_conn, le_pconn);
	} else { /* non persistent */
		list_entry *index_ptr,new_index_ptr;
		
		/* first we check the hash for the hashed_details key.  if it exists,
		 * it should point us to the right offset where the actual Oracle link sits.
		 * if it doesn't, open a new Oracle link, add it to the resource list,
		 * and add a pointer to it with hashed_details as the key.
		 */
		if (zend_hash_find(&EG(regular_list),hashed_details,hashed_details_length+1,(void **) &index_ptr)==SUCCESS) {
			int type,link;
			void *ptr;

			if (Z_TYPE_P(index_ptr) != le_index_ptr) {
				RETURN_FALSE;
			}
			link = (int) index_ptr->ptr;
			ptr = zend_list_find(link,&type);   /* check if the link is still there */
			if (ptr && (type==le_conn || type==le_pconn)) {
				zend_list_addref(link);
				Z_LVAL_P(return_value) = link;
				Z_TYPE_P(return_value) = IS_RESOURCE;
				efree(hashed_details);
				return;
			} else {
				zend_hash_del(&EG(regular_list),hashed_details,hashed_details_length+1);
			}
		}
		if (ORA(max_links)!=-1 && ORA(num_links)>=ORA(max_links)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Oracle:  Too many open links (%d)",ORA(num_links));
			efree(hashed_details);
			RETURN_FALSE;
		}

		db_conn = (oraConnection *) emalloc(sizeof(oraConnection));
		memset((void *) db_conn,0,sizeof(oraConnection));	
		db_conn->persistent = 0;

		if (
#if HAS_OLOG
			olog(&db_conn->lda, db_conn->hda, user,strlen(user), passwd, strlen(passwd), 0, -1, OCI_LM_DEF)
#else
			orlon(&db_conn->lda, db_conn->hda, user,strlen(user), passwd, strlen(passwd), 0)
#endif
			) {
			ORA(db_err_conn) = *db_conn;
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Oracle: Connection Failed: %s\n",ora_error(&db_conn->lda));
			efree(hashed_details);
			efree(db_conn);
			RETURN_FALSE;
		}

		/* add it to the list */
		ZEND_REGISTER_RESOURCE(return_value, db_conn, le_conn);

		/* add it to the hash */
		new_index_ptr.ptr = (void *) Z_LVAL_P(return_value);
		Z_TYPE(new_index_ptr) = le_index_ptr;
		if (zend_hash_update(&EG(regular_list),hashed_details,hashed_details_length+1,(void *) &new_index_ptr, sizeof(list_entry), NULL)==FAILURE) {
			efree(hashed_details);
			RETURN_FALSE;
		}
		zend_hash_add(ORA(conns),(void*)&db_conn,sizeof(void*),(void*)&db_conn,sizeof(void*),NULL);
		ORA(num_links)++;
	}

	efree(hashed_details);
}
/* }}} */

/* {{{ proto int ora_logoff(int connection)
   Close an Oracle connection */
PHP_FUNCTION(ora_logoff)
{								/* conn_index */
	oraConnection *conn;
	pval **arg;

	if (zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE2(conn, oraConnection *, arg, -1, "Oracle-Connection", le_conn, le_pconn);

	zend_list_delete(Z_LVAL_PP(arg));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ora_open(int connection)
   Open an Oracle cursor */
PHP_FUNCTION(ora_open)
{								/* conn_index */
	pval **arg;
	oraConnection *conn = NULL;
	oraCursor *cursor = NULL;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE2(conn, oraConnection *, arg, -1, "Oracle-Connection", le_conn, le_pconn);

	cursor = (oraCursor *)emalloc(sizeof(oraCursor));
	memset(cursor, 0, sizeof(oraCursor));
	if (oopen(&cursor->cda, &conn->lda, (text *) 0, -1, -1, (text *) 0, -1)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to open new cursor (%s)",
				   ora_error(&cursor->cda));
		efree(cursor);
		RETURN_FALSE;
	}
	cursor->open = 1;
	cursor->conn_ptr = conn;	
	ZEND_REGISTER_RESOURCE(return_value, cursor, le_cursor);
	cursor->conn_id = Z_LVAL_P(return_value);
}

/* }}} */

/* {{{ proto int ora_close(int cursor)
   Close an Oracle cursor */
PHP_FUNCTION(ora_close)
{								/* conn_index */
	pval **arg;
	oraCursor *cursor;

	if (zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(cursor, oraCursor *, arg, -1, "Oracle-Cursor", le_cursor);

	zend_list_delete(Z_LVAL_PP(arg));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ora_commitoff(int connection)
   Disable automatic commit */
PHP_FUNCTION(ora_commitoff)
{								/* conn_index */
	pval **arg;
	oraConnection *conn;

	if (zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE2(conn, oraConnection *, arg, -1, "Oracle-Connection", le_conn, le_pconn);

	if (ocof(&conn->lda)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to turn off auto-commit (%s)",
				   ora_error(&conn->lda));
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ora_commiton(int connection)
   Enable automatic commit */
PHP_FUNCTION(ora_commiton)
{								/* conn_index */
	pval **arg;
	oraConnection *conn;

	if (zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE2(conn, oraConnection *, arg, -1, "Oracle-Connection", le_conn, le_pconn);

	if (ocon(&conn->lda)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to turn on auto-commit (%s)",
				   ora_error(&conn->lda));
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ora_commit(int connection)
   Commit an Oracle transaction */
PHP_FUNCTION(ora_commit)
{								/* conn_index */
	pval **arg;
	oraConnection *conn;

	if (zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE2(conn, oraConnection *, arg, -1, "Oracle-Connection", le_conn, le_pconn);

	if (ocom(&conn->lda)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to commit transaction (%s)",
				   ora_error(&conn->lda));
		RETURN_FALSE;
	}
	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int ora_rollback(int connection)
   Roll back an Oracle transaction */
PHP_FUNCTION(ora_rollback)
{								/* conn_index */
	pval **arg;
	oraConnection *conn;

	if (zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE2(conn, oraConnection *, arg, -1, "Oracle-Connection", le_conn, le_pconn);

	if (orol(&conn->lda)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to roll back transaction (%s)",
				   ora_error(&conn->lda));
		RETURN_FALSE;
	}
	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int ora_parse(int cursor, string sql_statement [, int defer])
   Parse an Oracle SQL statement */
PHP_FUNCTION(ora_parse)
{	
    pval **curs, **sql, **def;
	oraCursor *cursor;
	sword defer = 0;
	text *query;

	switch (ZEND_NUM_ARGS()) {
	case 3:
		zend_get_parameters_ex(3,&curs,&sql,&def);
		convert_to_long_ex(def);
		if (Z_LVAL_PP(def)) {
			defer = DEFER_PARSE;
		}
		break;
	case 2:
		zend_get_parameters_ex(2,&curs,&sql);
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(sql);
 	query = (text *) estrndup(Z_STRVAL_PP(sql),Z_STRLEN_PP(sql));

	if (query == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid query");
		RETURN_FALSE;
	}

	if (!(cursor = ora_get_cursor(&EG(regular_list),curs TSRMLS_CC))){
		efree(query);
		RETURN_FALSE;
	}

	if (cursor->query) {
		efree(cursor->query);
	}

	cursor->query = query;
	cursor->fetched = 0;

	if (cursor->params && cursor->nparams > 0){
		zend_hash_destroy(cursor->params);
		efree(cursor->params);
		cursor->params = NULL;
		cursor->nparams = 0;
	}
	
	if (oparse(&cursor->cda, query, (sb4) - 1, defer, VERSION_7)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Ora_Parse failed (%s)",ora_error(&cursor->cda));
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ora_bind(int cursor, string php_variable_name, string sql_parameter_name, int length [, int type])
   Bind a PHP variable to an Oracle parameter */
PHP_FUNCTION(ora_bind)
{ 
	pval **curs, **pvar, **svar, **plen, **ptyp;
	int inout = 0;
	oraParam *newparam, *paramptr;
	oraCursor *cursor;
	char *paramname;

	switch (ZEND_NUM_ARGS()) {
	case 5:
		zend_get_parameters_ex(5,&curs,&pvar,&svar,&plen,&ptyp);
		convert_to_long_ex(ptyp);
		inout = Z_LVAL_PP(ptyp);
		break;
	case 4:
		zend_get_parameters_ex(4,&curs,&pvar,&svar,&plen);
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	cursor = ora_get_cursor(&EG(regular_list), curs TSRMLS_CC);
	if (cursor == NULL) {
		RETURN_FALSE;
	}

	convert_to_string_ex(pvar);
	convert_to_string_ex(svar);
	convert_to_long_ex(plen);

	if (cursor->params == NULL) {
		ALLOC_HASHTABLE(cursor->params);
		if (!cursor->params ||
			zend_hash_init(cursor->params, 19, NULL,
						   HASH_DTOR pval_ora_param_destructor, 0) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unable to initialize parameter list");
			RETURN_FALSE;
		}
	}
	newparam = (oraParam *)emalloc(sizeof(oraParam));

	if ((paramname = estrndup(Z_STRVAL_PP(pvar), Z_STRLEN_PP(pvar))) == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Out of memory for parametername");
		efree(newparam);
		RETURN_FALSE;
	}

	if (zend_hash_add(cursor->params, paramname, Z_STRLEN_PP(pvar) + 1, 
					  newparam, sizeof(oraParam), (void **)&paramptr) == FAILURE) {
		/* XXX zend_hash_destroy */
		efree(paramname);
		efree(newparam);
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Could not make parameter placeholder");
		RETURN_FALSE;
	}

	efree(newparam);
	efree(paramname);
	
	paramptr->progvl = Z_LVAL_PP(plen) + 1;
	paramptr->inout = inout;

	paramptr->progv = (text *)emalloc(paramptr->progvl);

/* XXX Maximum for progvl */
	paramptr->alen = paramptr->progvl;

	if (obndra(&cursor->cda,              
			   Z_STRVAL_PP(svar),
			   -1,
			   (ub1 *)paramptr->progv,
			   paramptr->progvl,
			   SQLT_STR, /* ftype */
			   -1, /* scale */
			   0/*&paramptr->ind*/, /* ind */
			   &paramptr->alen, /* alen */
			   0 /*&paramptr->arcode*/,
			   0, /* maxsize */
			   0,
			   0,
			   -1,
			   -1)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Ora_Bind failed (%s)",
				   ora_error(&cursor->cda));
		RETURN_FALSE;
	}

	cursor->nparams++;
	RETURN_TRUE;
}
/* }}} */

/*
  XXX Make return values compatible with old module ? 
 */
/* {{{ proto int ora_exec(int cursor)
   Execute a parsed statement */
PHP_FUNCTION(ora_exec)
{								/* cursor_index */
	pval **arg;
	oraCursor *cursor = NULL;

	if (zend_get_parameters_ex(1, &arg) == FAILURE)
		WRONG_PARAM_COUNT;

	if ((cursor = ora_get_cursor(&EG(regular_list), arg TSRMLS_CC)) == NULL) {
		RETURN_FALSE;
	}

	if (cursor->cda.ft == FT_SELECT) {
		if (ora_describe_define(cursor) < 0) {
			/* error message is given by ora_describe_define() */
			RETURN_FALSE;
		}
	}

	if(cursor->nparams > 0){
		if(!ora_set_param_values(cursor, 0 TSRMLS_CC)){
			RETURN_FALSE;
		}
	}

	if (oexec(&cursor->cda)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Ora_Exec failed (%s)",
				   ora_error(&cursor->cda));
		RETURN_FALSE;
	}
	
	if(cursor->nparams > 0){
		if(!ora_set_param_values(cursor, 1 TSRMLS_CC)){
			RETURN_FALSE;
		}
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ora_numcols(int cursor)
   Returns the numbers of columns in a result */
PHP_FUNCTION(ora_numcols)
{								/* cursor_index */
	pval **arg;
	oraCursor *cursor = NULL;

	if (zend_get_parameters_ex(1, &arg) == FAILURE)
		WRONG_PARAM_COUNT;

	if ((cursor = ora_get_cursor(&EG(regular_list), arg TSRMLS_CC)) == NULL) {
		RETURN_FALSE;
	}

	RETURN_LONG(cursor->ncols); 
}
/* }}} */

/* {{{ proto int ora_numrows(int cursor)
   Returns the number of rows in a result */
PHP_FUNCTION(ora_numrows)
{								/* cursor_index */
	pval **arg;
	oraCursor *cursor = NULL;

	if(zend_get_parameters_ex(1, &arg) == FAILURE)
		WRONG_PARAM_COUNT;

	if((cursor = ora_get_cursor(&EG(regular_list), arg TSRMLS_CC)) == NULL) {
		RETURN_FALSE;
	}

	RETURN_LONG(cursor->cda.rpc); 
}
/* }}} */

/* prepares/executes/fetches 1st row if avail*/
/* {{{ proto int ora_do(int connection, int cursor)
   Parse and execute a statement and fetch first result row */ 
PHP_FUNCTION(ora_do)
{
	pval **con,**sql;
	oraConnection *conn = NULL;
	oraCursor *cursor = NULL;
	text *query;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &con,&sql) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE2(conn, oraConnection *, con, -1, "Oracle-Connection", le_conn, le_pconn);

	convert_to_string_ex(sql);

	cursor = (oraCursor *)emalloc(sizeof(oraCursor));

	memset(cursor, 0, sizeof(oraCursor));

	query = (text *) estrndup(Z_STRVAL_PP(sql),Z_STRLEN_PP(sql));

	if (query == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid query in Ora_Do");
		RETURN_FALSE;
	}
	
	cursor->query = query;
	
	if (oopen(&cursor->cda, &conn->lda, (text *) 0, -1, -1, (text *) 0, -1)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to open new cursor (%s)",
				  ora_error(&cursor->cda));
		efree(cursor);
		RETURN_FALSE;
	}
	cursor->open = 1;
	cursor->conn_ptr = conn;	
	cursor->conn_id = Z_LVAL_PP(con);	
	
	/* Prepare stmt */

	if (oparse(&cursor->cda, query, (sb4) - 1, 1, VERSION_7)){
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Ora_Do failed (%s)",
				  ora_error(&cursor->cda));
		_close_oracur(cursor TSRMLS_CC);
		RETURN_FALSE;
	}
	
	/* Execute stmt (and fetch 1st row for selects) */
	if (cursor->cda.ft == FT_SELECT) {
		if (ora_describe_define(cursor) < 0){
			/* error message is given by ora_describe_define() */
			_close_oracur(cursor TSRMLS_CC);
			RETURN_FALSE;
		}
		if (oexfet(&cursor->cda, 1, 0, 0)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Ora_Do failed (%s)",
					  ora_error(&cursor->cda));
			_close_oracur(cursor TSRMLS_CC);
			RETURN_FALSE;
		}
		cursor->fetched = 1;
	} else {
		if (oexec(&cursor->cda)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Ora_Do failed (%s)",
					  ora_error(&cursor->cda));
			_close_oracur(cursor TSRMLS_CC);
			RETURN_FALSE;
		}
	}
	
	ZEND_REGISTER_RESOURCE(return_value, cursor, le_cursor);
}
/* }}} */

/* {{{ proto int ora_fetch(int cursor)
   Fetch a row of result data from a cursor */
PHP_FUNCTION(ora_fetch)
{								/* cursor_index */
	pval **arg;
	oraCursor *cursor;

	if (zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if ((cursor = ora_get_cursor(&EG(regular_list), arg TSRMLS_CC)) == NULL) {
		RETURN_FALSE;
	}

	if (cursor->ncols == 0){
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No tuples available on this cursor");
		RETURN_FALSE;
	}

	/* Get data from Oracle */
	if (ofetch(&cursor->cda)) {
		if (cursor->cda.rc != NO_DATA_FOUND) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Ora_Fetch failed (%s)",
					   ora_error(&cursor->cda));
		}
		RETURN_FALSE;
	}
	cursor->fetched++;
	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int ora_fetch_into(int cursor, array result [, int flags])
   Fetch a row into the specified result array */
PHP_FUNCTION(ora_fetch_into)
{
	pval **curs, **arr, **flg, *tmp;
	oraCursor *cursor;
	int i;
	int flags = 0;

	switch(ZEND_NUM_ARGS()){
	case 2:
		zend_get_parameters_ex(2, &curs, &arr);
		break;
		
	case 3:
		zend_get_parameters_ex(3, &curs, &arr, &flg);
		convert_to_long_ex(flg);
		flags = Z_LVAL_PP(flg);
		break;
		
	default:
		WRONG_PARAM_COUNT;
		break;
	}
	
	/* Find the cursor */
	if ((cursor = ora_get_cursor(&EG(regular_list), curs TSRMLS_CC)) == NULL) {
		RETURN_FALSE;
	}

	if (cursor->ncols == 0){
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No tuples available on this cursor");
		RETURN_FALSE;
	}

	if (ofetch(&cursor->cda)) {
		if (cursor->cda.rc != NO_DATA_FOUND) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Ora_Fetch_Into failed (%s)",ora_error(&cursor->cda));
		}
		RETURN_FALSE;
	}
	cursor->fetched++;

	if (Z_TYPE_PP(arr) != IS_ARRAY){
		pval_destructor(*arr);
		array_init(*arr);
	}
	zend_hash_internal_pointer_reset(Z_ARRVAL_PP(arr));

	for (i = 0; i < cursor->ncols; i++) {
		if (cursor->columns[i].col_retcode == 1405) {
			if (!(flags&ORA_FETCHINTO_NULLS)){
				continue; /* don't add anything for NULL columns, unless the calles wants it */
			} else {
				MAKE_STD_ZVAL(tmp);
				ZVAL_NULL(tmp);
			}
		} else if (cursor->columns[i].col_retcode != 0 &&
				   cursor->columns[i].col_retcode != 1406) {
			/* So error fetching column.  The most common is 1405, a NULL */
			/* was retreived.  1406 is ASCII or string buffer data was */
			/* truncated. The converted data from the database did not fit */
			/* into the buffer.  Since we allocated the buffer to be large */
			/* enough, this should not occur.  Anyway, we probably want to */
			/* return what we did get, in that case */
			RETURN_FALSE;
		} else {
			MAKE_STD_ZVAL(tmp);

			Z_TYPE_P(tmp) = IS_STRING;
			Z_STRLEN_P(tmp) = 0;

			switch(cursor->columns[i].dbtype) {
				case SQLT_LNG:
				case SQLT_LBI:
					{ 
						ub4 ret_len;
						int offset = cursor->columns[i].col_retlen;
						sb2 result;
						
						if (cursor->columns[i].col_retcode == 1406) { /* truncation -> get the rest! */
							while (1) {
								cursor->columns[i].buf = erealloc(cursor->columns[i].buf,offset + DB_SIZE + 1);
								
								if (! cursor->columns[i].buf) {
									offset = 0;
									break;
								}
								
								result = oflng(&cursor->cda, 
											   (sword)(i + 1),
											   cursor->columns[i].buf + offset, 
											   DB_SIZE, 
											   1,
											   &ret_len, 
											   offset);
								if (result) {
									break;
								}
								
								if (ret_len <= 0) {
									break;
								}
								
								offset += ret_len;
							}
						}
						if (cursor->columns[i].buf && offset) {
							Z_STRLEN_P(tmp) = offset;
						} else {
							Z_STRLEN_P(tmp) = 0;
						}
					}
					break;
				default:
					Z_STRLEN_P(tmp) = min(cursor->columns[i].col_retlen,
											 cursor->columns[i].dsize);
					break;
			}
			Z_STRVAL_P(tmp) = estrndup(cursor->columns[i].buf,Z_STRLEN_P(tmp));
		}

		if (flags&ORA_FETCHINTO_ASSOC){
			zend_hash_update(Z_ARRVAL_PP(arr), cursor->columns[i].cbuf, cursor->columns[i].cbufl+1, (void *) &tmp, sizeof(pval*), NULL);
		} else {
			zend_hash_index_update(Z_ARRVAL_PP(arr), i, (void *) &tmp, sizeof(pval*), NULL);
		}

	}

	RETURN_LONG(cursor->ncols); 
}
/* }}} */

/* {{{ proto string ora_columnname(int cursor, int column)
   Get the name of an Oracle result column */
PHP_FUNCTION(ora_columnname)
{								/* cursor_index, column_index */
	pval **curs, **col;
	oraCursor *cursor = NULL;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &curs, &col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if ((cursor = ora_get_cursor(&EG(regular_list), curs TSRMLS_CC)) == NULL) {
		RETURN_FALSE;
	}

	convert_to_long_ex(col);
	
	if (cursor->ncols == 0){
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No tuples available at this cursor index");
		RETURN_FALSE;
	}
        
	if (Z_LVAL_PP(col) >= cursor->ncols){
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Column index larger than number of columns");
		RETURN_FALSE;
	}

	if (Z_LVAL_PP(col) < 0){
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Column numbering starts at 0");
		RETURN_FALSE;
	}
        
	RETURN_STRINGL(cursor->columns[Z_LVAL_PP(col)].cbuf,
				   cursor->columns[Z_LVAL_PP(col)].cbufl,1);
}
/* }}} */

/* {{{ proto string ora_columntype(int cursor, int column) 
   Get the type of an Oracle result column */
PHP_FUNCTION(ora_columntype)
{								/* cursor_index, column_index */
	pval **curs, **col;
	int colno;
	oraCursor *cursor = NULL;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &curs, &col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if ((cursor = ora_get_cursor(&EG(regular_list), curs TSRMLS_CC)) == NULL) {
		RETURN_FALSE;
	}

	convert_to_long_ex(col);
	colno = Z_LVAL_PP(col);

	if (cursor->ncols == 0){
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No tuples available at this cursor index");
		RETURN_FALSE;
	}
        
	if (colno >= cursor->ncols){
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Column index larger than number of columns");
		RETURN_FALSE;
	}

	if (colno < 0){
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Column numbering starts at 0");
		RETURN_FALSE;
	}

	switch (cursor->columns[colno].dbtype) {
		case SQLT_CHR:
			RETURN_STRINGL("VARCHAR2", 8, 1);
		case SQLT_VCS:
	    case SQLT_AVC:
			RETURN_STRINGL("VARCHAR", 7, 1);
		case SQLT_STR:
	    case SQLT_AFC:
			RETURN_STRINGL("CHAR", 4, 1);
		case SQLT_NUM: case SQLT_INT:
		case SQLT_FLT: case SQLT_UIN:
			RETURN_STRINGL("NUMBER", 6, 1);
		case SQLT_LNG:
			RETURN_STRINGL("LONG", 4, 1);
		case SQLT_LBI:
			RETURN_STRINGL("LONG RAW", 8, 1);
		case SQLT_RID:
			RETURN_STRINGL("ROWID", 5, 1);
		case SQLT_DAT:
			RETURN_STRINGL("DATE", 4, 1);
#ifdef SQLT_CUR
		case SQLT_CUR:
			RETURN_STRINGL("CURSOR", 6, 1);
#endif
		default:
		{
			char numbuf[21];
			snprintf(numbuf, 20, "UNKNOWN(%d)", cursor->columns[colno].dbtype);
			numbuf[20] = '\0';
			RETVAL_STRING(numbuf,1);
		}
	}
}
/* }}} */

/* {{{ proto int ora_columnsize(int cursor, int column)
   Return the size of the column */
PHP_FUNCTION(ora_columnsize)
{								/* cursor_index, column_index */
	pval **curs, **col;
	oraCursor *cursor = NULL;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &curs, &col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	/* Find the cursor */
	if ((cursor = ora_get_cursor(&EG(regular_list), curs TSRMLS_CC)) == NULL) {
		RETURN_FALSE;
	}

	convert_to_long_ex(col);
	
	if (cursor->ncols == 0){
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No tuples available at this cursor index");
		RETURN_FALSE;
	}
        
	if (Z_LVAL_PP(col) >= cursor->ncols){
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Column index larger than number of columns");
		RETURN_FALSE;
	}

	if (Z_LVAL_PP(col) < 0){
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Column numbering starts at 0");
		RETURN_FALSE;
	}
        
	RETURN_LONG(cursor->columns[Z_LVAL_PP(col)].dbsize);
}
/* }}} */

/* {{{ proto mixed ora_getcolumn(int cursor, int column)
   Get data from a fetched row */
PHP_FUNCTION(ora_getcolumn)
{								/* cursor_index, column_index */
	pval **curs,**col;
	int colno;
	oraCursor *cursor = NULL;
	oraColumn *column = NULL;
	int len;
	sb2 type;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &curs, &col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if ((cursor = ora_get_cursor(&EG(regular_list), curs TSRMLS_CC)) == NULL) {
		RETURN_FALSE;
	}

	if (cursor->ncols == 0){
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No tuples available at this cursor index");
		RETURN_FALSE;
	}

	convert_to_long_ex(col);
	colno = Z_LVAL_PP(col);        

	if (colno >= cursor->ncols){
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Column index larger than number of columns");
		RETURN_FALSE;
	}

	if (colno < 0){
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Column numbering starts at 0");
		RETURN_FALSE;
	}

	if (cursor->fetched == 0){
		if (ofetch(&cursor->cda)) {
			if (cursor->cda.rc != NO_DATA_FOUND) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Ora_Fetch failed (%s)",
						   ora_error(&cursor->cda));
			}
			RETURN_FALSE;
		}
		cursor->fetched++;		
	}

 	column = &cursor->columns[colno]; 

 	type = column->dbtype; 

	if (column->col_retcode == 1405) {
		RETURN_NULL();
	}

	if (column->col_retcode != 0 && column->col_retcode != 1406) {
		/* So error fetching column.  The most common is 1405, a NULL
		 * was retreived.  1406 is ASCII or string buffer data was
		 * truncated. The converted data from the database did not fit
		 * into the buffer.  Since we allocated the buffer to be large
		 * enough, this should not occur.  Anyway, we probably want to
		 * return what we did get, in that case
		 */
		RETURN_FALSE;
	} else {
		switch(type) {
		case SQLT_CHR:
		case SQLT_NUM:
		case SQLT_INT: 
		case SQLT_FLT:
		case SQLT_STR:
		case SQLT_UIN:
		case SQLT_AFC:
		case SQLT_AVC:
		case SQLT_DAT:
			len = min(column->col_retlen, column->dsize);
			RETURN_STRINGL(column->buf,len,1);

		case SQLT_LNG:
		case SQLT_LBI:
			{ 
				ub4 ret_len;
				int offset = column->col_retlen;
				sb2 result;
				
				if (column->col_retcode == 1406) { /* truncation -> get the rest! */
					while (1) {
						column->buf = erealloc(column->buf,offset + DB_SIZE + 1);
						
						if (! column->buf) {
							offset = 0;
							break;
						}
						
						result = oflng(&cursor->cda, 
									   (sword)(colno + 1),
									   column->buf + offset, 
									   DB_SIZE, 
									   1,
									   &ret_len, 
									   offset);
						if (result) {
							break;
						}
						
						if (ret_len <= 0) {
							break;
						}
						
						offset += ret_len;
					}
				}
				if (column->buf && offset) {
					RETURN_STRINGL(column->buf, offset, 1);
				} else {
					RETURN_FALSE;
				}
			}
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Ora_GetColumn found invalid type (%d)", type);
			RETURN_FALSE;
		}
	}
}
/* }}} */

/* {{{ proto string ora_error(int cursor_or_connection)
   Get an Oracle error message */
PHP_FUNCTION(ora_error)
{
	pval **arg;
	oraCursor *cursor;
	oraConnection *conn;
	void *res;
	int what;
	int argc = ZEND_NUM_ARGS();

	if (argc < 0 || argc >> 1 || zend_get_parameters_ex(argc, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (argc == 1) {
		res = zend_fetch_resource(arg TSRMLS_CC, -1,"Oracle-Connection/Cursor",&what,3,le_conn, le_pconn, le_cursor);
		ZEND_VERIFY_RESOURCE(res);

		if (what == le_cursor) {
			cursor = (oraCursor *) res;
			RETURN_STRING(ora_error(&cursor->cda),1);
		} else {
			conn = (oraConnection *) res;
			RETURN_STRING(ora_error(&conn->lda),1);
		}
	} else {
		RETURN_STRING(ora_error(&ORA(db_err_conn).lda),1);
	}
}
/* }}} */

/* {{{ proto int ora_errorcode(int cursor_or_connection)
   Get an Oracle error code */
PHP_FUNCTION(ora_errorcode)
{
	pval **arg;
	oraCursor *cursor;
	oraConnection *conn;
	void *res;
	int what;
	int argc = ZEND_NUM_ARGS();

	if (argc < 0 || argc >> 1 || zend_get_parameters_ex(argc, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (argc == 1) {
		res = zend_fetch_resource(arg TSRMLS_CC, -1,"Oracle-Connection/Cursor",&what,3,le_conn, le_pconn, le_cursor);
		ZEND_VERIFY_RESOURCE(res);

		if (what == le_cursor) {
			cursor = (oraCursor *) res;
			RETURN_LONG(cursor->cda.rc);
		} else {
			conn = (oraConnection *) res;
			RETURN_LONG(conn->lda.rc);
		}
	} else {
		RETURN_LONG(ORA(db_err_conn).lda.rc);
	}
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(oracle)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Oracle Support", "enabled");
#ifndef PHP_WIN32
    php_info_print_table_row(2, "Oracle Version", PHP_ORACLE_VERSION );
    php_info_print_table_row(2, "Compile-time ORACLE_HOME", PHP_ORACLE_DIR );
    php_info_print_table_row(2, "Libraries Used", PHP_ORACLE_SHARED_LIBADD );                                                                                       
#endif
	php_info_print_table_end();
}
/* }}} */

/*
** Functions internal to this module.
*/

/* {{{ ora_get_cursor
 */
static oraCursor *
ora_get_cursor(HashTable *list, pval **ind TSRMLS_DC)
{
	oraCursor *cursor;
	oraConnection *db_conn;

	cursor = (oraCursor *) zend_fetch_resource(ind TSRMLS_CC, -1, "Oracle-Cursor", NULL, 1, le_cursor);
	if (! cursor) {
		return NULL;
	}

	if (zend_hash_find(ORA(conns),(void*)&(cursor->conn_ptr),sizeof(void*),(void **)&db_conn) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Connection already closed for cursor index %d", ind);
		return NULL;
	}

	return cursor;
}
/* }}} */

/* {{{ ora_error
 */
static char *
ora_error(Cda_Def * cda)
{
	sword n, l;
	static text errmsg[ 512 ];

	n = oerhms(cda, cda->rc, errmsg, 400);

	/* remove the last newline */
	l = strlen(errmsg);
	if (l < 400 && errmsg[l - 1] == '\n') {
		errmsg[l - 1] = '\0';
		l--;
	}
	if (cda->fc > 0) {
		strcat(errmsg, " -- while processing OCI function ");
		strncat(errmsg, ora_func_tab[cda->fc], 75);  /* 512 - 400 - 36 */
	}
	return (char *) errmsg;
}
/* }}} */

/* {{{ ora_describe_define
 */
static sword
ora_describe_define(oraCursor * cursor)
{
	long col = 0;
	int i;
	sb2 type;
	sb4 dbsize;

	if (cursor == NULL) {
		return -1;
	}

	if (cursor->columns) {
		for(i = 0; i < cursor->ncols; i++){
			if (cursor->columns[i].buf)
				efree(cursor->columns[i].buf);
		}
		efree(cursor->columns);
	} 

	cursor->ncols = 0;

	while(1){
		if (odescr(&cursor->cda, (sword) cursor->ncols + 1, &dbsize, (sb2 *)0, (sb1 *)0, 
			   (sb4 *)0, (sb4 *)0,	(sb2 *)0, (sb2 *)0, (sb2 *)0)){
			if (cursor->cda.rc == VAR_NOT_IN_LIST) {
				break;
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", ora_error(&cursor->cda));
				cursor->ncols = 0;
				return -1;
			}
		}
		cursor->ncols++;
	}

	if (cursor->ncols > 0){
		cursor->columns = (oraColumn *) emalloc(sizeof(oraColumn) * cursor->ncols);
		memset(cursor->columns,0,sizeof(oraColumn) * cursor->ncols);
	}

	for(col = 0; col < cursor->ncols; col++){
		cursor->columns[col].cbufl = ORANAMELEN;
		
		if (odescr(&cursor->cda, (sword)col + 1, &cursor->columns[col].dbsize,
				   &cursor->columns[col].dbtype, &cursor->columns[col].cbuf[0],
				   &cursor->columns[col].cbufl, &cursor->columns[col].dsize,
				   &cursor->columns[col].prec, &cursor->columns[col].scale,
				   &cursor->columns[col].nullok)) {
			if (cursor->cda.rc == VAR_NOT_IN_LIST) {
				break;
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", ora_error(&cursor->cda));
				return -1;
			}
		}

		cursor->columns[col].cbuf[cursor->columns[col].cbufl] = '\0';

		switch (cursor->columns[col].dbtype) {
			case SQLT_LBI:
				cursor->columns[col].dsize = DB_SIZE;
				type = SQLT_LBI;
				break;
			case SQLT_LNG: 
				cursor->columns[col].dsize = DB_SIZE;
			default:
				type = SQLT_STR;
				break;
		}
		
		cursor->columns[col].buf = (ub1 *) emalloc(cursor->columns[col].dsize + 1);

		/* Define an output variable for the column */
		if (odefin(&cursor->cda, (sword)col + 1, cursor->columns[col].buf, 
				   cursor->columns[col].dsize + 1, type, -1, &cursor->columns[col].indp,
				   (text *) 0, -1, -1, &cursor->columns[col].col_retlen, 
				   &cursor->columns[col].col_retcode)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", ora_error(&cursor->cda));
			return -1;
		}
	}
	return 1;
}
/* }}} */

/* {{{ ora_set_param_values
 */
int ora_set_param_values(oraCursor *cursor, int isout TSRMLS_DC)
{
	char *paramname;
	oraParam *param;
	pval **pdata;
	int i, len, plen;

	zend_hash_internal_pointer_reset(cursor->params);

	if(zend_hash_num_elements(cursor->params) != cursor->nparams){
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Mismatch in number of parameters");
		return 0;
	}

	for(i = 0; i < cursor->nparams; i++, zend_hash_move_forward(cursor->params)){
		if(zend_hash_get_current_key(cursor->params, &paramname, NULL, 0) != HASH_KEY_IS_STRING){
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't get parameter name");
			return 0;
		}

		if(zend_hash_get_current_data(cursor->params, (void **)&param) == FAILURE){
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't get parameter data");
			return 0;
		}

		if(isout){
			SET_VAR_STRINGL(paramname, estrdup(param->progv), strlen(param->progv));
			continue;
		}
		
		/* doing the in-loop */

		if (zend_hash_find(&EG(symbol_table), paramname, strlen(paramname) + 1, (void **)&pdata) == FAILURE){
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't find variable for parameter");
			return 0;
		}

		convert_to_string(*pdata);
		plen = Z_STRLEN_PP(pdata);

 		if (param->progvl <= plen){
  			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Input value will be truncated");
  		}

		len = min(param->progvl - 1, plen);

		strncpy(param->progv, Z_STRVAL_PP(pdata), len);
		param->progv[len] = '\0';
	}

	return 1;
}
/* }}} */

#endif							/* HAVE_ORACLE */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
