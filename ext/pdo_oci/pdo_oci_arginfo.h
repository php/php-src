/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 972d15af35e36d0ece135ee3cfa23706b6024df3 */




static const zend_function_entry class_PDOOci_methods[] = {
	ZEND_FE_END
};

static zend_class_entry *register_class_PDOOci(zend_class_entry *class_entry_PDO)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "PDOOci", class_PDOOci_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_PDO);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}
