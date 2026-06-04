/* This is a generated file, edit opcache.stub.php instead.
 * Stub hash: 8a3b970e24f6d70270db9edace77cea499233773 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_opcache_reset, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_opcache_static_cache_volatile_reset arginfo_opcache_reset

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_opcache_get_status, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, include_scripts, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_opcache_compile_file, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_opcache_invalidate, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, force, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_opcache_jit_blacklist, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, closure, Closure, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_opcache_get_configuration, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

#define arginfo_opcache_is_script_cached arginfo_opcache_compile_file

#define arginfo_opcache_is_script_cached_in_file_cache arginfo_opcache_compile_file

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OPcache_StaticCacheInfo___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OPcache_VolatileStatic___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, ttl, IS_LONG, 0, "0")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, strategy, OPcache\\CacheStrategy, 0, "OPcache\\CacheStrategy::Immediate")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OPcache_StaticCacheInterface_getInstance, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, pool_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_OPcache_StaticCacheInterface_fetch, 0, 1, MAY_BE_NULL|MAY_BE_BOOL|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, default, MAY_BE_NULL|MAY_BE_BOOL|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_OPcache_StaticCacheInterface_fetchMultiple, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, default, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OPcache_StaticCacheInterface_store, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, value, MAY_BE_NULL|MAY_BE_BOOL|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OPcache_StaticCacheInterface_storeMultiple, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, values, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OPcache_StaticCacheInterface_has, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OPcache_StaticCacheInterface_delete arginfo_class_OPcache_StaticCacheInterface_has

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OPcache_StaticCacheInterface_deleteMultiple, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OPcache_StaticCacheInterface_clear arginfo_opcache_reset

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OPcache_StaticCacheInterface_lock, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, lease, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_OPcache_StaticCacheInterface_unlock arginfo_class_OPcache_StaticCacheInterface_has

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_OPcache_StaticCacheInterface_info, 0, 0, OPcache\\StaticCacheInfo, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OPcache_VolatileCache___construct arginfo_class_OPcache_StaticCacheInfo___construct

#define arginfo_class_OPcache_VolatileCache_getInstance arginfo_class_OPcache_StaticCacheInterface_getInstance

#define arginfo_class_OPcache_VolatileCache_fetch arginfo_class_OPcache_StaticCacheInterface_fetch

#define arginfo_class_OPcache_VolatileCache_fetchMultiple arginfo_class_OPcache_StaticCacheInterface_fetchMultiple

#define arginfo_class_OPcache_VolatileCache_store arginfo_class_OPcache_StaticCacheInterface_store

#define arginfo_class_OPcache_VolatileCache_storeMultiple arginfo_class_OPcache_StaticCacheInterface_storeMultiple

#define arginfo_class_OPcache_VolatileCache_has arginfo_class_OPcache_StaticCacheInterface_has

#define arginfo_class_OPcache_VolatileCache_delete arginfo_class_OPcache_StaticCacheInterface_has

#define arginfo_class_OPcache_VolatileCache_deleteMultiple arginfo_class_OPcache_StaticCacheInterface_deleteMultiple

#define arginfo_class_OPcache_VolatileCache_clear arginfo_opcache_reset

#define arginfo_class_OPcache_VolatileCache_lock arginfo_class_OPcache_StaticCacheInterface_lock

#define arginfo_class_OPcache_VolatileCache_unlock arginfo_class_OPcache_StaticCacheInterface_has

#define arginfo_class_OPcache_VolatileCache_info arginfo_class_OPcache_StaticCacheInterface_info

#define arginfo_class_OPcache_StableCache___construct arginfo_class_OPcache_StaticCacheInfo___construct

#define arginfo_class_OPcache_StableCache_getInstance arginfo_class_OPcache_StaticCacheInterface_getInstance

#define arginfo_class_OPcache_StableCache_fetch arginfo_class_OPcache_StaticCacheInterface_fetch

#define arginfo_class_OPcache_StableCache_fetchMultiple arginfo_class_OPcache_StaticCacheInterface_fetchMultiple

#define arginfo_class_OPcache_StableCache_store arginfo_class_OPcache_StaticCacheInterface_store

#define arginfo_class_OPcache_StableCache_storeMultiple arginfo_class_OPcache_StaticCacheInterface_storeMultiple

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OPcache_StableCache_storeWithTtl, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, value, MAY_BE_NULL|MAY_BE_BOOL|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT, NULL)
	ZEND_ARG_TYPE_INFO(0, ttl, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_OPcache_StableCache_storeMultipleWithTtl, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, values, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, ttl, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_OPcache_StableCache_has arginfo_class_OPcache_StaticCacheInterface_has

#define arginfo_class_OPcache_StableCache_delete arginfo_class_OPcache_StaticCacheInterface_has

#define arginfo_class_OPcache_StableCache_deleteMultiple arginfo_class_OPcache_StaticCacheInterface_deleteMultiple

#define arginfo_class_OPcache_StableCache_clear arginfo_opcache_reset

#define arginfo_class_OPcache_StableCache_lock arginfo_class_OPcache_StaticCacheInterface_lock

#define arginfo_class_OPcache_StableCache_unlock arginfo_class_OPcache_StaticCacheInterface_has

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_OPcache_StableCache_increment, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, step, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

#define arginfo_class_OPcache_StableCache_decrement arginfo_class_OPcache_StableCache_increment

#define arginfo_class_OPcache_StableCache_info arginfo_class_OPcache_StaticCacheInterface_info

ZEND_FUNCTION(opcache_reset);
ZEND_FUNCTION(opcache_static_cache_volatile_reset);
ZEND_FUNCTION(opcache_get_status);
ZEND_FUNCTION(opcache_compile_file);
ZEND_FUNCTION(opcache_invalidate);
ZEND_FUNCTION(opcache_jit_blacklist);
ZEND_FUNCTION(opcache_get_configuration);
ZEND_FUNCTION(opcache_is_script_cached);
ZEND_FUNCTION(opcache_is_script_cached_in_file_cache);
ZEND_METHOD(OPcache_StaticCacheInfo, __construct);
ZEND_METHOD(OPcache_VolatileStatic, __construct);
ZEND_METHOD(OPcache_VolatileCache, __construct);
ZEND_METHOD(OPcache_VolatileCache, getInstance);
ZEND_METHOD(OPcache_VolatileCache, fetch);
ZEND_METHOD(OPcache_VolatileCache, fetchMultiple);
ZEND_METHOD(OPcache_VolatileCache, store);
ZEND_METHOD(OPcache_VolatileCache, storeMultiple);
ZEND_METHOD(OPcache_VolatileCache, has);
ZEND_METHOD(OPcache_VolatileCache, delete);
ZEND_METHOD(OPcache_VolatileCache, deleteMultiple);
ZEND_METHOD(OPcache_VolatileCache, clear);
ZEND_METHOD(OPcache_VolatileCache, lock);
ZEND_METHOD(OPcache_VolatileCache, unlock);
ZEND_METHOD(OPcache_VolatileCache, info);
ZEND_METHOD(OPcache_StableCache, __construct);
ZEND_METHOD(OPcache_StableCache, getInstance);
ZEND_METHOD(OPcache_StableCache, fetch);
ZEND_METHOD(OPcache_StableCache, fetchMultiple);
ZEND_METHOD(OPcache_StableCache, store);
ZEND_METHOD(OPcache_StableCache, storeMultiple);
ZEND_METHOD(OPcache_StableCache, storeWithTtl);
ZEND_METHOD(OPcache_StableCache, storeMultipleWithTtl);
ZEND_METHOD(OPcache_StableCache, has);
ZEND_METHOD(OPcache_StableCache, delete);
ZEND_METHOD(OPcache_StableCache, deleteMultiple);
ZEND_METHOD(OPcache_StableCache, clear);
ZEND_METHOD(OPcache_StableCache, lock);
ZEND_METHOD(OPcache_StableCache, unlock);
ZEND_METHOD(OPcache_StableCache, increment);
ZEND_METHOD(OPcache_StableCache, decrement);
ZEND_METHOD(OPcache_StableCache, info);

static const zend_function_entry ext_functions[] = {
	ZEND_FE(opcache_reset, arginfo_opcache_reset)
	ZEND_FE(opcache_static_cache_volatile_reset, arginfo_opcache_static_cache_volatile_reset)
	ZEND_FE(opcache_get_status, arginfo_opcache_get_status)
	ZEND_FE(opcache_compile_file, arginfo_opcache_compile_file)
	ZEND_FE(opcache_invalidate, arginfo_opcache_invalidate)
	ZEND_FE(opcache_jit_blacklist, arginfo_opcache_jit_blacklist)
	ZEND_FE(opcache_get_configuration, arginfo_opcache_get_configuration)
	ZEND_FE(opcache_is_script_cached, arginfo_opcache_is_script_cached)
	ZEND_FE(opcache_is_script_cached_in_file_cache, arginfo_opcache_is_script_cached_in_file_cache)
	ZEND_FE_END
};

static const zend_function_entry class_OPcache_StaticCacheInfo_methods[] = {
	ZEND_ME(OPcache_StaticCacheInfo, __construct, arginfo_class_OPcache_StaticCacheInfo___construct, ZEND_ACC_PRIVATE)
	ZEND_FE_END
};

static const zend_function_entry class_OPcache_VolatileStatic_methods[] = {
	ZEND_ME(OPcache_VolatileStatic, __construct, arginfo_class_OPcache_VolatileStatic___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_OPcache_StaticCacheInterface_methods[] = {
	ZEND_RAW_FENTRY("getInstance", NULL, arginfo_class_OPcache_StaticCacheInterface_getInstance, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("fetch", NULL, arginfo_class_OPcache_StaticCacheInterface_fetch, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("fetchMultiple", NULL, arginfo_class_OPcache_StaticCacheInterface_fetchMultiple, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("store", NULL, arginfo_class_OPcache_StaticCacheInterface_store, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("storeMultiple", NULL, arginfo_class_OPcache_StaticCacheInterface_storeMultiple, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("has", NULL, arginfo_class_OPcache_StaticCacheInterface_has, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("delete", NULL, arginfo_class_OPcache_StaticCacheInterface_delete, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("deleteMultiple", NULL, arginfo_class_OPcache_StaticCacheInterface_deleteMultiple, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("clear", NULL, arginfo_class_OPcache_StaticCacheInterface_clear, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("lock", NULL, arginfo_class_OPcache_StaticCacheInterface_lock, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("unlock", NULL, arginfo_class_OPcache_StaticCacheInterface_unlock, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("info", NULL, arginfo_class_OPcache_StaticCacheInterface_info, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_OPcache_VolatileCache_methods[] = {
	ZEND_ME(OPcache_VolatileCache, __construct, arginfo_class_OPcache_VolatileCache___construct, ZEND_ACC_PRIVATE)
	ZEND_ME(OPcache_VolatileCache, getInstance, arginfo_class_OPcache_VolatileCache_getInstance, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(OPcache_VolatileCache, fetch, arginfo_class_OPcache_VolatileCache_fetch, ZEND_ACC_PUBLIC)
	ZEND_ME(OPcache_VolatileCache, fetchMultiple, arginfo_class_OPcache_VolatileCache_fetchMultiple, ZEND_ACC_PUBLIC)
	ZEND_ME(OPcache_VolatileCache, store, arginfo_class_OPcache_VolatileCache_store, ZEND_ACC_PUBLIC)
	ZEND_ME(OPcache_VolatileCache, storeMultiple, arginfo_class_OPcache_VolatileCache_storeMultiple, ZEND_ACC_PUBLIC)
	ZEND_ME(OPcache_VolatileCache, has, arginfo_class_OPcache_VolatileCache_has, ZEND_ACC_PUBLIC)
	ZEND_ME(OPcache_VolatileCache, delete, arginfo_class_OPcache_VolatileCache_delete, ZEND_ACC_PUBLIC)
	ZEND_ME(OPcache_VolatileCache, deleteMultiple, arginfo_class_OPcache_VolatileCache_deleteMultiple, ZEND_ACC_PUBLIC)
	ZEND_ME(OPcache_VolatileCache, clear, arginfo_class_OPcache_VolatileCache_clear, ZEND_ACC_PUBLIC)
	ZEND_ME(OPcache_VolatileCache, lock, arginfo_class_OPcache_VolatileCache_lock, ZEND_ACC_PUBLIC)
	ZEND_ME(OPcache_VolatileCache, unlock, arginfo_class_OPcache_VolatileCache_unlock, ZEND_ACC_PUBLIC)
	ZEND_ME(OPcache_VolatileCache, info, arginfo_class_OPcache_VolatileCache_info, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};

static const zend_function_entry class_OPcache_StableCache_methods[] = {
	ZEND_ME(OPcache_StableCache, __construct, arginfo_class_OPcache_StableCache___construct, ZEND_ACC_PRIVATE)
	ZEND_ME(OPcache_StableCache, getInstance, arginfo_class_OPcache_StableCache_getInstance, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(OPcache_StableCache, fetch, arginfo_class_OPcache_StableCache_fetch, ZEND_ACC_PUBLIC)
	ZEND_ME(OPcache_StableCache, fetchMultiple, arginfo_class_OPcache_StableCache_fetchMultiple, ZEND_ACC_PUBLIC)
	ZEND_ME(OPcache_StableCache, store, arginfo_class_OPcache_StableCache_store, ZEND_ACC_PUBLIC)
	ZEND_ME(OPcache_StableCache, storeMultiple, arginfo_class_OPcache_StableCache_storeMultiple, ZEND_ACC_PUBLIC)
	ZEND_ME(OPcache_StableCache, storeWithTtl, arginfo_class_OPcache_StableCache_storeWithTtl, ZEND_ACC_PUBLIC)
	ZEND_ME(OPcache_StableCache, storeMultipleWithTtl, arginfo_class_OPcache_StableCache_storeMultipleWithTtl, ZEND_ACC_PUBLIC)
	ZEND_ME(OPcache_StableCache, has, arginfo_class_OPcache_StableCache_has, ZEND_ACC_PUBLIC)
	ZEND_ME(OPcache_StableCache, delete, arginfo_class_OPcache_StableCache_delete, ZEND_ACC_PUBLIC)
	ZEND_ME(OPcache_StableCache, deleteMultiple, arginfo_class_OPcache_StableCache_deleteMultiple, ZEND_ACC_PUBLIC)
	ZEND_ME(OPcache_StableCache, clear, arginfo_class_OPcache_StableCache_clear, ZEND_ACC_PUBLIC)
	ZEND_ME(OPcache_StableCache, lock, arginfo_class_OPcache_StableCache_lock, ZEND_ACC_PUBLIC)
	ZEND_ME(OPcache_StableCache, unlock, arginfo_class_OPcache_StableCache_unlock, ZEND_ACC_PUBLIC)
	ZEND_ME(OPcache_StableCache, increment, arginfo_class_OPcache_StableCache_increment, ZEND_ACC_PUBLIC)
	ZEND_ME(OPcache_StableCache, decrement, arginfo_class_OPcache_StableCache_decrement, ZEND_ACC_PUBLIC)
	ZEND_ME(OPcache_StableCache, info, arginfo_class_OPcache_StableCache_info, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_OPcache_StaticCacheException(zend_class_entry *class_entry_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "OPcache", "StaticCacheException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Exception, 0);

	return class_entry;
}

static zend_class_entry *register_class_OPcache_StaticCacheInfo(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "OPcache", "StaticCacheInfo", class_OPcache_StaticCacheInfo_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_READONLY_CLASS);

	zval property_enabled_default_value;
	ZVAL_UNDEF(&property_enabled_default_value);
	zend_string *property_enabled_name = zend_string_init("enabled", sizeof("enabled") - 1, true);
	zend_declare_typed_property(class_entry, property_enabled_name, &property_enabled_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release_ex(property_enabled_name, true);

	zval property_available_default_value;
	ZVAL_UNDEF(&property_available_default_value);
	zend_string *property_available_name = zend_string_init("available", sizeof("available") - 1, true);
	zend_declare_typed_property(class_entry, property_available_name, &property_available_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release_ex(property_available_name, true);

	zval property_startup_failed_default_value;
	ZVAL_UNDEF(&property_startup_failed_default_value);
	zend_string *property_startup_failed_name = zend_string_init("startup_failed", sizeof("startup_failed") - 1, true);
	zend_declare_typed_property(class_entry, property_startup_failed_name, &property_startup_failed_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release_ex(property_startup_failed_name, true);

	zval property_backend_initialized_default_value;
	ZVAL_UNDEF(&property_backend_initialized_default_value);
	zend_string *property_backend_initialized_name = zend_string_init("backend_initialized", sizeof("backend_initialized") - 1, true);
	zend_declare_typed_property(class_entry, property_backend_initialized_name, &property_backend_initialized_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release_ex(property_backend_initialized_name, true);

	zval property_configured_memory_default_value;
	ZVAL_UNDEF(&property_configured_memory_default_value);
	zend_string *property_configured_memory_name = zend_string_init("configured_memory", sizeof("configured_memory") - 1, true);
	zend_declare_typed_property(class_entry, property_configured_memory_name, &property_configured_memory_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(property_configured_memory_name, true);

	zval property_shared_memory_default_value;
	ZVAL_UNDEF(&property_shared_memory_default_value);
	zend_string *property_shared_memory_name = zend_string_init("shared_memory", sizeof("shared_memory") - 1, true);
	zend_declare_typed_property(class_entry, property_shared_memory_name, &property_shared_memory_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(property_shared_memory_name, true);

	zval property_entry_count_default_value;
	ZVAL_UNDEF(&property_entry_count_default_value);
	zend_string *property_entry_count_name = zend_string_init("entry_count", sizeof("entry_count") - 1, true);
	zend_declare_typed_property(class_entry, property_entry_count_name, &property_entry_count_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(property_entry_count_name, true);

	zval property_segment_count_default_value;
	ZVAL_UNDEF(&property_segment_count_default_value);
	zend_string *property_segment_count_name = zend_string_init("segment_count", sizeof("segment_count") - 1, true);
	zend_declare_typed_property(class_entry, property_segment_count_name, &property_segment_count_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(property_segment_count_name, true);

	zval property_shared_model_default_value;
	ZVAL_UNDEF(&property_shared_model_default_value);
	zend_string *property_shared_model_name = zend_string_init("shared_model", sizeof("shared_model") - 1, true);
	zend_declare_typed_property(class_entry, property_shared_model_name, &property_shared_model_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release_ex(property_shared_model_name, true);

	zval property_failure_reason_default_value;
	ZVAL_UNDEF(&property_failure_reason_default_value);
	zend_string *property_failure_reason_name = zend_string_init("failure_reason", sizeof("failure_reason") - 1, true);
	zend_declare_typed_property(class_entry, property_failure_reason_name, &property_failure_reason_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release_ex(property_failure_reason_name, true);

	return class_entry;
}

static zend_class_entry *register_class_OPcache_StableStatic(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "OPcache", "StableStatic", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);

	zend_string *attribute_name_Attribute_class_OPcache_StableStatic_0 = zend_string_init_interned("Attribute", sizeof("Attribute") - 1, true);
	zend_attribute *attribute_Attribute_class_OPcache_StableStatic_0 = zend_add_class_attribute(class_entry, attribute_name_Attribute_class_OPcache_StableStatic_0, 1);
	zend_string_release_ex(attribute_name_Attribute_class_OPcache_StableStatic_0, true);
	ZVAL_LONG(&attribute_Attribute_class_OPcache_StableStatic_0->args[0].value, 13);

	return class_entry;
}

static zend_class_entry *register_class_OPcache_CacheStrategy(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("OPcache\\CacheStrategy", IS_LONG, NULL);

	zval enum_case_Immediate_value;
	ZVAL_LONG(&enum_case_Immediate_value, 0);
	zend_enum_add_case_cstr(class_entry, "Immediate", &enum_case_Immediate_value);

	zval enum_case_Tracking_value;
	ZVAL_LONG(&enum_case_Tracking_value, 1);
	zend_enum_add_case_cstr(class_entry, "Tracking", &enum_case_Tracking_value);

	return class_entry;
}

static zend_class_entry *register_class_OPcache_VolatileStatic(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "OPcache", "VolatileStatic", class_OPcache_VolatileStatic_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);

	zval property_ttl_default_value;
	ZVAL_UNDEF(&property_ttl_default_value);
	zend_string *property_ttl_name = zend_string_init("ttl", sizeof("ttl") - 1, true);
	zend_declare_typed_property(class_entry, property_ttl_name, &property_ttl_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(property_ttl_name, true);

	zval property_strategy_default_value;
	ZVAL_UNDEF(&property_strategy_default_value);
	zend_string *property_strategy_name = zend_string_init("strategy", sizeof("strategy") - 1, true);
	zend_string *property_strategy_class_OPcache_CacheStrategy = zend_string_init("OPcache\\CacheStrategy", sizeof("OPcache\\CacheStrategy")-1, 1);
	zend_declare_typed_property(class_entry, property_strategy_name, &property_strategy_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_strategy_class_OPcache_CacheStrategy, 0, 0));
	zend_string_release_ex(property_strategy_name, true);

	zend_string *attribute_name_Attribute_class_OPcache_VolatileStatic_0 = zend_string_init_interned("Attribute", sizeof("Attribute") - 1, true);
	zend_attribute *attribute_Attribute_class_OPcache_VolatileStatic_0 = zend_add_class_attribute(class_entry, attribute_name_Attribute_class_OPcache_VolatileStatic_0, 1);
	zend_string_release_ex(attribute_name_Attribute_class_OPcache_VolatileStatic_0, true);
	ZVAL_LONG(&attribute_Attribute_class_OPcache_VolatileStatic_0->args[0].value, 13);

	return class_entry;
}

static zend_class_entry *register_class_OPcache_StaticCacheInterface(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "OPcache", "StaticCacheInterface", class_OPcache_StaticCacheInterface_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}

static zend_class_entry *register_class_OPcache_VolatileCache(zend_class_entry *class_entry_OPcache_StaticCacheInterface)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "OPcache", "VolatileCache", class_OPcache_VolatileCache_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NOT_SERIALIZABLE);
	zend_class_implements(class_entry, 1, class_entry_OPcache_StaticCacheInterface);

	return class_entry;
}

static zend_class_entry *register_class_OPcache_StableCache(zend_class_entry *class_entry_OPcache_StaticCacheInterface)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "OPcache", "StableCache", class_OPcache_StableCache_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NOT_SERIALIZABLE);
	zend_class_implements(class_entry, 1, class_entry_OPcache_StaticCacheInterface);

	return class_entry;
}
