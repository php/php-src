/* This is a generated file, edit opcache_user_cache.stub.php instead.
 * Stub hash: e737d9b829fb1679cd77f37d5a0524ebd6ab170d */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Opcache_UserCacheInfo___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Opcache_UserCache___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, scope, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Opcache_UserCache_store, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, value, MAY_BE_NULL|MAY_BE_BOOL|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, ttl, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Opcache_UserCache_storeMultiple, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, values, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, ttl, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Opcache_UserCache_increment, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, step, IS_LONG, 0, "1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, ttl, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_Opcache_UserCache_decrement arginfo_class_Opcache_UserCache_increment

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Opcache_UserCache_fetch, 0, 1, MAY_BE_NULL|MAY_BE_BOOL|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, default, MAY_BE_NULL|MAY_BE_BOOL|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Opcache_UserCache_fetchMultiple, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, default, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Opcache_UserCache_delete, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Opcache_UserCache_deleteMultiple, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Opcache_UserCache_clear, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Opcache_UserCache_has arginfo_class_Opcache_UserCache_delete

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Opcache_UserCache_lock, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, lease, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_Opcache_UserCache_unlock arginfo_class_Opcache_UserCache_delete

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Opcache_UserCache_remember, 0, 2, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, ttl, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Opcache_UserCache_info, 0, 0, Opcache\\\125serCacheInfo, 0)
ZEND_END_ARG_INFO()

ZEND_METHOD(Opcache_UserCacheInfo, __construct);
ZEND_METHOD(Opcache_UserCache, __construct);
ZEND_METHOD(Opcache_UserCache, store);
ZEND_METHOD(Opcache_UserCache, storeMultiple);
ZEND_METHOD(Opcache_UserCache, increment);
ZEND_METHOD(Opcache_UserCache, decrement);
ZEND_METHOD(Opcache_UserCache, fetch);
ZEND_METHOD(Opcache_UserCache, fetchMultiple);
ZEND_METHOD(Opcache_UserCache, delete);
ZEND_METHOD(Opcache_UserCache, deleteMultiple);
ZEND_METHOD(Opcache_UserCache, clear);
ZEND_METHOD(Opcache_UserCache, has);
ZEND_METHOD(Opcache_UserCache, lock);
ZEND_METHOD(Opcache_UserCache, unlock);
ZEND_METHOD(Opcache_UserCache, remember);
ZEND_METHOD(Opcache_UserCache, info);

static const zend_function_entry class_Opcache_UserCacheInfo_methods[] = {
	ZEND_ME(Opcache_UserCacheInfo, __construct, arginfo_class_Opcache_UserCacheInfo___construct, ZEND_ACC_PRIVATE)
	ZEND_FE_END
};

static const zend_function_entry class_Opcache_UserCache_methods[] = {
	ZEND_ME(Opcache_UserCache, __construct, arginfo_class_Opcache_UserCache___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Opcache_UserCache, store, arginfo_class_Opcache_UserCache_store, ZEND_ACC_PUBLIC)
	ZEND_ME(Opcache_UserCache, storeMultiple, arginfo_class_Opcache_UserCache_storeMultiple, ZEND_ACC_PUBLIC)
	ZEND_ME(Opcache_UserCache, increment, arginfo_class_Opcache_UserCache_increment, ZEND_ACC_PUBLIC)
	ZEND_ME(Opcache_UserCache, decrement, arginfo_class_Opcache_UserCache_decrement, ZEND_ACC_PUBLIC)
	ZEND_ME(Opcache_UserCache, fetch, arginfo_class_Opcache_UserCache_fetch, ZEND_ACC_PUBLIC)
	ZEND_ME(Opcache_UserCache, fetchMultiple, arginfo_class_Opcache_UserCache_fetchMultiple, ZEND_ACC_PUBLIC)
	ZEND_ME(Opcache_UserCache, delete, arginfo_class_Opcache_UserCache_delete, ZEND_ACC_PUBLIC)
	ZEND_ME(Opcache_UserCache, deleteMultiple, arginfo_class_Opcache_UserCache_deleteMultiple, ZEND_ACC_PUBLIC)
	ZEND_ME(Opcache_UserCache, clear, arginfo_class_Opcache_UserCache_clear, ZEND_ACC_PUBLIC)
	ZEND_ME(Opcache_UserCache, has, arginfo_class_Opcache_UserCache_has, ZEND_ACC_PUBLIC)
	ZEND_ME(Opcache_UserCache, lock, arginfo_class_Opcache_UserCache_lock, ZEND_ACC_PUBLIC)
	ZEND_ME(Opcache_UserCache, unlock, arginfo_class_Opcache_UserCache_unlock, ZEND_ACC_PUBLIC)
	ZEND_ME(Opcache_UserCache, remember, arginfo_class_Opcache_UserCache_remember, ZEND_ACC_PUBLIC)
	ZEND_ME(Opcache_UserCache, info, arginfo_class_Opcache_UserCache_info, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Opcache_UserCacheInfo(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Opcache", "UserCacheInfo", class_Opcache_UserCacheInfo_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_READONLY_CLASS);

	zval property_scope_default_value;
	ZVAL_UNDEF(&property_scope_default_value);
	zend_string *property_scope_name = zend_string_init("scope", sizeof("scope") - 1, true);
	zend_declare_typed_property(class_entry, property_scope_name, &property_scope_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release_ex(property_scope_name, true);

	zval property_available_default_value;
	ZVAL_UNDEF(&property_available_default_value);
	zend_string *property_available_name = zend_string_init("available", sizeof("available") - 1, true);
	zend_declare_typed_property(class_entry, property_available_name, &property_available_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release_ex(property_available_name, true);

	zval property_unavailableReason_default_value;
	ZVAL_UNDEF(&property_unavailableReason_default_value);
	zend_string *property_unavailableReason_name = zend_string_init("unavailableReason", sizeof("unavailableReason") - 1, true);
	zend_declare_typed_property(class_entry, property_unavailableReason_name, &property_unavailableReason_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release_ex(property_unavailableReason_name, true);

	zval property_enabled_default_value;
	ZVAL_UNDEF(&property_enabled_default_value);
	zend_string *property_enabled_name = zend_string_init("enabled", sizeof("enabled") - 1, true);
	zend_declare_typed_property(class_entry, property_enabled_name, &property_enabled_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release_ex(property_enabled_name, true);

	zval property_startupFailed_default_value;
	ZVAL_UNDEF(&property_startupFailed_default_value);
	zend_string *property_startupFailed_name = zend_string_init("startupFailed", sizeof("startupFailed") - 1, true);
	zend_declare_typed_property(class_entry, property_startupFailed_name, &property_startupFailed_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release_ex(property_startupFailed_name, true);

	zval property_backendInitialized_default_value;
	ZVAL_UNDEF(&property_backendInitialized_default_value);
	zend_string *property_backendInitialized_name = zend_string_init("backendInitialized", sizeof("backendInitialized") - 1, true);
	zend_declare_typed_property(class_entry, property_backendInitialized_name, &property_backendInitialized_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release_ex(property_backendInitialized_name, true);

	zval property_configuredMemory_default_value;
	ZVAL_UNDEF(&property_configuredMemory_default_value);
	zend_string *property_configuredMemory_name = zend_string_init("configuredMemory", sizeof("configuredMemory") - 1, true);
	zend_declare_typed_property(class_entry, property_configuredMemory_name, &property_configuredMemory_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(property_configuredMemory_name, true);

	zval property_sharedMemorySize_default_value;
	ZVAL_UNDEF(&property_sharedMemorySize_default_value);
	zend_string *property_sharedMemorySize_name = zend_string_init("sharedMemorySize", sizeof("sharedMemorySize") - 1, true);
	zend_declare_typed_property(class_entry, property_sharedMemorySize_name, &property_sharedMemorySize_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(property_sharedMemorySize_name, true);

	zval property_usedMemory_default_value;
	ZVAL_UNDEF(&property_usedMemory_default_value);
	zend_string *property_usedMemory_name = zend_string_init("usedMemory", sizeof("usedMemory") - 1, true);
	zend_declare_typed_property(class_entry, property_usedMemory_name, &property_usedMemory_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(property_usedMemory_name, true);

	zval property_freeMemory_default_value;
	ZVAL_UNDEF(&property_freeMemory_default_value);
	zend_string *property_freeMemory_name = zend_string_init("freeMemory", sizeof("freeMemory") - 1, true);
	zend_declare_typed_property(class_entry, property_freeMemory_name, &property_freeMemory_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(property_freeMemory_name, true);

	zval property_wastedMemory_default_value;
	ZVAL_UNDEF(&property_wastedMemory_default_value);
	zend_string *property_wastedMemory_name = zend_string_init("wastedMemory", sizeof("wastedMemory") - 1, true);
	zend_declare_typed_property(class_entry, property_wastedMemory_name, &property_wastedMemory_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(property_wastedMemory_name, true);

	zval property_entryCount_default_value;
	ZVAL_UNDEF(&property_entryCount_default_value);
	zend_string *property_entryCount_name = zend_string_init("entryCount", sizeof("entryCount") - 1, true);
	zend_declare_typed_property(class_entry, property_entryCount_name, &property_entryCount_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(property_entryCount_name, true);

	zval property_entryCapacity_default_value;
	ZVAL_UNDEF(&property_entryCapacity_default_value);
	zend_string *property_entryCapacity_name = zend_string_init("entryCapacity", sizeof("entryCapacity") - 1, true);
	zend_declare_typed_property(class_entry, property_entryCapacity_name, &property_entryCapacity_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(property_entryCapacity_name, true);

	zval property_tombstoneCount_default_value;
	ZVAL_UNDEF(&property_tombstoneCount_default_value);
	zend_string *property_tombstoneCount_name = zend_string_init("tombstoneCount", sizeof("tombstoneCount") - 1, true);
	zend_declare_typed_property(class_entry, property_tombstoneCount_name, &property_tombstoneCount_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(property_tombstoneCount_name, true);

	return class_entry;
}

static zend_class_entry *register_class_Opcache_UserCache(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Opcache", "UserCache", class_Opcache_UserCache_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NOT_SERIALIZABLE);

	return class_entry;
}
