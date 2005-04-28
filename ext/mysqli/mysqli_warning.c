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

*/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <signal.h>

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_mysqli.h"

/* {{{ void php_clear_warnings() */
void php_clear_warnings(MYSQLI_WARNING *w)
{
	if (w->result) {
		mysql_free_result(w->result);
	}
	efree(w);
}
/* }}} */

/* {{{ void php_get_warnings(MYSQL *mysql) */
MYSQLI_WARNING *php_get_warnings(MYSQL *mysql)
{
	MYSQLI_WARNING *w;
	int cwarnings;

	if (!(cwarnings = mysql_warning_count(mysql))) {
		return NULL;
	}

	if (mysql_query(mysql, "SHOW WARNINGS")) {
		return NULL;
	}

	w = (MYSQLI_WARNING *)ecalloc(sizeof(MYSQLI_WARNING), 1);

	w->warning_count = cwarnings;
	w->result = mysql_store_result(mysql);
	if (!(w->row = mysql_fetch_row(w->result))) {
		mysql_free_result(w->result);
		efree(w);
		return NULL;
	}

	return w;
}
/* }}} */

/* {{{ mysqli_warning::__construct */
ZEND_FUNCTION(mysqli_warning_construct)
{
	MYSQL			*mysql = NULL;
	MYSQLI_WARNING  *w;
	MYSQLI_RESOURCE	*mysqli_resource;
	mysqli_object	*obj;

	if (!getThis()) {
		RETURN_FALSE;
	} 

	obj = (mysqli_object *)zend_objects_get_address(getThis() TSRMLS_CC);

	if (obj->zo.ce == mysqli_link_class_entry) {
		mysql = (MYSQL *)((MY_MYSQL *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr)->mysql;
	} else if (obj->zo.ce == mysqli_stmt_class_entry) {
		mysql = (MYSQL *)((MY_STMT *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr)->stmt->mysql;
	}

	if ((w = php_get_warnings(mysql))) {
		mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
		mysqli_resource->ptr = (void *)w;
		obj->valid = 1;

		MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_warning_class_entry);	
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ bool mysqli_warning::next */
ZEND_FUNCTION(mysqli_warning_next) {
	MYSQLI_WARNING 	*w;
	zval  			*mysql_warning;
	mysqli_object *obj = (mysqli_object *)zend_objects_get_address(getThis() TSRMLS_CC);

	if (obj->valid) {
		if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", 
										 &mysql_warning, mysqli_warning_class_entry) == FAILURE) {
			return;
		}

		MYSQLI_FETCH_RESOURCE(w, MYSQLI_WARNING *, &mysql_warning, "mysqli_warning");

		if (w->warning_count && (w->row = mysql_fetch_row(w->result))) {
			RETURN_TRUE;
		}
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ property mysqli_warning_error */
int mysqli_warning_error(mysqli_object *obj, zval **retval TSRMLS_DC)
{
	MYSQLI_WARNING *w;

	ALLOC_ZVAL(*retval);
	w = obj->valid ? (MYSQLI_WARNING *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr : NULL;

	if (w && w->row && w->row[2]) {
		ZVAL_STRING(*retval, w->row[2], 1);
	} else {
		ZVAL_NULL(*retval);
	}
	return SUCCESS;
}
/* }}} */

/* {{{ property mysqli_warning_error */
int mysqli_warning_errno(mysqli_object *obj, zval **retval TSRMLS_DC)
{
	MYSQLI_WARNING *w;

	ALLOC_ZVAL(*retval);
	w = obj->valid ? (MYSQLI_WARNING *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr : NULL;

	if (w && w->row && w->row[1]) {
		ZVAL_LONG(*retval, atoi(w->row[1]));
	} else {
		ZVAL_NULL(*retval);
	}
	return SUCCESS;
}
/* }}} */

/* {{{ mysqli_warning_methods[]
 */
function_entry mysqli_warning_methods[] = {
	PHP_FALIAS(__construct,mysqli_warning_construct, NULL)
	PHP_FALIAS(next,mysqli_warning_next,NULL)
	{NULL, NULL, NULL}
};
/* }}} */


mysqli_property_entry mysqli_warning_property_entries[] = {
	{"error", mysqli_warning_error, NULL},
	{"errno", mysqli_warning_errno, NULL},
	{NULL, NULL, NULL}
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
