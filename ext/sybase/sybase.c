/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999 The PHP Group                         |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
   | php3_sybase_get_column_content_with_type() based on code by:         |
   |                     Muhammad A Muquit <MA_Muquit@fccc.edu>           |
   |                     Rasmus Lerdorf <rasmus@lerdorf.on.ca>            |
   +----------------------------------------------------------------------+
 */
 
/* $Id$ */


#include "php.h"
#include "php3_sybase.h"
#include "ext/standard/php3_standard.h"
#include "php_globals.h"

#if HAVE_SYBASE

#include <sybfront.h>
#include <sybdb.h>
#include <syberror.h>

#if BROKEN_SYBASE_PCONNECTS
#include "http_log.h"
#endif

function_entry sybase_functions[] = {
	{"sybase_connect",			php3_sybase_connect,			NULL},
	{"sybase_pconnect",			php3_sybase_pconnect,			NULL},
	{"sybase_close",			php3_sybase_close,				NULL},
	{"sybase_select_db",		php3_sybase_select_db,			NULL},
	{"sybase_query",			php3_sybase_query,				NULL},
	{"sybase_free_result",		php3_sybase_free_result,		NULL},
	{"sybase_get_last_message",	php3_sybase_get_last_message,	NULL},
	{"sybase_num_rows",			php3_sybase_num_rows,			NULL},
	{"sybase_num_fields",		php3_sybase_num_fields,			NULL},
	{"sybase_fetch_row",		php3_sybase_fetch_row,			NULL},
	{"sybase_fetch_array",		php3_sybase_fetch_array,		NULL},
	{"sybase_fetch_object",		php3_sybase_fetch_object,		NULL},
	{"sybase_data_seek",		php3_sybase_data_seek,			NULL},
	{"sybase_fetch_field",		php3_sybase_fetch_field,		NULL},
	{"sybase_field_seek",		php3_sybase_field_seek,			NULL},
	{"sybase_result",			php3_sybase_result,				NULL},
	{"sybase_min_error_severity",	php3_sybase_min_error_severity,		NULL},
	{"sybase_min_message_severity",	php3_sybase_min_message_severity,	NULL},
	{"mssql_connect",			php3_sybase_connect,			NULL},
	{"mssql_pconnect",			php3_sybase_pconnect,			NULL},
	{"mssql_close",				php3_sybase_close,				NULL},
	{"mssql_select_db",			php3_sybase_select_db,			NULL},
	{"mssql_query",				php3_sybase_query,				NULL},
	{"mssql_free_result",		php3_sybase_free_result,		NULL},
	{"mssql_get_last_message",	php3_sybase_get_last_message,	NULL},
	{"mssql_num_rows",			php3_sybase_num_rows,			NULL},
	{"mssql_num_fields",		php3_sybase_num_fields,			NULL},
	{"mssql_fetch_row",			php3_sybase_fetch_row,			NULL},
	{"mssql_fetch_array",		php3_sybase_fetch_array,		NULL},
	{"mssql_fetch_object",		php3_sybase_fetch_object,		NULL},
	{"mssql_data_seek",			php3_sybase_data_seek,			NULL},
	{"mssql_fetch_field",		php3_sybase_fetch_field,		NULL},
	{"mssql_field_seek",		php3_sybase_field_seek,			NULL},
	{"mssql_result",			php3_sybase_result,				NULL},
	{"mssql_min_error_severity",	php3_sybase_min_error_severity,		NULL},
	{"mssql_min_message_severity",	php3_sybase_min_message_severity,	NULL},
	{NULL, NULL, NULL}
};

php3_module_entry sybase_module_entry = {
	"Sybase SQL", sybase_functions, php3_minit_sybase, php3_mshutdown_sybase, php3_rinit_sybase, php3_rshutdown_sybase, php3_info_sybase, STANDARD_MODULE_PROPERTIES
};

#if COMPILE_DL
php3_module_entry *get_module() { return &sybase_module_entry; }
#endif

THREAD_LS sybase_module php3_sybase_module;
THREAD_LS static HashTable *resource_list, *resource_plist;


#define CHECK_LINK(link) { if (link==-1) { php_error(E_WARNING,"Sybase:  A link to the server could not be established"); RETURN_FALSE; } }


static void php3_sybase_get_column_content(sybase_link *sybase_ptr,int offset,pval *result, int column_type);

/* error handler */
static int php3_sybase_error_handler(DBPROCESS *dbproc,int severity,int dberr,
										int oserr,char *dberrstr,char *oserrstr)
{
	if (severity >= php3_sybase_module.min_error_severity) {
		php_error(E_WARNING,"Sybase error:  %s (severity %d)",dberrstr,severity);
	}
	return INT_CANCEL;  
}

/* message handler */
static int php3_sybase_message_handler(DBPROCESS *dbproc,DBINT msgno,int msgstate,
										int severity,char *msgtext,char *srvname,
										char *procname,DBUSMALLINT line)
{
	if (severity >= php3_sybase_module.min_message_severity) {
		php_error(E_WARNING,"Sybase message:  %s (severity %d)",msgtext,severity);
	}
	STR_FREE(php3_sybase_module.server_message);
	php3_sybase_module.server_message = estrdup(msgtext);
	return 0;
}


static int _clean_invalid_results(list_entry *le)
{
	if (le->type == php3_sybase_module.le_result) {
		sybase_link *sybase_ptr = ((sybase_result *) le->ptr)->sybase_ptr;
		
		if (!sybase_ptr->valid) {
			return 1;
		}
	}
	return 0;
}


static void _free_sybase_result(sybase_result *result)
{
	int i,j;
	
	if (result->data) {
		for (i=0; i<result->num_rows; i++) {
			for (j=0; j<result->num_fields; j++) {
				pval_destructor(&result->data[i][j]);
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


static void _close_sybase_link(sybase_link *sybase_ptr)
{
	sybase_ptr->valid = 0;
	zend_hash_apply(resource_list,(int (*)(void *))_clean_invalid_results);
	dbclose(sybase_ptr->link);
	dbloginfree(sybase_ptr->login);
	efree(sybase_ptr);
	php3_sybase_module.num_links--;
}


static void _close_sybase_plink(sybase_link *sybase_ptr)
{
	dbclose(sybase_ptr->link);
	dbloginfree(sybase_ptr->login);
	free(sybase_ptr);
	php3_sybase_module.num_persistent--;
	php3_sybase_module.num_links--;
}


int php3_minit_sybase(INIT_FUNC_ARGS)
{
	char *interface_file;

	if (dbinit()==FAIL) {
		return FAILURE;
	}
	dberrhandle((EHANDLEFUNC) php3_sybase_error_handler);
	dbmsghandle((MHANDLEFUNC) php3_sybase_message_handler);

	if (cfg_get_string("sybase.interface_file",&interface_file)==SUCCESS) {
		dbsetifile(interface_file);
	}
	if (cfg_get_long("sybase.allow_persistent",&php3_sybase_module.allow_persistent)==FAILURE) {
		php3_sybase_module.allow_persistent=1;
	}
	if (cfg_get_long("sybase.max_persistent",&php3_sybase_module.max_persistent)==FAILURE) {
		php3_sybase_module.max_persistent=-1;
	}
	if (cfg_get_long("sybase.max_links",&php3_sybase_module.max_links)==FAILURE) {
		php3_sybase_module.max_links=-1;
	}
	if (cfg_get_long("sybase.min_error_severity",&php3_sybase_module.cfg_min_error_severity)==FAILURE) {
		php3_sybase_module.cfg_min_error_severity=10;
	}
	if (cfg_get_long("sybase.min_message_severity",&php3_sybase_module.cfg_min_message_severity)==FAILURE) {
		php3_sybase_module.cfg_min_message_severity=10;
	}
	if (cfg_get_long("sybase.compatability_mode",&php3_sybase_module.compatability_mode)==FAILURE) {
		php3_sybase_module.compatability_mode = 0;
	}
	
	php3_sybase_module.num_persistent=0;
	php3_sybase_module.le_link = register_list_destructors(_close_sybase_link,NULL);
	php3_sybase_module.le_plink = register_list_destructors(NULL,_close_sybase_plink);
	php3_sybase_module.le_result = register_list_destructors(_free_sybase_result,NULL);
	
	return SUCCESS;
}


int php3_rinit_sybase(INIT_FUNC_ARGS)
{
	php3_sybase_module.default_link=-1;
	php3_sybase_module.num_links = php3_sybase_module.num_persistent;
	php3_sybase_module.appname = estrndup("PHP 3.0",7);
	php3_sybase_module.server_message = empty_string;
	php3_sybase_module.min_error_severity = php3_sybase_module.cfg_min_error_severity;
	php3_sybase_module.min_message_severity = php3_sybase_module.cfg_min_message_severity;
	return SUCCESS;
}

int php3_mshutdown_sybase(SHUTDOWN_FUNC_ARGS)
{
	dbexit();
	return SUCCESS;
}

int php3_rshutdown_sybase(SHUTDOWN_FUNC_ARGS)
{
	efree(php3_sybase_module.appname);
	STR_FREE(php3_sybase_module.server_message);
	return SUCCESS;
}

static void php3_sybase_do_connect(INTERNAL_FUNCTION_PARAMETERS,int persistent)
{
	char *user,*passwd,*host;
	char *hashed_details;
	int hashed_details_length;
	sybase_link sybase,*sybase_ptr;

	resource_list = list;
	resource_plist = plist;
		
	switch(ARG_COUNT(ht)) {
		case 0: /* defaults */
			host=user=passwd=NULL;
			hashed_details_length=6+3;
			hashed_details = (char *) emalloc(hashed_details_length+1);
			strcpy(hashed_details,"sybase___");
			break;
		case 1: {
				pval *yyhost;
				
				if (getParameters(ht, 1, &yyhost)==FAILURE) {
					RETURN_FALSE;
				}
				convert_to_string(yyhost);
				host = yyhost->value.str.val;
				user=passwd=NULL;
				hashed_details_length = yyhost->value.str.len+6+3;
				hashed_details = (char *) emalloc(hashed_details_length+1);
				sprintf(hashed_details,"sybase_%s__",yyhost->value.str.val);
			}
			break;
		case 2: {
				pval *yyhost,*yyuser;
				
				if (getParameters(ht, 2, &yyhost, &yyuser)==FAILURE) {
					RETURN_FALSE;
				}
				convert_to_string(yyhost);
				convert_to_string(yyuser);
				host = yyhost->value.str.val;
				user = yyuser->value.str.val;
				passwd=NULL;
				hashed_details_length = yyhost->value.str.len+yyuser->value.str.len+6+3;
				hashed_details = (char *) emalloc(hashed_details_length+1);
				sprintf(hashed_details,"sybase_%s_%s_",yyhost->value.str.val,yyuser->value.str.val);
			}
			break;
		case 3: {
				pval *yyhost,*yyuser,*yypasswd;
			
				if (getParameters(ht, 3, &yyhost, &yyuser, &yypasswd) == FAILURE) {
					RETURN_FALSE;
				}
				convert_to_string(yyhost);
				convert_to_string(yyuser);
				convert_to_string(yypasswd);
				host = yyhost->value.str.val;
				user = yyuser->value.str.val;
				passwd = yypasswd->value.str.val;
				hashed_details_length = yyhost->value.str.len+yyuser->value.str.len+yypasswd->value.str.len+6+3;
				hashed_details = (char *) emalloc(hashed_details_length+1);
				sprintf(hashed_details,"sybase_%s_%s_%s",yyhost->value.str.val,yyuser->value.str.val,yypasswd->value.str.val); /* SAFE */
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}


	/* set a DBLOGIN record */	
	if ((sybase.login=dblogin())==NULL) {
		php_error(E_WARNING,"Sybase:  Unable to allocate login record");
		RETURN_FALSE;
	}
	
	if (user) {
		DBSETLUSER(sybase.login,user);
	}
	if (passwd) {
		DBSETLPWD(sybase.login,passwd);
	}
	DBSETLAPP(sybase.login,php3_sybase_module.appname);
	sybase.valid = 1;

	if (!php3_sybase_module.allow_persistent) {
		persistent=0;
	}
	if (persistent) {
		list_entry *le;

		/* try to find if we already have this link in our persistent list */
		if (zend_hash_find(plist, hashed_details, hashed_details_length+1, (void **) &le)==FAILURE) {  /* we don't */
			list_entry new_le;

			if (php3_sybase_module.max_links!=-1 && php3_sybase_module.num_links>=php3_sybase_module.max_links) {
				php_error(E_WARNING,"Sybase:  Too many open links (%d)",php3_sybase_module.num_links);
				efree(hashed_details);
				dbloginfree(sybase.login);
				RETURN_FALSE;
			}
			if (php3_sybase_module.max_persistent!=-1 && php3_sybase_module.num_persistent>=php3_sybase_module.max_persistent) {
				php_error(E_WARNING,"Sybase:  Too many open persistent links (%d)",php3_sybase_module.num_persistent);
				efree(hashed_details);
				dbloginfree(sybase.login);
				RETURN_FALSE;
			}
			/* create the link */
			if ((sybase.link=dbopen(sybase.login,host))==FAIL) {
				/*php_error(E_WARNING,"Sybase:  Unable to connect to server:  %s",sybase_error(sybase));*/
				efree(hashed_details);
				dbloginfree(sybase.login);
				RETURN_FALSE;
			}

			if (dbsetopt(sybase.link,DBBUFFER,"2",-1)==FAIL) {
				efree(hashed_details);
				dbloginfree(sybase.login);
				dbclose(sybase.link);
				RETURN_FALSE;
			}

			/* hash it up */
			sybase_ptr = (sybase_link *) malloc(sizeof(sybase_link));
			memcpy(sybase_ptr,&sybase,sizeof(sybase_link));
			new_le.type = php3_sybase_module.le_plink;
			new_le.ptr = sybase_ptr;
			if (zend_hash_update(plist, hashed_details, hashed_details_length+1, (void *) &new_le, sizeof(list_entry),NULL)==FAILURE) {
				free(sybase_ptr);
				efree(hashed_details);
				dbloginfree(sybase.login);
				RETURN_FALSE;
			}
			php3_sybase_module.num_persistent++;
			php3_sybase_module.num_links++;
		} else {  /* we do */
			if (le->type != php3_sybase_module.le_plink) {
				php_error(E_WARNING,"Sybase:  Hashed persistent link is not a Sybase link!");
				RETURN_FALSE;
			}
			
			sybase_ptr = (sybase_link *) le->ptr;
			/* test that the link hasn't died */
			if (DBDEAD(sybase_ptr->link)==TRUE) {
				if ((sybase_ptr->link=dbopen(sybase_ptr->login,host))==FAIL) {
					/*php_error(E_WARNING,"Sybase:  Link to server lost, unable to reconnect");*/
					zend_hash_del(plist, hashed_details, hashed_details_length+1);
					efree(hashed_details);
					RETURN_FALSE;
				}
				if (dbsetopt(sybase_ptr->link,DBBUFFER,"2",-1)==FAIL) {
					zend_hash_del(plist, hashed_details, hashed_details_length+1);
					efree(hashed_details);
					RETURN_FALSE;
				}
			}
		}
		return_value->value.lval = php3_list_insert(sybase_ptr,php3_sybase_module.le_plink);
		return_value->type = IS_LONG;
	} else { /* non persistent */
		list_entry *index_ptr,new_index_ptr;
		
		/* first we check the hash for the hashed_details key.  if it exists,
		 * it should point us to the right offset where the actual sybase link sits.
		 * if it doesn't, open a new sybase link, add it to the resource list,
		 * and add a pointer to it with hashed_details as the key.
		 */
		if (zend_hash_find(list,hashed_details,hashed_details_length+1,(void **) &index_ptr)==SUCCESS) {
			int type,link;
			void *ptr;

			if (index_ptr->type != le_index_ptr) {
				RETURN_FALSE;
			}
			link = (int) index_ptr->ptr;
			ptr = php3_list_find(link,&type);   /* check if the link is still there */
			if (ptr && (type==php3_sybase_module.le_link || type==php3_sybase_module.le_plink)) {
				return_value->value.lval = php3_sybase_module.default_link = link;
				return_value->type = IS_LONG;
				efree(hashed_details);
				return;
			} else {
				zend_hash_del(list,hashed_details,hashed_details_length+1);
			}
		}
		if (php3_sybase_module.max_links!=-1 && php3_sybase_module.num_links>=php3_sybase_module.max_links) {
			php_error(E_WARNING,"Sybase:  Too many open links (%d)",php3_sybase_module.num_links);
			efree(hashed_details);
			RETURN_FALSE;
		}
		
		if ((sybase.link=dbopen(sybase.login,host))==NULL) {
			/*php_error(E_WARNING,"Sybase:  Unable to connect to server:  %s",sybase_error(sybase));*/
			efree(hashed_details);
			RETURN_FALSE;
		}

		if (dbsetopt(sybase.link,DBBUFFER,"2",-1)==FAIL) {
			efree(hashed_details);
			dbloginfree(sybase.login);
			dbclose(sybase.link);
			RETURN_FALSE;
		}
		
		/* add it to the list */
		sybase_ptr = (sybase_link *) emalloc(sizeof(sybase_link));
		memcpy(sybase_ptr,&sybase,sizeof(sybase_link));
		return_value->value.lval = php3_list_insert(sybase_ptr,php3_sybase_module.le_link);
		return_value->type = IS_LONG;
		
		/* add it to the hash */
		new_index_ptr.ptr = (void *) return_value->value.lval;
		new_index_ptr.type = le_index_ptr;
		if (zend_hash_update(list,hashed_details,hashed_details_length+1,(void *) &new_index_ptr, sizeof(list_entry),NULL)==FAILURE) {
			efree(hashed_details);
			RETURN_FALSE;
		}
		php3_sybase_module.num_links++;
	}
	efree(hashed_details);
	php3_sybase_module.default_link=return_value->value.lval;
}


static int php3_sybase_get_default_link(INTERNAL_FUNCTION_PARAMETERS)
{
	if (php3_sybase_module.default_link==-1) { /* no link opened yet, implicitly open one */
		ht = 0;
		php3_sybase_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);
	}
	return php3_sybase_module.default_link;
}


PHP_FUNCTION(sybase_connect)
{
	php3_sybase_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);
}

PHP_FUNCTION(sybase_pconnect)
{
	php3_sybase_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,1);
}


PHP_FUNCTION(sybase_close)
{
	pval *sybase_link_index;
	int id,type;
	
	switch (ARG_COUNT(ht)) {
		case 0:
			id = php3_sybase_module.default_link;
			break;
		case 1:
			if (getParameters(ht, 1, &sybase_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(sybase_link_index);
			id = sybase_link_index->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	php3_list_find(id,&type);
	if (type!=php3_sybase_module.le_link && type!=php3_sybase_module.le_plink) {
		php_error(E_WARNING,"%d is not a Sybase link index",id);
		RETURN_FALSE;
	}
	
	php3_list_delete(id);
	RETURN_TRUE;
}
	

PHP_FUNCTION(sybase_select_db)
{
	pval *db,*sybase_link_index;
	int id,type;
	sybase_link  *sybase_ptr;
	
	switch(ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &db)==FAILURE) {
				RETURN_FALSE;
			}
			id = php3_sybase_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			break;
		case 2:
			if (getParameters(ht, 2, &db, &sybase_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(sybase_link_index);
			id = sybase_link_index->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	CHECK_LINK(id);
	
	sybase_ptr = (sybase_link *) php3_list_find(id,&type);
	if (type!=php3_sybase_module.le_link && type!=php3_sybase_module.le_plink) {
		php_error(E_WARNING,"%d is not a Sybase link index",id);
		RETURN_FALSE;
	}
	
	convert_to_string(db);
	
	if (dbuse(sybase_ptr->link,db->value.str.val)==FAIL) {
		/*php_error(E_WARNING,"Sybase:  Unable to select database:  %s",sybase_error(sybase));*/
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}


static void php3_sybase_get_column_content(sybase_link *sybase_ptr,int offset,pval *result, int column_type)
{
	if (dbdatlen(sybase_ptr->link,offset) == 0) {
		var_reset(result);
		return;
	}

	switch (column_type)
	{
		case SYBINT2:
		case SYBINT4: {	
			result->value.lval = (long) anyintcol(offset);
			result->type = IS_LONG;
			break;
		}
		case SYBCHAR:
		case SYBTEXT: {
			int length;
			char *data = charcol(offset);

			length=dbdatlen(sybase_ptr->link,offset);
			while (length>0 && charcol(offset)[length-1] == ' ') { /* nuke trailing whitespace */
				length--;
			}
			result->value.str.val = estrndup(data,length);
			result->value.str.len = length;
			result->type = IS_STRING;
			break;
		}
		/*case SYBFLT8:*/
		case SYBREAL: {
			result->value.dval = (double) floatcol(offset);
			result->type = IS_DOUBLE;
			break;
		}
		default: {
			if (dbwillconvert(coltype(offset),SYBCHAR)) {
				char *res_buf;
				int res_length = dbdatlen(sybase_ptr->link,offset);
				register char *p;
			
				switch (coltype(offset)) {
					case SYBBINARY:
					case SYBVARBINARY:
					case SYBCHAR:
					case SYBVARCHAR:
					case SYBTEXT:
					case SYBIMAGE:
						break;
					default:
						/* take no chances, no telling how big the result would really be */
						res_length += 20;
						break;
				}

				res_buf = (char *) emalloc(res_length+1);
				dbconvert(NULL,coltype(offset),dbdata(sybase_ptr->link,offset), res_length,SYBCHAR,res_buf,-1);
		
				/* get rid of trailing spaces */
				p = res_buf + res_length;
				while (*p == ' ') {
					p--;
					res_length--;
				}
				*(++p) = 0; /* put a trailing NULL */
		
				result->value.str.len = res_length;
				result->value.str.val = res_buf;
				result->type = IS_STRING;
			} else {
				php_error(E_WARNING,"Sybase:  column %d has unknown data type (%d)", offset, coltype(offset));
				var_reset(result);
			}
		}
	}
}


PHP_FUNCTION(sybase_query)
{
	pval *query,*sybase_link_index;
	int id,type,retvalue;
	sybase_link *sybase_ptr;
	sybase_result *result;
	int num_fields;
	int blocks_initialized=1;
	int i,j;
	int *column_types;

	switch(ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &query)==FAILURE) {
				RETURN_FALSE;
			}
			id = php3_sybase_module.default_link;
			break;
		case 2:
			if (getParameters(ht, 2, &query, &sybase_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(sybase_link_index);
			id = sybase_link_index->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	sybase_ptr = (sybase_link *) php3_list_find(id,&type);
	if (type!=php3_sybase_module.le_link && type!=php3_sybase_module.le_plink) {
		php_error(E_WARNING,"%d is not a Sybase link index",id);
		RETURN_FALSE;
	}
	
	convert_to_string(query);
	if (dbcmd(sybase_ptr->link,query->value.str.val)==FAIL) {
		/*php_error(E_WARNING,"Sybase:  Unable to set query");*/
		RETURN_FALSE;
	}
	if (dbsqlexec(sybase_ptr->link)==FAIL || dbresults(sybase_ptr->link)==FAIL) {
		/*php_error(E_WARNING,"Sybase:  Query failed");*/
		RETURN_FALSE;
	}
	
	/* The following is more or less the equivalent of mysql_store_result().
	 * fetch all rows from the server into the row buffer, thus:
	 * 1)  Being able to fire up another query without explicitly reading all rows
	 * 2)  Having numrows accessible
	 */
	
	retvalue=dbnextrow(sybase_ptr->link);
	
	if (retvalue==FAIL) {
		RETURN_FALSE;
	}

	num_fields = dbnumcols(sybase_ptr->link);
	if (num_fields<=0) {
		RETURN_TRUE;
	}
	
	column_types = (int *) emalloc(sizeof(int) * num_fields);
	for (i=0; i<num_fields; i++) {
		column_types[i] = coltype(i+1);
	}
	
	result = (sybase_result *) emalloc(sizeof(sybase_result));
	result->data = (pval **) emalloc(sizeof(pval *)*SYBASE_ROWS_BLOCK);
	result->sybase_ptr = sybase_ptr;
	result->cur_field=result->cur_row=result->num_rows=0;
	result->num_fields = num_fields;

	i=0;
	while (retvalue!=FAIL && retvalue!=NO_MORE_ROWS) {
		result->num_rows++;
		if (result->num_rows > blocks_initialized*SYBASE_ROWS_BLOCK) {
			result->data = (pval **) erealloc(result->data,sizeof(pval *)*SYBASE_ROWS_BLOCK*(++blocks_initialized));
		}
		result->data[i] = (pval *) emalloc(sizeof(pval)*num_fields);
		for (j=1; j<=num_fields; j++) {
			php3_sybase_get_column_content(sybase_ptr, j, &result->data[i][j-1], column_types[j-1]);
			if (!php3_sybase_module.compatability_mode) {
				convert_to_string(&result->data[i][j-1]);
			}
		}
		retvalue=dbnextrow(sybase_ptr->link);
		dbclrbuf(sybase_ptr->link,DBLASTROW(sybase_ptr->link)-1); 
		i++;
	}
	result->num_rows = DBCOUNT(sybase_ptr->link);
	
	result->fields = (sybase_field *) emalloc(sizeof(sybase_field)*num_fields);
	j=0;
	for (i=0; i<num_fields; i++) {
		char *fname = dbcolname(sybase_ptr->link,i+1);
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
		result->fields[i].max_length = dbcollen(sybase_ptr->link,i+1);
		result->fields[i].column_source = estrdup(dbcolsource(sybase_ptr->link,i+1));
		if (!result->fields[i].column_source) {
			result->fields[i].column_source = empty_string;
		}
		result->fields[i].type = column_types[i];
		/* set numeric flag */
		switch (column_types[i]) {
			case SYBINT2:
			case SYBINT4:
			case SYBFLT8:
			case SYBREAL:
				result->fields[i].numeric = 1;
				break;
			case SYBCHAR:
			case SYBTEXT:
			default:
				result->fields[i].numeric = 0;
				break;
		}
	}
	efree(column_types);
	return_value->value.lval = php3_list_insert(result,php3_sybase_module.le_result);
	return_value->type = IS_LONG;
}

                        
PHP_FUNCTION(sybase_free_result)
{
	pval *sybase_result_index;
	sybase_result *result;
	int type;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &sybase_result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(sybase_result_index);
	if (sybase_result_index->value.lval==0) {
		RETURN_FALSE;
	}
	result = (sybase_result *) php3_list_find(sybase_result_index->value.lval,&type);
	
	if (type!=php3_sybase_module.le_result) {
		php_error(E_WARNING,"%d is not a Sybase result index",sybase_result_index->value.lval);
		RETURN_FALSE;
	}
	php3_list_delete(sybase_result_index->value.lval);
	RETURN_TRUE;
}


PHP_FUNCTION(sybase_get_last_message)
{
	RETURN_STRING(php3_sybase_module.server_message,1);
}


PHP_FUNCTION(sybase_num_rows)
{
	pval *result_index;
	int type,id;
	sybase_result *result;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result_index);
	id = result_index->value.lval;
	
	result = (sybase_result *) php3_list_find(id,&type);
	if (type!=php3_sybase_module.le_result) {
		php_error(E_WARNING,"%d is not a Sybase result index",id);
		RETURN_FALSE;
	}	
	
	return_value->value.lval = result->num_rows;
	return_value->type = IS_LONG;
}


PHP_FUNCTION(sybase_num_fields)
{
	pval *result_index;
	int type,id;
	sybase_result *result;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result_index);
	id = result_index->value.lval;
	
	result = (sybase_result *) php3_list_find(id,&type);
	if (type!=php3_sybase_module.le_result) {
		php_error(E_WARNING,"%d is not a Sybase result index",id);
		RETURN_FALSE;
	}	
	
	return_value->value.lval = result->num_fields;
	return_value->type = IS_LONG;
}


PHP_FUNCTION(sybase_fetch_row)
{
	pval *sybase_result_index;
	int type,i,id;
	sybase_result *result;
	pval *field_content;

	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &sybase_result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(sybase_result_index);
	id = sybase_result_index->value.lval;
	
	result = (sybase_result *) php3_list_find(id,&type);
	if (type!=php3_sybase_module.le_result) {
		php_error(E_WARNING,"%d is not a Sybase result index",id);
		RETURN_FALSE;
	}
	
	if (result->cur_row >= result->num_rows) {
		RETURN_FALSE;
	}
	
	array_init(return_value);
	for (i=0; i<result->num_fields; i++) {
		MAKE_STD_ZVAL(field_content);
		*field_content = result->data[result->cur_row][i];
		pval_copy_constructor(field_content);
		zend_hash_index_update(return_value->value.ht, i, (void *) &field_content, sizeof(pval *), NULL);
	}
	result->cur_row++;
}


static PHP_FUNCTION(sybase_fetch_hash)
{
	pval *sybase_result_index;
	sybase_result *result;
	int type;
	int i;
	pval *tmp;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &sybase_result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(sybase_result_index);
	result = (sybase_result *) php3_list_find(sybase_result_index->value.lval,&type);
	
	if (type!=php3_sybase_module.le_result) {
		php_error(E_WARNING,"%d is not a Sybase result index",sybase_result_index->value.lval);
		RETURN_FALSE;
	}
	
	if (result->cur_row >= result->num_rows) {
		RETURN_FALSE;
	}
	
	if (array_init(return_value)==FAILURE) {
		RETURN_FALSE;
	}
	
	for (i=0; i<result->num_fields; i++) {
		MAKE_STD_ZVAL(tmp);
		*tmp = result->data[result->cur_row][i];
		pval_copy_constructor(tmp);
		if (PG(magic_quotes_runtime) && tmp->type == IS_STRING) {
			tmp->value.str.val = _php3_addslashes(tmp->value.str.val,tmp->value.str.len,&tmp->value.str.len,1);
		}
		zend_hash_index_update(return_value->value.ht, i, (void *) &tmp, sizeof(pval *), NULL);
		tmp->refcount++;
		zend_hash_update(return_value->value.ht, result->fields[i].name, strlen(result->fields[i].name)+1, (void *) &tmp, sizeof(pval  *), NULL);
	}
	result->cur_row++;
}


PHP_FUNCTION(sybase_fetch_object)
{
	php3_sybase_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	if (return_value->type==IS_ARRAY) {
		return_value->type=IS_OBJECT;
		return_value->value.obj.properties = return_value->value.ht;
		return_value->value.obj.ce = &zend_standard_class_def;
	}
}


PHP_FUNCTION(sybase_fetch_array)
{
	php3_sybase_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_FUNCTION(sybase_data_seek)
{
	pval *sybase_result_index,*offset;
	int type,id;
	sybase_result *result;

	if (ARG_COUNT(ht)!=2 || getParameters(ht, 2, &sybase_result_index, &offset)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(sybase_result_index);
	id = sybase_result_index->value.lval;
	
	result = (sybase_result *) php3_list_find(id,&type);
	if (type!=php3_sybase_module.le_result) {
		php_error(E_WARNING,"%d is not a Sybase result index",id);
		RETURN_FALSE;
	}

	convert_to_long(offset);
	if (offset->value.lval<0 || offset->value.lval>=result->num_rows) {
		php_error(E_WARNING,"Sybase:  Bad row offset");
		RETURN_FALSE;
	}
	
	result->cur_row = offset->value.lval;
	RETURN_TRUE;
}


static char *php3_sybase_get_field_name(int type)
{
	switch (type) {
		case SYBBINARY:
		case SYBVARBINARY:
			return "blob";
			break;
		case SYBCHAR:
		case SYBVARCHAR:
		case SYBTEXT:
			return "string";
		case SYBDATETIME:
		case SYBDATETIME4:
		case SYBDATETIMN:
			return "datetime";
			break;
		case SYBDECIMAL:
		case SYBFLT8:
		case SYBFLTN:
		case SYBREAL:
		case SYBNUMERIC:
			return "real";
			break;
		case SYBINT1:
		case SYBINT2:
		case SYBINT4:
		case SYBINTN:
			return "int";
			break;
		case SYBMONEY:
		case SYBMONEY4:
		case SYBMONEYN:
			return "money";
			break;
		case SYBBIT:
			return "bit";
			break;
		case SYBIMAGE:
			return "image";
			break;
		default:
			return "unknown";
			break;
	}
}


PHP_FUNCTION(sybase_fetch_field)
{
	pval *sybase_result_index,*offset;
	int type,id,field_offset;
	sybase_result *result;

	switch (ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &sybase_result_index)==FAILURE) {
				RETURN_FALSE;
			}
			field_offset=-1;
			break;
		case 2:
			if (getParameters(ht, 2, &sybase_result_index, &offset)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(offset);
			field_offset = offset->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	convert_to_long(sybase_result_index);
	id = sybase_result_index->value.lval;
	
	result = (sybase_result *) php3_list_find(id,&type);
	if (type!=php3_sybase_module.le_result) {
		php_error(E_WARNING,"%d is not a Sybase result index",id);
		RETURN_FALSE;
	}
	
	if (field_offset==-1) {
		field_offset = result->cur_field;
		result->cur_field++;
	}
	
	if (field_offset<0 || field_offset >= result->num_fields) {
		if (ARG_COUNT(ht)==2) { /* field specified explicitly */
			php_error(E_WARNING,"Sybase:  Bad column offset");
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
	add_property_string(return_value, "type", php3_sybase_get_field_name(result->fields[field_offset].type), 1);
}

PHP_FUNCTION(sybase_field_seek)
{
	pval *sybase_result_index,*offset;
	int type,id,field_offset;
	sybase_result *result;

	if (ARG_COUNT(ht)!=2 || getParameters(ht, 2, &sybase_result_index, &offset)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(sybase_result_index);
	id = sybase_result_index->value.lval;
	
	result = (sybase_result *) php3_list_find(id,&type);
	if (type!=php3_sybase_module.le_result) {
		php_error(E_WARNING,"%d is not a Sybase result index",id);
		RETURN_FALSE;
	}
	
	convert_to_long(offset);
	field_offset = offset->value.lval;
	
	if (field_offset<0 || field_offset >= result->num_fields) {
		php_error(E_WARNING,"Sybase:  Bad column offset");
		RETURN_FALSE;
	}

	result->cur_field = field_offset;
	RETURN_TRUE;
}


PHP_FUNCTION(sybase_result)
{
	pval *row, *field, *sybase_result_index;
	int id,type,field_offset=0;
	sybase_result *result;
	

	if (ARG_COUNT(ht)!=3 || getParameters(ht, 3, &sybase_result_index, &row, &field)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(sybase_result_index);
	id = sybase_result_index->value.lval;
	
	result = (sybase_result *) php3_list_find(id,&type);
	if (type!=php3_sybase_module.le_result) {
		php_error(E_WARNING,"%d is not a Sybase result index",id);
		RETURN_FALSE;
	}
	
	convert_to_long(row);
	if (row->value.lval<0 || row->value.lval>=result->num_rows) {
		php_error(E_WARNING,"Sybase:  Bad row offset (%d)",row->value.lval);
		RETURN_FALSE;
	}

	switch(field->type) {
		case IS_STRING: {
			int i;

			for (i=0; i<result->num_fields; i++) {
				if (!strcasecmp(result->fields[i].name,field->value.str.val)) {
					field_offset = i;
					break;
				}
			}
			if (i>=result->num_fields) { /* no match found */
				php_error(E_WARNING,"Sybase:  %s field not found in result",field->value.str.val);
				RETURN_FALSE;
			}
			break;
		}
		default:
			convert_to_long(field);
			field_offset = field->value.lval;
			if (field_offset<0 || field_offset>=result->num_fields) {
				php_error(E_WARNING,"Sybase:  Bad column offset specified");
				RETURN_FALSE;
			}
			break;
	}

	*return_value = result->data[row->value.lval][field_offset];
	pval_copy_constructor(return_value);
}


void php3_info_sybase(ZEND_MODULE_INFO_FUNC_ARGS)
{
	char maxp[16],maxl[16];
	
	if (php3_sybase_module.max_persistent==-1) {
		strcpy(maxp,"Unlimited");
	} else {
		snprintf(maxp,15,"%ld",php3_sybase_module.max_persistent);
		maxp[15]=0;
	}
	if (php3_sybase_module.max_links==-1) {
		strcpy(maxl,"Unlimited");
	} else {
		snprintf(maxl,15,"%ld",php3_sybase_module.max_links);
		maxl[15]=0;
	}
	php_printf("<table cellpadding=5>"
				"<tr><td>Allow persistent links:</td><td>%s</td></tr>\n"
				"<tr><td>Persistent links:</td><td>%d/%s</td></tr>\n"
				"<tr><td>Total links:</td><td>%d/%s</td></tr>\n"
				"<tr><td>Application name:</td><td>%s</td></tr>\n"
				"<tr><td valign=\"top\" width=\"20%%\">Client API information:</td><td><pre>%s</pre></td></tr>\n"
				"</table>\n",
				(php3_sybase_module.allow_persistent?"Yes":"No"),
				php3_sybase_module.num_persistent,maxp,
				php3_sybase_module.num_links,maxl,
				php3_sybase_module.appname,
				dbversion());
}


PHP_FUNCTION(sybase_min_error_severity)
{
	pval *severity;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &severity)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(severity);
	php3_sybase_module.min_error_severity = severity->value.lval;
}


PHP_FUNCTION(sybase_min_message_severity)
{
	pval *severity;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &severity)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(severity);
	php3_sybase_module.min_message_severity = severity->value.lval;
}

#endif
