/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2009 The PHP Group                                |
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

#ifdef HAVE_LIBMYSQL_REPLICATION
/* {{{ proto void mysqli_disable_reads_from_master(object link)
*/
PHP_FUNCTION(mysqli_disable_reads_from_master)
{
	MYSQL		*mysql;
	zval		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, &mysql_link, "mysqli_link", MYSQLI_STATUS_VALID);
	mysql_disable_reads_from_master(mysql);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void mysqli_disable_rpl_parse(object link)
*/
PHP_FUNCTION(mysqli_disable_rpl_parse)
{
	MYSQL		*mysql;
	zval    	*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, &mysql_link, "mysqli_link", MYSQLI_STATUS_VALID);
	mysql_disable_rpl_parse(mysql);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void mysqli_enable_reads_from_master(object link)
*/
PHP_FUNCTION(mysqli_enable_reads_from_master)
{
	MYSQL		*mysql;
	zval		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, &mysql_link, "mysqli_link", MYSQLI_STATUS_VALID);

	mysql_enable_reads_from_master(mysql);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void mysqli_enable_rpl_parse(object link)
*/
PHP_FUNCTION(mysqli_enable_rpl_parse)
{
	MYSQL		*mysql;
	zval		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, &mysql_link, "mysqli_link", MYSQLI_STATUS_VALID);

	mysql_enable_rpl_parse(mysql);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool mysqli_master_query(object link, string query)
   Enforce execution of a query on the master in a master/slave setup */
PHP_FUNCTION(mysqli_master_query) {
	MYSQL			*mysql;
	zval			*mysql_link;
	char			*query = NULL;
	unsigned int 	query_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &mysql_link, mysqli_link_class_entry, &query, &query_len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, &mysql_link, "mysqli_link", MYSQLI_STATUS_VALID);

	if (mysql_master_query(mysql, query, query_len)) {
		RETURN_FALSE;
	}	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int mysqli_rpl_parse_enabled(object link)
*/
PHP_FUNCTION(mysqli_rpl_parse_enabled)
{
	MYSQL		*mysql;
	zval  		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, &mysql_link, "mysqli_link", MYSQLI_STATUS_VALID);

	RETURN_LONG(mysql_rpl_parse_enabled(mysql));
}
/* }}} */

/* {{{ proto bool mysqli_rpl_probe(object link)
*/
PHP_FUNCTION(mysqli_rpl_probe)
{
	MYSQL		*mysql;
	zval  		*mysql_link;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &mysql_link, mysqli_link_class_entry) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, &mysql_link, "mysqli_link", MYSQLI_STATUS_VALID);


	if (mysql_rpl_probe(mysql)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int mysqli_rpl_query_type(string query)
*/
PHP_FUNCTION(mysqli_rpl_query_type)
{
	MYSQL		*mysql;
	zval		*mysql_link;
	char		*query;
	int	 		query_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &mysql_link, mysqli_link_class_entry, &query, &query_len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, &mysql_link, "mysqli_link", MYSQLI_STATUS_VALID);

	RETURN_LONG(mysql_rpl_query_type(query, query_len));
}
/* }}} */

/* {{{ proto bool mysqli_send_query(object link, string query)
*/
PHP_FUNCTION(mysqli_send_query)
{
	MYSQL			*mysql;
	zval			*mysql_link;
	char			*query = NULL;
	unsigned int 	query_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &mysql_link, mysqli_link_class_entry, &query, &query_len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, &mysql_link, "mysqli_link", MYSQLI_STATUS_VALID);

	if (mysql_send_query(mysql, query, query_len)) {
		RETURN_FALSE;
	}	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool mysqli_slave_query(object link, string query)
   Enforce execution of a query on a slave in a master/slave setup */
PHP_FUNCTION(mysqli_slave_query)
{
	MYSQL			*mysql;
	zval			*mysql_link;
	char			*query = NULL;
	unsigned int	query_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &mysql_link, mysqli_link_class_entry, &query, &query_len) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL *, &mysql_link, "mysqli_link", MYSQLI_STATUS_VALID);

	if (mysql_slave_query(mysql, query, query_len)) {
		RETURN_FALSE;
	}	
	RETURN_TRUE;
}
/* }}} */

#endif /* HAVE_LIBMYSQL_REPLICATION */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
