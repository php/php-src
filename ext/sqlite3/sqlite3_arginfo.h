/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, encryption_key, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SQLite3_open arginfo_class_SQLite3___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3_close, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SQLite3_version arginfo_class_SQLite3_close

#define arginfo_class_SQLite3_lastInsertRowID arginfo_class_SQLite3_close

#define arginfo_class_SQLite3_lastErrorCode arginfo_class_SQLite3_close

#define arginfo_class_SQLite3_lastExtendedErrorCode arginfo_class_SQLite3_close

#define arginfo_class_SQLite3_lastErrorMsg arginfo_class_SQLite3_close

#define arginfo_class_SQLite3_changes arginfo_class_SQLite3_close

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3_busyTimeout, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, ms, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if !defined(SQLITE_OMIT_LOAD_EXTENSION)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3_loadExtension, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, shared_library, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if SQLITE_VERSION_NUMBER >= 3006011
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3_backup, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, destination_db, SQLite3, 0)
	ZEND_ARG_TYPE_INFO(0, source_dbname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, destination_dbname, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3_escapeString, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3_prepare, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SQLite3_query arginfo_class_SQLite3_prepare

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3_querySingle, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, entire_row, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3_createFunction, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_INFO(0, callback)
	ZEND_ARG_TYPE_INFO(0, argument_count, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3_createAggregate, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_INFO(0, step_callback)
	ZEND_ARG_INFO(0, final_callback)
	ZEND_ARG_TYPE_INFO(0, argument_count, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3_createCollation, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3_openBlob, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, table, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, column, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, rowid, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, dbname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3_enableExceptions, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, enableExceptions, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3_enableExtendedResultCodes, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, enable, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3Stmt___construct, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, sqlite3, SQLite3, 0)
	ZEND_ARG_TYPE_INFO(0, sql, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3Stmt_bindParam, 0, 0, 2)
	ZEND_ARG_INFO(0, param_number)
	ZEND_ARG_INFO(1, param)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3Stmt_bindValue, 0, 0, 2)
	ZEND_ARG_INFO(0, param_number)
	ZEND_ARG_INFO(0, param)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SQLite3Stmt_clear arginfo_class_SQLite3_close

#define arginfo_class_SQLite3Stmt_close arginfo_class_SQLite3_close

#define arginfo_class_SQLite3Stmt_execute arginfo_class_SQLite3_close

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3Stmt_getSQL, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, expanded, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SQLite3Stmt_paramCount arginfo_class_SQLite3_close

#define arginfo_class_SQLite3Stmt_readOnly arginfo_class_SQLite3_close

#define arginfo_class_SQLite3Stmt_reset arginfo_class_SQLite3_close

#define arginfo_class_SQLite3Result___construct arginfo_class_SQLite3_close

#define arginfo_class_SQLite3Result_numColumns arginfo_class_SQLite3_close

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3Result_columnName, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, column_number, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SQLite3Result_columnType arginfo_class_SQLite3Result_columnName

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3Result_fetchArray, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SQLite3Result_reset arginfo_class_SQLite3_close

#define arginfo_class_SQLite3Result_finalize arginfo_class_SQLite3_close
