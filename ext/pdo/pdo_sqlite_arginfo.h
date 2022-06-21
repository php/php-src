/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: b0396a798b67a578c9d0c7a1f04c6fc3dcd24691 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_PDOSqlite_createFunction, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, function_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, num_args, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#if !defined(SQLITE_OMIT_LOAD_EXTENSION)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_PDOSqlite_loadExtension, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif


ZEND_METHOD(PDOSqlite, createFunction);
#if !defined(SQLITE_OMIT_LOAD_EXTENSION)
ZEND_METHOD(PDOSqlite, loadExtension);
#endif


static const zend_function_entry class_PDOSqlite_methods[] = {
	ZEND_ME(PDOSqlite, createFunction, arginfo_class_PDOSqlite_createFunction, ZEND_ACC_PUBLIC)
#if !defined(SQLITE_OMIT_LOAD_EXTENSION)
	ZEND_ME(PDOSqlite, loadExtension, arginfo_class_PDOSqlite_loadExtension, ZEND_ACC_PUBLIC)
#endif
	ZEND_FE_END
};

static zend_class_entry *register_class_PDOSqlite(zend_class_entry *class_entry_PDO)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "PDOSqlite", class_PDOSqlite_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_PDO);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}
