/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-1999 PHP Development Team (See Credits file)      |
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
   | Authors: Stig Sæther Bakken <ssb@fast.no>                            |
   |          Mitch Golden <mgolden@interport.net>                        |
   |          Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Andreas Karajannis <Andreas.Karajannis@gmd.de>              |
   |          Thies C. Arntzen <thies@digicol.de>                         |
   +----------------------------------------------------------------------+
 */
#if defined(COMPILE_DL)
# if PHP_31
#  include "../phpdl.h"
# else
#  ifdef THREAD_SAFE
#  undef THREAD_SAFE
#  endif
#  include "dl/phpdl.h"
# endif
#endif
#include "php.h"

#if PHP_API_VERSION < 19990421 
  #include "internal_functions.h"
  #include "php3_list.h"
  #include "head.h"
#else
  #include "zend_globals.h"
#endif

#if HAVE_ORACLE

#include "php3_oracle.h"
#ifdef WIN32
# include "variables.h"
#else
# include "build-defs.h"
#endif


#include "snprintf.h"

#ifndef min
#define min(a, b) ((a) > (b) ? (b) : (a))
#endif

#ifdef THREAD_SAFE

void *oracle_mutex;
DWORD ORACLETls;
static int numthreads=0;

typedef struct oracle_global_struct {
	oracle_module php3_oracle_module;
} oracle_global_struct;

#define ORACLE_GLOBAL(a) oracle_globals->a

#define ORACLE_TLS_VARS \
	oracle_global_struct *oracle_globals = TlsGetValue(ORACLETls); 

#else
oracle_module php3_oracle_module;
#define ORACLE_GLOBAL(a) a
#define ORACLE_TLS_VARS
#endif

#undef ORACLE_DEBUG

#define DB_SIZE 65536

#define ORA_FETCHINTO_ASSOC (1<<0)
#define ORA_FETCHINTO_NULLS (1<<1)

static oraConnection *ora_get_conn(HashTable *, int);
static int ora_add_cursor(HashTable *, oraCursor *);
static oraCursor *ora_get_cursor(HashTable *, int);
static void ora_del_cursor(HashTable *, int);
static char *ora_error(Cda_Def *);
static int ora_describe_define(oraCursor *);
static int _cursors_cleanup(list_entry *le);
static int _close_oraconn(oraConnection *conn);
static int _close_orapconn(oraConnection *conn);
static int _close_oracur(oraCursor *cur);
static int _ora_ping(oraConnection *conn);
int ora_set_param_values(oraCursor *cursor, int isout);

void php3_Ora_Do_Logon(INTERNAL_FUNCTION_PARAMETERS, int persistent);

function_entry oracle_functions[] = {
	{"ora_bind",		php3_Ora_Bind,	       	NULL},
	{"ora_close",		php3_Ora_Close,	       	NULL},
	{"ora_commit",		php3_Ora_Commit,       	NULL},
	{"ora_commitoff",	php3_Ora_CommitOff,    	NULL},
	{"ora_commiton",	php3_Ora_CommitOn,     	NULL},
	{"ora_do",          php3_Ora_Do,          	NULL},
	{"ora_error",		php3_Ora_Error,	       	NULL},
	{"ora_errorcode",	php3_Ora_ErrorCode,    	NULL},
	{"ora_exec",		php3_Ora_Exec,	       	NULL},
	{"ora_fetch",		php3_Ora_Fetch,	       	NULL},
   	{"ora_fetch_into",	php3_Ora_FetchInto,   	NULL},
	{"ora_columntype",	php3_Ora_ColumnType,	NULL},
	{"ora_columnname",	php3_Ora_ColumnName,	NULL},
	{"ora_columnsize",  php3_Ora_ColumnSize,    NULL},
	{"ora_getcolumn",	php3_Ora_GetColumn,	    NULL},
	{"ora_numcols",		php3_Ora_NumCols,	    NULL},
	{"ora_numrows",		php3_Ora_NumRows,	    NULL},
	{"ora_logoff",		php3_Ora_Logoff,       	NULL},
	{"ora_logon",		php3_Ora_Logon,	       	NULL},
	{"ora_plogon",		php3_Ora_PLogon,       	NULL},
	{"ora_open",		php3_Ora_Open,	       	NULL},
	{"ora_parse",		php3_Ora_Parse,	       	NULL},
	{"ora_rollback",	php3_Ora_Rollback,     	NULL},
	{NULL, NULL, NULL}
};

php3_module_entry oracle_module_entry = {
	"Oracle",
	oracle_functions,
	php3_minit_oracle,
	php3_mshutdown_oracle,
	php3_rinit_oracle,
	php3_rshutdown_oracle,
	php3_info_oracle,
    STANDARD_MODULE_PROPERTIES
};

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

#if COMPILE_DL
DLEXPORT php3_module_entry *get_module() { return &oracle_module_entry; };
#endif

#if 0
static int _cursors_cleanup(list_entry *le)
{
  ORACLE_TLS_VARS;

  if (le->type == ORACLE_GLOBAL(php3_oracle_module).le_cursor){
    oraCursor *curs = ((oraCursor *) le->ptr);
    oraConnection *conn = curs->conn_ptr;

	if (! conn->open) 
		return 1;

	/*
    if ((!(conn->open)) && (curs->open > 0)){
      oclose(&curs->cda);
      curs->open = 0;
    }
	*/

	return 1;
  }

  return 0;
}
#endif

static int _close_oraconn(oraConnection *conn)
{
	ORACLE_TLS_VARS;
	
	conn->open = 0;

	/* zeev says, this is a no-no (thies 990430) 
	_php3_hash_apply(ORACLE_GLOBAL(php3_oracle_module).resource_list,
					 (int (*)(void *))_cursors_cleanup);
	*/
	
	ologof(&conn->lda);
	ORACLE_GLOBAL(php3_oracle_module).num_links--;
	efree(conn);

	return 1;
}

static int _close_orapconn(oraConnection *conn)
{
	ORACLE_TLS_VARS;
  
	conn->open = 0;

	/* zeev says, this is a no-no (thies 990430) 
	_php3_hash_apply(ORACLE_GLOBAL(php3_oracle_module).resource_plist,
					 (int (*)(void *))_cursors_cleanup);
	*/
	
	ologof(&conn->lda);
	free(conn);
	ORACLE_GLOBAL(php3_oracle_module).num_links--;
	ORACLE_GLOBAL(php3_oracle_module).num_persistent--;

	return 1;
}

static int
pval_ora_param_destructor(oraParam *param)
{
	if (! param) {
		return 0;
	}

	if (param->progv) {
		efree(param->progv);
	}
	return 0;
}


static int _close_oracur(oraCursor *cur)
{
	int i;

	if (cur){
		if (cur->query){
			efree(cur->query);
		}
		if (cur->params){
			_php3_hash_destroy(cur->params);
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
			oclose(&cur->cda);
			cur->open = 0;
		}
	   	efree(cur);
	}
	
	return 1;
}

int php3_minit_oracle(INIT_FUNC_ARGS)
{
#if defined(THREAD_SAFE)
	oracle_global_struct *oracle_globals;
	PHP3_MUTEX_ALLOC(oracle_mutex);
	PHP3_MUTEX_LOCK(oracle_mutex);
	numthreads++;
	if (numthreads==1){
		if (!PHP3_TLS_PROC_STARTUP(ORACLETls)){
			PHP3_MUTEX_UNLOCK(oracle_mutex);
			PHP3_MUTEX_FREE(oracle_mutex);
			return FAILURE;
		}
	}
	PHP3_MUTEX_UNLOCK(oracle_mutex);
	if(!PHP3_TLS_THREAD_INIT(ORACLETls,oracle_globals,oracle_global_struct)){
		PHP3_MUTEX_FREE(oracle_mutex);
		return FAILURE;
	}
#endif

	if (cfg_get_long("oracle.allow_persistent",
			 &ORACLE_GLOBAL(php3_oracle_module).allow_persistent)
		== FAILURE) {
	  ORACLE_GLOBAL(php3_oracle_module).allow_persistent = -1;
	}
	if (cfg_get_long("oracle.max_persistent",
					 &ORACLE_GLOBAL(php3_oracle_module).max_persistent)
	    == FAILURE) {
		ORACLE_GLOBAL(php3_oracle_module).max_persistent = -1;
	}
	if (cfg_get_long("oracle.max_links",
					 &ORACLE_GLOBAL(php3_oracle_module).max_links)
	    == FAILURE) {
		ORACLE_GLOBAL(php3_oracle_module).max_links = -1;
	}
	
	ORACLE_GLOBAL(php3_oracle_module).num_persistent = 0;
	
	ORACLE_GLOBAL(php3_oracle_module).le_cursor =
		register_list_destructors(_close_oracur, NULL);
	ORACLE_GLOBAL(php3_oracle_module).le_conn =
		register_list_destructors(_close_oraconn, NULL);
	ORACLE_GLOBAL(php3_oracle_module).le_pconn =
		register_list_destructors(NULL, _close_orapconn);

	REGISTER_LONG_CONSTANT("ORA_BIND_INOUT", 0, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ORA_BIND_IN",    1, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ORA_BIND_OUT",   2, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("ORA_FETCHINTO_ASSOC",ORA_FETCHINTO_ASSOC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ORA_FETCHINTO_NULLS",ORA_FETCHINTO_NULLS, CONST_CS | CONST_PERSISTENT);

#if NEEDED
	opinit(OCI_EV_TSF); /* initialize threaded environment - must match the threaded mode 
						   of the oci8 driver if both are used at the same time!! */
#endif

	return SUCCESS;
}

int php3_rinit_oracle(INIT_FUNC_ARGS)
{
	ORACLE_TLS_VARS;
	
	ORACLE_GLOBAL(php3_oracle_module).num_links = 
		ORACLE_GLOBAL(php3_oracle_module).num_persistent;
	/*
	  ORACLE_GLOBAL(php3_oracle_module).defaultlrl = 0;
	  ORACLE_GLOBAL(php3_oracle_module).defaultbinmode = 0;
	  ORACLE_GLOBAL(php3_oracle_module).defaultconn = 0;
	*/
	return SUCCESS;
}


int php3_mshutdown_oracle(SHUTDOWN_FUNC_ARGS)
{
	ORACLE_TLS_VARS;

#ifdef THREAD_SAFE
	PHP3_TLS_THREAD_FREE(oracle_globals);
	PHP3_MUTEX_LOCK(oracle_mutex);
	numthreads--;
	if (numthreads<1) {
		PHP3_TLS_PROC_SHUTDOWN(ORACLETls);
		PHP3_MUTEX_UNLOCK(oracle_mutex);
		PHP3_MUTEX_FREE(oracle_mutex);
		return SUCCESS;
	}
	PHP3_MUTEX_UNLOCK(oracle_mutex);
#endif
	return SUCCESS;

}

int php3_rshutdown_oracle(SHUTDOWN_FUNC_ARGS)
{
	ORACLE_TLS_VARS;

	return SUCCESS;

}

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

/*
   ** PHP functions
*/

/* {{{ proto int ora_logon(string user, string password)
   Open an Oracle connection */
void php3_Ora_Logon(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_Ora_Do_Logon(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int ora_plogon(string user, string password)
   Open a persistant Oracle connection */
void php3_Ora_PLogon(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_Ora_Do_Logon(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

void php3_Ora_Do_Logon(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	char    *user = NULL;
	char    *pwd = NULL;
	pval *arg1, *arg2;
	oraConnection *db_conn;
	list_entry *index_ptr;
	char *hashed_details;
	int hashed_len, len, id;
	ORACLE_TLS_VARS;

	ORACLE_GLOBAL(php3_oracle_module).resource_list = list;
	ORACLE_GLOBAL(php3_oracle_module).resource_plist = plist;
  
	if (getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
  
	convert_to_string(arg1);
	convert_to_string(arg2);
  
	user = arg1->value.str.val;
	pwd = arg2->value.str.val;

	if (!ORACLE_GLOBAL(php3_oracle_module).allow_persistent) {
		persistent = 0;
	}
  
	if (ORACLE_GLOBAL(php3_oracle_module).max_links != -1 &&
		ORACLE_GLOBAL(php3_oracle_module).num_links >=
		ORACLE_GLOBAL(php3_oracle_module).max_links) {
		php3_error(E_WARNING, "Oracle: Too many open links (%d)",
				   ORACLE_GLOBAL(php3_oracle_module).num_links);
		RETURN_FALSE;
	}

	/* the user requested a persistent connection */
	if (persistent && 
		ORACLE_GLOBAL(php3_oracle_module).max_persistent != -1 &&
		ORACLE_GLOBAL(php3_oracle_module).num_persistent >=
		ORACLE_GLOBAL(php3_oracle_module).max_persistent) {
		php3_error(E_WARNING,"Oracle: Too many open persistent links (%d)",
				   ORACLE_GLOBAL(php3_oracle_module).num_persistent);
		RETURN_FALSE;
	}
	
	len = strlen(user) + strlen(pwd) + 9; 
	hashed_details = emalloc(len);

	if (hashed_details == NULL) {
		php3_error(E_WARNING, "Out of memory");
		RETURN_FALSE;
	}

	hashed_len = _php3_sprintf(hashed_details, "ora_%s_%s", user, pwd);

	/* try to find if we already have this link in our persistent list,
	 * no matter if it is to be persistent or not
	 */

	if (_php3_hash_find(plist, hashed_details, hashed_len + 1,
				  (void **) &index_ptr) == FAILURE) {
		/* the link is not in the persistent list */
		list_entry new_index_ptr;

		if (persistent)
			db_conn = (oraConnection *)malloc(sizeof(oraConnection));
		else
			db_conn = (oraConnection *)emalloc(sizeof(oraConnection));
		
		if (db_conn == NULL){
			efree(hashed_details);
			php3_error(E_WARNING, "Out of memory");
			RETURN_FALSE;
		}
		
		memset((void *) db_conn,0,sizeof(oraConnection));	

		if (olog(&db_conn->lda, db_conn->hda, user,
				 strlen(user), pwd, strlen(pwd), 0, -1, OCI_LM_DEF)) {
			php3_error(E_WARNING, "Unable to connect to ORACLE (%s)",
					   ora_error(&db_conn->lda));
			if (persistent)
				free(db_conn);
			else
				efree(db_conn);
			efree(hashed_details);
			RETURN_FALSE;
		}
		
		db_conn->open = 1;
		if (persistent){
			/*new_le.type = ORACLE_GLOBAL(php3_oracle_module).le_pconn;
			  new_le.ptr = db_conn;*/
			return_value->value.lval = 
				php3_plist_insert(db_conn, ORACLE_GLOBAL(php3_oracle_module).le_pconn);
			new_index_ptr.ptr = (void *) return_value->value.lval;
#ifdef THREAD_SAFE
			new_index_ptr.type = _php3_le_index_ptr();
#else
			new_index_ptr.type = le_index_ptr;
#endif
			if (_php3_hash_update(plist,hashed_details,hashed_len + 1,(void *) &new_index_ptr,
							sizeof(list_entry),NULL) == FAILURE) {
				ologof(&db_conn->lda);
				free(db_conn);
				efree(hashed_details);
				php3_error(E_WARNING, "Can't update hashed details list");
				RETURN_FALSE;
			}
			ORACLE_GLOBAL(php3_oracle_module).num_persistent++;
		} else {
			/* non persistent, simply add to list */
			return_value->value.lval = 
				php3_list_insert(db_conn, ORACLE_GLOBAL(php3_oracle_module).le_conn);
		}
		
		ORACLE_GLOBAL(php3_oracle_module).num_links++;
    
	} else {
		int type;
    
		/* the link is already in the persistent list */
#ifdef THREAD_SAFE
		if (index_ptr->type != _php3_le_index_ptr()) {
#else
		if (index_ptr->type != le_index_ptr) {
#endif
			efree(hashed_details);
			php3_error(E_WARNING, "Oops, something went completly wrong");
			RETURN_FALSE;
		}
		id = (int) index_ptr->ptr;
		db_conn = (oraConnection *)php3_plist_find(id, &type);
    
		if (db_conn && (type ==  ORACLE_GLOBAL(php3_oracle_module).le_conn ||
					type == ORACLE_GLOBAL(php3_oracle_module).le_pconn)){
			if(!_ora_ping(db_conn)) {
				/* XXX Reinitialize lda, hda ? */
				if(olog(&db_conn->lda, db_conn->hda, user,
						 strlen(user), pwd, strlen(pwd), 0, -1, OCI_LM_DEF)) {
					php3_error(E_WARNING, "Unable to reconnect to ORACLE (%s)",
							   ora_error(&db_conn->lda));
					/* Delete list entry for this connection */
					php3_plist_delete(id);
					/* Delete hashed list entry for this dead connection */
					_php3_hash_del(plist, hashed_details, hashed_len); 
					efree(hashed_details);
					RETURN_FALSE;
				}
			}
			return_value->value.lval = id;
		}
	}
	efree(hashed_details);
	return_value->type = IS_LONG;
}

/* {{{ proto int ora_logoff(int connection)
   Close an Oracle connection */
void php3_Ora_Logoff(INTERNAL_FUNCTION_PARAMETERS)
{								/* conn_index */
	int type, ind;
	oraConnection *conn;
	pval *arg;
	ORACLE_TLS_VARS;

	if (getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg);
	ind = (int)arg->value.lval;

	conn = (oraConnection *)php3_list_find(ind, &type);
	if (!conn || (type != ORACLE_GLOBAL(php3_oracle_module).le_conn &&
				  type != ORACLE_GLOBAL(php3_oracle_module).le_pconn)) {
		return;
	}
	php3_list_delete(ind);
}
/* }}} */

/* {{{ proto int ora_open(int connection)
   Open an Oracle cursor */
void php3_Ora_Open(INTERNAL_FUNCTION_PARAMETERS)
{								/* conn_index */
	pval *arg;
	oraConnection *conn = NULL;
	oraCursor *cursor = NULL;
	int conn_ind;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg);
  
	conn_ind = arg->value.lval;
	conn = ora_get_conn(list, conn_ind);
	if (conn == NULL) {
		RETURN_FALSE;
	}

	if ((cursor = (oraCursor *)emalloc(sizeof(oraCursor))) == NULL){
		php3_error(E_WARNING, "Out of memory");
		RETURN_FALSE;
	}
	memset(cursor, 0, sizeof(oraCursor));
	if (oopen(&cursor->cda, &conn->lda, (text *) 0, -1, -1, (text *) 0, -1)) {
		php3_error(E_WARNING, "Unable to open new cursor (%s)",
				   ora_error(&cursor->cda));
		efree(cursor);
		RETURN_FALSE;
	}
	cursor->open = 1;
	cursor->conn_ptr = conn;	
	RETURN_LONG(ora_add_cursor(list, cursor));
}
/* }}} */

/* {{{ proto int ora_close(int cursor)
   Close an Oracle cursor */
void php3_Ora_Close(INTERNAL_FUNCTION_PARAMETERS)
{								/* conn_index */
	pval *arg;

	if (getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg);

	ora_del_cursor(list, arg->value.lval);
	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int ora_commitoff(int connection)
   Disable automatic commit */
void php3_Ora_CommitOff(INTERNAL_FUNCTION_PARAMETERS)
{								/* conn_index */
	pval *arg;
	oraConnection *conn;

	if (getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg);

	conn = ora_get_conn(list, arg->value.lval);
	if (conn == NULL) {
		RETURN_FALSE;
	}
	if (ocof(&conn->lda)) {
		php3_error(E_WARNING, "Unable to turn off auto-commit (%s)",
				   ora_error(&conn->lda));
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ora_commiton(int connection)
   Enable automatic commit */
void php3_Ora_CommitOn(INTERNAL_FUNCTION_PARAMETERS)
{								/* conn_index */
	pval *arg;
	oraConnection *conn;

	if (getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg);

	if (!(conn = ora_get_conn(list, arg->value.lval))) {
		RETURN_FALSE;
	}

	if (ocon(&conn->lda)) {
		php3_error(E_WARNING, "Unable to turn on auto-commit (%s)",
				   ora_error(&conn->lda));
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ora_commit(int connection)
   Commit an Oracle transaction */
void php3_Ora_Commit(INTERNAL_FUNCTION_PARAMETERS)
{								/* conn_index */
	pval *arg;
	oraConnection *conn;

	if (getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg);

	conn = ora_get_conn(list, arg->value.lval);
	if (conn == NULL) {
		RETURN_FALSE;
	}
	if (ocom(&conn->lda)) {
		php3_error(E_WARNING, "Unable to commit transaction (%s)",
				   ora_error(&conn->lda));
		RETURN_FALSE;
	}
	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int ora_rollback(int connection)
   Roll back an Oracle transaction */
void php3_Ora_Rollback(INTERNAL_FUNCTION_PARAMETERS)
{								/* conn_index */
	pval *arg;
	oraConnection *conn;

	if (getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg);

	conn = ora_get_conn(list, arg->value.lval);
	if (conn == NULL) {
		RETURN_FALSE;
	}
	if (orol(&conn->lda)) {
		php3_error(E_WARNING, "Unable to roll back transaction (%s)",
				   ora_error(&conn->lda));
		RETURN_FALSE;
	}
	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int ora_parse(int cursor, string sql_statement [, int defer])
   Parse an Oracle SQL statement */
void php3_Ora_Parse(INTERNAL_FUNCTION_PARAMETERS)
{	
     /* cursor_ind, sql_statement [, defer] */
	int argc;
	pval *argv[3];
	oraCursor *cursor;
	sword defer = 0;
	text *query;

	argc = ARG_COUNT(ht);
	if ((argc != 2 && argc != 3) || getParametersArray(ht, argc, argv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(argv[0]);
	convert_to_string(argv[1]);

	if (argc == 3) {
		convert_to_long(argv[2]);
		if (argv[2]->value.lval != 0) {
			defer = DEFER_PARSE;
		}
	}

 	query = (text *) estrndup(argv[1]->value.str.val,argv[1]->value.str.len);

	if (query == NULL) {
		php3_error(E_WARNING, "Invalid query");
		RETURN_FALSE;
	}
	if (!(cursor = ora_get_cursor(list, argv[0]->value.lval))){
		RETURN_FALSE;
	}

	if (cursor->query) {
		efree(cursor->query);
	}
	cursor->query = query;
	cursor->fetched = 0;
	if(cursor->params && cursor->nparams > 0){
		_php3_hash_destroy(cursor->params);
		efree(cursor->params);
		cursor->params = NULL;
		cursor->nparams = 0;
	}

	if (oparse(&cursor->cda, query, (sb4) - 1, defer, VERSION_7)) {
		php3_error(E_WARNING, "Ora_Parse failed (%s)",
				   ora_error(&cursor->cda));
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ora_bind(int cursor, string php_variable_name, string sql_parameter_name, int length [, int type])
   Bind a PHP variable to an Oracle parameter */
void php3_Ora_Bind(INTERNAL_FUNCTION_PARAMETERS)
{ /* cursor_ind, php_var_name, sql_var_name, data_len [, inout]*/
	/* inout: 0 = in/out, 1 = in, 2 = out */
	int argc;
	pval *argv[5];
	oraParam *newparam, *paramptr;
	oraCursor *cursor;
	char *paramname;

	argc = ARG_COUNT(ht);
	if (argc < 4 || argc > 5 || getParametersArray(ht, argc, argv) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	convert_to_long(argv[0]);
	convert_to_string(argv[1]);
	convert_to_string(argv[2]);
	convert_to_long(argv[3]);
		
	cursor = ora_get_cursor(list, argv[0]->value.lval);
	if (cursor == NULL) {
		php3_error(E_WARNING, "Invalid cursor index %d",
				   argv[0]->value.lval);
		RETURN_FALSE;
	}

	if(cursor->params == NULL){
		cursor->params = (HashTable *)emalloc(sizeof(HashTable));
		if (!cursor->params ||
			_php3_hash_init(cursor->params, 19, NULL,
							(void (*)(void *))pval_ora_param_destructor, 0) == FAILURE) {
			php3_error(E_ERROR, "Unable to initialize parameter list");
			RETURN_FALSE;
		}
	}
	if((newparam = (oraParam *)emalloc(sizeof(oraParam))) == NULL){
		php3_error(E_WARNING, "Out of memory for parameter");
		RETURN_FALSE;
	}

	if((paramname = estrndup(argv[1]->value.str.val, argv[1]->value.str.len)) == NULL){
		php3_error(E_WARNING, "Out of memory for parametername");
		efree(newparam);
		RETURN_FALSE;
	}

	if (_php3_hash_add(cursor->params, paramname, argv[1]->value.str.len + 1, newparam, sizeof(oraParam), (void **)&paramptr) == FAILURE) {
		/* XXX _php3_hash_destroy */
		efree(paramname);
		efree(newparam);
		php3_error(E_ERROR, "Could not make parameter placeholder");
		RETURN_FALSE;
	}

	efree(newparam);
	efree(paramname);

	paramptr->progvl = argv[3]->value.lval + 1;
	if(argc > 4){
		convert_to_long(argv[4]);
		paramptr->inout = (short)argv[4]->value.lval;
	}else{
		paramptr->inout = 0;
	}

	if((paramptr->progv = (text *)emalloc(paramptr->progvl)) == NULL){		
		php3_error(E_WARNING, "Out of memory for parameter value");
		RETURN_FALSE;
	}

/* XXX Maximum for progvl */
	paramptr->alen = paramptr->progvl;

	if (obndra(&cursor->cda,              
			   argv[2]->value.str.val,
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
		php3_error(E_WARNING, "Ora_Bind failed (%s)",
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
void php3_Ora_Exec(INTERNAL_FUNCTION_PARAMETERS)
{								/* cursor_index */
	pval *arg;
	oraCursor *cursor = NULL;

	if (getParameters(ht, 1, &arg) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long(arg);

	if ((cursor = ora_get_cursor(list, arg->value.lval)) == NULL) {
		RETURN_FALSE;
	}

	if (cursor->cda.ft == FT_SELECT) {
		if (ora_describe_define(cursor) < 0) {
			/* error message is given by ora_describe_define() */
			RETURN_FALSE;
		}
	}

	if(cursor->nparams > 0){
		if(!ora_set_param_values(cursor, 0)){
			RETURN_FALSE;
		}
	}

	if (oexec(&cursor->cda)) {
		php3_error(E_WARNING, "Ora_Exec failed (%s)",
				   ora_error(&cursor->cda));
		RETURN_FALSE;
	}
	
	if(cursor->nparams > 0){
		if(!ora_set_param_values(cursor, 1)){
			RETURN_FALSE;
		}
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ora_numcols(int cursor)
   Returns the numbers of columns in a result */
void php3_Ora_NumCols(INTERNAL_FUNCTION_PARAMETERS)
{								/* cursor_index */
	pval *arg;
	oraCursor *cursor = NULL;

	if (getParameters(ht, 1, &arg) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long(arg);

	if ((cursor = ora_get_cursor(list, arg->value.lval)) == NULL) {
		RETURN_FALSE;
	}

	RETURN_LONG(cursor->ncols); 
}
/* }}} */

/* {{{ proto int ora_numrows(int cursor)
   Returns the number of rows in a result */
void php3_Ora_NumRows(INTERNAL_FUNCTION_PARAMETERS)
{								/* cursor_index */
	pval *arg;
	oraCursor *cursor = NULL;

	if(getParameters(ht, 1, &arg) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long(arg);

	if((cursor = ora_get_cursor(list, arg->value.lval)) == NULL) {
		RETURN_FALSE;
	}

	RETURN_LONG(cursor->cda.rpc); 
}
/* }}} */

/* prepares/executes/fetches 1st row if avail*/
/* {{{ proto int ora_do(int connection, int cursor)
   Parse and execute a statement and fetch first result row */ 
void php3_Ora_Do(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *argv[2];
	oraConnection *conn = NULL;
	oraCursor *cursor = NULL;
	text *query;

	if (ARG_COUNT(ht) != 2 || getParametersArray(ht, 2, argv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(argv[0]);
	convert_to_string(argv[1]);

	conn = ora_get_conn(list, argv[0]->value.lval);
	if (conn == NULL) {
		RETURN_FALSE;
	}

	if ((cursor = (oraCursor *)emalloc(sizeof(oraCursor))) == NULL){
		php3_error(E_WARNING, "Out of memory");
		RETURN_FALSE;
	}

	memset(cursor, 0, sizeof(oraCursor));

        query = (text *) estrndup(argv[1]->value.str.val,argv[1]->value.str.len);

        if (query == NULL) {
                php3_error(E_WARNING, "Invalid query in Ora_Do");
                RETURN_FALSE;
        }

        cursor->query = query;

	if (oopen(&cursor->cda, &conn->lda, (text *) 0, -1, -1, (text *) 0, -1)) {
		php3_error(E_WARNING, "Unable to open new cursor (%s)",
				   ora_error(&cursor->cda));
		efree(cursor);
		RETURN_FALSE;
	}
	cursor->open = 1;
	cursor->conn_ptr = conn;	
	
	/* Prepare stmt */

	if (oparse(&cursor->cda, query, (sb4) - 1, 1, VERSION_7)){
		php3_error(E_WARNING, "Ora_Do failed (%s)",
				   ora_error(&cursor->cda));
		_close_oracur(cursor);
		RETURN_FALSE;
	}

	/* Execute stmt (and fetch 1st row for selects) */
	if (cursor->cda.ft == FT_SELECT) {
		if (ora_describe_define(cursor) < 0){
			/* error message is given by ora_describe_define() */
			_close_oracur(cursor);
			RETURN_FALSE;
		}
		if (oexfet(&cursor->cda, 1, 0, 0)) {
			php3_error(E_WARNING, "Ora_Do failed (%s)",
					   ora_error(&cursor->cda));
			_close_oracur(cursor);
			RETURN_FALSE;
		}
		cursor->fetched = 1;
	} else {
		if (oexec(&cursor->cda)) {
			php3_error(E_WARNING, "Ora_Do failed (%s)",
					   ora_error(&cursor->cda));
			_close_oracur(cursor);
			RETURN_FALSE;
		}
	}

	RETURN_LONG(ora_add_cursor(list, cursor));
}
/* }}} */

/* {{{ proto int ora_fetch(int cursor)
   Fetch a row of result data from a cursor */
void php3_Ora_Fetch(INTERNAL_FUNCTION_PARAMETERS)
{								/* cursor_index */
	pval *arg;
	oraCursor *cursor;

	if (getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg);

	/* Find the cursor */
	if ((cursor = ora_get_cursor(list, arg->value.lval)) == NULL) {
		RETURN_FALSE;
	}

	if (cursor->ncols == 0){
		php3_error(E_WARNING, "No tuples available on this cursor");
		RETURN_FALSE;
	}

	/* Get data from Oracle */
	if (ofetch(&cursor->cda)) {
		if (cursor->cda.rc != NO_DATA_FOUND) {
			php3_error(E_WARNING, "Ora_Fetch failed (%s)",
					   ora_error(&cursor->cda));
		}
		RETURN_FALSE;
	}
	cursor->fetched++;
	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int ora_fetch_into(int cursor, array result [ , int flags ])
   Fetch a row into the specified result array */
void php3_Ora_FetchInto(INTERNAL_FUNCTION_PARAMETERS)
{
	pval     *arg1, *arr, *flg, *tmp;
	oraCursor *cursor;
	ub4 ret_len;
	int i;
	int flags = 0;

	switch(ARG_COUNT(ht)){
		case 2:
			if (getParameters(ht, 2, &arg1, &arr) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;

		case 3:
			if (getParameters(ht, 3, &arg1, &arr, &flg) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_long(flg);
			flags = flg->value.lval;
			break;

		default:
			WRONG_PARAM_COUNT;
			break;
	}

	if (!ParameterPassedByReference(ht, 2)){
		php3_error(E_WARNING, "Array not passed by reference in call to ora_fetch_into()");
		RETURN_FALSE;
	}

	convert_to_long(arg1);

	/* Find the cursor */
	if ((cursor = ora_get_cursor(list, arg1->value.lval)) == NULL) {
		RETURN_FALSE;
	}

	if (cursor->ncols == 0){
		php3_error(E_WARNING, "No tuples available on this cursor");
		RETURN_FALSE;
	}

	if (arr->type != IS_ARRAY){
		if (array_init(arr) == FAILURE){
			php3_error(E_WARNING, "Can't convert to type Array");
			RETURN_FALSE;
		}
	}

	if (ofetch(&cursor->cda)) {
		if (cursor->cda.rc != NO_DATA_FOUND) {
			php3_error(E_WARNING, "Ora_Fetch_Into failed (%s)",
					   ora_error(&cursor->cda));
		}
		RETURN_FALSE;
	}
	cursor->fetched++;

#if PHP_API_VERSION < 19990421
	tmp = emalloc(sizeof(pval));
#endif

	for (i = 0; i < cursor->ncols; i++) {
       
		if (cursor->columns[i].col_retcode == 1405) {
			if (!(flags&ORA_FETCHINTO_NULLS)){
				continue; /* don't add anything for NULL columns, unless the calles wants it */
			} else {
				tmp->value.str.val = empty_string;
				tmp->value.str.len = 0;
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
#if PHP_API_VERSION >= 19990421
			tmp = emalloc(sizeof(pval));
			tmp->is_ref = 0;
			tmp->refcount = 1;
#endif

			tmp->type = IS_STRING;
			tmp->value.str.len = 0;

			switch(cursor->columns[i].dbtype) {
				case SQLT_LNG:
				case SQLT_LBI:
					/* XXX 64k max for LONG and LONG RAW */
					oflng(&cursor->cda, (sword)(i + 1), cursor->columns[i].buf, DB_SIZE, 1,
						  &ret_len, 0);

					tmp->value.str.len = ret_len;
					break;
				default:
					tmp->value.str.len = min(cursor->columns[i].col_retlen,
											 cursor->columns[i].dsize);
					break;
			}
			tmp->value.str.val = estrndup(cursor->columns[i].buf,
										  tmp->value.str.len);
		}

		if (flags&ORA_FETCHINTO_ASSOC){
#if PHP_API_VERSION >= 19990421
			_php3_hash_update(arr->value.ht, cursor->columns[i].cbuf, cursor->columns[i].cbufl+1, (void *) &tmp, sizeof(pval), NULL);
#else
			_php3_hash_update(arr->value.ht, cursor->columns[i].cbuf, cursor->columns[i].cbufl+1, (void *) tmp, sizeof(pval), NULL);
#endif
		} else {
#if PHP_API_VERSION >= 19990421
			_php3_hash_index_update(arr->value.ht, i, (void *) &tmp, sizeof(pval), NULL);
#else
			_php3_hash_index_update(arr->value.ht, i, (void *) tmp, sizeof(pval), NULL);
#endif
		}

	}

#if PHP_API_VERSION < 19990421
	efree(tmp); 
#endif

	RETURN_LONG(cursor->ncols); 
}
/* }}} */

/* {{{ proto string ora_columnname(int cursor, int column)
   Get the name of an Oracle result column */
void php3_Ora_ColumnName(INTERNAL_FUNCTION_PARAMETERS)
{								/* cursor_index, column_index */
	pval *argv[2];
	int cursor_ind;
	oraCursor *cursor = NULL;

	if (ARG_COUNT(ht) != 2 || getParametersArray(ht, 2, argv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(argv[0]);

	cursor_ind = argv[0]->value.lval;
	/* Find the cursor */
	if ((cursor = ora_get_cursor(list, cursor_ind)) == NULL) {
		RETURN_FALSE;
	}

	convert_to_long(argv[1]);
	
	if (cursor->ncols == 0){
		php3_error(E_WARNING, "No tuples available at this cursor index");
		RETURN_FALSE;
	}
        
	if (argv[1]->value.lval >= cursor->ncols){
		php3_error(E_WARNING, "Column index larger than number of columns");
		RETURN_FALSE;
	}

	if (argv[1]->value.lval < 0){
		php3_error(E_WARNING, "Column numbering starts at 0");
		RETURN_FALSE;
	}
        
	RETURN_STRINGL(cursor->columns[argv[1]->value.lval].cbuf,
				   cursor->columns[argv[1]->value.lval].cbufl,1);
}
/* }}} */

/* {{{ proto string ora_columntype(int cursor, int column) 
   Get the type of an Oracle result column */
void php3_Ora_ColumnType(INTERNAL_FUNCTION_PARAMETERS)
{								/* cursor_index, column_index */
	pval *argv[2];
	int cursor_ind, colno;
	oraCursor *cursor = NULL;

	if (ARG_COUNT(ht) != 2 || getParametersArray(ht, 2, argv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(argv[0]);
	/* don't convert the column index yet, it might be the column name */

	cursor_ind = argv[0]->value.lval;
	/* Find the cursor */
	if ((cursor = ora_get_cursor(list, cursor_ind)) == NULL) {
		RETURN_FALSE;
	}

	convert_to_long(argv[1]);
	colno = argv[1]->value.lval;

	if (cursor->ncols == 0){
		php3_error(E_WARNING, "No tuples available at this cursor index");
		RETURN_FALSE;
	}
        
	if (colno >= cursor->ncols){
		php3_error(E_WARNING, "Column index larger than number of columns");
		RETURN_FALSE;
	}

	if (colno < 0){
		php3_error(E_WARNING, "Column numbering starts at 0");
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
void php3_Ora_ColumnSize(INTERNAL_FUNCTION_PARAMETERS)
{								/* cursor_index, column_index */
	pval *argv[2];
	int cursor_ind;
	oraCursor *cursor = NULL;

	if (ARG_COUNT(ht) != 2 || getParametersArray(ht, 2, argv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(argv[0]);

	cursor_ind = argv[0]->value.lval;
	/* Find the cursor */
	if ((cursor = ora_get_cursor(list, cursor_ind)) == NULL) {
		RETURN_FALSE;
	}

	convert_to_long(argv[1]);
	
	if (cursor->ncols == 0){
		php3_error(E_WARNING, "No tuples available at this cursor index");
		RETURN_FALSE;
	}
        
	if (argv[1]->value.lval >= cursor->ncols){
		php3_error(E_WARNING, "Column index larger than number of columns");
		RETURN_FALSE;
	}

	if (argv[1]->value.lval < 0){
		php3_error(E_WARNING, "Column numbering starts at 0");
		RETURN_FALSE;
	}
        
	RETURN_LONG(cursor->columns[argv[1]->value.lval].dbsize);
}
/* }}} */

/* {{{ proto mixed ora_getcolumn(int cursor, int column)
   Get data from a fetched row */
void php3_Ora_GetColumn(INTERNAL_FUNCTION_PARAMETERS)
{								/* cursor_index, column_index */
	pval *argv[2];
	int colno;
	oraCursor *cursor = NULL;
	oraColumn *column = NULL;
	sb2 type;

	if (ARG_COUNT(ht) != 2 || getParametersArray(ht, 2, argv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(argv[0]);

	/* Find the cursor */
	if ((cursor = ora_get_cursor(list, argv[0]->value.lval)) == NULL) {
		RETURN_FALSE;
	}

	if (cursor->ncols == 0){
		php3_error(E_WARNING, "No tuples available at this cursor index");
		RETURN_FALSE;
	}

	convert_to_long(argv[1]);
	colno = argv[1]->value.lval;        

	if (colno >= cursor->ncols){
		php3_error(E_WARNING, "Column index larger than number of columns");
		RETURN_FALSE;
	}

	if (colno < 0){
		php3_error(E_WARNING, "Column numbering starts at 0");
		RETURN_FALSE;
	}

	if (cursor->fetched == 0){
		if (ofetch(&cursor->cda)) {
			if (cursor->cda.rc != NO_DATA_FOUND) {
				php3_error(E_WARNING, "Ora_Fetch failed (%s)",
						   ora_error(&cursor->cda));
			}
			RETURN_FALSE;
		}
		cursor->fetched++;		
	}

 	column = &cursor->columns[colno]; 

 	type = column->dbtype; 

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
		switch(type)
			{
			case SQLT_CHR:
			case SQLT_NUM:
			case SQLT_INT: 
			case SQLT_FLT:
			case SQLT_STR:
			case SQLT_UIN:
			case SQLT_AFC:
			case SQLT_AVC:
			case SQLT_DAT:
				RETURN_STRINGL(column->buf, min(column->col_retlen, column->dsize), 1);
			case SQLT_LNG:
			case SQLT_LBI:
				{ 
				ub4 ret_len;
				/* XXX 64k max for LONG and LONG RAW */
				oflng(&cursor->cda, (sword)(colno + 1), column->buf, DB_SIZE, 1,
					  &ret_len, 0);
				RETURN_STRINGL(column->buf, ret_len, 1);
				}
			default:
				php3_error(E_WARNING,
						   "Ora_GetColumn found invalid type (%d)", type);
				RETURN_FALSE;
			}
	}
}
/* }}} */

/* {{{ proto string ora_error(int cursor_or_connection)
   Get an Oracle error message */
void php3_Ora_Error(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg;
	oraCursor *cursor;
	oraConnection *conn;

	if (ARG_COUNT(ht) != 1 || getParametersArray(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg);
	if ((cursor = ora_get_cursor(list, arg->value.lval)) != NULL) {
		return_value->type = IS_STRING;
		return_value->value.str.val = estrdup(ora_error(&cursor->cda));
		return_value->value.str.len = strlen(return_value->value.str.val);
	} else if ((conn = ora_get_conn(list, arg->value.lval)) != NULL) {
		return_value->type = IS_STRING;
		return_value->value.str.val = estrdup(ora_error(&conn->lda));
		return_value->value.str.len = strlen(return_value->value.str.val);
	}
}
/* }}} */

/* {{{ proto int ora_errorcode(int cursor_or_connection)
   Get an Oracle error code */
void php3_Ora_ErrorCode(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg;
	oraCursor *cursor;
	oraConnection *conn;

	if (ARG_COUNT(ht) != 1 || getParametersArray(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg);
	if ((cursor = ora_get_cursor(list, arg->value.lval)) != NULL) {
		RETVAL_LONG(cursor->cda.rc);
	} else if ((conn = ora_get_conn(list, arg->value.lval)) != NULL) {
		RETURN_LONG(conn->lda.rc);
	}
}
/* }}} */

void php3_info_oracle(ZEND_MODULE_INFO_FUNC_ARGS)
{
#if !(WIN32|WINNT)
	php3_printf("Oracle version: %s<br>\n"
			    "Compile-time ORACLE_HOME: %s<br>\n"
			    "Libraries used: %s",
			    PHP_ORACLE_VERSION, PHP_ORACLE_HOME, PHP_ORACLE_LIBS);
#endif
}


/*
** Functions internal to this module.
*/

static oraConnection *
ora_get_conn(HashTable *list, int ind)
{
	oraConnection *conn = NULL;
	int type;
	HashTable *plist;
	ORACLE_TLS_VARS;

	plist = ORACLE_GLOBAL(php3_oracle_module).resource_plist;

	conn = (oraConnection *)php3_list_find(ind, &type);
	if (conn && type == ORACLE_GLOBAL(php3_oracle_module).le_conn)
		return conn;

	conn = (oraConnection *)php3_plist_find(ind, &type);
	if (conn && type == ORACLE_GLOBAL(php3_oracle_module).le_pconn)
		return conn;

	php3_error(E_WARNING,"Bad Oracle connection number (%d)", ind);
	return NULL;
}

int ora_add_cursor(HashTable *list, oraCursor *cursor)
{
	ORACLE_TLS_VARS;
	return php3_list_insert(cursor, ORACLE_GLOBAL(php3_oracle_module).le_cursor);
}

static oraCursor *
ora_get_cursor(HashTable *list, int ind)
{
	oraCursor *cursor;
	int type;
	ORACLE_TLS_VARS;

	cursor = php3_list_find(ind, &type);
	if (!cursor || type != ORACLE_GLOBAL(php3_oracle_module).le_cursor) {
		php3_error(E_WARNING, "Invalid cursor index %d", ind);
		return NULL;
	}
	return cursor;
}

void ora_del_cursor(HashTable *list, int ind)
{
	oraCursor *cursor;
	int type;
	ORACLE_TLS_VARS;
  
	cursor = (oraCursor *) php3_list_find(ind, &type);
	if (!cursor || type != ORACLE_GLOBAL(php3_oracle_module).le_cursor) {
		php3_error(E_WARNING,"Can't find cursor %d",ind);
		return;
	}
	php3_list_delete(ind);
}

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
				php3_error(E_WARNING, "%s", ora_error(&cursor->cda));
				cursor->ncols = 0;
				return -1;
			}
		}
		cursor->ncols++;
	}

	if (cursor->ncols > 0){
		cursor->columns = (oraColumn *) emalloc(sizeof(oraColumn) * cursor->ncols);
		if (cursor->columns == NULL){
			php3_error(E_WARNING, "Out of memory");
			return -1;
		}
	}

	for(col = 0; col < cursor->ncols; col++){
		memset(&cursor->columns[col], 0, sizeof(oraColumn));
		cursor->columns[col].cbufl = ORANAMELEN;
		
		if (odescr(&cursor->cda, (sword)col + 1, &cursor->columns[col].dbsize,
				   &cursor->columns[col].dbtype, &cursor->columns[col].cbuf[0],
				   &cursor->columns[col].cbufl, &cursor->columns[col].dsize,
				   &cursor->columns[col].prec, &cursor->columns[col].scale,
				   &cursor->columns[col].nullok)) {
			if (cursor->cda.rc == VAR_NOT_IN_LIST) {
				break;
			} else {
				php3_error(E_WARNING, "%s", ora_error(&cursor->cda));
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
		
		if ((cursor->columns[col].buf = (ub1 *) emalloc(cursor->columns[col].dsize + 1)) == NULL){
			php3_error(E_WARNING, "Out of memory");
			return -1;
		}
		/* Define an output variable for the column */
		if (odefin(&cursor->cda, (sword)col + 1, cursor->columns[col].buf, 
				   cursor->columns[col].dsize + 1, type, -1, &cursor->columns[col].indp,
				   (text *) 0, -1, -1, &cursor->columns[col].col_retlen, 
				   &cursor->columns[col].col_retcode)) {
			php3_error(E_WARNING, "%s", ora_error(&cursor->cda));
			return -1;
		}
	}
	return 1;
}

int ora_set_param_values(oraCursor *cursor, int isout)
{
	char *paramname;
	oraParam *param;
	pval *pdata;
	int i, len;
#if (WIN32|WINNT)
	/* see variables.c */
	HashTable *symbol_table=php3i_get_symbol_table();
#endif
	_php3_hash_internal_pointer_reset(cursor->params);

	if(_php3_hash_num_elements(cursor->params) != cursor->nparams){
		php3_error(E_WARNING, "Mismatch in number of parameters");
		return 0;
	}

	for(i = 0; i < cursor->nparams; i++, _php3_hash_move_forward(cursor->params)){
		if(_php3_hash_get_current_key(cursor->params, &paramname, NULL) != HASH_KEY_IS_STRING){
			php3_error(E_WARNING, "Can't get parameter name");
			return 0;
		}

		if(_php3_hash_get_current_data(cursor->params, (void **)&param) == FAILURE){
			php3_error(E_WARNING, "Can't get parameter data");
			efree(paramname);
			return 0;
		}

		if(isout){
#if (WIN32|WINNT)
			/* see oracle_hack.c */
			{ 
				pval var; 
				char *name=(paramname); 
				var.value.str.val = estrdup(param->progv);
				var.value.str.len = strlen(param->progv);
				var.type = IS_STRING; 
				_php3_hash_update(symbol_table, name, strlen(name)+1, &var, sizeof(pval),NULL); 
			} 
#else
			SET_VAR_STRINGL(paramname, estrdup(param->progv), strlen(param->progv));
#endif
			efree(paramname);
			continue;
		}
		
		/* doing the in-loop */

		/* FIXME Globals don't work in extensions on windows, have to do something
			else here.  See oracle_hack.c */
#if (WIN32|WINNT)
		if(_php3_hash_find(symbol_table, paramname, strlen(paramname) + 1, (void **)&pdata) == FAILURE){
			php3_error(E_WARNING, "Can't find variable for parameter");
			efree(paramname);
			return 0;
		}
#else

#if PHP_API_VERSION < 19990421 
		if(_php3_hash_find(&GLOBAL(symbol_table), paramname, strlen(paramname) + 1, (void **)&pdata) == FAILURE){
#else
		if(_php3_hash_find(&EG(symbol_table), paramname, strlen(paramname) + 1, (void **)&pdata) == FAILURE){
#endif
			php3_error(E_WARNING, "Can't find variable for parameter");
			efree(paramname);
			return 0;
		}
#endif
		convert_to_string(pdata);
		if(param->progvl <= pdata->value.str.len){
			php3_error(E_NOTICE, "Input value will be truncated");
		}

		len = min(param->progvl - 1, pdata->value.str.len);

		strncpy(param->progv, pdata->value.str.val, len);

		param->progv[len] = '\0';
		efree(paramname);
	}
	return 1;
	
}

#endif							/* HAVE_ORACLE */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
