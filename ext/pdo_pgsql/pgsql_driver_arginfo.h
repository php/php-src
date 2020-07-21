/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 80c3e9c95db8c68ff3a2c2753aa5e9485af9fe4c */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO_PGSql_Ext_pgsqlCopyFromArray, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, table_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, rows, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, delimiter, IS_STRING, 0, "\"\\t\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, null_as, IS_STRING, 0, "\"\\\\\\\\N\"")
	ZEND_ARG_TYPE_INFO(0, fields, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO_PGSql_Ext_pgsqlCopyFromFile, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, table_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, delimiter, IS_STRING, 0, "\"\\t\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, null_as, IS_STRING, 0, "\"\\\\\\\\N\"")
	ZEND_ARG_TYPE_INFO(0, fields, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO_PGSql_Ext_pgsqlCopyToArray, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, table_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, delimiter, IS_STRING, 0, "\"\\t\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, null_as, IS_STRING, 0, "\"\\\\\\\\N\"")
	ZEND_ARG_TYPE_INFO(0, fields, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_PDO_PGSql_Ext_pgsqlCopyToFile arginfo_class_PDO_PGSql_Ext_pgsqlCopyFromFile

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO_PGSql_Ext_pgsqlLOBCreate, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO_PGSql_Ext_pgsqlLOBOpen, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, oid, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_STRING, 0, "\"rb\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO_PGSql_Ext_pgsqlLOBUnlink, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, oid, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PDO_PGSql_Ext_pgsqlGetNotify, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, result_type, IS_LONG, 0, "PDO::FETCH_USE_DEFAULT")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, ms_timeout, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_PDO_PGSql_Ext_pgsqlGetPid arginfo_class_PDO_PGSql_Ext_pgsqlLOBCreate


ZEND_METHOD(PDO_PGSql_Ext, pgsqlCopyFromArray);
ZEND_METHOD(PDO_PGSql_Ext, pgsqlCopyFromFile);
ZEND_METHOD(PDO_PGSql_Ext, pgsqlCopyToArray);
ZEND_METHOD(PDO_PGSql_Ext, pgsqlCopyToFile);
ZEND_METHOD(PDO_PGSql_Ext, pgsqlLOBCreate);
ZEND_METHOD(PDO_PGSql_Ext, pgsqlLOBOpen);
ZEND_METHOD(PDO_PGSql_Ext, pgsqlLOBUnlink);
ZEND_METHOD(PDO_PGSql_Ext, pgsqlGetNotify);
ZEND_METHOD(PDO_PGSql_Ext, pgsqlGetPid);


static const zend_function_entry class_PDO_PGSql_Ext_methods[] = {
	ZEND_ME(PDO_PGSql_Ext, pgsqlCopyFromArray, arginfo_class_PDO_PGSql_Ext_pgsqlCopyFromArray, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO_PGSql_Ext, pgsqlCopyFromFile, arginfo_class_PDO_PGSql_Ext_pgsqlCopyFromFile, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO_PGSql_Ext, pgsqlCopyToArray, arginfo_class_PDO_PGSql_Ext_pgsqlCopyToArray, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO_PGSql_Ext, pgsqlCopyToFile, arginfo_class_PDO_PGSql_Ext_pgsqlCopyToFile, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO_PGSql_Ext, pgsqlLOBCreate, arginfo_class_PDO_PGSql_Ext_pgsqlLOBCreate, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO_PGSql_Ext, pgsqlLOBOpen, arginfo_class_PDO_PGSql_Ext_pgsqlLOBOpen, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO_PGSql_Ext, pgsqlLOBUnlink, arginfo_class_PDO_PGSql_Ext_pgsqlLOBUnlink, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO_PGSql_Ext, pgsqlGetNotify, arginfo_class_PDO_PGSql_Ext_pgsqlGetNotify, ZEND_ACC_PUBLIC)
	ZEND_ME(PDO_PGSql_Ext, pgsqlGetPid, arginfo_class_PDO_PGSql_Ext_pgsqlGetPid, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
