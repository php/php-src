/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2007 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
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

/* {{{ proto object mysqli_connect([string hostname [,string username [,string passwd [,string dbname [,int port [,string socket]]]]]]) U
   Open a connection to a mysql server */ 
PHP_FUNCTION(mysqli_connect)
{
	MY_MYSQL 			*mysql;
	MYSQLI_RESOURCE 	*mysqli_resource;
	zval  				*object = getThis();
	char				*hostname, *username, *passwd, *dbname, *socket;
	int					hostname_len, username_len, passwd_len, dbname_len, socket_len;
	long				port=0;
#if MYSQL_VERSION_ID > 50012
	my_bool				my_true = 1;
#endif

	if (getThis() && !ZEND_NUM_ARGS()) {
		RETURN_NULL();
	}

	hostname = username = dbname = passwd = socket = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s&s&s&s&ls&", &hostname, &hostname_len, UG(utf8_conv),
							  &username, &username_len, UG(utf8_conv), &passwd, &passwd_len, UG(utf8_conv), 
							  &dbname, &dbname_len, UG(utf8_conv), &port, &socket, &socket_len, UG(utf8_conv)) == FAILURE) {
		return;
	}

	if (!socket_len) {
		socket = NULL;
	}

	if (!passwd) {
		passwd = MyG(default_pw);
		if (!username){
			username = MyG(default_user);
			if (!hostname) {
				hostname = MyG(default_host);
			}
		}
	}


	mysql = (MY_MYSQL *) ecalloc(1, sizeof(MY_MYSQL));

	if (!(mysql->mysql = mysql_init(NULL))) {
		efree(mysql);
		RETURN_FALSE;
	}

#ifdef HAVE_EMBEDDED_MYSQLI
	if (hostname_len) {
		unsigned int external=1;
		mysql_options(mysql->mysql, MYSQL_OPT_USE_REMOTE_CONNECTION, (char *)&external);
	} else {
		mysql_options(mysql->mysql, MYSQL_OPT_USE_EMBEDDED_CONNECTION, 0);
	}
#endif

#if MYSQL_VERSION_ID > 50012
	mysql_options(mysql->mysql, MYSQL_OPT_RECONNECT, (const char *)&my_true);
#endif

	if (!socket) {
		socket = MyG(default_socket);
	}

	if (mysql_real_connect(mysql->mysql, hostname, username, passwd, dbname, port, socket, CLIENT_MULTI_RESULTS) == NULL) {
		/* Save error messages */

		php_mysqli_throw_sql_exception( mysql->mysql->net.sqlstate, mysql->mysql->net.last_errno TSRMLS_CC,
										"%s", mysql->mysql->net.last_error);

		php_mysqli_set_error(mysql_errno(mysql->mysql), (char *) mysql_error(mysql->mysql) TSRMLS_CC);

		/* free mysql structure */
		mysql_close(mysql->mysql);
		efree(mysql);
		RETURN_FALSE;
	}

#ifdef HAVE_MYSQLI_SET_CHARSET
	/* when PHP runs in unicode, set default character set to utf8 */
	if (UG(unicode)) {
		mysql_set_character_set(mysql->mysql, "utf8");
		mysql->conv = UG(utf8_conv);
	}
#endif

	/* clear error */
	php_mysqli_set_error(mysql_errno(mysql->mysql), (char *) mysql_error(mysql->mysql) TSRMLS_CC);

	mysql->mysql->reconnect = MyG(reconnect);

	/* set our own local_infile handler */
	php_set_local_infile_handler_default(mysql);

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)mysql;
	mysqli_resource->status = MYSQLI_STATUS_VALID;

	if (!object || !instanceof_function(Z_OBJCE_P(object), mysqli_link_class_entry TSRMLS_CC)) {
		MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_link_class_entry);	
	} else {
		((mysqli_object *) zend_object_store_get_object(object TSRMLS_CC))->ptr = mysqli_resource;
	}
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
	php_mysqli_fetch_into_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, 0);
}
/* }}} */

/* {{{ proto mixed mysqli_fetch_assoc (object result) U
   Fetch a result row as an associative array */
PHP_FUNCTION(mysqli_fetch_assoc) 
{
	php_mysqli_fetch_into_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, MYSQLI_ASSOC, 0);
}
/* }}} */

/* {{{ proto mixed mysqli_fetch_object (object result [, string class_name [, NULL|array ctor_params]]) U
   Fetch a result row as an object */
PHP_FUNCTION(mysqli_fetch_object) 
{
	php_mysqli_fetch_into_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, MYSQLI_ASSOC, 1);	
}
/* }}} */

/* {{{ proto bool mysqli_multi_query(object link, string query) U
   allows to execute multiple queries  */
PHP_FUNCTION(mysqli_multi_query)
{
	MY_MYSQL		*mysql;
	zval			*mysql_link;
	char			*query;
	int				query_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os&", &mysql_link, mysqli_link_class_entry, 
									 &query, &query_len, UG(utf8_conv)) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &mysql_link, "mysqli_link", MYSQLI_STATUS_VALID);

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

/* {{{ proto mixed mysqli_query(object link, string query [,int resultmode]) U */
PHP_FUNCTION(mysqli_query)
{
	MY_MYSQL			*mysql;
	zval				*mysql_link;
	MYSQLI_RESOURCE		*mysqli_resource;
	MYSQL_RES 			*result;
	char				*query;
	int					query_len;
	unsigned long 		resultmode = MYSQLI_STORE_RESULT;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os&|l", &mysql_link, mysqli_link_class_entry, 
									 &query, &query_len, UG(utf8_conv), &resultmode) == FAILURE) {
		return;
	}

	if (resultmode != MYSQLI_USE_RESULT && resultmode != MYSQLI_STORE_RESULT) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid value for resultmode");
		RETURN_FALSE;
	}

	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL*, &mysql_link, "mysqli_link", MYSQLI_STATUS_VALID);

	if (!query_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty query");
		RETURN_FALSE;
	}
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
		php_mysqli_throw_sql_exception(mysql->mysql->net.sqlstate, mysql->mysql->net.last_errno TSRMLS_CC,
										"%s", mysql->mysql->net.last_error); 
		RETURN_FALSE;
	}

	if (MyG(report_mode) & MYSQLI_REPORT_INDEX) {
		php_mysqli_report_index((char *)query, mysql->mysql->server_status TSRMLS_CC);
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)result;
	mysqli_resource->status = MYSQLI_STATUS_VALID;
	MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_result_class_entry);
}
/* }}} */

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
		w = php_get_warnings(mysql->mysql); 
	} else {
		RETURN_FALSE;
	}
	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = mysqli_resource->info = (void *)w;
	mysqli_resource->status = MYSQLI_STATUS_INITIALIZED;
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

	if (mysql_warning_count(stmt->stmt->mysql)) {
		w = php_get_warnings(stmt->stmt->mysql); 
	} else {
		RETURN_FALSE;
	}
	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = mysqli_resource->info = (void *)w;
	mysqli_resource->status = MYSQLI_STATUS_INITIALIZED;
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
	char				*csname;
	int					csname_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os&", &mysql_link, mysqli_link_class_entry, 
									 &csname, &csname_len, UG(utf8_conv)) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL*, &mysql_link, "mysqli_link", MYSQLI_STATUS_VALID);

	/* check unicode modus */
	/* todo: we need also to support UCS2. This will not work when using SET NAMES */
	if (UG(unicode) && (csname_len != 4  || strncasecmp(csname, "utf8", 4))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Character set %s is not supported when running PHP with unicode.semantics=On.", csname);
		RETURN_FALSE;
	}

	if (mysql_set_character_set(mysql->mysql, csname)) {
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
	MY_CHARSET_INFO			cs;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL*, &mysql_link, "mysqli_link", MYSQLI_STATUS_VALID);

	object_init(return_value);

	mysql_get_character_set_info(mysql->mysql, &cs);

	if (UG(unicode)) {
		UErrorCode status = U_ZERO_ERROR;
		UChar *ustr;
		int ulen;

		zend_string_to_unicode(UG(utf8_conv), &ustr, &ulen, (cs.csname) ? cs.csname : "", 
								(cs.csname) ? strlen(cs.csname) : 0 TSRMLS_CC);
		add_property_unicodel(return_value, "charset", ustr, ulen, 1);
		zend_string_to_unicode(UG(utf8_conv), &ustr, &ulen, (cs.name) ? cs.name : "", 
								(cs.name) ? strlen(cs.name) : 0 TSRMLS_CC);
		add_property_unicodel(return_value, "collation", ustr, ulen, 1);
		zend_string_to_unicode(UG(utf8_conv), &ustr, &ulen, (cs.comment) ? cs.comment : "", 
								(cs.comment) ? strlen(cs.comment) : 0 TSRMLS_CC);
		add_property_unicodel(return_value, "comment", ustr, ulen, 1);
		zend_string_to_unicode(UG(utf8_conv), &ustr, &ulen, (cs.dir) ? cs.dir : "", 
								(cs.dir) ? strlen(cs.dir) : 0 TSRMLS_CC);
		add_property_unicodel(return_value, "dir", ustr, ulen, 1);
	} else {
		add_property_string(return_value, "charset", (cs.name) ? (char *)cs.csname : "", 1);
		add_property_string(return_value, "collation",(cs.name) ? (char *)cs.name : "", 1);
		add_property_string(return_value, "comment", (cs.comment) ? (char *)cs.comment : "", 1);
		add_property_string(return_value, "dir", (cs.dir) ? (char *)cs.dir : "", 1);
	}
	add_property_long(return_value, "min_length", cs.mbminlen);
	add_property_long(return_value, "max_length", cs.mbmaxlen);
	add_property_long(return_value, "number", cs.number);
	add_property_long(return_value, "state", cs.state);
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
