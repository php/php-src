/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 949c01f6e018c7abeb29f2ed699e98ed2c0553a9 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_PdoSqlite_createFunction, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, function_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, num_args, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#if !defined(SQLITE_OMIT_LOAD_EXTENSION)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_PdoSqlite_loadExtension, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_PdoSqlite_openBlob, 0, 3, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, table, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, column, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, rowid, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, dbname, IS_STRING, 1, "\"main\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "PDO::SQLITE_OPEN_READONLY")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_PdoSqlite_createAggregate, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, step, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO(0, finalize, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, numArgs, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_PdoSqlite_createCollation, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(PdoSqlite, createFunction);
#if !defined(SQLITE_OMIT_LOAD_EXTENSION)
ZEND_METHOD(PdoSqlite, loadExtension);
#endif
ZEND_METHOD(PdoSqlite, openBlob);
ZEND_METHOD(PdoSqlite, createAggregate);
ZEND_METHOD(PdoSqlite, createCollation);


static const zend_function_entry class_PdoSqlite_methods[] = {
	ZEND_ME(PdoSqlite, createFunction, arginfo_class_PdoSqlite_createFunction, ZEND_ACC_PUBLIC)
#if !defined(SQLITE_OMIT_LOAD_EXTENSION)
	ZEND_ME(PdoSqlite, loadExtension, arginfo_class_PdoSqlite_loadExtension, ZEND_ACC_PUBLIC)
#endif
	ZEND_ME(PdoSqlite, openBlob, arginfo_class_PdoSqlite_openBlob, ZEND_ACC_PUBLIC)
	ZEND_ME(PdoSqlite, createAggregate, arginfo_class_PdoSqlite_createAggregate, ZEND_ACC_PUBLIC)
	ZEND_ME(PdoSqlite, createCollation, arginfo_class_PdoSqlite_createCollation, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_PdoSqlite(zend_class_entry *class_entry_PDO)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "PdoSqlite", class_PdoSqlite_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_PDO);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}
