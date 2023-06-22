/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: d9cf61aa0f398bb1188638856f46286f107e62de */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_SeqCollection_add, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DictCollection_add, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()




static const zend_function_entry class_SeqCollection_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(SeqCollection, add, arginfo_class_SeqCollection_add, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};


static const zend_function_entry class_DictCollection_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(DictCollection, add, arginfo_class_DictCollection_add, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
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
