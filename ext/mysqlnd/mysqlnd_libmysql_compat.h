/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
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
  | Authors: Andrey Hristov <andrey@php.net>                             |
  |          Ulf Wendel <uw@php.net>                                     |
  |          Georg Richter <georg@php.net>                               |
  +----------------------------------------------------------------------+
*/
#ifndef MYSQLND_LIBMYSQL_COMPAT_H
#define MYSQLND_LIBMYSQL_COMPAT_H

/* Global types and definitions*/
#define MYSQL_NO_DATA			MYSQLND_NO_DATA
#define MYSQL_DATA_TRUNCATED	MYSQLND_DATA_TRUNCATED
#define MYSQL_STMT				MYSQLND_STMT
#define MYSQL_FIELD				MYSQLND_FIELD
#define MYSQL_RES				MYSQLND_RES
#define MYSQL_ROW				MYSQLND_ROW_C
#define MYSQL					MYSQLND
#define my_bool					zend_bool
#define my_ulonglong			uint64_t

#define MYSQL_VERSION_ID		MYSQLND_VERSION_ID
#define MYSQL_SERVER_VERSION	PHP_MYSQLND_VERSION
#define MYSQL_ERRMSG_SIZE		MYSQLND_ERRMSG_SIZE
#define SQLSTATE_LENGTH			MYSQLND_SQLSTATE_LENGTH

/* functions */
#define mysql_affected_rows(r)			mysqlnd_affected_rows((r))
#define mysql_autocommit(r,m)			mysqlnd_autocommit((r),(m))
#define mysql_change_user(r,a,b,c)		mysqlnd_change_user((r), (a), (b), (c), FALSE)
#define mysql_character_set_name(c)		mysqlnd_character_set_name((c))
#define mysql_close(r)					mysqlnd_close((r), MYSQLND_CLOSE_EXPLICIT)
#define mysql_commit(r)					mysqlnd_commit((r), TRANS_COR_NO_OPT, NULL)
#define mysql_data_seek(r,o)			mysqlnd_data_seek((r),(o))
#define mysql_debug(x)					mysqlnd_debug((x))
#define mysql_dump_debug_info(r)		mysqlnd_dump_debug_info((r))
#define mysql_errno(r)					mysqlnd_errno((r))
#define mysql_error(r)					mysqlnd_error((r))
#define mysql_escape_string(a,b,c)		mysqlnd_escape_string((a), (b), (c))
#define mysql_fetch_field(r)			mysqlnd_fetch_field((r))
#define mysql_fetch_field_direct(r,o)	mysqlnd_fetch_field_direct((r), (o))
#define mysql_fetch_fields(r)			mysqlnd_fetch_fields((r))
#define mysql_fetch_lengths(r)			mysqlnd_fetch_lengths((r))
#define mysql_fetch_row(r)				mysqlnd_fetch_row_c((r))
#define mysql_field_count(r)			mysqlnd_field_count((r))
#define mysql_field_seek(r,o)			mysqlnd_field_seek((r), (o))
#define mysql_field_tell(r)				mysqlnd_field_tell((r))
#define mysql_init(a)					mysqlnd_connection_init((a), false)
#define mysql_insert_id(r)				mysqlnd_insert_id((r))
#define mysql_kill(r,n)					mysqlnd_kill((r), (n))
#define mysql_list_dbs(c, wild)			mysqlnd_list_dbs((c), (wild))
#define mysql_list_processes(c)			mysqlnd_list_processes((c))
#define mysql_list_tables(c, wild)		mysqlnd_list_tables((c), (wild))
#define mysql_more_results(r)			mysqlnd_more_results((r))
#define mysql_next_result(r)			mysqlnd_next_result((r))
#define mysql_num_fields(r)				mysqlnd_num_fields((r))
#define mysql_num_rows(r)				mysqlnd_num_rows((r))
#define mysql_ping(r)					mysqlnd_ping((r))
#define mysql_real_escape_string(r,a,b,c) mysqlnd_real_escape_string((r), (a), (b), (c))
#define mysql_real_query(r,a,b)			mysqlnd_query((r), (a), (b))
#define mysql_refresh(conn, options)	mysqlnd_refresh((conn), (options))
#define mysql_rollback(r)				mysqlnd_rollback((r), TRANS_COR_NO_OPT, NULL)
#define mysql_select_db(r,a)			mysqlnd_select_db((r), (a) ,strlen((a)))
#define mysql_set_server_option(r,o)	mysqlnd_set_server_option((r), (o))
#define mysql_set_character_set(r,a)	mysqlnd_set_character_set((r), (a))
#define mysql_sqlstate(r)				mysqlnd_sqlstate((r))
#define mysql_ssl_set(c,key,cert,ca,capath,cipher)	mysqlnd_ssl_set((c), (key), (cert), (ca), (capath), (cipher))
#define mysql_stmt_affected_rows(s)		mysqlnd_stmt_affected_rows((s))
#define mysql_stmt_field_count(s)		mysqlnd_stmt_field_count((s))
#define mysql_stmt_param_count(s)		mysqlnd_stmt_param_count((s))
#define mysql_stmt_num_rows(s)			mysqlnd_stmt_num_rows((s))
#define mysql_stmt_insert_id(s)			mysqlnd_stmt_insert_id((s))
#define mysql_stmt_close(s)				mysqlnd_stmt_close((s))
#define mysql_stmt_bind_param(s,b)		mysqlnd_stmt_bind_param((s), (b))
#define mysql_stmt_bind_result(s,b)		mysqlnd_stmt_bind_result((s), (b))
#define mysql_stmt_errno(s)				mysqlnd_stmt_errno((s))
#define mysql_stmt_error(s)				mysqlnd_stmt_error((s))
#define mysql_stmt_sqlstate(s)			mysqlnd_stmt_sqlstate((s))
#define mysql_stmt_prepare(s,q,l)		mysqlnd_stmt_prepare((s), (q), (l))
#define mysql_stmt_execute(s)			mysqlnd_stmt_execute((s))
#define mysql_stmt_reset(s)				mysqlnd_stmt_reset((s))
#define mysql_stmt_store_result(s)		mysqlnd_stmt_store_result((s))
#define mysql_stmt_free_result(s)		mysqlnd_stmt_free_result((s))
#define mysql_stmt_data_seek(s,r)		mysqlnd_stmt_data_seek((s), (r))
#define mysql_stmt_send_long_data(s,p,d,l) mysqlnd_stmt_send_long_data((s), (p), (d), (l))
#define mysql_stmt_attr_get(s,a,v)		mysqlnd_stmt_attr_get((s), (a), (v))
#define mysql_stmt_attr_set(s,a,v)		mysqlnd_stmt_attr_set((s), (a), (v))
#define mysql_stmt_param_metadata(s)	mysqlnd_stmt_param_metadata((s))
#define mysql_stmt_result_metadata(s)	mysqlnd_stmt_result_metadata((s))
#define mysql_stmt_next_result(s)		mysqlnd_stmt_next_result((s))
#define mysql_stmt_more_results(s)		mysqlnd_stmt_more_results((s))
#define mysql_thread_safe()				mysqlnd_thread_safe()
#define mysql_info(r)					mysqlnd_info((r))
#define mysql_options(c,a,v)			mysqlnd_options((c), (a), (v))
#define mysql_options4(c,a,k,v)			mysqlnd_options4((c), (a), (k), (v))
#define mysql_stmt_init(r)				mysqlnd_stmt_init((r))
#define mysql_free_result(r)			mysqlnd_free_result((r), FALSE)
#define mysql_store_result(r)			mysqlnd_store_result((r))
#define mysql_use_result(r)				mysqlnd_use_result((r))
#define mysql_async_store_result(r)		mysqlnd_async_store_result((r))
#define mysql_thread_id(r)				mysqlnd_thread_id((r))
#define mysql_get_client_info()			mysqlnd_get_client_info()
#define mysql_get_client_version()		mysqlnd_get_client_version()
#define mysql_get_host_info(r)			mysqlnd_get_host_info((r))
#define mysql_get_proto_info(r)			mysqlnd_get_proto_info((r))
#define mysql_get_server_info(r)		mysqlnd_get_server_info((r))
#define mysql_get_server_version(r)		mysqlnd_get_server_version((r))
#define mysql_warning_count(r)			mysqlnd_warning_count((r))
#define mysql_eof(r)					(((r)->unbuf && (r)->unbuf->eof_reached) || (r)->stored_data)

#define REFRESH_GRANT		MYSQLND_REFRESH_GRANT
#define REFRESH_LOG			MYSQLND_REFRESH_LOG
#define REFRESH_TABLES		MYSQLND_REFRESH_TABLES
#define REFRESH_HOSTS		MYSQLND_REFRESH_HOSTS
#define REFRESH_STATUS		MYSQLND_REFRESH_STATUS
#define REFRESH_THREADS		MYSQLND_REFRESH_THREADS
#define REFRESH_SLAVE		MYSQLND_REFRESH_SLAVE
#define REFRESH_MASTER		MYSQLND_REFRESH_MASTER
#define REFRESH_BACKUP_LOG	MYSQLND_REFRESH_BACKUP_LOG

#endif /* MYSQLND_LIBMYSQL_COMPAT_H */
