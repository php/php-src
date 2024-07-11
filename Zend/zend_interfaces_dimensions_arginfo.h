/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 13678e0beb23fdf1dee9c68729f567d808aa1c2e */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DimensionReadable_offsetGet, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DimensionReadable_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DimensionFetchable_offsetFetch, 1, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DimensionWritable_offsetSet, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DimensionUnsetable_offsetUnset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Appendable_append, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_FetchAppendable_fetchAppend, 1, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ArrayAccess_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ArrayAccess_offsetGet, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ArrayAccess_offsetSet, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_ArrayAccess_offsetUnset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_MIXED, 0)
ZEND_END_ARG_INFO()


static const zend_function_entry class_DimensionReadable_methods[] = {
	ZEND_RAW_FENTRY("offsetGet", NULL, arginfo_class_DimensionReadable_offsetGet, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("offsetExists", NULL, arginfo_class_DimensionReadable_offsetExists, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_DimensionFetchable_methods[] = {
	ZEND_RAW_FENTRY("offsetFetch", NULL, arginfo_class_DimensionFetchable_offsetFetch, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_DimensionWritable_methods[] = {
	ZEND_RAW_FENTRY("offsetSet", NULL, arginfo_class_DimensionWritable_offsetSet, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_DimensionUnsetable_methods[] = {
	ZEND_RAW_FENTRY("offsetUnset", NULL, arginfo_class_DimensionUnsetable_offsetUnset, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_Appendable_methods[] = {
	ZEND_RAW_FENTRY("append", NULL, arginfo_class_Appendable_append, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_FetchAppendable_methods[] = {
	ZEND_RAW_FENTRY("fetchAppend", NULL, arginfo_class_FetchAppendable_fetchAppend, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_ArrayAccess_methods[] = {
	ZEND_RAW_FENTRY("offsetExists", NULL, arginfo_class_ArrayAccess_offsetExists, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("offsetGet", NULL, arginfo_class_ArrayAccess_offsetGet, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("offsetSet", NULL, arginfo_class_ArrayAccess_offsetSet, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("offsetUnset", NULL, arginfo_class_ArrayAccess_offsetUnset, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_FE_END
};

static zend_class_entry *register_class_DimensionReadable(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DimensionReadable", class_DimensionReadable_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}

static zend_class_entry *register_class_DimensionFetchable(zend_class_entry *class_entry_DimensionReadable)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DimensionFetchable", class_DimensionFetchable_methods);
	class_entry = zend_register_internal_interface(&ce);
	zend_class_implements(class_entry, 1, class_entry_DimensionReadable);

	return class_entry;
}

static zend_class_entry *register_class_DimensionWritable(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DimensionWritable", class_DimensionWritable_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}

static zend_class_entry *register_class_DimensionUnsetable(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DimensionUnsetable", class_DimensionUnsetable_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}

static zend_class_entry *register_class_Appendable(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "Appendable", class_Appendable_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}

static zend_class_entry *register_class_FetchAppendable(zend_class_entry *class_entry_Appendable)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "FetchAppendable", class_FetchAppendable_methods);
	class_entry = zend_register_internal_interface(&ce);
	zend_class_implements(class_entry, 1, class_entry_Appendable);

	return class_entry;
}

static zend_class_entry *register_class_ArrayAccess(zend_class_entry *class_entry_DimensionReadable, zend_class_entry *class_entry_DimensionWritable, zend_class_entry *class_entry_DimensionUnsetable)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "ArrayAccess", class_ArrayAccess_methods);
	class_entry = zend_register_internal_interface(&ce);
	zend_class_implements(class_entry, 3, class_entry_DimensionReadable, class_entry_DimensionWritable, class_entry_DimensionUnsetable);

	return class_entry;
}
