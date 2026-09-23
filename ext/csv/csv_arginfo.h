/* This is a generated file, edit csv.stub.php instead.
 * Stub hash: 26520c4122f9dd9b24d8a53c542debc0c22e6752 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Csv_array_to_row, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, fields, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, delimiter, IS_STRING, 0, "\',\'")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enclosure, IS_STRING, 0, "\'\"\'")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, eolSequence, IS_STRING, 0, "\"\\r\\n\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Csv_row_to_array, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, row, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, delimiter, IS_STRING, 0, "\',\'")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enclosure, IS_STRING, 0, "\'\"\'")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, eolSequence, IS_STRING, 0, "\"\\r\\n\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Csv_collection_to_buffer, 0, 1, IS_STRING, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, collection, Traversable, MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, delimiter, IS_STRING, 0, "\',\'")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enclosure, IS_STRING, 0, "\'\"\'")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, eolSequence, IS_STRING, 0, "\"\\r\\n\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Csv_collection_to_file, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, file, IS_STRING, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, collection, Traversable, MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, delimiter, IS_STRING, 0, "\',\'")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enclosure, IS_STRING, 0, "\'\"\'")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, eolSequence, IS_STRING, 0, "\"\\r\\n\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Csv_buffer_to_collection, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, buffer, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, delimiter, IS_STRING, 0, "\',\'")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enclosure, IS_STRING, 0, "\'\"\'")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, eolSequence, IS_STRING, 0, "\"\\r\\n\"")
ZEND_END_ARG_INFO()

#define arginfo_Csv_buffer_to_collection_lax arginfo_Csv_buffer_to_collection

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Csv_LazyLaxCollection___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Csv_LazyLaxCollection_getIterator, 0, 0, InternalIterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Csv_LazyLaxCollection_createFromBuffer, 0, 1, Csv\\LazyLaxCollection, 0)
	ZEND_ARG_TYPE_INFO(0, buffer, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, delimiter, IS_STRING, 0, "\',\'")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enclosure, IS_STRING, 0, "\'\"\'")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, eolSequence, IS_STRING, 0, "\"\\r\\n\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Csv_LazyLaxCollection_createFromFile, 0, 1, Csv\\LazyLaxCollection, 0)
	ZEND_ARG_TYPE_INFO(0, file, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, delimiter, IS_STRING, 0, "\',\'")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enclosure, IS_STRING, 0, "\'\"\'")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, eolSequence, IS_STRING, 0, "\"\\r\\n\"")
ZEND_END_ARG_INFO()

ZEND_FUNCTION(Csv_array_to_row);
ZEND_FUNCTION(Csv_row_to_array);
ZEND_FUNCTION(Csv_collection_to_buffer);
ZEND_FUNCTION(Csv_collection_to_file);
ZEND_FUNCTION(Csv_buffer_to_collection);
ZEND_FUNCTION(Csv_buffer_to_collection_lax);
ZEND_METHOD(Csv_LazyLaxCollection, __construct);
ZEND_METHOD(Csv_LazyLaxCollection, getIterator);
ZEND_METHOD(Csv_LazyLaxCollection, createFromBuffer);
ZEND_METHOD(Csv_LazyLaxCollection, createFromFile);

static const zend_function_entry ext_functions[] = {
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Csv", "array_to_row"), zif_Csv_array_to_row, arginfo_Csv_array_to_row, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Csv", "row_to_array"), zif_Csv_row_to_array, arginfo_Csv_row_to_array, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Csv", "collection_to_buffer"), zif_Csv_collection_to_buffer, arginfo_Csv_collection_to_buffer, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Csv", "collection_to_file"), zif_Csv_collection_to_file, arginfo_Csv_collection_to_file, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Csv", "buffer_to_collection"), zif_Csv_buffer_to_collection, arginfo_Csv_buffer_to_collection, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Csv", "buffer_to_collection_lax"), zif_Csv_buffer_to_collection_lax, arginfo_Csv_buffer_to_collection_lax, 0, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_Csv_LazyLaxCollection_methods[] = {
	ZEND_ME(Csv_LazyLaxCollection, __construct, arginfo_class_Csv_LazyLaxCollection___construct, ZEND_ACC_PRIVATE)
	ZEND_ME(Csv_LazyLaxCollection, getIterator, arginfo_class_Csv_LazyLaxCollection_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(Csv_LazyLaxCollection, createFromBuffer, arginfo_class_Csv_LazyLaxCollection_createFromBuffer, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Csv_LazyLaxCollection, createFromFile, arginfo_class_Csv_LazyLaxCollection_createFromFile, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Csv_LazyLaxCollection(zend_class_entry *class_entry_IteratorAggregate)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Csv", "LazyLaxCollection", class_Csv_LazyLaxCollection_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);
	zend_class_implements(class_entry, 1, class_entry_IteratorAggregate);

	return class_entry;
}
