/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 151e8bf9fb5870f4b2793c8f1549815ca036c64e */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_PdoMysql_getWarningCount, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(PdoMysql, getWarningCount);


static const zend_function_entry class_PdoMysql_methods[] = {
	ZEND_ME(PdoMysql, getWarningCount, arginfo_class_PdoMysql_getWarningCount, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_PdoMysql(zend_class_entry *class_entry_PDO)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "PdoMysql", class_PdoMysql_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_PDO);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}
