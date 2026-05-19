/* This is a generated file, edit opcache.stub.php instead.
 * Stub hash: cc89c045c31ebd7d8db0e9c0a12c136451472cc5 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_opcache_reset, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

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

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_OPcache_volatile_store, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, value, MAY_BE_NULL|MAY_BE_BOOL|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, ttl, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_OPcache_volatile_store_array, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, values, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, ttl, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_OPcache_volatile_fetch, 0, 1, MAY_BE_NULL|MAY_BE_BOOL|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, default, MAY_BE_NULL|MAY_BE_BOOL|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_OPcache_volatile_fetch_array, 0, 1, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, default, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_OPcache_volatile_exists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_OPcache_volatile_lock, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, lease, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_OPcache_volatile_unlock arginfo_OPcache_volatile_exists

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_OPcache_volatile_delete, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, key_or_class, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_OPcache_volatile_delete_array, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_OPcache_volatile_clear, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_OPcache_volatile_cache_info, 0, 0, OPcache\\StaticCacheInfo, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_OPcache_pinned_store, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, value, MAY_BE_NULL|MAY_BE_BOOL|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_OPcache_pinned_store_array, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, values, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_OPcache_pinned_fetch arginfo_OPcache_volatile_fetch

#define arginfo_OPcache_pinned_fetch_array arginfo_OPcache_volatile_fetch_array

#define arginfo_OPcache_pinned_exists arginfo_OPcache_volatile_exists

#define arginfo_OPcache_pinned_lock arginfo_OPcache_volatile_lock

#define arginfo_OPcache_pinned_unlock arginfo_OPcache_volatile_exists

#define arginfo_OPcache_pinned_delete arginfo_OPcache_volatile_delete

#define arginfo_OPcache_pinned_delete_array arginfo_OPcache_volatile_delete_array

#define arginfo_OPcache_pinned_clear arginfo_OPcache_volatile_clear

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_OPcache_pinned_atomic_increment, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, step, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

#define arginfo_OPcache_pinned_atomic_decrement arginfo_OPcache_pinned_atomic_increment

#define arginfo_OPcache_pinned_cache_info arginfo_OPcache_volatile_cache_info

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OPcache_StaticCacheInfo___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OPcache_VolatileStatic___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, ttl, IS_LONG, 0, "0")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, strategy, OPcache\\CacheStrategy, 0, "OPcache\\CacheStrategy::Immediate")
ZEND_END_ARG_INFO()

ZEND_FUNCTION(opcache_reset);
ZEND_FUNCTION(opcache_get_status);
ZEND_FUNCTION(opcache_compile_file);
ZEND_FUNCTION(opcache_invalidate);
ZEND_FUNCTION(opcache_jit_blacklist);
ZEND_FUNCTION(opcache_get_configuration);
ZEND_FUNCTION(opcache_is_script_cached);
ZEND_FUNCTION(opcache_is_script_cached_in_file_cache);
ZEND_FUNCTION(OPcache_volatile_store);
ZEND_FUNCTION(OPcache_volatile_store_array);
ZEND_FUNCTION(OPcache_volatile_fetch);
ZEND_FUNCTION(OPcache_volatile_fetch_array);
ZEND_FUNCTION(OPcache_volatile_exists);
ZEND_FUNCTION(OPcache_volatile_lock);
ZEND_FUNCTION(OPcache_volatile_unlock);
ZEND_FUNCTION(OPcache_volatile_delete);
ZEND_FUNCTION(OPcache_volatile_delete_array);
ZEND_FUNCTION(OPcache_volatile_clear);
ZEND_FUNCTION(OPcache_volatile_cache_info);
ZEND_FUNCTION(OPcache_pinned_store);
ZEND_FUNCTION(OPcache_pinned_store_array);
ZEND_FUNCTION(OPcache_pinned_fetch);
ZEND_FUNCTION(OPcache_pinned_fetch_array);
ZEND_FUNCTION(OPcache_pinned_exists);
ZEND_FUNCTION(OPcache_pinned_lock);
ZEND_FUNCTION(OPcache_pinned_unlock);
ZEND_FUNCTION(OPcache_pinned_delete);
ZEND_FUNCTION(OPcache_pinned_delete_array);
ZEND_FUNCTION(OPcache_pinned_clear);
ZEND_FUNCTION(OPcache_pinned_atomic_increment);
ZEND_FUNCTION(OPcache_pinned_atomic_decrement);
ZEND_FUNCTION(OPcache_pinned_cache_info);
ZEND_METHOD(OPcache_StaticCacheInfo, __construct);
ZEND_METHOD(OPcache_VolatileStatic, __construct);

static const zend_function_entry ext_functions[] = {
	ZEND_FE(opcache_reset, arginfo_opcache_reset)
	ZEND_FE(opcache_get_status, arginfo_opcache_get_status)
	ZEND_FE(opcache_compile_file, arginfo_opcache_compile_file)
	ZEND_FE(opcache_invalidate, arginfo_opcache_invalidate)
	ZEND_FE(opcache_jit_blacklist, arginfo_opcache_jit_blacklist)
	ZEND_FE(opcache_get_configuration, arginfo_opcache_get_configuration)
	ZEND_FE(opcache_is_script_cached, arginfo_opcache_is_script_cached)
	ZEND_FE(opcache_is_script_cached_in_file_cache, arginfo_opcache_is_script_cached_in_file_cache)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("OPcache", "volatile_store"), zif_OPcache_volatile_store, arginfo_OPcache_volatile_store, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("OPcache", "volatile_store_array"), zif_OPcache_volatile_store_array, arginfo_OPcache_volatile_store_array, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("OPcache", "volatile_fetch"), zif_OPcache_volatile_fetch, arginfo_OPcache_volatile_fetch, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("OPcache", "volatile_fetch_array"), zif_OPcache_volatile_fetch_array, arginfo_OPcache_volatile_fetch_array, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("OPcache", "volatile_exists"), zif_OPcache_volatile_exists, arginfo_OPcache_volatile_exists, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("OPcache", "volatile_lock"), zif_OPcache_volatile_lock, arginfo_OPcache_volatile_lock, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("OPcache", "volatile_unlock"), zif_OPcache_volatile_unlock, arginfo_OPcache_volatile_unlock, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("OPcache", "volatile_delete"), zif_OPcache_volatile_delete, arginfo_OPcache_volatile_delete, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("OPcache", "volatile_delete_array"), zif_OPcache_volatile_delete_array, arginfo_OPcache_volatile_delete_array, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("OPcache", "volatile_clear"), zif_OPcache_volatile_clear, arginfo_OPcache_volatile_clear, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("OPcache", "volatile_cache_info"), zif_OPcache_volatile_cache_info, arginfo_OPcache_volatile_cache_info, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("OPcache", "pinned_store"), zif_OPcache_pinned_store, arginfo_OPcache_pinned_store, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("OPcache", "pinned_store_array"), zif_OPcache_pinned_store_array, arginfo_OPcache_pinned_store_array, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("OPcache", "pinned_fetch"), zif_OPcache_pinned_fetch, arginfo_OPcache_pinned_fetch, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("OPcache", "pinned_fetch_array"), zif_OPcache_pinned_fetch_array, arginfo_OPcache_pinned_fetch_array, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("OPcache", "pinned_exists"), zif_OPcache_pinned_exists, arginfo_OPcache_pinned_exists, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("OPcache", "pinned_lock"), zif_OPcache_pinned_lock, arginfo_OPcache_pinned_lock, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("OPcache", "pinned_unlock"), zif_OPcache_pinned_unlock, arginfo_OPcache_pinned_unlock, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("OPcache", "pinned_delete"), zif_OPcache_pinned_delete, arginfo_OPcache_pinned_delete, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("OPcache", "pinned_delete_array"), zif_OPcache_pinned_delete_array, arginfo_OPcache_pinned_delete_array, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("OPcache", "pinned_clear"), zif_OPcache_pinned_clear, arginfo_OPcache_pinned_clear, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("OPcache", "pinned_atomic_increment"), zif_OPcache_pinned_atomic_increment, arginfo_OPcache_pinned_atomic_increment, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("OPcache", "pinned_atomic_decrement"), zif_OPcache_pinned_atomic_decrement, arginfo_OPcache_pinned_atomic_decrement, 0, NULL, NULL)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("OPcache", "pinned_cache_info"), zif_OPcache_pinned_cache_info, arginfo_OPcache_pinned_cache_info, 0, NULL, NULL)
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

static zend_class_entry *register_class_OPcache_PinnedStatic(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "OPcache", "PinnedStatic", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL);

	zend_string *attribute_name_Attribute_class_OPcache_PinnedStatic_0 = zend_string_init_interned("Attribute", sizeof("Attribute") - 1, true);
	zend_attribute *attribute_Attribute_class_OPcache_PinnedStatic_0 = zend_add_class_attribute(class_entry, attribute_name_Attribute_class_OPcache_PinnedStatic_0, 1);
	zend_string_release_ex(attribute_name_Attribute_class_OPcache_PinnedStatic_0, true);
	ZVAL_LONG(&attribute_Attribute_class_OPcache_PinnedStatic_0->args[0].value, 13);

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
