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

/* A little hack to prevent build break, when mysql is used together with
 * c-client, which also defines LIST.
 */
#ifdef LIST
#undef LIST
#endif

#include <mysql.h>

#ifndef PHP_MYSQLI_H
#define PHP_MYSQLI_H


typedef struct {
	ulong		buflen;
	char		*buffer;
	ulong		type;
} BIND_BUFFER;

typedef struct {
	MYSQL_STMT	*stmt;
	unsigned int	var_cnt;
	zval		**vars;
 	BIND_BUFFER	*bind;
	char		*is_null;
	char		type;
} STMT;

typedef struct {
	void		*prinfo;	/* profiler info */
	void		*ptr;		/* resource: (mysql, result, stmt) */
} MYSQLI_RESOURCE;

typedef struct _mysqli_object {
	zend_object zo;
	void *ptr;
} mysqli_object; /* extends zend_object */

#define phpext_mysqli_ptr &mysqli_module_entry

#ifdef PHP_WIN32
#define PHP_MYSQLI_API __declspec(dllexport)
#else
#define PHP_MYSQLI_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#define PHP_MYSQLI_EXPORT(__type) PHP_MYSQLI_API __type

extern zend_module_entry mysqli_module_entry;
extern function_entry mysqli_functions[];
extern function_entry mysqli_link_methods[];
extern function_entry mysqli_stmt_methods[];
extern function_entry mysqli_result_methods[];
extern void php_mysqli_fetch_into_hash(INTERNAL_FUNCTION_PARAMETERS, int flag);
extern void php_clear_stmt_bind(STMT *stmt);

extern int le_mysqli_link, le_mysqli_result, le_mysqli_stmt;

#define MYSQLI_RETURN_LONG_LONG(__val) \
{ \
	if ((__val) < LONG_MAX) {		\
		RETURN_LONG((__val));		\
	} else {				\
		char ret[40];			\
		sprintf(ret, "%llu", (__val));	\
		RETURN_STRING(ret,1);		\
	}					\
}

#define MYSQLI_FETCH_STMT(rsrc, rsrc_type, passed_id, default_id, resource_type_name, resource_type) \
{ \
	ZEND_FETCH_RESOURCE(rsrc, rsrc_type, passed_id, default_id, resource_type_name, resource_type); \
	if (!rsrc->stmt->mysql) { \
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Connection closed. Statement resource invalid.");\
		RETURN_FALSE;\
	} \
}

#define MYSQLI_STORE_RESULT 	0
#define MYSQLI_USE_RESULT 	1

/* for mysqli_fetch_assoc */
#define MYSQLI_ASSOC		1
#define MYSQLI_NUM		2
#define MYSQLI_BOTH		3

/* for mysqli_bind_param */
#define MYSQLI_BIND_INT		1
#define MYSQLI_BIND_DOUBLE	2
#define MYSQLI_BIND_STRING	3
#define MYSQLI_BIND_SEND_DATA	4

/* fetch types */
#define FETCH_SIMPLE		1
#define FETCH_RESULT		2

PHP_MYSQLI_API void mysqli_register_link(zval *return_value, void *link TSRMLS_DC);
PHP_MYSQLI_API void mysqli_register_stmt(zval *return_value, void *stmt TSRMLS_DC);
PHP_MYSQLI_API void mysqli_register_result(zval *return_value, void *result TSRMLS_DC);
PHP_MYSQLI_API void php_mysqli_set_error(long mysql_errno, char *mysql_err TSRMLS_DC);

PHP_MINIT_FUNCTION(mysqli);
PHP_MSHUTDOWN_FUNCTION(mysqli);
PHP_RINIT_FUNCTION(mysqli);
PHP_RSHUTDOWN_FUNCTION(mysqli);
PHP_MINFO_FUNCTION(mysqli);

PHP_FUNCTION(mysqli_affected_rows);
PHP_FUNCTION(mysqli_autocommit);
PHP_FUNCTION(mysqli_bind_param);
PHP_FUNCTION(mysqli_bind_result);
PHP_FUNCTION(mysqli_change_user);
PHP_FUNCTION(mysqli_character_set_name);
PHP_FUNCTION(mysqli_close);
PHP_FUNCTION(mysqli_commit);
PHP_FUNCTION(mysqli_connect);
PHP_FUNCTION(mysqli_data_seek);
PHP_FUNCTION(mysqli_debug);
PHP_FUNCTION(mysqli_disable_reads_from_master);
PHP_FUNCTION(mysqli_disable_rpl_parse);
PHP_FUNCTION(mysqli_dump_debug_info);
PHP_FUNCTION(mysqli_enable_reads_from_master);
PHP_FUNCTION(mysqli_enable_rpl_parse);
PHP_FUNCTION(mysqli_errno);
PHP_FUNCTION(mysqli_error);
PHP_FUNCTION(mysqli_execute);
PHP_FUNCTION(mysqli_fetch);
PHP_FUNCTION(mysqli_fetch_array);
PHP_FUNCTION(mysqli_fetch_assoc);
PHP_FUNCTION(mysqli_fetch_object);
PHP_FUNCTION(mysqli_fetch_field);
PHP_FUNCTION(mysqli_fetch_fields);
PHP_FUNCTION(mysqli_fetch_field_direct);
PHP_FUNCTION(mysqli_fetch_lengths);
PHP_FUNCTION(mysqli_fetch_row);
PHP_FUNCTION(mysqli_field_count);
PHP_FUNCTION(mysqli_field_seek);
PHP_FUNCTION(mysqli_field_tell);
PHP_FUNCTION(mysqli_free_result);
PHP_FUNCTION(mysqli_get_client_info);
PHP_FUNCTION(mysqli_get_host_info);
PHP_FUNCTION(mysqli_get_proto_info);
PHP_FUNCTION(mysqli_get_server_info);
PHP_FUNCTION(mysqli_get_server_version);
PHP_FUNCTION(mysqli_info);
PHP_FUNCTION(mysqli_insert_id);
PHP_FUNCTION(mysqli_init);
PHP_FUNCTION(mysqli_kill);
PHP_FUNCTION(mysqli_master_query);
PHP_FUNCTION(mysqli_num_fields);
PHP_FUNCTION(mysqli_num_rows);
PHP_FUNCTION(mysqli_options);
PHP_FUNCTION(mysqli_param_count);
PHP_FUNCTION(mysqli_ping);
PHP_FUNCTION(mysqli_prepare);
PHP_FUNCTION(mysqli_query);
PHP_FUNCTION(mysqli_prepare_result);
PHP_FUNCTION(mysqli_read_query_result);
PHP_FUNCTION(mysqli_real_connect);
PHP_FUNCTION(mysqli_real_query);
PHP_FUNCTION(mysqli_real_escape_string);
PHP_FUNCTION(mysqli_reload);
PHP_FUNCTION(mysqli_rollback);
PHP_FUNCTION(mysqli_row_seek);
PHP_FUNCTION(mysqli_rpl_parse_enabled);
PHP_FUNCTION(mysqli_rpl_probe);
PHP_FUNCTION(mysqli_rpl_query_type);
PHP_FUNCTION(mysqli_select_db);
PHP_FUNCTION(mysqli_send_long_data);
PHP_FUNCTION(mysqli_send_query);
PHP_FUNCTION(mysqli_slave_query);
PHP_FUNCTION(mysqli_ssl_set);
PHP_FUNCTION(mysqli_stat);
PHP_FUNCTION(mysqli_stmt_affected_rows);
PHP_FUNCTION(mysqli_stmt_close);
PHP_FUNCTION(mysqli_stmt_errno);
PHP_FUNCTION(mysqli_stmt_error);
PHP_FUNCTION(mysqli_stmt_store_result);
PHP_FUNCTION(mysqli_store_result);
PHP_FUNCTION(mysqli_thread_id);
PHP_FUNCTION(mysqli_thread_safe);
PHP_FUNCTION(mysqli_use_result);
PHP_FUNCTION(mysqli_warning_count);

ZEND_BEGIN_MODULE_GLOBALS(mysqli)
	long		default_link;
	long		num_links;
	long		max_links;
	unsigned int	default_port;
	char		*default_host;
	char		*default_user;
	char		*default_pw;
	char		*default_socket;
	long		error_no;
	char		*error_msg;
ZEND_END_MODULE_GLOBALS(mysqli)

#ifdef ZTS
#define MyG(v) TSRMG(mysqli_globals_id, zend_mysqli_globals *, v)
#else
#define MyG(v) (mysqli_globals.v)
#endif

#define my_estrdup(x) (x) ? estrdup(x) : NULL
#define my_efree(x) if (x) efree(x)

ZEND_EXTERN_MODULE_GLOBALS(mysqli);

#endif	/* PHP_MYSQLI.H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
