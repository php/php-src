/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 037c073de9e3e3593c62cb7da8ee7202c38c2723 */

static const zend_function_entry class_PdoFirebird_methods[] = {
	ZEND_FE_END
};

static zend_class_entry *register_class_PdoFirebird(zend_class_entry *class_entry_PDO)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "PdoFirebird", class_PdoFirebird_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_PDO);
	class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;

	zval const_ATTR_DATE_FORMAT_value;
	ZVAL_LONG(&const_ATTR_DATE_FORMAT_value, PDO_FB_ATTR_DATE_FORMAT);
	zend_string *const_ATTR_DATE_FORMAT_name = zend_string_init_interned("ATTR_DATE_FORMAT", sizeof("ATTR_DATE_FORMAT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_DATE_FORMAT_name, &const_ATTR_DATE_FORMAT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_DATE_FORMAT_name);

	zval const_ATTR_TIME_FORMAT_value;
	ZVAL_LONG(&const_ATTR_TIME_FORMAT_value, PDO_FB_ATTR_TIME_FORMAT);
	zend_string *const_ATTR_TIME_FORMAT_name = zend_string_init_interned("ATTR_TIME_FORMAT", sizeof("ATTR_TIME_FORMAT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_TIME_FORMAT_name, &const_ATTR_TIME_FORMAT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_TIME_FORMAT_name);

	zval const_ATTR_TIMESTAMP_FORMAT_value;
	ZVAL_LONG(&const_ATTR_TIMESTAMP_FORMAT_value, PDO_FB_ATTR_TIMESTAMP_FORMAT);
	zend_string *const_ATTR_TIMESTAMP_FORMAT_name = zend_string_init_interned("ATTR_TIMESTAMP_FORMAT", sizeof("ATTR_TIMESTAMP_FORMAT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_TIMESTAMP_FORMAT_name, &const_ATTR_TIMESTAMP_FORMAT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_TIMESTAMP_FORMAT_name);

	zval const_TRANSACTION_ISOLATION_LEVEL_value;
	ZVAL_LONG(&const_TRANSACTION_ISOLATION_LEVEL_value, PDO_FB_TRANSACTION_ISOLATION_LEVEL);
	zend_string *const_TRANSACTION_ISOLATION_LEVEL_name = zend_string_init_interned("TRANSACTION_ISOLATION_LEVEL", sizeof("TRANSACTION_ISOLATION_LEVEL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TRANSACTION_ISOLATION_LEVEL_name, &const_TRANSACTION_ISOLATION_LEVEL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TRANSACTION_ISOLATION_LEVEL_name);

	zval const_READ_COMMITTED_value;
	ZVAL_LONG(&const_READ_COMMITTED_value, PDO_FB_READ_COMMITTED);
	zend_string *const_READ_COMMITTED_name = zend_string_init_interned("READ_COMMITTED", sizeof("READ_COMMITTED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_READ_COMMITTED_name, &const_READ_COMMITTED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_READ_COMMITTED_name);

	zval const_REPEATABLE_READ_value;
	ZVAL_LONG(&const_REPEATABLE_READ_value, PDO_FB_REPEATABLE_READ);
	zend_string *const_REPEATABLE_READ_name = zend_string_init_interned("REPEATABLE_READ", sizeof("REPEATABLE_READ") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_REPEATABLE_READ_name, &const_REPEATABLE_READ_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_REPEATABLE_READ_name);

	zval const_SERIALIZABLE_value;
	ZVAL_LONG(&const_SERIALIZABLE_value, PDO_FB_SERIALIZABLE);
	zend_string *const_SERIALIZABLE_name = zend_string_init_interned("SERIALIZABLE", sizeof("SERIALIZABLE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_SERIALIZABLE_name, &const_SERIALIZABLE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_SERIALIZABLE_name);

	zval const_WRITABLE_TRANSACTION_value;
	ZVAL_LONG(&const_WRITABLE_TRANSACTION_value, PDO_FB_WRITABLE_TRANSACTION);
	zend_string *const_WRITABLE_TRANSACTION_name = zend_string_init_interned("WRITABLE_TRANSACTION", sizeof("WRITABLE_TRANSACTION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_WRITABLE_TRANSACTION_name, &const_WRITABLE_TRANSACTION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_WRITABLE_TRANSACTION_name);

	return class_entry;
}
