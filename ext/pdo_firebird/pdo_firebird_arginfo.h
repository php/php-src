/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 8b089db6fa589edf596ed0564751ad60507ed0dd */




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

	return class_entry;
}
