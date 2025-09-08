/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 721c46905fa8fb1e18d7196ed85c37f56049ea33 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Pdo_Sqlite_createAggregate, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, step, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO(0, finalize, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, numArgs, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Pdo_Sqlite_createCollation, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Pdo_Sqlite_createFunction, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, function_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, num_args, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#if !defined(PDO_SQLITE_OMIT_LOAD_EXTENSION)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Pdo_Sqlite_loadExtension, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Pdo_Sqlite_openBlob, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, table, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, column, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, rowid, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, dbname, IS_STRING, 1, "\"main\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "Pdo\\Sqlite::OPEN_READONLY")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Pdo_Sqlite_setAuthorizer, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 1)
ZEND_END_ARG_INFO()

ZEND_METHOD(Pdo_Sqlite, createAggregate);
ZEND_METHOD(Pdo_Sqlite, createCollation);
ZEND_METHOD(Pdo_Sqlite, createFunction);
#if !defined(PDO_SQLITE_OMIT_LOAD_EXTENSION)
ZEND_METHOD(Pdo_Sqlite, loadExtension);
#endif
ZEND_METHOD(Pdo_Sqlite, openBlob);
ZEND_METHOD(Pdo_Sqlite, setAuthorizer);

static const zend_function_entry class_Pdo_Sqlite_methods[] = {
	ZEND_ME(Pdo_Sqlite, createAggregate, arginfo_class_Pdo_Sqlite_createAggregate, ZEND_ACC_PUBLIC)
	ZEND_ME(Pdo_Sqlite, createCollation, arginfo_class_Pdo_Sqlite_createCollation, ZEND_ACC_PUBLIC)
	ZEND_ME(Pdo_Sqlite, createFunction, arginfo_class_Pdo_Sqlite_createFunction, ZEND_ACC_PUBLIC)
#if !defined(PDO_SQLITE_OMIT_LOAD_EXTENSION)
	ZEND_ME(Pdo_Sqlite, loadExtension, arginfo_class_Pdo_Sqlite_loadExtension, ZEND_ACC_PUBLIC)
#endif
	ZEND_ME(Pdo_Sqlite, openBlob, arginfo_class_Pdo_Sqlite_openBlob, ZEND_ACC_PUBLIC)
	ZEND_ME(Pdo_Sqlite, setAuthorizer, arginfo_class_Pdo_Sqlite_setAuthorizer, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Pdo_Sqlite(zend_class_entry *class_entry_PDO)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Pdo", "Sqlite", class_Pdo_Sqlite_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_PDO, ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);
#if defined(SQLITE_DETERMINISTIC)

	zval const_DETERMINISTIC_value;
	ZVAL_LONG(&const_DETERMINISTIC_value, SQLITE_DETERMINISTIC);
	zend_string *const_DETERMINISTIC_name = zend_string_init_interned("DETERMINISTIC", sizeof("DETERMINISTIC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DETERMINISTIC_name, &const_DETERMINISTIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DETERMINISTIC_name);
#endif

	zval const_OPEN_READONLY_value;
	ZVAL_LONG(&const_OPEN_READONLY_value, SQLITE_OPEN_READONLY);
	zend_string *const_OPEN_READONLY_name = zend_string_init_interned("OPEN_READONLY", sizeof("OPEN_READONLY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_OPEN_READONLY_name, &const_OPEN_READONLY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_OPEN_READONLY_name);

	zval const_OPEN_READWRITE_value;
	ZVAL_LONG(&const_OPEN_READWRITE_value, SQLITE_OPEN_READWRITE);
	zend_string *const_OPEN_READWRITE_name = zend_string_init_interned("OPEN_READWRITE", sizeof("OPEN_READWRITE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_OPEN_READWRITE_name, &const_OPEN_READWRITE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_OPEN_READWRITE_name);

	zval const_OPEN_CREATE_value;
	ZVAL_LONG(&const_OPEN_CREATE_value, SQLITE_OPEN_CREATE);
	zend_string *const_OPEN_CREATE_name = zend_string_init_interned("OPEN_CREATE", sizeof("OPEN_CREATE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_OPEN_CREATE_name, &const_OPEN_CREATE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_OPEN_CREATE_name);

	zval const_ATTR_OPEN_FLAGS_value;
	ZVAL_LONG(&const_ATTR_OPEN_FLAGS_value, PDO_SQLITE_ATTR_OPEN_FLAGS);
	zend_string *const_ATTR_OPEN_FLAGS_name = zend_string_init_interned("ATTR_OPEN_FLAGS", sizeof("ATTR_OPEN_FLAGS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_OPEN_FLAGS_name, &const_ATTR_OPEN_FLAGS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_OPEN_FLAGS_name);

	zval const_ATTR_READONLY_STATEMENT_value;
	ZVAL_LONG(&const_ATTR_READONLY_STATEMENT_value, PDO_SQLITE_ATTR_READONLY_STATEMENT);
	zend_string *const_ATTR_READONLY_STATEMENT_name = zend_string_init_interned("ATTR_READONLY_STATEMENT", sizeof("ATTR_READONLY_STATEMENT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_READONLY_STATEMENT_name, &const_ATTR_READONLY_STATEMENT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_READONLY_STATEMENT_name);

	zval const_ATTR_EXTENDED_RESULT_CODES_value;
	ZVAL_LONG(&const_ATTR_EXTENDED_RESULT_CODES_value, PDO_SQLITE_ATTR_EXTENDED_RESULT_CODES);
	zend_string *const_ATTR_EXTENDED_RESULT_CODES_name = zend_string_init_interned("ATTR_EXTENDED_RESULT_CODES", sizeof("ATTR_EXTENDED_RESULT_CODES") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_EXTENDED_RESULT_CODES_name, &const_ATTR_EXTENDED_RESULT_CODES_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_EXTENDED_RESULT_CODES_name);

	zval const_ATTR_BUSY_STATEMENT_value;
	ZVAL_LONG(&const_ATTR_BUSY_STATEMENT_value, PDO_SQLITE_ATTR_BUSY_STATEMENT);
	zend_string *const_ATTR_BUSY_STATEMENT_name = zend_string_init_interned("ATTR_BUSY_STATEMENT", sizeof("ATTR_BUSY_STATEMENT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_BUSY_STATEMENT_name, &const_ATTR_BUSY_STATEMENT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_BUSY_STATEMENT_name);

	zval const_ATTR_EXPLAIN_STATEMENT_value;
	ZVAL_LONG(&const_ATTR_EXPLAIN_STATEMENT_value, PDO_SQLITE_ATTR_EXPLAIN_STATEMENT);
	zend_string *const_ATTR_EXPLAIN_STATEMENT_name = zend_string_init_interned("ATTR_EXPLAIN_STATEMENT", sizeof("ATTR_EXPLAIN_STATEMENT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_EXPLAIN_STATEMENT_name, &const_ATTR_EXPLAIN_STATEMENT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_EXPLAIN_STATEMENT_name);

	zval const_ATTR_TRANSACTION_MODE_value;
	ZVAL_LONG(&const_ATTR_TRANSACTION_MODE_value, PDO_SQLITE_ATTR_TRANSACTION_MODE);
	zend_string *const_ATTR_TRANSACTION_MODE_name = zend_string_init_interned("ATTR_TRANSACTION_MODE", sizeof("ATTR_TRANSACTION_MODE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_TRANSACTION_MODE_name, &const_ATTR_TRANSACTION_MODE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_TRANSACTION_MODE_name);

	zval const_TRANSACTION_MODE_DEFERRED_value;
	ZVAL_LONG(&const_TRANSACTION_MODE_DEFERRED_value, 0);
	zend_string *const_TRANSACTION_MODE_DEFERRED_name = zend_string_init_interned("TRANSACTION_MODE_DEFERRED", sizeof("TRANSACTION_MODE_DEFERRED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TRANSACTION_MODE_DEFERRED_name, &const_TRANSACTION_MODE_DEFERRED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TRANSACTION_MODE_DEFERRED_name);

	zval const_TRANSACTION_MODE_IMMEDIATE_value;
	ZVAL_LONG(&const_TRANSACTION_MODE_IMMEDIATE_value, 1);
	zend_string *const_TRANSACTION_MODE_IMMEDIATE_name = zend_string_init_interned("TRANSACTION_MODE_IMMEDIATE", sizeof("TRANSACTION_MODE_IMMEDIATE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TRANSACTION_MODE_IMMEDIATE_name, &const_TRANSACTION_MODE_IMMEDIATE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TRANSACTION_MODE_IMMEDIATE_name);

	zval const_TRANSACTION_MODE_EXCLUSIVE_value;
	ZVAL_LONG(&const_TRANSACTION_MODE_EXCLUSIVE_value, 2);
	zend_string *const_TRANSACTION_MODE_EXCLUSIVE_name = zend_string_init_interned("TRANSACTION_MODE_EXCLUSIVE", sizeof("TRANSACTION_MODE_EXCLUSIVE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TRANSACTION_MODE_EXCLUSIVE_name, &const_TRANSACTION_MODE_EXCLUSIVE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TRANSACTION_MODE_EXCLUSIVE_name);
#if SQLITE_VERSION_NUMBER >= 3043000

	zval const_EXPLAIN_MODE_PREPARED_value;
	ZVAL_LONG(&const_EXPLAIN_MODE_PREPARED_value, 0);
	zend_string *const_EXPLAIN_MODE_PREPARED_name = zend_string_init_interned("EXPLAIN_MODE_PREPARED", sizeof("EXPLAIN_MODE_PREPARED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_EXPLAIN_MODE_PREPARED_name, &const_EXPLAIN_MODE_PREPARED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_EXPLAIN_MODE_PREPARED_name);

	zval const_EXPLAIN_MODE_EXPLAIN_value;
	ZVAL_LONG(&const_EXPLAIN_MODE_EXPLAIN_value, 1);
	zend_string *const_EXPLAIN_MODE_EXPLAIN_name = zend_string_init_interned("EXPLAIN_MODE_EXPLAIN", sizeof("EXPLAIN_MODE_EXPLAIN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_EXPLAIN_MODE_EXPLAIN_name, &const_EXPLAIN_MODE_EXPLAIN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_EXPLAIN_MODE_EXPLAIN_name);

	zval const_EXPLAIN_MODE_EXPLAIN_QUERY_PLAN_value;
	ZVAL_LONG(&const_EXPLAIN_MODE_EXPLAIN_QUERY_PLAN_value, 2);
	zend_string *const_EXPLAIN_MODE_EXPLAIN_QUERY_PLAN_name = zend_string_init_interned("EXPLAIN_MODE_EXPLAIN_QUERY_PLAN", sizeof("EXPLAIN_MODE_EXPLAIN_QUERY_PLAN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_EXPLAIN_MODE_EXPLAIN_QUERY_PLAN_name, &const_EXPLAIN_MODE_EXPLAIN_QUERY_PLAN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_EXPLAIN_MODE_EXPLAIN_QUERY_PLAN_name);
#endif

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

	return class_entry;
}
