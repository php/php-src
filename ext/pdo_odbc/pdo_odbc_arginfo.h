/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 3cc19acf5943c09a011f15d9ef6d0f443d4b8a71 */

static const zend_function_entry class_PdoOdbc_methods[] = {
	ZEND_FE_END
};

static void register_pdo_odbc_symbols(int module_number)
{
	REGISTER_STRING_CONSTANT("PDO_ODBC_TYPE", PDO_ODBC_TYPE, CONST_PERSISTENT);
}

static zend_class_entry *register_class_PdoOdbc(zend_class_entry *class_entry_PDO)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "PdoOdbc", class_PdoOdbc_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_PDO);
	class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;

	zval const_ATTR_USE_CURSOR_LIBRARY_value;
	ZVAL_LONG(&const_ATTR_USE_CURSOR_LIBRARY_value, PDO_ODBC_ATTR_USE_CURSOR_LIBRARY);
	zend_string *const_ATTR_USE_CURSOR_LIBRARY_name = zend_string_init_interned("ATTR_USE_CURSOR_LIBRARY", sizeof("ATTR_USE_CURSOR_LIBRARY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_USE_CURSOR_LIBRARY_name, &const_ATTR_USE_CURSOR_LIBRARY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_USE_CURSOR_LIBRARY_name);

	zval const_ATTR_ASSUME_UTF8_value;
	ZVAL_LONG(&const_ATTR_ASSUME_UTF8_value, PDO_ODBC_ATTR_ASSUME_UTF8);
	zend_string *const_ATTR_ASSUME_UTF8_name = zend_string_init_interned("ATTR_ASSUME_UTF8", sizeof("ATTR_ASSUME_UTF8") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_ASSUME_UTF8_name, &const_ATTR_ASSUME_UTF8_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_ASSUME_UTF8_name);

	zval const_SQL_USE_IF_NEEDED_value;
	ZVAL_LONG(&const_SQL_USE_IF_NEEDED_value, SQL_CUR_USE_IF_NEEDED);
	zend_string *const_SQL_USE_IF_NEEDED_name = zend_string_init_interned("SQL_USE_IF_NEEDED", sizeof("SQL_USE_IF_NEEDED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SQL_USE_IF_NEEDED_name, &const_SQL_USE_IF_NEEDED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SQL_USE_IF_NEEDED_name);

	zval const_SQL_USE_DRIVER_value;
	ZVAL_LONG(&const_SQL_USE_DRIVER_value, SQL_CUR_USE_DRIVER);
	zend_string *const_SQL_USE_DRIVER_name = zend_string_init_interned("SQL_USE_DRIVER", sizeof("SQL_USE_DRIVER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SQL_USE_DRIVER_name, &const_SQL_USE_DRIVER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SQL_USE_DRIVER_name);

	zval const_SQL_USE_ODBC_value;
	ZVAL_LONG(&const_SQL_USE_ODBC_value, SQL_CUR_USE_ODBC);
	zend_string *const_SQL_USE_ODBC_name = zend_string_init_interned("SQL_USE_ODBC", sizeof("SQL_USE_ODBC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SQL_USE_ODBC_name, &const_SQL_USE_ODBC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SQL_USE_ODBC_name);

	return class_entry;
}
