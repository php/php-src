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
   | Author: Zeev Suraski <zeev@zend.com>                                 |
   +----------------------------------------------------------------------+
 */
 
/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_msql.h"
#include "ext/standard/php_standard.h"
#include "ext/standard/info.h"
#include "php_globals.h"

#if HAVE_MSQL

#define OS_UNIX 1

#if defined(WIN32) && defined(MSQL1)
#include <msql1.h>
#else
#include <msql.h>
#endif

static php_msql_globals msql_globals;

#define MSQL_ASSOC		1<<0
#define MSQL_NUM		1<<1
#define MSQL_BOTH		(MSQL_ASSOC|MSQL_NUM)

/* {{{ msql_functions[]
 */
function_entry msql_functions[] = {
	PHP_FE(msql_connect,							NULL)
	PHP_FE(msql_pconnect,							NULL)
	PHP_FE(msql_close,								NULL)
	PHP_FE(msql_select_db,							NULL)
	PHP_FE(msql_create_db,							NULL)
	PHP_FE(msql_drop_db,							NULL)
	PHP_FE(msql_query,								NULL)
	PHP_FE(msql_db_query,							NULL)
	PHP_FE(msql_list_dbs,							NULL)
	PHP_FE(msql_list_tables,						NULL)
	PHP_FE(msql_list_fields,						NULL)
	PHP_FE(msql_error,								NULL)
	PHP_FE(msql_result,								NULL)
	PHP_FE(msql_num_rows,							NULL)
	PHP_FE(msql_num_fields,							NULL)
	PHP_FE(msql_fetch_row,							NULL)
	PHP_FE(msql_fetch_array,						NULL)
	PHP_FE(msql_fetch_object,						NULL)
	PHP_FE(msql_data_seek,							NULL)
	PHP_FE(msql_fetch_field,						NULL)
	PHP_FE(msql_field_seek,							NULL)
	PHP_FE(msql_free_result,						NULL)
	PHP_FE(msql_field_name,							NULL)
	PHP_FE(msql_field_table,						NULL)
	PHP_FE(msql_field_len,							NULL)
	PHP_FE(msql_field_type,							NULL)
	PHP_FE(msql_field_flags,						NULL)
	
	PHP_FALIAS(msql_fieldname,		msql_field_name,		NULL)
	PHP_FALIAS(msql_fieldtable,		msql_field_table,		NULL)
	PHP_FALIAS(msql_fieldlen,		msql_field_len,			NULL)
	PHP_FALIAS(msql_fieldtype,		msql_field_type,		NULL)
	PHP_FALIAS(msql_fieldflags,		msql_field_flags,		NULL)
	
	PHP_FALIAS(msql_regcase,		sql_regcase,			NULL)
	
	PHP_FALIAS(msql_affected_rows,	msql_affected_rows,		NULL)
	
	/* for downwards compatability */
	PHP_FALIAS(msql,				msql_db_query,			NULL)
	PHP_FALIAS(msql_selectdb,		msql_select_db,			NULL)
	PHP_FALIAS(msql_createdb,		msql_create_db,			NULL)
	PHP_FALIAS(msql_dropdb,			msql_drop_db,			NULL)
	PHP_FALIAS(msql_freeresult,		msql_free_result,		NULL)
	PHP_FALIAS(msql_numfields,		msql_num_fields,		NULL)
	PHP_FALIAS(msql_numrows,		msql_num_rows,			NULL)
	PHP_FALIAS(msql_listdbs,		msql_list_dbs,			NULL)
	PHP_FALIAS(msql_listtables,		msql_list_tables,		NULL)
	PHP_FALIAS(msql_listfields,		msql_list_fields,		NULL)
	PHP_FALIAS(msql_dbname,			msql_result,			NULL)
	PHP_FALIAS(msql_tablename,		msql_result,			NULL)
	{NULL, NULL, NULL}
};
/* }}} */

zend_module_entry msql_module_entry = {
	STANDARD_MODULE_HEADER,
	"msql", msql_functions, PHP_MINIT(msql), NULL, PHP_RINIT(msql), NULL,
			PHP_MINFO(msql), NO_VERSION_YET, STANDARD_MODULE_PROPERTIES
};


#ifdef COMPILE_DL_MSQL
ZEND_GET_MODULE(msql)
#endif

typedef struct {
	m_result *result;
	int af_rows;
} m_query;

#define MSQL_GET_QUERY(res)																			\
	ZEND_FETCH_RESOURCE(msql_query, m_query *, &res, -1, "mSQL result", msql_globals.le_query);	\
	msql_result = msql_query->result

static void _delete_query(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	m_query *query = (m_query *)rsrc->ptr;

	if(query->result) msqlFreeResult(query->result);
	efree(query);
}

static m_query *php_msql_query_wrapper(m_result *res, int af_rows)
{
	m_query *query = (m_query *) emalloc(sizeof(m_query));
	
	query->result = res;
	query->af_rows = af_rows;
	
	return query;
}

static void _close_msql_link(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	int link = (int)rsrc->ptr;

	msqlClose(link);
	msql_globals.num_links--;
}


static void _close_msql_plink(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	int link = (int)rsrc->ptr;

	msqlClose(link);
	msql_globals.num_persistent--;
	msql_globals.num_links--;
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(msql)
{
	if (cfg_get_long("msql.allow_persistent",&msql_globals.allow_persistent)==FAILURE) {
		msql_globals.allow_persistent=1;
	}
	if (cfg_get_long("msql.max_persistent",&msql_globals.max_persistent)==FAILURE) {
		msql_globals.max_persistent=-1;
	}
	if (cfg_get_long("msql.max_links",&msql_globals.max_links)==FAILURE) {
		msql_globals.max_links=-1;
	}
	msql_globals.num_persistent=0;
	msql_globals.le_query = zend_register_list_destructors_ex(_delete_query, NULL, "msql query", module_number);
	msql_globals.le_link = zend_register_list_destructors_ex(_close_msql_link, NULL, "msql link", module_number);
	msql_globals.le_plink = zend_register_list_destructors_ex(NULL, _close_msql_plink, "msql link persistent", module_number);
	
	Z_TYPE(msql_module_entry) = type;

	REGISTER_LONG_CONSTANT("MSQL_ASSOC", MSQL_ASSOC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MSQL_NUM", MSQL_NUM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MSQL_BOTH", MSQL_BOTH, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(msql)
{
	msql_globals.default_link=-1;
	msql_globals.num_links = msql_globals.num_persistent;
	msqlErrMsg[0]=0;
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(msql)
{
	char maxp[32],maxl[32];

	if (msql_globals.max_persistent==-1) {
		snprintf(maxp, 31, "%d/unlimited", msql_globals.num_persistent );
	} else {
		snprintf(maxp, 31, "%d/%ld", msql_globals.num_persistent, msql_globals.max_persistent);
	}
	maxp[31]=0;

	if (msql_globals.max_links==-1) {
		snprintf(maxl, 15, "%d/unlimited", msql_globals.num_links );
	} else {
		snprintf(maxl, 15, "%d/%ld", msql_globals.num_links, msql_globals.max_links);
	}
	maxl[31]=0;

	php_info_print_table_start();
	php_info_print_table_row(2, "MSQL Support", "enabled" );
	php_info_print_table_row(2, "Allow Persistent Links", (msql_globals.allow_persistent?"yes":"no") );
	php_info_print_table_row(2, "Persistent Links", maxp );
	php_info_print_table_row(2, "Total Links", maxl );
	php_info_print_table_end();

}
/* }}} */

/* {{{ php_msql_do_connect
 */
static void php_msql_do_connect(INTERNAL_FUNCTION_PARAMETERS,int persistent)
{
	char *host;
	char *hashed_details;
	int hashed_details_length;
	int msql;
	
	switch(ZEND_NUM_ARGS()) {
		case 0: /* defaults */
			host=NULL;
			hashed_details=estrndup("msql_",5);
			hashed_details_length=4+1;
			break;
		case 1: {
				pval *yyhost;
			
				if (getParameters(ht, 1, &yyhost) == FAILURE) {
					RETURN_FALSE;
				}
				convert_to_string(yyhost);
				host = Z_STRVAL_P(yyhost);
				hashed_details_length = Z_STRLEN_P(yyhost)+4+1;
				hashed_details = emalloc(hashed_details_length+1);
				sprintf(hashed_details,"msql_%s",Z_STRVAL_P(yyhost)); /* SAFE */
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	if (!msql_globals.allow_persistent) {
		persistent=0;
	}
	if (persistent) {
		list_entry *le;
		
		if (msql_globals.max_links!=-1 && msql_globals.num_links>=msql_globals.max_links) {
			php_error(E_WARNING, "%s(): Too many open links (%d)", get_active_function_name(TSRMLS_C), msql_globals.num_links);
			efree(hashed_details);
			RETURN_FALSE;
		}
		if (msql_globals.max_persistent!=-1 && msql_globals.num_persistent>=msql_globals.max_persistent) {
			php_error(E_WARNING, "%s(): Too many open persistent links (%d)", get_active_function_name(TSRMLS_C), msql_globals.num_persistent);
			efree(hashed_details);
			RETURN_FALSE;
		}
		
		/* try to find if we already have this link in our persistent list */
		if (zend_hash_find(&EG(persistent_list), hashed_details, hashed_details_length+1, (void **) &le)==FAILURE) {  /* we don't */
			list_entry new_le;
			
			/* create the link */
			if ((msql=msqlConnect(host))==-1) {
				efree(hashed_details);
				RETURN_FALSE;
			}
			
			/* hash it up */
			Z_TYPE(new_le) = msql_globals.le_plink;
			new_le.ptr = (void *) msql;
			if (zend_hash_update(&EG(persistent_list), hashed_details, hashed_details_length+1, (void *) &new_le, sizeof(list_entry), NULL)==FAILURE) {
				efree(hashed_details);
				RETURN_FALSE;
			}
			msql_globals.num_persistent++;
			msql_globals.num_links++;
		} else {  /* we do */
			if (Z_TYPE_P(le) != msql_globals.le_plink) {
				efree(hashed_details);
				RETURN_FALSE;
			}
#if 0
			/* ensure that the link did not die */
			/* still have to find a way to do this nicely with mSQL */
			if (msql_stat(le->ptr)==NULL) { /* the link died */
				if (msql_connect(le->ptr,host,user,passwd)==NULL) {
					php_error(E_WARNING,"mSQL link lost, unable to reconnect");
					zend_hash_del(&EG(persistent_list),hashed_details,hashed_details_length+1);
					efree(hashed_details);
					RETURN_FALSE;
				}
			}
#endif
			msql = (int) le->ptr;
		}
		ZEND_REGISTER_RESOURCE(return_value, (void *) msql, msql_globals.le_plink);
	} else {
		list_entry *index_ptr,new_index_ptr;
		
		/* first we check the hash for the hashed_details key.  if it exists,
		 * it should point us to the right offset where the actual msql link sits.
		 * if it doesn't, open a new msql link, add it to the resource list,
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
			if (ptr && (type==msql_globals.le_link || type==msql_globals.le_plink)) {
				Z_LVAL_P(return_value) = msql_globals.default_link = link;
				Z_TYPE_P(return_value) = IS_RESOURCE;
				efree(hashed_details);
				return;
			} else {
				zend_hash_del(&EG(regular_list),hashed_details,hashed_details_length+1);
			}
		}
		if (msql_globals.max_links!=-1 && msql_globals.num_links>=msql_globals.max_links) {
			php_error(E_WARNING, "%s(): Too many open links (%d)", get_active_function_name(TSRMLS_C),msql_globals.num_links);
			efree(hashed_details);
			RETURN_FALSE;
		}
		if ((msql=msqlConnect(host))==-1) {
			efree(hashed_details);
			RETURN_FALSE;
		}

		/* add it to the list */
		ZEND_REGISTER_RESOURCE(return_value, (void *) msql, msql_globals.le_link);
		
		/* add it to the hash */
		new_index_ptr.ptr = (void *) Z_LVAL_P(return_value);
		Z_TYPE(new_index_ptr) = le_index_ptr;
		if (zend_hash_update(&EG(regular_list),hashed_details,hashed_details_length+1,(void *) &new_index_ptr, sizeof(list_entry), NULL)==FAILURE) {
			efree(hashed_details);
			RETURN_FALSE;
		}
		msql_globals.num_links++;
	}
	efree(hashed_details);
	msql_globals.default_link=Z_LVAL_P(return_value);
}
/* }}} */

/* {{{ php_msql_get_default_link
 */
static int php_msql_get_default_link(INTERNAL_FUNCTION_PARAMETERS)
{
	if (msql_globals.default_link==-1) { /* no link opened yet, implicitly open one */
		ht = 0;
		php_msql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
	}
	return msql_globals.default_link;
}
/* }}} */

/* {{{ proto int msql_connect([string hostname[:port]] [, string username] [, string password])
   Open a connection to an mSQL Server */
PHP_FUNCTION(msql_connect)
{
	php_msql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);
}
/* }}} */

/* {{{ proto int msql_pconnect([string hostname[:port]] [, string username] [, string password])
   Open a persistent connection to an mSQL Server */
PHP_FUNCTION(msql_pconnect)
{
	php_msql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,1);
}
/* }}} */

/* {{{ proto int msql_close([int link_identifier])
   Close an mSQL connection */
PHP_FUNCTION(msql_close)
{
	pval *msql_link;
	int id;
	int msql;
	
	switch (ZEND_NUM_ARGS()) {
		case 0:
			id = msql_globals.default_link;
			break;
		case 1:
			if (getParameters(ht, 1, &msql_link)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE2(msql, int, &msql_link, id, "mSQL-Link", msql_globals.le_link, msql_globals.le_plink);
	
	if (id==-1) { /* explicit resource number */
		zend_list_delete(Z_RESVAL_P(msql_link));
	}

	if (id!=-1 
		|| (msql_link && Z_RESVAL_P(msql_link)==msql_globals.default_link)) {
		zend_list_delete(msql_globals.default_link);
		msql_globals.default_link = -1;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int msql_select_db(string database_name [, int link_identifier])
   Select an mSQL database */
PHP_FUNCTION(msql_select_db)
{
	pval *db,*msql_link;
	int id;
	int msql;
	
	switch(ZEND_NUM_ARGS()) {
		case 1:
			if (getParameters(ht, 1, &db)==FAILURE) {
				RETURN_FALSE;
			}
			id = php_msql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			break;
		case 2:
			if (getParameters(ht, 2, &db, &msql_link)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	

	ZEND_FETCH_RESOURCE2(msql, int, &msql_link, id, "mSQL-Link", msql_globals.le_link, msql_globals.le_plink);

	convert_to_string(db);
	
	if (msqlSelectDB(msql,Z_STRVAL_P(db))==-1) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ proto int msql_create_db(string database_name [, int link_identifier])
   Create an mSQL database */
PHP_FUNCTION(msql_create_db)
{
	pval *db,*msql_link;
	int id;
	int msql;
	
	switch(ZEND_NUM_ARGS()) {
		case 1:
			if (getParameters(ht, 1, &db)==FAILURE) {
				WRONG_PARAM_COUNT;
			}
			id = php_msql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			break;
		case 2:
			if (getParameters(ht, 2, &db, &msql_link)==FAILURE) {
				WRONG_PARAM_COUNT;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE2(msql, int, &msql_link, id, "mSQL-Link", msql_globals.le_link, msql_globals.le_plink);
	
	convert_to_string(db);
	if (msqlCreateDB(msql,Z_STRVAL_P(db))<0) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ proto int msql_drop_db(string database_name [, int link_identifier])
   Drop (delete) an mSQL database */
PHP_FUNCTION(msql_drop_db)
{
	pval *db,*msql_link;
	int id;
	int msql;
	
	switch(ZEND_NUM_ARGS()) {
		case 1:
			if (getParameters(ht, 1, &db)==FAILURE) {
				WRONG_PARAM_COUNT;
			}
			id = php_msql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			break;
		case 2:
			if (getParameters(ht, 2, &db, &msql_link)==FAILURE) {
				WRONG_PARAM_COUNT;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE2(msql, int, &msql_link, id, "mSQL-Link", msql_globals.le_link, msql_globals.le_plink);
	
	convert_to_string(db);
	if (msqlDropDB(msql,Z_STRVAL_P(db))<0) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ proto int msql_query(string query [, int link_identifier])
   Send an SQL query to mSQL */
PHP_FUNCTION(msql_query)
{
	pval *query,*msql_link;
	int id;
	int msql;
	int af_rows;
	
	switch(ZEND_NUM_ARGS()) {
		case 1:
			if (getParameters(ht, 1, &query)==FAILURE) {
				WRONG_PARAM_COUNT;
			}
			id = msql_globals.default_link;
			break;
		case 2:
			if (getParameters(ht, 2, &query, &msql_link)==FAILURE) {
				WRONG_PARAM_COUNT;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE2(msql, int, &msql_link, id, "mSQL-Link", msql_globals.le_link, msql_globals.le_plink);
	
	convert_to_string(query);
	if ((af_rows = msqlQuery(msql,Z_STRVAL_P(query)))==-1) {
		RETURN_FALSE;
	}
	ZEND_REGISTER_RESOURCE(return_value, php_msql_query_wrapper(msqlStoreResult(), af_rows), msql_globals.le_query);
}
/* }}} */

/* {{{ proto int msql_db_query(string database_name, string query [, int link_identifier])
   Send an SQL query to mSQL */
PHP_FUNCTION(msql_db_query)
{
	pval *db,*query,*msql_link;
	int id;
	int msql;
	int af_rows;
	
	switch(ZEND_NUM_ARGS()) {
		case 2:
			if (getParameters(ht, 2, &db, &query)==FAILURE) {
				RETURN_FALSE;
			}
			id = php_msql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			break;
		case 3:
			if (getParameters(ht, 3, &db, &query, &msql_link)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE2(msql, int, &msql_link, id, "mSQL-Link", msql_globals.le_link, msql_globals.le_plink);
	
	convert_to_string(db);
	if (msqlSelectDB(msql,Z_STRVAL_P(db))==-1) {
		RETURN_FALSE;
	}
	
	convert_to_string(query);
	if ((af_rows = msqlQuery(msql,Z_STRVAL_P(query)))==-1) {
		RETURN_FALSE;
	}
	ZEND_REGISTER_RESOURCE(return_value, php_msql_query_wrapper(msqlStoreResult(), af_rows), msql_globals.le_query);
}
/* }}} */

/* {{{ proto int msql_list_dbs([int link_identifier])
   List databases available on an mSQL server */
PHP_FUNCTION(msql_list_dbs)
{
	pval *msql_link;
	int id;
	int msql;
	m_result *msql_result;
	
	switch(ZEND_NUM_ARGS()) {
		case 0:
			id = php_msql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			break;
		case 1:
			if (getParameters(ht, 1, &msql_link)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE2(msql, int, &msql_link, id, "mSQL-Link", msql_globals.le_link, msql_globals.le_plink);

	if ((msql_result=msqlListDBs(msql))==NULL) {
		php_error(E_WARNING, "%s(): Unable to save mSQL query result", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}
	ZEND_REGISTER_RESOURCE(return_value, php_msql_query_wrapper(msql_result, 0), msql_globals.le_query);
}
/* }}} */

/* {{{ proto int msql_list_tables(string database_name [, int link_identifier])
   List tables in an mSQL database */
PHP_FUNCTION(msql_list_tables)
{
	pval *db,*msql_link;
	int id;
	int msql;
	m_result *msql_result;
	
	switch(ZEND_NUM_ARGS()) {
		case 1:
			if (getParameters(ht, 1, &db)==FAILURE) {
				RETURN_FALSE;
			}
			id = php_msql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			break;
		case 2:
			if (getParameters(ht, 2, &db, &msql_link)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE2(msql, int, &msql_link, id, "mSQL-Link", msql_globals.le_link, msql_globals.le_plink);
	
	convert_to_string(db);
	if (msqlSelectDB(msql,Z_STRVAL_P(db))==-1) {
		RETURN_FALSE;
	}
	if ((msql_result=msqlListTables(msql))==NULL) {
		php_error(E_WARNING, "%s(): Unable to save mSQL query result", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}
	ZEND_REGISTER_RESOURCE(return_value, php_msql_query_wrapper(msql_result, 0), msql_globals.le_query);
}
/* }}} */

/* {{{ proto int msql_list_fields(string database_name, string table_name [, int link_identifier])
   List mSQL result fields */
PHP_FUNCTION(msql_list_fields)
{
	pval *db,*table,*msql_link;
	int id;
	int msql;
	m_result *msql_result;
	
	switch(ZEND_NUM_ARGS()) {
		case 2:
			if (getParameters(ht, 2, &db, &table)==FAILURE) {
				RETURN_FALSE;
			}
			id = php_msql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			break;
		case 3:
			if (getParameters(ht, 3, &db, &table, &msql_link)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE2(msql, int, &msql_link, id, "mSQL-Link", msql_globals.le_link, msql_globals.le_plink);
	
	convert_to_string(db);
	if (msqlSelectDB(msql,Z_STRVAL_P(db))==-1) {
		RETURN_FALSE;
	}
	convert_to_string(table);
	if ((msql_result=msqlListFields(msql,Z_STRVAL_P(table)))==NULL) {
		php_error(E_WARNING, "%s(): Unable to save mSQL query result", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}
	ZEND_REGISTER_RESOURCE(return_value, php_msql_query_wrapper(msql_result, 0), msql_globals.le_query);
}
/* }}} */

/* {{{ proto string msql_error([int link_identifier])
   Returns the text of the error message from previous mSQL operation */
PHP_FUNCTION(msql_error)
{
	if (ZEND_NUM_ARGS()) {
		WRONG_PARAM_COUNT;
	}
	RETURN_STRING(msqlErrMsg,1);
}
/* }}} */

/* {{{ proto int msql_result(int query, int row [, mixed field])
   Get result data */
PHP_FUNCTION(msql_result)
{
	pval *result, *row, *field=NULL;
	m_result *msql_result;
	m_query *msql_query;
	m_row sql_row;
	int field_offset=0;
	
	switch (ZEND_NUM_ARGS()) {
		case 2:
			if (getParameters(ht, 2, &result, &row)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 3:
			if (getParameters(ht, 3, &result, &row, &field)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	MSQL_GET_QUERY(result);
	
	convert_to_long(row);
	if (Z_LVAL_P(row)<0 || Z_LVAL_P(row)>=msqlNumRows(msql_result)) {
		php_error(E_WARNING, "%s(): Unable to jump to row %d on mSQL query index %d", get_active_function_name(TSRMLS_C),Z_LVAL_P(row),Z_LVAL_P(result));
		RETURN_FALSE;
	}
	msqlDataSeek(msql_result,Z_LVAL_P(row));
	if ((sql_row=msqlFetchRow(msql_result))==NULL) { /* shouldn't happen? */
		RETURN_FALSE;
	}

	if (field) {
		switch(Z_TYPE_P(field)) {
			case IS_STRING: {
					int i=0;
					m_field *tmp_field;
					char *table_name,*field_name,*tmp;
					
					if ((tmp=strchr(Z_STRVAL_P(field),'.'))) {
						*tmp = 0;
						table_name = estrdup(Z_STRVAL_P(field));
						field_name = estrdup(tmp+1);
					} else {
						table_name = NULL;
						field_name = estrndup(Z_STRVAL_P(field),Z_STRLEN_P(field));
					}
					msqlFieldSeek(msql_result,0);
					while ((tmp_field=msqlFetchField(msql_result))) {
						if ((!table_name || !strcasecmp(tmp_field->table,table_name)) && !strcasecmp(tmp_field->name,field_name)) {
							field_offset = i;
							break;
						}
						i++;
					}
					if (!tmp_field) { /* no match found */
						php_error(E_WARNING, "%s(): %s%s%s not found in mSQL query index %d", get_active_function_name(TSRMLS_C),
									(table_name?table_name:""), (table_name?".":""), field_name, Z_LVAL_P(result));
						efree(field_name);
						if (table_name) {
							efree(table_name);
						}
						RETURN_FALSE;
					}
					efree(field_name);
					if (table_name) {
						efree(table_name);
					}
				}
				break;
			default:
				convert_to_long(field);
				field_offset = Z_LVAL_P(field);
				if (field_offset<0 || field_offset>=msqlNumFields(msql_result)) {
					php_error(E_WARNING, "%s(): Bad column offset specified", get_active_function_name(TSRMLS_C));
					RETURN_FALSE;
				}
				break;
		}
	}
	
	if (sql_row[field_offset]) {
		if (PG(magic_quotes_runtime)) {
			Z_STRVAL_P(return_value) = php_addslashes(sql_row[field_offset],0,&Z_STRLEN_P(return_value),0 TSRMLS_CC);
		} else {	
			Z_STRLEN_P(return_value) = (sql_row[field_offset]?strlen(sql_row[field_offset]):0);
			Z_STRVAL_P(return_value) = (char *) safe_estrndup(sql_row[field_offset],Z_STRLEN_P(return_value));
		}
		Z_TYPE_P(return_value) = IS_STRING;
	} else {
		ZVAL_FALSE(return_value);
	}
}
/* }}} */

/* {{{ proto int msql_num_rows(int query)
   Get number of rows in a result */
PHP_FUNCTION(msql_num_rows)
{
	pval *result;
	m_result *msql_result;
	m_query *msql_query;
	
	if (ZEND_NUM_ARGS()!=1 || getParameters(ht, 1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	MSQL_GET_QUERY(result);
	RETVAL_LONG(msql_result ? msqlNumRows(msql_result) : 0);
}
/* }}} */

/* {{{ proto int msql_num_fields(int query)
   Get number of fields in a result */
PHP_FUNCTION(msql_num_fields)
{
	pval *result;
	m_result *msql_result;
	m_query *msql_query;
	
	if (ZEND_NUM_ARGS()!=1 || getParameters(ht, 1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	MSQL_GET_QUERY(result);
	RETVAL_LONG(msql_result ? msqlNumFields(msql_result) : 0);
}
/* }}} */

/* {{{ php_msql_fetch_hash
 */
static void php_msql_fetch_hash(INTERNAL_FUNCTION_PARAMETERS, int result_type)
{
	pval *result, *arg2;
	m_result *msql_result;
	m_row msql_row;
	m_field *msql_field;
	m_query *msql_query;
	int num_fields;
	int i;
	
	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (getParameters(ht, 1, &result)==FAILURE) {
				RETURN_FALSE;
			}
			if (!result_type) {
				result_type = MSQL_BOTH;
			}
			break;
		case 2:
			if (getParameters(ht, 2, &result, &arg2)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(arg2);
			result_type = Z_LVAL_P(arg2);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	MSQL_GET_QUERY(result);
	if (!msql_result || (msql_row=msqlFetchRow(msql_result))==NULL) {
		RETURN_FALSE;
	}

	num_fields = msqlNumFields(msql_result);
	
	if (array_init(return_value)==FAILURE) {
		RETURN_FALSE;
	}
	
	msqlFieldSeek(msql_result,0);
	for (msql_field=msqlFetchField(msql_result),i=0; msql_field; msql_field=msqlFetchField(msql_result),i++) {
		if (msql_row[i]) {
			char *data;
			int data_len;
			int should_copy;

			if (PG(magic_quotes_runtime)) {
				data = php_addslashes(msql_row[i], 0, &data_len, 0 TSRMLS_CC);
				should_copy = 0;
			} else {
				data = msql_row[i];
				data_len = strlen(data);
				should_copy = 1;
			}
			
			if (result_type & MSQL_NUM) {
				add_index_stringl(return_value, i, data, data_len, should_copy);
				should_copy = 1;
			}
			
			if (result_type & MSQL_ASSOC) {
				add_assoc_stringl(return_value, msql_field->name, data, data_len, should_copy);
			}
		} else {
			/*
			add_get_index_stringl(return_value, i, empty_string, 0, (void **) &pval_ptr, 1);
			*/
		}
	}
}
/* }}} */

/* {{{ proto array msql_fetch_row(int query)
   Get a result row as an enumerated array */
PHP_FUNCTION(msql_fetch_row)
{
	php_msql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, MSQL_NUM);
}
/* }}} */

/* {{{ proto object msql_fetch_object(int query [, int result_type])
   Fetch a result row as an object */
PHP_FUNCTION(msql_fetch_object)
{
	php_msql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
	if (Z_TYPE_P(return_value)==IS_ARRAY) {
		object_and_properties_init(return_value, ZEND_STANDARD_CLASS_DEF_PTR, Z_ARRVAL_P(return_value));
	}
}
/* }}} */

/* {{{ proto array msql_fetch_array(int query [, int result_type])
   Fetch a result row as an associative array */
PHP_FUNCTION(msql_fetch_array)
{
	php_msql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int msql_data_seek(int query, int row_number)
   Move internal result pointer */
PHP_FUNCTION(msql_data_seek)
{
	pval *result,*offset;
	m_result *msql_result;
	m_query *msql_query;
	
	if (ZEND_NUM_ARGS()!=2 || getParameters(ht, 2, &result, &offset)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	MSQL_GET_QUERY(result);
	convert_to_long(offset);
	if (!msql_result ||
			Z_LVAL_P(offset)<0 || 
			Z_LVAL_P(offset)>=msqlNumRows(msql_result)) {
		php_error(E_WARNING, "%s(): %d is invalid for mSQL query index %d", get_active_function_name(TSRMLS_C),Z_LVAL_P(offset),Z_LVAL_P(result));
		RETURN_FALSE;
	}
	msqlDataSeek(msql_result,Z_LVAL_P(offset));
	RETURN_TRUE;
}
/* }}} */

/* {{{ php_msql_get_field_name
 */
static char *php_msql_get_field_name(int field_type)
{
	switch (field_type) {
#if MSQL1
		case INT_TYPE:
			return "int";
			break;
		case CHAR_TYPE:
			return "char";
			break;
		case REAL_TYPE:
			return "real";
			break;
		case IDENT_TYPE:
			return "ident";
			break;
		case NULL_TYPE:
			return "null";
			break;
#else
		case INT_TYPE:
		case UINT_TYPE:
		case CHAR_TYPE:
		case TEXT_TYPE:
		case REAL_TYPE:
		case NULL_TYPE:
		case DATE_TYPE:
		case TIME_TYPE:
		case MONEY_TYPE:
			return msqlTypeNames[field_type];
			break;
#endif
		default:
			return "unknown";
			break;
	}
}
/* }}} */

/* {{{ proto object msql_fetch_field(int query [, int field_offset])
   Get column information from a result and return as an object */
PHP_FUNCTION(msql_fetch_field)
{
	pval *result, *field=NULL;
	m_result *msql_result;
	m_field *msql_field;
	m_query *msql_query;
	
	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (getParameters(ht, 1, &result)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (getParameters(ht, 2, &result, &field)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(field);
		default:
			WRONG_PARAM_COUNT;
	}
	
	MSQL_GET_QUERY(result);
	
	if (field) {
		if (Z_LVAL_P(field)<0 || Z_LVAL_P(field)>=msqlNumRows(msql_result)) {
			php_error(E_NOTICE, "%s(): Bad field offset specified", get_active_function_name(TSRMLS_C));
			RETURN_FALSE;
		}
		msqlFieldSeek(msql_result,Z_LVAL_P(field));
	}
	if (!msql_result || (msql_field=msqlFetchField(msql_result))==NULL) {
		RETURN_FALSE;
	}
	if (object_init(return_value)==FAILURE) {
		RETURN_FALSE;
	}

	add_property_string(return_value, "name",(msql_field->name?msql_field->name:empty_string), 1);
	add_property_string(return_value, "table",(msql_field->table?msql_field->table:empty_string), 1);
	add_property_long(return_value, "not_null",IS_NOT_NULL(msql_field->flags));
#if MSQL1
	add_property_long(return_value, "primary_key",(msql_field->flags&PRI_KEY_FLAG?1:0));
#else
	add_property_long(return_value, "unique",(msql_field->flags&UNIQUE_FLAG?1:0));
#endif

	add_property_string(return_value, "type",php_msql_get_field_name(Z_TYPE_P(msql_field)), 1);
}
/* }}} */

/* {{{ proto int msql_field_seek(int query, int field_offset)
   Set result pointer to a specific field offset */
PHP_FUNCTION(msql_field_seek)
{
	pval *result, *offset;
	m_result *msql_result;
	m_query *msql_query;
	
	if (ZEND_NUM_ARGS()!=2 || getParameters(ht, 2, &result, &offset)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	MSQL_GET_QUERY(result);
	convert_to_long(offset);
	if(!msql_result) {
		RETURN_FALSE;
	}
	if (Z_LVAL_P(offset)<0 || Z_LVAL_P(offset)>=msqlNumFields(msql_result)) {
		php_error(E_WARNING,"%s(): Field %d is invalid for mSQL query index %d", get_active_function_name(TSRMLS_C),
				Z_LVAL_P(offset),Z_LVAL_P(result));
		RETURN_FALSE;
	}
	msqlFieldSeek(msql_result,Z_LVAL_P(offset));
	RETURN_TRUE;
}
/* }}} */

#define PHP_MSQL_FIELD_NAME 1
#define PHP_MSQL_FIELD_TABLE 2
#define PHP_MSQL_FIELD_LEN 3
#define PHP_MSQL_FIELD_TYPE 4
#define PHP_MSQL_FIELD_FLAGS 5

/* {{{ php_msql_field_info
 */
static void php_msql_field_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type)
{
	pval *result, *field;
	m_result *msql_result;
	m_field *msql_field;
	m_query *msql_query;
	
	if (ZEND_NUM_ARGS()!=2 || getParameters(ht, 2, &result, &field)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	MSQL_GET_QUERY(result);
	if(!msql_result) {
		RETURN_FALSE;
	}
	convert_to_long(field);
	if (Z_LVAL_P(field)<0 || Z_LVAL_P(field)>=msqlNumFields(msql_result)) {
		php_error(E_WARNING,"%s(): Field %d is invalid for mSQL query index %d", get_active_function_name(TSRMLS_C),Z_LVAL_P(field),Z_LVAL_P(result));
		RETURN_FALSE;
	}
	msqlFieldSeek(msql_result,Z_LVAL_P(field));
	if ((msql_field=msqlFetchField(msql_result))==NULL) {
		RETURN_FALSE;
	}
	
	switch (entry_type) {
		case PHP_MSQL_FIELD_NAME:
			Z_STRLEN_P(return_value) = strlen(msql_field->name);
			Z_STRVAL_P(return_value) = estrndup(msql_field->name,Z_STRLEN_P(return_value));
			Z_TYPE_P(return_value) = IS_STRING;
			break;
		case PHP_MSQL_FIELD_TABLE:
			Z_STRLEN_P(return_value) = strlen(msql_field->table);
			Z_STRVAL_P(return_value) = estrndup(msql_field->table,Z_STRLEN_P(return_value));
			Z_TYPE_P(return_value) = IS_STRING;
			break;
		case PHP_MSQL_FIELD_LEN:
			Z_LVAL_P(return_value) = msql_field->length;
			Z_TYPE_P(return_value) = IS_LONG;
			break;
		case PHP_MSQL_FIELD_TYPE:
			Z_STRVAL_P(return_value) = estrdup(php_msql_get_field_name(Z_TYPE_P(msql_field)));
			Z_STRLEN_P(return_value) = strlen(Z_STRVAL_P(return_value));
			Z_TYPE_P(return_value) = IS_STRING;
			break;
		case PHP_MSQL_FIELD_FLAGS:
#if MSQL1
			if ((msql_field->flags&NOT_NULL_FLAG) && (msql_field->flags&PRI_KEY_FLAG)) {
				Z_STRVAL_P(return_value) = estrndup("primary key not null",20);
				Z_STRLEN_P(return_value) = 20;
				Z_TYPE_P(return_value) = IS_STRING;
			} else if (msql_field->flags&NOT_NULL_FLAG) {
				Z_STRVAL_P(return_value) = estrndup("not null",8);
				Z_STRLEN_P(return_value) = 8;
				Z_TYPE_P(return_value) = IS_STRING;
			} else if (msql_field->flags&PRI_KEY_FLAG) {
				Z_STRVAL_P(return_value) = estrndup("primary key",11);
				Z_STRLEN_P(return_value) = 11;
				Z_TYPE_P(return_value) = IS_STRING;
			} else {
				ZVAL_FALSE(return_value);
			}
#else
			if ((msql_field->flags&NOT_NULL_FLAG) && (msql_field->flags&UNIQUE_FLAG)) {
				Z_STRVAL_P(return_value) = estrndup("unique not null",15);
				Z_STRLEN_P(return_value) = 15;
				Z_TYPE_P(return_value) = IS_STRING;
			} else if (msql_field->flags&NOT_NULL_FLAG) {
				Z_STRVAL_P(return_value) = estrndup("not null",8);
				Z_STRLEN_P(return_value) = 8;
				Z_TYPE_P(return_value) = IS_STRING;
			} else if (msql_field->flags&UNIQUE_FLAG) {
				Z_STRVAL_P(return_value) = estrndup("unique",6);
				Z_STRLEN_P(return_value) = 6;
				Z_TYPE_P(return_value) = IS_STRING;
			} else {
				ZVAL_FALSE(return_value);
			}
#endif
			break;
		default:
			RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string msql_field_name(int query, int field_index)
   Get the name of the specified field in a result */
PHP_FUNCTION(msql_field_name)
{
	php_msql_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_MSQL_FIELD_NAME);
}
/* }}} */

/* {{{ proto string msql_field_table(int query, int field_offset)
   Get name of the table the specified field is in */
PHP_FUNCTION(msql_field_table)
{
	php_msql_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_MSQL_FIELD_TABLE);
}
/* }}} */

/* {{{ proto int msql_field_len(int query, int field_offet)
   Returns the length of the specified field */
PHP_FUNCTION(msql_field_len)
{
	php_msql_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_MSQL_FIELD_LEN);
}
/* }}} */

/* {{{ proto string msql_field_type(int query, int field_offset)
   Get the type of the specified field in a result */
PHP_FUNCTION(msql_field_type)
{
	php_msql_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_MSQL_FIELD_TYPE);
}
/* }}} */

/* {{{ proto string msql_field_flags(int query, int field_offset)
   Get the flags associated with the specified field in a result */
PHP_FUNCTION(msql_field_flags)
{
	php_msql_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_MSQL_FIELD_FLAGS);
}
/* }}} */

/* {{{ proto int msql_free_result(int query)
   Free result memory */
PHP_FUNCTION(msql_free_result)
{
	pval *result;
	m_result *msql_result;
	m_query *msql_query;
	
	if (ZEND_NUM_ARGS()!=1 || getParameters(ht, 1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	MSQL_GET_QUERY(result);
	zend_list_delete(Z_LVAL_P(result));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int msql_affected_rows(int query)
   Return number of affected rows */
PHP_FUNCTION(msql_affected_rows) 
{
	pval *result;
	m_result *msql_result;
	m_query *msql_query;

	if(ZEND_NUM_ARGS() != 1 || getParameters(ht, 1, &result) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	MSQL_GET_QUERY(result);
	RETVAL_LONG(msql_query->af_rows);
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
