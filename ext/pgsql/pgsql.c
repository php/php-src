/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_01.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Zeev Suraski <zeev@zend.com>                                |
   |          Jouni Ahto <jah@cultnet.fi> (large object interface)        |
   +----------------------------------------------------------------------+
 */
 
/* $Id$ */

#include <stdlib.h>

#if COMPILE_DL
#include "dl/phpdl.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/php_standard.h"
#include "php_pgsql.h"
#include "php_globals.h"

#if HAVE_PGSQL


#define PGSQL_ASSOC		1<<0
#define PGSQL_NUM		1<<1
#define PGSQL_BOTH		(PGSQL_ASSOC|PGSQL_NUM)


function_entry pgsql_functions[] = {
	PHP_FE(pg_connect,		NULL)
	PHP_FE(pg_pconnect,		NULL)
	PHP_FE(pg_close,		NULL)
	PHP_FE(pg_cmdtuples,	NULL)
	PHP_FE(pg_dbname,		NULL)
	PHP_FE(pg_errormessage,	NULL)
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
	{NULL, NULL, NULL}
};

zend_module_entry pgsql_module_entry = {
	"PostgreSQL", pgsql_functions, PHP_MINIT(pgsql), PHP_MSHUTDOWN(pgsql), PHP_RINIT(pgsql), NULL, NULL, STANDARD_MODULE_PROPERTIES
};

#if COMPILE_DL
PHP_PGSQL_API zend_module_entry *get_module() { return &pgsql_module_entry; }
#endif

static int le_link,le_plink,le_result,le_lofp,le_string;

#ifdef ZTS
int pgsql_globals_id;
#else
PHP_PGSQL_API php_pgsql_globals pgsql_globals;
#endif

static void _close_pgsql_link(PGconn *link)
{
	PGLS_FETCH();

	PQfinish(link);
	PGG(num_links)--;
}


static void _close_pgsql_plink(PGconn *link)
{
	PGLS_FETCH();

	PQfinish(link);
	PGG(num_persistent)--;
	PGG(num_links)--;
}


static void _free_ptr(pgLofp *lofp)
{
	efree(lofp);
}


static void _free_result(pgsql_result_handle *pg_result)
{
	PQclear(pg_result->result);
	efree(pg_result);
}

PHP_INI_BEGIN()
	STD_PHP_INI_BOOLEAN("pgsql.allow_persistent",	"1",	PHP_INI_SYSTEM,		OnUpdateInt,		allow_persistent,	php_pgsql_globals,		pgsql_globals)
	STD_PHP_INI_ENTRY_EX("pgsql.max_persistent",	"-1",	PHP_INI_SYSTEM,		OnUpdateInt,		max_persistent,		php_pgsql_globals,		pgsql_globals,	display_link_numbers)
	STD_PHP_INI_ENTRY_EX("pgsql.max_links",		"-1",	PHP_INI_SYSTEM,			OnUpdateInt,		max_links,			php_pgsql_globals,		pgsql_globals,	display_link_numbers)
PHP_INI_END()


static void php_pgsql_init_globals(PGLS_D)
{
	PGG(num_persistent) = 0;
}

PHP_MINIT_FUNCTION(pgsql)
{
#ifdef ZTS
	pgsql_globals_id = ts_allocate_id(sizeof(php_pgsql_globals), (ts_allocate_ctor) php_pgsql_init_globals, NULL);
#else
	php_pgsql_init_globals(PGLS_C);
#endif

	REGISTER_INI_ENTRIES();
	
	le_link = register_list_destructors(_close_pgsql_link,NULL);
	le_plink = register_list_destructors(NULL,_close_pgsql_plink);
	/*	PGG(le_result = register_list_destructors(PQclear,NULL); */
	le_result = register_list_destructors(_free_result,NULL);
	le_lofp = register_list_destructors(_free_ptr,NULL);
	le_string = register_list_destructors(_free_ptr,NULL);

	REGISTER_LONG_CONSTANT("PGSQL_ASSOC", PGSQL_ASSOC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_NUM", PGSQL_NUM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_BOTH", PGSQL_BOTH, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}


PHP_MSHUTDOWN_FUNCTION(pgsql)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}


PHP_RINIT_FUNCTION(pgsql)
{
	PGLS_FETCH();

	PGG(default_link)=-1;
	PGG(num_links) = PGG(num_persistent);
	return SUCCESS;
}


void php_pgsql_do_connect(INTERNAL_FUNCTION_PARAMETERS,int persistent)
{
	char *host=NULL,*port=NULL,*options=NULL,*tty=NULL,*dbname=NULL,*connstring=NULL;
	char *hashed_details;
	int hashed_details_length;
	PGconn *pgsql;
	PGLS_FETCH();
	
	switch(ARG_COUNT(ht)) {
		case 1: { /* new style, using connection string */
				pval *yyconnstring;
				if (getParameters(ht, 1, &yyconnstring) == FAILURE) {
					RETURN_FALSE;
				}
				convert_to_string(yyconnstring);
				connstring = yyconnstring->value.str.val;
				hashed_details_length = yyconnstring->value.str.len+5+1;
				hashed_details = (char *) emalloc(hashed_details_length+1);
				sprintf(hashed_details,"pgsql_%s",connstring); /* SAFE */
			}
			break;
		case 3: { /* host, port, dbname */
				pval *yyhost, *yyport, *yydbname;
				
				if (getParameters(ht, 3, &yyhost, &yyport, &yydbname) == FAILURE) {
					RETURN_FALSE;
				}
				convert_to_string(yyhost);
				convert_to_string(yyport);
				convert_to_string(yydbname);
				host = yyhost->value.str.val;
				port = yyport->value.str.val;
				dbname = yydbname->value.str.val;
				options=tty=NULL;
				hashed_details_length = yyhost->value.str.len+yyport->value.str.len+yydbname->value.str.len+5+5;
				hashed_details = (char *) emalloc(hashed_details_length+1);
				sprintf(hashed_details,"pgsql_%s_%s___%s",host,port,dbname);  /* SAFE */
			}
			break;
		case 4: { /* host, port, options, dbname */
				pval *yyhost, *yyport, *yyoptions, *yydbname;
				
				if (getParameters(ht, 4, &yyhost, &yyport, &yyoptions, &yydbname) == FAILURE) {
					RETURN_FALSE;
				}
				convert_to_string(yyhost);
				convert_to_string(yyport);
				convert_to_string(yyoptions);
				convert_to_string(yydbname);
				host = yyhost->value.str.val;
				port = yyport->value.str.val;
				options = yyoptions->value.str.val;
				dbname = yydbname->value.str.val;
				tty=NULL;
				hashed_details_length = yyhost->value.str.len+yyport->value.str.len+yyoptions->value.str.len+yydbname->value.str.len+5+5;
				hashed_details = (char *) emalloc(hashed_details_length+1);
				sprintf(hashed_details,"pgsql_%s_%s_%s__%s",host,port,options,dbname);  /* SAFE */
			}
			break;
		case 5: { /* host, port, options, tty, dbname */
				pval *yyhost, *yyport, *yyoptions, *yytty, *yydbname;
				
				if (getParameters(ht, 5, &yyhost, &yyport, &yyoptions, &yytty, &yydbname) == FAILURE) {
					RETURN_FALSE;
				}
				convert_to_string(yyhost);
				convert_to_string(yyport);
				convert_to_string(yyoptions);
				convert_to_string(yytty);
				convert_to_string(yydbname);
				host = yyhost->value.str.val;
				port = yyport->value.str.val;
				options = yyoptions->value.str.val;
				tty = yytty->value.str.val;
				dbname = yydbname->value.str.val;
				hashed_details_length = yyhost->value.str.len+yyport->value.str.len+yyoptions->value.str.len+yytty->value.str.len+yydbname->value.str.len+5+5;
				hashed_details = (char *) emalloc(hashed_details_length+1);
				sprintf(hashed_details,"pgsql_%s_%s_%s_%s_%s",host,port,options,tty,dbname);  /* SAFE */
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	if (persistent) {
		list_entry *le;
		
		/* try to find if we already have this link in our persistent list */
		if (zend_hash_find(&EG(persistent_list), hashed_details, hashed_details_length+1, (void **) &le)==FAILURE) {  /* we don't */
			list_entry new_le;
			
			if (PGG(max_links)!=-1 && PGG(num_links)>=PGG(max_links)) {
				php_error(E_WARNING,"PostgresSQL:  Too many open links (%d)",PGG(num_links));
				efree(hashed_details);
				RETURN_FALSE;
			}
			if (PGG(max_persistent)!=-1 && PGG(num_persistent)>=PGG(max_persistent)) {
				php_error(E_WARNING,"PostgresSQL:  Too many open persistent links (%d)",PGG(num_persistent));
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
				php_error(E_WARNING,"Unable to connect to PostgresSQL server:  %s",PQerrorMessage(pgsql));
				efree(hashed_details);
				RETURN_FALSE;
			}

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
					php_error(E_WARNING,"PostgresSQL link lost, unable to reconnect");
					zend_hash_del(&EG(persistent_list),hashed_details,hashed_details_length+1);
					efree(hashed_details);
					RETURN_FALSE;
				}
			}
			pgsql = (PGconn *) le->ptr;
		}
		return_value->value.lval = zend_list_insert(pgsql,le_plink);
		return_value->type = IS_LONG;
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
				return_value->value.lval = PGG(default_link) = link;
				return_value->type = IS_LONG;
				efree(hashed_details);
				return;
			} else {
				zend_hash_del(&EG(regular_list),hashed_details,hashed_details_length+1);
			}
		}
		if (PGG(max_links)!=-1 && PGG(num_links)>=PGG(max_links)) {
			php_error(E_WARNING,"PostgresSQL:  Too many open links (%d)",PGG(num_links));
			efree(hashed_details);
			RETURN_FALSE;
		}
		if (connstring) {
			pgsql=PQconnectdb(connstring);
		} else {
			pgsql=PQsetdb(host,port,options,tty,dbname);
		}
		if (pgsql==NULL || PQstatus(pgsql)==CONNECTION_BAD) {
			php_error(E_WARNING,"Unable to connect to PostgresSQL server:  %s",PQerrorMessage(pgsql));
			efree(hashed_details);
			RETURN_FALSE;
		}

		/* add it to the list */
		return_value->value.lval = zend_list_insert(pgsql,le_link);
		return_value->type = IS_LONG;

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
	PGG(default_link)=return_value->value.lval;
}


int php_pgsql_get_default_link(INTERNAL_FUNCTION_PARAMETERS)
{
	PGLS_FETCH();

	if (PGG(default_link)==-1) { /* no link opened yet, implicitly open one */
		ht = 0;
		php_pgsql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);
	}
	return PGG(default_link);
}


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
	pval *pgsql_link;
	int id,type;
	PGconn *pgsql;
	PGLS_FETCH();
	
	switch (ARG_COUNT(ht)) {
		case 0:
			id = PGG(default_link);
			break;
		case 1:
			if (getParameters(ht, 1, &pgsql_link)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(pgsql_link);
			id = pgsql_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	pgsql = (PGconn *) zend_list_find(id,&type);
	if (type!=le_link && type!=le_plink) {
		php_error(E_WARNING,"%d is not a PostgresSQL link index",id);
		RETURN_FALSE;
	}
	
	zend_list_delete(pgsql_link->value.lval);
	RETURN_TRUE;
}
/* }}} */


#define PHP_PG_DBNAME 1
#define PHP_PG_ERROR_MESSAGE 2
#define PHP_PG_OPTIONS 3
#define PHP_PG_PORT 4
#define PHP_PG_TTY 5
#define PHP_PG_HOST 6

void php_pgsql_get_link_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type)
{
	pval *pgsql_link;
	int id,type;
	PGconn *pgsql;
	PGLS_FETCH();

	switch(ARG_COUNT(ht)) {
		case 0:
			id = PGG(default_link);
			break;
		case 1:
			if (getParameters(ht, 1, &pgsql_link)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(pgsql_link);
			id = pgsql_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	pgsql = (PGconn *) zend_list_find(id,&type);
	if (type!=le_link && type!=le_plink) {
		php_error(E_WARNING,"%d is not a PostgresSQL link index",id);
		RETURN_FALSE;
	}
	
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
	pval *query,*pgsql_link;
	int id,type;
	PGconn *pgsql;
	PGresult *pgsql_result;
	ExecStatusType  status;
	pgsql_result_handle *pg_result;
	PGLS_FETCH();

	switch(ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &query)==FAILURE) {
				RETURN_FALSE;
			}
			id = PGG(default_link);
			break;
		case 2:
			if (getParameters(ht, 2, &pgsql_link, &query)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(pgsql_link);
			id = pgsql_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	pgsql = (PGconn *) zend_list_find(id,&type);
	if (type!=le_link && type!=le_plink) {
		php_error(E_WARNING,"%d is not a PostgresSQL link index",id);
		RETURN_FALSE;
	}
	
	convert_to_string(query);
	pgsql_result=PQexec(pgsql,query->value.str.val);
	
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
			php_error(E_WARNING,"PostgresSQL query failed:  %s",PQerrorMessage(pgsql));
			RETURN_FALSE;
			break;
		case PGRES_COMMAND_OK: /* successful command that did not return rows */
		default:
			if (pgsql_result) {
				pg_result = (pgsql_result_handle *) emalloc(sizeof(pgsql_result_handle));
				pg_result->conn = pgsql;
				pg_result->result = pgsql_result;
				return_value->value.lval = zend_list_insert(pg_result,le_result);
				return_value->type = IS_LONG;
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

void php_pgsql_get_result_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type)
{
	pval *result;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;
	int type;

	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	pg_result = (pgsql_result_handle *) zend_list_find(result->value.lval,&type);

	if (type!=le_result) {
		php_error(E_WARNING,"%d is not a PostgresSQL result index",result->value.lval);
		RETURN_FALSE;
	}
	
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

		if ((result=PQexec(pgsql,"select oid,typname from pg_type"))==NULL) {
			return empty_string;
		}
		num_rows=PQntuples(result);
		oid_offset = PQfnumber(result,"oid");
		name_offset = PQfnumber(result,"typname");
		
		for (i=0; i<num_rows; i++) {
			if ((tmp_oid=PQgetvalue(result,i,oid_offset))==NULL) {
				continue;
			}
			snprintf(hashed_oid_key,31,"pgsql_oid_%s",tmp_oid);
			if ((tmp_name=PQgetvalue(result,i,name_offset))==NULL) {
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
			

#define PHP_PG_FIELD_NAME 1
#define PHP_PG_FIELD_SIZE 2
#define PHP_PG_FIELD_TYPE 3

void php_pgsql_get_field_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type)
{
	pval *result,*field;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;
	int type;
	
	if (ARG_COUNT(ht)!=2 || getParameters(ht, 2, &result, &field)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	pg_result = (pgsql_result_handle *) zend_list_find(result->value.lval,&type);
	
	if (type!=le_result) {
		php_error(E_WARNING,"%d is not a PostgresSQL result index",result->value.lval);
		RETURN_FALSE;
	}
	
	pgsql_result = pg_result->result;
	convert_to_long(field);
	
	if (field->value.lval<0 || field->value.lval>=PQnfields(pgsql_result)) {
		php_error(E_WARNING,"Bad field offset specified");
		RETURN_FALSE;
	}
	
	switch (entry_type) {
		case PHP_PG_FIELD_NAME:
			return_value->value.str.val = PQfname(pgsql_result,field->value.lval);
			return_value->value.str.len = strlen(return_value->value.str.val);
			return_value->value.str.val = estrndup(return_value->value.str.val,return_value->value.str.len);
			return_value->type = IS_STRING;
			break;
		case PHP_PG_FIELD_SIZE:
			return_value->value.lval = PQfsize(pgsql_result,field->value.lval);
			return_value->type = IS_LONG;
			break;
		case PHP_PG_FIELD_TYPE:
			return_value->value.str.val = get_field_name(pg_result->conn,PQftype(pgsql_result,field->value.lval),&EG(regular_list));
			return_value->value.str.len = strlen(return_value->value.str.val);
			return_value->type = IS_STRING;
			break;
		default:
			RETURN_FALSE;
	}
}

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
	pval *result,*field;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;
	int type;

	if (ARG_COUNT(ht)!=2 || getParameters(ht, 2, &result, &field)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	pg_result = (pgsql_result_handle *) zend_list_find(result->value.lval,&type);
	
	if (type!=le_result) {
		php_error(E_WARNING,"%d is not a PostgresSQL result index",result->value.lval);
		RETURN_FALSE;
	}
	pgsql_result = pg_result->result;
	
	convert_to_string(field);
	return_value->value.lval = PQfnumber(pgsql_result,field->value.str.val);
	return_value->type = IS_LONG;
}
/* }}} */

/* {{{ proto mixed pg_result(int result, int row_number, mixed field_name)
   Returns values from a result identifier */
PHP_FUNCTION(pg_result)
{
	pval *result, *row, *field=NULL;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;
	int type,field_offset;
	
	if (ARG_COUNT(ht)!=3 || getParameters(ht, 3, &result, &row, &field)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	pg_result = (pgsql_result_handle *) zend_list_find(result->value.lval,&type);
	
	if (type!=le_result) {
		php_error(E_WARNING,"%d is not a PostgresSQL result index",result->value.lval);
		RETURN_FALSE;
	}
	pgsql_result = pg_result->result;
	
	convert_to_long(row);
	if (row->value.lval<0 || row->value.lval>=PQntuples(pgsql_result)) {
		php_error(E_WARNING,"Unable to jump to row %d on PostgresSQL result index %d",row->value.lval,result->value.lval);
		RETURN_FALSE;
	}
	switch(field->type) {
		case IS_STRING:
			field_offset = PQfnumber(pgsql_result,field->value.str.val);
			break;
		default:
			convert_to_long(field);
			field_offset = field->value.lval;
			break;
	}
	if (field_offset<0 || field_offset>=PQnfields(pgsql_result)) {
		php_error(E_WARNING,"Bad column offset specified");
		RETURN_FALSE;
	}
	
	return_value->value.str.val = PQgetvalue(pgsql_result,row->value.lval,field_offset);
	return_value->value.str.len = (return_value->value.str.val ? strlen(return_value->value.str.val) : 0);
	return_value->value.str.val = safe_estrndup(return_value->value.str.val,return_value->value.str.len);
	return_value->type = IS_STRING;
}
/* }}} */


static void php_pgsql_fetch_hash(INTERNAL_FUNCTION_PARAMETERS, int result_type)
{
	pval *result, *row, *arg3;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;
	int type;
	int i,num_fields;
	char *element,*field_name;
	uint element_len;
	PLS_FETCH();

	switch (ARG_COUNT(ht)) {
		case 2:
			if (getParameters(ht, 2, &result, &row)==FAILURE) {
				RETURN_FALSE;
			}
			if (!result_type) {
				result_type = PGSQL_BOTH;
			}
			break;
		case 3:
			if (getParameters(ht, 3, &result, &row, &arg3)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(arg3);
			result_type = arg3->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	convert_to_long(result);
	pg_result = (pgsql_result_handle *) zend_list_find(result->value.lval,&type);
	
	if (type!=le_result) {
		php_error(E_WARNING,"%d is not a PostgresSQL result index",result->value.lval);
		RETURN_FALSE;
	}
	pgsql_result = pg_result->result;
	
	convert_to_long(row);
	if (row->value.lval<0 || row->value.lval>=PQntuples(pgsql_result)) {
		php_error(E_WARNING,"Unable to jump to row %d on PostgresSQL result index %d",row->value.lval,result->value.lval);
		RETURN_FALSE;
	}
	array_init(return_value);
	for (i=0,num_fields=PQnfields(pgsql_result); i<num_fields; i++) {
		element = PQgetvalue(pgsql_result,row->value.lval,i);
		element_len = (element ? strlen(element) : 0);
		if (element) {
			char *data;
			int data_len;
			int should_copy=0;

			if (PG(magic_quotes_runtime)) {
				data = php_addslashes(element,element_len,&data_len,0);
			} else {
				data = safe_estrndup(element,element_len);
				data_len = element_len;
			}
			
			if (result_type & PGSQL_NUM) {
				add_index_stringl(return_value, i, data, data_len, should_copy);
				should_copy=1;
			}
			
			if (result_type & PGSQL_ASSOC) {
				field_name = PQfname(pgsql_result,i);
				add_assoc_stringl(return_value, field_name, data, data_len, should_copy);
			}
        } else {
            /* NULL field, don't set it */
            /* add_get_index_stringl(return_value, i, empty_string, 0, (void **) &pval_ptr); */
        }
	}
}


/* {{{ proto array pg_fetchrow(int result, int row)
   Get a row as an enumerated array */ 
PHP_FUNCTION(pg_fetch_row)
{
	php_pgsql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, PGSQL_NUM);
}
/* }}} */

/* ??  This is a rather odd function - why not just point pg_fetcharray() directly at fetch_hash ? -RL */
/* {{{ proto array pg_fetch_array(int result, int row [, int result_type])
   Fetch a row as an array */
PHP_FUNCTION(pg_fetch_array)
{
	php_pgsql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto object pg_fetch_object(int result, int row [, int result_type])
   Fetch a row as an object */
PHP_FUNCTION(pg_fetch_object)
{
	php_pgsql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
	if (return_value->type==IS_ARRAY) {
		return_value->type=IS_OBJECT;
		return_value->value.obj.properties = return_value->value.ht;
		return_value->value.obj.ce = &zend_standard_class_def;
	}
}
/* }}} */

#define PHP_PG_DATA_LENGTH 1
#define PHP_PG_DATA_ISNULL 2

void php_pgsql_data_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type)
{
	pval *result,*row,*field;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;
	int type,field_offset;
	
	if (ARG_COUNT(ht)!=3 || getParameters(ht, 3, &result, &row, &field)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	pg_result = (pgsql_result_handle *) zend_list_find(result->value.lval,&type);
	
	if (type!=le_result) {
		php_error(E_WARNING,"%d is not a PostgresSQL result index",result->value.lval);
		RETURN_FALSE;
	}
	pgsql_result = pg_result->result;
	
	convert_to_long(row);
	if (row->value.lval<0 || row->value.lval>=PQntuples(pgsql_result)) {
		php_error(E_WARNING,"Unable to jump to row %d on PostgresSQL result index %d",row->value.lval,result->value.lval);
		RETURN_FALSE;
	}
	switch(field->type) {
		case IS_STRING:
			field_offset = PQfnumber(pgsql_result,field->value.str.val);
			break;
		default:
			convert_to_long(field);
			field_offset = field->value.lval;
			break;
	}
	if (field_offset<0 || field_offset>=PQnfields(pgsql_result)) {
		php_error(E_WARNING,"Bad column offset specified");
		RETURN_FALSE;
	}
	
	switch (entry_type) {
		case PHP_PG_DATA_LENGTH:
			return_value->value.lval = PQgetlength(pgsql_result,row->value.lval,field_offset);
			break;
		case PHP_PG_DATA_ISNULL:
			return_value->value.lval = PQgetisnull(pgsql_result,row->value.lval,field_offset);
			break;
	}
	return_value->type = IS_LONG;
}

/* {{{ proto int pg_fieldprtlen(int result, int row, mixed field_name_or_number)
   Returns the printed length */
PHP_FUNCTION(pg_fieldprtlen)
{
	php_pgsql_data_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_PG_DATA_LENGTH);
}
/* }}} */

/* {{{ proto int pg_fieldisnull(int result, int row, mixed field_name_or_number)
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
	pval *result;
	pgsql_result_handle *pg_result;
	int type;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	if (result->value.lval==0) {
		RETURN_FALSE;
	}
	pg_result = (pgsql_result_handle *) zend_list_find(result->value.lval,&type);
	
	if (type!=le_result) {
		php_error(E_WARNING,"%d is not a PostgresSQL result index",result->value.lval);
		RETURN_FALSE;
	}
	zend_list_delete(result->value.lval);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int pg_getlastoid(int result)
   Returns the last object identifier */
PHP_FUNCTION(pg_getlastoid)
{
	pval *result;
	PGresult *pgsql_result;
	pgsql_result_handle *pg_result;
	int type;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	pg_result = (pgsql_result_handle *) zend_list_find(result->value.lval,&type);
	
	if (type!=le_result) {
		php_error(E_WARNING,"%d is not a PostgresSQL result index",result->value.lval);
		RETURN_FALSE;
	}
	pgsql_result = pg_result->result;
	return_value->value.str.val = (char *) PQoidStatus(pgsql_result);
	if (return_value->value.str.val) {
		return_value->value.str.len = strlen(return_value->value.str.val);
		return_value->value.str.val = estrndup(return_value->value.str.val, return_value->value.str.len);
		return_value->type = IS_STRING;
	} else {
		return_value->value.str.val = empty_string;
	} 
}
/* }}} */

/* {{{ proto int pg_locreate(int connection)
   Create a large object */
PHP_FUNCTION(pg_locreate)
{
  	pval *pgsql_link;
	PGconn *pgsql;
	Oid pgsql_oid;
	int id, type;
	PGLS_FETCH();

	switch(ARG_COUNT(ht)) {
		case 0:
			id = PGG(default_link);
			break;
		case 1:
			if (getParameters(ht, 1, &pgsql_link)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(pgsql_link);
			id = pgsql_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	pgsql = (PGconn *) zend_list_find(id,&type);
	if (type!=le_link && type!=le_plink) {
		php_error(E_WARNING,"%d is not a PostgresSQL link index",id);
		RETURN_FALSE;
	}
	
	/* XXX: Archive modes not supported until I get some more data. Don't think anybody's
	   using it anyway. I believe it's also somehow related to the 'time travel' feature of
	   PostgreSQL, that's on the list of features to be removed... Create modes not supported.
	   What's the use of an object that can be only written to, but not read from, and vice
	   versa? Beats me... And the access type (r/w) must be specified again when opening
	   the object, probably (?) overrides this. (Jouni) 
	 */

	if ((pgsql_oid=lo_creat(pgsql, INV_READ|INV_WRITE))==0) {
		php_error(E_WARNING,"Unable to create PostgresSQL large object");
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
	pval *pgsql_link, *oid;
	PGconn *pgsql;
	Oid pgsql_oid;
	int id, type;
	PGLS_FETCH();

	switch(ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &oid)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(oid);
			pgsql_oid = oid->value.lval;
			id = PGG(default_link);
			break;
		case 2:
			if (getParameters(ht, 2, &pgsql_link, &oid)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(pgsql_link);
			id = pgsql_link->value.lval;
			convert_to_long(oid);
			pgsql_oid = oid->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	pgsql = (PGconn *) zend_list_find(id,&type);
	if (type!=le_link && type!=le_plink) {
		php_error(E_WARNING,"%d is not a PostgresSQL link index",id);
		RETURN_FALSE;
	}
	
	if (lo_unlink(pgsql, pgsql_oid)==-1) {
		php_error(E_WARNING,"Unable to delete PostgresSQL large object %d", (int) pgsql_oid);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int pg_loopen([int connection,] int objoid, string mode)
   Open a large object and return fd */
PHP_FUNCTION(pg_loopen)
{
	pval *pgsql_link, *oid, *mode;
	PGconn *pgsql;
	Oid pgsql_oid;
	int id, type, pgsql_mode=0, pgsql_lofd;
	int create=0;
	char *mode_string=NULL;
	pgLofp *pgsql_lofp;
	PGLS_FETCH();

	switch(ARG_COUNT(ht)) {
		case 2:
			if (getParameters(ht, 2, &oid, &mode)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(oid);
			pgsql_oid = oid->value.lval;
			convert_to_string(mode);
			mode_string = mode->value.str.val;
			id = PGG(default_link);
			break;
		case 3:
			if (getParameters(ht, 3, &pgsql_link, &oid, &mode)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(pgsql_link);
			id = pgsql_link->value.lval;
			convert_to_long(oid);
			pgsql_oid = oid->value.lval;
			convert_to_string(mode);
			mode_string = mode->value.str.val;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	pgsql = (PGconn *) zend_list_find(id,&type);
	if (type!=le_link && type!=le_plink) {
		php_error(E_WARNING,"%d is not a PostgresSQL link index",id);
		RETURN_FALSE;
	}
	
	/* r/w/+ is little bit more PHP-like than INV_READ/INV_WRITE and a lot of
	   faster to type. Unfortunately, doesn't behave the same way as fopen()...
	   (Jouni)
	 */

	if (strchr(mode_string, 'r')==mode_string) {
		pgsql_mode |= INV_READ;
		if (strchr(mode_string, '+')==mode_string+1) {
			pgsql_mode |= INV_WRITE;
		}
	}
	if (strchr(mode_string, 'w')==mode_string) {
		pgsql_mode |= INV_WRITE;
		create = 1;
		if (strchr(mode_string, '+')==mode_string+1) {
			pgsql_mode |= INV_READ;
		}
	}


	pgsql_lofp = (pgLofp *) emalloc(sizeof(pgLofp));

	if ((pgsql_lofd=lo_open(pgsql, pgsql_oid, pgsql_mode))==-1) {
		if (create) {
			if ((pgsql_oid=lo_creat(pgsql, INV_READ|INV_WRITE))==0) {
				efree(pgsql_lofp);
				php_error(E_WARNING,"Unable to create PostgresSQL large object");
				RETURN_FALSE;
			} else {
				if ((pgsql_lofd=lo_open(pgsql, pgsql_oid, pgsql_mode))==-1) {
					if (lo_unlink(pgsql, pgsql_oid)==-1) {
						efree(pgsql_lofp);
						php_error(E_WARNING,"Something's really messed up!!! Your database is badly corrupted in a way NOT related to PHP.");
						RETURN_FALSE;
					}
					efree(pgsql_lofp);
					php_error(E_WARNING,"Unable to open PostgresSQL large object");
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
			php_error(E_WARNING,"Unable to open PostgresSQL large object");
			RETURN_FALSE;
		}
	} else {
		pgsql_lofp->conn = pgsql;
		pgsql_lofp->lofd = pgsql_lofd;
		return_value->value.lval = zend_list_insert(pgsql_lofp, le_lofp);
		return_value->type = IS_LONG;
	}
}
/* }}} */

/* {{{ proto void pg_loclose(int fd)
   Close a large object */
PHP_FUNCTION(pg_loclose)
{
	pval *pgsql_lofp;
	int id, type;
	pgLofp *pgsql;

	switch(ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &pgsql_lofp)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(pgsql_lofp);
			id = pgsql_lofp->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	pgsql = (pgLofp *) zend_list_find(id,&type);
	if (type!=le_lofp) {
		php_error(E_WARNING,"%d is not a PostgresSQL large object index",id);
		RETURN_FALSE;
	}
	
	if (lo_close((PGconn *)pgsql->conn, pgsql->lofd)<0) {
		php_error(E_WARNING,"Unable to close PostgresSQL large object descriptor %d", pgsql->lofd);
		RETVAL_FALSE;
	} else {
		RETVAL_TRUE;
	}
	zend_list_delete(id);
	return;
}
/* }}} */

/* {{{ proto string pg_loread(int fd, int len)
   Read a large object */
PHP_FUNCTION(pg_loread)
{
  	pval *pgsql_id, *len;
	int id, buf_len, type, nbytes;
	char *buf;
	pgLofp *pgsql;

	switch(ARG_COUNT(ht)) {
		case 2:
			if (getParameters(ht, 2, &pgsql_id, &len)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(pgsql_id);
			id = pgsql_id->value.lval;
			convert_to_long(len);
			buf_len = len->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	pgsql = (pgLofp *) zend_list_find(id,&type);
	if (type!=le_lofp) {
		php_error(E_WARNING,"%d is not a PostgresSQL large object index",id);
		RETURN_FALSE;
	}
	
	buf = (char *) emalloc(sizeof(char)*(buf_len+1));
	if ((nbytes = lo_read((PGconn *)pgsql->conn, pgsql->lofd, buf, buf_len))<0) {
		efree(buf);
		RETURN_FALSE;
	}
	return_value->value.str.val = buf;
	return_value->value.str.len = nbytes;
	return_value->value.str.val[nbytes] = 0;
	return_value->type = IS_STRING;
}
/* }}} */

/* {{{ proto int pg_lowrite(int fd, string buf)
   Write a large object */
PHP_FUNCTION(pg_lowrite)
{
  	pval *pgsql_id, *str;
	int id, buf_len, nbytes, type;
	char *buf;
	pgLofp *pgsql;

	switch(ARG_COUNT(ht)) {
		case 2:
			if (getParameters(ht, 2, &pgsql_id, &str)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(pgsql_id);
			id = pgsql_id->value.lval;
			convert_to_string(str);
			buf = str->value.str.val;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	pgsql = (pgLofp *) zend_list_find(id,&type);
	if (type!=le_lofp) {
		php_error(E_WARNING,"%d is not a PostgresSQL large object index",id);
		RETURN_FALSE;
	}
	
	buf_len = str->value.str.len;
	if ((nbytes = lo_write((PGconn *)pgsql->conn, pgsql->lofd, buf, buf_len))==-1) {
		RETURN_FALSE;
	}
	return_value->value.lval = nbytes;
	return_value->type = IS_LONG;
}
/* }}} */

/* {{{ proto void pg_loreadall(int fd)
   Read a large object and send straight to browser */
PHP_FUNCTION(pg_loreadall)
{
  	pval *pgsql_id;
	int i, id, tbytes, type;
	volatile int nbytes;
	char buf[8192];
	pgLofp *pgsql;
	int output=1;

	switch(ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &pgsql_id)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(pgsql_id);
			id = pgsql_id->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	pgsql = (pgLofp *) zend_list_find(id,&type);
	if (type!=le_lofp) {
		php_error(E_WARNING,"%d is not a PostgresSQL large object index",id);
		RETURN_FALSE;
	}

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
	
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
