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
   |          Tom May <tom@go2net.com>                                    |
   +----------------------------------------------------------------------+
 */
 
/* $Id$ */


#include "php.h"
#include "php3_sybase-ct.h"
#include "ext/standard/php3_standard.h"
#include "php_globals.h"

#if HAVE_SYBASE_CT



function_entry sybct_functions[] = {
	{"sybase_connect",		php3_sybct_connect,			NULL},
	{"sybase_pconnect",		php3_sybct_pconnect,		NULL},
	{"sybase_close",		php3_sybct_close,			NULL},
	{"sybase_select_db",	php3_sybct_select_db,		NULL},
	{"sybase_query",		php3_sybct_query,			NULL},
	{"sybase_free_result",	php3_sybct_free_result,		NULL},
	{"sybase_num_rows",		php3_sybct_num_rows,		NULL},
	{"sybase_num_fields",	php3_sybct_num_fields,		NULL},
	{"sybase_fetch_row",	php3_sybct_fetch_row,		NULL},
	{"sybase_fetch_array",	php3_sybct_fetch_array,		NULL},
	{"sybase_fetch_object",	php3_sybct_fetch_object,	NULL},
	{"sybase_data_seek",	php3_sybct_data_seek,		NULL},
	{"sybase_fetch_field",	php3_sybct_fetch_field,		NULL},
	{"sybase_field_seek",	php3_sybct_field_seek,		NULL},
	{"sybase_result",		php3_sybct_result,			NULL},
	{"sybase_affected_rows",php3_sybct_affected_rows,	NULL},
	{"sybase_min_client_severity",	php3_sybct_min_client_severity,		NULL},
	{"sybase_min_server_severity",	php3_sybct_min_server_severity,		NULL},
	{"mssql_connect",		php3_sybct_connect,			NULL},
	{"mssql_pconnect",		php3_sybct_pconnect,		NULL},
	{"mssql_close",			php3_sybct_close,			NULL},
	{"mssql_select_db",		php3_sybct_select_db,		NULL},
	{"mssql_query",			php3_sybct_query,			NULL},
	{"mssql_free_result",	php3_sybct_free_result,		NULL},
	{"mssql_num_rows",		php3_sybct_num_rows,		NULL},
	{"mssql_num_fields",	php3_sybct_num_fields,		NULL},
	{"mssql_fetch_row",		php3_sybct_fetch_row,		NULL},
	{"mssql_fetch_array",	php3_sybct_fetch_array,		NULL},
	{"mssql_fetch_object",	php3_sybct_fetch_object,	NULL},
	{"mssql_data_seek",		php3_sybct_data_seek,		NULL},
	{"mssql_fetch_field",	php3_sybct_fetch_field,		NULL},
	{"mssql_field_seek",	php3_sybct_field_seek,		NULL},
	{"mssql_result",		php3_sybct_result,			NULL},
	{"mssql_affected_rows",	php3_sybct_affected_rows,	NULL},
	{"mssql_min_client_severity",	php3_sybct_min_client_severity,		NULL},
	{"mssql_min_server_severity",	php3_sybct_min_server_severity,		NULL},
	{NULL, NULL, NULL}
};

php3_module_entry sybct_module_entry = {
	"Sybase SQL - CT", sybct_functions, php3_minit_sybct, php3_mshutdown_sybct, php3_rinit_sybct, php3_rshutdown_sybct, php3_info_sybct, STANDARD_MODULE_PROPERTIES
};

#if COMPILE_DL
php3_module_entry *get_module() { return &sybct_module_entry; }
#endif

THREAD_LS sybct_module php3_sybct_module;
THREAD_LS static CS_CONTEXT *context;
THREAD_LS static HashTable *resource_list,*resource_plist;

#define CHECK_LINK(link) { if (link==-1) { php3_error(E_WARNING,"Sybase:  A link to the server could not be established"); RETURN_FALSE; } }


static int _clean_invalid_results(list_entry *le)
{
	if (le->type == php3_sybct_module.le_result) {
		sybct_link *sybct_ptr = ((sybct_result *) le->ptr)->sybct_ptr;
		
		if (!sybct_ptr->valid) {
			return 1;
		}
	}
	return 0;
}


static void _free_sybct_result(sybct_result *result)
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


static void _close_sybct_link(sybct_link *sybct_ptr)
{
	CS_INT con_status;

	sybct_ptr->valid = 0;
	_php3_hash_apply(resource_list,(int (*)(void *))_clean_invalid_results);

	/* Non-persistent connections will always be connected or we wouldn't
	 * get here, but since we want to check the death status anyway
	 * we might as well double-check the connect status.
	 */
	if (ct_con_props(sybct_ptr->connection, CS_GET, CS_CON_STATUS,
					 &con_status, CS_UNUSED, NULL)!=CS_SUCCEED) {
		php3_error(E_WARNING,"Sybase:  Unable to get connection status on close");
		/* Assume the worst. */
		con_status = CS_CONSTAT_CONNECTED | CS_CONSTAT_DEAD;
	}
	if (con_status & CS_CONSTAT_CONNECTED) {
		if ((con_status & CS_CONSTAT_DEAD) || ct_close(sybct_ptr->connection, CS_UNUSED)!=CS_SUCCEED) {
			ct_close(sybct_ptr->connection, CS_FORCE_CLOSE);
		}
	}

	ct_con_drop(sybct_ptr->connection);
	efree(sybct_ptr);
	php3_sybct_module.num_links--;
}


static void _close_sybct_plink(sybct_link *sybct_ptr)
{
	CS_INT con_status;

	/* Persistent connections may have been closed before a failed
	 * reopen attempt.
	 */
	if (ct_con_props(sybct_ptr->connection, CS_GET, CS_CON_STATUS,
					 &con_status, CS_UNUSED, NULL)!=CS_SUCCEED) {
		php3_error(E_WARNING,"Sybase:  Unable to get connection status on close");
		/* Assume the worst. */
		con_status = CS_CONSTAT_CONNECTED | CS_CONSTAT_DEAD;
	}
	if (con_status & CS_CONSTAT_CONNECTED) {
		if ((con_status & CS_CONSTAT_DEAD) || ct_close(sybct_ptr->connection, CS_UNUSED)!=CS_SUCCEED) {
			ct_close(sybct_ptr->connection, CS_FORCE_CLOSE);
		}
	}

	ct_con_drop(sybct_ptr->connection);
	free(sybct_ptr);
	php3_sybct_module.num_persistent--;
	php3_sybct_module.num_links--;
}


static CS_RETCODE _client_message_handler(CS_CONTEXT *context, CS_CONNECTION *connection, CS_CLIENTMSG *errmsg)
{
	if (CS_SEVERITY(errmsg->msgnumber) >= php3_sybct_module.min_client_severity) {
		php3_error(E_WARNING,"Sybase:  Client message:  %s (severity %d)",errmsg->msgstring, CS_SEVERITY(errmsg->msgnumber));
	}

	/* If this is a timeout message, return CS_FAIL to cancel the
	 * operation and mark the connection as dead.
	 */
	if (CS_SEVERITY(errmsg->msgnumber) == CS_SV_RETRY_FAIL &&
		CS_NUMBER(errmsg->msgnumber) == 63 &&
		CS_ORIGIN(errmsg->msgnumber) == 2 &&
		CS_LAYER(errmsg->msgnumber) == 1)
	{
		return CS_FAIL;
	}

    return CS_SUCCEED;
}


static CS_RETCODE _server_message_handler(CS_CONTEXT *context, CS_CONNECTION *connection, CS_SERVERMSG *srvmsg)
{
	if (srvmsg->severity >= php3_sybct_module.min_server_severity) {
		php3_error(E_WARNING,"Sybase:  Server message:  %s (severity %d, procedure %s)",
					srvmsg->text, srvmsg->severity, ((srvmsg->proclen>0) ? srvmsg->proc : "N/A"));
	}

	/* If this is a deadlock message, set the connection's deadlock flag
	 * so we will retry the request.  Sorry about the bare constant here,
	 * but it's not defined anywhere and it's a "well-known" number.
	 */
	if (srvmsg->msgnumber == 1205) {
		sybct_link *sybct;

		if (ct_con_props(connection, CS_GET, CS_USERDATA, &sybct, CS_SIZEOF(sybct), NULL)==CS_SUCCEED) {
		    sybct->deadlock = 1;
		}
		else {
			/* oh well */
		}
	}

    return CS_SUCCEED;
}


int php3_minit_sybct(INIT_FUNC_ARGS)
{
	long timeout;

	if (cs_ctx_alloc(CTLIB_VERSION, &context)!=CS_SUCCEED || ct_init(context,CTLIB_VERSION)!=CS_SUCCEED) {
		return FAILURE;
	}
	/* Initialize message handlers */
	
	if (ct_callback(context, NULL, CS_SET, CS_SERVERMSG_CB, (CS_VOID *)_server_message_handler)!=CS_SUCCEED) {
		php3_error(E_WARNING,"Sybase:  Unable to set server message handler");
	}
	if (ct_callback(context, NULL, CS_SET, CS_CLIENTMSG_CB, (CS_VOID *)_client_message_handler)!=CS_SUCCEED) {
		php3_error(E_WARNING,"Sybase:  Unable to set client message handler");
	}
	
	/* Set datetime conversion format to "Nov  3 1998  8:06PM".
	 * This is the default format for the ct-lib that comes with
	 * Sybase ASE 11.5.1 for Solaris, but the Linux libraries that
	 * come with 11.0.3.3 default to "03/11/98" which is singularly
	 * useless.  This levels the playing field for all platforms.
	 */
	{
		CS_INT dt_convfmt = CS_DATES_SHORT;
		if (cs_dt_info(context, CS_SET, NULL, CS_DT_CONVFMT, CS_UNUSED, &dt_convfmt, sizeof(dt_convfmt), NULL)!=CS_SUCCEED) {
			php3_error(E_WARNING,"Sybase:  Unable to set datetime conversion format");
		}
	}

	/* Set the login and command timeouts.  These are per-context and
	 * can't be set with ct_con_props(), so set them globally from
	 * their config values if requested.  The defaults are 1 minute
	 * for CS_LOGIN_TIMEOUT and CS_NO_LIMIT for CS_TIMEOUT.  The latter
	 * especially is fairly useless for web applications.
	 * Note that depite some noise in the documentation about using
	 * signals to implement timeouts, they are actually implemented
	 * by using poll() or select() on Solaris and Linux.
	 */
	if (cfg_get_long("sybct.login_timeout",&timeout)==SUCCESS) {
		CS_INT cs_login_timeout = timeout;
		if (ct_config(context, CS_SET, CS_LOGIN_TIMEOUT, &cs_login_timeout, CS_UNUSED, NULL)!=CS_SUCCEED) {
			php3_error(E_WARNING,"Sybase:  Unable to set login timeoutt");
		}
	}
	if (cfg_get_long("sybct.timeout",&timeout)==SUCCESS) {
		CS_INT cs_timeout = timeout;
		if (ct_config(context, CS_SET, CS_TIMEOUT, &cs_timeout, CS_UNUSED, NULL)!=CS_SUCCEED) {
			php3_error(E_WARNING,"Sybase:  Unable to set timeout");
		}
	}

	if (cfg_get_long("sybct.allow_persistent",&php3_sybct_module.allow_persistent)==FAILURE) {
		php3_sybct_module.allow_persistent=1;
	}
	if (cfg_get_long("sybct.max_persistent",&php3_sybct_module.max_persistent)==FAILURE) {
		php3_sybct_module.max_persistent=-1;
	}
	if (cfg_get_long("sybct.max_links",&php3_sybct_module.max_links)==FAILURE) {
		php3_sybct_module.max_links=-1;
	}
	if (cfg_get_long("sybct.min_server_severity",&php3_sybct_module.cfg_min_server_severity)==FAILURE) {
		php3_sybct_module.cfg_min_server_severity=10;
	}
	if (cfg_get_long("sybct.min_client_severity",&php3_sybct_module.cfg_min_client_severity)==FAILURE) {
		php3_sybct_module.cfg_min_client_severity=10;
	}
	
	if (cfg_get_string("sybct.hostname",&php3_sybct_module.hostname)==FAILURE
		|| php3_sybct_module.hostname[0]==0) {
		php3_sybct_module.hostname=NULL;
	}

	php3_sybct_module.num_persistent=0;
	php3_sybct_module.le_link = register_list_destructors(_close_sybct_link,NULL);
	php3_sybct_module.le_plink = register_list_destructors(NULL,_close_sybct_plink);
	php3_sybct_module.le_result = register_list_destructors(_free_sybct_result,NULL);

	return SUCCESS;
}


int php3_rinit_sybct(INIT_FUNC_ARGS)
{
	php3_sybct_module.default_link=-1;
	php3_sybct_module.num_links = php3_sybct_module.num_persistent;
	php3_sybct_module.appname = estrndup("PHP 3.0",7);
	php3_sybct_module.server_message = NULL;
	php3_sybct_module.min_server_severity = php3_sybct_module.cfg_min_server_severity;
	php3_sybct_module.min_client_severity = php3_sybct_module.cfg_min_client_severity;
	return SUCCESS;
}

int php3_mshutdown_sybct(SHUTDOWN_FUNC_ARGS)
{
/*	ct_exit(context,CS_UNUSED);
	cs_ctx_drop(context); */
	return SUCCESS;
}

int php3_rshutdown_sybct(SHUTDOWN_FUNC_ARGS)
{
	efree(php3_sybct_module.appname);
	if (php3_sybct_module.server_message) {
		efree(php3_sybct_module.server_message);
	}
	return SUCCESS;
}


static int _php3_sybct_really_connect(sybct_link *sybct, char *host, char *user, char *passwd)
{
	/* set a CS_CONNECTION record */
	if (ct_con_alloc(context, &sybct->connection)!=CS_SUCCEED) {
		php3_error(E_WARNING,"Sybase:  Unable to allocate connection record");
		return 0;
	}
	
	/* Note - this saves a copy of sybct, not a pointer to it. */
	if (ct_con_props(sybct->connection, CS_SET, CS_USERDATA, &sybct, CS_SIZEOF(sybct), NULL)!=CS_SUCCEED) {
		php3_error(E_WARNING,"Sybase:  Unable to set userdata");
		ct_con_drop(sybct->connection);
		return 0;
	}

	if (user) {
		ct_con_props(sybct->connection, CS_SET, CS_USERNAME, user, CS_NULLTERM, NULL);
	}
	if (passwd) {
		ct_con_props(sybct->connection, CS_SET, CS_PASSWORD, passwd, CS_NULLTERM, NULL);
	}
	ct_con_props(sybct->connection, CS_SET, CS_APPNAME, php3_sybct_module.appname, CS_NULLTERM, NULL);

	if (php3_sybct_module.hostname) {
		ct_con_props(sybct->connection, CS_SET, CS_HOSTNAME, php3_sybct_module.hostname, CS_NULLTERM, NULL);
	}

	sybct->valid = 1;
	sybct->dead = 0;

	/* create the link */
	if (ct_connect(sybct->connection, host, CS_NULLTERM)!=CS_SUCCEED) {
		php3_error(E_WARNING,"Sybase:  Unable to connect");
		ct_con_drop(sybct->connection);
		return 0;
	}

	if (ct_cmd_alloc(sybct->connection,&sybct->cmd)!=CS_SUCCEED) {
		php3_error(E_WARNING,"Sybase:  Unable to allocate command record");
		ct_close(sybct->connection,CS_UNUSED);
		ct_con_drop(sybct->connection);
		return 0;
	}

	return 1;
}


static void php3_sybct_do_connect(INTERNAL_FUNCTION_PARAMETERS,int persistent)
{
	char *user,*passwd,*host;
	char *hashed_details;
	int hashed_details_length;
	sybct_link *sybct_ptr;

	resource_list = list;
	resource_plist = plist;
		
	switch(ARG_COUNT(ht)) {
		case 0: /* defaults */
			host=user=passwd=NULL;
			hashed_details_length=5+3;
			hashed_details = (char *) emalloc(hashed_details_length+1);
			strcpy(hashed_details,"sybct___");
			break;
		case 1: {
				pval *yyhost;
				
				if (getParameters(ht, 1, &yyhost)==FAILURE) {
					RETURN_FALSE;
				}
				convert_to_string(yyhost);
				host = yyhost->value.str.val;
				user=passwd=NULL;
				hashed_details_length = yyhost->value.str.len+5+3;
				hashed_details = (char *) emalloc(hashed_details_length+1);
				sprintf(hashed_details,"sybct_%s__",yyhost->value.str.val);
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
				hashed_details_length = yyhost->value.str.len+yyuser->value.str.len+5+3;
				hashed_details = (char *) emalloc(hashed_details_length+1);
				sprintf(hashed_details,"sybct_%s_%s_",yyhost->value.str.val,yyuser->value.str.val);
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
				hashed_details_length = yyhost->value.str.len+yyuser->value.str.len+yypasswd->value.str.len+5+3;
				hashed_details = (char *) emalloc(hashed_details_length+1);
				sprintf(hashed_details,"sybct_%s_%s_%s",yyhost->value.str.val,yyuser->value.str.val,yypasswd->value.str.val); /* SAFE */
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}


	if (!php3_sybct_module.allow_persistent) {
		persistent=0;
	}
	if (persistent) {
		list_entry *le;

		/* try to find if we already have this link in our persistent list */
		if (_php3_hash_find(plist, hashed_details, hashed_details_length+1, (void **) &le)==FAILURE) {  /* we don't */
			list_entry new_le;

			if (php3_sybct_module.max_links!=-1 && php3_sybct_module.num_links>=php3_sybct_module.max_links) {
				php3_error(E_WARNING,"Sybase:  Too many open links (%d)",php3_sybct_module.num_links);
				efree(hashed_details);
				RETURN_FALSE;
			}
			if (php3_sybct_module.max_persistent!=-1 && php3_sybct_module.num_persistent>=php3_sybct_module.max_persistent) {
				php3_error(E_WARNING,"Sybase:  Too many open persistent links (%d)",php3_sybct_module.num_persistent);
				efree(hashed_details);
				RETURN_FALSE;
			}

			sybct_ptr = (sybct_link *) malloc(sizeof(sybct_link));
			if (!_php3_sybct_really_connect(sybct_ptr, host, user, passwd)) {
				free(sybct_ptr);
				efree(hashed_details);
				RETURN_FALSE;
			}

			/* hash it up */
			new_le.type = php3_sybct_module.le_plink;
			new_le.ptr = sybct_ptr;
			if (_php3_hash_update(plist, hashed_details, hashed_details_length+1, (void *) &new_le, sizeof(list_entry),NULL)==FAILURE) {
				ct_close(sybct_ptr->connection, CS_UNUSED);
				ct_con_drop(sybct_ptr->connection);
				free(sybct_ptr);
				efree(hashed_details);
				RETURN_FALSE;
			}
			php3_sybct_module.num_persistent++;
			php3_sybct_module.num_links++;
		} else {  /* we do */
			CS_INT con_status;

			if (le->type != php3_sybct_module.le_plink) {
				efree(hashed_details);
				RETURN_FALSE;
			}
			
			sybct_ptr = (sybct_link *) le->ptr;

			/* If the link has died, close it and overwrite it with a new one. */

			if (ct_con_props(sybct_ptr->connection, CS_GET, CS_CON_STATUS,
							 &con_status, CS_UNUSED, NULL)!=CS_SUCCEED) {
				php3_error(E_WARNING,"Sybase:  Unable to get connection status");
				efree(hashed_details);
				RETURN_FALSE;
			}
			if (!(con_status & CS_CONSTAT_CONNECTED) || (con_status & CS_CONSTAT_DEAD) || sybct_ptr->dead) {
				sybct_link sybct;

				if (con_status & CS_CONSTAT_CONNECTED) {
					ct_close(sybct_ptr->connection, CS_FORCE_CLOSE);
				}
				/* Create a new connection, then replace the old
				 * connection.  If we fail to create a new connection,
				 * put the old one back so there will be a connection,
				 * even if it is a non-functional one.  This is because
				 * code may still be holding an id for this connection
				 * so we can't free the CS_CONNECTION.
				 * (This is actually totally hokey, it would be better
				 * to just ct_con_drop() the connection and set
				 * sybct_ptr->connection to NULL, then test it for
				 * NULL before trying to use it elsewhere . . .)
				 */
				memcpy(&sybct,sybct_ptr,sizeof(sybct_link));
				if (!_php3_sybct_really_connect(sybct_ptr, host, user, passwd)) {
					memcpy(sybct_ptr,&sybct,sizeof(sybct_link));
					efree(hashed_details);
					RETURN_FALSE;
				}
				ct_con_drop(sybct.connection); /* drop old connection */
			}
		}
		return_value->value.lval = php3_list_insert(sybct_ptr,php3_sybct_module.le_plink);
		return_value->type = IS_LONG;
	} else { /* non persistent */
		list_entry *index_ptr,new_index_ptr;
		
		/* first we check the hash for the hashed_details key.  if it exists,
		 * it should point us to the right offset where the actual sybct link sits.
		 * if it doesn't, open a new sybct link, add it to the resource list,
		 * and add a pointer to it with hashed_details as the key.
		 */
		if (_php3_hash_find(list,hashed_details,hashed_details_length+1,(void **) &index_ptr)==SUCCESS) {
			int type,link;
			void *ptr;

			if (index_ptr->type != le_index_ptr) {
				efree(hashed_details);
				RETURN_FALSE;
			}
			link = (int) index_ptr->ptr;
			ptr = php3_list_find(link,&type);   /* check if the link is still there */
			if (ptr && (type==php3_sybct_module.le_link || type==php3_sybct_module.le_plink)) {
				return_value->value.lval = php3_sybct_module.default_link = link;
				return_value->type = IS_LONG;
				efree(hashed_details);
				return;
			} else {
				_php3_hash_del(list,hashed_details,hashed_details_length+1);
			}
		}
		if (php3_sybct_module.max_links!=-1 && php3_sybct_module.num_links>=php3_sybct_module.max_links) {
			php3_error(E_WARNING,"Sybase:  Too many open links (%d)",php3_sybct_module.num_links);
			efree(hashed_details);
			RETURN_FALSE;
		}

		sybct_ptr = (sybct_link *) emalloc(sizeof(sybct_link));
		if (!_php3_sybct_really_connect(sybct_ptr, host, user, passwd)) {
			efree(sybct_ptr);
			efree(hashed_details);
			RETURN_FALSE;
		}

		/* add it to the list */
		return_value->value.lval = php3_list_insert(sybct_ptr,php3_sybct_module.le_link);
		return_value->type = IS_LONG;
		
		/* add it to the hash */
		new_index_ptr.ptr = (void *) return_value->value.lval;
		new_index_ptr.type = le_index_ptr;
		if (_php3_hash_update(list,hashed_details,hashed_details_length+1,(void *) &new_index_ptr, sizeof(list_entry),NULL)==FAILURE) {
			ct_close(sybct_ptr->connection, CS_UNUSED);
			ct_con_drop(sybct_ptr->connection);
			efree(sybct_ptr);
			efree(hashed_details);
			RETURN_FALSE;
		}
		php3_sybct_module.num_links++;
	}
	efree(hashed_details);
	php3_sybct_module.default_link=return_value->value.lval;
}


static int php3_sybct_get_default_link(INTERNAL_FUNCTION_PARAMETERS)
{
	if (php3_sybct_module.default_link==-1) { /* no link opened yet, implicitly open one */
		php3_sybct_do_connect(0,return_value,list,plist,0);
	}
	return php3_sybct_module.default_link;
}


PHP_FUNCTION(sybct_connect)
{
	php3_sybct_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);
}

PHP_FUNCTION(sybct_pconnect)
{
	php3_sybct_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,1);
}


PHP_FUNCTION(sybct_close)
{
	pval *sybct_link_index;
	int id,type;
	
	switch (ARG_COUNT(ht)) {
		case 0:
			id = php3_sybct_module.default_link;
			break;
		case 1:
			if (getParameters(ht, 1, &sybct_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(sybct_link_index);
			id = sybct_link_index->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	php3_list_find(id,&type);
	if (type!=php3_sybct_module.le_link && type!=php3_sybct_module.le_plink) {
		php3_error(E_WARNING,"%d is not a Sybase link index",id);
		RETURN_FALSE;
	}
	
	php3_list_delete(id);
	RETURN_TRUE;
}
	

static int exec_cmd(sybct_link *sybct_ptr,char *cmdbuf)
{
	CS_RETCODE retcode;
	CS_INT restype;
	int failure=0;

	/* Fail if we already marked this connection dead. */

	if (sybct_ptr->dead) {
		return FAILURE;
	}

	/*
	 ** Get a command handle, store the command string in it, and
	 ** send it to the server.
	 */
	
	if (ct_command(sybct_ptr->cmd, CS_LANG_CMD, cmdbuf, CS_NULLTERM, CS_UNUSED)!=CS_SUCCEED) {
		sybct_ptr->dead = 1;
		return FAILURE;
	}
	if (ct_send(sybct_ptr->cmd)!=CS_SUCCEED) {
		sybct_ptr->dead = 1;
		return FAILURE;
	}
	
	while ((retcode = ct_results(sybct_ptr->cmd, &restype))==CS_SUCCEED) {
		switch ((int) restype) {
			case CS_CMD_SUCCEED:
			case CS_CMD_DONE:
				break;
		
			case CS_CMD_FAIL:
				failure=1;
				break;
			
			case CS_STATUS_RESULT:
				ct_cancel(NULL, sybct_ptr->cmd, CS_CANCEL_CURRENT);
				break;
			
			default:
				failure=1;
				break;
		}
		if (failure) {
			ct_cancel(NULL, sybct_ptr->cmd, CS_CANCEL_ALL);
			return FAILURE;
		}
	}

	switch (retcode) {
		case CS_END_RESULTS:
			return SUCCESS;
			break;

		case CS_FAIL:
			/* Hopefully this either cleans up the connection, or the
			 * connection ends up marked dead so it will be reopened
			 * if it is persistent.  We may want to do
			 * ct_close(CS_FORCE_CLOSE) if ct_cancel() fails; see the
			 * doc for ct_results()==CS_FAIL.
			 */
			ct_cancel(NULL, sybct_ptr->cmd, CS_CANCEL_ALL);
			/* Don't take chances with the vagaries of ct-lib.  Mark it
			 * dead ourselves.
			 */
			sybct_ptr->dead = 1;
			return FAILURE;

		default:
			return FAILURE;
	}
}


PHP_FUNCTION(sybct_select_db)
{
	pval *db,*sybct_link_index;
	int id,type;
	char *cmdbuf;
	sybct_link  *sybct_ptr;
	
	switch(ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &db)==FAILURE) {
				RETURN_FALSE;
			}
			id = php3_sybct_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			break;
		case 2:
			if (getParameters(ht, 2, &db, &sybct_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(sybct_link_index);
			id = sybct_link_index->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	CHECK_LINK(id);
	
	sybct_ptr = (sybct_link *) php3_list_find(id,&type);
	if (type!=php3_sybct_module.le_link && type!=php3_sybct_module.le_plink) {
		php3_error(E_WARNING,"%d is not a Sybase link index",id);
		RETURN_FALSE;
	}
	
	convert_to_string(db);
	cmdbuf = (char *) emalloc(sizeof("use ")+db->value.str.len+1);
	sprintf(cmdbuf,"use %s",db->value.str.val); /* SAFE */

	if (exec_cmd(sybct_ptr,cmdbuf)==FAILURE) {
		efree(cmdbuf);
		RETURN_FALSE;
	} else {
		efree(cmdbuf);
		RETURN_TRUE;
	}
}


static sybct_result * _php3_sybct_fetch_result_set (sybct_link *sybct_ptr)
{
	int num_fields;
	sybct_result *result;
	char **tmp_buffer;
	CS_INT *lengths;
	CS_SMALLINT *indicators;
	unsigned char *numerics;
	CS_INT *types;
	CS_DATAFMT *datafmt;
	int i,j,retcode;
	int blocks_initialized=1;

	/* The following is more or less the equivalent of mysql_store_result().
	 * fetch all rows from the server into the row buffer, thus:
	 * 1)  Being able to fire up another query without explicitly reading all rows
	 * 2)  Having numrows accessible
	 */

	if (ct_res_info(sybct_ptr->cmd, CS_NUMDATA, &num_fields, CS_UNUSED, NULL)!=CS_SUCCEED) {
		return NULL;
	}
	
	result = (sybct_result *) emalloc(sizeof(sybct_result));
	result->data = (pval **) emalloc(sizeof(pval *)*SYBASE_ROWS_BLOCK);
	result->fields = NULL;
	result->sybct_ptr = sybct_ptr;
	result->cur_field=result->cur_row=result->num_rows=0;
	result->num_fields = num_fields;

	tmp_buffer = (char **) emalloc(sizeof(char *)*num_fields);
	lengths = (CS_INT *) emalloc(sizeof(CS_INT)*num_fields);
	indicators = (CS_SMALLINT *) emalloc(sizeof(CS_INT)*num_fields);
	datafmt = (CS_DATAFMT *) emalloc(sizeof(CS_DATAFMT)*num_fields);
	numerics = (unsigned char *) emalloc(sizeof(unsigned char)*num_fields);
	types = (CS_INT *) emalloc(sizeof(CS_INT)*num_fields);
	

	for (i=0; i<num_fields; i++) {
		ct_describe(sybct_ptr->cmd,i+1,&datafmt[i]);
		types[i] = datafmt[i].datatype;
		switch (datafmt[i].datatype) {
			case CS_CHAR_TYPE:
			case CS_VARCHAR_TYPE:
			case CS_TEXT_TYPE:
			case CS_IMAGE_TYPE:
				datafmt[i].maxlength++;
				numerics[i] = 0;
				break;
			case CS_BINARY_TYPE:
			case CS_VARBINARY_TYPE:
				datafmt[i].maxlength *= 2;
				datafmt[i].maxlength++;
				numerics[i] = 0;
				break;
			case CS_BIT_TYPE:
			case CS_TINYINT_TYPE:
				datafmt[i].maxlength = 4;
				numerics[i] = 1;
				break;
			case CS_SMALLINT_TYPE:
				datafmt[i].maxlength = 7;
				numerics[i] = 1;
				break;
			case CS_INT_TYPE:
				datafmt[i].maxlength = 12;
				numerics[i] = 1;
				break;
			case CS_REAL_TYPE:
			case CS_FLOAT_TYPE:
				datafmt[i].maxlength = 24;
				numerics[i] = 1;
				break;
			case CS_MONEY_TYPE:
			case CS_MONEY4_TYPE:
				datafmt[i].maxlength = 24;
				numerics[i] = 0;
				break;
			case CS_DATETIME_TYPE:
			case CS_DATETIME4_TYPE:
				datafmt[i].maxlength = 30;
				numerics[i] = 0;
				break;
			case CS_NUMERIC_TYPE:
			case CS_DECIMAL_TYPE:
				datafmt[i].maxlength = datafmt[i].precision + 3;
				numerics[i] = 1;
				break;
			default:
				datafmt[i].maxlength++;
				numerics[i] = 0;
				break;
		}
		tmp_buffer[i] = (char *)emalloc(datafmt[i].maxlength);
		datafmt[i].datatype = CS_CHAR_TYPE;
		datafmt[i].format = CS_FMT_NULLTERM;
		ct_bind(sybct_ptr->cmd,i+1,&datafmt[i],tmp_buffer[i],&lengths[i],&indicators[i]);
	}
	
	while ((retcode=ct_fetch(sybct_ptr->cmd,CS_UNUSED,CS_UNUSED,CS_UNUSED,NULL))==CS_SUCCEED
			|| retcode==CS_ROW_FAIL) {
		/*
		if (retcode==CS_ROW_FAIL) {
			php3_error(E_WARNING,"Sybase:  Error reading row %d",result->num_rows);
		}
		*/
		i = result->num_rows++;
		if (result->num_rows > blocks_initialized*SYBASE_ROWS_BLOCK) {
			result->data = (pval **) erealloc(result->data,sizeof(pval *)*SYBASE_ROWS_BLOCK*(++blocks_initialized));
		}
		result->data[i] = (pval *) emalloc(sizeof(pval)*num_fields);
		for (j=0; j<num_fields; j++) {
			if (indicators[j] == -1) { /* null value */
				var_reset(&result->data[i][j]);
			} else {
				result->data[i][j].value.str.len = lengths[j]-1;  /* we don't need the NULL in the length */
				result->data[i][j].value.str.val = estrndup(tmp_buffer[j],lengths[j]);
				result->data[i][j].type = IS_STRING;
			}
		}
	}

	if (retcode != CS_END_DATA) {
		_free_sybct_result(result);
		result = NULL;
	} else {
		result->fields = (sybct_field *) emalloc(sizeof(sybct_field)*num_fields);
		j=0;
		for (i=0; i<num_fields; i++) {
			char computed_buf[16];
		
			if (datafmt[i].namelen>0) {
				result->fields[i].name = estrndup(datafmt[i].name,datafmt[i].namelen);
			} else {
				if (j>0) {
					snprintf(computed_buf,16,"computed%d",j);
				} else {
					strcpy(computed_buf,"computed");
				}
				result->fields[i].name = estrdup(computed_buf);
				j++;
			}
			result->fields[i].column_source = empty_string;
			result->fields[i].max_length = datafmt[i].maxlength-1;
			result->fields[i].numeric = numerics[i];
			result->fields[i].type = types[i];
		}
	}

	efree(datafmt);
	efree(lengths);
	efree(indicators);
	efree(numerics);
	efree(types);
	for (i=0; i<num_fields; i++) {
		efree(tmp_buffer[i]);
	}
	efree(tmp_buffer);

	return result;
}


PHP_FUNCTION(sybct_query)
{
	pval *query,*sybct_link_index;
	int id,type;
	sybct_link *sybct_ptr;
	sybct_result *result;
	CS_INT restype;
	CS_RETCODE retcode;
	enum {
		Q_RESULT,				/* Success with results. */
		Q_SUCCESS,				/* Success but no results. */
		Q_FAILURE,				/* Failure, no results. */
	} status;
	
	switch(ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &query)==FAILURE) {
				RETURN_FALSE;
			}
			id = php3_sybct_module.default_link;
			break;
		case 2:
			if (getParameters(ht, 2, &query, &sybct_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(sybct_link_index);
			id = sybct_link_index->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	sybct_ptr = (sybct_link *) php3_list_find(id,&type);
	if (type!=php3_sybct_module.le_link && type!=php3_sybct_module.le_plink) {
		php3_error(E_WARNING,"%d is not a Sybase link index",id);
		RETURN_FALSE;
	}
	
	convert_to_string(query);
	
	/* Fail if we already marked this connection dead. */

	if (sybct_ptr->dead) {
		RETURN_FALSE;
	}

	/* Repeat until we don't deadlock. */

	for (;;) {
		result = NULL;
		sybct_ptr->deadlock = 0;
		sybct_ptr->affected_rows = 0;

		/* On Solaris 11.5, ct_command() can be moved outside the
		 * loop, but not on Linux 11.0.
		 */
		if (ct_command(sybct_ptr->cmd, CS_LANG_CMD, query->value.str.val, CS_NULLTERM, CS_UNUSED)!=CS_SUCCEED) {
			/* If this didn't work, the connection is screwed but
			 * ct-lib might not set CS_CONSTAT_DEAD.  So set our own
			 * flag.  This happens sometimes when the database is restarted
			 * and/or its machine is rebooted, and ct_command() returns
			 * CS_BUSY for some reason.
			 */
			sybct_ptr->dead = 1;
		    RETURN_FALSE;
		}

		if (ct_send(sybct_ptr->cmd)!=CS_SUCCEED) {
			ct_cancel(NULL, sybct_ptr->cmd, CS_CANCEL_ALL);
			sybct_ptr->dead = 1;
			RETURN_FALSE;
		}

		/* Use the first result set or succeed/fail status and discard the
		 * others.  Applications really shouldn't be making calls that
		 * return multiple result sets, but if they do then we need to
		 * properly read or cancel them or the connection will become
		 * unusable.
		 */
		if (ct_results(sybct_ptr->cmd, &restype)!=CS_SUCCEED) {
			ct_cancel(NULL, sybct_ptr->cmd, CS_CANCEL_ALL);
			sybct_ptr->dead = 1;
			RETURN_FALSE;
		}

		switch ((int) restype) {
			case CS_CMD_FAIL:
			default:
				status = Q_FAILURE;
				break;
			case CS_CMD_SUCCEED:
			case CS_CMD_DONE: {
					CS_INT row_count;
					if (ct_res_info(sybct_ptr->cmd, CS_ROW_COUNT, &row_count, CS_UNUSED, NULL)==CS_SUCCEED) {
						sybct_ptr->affected_rows = (long)row_count;
					}
				}
				/* Fall through */
			case CS_COMPUTEFMT_RESULT:
			case CS_ROWFMT_RESULT:
			case CS_DESCRIBE_RESULT:
			case CS_MSG_RESULT:
				status = Q_SUCCESS;
				break;
			case CS_COMPUTE_RESULT:
			case CS_CURSOR_RESULT:
			case CS_PARAM_RESULT:
			case CS_ROW_RESULT:
			case CS_STATUS_RESULT:
				result = _php3_sybct_fetch_result_set(sybct_ptr);
				if (result == NULL) {
					ct_cancel(NULL, sybct_ptr->cmd, CS_CANCEL_ALL);
					sybct_ptr->dead = 1;
					RETURN_FALSE;
				}
				status = Q_RESULT;
				break;
		}

		/* The only restype we should get now is CS_CMD_DONE, possibly
		 * followed by a CS_STATUS_RESULT/CS_CMD_SUCCEED/CS_CMD_DONE
		 * sequence if the command was a stored procedure call.  But we
		 * still need to read and discard unexpected results.  We might
		 * want to return a failure in this case because the application
		 * won't be getting all the results it asked for.
		 */
		while ((retcode = ct_results(sybct_ptr->cmd, &restype))==CS_SUCCEED) {
			switch ((int) restype) {
				case CS_CMD_SUCCEED:
				case CS_CMD_DONE:
					break;

				case CS_CMD_FAIL:
					status = Q_FAILURE;
					break;
			
				case CS_COMPUTE_RESULT:
				case CS_CURSOR_RESULT:
				case CS_PARAM_RESULT:
				case CS_ROW_RESULT:
					/* Unexpected results, cancel them. */
				case CS_STATUS_RESULT:
					ct_cancel(NULL, sybct_ptr->cmd, CS_CANCEL_CURRENT);
					break;
			
				default:
					status = Q_FAILURE;
					break;
			}
			if (status == Q_FAILURE) {
				ct_cancel(NULL, sybct_ptr->cmd, CS_CANCEL_ALL);
			}
		}

		switch (retcode) {
			case CS_END_RESULTS:
				/* Normal. */
				break;

			case CS_FAIL:
				/* Hopefully this either cleans up the connection, or the
				 * connection ends up marked dead so it will be reopened
				 * if it is persistent.  We may want to do
				 * ct_close(CS_FORCE_CLOSE) if ct_cancel() fails; see the
				 * doc for ct_results()==CS_FAIL.
				 */
				ct_cancel(NULL, sybct_ptr->cmd, CS_CANCEL_ALL);
				/* Don't take chances with the vagaries of ct-lib.  Mark it
				 * dead ourselves.
				 */
				sybct_ptr->dead = 1;
			case CS_CANCELED:
			default:
				status = Q_FAILURE;
				break;
		}

		/* If query completed without deadlock, break out of the loop.
		 * Sometimes deadlock results in failures and sometimes not,
		 * it seems to depend on the server flavor.  But we want to
		 * retry all deadlocks.
		 */
		if (sybct_ptr->dead || sybct_ptr->deadlock == 0) {
			break;
		}

		/* Get rid of any results we may have fetched.  This happens:
		 * e.g., our result set may be a stored procedure status which
		 * is returned even if the stored procedure deadlocks.  As an
		 * optimization, we could try not to fetch results in known
		 * deadlock conditions, but deadlock is (should be) rare.
		 */
		if (result != NULL) {
			_free_sybct_result(result);
		}
	}

	if (status == Q_SUCCESS) {
		RETURN_TRUE;
	}

	if (status == Q_FAILURE) {
		if (result != NULL) {
			_free_sybct_result(result);
		}
		RETURN_FALSE;
	}

	return_value->value.lval = php3_list_insert(result,php3_sybct_module.le_result);
	return_value->type = IS_LONG;
}


PHP_FUNCTION(sybct_free_result)
{
	pval *sybct_result_index;
	sybct_result *result;
	int type;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &sybct_result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(sybct_result_index);
	if (sybct_result_index->value.lval==0) {
		RETURN_FALSE;
	}
	result = (sybct_result *) php3_list_find(sybct_result_index->value.lval,&type);
	
	if (type!=php3_sybct_module.le_result) {
		php3_error(E_WARNING,"%d is not a Sybase result index",sybct_result_index->value.lval);
		RETURN_FALSE;
	}
	php3_list_delete(sybct_result_index->value.lval);
	RETURN_TRUE;
}


#if 0
PHP_FUNCTION(sybct_get_last_message)
{
	if (php3_sybct_module.server_message) {
		RETURN_STRING(php3_sybct_module.server_message,1);
	}
}
#endif 


PHP_FUNCTION(sybct_num_rows)
{
	pval *result_index;
	int type,id;
	sybct_result *result;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result_index);
	id = result_index->value.lval;
	
	result = (sybct_result *) php3_list_find(id,&type);
	if (type!=php3_sybct_module.le_result) {
		php3_error(E_WARNING,"%d is not a Sybase result index",id);
		RETURN_FALSE;
	}	
	
	return_value->value.lval = result->num_rows;
	return_value->type = IS_LONG;
}


PHP_FUNCTION(sybct_num_fields)
{
	pval *result_index;
	int type,id;
	sybct_result *result;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result_index);
	id = result_index->value.lval;
	
	result = (sybct_result *) php3_list_find(id,&type);
	if (type!=php3_sybct_module.le_result) {
		php3_error(E_WARNING,"%d is not a Sybase result index",id);
		RETURN_FALSE;
	}	
	
	return_value->value.lval = result->num_fields;
	return_value->type = IS_LONG;
}


PHP_FUNCTION(sybct_fetch_row)
{
	pval *sybct_result_index;
	int type,i,id;
	sybct_result *result;
	pval *field_content;

	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &sybct_result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(sybct_result_index);
	id = sybct_result_index->value.lval;
	
	result = (sybct_result *) php3_list_find(id,&type);
	if (type!=php3_sybct_module.le_result) {
		php3_error(E_WARNING,"%d is not a Sybase result index",id);
		RETURN_FALSE;
	}
	
	if (result->cur_row >= result->num_rows) {
		RETURN_FALSE;
	}
	
	array_init(return_value);
	for (i=0; i<result->num_fields; i++) {
		field_content = (pval *) emalloc(sizeof(pval));
		*field_content = result->data[result->cur_row][i];
		INIT_PZVAL(field_content);
		pval_copy_constructor(field_content);
		_php3_hash_index_update(return_value->value.ht, i, (void *) &field_content, sizeof(pval* ), NULL);
	}
	result->cur_row++;
}


static PHP_FUNCTION(sybct_fetch_hash)
{
	pval *sybct_result_index;
	sybct_result *result;
	int type;
	int i;
	pval *tmp;
	PLS_FETCH();
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &sybct_result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(sybct_result_index);
	result = (sybct_result *) php3_list_find(sybct_result_index->value.lval,&type);
	
	if (type!=php3_sybct_module.le_result) {
		php3_error(E_WARNING,"%d is not a Sybase result index",sybct_result_index->value.lval);
		RETURN_FALSE;
	}
	
	if (result->cur_row >= result->num_rows) {
		RETURN_FALSE;
	}
	
	if (array_init(return_value)==FAILURE) {
		RETURN_FALSE;
	}
	
	for (i=0; i<result->num_fields; i++) {
		tmp = (pval *) emalloc(sizeof(pval));
		*tmp = result->data[result->cur_row][i];
		INIT_PZVAL(tmp);
		pval_copy_constructor(tmp);
		if (PG(magic_quotes_runtime) && tmp->type == IS_STRING) {
			tmp->value.str.val = _php3_addslashes(tmp->value.str.val,tmp->value.str.len,&tmp->value.str.len,1);
		}
		_php3_hash_index_update(return_value->value.ht, i, (void *) &tmp, sizeof(pval *), NULL);
		_php3_hash_update(return_value->value.ht, result->fields[i].name, strlen(result->fields[i].name)+1, (void *) &tmp, sizeof(pval *), NULL);
	}
	result->cur_row++;
}


PHP_FUNCTION(sybct_fetch_object)
{
	php3_sybct_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	if (return_value->type==IS_ARRAY) {
		return_value->type=IS_OBJECT;
	}
}


PHP_FUNCTION(sybct_fetch_array)
{
	php3_sybct_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_FUNCTION(sybct_data_seek)
{
	pval *sybct_result_index,*offset;
	int type,id;
	sybct_result *result;

	if (ARG_COUNT(ht)!=2 || getParameters(ht, 2, &sybct_result_index, &offset)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(sybct_result_index);
	id = sybct_result_index->value.lval;
	
	result = (sybct_result *) php3_list_find(id,&type);
	if (type!=php3_sybct_module.le_result) {
		php3_error(E_WARNING,"%d is not a Sybase result index",id);
		RETURN_FALSE;
	}

	convert_to_long(offset);
	if (offset->value.lval<0 || offset->value.lval>=result->num_rows) {
		php3_error(E_WARNING,"Sybase:  Bad row offset");
		RETURN_FALSE;
	}
	
	result->cur_row = offset->value.lval;
	RETURN_TRUE;
}


static char *php3_sybct_get_field_name(CS_INT type)
{
	switch (type) {
		case CS_CHAR_TYPE:
		case CS_VARCHAR_TYPE:
		case CS_TEXT_TYPE:
			return "string";
			break;
		case CS_IMAGE_TYPE:
			return "image";
			break;
		case CS_BINARY_TYPE:
		case CS_VARBINARY_TYPE:
			return "blob";
			break;
		case CS_BIT_TYPE:
			return "bit";
			break;
		case CS_TINYINT_TYPE:
		case CS_SMALLINT_TYPE:
		case CS_INT_TYPE:
			return "int";
			break;
		case CS_REAL_TYPE:
		case CS_FLOAT_TYPE:
		case CS_NUMERIC_TYPE:
		case CS_DECIMAL_TYPE:
			return "real";
			break;
		case CS_MONEY_TYPE:
		case CS_MONEY4_TYPE:
			return "money";
			break;
		case CS_DATETIME_TYPE:
		case CS_DATETIME4_TYPE:
			return "datetime";
			break;
		default:
			return "unknown";
			break;
	}
}


PHP_FUNCTION(sybct_fetch_field)
{
	pval *sybct_result_index,*offset;
	int type,id,field_offset;
	sybct_result *result;

	switch (ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &sybct_result_index)==FAILURE) {
				RETURN_FALSE;
			}
			field_offset=-1;
			break;
		case 2:
			if (getParameters(ht, 2, &sybct_result_index, &offset)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(offset);
			field_offset = offset->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	convert_to_long(sybct_result_index);
	id = sybct_result_index->value.lval;
	
	result = (sybct_result *) php3_list_find(id,&type);
	if (type!=php3_sybct_module.le_result) {
		php3_error(E_WARNING,"%d is not a Sybase result index",id);
		RETURN_FALSE;
	}
	
	if (field_offset==-1) {
		field_offset = result->cur_field;
		result->cur_field++;
	}
	
	if (field_offset<0 || field_offset >= result->num_fields) {
		if (ARG_COUNT(ht)==2) { /* field specified explicitly */
			php3_error(E_WARNING,"Sybase:  Bad column offset");
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
	add_property_string(return_value, "type", php3_sybct_get_field_name(result->fields[field_offset].type), 1);
}


PHP_FUNCTION(sybct_field_seek)
{
	pval *sybct_result_index,*offset;
	int type,id,field_offset;
	sybct_result *result;

	if (ARG_COUNT(ht)!=2 || getParameters(ht, 2, &sybct_result_index, &offset)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(sybct_result_index);
	id = sybct_result_index->value.lval;
	
	result = (sybct_result *) php3_list_find(id,&type);
	if (type!=php3_sybct_module.le_result) {
		php3_error(E_WARNING,"%d is not a Sybase result index",id);
		RETURN_FALSE;
	}
	
	convert_to_long(offset);
	field_offset = offset->value.lval;
	
	if (field_offset<0 || field_offset >= result->num_fields) {
		php3_error(E_WARNING,"Sybase:  Bad column offset");
		RETURN_FALSE;
	}

	result->cur_field = field_offset;
	RETURN_TRUE;
}


PHP_FUNCTION(sybct_result)
{
	pval *row, *field, *sybct_result_index;
	int id,type,field_offset=0;
	sybct_result *result;
	

	if (ARG_COUNT(ht)!=3 || getParameters(ht, 3, &sybct_result_index, &row, &field)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(sybct_result_index);
	id = sybct_result_index->value.lval;
	
	result = (sybct_result *) php3_list_find(id,&type);
	if (type!=php3_sybct_module.le_result) {
		php3_error(E_WARNING,"%d is not a Sybase result index",id);
		RETURN_FALSE;
	}
	
	convert_to_long(row);
	if (row->value.lval<0 || row->value.lval>=result->num_rows) {
		php3_error(E_WARNING,"Sybase:  Bad row offset (%d)",row->value.lval);
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
				php3_error(E_WARNING,"Sybase:  %s field not found in result",field->value.str.val);
				RETURN_FALSE;
			}
			break;
		}
		default:
			convert_to_long(field);
			field_offset = field->value.lval;
			if (field_offset<0 || field_offset>=result->num_fields) {
				php3_error(E_WARNING,"Sybase:  Bad column offset specified");
				RETURN_FALSE;
			}
			break;
	}

	*return_value = result->data[row->value.lval][field_offset];
	pval_copy_constructor(return_value);
}


PHP_FUNCTION(sybct_affected_rows)
{
	pval *sybct_link_index;
	int id,type;
	sybct_link *sybct_ptr;
	CS_INT row_count;
	
	switch(ARG_COUNT(ht)) {
		case 0:
			id = php3_sybct_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			break;
		case 1:
			if (getParameters(ht, 1, &sybct_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(sybct_link_index);
			id = sybct_link_index->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	sybct_ptr = (sybct_link *) php3_list_find(id,&type);
	if (type!=php3_sybct_module.le_link && type!=php3_sybct_module.le_plink) {
		php3_error(E_WARNING,"%d is not a Sybase link index",id);
		RETURN_FALSE;
	}

	return_value->value.lval = sybct_ptr->affected_rows;
	return_value->type = IS_LONG;
}


void php3_info_sybct(ZEND_MODULE_INFO_FUNC_ARGS)
{
	char maxp[16],maxl[16];
	
	if (php3_sybct_module.max_persistent==-1) {
		strcpy(maxp,"Unlimited");
	} else {
		snprintf(maxp,15,"%ld",php3_sybct_module.max_persistent);
		maxp[15]=0;
	}
	if (php3_sybct_module.max_links==-1) {
		strcpy(maxl,"Unlimited");
	} else {
		snprintf(maxl,15,"%ld",php3_sybct_module.max_links);
		maxl[15]=0;
	}
	php3_printf("<table cellpadding=5>"
				"<tr><td>Allow persistent links:</td><td>%s</td></tr>\n"
				"<tr><td>Persistent links:</td><td>%d/%s</td></tr>\n"
				"<tr><td>Total links:</td><td>%d/%s</td></tr>\n"
				"<tr><td>Application name:</td><td>%s</td></tr>\n"
				"</table>\n",
				(php3_sybct_module.allow_persistent?"Yes":"No"),
				php3_sybct_module.num_persistent,maxp,
				php3_sybct_module.num_links,maxl,
				php3_sybct_module.appname);
}


PHP_FUNCTION(sybct_min_client_severity)
{
	pval *severity;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &severity)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(severity);
	php3_sybct_module.min_client_severity = severity->value.lval;
}


PHP_FUNCTION(sybct_min_server_severity)
{
	pval *severity;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &severity)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(severity);
	php3_sybct_module.min_server_severity = severity->value.lval;
}


#endif
