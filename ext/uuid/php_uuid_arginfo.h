/* This is a generated file, edit php_uuid.stub.php instead.
 * Stub hash: 441fb7588f8f1f43517242156d1d18453f08bea6 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uuid_UuidV7_parse, 0, 1, IS_STATIC, 1)
	ZEND_ARG_TYPE_INFO(0, uuid, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uuid_UuidV7_generate, 0, 0, IS_STATIC, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, datetime, DateTimeImmutable, 1, "null")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, randomEngine, Random\\Engine, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Uuid_UuidV7___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, uuid, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uuid_UuidV7_toBytes, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Uuid_UuidV7_toString arginfo_class_Uuid_UuidV7_toBytes

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uuid_UuidV7_equals, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, uuid, Uuid\\\125uidV7, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uuid_UuidV7___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Uuid_UuidV7___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Uuid_UuidV7___debugInfo arginfo_class_Uuid_UuidV7___serialize

ZEND_METHOD(Uuid_UuidV7, parse);
ZEND_METHOD(Uuid_UuidV7, generate);
ZEND_METHOD(Uuid_UuidV7, __construct);
ZEND_METHOD(Uuid_UuidV7, toBytes);
ZEND_METHOD(Uuid_UuidV7, toString);
ZEND_METHOD(Uuid_UuidV7, equals);
ZEND_METHOD(Uuid_UuidV7, __serialize);
ZEND_METHOD(Uuid_UuidV7, __unserialize);
ZEND_METHOD(Uuid_UuidV7, __debugInfo);

static const zend_function_entry class_Uuid_UuidV7_methods[] = {
	ZEND_ME(Uuid_UuidV7, parse, arginfo_class_Uuid_UuidV7_parse, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Uuid_UuidV7, generate, arginfo_class_Uuid_UuidV7_generate, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Uuid_UuidV7, __construct, arginfo_class_Uuid_UuidV7___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Uuid_UuidV7, toBytes, arginfo_class_Uuid_UuidV7_toBytes, ZEND_ACC_PUBLIC)
	ZEND_ME(Uuid_UuidV7, toString, arginfo_class_Uuid_UuidV7_toString, ZEND_ACC_PUBLIC)
	ZEND_ME(Uuid_UuidV7, equals, arginfo_class_Uuid_UuidV7_equals, ZEND_ACC_PUBLIC)
	ZEND_ME(Uuid_UuidV7, __serialize, arginfo_class_Uuid_UuidV7___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Uuid_UuidV7, __unserialize, arginfo_class_Uuid_UuidV7___unserialize, ZEND_ACC_PUBLIC)
	ZEND_ME(Uuid_UuidV7, __debugInfo, arginfo_class_Uuid_UuidV7___debugInfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Uuid_UuidV7(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Uuid", "UuidV7", class_Uuid_UuidV7_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_READONLY_CLASS);

	return class_entry;
}
