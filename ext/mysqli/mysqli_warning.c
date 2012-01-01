/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
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
#include "php_mysqli_structs.h"
#include "mysqli_priv.h"

/* Define these in the PHP5 tree to make merging easy process */
#define ZSTR_DUPLICATE (1<<0)
#define ZSTR_AUTOFREE  (1<<1)

#define ZVAL_UTF8_STRING(z, s, flags)          ZVAL_STRING((z), (char*)(s), ((flags) & ZSTR_DUPLICATE))
#define ZVAL_UTF8_STRINGL(z, s, l, flags)      ZVAL_STRINGL((z), (char*)(s), (l), ((flags) & ZSTR_DUPLICATE))


/* {{{ void php_clear_warnings() */
void php_clear_warnings(MYSQLI_WARNING *w)
{
	MYSQLI_WARNING *n;

	while (w) {
		n = w;
		zval_dtor(&(w->reason));
		zval_dtor(&(w->sqlstate));
		w = w->next;
		efree(n);
	}
}
/* }}} */


#ifndef MYSQLI_USE_MYSQLND
/* {{{ MYSQLI_WARNING *php_new_warning */
static
MYSQLI_WARNING *php_new_warning(const char *reason, int errorno TSRMLS_DC)
{
	MYSQLI_WARNING *w;

	w = (MYSQLI_WARNING *)ecalloc(1, sizeof(MYSQLI_WARNING));

	ZVAL_UTF8_STRING(&(w->reason), reason, ZSTR_DUPLICATE);

	ZVAL_UTF8_STRINGL(&(w->sqlstate), "HY000", sizeof("HY000") - 1,  ZSTR_DUPLICATE);

	w->errorno = errorno;

	return w;
}
/* }}} */


/* {{{ MYSQLI_WARNING *php_get_warnings(MYSQL *mysql TSRMLS_DC) */
MYSQLI_WARNING *php_get_warnings(MYSQL *mysql TSRMLS_DC)
{
	MYSQLI_WARNING *w, *first = NULL, *prev = NULL;
	MYSQL_RES		*result;
	MYSQL_ROW 		row;

	if (mysql_real_query(mysql, "SHOW WARNINGS", 13)) {
		return NULL;
	}

	result = mysql_store_result(mysql);

	while ((row = mysql_fetch_row(result))) {
		w = php_new_warning(row[2], atoi(row[1]) TSRMLS_CC);
		if (!first) {
			first = w;
		}
		if (prev) {
			prev->next = w;
		}
		prev = w;
	}
	mysql_free_result(result);
	return first;
}
/* }}} */
#else
/* {{{ MYSQLI_WARNING *php_new_warning */
static
MYSQLI_WARNING *php_new_warning(const zval * reason, int errorno TSRMLS_DC)
{
	MYSQLI_WARNING *w;

	w = (MYSQLI_WARNING *)ecalloc(1, sizeof(MYSQLI_WARNING));

	w->reason = *reason;
	zval_copy_ctor(&(w->reason));

	ZVAL_UTF8_STRINGL(&(w->reason),  Z_STRVAL(w->reason), Z_STRLEN(w->reason),  ZSTR_AUTOFREE);

	ZVAL_UTF8_STRINGL(&(w->sqlstate), "HY000", sizeof("HY000") - 1,  ZSTR_DUPLICATE);

	w->errorno = errorno;

	return w;
}
/* }}} */


/* {{{ MYSQLI_WARNING *php_get_warnings(MYSQL *mysql TSRMLS_DC) */
MYSQLI_WARNING * php_get_warnings(MYSQLND_CONN_DATA * mysql TSRMLS_DC)
{
	MYSQLI_WARNING	*w, *first = NULL, *prev = NULL;
	MYSQL_RES		*result;
	zval			*row;

	if (mysql->m->query(mysql, "SHOW WARNINGS", 13 TSRMLS_CC)) {
		return NULL;
	}

	result = mysql->m->use_result(mysql TSRMLS_CC);

	for (;;) {
		zval **entry;
		int errno;

		MAKE_STD_ZVAL(row);
		mysqlnd_fetch_into(result, MYSQLND_FETCH_NUM, row, MYSQLND_MYSQLI);
		if (Z_TYPE_P(row) != IS_ARRAY) {
			zval_ptr_dtor(&row);
			break;
		}
		zend_hash_internal_pointer_reset(Z_ARRVAL_P(row));
		/* 0. we don't care about the first */
		zend_hash_move_forward(Z_ARRVAL_P(row));

		/* 1. Here comes the error no */
		zend_hash_get_current_data(Z_ARRVAL_P(row), (void **)&entry);
		convert_to_long_ex(entry);
		errno = Z_LVAL_PP(entry);
		zend_hash_move_forward(Z_ARRVAL_P(row));

		/* 2. Here comes the reason */
		zend_hash_get_current_data(Z_ARRVAL_P(row), (void **)&entry);

		w = php_new_warning(*entry, errno TSRMLS_CC);
		/*
		  Don't destroy entry, because the row destroy will decrease
		  the refcounter. Decreased twice then mysqlnd_free_result()
		  will crash, because it will try to access already freed memory.
		*/
		if (!first) {
			first = w;
		}
		if (prev) {
			prev->next = (void *)w;
		}
		prev = w;

		zval_ptr_dtor(&row);
	}

	mysql_free_result(result);
	return first;
}
/* }}} */
#endif


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

		if (w && w->next) {
			w = w->next;
	        ((MYSQLI_RESOURCE *)(obj->ptr))->ptr = w;
			RETURN_TRUE;
		}
	}
	RETURN_FALSE;
}
/* }}} */


/* {{{ property mysqli_warning_message */
static
int mysqli_warning_message(mysqli_object *obj, zval **retval TSRMLS_DC)
{
	MYSQLI_WARNING *w;

	if (!obj->ptr || !((MYSQLI_RESOURCE *)(obj->ptr))->ptr) {
		return FAILURE;
	}

	w = (MYSQLI_WARNING *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr;
	MAKE_STD_ZVAL(*retval);
	**retval = w->reason;
	zval_copy_ctor(*retval);
	return SUCCESS;
}
/* }}} */


/* {{{ property mysqli_warning_sqlstate */
static
int mysqli_warning_sqlstate(mysqli_object *obj, zval **retval TSRMLS_DC)
{
	MYSQLI_WARNING *w;

	if (!obj->ptr || !((MYSQLI_RESOURCE *)(obj->ptr))->ptr) {
		return FAILURE;
	}

	w = (MYSQLI_WARNING *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr;
	MAKE_STD_ZVAL(*retval);
	**retval = w->sqlstate;
	zval_copy_ctor(*retval);
	return SUCCESS;
}
/* }}} */


/* {{{ property mysqli_warning_error */
static
int mysqli_warning_errno(mysqli_object *obj, zval **retval TSRMLS_DC)
{
	MYSQLI_WARNING *w;

	if (!obj->ptr || !((MYSQLI_RESOURCE *)(obj->ptr))->ptr) {
		return FAILURE;
	}
	w = (MYSQLI_WARNING *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr;
	MAKE_STD_ZVAL(*retval);
	ZVAL_LONG(*retval, w->errorno);
	return SUCCESS;
}
/* }}} */

/* {{{ mysqli_warning_construct(object obj) */
PHP_METHOD(mysqli_warning, __construct)
{
	zval			*z;
	mysqli_object	*obj;
#ifndef MYSQLI_USE_MYSQLND
	MYSQL			*hdl;
#endif
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
		MYSQLI_FETCH_RESOURCE_CONN(mysql, &z, MYSQLI_STATUS_VALID);
		if (mysql_warning_count(mysql->mysql)) {
#ifndef MYSQLI_USE_MYSQLND
			w = php_get_warnings(mysql->mysql TSRMLS_CC);
#else
			w = php_get_warnings(mysql->mysql->data TSRMLS_CC);
#endif
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "No warnings found");
			RETURN_FALSE;
		}
	} else if (obj->zo.ce == mysqli_stmt_class_entry) {
		MY_STMT *stmt;
		MYSQLI_FETCH_RESOURCE_STMT(stmt, &z, MYSQLI_STATUS_VALID);
#ifndef MYSQLI_USE_MYSQLND
		hdl = mysqli_stmt_get_connection(stmt->stmt);
		if (mysql_warning_count(hdl)) {
			w = php_get_warnings(hdl TSRMLS_CC);
#else
		if (mysqlnd_stmt_warning_count(stmt->stmt)) {
			w = php_get_warnings(mysqli_stmt_get_connection(stmt->stmt) TSRMLS_CC);
#endif
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "No warnings found");
			RETURN_FALSE;
		}
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid class argument");
		RETURN_FALSE;
	}

	mysqli_resource = (MYSQLI_RESOURCE *)ecalloc (1, sizeof(MYSQLI_RESOURCE));
	mysqli_resource->ptr = mysqli_resource->info = (void *)w;
	mysqli_resource->status = MYSQLI_STATUS_VALID;

	if (!getThis() || !instanceof_function(Z_OBJCE_P(getThis()), mysqli_warning_class_entry TSRMLS_CC)) {
		MYSQLI_RETURN_RESOURCE(mysqli_resource, mysqli_warning_class_entry);
	} else {
		((mysqli_object *) zend_object_store_get_object(getThis() TSRMLS_CC))->ptr = mysqli_resource;
	}

}
/* }}} */

/* {{{ mysqli_warning_methods */
const zend_function_entry mysqli_warning_methods[] = {
	PHP_ME(mysqli_warning, __construct,		NULL, ZEND_ACC_PROTECTED)
	PHP_ME(mysqli_warning, next, 			NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ mysqli_warning_property_entries */
const mysqli_property_entry mysqli_warning_property_entries[] = {
	{"message", sizeof("message") - 1, mysqli_warning_message, NULL},
	{"sqlstate", sizeof("sqlstate") - 1, mysqli_warning_sqlstate, NULL},
	{"errno", sizeof("errno") - 1, mysqli_warning_errno, NULL},
	{NULL, 0, NULL, NULL}
};
/* }}} */

/* {{{ mysqli_warning_property_info_entries */
const zend_property_info mysqli_warning_property_info_entries[] = {
	{ZEND_ACC_PUBLIC, "message", 	sizeof("message") - 1,	-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "sqlstate",	sizeof("sqlstate") - 1,	-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "errno",		sizeof("errno") - 1, 	-1, 0, NULL, 0, NULL},
	{0,					NULL, 			0,					-1, 0, NULL, 0, NULL}
};
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
