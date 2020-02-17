/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_odbc_close_all, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_odbc_binmode, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, result_id)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_odbc_longreadlen, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, result_id)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_prepare, 0, 0, 2)
	ZEND_ARG_INFO(0, connection_id)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_odbc_execute, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, result_id)
	ZEND_ARG_TYPE_INFO(0, parameters_array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_odbc_cursor, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, result_id)
ZEND_END_ARG_INFO()

#if defined(HAVE_SQLDATASOURCES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_odbc_data_source, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, connection_id)
	ZEND_ARG_TYPE_INFO(0, fetch_type, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_exec, 0, 0, 2)
	ZEND_ARG_INFO(0, connection_id)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_odbc_do arginfo_odbc_exec

#if defined(PHP_ODBC_HAVE_FETCH_HASH)
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_odbc_fetch_object, 0, 1, stdClass, MAY_BE_FALSE)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_TYPE_INFO(0, rownumber, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(PHP_ODBC_HAVE_FETCH_HASH)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_odbc_fetch_array, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_TYPE_INFO(0, rownumber, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_odbc_fetch_into, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, result_id)
	ZEND_ARG_INFO(1, result_array)
	ZEND_ARG_TYPE_INFO(0, rownumber, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_odbc_fetch_row, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, result_id)
	ZEND_ARG_TYPE_INFO(0, row_number, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_odbc_result, 0, 2, MAY_BE_STRING|MAY_BE_BOOL|MAY_BE_NULL)
	ZEND_ARG_INFO(0, result_id)
	ZEND_ARG_INFO(0, field)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_odbc_result_all, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, result_id)
	ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_odbc_free_result, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, result_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_connect, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, dsn, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, user, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, cursor_option, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_odbc_pconnect arginfo_odbc_connect

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_odbc_close, 0, 1, IS_VOID, 0)
	ZEND_ARG_INFO(0, connection_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_odbc_num_rows, 0, 1, IS_LONG, 0)
	ZEND_ARG_INFO(0, result_id)
ZEND_END_ARG_INFO()

#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_odbc_next_result, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, result_id)
ZEND_END_ARG_INFO()
#endif

#define arginfo_odbc_num_fields arginfo_odbc_num_rows

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_odbc_field_name, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, result_id)
	ZEND_ARG_TYPE_INFO(0, field_number, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_odbc_field_type arginfo_odbc_field_name

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_odbc_field_len, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, result_id)
	ZEND_ARG_TYPE_INFO(0, field_number, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_odbc_field_precision arginfo_odbc_field_len

#define arginfo_odbc_field_scale arginfo_odbc_field_len

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_odbc_field_num, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, result_id)
	ZEND_ARG_TYPE_INFO(0, field_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_odbc_autocommit, 0, 1, MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_INFO(0, connection_id)
	ZEND_ARG_TYPE_INFO(0, onoff, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_odbc_commit, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, connection_id)
ZEND_END_ARG_INFO()

#define arginfo_odbc_rollback arginfo_odbc_commit

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_odbc_error, 0, 0, IS_STRING, 0)
	ZEND_ARG_INFO(0, connection_id)
ZEND_END_ARG_INFO()

#define arginfo_odbc_errormsg arginfo_odbc_error

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_odbc_setoption, 0, 4, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, conn_id)
	ZEND_ARG_TYPE_INFO(0, which, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_tables, 0, 0, 1)
	ZEND_ARG_INFO(0, connection_id)
	ZEND_ARG_TYPE_INFO(0, qualfier, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, owner, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, table_types, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_columns, 0, 0, 1)
	ZEND_ARG_INFO(0, connection_id)
	ZEND_ARG_TYPE_INFO(0, qualifier, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, owner, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, table_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, column_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_gettypeinfo, 0, 0, 1)
	ZEND_ARG_INFO(0, connection_id)
	ZEND_ARG_TYPE_INFO(0, data_type, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_primarykeys, 0, 0, 4)
	ZEND_ARG_INFO(0, connection_id)
	ZEND_ARG_TYPE_INFO(0, qualifier, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, owner, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, table, IS_STRING, 0)
ZEND_END_ARG_INFO()

#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35)
ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_procedurecolumns, 0, 0, 1)
	ZEND_ARG_INFO(0, connection_id)
	ZEND_ARG_TYPE_INFO(0, qualifier, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, owner, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, proc, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, column, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35)
ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_procedures, 0, 0, 1)
	ZEND_ARG_INFO(0, connection_id)
	ZEND_ARG_TYPE_INFO(0, qualifier, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, owner, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35)
ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_foreignkeys, 0, 0, 7)
	ZEND_ARG_INFO(0, connection_id)
	ZEND_ARG_TYPE_INFO(0, pk_qualifier, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, pk_owner, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, pk_table, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, fk_qualifier, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, fk_owner, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, fk_table, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_specialcolumns, 0, 0, 6)
	ZEND_ARG_INFO(0, connection_id)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, qualifier, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, owner, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, table, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, scope, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_statistics, 0, 0, 6)
	ZEND_ARG_INFO(0, connection_id)
	ZEND_ARG_TYPE_INFO(0, qualfier, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, owner, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, unique, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, accuracy, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if !defined(HAVE_DBMAKER) && !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) &&!defined(HAVE_SOLID_35)
ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_tableprivileges, 0, 0, 4)
	ZEND_ARG_INFO(0, connection_id)
	ZEND_ARG_TYPE_INFO(0, qualifier, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, owner, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if !defined(HAVE_DBMAKER) && !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) &&!defined(HAVE_SOLID_35)
ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_columnprivileges, 0, 0, 5)
	ZEND_ARG_INFO(0, connection_id)
	ZEND_ARG_TYPE_INFO(0, catalog, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, schema, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, table, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, column, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif
