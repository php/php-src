/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: ec5de160dfbacf129b1377ea555ed62442d212ec */






static const zend_function_entry class_SeqCollection_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_DictCollection_methods[] = {
	ZEND_FE_END
};

static zend_class_entry *register_class_SeqCollection(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SeqCollection", class_SeqCollection_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}

static zend_class_entry *register_class_DictCollection(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DictCollection", class_DictCollection_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}
