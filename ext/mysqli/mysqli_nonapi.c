/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://www.php.net/license/3_01.txt                                 |
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
#define ERROR_ARG_POS(arg_num) (getThis() ? (arg_num-1) : (arg_num))

#define SAFE_STR(a) ((a)?a:"")

/* {{{ php_mysqli_set_error */
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

void mysqli_common_connect(INTERNAL_FUNCTION_PARAMETERS, bool is_real_connect, bool in_ctor) /* {{{ */
{
	MY_MYSQL			*mysql = NULL;
	MYSQLI_RESOURCE		*mysqli_resource = NULL;
	zval				*object = getThis();
	char				*hostname = NULL, *username=NULL, *passwd=NULL, *dbname=NULL, *socket=NULL,
						*ssl_key = NULL, *ssl_cert = NULL, *ssl_ca = NULL, *ssl_capath = NULL,
						*ssl_cipher = NULL;
	size_t				hostname_len = 0, username_len = 0, passwd_len = 0, dbname_len = 0, socket_len = 0;
	bool			persistent = false, ssl = false;
	zend_long			port = 0, flags = 0;
	bool           port_is_null = 1;
	zend_string			*hash_key = NULL;
	bool			new_connection = false;
	zend_resource		*le;
	mysqli_plist_entry *plist = NULL;
	bool			self_alloced = 0;


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
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s!s!s!s!l!s!", &hostname, &hostname_len, &username, &username_len,
				&passwd, &passwd_len, &dbname, &dbname_len, &port, &port_is_null, &socket, &socket_len) == FAILURE) {
			RETURN_THROWS();
		}

		if (object) {
			ZEND_ASSERT(instanceof_function(Z_OBJCE_P(object), mysqli_link_class_entry));
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
		if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O|s!s!s!s!l!s!l", &object, mysqli_link_class_entry,
				&hostname, &hostname_len, &username, &username_len, &passwd, &passwd_len, &dbname, &dbname_len, &port, &port_is_null, &socket, &socket_len, &flags) == FAILURE) {
			RETURN_THROWS();
		}

		mysqli_resource = (Z_MYSQLI_P(object))->ptr;
		MYSQLI_FETCH_RESOURCE_CONN(mysql, object, MYSQLI_STATUS_INITIALIZED);

		/* set some required options */
		flags |= CLIENT_MULTI_RESULTS; /* needed for mysql_multi_query() */
		/* remove some insecure options */
		flags &= ~CLIENT_MULTI_STATEMENTS;   /* don't allow multi_queries via connect parameter */
	}

	if (!socket_len || !socket) {
		socket = MyG(default_socket);
	}
	if (port_is_null || !port) {
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
			mysql->persistent = persistent = true;

			hash_key = strpprintf(0, "mysqli_%s_%s" ZEND_LONG_FMT "%s%s%s", SAFE_STR(hostname), SAFE_STR(socket),
								port, SAFE_STR(username), SAFE_STR(dbname),
								SAFE_STR(passwd));

			mysql->hash_key = hash_key;

			/* check if we can reuse existing connection ... */
			if ((le = zend_hash_find_ptr(&EG(persistent_list), hash_key)) != NULL) {
				if (le->type == php_le_pmysqli()) {
					plist = (mysqli_plist_entry *) le->ptr;

					do {
						if (zend_ptr_stack_num_elements(&plist->free_links)) {
							/* If we have an initialized (but unconnected) mysql resource,
							 * close it before we reuse an existing persistent resource. */
							if (mysql->mysql) {
								mysqli_close(mysql->mysql, MYSQLI_CLOSE_IMPLICIT);
							}

							mysql->mysql = zend_ptr_stack_pop(&plist->free_links);

							MyG(num_inactive_persistent)--;
							/* reset variables */

#ifndef MYSQLI_NO_CHANGE_USER_ON_PCONNECT
							if (!mysqli_change_user_silent(mysql->mysql, username, passwd, dbname, passwd_len)) {
#else
							if (!mysql_ping(mysql->mysql)) {
#endif
								mysqlnd_restart_psession(mysql->mysql);
								MyG(num_active_persistent)++;

								/* clear error */
								php_mysqli_set_error(mysql_errno(mysql->mysql), (char *) mysql_error(mysql->mysql));

								goto end;
							} else {
								if (mysql->mysql->data->vio->data->ssl) {
									/* copy over pre-existing ssl settings so we can reuse them when reconnecting */
									ssl = true;

									ssl_key = mysql->mysql->data->vio->data->options.ssl_key ? estrdup(mysql->mysql->data->vio->data->options.ssl_key) : NULL;
									ssl_cert = mysql->mysql->data->vio->data->options.ssl_cert ? estrdup(mysql->mysql->data->vio->data->options.ssl_cert) : NULL;
									ssl_ca = mysql->mysql->data->vio->data->options.ssl_ca ? estrdup(mysql->mysql->data->vio->data->options.ssl_ca) : NULL;
									ssl_capath = mysql->mysql->data->vio->data->options.ssl_capath ? estrdup(mysql->mysql->data->vio->data->options.ssl_capath) : NULL;
									ssl_cipher = mysql->mysql->data->vio->data->options.ssl_cipher ? estrdup(mysql->mysql->data->vio->data->options.ssl_cipher) : NULL;
								}

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
		if (!(mysql->mysql = mysqlnd_init(MYSQLND_CLIENT_NO_FLAG, persistent))) {
			goto err;
		}
		new_connection = true;
	}

	if (ssl) {
		/* if we're here, this means previous conn was ssl, repopulate settings */
		mysql_ssl_set(mysql->mysql, ssl_key, ssl_cert, ssl_ca, ssl_capath, ssl_cipher);

		if (ssl_key) {
		    efree(ssl_key);
		}

		if (ssl_cert) {
		    efree(ssl_cert);
		}

		if (ssl_ca) {
		    efree(ssl_ca);
		}

		if (ssl_capath) {
		    efree(ssl_capath);
		}

		if (ssl_cipher) {
		    efree(ssl_cipher);
		}
	}
	if (mysqlnd_connect(mysql->mysql, hostname, username, passwd, passwd_len, dbname, dbname_len,
						port, socket, flags, MYSQLND_CLIENT_NO_FLAG) == NULL)
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

	unsigned int allow_local_infile = MyG(allow_local_infile);
	mysql_options(mysql->mysql, MYSQL_OPT_LOCAL_INFILE, (char *)&allow_local_infile);

	if (MyG(local_infile_directory) && !php_check_open_basedir(MyG(local_infile_directory))) {
		mysql_options(mysql->mysql, MYSQL_OPT_LOAD_DATA_LOCAL_DIR, MyG(local_infile_directory));
	}

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

	if (object) {
		ZEND_ASSERT(instanceof_function(Z_OBJCE_P(object), mysqli_link_class_entry));
		(Z_MYSQLI_P(object))->ptr = mysqli_resource;
		RETURN_TRUE;
	} else {
		MYSQLI_RETVAL_RESOURCE(mysqli_resource, mysqli_link_class_entry);
		return;
	}

err:
	if (mysql->hash_key) {
		zend_string_release_ex(mysql->hash_key, 0);
		mysql->hash_key = NULL;
		mysql->persistent = false;
	}
	if (!is_real_connect && self_alloced) {
		efree(mysql);
	}
	RETVAL_FALSE;
} /* }}} */

/* {{{ Open a connection to a mysql server */
PHP_FUNCTION(mysqli_connect)
{
	mysqli_common_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, false, false);
}
/* }}} */

PHP_METHOD(mysqli, __construct)
{
	mysqli_common_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, false, true);
}

/* {{{ Initialize mysqli and return a resource for use with mysql_real_connect */
PHP_METHOD(mysqli, init)
{
	php_mysqli_init(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}
/* }}} */

/* {{{ Returns the numerical value of the error message from last connect command */
PHP_FUNCTION(mysqli_connect_errno)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_LONG(MyG(error_no));
}
/* }}} */

/* {{{ Returns the text of the error message from previous MySQL operation */
PHP_FUNCTION(mysqli_connect_error)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (MyG(error_msg)) {
		RETURN_STRING(MyG(error_msg));
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ Fetch a result row as an associative array, a numeric array, or both */
PHP_FUNCTION(mysqli_fetch_array)
{
	php_mysqli_fetch_into_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, 0);
}
/* }}} */

/* {{{ Fetch a result row as an associative array */
PHP_FUNCTION(mysqli_fetch_assoc)
{
	php_mysqli_fetch_into_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, MYSQLI_ASSOC, 0);
}
/* }}} */

/* {{{ Fetch a column from the result row  */
PHP_FUNCTION(mysqli_fetch_column)
{
	MYSQL_RES		*result;
	zval			*mysql_result;
	zval 			row_array;
	zend_long		col_no = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O|l", &mysql_result, mysqli_result_class_entry, &col_no) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES*, mysql_result, "mysqli_result", MYSQLI_STATUS_VALID);

	if (col_no < 0) {
		zend_argument_value_error(ERROR_ARG_POS(2), "must be greater than or equal to 0");
		RETURN_THROWS();
	}
	if (col_no >= mysql_num_fields(result)) {
		zend_argument_value_error(ERROR_ARG_POS(2), "must be less than the number of fields for this result set");
		RETURN_THROWS();
	}

	php_mysqli_fetch_into_hash_aux(&row_array, result, MYSQLI_NUM);
	if (Z_TYPE(row_array) != IS_ARRAY) {
		zval_ptr_dtor_nogc(&row_array);
		RETURN_FALSE;
	}

	ZVAL_COPY(return_value, zend_hash_index_find(Z_ARR(row_array), col_no));
	zval_ptr_dtor_nogc(&row_array);
}
/* }}} */

/* {{{ Fetches all result rows as an associative array, a numeric array, or both */
PHP_FUNCTION(mysqli_fetch_all)
{
	MYSQL_RES	*result;
	zval		*mysql_result;
	zend_long		mode = MYSQLI_NUM;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O|l", &mysql_result, mysqli_result_class_entry, &mode) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, mysql_result, "mysqli_result", MYSQLI_STATUS_VALID);

	if (!mode || (mode & ~MYSQLI_BOTH)) {
		zend_argument_value_error(ERROR_ARG_POS(2), "must be one of MYSQLI_NUM, MYSQLI_ASSOC, or MYSQLI_BOTH");
		RETURN_THROWS();
	}

	array_init_size(return_value, mysql_num_rows(result));

	zend_ulong i = 0;
	do {
		zval row;
		php_mysqli_fetch_into_hash_aux(&row, result, mode);
		if (Z_TYPE(row) != IS_ARRAY) {
			zval_ptr_dtor_nogc(&row);
			break;
		}
		add_index_zval(return_value, i++, &row);
	} while (1);
}
/* }}} */

/* {{{ Returns statistics about the zval cache */
PHP_FUNCTION(mysqli_get_client_stats)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	mysqlnd_get_client_stats(return_value);
}
/* }}} */

/* {{{ Returns statistics about the zval cache */
PHP_FUNCTION(mysqli_get_connection_stats)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O",
									 &mysql_link, mysqli_link_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	mysqlnd_get_connection_stats(mysql->mysql, return_value);
}
/* }}} */

/* {{{ Fetches all client errors */
PHP_FUNCTION(mysqli_error_list)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

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
}
/* }}} */

/* {{{ */
PHP_FUNCTION(mysqli_stmt_error_list)
{
	MY_STMT	*stmt;
	zval 	*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, mysql_stmt, MYSQLI_STATUS_INITIALIZED);
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
		RETURN_EMPTY_ARRAY();
	}
}
/* }}} */

/* {{{ Fetch a result row as an object */
PHP_FUNCTION(mysqli_fetch_object)
{
	php_mysqli_fetch_into_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, MYSQLI_ASSOC, 1);
}
/* }}} */

/* {{{ allows to execute multiple queries  */
PHP_FUNCTION(mysqli_multi_query)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	char		*query = NULL;
	size_t 		query_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &mysql_link, mysqli_link_class_entry, &query, &query_len) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	MYSQLI_ENABLE_MQ;
	if (mysql_real_query(mysql->mysql, query, query_len)) {
		MYSQLND_ERROR_INFO error_info = *mysql->mysql->data->error_info;
		mysql->mysql->data->error_info->error_list.head = NULL;
		mysql->mysql->data->error_info->error_list.tail = NULL;
		mysql->mysql->data->error_info->error_list.count = 0;
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		MYSQLI_DISABLE_MQ;

		zend_llist_clean(&mysql->mysql->data->error_info->error_list);
		*mysql->mysql->data->error_info = error_info;
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ */
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
		RETURN_THROWS();
	}

	if (!query_len) {
		zend_argument_value_error(ERROR_ARG_POS(2), "cannot be empty");
		RETURN_THROWS();
	}
	if ((resultmode & ~MYSQLI_ASYNC) != MYSQLI_USE_RESULT &&
		MYSQLI_STORE_RESULT != (resultmode & ~(MYSQLI_ASYNC | MYSQLI_STORE_RESULT_COPY_DATA))
	) {
		zend_argument_value_error(ERROR_ARG_POS(3), "must be either MYSQLI_USE_RESULT or MYSQLI_STORE_RESULT with MYSQLI_ASYNC as an optional bitmask flag");
		RETURN_THROWS();
	}

	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	MYSQLI_DISABLE_MQ;


	if (resultmode & MYSQLI_ASYNC) {
		if (mysqli_async_query(mysql->mysql, query, query_len)) {
			MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
			RETURN_FALSE;
		}
		mysql->async_result_fetch_type = resultmode & ~MYSQLI_ASYNC;
		RETURN_TRUE;
	}

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

	switch (resultmode & ~(MYSQLI_ASYNC | MYSQLI_STORE_RESULT_COPY_DATA)) {
		case MYSQLI_STORE_RESULT:
			result = mysql_store_result(mysql->mysql);
			break;
		case MYSQLI_USE_RESULT:
			result = mysql_use_result(mysql->mysql);
			break;
	}
	if (!result) {
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		RETURN_FALSE;
	}

	if (MyG(report_mode) & MYSQLI_REPORT_INDEX) {
		php_mysqli_report_index(query, mysqli_server_status(mysql->mysql));
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)result;
	mysqli_resource->status = MYSQLI_STATUS_VALID;
	MYSQLI_RETVAL_RESOURCE(mysqli_resource, mysqli_result_class_entry);
}
/* }}} */

#include "php_network.h"
/* {{{ mysqlnd_zval_array_to_mysqlnd_array functions */
static int mysqlnd_zval_array_to_mysqlnd_array(zval *in_array, MYSQLND ***out_array)
{
	zval *elem;
	int i = 0, current = 0;

	if (Z_TYPE_P(in_array) != IS_ARRAY) {
		return SUCCESS;
	}
	*out_array = ecalloc(zend_hash_num_elements(Z_ARRVAL_P(in_array)) + 1, sizeof(MYSQLND *));
	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(in_array), elem) {
		i++;
		if (Z_TYPE_P(elem) != IS_OBJECT ||
			!instanceof_function(Z_OBJCE_P(elem), mysqli_link_class_entry)) {
			zend_argument_type_error(i, "must be an instance of mysqli, %s given", zend_zval_type_name(elem));
			return FAILURE;
		} else {
			MY_MYSQL *mysql;
			MYSQLI_RESOURCE *my_res;
			mysqli_object *intern = Z_MYSQLI_P(elem);
			if (!(my_res = (MYSQLI_RESOURCE *)intern->ptr)) {
				zend_throw_error(NULL, "%s object is already closed", ZSTR_VAL(intern->zo.ce->name));
				return FAILURE;
		  	}
			mysql = (MY_MYSQL*) my_res->ptr;
			if (my_res->status < MYSQLI_STATUS_VALID) {
				zend_throw_error(NULL, "%s object is not fully initialized", ZSTR_VAL(intern->zo.ce->name));
				return FAILURE;
			}
			(*out_array)[current++] = mysql->mysql;
		}
	} ZEND_HASH_FOREACH_END();
	return SUCCESS;
}
/* }}} */

/* {{{ mysqlnd_zval_array_from_mysqlnd_array */
static int mysqlnd_zval_array_from_mysqlnd_array(MYSQLND **in_array, zval *out_array)
{
	MYSQLND **p = in_array;
	zval dest_array;
	zval *elem, *dest_elem;

	array_init_size(&dest_array, zend_hash_num_elements(Z_ARRVAL_P(out_array)));

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(out_array), elem) {
		if (Z_TYPE_P(elem) != IS_OBJECT ||
				!instanceof_function(Z_OBJCE_P(elem), mysqli_link_class_entry)) {
			continue;
		}
		{
			MY_MYSQL *mysql;
			MYSQLI_RESOURCE *my_res;
			mysqli_object *intern = Z_MYSQLI_P(elem);
			if (!(my_res = (MYSQLI_RESOURCE *)intern->ptr)) {
				zend_throw_error(NULL, "%s object is already closed", ZSTR_VAL(intern->zo.ce->name));
				return FAILURE;
		  	}
			mysql = (MY_MYSQL *) my_res->ptr;
			if (mysql->mysql == *p) {
				dest_elem = zend_hash_next_index_insert(Z_ARRVAL(dest_array), elem);
				if (dest_elem) {
					zval_add_ref(dest_elem);
				}
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

/* {{{ Poll connections */
PHP_FUNCTION(mysqli_poll)
{
	zval			*r_array, *e_array, *dont_poll_array;
	MYSQLND			**new_r_array = NULL, **new_e_array = NULL, **new_dont_poll_array = NULL;
	zend_long			sec = 0, usec = 0;
	enum_func_status ret;
	int 			desc_num;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "a!a!al|l", &r_array, &e_array, &dont_poll_array, &sec, &usec) == FAILURE) {
		RETURN_THROWS();
	}
	if (sec < 0) {
		zend_argument_value_error(4, "must be greater than or equal to 0");
		RETURN_THROWS();
	}
	if (usec < 0) {
		zend_argument_value_error(5, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	// TODO Error promotion
	if (!r_array && !e_array) {
		php_error_docref(NULL, E_WARNING, "No stream arrays were passed");
		RETURN_FALSE;
	}

	if (r_array != NULL) {
		if (mysqlnd_zval_array_to_mysqlnd_array(r_array, &new_r_array) == FAILURE) {
			efree(new_r_array);
			RETURN_THROWS();
		}
	}
	if (e_array != NULL) {
		if (mysqlnd_zval_array_to_mysqlnd_array(e_array, &new_e_array) == FAILURE) {
			efree(new_e_array);
			efree(new_r_array);
			RETURN_THROWS();
		}
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

/* {{{ Poll connections */
PHP_FUNCTION(mysqli_reap_async_query)
{
	MY_MYSQL		*mysql;
	zval			*mysql_link;
	MYSQLI_RESOURCE		*mysqli_resource;
	MYSQL_RES 			*result = NULL;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		RETURN_THROWS();
	}

	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	if (FAIL == mysqlnd_reap_async_query(mysql->mysql)) {
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
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
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		RETURN_FALSE;
	}

	if (MyG(report_mode) & MYSQLI_REPORT_INDEX) {
/*		php_mysqli_report_index("n/a", mysqli_server_status(mysql->mysql)); */
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)result;
	mysqli_resource->status = MYSQLI_STATUS_VALID;
	MYSQLI_RETVAL_RESOURCE(mysqli_resource, mysqli_result_class_entry);
}
/* }}} */

/* {{{ Buffer result set on client */
PHP_FUNCTION(mysqli_stmt_get_result)
{
	MYSQL_RES 		*result;
	MYSQLI_RESOURCE	*mysqli_resource;
	MY_STMT			*stmt;
	zval 			*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, mysql_stmt, MYSQLI_STATUS_VALID);

	if (!(result = mysqlnd_stmt_get_result(stmt->stmt))) {
		MYSQLI_REPORT_STMT_ERROR(stmt->stmt);
		RETURN_FALSE;
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)result;
	mysqli_resource->status = MYSQLI_STATUS_VALID;
	MYSQLI_RETVAL_RESOURCE(mysqli_resource, mysqli_result_class_entry);
}
/* }}} */

/* {{{ */
PHP_FUNCTION(mysqli_get_warnings)
{
	MY_MYSQL			*mysql;
	zval				*mysql_link;
	MYSQLI_RESOURCE		*mysqli_resource;
	MYSQLI_WARNING		*w = NULL;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	if (mysql_warning_count(mysql->mysql)) {
		w = php_get_warnings(mysql->mysql->data);
	}
	if (!w) {
		RETURN_FALSE;
	}
	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = mysqli_resource->info = (void *)w;
	mysqli_resource->status = MYSQLI_STATUS_VALID;
	MYSQLI_RETVAL_RESOURCE(mysqli_resource, mysqli_warning_class_entry);
}
/* }}} */

/* {{{ */
PHP_FUNCTION(mysqli_stmt_get_warnings)
{
	MY_STMT				*stmt;
	zval				*stmt_link;
	MYSQLI_RESOURCE		*mysqli_resource;
	MYSQLI_WARNING		*w = NULL;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &stmt_link, mysqli_stmt_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_STMT(stmt, stmt_link, MYSQLI_STATUS_VALID);

	if (mysqli_stmt_warning_count(stmt->stmt)) {
		w = php_get_warnings(mysqli_stmt_get_connection(stmt->stmt));
	}
	if (!w) {
		RETURN_FALSE;
	}
	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = mysqli_resource->info = (void *)w;
	mysqli_resource->status = MYSQLI_STATUS_VALID;
	MYSQLI_RETVAL_RESOURCE(mysqli_resource, mysqli_warning_class_entry);
}
/* }}} */

/* {{{ sets client character set */
PHP_FUNCTION(mysqli_set_charset)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	char		*cs_name;
	size_t		csname_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &mysql_link, mysqli_link_class_entry, &cs_name, &csname_len) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);

	if (mysql_set_character_set(mysql->mysql, cs_name)) {
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ returns a character set object */
PHP_FUNCTION(mysqli_get_charset)
{
	MY_MYSQL				*mysql;
	zval					*mysql_link;
	const char 				*name = NULL, *collation = NULL, *dir = NULL, *comment = NULL;
	uint32_t				minlength, maxlength, number, state;
	const MYSQLND_CHARSET	*cs;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);


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

/* {{{ Starts a transaction */
PHP_FUNCTION(mysqli_begin_transaction)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	zend_long		flags = TRANS_START_NO_OPT;
	char *		name = NULL;
	size_t			name_len = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O|ls!", &mysql_link, mysqli_link_class_entry, &flags, &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);
	if (flags < 0) {
		zend_argument_value_error(ERROR_ARG_POS(2), "must be one of the MYSQLI_TRANS_* constants");
		RETURN_THROWS();
	}
	if (name && !name_len) {
		zend_argument_value_error(ERROR_ARG_POS(3), "cannot be empty");
		RETURN_THROWS();
	}

	if (FAIL == mysqlnd_begin_transaction(mysql->mysql, flags, name)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Starts a transaction */
PHP_FUNCTION(mysqli_savepoint)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	char *		name = NULL;
	size_t			name_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &mysql_link, mysqli_link_class_entry, &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);
	if (name_len == 0) {
		zend_argument_value_error(ERROR_ARG_POS(2), "cannot be empty");
		RETURN_THROWS();
	}

	if (FAIL == mysqlnd_savepoint(mysql->mysql, name)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Starts a transaction */
PHP_FUNCTION(mysqli_release_savepoint)
{
	MY_MYSQL	*mysql;
	zval		*mysql_link;
	char *		name = NULL;
	size_t			name_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &mysql_link, mysqli_link_class_entry, &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}
	MYSQLI_FETCH_RESOURCE_CONN(mysql, mysql_link, MYSQLI_STATUS_VALID);
	if (name_len == 0) {
		zend_argument_value_error(ERROR_ARG_POS(2), "cannot be empty");
		RETURN_THROWS();
	}
	if (FAIL == mysqlnd_release_savepoint(mysql->mysql, name)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ Returns information about open and cached links */
PHP_FUNCTION(mysqli_get_links_stats)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	array_init(return_value);
	add_assoc_long_ex(return_value, "total", sizeof("total") - 1, MyG(num_links));
	add_assoc_long_ex(return_value, "active_plinks", sizeof("active_plinks") - 1, MyG(num_active_persistent));
	add_assoc_long_ex(return_value, "cached_plinks", sizeof("cached_plinks") - 1, MyG(num_inactive_persistent));
}
/* }}} */
