/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mysqli_affected_rows, 0, 1, MAY_BE_LONG|MAY_BE_STRING)
	ZEND_ARG_OBJ_INFO(0, mysql_link, mysqli, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_autocommit, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysql_link, mysqli, 0)
	ZEND_ARG_TYPE_INFO(0, mode, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_begin_transaction, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysql_link, mysqli, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_change_user, 0, 4, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysql_link, mysqli, 0)
	ZEND_ARG_TYPE_INFO(0, user, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, database, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_character_set_name, 0, 1, IS_STRING, 1)
	ZEND_ARG_OBJ_INFO(0, mysql_link, mysqli, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_close, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysql_link, mysqli, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_commit, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysql_link, mysqli, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_mysqli_connect, 0, 0, mysqli, MAY_BE_NULL|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, host, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, user, IS_STRING, 1, "null")
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
	ZEND_ARG_OBJ_INFO(0, mysql_result, mysqli_result, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_dump_debug_info arginfo_mysqli_close

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_debug, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, debug, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_errno, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, mysql_link, mysqli, 0)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_error arginfo_mysqli_character_set_name

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_error_list, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_OBJ_INFO(0, mysql_link, mysqli, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_stmt_execute, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysql_stmt, mysqli_stmt, 0)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_execute arginfo_mysqli_stmt_execute

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mysqli_fetch_field, 0, 1, MAY_BE_OBJECT|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, mysql_result, mysqli_result, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_fetch_fields, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_OBJ_INFO(0, mysql_result, mysqli_result, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mysqli_fetch_field_direct, 0, 2, MAY_BE_OBJECT|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, mysql_result, mysqli_result, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mysqli_fetch_lengths, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, mysql_result, mysqli_result, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mysqli_fetch_all, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, mysql_result, mysqli_result, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "MYSQLI_NUM")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mysqli_fetch_array, 0, 1, MAY_BE_ARRAY|MAY_BE_NULL|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, mysql_result, mysqli_result, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, fetchtype, IS_LONG, 0, "MYSQLI_BOTH")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_fetch_assoc, 0, 1, IS_ARRAY, 1)
	ZEND_ARG_OBJ_INFO(0, mysql_result, mysqli_result, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_fetch_object, 0, 1, IS_OBJECT, 1)
	ZEND_ARG_OBJ_INFO(0, mysqli_result, mysqli_result, 0)
	ZEND_ARG_TYPE_INFO(0, class_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, params, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_fetch_row, 0, 1, IS_ARRAY, 1)
	ZEND_ARG_OBJ_INFO(0, mysqli_result, mysqli_result, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_field_count, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, mysqli_link, mysqli, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_field_seek, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysqli_result, mysqli_result, 0)
	ZEND_ARG_TYPE_INFO(0, field_nr, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_field_tell, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, mysqli_result, mysqli_result, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_free_result, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, mysqli_result, mysqli_result, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_get_connection_stats, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_OBJ_INFO(0, mysqli_link, mysqli, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_get_client_stats, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_get_charset, 0, 1, IS_OBJECT, 1)
	ZEND_ARG_OBJ_INFO(0, mysqli_link, mysqli, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_get_client_info, 0, 0, IS_STRING, 1)
	ZEND_ARG_OBJ_INFO(0, mysqli_link, mysqli, 0)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_get_client_version arginfo_mysqli_connect_errno

#define arginfo_mysqli_get_links_stats arginfo_mysqli_get_client_stats

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_get_host_info, 0, 1, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, mysqli_link, mysqli, 0)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_get_proto_info arginfo_mysqli_field_count

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_get_server_info, 0, 1, IS_STRING, 1)
	ZEND_ARG_OBJ_INFO(0, mysqli_link, mysqli, 0)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_get_server_version arginfo_mysqli_field_count

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_mysqli_get_warnings, 0, 1, mysqli_warning, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, mysqli_link, mysqli, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_mysqli_init, 0, 0, mysqli, MAY_BE_FALSE)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_info arginfo_mysqli_get_server_info

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mysqli_insert_id, 0, 1, MAY_BE_LONG|MAY_BE_STRING)
	ZEND_ARG_OBJ_INFO(0, mysqli_link, mysqli, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_kill, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysqli_link, mysqli, 0)
	ZEND_ARG_TYPE_INFO(0, connection_id, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_more_results, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysqli_link, mysqli, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_multi_query, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysqli_link, mysqli, 0)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_next_result arginfo_mysqli_more_results

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_num_fields, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, mysql_result, mysqli_result, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mysqli_num_rows, 0, 1, MAY_BE_LONG|MAY_BE_STRING)
	ZEND_ARG_OBJ_INFO(0, mysqli_result, mysqli_result, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_options, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysqli_link, mysqli, 0)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_ping arginfo_mysqli_more_results

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mysqli_poll, 0, 4, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(1, read, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO(1, write, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO(1, error, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, sec, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, usec, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_mysqli_prepare, 0, 2, mysqli_stmt, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, mysqli_link, mysqli, 0)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_report, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_mysqli_query, 0, 2, mysqli_result, MAY_BE_BOOL)
	ZEND_ARG_OBJ_INFO(0, mysqli_link, mysqli, 0)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, resultmode, IS_LONG, 0, "MYSQLI_STORE_RESULT")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_real_connect, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysqli_link, mysqli, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, host, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, user, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, password, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, database, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, port, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, socket, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_real_escape_string, 0, 2, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, mysqli_link, mysqli, 0)
	ZEND_ARG_TYPE_INFO(0, string_to_escape, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_real_query arginfo_mysqli_multi_query

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_mysqli_reap_async_query, 0, 1, mysqli_result, MAY_BE_BOOL)
	ZEND_ARG_OBJ_INFO(0, mysqli_link, mysqli, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_release_savepoint, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysqli_link, mysqli, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_rollback, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysqli_link, mysqli, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, name, IS_STRING, 0, "\'\'")
ZEND_END_ARG_INFO()

#define arginfo_mysqli_savepoint arginfo_mysqli_release_savepoint

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_select_db, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysqli_link, mysqli, 0)
	ZEND_ARG_TYPE_INFO(0, dbname, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_set_charset, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysqli_link, mysqli, 0)
	ZEND_ARG_TYPE_INFO(0, charset, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mysqli_stmt_affected_rows, 0, 1, MAY_BE_LONG|MAY_BE_STRING)
	ZEND_ARG_OBJ_INFO(0, mysql_stmt, mysqli_stmt, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mysqli_stmt_attr_get, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, mysql_stmt, mysqli_stmt, 0)
	ZEND_ARG_TYPE_INFO(0, attr, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_stmt_attr_set, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysql_stmt, mysqli_stmt, 0)
	ZEND_ARG_TYPE_INFO(0, attr, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, mode_in, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_stmt_bind_param, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysql_stmt, mysqli_stmt, 0)
	ZEND_ARG_TYPE_INFO(0, types, IS_STRING, 0)
	ZEND_ARG_VARIADIC_INFO(1, vars)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_stmt_bind_result, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysql_stmt, mysqli_stmt, 0)
	ZEND_ARG_VARIADIC_INFO(1, vars)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_stmt_close arginfo_mysqli_stmt_execute

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_stmt_data_seek, 0, 2, _IS_BOOL, 1)
	ZEND_ARG_OBJ_INFO(0, mysql_stmt, mysqli_stmt, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_stmt_errno, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, mysql_stmt, mysqli_stmt, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_stmt_error, 0, 1, IS_STRING, 1)
	ZEND_ARG_OBJ_INFO(0, mysql_stmt, mysqli_stmt, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_stmt_error_list, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_OBJ_INFO(0, mysql_stmt, mysqli_stmt, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_stmt_fetch, 0, 1, _IS_BOOL, 1)
	ZEND_ARG_OBJ_INFO(0, mysql_stmt, mysqli_stmt, 0)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_stmt_field_count arginfo_mysqli_stmt_errno

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_stmt_free_result, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, mysql_stmt, mysqli_stmt, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_mysqli_stmt_get_result, 0, 1, mysqli_result, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, mysql_stmt, mysqli_stmt, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_mysqli_stmt_get_warnings, 0, 1, mysqli_warning, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, mysql_stmt, mysqli_stmt, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_mysqli_stmt_init, 0, 1, mysqli_stmt, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, mysql_link, mysqli, 0)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_stmt_insert_id arginfo_mysqli_stmt_affected_rows

#define arginfo_mysqli_stmt_more_results arginfo_mysqli_stmt_execute

#define arginfo_mysqli_stmt_next_result arginfo_mysqli_stmt_execute

#define arginfo_mysqli_stmt_num_rows arginfo_mysqli_stmt_affected_rows

#define arginfo_mysqli_stmt_param_count arginfo_mysqli_stmt_errno

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_stmt_prepare, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysql_stmt, mysqli_stmt, 0)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_stmt_reset arginfo_mysqli_stmt_execute

#define arginfo_mysqli_stmt_result_metadata arginfo_mysqli_stmt_get_result

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_stmt_send_long_data, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysql_stmt, mysqli_stmt, 0)
	ZEND_ARG_TYPE_INFO(0, param_nr, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_stmt_store_result arginfo_mysqli_stmt_execute

#define arginfo_mysqli_stmt_sqlstate arginfo_mysqli_stmt_error

#define arginfo_mysqli_sqlstate arginfo_mysqli_get_server_info

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_ssl_set, 0, 6, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysql_link, mysqli, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, cert, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, certificate_authority, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, certificate_authority_path, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, cipher, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_mysqli_stat, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, mysql_link, mysqli, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_mysqli_store_result, 0, 1, mysqli_result, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, mysql_link, mysqli, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_mysqli_thread_id arginfo_mysqli_errno

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_thread_safe, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_mysqli_use_result, 0, 1, mysqli_result, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, mysql_link, mysqli, 0)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_warning_count arginfo_mysqli_errno

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mysqli_refresh, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, mysqli_link, mysqli, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_mysqli_escape_string arginfo_mysqli_real_escape_string

#define arginfo_mysqli_set_opt arginfo_mysqli_options

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, host, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, user, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, password, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, database, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, port, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, socket, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_autocommit, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, mode, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_begin_transaction, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_change_user, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, user, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, database, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_character_set_name, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_mysqli_close arginfo_class_mysqli_character_set_name

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_commit, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_mysqli_connect arginfo_class_mysqli___construct

#define arginfo_class_mysqli_dump_debug_info arginfo_class_mysqli_character_set_name

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_debug, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, debug_options, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_mysqli_get_charset arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_get_client_info arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_get_connection_stats arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_get_server_info arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_get_warnings arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_init arginfo_class_mysqli_character_set_name

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_kill, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, connection_id, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_multi_query, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_mysqli_more_results arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_next_result arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_ping arginfo_class_mysqli_character_set_name

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_poll, 0, 0, 4)
	ZEND_ARG_TYPE_INFO(1, read, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO(1, write, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO(1, error, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, sec, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, usec, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_mysqli_prepare arginfo_class_mysqli_multi_query

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_query, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, resultmode, IS_LONG, 0, "MYSQLI_STORE_RESULT")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_real_connect, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, host, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, user, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, password, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, database, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, port, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, socket, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_real_escape_string, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, string_to_escape, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_mysqli_reap_async_query arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_escape_string arginfo_class_mysqli_real_escape_string

#define arginfo_class_mysqli_real_query arginfo_class_mysqli_multi_query

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_release_savepoint, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_rollback, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, name, IS_STRING, 0, "\'\'")
ZEND_END_ARG_INFO()

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
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, cert, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, certificate_authority, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, certificate_authority_path, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, cipher, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_mysqli_stat arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_stmt_init arginfo_class_mysqli_character_set_name

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_store_result, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_mysqli_thread_safe arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_use_result arginfo_class_mysqli_character_set_name

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_refresh, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_result___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, mysqli_link, IS_OBJECT, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, resmode, IS_LONG, 0, "MYSQLI_STORE_RESULT")
ZEND_END_ARG_INFO()

#define arginfo_class_mysqli_result_close arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_result_free arginfo_class_mysqli_character_set_name

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_result_data_seek, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_mysqli_result_fetch_field arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_result_fetch_fields arginfo_class_mysqli_character_set_name

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_result_fetch_field_direct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, field_nr, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_result_fetch_all, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, result_type, IS_LONG, 0, "MYSQLI_NUM")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_result_fetch_array, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, result_type, IS_LONG, 0, "MYSQLI_BOTH")
ZEND_END_ARG_INFO()

#define arginfo_class_mysqli_result_fetch_assoc arginfo_class_mysqli_character_set_name

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_result_fetch_object, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, class_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, params, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

#define arginfo_class_mysqli_result_fetch_row arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_result_field_seek arginfo_class_mysqli_result_fetch_field_direct

#define arginfo_class_mysqli_result_free_result arginfo_class_mysqli_character_set_name

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_stmt___construct, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, mysqli_link, mysqli, 0)
	ZEND_ARG_TYPE_INFO(0, statement, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_stmt_attr_get, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, attr, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_stmt_attr_set, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, attr, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, mode_in, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_stmt_bind_param, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, types, IS_STRING, 0)
	ZEND_ARG_VARIADIC_INFO(1, vars)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_stmt_bind_result, 0, 0, 0)
	ZEND_ARG_VARIADIC_INFO(1, vars)
ZEND_END_ARG_INFO()

#define arginfo_class_mysqli_stmt_close arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_stmt_data_seek arginfo_class_mysqli_result_data_seek

#define arginfo_class_mysqli_stmt_execute arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_stmt_fetch arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_stmt_get_warnings arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_stmt_result_metadata arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_stmt_more_results arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_stmt_next_result arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_stmt_num_rows arginfo_class_mysqli_character_set_name

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_mysqli_stmt_send_long_data, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, param_nr, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_mysqli_stmt_free_result arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_stmt_reset arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_stmt_prepare arginfo_class_mysqli_multi_query

#define arginfo_class_mysqli_stmt_store_result arginfo_class_mysqli_character_set_name

#define arginfo_class_mysqli_stmt_get_result arginfo_class_mysqli_character_set_name
