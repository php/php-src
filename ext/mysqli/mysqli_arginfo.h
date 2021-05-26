/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 04fabe8f7b61ccf36e8ed383396bd4b56e11dd25 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mysqli_affected_rows, 0, 1, MAY_BE_LONG|MAY_BE_STRING)
	ZEND_ARG_OBJ_INFO(0, mysql, mysqli, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_autocommit, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysql, mysqli, 0)
	ZEND_ARG_TYPE_INFO(0, enable, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_begin_transaction, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysql, mysqli, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, name, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_change_user, 0, 4, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysql, mysqli, 0)
	ZEND_ARG_TYPE_INFO(0, username, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, database, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_character_set_name, 0, 1, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, mysql, mysqli, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_close, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysql, mysqli, 0)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_commit arginfo_mysqli_begin_transaction

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_mysqli_connect, 0, 0, mysqli, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, hostname, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, username, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, password, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, database, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, port, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, socket, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_connect_errno, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_connect_error, 0, 0, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_data_seek, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, result, mysqli_result, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_dump_debug_info arginfo_mysqli_close

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_debug, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_errno, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, mysql, mysqli, 0)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_error arginfo_mysqli_character_set_name

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_error_list, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_OBJ_INFO(0, mysql, mysqli, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_stmt_execute, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, statement, mysqli_stmt, 0)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_execute arginfo_mysqli_stmt_execute

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mysqli_fetch_field, 0, 1, MAY_BE_OBJECT|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, result, mysqli_result, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_fetch_fields, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_OBJ_INFO(0, result, mysqli_result, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mysqli_fetch_field_direct, 0, 2, MAY_BE_OBJECT|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, result, mysqli_result, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mysqli_fetch_lengths, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, result, mysqli_result, 0)
ZEND_END_ARG_INFO()

#if defined(MYSQLI_USE_MYSQLND)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_fetch_all, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_OBJ_INFO(0, result, mysqli_result, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "MYSQLI_NUM")
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mysqli_fetch_array, 0, 1, MAY_BE_ARRAY|MAY_BE_NULL|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, result, mysqli_result, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "MYSQLI_BOTH")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mysqli_fetch_assoc, 0, 1, MAY_BE_ARRAY|MAY_BE_NULL|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, result, mysqli_result, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mysqli_fetch_object, 0, 1, MAY_BE_OBJECT|MAY_BE_NULL|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, result, mysqli_result, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, class, IS_STRING, 0, "\"stdClass\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, constructor_args, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

#define arginfo_mysqli_fetch_row arginfo_mysqli_fetch_assoc

#define arginfo_mysqli_field_count arginfo_mysqli_errno

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_field_seek, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, result, mysqli_result, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_field_tell, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, result, mysqli_result, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_free_result, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, result, mysqli_result, 0)
ZEND_END_ARG_INFO()

#if defined(MYSQLI_USE_MYSQLND)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_get_connection_stats, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_OBJ_INFO(0, mysql, mysqli, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(MYSQLI_USE_MYSQLND)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_get_client_stats, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_get_charset, 0, 1, IS_OBJECT, 1)
	ZEND_ARG_OBJ_INFO(0, mysql, mysqli, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_get_client_info, 0, 0, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, mysql, mysqli, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_mysqli_get_client_version arginfo_mysqli_connect_errno

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_get_links_stats, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_get_host_info arginfo_mysqli_character_set_name

#define arginfo_mysqli_get_proto_info arginfo_mysqli_errno

#define arginfo_mysqli_get_server_info arginfo_mysqli_character_set_name

#define arginfo_mysqli_get_server_version arginfo_mysqli_errno

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_mysqli_get_warnings, 0, 1, mysqli_warning, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, mysql, mysqli, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_mysqli_init, 0, 0, mysqli, MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_info, 0, 1, IS_STRING, 1)
	ZEND_ARG_OBJ_INFO(0, mysql, mysqli, 0)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_insert_id arginfo_mysqli_affected_rows

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_kill, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysql, mysqli, 0)
	ZEND_ARG_TYPE_INFO(0, process_id, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_more_results arginfo_mysqli_close

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_multi_query, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysql, mysqli, 0)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_next_result arginfo_mysqli_close

#define arginfo_mysqli_num_fields arginfo_mysqli_field_tell

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mysqli_num_rows, 0, 1, MAY_BE_LONG|MAY_BE_STRING)
	ZEND_ARG_OBJ_INFO(0, result, mysqli_result, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_options, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysql, mysqli, 0)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_set_opt arginfo_mysqli_options

#define arginfo_mysqli_ping arginfo_mysqli_close

#if defined(MYSQLI_USE_MYSQLND)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mysqli_poll, 0, 4, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(1, read, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO(1, error, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO(1, reject, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, seconds, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, microseconds, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_mysqli_prepare, 0, 2, mysqli_stmt, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, mysql, mysqli, 0)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_report, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_mysqli_query, 0, 2, mysqli_result, MAY_BE_BOOL)
	ZEND_ARG_OBJ_INFO(0, mysql, mysqli, 0)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, result_mode, IS_LONG, 0, "MYSQLI_STORE_RESULT")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_real_connect, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysql, mysqli, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, hostname, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, username, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, password, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, database, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, port, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, socket, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_real_escape_string, 0, 2, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, mysql, mysqli, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_escape_string arginfo_mysqli_real_escape_string

#define arginfo_mysqli_real_query arginfo_mysqli_multi_query

#if defined(MYSQLI_USE_MYSQLND)
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_mysqli_reap_async_query, 0, 1, mysqli_result, MAY_BE_BOOL)
	ZEND_ARG_OBJ_INFO(0, mysql, mysqli, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_release_savepoint, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysql, mysqli, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_rollback arginfo_mysqli_begin_transaction

#define arginfo_mysqli_savepoint arginfo_mysqli_release_savepoint

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_select_db, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysql, mysqli, 0)
	ZEND_ARG_TYPE_INFO(0, database, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_set_charset, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysql, mysqli, 0)
	ZEND_ARG_TYPE_INFO(0, charset, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mysqli_stmt_affected_rows, 0, 1, MAY_BE_LONG|MAY_BE_STRING)
	ZEND_ARG_OBJ_INFO(0, statement, mysqli_stmt, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_stmt_attr_get, 0, 2, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, statement, mysqli_stmt, 0)
	ZEND_ARG_TYPE_INFO(0, attribute, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_stmt_attr_set, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, statement, mysqli_stmt, 0)
	ZEND_ARG_TYPE_INFO(0, attribute, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_stmt_bind_param, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, statement, mysqli_stmt, 0)
	ZEND_ARG_TYPE_INFO(0, types, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(1, vars, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_stmt_bind_result, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, statement, mysqli_stmt, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(1, vars, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_stmt_close arginfo_mysqli_stmt_execute

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_stmt_data_seek, 0, 2, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, statement, mysqli_stmt, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_stmt_errno, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, statement, mysqli_stmt, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_stmt_error, 0, 1, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, statement, mysqli_stmt, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_stmt_error_list, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_OBJ_INFO(0, statement, mysqli_stmt, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_stmt_fetch, 0, 1, _IS_BOOL, 1)
	ZEND_ARG_OBJ_INFO(0, statement, mysqli_stmt, 0)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_stmt_field_count arginfo_mysqli_stmt_errno

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_stmt_free_result, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, statement, mysqli_stmt, 0)
ZEND_END_ARG_INFO()

#if defined(MYSQLI_USE_MYSQLND)
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_mysqli_stmt_get_result, 0, 1, mysqli_result, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, statement, mysqli_stmt, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_mysqli_stmt_get_warnings, 0, 1, mysqli_warning, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, statement, mysqli_stmt, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_mysqli_stmt_init, 0, 1, mysqli_stmt, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, mysql, mysqli, 0)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_stmt_insert_id arginfo_mysqli_stmt_affected_rows

#if defined(MYSQLI_USE_MYSQLND)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_stmt_more_results, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, statement, mysqli_stmt, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(MYSQLI_USE_MYSQLND)
#define arginfo_mysqli_stmt_next_result arginfo_mysqli_stmt_more_results
#endif

#define arginfo_mysqli_stmt_num_rows arginfo_mysqli_stmt_affected_rows

#define arginfo_mysqli_stmt_param_count arginfo_mysqli_stmt_errno

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_stmt_prepare, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, statement, mysqli_stmt, 0)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_stmt_reset arginfo_mysqli_stmt_execute

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_mysqli_stmt_result_metadata, 0, 1, mysqli_result, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, statement, mysqli_stmt, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_stmt_send_long_data, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, statement, mysqli_stmt, 0)
	ZEND_ARG_TYPE_INFO(0, param_num, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_stmt_store_result arginfo_mysqli_stmt_execute

#define arginfo_mysqli_stmt_sqlstate arginfo_mysqli_stmt_error

#define arginfo_mysqli_sqlstate arginfo_mysqli_character_set_name

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_ssl_set, 0, 6, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysql, mysqli, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, certificate, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, ca_certificate, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, ca_path, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, cipher_algos, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mysqli_stat, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, mysql, mysqli, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_mysqli_store_result, 0, 1, mysqli_result, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, mysql, mysqli, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_mysqli_thread_id arginfo_mysqli_errno

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_thread_safe, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_mysqli_use_result, 0, 1, mysqli_result, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, mysql, mysqli, 0)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_warning_count arginfo_mysqli_errno

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_refresh, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysql, mysqli, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, hostname, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, username, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, password, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, database, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, port, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, socket, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_autocommit, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, enable, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_begin_transaction, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, name, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_change_user, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, username, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, database, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_character_set_name, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_mysqli_close arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_commit arginfo_class_mysqli_begin_transaction

#define arginfo_class_mysqli_connect arginfo_class_mysqli___construct

#define arginfo_class_mysqli_dump_debug_info arginfo_class_mysqli_character_set_name

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_debug, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, options, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_mysqli_get_charset arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_get_client_info arginfo_class_mysqli_character_set_name

#if defined(MYSQLI_USE_MYSQLND)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_get_connection_stats, 0, 0, 0)
ZEND_END_ARG_INFO()
#endif

#define arginfo_class_mysqli_get_server_info arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_get_warnings arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_init arginfo_class_mysqli_character_set_name

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_kill, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, process_id, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_multi_query, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_mysqli_more_results arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_next_result arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_ping arginfo_class_mysqli_character_set_name

#if defined(MYSQLI_USE_MYSQLND)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_poll, 0, 0, 4)
	ZEND_ARG_TYPE_INFO(1, read, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO(1, error, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO(1, reject, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, seconds, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, microseconds, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#define arginfo_class_mysqli_prepare arginfo_class_mysqli_multi_query

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_query, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, result_mode, IS_LONG, 0, "MYSQLI_STORE_RESULT")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_real_connect, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, hostname, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, username, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, password, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, database, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, port, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, socket, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_real_escape_string, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

#if defined(MYSQLI_USE_MYSQLND)
#define arginfo_class_mysqli_reap_async_query arginfo_class_mysqli_get_connection_stats
#endif

#define arginfo_class_mysqli_escape_string arginfo_class_mysqli_real_escape_string

#define arginfo_class_mysqli_real_query arginfo_class_mysqli_multi_query

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_release_savepoint, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_mysqli_rollback arginfo_class_mysqli_begin_transaction

#define arginfo_class_mysqli_savepoint arginfo_class_mysqli_release_savepoint

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_select_db, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, database, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_set_charset, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, charset, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_options, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

#define arginfo_class_mysqli_set_opt arginfo_class_mysqli_options

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_ssl_set, 0, 0, 5)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, certificate, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, ca_certificate, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, ca_path, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, cipher_algos, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_mysqli_stat arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_stmt_init arginfo_class_mysqli_character_set_name

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_store_result, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_mysqli_thread_safe arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_use_result arginfo_class_mysqli_character_set_name

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_refresh, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_result___construct, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, mysql, mysqli, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, result_mode, IS_LONG, 0, "MYSQLI_STORE_RESULT")
ZEND_END_ARG_INFO()

#define arginfo_class_mysqli_result_close arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_result_free arginfo_class_mysqli_character_set_name

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_result_data_seek, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_mysqli_result_fetch_field arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_result_fetch_fields arginfo_class_mysqli_character_set_name

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_result_fetch_field_direct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if defined(MYSQLI_USE_MYSQLND)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_result_fetch_all, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "MYSQLI_NUM")
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_result_fetch_array, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "MYSQLI_BOTH")
ZEND_END_ARG_INFO()

#define arginfo_class_mysqli_result_fetch_assoc arginfo_class_mysqli_character_set_name

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_result_fetch_object, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, class, IS_STRING, 0, "\"stdClass\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, constructor_args, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

#define arginfo_class_mysqli_result_fetch_row arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_result_field_seek arginfo_class_mysqli_result_fetch_field_direct

#define arginfo_class_mysqli_result_free_result arginfo_class_mysqli_character_set_name

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_mysqli_result_getIterator, 0, 0, Iterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_stmt___construct, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, mysql, mysqli, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, query, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_stmt_attr_get, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, attribute, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_stmt_attr_set, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, attribute, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_stmt_bind_param, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, types, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(1, vars, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_stmt_bind_result, 0, 0, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(1, vars, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_mysqli_stmt_close arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_stmt_data_seek arginfo_class_mysqli_result_data_seek

#define arginfo_class_mysqli_stmt_execute arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_stmt_fetch arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_stmt_get_warnings arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_stmt_result_metadata arginfo_class_mysqli_character_set_name

#if defined(MYSQLI_USE_MYSQLND)
#define arginfo_class_mysqli_stmt_more_results arginfo_class_mysqli_get_connection_stats
#endif

#if defined(MYSQLI_USE_MYSQLND)
#define arginfo_class_mysqli_stmt_next_result arginfo_class_mysqli_get_connection_stats
#endif

#define arginfo_class_mysqli_stmt_num_rows arginfo_class_mysqli_character_set_name

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_stmt_send_long_data, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, param_num, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_mysqli_stmt_free_result arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_stmt_reset arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_stmt_prepare arginfo_class_mysqli_multi_query

#define arginfo_class_mysqli_stmt_store_result arginfo_class_mysqli_character_set_name

#if defined(MYSQLI_USE_MYSQLND)
#define arginfo_class_mysqli_stmt_get_result arginfo_class_mysqli_get_connection_stats
#endif

#define arginfo_class_mysqli_warning___construct arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_warning_next arginfo_mysqli_thread_safe


ZEND_FUNCTION(mysqli_affected_rows);
ZEND_FUNCTION(mysqli_autocommit);
ZEND_FUNCTION(mysqli_begin_transaction);
ZEND_FUNCTION(mysqli_change_user);
ZEND_FUNCTION(mysqli_character_set_name);
ZEND_FUNCTION(mysqli_close);
ZEND_FUNCTION(mysqli_commit);
ZEND_FUNCTION(mysqli_connect);
ZEND_FUNCTION(mysqli_connect_errno);
ZEND_FUNCTION(mysqli_connect_error);
ZEND_FUNCTION(mysqli_data_seek);
ZEND_FUNCTION(mysqli_dump_debug_info);
ZEND_FUNCTION(mysqli_debug);
ZEND_FUNCTION(mysqli_errno);
ZEND_FUNCTION(mysqli_error);
ZEND_FUNCTION(mysqli_error_list);
ZEND_FUNCTION(mysqli_stmt_execute);
ZEND_FUNCTION(mysqli_fetch_field);
ZEND_FUNCTION(mysqli_fetch_fields);
ZEND_FUNCTION(mysqli_fetch_field_direct);
ZEND_FUNCTION(mysqli_fetch_lengths);
#if defined(MYSQLI_USE_MYSQLND)
ZEND_FUNCTION(mysqli_fetch_all);
#endif
ZEND_FUNCTION(mysqli_fetch_array);
ZEND_FUNCTION(mysqli_fetch_assoc);
ZEND_FUNCTION(mysqli_fetch_object);
ZEND_FUNCTION(mysqli_fetch_row);
ZEND_FUNCTION(mysqli_field_count);
ZEND_FUNCTION(mysqli_field_seek);
ZEND_FUNCTION(mysqli_field_tell);
ZEND_FUNCTION(mysqli_free_result);
#if defined(MYSQLI_USE_MYSQLND)
ZEND_FUNCTION(mysqli_get_connection_stats);
#endif
#if defined(MYSQLI_USE_MYSQLND)
ZEND_FUNCTION(mysqli_get_client_stats);
#endif
ZEND_FUNCTION(mysqli_get_charset);
ZEND_FUNCTION(mysqli_get_client_info);
ZEND_FUNCTION(mysqli_get_client_version);
ZEND_FUNCTION(mysqli_get_links_stats);
ZEND_FUNCTION(mysqli_get_host_info);
ZEND_FUNCTION(mysqli_get_proto_info);
ZEND_FUNCTION(mysqli_get_server_info);
ZEND_FUNCTION(mysqli_get_server_version);
ZEND_FUNCTION(mysqli_get_warnings);
ZEND_FUNCTION(mysqli_init);
ZEND_FUNCTION(mysqli_info);
ZEND_FUNCTION(mysqli_insert_id);
ZEND_FUNCTION(mysqli_kill);
ZEND_FUNCTION(mysqli_more_results);
ZEND_FUNCTION(mysqli_multi_query);
ZEND_FUNCTION(mysqli_next_result);
ZEND_FUNCTION(mysqli_num_fields);
ZEND_FUNCTION(mysqli_num_rows);
ZEND_FUNCTION(mysqli_options);
ZEND_FUNCTION(mysqli_ping);
#if defined(MYSQLI_USE_MYSQLND)
ZEND_FUNCTION(mysqli_poll);
#endif
ZEND_FUNCTION(mysqli_prepare);
ZEND_FUNCTION(mysqli_report);
ZEND_FUNCTION(mysqli_query);
ZEND_FUNCTION(mysqli_real_connect);
ZEND_FUNCTION(mysqli_real_escape_string);
ZEND_FUNCTION(mysqli_real_query);
#if defined(MYSQLI_USE_MYSQLND)
ZEND_FUNCTION(mysqli_reap_async_query);
#endif
ZEND_FUNCTION(mysqli_release_savepoint);
ZEND_FUNCTION(mysqli_rollback);
ZEND_FUNCTION(mysqli_savepoint);
ZEND_FUNCTION(mysqli_select_db);
ZEND_FUNCTION(mysqli_set_charset);
ZEND_FUNCTION(mysqli_stmt_affected_rows);
ZEND_FUNCTION(mysqli_stmt_attr_get);
ZEND_FUNCTION(mysqli_stmt_attr_set);
ZEND_FUNCTION(mysqli_stmt_bind_param);
ZEND_FUNCTION(mysqli_stmt_bind_result);
ZEND_FUNCTION(mysqli_stmt_close);
ZEND_FUNCTION(mysqli_stmt_data_seek);
ZEND_FUNCTION(mysqli_stmt_errno);
ZEND_FUNCTION(mysqli_stmt_error);
ZEND_FUNCTION(mysqli_stmt_error_list);
ZEND_FUNCTION(mysqli_stmt_fetch);
ZEND_FUNCTION(mysqli_stmt_field_count);
ZEND_FUNCTION(mysqli_stmt_free_result);
#if defined(MYSQLI_USE_MYSQLND)
ZEND_FUNCTION(mysqli_stmt_get_result);
#endif
ZEND_FUNCTION(mysqli_stmt_get_warnings);
ZEND_FUNCTION(mysqli_stmt_init);
ZEND_FUNCTION(mysqli_stmt_insert_id);
#if defined(MYSQLI_USE_MYSQLND)
ZEND_FUNCTION(mysqli_stmt_more_results);
#endif
#if defined(MYSQLI_USE_MYSQLND)
ZEND_FUNCTION(mysqli_stmt_next_result);
#endif
ZEND_FUNCTION(mysqli_stmt_num_rows);
ZEND_FUNCTION(mysqli_stmt_param_count);
ZEND_FUNCTION(mysqli_stmt_prepare);
ZEND_FUNCTION(mysqli_stmt_reset);
ZEND_FUNCTION(mysqli_stmt_result_metadata);
ZEND_FUNCTION(mysqli_stmt_send_long_data);
ZEND_FUNCTION(mysqli_stmt_store_result);
ZEND_FUNCTION(mysqli_stmt_sqlstate);
ZEND_FUNCTION(mysqli_sqlstate);
ZEND_FUNCTION(mysqli_ssl_set);
ZEND_FUNCTION(mysqli_stat);
ZEND_FUNCTION(mysqli_store_result);
ZEND_FUNCTION(mysqli_thread_id);
ZEND_FUNCTION(mysqli_thread_safe);
ZEND_FUNCTION(mysqli_use_result);
ZEND_FUNCTION(mysqli_warning_count);
ZEND_FUNCTION(mysqli_refresh);
ZEND_METHOD(mysqli, __construct);
ZEND_METHOD(mysqli, init);
ZEND_METHOD(mysqli_result, __construct);
ZEND_METHOD(mysqli_result, getIterator);
ZEND_METHOD(mysqli_stmt, __construct);
ZEND_METHOD(mysqli_warning, __construct);
ZEND_METHOD(mysqli_warning, next);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(mysqli_affected_rows, arginfo_mysqli_affected_rows)
	ZEND_FE(mysqli_autocommit, arginfo_mysqli_autocommit)
	ZEND_FE(mysqli_begin_transaction, arginfo_mysqli_begin_transaction)
	ZEND_FE(mysqli_change_user, arginfo_mysqli_change_user)
	ZEND_FE(mysqli_character_set_name, arginfo_mysqli_character_set_name)
	ZEND_FE(mysqli_close, arginfo_mysqli_close)
	ZEND_FE(mysqli_commit, arginfo_mysqli_commit)
	ZEND_FE(mysqli_connect, arginfo_mysqli_connect)
	ZEND_FE(mysqli_connect_errno, arginfo_mysqli_connect_errno)
	ZEND_FE(mysqli_connect_error, arginfo_mysqli_connect_error)
	ZEND_FE(mysqli_data_seek, arginfo_mysqli_data_seek)
	ZEND_FE(mysqli_dump_debug_info, arginfo_mysqli_dump_debug_info)
	ZEND_FE(mysqli_debug, arginfo_mysqli_debug)
	ZEND_FE(mysqli_errno, arginfo_mysqli_errno)
	ZEND_FE(mysqli_error, arginfo_mysqli_error)
	ZEND_FE(mysqli_error_list, arginfo_mysqli_error_list)
	ZEND_FE(mysqli_stmt_execute, arginfo_mysqli_stmt_execute)
	ZEND_FALIAS(mysqli_execute, mysqli_stmt_execute, arginfo_mysqli_execute)
	ZEND_FE(mysqli_fetch_field, arginfo_mysqli_fetch_field)
	ZEND_FE(mysqli_fetch_fields, arginfo_mysqli_fetch_fields)
	ZEND_FE(mysqli_fetch_field_direct, arginfo_mysqli_fetch_field_direct)
	ZEND_FE(mysqli_fetch_lengths, arginfo_mysqli_fetch_lengths)
#if defined(MYSQLI_USE_MYSQLND)
	ZEND_FE(mysqli_fetch_all, arginfo_mysqli_fetch_all)
#endif
	ZEND_FE(mysqli_fetch_array, arginfo_mysqli_fetch_array)
	ZEND_FE(mysqli_fetch_assoc, arginfo_mysqli_fetch_assoc)
	ZEND_FE(mysqli_fetch_object, arginfo_mysqli_fetch_object)
	ZEND_FE(mysqli_fetch_row, arginfo_mysqli_fetch_row)
	ZEND_FE(mysqli_field_count, arginfo_mysqli_field_count)
	ZEND_FE(mysqli_field_seek, arginfo_mysqli_field_seek)
	ZEND_FE(mysqli_field_tell, arginfo_mysqli_field_tell)
	ZEND_FE(mysqli_free_result, arginfo_mysqli_free_result)
#if defined(MYSQLI_USE_MYSQLND)
	ZEND_FE(mysqli_get_connection_stats, arginfo_mysqli_get_connection_stats)
#endif
#if defined(MYSQLI_USE_MYSQLND)
	ZEND_FE(mysqli_get_client_stats, arginfo_mysqli_get_client_stats)
#endif
	ZEND_FE(mysqli_get_charset, arginfo_mysqli_get_charset)
	ZEND_FE(mysqli_get_client_info, arginfo_mysqli_get_client_info)
	ZEND_FE(mysqli_get_client_version, arginfo_mysqli_get_client_version)
	ZEND_FE(mysqli_get_links_stats, arginfo_mysqli_get_links_stats)
	ZEND_FE(mysqli_get_host_info, arginfo_mysqli_get_host_info)
	ZEND_FE(mysqli_get_proto_info, arginfo_mysqli_get_proto_info)
	ZEND_FE(mysqli_get_server_info, arginfo_mysqli_get_server_info)
	ZEND_FE(mysqli_get_server_version, arginfo_mysqli_get_server_version)
	ZEND_FE(mysqli_get_warnings, arginfo_mysqli_get_warnings)
	ZEND_FE(mysqli_init, arginfo_mysqli_init)
	ZEND_FE(mysqli_info, arginfo_mysqli_info)
	ZEND_FE(mysqli_insert_id, arginfo_mysqli_insert_id)
	ZEND_FE(mysqli_kill, arginfo_mysqli_kill)
	ZEND_FE(mysqli_more_results, arginfo_mysqli_more_results)
	ZEND_FE(mysqli_multi_query, arginfo_mysqli_multi_query)
	ZEND_FE(mysqli_next_result, arginfo_mysqli_next_result)
	ZEND_FE(mysqli_num_fields, arginfo_mysqli_num_fields)
	ZEND_FE(mysqli_num_rows, arginfo_mysqli_num_rows)
	ZEND_FE(mysqli_options, arginfo_mysqli_options)
	ZEND_FALIAS(mysqli_set_opt, mysqli_options, arginfo_mysqli_set_opt)
	ZEND_FE(mysqli_ping, arginfo_mysqli_ping)
#if defined(MYSQLI_USE_MYSQLND)
	ZEND_FE(mysqli_poll, arginfo_mysqli_poll)
#endif
	ZEND_FE(mysqli_prepare, arginfo_mysqli_prepare)
	ZEND_FE(mysqli_report, arginfo_mysqli_report)
	ZEND_FE(mysqli_query, arginfo_mysqli_query)
	ZEND_FE(mysqli_real_connect, arginfo_mysqli_real_connect)
	ZEND_FE(mysqli_real_escape_string, arginfo_mysqli_real_escape_string)
	ZEND_FALIAS(mysqli_escape_string, mysqli_real_escape_string, arginfo_mysqli_escape_string)
	ZEND_FE(mysqli_real_query, arginfo_mysqli_real_query)
#if defined(MYSQLI_USE_MYSQLND)
	ZEND_FE(mysqli_reap_async_query, arginfo_mysqli_reap_async_query)
#endif
	ZEND_FE(mysqli_release_savepoint, arginfo_mysqli_release_savepoint)
	ZEND_FE(mysqli_rollback, arginfo_mysqli_rollback)
	ZEND_FE(mysqli_savepoint, arginfo_mysqli_savepoint)
	ZEND_FE(mysqli_select_db, arginfo_mysqli_select_db)
	ZEND_FE(mysqli_set_charset, arginfo_mysqli_set_charset)
	ZEND_FE(mysqli_stmt_affected_rows, arginfo_mysqli_stmt_affected_rows)
	ZEND_FE(mysqli_stmt_attr_get, arginfo_mysqli_stmt_attr_get)
	ZEND_FE(mysqli_stmt_attr_set, arginfo_mysqli_stmt_attr_set)
	ZEND_FE(mysqli_stmt_bind_param, arginfo_mysqli_stmt_bind_param)
	ZEND_FE(mysqli_stmt_bind_result, arginfo_mysqli_stmt_bind_result)
	ZEND_FE(mysqli_stmt_close, arginfo_mysqli_stmt_close)
	ZEND_FE(mysqli_stmt_data_seek, arginfo_mysqli_stmt_data_seek)
	ZEND_FE(mysqli_stmt_errno, arginfo_mysqli_stmt_errno)
	ZEND_FE(mysqli_stmt_error, arginfo_mysqli_stmt_error)
	ZEND_FE(mysqli_stmt_error_list, arginfo_mysqli_stmt_error_list)
	ZEND_FE(mysqli_stmt_fetch, arginfo_mysqli_stmt_fetch)
	ZEND_FE(mysqli_stmt_field_count, arginfo_mysqli_stmt_field_count)
	ZEND_FE(mysqli_stmt_free_result, arginfo_mysqli_stmt_free_result)
#if defined(MYSQLI_USE_MYSQLND)
	ZEND_FE(mysqli_stmt_get_result, arginfo_mysqli_stmt_get_result)
#endif
	ZEND_FE(mysqli_stmt_get_warnings, arginfo_mysqli_stmt_get_warnings)
	ZEND_FE(mysqli_stmt_init, arginfo_mysqli_stmt_init)
	ZEND_FE(mysqli_stmt_insert_id, arginfo_mysqli_stmt_insert_id)
#if defined(MYSQLI_USE_MYSQLND)
	ZEND_FE(mysqli_stmt_more_results, arginfo_mysqli_stmt_more_results)
#endif
#if defined(MYSQLI_USE_MYSQLND)
	ZEND_FE(mysqli_stmt_next_result, arginfo_mysqli_stmt_next_result)
#endif
	ZEND_FE(mysqli_stmt_num_rows, arginfo_mysqli_stmt_num_rows)
	ZEND_FE(mysqli_stmt_param_count, arginfo_mysqli_stmt_param_count)
	ZEND_FE(mysqli_stmt_prepare, arginfo_mysqli_stmt_prepare)
	ZEND_FE(mysqli_stmt_reset, arginfo_mysqli_stmt_reset)
	ZEND_FE(mysqli_stmt_result_metadata, arginfo_mysqli_stmt_result_metadata)
	ZEND_FE(mysqli_stmt_send_long_data, arginfo_mysqli_stmt_send_long_data)
	ZEND_FE(mysqli_stmt_store_result, arginfo_mysqli_stmt_store_result)
	ZEND_FE(mysqli_stmt_sqlstate, arginfo_mysqli_stmt_sqlstate)
	ZEND_FE(mysqli_sqlstate, arginfo_mysqli_sqlstate)
	ZEND_FE(mysqli_ssl_set, arginfo_mysqli_ssl_set)
	ZEND_FE(mysqli_stat, arginfo_mysqli_stat)
	ZEND_FE(mysqli_store_result, arginfo_mysqli_store_result)
	ZEND_FE(mysqli_thread_id, arginfo_mysqli_thread_id)
	ZEND_FE(mysqli_thread_safe, arginfo_mysqli_thread_safe)
	ZEND_FE(mysqli_use_result, arginfo_mysqli_use_result)
	ZEND_FE(mysqli_warning_count, arginfo_mysqli_warning_count)
	ZEND_FE(mysqli_refresh, arginfo_mysqli_refresh)
	ZEND_FE_END
};


static const zend_function_entry class_mysqli_driver_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_mysqli_methods[] = {
	ZEND_ME(mysqli, __construct, arginfo_class_mysqli___construct, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(autocommit, mysqli_autocommit, arginfo_class_mysqli_autocommit, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(begin_transaction, mysqli_begin_transaction, arginfo_class_mysqli_begin_transaction, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(change_user, mysqli_change_user, arginfo_class_mysqli_change_user, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(character_set_name, mysqli_character_set_name, arginfo_class_mysqli_character_set_name, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(close, mysqli_close, arginfo_class_mysqli_close, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(commit, mysqli_commit, arginfo_class_mysqli_commit, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(connect, mysqli_connect, arginfo_class_mysqli_connect, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(dump_debug_info, mysqli_dump_debug_info, arginfo_class_mysqli_dump_debug_info, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(debug, mysqli_debug, arginfo_class_mysqli_debug, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(get_charset, mysqli_get_charset, arginfo_class_mysqli_get_charset, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(get_client_info, mysqli_get_client_info, arginfo_class_mysqli_get_client_info, ZEND_ACC_PUBLIC)
#if defined(MYSQLI_USE_MYSQLND)
	ZEND_ME_MAPPING(get_connection_stats, mysqli_get_connection_stats, arginfo_class_mysqli_get_connection_stats, ZEND_ACC_PUBLIC)
#endif
	ZEND_ME_MAPPING(get_server_info, mysqli_get_server_info, arginfo_class_mysqli_get_server_info, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(get_warnings, mysqli_get_warnings, arginfo_class_mysqli_get_warnings, ZEND_ACC_PUBLIC)
	ZEND_ME(mysqli, init, arginfo_class_mysqli_init, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(kill, mysqli_kill, arginfo_class_mysqli_kill, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(multi_query, mysqli_multi_query, arginfo_class_mysqli_multi_query, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(more_results, mysqli_more_results, arginfo_class_mysqli_more_results, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(next_result, mysqli_next_result, arginfo_class_mysqli_next_result, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(ping, mysqli_ping, arginfo_class_mysqli_ping, ZEND_ACC_PUBLIC)
#if defined(MYSQLI_USE_MYSQLND)
	ZEND_ME_MAPPING(poll, mysqli_poll, arginfo_class_mysqli_poll, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
#endif
	ZEND_ME_MAPPING(prepare, mysqli_prepare, arginfo_class_mysqli_prepare, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(query, mysqli_query, arginfo_class_mysqli_query, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(real_connect, mysqli_real_connect, arginfo_class_mysqli_real_connect, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(real_escape_string, mysqli_real_escape_string, arginfo_class_mysqli_real_escape_string, ZEND_ACC_PUBLIC)
#if defined(MYSQLI_USE_MYSQLND)
	ZEND_ME_MAPPING(reap_async_query, mysqli_reap_async_query, arginfo_class_mysqli_reap_async_query, ZEND_ACC_PUBLIC)
#endif
	ZEND_ME_MAPPING(escape_string, mysqli_real_escape_string, arginfo_class_mysqli_escape_string, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(real_query, mysqli_real_query, arginfo_class_mysqli_real_query, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(release_savepoint, mysqli_release_savepoint, arginfo_class_mysqli_release_savepoint, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(rollback, mysqli_rollback, arginfo_class_mysqli_rollback, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(savepoint, mysqli_savepoint, arginfo_class_mysqli_savepoint, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(select_db, mysqli_select_db, arginfo_class_mysqli_select_db, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(set_charset, mysqli_set_charset, arginfo_class_mysqli_set_charset, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(options, mysqli_options, arginfo_class_mysqli_options, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(set_opt, mysqli_options, arginfo_class_mysqli_set_opt, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(ssl_set, mysqli_ssl_set, arginfo_class_mysqli_ssl_set, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(stat, mysqli_stat, arginfo_class_mysqli_stat, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(stmt_init, mysqli_stmt_init, arginfo_class_mysqli_stmt_init, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(store_result, mysqli_store_result, arginfo_class_mysqli_store_result, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(thread_safe, mysqli_thread_safe, arginfo_class_mysqli_thread_safe, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(use_result, mysqli_use_result, arginfo_class_mysqli_use_result, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(refresh, mysqli_refresh, arginfo_class_mysqli_refresh, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_mysqli_result_methods[] = {
	ZEND_ME(mysqli_result, __construct, arginfo_class_mysqli_result___construct, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(close, mysqli_free_result, arginfo_class_mysqli_result_close, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(free, mysqli_free_result, arginfo_class_mysqli_result_free, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(data_seek, mysqli_data_seek, arginfo_class_mysqli_result_data_seek, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(fetch_field, mysqli_fetch_field, arginfo_class_mysqli_result_fetch_field, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(fetch_fields, mysqli_fetch_fields, arginfo_class_mysqli_result_fetch_fields, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(fetch_field_direct, mysqli_fetch_field_direct, arginfo_class_mysqli_result_fetch_field_direct, ZEND_ACC_PUBLIC)
#if defined(MYSQLI_USE_MYSQLND)
	ZEND_ME_MAPPING(fetch_all, mysqli_fetch_all, arginfo_class_mysqli_result_fetch_all, ZEND_ACC_PUBLIC)
#endif
	ZEND_ME_MAPPING(fetch_array, mysqli_fetch_array, arginfo_class_mysqli_result_fetch_array, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(fetch_assoc, mysqli_fetch_assoc, arginfo_class_mysqli_result_fetch_assoc, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(fetch_object, mysqli_fetch_object, arginfo_class_mysqli_result_fetch_object, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(fetch_row, mysqli_fetch_row, arginfo_class_mysqli_result_fetch_row, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(field_seek, mysqli_field_seek, arginfo_class_mysqli_result_field_seek, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(free_result, mysqli_free_result, arginfo_class_mysqli_result_free_result, ZEND_ACC_PUBLIC)
	ZEND_ME(mysqli_result, getIterator, arginfo_class_mysqli_result_getIterator, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_mysqli_stmt_methods[] = {
	ZEND_ME(mysqli_stmt, __construct, arginfo_class_mysqli_stmt___construct, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(attr_get, mysqli_stmt_attr_get, arginfo_class_mysqli_stmt_attr_get, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(attr_set, mysqli_stmt_attr_set, arginfo_class_mysqli_stmt_attr_set, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(bind_param, mysqli_stmt_bind_param, arginfo_class_mysqli_stmt_bind_param, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(bind_result, mysqli_stmt_bind_result, arginfo_class_mysqli_stmt_bind_result, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(close, mysqli_stmt_close, arginfo_class_mysqli_stmt_close, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(data_seek, mysqli_stmt_data_seek, arginfo_class_mysqli_stmt_data_seek, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(execute, mysqli_stmt_execute, arginfo_class_mysqli_stmt_execute, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(fetch, mysqli_stmt_fetch, arginfo_class_mysqli_stmt_fetch, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(get_warnings, mysqli_stmt_get_warnings, arginfo_class_mysqli_stmt_get_warnings, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(result_metadata, mysqli_stmt_result_metadata, arginfo_class_mysqli_stmt_result_metadata, ZEND_ACC_PUBLIC)
#if defined(MYSQLI_USE_MYSQLND)
	ZEND_ME_MAPPING(more_results, mysqli_stmt_more_results, arginfo_class_mysqli_stmt_more_results, ZEND_ACC_PUBLIC)
#endif
#if defined(MYSQLI_USE_MYSQLND)
	ZEND_ME_MAPPING(next_result, mysqli_stmt_next_result, arginfo_class_mysqli_stmt_next_result, ZEND_ACC_PUBLIC)
#endif
	ZEND_ME_MAPPING(num_rows, mysqli_stmt_num_rows, arginfo_class_mysqli_stmt_num_rows, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(send_long_data, mysqli_stmt_send_long_data, arginfo_class_mysqli_stmt_send_long_data, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(free_result, mysqli_stmt_free_result, arginfo_class_mysqli_stmt_free_result, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(reset, mysqli_stmt_reset, arginfo_class_mysqli_stmt_reset, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(prepare, mysqli_stmt_prepare, arginfo_class_mysqli_stmt_prepare, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(store_result, mysqli_stmt_store_result, arginfo_class_mysqli_stmt_store_result, ZEND_ACC_PUBLIC)
#if defined(MYSQLI_USE_MYSQLND)
	ZEND_ME_MAPPING(get_result, mysqli_stmt_get_result, arginfo_class_mysqli_stmt_get_result, ZEND_ACC_PUBLIC)
#endif
	ZEND_FE_END
};


static const zend_function_entry class_mysqli_warning_methods[] = {
	ZEND_ME(mysqli_warning, __construct, arginfo_class_mysqli_warning___construct, ZEND_ACC_PRIVATE)
	ZEND_ME(mysqli_warning, next, arginfo_class_mysqli_warning_next, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_mysqli_sql_exception_methods[] = {
	ZEND_FE_END
};
