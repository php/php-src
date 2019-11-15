/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, dsn, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, username, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, passwd, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, options, IS_ARRAY, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO_prepare, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, statement, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO_beginTransaction, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_PDO_commit arginfo_class_PDO_beginTransaction

#define arginfo_class_PDO_rollBack arginfo_class_PDO_beginTransaction

#define arginfo_class_PDO_inTransaction arginfo_class_PDO_beginTransaction

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO_setAttribute, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, attribute, IS_LONG, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO_exec, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO_query, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, sql, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO_lastInsertId, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, seqname, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_PDO_errorCode arginfo_class_PDO_beginTransaction

#define arginfo_class_PDO_errorInfo arginfo_class_PDO_beginTransaction

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO_getAttribute, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, attribute, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO_quote, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, paramtype, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_PDO_getAvailableDrivers arginfo_class_PDO_beginTransaction

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_execute, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, bound_input_params, IS_ARRAY, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_fetch, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, how, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, orientation, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_bindParam, 0, 0, 2)
	ZEND_ARG_INFO(0, paramno)
	ZEND_ARG_INFO(1, param)
	ZEND_ARG_INFO(0, type)
	ZEND_ARG_INFO(0, maxlen)
	ZEND_ARG_INFO(0, driverdata)
ZEND_END_ARG_INFO()

#define arginfo_class_PDOStatement_bindColumn arginfo_class_PDOStatement_bindParam

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_bindValue, 0, 0, 2)
	ZEND_ARG_INFO(0, paramno)
	ZEND_ARG_INFO(0, param)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_PDOStatement_rowCount arginfo_class_PDO_beginTransaction

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_fetchColumn, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, column_number, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_fetchAll, 0, 0, 0)
	ZEND_ARG_INFO(0, how)
	ZEND_ARG_INFO(0, class_name)
	ZEND_ARG_INFO(0, ctor_args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_fetchObject, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, class_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, ctor_args, IS_ARRAY, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_PDOStatement_errorCode arginfo_class_PDO_beginTransaction

#define arginfo_class_PDOStatement_errorInfo arginfo_class_PDO_beginTransaction

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_setAttribute, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, attribute, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, value, mixed, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_PDOStatement_getAttribute arginfo_class_PDO_getAttribute

#define arginfo_class_PDOStatement_columnCount arginfo_class_PDO_beginTransaction

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_getColumnMeta, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, column, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDOStatement_setFetchMode, 0, 0, 1)
	ZEND_ARG_INFO(0, mode)
	ZEND_ARG_INFO(0, params)
ZEND_END_ARG_INFO()

#define arginfo_class_PDOStatement_nextRowset arginfo_class_PDO_beginTransaction

#define arginfo_class_PDOStatement_closeCursor arginfo_class_PDO_beginTransaction

#define arginfo_class_PDOStatement_debugDumpParams arginfo_class_PDO_beginTransaction
