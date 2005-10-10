/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2004 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Georg Richter <georg@php.net>                                |
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
#include "php_mysqli.h"

/* {{{ proto object mysqli_connect([string hostname [,string username [,string passwd [,string dbname [,int port [,string socket]]]]]])
   Open a connection to a mysql server */ 
PHP_FUNCTION(mysqli_connect)
{
	MY_MYSQL 			*mysql;
	MYSQLI_RESOURCE 	*mysqli_resource;
	zval  				*object = getThis();
	char 				*hostname = NULL, *username=NULL, *passwd=NULL, *dbname=NULL, *socket=NULL;
	unsigned int 		hostname_len = 0, username_len = 0, passwd_len = 0, dbname_len = 0, socket_len = 0;
	long				port=0;

	if (getThis() && !ZEND_NUM_ARGS()) {
		RETURN_NULL();
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|ssssls", &hostname, &hostname_len, &username, &username_len, 
		&passwd, &passwd_len, &dbname, &dbname_len, &port, &socket, &socket_len) == FAILURE) {
		return;
	}

	if (!socket_len) {
		socket = NULL;
	}

	/* TODO: safe mode handling */
	if (PG(sql_safe_mode)){
	} else {
		if (!passwd) {
			passwd = MyG(default_pw);
			if (!username){
				username = MyG(default_user);
				if (!hostname) {
					hostname = MyG(default_host);
				}
			}
		}
	}

	mysql = (MY_MYSQL *)ecalloc(1, sizeof(MY_MYSQL));

	if (!(mysql->mysql = mysql_init(NULL))) {
		efree(mysql);
		RETURN_FALSE;
	}

	if (!socket) {
		socket = MyG(default_socket);
	}

	if (mysql_real_connect(mysql->mysql,hostname,username,passwd,dbname,port,socket,CLIENT_MULTI_RESULTS) == NULL) {
		/* Save error messages */

		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		php_mysqli_set_error(mysql_errno(mysql->mysql), (char *) mysql_error(mysql->mysql) TSRMLS_CC);

		if (!(MyG(report_mode) & MYSQLI_REPORT_ERROR)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", mysql_error(mysql->mysql));
		}
		/* free mysql structure */
		mysql_close(mysql->mysql);
		efree(mysql);
		RETURN_FALSE;
	}

	/* clear error */
	php_mysqli_set_error(mysql_errno(mysql->mysql), (char *) mysql_error(mysql->mysql) TSRMLS_CC);

	mysql->mysql->reconnect = MyG(reconnect);

	/* set our own local_infile handler */
	php_set_local_infile_handler_default(mysql);

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)mysql;

	if (!object || !instanceof_function(Z_OBJCE_P(object), mysqli_link_class_entry TSRMLS_CC)) {
		MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_link_class_entry);	
	} else {
		((mysqli_object *) zend_object_store_get_object(object TSRMLS_CC))->ptr = mysqli_resource;
		((mysqli_object *) zend_object_store_get_object(object TSRMLS_CC))->valid = 1;
	}
}
/* }}} */

#ifdef HAVE_EMBEDDED_MYSQLI
/* {{{ proto object mysqli_embedded_connect(void)
   Open a connection to a embedded mysql server */ 
PHP_FUNCTION(mysqli_embedded_connect)
{
	MY_MYSQL 			*mysql;
	MYSQLI_RESOURCE 	*mysqli_resource;
	zval  				*object = getThis();
	char				*dbname = NULL;
	int					dblen = 0;

	if (!MyG(embedded)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Embedded server was not initialized.");
		RETURN_FALSE;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &dbname,  &dblen) == FAILURE) {
		return;
	}

	mysql = (MY_MYSQL *) ecalloc(1, sizeof(MY_MYSQL));

	if (!(mysql = mysql_init(NULL))) {
		efree(mysql);
		RETURN_FALSE;
	}

	if (mysql_real_connect(mysql, NULL, NULL, NULL, dbname, 0, NULL, 0) == NULL) {
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		php_mysqli_set_error(mysql_errno(mysql->mysql), (char *) mysql_error(mysql->mysql) TSRMLS_CC);

		if (!(MyG(report_mode) & MYSQLI_REPORT_ERROR)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", mysql_error(mysql->mysql));
		}
		/* free mysql structure */
		mysql_close(mysql->mysql);
		efree(mysql);
		RETURN_FALSE;
	}

	php_mysqli_set_error(mysql_errno(mysql->mysql), (char *) mysql_error(mysql->mysql) TSRMLS_CC);

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)mysql;

	if (!object) {
		MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_link_class_entry);	
	} else {
		((mysqli_object *) zend_object_store_get_object(object TSRMLS_CC))->ptr = mysqli_resource;
	}
}
/* }}} */
#endif

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
		RETURN_STRING(MyG(error_msg),1);
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ proto mixed mysqli_fetch_array (object result [,int resulttype])
   Fetch a result row as an associative array, a numeric array, or both */
PHP_FUNCTION(mysqli_fetch_array) 
{
	php_mysqli_fetch_into_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, 0);
}
/* }}} */

/* {{{ proto mixed mysqli_fetch_assoc (object result)
   Fetch a result row as an associative array */
PHP_FUNCTION(mysqli_fetch_assoc) 
{
	php_mysqli_fetch_into_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, MYSQLI_ASSOC, 0);
}
/* }}} */

/* {{{ proto mixed mysqli_fetch_object (object result [, string class_name [, NULL|array ctor_params]])
   Fetch a result row as an object */
PHP_FUNCTION(mysqli_fetch_object) 
{
	php_mysqli_fetch_into_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, MYSQLI_ASSOC, 1);	
}
/* }}} */

/* {{{ proto bool mysqli_multi_query(object link, string query)
   Binary-safe version of mysql_query() */
PHP_FUNCTION(mysqli_multi_query)
{
	MY_MYSQL		*mysql;
	zval			*mysql_link;
	char			*query = NULL;
	unsigned long 	query_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &mysql_link, mysqli_link_class_entry, &query, &query_len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link");

	MYSQLI_ENABLE_MQ;	
	if (mysql_real_query(mysql->mysql, query, query_len)) {
		char s_error[MYSQL_ERRMSG_SIZE], s_sqlstate[SQLSTATE_LENGTH+1];
		unsigned int s_errno;
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);

		/* we have to save error information, cause 
		MYSQLI_DISABLE_MQ will reset error information */
		strcpy(s_error, mysql_error(mysql->mysql));
		strcpy(s_sqlstate, mysql_sqlstate(mysql->mysql));
		s_errno = mysql_errno(mysql->mysql);

		MYSQLI_DISABLE_MQ;

		/* restore error information */
		strcpy(mysql->mysql->net.last_error, s_error);
		strcpy(mysql->mysql->net.sqlstate, s_sqlstate);
		mysql->mysql->net.last_errno = s_errno;	

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
	MYSQL_RES 			*result;
	char				*query = NULL;
	unsigned int 		query_len;
	unsigned long 		resultmode = MYSQLI_STORE_RESULT;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os|l", &mysql_link, mysqli_link_class_entry, &query, &query_len, &resultmode) == FAILURE) {
		return;
	}
	if (!query_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty query");
		RETURN_FALSE;
	}
	if (resultmode != MYSQLI_USE_RESULT && resultmode != MYSQLI_STORE_RESULT) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid value for resultmode");
		RETURN_FALSE;
	}

	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL*, &mysql_link, "mysqli_link");

	MYSQLI_DISABLE_MQ;

	if (mysql_real_query(mysql->mysql, query, query_len)) {
		MYSQLI_REPORT_MYSQL_ERROR(mysql->mysql);
		RETURN_FALSE;
	}

	if (!mysql_field_count(mysql->mysql)) {
		/* no result set - not a SELECT */
		if (MyG(report_mode) & MYSQLI_REPORT_INDEX) {
			php_mysqli_report_index(query, mysql->mysql->server_status TSRMLS_CC);
		}
		RETURN_TRUE;
	}

	result = (resultmode == MYSQLI_USE_RESULT) ? mysql_use_result(mysql->mysql) : mysql_store_result(mysql->mysql);

	if (!result) {
		RETURN_FALSE;
	}

	if (MyG(report_mode) & MYSQLI_REPORT_INDEX) {
		php_mysqli_report_index(query, mysql->mysql->server_status TSRMLS_CC);
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)result;
	MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_result_class_entry);
}
/* }}} */

#ifdef HAVE_MYSQLI_SET_CHARSET
/* {{{ proto bool mysqli_set_charset(object link, string csname)
   sets client character set */
PHP_FUNCTION(mysqli_set_charset)
{
	MY_MYSQL			*mysql;
	zval				*mysql_link;
	char				*cs_name = NULL;
	unsigned int 		len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &mysql_link, mysqli_link_class_entry, &cs_name, &len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL*, &mysql_link, "mysqli_link");

	if (mysql_set_character_set(mysql->mysql, cs_name))
	{
		RETURN_FALSE;
	}
	RETURN_TRUE;
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
