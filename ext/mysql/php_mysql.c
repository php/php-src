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
   | Author: Zeev Suraski <zeev@zend.com>                                 |
   +----------------------------------------------------------------------+
*/
 
/* $Id$ */


/* TODO:
 *
 * ? Safe mode implementation
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_globals.h"
#include "php_mysql.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"

#if HAVE_MYSQL

#ifdef PHP_WIN32
#include <winsock.h>
#define signal(a, b) NULL
#else
#include "build-defs.h"
#if HAVE_SIGNAL_H
#include <signal.h>
#endif

#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <netdb.h>
#include <netinet/in.h>
#endif

#ifndef HAVE_USHORT
#undef ushort
typedef unsigned short ushort;
#endif

/* True globals, no need for thread safety */
static int le_result, le_link, le_plink;

#include "php_ini.h"

# include <mysql.h>

#ifdef HAVE_MYSQL_REAL_CONNECT
# ifdef HAVE_ERRMSG_H
#  include <errmsg.h>
# endif
#endif

#define SAFE_STRING(s) ((s)?(s):"")

#if MYSQL_VERSION_ID > 32199
#define mysql_row_length_type unsigned long
#define HAVE_MYSQL_ERRNO
#else
#define mysql_row_length_type unsigned int
#	ifdef mysql_errno
#	define HAVE_MYSQL_ERRNO
#	endif
#endif

#if MYSQL_VERSION_ID >= 32032
#define HAVE_GETINFO_FUNCS
#endif

#if MYSQL_VERSION_ID > 32133 || defined(FIELD_TYPE_TINY)
#define MYSQL_HAS_TINY
#endif

#if MYSQL_VERSION_ID >= 32200
#define MYSQL_HAS_YEAR
#endif

#define MYSQL_ASSOC		1<<0
#define MYSQL_NUM		1<<1
#define MYSQL_BOTH		(MYSQL_ASSOC|MYSQL_NUM)

#define MYSQL_USE_RESULT	0
#define MYSQL_STORE_RESULT	1

#if MYSQL_VERSION_ID < 32223
#define PHP_MYSQL_VALID_RESULT(mysql)		\
	(mysql_num_fields(mysql)>0)
#else
#define PHP_MYSQL_VALID_RESULT(mysql)		\
	(mysql_field_count(mysql)>0)
#endif

typedef struct _php_mysql_conn {
	MYSQL conn;
	int active_result_id;
} php_mysql_conn;


/* {{{ mysql_functions[]
 */
function_entry mysql_functions[] = {
	PHP_FE(mysql_connect,								NULL)
	PHP_FE(mysql_pconnect,								NULL)
	PHP_FE(mysql_close,									NULL)
	PHP_FE(mysql_select_db,								NULL)
#if MYSQL_VERSION_ID < 40000
	PHP_FE(mysql_create_db,								NULL)
	PHP_FE(mysql_drop_db,								NULL)
#endif
	PHP_FE(mysql_query,									NULL)
	PHP_FE(mysql_unbuffered_query,						NULL)
	PHP_FE(mysql_db_query,								NULL)
	PHP_FE(mysql_list_dbs,								NULL)
	PHP_FE(mysql_list_tables,							NULL)
	PHP_FE(mysql_list_fields,							NULL)
	PHP_FE(mysql_list_processes,					NULL)
	PHP_FE(mysql_error,									NULL)
#ifdef HAVE_MYSQL_ERRNO
	PHP_FE(mysql_errno,									NULL)
#endif
	PHP_FE(mysql_affected_rows,							NULL)
	PHP_FE(mysql_insert_id,								NULL)
	PHP_FE(mysql_result,								NULL)
	PHP_FE(mysql_num_rows,								NULL)
	PHP_FE(mysql_num_fields,							NULL)
	PHP_FE(mysql_fetch_row,								NULL)
	PHP_FE(mysql_fetch_array,							NULL)
	PHP_FE(mysql_fetch_assoc,							NULL)
	PHP_FE(mysql_fetch_object,							NULL)
	PHP_FE(mysql_data_seek,								NULL)
	PHP_FE(mysql_fetch_lengths,							NULL)
	PHP_FE(mysql_fetch_field,							NULL)
	PHP_FE(mysql_field_seek,							NULL)
	PHP_FE(mysql_free_result,							NULL)
	PHP_FE(mysql_field_name,							NULL)
	PHP_FE(mysql_field_table,							NULL)
	PHP_FE(mysql_field_len,								NULL)
	PHP_FE(mysql_field_type,							NULL)
	PHP_FE(mysql_field_flags,							NULL)
	PHP_FE(mysql_escape_string,							NULL)
	PHP_FE(mysql_real_escape_string,					NULL)
	PHP_FE(mysql_stat,									NULL)
	PHP_FE(mysql_thread_id,								NULL)
	PHP_FE(mysql_character_set_name,					NULL)
	PHP_FE(mysql_ping,									NULL)
#ifdef HAVE_GETINFO_FUNCS
	PHP_FE(mysql_get_client_info,						NULL)
	PHP_FE(mysql_get_host_info,							NULL)
	PHP_FE(mysql_get_proto_info,						NULL)
	PHP_FE(mysql_get_server_info,						NULL)
#endif

	PHP_FE(mysql_info,		            				NULL)
	 
	/* for downwards compatability */
	PHP_FALIAS(mysql,				mysql_db_query,		NULL)
	PHP_FALIAS(mysql_fieldname,		mysql_field_name,	NULL)
	PHP_FALIAS(mysql_fieldtable,	mysql_field_table,	NULL)
	PHP_FALIAS(mysql_fieldlen,		mysql_field_len,	NULL)
	PHP_FALIAS(mysql_fieldtype,		mysql_field_type,	NULL)
	PHP_FALIAS(mysql_fieldflags,	mysql_field_flags,	NULL)
	PHP_FALIAS(mysql_selectdb,		mysql_select_db,	NULL)
#if MYSQL_VERSION_ID < 40000
	PHP_FALIAS(mysql_createdb,		mysql_create_db,	NULL)
	PHP_FALIAS(mysql_dropdb,		mysql_drop_db,		NULL)
#endif
	PHP_FALIAS(mysql_freeresult,	mysql_free_result,	NULL)
	PHP_FALIAS(mysql_numfields,		mysql_num_fields,	NULL)
	PHP_FALIAS(mysql_numrows,		mysql_num_rows,		NULL)
	PHP_FALIAS(mysql_listdbs,		mysql_list_dbs,		NULL)
	PHP_FALIAS(mysql_listtables,	mysql_list_tables,	NULL)
	PHP_FALIAS(mysql_listfields,	mysql_list_fields,	NULL)
	PHP_FALIAS(mysql_db_name,		mysql_result,		NULL)
	PHP_FALIAS(mysql_dbname,		mysql_result,		NULL)
	PHP_FALIAS(mysql_tablename,		mysql_result,		NULL)
	PHP_FALIAS(mysql_table_name,	mysql_result,		NULL)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ mysql_module_entry
 */
zend_module_entry mysql_module_entry = {
	STANDARD_MODULE_HEADER,
	"mysql",
	mysql_functions,
	ZEND_MODULE_STARTUP_N(mysql),
	PHP_MSHUTDOWN(mysql),
	PHP_RINIT(mysql),
	PHP_RSHUTDOWN(mysql),
	PHP_MINFO(mysql),
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

ZEND_DECLARE_MODULE_GLOBALS(mysql)

#ifdef COMPILE_DL_MYSQL
ZEND_GET_MODULE(mysql)
#endif

void timeout(int sig);

#define CHECK_LINK(link) { if (link==-1) { php_error(E_WARNING, "%s(): A link to the server could not be established", get_active_function_name(TSRMLS_C)); RETURN_FALSE; } }

/* {{{ _free_mysql_result
 * This wrapper is required since mysql_free_result() returns an integer, and
 * thus, cannot be used directly
 */
static void _free_mysql_result(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	MYSQL_RES *mysql_result = (MYSQL_RES *)rsrc->ptr;

	mysql_free_result(mysql_result);
}
/* }}} */

/* {{{ php_mysql_set_default_link
 */
static void php_mysql_set_default_link(int id TSRMLS_DC)
{
	if (MySG(default_link)!=-1) {
		zend_list_delete(MySG(default_link));
	}
	MySG(default_link) = id;
	zend_list_addref(id);
}
/* }}} */

/* {{{ _close_mysql_link
 */
static void _close_mysql_link(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	php_mysql_conn *link = (php_mysql_conn *)rsrc->ptr;
	void (*handler) (int); 

	handler = signal(SIGPIPE, SIG_IGN);
	mysql_close(&link->conn);
	signal(SIGPIPE, handler);
	efree(link);
	MySG(num_links)--;
}
/* }}} */

/* {{{ _close_mysql_plink
 */
static void _close_mysql_plink(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	php_mysql_conn *link = (php_mysql_conn *)rsrc->ptr;
	void (*handler) (int);

	handler = signal(SIGPIPE, SIG_IGN);
	mysql_close(&link->conn);
	signal(SIGPIPE, handler);

	free(link);
	MySG(num_persistent)--;
	MySG(num_links)--;
}
/* }}} */

/* {{{ PHP_INI_MH
 */
static PHP_INI_MH(OnMySQLPort)
{
	if (new_value==NULL) { /* default port */
#ifndef PHP_WIN32
		struct servent *serv_ptr;
		char *env;
		
		MySG(default_port) = MYSQL_PORT;
		if ((serv_ptr = getservbyname("mysql", "tcp"))) {
			MySG(default_port) = (uint) ntohs((ushort) serv_ptr->s_port);
		}
		if ((env = getenv("MYSQL_TCP_PORT"))) {
			MySG(default_port) = (uint) atoi(env);
		}
#else
		MySG(default_port) = MYSQL_PORT;
#endif
	} else {
		MySG(default_port) = atoi(new_value);
	}
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_INI */
PHP_INI_BEGIN()
	STD_PHP_INI_BOOLEAN("mysql.allow_persistent",	"1",	PHP_INI_SYSTEM,		OnUpdateInt,		allow_persistent,	zend_mysql_globals,		mysql_globals)
	STD_PHP_INI_ENTRY_EX("mysql.max_persistent",	"-1",	PHP_INI_SYSTEM,		OnUpdateInt,		max_persistent,		zend_mysql_globals,		mysql_globals,	display_link_numbers)
	STD_PHP_INI_ENTRY_EX("mysql.max_links",			"-1",	PHP_INI_SYSTEM,		OnUpdateInt,		max_links,			zend_mysql_globals,		mysql_globals,	display_link_numbers)
	STD_PHP_INI_ENTRY("mysql.default_host",			NULL,	PHP_INI_ALL,		OnUpdateString,		default_host,		zend_mysql_globals,		mysql_globals)
	STD_PHP_INI_ENTRY("mysql.default_user",			NULL,	PHP_INI_ALL,		OnUpdateString,		default_user,		zend_mysql_globals,		mysql_globals)
	STD_PHP_INI_ENTRY("mysql.default_password",		NULL,	PHP_INI_ALL,		OnUpdateString,		default_password,	zend_mysql_globals,		mysql_globals)
	PHP_INI_ENTRY("mysql.default_port",				NULL,	PHP_INI_ALL,		OnMySQLPort)
	STD_PHP_INI_ENTRY("mysql.default_socket",		NULL,	PHP_INI_ALL,		OnUpdateStringUnempty,	default_socket,	zend_mysql_globals,		mysql_globals)
PHP_INI_END()
/* }}} */

/* {{{ php_mysql_init_globals
 */
static void php_mysql_init_globals(zend_mysql_globals *mysql_globals)
{
	mysql_globals->num_persistent = 0;
	mysql_globals->default_socket = NULL;
	mysql_globals->default_host = NULL;
	mysql_globals->default_user = NULL;
	mysql_globals->default_password = NULL;
	mysql_globals->connect_errno = 0;
	mysql_globals->connect_error = NULL;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
ZEND_MODULE_STARTUP_D(mysql)
{
	ZEND_INIT_MODULE_GLOBALS(mysql, php_mysql_init_globals, NULL);

	REGISTER_INI_ENTRIES();
	le_result = zend_register_list_destructors_ex(_free_mysql_result, NULL, "mysql result", module_number);
	le_link = zend_register_list_destructors_ex(_close_mysql_link, NULL, "mysql link", module_number);
	le_plink = zend_register_list_destructors_ex(NULL, _close_mysql_plink, "mysql link persistent", module_number);
	Z_TYPE(mysql_module_entry) = type;
	
	REGISTER_LONG_CONSTANT("MYSQL_ASSOC", MYSQL_ASSOC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQL_NUM", MYSQL_NUM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQL_BOTH", MYSQL_BOTH, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQL_USE_RESULT", MYSQL_USE_RESULT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQL_STORE_RESULT", MYSQL_STORE_RESULT, CONST_CS | CONST_PERSISTENT);

#ifdef ZTS
# if MYSQL_VERSION_ID >= 40000
	mysql_thread_init();
# endif
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(mysql)
{
#ifdef ZTS
# if MYSQL_VERSION_ID >= 40000
	mysql_thread_end();
# endif
#endif

	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(mysql)
{
	MySG(default_link)=-1;
	MySG(num_links) = MySG(num_persistent);
	/* Reset connect error/errno on every request */
	MySG(connect_error) = NULL;
	MySG(connect_errno)=0;
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(mysql)
{
	if (MySG(connect_error)!=NULL) {
		efree(MySG(connect_error));
	}
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(mysql)
{
	char buf[32];

	php_info_print_table_start();
	php_info_print_table_header(2, "MySQL Support", "enabled");
	sprintf(buf, "%ld", MySG(num_persistent));
	php_info_print_table_row(2, "Active Persistent Links", buf);
	sprintf(buf, "%ld", MySG(num_links));
	php_info_print_table_row(2, "Active Links", buf);
	php_info_print_table_row(2, "Client API version", mysql_get_client_info());
#ifndef PHP_WIN32
	php_info_print_table_row(2, "MYSQL_MODULE_TYPE", PHP_MYSQL_TYPE);
	php_info_print_table_row(2, "MYSQL_SOCKET", MYSQL_UNIX_ADDR);
	php_info_print_table_row(2, "MYSQL_INCLUDE", PHP_MYSQL_INCLUDE);
	php_info_print_table_row(2, "MYSQL_LIBS", PHP_MYSQL_LIBS);
#endif
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();

}
/* }}} */

/* {{{ php_mysql_do_connect
 */
#define MYSQL_DO_CONNECT_CLEANUP()	\
	if (free_host) {				\
		efree(host);				\
	}

#define MYSQL_DO_CONNECT_RETURN_FALSE()		\
	MYSQL_DO_CONNECT_CLEANUP();				\
	RETURN_FALSE;

static void php_mysql_do_connect(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	char *user=NULL, *passwd=NULL, *host_and_port=NULL, *socket=NULL, *tmp=NULL, *host=NULL;
	char *hashed_details=NULL;
	int hashed_details_length, port = MYSQL_PORT;
	php_mysql_conn *mysql=NULL;
	void (*handler) (int);
	zval **z_host=NULL, **z_user=NULL, **z_passwd=NULL, **z_new_link=NULL;
	zend_bool free_host=0, new_link=0;

	socket = MySG(default_socket);

	if (PG(sql_safe_mode)) {
		if (ZEND_NUM_ARGS()>0) {
			php_error(E_NOTICE, "%s(): SQL safe mode in effect - ignoring host/user/password information", get_active_function_name(TSRMLS_C));
		}
		host_and_port=passwd=NULL;
		user=php_get_current_user();
		hashed_details_length = strlen(user)+5+3;
		hashed_details = (char *) emalloc(hashed_details_length+1);
		sprintf(hashed_details, "mysql__%s_", user);
	} else {
		host_and_port = MySG(default_host);
		user = MySG(default_user);
		passwd = MySG(default_password);
		
		switch(ZEND_NUM_ARGS()) {
			case 0: /* defaults */
				break;
			case 1: {					
					if (zend_get_parameters_ex(1, &z_host)==FAILURE) {
						MYSQL_DO_CONNECT_RETURN_FALSE();
					}
				}
				break;
			case 2: {
					if (zend_get_parameters_ex(2, &z_host, &z_user)==FAILURE) {
						MYSQL_DO_CONNECT_RETURN_FALSE();
					}
					convert_to_string_ex(z_user);
					user = Z_STRVAL_PP(z_user);
				}
				break;
			case 3: {
					if (zend_get_parameters_ex(3, &z_host, &z_user, &z_passwd) == FAILURE) {
						MYSQL_DO_CONNECT_RETURN_FALSE();
					}
					convert_to_string_ex(z_user);
					convert_to_string_ex(z_passwd);
					user = Z_STRVAL_PP(z_user);
					passwd = Z_STRVAL_PP(z_passwd);
				}
				break;
			case 4: {
					if (zend_get_parameters_ex(4, &z_host, &z_user, &z_passwd, &z_new_link) == FAILURE) {
						MYSQL_DO_CONNECT_RETURN_FALSE();
					}
					convert_to_string_ex(z_user);
					convert_to_string_ex(z_passwd);
					user = Z_STRVAL_PP(z_user);
					passwd = Z_STRVAL_PP(z_passwd);
					new_link = Z_BVAL_PP(z_new_link);
				}
				break;
			default:
				WRONG_PARAM_COUNT;
				break;
		}
		if (z_host) {
			SEPARATE_ZVAL(z_host); /* We may modify z_host if it contains a port, separate */
			convert_to_string_ex(z_host);
			host_and_port = Z_STRVAL_PP(z_host);
			if (z_user) {
				convert_to_string_ex(z_user);
				user = Z_STRVAL_PP(z_user);
				if (z_passwd) {
					convert_to_string_ex(z_passwd);
					passwd = Z_STRVAL_PP(z_passwd);
				}
			}
		}

		hashed_details_length = sizeof("mysql___")-1 + strlen(SAFE_STRING(host_and_port))+strlen(SAFE_STRING(user))+strlen(SAFE_STRING(passwd));
		hashed_details = (char *) emalloc(hashed_details_length+1);
		sprintf(hashed_details, "mysql_%s_%s_%s", SAFE_STRING(host_and_port), SAFE_STRING(user), SAFE_STRING(passwd));
	}

	/* We cannot use mysql_port anymore in windows, need to use
	 * mysql_real_connect() to set the port.
	 */
	if (host_and_port && (tmp=strchr(host_and_port, ':'))) {
		host = estrndup(host_and_port, tmp-host_and_port);
		free_host = 1;
		tmp++;
		if (tmp[0] != '/') {
			port = atoi(tmp);
			if ((tmp=strchr(tmp, ':'))) {
				tmp++;
				socket=tmp;
			}
		} else {
			socket = tmp;
		}
	} else {
		host = host_and_port;
		port = MySG(default_port);
	}

#if MYSQL_VERSION_ID < 32200
	mysql_port = port;
#endif

	if (!MySG(allow_persistent)) {
		persistent=0;
	}
	if (persistent) {
		list_entry *le;

		/* try to find if we already have this link in our persistent list */
		if (zend_hash_find(&EG(persistent_list), hashed_details, hashed_details_length+1, (void **) &le)==FAILURE) {  /* we don't */
			list_entry new_le;

			if (MySG(max_links)!=-1 && MySG(num_links)>=MySG(max_links)) {
				php_error(E_WARNING, "%s(): Too many open links (%d)", get_active_function_name(TSRMLS_C), MySG(num_links));
				efree(hashed_details);
				MYSQL_DO_CONNECT_RETURN_FALSE();
			}
			if (MySG(max_persistent)!=-1 && MySG(num_persistent)>=MySG(max_persistent)) {
				php_error(E_WARNING, "%s(): Too many open persistent links (%d)", get_active_function_name(TSRMLS_C), MySG(num_persistent));
				efree(hashed_details);
				MYSQL_DO_CONNECT_RETURN_FALSE();
			}
			/* create the link */
			mysql = (php_mysql_conn *) malloc(sizeof(php_mysql_conn));
			mysql->active_result_id = 0;
#if MYSQL_VERSION_ID > 32199 /* this lets us set the port number */
			mysql_init(&mysql->conn);
			if (mysql_real_connect(&mysql->conn, host, user, passwd, NULL, port, socket, 0)==NULL) {
#else
			if (mysql_connect(&mysql->conn, host, user, passwd)==NULL) {
#endif
				/* Populate connect error globals so that the error functions can read them */
				if (MySG(connect_error)!=NULL) efree(MySG(connect_error));
				MySG(connect_error)=estrdup(mysql_error(&mysql->conn));
				php_error(E_WARNING, "%s(): %s", get_active_function_name(TSRMLS_C), MySG(connect_error));
#if defined(HAVE_MYSQL_ERRNO)
				MySG(connect_errno)=mysql_errno(&mysql->conn);
#endif
				free(mysql);
				efree(hashed_details);
				MYSQL_DO_CONNECT_RETURN_FALSE();
			}

			/* hash it up */
			Z_TYPE(new_le) = le_plink;
			new_le.ptr = mysql;
			if (zend_hash_update(&EG(persistent_list), hashed_details, hashed_details_length+1, (void *) &new_le, sizeof(list_entry), NULL)==FAILURE) {
				free(mysql);
				efree(hashed_details);
				MYSQL_DO_CONNECT_RETURN_FALSE();
			}
			MySG(num_persistent)++;
			MySG(num_links)++;
		} else {  /* we do */
			if (Z_TYPE_P(le) != le_plink) {
				MYSQL_DO_CONNECT_RETURN_FALSE();
			}
			/* ensure that the link did not die */
			handler=signal(SIGPIPE, SIG_IGN);
#if defined(HAVE_MYSQL_ERRNO) && defined(CR_SERVER_GONE_ERROR)
			mysql_stat(le->ptr);
			if (mysql_errno(&((php_mysql_conn *) le->ptr)->conn) == CR_SERVER_GONE_ERROR) {
#else
			if (!strcasecmp(mysql_stat(le->ptr), "mysql server has gone away")) { /* the link died */
#endif
				signal(SIGPIPE, handler);
#if MYSQL_VERSION_ID > 32199 /* this lets us set the port number */
				if (mysql_real_connect(le->ptr, host, user, passwd, NULL, port, socket, 0)==NULL) {
#else
				if (mysql_connect(le->ptr, host, user, passwd)==NULL) {
#endif
					php_error(E_WARNING, "%s: Link to server lost, unable to reconnect", get_active_function_name(TSRMLS_C));
					zend_hash_del(&EG(persistent_list), hashed_details, hashed_details_length+1);
					efree(hashed_details);
					MYSQL_DO_CONNECT_RETURN_FALSE();
				}
			}
			signal(SIGPIPE, handler);
			mysql = (php_mysql_conn *) le->ptr;
		}
		ZEND_REGISTER_RESOURCE(return_value, mysql, le_plink);
	} else { /* non persistent */
		list_entry *index_ptr, new_index_ptr;
		
		/* first we check the hash for the hashed_details key.  if it exists,
		 * it should point us to the right offset where the actual mysql link sits.
		 * if it doesn't, open a new mysql link, add it to the resource list,
		 * and add a pointer to it with hashed_details as the key.
		 */
		if (!new_link && zend_hash_find(&EG(regular_list), hashed_details, hashed_details_length+1,(void **) &index_ptr)==SUCCESS) {
			int type, link;
			void *ptr;

			if (Z_TYPE_P(index_ptr) != le_index_ptr) {
				MYSQL_DO_CONNECT_RETURN_FALSE();
			}
			link = (int) index_ptr->ptr;
			ptr = zend_list_find(link,&type);   /* check if the link is still there */
			if (ptr && (type==le_link || type==le_plink)) {
				zend_list_addref(link);
				Z_LVAL_P(return_value) = link;
				php_mysql_set_default_link(link TSRMLS_CC);
				Z_TYPE_P(return_value) = IS_RESOURCE;
				efree(hashed_details);
				MYSQL_DO_CONNECT_CLEANUP();
				return;
			} else {
				zend_hash_del(&EG(regular_list), hashed_details, hashed_details_length+1);
			}
		}
		if (MySG(max_links)!=-1 && MySG(num_links)>=MySG(max_links)) {
			php_error(E_WARNING, "%s(): Too many open links (%d)", get_active_function_name(TSRMLS_C), MySG(num_links));
			efree(hashed_details);
			MYSQL_DO_CONNECT_RETURN_FALSE();
		}

		mysql = (php_mysql_conn *) emalloc(sizeof(php_mysql_conn));
		mysql->active_result_id = 0;
#if MYSQL_VERSION_ID > 32199 /* this lets us set the port number */
		mysql_init(&mysql->conn);
		if (mysql_real_connect(&mysql->conn, host, user, passwd, NULL, port, socket, 0)==NULL) {
#else
		if (mysql_connect(&mysql->conn, host, user, passwd)==NULL) {
#endif
			/* Populate connect error globals so that the error functions can read them */
			if (MySG(connect_error)!=NULL) efree(MySG(connect_error));
			MySG(connect_error)=estrdup(mysql_error(&mysql->conn));
			php_error(E_WARNING, "%s(): %s", get_active_function_name(TSRMLS_C), MySG(connect_error));
#if defined(HAVE_MYSQL_ERRNO)
			MySG(connect_errno)=mysql_errno(&mysql->conn);
#endif
			php_error(E_WARNING, "%s(): MySQL Connection Failed: %s\n", get_active_function_name(TSRMLS_C), mysql_error(&mysql->conn));
			efree(hashed_details);
			efree(mysql);
			MYSQL_DO_CONNECT_RETURN_FALSE();
		}

		/* add it to the list */
		ZEND_REGISTER_RESOURCE(return_value, mysql, le_link);

		/* add it to the hash */
		new_index_ptr.ptr = (void *) Z_LVAL_P(return_value);
		Z_TYPE(new_index_ptr) = le_index_ptr;
		if (zend_hash_update(&EG(regular_list), hashed_details, hashed_details_length+1,(void *) &new_index_ptr, sizeof(list_entry), NULL)==FAILURE) {
			efree(hashed_details);
			MYSQL_DO_CONNECT_RETURN_FALSE();
		}
		MySG(num_links)++;
	}

	efree(hashed_details);
	php_mysql_set_default_link(Z_LVAL_P(return_value) TSRMLS_CC);
	MYSQL_DO_CONNECT_CLEANUP();
}
/* }}} */

/* {{{ php_mysql_get_default_link
 */
static int php_mysql_get_default_link(INTERNAL_FUNCTION_PARAMETERS)
{
	if (MySG(default_link)==-1) { /* no link opened yet, implicitly open one */
		ht = 0;
		php_mysql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
	}
	return MySG(default_link);
}
/* }}} */

/* {{{ proto resource mysql_connect([string hostname[:port][:/path/to/socket]] [, string username] [, string password] [, bool new])
   Opens a connection to a MySQL Server */
PHP_FUNCTION(mysql_connect)
{
	php_mysql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto resource mysql_pconnect([string hostname[:port][:/path/to/socket]] [, string username] [, string password])
   Opens a persistent connection to a MySQL Server */
PHP_FUNCTION(mysql_pconnect)
{
	php_mysql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto bool mysql_close([int link_identifier])
   Close a MySQL connection */
PHP_FUNCTION(mysql_close)
{
	zval **mysql_link=NULL;
	int id;
	php_mysql_conn *mysql;

	switch (ZEND_NUM_ARGS()) {
		case 0:
			id = MySG(default_link);
			break;
		case 1:
			if (zend_get_parameters_ex(1, &mysql_link)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE2(mysql, php_mysql_conn *, mysql_link, id, "MySQL-Link", le_link, le_plink);

	if (id==-1) { /* explicit resource number */
		zend_list_delete(Z_RESVAL_PP(mysql_link));
	}

	if (id!=-1 
		|| (mysql_link && Z_RESVAL_PP(mysql_link)==MySG(default_link))) {
		zend_list_delete(MySG(default_link));
		MySG(default_link) = -1;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool mysql_select_db(string database_name [, int link_identifier [, bool return_prev_dbname]])
   Selects a MySQL database */
PHP_FUNCTION(mysql_select_db)
{
	zval **db, **mysql_link, **ret_prevdb;
	int id, ret_dbname=0;
	php_mysql_conn *mysql;
	char *prev_db=NULL;
	
	switch(ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &db)==FAILURE) {
				RETURN_FALSE;
			}
			id = php_mysql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			break;
		case 2:
			if (zend_get_parameters_ex(2, &db, &mysql_link)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		case 3:
			if (zend_get_parameters_ex(3, &db, &mysql_link, &ret_prevdb)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			convert_to_long_ex(ret_prevdb);
			ret_dbname = Z_LVAL_PP(ret_prevdb);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	ZEND_FETCH_RESOURCE2(mysql, php_mysql_conn *, mysql_link, id, "MySQL-Link", le_link, le_plink);
	
	convert_to_string_ex(db);

	/* Get the previous database name */
	if (ret_dbname && mysql->conn.db) {
		prev_db=estrdup(mysql->conn.db);
	}
	
	if (mysql_select_db(&mysql->conn, Z_STRVAL_PP(db))!=0) {
		RETVAL_FALSE;
	} else if (prev_db) {
		RETVAL_STRING(prev_db, 1);
	} else {
		RETVAL_TRUE;
	}

	if (prev_db) {
		efree(prev_db);
	}
}
/* }}} */

#ifdef HAVE_GETINFO_FUNCS

/* {{{ proto string mysql_get_client_info(void)
   Returns a string that represents the client library version */
PHP_FUNCTION(mysql_get_client_info)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	RETURN_STRING(mysql_get_client_info(),1);	
}
/* }}} */

/* {{{ proto string mysql_get_host_info([int link_identifier])
   Returns a string describing the type of connection in use, including the server host name */
PHP_FUNCTION(mysql_get_host_info)
{
	zval **mysql_link;
	int id;
	php_mysql_conn *mysql;

	switch(ZEND_NUM_ARGS()) {
		case 0:
			id = php_mysql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			break;
		case 1:
			if (zend_get_parameters_ex(1,&mysql_link)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	ZEND_FETCH_RESOURCE2(mysql, php_mysql_conn *, mysql_link, id, "MySQL-Link", le_link, le_plink);

	RETURN_STRING(mysql_get_host_info(&mysql->conn),1);
}
/* }}} */

/* {{{ proto int mysql_get_proto_info([int link_identifier])
   Returns the protocol version used by current connection */
PHP_FUNCTION(mysql_get_proto_info)
{
	zval **mysql_link;
	int id;
	php_mysql_conn *mysql;

	switch(ZEND_NUM_ARGS()) {
		case 0:
			id = php_mysql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			break;
		case 1:
			if (zend_get_parameters_ex(1,&mysql_link)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	ZEND_FETCH_RESOURCE2(mysql, php_mysql_conn *, mysql_link, id, "MySQL-Link", le_link, le_plink);

	RETURN_LONG(mysql_get_proto_info(&mysql->conn));
}
/* }}} */

/* {{{ proto string mysql_get_server_info([int link_identifier])
   Returns a string that represents the server version number */
PHP_FUNCTION(mysql_get_server_info)
{
	zval **mysql_link;
	int id;
	php_mysql_conn *mysql;

	switch(ZEND_NUM_ARGS()) {
		case 0:
			id = php_mysql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			break;
		case 1:
			if (zend_get_parameters_ex(1,&mysql_link)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	ZEND_FETCH_RESOURCE2(mysql, php_mysql_conn *, mysql_link, id, "MySQL-Link", le_link, le_plink);

	RETURN_STRING(mysql_get_server_info(&mysql->conn),1);
}
/* }}} */

/* {{{ proto string mysql_info([int link_identifier])
   Returns a string containing information about the most recent query */
PHP_FUNCTION(mysql_info)
{
	zval **mysql_link;
	int id;
	char *str;
	php_mysql_conn *mysql;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|r", &mysql_link) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS() == 0) {
		id = php_mysql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
		CHECK_LINK(id);
	}

	ZEND_FETCH_RESOURCE2(mysql, php_mysql_conn *, mysql_link, id, "MySQL-Link", le_link, le_plink);

	if ((str = mysql_info(&mysql->conn))) {
		RETURN_STRING(str,1);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int mysql_thread_id([int link_identifier])
	Returns the thread id of current connection */
PHP_FUNCTION(mysql_thread_id)
{
	zval *mysql_link = NULL;
	int  id = -1;
	php_mysql_conn *mysql;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|r", &mysql_link) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS() == 0) {
		id = php_mysql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
		CHECK_LINK(id);		
	}
	ZEND_FETCH_RESOURCE2(mysql, php_mysql_conn *, &mysql_link, id, "MySQL-Link", le_link, le_plink);

	RETURN_LONG(mysql_thread_id(&mysql->conn));
}
/* }}} */

/* {{{ proto string mysql_stat([int link_identifier])
	Returns a string containing status information */
PHP_FUNCTION(mysql_stat)
{
	zval *mysql_link = NULL;
	int id = -1;
	php_mysql_conn *mysql;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|r", &mysql_link) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS() == 0) {
		id = php_mysql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
		CHECK_LINK(id);		
	}
	ZEND_FETCH_RESOURCE2(mysql, php_mysql_conn *, &mysql_link, id, "MySQL-Link", le_link, le_plink);

	RETURN_STRING(mysql_stat(&mysql->conn), 1);
}
/* }}} */

/* {{{ proto string mysql_character_set_name([int link_identifier])
	Returns the default character set for the current connection */
PHP_FUNCTION(mysql_character_set_name)
{
	zval *mysql_link = NULL;
	int id = -1;
	php_mysql_conn *mysql;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|r", &mysql_link) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS() == 0) {
		id = php_mysql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
		CHECK_LINK(id);
	}

	ZEND_FETCH_RESOURCE2(mysql, php_mysql_conn *, &mysql_link, id, "MySQL-Link", le_link, le_plink);

	RETURN_STRING((char *)mysql_character_set_name(&mysql->conn), 1);
}
/* }}} */


#endif

#if MYSQL_VERSION_ID < 40000
/* {{{ proto bool mysql_create_db(string database_name [, int link_identifier])
   Create a MySQL database */
PHP_FUNCTION(mysql_create_db)
{
	zval **db, **mysql_link;
	int id;
	php_mysql_conn *mysql;

	switch(ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &db)==FAILURE) {
				RETURN_FALSE;
			}
			id = php_mysql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			break;
		case 2:
			if (zend_get_parameters_ex(2, &db, &mysql_link)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE2(mysql, php_mysql_conn *, mysql_link, id, "MySQL-Link", le_link, le_plink);
	
	convert_to_string_ex(db);
	if (mysql_create_db(&mysql->conn, Z_STRVAL_PP(db))==0) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool mysql_drop_db(string database_name [, int link_identifier])
   Drops (delete) a MySQL database */
PHP_FUNCTION(mysql_drop_db)
{
	zval **db, **mysql_link;
	int id;
	php_mysql_conn *mysql;
	
	switch(ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &db)==FAILURE) {
				RETURN_FALSE;
			}
			id = php_mysql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			break;
		case 2:
			if (zend_get_parameters_ex(2, &db, &mysql_link)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	
	ZEND_FETCH_RESOURCE2(mysql, php_mysql_conn *, mysql_link, id, "MySQL-Link", le_link, le_plink);
	
	convert_to_string_ex(db);
	if (mysql_drop_db(&mysql->conn, Z_STRVAL_PP(db))==0) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */
#endif

/* {{{ php_mysql_do_query_general
 */
static void php_mysql_do_query_general(zval **query, zval **mysql_link, int link_id, zval **db, int use_store, zval *return_value TSRMLS_DC)
{
	php_mysql_conn *mysql;
	MYSQL_RES *mysql_result;
	
	ZEND_FETCH_RESOURCE2(mysql, php_mysql_conn *, mysql_link, link_id, "MySQL-Link", le_link, le_plink);
	
	if (db) {
		convert_to_string_ex(db);
		if (mysql_select_db(&mysql->conn, Z_STRVAL_PP(db))!=0) {
			RETURN_FALSE;
		}
	}
	

	if (mysql->active_result_id) do {
		int type;
		MYSQL_RES *mysql_result;

		mysql_result = (MYSQL_RES *) zend_list_find(mysql->active_result_id, &type);
		if (mysql_result && type==le_result) {
			if (!mysql_eof(mysql_result)) {
				php_error(E_NOTICE, "%s(): Function called without first fetching all rows from a previous unbuffered query",
							get_active_function_name(TSRMLS_C));
				while (mysql_fetch_row(mysql_result));
			}
			zend_list_delete(mysql->active_result_id);
			mysql->active_result_id = 0;
		}
	} while(0);

	convert_to_string_ex(query);
	/* mysql_query is binary unsafe, use mysql_real_query */
#if MYSQL_VERSION_ID > 32199
	if (mysql_real_query(&mysql->conn, Z_STRVAL_PP(query), Z_STRLEN_PP(query))!=0) {
		RETURN_FALSE;
	}
#else
	if (mysql_query(&mysql->conn, Z_STRVAL_PP(query))!=0) {
		RETURN_FALSE;
	}
#endif
	if(use_store == MYSQL_USE_RESULT) {
		mysql_result=mysql_use_result(&mysql->conn);
	} else {
		mysql_result=mysql_store_result(&mysql->conn);
	}
	if (!mysql_result) {
		if (PHP_MYSQL_VALID_RESULT(&mysql->conn)) { /* query should have returned rows */
			php_error(E_WARNING, "%s(): Unable to save result set", get_active_function_name(TSRMLS_C));
			RETURN_FALSE;
		} else {
			RETURN_TRUE;
		}
	}
	ZEND_REGISTER_RESOURCE(return_value, mysql_result, le_result);
	if (use_store == MYSQL_USE_RESULT) {
		mysql->active_result_id = Z_LVAL_P(return_value);
	}
}
/* }}} */

/* {{{ php_mysql_do_query
 */
static void php_mysql_do_query(INTERNAL_FUNCTION_PARAMETERS, int use_store)
{
	zval **query, **mysql_link;
	int id;
	
	switch(ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &query)==FAILURE) {
				RETURN_FALSE;
			}
			id = php_mysql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			break;
		case 2:
			if (zend_get_parameters_ex(2, &query, &mysql_link)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	php_mysql_do_query_general(query, mysql_link, id, NULL, use_store, return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto resource mysql_query(string query [, int link_identifier] [, int result_mode])
   Sends an SQL query to MySQL */
PHP_FUNCTION(mysql_query)
{
	php_mysql_do_query(INTERNAL_FUNCTION_PARAM_PASSTHRU, MYSQL_STORE_RESULT);
}
/* }}} */


/* {{{ proto resource mysql_unbuffered_query(string query [, int link_identifier] [, int result_mode])
   Sends an SQL query to MySQL, without fetching and buffering the result rows */
PHP_FUNCTION(mysql_unbuffered_query)
{
	php_mysql_do_query(INTERNAL_FUNCTION_PARAM_PASSTHRU, MYSQL_USE_RESULT);
}
/* }}} */


/* {{{ proto resource mysql_db_query(string database_name, string query [, int link_identifier])
   Sends an SQL query to MySQL */
PHP_FUNCTION(mysql_db_query)
{
	zval **db, **query, **mysql_link;
	int id;
	
	switch(ZEND_NUM_ARGS()) {
		case 2:
			if (zend_get_parameters_ex(2, &db, &query)==FAILURE) {
				RETURN_FALSE;
			}
			id = php_mysql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			break;
		case 3:
			if (zend_get_parameters_ex(3, &db, &query, &mysql_link)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	zend_error(E_NOTICE, "%s is deprecated; use mysql_select_db() and mysql_query() instead", get_active_function_name(TSRMLS_C));
	
	php_mysql_do_query_general(query, mysql_link, id, db, MYSQL_STORE_RESULT, return_value TSRMLS_CC);
}
/* }}} */


/* {{{ proto resource mysql_list_dbs([int link_identifier])
   List databases available on a MySQL server */
PHP_FUNCTION(mysql_list_dbs)
{
	zval **mysql_link;
	int id;
	php_mysql_conn *mysql;
	MYSQL_RES *mysql_result;

	switch(ZEND_NUM_ARGS()) {
		case 0:
			id = php_mysql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			break;
		case 1:
			if (zend_get_parameters_ex(1, &mysql_link)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	ZEND_FETCH_RESOURCE2(mysql, php_mysql_conn *, mysql_link, id, "MySQL-Link", le_link, le_plink);

	if ((mysql_result=mysql_list_dbs(&mysql->conn, NULL))==NULL) {
		php_error(E_WARNING, "%s(): Unable to save MySQL query result", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}
	ZEND_REGISTER_RESOURCE(return_value, mysql_result, le_result);
}
/* }}} */


/* {{{ proto resource mysql_list_tables(string database_name [, int link_identifier])
   List tables in a MySQL database */
PHP_FUNCTION(mysql_list_tables)
{
	zval **db, **mysql_link;
	int id;
	php_mysql_conn *mysql;
	MYSQL_RES *mysql_result;
	
	switch(ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &db)==FAILURE) {
				RETURN_FALSE;
			}
			id = php_mysql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			break;
		case 2:
			if (zend_get_parameters_ex(2, &db, &mysql_link)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	ZEND_FETCH_RESOURCE2(mysql, php_mysql_conn *, mysql_link, id, "MySQL-Link", le_link, le_plink);

	convert_to_string_ex(db);
	if (mysql_select_db(&mysql->conn, Z_STRVAL_PP(db))!=0) {
		RETURN_FALSE;
	}
	if ((mysql_result=mysql_list_tables(&mysql->conn, NULL))==NULL) {
		php_error(E_WARNING, "%s(): Unable to save MySQL query result", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}
	ZEND_REGISTER_RESOURCE(return_value, mysql_result, le_result);
}
/* }}} */


/* {{{ proto resource mysql_list_fields(string database_name, string table_name [, int link_identifier])
   List MySQL result fields */
PHP_FUNCTION(mysql_list_fields)
{
	zval **db, **table, **mysql_link;
	int id;
	php_mysql_conn *mysql;
	MYSQL_RES *mysql_result;

	switch(ZEND_NUM_ARGS()) {
		case 2:
			if (zend_get_parameters_ex(2, &db, &table)==FAILURE) {
				RETURN_FALSE;
			}
			id = php_mysql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			CHECK_LINK(id);
			break;
		case 3:
			if (zend_get_parameters_ex(3, &db, &table, &mysql_link)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	ZEND_FETCH_RESOURCE2(mysql, php_mysql_conn *, mysql_link, id, "MySQL-Link", le_link, le_plink);

	convert_to_string_ex(db);
	if (mysql_select_db(&mysql->conn, Z_STRVAL_PP(db))!=0) {
		RETURN_FALSE;
	}
	convert_to_string_ex(table);
	if ((mysql_result=mysql_list_fields(&mysql->conn, Z_STRVAL_PP(table), NULL))==NULL) {
		php_error(E_WARNING, "%s(): Unable to save MySQL query result", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}
	ZEND_REGISTER_RESOURCE(return_value, mysql_result, le_result);
}
/* }}} */

/* {{{ proto resource mysql_list_processes([int link_identifier])
	Returns a result set describing the current server threads */
PHP_FUNCTION(mysql_list_processes)
{
	zval *mysql_link = NULL;
	int id = -1;
	php_mysql_conn *mysql;
	MYSQL_RES *mysql_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|r", &mysql_link) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS() == 0) {
		id = php_mysql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
		CHECK_LINK(id);
	}

	ZEND_FETCH_RESOURCE2(mysql, php_mysql_conn *, &mysql_link, id, "MySQL-Link", le_link, le_plink);

	mysql_result = mysql_list_processes(&mysql->conn);
	if (mysql_result == NULL) {
		php_error(E_WARNING, "%s(): Unable to save MySQL query result", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	ZEND_REGISTER_RESOURCE(return_value, mysql_result, le_result);
}
/* }}} */


/* {{{ proto string mysql_error([int link_identifier])
   Returns the text of the error message from previous MySQL operation */
PHP_FUNCTION(mysql_error)
{
	zval **mysql_link;
	int id;
	php_mysql_conn *mysql;
	
	switch(ZEND_NUM_ARGS()) {
		case 0:
			id = MySG(default_link);
			if (id==-1) {
				if (MySG(connect_error)!=NULL){
					RETURN_STRING(MySG(connect_error),1);
				} else {
					RETURN_FALSE;
				}
			}
			break;
		case 1:
			if (zend_get_parameters_ex(1, &mysql_link)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE2(mysql, php_mysql_conn *, mysql_link, id, "MySQL-Link", le_link, le_plink);
	
	RETURN_STRING(mysql_error(&mysql->conn), 1);
}
/* }}} */


/* {{{ proto int mysql_errno([int link_identifier])
   Returns the number of the error message from previous MySQL operation */
#ifdef HAVE_MYSQL_ERRNO
PHP_FUNCTION(mysql_errno)
{
	zval **mysql_link;
	int id;
	php_mysql_conn *mysql;
	
	switch(ZEND_NUM_ARGS()) {
		case 0:
			id = MySG(default_link);
			if (id==-1) {
			  	if (MySG(connect_errno)!=0){
					RETURN_LONG(MySG(connect_errno));
				} else {
					RETURN_FALSE;
				}
			}
			break;
		case 1:
			if (zend_get_parameters_ex(1, &mysql_link)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE2(mysql, php_mysql_conn *, mysql_link, id, "MySQL-Link", le_link, le_plink);
	
	RETURN_LONG(mysql_errno(&mysql->conn));
}
#endif
/* }}} */


/* {{{ proto int mysql_affected_rows([int link_identifier])
   Gets number of affected rows in previous MySQL operation */
PHP_FUNCTION(mysql_affected_rows)
{
	zval **mysql_link;
	int id;
	php_mysql_conn *mysql;
	
	switch(ZEND_NUM_ARGS()) {
		case 0:
			id = MySG(default_link);
			CHECK_LINK(id);
			break;
		case 1:
			if (zend_get_parameters_ex(1, &mysql_link)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE2(mysql, php_mysql_conn *, mysql_link, id, "MySQL-Link", le_link, le_plink);
	
	/* conversion from int64 to long happing here */
	Z_LVAL_P(return_value) = (long) mysql_affected_rows(&mysql->conn);
	Z_TYPE_P(return_value) = IS_LONG;
}
/* }}} */


/* {{{ proto string mysql_escape_string(string to_be_escaped)
   Escape string for mysql query */
PHP_FUNCTION(mysql_escape_string)
{
	zval **str;

	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &str) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	convert_to_string_ex(str);
	/* assume worst case situation, which is 2x of the original string.
	 * we don't realloc() down to the real size since it'd most probably not
	 * be worth it
	 */
	Z_STRVAL_P(return_value) = (char *) emalloc(Z_STRLEN_PP(str)*2+1);
	Z_STRLEN_P(return_value) = mysql_escape_string(Z_STRVAL_P(return_value), Z_STRVAL_PP(str), Z_STRLEN_PP(str));
	Z_TYPE_P(return_value) = IS_STRING;
}
/* }}} */

/* {{{ proto string mysql_real_escape_string(string to_be_escaped [, int link_identifier])
	Escape special characters in a string for use in a SQL statement, taking into account the current charset of the connection */
PHP_FUNCTION(mysql_real_escape_string)
{
	zval *mysql_link = NULL;
	char *str;
	char *new_str;
	int id = -1, str_len, new_str_len;
	php_mysql_conn *mysql;


	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|r", &str, &str_len, &mysql_link) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS() == 1) {
		id = php_mysql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
		CHECK_LINK(id);
	}

	ZEND_FETCH_RESOURCE2(mysql, php_mysql_conn *, &mysql_link, id, "MySQL-Link", le_link, le_plink);

	new_str = emalloc(str_len * 2 + 1);
	new_str_len = mysql_real_escape_string(&mysql->conn, new_str, str, str_len);
	new_str = erealloc(new_str, new_str_len);

	RETURN_STRINGL(new_str, new_str_len, 0);
}
/* }}} */

/* {{{ proto int mysql_insert_id([int link_identifier])
   Gets the ID generated from the previous INSERT operation */
PHP_FUNCTION(mysql_insert_id)
{
	zval **mysql_link;
	int id;
	php_mysql_conn *mysql;
	
	switch(ZEND_NUM_ARGS()) {
		case 0:
			id = MySG(default_link);
			CHECK_LINK(id);
			break;
		case 1:
			if (zend_get_parameters_ex(1, &mysql_link)==FAILURE) {
				RETURN_FALSE;
			}
			id = -1;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE2(mysql, php_mysql_conn *, mysql_link, id, "MySQL-Link", le_link, le_plink);
	
	/* conversion from int64 to long happing here */
	Z_LVAL_P(return_value) = (long) mysql_insert_id(&mysql->conn);
	Z_TYPE_P(return_value) = IS_LONG;
}
/* }}} */


/* {{{ proto mixed mysql_result(int result, int row [, mixed field])
   Gets result data */
PHP_FUNCTION(mysql_result)
{
	zval **result, **row, **field=NULL;
	MYSQL_RES *mysql_result;
	MYSQL_ROW sql_row;
	mysql_row_length_type *sql_row_lengths;
	int field_offset=0;

	switch (ZEND_NUM_ARGS()) {
		case 2:
			if (zend_get_parameters_ex(2, &result, &row)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 3:
			if (zend_get_parameters_ex(3, &result, &row, &field)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE(mysql_result, MYSQL_RES *, result, -1, "MySQL result", le_result);
		
	convert_to_long_ex(row);
	if (Z_LVAL_PP(row)<0 || Z_LVAL_PP(row)>=(int)mysql_num_rows(mysql_result)) {
		php_error(E_WARNING, "%s(): Unable to jump to row %d on MySQL result index %d", get_active_function_name(TSRMLS_C), Z_LVAL_PP(row), Z_LVAL_PP(result));
		RETURN_FALSE;
	}
	mysql_data_seek(mysql_result, Z_LVAL_PP(row));
	if ((sql_row=mysql_fetch_row(mysql_result))==NULL 
		|| (sql_row_lengths=mysql_fetch_lengths(mysql_result))==NULL) { /* shouldn't happen? */
		RETURN_FALSE;
	}
	
	if (field) {
		switch(Z_TYPE_PP(field)) {
			case IS_STRING: {
					int i=0;
					MYSQL_FIELD *tmp_field;
					char *table_name, *field_name, *tmp;

					if ((tmp=strchr(Z_STRVAL_PP(field), '.'))) {
						table_name = estrndup(Z_STRVAL_PP(field), tmp-Z_STRVAL_PP(field));
						field_name = estrdup(tmp+1);
					} else {
						table_name = NULL;
						field_name = estrndup(Z_STRVAL_PP(field),Z_STRLEN_PP(field));
					}
					mysql_field_seek(mysql_result, 0);
					while ((tmp_field=mysql_fetch_field(mysql_result))) {
						if ((!table_name || !strcasecmp(tmp_field->table, table_name)) && !strcasecmp(tmp_field->name, field_name)) {
							field_offset = i;
							break;
						}
						i++;
					}
					if (!tmp_field) { /* no match found */
						php_error(E_WARNING, "%s(): %s%s%s not found in MySQL result index %d", get_active_function_name(TSRMLS_C),
									(table_name?table_name:""), (table_name?".":""), field_name, Z_LVAL_PP(result));
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
				convert_to_long_ex(field);
				field_offset = Z_LVAL_PP(field);
				if (field_offset<0 || field_offset>=(int)mysql_num_fields(mysql_result)) {
					php_error(E_WARNING, "%s(): Bad column offset specified", get_active_function_name(TSRMLS_C));
					RETURN_FALSE;
				}
				break;
		}
	}

	if (sql_row[field_offset]) {
		Z_TYPE_P(return_value) = IS_STRING;

		if (PG(magic_quotes_runtime)) {
			Z_STRVAL_P(return_value) = php_addslashes(sql_row[field_offset], sql_row_lengths[field_offset],&Z_STRLEN_P(return_value), 0 TSRMLS_CC);
		} else {	
			Z_STRLEN_P(return_value) = sql_row_lengths[field_offset];
			Z_STRVAL_P(return_value) = (char *) safe_estrndup(sql_row[field_offset], Z_STRLEN_P(return_value));
		}
	} else {
		Z_TYPE_P(return_value) = IS_NULL;
	}
}
/* }}} */


/* {{{ proto int mysql_num_rows(int result)
   Gets number of rows in a result */
PHP_FUNCTION(mysql_num_rows)
{
	zval **result;
	MYSQL_RES *mysql_result;
	
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(mysql_result, MYSQL_RES *, result, -1, "MySQL result", le_result);
	
	/* conversion from int64 to long happing here */
	Z_LVAL_P(return_value) = (long) mysql_num_rows(mysql_result);
	Z_TYPE_P(return_value) = IS_LONG;
}
/* }}} */

/* {{{ proto int mysql_num_fields(int result)
   Gets number of fields in a result */
PHP_FUNCTION(mysql_num_fields)
{
	zval **result;
	MYSQL_RES *mysql_result;
	
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(mysql_result, MYSQL_RES *, result, -1, "MySQL result", le_result);
	
	Z_LVAL_P(return_value) = mysql_num_fields(mysql_result);
	Z_TYPE_P(return_value) = IS_LONG;
}
/* }}} */

/* {{{ php_mysql_fetch_hash
 */
static void php_mysql_fetch_hash(INTERNAL_FUNCTION_PARAMETERS, int result_type, int expected_args)
{
	zval **result, **arg2;
	MYSQL_RES *mysql_result;
	MYSQL_ROW mysql_row;
	MYSQL_FIELD *mysql_field;
	mysql_row_length_type *mysql_row_lengths;
	int num_fields;
	int i;

	if (ZEND_NUM_ARGS() > expected_args) {
		WRONG_PARAM_COUNT;
	}

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &result)==FAILURE) {
				RETURN_FALSE;
			}
			if (!result_type) {
				result_type = MYSQL_BOTH;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &result, &arg2)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long_ex(arg2);
			result_type = Z_LVAL_PP(arg2);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ZEND_FETCH_RESOURCE(mysql_result, MYSQL_RES *, result, -1, "MySQL result", le_result);

	if ((mysql_row=mysql_fetch_row(mysql_result))==NULL 
		|| (mysql_row_lengths=mysql_fetch_lengths(mysql_result))==NULL) {
		RETURN_FALSE;
	}

	num_fields = mysql_num_fields(mysql_result);
	
	if (array_init(return_value)==FAILURE) {
		RETURN_FALSE;
	}
	
	mysql_field_seek(mysql_result, 0);
	for (mysql_field=mysql_fetch_field(mysql_result), i=0; mysql_field; mysql_field=mysql_fetch_field(mysql_result), i++) {
		if (mysql_row[i]) {
			char *data;
			int data_len;
			int should_copy;

			if (PG(magic_quotes_runtime)) {
				data = php_addslashes(mysql_row[i], mysql_row_lengths[i],&data_len, 0 TSRMLS_CC);
				should_copy = 0;
			} else {
				data = mysql_row[i];
				data_len = mysql_row_lengths[i];
				should_copy = 1;
			}
			
			if (result_type & MYSQL_NUM) {
				add_index_stringl(return_value, i, data, data_len, should_copy);
				should_copy = 1;
			}
			
			if (result_type & MYSQL_ASSOC) {
				add_assoc_stringl(return_value, mysql_field->name, data, data_len, should_copy);
			}
		} else {
			/* NULL value. */
			if (result_type & MYSQL_NUM) {
				add_index_null(return_value, i);
			}

			if (result_type & MYSQL_ASSOC) {
				add_assoc_null(return_value, mysql_field->name);
			}
		}
	}
}
/* }}} */

/* {{{ proto array mysql_fetch_row(int result)
   Gets a result row as an enumerated array */
PHP_FUNCTION(mysql_fetch_row)
{
	php_mysql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, MYSQL_NUM, 1);
}
/* }}} */


/* {{{ proto object mysql_fetch_object(int result [, int result_type])
   Fetch a result row as an object */
PHP_FUNCTION(mysql_fetch_object)
{
	php_mysql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, MYSQL_ASSOC, 2);

	if (Z_TYPE_P(return_value) == IS_ARRAY) {
		object_and_properties_init(return_value, ZEND_STANDARD_CLASS_DEF_PTR, Z_ARRVAL_P(return_value));
	}
}
/* }}} */


/* {{{ proto array mysql_fetch_array(int result [, int result_type])
   Fetch a result row as an array (associative, numeric or both) */
PHP_FUNCTION(mysql_fetch_array)
{
	php_mysql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, 2);
}
/* }}} */


/* {{{ proto array mysql_fetch_assoc(int result)
   Fetch a result row as an associative array */
PHP_FUNCTION(mysql_fetch_assoc)
{
	php_mysql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, MYSQL_ASSOC, 1);
}
/* }}} */

/* {{{ proto bool mysql_data_seek(int result, int row_number)
   Move internal result pointer */
PHP_FUNCTION(mysql_data_seek)
{
	zval **result, **offset;
	MYSQL_RES *mysql_result;
	
	if (ZEND_NUM_ARGS()!=2 || zend_get_parameters_ex(2, &result, &offset)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(mysql_result, MYSQL_RES *, result, -1, "MySQL result", le_result);

	convert_to_long_ex(offset);
	if (Z_LVAL_PP(offset)<0 || Z_LVAL_PP(offset)>=(int)mysql_num_rows(mysql_result)) {
		php_error(E_WARNING, "%s(): Offset %d is invalid for MySQL result index %d (or the query data is unbuffered)", get_active_function_name(TSRMLS_C), Z_LVAL_PP(offset), Z_LVAL_PP(result));
		RETURN_FALSE;
	}
	mysql_data_seek(mysql_result, Z_LVAL_PP(offset));
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto array mysql_fetch_lengths(int result)
   Gets max data size of each column in a result */
PHP_FUNCTION(mysql_fetch_lengths)
{
	zval **result;
	MYSQL_RES *mysql_result;
	mysql_row_length_type *lengths;
	int num_fields;
	int i;
	
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(mysql_result, MYSQL_RES *, result, -1, "MySQL result", le_result);

	if ((lengths=mysql_fetch_lengths(mysql_result))==NULL) {
		RETURN_FALSE;
	}
	if (array_init(return_value)==FAILURE) {
		RETURN_FALSE;
	}
	num_fields = mysql_num_fields(mysql_result);
	
	for (i=0; i<num_fields; i++) {
		add_index_long(return_value, i, lengths[i]);
	}
}
/* }}} */

/* {{{ php_mysql_get_field_name
 */
static char *php_mysql_get_field_name(int field_type)
{
	switch(field_type) {
		case FIELD_TYPE_STRING:
		case FIELD_TYPE_VAR_STRING:
			return "string";
			break;
#ifdef MYSQL_HAS_TINY
		case FIELD_TYPE_TINY:
#endif
		case FIELD_TYPE_SHORT:
		case FIELD_TYPE_LONG:
		case FIELD_TYPE_LONGLONG:
		case FIELD_TYPE_INT24:
			return "int";
			break;
		case FIELD_TYPE_FLOAT:
		case FIELD_TYPE_DOUBLE:
		case FIELD_TYPE_DECIMAL:
			return "real";
			break;
		case FIELD_TYPE_TIMESTAMP:
			return "timestamp";
			break;
#ifdef MYSQL_HAS_YEAR
		case FIELD_TYPE_YEAR:
			return "year";
			break;
#endif
		case FIELD_TYPE_DATE:
			return "date";
			break;
		case FIELD_TYPE_TIME:
			return "time";
			break;
		case FIELD_TYPE_DATETIME:
			return "datetime";
			break;
		case FIELD_TYPE_TINY_BLOB:
		case FIELD_TYPE_MEDIUM_BLOB:
		case FIELD_TYPE_LONG_BLOB:
		case FIELD_TYPE_BLOB:
			return "blob";
			break;
		case FIELD_TYPE_NULL:
			return "null";
			break;
		default:
			return "unknown";
			break;
	}
}
/* }}} */

/* {{{ proto object mysql_fetch_field(int result [, int field_offset])
   Gets column information from a result and return as an object */
PHP_FUNCTION(mysql_fetch_field)
{
	zval **result, **field=NULL;
	MYSQL_RES *mysql_result;
	MYSQL_FIELD *mysql_field;
	
	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &result)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &result, &field)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long_ex(field);
			break;
		default:
			WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(mysql_result, MYSQL_RES *, result, -1, "MySQL result", le_result);

	if (field) {
		if (Z_LVAL_PP(field)<0 || Z_LVAL_PP(field)>=(int)mysql_num_fields(mysql_result)) {
			php_error(E_WARNING, "%s(): Bad field offset", get_active_function_name(TSRMLS_C));
			RETURN_FALSE;
		}
		mysql_field_seek(mysql_result, Z_LVAL_PP(field));
	}
	if ((mysql_field=mysql_fetch_field(mysql_result))==NULL) {
		RETURN_FALSE;
	}
	if (object_init(return_value)==FAILURE) {
		RETURN_FALSE;
	}

	add_property_string(return_value, "name",(mysql_field->name?mysql_field->name:empty_string), 1);
	add_property_string(return_value, "table",(mysql_field->table?mysql_field->table:empty_string), 1);
	add_property_string(return_value, "def",(mysql_field->def?mysql_field->def:empty_string), 1);
	add_property_long(return_value, "max_length", mysql_field->max_length);
	add_property_long(return_value, "not_null", IS_NOT_NULL(mysql_field->flags)?1:0);
	add_property_long(return_value, "primary_key", IS_PRI_KEY(mysql_field->flags)?1:0);
	add_property_long(return_value, "multiple_key",(mysql_field->flags&MULTIPLE_KEY_FLAG?1:0));
	add_property_long(return_value, "unique_key",(mysql_field->flags&UNIQUE_KEY_FLAG?1:0));
	add_property_long(return_value, "numeric", IS_NUM(Z_TYPE_P(mysql_field))?1:0);
	add_property_long(return_value, "blob", IS_BLOB(mysql_field->flags)?1:0);
	add_property_string(return_value, "type", php_mysql_get_field_name(Z_TYPE_P(mysql_field)), 1);
	add_property_long(return_value, "unsigned",(mysql_field->flags&UNSIGNED_FLAG?1:0));
	add_property_long(return_value, "zerofill",(mysql_field->flags&ZEROFILL_FLAG?1:0));
}
/* }}} */


/* {{{ proto bool mysql_field_seek(int result, int field_offset)
   Sets result pointer to a specific field offset */
PHP_FUNCTION(mysql_field_seek)
{
	zval **result, **offset;
	MYSQL_RES *mysql_result;
	
	if (ZEND_NUM_ARGS()!=2 || zend_get_parameters_ex(2, &result, &offset)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(mysql_result, MYSQL_RES *, result, -1, "MySQL result", le_result);

	convert_to_long_ex(offset);
	if (Z_LVAL_PP(offset)<0 || Z_LVAL_PP(offset)>=(int)mysql_num_fields(mysql_result)) {
		php_error(E_WARNING, "%s(): Field %d is invalid for MySQL result index %d", get_active_function_name(TSRMLS_C), Z_LVAL_PP(offset), Z_LVAL_PP(result));
		RETURN_FALSE;
	}
	mysql_field_seek(mysql_result, Z_LVAL_PP(offset));
	RETURN_TRUE;
}
/* }}} */


#define PHP_MYSQL_FIELD_NAME 1
#define PHP_MYSQL_FIELD_TABLE 2
#define PHP_MYSQL_FIELD_LEN 3
#define PHP_MYSQL_FIELD_TYPE 4
#define PHP_MYSQL_FIELD_FLAGS 5
 
/* {{{ php_mysql_field_info
 */
static void php_mysql_field_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type)
{
	zval **result, **field;
	MYSQL_RES *mysql_result;
	MYSQL_FIELD *mysql_field;
	char buf[512];
	int  len;

	if (ZEND_NUM_ARGS()!=2 || zend_get_parameters_ex(2, &result, &field)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(mysql_result, MYSQL_RES *, result, -1, "MySQL result", le_result);
	
	convert_to_long_ex(field);
	if (Z_LVAL_PP(field)<0 || Z_LVAL_PP(field)>=(int)mysql_num_fields(mysql_result)) {
		php_error(E_WARNING, "%s(): Field %d is invalid for MySQL result index %d", get_active_function_name(TSRMLS_C), Z_LVAL_PP(field), Z_LVAL_PP(result));
		RETURN_FALSE;
	}
	mysql_field_seek(mysql_result, Z_LVAL_PP(field));
	if ((mysql_field=mysql_fetch_field(mysql_result))==NULL) {
		RETURN_FALSE;
	}
	
	switch (entry_type) {
		case PHP_MYSQL_FIELD_NAME:
			Z_STRLEN_P(return_value) = strlen(mysql_field->name);
			Z_STRVAL_P(return_value) = estrndup(mysql_field->name, Z_STRLEN_P(return_value));
			Z_TYPE_P(return_value) = IS_STRING;
			break;
		case PHP_MYSQL_FIELD_TABLE:
			Z_STRLEN_P(return_value) = strlen(mysql_field->table);
			Z_STRVAL_P(return_value) = estrndup(mysql_field->table, Z_STRLEN_P(return_value));
			Z_TYPE_P(return_value) = IS_STRING;
			break;
		case PHP_MYSQL_FIELD_LEN:
			Z_LVAL_P(return_value) = mysql_field->length;
			Z_TYPE_P(return_value) = IS_LONG;
			break;
		case PHP_MYSQL_FIELD_TYPE:
			Z_STRVAL_P(return_value) = php_mysql_get_field_name(Z_TYPE_P(mysql_field));
			Z_STRLEN_P(return_value) = strlen(Z_STRVAL_P(return_value));
			Z_STRVAL_P(return_value) = estrndup(Z_STRVAL_P(return_value), Z_STRLEN_P(return_value));
			Z_TYPE_P(return_value) = IS_STRING;
			break;
		case PHP_MYSQL_FIELD_FLAGS:
			strcpy(buf, "");
#ifdef IS_NOT_NULL
			if (IS_NOT_NULL(mysql_field->flags)) {
				strcat(buf, "not_null ");
			}
#endif
#ifdef IS_PRI_KEY
			if (IS_PRI_KEY(mysql_field->flags)) {
				strcat(buf, "primary_key ");
			}
#endif
#ifdef UNIQUE_KEY_FLAG
			if (mysql_field->flags&UNIQUE_KEY_FLAG) {
				strcat(buf, "unique_key ");
			}
#endif
#ifdef MULTIPLE_KEY_FLAG
			if (mysql_field->flags&MULTIPLE_KEY_FLAG) {
				strcat(buf, "multiple_key ");
			}
#endif
#ifdef IS_BLOB
			if (IS_BLOB(mysql_field->flags)) {
				strcat(buf, "blob ");
			}
#endif
#ifdef UNSIGNED_FLAG
			if (mysql_field->flags&UNSIGNED_FLAG) {
				strcat(buf, "unsigned ");
			}
#endif
#ifdef ZEROFILL_FLAG
			if (mysql_field->flags&ZEROFILL_FLAG) {
				strcat(buf, "zerofill ");
			}
#endif
#ifdef BINARY_FLAG
			if (mysql_field->flags&BINARY_FLAG) {
				strcat(buf, "binary ");
			}
#endif
#ifdef ENUM_FLAG
			if (mysql_field->flags&ENUM_FLAG) {
				strcat(buf, "enum ");
			}
#endif
#ifdef SET_FLAG
			if (mysql_field->flags&SET_FLAG) {
				strcat(buf, "set ");
			}
#endif
#ifdef AUTO_INCREMENT_FLAG
			if (mysql_field->flags&AUTO_INCREMENT_FLAG) {
				strcat(buf, "auto_increment ");
			}
#endif
#ifdef TIMESTAMP_FLAG
			if (mysql_field->flags&TIMESTAMP_FLAG) {
				strcat(buf, "timestamp ");
			}
#endif
			len = strlen(buf);
			/* remove trailing space, if present */
			if (len && buf[len-1] == ' ') {
				buf[len-1] = 0;
				len--;
			}
			
	   		Z_STRLEN_P(return_value) = len;
   			Z_STRVAL_P(return_value) = estrndup(buf, len);
   			Z_TYPE_P(return_value) = IS_STRING;
			break;

		default:
			RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string mysql_field_name(int result, int field_index)
   Gets the name of the specified field in a result */
PHP_FUNCTION(mysql_field_name)
{
	php_mysql_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_MYSQL_FIELD_NAME);
}
/* }}} */


/* {{{ proto string mysql_field_table(int result, int field_offset)
   Gets name of the table the specified field is in */
PHP_FUNCTION(mysql_field_table)
{
	php_mysql_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_MYSQL_FIELD_TABLE);
}
/* }}} */


/* {{{ proto int mysql_field_len(int result, int field_offset)
   Returns the length of the specified field */
PHP_FUNCTION(mysql_field_len)
{
	php_mysql_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_MYSQL_FIELD_LEN);
}
/* }}} */


/* {{{ proto string mysql_field_type(int result, int field_offset)
   Gets the type of the specified field in a result */
PHP_FUNCTION(mysql_field_type)
{
	php_mysql_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_MYSQL_FIELD_TYPE);
}
/* }}} */


/* {{{ proto string mysql_field_flags(int result, int field_offset)
   Gets the flags associated with the specified field in a result */
PHP_FUNCTION(mysql_field_flags)
{
	php_mysql_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_MYSQL_FIELD_FLAGS);
}
/* }}} */


/* {{{ proto bool mysql_free_result(int result)
   Free result memory */
PHP_FUNCTION(mysql_free_result)
{
	zval **result;
	MYSQL_RES *mysql_result;

	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (Z_TYPE_PP(result)==IS_RESOURCE && Z_LVAL_PP(result)==0) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(mysql_result, MYSQL_RES *, result, -1, "MySQL result", le_result);

	zend_list_delete(Z_LVAL_PP(result));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool mysql_ping([int link_identifier])
   Ping a server connection. If no connection then reconnect. */
PHP_FUNCTION(mysql_ping)
{
	zval           *mysql_link = NULL;
	int             id         = -1;
	php_mysql_conn *mysql;

	if (0 == ZEND_NUM_ARGS()) {
		id = php_mysql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
		CHECK_LINK(id);
	} else if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &mysql_link)==FAILURE) {
		return;
	}
	
	ZEND_FETCH_RESOURCE2(mysql, php_mysql_conn *, &mysql_link, id, "MySQL-Link", le_link, le_plink);
	RETURN_BOOL(! mysql_ping(&mysql->conn));
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
