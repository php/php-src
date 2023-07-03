/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 8183c1fc28fe958e997673b93c498812e2a3bef2 */




static const zend_function_entry class_PdoFirebird_methods[] = {
	ZEND_FE_END
};

static zend_class_entry *register_class_PdoFirebird(zend_class_entry *class_entry_PDO)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "PdoFirebird", class_PdoFirebird_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_PDO);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}
