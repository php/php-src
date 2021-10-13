/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 72a1beeb327cd6fdb135ed8de8c6fc1372f76303 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_connect, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, connection_string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_pg_pconnect arginfo_pg_connect

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_connect_poll, 0, 1, IS_LONG, 0)
	ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_close, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, connection, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_dbname, 0, 0, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, connection, "null")
ZEND_END_ARG_INFO()

#define arginfo_pg_last_error arginfo_pg_dbname

#define arginfo_pg_errormessage arginfo_pg_dbname

#define arginfo_pg_options arginfo_pg_dbname

#define arginfo_pg_port arginfo_pg_dbname

#define arginfo_pg_tty arginfo_pg_dbname

#define arginfo_pg_host arginfo_pg_dbname

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_version, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, connection, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_parameter_status, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_pg_ping arginfo_pg_close

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_query, 0, 0, 1)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_pg_exec arginfo_pg_query

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_query_params, 0, 0, 2)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, query)
	ZEND_ARG_TYPE_INFO(0, params, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_prepare, 0, 0, 2)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, statement_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_execute, 0, 0, 2)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, statement_name)
	ZEND_ARG_TYPE_INFO(0, params, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_num_rows, 0, 1, IS_LONG, 0)
	ZEND_ARG_INFO(0, result)
ZEND_END_ARG_INFO()

#define arginfo_pg_numrows arginfo_pg_num_rows

#define arginfo_pg_num_fields arginfo_pg_num_rows

#define arginfo_pg_numfields arginfo_pg_num_rows

#define arginfo_pg_affected_rows arginfo_pg_num_rows

#define arginfo_pg_cmdtuples arginfo_pg_num_rows

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_last_notice, 0, 1, MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "PGSQL_NOTICE_LAST")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_field_table, 0, 2, MAY_BE_STRING|MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_TYPE_INFO(0, field, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, oid_only, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_field_name, 0, 2, IS_STRING, 0)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_TYPE_INFO(0, field, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_pg_fieldname arginfo_pg_field_name

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_field_size, 0, 2, IS_LONG, 0)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_TYPE_INFO(0, field, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_pg_fieldsize arginfo_pg_field_size

#define arginfo_pg_field_type arginfo_pg_field_name

#define arginfo_pg_fieldtype arginfo_pg_field_name

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_field_type_oid, 0, 2, MAY_BE_STRING|MAY_BE_LONG)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_TYPE_INFO(0, field, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_field_num, 0, 2, IS_LONG, 0)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_TYPE_INFO(0, field, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_pg_fieldnum arginfo_pg_field_num

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_fetch_result, 0, 2, MAY_BE_STRING|MAY_BE_FALSE|MAY_BE_NULL)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, row)
	ZEND_ARG_TYPE_MASK(0, field, MAY_BE_STRING|MAY_BE_LONG, NULL)
ZEND_END_ARG_INFO()

#define arginfo_pg_result arginfo_pg_fetch_result

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_fetch_row, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, row, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "PGSQL_NUM")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_fetch_assoc, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, row, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_fetch_array, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, row, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "PGSQL_BOTH")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_fetch_object, 0, 1, MAY_BE_OBJECT|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, row, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, class, IS_STRING, 0, "\"stdClass\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, constructor_args, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_fetch_all, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "PGSQL_ASSOC")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_fetch_all_columns, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, field, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_result_seek, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_TYPE_INFO(0, row, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_field_prtlen, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(0, row)
	ZEND_ARG_TYPE_MASK(0, field, MAY_BE_STRING|MAY_BE_LONG, NULL)
ZEND_END_ARG_INFO()

#define arginfo_pg_fieldprtlen arginfo_pg_field_prtlen

#define arginfo_pg_field_is_null arginfo_pg_field_prtlen

#define arginfo_pg_fieldisnull arginfo_pg_field_prtlen

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_free_result, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, result)
ZEND_END_ARG_INFO()

#define arginfo_pg_freeresult arginfo_pg_free_result

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_last_oid, 0, 1, MAY_BE_STRING|MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, result)
ZEND_END_ARG_INFO()

#define arginfo_pg_getlastoid arginfo_pg_last_oid

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_trace, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_STRING, 0, "\"w\"")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, connection, "null")
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_lo_open, 0, 0, 1)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_INFO(0, oid)
	ZEND_ARG_TYPE_INFO(0, mode, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_pg_loopen arginfo_pg_lo_open

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_lo_close, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, lob)
ZEND_END_ARG_INFO()

#define arginfo_pg_loclose arginfo_pg_lo_close

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_lo_read, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, lob)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "8192")
ZEND_END_ARG_INFO()

#define arginfo_pg_loread arginfo_pg_lo_read

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_lo_write, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, lob)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_pg_lowrite arginfo_pg_lo_write

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_lo_read_all, 0, 1, IS_LONG, 0)
	ZEND_ARG_INFO(0, lob)
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
	ZEND_ARG_INFO(0, lob)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, whence, IS_LONG, 0, "SEEK_CUR")
ZEND_END_ARG_INFO()

#define arginfo_pg_lo_tell arginfo_pg_lo_read_all

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_lo_truncate, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, lob)
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

#define arginfo_pg_end_copy arginfo_pg_close

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_put_line, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_copy_to, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, table_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, separator, IS_STRING, 0, "\"\\t\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, null_as, IS_STRING, 0, "\"\\\\\\\\N\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_copy_from, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, connection)
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
	ZEND_ARG_INFO(0, result)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_result_error_field, 0, 2, MAY_BE_STRING|MAY_BE_FALSE|MAY_BE_NULL)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_TYPE_INFO(0, field_code, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_pg_connection_status arginfo_pg_connect_poll

#define arginfo_pg_transaction_status arginfo_pg_connect_poll

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pg_connection_reset, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

#define arginfo_pg_cancel_query arginfo_pg_connection_reset

#define arginfo_pg_connection_busy arginfo_pg_connection_reset

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_send_query, 0, 2, MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_send_query_params, 0, 3, MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, params, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_send_prepare, 0, 3, MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, statement_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_send_execute, 0, 3, MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, statement_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, params, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_get_result, 0, 0, 1)
	ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_result_status, 0, 1, MAY_BE_STRING|MAY_BE_LONG)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "PGSQL_STATUS_LONG")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_get_notify, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "PGSQL_ASSOC")
ZEND_END_ARG_INFO()

#define arginfo_pg_get_pid arginfo_pg_connect_poll

#define arginfo_pg_socket arginfo_pg_get_result

#define arginfo_pg_consume_input arginfo_pg_connection_reset

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_flush, 0, 1, MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_meta_data, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, table_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, extended, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_convert, 0, 3, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, table_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, values, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pg_insert, 0, 0, 3)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, table_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, values, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "PGSQL_DML_EXEC")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_update, 0, 4, MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, table_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, values, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, conditions, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "PGSQL_DML_EXEC")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_delete, 0, 3, MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, table_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, conditions, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "PGSQL_DML_EXEC")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pg_select, 0, 3, MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, connection)
	ZEND_ARG_TYPE_INFO(0, table_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, conditions, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "PGSQL_DML_EXEC")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "PGSQL_ASSOC")
ZEND_END_ARG_INFO()


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
ZEND_FUNCTION(pg_field_is_null);
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
	ZEND_DEP_FALIAS(pg_fieldprtlen, pg_field_prtlen, arginfo_pg_fieldprtlen)
	ZEND_FE(pg_field_is_null, arginfo_pg_field_is_null)
	ZEND_DEP_FALIAS(pg_fieldisnull, pg_field_is_null, arginfo_pg_fieldisnull)
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
	ZEND_FE_END
};
