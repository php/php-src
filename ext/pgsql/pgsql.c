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
   | Authors: Zeev Suraski <zeev@zend.com>                                |
   |          Jouni Ahto <jouni.ahto@exdec.fi> (large object interface)   |
   +----------------------------------------------------------------------+
 */
 
/* $Id$ */

#include <stdlib.h>

#define PHP_PGSQL_PRIVATE 1

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/php_standard.h"
#include "php_pgsql.h"
#include "php_globals.h"

#if HAVE_PGSQL

#ifndef InvalidOid
#define InvalidOid ((Oid) 0)
#endif

#define PGSQL_ASSOC		1<<0
#define PGSQL_NUM		1<<1
#define PGSQL_BOTH		(PGSQL_ASSOC|PGSQL_NUM)

#define CHECK_DEFAULT_LINK(x) if (x == -1) { php_error(E_WARNING, "%s: no PostgreSQL link opened yet", get_active_function_name(TSRMLS_C)); }

/* {{{ pgsql_functions[]
 */
function_entry pgsql_functions[] = {
	PHP_FE(pg_connect,		NULL)
	PHP_FE(pg_pconnect,		NULL)
	PHP_FE(pg_close,		NULL)
	PHP_FE(pg_cmdtuples,	NULL)
	PHP_FE(pg_last_notice,  NULL)
	PHP_FE(pg_dbname,		NULL)
	PHP_FE(pg_errormessage,	NULL)
	PHP_FE(pg_trace,		NULL)
	PHP_FE(pg_untrace,		NULL)
	PHP_FE(pg_options,		NULL)
	PHP_FE(pg_port,			NULL)
	PHP_FE(pg_tty,			NULL)
	PHP_FE(pg_host,			NULL)
	PHP_FE(pg_exec,			NULL)
	PHP_FE(pg_numrows,		NULL)
	PHP_FE(pg_numfields,	NULL)
	PHP_FE(pg_fieldname,	NULL)
	PHP_FE(pg_fieldsize,	NULL)
	PHP_FE(pg_fieldtype,	NULL)
	PHP_FE(pg_fieldnum,		NULL)
	PHP_FE(pg_result,		NULL)
	PHP_FE(pg_fetch_row,	NULL)
	PHP_FE(pg_fetch_array,	NULL)
	PHP_FE(pg_fetch_object,	NULL)
	PHP_FE(pg_fieldprtlen,	NULL)
	PHP_FE(pg_fieldisnull,	NULL)
	PHP_FE(pg_freeresult,	NULL)
	PHP_FE(pg_getlastoid,	NULL)
	PHP_FE(pg_locreate,		NULL)
	PHP_FE(pg_lounlink,		NULL)
	PHP_FE(pg_loopen,		NULL)
	PHP_FE(pg_loclose,		NULL)
	PHP_FE(pg_loread,		NULL)
	PHP_FE(pg_lowrite,		NULL)
	PHP_FE(pg_loreadall,	NULL)
	PHP_FE(pg_loimport,		NULL)
	PHP_FE(pg_loexport,		NULL)
	PHP_FE(pg_put_line,		NULL)
	PHP_FE(pg_end_copy,		NULL)
#if HAVE_PQCLIENTENCODING
	PHP_FE(pg_client_encoding,		NULL)
	PHP_FE(pg_set_client_encoding,	NULL)
	/* for downwards compatibility */
	PHP_FALIAS(pg_clientencoding,		pg_client_encoding,			NULL)
	PHP_FALIAS(pg_setclientencoding,	pg_set_client_encoding,	NULL)
#endif
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ pgsql_module_entry
 */
zend_module_entry pgsql_module_entry = {
	"pgsql",
	pgsql_functions,
	PHP_MINIT(pgsql),
	PHP_MSHUTDOWN(pgsql),
	PHP_RINIT(pgsql),
	PHP_RSHUTDOWN(pgsql),
	PHP_MINFO(pgsql),
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PGSQL
ZEND_GET_MODULE(pgsql)
#endif

static int le_link, le_plink, le_result, le_lofp, le_string;

#ifdef ZTS
int pgsql_globals_id;
#else
PHP_PGSQL_API php_pgsql_globals pgsql_globals;
#endif

/* {{{ php_pgsql_set_default_link
 */
static void php_pgsql_set_default_link(int id)
{   
	TSRMLS_FETCH();

	zend_list_addref(id);

	if (PGG(default_link) != -1) {
		zend_list_delete(PGG(default_link));
	}

	PGG(default_link) = id;
}
/* }}} */

/* {{{ _close_pgsql_link
 */
static void _close_pgsql_link(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	PGconn *link = (PGconn *)rsrc->ptr;

	PQfinish(link);
	PGG(num_links)--;
}
/* }}} */

/* {{{ _close_pgsql_plink
 */
static void _close_pgsql_plink(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	PGconn *link = (PGconn *)rsrc->ptr;

	PQfinish(link);
	PGG(num_persistent)--;
	PGG(num_links)--;
	if(PGG(last_notice) != NULL) {
		efree(PGG(last_notice));
	}
}
/* }}} */

/* {{{ _notice_handler
 */
static void
_notice_handler(void *arg, const char *message)
{
	TSRMLS_FETCH();

	if (! PGG(ignore_notices)) {
		php_log_err((char *) message TSRMLS_CC);
		if (PGG(last_notice) != NULL) {
			efree(PGG(last_notice));
		}
		PGG(last_notice) = estrdup(message);
	}
}
/* }}} */

/* {{{ _rollback_transactions
 */
static int _rollback_transactions(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	PGconn *link;

	if (rsrc->type != le_plink) 
		return 0;

	link = (PGconn *) rsrc->ptr;
	
	PGG(ignore_notices) = 1;
	PQexec(link,"BEGIN;ROLLBACK;");
	PGG(ignore_notices) = 0;

	return 0;
}
/* }}} */

/* {{{ _free_ptr
 */
static void _free_ptr(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	pgLofp *lofp = (pgLofp *)rsrc->ptr;
	efree(lofp);
}
/* }}} */

/* {{{ _free_result
 */
static void _free_result(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	pgsql_result_handle *pg_result = (pgsql_result_handle *)rsrc->ptr;

	PQclear(pg_result->result);
	efree(pg_result);
}
/* }}} */

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
	STD_PHP_INI_BOOLEAN("pgsql.allow_persistent",	"1",	PHP_INI_SYSTEM,		OnUpdateInt,		allow_persistent,	php_pgsql_globals,		pgsql_globals)
	STD_PHP_INI_ENTRY_EX("pgsql.max_persistent",	"-1",	PHP_INI_SYSTEM,		OnUpdateInt,		max_persistent,		php_pgsql_globals,		pgsql_globals,	display_link_numbers)
	STD_PHP_INI_ENTRY_EX("pgsql.max_links",		"-1",	PHP_INI_SYSTEM,			OnUpdateInt,		max_links,			php_pgsql_globals,		pgsql_globals,	display_link_numbers)
PHP_INI_END()
/* }}} */

/* {{{ php_pgsql_init_globals
 */
static void php_pgsql_init_globals(php_pgsql_globals *pgsql_globals_p TSRMLS_DC)
{
	PGG(num_persistent) = 0;
	PGG(ignore_notices) = 0;
	PGG(last_notice) = NULL;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(pgsql)
{
#ifdef ZTS
	ts_allocate_id(&pgsql_globals_id, sizeof(php_pgsql_globals), (ts_allocate_ctor) php_pgsql_init_globals, NULL);
#else
	php_pgsql_init_globals(&pgsql_globals TSRMLS_CC);
#endif

	REGISTER_INI_ENTRIES();
	
	le_link = zend_register_list_destructors_ex(_close_pgsql_link, NULL, "pgsql link", module_number);
	le_plink = zend_register_list_destructors_ex(NULL, _close_pgsql_plink, "pgsql link persistent", module_number);
	/*	PGG(le_result = register_list_destructors(PQclear,NULL); */
	le_result = zend_register_list_destructors_ex(_free_result, NULL, "pgsql result", module_number);
	le_lofp = zend_register_list_destructors_ex(_free_ptr, NULL, "pgsql large object", module_number);
	le_string = zend_register_list_destructors_ex(_free_ptr, NULL, "pgsql string", module_number);

	REGISTER_LONG_CONSTANT("PGSQL_ASSOC", PGSQL_ASSOC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_NUM", PGSQL_NUM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_BOTH", PGSQL_BOTH, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(pgsql)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(pgsql)
{
	PGG(default_link)=-1;
	PGG(num_links) = PGG(num_persistent);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(pgsql)
{
	zend_hash_apply(&EG(persistent_list), (apply_func_t) _rollback_transactions TSRMLS_CC);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(pgsql)
{
	char buf[32];

	php_info_print_table_start();
	php_info_print_table_header(2, "PostgreSQL Support", "enabled");
	sprintf(buf, "%ld", PGG(num_persistent));
	php_info_print_table_row(2, "Active Persistent Links", buf);
	sprintf(buf, "%ld", PGG(num_links));
	php_info_print_table_row(2, "Active Links", buf);
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();

}
/* }}} */


/* {{{ php_pgsql_do_connect
 */
void php_pgsql_do_connect(INTERNAL_FUNCTION_PARAMETERS,int persistent)
{
	char *host=NULL,*port=NULL,*options=NULL,*tty=NULL,*dbname=NULL,*connstring=NULL;
	char *hashed_details;
	int hashed_details_length;
	PGconn *pgsql;
	
	switch(ZEND_NUM_ARGS()) {
		case 1: { /* new style, using connection string */
				zval **yyconnstring;
				if (zend_get_parameters_ex(1, &yyconnstring) == FAILURE) {
					RETURN_FALSE;
				}
				convert_to_string_ex(yyconnstring);
				connstring = Z_STRVAL_PP(yyconnstring);
				hashed_details_length = Z_STRLEN_PP(yyconnstring)+5+1;
				hashed_details = (char *) emalloc(hashed_details_length+1);
				sprintf(hashed_details, "pgsql_%s", connstring); /* SAFE */
			}
			break;
		case 3: { /* host, port, dbname */
				zval **yyhost, **yyport, **yydbname;
				
				if (zend_get_parameters_ex(3, &yyhost, &yyport, &yydbname) == FAILURE) {
					RETURN_FALSE;
				}
				convert_to_string_ex(yyhost);
				convert_to_string_ex(yyport);
				convert_to_string_ex(yydbname);
				host = Z_STRVAL_PP(yyhost);
				port = Z_STRVAL_PP(yyport);
				dbname = Z_STRVAL_PP(yydbname);
				options=tty=NULL;
				hashed_details_length = Z_STRLEN_PP(yyhost) + Z_STRLEN_PP(yyport) + Z_STRLEN_PP(yydbname) + 5 + 5;
				hashed_details = (char *) emalloc(hashed_details_length+1);
				sprintf(hashed_details, "pgsql_%s_%s___%s", host, port, dbname);  /* SAFE */
			}
			break;
		case 4: { /* host, port, options, dbname */
				zval **yyhost, **yyport, **yyoptions, **yydbname;
				
				if (zend_get_parameters_ex(4, &yyhost, &yyport, &yyoptions, &yydbname) == FAILURE) {
					RETURN_FALSE;
				}
				convert_to_string_ex(yyhost);
				convert_to_string_ex(yyport);
				convert_to_string_ex(yyoptions);
				convert_to_string_ex(yydbname);
				host = Z_STRVAL_PP(yyhost);
				port = Z_STRVAL_PP(yyport);
				options = Z_STRVAL_PP(yyoptions);
				dbname = Z_STRVAL_PP(yydbname);
				tty=NULL;
				hashed_details_length = Z_STRLEN_PP(yyhost) + Z_STRLEN_PP(yyport) + Z_STRLEN_PP(yyoptions) + Z_STRLEN_PP(yydbname) + 5 + 5;
				hashed_details = (char *) emalloc(hashed_details_length+1);
				sprintf(hashed_details, "pgsql_%s_%s_%s__%s", host, port, options, dbname);  /* SAFE */
			}
			break;
		case 5: { /* host, port, options, tty, dbname */
				zval **yyhost, **yyport, **yyoptions, **yytty, **yydbname;
				
				if (zend_get_parameters_ex(5, &yyhost, &yyport, &yyoptions, &yytty, &yydbname) == FAILURE) {
					RETURN_FALSE;
				}
				convert_to_string_ex(yyhost);
				convert_to_string_ex(yyport);
				convert_to_string_ex(yyoptions);
				convert_to_string_ex(yytty);
				convert_to_string_ex(yydbname);
				host = Z_STRVAL_PP(yyhost);
				port = Z_STRVAL_PP(yyport);
				options = Z_STRVAL_PP(yyoptions);
				tty = Z_STRVAL_PP(yytty);
				dbname = Z_STRVAL_PP(yydbname);
				hashed_details_length = Z_STRLEN_PP(yyhost) + Z_STRLEN_PP(yyport) + Z_STRLEN_PP(yyoptions) + Z_STRLEN_PP(yytty) + Z_STRLEN_PP(yydbname) + 5 + 5;
				hashed_details = (char *) emalloc(hashed_details_length+1);
				sprintf(hashed_details, "pgsql_%s_%s_%s_%s_%s", host, port, options, tty, dbname);  /* SAFE */
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	if (persistent && PGG(allow_persistent)) {
		list_entry *le;
		
		/* try to find if we already have this link in our persistent list */
		if (zend_hash_find(&EG(persistent_list), hashed_details, hashed_details_length+1, (void **) &le)==FAILURE) {  /* we don't */
			list_entry new_le;
			
			if (PGG(max_links)!=-1 && PGG(num_links)>=PGG(max_links)) {
				php_error(E_WARNING,"PostgreSQL:  Too many open links (%d)",PGG(num_links));
				efree(hashed_details);
				RETURN_FALSE;
			}
			if (PGG(max_persistent)!=-1 && PGG(num_persistent)>=PGG(max_persistent)) {
				php_error(E_WARNING,"PostgreSQL:  Too many open persistent links (%d)",PGG(num_persistent));
				efree(hashed_details);
				RETURN_FALSE;
			}

			/* create the link */
			if (connstring) {
				pgsql=PQconnectdb(connstring);
			} else {
				pgsql=PQsetdb(host,port,options,tty,dbname);
			}
			if (pgsql==NULL || PQstatus(pgsql)==CONNECTION_BAD) {
				php_error(E_WARNING,"Unable to connect to PostgreSQL server:  %s",PQerrorMessage(pgsql));
				if (pgsql) {
					PQfinish(pgsql);
				}
				efree(hashed_details);
				RETURN_FALSE;
			}

 			PQsetNoticeProcessor(pgsql, _notice_handler, NULL);

			/* hash it up */
			new_le.type = le_plink;
			new_le.ptr = pgsql;
			if (zend_hash_update(&EG(persistent_list), hashed_details, hashed_details_length+1, (void *) &new_le, sizeof(list_entry), NULL)==FAILURE) {
				efree(hashed_details);
				RETURN_FALSE;
			}
			PGG(num_links)++;
			PGG(num_persistent)++;
		} else {  /* we do */
			if (le->type != le_plink) {
				RETURN_FALSE;
			}
			/* ensure that the link did not die */
			if (PQstatus(le->ptr)==CONNECTION_BAD) { /* the link died */
				if (connstring) {
					le->ptr=PQconnectdb(connstring);
				} else {
					le->ptr=PQsetdb(host,port,options,tty,dbname);
				}
				if (le->ptr==NULL || PQstatus(le->ptr)==CONNECTION_BAD) {
					php_error(E_WARNING,"PostgreSQL link lost, unable to reconnect");
					zend_hash_del(&EG(persistent_list),hashed_details,hashed_details_length+1);
					efree(hashed_details);
					RETURN_FALSE;
				}
			}
			pgsql = (PGconn *) le->ptr;

 			PQsetNoticeProcessor(pgsql, _notice_handler, NULL);
		}
		ZEND_REGISTER_RESOURCE(return_value, pgsql, le_plink);
	} else {
		list_entry *index_ptr,new_index_ptr;
		
		/* first we check the hash for the hashed_details key.  if it exists,
		 * it should point us to the right offset where the actual pgsql link sits.
		 * if it doesn't, open a new pgsql link, add it to the resource list,
		 * and add a pointer to it with hashed_details as the key.
		 */
		if (zend_hash_find(&EG(regular_list),hashed_details,hashed_details_length+1,(void **) &index_ptr)==SUCCESS) {
			int type,link;
			void *ptr;

			if (index_ptr->type != le_index_ptr) {
				RETURN_FALSE;
			}
			link = (int) (long) index_ptr->ptr;
			ptr = zend_list_find(link,&type);   /* check if the link is still there */
			if (ptr && (type==le_link || type==le_plink)) {
				return_value->value.lval = link;
				zend_list_addref(link);
				php_pgsql_set_default_link(link);
				return_value->type = IS_RESOURCE;
				efree(hashed_details);
				return;
			} else {
				zend_hash_del(&EG(regular_list),hashed_details,hashed_details_length+1);
			}
		}
		if (PGG(max_links)!=-1 && PGG(num_links)>=PGG(max_links)) {
			php_error(E_WARNING,"PostgreSQL:  Too many open links (%d)",PGG(num_links));
			efree(hashed_details);
			RETURN_FALSE;
		}
		if (connstring) {
			pgsql = PQconnectdb(connstring);
		} else {
			pgsql = PQsetdb(host,port,options,tty,dbname);
		}
		if (pgsql==NULL || PQstatus(pgsql)==CONNECTION_BAD) {
			php_error(E_WARNING,"Unable to connect to PostgreSQL server:  %s",PQerrorMessage(pgsql));
			efree(hashed_details);
			RETURN_FALSE;
		}

		PQsetNoticeProcessor(pgsql, _notice_handler, NULL);

		/* add it to the list */
		ZEND_REGISTER_RESOURCE(return_value, pgsql, le_link);

		/* add it to the hash */
		new_index_ptr.ptr = (void *) return_value->value.lval;
		new_index_ptr.type = le_index_ptr;
		if (zend_hash_update(&EG(regular_list),hashed_details,hashed_details_length+1,(void *) &new_index_ptr, sizeof(list_entry), NULL)==FAILURE) {
			efree(hashed_details);
			RETURN_FALSE;
		}
		PGG(num_links)++;
	}
	efree(hashed_details);
	php_pgsql_set_default_link(return_value->value.lval);
}
/* }}} */

/* {{{ php_pgsql_get_default_link
 */
int php_pgsql_get_default_link(INTERNAL_FUNCTION_PARAMETERS)
{
	if (PGG(default_link)==-1) { /* no link opened yet, implicitly open one */
		ht = 0;
		php_pgsql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);
	}
	return PGG(default_link);
}
/* }}} */

/* {{{ proto int pg_connect([string connection_string] | [string host, string port [, string options [, string tty,]] string database)
   Open a PostgreSQL connection */
PHP_FUNCTION(pg_connect)
{
	php_pgsql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);
}
/* }}} */

/* {{{ proto int pg_pconnect([string connection_string] | [string host, string port [, string options [, string tty,]] string database)
   Open a persistent PostgreSQL connection */
PHP_FUNCTION(pg_pconnect)
{
	php_pgsql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,1);
}
/* }}} */

/* {{{ proto bool pg_close([int connection])
   Close a PostgreSQL connection */ 
PHP_FUNCTION(pg_close)
{
	zval **pgsql_link = NULL;
	int id;
	PGconn *pgsql;
	
	switch (ZEND_NUM_ARGS()) {
		case 0:
			id = PGG(default_link);
			CHECK_DEFAULT_LINK(id);
			break;
		case 1:
			if (zend_get_parameters_ex(1, &pgsql_link)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (id==-1) { /* explicit resource number */
		zend_list_delete(Z_RESVAL_PP(pgsql_link));
	}

	if (id!=-1 
		|| (pgsql_link && Z_RESVAL_PP(pgsql_link)==PGG(default_link))) {
		zend_list_delete(PGG(default_link));
		PGG(default_link) = -1;
	}

	RETURN_TRUE;
}
/* }}} */


#define PHP_PG_DBNAME 1
#define PHP_PG_ERROR_MESSAGE 2
#define PHP_PG_OPTIONS 3
#define PHP_PG_PORT 4
#define PHP_PG_TTY 5
#define PHP_PG_HOST 6

/* {{{ php_pgsql_get_link_info
 */
void php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type)
{
	zval **pgsql_link = NULL;
	int id = -1;
	PGconn *pgsql;

	switch(ZEND_NUM_ARGS()) {
		case 0:
			id = PGG(default_link);
			CHECK_DEFAULT_LINK(id);
			break;
		case 1:
			if (zend_get_parameters_ex(1, &pgsql_link)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	switch(entry_type) {
		case PHP_PG_DBNAME:
			return_value->value.str.val = PQdb(pgsql);
			break;
		case PHP_PG_ERROR_MESSAGE:
			return_value->value.str.val = PQerrorMessage(pgsql);
			break;
		case PHP_PG_OPTIONS:
			return_value->value.str.val = PQoptions(pgsql);
			break;
		case PHP_PG_PORT:
			return_value->value.str.val = PQport(pgsql);
			break;
		case PHP_PG_TTY:
			return_value->value.str.val = PQtty(pgsql);
			break;
		case PHP_PG_HOST:
			return_value->value.str.val = PQhost(pgsql);
			break;
		default:
			RETURN_FALSE;
	}
	return_value->value.str.len = strlen(return_value->value.str.val);
	return_value->value.str.val = (char *) estrdup(return_value->value.str.val);
	return_value->type = IS_STRING;
}
/* }}} */

/* {{{ proto string pg_dbname([int connection])
   Get the database name */ 
PHP_FUNCTION(pg_dbname)
{
	php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_DBNAME);
}
/* }}} */

/* {{{ proto string pg_errormessage([int connection])
   Get the error message string */
PHP_FUNCTION(pg_errormessage)
{
	php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_ERROR_MESSAGE);
}
/* }}} */

/* {{{ proto string pg_options([int connection])
   Get the options associated with the connection */
PHP_FUNCTION(pg_options)
{
	php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_OPTIONS);
}
/* }}} */

/* {{{ proto int pg_port([int connection])
   Return the port number associated with the connection */
PHP_FUNCTION(pg_port)
{
	php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_PORT);
}
/* }}} */

/* {{{ proto string pg_tty([int connection])
   Return the tty name associated with the connection */
PHP_FUNCTION(pg_tty)
{
	php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_TTY);
}
/* }}} */

/* {{{ proto string pg_host([int connection])
   Returns the host name associated with the connection */
PHP_FUNCTION(pg_host)
{
	php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_HOST);
}
/* }}} */

/* {{{ proto int pg_exec([int connection,] string query)
   Execute a query */
PHP_FUNCTION(pg_exec)
{
	zval **query, **pgsql_link = NULL;
	int id = -1;
	PGconn *pgsql;
	PGresult *pgsql_result;
	ExecStatusType status;
	pgsql_result_handle *pg_result;

	switch(ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &query)==FAILURE) {
				RETURN_FALSE;
			}
			id = PGG(default_link);
			CHECK_DEFAULT_LINK(id);
			break;
		case 2:
			if (zend_get_parameters_ex(2, &pgsql_link, &query)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	convert_to_string_ex(query);
	pgsql_result = PQexec(pgsql, Z_STRVAL_PP(query));
	
	if (pgsql_result) {
		status = PQresultStatus(pgsql_result);
	} else {
		status = (ExecStatusType) PQstatus(pgsql);
	}
	
	
	switch (status) {
		case PGRES_EMPTY_QUERY:
		case PGRES_BAD_RESPONSE:
		case PGRES_NONFATAL_ERROR:
		case PGRES_FATAL_ERROR:
			php_error(E_WARNING, "PostgreSQL query failed:  %s", PQerrorMessage(pgsql));
			RETURN_FALSE;
			break;
		case PGRES_COMMAND_OK: /* successful command that did not return rows */
		default:
			if (pgsql_result) {
				pg_result = (pgsql_result_handle *) emalloc(sizeof(pgsql_result_handle));
				pg_result->conn = pgsql;
				pg_result->result = pgsql_result;
				pg_result->row = -1;
				ZEND_REGISTER_RESOURCE(return_value, pg_result, le_result);
				/*
				return_value->value.lval = zend_list_insert(pg_result,le_result);
				return_value->type = IS_LONG;
				*/
			} else {
				RETURN_FALSE;
			}
			break;
	}
}
/* }}} */

/* {{{ proto int pg_end_copy([int connection])
   Sync with backend. Completes the Copy command */
PHP_FUNCTION(pg_end_copy)
{
	zval **pgsql_link = NULL;
	int id = -1;
	PGconn *pgsql;
	int result = 0;

	switch(ZEND_NUM_ARGS()) {
		case 0:
			id = PGG(default_link);
			CHECK_DEFAULT_LINK(id);
			break;
		case 1:
			if (zend_get_parameters_ex(1, &pgsql_link)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	result = PQendcopy(pgsql);

	if (result!=0) {
		php_error(E_WARNING, "PostgreSQL query failed:  %s", PQerrorMessage(pgsql));
		RETURN_FALSE;
   }
   RETURN_TRUE;
}
/* }}} */

/* {{{ proto int pg_put_line([int connection,] string query)
   Send null-terminated string to backend server*/
PHP_FUNCTION(pg_put_line)
{
	zval **query, **pgsql_link = NULL;
	int id = -1;
	PGconn *pgsql;
	int result = 0;

	switch(ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &query)==FAILURE) {
				RETURN_FALSE;
			}
			id = PGG(default_link);
			CHECK_DEFAULT_LINK(id);
			break;
		case 2:
			if (zend_get_parameters_ex(2, &pgsql_link, &query)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	convert_to_string_ex(query);
	result = PQputline(pgsql, Z_STRVAL_PP(query));

	if (result==EOF) {
		php_error(E_WARNING, "PostgreSQL query failed:  %s", PQerrorMessage(pgsql));
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */
#define PHP_PG_NUM_ROWS 1
#define PHP_PG_NUM_FIELDS 2
#define PHP_PG_CMD_TUPLES 3

/* {{{ php_pgsql_get_result_info
 */
void php_pgsql_get_result_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type)
{
	zval **result;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(pg_result, pgsql_result_handle *, result, -1, "PostgreSQL result", le_result);

	pgsql_result = pg_result->result;
	
	switch (entry_type) {
		case PHP_PG_NUM_ROWS:
			return_value->value.lval = PQntuples(pgsql_result);
			break;
		case PHP_PG_NUM_FIELDS:
			return_value->value.lval = PQnfields(pgsql_result);
			break;
		case PHP_PG_CMD_TUPLES:
#if HAVE_PQCMDTUPLES
			return_value->value.lval = atoi(PQcmdTuples(pgsql_result));
#else
			php_error(E_WARNING,"This compilation does not support pg_cmdtuples()");
			return_value->value.lval = 0;
#endif
			break;
		default:
			RETURN_FALSE;
	}
	return_value->type = IS_LONG;
}
/* }}} */

/* {{{ proto int pg_numrows(int result)
   Return the number of rows in the result */
PHP_FUNCTION(pg_numrows)
{
	php_pgsql_get_result_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_NUM_ROWS);
}
/* }}} */

/* {{{ proto int pg_numfields(int result)
   Return the number of fields in the result */
PHP_FUNCTION(pg_numfields)
{
	php_pgsql_get_result_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_NUM_FIELDS);
}
/* }}} */

/* {{{ proto int pg_cmdtuples(int result)
   Returns the number of affected tuples */
PHP_FUNCTION(pg_cmdtuples)
{
	php_pgsql_get_result_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_CMD_TUPLES);
}
/* }}} */

/* {{{ proto int pg_last_notice(int connection)
   Returns the last notice set by the backend */
PHP_FUNCTION(pg_last_notice) 
{
	if (PGG(last_notice) == NULL) {
		RETURN_FALSE;
	} else {       
		RETURN_STRING(PGG(last_notice),0);
	}
}
/* }}} */

/* {{{ get_field_name
 */
char *get_field_name(PGconn *pgsql, Oid oid, HashTable *list)
{
	PGresult *result;
	char hashed_oid_key[32];
	list_entry *field_type;
	char *ret=NULL;

	/* try to lookup the type in the resource list */
	snprintf(hashed_oid_key,31,"pgsql_oid_%d",(int) oid);
	hashed_oid_key[31]=0;

	if (zend_hash_find(list,hashed_oid_key,strlen(hashed_oid_key)+1,(void **) &field_type)==SUCCESS) {
		ret = estrdup((char *)field_type->ptr);
	} else { /* hash all oid's */
		int i,num_rows;
		int oid_offset,name_offset;
		char *tmp_oid, *tmp_name;
		list_entry new_oid_entry;

		if ((result = PQexec(pgsql,"select oid,typname from pg_type")) == NULL) {
			return empty_string;
		}
		num_rows = PQntuples(result);
		oid_offset = PQfnumber(result,"oid");
		name_offset = PQfnumber(result,"typname");
		
		for (i=0; i<num_rows; i++) {
			if ((tmp_oid = PQgetvalue(result,i,oid_offset))==NULL) {
				continue;
			}
			snprintf(hashed_oid_key,31,"pgsql_oid_%s",tmp_oid);
			if ((tmp_name = PQgetvalue(result,i,name_offset))==NULL) {
				continue;
			}
			new_oid_entry.type = le_string;
			new_oid_entry.ptr = estrdup(tmp_name);
			zend_hash_update(list,hashed_oid_key,strlen(hashed_oid_key)+1,(void *) &new_oid_entry, sizeof(list_entry), NULL);
			if (!ret && atoi(tmp_oid)==oid) {
				ret = estrdup(tmp_name);
			}
		}
	}
	return ret;
}
/* }}} */			

#define PHP_PG_FIELD_NAME 1
#define PHP_PG_FIELD_SIZE 2
#define PHP_PG_FIELD_TYPE 3

/* {{{ php_pgsql_get_field_info
 */
void php_pgsql_get_field_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type)
{
	zval **result, **field;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;
	
	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &result, &field)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(pg_result, pgsql_result_handle *, result, -1, "PostgreSQL result", le_result);

	pgsql_result = pg_result->result;
	convert_to_long_ex(field);
	
	if (Z_LVAL_PP(field) < 0 || Z_LVAL_PP(field) >= PQnfields(pgsql_result)) {
		php_error(E_WARNING,"Bad field offset specified");
		RETURN_FALSE;
	}
	
	switch (entry_type) {
		case PHP_PG_FIELD_NAME:
			return_value->value.str.val = PQfname(pgsql_result, Z_LVAL_PP(field));
			return_value->value.str.len = strlen(return_value->value.str.val);
			return_value->value.str.val = estrndup(return_value->value.str.val,return_value->value.str.len);
			return_value->type = IS_STRING;
			break;
		case PHP_PG_FIELD_SIZE:
			return_value->value.lval = PQfsize(pgsql_result, Z_LVAL_PP(field));
			return_value->type = IS_LONG;
			break;
		case PHP_PG_FIELD_TYPE:
			return_value->value.str.val = get_field_name(pg_result->conn, PQftype(pgsql_result, Z_LVAL_PP(field)), &EG(regular_list));
			return_value->value.str.len = strlen(return_value->value.str.val);
			return_value->type = IS_STRING;
			break;
		default:
			RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string pg_fieldname(int result, int field_number)
   Returns the name of the field */
PHP_FUNCTION(pg_fieldname)
{
	php_pgsql_get_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_FIELD_NAME);
}
/* }}} */

/* {{{ proto int pg_fieldsize(int result, int field_number)
   Returns the internal size of the field */ 
PHP_FUNCTION(pg_fieldsize)
{
	php_pgsql_get_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_FIELD_SIZE);
}
/* }}} */

/* {{{ proto string pg_fieldtype(int result, int field_number)
   Returns the type name for the given field */
PHP_FUNCTION(pg_fieldtype)
{
	php_pgsql_get_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_FIELD_TYPE);
}
/* }}} */

/* {{{ proto int pg_fieldnum(int result, string field_name)
   Returns the field number of the named field */
PHP_FUNCTION(pg_fieldnum)
{
	zval **result, **field;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &result, &field)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(pg_result, pgsql_result_handle *, result, -1, "PostgreSQL result", le_result);

	pgsql_result = pg_result->result;
	
	convert_to_string_ex(field);
	return_value->value.lval = PQfnumber(pgsql_result, Z_STRVAL_PP(field));
	return_value->type = IS_LONG;
}
/* }}} */

/* {{{ proto mixed pg_result(int result, [int row_number,] mixed field_name)
   Returns values from a result identifier */
PHP_FUNCTION(pg_result)
{
	zval **result, **row, **field=NULL;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;
	int field_offset, pgsql_row;
	
	if ((ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &result, &row, &field)==FAILURE) &&
	    (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &result, &field)==FAILURE)) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(pg_result, pgsql_result_handle *, result, -1, "PostgreSQL result", le_result);

	pgsql_result = pg_result->result;
	if (ZEND_NUM_ARGS() == 2) {
		if (pg_result->row < 0)
			pg_result->row = 0;
		pgsql_row = pg_result->row;
		if (pgsql_row >= PQntuples(pgsql_result)) {
			RETURN_FALSE;
		}
	} else {
		convert_to_long_ex(row);
		pgsql_row = Z_LVAL_PP(row);
		if (pgsql_row < 0 || pgsql_row >= PQntuples(pgsql_result)) {
			php_error(E_WARNING,"Unable to jump to row %d on PostgreSQL result index %d", Z_LVAL_PP(row), Z_LVAL_PP(result));
			RETURN_FALSE;
		}
	}
	switch(Z_TYPE_PP(field)) {
		case IS_STRING:
			field_offset = PQfnumber(pgsql_result, Z_STRVAL_PP(field));
			break;
		default:
			convert_to_long_ex(field);
			field_offset = Z_LVAL_PP(field);
			break;
	}
	if (field_offset<0 || field_offset>=PQnfields(pgsql_result)) {
		php_error(E_WARNING,"Bad column offset specified");
		RETURN_FALSE;
	}
	
	if (PQgetisnull(pgsql_result, pgsql_row, field_offset)) {
		return_value->type = IS_NULL;
	} else {
		return_value->value.str.val = PQgetvalue(pgsql_result, pgsql_row, field_offset);
		return_value->value.str.len = (return_value->value.str.val ? strlen(return_value->value.str.val) : 0);
		return_value->value.str.val = safe_estrndup(return_value->value.str.val,return_value->value.str.len);
		return_value->type = IS_STRING;
	}
}
/* }}} */

/* {{{ void php_pgsql_fetch_hash
 */
static void php_pgsql_fetch_hash(INTERNAL_FUNCTION_PARAMETERS, int result_type)
{
	zval **result, **row, **arg3;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;
	int i, num_fields, pgsql_row;
	char *element, *field_name;
	uint element_len;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &result)==FAILURE) {
				RETURN_FALSE;
			}
			if (!result_type) {
				result_type = PGSQL_BOTH;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &result, &row)==FAILURE) {
				RETURN_FALSE;
			}
			if (!result_type) {
				result_type = PGSQL_BOTH;
			}
			break;
		case 3:
			if (zend_get_parameters_ex(3, &result, &row, &arg3)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long_ex(arg3);
			result_type = Z_LVAL_PP(arg3);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE(pg_result, pgsql_result_handle *, result, -1, "PostgreSQL result", le_result);

	pgsql_result = pg_result->result;

	if (ZEND_NUM_ARGS() == 1) {
		pg_result->row++;
		pgsql_row = pg_result->row;
		if (pgsql_row < 0 || pgsql_row >= PQntuples(pgsql_result)) {
			RETURN_FALSE;
		}
	} else {
		convert_to_long_ex(row);
		pgsql_row = Z_LVAL_PP(row);
		pg_result->row = pgsql_row;
		if (pgsql_row < 0 || pgsql_row >= PQntuples(pgsql_result)) {
			php_error(E_WARNING,"Unable to jump to row %d on PostgreSQL result index %d", Z_LVAL_PP(row), Z_LVAL_PP(result));
			RETURN_FALSE;
		}
	}
	array_init(return_value);
	for (i = 0, num_fields = PQnfields(pgsql_result); i<num_fields; i++) {
		if (PQgetisnull(pgsql_result, pgsql_row, i)) {
			if (result_type & PGSQL_NUM) {
				add_index_null(return_value, i);
			}
			if (result_type & PGSQL_ASSOC) {
				field_name = PQfname(pgsql_result, i);
				add_assoc_null(return_value, field_name);
			}
		} else {
			element = PQgetvalue(pgsql_result, pgsql_row, i);
			element_len = (element ? strlen(element) : 0);
			if (element) {
				char *data;
				int data_len;
				int should_copy=0;

				if (PG(magic_quotes_runtime)) {
					data = php_addslashes(element, element_len, &data_len, 0 TSRMLS_CC);
				} else {
					data = safe_estrndup(element, element_len);
					data_len = element_len;
				}
			
				if (result_type & PGSQL_NUM) {
					add_index_stringl(return_value, i, data, data_len, should_copy);
					should_copy=1;
				}
			
				if (result_type & PGSQL_ASSOC) {
					field_name = PQfname(pgsql_result, i);
					add_assoc_stringl(return_value, field_name, data, data_len, should_copy);
				}
			}
		}
	}
}
/* }}} */

/* {{{ proto array pg_fetch_row(int result, [int row])
   Get a row as an enumerated array */ 
PHP_FUNCTION(pg_fetch_row)
{
	php_pgsql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, PGSQL_NUM);
}
/* }}} */

/* ??  This is a rather odd function - why not just point pg_fetcharray() directly at fetch_hash ? -RL */
/* {{{ proto array pg_fetch_array(int result, [int row [, int result_type]])
   Fetch a row as an array */
PHP_FUNCTION(pg_fetch_array)
{
	php_pgsql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto object pg_fetch_object(int result, [int row [, int result_type]])
   Fetch a row as an object */
PHP_FUNCTION(pg_fetch_object)
{
	php_pgsql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
	if (return_value->type==IS_ARRAY) {
		object_and_properties_init(return_value, &zend_standard_class_def, return_value->value.ht);
	}
}
/* }}} */

#define PHP_PG_DATA_LENGTH 1
#define PHP_PG_DATA_ISNULL 2

/* {{{ php_pgsql_data_info
 */
void php_pgsql_data_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type)
{
	zval **result, **row, **field;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;
	int field_offset, pgsql_row;
	
	if ((ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &result, &row, &field)==FAILURE) &&
	    (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &result, &field)==FAILURE)) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(pg_result, pgsql_result_handle *, result, -1, "PostgreSQL result", le_result);

	pgsql_result = pg_result->result;
	if (ZEND_NUM_ARGS() == 2) {
		if (pg_result->row < 0)
			pg_result->row = 0;
		pgsql_row = pg_result->row;
		if (pgsql_row < 0 || pgsql_row >= PQntuples(pgsql_result)) {
			RETURN_FALSE;
		}
	} else {
		convert_to_long_ex(row);
		pgsql_row = Z_LVAL_PP(row);
		if (pgsql_row < 0 || pgsql_row >= PQntuples(pgsql_result)) {
			php_error(E_WARNING,"Unable to jump to row %d on PostgreSQL result index %d", Z_LVAL_PP(row), Z_LVAL_PP(result));
			RETURN_FALSE;
		}
	}
	
	switch(Z_TYPE_PP(field)) {
		case IS_STRING:
			convert_to_string_ex(field);
			field_offset = PQfnumber(pgsql_result, Z_STRVAL_PP(field));
			break;
		default:
			convert_to_long_ex(field);
			field_offset = Z_LVAL_PP(field);
			break;
	}
	if (field_offset < 0 || field_offset >= PQnfields(pgsql_result)) {
		php_error(E_WARNING, "Bad column offset specified");
		RETURN_FALSE;
	}
	
	switch (entry_type) {
		case PHP_PG_DATA_LENGTH:
			return_value->value.lval = PQgetlength(pgsql_result, pgsql_row, field_offset);
			break;
		case PHP_PG_DATA_ISNULL:
			return_value->value.lval = PQgetisnull(pgsql_result, pgsql_row, field_offset);
			break;
	}
	return_value->type = IS_LONG;
}
/* }}} */

/* {{{ proto int pg_fieldprtlen(int result, [int row,] mixed field_name_or_number)
   Returns the printed length */
PHP_FUNCTION(pg_fieldprtlen)
{
	php_pgsql_data_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_PG_DATA_LENGTH);
}
/* }}} */

/* {{{ proto int pg_fieldisnull(int result, [int row,] mixed field_name_or_number)
   Test if a field is NULL */
PHP_FUNCTION(pg_fieldisnull)
{
	php_pgsql_data_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_PG_DATA_ISNULL);
}
/* }}} */

/* {{{ proto int pg_freeresult(int result)
   Free result memory */
PHP_FUNCTION(pg_freeresult)
{
	zval **result;
	pgsql_result_handle *pg_result;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pg_result, pgsql_result_handle *, result, -1, "PostgreSQL result", le_result);
	if (Z_LVAL_PP(result) == 0) {
		RETURN_FALSE;
	}
	zend_list_delete(Z_LVAL_PP(result));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int pg_getlastoid(int result)
   Returns the last object identifier */
PHP_FUNCTION(pg_getlastoid)
{
	zval **result;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(pg_result, pgsql_result_handle *, result, -1, "PostgreSQL result", le_result);
	pgsql_result = pg_result->result;
#ifndef HAVE_PQOIDVALUE
	return_value->value.str.val = (char *) PQoidStatus(pgsql_result);
	if (return_value->value.str.val) {
		return_value->value.str.len = strlen(return_value->value.str.val);
		return_value->value.str.val = estrndup(return_value->value.str.val, return_value->value.str.len);
		return_value->type = IS_STRING;
	} else {
		return_value->value.str.val = empty_string;
	}
#else
	return_value->value.lval = (int) PQoidValue(pgsql_result);
	if (return_value->value.lval == InvalidOid) {
		RETURN_FALSE;
	} else {
		return_value->type = IS_LONG;
	}
#endif
}
/* }}} */

/* {{{ proto bool pg_trace(string filename [, string mode [, resource connection]])
   Enable tracing a PostgreSQL connection */
PHP_FUNCTION(pg_trace)
{
	zval **z_filename, **z_mode, **z_pgsql_link = NULL;
	int id = -1;
	PGconn *pgsql;
	char *mode = "w";
	int issock, socketd;
	FILE *fp;

	id = PGG(default_link);

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &z_filename)==FAILURE) {
				RETURN_FALSE;
			}
			CHECK_DEFAULT_LINK(id);
			break;
		case 2:
			if (zend_get_parameters_ex(2, &z_filename, &z_mode)==FAILURE) {
				RETURN_FALSE;
			}
			CHECK_DEFAULT_LINK(id);
			convert_to_string_ex(z_mode);
			mode = Z_STRVAL_PP(z_mode);
			break;
		case 3:
			if (zend_get_parameters_ex(3, &z_filename, &z_mode, &z_pgsql_link)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_string_ex(z_mode);
			mode = Z_STRVAL_PP(z_mode);
			break;
		default:
			ZEND_WRONG_PARAM_COUNT();
			break;
	}
		
    ZEND_FETCH_RESOURCE2(pgsql, PGconn *, z_pgsql_link, id, "PostgreSQL link", le_link, le_plink);
	convert_to_string_ex(z_filename);

	fp = php_fopen_wrapper(Z_STRVAL_PP(z_filename), mode, ENFORCE_SAFE_MODE, &issock, &socketd, NULL TSRMLS_CC);

	if (!fp) {
		php_error(E_WARNING, "Unable to open %s for logging", Z_STRVAL_PP(z_filename));
		RETURN_FALSE;
	}
	
	PQtrace(pgsql, fp);
	ZEND_REGISTER_RESOURCE(NULL, fp, php_file_le_fopen());
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool pg_untrace([int connection])
   Disable tracing of a PostgreSQL connection */
PHP_FUNCTION(pg_untrace)
{
	zval **pgsql_link = NULL;
	int id = -1;
	PGconn *pgsql;

	switch (ZEND_NUM_ARGS()) {
		case 0:
			id = PGG(default_link);
			CHECK_DEFAULT_LINK(id);
			break;
		case 1:
			if (zend_get_parameters_ex(1, &pgsql_link)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			ZEND_WRONG_PARAM_COUNT();
			break;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, pgsql_link, id, "PostgreSQL link", le_link, le_plink);
	PQuntrace(pgsql);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int pg_locreate(int connection)
   Create a large object */
PHP_FUNCTION(pg_locreate)
{
  	zval **pgsql_link = NULL;
	PGconn *pgsql;
	Oid pgsql_oid;
	int id = -1;

	switch(ZEND_NUM_ARGS()) {
		case 0:
			id = PGG(default_link);
			CHECK_DEFAULT_LINK(id);
			break;
		case 1:
			if (zend_get_parameters_ex(1, &pgsql_link)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, pgsql_link, id, "PostgreSQL link", le_link, le_plink);
	
	/* XXX: Archive modes not supported until I get some more data. Don't think anybody's
	   using it anyway. I believe it's also somehow related to the 'time travel' feature of
	   PostgreSQL, that's on the list of features to be removed... Create modes not supported.
	   What's the use of an object that can be only written to, but not read from, and vice
	   versa? Beats me... And the access type (r/w) must be specified again when opening
	   the object, probably (?) overrides this. (Jouni) 
	 */

	if ((pgsql_oid = lo_creat(pgsql, INV_READ|INV_WRITE))==0) {
		php_error(E_WARNING,"Unable to create PostgreSQL large object");
		RETURN_FALSE;
	}

	return_value->value.lval = pgsql_oid;
	return_value->type = IS_LONG;
}
/* }}} */

/* {{{ proto void pg_lounlink([int connection,] int large_obj_id)
   Delete a large object */
PHP_FUNCTION(pg_lounlink)
{
	zval **pgsql_link = NULL, **oid;
	PGconn *pgsql;
	Oid pgsql_oid;
	int id = -1;

	switch(ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &oid)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long_ex(oid);
			pgsql_oid = Z_LVAL_PP(oid);
			id = PGG(default_link);
			CHECK_DEFAULT_LINK(id);
			break;
		case 2:
			if (zend_get_parameters_ex(2, &pgsql_link, &oid)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long_ex(oid);
			pgsql_oid = Z_LVAL_PP(oid);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, pgsql_link, id, "PostgreSQL link", le_link, le_plink);
	
	if (lo_unlink(pgsql, pgsql_oid) == -1) {
		php_error(E_WARNING, "Unable to delete PostgreSQL large object %d", (int) pgsql_oid);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int pg_loopen([int connection,] int objoid, string mode)
   Open a large object and return fd */
PHP_FUNCTION(pg_loopen)
{
	zval **pgsql_link = NULL, **oid, **mode;
	PGconn *pgsql;
	Oid pgsql_oid;
	int id = -1, pgsql_mode=0, pgsql_lofd;
	int create=0;
	char *mode_string=NULL;
	pgLofp *pgsql_lofp;

	switch(ZEND_NUM_ARGS()) {
		case 2:
			if (zend_get_parameters_ex(2, &oid, &mode)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long_ex(oid);
			pgsql_oid = Z_LVAL_PP(oid);
			convert_to_string_ex(mode);
			mode_string = Z_STRVAL_PP(mode);
			id = PGG(default_link);
			CHECK_DEFAULT_LINK(id);
			break;
		case 3:
			if (zend_get_parameters_ex(3, &pgsql_link, &oid, &mode)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long_ex(oid);
			pgsql_oid = Z_LVAL_PP(oid);
			convert_to_string_ex(mode);
			mode_string = Z_STRVAL_PP(mode);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, pgsql_link, id, "PostgreSQL link", le_link, le_plink);
	
	/* r/w/+ is little bit more PHP-like than INV_READ/INV_WRITE and a lot of
	   faster to type. Unfortunately, doesn't behave the same way as fopen()...
	   (Jouni)
	 */

	if (strchr(mode_string, 'r') == mode_string) {
		pgsql_mode |= INV_READ;
		if (strchr(mode_string, '+') == mode_string+1) {
			pgsql_mode |= INV_WRITE;
		}
	}
	if (strchr(mode_string, 'w') == mode_string) {
		pgsql_mode |= INV_WRITE;
		create = 1;
		if (strchr(mode_string, '+') == mode_string+1) {
			pgsql_mode |= INV_READ;
		}
	}


	pgsql_lofp = (pgLofp *) emalloc(sizeof(pgLofp));

	if ((pgsql_lofd = lo_open(pgsql, pgsql_oid, pgsql_mode)) == -1) {
		if (create) {
			if ((pgsql_oid = lo_creat(pgsql, INV_READ|INV_WRITE)) == 0) {
				efree(pgsql_lofp);
				php_error(E_WARNING,"Unable to create PostgreSQL large object");
				RETURN_FALSE;
			} else {
				if ((pgsql_lofd = lo_open(pgsql, pgsql_oid, pgsql_mode)) == -1) {
					if (lo_unlink(pgsql, pgsql_oid) == -1) {
						efree(pgsql_lofp);
						php_error(E_WARNING,"Something's really messed up!!! Your database is badly corrupted in a way NOT related to PHP.");
						RETURN_FALSE;
					}
					efree(pgsql_lofp);
					php_error(E_WARNING,"Unable to open PostgreSQL large object");
					RETURN_FALSE;
				} else {
					pgsql_lofp->conn = pgsql;
					pgsql_lofp->lofd = pgsql_lofd;
					return_value->value.lval = zend_list_insert(pgsql_lofp, le_lofp);
					return_value->type = IS_LONG;
				}
			}
		} else {
			efree(pgsql_lofp);
			php_error(E_WARNING,"Unable to open PostgreSQL large object");
			RETURN_FALSE;
		}
	} else {
		pgsql_lofp->conn = pgsql;
		pgsql_lofp->lofd = pgsql_lofd;
		ZEND_REGISTER_RESOURCE(return_value, pgsql_lofp, le_lofp);
		/*
		return_value->value.lval = zend_list_insert(pgsql_lofp, le_lofp);
		return_value->type = IS_LONG;
		*/
	}
}
/* }}} */

/* {{{ proto void pg_loclose(int fd)
   Close a large object */
PHP_FUNCTION(pg_loclose)
{
	zval **pgsql_lofp;
	pgLofp *pgsql;

	switch(ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &pgsql_lofp)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	ZEND_FETCH_RESOURCE(pgsql, pgLofp *, pgsql_lofp, -1, "PostgreSQL large object", le_lofp);
	
	if (lo_close((PGconn *)pgsql->conn, pgsql->lofd) < 0) {
		php_error(E_WARNING,"Unable to close PostgreSQL large object descriptor %d", pgsql->lofd);
		RETVAL_FALSE;
	} else {
		RETVAL_TRUE;
	}

	zend_list_delete(Z_LVAL_PP(pgsql_lofp));
	return;
}
/* }}} */

/* {{{ proto string pg_loread(int fd [, int len])
   Read a large object */
PHP_FUNCTION(pg_loread)
{
  	zval **pgsql_id, **len;
	int buf_len = 1024, nbytes;
	char *buf;
	pgLofp *pgsql;

	if (ZEND_NUM_ARGS() < 1 || ZEND_NUM_ARGS() > 2 ||
	    zend_get_parameters_ex(ZEND_NUM_ARGS(), &pgsql_id, &len) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pgsql, pgLofp *, pgsql_id, -1, "PostgreSQL large object", le_lofp);

	if (ZEND_NUM_ARGS() > 1) {
		convert_to_long_ex(len);
		buf_len = Z_LVAL_PP(len);
	}
	
	buf = (char *) emalloc(sizeof(char)*(buf_len+1));
	if ((nbytes = lo_read((PGconn *)pgsql->conn, pgsql->lofd, buf, buf_len))<0) {
		efree(buf);
		RETURN_FALSE;
	}

	buf[nbytes] = 0;
	RETURN_STRINGL(buf, nbytes, 0);
}
/* }}} */

/* {{{ proto int pg_lowrite(int fd, string buf [, int len])
   Write a large object */
PHP_FUNCTION(pg_lowrite)
{
  	zval **pgsql_id, **str, **z_len;
	int nbytes;
	int len;
	pgLofp *pgsql;
	int argc = ZEND_NUM_ARGS();

	if (argc < 2 || argc > 3 ||
	    zend_get_parameters_ex(argc, &pgsql_id, &str, &z_len) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(str);

	if (argc > 2) {
		convert_to_long_ex(z_len);
		len = Z_LVAL_PP(z_len);
	}
	else {
		len = Z_STRLEN_PP(str);
	}

	ZEND_FETCH_RESOURCE(pgsql, pgLofp *, pgsql_id, -1, "PostgreSQL large object", le_lofp);

	if ((nbytes = lo_write((PGconn *)pgsql->conn, pgsql->lofd, Z_STRVAL_PP(str), len)) == -1) {
		RETURN_FALSE;
	}

	RETURN_LONG(nbytes);
}
/* }}} */

/* {{{ proto void pg_loreadall(int fd)
   Read a large object and send straight to browser */
PHP_FUNCTION(pg_loreadall)
{
  	zval **pgsql_id;
	int i, tbytes;
	volatile int nbytes;
	char buf[8192];
	pgLofp *pgsql;
	int output=1;

	switch(ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &pgsql_id)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	ZEND_FETCH_RESOURCE(pgsql, pgLofp *, pgsql_id, -1, "PostgreSQL large object", le_lofp);

	tbytes = 0;
	while ((nbytes = lo_read((PGconn *)pgsql->conn, pgsql->lofd, buf, 8192))>0) {
		for(i=0; i<nbytes; i++) {
			if (output) { (void) PUTC(buf[i]); }
		}
		tbytes += i;
	}
	return_value->value.lval = tbytes;
	return_value->type = IS_LONG;
}
/* }}} */

/* {{{ proto int pg_loimport(string filename [, resource connection])
   Import large object direct from filesystem */
PHP_FUNCTION(pg_loimport)
{
	zval **pgsql_link = NULL, **file_in;
	int id = -1;
	PGconn *pgsql;
	Oid oid;
	
	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &file_in) == FAILURE) {
				RETURN_FALSE;
			}
			id = PGG(default_link);
			CHECK_DEFAULT_LINK(id);
			break;
		case 2:
			if (zend_get_parameters_ex(2, &file_in, &pgsql_link) == FAILURE) {
				RETURN_FALSE;
			}
			convert_to_string_ex(file_in);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	if (PG(safe_mode) &&(!php_checkuid(Z_STRVAL_PP(file_in), NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	oid = lo_import(pgsql, Z_STRVAL_PP(file_in));

	if (oid > 0) {
		RETURN_LONG((int)oid);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool pg_loexport(int objoid, string filename [, resource connection])
   Export large object direct to filesystem */
PHP_FUNCTION(pg_loexport)
{
	zval **pgsql_link = NULL, **oid_id, **file_out;
	int id = -1;
	Oid oid;
	PGconn *pgsql;
	
	switch (ZEND_NUM_ARGS()) {
		case 2:
			if (zend_get_parameters_ex(2, &oid_id, &file_out) == FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long_ex(oid_id);
			convert_to_string_ex(file_out);
			id = PGG(default_link);
			CHECK_DEFAULT_LINK(id);
			break;
		case 3:
			if (zend_get_parameters_ex(3, &oid_id, &file_out, &pgsql_link) == FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long_ex(oid_id);
			convert_to_string_ex(file_out);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	oid = (Oid) Z_LVAL_PP(oid_id);

	if (lo_export(pgsql, oid, Z_STRVAL_PP(file_out))) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

#if HAVE_PQCLIENTENCODING

/* {{{ proto int pg_set_client_encoding([int connection,] string encoding)
   Set client encoding */
PHP_FUNCTION(pg_set_client_encoding)
{
	zval **encoding, **pgsql_link = NULL;
	int id = -1;
	PGconn *pgsql;

	switch(ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &encoding)==FAILURE) {
				RETURN_FALSE;
			}
			id = PGG(default_link);
			CHECK_DEFAULT_LINK(id);
			break;
		case 2:
			if (zend_get_parameters_ex(2, &pgsql_link, &encoding)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	convert_to_string_ex(encoding);
	return_value->value.lval = PQsetClientEncoding(pgsql, Z_STRVAL_PP(encoding));
	return_value->type = IS_LONG;

}
/* }}} */

/* {{{ proto string pg_client_encoding([int connection])
   Get the current client encoding */
PHP_FUNCTION(pg_client_encoding)
{
	zval **pgsql_link = NULL;
	int id = -1;
	PGconn *pgsql;

	switch(ZEND_NUM_ARGS()) {
		case 0:
			id = PGG(default_link);
			CHECK_DEFAULT_LINK(id);
			break;
		case 1:
			if (zend_get_parameters_ex(1, &pgsql_link)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	/* Just do the same as found in PostgreSQL sources... */

#ifndef HAVE_PGSQL_WITH_MULTIBYTE_SUPPORT
#define pg_encoding_to_char(x) "SQL_ASCII"
#endif

	return_value->value.str.val 
		= (char *) pg_encoding_to_char(PQclientEncoding(pgsql));
	return_value->value.str.len = strlen(return_value->value.str.val);
	return_value->value.str.val = (char *) estrdup(return_value->value.str.val);
	return_value->type = IS_STRING;
}
/* }}} */
#endif
	
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 tw=78 fdm=marker
 * vim<600: sw=4 ts=4 tw=78
 */
