/* This is a generated file, edit user_cache.stub.php instead.
 * Stub hash: b8be7cb1df9773c913f73583906d434b343175b5
 * Has decl header: yes */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_UserCache_CacheStatus___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_UserCache_CacheStatus_getAvailability, 0, 0, UserCache\\CacheAvailability, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_UserCache_CacheStatus_getConfiguredMemory, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_UserCache_CacheStatus_getSharedMemorySize arginfo_class_UserCache_CacheStatus_getConfiguredMemory

#define arginfo_class_UserCache_CacheStatus_getUsedMemory arginfo_class_UserCache_CacheStatus_getConfiguredMemory

#define arginfo_class_UserCache_CacheStatus_getFreeMemory arginfo_class_UserCache_CacheStatus_getConfiguredMemory

#define arginfo_class_UserCache_CacheStatus_getWastedMemory arginfo_class_UserCache_CacheStatus_getConfiguredMemory

#define arginfo_class_UserCache_CacheStatus_getEntryCount arginfo_class_UserCache_CacheStatus_getConfiguredMemory

#define arginfo_class_UserCache_CacheStatus_getEntryCapacity arginfo_class_UserCache_CacheStatus_getConfiguredMemory

#define arginfo_class_UserCache_CacheStatus_getTombstoneCount arginfo_class_UserCache_CacheStatus_getConfiguredMemory

#define arginfo_class_UserCache_CacheStatus_getExpungeCount arginfo_class_UserCache_CacheStatus_getConfiguredMemory

#define arginfo_class_UserCache_CacheStatus_getStoreFailureCount arginfo_class_UserCache_CacheStatus_getConfiguredMemory

#define arginfo_class_UserCache_CacheStatus_getGraphPinSlotsInUse arginfo_class_UserCache_CacheStatus_getConfiguredMemory

#define arginfo_class_UserCache_CacheStatus_getGraphPinnedReferences arginfo_class_UserCache_CacheStatus_getConfiguredMemory

#define arginfo_class_UserCache_CacheStatus_getDeadPinOwnersReclaimed arginfo_class_UserCache_CacheStatus_getConfiguredMemory

#define arginfo_class_UserCache_CacheStatus_getDeadPinsStripped arginfo_class_UserCache_CacheStatus_getConfiguredMemory

#define arginfo_class_UserCache_CachePoolStatus___construct arginfo_class_UserCache_CacheStatus___construct

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_UserCache_CachePoolStatus_getPoolName, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_UserCache_CachePoolStatus_getEntryCount arginfo_class_UserCache_CacheStatus_getConfiguredMemory

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_UserCache_CachePoolStatus_getEntryKeys, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_UserCache_CachePoolStatus_getUsedMemory arginfo_class_UserCache_CacheStatus_getConfiguredMemory

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_UserCache_Cache_hasPool, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pool, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_UserCache_Cache_getPool, 0, 1, UserCache\\Cache, 0)
	ZEND_ARG_TYPE_INFO(0, pool, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_UserCache_Cache_deletePool arginfo_class_UserCache_Cache_hasPool

#define arginfo_class_UserCache_Cache_getPools arginfo_class_UserCache_CachePoolStatus_getEntryKeys

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_UserCache_Cache_getStatus, 0, 0, UserCache\\CacheStatus, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_UserCache_Cache___construct arginfo_class_UserCache_CacheStatus___construct

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_UserCache_Cache_store, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, value, MAY_BE_NULL|MAY_BE_BOOL|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, ttl, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_UserCache_Cache_add arginfo_class_UserCache_Cache_store

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_UserCache_Cache_storeMultiple, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, values, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, ttl, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_UserCache_Cache_increment, 0, 1, IS_LONG, 1)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, step, IS_LONG, 0, "1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, ttl, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_UserCache_Cache_decrement arginfo_class_UserCache_Cache_increment

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_UserCache_Cache_fetch, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, default, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_UserCache_Cache_fetchMultiple, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, default, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_UserCache_Cache_delete, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_UserCache_Cache_deleteMultiple, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_UserCache_Cache_clear, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_UserCache_Cache_has arginfo_class_UserCache_Cache_delete

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_UserCache_Cache_lock, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, lease, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_UserCache_Cache_unlock arginfo_class_UserCache_Cache_delete

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_UserCache_Cache_remember, 0, 2, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, ttl, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_UserCache_Cache_getPoolStatus, 0, 0, UserCache\\CachePoolStatus, 0)
ZEND_END_ARG_INFO()

ZEND_METHOD(UserCache_CacheStatus, __construct);
ZEND_METHOD(UserCache_CacheStatus, getAvailability);
ZEND_METHOD(UserCache_CacheStatus, getConfiguredMemory);
ZEND_METHOD(UserCache_CacheStatus, getSharedMemorySize);
ZEND_METHOD(UserCache_CacheStatus, getUsedMemory);
ZEND_METHOD(UserCache_CacheStatus, getFreeMemory);
ZEND_METHOD(UserCache_CacheStatus, getWastedMemory);
ZEND_METHOD(UserCache_CacheStatus, getEntryCount);
ZEND_METHOD(UserCache_CacheStatus, getEntryCapacity);
ZEND_METHOD(UserCache_CacheStatus, getTombstoneCount);
ZEND_METHOD(UserCache_CacheStatus, getExpungeCount);
ZEND_METHOD(UserCache_CacheStatus, getStoreFailureCount);
ZEND_METHOD(UserCache_CacheStatus, getGraphPinSlotsInUse);
ZEND_METHOD(UserCache_CacheStatus, getGraphPinnedReferences);
ZEND_METHOD(UserCache_CacheStatus, getDeadPinOwnersReclaimed);
ZEND_METHOD(UserCache_CacheStatus, getDeadPinsStripped);
ZEND_METHOD(UserCache_CachePoolStatus, __construct);
ZEND_METHOD(UserCache_CachePoolStatus, getPoolName);
ZEND_METHOD(UserCache_CachePoolStatus, getEntryCount);
ZEND_METHOD(UserCache_CachePoolStatus, getEntryKeys);
ZEND_METHOD(UserCache_CachePoolStatus, getUsedMemory);
ZEND_METHOD(UserCache_Cache, hasPool);
ZEND_METHOD(UserCache_Cache, getPool);
ZEND_METHOD(UserCache_Cache, deletePool);
ZEND_METHOD(UserCache_Cache, getPools);
ZEND_METHOD(UserCache_Cache, getStatus);
ZEND_METHOD(UserCache_Cache, __construct);
ZEND_METHOD(UserCache_Cache, store);
ZEND_METHOD(UserCache_Cache, add);
ZEND_METHOD(UserCache_Cache, storeMultiple);
ZEND_METHOD(UserCache_Cache, increment);
ZEND_METHOD(UserCache_Cache, decrement);
ZEND_METHOD(UserCache_Cache, fetch);
ZEND_METHOD(UserCache_Cache, fetchMultiple);
ZEND_METHOD(UserCache_Cache, delete);
ZEND_METHOD(UserCache_Cache, deleteMultiple);
ZEND_METHOD(UserCache_Cache, clear);
ZEND_METHOD(UserCache_Cache, has);
ZEND_METHOD(UserCache_Cache, lock);
ZEND_METHOD(UserCache_Cache, unlock);
ZEND_METHOD(UserCache_Cache, remember);
ZEND_METHOD(UserCache_Cache, getPoolStatus);

static const zend_function_entry class_UserCache_CacheStatus_methods[] = {
	ZEND_ME(UserCache_CacheStatus, __construct, arginfo_class_UserCache_CacheStatus___construct, ZEND_ACC_PRIVATE)
	ZEND_ME(UserCache_CacheStatus, getAvailability, arginfo_class_UserCache_CacheStatus_getAvailability, ZEND_ACC_PUBLIC)
	ZEND_ME(UserCache_CacheStatus, getConfiguredMemory, arginfo_class_UserCache_CacheStatus_getConfiguredMemory, ZEND_ACC_PUBLIC)
	ZEND_ME(UserCache_CacheStatus, getSharedMemorySize, arginfo_class_UserCache_CacheStatus_getSharedMemorySize, ZEND_ACC_PUBLIC)
	ZEND_ME(UserCache_CacheStatus, getUsedMemory, arginfo_class_UserCache_CacheStatus_getUsedMemory, ZEND_ACC_PUBLIC)
	ZEND_ME(UserCache_CacheStatus, getFreeMemory, arginfo_class_UserCache_CacheStatus_getFreeMemory, ZEND_ACC_PUBLIC)
	ZEND_ME(UserCache_CacheStatus, getWastedMemory, arginfo_class_UserCache_CacheStatus_getWastedMemory, ZEND_ACC_PUBLIC)
	ZEND_ME(UserCache_CacheStatus, getEntryCount, arginfo_class_UserCache_CacheStatus_getEntryCount, ZEND_ACC_PUBLIC)
	ZEND_ME(UserCache_CacheStatus, getEntryCapacity, arginfo_class_UserCache_CacheStatus_getEntryCapacity, ZEND_ACC_PUBLIC)
	ZEND_ME(UserCache_CacheStatus, getTombstoneCount, arginfo_class_UserCache_CacheStatus_getTombstoneCount, ZEND_ACC_PUBLIC)
	ZEND_ME(UserCache_CacheStatus, getExpungeCount, arginfo_class_UserCache_CacheStatus_getExpungeCount, ZEND_ACC_PUBLIC)
	ZEND_ME(UserCache_CacheStatus, getStoreFailureCount, arginfo_class_UserCache_CacheStatus_getStoreFailureCount, ZEND_ACC_PUBLIC)
	ZEND_ME(UserCache_CacheStatus, getGraphPinSlotsInUse, arginfo_class_UserCache_CacheStatus_getGraphPinSlotsInUse, ZEND_ACC_PUBLIC)
	ZEND_ME(UserCache_CacheStatus, getGraphPinnedReferences, arginfo_class_UserCache_CacheStatus_getGraphPinnedReferences, ZEND_ACC_PUBLIC)
	ZEND_ME(UserCache_CacheStatus, getDeadPinOwnersReclaimed, arginfo_class_UserCache_CacheStatus_getDeadPinOwnersReclaimed, ZEND_ACC_PUBLIC)
	ZEND_ME(UserCache_CacheStatus, getDeadPinsStripped, arginfo_class_UserCache_CacheStatus_getDeadPinsStripped, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_UserCache_CachePoolStatus_methods[] = {
	ZEND_ME(UserCache_CachePoolStatus, __construct, arginfo_class_UserCache_CachePoolStatus___construct, ZEND_ACC_PRIVATE)
	ZEND_ME(UserCache_CachePoolStatus, getPoolName, arginfo_class_UserCache_CachePoolStatus_getPoolName, ZEND_ACC_PUBLIC)
	ZEND_ME(UserCache_CachePoolStatus, getEntryCount, arginfo_class_UserCache_CachePoolStatus_getEntryCount, ZEND_ACC_PUBLIC)
	ZEND_ME(UserCache_CachePoolStatus, getEntryKeys, arginfo_class_UserCache_CachePoolStatus_getEntryKeys, ZEND_ACC_PUBLIC)
	ZEND_ME(UserCache_CachePoolStatus, getUsedMemory, arginfo_class_UserCache_CachePoolStatus_getUsedMemory, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_UserCache_Cache_methods[] = {
	ZEND_ME(UserCache_Cache, hasPool, arginfo_class_UserCache_Cache_hasPool, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(UserCache_Cache, getPool, arginfo_class_UserCache_Cache_getPool, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(UserCache_Cache, deletePool, arginfo_class_UserCache_Cache_deletePool, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(UserCache_Cache, getPools, arginfo_class_UserCache_Cache_getPools, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(UserCache_Cache, getStatus, arginfo_class_UserCache_Cache_getStatus, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(UserCache_Cache, __construct, arginfo_class_UserCache_Cache___construct, ZEND_ACC_PRIVATE)
	ZEND_ME(UserCache_Cache, store, arginfo_class_UserCache_Cache_store, ZEND_ACC_PUBLIC)
	ZEND_ME(UserCache_Cache, add, arginfo_class_UserCache_Cache_add, ZEND_ACC_PUBLIC)
	ZEND_ME(UserCache_Cache, storeMultiple, arginfo_class_UserCache_Cache_storeMultiple, ZEND_ACC_PUBLIC)
	ZEND_ME(UserCache_Cache, increment, arginfo_class_UserCache_Cache_increment, ZEND_ACC_PUBLIC)
	ZEND_ME(UserCache_Cache, decrement, arginfo_class_UserCache_Cache_decrement, ZEND_ACC_PUBLIC)
	ZEND_ME(UserCache_Cache, fetch, arginfo_class_UserCache_Cache_fetch, ZEND_ACC_PUBLIC)
	ZEND_ME(UserCache_Cache, fetchMultiple, arginfo_class_UserCache_Cache_fetchMultiple, ZEND_ACC_PUBLIC)
	ZEND_ME(UserCache_Cache, delete, arginfo_class_UserCache_Cache_delete, ZEND_ACC_PUBLIC)
	ZEND_ME(UserCache_Cache, deleteMultiple, arginfo_class_UserCache_Cache_deleteMultiple, ZEND_ACC_PUBLIC)
	ZEND_ME(UserCache_Cache, clear, arginfo_class_UserCache_Cache_clear, ZEND_ACC_PUBLIC)
	ZEND_ME(UserCache_Cache, has, arginfo_class_UserCache_Cache_has, ZEND_ACC_PUBLIC)
	ZEND_ME(UserCache_Cache, lock, arginfo_class_UserCache_Cache_lock, ZEND_ACC_PUBLIC)
	ZEND_ME(UserCache_Cache, unlock, arginfo_class_UserCache_Cache_unlock, ZEND_ACC_PUBLIC)
	ZEND_ME(UserCache_Cache, remember, arginfo_class_UserCache_Cache_remember, ZEND_ACC_PUBLIC)
	ZEND_ME(UserCache_Cache, getPoolStatus, arginfo_class_UserCache_Cache_getPoolStatus, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_UserCache_CacheAvailability(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("UserCache\\CacheAvailability", IS_UNDEF, NULL);

	zend_enum_add_case_cstr(class_entry, "Available", NULL);

	zend_enum_add_case_cstr(class_entry, "DisabledByIni", NULL);

	zend_enum_add_case_cstr(class_entry, "DisabledBySapi", NULL);

	zend_enum_add_case_cstr(class_entry, "UnavailableBySharedMemoryInitializationFailed", NULL);

	zend_enum_add_case_cstr(class_entry, "UnavailableByBackendNotInitializedBeforeWorkerStartup", NULL);

	zend_enum_add_case_cstr(class_entry, "UnavailableByBackendInitializedAfterWorkerStartup", NULL);

	zend_enum_add_case_cstr(class_entry, "UnavailableByCgiFastCgiBoundary", NULL);

	zend_enum_add_case_cstr(class_entry, "UnavailableByLsapiBoundary", NULL);

	zend_enum_add_case_cstr(class_entry, "UnavailableByUnknownReason", NULL);

	return class_entry;
}

static zend_class_entry *register_class_UserCache_CacheStatus(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "UserCache", "CacheStatus", class_UserCache_CacheStatus_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE|ZEND_ACC_READONLY_CLASS);

	return class_entry;
}

static zend_class_entry *register_class_UserCache_CachePoolStatus(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "UserCache", "CachePoolStatus", class_UserCache_CachePoolStatus_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE|ZEND_ACC_READONLY_CLASS);

	return class_entry;
}

static zend_class_entry *register_class_UserCache_Cache(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "UserCache", "Cache", class_UserCache_Cache_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NOT_SERIALIZABLE);

	return class_entry;
}
