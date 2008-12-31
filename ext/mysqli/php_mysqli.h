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

/* A little hack to prevent build break, when mysql is used together with
 * c-client, which also defines LIST.
 */
#ifdef LIST
#undef LIST
#endif

#include <mysql.h>

/* character set support */
#if MYSQL_VERSION_ID > 50009
#define HAVE_MYSQLI_GET_CHARSET
#endif

#if (MYSQL_VERSION_ID > 40112 && MYSQL_VERSION_ID < 50000) || MYSQL_VERSION_ID > 50005
#define HAVE_MYSQLI_SET_CHARSET
#endif


#include <errmsg.h>

#ifndef PHP_MYSQLI_H
#define PHP_MYSQLI_H

#define MYSQLI_VERSION_ID		101009

enum mysqli_status {
	MYSQLI_STATUS_UNKNOWN=0,
	MYSQLI_STATUS_CLEARED,
	MYSQLI_STATUS_INITIALIZED,
	MYSQLI_STATUS_VALID
};

typedef struct {
	ulong		buflen;
	char		*val;
	ulong		type;
} VAR_BUFFER;

typedef struct {
	unsigned int	var_cnt;
	VAR_BUFFER		*buf;
	zval			**vars;
	char			*is_null;
} BIND_BUFFER;

typedef struct {
	MYSQL_STMT			*stmt;
	BIND_BUFFER			param;
	BIND_BUFFER			result;
	char				*query;
} MY_STMT;

typedef struct {
	MYSQL				*mysql;
	zval				*li_read;
	php_stream			*li_stream;
	unsigned int 		multi_query;	
} MY_MYSQL;

typedef struct {
	int			mode;
	int			socket;
	FILE		*fp;
} PROFILER;

typedef struct {
	void				*ptr;		/* resource: (mysql, result, stmt)   */
	void				*info;		/* additional buffer				 */
    enum mysqli_status	status;
} MYSQLI_RESOURCE;

typedef struct _mysqli_object {
	zend_object 		zo;
	void 				*ptr;
	HashTable 			*prop_handler;
} mysqli_object; /* extends zend_object */

typedef struct {
	char			*reason;
	char			sqlstate[6];
	int				errorno;
   	void			*next;
} MYSQLI_WARNING;

typedef struct _mysqli_property_entry {
	char *pname;
	int (*r_func)(mysqli_object *obj, zval **retval TSRMLS_DC);
	int (*w_func)(mysqli_object *obj, zval *value TSRMLS_DC);
} mysqli_property_entry;

typedef struct {
	char	error_msg[LOCAL_INFILE_ERROR_LEN];
	void	*userdata;
} mysqli_local_infile;

#define phpext_mysqli_ptr &mysqli_module_entry

#ifdef PHP_WIN32
#define PHP_MYSQLI_API __declspec(dllexport)
#define MYSQLI_LLU_SPEC "%I64u"
#define MYSQLI_LL_SPEC "%I64d"
#else
#define PHP_MYSQLI_API
#define MYSQLI_LLU_SPEC "%llu"
#define MYSQLI_LL_SPEC "%lld"
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#define PHP_MYSQLI_EXPORT(__type) PHP_MYSQLI_API __type

extern zend_module_entry mysqli_module_entry;
extern zend_function_entry mysqli_functions[];
extern zend_function_entry mysqli_link_methods[];
extern zend_function_entry mysqli_stmt_methods[];
extern zend_function_entry mysqli_result_methods[];
extern zend_function_entry mysqli_driver_methods[];
extern zend_function_entry mysqli_warning_methods[];
extern zend_function_entry mysqli_exception_methods[];

extern mysqli_property_entry mysqli_link_property_entries[];
extern mysqli_property_entry mysqli_result_property_entries[];
extern mysqli_property_entry mysqli_stmt_property_entries[];
extern mysqli_property_entry mysqli_driver_property_entries[];
extern mysqli_property_entry mysqli_warning_property_entries[];

extern void php_mysqli_fetch_into_hash(INTERNAL_FUNCTION_PARAMETERS, int override_flag, int into_object);
extern void php_clear_stmt_bind(MY_STMT *stmt);
extern void php_clear_mysql(MY_MYSQL *);
extern MYSQLI_WARNING *php_get_warnings(MYSQL *mysql);
extern void php_clear_warnings(MYSQLI_WARNING *w);
extern void php_free_stmt_bind_buffer(BIND_BUFFER bbuf, int type);
extern void php_mysqli_report_error(char *sqlstate, int errorno, char *error TSRMLS_DC);
extern void php_mysqli_report_index(char *query, unsigned int status TSRMLS_DC);
extern int php_local_infile_init(void **, const char *, void *);
extern int php_local_infile_read(void *, char *, uint);
extern void php_local_infile_end(void *);
extern int php_local_infile_error(void *, char *, uint);
extern void php_set_local_infile_handler_default(MY_MYSQL *);
extern void php_mysqli_throw_sql_exception(char *sqlstate, int errorno TSRMLS_DC, char *format, ...);
extern zend_class_entry *mysqli_link_class_entry;
extern zend_class_entry *mysqli_stmt_class_entry;
extern zend_class_entry *mysqli_result_class_entry;
extern zend_class_entry *mysqli_driver_class_entry;
extern zend_class_entry *mysqli_warning_class_entry;
extern zend_class_entry *mysqli_exception_class_entry;

#ifdef HAVE_SPL
extern PHPAPI zend_class_entry *spl_ce_RuntimeException;
#endif

PHP_MYSQLI_EXPORT(zend_object_value) mysqli_objects_new(zend_class_entry * TSRMLS_DC);

#define MYSQLI_DISABLE_MQ if (mysql->multi_query) { \
	mysql_set_server_option(mysql->mysql, MYSQL_OPTION_MULTI_STATEMENTS_OFF); \
	mysql->multi_query = 0; \
} 

#define MYSQLI_ENABLE_MQ if (!mysql->multi_query) { \
	mysql_set_server_option(mysql->mysql, MYSQL_OPTION_MULTI_STATEMENTS_ON); \
	mysql->multi_query = 1; \
} 

#define REGISTER_MYSQLI_CLASS_ENTRY(name, mysqli_entry, class_functions) { \
	zend_class_entry ce; \
	INIT_CLASS_ENTRY(ce, name,class_functions); \
	ce.create_object = mysqli_objects_new; \
	mysqli_entry = zend_register_internal_class(&ce TSRMLS_CC); \
} \

#define MYSQLI_REGISTER_RESOURCE_EX(__ptr, __zval)  \
	((mysqli_object *) zend_object_store_get_object(__zval TSRMLS_CC))->ptr = __ptr; \

#define MYSQLI_RETURN_RESOURCE(__ptr, __ce) \
	Z_TYPE_P(return_value) = IS_OBJECT; \
	(return_value)->value.obj = mysqli_objects_new(__ce TSRMLS_CC); \
	MYSQLI_REGISTER_RESOURCE_EX(__ptr, return_value)

#define MYSQLI_REGISTER_RESOURCE(__ptr, __ce) \
{\
	zval *object = getThis();\
	if (!object || !instanceof_function(Z_OBJCE_P(object), mysqli_link_class_entry TSRMLS_CC)) {\
		object = return_value;\
		Z_TYPE_P(object) = IS_OBJECT;\
		(object)->value.obj = mysqli_objects_new(__ce TSRMLS_CC);\
	}\
	MYSQLI_REGISTER_RESOURCE_EX(__ptr, object)\
}

#define MYSQLI_FETCH_RESOURCE(__ptr, __type, __id, __name, __check) \
{ \
	MYSQLI_RESOURCE *my_res; \
	mysqli_object *intern = (mysqli_object *)zend_object_store_get_object(*(__id) TSRMLS_CC);\
	if (!(my_res = (MYSQLI_RESOURCE *)intern->ptr)) {\
  		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't fetch %s", intern->zo.ce->name);\
  		RETURN_NULL();\
  	}\
	__ptr = (__type)my_res->ptr; \
	if (__check && my_res->status < __check) { \
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid object or resource %s\n", intern->zo.ce->name); \
		RETURN_NULL();\
	}\
} 

#define MYSQLI_SET_STATUS(__id, __value) \
{ \
	mysqli_object *intern = (mysqli_object *)zend_object_store_get_object(*(__id) TSRMLS_CC);\
	((MYSQLI_RESOURCE *)intern->ptr)->status = __value; \
} \

#define MYSQLI_CLEAR_RESOURCE(__id) \
{ \
	mysqli_object *intern = (mysqli_object *)zend_object_store_get_object(*(__id) TSRMLS_CC);\
	efree(intern->ptr); \
	intern->ptr = NULL; \
}

#define MYSQLI_RETURN_LONG_LONG(__val) \
{ \
	if ((__val) < LONG_MAX) {		\
		RETURN_LONG((long) (__val));		\
	} else {				\
		char *ret;			\
		/* always used with my_ulonglong -> %llu */ \
		int l = spprintf(&ret, 0, "%llu", (__val));	\
		RETURN_STRINGL(ret, l, 0);		\
	}					\
}

#define MYSQLI_ADD_PROPERTIES(a,b) \
{ \
	int i = 0; \
	while (b[i].pname != NULL) { \
		mysqli_add_property(a, b[i].pname, (mysqli_read_t)b[i].r_func, (mysqli_write_t)b[i].w_func TSRMLS_CC); \
		i++; \
	}\
}

#if WIN32|WINNT
#define SCLOSE(a) closesocket(a)
#else
#define SCLOSE(a) close(a)
#endif

#define MYSQLI_STORE_RESULT 0
#define MYSQLI_USE_RESULT 	1

/* for mysqli_fetch_assoc */
#define MYSQLI_ASSOC	1
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

/*** REPORT MODES ***/
#define MYSQLI_REPORT_OFF           0
#define MYSQLI_REPORT_ERROR			1
#define MYSQLI_REPORT_STRICT		2
#define MYSQLI_REPORT_INDEX			4
#define MYSQLI_REPORT_CLOSE			8	
#define MYSQLI_REPORT_ALL		  255

#define MYSQLI_REPORT_MYSQL_ERROR(mysql) \
if ((MyG(report_mode) & MYSQLI_REPORT_ERROR) && mysql->net.last_errno) { \
	php_mysqli_report_error(mysql->net.sqlstate, mysql->net.last_errno, mysql->net.last_error TSRMLS_CC); \
}

#define MYSQLI_REPORT_STMT_ERROR(stmt) \
if ((MyG(report_mode) & MYSQLI_REPORT_ERROR) && stmt->last_errno) { \
	php_mysqli_report_error(stmt->sqlstate, stmt->last_errno, stmt->last_error TSRMLS_CC); \
}

#if MYSQL_VERSION_ID > 32300 && MYSQL_VERSION_ID < 60000
#define HAVE_LIBMYSQL_REPLICATION
#endif


PHP_MYSQLI_API void mysqli_register_link(zval *return_value, void *link TSRMLS_DC);
PHP_MYSQLI_API void mysqli_register_stmt(zval *return_value, void *stmt TSRMLS_DC);
PHP_MYSQLI_API void mysqli_register_result(zval *return_value, void *result TSRMLS_DC);
PHP_MYSQLI_API void php_mysqli_set_error(long mysql_errno, char *mysql_err TSRMLS_DC);

PHP_MINIT_FUNCTION(mysqli);
PHP_MSHUTDOWN_FUNCTION(mysqli);
PHP_RINIT_FUNCTION(mysqli);
PHP_RSHUTDOWN_FUNCTION(mysqli);
PHP_MINFO_FUNCTION(mysqli);

PHP_FUNCTION(mysqli);
PHP_FUNCTION(mysqli_affected_rows);
PHP_FUNCTION(mysqli_autocommit);
PHP_FUNCTION(mysqli_change_user);
PHP_FUNCTION(mysqli_character_set_name);
#ifdef HAVE_MYSQLI_SET_CHARSET
PHP_FUNCTION(mysqli_set_charset);
#endif
PHP_FUNCTION(mysqli_close);
PHP_FUNCTION(mysqli_commit);
PHP_FUNCTION(mysqli_connect);
PHP_FUNCTION(mysqli_connect_errno);
PHP_FUNCTION(mysqli_connect_error);
PHP_FUNCTION(mysqli_data_seek);
PHP_FUNCTION(mysqli_debug);
PHP_FUNCTION(mysqli_disable_reads_from_master);
PHP_FUNCTION(mysqli_disable_rpl_parse);
PHP_FUNCTION(mysqli_dump_debug_info);
PHP_FUNCTION(mysqli_enable_reads_from_master);
PHP_FUNCTION(mysqli_enable_rpl_parse);
PHP_FUNCTION(mysqli_errno);
PHP_FUNCTION(mysqli_error);
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
#ifdef HAVE_MYSQLI_GET_CHARSET 
PHP_FUNCTION(mysqli_get_charset);
#endif
PHP_FUNCTION(mysqli_get_client_info);
PHP_FUNCTION(mysqli_get_client_version);
PHP_FUNCTION(mysqli_get_host_info);
PHP_FUNCTION(mysqli_get_proto_info);
PHP_FUNCTION(mysqli_get_server_info);
PHP_FUNCTION(mysqli_get_server_version);
PHP_FUNCTION(mysqli_get_warnings);
PHP_FUNCTION(mysqli_info);
PHP_FUNCTION(mysqli_insert_id);
PHP_FUNCTION(mysqli_init);
PHP_FUNCTION(mysqli_kill);
PHP_FUNCTION(mysqli_set_local_infile_default);
PHP_FUNCTION(mysqli_set_local_infile_handler);
PHP_FUNCTION(mysqli_master_query);
PHP_FUNCTION(mysqli_more_results);
PHP_FUNCTION(mysqli_multi_query);
PHP_FUNCTION(mysqli_next_result);
PHP_FUNCTION(mysqli_num_fields);
PHP_FUNCTION(mysqli_num_rows);
PHP_FUNCTION(mysqli_options);
PHP_FUNCTION(mysqli_ping);
PHP_FUNCTION(mysqli_prepare);
PHP_FUNCTION(mysqli_query);
PHP_FUNCTION(mysqli_stmt_result_metadata);
PHP_FUNCTION(mysqli_report);
PHP_FUNCTION(mysqli_read_query_result);
PHP_FUNCTION(mysqli_real_connect);
PHP_FUNCTION(mysqli_real_query);
PHP_FUNCTION(mysqli_real_escape_string);
PHP_FUNCTION(mysqli_rollback);
PHP_FUNCTION(mysqli_row_seek);
PHP_FUNCTION(mysqli_rpl_parse_enabled);
PHP_FUNCTION(mysqli_rpl_probe);
PHP_FUNCTION(mysqli_rpl_query_type);
PHP_FUNCTION(mysqli_select_db);
PHP_FUNCTION(mysqli_stmt_attr_get);
PHP_FUNCTION(mysqli_stmt_attr_set);
PHP_FUNCTION(mysqli_stmt_bind_param);
PHP_FUNCTION(mysqli_stmt_bind_result);
PHP_FUNCTION(mysqli_stmt_execute);
PHP_FUNCTION(mysqli_stmt_field_count);
PHP_FUNCTION(mysqli_stmt_init);
PHP_FUNCTION(mysqli_stmt_prepare);
PHP_FUNCTION(mysqli_stmt_fetch);
PHP_FUNCTION(mysqli_stmt_param_count);
PHP_FUNCTION(mysqli_stmt_send_long_data);
PHP_FUNCTION(mysqli_send_query);
PHP_FUNCTION(mysqli_embedded_server_end);
PHP_FUNCTION(mysqli_embedded_server_start);
PHP_FUNCTION(mysqli_slave_query);
PHP_FUNCTION(mysqli_sqlstate);
PHP_FUNCTION(mysqli_ssl_set);
PHP_FUNCTION(mysqli_stat);
PHP_FUNCTION(mysqli_stmt_affected_rows);
PHP_FUNCTION(mysqli_stmt_close);
PHP_FUNCTION(mysqli_stmt_data_seek);
PHP_FUNCTION(mysqli_stmt_errno);
PHP_FUNCTION(mysqli_stmt_error);
PHP_FUNCTION(mysqli_stmt_free_result);
PHP_FUNCTION(mysqli_stmt_get_warnings);
PHP_FUNCTION(mysqli_stmt_reset);
PHP_FUNCTION(mysqli_stmt_insert_id);
PHP_FUNCTION(mysqli_stmt_num_rows);
PHP_FUNCTION(mysqli_stmt_sqlstate);
PHP_FUNCTION(mysqli_stmt_store_result);
PHP_FUNCTION(mysqli_store_result);
PHP_FUNCTION(mysqli_thread_id);
PHP_FUNCTION(mysqli_thread_safe);
PHP_FUNCTION(mysqli_use_result);
PHP_FUNCTION(mysqli_warning_count);

ZEND_FUNCTION(mysqli_stmt_construct);
ZEND_FUNCTION(mysqli_result_construct);
ZEND_FUNCTION(mysqli_driver_construct);
ZEND_METHOD(mysqli_warning,__construct);

ZEND_BEGIN_MODULE_GLOBALS(mysqli)
	long			default_link;
	long			num_links;
	long			max_links;
	unsigned int	default_port;
	char			*default_host;
	char			*default_user;
	char			*default_socket;
	char            *default_pw;
	int				reconnect;
	int				strict;
	long			error_no;
	char			*error_msg;
	int				report_mode;
	HashTable		*report_ht;
	unsigned int	multi_query;
	unsigned int	embedded;
ZEND_END_MODULE_GLOBALS(mysqli)


#define MYSQLI_PROPERTY(a) extern int a(mysqli_object *obj, zval **retval TSRMLS_DC)

MYSQLI_PROPERTY(my_prop_link_host);

#ifdef ZTS
#define MyG(v) TSRMG(mysqli_globals_id, zend_mysqli_globals *, v)
#else
#define MyG(v) (mysqli_globals.v)
#endif

#define my_estrdup(x) (x) ? estrdup(x) : NULL
#define my_efree(x) if (x) efree(x)

#ifdef PHP_WIN32
#define L64(x) x##i64
typedef __int64 my_longlong;
#else
#define L64(x) x##LL
typedef long long my_longlong;
#endif


ZEND_EXTERN_MODULE_GLOBALS(mysqli)

#endif	/* PHP_MYSQLI.H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
