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

/* {{{ proto resource mysqli_connect([string hostname, [string username, [string passwd, [string dbname, [int port, [string socket]]]]])
   open a connection to a mysql server */ 
PHP_FUNCTION(mysqli_connect)
{
	MYSQL *mysql;
	char *hostname = NULL, *username=NULL, *passwd=NULL, *dbname=NULL, *socket=NULL;
	unsigned int hostname_len, username_len, passwd_len, dbname_len, socket_len;
	unsigned int port=0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|ssssls", &hostname, &hostname_len, &username, &username_len, 
		&passwd, &passwd_len, &dbname, &dbname_len, &port, &socket, &socket_len) == FAILURE) {
		return;
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
	mysql = mysql_init(NULL);

	if (mysql_real_connect(mysql,hostname,username,passwd,dbname,port,socket,0) == NULL) {
		/* Save error messages */
		php_mysqli_set_error(mysql_errno(mysql), (char *mysql_error(mysql) TSRMLS_CC);

		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", mysql_error(mysql));
		/* free mysql structure */
		mysql_close(mysql);
		RETURN_FALSE;
	}

	MYSQLI_RETURN_RESOURCE(mysql, mysqli_link_class_entry);	
}
/* }}} */

/* {{{ proto array mysqli_fetch_array (resource result, [int resulttype])
*/
PHP_FUNCTION(mysqli_fetch_array) 
{
	php_mysqli_fetch_into_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto array mysqli_fetch_assoc (resource result)
*/
PHP_FUNCTION(mysqli_fetch_assoc) 
{
	php_mysqli_fetch_into_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, MYSQLI_ASSOC);
}
/* }}} */

/* {{{ proto array mysqli_fetch_object (resource result)
*/
PHP_FUNCTION(mysqli_fetch_object) 
{
	php_mysqli_fetch_into_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, MYSQLI_ASSOC);
	
	if (Z_TYPE_P(return_value) == IS_ARRAY) {
		object_and_properties_init(return_value, ZEND_STANDARD_CLASS_DEF_PTR, Z_ARRVAL_P(return_value));
	}
}
/* }}} */

/* {{{ proto resource mysqli_query(resource link, string query, [int resultmode])
*/
PHP_FUNCTION(mysqli_query) {
	MYSQL	*mysql;
	zval	*mysql_link;
	MYSQL_RES *result;
	char	*query = NULL;
	unsigned int query_len;
	unsigned int resultmode = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os|l", &mysql_link, mysqli_link_class_entry, &query, &query_len, &resultmode) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, &mysql_link, "mysqli_link"); 

	if (mysql_real_query(mysql, query, query_len)) {
		RETURN_FALSE;
	}

	if (!mysql_field_count(mysql)) {
		RETURN_FALSE;
	}

	result = (resultmode == MYSQLI_USE_RESULT) ? mysql_use_result(mysql) : mysql_store_result(mysql);
	if (!result) {
		RETURN_FALSE;
	}	
	MYSQLI_RETURN_RESOURCE(result, mysqli_result_class_entry);
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
