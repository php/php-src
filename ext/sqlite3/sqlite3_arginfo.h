/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 6c5f8bc087baa56f40834f9745f3d0f22f60edba */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "SQLITE3_OPEN_READWRITE | SQLITE3_OPEN_CREATE")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encryptionKey, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SQLite3_open, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "SQLITE3_OPEN_READWRITE | SQLITE3_OPEN_CREATE")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encryptionKey, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3_close, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SQLite3_version, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SQLite3_lastInsertRowID, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SQLite3_lastErrorCode arginfo_class_SQLite3_lastInsertRowID

#define arginfo_class_SQLite3_lastExtendedErrorCode arginfo_class_SQLite3_lastInsertRowID

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SQLite3_lastErrorMsg, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SQLite3_changes arginfo_class_SQLite3_lastInsertRowID

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SQLite3_busyTimeout, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, milliseconds, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if !defined(SQLITE_OMIT_LOAD_EXTENSION)
ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SQLite3_loadExtension, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if SQLITE_VERSION_NUMBER >= 3006011
ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SQLite3_backup, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, destination, SQLite3, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, sourceDatabase, IS_STRING, 0, "\"main\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, destinationDatabase, IS_STRING, 0, "\"main\"")
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SQLite3_escapeString, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_SQLite3_prepare, 0, 1, SQLite3Stmt, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SQLite3_exec, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_SQLite3_query, 0, 1, SQLite3Result, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SQLite3_querySingle, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, entireRow, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SQLite3_createFunction, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, argCount, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SQLite3_createAggregate, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, stepCallback, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO(0, finalCallback, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, argCount, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SQLite3_createCollation, 0, 2, _IS_BOOL, 0)
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

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SQLite3_enableExceptions, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enable, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SQLite3_enableExtendedResultCodes, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enable, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SQLite3_setAuthorizer, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SQLite3Stmt___construct, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, sqlite3, SQLite3, 0)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SQLite3Stmt_bindParam, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_MASK(0, param, MAY_BE_STRING|MAY_BE_LONG, NULL)
	ZEND_ARG_TYPE_INFO(1, var, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "SQLITE3_TEXT")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SQLite3Stmt_bindValue, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_MASK(0, param, MAY_BE_STRING|MAY_BE_LONG, NULL)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "SQLITE3_TEXT")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_SQLite3Stmt_clear, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SQLite3Stmt_close arginfo_class_SQLite3Stmt_clear

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_SQLite3Stmt_execute, 0, 0, SQLite3Result, MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_SQLite3Stmt_getSQL, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, expand, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#define arginfo_class_SQLite3Stmt_paramCount arginfo_class_SQLite3_lastInsertRowID

#define arginfo_class_SQLite3Stmt_readOnly arginfo_class_SQLite3Stmt_clear

#define arginfo_class_SQLite3Stmt_reset arginfo_class_SQLite3Stmt_clear

#define arginfo_class_SQLite3Result___construct arginfo_class_SQLite3_close

#define arginfo_class_SQLite3Result_numColumns arginfo_class_SQLite3_lastInsertRowID

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_SQLite3Result_columnName, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, column, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_SQLite3Result_columnType, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, column, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_SQLite3Result_fetchArray, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "SQLITE3_BOTH")
ZEND_END_ARG_INFO()

#define arginfo_class_SQLite3Result_reset arginfo_class_SQLite3Stmt_clear

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

static const zend_function_entry class_SQLite3Exception_methods[] = {
	ZEND_FE_END
};

static const zend_function_entry class_SQLite3_methods[] = {
	ZEND_RAW_FENTRY("__construct", zim_SQLite3_open, arginfo_class_SQLite3___construct, ZEND_ACC_PUBLIC, NULL, NULL)
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

static void register_sqlite3_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("SQLITE3_ASSOC", PHP_SQLITE3_ASSOC, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE3_NUM", PHP_SQLITE3_NUM, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE3_BOTH", PHP_SQLITE3_BOTH, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE3_INTEGER", SQLITE_INTEGER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE3_FLOAT", SQLITE_FLOAT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE3_TEXT", SQLITE3_TEXT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE3_BLOB", SQLITE_BLOB, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE3_NULL", SQLITE_NULL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE3_OPEN_READONLY", SQLITE_OPEN_READONLY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE3_OPEN_READWRITE", SQLITE_OPEN_READWRITE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SQLITE3_OPEN_CREATE", SQLITE_OPEN_CREATE, CONST_PERSISTENT);
#if defined(SQLITE_DETERMINISTIC)
	REGISTER_LONG_CONSTANT("SQLITE3_DETERMINISTIC", SQLITE_DETERMINISTIC, CONST_PERSISTENT);
#endif
}

static zend_class_entry *register_class_SQLite3Exception(zend_class_entry *class_entry_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SQLite3Exception", class_SQLite3Exception_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_Exception);
	class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

	return class_entry;
}

static zend_class_entry *register_class_SQLite3(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SQLite3", class_SQLite3_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;

	zval const_OK_value;
	ZVAL_LONG(&const_OK_value, SQLITE_OK);
	zend_string *const_OK_name = zend_string_init_interned("OK", sizeof("OK") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_OK_name, &const_OK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_OK_name);

	zval const_DENY_value;
	ZVAL_LONG(&const_DENY_value, SQLITE_DENY);
	zend_string *const_DENY_name = zend_string_init_interned("DENY", sizeof("DENY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DENY_name, &const_DENY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DENY_name);

	zval const_IGNORE_value;
	ZVAL_LONG(&const_IGNORE_value, SQLITE_IGNORE);
	zend_string *const_IGNORE_name = zend_string_init_interned("IGNORE", sizeof("IGNORE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_IGNORE_name, &const_IGNORE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_IGNORE_name);

	zval const_CREATE_INDEX_value;
	ZVAL_LONG(&const_CREATE_INDEX_value, SQLITE_CREATE_INDEX);
	zend_string *const_CREATE_INDEX_name = zend_string_init_interned("CREATE_INDEX", sizeof("CREATE_INDEX") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CREATE_INDEX_name, &const_CREATE_INDEX_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CREATE_INDEX_name);

	zval const_CREATE_TABLE_value;
	ZVAL_LONG(&const_CREATE_TABLE_value, SQLITE_CREATE_TABLE);
	zend_string *const_CREATE_TABLE_name = zend_string_init_interned("CREATE_TABLE", sizeof("CREATE_TABLE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CREATE_TABLE_name, &const_CREATE_TABLE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CREATE_TABLE_name);

	zval const_CREATE_TEMP_INDEX_value;
	ZVAL_LONG(&const_CREATE_TEMP_INDEX_value, SQLITE_CREATE_TEMP_INDEX);
	zend_string *const_CREATE_TEMP_INDEX_name = zend_string_init_interned("CREATE_TEMP_INDEX", sizeof("CREATE_TEMP_INDEX") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CREATE_TEMP_INDEX_name, &const_CREATE_TEMP_INDEX_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CREATE_TEMP_INDEX_name);

	zval const_CREATE_TEMP_TABLE_value;
	ZVAL_LONG(&const_CREATE_TEMP_TABLE_value, SQLITE_CREATE_TEMP_TABLE);
	zend_string *const_CREATE_TEMP_TABLE_name = zend_string_init_interned("CREATE_TEMP_TABLE", sizeof("CREATE_TEMP_TABLE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CREATE_TEMP_TABLE_name, &const_CREATE_TEMP_TABLE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CREATE_TEMP_TABLE_name);

	zval const_CREATE_TEMP_TRIGGER_value;
	ZVAL_LONG(&const_CREATE_TEMP_TRIGGER_value, SQLITE_CREATE_TEMP_TRIGGER);
	zend_string *const_CREATE_TEMP_TRIGGER_name = zend_string_init_interned("CREATE_TEMP_TRIGGER", sizeof("CREATE_TEMP_TRIGGER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CREATE_TEMP_TRIGGER_name, &const_CREATE_TEMP_TRIGGER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CREATE_TEMP_TRIGGER_name);

	zval const_CREATE_TEMP_VIEW_value;
	ZVAL_LONG(&const_CREATE_TEMP_VIEW_value, SQLITE_CREATE_TEMP_VIEW);
	zend_string *const_CREATE_TEMP_VIEW_name = zend_string_init_interned("CREATE_TEMP_VIEW", sizeof("CREATE_TEMP_VIEW") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CREATE_TEMP_VIEW_name, &const_CREATE_TEMP_VIEW_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CREATE_TEMP_VIEW_name);

	zval const_CREATE_TRIGGER_value;
	ZVAL_LONG(&const_CREATE_TRIGGER_value, SQLITE_CREATE_TRIGGER);
	zend_string *const_CREATE_TRIGGER_name = zend_string_init_interned("CREATE_TRIGGER", sizeof("CREATE_TRIGGER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CREATE_TRIGGER_name, &const_CREATE_TRIGGER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CREATE_TRIGGER_name);

	zval const_CREATE_VIEW_value;
	ZVAL_LONG(&const_CREATE_VIEW_value, SQLITE_CREATE_VIEW);
	zend_string *const_CREATE_VIEW_name = zend_string_init_interned("CREATE_VIEW", sizeof("CREATE_VIEW") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CREATE_VIEW_name, &const_CREATE_VIEW_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CREATE_VIEW_name);

	zval const_DELETE_value;
	ZVAL_LONG(&const_DELETE_value, SQLITE_DELETE);
	zend_string *const_DELETE_name = zend_string_init_interned("DELETE", sizeof("DELETE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DELETE_name, &const_DELETE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DELETE_name);

	zval const_DROP_INDEX_value;
	ZVAL_LONG(&const_DROP_INDEX_value, SQLITE_DROP_INDEX);
	zend_string *const_DROP_INDEX_name = zend_string_init_interned("DROP_INDEX", sizeof("DROP_INDEX") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DROP_INDEX_name, &const_DROP_INDEX_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DROP_INDEX_name);

	zval const_DROP_TABLE_value;
	ZVAL_LONG(&const_DROP_TABLE_value, SQLITE_DROP_TABLE);
	zend_string *const_DROP_TABLE_name = zend_string_init_interned("DROP_TABLE", sizeof("DROP_TABLE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DROP_TABLE_name, &const_DROP_TABLE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DROP_TABLE_name);

	zval const_DROP_TEMP_INDEX_value;
	ZVAL_LONG(&const_DROP_TEMP_INDEX_value, SQLITE_DROP_TEMP_INDEX);
	zend_string *const_DROP_TEMP_INDEX_name = zend_string_init_interned("DROP_TEMP_INDEX", sizeof("DROP_TEMP_INDEX") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DROP_TEMP_INDEX_name, &const_DROP_TEMP_INDEX_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DROP_TEMP_INDEX_name);

	zval const_DROP_TEMP_TABLE_value;
	ZVAL_LONG(&const_DROP_TEMP_TABLE_value, SQLITE_DROP_TEMP_TABLE);
	zend_string *const_DROP_TEMP_TABLE_name = zend_string_init_interned("DROP_TEMP_TABLE", sizeof("DROP_TEMP_TABLE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DROP_TEMP_TABLE_name, &const_DROP_TEMP_TABLE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DROP_TEMP_TABLE_name);

	zval const_DROP_TEMP_TRIGGER_value;
	ZVAL_LONG(&const_DROP_TEMP_TRIGGER_value, SQLITE_DROP_TEMP_TRIGGER);
	zend_string *const_DROP_TEMP_TRIGGER_name = zend_string_init_interned("DROP_TEMP_TRIGGER", sizeof("DROP_TEMP_TRIGGER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DROP_TEMP_TRIGGER_name, &const_DROP_TEMP_TRIGGER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DROP_TEMP_TRIGGER_name);

	zval const_DROP_TEMP_VIEW_value;
	ZVAL_LONG(&const_DROP_TEMP_VIEW_value, SQLITE_DROP_TEMP_VIEW);
	zend_string *const_DROP_TEMP_VIEW_name = zend_string_init_interned("DROP_TEMP_VIEW", sizeof("DROP_TEMP_VIEW") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DROP_TEMP_VIEW_name, &const_DROP_TEMP_VIEW_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DROP_TEMP_VIEW_name);

	zval const_DROP_TRIGGER_value;
	ZVAL_LONG(&const_DROP_TRIGGER_value, SQLITE_DROP_TRIGGER);
	zend_string *const_DROP_TRIGGER_name = zend_string_init_interned("DROP_TRIGGER", sizeof("DROP_TRIGGER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DROP_TRIGGER_name, &const_DROP_TRIGGER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DROP_TRIGGER_name);

	zval const_DROP_VIEW_value;
	ZVAL_LONG(&const_DROP_VIEW_value, SQLITE_DROP_VIEW);
	zend_string *const_DROP_VIEW_name = zend_string_init_interned("DROP_VIEW", sizeof("DROP_VIEW") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DROP_VIEW_name, &const_DROP_VIEW_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DROP_VIEW_name);

	zval const_INSERT_value;
	ZVAL_LONG(&const_INSERT_value, SQLITE_INSERT);
	zend_string *const_INSERT_name = zend_string_init_interned("INSERT", sizeof("INSERT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_INSERT_name, &const_INSERT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_INSERT_name);

	zval const_PRAGMA_value;
	ZVAL_LONG(&const_PRAGMA_value, SQLITE_PRAGMA);
	zend_string *const_PRAGMA_name = zend_string_init_interned("PRAGMA", sizeof("PRAGMA") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_PRAGMA_name, &const_PRAGMA_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_PRAGMA_name);

	zval const_READ_value;
	ZVAL_LONG(&const_READ_value, SQLITE_READ);
	zend_string *const_READ_name = zend_string_init_interned("READ", sizeof("READ") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_READ_name, &const_READ_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_READ_name);

	zval const_SELECT_value;
	ZVAL_LONG(&const_SELECT_value, SQLITE_SELECT);
	zend_string *const_SELECT_name = zend_string_init_interned("SELECT", sizeof("SELECT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SELECT_name, &const_SELECT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SELECT_name);

	zval const_TRANSACTION_value;
	ZVAL_LONG(&const_TRANSACTION_value, SQLITE_TRANSACTION);
	zend_string *const_TRANSACTION_name = zend_string_init_interned("TRANSACTION", sizeof("TRANSACTION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TRANSACTION_name, &const_TRANSACTION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TRANSACTION_name);

	zval const_UPDATE_value;
	ZVAL_LONG(&const_UPDATE_value, SQLITE_UPDATE);
	zend_string *const_UPDATE_name = zend_string_init_interned("UPDATE", sizeof("UPDATE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_UPDATE_name, &const_UPDATE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_UPDATE_name);

	zval const_ATTACH_value;
	ZVAL_LONG(&const_ATTACH_value, SQLITE_ATTACH);
	zend_string *const_ATTACH_name = zend_string_init_interned("ATTACH", sizeof("ATTACH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTACH_name, &const_ATTACH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTACH_name);

	zval const_DETACH_value;
	ZVAL_LONG(&const_DETACH_value, SQLITE_DETACH);
	zend_string *const_DETACH_name = zend_string_init_interned("DETACH", sizeof("DETACH") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DETACH_name, &const_DETACH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DETACH_name);

	zval const_ALTER_TABLE_value;
	ZVAL_LONG(&const_ALTER_TABLE_value, SQLITE_ALTER_TABLE);
	zend_string *const_ALTER_TABLE_name = zend_string_init_interned("ALTER_TABLE", sizeof("ALTER_TABLE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ALTER_TABLE_name, &const_ALTER_TABLE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ALTER_TABLE_name);

	zval const_REINDEX_value;
	ZVAL_LONG(&const_REINDEX_value, SQLITE_REINDEX);
	zend_string *const_REINDEX_name = zend_string_init_interned("REINDEX", sizeof("REINDEX") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_REINDEX_name, &const_REINDEX_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_REINDEX_name);

	zval const_ANALYZE_value;
	ZVAL_LONG(&const_ANALYZE_value, SQLITE_ANALYZE);
	zend_string *const_ANALYZE_name = zend_string_init_interned("ANALYZE", sizeof("ANALYZE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ANALYZE_name, &const_ANALYZE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ANALYZE_name);

	zval const_CREATE_VTABLE_value;
	ZVAL_LONG(&const_CREATE_VTABLE_value, SQLITE_CREATE_VTABLE);
	zend_string *const_CREATE_VTABLE_name = zend_string_init_interned("CREATE_VTABLE", sizeof("CREATE_VTABLE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_CREATE_VTABLE_name, &const_CREATE_VTABLE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_CREATE_VTABLE_name);

	zval const_DROP_VTABLE_value;
	ZVAL_LONG(&const_DROP_VTABLE_value, SQLITE_DROP_VTABLE);
	zend_string *const_DROP_VTABLE_name = zend_string_init_interned("DROP_VTABLE", sizeof("DROP_VTABLE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DROP_VTABLE_name, &const_DROP_VTABLE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DROP_VTABLE_name);

	zval const_FUNCTION_value;
	ZVAL_LONG(&const_FUNCTION_value, SQLITE_FUNCTION);
	zend_string *const_FUNCTION_name = zend_string_init_interned("FUNCTION", sizeof("FUNCTION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_FUNCTION_name, &const_FUNCTION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_FUNCTION_name);

	zval const_SAVEPOINT_value;
	ZVAL_LONG(&const_SAVEPOINT_value, SQLITE_SAVEPOINT);
	zend_string *const_SAVEPOINT_name = zend_string_init_interned("SAVEPOINT", sizeof("SAVEPOINT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SAVEPOINT_name, &const_SAVEPOINT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SAVEPOINT_name);

	zval const_COPY_value;
	ZVAL_LONG(&const_COPY_value, SQLITE_COPY);
	zend_string *const_COPY_name = zend_string_init_interned("COPY", sizeof("COPY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_COPY_name, &const_COPY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_COPY_name);
#if defined(SQLITE_RECURSIVE)

	zval const_RECURSIVE_value;
	ZVAL_LONG(&const_RECURSIVE_value, SQLITE_RECURSIVE);
	zend_string *const_RECURSIVE_name = zend_string_init_interned("RECURSIVE", sizeof("RECURSIVE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_RECURSIVE_name, &const_RECURSIVE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_RECURSIVE_name);
#endif

	return class_entry;
}

static zend_class_entry *register_class_SQLite3Stmt(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SQLite3Stmt", class_SQLite3Stmt_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}

static zend_class_entry *register_class_SQLite3Result(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SQLite3Result", class_SQLite3Result_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}
