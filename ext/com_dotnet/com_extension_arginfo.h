/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_variant_set, 0, 2, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, variant, variant, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_variant_add, 0, 2, variant, 0)
	ZEND_ARG_INFO(0, left)
	ZEND_ARG_INFO(0, right)
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
	ZEND_ARG_INFO(0, left)
ZEND_END_ARG_INFO()

#define arginfo_variant_fix arginfo_variant_abs

#define arginfo_variant_int arginfo_variant_abs

#define arginfo_variant_neg arginfo_variant_abs

#define arginfo_variant_not arginfo_variant_abs

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_variant_round, 0, 2, variant, 1)
	ZEND_ARG_INFO(0, left)
	ZEND_ARG_TYPE_INFO(0, decimals, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_variant_cmp, 0, 0, 2)
	ZEND_ARG_INFO(0, left)
	ZEND_ARG_INFO(0, right)
	ZEND_ARG_TYPE_INFO(0, lcid, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_variant_date_to_timestamp, 0, 1, IS_LONG, 1)
	ZEND_ARG_OBJ_INFO(0, variant, variant, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_variant_date_from_timestamp, 0, 0, 1)
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
	ZEND_ARG_TYPE_INFO(0, progid, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, code_page, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_com_create_guid, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_com_event_sink, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, comobject, variant, 0)
	ZEND_ARG_TYPE_INFO(0, sinkobject, IS_OBJECT, 0)
	ZEND_ARG_INFO(0, sinkinterface)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_com_print_typeinfo, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, comobject)
	ZEND_ARG_TYPE_INFO(0, dispinterface, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, wantsink, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_com_message_pump, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, timeoutms, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_com_load_typelib, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, typelib_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, case_insensitive, _IS_BOOL, 0)
ZEND_END_ARG_INFO()
