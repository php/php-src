/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 49737f1a08f0b1b66e326b7d06e316105202d27d */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO_SQLite_Ext_sqliteCreateFunction, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, numArgs, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO_SQLite_Ext_sqliteCreateAggregate, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, step, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO(0, finalize, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, numArgs, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO_SQLite_Ext_sqliteCreateCollation, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(PDO_SQLite_Ext, sqliteCreateFunction);
ZEND_METHOD(PDO_SQLite_Ext, sqliteCreateAggregate);
ZEND_METHOD(PDO_SQLite_Ext, sqliteCreateCollation);


static const zend_function_entry class_PDO_SQLite_Ext_methods[] = {
	ZEND_ME(PDO_SQLite_Ext, sqliteCreateFunction, arginfo_class_PDO_SQLite_Ext_sqliteCreateFunction, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO_SQLite_Ext, sqliteCreateAggregate, arginfo_class_PDO_SQLite_Ext_sqliteCreateAggregate, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO_SQLite_Ext, sqliteCreateCollation, arginfo_class_PDO_SQLite_Ext_sqliteCreateCollation, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
