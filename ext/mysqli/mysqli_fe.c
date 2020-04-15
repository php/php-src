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
  | Authors: Georg Richter <georg@php.net>                               |
  |          Andrey Hristov <andrey@php.net>                             |
  |          Ulf Wendel <uw@php.net>                                     |
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
#include "mysqli_fe.h"
#include "mysqli_priv.h"

#include "mysqli_arginfo.h"

#ifdef MYSQLI_USE_FULL_TYPED_ARGINFO_0
#define MYSQLI_ZEND_ARG_OBJ_INFO_LINK() ZEND_ARG_OBJ_INFO(0, link, mysqli, 0)
#define MYSQLI_ZEND_ARG_OBJ_INFO_RESULT() ZEND_ARG_OBJ_INFO(0, result, mysqli_result, 0)
#define MYSQLI_ZEND_ARG_OBJ_INFO_STMT() ZEND_ARG_OBJ_INFO(0, stmt, mysqli_stmt, 0)
#else
#define MYSQLI_ZEND_ARG_OBJ_INFO_LINK() ZEND_ARG_INFO(0, link)
#define MYSQLI_ZEND_ARG_OBJ_INFO_RESULT() ZEND_ARG_INFO(0, result)
#define MYSQLI_ZEND_ARG_OBJ_INFO_STMT() ZEND_ARG_INFO(0, stmt)
#endif

/* {{{ mysqli_functions[]
 *
 * Every user visible function must have an entry in mysqli_functions[].
 */
const zend_function_entry mysqli_functions[] = {
	PHP_FE(mysqli_affected_rows,						arginfo_mysqli_affected_rows)
	PHP_FE(mysqli_autocommit,							arginfo_mysqli_autocommit)
	PHP_FE(mysqli_begin_transaction,					arginfo_mysqli_begin_transaction)
	PHP_FE(mysqli_change_user,							arginfo_mysqli_change_user)
	PHP_FE(mysqli_character_set_name,					arginfo_mysqli_character_set_name)
	PHP_FE(mysqli_close,								arginfo_mysqli_close)
	PHP_FE(mysqli_commit,								arginfo_mysqli_commit)
	PHP_FE(mysqli_connect, 								arginfo_mysqli_connect)
	PHP_FE(mysqli_connect_errno,						arginfo_mysqli_connect_errno)
	PHP_FE(mysqli_connect_error,						arginfo_mysqli_connect_error)
	PHP_FE(mysqli_data_seek,							arginfo_mysqli_data_seek)
	PHP_FE(mysqli_dump_debug_info,						arginfo_mysqli_dump_debug_info)
	PHP_FE(mysqli_debug,								arginfo_mysqli_debug)
	PHP_FE(mysqli_errno,								arginfo_mysqli_errno)
	PHP_FE(mysqli_error,								arginfo_mysqli_error)
	PHP_FE(mysqli_error_list,							arginfo_mysqli_error_list)
	PHP_FE(mysqli_stmt_execute,							arginfo_mysqli_stmt_execute)
	PHP_FALIAS(mysqli_execute, mysqli_stmt_execute,		arginfo_mysqli_execute)
	PHP_FE(mysqli_fetch_field,							arginfo_mysqli_fetch_field)
	PHP_FE(mysqli_fetch_fields,							arginfo_mysqli_fetch_fields)
	PHP_FE(mysqli_fetch_field_direct,					arginfo_mysqli_fetch_field_direct)
	PHP_FE(mysqli_fetch_lengths,						arginfo_mysqli_fetch_lengths)
#ifdef MYSQLI_USE_MYSQLND
	PHP_FE(mysqli_fetch_all,							arginfo_mysqli_fetch_all)
#endif
	PHP_FE(mysqli_fetch_array,							arginfo_mysqli_fetch_array)
	PHP_FE(mysqli_fetch_assoc,							arginfo_mysqli_fetch_assoc)
	PHP_FE(mysqli_fetch_object,							arginfo_mysqli_fetch_object)
	PHP_FE(mysqli_fetch_row,							arginfo_mysqli_fetch_row)
	PHP_FE(mysqli_field_count,							arginfo_mysqli_field_count)
	PHP_FE(mysqli_field_seek,							arginfo_mysqli_field_seek)
	PHP_FE(mysqli_field_tell,							arginfo_mysqli_field_tell)
	PHP_FE(mysqli_free_result,							arginfo_mysqli_free_result)
#if defined(MYSQLI_USE_MYSQLND)
	PHP_FE(mysqli_get_connection_stats,					arginfo_mysqli_get_connection_stats)
	PHP_FE(mysqli_get_client_stats,						arginfo_mysqli_get_client_stats)
#endif
	PHP_FE(mysqli_get_charset,							arginfo_mysqli_get_charset)
	PHP_FE(mysqli_get_client_info,						arginfo_mysqli_get_client_info)
	PHP_FE(mysqli_get_client_version,					arginfo_mysqli_get_client_version)
	PHP_FE(mysqli_get_links_stats,						arginfo_mysqli_get_links_stats)
	PHP_FE(mysqli_get_host_info,						arginfo_mysqli_get_host_info)
	PHP_FE(mysqli_get_proto_info,						arginfo_mysqli_get_proto_info)
	PHP_FE(mysqli_get_server_info,						arginfo_mysqli_get_server_info)
	PHP_FE(mysqli_get_server_version,					arginfo_mysqli_get_server_version)
	PHP_FE(mysqli_get_warnings,							arginfo_mysqli_get_warnings)
	PHP_FE(mysqli_init, 								arginfo_mysqli_init)
	PHP_FE(mysqli_info,									arginfo_mysqli_info)
	PHP_FE(mysqli_insert_id,							arginfo_mysqli_insert_id)
	PHP_FE(mysqli_kill,									arginfo_mysqli_kill)
	PHP_FE(mysqli_more_results,							arginfo_mysqli_more_results)
	PHP_FE(mysqli_multi_query, 							arginfo_mysqli_multi_query)
	PHP_FE(mysqli_next_result,							arginfo_mysqli_next_result)
	PHP_FE(mysqli_num_fields,							arginfo_mysqli_num_fields)
	PHP_FE(mysqli_num_rows,								arginfo_mysqli_num_rows)
	PHP_FE(mysqli_options, 								arginfo_mysqli_options)
	PHP_FE(mysqli_ping,									arginfo_mysqli_ping)
#if defined(MYSQLI_USE_MYSQLND)
	PHP_FE(mysqli_poll,									arginfo_mysqli_poll)
#endif
	PHP_FE(mysqli_prepare,								arginfo_mysqli_prepare)
	PHP_FE(mysqli_report,								arginfo_mysqli_report)
	PHP_FE(mysqli_query,								arginfo_mysqli_query)
	PHP_FE(mysqli_real_connect,							arginfo_mysqli_real_connect)
	PHP_FE(mysqli_real_escape_string,					arginfo_mysqli_real_escape_string)
	PHP_FE(mysqli_real_query,							arginfo_mysqli_real_query)
#if defined(MYSQLI_USE_MYSQLND)
	PHP_FE(mysqli_reap_async_query,						arginfo_mysqli_reap_async_query)
#endif
	PHP_FE(mysqli_release_savepoint,					arginfo_mysqli_release_savepoint)
	PHP_FE(mysqli_rollback,								arginfo_mysqli_rollback)
	PHP_FE(mysqli_savepoint,							arginfo_mysqli_savepoint)
	PHP_FE(mysqli_select_db,							arginfo_mysqli_select_db)
	PHP_FE(mysqli_set_charset,							arginfo_mysqli_set_charset)
	PHP_FE(mysqli_stmt_affected_rows,					arginfo_mysqli_stmt_affected_rows)
	PHP_FE(mysqli_stmt_attr_get,						arginfo_mysqli_stmt_attr_get)
	PHP_FE(mysqli_stmt_attr_set,						arginfo_mysqli_stmt_attr_set)
	PHP_FE(mysqli_stmt_bind_param,						arginfo_mysqli_stmt_bind_param)
	PHP_FE(mysqli_stmt_bind_result,						arginfo_mysqli_stmt_bind_result)
	PHP_FE(mysqli_stmt_close,							arginfo_mysqli_stmt_close)
	PHP_FE(mysqli_stmt_data_seek,						arginfo_mysqli_stmt_data_seek)
	PHP_FE(mysqli_stmt_errno,							arginfo_mysqli_stmt_errno)
	PHP_FE(mysqli_stmt_error,							arginfo_mysqli_stmt_error)
	PHP_FE(mysqli_stmt_error_list,						arginfo_mysqli_stmt_error_list)
	PHP_FE(mysqli_stmt_fetch,							arginfo_mysqli_stmt_fetch)
	PHP_FE(mysqli_stmt_field_count,						arginfo_mysqli_stmt_field_count)
	PHP_FE(mysqli_stmt_free_result,						arginfo_mysqli_stmt_free_result)
#if defined(MYSQLI_USE_MYSQLND)
	PHP_FE(mysqli_stmt_get_result,						arginfo_mysqli_stmt_get_result)
#endif
	PHP_FE(mysqli_stmt_get_warnings,					arginfo_mysqli_stmt_get_warnings)
	PHP_FE(mysqli_stmt_init,							arginfo_mysqli_stmt_init)
	PHP_FE(mysqli_stmt_insert_id,						arginfo_mysqli_stmt_insert_id)
#if defined(MYSQLI_USE_MYSQLND)
	PHP_FE(mysqli_stmt_more_results,					arginfo_mysqli_stmt_more_results)
	PHP_FE(mysqli_stmt_next_result,						arginfo_mysqli_stmt_next_result)
#endif
	PHP_FE(mysqli_stmt_num_rows,						arginfo_mysqli_stmt_num_rows)
	PHP_FE(mysqli_stmt_param_count,						arginfo_mysqli_stmt_param_count)
	PHP_FE(mysqli_stmt_prepare,							arginfo_mysqli_stmt_prepare)
	PHP_FE(mysqli_stmt_reset,							arginfo_mysqli_stmt_reset)
	PHP_FE(mysqli_stmt_result_metadata,					arginfo_mysqli_stmt_result_metadata)
	PHP_FE(mysqli_stmt_send_long_data,					arginfo_mysqli_stmt_send_long_data)
	PHP_FE(mysqli_stmt_store_result,					arginfo_mysqli_stmt_store_result)
	PHP_FE(mysqli_stmt_sqlstate,   						arginfo_mysqli_stmt_sqlstate)
	PHP_FE(mysqli_sqlstate,   							arginfo_mysqli_sqlstate)
	PHP_FE(mysqli_ssl_set,								arginfo_mysqli_ssl_set)
	PHP_FE(mysqli_stat,									arginfo_mysqli_stat)
	PHP_FE(mysqli_store_result,							arginfo_mysqli_store_result)
	PHP_FE(mysqli_thread_id,							arginfo_mysqli_thread_id)
	PHP_FE(mysqli_thread_safe,							arginfo_mysqli_thread_safe)
	PHP_FE(mysqli_use_result,							arginfo_mysqli_use_result)
	PHP_FE(mysqli_warning_count,						arginfo_mysqli_warning_count)

	PHP_FE(mysqli_refresh,								arginfo_mysqli_refresh)

	/* Aliases */
	PHP_FALIAS(mysqli_escape_string,	mysqli_real_escape_string,	arginfo_mysqli_escape_string)
	PHP_FALIAS(mysqli_set_opt,			mysqli_options,				arginfo_mysqli_set_opt)

	PHP_FE_END
};
/* }}} */

/* {{{ mysqli_link_methods[]
 *
 * Every user visible function must have an entry in mysqli_link_methods[].
 */
const zend_function_entry mysqli_link_methods[] = {
	PHP_ME_MAPPING(autocommit, mysqli_autocommit, arginfo_class_mysqli_autocommit, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(begin_transaction, mysqli_begin_transaction, arginfo_class_mysqli_begin_transaction, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(change_user, mysqli_change_user, arginfo_class_mysqli_change_user, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(character_set_name, mysqli_character_set_name, arginfo_class_mysqli_character_set_name, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(close, mysqli_close, arginfo_class_mysqli_close, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(commit, mysqli_commit, arginfo_class_mysqli_commit, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(connect, mysqli_connect, arginfo_class_mysqli_connect, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(dump_debug_info, mysqli_dump_debug_info, arginfo_class_mysqli_dump_debug_info, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(debug, mysqli_debug, arginfo_class_mysqli_debug, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(get_charset, mysqli_get_charset, arginfo_class_mysqli_get_charset, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(get_client_info, mysqli_get_client_info, arginfo_class_mysqli_get_client_info, ZEND_ACC_PUBLIC)
#if defined(MYSQLI_USE_MYSQLND)
	PHP_ME_MAPPING(get_connection_stats, mysqli_get_connection_stats, arginfo_class_mysqli_get_connection_stats, ZEND_ACC_PUBLIC)
#endif
	PHP_ME_MAPPING(get_server_info, mysqli_get_server_info, arginfo_class_mysqli_get_server_info, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(get_warnings, mysqli_get_warnings, arginfo_class_mysqli_get_warnings, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(init, mysqli_init_method, arginfo_class_mysqli_init, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(kill, mysqli_kill, arginfo_class_mysqli_kill, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(multi_query, mysqli_multi_query, arginfo_class_mysqli_multi_query, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(__construct, mysqli_link_construct, arginfo_class_mysqli___construct, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(more_results, mysqli_more_results, arginfo_class_mysqli_more_results, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(next_result, mysqli_next_result, arginfo_class_mysqli_next_result, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(options, mysqli_options, arginfo_class_mysqli_options, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(ping, mysqli_ping, arginfo_class_mysqli_ping, ZEND_ACC_PUBLIC)
#if defined(MYSQLI_USE_MYSQLND)
	PHP_ME_MAPPING(poll, mysqli_poll, arginfo_class_mysqli_poll, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
#endif
	PHP_ME_MAPPING(prepare, mysqli_prepare, arginfo_class_mysqli_prepare, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(query, mysqli_query, arginfo_class_mysqli_query, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(real_connect, mysqli_real_connect, arginfo_class_mysqli_real_connect, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(real_escape_string, mysqli_real_escape_string, arginfo_class_mysqli_real_escape_string, ZEND_ACC_PUBLIC)
#if defined(MYSQLI_USE_MYSQLND)
	PHP_ME_MAPPING(reap_async_query, mysqli_reap_async_query, arginfo_class_mysqli_reap_async_query, ZEND_ACC_PUBLIC)
#endif
	PHP_ME_MAPPING(escape_string, mysqli_real_escape_string, arginfo_class_mysqli_real_escape_string, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(real_query, mysqli_real_query, arginfo_class_mysqli_real_query, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(release_savepoint, mysqli_release_savepoint, arginfo_class_mysqli_release_savepoint, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(rollback, mysqli_rollback, arginfo_class_mysqli_rollback, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(savepoint, mysqli_savepoint, arginfo_class_mysqli_savepoint, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(select_db, mysqli_select_db, arginfo_class_mysqli_select_db, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(set_charset, mysqli_set_charset, arginfo_class_mysqli_set_charset, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(set_opt, mysqli_options, arginfo_class_mysqli_set_opt, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(ssl_set, mysqli_ssl_set, arginfo_class_mysqli_ssl_set, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(stat, mysqli_stat, arginfo_class_mysqli_stat, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(stmt_init, mysqli_stmt_init, arginfo_class_mysqli_stmt_init, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(store_result, mysqli_store_result, arginfo_class_mysqli_store_result, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(thread_safe, mysqli_thread_safe, arginfo_class_mysqli_thread_safe, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(use_result, mysqli_use_result, arginfo_class_mysqli_use_result, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(refresh, mysqli_refresh, arginfo_class_mysqli_refresh, ZEND_ACC_PUBLIC)
	PHP_FE_END
};
/* }}} */

/* {{{ mysqli_result_methods[]
 *
 * Every user visible function must have an entry in mysqli_result_methods[].
 */
const zend_function_entry mysqli_result_methods[] = {
	PHP_ME_MAPPING(__construct, mysqli_result_construct, arginfo_class_mysqli_result___construct, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(close, mysqli_free_result, arginfo_class_mysqli_result_close, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(free, mysqli_free_result, arginfo_class_mysqli_result_free, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(data_seek, mysqli_data_seek, arginfo_class_mysqli_result_data_seek, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(fetch_field, mysqli_fetch_field, arginfo_class_mysqli_result_fetch_field, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(fetch_fields, mysqli_fetch_fields, arginfo_class_mysqli_result_fetch_fields, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(fetch_field_direct, mysqli_fetch_field_direct, arginfo_class_mysqli_result_fetch_field_direct, ZEND_ACC_PUBLIC)
#if defined(MYSQLI_USE_MYSQLND)
	PHP_ME_MAPPING(fetch_all, mysqli_fetch_all, arginfo_class_mysqli_result_fetch_all, ZEND_ACC_PUBLIC)
#endif
	PHP_ME_MAPPING(fetch_array, mysqli_fetch_array, arginfo_class_mysqli_result_fetch_array, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(fetch_assoc, mysqli_fetch_assoc, arginfo_class_mysqli_result_fetch_assoc, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(fetch_object, mysqli_fetch_object, arginfo_class_mysqli_result_fetch_object, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(fetch_row, mysqli_fetch_row, arginfo_class_mysqli_result_fetch_row, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(field_seek, mysqli_field_seek, arginfo_class_mysqli_result_field_seek, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(free_result, mysqli_free_result, arginfo_class_mysqli_result_free_result, ZEND_ACC_PUBLIC)
	PHP_FE_END
};
/* }}} */

/* {{{ mysqli_stmt_methods[]
 *
 * Every user visible function must have an entry in mysqli_stmt_methods[].
 */
const zend_function_entry mysqli_stmt_methods[] = {
	PHP_ME_MAPPING(__construct, mysqli_stmt_construct, arginfo_class_mysqli_stmt___construct, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(attr_get, mysqli_stmt_attr_get, arginfo_class_mysqli_stmt_attr_get, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(attr_set, mysqli_stmt_attr_set, arginfo_class_mysqli_stmt_attr_set, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(bind_param, mysqli_stmt_bind_param, arginfo_class_mysqli_stmt_bind_param, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(bind_result, mysqli_stmt_bind_result, arginfo_class_mysqli_stmt_bind_result, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(close, mysqli_stmt_close, arginfo_class_mysqli_stmt_close, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(data_seek, mysqli_stmt_data_seek, arginfo_class_mysqli_stmt_data_seek, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(execute, mysqli_stmt_execute, arginfo_class_mysqli_stmt_execute, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(fetch, mysqli_stmt_fetch, arginfo_class_mysqli_stmt_fetch, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(get_warnings, mysqli_stmt_get_warnings, arginfo_class_mysqli_stmt_get_warnings, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(result_metadata, mysqli_stmt_result_metadata, arginfo_class_mysqli_stmt_result_metadata, ZEND_ACC_PUBLIC)
#if defined(MYSQLI_USE_MYSQLND)
	PHP_ME_MAPPING(more_results, mysqli_stmt_more_results, arginfo_class_mysqli_stmt_more_results, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(next_result, mysqli_stmt_next_result, arginfo_class_mysqli_stmt_next_result, ZEND_ACC_PUBLIC)
#endif
	PHP_ME_MAPPING(num_rows, mysqli_stmt_num_rows, arginfo_class_mysqli_stmt_num_rows, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(send_long_data, mysqli_stmt_send_long_data, arginfo_class_mysqli_stmt_send_long_data, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(free_result, mysqli_stmt_free_result, arginfo_class_mysqli_stmt_free_result, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(reset, mysqli_stmt_reset, arginfo_class_mysqli_stmt_reset, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(prepare, mysqli_stmt_prepare, arginfo_class_mysqli_stmt_prepare, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(store_result, mysqli_stmt_store_result, arginfo_class_mysqli_stmt_store_result, ZEND_ACC_PUBLIC)
#if defined(MYSQLI_USE_MYSQLND)
	PHP_ME_MAPPING(get_result, mysqli_stmt_get_result, arginfo_class_mysqli_stmt_get_result, ZEND_ACC_PUBLIC)
#endif
	PHP_FE_END
};
/* }}} */
