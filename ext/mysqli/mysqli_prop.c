/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2002 The PHP Group                                |
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

#define MYSQLI_MAP_PROPERTY_LONG( __func, __type, __val)\
int __func(mysqli_object *obj, zval **retval TSRMLS_DC) \
{\
	__type *p = (__type *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr; \
	ALLOC_ZVAL(*retval);\
	if (!p) {\
		ZVAL_NULL(*retval);\
	} else {\
		ZVAL_LONG(*retval, (long)p->__val);\
	}\
	return SUCCESS;\
}\

#define MYSQLI_MAP_PROPERTY_LONG_LONG( __func, __type, __val)\
int __func(mysqli_object *obj, zval **retval TSRMLS_DC) \
{\
	__type *p = (__type *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr; \
	ALLOC_ZVAL(*retval);\
	if (!p) {\
		ZVAL_NULL(*retval);\
	} else {\
		if (p->__val < LONG_MAX) { \
			ZVAL_LONG(*retval, (long)p->__val); \
		} else { \
			char ret[40]; \
			sprintf(ret, "%llu", p->__val); \
			ZVAL_STRING(*retval, ret, 1); \
		} \
	}\
	return SUCCESS;\
}\

#define MYSQLI_MAP_PROPERTY_STRING( __func, __type, __val)\
int __func(mysqli_object *obj, zval **retval TSRMLS_DC) \
{\
	__type *p = (__type *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr; \
	ALLOC_ZVAL(*retval);\
	if (!p) {\
		ZVAL_NULL(*retval);\
	} else {\
		if (!p->__val) {\
			ZVAL_NULL(*retval);\
		} else {\
			ZVAL_STRING(*retval, p->__val, 1);\
		}\
	}\
	return SUCCESS;\
}\

/* {{{ property link_client_version_read */
int link_client_version_read(mysqli_object *obj, zval **retval TSRMLS_DC) {
	ALLOC_ZVAL(*retval);
	ZVAL_STRING(*retval, MYSQL_SERVER_VERSION, 1);
	return SUCCESS;
}
/* }}} */

/* {{{ property link_connect_errno_read */
int link_connect_errno_read(mysqli_object *obj, zval **retval TSRMLS_DC) {
	ALLOC_ZVAL(*retval);
	ZVAL_LONG(*retval, (long)MyG(error_no));
	return SUCCESS;
}
/* }}} */

/* {{{ property link_connect_error_read */
int link_connect_error_read(mysqli_object *obj, zval **retval TSRMLS_DC) {
	ALLOC_ZVAL(*retval);
	ZVAL_STRING(*retval, MyG(error_msg), 1);
	return SUCCESS;
}
/* }}} */

/* link properties */
MYSQLI_MAP_PROPERTY_LONG_LONG(link_affected_rows_read, MYSQL, last_used_con->affected_rows);
MYSQLI_MAP_PROPERTY_LONG(link_client_flags_read, MYSQL, client_flag);
MYSQLI_MAP_PROPERTY_LONG(link_errno_read, MYSQL, net.last_errno);
MYSQLI_MAP_PROPERTY_STRING(link_error_read, MYSQL, net.last_error);
MYSQLI_MAP_PROPERTY_LONG(link_field_count_read, MYSQL, field_count);
MYSQLI_MAP_PROPERTY_STRING(link_host_read, MYSQL, host);
MYSQLI_MAP_PROPERTY_STRING(link_host_info_read, MYSQL, host_info);
MYSQLI_MAP_PROPERTY_STRING(link_info_read, MYSQL, info);
MYSQLI_MAP_PROPERTY_LONG_LONG(link_insert_id_read, MYSQL, last_used_con->insert_id);
MYSQLI_MAP_PROPERTY_LONG(link_port_read, MYSQL, port);
MYSQLI_MAP_PROPERTY_LONG(link_protocol_version_read, MYSQL, protocol_version);
MYSQLI_MAP_PROPERTY_LONG(link_server_capabilities_read, MYSQL, server_capabilities);
MYSQLI_MAP_PROPERTY_LONG(link_server_language_read, MYSQL, server_language);
MYSQLI_MAP_PROPERTY_LONG(link_server_status_read, MYSQL, server_status);
MYSQLI_MAP_PROPERTY_STRING(link_server_version_read, MYSQL, server_version);
MYSQLI_MAP_PROPERTY_STRING(link_sqlstate_read, MYSQL, net.sqlstate);
MYSQLI_MAP_PROPERTY_LONG(link_thread_id_read, MYSQL, thread_id);
MYSQLI_MAP_PROPERTY_STRING(link_user_read, MYSQL, user);
MYSQLI_MAP_PROPERTY_LONG(link_warning_count_read, MYSQL, warning_count);

/* result properties */

/* {{{ property result_type_read */
int result_type_read(mysqli_object *obj, zval **retval TSRMLS_DC) {
	MYSQL_RES *p = (MYSQL_RES *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr;

	ALLOC_ZVAL(*retval);
	if (!p) {
		ZVAL_NULL(*retval);
	} else {
		ZVAL_LONG(*retval, (p->data) ? MYSQLI_STORE_RESULT : MYSQLI_USE_RESULT);
	}
	return SUCCESS;
}
/* }}} */

/* {{{ property result_lengths_read */
int result_lengths_read(mysqli_object *obj, zval **retval TSRMLS_DC) {
	MYSQL_RES *p = (MYSQL_RES *)((MYSQLI_RESOURCE *)(obj->ptr))->ptr;

	ALLOC_ZVAL(*retval);
	if (!p || !p->field_count) {
		ZVAL_NULL(*retval);
	} else {
		ulong i;
		zval *l;
	
		array_init(*retval);

		for (i=0; i < p->field_count; i++) {
			MAKE_STD_ZVAL(l);
			ZVAL_LONG(l, p->lengths[i]);
			add_index_zval(*retval, i, l);
		}	
	}
	return SUCCESS;
}
/* }}} */

MYSQLI_MAP_PROPERTY_LONG(result_current_field_read, MYSQL_RES, current_field);
MYSQLI_MAP_PROPERTY_LONG(result_field_count_read, MYSQL_RES, field_count);
MYSQLI_MAP_PROPERTY_LONG_LONG(result_num_rows_read, MYSQL_RES, row_count);

/* statement properties */
MYSQLI_MAP_PROPERTY_LONG_LONG(stmt_affected_rows_read, STMT, stmt->mysql->last_used_con->affected_rows);
MYSQLI_MAP_PROPERTY_STRING(stmt_query_read, STMT, stmt->query);
MYSQLI_MAP_PROPERTY_LONG(stmt_param_count_read, STMT, stmt->param_count);
MYSQLI_MAP_PROPERTY_LONG(stmt_field_count_read, STMT, stmt->field_count);
MYSQLI_MAP_PROPERTY_LONG(stmt_id_read, STMT, stmt->stmt_id);
MYSQLI_MAP_PROPERTY_LONG(stmt_errno_read, STMT, stmt->last_errno);
MYSQLI_MAP_PROPERTY_STRING(stmt_error_read, STMT, stmt->last_error);
MYSQLI_MAP_PROPERTY_STRING(stmt_sqlstate_read, STMT, stmt->sqlstate);

mysqli_property_entry mysqli_link_property_entries[] = {
	{"affected_rows", link_affected_rows_read, NULL},
	{"client_flags", link_client_flags_read, NULL},
	{"client_version", link_client_version_read, NULL},
	{"connect_errno", link_connect_errno_read, NULL},
	{"connect_error", link_connect_error_read, NULL},
	{"errno", link_errno_read, NULL},
	{"error", link_error_read, NULL},
	{"field_count", link_field_count_read, NULL},
	{"host", link_host_read, NULL},
	{"host_info", link_host_info_read, NULL},
	{"info", link_info_read, NULL},
	{"insert_id", link_insert_id_read, NULL},
	{"server_capabilities", link_server_capabilities_read, NULL},
	{"server_status", link_server_status_read, NULL},
	{"server_version", link_server_version_read, NULL},
	{"sqlstate", link_sqlstate_read, NULL},
	{"port", link_port_read, NULL},
	{"protocol_version", link_protocol_version_read, NULL},
	{"server_language", link_protocol_version_read, NULL},
	{"thread_id", link_thread_id_read, NULL},
	{"user", link_user_read, NULL},
	{"warning_count", link_warning_count_read, NULL},
	{NULL, NULL, NULL}	
};

mysqli_property_entry mysqli_result_property_entries[] = {
	{"current_field", result_current_field_read, NULL},
	{"field_count", result_field_count_read, NULL},
	{"lengths", result_lengths_read, NULL},
	{"num_rows", result_num_rows_read, NULL},
	{"type", result_type_read, NULL},
	{NULL, NULL, NULL}
};

mysqli_property_entry mysqli_stmt_property_entries[] = {
	{"affected_rows", stmt_affected_rows_read, NULL},
	{"query", stmt_query_read, NULL},
	{"param_count", stmt_param_count_read, NULL},
	{"field_count", stmt_field_count_read, NULL},
	{"id", stmt_id_read, NULL},
	{"errno", stmt_errno_read, NULL},
	{"error", stmt_error_read, NULL},
	{"sqlstate", stmt_sqlstate_read, NULL},
	{NULL, NULL, NULL}
};
