/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
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

/* {{{ proto resource mysqli_connect([string hostname [,string username [,string passwd [,string dbname [,int port [,string socket]]]]]])
   Open a connection to a mysql server */ 
PHP_FUNCTION(mysqli_connect)
{
	MYSQL 				*mysql;
	MYSQLI_RESOURCE 	*mysqli_resource;
	PR_MYSQL			*prmysql = NULL;
	zval  				*object = getThis();
	char 				*hostname = NULL, *username=NULL, *passwd=NULL, *dbname=NULL, *socket=NULL;
	unsigned int 		hostname_len, username_len, passwd_len, dbname_len, socket_len;
	unsigned int 		port=0;
	struct timeval		starttime;

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

	if (MyG(profiler)){
		gettimeofday(&starttime, NULL);
	}

	if (mysql_real_connect(mysql,hostname,username,passwd,dbname,port,socket,0) == NULL) {
		/* Save error messages */

		php_mysqli_set_error(mysql_errno(mysql), (char *) mysql_error(mysql) TSRMLS_CC);

		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", mysql_error(mysql));
		/* free mysql structure */
		mysql_close(mysql);
		RETURN_FALSE;
	}

	if (MyG(profiler)) {
		prmysql = (PR_MYSQL *)MYSQLI_PROFILER_NEW(prmain, MYSQLI_PR_MYSQL, 0);
		php_mysqli_profiler_timediff(starttime, &prmysql->header.elapsedtime);
		MYSQLI_PROFILER_STARTTIME(prmysql);
		prmysql->hostname = estrdup(hostname);
		prmysql->username = estrdup(username);
		prmysql->thread_id = mysql->thread_id;
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)mysql;
	mysqli_resource->prinfo = prmysql;


	if (!object) {
		MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_link_class_entry);	
	} else {
		((mysqli_object *) zend_object_store_get_object(object TSRMLS_CC))->ptr = mysqli_resource;
	}
}
/* }}} */

/* {{{ proto array mysqli_fetch_array (resource result [,int resulttype])
   Fetch a result row as an associative array, a numeric array, or both */
PHP_FUNCTION(mysqli_fetch_array) 
{
	php_mysqli_fetch_into_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto array mysqli_fetch_assoc (resource result)
   Fetch a result row as an associative array */
PHP_FUNCTION(mysqli_fetch_assoc) 
{
	php_mysqli_fetch_into_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, MYSQLI_ASSOC);
}
/* }}} */

/* {{{ proto array mysqli_fetch_object (resource result)
   Fetch a result row as an object */
PHP_FUNCTION(mysqli_fetch_object) 
{
	php_mysqli_fetch_into_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, MYSQLI_ASSOC);
	
	if (Z_TYPE_P(return_value) == IS_ARRAY) {
		object_and_properties_init(return_value, ZEND_STANDARD_CLASS_DEF_PTR, Z_ARRVAL_P(return_value));
	}
}
/* }}} */

/* {{{ proto resource mysqli_query(resource link, string query [,int resultmode])
   Send a MySQL query */
PHP_FUNCTION(mysqli_query) {
	MYSQL				*mysql;
	zval				*mysql_link;
	MYSQLI_RESOURCE		*mysqli_resource;
	MYSQL_RES 			*result;
	PR_MYSQL			*prmysql;
	PR_QUERY			*prquery;
	PR_RESULT			*prresult;
	char				*query = NULL;
	unsigned int 		query_len;
	unsigned int 		resultmode = 0;
	struct timeval		starttime;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os|l", &mysql_link, mysqli_link_class_entry, &query, &query_len, &resultmode) == FAILURE) {
		return;
	}
	MYSQLI_FETCH_RESOURCE(mysql, MYSQL*, prmysql, PR_MYSQL *, &mysql_link, "mysqli_link");

	/* profiling information */
	if (MyG(profiler)) {
		prquery = (PR_QUERY *)MYSQLI_PROFILER_NEW(prmysql, MYSQLI_PR_QUERY, 1);
		prquery->explain.query = my_estrdup(query);
		if (!strncasecmp("select", query, 6)){
			if (!(MYSQLI_PROFILER_EXPLAIN(&prquery->explain, &prquery->header, mysql, query))) {
				RETURN_FALSE;
			}
		}
	}

	if (mysql_real_query(mysql, query, query_len)) {
		RETURN_FALSE;
	}

	if (MyG(profiler)) {
		MYSQLI_PROFILER_ELAPSEDTIME(prquery);
		prquery->insertid = mysql_insert_id(mysql);
		prquery->affectedrows = mysql_affected_rows(mysql);
	}

	if (!mysql_field_count(mysql)) {
		RETURN_FALSE;
	}

	/* profiler result information */
	if (MyG(profiler)) {
		gettimeofday(&starttime, NULL);
		prresult = (PR_RESULT *)MYSQLI_PROFILER_NEW(prquery, MYSQLI_PR_RESULT, 1);
	}

	result = (resultmode == MYSQLI_USE_RESULT) ? mysql_use_result(mysql) : mysql_store_result(mysql);

	if (result && MyG(profiler)) {
		MYSQLI_PROFILER_ELAPSEDTIME(prresult);
		prresult->rows = result->row_count;
		prresult->columns = result->field_count;
	}
		
	if (!result) {
		RETURN_FALSE;
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = (void *)result;
	mysqli_resource->prinfo = (void *)prresult;
	MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_result_class_entry);
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
