/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: d58317ea6f49a25527827719fc41e2b29667a9de */




static const zend_function_entry class_PDOOdbc_methods[] = {
	ZEND_FE_END
};

static zend_class_entry *register_class_PDOOdbc(zend_class_entry *class_entry_PDO)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "PDOOdbc", class_PDOOdbc_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_PDO);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}
