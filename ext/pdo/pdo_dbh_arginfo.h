/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, dsn, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, username, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, passwd, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, options, IS_ARRAY, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO_beginTransaction, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_PDO_commit arginfo_class_PDO_beginTransaction

#define arginfo_class_PDO_errorCode arginfo_class_PDO_beginTransaction

#define arginfo_class_PDO_errorInfo arginfo_class_PDO_beginTransaction

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO_exec, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, statement, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO_getAttribute, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, attribute, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_PDO_getAvailableDrivers arginfo_class_PDO_beginTransaction

#define arginfo_class_PDO_inTransaction arginfo_class_PDO_beginTransaction

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO_lastInsertId, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO_prepare, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, statement, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, driver_options, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_PDO_query arginfo_class_PDO_exec

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO_quote, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, parameter_type, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_PDO_rollBack arginfo_class_PDO_beginTransaction

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO_setAttribute, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, attribute, IS_LONG, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()
