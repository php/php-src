/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 06c2f52a638c1ca9c56bcbebdc00367bb3eb9b26 */




static const zend_function_entry class_PDODBLIB_methods[] = {
	ZEND_FE_END
};

static zend_class_entry *register_class_PDODBLIB(zend_class_entry *class_entry_PDO)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "PDODBLIB", class_PDODBLIB_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_PDO);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}
