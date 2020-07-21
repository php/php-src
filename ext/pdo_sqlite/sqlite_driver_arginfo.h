/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: e367675f85371fb06484e39dd6ccb3433766ffb8 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO_SQLite_Ext_sqliteCreateFunction, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, function_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, num_args, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO_SQLite_Ext_sqliteCreateAggregate, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, function_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, step_func, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO(0, finalize_func, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, num_args, IS_LONG, 0, "-1")
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
