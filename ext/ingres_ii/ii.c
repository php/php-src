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
   | Contributed by ECL IP'S Software & Services                          |
   |                http://www.eclips-software.com                        |
   |                mailto://idev@eclips-software.com                     |
   | Author: David Hénot <henot@php.net>                                  |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_globals.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_ii.h"
#include "ii.h"
#include "ext/standard/php_string.h"

#if HAVE_II

ZEND_DECLARE_MODULE_GLOBALS(ii)

/* True globals, no need for thread safety */
static int le_ii_link, le_ii_plink;

#define SAFE_STRING(s) ((s)?(s):"")

/* Every user visible function must have an entry in ii_functions[].
*/
function_entry ii_functions[] = {
	PHP_FE(ingres_connect,			NULL)
	PHP_FE(ingres_pconnect,			NULL)
	PHP_FE(ingres_close,			NULL)
	PHP_FE(ingres_query,			NULL)
	PHP_FE(ingres_num_rows,			NULL)
	PHP_FE(ingres_num_fields,		NULL)
	PHP_FE(ingres_field_name,		NULL)
	PHP_FE(ingres_field_type,		NULL)
	PHP_FE(ingres_field_nullable,	NULL)
	PHP_FE(ingres_field_length,		NULL)
	PHP_FE(ingres_field_precision,	NULL)
	PHP_FE(ingres_field_scale,		NULL)
	PHP_FE(ingres_fetch_array,		NULL)
	PHP_FE(ingres_fetch_row,		NULL)
	PHP_FE(ingres_fetch_object,		NULL)
	PHP_FE(ingres_rollback,			NULL)
	PHP_FE(ingres_commit,			NULL)
	PHP_FE(ingres_autocommit,		NULL)
	{NULL, NULL, NULL}	/* Must be the last line in ii_functions[] */
};

zend_module_entry ingres_ii_module_entry = {
	STANDARD_MODULE_HEADER,
	"ingres_ii",
	ii_functions,
	PHP_MINIT(ii),
	PHP_MSHUTDOWN(ii),
	PHP_RINIT(ii),
	PHP_RSHUTDOWN(ii),
	PHP_MINFO(ii),
    NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_INGRES_II
ZEND_GET_MODULE(ingres_ii)
#endif

/* php.ini entries
*/
PHP_INI_BEGIN()
	STD_PHP_INI_BOOLEAN("ingres.allow_persistent", "1", PHP_INI_SYSTEM,	OnUpdateInt, allow_persistent, zend_ii_globals, ii_globals)
	STD_PHP_INI_ENTRY_EX("ingres.max_persistent", "-1", PHP_INI_SYSTEM, OnUpdateInt, max_persistent, zend_ii_globals, ii_globals, display_link_numbers)
	STD_PHP_INI_ENTRY_EX("ingres.max_links", "-1", PHP_INI_SYSTEM, OnUpdateInt, max_links, zend_ii_globals, ii_globals, display_link_numbers)
	STD_PHP_INI_ENTRY("ingres.default_database", NULL, PHP_INI_ALL, OnUpdateString, default_database, zend_ii_globals, ii_globals)
	STD_PHP_INI_ENTRY("ingres.default_user", NULL, PHP_INI_ALL, OnUpdateString, default_user, zend_ii_globals, ii_globals)
	STD_PHP_INI_ENTRY("ingres.default_password", NULL, PHP_INI_ALL, OnUpdateString, default_password, zend_ii_globals, ii_globals)
PHP_INI_END()

/* closes statement in given link
*/
static int _close_statement(II_LINK *link)
{
	IIAPI_CLOSEPARM closeParm;

	closeParm.cl_genParm.gp_callback = NULL;
	closeParm.cl_genParm.gp_closure = NULL;
	closeParm.cl_stmtHandle = link->stmtHandle;

	IIapi_close(&closeParm);
	ii_sync(&(closeParm.cl_genParm));

	if (ii_success(&(closeParm.cl_genParm)) == II_FAIL) {
		return 1;
	}

	link->stmtHandle = NULL;
	link->fieldCount = 0;
	link->descriptor = NULL;
	return 0;
}

/* rolls back transaction in given link
   after closing the active transaction (if any)
*/
static int _rollback_transaction(II_LINK *link)
{
	IIAPI_ROLLBACKPARM rollbackParm;

	if (link->stmtHandle && _close_statement(link)) {
		php_error(E_WARNING, "Ingres II:  Unable to close statement !!");
		return 1;
	}

	rollbackParm.rb_genParm.gp_callback = NULL;
	rollbackParm.rb_genParm.gp_closure = NULL;
	rollbackParm.rb_tranHandle = link->tranHandle;
	rollbackParm.rb_savePointHandle = NULL;

	IIapi_rollback(&rollbackParm);
	ii_sync(&(rollbackParm.rb_genParm));

	if (ii_success(&(rollbackParm.rb_genParm)) == II_FAIL) {
		return 1;
	}

	link->tranHandle = NULL;
	return 0;
}

static void _close_ii_link(II_LINK *link TSRMLS_DC)
{
	IIAPI_DISCONNPARM disconnParm;

	if (link->tranHandle && _rollback_transaction(link)) {
		php_error(E_WARNING, "Ingres II:  Unable to rollback transaction !!");
	}

	disconnParm.dc_genParm.gp_callback = NULL;
	disconnParm.dc_genParm.gp_closure = NULL;
	disconnParm.dc_connHandle = link->connHandle;

	IIapi_disconnect(&disconnParm);

	free(link);

	IIG(num_links)--;
}

/* closes the given link, actually disconnecting from server
   and releasing associated resources after rolling back the
   active transaction (if any)
*/
static void php_close_ii_link(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	II_LINK *link = (II_LINK *) rsrc->ptr;

	_close_ii_link(link TSRMLS_CC);
}

/* closes the given persistent link, see _close_ii_link
*/
static void _close_ii_plink(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	II_LINK *link = (II_LINK *) rsrc->ptr;

	_close_ii_link(link TSRMLS_CC);
	IIG(num_persistent)--;
}

/* cleans up the given persistent link.
   used when the request ends to 'refresh' the link for use
   by the next request
*/
static void _clean_ii_plink(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	II_LINK *link = (II_LINK *) rsrc->ptr;
	IIAPI_AUTOPARM autoParm;

	if (link->autocommit) {

		if (link->stmtHandle && _close_statement(link)) {
			php_error(E_WARNING, "Ingres II:  Unable to close statement !!");
		}

		autoParm.ac_genParm.gp_callback = NULL;
		autoParm.ac_genParm.gp_closure = NULL;
		autoParm.ac_connHandle = link->connHandle;
		autoParm.ac_tranHandle = link->tranHandle;

		IIapi_autocommit(&autoParm);
		ii_sync(&(autoParm.ac_genParm));

		if (ii_success(&(autoParm.ac_genParm)) == II_FAIL) {
			php_error(E_WARNING, "Ingres II:  Unable to disable autocommit");
		}

		link->autocommit = 0;
		link->tranHandle = NULL;
	}

	if (link->tranHandle && _rollback_transaction(link)) {
		php_error(E_WARNING, "Ingres II:  Unable to rollback transaction !!");
	}
}

/* sets the default link
*/
static void php_ii_set_default_link(int id TSRMLS_DC)
{
	if (IIG(default_link) != -1) {
		zend_list_delete(IIG(default_link));
	}
	IIG(default_link) = id;
	zend_list_addref(id);
}

/* gets the default link
   if none has been set, tries to open a new one with default
   parameters
*/
static int php_ii_get_default_link(INTERNAL_FUNCTION_PARAMETERS)
{
	if (IIG(default_link) == -1) {	/* no link opened yet, implicitly open one */
		ht = 0;
		php_ii_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
	}
	return IIG(default_link);
}

static void php_ii_globals_init(zend_ii_globals *ii_globals)
{
	ii_globals->num_persistent = 0;
}

/* Module initialization
*/
PHP_MINIT_FUNCTION(ii)
{
	IIAPI_INITPARM initParm;

	ZEND_INIT_MODULE_GLOBALS(ii, php_ii_globals_init, NULL);
	REGISTER_INI_ENTRIES();

	le_ii_link = zend_register_list_destructors_ex(php_close_ii_link, NULL, "ingres", module_number);
	le_ii_plink = zend_register_list_destructors_ex(_clean_ii_plink, _close_ii_plink, "ingres persistent", module_number);

	/* Constants registration */
	REGISTER_LONG_CONSTANT("INGRES_ASSOC",	II_ASSOC,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INGRES_NUM",	II_NUM,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INGRES_BOTH",	II_BOTH,	CONST_CS | CONST_PERSISTENT);

	/* Ingres api initialization */
	initParm.in_timeout = -1;				/* timeout in ms, -1 = no timeout */
	initParm.in_version = IIAPI_VERSION_1;	/* api version used */

	IIapi_initialize(&initParm);
	if (initParm.in_status == IIAPI_ST_SUCCESS) {
		return SUCCESS;
	} else {
		return FAILURE;
	}
}

/* Module shutdown
*/
PHP_MSHUTDOWN_FUNCTION(ii)
{
	IIAPI_TERMPARM termParm;

	UNREGISTER_INI_ENTRIES();

	/* Ingres api termination */
	IIapi_terminate(&termParm);
	if (termParm.tm_status == IIAPI_ST_SUCCESS) {
		return SUCCESS;
	} else {
		return FAILURE;
	}
}

/* New request initialization
*/
PHP_RINIT_FUNCTION(ii)
{
	IIG(default_link) = -1;
	IIG(num_links) = IIG(num_persistent);
	return SUCCESS;
}

/* End of request
*/
PHP_RSHUTDOWN_FUNCTION(ii)
{
	if (IIG(default_link) != -1) {
		zend_list_delete(IIG(default_link));
		IIG(default_link) = -1;
	}
	return SUCCESS;
}

/* Informations reported to phpinfo()
*/
PHP_MINFO_FUNCTION(ii)
{
	char buf[32];

	php_info_print_table_start();
	php_info_print_table_header(2, "Ingres II Support", "enabled");
	sprintf(buf, "%ld", IIG(num_persistent));
	php_info_print_table_row(2, "Active Persistent Links", buf);
	sprintf(buf, "%ld", IIG(num_links));
	php_info_print_table_row(2, "Active Links", buf);
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}

/* Waits for completion of the last Ingres api call
   used because of the asynchronous design of this api
*/
static int ii_sync(IIAPI_GENPARM *genParm)
{
	static IIAPI_WAITPARM waitParm = {
		-1,		/* no timeout, we don't want asynchronous queries */
		0		/* wt_status (output) */
	};

	while (genParm->gp_completed == FALSE) {
		IIapi_wait(&waitParm);
	}

	if (waitParm.wt_status != IIAPI_ST_SUCCESS) {
		php_error(E_WARNING, "Ingres II:  Unexpected failure of IIapi_wait()");
		return 0;
	}
	return 1;
}

/* Handles errors from Ingres api
*/
static int ii_success(IIAPI_GENPARM *genParm)
{
	switch (genParm->gp_status) {
	
		case IIAPI_ST_SUCCESS:
			return II_OK;

		case IIAPI_ST_NO_DATA:
			return II_NO_DATA;

		default:
			if (genParm->gp_errorHandle == NULL) {	/* no error message available */
				php_error(E_WARNING, "Ingres II:  Server or API error - no error message available");
			} else {
				IIAPI_GETEINFOPARM getEInfoParm;

				getEInfoParm.ge_errorHandle = genParm->gp_errorHandle;
				IIapi_getErrorInfo(&getEInfoParm);
				php_error(E_WARNING, "Ingres II:  Server or API error : %s", getEInfoParm.ge_message);
				php_error(E_WARNING, "Ingres II:  SQLSTATE : %s", getEInfoParm.ge_SQLSTATE);
			}
			return II_FAIL;
	}
}

/* Actually handles connection creation, either persistent or not
*/
static void php_ii_do_connect(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	zval **database, **username, **password;
	char *db, *user, *pass;
	int argc;
	char *hashed_details;
	int hashed_details_length;
	IIAPI_CONNPARM connParm;
	II_LINK *link;

	/* Setting db, user and pass according to sql_safe_mode, parameters and/or default values */
	argc = ZEND_NUM_ARGS();

	if (PG(sql_safe_mode)) {	/* sql_safe_mode */

		if (argc > 0) {
			php_error(E_NOTICE, "SQL safe mode in effect - ignoring host/user/password information");
		}

		db = pass = NULL;
		user = php_get_current_user();
		hashed_details_length = strlen(user) + sizeof("ingres___") - 1;
		hashed_details = (char *) emalloc(hashed_details_length + 1);
		sprintf(hashed_details, "Ingres__%s_", user);

	} else {					/* non-sql_safe_mode */

		db = IIG(default_database);
		user = IIG(default_user);
		pass = IIG(default_password);

		if (argc > 3 || zend_get_parameters_ex(argc, &database, &username, &password) == FAILURE) {
			WRONG_PARAM_COUNT;
		}

		switch (argc) {

			case 3:
				convert_to_string_ex(password);
				pass = Z_STRVAL_PP(password);
				/* Fall-through. */
		
			case 2:
				convert_to_string_ex(username);
				user = Z_STRVAL_PP(username);
				/* Fall-through. */
		
			case 1:
				convert_to_string_ex(database);
				db = Z_STRVAL_PP(database);
				/* Fall-through. */

			case 0:
				break;
		}

		hashed_details_length =	sizeof("ingres___") - 1 + 
								strlen(SAFE_STRING(db)) +
								strlen(SAFE_STRING(user)) + 
								strlen(SAFE_STRING(pass));

		hashed_details = (char *) emalloc(hashed_details_length + 1);
		sprintf(hashed_details, "Ingres_%s_%s_%s", SAFE_STRING(db),	SAFE_STRING(user), SAFE_STRING(pass));
	}

	/* if asked for unauthorized persistency, issue a warning
	   and go for a non-persistent link */
	if (persistent && !IIG(allow_persistent)) {
		php_error(E_WARNING, "Ingres II:  Persistent links disabled !");
		persistent = 0;
	}

	if (persistent) {
		list_entry *le;

		/* is this link already in the persistent list ? */
		if (zend_hash_find(&EG(persistent_list), hashed_details, hashed_details_length + 1, (void **) &le) == FAILURE) { /* no, new persistent connection */
			list_entry new_le;

			if (IIG(max_links) != -1 && IIG(num_links) >= IIG(max_links)) {
				php_error(E_WARNING, "Ingres II:  Too many open links (%d)", IIG(num_links));
				efree(hashed_details);
				RETURN_FALSE;
			}
			if (IIG(max_persistent) != -1 && IIG(num_persistent) >= IIG(max_persistent)) {
				php_error(E_WARNING, "Ingres II:  Too many open persistent links (%d)", IIG(num_persistent));
				efree(hashed_details);
				RETURN_FALSE;
			}

			/* create the link */
			connParm.co_genParm.gp_callback = NULL;
			connParm.co_genParm.gp_closure = NULL;
			connParm.co_target = db;
			connParm.co_username = user;
			connParm.co_password = pass;
			connParm.co_timeout = -1;	/* -1 is no timeout */
			connParm.co_connHandle = NULL;
			connParm.co_tranHandle = NULL;

			IIapi_connect(&connParm);

			if (!ii_sync(&(connParm.co_genParm)) || ii_success(&(connParm.co_genParm)) == II_FAIL) {
				efree(hashed_details);
				php_error(E_WARNING, "Ingres II:  Unable to connect to database (%s)", db);
				RETURN_FALSE;
			}

			link = (II_LINK *) malloc(sizeof(II_LINK));
			link->connHandle = connParm.co_connHandle;
			link->tranHandle = NULL;
			link->stmtHandle = NULL;
			link->fieldCount = 0;
			link->descriptor = NULL;
			link->autocommit = 0;

			/* hash it up */
			Z_TYPE(new_le) = le_ii_plink;
			new_le.ptr = link;
			if (zend_hash_update(&EG(persistent_list), hashed_details, hashed_details_length + 1, (void *) &new_le, sizeof(list_entry), NULL) == FAILURE) {
				php_error(E_WARNING, "Ingres II:  Unable to hash (%s)", hashed_details);
				free(link);
				efree(hashed_details);
				RETURN_FALSE;
			}
			IIG(num_persistent)++;
			IIG(num_links)++;

		} else { /* already open persistent connection */

			if (Z_TYPE_P(le) != le_ii_plink) {
				RETURN_FALSE;
			}
			/* here we should ensure that the link did not die */
			/* unable to figure out the right way to do this   */
			/* maybe does the api handle the reconnection transparently ? */
			link = (II_LINK *) le->ptr;
		}
		ZEND_REGISTER_RESOURCE(return_value, link, le_ii_plink);

	} else { /* non persistent */
		list_entry *index_ptr, new_index_ptr;

		/* first we check the hash for the hashed_details key.  if it exists,
		 * it should point us to the right offset where the actual link sits.
		 * if it doesn't, open a new link, add it to the resource list,
		 * and add a pointer to it with hashed_details as the key.
		 */
		if (zend_hash_find(&EG(regular_list), hashed_details, hashed_details_length + 1, (void **) &index_ptr) == SUCCESS) {
			int type;
			void *ptr;

			if (Z_TYPE_P(index_ptr) != le_index_ptr) {
				RETURN_FALSE;
			}
			link = (II_LINK *) index_ptr->ptr;
			ptr = zend_list_find((int) link, &type);	/* check if the link is still there */
			if (ptr && (type == le_ii_link || type == le_ii_plink)) {
				zend_list_addref((int) link);
				Z_LVAL_P(return_value) = (int) link;

				php_ii_set_default_link((int) link TSRMLS_CC);

				Z_TYPE_P(return_value) = IS_RESOURCE;
				efree(hashed_details);
				return;
			} else {
				zend_hash_del(&EG(regular_list), hashed_details, hashed_details_length + 1);
			}
		}
		if (IIG(max_links) != -1 && IIG(num_links) >= IIG(max_links)) {
			php_error(E_WARNING, "Ingres II:  Too many open links (%d)", IIG(num_links));
			efree(hashed_details);
			RETURN_FALSE;
		}

		/* create the link */
		connParm.co_genParm.gp_callback = NULL;
		connParm.co_genParm.gp_closure = NULL;
		connParm.co_target = db;
		connParm.co_username = user;
		connParm.co_password = pass;
		connParm.co_timeout = -1;	/* -1 is no timeout */
		connParm.co_connHandle = NULL;
		connParm.co_tranHandle = NULL;

		IIapi_connect(&connParm);

		if (!ii_sync(&(connParm.co_genParm)) || ii_success(&(connParm.co_genParm)) == II_FAIL) {
			efree(hashed_details);
			php_error(E_WARNING, "Ingres II:  Unable to connect to database (%s)", db);
			RETURN_FALSE;
		}

		link = (II_LINK *) malloc(sizeof(II_LINK));
		link->connHandle = connParm.co_connHandle;
		link->tranHandle = NULL;
		link->stmtHandle = NULL;
		link->fieldCount = 0;
		link->descriptor = NULL;
		link->autocommit = 0;

		/* add it to the list */
		ZEND_REGISTER_RESOURCE(return_value, link, le_ii_link);

		/* add it to the hash */
		new_index_ptr.ptr = (void *) Z_LVAL_P(return_value);
		Z_TYPE(new_index_ptr) = le_index_ptr;
		if (zend_hash_update(&EG(regular_list), hashed_details, hashed_details_length + 1, (void *) &new_index_ptr, sizeof(list_entry), NULL) == FAILURE) {
			php_error(E_WARNING, "Ingres II:  Unable to hash (%s)", hashed_details);
			free(link);
			efree(hashed_details);
			RETURN_FALSE;
		}
		IIG(num_links)++;
	}

	efree(hashed_details);
	php_ii_set_default_link(Z_LVAL_P(return_value) TSRMLS_CC);
}

/* {{{ proto resource ingres_connect([string database [, string username [, string password]]])
   Open a connection to an Ingres II database the syntax of database is [node_id::]dbname[/svr_class] */
PHP_FUNCTION(ingres_connect)
{
	php_ii_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto resource ingres_pconnect([string database [, string username [, string password]]])
   Open a persistent connection to an Ingres II database the syntax of database is [node_id::]dbname[/svr_class] */
PHP_FUNCTION(ingres_pconnect)
{
	php_ii_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto bool ingres_close([resource link])
   Close an Ingres II database connection */
PHP_FUNCTION(ingres_close)
{
	zval **link;
	int argc;
	int link_id = -1;
	II_LINK *ii_link;

	argc = ZEND_NUM_ARGS();
	if (argc > 1 || (argc && zend_get_parameters_ex(argc, &link) == FAILURE)) {
		WRONG_PARAM_COUNT;
	}

	if (argc == 0) {
		link_id = IIG(default_link);
	}

	ZEND_FETCH_RESOURCE2(ii_link, II_LINK *, link, link_id, "Ingres II Link", le_ii_link, le_ii_plink);

	zend_list_delete(link_id);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ingres_query(string query [, resource link])
   Send a SQL query to Ingres II */
/* This should go into the documentation */
/* Unsupported query types:
   - close
   - commit
   - connect
   - disconnect
   - get dbevent
   - prepare to commit
   - rollback
   - savepoint
   - set autocommit
   - <all cursor related queries>
   (look for dedicated functions instead) */
PHP_FUNCTION(ingres_query)
{
	zval **query, **link;
	int argc;
	int link_id = -1;
	II_LINK *ii_link;
	IIAPI_QUERYPARM queryParm;
	IIAPI_GETDESCRPARM getDescrParm;

	argc = ZEND_NUM_ARGS();
	if (argc < 1 || argc > 2 || zend_get_parameters_ex(argc, &query, &link) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (argc < 2) {
		link_id = php_ii_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	}

	ZEND_FETCH_RESOURCE2(ii_link, II_LINK *, link, link_id, "Ingres II Link", le_ii_link, le_ii_plink);

	convert_to_string_ex(query);

	/* if there's already an active statement, close it */
	if (ii_link->stmtHandle && _close_statement(ii_link)) {
		php_error(E_WARNING, "Ingres II:  Unable to close statement !!");
		RETURN_FALSE;
	}

	/* send the query */
	queryParm.qy_genParm.gp_callback = NULL;
	queryParm.qy_genParm.gp_closure = NULL;
	queryParm.qy_connHandle = ii_link->connHandle;
	queryParm.qy_tranHandle = ii_link->tranHandle;
	queryParm.qy_stmtHandle = NULL;
	queryParm.qy_queryType  = IIAPI_QT_QUERY;
	queryParm.qy_parameters = FALSE;
	queryParm.qy_queryText  = Z_STRVAL_PP(query);

	IIapi_query(&queryParm);
	ii_sync(&(queryParm.qy_genParm));

	if (ii_success(&(queryParm.qy_genParm)) == II_FAIL) {
		RETURN_FALSE;
	}

	/* store transaction and statement handles */
	ii_link->tranHandle = queryParm.qy_tranHandle;
	ii_link->stmtHandle = queryParm.qy_stmtHandle;

	/* get description of results */
	getDescrParm.gd_genParm.gp_callback = NULL;
	getDescrParm.gd_genParm.gp_closure  = NULL;
	getDescrParm.gd_stmtHandle = ii_link->stmtHandle;

	IIapi_getDescriptor(&getDescrParm);
	ii_sync(&(getDescrParm.gd_genParm));

	if (ii_success(&(getDescrParm.gd_genParm)) == II_FAIL) {
		RETURN_FALSE;
	}

	/* store the results */
	ii_link->fieldCount = getDescrParm.gd_descriptorCount;
	ii_link->descriptor = getDescrParm.gd_descriptor;

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ingres_num_rows([resource link])
   Return the number of rows affected/returned by the last query */

/* Warning : don't call ingres_num_rows() before ingres_fetch_xx(),
   or ingres_fetch_xx() wouldn't find any data */
PHP_FUNCTION(ingres_num_rows)
{
	zval **link;
	int argc;
	int link_id = -1;
	II_LINK *ii_link;
	IIAPI_GETQINFOPARM getQInfoParm;

	argc = ZEND_NUM_ARGS();
	if (argc > 1 || zend_get_parameters_ex(argc, &link) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (argc < 1) {
		link_id = php_ii_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	}

	ZEND_FETCH_RESOURCE2(ii_link, II_LINK *, link, link_id, "Ingres II Link", le_ii_link, le_ii_plink);

	/* get number of affected rows */
	getQInfoParm.gq_genParm.gp_callback = NULL;
	getQInfoParm.gq_genParm.gp_closure = NULL;
	getQInfoParm.gq_stmtHandle = ii_link->stmtHandle;

	IIapi_getQueryInfo(&getQInfoParm);
	ii_sync(&(getQInfoParm.gq_genParm));

	if (ii_success(&(getQInfoParm.gq_genParm)) == II_FAIL) {
		RETURN_FALSE;
	}

	/* return the result */
	if (getQInfoParm.gq_mask & IIAPI_GQ_ROW_COUNT) {
		RETURN_LONG(getQInfoParm.gq_rowCount);
	} else {
		RETURN_LONG(0);
	}
}
/* }}} */

/* {{{ proto int ingres_num_fields([resource link])
   Return the number of fields returned by the last query */
PHP_FUNCTION(ingres_num_fields)
{
	zval **link;
	int argc;
	int link_id = -1;
	II_LINK *ii_link;

	argc = ZEND_NUM_ARGS();
	if (argc > 1 || zend_get_parameters_ex(argc, &link) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (argc < 1) {
		link_id = php_ii_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	}

	ZEND_FETCH_RESOURCE2(ii_link, II_LINK *, link, link_id, "Ingres II Link", le_ii_link, le_ii_plink);

	RETURN_LONG(ii_link->fieldCount);
}
/* }}} */

#define II_FIELD_INFO_NAME 1
#define II_FIELD_INFO_TYPE 2
#define II_FIELD_INFO_NULLABLE 3
#define II_FIELD_INFO_LENGTH 4
#define II_FIELD_INFO_PRECISION 5
#define II_FIELD_INFO_SCALE 6

/* Return information about a field in a query result
*/
static void php_ii_field_info(INTERNAL_FUNCTION_PARAMETERS, int info_type)
{
	zval **idx, **link;
	int argc;
	int link_id = -1;
	char *name, *fun_name;
	int index;
	II_LINK *ii_link;

	argc = ZEND_NUM_ARGS();
	if (argc < 1 || argc > 2 || zend_get_parameters_ex(argc, &idx, &link) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (argc < 2) {
		link_id = php_ii_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	}

	ZEND_FETCH_RESOURCE2(ii_link, II_LINK *, link, link_id, "Ingres II Link", le_ii_link, le_ii_plink);

	convert_to_long_ex(idx);
	index = Z_LVAL_PP(idx);

	if (index < 1 || index > ii_link->fieldCount) {
		switch (info_type) {

			case II_FIELD_INFO_NAME:
				fun_name = "ii_field_name";
				break;

			case II_FIELD_INFO_TYPE:
				fun_name = "ii_field_type";
				break;

			case II_FIELD_INFO_NULLABLE:
				fun_name = "ii_field_nullable";
				break;
	
			case II_FIELD_INFO_LENGTH:
				fun_name = "ii_field_length";
				break;
	
			case II_FIELD_INFO_PRECISION:
				fun_name = "ii_field_precision";
				break;

			case II_FIELD_INFO_SCALE:
				fun_name = "ii_field_scale";
				break;

			default:
				fun_name = "foobar";
				break;
		}
		php_error(E_WARNING, "Ingres II:  %s() called with wrong index (%d)", fun_name, index);
		RETURN_FALSE;
	}

	switch (info_type) {

		case II_FIELD_INFO_NAME:
			name = php_ii_field_name(ii_link, index);
			if (name == NULL) {
				RETURN_FALSE;
			}
			RETURN_STRING(name, 1);
			break;

		case II_FIELD_INFO_TYPE:
			switch ((ii_link->descriptor[index - 1]).ds_dataType) {
	
				case IIAPI_BYTE_TYPE:
					RETURN_STRING("IIAPI_BYTE_TYPE", 1);
	
				case IIAPI_CHA_TYPE:
					RETURN_STRING("IIAPI_CHA_TYPE", 1);
	
				case IIAPI_CHR_TYPE:
					RETURN_STRING("IIAPI_CHR_TYPE", 1);

				case IIAPI_DEC_TYPE:
					RETURN_STRING("IIAPI_DEC_TYPE", 1);
	
				case IIAPI_DTE_TYPE:
					RETURN_STRING("IIAPI_DTE_TYPE", 1);
	
				case IIAPI_FLT_TYPE:
					RETURN_STRING("IIAPI_FLT_TYPE", 1);
	
				case IIAPI_INT_TYPE:
					RETURN_STRING("IIAPI_INT_TYPE", 1);
	
				case IIAPI_LOGKEY_TYPE:
					RETURN_STRING("IIAPI_LOGKEY_TYPE", 1);
	
				case IIAPI_LBYTE_TYPE:
					RETURN_STRING("IIAPI_LBYTE_TYPE", 1);
	
				case IIAPI_LVCH_TYPE:
					RETURN_STRING("IIAPI_LVCH_TYPE", 1);
	
				case IIAPI_MNY_TYPE:
					RETURN_STRING("IIAPI_MNY_TYPE", 1);
	
				case IIAPI_TABKEY_TYPE:
					RETURN_STRING("IIAPI_TABKEY_TYPE", 1);
	
				case IIAPI_TXT_TYPE:
					RETURN_STRING("IIAPI_TXT_TYPE", 1);
	
				case IIAPI_VBYTE_TYPE:
					RETURN_STRING("IIAPI_VBYTE_TYPE", 1);
	
				case IIAPI_VCH_TYPE:
					RETURN_STRING("IIAPI_VCH_TYPE", 1);
		
				default:
					php_error(E_WARNING, "Ingres II:  Unknown Ingres data type");
					RETURN_FALSE;
					break;
			}
			break;

		case II_FIELD_INFO_NULLABLE:
			if ((ii_link->descriptor[index - 1]).ds_nullable) {
				RETURN_TRUE;
			} else {
				RETURN_FALSE;
			}
			break;

		case II_FIELD_INFO_LENGTH:
			RETURN_LONG((ii_link->descriptor[index - 1]).ds_length);
			break;
	
		case II_FIELD_INFO_PRECISION:
			RETURN_LONG((ii_link->descriptor[index - 1]).ds_precision);
			break;

		case II_FIELD_INFO_SCALE:
			RETURN_LONG((ii_link->descriptor[index - 1]).ds_scale);
			break;
	
		default:
			RETURN_FALSE;
	}
}

/* Return the name of a field in a query result
*/
static char *php_ii_field_name(II_LINK *ii_link, int index)
{
	if (index < 1 || index > ii_link->fieldCount) {
		php_error(E_WARNING, "Ingres II:  php_ii_field_name() called with wrong index (%d)", index);
		return NULL;
	}

	return (ii_link->descriptor[index - 1]).ds_columnName;
}

/* {{{ proto string ingres_field_name(int index [, resource link])
   Return the name of a field in a query result index must be >0 and <= ingres_num_fields() */
PHP_FUNCTION(ingres_field_name)
{
	php_ii_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, II_FIELD_INFO_NAME);
}
/* }}} */

/* {{{ proto string ingres_field_type(int index [, resource link])
   Return the type of a field in a query result index must be >0 and <= ingres_num_fields() */
PHP_FUNCTION(ingres_field_type)
{
	php_ii_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, II_FIELD_INFO_TYPE);
}
/* }}} */

/* {{{ proto string ingres_field_nullable(int index [, resource link])
   Return true if the field is nullable and false otherwise index must be >0 and <= ingres_num_fields() */
PHP_FUNCTION(ingres_field_nullable)
{
	php_ii_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, II_FIELD_INFO_NULLABLE);
}
/* }}} */

/* {{{ proto string ingres_field_length(int index [, resource link])
   Return the length of a field in a query result index must be >0 and <= ingres_num_fields() */
PHP_FUNCTION(ingres_field_length)
{
	php_ii_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, II_FIELD_INFO_LENGTH);
}
/* }}} */

/* {{{ proto string ingres_field_precision(int index [, resource link])
   Return the precision of a field in a query result index must be >0 and <= ingres_num_fields() */
PHP_FUNCTION(ingres_field_precision)
{
	php_ii_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, II_FIELD_INFO_PRECISION);
}
/* }}} */

/* {{{ proto string ingres_field_scale(int index [, resource link])
   Return the scale of a field in a query result index must be >0 and <= ingres_num_fields() */
PHP_FUNCTION(ingres_field_scale)
{
	php_ii_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, II_FIELD_INFO_SCALE);
}
/* }}} */


/* Convert complex Ingres data types to php-usable ones
*/
#define IIAPI_CONVERT(destType, destSize, precision) {\
      convertParm.cv_srcDesc.ds_dataType = (ii_link->descriptor[i+k-2]).ds_dataType;\
      convertParm.cv_srcDesc.ds_nullable = (ii_link->descriptor[i+k-2]).ds_nullable;\
      convertParm.cv_srcDesc.ds_length = (ii_link->descriptor[i+k-2]).ds_length;\
      convertParm.cv_srcDesc.ds_precision = (ii_link->descriptor[i+k-2]).ds_precision;\
      convertParm.cv_srcDesc.ds_scale = (ii_link->descriptor[i+k-2]).ds_scale;\
      convertParm.cv_srcDesc.ds_columnType = (ii_link->descriptor[i+k-2]).ds_columnType;\
      convertParm.cv_srcDesc.ds_columnName = (ii_link->descriptor[i+k-2]).ds_columnName;\
      convertParm.cv_srcValue.dv_null = columnData[k-1].dv_null;\
      convertParm.cv_srcValue.dv_length = columnData[k-1].dv_length;\
      convertParm.cv_srcValue.dv_value = columnData[k-1].dv_value;\
      convertParm.cv_dstDesc.ds_dataType = destType;\
      convertParm.cv_dstDesc.ds_nullable = FALSE;\
      convertParm.cv_dstDesc.ds_length = destSize;\
      convertParm.cv_dstDesc.ds_precision = precision;\
      convertParm.cv_dstDesc.ds_scale = 0;\
      convertParm.cv_dstDesc.ds_columnType = IIAPI_COL_TUPLE;\
      convertParm.cv_dstDesc.ds_columnName = NULL;\
      convertParm.cv_dstValue.dv_null = FALSE;\
      convertParm.cv_dstValue.dv_length = convertParm.cv_dstDesc.ds_length;\
      convertParm.cv_dstValue.dv_value = emalloc(convertParm.cv_dstDesc.ds_length+1);\
\
      IIapi_convertData(&convertParm);\
\
      if(ii_success(&(getColParm.gc_genParm))!=II_OK) {\
          RETURN_FALSE;\
      }\
\
      columnData[k-1].dv_length = convertParm.cv_dstValue.dv_length;\
      columnData[k-1].dv_value = convertParm.cv_dstValue.dv_value;\
      efree(convertParm.cv_srcValue.dv_value);\
}


/* Fetch a row of result
*/
static void php_ii_fetch(INTERNAL_FUNCTION_PARAMETERS, II_LINK *ii_link, int result_type)
{
	IIAPI_GETCOLPARM getColParm;
	IIAPI_DATAVALUE *columnData;
	IIAPI_CONVERTPARM convertParm;
	int i, j, k;
	int more;
	double value_double = 0;
	long value_long = 0;
	char *value_char_p;
	int len, should_copy, correct_length;

	/* array initialization */
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	/* going through all fields */
	for (i = 1; i <= ii_link->fieldCount;) {
		j = 1;

		/* as long as there are no long byte or long varchar fields,
		   Ingres is able to fetch many fields at a time, so try to find
		   these types and stop if they're found.
		   variable j will get number of fields to fetch */
		if ((ii_link->descriptor[i]).ds_dataType != IIAPI_LBYTE_TYPE &&
			(ii_link->descriptor[i]).ds_dataType != IIAPI_LVCH_TYPE) {
			while (	(ii_link->descriptor[i + j - 1]).ds_dataType != IIAPI_LBYTE_TYPE &&
					(ii_link->descriptor[i + j - 1]).ds_dataType != IIAPI_LVCH_TYPE && 
					i + j <= ii_link->fieldCount) {
				j++;
			}
		}

		/* allocate memory for j fields */
		columnData = (IIAPI_DATAVALUE *) emalloc(j * sizeof(IIAPI_DATAVALUE));
		for (k = 1; k <= j; k++) {
			columnData[k - 1].dv_value = (II_PTR) emalloc((ii_link->descriptor[i + k - 2]).ds_length);
		}

		more = 1;				/* this is for multi segment LBYTE and LVCH elements */

		while (more) {
			getColParm.gc_genParm.gp_callback = NULL;
			getColParm.gc_genParm.gp_closure = NULL;
			getColParm.gc_rowCount = 1;
			getColParm.gc_columnCount = j;
			getColParm.gc_columnData = columnData;
			getColParm.gc_stmtHandle = ii_link->stmtHandle;
			getColParm.gc_moreSegments = 0;

			IIapi_getColumns(&getColParm);
			ii_sync(&(getColParm.gc_genParm));

			if (ii_success(&(getColParm.gc_genParm)) != II_OK) {
				RETURN_FALSE;
			}

			more = getColParm.gc_moreSegments;

			if (more) {			/* more segments of LBYTE or LVCH element to come */

				/* Multi segment LBYTE and LVCH elements not supported yet */
				php_error(E_ERROR, "Ingres II:  Multi segment LBYTE and LVCH elements not supported yet");

			} else {

				for (k = 1; k <= j; k++) {
					if (columnData[k - 1].dv_null) {	/* NULL value ? */

						if (result_type & II_NUM) {
							add_index_null(return_value, i + k - 1);
						}
						if (result_type & II_ASSOC) {
							add_assoc_null(return_value, php_ii_field_name(ii_link, i + k - 1));
						}

					} else {	/* non NULL value */
						correct_length = 0;

						switch ((ii_link->descriptor[i + k - 2]).ds_dataType) {
	
							case IIAPI_DEC_TYPE:	/* decimal (fixed point number) */
							case IIAPI_MNY_TYPE:	/* money */
								/* convert to floating point number */
								IIAPI_CONVERT(IIAPI_FLT_TYPE, sizeof(II_FLOAT8), 53);
								/* NO break */
	
							case IIAPI_FLT_TYPE:	/* floating point number */
								switch (columnData[k - 1].dv_length) {

									case 4:
										value_double = (double) *((II_FLOAT4 *) columnData[k - 1].dv_value);
										break;

									case 8:
										value_double = (double) *((II_FLOAT8 *) columnData[k - 1].dv_value);
										break;

									default:
										php_error(E_WARNING, "Ingres II:  Invalid size for IIAPI_FLT_TYPE data (%d)", columnData[k - 1].dv_length);
										break;
								}

								if (result_type & II_NUM) {
									add_index_double(return_value, i + k - 1, value_double);
								}

								if (result_type & II_ASSOC) {
									add_assoc_double(return_value, php_ii_field_name(ii_link, i + k - 1), value_double);
								}
								break;

							case IIAPI_INT_TYPE:	/* integer */
								switch (columnData[k - 1].dv_length) {

									case 1:
										value_long = (long) *((II_INT1 *) columnData[k - 1].dv_value);
										break;

									case 2:
										value_long = (long) *((II_INT2 *) columnData[k - 1].dv_value);
										break;
			
									case 4:
										value_long = (long) *((II_INT4 *) columnData[k - 1].dv_value);
										break;
		
									default:
										php_error(E_WARNING, "Ingres II:  Invalid size for IIAPI_INT_TYPE data (%d)", columnData[k - 1].dv_length);
										break;
								}

								if (result_type & II_NUM) {
									add_index_long(return_value, i + k - 1, value_long);
								}
	
								if (result_type & II_ASSOC) {
									add_assoc_long(return_value, php_ii_field_name(ii_link, i + k - 1), value_long);
								}
								break;

							case IIAPI_TXT_TYPE:	/* variable length character string */
							case IIAPI_VBYTE_TYPE:	/* variable length binary string */
							case IIAPI_VCH_TYPE:	/* variable length character string */
								/* real length is stored in first 2 bytes of data, so adjust
								   length variable and data pointer */
								columnData[k - 1].dv_length = *((II_INT2 *) columnData[k - 1].dv_value);
								((II_INT2 *) columnData[k - 1].dv_value)++;
								correct_length = 1;
								/* NO break */
	
							case IIAPI_BYTE_TYPE:	/* fixed length binary string */
							case IIAPI_CHA_TYPE:	/* fixed length character string */
							case IIAPI_CHR_TYPE:	/* fixed length character string */
							case IIAPI_LOGKEY_TYPE:	/* value unique to database */
							case IIAPI_TABKEY_TYPE:	/* value unique to table */
							case IIAPI_DTE_TYPE:	/* date */
								/* eventualy convert date to string */
								if ((ii_link->descriptor[i + k - 2]).
									ds_dataType == IIAPI_DTE_TYPE) {
									IIAPI_CONVERT(IIAPI_CHA_TYPE, 32, 0);
								}
	
								/* use php_addslashes if asked to */
								if (PG(magic_quotes_runtime)) {
									value_char_p = php_addslashes((char *) columnData[k - 1].dv_value, columnData[k - 1].dv_length, &len, 0 TSRMLS_CC);
									should_copy = 0;
								} else {
									value_char_p = (char *) columnData[k - 1].dv_value;
									len = columnData[k - 1].dv_length;
									should_copy = 1;
								}

								if (result_type & II_NUM) {
									add_index_stringl(return_value, i + k - 1, value_char_p, len, should_copy);
								}

								if (result_type & II_ASSOC) {
									add_assoc_stringl(return_value, php_ii_field_name (ii_link, i + k - 1), value_char_p, len, should_copy);
								}

								/* eventualy restore data pointer state for
							 	   variable length data types */
								if (correct_length) {
									((II_INT2 *) columnData[k - 1].dv_value)--;
								}
								break;
			
							default:
								php_error(E_WARNING, "Ingres II:  Invalid SQL data type in fetched field (%d -- length : %d)", (ii_link->descriptor[i + k - 2]).ds_dataType, columnData[k - 1].dv_length);
								break;
						}
					}
				}
			}
		}

		/* free the memory buffers */
		for (k = 1; k <= j; k++) {
			efree(columnData[k - 1].dv_value);
		}
		efree(columnData);

		/* increase field pointer by number of fetched fields */
		i += j;
	}
}

/* {{{ proto array ingres_fetch_array([int result_type [, resource link]])
   Fetch a row of result into an array result_type can be II_NUM for enumerated array, II_ASSOC for associative array, or II_BOTH (default) */
PHP_FUNCTION(ingres_fetch_array)
{
	zval **result_type, **link;
	int argc;
	int link_id = -1;
	II_LINK *ii_link;

	argc = ZEND_NUM_ARGS();
	if (argc > 2 || zend_get_parameters_ex(argc, &result_type, &link) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (argc != 2) {
		link_id = php_ii_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	}

	if (argc != 0) {
		convert_to_long_ex(result_type);
	}

	ZEND_FETCH_RESOURCE2(ii_link, II_LINK *, link, link_id, "Ingres II Link", le_ii_link, le_ii_plink);

	php_ii_fetch(INTERNAL_FUNCTION_PARAM_PASSTHRU, ii_link, (argc == 0 ? II_BOTH : Z_LVAL_PP(result_type)));
}
/* }}} */

/* {{{ proto array ingres_fetch_row([resource link])
   Fetch a row of result into an enumerated array */
PHP_FUNCTION(ingres_fetch_row)
{
	zval **link;
	int argc;
	int link_id = -1;
	II_LINK *ii_link;

	argc = ZEND_NUM_ARGS();
	if (argc > 1 || zend_get_parameters_ex(argc, &link) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (argc != 1) {
		link_id = php_ii_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	}

	ZEND_FETCH_RESOURCE2(ii_link, II_LINK *, link, link_id, "Ingres II Link", le_ii_link, le_ii_plink);

	php_ii_fetch(INTERNAL_FUNCTION_PARAM_PASSTHRU, ii_link, II_NUM);
}
/* }}} */

/* {{{ proto array ingres_fetch_object([int result_type [, resource link]])
   Fetch a row of result into an object result_type can be II_NUM for enumerated object, II_ASSOC for associative object, or II_BOTH (default) */
PHP_FUNCTION(ingres_fetch_object)
{
	zval **result_type, **link;
	int argc;
	int link_id = -1;
	II_LINK *ii_link;

	argc = ZEND_NUM_ARGS();
	if (argc > 2 || zend_get_parameters_ex(argc, &result_type, &link) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (argc != 2) {
		link_id = php_ii_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	}

	if (argc != 0) {
		convert_to_long_ex(result_type);
	}

	ZEND_FETCH_RESOURCE2(ii_link, II_LINK *, link, link_id, "Ingres II Link", le_ii_link, le_ii_plink);

	php_ii_fetch(INTERNAL_FUNCTION_PARAM_PASSTHRU, ii_link, (argc == 0 ? II_BOTH : Z_LVAL_PP(result_type)));

	if (Z_TYPE_P(return_value) == IS_ARRAY) {
		convert_to_object(return_value);
	}
}
/* }}} */

/* {{{ proto bool ingres_rollback([resource link])
   Roll back a transaction */
PHP_FUNCTION(ingres_rollback)
{
	zval **link;
	int argc;
	int link_id = -1;
	II_LINK *ii_link;

	argc = ZEND_NUM_ARGS();
	if (argc > 1 || (argc && zend_get_parameters_ex(argc, &link) == FAILURE)) {
		WRONG_PARAM_COUNT;
	}

	if (argc == 0) {
		link_id = IIG(default_link);
	}
	ZEND_FETCH_RESOURCE2(ii_link, II_LINK *, link, link_id, "Ingres II Link", le_ii_link, le_ii_plink);

	if (_rollback_transaction(ii_link)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ingres_commit([resource link])
   Commit a transaction */
PHP_FUNCTION(ingres_commit)
{
	zval **link;
	int argc;
	int link_id = -1;
	II_LINK *ii_link;
	IIAPI_COMMITPARM commitParm;

	argc = ZEND_NUM_ARGS();
	if (argc > 1 || (argc && zend_get_parameters_ex(argc, &link) == FAILURE)) {
		WRONG_PARAM_COUNT;
	}

	if (argc == 0) {
		link_id = IIG(default_link);
	}

	ZEND_FETCH_RESOURCE2(ii_link, II_LINK *, link, link_id, "Ingres II Link", le_ii_link, le_ii_plink);

	if (ii_link->stmtHandle && _close_statement(ii_link)) {
		php_error(E_WARNING, "Ingres II:  Unable to close statement !!");
		RETURN_FALSE;
	}

	commitParm.cm_genParm.gp_callback = NULL;
	commitParm.cm_genParm.gp_closure = NULL;
	commitParm.cm_tranHandle = ii_link->tranHandle;

	IIapi_commit(&commitParm);
	ii_sync(&(commitParm.cm_genParm));

	if (ii_success(&(commitParm.cm_genParm)) == II_FAIL) {
		RETURN_FALSE;
	}

	ii_link->tranHandle = NULL;
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ingres_autocommit([resource link])
   Switch autocommit on or off */
PHP_FUNCTION(ingres_autocommit)
{
	zval **link;
	int argc;
	int link_id = -1;
	II_LINK *ii_link;
	IIAPI_AUTOPARM autoParm;

	argc = ZEND_NUM_ARGS();
	if (argc > 1 || (argc && zend_get_parameters_ex(argc, &link) == FAILURE)) {
		WRONG_PARAM_COUNT;
	}

	if (argc == 0) {
		link_id = IIG(default_link);
	}

	ZEND_FETCH_RESOURCE2(ii_link, II_LINK *, link, link_id, "Ingres II Link", le_ii_link, le_ii_plink);

	autoParm.ac_genParm.gp_callback = NULL;
	autoParm.ac_genParm.gp_closure = NULL;
	autoParm.ac_connHandle = ii_link->connHandle;
	autoParm.ac_tranHandle = ii_link->tranHandle;

	IIapi_autocommit(&autoParm);
	ii_sync(&(autoParm.ac_genParm));

	if (ii_success(&(autoParm.ac_genParm)) == II_FAIL) {
		RETURN_FALSE;
	}

	ii_link->autocommit = (ii_link->autocommit ? 0 : 1);
	ii_link->tranHandle = autoParm.ac_tranHandle;
	RETURN_TRUE;
}
/* }}} */

#endif /* HAVE_II */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
     
