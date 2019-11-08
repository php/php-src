/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zend_version, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_func_num_args, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_func_get_arg, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, arg_num, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_func_get_args, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_strlen, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_strcmp, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, str1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str2, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_strncmp, 0, 3, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, str1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, len, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_error_reporting, 0, 0, IS_LONG, 0)
	ZEND_ARG_INFO(0, new_error_level)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_define, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, constant_name, IS_STRING, 0)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_TYPE_INFO(0, case_insensitive, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_defined, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, constant_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_get_class, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, object, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_get_called_class, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_get_parent_class, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_is_subclass_of, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_TYPE_INFO(0, class_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, allow_string, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_is_a arginfo_is_subclass_of

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_get_class_vars, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, class_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_get_object_vars, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, obj, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

#define arginfo_get_mangled_object_vars arginfo_get_object_vars

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_get_class_methods, 0, 1, IS_ARRAY, 1)
	ZEND_ARG_INFO(0, class)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_method_exists, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, object_or_class)
	ZEND_ARG_TYPE_INFO(0, method, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_property_exists, 0, 2, _IS_BOOL, 1)
	ZEND_ARG_INFO(0, object_or_class)
	ZEND_ARG_TYPE_INFO(0, property_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_exists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, classname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, autoload, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_interface_exists arginfo_class_exists

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_trait_exists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, traitname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, autoload, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_function_exists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, function_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_alias, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, user_class_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, alias_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, autoload, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_get_included_files, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_get_required_files arginfo_get_included_files

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_trigger_error, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, error_type, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_user_error arginfo_trigger_error

ZEND_BEGIN_ARG_INFO_EX(arginfo_set_error_handler, 0, 0, 1)
	ZEND_ARG_INFO(0, error_handler)
	ZEND_ARG_TYPE_INFO(0, error_types, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_restore_error_handler, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_set_exception_handler, 0, 0, 1)
	ZEND_ARG_INFO(0, exception_handler)
ZEND_END_ARG_INFO()

#define arginfo_restore_exception_handler arginfo_restore_error_handler

#define arginfo_get_declared_classes arginfo_get_included_files

#define arginfo_get_declared_traits arginfo_get_included_files

#define arginfo_get_declared_interfaces arginfo_get_included_files

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_get_defined_functions, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, exclude_disabled, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_get_defined_vars, 0, 0, IS_ARRAY, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_get_resource_type, 0, 1, IS_STRING, 0)
	ZEND_ARG_INFO(0, res)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_get_resources, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, type, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_get_loaded_extensions, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, zend_extensions, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_get_defined_constants, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, categorize, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_debug_backtrace, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, limit, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_debug_print_backtrace, 0, 0, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, limit, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_extension_loaded, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, extension_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_get_extension_funcs, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, extension_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

#if ZEND_DEBUG && defined(ZTS)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_zend_thread_id, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#define arginfo_gc_mem_caches arginfo_func_num_args

#define arginfo_gc_collect_cycles arginfo_func_num_args

#define arginfo_gc_enabled arginfo_restore_error_handler

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gc_enable, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_gc_disable arginfo_gc_enable

#define arginfo_gc_status arginfo_get_included_files
