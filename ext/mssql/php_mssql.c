/*
  +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Frank M. Kromann <fmk@businesnet.dk>                        |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef COMPILE_DL_MSSQL
#define HAVE_MSSQL 1
#endif

#include "php.h"
#include "php_globals.h"
#include "ext/standard/php_standard.h"
#include "ext/standard/info.h"
#include "php_mssql.h"
#include "php_ini.h"

#if HAVE_MSSQL
#define SAFE_STRING(s) ((s)?(s):"")

static int le_result, le_link, le_plink;

static void php_mssql_get_column_content_with_type(mssql_link *mssql_ptr,int offset,zval *result, int column_type);
static void php_mssql_get_column_content_without_type(mssql_link *mssql_ptr,int offset,zval *result, int column_type);

function_entry mssql_functions[] = {
	PHP_FE(mssql_connect,				NULL)
	PHP_FE(mssql_pconnect,				NULL)
	PHP_FE(mssql_close,					NULL)
	PHP_FE(mssql_select_db,				NULL)
	PHP_FE(mssql_query,					NULL)
	PHP_FE(mssql_free_result,			NULL)
	PHP_FE(mssql_get_last_message,		NULL)
	PHP_FE(mssql_num_rows,				NULL)
	PHP_FE(mssql_num_fields,			NULL)
	PHP_FE(mssql_fetch_field,			NULL)
	PHP_FE(mssql_fetch_row,				NULL)
	PHP_FE(mssql_fetch_array,			NULL)
	PHP_FE(mssql_fetch_object,			NULL)
	PHP_FE(mssql_field_length,			NULL)
	PHP_FE(mssql_field_name,			NULL)
	PHP_FE(mssql_field_type,			NULL)
	PHP_FE(mssql_data_seek,				NULL)
	PHP_FE(mssql_field_seek,			NULL)
	PHP_FE(mssql_result,				NULL)
	PHP_FE(mssql_min_error_severity,	NULL)
	PHP_FE(mssql_min_message_severity,	NULL)
	{NULL, NULL, NULL}
};

zend_module_entry mssql_module_entry = {
	"mssql", 
	mssql_functions, 
	PHP_MINIT(mssql), 
	PHP_MSHUTDOWN(mssql), 
	PHP_RINIT(mssql), 
	PHP_RSHUTDOWN(mssql), 
	PHP_MINFO(mssql), 
	STANDARD_MODULE_PROPERTIES
};

#ifdef ZTS
int mssql_globals_id;
#else
PHP_MSSQL_API php_mssql_globals mssql_globals;
#endif

#ifdef COMPILE_DL_MSSQL
ZEND_GET_MODULE(mssql)
#endif

#define CHECK_LINK(link) { if (link==-1) { php_error(E_WARNING,"MS SQL:  A link to the server could not be established"); RETURN_FALSE; } }


PHP_INI_BEGIN()
	STD_PHP_INI_BOOLEAN("mssql.allow_persistent",		"1",	PHP_INI_SYSTEM,	OnUpdateBool,	allow_persistent,			php_mssql_globals,		mssql_globals)
	STD_PHP_INI_ENTRY_EX("mssql.max_persistent",		"-1",	PHP_INI_SYSTEM,	OnUpdateInt,	max_persistent,				php_mssql_globals,		mssql_globals,	display_link_numbers)
	STD_PHP_INI_ENTRY_EX("mssql.max_links",				"-1",	PHP_INI_SYSTEM,	OnUpdateInt,	max_links,					php_mssql_globals,		mssql_globals,	display_link_numbers)
	STD_PHP_INI_ENTRY_EX("mssql.min_error_severity",	"10",	PHP_INI_ALL,	OnUpdateInt,	cfg_min_error_severity,		php_mssql_globals,		mssql_globals,	display_link_numbers)
	STD_PHP_INI_ENTRY_EX("mssql.min_message_severity",	"10",	PHP_INI_ALL,	OnUpdateInt,	cfg_min_message_severity,	php_mssql_globals,		mssql_globals,	display_link_numbers)
	STD_PHP_INI_BOOLEAN("mssql.compatability_mode",		"0",	PHP_INI_ALL,	OnUpdateBool,	compatability_mode,			php_mssql_globals,		mssql_globals)
	STD_PHP_INI_ENTRY_EX("mssql.connect_timeout",    	"5",	PHP_INI_ALL,	OnUpdateInt,	connect_timeout,			php_mssql_globals,		mssql_globals,	display_link_numbers)
	STD_PHP_INI_ENTRY_EX("mssql.textsize",   			"-1",	PHP_INI_ALL,	OnUpdateInt,	textsize,					php_mssql_globals,		mssql_globals,	display_link_numbers)
	STD_PHP_INI_ENTRY_EX("mssql.textlimit",   			"-1",	PHP_INI_ALL,	OnUpdateInt,	textlimit,					php_mssql_globals,		mssql_globals,	display_link_numbers)
PHP_INI_END()

/* error handler */
static int php_mssql_error_handler(DBPROCESS *dbproc, int severity, int dberr, int oserr, char *dberrstr, char *oserrstr)
{
	MSSQLLS_FETCH();
	if (severity >= MS_SQL_G(min_error_severity)) {
		php_error(E_WARNING,"MS SQL error:  %s (severity %d)", dberrstr, severity);
	}
	return INT_CANCEL;  
}

/* message handler */
static int php_mssql_message_handler(DBPROCESS *dbproc, DBINT msgno,int msgstate, int severity,char *msgtext,char *srvname, char *procname,DBUSMALLINT line)
{
	MSSQLLS_FETCH();
	if (severity >= MS_SQL_G(min_message_severity)) {
		php_error(E_WARNING,"MS SQL message:  %s (severity %d)", msgtext, severity);
	}
	STR_FREE(MS_SQL_G(server_message));
	MS_SQL_G(server_message) = estrdup(msgtext);
	return 0;
}

static int _clean_invalid_results(list_entry *le)
{
	MSSQLLS_FETCH();
	if (le->type == le_result) {
		mssql_link *mssql_ptr = ((mssql_result *) le->ptr)->mssql_ptr;
		
		if (!mssql_ptr->valid) {
			return 1;
		}
	}
	return 0;
}

static void _free_mssql_result(mssql_result *result)
{
	int i,j;

	if (result->data) {
		for (i=0; i<result->num_rows; i++) {
			for (j=0; j<result->num_fields; j++) {
				zval_dtor(&result->data[i][j]);
			}
			efree(result->data[i]);
		}
		efree(result->data);
	}
	
	if (result->fields) {
		for (i=0; i<result->num_fields; i++) {
			STR_FREE(result->fields[i].name);
			STR_FREE(result->fields[i].column_source);
		}
		efree(result->fields);
	}
	efree(result);
}

static void php_mssql_set_default_link(int id)
{
	MSSQLLS_FETCH();

	if (MS_SQL_G(default_link)!=-1) {
		zend_list_delete(MS_SQL_G(default_link));
	}
	MS_SQL_G(default_link) = id;
	zend_list_addref(id);
}

static void _close_mssql_link(mssql_link *mssql_ptr)
{
	MSSQLLS_FETCH();
	ELS_FETCH();

	mssql_ptr->valid = 0;
	zend_hash_apply(&EG(regular_list),(int (*)(void *))_clean_invalid_results);
	dbclose(mssql_ptr->link);
	dbfreelogin(mssql_ptr->login);
	efree(mssql_ptr);
	MS_SQL_G(num_links)--;
}


static void _close_mssql_plink(mssql_link *mssql_ptr)
{
	MSSQLLS_FETCH();

	dbclose(mssql_ptr->link);
	dbfreelogin(mssql_ptr->login);
	efree(mssql_ptr);
	MS_SQL_G(num_persistent)--;
	MS_SQL_G(num_links)--;
}

#ifdef ZTS
static void php_mssql_init_globals(php_mssql_globals *mssql_globals)
{
	MS_SQL_G(num_persistent) = 0;
	if (MS_SQL_G(compatability_mode)) {
		MS_SQL_G(get_column_content) = php_mssql_get_column_content_with_type;
	} else {
		MS_SQL_G(get_column_content) = php_mssql_get_column_content_without_type;	
	}
}
#endif

PHP_MINIT_FUNCTION(mssql)
{
	MSSQLLS_D;

#ifdef ZTS
	mssql_globals_id = ts_allocate_id(sizeof(php_mssql_globals), php_mssql_init_globals, NULL);
	mssql_globals = ts_resource(mssql_globals_id);
#else
	MS_SQL_G(num_persistent) = 0;
#endif

	REGISTER_INI_ENTRIES();
	le_result = register_list_destructors(_free_mssql_result, NULL);
	le_link = register_list_destructors(_close_mssql_link, NULL);
	le_plink = register_list_destructors(NULL, _close_mssql_plink);
	mssql_module_entry.type = type;

	if (dbinit()==FAIL) {
		return FAILURE;
	}
	dberrhandle((DBERRHANDLE_PROC) php_mssql_error_handler);
	dbmsghandle((DBMSGHANDLE_PROC) php_mssql_message_handler);
	if (MS_SQL_G(connect_timeout) < 1) MS_SQL_G(connect_timeout) = 1;
	dbsetlogintime(MS_SQL_G(connect_timeout));

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(mssql)
{
	UNREGISTER_INI_ENTRIES();
	dbexit();
	return SUCCESS;
}


PHP_RINIT_FUNCTION(mssql)
{
	MSSQLLS_FETCH();
	
	MS_SQL_G(default_link) = -1;
	MS_SQL_G(num_links) = MS_SQL_G(num_persistent);
	MS_SQL_G(appname) = estrndup("PHP 4.0",7);
	MS_SQL_G(server_message) = empty_string;
	MS_SQL_G(min_error_severity) = MS_SQL_G(cfg_min_error_severity);
	MS_SQL_G(min_message_severity) = MS_SQL_G(cfg_min_message_severity);

	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(mssql)
{
	MSSQLLS_FETCH();

	STR_FREE(MS_SQL_G(appname));
	if (MS_SQL_G(server_message)) {
		STR_FREE(MS_SQL_G(server_message));
	}
	return SUCCESS;
}

PHP_MINFO_FUNCTION(mssql)
{
	char buf[32];
	MSSQLLS_FETCH();

	php_info_print_table_start();
	php_info_print_table_header(2, "MSSQL Support", "enabled");

	sprintf(buf, "%ld", MS_SQL_G(num_persistent));
	php_info_print_table_row(2, "Active Persistent Links", buf);
	sprintf(buf, "%ld", MS_SQL_G(num_links));
	php_info_print_table_row(2, "Active Links", buf);

	php_info_print_table_row(2, "Library version", MSSQL_VERSION);
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();

}

void php_mssql_do_connect(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	char *user, *passwd, *host;
	char *hashed_details;
	int hashed_details_length;
	mssql_link mssql, *mssql_ptr;
	char buffer[32];
	MSSQLLS_FETCH();
/*	PLS_FETCH(); */

	switch(ZEND_NUM_ARGS()) {
		case 0: /* defaults */
			host=user=passwd=NULL;
			hashed_details_length=5+3;
			hashed_details = (char *) emalloc(hashed_details_length+1);
			strcpy(hashed_details,"mssql___");
			break;
		case 1: {
				zval **yyhost;
				
				if (zend_get_parameters_ex(1, &yyhost)==FAILURE) {
					WRONG_PARAM_COUNT;
				}
				convert_to_string_ex(yyhost);
				host = (*yyhost)->value.str.val;
				user=passwd=NULL;
				hashed_details_length = (*yyhost)->value.str.len+5+3;
				hashed_details = (char *) emalloc(hashed_details_length+1);
				sprintf(hashed_details,"mssql_%s__",(*yyhost)->value.str.val);
			}
			break;
		case 2: {
				zval **yyhost,**yyuser;
				
				if (zend_get_parameters_ex(2, &yyhost, &yyuser)==FAILURE) {
					WRONG_PARAM_COUNT;
				}
				convert_to_string_ex(yyhost);
				convert_to_string_ex(yyuser);
				host = (*yyhost)->value.str.val;
				user = (*yyuser)->value.str.val;
				passwd=NULL;
				hashed_details_length = (*yyhost)->value.str.len+(*yyuser)->value.str.len+5+3;
				hashed_details = (char *) emalloc(hashed_details_length+1);
				sprintf(hashed_details,"mssql_%s_%s_",(*yyhost)->value.str.val,(*yyuser)->value.str.val);
			}
			break;
		case 3: {
				zval **yyhost,**yyuser,**yypasswd;
			
				if (zend_get_parameters_ex(3, &yyhost, &yyuser, &yypasswd) == FAILURE) {
					WRONG_PARAM_COUNT;
				}
				convert_to_string_ex(yyhost);
				convert_to_string_ex(yyuser);
				convert_to_string_ex(yypasswd);
				host = (*yyhost)->value.str.val;
				user = (*yyuser)->value.str.val;
				passwd = (*yypasswd)->value.str.val;
				hashed_details_length = (*yyhost)->value.str.len+(*yyuser)->value.str.len+(*yypasswd)->value.str.len+5+3;
				hashed_details = (char *) emalloc(hashed_details_length+1);
				sprintf(hashed_details,"mssql_%s_%s_%s",(*yyhost)->value.str.val,(*yyuser)->value.str.val,(*yypasswd)->value.str.val); /* SAFE */
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	if (hashed_details == NULL) {
		php_error(E_WARNING, "Out of memory");
		RETURN_FALSE;
	}

	/* set a DBLOGIN record */	
	if ((mssql.login = dblogin()) == NULL) {
		php_error(E_WARNING,"MS SQL:  Unable to allocate login record");
		RETURN_FALSE;
	}
	
	if (user) {
		DBSETLUSER(mssql.login,user);
	}
	if (passwd) {
		DBSETLPWD(mssql.login,passwd);
	}
	DBSETLAPP(mssql.login,MS_SQL_G(appname));
	mssql.valid = 1;

	DBSETLVERSION(mssql.login, DBVER60);
/*	DBSETLTIME(mssql.login, TIMEOUT_INFINITE); */

	if (!MS_SQL_G(allow_persistent)) {
		persistent=0;
	}
	if (persistent) {
		list_entry *le;

		/* try to find if we already have this link in our persistent list */
		if (zend_hash_find(&EG(persistent_list), hashed_details, hashed_details_length + 1, (void **) &le)==FAILURE) {  /* we don't */
			list_entry new_le;

			if (MS_SQL_G(max_links) != -1 && MS_SQL_G(num_links) >= MS_SQL_G(max_links)) {
				php_error(E_WARNING,"MS SQL:  Too many open links (%d)",MS_SQL_G(num_links));
				efree(hashed_details);
				dbfreelogin(mssql.login);
				RETURN_FALSE;
			}
			if (MS_SQL_G(max_persistent) != -1 && MS_SQL_G(num_persistent) >= MS_SQL_G(max_persistent)) {
				php_error(E_WARNING,"MS SQL:  Too many open persistent links (%d)",MS_SQL_G(num_persistent));
				efree(hashed_details);
				dbfreelogin(mssql.login);
				RETURN_FALSE;
			}
			/* create the link */
			if ((mssql.link = dbopen(mssql.login, host)) == FAIL) {
				php_error(E_WARNING,"MS SQL:  Unable to connect to server:  %s", host);
				efree(hashed_details);
				dbfreelogin(mssql.login);
				RETURN_FALSE;
			}

			if (dbsetopt(mssql.link, DBBUFFER, "2")==FAIL) {
				efree(hashed_details);
				dbfreelogin(mssql.login);
				dbclose(mssql.link);
				RETURN_FALSE;
			}

			if (MS_SQL_G(textlimit) != -1) {
				sprintf(buffer, "%li", MS_SQL_G(textlimit));
				if (dbsetopt(mssql.link, DBTEXTLIMIT, buffer)==FAIL) {
					efree(hashed_details);
					dbfreelogin(mssql.login);
					RETURN_FALSE;
				}
			}
			if (MS_SQL_G(textsize) != -1) {
				sprintf(buffer, "SET TEXTSIZE %li", MS_SQL_G(textsize));
				dbcmd(mssql.link, buffer);
				dbsqlexec(mssql.link);
				dbresults(mssql.link);
			}

			/* hash it up */
			mssql_ptr = (mssql_link *) emalloc(sizeof(mssql_link));
			memcpy(mssql_ptr, &mssql, sizeof(mssql_link));
			new_le.type = le_plink;
			new_le.ptr = mssql_ptr;
			if (zend_hash_update(&EG(persistent_list), hashed_details, hashed_details_length + 1, &new_le, sizeof(list_entry), NULL)==FAILURE) {
				efree(mssql_ptr);
				efree(hashed_details);
				dbfreelogin(mssql.login);
				RETURN_FALSE;
			}
			MS_SQL_G(num_persistent)++;
			MS_SQL_G(num_links)++;
		} else {  /* we do */
			if (le->type != le_plink) {
#if BROKEN_MSSQL_PCONNECTS
				log_error("PHP/MS SQL:  Hashed persistent link is not a MS SQL link!",php_rqst->server);
#endif
				php_error(E_WARNING,"MS SQL:  Hashed persistent link is not a MS SQL link!");
				RETURN_FALSE;
			}
			
			mssql_ptr = (mssql_link *) le->ptr;
			/* test that the link hasn't died */
			if (DBDEAD(mssql_ptr->link) == TRUE) {
#if BROKEN_MSSQL_PCONNECTS
				log_error("PHP/MS SQL:  Persistent link died, trying to reconnect...",php_rqst->server);
#endif
				if ((mssql_ptr->link=dbopen(mssql_ptr->login,host))==FAIL) {
#if BROKEN_MSSQL_PCONNECTS
					log_error("PHP/MS SQL:  Unable to reconnect!",php_rqst->server);
#endif
					php_error(E_WARNING,"MS SQL:  Link to server lost, unable to reconnect");
					zend_hash_del(&EG(persistent_list), hashed_details, hashed_details_length+1);
					efree(hashed_details);
					RETURN_FALSE;
				}
#if BROKEN_MSSQL_PCONNECTS
				log_error("PHP/MS SQL:  Reconnect successful!",php_rqst->server);
#endif
				if (dbsetopt(mssql_ptr->link, DBBUFFER, "2")==FAIL) {
#if BROKEN_MSSQL_PCONNECTS
					log_error("PHP/MS SQL:  Unable to set required options",php_rqst->server);
#endif
					zend_hash_del(&EG(persistent_list), hashed_details, hashed_details_length + 1);
					efree(hashed_details);
					RETURN_FALSE;
				}
			}
		}
		ZEND_REGISTER_RESOURCE(return_value, mssql_ptr, le_plink);
	} else { /* non persistent */
		list_entry *index_ptr, new_index_ptr;
		
		/* first we check the hash for the hashed_details key.  if it exists,
		 * it should point us to the right offset where the actual mssql link sits.
		 * if it doesn't, open a new mssql link, add it to the resource list,
		 * and add a pointer to it with hashed_details as the key.
		 */
		if (zend_hash_find(&EG(regular_list), hashed_details, hashed_details_length + 1,(void **) &index_ptr)==SUCCESS) {
			int type,link;
			void *ptr;

			if (index_ptr->type != le_index_ptr) {
				RETURN_FALSE;
			}
			link = (int) index_ptr->ptr;
			ptr = zend_list_find(link,&type);   /* check if the link is still there */
			if (ptr && (type==le_link || type==le_plink)) {
				zend_list_addref(link);
				return_value->value.lval = link;
				php_mssql_set_default_link(link);
				return_value->type = IS_RESOURCE;
				efree(hashed_details);
				return;
			} else {
				zend_hash_del(&EG(regular_list), hashed_details, hashed_details_length + 1);
			}
		}
		if (MS_SQL_G(max_links) != -1 && MS_SQL_G(num_links) >= MS_SQL_G(max_links)) {
			php_error(E_WARNING,"MS SQL:  Too many open links (%d)",MS_SQL_G(num_links));
			efree(hashed_details);
			RETURN_FALSE;
		}
		
		if ((mssql.link=dbopen(mssql.login, host))==NULL) {
			php_error(E_WARNING,"MS SQL:  Unable to connect to server:  %s", host);
			efree(hashed_details);
			RETURN_FALSE;
		}

		if (dbsetopt(mssql.link, DBBUFFER,"2")==FAIL) {
			efree(hashed_details);
			dbfreelogin(mssql.login);
			dbclose(mssql.link);
			RETURN_FALSE;
		}

		if (MS_SQL_G(textlimit) != -1) {
			sprintf(buffer, "%li", MS_SQL_G(textlimit));
			if (dbsetopt(mssql.link, DBTEXTLIMIT, buffer)==FAIL) {
				efree(hashed_details);
				dbfreelogin(mssql.login);
				RETURN_FALSE;
			}
		}
		if (MS_SQL_G(textsize) != -1) {
			sprintf(buffer, "SET TEXTSIZE %li", MS_SQL_G(textsize));
			dbcmd(mssql.link, buffer);
			dbsqlexec(mssql.link);
			dbresults(mssql.link);
		}

		/* add it to the list */
		mssql_ptr = (mssql_link *) emalloc(sizeof(mssql_link));
		memcpy(mssql_ptr, &mssql, sizeof(mssql_link));
		ZEND_REGISTER_RESOURCE(return_value, mssql_ptr, le_link);
		
		/* add it to the hash */
		new_index_ptr.ptr = (void *) return_value->value.lval;
		new_index_ptr.type = le_index_ptr;
		if (zend_hash_update(&EG(regular_list), hashed_details, hashed_details_length + 1,(void *) &new_index_ptr, sizeof(list_entry),NULL)==FAILURE) {
			efree(hashed_details);
			RETURN_FALSE;
		}
		MS_SQL_G(num_links)++;
	}
	efree(hashed_details);
	php_mssql_set_default_link(return_value->value.lval);
}


static int php_mssql_get_default_link(INTERNAL_FUNCTION_PARAMETERS MSSQLLS_DC)
{
	if (MS_SQL_G(default_link)==-1) { /* no link opened yet, implicitly open one */
		ht = 0;
		php_mssql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);
	}
	return MS_SQL_G(default_link);
}

/* {{{ proto int mssql_connect([string servername [, string username [, string password]]])
   Establishes a connection to a MS-SQL server */
PHP_FUNCTION(mssql_connect)
{
	php_mssql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);
}

/* }}} */

/* {{{ proto int mssql_pconnect([string servername [, string username [, string password]]])
   Establishes a persistent connection to a MS-SQL server */
PHP_FUNCTION(mssql_pconnect)
{
	php_mssql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,1);
}

/* }}} */

/* {{{ proto int mssql_close([int connectionid])
   Closes a connection to a MS-SQL server */
PHP_FUNCTION(mssql_close)
{
	zval **mssql_link_index=NULL;
	int id;
	mssql_link *mssql_ptr;
	MSSQLLS_FETCH();

	
	switch (ZEND_NUM_ARGS()) {
		case 0:
			id = php_mssql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU MSSQLLS_CC);
			CHECK_LINK(id);
			break;
		case 1:
			if (zend_get_parameters_ex(1, &mssql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ZEND_FETCH_RESOURCE2(mssql_ptr, mssql_link *, mssql_link_index, id, "MS SQL-Link", le_link, le_plink);
	zend_list_delete((*mssql_link_index)->value.lval);

	RETURN_TRUE;
}

/* }}} */

/* {{{ proto bool mssql_select_db(string database_name [, int conn_id])
   Select a MS-SQL database */
PHP_FUNCTION(mssql_select_db)
{
	zval **db, **mssql_link_index;
	int id;
	mssql_link  *mssql_ptr;
	MSSQLLS_FETCH();

	
	switch(ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &db)==FAILURE) {
				RETURN_FALSE;
			}
			id = php_mssql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU MSSQLLS_CC);
			CHECK_LINK(id);
			break;
		case 2:
			if (zend_get_parameters_ex(2, &db, &mssql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	ZEND_FETCH_RESOURCE2(mssql_ptr, mssql_link *, mssql_link_index, id, "MS SQL-Link", le_link, le_plink);
	
	convert_to_string_ex(db);
	
	if (dbuse(mssql_ptr->link, (*db)->value.str.val)==FAIL) {
		php_error(E_WARNING,"MS SQL:  Unable to select database:  %s", (*db)->value.str.val);
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}

/* }}} */

static void php_mssql_get_column_content_with_type(mssql_link *mssql_ptr,int offset,zval *result, int column_type)
{
	if (dbdatlen(mssql_ptr->link,offset) == 0) {
		var_reset(result);
		return;
	}

	switch (column_type)
	{
		case SQLINT1:
		case SQLINT2:
		case SQLINT4:
		case SQLINTN: {	
			result->value.lval = (long) anyintcol(offset);
			result->type = IS_LONG;
			break;
		}
		case SQLCHAR:
		case SQLVARCHAR:
		case SQLTEXT: {
			int length;
			char *data = charcol(offset);

			length=dbdatlen(mssql_ptr->link,offset);
			while (length>0 && charcol(offset)[length-1] == ' ') { /* nuke trailing whitespace */
				length--;
			}
			result->value.str.val = estrndup(data,length);
			result->value.str.len = length;
			result->type = IS_STRING;
			break;
		}
		case SQLFLT8: {
			result->value.dval = (double) floatcol(offset);
			result->type = IS_DOUBLE;
			break;
		}
		case SQLNUMERIC:
		default: {
			if (dbwillconvert(coltype(offset),SQLCHAR)) {
				char *res_buf;
				int res_length = dbdatlen(mssql_ptr->link,offset);
			
				res_buf = (char *) emalloc(res_length+1);
				memset(res_buf, 0, res_length+1);
				dbconvert(NULL,coltype(offset),dbdata(mssql_ptr->link,offset), res_length,SQLCHAR,res_buf,-1);
		
				result->value.str.len = res_length;
				result->value.str.val = res_buf;
				result->type = IS_STRING;
			} else {
				php_error(E_WARNING,"MS SQL:  column %d has unknown data type (%d)", offset, coltype(offset));
				var_reset(result);
			}
		}
	}
}

static void php_mssql_get_column_content_without_type(mssql_link *mssql_ptr,int offset,zval *result, int column_type)
{
	if (dbdatlen(mssql_ptr->link,offset) == 0) {
		var_reset(result);
		return;
	}

	if (dbwillconvert(coltype(offset),SQLCHAR)) {
		unsigned char *res_buf;
		int res_length = dbdatlen(mssql_ptr->link,offset);
		
		res_buf = (unsigned char *) emalloc(res_length+1 + 19);
		memset(res_buf, 0, res_length+1 + 10);
		dbconvert(NULL,coltype(offset),dbdata(mssql_ptr->link,offset), res_length, SQLCHAR,res_buf,-1);
		
		result->value.str.len = strlen(res_buf);
		result->value.str.val = res_buf;
		result->type = IS_STRING;
	} else {
		php_error(E_WARNING,"MS SQL:  column %d has unknown data type (%d)", offset, coltype(offset));
		var_reset(result);
	}
}

/* {{{ proto int mssql_query(string query [, int conn_id])
   Perform an SQL query on a MS-SQL server database */
PHP_FUNCTION(mssql_query)
{
	zval **query, **mssql_link_index;
	int retvalue;
	mssql_link *mssql_ptr;
	mssql_result *result;
	int id;
	int num_fields;
	int blocks_initialized=1;
	int i,j;
	int *column_types;
	MSSQLLS_FETCH();


	switch(ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &query)==FAILURE) {
				RETURN_FALSE;
			}
			id = php_mssql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU MSSQLLS_CC);
			CHECK_LINK(id);
			break;
		case 2:
			if (zend_get_parameters_ex(2, &query, &mssql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	ZEND_FETCH_RESOURCE2(mssql_ptr, mssql_link *, mssql_link_index, id, "MS SQL-Link", le_link, le_plink);
	
	convert_to_string_ex(query);
	
	if (dbcmd(mssql_ptr->link, (*query)->value.str.val)==FAIL) {
		php_error(E_WARNING,"MS SQL:  Unable to set query");
		RETURN_FALSE;
	}
	if (dbsqlexec(mssql_ptr->link)==FAIL || dbresults(mssql_ptr->link)==FAIL) {
		php_error(E_WARNING,"MS SQL:  Query failed");
		RETURN_FALSE;
	}
	
	/* The following is more or less the equivalent of mysql_store_result().
	 * fetch all rows from the server into the row buffer, thus:
	 * 1)  Being able to fire up another query without explicitly reading all rows
	 * 2)  Having numrows accessible
	 */
	retvalue=dbnextrow(mssql_ptr->link);
	
	if (retvalue==FAIL) {
		RETURN_FALSE;
	}

	num_fields = dbnumcols(mssql_ptr->link);
	if (num_fields <= 0) {
		RETURN_TRUE;
	}
	
	column_types = (int *) emalloc(sizeof(int) * num_fields);
	for (i=0; i<num_fields; i++) {
		column_types[i] = coltype(i+1);
	}
	
	result = (mssql_result *) emalloc(sizeof(mssql_result));
	result->data = (zval **) emalloc(sizeof(zval *)*MSSQL_ROWS_BLOCK);
	result->mssql_ptr = mssql_ptr;
	result->cur_field=result->cur_row=result->num_rows=0;
	result->num_fields = num_fields;

	i=0;
	while (retvalue!=FAIL && retvalue!=NO_MORE_ROWS) {
		result->num_rows++;
		if (result->num_rows > blocks_initialized*MSSQL_ROWS_BLOCK) {
			result->data = (zval **) erealloc(result->data,sizeof(zval *)*MSSQL_ROWS_BLOCK*(++blocks_initialized));
		}
		result->data[i] = (zval *) emalloc(sizeof(zval)*num_fields);
		for (j=1; j<=num_fields; j++) {
			INIT_PZVAL(&result->data[i][j-1]);
			MS_SQL_G(get_column_content(mssql_ptr, j, &result->data[i][j-1], column_types[j-1]));
		}
		retvalue=dbnextrow(mssql_ptr->link);
		dbclrbuf(mssql_ptr->link,DBLASTROW(mssql_ptr->link)-1); 
		i++;
	}
	result->num_rows = DBCOUNT(mssql_ptr->link);
	
	result->fields = (mssql_field *) emalloc(sizeof(mssql_field)*num_fields);
	j=0;
	for (i=0; i<num_fields; i++) {
		char *fname = (char *)dbcolname(mssql_ptr->link,i+1);
		char computed_buf[16];
		
		if (*fname) {
			result->fields[i].name = estrdup(fname);
		} else {
			if (j>0) {
				snprintf(computed_buf,16,"computed%d",j);
			} else {
				strcpy(computed_buf,"computed");
			}
			result->fields[i].name = estrdup(computed_buf);
			j++;
		}
		result->fields[i].max_length = dbcollen(mssql_ptr->link,i+1);
		result->fields[i].column_source = estrdup(dbcolsource(mssql_ptr->link,i+1));
		if (!result->fields[i].column_source) {
			result->fields[i].column_source = empty_string;
		}
		result->fields[i].type = column_types[i];
		/* set numeric flag */
		switch (column_types[i]) {
			case SQLINT1:
			case SQLINT2:
			case SQLINT4:
			case SQLFLT8:
			case SQLNUMERIC:
			case SQLDECIMAL:
				result->fields[i].numeric = 1;
				break;
			case SQLCHAR:
			case SQLVARCHAR:
			case SQLTEXT:
			default:
				result->fields[i].numeric = 0;
				break;
		}
	}
	efree(column_types);

	ZEND_REGISTER_RESOURCE(return_value, result, le_result);
}

/* }}} */

/* {{{ proto int mssql_free_result(string result_index)
   Free a MS-SQL result index */
PHP_FUNCTION(mssql_free_result)
{
	zval **mssql_result_index;
	mssql_result *result;
	MSSQLLS_FETCH();

	
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &mssql_result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	if ((*mssql_result_index)->type==IS_RESOURCE && (*mssql_result_index)->value.lval==0) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(result, mssql_result *, mssql_result_index, -1, "MS SQL-result", le_result);	
	zend_list_delete((*mssql_result_index)->value.lval);
	RETURN_TRUE;
}

/* }}} */

/* {{{ proto string mssql_get_last_message(void)
   Gets the last message from the MS-SQL server */
PHP_FUNCTION(mssql_get_last_message)
{
	MSSQLLS_FETCH();

	RETURN_STRING(MS_SQL_G(server_message),1);
}

/* }}} */

/* {{{ proto int mssql_num_rows(int mssql_result_index)
   Returns the number of rows fetched in from the result id specified */
PHP_FUNCTION(mssql_num_rows)
{
	zval **mssql_result_index;
	mssql_result *result;
	MSSQLLS_FETCH();

	
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &mssql_result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(result, mssql_result *, mssql_result_index, -1, "MS SQL-result", le_result);	
	
	return_value->value.lval = result->num_rows;
	return_value->type = IS_LONG;
}

/* }}} */

/* {{{ proto int mssql_num_fields(int mssql_result_index)
   Returns the number of fields fetched in from the result id specified */
PHP_FUNCTION(mssql_num_fields)
{
	zval **mssql_result_index;
	mssql_result *result;
	MSSQLLS_FETCH();

	
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &mssql_result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(result, mssql_result *, mssql_result_index, -1, "MS SQL-result", le_result);	

	return_value->value.lval = result->num_fields;
	return_value->type = IS_LONG;
}

/* }}} */

/* {{{ proto array mssql_fetch_row(int result_id)
   Returns an array of the current row in the result set specified by result_id */
PHP_FUNCTION(mssql_fetch_row)
{
	zval **mssql_result_index;
	int i;
	mssql_result *result;
	zval *field_content;
	MSSQLLS_FETCH();

	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &mssql_result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(result, mssql_result *, mssql_result_index, -1, "MS SQL-result", le_result);	
	
	if (result->cur_row >= result->num_rows) {
		RETURN_FALSE;
	}
	
	array_init(return_value);
	for (i=0; i<result->num_fields; i++) {
		MAKE_STD_ZVAL(field_content);
		*field_content = result->data[result->cur_row][i];
		ZVAL_COPY_CTOR(field_content);
		zend_hash_index_update(return_value->value.ht, i, (void *) &field_content, sizeof(zval *), NULL);
	}
	result->cur_row++;
}

/* }}} */

static void php_mssql_fetch_hash(INTERNAL_FUNCTION_PARAMETERS)
{
	zval **mssql_result_index;
	mssql_result *result;
	int i;
	zval *tmp;
	MSSQLLS_FETCH();
	PLS_FETCH();

	
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &mssql_result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(result, mssql_result *, mssql_result_index, -1, "MS SQL-result", le_result);	
	
	if (result->cur_row >= result->num_rows) {
		RETURN_FALSE;
	}
	
	if (array_init(return_value)==FAILURE) {
		RETURN_FALSE;
	}
	
	for (i=0; i<result->num_fields; i++) {
		MAKE_STD_ZVAL(tmp);
		*tmp = result->data[result->cur_row][i];
		ZVAL_COPY_CTOR(tmp);
		if (PG(magic_quotes_runtime) && tmp->type == IS_STRING) {
			tmp->value.str.val = php_addslashes(tmp->value.str.val, tmp->value.str.len, &tmp->value.str.len,1);
		}
		zend_hash_index_update(return_value->value.ht, i, (void *) &tmp, sizeof(zval *), NULL);
		zend_hash_update(return_value->value.ht, result->fields[i].name, strlen(result->fields[i].name)+1, (void *) &tmp, sizeof(zval *), NULL);
	}
	result->cur_row++;
}

/* {{{ proto object mssql_fetch_object(int result_id)
   Returns a psuedo-object of the current row in the result set specified by result_id */
PHP_FUNCTION(mssql_fetch_object)
{
	php_mssql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	if (return_value->type==IS_ARRAY) {
		return_value->type=IS_OBJECT;
	}
}

/* }}} */

/* {{{ proto array mssql_fetch_array(int result_id)
   Returns an associative array of the current row in the result set specified by result_id */
PHP_FUNCTION(mssql_fetch_array)
{
	php_mssql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/* }}} */


/* {{{ proto int mssql_data_seek(int result_id, int offset)
   Moves the internal row pointer of the MS-SQL result associated with the specified result identifier to pointer to the specified row number */
PHP_FUNCTION(mssql_data_seek)
{
	zval **mssql_result_index, **offset;
	int type,id;
	mssql_result *result;
	MSSQLLS_FETCH();


	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &mssql_result_index, &offset)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(mssql_result_index);
	id = (*mssql_result_index)->value.lval;
	
	result = (mssql_result *) zend_list_find(id,&type);
	if (type!=le_result) {
		php_error(E_WARNING,"%d is not a MS SQL result index",id);
		RETURN_FALSE;
	}

	convert_to_long_ex(offset);
	if ((*offset)->value.lval<0 || (*offset)->value.lval>=result->num_rows) {
		php_error(E_WARNING,"MS SQL:  Bad row offset");
		RETURN_FALSE;
	}
	
	result->cur_row = (*offset)->value.lval;
	RETURN_TRUE;
}

/* }}} */

static char *php_mssql_get_field_name(int type)
{
	switch (type) {
		case SQLBINARY:
		case SQLVARBINARY:
			return "blob";
			break;
		case SQLCHAR:
		case SQLVARCHAR:
			return "char";
			break;
		case SQLTEXT:
			return "text";
			break;
		case SQLDATETIME:
		case SQLDATETIM4:
		case SQLDATETIMN:
			return "datetime";
			break;
		case SQLDECIMAL:
		case SQLFLT8:
		case SQLFLTN:
			return "real";
			break;
		case SQLINT1:
		case SQLINT2:
		case SQLINT4:
		case SQLINTN:
			return "int";
			break;
		case SQLNUMERIC:
			return "numeric";
			break;
		case SQLMONEY:
		case SQLMONEY4:
		case SQLMONEYN:
			return "money";
			break;
		case SQLBIT:
			return "bit";
			break;
		case SQLIMAGE:
			return "image";
			break;
		default:
			return "unknown";
			break;
	}
}

/* {{{ proto object mssql_fetch_field(int result_id [, int offset])
   Gets information about certain fields in a query result */
PHP_FUNCTION(mssql_fetch_field)
{
	zval **mssql_result_index, **offset;
	int type,id,field_offset;
	mssql_result *result;
	MSSQLLS_FETCH();


	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &mssql_result_index)==FAILURE) {
				RETURN_FALSE;
			}
			field_offset=-1;
			break;
		case 2:
			if (zend_get_parameters_ex(2, &mssql_result_index, &offset)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long_ex(offset);
			field_offset = (*offset)->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	convert_to_long_ex(mssql_result_index);
	id = (*mssql_result_index)->value.lval;
	
	result = (mssql_result *) zend_list_find(id,&type);
	if (type!=le_result) {
		php_error(E_WARNING,"%d is not a MS SQL result index",id);
		RETURN_FALSE;
	}
	
	if (field_offset==-1) {
		field_offset = result->cur_field;
		result->cur_field++;
	}
	
	if (field_offset<0 || field_offset >= result->num_fields) {
		if (ZEND_NUM_ARGS()==2) { /* field specified explicitly */
			php_error(E_WARNING,"MS SQL:  Bad column offset");
		}
		RETURN_FALSE;
	}

	if (object_init(return_value)==FAILURE) {
		RETURN_FALSE;
	}
	add_property_string(return_value, "name",result->fields[field_offset].name, 1);
	add_property_long(return_value, "max_length",result->fields[field_offset].max_length);
	add_property_string(return_value, "column_source",result->fields[field_offset].column_source, 1);
	add_property_long(return_value, "numeric", result->fields[field_offset].numeric);
	add_property_string(return_value, "type", php_mssql_get_field_name(result->fields[field_offset].type), 1);
}

/* }}} */

/* {{{ proto int mssql_field_length(int result_id [, int offset])
   Get the length of a MS-SQL field */
PHP_FUNCTION(mssql_field_length)
{
	zval **mssql_result_index, **offset;
	int type,id,field_offset;
	mssql_result *result;
	MSSQLLS_FETCH();


	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &mssql_result_index)==FAILURE) {
				RETURN_FALSE;
			}
			field_offset=-1;
			break;
		case 2:
			if (zend_get_parameters_ex(2, &mssql_result_index, &offset)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long_ex(offset);
			field_offset = (*offset)->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	convert_to_long_ex(mssql_result_index);
	id = (*mssql_result_index)->value.lval;
	
	result = (mssql_result *) zend_list_find(id,&type);
	if (type!=le_result) {
		php_error(E_WARNING,"%d is not a MS SQL result index",id);
		RETURN_FALSE;
	}
	
	if (field_offset==-1) {
		field_offset = result->cur_field;
		result->cur_field++;
	}
	
	if (field_offset<0 || field_offset >= result->num_fields) {
		if (ZEND_NUM_ARGS()==2) { /* field specified explicitly */
			php_error(E_WARNING,"MS SQL:  Bad column offset");
		}
		RETURN_FALSE;
	}

	return_value->value.lval = result->fields[field_offset].max_length;
	return_value->type = IS_LONG;
}

/* }}} */

/* {{{ proto string mssql_field_name(int result_id [, int offset])
   Returns the name of the field given by offset in the result set given by result_id */
PHP_FUNCTION(mssql_field_name)
{
	zval **mssql_result_index, **offset;
	int type,id,field_offset;
	mssql_result *result;
	MSSQLLS_FETCH();


	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &mssql_result_index)==FAILURE) {
				RETURN_FALSE;
			}
			field_offset=-1;
			break;
		case 2:
			if (zend_get_parameters_ex(2, &mssql_result_index, &offset)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long_ex(offset);
			field_offset = (*offset)->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	convert_to_long_ex(mssql_result_index);
	id = (*mssql_result_index)->value.lval;
	
	result = (mssql_result *) zend_list_find(id,&type);
	if (type!=le_result) {
		php_error(E_WARNING,"%d is not a MS SQL result index",id);
		RETURN_FALSE;
	}
	
	if (field_offset==-1) {
		field_offset = result->cur_field;
		result->cur_field++;
	}
	
	if (field_offset<0 || field_offset >= result->num_fields) {
		if (ZEND_NUM_ARGS()==2) { /* field specified explicitly */
			php_error(E_WARNING,"MS SQL:  Bad column offset");
		}
		RETURN_FALSE;
	}

	return_value->value.str.val = estrdup(result->fields[field_offset].name);
	return_value->value.str.len = strlen(result->fields[field_offset].name);
	return_value->type = IS_STRING;
}

/* }}} */

/* {{{ proto string mssql_field_type(int result_id [, int offset])
   Returns the type of a field */
PHP_FUNCTION(mssql_field_type)
{
	zval **mssql_result_index, **offset;
	int type,id,field_offset;
	mssql_result *result;
	MSSQLLS_FETCH();


	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &mssql_result_index)==FAILURE) {
				RETURN_FALSE;
			}
			field_offset=-1;
			break;
		case 2:
			if (zend_get_parameters_ex(2, &mssql_result_index, &offset)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long_ex(offset);
			field_offset = (*offset)->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	convert_to_long_ex(mssql_result_index);
	id = (*mssql_result_index)->value.lval;
	
	result = (mssql_result *) zend_list_find(id,&type);
	if (type!=le_result) {
		php_error(E_WARNING,"%d is not a MS SQL result index",id);
		RETURN_FALSE;
	}
	
	if (field_offset==-1) {
		field_offset = result->cur_field;
		result->cur_field++;
	}
	
	if (field_offset<0 || field_offset >= result->num_fields) {
		if (ZEND_NUM_ARGS()==2) { /* field specified explicitly */
			php_error(E_WARNING,"MS SQL:  Bad column offset");
		}
		RETURN_FALSE;
	}

	return_value->value.str.val = estrdup(php_mssql_get_field_name(result->fields[field_offset].type));
	return_value->value.str.len = strlen(php_mssql_get_field_name(result->fields[field_offset].type));
	return_value->type = IS_STRING;
}

/* }}} */

/* {{{ proto bool mssql_field_seek(int result_id, int offset)
   Seeks to the specified field offset */
PHP_FUNCTION(mssql_field_seek)
{
	zval **mssql_result_index, **offset;
	int type,id,field_offset;
	mssql_result *result;
	MSSQLLS_FETCH();


	if (ZEND_NUM_ARGS()!=2 || zend_get_parameters_ex(2, &mssql_result_index, &offset)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(mssql_result_index);
	id = (*mssql_result_index)->value.lval;
	
	result = (mssql_result *) zend_list_find(id,&type);
	if (type!=le_result) {
		php_error(E_WARNING,"%d is not a MS SQL result index",id);
		RETURN_FALSE;
	}
	
	convert_to_long_ex(offset);
	field_offset = (*offset)->value.lval;
	
	if (field_offset<0 || field_offset >= result->num_fields) {
		php_error(E_WARNING,"MS SQL:  Bad column offset");
		RETURN_FALSE;
	}

	result->cur_field = field_offset;
	RETURN_TRUE;
}

/* }}} */

/* {{{ proto string mssql_result(int result_id, int row, mixed field)
   Returns the contents of one cell from a MS-SQL result set */
PHP_FUNCTION(mssql_result)
{
	zval **row, **field, **mssql_result_index;
	int id,type,field_offset=0;
	mssql_result *result;
	MSSQLLS_FETCH();


	if (ZEND_NUM_ARGS()!=3 || zend_get_parameters_ex(3, &mssql_result_index, &row, &field)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(mssql_result_index);
	id = (*mssql_result_index)->value.lval;
	
	result = (mssql_result *) zend_list_find(id,&type);
	if (type!=le_result) {
		php_error(E_WARNING,"%d is not a MS SQL result index",id);
		RETURN_FALSE;
	}
	
	convert_to_long_ex(row);
	if ((*row)->value.lval < 0 || (*row)->value.lval >= result->num_rows) {
		php_error(E_WARNING,"MS SQL:  Bad row offset (%d)", (*row)->value.lval);
		RETURN_FALSE;
	}

	switch((*field)->type) {
		case IS_STRING: {
			int i;

			for (i=0; i<result->num_fields; i++) {
				if (!strcasecmp(result->fields[i].name, (*field)->value.str.val)) {
					field_offset = i;
					break;
				}
			}
			if (i>=result->num_fields) { /* no match found */
				php_error(E_WARNING,"MS SQL:  %s field not found in result", (*field)->value.str.val);
				RETURN_FALSE;
			}
			break;
		}
		default:
			convert_to_long_ex(field);
			field_offset = (*field)->value.lval;
			if (field_offset<0 || field_offset>=result->num_fields) {
				php_error(E_WARNING,"MS SQL:  Bad column offset specified");
				RETURN_FALSE;
			}
			break;
	}

	*return_value = result->data[(*row)->value.lval][field_offset];
	ZVAL_COPY_CTOR(return_value);
}

/* }}} */

/* {{{ proto void mssql_min_error_severity(int severity)
   Sets the lower error severity */
PHP_FUNCTION(mssql_min_error_severity)
{
	zval **severity;
	MSSQLLS_FETCH();

	
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &severity)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(severity);
	MS_SQL_G(min_error_severity) = (*severity)->value.lval;
}

/* }}} */

/* {{{ proto void mssql_min_message_severity(int severity)
   Sets the lower message severity */
PHP_FUNCTION(mssql_min_message_severity)
{
	zval **severity;
	MSSQLLS_FETCH();

	
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &severity)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(severity);
	MS_SQL_G(min_message_severity) = (*severity)->value.lval;
}
/* }}} */

#endif
