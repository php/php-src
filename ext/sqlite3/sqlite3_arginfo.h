/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 61a684b6c221a15538a5f8cc32bc4e7d4cf3c8fd */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "SQLITE3_OPEN_READWRITE | SQLITE3_OPEN_CREATE")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encryptionKey, IS_STRING, 0, "\"\"")
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
	ZEND_ARG_TYPE_INFO(0, milliseconds, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if !defined(SQLITE_OMIT_LOAD_EXTENSION)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3_loadExtension, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if SQLITE_VERSION_NUMBER >= 3006011
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3_backup, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, destination, SQLite3, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, sourceDatabase, IS_STRING, 0, "\"main\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, destinationDatabase, IS_STRING, 0, "\"main\"")
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3_escapeString, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3_prepare, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SQLite3_exec arginfo_class_SQLite3_prepare

#define arginfo_class_SQLite3_query arginfo_class_SQLite3_prepare

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3_querySingle, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, entireRow, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3_createFunction, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, argCount, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3_createAggregate, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, stepCallback, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO(0, finalCallback, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, argCount, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3_createCollation, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3_openBlob, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, table, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, column, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, rowid, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, database, IS_STRING, 0, "\"main\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "SQLITE3_OPEN_READONLY")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3_enableExceptions, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enable, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3_enableExtendedResultCodes, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enable, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3_setAuthorizer, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3Stmt___construct, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, sqlite3, SQLite3, 0)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3Stmt_bindParam, 0, 0, 2)
	ZEND_ARG_TYPE_MASK(0, param, MAY_BE_STRING|MAY_BE_LONG, NULL)
	ZEND_ARG_TYPE_INFO(1, var, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "SQLITE3_TEXT")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3Stmt_bindValue, 0, 0, 2)
	ZEND_ARG_TYPE_MASK(0, param, MAY_BE_STRING|MAY_BE_LONG, NULL)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "SQLITE3_TEXT")
ZEND_END_ARG_INFO()

#define arginfo_class_SQLite3Stmt_clear arginfo_class_SQLite3_close

#define arginfo_class_SQLite3Stmt_close arginfo_class_SQLite3_close

#define arginfo_class_SQLite3Stmt_execute arginfo_class_SQLite3_close

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3Stmt_getSQL, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, expand, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#define arginfo_class_SQLite3Stmt_paramCount arginfo_class_SQLite3_close

#define arginfo_class_SQLite3Stmt_readOnly arginfo_class_SQLite3_close

#define arginfo_class_SQLite3Stmt_reset arginfo_class_SQLite3_close

#define arginfo_class_SQLite3Result___construct arginfo_class_SQLite3_close

#define arginfo_class_SQLite3Result_numColumns arginfo_class_SQLite3_close

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3Result_columnName, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, column, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SQLite3Result_columnType arginfo_class_SQLite3Result_columnName

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3Result_fetchArray, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "SQLITE3_BOTH")
ZEND_END_ARG_INFO()

#define arginfo_class_SQLite3Result_reset arginfo_class_SQLite3_close

#define arginfo_class_SQLite3Result_finalize arginfo_class_SQLite3_close


ZEND_METHOD(SQLite3, open);
ZEND_METHOD(SQLite3, close);
ZEND_METHOD(SQLite3, version);
ZEND_METHOD(SQLite3, lastInsertRowID);
ZEND_METHOD(SQLite3, lastErrorCode);
ZEND_METHOD(SQLite3, lastExtendedErrorCode);
ZEND_METHOD(SQLite3, lastErrorMsg);
ZEND_METHOD(SQLite3, changes);
ZEND_METHOD(SQLite3, busyTimeout);
#if !defined(SQLITE_OMIT_LOAD_EXTENSION)
ZEND_METHOD(SQLite3, loadExtension);
#endif
#if SQLITE_VERSION_NUMBER >= 3006011
ZEND_METHOD(SQLite3, backup);
#endif
ZEND_METHOD(SQLite3, escapeString);
ZEND_METHOD(SQLite3, prepare);
ZEND_METHOD(SQLite3, exec);
ZEND_METHOD(SQLite3, query);
ZEND_METHOD(SQLite3, querySingle);
ZEND_METHOD(SQLite3, createFunction);
ZEND_METHOD(SQLite3, createAggregate);
ZEND_METHOD(SQLite3, createCollation);
ZEND_METHOD(SQLite3, openBlob);
ZEND_METHOD(SQLite3, enableExceptions);
ZEND_METHOD(SQLite3, enableExtendedResultCodes);
ZEND_METHOD(SQLite3, setAuthorizer);
ZEND_METHOD(SQLite3Stmt, __construct);
ZEND_METHOD(SQLite3Stmt, bindParam);
ZEND_METHOD(SQLite3Stmt, bindValue);
ZEND_METHOD(SQLite3Stmt, clear);
ZEND_METHOD(SQLite3Stmt, close);
ZEND_METHOD(SQLite3Stmt, execute);
ZEND_METHOD(SQLite3Stmt, getSQL);
ZEND_METHOD(SQLite3Stmt, paramCount);
ZEND_METHOD(SQLite3Stmt, readOnly);
ZEND_METHOD(SQLite3Stmt, reset);
ZEND_METHOD(SQLite3Result, __construct);
ZEND_METHOD(SQLite3Result, numColumns);
ZEND_METHOD(SQLite3Result, columnName);
ZEND_METHOD(SQLite3Result, columnType);
ZEND_METHOD(SQLite3Result, fetchArray);
ZEND_METHOD(SQLite3Result, reset);
ZEND_METHOD(SQLite3Result, finalize);


static const zend_function_entry class_SQLite3_methods[] = {
	ZEND_MALIAS(SQLite3, __construct, open, arginfo_class_SQLite3___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(SQLite3, open, arginfo_class_SQLite3_open, ZEND_ACC_PUBLIC)
	ZEND_ME(SQLite3, close, arginfo_class_SQLite3_close, ZEND_ACC_PUBLIC)
	ZEND_ME(SQLite3, version, arginfo_class_SQLite3_version, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(SQLite3, lastInsertRowID, arginfo_class_SQLite3_lastInsertRowID, ZEND_ACC_PUBLIC)
	ZEND_ME(SQLite3, lastErrorCode, arginfo_class_SQLite3_lastErrorCode, ZEND_ACC_PUBLIC)
	ZEND_ME(SQLite3, lastExtendedErrorCode, arginfo_class_SQLite3_lastExtendedErrorCode, ZEND_ACC_PUBLIC)
	ZEND_ME(SQLite3, lastErrorMsg, arginfo_class_SQLite3_lastErrorMsg, ZEND_ACC_PUBLIC)
	ZEND_ME(SQLite3, changes, arginfo_class_SQLite3_changes, ZEND_ACC_PUBLIC)
	ZEND_ME(SQLite3, busyTimeout, arginfo_class_SQLite3_busyTimeout, ZEND_ACC_PUBLIC)
#if !defined(SQLITE_OMIT_LOAD_EXTENSION)
	ZEND_ME(SQLite3, loadExtension, arginfo_class_SQLite3_loadExtension, ZEND_ACC_PUBLIC)
#endif
#if SQLITE_VERSION_NUMBER >= 3006011
	ZEND_ME(SQLite3, backup, arginfo_class_SQLite3_backup, ZEND_ACC_PUBLIC)
#endif
	ZEND_ME(SQLite3, escapeString, arginfo_class_SQLite3_escapeString, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(SQLite3, prepare, arginfo_class_SQLite3_prepare, ZEND_ACC_PUBLIC)
	ZEND_ME(SQLite3, exec, arginfo_class_SQLite3_exec, ZEND_ACC_PUBLIC)
	ZEND_ME(SQLite3, query, arginfo_class_SQLite3_query, ZEND_ACC_PUBLIC)
	ZEND_ME(SQLite3, querySingle, arginfo_class_SQLite3_querySingle, ZEND_ACC_PUBLIC)
	ZEND_ME(SQLite3, createFunction, arginfo_class_SQLite3_createFunction, ZEND_ACC_PUBLIC)
	ZEND_ME(SQLite3, createAggregate, arginfo_class_SQLite3_createAggregate, ZEND_ACC_PUBLIC)
	ZEND_ME(SQLite3, createCollation, arginfo_class_SQLite3_createCollation, ZEND_ACC_PUBLIC)
	ZEND_ME(SQLite3, openBlob, arginfo_class_SQLite3_openBlob, ZEND_ACC_PUBLIC)
	ZEND_ME(SQLite3, enableExceptions, arginfo_class_SQLite3_enableExceptions, ZEND_ACC_PUBLIC)
	ZEND_ME(SQLite3, enableExtendedResultCodes, arginfo_class_SQLite3_enableExtendedResultCodes, ZEND_ACC_PUBLIC)
	ZEND_ME(SQLite3, setAuthorizer, arginfo_class_SQLite3_setAuthorizer, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_SQLite3Stmt_methods[] = {
	ZEND_ME(SQLite3Stmt, __construct, arginfo_class_SQLite3Stmt___construct, ZEND_ACC_PRIVATE)
	ZEND_ME(SQLite3Stmt, bindParam, arginfo_class_SQLite3Stmt_bindParam, ZEND_ACC_PUBLIC)
	ZEND_ME(SQLite3Stmt, bindValue, arginfo_class_SQLite3Stmt_bindValue, ZEND_ACC_PUBLIC)
	ZEND_ME(SQLite3Stmt, clear, arginfo_class_SQLite3Stmt_clear, ZEND_ACC_PUBLIC)
	ZEND_ME(SQLite3Stmt, close, arginfo_class_SQLite3Stmt_close, ZEND_ACC_PUBLIC)
	ZEND_ME(SQLite3Stmt, execute, arginfo_class_SQLite3Stmt_execute, ZEND_ACC_PUBLIC)
	ZEND_ME(SQLite3Stmt, getSQL, arginfo_class_SQLite3Stmt_getSQL, ZEND_ACC_PUBLIC)
	ZEND_ME(SQLite3Stmt, paramCount, arginfo_class_SQLite3Stmt_paramCount, ZEND_ACC_PUBLIC)
	ZEND_ME(SQLite3Stmt, readOnly, arginfo_class_SQLite3Stmt_readOnly, ZEND_ACC_PUBLIC)
	ZEND_ME(SQLite3Stmt, reset, arginfo_class_SQLite3Stmt_reset, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_SQLite3Result_methods[] = {
	ZEND_ME(SQLite3Result, __construct, arginfo_class_SQLite3Result___construct, ZEND_ACC_PRIVATE)
	ZEND_ME(SQLite3Result, numColumns, arginfo_class_SQLite3Result_numColumns, ZEND_ACC_PUBLIC)
	ZEND_ME(SQLite3Result, columnName, arginfo_class_SQLite3Result_columnName, ZEND_ACC_PUBLIC)
	ZEND_ME(SQLite3Result, columnType, arginfo_class_SQLite3Result_columnType, ZEND_ACC_PUBLIC)
	ZEND_ME(SQLite3Result, fetchArray, arginfo_class_SQLite3Result_fetchArray, ZEND_ACC_PUBLIC)
	ZEND_ME(SQLite3Result, reset, arginfo_class_SQLite3Result_reset, ZEND_ACC_PUBLIC)
	ZEND_ME(SQLite3Result, finalize, arginfo_class_SQLite3Result_finalize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
