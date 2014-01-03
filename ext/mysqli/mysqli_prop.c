/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2014 The PHP Group                                |
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
  |         Andrey Hristov <andrey@php.net>                              |
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
#include "mysqli_priv.h"

#define CHECK_STATUS(value) \
	if (!obj->ptr || ((MYSQLI_RESOURCE *)obj->ptr)->status < value ) { \
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Property access is not allowed yet"); \
		ZVAL_NULL(*retval); \
		return SUCCESS; \
	} \

#define MYSQLI_GET_MYSQL(statusval) \
MYSQL *p; \
MAKE_STD_ZVAL(*retval);\
if (!obj->ptr || !(MY_MYSQL *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr) { \
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't fetch %s", obj->zo.ce->name);\
	ZVAL_NULL(*retval);\
	return SUCCESS; \
} else { \
	CHECK_STATUS(statusval);\
    p = (MYSQL *)((MY_MYSQL *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr)->mysql;\
}

#define MYSQLI_GET_RESULT(statusval) \
MYSQL_RES *p; \
MAKE_STD_ZVAL(*retval);\
if (!obj->ptr) { \
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't fetch %s", obj->zo.ce->name);\
	ZVAL_NULL(*retval);\
	return SUCCESS; \
} else { \
	CHECK_STATUS(statusval);\
	p = (MYSQL_RES *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr; \
}


#define MYSQLI_GET_STMT(statusval) \
MYSQL_STMT *p; \
MAKE_STD_ZVAL(*retval);\
if (!obj->ptr) { \
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't fetch %s", obj->zo.ce->name);\
	ZVAL_NULL(*retval);\
	return SUCCESS; \
} else { \
	CHECK_STATUS(statusval);\
	p = (MYSQL_STMT *)((MY_STMT *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr)->stmt;\
}

#define MYSQLI_MAP_PROPERTY_FUNC_LONG( __func, __int_func, __get_type, __ret_type, __ret_type_sprint_mod)\
static int __func(mysqli_object *obj, zval **retval TSRMLS_DC) \
{\
	__ret_type l;\
	__get_type;\
	if (!p) {\
		ZVAL_NULL(*retval);\
	} else {\
		l = (__ret_type)__int_func(p);\
		if (l < LONG_MAX) {\
			ZVAL_LONG(*retval, (long) l);\
		} else { \
			char *ret; \
			int ret_len = spprintf(&ret, 0, __ret_type_sprint_mod, l); \
			ZVAL_STRINGL(*retval, ret, ret_len, 0); \
		} \
	}\
	return SUCCESS;\
}

#define MYSQLI_MAP_PROPERTY_FUNC_STRING(__func, __int_func, __get_type)\
static int __func(mysqli_object *obj, zval **retval TSRMLS_DC)\
{\
	char *c;\
	__get_type;\
	if (!p) {\
		ZVAL_NULL(*retval);\
	} else {\
		c = (char *)__int_func(p);\
		if (!c) {\
			ZVAL_NULL(*retval);\
		} else {\
			ZVAL_STRING(*retval, c, 1);\
		}\
	}\
	return SUCCESS;\
}

/* {{{ property link_client_version_read */
static int link_client_version_read(mysqli_object *obj, zval **retval TSRMLS_DC)
{
	MAKE_STD_ZVAL(*retval);
	ZVAL_LONG(*retval, MYSQL_VERSION_ID);
	return SUCCESS;
}
/* }}} */

/* {{{ property link_client_info_read */
static int link_client_info_read(mysqli_object *obj, zval **retval TSRMLS_DC)
{
	MAKE_STD_ZVAL(*retval);
	CHECK_STATUS(MYSQLI_STATUS_INITIALIZED);
	ZVAL_STRING(*retval, MYSQL_SERVER_VERSION, 1);
	return SUCCESS;
}
/* }}} */

/* {{{ property link_connect_errno_read */
static int link_connect_errno_read(mysqli_object *obj, zval **retval TSRMLS_DC)
{
	MAKE_STD_ZVAL(*retval);
	ZVAL_LONG(*retval, (long)MyG(error_no));
	return SUCCESS;
}
/* }}} */

/* {{{ property link_connect_error_read */
static int link_connect_error_read(mysqli_object *obj, zval **retval TSRMLS_DC)
{
	MAKE_STD_ZVAL(*retval);
	if (MyG(error_msg)) {
		ZVAL_STRING(*retval, MyG(error_msg), 1);
	} else {
		ZVAL_NULL(*retval);
	}
	return SUCCESS;
}
/* }}} */

/* {{{ property link_affected_rows_read */
static int link_affected_rows_read(mysqli_object *obj, zval **retval TSRMLS_DC)
{
	MY_MYSQL *mysql;
	my_ulonglong rc;

	MAKE_STD_ZVAL(*retval);

	CHECK_STATUS(MYSQLI_STATUS_INITIALIZED);

 	mysql = (MY_MYSQL *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr;

	if (!mysql) {
		ZVAL_NULL(*retval);
	} else {
		CHECK_STATUS(MYSQLI_STATUS_VALID);

		rc = mysql_affected_rows(mysql->mysql);

		if (rc == (my_ulonglong) -1) {
			ZVAL_LONG(*retval, -1);
			return SUCCESS;
		}

		if (rc < LONG_MAX) {
			ZVAL_LONG(*retval, (long) rc);
		} else {
			char *ret;
			int l = spprintf(&ret, 0, MYSQLI_LLU_SPEC, rc);
			ZVAL_STRINGL(*retval, ret, l, 0);
		}
	}
	return SUCCESS;
}
/* }}} */


/* {{{ property link_error_list_read */
static int link_error_list_read(mysqli_object *obj, zval **retval TSRMLS_DC)
{
	MY_MYSQL *mysql;

	MAKE_STD_ZVAL(*retval);

	CHECK_STATUS(MYSQLI_STATUS_VALID);

 	mysql = (MY_MYSQL *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr;

	array_init(*retval);
	if (mysql) {
#if defined(MYSQLI_USE_MYSQLND)
		if (mysql->mysql->data->error_info->error_list) {
			MYSQLND_ERROR_LIST_ELEMENT * message;
			zend_llist_position pos;
			for (message = (MYSQLND_ERROR_LIST_ELEMENT *) zend_llist_get_first_ex(mysql->mysql->data->error_info->error_list, &pos);
				 message;
				 message = (MYSQLND_ERROR_LIST_ELEMENT *) zend_llist_get_next_ex(mysql->mysql->data->error_info->error_list, &pos)) 
			{
				zval * single_error;
				MAKE_STD_ZVAL(single_error);
				array_init(single_error);
				add_assoc_long_ex(single_error, "errno", sizeof("errno"), message->error_no);
				add_assoc_string_ex(single_error, "sqlstate", sizeof("sqlstate"), message->sqlstate, 1);
				add_assoc_string_ex(single_error, "error", sizeof("error"), message->error, 1);
				add_next_index_zval(*retval, single_error);
			}
		}
#else
		if (mysql_errno(mysql->mysql)) {
			zval * single_error;
			MAKE_STD_ZVAL(single_error);
			array_init(single_error);
			add_assoc_long_ex(single_error, "errno", sizeof("errno"), mysql_errno(mysql->mysql));
			add_assoc_string_ex(single_error, "sqlstate", sizeof("sqlstate"), mysql_sqlstate(mysql->mysql), 1);
			add_assoc_string_ex(single_error, "error", sizeof("error"), mysql_error(mysql->mysql), 1);
			add_next_index_zval(*retval, single_error);
		}
#endif
	}
	return SUCCESS;
}
/* }}} */


/* link properties */
MYSQLI_MAP_PROPERTY_FUNC_LONG(link_errno_read, mysql_errno, MYSQLI_GET_MYSQL(MYSQLI_STATUS_INITIALIZED), ulong, "%lu")
MYSQLI_MAP_PROPERTY_FUNC_STRING(link_error_read, mysql_error, MYSQLI_GET_MYSQL(MYSQLI_STATUS_INITIALIZED))
MYSQLI_MAP_PROPERTY_FUNC_LONG(link_field_count_read, mysql_field_count, MYSQLI_GET_MYSQL(MYSQLI_STATUS_VALID), ulong, "%lu")
MYSQLI_MAP_PROPERTY_FUNC_STRING(link_host_info_read, mysql_get_host_info, MYSQLI_GET_MYSQL(MYSQLI_STATUS_VALID))
MYSQLI_MAP_PROPERTY_FUNC_STRING(link_info_read, mysql_info, MYSQLI_GET_MYSQL(MYSQLI_STATUS_VALID))
MYSQLI_MAP_PROPERTY_FUNC_LONG(link_insert_id_read, mysql_insert_id, MYSQLI_GET_MYSQL(MYSQLI_STATUS_VALID), my_ulonglong, MYSQLI_LLU_SPEC)
MYSQLI_MAP_PROPERTY_FUNC_LONG(link_protocol_version_read, mysql_get_proto_info, MYSQLI_GET_MYSQL(MYSQLI_STATUS_VALID), ulong, "%lu")
MYSQLI_MAP_PROPERTY_FUNC_STRING(link_server_info_read, mysql_get_server_info, MYSQLI_GET_MYSQL(MYSQLI_STATUS_VALID))
MYSQLI_MAP_PROPERTY_FUNC_LONG(link_server_version_read, mysql_get_server_version, MYSQLI_GET_MYSQL(MYSQLI_STATUS_VALID), ulong, "%lu")
MYSQLI_MAP_PROPERTY_FUNC_STRING(link_sqlstate_read, mysql_sqlstate, MYSQLI_GET_MYSQL(MYSQLI_STATUS_VALID))
MYSQLI_MAP_PROPERTY_FUNC_LONG(link_thread_id_read, mysql_thread_id, MYSQLI_GET_MYSQL(MYSQLI_STATUS_VALID), ulong, "%lu")
MYSQLI_MAP_PROPERTY_FUNC_LONG(link_warning_count_read, mysql_warning_count, MYSQLI_GET_MYSQL(MYSQLI_STATUS_VALID), ulong, "%lu")

/* {{{ property link_stat_read */
static int link_stat_read(mysqli_object *obj, zval **retval TSRMLS_DC)
{
	MY_MYSQL *mysql;

	MAKE_STD_ZVAL(*retval);
	ZVAL_NULL(*retval);

#if defined(MYSQLI_USE_MYSQLND)
	CHECK_STATUS(MYSQLI_STATUS_INITIALIZED);
#else
	CHECK_STATUS(MYSQLI_STATUS_VALID);
#endif

 	mysql = (MY_MYSQL *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr;

	if (mysql) {
		char * stat_msg;
#if defined(MYSQLI_USE_MYSQLND)
		uint stat_msg_len;
		if (mysqlnd_stat(mysql->mysql, &stat_msg, &stat_msg_len) == PASS) {
			ZVAL_STRINGL(*retval, stat_msg, stat_msg_len, 0);
		}
#else
		if ((stat_msg = (char *) mysql_stat(mysql->mysql))) {
			ZVAL_STRING(*retval, stat_msg, 1);
		}
#endif
	}
	return SUCCESS;
}
/* }}} */


/* result properties */

/* {{{ property result_type_read */
static int result_type_read(mysqli_object *obj, zval **retval TSRMLS_DC)
{
	MYSQL_RES *p;

	MAKE_STD_ZVAL(*retval);
	CHECK_STATUS(MYSQLI_STATUS_VALID);
 	p = (MYSQL_RES *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr;

	if (!p) {
		ZVAL_NULL(*retval);
	} else {
		ZVAL_LONG(*retval, mysqli_result_is_unbuffered(p) ? MYSQLI_USE_RESULT:MYSQLI_STORE_RESULT);
	}
	return SUCCESS;
}
/* }}} */

/* {{{ property result_lengths_read */
static int result_lengths_read(mysqli_object *obj, zval **retval TSRMLS_DC)
{
	MYSQL_RES *p;
	ulong *ret;

	MAKE_STD_ZVAL(*retval);

	CHECK_STATUS(MYSQLI_STATUS_VALID);
	p = (MYSQL_RES *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr;
	if (!p || !p->field_count || !(ret = mysql_fetch_lengths(p))) {
		ZVAL_NULL(*retval);
	} else {
		ulong i;

		array_init(*retval);

		for (i = 0; i < p->field_count; i++) {
			add_index_long(*retval, i, ret[i]);
		}
	}
	return SUCCESS;
}
/* }}} */


MYSQLI_MAP_PROPERTY_FUNC_LONG(result_current_field_read, mysql_field_tell, MYSQLI_GET_RESULT(MYSQLI_STATUS_VALID), ulong, "%lu")
MYSQLI_MAP_PROPERTY_FUNC_LONG(result_field_count_read, mysql_num_fields, MYSQLI_GET_RESULT(MYSQLI_STATUS_VALID), ulong, "%lu")
MYSQLI_MAP_PROPERTY_FUNC_LONG(result_num_rows_read, mysql_num_rows, MYSQLI_GET_RESULT(MYSQLI_STATUS_VALID), my_ulonglong, MYSQLI_LLU_SPEC)

/* statement properties */

/* {{{ property stmt_id_read */
static int stmt_id_read(mysqli_object *obj, zval **retval TSRMLS_DC)
{
	MY_STMT *p;

	MAKE_STD_ZVAL(*retval);
	CHECK_STATUS(MYSQLI_STATUS_VALID);

 	p = (MY_STMT*)((MYSQLI_RESOURCE *)(obj->ptr))->ptr;

	if (!p) {
		ZVAL_NULL(*retval);
	} else {
		ZVAL_LONG(*retval, mysqli_stmt_get_id(p->stmt));
	}
	return SUCCESS;
}
/* }}} */

/* {{{ property stmt_affected_rows_read */
static int stmt_affected_rows_read(mysqli_object *obj, zval **retval TSRMLS_DC)
{
	MY_STMT *p;
	my_ulonglong rc;

	MAKE_STD_ZVAL(*retval);
	CHECK_STATUS(MYSQLI_STATUS_VALID);

 	p = (MY_STMT *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr;

	if (!p) {
		ZVAL_NULL(*retval);
	} else {
		rc = mysql_stmt_affected_rows(p->stmt);

		if (rc == (my_ulonglong) -1) {
			ZVAL_LONG(*retval, -1);
			return SUCCESS;
		}

		if (rc < LONG_MAX) {
			ZVAL_LONG(*retval, (long) rc);
		} else {
			char *ret;
			int l = spprintf(&ret, 0, MYSQLI_LLU_SPEC, rc);
			ZVAL_STRINGL(*retval, ret, l, 0);
		}
	}
	return SUCCESS;
}
/* }}} */

/* {{{ property stmt_error_list_read */
static int stmt_error_list_read(mysqli_object *obj, zval **retval TSRMLS_DC)
{
	MY_STMT * stmt;

	MAKE_STD_ZVAL(*retval);
	CHECK_STATUS(MYSQLI_STATUS_INITIALIZED);

 	stmt = (MY_STMT *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr;
	array_init(*retval);
	if (stmt && stmt->stmt) {
#if defined(MYSQLI_USE_MYSQLND)
		if (stmt->stmt->data && stmt->stmt->data->error_info->error_list) {
			MYSQLND_ERROR_LIST_ELEMENT * message;
			zend_llist_position pos;
			for (message = (MYSQLND_ERROR_LIST_ELEMENT *) zend_llist_get_first_ex(stmt->stmt->data->error_info->error_list, &pos);
				 message;
				 message = (MYSQLND_ERROR_LIST_ELEMENT *) zend_llist_get_next_ex(stmt->stmt->data->error_info->error_list, &pos)) 
			{
				zval * single_error;
				MAKE_STD_ZVAL(single_error);
				array_init(single_error);
				add_assoc_long_ex(single_error, "errno", sizeof("errno"), message->error_no);
				add_assoc_string_ex(single_error, "sqlstate", sizeof("sqlstate"), message->sqlstate, 1);
				add_assoc_string_ex(single_error, "error", sizeof("error"), message->error, 1);
				add_next_index_zval(*retval, single_error);
			}
		}
#else
		if (mysql_stmt_errno(stmt->stmt)) {
			zval * single_error;
			MAKE_STD_ZVAL(single_error);
			array_init(single_error);
			add_assoc_long_ex(single_error, "errno", sizeof("errno"), mysql_stmt_errno(stmt->stmt));
			add_assoc_string_ex(single_error, "sqlstate", sizeof("sqlstate"), mysql_stmt_sqlstate(stmt->stmt), 1);
			add_assoc_string_ex(single_error, "error", sizeof("error"), mysql_stmt_error(stmt->stmt), 1);
			add_next_index_zval(*retval, single_error);
		}
#endif
	}
	return SUCCESS;
}
/* }}} */


MYSQLI_MAP_PROPERTY_FUNC_LONG(stmt_insert_id_read, mysql_stmt_insert_id, MYSQLI_GET_STMT(MYSQLI_STATUS_VALID), my_ulonglong, MYSQLI_LLU_SPEC)
MYSQLI_MAP_PROPERTY_FUNC_LONG(stmt_num_rows_read, mysql_stmt_num_rows, MYSQLI_GET_STMT(MYSQLI_STATUS_VALID), my_ulonglong, MYSQLI_LLU_SPEC)
MYSQLI_MAP_PROPERTY_FUNC_LONG(stmt_param_count_read, mysql_stmt_param_count, MYSQLI_GET_STMT(MYSQLI_STATUS_VALID), ulong, "%lu")
MYSQLI_MAP_PROPERTY_FUNC_LONG(stmt_field_count_read, mysql_stmt_field_count, MYSQLI_GET_STMT(MYSQLI_STATUS_VALID), ulong, "%lu")
MYSQLI_MAP_PROPERTY_FUNC_LONG(stmt_errno_read, mysql_stmt_errno, MYSQLI_GET_STMT(MYSQLI_STATUS_INITIALIZED), ulong, "%lu")
MYSQLI_MAP_PROPERTY_FUNC_STRING(stmt_error_read, mysql_stmt_error, MYSQLI_GET_STMT(MYSQLI_STATUS_INITIALIZED))
MYSQLI_MAP_PROPERTY_FUNC_STRING(stmt_sqlstate_read, mysql_stmt_sqlstate, MYSQLI_GET_STMT(MYSQLI_STATUS_INITIALIZED))

/* }}} */
const mysqli_property_entry mysqli_link_property_entries[] = {
	{"affected_rows", 	sizeof("affected_rows") - 1,	link_affected_rows_read, NULL},
	{"client_info", 	sizeof("client_info") - 1,		link_client_info_read, NULL},
	{"client_version",	sizeof("client_version") - 1,	link_client_version_read, NULL},
	{"connect_errno",	sizeof("connect_errno") - 1,	link_connect_errno_read, NULL},
	{"connect_error",	sizeof("connect_error") - 1,	link_connect_error_read, NULL},
	{"errno",			sizeof("errno") - 1,			link_errno_read, NULL},
	{"error",			sizeof("error") - 1,			link_error_read, NULL},
	{"error_list",		sizeof("error_list") - 1,		link_error_list_read, NULL},
	{"field_count",		sizeof("field_count") - 1,		link_field_count_read, NULL},
	{"host_info",		sizeof("host_info") - 1,		link_host_info_read, NULL},
	{"info",			sizeof("info") - 1,				link_info_read, NULL},
	{"insert_id",		sizeof("insert_id") - 1,		link_insert_id_read, NULL},
	{"server_info",		sizeof("server_info") - 1,		link_server_info_read, NULL},
	{"server_version",	sizeof("server_version") - 1,	link_server_version_read, NULL},
	{"stat",			sizeof("stat") - 1,				link_stat_read, NULL},
	{"sqlstate",		sizeof("sqlstate") - 1,			link_sqlstate_read, NULL},
	{"protocol_version",sizeof("protocol_version") - 1,	link_protocol_version_read, NULL},
	{"thread_id",		sizeof("thread_id") - 1, 		link_thread_id_read, NULL},
	{"warning_count",	sizeof("warning_count") - 1, 	link_warning_count_read, NULL},
	{NULL, 0, NULL, NULL}
};


const zend_property_info mysqli_link_property_info_entries[] = {
	{ZEND_ACC_PUBLIC, "affected_rows",	sizeof("affected_rows") - 1,	-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "client_info",	sizeof("client_info") - 1,		-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "client_version",	sizeof("client_version") - 1,	-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "connect_errno",	sizeof("connect_errno") - 1,	-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "connect_error",	sizeof("connect_error") - 1,	-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "errno",			sizeof("errno") - 1,			-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "error",			sizeof("error") - 1,			-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "error_list",		sizeof("error_list") - 1,		-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "field_count",	sizeof("field_count") - 1,		-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "host_info",		sizeof("host_info") - 1,		-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "info",			sizeof("info") - 1,				-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "insert_id",		sizeof("insert_id") - 1,		-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "server_info",	sizeof("server_info") - 1,		-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "server_version",	sizeof("server_version") - 1,	-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "stat",			sizeof("stat") - 1,				-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "sqlstate",		sizeof("sqlstate") - 1,			-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "protocol_version", sizeof("protocol_version")-1, -1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "thread_id", 		sizeof("thread_id") - 1,		-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "warning_count",	sizeof("warning_count") - 1,	-1, 0, NULL, 0, NULL},
	{0,					NULL, 			0,								-1, 0, NULL, 0, NULL}
};


const mysqli_property_entry mysqli_result_property_entries[] = {
	{"current_field",sizeof("current_field")-1,	result_current_field_read, NULL},
	{"field_count", sizeof("field_count") - 1,	result_field_count_read, NULL},
	{"lengths", 	sizeof("lengths") - 1,		result_lengths_read, NULL},
	{"num_rows", 	sizeof("num_rows") - 1,		result_num_rows_read, NULL},
	{"type", 		sizeof("type") - 1,			result_type_read, NULL},
	{NULL, 0, NULL, NULL}
};

const zend_property_info mysqli_result_property_info_entries[] = {
	{ZEND_ACC_PUBLIC, "current_field",	sizeof("current_field")-1,	-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "field_count",	sizeof("field_count") - 1, 	-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "lengths",		sizeof("lengths") - 1, 		-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "num_rows",		sizeof("num_rows") - 1, 	-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "type",			sizeof("type") - 1, 		-1, 0, NULL, 0, NULL},
	{0,					NULL, 			0,							-1, 0, NULL, 0, NULL}
};

const mysqli_property_entry mysqli_stmt_property_entries[] = {
	{"affected_rows", sizeof("affected_rows")-1,stmt_affected_rows_read, NULL},
	{"insert_id",	sizeof("insert_id") - 1, 	stmt_insert_id_read, NULL},
	{"num_rows",	sizeof("num_rows") - 1, 	stmt_num_rows_read, NULL},
	{"param_count", sizeof("param_count") - 1,	stmt_param_count_read, NULL},
	{"field_count", sizeof("field_count") - 1,	stmt_field_count_read, NULL},
	{"errno",		sizeof("errno") - 1,		stmt_errno_read, NULL},
	{"error",		sizeof("error") - 1, 		stmt_error_read, NULL},
	{"error_list",	sizeof("error_list") - 1, 	stmt_error_list_read, NULL},
	{"sqlstate",	sizeof("sqlstate") - 1,		stmt_sqlstate_read, NULL},
	{"id",			sizeof("id") - 1,			stmt_id_read, NULL},
	{NULL, 0, NULL, NULL}
};


const zend_property_info mysqli_stmt_property_info_entries[] = {
	{ZEND_ACC_PUBLIC, "affected_rows", sizeof("affected_rows") - 1,	-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "insert_id",	sizeof("insert_id") - 1,		-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "num_rows",	sizeof("num_rows") - 1,			-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "param_count",sizeof("param_count") - 1,		-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "field_count",sizeof("field_count") - 1,		-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "errno",		sizeof("errno") - 1,			-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "error",		sizeof("error") - 1,			-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "error_list",	sizeof("error_list") - 1,		-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "sqlstate",	sizeof("sqlstate") - 1,			-1, 0, NULL, 0, NULL},
	{ZEND_ACC_PUBLIC, "id",			sizeof("id") - 1,				-1, 0, NULL, 0, NULL},
	{0,					NULL, 			0,							-1, 0, NULL, 0, NULL}
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
