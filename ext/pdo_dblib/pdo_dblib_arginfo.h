/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: d2a7022c8e6e259b452786e00867b4a167d58277 */




static const zend_function_entry class_PdoDblib_methods[] = {
	ZEND_FE_END
};

static zend_class_entry *register_class_PdoDblib(zend_class_entry *class_entry_PDO)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "PdoDblib", class_PdoDblib_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_PDO);
	class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;

	zval const_ATTR_CONNECTION_TIMEOUT_value;
	ZVAL_LONG(&const_ATTR_CONNECTION_TIMEOUT_value, PDO_DBLIB_ATTR_CONNECTION_TIMEOUT);
	zend_string *const_ATTR_CONNECTION_TIMEOUT_name = zend_string_init_interned("ATTR_CONNECTION_TIMEOUT", sizeof("ATTR_CONNECTION_TIMEOUT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_CONNECTION_TIMEOUT_name, &const_ATTR_CONNECTION_TIMEOUT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_CONNECTION_TIMEOUT_name);

	zval const_ATTR_QUERY_TIMEOUT_value;
	ZVAL_LONG(&const_ATTR_QUERY_TIMEOUT_value, PDO_DBLIB_ATTR_QUERY_TIMEOUT);
	zend_string *const_ATTR_QUERY_TIMEOUT_name = zend_string_init_interned("ATTR_QUERY_TIMEOUT", sizeof("ATTR_QUERY_TIMEOUT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_QUERY_TIMEOUT_name, &const_ATTR_QUERY_TIMEOUT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_QUERY_TIMEOUT_name);

	zval const_ATTR_STRINGIFY_UNIQUEIDENTIFIER_value;
	ZVAL_LONG(&const_ATTR_STRINGIFY_UNIQUEIDENTIFIER_value, PDO_DBLIB_ATTR_STRINGIFY_UNIQUEIDENTIFIER);
	zend_string *const_ATTR_STRINGIFY_UNIQUEIDENTIFIER_name = zend_string_init_interned("ATTR_STRINGIFY_UNIQUEIDENTIFIER", sizeof("ATTR_STRINGIFY_UNIQUEIDENTIFIER") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_STRINGIFY_UNIQUEIDENTIFIER_name, &const_ATTR_STRINGIFY_UNIQUEIDENTIFIER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_STRINGIFY_UNIQUEIDENTIFIER_name);

	zval const_ATTR_VERSION_value;
	ZVAL_LONG(&const_ATTR_VERSION_value, PDO_DBLIB_ATTR_VERSION);
	zend_string *const_ATTR_VERSION_name = zend_string_init_interned("ATTR_VERSION", sizeof("ATTR_VERSION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_VERSION_name, &const_ATTR_VERSION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_VERSION_name);

	zval const_ATTR_TDS_VERSION_value;
	ZVAL_LONG(&const_ATTR_TDS_VERSION_value, PDO_DBLIB_ATTR_TDS_VERSION);
	zend_string *const_ATTR_TDS_VERSION_name = zend_string_init_interned("ATTR_TDS_VERSION", sizeof("ATTR_TDS_VERSION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_TDS_VERSION_name, &const_ATTR_TDS_VERSION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_TDS_VERSION_name);

	zval const_ATTR_SKIP_EMPTY_ROWSETS_value;
	ZVAL_LONG(&const_ATTR_SKIP_EMPTY_ROWSETS_value, PDO_DBLIB_ATTR_SKIP_EMPTY_ROWSETS);
	zend_string *const_ATTR_SKIP_EMPTY_ROWSETS_name = zend_string_init_interned("ATTR_SKIP_EMPTY_ROWSETS", sizeof("ATTR_SKIP_EMPTY_ROWSETS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_SKIP_EMPTY_ROWSETS_name, &const_ATTR_SKIP_EMPTY_ROWSETS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_SKIP_EMPTY_ROWSETS_name);

	zval const_ATTR_DATETIME_CONVERT_value;
	ZVAL_LONG(&const_ATTR_DATETIME_CONVERT_value, PDO_DBLIB_ATTR_DATETIME_CONVERT);
	zend_string *const_ATTR_DATETIME_CONVERT_name = zend_string_init_interned("ATTR_DATETIME_CONVERT", sizeof("ATTR_DATETIME_CONVERT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_DATETIME_CONVERT_name, &const_ATTR_DATETIME_CONVERT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_DATETIME_CONVERT_name);

	return class_entry;
}
