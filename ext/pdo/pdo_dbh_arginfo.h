/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 36270d1418fc4ddd8f79018372b0ef00fb6f5889 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, dsn, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, username, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, passwd, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
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
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, name, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO_prepare, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, statement, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, driver_options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO_query, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, statement, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, fetch_mode, IS_LONG, 1, "null")
	ZEND_ARG_VARIADIC_INFO(0, fetch_mode_args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO_quote, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, parameter_type, IS_LONG, 0, "PDO::PARAM_STR")
ZEND_END_ARG_INFO()

#define arginfo_class_PDO_rollBack arginfo_class_PDO_beginTransaction

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO_setAttribute, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, attribute, IS_LONG, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()


ZEND_METHOD(PDO, __construct);
ZEND_METHOD(PDO, beginTransaction);
ZEND_METHOD(PDO, commit);
ZEND_METHOD(PDO, errorCode);
ZEND_METHOD(PDO, errorInfo);
ZEND_METHOD(PDO, exec);
ZEND_METHOD(PDO, getAttribute);
ZEND_METHOD(PDO, getAvailableDrivers);
ZEND_METHOD(PDO, inTransaction);
ZEND_METHOD(PDO, lastInsertId);
ZEND_METHOD(PDO, prepare);
ZEND_METHOD(PDO, query);
ZEND_METHOD(PDO, quote);
ZEND_METHOD(PDO, rollBack);
ZEND_METHOD(PDO, setAttribute);


static const zend_function_entry class_PDO_methods[] = {
	ZEND_ME(PDO, __construct, arginfo_class_PDO___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO, beginTransaction, arginfo_class_PDO_beginTransaction, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO, commit, arginfo_class_PDO_commit, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO, errorCode, arginfo_class_PDO_errorCode, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO, errorInfo, arginfo_class_PDO_errorInfo, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO, exec, arginfo_class_PDO_exec, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO, getAttribute, arginfo_class_PDO_getAttribute, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO, getAvailableDrivers, arginfo_class_PDO_getAvailableDrivers, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(PDO, inTransaction, arginfo_class_PDO_inTransaction, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO, lastInsertId, arginfo_class_PDO_lastInsertId, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO, prepare, arginfo_class_PDO_prepare, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO, query, arginfo_class_PDO_query, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO, quote, arginfo_class_PDO_quote, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO, rollBack, arginfo_class_PDO_rollBack, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO, setAttribute, arginfo_class_PDO_setAttribute, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
