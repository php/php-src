/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 7aa8f420e9fb24633be2f72a061b32f4d9207620 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_PDOPgSql_escapeIdentifier, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, input, IS_STRING, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(PDOPgSql, escapeIdentifier);


static const zend_function_entry class_PDOPgSql_methods[] = {
	ZEND_ME(PDOPgSql, escapeIdentifier, arginfo_class_PDOPgSql_escapeIdentifier, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_PDOPgSql(zend_class_entry *class_entry_PDO)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "PDOPgSql", class_PDOPgSql_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_PDO);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}
