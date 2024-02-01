/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 2be4e9679d29fcbc3330b78f9e63c4c6e1284f19 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_PdoPgsql_escapeIdentifier, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, input, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_PdoPgsql_copyFromArray, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, tableName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, rows, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, separator, IS_STRING, 0, "\"\\t\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, nullAs, IS_STRING, 0, "\"\\\\\\\\N\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, fields, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_PdoPgsql_copyFromFile, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, tableName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, separator, IS_STRING, 0, "\"\\t\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, nullAs, IS_STRING, 0, "\"\\\\\\\\N\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, fields, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_PdoPgsql_copyToArray, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, tableName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, separator, IS_STRING, 0, "\"\\t\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, nullAs, IS_STRING, 0, "\"\\\\\\\\N\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, fields, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_PdoPgsql_copyToFile arginfo_class_PdoPgsql_copyFromFile

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_PdoPgsql_lobCreate, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_PdoPgsql_lobOpen, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, oid, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_STRING, 0, "\"rb\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_PdoPgsql_lobUnlink, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, oid, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_PdoPgsql_getNotify, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, fetchMode, IS_LONG, 0, "PDO::FETCH_DEFAULT")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeoutMilliseconds, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_PdoPgsql_getPid, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(PdoPgsql, escapeIdentifier);
ZEND_METHOD(PdoPgsql, copyFromArray);
ZEND_METHOD(PdoPgsql, copyFromFile);
ZEND_METHOD(PdoPgsql, copyToArray);
ZEND_METHOD(PdoPgsql, copyToFile);
ZEND_METHOD(PdoPgsql, lobCreate);
ZEND_METHOD(PdoPgsql, lobOpen);
ZEND_METHOD(PdoPgsql, lobUnlink);
ZEND_METHOD(PdoPgsql, getNotify);
ZEND_METHOD(PdoPgsql, getPid);


static const zend_function_entry class_PdoPgsql_methods[] = {
	ZEND_ME(PdoPgsql, escapeIdentifier, arginfo_class_PdoPgsql_escapeIdentifier, ZEND_ACC_PUBLIC)
	ZEND_ME(PdoPgsql, copyFromArray, arginfo_class_PdoPgsql_copyFromArray, ZEND_ACC_PUBLIC)
	ZEND_ME(PdoPgsql, copyFromFile, arginfo_class_PdoPgsql_copyFromFile, ZEND_ACC_PUBLIC)
	ZEND_ME(PdoPgsql, copyToArray, arginfo_class_PdoPgsql_copyToArray, ZEND_ACC_PUBLIC)
	ZEND_ME(PdoPgsql, copyToFile, arginfo_class_PdoPgsql_copyToFile, ZEND_ACC_PUBLIC)
	ZEND_ME(PdoPgsql, lobCreate, arginfo_class_PdoPgsql_lobCreate, ZEND_ACC_PUBLIC)
	ZEND_ME(PdoPgsql, lobOpen, arginfo_class_PdoPgsql_lobOpen, ZEND_ACC_PUBLIC)
	ZEND_ME(PdoPgsql, lobUnlink, arginfo_class_PdoPgsql_lobUnlink, ZEND_ACC_PUBLIC)
	ZEND_ME(PdoPgsql, getNotify, arginfo_class_PdoPgsql_getNotify, ZEND_ACC_PUBLIC)
	ZEND_ME(PdoPgsql, getPid, arginfo_class_PdoPgsql_getPid, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_PdoPgsql(zend_class_entry *class_entry_PDO)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "PdoPgsql", class_PdoPgsql_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_PDO);
	class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;

	zval const_ATTR_DISABLE_PREPARES_value;
	ZVAL_LONG(&const_ATTR_DISABLE_PREPARES_value, PDO_PGSQL_ATTR_DISABLE_PREPARES);
	zend_string *const_ATTR_DISABLE_PREPARES_name = zend_string_init_interned("ATTR_DISABLE_PREPARES", sizeof("ATTR_DISABLE_PREPARES") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_ATTR_DISABLE_PREPARES_name, &const_ATTR_DISABLE_PREPARES_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_ATTR_DISABLE_PREPARES_name);

	zval const_TRANSACTION_IDLE_value;
	ZVAL_LONG(&const_TRANSACTION_IDLE_value, PGSQL_TRANSACTION_IDLE);
	zend_string *const_TRANSACTION_IDLE_name = zend_string_init_interned("TRANSACTION_IDLE", sizeof("TRANSACTION_IDLE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TRANSACTION_IDLE_name, &const_TRANSACTION_IDLE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TRANSACTION_IDLE_name);

	zval const_TRANSACTION_ACTIVE_value;
	ZVAL_LONG(&const_TRANSACTION_ACTIVE_value, PGSQL_TRANSACTION_ACTIVE);
	zend_string *const_TRANSACTION_ACTIVE_name = zend_string_init_interned("TRANSACTION_ACTIVE", sizeof("TRANSACTION_ACTIVE") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TRANSACTION_ACTIVE_name, &const_TRANSACTION_ACTIVE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TRANSACTION_ACTIVE_name);

	zval const_TRANSACTION_INTRANS_value;
	ZVAL_LONG(&const_TRANSACTION_INTRANS_value, PGSQL_TRANSACTION_INTRANS);
	zend_string *const_TRANSACTION_INTRANS_name = zend_string_init_interned("TRANSACTION_INTRANS", sizeof("TRANSACTION_INTRANS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TRANSACTION_INTRANS_name, &const_TRANSACTION_INTRANS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TRANSACTION_INTRANS_name);

	zval const_TRANSACTION_INERROR_value;
	ZVAL_LONG(&const_TRANSACTION_INERROR_value, PGSQL_TRANSACTION_INERROR);
	zend_string *const_TRANSACTION_INERROR_name = zend_string_init_interned("TRANSACTION_INERROR", sizeof("TRANSACTION_INERROR") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TRANSACTION_INERROR_name, &const_TRANSACTION_INERROR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TRANSACTION_INERROR_name);

	zval const_TRANSACTION_UNKNOWN_value;
	ZVAL_LONG(&const_TRANSACTION_UNKNOWN_value, PGSQL_TRANSACTION_UNKNOWN);
	zend_string *const_TRANSACTION_UNKNOWN_name = zend_string_init_interned("TRANSACTION_UNKNOWN", sizeof("TRANSACTION_UNKNOWN") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TRANSACTION_UNKNOWN_name, &const_TRANSACTION_UNKNOWN_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TRANSACTION_UNKNOWN_name);

	return class_entry;
}
