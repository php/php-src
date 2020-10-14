/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
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

/* Define these in the PHP7 tree to make merging easy process */
#define ZSTR_DUPLICATE (1<<0)
#define ZSTR_AUTOFREE  (1<<1)

#define ZVAL_UTF8_STRING(z, s, flags)          ZVAL_STRING((z), (char*)(s))
#define ZVAL_UTF8_STRINGL(z, s, l, flags)      ZVAL_STRINGL((z), (char*)(s), (l))

/* {{{ void php_clear_warnings() */
void php_clear_warnings(MYSQLI_WARNING *w)
{
	MYSQLI_WARNING *n;

	while (w) {
		n = w;
		zval_ptr_dtor_str(&(w->reason));
		zval_ptr_dtor_str(&(w->sqlstate));
		w = w->next;
		efree(n);
	}
}
/* }}} */

#ifndef MYSQLI_USE_MYSQLND
/* {{{ MYSQLI_WARNING *php_new_warning */
static
MYSQLI_WARNING *php_new_warning(const char *reason, int errorno)
{
	MYSQLI_WARNING *w;

	w = (MYSQLI_WARNING *)ecalloc(1, sizeof(MYSQLI_WARNING));

	ZVAL_UTF8_STRING(&(w->reason), reason, ZSTR_DUPLICATE);

	ZVAL_UTF8_STRINGL(&(w->sqlstate), "HY000", sizeof("HY000") - 1,  ZSTR_DUPLICATE);

	w->errorno = errorno;

	return w;
}
/* }}} */

/* {{{ MYSQLI_WARNING *php_get_warnings(MYSQL *mysql) */
MYSQLI_WARNING *php_get_warnings(MYSQL *mysql)
{
	MYSQLI_WARNING *w, *first = NULL, *prev = NULL;
	MYSQL_RES		*result;
	MYSQL_ROW 		row;

	if (mysql_real_query(mysql, "SHOW WARNINGS", 13)) {
		return NULL;
	}

	result = mysql_store_result(mysql);

	while ((row = mysql_fetch_row(result))) {
		w = php_new_warning(row[2], atoi(row[1]));
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
MYSQLI_WARNING *php_new_warning(zval * reason, int errorno)
{
	MYSQLI_WARNING *w;

	w = (MYSQLI_WARNING *)ecalloc(1, sizeof(MYSQLI_WARNING));

	ZVAL_COPY(&w->reason, reason);
	convert_to_string(&w->reason);

	ZVAL_UTF8_STRINGL(&(w->sqlstate), "HY000", sizeof("HY000") - 1,  ZSTR_DUPLICATE);

	w->errorno = errorno;

	return w;
}
/* }}} */

/* {{{ MYSQLI_WARNING *php_get_warnings(MYSQL *mysql) */
MYSQLI_WARNING * php_get_warnings(MYSQLND_CONN_DATA * mysql)
{
	MYSQLI_WARNING	*w, *first = NULL, *prev = NULL;
	MYSQL_RES		*result;
	zval			row;

	if (mysql->m->query(mysql, "SHOW WARNINGS", 13)) {
		return NULL;
	}

	result = mysql->m->use_result(mysql, 0);

	for (;;) {
		zval *entry;
		int errno;

		mysqlnd_fetch_into(result, MYSQLND_FETCH_NUM, &row, MYSQLND_MYSQLI);
		if (Z_TYPE(row) != IS_ARRAY) {
			zval_ptr_dtor(&row);
			break;
		}
		zend_hash_internal_pointer_reset(Z_ARRVAL(row));
		/* 0. we don't care about the first */
		zend_hash_move_forward(Z_ARRVAL(row));

		/* 1. Here comes the error no */
		entry = zend_hash_get_current_data(Z_ARRVAL(row));
		errno = zval_get_long(entry);
		zend_hash_move_forward(Z_ARRVAL(row));

		/* 2. Here comes the reason */
		entry = zend_hash_get_current_data(Z_ARRVAL(row));

		w = php_new_warning(entry, errno);
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
	mysqli_object *obj = Z_MYSQLI_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (obj->ptr) {
		MYSQLI_FETCH_RESOURCE(w, MYSQLI_WARNING *, ZEND_THIS, "mysqli_warning", MYSQLI_STATUS_VALID);

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
static int mysqli_warning_message(mysqli_object *obj, zval *retval, zend_bool quiet)
{
	MYSQLI_WARNING *w;

	if (!obj->ptr || !((MYSQLI_RESOURCE *)(obj->ptr))->ptr) {
		if (!quiet) {
			zend_throw_error(NULL, "%s object is already closed", ZSTR_VAL(obj->zo.ce->name));
		}

		return FAILURE;
	}

	w = (MYSQLI_WARNING *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr;
	ZVAL_COPY(retval, &w->reason);

	return SUCCESS;
}
/* }}} */

/* {{{ property mysqli_warning_sqlstate */
static int mysqli_warning_sqlstate(mysqli_object *obj, zval *retval, zend_bool quiet)
{
	MYSQLI_WARNING *w;

	if (!obj->ptr || !((MYSQLI_RESOURCE *)(obj->ptr))->ptr) {
		if (!quiet) {
			zend_throw_error(NULL, "%s object is already closed", ZSTR_VAL(obj->zo.ce->name));
		}

		return FAILURE;
	}

	w = (MYSQLI_WARNING *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr;
	ZVAL_COPY(retval, &w->sqlstate);

	return SUCCESS;
}
/* }}} */

/* {{{ property mysqli_warning_error */
static int mysqli_warning_errno(mysqli_object *obj, zval *retval, zend_bool quiet)
{
	MYSQLI_WARNING *w;

	if (!obj->ptr || !((MYSQLI_RESOURCE *)(obj->ptr))->ptr) {
		if (!quiet) {
			zend_throw_error(NULL, "%s object is already closed", ZSTR_VAL(obj->zo.ce->name));
		}

		return FAILURE;
	}

	w = (MYSQLI_WARNING *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr;
	ZVAL_LONG(retval, w->errorno);

	return SUCCESS;
}
/* }}} */

PHP_METHOD(mysqli_warning, __construct)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_throw_error(NULL, "Cannot directly construct mysqli_warning");
}

/* {{{ mysqli_warning_property_entries */
const mysqli_property_entry mysqli_warning_property_entries[] = {
	{"message", sizeof("message") - 1, mysqli_warning_message, NULL},
	{"sqlstate", sizeof("sqlstate") - 1, mysqli_warning_sqlstate, NULL},
	{"errno", sizeof("errno") - 1, mysqli_warning_errno, NULL},
	{NULL, 0, NULL, NULL}
};
/* }}} */
