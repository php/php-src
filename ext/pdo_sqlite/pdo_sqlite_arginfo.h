/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 167d0e30af99bd31c46ea1649c661b6aa38eaccb */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_PdoSqlite_createAggregate, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, step, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO(0, finalize, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, numArgs, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_PdoSqlite_createCollation, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_PdoSqlite_createFunction, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, function_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, num_args, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#if !defined(PDO_SQLITE_OMIT_LOAD_EXTENSION)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_PdoSqlite_loadExtension, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PdoSqlite_openBlob, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, table, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, column, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, rowid, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, dbname, IS_STRING, 1, "\"main\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "PdoSqlite::OPEN_READONLY")
ZEND_END_ARG_INFO()


ZEND_METHOD(PdoSqlite, createAggregate);
ZEND_METHOD(PdoSqlite, createCollation);
ZEND_METHOD(PdoSqlite, createFunction);
#if !defined(PDO_SQLITE_OMIT_LOAD_EXTENSION)
ZEND_METHOD(PdoSqlite, loadExtension);
#endif
ZEND_METHOD(PdoSqlite, openBlob);


static const zend_function_entry class_PdoSqlite_methods[] = {
	ZEND_ME(PdoSqlite, createAggregate, arginfo_class_PdoSqlite_createAggregate, ZEND_ACC_PUBLIC)
	ZEND_ME(PdoSqlite, createCollation, arginfo_class_PdoSqlite_createCollation, ZEND_ACC_PUBLIC)
	ZEND_ME(PdoSqlite, createFunction, arginfo_class_PdoSqlite_createFunction, ZEND_ACC_PUBLIC)
#if !defined(PDO_SQLITE_OMIT_LOAD_EXTENSION)
	ZEND_ME(PdoSqlite, loadExtension, arginfo_class_PdoSqlite_loadExtension, ZEND_ACC_PUBLIC)
#endif
	ZEND_ME(PdoSqlite, openBlob, arginfo_class_PdoSqlite_openBlob, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_PdoSqlite(zend_class_entry *class_entry_PDO)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "PdoSqlite", class_PdoSqlite_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_PDO);
	class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;
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

	return class_entry;
}
