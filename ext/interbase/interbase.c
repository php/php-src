/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
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
   | Authors: Jouni Ahto <jah@cultnet.fi>                                 |
   |                                                                      |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* TODO: A lot... */

#include "php_config.h"
#include "php.h"
#include "php3_interbase.h"

#if HAVE_IBASE
#include <ibase.h>
#include <time.h>
#include "php3_string.h"
#include "php_globals.h"

/* {{{ extension definition structures */
function_entry ibase_functions[] = {
	{"ibase_connect",		php3_ibase_connect,		NULL},
	{"ibase_pconnect",		php3_ibase_pconnect,	NULL},
	{"ibase_close",			php3_ibase_close,		NULL},
	{"ibase_query",			php3_ibase_query,		NULL},
	{"ibase_fetch_row",		php3_ibase_fetch_row,	NULL},
	{"ibase_free_result",	php3_ibase_free_result,	NULL},
	{"ibase_prepare",		php3_ibase_prepare,		NULL},
	{"ibase_bind",			php3_ibase_bind,		NULL},
	{"ibase_execute",		php3_ibase_execute,		NULL},
	{"ibase_free_query",	php3_ibase_free_query,	NULL},
	{"ibase_timefmt",		php3_ibase_timefmt,		NULL},
	{NULL, NULL, NULL}
};

php3_module_entry ibase_module_entry =
{
	"InterBase",
	ibase_functions,
	php3_minit_ibase,
	NULL,
	php3_rinit_ibase,
	NULL,
	php3_info_ibase,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

/* {{{ thread safety stuff */
#if defined(THREAD_SAFE)
typedef ibase_global_struct{
        ibase_module php3_ibase_module;
} ibase_global_struct;

#define IBASE_GLOBAL(a) ibase_globals->a

#define IBASE_TLS_VARS \
        ibase_global_struct *ibase_globals; \
        ibase_globals=TlsGetValue(IBASETls); 

#else
#define IBASE_GLOBAL(a) a
#define IBASE_TLS_VARS
ibase_module php3_ibase_module;
#endif
/* }}} */

/* {{{ _php3_ibase_close_link() */
static void _php3_ibase_close_link(isc_db_handle db)
{
	ISC_STATUS status[20];
	IBASE_TLS_VARS;

	isc_detach_database(status, &db);
	IBASE_GLOBAL(php3_ibase_module).num_links--;
}
/* }}} */

/* {{{ _php3_ibase_close_plink() */
static void _php3_ibase_close_plink(isc_db_handle db)
{
	ISC_STATUS status[20];
	IBASE_TLS_VARS;

	isc_detach_database(status, &db);
	php3_ibase_module.num_persistent--;
	php3_ibase_module.num_links--;
}
/* }}} */

/* {{{ _php3_ibase_free_result() */
static void _php3_ibase_free_result(ibase_result_handle *result)
{
	int i;
	ISC_STATUS status[20];
	IBASE_TLS_VARS;

	if (!IBASE_GLOBAL(php3_ibase_module).manualtransactions && result->commitok) {
		isc_commit_transaction(status, &result->trans);
	}
	if (result->sqlda != NULL) {
		for (i = 0; i < result->sqlda->sqld; i++) {
			efree(result->sqlda->sqlvar[i].sqldata);
			efree(result->sqlda->sqlvar[i].sqlind);
		}
		efree(result->sqlda);
	}
	isc_dsql_free_statement(status, &result->result, DSQL_drop);
	efree(result);
}
/* }}} */

/* {{{ _php3_ibase_free_query() */
static void _php3_ibase_free_query(ibase_query_handle *query)
{
	int i;
	ISC_STATUS status[20];

	if (query->sqlda != NULL) {
		if (query->alloced) {
			for (i = 0; i < query->sqlda->sqld; i++) {
				efree(query->sqlda->sqlvar[i].sqldata);
				efree(query->sqlda->sqlvar[i].sqlind);
			}
		}
		efree(query->sqlda);
	}
	isc_dsql_free_statement(status, &query->query, DSQL_drop);
	efree(query);
}
/* }}} */

/* {{{ startup, shutdown and info functions */
int php3_minit_ibase(INIT_FUNC_ARGS)
{
	IBASE_TLS_VARS;

	if (cfg_get_long("ibase.allow_persistent", &IBASE_GLOBAL(php3_ibase_module).allow_persistent) == FAILURE) {
		IBASE_GLOBAL(php3_ibase_module).allow_persistent = 1;
	}
	if (cfg_get_long("ibase.max_persistent", &IBASE_GLOBAL(php3_ibase_module).max_persistent) == FAILURE) {
		IBASE_GLOBAL(php3_ibase_module).max_persistent = -1;
	}
	if (cfg_get_long("ibase.max_links", &IBASE_GLOBAL(php3_ibase_module).max_links) == FAILURE) {
		IBASE_GLOBAL(php3_ibase_module).max_links = -1;
	}
	/*
	if (cfg_get_long("ibase.manualtransactions", &IBASE_GLOBAL(php3_ibase_module).manualtransactions) == FAILURE) {
		IBASE_GLOBAL(php3_ibase_module).manualtransactions = 0;
	}
	*/
	IBASE_GLOBAL(php3_ibase_module).manualtransactions = 0;
	if (cfg_get_string("ibase.default_user", &IBASE_GLOBAL(php3_ibase_module).default_user) == FAILURE
		|| IBASE_GLOBAL(php3_ibase_module).default_user[0] == 0) {
		IBASE_GLOBAL(php3_ibase_module).default_user = "";
	}
	if (cfg_get_string("ibase.default_password", &IBASE_GLOBAL(php3_ibase_module).default_password) == FAILURE
		|| IBASE_GLOBAL(php3_ibase_module).default_password[0] == 0) {	
		IBASE_GLOBAL(php3_ibase_module).default_password = "";
	}
	if (cfg_get_string("ibase.timeformat", &IBASE_GLOBAL(php3_ibase_module).timeformat) == FAILURE) {
		IBASE_GLOBAL(php3_ibase_module).timeformat = "%Y-%m-%d %H:%M:%S";
	}
	IBASE_GLOBAL(php3_ibase_module).num_persistent=0;
	IBASE_GLOBAL(php3_ibase_module).le_result = register_list_destructors(_php3_ibase_free_result, NULL);
	IBASE_GLOBAL(php3_ibase_module).le_query = register_list_destructors(_php3_ibase_free_query, NULL);
	IBASE_GLOBAL(php3_ibase_module).le_link = register_list_destructors(_php3_ibase_close_link, NULL);
	IBASE_GLOBAL(php3_ibase_module).le_plink = register_list_destructors(NULL, _php3_ibase_close_plink);
	return SUCCESS;
}

int php3_rinit_ibase(INIT_FUNC_ARGS)
{
	IBASE_TLS_VARS;

	IBASE_GLOBAL(php3_ibase_module).default_link=-1;
	IBASE_GLOBAL(php3_ibase_module).num_links = php3_ibase_module.num_persistent;
	return SUCCESS;
}

/* TODO IF NEEDED

int php3_mfinish_ibase(void)
{
}

int php3_rfinish_ibase(void)
{
}
*/

void php3_info_ibase(ZEND_MODULE_INFO_FUNC_ARGS)
{
	/* TODO */
}
/* }}} */

/* {{{ _php_ibase_attach_db() */
static int _php_ibase_attach_db(char *server, char *uname, int uname_len, char *passwd, int passwd_len, isc_db_handle *db)
{
	ISC_STATUS status[20];
	int db_parbuf_len = 4;
	short foo = 4;
	char dpb[255], *db_parbuf;

	if (uname_len) {
		db_parbuf_len += uname_len + 2;
	}

	if (passwd_len) {
		db_parbuf_len += passwd_len + 2;
	}

	db_parbuf = dpb;
	*db_parbuf++ = isc_dpb_version1;
	*db_parbuf++ = isc_dpb_num_buffers;
	*db_parbuf++ = 1;
	*db_parbuf++ = 90;

	if (uname_len) {
		if (passwd_len) {
			isc_expand_dpb(&db_parbuf, &foo, isc_dpb_user_name, uname, isc_dpb_password, passwd, NULL);
		} else {
			isc_expand_dpb(&db_parbuf, &foo, isc_dpb_user_name, uname, NULL);
		}
	}

	isc_attach_database(status, strlen(server), server, db, db_parbuf_len, dpb);

	if (status[0] == 1 && status[1]) {
		php3_error(E_WARNING,"Unable to connect to InterBase server:  %s", "XXX");
		return 1;
	}
	return 0;
}
/* }}} */

/* {{{ _php3_ibase_connect() */
static void _php3_ibase_connect(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	pval *server, *uname, *passwd;

	char *ib_server, *ib_uname, *ib_passwd;
	int ib_server_len, ib_uname_len, ib_passwd_len;
	isc_db_handle db_handle = NULL;
	char *hashed_details;
	int hashed_details_length;
	IBASE_TLS_VARS;

	ib_uname = IBASE_GLOBAL(php3_ibase_module).default_user;
	ib_passwd = IBASE_GLOBAL(php3_ibase_module).default_password;
	ib_uname_len = ib_uname ? strlen(ib_uname) : 0;
	ib_passwd_len = ib_passwd ? strlen(ib_passwd) : 0;

	switch(ARG_COUNT(ht)) {
	case 1:
		{
			if (getParameters(ht, 1, &server) == FAILURE) {
				RETURN_FALSE;
			}
			convert_to_string(server);
			ib_server = server->value.str.val;
			ib_server_len = server->value.str.len;
			hashed_details_length = server->value.str.len+ib_uname_len+ib_passwd_len+5+3;
			hashed_details = (char *) emalloc(hashed_details_length+1);
			sprintf(hashed_details, "ibase_%s_%s_%s", ib_server, ib_uname, ib_passwd);
		}
		break;
	case 2:
		{
			if (getParameters(ht, 2, &server, &uname) == FAILURE) {
				RETURN_FALSE;
			}
			convert_to_string(server);
			convert_to_string(uname);
			ib_server = server->value.str.val;
			ib_uname = uname->value.str.val;
			ib_server_len = server->value.str.len;
			ib_uname_len = uname->value.str.len;
			hashed_details_length = server->value.str.len+uname->value.str.len+ib_passwd_len+5+3;
			hashed_details = (char *) emalloc(hashed_details_length+1);
			sprintf(hashed_details, "ibase_%s_%s_%s", ib_server, ib_uname, ib_passwd);
		}
		break;
	case 3:
		{
			if (getParameters(ht, 3, &server, &uname, &passwd) == FAILURE) {
				RETURN_FALSE;
			}
			convert_to_string(server);
			convert_to_string(uname);
			convert_to_string(passwd);
			ib_server = server->value.str.val;
			ib_uname = uname->value.str.val;
			ib_passwd = passwd->value.str.val;
			ib_server_len = server->value.str.len;
			ib_uname_len = uname->value.str.len;
			ib_passwd_len = passwd->value.str.len;
			hashed_details_length = server->value.str.len+uname->value.str.len+passwd->value.str.len+5+3;
			hashed_details = (char *) emalloc(hashed_details_length+1);
			sprintf(hashed_details, "ibase_%s_%s_%s", ib_server, ib_uname, ib_passwd);
		}
		break;
	default:
		WRONG_PARAM_COUNT;
		break;
	}

	if (persistent) {
		list_entry *le;
		
		if (_php3_hash_find(plist, hashed_details, hashed_details_length+1, (void **) &le)==FAILURE) {
			list_entry new_le;
			
			if (IBASE_GLOBAL(php3_ibase_module).max_links!=-1 && IBASE_GLOBAL(php3_ibase_module).num_links>=IBASE_GLOBAL(php3_ibase_module).max_links) {
				php3_error(E_WARNING,"InterBase: Too many open links (%d)", IBASE_GLOBAL(php3_ibase_module).num_links);
				efree(hashed_details);
				RETURN_FALSE;
			}
			if (IBASE_GLOBAL(php3_ibase_module).max_persistent!=-1 && IBASE_GLOBAL(php3_ibase_module).num_persistent>=IBASE_GLOBAL(php3_ibase_module).max_persistent) {
				php3_error(E_WARNING,"InterBase: Too many open persistent links (%d)", IBASE_GLOBAL(php3_ibase_module).num_persistent);
				efree(hashed_details);
				RETURN_FALSE;
			}

			/* create the link */

			if (_php_ibase_attach_db(ib_server, ib_uname, ib_uname_len, ib_passwd, ib_passwd_len, &db_handle)) {
				efree(hashed_details);
				RETURN_FALSE;
			}

			/* hash it up */
			new_le.type = php3_ibase_module.le_plink;
			new_le.ptr = db_handle;
			if (_php3_hash_update(plist, hashed_details, hashed_details_length+1, (void *) &new_le, sizeof(list_entry), NULL)==FAILURE) {
				efree(hashed_details);
				RETURN_FALSE;
			}
			IBASE_GLOBAL(php3_ibase_module).num_links++;
			IBASE_GLOBAL(php3_ibase_module).num_persistent++;
		} else {
			if (le->type != IBASE_GLOBAL(php3_ibase_module).le_plink) {
				RETURN_FALSE;
			}
			/* TODO: ensure that the link did not die */

			db_handle = (isc_db_handle) le->ptr;
		}
		return_value->value.lval = php3_list_insert(db_handle, IBASE_GLOBAL(php3_ibase_module).le_plink);
		return_value->type = IS_LONG;
	} else {
		list_entry *index_ptr, new_index_ptr;
		
		/* first we check the hash for the hashed_details key.  if it exists,
		 * it should point us to the right offset where the actual pgsql link sits.
		 * if it doesn't, open a new pgsql link, add it to the resource list,
		 * and add a pointer to it with hashed_details as the key.
		 */
		if (_php3_hash_find(list,hashed_details,hashed_details_length+1,(void **) &index_ptr)==SUCCESS) {
			int type,link;
			void *ptr;

			if (index_ptr->type != le_index_ptr) {
				RETURN_FALSE;
			}
			link = (int) index_ptr->ptr;
			ptr = php3_list_find(link,&type);   /* check if the link is still there */
			if (ptr && (type==IBASE_GLOBAL(php3_ibase_module).le_link || type==IBASE_GLOBAL(php3_ibase_module).le_plink)) {
				return_value->value.lval = IBASE_GLOBAL(php3_ibase_module).default_link = link;
				return_value->type = IS_LONG;
				efree(hashed_details);
				return;
			} else {
				_php3_hash_del(list,hashed_details,hashed_details_length+1);
			}
		}
		if (IBASE_GLOBAL(php3_ibase_module).max_links!=-1 && IBASE_GLOBAL(php3_ibase_module).num_links>=IBASE_GLOBAL(php3_ibase_module).max_links) {
			php3_error(E_WARNING,"InterBase:  Too many open links (%d)", IBASE_GLOBAL(php3_ibase_module).num_links);
			efree(hashed_details);
			RETURN_FALSE;
		}
		/* create the link */

		if (_php_ibase_attach_db(ib_server, ib_uname, ib_uname_len, ib_passwd, ib_passwd_len, &db_handle)) {
			efree(hashed_details);
			RETURN_FALSE;
		}

		/* add it to the list */
		return_value->value.lval = php3_list_insert(db_handle, IBASE_GLOBAL(php3_ibase_module).le_link);
		return_value->type = IS_LONG;

		/* add it to the hash */
		new_index_ptr.ptr = (void *) return_value->value.lval;
		new_index_ptr.type = le_index_ptr;
		if (_php3_hash_update(list,hashed_details,hashed_details_length+1,(void *) &new_index_ptr, sizeof(list_entry), NULL)==FAILURE) {
			efree(hashed_details);
			RETURN_FALSE;
		}
		IBASE_GLOBAL(php3_ibase_module).num_links++;
	}
	efree(hashed_details);
	IBASE_GLOBAL(php3_ibase_module).default_link=return_value->value.lval;
}
/* }}} */

/* {{{ proto int ibase_connect(string database [, string username] [, string password])
   Open a connection to an InterBase database */
PHP_FUNCTION(ibase_connect)
{
	_php3_ibase_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int ibase_pconnect(string database [, string username] [, string password])
   Open a persistent connection to an InterBase database */
PHP_FUNCTION(ibase_pconnect)
{
	_php3_ibase_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto int ibase_close([int link_identifier])
   Close an InterBase connection */
PHP_FUNCTION(ibase_close)
{
	pval *ibase_link;
	int id, type;
	isc_db_handle db_handle;
	IBASE_TLS_VARS;
	
	switch (ARG_COUNT(ht)) {
		case 0:
			id = IBASE_GLOBAL(php3_ibase_module).default_link;
			break;
		case 1:
			if (getParameters(ht, 1, &ibase_link) == FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(ibase_link);
			id = ibase_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	db_handle = (isc_db_handle) php3_list_find(id, &type);
	if (type!=IBASE_GLOBAL(php3_ibase_module).le_link && type!=IBASE_GLOBAL(php3_ibase_module).le_plink) {
		php3_error(E_WARNING, "%d is not an InterBase link index",id);
		RETURN_FALSE;
	}
	
	php3_list_delete(ibase_link->value.lval);
	RETURN_TRUE;
}
/* }}} */

/* {{{ _php3_ibase_prepare() */
static XSQLDA *_php3_ibase_prepare(isc_db_handle db, isc_tr_handle tr, isc_stmt_handle *query_handle, char *query)
{
	ISC_STATUS status[20];
	XSQLDA *isqlda;

	isqlda = (XSQLDA *) emalloc(XSQLDA_LENGTH(0));
    isqlda->sqln = 0;
    isqlda->version = SQLDA_VERSION1;

	if (isc_dsql_allocate_statement(status, &db, query_handle)) {
		php3_error(E_WARNING, "InterBase: couldn't allocate space for query");
		return NULL;
	}

	if (isc_dsql_prepare(status, &tr, query_handle, 0, query, 1, isqlda)) {
		php3_error(E_WARNING, "InterBase: couldn't prepare query");
		return NULL;
	}
	
	/*
	 * Check if query has placeholders and needs binding. If it has, allocate
	 * input sqlda big enough and return it.
	 */


	if (isc_dsql_describe_bind(status, query_handle, 1, isqlda)) {
		php3_error(E_WARNING, "InterBase: couldn't describe placeholders in query");
	}
	
	if (isqlda->sqld > 1) {
		isqlda = (XSQLDA *) erealloc(isqlda, XSQLDA_LENGTH(isqlda->sqld));
		isqlda->sqln = isqlda->sqld;
		isqlda->version = SQLDA_VERSION1;
		if (isc_dsql_describe(status, query_handle, 1, isqlda)) {
			php3_error(E_WARNING, "InterBase: couldn't describe query");
		}
		return isqlda;
	} else if (isqlda->sqld == 1) {
		return isqlda;
	} else {
		efree(isqlda);
		return NULL;
	}
}
/* }}} */

/* {{{ _php3_ibase_execute() */
static XSQLDA *_php3_ibase_execute(isc_tr_handle tr_handle, isc_stmt_handle query_handle, XSQLDA *isqlda, ISC_STATUS *status)
{
	int i, coltype;
	static char query_info[] = { isc_info_sql_stmt_type };
	char info_buffer[18];
    short l;
    long query_type;
	XSQLDA *osqlda;

	/*
	 * Find out what kind of query is to be executed.
	 */
	
	if (!isc_dsql_sql_info(status, &query_handle, sizeof(query_info), query_info, sizeof(info_buffer), info_buffer)) {
		l = (short) isc_vax_integer((char ISC_FAR *) info_buffer + 1, 2);
		query_type = isc_vax_integer((char ISC_FAR *) info_buffer + 3, l);
	}

	if (query_type == isc_info_sql_stmt_select || query_type == isc_info_sql_stmt_select_for_upd) {
		/*
		 * Select, need to allocate output sqlda and and prepare it for use.
		 */

		osqlda = (XSQLDA *) emalloc(XSQLDA_LENGTH(0));
		osqlda->sqln = 0;
		osqlda->version = SQLDA_VERSION1;

		if (isc_dsql_describe(status, &query_handle, 1, osqlda)) {
			php3_error(E_WARNING, "InterBase: couldn't describe query");
		}

		if (osqlda->sqld) {
			osqlda = (XSQLDA *) erealloc(osqlda, XSQLDA_LENGTH(osqlda->sqld));
			osqlda->sqln = osqlda->sqld;
			osqlda->version = SQLDA_VERSION1;
			if (isc_dsql_describe(status, &query_handle, 1, osqlda)) {
				php3_error(E_WARNING, "InterBase: couldn't describe query");
			}
		}
		for (i = 0; i < osqlda->sqld; i++) {
			osqlda->sqlvar[i].sqlind = (short *) emalloc(sizeof(short));
			coltype = osqlda->sqlvar[i].sqltype & ~1;
			switch(coltype)
				{
				case SQL_TEXT:
					osqlda->sqlvar[i].sqldata = (char *) emalloc(sizeof(char)*(osqlda->sqlvar[i].sqllen));
					break;
				case SQL_VARYING:
					osqlda->sqlvar[i].sqldata = (char *) emalloc(sizeof(char)*(osqlda->sqlvar[i].sqllen+2));
					break;
				case SQL_SHORT:
					osqlda->sqlvar[i].sqldata = (char *) emalloc(sizeof(short));
					break;
				case SQL_LONG:
					osqlda->sqlvar[i].sqldata = (char *) emalloc(sizeof(long));
					break;
				case SQL_FLOAT:
					osqlda->sqlvar[i].sqldata = (char *) emalloc(sizeof(float));
					break;
				case SQL_DOUBLE:
					osqlda->sqlvar[i].sqldata = (char *) emalloc(sizeof(double));
					break;
				case SQL_DATE:
					osqlda->sqlvar[i].sqldata = (char *) emalloc(sizeof(ISC_QUAD));
					break;
				case SQL_BLOB:
					osqlda->sqlvar[i].sqldata = (char *) emalloc(sizeof(ISC_QUAD));
					break;
				case SQL_ARRAY:
					osqlda->sqlvar[i].sqldata = (char *) emalloc(sizeof(ISC_QUAD));
					break;
				}
		}
		if (isqlda == NULL) {
			if (isc_dsql_execute(status, &tr_handle, &query_handle, 1, NULL)) {
				php3_error(E_WARNING, "InterBase: couldn't execute query");
				return NULL;
			} else {
				return osqlda;
			}
	
		} else {
			if (isc_dsql_execute2(status, &tr_handle, &query_handle, 1, isqlda, osqlda)) {
				php3_error(E_WARNING, "InterBase: couldn't execute query");
				return NULL;
			} else {
				return osqlda;
			}
	
		}
	} else {
		/* Not select */
		if (isc_dsql_execute(status, &tr_handle, &query_handle, 1, isqlda)) {
			php3_error(E_WARNING, "InterBase: couldn't execute query");
		}
		/*
		if (!php3_ibase_module.manualtransactions) {
			if(0) {
				isc_commit_transaction(status, tr_handle);
			} else {
				isc_rollback_transaction(status, tr_handle);
			}
		}
		*/
	}

	return NULL;
}
/* }}} */

/* {{{ proto int ibase_query([int link_identifier, ]string query)
   Execute a query (without parameter placeholders). */
PHP_FUNCTION(ibase_query)
{
	pval *query, *ibase_link;
	int id, type;
	isc_db_handle db_handle;
	isc_tr_handle tr_handle = NULL;
	isc_stmt_handle query_handle = NULL;
	ISC_STATUS status[20];
	XSQLDA *isqlda, *osqlda;
	ibase_result_handle *ibase_result;
	IBASE_TLS_VARS;

	switch (ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &query) == FAILURE) {
				RETURN_FALSE;
			}
			id = IBASE_GLOBAL(php3_ibase_module).default_link;
			break;
		case 2:
			if (getParameters(ht, 2, &ibase_link, &query) == FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(ibase_link);
			id = ibase_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	db_handle = (isc_db_handle) php3_list_find(id, &type);
	if (type!=IBASE_GLOBAL(php3_ibase_module).le_link && type!=IBASE_GLOBAL(php3_ibase_module).le_plink) {
		php3_error(E_WARNING, "%d is not an InterBase link index", id);
		RETURN_FALSE;
	}
	
	convert_to_string(query);

	if (!IBASE_GLOBAL(php3_ibase_module).manualtransactions) {
		if (isc_start_transaction(status, &tr_handle, 1, &db_handle, 0, NULL)) {
			php3_error(E_WARNING, "InterBase: couldn't start transaction");
			RETURN_FALSE;
		}
	}

	isqlda = _php3_ibase_prepare(db_handle, tr_handle, &query_handle, query->value.str.val);
	if (isqlda != NULL) {
		isc_rollback_transaction(status, &tr_handle);
		isc_dsql_free_statement(status, &query_handle, DSQL_drop);
		php3_error(E_WARNING, "InterBase: ibase_query doesn't support parameter placeholders in query");
		RETURN_FALSE;
	}

	osqlda = _php3_ibase_execute(tr_handle, query_handle, isqlda, status);
	if (osqlda != NULL) {
		ibase_result = (ibase_result_handle *) emalloc(sizeof(ibase_result_handle));
		ibase_result->result = query_handle;
		ibase_result->sqlda = osqlda;
		ibase_result->trans = tr_handle;
		ibase_result->commitok = 1;
		return_value->value.lval = php3_list_insert(ibase_result, php3_ibase_module.le_result);
		return_value->type = IS_LONG;
	} else {
		if (status[0] == 1 && status[1]) {
			if (!IBASE_GLOBAL(php3_ibase_module).manualtransactions) {
				isc_rollback_transaction(status, &tr_handle);
				isc_dsql_free_statement(status, &query_handle, DSQL_drop);
				RETURN_FALSE;
			}
		} else {
			if (!IBASE_GLOBAL(php3_ibase_module).manualtransactions) {
				isc_commit_transaction(status, &tr_handle);
				isc_dsql_free_statement(status, &query_handle, DSQL_drop);
				RETURN_FALSE;
			}
		}
	}
}
/* }}} */

/* {{{ proto int ibase_fetch_row(int result)
   Fetch a row from the results of a query. */
PHP_FUNCTION(ibase_fetch_row)
{
	pval *result;
	pval *pval_ptr;
	int type;
	int i, coltype, collen;
	char string_data[255];
	char *fieldname;
	char *char_data;
	/*
	struct tm t;
	*/
	ibase_result_handle *ibase_result;
	ISC_STATUS status[20];
	XSQLVAR *var;
	IBASE_VCHAR *vchar;
	IBASE_TLS_VARS;
	PLS_FETCH();

	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	ibase_result = (ibase_result_handle *) php3_list_find(result->value.lval, &type);
	
	if (type!=IBASE_GLOBAL(php3_ibase_module).le_result) {
		php3_error(E_WARNING,"%d is not an InterBase result index", result->value.lval);
		RETURN_FALSE;
	}

	if (ibase_result->sqlda == NULL) {
		php3_error(E_WARNING,"InterBase: trying to fetch results from a non-select query");
		RETURN_FALSE;
	}

	var = ibase_result->sqlda->sqlvar;

	if (isc_dsql_fetch(status, &ibase_result->result, 1, ibase_result->sqlda) != 100L) {
		if (array_init(return_value)==FAILURE) {
			RETURN_FALSE;
		}

		for (i = 0; i < ibase_result->sqlda->sqld; i++) {
			var[i].sqlname[var[i].sqlname_length] = '\0';
			fieldname = var[i].sqlname;
			if (*var[i].sqlind < 0) {
				/* XXX. Column is NULL. This is not the best idea to do, think something... */
				add_get_index_stringl(return_value, i, NULL, 0, (void **) &pval_ptr, 1);
				_php3_hash_pointer_update(return_value->value.ht, fieldname, var[i].sqlname_length+1, pval_ptr);
				continue;
			}
			coltype = var[i].sqltype & ~1;
			switch(coltype) {
				case SQL_TEXT:
					{
						int len = var[i].sqllen;
						char_data = (char *)emalloc(sizeof(char)*(var[i].sqllen));
						collen = sprintf(char_data, "%*.*s", len, len, ibase_result->sqlda->sqlvar[i].sqldata);
						if (PG(magic_quotes_runtime)) {
							int newlen;
							char *tmp = _php3_addslashes(char_data, collen, &newlen, 0);
							add_get_index_stringl(return_value, i, tmp, newlen, (void **) &pval_ptr, 0);
						} else {
							add_get_index_stringl(return_value, i, char_data, collen, (void **) &pval_ptr, 1);
						}
						_php3_hash_pointer_update(return_value->value.ht, fieldname, var[i].sqlname_length+1, pval_ptr);
						efree(char_data);
					}
					break;
				case SQL_VARYING:
					{
						vchar = (IBASE_VCHAR *) var[i].sqldata;
						char_data = (char *)emalloc(sizeof(char)*(vchar->var_len));
						collen = sprintf(char_data, "%*.*s", vchar->var_len, vchar->var_len, vchar->var_str);
						if (PG(magic_quotes_runtime)) {
							int newlen;
							char *tmp = _php3_addslashes(char_data, collen, &newlen, 0);
							add_get_index_stringl(return_value, i, tmp, newlen, (void **) &pval_ptr, 0);
						} else {
							add_get_index_stringl(return_value, i, char_data, collen, (void **) &pval_ptr, 1);
						}
						_php3_hash_pointer_update(return_value->value.ht, fieldname, var[i].sqlname_length+1, pval_ptr);
						efree(char_data);
					}
					break;
				case SQL_SHORT:
					collen = sprintf(string_data, "%d", *(short *)(var[i].sqldata));
					add_get_index_stringl(return_value, i, string_data, collen, (void **) &pval_ptr, 1);
					_php3_hash_pointer_update(return_value->value.ht, fieldname, var[i].sqlname_length+1, pval_ptr);
					break;
				case SQL_LONG:
					if (var[i].sqlscale) {
						int j, f = 1;
						float n;
						n = *(long *)(var[i].sqldata);
						for (j = 0; j < -var[i].sqlscale; j++)
							f *= 10;
						n /= f;
						collen = sprintf(string_data, "%.*f", -var[i].sqlscale, n);

					} else {
						collen = sprintf(string_data, "%ld", *(long *)(var[i].sqldata));
					}
					add_get_index_stringl(return_value, i, string_data, collen, (void **) &pval_ptr, 1);
					_php3_hash_pointer_update(return_value->value.ht, fieldname, var[i].sqlname_length+1, pval_ptr);
					break;
				case SQL_FLOAT:
					collen = sprintf(string_data, "%f", *(float *)(var[i].sqldata));
					add_get_index_stringl(return_value, i, string_data, collen, (void **) &pval_ptr, 1);
					_php3_hash_pointer_update(return_value->value.ht, fieldname, var[i].sqlname_length+1, pval_ptr);
					break;
				case SQL_DOUBLE:
					if (ibase_result->sqlda->sqlvar[i].sqlscale) {
						collen = sprintf(string_data, "%.*f", -var[i].sqlscale, *(double *)(var[i].sqldata));
					} else {
						collen = sprintf(string_data, "%f", *(double *)(var[i].sqldata));
					}
					add_get_index_stringl(return_value, i, string_data, collen, (void **) &pval_ptr, 1);
					_php3_hash_pointer_update(return_value->value.ht, fieldname, var[i].sqlname_length+1, pval_ptr);
					break;
				case SQL_DATE: {
					struct tm *t;
					t = emalloc(sizeof(struct tm));

					isc_decode_date((ISC_QUAD *) var[i].sqldata, t);
					/*
#if HAVE_STRFTIME
					collen = (int) strftime(string_data, 255, IBASE_GLOBAL(php3_ibase_module).timeformat, t);
#else
					*/
					collen = sprintf(string_data, "%4d-%02d-%02d %02d:%02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
					/* #endif */
					add_get_index_stringl(return_value, i, string_data, collen, (void **) &pval_ptr, 1);
					_php3_hash_pointer_update(return_value->value.ht, fieldname, var[i].sqlname_length+1, pval_ptr);
					efree(t);
					}
					break;
				case SQL_BLOB:
				case SQL_ARRAY:
					{
						/* TODO, currently just show the id */
						ISC_QUAD bid;
						bid = *(ISC_QUAD ISC_FAR *) var[i].sqldata;
						sprintf(string_data, "%lx:%lx", bid.isc_quad_high, bid.isc_quad_low);
						add_get_index_stringl(return_value, i, string_data, collen, (void **) &pval_ptr, 1);
						_php3_hash_pointer_update(return_value->value.ht, fieldname, var[i].sqlname_length+1, pval_ptr);
					}
					break;
				default:
					break;
			}
		}
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int ibase_free_result(int result)
   Free the memory used by a result. */
PHP_FUNCTION(ibase_free_result)
{
	pval *result;
	ibase_result_handle *ibase_result;
	int type;
	IBASE_TLS_VARS;

	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	if (result->value.lval==0) {
		RETURN_FALSE;
	}
	
	ibase_result = (ibase_result_handle *) php3_list_find(result->value.lval,&type);
	
	if (type!=IBASE_GLOBAL(php3_ibase_module).le_result) {
		php3_error(E_WARNING,"%d is not an InterBase result index",result->value.lval);
		RETURN_FALSE;
	}
	php3_list_delete(result->value.lval);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ibase_prepare([int link_identifier, ]string query)
   Prepare a query for later binding of parameter placeholders and execution. */
PHP_FUNCTION(ibase_prepare)
{
	pval *query, *ibase_link;
	int id, type;
	isc_db_handle db_handle;
	isc_tr_handle tr_handle = NULL;
	isc_stmt_handle query_handle = NULL;
	ISC_STATUS status[20];
	XSQLDA *isqlda;
	ibase_query_handle *ibase_query;
	IBASE_TLS_VARS;

	switch (ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &query) == FAILURE) {
				RETURN_FALSE;
			}
			id = IBASE_GLOBAL(php3_ibase_module).default_link;
			break;
		case 2:
			if (getParameters(ht, 2, &ibase_link, &query) == FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(ibase_link);
			id = ibase_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	db_handle = (isc_db_handle) php3_list_find(id, &type);
	if (type!=IBASE_GLOBAL(php3_ibase_module).le_link && type!=IBASE_GLOBAL(php3_ibase_module).le_plink) {
		php3_error(E_WARNING, "%d is not an InterBase link index", id);
		RETURN_FALSE;
	}
	
	convert_to_string(query);

	if (!IBASE_GLOBAL(php3_ibase_module).manualtransactions) {
		if (isc_start_transaction(status, &tr_handle, 1, &db_handle, 0, NULL)) {
			php3_error(E_WARNING, "InterBase: couldn't start transaction");
			RETURN_FALSE;
		}
	}

	isqlda = _php3_ibase_prepare(db_handle, tr_handle, &query_handle, query->value.str.val);
	ibase_query = (ibase_query_handle *) emalloc(sizeof(ibase_query_handle));
	ibase_query->query = query_handle;
	ibase_query->sqlda = isqlda;
	ibase_query->trans = tr_handle;
	ibase_query->alloced = 0;
	return_value->value.lval = php3_list_insert(ibase_query, php3_ibase_module.le_query);
	return_value->type = IS_LONG;
}
/* }}} */

/* {{{ proto int ibase_bind (int query)
   Bind parameter placeholders in a previously prepared query. Still nonfunctional. */
PHP_FUNCTION(ibase_bind)
{
	pval *query;
	ibase_query_handle *ibase_query;
	int type;
	IBASE_TLS_VARS;

	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &query)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(query);
	if (query->value.lval==0) {
		RETURN_FALSE;
	}
	
	ibase_query = (ibase_query_handle *) php3_list_find(query->value.lval,&type);
	
	if (type!=IBASE_GLOBAL(php3_ibase_module).le_query) {
		php3_error(E_WARNING,"%d is not an InterBase query index",query->value.lval);
		RETURN_FALSE;
	}

	if (ibase_query->sqlda == NULL) {
		php3_error(E_WARNING,"InterBase: trying to bind a query having no parameter placeholders");
		RETURN_FALSE;
	}

	/* TODO 
	for (i = 0; i < ibase_query->sqlda->sqld; i++) {
		ibase_query->sqlda->sqlvar[i].sqlind = (short *) emalloc(sizeof(short));
		coltype = ibase_query->sqlda->sqlvar[i].sqltype & ~1;
		switch(coltype)
			{
			case SQL_TEXT:
				break;
			case SQL_VARYING:
				break;
			case SQL_SHORT:
				break;
			case SQL_LONG:
				break;
			case SQL_FLOAT:
				break;
			case SQL_DOUBLE:
				break;
			case SQL_DATE:
				break;
			case SQL_BLOB:
				break;
			case SQL_ARRAY:
				break;
			}
	}
	*/
}
/* }}} */

/* {{{ proto int ibase_execute(int query)
   Execute a previously prepared (and possibly binded) query. */
PHP_FUNCTION(ibase_execute)
{
	pval *query;
	ibase_query_handle *ibase_query;
	ibase_result_handle *ibase_result;
	int type;
	ISC_STATUS status[20];
	XSQLDA *osqlda;
	IBASE_TLS_VARS;

	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &query)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(query);
	if (query->value.lval==0) {
		RETURN_FALSE;
	}
	
	ibase_query = (ibase_query_handle *) php3_list_find(query->value.lval,&type);
	
	if (type!=IBASE_GLOBAL(php3_ibase_module).le_query) {
		php3_error(E_WARNING,"%d is not an InterBase query index", query->value.lval);
		RETURN_FALSE;
	}

	osqlda = _php3_ibase_execute(ibase_query->trans, ibase_query->query, ibase_query->sqlda, status);
	ibase_result = (ibase_result_handle *) emalloc(sizeof(ibase_result_handle));
	ibase_result->result = ibase_query->query;
	ibase_result->sqlda = osqlda;
	return_value->value.lval = php3_list_insert(ibase_result, IBASE_GLOBAL(php3_ibase_module).le_result);
	return_value->type = IS_LONG;
}
/* }}} */

/* {{{ proto int ibase_free_query(int query)
   Free memory used by a query */
PHP_FUNCTION(ibase_free_query)
{
	pval *query;
	ibase_query_handle *ibase_query;
	int type;
	IBASE_TLS_VARS;

	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &query)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(query);
	if (query->value.lval==0) {
		RETURN_FALSE;
	}
	
	ibase_query = (ibase_query_handle *) php3_list_find(query->value.lval, &type);
	
	if (type!=IBASE_GLOBAL(php3_ibase_module).le_query) {
		php3_error(E_WARNING,"%d is not an InterBase query index", query->value.lval);
		RETURN_FALSE;
	}
	php3_list_delete(query->value.lval);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ibase_timefmt(string format)
   Sets the format of datetime columns returned from queries. Still nonfunctional. */
PHP_FUNCTION(ibase_timefmt)
{
	pval *pmode;
	IBASE_TLS_VARS;

#if HAVE_STRFTIME
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &pmode)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(pmode);

	RETURN_TRUE;
#else
	php3_error(E_WARNING,"InterBase: ibase_timefmt not supported on this platform");
	RETURN_FALSE;
#endif
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
