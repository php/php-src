/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 7966ed8575d26d934f6b292f3b13b0b6a4bf0e1e */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_pg_connect, 0, 1, PgSql\\Connection, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, connection_string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_pg_pconnect arginfo_pg_connect

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_connect_poll, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, connection, PgSql\\Connection, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_close, 0, 0, IS_TRUE, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, connection, PgSql\\Connection, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_dbname, 0, 0, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, connection, PgSql\\Connection, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_pg_last_error arginfo_pg_dbname

#define arginfo_pg_errormessage arginfo_pg_dbname

#define arginfo_pg_options arginfo_pg_dbname

#define arginfo_pg_port arginfo_pg_dbname

#define arginfo_pg_tty arginfo_pg_dbname

#define arginfo_pg_host arginfo_pg_dbname

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_version, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, connection, PgSql\\Connection, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_parameter_status, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_ping, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, connection, PgSql\\Connection, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_pg_query, 0, 1, PgSql\\Result, MAY_BE_FALSE)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_pg_exec arginfo_pg_query

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_pg_query_params, 0, 2, PgSql\\Result, MAY_BE_FALSE)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, query)
	ZEND_ARG_TYPE_INFO(0, params, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_pg_prepare, 0, 2, PgSql\\Result, MAY_BE_FALSE)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, statement_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_pg_execute, 0, 2, PgSql\\Result, MAY_BE_FALSE)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, statement_name)
	ZEND_ARG_TYPE_INFO(0, params, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_num_rows, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, result, PgSql\\Result, 0)
ZEND_END_ARG_INFO()

#define arginfo_pg_numrows arginfo_pg_num_rows

#define arginfo_pg_num_fields arginfo_pg_num_rows

#define arginfo_pg_numfields arginfo_pg_num_rows

#define arginfo_pg_affected_rows arginfo_pg_num_rows

#define arginfo_pg_cmdtuples arginfo_pg_num_rows

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_last_notice, 0, 1, MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_OBJ_INFO(0, connection, PgSql\\Connection, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "PGSQL_NOTICE_LAST")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_field_table, 0, 2, MAY_BE_STRING|MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, result, PgSql\\Result, 0)
	ZEND_ARG_TYPE_INFO(0, field, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, oid_only, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_field_name, 0, 2, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, result, PgSql\\Result, 0)
	ZEND_ARG_TYPE_INFO(0, field, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_pg_fieldname arginfo_pg_field_name

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_field_size, 0, 2, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, result, PgSql\\Result, 0)
	ZEND_ARG_TYPE_INFO(0, field, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_pg_fieldsize arginfo_pg_field_size

#define arginfo_pg_field_type arginfo_pg_field_name

#define arginfo_pg_fieldtype arginfo_pg_field_name

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_field_type_oid, 0, 2, MAY_BE_STRING|MAY_BE_LONG)
	ZEND_ARG_OBJ_INFO(0, result, PgSql\\Result, 0)
	ZEND_ARG_TYPE_INFO(0, field, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_field_num, 0, 2, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, result, PgSql\\Result, 0)
	ZEND_ARG_TYPE_INFO(0, field, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_pg_fieldnum arginfo_pg_field_num

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_fetch_result, 0, 2, MAY_BE_STRING|MAY_BE_FALSE|MAY_BE_NULL)
	ZEND_ARG_OBJ_INFO(0, result, PgSql\\Result, 0)
	ZEND_ARG_INFO(0, row)
	ZEND_ARG_TYPE_MASK(0, field, MAY_BE_STRING|MAY_BE_LONG, NULL)
ZEND_END_ARG_INFO()

#define arginfo_pg_result arginfo_pg_fetch_result

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_fetch_row, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, result, PgSql\\Result, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, row, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "PGSQL_NUM")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_fetch_assoc, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, result, PgSql\\Result, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, row, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_fetch_array, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, result, PgSql\\Result, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, row, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "PGSQL_BOTH")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_fetch_object, 0, 1, MAY_BE_OBJECT|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, result, PgSql\\Result, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, row, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, class, IS_STRING, 0, "\"stdClass\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, constructor_args, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_fetch_all, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_OBJ_INFO(0, result, PgSql\\Result, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "PGSQL_ASSOC")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_fetch_all_columns, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_OBJ_INFO(0, result, PgSql\\Result, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, field, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_result_seek, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, result, PgSql\\Result, 0)
	ZEND_ARG_TYPE_INFO(0, row, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_field_prtlen, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, result, PgSql\\Result, 0)
	ZEND_ARG_INFO(0, row)
	ZEND_ARG_TYPE_MASK(0, field, MAY_BE_STRING|MAY_BE_LONG, NULL)
ZEND_END_ARG_INFO()

#define arginfo_pg_fieldprtlen arginfo_pg_field_prtlen

#define arginfo_pg_field_is_null arginfo_pg_field_prtlen

#define arginfo_pg_fieldisnull arginfo_pg_field_prtlen

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_free_result, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, result, PgSql\\Result, 0)
ZEND_END_ARG_INFO()

#define arginfo_pg_freeresult arginfo_pg_free_result

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_last_oid, 0, 1, MAY_BE_STRING|MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, result, PgSql\\Result, 0)
ZEND_END_ARG_INFO()

#define arginfo_pg_getlastoid arginfo_pg_last_oid

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_trace, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_STRING, 0, "\"w\"")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, connection, PgSql\\Connection, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, trace_mode, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_pg_untrace arginfo_pg_close

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_lo_create, 0, 0, MAY_BE_STRING|MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, oid)
ZEND_END_ARG_INFO()

#define arginfo_pg_locreate arginfo_pg_lo_create

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_lo_unlink, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, oid)
ZEND_END_ARG_INFO()

#define arginfo_pg_lounlink arginfo_pg_lo_unlink

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_pg_lo_open, 0, 1, PgSql\\Lob, MAY_BE_FALSE)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, oid)
	ZEND_ARG_TYPE_INFO(0, mode, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_pg_loopen arginfo_pg_lo_open

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_lo_close, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, lob, PgSql\\Lob, 0)
ZEND_END_ARG_INFO()

#define arginfo_pg_loclose arginfo_pg_lo_close

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_lo_read, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, lob, PgSql\\Lob, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "8192")
ZEND_END_ARG_INFO()

#define arginfo_pg_loread arginfo_pg_lo_read

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_lo_write, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, lob, PgSql\\Lob, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_pg_lowrite arginfo_pg_lo_write

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_lo_read_all, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, lob, PgSql\\Lob, 0)
ZEND_END_ARG_INFO()

#define arginfo_pg_loreadall arginfo_pg_lo_read_all

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_lo_import, 0, 1, MAY_BE_STRING|MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, oid)
ZEND_END_ARG_INFO()

#define arginfo_pg_loimport arginfo_pg_lo_import

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_lo_export, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, oid)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

#define arginfo_pg_loexport arginfo_pg_lo_export

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_lo_seek, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, lob, PgSql\\Lob, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, whence, IS_LONG, 0, "SEEK_CUR")
ZEND_END_ARG_INFO()

#define arginfo_pg_lo_tell arginfo_pg_lo_read_all

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_lo_truncate, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, lob, PgSql\\Lob, 0)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_set_error_verbosity, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, verbosity, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_set_client_encoding, 0, 1, IS_LONG, 0)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_pg_setclientencoding arginfo_pg_set_client_encoding

#define arginfo_pg_client_encoding arginfo_pg_dbname

#define arginfo_pg_clientencoding arginfo_pg_dbname

#define arginfo_pg_end_copy arginfo_pg_ping

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_put_line, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_copy_to, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, connection, PgSql\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, table_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, separator, IS_STRING, 0, "\"\\t\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, null_as, IS_STRING, 0, "\"\\\\\\\\N\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_copy_from, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, connection, PgSql\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, table_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, rows, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, separator, IS_STRING, 0, "\"\\t\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, null_as, IS_STRING, 0, "\"\\\\\\\\N\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_escape_string, 0, 1, IS_STRING, 0)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_pg_escape_bytea arginfo_pg_escape_string

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_unescape_bytea, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_escape_literal, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_pg_escape_identifier arginfo_pg_escape_literal

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_result_error, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, result, PgSql\\Result, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_result_error_field, 0, 2, MAY_BE_STRING|MAY_BE_FALSE|MAY_BE_NULL)
	ZEND_ARG_OBJ_INFO(0, result, PgSql\\Result, 0)
	ZEND_ARG_TYPE_INFO(0, field_code, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_pg_connection_status arginfo_pg_connect_poll

#define arginfo_pg_transaction_status arginfo_pg_connect_poll

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_connection_reset, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, connection, PgSql\\Connection, 0)
ZEND_END_ARG_INFO()

#define arginfo_pg_cancel_query arginfo_pg_connection_reset

#define arginfo_pg_connection_busy arginfo_pg_connection_reset

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_send_query, 0, 2, MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_OBJ_INFO(0, connection, PgSql\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_send_query_params, 0, 3, MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_OBJ_INFO(0, connection, PgSql\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, params, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_send_prepare, 0, 3, MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_OBJ_INFO(0, connection, PgSql\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, statement_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_send_execute, 0, 3, MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_OBJ_INFO(0, connection, PgSql\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, statement_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, params, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_pg_get_result, 0, 1, PgSql\\Result, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, connection, PgSql\\Connection, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_result_status, 0, 1, MAY_BE_STRING|MAY_BE_LONG)
	ZEND_ARG_OBJ_INFO(0, result, PgSql\\Result, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "PGSQL_STATUS_LONG")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_get_notify, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, connection, PgSql\\Connection, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "PGSQL_ASSOC")
ZEND_END_ARG_INFO()

#define arginfo_pg_get_pid arginfo_pg_connect_poll

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_socket, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, connection, PgSql\\Connection, 0)
ZEND_END_ARG_INFO()

#define arginfo_pg_consume_input arginfo_pg_connection_reset

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_flush, 0, 1, MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_OBJ_INFO(0, connection, PgSql\\Connection, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_meta_data, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, connection, PgSql\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, table_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, extended, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_convert, 0, 3, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, connection, PgSql\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, table_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, values, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_pg_insert, 0, 3, PgSql\\Result, MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_OBJ_INFO(0, connection, PgSql\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, table_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, values, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "PGSQL_DML_EXEC")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_update, 0, 4, MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_OBJ_INFO(0, connection, PgSql\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, table_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, values, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, conditions, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "PGSQL_DML_EXEC")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_delete, 0, 3, MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_OBJ_INFO(0, connection, PgSql\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, table_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, conditions, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "PGSQL_DML_EXEC")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_select, 0, 3, MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, connection, PgSql\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, table_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, conditions, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "PGSQL_DML_EXEC")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "PGSQL_ASSOC")
ZEND_END_ARG_INFO()

#if defined(HAVE_PG_CONTEXT_VISIBILITY)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_set_error_context_visibility, 0, 2, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, connection, PgSql\\Connection, 0)
	ZEND_ARG_TYPE_INFO(0, visibility, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif


ZEND_FUNCTION(pg_connect);
ZEND_FUNCTION(pg_pconnect);
ZEND_FUNCTION(pg_connect_poll);
ZEND_FUNCTION(pg_close);
ZEND_FUNCTION(pg_dbname);
ZEND_FUNCTION(pg_last_error);
ZEND_FUNCTION(pg_options);
ZEND_FUNCTION(pg_port);
ZEND_FUNCTION(pg_tty);
ZEND_FUNCTION(pg_host);
ZEND_FUNCTION(pg_version);
ZEND_FUNCTION(pg_parameter_status);
ZEND_FUNCTION(pg_ping);
ZEND_FUNCTION(pg_query);
ZEND_FUNCTION(pg_query_params);
ZEND_FUNCTION(pg_prepare);
ZEND_FUNCTION(pg_execute);
ZEND_FUNCTION(pg_num_rows);
ZEND_FUNCTION(pg_num_fields);
ZEND_FUNCTION(pg_affected_rows);
ZEND_FUNCTION(pg_last_notice);
ZEND_FUNCTION(pg_field_table);
ZEND_FUNCTION(pg_field_name);
ZEND_FUNCTION(pg_field_size);
ZEND_FUNCTION(pg_field_type);
ZEND_FUNCTION(pg_field_type_oid);
ZEND_FUNCTION(pg_field_num);
ZEND_FUNCTION(pg_fetch_result);
ZEND_FUNCTION(pg_fetch_row);
ZEND_FUNCTION(pg_fetch_assoc);
ZEND_FUNCTION(pg_fetch_array);
ZEND_FUNCTION(pg_fetch_object);
ZEND_FUNCTION(pg_fetch_all);
ZEND_FUNCTION(pg_fetch_all_columns);
ZEND_FUNCTION(pg_result_seek);
ZEND_FUNCTION(pg_field_prtlen);
ZEND_FUNCTION(pg_fieldprtlen);
ZEND_FUNCTION(pg_field_is_null);
ZEND_FUNCTION(pg_fieldisnull);
ZEND_FUNCTION(pg_free_result);
ZEND_FUNCTION(pg_last_oid);
ZEND_FUNCTION(pg_trace);
ZEND_FUNCTION(pg_untrace);
ZEND_FUNCTION(pg_lo_create);
ZEND_FUNCTION(pg_lo_unlink);
ZEND_FUNCTION(pg_lo_open);
ZEND_FUNCTION(pg_lo_close);
ZEND_FUNCTION(pg_lo_read);
ZEND_FUNCTION(pg_lo_write);
ZEND_FUNCTION(pg_lo_read_all);
ZEND_FUNCTION(pg_lo_import);
ZEND_FUNCTION(pg_lo_export);
ZEND_FUNCTION(pg_lo_seek);
ZEND_FUNCTION(pg_lo_tell);
ZEND_FUNCTION(pg_lo_truncate);
ZEND_FUNCTION(pg_set_error_verbosity);
ZEND_FUNCTION(pg_set_client_encoding);
ZEND_FUNCTION(pg_client_encoding);
ZEND_FUNCTION(pg_end_copy);
ZEND_FUNCTION(pg_put_line);
ZEND_FUNCTION(pg_copy_to);
ZEND_FUNCTION(pg_copy_from);
ZEND_FUNCTION(pg_escape_string);
ZEND_FUNCTION(pg_escape_bytea);
ZEND_FUNCTION(pg_unescape_bytea);
ZEND_FUNCTION(pg_escape_literal);
ZEND_FUNCTION(pg_escape_identifier);
ZEND_FUNCTION(pg_result_error);
ZEND_FUNCTION(pg_result_error_field);
ZEND_FUNCTION(pg_connection_status);
ZEND_FUNCTION(pg_transaction_status);
ZEND_FUNCTION(pg_connection_reset);
ZEND_FUNCTION(pg_cancel_query);
ZEND_FUNCTION(pg_connection_busy);
ZEND_FUNCTION(pg_send_query);
ZEND_FUNCTION(pg_send_query_params);
ZEND_FUNCTION(pg_send_prepare);
ZEND_FUNCTION(pg_send_execute);
ZEND_FUNCTION(pg_get_result);
ZEND_FUNCTION(pg_result_status);
ZEND_FUNCTION(pg_get_notify);
ZEND_FUNCTION(pg_get_pid);
ZEND_FUNCTION(pg_socket);
ZEND_FUNCTION(pg_consume_input);
ZEND_FUNCTION(pg_flush);
ZEND_FUNCTION(pg_meta_data);
ZEND_FUNCTION(pg_convert);
ZEND_FUNCTION(pg_insert);
ZEND_FUNCTION(pg_update);
ZEND_FUNCTION(pg_delete);
ZEND_FUNCTION(pg_select);
#if defined(HAVE_PG_CONTEXT_VISIBILITY)
ZEND_FUNCTION(pg_set_error_context_visibility);
#endif


static const zend_function_entry ext_functions[] = {
	ZEND_FE(pg_connect, arginfo_pg_connect)
	ZEND_FE(pg_pconnect, arginfo_pg_pconnect)
	ZEND_FE(pg_connect_poll, arginfo_pg_connect_poll)
	ZEND_FE(pg_close, arginfo_pg_close)
	ZEND_FE(pg_dbname, arginfo_pg_dbname)
	ZEND_FE(pg_last_error, arginfo_pg_last_error)
	ZEND_DEP_FALIAS(pg_errormessage, pg_last_error, arginfo_pg_errormessage)
	ZEND_FE(pg_options, arginfo_pg_options)
	ZEND_FE(pg_port, arginfo_pg_port)
	ZEND_FE(pg_tty, arginfo_pg_tty)
	ZEND_FE(pg_host, arginfo_pg_host)
	ZEND_FE(pg_version, arginfo_pg_version)
	ZEND_FE(pg_parameter_status, arginfo_pg_parameter_status)
	ZEND_FE(pg_ping, arginfo_pg_ping)
	ZEND_FE(pg_query, arginfo_pg_query)
	ZEND_FALIAS(pg_exec, pg_query, arginfo_pg_exec)
	ZEND_FE(pg_query_params, arginfo_pg_query_params)
	ZEND_FE(pg_prepare, arginfo_pg_prepare)
	ZEND_FE(pg_execute, arginfo_pg_execute)
	ZEND_FE(pg_num_rows, arginfo_pg_num_rows)
	ZEND_DEP_FALIAS(pg_numrows, pg_num_rows, arginfo_pg_numrows)
	ZEND_FE(pg_num_fields, arginfo_pg_num_fields)
	ZEND_DEP_FALIAS(pg_numfields, pg_num_fields, arginfo_pg_numfields)
	ZEND_FE(pg_affected_rows, arginfo_pg_affected_rows)
	ZEND_DEP_FALIAS(pg_cmdtuples, pg_affected_rows, arginfo_pg_cmdtuples)
	ZEND_FE(pg_last_notice, arginfo_pg_last_notice)
	ZEND_FE(pg_field_table, arginfo_pg_field_table)
	ZEND_FE(pg_field_name, arginfo_pg_field_name)
	ZEND_DEP_FALIAS(pg_fieldname, pg_field_name, arginfo_pg_fieldname)
	ZEND_FE(pg_field_size, arginfo_pg_field_size)
	ZEND_DEP_FALIAS(pg_fieldsize, pg_field_size, arginfo_pg_fieldsize)
	ZEND_FE(pg_field_type, arginfo_pg_field_type)
	ZEND_DEP_FALIAS(pg_fieldtype, pg_field_type, arginfo_pg_fieldtype)
	ZEND_FE(pg_field_type_oid, arginfo_pg_field_type_oid)
	ZEND_FE(pg_field_num, arginfo_pg_field_num)
	ZEND_DEP_FALIAS(pg_fieldnum, pg_field_num, arginfo_pg_fieldnum)
	ZEND_FE(pg_fetch_result, arginfo_pg_fetch_result)
	ZEND_DEP_FALIAS(pg_result, pg_fetch_result, arginfo_pg_result)
	ZEND_FE(pg_fetch_row, arginfo_pg_fetch_row)
	ZEND_FE(pg_fetch_assoc, arginfo_pg_fetch_assoc)
	ZEND_FE(pg_fetch_array, arginfo_pg_fetch_array)
	ZEND_FE(pg_fetch_object, arginfo_pg_fetch_object)
	ZEND_FE(pg_fetch_all, arginfo_pg_fetch_all)
	ZEND_FE(pg_fetch_all_columns, arginfo_pg_fetch_all_columns)
	ZEND_FE(pg_result_seek, arginfo_pg_result_seek)
	ZEND_FE(pg_field_prtlen, arginfo_pg_field_prtlen)
	ZEND_DEP_FE(pg_fieldprtlen, arginfo_pg_fieldprtlen)
	ZEND_FE(pg_field_is_null, arginfo_pg_field_is_null)
	ZEND_DEP_FE(pg_fieldisnull, arginfo_pg_fieldisnull)
	ZEND_FE(pg_free_result, arginfo_pg_free_result)
	ZEND_DEP_FALIAS(pg_freeresult, pg_free_result, arginfo_pg_freeresult)
	ZEND_FE(pg_last_oid, arginfo_pg_last_oid)
	ZEND_DEP_FALIAS(pg_getlastoid, pg_last_oid, arginfo_pg_getlastoid)
	ZEND_FE(pg_trace, arginfo_pg_trace)
	ZEND_FE(pg_untrace, arginfo_pg_untrace)
	ZEND_FE(pg_lo_create, arginfo_pg_lo_create)
	ZEND_DEP_FALIAS(pg_locreate, pg_lo_create, arginfo_pg_locreate)
	ZEND_FE(pg_lo_unlink, arginfo_pg_lo_unlink)
	ZEND_DEP_FALIAS(pg_lounlink, pg_lo_unlink, arginfo_pg_lounlink)
	ZEND_FE(pg_lo_open, arginfo_pg_lo_open)
	ZEND_DEP_FALIAS(pg_loopen, pg_lo_open, arginfo_pg_loopen)
	ZEND_FE(pg_lo_close, arginfo_pg_lo_close)
	ZEND_DEP_FALIAS(pg_loclose, pg_lo_close, arginfo_pg_loclose)
	ZEND_FE(pg_lo_read, arginfo_pg_lo_read)
	ZEND_DEP_FALIAS(pg_loread, pg_lo_read, arginfo_pg_loread)
	ZEND_FE(pg_lo_write, arginfo_pg_lo_write)
	ZEND_DEP_FALIAS(pg_lowrite, pg_lo_write, arginfo_pg_lowrite)
	ZEND_FE(pg_lo_read_all, arginfo_pg_lo_read_all)
	ZEND_DEP_FALIAS(pg_loreadall, pg_lo_read_all, arginfo_pg_loreadall)
	ZEND_FE(pg_lo_import, arginfo_pg_lo_import)
	ZEND_DEP_FALIAS(pg_loimport, pg_lo_import, arginfo_pg_loimport)
	ZEND_FE(pg_lo_export, arginfo_pg_lo_export)
	ZEND_DEP_FALIAS(pg_loexport, pg_lo_export, arginfo_pg_loexport)
	ZEND_FE(pg_lo_seek, arginfo_pg_lo_seek)
	ZEND_FE(pg_lo_tell, arginfo_pg_lo_tell)
	ZEND_FE(pg_lo_truncate, arginfo_pg_lo_truncate)
	ZEND_FE(pg_set_error_verbosity, arginfo_pg_set_error_verbosity)
	ZEND_FE(pg_set_client_encoding, arginfo_pg_set_client_encoding)
	ZEND_DEP_FALIAS(pg_setclientencoding, pg_set_client_encoding, arginfo_pg_setclientencoding)
	ZEND_FE(pg_client_encoding, arginfo_pg_client_encoding)
	ZEND_DEP_FALIAS(pg_clientencoding, pg_client_encoding, arginfo_pg_clientencoding)
	ZEND_FE(pg_end_copy, arginfo_pg_end_copy)
	ZEND_FE(pg_put_line, arginfo_pg_put_line)
	ZEND_FE(pg_copy_to, arginfo_pg_copy_to)
	ZEND_FE(pg_copy_from, arginfo_pg_copy_from)
	ZEND_FE(pg_escape_string, arginfo_pg_escape_string)
	ZEND_FE(pg_escape_bytea, arginfo_pg_escape_bytea)
	ZEND_FE(pg_unescape_bytea, arginfo_pg_unescape_bytea)
	ZEND_FE(pg_escape_literal, arginfo_pg_escape_literal)
	ZEND_FE(pg_escape_identifier, arginfo_pg_escape_identifier)
	ZEND_FE(pg_result_error, arginfo_pg_result_error)
	ZEND_FE(pg_result_error_field, arginfo_pg_result_error_field)
	ZEND_FE(pg_connection_status, arginfo_pg_connection_status)
	ZEND_FE(pg_transaction_status, arginfo_pg_transaction_status)
	ZEND_FE(pg_connection_reset, arginfo_pg_connection_reset)
	ZEND_FE(pg_cancel_query, arginfo_pg_cancel_query)
	ZEND_FE(pg_connection_busy, arginfo_pg_connection_busy)
	ZEND_FE(pg_send_query, arginfo_pg_send_query)
	ZEND_FE(pg_send_query_params, arginfo_pg_send_query_params)
	ZEND_FE(pg_send_prepare, arginfo_pg_send_prepare)
	ZEND_FE(pg_send_execute, arginfo_pg_send_execute)
	ZEND_FE(pg_get_result, arginfo_pg_get_result)
	ZEND_FE(pg_result_status, arginfo_pg_result_status)
	ZEND_FE(pg_get_notify, arginfo_pg_get_notify)
	ZEND_FE(pg_get_pid, arginfo_pg_get_pid)
	ZEND_FE(pg_socket, arginfo_pg_socket)
	ZEND_FE(pg_consume_input, arginfo_pg_consume_input)
	ZEND_FE(pg_flush, arginfo_pg_flush)
	ZEND_FE(pg_meta_data, arginfo_pg_meta_data)
	ZEND_FE(pg_convert, arginfo_pg_convert)
	ZEND_FE(pg_insert, arginfo_pg_insert)
	ZEND_FE(pg_update, arginfo_pg_update)
	ZEND_FE(pg_delete, arginfo_pg_delete)
	ZEND_FE(pg_select, arginfo_pg_select)
#if defined(HAVE_PG_CONTEXT_VISIBILITY)
	ZEND_FE(pg_set_error_context_visibility, arginfo_pg_set_error_context_visibility)
#endif
	ZEND_FE_END
};


static const zend_function_entry class_PgSql_Connection_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_PgSql_Result_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_PgSql_Lob_methods[] = {
	ZEND_FE_END
};

static void register_pgsql_symbols(int module_number)
{
	REGISTER_STRING_CONSTANT("PGSQL_LIBPQ_VERSION", pgsql_libpq_version, CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("PGSQL_LIBPQ_VERSION_STR", pgsql_libpq_version, CONST_PERSISTENT | CONST_DEPRECATED);
	REGISTER_LONG_CONSTANT("PGSQL_CONNECT_FORCE_NEW", PGSQL_CONNECT_FORCE_NEW, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_CONNECT_ASYNC", PGSQL_CONNECT_ASYNC, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_ASSOC", PGSQL_ASSOC, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_NUM", PGSQL_NUM, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_BOTH", PGSQL_BOTH, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_NOTICE_LAST", PGSQL_NOTICE_LAST, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_NOTICE_ALL", PGSQL_NOTICE_ALL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_NOTICE_CLEAR", PGSQL_NOTICE_CLEAR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_CONNECTION_BAD", CONNECTION_BAD, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_CONNECTION_OK", CONNECTION_OK, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_CONNECTION_STARTED", CONNECTION_STARTED, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_CONNECTION_MADE", CONNECTION_MADE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_CONNECTION_AWAITING_RESPONSE", CONNECTION_AWAITING_RESPONSE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_CONNECTION_AUTH_OK", CONNECTION_AUTH_OK, CONST_PERSISTENT);
#if defined(CONNECTION_SSL_STARTUP)
	REGISTER_LONG_CONSTANT("PGSQL_CONNECTION_SSL_STARTUP", CONNECTION_SSL_STARTUP, CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("PGSQL_CONNECTION_SETENV", CONNECTION_SETENV, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_POLLING_FAILED", PGRES_POLLING_FAILED, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_POLLING_READING", PGRES_POLLING_READING, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_POLLING_WRITING", PGRES_POLLING_WRITING, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_POLLING_OK", PGRES_POLLING_OK, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_POLLING_ACTIVE", PGRES_POLLING_ACTIVE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_TRANSACTION_IDLE", PQTRANS_IDLE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_TRANSACTION_ACTIVE", PQTRANS_ACTIVE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_TRANSACTION_INTRANS", PQTRANS_INTRANS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_TRANSACTION_INERROR", PQTRANS_INERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_TRANSACTION_UNKNOWN", PQTRANS_UNKNOWN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_ERRORS_TERSE", PQERRORS_TERSE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_ERRORS_DEFAULT", PQERRORS_DEFAULT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_ERRORS_VERBOSE", PQERRORS_VERBOSE, CONST_PERSISTENT);
#if PGVERSION_NUM > 110000
	REGISTER_LONG_CONSTANT("PGSQL_ERRORS_SQLSTATE", PQERRORS_SQLSTATE, CONST_PERSISTENT);
#endif
#if !(PGVERSION_NUM > 110000)
	REGISTER_LONG_CONSTANT("PGSQL_ERRORS_SQLSTATE", PQERRORS_TERSE, CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("PGSQL_SEEK_SET", SEEK_SET, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_SEEK_CUR", SEEK_CUR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_SEEK_END", SEEK_END, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_STATUS_LONG", PGSQL_STATUS_LONG, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_STATUS_STRING", PGSQL_STATUS_STRING, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_EMPTY_QUERY", PGRES_EMPTY_QUERY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_COMMAND_OK", PGRES_COMMAND_OK, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_TUPLES_OK", PGRES_TUPLES_OK, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_COPY_OUT", PGRES_COPY_OUT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_COPY_IN", PGRES_COPY_IN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_BAD_RESPONSE", PGRES_BAD_RESPONSE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_NONFATAL_ERROR", PGRES_NONFATAL_ERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_FATAL_ERROR", PGRES_FATAL_ERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_SEVERITY", PG_DIAG_SEVERITY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_SQLSTATE", PG_DIAG_SQLSTATE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_MESSAGE_PRIMARY", PG_DIAG_MESSAGE_PRIMARY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_MESSAGE_DETAIL", PG_DIAG_MESSAGE_DETAIL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_MESSAGE_HINT", PG_DIAG_MESSAGE_HINT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_STATEMENT_POSITION", PG_DIAG_STATEMENT_POSITION, CONST_PERSISTENT);
#if defined(PG_DIAG_INTERNAL_POSITION)
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_INTERNAL_POSITION", PG_DIAG_INTERNAL_POSITION, CONST_PERSISTENT);
#endif
#if defined(PG_DIAG_INTERNAL_QUERY)
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_INTERNAL_QUERY", PG_DIAG_INTERNAL_QUERY, CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_CONTEXT", PG_DIAG_CONTEXT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_SOURCE_FILE", PG_DIAG_SOURCE_FILE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_SOURCE_LINE", PG_DIAG_SOURCE_LINE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_SOURCE_FUNCTION", PG_DIAG_SOURCE_FUNCTION, CONST_PERSISTENT);
#if defined(PG_DIAG_SCHEMA_NAME)
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_SCHEMA_NAME", PG_DIAG_SCHEMA_NAME, CONST_PERSISTENT);
#endif
#if defined(PG_DIAG_TABLE_NAME)
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_TABLE_NAME", PG_DIAG_TABLE_NAME, CONST_PERSISTENT);
#endif
#if defined(PG_DIAG_COLUMN_NAME)
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_COLUMN_NAME", PG_DIAG_COLUMN_NAME, CONST_PERSISTENT);
#endif
#if defined(PG_DIAG_DATATYPE_NAME)
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_DATATYPE_NAME", PG_DIAG_DATATYPE_NAME, CONST_PERSISTENT);
#endif
#if defined(PG_DIAG_CONSTRAINT_NAME)
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_CONSTRAINT_NAME", PG_DIAG_CONSTRAINT_NAME, CONST_PERSISTENT);
#endif
#if defined(PG_DIAG_SEVERITY_NONLOCALIZED)
	REGISTER_LONG_CONSTANT("PGSQL_DIAG_SEVERITY_NONLOCALIZED", PG_DIAG_SEVERITY_NONLOCALIZED, CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("PGSQL_CONV_IGNORE_DEFAULT", PGSQL_CONV_IGNORE_DEFAULT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_CONV_FORCE_NULL", PGSQL_CONV_FORCE_NULL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_CONV_IGNORE_NOT_NULL", PGSQL_CONV_IGNORE_NOT_NULL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DML_ESCAPE", PGSQL_DML_ESCAPE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DML_NO_CONV", PGSQL_DML_NO_CONV, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DML_EXEC", PGSQL_DML_EXEC, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DML_ASYNC", PGSQL_DML_ASYNC, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PGSQL_DML_STRING", PGSQL_DML_STRING, CONST_PERSISTENT);
#if defined(PQTRACE_SUPPPRESS_TIMESTAMPS)
	REGISTER_LONG_CONSTANT("PGSQL_TRACE_SUPPRESS_TIMESTAMPS", PQTRACE_SUPPRESS_TIMESTAMPS, CONST_PERSISTENT);
#endif
#if defined(PQTRACE_REGRESS_MODE)
	REGISTER_LONG_CONSTANT("PGSQL_TRACE_REGRESS_MODE", PQTRACE_REGRESS_MODE, CONST_PERSISTENT);
#endif
#if defined(HAVE_PG_CONTEXT_VISIBILITY)
	REGISTER_LONG_CONSTANT("PGSQL_SHOW_CONTEXT_NEVER", PQSHOW_CONTEXT_NEVER, CONST_PERSISTENT);
#endif
#if defined(HAVE_PG_CONTEXT_VISIBILITY)
	REGISTER_LONG_CONSTANT("PGSQL_SHOW_CONTEXT_ERRORS", PQSHOW_CONTEXT_ERRORS, CONST_PERSISTENT);
#endif
#if defined(HAVE_PG_CONTEXT_VISIBILITY)
	REGISTER_LONG_CONSTANT("PGSQL_SHOW_CONTEXT_ALWAYS", PQSHOW_CONTEXT_ALWAYS, CONST_PERSISTENT);
#endif
}

static zend_class_entry *register_class_PgSql_Connection(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "PgSql", "Connection", class_PgSql_Connection_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}

static zend_class_entry *register_class_PgSql_Result(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "PgSql", "Result", class_PgSql_Result_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}

static zend_class_entry *register_class_PgSql_Lob(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "PgSql", "Lob", class_PgSql_Lob_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}
