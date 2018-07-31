/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2018 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Georg Richter <georg@php.net>                               |
  |          Andrey Hristov <andrey@php.net>                             |
  |          Ulf Wendel <uw@php.net>                                     |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <signal.h>

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_smart_str.h"
#include "php_mysqli_structs.h"
#include "mysqli_priv.h"

#define SAFE_STR(a) ((a)?a:"")

/* {{{ php_mysqli_set_error
 */
static void php_mysqli_set_error(zend_long mysql_errno, char *mysql_err)
{
	MyG(error_no) = mysql_errno;
	if (MyG(error_msg)) {
		efree(MyG(error_msg));
	}
	if(mysql_err && *mysql_err) {
		MyG(error_msg) = estrdup(mysql_err);
	} else {
		MyG(error_msg) = NULL;
	}
}
/* }}} */

void mysqli_common_connect(INTERNAL_FUNCTION_PARAMETERS, zend_bool is_real_connect, zend_bool in_ctor) /* {{{ */
{
	MY_MYSQL			*mysql = NULL;
	MYSQLI_RESOURCE		*mysqli_resource = NULL;
	zval				*object = getThis();
	char				*hostname = NULL, *username=NULL, *passwd=NULL, *dbname=NULL, *socket=NULL;
	size_t					hostname_len = 0, username_len = 0, passwd_len = 0, dbname_len = 0, socket_len = 0;
	zend_bool			persistent = FALSE;
	zend_long				port = 0, flags = 0;
	zend_string			*hash_key = NULL;
	zend_bool			new_connection = FALSE;
	zend_resource		*le;
	mysqli_plist_entry *plist = NULL;
	zend_bool			self_alloced = 0;


#if !defined(MYSQL_USE_MYSQLND)
	if ((MYSQL_VERSION_ID / 100) != (mysql_get_client_version() / 100)) {
		php_error_docref(NULL, E_WARNING,
						"Headers and client library minor version mismatch. Headers:%d Library:%ld",
						MYSQL_VERSION_ID, mysql_get_client_version());
	}
#endif

	if (getThis() && !ZEND_NUM_ARGS() && in_ctor) {
		php_mysqli_init(INTERNAL_FUNCTION_PARAM_PASSTHRU, in_ctor);
		return;
	}
	hostname = username = dbname = passwd = socket = NULL;

	if (!is_real_connect) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "|ssssls", &hostname, &hostname_len, &username, &username_len,
									&passwd, &passwd_len, &dbname, &dbname_len, &port, &socket, &socket_len) == FAILURE) {
			return;
		}

		if (object && instanceof_function(Z_OBJCE_P(object), mysqli_link_class_entry)) {
			mysqli_resource = (Z_MYSQLI_P(object))->ptr;
			if (mysqli_resource && mysqli_resource->ptr) {
				mysql = (MY_MYSQL*) mysqli_resource->ptr;
			}
		}
		if (!mysql) {
			mysql = (MY_MYSQL *) ecalloc(1, sizeof(MY_MYSQL));
			self_alloced = 1;
		}
		flags |= CLIENT_MULTI_RESULTS; /* needed for mysql_multi_query() */
	} else {
		/* We have flags too */
		if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O|sssslsl", &object, mysqli_link_class_entry,
										&hostname, &hostname_len, &username, &username_len, &passwd, &passwd_len, &dbname, &dbname_len, &port, &socket, &socket_len,
										&flags) == FAILURE) {
			return;
		}

		mysqli_resource = (Z_MYSQLI_P(object))->ptr;
		MYSQLI_FETCH_RESOURCE_CONN(mysql, object, MYSQLI_STATUS_INITIALIZED);

		/* set some required options */
		flags |= CLIENT_MULTI_RESULTS; /* needed for mysql_multi_query() */
		/* remove some insecure options */
		flags &= ~CLIENT_MULTI_STATEMENTS;   /* don't allow multi_queries via connect parameter */
#if !defined(MYSQLI_USE_MYSQLND)
		if (PG(open_basedir) && PG(open_basedir)[0] != '\0') {
			flags &= ~CLIENT_LOCAL_FILES;
		}
#endif
	}

	if (!socket_len || !socket) {
		socket = MyG(default_socket);
	}
	if (!port){
		port = MyG(default_port);
	}
	if (!passwd) {
		passwd = MyG(default_pw);
		passwd_len = strlen(SAFE_STR(passwd));
	}
	if (!username){
		username = MyG(default_user);
	}
	if (!hostname || !hostname_len) {
		hostname = MyG(default_host);
	}

	if (mysql->mysql && mysqli_resource &&
		(mysqli_resource->status > MYSQLI_STATUS_INITIALIZED))
	{
		/* already connected, we should close the connection */
		php_mysqli_close(mysql, MYSQLI_CLOSE_IMPLICIT, mysqli_resource->status);
	}

	if (strlen(SAFE_STR(hostname)) > 2 && !strncasecmp(hostname, "p:", 2)) {
		hostname += 2;
		if (!MyG(allow_persistent)) {
			php_error_docref(NULL, E_WARNING, "Persistent connections are disabled. Downgrading to normal");
		} else {
			mysql->persistent = persistent = TRUE;

			hash_key = strpprintf(0, "mysqli_%s_%s" ZEND_LONG_FMT "%s%s%s", SAFE_STR(hostname), SAFE_STR(socket),
								port, SAFE_STR(username), SAFE_STR(dbname),
								SAFE_STR(passwd));

			mysql->hash_key = hash_key;

			/* check if we can reuse exisiting connection ... */
			if ((le = zend_hash_find_ptr(&EG(persistent_list), hash_key)) != NULL) {
				if (le->type == php_le_pmysqli()) {
					plist = (mysqli_plist_entry *) le->ptr;

					do {
						if (zend_ptr_stack_num_elements(&plist->free_links)) {
							mysql->mysql = zend_ptr_stack_pop(&plist->free_links);

							MyG(num_inactive_persistent)--;
							/* reset variables */

#ifndef MYSQLI_NO_CHANGE_USER_ON_PCONNECT
							if (!mysqli_change_user_silent(mysql->mysql, username, passwd, dbname, passwd_len)) {
#else
							if (!mysql_ping(mysql->mysql)) {
#endif
#ifdef MYSQLI_USE_MYSQLND
								mysqlnd_restart_psession(mysql->mysql);
#endif
								MyG(num_active_persistent)++;

								/* clear error */
								php_mysqli_set_error(mysql_errno(mysql->mysql), (char *) mysql_error(mysql->mysql));

								goto end;
							} else {
								mysqli_close(mysql->mysql, MYSQLI_CLOSE_IMPLICIT);
								mysql->mysql = NULL;
							}
						}
					} while (0);
				}
			} else {
				plist = calloc(1, sizeof(mysqli_plist_entry));

				zend_ptr_stack_init_ex(&plist->free_links, 1);
				zend_register_persistent_resource(ZSTR_VAL(hash_key), ZSTR_LEN(hash_key), plist, php_le_pmysqli());
			}
		}
	}
	if (MyG(max_links) != -1 && MyG(num_links) >= MyG(max_links)) {
		php_error_docref(NULL, E_WARNING, "Too many open links (" ZEND_LONG_FMT ")", MyG(num_links));
		goto err;
	}

	if (persistent && MyG(max_persistent) != -1 &&
		(MyG(num_active_persistent) + MyG(num_inactive_persistent))>= MyG(max_persistent))
	{
		php_error_docref(NULL, E_WARNING, "Too many open persistent links (" ZEND_LONG_FMT ")",
								MyG(num_active_persistent) + MyG(num_inactive_persistent));
		goto err;
	}
	if (!mysql->mysql) {
#if !defined(MYSQLI_USE_MYSQLND)
		if (!(mysql->mysql = mysql_init(NULL))) {
#else
		if (!(mysql->mysql = mysqlnd_init(MYSQLND_CLIENT_KNOWS_RSET_COPY_DATA, persistent))) {
#endif
			goto err;
		}
		new_connection = TRUE;
	}

#ifdef HAVE_EMBEDDED_MYSQLI
	if (hostname_len) {
		unsigned int external=1;
		mysql_options(mysql->mysql, MYSQL_OPT_USE_REMOTE_CONNECTION, (char *)&external);
	} else {
		mysql_options(mysql->mysql, MYSQL_OPT_USE_EMBEDDED_CONNECTION, 0);
	}
#endif

#if !defined(MYSQLI_USE_MYSQLND)
	/* BC for prior to bug fix #53425 */
	flags |= CLIENT_MULTI_RESULTS;

	if (mysql_real_connect(mysql->mysql, hostname, username, passwd, dbname, port, socket, flags) == NULL)
#else
	if (mysqlnd_connect(mysql->mysql, hostname, username, passwd, passwd_len, dbname, dbname_len,
						port, socket, flags, MYSQLND_CLIENT_KNOWS_RSET_COPY_DATA) == NULL)
#endif
	{
		/* Save error messages - for mysqli_connect_error() & mysqli_connect_errno() */
		php_mysqli_set_error(mysql_errno(mysql->mysql), (char *) mysql_error(mysql->mysql));
		php_mysqli_throw_sql_exception((char *)mysql_sqlstate(mysql->mysql), mysql_errno(mysql->mysql),
										"%s", mysql_error(mysql->mysql));
		if (!is_real_connect) {
			/* free mysql structure */
			mysqli_close(mysql->mysql, MYSQLI_CLOSE_DISCONNECTED);
			mysql->mysql = NULL;
		}
		goto err;
	}

	/* clear error */
	php_mysqli_set_error(mysql_errno(mysql->mysql), (char *) mysql_error(mysql->mysql));

#if !defined(MYSQLI_USE_MYSQLND)
	mysql->mysql->reconnect = MyG(reconnect);
#endif

	mysql_options(mysql->mysql, MYSQL_OPT_LOCAL_INFILE, (char *)&MyG(allow_local_infile));

end:
	if (!mysqli_resource) {
		mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
		mysqli_resource->ptr = (void *)mysql;
	}
	mysqli_resource->status = MYSQLI_STATUS_VALID;

	/* store persistent connection */
	if (persistent && (new_connection || is_real_connect)) {
		MyG(num_active_persistent)++;
	}

	MyG(num_links)++;

	mysql->multi_query = 0;

	if (!object || !instanceof_function(Z_OBJCE_P(object), mysqli_link_class_entry)) {
		MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_link_class_entry);
	} else {
		(Z_MYSQLI_P(object))->ptr = mysqli_resource;
	}
	if (!is_real_connect) {
		return;
	} else {
		RETURN_TRUE;
	}

err:
	if (mysql->hash_key) {
		zend_string_release_ex(mysql->hash_key, 0);
		mysql->hash_key = NULL;
		mysql->persistent = FALSE;
	}
	if (!is_real_connect && self_alloced) {
		efree(mysql);
	}
	RETVAL_FALSE;
} /* }}} */

/* {{{ proto object mysqli_connect([string hostname [,string username [,string passwd [,string dbname [,int port [,string socket]]]]]])
   Open a connection to a mysql server */
PHP_FUNCTION(mysqli_connect)
{
	mysqli_common_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, FALSE, FALSE);
}
/* }}} */

/* {{{ proto object mysqli_link_construct()
  */
PHP_FUNCTION(mysqli_link_construct)
{
	mysqli_common_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, FALSE, TRUE);
}
/* }}} */

/* {{{ proto int mysqli_connect_errno(void)
   Returns the numerical value of the error message from last connect command */
PHP_FUNCTION(mysqli_connect_errno)
{
	RETURN_LONG(MyG(error_no));
}
/* }}} */

/* {{{ proto string mysqli_connect_error(void)
   Returns the text of the error message from previous MySQL operation */
PHP_FUNCTION(mysqli_connect_error)
{
	if (MyG(error_msg)) {
		RETURN_STRING(MyG(error_msg));
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ proto mixed mysqli_fetch_array(object result [,int resulttype])
   Fetch a result row as an associative array, a numeric array, or both */
PHP_FUNCTION(mysqli_fetch_array)
{
	php_mysqli_fetch_into_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, 0);
}
/* }}} */

/* {{{ proto mixed mysqli_fetch_assoc(object result)
   Fetch a result row as an associative array */
PHP_FUNCTION(mysqli_fetch_assoc)
{
	php_mysqli_fetch_into_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, MYSQLI_ASSOC, 0);
}
/* }}} */

/* {{{ proto mixed mysqli_fetch_all(object result [,int resulttype])
   Fetches all result rows as an associative array, a numeric array, or both */
#if defined(MYSQLI_USE_MYSQLND)
PHP_FUNCTION(mysqli_fetch_all)
{
	MYSQL_RES	*result;
	zval		*mysql_result;
	zend_long		mode = MYSQLND_FETCH_NUM;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O|l", &mysql_result, mysqli_result_class_entry, &mode) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, mysql_result, "mysqli_result", MYSQLI_STATUS_VALID);

	if (!mode || (mode & ~MYSQLND_FETCH_BOTH)) {
		php_error_docref(NULL, E_WARNING, "Mode can be only MYSQLI_FETCH_NUM, "
		                 "MYSQLI_FETCH_ASSOC or MYSQLI_FETCH_BOTH");
		RETURN_FALSE;
	}

	mysqlnd_fetch_all(result, mode, return_value);
}
/* }}} */

/* {{{ proto array mysqli_get_client_stats(void)
   Returns statistics about the zval cache */
PHP_FUNCTION(mysqli_get_client_stats)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	mysqlnd_get_client_stats(return_value);
}
/* }}} */

/* {{{ proto array mysqli_get_connection_stats(void)
   Returns statistics about the zval cache */
PHP_FUNCTION(mysqli_get_connection_stats)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O",
									 &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	mysqlnd_get_connection_stats(mysql->mysql, return_value);
}
#endif
/* }}} */

/* {{{ proto mixed mysqli_error_list(object connection)
   Fetches all client errors */
PHP_FUNCTION(mysqli_error_list)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);
#if defined(MYSQLI_USE_MYSQLND)
	if (1) {
		MYSQLND_ERROR_LIST_ELEMENT * message;
		zend_llist_position pos;
		array_init(return_value);
		for (message = (MYSQLND_ERROR_LIST_ELEMENT *) zend_llist_get_first_ex(&mysql->mysql->data->error_info->error_list, &pos);
			 message;
			 message = (MYSQLND_ERROR_LIST_ELEMENT *) zend_llist_get_next_ex(&mysql->mysql->data->error_info->error_list, &pos))
		{
			zval single_error;
			array_init(&single_error);
			add_assoc_long_ex(&single_error, "errno", sizeof("errno") - 1, message->error_no);
			add_assoc_string_ex(&single_error, "sqlstate", sizeof("sqlstate") - 1, message->sqlstate);
			add_assoc_string_ex(&single_error, "error", sizeof("error") - 1, message->error);
			add_next_index_zval(return_value, &single_error);
		}
	} else {
		ZVAL_EMPTY_ARRAY(return_value);
	}
#else
	if (mysql_errno(mysql->mysql)) {
		zval single_error;
		array_init(return_value);
		array_init(&single_error);
		add_assoc_long_ex(&single_error, "errno", sizeof("errno") - 1, mysql_errno(mysql->mysql));
		add_assoc_string_ex(&single_error, "sqlstate", sizeof("sqlstate") - 1, mysql_sqlstate(mysql->mysql));
		add_assoc_string_ex(&single_error, "error", sizeof("error") - 1, mysql_error(mysql->mysql));
		add_next_index_zval(return_value, &single_error);
	} else {
		ZVAL_EMPTY_ARRAY(return_value);
	}
#endif
}
/* }}} */

/* {{{ proto string mysqli_stmt_error_list(object stmt)
*/
PHP_FUNCTION(mysqli_stmt_error_list)
{
	MY_STMT	*stmt;
	zval 	*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, mysql_stmt, MYSQLI_STATUS_INITIALIZED);
#if defined(MYSQLI_USE_MYSQLND)
	if (stmt->stmt && stmt->stmt->data && stmt->stmt->data->error_info) {
		MYSQLND_ERROR_LIST_ELEMENT * message;
		zend_llist_position pos;
		array_init(return_value);
		for (message = (MYSQLND_ERROR_LIST_ELEMENT *) zend_llist_get_first_ex(&stmt->stmt->data->error_info->error_list, &pos);
			 message;
			 message = (MYSQLND_ERROR_LIST_ELEMENT *) zend_llist_get_next_ex(&stmt->stmt->data->error_info->error_list, &pos))
		{
			zval single_error;
			array_init(&single_error);
			add_assoc_long_ex(&single_error, "errno", sizeof("errno") - 1, message->error_no);
			add_assoc_string_ex(&single_error, "sqlstate", sizeof("sqlstate") - 1, message->sqlstate);
			add_assoc_string_ex(&single_error, "error", sizeof("error") - 1, message->error);
			add_next_index_zval(return_value, &single_error);
		}
	} else {
		ZVAL_EMPTY_ARRAY(return_value);
	}
#else
	if (mysql_stmt_errno(stmt->stmt)) {
		zval single_error;
		array_init(return_value);
		array_init(&single_error);
		add_assoc_long_ex(&single_error, "errno", sizeof("errno") - 1, mysql_stmt_errno(stmt->stmt));
		add_assoc_string_ex(&single_error, "sqlstate", sizeof("sqlstate") - 1, mysql_stmt_sqlstate(stmt->stmt));
		add_assoc_string_ex(&single_error, "error", sizeof("error") - 1, mysql_stmt_error(stmt->stmt));
		add_next_index_zval(return_value, &single_error);
	} else {
		ZVAL_EMPTY_ARRAY(return_value);
	}
#endif
}
/* }}} */

/* {{{ proto mixed mysqli_fetch_object(object result [, string class_name [, NULL|array ctor_params]])
   Fetch a result row as an object */
PHP_FUNCTION(mysqli_fetch_object)
{
	php_mysqli_fetch_into_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, MYSQLI_ASSOC, 1);
}
/* }}} */

/* {{{ proto bool mysqli_multi_query(object link, string query)
   allows to execute multiple queries  */
PHP_FUNCTION(mysqli_multi_query)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	char		*query = NULL;
	size_t 		query_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &mysql_link, mysqli_link_class_entry, &query, &query_len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	MYSQLI_ENABLE_MQ;
	if (mysql_real_query(mysql->mysql, query, query_len)) {
#ifndef MYSQLI_USE_MYSQLND
		char s_error[MYSQL_ERRMSG_SIZE], s_sqlstate[SQLSTATE_LENGTH+1];
		unsigned int s_errno;
		/* we have to save error information, cause
		MYSQLI_DISABLE_MQ will reset error information */
		strcpy(s_error, mysql_error(mysql->mysql));
		strcpy(s_sqlstate, mysql_sqlstate(mysql->mysql));
		s_errno = mysql_errno(mysql->mysql);
#else
		MYSQLND_ERROR_INFO error_info = *mysql->mysql->data->error_info;
		mysql->mysql->data->error_info->error_list.head = NULL;
		mysql->mysql->data->error_info->error_list.tail = NULL;
		mysql->mysql->data->error_info->error_list.count = 0;
#endif
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		MYSQLI_DISABLE_MQ;

#ifndef MYSQLI_USE_MYSQLND
		/* restore error information */
		strcpy(mysql->mysql->net.last_error, s_error);
		strcpy(mysql->mysql->net.sqlstate, s_sqlstate);
		mysql->mysql->net.last_errno = s_errno;
#else
		zend_llist_clean(&mysql->mysql->data->error_info->error_list);
		*mysql->mysql->data->error_info = error_info;
#endif
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto mixed mysqli_query(object link, string query [,int resultmode]) */
PHP_FUNCTION(mysqli_query)
{
	MY_MYSQL			*mysql;
	zval				*mysql_link;
	MYSQLI_RESOURCE		*mysqli_resource;
	MYSQL_RES 			*result = NULL;
	char				*query = NULL;
	size_t 				query_len;
	zend_long 				resultmode = MYSQLI_STORE_RESULT;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os|l", &mysql_link, mysqli_link_class_entry, &query, &query_len, &resultmode) == FAILURE) {
		return;
	}

	if (!query_len) {
		php_error_docref(NULL, E_WARNING, "Empty query");
		RETURN_FALSE;
	}
#ifdef MYSQLI_USE_MYSQLND
	if ((resultmode & ~MYSQLI_ASYNC) != MYSQLI_USE_RESULT && (resultmode & ~(MYSQLI_ASYNC | MYSQLI_STORE_RESULT_COPY_DATA)) != MYSQLI_STORE_RESULT) {
#else
	if ((resultmode & ~MYSQLI_ASYNC) != MYSQLI_USE_RESULT && (resultmode & ~MYSQLI_ASYNC) != MYSQLI_STORE_RESULT) {
#endif
		php_error_docref(NULL, E_WARNING, "Invalid value for resultmode");
		RETURN_FALSE;
	}

	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	MYSQLI_DISABLE_MQ;


#ifdef MYSQLI_USE_MYSQLND
	if (resultmode & MYSQLI_ASYNC) {
		if (mysqli_async_query(mysql->mysql, query, query_len)) {
			MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
			RETURN_FALSE;
		}
		mysql->async_result_fetch_type = resultmode & ~MYSQLI_ASYNC;
		RETURN_TRUE;
	}
#endif

	if (mysql_real_query(mysql->mysql, query, query_len)) {
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		RETURN_FALSE;
	}

	if (!mysql_field_count(mysql->mysql)) {
		/* no result set - not a SELECT */
		if (MyG(report_mode) & MYSQLI_REPORT_INDEX) {
			php_mysqli_report_index(query, mysqli_server_status(mysql->mysql));
		}
		RETURN_TRUE;
	}

#ifdef MYSQLI_USE_MYSQLND
	switch (resultmode & ~(MYSQLI_ASYNC | MYSQLI_STORE_RESULT_COPY_DATA)) {
#else
	switch (resultmode & ~MYSQLI_ASYNC) {
#endif
		case MYSQLI_STORE_RESULT:
#ifdef MYSQLI_USE_MYSQLND
			if (resultmode & MYSQLI_STORE_RESULT_COPY_DATA) {
				result = mysqlnd_store_result_ofs(mysql->mysql);
			} else
#endif
				result = mysql_store_result(mysql->mysql);
			break;
		case MYSQLI_USE_RESULT:
			result = mysql_use_result(mysql->mysql);
			break;
	}
	if (!result) {
		php_mysqli_throw_sql_exception((char *)mysql_sqlstate(mysql->mysql), mysql_errno(mysql->mysql),
										"%s", mysql_error(mysql->mysql));
		RETURN_FALSE;
	}

	if (MyG(report_mode) & MYSQLI_REPORT_INDEX) {
		php_mysqli_report_index(query, mysqli_server_status(mysql->mysql));
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)result;
	mysqli_resource->status = MYSQLI_STATUS_VALID;
	MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_result_class_entry);
}
/* }}} */

#if defined(MYSQLI_USE_MYSQLND)
#include "php_network.h"
/* {{{ mysqlnd_zval_array_to_mysqlnd_array functions */
static int mysqlnd_zval_array_to_mysqlnd_array(zval *in_array, MYSQLND ***out_array)
{
	zval *elem;
	int i = 0, current = 0;

	if (Z_TYPE_P(in_array) != IS_ARRAY) {
		return 0;
	}
	*out_array = ecalloc(zend_hash_num_elements(Z_ARRVAL_P(in_array)) + 1, sizeof(MYSQLND *));
	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(in_array), elem) {
		i++;
		if (Z_TYPE_P(elem) != IS_OBJECT ||
			!instanceof_function(Z_OBJCE_P(elem), mysqli_link_class_entry)) {
			php_error_docref(NULL, E_WARNING, "Parameter %d not a mysqli object", i);
		} else {
			MY_MYSQL *mysql;
			MYSQLI_RESOURCE *my_res;
			mysqli_object *intern = Z_MYSQLI_P(elem);
			if (!(my_res = (MYSQLI_RESOURCE *)intern->ptr)) {
		  		php_error_docref(NULL, E_WARNING, "[%d] Couldn't fetch %s", i, ZSTR_VAL(intern->zo.ce->name));
				continue;
		  	}
			mysql = (MY_MYSQL*) my_res->ptr;
			if (MYSQLI_STATUS_VALID && my_res->status < MYSQLI_STATUS_VALID) {
				php_error_docref(NULL, E_WARNING, "Invalid object %d or resource %s", i, ZSTR_VAL(intern->zo.ce->name));
				continue;
			}
			(*out_array)[current++] = mysql->mysql;
		}
	} ZEND_HASH_FOREACH_END();
	return 0;
}
/* }}} */

/* {{{ mysqlnd_zval_array_from_mysqlnd_array */
static int mysqlnd_zval_array_from_mysqlnd_array(MYSQLND **in_array, zval *out_array)
{
	MYSQLND **p = in_array;
	zval dest_array;
	zval *elem, *dest_elem;
	int ret = 0, i = 0;

	array_init_size(&dest_array, zend_hash_num_elements(Z_ARRVAL_P(out_array)));

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(out_array), elem) {
		i++;
		if (Z_TYPE_P(elem) != IS_OBJECT ||
				!instanceof_function(Z_OBJCE_P(elem), mysqli_link_class_entry)) {
			continue;
		}
		{
			MY_MYSQL *mysql;
			MYSQLI_RESOURCE *my_res;
			mysqli_object *intern = Z_MYSQLI_P(elem);
			if (!(my_res = (MYSQLI_RESOURCE *)intern->ptr)) {
		  		php_error_docref(NULL, E_WARNING, "[%d] Couldn't fetch %s", i, ZSTR_VAL(intern->zo.ce->name));
				continue;
		  	}
			mysql = (MY_MYSQL *) my_res->ptr;
			if (mysql->mysql == *p) {
				dest_elem = zend_hash_next_index_insert(Z_ARRVAL(dest_array), elem);
				if (dest_elem) {
					zval_add_ref(dest_elem);
				}
				ret++;
				p++;
			}
		}
	} ZEND_HASH_FOREACH_END();

	/* destroy old array and add new one */
	zval_ptr_dtor(out_array);
	ZVAL_COPY_VALUE(out_array, &dest_array);

	return 0;
}
/* }}} */

/* {{{ mysqlnd_dont_poll_zval_array_from_mysqlnd_array */
static int mysqlnd_dont_poll_zval_array_from_mysqlnd_array(MYSQLND **in_array, zval *in_zval_array, zval *out_array)
{
	MYSQLND **p = in_array;
	zval proxy, *elem, *dest_elem;
	int ret = 0;

	array_init(&proxy);
	if (in_array) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(in_zval_array), elem) {
			MY_MYSQL *mysql;
			mysqli_object *intern = Z_MYSQLI_P(elem);
			mysql = (MY_MYSQL *)((MYSQLI_RESOURCE *)intern->ptr)->ptr;
			if (mysql->mysql == *p) {
				dest_elem = zend_hash_next_index_insert(Z_ARRVAL(proxy), elem);
				if (dest_elem) {
					zval_add_ref(dest_elem);
				}
				ret++;
				p++;
			}
		} ZEND_HASH_FOREACH_END();
	}

	/* destroy old array and add new one */
	zval_ptr_dtor(out_array);
	ZVAL_COPY_VALUE(out_array, &proxy);

	return 0;
}
/* }}} */

/* {{{ proto int mysqli_poll(array read, array write, array error, int sec [, int usec]) U
   Poll connections */
PHP_FUNCTION(mysqli_poll)
{
	zval			*r_array, *e_array, *dont_poll_array;
	MYSQLND			**new_r_array = NULL, **new_e_array = NULL, **new_dont_poll_array = NULL;
	zend_long			sec = 0, usec = 0;
	enum_func_status ret;
	int 			desc_num;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "a!a!al|l", &r_array, &e_array, &dont_poll_array, &sec, &usec) == FAILURE) {
		return;
	}
	if (sec < 0 || usec < 0) {
		php_error_docref(NULL, E_WARNING, "Negative values passed for sec and/or usec");
		RETURN_FALSE;
	}

	if (!r_array && !e_array) {
		php_error_docref(NULL, E_WARNING, "No stream arrays were passed");
		RETURN_FALSE;
	}

	if (r_array != NULL) {
		mysqlnd_zval_array_to_mysqlnd_array(r_array, &new_r_array);
	}
	if (e_array != NULL) {
		mysqlnd_zval_array_to_mysqlnd_array(e_array, &new_e_array);
	}

	ret = mysqlnd_poll(new_r_array, new_e_array, &new_dont_poll_array, sec, usec, &desc_num);

	mysqlnd_dont_poll_zval_array_from_mysqlnd_array(r_array != NULL ? new_dont_poll_array:NULL, r_array, dont_poll_array);

	if (r_array != NULL) {
		mysqlnd_zval_array_from_mysqlnd_array(new_r_array, r_array);
	}
	if (e_array != NULL) {
		mysqlnd_zval_array_from_mysqlnd_array(new_e_array, e_array);
	}

	if (new_dont_poll_array) {
		efree(new_dont_poll_array);
	}
	if (new_r_array) {
		efree(new_r_array);
	}
	if (new_e_array) {
		efree(new_e_array);
	}
	if (ret == PASS) {
		RETURN_LONG(desc_num);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int mysqli_reap_async_query(object link) U
   Poll connections */
PHP_FUNCTION(mysqli_reap_async_query)
{
	MY_MYSQL		*mysql;
	zval			*mysql_link;
	MYSQLI_RESOURCE		*mysqli_resource;
	MYSQL_RES 			*result = NULL;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}

	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	if (FAIL == mysqlnd_reap_async_query(mysql->mysql)) {
		RETURN_FALSE;
	}

	if (!mysql_field_count(mysql->mysql)) {
		/* no result set - not a SELECT */
		if (MyG(report_mode) & MYSQLI_REPORT_INDEX) {
/*			php_mysqli_report_index("n/a", mysqli_server_status(mysql->mysql)); */
		}
		RETURN_TRUE;
	}

	switch (mysql->async_result_fetch_type) {
		case MYSQLI_STORE_RESULT:
			result = mysql_store_result(mysql->mysql);
			break;
		case MYSQLI_USE_RESULT:
			result = mysql_use_result(mysql->mysql);
			break;
	}

	if (!result) {
		php_mysqli_throw_sql_exception((char *)mysql_sqlstate(mysql->mysql), mysql_errno(mysql->mysql),
										"%s", mysql_error(mysql->mysql));
		RETURN_FALSE;
	}

	if (MyG(report_mode) & MYSQLI_REPORT_INDEX) {
/*		php_mysqli_report_index("n/a", mysqli_server_status(mysql->mysql)); */
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)result;
	mysqli_resource->status = MYSQLI_STATUS_VALID;
	MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_result_class_entry);
}
/* }}} */

/* {{{ proto object mysqli_stmt_get_result(object link) U
   Buffer result set on client */
PHP_FUNCTION(mysqli_stmt_get_result)
{
	MYSQL_RES 		*result;
	MYSQLI_RESOURCE	*mysqli_resource;
	MY_STMT			*stmt;
	zval 			*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, mysql_stmt, MYSQLI_STATUS_VALID);

	if (!(result = mysqlnd_stmt_get_result(stmt->stmt))) {
		MYSQLI_REPORT_STMT_ERROR(stmt->stmt);
		RETURN_FALSE;
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)result;
	mysqli_resource->status = MYSQLI_STATUS_VALID;
	MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_result_class_entry);
}
/* }}} */
#endif

/* {{{ proto object mysqli_get_warnings(object link) */
PHP_FUNCTION(mysqli_get_warnings)
{
	MY_MYSQL			*mysql;
	zval				*mysql_link;
	MYSQLI_RESOURCE		*mysqli_resource;
	MYSQLI_WARNING		*w;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	if (mysql_warning_count(mysql->mysql)) {
#ifdef MYSQLI_USE_MYSQLND
		w = php_get_warnings(mysql->mysql->data);
#else
		w = php_get_warnings(mysql->mysql);
#endif
	} else {
		RETURN_FALSE;
	}
	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = mysqli_resource->info = (void *)w;
	mysqli_resource->status = MYSQLI_STATUS_VALID;
	MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_warning_class_entry);
}
/* }}} */

/* {{{ proto object mysqli_stmt_get_warnings(object link) */
PHP_FUNCTION(mysqli_stmt_get_warnings)
{
	MY_STMT				*stmt;
	zval				*stmt_link;
	MYSQLI_RESOURCE		*mysqli_resource;
	MYSQLI_WARNING		*w;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &stmt_link, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, stmt_link, MYSQLI_STATUS_VALID);

	if (mysqli_stmt_warning_count(stmt->stmt)) {
		w = php_get_warnings(mysqli_stmt_get_connection(stmt->stmt));
	} else {
		RETURN_FALSE;
	}
	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = mysqli_resource->info = (void *)w;
	mysqli_resource->status = MYSQLI_STATUS_VALID;
	MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_warning_class_entry);
}
/* }}} */

#ifdef HAVE_MYSQLI_SET_CHARSET
/* {{{ proto bool mysqli_set_charset(object link, string csname)
   sets client character set */
PHP_FUNCTION(mysqli_set_charset)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	char		*cs_name;
	size_t		csname_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &mysql_link, mysqli_link_class_entry, &cs_name, &csname_len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	if (mysql_set_character_set(mysql->mysql, cs_name)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */
#endif

#ifdef HAVE_MYSQLI_GET_CHARSET
/* {{{ proto object mysqli_get_charset(object link) U
   returns a character set object */
PHP_FUNCTION(mysqli_get_charset)
{
	MY_MYSQL				*mysql;
	zval					*mysql_link;
	const char 				*name = NULL, *collation = NULL, *dir = NULL, *comment = NULL;
	uint32_t				minlength, maxlength, number, state;
#if !defined(MYSQLI_USE_MYSQLND)
	MY_CHARSET_INFO			cs;
#else
	const MYSQLND_CHARSET	*cs;
#endif

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);


#if !defined(MYSQLI_USE_MYSQLND)
	mysql_get_character_set_info(mysql->mysql, &cs);
	name = (char *)cs.csname;
	collation = (char *)cs.name;
	dir = (char *)cs.dir;
	minlength = cs.mbminlen;
	maxlength = cs.mbmaxlen;
	number = cs.number;
	state = cs.state;
	comment = cs.comment;
#else
	cs = mysql->mysql->data->charset;
	if (!cs) {
		php_error_docref(NULL, E_WARNING, "The connection has no charset associated");
		RETURN_NULL();
	}
	name = cs->name;
	collation = cs->collation;
	minlength = cs->char_minlen;
	maxlength = cs->char_maxlen;
	number = cs->nr;
	comment = cs->comment;
	state = 1;	/* all charsets are compiled in */
#endif
	object_init(return_value);

	add_property_string(return_value, "charset", (name) ? (char *)name : "");
	add_property_string(return_value, "collation",(collation) ? (char *)collation : "");
	add_property_string(return_value, "dir", (dir) ? (char *)dir : "");
	add_property_long(return_value, "min_length", minlength);
	add_property_long(return_value, "max_length", maxlength);
	add_property_long(return_value, "number", number);
	add_property_long(return_value, "state", state);
	add_property_string(return_value, "comment", (comment) ? (char *)comment : "");
}
/* }}} */
#endif

#if !defined(MYSQLI_USE_MYSQLND)
extern char * mysqli_escape_string_for_tx_name_in_comment(const char * const name);

/* {{{ proto bool mysqli_begin_transaction_libmysql */
static int mysqli_begin_transaction_libmysql(MYSQL * conn, const unsigned int mode, const char * const name)
{
	int ret;
	zend_bool err = FALSE;
	smart_str tmp_str = {0};
	if (mode & TRANS_START_WITH_CONSISTENT_SNAPSHOT) {
		if (tmp_str.s) {
			smart_str_appendl(&tmp_str, ", ", sizeof(", ") - 1);
		}
		smart_str_appendl(&tmp_str, "WITH CONSISTENT SNAPSHOT", sizeof("WITH CONSISTENT SNAPSHOT") - 1);
	}
	if (mode & (TRANS_START_READ_WRITE | TRANS_START_READ_ONLY)) {
		if (mysql_get_server_version(conn) < 50605L) {
			php_error_docref(NULL, E_WARNING, "This server version doesn't support 'READ WRITE' and 'READ ONLY'. Minimum 5.6.5 is required");
			err = TRUE;
		} else if (mode & TRANS_START_READ_WRITE) {
			if (tmp_str.s) {
				smart_str_appendl(&tmp_str, ", ", sizeof(", ") - 1);
			}
			smart_str_appendl(&tmp_str, "READ WRITE", sizeof("READ WRITE") - 1);
		} else if (mode & TRANS_START_READ_ONLY) {
			if (tmp_str.s) {
				smart_str_appendl(&tmp_str, ", ", sizeof(", ") - 1);
			}
			smart_str_appendl(&tmp_str, "READ ONLY", sizeof("READ ONLY") - 1);
		}
	}
	smart_str_0(&tmp_str);

	if (err == FALSE){
		char * name_esc = mysqli_escape_string_for_tx_name_in_comment(name);
		char * query;
		unsigned int query_len = spprintf(&query, 0, "START TRANSACTION%s %s",
										  name_esc? name_esc:"", tmp_str.s? ZSTR_VAL(tmp_str.s):"");

		smart_str_free(&tmp_str);
		if (name_esc) {
			efree(name_esc);
		}

		ret = mysql_real_query(conn, query, query_len);
		efree(query);
	}
	return ret;
}
/* }}} */
#endif

/* {{{ proto bool mysqli_begin_transaction(object link, [int flags [, string name]])
   Starts a transaction */
PHP_FUNCTION(mysqli_begin_transaction)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	zend_long		flags = TRANS_START_NO_OPT;
	char *		name = NULL;
	size_t			name_len = -1;
	zend_bool	err = FALSE;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O|ls", &mysql_link, mysqli_link_class_entry, &flags, &name, &name_len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);
	if (flags < 0) {
		php_error_docref(NULL, E_WARNING, "Invalid value for parameter flags (" ZEND_LONG_FMT ")", flags);
		err = TRUE;
	}
	if (!name_len) {
		php_error_docref(NULL, E_WARNING, "Savepoint name cannot be empty");
		err = TRUE;
	}
	if (TRUE == err) {
		RETURN_FALSE;
	}

#if !defined(MYSQLI_USE_MYSQLND)
	if (mysqli_begin_transaction_libmysql(mysql->mysql, flags, name)) {
		RETURN_FALSE;
	}
#else
	if (FAIL == mysqlnd_begin_transaction(mysql->mysql, flags, name)) {
		RETURN_FALSE;
	}
#endif
	RETURN_TRUE;
}
/* }}} */

#if !defined(MYSQLI_USE_MYSQLND)
/* {{{ proto bool mysqli_savepoint_libmysql */
static int mysqli_savepoint_libmysql(MYSQL * conn, const char * const name, zend_bool release)
{
	int ret;
	char * query;
	unsigned int query_len = spprintf(&query, 0, release? "RELEASE SAVEPOINT `%s`":"SAVEPOINT `%s`", name);
	ret = mysql_real_query(conn, query, query_len);
	efree(query);
	return ret;
}
/* }}} */
#endif

/* {{{ proto bool mysqli_savepoint(object link, string name)
   Starts a transaction */
PHP_FUNCTION(mysqli_savepoint)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	char *		name = NULL;
	size_t			name_len = -1;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &mysql_link, mysqli_link_class_entry, &name, &name_len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);
	if (!name || !name_len) {
		php_error_docref(NULL, E_WARNING, "Savepoint name cannot be empty");
		RETURN_FALSE;
	}

#if !defined(MYSQLI_USE_MYSQLND)
	if (mysqli_savepoint_libmysql(mysql->mysql, name, FALSE)) {
#else
	if (FAIL == mysqlnd_savepoint(mysql->mysql, name)) {
#endif
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool mysqli_release_savepoint(object link, string name)
   Starts a transaction */
PHP_FUNCTION(mysqli_release_savepoint)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	char *		name = NULL;
	size_t			name_len = -1;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &mysql_link, mysqli_link_class_entry, &name, &name_len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);
	if (!name || !name_len) {
		php_error_docref(NULL, E_WARNING, "Savepoint name cannot be empty");
		RETURN_FALSE;
	}
#if !defined(MYSQLI_USE_MYSQLND)
	if (mysqli_savepoint_libmysql(mysql->mysql, name, TRUE)) {
#else
	if (FAIL == mysqlnd_savepoint(mysql->mysql, name)) {
#endif
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool mysqli_get_links_stats()
   Returns information about open and cached links */
PHP_FUNCTION(mysqli_get_links_stats)
{
	if (ZEND_NUM_ARGS()) {
		php_error_docref(NULL, E_WARNING, "no parameters expected");
		return;
	}
	array_init(return_value);
	add_assoc_long_ex(return_value, "total", sizeof("total") - 1, MyG(num_links));
	add_assoc_long_ex(return_value, "active_plinks", sizeof("active_plinks") - 1, MyG(num_active_persistent));
	add_assoc_long_ex(return_value, "cached_plinks", sizeof("cached_plinks") - 1, MyG(num_inactive_persistent));
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
