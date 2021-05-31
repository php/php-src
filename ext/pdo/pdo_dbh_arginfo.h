/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: c0f18da92bca9f57b18e58fbc96e47b4195519a2 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, dsn, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, username, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, password, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_PDO_beginTransaction, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_PDO_commit arginfo_class_PDO_beginTransaction

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_PDO_errorCode, 0, 0, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_PDO_errorInfo, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_PDO_exec, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, statement, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_PDO_getAttribute, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, attribute, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_PDO_getAvailableDrivers arginfo_class_PDO_errorInfo

#define arginfo_class_PDO_inTransaction arginfo_class_PDO_beginTransaction

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_PDO_lastInsertId, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, name, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_PDO_prepare, 0, 1, PDOStatement, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_PDO_query, 0, 1, PDOStatement, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, fetchMode, IS_LONG, 1, "null")
	ZEND_ARG_VARIADIC_TYPE_INFO(0, fetchModeArgs, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_PDO_quote, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "PDO::PARAM_STR")
ZEND_END_ARG_INFO()

#define arginfo_class_PDO_rollBack arginfo_class_PDO_beginTransaction

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_PDO_setAttribute, 0, 2, _IS_BOOL, 0)
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

static zend_class_entry *register_class_PDO(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "PDO", class_PDO_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	return class_entry;
}
