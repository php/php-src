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
   | Author: Frank M. Kromann <frank@frontbase.com>                       |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef COMPILE_DL_MSSQL
#define HAVE_MSSQL 1
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_globals.h"
#include "ext/standard/php_standard.h"
#include "ext/standard/info.h"
#include "php_mssql.h"
#include "php_ini.h"

#if HAVE_MSSQL
#define SAFE_STRING(s) ((s)?(s):"")

#define MSSQL_ASSOC		1<<0
#define MSSQL_NUM		1<<1
#define MSSQL_BOTH		(MSSQL_ASSOC|MSSQL_NUM)

static int le_result, le_link, le_plink, le_statement;

static void php_mssql_get_column_content_with_type(mssql_link *mssql_ptr,int offset,zval *result, int column_type TSRMLS_DC);
static void php_mssql_get_column_content_without_type(mssql_link *mssql_ptr,int offset,zval *result, int column_type TSRMLS_DC);

static void _mssql_bind_hash_dtor(void *data);
static unsigned char a3_arg_force_ref[] = { 3, BYREF_NONE, BYREF_NONE, BYREF_FORCE };

function_entry mssql_functions[] = {
	PHP_FE(mssql_connect,				NULL)
	PHP_FE(mssql_pconnect,				NULL)
	PHP_FE(mssql_close,					NULL)
	PHP_FE(mssql_select_db,				NULL)
	PHP_FE(mssql_query,					NULL)
	PHP_FE(mssql_fetch_batch,			NULL)
	PHP_FE(mssql_rows_affected,			NULL)
	PHP_FE(mssql_free_result,			NULL)
	PHP_FE(mssql_get_last_message,		NULL)
	PHP_FE(mssql_num_rows,				NULL)
	PHP_FE(mssql_num_fields,			NULL)
	PHP_FE(mssql_fetch_field,			NULL)
	PHP_FE(mssql_fetch_row,				NULL)
	PHP_FE(mssql_fetch_array,			NULL)
	PHP_FE(mssql_fetch_assoc,			NULL)
	PHP_FE(mssql_fetch_object,			NULL)
	PHP_FE(mssql_field_length,			NULL)
	PHP_FE(mssql_field_name,			NULL)
	PHP_FE(mssql_field_type,			NULL)
	PHP_FE(mssql_data_seek,				NULL)
	PHP_FE(mssql_field_seek,			NULL)
	PHP_FE(mssql_result,				NULL)
	PHP_FE(mssql_next_result,			NULL)
	PHP_FE(mssql_min_error_severity,	NULL)
	PHP_FE(mssql_min_message_severity,	NULL)
 	PHP_FE(mssql_init,					NULL)
 	PHP_FE(mssql_bind,					a3_arg_force_ref)
 	PHP_FE(mssql_execute,				NULL)
	PHP_FE(mssql_free_statement,		NULL)
 	PHP_FE(mssql_guid_string,			NULL)
	{NULL, NULL, NULL}
};

zend_module_entry mssql_module_entry = 
{
	STANDARD_MODULE_HEADER,
	"mssql", 
	mssql_functions, 
	PHP_MINIT(mssql), 
	PHP_MSHUTDOWN(mssql), 
	PHP_RINIT(mssql), 
	PHP_RSHUTDOWN(mssql), 
	PHP_MINFO(mssql), 
        NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

ZEND_DECLARE_MODULE_GLOBALS(mssql)

#ifdef COMPILE_DL_MSSQL
ZEND_GET_MODULE(mssql)
#endif

#define CHECK_LINK(link) { if (link==-1) { php_error_docref(NULL TSRMLS_CC, E_WARNING, "A link to the server could not be established"); RETURN_FALSE; } }

static PHP_INI_DISP(display_text_size)
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

	if (atoi(value) == -1) {
		PUTS("Server default");
	} else {
		php_printf("%s", value);
	}
}

PHP_INI_BEGIN()
	STD_PHP_INI_BOOLEAN("mssql.allow_persistent",		"1",	PHP_INI_SYSTEM,	OnUpdateBool,	allow_persistent,			zend_mssql_globals,		mssql_globals)
	STD_PHP_INI_ENTRY_EX("mssql.max_persistent",		"-1",	PHP_INI_SYSTEM,	OnUpdateInt,	max_persistent,				zend_mssql_globals,		mssql_globals,	display_link_numbers)
	STD_PHP_INI_ENTRY_EX("mssql.max_links",				"-1",	PHP_INI_SYSTEM,	OnUpdateInt,	max_links,					zend_mssql_globals,		mssql_globals,	display_link_numbers)
	STD_PHP_INI_ENTRY_EX("mssql.min_error_severity",	"10",	PHP_INI_ALL,	OnUpdateInt,	cfg_min_error_severity,		zend_mssql_globals,		mssql_globals,	display_link_numbers)
	STD_PHP_INI_ENTRY_EX("mssql.min_message_severity",	"10",	PHP_INI_ALL,	OnUpdateInt,	cfg_min_message_severity,	zend_mssql_globals,		mssql_globals,	display_link_numbers)
	STD_PHP_INI_BOOLEAN("mssql.compatability_mode",		"0",	PHP_INI_ALL,	OnUpdateBool,	compatability_mode,			zend_mssql_globals,		mssql_globals)
	STD_PHP_INI_ENTRY_EX("mssql.connect_timeout",    	"5",	PHP_INI_ALL,	OnUpdateInt,	connect_timeout,			zend_mssql_globals,		mssql_globals,	display_link_numbers)
	STD_PHP_INI_ENTRY_EX("mssql.timeout",      			"60",	PHP_INI_ALL,	OnUpdateInt,	timeout,					zend_mssql_globals,		mssql_globals,	display_link_numbers)
	STD_PHP_INI_ENTRY_EX("mssql.textsize",   			"-1",	PHP_INI_ALL,	OnUpdateInt,	textsize,					zend_mssql_globals,		mssql_globals,	display_text_size)
	STD_PHP_INI_ENTRY_EX("mssql.textlimit",   			"-1",	PHP_INI_ALL,	OnUpdateInt,	textlimit,					zend_mssql_globals,		mssql_globals,	display_text_size)
	STD_PHP_INI_ENTRY_EX("mssql.batchsize",   			"0",	PHP_INI_ALL,	OnUpdateInt,	batchsize,					zend_mssql_globals,		mssql_globals,	display_link_numbers)
	STD_PHP_INI_BOOLEAN("mssql.datetimeconvert",  		"1",	PHP_INI_ALL,	OnUpdateBool,	datetimeconvert,			zend_mssql_globals,		mssql_globals)
	STD_PHP_INI_BOOLEAN("mssql.secure_connection",		"0",	PHP_INI_SYSTEM, OnUpdateBool,	secure_connection,			zend_mssql_globals,		mssql_globals)
	STD_PHP_INI_ENTRY_EX("mssql.max_procs",				"25",	PHP_INI_ALL,	OnUpdateInt,	max_procs,					zend_mssql_globals,		mssql_globals,	display_link_numbers)
PHP_INI_END()

/* error handler */
static int php_mssql_error_handler(DBPROCESS *dbproc, int severity, int dberr, int oserr, char *dberrstr, char *oserrstr)
{
	TSRMLS_FETCH();

	if (severity >= MS_SQL_G(min_error_severity)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s (severity %d)", dberrstr, severity);
	}
	return INT_CANCEL;  
}

/* message handler */
static int php_mssql_message_handler(DBPROCESS *dbproc, DBINT msgno,int msgstate, int severity,char *msgtext,char *srvname, char *procname,DBUSMALLINT line)
{
	TSRMLS_FETCH();

	if (severity >= MS_SQL_G(min_message_severity)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "message: %s (severity %d)", msgtext, severity);
	}
	if (MS_SQL_G(server_message)) {
		STR_FREE(MS_SQL_G(server_message));
	}
	MS_SQL_G(server_message) = estrdup(msgtext);
	return 0;
}

static int _clean_invalid_results(list_entry *le TSRMLS_DC)
{
	if (Z_TYPE_P(le) == le_result) {
		mssql_link *mssql_ptr = ((mssql_result *) le->ptr)->mssql_ptr;
		
		if (!mssql_ptr->valid) {
			return 1;
		}
	}
	return 0;
}

static void _free_result(mssql_result *result, int free_fields) 
{
	int i,j;

	if (result->data) {
		for (i=0; i<result->num_rows; i++) {
			if (result->data[i]) {
				for (j=0; j<result->num_fields; j++) {
					zval_dtor(&result->data[i][j]);
				}
				efree(result->data[i]);
			}
		}
		efree(result->data);
		result->data = NULL;
		result->blocks_initialized = 0;
	}
	
	if (free_fields && result->fields) {
		for (i=0; i<result->num_fields; i++) {
			STR_FREE(result->fields[i].name);
			STR_FREE(result->fields[i].column_source);
		}
		efree(result->fields);
	}
}

static void _free_mssql_statement(mssql_statement *statement)
{
	if (statement->binds) {
		zend_hash_destroy(statement->binds);
		efree(statement->binds);
	}
	
	efree(statement);
}

static void _free_mssql_result(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	mssql_result *result = (mssql_result *)rsrc->ptr;

	_free_result(result, 1);
	efree(result);
}

static void php_mssql_set_default_link(int id TSRMLS_DC)
{
	if (MS_SQL_G(default_link)!=-1) {
		zend_list_delete(MS_SQL_G(default_link));
	}
	MS_SQL_G(default_link) = id;
	zend_list_addref(id);
}

static void _close_mssql_link(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	mssql_link *mssql_ptr = (mssql_link *)rsrc->ptr;

	mssql_ptr->valid = 0;
	zend_hash_apply(&EG(regular_list),(apply_func_t) _clean_invalid_results TSRMLS_CC);
	dbclose(mssql_ptr->link);
	dbfreelogin(mssql_ptr->login);
	efree(mssql_ptr);
	MS_SQL_G(num_links)--;
}


static void _close_mssql_plink(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	mssql_link *mssql_ptr = (mssql_link *)rsrc->ptr;

	dbclose(mssql_ptr->link);
	dbfreelogin(mssql_ptr->login);
	free(mssql_ptr);
	MS_SQL_G(num_persistent)--;
	MS_SQL_G(num_links)--;
}

static void _mssql_bind_hash_dtor(void *data)
{
	mssql_bind *bind= (mssql_bind *) data;

   	zval_ptr_dtor(&(bind->zval));
}

static void php_mssql_init_globals(zend_mssql_globals *mssql_globals)
{
	long compatability_mode;

	mssql_globals->num_persistent = 0;
	if (cfg_get_long("mssql.compatability_mode", &compatability_mode) == SUCCESS) {
		if (compatability_mode) {
			mssql_globals->get_column_content = php_mssql_get_column_content_without_type;	
		} else {
			mssql_globals->get_column_content = php_mssql_get_column_content_with_type;
		}
	} else {
		mssql_globals->get_column_content = php_mssql_get_column_content_with_type;
	}
}

PHP_MINIT_FUNCTION(mssql)
{
	ZEND_INIT_MODULE_GLOBALS(mssql, php_mssql_init_globals, NULL);

	REGISTER_INI_ENTRIES();

	le_statement = register_list_destructors(_free_mssql_statement, NULL);
	le_result = zend_register_list_destructors_ex(_free_mssql_result, NULL, "mssql result", module_number);
	le_link = zend_register_list_destructors_ex(_close_mssql_link, NULL, "mssql link", module_number);
	le_plink = zend_register_list_destructors_ex(NULL, _close_mssql_plink, "mssql link persistent", module_number);
	Z_TYPE(mssql_module_entry) = type;

	if (dbinit()==FAIL) {
		return FAILURE;
	}

	/* BEGIN MSSQL data types for mssql_bind */
	REGISTER_LONG_CONSTANT("MSSQL_ASSOC", MSSQL_ASSOC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MSSQL_NUM", MSSQL_NUM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MSSQL_BOTH", MSSQL_BOTH, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("SQLTEXT",SQLTEXT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLVARCHAR",SQLVARCHAR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLCHAR",SQLCHAR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLINT1",SQLINT1, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLINT2",SQLINT2, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLINT4",SQLINT4, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLBIT",SQLBIT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLFLT4",SQLFLT4, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLFLT8",SQLFLT8, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLFLTN",SQLFLTN, CONST_CS | CONST_PERSISTENT);
	/* END MSSQL data types for mssql_sp_bind */

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
	MS_SQL_G(default_link) = -1;
	MS_SQL_G(num_links) = MS_SQL_G(num_persistent);
	MS_SQL_G(appname) = estrndup("PHP 4.0",7);
	MS_SQL_G(server_message) = empty_string;
	MS_SQL_G(min_error_severity) = MS_SQL_G(cfg_min_error_severity);
	MS_SQL_G(min_message_severity) = MS_SQL_G(cfg_min_message_severity);
	if (MS_SQL_G(connect_timeout) < 1) MS_SQL_G(connect_timeout) = 1;
	dbsetlogintime(MS_SQL_G(connect_timeout));
	if (MS_SQL_G(timeout) < 0) MS_SQL_G(timeout) = 60;
	dbsettime(MS_SQL_G(timeout));
	dbsetmaxprocs((SHORT)MS_SQL_G(max_procs));

	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(mssql)
{
	STR_FREE(MS_SQL_G(appname));
	if (MS_SQL_G(server_message)) {
		STR_FREE(MS_SQL_G(server_message));
	}
	return SUCCESS;
}

PHP_MINFO_FUNCTION(mssql)
{
	char buf[32];

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

static void php_mssql_do_connect(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	char *user, *passwd, *host;
	char *hashed_details;
	int hashed_details_length;
	mssql_link mssql, *mssql_ptr;
	char buffer[32];

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
				host = Z_STRVAL_PP(yyhost);
				user=passwd=NULL;
				hashed_details_length = Z_STRLEN_PP(yyhost)+5+3;
				hashed_details = (char *) emalloc(hashed_details_length+1);
				sprintf(hashed_details,"mssql_%s__",Z_STRVAL_PP(yyhost));
			}
			break;
		case 2: {
				zval **yyhost,**yyuser;
				
				if (zend_get_parameters_ex(2, &yyhost, &yyuser)==FAILURE) {
					WRONG_PARAM_COUNT;
				}
				convert_to_string_ex(yyhost);
				convert_to_string_ex(yyuser);
				host = Z_STRVAL_PP(yyhost);
				user = Z_STRVAL_PP(yyuser);
				passwd=NULL;
				hashed_details_length = Z_STRLEN_PP(yyhost)+Z_STRLEN_PP(yyuser)+5+3;
				hashed_details = (char *) emalloc(hashed_details_length+1);
				sprintf(hashed_details,"mssql_%s_%s_",Z_STRVAL_PP(yyhost),Z_STRVAL_PP(yyuser));
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
				host = Z_STRVAL_PP(yyhost);
				user = Z_STRVAL_PP(yyuser);
				passwd = Z_STRVAL_PP(yypasswd);
				hashed_details_length = Z_STRLEN_PP(yyhost)+Z_STRLEN_PP(yyuser)+Z_STRLEN_PP(yypasswd)+5+3;
				hashed_details = (char *) emalloc(hashed_details_length+1);
				sprintf(hashed_details,"mssql_%s_%s_%s",Z_STRVAL_PP(yyhost),Z_STRVAL_PP(yyuser),Z_STRVAL_PP(yypasswd)); /* SAFE */
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	if (hashed_details == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Out of memory");
		RETURN_FALSE;
	}

	/* set a DBLOGIN record */	
	if ((mssql.login = dblogin()) == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to allocate login record");
		RETURN_FALSE;
	}
	
	DBERRHANDLE(mssql.login, (EHANDLEFUNC) php_mssql_error_handler);
	DBMSGHANDLE(mssql.login, (MHANDLEFUNC) php_mssql_message_handler);

#ifndef HAVE_FREETDS
	if (MS_SQL_G(secure_connection)){
		DBSETLSECURE(mssql.login);
	}
	else {
#endif
		if (user) {
			DBSETLUSER(mssql.login,user);
		}
		if (passwd) {
			DBSETLPWD(mssql.login,passwd);
		}
#ifndef HAVE_FREETDS
	}
#endif
	DBSETLAPP(mssql.login,MS_SQL_G(appname));
	mssql.valid = 1;

#ifndef HAVE_FREETDS
	DBSETLVERSION(mssql.login, DBVER60);
#endif
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
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Too many open links (%d)", MS_SQL_G(num_links));
				efree(hashed_details);
				dbfreelogin(mssql.login);
				RETURN_FALSE;
			}
			if (MS_SQL_G(max_persistent) != -1 && MS_SQL_G(num_persistent) >= MS_SQL_G(max_persistent)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Too many open persistent links (%d)", MS_SQL_G(num_persistent));
				efree(hashed_details);
				dbfreelogin(mssql.login);
				RETURN_FALSE;
			}
			/* create the link */
			if ((mssql.link = dbopen(mssql.login, host)) == FAIL) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to connect to server:  %s", host);
				efree(hashed_details);
				dbfreelogin(mssql.login);
				RETURN_FALSE;
			}

			if (DBSETOPT(mssql.link, DBBUFFER, "2")==FAIL) {
				efree(hashed_details);
				dbfreelogin(mssql.login);
				dbclose(mssql.link);
				RETURN_FALSE;
			}

			if (MS_SQL_G(textlimit) != -1) {
				sprintf(buffer, "%li", MS_SQL_G(textlimit));
				if (DBSETOPT(mssql.link, DBTEXTLIMIT, buffer)==FAIL) {
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
			mssql_ptr = (mssql_link *) malloc(sizeof(mssql_link));
			memcpy(mssql_ptr, &mssql, sizeof(mssql_link));
			Z_TYPE(new_le) = le_plink;
			new_le.ptr = mssql_ptr;
			if (zend_hash_update(&EG(persistent_list), hashed_details, hashed_details_length + 1, &new_le, sizeof(list_entry), NULL)==FAILURE) {
				free(mssql_ptr);
				efree(hashed_details);
				dbfreelogin(mssql.login);
				RETURN_FALSE;
			}
			MS_SQL_G(num_persistent)++;
			MS_SQL_G(num_links)++;
		} else {  /* we do */
			if (Z_TYPE_P(le) != le_plink) {
#if BROKEN_MSSQL_PCONNECTS
				log_error("PHP/MS SQL:  Hashed persistent link is not a MS SQL link!",php_rqst->server);
#endif
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Hashed persistent link is not a MS SQL link!");
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
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Link to server lost, unable to reconnect");
					zend_hash_del(&EG(persistent_list), hashed_details, hashed_details_length+1);
					efree(hashed_details);
					RETURN_FALSE;
				}
#if BROKEN_MSSQL_PCONNECTS
				log_error("PHP/MS SQL:  Reconnect successful!",php_rqst->server);
#endif
				if (DBSETOPT(mssql_ptr->link, DBBUFFER, "2")==FAIL) {
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

			if (Z_TYPE_P(index_ptr) != le_index_ptr) {
				RETURN_FALSE;
			}
			link = (int) index_ptr->ptr;
			ptr = zend_list_find(link,&type);   /* check if the link is still there */
			if (ptr && (type==le_link || type==le_plink)) {
				zend_list_addref(link);
				Z_LVAL_P(return_value) = link;
				php_mssql_set_default_link(link TSRMLS_CC);
				Z_TYPE_P(return_value) = IS_RESOURCE;
				efree(hashed_details);
				return;
			} else {
				zend_hash_del(&EG(regular_list), hashed_details, hashed_details_length + 1);
			}
		}
		if (MS_SQL_G(max_links) != -1 && MS_SQL_G(num_links) >= MS_SQL_G(max_links)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Too many open links (%d)", MS_SQL_G(num_links));
			efree(hashed_details);
			RETURN_FALSE;
		}
		
		if ((mssql.link=dbopen(mssql.login, host))==NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to connect to server:  %s", host);
			efree(hashed_details);
			RETURN_FALSE;
		}

		if (DBSETOPT(mssql.link, DBBUFFER,"2")==FAIL) {
			efree(hashed_details);
			dbfreelogin(mssql.login);
			dbclose(mssql.link);
			RETURN_FALSE;
		}

		if (MS_SQL_G(textlimit) != -1) {
			sprintf(buffer, "%li", MS_SQL_G(textlimit));
			if (DBSETOPT(mssql.link, DBTEXTLIMIT, buffer)==FAIL) {
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
		new_index_ptr.ptr = (void *) Z_LVAL_P(return_value);
		Z_TYPE(new_index_ptr) = le_index_ptr;
		if (zend_hash_update(&EG(regular_list), hashed_details, hashed_details_length + 1,(void *) &new_index_ptr, sizeof(list_entry),NULL)==FAILURE) {
			efree(hashed_details);
			RETURN_FALSE;
		}
		MS_SQL_G(num_links)++;
	}
	efree(hashed_details);
	php_mssql_set_default_link(Z_LVAL_P(return_value) TSRMLS_CC);
}


static int php_mssql_get_default_link(INTERNAL_FUNCTION_PARAMETERS)
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
	
	switch (ZEND_NUM_ARGS()) {
		case 0:
			id = php_mssql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
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

	if (mssql_link_index) 
		zend_list_delete(Z_RESVAL_PP(mssql_link_index));
	else 
		zend_list_delete(id);

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
	
	switch(ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &db)==FAILURE) {
				RETURN_FALSE;
			}
			id = php_mssql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
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
	
	if (dbuse(mssql_ptr->link, Z_STRVAL_PP(db))==FAIL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to select database:  %s", Z_STRVAL_PP(db));
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}

/* }}} */

static void php_mssql_get_column_content_with_type(mssql_link *mssql_ptr,int offset,zval *result, int column_type  TSRMLS_DC)
{
	if (dbdatlen(mssql_ptr->link,offset) == 0) {
		ZVAL_NULL(result);
		return;
	}

	switch (column_type)
	{
		case SQLINT1:
		case SQLINT2:
		case SQLINT4:
		case SQLINTN: {	
			Z_LVAL_P(result) = (long) anyintcol(offset);
			Z_TYPE_P(result) = IS_LONG;
			break;
		} 
		case SQLCHAR:
		case SQLVARCHAR:
		case SQLTEXT: {
			int length;
			char *data = charcol(offset);

			length=dbdatlen(mssql_ptr->link,offset);
			while (length>0 && data[length-1] == ' ') { /* nuke trailing whitespace */
				length--;
			}
			Z_STRVAL_P(result) = estrndup(data,length);
			Z_STRLEN_P(result) = length;
			Z_TYPE_P(result) = IS_STRING;
			break;
		}
		case SQLFLT4:
			Z_DVAL_P(result) = (double) floatcol4(offset);
			Z_TYPE_P(result) = IS_DOUBLE;
			break;
		case SQLFLT8:
			Z_DVAL_P(result) = (double) floatcol8(offset);
			Z_TYPE_P(result) = IS_DOUBLE;
			break;
		case SQLVARBINARY:
		case SQLBINARY:
		case SQLIMAGE: {
			DBBINARY *bin;
			unsigned char *res_buf;
			int res_length = dbdatlen(mssql_ptr->link, offset);

			res_buf = (unsigned char *) emalloc(res_length+1);
			bin = ((DBBINARY *)dbdata(mssql_ptr->link, offset));
			memcpy(res_buf,bin,res_length);
			res_buf[res_length] = '\0';
			Z_STRLEN_P(result) = res_length;
			Z_STRVAL_P(result) = res_buf;
			Z_TYPE_P(result) = IS_STRING;
			}
			break;
		case SQLNUMERIC:
		default: {
			if (dbwillconvert(column_type,SQLCHAR)) {
				char *res_buf;
				DBDATEREC dateinfo;	
				int res_length = dbdatlen(mssql_ptr->link,offset);

				if ((column_type != SQLDATETIME) || MS_SQL_G(datetimeconvert)) {

					if (column_type == SQLDATETIM4) res_length += 14;
					if (column_type == SQLDATETIME) res_length += 10;
			
					res_buf = (unsigned char *) emalloc(res_length+1);
					res_length = dbconvert(NULL,coltype(offset),dbdata(mssql_ptr->link,offset), res_length, SQLCHAR,res_buf,-1);
				} else {
					dbdatecrack(mssql_ptr->link, &dateinfo, (DBDATETIME *) dbdata(mssql_ptr->link,offset));
			
					res_length = 19;
					res_buf = (unsigned char *) emalloc(res_length+1);
					sprintf(res_buf, "%d-%02d-%02d %02d:%02d:%02d" , dateinfo.year, dateinfo.month, dateinfo.day, dateinfo.hour, dateinfo.minute, dateinfo.second);
				}
		
				Z_STRVAL_P(result) = res_buf;
				Z_STRLEN_P(result) = res_length;
				Z_TYPE_P(result) = IS_STRING;
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "column %d has unknown data type (%d)", offset, coltype(offset));
				ZVAL_FALSE(result);
			}
		}
	}
}

static void php_mssql_get_column_content_without_type(mssql_link *mssql_ptr,int offset,zval *result, int column_type TSRMLS_DC)
{
	if (dbdatlen(mssql_ptr->link,offset) == 0) {
		ZVAL_NULL(result);
		return;
	}

	if (column_type == SQLVARBINARY ||
		column_type == SQLBINARY ||
		column_type == SQLIMAGE) {
		DBBINARY *bin;
		unsigned char *res_buf;
		int res_length = dbdatlen(mssql_ptr->link, offset);

		res_buf = (unsigned char *) emalloc(res_length+1);
		bin = ((DBBINARY *)dbdata(mssql_ptr->link, offset));
		memcpy(res_buf, bin, res_length);
		res_buf[res_length] = '\0';
		Z_STRLEN_P(result) = res_length;
		Z_STRVAL_P(result) = res_buf;
		Z_TYPE_P(result) = IS_STRING;
	}
	else if  (dbwillconvert(coltype(offset),SQLCHAR)) {
		unsigned char *res_buf;
		DBDATEREC dateinfo;	
		int res_length = dbdatlen(mssql_ptr->link,offset);

		if ((column_type != SQLDATETIME) || MS_SQL_G(datetimeconvert)) {

			if (column_type == SQLDATETIM4) res_length += 14;
			if (column_type == SQLDATETIME) res_length += 10;
			
			res_buf = (unsigned char *) emalloc(res_length+1);
			res_length = dbconvert(NULL,coltype(offset),dbdata(mssql_ptr->link,offset), res_length, SQLCHAR, res_buf, -1);

		} else {
			dbdatecrack(mssql_ptr->link, &dateinfo, (DBDATETIME *) dbdata(mssql_ptr->link,offset));
			
			res_length = 19;
			res_buf = (unsigned char *) emalloc(res_length+1);
			sprintf(res_buf, "%d-%02d-%02d %02d:%02d:%02d" , dateinfo.year, dateinfo.month, dateinfo.day, dateinfo.hour, dateinfo.minute, dateinfo.second);
		}

		Z_STRVAL_P(result) = res_buf;
		Z_STRLEN_P(result) = res_length;
		Z_TYPE_P(result) = IS_STRING;
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "column %d has unknown data type (%d)", offset, coltype(offset));
		ZVAL_FALSE(result);
	}
}

static void _mssql_get_sp_result(mssql_link *mssql_ptr, mssql_statement *statement TSRMLS_DC) 
{
	int i, num_rets, type;
	char *parameter;
	mssql_bind *bind;

	/* Now to fetch RETVAL and OUTPUT values*/
	num_rets = dbnumrets(mssql_ptr->link);
	
	if (num_rets!=0) {
		for (i = 1; i <= num_rets; i++) {
			parameter = (char*)dbretname(mssql_ptr->link, i);
			type = dbrettype(mssql_ptr->link, i);
						
			if (statement->binds != NULL) {	/*	Maybe a non-parameter sp	*/
				if (zend_hash_find(statement->binds, parameter, strlen(parameter), (void**)&bind)==SUCCESS) {
					switch (type) {
						case SQLBIT:
						case SQLINT1:
						case SQLINT2:
						case SQLINT4:
							convert_to_long_ex(&bind->zval);
							Z_LVAL_P(bind->zval) = *((int *)(dbretdata(mssql_ptr->link,i)));
							break;
			
						case SQLFLT4:
						case SQLFLT8:
						case SQLFLTN:
							convert_to_double_ex(&bind->zval);
							Z_DVAL_P(bind->zval) = *((double *)(dbretdata(mssql_ptr->link,i)));
							break;

						case SQLCHAR:
						case SQLVARCHAR:
						case SQLTEXT:
							convert_to_string_ex(&bind->zval);
							Z_STRLEN_P(bind->zval) = dbretlen(mssql_ptr->link,i);
							Z_STRVAL_P(bind->zval) = estrndup(dbretdata(mssql_ptr->link,i),Z_STRLEN_P(bind->zval));
							break;
					}
				}
				else {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "An output parameter variable was not provided");
				}
			}
		}
	}
	if (statement->binds != NULL) {	/*	Maybe a non-parameter sp	*/
		if (zend_hash_find(statement->binds, "RETVAL", 6, (void**)&bind)==SUCCESS) {
			if (dbhasretstat(mssql_ptr->link)) {
				convert_to_long_ex(&bind->zval);
				Z_LVAL_P(bind->zval)=dbretstatus(mssql_ptr->link);
			}
			else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "stored procedure has no return value. Nothing was returned into RETVAL");
			}
		}
	}
}

static int _mssql_fetch_batch(mssql_link *mssql_ptr, mssql_result *result, int retvalue TSRMLS_DC) 
{
	int i, j = 0;
	int *column_types;
	char computed_buf[16];

	column_types = (int *) emalloc(sizeof(int) * result->num_fields);
	for (i=0; i<result->num_fields; i++) {
		char *fname = (char *)dbcolname(mssql_ptr->link,i+1);

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

		column_types[i] = coltype(i+1);

		Z_TYPE(result->fields[i]) = column_types[i];
		/* set numeric flag */
		switch (column_types[i]) {
			case SQLINT1:
			case SQLINT2:
			case SQLINT4:
			case SQLINTN:
			case SQLFLT4:
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

	i=0;
	if (!result->data) {
		result->data = (zval **) emalloc(sizeof(zval *)*MSSQL_ROWS_BLOCK*(++result->blocks_initialized));
	}
	while (retvalue!=FAIL && retvalue!=NO_MORE_ROWS) {
		result->num_rows++;
		if (result->num_rows > result->blocks_initialized*MSSQL_ROWS_BLOCK) {
			result->data = (zval **) erealloc(result->data,sizeof(zval *)*MSSQL_ROWS_BLOCK*(++result->blocks_initialized));
		}
		result->data[i] = (zval *) emalloc(sizeof(zval)*result->num_fields);
		for (j=0; j<result->num_fields; j++) {
			INIT_ZVAL(result->data[i][j]);
			MS_SQL_G(get_column_content(mssql_ptr, j+1, &result->data[i][j], column_types[j] TSRMLS_CC));
		}
		if (i<result->batchsize || result->batchsize==0) {
			i++;
			dbclrbuf(mssql_ptr->link,DBLASTROW(mssql_ptr->link)); 
			retvalue=dbnextrow(mssql_ptr->link);
		}
		else
			break;
		result->lastresult = retvalue;
	}
	efree(column_types);
	return i;
}

/* {{{ proto int mssql_fetch_batch(resource result_index)
   Returns the next batch of records */
PHP_FUNCTION(mssql_fetch_batch)
{
	zval **mssql_result_index;
	mssql_result *result;
	mssql_link *mssql_ptr;
	
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &mssql_result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	if (Z_TYPE_PP(mssql_result_index)==IS_RESOURCE && Z_LVAL_PP(mssql_result_index)==0) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(result, mssql_result *, mssql_result_index, -1, "MS SQL-result", le_result);
	mssql_ptr = result->mssql_ptr;
	_free_result(result, 0);
	result->cur_row=result->num_rows=0;
	result->num_rows = _mssql_fetch_batch(mssql_ptr, result, result->lastresult TSRMLS_CC);
	RETURN_LONG(result->num_rows);
}
/* }}} */

/* {{{ proto int mssql_query(string query [, int conn_id [, int batch_size]])
   Perform an SQL query on a MS-SQL server database */
PHP_FUNCTION(mssql_query)
{
	zval **query, **mssql_link_index, **zbatchsize;
	int retvalue;
	mssql_link *mssql_ptr;
	mssql_result *result;
	int id, num_fields;
	int batchsize;

	batchsize = MS_SQL_G(batchsize);
	switch(ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &query)==FAILURE) {
				RETURN_FALSE;
			}
			id = php_mssql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			break;
		case 2:
			if (zend_get_parameters_ex(2, &query, &mssql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		case 3:
			if (zend_get_parameters_ex(3, &query, &mssql_link_index, &zbatchsize)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			convert_to_long_ex(zbatchsize);
			batchsize = Z_LVAL_PP(zbatchsize);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	ZEND_FETCH_RESOURCE2(mssql_ptr, mssql_link *, mssql_link_index, id, "MS SQL-Link", le_link, le_plink);
	
	convert_to_string_ex(query);
	
	if (dbcmd(mssql_ptr->link, Z_STRVAL_PP(query))==FAIL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to set query");
		RETURN_FALSE;
	}
	if (dbsqlexec(mssql_ptr->link)==FAIL || (retvalue = dbresults(mssql_ptr->link))==FAIL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Query failed");
		RETURN_FALSE;
	}
	
	// Skip results not returning any columns
	while ((num_fields = dbnumcols(mssql_ptr->link)) <= 0 && retvalue == SUCCEED) {
		retvalue = dbresults(mssql_ptr->link);
	}
	if ((num_fields = dbnumcols(mssql_ptr->link)) <= 0) {
		RETURN_TRUE;
	}

	retvalue=dbnextrow(mssql_ptr->link);	
	if (retvalue==FAIL) {
		RETURN_FALSE;
	}

	result = (mssql_result *) emalloc(sizeof(mssql_result));
	result->statement = NULL;
	result->num_fields = num_fields;
	result->blocks_initialized = 1;
	
	result->batchsize = batchsize;
	result->data = NULL;
	result->blocks_initialized = 0;
	result->mssql_ptr = mssql_ptr;
	result->cur_field=result->cur_row=result->num_rows=0;

	if (num_fields > 0) {
		result->fields = (mssql_field *) emalloc(sizeof(mssql_field)*result->num_fields);
		result->num_rows = _mssql_fetch_batch(mssql_ptr, result, retvalue TSRMLS_CC);
	}
	else
		result->fields = NULL;
	
	ZEND_REGISTER_RESOURCE(return_value, result, le_result);
}
/* }}} */

/* {{{ proto int mssql_rows_affected(int conn_id)
   Returns the number of records affected by the query */
PHP_FUNCTION(mssql_rows_affected)
{
	zval **mssql_link_index;
	mssql_link *mssql_ptr;

	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &mssql_link_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE2(mssql_ptr, mssql_link *, mssql_link_index, -1, "MS SQL-Link", le_link, le_plink);
	RETURN_LONG(DBCOUNT(mssql_ptr->link));
}
/* }}} */


/* {{{ proto int mssql_free_result(resource result_index)
   Free a MS-SQL result index */
PHP_FUNCTION(mssql_free_result)
{
	zval **mssql_result_index;
	mssql_result *result;
	int retvalue;
	
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &mssql_result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	if (Z_TYPE_PP(mssql_result_index)==IS_RESOURCE && Z_LVAL_PP(mssql_result_index)==0) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(result, mssql_result *, mssql_result_index, -1, "MS SQL-result", le_result);	
	/* Release remaining results */
	do {
		dbcanquery(result->mssql_ptr->link);
		retvalue = dbresults(result->mssql_ptr->link);
	} while (retvalue == SUCCEED);

	zend_list_delete(Z_RESVAL_PP(mssql_result_index));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string mssql_get_last_message(void)
   Gets the last message from the MS-SQL server */
PHP_FUNCTION(mssql_get_last_message)
{
	if (MS_SQL_G(server_message)) {
		RETURN_STRING(MS_SQL_G(server_message),1);
	}
	else {
		RETURN_STRING(empty_string,1);
	}
}

/* }}} */

/* {{{ proto int mssql_num_rows(int mssql_result_index)
   Returns the number of rows fetched in from the result id specified */
PHP_FUNCTION(mssql_num_rows)
{
	zval **mssql_result_index;
	mssql_result *result;
	
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &mssql_result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(result, mssql_result *, mssql_result_index, -1, "MS SQL-result", le_result);	
	
	Z_LVAL_P(return_value) = result->num_rows;
	Z_TYPE_P(return_value) = IS_LONG;
}

/* }}} */

/* {{{ proto int mssql_num_fields(int mssql_result_index)
   Returns the number of fields fetched in from the result id specified */
PHP_FUNCTION(mssql_num_fields)
{
	zval **mssql_result_index;
	mssql_result *result;
	
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &mssql_result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(result, mssql_result *, mssql_result_index, -1, "MS SQL-result", le_result);	

	Z_LVAL_P(return_value) = result->num_fields;
	Z_TYPE_P(return_value) = IS_LONG;
}

/* }}} */

static void php_mssql_fetch_hash(INTERNAL_FUNCTION_PARAMETERS, int result_type)
{
	zval **mssql_result_index, **resulttype = NULL;
	mssql_result *result;
	int i;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &mssql_result_index)==FAILURE) {
				RETURN_FALSE;
			}
			if (!result_type) {
				result_type = MSSQL_BOTH;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &mssql_result_index, &resulttype)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long_ex(resulttype);
			result_type = Z_LVAL_PP(resulttype);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	ZEND_FETCH_RESOURCE(result, mssql_result *, mssql_result_index, -1, "MS SQL-result", le_result);	

	if (MS_SQL_G(server_message)) {
		STR_FREE(MS_SQL_G(server_message));
		MS_SQL_G(server_message) = NULL;
	}

	if (result->cur_row >= result->num_rows) {
		RETURN_FALSE;
	}
	
	array_init(return_value);
	
	for (i=0; i<result->num_fields; i++) {
		if (Z_TYPE(result->data[result->cur_row][i]) != IS_NULL) {
			char *data;
			int data_len;
			int should_copy;

			if (Z_TYPE(result->data[result->cur_row][i]) == IS_STRING) {
				if (PG(magic_quotes_runtime)) {
					data = php_addslashes(Z_STRVAL(result->data[result->cur_row][i]), Z_STRLEN(result->data[result->cur_row][i]), &data_len, 0 TSRMLS_CC);
					should_copy = 0;
				}
				else
				{
					data = Z_STRVAL(result->data[result->cur_row][i]);
					data_len = Z_STRLEN(result->data[result->cur_row][i]);
					should_copy = 1;
				}

				if (result_type & MSSQL_NUM) {
					add_index_stringl(return_value, i, data, data_len, should_copy);
					should_copy = 1;
				}
				
				if (result_type & MSSQL_ASSOC) {
					add_assoc_stringl(return_value, result->fields[i].name, data, data_len, should_copy);
				}
			}
			else if (Z_TYPE(result->data[result->cur_row][i]) == IS_LONG) {
				if (result_type & MSSQL_NUM)
					add_index_long(return_value, i, Z_LVAL(result->data[result->cur_row][i]));
				
				if (result_type & MSSQL_ASSOC)
					add_assoc_long(return_value, result->fields[i].name, Z_LVAL(result->data[result->cur_row][i]));
			}
			else if (Z_TYPE(result->data[result->cur_row][i]) == IS_DOUBLE) {
				if (result_type & MSSQL_NUM)
					add_index_double(return_value, i, Z_DVAL(result->data[result->cur_row][i]));
				
				if (result_type & MSSQL_ASSOC)
					add_assoc_double(return_value, result->fields[i].name, Z_DVAL(result->data[result->cur_row][i]));
			}
		}
		else
		{
			if (result_type & MSSQL_NUM)
				add_index_null(return_value, i);
			if (result_type & MSSQL_ASSOC)
				add_assoc_null(return_value, result->fields[i].name);
		}
	}
	result->cur_row++;
}

/* {{{ proto array mssql_fetch_row(int result_id [, int result_type])
   Returns an array of the current row in the result set specified by result_id */
PHP_FUNCTION(mssql_fetch_row)
{
	php_mssql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, MSSQL_NUM);
}

/* }}} */

/* {{{ proto object mssql_fetch_object(int result_id [, int result_type])
   Returns a psuedo-object of the current row in the result set specified by result_id */
PHP_FUNCTION(mssql_fetch_object)
{
	php_mssql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, MSSQL_ASSOC);
	if (Z_TYPE_P(return_value)==IS_ARRAY) {
		object_and_properties_init(return_value, ZEND_STANDARD_CLASS_DEF_PTR, Z_ARRVAL_P(return_value));
	}
}

/* }}} */

/* {{{ proto array mssql_fetch_array(int result_id [, int result_type])
   Returns an associative array of the current row in the result set specified by result_id */
PHP_FUNCTION(mssql_fetch_array)
{
	php_mssql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, MSSQL_BOTH);
}

/* }}} */

/* {{{ proto array mssql_fetch_assoc(int result_id [, int result_type])
   Returns an associative array of the current row in the result set specified by result_id */
PHP_FUNCTION(mssql_fetch_assoc)
{
	php_mssql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, MSSQL_ASSOC);
}

/* }}} */

/* {{{ proto int mssql_data_seek(int result_id, int offset)
   Moves the internal row pointer of the MS-SQL result associated with the specified result identifier to pointer to the specified row number */
PHP_FUNCTION(mssql_data_seek)
{
	zval **mssql_result_index, **offset;
	mssql_result *result;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &mssql_result_index, &offset)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(result, mssql_result *, mssql_result_index, -1, "MS SQL-result", le_result);	

	convert_to_long_ex(offset);
	if (Z_LVAL_PP(offset)<0 || Z_LVAL_PP(offset)>=result->num_rows) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Bad row offset");
		RETURN_FALSE;
	}
	
	result->cur_row = Z_LVAL_PP(offset);
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
		case SQLFLT4:
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
	int field_offset;
	mssql_result *result;

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
			field_offset = Z_LVAL_PP(offset);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE(result, mssql_result *, mssql_result_index, -1, "MS SQL-result", le_result);	
	
	if (field_offset==-1) {
		field_offset = result->cur_field;
		result->cur_field++;
	}
	
	if (field_offset<0 || field_offset >= result->num_fields) {
		if (ZEND_NUM_ARGS()==2) { /* field specified explicitly */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Bad column offset");
		}
		RETURN_FALSE;
	}

	object_init(return_value);

	add_property_string(return_value, "name",result->fields[field_offset].name, 1);
	add_property_long(return_value, "max_length",result->fields[field_offset].max_length);
	add_property_string(return_value, "column_source",result->fields[field_offset].column_source, 1);
	add_property_long(return_value, "numeric", result->fields[field_offset].numeric);
	add_property_string(return_value, "type", php_mssql_get_field_name(Z_TYPE(result->fields[field_offset])), 1);
}

/* }}} */

/* {{{ proto int mssql_field_length(int result_id [, int offset])
   Get the length of a MS-SQL field */
PHP_FUNCTION(mssql_field_length)
{
	zval **mssql_result_index, **offset;
	int field_offset;
	mssql_result *result;

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
			field_offset = Z_LVAL_PP(offset);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE(result, mssql_result *, mssql_result_index, -1, "MS SQL-result", le_result);	
	
	if (field_offset==-1) {
		field_offset = result->cur_field;
		result->cur_field++;
	}
	
	if (field_offset<0 || field_offset >= result->num_fields) {
		if (ZEND_NUM_ARGS()==2) { /* field specified explicitly */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Bad column offset");
		}
		RETURN_FALSE;
	}

	Z_LVAL_P(return_value) = result->fields[field_offset].max_length;
	Z_TYPE_P(return_value) = IS_LONG;
}

/* }}} */

/* {{{ proto string mssql_field_name(int result_id [, int offset])
   Returns the name of the field given by offset in the result set given by result_id */
PHP_FUNCTION(mssql_field_name)
{
	zval **mssql_result_index, **offset;
	int field_offset;
	mssql_result *result;

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
			field_offset = Z_LVAL_PP(offset);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE(result, mssql_result *, mssql_result_index, -1, "MS SQL-result", le_result);	
	
	if (field_offset==-1) {
		field_offset = result->cur_field;
		result->cur_field++;
	}
	
	if (field_offset<0 || field_offset >= result->num_fields) {
		if (ZEND_NUM_ARGS()==2) { /* field specified explicitly */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Bad column offset");
		}
		RETURN_FALSE;
	}

	Z_STRVAL_P(return_value) = estrdup(result->fields[field_offset].name);
	Z_STRLEN_P(return_value) = strlen(result->fields[field_offset].name);
	Z_TYPE_P(return_value) = IS_STRING;
}

/* }}} */

/* {{{ proto string mssql_field_type(int result_id [, int offset])
   Returns the type of a field */
PHP_FUNCTION(mssql_field_type)
{
	zval **mssql_result_index, **offset;
	int field_offset;
	mssql_result *result;

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
			field_offset = Z_LVAL_PP(offset);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE(result, mssql_result *, mssql_result_index, -1, "MS SQL-result", le_result);	
	
	if (field_offset==-1) {
		field_offset = result->cur_field;
		result->cur_field++;
	}
	
	if (field_offset<0 || field_offset >= result->num_fields) {
		if (ZEND_NUM_ARGS()==2) { /* field specified explicitly */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Bad column offset");
		}
		RETURN_FALSE;
	}

	Z_STRVAL_P(return_value) = estrdup(php_mssql_get_field_name(Z_TYPE(result->fields[field_offset])));
	Z_STRLEN_P(return_value) = strlen(php_mssql_get_field_name(Z_TYPE(result->fields[field_offset])));
	Z_TYPE_P(return_value) = IS_STRING;
}

/* }}} */

/* {{{ proto bool mssql_field_seek(int result_id, int offset)
   Seeks to the specified field offset */
PHP_FUNCTION(mssql_field_seek)
{
	zval **mssql_result_index, **offset;
	int field_offset;
	mssql_result *result;

	if (ZEND_NUM_ARGS()!=2 || zend_get_parameters_ex(2, &mssql_result_index, &offset)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(result, mssql_result *, mssql_result_index, -1, "MS SQL-result", le_result);	
	
	convert_to_long_ex(offset);
	field_offset = Z_LVAL_PP(offset);
	
	if (field_offset<0 || field_offset >= result->num_fields) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Bad column offset");
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
	int field_offset=0;
	mssql_result *result;

	if (ZEND_NUM_ARGS()!=3 || zend_get_parameters_ex(3, &mssql_result_index, &row, &field)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(result, mssql_result *, mssql_result_index, -1, "MS SQL-result", le_result);	
	
	convert_to_long_ex(row);
	if (Z_LVAL_PP(row) < 0 || Z_LVAL_PP(row) >= result->num_rows) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Bad row offset (%d)", Z_LVAL_PP(row));
		RETURN_FALSE;
	}

	switch(Z_TYPE_PP(field)) {
		case IS_STRING: {
			int i;

			for (i=0; i<result->num_fields; i++) {
				if (!strcasecmp(result->fields[i].name, Z_STRVAL_PP(field))) {
					field_offset = i;
					break;
				}
			}
			if (i>=result->num_fields) { /* no match found */
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s field not found in result", Z_STRVAL_PP(field));
				RETURN_FALSE;
			}
			break;
		}
		default:
			convert_to_long_ex(field);
			field_offset = Z_LVAL_PP(field);
			if (field_offset<0 || field_offset>=result->num_fields) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Bad column offset specified");
				RETURN_FALSE;
			}
			break;
	}

	*return_value = result->data[Z_LVAL_PP(row)][field_offset];
	ZVAL_COPY_CTOR(return_value);
}
/* }}} */

/* {{{ proto bool mssql_next_result(int result_id)
   Move the internal result pointer to the next result */
PHP_FUNCTION(mssql_next_result)
{
	zval **mssql_result_index;
	int retvalue;
	mssql_result *result;
	mssql_link *mssql_ptr;

	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &mssql_result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(result, mssql_result *, mssql_result_index, -1, "MS SQL-result", le_result);	

	mssql_ptr = result->mssql_ptr;
	retvalue = dbresults(mssql_ptr->link);
	if (retvalue == FAIL) {
		RETURN_FALSE;
	}
	else if (retvalue == NO_MORE_RESULTS || retvalue == NO_MORE_RPC_RESULTS) {
		if (result->statement) {
			_mssql_get_sp_result(result->mssql_ptr, result->statement TSRMLS_CC);
		}
		RETURN_FALSE;
	}
	else {
		_free_result(result, 1);
		result->cur_row=result->num_fields=result->num_rows=0;
		dbclrbuf(mssql_ptr->link,DBLASTROW(mssql_ptr->link));
		retvalue = dbnextrow(mssql_ptr->link);

		result->num_fields = dbnumcols(mssql_ptr->link);
		result->fields = (mssql_field *) emalloc(sizeof(mssql_field)*result->num_fields);
		result->num_rows = _mssql_fetch_batch(mssql_ptr, result, retvalue TSRMLS_CC);
		RETURN_TRUE;
	}

}
/* }}} */


/* {{{ proto void mssql_min_error_severity(int severity)
   Sets the lower error severity */
PHP_FUNCTION(mssql_min_error_severity)
{
	zval **severity;
	
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &severity)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(severity);
	MS_SQL_G(min_error_severity) = Z_LVAL_PP(severity);
}

/* }}} */

/* {{{ proto void mssql_min_message_severity(int severity)
   Sets the lower message severity */
PHP_FUNCTION(mssql_min_message_severity)
{
	zval **severity;
	
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &severity)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(severity);
	MS_SQL_G(min_message_severity) = Z_LVAL_PP(severity);
}
/* }}} */

/* {{{ proto int mssql_init(string sp_name [, int conn_id])
   Initializes a stored procedure or a remote stored procedure  */
PHP_FUNCTION(mssql_init)
{
	zval **sp_name, **mssql_link_index;
	mssql_link *mssql_ptr;
	mssql_statement *statement;
	int id;
	
	switch(ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &sp_name)==FAILURE) {
				RETURN_FALSE;
			}
			id = php_mssql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			break;

		case 2:
			if (zend_get_parameters_ex(2, &sp_name, &mssql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;

		default:
			WRONG_PARAM_COUNT;
			break;
	}

	ZEND_FETCH_RESOURCE2(mssql_ptr, mssql_link *, mssql_link_index, id, "MS SQL-Link", le_link, le_plink);
	
	convert_to_string_ex(sp_name);
	
	if (dbrpcinit(mssql_ptr->link, Z_STRVAL_PP(sp_name),0)==FAIL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to init stored procedure");
		RETURN_FALSE;
	}

	statement=NULL;
	statement = ecalloc(1,sizeof(mssql_statement));
	statement->link = mssql_ptr;
	statement->executed=FALSE;

	statement->id = zend_list_insert(statement,le_statement);
	
	RETURN_RESOURCE(statement->id);
}
/* }}} */

/* {{{ proto int mssql_bind(int stmt, string param_name, mixed var, int type 
		[, int is_output[, int is_null[, int maxlen]]])
   Adds a parameter to a stored procedure or a remote stored procedure  */
PHP_FUNCTION(mssql_bind)
{
	int	type, is_output, is_null, datalen, maxlen = -1;
	zval **stmt, **param_name, **var, **yytype;
	mssql_link *mssql_ptr;
	mssql_statement *statement;
	mssql_bind bind,*bindp;
	int id = 0, status = 0;
	LPBYTE value = NULL;

	/* BEGIN input validation */
	switch(ZEND_NUM_ARGS()) {
		case 4: 
			if (zend_get_parameters_ex(4, &stmt, &param_name, &var, &yytype)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long_ex(yytype);
			type=Z_LVAL_PP(yytype);
			is_null=FALSE;
			is_output=FALSE;
			break;
						
		case 5: {
			zval **yyis_output;

				if (zend_get_parameters_ex(5, &stmt, &param_name, &var, &yytype, &yyis_output)==FAILURE) {
					RETURN_FALSE;
				}
				convert_to_long_ex(yytype);
				convert_to_long_ex(yyis_output);
				type=Z_LVAL_PP(yytype);
				is_null=FALSE;
				is_output=Z_LVAL_PP(yyis_output);
			}
			break;	

		case 6: {
				zval **yyis_output, **yyis_null;

				if (zend_get_parameters_ex(6, &stmt, &param_name, &var, &yytype, &yyis_output, &yyis_null)==FAILURE) {
					RETURN_FALSE;
				}
				convert_to_long_ex(yytype);
				convert_to_long_ex(yyis_output);
				convert_to_long_ex(yyis_null);
				type=Z_LVAL_PP(yytype);
				is_output=Z_LVAL_PP(yyis_output);
				is_null=Z_LVAL_PP(yyis_null);
			}
			break;
		
		case 7: {
				zval **yyis_output, **yyis_null, **yymaxlen;

				if (zend_get_parameters_ex(7, &stmt, &param_name, &var, &yytype, &yyis_output, &yyis_null, &yymaxlen)==FAILURE) {
					RETURN_FALSE;
				}
				convert_to_long_ex(yytype);
				convert_to_long_ex(yyis_output);
				convert_to_long_ex(yyis_null);
				convert_to_long_ex(yymaxlen);
				type=Z_LVAL_PP(yytype);
				is_output=Z_LVAL_PP(yyis_output);
				is_null=Z_LVAL_PP(yyis_null);
				maxlen=Z_LVAL_PP(yymaxlen);				
			}
			break;	
		
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	/* END input validation */
	
	ZEND_FETCH_RESOURCE(statement, mssql_statement *, stmt, -1, "MS SQL-Statement", le_statement);
	if (statement==NULL) {
		RETURN_FALSE;
	}
	mssql_ptr=statement->link;

	/* modify datalen and maxlen according to dbrpcparam documentation */
	if ( (type==SQLVARCHAR) || (type==SQLCHAR) || (type==SQLTEXT) )	{	/* variable-length type */
		if (is_null) {
			maxlen=0;
			datalen=0;
		}
		else {
			convert_to_string_ex(var);
			datalen=Z_STRLEN_PP(var);
			value=(LPBYTE)Z_STRVAL_PP(var);
		}
	}
	else	{	/* fixed-length type */
		if (is_null)	{
			datalen=0;
		}
		else {
			datalen=-1;
		}
		maxlen=-1;

		switch (type)	{

			case SQLFLT4:
			case SQLFLT8:
			case SQLFLTN:
				convert_to_double_ex(var);
				value=(LPBYTE)(&Z_DVAL_PP(var));
				break;

			case SQLINT1:
			case SQLINT2:
			case SQLINT4:
				convert_to_long_ex(var);
				value=(LPBYTE)(&Z_LVAL_PP(var));
				break;

			default:
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "unsupported type");
				RETURN_FALSE;
				break;
		}
	}

	convert_to_string_ex(param_name);
	
	if (is_output) {
		status=DBRPCRETURN;
	}
	
	/* hashtable of binds */
	if (! statement->binds) {
		ALLOC_HASHTABLE(statement->binds);
		zend_hash_init(statement->binds, 13, NULL, _mssql_bind_hash_dtor, 0);
	}

	memset((void*)&bind,0,sizeof(mssql_bind));
	zend_hash_add(statement->binds,Z_STRVAL_PP(param_name),Z_STRLEN_PP(param_name),&bind,sizeof(mssql_bind),(void **)&bindp);
	bindp->zval=*var;
	zval_add_ref(var);

	/* no call to dbrpcparam if RETVAL */
	if ( strcmp("RETVAL",Z_STRVAL_PP(param_name))!=0 ) {						
		if (dbrpcparam(mssql_ptr->link, Z_STRVAL_PP(param_name), (BYTE)status, type, maxlen, datalen, (LPBYTE)value)==FAIL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to set parameter");
			RETURN_FALSE;
		}
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int mssql_execute(int stmt [, bool skip_results = false])
   Executes a stored procedure on a MS-SQL server database */
PHP_FUNCTION(mssql_execute)
{
	zval **stmt, **skip;
	zend_bool skip_results = 0;
	int retvalue, retval_results;
	mssql_link *mssql_ptr;
	mssql_statement *statement;
	mssql_result *result;
	int num_fields;
	int blocks_initialized=1;
	int batchsize;
	int ac = ZEND_NUM_ARGS();

	batchsize = MS_SQL_G(batchsize);
	if (ac < 1 || ac > 2 || zend_get_parameters_ex(ac, &stmt, &skip)==FAILURE) {
        WRONG_PARAM_COUNT;
    }
	if (ac == 2) {
		skip_results = Z_BVAL_PP(skip);
	}

	ZEND_FETCH_RESOURCE(statement, mssql_statement *, stmt, -1, "MS SQL-Statement", le_statement);

	mssql_ptr=statement->link;

	if (dbrpcexec(mssql_ptr->link)==FAIL || dbsqlok(mssql_ptr->link)==FAIL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "stored procedure execution failed");
		RETURN_FALSE;
	}

	retval_results=dbresults(mssql_ptr->link);

	if (retval_results==FAIL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "could not retrieve results");
		RETURN_FALSE;
	}

	/* The following is just like mssql_query, fetch all rows from the first result 
	 *	set into the row buffer. 
 	 */
	result=NULL;
	if (retval_results == SUCCEED) {
		if (skip_results) {
			do {
				dbcanquery(mssql_ptr->link);
				retval_results = dbresults(mssql_ptr->link);
			} while (retval_results == SUCCEED);

			_mssql_get_sp_result(mssql_ptr, statement TSRMLS_CC);
		}
		else {
			// Skip results not returning any columns
			while ((num_fields = dbnumcols(mssql_ptr->link)) <= 0 && retval_results == SUCCEED) {
				retval_results = dbresults(mssql_ptr->link);
			}
			if ((num_fields = dbnumcols(mssql_ptr->link)) > 0) {
				retvalue = dbnextrow(mssql_ptr->link);
				result = (mssql_result *) emalloc(sizeof(mssql_result));
				result->batchsize = batchsize;
				result->blocks_initialized = 1;
				result->data = (zval **) emalloc(sizeof(zval *)*MSSQL_ROWS_BLOCK);
				result->mssql_ptr = mssql_ptr;
				result->cur_field=result->cur_row=result->num_rows=0;
				result->num_fields = num_fields;

				result->fields = (mssql_field *) emalloc(sizeof(mssql_field)*num_fields);
				result->num_rows = _mssql_fetch_batch(mssql_ptr, result, retvalue TSRMLS_CC);
				result->statement = statement;
			}
			else {
				_mssql_get_sp_result(mssql_ptr, statement TSRMLS_CC);
			}
		}
	}
	else if (retval_results == NO_MORE_RESULTS || retval_results == NO_MORE_RPC_RESULTS) {
		_mssql_get_sp_result(mssql_ptr, statement TSRMLS_CC);
	}
	
	if (result==NULL) {
		RETURN_TRUE;	/* no recordset returned ...*/
	}
	else {
		ZEND_REGISTER_RESOURCE(return_value, result, le_result);
	}
}
/* }}} */

/* {{{ proto int mssql_free_statement(resource result_index)
   Free a MS-SQL statement index */
PHP_FUNCTION(mssql_free_statement)
{
	zval **mssql_statement_index;
	mssql_statement *statement;
	int retvalue;
	
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &mssql_statement_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	if (Z_TYPE_PP(mssql_statement_index)==IS_RESOURCE && Z_LVAL_PP(mssql_statement_index)==0) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(statement, mssql_statement *, mssql_statement_index, -1, "MS SQL-statement", le_statement);	
	/* Release remaining results */
	do {
		dbcanquery(statement->link->link);
		retvalue = dbresults(statement->link->link);
	} while (retvalue == SUCCEED);

	zend_list_delete(Z_RESVAL_PP(mssql_statement_index));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string mssql_guid_string(string binary [,int short_format])
   Converts a 16 byte binary GUID to a string  */
PHP_FUNCTION(mssql_guid_string)
{
	zval **binary, **short_format;
	int sf = 0;
	char buffer[32+1];
	char buffer2[36+1];
	
	switch(ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &binary)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_string_ex(binary);
			break;
		case 2:
			if (zend_get_parameters_ex(2, &binary, &short_format)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_string_ex(binary);
			convert_to_long_ex(short_format);
			sf = Z_LVAL_PP(short_format);
			break;

		default:
			WRONG_PARAM_COUNT;
			break;
	}

	dbconvert(NULL, SQLBINARY, (BYTE*)Z_STRVAL_PP(binary), 16, SQLCHAR, buffer, -1);

	if (sf) {
		php_strtoupper(buffer, 32);
		RETURN_STRING(buffer, 1);
	}
	else {
		int i;
		for (i=0; i<4; i++) {
			buffer2[2*i] = buffer[6-2*i];
			buffer2[2*i+1] = buffer[7-2*i];
		}
		buffer2[8] = '-';
		for (i=0; i<2; i++) {
			buffer2[9+2*i] = buffer[10-2*i];
			buffer2[10+2*i] = buffer[11-2*i];
		}
		buffer2[13] = '-';
		for (i=0; i<2; i++) {
			buffer2[14+2*i] = buffer[14-2*i];
			buffer2[15+2*i] = buffer[15-2*i];
		}
		buffer2[18] = '-';
		for (i=0; i<4; i++) {
			buffer2[19+i] = buffer[16+i];
		}
		buffer2[23] = '-';
		for (i=0; i<12; i++) {
			buffer2[24+i] = buffer[20+i];
		}
		buffer2[36] = 0;

		php_strtoupper(buffer2, 36);
		RETURN_STRING(buffer2, 1);
	}
}
/* }}} */

#endif
