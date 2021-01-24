/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 6b162963bcceb90144fdd3165137fb567f916812 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_variant_set, 0, 2, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, variant, variant, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_variant_add, 0, 2, variant, 0)
	ZEND_ARG_TYPE_INFO(0, left, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, right, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_variant_cat arginfo_variant_add

#define arginfo_variant_sub arginfo_variant_add

#define arginfo_variant_mul arginfo_variant_add

#define arginfo_variant_and arginfo_variant_add

#define arginfo_variant_div arginfo_variant_add

#define arginfo_variant_eqv arginfo_variant_add

#define arginfo_variant_idiv arginfo_variant_add

#define arginfo_variant_imp arginfo_variant_add

#define arginfo_variant_mod arginfo_variant_add

#define arginfo_variant_or arginfo_variant_add

#define arginfo_variant_pow arginfo_variant_add

#define arginfo_variant_xor arginfo_variant_add

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_variant_abs, 0, 1, variant, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_variant_fix arginfo_variant_abs

#define arginfo_variant_int arginfo_variant_abs

#define arginfo_variant_neg arginfo_variant_abs

#define arginfo_variant_not arginfo_variant_abs

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_variant_round, 0, 2, variant, 1)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, decimals, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_variant_cmp, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, left, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, right, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, locale_id, IS_LONG, 0, "LOCALE_SYSTEM_DEFAULT")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_variant_date_to_timestamp, 0, 1, IS_LONG, 1)
	ZEND_ARG_OBJ_INFO(0, variant, variant, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_variant_date_from_timestamp, 0, 1, variant, 0)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_variant_get_type, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, variant, variant, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_variant_set_type, 0, 2, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, variant, variant, 0)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_variant_cast, 0, 2, variant, 0)
	ZEND_ARG_OBJ_INFO(0, variant, variant, 0)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_com_get_active_object, 0, 1, variant, 0)
	ZEND_ARG_TYPE_INFO(0, prog_id, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, codepage, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_com_create_guid, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_com_event_sink, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, variant, variant, 0)
	ZEND_ARG_TYPE_INFO(0, sink_object, IS_OBJECT, 0)
	ZEND_ARG_TYPE_MASK(0, sink_interface, MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_NULL, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_com_print_typeinfo, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, variant, variant, MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, dispatch_interface, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, display_sink, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_com_message_pump, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout_milliseconds, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_com_load_typelib, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, typelib, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, case_insensitive, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_variant___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, value, IS_MIXED, 0, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "VT_EMPTY")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, codepage, IS_LONG, 0, "CP_ACP")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_com___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, module_name, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, server_name, MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_NULL, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, codepage, IS_LONG, 0, "CP_ACP")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, typelib, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

#if HAVE_MSCOREE_H
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_dotnet___construct, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, assembly_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, datatype_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, codepage, IS_LONG, 0, "CP_ACP")
ZEND_END_ARG_INFO()
#endif


ZEND_FUNCTION(variant_set);
ZEND_FUNCTION(variant_add);
ZEND_FUNCTION(variant_cat);
ZEND_FUNCTION(variant_sub);
ZEND_FUNCTION(variant_mul);
ZEND_FUNCTION(variant_and);
ZEND_FUNCTION(variant_div);
ZEND_FUNCTION(variant_eqv);
ZEND_FUNCTION(variant_idiv);
ZEND_FUNCTION(variant_imp);
ZEND_FUNCTION(variant_mod);
ZEND_FUNCTION(variant_or);
ZEND_FUNCTION(variant_pow);
ZEND_FUNCTION(variant_xor);
ZEND_FUNCTION(variant_abs);
ZEND_FUNCTION(variant_fix);
ZEND_FUNCTION(variant_int);
ZEND_FUNCTION(variant_neg);
ZEND_FUNCTION(variant_not);
ZEND_FUNCTION(variant_round);
ZEND_FUNCTION(variant_cmp);
ZEND_FUNCTION(variant_date_to_timestamp);
ZEND_FUNCTION(variant_date_from_timestamp);
ZEND_FUNCTION(variant_get_type);
ZEND_FUNCTION(variant_set_type);
ZEND_FUNCTION(variant_cast);
ZEND_FUNCTION(com_get_active_object);
ZEND_FUNCTION(com_create_guid);
ZEND_FUNCTION(com_event_sink);
ZEND_FUNCTION(com_print_typeinfo);
ZEND_FUNCTION(com_message_pump);
ZEND_FUNCTION(com_load_typelib);
ZEND_METHOD(variant, __construct);
ZEND_METHOD(com, __construct);
#if HAVE_MSCOREE_H
ZEND_METHOD(dotnet, __construct);
#endif


static const zend_function_entry ext_functions[] = {
	ZEND_FE(variant_set, arginfo_variant_set)
	ZEND_FE(variant_add, arginfo_variant_add)
	ZEND_FE(variant_cat, arginfo_variant_cat)
	ZEND_FE(variant_sub, arginfo_variant_sub)
	ZEND_FE(variant_mul, arginfo_variant_mul)
	ZEND_FE(variant_and, arginfo_variant_and)
	ZEND_FE(variant_div, arginfo_variant_div)
	ZEND_FE(variant_eqv, arginfo_variant_eqv)
	ZEND_FE(variant_idiv, arginfo_variant_idiv)
	ZEND_FE(variant_imp, arginfo_variant_imp)
	ZEND_FE(variant_mod, arginfo_variant_mod)
	ZEND_FE(variant_or, arginfo_variant_or)
	ZEND_FE(variant_pow, arginfo_variant_pow)
	ZEND_FE(variant_xor, arginfo_variant_xor)
	ZEND_FE(variant_abs, arginfo_variant_abs)
	ZEND_FE(variant_fix, arginfo_variant_fix)
	ZEND_FE(variant_int, arginfo_variant_int)
	ZEND_FE(variant_neg, arginfo_variant_neg)
	ZEND_FE(variant_not, arginfo_variant_not)
	ZEND_FE(variant_round, arginfo_variant_round)
	ZEND_FE(variant_cmp, arginfo_variant_cmp)
	ZEND_FE(variant_date_to_timestamp, arginfo_variant_date_to_timestamp)
	ZEND_FE(variant_date_from_timestamp, arginfo_variant_date_from_timestamp)
	ZEND_FE(variant_get_type, arginfo_variant_get_type)
	ZEND_FE(variant_set_type, arginfo_variant_set_type)
	ZEND_FE(variant_cast, arginfo_variant_cast)
	ZEND_FE(com_get_active_object, arginfo_com_get_active_object)
	ZEND_FE(com_create_guid, arginfo_com_create_guid)
	ZEND_FE(com_event_sink, arginfo_com_event_sink)
	ZEND_FE(com_print_typeinfo, arginfo_com_print_typeinfo)
	ZEND_FE(com_message_pump, arginfo_com_message_pump)
	ZEND_FE(com_load_typelib, arginfo_com_load_typelib)
	ZEND_FE_END
};


static const zend_function_entry class_variant_methods[] = {
	ZEND_ME(variant, __construct, arginfo_class_variant___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_com_methods[] = {
	ZEND_ME(com, __construct, arginfo_class_com___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_dotnet_methods[] = {
#if HAVE_MSCOREE_H
	ZEND_ME(dotnet, __construct, arginfo_class_dotnet___construct, ZEND_ACC_PUBLIC)
#endif
	ZEND_FE_END
};


static const zend_function_entry class_com_exception_methods[] = {
	ZEND_FE_END
};
