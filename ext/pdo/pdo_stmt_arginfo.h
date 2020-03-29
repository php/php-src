/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_bindColumn, 0, 0, 2)
	ZEND_ARG_TYPE_MASK(0, column, MAY_BE_LONG|MAY_BE_STRING)
	ZEND_ARG_INFO(1, param)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, maxlen, IS_LONG, 0)
	ZEND_ARG_INFO(0, driverdata)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_bindParam, 0, 0, 2)
	ZEND_ARG_TYPE_MASK(0, parameter, MAY_BE_LONG|MAY_BE_STRING)
	ZEND_ARG_INFO(1, param)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, maxlen, IS_LONG, 0)
	ZEND_ARG_INFO(0, driverdata)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_bindValue, 0, 0, 2)
	ZEND_ARG_INFO(0, parameter)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_closeCursor, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_PDOStatement_columnCount arginfo_class_PDOStatement_closeCursor

#define arginfo_class_PDOStatement_debugDumpParams arginfo_class_PDOStatement_closeCursor

#define arginfo_class_PDOStatement_errorCode arginfo_class_PDOStatement_closeCursor

#define arginfo_class_PDOStatement_errorInfo arginfo_class_PDOStatement_closeCursor

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_execute, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, input_parameters, IS_ARRAY, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_fetch, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, fetch_style, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, cursor_orientation, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, cursor_offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_fetchAll, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, fetch_style, IS_LONG, 0)
	ZEND_ARG_INFO(0, fetch_argument)
	ZEND_ARG_TYPE_INFO(0, ctor_args, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_fetchColumn, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, column_number, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_fetchObject, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, class_name, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, ctor_args, IS_ARRAY, 1)
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
