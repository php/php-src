/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
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
#include "mysqli_fe.h"
#include "mysqli_priv.h"

#ifdef MYSQLI_USE_FULL_TYPED_ARGINFO_0
#define MYSQLI_ZEND_ARG_OBJ_INFO_LINK() ZEND_ARG_OBJ_INFO(0, link, mysqli, 0)
#define MYSQLI_ZEND_ARG_OBJ_INFO_RESULT() ZEND_ARG_OBJ_INFO(0, result, mysqli_result, 0)
#define MYSQLI_ZEND_ARG_OBJ_INFO_STMT() ZEND_ARG_OBJ_INFO(0, stmt, mysqli_stmt, 0)
#else
#define MYSQLI_ZEND_ARG_OBJ_INFO_LINK() ZEND_ARG_INFO(0, link)
#define MYSQLI_ZEND_ARG_OBJ_INFO_RESULT() ZEND_ARG_INFO(0, result)
#define MYSQLI_ZEND_ARG_OBJ_INFO_STMT() ZEND_ARG_INFO(0, stmt)
#endif

ZEND_BEGIN_ARG_INFO(arginfo_mysqli_stmt_bind_result, 0)
	MYSQLI_ZEND_ARG_OBJ_INFO_STMT()
	ZEND_ARG_VARIADIC_INFO(1, vars)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_mysqli_stmt_bind_param, 0)
	MYSQLI_ZEND_ARG_OBJ_INFO_STMT()
	ZEND_ARG_INFO(0, types)
	ZEND_ARG_VARIADIC_INFO(1, vars)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_class_mysqli_stmt_bind_result, 0)
	ZEND_ARG_VARIADIC_INFO(1, vars)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_class_mysqli_stmt_bind_param, 0)
	ZEND_ARG_INFO(0, types)
	ZEND_ARG_VARIADIC_INFO(1, vars)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(all_args_force_by_ref, 0)
	ZEND_ARG_VARIADIC_INFO(1, vars)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_poll, 0, 0, 4)
	ZEND_ARG_ARRAY_INFO(1, read, 1)
	ZEND_ARG_ARRAY_INFO(1, write, 1)
	ZEND_ARG_ARRAY_INFO(1, error, 1)
	ZEND_ARG_INFO(0, sec)
	ZEND_ARG_INFO(0, usec)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_no_params, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_only_link, 0, 0, 1)
	MYSQLI_ZEND_ARG_OBJ_INFO_LINK()
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_autocommit, 0, 0, 2)
	MYSQLI_ZEND_ARG_OBJ_INFO_LINK()
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_autocommit, 0, 0, 1)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_begin_transaction, 0, 0, 1)
	MYSQLI_ZEND_ARG_OBJ_INFO_LINK()
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_begin_transaction, 0, 0, 0)
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_savepoint, 0, 0, 2)
	MYSQLI_ZEND_ARG_OBJ_INFO_LINK()
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_savepoint, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_release_savepoint, 0, 0, 2)
	MYSQLI_ZEND_ARG_OBJ_INFO_LINK()
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_release_savepoint, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_commit, 0, 0, 1)
	MYSQLI_ZEND_ARG_OBJ_INFO_LINK()
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_commit, 0, 0, 0)
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_rollback, 0, 0, 1)
	MYSQLI_ZEND_ARG_OBJ_INFO_LINK()
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_rollback, 0, 0, 0)
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_store_result, 0, 0, 1)
	MYSQLI_ZEND_ARG_OBJ_INFO_LINK()
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_store_result, 0, 0, 0)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_change_user, 0, 0, 4)
	MYSQLI_ZEND_ARG_OBJ_INFO_LINK()
	ZEND_ARG_INFO(0, user)
	ZEND_ARG_INFO(0, password)
	ZEND_ARG_INFO(0, database)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_change_user, 0, 0, 3)
	ZEND_ARG_INFO(0, user)
	ZEND_ARG_INFO(0, password)
	ZEND_ARG_INFO(0, database)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_connect, 0, 0, 0)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, user)
	ZEND_ARG_INFO(0, password)
	ZEND_ARG_INFO(0, database)
	ZEND_ARG_INFO(0, port)
	ZEND_ARG_INFO(0, socket)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_real_connect, 0, 0, 1)
	MYSQLI_ZEND_ARG_OBJ_INFO_LINK()
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, user)
	ZEND_ARG_INFO(0, password)
	ZEND_ARG_INFO(0, database)
	ZEND_ARG_INFO(0, port)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_real_connect, 0, 0, 0)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, user)
	ZEND_ARG_INFO(0, password)
	ZEND_ARG_INFO(0, database)
	ZEND_ARG_INFO(0, port)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_only_result, 0, 0, 1)
	MYSQLI_ZEND_ARG_OBJ_INFO_RESULT()
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_only_statement, 0, 0, 1)
	MYSQLI_ZEND_ARG_OBJ_INFO_STMT()
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_data_seek, 0, 0, 2)
	MYSQLI_ZEND_ARG_OBJ_INFO_RESULT()
	ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_stmt_data_seek, 0, 0, 2)
	MYSQLI_ZEND_ARG_OBJ_INFO_STMT()
	ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_data_seek, 0, 0, 1)
	ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_debug, 0, 0, 1)
	ZEND_ARG_INFO(0, debug_options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_result_and_fieldnr, 0, 0, 2)
	MYSQLI_ZEND_ARG_OBJ_INFO_RESULT()
	ZEND_ARG_INFO(0, field_nr)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_result_and_fieldnr, 0, 0, 1)
	ZEND_ARG_INFO(0, field_nr)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_fetch_array, 0, 0, 1)
	MYSQLI_ZEND_ARG_OBJ_INFO_RESULT()
	ZEND_ARG_INFO(0, result_type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_fetch_array, 0, 0, 0)
	ZEND_ARG_INFO(0, result_type)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_fetch_object, 0, 0, 1)
#if PHP_VERSION_ID > 50399
	MYSQLI_ZEND_ARG_OBJ_INFO_RESULT()
	ZEND_ARG_INFO(0, class_name)
	ZEND_ARG_ARRAY_INFO(0, params, 0)
#endif
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_fetch_object, 0, 0, 0)
#if PHP_VERSION_ID > 50399
	ZEND_ARG_INFO(0, class_name)
	ZEND_ARG_ARRAY_INFO(0, params, 0)
#endif
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_kill, 0, 0, 2)
	MYSQLI_ZEND_ARG_OBJ_INFO_LINK()
	ZEND_ARG_INFO(0, connection_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_kill, 0, 0, 1)
	ZEND_ARG_INFO(0, connection_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_query, 0, 0, 2)
	MYSQLI_ZEND_ARG_OBJ_INFO_LINK()
	ZEND_ARG_INFO(0, query)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_prepare, 0, 0, 2)
	MYSQLI_ZEND_ARG_OBJ_INFO_LINK()
	ZEND_ARG_INFO(0, query)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_stmt_prepare, 0, 0, 2)
	MYSQLI_ZEND_ARG_OBJ_INFO_STMT()
	ZEND_ARG_INFO(0, query)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_query, 0, 0, 1)
	ZEND_ARG_INFO(0, query)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_options, 0, 0, 3)
	MYSQLI_ZEND_ARG_OBJ_INFO_LINK()
	ZEND_ARG_INFO(0, option)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_options, 0, 0, 2)
	ZEND_ARG_INFO(0, option)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_report, 0, 0, 1)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_real_escape_string, 0, 0, 2)
	MYSQLI_ZEND_ARG_OBJ_INFO_LINK()
	ZEND_ARG_INFO(0, string_to_escape)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_real_escape_string, 0, 0, 1)
	ZEND_ARG_INFO(0, string_to_escape)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_select_db, 0, 0, 2)
	MYSQLI_ZEND_ARG_OBJ_INFO_LINK()
	ZEND_ARG_INFO(0, database)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_select_db, 0, 0, 1)
	ZEND_ARG_INFO(0, database)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_set_charset, 0, 0, 2)
	MYSQLI_ZEND_ARG_OBJ_INFO_LINK()
	ZEND_ARG_INFO(0, charset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_set_charset, 0, 0, 1)
	ZEND_ARG_INFO(0, charset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_stmt_attr_get, 0, 0, 2)
	MYSQLI_ZEND_ARG_OBJ_INFO_STMT()
	ZEND_ARG_INFO(0, attribute)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_stmt_attr_get, 0, 0, 1)
	ZEND_ARG_INFO(0, attribute)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_stmt_attr_set, 0, 0, 3)
	MYSQLI_ZEND_ARG_OBJ_INFO_STMT()
	ZEND_ARG_INFO(0, attribute)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_stmt_attr_set, 0, 0, 2)
	ZEND_ARG_INFO(0, attribute)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_stmt_send_long_data, 0, 0, 3)
	MYSQLI_ZEND_ARG_OBJ_INFO_STMT()
	ZEND_ARG_INFO(0, param_nr)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_stmt_send_long_data, 0, 0, 2)
	ZEND_ARG_INFO(0, param_nr)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_ssl_set, 0, 0, 6)
	MYSQLI_ZEND_ARG_OBJ_INFO_LINK()
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, cert)
	ZEND_ARG_INFO(0, certificate_authority)
	ZEND_ARG_INFO(0, certificate_authority_path)
	ZEND_ARG_INFO(0, cipher)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_ssl_set, 0, 0, 5)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, cert)
	ZEND_ARG_INFO(0, certificate_authority)
	ZEND_ARG_INFO(0, certificate_authority_path)
	ZEND_ARG_INFO(0, cipher)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_refresh, 0, 0, 2)
	MYSQLI_ZEND_ARG_OBJ_INFO_LINK()
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_refresh, 0, 0, 1)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mysqli_no_options, 0, 0, 0)
ZEND_END_ARG_INFO()


/* {{{ mysqli_functions[]
 *
 * Every user visible function must have an entry in mysqli_functions[].
 */
const zend_function_entry mysqli_functions[] = {
	PHP_FE(mysqli_affected_rows,						arginfo_mysqli_only_link)
	PHP_FE(mysqli_autocommit,							arginfo_mysqli_autocommit)
	PHP_FE(mysqli_begin_transaction,					arginfo_mysqli_begin_transaction)
	PHP_FE(mysqli_change_user,							arginfo_mysqli_change_user)
	PHP_FE(mysqli_character_set_name,					arginfo_mysqli_only_link)
	PHP_FE(mysqli_close,								arginfo_mysqli_only_link)
	PHP_FE(mysqli_commit,								arginfo_mysqli_commit)
	PHP_FE(mysqli_connect, 								arginfo_mysqli_connect)
	PHP_FE(mysqli_connect_errno,						arginfo_mysqli_no_params)
	PHP_FE(mysqli_connect_error,						arginfo_mysqli_no_params)
	PHP_FE(mysqli_data_seek,							arginfo_mysqli_data_seek)
	PHP_FE(mysqli_dump_debug_info,						arginfo_mysqli_only_link)
	PHP_FE(mysqli_debug,								arginfo_mysqli_debug)
#if defined(HAVE_EMBEDDED_MYSQLI)
	PHP_FE(mysqli_embedded_server_end,					NULL)
	PHP_FE(mysqli_embedded_server_start,				NULL)
#endif
	PHP_FE(mysqli_errno,								arginfo_mysqli_only_link)
	PHP_FE(mysqli_error,								arginfo_mysqli_only_link)
	PHP_FE(mysqli_error_list,							arginfo_mysqli_only_link)
	PHP_FE(mysqli_stmt_execute,							arginfo_mysqli_only_statement)
	PHP_FALIAS(mysqli_execute, mysqli_stmt_execute,		arginfo_mysqli_only_statement)
	PHP_FE(mysqli_fetch_field,							arginfo_mysqli_only_result)
	PHP_FE(mysqli_fetch_fields,							arginfo_mysqli_only_result)
	PHP_FE(mysqli_fetch_field_direct,					arginfo_mysqli_result_and_fieldnr)
	PHP_FE(mysqli_fetch_lengths,						arginfo_mysqli_only_result)
#ifdef MYSQLI_USE_MYSQLND
	PHP_FE(mysqli_fetch_all,							arginfo_mysqli_only_result)
#endif
	PHP_FE(mysqli_fetch_array,							arginfo_mysqli_fetch_array)
	PHP_FE(mysqli_fetch_assoc,							arginfo_mysqli_only_result)
	PHP_FE(mysqli_fetch_object,							arginfo_mysqli_fetch_object)
	PHP_FE(mysqli_fetch_row,							arginfo_mysqli_only_result)
	PHP_FE(mysqli_field_count,							arginfo_mysqli_only_link)
	PHP_FE(mysqli_field_seek,							arginfo_mysqli_result_and_fieldnr)
	PHP_FE(mysqli_field_tell,							arginfo_mysqli_only_result)
	PHP_FE(mysqli_free_result,							arginfo_mysqli_only_result)
#if defined(MYSQLI_USE_MYSQLND)
	PHP_FE(mysqli_get_connection_stats,					arginfo_mysqli_only_link)
	PHP_FE(mysqli_get_client_stats,						arginfo_mysqli_no_params)
#endif
#ifdef HAVE_MYSQLI_GET_CHARSET
	PHP_FE(mysqli_get_charset,							arginfo_mysqli_only_link)
#endif
	PHP_FE(mysqli_get_client_info,						arginfo_mysqli_only_link)
	PHP_FE(mysqli_get_client_version,					arginfo_mysqli_only_link)
	PHP_FE(mysqli_get_links_stats,						arginfo_mysqli_no_options)
	PHP_FE(mysqli_get_host_info,						arginfo_mysqli_only_link)
	PHP_FE(mysqli_get_proto_info,						arginfo_mysqli_only_link)
	PHP_FE(mysqli_get_server_info,						arginfo_mysqli_only_link)
	PHP_FE(mysqli_get_server_version,					arginfo_mysqli_only_link)
	PHP_FE(mysqli_get_warnings,							arginfo_mysqli_only_link)
	PHP_FE(mysqli_init, 								arginfo_mysqli_no_params)
	PHP_FE(mysqli_info,									arginfo_mysqli_only_link)
	PHP_FE(mysqli_insert_id,							arginfo_mysqli_only_link)
	PHP_FE(mysqli_kill,									arginfo_mysqli_kill)
	PHP_FE(mysqli_more_results,							arginfo_mysqli_only_link)
	PHP_FE(mysqli_multi_query, 							arginfo_mysqli_query)
	PHP_FE(mysqli_next_result,							arginfo_mysqli_only_link)
	PHP_FE(mysqli_num_fields,							arginfo_mysqli_only_result)
	PHP_FE(mysqli_num_rows,								arginfo_mysqli_only_result)
	PHP_FE(mysqli_options, 								arginfo_mysqli_options)
	PHP_FE(mysqli_ping,									arginfo_mysqli_only_link)
#if defined(MYSQLI_USE_MYSQLND)
	PHP_FE(mysqli_poll,									arginfo_mysqli_poll)
#endif
	PHP_FE(mysqli_prepare,								arginfo_mysqli_prepare)
	PHP_FE(mysqli_report,								arginfo_mysqli_report)
	PHP_FE(mysqli_query,								arginfo_mysqli_query)
	PHP_FE(mysqli_real_connect,							arginfo_mysqli_real_connect)
	PHP_FE(mysqli_real_escape_string,					arginfo_mysqli_real_escape_string)
	PHP_FE(mysqli_real_query,							arginfo_mysqli_query)
#if defined(MYSQLI_USE_MYSQLND)
	PHP_FE(mysqli_reap_async_query,						arginfo_mysqli_only_link)
#endif
	PHP_FE(mysqli_release_savepoint,					arginfo_mysqli_release_savepoint)
	PHP_FE(mysqli_rollback,								arginfo_mysqli_rollback)
	PHP_FE(mysqli_savepoint,							arginfo_mysqli_savepoint)
	PHP_FE(mysqli_select_db,							arginfo_mysqli_select_db)
#ifdef HAVE_MYSQLI_SET_CHARSET
	PHP_FE(mysqli_set_charset,							arginfo_mysqli_set_charset)
#endif
	PHP_FE(mysqli_stmt_affected_rows,					arginfo_mysqli_only_statement)
	PHP_FE(mysqli_stmt_attr_get,						arginfo_mysqli_stmt_attr_get)
	PHP_FE(mysqli_stmt_attr_set,						arginfo_mysqli_stmt_attr_set)
	PHP_FE(mysqli_stmt_bind_param,						arginfo_mysqli_stmt_bind_param)
	PHP_FE(mysqli_stmt_bind_result,						arginfo_mysqli_stmt_bind_result)
	PHP_FE(mysqli_stmt_close,							arginfo_mysqli_only_statement)
	PHP_FE(mysqli_stmt_data_seek,						arginfo_mysqli_stmt_data_seek)
	PHP_FE(mysqli_stmt_errno,							arginfo_mysqli_only_statement)
	PHP_FE(mysqli_stmt_error,							arginfo_mysqli_only_statement)
	PHP_FE(mysqli_stmt_error_list,						arginfo_mysqli_only_statement)
	PHP_FE(mysqli_stmt_fetch,							arginfo_mysqli_only_statement)
	PHP_FE(mysqli_stmt_field_count,						arginfo_mysqli_only_statement)
	PHP_FE(mysqli_stmt_free_result,						arginfo_mysqli_only_statement)
#if defined(MYSQLI_USE_MYSQLND)
	PHP_FE(mysqli_stmt_get_result,						arginfo_mysqli_only_statement)
#endif
	PHP_FE(mysqli_stmt_get_warnings,					arginfo_mysqli_only_statement)
	PHP_FE(mysqli_stmt_init,							arginfo_mysqli_only_link)
	PHP_FE(mysqli_stmt_insert_id,						arginfo_mysqli_only_statement)
#if defined(MYSQLI_USE_MYSQLND)
	PHP_FE(mysqli_stmt_more_results,					arginfo_mysqli_only_statement)
	PHP_FE(mysqli_stmt_next_result,						arginfo_mysqli_only_statement)
#endif
	PHP_FE(mysqli_stmt_num_rows,						arginfo_mysqli_only_statement)
	PHP_FE(mysqli_stmt_param_count,						arginfo_mysqli_only_statement)
	PHP_FE(mysqli_stmt_prepare,							arginfo_mysqli_stmt_prepare)
	PHP_FE(mysqli_stmt_reset,							arginfo_mysqli_only_statement)
	PHP_FE(mysqli_stmt_result_metadata,					arginfo_mysqli_only_statement)
	PHP_FE(mysqli_stmt_send_long_data,					arginfo_mysqli_stmt_send_long_data)
	PHP_FE(mysqli_stmt_store_result,					arginfo_mysqli_only_statement)
	PHP_FE(mysqli_stmt_sqlstate,   						arginfo_mysqli_only_statement)
	PHP_FE(mysqli_sqlstate,   							arginfo_mysqli_only_link)
	PHP_FE(mysqli_ssl_set,								arginfo_mysqli_ssl_set)
	PHP_FE(mysqli_stat,									arginfo_mysqli_only_link)
	PHP_FE(mysqli_store_result,							arginfo_mysqli_store_result)
	PHP_FE(mysqli_thread_id,							arginfo_mysqli_only_link)
	PHP_FE(mysqli_thread_safe,							arginfo_mysqli_no_params)
	PHP_FE(mysqli_use_result,							arginfo_mysqli_only_link)
	PHP_FE(mysqli_warning_count,						arginfo_mysqli_only_link)

	PHP_FE(mysqli_refresh,								arginfo_mysqli_refresh)

	/* Aliases */
	PHP_FALIAS(mysqli_escape_string,	mysqli_real_escape_string,	arginfo_mysqli_query)
	PHP_FALIAS(mysqli_set_opt,			mysqli_options,				NULL)

	PHP_FE_END
};
/* }}} */

/* {{{ mysqli_link_methods[]
 *
 * Every user visible function must have an entry in mysqli_functions[].
 */
const zend_function_entry mysqli_link_methods[] = {
	PHP_FALIAS(autocommit, mysqli_autocommit, arginfo_class_mysqli_autocommit)
	PHP_FALIAS(begin_transaction, mysqli_begin_transaction, arginfo_class_mysqli_begin_transaction)
	PHP_FALIAS(change_user,mysqli_change_user, arginfo_class_mysqli_change_user)
	PHP_FALIAS(character_set_name, mysqli_character_set_name, arginfo_mysqli_no_params)
	PHP_FALIAS(close, mysqli_close, arginfo_mysqli_no_params)
	PHP_FALIAS(commit, mysqli_commit, arginfo_class_mysqli_commit)
	PHP_FALIAS(connect, mysqli_connect, arginfo_mysqli_connect)
	PHP_FALIAS(dump_debug_info, mysqli_dump_debug_info, arginfo_mysqli_no_params)
	PHP_FALIAS(debug, mysqli_debug, arginfo_mysqli_debug)
#ifdef HAVE_MYSQLI_GET_CHARSET
	PHP_FALIAS(get_charset, mysqli_get_charset, arginfo_mysqli_no_params)
#endif
	PHP_FALIAS(get_client_info, mysqli_get_client_info, arginfo_mysqli_no_params)
#if defined(MYSQLI_USE_MYSQLND)
	PHP_FALIAS(get_connection_stats, mysqli_get_connection_stats, arginfo_mysqli_no_params)
#endif
	PHP_FALIAS(get_server_info, mysqli_get_server_info, arginfo_mysqli_no_params)
	PHP_FALIAS(get_warnings, mysqli_get_warnings, arginfo_mysqli_no_params)
	PHP_FALIAS(init,mysqli_init, arginfo_mysqli_no_params)
	PHP_FALIAS(kill,mysqli_kill, arginfo_class_mysqli_kill)
	PHP_FALIAS(multi_query, mysqli_multi_query, arginfo_class_mysqli_query)
	PHP_FALIAS(mysqli, mysqli_link_construct, arginfo_mysqli_connect)
	PHP_FALIAS(more_results, mysqli_more_results, arginfo_mysqli_no_params)
	PHP_FALIAS(next_result, mysqli_next_result, arginfo_mysqli_no_params)
	PHP_FALIAS(options, mysqli_options, arginfo_class_mysqli_options)
	PHP_FALIAS(ping, mysqli_ping, arginfo_mysqli_no_params)
#if defined(MYSQLI_USE_MYSQLND)
	ZEND_FENTRY(poll, ZEND_FN(mysqli_poll), arginfo_mysqli_poll, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
#endif
	PHP_FALIAS(prepare, mysqli_prepare, arginfo_class_mysqli_query)
	PHP_FALIAS(query, mysqli_query, arginfo_class_mysqli_query)
	PHP_FALIAS(real_connect, mysqli_real_connect, arginfo_class_mysqli_real_connect)
	PHP_FALIAS(real_escape_string, mysqli_real_escape_string, arginfo_class_mysqli_real_escape_string)
#if defined(MYSQLI_USE_MYSQLND)
	PHP_FALIAS(reap_async_query, mysqli_reap_async_query, arginfo_mysqli_no_params)
#endif
	PHP_FALIAS(escape_string, mysqli_real_escape_string, arginfo_class_mysqli_real_escape_string)
	PHP_FALIAS(real_query, mysqli_real_query, arginfo_class_mysqli_query)
	PHP_FALIAS(release_savepoint, mysqli_release_savepoint, arginfo_class_mysqli_release_savepoint)
	PHP_FALIAS(rollback, mysqli_rollback, arginfo_class_mysqli_rollback)
	PHP_FALIAS(savepoint, mysqli_savepoint, arginfo_class_mysqli_savepoint)
	PHP_FALIAS(select_db,mysqli_select_db, arginfo_class_mysqli_select_db)
#ifdef HAVE_MYSQLI_SET_CHARSET
	PHP_FALIAS(set_charset, mysqli_set_charset, arginfo_class_mysqli_set_charset)
#endif
	PHP_FALIAS(set_opt, mysqli_options, arginfo_class_mysqli_options)
	PHP_FALIAS(ssl_set, mysqli_ssl_set, arginfo_class_mysqli_ssl_set)
	PHP_FALIAS(stat, mysqli_stat, arginfo_mysqli_no_params)
	PHP_FALIAS(stmt_init, mysqli_stmt_init, arginfo_mysqli_no_params)
	PHP_FALIAS(store_result, mysqli_store_result, arginfo_class_store_result)
	PHP_FALIAS(thread_safe, mysqli_thread_safe, arginfo_mysqli_no_params)
	PHP_FALIAS(use_result, mysqli_use_result, arginfo_mysqli_no_params)
	PHP_FALIAS(refresh,mysqli_refresh, arginfo_class_mysqli_refresh)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ mysqli_result_methods[]
 *
 * Every user visible function must have an entry in mysqli_result_functions[].
 */
const zend_function_entry mysqli_result_methods[] = {
	PHP_FALIAS(__construct, mysqli_result_construct, NULL)
	PHP_FALIAS(close, mysqli_free_result, arginfo_mysqli_no_params)
	PHP_FALIAS(free, mysqli_free_result, arginfo_mysqli_no_params)
	PHP_FALIAS(data_seek, mysqli_data_seek, arginfo_class_mysqli_data_seek)
	PHP_FALIAS(fetch_field, mysqli_fetch_field, arginfo_mysqli_no_params)
	PHP_FALIAS(fetch_fields, mysqli_fetch_fields, arginfo_mysqli_no_params)
	PHP_FALIAS(fetch_field_direct, mysqli_fetch_field_direct, arginfo_class_mysqli_result_and_fieldnr)
#if defined(MYSQLI_USE_MYSQLND)
	PHP_FALIAS(fetch_all, mysqli_fetch_all, arginfo_mysqli_no_params)
#endif
	PHP_FALIAS(fetch_array, mysqli_fetch_array, arginfo_class_mysqli_fetch_array)
	PHP_FALIAS(fetch_assoc, mysqli_fetch_assoc, arginfo_mysqli_no_params)
	PHP_FALIAS(fetch_object,mysqli_fetch_object, arginfo_class_mysqli_fetch_object)
	PHP_FALIAS(fetch_row, mysqli_fetch_row, arginfo_mysqli_no_params)
	PHP_FALIAS(field_seek, mysqli_field_seek, arginfo_class_mysqli_result_and_fieldnr)
	PHP_FALIAS(free_result, mysqli_free_result, arginfo_mysqli_no_params)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ mysqli_stmt_methods[]
 *
 * Every user visible function must have an entry in mysqli_stmt_functions[].
 */
const zend_function_entry mysqli_stmt_methods[] = {
	PHP_FALIAS(__construct, mysqli_stmt_construct, NULL)
	PHP_FALIAS(attr_get, mysqli_stmt_attr_get, arginfo_class_mysqli_stmt_attr_get)
	PHP_FALIAS(attr_set,mysqli_stmt_attr_set, arginfo_class_mysqli_stmt_attr_set)
	PHP_FALIAS(bind_param,mysqli_stmt_bind_param, arginfo_class_mysqli_stmt_bind_param)
	PHP_FALIAS(bind_result,mysqli_stmt_bind_result, arginfo_class_mysqli_stmt_bind_result)
	PHP_FALIAS(close, mysqli_stmt_close, arginfo_mysqli_no_params)
	PHP_FALIAS(data_seek, mysqli_stmt_data_seek, arginfo_class_mysqli_data_seek)
	PHP_FALIAS(execute, mysqli_stmt_execute, arginfo_mysqli_no_params)
	PHP_FALIAS(fetch, mysqli_stmt_fetch, arginfo_mysqli_no_params)
	PHP_FALIAS(get_warnings, mysqli_stmt_get_warnings,	arginfo_mysqli_no_params)
	PHP_FALIAS(result_metadata, mysqli_stmt_result_metadata, arginfo_mysqli_no_params)
#if defined(MYSQLI_USE_MYSQLND)
	PHP_FALIAS(more_results, mysqli_stmt_more_results, arginfo_mysqli_no_params)
	PHP_FALIAS(next_result, mysqli_stmt_next_result, arginfo_mysqli_no_params)
#endif
	PHP_FALIAS(num_rows, mysqli_stmt_num_rows, arginfo_mysqli_no_params)
	PHP_FALIAS(send_long_data, mysqli_stmt_send_long_data, arginfo_class_mysqli_stmt_send_long_data)
	PHP_FALIAS(free_result, mysqli_stmt_free_result, arginfo_mysqli_no_params)
	PHP_FALIAS(reset, mysqli_stmt_reset, arginfo_mysqli_no_params)
	PHP_FALIAS(prepare, mysqli_stmt_prepare, arginfo_class_mysqli_query)
	PHP_FALIAS(store_result, mysqli_stmt_store_result, arginfo_mysqli_no_params)
#if defined(MYSQLI_USE_MYSQLND)
	PHP_FALIAS(get_result, mysqli_stmt_get_result, arginfo_mysqli_no_params)
#endif
	{NULL, NULL, NULL}
};
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
