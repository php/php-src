/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: ff33935d54ae09f494fe957ca0b86d6c5c8bcab2 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_odbc_close_all, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_odbc_binmode, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, statement)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_odbc_longreadlen, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, statement)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_prepare, 0, 0, 2)
	ZEND_ARG_INFO(0, odbc)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_odbc_execute, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, statement)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, params, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_odbc_cursor, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, statement)
ZEND_END_ARG_INFO()

#if defined(HAVE_SQLDATASOURCES)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_odbc_data_source, 0, 2, MAY_BE_ARRAY|MAY_BE_NULL|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, odbc)
	ZEND_ARG_TYPE_INFO(0, fetch_type, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#define arginfo_odbc_exec arginfo_odbc_prepare

#define arginfo_odbc_do arginfo_odbc_prepare

#if defined(PHP_ODBC_HAVE_FETCH_HASH)
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_odbc_fetch_object, 0, 1, stdClass, MAY_BE_FALSE)
	ZEND_ARG_INFO(0, statement)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, row, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()
#endif

#if defined(PHP_ODBC_HAVE_FETCH_HASH)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_odbc_fetch_array, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, statement)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, row, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_odbc_fetch_into, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, statement)
	ZEND_ARG_INFO(1, array)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, row, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_odbc_fetch_row, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, statement)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, row, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_odbc_result, 0, 2, MAY_BE_STRING|MAY_BE_BOOL|MAY_BE_NULL)
	ZEND_ARG_INFO(0, statement)
	ZEND_ARG_TYPE_MASK(0, field, MAY_BE_STRING|MAY_BE_LONG, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_odbc_result_all, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, statement)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, format, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_odbc_free_result, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, statement)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_connect, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, dsn, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, user, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, password, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, cursor_option, IS_LONG, 0, "SQL_CUR_USE_DRIVER")
ZEND_END_ARG_INFO()

#define arginfo_odbc_pconnect arginfo_odbc_connect

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_odbc_close, 0, 1, IS_VOID, 0)
	ZEND_ARG_INFO(0, odbc)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_odbc_num_rows, 0, 1, IS_LONG, 0)
	ZEND_ARG_INFO(0, statement)
ZEND_END_ARG_INFO()

#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_odbc_next_result, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, statement)
ZEND_END_ARG_INFO()
#endif

#define arginfo_odbc_num_fields arginfo_odbc_num_rows

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_odbc_field_name, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, statement)
	ZEND_ARG_TYPE_INFO(0, field, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_odbc_field_type arginfo_odbc_field_name

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_odbc_field_len, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, statement)
	ZEND_ARG_TYPE_INFO(0, field, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_odbc_field_precision arginfo_odbc_field_len

#define arginfo_odbc_field_scale arginfo_odbc_field_len

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_odbc_field_num, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, statement)
	ZEND_ARG_TYPE_INFO(0, field, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_odbc_autocommit, 0, 1, MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_INFO(0, odbc)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enable, _IS_BOOL, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_odbc_commit, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, odbc)
ZEND_END_ARG_INFO()

#define arginfo_odbc_rollback arginfo_odbc_commit

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_odbc_error, 0, 0, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, odbc, "null")
ZEND_END_ARG_INFO()

#define arginfo_odbc_errormsg arginfo_odbc_error

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_odbc_setoption, 0, 4, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, odbc)
	ZEND_ARG_TYPE_INFO(0, which, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_tables, 0, 0, 1)
	ZEND_ARG_INFO(0, odbc)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, catalog, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, schema, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, table, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, types, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_columns, 0, 0, 1)
	ZEND_ARG_INFO(0, odbc)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, catalog, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, schema, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, table, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, column, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_gettypeinfo, 0, 0, 1)
	ZEND_ARG_INFO(0, odbc)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, data_type, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_primarykeys, 0, 0, 4)
	ZEND_ARG_INFO(0, odbc)
	ZEND_ARG_TYPE_INFO(0, catalog, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, schema, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, table, IS_STRING, 0)
ZEND_END_ARG_INFO()

#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35)
ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_procedurecolumns, 0, 0, 1)
	ZEND_ARG_INFO(0, odbc)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, catalog, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, schema, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, procedure, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, column, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()
#endif

#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35)
ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_procedures, 0, 0, 1)
	ZEND_ARG_INFO(0, odbc)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, catalog, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, schema, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, procedure, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()
#endif

#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35)
ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_foreignkeys, 0, 0, 7)
	ZEND_ARG_INFO(0, odbc)
	ZEND_ARG_TYPE_INFO(0, pk_catalog, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, pk_schema, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, pk_table, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, fk_catalog, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, fk_schema, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, fk_table, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_specialcolumns, 0, 0, 7)
	ZEND_ARG_INFO(0, odbc)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, catalog, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, schema, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, table, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, scope, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, nullable, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_statistics, 0, 0, 6)
	ZEND_ARG_INFO(0, odbc)
	ZEND_ARG_TYPE_INFO(0, catalog, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, schema, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, table, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, unique, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, accuracy, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if !defined(HAVE_DBMAKER) && !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) &&!defined(HAVE_SOLID_35)
ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_tableprivileges, 0, 0, 4)
	ZEND_ARG_INFO(0, odbc)
	ZEND_ARG_TYPE_INFO(0, catalog, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, schema, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, table, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if !defined(HAVE_DBMAKER) && !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) &&!defined(HAVE_SOLID_35)
ZEND_BEGIN_ARG_INFO_EX(arginfo_odbc_columnprivileges, 0, 0, 5)
	ZEND_ARG_INFO(0, odbc)
	ZEND_ARG_TYPE_INFO(0, catalog, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, schema, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, table, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, column, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_odbc_connection_string_is_quoted, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_odbc_connection_string_should_quote arginfo_odbc_connection_string_is_quoted

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_odbc_connection_string_quote, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(odbc_close_all);
ZEND_FUNCTION(odbc_binmode);
ZEND_FUNCTION(odbc_longreadlen);
ZEND_FUNCTION(odbc_prepare);
ZEND_FUNCTION(odbc_execute);
ZEND_FUNCTION(odbc_cursor);
#if defined(HAVE_SQLDATASOURCES)
ZEND_FUNCTION(odbc_data_source);
#endif
ZEND_FUNCTION(odbc_exec);
#if defined(PHP_ODBC_HAVE_FETCH_HASH)
ZEND_FUNCTION(odbc_fetch_object);
#endif
#if defined(PHP_ODBC_HAVE_FETCH_HASH)
ZEND_FUNCTION(odbc_fetch_array);
#endif
ZEND_FUNCTION(odbc_fetch_into);
ZEND_FUNCTION(odbc_fetch_row);
ZEND_FUNCTION(odbc_result);
ZEND_FUNCTION(odbc_result_all);
ZEND_FUNCTION(odbc_free_result);
ZEND_FUNCTION(odbc_connect);
ZEND_FUNCTION(odbc_pconnect);
ZEND_FUNCTION(odbc_close);
ZEND_FUNCTION(odbc_num_rows);
#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30)
ZEND_FUNCTION(odbc_next_result);
#endif
ZEND_FUNCTION(odbc_num_fields);
ZEND_FUNCTION(odbc_field_name);
ZEND_FUNCTION(odbc_field_type);
ZEND_FUNCTION(odbc_field_len);
ZEND_FUNCTION(odbc_field_scale);
ZEND_FUNCTION(odbc_field_num);
ZEND_FUNCTION(odbc_autocommit);
ZEND_FUNCTION(odbc_commit);
ZEND_FUNCTION(odbc_rollback);
ZEND_FUNCTION(odbc_error);
ZEND_FUNCTION(odbc_errormsg);
ZEND_FUNCTION(odbc_setoption);
ZEND_FUNCTION(odbc_tables);
ZEND_FUNCTION(odbc_columns);
ZEND_FUNCTION(odbc_gettypeinfo);
ZEND_FUNCTION(odbc_primarykeys);
#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35)
ZEND_FUNCTION(odbc_procedurecolumns);
#endif
#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35)
ZEND_FUNCTION(odbc_procedures);
#endif
#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35)
ZEND_FUNCTION(odbc_foreignkeys);
#endif
ZEND_FUNCTION(odbc_specialcolumns);
ZEND_FUNCTION(odbc_statistics);
#if !defined(HAVE_DBMAKER) && !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) &&!defined(HAVE_SOLID_35)
ZEND_FUNCTION(odbc_tableprivileges);
#endif
#if !defined(HAVE_DBMAKER) && !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) &&!defined(HAVE_SOLID_35)
ZEND_FUNCTION(odbc_columnprivileges);
#endif
ZEND_FUNCTION(odbc_connection_string_is_quoted);
ZEND_FUNCTION(odbc_connection_string_should_quote);
ZEND_FUNCTION(odbc_connection_string_quote);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(odbc_close_all, arginfo_odbc_close_all)
	ZEND_FE(odbc_binmode, arginfo_odbc_binmode)
	ZEND_FE(odbc_longreadlen, arginfo_odbc_longreadlen)
	ZEND_FE(odbc_prepare, arginfo_odbc_prepare)
	ZEND_FE(odbc_execute, arginfo_odbc_execute)
	ZEND_FE(odbc_cursor, arginfo_odbc_cursor)
#if defined(HAVE_SQLDATASOURCES)
	ZEND_FE(odbc_data_source, arginfo_odbc_data_source)
#endif
	ZEND_FE(odbc_exec, arginfo_odbc_exec)
	ZEND_FALIAS(odbc_do, odbc_exec, arginfo_odbc_do)
#if defined(PHP_ODBC_HAVE_FETCH_HASH)
	ZEND_FE(odbc_fetch_object, arginfo_odbc_fetch_object)
#endif
#if defined(PHP_ODBC_HAVE_FETCH_HASH)
	ZEND_FE(odbc_fetch_array, arginfo_odbc_fetch_array)
#endif
	ZEND_FE(odbc_fetch_into, arginfo_odbc_fetch_into)
	ZEND_FE(odbc_fetch_row, arginfo_odbc_fetch_row)
	ZEND_FE(odbc_result, arginfo_odbc_result)
	ZEND_DEP_FE(odbc_result_all, arginfo_odbc_result_all)
	ZEND_FE(odbc_free_result, arginfo_odbc_free_result)
	ZEND_FE(odbc_connect, arginfo_odbc_connect)
	ZEND_FE(odbc_pconnect, arginfo_odbc_pconnect)
	ZEND_FE(odbc_close, arginfo_odbc_close)
	ZEND_FE(odbc_num_rows, arginfo_odbc_num_rows)
#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30)
	ZEND_FE(odbc_next_result, arginfo_odbc_next_result)
#endif
	ZEND_FE(odbc_num_fields, arginfo_odbc_num_fields)
	ZEND_FE(odbc_field_name, arginfo_odbc_field_name)
	ZEND_FE(odbc_field_type, arginfo_odbc_field_type)
	ZEND_FE(odbc_field_len, arginfo_odbc_field_len)
	ZEND_FALIAS(odbc_field_precision, odbc_field_len, arginfo_odbc_field_precision)
	ZEND_FE(odbc_field_scale, arginfo_odbc_field_scale)
	ZEND_FE(odbc_field_num, arginfo_odbc_field_num)
	ZEND_FE(odbc_autocommit, arginfo_odbc_autocommit)
	ZEND_FE(odbc_commit, arginfo_odbc_commit)
	ZEND_FE(odbc_rollback, arginfo_odbc_rollback)
	ZEND_FE(odbc_error, arginfo_odbc_error)
	ZEND_FE(odbc_errormsg, arginfo_odbc_errormsg)
	ZEND_FE(odbc_setoption, arginfo_odbc_setoption)
	ZEND_FE(odbc_tables, arginfo_odbc_tables)
	ZEND_FE(odbc_columns, arginfo_odbc_columns)
	ZEND_FE(odbc_gettypeinfo, arginfo_odbc_gettypeinfo)
	ZEND_FE(odbc_primarykeys, arginfo_odbc_primarykeys)
#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35)
	ZEND_FE(odbc_procedurecolumns, arginfo_odbc_procedurecolumns)
#endif
#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35)
	ZEND_FE(odbc_procedures, arginfo_odbc_procedures)
#endif
#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) && !defined(HAVE_SOLID_35)
	ZEND_FE(odbc_foreignkeys, arginfo_odbc_foreignkeys)
#endif
	ZEND_FE(odbc_specialcolumns, arginfo_odbc_specialcolumns)
	ZEND_FE(odbc_statistics, arginfo_odbc_statistics)
#if !defined(HAVE_DBMAKER) && !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) &&!defined(HAVE_SOLID_35)
	ZEND_FE(odbc_tableprivileges, arginfo_odbc_tableprivileges)
#endif
#if !defined(HAVE_DBMAKER) && !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30) &&!defined(HAVE_SOLID_35)
	ZEND_FE(odbc_columnprivileges, arginfo_odbc_columnprivileges)
#endif
	ZEND_FE(odbc_connection_string_is_quoted, arginfo_odbc_connection_string_is_quoted)
	ZEND_FE(odbc_connection_string_should_quote, arginfo_odbc_connection_string_should_quote)
	ZEND_FE(odbc_connection_string_quote, arginfo_odbc_connection_string_quote)
	ZEND_FE_END
};

static void register_odbc_symbols(int module_number)
{
	REGISTER_STRING_CONSTANT("ODBC_TYPE", PHP_ODBC_TYPE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ODBC_BINMODE_PASSTHRU", PHP_ODBC_BINMODE_PASSTHRU, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ODBC_BINMODE_RETURN", PHP_ODBC_BINMODE_RETURN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ODBC_BINMODE_CONVERT", PHP_ODBC_BINMODE_CONVERT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_ODBC_CURSORS", SQL_ODBC_CURSORS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_CUR_USE_DRIVER", SQL_CUR_USE_DRIVER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_CUR_USE_IF_NEEDED", SQL_CUR_USE_IF_NEEDED, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_CUR_USE_ODBC", SQL_CUR_USE_ODBC, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_CONCURRENCY", SQL_CONCURRENCY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_CONCUR_READ_ONLY", SQL_CONCUR_READ_ONLY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_CONCUR_LOCK", SQL_CONCUR_LOCK, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_CONCUR_ROWVER", SQL_CONCUR_ROWVER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_CONCUR_VALUES", SQL_CONCUR_VALUES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_CURSOR_TYPE", SQL_CURSOR_TYPE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_CURSOR_FORWARD_ONLY", SQL_CURSOR_FORWARD_ONLY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_CURSOR_KEYSET_DRIVEN", SQL_CURSOR_KEYSET_DRIVEN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_CURSOR_DYNAMIC", SQL_CURSOR_DYNAMIC, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_CURSOR_STATIC", SQL_CURSOR_STATIC, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_KEYSET_SIZE", SQL_KEYSET_SIZE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_FETCH_FIRST", SQL_FETCH_FIRST, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_FETCH_NEXT", SQL_FETCH_NEXT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_CHAR", SQL_CHAR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_VARCHAR", SQL_VARCHAR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_LONGVARCHAR", SQL_LONGVARCHAR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_DECIMAL", SQL_DECIMAL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_NUMERIC", SQL_NUMERIC, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_BIT", SQL_BIT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_TINYINT", SQL_TINYINT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_SMALLINT", SQL_SMALLINT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_INTEGER", SQL_INTEGER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_BIGINT", SQL_BIGINT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_REAL", SQL_REAL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_FLOAT", SQL_FLOAT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_DOUBLE", SQL_DOUBLE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_BINARY", SQL_BINARY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_VARBINARY", SQL_VARBINARY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_LONGVARBINARY", SQL_LONGVARBINARY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_DATE", SQL_DATE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_TIME", SQL_TIME, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQL_TIMESTAMP", SQL_TIMESTAMP, CONST_PERSISTENT);
#if (defined(ODBCVER) && (ODBCVER >= 0x0300))
	REGISTER_LONG_CONSTANT("SQL_TYPE_DATE", SQL_TYPE_DATE, CONST_PERSISTENT);
#endif
#if (defined(ODBCVER) && (ODBCVER >= 0x0300))
	REGISTER_LONG_CONSTANT("SQL_TYPE_TIME", SQL_TYPE_TIME, CONST_PERSISTENT);
#endif
#if (defined(ODBCVER) && (ODBCVER >= 0x0300))
	REGISTER_LONG_CONSTANT("SQL_TYPE_TIMESTAMP", SQL_TYPE_TIMESTAMP, CONST_PERSISTENT);
#endif
#if (defined(ODBCVER) && (ODBCVER >= 0x0300))
	REGISTER_LONG_CONSTANT("SQL_WCHAR", SQL_WCHAR, CONST_PERSISTENT);
#endif
#if (defined(ODBCVER) && (ODBCVER >= 0x0300))
	REGISTER_LONG_CONSTANT("SQL_WVARCHAR", SQL_WVARCHAR, CONST_PERSISTENT);
#endif
#if (defined(ODBCVER) && (ODBCVER >= 0x0300))
	REGISTER_LONG_CONSTANT("SQL_WLONGVARCHAR", SQL_WLONGVARCHAR, CONST_PERSISTENT);
#endif
#if (defined(ODBCVER) && (ODBCVER >= 0x0300))
	REGISTER_LONG_CONSTANT("SQL_BEST_ROWID", SQL_BEST_ROWID, CONST_PERSISTENT);
#endif
#if (defined(ODBCVER) && (ODBCVER >= 0x0300))
	REGISTER_LONG_CONSTANT("SQL_ROWVER", SQL_ROWVER, CONST_PERSISTENT);
#endif
#if (defined(ODBCVER) && (ODBCVER >= 0x0300))
	REGISTER_LONG_CONSTANT("SQL_SCOPE_CURROW", SQL_SCOPE_CURROW, CONST_PERSISTENT);
#endif
#if (defined(ODBCVER) && (ODBCVER >= 0x0300))
	REGISTER_LONG_CONSTANT("SQL_SCOPE_TRANSACTION", SQL_SCOPE_TRANSACTION, CONST_PERSISTENT);
#endif
#if (defined(ODBCVER) && (ODBCVER >= 0x0300))
	REGISTER_LONG_CONSTANT("SQL_SCOPE_SESSION", SQL_SCOPE_SESSION, CONST_PERSISTENT);
#endif
#if (defined(ODBCVER) && (ODBCVER >= 0x0300))
	REGISTER_LONG_CONSTANT("SQL_NO_NULLS", SQL_NO_NULLS, CONST_PERSISTENT);
#endif
#if (defined(ODBCVER) && (ODBCVER >= 0x0300))
	REGISTER_LONG_CONSTANT("SQL_NULLABLE", SQL_NULLABLE, CONST_PERSISTENT);
#endif
#if (defined(ODBCVER) && (ODBCVER >= 0x0300))
	REGISTER_LONG_CONSTANT("SQL_INDEX_UNIQUE", SQL_INDEX_UNIQUE, CONST_PERSISTENT);
#endif
#if (defined(ODBCVER) && (ODBCVER >= 0x0300))
	REGISTER_LONG_CONSTANT("SQL_INDEX_ALL", SQL_INDEX_ALL, CONST_PERSISTENT);
#endif
#if (defined(ODBCVER) && (ODBCVER >= 0x0300))
	REGISTER_LONG_CONSTANT("SQL_ENSURE", SQL_ENSURE, CONST_PERSISTENT);
#endif
#if (defined(ODBCVER) && (ODBCVER >= 0x0300))
	REGISTER_LONG_CONSTANT("SQL_QUICK", SQL_QUICK, CONST_PERSISTENT);
#endif


	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "odbc_connect", sizeof("odbc_connect") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);

	zend_add_parameter_attribute(zend_hash_str_find_ptr(CG(function_table), "odbc_pconnect", sizeof("odbc_pconnect") - 1), 2, ZSTR_KNOWN(ZEND_STR_SENSITIVEPARAMETER), 0);
}
