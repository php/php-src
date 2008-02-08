/*
  +----------------------------------------------------------------------+
  | PHP Version 6                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2008 The PHP Group                                |
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

  $Id$ 
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <signal.h>

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_mysqli_structs.h"

#define SAFE_STR(a) ((a)?a:"")

void mysqli_common_connect(INTERNAL_FUNCTION_PARAMETERS, zend_bool is_real_connect)
{
	MY_MYSQL			*mysql = NULL;
	MYSQLI_RESOURCE		*mysqli_resource = NULL;
	zval				*object = getThis();
	char				*hostname = NULL, *username=NULL, *passwd=NULL, *dbname=NULL, *socket=NULL;
	unsigned int		hostname_len = 0, username_len = 0, passwd_len = 0, dbname_len = 0, socket_len = 0;
	zend_bool			persistent = FALSE;
	long				port = 0, flags = 0;
	uint				hash_len;
	char				*hash_key = NULL;
	zend_bool			new_connection = FALSE;
	zend_rsrc_list_entry	*le;
	mysqli_plist_entry *plist = NULL;

	if (getThis() && !ZEND_NUM_ARGS()) {
		RETURN_NULL();
	}
	hostname = username = dbname = passwd = socket = NULL;

	if (!is_real_connect) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s&s&s&s&ls&", &hostname, &hostname_len, UG(utf8_conv),
								  &username, &username_len, UG(utf8_conv), &passwd, &passwd_len, UG(utf8_conv), 
								  &dbname, &dbname_len, UG(utf8_conv), &port, &socket, &socket_len, UG(utf8_conv)) == FAILURE) {
			return;
		}

		if (object && instanceof_function(Z_OBJCE_P(object), mysqli_link_class_entry TSRMLS_CC)) {
			mysqli_resource = ((mysqli_object *) zend_object_store_get_object(object TSRMLS_CC))->ptr;
			if (mysqli_resource && mysqli_resource->ptr &&
				mysqli_resource->status > MYSQLI_STATUS_INITIALIZED)
			{
				mysql = (MY_MYSQL*)mysqli_resource->ptr;
				php_clear_mysql(mysql);
				if (mysql->mysql) {
					mysqli_close(mysql->mysql, MYSQLI_CLOSE_EXPLICIT);
					mysql->mysql = NULL;
				}
			}
		}
		if (!mysql) {
			mysql = (MY_MYSQL *) ecalloc(1, sizeof(MY_MYSQL));
		}
		flags |= CLIENT_MULTI_RESULTS; /* needed for mysql_multi_query() */

	} else {
		/* We have flags too */
		if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O|s&s&s&s&ls&l", &object, mysqli_link_class_entry,
			&hostname, &hostname_len, UG(utf8_conv), &username, &username_len, UG(utf8_conv), &passwd, &passwd_len, UG(utf8_conv),
			&dbname, &dbname_len, UG(utf8_conv), &port, &socket, &socket_len, UG(utf8_conv), &flags) == FAILURE) {
			return;
		}
		mysqli_resource = ((mysqli_object *) zend_object_store_get_object(object TSRMLS_CC))->ptr;
		MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &object, "mysqli_link", MYSQLI_STATUS_INITIALIZED);

		flags |= CLIENT_MULTI_RESULTS; /* needed for mysql_multi_query() */
		/* remove some insecure options */
		flags &= ~CLIENT_MULTI_STATEMENTS;   /* don't allow multi_queries via connect parameter */
		if (PG(open_basedir) && PG(open_basedir)[0] != '\0') {
			flags ^= CLIENT_LOCAL_FILES;
		}
	}


	if (!socket_len || !socket) {
		socket = MyG(default_socket);
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

	if (strlen(SAFE_STR(hostname)) > 2 && !strncasecmp(hostname, "p:", 2)) {
		hostname += 2;
		if (!MyG(allow_persistent)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Persistent connections are disabled. Downgrading to normal");			
		} else {
			mysql->persistent = persistent = TRUE;

			hash_len = spprintf(&hash_key, 0, "mysqli_%s_%s%ld%s%s%s", SAFE_STR(hostname), SAFE_STR(socket), 
								port, SAFE_STR(username), SAFE_STR(dbname), 
								SAFE_STR(passwd));

			mysql->hash_key = hash_key;

			/* check if we can reuse exisiting connection ... */
			if (zend_hash_find(&EG(persistent_list), hash_key, hash_len + 1, (void **)&le) == SUCCESS) {
				if (Z_TYPE_P(le) == php_le_pmysqli()) {
					plist = (mysqli_plist_entry *) le->ptr;

					do {
						if (zend_ptr_stack_num_elements(&plist->free_links)) {
							if (is_real_connect) {
								/*
								  Gotcha! If there are some options set on the handle with mysqli_options()
								  they will be lost. We will fetch other handle with other options. This could
								  be a source of bug reports of people complaining but...nothing else could be
								  done, if they want PCONN!
								*/
								mysqli_close(mysql->mysql, MYSQLI_CLOSE_IMPLICIT);
							}
							mysql->mysql = zend_ptr_stack_pop(&plist->free_links);

							MyG(num_inactive_persistent)--;
							/* reset variables */
							/* todo: option for ping or change_user */
#if G0
							if (!mysql_change_user(mysql->mysql, username, passwd, dbname)) {
#else
							if (!mysql_ping(mysql->mysql)) {
#endif
#ifdef HAVE_MYSQLND
								mysqlnd_restart_psession(mysql->mysql);
#endif
								MyG(num_active_persistent)++;
								goto end;
							} else {
#if defined(HAVE_MYSQLND)
								mysqlnd_end_psession(mysql->mysql);
#endif	
								mysqli_close(mysql->mysql, MYSQLI_CLOSE_IMPLICIT);
								mysql->mysql = NULL;
							}
						}
					} while (0);
				}
			} else {
				zend_rsrc_list_entry le;
				le.type = php_le_pmysqli();
				le.ptr = plist = calloc(1, sizeof(mysqli_plist_entry));

				zend_ptr_stack_init_ex(&plist->free_links, 1);
				zend_hash_update(&EG(persistent_list), hash_key, hash_len + 1, (void *)&le, sizeof(le), NULL);
			}
		}
	}

	if (MyG(max_links) != -1 && MyG(num_links) >= MyG(max_links)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Too many open links (%ld)", MyG(num_links));
		goto err;
	}
	if (persistent && MyG(max_persistent) != -1 &&
		(MyG(num_active_persistent) + MyG(num_inactive_persistent))>= MyG(max_persistent))
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Too many open persistent links (%ld)",
								MyG(num_active_persistent) + MyG(num_inactive_persistent));
		goto err;
	}
	if (!is_real_connect) {
#if !defined(HAVE_MYSQLND)
		if (!(mysql->mysql = mysql_init(NULL))) {
#else
		if (!(mysql->mysql = mysqlnd_init(persistent))) {
#endif
			goto err;
		}
		new_connection = TRUE;
	}

	if (UG(unicode)) {
		mysql_options(mysql->mysql, MYSQL_SET_CHARSET_NAME, "utf8");
	}


#ifdef HAVE_EMBEDDED_MYSQLI
	if (hostname_len) {
		unsigned int external=1;
		mysql_options(mysql->mysql, MYSQL_OPT_USE_REMOTE_CONNECTION, (char *)&external);
	} else {
		mysql_options(mysql->mysql, MYSQL_OPT_USE_EMBEDDED_CONNECTION, 0);
	}
#endif

#if !defined(HAVE_MYSQLND)
	if (mysql_real_connect(mysql->mysql, hostname, username, passwd, dbname, port, socket, CLIENT_MULTI_RESULTS) == NULL)
#else
	if (mysqlnd_connect(mysql->mysql, hostname, username, passwd, passwd_len, dbname, dbname_len,
						port, socket, flags, MyG(mysqlnd_thd_zval_cache) TSRMLS_CC) == NULL)
#endif
	{
		/* Save error messages */
		php_mysqli_set_error(mysql_errno(mysql->mysql), (char *) mysql_error(mysql->mysql) TSRMLS_CC);
		php_mysqli_throw_sql_exception((char *)mysql_sqlstate(mysql->mysql), mysql_errno(mysql->mysql) TSRMLS_CC,
										"%s", mysql_error(mysql->mysql));
		if (!is_real_connect) {
			/* free mysql structure */
			mysqli_close(mysql->mysql, MYSQLI_CLOSE_DISCONNECTED);
		}
		goto err;
	}

	/* when PHP runs in unicode, set default character set to utf8 */
	if (UG(unicode)) {
		mysql->conv = UG(utf8_conv);
	}

	/* clear error */
	php_mysqli_set_error(mysql_errno(mysql->mysql), (char *) mysql_error(mysql->mysql) TSRMLS_CC);

#if !defined(HAVE_MYSQLND)
	mysql->mysql->reconnect = MyG(reconnect);

	/* set our own local_infile handler */
	php_set_local_infile_handler_default(mysql);
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

#if !defined(HAVE_MYSQLND)
	mysql->multi_query = 0;
#else
	mysql->multi_query = 1;
#endif

	if (!object || !instanceof_function(Z_OBJCE_P(object), mysqli_link_class_entry TSRMLS_CC)) {
		MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_link_class_entry);	
	} else {
		((mysqli_object *) zend_object_store_get_object(object TSRMLS_CC))->ptr = mysqli_resource;
	}
	if (!is_real_connect) {
		return;
	} else {
		RETURN_TRUE;
	}

err:
	if (mysql->hash_key) {
		efree(mysql->hash_key);
		mysql->hash_key = NULL;
	}
	if (!is_real_connect) {
		efree(mysql);
	}
	RETVAL_FALSE;
}


/* {{{ proto object mysqli_connect([string hostname [,string username [,string passwd [,string dbname [,int port [,string socket]]]]]])
   Open a connection to a mysql server */ 
PHP_FUNCTION(mysqli_connect)
{
	mysqli_common_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, FALSE);
}
/* }}} */


/* {{{ proto int mysqli_connect_errno(void) U
   Returns the numerical value of the error message from last connect command */
PHP_FUNCTION(mysqli_connect_errno)
{
	RETURN_LONG(MyG(error_no));
}
/* }}} */

/* {{{ proto string mysqli_connect_error(void) U
   Returns the text of the error message from previous MySQL operation */
PHP_FUNCTION(mysqli_connect_error) 
{
	if (MyG(error_msg)) {
		RETURN_UTF8_STRING((char *)MyG(error_msg), ZSTR_DUPLICATE);
	} else {
		RETURN_NULL();
	}
}
/* }}} */


/* {{{ proto mixed mysqli_fetch_array (object result [,int resulttype]) U
   Fetch a result row as an associative array, a numeric array, or both */
PHP_FUNCTION(mysqli_fetch_array) 
{
#if !defined(HAVE_MYSQLND)
	php_mysqli_fetch_into_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, 0);
#else
	MYSQL_RES		*result;
	zval			*mysql_result;
	long			mode = MYSQLND_FETCH_BOTH;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O|l", &mysql_result, mysqli_result_class_entry, &mode) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, &mysql_result, "mysqli_result", MYSQLI_STATUS_VALID);

	if (mode < MYSQLI_ASSOC || mode > MYSQLI_BOTH) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "The result type should be either MYSQLI_NUM, MYSQLI_ASSOC or MYSQLI_BOTH");
		RETURN_FALSE;
	}

	mysqlnd_fetch_into(result, mode, return_value, MYSQLND_MYSQLI);
#endif
}
/* }}} */

/* {{{ proto mixed mysqli_fetch_assoc (object result) U
   Fetch a result row as an associative array */
PHP_FUNCTION(mysqli_fetch_assoc) 
{
#if !defined(HAVE_MYSQLND)
	php_mysqli_fetch_into_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, MYSQLI_ASSOC, 0);
#else
	MYSQL_RES		*result;
	zval			*mysql_result;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_result, mysqli_result_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, &mysql_result, "mysqli_result", MYSQLI_STATUS_VALID);
	mysqlnd_fetch_into(result, MYSQLND_FETCH_ASSOC, return_value, MYSQLND_MYSQLI);
	
#endif
}
/* }}} */


/* {{{ proto mixed mysqli_fetch_all (object result [,int resulttype]) U
   Fetches all result rows as an associative array, a numeric array, or both */
#if defined(HAVE_MYSQLND)
PHP_FUNCTION(mysqli_fetch_all) 
{
	MYSQL_RES		*result;
	zval			*mysql_result;
	long			mode = MYSQLND_FETCH_NUM;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O|l", &mysql_result, mysqli_result_class_entry, &mode) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(result, MYSQL_RES *, &mysql_result, "mysqli_result", MYSQLI_STATUS_VALID);

	if (!mode || (mode & ~MYSQLND_FETCH_BOTH)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Mode can be only MYSQLI_FETCH_NUM, "
		                 "MYSQLI_FETCH_ASSOC or MYSQLI_FETCH_BOTH");
		RETURN_FALSE;
	}

	mysqlnd_fetch_all(result, mode, return_value);
}
/* }}} */


/* {{{ proto array mysqli_cache_stats(void) U
   Returns statistics about the zval cache */
PHP_FUNCTION(mysqli_get_cache_stats) 
{
	if (ZEND_NUM_ARGS()) {
		WRONG_PARAM_COUNT;
	}
	mysqlnd_palloc_stats(mysqli_mysqlnd_zval_cache, return_value);
}
/* }}} */


/* {{{ proto array mysqli_get_client_stats(void)
   Returns statistics about the zval cache */
PHP_FUNCTION(mysqli_get_client_stats) 
{
	if (ZEND_NUM_ARGS()) {
		WRONG_PARAM_COUNT;
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

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O",
									 &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link", MYSQLI_STATUS_VALID);

	mysqlnd_get_connection_stats(mysql->mysql, return_value);
}
#endif
/* }}} */


/* {{{ proto mixed mysqli_fetch_object (object result [, string class_name [, NULL|array ctor_params]]) U
   Fetch a result row as an object */
PHP_FUNCTION(mysqli_fetch_object) 
{
	php_mysqli_fetch_into_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, MYSQLI_ASSOC, 1);	
/* todo: mysqlnd support */
}
/* }}} */

/* {{{ proto bool mysqli_multi_query(object link, string query) U
   allows to execute multiple queries  */
PHP_FUNCTION(mysqli_multi_query)
{
	MY_MYSQL		*mysql;
	zval			*mysql_link;
	char			*query = NULL;
	unsigned int 	query_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os&", &mysql_link, mysqli_link_class_entry, 
									 &query, &query_len, UG(utf8_conv)) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link", MYSQLI_STATUS_VALID);

	MYSQLI_ENABLE_MQ;	
	if (mysql_real_query(mysql->mysql, query, query_len)) {
#ifndef HAVE_MYSQLND
		char s_error[MYSQL_ERRMSG_SIZE], s_sqlstate[SQLSTATE_LENGTH+1];
		unsigned int s_errno;
		/* we have to save error information, cause 
		MYSQLI_DISABLE_MQ will reset error information */
		strcpy(s_error, mysql_error(mysql->mysql));
		strcpy(s_sqlstate, mysql_sqlstate(mysql->mysql));
		s_errno = mysql_errno(mysql->mysql);
#else
		mysqlnd_error_info error_info = mysql->mysql->error_info;
#endif
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		MYSQLI_DISABLE_MQ;

#ifndef HAVE_MYSQLND
		/* restore error information */
		strcpy(mysql->mysql->net.last_error, s_error);
		strcpy(mysql->mysql->net.sqlstate, s_sqlstate);
		mysql->mysql->net.last_errno = s_errno;	
#else
		mysql->mysql->error_info = error_info;
#endif
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto mixed mysqli_query(object link, string query [,int resultmode]) U */
PHP_FUNCTION(mysqli_query)
{
	MY_MYSQL			*mysql;
	zval				*mysql_link;
	MYSQLI_RESOURCE		*mysqli_resource;
	MYSQL_RES 			*result;
	char				*query = NULL;
	unsigned int 		query_len;
	unsigned long 		resultmode = MYSQLI_STORE_RESULT;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os&|l", &mysql_link, mysqli_link_class_entry, 
									 &query, &query_len, UG(utf8_conv), &resultmode) == FAILURE) {
		return;
	}

	if (!query_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty query");
		RETURN_FALSE;
	}
	if (resultmode != MYSQLI_USE_RESULT && resultmode != MYSQLI_STORE_RESULT
#if defined(HAVE_MYSQLND) && defined(MYSQLND_THREADED)
		&& resultmode != MYSQLI_BG_STORE_RESULT
#endif
	) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid value for resultmode");
		RETURN_FALSE;
	}

	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL*, &mysql_link, "mysqli_link", MYSQLI_STATUS_VALID);

	MYSQLI_DISABLE_MQ;

	if (mysql_real_query(mysql->mysql, query, query_len)) {
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		RETURN_FALSE;
	}

	if (!mysql_field_count(mysql->mysql)) {
		/* no result set - not a SELECT */
		if (MyG(report_mode) & MYSQLI_REPORT_INDEX) {
			php_mysqli_report_index(query, mysqli_server_status(mysql->mysql) TSRMLS_CC);
		}
		RETURN_TRUE;
	}

	switch (resultmode) {
		case MYSQLI_STORE_RESULT:
			result = mysql_store_result(mysql->mysql);
			break;
		case MYSQLI_USE_RESULT:
			result = mysql_use_result(mysql->mysql);
			break;
#if defined(HAVE_MYSQLND) && defined(MYSQLND_THREADED)
		case MYSQLI_BG_STORE_RESULT:
			result = mysqli_bg_store_result(mysql->mysql);
			break;
#endif
	}
	if (!result) {
		php_mysqli_throw_sql_exception((char *)mysql_sqlstate(mysql->mysql), mysql_errno(mysql->mysql) TSRMLS_CC,
										"%s", mysql_error(mysql->mysql)); 
		RETURN_FALSE;
	}

	if (MyG(report_mode) & MYSQLI_REPORT_INDEX) {
		php_mysqli_report_index(query, mysqli_server_status(mysql->mysql) TSRMLS_CC);
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)result;
	mysqli_resource->status = MYSQLI_STATUS_VALID;
	MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_result_class_entry);
}
/* }}} */


#if defined(HAVE_MYSQLND)
/* {{{ proto object mysqli_stmt_get_result(object link) U
   Buffer result set on client */
PHP_FUNCTION(mysqli_stmt_get_result)
{
	MYSQL_RES 		*result;
	MYSQLI_RESOURCE	*mysqli_resource;
	MY_STMT			*stmt;
	zval 			*mysql_stmt;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_stmt, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(stmt, MY_STMT *, &mysql_stmt, "mysqli_stmt", MYSQLI_STATUS_VALID);

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


/* {{{ proto object mysqli_get_warnings(object link) U */
PHP_FUNCTION(mysqli_get_warnings)
{
	MY_MYSQL			*mysql;
	zval				*mysql_link;
	MYSQLI_RESOURCE		*mysqli_resource;
	MYSQLI_WARNING		*w;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL*, &mysql_link, "mysqli_link", MYSQLI_STATUS_VALID);

	if (mysql_warning_count(mysql->mysql)) {
		w = php_get_warnings(mysql->mysql TSRMLS_CC); 
	} else {
		RETURN_FALSE;
	}
	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = mysqli_resource->info = (void *)w;
	mysqli_resource->status = MYSQLI_STATUS_VALID;
	MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_warning_class_entry);	
}
/* }}} */

/* {{{ proto object mysqli_stmt_get_warnings(object link) U */ 
PHP_FUNCTION(mysqli_stmt_get_warnings)
{
	MY_STMT				*stmt;
	zval				*stmt_link;
	MYSQLI_RESOURCE		*mysqli_resource;
	MYSQLI_WARNING		*w;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &stmt_link, mysqli_stmt_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(stmt, MY_STMT*, &stmt_link, "mysqli_stmt", MYSQLI_STATUS_VALID);

	if (mysqli_stmt_warning_count(stmt->stmt)) {
		w = php_get_warnings(mysqli_stmt_get_connection(stmt->stmt) TSRMLS_CC); 
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
/* {{{ proto bool mysqli_set_charset(object link, string csname) U
   sets client character set */
PHP_FUNCTION(mysqli_set_charset)
{
	MY_MYSQL			*mysql;
	zval				*mysql_link;
	char				*cs_name;
	int					csname_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os&", &mysql_link, mysqli_link_class_entry, 
									 &cs_name, &csname_len, UG(utf8_conv)) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL*, &mysql_link, "mysqli_link", MYSQLI_STATUS_VALID);

	/* check unicode modus */
	/* todo: we need also to support UCS2. This will not work when using SET NAMES */
	if (UG(unicode) && (csname_len != 4  || strncasecmp(cs_name, "utf8", 4))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Character set %s is not supported when running PHP with unicode.semantics=On.", cs_name);
		RETURN_FALSE;
	}

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
	char 					*name = NULL, *collation = NULL, *dir = NULL;
	uint					minlength, maxlength, number, state;
#if !defined(HAVE_MYSQLND)
	MY_CHARSET_INFO			cs;
#else
	const MYSQLND_CHARSET	*cs;
#endif

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL*, &mysql_link, "mysqli_link", MYSQLI_STATUS_VALID);

	object_init(return_value);

#if !defined(HAVE_MYSQLND)
	mysql_get_character_set_info(mysql->mysql, &cs);
	name = (char *)cs.csname;
	collation = (char *)cs.name;
	dir = (char *)cs.dir;
	minlength = cs.mbminlen;
	maxlength = cs.mbmaxlen;
	number = cs.number;
	state = cs.state;
#else
	cs = mysql->mysql->charset;
	name = cs->name;	
	collation = cs->collation;	
	minlength = cs->char_minlen;
	maxlength = cs->char_maxlen;
	number = cs->nr;
	state = 1;	/* all charsets are compiled in */
#endif

	if (UG(unicode)) {
		UChar *ustr = NULL;
		int ulen;

		zend_string_to_unicode(UG(utf8_conv), &ustr, &ulen, (name) ? name : "", 
								(name) ? strlen(name) : 0 TSRMLS_CC);
		add_property_unicodel(return_value, "charset", ustr, ulen, 1);
		efree(ustr);
		zend_string_to_unicode(UG(utf8_conv), &ustr, &ulen, (collation) ? collation : "", 
								(collation) ? strlen(collation) : 0 TSRMLS_CC);
		add_property_unicodel(return_value, "collation", ustr, ulen, 1);
		efree(ustr);
		zend_string_to_unicode(UG(utf8_conv), &ustr, &ulen, (dir) ? dir : "", 
								(dir) ? strlen(dir) : 0 TSRMLS_CC);
		add_property_unicodel(return_value, "dir", ustr, ulen, 1);
		efree(ustr);
	} else {
		add_property_string(return_value, "charset", (name) ? (char *)name : "", 1);
		add_property_string(return_value, "collation",(collation) ? (char *)collation : "", 1);
		add_property_string(return_value, "dir", (dir) ? (char *)dir : "", 1);
	}
	add_property_long(return_value, "min_length", minlength);
	add_property_long(return_value, "max_length", maxlength);
	add_property_long(return_value, "number", number);
	add_property_long(return_value, "state", state);
}
/* }}} */
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
