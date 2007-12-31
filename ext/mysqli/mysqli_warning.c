/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
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
	MYSQLI_WARNING	*n;

	while (w) {
		n = w;
		efree(w->reason);
		w = w->next;
		efree(n);
	} 
}
/* }}} */

/* {{{ MYSQLI_WARNING *php_new_warning */
MYSQLI_WARNING *php_new_warning(char *reason, char *sqlstate, int errorno)
{
	MYSQLI_WARNING	*w;

	w = (MYSQLI_WARNING *)ecalloc(1, sizeof(MYSQLI_WARNING));

	w->reason = safe_estrdup(reason);
	if (sqlstate) {
		strcpy(w->sqlstate, sqlstate);
	} else {
		strcpy(w->sqlstate, "00000");
	}
	w->errorno = errorno;

	return w;
}
/* }}} */

/* {{{ MYSQLI_WARNING *php_get_warnings(MYSQL *mysql) */
MYSQLI_WARNING *php_get_warnings(MYSQL *mysql)
{
	MYSQLI_WARNING	*w, *first = NULL, *prev = NULL;
	MYSQL_RES		*result;
	MYSQL_ROW 		row;

	if (mysql_query(mysql, "SHOW WARNINGS")) {
		return NULL;
	}

	result = mysql_store_result(mysql);
	while ((row = mysql_fetch_row(result))) {
		w = php_new_warning(row[2], "HY000", atoi(row[1]));
		if (!first) {
			first = w;
		}
		if (prev) {
			prev->next = (void *)w;
		}
		prev = w;
	}
	mysql_free_result(result);
	return first;
}
/* }}} */

/* {{{ bool mysqli_warning::next() */
PHP_METHOD(mysqli_warning, next) 
{
	MYSQLI_WARNING 	*w;
	zval  			*mysqli_warning;
	mysqli_object *obj = (mysqli_object *)zend_objects_get_address(getThis() TSRMLS_CC);

	if (obj->ptr) {
		if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", 
										 &mysqli_warning, mysqli_warning_class_entry) == FAILURE) {
			return;
		}

		MYSQLI_FETCH_RESOURCE(w, MYSQLI_WARNING *, &mysqli_warning, "mysqli_warning", MYSQLI_STATUS_VALID);

		if (w->next) {
			w = w->next;
	        ((MYSQLI_RESOURCE *)(obj->ptr))->ptr = w;
			RETURN_TRUE;
		}
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ property mysqli_warning_message */
int mysqli_warning_message(mysqli_object *obj, zval **retval TSRMLS_DC)
{
	MYSQLI_WARNING *w;

	if (!obj->ptr || !((MYSQLI_RESOURCE *)(obj->ptr))->ptr) {
		return FAILURE;
	}

	w = (MYSQLI_WARNING *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr;
	ALLOC_ZVAL(*retval);
	if (w->reason) {
		ZVAL_STRING(*retval, w->reason, 1);
	} else {
		ZVAL_NULL(*retval);
	}
	return SUCCESS;
}
/* }}} */

/* {{{ property mysqli_warning_sqlstate */
int mysqli_warning_sqlstate(mysqli_object *obj, zval **retval TSRMLS_DC)
{
	MYSQLI_WARNING *w;
	
	if (!obj->ptr || !((MYSQLI_RESOURCE *)(obj->ptr))->ptr) {
		return FAILURE;
	}

	w = (MYSQLI_WARNING *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr;
	ALLOC_ZVAL(*retval);
	ZVAL_STRING(*retval, w->sqlstate, 1);
	return SUCCESS;
}
/* }}} */

/* {{{ property mysqli_warning_error */
int mysqli_warning_errno(mysqli_object *obj, zval **retval TSRMLS_DC)
{
	MYSQLI_WARNING *w;

	if (!obj->ptr || !((MYSQLI_RESOURCE *)(obj->ptr))->ptr) {
		return FAILURE;
	}
	w = (MYSQLI_WARNING *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr;
	ALLOC_ZVAL(*retval);
	ZVAL_LONG(*retval, w->errorno);
	return SUCCESS;
}
/* }}} */

/* {{{ mysqli_warning_construct(object obj) */
PHP_METHOD(mysqli_warning, __construct)
{
	zval			*z;
	mysqli_object	*obj;
	MYSQL			*hdl;
	MYSQLI_WARNING  *w;
	MYSQLI_RESOURCE *mysqli_resource;

	if (ZEND_NUM_ARGS() != 1) {
		WRONG_PARAM_COUNT;
	}
	if (zend_parse_parameters(1 TSRMLS_CC, "o", &z)==FAILURE) {
		return;
	}
	obj = (mysqli_object *)zend_object_store_get_object(z TSRMLS_CC);\

	if (obj->zo.ce == mysqli_link_class_entry) {
		MY_MYSQL *mysql;
		MYSQLI_FETCH_RESOURCE(mysql, MY_MYSQL *, &z, "mysqli_link", MYSQLI_STATUS_VALID);
		hdl = mysql->mysql;
	} else if (obj->zo.ce == mysqli_stmt_class_entry) {
		MY_STMT *stmt;
		MYSQLI_FETCH_RESOURCE(stmt, MY_STMT *, &z, "mysqli_stmt", MYSQLI_STATUS_VALID);
		hdl = stmt->stmt->mysql;
	} else {
		RETURN_FALSE;
	}

	if (mysql_warning_count(hdl)) {
		w = php_get_warnings(hdl); 
	} else {
		RETURN_FALSE;
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->status = MYSQLI_STATUS_VALID;
	mysqli_resource->ptr = mysqli_resource->info = (void *)w;

	if (!getThis() || !instanceof_function(Z_OBJCE_P(getThis()), mysqli_warning_class_entry TSRMLS_CC)) {
		MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_warning_class_entry);	
	} else {
		((mysqli_object *) zend_object_store_get_object(getThis() TSRMLS_CC))->ptr = mysqli_resource;
	}

}
/* }}} */

zend_function_entry mysqli_warning_methods[] = {
	PHP_ME(mysqli_warning, __construct,		NULL, ZEND_ACC_PROTECTED)
	PHP_ME(mysqli_warning, next, 			NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

mysqli_property_entry mysqli_warning_property_entries[] = {
	{"message", mysqli_warning_message, NULL},
	{"sqlstate", mysqli_warning_sqlstate, NULL},
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
