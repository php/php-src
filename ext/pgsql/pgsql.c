/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
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
   |          Jouni Ahto <jouni.ahto@exdec.fi>                            |
   |          Yasuo Ohgaki <yohgaki@php.net>                              |
   |          Youichi Iwakiri <yiwakiri@st.rim.or.jp> (pg_copy_*)         | 
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

#define PGSQL_ESCAPE_STRING   1
#define PGSQL_ESCAPE_BYTEA    2

#define CHECK_DEFAULT_LINK(x) if (x == -1) { php_error(E_WARNING, "%s: no PostgreSQL link opened yet", get_active_function_name(TSRMLS_C)); }

/* {{{ pgsql_functions[]
 */
function_entry pgsql_functions[] = {
	/* connection functions */
	PHP_FE(pg_connect,		NULL)
	PHP_FE(pg_pconnect,		NULL)
	PHP_FE(pg_close,		NULL)
	PHP_FE(pg_connection_status,	NULL)
	PHP_FE(pg_connection_busy,		NULL)
	PHP_FE(pg_connection_reset,		NULL)
	PHP_FE(pg_host,			NULL)
	PHP_FE(pg_dbname,		NULL)
	PHP_FE(pg_port,			NULL)
	PHP_FE(pg_tty,			NULL)
	PHP_FE(pg_options,		NULL)
	/* query functions */
	PHP_FE(pg_query,		NULL)
	PHP_FE(pg_send_query,	NULL)
	PHP_FE(pg_cancel_query, NULL)
	/* result functions */
	PHP_FE(pg_fetch_result,	NULL)
	PHP_FE(pg_fetch_row,	NULL)
	PHP_FE(pg_fetch_array,	NULL)
	PHP_FE(pg_fetch_object,	NULL)
	PHP_FE(pg_affected_rows,NULL)
	PHP_FE(pg_get_result,	NULL)
	PHP_FE(pg_result_status,NULL)
	PHP_FE(pg_free_result,	NULL)
	PHP_FE(pg_last_oid,	    NULL)
	PHP_FE(pg_num_rows,		NULL)
	PHP_FE(pg_num_fields,	NULL)
	PHP_FE(pg_field_name,	NULL)
	PHP_FE(pg_field_num,	NULL)
	PHP_FE(pg_field_size,	NULL)
	PHP_FE(pg_field_type,	NULL)
	PHP_FE(pg_field_prtlen,	NULL)
	PHP_FE(pg_field_is_null,NULL)
	/* error message functions */
	PHP_FE(pg_result_error, NULL)
	PHP_FE(pg_last_error,   NULL)
	PHP_FE(pg_last_notice,  NULL)
	/* copy functions */
	PHP_FE(pg_put_line,		NULL)
	PHP_FE(pg_end_copy,		NULL)
	PHP_FE(pg_copy_to,      NULL)
	PHP_FE(pg_copy_from,    NULL)
	/* debug functions */
	PHP_FE(pg_trace,		NULL)
	PHP_FE(pg_untrace,		NULL)
	/* large object functions */
	PHP_FE(pg_lo_create,	NULL)
	PHP_FE(pg_lo_unlink,	NULL)
	PHP_FE(pg_lo_open,		NULL)
	PHP_FE(pg_lo_close,		NULL)
	PHP_FE(pg_lo_read,		NULL)
	PHP_FE(pg_lo_write,		NULL)
	PHP_FE(pg_lo_read_all,	NULL)
	PHP_FE(pg_lo_import,	NULL)
	PHP_FE(pg_lo_export,	NULL)
	PHP_FE(pg_lo_seek,		NULL)
	PHP_FE(pg_lo_tell,		NULL)
	/* utility functions */
#if HAVE_PQESCAPE
	PHP_FE(pg_escape_string,NULL)
	PHP_FE(pg_escape_bytea, NULL)
#endif
#if HAVE_PQCLIENTENCODING
	PHP_FE(pg_client_encoding,		NULL)
	PHP_FE(pg_set_client_encoding,	NULL)
#endif
	/* aliases for downwards compatibility */
	PHP_FALIAS(pg_exec,          pg_query,          NULL)
	PHP_FALIAS(pg_getlastoid,    pg_last_oid,       NULL)
	PHP_FALIAS(pg_cmdtuples,	 pg_affected_rows,  NULL)
	PHP_FALIAS(pg_errormessage,	 pg_last_error,     NULL)
	PHP_FALIAS(pg_numrows,		 pg_num_rows,       NULL)
	PHP_FALIAS(pg_numfields,	 pg_num_fields,     NULL)
	PHP_FALIAS(pg_fieldname,	 pg_field_name,     NULL)
	PHP_FALIAS(pg_fieldsize,     pg_field_size,     NULL)
	PHP_FALIAS(pg_fieldtype,	 pg_field_type,     NULL)
	PHP_FALIAS(pg_fieldnum,	     pg_field_num,      NULL)
	PHP_FALIAS(pg_fieldprtlen,	 pg_field_prtlen,   NULL)
	PHP_FALIAS(pg_fieldisnull,	 pg_field_is_null,  NULL)
	PHP_FALIAS(pg_freeresult,    pg_free_result,    NULL)
	PHP_FALIAS(pg_result,	     pg_fetch_result,   NULL)
	PHP_FALIAS(pg_loreadall,	 pg_lo_read_all,    NULL)
	PHP_FALIAS(pg_locreate,	     pg_lo_create,      NULL)
	PHP_FALIAS(pg_lounlink,	     pg_lo_unlink,      NULL)
	PHP_FALIAS(pg_loopen,	     pg_lo_open,        NULL)
	PHP_FALIAS(pg_loclose,	     pg_lo_close,       NULL)
	PHP_FALIAS(pg_loread,	     pg_lo_read,        NULL)
	PHP_FALIAS(pg_lowrite,	     pg_lo_write,       NULL)
	PHP_FALIAS(pg_loimport,	     pg_lo_import,      NULL)
	PHP_FALIAS(pg_loexport,	     pg_lo_export,      NULL)
#if HAVE_PQCLIENTENCODING
	PHP_FALIAS(pg_clientencoding,		pg_client_encoding,		NULL)
	PHP_FALIAS(pg_setclientencoding,	pg_set_client_encoding,	NULL)
#endif
	{NULL, NULL, NULL} 
};
/* }}} */

/* {{{ pgsql_module_entry
 */
zend_module_entry pgsql_module_entry = {
	STANDARD_MODULE_HEADER,
	"pgsql",
	pgsql_functions,
	PHP_MINIT(pgsql),
	PHP_MSHUTDOWN(pgsql),
	PHP_RINIT(pgsql),
	PHP_RSHUTDOWN(pgsql),
	PHP_MINFO(pgsql),
    NO_VERSION_YET,
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
php_pgsql_globals pgsql_globals;
#endif

/* {{{ php_pgsql_set_default_link
 */
static void php_pgsql_set_default_link(int id TSRMLS_DC)
{   
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
	PGresult *res;

	while ((res = PQgetResult(link))) {
		PQclear(res);
	}
	PQfinish(link);
	PGG(num_links)--;
}
/* }}} */

/* {{{ _close_pgsql_plink
 */
static void _close_pgsql_plink(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	PGconn *link = (PGconn *)rsrc->ptr;
	PGresult *res;

	while ((res = PQgetResult(link))) {
		PQclear(res);
	}
	PQfinish(link);
	PGG(num_persistent)--;
	PGG(num_links)--;
}
/* }}} */

/* {{{ _notice_handler
 */
static void _notice_handler(void *arg, const char *message)
{
	TSRMLS_FETCH();

	if (! PGG(ignore_notices)) {
		php_log_err((char *) message TSRMLS_CC);
		if (PGG(last_notice)) {
			efree(PGG(last_notice));
		}
		PGG(last_notice_len) = strlen(message);
		PGG(last_notice) = estrndup(message, PGG(last_notice_len));
	}
}
/* }}} */

/* {{{ _rollback_transactions
 */
static int _rollback_transactions(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	PGconn *link;
	PGresult *res;

	if (Z_TYPE_P(rsrc) != le_plink) 
		return 0;

	link = (PGconn *) rsrc->ptr;
	
	PQsetnonblocking(link, 0);
	while ((res = PQgetResult(link))) {
		PQclear(res);
	}
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
	STD_PHP_INI_BOOLEAN("pgsql.allow_persistent",	"1",	PHP_INI_SYSTEM,		OnUpdateBool,		allow_persistent,	php_pgsql_globals,		pgsql_globals)
	STD_PHP_INI_ENTRY_EX("pgsql.max_persistent",	"-1",	PHP_INI_SYSTEM,		OnUpdateInt,		max_persistent,		php_pgsql_globals,		pgsql_globals,	display_link_numbers)
	STD_PHP_INI_ENTRY_EX("pgsql.max_links",		"-1",	PHP_INI_SYSTEM,			OnUpdateInt,		max_links,			php_pgsql_globals,		pgsql_globals,	display_link_numbers)
	STD_PHP_INI_BOOLEAN("pgsql.auto_reset_persistent",	"0",	PHP_INI_SYSTEM,		OnUpdateBool,		auto_reset_persistent,	php_pgsql_globals,		pgsql_globals)
PHP_INI_END()
/* }}} */

/* {{{ php_pgsql_init_globals
 */
static void php_pgsql_init_globals(php_pgsql_globals *pgsql_globals_p TSRMLS_DC)
{
	PGG(num_persistent) = 0;
	PGG(ignore_notices) = 0;
	PGG(auto_reset_persistent) = 0;
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
	le_result = zend_register_list_destructors_ex(_free_result, NULL, "pgsql result", module_number);
	le_lofp = zend_register_list_destructors_ex(_free_ptr, NULL, "pgsql large object", module_number);
	le_string = zend_register_list_destructors_ex(_free_ptr, NULL, "pgsql string", module_number);
	/* For pg_fetch_array() */
	REGISTER_LONG_CONSTANT("PGSQL_ASSOC", PGSQL_ASSOC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_NUM", PGSQL_NUM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_BOTH", PGSQL_BOTH, CONST_CS | CONST_PERSISTENT);
	/* For pg_connection_status() */
	REGISTER_LONG_CONSTANT("PGSQL_CONNECTION_BAD", CONNECTION_BAD, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_CONNECTION_OK", CONNECTION_OK, CONST_CS | CONST_PERSISTENT);
	/* For lo_seek() */
	REGISTER_LONG_CONSTANT("PGSQL_SEEK_SET", SEEK_SET, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_SEEK_CUR", SEEK_CUR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_SEEK_END", SEEK_END, CONST_CS | CONST_PERSISTENT);
	/* For pg_result_status() */
	REGISTER_LONG_CONSTANT("PGSQL_EMPTY_QUERY", PGRES_EMPTY_QUERY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_COMMAND_OK", PGRES_COMMAND_OK, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_TUPLES_OK", PGRES_TUPLES_OK, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_COPY_OUT", PGRES_COPY_OUT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_COPY_IN", PGRES_COPY_IN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_BAD_RESPONSE", PGRES_BAD_RESPONSE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_NONFATAL_ERROR", PGRES_NONFATAL_ERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_FATAL_ERROR", PGRES_FATAL_ERROR, CONST_CS | CONST_PERSISTENT);

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
	PGG(last_notice) = NULL;
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(pgsql)
{
	if (PGG(last_notice)) {
		efree(PGG(last_notice));
	}
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
static void php_pgsql_do_connect(INTERNAL_FUNCTION_PARAMETERS, int persistent)
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
			Z_TYPE(new_le) = le_plink;
			new_le.ptr = pgsql;
			if (zend_hash_update(&EG(persistent_list), hashed_details, hashed_details_length+1, (void *) &new_le, sizeof(list_entry), NULL)==FAILURE) {
				efree(hashed_details);
				RETURN_FALSE;
			}
			PGG(num_links)++;
			PGG(num_persistent)++;
		} else {  /* we do */
			if (Z_TYPE_P(le) != le_plink) {
				RETURN_FALSE;
			}
			/* ensure that the link did not die */
			if (PGG(auto_reset_persistent)) {
				/* need to send & get something from backend to
				   make sure we catch CONNECTION_BAD everytime */
				PGresult *pg_result;
				pg_result = PQexec(le->ptr, "select 1");
				PQclear(pg_result);
			}
			if (PQstatus(le->ptr)==CONNECTION_BAD) { /* the link died */
				if (le->ptr == NULL) {
					if (connstring) {
						le->ptr=PQconnectdb(connstring);
					} else {
						le->ptr=PQsetdb(host,port,options,tty,dbname);
					}
				}
				else {
					PQreset(le->ptr);
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

			if (Z_TYPE_P(index_ptr) != le_index_ptr) {
				RETURN_FALSE;
			}
			link = (int) (long) index_ptr->ptr;
			ptr = zend_list_find(link,&type);   /* check if the link is still there */
			if (ptr && (type==le_link || type==le_plink)) {
				Z_LVAL_P(return_value) = link;
				zend_list_addref(link);
				php_pgsql_set_default_link(link TSRMLS_CC);
				Z_TYPE_P(return_value) = IS_RESOURCE;
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
		new_index_ptr.ptr = (void *) Z_LVAL_P(return_value);
		Z_TYPE(new_index_ptr) = le_index_ptr;
		if (zend_hash_update(&EG(regular_list),hashed_details,hashed_details_length+1,(void *) &new_index_ptr, sizeof(list_entry), NULL)==FAILURE) {
			efree(hashed_details);
			RETURN_FALSE;
		}
		PGG(num_links)++;
	}
	efree(hashed_details);
	php_pgsql_set_default_link(Z_LVAL_P(return_value) TSRMLS_CC);
}
/* }}} */

#if 0
/* {{{ php_pgsql_get_default_link
 */
static int php_pgsql_get_default_link(INTERNAL_FUNCTION_PARAMETERS)
{
	if (PGG(default_link)==-1) { /* no link opened yet, implicitly open one */
		ht = 0;
		php_pgsql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);
	}
	return PGG(default_link);
}
/* }}} */
#endif

/* {{{ proto resource pg_connect([string connection_string] | [string host, string port [, string options [, string tty,]] string database)
   Open a PostgreSQL connection */
PHP_FUNCTION(pg_connect)
{
	php_pgsql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);
}
/* }}} */

/* {{{ proto resource pg_pconnect([string connection_string] | [string host, string port [, string options [, string tty,]] string database)
   Open a persistent PostgreSQL connection */
PHP_FUNCTION(pg_pconnect)
{
	php_pgsql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,1);
}
/* }}} */

/* {{{ proto bool pg_close([resource connection])
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
static void php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type)
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
			Z_STRVAL_P(return_value) = PQdb(pgsql);
			break;
		case PHP_PG_ERROR_MESSAGE:
			Z_STRVAL_P(return_value) = PQerrorMessage(pgsql);
			break;
		case PHP_PG_OPTIONS:
			Z_STRVAL_P(return_value) = PQoptions(pgsql);
			break;
		case PHP_PG_PORT:
			Z_STRVAL_P(return_value) = PQport(pgsql);
			break;
		case PHP_PG_TTY:
			Z_STRVAL_P(return_value) = PQtty(pgsql);
			break;
		case PHP_PG_HOST:
			Z_STRVAL_P(return_value) = PQhost(pgsql);
			break;
		default:
			RETURN_FALSE;
	}
	Z_STRLEN_P(return_value) = strlen(Z_STRVAL_P(return_value));
	Z_STRVAL_P(return_value) = (char *) estrdup(Z_STRVAL_P(return_value));
	Z_TYPE_P(return_value) = IS_STRING;
}
/* }}} */

/* {{{ proto string pg_dbname([resource connection])
   Get the database name */ 
PHP_FUNCTION(pg_dbname)
{
	php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_DBNAME);
}
/* }}} */

/* {{{ proto string pg_last_error([resource connection])
   Get the error message string */
PHP_FUNCTION(pg_last_error)
{
	php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_ERROR_MESSAGE);
}
/* }}} */

/* {{{ proto string pg_options([resource connection])
   Get the options associated with the connection */
PHP_FUNCTION(pg_options)
{
	php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_OPTIONS);
}
/* }}} */

/* {{{ proto int pg_port([resource connection])
   Return the port number associated with the connection */
PHP_FUNCTION(pg_port)
{
	php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_PORT);
}
/* }}} */

/* {{{ proto string pg_tty([resource connection])
   Return the tty name associated with the connection */
PHP_FUNCTION(pg_tty)
{
	php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_TTY);
}
/* }}} */

/* {{{ proto string pg_host([resource connection])
   Returns the host name associated with the connection */
PHP_FUNCTION(pg_host)
{
	php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_HOST);
}
/* }}} */

/* {{{ proto resource pg_query([resource connection,] string query)
   Execute a query */
PHP_FUNCTION(pg_query)
{
	zval **query, **pgsql_link = NULL;
	int id = -1;
	int leftover = 0;
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
	PQsetnonblocking(pgsql, 0);
	while ((pgsql_result = PQgetResult(pgsql))) {
		PQclear(pgsql_result);
		leftover = 1;
	}
	if (leftover) {
		php_error(E_NOTICE,"%s() found results on this connection. Use pg_get_result() to get results",
				  get_active_function_name(TSRMLS_C));
	}
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
			} else {
				RETURN_FALSE;
			}
			break;
	}
}
/* }}} */

#define PHP_PG_NUM_ROWS 1
#define PHP_PG_NUM_FIELDS 2
#define PHP_PG_CMD_TUPLES 3

/* {{{ php_pgsql_get_result_info
 */
static void php_pgsql_get_result_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type)
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
			Z_LVAL_P(return_value) = PQntuples(pgsql_result);
			break;
		case PHP_PG_NUM_FIELDS:
			Z_LVAL_P(return_value) = PQnfields(pgsql_result);
			break;
		case PHP_PG_CMD_TUPLES:
#if HAVE_PQCMDTUPLES
			Z_LVAL_P(return_value) = atoi(PQcmdTuples(pgsql_result));
#else
			php_error(E_WARNING,"This compilation does not support %s()",
					  get_active_function_name(TSRMLS_C));
			Z_LVAL_P(return_value) = 0;
#endif
			break;
		default:
			RETURN_FALSE;
	}
	Z_TYPE_P(return_value) = IS_LONG;
}
/* }}} */

/* {{{ proto int pg_num_rows(resource result)
   Return the number of rows in the result */
PHP_FUNCTION(pg_num_rows)
{
	php_pgsql_get_result_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_NUM_ROWS);
}
/* }}} */

/* {{{ proto int pg_num_fields(resource result)
   Return the number of fields in the result */
PHP_FUNCTION(pg_num_fields)
{
	php_pgsql_get_result_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_NUM_FIELDS);
}
/* }}} */

/* {{{ proto int pg_affected_rows(resource result)
   Returns the number of affected tuples */
PHP_FUNCTION(pg_affected_rows)
{
	php_pgsql_get_result_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_CMD_TUPLES);
}
/* }}} */

/* {{{ proto string pg_last_notice(resource connection)
   Returns the last notice set by the backend */
PHP_FUNCTION(pg_last_notice) 
{
	if (PGG(last_notice)) {
		RETURN_STRINGL(PGG(last_notice), PGG(last_notice_len), 0);
	} else {       
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ get_field_name
 */
static char *get_field_name(PGconn *pgsql, Oid oid, HashTable *list TSRMLS_DC)
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
			Z_TYPE(new_oid_entry) = le_string;
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
static void php_pgsql_get_field_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type)
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
			Z_STRVAL_P(return_value) = PQfname(pgsql_result, Z_LVAL_PP(field));
			Z_STRLEN_P(return_value) = strlen(Z_STRVAL_P(return_value));
			Z_STRVAL_P(return_value) = estrndup(Z_STRVAL_P(return_value),Z_STRLEN_P(return_value));
			Z_TYPE_P(return_value) = IS_STRING;
			break;
		case PHP_PG_FIELD_SIZE:
			Z_LVAL_P(return_value) = PQfsize(pgsql_result, Z_LVAL_PP(field));
			Z_TYPE_P(return_value) = IS_LONG;
			break;
		case PHP_PG_FIELD_TYPE:
			Z_STRVAL_P(return_value) = get_field_name(pg_result->conn, PQftype(pgsql_result, Z_LVAL_PP(field)), &EG(regular_list) TSRMLS_CC);
			Z_STRLEN_P(return_value) = strlen(Z_STRVAL_P(return_value));
			Z_TYPE_P(return_value) = IS_STRING;
			break;
		default:
			RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string pg_field_name(resource result, int field_number)
   Returns the name of the field */
PHP_FUNCTION(pg_field_name)
{
	php_pgsql_get_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_FIELD_NAME);
}
/* }}} */

/* {{{ proto int pg_field_size(resource result, int field_number)
   Returns the internal size of the field */ 
PHP_FUNCTION(pg_field_size)
{
	php_pgsql_get_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_FIELD_SIZE);
}
/* }}} */

/* {{{ proto string pg_field_type(resource result, int field_number)
   Returns the type name for the given field */
PHP_FUNCTION(pg_field_type)
{
	php_pgsql_get_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP_PG_FIELD_TYPE);
}
/* }}} */

/* {{{ proto int pg_field_num(resource result, string field_name)
   Returns the field number of the named field */
PHP_FUNCTION(pg_field_num)
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
	Z_LVAL_P(return_value) = PQfnumber(pgsql_result, Z_STRVAL_PP(field));
	Z_TYPE_P(return_value) = IS_LONG;
}
/* }}} */

/* {{{ proto mixed pg_fetch_result(resource result, [int row_number,] mixed field_name)
   Returns values from a result identifier */
PHP_FUNCTION(pg_fetch_result)
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
		Z_TYPE_P(return_value) = IS_NULL;
	} else {
		Z_STRVAL_P(return_value) = PQgetvalue(pgsql_result, pgsql_row, field_offset);
		Z_STRLEN_P(return_value) = (Z_STRVAL_P(return_value) ? strlen(Z_STRVAL_P(return_value)) : 0);
		Z_STRVAL_P(return_value) = safe_estrndup(Z_STRVAL_P(return_value),Z_STRLEN_P(return_value));
		Z_TYPE_P(return_value) = IS_STRING;
	}
}
/* }}} */

/* {{{ void php_pgsql_fetch_hash */
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
			if (zend_get_parameters_ex(1, &result) == FAILURE) {
				RETURN_FALSE;
			}
			if (!result_type) {
				result_type = PGSQL_BOTH;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &result, &row) == FAILURE) {
				RETURN_FALSE;
			}
			if (!result_type) {
				result_type = PGSQL_BOTH;
			}
			break;
		case 3:
			if (zend_get_parameters_ex(3, &result, &row, &arg3) == FAILURE) {
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
		if (Z_TYPE_PP(row) != IS_NULL) { 
			convert_to_long_ex(row);
			pgsql_row = Z_LVAL_PP(row);
			pg_result->row = pgsql_row;
			if (pgsql_row < 0 || pgsql_row >= PQntuples(pgsql_result)) {
				php_error(E_WARNING, "Unable to jump to row %d on PostgreSQL result index %d", Z_LVAL_PP(row), Z_LVAL_PP(result));
				RETURN_FALSE;
			}
		} else {
			/* If 2nd param is NULL, ignore it and use the normal way of accessing the next row */
			pg_result->row++;
			pgsql_row = pg_result->row;
			if (pgsql_row < 0 || pgsql_row >= PQntuples(pgsql_result)) {
				RETURN_FALSE;
			}
		}
	}
	array_init(return_value);
	for (i = 0, num_fields = PQnfields(pgsql_result); i < num_fields; i++) {
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

/* {{{ proto array pg_fetch_row(resource result [, int row])
   Get a row as an enumerated array */ 
PHP_FUNCTION(pg_fetch_row)
{
	php_pgsql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, PGSQL_NUM);
}
/* }}} */

/* {{{ proto array pg_fetch_array(resource result [, int row [, int result_type]])
   Fetch a row as an array */
PHP_FUNCTION(pg_fetch_array)
{
	php_pgsql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto object pg_fetch_object(resource result [, int row [, int result_type]])
   Fetch a row as an object */
PHP_FUNCTION(pg_fetch_object)
{
	php_pgsql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
	if (Z_TYPE_P(return_value)==IS_ARRAY) {
		object_and_properties_init(return_value, &zend_standard_class_def, Z_ARRVAL_P(return_value));
	}
}
/* }}} */

#define PHP_PG_DATA_LENGTH 1
#define PHP_PG_DATA_ISNULL 2

/* {{{ php_pgsql_data_info
 */
static void php_pgsql_data_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type)
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
			Z_LVAL_P(return_value) = PQgetlength(pgsql_result, pgsql_row, field_offset);
			break;
		case PHP_PG_DATA_ISNULL:
			Z_LVAL_P(return_value) = PQgetisnull(pgsql_result, pgsql_row, field_offset);
			break;
	}
	Z_TYPE_P(return_value) = IS_LONG;
}
/* }}} */

/* {{{ proto int pg_field_prtlen(resource result, [int row,] mixed field_name_or_number)
   Returns the printed length */
PHP_FUNCTION(pg_field_prtlen)
{
	php_pgsql_data_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_PG_DATA_LENGTH);
}
/* }}} */

/* {{{ proto int pg_field_is_null(resource result, [int row,] mixed field_name_or_number)
   Test if a field is NULL */
PHP_FUNCTION(pg_field_is_null)
{
	php_pgsql_data_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_PG_DATA_ISNULL);
}
/* }}} */

/* {{{ proto bool pg_free_result(resource result)
   Free result memory */
PHP_FUNCTION(pg_free_result)
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

/* {{{ proto int pg_last_oid(resource result)
   Returns the last object identifier */
PHP_FUNCTION(pg_last_oid)
{
	zval **result;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(pg_result, pgsql_result_handle *, result, -1, "PostgreSQL result", le_result);
	pgsql_result = pg_result->result;
#if HAVE_PQOIDVALUE
	Z_LVAL_P(return_value) = (int) PQoidValue(pgsql_result);
	if (Z_LVAL_P(return_value) == InvalidOid) {
		RETURN_FALSE;
	} else {
		Z_TYPE_P(return_value) = IS_LONG;
	}
#else
	Z_STRVAL_P(return_value) = (char *) PQoidStatus(pgsql_result);
	if (Z_STRVAL_P(return_value)) {
		Z_STRLEN_P(return_value) = strlen(Z_STRVAL_P(return_value));
		Z_STRVAL_P(return_value) = estrndup(Z_STRVAL_P(return_value), Z_STRLEN_P(return_value));
		Z_TYPE_P(return_value) = IS_STRING;
	} else {
		Z_STRVAL_P(return_value) = empty_string;
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

/* {{{ proto bool pg_untrace([resource connection])
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

/* {{{ proto int pg_lo_create(resource connection)
   Create a large object */
PHP_FUNCTION(pg_lo_create)
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

	Z_LVAL_P(return_value) = pgsql_oid;
	Z_TYPE_P(return_value) = IS_LONG;
}
/* }}} */

/* {{{ proto bool pg_lo_unlink([resource connection,] int large_object_oid)
   Delete a large object */
PHP_FUNCTION(pg_lo_unlink)
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

/* {{{ proto resource pg_lo_open([resource connection,] int large_object_oid, string mode)
   Open a large object and return fd */
PHP_FUNCTION(pg_lo_open)
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
					Z_LVAL_P(return_value) = zend_list_insert(pgsql_lofp, le_lofp);
					Z_TYPE_P(return_value) = IS_LONG;
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
		Z_LVAL_P(return_value) = zend_list_insert(pgsql_lofp, le_lofp);
		Z_TYPE_P(return_value) = IS_LONG;
		*/
	}
}
/* }}} */

/* {{{ proto bool pg_lo_close(resource large_object)
   Close a large object */
PHP_FUNCTION(pg_lo_close)
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

/* {{{ proto string pg_lo_read(resource large_object [, int len])
   Read a large object */
PHP_FUNCTION(pg_lo_read)
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

/* {{{ proto int pg_lo_write(resource large_object, string buf [, int len])
   Write a large object */
PHP_FUNCTION(pg_lo_write)
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

/* {{{ proto int pg_lo_read_all(resource large_object)
   Read a large object and send straight to browser */
PHP_FUNCTION(pg_lo_read_all)
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
	RETURN_LONG(tbytes);
}
/* }}} */

/* {{{ proto int pg_lo_import([resource connection, ] string filename)
   Import large object direct from filesystem */
PHP_FUNCTION(pg_lo_import)
{
	zval *pgsql_link = NULL;
	char *file_in;
	int id = -1, name_len;
	int argc = ZEND_NUM_ARGS();
	PGconn *pgsql;
	Oid oid;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
								 "rs", &pgsql_link, &file_in, &name_len) == SUCCESS) {
		;
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
									 "s", &file_in, &name_len) == SUCCESS) {
		id = PGG(default_link);
		CHECK_DEFAULT_LINK(id);
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
									 "rs", &pgsql_link, &file_in, &name_len) == SUCCESS) {
		php_error(E_NOTICE, "Old API for %s() is used.", get_active_function_name(TSRMLS_C));
	}
	else {
		WRONG_PARAM_COUNT;
	}
	
	if (PG(safe_mode) &&(!php_checkuid(file_in, NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	oid = lo_import(pgsql, file_in);

	if (oid > 0) {
		RETURN_LONG((int)oid);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool pg_lo_export([resource connection, ] int objoid, string filename)
   Export large object direct to filesystem */
PHP_FUNCTION(pg_lo_export)
{
	zval *pgsql_link = NULL;
	char *file_out;
	int id = -1, name_len, oid_id;
	int argc = ZEND_NUM_ARGS();
	Oid oid;
	PGconn *pgsql;
	
	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
								 "rls", &pgsql_link, &oid_id, &file_out, &name_len) == SUCCESS) {
		;
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
									 "ls",  &oid_id, &file_out, &name_len) == SUCCESS) {
		id = PGG(default_link);
		CHECK_DEFAULT_LINK(id);
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
									 "lsr", &oid_id, &file_out, &name_len, &pgsql_link) == SUCCESS) {
		php_error(E_NOTICE, "Old API for %s() is used.", get_active_function_name(TSRMLS_C));
	}
	else {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	oid = (Oid) oid_id;

	if (lo_export(pgsql, oid, file_out)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool pg_lo_seek(resource large_object, int offset [, int whence])
   Seeks position of large object */
PHP_FUNCTION(pg_lo_seek)
{
	zval *pgsql_id = NULL;
	int offset = 0, whence = SEEK_CUR;
	pgLofp *pgsql;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "rl|l", &pgsql_id, &offset, &whence) == FAILURE) {
		return;
	}
	if (whence != SEEK_SET && whence != SEEK_CUR && whence != SEEK_END) {
		php_error(E_WARNING, "Invalid whence parameter for %s()",
				  get_active_function_name(TSRMLS_C));
		return;
	}

	ZEND_FETCH_RESOURCE(pgsql, pgLofp *, &pgsql_id, -1, "PostgreSQL large object", le_lofp);

	if (lo_lseek((PGconn *)pgsql->conn, pgsql->lofd, offset, whence )) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int pg_lo_tell(resource large_object)
   Returns current position of large object */
PHP_FUNCTION(pg_lo_tell)
{
	zval *pgsql_id = NULL;
	int offset = 0;
	pgLofp *pgsql;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "r", &pgsql_id) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(pgsql, pgLofp *, &pgsql_id, -1, "PostgreSQL large object", le_lofp);

	offset = lo_tell((PGconn *)pgsql->conn, pgsql->lofd);
	RETURN_LONG(offset);
}
/* }}} */

#if HAVE_PQCLIENTENCODING
/* {{{ proto int pg_set_client_encoding([resource connection,] string encoding)
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
	Z_LVAL_P(return_value) = PQsetClientEncoding(pgsql, Z_STRVAL_PP(encoding));
	Z_TYPE_P(return_value) = IS_LONG;

}
/* }}} */

/* {{{ proto string pg_client_encoding([resource connection])
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

	Z_STRVAL_P(return_value) 
		= (char *) pg_encoding_to_char(PQclientEncoding(pgsql));
	Z_STRLEN_P(return_value) = strlen(Z_STRVAL_P(return_value));
	Z_STRVAL_P(return_value) = (char *) estrdup(Z_STRVAL_P(return_value));
	Z_TYPE_P(return_value) = IS_STRING;
}
/* }}} */
#endif


#define	COPYBUFSIZ	8192

/* {{{ proto bool pg_end_copy([resource connection])
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


/* {{{ proto bool pg_put_line([resource connection,] string query)
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

/* {{{ proto array pg_copy_to(int connection, string table_name [, string delimiter [, string null_as]])
   Copy table to array */
PHP_FUNCTION(pg_copy_to)
{
	zval *pgsql_link;
	char *table_name, *pg_delim = NULL, *pg_null_as = NULL;
	int table_name_len, pg_delim_len, pg_null_as_len;
	char *query;
	char *query_template = "COPY \"\" TO STDOUT DELIMITERS ':' WITH NULL AS ''";
	int id = -1;
	PGconn *pgsql;
	PGresult *pgsql_result;
	ExecStatusType status;
	int copydone = 0;
	char copybuf[COPYBUFSIZ];
	char *csv = (char *)NULL;
	int ret;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "rs|ss",
							  &pgsql_link, &table_name, &table_name_len,
							  &pg_delim, &pg_delim_len, &pg_null_as, &pg_null_as_len) == FAILURE) {
		return;
	}
	if (!pg_delim) {
		pg_delim = "\t";
	}
	if (!pg_null_as) {
		pg_null_as = safe_estrdup("\\\\N");
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	query = (char *)emalloc(strlen(query_template) + strlen(table_name) + strlen(pg_null_as) + 1);
	sprintf(query, "COPY \"%s\" TO STDOUT DELIMITERS '%s' WITH NULL AS '%s'",
				table_name,	pg_delim, pg_null_as);

	while ((pgsql_result = PQgetResult(pgsql))) {
		PQclear(pgsql_result);
	}
	pgsql_result = PQexec(pgsql, query);

	efree(pg_null_as);
	efree(query);

	if (pgsql_result) {
		status = PQresultStatus(pgsql_result);
	} else {
		status = (ExecStatusType) PQstatus(pgsql);
	}

	switch (status) {
		case PGRES_COPY_OUT:
			if (pgsql_result) {
				PQclear(pgsql_result);
				if (array_init(return_value) == FAILURE) {
					RETURN_FALSE;
				}
				while (!copydone)
				{
					if ((ret = PQgetline(pgsql, copybuf, COPYBUFSIZ))) {
						php_error(E_WARNING, "%s() query failed:  %s",
								  get_active_function_name(TSRMLS_C), PQerrorMessage(pgsql));
						RETURN_FALSE;
					}
			
					if (copybuf[0] == '\\' &&
						copybuf[1] == '.' &&
						copybuf[2] == '\0')
					{
						copydone = 1;
					}
					else
					{
						if (csv == (char *)NULL) {
							csv = estrdup(copybuf);
						} else {
							csv = (char *)erealloc(csv, strlen(csv) + sizeof(char)*(COPYBUFSIZ+1));
							strcat(csv, copybuf);
						}
							
						switch (ret)
						{
							case EOF:
								copydone = 1;
							case 0:
								add_next_index_string(return_value, csv, 1);
								efree(csv);
								csv = (char *)NULL;
								break;
							case 1:
								break;
						}
					}
				}
				if (PQendcopy(pgsql)) {
					php_error(E_WARNING, "%s() query failed:  %s",
							  get_active_function_name(TSRMLS_C), PQerrorMessage(pgsql));
					RETURN_FALSE;
				}
				while ((pgsql_result = PQgetResult(pgsql))) {
					PQclear(pgsql_result);
				}
			} else {
				PQclear(pgsql_result);
				RETURN_FALSE;
			}
			break;
		default:
			PQclear(pgsql_result);
			php_error(E_WARNING, "%s() query failed:  %s",
					  get_active_function_name(TSRMLS_C), PQerrorMessage(pgsql));
			RETURN_FALSE;
			break;
	}
}
/* }}} */

/* {{{ proto bool pg_copy_from(int connection, string table_name , array rows [, string delimiter [, string null_as]])
   Copy table from array */
PHP_FUNCTION(pg_copy_from)
{
	zval *pgsql_link = NULL, *pg_rows;
	zval **tmp;
	char *table_name, *pg_delim = NULL, *pg_null_as = NULL;
	int  table_name_len, pg_delim_len, pg_null_as_len;
	char *query;
	char *query_template = "COPY \"\" FROM STDIN DELIMITERS ':' WITH NULL AS ''";
	HashPosition pos;
	int id = -1;
	PGconn *pgsql;
	PGresult *pgsql_result;
	ExecStatusType status;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "rs/a|ss",
							  &pgsql_link, &table_name, &table_name_len, &pg_rows,
							  &pg_delim, &pg_delim_len, &pg_null_as, &pg_null_as_len) == FAILURE) {
		return;
	}
	if (!pg_delim) {
		pg_delim = "\t";
	}
	if (!pg_null_as) {
		pg_null_as = safe_estrdup("\\\\N");
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	query = (char *)emalloc(strlen(query_template) + strlen(table_name) + strlen(pg_null_as) + 1);
	sprintf(query, "COPY \"%s\" FROM STDIN DELIMITERS '%s' WITH NULL AS '%s'",
				table_name, pg_delim, pg_null_as);
	while ((pgsql_result = PQgetResult(pgsql))) {
		PQclear(pgsql_result);
	}
	pgsql_result = PQexec(pgsql, query);

	efree(pg_null_as);
	efree(query);

	if (pgsql_result) {
		status = PQresultStatus(pgsql_result);
	} else {
		status = (ExecStatusType) PQstatus(pgsql);
	}

	switch (status) {
		case PGRES_COPY_IN:
			if (pgsql_result) {
				PQclear(pgsql_result);
				zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(pg_rows), &pos);
				while (zend_hash_get_current_data_ex(Z_ARRVAL_P(pg_rows), (void **) &tmp, &pos) == SUCCESS) {
					convert_to_string_ex(tmp);
					query = (char *)emalloc(Z_STRLEN_PP(tmp) +2);
					strcpy(query, Z_STRVAL_PP(tmp));
					if(*(query+Z_STRLEN_PP(tmp)-1) != '\n') strcat(query, "\n");
					if (PQputline(pgsql, query)) {
						efree(query);
						php_error(E_WARNING, "%s() query failed: %s",
								  get_active_function_name, PQerrorMessage(pgsql));
						RETURN_FALSE;
					}
					efree(query);
					zend_hash_move_forward_ex(Z_ARRVAL_P(pg_rows), &pos);
				}
				if (PQputline(pgsql, "\\.\n")) {
					php_error(E_WARNING, "%s() query failed: %s",
							  get_active_function_name(TSRMLS_C), PQerrorMessage(pgsql));
					RETURN_FALSE;
				}
				if (PQendcopy(pgsql)) {
					php_error(E_WARNING, "%s() query failed: %s",
							   get_active_function_name(TSRMLS_C), PQerrorMessage(pgsql));
					RETURN_FALSE;
				}
				while ((pgsql_result = PQgetResult(pgsql))) {
					PQclear(pgsql_result);
				}
			} else {
				PQclear(pgsql_result);
				RETURN_FALSE;
			}
			RETURN_TRUE;
			break;
		default:
			PQclear(pgsql_result);
			php_error(E_WARNING, "%s() query failed: %s",
					  get_active_function_name(TSRMLS_C), PQerrorMessage(pgsql));
			RETURN_FALSE;
			break;
	}
}
/* }}} */

#if HAVE_PQESCAPE
/* {{{ proto string pg_escape_string(string data)
   Escape string for text/char type */
PHP_FUNCTION(pg_escape_string)
{
	char *from = NULL, *to = NULL;
	long type = PGSQL_ESCAPE_STRING;
	int len;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
							  &from, &len) == FAILURE) {
		return;
	}

	len = (int)PQescapeString(to, from, strlen(from));
	if (len < 0) {
		RETURN_FALSE;
	}
	RETURN_STRINGL(to, len, 0);
}
/* }}} */

/* {{{ proto string pg_escape_bytea(string data)
   Escape binary for bytea type  */
PHP_FUNCTION(pg_escape_bytea)
{
	char *from = NULL, *to = NULL;
	long type = PGSQL_ESCAPE_STRING;
	int len;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
							  &from, &len) == FAILURE) {
		return;
	}

	to = (char *)PQescapeBytea((unsigned char*)from, strlen(from), (size_t *)&len);
	if (len < 0) {
		RETURN_FALSE;
	}
	RETURN_STRINGL(to, len, 0);
}
/* }}} */
#endif

/* {{{ proto string pg_result_error(resource result)
   Get error message associated with result */
PHP_FUNCTION(pg_result_error)
{
	zval *result;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r",
							  &result) == FAILURE) {
		return;
	}
	
	ZEND_FETCH_RESOURCE(pg_result, pgsql_result_handle *, &result, -1, "PostgreSQL result", le_result);

	pgsql_result = pg_result->result;
	if (!pgsql_result) {
			RETURN_FALSE;
	}
	RETURN_STRING(PQresultErrorMessage(pgsql_result),1);
}
/* }}} */

/* {{{ proto int pg_connection_status(resource connnection)
   Get connection status */
PHP_FUNCTION(pg_connection_status)
{
	zval *pgsql_link = NULL;
	int id = -1;
	PGconn *pgsql;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r",
							  &pgsql_link) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	RETURN_LONG(PQstatus(pgsql));
}

/* }}} */

/* {{{ proto bool pg_connection_reset(resource connection)
   Reset connection (reconnect) */
PHP_FUNCTION(pg_connection_reset)
{
	zval *pgsql_link;
	int id = -1;
	PGconn *pgsql;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r",
							  &pgsql_link) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);
	
	PQreset(pgsql);
	if (PQstatus(pgsql) == CONNECTION_BAD) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}

/* }}} */

#define PHP_PG_ASYNC_IS_BUSY		1
#define PHP_PG_ASYNC_REQUEST_CANCEL 2

/* {{{ php_pgsql_do_async
 */
static void php_pgsql_do_async(INTERNAL_FUNCTION_PARAMETERS, int entry_type) 
{
	zval *pgsql_link;
	int id = -1;
	PGconn *pgsql;
	PGresult *pgsql_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r",
							  &pgsql_link) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (PQsetnonblocking(pgsql, 1)) {
		php_error(E_NOTICE,"%s() cannot set connection to nonblocking mode",
				  get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}
	switch(entry_type) {
		case PHP_PG_ASYNC_IS_BUSY:
			PQconsumeInput(pgsql);
			Z_LVAL_P(return_value) = PQisBusy(pgsql);
			Z_TYPE_P(return_value) = IS_LONG;
			break;
		case PHP_PG_ASYNC_REQUEST_CANCEL:
			Z_LVAL_P(return_value) = PQrequestCancel(pgsql);
			Z_TYPE_P(return_value) = IS_LONG;
			while ((pgsql_result = PQgetResult(pgsql))) {
				PQclear(pgsql_result);
			}
			break;
		default:
			php_error(E_ERROR,"Pgsql module error. Report this error");
			break;
	}
	if (PQsetnonblocking(pgsql, 0)) {
		php_error(E_NOTICE,"%s() cannot set connection to blocking mode",
				  get_active_function_name(TSRMLS_C));
	}
	convert_to_boolean_ex(&return_value);
}
/* }}} */

/* {{{ proto bool pg_cancel_query(resource connection)
   Cancel request */
PHP_FUNCTION(pg_cancel_query)
{
	php_pgsql_do_async(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_PG_ASYNC_REQUEST_CANCEL);
}
/* }}} */

/* {{{ proto bool pg_connection_busy(resource connection)
   Get connection is busy or not */
PHP_FUNCTION(pg_connection_busy)
{
	php_pgsql_do_async(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_PG_ASYNC_IS_BUSY);
}
/* }}} */

/* {{{ proto bool pg_send_query(resource connection, string qeury)
   Send asynchronous query */
PHP_FUNCTION(pg_send_query)
{
	zval *pgsql_link;
	char *query;
	int len;
	int id = -1;
	PGconn *pgsql;
	PGresult *res;
	int leftover = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs",
							  &pgsql_link, &query, &len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (PQsetnonblocking(pgsql, 1)) {
		php_error(E_NOTICE,"%s() cannot set connection to nonblocking mode",
				  get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}
	while ((res = PQgetResult(pgsql))) {
		PQclear(res);
		leftover = 1;
	}
	if (leftover) {
		php_error(E_NOTICE,"%s() - There are results on this connection. Call pg_get_result() until it returns FALSE",
				  get_active_function_name(TSRMLS_C));
	}
	if (!PQsendQuery(pgsql, query)) {
		RETURN_FALSE;
	}
	if (PQsetnonblocking(pgsql, 0)) {
		php_error(E_NOTICE,"%s() cannot set connection to blocking mode",
				  get_active_function_name(TSRMLS_C));
	}
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto resource pg_get_result([resource connection])
   Get asynchronous query result */
PHP_FUNCTION(pg_get_result)
{
	zval *pgsql_link;
	int id = -1;
	PGconn *pgsql;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r",
							  &pgsql_link) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);
	
	pgsql_result = PQgetResult(pgsql);
	if (!pgsql_result) {
		/* no result */
		RETURN_FALSE;
	}
	pg_result = (pgsql_result_handle *) emalloc(sizeof(pgsql_result_handle));
	pg_result->conn = pgsql;
	pg_result->result = pgsql_result;
	pg_result->row = -1;
	ZEND_REGISTER_RESOURCE(return_value, pg_result, le_result);
}
/* }}} */

/* {{{ proto int pg_result_status(resource result)
   Get status of query result */
PHP_FUNCTION(pg_result_status)
{
	zval *result;
	ExecStatusType status;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r",
							  &result) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(pg_result, pgsql_result_handle *, &result, -1, "PostgreSQL result", le_result);

	pgsql_result = pg_result->result;
	status = PQresultStatus(pgsql_result);
	RETURN_LONG((int)status);
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
