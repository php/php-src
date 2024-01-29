/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: b91206482b5119ce6d7c899e9599acfa2e06ec2a */

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
	ZEND_RAW_FENTRY("variant_set", zif_variant_set, arginfo_variant_set, 0, NULL)
	ZEND_RAW_FENTRY("variant_add", zif_variant_add, arginfo_variant_add, 0, NULL)
	ZEND_RAW_FENTRY("variant_cat", zif_variant_cat, arginfo_variant_cat, 0, NULL)
	ZEND_RAW_FENTRY("variant_sub", zif_variant_sub, arginfo_variant_sub, 0, NULL)
	ZEND_RAW_FENTRY("variant_mul", zif_variant_mul, arginfo_variant_mul, 0, NULL)
	ZEND_RAW_FENTRY("variant_and", zif_variant_and, arginfo_variant_and, 0, NULL)
	ZEND_RAW_FENTRY("variant_div", zif_variant_div, arginfo_variant_div, 0, NULL)
	ZEND_RAW_FENTRY("variant_eqv", zif_variant_eqv, arginfo_variant_eqv, 0, NULL)
	ZEND_RAW_FENTRY("variant_idiv", zif_variant_idiv, arginfo_variant_idiv, 0, NULL)
	ZEND_RAW_FENTRY("variant_imp", zif_variant_imp, arginfo_variant_imp, 0, NULL)
	ZEND_RAW_FENTRY("variant_mod", zif_variant_mod, arginfo_variant_mod, 0, NULL)
	ZEND_RAW_FENTRY("variant_or", zif_variant_or, arginfo_variant_or, 0, NULL)
	ZEND_RAW_FENTRY("variant_pow", zif_variant_pow, arginfo_variant_pow, 0, NULL)
	ZEND_RAW_FENTRY("variant_xor", zif_variant_xor, arginfo_variant_xor, 0, NULL)
	ZEND_RAW_FENTRY("variant_abs", zif_variant_abs, arginfo_variant_abs, 0, NULL)
	ZEND_RAW_FENTRY("variant_fix", zif_variant_fix, arginfo_variant_fix, 0, NULL)
	ZEND_RAW_FENTRY("variant_int", zif_variant_int, arginfo_variant_int, 0, NULL)
	ZEND_RAW_FENTRY("variant_neg", zif_variant_neg, arginfo_variant_neg, 0, NULL)
	ZEND_RAW_FENTRY("variant_not", zif_variant_not, arginfo_variant_not, 0, NULL)
	ZEND_RAW_FENTRY("variant_round", zif_variant_round, arginfo_variant_round, 0, NULL)
	ZEND_RAW_FENTRY("variant_cmp", zif_variant_cmp, arginfo_variant_cmp, 0, NULL)
	ZEND_RAW_FENTRY("variant_date_to_timestamp", zif_variant_date_to_timestamp, arginfo_variant_date_to_timestamp, 0, NULL)
	ZEND_RAW_FENTRY("variant_date_from_timestamp", zif_variant_date_from_timestamp, arginfo_variant_date_from_timestamp, 0, NULL)
	ZEND_RAW_FENTRY("variant_get_type", zif_variant_get_type, arginfo_variant_get_type, 0, NULL)
	ZEND_RAW_FENTRY("variant_set_type", zif_variant_set_type, arginfo_variant_set_type, 0, NULL)
	ZEND_RAW_FENTRY("variant_cast", zif_variant_cast, arginfo_variant_cast, 0, NULL)
	ZEND_RAW_FENTRY("com_get_active_object", zif_com_get_active_object, arginfo_com_get_active_object, 0, NULL)
	ZEND_RAW_FENTRY("com_create_guid", zif_com_create_guid, arginfo_com_create_guid, 0, NULL)
	ZEND_RAW_FENTRY("com_event_sink", zif_com_event_sink, arginfo_com_event_sink, 0, NULL)
	ZEND_RAW_FENTRY("com_print_typeinfo", zif_com_print_typeinfo, arginfo_com_print_typeinfo, 0, NULL)
	ZEND_RAW_FENTRY("com_message_pump", zif_com_message_pump, arginfo_com_message_pump, 0, NULL)
	ZEND_RAW_FENTRY("com_load_typelib", zif_com_load_typelib, arginfo_com_load_typelib, 0, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_variant_methods[] = {
	ZEND_RAW_FENTRY("__construct", zim_variant___construct, arginfo_class_variant___construct, ZEND_ACC_PUBLIC, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_com_methods[] = {
	ZEND_RAW_FENTRY("__construct", zim_com___construct, arginfo_class_com___construct, ZEND_ACC_PUBLIC, NULL)
	ZEND_FE_END
};

#if HAVE_MSCOREE_H
static const zend_function_entry class_dotnet_methods[] = {
	ZEND_RAW_FENTRY("__construct", zim_dotnet___construct, arginfo_class_dotnet___construct, ZEND_ACC_PUBLIC, NULL)
	ZEND_FE_END
};
#endif

static const zend_function_entry class_com_safearray_proxy_methods[] = {
	ZEND_FE_END
};

static const zend_function_entry class_com_exception_methods[] = {
	ZEND_FE_END
};

static void register_com_extension_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("CLSCTX_INPROC_SERVER", CLSCTX_INPROC_SERVER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CLSCTX_INPROC_HANDLER", CLSCTX_INPROC_HANDLER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CLSCTX_LOCAL_SERVER", CLSCTX_LOCAL_SERVER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CLSCTX_REMOTE_SERVER", CLSCTX_REMOTE_SERVER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CLSCTX_SERVER", CLSCTX_SERVER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CLSCTX_ALL", CLSCTX_ALL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_NULL", VT_NULL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_EMPTY", VT_EMPTY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_UI1", VT_UI1, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_I1", VT_I1, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_UI2", VT_UI2, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_I2", VT_I2, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_UI4", VT_UI4, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_I4", VT_I4, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_R4", VT_R4, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_R8", VT_R8, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_BOOL", VT_BOOL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_ERROR", VT_ERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_CY", VT_CY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_DATE", VT_DATE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_BSTR", VT_BSTR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_DECIMAL", VT_DECIMAL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_UNKNOWN", VT_UNKNOWN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_DISPATCH", VT_DISPATCH, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_VARIANT", VT_VARIANT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_INT", VT_INT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_UINT", VT_UINT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_ARRAY", VT_ARRAY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VT_BYREF", VT_BYREF, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CP_ACP", CP_ACP, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CP_MACCP", CP_MACCP, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CP_OEMCP", CP_OEMCP, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CP_UTF7", CP_UTF7, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CP_UTF8", CP_UTF8, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CP_SYMBOL", CP_SYMBOL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CP_THREAD_ACP", CP_THREAD_ACP, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VARCMP_LT", VARCMP_LT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VARCMP_EQ", VARCMP_EQ, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VARCMP_GT", VARCMP_GT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VARCMP_NULL", VARCMP_NULL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LOCALE_SYSTEM_DEFAULT", LOCALE_SYSTEM_DEFAULT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LOCALE_NEUTRAL", LOCALE_NEUTRAL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NORM_IGNORECASE", NORM_IGNORECASE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NORM_IGNORENONSPACE", NORM_IGNORENONSPACE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NORM_IGNORESYMBOLS", NORM_IGNORESYMBOLS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NORM_IGNOREWIDTH", NORM_IGNOREWIDTH, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("NORM_IGNOREKANATYPE", NORM_IGNOREKANATYPE, CONST_PERSISTENT);
#if defined(NORM_IGNOREKASHIDA)
	REGISTER_LONG_CONSTANT("NORM_IGNOREKASHIDA", NORM_IGNOREKASHIDA, CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("DISP_E_DIVBYZERO", PHP_DISP_E_DIVBYZERO, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DISP_E_OVERFLOW", PHP_DISP_E_OVERFLOW, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DISP_E_BADINDEX", PHP_DISP_E_BADINDEX, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DISP_E_PARAMNOTFOUND", PHP_DISP_E_PARAMNOTFOUND, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MK_E_UNAVAILABLE", PHP_MK_E_UNAVAILABLE, CONST_PERSISTENT);
#if SIZEOF_ZEND_LONG == 8
	REGISTER_LONG_CONSTANT("VT_UI8", VT_UI8, CONST_PERSISTENT);
#endif
#if SIZEOF_ZEND_LONG == 8
	REGISTER_LONG_CONSTANT("VT_I8", VT_I8, CONST_PERSISTENT);
#endif
}

static zend_class_entry *register_class_variant(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "variant", class_variant_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}

static zend_class_entry *register_class_com(zend_class_entry *class_entry_variant)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "com", class_com_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_variant);

	return class_entry;
}

#if HAVE_MSCOREE_H
static zend_class_entry *register_class_dotnet(zend_class_entry *class_entry_variant)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "dotnet", class_dotnet_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_variant);

	return class_entry;
}
#endif

static zend_class_entry *register_class_com_safearray_proxy(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "com_safearray_proxy", class_com_safearray_proxy_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	return class_entry;
}

static zend_class_entry *register_class_com_exception(zend_class_entry *class_entry_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "com_exception", class_com_exception_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_Exception);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	return class_entry;
}
