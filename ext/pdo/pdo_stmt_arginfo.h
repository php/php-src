/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_bindColumn, 0, 0, 2)
	ZEND_ARG_TYPE_MASK(0, column, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_INFO(1, param)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, maxlen, IS_LONG, 0, "0")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, driverdata, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_bindParam, 0, 0, 2)
	ZEND_ARG_TYPE_MASK(0, parameter, MAY_BE_LONG|MAY_BE_STRING, NULL)
	ZEND_ARG_INFO(1, param)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "PDO::PARAM_STR")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, maxlen, IS_LONG, 0, "0")
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, driverdata, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_bindValue, 0, 0, 2)
	ZEND_ARG_INFO(0, parameter)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "PDO::PARAM_STR")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_closeCursor, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_PDOStatement_columnCount arginfo_class_PDOStatement_closeCursor

#define arginfo_class_PDOStatement_debugDumpParams arginfo_class_PDOStatement_closeCursor

#define arginfo_class_PDOStatement_errorCode arginfo_class_PDOStatement_closeCursor

#define arginfo_class_PDOStatement_errorInfo arginfo_class_PDOStatement_closeCursor

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_execute, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, input_parameters, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_fetch, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, fetch_style, IS_LONG, 0, "PDO::FETCH_BOTH")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, cursor_orientation, IS_LONG, 0, "PDO::FETCH_ORI_NEXT")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, cursor_offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_fetchAll, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, fetch_style, IS_LONG, 0, "PDO::FETCH_BOTH")
	ZEND_ARG_INFO(0, fetch_argument)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, ctor_args, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_fetchColumn, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, column_number, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_fetchObject, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, class_name, IS_STRING, 1, "\"stdClass\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, ctor_args, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_getAttribute, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, attribute, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_getColumnMeta, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, column, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_PDOStatement_nextRowset arginfo_class_PDOStatement_closeCursor

#define arginfo_class_PDOStatement_rowCount arginfo_class_PDOStatement_closeCursor

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_setAttribute, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, attribute, IS_LONG, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_setFetchMode, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
	ZEND_ARG_INFO(0, param1)
	ZEND_ARG_INFO(0, param2)
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


static const zend_function_entry class_PDOStatement_methods[] = {
	ZEND_ME(PDOStatement, bindColumn, arginfo_class_PDOStatement_bindColumn, ZEND_ACC_PUBLIC)
	ZEND_ME(PDOStatement, bindParam, arginfo_class_PDOStatement_bindParam, ZEND_ACC_PUBLIC)
	ZEND_ME(PDOStatement, bindValue, arginfo_class_PDOStatement_bindValue, ZEND_ACC_PUBLIC)
	ZEND_ME(PDOStatement, closeCursor, arginfo_class_PDOStatement_closeCursor, ZEND_ACC_PUBLIC)
	ZEND_ME(PDOStatement, columnCount, arginfo_class_PDOStatement_columnCount, ZEND_ACC_PUBLIC)
	ZEND_ME(PDOStatement, debugDumpParams, arginfo_class_PDOStatement_debugDumpParams, ZEND_ACC_PUBLIC)
	ZEND_ME(PDOStatement, errorCode, arginfo_class_PDOStatement_errorCode, ZEND_ACC_PUBLIC)
	ZEND_ME(PDOStatement, errorInfo, arginfo_class_PDOStatement_errorInfo, ZEND_ACC_PUBLIC)
	ZEND_ME(PDOStatement, execute, arginfo_class_PDOStatement_execute, ZEND_ACC_PUBLIC)
	ZEND_ME(PDOStatement, fetch, arginfo_class_PDOStatement_fetch, ZEND_ACC_PUBLIC)
	ZEND_ME(PDOStatement, fetchAll, arginfo_class_PDOStatement_fetchAll, ZEND_ACC_PUBLIC)
	ZEND_ME(PDOStatement, fetchColumn, arginfo_class_PDOStatement_fetchColumn, ZEND_ACC_PUBLIC)
	ZEND_ME(PDOStatement, fetchObject, arginfo_class_PDOStatement_fetchObject, ZEND_ACC_PUBLIC)
	ZEND_ME(PDOStatement, getAttribute, arginfo_class_PDOStatement_getAttribute, ZEND_ACC_PUBLIC)
	ZEND_ME(PDOStatement, getColumnMeta, arginfo_class_PDOStatement_getColumnMeta, ZEND_ACC_PUBLIC)
	ZEND_ME(PDOStatement, nextRowset, arginfo_class_PDOStatement_nextRowset, ZEND_ACC_PUBLIC)
	ZEND_ME(PDOStatement, rowCount, arginfo_class_PDOStatement_rowCount, ZEND_ACC_PUBLIC)
	ZEND_ME(PDOStatement, setAttribute, arginfo_class_PDOStatement_setAttribute, ZEND_ACC_PUBLIC)
	ZEND_ME(PDOStatement, setFetchMode, arginfo_class_PDOStatement_setFetchMode, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_PDORow_methods[] = {
	ZEND_FE_END
};
