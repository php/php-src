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
   |          Tom May <tom@go2net.com>                                    |
   +----------------------------------------------------------------------+
 */
 
/* $Id$ */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_sybase_ct.h"
#include "ext/standard/php_standard.h"
#include "ext/standard/info.h"
#include "php_globals.h"
#include "php_ini.h"

/* True globals, no need for thread safety */
static int le_link, le_plink, le_result;

#if HAVE_SYBASE_CT

function_entry sybase_functions[] = {
	PHP_FE(sybase_connect,				NULL)
	PHP_FE(sybase_pconnect,				NULL)
	PHP_FE(sybase_close,				NULL)
	PHP_FE(sybase_select_db,			NULL)
	PHP_FE(sybase_query,				NULL)
	PHP_FE(sybase_free_result,			NULL)
	PHP_FE(sybase_get_last_message,		NULL)
	PHP_FE(sybase_num_rows,				NULL)
	PHP_FE(sybase_num_fields,			NULL)
	PHP_FE(sybase_fetch_row,			NULL)
	PHP_FE(sybase_fetch_array,			NULL)
	PHP_FE(sybase_fetch_object,			NULL)
	PHP_FE(sybase_data_seek,			NULL)
	PHP_FE(sybase_fetch_field,			NULL)
	PHP_FE(sybase_field_seek,           NULL)
	PHP_FE(sybase_result,				NULL)
	PHP_FE(sybase_affected_rows,		NULL)
	PHP_FE(sybase_min_client_severity,	NULL)
	PHP_FE(sybase_min_server_severity,	NULL)

	PHP_FALIAS(mssql_connect,			sybase_connect,			NULL)
	PHP_FALIAS(mssql_pconnect,			sybase_pconnect,		NULL)
	PHP_FALIAS(mssql_close,				sybase_close,			NULL)
	PHP_FALIAS(mssql_select_db,			sybase_select_db,		NULL)
	PHP_FALIAS(mssql_query,				sybase_query,			NULL)
	PHP_FALIAS(mssql_free_result,		sybase_free_result,		NULL)
	PHP_FALIAS(mssql_get_last_message,	sybase_get_last_message,NULL)
	PHP_FALIAS(mssql_num_rows,			sybase_num_rows,		NULL)
	PHP_FALIAS(mssql_num_fields,		sybase_num_fields,		NULL)
	PHP_FALIAS(mssql_fetch_row,			sybase_fetch_row,		NULL)
	PHP_FALIAS(mssql_fetch_array,		sybase_fetch_array,		NULL)
	PHP_FALIAS(mssql_fetch_object,		sybase_fetch_object,	NULL)
	PHP_FALIAS(mssql_data_seek,			sybase_data_seek,		NULL)
	PHP_FALIAS(mssql_fetch_field,		sybase_fetch_field,		NULL)
	PHP_FALIAS(mssql_field_seek,		sybase_field_seek,		NULL)
	PHP_FALIAS(mssql_result,			sybase_result,			NULL)
	PHP_FALIAS(mssql_affected_rows,		sybase_affected_rows,	NULL)
	PHP_FALIAS(mssql_min_client_severity,	sybase_min_client_severity,	NULL)
	PHP_FALIAS(mssql_min_server_severity,	sybase_min_server_severity,	NULL)
	{NULL, NULL, NULL}
};

zend_module_entry sybase_module_entry = {
	"sybase_ct", sybase_functions, PHP_MINIT(sybase), PHP_MSHUTDOWN(sybase), PHP_RINIT(sybase), PHP_RSHUTDOWN(sybase), PHP_MINFO(sybase), STANDARD_MODULE_PROPERTIES
};

ZEND_DECLARE_MODULE_GLOBALS(sybase)
/* static CS_CONTEXT *context; */

#ifdef COMPILE_DL_SYBASE_CT
ZEND_GET_MODULE(sybase)
#endif

ZEND_DECLARE_MODULE_GLOBALS(sybase)

#define CHECK_LINK(link) { if (link==-1) { php_error(E_WARNING, "Sybase:  A link to the server could not be established"); RETURN_FALSE; } }


static int _clean_invalid_results(list_entry *le TSRMLS_DC)
{
	if (le->type == le_result) {
		sybase_link *sybase_ptr = ((sybase_result *) le->ptr)->sybase_ptr;
		
		if (!sybase_ptr->valid) {
			return 1;
		}
	}
	return 0;
}


static void _free_sybase_result(sybase_result *result)
{
	int i, j;

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

static void php_free_sybase_result(zend_rsrc_list_entry *rsrc)
{
	sybase_result *result = (sybase_result *)rsrc->ptr;
	_free_sybase_result(result);
}

static void _close_sybase_link(zend_rsrc_list_entry *rsrc)
{
	sybase_link *sybase_ptr = (sybase_link *)rsrc->ptr;
	CS_INT con_status;
	TSRMLS_FETCH();

	sybase_ptr->valid = 0;

	zend_hash_apply(&EG(regular_list), (apply_func_t) _clean_invalid_results TSRMLS_CC);

	/* Non-persistent connections will always be connected or we wouldn't
	 * get here, but since we want to check the death status anyway
	 * we might as well double-check the connect status.
	 */
	if (ct_con_props(sybase_ptr->connection, CS_GET, CS_CON_STATUS,
					 &con_status, CS_UNUSED, NULL)!=CS_SUCCEED) {
		php_error(E_WARNING, "Sybase:  Unable to get connection status on close");
		/* Assume the worst. */
		con_status = CS_CONSTAT_CONNECTED | CS_CONSTAT_DEAD;
	}
	if (con_status & CS_CONSTAT_CONNECTED) {
		if ((con_status & CS_CONSTAT_DEAD) || ct_close(sybase_ptr->connection, CS_UNUSED)!=CS_SUCCEED) {
			ct_close(sybase_ptr->connection, CS_FORCE_CLOSE);
		}
	}

	ct_con_drop(sybase_ptr->connection);
	efree(sybase_ptr);
	SybCtG(num_links)--;
}


static void _close_sybase_plink(zend_rsrc_list_entry *rsrc)
{
	sybase_link *sybase_ptr = (sybase_link *)rsrc->ptr;
	CS_INT con_status;
	TSRMLS_FETCH();

	/* Persistent connections may have been closed before a failed
	 * reopen attempt.
	 */
	if (ct_con_props(sybase_ptr->connection, CS_GET, CS_CON_STATUS,
					 &con_status, CS_UNUSED, NULL)!=CS_SUCCEED) {
		php_error(E_WARNING, "Sybase:  Unable to get connection status on close");
		/* Assume the worst. */
		con_status = CS_CONSTAT_CONNECTED | CS_CONSTAT_DEAD;
	}
	if (con_status & CS_CONSTAT_CONNECTED) {
		if ((con_status & CS_CONSTAT_DEAD) || ct_close(sybase_ptr->connection, CS_UNUSED)!=CS_SUCCEED) {
			ct_close(sybase_ptr->connection, CS_FORCE_CLOSE);
		}
	}

	ct_con_drop(sybase_ptr->connection);
	free(sybase_ptr);
	SybCtG(num_persistent)--;
	SybCtG(num_links)--;
}


static CS_RETCODE CS_PUBLIC _client_message_handler(CS_CONTEXT *context, CS_CONNECTION *connection, CS_CLIENTMSG *errmsg)
{
	TSRMLS_FETCH();

	if (CS_SEVERITY(errmsg->msgnumber) >= SybCtG(min_client_severity)) {
		php_error(E_WARNING, "Sybase:  Client message:  %s (severity %d)", errmsg->msgstring, CS_SEVERITY(errmsg->msgnumber));
	}
	STR_FREE(SybCtG(server_message));
	SybCtG(server_message) = estrdup(errmsg->msgstring);


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


static CS_RETCODE CS_PUBLIC _server_message_handler(CS_CONTEXT *context, CS_CONNECTION *connection, CS_SERVERMSG *srvmsg)
{
	TSRMLS_FETCH();

	if (srvmsg->severity >= SybCtG(min_server_severity)) {
		php_error(E_WARNING, "Sybase:  Server message:  %s (severity %d, procedure %s)",
					srvmsg->text, srvmsg->severity, ((srvmsg->proclen>0) ? srvmsg->proc : "N/A"));
	}
	STR_FREE(SybCtG(server_message));
	SybCtG(server_message) = estrdup(srvmsg->text);

	/* If this is a deadlock message, set the connection's deadlock flag
	 * so we will retry the request.  Sorry about the bare constant here,
	 * but it's not defined anywhere and it's a "well-known" number.
	 */
	if (srvmsg->msgnumber == 1205) {
		sybase_link *sybase;

		if (ct_con_props(connection, CS_GET, CS_USERDATA, &sybase, CS_SIZEOF(sybase), NULL)==CS_SUCCEED) {
		    sybase->deadlock = 1;
		}
		else {
			/* oh well */
		}
	}

    return CS_SUCCEED;
}


PHP_INI_BEGIN()
	STD_PHP_INI_BOOLEAN("sybct.allow_persistent",	"1",	PHP_INI_SYSTEM,		OnUpdateInt,		allow_persistent,	zend_sybase_globals,	sybase_globals)
	STD_PHP_INI_ENTRY_EX("sybct.max_persistent",	"-1",	PHP_INI_SYSTEM,		OnUpdateInt,		max_persistent,		zend_sybase_globals,	sybase_globals,	display_link_numbers)
	STD_PHP_INI_ENTRY_EX("sybct.max_links",		"-1",	PHP_INI_SYSTEM,		OnUpdateInt,		max_links,			zend_sybase_globals,	sybase_globals,	display_link_numbers)
	STD_PHP_INI_ENTRY("sybct.min_server_severity",	"10",	PHP_INI_ALL,		OnUpdateInt,		min_server_severity,	zend_sybase_globals,	sybase_globals)
	STD_PHP_INI_ENTRY("sybct.min_client_severity",	"10",	PHP_INI_ALL,		OnUpdateInt,		min_client_severity,	zend_sybase_globals,	sybase_globals)
	STD_PHP_INI_ENTRY("sybct.hostname",			NULL,	PHP_INI_ALL,		OnUpdateString,		hostname,		zend_sybase_globals,		sybase_globals)
PHP_INI_END()


static void php_sybase_init_globals(zend_sybase_globals *sybase_globals)
{
	long timeout;

	if (cs_ctx_alloc(CTLIB_VERSION, &sybase_globals->context)!=CS_SUCCEED || ct_init(sybase_globals->context,CTLIB_VERSION)!=CS_SUCCEED) {
		return;
	}

	/* Initialize message handlers */
	if (ct_callback(sybase_globals->context, NULL, CS_SET, CS_SERVERMSG_CB, (CS_VOID *)_server_message_handler)!=CS_SUCCEED) {
		php_error(E_WARNING, "Sybase:  Unable to set server message handler");
	}

	if (ct_callback(sybase_globals->context, NULL, CS_SET, CS_CLIENTMSG_CB, (CS_VOID *)_client_message_handler)!=CS_SUCCEED) {
		php_error(E_WARNING, "Sybase:  Unable to set client message handler");
	}
	
	/* Set datetime conversion format to "Nov  3 1998  8:06PM".
	 * This is the default format for the ct-lib that comes with
	 * Sybase ASE 11.5.1 for Solaris, but the Linux libraries that
	 * come with 11.0.3.3 default to "03/11/98" which is singularly
	 * useless.  This levels the playing field for all platforms.
	 */
	{
		CS_INT dt_convfmt = CS_DATES_SHORT;
		if (cs_dt_info(sybase_globals->context, CS_SET, NULL, CS_DT_CONVFMT, CS_UNUSED, &dt_convfmt, sizeof(dt_convfmt), NULL)!=CS_SUCCEED) {
			php_error(E_WARNING, "Sybase:  Unable to set datetime conversion format");
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
	if (cfg_get_long("sybct.login_timeout", &timeout)==SUCCESS) {
		CS_INT cs_login_timeout = timeout;
		if (ct_config(sybase_globals->context, CS_SET, CS_LOGIN_TIMEOUT, &cs_login_timeout, CS_UNUSED, NULL)!=CS_SUCCEED) {
			php_error(E_WARNING, "Sybase:  Unable to set login timeoutt");
		}
	}
	if (cfg_get_long("sybct.timeout", &timeout)==SUCCESS) {
		CS_INT cs_timeout = timeout;
		if (ct_config(sybase_globals->context, CS_SET, CS_TIMEOUT, &cs_timeout, CS_UNUSED, NULL)!=CS_SUCCEED) {
			php_error(E_WARNING, "Sybase:  Unable to set timeout");
		}
	}
	sybase_globals->num_persistent=0;
}


static void php_sybase_destroy_globals(zend_sybase_globals *sybase_globals)
{
    ct_exit(sybase_globals->context, CS_UNUSED);
    cs_ctx_drop(sybase_globals->context);
}

PHP_MINIT_FUNCTION(sybase)
{
	ZEND_INIT_MODULE_GLOBALS(sybase, php_sybase_init_globals, php_sybase_destroy_globals);

	REGISTER_INI_ENTRIES();

	le_link = zend_register_list_destructors_ex(_close_sybase_link, NULL, "sybase-ct link", module_number);
	le_plink = zend_register_list_destructors_ex(NULL, _close_sybase_plink, "sybase-ct link persistent", module_number);
	le_result = zend_register_list_destructors_ex(php_free_sybase_result, NULL, "sybase-ct result", module_number);

	return SUCCESS;
}



PHP_RINIT_FUNCTION(sybase)
{
	SybCtG(default_link)=-1;
	SybCtG(num_links) = SybCtG(num_persistent);
	SybCtG(appname) = estrndup("PHP 4.0", 7);
	SybCtG(server_message) = empty_string;
	return SUCCESS;
}



PHP_MSHUTDOWN_FUNCTION(sybase)
{
	UNREGISTER_INI_ENTRIES();
#if 0
	ct_exit(context,CS_UNUSED);
	cs_ctx_drop(context);
#endif
	return SUCCESS;
}


PHP_RSHUTDOWN_FUNCTION(sybase)
{
	efree(SybCtG(appname));
	STR_FREE(SybCtG(server_message));
	return SUCCESS;
}


static int php_sybase_do_connect_internal(sybase_link *sybase, char *host, char *user, char *passwd, char *charset)
{
	CS_LOCALE *tmp_locale;
	TSRMLS_FETCH();

	/* set a CS_CONNECTION record */
	if (ct_con_alloc(SybCtG(context), &sybase->connection)!=CS_SUCCEED) {
		php_error(E_WARNING, "Sybase:  Unable to allocate connection record");
		return 0;
	}
	
	/* Note - this saves a copy of sybase, not a pointer to it. */
	if (ct_con_props(sybase->connection, CS_SET, CS_USERDATA, &sybase, CS_SIZEOF(sybase), NULL)!=CS_SUCCEED) {
		php_error(E_WARNING, "Sybase:  Unable to set userdata");
		ct_con_drop(sybase->connection);
		return 0;
	}

	if (user) {
		ct_con_props(sybase->connection, CS_SET, CS_USERNAME, user, CS_NULLTERM, NULL);
	}
	if (passwd) {
		ct_con_props(sybase->connection, CS_SET, CS_PASSWORD, passwd, CS_NULLTERM, NULL);
	}
	ct_con_props(sybase->connection, CS_SET, CS_APPNAME, SybCtG(appname), CS_NULLTERM, NULL);

	if (SybCtG(hostname)) {
		ct_con_props(sybase->connection, CS_SET, CS_HOSTNAME, SybCtG(hostname), CS_NULLTERM, NULL);
	}

	if (charset) {
		if (cs_loc_alloc(SybCtG(context), &tmp_locale)!=CS_SUCCEED) {
			php_error(E_WARNING,"Sybase: Unable to allocate locale information.");
		} else {
			if (cs_locale(SybCtG(context), CS_SET, tmp_locale, CS_LC_ALL, NULL, CS_NULLTERM, NULL)!=CS_SUCCEED) {
				php_error(E_WARNING,"Sybase: Unable to load default locale data.");
			} else {
				if (cs_locale(SybCtG(context), CS_SET, tmp_locale, CS_SYB_CHARSET, charset, CS_NULLTERM, NULL)!=CS_SUCCEED) {
					php_error(E_WARNING,"Sybase: Unable to update character set.");
				} else {
					if (ct_con_props(sybase->connection, CS_SET, CS_LOC_PROP, tmp_locale, CS_UNUSED, NULL)!=CS_SUCCEED) {
						php_error(E_WARNING,"Sybase: Unable to update connection properties.");
					}
				}
			}
		}
	}

	sybase->valid = 1;
	sybase->dead = 0;

	/* create the link */
	if (ct_connect(sybase->connection, host, CS_NULLTERM)!=CS_SUCCEED) {
		php_error(E_WARNING, "Sybase:  Unable to connect");
		ct_con_drop(sybase->connection);
		return 0;
	}

	if (ct_cmd_alloc(sybase->connection, &sybase->cmd)!=CS_SUCCEED) {
		php_error(E_WARNING, "Sybase:  Unable to allocate command record");
		ct_close(sybase->connection,CS_UNUSED);
		ct_con_drop(sybase->connection);
		return 0;
	}

	return 1;
}


static void php_sybase_do_connect(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	char *user,*passwd,*host,*charset;
	char *hashed_details;
	int hashed_details_length;
	sybase_link *sybase_ptr;

	switch(ZEND_NUM_ARGS()) {
		case 0: /* defaults */
			host=user=passwd=charset=NULL;
			hashed_details_length=6+4;
			hashed_details = (char *) emalloc(hashed_details_length+1);
			strcpy(hashed_details, "sybase____");
			break;
		case 1: {
				pval *yyhost;
				
				if (getParameters(ht, 1, &yyhost)==FAILURE) {
					RETURN_FALSE;
				}
				convert_to_string(yyhost);
				host = yyhost->value.str.val;
				user=passwd=charset=NULL;
				hashed_details_length = yyhost->value.str.len+6+4;
				hashed_details = (char *) emalloc(hashed_details_length+1);
				sprintf(hashed_details, "sybase_%s___", yyhost->value.str.val);
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
				passwd=charset=NULL;
				hashed_details_length = yyhost->value.str.len+yyuser->value.str.len+6+4;
				hashed_details = (char *) emalloc(hashed_details_length+1);
				sprintf(hashed_details, "sybase_%s_%s__", yyhost->value.str.val, yyuser->value.str.val);
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
				charset=NULL;
				hashed_details_length = yyhost->value.str.len+yyuser->value.str.len+yypasswd->value.str.len+6+4;
				hashed_details = (char *) emalloc(hashed_details_length+1);
				sprintf(hashed_details, "sybase_%s_%s_%s_", yyhost->value.str.val, yyuser->value.str.val, yypasswd->value.str.val);
			}
			break;
		case 4: {
				pval *yyhost,*yyuser,*yypasswd,*yycharset;

				if (getParameters(ht, 4, &yyhost, &yyuser, &yypasswd, &yycharset) == FAILURE) {
					RETURN_FALSE;
				}
				convert_to_string(yyhost);
				convert_to_string(yyuser);
				convert_to_string(yypasswd);
				convert_to_string(yycharset);
				host = Z_STRVAL_P(yyhost);
				user = Z_STRVAL_P(yyuser);
				passwd = Z_STRVAL_P(yypasswd);
				charset = Z_STRVAL_P(yycharset);
				hashed_details_length = Z_STRLEN_P(yyhost)+Z_STRLEN_P(yyuser)+Z_STRLEN_P(yypasswd)+Z_STRLEN_P(yycharset)+6+4;
				hashed_details = (char *) emalloc(hashed_details_length+1);
				sprintf(hashed_details, "sybase_%s_%s_%s_%s", Z_STRVAL_P(yyhost), Z_STRVAL_P(yyuser), Z_STRVAL_P(yypasswd), Z_STRVAL_P(yycharset));
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}


	if (!SybCtG(allow_persistent)) {
		persistent=0;
	}
	if (persistent) {
		list_entry *le;

		/* try to find if we already have this link in our persistent list */
		if (zend_hash_find(&EG(persistent_list), hashed_details, hashed_details_length+1, (void **) &le)==FAILURE) {  /* we don't */
			list_entry new_le;

			if (SybCtG(max_links)!=-1 && SybCtG(num_links)>=SybCtG(max_links)) {
				php_error(E_WARNING, "Sybase:  Too many open links (%d)",SybCtG(num_links));
				efree(hashed_details);
				RETURN_FALSE;
			}
			if (SybCtG(max_persistent)!=-1 && SybCtG(num_persistent)>=SybCtG(max_persistent)) {
				php_error(E_WARNING, "Sybase:  Too many open persistent links (%d)",SybCtG(num_persistent));
				efree(hashed_details);
				RETURN_FALSE;
			}

			sybase_ptr = (sybase_link *) malloc(sizeof(sybase_link));
			if (!php_sybase_do_connect_internal(sybase_ptr, host, user, passwd, charset)) {
				free(sybase_ptr);
				efree(hashed_details);
				RETURN_FALSE;
			}

			/* hash it up */
			new_le.type = le_plink;
			new_le.ptr = sybase_ptr;
			if (zend_hash_update(&EG(persistent_list), hashed_details, hashed_details_length+1, (void *) &new_le, sizeof(list_entry),NULL)==FAILURE) {
				ct_close(sybase_ptr->connection, CS_UNUSED);
				ct_con_drop(sybase_ptr->connection);
				free(sybase_ptr);
				efree(hashed_details);
				RETURN_FALSE;
			}
			SybCtG(num_persistent)++;
			SybCtG(num_links)++;
		} else {  /* we do */
			CS_INT con_status;

			if (le->type != le_plink) {
				efree(hashed_details);
				RETURN_FALSE;
			}
			
			sybase_ptr = (sybase_link *) le->ptr;

			/* If the link has died, close it and overwrite it with a new one. */

			if (ct_con_props(sybase_ptr->connection, CS_GET, CS_CON_STATUS,
							 &con_status, CS_UNUSED, NULL)!=CS_SUCCEED) {
				php_error(E_WARNING, "Sybase:  Unable to get connection status");
				efree(hashed_details);
				RETURN_FALSE;
			}
			if (!(con_status & CS_CONSTAT_CONNECTED) || (con_status & CS_CONSTAT_DEAD) || sybase_ptr->dead) {
				sybase_link sybase;

				if (con_status & CS_CONSTAT_CONNECTED) {
					ct_close(sybase_ptr->connection, CS_FORCE_CLOSE);
				}
				/* Create a new connection, then replace the old
				 * connection.  If we fail to create a new connection,
				 * put the old one back so there will be a connection,
				 * even if it is a non-functional one.  This is because
				 * code may still be holding an id for this connection
				 * so we can't free the CS_CONNECTION.
				 * (This is actually totally hokey, it would be better
				 * to just ct_con_drop() the connection and set
				 * sybase_ptr->connection to NULL, then test it for
				 * NULL before trying to use it elsewhere . . .)
				 */
				memcpy(&sybase, sybase_ptr, sizeof(sybase_link));
				if (!php_sybase_do_connect_internal(sybase_ptr, host, user, passwd, charset)) {
					memcpy(sybase_ptr, &sybase, sizeof(sybase_link));
					efree(hashed_details);
					RETURN_FALSE;
				}
				ct_con_drop(sybase.connection); /* drop old connection */
			}
		}
		ZEND_REGISTER_RESOURCE(return_value, sybase_ptr, le_plink);
	} else { /* non persistent */
		list_entry *index_ptr, new_index_ptr;
		
		/* first we check the hash for the hashed_details key.  if it exists,
		 * it should point us to the right offset where the actual sybase link sits.
		 * if it doesn't, open a new sybase link, add it to the resource list,
		 * and add a pointer to it with hashed_details as the key.
		 */
		if (zend_hash_find(&EG(regular_list), hashed_details, hashed_details_length+1,(void **) &index_ptr)==SUCCESS) {
			int type, link;
			void *ptr;

			if (index_ptr->type != le_index_ptr) {
				efree(hashed_details);
				RETURN_FALSE;
			}
			link = (int) index_ptr->ptr;
			ptr = zend_list_find(link, &type);   /* check if the link is still there */
			if (ptr && (type==le_link || type==le_plink)) {
				zend_list_addref(link);
				return_value->value.lval = SybCtG(default_link) = link;
				return_value->type = IS_RESOURCE;
				efree(hashed_details);
				return;
			} else {
				zend_hash_del(&EG(regular_list), hashed_details, hashed_details_length+1);
			}
		}
		if (SybCtG(max_links)!=-1 && SybCtG(num_links)>=SybCtG(max_links)) {
			php_error(E_WARNING, "Sybase:  Too many open links (%d)",SybCtG(num_links));
			efree(hashed_details);
			RETURN_FALSE;
		}

		sybase_ptr = (sybase_link *) emalloc(sizeof(sybase_link));
		if (!php_sybase_do_connect_internal(sybase_ptr, host, user, passwd, charset)) {
			efree(sybase_ptr);
			efree(hashed_details);
			RETURN_FALSE;
		}

		/* add it to the list */
		ZEND_REGISTER_RESOURCE(return_value, sybase_ptr, le_link);
		
		/* add it to the hash */
		new_index_ptr.ptr = (void *) return_value->value.lval;
		new_index_ptr.type = le_index_ptr;
		if (zend_hash_update(&EG(regular_list), hashed_details, hashed_details_length+1,(void *) &new_index_ptr, sizeof(list_entry),NULL)==FAILURE) {
			ct_close(sybase_ptr->connection, CS_UNUSED);
			ct_con_drop(sybase_ptr->connection);
			efree(sybase_ptr);
			efree(hashed_details);
			RETURN_FALSE;
		}
		SybCtG(num_links)++;
	}
	efree(hashed_details);
	SybCtG(default_link)=return_value->value.lval;
	zend_list_addref(SybCtG(default_link));
}


static int php_sybase_get_default_link(INTERNAL_FUNCTION_PARAMETERS)
{
	if (SybCtG(default_link)==-1) { /* no link opened yet, implicitly open one */
		ht = 0;
		php_sybase_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
	}
	return SybCtG(default_link);
}


/* {{{ proto int sybase_connect([string host [, string user [, string password [, string charset]]]])
   Open Sybase server connection */
PHP_FUNCTION(sybase_connect)
{
	php_sybase_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}

/* }}} */

/* {{{ proto int sybase_pconnect([string host [, string user [, string password [, string charset]]]])
   Open persistent Sybase connection */
PHP_FUNCTION(sybase_pconnect)
{
	php_sybase_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}

/* }}} */


/* {{{ proto bool sybase_close([int link_id])
   Close Sybase connection */
PHP_FUNCTION(sybase_close)
{
	pval *sybase_link_index = 0;
	int id;
	sybase_link *sybase_ptr;
	
	switch (ZEND_NUM_ARGS()) {
		case 0:
			id = SybCtG(default_link);
			break;
		case 1:
			if (getParameters(ht, 1, &sybase_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE2(sybase_ptr, sybase_link *, &sybase_link_index, id, "Sybase-Link", le_link, le_plink);
	
	if (id==-1) { /* explicit resource number */
		zend_list_delete(Z_RESVAL_P(sybase_link_index));
	}

	if (id!=-1 
		|| (sybase_link_index && Z_RESVAL_P(sybase_link_index)==SybCtG(default_link))) {
		zend_list_delete(SybCtG(default_link));
		SybCtG(default_link) = -1;
	}

	RETURN_TRUE;
}

/* }}} */
	

static int exec_cmd(sybase_link *sybase_ptr, char *cmdbuf)
{
	CS_RETCODE retcode;
	CS_INT restype;
	int failure=0;
	TSRMLS_FETCH();

	/* Fail if we already marked this connection dead. */

	if (sybase_ptr->dead) {
		return FAILURE;
	}

	/*
	 ** Get a command handle, store the command string in it, and
	 ** send it to the server.
	 */
	
	if (ct_command(sybase_ptr->cmd, CS_LANG_CMD, cmdbuf, CS_NULLTERM, CS_UNUSED)!=CS_SUCCEED) {
		sybase_ptr->dead = 1;
		return FAILURE;
	}
	if (ct_send(sybase_ptr->cmd)!=CS_SUCCEED) {
		sybase_ptr->dead = 1;
		return FAILURE;
	}
	
	while ((retcode = ct_results(sybase_ptr->cmd, &restype))==CS_SUCCEED) {
		switch ((int) restype) {
			case CS_CMD_SUCCEED:
			case CS_CMD_DONE:
				break;
		
			case CS_CMD_FAIL:
				failure=1;
				break;
			
			case CS_STATUS_RESULT:
				ct_cancel(NULL, sybase_ptr->cmd, CS_CANCEL_CURRENT);
				break;
			
			default:
				failure=1;
				break;
		}
		if (failure) {
			ct_cancel(NULL, sybase_ptr->cmd, CS_CANCEL_ALL);
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
			ct_cancel(NULL, sybase_ptr->cmd, CS_CANCEL_ALL);
			/* Don't take chances with the vagaries of ct-lib.  Mark it
			 * dead ourselves.
			 */
			sybase_ptr->dead = 1;
			return FAILURE;

		default:
			return FAILURE;
	}
}


/* {{{ proto bool sybase_select_db(string database [, int link_id])
   Select Sybase database */
PHP_FUNCTION(sybase_select_db)
{
	pval *db,*sybase_link_index;
	int id;
	char *cmdbuf;
	sybase_link  *sybase_ptr;
	
	switch(ZEND_NUM_ARGS()) {
		case 1:
			if (getParameters(ht, 1, &db)==FAILURE) {
				RETURN_FALSE;
			}
			id = php_sybase_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			break;
		case 2:
			if (getParameters(ht, 2, &db, &sybase_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE2(sybase_ptr, sybase_link *, &sybase_link_index, id, "Sybase-Link", le_link, le_plink);
	
	convert_to_string(db);
	cmdbuf = (char *) emalloc(sizeof("use ")+db->value.str.len+1);
	sprintf(cmdbuf, "use %s", db->value.str.val); /* SAFE */

	if (exec_cmd(sybase_ptr, cmdbuf)==FAILURE) {
		efree(cmdbuf);
		RETURN_FALSE;
	} else {
		efree(cmdbuf);
		RETURN_TRUE;
	}
}

/* }}} */


static sybase_result * php_sybase_fetch_result_set (sybase_link *sybase_ptr)
{
	int num_fields;
	sybase_result *result;
	char **tmp_buffer;
	CS_INT *lengths;
	CS_SMALLINT *indicators;
	unsigned char *numerics;
	CS_INT *types;
	CS_DATAFMT *datafmt;
	int i, j, retcode;
	int blocks_initialized=1;
	TSRMLS_FETCH();

	/* The following is more or less the equivalent of mysql_store_result().
	 * fetch all rows from the server into the row buffer, thus:
	 * 1)  Being able to fire up another query without explicitly reading all rows
	 * 2)  Having numrows accessible
	 */

	if (ct_res_info(sybase_ptr->cmd, CS_NUMDATA, &num_fields, CS_UNUSED, NULL)!=CS_SUCCEED) {
		return NULL;
	}
	
	result = (sybase_result *) emalloc(sizeof(sybase_result));
	result->data = (pval **) emalloc(sizeof(pval *)*SYBASE_ROWS_BLOCK);
	result->fields = NULL;
	result->sybase_ptr = sybase_ptr;
	result->cur_field=result->cur_row=result->num_rows=0;
	result->num_fields = num_fields;

	tmp_buffer = (char **) emalloc(sizeof(char *)*num_fields);
	lengths = (CS_INT *) emalloc(sizeof(CS_INT)*num_fields);
	indicators = (CS_SMALLINT *) emalloc(sizeof(CS_INT)*num_fields);
	datafmt = (CS_DATAFMT *) emalloc(sizeof(CS_DATAFMT)*num_fields);
	numerics = (unsigned char *) emalloc(sizeof(unsigned char)*num_fields);
	types = (CS_INT *) emalloc(sizeof(CS_INT)*num_fields);
	

	for (i=0; i<num_fields; i++) {
		ct_describe(sybase_ptr->cmd, i+1, &datafmt[i]);
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
		ct_bind(sybase_ptr->cmd, i+1, &datafmt[i], tmp_buffer[i], &lengths[i], &indicators[i]);
	}
	
	while ((retcode=ct_fetch(sybase_ptr->cmd,CS_UNUSED,CS_UNUSED,CS_UNUSED,NULL))==CS_SUCCEED
			|| retcode==CS_ROW_FAIL) {
		/*
		if (retcode==CS_ROW_FAIL) {
			php_error(E_WARNING, "Sybase:  Error reading row %d", result->num_rows);
		}
		*/
		i = result->num_rows++;
		if (result->num_rows > blocks_initialized*SYBASE_ROWS_BLOCK) {
			result->data = (pval **) erealloc(result->data, sizeof(pval *)*SYBASE_ROWS_BLOCK*(++blocks_initialized));
		}
		result->data[i] = (pval *) emalloc(sizeof(pval)*num_fields);
		for (j=0; j<num_fields; j++) {
			if (indicators[j] == -1) { /* null value */
				ZVAL_FALSE(&result->data[i][j]);
			} else {
				result->data[i][j].value.str.len = lengths[j]-1;  /* we don't need the NULL in the length */
				result->data[i][j].value.str.val = estrndup(tmp_buffer[j], lengths[j]);
				result->data[i][j].type = IS_STRING;
			}
		}
	}

	if (retcode != CS_END_DATA) {
		_free_sybase_result(result);
		result = NULL;
	} else {
		result->fields = (sybase_field *) emalloc(sizeof(sybase_field)*num_fields);
		j=0;
		for (i=0; i<num_fields; i++) {
			char computed_buf[16];
		
			if (datafmt[i].namelen>0) {
				result->fields[i].name = estrndup(datafmt[i].name, datafmt[i].namelen);
			} else {
				if (j>0) {
					snprintf(computed_buf, 16, "computed%d", j);
				} else {
					strcpy(computed_buf, "computed");
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


/* {{{ proto int sybase_query(string query [, int link_id])
   Send Sybase query */
PHP_FUNCTION(sybase_query)
{
	pval **query, **sybase_link_index=NULL;
	int id;
	sybase_link *sybase_ptr;
	sybase_result *result;
	CS_INT restype;
	CS_RETCODE retcode;
	enum {
		Q_RESULT,				/* Success with results. */
		Q_SUCCESS,				/* Success but no results. */
		Q_FAILURE,				/* Failure, no results. */
	} status;
	
	switch(ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &query)==FAILURE) {
				RETURN_FALSE;
			}
			id = SybCtG(default_link);
			break;
		case 2:
			if (zend_get_parameters_ex(2, &query, &sybase_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE2(sybase_ptr, sybase_link *, sybase_link_index, id, "Sybase-Link", le_link, le_plink);
	
	convert_to_string_ex(query);
	
	/* Fail if we already marked this connection dead. */

	if (sybase_ptr->dead) {
		RETURN_FALSE;
	}

	/* Repeat until we don't deadlock. */

	for (;;) {
		result = NULL;
		sybase_ptr->deadlock = 0;
		sybase_ptr->affected_rows = 0;

		/* On Solaris 11.5, ct_command() can be moved outside the
		 * loop, but not on Linux 11.0.
		 */
		if (ct_command(sybase_ptr->cmd, CS_LANG_CMD, (*query)->value.str.val, CS_NULLTERM, CS_UNUSED)!=CS_SUCCEED) {
			/* If this didn't work, the connection is screwed but
			 * ct-lib might not set CS_CONSTAT_DEAD.  So set our own
			 * flag.  This happens sometimes when the database is restarted
			 * and/or its machine is rebooted, and ct_command() returns
			 * CS_BUSY for some reason.
			 */
			sybase_ptr->dead = 1;
		    RETURN_FALSE;
		}

		if (ct_send(sybase_ptr->cmd)!=CS_SUCCEED) {
			ct_cancel(NULL, sybase_ptr->cmd, CS_CANCEL_ALL);
			sybase_ptr->dead = 1;
			RETURN_FALSE;
		}

		/* Use the first result set or succeed/fail status and discard the
		 * others.  Applications really shouldn't be making calls that
		 * return multiple result sets, but if they do then we need to
		 * properly read or cancel them or the connection will become
		 * unusable.
		 */
		if (ct_results(sybase_ptr->cmd, &restype)!=CS_SUCCEED) {
			ct_cancel(NULL, sybase_ptr->cmd, CS_CANCEL_ALL);
			sybase_ptr->dead = 1;
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
					if (ct_res_info(sybase_ptr->cmd, CS_ROW_COUNT, &row_count, CS_UNUSED, NULL)==CS_SUCCEED) {
						sybase_ptr->affected_rows = (long)row_count;
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
				result = php_sybase_fetch_result_set(sybase_ptr);
				if (result == NULL) {
					ct_cancel(NULL, sybase_ptr->cmd, CS_CANCEL_ALL);
					sybase_ptr->dead = 1;
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
		while ((retcode = ct_results(sybase_ptr->cmd, &restype))==CS_SUCCEED) {
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
					ct_cancel(NULL, sybase_ptr->cmd, CS_CANCEL_CURRENT);
					break;
			
				default:
					status = Q_FAILURE;
					break;
			}
			if (status == Q_FAILURE) {
				ct_cancel(NULL, sybase_ptr->cmd, CS_CANCEL_ALL);
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
				ct_cancel(NULL, sybase_ptr->cmd, CS_CANCEL_ALL);
				/* Don't take chances with the vagaries of ct-lib.  Mark it
				 * dead ourselves.
				 */
				sybase_ptr->dead = 1;
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
		if (sybase_ptr->dead || sybase_ptr->deadlock == 0) {
			break;
		}

		/* Get rid of any results we may have fetched.  This happens:
		 * e.g., our result set may be a stored procedure status which
		 * is returned even if the stored procedure deadlocks.  As an
		 * optimization, we could try not to fetch results in known
		 * deadlock conditions, but deadlock is (should be) rare.
		 */
		if (result != NULL) {
			_free_sybase_result(result);
		}
	}

	if (status == Q_SUCCESS) {
		RETURN_TRUE;
	}

	if (status == Q_FAILURE) {
		if (result != NULL) {
			_free_sybase_result(result);
		}
		RETURN_FALSE;
	}

	ZEND_REGISTER_RESOURCE(return_value, result, le_result);
}

/* }}} */

/* {{{ proto bool sybase_free_result(int result)
   Free result memory */
PHP_FUNCTION(sybase_free_result)
{
	pval *sybase_result_index;
	sybase_result *result;
	
	if (ZEND_NUM_ARGS()!=1 || getParameters(ht, 1, &sybase_result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	if (sybase_result_index->type==IS_RESOURCE
		&& sybase_result_index->value.lval==0) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(result, sybase_result *, &sybase_result_index, -1, "Sybase result", le_result);
	
	zend_list_delete(sybase_result_index->value.lval);
	RETURN_TRUE;
}

/* }}} */

/* {{{ proto string sybase_get_last_message(void)
   Returns the last message from server (over min_message_severity) */
PHP_FUNCTION(sybase_get_last_message)
{	
	RETURN_STRING(SybCtG(server_message),1);
}
/* }}} */

/* {{{ proto int sybase_num_rows(int result)
   Get number of rows in result */
PHP_FUNCTION(sybase_num_rows)
{
	pval *sybase_result_index;
	sybase_result *result;
	
	if (ZEND_NUM_ARGS()!=1 || getParameters(ht, 1, &sybase_result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(result, sybase_result *, &sybase_result_index, -1, "Sybase result", le_result);
	
	return_value->value.lval = result->num_rows;
	return_value->type = IS_LONG;
}

/* }}} */

/* {{{ proto int sybase_num_fields(int result)
   Get number of fields in result */
PHP_FUNCTION(sybase_num_fields)
{
	pval *sybase_result_index;
	sybase_result *result;
	TSRMLS_FETCH();
	
	if (ZEND_NUM_ARGS()!=1 || getParameters(ht, 1, &sybase_result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(result, sybase_result *, &sybase_result_index, -1, "Sybase result", le_result);
	
	return_value->value.lval = result->num_fields;
	return_value->type = IS_LONG;
}

/* }}} */

/* {{{ proto array sybase_fetch_row(int result)
   Get row as enumerated array */
PHP_FUNCTION(sybase_fetch_row)
{
	pval *sybase_result_index;
	int i;
	sybase_result *result;
	pval *field_content;
	TSRMLS_FETCH();

	if (ZEND_NUM_ARGS()!=1 || getParameters(ht, 1, &sybase_result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(result, sybase_result *, &sybase_result_index, -1, "Sybase result", le_result);
	
	if (result->cur_row >= result->num_rows) {
		RETURN_FALSE;
	}
	
	array_init(return_value);
	for (i=0; i<result->num_fields; i++) {
		ALLOC_ZVAL(field_content);
		*field_content = result->data[result->cur_row][i];
		INIT_PZVAL(field_content);
		pval_copy_constructor(field_content);
		zend_hash_index_update(return_value->value.ht, i, (void *) &field_content, sizeof(pval* ), NULL);
	}
	result->cur_row++;
}

/* }}} */

static void php_sybase_fetch_hash(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *sybase_result_index;
	sybase_result *result;
	int i;
	pval *tmp;
	TSRMLS_FETCH();
	TSRMLS_FETCH();
	
	if (ZEND_NUM_ARGS()!=1 || getParameters(ht, 1, &sybase_result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(result, sybase_result *, &sybase_result_index, -1, "Sybase result", le_result);
	
	if (result->cur_row >= result->num_rows) {
		RETURN_FALSE;
	}
	
	if (array_init(return_value)==FAILURE) {
		RETURN_FALSE;
	}
	
	for (i=0; i<result->num_fields; i++) {
		ALLOC_ZVAL(tmp);
		*tmp = result->data[result->cur_row][i];
		INIT_PZVAL(tmp);
		if (PG(magic_quotes_runtime) && tmp->type == IS_STRING) {
			tmp->value.str.val = php_addslashes(tmp->value.str.val, tmp->value.str.len, &tmp->value.str.len, 0);
		} else {
			pval_copy_constructor(tmp);
		}
		zend_hash_index_update(return_value->value.ht, i, (void *) &tmp, sizeof(pval *), NULL);
		tmp->refcount++;
		zend_hash_update(return_value->value.ht, result->fields[i].name, strlen(result->fields[i].name)+1, (void *) &tmp, sizeof(pval *), NULL);
	}
	result->cur_row++;
}


/* {{{ proto object sybase_fetch_object(int result)
   Fetch row as object */
PHP_FUNCTION(sybase_fetch_object)
{
	php_sybase_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	if (return_value->type==IS_ARRAY) {
		return_value->type=IS_OBJECT;
		return_value->value.obj.properties = return_value->value.ht;
		return_value->value.obj.ce = &zend_standard_class_def;
	}
}
/* }}} */

/* {{{ proto array sybase_fetch_array(int result)
   Fetch row as array */
PHP_FUNCTION(sybase_fetch_array)
{
	php_sybase_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto bool sybase_data_seek(int result, int offset)
   Move internal row pointer */
PHP_FUNCTION(sybase_data_seek)
{
	pval *sybase_result_index,*offset;
	sybase_result *result;
	TSRMLS_FETCH();

	if (ZEND_NUM_ARGS()!=2 || getParameters(ht, 2, &sybase_result_index, &offset)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	

	ZEND_FETCH_RESOURCE(result, sybase_result *, &sybase_result_index, -1, "Sybase result", le_result);

	convert_to_long(offset);
	if (offset->value.lval<0 || offset->value.lval>=result->num_rows) {
		php_error(E_WARNING, "Sybase:  Bad row offset");
		RETURN_FALSE;
	}
	
	result->cur_row = offset->value.lval;
	RETURN_TRUE;
}
/* }}} */

static char *php_sybase_get_field_name(CS_INT type)
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


/* {{{ proto object sybase_fetch_field(int result [, int offset])
   Get field information */
PHP_FUNCTION(sybase_fetch_field)
{
	pval *sybase_result_index,*offset;
	int field_offset;
	sybase_result *result;
	TSRMLS_FETCH();

	switch (ZEND_NUM_ARGS()) {
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
	
	ZEND_FETCH_RESOURCE(result, sybase_result *, &sybase_result_index, -1, "Sybase result", le_result);
	
	if (field_offset==-1) {
		field_offset = result->cur_field;
		result->cur_field++;
	}
	
	if (field_offset<0 || field_offset >= result->num_fields) {
		if (ZEND_NUM_ARGS()==2) { /* field specified explicitly */
			php_error(E_WARNING, "Sybase:  Bad column offset");
		}
		RETURN_FALSE;
	}

	if (object_init(return_value)==FAILURE) {
		RETURN_FALSE;
	}
	add_property_string(return_value, "name", result->fields[field_offset].name, 1);
	add_property_long(return_value, "max_length", result->fields[field_offset].max_length);
	add_property_string(return_value, "column_source", result->fields[field_offset].column_source, 1);
	add_property_long(return_value, "numeric", result->fields[field_offset].numeric);
	add_property_string(return_value, "type", php_sybase_get_field_name(result->fields[field_offset].type), 1);
}
/* }}} */


/* {{{ proto bool sybase_field_seek(int result, int offset)
   Set field offset */
PHP_FUNCTION(sybase_field_seek)
{
	pval *sybase_result_index,*offset;
	int field_offset;
	sybase_result *result;
	TSRMLS_FETCH();

	if (ZEND_NUM_ARGS()!=2 || getParameters(ht, 2, &sybase_result_index, &offset)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(result, sybase_result *, &sybase_result_index, -1, "Sybase result", le_result);
	
	convert_to_long(offset);
	field_offset = offset->value.lval;
	
	if (field_offset<0 || field_offset >= result->num_fields) {
		php_error(E_WARNING, "Sybase:  Bad column offset");
		RETURN_FALSE;
	}

	result->cur_field = field_offset;
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto string sybase_result(int result, int row, mixed field)
   Get result data */
PHP_FUNCTION(sybase_result)
{
	pval *row, *field, *sybase_result_index;
	int field_offset=0;
	sybase_result *result;
	TSRMLS_FETCH();

	if (ZEND_NUM_ARGS()!=3 || getParameters(ht, 3, &sybase_result_index, &row, &field)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	
	ZEND_FETCH_RESOURCE(result, sybase_result *, &sybase_result_index, -1, "Sybase result", le_result);

	convert_to_long(row);
	if (row->value.lval<0 || row->value.lval>=result->num_rows) {
		php_error(E_WARNING, "Sybase:  Bad row offset (%d)", row->value.lval);
		RETURN_FALSE;
	}

	switch(field->type) {
		case IS_STRING: {
			int i;

			for (i=0; i<result->num_fields; i++) {
				if (!strcasecmp(result->fields[i].name, field->value.str.val)) {
					field_offset = i;
					break;
				}
			}
			if (i>=result->num_fields) { /* no match found */
				php_error(E_WARNING, "Sybase:  %s field not found in result", field->value.str.val);
				RETURN_FALSE;
			}
			break;
		}
		default:
			convert_to_long(field);
			field_offset = field->value.lval;
			if (field_offset<0 || field_offset>=result->num_fields) {
				php_error(E_WARNING, "Sybase:  Bad column offset specified");
				RETURN_FALSE;
			}
			break;
	}

	*return_value = result->data[row->value.lval][field_offset];
	pval_copy_constructor(return_value);
}
/* }}} */


/* {{{ proto int sybase_affected_rows([int link_id])
   Get number of affected rows in last query */
PHP_FUNCTION(sybase_affected_rows)
{
	pval *sybase_link_index;
	sybase_link *sybase_ptr;
	int id;
	TSRMLS_FETCH();
	
	switch(ZEND_NUM_ARGS()) {
		case 0:
			id = php_sybase_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			break;
		case 1:
			if (getParameters(ht, 1, &sybase_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE2(sybase_ptr, sybase_link *, &sybase_link_index, id, "Sybase-Link", le_link, le_plink);

	return_value->value.lval = sybase_ptr->affected_rows;
	return_value->type = IS_LONG;
}
/* }}} */


PHP_MINFO_FUNCTION(sybase)
{
	char buf[32];
	
	php_info_print_table_start();
	php_info_print_table_header(2, "Sybase_CT Support", "enabled" );
	sprintf(buf, "%ld", SybCtG(num_persistent));
	php_info_print_table_row(2, "Active Persistent Links", buf);
	sprintf(buf, "%ld", SybCtG(num_links));
	php_info_print_table_row(2, "Active Links", buf);
	php_info_print_table_row(2, "Application Name", SybCtG(appname));
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}


/* {{{ proto void sybase_min_client_severity(int severity)
   Sets minimum client severity */
PHP_FUNCTION(sybase_min_client_severity)
{
	pval *severity;
	
	if (ZEND_NUM_ARGS()!=1 || getParameters(ht, 1, &severity)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(severity);
	SybCtG(min_client_severity) = severity->value.lval;
}
/* }}} */


/* {{{ proto void sybase_min_server_severity(int severity)
   Sets minimum server severity */
PHP_FUNCTION(sybase_min_server_severity)
{
	pval *severity;
	
	if (ZEND_NUM_ARGS()!=1 || getParameters(ht, 1, &severity)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(severity);
	SybCtG(min_server_severity) = severity->value.lval;
}
/* }}} */


#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
