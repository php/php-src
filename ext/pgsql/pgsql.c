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

#define SMART_STR_PREALLOC 512

#include "php.h"
#include "php_ini.h"
#include "ext/standard/php_standard.h"
#include "ext/standard/php_smart_str.h"
#include "php_pgsql.h"
#include "php_globals.h"

#if HAVE_PGSQL

#ifndef InvalidOid
#define InvalidOid ((Oid) 0)
#endif

#define PGSQL_ASSOC		1<<0
#define PGSQL_NUM		1<<1
#define PGSQL_BOTH		(PGSQL_ASSOC|PGSQL_NUM)

#define PGSQL_STATUS_LONG     1
#define PGSQL_STATUS_STRING   2

#define PGSQL_MAX_LENGTH_OF_LONG   30
#define PGSQL_MAX_LENGTH_OF_DOUBLE 60

#define PGSQL_RETURN_OID(oid) do { \
	if (oid > LONG_MAX) { \
		smart_str s = {0}; \
		smart_str_append_unsigned(&s, oid); \
		smart_str_0(&s); \
		RETURN_STRINGL(s.c, s.len, 0); \
	} \
	RETURN_LONG((long)oid); \
} while(0)


#if HAVE_PQSETNONBLOCKING
#define PQ_SETNONBLOCKING(pg_link, flag) PQsetnonblocking(pg_link, flag)
#else
#define PQ_SETNONBLOCKING(pg_link, flag) 0
#endif

#define CHECK_DEFAULT_LINK(x) if ((x) == -1) { php_error_docref(NULL TSRMLS_CC, E_WARNING, "No PostgreSQL link opened yet"); }

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
	PHP_FE(pg_ping,			NULL)
	/* query functions */
	PHP_FE(pg_query,		NULL)
	PHP_FE(pg_send_query,	NULL)
	PHP_FE(pg_cancel_query, NULL)
	/* result functions */
	PHP_FE(pg_fetch_result,	NULL)
	PHP_FE(pg_fetch_row,	NULL)
	PHP_FE(pg_fetch_assoc,	NULL)
	PHP_FE(pg_fetch_array,	NULL)
	PHP_FE(pg_fetch_object,	NULL)
	PHP_FE(pg_fetch_all,	NULL)
	PHP_FE(pg_affected_rows,NULL)
	PHP_FE(pg_get_result,	NULL)
	PHP_FE(pg_result_seek,	NULL)
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
	/* misc function */
	PHP_FE(pg_meta_data,	NULL)
	PHP_FE(pg_convert,      NULL)
	PHP_FE(pg_insert,       NULL)
	PHP_FE(pg_update,       NULL)
	PHP_FE(pg_delete,       NULL)
	PHP_FE(pg_select,       NULL)
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

/* {{{ _php_pgsql_notice_handler
 */
static void _php_pgsql_notice_handler(void *resource_id, const char *message)
{
	php_pgsql_notice *notice;
	
	TSRMLS_FETCH();
	if (! PGG(ignore_notices)) {
		if (PGG(log_notices)) {
			php_log_err((char *) message TSRMLS_CC);
		}
		notice = (php_pgsql_notice *)emalloc(sizeof(php_pgsql_notice));
		notice->len = strlen(message);
		notice->message = estrndup(message, notice->len);
		zend_hash_index_update(&PGG(notices), *(int *)resource_id, (void **)&notice, sizeof(php_pgsql_notice *), NULL);
	}
}
/* }}} */

#define PHP_PGSQL_NOTICE_PTR_DTOR (void (*)(void *))_php_pgsql_notice_ptr_dtor

/* {{{ _php_pgsql_notice_dtor
 */
static void _php_pgsql_notice_ptr_dtor(void **ptr) 
{
	php_pgsql_notice *notice = (php_pgsql_notice *)*ptr;
 	efree(notice->message);
  	efree(notice);
}
/* }}} */

/* {{{ _rollback_transactions
 */
static int _rollback_transactions(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	PGconn *link;
	PGresult *res;
	int orig;

	if (Z_TYPE_P(rsrc) != le_plink) 
		return 0;

	link = (PGconn *) rsrc->ptr;

	if (PQ_SETNONBLOCKING(link, 0)) {
		php_error_docref("ref.pgsql" TSRMLS_CC, E_NOTICE, "Cannot set connection to blocking mode");
		return -1;
	}
	
	while ((res = PQgetResult(link))) {
		PQclear(res);
	}
	orig = PGG(ignore_notices);
	PGG(ignore_notices) = 1;
	res = PQexec(link,"BEGIN;ROLLBACK;");
	PQclear(res);
	PGG(ignore_notices) = orig;

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
STD_PHP_INI_BOOLEAN("pgsql.ignore_notice",	"0",	PHP_INI_ALL,		OnUpdateBool,		ignore_notices,	php_pgsql_globals,		pgsql_globals)
STD_PHP_INI_BOOLEAN("pgsql.log_notice",	"0",	PHP_INI_ALL,		OnUpdateBool,		log_notices,	php_pgsql_globals,		pgsql_globals)
PHP_INI_END()
/* }}} */

/* {{{ php_pgsql_init_globals
 */
static void php_pgsql_init_globals(php_pgsql_globals *pgsql_globals_p TSRMLS_DC)
{
	PGG(num_persistent) = 0;
	/* Initilize notice message hash at MINIT only */
	zend_hash_init_ex(&PGG(notices), 0, NULL, PHP_PGSQL_NOTICE_PTR_DTOR, 1, 0); 
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
	/* For pg_result_status() return value type */
	REGISTER_LONG_CONSTANT("PGSQL_STATUS_LONG", PGSQL_STATUS_LONG, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_STATUS_STRING", PGSQL_STATUS_STRING, CONST_CS | CONST_PERSISTENT);
	/* For pg_result_status() return value */
	REGISTER_LONG_CONSTANT("PGSQL_EMPTY_QUERY", PGRES_EMPTY_QUERY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_COMMAND_OK", PGRES_COMMAND_OK, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_TUPLES_OK", PGRES_TUPLES_OK, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_COPY_OUT", PGRES_COPY_OUT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_COPY_IN", PGRES_COPY_IN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_BAD_RESPONSE", PGRES_BAD_RESPONSE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_NONFATAL_ERROR", PGRES_NONFATAL_ERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_FATAL_ERROR", PGRES_FATAL_ERROR, CONST_CS | CONST_PERSISTENT);
	/* pg_convert options */
	REGISTER_LONG_CONSTANT("PGSQL_CONV_IGNORE_DEFAULT", PGSQL_CONV_IGNORE_DEFAULT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_CONV_FORCE_NULL", PGSQL_CONV_FORCE_NULL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_CONV_IGNORE_NOT_NULL", PGSQL_CONV_IGNORE_NOT_NULL, CONST_CS | CONST_PERSISTENT);
	/* pg_insert/update/delete/select options */
	REGISTER_LONG_CONSTANT("PGSQL_DML_NO_CONV", PGSQL_DML_NO_CONV, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DML_EXEC", PGSQL_DML_EXEC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DML_ASYNC", PGSQL_DML_ASYNC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DML_STRING", PGSQL_DML_STRING, CONST_CS | CONST_PERSISTENT);
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
	/* clean up notice messages */
	zend_hash_clean(&PGG(notices));
	/* clean up persistent connection */
	zend_hash_apply(&EG(persistent_list), (apply_func_t) _rollback_transactions TSRMLS_CC);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(pgsql)
{
	char buf[256];

	php_info_print_table_start();
	php_info_print_table_header(2, "PostgreSQL Support", "enabled");
#if HAVE_PG_CONFIG_H
	php_info_print_table_row(2, "PostgreSQL(libpq) Version", PG_VERSION);
#ifdef MULTIBYTE
	php_info_print_table_row(2, "Multibyte charater support", "enabled");
#else
	php_info_print_table_row(2, "Multibyte charater support", "disabled");
#endif
#ifdef USE_SSL
	php_info_print_table_row(2, "SSL support", "enabled");
#else
	php_info_print_table_row(2, "SSL support", "disabled");
#endif
#endif /* HAVE_PG_CONFIG_H */	
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
	PGconn *pgsql;
	smart_str str = {0};
	zval **args[5];
	int i;

	if (ZEND_NUM_ARGS() < 1 || ZEND_NUM_ARGS() == 2 || ZEND_NUM_ARGS() > 5
			|| zend_get_parameters_array_ex(ZEND_NUM_ARGS(), args) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	smart_str_appends(&str, "pgsql");
	
	for (i = 0; i < ZEND_NUM_ARGS(); i++) {
		convert_to_string_ex(args[i]);
		smart_str_appendc(&str, '_');
		smart_str_appendl(&str, Z_STRVAL_PP(args[i]), Z_STRLEN_PP(args[i]));
	}

	smart_str_0(&str);

	if (ZEND_NUM_ARGS() == 1) { /* new style, using connection string */
		connstring = Z_STRVAL_PP(args[0]);
	} else {
		host = Z_STRVAL_PP(args[0]);
		port = Z_STRVAL_PP(args[1]);
		dbname = Z_STRVAL_PP(args[ZEND_NUM_ARGS()-1]);

		switch (ZEND_NUM_ARGS()) {
		case 5:
			tty = Z_STRVAL_PP(args[3]);
			/* fall through */
		case 4:
			options = Z_STRVAL_PP(args[2]);
			break;
		}
	}
	
	if (persistent && PGG(allow_persistent)) {
		list_entry *le;
		
		/* try to find if we already have this link in our persistent list */
		if (zend_hash_find(&EG(persistent_list), str.c, str.len+1, (void **) &le)==FAILURE) {  /* we don't */
			list_entry new_le;
			
			if (PGG(max_links)!=-1 && PGG(num_links)>=PGG(max_links)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING,
								 "Cannot create new link. Too many open links (%d)", PGG(num_links));
				goto err;
			}
			if (PGG(max_persistent)!=-1 && PGG(num_persistent)>=PGG(max_persistent)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING,
								 "Cannot create new link. Too many open persistent links (%d)", PGG(num_persistent));
				goto err;
			}

			/* create the link */
			if (connstring) {
				pgsql=PQconnectdb(connstring);
			} else {
				pgsql=PQsetdb(host,port,options,tty,dbname);
			}
			if (pgsql==NULL || PQstatus(pgsql)==CONNECTION_BAD) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING,
								 "Unable to connect to PostgreSQL server: %s", PQerrorMessage(pgsql));
				if (pgsql) {
					PQfinish(pgsql);
				}
				goto err;
			}

			/* hash it up */
			Z_TYPE(new_le) = le_plink;
			new_le.ptr = pgsql;
			if (zend_hash_update(&EG(persistent_list), str.c, str.len+1, (void *) &new_le, sizeof(list_entry), NULL)==FAILURE) {
				goto err;
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
					php_error_docref(NULL TSRMLS_CC, E_WARNING,"PostgreSQL link lost, unable to reconnect");
					zend_hash_del(&EG(persistent_list),str.c,str.len+1);
					goto err;
				}
			}
			pgsql = (PGconn *) le->ptr;
		}
		ZEND_REGISTER_RESOURCE(return_value, pgsql, le_plink);
	} else {
		list_entry *index_ptr,new_index_ptr;
		
		/* first we check the hash for the hashed_details key.  if it exists,
		 * it should point us to the right offset where the actual pgsql link sits.
		 * if it doesn't, open a new pgsql link, add it to the resource list,
		 * and add a pointer to it with hashed_details as the key.
		 */
		if (zend_hash_find(&EG(regular_list),str.c,str.len+1,(void **) &index_ptr)==SUCCESS) {
			int type,link;
			void *ptr;

			if (Z_TYPE_P(index_ptr) != le_index_ptr) {
				RETURN_FALSE;
			}
			link = (int) (long) index_ptr->ptr; /* XXX: bogus? cast */
			ptr = zend_list_find(link,&type);   /* check if the link is still there */
			if (ptr && (type==le_link || type==le_plink)) {
				Z_LVAL_P(return_value) = link;
				zend_list_addref(link);
				php_pgsql_set_default_link(link TSRMLS_CC);
				Z_TYPE_P(return_value) = IS_RESOURCE;
				goto cleanup;
			} else {
				zend_hash_del(&EG(regular_list),str.c,str.len+1);
			}
		}
		if (PGG(max_links)!=-1 && PGG(num_links)>=PGG(max_links)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot create new link. Too many open links (%d)", PGG(num_links));
			goto err;
		}
		if (connstring) {
			pgsql = PQconnectdb(connstring);
		} else {
			pgsql = PQsetdb(host,port,options,tty,dbname);
		}
		if (pgsql==NULL || PQstatus(pgsql)==CONNECTION_BAD) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to connect to PostgreSQL server: %s", PQerrorMessage(pgsql));
			goto err;
		}

		/* add it to the list */
		ZEND_REGISTER_RESOURCE(return_value, pgsql, le_link);

		/* add it to the hash */
		new_index_ptr.ptr = (void *) Z_LVAL_P(return_value);
		Z_TYPE(new_index_ptr) = le_index_ptr;
		if (zend_hash_update(&EG(regular_list),str.c,str.len+1,(void *) &new_index_ptr, sizeof(list_entry), NULL)==FAILURE) {
			goto err;
		}
		PGG(num_links)++;
	}
	/* set notice processer */
	if (! PGG(ignore_notices) && Z_TYPE_P(return_value) == IS_RESOURCE) {
		PQsetNoticeProcessor(pgsql, _php_pgsql_notice_handler, (void *)&Z_RESVAL_P(return_value));
	}
	php_pgsql_set_default_link(Z_LVAL_P(return_value) TSRMLS_CC);
	
cleanup:
	smart_str_free(&str);
	return;
	
err:
	smart_str_free(&str);
	RETURN_FALSE;
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

/* {{{ proto bool pg_ping([resource connection])
   Ping database. If connection is bad, try to reconnect. */
PHP_FUNCTION(pg_ping)
{
	zval *pgsql_link = NULL;
	int id = -1;
	PGconn *pgsql;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "r",
								 &pgsql_link) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	/* ping connection */
	PQexec(pgsql, "SELECT 1;");

	/* check status. */
	if (PQstatus(pgsql) == CONNECTION_OK)
		RETURN_TRUE;

	/* reset connection if it's broken */
	PQreset(pgsql);
	if (PQstatus(pgsql) == CONNECTION_OK) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
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
	if (PQ_SETNONBLOCKING(pgsql, 0)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE,"Cannot set connection to blocking mode");
		RETURN_FALSE;
	}
	while ((pgsql_result = PQgetResult(pgsql))) {
		PQclear(pgsql_result);
		leftover = 1;
	}
	if (leftover) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Found results on this connection. Use pg_get_result() to get results");
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
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Query failed:  %s", PQerrorMessage(pgsql));
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
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Not supportted under this build");
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
	zval *pgsql_link;
	PGconn *pg_link;
	int id = -1;
	php_pgsql_notice **notice;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r",
							  &pgsql_link) == FAILURE) {
		return;
	}
	/* Just to check if user passed valid resoruce */
	ZEND_FETCH_RESOURCE2(pg_link, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (zend_hash_index_find(&PGG(notices), Z_RESVAL_P(pgsql_link), (void **)&notice) == FAILURE) {
		RETURN_FALSE;
	}
	RETURN_STRINGL((*notice)->message, (*notice)->len, 1);
}
/* }}} */

/* {{{ get_field_name
 */
static char *get_field_name(PGconn *pgsql, Oid oid, HashTable *list TSRMLS_DC)
{
	PGresult *result;
	smart_str str = {0};
	list_entry *field_type;
	char *ret=NULL;

	/* try to lookup the type in the resource list */
	smart_str_appends(&str, "pgsql_oid_");
	smart_str_append_unsigned(&str, oid);
	smart_str_0(&str);

	if (zend_hash_find(list,str.c,str.len+1,(void **) &field_type)==SUCCESS) {
		ret = estrdup((char *)field_type->ptr);
	} else { /* hash all oid's */
		int i,num_rows;
		int oid_offset,name_offset;
		char *tmp_oid, *end_ptr, *tmp_name;
		list_entry new_oid_entry;

		if ((result = PQexec(pgsql,"select oid,typname from pg_type")) == NULL) {
			smart_str_free(&str);
			return empty_string;
		}
		num_rows = PQntuples(result);
		oid_offset = PQfnumber(result,"oid");
		name_offset = PQfnumber(result,"typname");
		
		for (i=0; i<num_rows; i++) {
			if ((tmp_oid = PQgetvalue(result,i,oid_offset))==NULL) {
				continue;
			}
			
			str.len = 0;
			smart_str_appends(&str, "pgsql_oid_");
			smart_str_appends(&str, tmp_oid);
			smart_str_0(&str);
	
			if ((tmp_name = PQgetvalue(result,i,name_offset))==NULL) {
				continue;
			}
			Z_TYPE(new_oid_entry) = le_string;
			new_oid_entry.ptr = estrdup(tmp_name);
			zend_hash_update(list,str.c,str.len+1,(void *) &new_oid_entry, sizeof(list_entry), NULL);
			if (!ret && strtoul(tmp_oid, &end_ptr, 10)==oid) {
				ret = estrdup(tmp_name);
			}
		}
	}

	smart_str_free(&str);
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
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Bad field offset specified");
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
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to jump to row %d on PostgreSQL result index %d", Z_LVAL_PP(row), Z_LVAL_PP(result));
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
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Bad column offset specified");
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
		case 1: /* pg_fetch_*(result) */ 
			if (zend_get_parameters_ex(1, &result) == FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2: /* pg_fetch_*(result, row) */
			if (zend_get_parameters_ex(2, &result, &row) == FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 3: /* pg_fetch_*(result, row, result_type) */
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
	
	if (!(result_type & PGSQL_BOTH)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid result type");
		RETURN_FALSE;
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
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to jump to row %d on PostgreSQL result index %d", Z_LVAL_PP(row), Z_LVAL_PP(result));
				RETURN_FALSE;
			}
		} else {
			/* If 2nd param is NULL, use internal row counter to access next row */
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

/* {{{ proto array pg_fetch_row(resource result [, int row [, int result_type]])
   Get a row as an enumerated array */ 
PHP_FUNCTION(pg_fetch_row)
{
	php_pgsql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, PGSQL_NUM);
}
/* }}} */

/* {{{ proto array pg_fetch_assoc(resource result [, int row])
   Fetch a row as an assoc array */
PHP_FUNCTION(pg_fetch_assoc)
{
	/* pg_fetch_assoc() is added from PHP 4.3.0. It should raise error, when
	   there is 3rd parameter */
	if (ZEND_NUM_ARGS() > 2)
		WRONG_PARAM_COUNT;
	php_pgsql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, PGSQL_ASSOC);
}
/* }}} */

/* {{{ proto array pg_fetch_array(resource result [, int row [, int result_type]])
   Fetch a row as an array */
PHP_FUNCTION(pg_fetch_array)
{
	php_pgsql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, PGSQL_BOTH);
}
/* }}} */

/* {{{ proto object pg_fetch_object(resource result [, int row])
   Fetch a row as an object */
PHP_FUNCTION(pg_fetch_object)
{
	/* pg_fetch_object() allowed result_type used to be. 3rd parameter
	   must be allowed for compatibility */
	php_pgsql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, PGSQL_ASSOC);
	if (Z_TYPE_P(return_value)==IS_ARRAY) {
		object_and_properties_init(return_value, ZEND_STANDARD_CLASS_DEF_PTR, Z_ARRVAL_P(return_value));
	}
}
/* }}} */

/* {{{ proto array pg_fetch_all(resource result)
   Fetch all rows into array */
PHP_FUNCTION(pg_fetch_all)
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
	array_init(return_value);
	if (php_pgsql_result2array(pgsql_result, return_value TSRMLS_CC) == FAILURE) {
		zval_dtor(return_value);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto mixed pg_result_seek(resource result, int offset)
   Set internal row offset */
PHP_FUNCTION(pg_result_seek)
{
	zval *result;
	int row;
	pgsql_result_handle *pg_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|l",
							  &result, &row) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(pg_result, pgsql_result_handle *, &result, -1, "PostgreSQL result", le_result);

	/* Let see if we are better to have another function for this */
	/* if offset is omitted, return current position */
/* 	if (ZEND_NUM_ARGS() == 1) */
/* 		RETURN_LONG(pg_result->row); */

	if (row < 0 || row >= PQntuples(pg_result->result))
		RETURN_FALSE;
	
	/* seek to offset */
	pg_result->row = row;
	RETURN_TRUE;
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
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to jump to row %d on PostgreSQL result index %d", Z_LVAL_PP(row), Z_LVAL_PP(result));
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
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Bad column offset specified");
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

/* {{{ proto string pg_last_oid(resource result)
   Returns the last object identifier */
PHP_FUNCTION(pg_last_oid)
{
	zval **result;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;
	Oid oid;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(pg_result, pgsql_result_handle *, result, -1, "PostgreSQL result", le_result);
	pgsql_result = pg_result->result;
#ifdef HAVE_PQOIDVALUE
	oid = PQoidValue(pgsql_result);
	if (oid == InvalidOid) {
		RETURN_FALSE;
	}
	PGSQL_RETURN_OID(oid);
#else
	Z_STRVAL_P(return_value) = (char *) PQoidStatus(pgsql_result);
	if (Z_STRVAL_P(return_value)) {
		RETURN_STRING(Z_STRVAL_P(return_value), 1);
	}
	RETURN_STRING(empty_string, 0);
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
	FILE *fp = NULL;
	php_stream *stream;
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

	stream = php_stream_open_wrapper(Z_STRVAL_PP(z_filename), mode, ENFORCE_SAFE_MODE|REPORT_ERRORS, NULL);

	if (!stream) {
		RETURN_FALSE;
	}

	if (FAILURE == php_stream_cast(stream, PHP_STREAM_AS_STDIO, (void**)fp, REPORT_ERRORS))	{
		php_stream_close(stream);
		RETURN_FALSE;
	}
	php_stream_auto_cleanup(stream);
	PQtrace(pgsql, fp);
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

/* {{{ proto int pg_lo_create([resource connection])
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
	
	/* NOTE: Archive modes not supported until I get some more data. Don't think anybody's
	   using it anyway. I believe it's also somehow related to the 'time travel' feature of
	   PostgreSQL, that's on the list of features to be removed... Create modes not supported.
	   What's the use of an object that can be only written to, but not read from, and vice
	   versa? Beats me... And the access type (r/w) must be specified again when opening
	   the object, probably (?) overrides this. (Jouni) 
	*/

	if ((pgsql_oid = lo_creat(pgsql, INV_READ|INV_WRITE)) == InvalidOid) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to create PostgreSQL large object");
		RETURN_FALSE;
	}
	PGSQL_RETURN_OID(pgsql_oid);	
}
/* }}} */

/* {{{ proto bool pg_lo_unlink([resource connection,] string large_object_oid)
   Delete a large object */
PHP_FUNCTION(pg_lo_unlink)
{
	zval *pgsql_link = NULL;
	long oid_long;
	char *oid_string, *end_ptr;
	size_t oid_strlen;
	PGconn *pgsql;
	Oid oid;
	int id = -1;
	int argc = ZEND_NUM_ARGS();

	/* accept string type since Oid type is unsigned int */
	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
								 "rs", &pgsql_link, &oid_string, &oid_strlen) == SUCCESS) {
		oid = (Oid)strtoul(oid_string, &end_ptr, 10);
		if ((oid_string+oid_strlen) != end_ptr) {
			/* wrong integer format */
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Wrong OID value passed");
			RETURN_FALSE;
		}
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
								 "rl", &pgsql_link, &oid_long) == SUCCESS) {
		if (oid_long <= InvalidOid) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Invalid OID is specified");
			RETURN_FALSE;
		}
		oid = (Oid)oid_long;
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
								 "s", &oid_string, &oid_strlen) == SUCCESS) {
		oid = (Oid)strtoul(oid_string, &end_ptr, 10);
		if ((oid_string+oid_strlen) != end_ptr) {
			/* wrong integer format */
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Wrong OID value passed");
			RETURN_FALSE;
		}
		id = PGG(default_link);
		CHECK_DEFAULT_LINK(id);
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
								 "l", &oid_long) == SUCCESS) {
		if (oid_long <= InvalidOid) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Invalid OID is specified");
			RETURN_FALSE;
		}
		oid = (Oid)oid_long;
		id = PGG(default_link);
		CHECK_DEFAULT_LINK(id);
	}
	else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Requires 1 or 2 arguments");
		RETURN_FALSE;
	}
	
	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (lo_unlink(pgsql, oid) == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to delete PostgreSQL large object %u", oid);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto resource pg_lo_open([resource connection,] int large_object_oid, string mode)
   Open a large object and return fd */
PHP_FUNCTION(pg_lo_open)
{
	zval *pgsql_link = NULL;
	long oid_long;
	char *oid_string, *end_ptr, *mode_string;
	size_t oid_strlen, mode_strlen;
	PGconn *pgsql;
	Oid oid;
	int id = -1, pgsql_mode=0, pgsql_lofd;
	int create=0;
	pgLofp *pgsql_lofp;
	int argc = ZEND_NUM_ARGS();

	/* accept string type since Oid is unsigned int */
	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
								 "rss", &pgsql_link, &oid_string, &oid_strlen, &mode_string, &mode_strlen) == SUCCESS) {
		oid = (Oid)strtoul(oid_string, &end_ptr, 10);
		if ((oid_string+oid_strlen) != end_ptr) {
			/* wrong integer format */
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Wrong OID value passed");
			RETURN_FALSE;
		}
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
								 "rls", &pgsql_link, &oid_long, &mode_string, &mode_strlen) == SUCCESS) {
		if (oid_long <= InvalidOid) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Invalid OID is specified");
			RETURN_FALSE;
		}
		oid = (Oid)oid_long;
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
								 "ss", &oid_string, &oid_strlen, &mode_string, &mode_strlen) == SUCCESS) {
		oid = (Oid)strtoul(oid_string, &end_ptr, 10);
		if ((oid_string+oid_strlen) != end_ptr) {
			/* wrong integer format */
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Wrong OID value passed");
			RETURN_FALSE;
		}
		id = PGG(default_link);
		CHECK_DEFAULT_LINK(id);
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
								 "ls", &oid_long, &mode_string, &mode_strlen) == SUCCESS) {
		if (oid_long <= InvalidOid) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Invalid OID is specified");
			RETURN_FALSE;
		}
		oid = (Oid)oid_long;
		id = PGG(default_link);
		CHECK_DEFAULT_LINK(id);
	}
	else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Requires 1 or 2 arguments");
		RETURN_FALSE;
	}
	
	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);
	
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

	if ((pgsql_lofd = lo_open(pgsql, oid, pgsql_mode)) == -1) {
		if (create) {
			if ((oid = lo_creat(pgsql, INV_READ|INV_WRITE)) == 0) {
				efree(pgsql_lofp);
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to create PostgreSQL large object");
				RETURN_FALSE;
			} else {
				if ((pgsql_lofd = lo_open(pgsql, oid, pgsql_mode)) == -1) {
					if (lo_unlink(pgsql, oid) == -1) {
						efree(pgsql_lofp);
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Something's really messed up!!! Your database is badly corrupted in a way NOT related to PHP");
						RETURN_FALSE;
					}
					efree(pgsql_lofp);
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to open PostgreSQL large object");
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
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to open PostgreSQL large object");
			RETURN_FALSE;
		}
	} else {
		pgsql_lofp->conn = pgsql;
		pgsql_lofp->lofd = pgsql_lofd;
		ZEND_REGISTER_RESOURCE(return_value, pgsql_lofp, le_lofp);
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
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to close PostgreSQL large object descriptor %d", pgsql->lofd);
		RETVAL_FALSE;
	} else {
		RETVAL_TRUE;
	}

	zend_list_delete(Z_RESVAL_PP(pgsql_lofp));
	return;
}
/* }}} */

#define PGSQL_LO_READ_BUF_SIZE  8192

/* {{{ proto string pg_lo_read(resource large_object [, int len])
   Read a large object */
PHP_FUNCTION(pg_lo_read)
{
  	zval **pgsql_id, **len;
	int buf_len = PGSQL_LO_READ_BUF_SIZE, nbytes;
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

	buf[nbytes] = '\0';
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
		if (Z_LVAL_PP(z_len) > Z_STRLEN_PP(str)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot write more than buffer size %d. Tried to wtite %d", Z_LVAL_PP(str), Z_LVAL_PP(z_len));
			RETURN_FALSE;
		}
		if (Z_LVAL_PP(z_len) < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Buffer size must be larger than 0. %d specified for buffer size", Z_LVAL_PP(str), Z_LVAL_PP(z_len));
			RETURN_FALSE;
		}
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
	int tbytes;
	volatile int nbytes;
	char buf[PGSQL_LO_READ_BUF_SIZE];
	pgLofp *pgsql;

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
	while ((nbytes = lo_read((PGconn *)pgsql->conn, pgsql->lofd, buf, PGSQL_LO_READ_BUF_SIZE))>0) {
		php_body_write(buf, nbytes TSRMLS_CC);
		tbytes += nbytes;
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
									  "sr", &file_in, &name_len, &pgsql_link ) == SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Old API is used.");
	}
	else {
		WRONG_PARAM_COUNT;
	}
	
	if (PG(safe_mode) &&(!php_checkuid(file_in, NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
		RETURN_FALSE;
	}
	
	if (php_check_open_basedir(file_in TSRMLS_CC)) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	oid = lo_import(pgsql, file_in);

	if (oid == InvalidOid) {
		RETURN_FALSE;
	}
	PGSQL_RETURN_OID(oid);
}
/* }}} */

/* {{{ proto bool pg_lo_export([resource connection, ] int objoid, string filename)
   Export large object direct to filesystem */
PHP_FUNCTION(pg_lo_export)
{
	zval *pgsql_link = NULL;
	char *file_out, *oid_string, *end_ptr;
	size_t oid_strlen;
	int id = -1, name_len;
	long oid_long;
	Oid oid;
	PGconn *pgsql;
	int argc = ZEND_NUM_ARGS();

	/* allow string to handle large OID value correctly */
	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
								 "rls", &pgsql_link, &oid_long, &file_out, &name_len) == SUCCESS) {
		if (oid_long <= InvalidOid) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Invalid OID is specified");
			RETURN_FALSE;
		}
		oid = (Oid)oid_long;
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
								 "rss", &pgsql_link, &oid_string, &oid_strlen, &file_out, &name_len) == SUCCESS) {
		oid = (Oid)strtoul(oid_string, &end_ptr, 10);
		if ((oid_string+oid_strlen) != end_ptr) {
			/* wrong integer format */
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Wrong OID value passed");
			RETURN_FALSE;
		}
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
									  "ls",  &oid_long, &file_out, &name_len) == SUCCESS) {
		if (oid_long <= InvalidOid) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Invalid OID is specified");
			RETURN_FALSE;
		}
		oid = (Oid)oid_long;
		id = PGG(default_link);
		CHECK_DEFAULT_LINK(id);
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
								 "ss", &pgsql_link, &oid_string, &oid_strlen, &file_out, &name_len) == SUCCESS) {
		oid = (Oid)strtoul(oid_string, &end_ptr, 10);
		if ((oid_string+oid_strlen) != end_ptr) {
			/* wrong integer format */
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Wrong OID value passed");
			RETURN_FALSE;
		}
		id = PGG(default_link);
		CHECK_DEFAULT_LINK(id);
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
								 "ssr", &oid_string, &oid_strlen, &file_out, &name_len, &pgsql_link) == SUCCESS) {
		oid = (Oid)strtoul(oid_string, &end_ptr, 10);
		if ((oid_string+oid_strlen) != end_ptr) {
			/* wrong integer format */
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Wrong OID value passed");
			RETURN_FALSE;
		}
	}
	else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC,
									  "lsr", &oid_long, &file_out, &name_len, &pgsql_link) == SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, " %s(): Old API is used");
		if (oid_long <= InvalidOid) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Invalid OID is specified");
			RETURN_FALSE;
		}
		oid = (Oid)oid_long;
	}
	else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Requires 2 or 3 arguments");
		RETURN_FALSE;
	}
	
	if (PG(safe_mode) &&(!php_checkuid(file_out, NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
		RETURN_FALSE;
	}
	
	if (php_check_open_basedir(file_out TSRMLS_CC)) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (lo_export(pgsql, oid, file_out)) {
		RETURN_TRUE;
	} 
	RETURN_FALSE;
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
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid whence parameter");
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

#ifdef HAVE_PQCLIENTENCODING
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
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Query failed: %s", PQerrorMessage(pgsql));
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
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Query failed: %s", PQerrorMessage(pgsql));
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto array pg_copy_to(resource connection, string table_name [, string delimiter [, string null_as]])
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
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "getline failed: %s", PQerrorMessage(pgsql));
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
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "endcopy failed: %s", PQerrorMessage(pgsql));
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
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "COPY command failed: %s", PQerrorMessage(pgsql));
			RETURN_FALSE;
			break;
	}
}
/* }}} */

/* {{{ proto bool pg_copy_from(resource connection, string table_name , array rows [, string delimiter [, string null_as]])
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
					if(*(query+Z_STRLEN_PP(tmp)-1) != '\n')
						strcat(query, "\n");
					if (PQputline(pgsql, query)) {
						efree(query);
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "copy failed: %s", PQerrorMessage(pgsql));
						RETURN_FALSE;
					}
					efree(query);
					zend_hash_move_forward_ex(Z_ARRVAL_P(pg_rows), &pos);
				}
				if (PQputline(pgsql, "\\.\n") == EOF) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "putline failed: %s", PQerrorMessage(pgsql));
					RETURN_FALSE;
				}
				if (PQendcopy(pgsql)) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "endcopy failed: %s", PQerrorMessage(pgsql));
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
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "COPY command failed: %s", PQerrorMessage(pgsql));
			RETURN_FALSE;
			break;
	}
}
/* }}} */

#ifdef HAVE_PQESCAPE
/* {{{ proto string pg_escape_string(string data)
   Escape string for text/char type */
PHP_FUNCTION(pg_escape_string)
{
	char *from = NULL, *to = NULL;
	size_t from_len, to_len;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
							  &from, &from_len) == FAILURE) {
		return;
	}

	to = (char *)emalloc(from_len*2+1);
	to_len = (int)PQescapeString(to, from, from_len);
	RETURN_STRINGL(to, to_len, 0);
}
/* }}} */

/* {{{ proto string pg_escape_bytea(string data)
   Escape binary for bytea type  */
PHP_FUNCTION(pg_escape_bytea)
{
	char *from = NULL, *to = NULL;
	size_t from_len, to_len;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
							  &from, &from_len) == FAILURE) {
		return;
	}

	to = (char *)PQescapeBytea((unsigned char*)from, from_len, &to_len);
	RETVAL_STRINGL(to, to_len-1, 1); /* to_len includes addtional '\0' */
	free(to);
}
/* }}} */

/* PQunescapeBytea() from PostgreSQL 7.3 to provide bytea unescape feature to 7.2 users.
   Renamed to php_pgsql_unescape_bytea() */
/*
 *		PQunescapeBytea - converts the null terminated string representation
 *		of a bytea, strtext, into binary, filling a buffer. It returns a
 *		pointer to the buffer which is NULL on error, and the size of the
 *		buffer in retbuflen. The pointer may subsequently be used as an
 *		argument to the function free(3). It is the reverse of PQescapeBytea.
 *
 *		The following transformations are reversed:
 *		'\0' == ASCII  0 == \000
 *		'\'' == ASCII 39 == \'
 *		'\\' == ASCII 92 == \\
 *
 *		States:
 *		0	normal		0->1->2->3->4
 *		1	\			   1->5
 *		2	\0			   1->6
 *		3	\00
 *		4	\000
 *		5	\'
 *		6	\\
 */
static unsigned char * php_pgsql_unescape_bytea(unsigned char *strtext, size_t *retbuflen)
{
	size_t		buflen;
	unsigned char *buffer,
			   *sp,
			   *bp;
	unsigned int state = 0;

	if (strtext == NULL)
		return NULL;
	buflen = strlen(strtext);	/* will shrink, also we discover if
								 * strtext */
	buffer = (unsigned char *) malloc(buflen);	/* isn't NULL terminated */
	if (buffer == NULL)
		return NULL;
	for (bp = buffer, sp = strtext; *sp != '\0'; bp++, sp++)
	{
		switch (state)
		{
			case 0:
				if (*sp == '\\')
					state = 1;
				*bp = *sp;
				break;
			case 1:
				if (*sp == '\'')	/* state=5 */
				{				/* replace \' with 39 */
					bp--;
					*bp = '\'';
					buflen--;
					state = 0;
				}
				else if (*sp == '\\')	/* state=6 */
				{				/* replace \\ with 92 */
					bp--;
					*bp = '\\';
					buflen--;
					state = 0;
				}
				else
				{
					if (isdigit(*sp))
						state = 2;
					else
						state = 0;
					*bp = *sp;
				}
				break;
			case 2:
				if (isdigit(*sp))
					state = 3;
				else
					state = 0;
				*bp = *sp;
				break;
			case 3:
				if (isdigit(*sp))		/* state=4 */
				{
					int			v;

					bp -= 3;
					sscanf(sp - 2, "%03o", &v);
					*bp = v;
					buflen -= 3;
					state = 0;
				}
				else
				{
					*bp = *sp;
					state = 0;
				}
				break;
		}
	}
	buffer = realloc(buffer, buflen);
	if (buffer == NULL)
		return NULL;

	*retbuflen = buflen;
	return buffer;
}

/* {{{ proto string pg_unescape_bytea(string data)
   Unescape binary for bytea type  */
PHP_FUNCTION(pg_unescape_bytea)
{
	char *from = NULL, *to = NULL;
	size_t from_len, to_len;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
							  &from, &from_len) == FAILURE) {
		return;
	}

	to = (char *)php_pgsql_unescape_bytea((unsigned char*)from, &to_len);
	if (!to) {
		RETURN_FALSE;
	}
	RETVAL_STRINGL(to, to_len, 1);
	free(to);
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
	char *err = NULL;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "r",
								 &result) == FAILURE) {
		RETURN_FALSE;
	}
	
	ZEND_FETCH_RESOURCE(pg_result, pgsql_result_handle *, &result, -1, "PostgreSQL result", le_result);

	pgsql_result = pg_result->result;
	if (!pgsql_result) {
		RETURN_FALSE;
	}
	err = (char *)PQresultErrorMessage(pgsql_result);
	RETURN_STRING(err,1);
}
/* }}} */

/* {{{ proto int pg_connection_status(resource connnection)
   Get connection status */
PHP_FUNCTION(pg_connection_status)
{
	zval *pgsql_link = NULL;
	int id = -1;
	PGconn *pgsql;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "r",
								 &pgsql_link) == FAILURE) {
		RETURN_FALSE;
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
	
	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "r",
								 &pgsql_link) == FAILURE) {
		RETURN_FALSE;
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
														  
/* {{{ php_pgsql_flush_query
 */
static int php_pgsql_flush_query(PGconn *pgsql TSRMLS_DC) 
{
	PGresult *res;
	int leftover = 0;
	
	if (PQsetnonblocking(pgsql, 1)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE,"Cannot set connection to nonblocking mode");
		return -1;
	}
	while ((res = PQgetResult(pgsql))) {
		PQclear(res);
		leftover++;
	}
	PQsetnonblocking(pgsql, 0);
	return leftover;
}
/* }}} */
														  
/* {{{ php_pgsql_do_async
 */
static void php_pgsql_do_async(INTERNAL_FUNCTION_PARAMETERS, int entry_type) 
{
	zval *pgsql_link;
	int id = -1;
	PGconn *pgsql;
	PGresult *pgsql_result;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "r",
								 &pgsql_link) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (PQ_SETNONBLOCKING(pgsql, 1)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE,"Cannot set connection to nonblocking mode");
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
			php_error_docref(NULL TSRMLS_CC, E_ERROR,"PostgreSQL module error. Report this error");
			break;
	}
	if (PQ_SETNONBLOCKING(pgsql, 0)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE,"Cannot set connection to blocking mode");
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

	if (PQ_SETNONBLOCKING(pgsql, 1)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE,"Cannot set connection to nonblocking mode");
		RETURN_FALSE;
	}
	while ((res = PQgetResult(pgsql))) {
		PQclear(res);
		leftover = 1;
	}
	if (leftover) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE,"There are results on this connection. Call pg_get_result() until it returns FALSE");
	}
	if (!PQsendQuery(pgsql, query)) {
		RETURN_FALSE;
	}
	if (PQ_SETNONBLOCKING(pgsql, 0)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE,"Cannot set connection to blocking mode");
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

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "r",
								 &pgsql_link) == FAILURE) {
		RETURN_FALSE;
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

/* {{{ proto int pg_result_status(resource result[, long result_type])
   Get status of query result */
PHP_FUNCTION(pg_result_status)
{
	zval *result;
	long result_type = PGSQL_STATUS_LONG;
	ExecStatusType status;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "r|l",
								 &result, &result_type) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(pg_result, pgsql_result_handle *, &result, -1, "PostgreSQL result", le_result);

	pgsql_result = pg_result->result;
	if (result_type == PGSQL_STATUS_LONG) {
		status = PQresultStatus(pgsql_result);
		RETURN_LONG((int)status);
	}
	else if (result_type == PGSQL_STATUS_STRING) {
		RETURN_STRING(PQcmdStatus(pgsql_result), 1);
	}
	else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Expects optional 2nd parameter to be PGSQL_STATUS_LONG or PGSQL_STATUS_STRING");
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ php_pgsql_meta_data
 * TODO: Add meta_data cache for better performance
 */
PHPAPI int php_pgsql_meta_data(PGconn *pg_link, const char *table_name, zval *meta TSRMLS_DC) 
{
	PGresult *pg_result;
	char *tmp_name;
	smart_str querystr = {0};
	size_t new_len;
	int i, num_rows;
	zval *elem;
	
	smart_str_appends(&querystr, 
			"SELECT a.attname, a.attnum, t.typname, a.attlen, a.attnotNULL, a.atthasdef "
			"FROM pg_class as c, pg_attribute a, pg_type t "
			"WHERE a.attnum > 0 AND a.attrelid = c.oid AND c.relname = '");
	
	tmp_name = php_addslashes((char *)table_name, strlen(table_name), &new_len, 0 TSRMLS_CC);
	smart_str_appendl(&querystr, tmp_name, new_len);
	efree(tmp_name);

	smart_str_appends(&querystr, "' AND a.atttypid = t.oid ORDER BY a.attnum;");
	smart_str_0(&querystr);
	
	pg_result = PQexec(pg_link, querystr.c);
	if (PQresultStatus(pg_result) != PGRES_TUPLES_OK || (num_rows = PQntuples(pg_result)) == 0) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Failed to query meta_data for '%s' table %s", table_name, querystr.c);
		smart_str_free(&querystr);
		PQclear(pg_result);
		return FAILURE;
	}
	smart_str_free(&querystr);

	for (i = 0; i < num_rows; i++) {
		char *name;
		MAKE_STD_ZVAL(elem);
		array_init(elem);
		add_assoc_long(elem, "num", atoi(PQgetvalue(pg_result,i,1)));
		add_assoc_string(elem, "type", PQgetvalue(pg_result,i,2), 1);
		add_assoc_long(elem, "len", atoi(PQgetvalue(pg_result,i,3)));
		if (!strcmp(PQgetvalue(pg_result,i,4), "t")) {
			add_assoc_bool(elem, "not null", 1);
		}
		else {
			add_assoc_bool(elem, "not null", 0);
		}
		if (!strcmp(PQgetvalue(pg_result,i,5), "t")) {
			add_assoc_bool(elem, "has default", 1);
		}
		else {
			add_assoc_bool(elem, "has default", 0);
		}
		name = PQgetvalue(pg_result,i,0);
		add_assoc_zval(meta, name, elem);
	}
	
	return SUCCESS;
}

/* }}} */


/* {{{ proto array pg_meta_data(resource db, string table)
   Get meta_data */
PHP_FUNCTION(pg_meta_data)
{
	zval *pgsql_link;
	char *table_name;
	uint table_name_len;
	PGconn *pgsql;
	int id = -1;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs",
							  &pgsql_link, &table_name, &table_name_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE2(pgsql, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);
	
	array_init(return_value);
	if (php_pgsql_meta_data(pgsql, table_name, return_value TSRMLS_CC) == FAILURE) {
		zval_dtor(return_value); /* destroy array */
		RETURN_FALSE;
	}
} 
/* }}} */

/* {{{ php_pgsql_get_data_type
 */
static php_pgsql_data_type php_pgsql_get_data_type(const char *type_name, size_t len)
{
    /* This is stupid way to do. I'll fix it when I decied how to support
	   user defined types. (Yasuo) */
	
	/* boolean */
	if (!strcmp(type_name, "bool")|| !strcmp(type_name, "boolean"))
		return PG_BOOL;
	/* object id */
	if (!strcmp(type_name, "oid"))
		return PG_OID;
	/* integer */
	if (!strcmp(type_name, "int2") || !strcmp(type_name, "smallint"))
		return PG_INT2;
	if (!strcmp(type_name, "int4") || !strcmp(type_name, "integer"))
		return PG_INT4;
	if (!strcmp(type_name, "int8") || !strcmp(type_name, "bigint"))
		return PG_INT8;
	/* real and other */
	if (!strcmp(type_name, "float4") || !strcmp(type_name, "real"))
		return PG_FLOAT4;
	if (!strcmp(type_name, "float8") || !strcmp(type_name, "double precision"))
		return PG_FLOAT8;
	if (!strcmp(type_name, "numeric"))
		return PG_NUMERIC;
	if (!strcmp(type_name, "money"))
		return PG_MONEY;
	/* character */
	if (!strcmp(type_name, "text"))
		return PG_TEXT;
	if (!strcmp(type_name, "bpchar") || !strcmp(type_name, "character"))
		return PG_CHAR;
	if (!strcmp(type_name, "varchar") || !strcmp(type_name, "character varying"))
		return PG_VARCHAR;
	/* time and interval */
	if (!strcmp(type_name, "abstime"))
		return PG_UNIX_TIME;
	if (!strcmp(type_name, "reltime"))
		return PG_UNIX_TIME_INTERVAL;
	if (!strcmp(type_name, "tinterval"))
		return PG_UNIX_TIME_INTERVAL;
	if (!strcmp(type_name, "date"))
		return PG_DATE;
	if (!strcmp(type_name, "time"))
		return PG_TIME;
	if (!strcmp(type_name, "timestamp") || !strcmp(type_name, "time with time zone"))
		return PG_TIME_WITH_TIMEZONE;
	if (!strcmp(type_name, "timestamp with time zone"))
		return PG_TIMESTAMP_WITH_TIMEZONE;
	if (!strcmp(type_name, "interval"))
		return PG_INTERVAL;
	/* binary */
	if (!strcmp(type_name, "bytea"))
		return PG_BYTEA;
	/* network */
	if (!strcmp(type_name, "cidr"))
		return PG_CIDR;
	if (!strcmp(type_name, "inet"))
		return PG_INET;
	if (!strcmp(type_name, "macaddr"))
		return PG_MACADDR;
	/* bit */
	if (!strcmp(type_name, "bit"))
		return PG_BIT;
	if (!strcmp(type_name, "bit varying"))
		return PG_VARBIT;
	/* geometric */
	if (!strcmp(type_name, "line"))
		return PG_LINE;
	if (!strcmp(type_name, "lseg"))
		return PG_LSEG;
	if (!strcmp(type_name, "box"))
		return PG_BOX;
	if (!strcmp(type_name, "path"))
		return PG_PATH;
	if (!strcmp(type_name, "point"))
		return PG_POINT;
	if (!strcmp(type_name, "polygon"))
		return PG_POLYGON;
	if (!strcmp(type_name, "circle"))
		return PG_CIRCLE;
		
	return PG_UNKNOWN;
}
/* }}} */

/* {{{ php_pgsql_convert_match
 * test field value with regular expression specified.  
 */
static int php_pgsql_convert_match(const char *str, const char *regex , int icase TSRMLS_DC)
{
	regex_t re;	
	regmatch_t *subs;
	int regopt = REG_EXTENDED;
	int regerr, ret = SUCCESS;

	if (icase) {
		regopt |= REG_ICASE;
	}
	
	regerr = regcomp(&re, regex, regopt);
	if (regerr) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot compile regex");
		regfree(&re);
		return FAILURE;
	}
	subs = (regmatch_t *)ecalloc(sizeof(regmatch_t), re.re_nsub+1);
	if (!subs) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot allocate memory");
		regfree(&re);
		return FAILURE;
	}
	regerr = regexec(&re, str, re.re_nsub+1, subs, 0);
	if (regerr == REG_NOMATCH) {
#ifdef PHP_DEBUG		
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "'%s' does not match with '%s'", str, regex);
#endif
		ret = FAILURE;
	}
	else if (regerr) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot exec regex");
		ret = FAILURE;
	}
	regfree(&re);
	efree(subs);
	return ret;
}

/* }}} */

/* {{{ php_pgsql_add_quote
 * add quotes around string.
 */
static int php_pgsql_add_quotes(zval *src, zend_bool should_free TSRMLS_DC) 
{
	smart_str str = {0};
	
	assert(Z_TYPE_P(src) == IS_STRING);
	assert(should_free == 1 || should_free == 0);

	smart_str_appendc(&str, '\'');
	smart_str_appendl(&str, Z_STRVAL_P(src), Z_STRLEN_P(src));
	smart_str_appendc(&str, '\'');
	smart_str_0(&str);
	
	if (should_free) {
		efree(Z_STRVAL_P(src));
	}
	Z_STRVAL_P(src) = str.c;
	Z_STRLEN_P(src) = str.len;

	return SUCCESS;
}
/* }}} */

#define PGSQL_CONV_CHECK_IGNORE() \
				if (!err && Z_TYPE_P(new_val) == IS_STRING && !strcmp(Z_STRVAL_P(new_val), "NULL")) { \
					/* if new_value is string "NULL" and field has default value, remove element to use default value */ \
					if (!(opt & PGSQL_CONV_IGNORE_DEFAULT) && Z_BVAL_PP(has_default)) { \
						zval_dtor(new_val); \
						FREE_ZVAL(new_val); \
						skip_field = 1; \
					} \
					/* raise error if it's not null and cannot be ignored */ \
					else if (!(opt & PGSQL_CONV_IGNORE_NOT_NULL) && Z_BVAL_PP(not_null)) { \
						php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Detected NULL for 'NOT NULL' field '%s'", field ); \
						err = 1; \
					} \
				} 		

/* {{{ php_pgsql_convert
 * check and convert array values (fieldname=>vlaue pair) for sql
 */
PHPAPI int php_pgsql_convert(PGconn *pg_link, const char *table_name, const zval *values, zval *result, ulong opt TSRMLS_DC) 
{
	HashPosition pos;
	char *field = NULL;
	uint field_len = -1;
	ulong num_idx = -1;
	zval *meta, **def, **type, **not_null, **has_default, **val, *new_val;
	int new_len, key_type, err = 0, skip_field;
	
	assert(pg_link != NULL);
	assert(Z_TYPE_P(values) == IS_ARRAY);
	assert(Z_TYPE_P(result) == IS_ARRAY);
	assert(!(opt & ~PGSQL_CONV_OPTS));

	if (!table_name) {
		return FAILURE;
	}
	MAKE_STD_ZVAL(meta);
	if (array_init(meta) == FAILURE) {
		zval_dtor(meta);
		FREE_ZVAL(meta);
		return FAILURE;
	}
	if (php_pgsql_meta_data(pg_link, table_name, meta TSRMLS_CC) == FAILURE) {
		zval_dtor(meta);
		FREE_ZVAL(meta);
		return FAILURE;
	}
	for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(values), &pos);
		 zend_hash_get_current_data_ex(Z_ARRVAL_P(values), (void **)&val, &pos) == SUCCESS;
		 zend_hash_move_forward_ex(Z_ARRVAL_P(values), &pos)) {
		skip_field = 0;
		if ((key_type = zend_hash_get_current_key_ex(Z_ARRVAL_P(values), &field, &field_len, &num_idx, 0, &pos)) == HASH_KEY_NON_EXISTANT) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to get array key type");
			err = 1;
		}
		if (!err && key_type == HASH_KEY_IS_LONG) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Accepts only string key for values");
			err = 1;
		}
		if (!err && key_type == HASH_KEY_NON_EXISTANT) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Accepts only string key for values");
			err = 1;
		}
		if (!err && zend_hash_find(Z_ARRVAL_P(meta), field, field_len, (void **)&def) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Invalid field name (%s) in values", field);
			err = 1;
		}
		if (!err && zend_hash_find(Z_ARRVAL_PP(def), "type", sizeof("type"), (void **)&type) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Detected broken meta data. Missing 'type'");
			err = 1;
		}
		if (!err && zend_hash_find(Z_ARRVAL_PP(def), "not null", sizeof("not null"), (void **)&not_null) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Detected broken meta data. Missing 'not null'");
			err = 1;
		}
		if (!err && zend_hash_find(Z_ARRVAL_PP(def), "has default", sizeof("has default"), (void **)&has_default) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Detected broken meta data. Missing 'has default'");
			err = 1;
		}
		if (!err && (Z_TYPE_PP(val) == IS_ARRAY ||
			 Z_TYPE_PP(val) == IS_OBJECT ||
			 Z_TYPE_PP(val) == IS_CONSTANT_ARRAY)) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Expects scaler values as field values");
			err = 1;
		}
		if (err) {
			break;
		}
		
		MAKE_STD_ZVAL(new_val);
		switch(php_pgsql_get_data_type(Z_STRVAL_PP(type), Z_STRLEN_PP(type)))
		{
			case PG_BOOL:
				switch (Z_TYPE_PP(val)) {
					case IS_STRING:
						if (Z_STRLEN_PP(val) == 0) {
							ZVAL_STRING(new_val, "NULL", 1);
						}
						else {
							if (!strcmp(Z_STRVAL_PP(val), "t") || !strcmp(Z_STRVAL_PP(val), "T") ||
								!strcmp(Z_STRVAL_PP(val), "y") || !strcmp(Z_STRVAL_PP(val), "Y") ||
								!strcmp(Z_STRVAL_PP(val), "true") || !strcmp(Z_STRVAL_PP(val), "True") ||
								!strcmp(Z_STRVAL_PP(val), "yes") || !strcmp(Z_STRVAL_PP(val), "Yes") ||
								!strcmp(Z_STRVAL_PP(val), "1")) {
								ZVAL_STRING(new_val, "'t'", 1);
							}
							else if (!strcmp(Z_STRVAL_PP(val), "f") || !strcmp(Z_STRVAL_PP(val), "F") ||
									 !strcmp(Z_STRVAL_PP(val), "n") || !strcmp(Z_STRVAL_PP(val), "N") ||
									 !strcmp(Z_STRVAL_PP(val), "false") ||  !strcmp(Z_STRVAL_PP(val), "False") ||
									 !strcmp(Z_STRVAL_PP(val), "no") ||  !strcmp(Z_STRVAL_PP(val), "No") ||
									 !strcmp(Z_STRVAL_PP(val), "0")) {
								ZVAL_STRING(new_val, "'f'", 1);
							}
							else {
								php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Detected invalid value (%s) for PostgreSQL %s field (%s)", Z_STRVAL_PP(val), Z_STRVAL_PP(type), field);
								err = 1;
							}
						}
						break;
						
					case IS_LONG:
					case IS_BOOL:
						if (Z_LVAL_PP(val)) {
							ZVAL_STRING(new_val, "'t'", 1);
						}
						else {
							ZVAL_STRING(new_val, "'f'", 1);
						}
						break;

					case IS_NULL:
						ZVAL_STRING(new_val, "NULL", 1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Expects string, null, long or boolelan value for PostgreSQL '%s' (%s)", Z_STRVAL_PP(type), field);
				}
				break;
					
			case PG_OID:
			case PG_INT2:
			case PG_INT4:
			case PG_INT8:
				switch (Z_TYPE_PP(val)) {
					case IS_STRING:
						if (Z_STRLEN_PP(val) == 0) {
							ZVAL_STRING(new_val, "NULL", 1);
						}
						else {
							/* FIXME: better regex must be used */
							if (php_pgsql_convert_match(Z_STRVAL_PP(val), "^([+-]{0,1}[0-9]+)$", 0 TSRMLS_CC) == FAILURE) {
								err = 1;
							}
							else {
								ZVAL_STRING(new_val, Z_STRVAL_PP(val), 1);
							}
						}
						break;
						
					case IS_DOUBLE:
						ZVAL_DOUBLE(new_val, Z_DVAL_PP(val));
						convert_to_long_ex(&new_val);
						break;
						
					case IS_LONG:
						ZVAL_LONG(new_val, Z_LVAL_PP(val));
						break;
						
					case IS_NULL:
						ZVAL_STRING(new_val, "NULL", 1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Expects string, null, long or double value for pgsql '%s' (%s)", Z_STRVAL_PP(type), field);
				}
				break;

			case PG_NUMERIC:
			case PG_MONEY:
			case PG_FLOAT4:
			case PG_FLOAT8:
				switch (Z_TYPE_PP(val)) {
					case IS_STRING:
						if (Z_STRLEN_PP(val) == 0) {
							ZVAL_STRING(new_val, "NULL", 1);
						}
						else {
							/* FIXME: better regex must be used */
							if (php_pgsql_convert_match(Z_STRVAL_PP(val), "^([+-]{0,1}[0-9]+)|([+-]{0,1}[0-9]*[\\.][0-9]+)|([+-]{0,1}[0-9]+[\\.][0-9]*)$", 0 TSRMLS_CC) == FAILURE) {
								err = 1;
							}
							else {
								ZVAL_STRING(new_val, Z_STRVAL_PP(val), 1);
							}
						}
						break;
						
					case IS_LONG:
						ZVAL_LONG(new_val, Z_LVAL_PP(val));
						break;
						
					case IS_DOUBLE:
						ZVAL_DOUBLE(new_val, Z_DVAL_PP(val));
						break;
						
					case IS_NULL:
						ZVAL_STRING(new_val, "NULL", 1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Expects string, null, long or double value for PostgreSQL '%s' (%s)", Z_STRVAL_PP(type), field);
				}
				break;

			case PG_TEXT:
			case PG_CHAR:
			case PG_VARCHAR:
				switch (Z_TYPE_PP(val)) {
					case IS_STRING:
						if (Z_STRLEN_PP(val) == 0) {
							if (opt & PGSQL_CONV_FORCE_NULL) {
								ZVAL_STRING(new_val, "NULL", 1);
							}
							else {
								ZVAL_STRING(new_val, empty_string, 1);
							}
						}
						else {
							Z_TYPE_P(new_val) = IS_STRING;
#if HAVE_PQESCAPE
							{
								char *tmp;
	 							tmp = (char *)emalloc(Z_STRLEN_PP(val)*2+1);
								Z_STRLEN_P(new_val) = (int)PQescapeString(tmp, Z_STRVAL_PP(val), Z_STRLEN_PP(val));
								Z_STRVAL_P(new_val) = tmp;
							}
#else					
							Z_STRVAL_P(new_val) = php_addslashes(Z_STRVAL_PP(val), Z_STRLEN_PP(val), &Z_STRLEN_P(new_val), 0 TSRMLS_CC);
#endif
						}
						php_pgsql_add_quotes(new_val, 1 TSRMLS_CC);
						break;
						
					case IS_LONG:
						ZVAL_LONG(new_val, Z_LVAL_PP(val));
						convert_to_string_ex(&new_val);
						break;
						
					case IS_DOUBLE:
						ZVAL_DOUBLE(new_val, Z_DVAL_PP(val));
						convert_to_string_ex(&new_val);
						break;

					case IS_NULL:
						ZVAL_STRING(new_val, "NULL", 1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Expects string, null, long or double value for PostgreSQL '%s' (%s)", Z_STRVAL_PP(type), field);
				}
				break;
					
			case PG_UNIX_TIME:
			case PG_UNIX_TIME_INTERVAL:
				/* these are the actallay a integer */
				switch (Z_TYPE_PP(val)) {
					case IS_STRING:
						if (Z_STRLEN_PP(val) == 0) {
							ZVAL_STRING(new_val, "NULL", 1);
						}
						else {
							/* FIXME: Better regex must be used */
							if (php_pgsql_convert_match(Z_STRVAL_PP(val), "^[0-9]+$", 0 TSRMLS_CC) == FAILURE) {
								err = 1;
							} 
							else {
								ZVAL_STRING(new_val, Z_STRVAL_PP(val), 1);
								convert_to_long_ex(&new_val);
							}
						}
						break;
						
					case IS_DOUBLE:
						ZVAL_DOUBLE(new_val, Z_DVAL_PP(val));
						convert_to_long_ex(&new_val);
						break;
						
					case IS_LONG:
						ZVAL_LONG(new_val, Z_LVAL_PP(val));
						break;
						
					case IS_NULL:
						ZVAL_STRING(new_val, "NULL", 1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Expects string, null, long or double value for '%s' (%s)", Z_STRVAL_PP(type), field);
				}
				break;
				
			case PG_CIDR:
			case PG_INET:
				switch (Z_TYPE_PP(val)) {
					case IS_STRING:
						if (Z_STRLEN_PP(val) == 0) {
							ZVAL_STRING(new_val, "NULL", 1);
						}
						else {
							/* FIXME: Better regex must be used */
							if (php_pgsql_convert_match(Z_STRVAL_PP(val), "^([0-9]{1,3}\\.){3}[0-9]{1,3}(/[0-9]{1,2}){0,1}$", 0 TSRMLS_CC) == FAILURE) {
								err = 1;
							}
							else {
								ZVAL_STRING(new_val, Z_STRVAL_PP(val), 1);
								php_pgsql_add_quotes(new_val, 1 TSRMLS_CC);
							}
						}
						break;
						
					case IS_NULL:
						ZVAL_STRING(new_val, "NULL", 1);
						break;

					default:
						err = 1;
				}				
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Expects string or null for '%s' (%s)", Z_STRVAL_PP(type), field);
				}
				break;
				
			case PG_TIME_WITH_TIMEZONE:
			case PG_TIMESTAMP_WITH_TIMEZONE:
				switch(Z_TYPE_PP(val)) {
					case IS_STRING:
						if (Z_STRLEN_PP(val) == 0) {
							ZVAL_STRING(new_val, "NULL", 1);
						}
						else {
							/* FIXME: better regex must be used */
							if (php_pgsql_convert_match(Z_STRVAL_PP(val), "^([0-9]{4}[/-][0-9]{1,2}[/-][0-9]{1,2})([ \\t]+(([0-9]{1,2}:[0-9]{1,2}){1}(:[0-9]{1,2}){0,1})){0,1}$", 1 TSRMLS_CC) == FAILURE) {
								err = 1;
							}
							else {
								ZVAL_STRING(new_val, Z_STRVAL_PP(val), 1);
								php_pgsql_add_quotes(new_val, 1 TSRMLS_CC);
							}
						}
						break;
				
					case IS_NULL:
						ZVAL_STRING(new_val, "NULL", 1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Expects string or null for PostgreSQL %s field (%s)", Z_STRVAL_PP(type), field);
				}
				break;
				
			case PG_DATE:
				switch(Z_TYPE_PP(val)) {
					case IS_STRING:
						if (Z_STRLEN_PP(val) == 0) {
							ZVAL_STRING(new_val, "NULL", 1);
						}
						else {
							/* FIXME: better regex must be used */
							if (php_pgsql_convert_match(Z_STRVAL_PP(val), "^([0-9]{4}[/-][0-9]{1,2}[/-][0-9]{1,2})$", 1 TSRMLS_CC) == FAILURE) {
								err = 1;
							}
							else {
								ZVAL_STRING(new_val, Z_STRVAL_PP(val), 1);
								php_pgsql_add_quotes(new_val, 1 TSRMLS_CC);
							}
						}
						break;
				
					case IS_NULL:
						ZVAL_STRING(new_val, "NULL", 1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Expects string or null for PostgreSQL %s field (%s)", Z_STRVAL_PP(type), field);
				}
				break;

			case PG_TIME:
				switch(Z_TYPE_PP(val)) {
					case IS_STRING:
						if (Z_STRLEN_PP(val) == 0) {
							ZVAL_STRING(new_val, "NULL", 1);
						}
						else {
							/* FIXME: better regex must be used */
							if (php_pgsql_convert_match(Z_STRVAL_PP(val), "^(([0-9]{1,2}:[0-9]{1,2}){1}(:[0-9]{1,2}){0,1})){0,1}$", 1 TSRMLS_CC) == FAILURE) {
								err = 1;
							}
							else {
								ZVAL_STRING(new_val, Z_STRVAL_PP(val), 1);
								php_pgsql_add_quotes(new_val, 1 TSRMLS_CC);
							}
						}
						break;
				
					case IS_NULL:
						ZVAL_STRING(new_val, "NULL", 1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Expects string or null for PostgreSQL %s field (%s)", Z_STRVAL_PP(type), field);
				}
				break;

			case PG_INTERVAL:
				switch(Z_TYPE_PP(val)) {
					case IS_STRING:
						if (Z_STRLEN_PP(val) == 0) {
							ZVAL_STRING(new_val, "NULL", 1);
						}
						else {
							/* FIXME: better regex must be used */
							if (php_pgsql_convert_match(Z_STRVAL_PP(val), "^[+-]{0,1}[ \\t]+((second|seconds|minute|minute|hour|hour|day|days|week|weeks|month|monthes|year|years|decade|decades|century|centuries|millennium|millenniums){1,1}[ \\t]+)+([ \\t]+ago){0,1}$", 1 TSRMLS_CC) == FAILURE &&
								php_pgsql_convert_match(Z_STRVAL_PP(val), "^@[ \\t]+[+-]{0,1}[ \\t]+(second|seconds|minute|minute|hour|hour|day|days|week|weeks|month|monthes|year|years|decade|decades|century|centuries|millennium|millenniums){1,1}[ \\t]+)+([ \\t]+ago$", 1 TSRMLS_CC) == FAILURE) {
								err = 1;
							}
							else {
								ZVAL_STRING(new_val, Z_STRVAL_PP(val), 1);
								php_pgsql_add_quotes(new_val, 1 TSRMLS_CC);
							}
						}
						break;
				
					case IS_NULL:
						ZVAL_STRING(new_val, "NULL", 1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Expects string or null for PostgreSQL %s field (%s)", Z_STRVAL_PP(type), field);
				}
				break;
#ifdef HAVE_PQESCAPE
			case PG_BYTEA:
				switch (Z_TYPE_PP(val)) {
					case IS_STRING:
						if (Z_STRLEN_PP(val) == 0) {
							ZVAL_STRING(new_val, "NULL", 1);
						}
						else {
							unsigned char *tmp;
							size_t to_len;
							tmp = PQescapeBytea(Z_STRVAL_PP(val), Z_STRLEN_PP(val), &to_len);
							Z_TYPE_P(new_val) = IS_STRING;
							Z_STRLEN_P(new_val) = to_len-1; /* PQescapeBytea's to_len includes additional '\0' */
							Z_STRVAL_P(new_val) = emalloc(to_len);
							memcpy(Z_STRVAL_P(new_val), tmp, to_len);
							free(tmp);
							php_pgsql_add_quotes(new_val, 1 TSRMLS_CC);
								
						}
						break;
						
					case IS_LONG:
						ZVAL_LONG(new_val, Z_LVAL_PP(val));
						convert_to_string_ex(&new_val);
						break;
						
					case IS_DOUBLE:
						ZVAL_DOUBLE(new_val, Z_DVAL_PP(val));
						convert_to_string_ex(&new_val);
						break;

					case IS_NULL:
						ZVAL_STRING(new_val, "NULL", 1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Expects string, null, long or double value for PostgreSQL '%s' (%s)", Z_STRVAL_PP(type), field);
				}
				break;
				
#endif
			case PG_MACADDR:
				switch(Z_TYPE_PP(val)) {
					case IS_STRING:
						if (Z_STRLEN_PP(val) == 0) {
							ZVAL_STRING(new_val, "NULL", 1);
						}
						else {
							if (php_pgsql_convert_match(Z_STRVAL_PP(val), "^([0-9a-f]{2,2}:){5,5}[0-9a-f]{2,2}$", 1 TSRMLS_CC) == FAILURE) {
								err = 1;
							}
							else {
								ZVAL_STRING(new_val, Z_STRVAL_PP(val), 1);
								php_pgsql_add_quotes(new_val, 1 TSRMLS_CC);
							}
						}
						break;
				
					case IS_NULL:
						ZVAL_STRING(new_val, "NULL", 1);
						break;

					default:
						err = 1;
				}
				PGSQL_CONV_CHECK_IGNORE();
				if (err) {
					php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Expects string or null for PostgreSQL %s field (%s)", Z_STRVAL_PP(type), field);
				}
				break;

				/* bit */
			case PG_BIT:
			case PG_VARBIT:
				/* geometric */
			case PG_LINE:
			case PG_LSEG:
			case PG_POINT:
			case PG_BOX:
			case PG_PATH:
			case PG_POLYGON:
			case PG_CIRCLE:
				php_error_docref(NULL TSRMLS_CC, E_NOTICE, "does not support PostgreSQL '%s' type (%s)", Z_STRVAL_PP(type), field);
				err = 1;
				break;
				
			case PG_UNKNOWN:
			default:
				php_error_docref(NULL TSRMLS_CC, E_NOTICE, "unknown or system data type '%s' for '%s'", Z_STRVAL_PP(type), field);
				err = 1;
				break;
		} /* switch */
		
		if (err) {
			zval_dtor(new_val);
			FREE_ZVAL(new_val);
			break;			
		}
		if (!skip_field) {
			/* If field is NULL and HAS DEFAULT, should be skipped */
			field = php_addslashes(field, strlen(field), &new_len, 0 TSRMLS_CC);
			add_assoc_zval(result, field, new_val);
			efree(field);
		}
	} /* for */
	zval_dtor(meta);
	FREE_ZVAL(meta);

	if (err) {
		/* shouldn't destroy & free zval here */
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */


/* {{{ proto array pg_convert(resource db, string table, array values[, int options])
   Check and convert values for PostgreSQL SQL statement */
PHP_FUNCTION(pg_convert)
{
	zval *pgsql_link, *values;
	char *table_name;
	size_t table_name_len;
	ulong option = 0;
	PGconn *pg_link;
	int id = -1;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
							  "rsa|l", &pgsql_link, &table_name, &table_name_len, &values, &option) == FAILURE) {
		return;
	}
	if (option & ~PGSQL_CONV_OPTS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid option is spedified");
		RETURN_FALSE;
	}
	if (!table_name_len) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Table name is invalid");
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE2(pg_link, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (php_pgsql_flush_query(pg_link TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Detected unhandled result(s) in connection");
	}
	array_init(return_value);
	if (php_pgsql_convert(pg_link, table_name, values, return_value, option TSRMLS_CC) == FAILURE) {
		zval_dtor(return_value);
		RETURN_FALSE;
	}
}
/* }}} */

static int do_exec(smart_str *querystr, int expect, PGconn *pg_link, ulong opt TSRMLS_DC)
{
	if (opt & PGSQL_DML_ASYNC) {
		if (PQsendQuery(pg_link, querystr->c)) {
			return 0;
		}
	}
	else {
		PGresult *pg_result;

		pg_result = PQexec(pg_link, querystr->c);
		if (PQresultStatus(pg_result) == expect) {
			return 0;
		} else {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Failed to execute '%s'", querystr->c);
			PQclear(pg_result);
		}
	}

	return -1;
}

/* {{{ php_pgsql_insert
 */
PHPAPI int php_pgsql_insert(PGconn *pg_link, const char *table, zval *var_array, ulong opt, char **sql TSRMLS_DC)
{
	zval **val, *converted = NULL;
	char buf[256];
	char *fld;
	smart_str querystr = {0};
	int key_type, fld_len, ret = FAILURE;
	ulong num_idx;
	HashPosition pos;

	assert(pg_link != NULL);
	assert(table != NULL);
	assert(Z_TYPE_P(var_array) == IS_ARRAY);

	if (zend_hash_num_elements(Z_ARRVAL_P(var_array)) == 0) {
		return FAILURE;
	}

	/* convert input array if needed */
	if (!(opt & PGSQL_DML_NO_CONV)) {
		MAKE_STD_ZVAL(converted);
		array_init(converted);
		if (php_pgsql_convert(pg_link, table, var_array, converted, (opt & PGSQL_CONV_OPTS) TSRMLS_CC) == FAILURE) {
			goto cleanup;
		}
		var_array = converted;
	}
	
	smart_str_appends(&querystr, "INSERT INTO ");
	smart_str_appends(&querystr, table);
	smart_str_appends(&querystr, " (");
	
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(var_array), &pos);
	while ((key_type = zend_hash_get_current_key_ex(Z_ARRVAL_P(var_array), &fld,
					&fld_len, &num_idx, 0, &pos)) != HASH_KEY_NON_EXISTANT) {
		if (key_type == HASH_KEY_IS_LONG) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE,"Expects associative array for values to be inserted");
			goto cleanup;
		}
		smart_str_appendl(&querystr, fld, fld_len - 1);
		smart_str_appendc(&querystr, ',');
		zend_hash_move_forward_ex(Z_ARRVAL_P(var_array), &pos);
	}
	querystr.len--;
	smart_str_appends(&querystr, ") VALUES (");
	
	/* make values string */
	for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(var_array), &pos);
		 zend_hash_get_current_data_ex(Z_ARRVAL_P(var_array), (void **)&val, &pos) == SUCCESS;
		 zend_hash_move_forward_ex(Z_ARRVAL_P(var_array), &pos)) {
		
		/* we can avoid the key_type check here, because we tested it in the other loop */
		switch(Z_TYPE_PP(val)) {
			case IS_STRING:
				smart_str_appendl(&querystr, Z_STRVAL_PP(val), Z_STRLEN_PP(val));
				break;
			case IS_LONG:
				smart_str_append_long(&querystr, Z_LVAL_PP(val));
				break;
			case IS_DOUBLE:
				smart_str_appendl(&querystr, buf, sprintf(buf, "%f", Z_DVAL_PP(val)));
				break;
			default:
				/* should not happen */
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Report this error to php-dev@lists.php.net. Type = %d", Z_TYPE_PP(val));
				goto cleanup;
				break;
		}
		smart_str_appendc(&querystr, ',');
	}
	/* Remove the trailing "," */
	querystr.len--;
	smart_str_appends(&querystr, ");");
	smart_str_0(&querystr);

	if ((opt & (PGSQL_DML_EXEC|PGSQL_DML_ASYNC)) &&
		do_exec(&querystr, PGRES_COMMAND_OK, pg_link, (opt & PGSQL_CONV_OPTS) TSRMLS_CC) == 0) {
		ret = SUCCESS;
	}
	else if (opt & PGSQL_DML_STRING) {
		ret = SUCCESS;
	}
	
cleanup:
	if (!(opt & PGSQL_DML_NO_CONV)) {
		zval_dtor(converted);			
		FREE_ZVAL(converted);
	}
	if (ret == SUCCESS && (opt & PGSQL_DML_STRING)) {
		*sql = querystr.c;
	}
	else {
		smart_str_free(&querystr);
	}
	return ret;
}
/* }}} */

/* {{{ proto mixed pg_insert(resource db, string table, array values[, int options])
   Insert values (filed=>value) to table */
PHP_FUNCTION(pg_insert)
{
	zval *pgsql_link, *values;
	char *table, *sql = NULL;
	ulong table_len;
	ulong option = PGSQL_DML_EXEC;
	PGconn *pg_link;
	int id = -1, argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "rsa|l",
							  &pgsql_link, &table, &table_len, &values, &option) == FAILURE) {
		return;
	}
	if (option & ~(PGSQL_CONV_OPTS|PGSQL_DML_NO_CONV|PGSQL_DML_EXEC|PGSQL_DML_ASYNC|PGSQL_DML_STRING)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid option is spedified");
		RETURN_FALSE;
	}
	
	ZEND_FETCH_RESOURCE2(pg_link, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (php_pgsql_flush_query(pg_link TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Detected unhandled result(s) in connection");
	}
	if (php_pgsql_insert(pg_link, table, values, option, &sql TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	if (option & PGSQL_DML_STRING) {
		RETURN_STRING(sql, 0);
	}
	RETURN_TRUE;
}
/* }}} */

static inline int build_assignment_string(smart_str *querystr, HashTable *ht, const char *pad, int pad_len TSRMLS_DC)
{
	HashPosition pos;
	size_t fld_len;
	int key_type;
	ulong num_idx;
	char *fld;
	char buf[256];
	zval **val;

	for (zend_hash_internal_pointer_reset_ex(ht, &pos);
		 zend_hash_get_current_data_ex(ht, (void **)&val, &pos) == SUCCESS;
		 zend_hash_move_forward_ex(ht, &pos)) {
		 key_type = zend_hash_get_current_key_ex(ht, &fld, &fld_len, &num_idx, 0, &pos);		
		if (key_type == HASH_KEY_IS_LONG) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE,"Expects associative array for values to be inserted");
			return -1;
		}
		smart_str_appendl(querystr, fld, fld_len - 1);
		smart_str_appendc(querystr, '=');
		
		switch(Z_TYPE_PP(val)) {
			case IS_STRING:
				smart_str_appendl(querystr, Z_STRVAL_PP(val), Z_STRLEN_PP(val));
				break;
			case IS_LONG:
				smart_str_append_long(querystr, Z_LVAL_PP(val));
				break;
			case IS_DOUBLE:
				smart_str_appendl(querystr, buf, sprintf(buf, "%f", Z_DVAL_PP(val)));
				break;
			default:
				/* should not happen */
				php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Expects scaler values other than null. Need to convert?");
				return -1;
		}
		smart_str_appendl(querystr, pad, pad_len);
	}
	querystr->len -= pad_len;

	return 0;
}

/* {{{ php_pgsql_update
 */
PHPAPI int php_pgsql_update(PGconn *pg_link, const char *table, zval *var_array, zval *ids_array, ulong opt, char **sql TSRMLS_DC) 
{
	zval *var_converted = NULL, *ids_converted = NULL;
	smart_str querystr = {0};
	int ret = FAILURE;

	assert(pg_link != NULL);
	assert(table != NULL);
	assert(Z_TYPE_P(var_array) == IS_ARRAY);
	assert(Z_TYPE_P(ids_array) == IS_ARRAY);
	assert(!(opt & ~(PGSQL_CONV_OPTS|PGSQL_DML_NO_CONV|PGSQL_DML_EXEC|PGSQL_DML_STRING)));

	if (zend_hash_num_elements(Z_ARRVAL_P(var_array)) == 0
			|| zend_hash_num_elements(Z_ARRVAL_P(ids_array)) == 0) {
		return FAILURE;
	}

	if (!(opt & PGSQL_DML_NO_CONV)) {
		MAKE_STD_ZVAL(var_converted);
		array_init(var_converted);
		if (php_pgsql_convert(pg_link, table, var_array, var_converted, (opt & PGSQL_CONV_OPTS) TSRMLS_CC) == FAILURE) {
			goto cleanup;
		}
		var_array = var_converted;
		MAKE_STD_ZVAL(ids_converted);
		array_init(ids_converted);
		if (php_pgsql_convert(pg_link, table, ids_array, ids_converted, (opt & PGSQL_CONV_OPTS) TSRMLS_CC) == FAILURE) {
			goto cleanup;
		}
		ids_array = ids_converted;
	}

	smart_str_appends(&querystr, "UPDATE ");
	smart_str_appends(&querystr, table);
	smart_str_appends(&querystr, " SET ");

	if (build_assignment_string(&querystr, Z_ARRVAL_P(var_array), ",", 1 TSRMLS_CC))
		goto cleanup;
	
	smart_str_appends(&querystr, " WHERE ");
	
	if (build_assignment_string(&querystr, Z_ARRVAL_P(ids_array), " AND ", sizeof(" AND ")-1 TSRMLS_CC))
		goto cleanup;

	smart_str_appendc(&querystr, ';');	
	smart_str_0(&querystr);

	if (do_exec(&querystr, PGRES_COMMAND_OK, pg_link, opt TSRMLS_CC) == 0)
		ret = SUCCESS;

cleanup:
	if (var_converted) {
		zval_dtor(var_converted);
		FREE_ZVAL(var_converted);
	}
	if (ids_converted) {
		zval_dtor(ids_converted);
		FREE_ZVAL(ids_converted);
	}
	if (ret == SUCCESS && (opt & PGSQL_DML_STRING)) {
		*sql = querystr.c;
	}
	else {
		smart_str_free(&querystr);
	}
	return ret;
}
/* }}} */

/* {{{ proto mixed pg_update(resource db, string table, array fields, array ids[, int options])
   Update table using values (field=>value) and ids (id=>value) */
PHP_FUNCTION(pg_update)
{
	zval *pgsql_link, *values, *ids;
	char *table, *sql = NULL;
	ulong table_len;
	ulong option =  PGSQL_DML_EXEC;
	PGconn *pg_link;
	int id = -1, argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "rsaa|l",
							  &pgsql_link, &table, &table_len, &values, &ids, &option) == FAILURE) {
		return;
	}
	if (option & ~(PGSQL_CONV_OPTS|PGSQL_DML_NO_CONV|PGSQL_DML_EXEC|PGSQL_DML_STRING)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid option is spedified");
		RETURN_FALSE;
	}
	
	ZEND_FETCH_RESOURCE2(pg_link, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (php_pgsql_flush_query(pg_link TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Detected unhandled result(s) in connection");
	}
	if (php_pgsql_update(pg_link, table, values, ids, option, &sql TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	if (option & PGSQL_DML_STRING) {
		RETURN_STRING(sql, 0);
	}
	RETURN_TRUE;
} 
/* }}} */

/* {{{ php_pgsql_delete
 */
PHPAPI int php_pgsql_delete(PGconn *pg_link, const char *table, zval *ids_array, ulong opt, char **sql TSRMLS_DC) 
{
	zval *ids_converted = NULL;
	smart_str querystr = {0};
	int ret = FAILURE;

	assert(pg_link != NULL);
	assert(table != NULL);
	assert(Z_TYPE_P(ids_array) == IS_ARRAY);
	assert(!(opt & ~(PGSQL_CONV_FORCE_NULL|PGSQL_DML_EXEC|PGSQL_DML_STRING)));
	
	if (zend_hash_num_elements(Z_ARRVAL_P(ids_array)) == 0) {
		return FAILURE;
	}

	if (!(opt & PGSQL_DML_NO_CONV)) {
		MAKE_STD_ZVAL(ids_converted);
		array_init(ids_converted);
		if (php_pgsql_convert(pg_link, table, ids_array, ids_converted, (opt & PGSQL_CONV_OPTS) TSRMLS_CC) == FAILURE) {
			goto cleanup;
		}
		ids_array = ids_converted;
	}

	smart_str_appends(&querystr, "DELETE FROM ");
	smart_str_appends(&querystr, table);
	smart_str_appends(&querystr, " WHERE ");

	if (build_assignment_string(&querystr, Z_ARRVAL_P(ids_array), " AND ", sizeof(" AND ")-1 TSRMLS_CC))
		goto cleanup;

	smart_str_appendc(&querystr, ';');
	smart_str_0(&querystr);

	if (do_exec(&querystr, PGRES_COMMAND_OK, pg_link, opt TSRMLS_CC) == 0)
		ret = SUCCESS;

cleanup:
	if (!(opt & PGSQL_DML_NO_CONV)) {
		zval_dtor(ids_converted);			
		FREE_ZVAL(ids_converted);
	}
	if (ret == SUCCESS && (opt & PGSQL_DML_STRING)) {
		*sql = estrdup(querystr.c);
	}
	else {
		smart_str_free(&querystr);
	}
	return ret;
}
/* }}} */

/* {{{ proto mixed pg_delete(resource db, string table, array ids[, int options])
   Delete records has ids (id=>value) */
PHP_FUNCTION(pg_delete)
{
	zval *pgsql_link, *ids;
	char *table, *sql = NULL;
	ulong table_len, option = PGSQL_DML_EXEC;
	PGconn *pg_link;
	int id = -1, argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "rsa|l",
							  &pgsql_link, &table, &table_len, &ids, &option) == FAILURE) {
		return;
	}
	if (option & ~(PGSQL_CONV_FORCE_NULL|PGSQL_DML_NO_CONV|PGSQL_DML_EXEC|PGSQL_DML_STRING)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid option is spedified");
		RETURN_FALSE;
	}
	
	ZEND_FETCH_RESOURCE2(pg_link, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (php_pgsql_flush_query(pg_link TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Detected unhandled result(s) in connection");
	}
	if (php_pgsql_delete(pg_link, table, ids, option, &sql TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	if (option & PGSQL_DML_STRING) {
		RETURN_STRING(sql, 0);
	}
	RETURN_TRUE;
} 
/* }}} */

/* {{{ php_pgsql_result2array
 */
PHPAPI int php_pgsql_result2array(PGresult *pg_result, zval *ret_array TSRMLS_DC) 
{
	zval *row;
	char *field_name, *element, *data;
	size_t num_fields, element_len, data_len;
	int pg_numrows, pg_row;
	uint i;
	assert(Z_TYPE_P(ret_array) == IS_ARRAY);

	if ((pg_numrows = PQntuples(pg_result)) <= 0) {
		return FAILURE;
	}
	for (pg_row = 0; pg_row < pg_numrows; pg_row++) {
		MAKE_STD_ZVAL(row);
		array_init(row);
		add_index_zval(ret_array, pg_row, row);
		for (i = 0, num_fields = PQnfields(pg_result); i < num_fields; i++) {
			if (PQgetisnull(pg_result, pg_row, i)) {
				field_name = PQfname(pg_result, i);
				add_assoc_null(row, field_name);
			} else {
				element = PQgetvalue(pg_result, pg_row, i);
				element_len = (element ? strlen(element) : 0);
				if (element) {
					if (PG(magic_quotes_runtime)) {
						data = php_addslashes(element, element_len, &data_len, 0 TSRMLS_CC);
					} else {
						data = safe_estrndup(element, element_len);
						data_len = element_len;
					}
					field_name = PQfname(pg_result, i);
					add_assoc_stringl(row, field_name, data, data_len, 0);
				}
			}
		}
	}
	return SUCCESS;
}
/* }}} */

/* {{{ php_pgsql_select
 */
PHPAPI int php_pgsql_select(PGconn *pg_link, const char *table, zval *ids_array, zval *ret_array, ulong opt, char **sql TSRMLS_DC) 
{
	zval *ids_converted = NULL;
	smart_str querystr = {0};
	int ret = FAILURE;
	PGresult *pg_result;

	assert(pg_link != NULL);
	assert(table != NULL);
	assert(Z_TYPE_P(ids_array) == IS_ARRAY);
	assert(Z_TYPE_P(ret_array) == IS_ARRAY);
	assert(!(opt & ~(PGSQL_CONV_OPTS|PGSQL_DML_NO_CONV|PGSQL_DML_EXEC|PGSQL_DML_ASYNC|PGSQL_DML_STRING)));
	
	if (zend_hash_num_elements(Z_ARRVAL_P(ids_array)) == 0) {
		return FAILURE;
	}

	if (!(opt & PGSQL_DML_NO_CONV)) {
		MAKE_STD_ZVAL(ids_converted);
		array_init(ids_converted);
		if (php_pgsql_convert(pg_link, table, ids_array, ids_converted, (opt & PGSQL_CONV_OPTS) TSRMLS_CC) == FAILURE) {
			goto cleanup;
		}
		ids_array = ids_converted;
	}

	smart_str_appends(&querystr, "SELECT * FROM ");
	smart_str_appends(&querystr, table);
	smart_str_appends(&querystr, " WHERE ");

	if (build_assignment_string(&querystr, Z_ARRVAL_P(ids_array), " AND ", sizeof(" AND ")-1 TSRMLS_CC))
		goto cleanup;

	smart_str_appendc(&querystr, ';');
	smart_str_0(&querystr);

	pg_result = PQexec(pg_link, querystr.c);
	if (PQresultStatus(pg_result) == PGRES_TUPLES_OK) {
		ret = php_pgsql_result2array(pg_result, ret_array TSRMLS_CC);
	} else {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Failed to execute '%s'", querystr.c);
		PQclear(pg_result);
	}
	
cleanup:
	if (!(opt & PGSQL_DML_NO_CONV)) {
		zval_dtor(ids_converted);			
		FREE_ZVAL(ids_converted);
	}
	if (ret == SUCCESS && (opt & PGSQL_DML_STRING)) {
		*sql = querystr.c;
	}
	else {
		smart_str_free(&querystr);
	}
	return ret;
}
/* }}} */

/* {{{ proto mixed pg_select(resource db, string table, array ids[, int options])
   Select records that has ids (id=>value) */
PHP_FUNCTION(pg_select)
{
	zval *pgsql_link, *ids;
	char *table, *sql = NULL;
	ulong table_len, option = PGSQL_DML_EXEC;
	PGconn *pg_link;
	int id = -1, argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "rsa|l",
							  &pgsql_link, &table, &table_len, &ids, &option) == FAILURE) {
		return;
	}
	if (option & ~(PGSQL_CONV_FORCE_NULL|PGSQL_DML_NO_CONV|PGSQL_DML_EXEC|PGSQL_DML_ASYNC|PGSQL_DML_STRING)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid option is spedified");
		RETURN_FALSE;
	}
	
	ZEND_FETCH_RESOURCE2(pg_link, PGconn *, &pgsql_link, id, "PostgreSQL link", le_link, le_plink);

	if (php_pgsql_flush_query(pg_link TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Detected unhandled result(s) in connection");
	}
	array_init(return_value);
	if (php_pgsql_select(pg_link, table, ids, return_value, option, &sql TSRMLS_CC) == FAILURE) {
		zval_dtor(return_value);
		RETURN_FALSE;
	}
	if (option & PGSQL_DML_STRING) {
		zval_dtor(return_value);
		RETURN_STRING(sql, 0);
	}
	return;
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
