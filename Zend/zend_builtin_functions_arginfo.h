/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: c3bec3b17079456ef17e5c992995dcfbe62c6fe0 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zend_version, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_func_num_args, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_func_get_arg, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, position, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_func_get_args, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_strlen, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_strcmp, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, string1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string2, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_strncmp, 0, 3, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, string1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_strcasecmp arginfo_strcmp

#define arginfo_strncasecmp arginfo_strncmp

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_error_reporting, 0, 0, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, error_level, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_define, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, constant_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, case_insensitive, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_defined, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, constant_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_get_class, 0, 0, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, object, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

#define arginfo_get_called_class arginfo_zend_version

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_get_parent_class, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_MASK(0, object_or_class, MAY_BE_OBJECT|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_is_subclass_of, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, object_or_class, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, class, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, allow_string, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_is_a, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, object_or_class, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, class, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, allow_string, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_get_class_vars, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, class, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_get_object_vars, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, object, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

#define arginfo_get_mangled_object_vars arginfo_get_object_vars

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_get_class_methods, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_MASK(0, object_or_class, MAY_BE_OBJECT|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_method_exists, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, object_or_class)
	ZEND_ARG_TYPE_INFO(0, method, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_property_exists, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, object_or_class)
	ZEND_ARG_TYPE_INFO(0, property, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_exists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, class, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, autoload, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_interface_exists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, interface, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, autoload, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_trait_exists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, trait, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, autoload, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_enum_exists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, enum, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, autoload, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_function_exists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, function, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_alias, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, class, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, alias, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, autoload, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

#define arginfo_get_included_files arginfo_func_get_args

#define arginfo_get_required_files arginfo_func_get_args

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_trigger_error, 0, 1, IS_TRUE, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, error_level, IS_LONG, 0, "E_USER_NOTICE")
ZEND_END_ARG_INFO()

#define arginfo_user_error arginfo_trigger_error

ZEND_BEGIN_ARG_INFO_EX(arginfo_set_error_handler, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, error_levels, IS_LONG, 0, "E_ALL")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_restore_error_handler, 0, 0, IS_TRUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_set_exception_handler, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 1)
ZEND_END_ARG_INFO()

#define arginfo_restore_exception_handler arginfo_restore_error_handler

#define arginfo_get_declared_classes arginfo_func_get_args

#define arginfo_get_declared_traits arginfo_func_get_args

#define arginfo_get_declared_interfaces arginfo_func_get_args

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_get_defined_functions, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, exclude_disabled, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

#define arginfo_get_defined_vars arginfo_func_get_args

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_get_resource_type, 0, 1, IS_STRING, 0)
	ZEND_ARG_INFO(0, resource)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_get_resource_id, 0, 1, IS_LONG, 0)
	ZEND_ARG_INFO(0, resource)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_get_resources, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_get_loaded_extensions, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, zend_extensions, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_get_defined_constants, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, categorize, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_debug_backtrace, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "DEBUG_BACKTRACE_PROVIDE_OBJECT")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, limit, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_debug_print_backtrace, 0, 0, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, limit, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_extension_loaded, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, extension, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_get_extension_funcs, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, extension, IS_STRING, 0)
ZEND_END_ARG_INFO()

#if ZEND_DEBUG && defined(ZTS)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zend_thread_id, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#define arginfo_gc_mem_caches arginfo_func_num_args

#define arginfo_gc_collect_cycles arginfo_func_num_args

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gc_enabled, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gc_enable, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_gc_disable arginfo_gc_enable

#define arginfo_gc_status arginfo_func_get_args


ZEND_FRAMELESS_FUNCTION(property_exists, 2);
static const zend_frameless_function_info frameless_function_infos_property_exists[] = {
	{ ZEND_FRAMELESS_FUNCTION_NAME(property_exists, 2), 2 },
	{ 0 },
};

ZEND_FRAMELESS_FUNCTION(class_exists, 1);
ZEND_FRAMELESS_FUNCTION(class_exists, 2);
static const zend_frameless_function_info frameless_function_infos_class_exists[] = {
	{ ZEND_FRAMELESS_FUNCTION_NAME(class_exists, 1), 1 },
	{ ZEND_FRAMELESS_FUNCTION_NAME(class_exists, 2), 2 },
	{ 0 },
};

ZEND_FUNCTION(zend_version);
ZEND_FUNCTION(func_num_args);
ZEND_FUNCTION(func_get_arg);
ZEND_FUNCTION(func_get_args);
ZEND_FUNCTION(strlen);
ZEND_FUNCTION(strcmp);
ZEND_FUNCTION(strncmp);
ZEND_FUNCTION(strcasecmp);
ZEND_FUNCTION(strncasecmp);
ZEND_FUNCTION(error_reporting);
ZEND_FUNCTION(define);
ZEND_FUNCTION(defined);
ZEND_FUNCTION(get_class);
ZEND_FUNCTION(get_called_class);
ZEND_FUNCTION(get_parent_class);
ZEND_FUNCTION(is_subclass_of);
ZEND_FUNCTION(is_a);
ZEND_FUNCTION(get_class_vars);
ZEND_FUNCTION(get_object_vars);
ZEND_FUNCTION(get_mangled_object_vars);
ZEND_FUNCTION(get_class_methods);
ZEND_FUNCTION(method_exists);
ZEND_FUNCTION(property_exists);
ZEND_FUNCTION(class_exists);
ZEND_FUNCTION(interface_exists);
ZEND_FUNCTION(trait_exists);
ZEND_FUNCTION(enum_exists);
ZEND_FUNCTION(function_exists);
ZEND_FUNCTION(class_alias);
ZEND_FUNCTION(get_included_files);
ZEND_FUNCTION(trigger_error);
ZEND_FUNCTION(set_error_handler);
ZEND_FUNCTION(restore_error_handler);
ZEND_FUNCTION(set_exception_handler);
ZEND_FUNCTION(restore_exception_handler);
ZEND_FUNCTION(get_declared_classes);
ZEND_FUNCTION(get_declared_traits);
ZEND_FUNCTION(get_declared_interfaces);
ZEND_FUNCTION(get_defined_functions);
ZEND_FUNCTION(get_defined_vars);
ZEND_FUNCTION(get_resource_type);
ZEND_FUNCTION(get_resource_id);
ZEND_FUNCTION(get_resources);
ZEND_FUNCTION(get_loaded_extensions);
ZEND_FUNCTION(get_defined_constants);
ZEND_FUNCTION(debug_backtrace);
ZEND_FUNCTION(debug_print_backtrace);
ZEND_FUNCTION(extension_loaded);
ZEND_FUNCTION(get_extension_funcs);
#if ZEND_DEBUG && defined(ZTS)
ZEND_FUNCTION(zend_thread_id);
#endif
ZEND_FUNCTION(gc_mem_caches);
ZEND_FUNCTION(gc_collect_cycles);
ZEND_FUNCTION(gc_enabled);
ZEND_FUNCTION(gc_enable);
ZEND_FUNCTION(gc_disable);
ZEND_FUNCTION(gc_status);

static const zend_function_entry ext_functions[] = {
	ZEND_RAW_FENTRY("zend_version", zif_zend_version, arginfo_zend_version, 0, NULL)
	ZEND_RAW_FENTRY("func_num_args", zif_func_num_args, arginfo_func_num_args, 0, NULL)
	ZEND_RAW_FENTRY("func_get_arg", zif_func_get_arg, arginfo_func_get_arg, 0, NULL)
	ZEND_RAW_FENTRY("func_get_args", zif_func_get_args, arginfo_func_get_args, 0, NULL)
	ZEND_RAW_FENTRY("strlen", zif_strlen, arginfo_strlen, 0, NULL)
	ZEND_RAW_FENTRY("strcmp", zif_strcmp, arginfo_strcmp, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("strncmp", zif_strncmp, arginfo_strncmp, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("strcasecmp", zif_strcasecmp, arginfo_strcasecmp, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("strncasecmp", zif_strncasecmp, arginfo_strncasecmp, ZEND_ACC_COMPILE_TIME_EVAL, NULL)
	ZEND_RAW_FENTRY("error_reporting", zif_error_reporting, arginfo_error_reporting, 0, NULL)
	ZEND_RAW_FENTRY("define", zif_define, arginfo_define, 0, NULL)
	ZEND_RAW_FENTRY("defined", zif_defined, arginfo_defined, 0, NULL)
	ZEND_RAW_FENTRY("get_class", zif_get_class, arginfo_get_class, 0, NULL)
	ZEND_RAW_FENTRY("get_called_class", zif_get_called_class, arginfo_get_called_class, 0, NULL)
	ZEND_RAW_FENTRY("get_parent_class", zif_get_parent_class, arginfo_get_parent_class, 0, NULL)
	ZEND_RAW_FENTRY("is_subclass_of", zif_is_subclass_of, arginfo_is_subclass_of, 0, NULL)
	ZEND_RAW_FENTRY("is_a", zif_is_a, arginfo_is_a, 0, NULL)
	ZEND_RAW_FENTRY("get_class_vars", zif_get_class_vars, arginfo_get_class_vars, 0, NULL)
	ZEND_RAW_FENTRY("get_object_vars", zif_get_object_vars, arginfo_get_object_vars, 0, NULL)
	ZEND_RAW_FENTRY("get_mangled_object_vars", zif_get_mangled_object_vars, arginfo_get_mangled_object_vars, 0, NULL)
	ZEND_RAW_FENTRY("get_class_methods", zif_get_class_methods, arginfo_get_class_methods, 0, NULL)
	ZEND_RAW_FENTRY("method_exists", zif_method_exists, arginfo_method_exists, 0, NULL)
	ZEND_FRAMELESS_FE(property_exists, arginfo_property_exists, 0, frameless_function_infos_property_exists)
	ZEND_FRAMELESS_FE(class_exists, arginfo_class_exists, 0, frameless_function_infos_class_exists)
	ZEND_RAW_FENTRY("interface_exists", zif_interface_exists, arginfo_interface_exists, 0, NULL)
	ZEND_RAW_FENTRY("trait_exists", zif_trait_exists, arginfo_trait_exists, 0, NULL)
	ZEND_RAW_FENTRY("enum_exists", zif_enum_exists, arginfo_enum_exists, 0, NULL)
	ZEND_RAW_FENTRY("function_exists", zif_function_exists, arginfo_function_exists, 0, NULL)
	ZEND_RAW_FENTRY("class_alias", zif_class_alias, arginfo_class_alias, 0, NULL)
	ZEND_RAW_FENTRY("get_included_files", zif_get_included_files, arginfo_get_included_files, 0, NULL)
	ZEND_RAW_FENTRY("get_required_files", zif_get_included_files, arginfo_get_required_files, 0, NULL)
	ZEND_RAW_FENTRY("trigger_error", zif_trigger_error, arginfo_trigger_error, 0, NULL)
	ZEND_RAW_FENTRY("user_error", zif_trigger_error, arginfo_user_error, 0, NULL)
	ZEND_RAW_FENTRY("set_error_handler", zif_set_error_handler, arginfo_set_error_handler, 0, NULL)
	ZEND_RAW_FENTRY("restore_error_handler", zif_restore_error_handler, arginfo_restore_error_handler, 0, NULL)
	ZEND_RAW_FENTRY("set_exception_handler", zif_set_exception_handler, arginfo_set_exception_handler, 0, NULL)
	ZEND_RAW_FENTRY("restore_exception_handler", zif_restore_exception_handler, arginfo_restore_exception_handler, 0, NULL)
	ZEND_RAW_FENTRY("get_declared_classes", zif_get_declared_classes, arginfo_get_declared_classes, 0, NULL)
	ZEND_RAW_FENTRY("get_declared_traits", zif_get_declared_traits, arginfo_get_declared_traits, 0, NULL)
	ZEND_RAW_FENTRY("get_declared_interfaces", zif_get_declared_interfaces, arginfo_get_declared_interfaces, 0, NULL)
	ZEND_RAW_FENTRY("get_defined_functions", zif_get_defined_functions, arginfo_get_defined_functions, 0, NULL)
	ZEND_RAW_FENTRY("get_defined_vars", zif_get_defined_vars, arginfo_get_defined_vars, 0, NULL)
	ZEND_RAW_FENTRY("get_resource_type", zif_get_resource_type, arginfo_get_resource_type, 0, NULL)
	ZEND_RAW_FENTRY("get_resource_id", zif_get_resource_id, arginfo_get_resource_id, 0, NULL)
	ZEND_RAW_FENTRY("get_resources", zif_get_resources, arginfo_get_resources, 0, NULL)
	ZEND_RAW_FENTRY("get_loaded_extensions", zif_get_loaded_extensions, arginfo_get_loaded_extensions, 0, NULL)
	ZEND_RAW_FENTRY("get_defined_constants", zif_get_defined_constants, arginfo_get_defined_constants, 0, NULL)
	ZEND_RAW_FENTRY("debug_backtrace", zif_debug_backtrace, arginfo_debug_backtrace, 0, NULL)
	ZEND_RAW_FENTRY("debug_print_backtrace", zif_debug_print_backtrace, arginfo_debug_print_backtrace, 0, NULL)
	ZEND_RAW_FENTRY("extension_loaded", zif_extension_loaded, arginfo_extension_loaded, 0, NULL)
	ZEND_RAW_FENTRY("get_extension_funcs", zif_get_extension_funcs, arginfo_get_extension_funcs, 0, NULL)
#if ZEND_DEBUG && defined(ZTS)
	ZEND_RAW_FENTRY("zend_thread_id", zif_zend_thread_id, arginfo_zend_thread_id, 0, NULL)
#endif
	ZEND_RAW_FENTRY("gc_mem_caches", zif_gc_mem_caches, arginfo_gc_mem_caches, 0, NULL)
	ZEND_RAW_FENTRY("gc_collect_cycles", zif_gc_collect_cycles, arginfo_gc_collect_cycles, 0, NULL)
	ZEND_RAW_FENTRY("gc_enabled", zif_gc_enabled, arginfo_gc_enabled, 0, NULL)
	ZEND_RAW_FENTRY("gc_enable", zif_gc_enable, arginfo_gc_enable, 0, NULL)
	ZEND_RAW_FENTRY("gc_disable", zif_gc_disable, arginfo_gc_disable, 0, NULL)
	ZEND_RAW_FENTRY("gc_status", zif_gc_status, arginfo_gc_status, 0, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_stdClass_methods[] = {
	ZEND_FE_END
};

static zend_class_entry *register_class_stdClass(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "stdClass", class_stdClass_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES;

	zend_string *attribute_name_AllowDynamicProperties_class_stdClass_0 = zend_string_init_interned("AllowDynamicProperties", sizeof("AllowDynamicProperties") - 1, 1);
	zend_add_class_attribute(class_entry, attribute_name_AllowDynamicProperties_class_stdClass_0, 0);
	zend_string_release(attribute_name_AllowDynamicProperties_class_stdClass_0);

	return class_entry;
}
