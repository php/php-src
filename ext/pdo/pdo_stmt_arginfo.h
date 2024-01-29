/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: adcbda7b6763141981700bec5d8c5b739f8de767 */

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_PDOStatement_bindColumn, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_MASK(0, column, MAY_BE_STRING|MAY_BE_LONG, NULL)
	ZEND_ARG_TYPE_INFO(1, var, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "PDO::PARAM_STR")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, maxLength, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, driverOptions, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_PDOStatement_bindParam, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_MASK(0, param, MAY_BE_STRING|MAY_BE_LONG, NULL)
	ZEND_ARG_TYPE_INFO(1, var, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "PDO::PARAM_STR")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, maxLength, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, driverOptions, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_PDOStatement_bindValue, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_MASK(0, param, MAY_BE_STRING|MAY_BE_LONG, NULL)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "PDO::PARAM_STR")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_PDOStatement_closeCursor, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_PDOStatement_columnCount, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_PDOStatement_debugDumpParams, 0, 0, _IS_BOOL, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_PDOStatement_errorCode, 0, 0, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_PDOStatement_errorInfo, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_PDOStatement_execute, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, params, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_PDOStatement_fetch, 0, 0, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "PDO::FETCH_DEFAULT")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, cursorOrientation, IS_LONG, 0, "PDO::FETCH_ORI_NEXT")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, cursorOffset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_PDOStatement_fetchAll, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "PDO::FETCH_DEFAULT")
	ZEND_ARG_VARIADIC_TYPE_INFO(0, args, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_PDOStatement_fetchColumn, 0, 0, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, column, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_PDOStatement_fetchObject, 0, 0, MAY_BE_OBJECT|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, class, IS_STRING, 1, "\"stdClass\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, constructorArgs, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_PDOStatement_getAttribute, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_PDOStatement_getColumnMeta, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, column, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_PDOStatement_nextRowset arginfo_class_PDOStatement_closeCursor

#define arginfo_class_PDOStatement_rowCount arginfo_class_PDOStatement_columnCount

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_PDOStatement_setAttribute, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, attribute, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_setFetchMode, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, args, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_PDOStatement_getIterator, 0, 0, Iterator, 0)
ZEND_END_ARG_INFO()

ZEND_METHOD(PDOStatement, bindColumn);
ZEND_METHOD(PDOStatement, bindParam);
ZEND_METHOD(PDOStatement, bindValue);
ZEND_METHOD(PDOStatement, closeCursor);
ZEND_METHOD(PDOStatement, columnCount);
ZEND_METHOD(PDOStatement, debugDumpParams);
ZEND_METHOD(PDOStatement, errorCode);
ZEND_METHOD(PDOStatement, errorInfo);
ZEND_METHOD(PDOStatement, execute);
ZEND_METHOD(PDOStatement, fetch);
ZEND_METHOD(PDOStatement, fetchAll);
ZEND_METHOD(PDOStatement, fetchColumn);
ZEND_METHOD(PDOStatement, fetchObject);
ZEND_METHOD(PDOStatement, getAttribute);
ZEND_METHOD(PDOStatement, getColumnMeta);
ZEND_METHOD(PDOStatement, nextRowset);
ZEND_METHOD(PDOStatement, rowCount);
ZEND_METHOD(PDOStatement, setAttribute);
ZEND_METHOD(PDOStatement, setFetchMode);
ZEND_METHOD(PDOStatement, getIterator);

static const zend_function_entry class_PDOStatement_methods[] = {
	ZEND_RAW_FENTRY("bindColumn", zim_PDOStatement_bindColumn, arginfo_class_PDOStatement_bindColumn, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("bindParam", zim_PDOStatement_bindParam, arginfo_class_PDOStatement_bindParam, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("bindValue", zim_PDOStatement_bindValue, arginfo_class_PDOStatement_bindValue, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("closeCursor", zim_PDOStatement_closeCursor, arginfo_class_PDOStatement_closeCursor, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("columnCount", zim_PDOStatement_columnCount, arginfo_class_PDOStatement_columnCount, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("debugDumpParams", zim_PDOStatement_debugDumpParams, arginfo_class_PDOStatement_debugDumpParams, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("errorCode", zim_PDOStatement_errorCode, arginfo_class_PDOStatement_errorCode, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("errorInfo", zim_PDOStatement_errorInfo, arginfo_class_PDOStatement_errorInfo, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("execute", zim_PDOStatement_execute, arginfo_class_PDOStatement_execute, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("fetch", zim_PDOStatement_fetch, arginfo_class_PDOStatement_fetch, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("fetchAll", zim_PDOStatement_fetchAll, arginfo_class_PDOStatement_fetchAll, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("fetchColumn", zim_PDOStatement_fetchColumn, arginfo_class_PDOStatement_fetchColumn, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("fetchObject", zim_PDOStatement_fetchObject, arginfo_class_PDOStatement_fetchObject, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("getAttribute", zim_PDOStatement_getAttribute, arginfo_class_PDOStatement_getAttribute, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("getColumnMeta", zim_PDOStatement_getColumnMeta, arginfo_class_PDOStatement_getColumnMeta, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("nextRowset", zim_PDOStatement_nextRowset, arginfo_class_PDOStatement_nextRowset, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("rowCount", zim_PDOStatement_rowCount, arginfo_class_PDOStatement_rowCount, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("setAttribute", zim_PDOStatement_setAttribute, arginfo_class_PDOStatement_setAttribute, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("setFetchMode", zim_PDOStatement_setFetchMode, arginfo_class_PDOStatement_setFetchMode, ZEND_ACC_PUBLIC, NULL)
	ZEND_RAW_FENTRY("getIterator", zim_PDOStatement_getIterator, arginfo_class_PDOStatement_getIterator, ZEND_ACC_PUBLIC, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_PDORow_methods[] = {
	ZEND_FE_END
};

static zend_class_entry *register_class_PDOStatement(zend_class_entry *class_entry_IteratorAggregate)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "PDOStatement", class_PDOStatement_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;
	zend_class_implements(class_entry, 1, class_entry_IteratorAggregate);

	zval property_queryString_default_value;
	ZVAL_UNDEF(&property_queryString_default_value);
	zend_string *property_queryString_name = zend_string_init("queryString", sizeof("queryString") - 1, 1);
	zend_declare_typed_property(class_entry, property_queryString_name, &property_queryString_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_queryString_name);

	return class_entry;
}

static zend_class_entry *register_class_PDORow(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "PDORow", class_PDORow_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NOT_SERIALIZABLE;

	zval property_queryString_default_value;
	ZVAL_UNDEF(&property_queryString_default_value);
	zend_string *property_queryString_name = zend_string_init("queryString", sizeof("queryString") - 1, 1);
	zend_declare_typed_property(class_entry, property_queryString_name, &property_queryString_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_queryString_name);

	return class_entry;
}
