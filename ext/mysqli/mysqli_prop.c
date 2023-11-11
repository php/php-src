/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://www.php.net/license/3_01.txt                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Georg Richter <georg@php.net>                                |
  |         Andrey Hristov <andrey@php.net>                              |
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

#define CHECK_STATUS(value, quiet) \
	if (!obj->ptr || ((MYSQLI_RESOURCE *)obj->ptr)->status < value ) { \
		if (!quiet) { \
			zend_throw_error(NULL, "Property access is not allowed yet"); \
		} \
		return FAILURE; \
	} \

#define MYSQLI_GET_MYSQL(statusval) \
MYSQL *p; \
if (!obj->ptr || !(MY_MYSQL *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr) { \
	if (!quiet) { \
		zend_throw_error(NULL, "%s object is already closed", ZSTR_VAL(obj->zo.ce->name)); \
	} \
	return FAILURE; \
} else { \
	CHECK_STATUS(statusval, quiet);\
	p = (MYSQL *)((MY_MYSQL *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr)->mysql;\
}

#define MYSQLI_GET_RESULT(statusval) \
MYSQL_RES *p; \
if (!obj->ptr) { \
	if (!quiet) { \
		zend_throw_error(NULL, "%s object is already closed", ZSTR_VAL(obj->zo.ce->name)); \
	} \
	return FAILURE; \
} else { \
	CHECK_STATUS(statusval, quiet);\
	p = (MYSQL_RES *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr; \
}

#define MYSQLI_GET_STMT(statusval) \
MYSQL_STMT *p; \
if (!obj->ptr) { \
	if (!quiet) { \
		zend_throw_error(NULL, "%s object is already closed", ZSTR_VAL(obj->zo.ce->name)); \
	} \
	return FAILURE; \
} else { \
	CHECK_STATUS(statusval, quiet); \
	p = (MYSQL_STMT *)((MY_STMT *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr)->stmt; \
}

#define MYSQLI_MAP_PROPERTY_FUNC_LONG_OR_STR( __func, __int_func, __get_type, __ret_type, __ret_type_sprint_mod)\
static int __func(mysqli_object *obj, zval *retval, bool quiet) \
{\
	__ret_type l;\
	__get_type;\
	ZEND_ASSERT(p);\
	l = (__ret_type)__int_func(p);\
	if (l < ZEND_LONG_MAX) {\
		ZVAL_LONG(retval, (zend_long) l);\
	} else { \
		ZVAL_NEW_STR(retval, strpprintf(0, __ret_type_sprint_mod, l)); \
	} \
	return SUCCESS; \
}

#define MYSQLI_MAP_PROPERTY_FUNC_LONG( __func, __int_func, __get_type, __ret_type, __ret_type_sprint_mod)\
static int __func(mysqli_object *obj, zval *retval, bool quiet) \
{\
	__ret_type l;\
	__get_type;\
	ZEND_ASSERT(p);\
	l = (__ret_type)__int_func(p);\
	ZEND_ASSERT(l < ZEND_LONG_MAX);\
	ZVAL_LONG(retval, (zend_long) l);\
	return SUCCESS; \
}

#define MYSQLI_MAP_PROPERTY_FUNC_STR_OR_NULL(__func, __int_func, __get_type)\
static int __func(mysqli_object *obj, zval *retval, bool quiet)\
{\
	char *c;\
	__get_type;\
	ZEND_ASSERT(p);\
	c = (char *)__int_func(p);\
	if (c) {\
		ZVAL_STRING(retval, c);\
	} else {\
		ZVAL_NULL(retval);\
	}\
	return SUCCESS; \
}

#define MYSQLI_MAP_PROPERTY_FUNC_STR(__func, __int_func, __get_type)\
static int __func(mysqli_object *obj, zval *retval, bool quiet)\
{\
	char *c;\
	__get_type;\
	ZEND_ASSERT(p);\
	c = (char *)__int_func(p);\
	ZEND_ASSERT(c);\
	ZVAL_STRING(retval, c);\
	return SUCCESS; \
}

/* {{{ property link_client_version_read */
static int link_client_version_read(mysqli_object *obj, zval *retval, bool quiet)
{
	ZVAL_LONG(retval, MYSQL_VERSION_ID);

	return SUCCESS;
}
/* }}} */

/* {{{ property link_client_info_read */
static int link_client_info_read(mysqli_object *obj, zval *retval, bool quiet)
{
	ZVAL_STRING(retval, MYSQL_SERVER_VERSION);

	return SUCCESS;
}
/* }}} */

/* {{{ property link_connect_errno_read */
static int link_connect_errno_read(mysqli_object *obj, zval *retval, bool quiet)
{
	ZVAL_LONG(retval, (zend_long)MyG(error_no));

	return SUCCESS;
}
/* }}} */

/* {{{ property link_connect_error_read */
static int link_connect_error_read(mysqli_object *obj, zval *retval, bool quiet)
{
	if (MyG(error_msg)) {
		ZVAL_STRING(retval, MyG(error_msg));
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}
/* }}} */

/* {{{ property link_affected_rows_read */
static int link_affected_rows_read(mysqli_object *obj, zval *retval, bool quiet)
{
	MY_MYSQL *mysql;
	my_ulonglong rc;

	CHECK_STATUS(MYSQLI_STATUS_VALID, quiet);

	mysql = (MY_MYSQL *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr;
	ZEND_ASSERT(mysql);

	rc = mysql_affected_rows(mysql->mysql);

	if (rc == (my_ulonglong) -1) {
		ZVAL_LONG(retval, -1);
		return SUCCESS;
	}

	if (rc < ZEND_LONG_MAX) {
		ZVAL_LONG(retval, (zend_long) rc);
	} else {
		ZVAL_NEW_STR(retval, strpprintf(0, MYSQLI_LLU_SPEC, rc));
	}

	return SUCCESS;
}
/* }}} */

/* {{{ property link_error_list_read */
static int link_error_list_read(mysqli_object *obj, zval *retval, bool quiet)
{
	MY_MYSQL *mysql;

	CHECK_STATUS(MYSQLI_STATUS_VALID, quiet);

	mysql = (MY_MYSQL *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr;

	if (mysql) {
		array_init(retval);
		MYSQLND_ERROR_LIST_ELEMENT * message;
		zend_llist_position pos;
		for (message = (MYSQLND_ERROR_LIST_ELEMENT *) zend_llist_get_first_ex(&mysql->mysql->data->error_info->error_list, &pos);
				message;
				message = (MYSQLND_ERROR_LIST_ELEMENT *) zend_llist_get_next_ex(&mysql->mysql->data->error_info->error_list, &pos))
		{
			zval single_error;
			array_init(&single_error);
			add_assoc_long_ex(&single_error, "errno", sizeof("errno") - 1, message->error_no);
			add_assoc_string_ex(&single_error, "sqlstate", sizeof("sqlstate") - 1, message->sqlstate);
			add_assoc_string_ex(&single_error, "error", sizeof("error") - 1, message->error);
			add_next_index_zval(retval, &single_error);
		}
	} else {
		ZVAL_EMPTY_ARRAY(retval);
	}

	return SUCCESS;
}
/* }}} */

/* link properties */
MYSQLI_MAP_PROPERTY_FUNC_LONG(link_errno_read, mysql_errno, MYSQLI_GET_MYSQL(MYSQLI_STATUS_INITIALIZED), zend_ulong, ZEND_ULONG_FMT)
MYSQLI_MAP_PROPERTY_FUNC_STR(link_error_read, mysql_error, MYSQLI_GET_MYSQL(MYSQLI_STATUS_INITIALIZED))
MYSQLI_MAP_PROPERTY_FUNC_LONG(link_field_count_read, mysql_field_count, MYSQLI_GET_MYSQL(MYSQLI_STATUS_VALID), zend_ulong, ZEND_ULONG_FMT)
MYSQLI_MAP_PROPERTY_FUNC_STR(link_host_info_read, mysql_get_host_info, MYSQLI_GET_MYSQL(MYSQLI_STATUS_VALID))
MYSQLI_MAP_PROPERTY_FUNC_STR_OR_NULL(link_info_read, mysql_info, MYSQLI_GET_MYSQL(MYSQLI_STATUS_VALID))
MYSQLI_MAP_PROPERTY_FUNC_LONG_OR_STR(link_insert_id_read, mysql_insert_id, MYSQLI_GET_MYSQL(MYSQLI_STATUS_VALID), my_ulonglong, MYSQLI_LLU_SPEC)
MYSQLI_MAP_PROPERTY_FUNC_LONG(link_protocol_version_read, mysql_get_proto_info, MYSQLI_GET_MYSQL(MYSQLI_STATUS_VALID), zend_ulong, ZEND_ULONG_FMT)
MYSQLI_MAP_PROPERTY_FUNC_STR(link_server_info_read, mysql_get_server_info, MYSQLI_GET_MYSQL(MYSQLI_STATUS_VALID))
MYSQLI_MAP_PROPERTY_FUNC_LONG(link_server_version_read, mysql_get_server_version, MYSQLI_GET_MYSQL(MYSQLI_STATUS_VALID), zend_ulong, ZEND_ULONG_FMT)
MYSQLI_MAP_PROPERTY_FUNC_STR(link_sqlstate_read, mysql_sqlstate, MYSQLI_GET_MYSQL(MYSQLI_STATUS_VALID))
MYSQLI_MAP_PROPERTY_FUNC_LONG(link_thread_id_read, mysql_thread_id, MYSQLI_GET_MYSQL(MYSQLI_STATUS_VALID), zend_ulong, ZEND_ULONG_FMT)
MYSQLI_MAP_PROPERTY_FUNC_LONG(link_warning_count_read, mysql_warning_count, MYSQLI_GET_MYSQL(MYSQLI_STATUS_VALID), zend_ulong, ZEND_ULONG_FMT)

/* result properties */

/* {{{ property result_type_read */
static int result_type_read(mysqli_object *obj, zval *retval, bool quiet)
{
	MYSQL_RES *p;

	CHECK_STATUS(MYSQLI_STATUS_VALID, quiet);
	p = (MYSQL_RES *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr;

	ZEND_ASSERT(p);
	ZVAL_LONG(retval, mysqli_result_is_unbuffered(p) ? MYSQLI_USE_RESULT:MYSQLI_STORE_RESULT);

	return SUCCESS;
}
/* }}} */

/* {{{ property result_lengths_read */
static int result_lengths_read(mysqli_object *obj, zval *retval, bool quiet)
{
	MYSQL_RES *p;
	const size_t *ret;
	uint32_t field_count;

	CHECK_STATUS(MYSQLI_STATUS_VALID, quiet);
	p = (MYSQL_RES *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr;
	field_count = mysql_num_fields(p);
	if (!p || !field_count || !(ret = mysql_fetch_lengths(p))) {
		ZVAL_NULL(retval);
	} else {
		zend_ulong i;

		array_init(retval);

		for (i = 0; i < field_count; i++) {
			add_index_long(retval, i, ret[i]);
		}
	}

	return SUCCESS;
}
/* }}} */

MYSQLI_MAP_PROPERTY_FUNC_LONG(result_current_field_read, mysql_field_tell, MYSQLI_GET_RESULT(MYSQLI_STATUS_VALID), zend_ulong, ZEND_ULONG_FMT)
MYSQLI_MAP_PROPERTY_FUNC_LONG(result_field_count_read, mysql_num_fields, MYSQLI_GET_RESULT(MYSQLI_STATUS_VALID), zend_ulong, ZEND_ULONG_FMT)
MYSQLI_MAP_PROPERTY_FUNC_LONG_OR_STR(result_num_rows_read, mysql_num_rows, MYSQLI_GET_RESULT(MYSQLI_STATUS_VALID), my_ulonglong, MYSQLI_LLU_SPEC)

/* statement properties */

/* {{{ property stmt_id_read */
static int stmt_id_read(mysqli_object *obj, zval *retval, bool quiet)
{
	MY_STMT *p;

	CHECK_STATUS(MYSQLI_STATUS_VALID, quiet);

	p = (MY_STMT*)((MYSQLI_RESOURCE *)(obj->ptr))->ptr;

	ZEND_ASSERT(p);
	ZVAL_LONG(retval, mysqli_stmt_get_id(p->stmt));

	return SUCCESS;
}
/* }}} */

/* {{{ property stmt_affected_rows_read */
static int stmt_affected_rows_read(mysqli_object *obj, zval *retval, bool quiet)
{
	MY_STMT *p;
	my_ulonglong rc;

	CHECK_STATUS(MYSQLI_STATUS_VALID, quiet);

	p = (MY_STMT *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr;

	ZEND_ASSERT(p);
	rc = mysql_stmt_affected_rows(p->stmt);

	if (rc == (my_ulonglong) -1) {
		ZVAL_LONG(retval, -1);
		return SUCCESS;
	}

	if (rc < ZEND_LONG_MAX) {
		ZVAL_LONG(retval, (zend_long) rc);
	} else {
		ZVAL_NEW_STR(retval, strpprintf(0, MYSQLI_LLU_SPEC, rc));
	}

	return SUCCESS;
}
/* }}} */

/* {{{ property stmt_error_list_read */
static int stmt_error_list_read(mysqli_object *obj, zval *retval, bool quiet)
{
	MY_STMT * stmt;

	CHECK_STATUS(MYSQLI_STATUS_INITIALIZED, quiet);

	stmt = (MY_STMT *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr;
	if (stmt && stmt->stmt) {
		array_init(retval);
		if (stmt->stmt->data && stmt->stmt->data->error_info) {
			MYSQLND_ERROR_LIST_ELEMENT * message;
			zend_llist_position pos;
			for (message = (MYSQLND_ERROR_LIST_ELEMENT *) zend_llist_get_first_ex(&stmt->stmt->data->error_info->error_list, &pos);
				 message;
				 message = (MYSQLND_ERROR_LIST_ELEMENT *) zend_llist_get_next_ex(&stmt->stmt->data->error_info->error_list, &pos))
			{
				zval single_error;
				array_init(&single_error);
				add_assoc_long_ex(&single_error, "errno", sizeof("errno") - 1, message->error_no);
				add_assoc_string_ex(&single_error, "sqlstate", sizeof("sqlstate") - 1, message->sqlstate);
				add_assoc_string_ex(&single_error, "error", sizeof("error") - 1, message->error);
				add_next_index_zval(retval, &single_error);
			}
		}
	} else {
		ZVAL_EMPTY_ARRAY(retval);
	}

	return SUCCESS;
}
/* }}} */

MYSQLI_MAP_PROPERTY_FUNC_LONG_OR_STR(stmt_insert_id_read, mysql_stmt_insert_id, MYSQLI_GET_STMT(MYSQLI_STATUS_VALID), my_ulonglong, MYSQLI_LLU_SPEC)
MYSQLI_MAP_PROPERTY_FUNC_LONG_OR_STR(stmt_num_rows_read, mysql_stmt_num_rows, MYSQLI_GET_STMT(MYSQLI_STATUS_VALID), my_ulonglong, MYSQLI_LLU_SPEC)
MYSQLI_MAP_PROPERTY_FUNC_LONG(stmt_param_count_read, mysql_stmt_param_count, MYSQLI_GET_STMT(MYSQLI_STATUS_VALID), zend_ulong, ZEND_ULONG_FMT)
MYSQLI_MAP_PROPERTY_FUNC_LONG(stmt_field_count_read, mysql_stmt_field_count, MYSQLI_GET_STMT(MYSQLI_STATUS_VALID), zend_ulong, ZEND_ULONG_FMT)
MYSQLI_MAP_PROPERTY_FUNC_LONG(stmt_errno_read, mysql_stmt_errno, MYSQLI_GET_STMT(MYSQLI_STATUS_INITIALIZED), zend_ulong, ZEND_ULONG_FMT)
MYSQLI_MAP_PROPERTY_FUNC_STR(stmt_error_read, mysql_stmt_error, MYSQLI_GET_STMT(MYSQLI_STATUS_INITIALIZED))
MYSQLI_MAP_PROPERTY_FUNC_STR(stmt_sqlstate_read, mysql_stmt_sqlstate, MYSQLI_GET_STMT(MYSQLI_STATUS_INITIALIZED))

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
	{"sqlstate",		sizeof("sqlstate") - 1,			link_sqlstate_read, NULL},
	{"protocol_version",sizeof("protocol_version") - 1,	link_protocol_version_read, NULL},
	{"thread_id",		sizeof("thread_id") - 1, 		link_thread_id_read, NULL},
	{"warning_count",	sizeof("warning_count") - 1, 	link_warning_count_read, NULL},
	{NULL, 0, NULL, NULL}
};


const mysqli_property_entry mysqli_result_property_entries[] = {
	{"current_field",sizeof("current_field")-1,	result_current_field_read, NULL},
	{"field_count", sizeof("field_count") - 1,	result_field_count_read, NULL},
	{"lengths", 	sizeof("lengths") - 1,		result_lengths_read, NULL},
	{"num_rows", 	sizeof("num_rows") - 1,		result_num_rows_read, NULL},
	{"type", 		sizeof("type") - 1,			result_type_read, NULL},
	{NULL, 0, NULL, NULL}
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
