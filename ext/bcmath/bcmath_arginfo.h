/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 687d6fb392a9b0c1329152cc0f62341a73e427f4 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_bcadd, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, num1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, num2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, scale, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_bcsub arginfo_bcadd

#define arginfo_bcmul arginfo_bcadd

#define arginfo_bcdiv arginfo_bcadd

#define arginfo_bcmod arginfo_bcadd

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_bcdivmod, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, num1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, num2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, scale, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_bcpowmod, 0, 3, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, num, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, exponent, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, modulus, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, scale, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_bcpow, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, num, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, exponent, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, scale, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_bcsqrt, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, num, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, scale, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_bccomp, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, num1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, num2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, scale, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_bcscale, 0, 0, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, scale, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_bcfloor, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, num, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_bcceil arginfo_bcfloor

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_bcround, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, num, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, precision, IS_LONG, 0, "0")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, mode, RoundingMode, 0, "RoundingMode::HalfAwayFromZero")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_BcMath_Number___construct, 0, 0, 1)
	ZEND_ARG_TYPE_MASK(0, num, MAY_BE_STRING|MAY_BE_LONG, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_BcMath_Number_add, 0, 1, BcMath\\\116umber, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, num, BcMath\\\116umber, MAY_BE_STRING|MAY_BE_LONG, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, scale, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_BcMath_Number_sub arginfo_class_BcMath_Number_add

#define arginfo_class_BcMath_Number_mul arginfo_class_BcMath_Number_add

#define arginfo_class_BcMath_Number_div arginfo_class_BcMath_Number_add

#define arginfo_class_BcMath_Number_mod arginfo_class_BcMath_Number_add

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_BcMath_Number_divmod, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, num, BcMath\\\116umber, MAY_BE_STRING|MAY_BE_LONG, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, scale, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_BcMath_Number_powmod, 0, 2, BcMath\\\116umber, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, exponent, BcMath\\\116umber, MAY_BE_STRING|MAY_BE_LONG, NULL)
	ZEND_ARG_OBJ_TYPE_MASK(0, modulus, BcMath\\\116umber, MAY_BE_STRING|MAY_BE_LONG, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, scale, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_BcMath_Number_pow, 0, 1, BcMath\\\116umber, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, exponent, BcMath\\\116umber, MAY_BE_STRING|MAY_BE_LONG, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, scale, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_BcMath_Number_sqrt, 0, 0, BcMath\\\116umber, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, scale, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_BcMath_Number_floor, 0, 0, BcMath\\\116umber, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_BcMath_Number_ceil arginfo_class_BcMath_Number_floor

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_BcMath_Number_round, 0, 0, BcMath\\\116umber, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, precision, IS_LONG, 0, "0")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, mode, RoundingMode, 0, "RoundingMode::HalfAwayFromZero")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_BcMath_Number_compare, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, num, BcMath\\\116umber, MAY_BE_STRING|MAY_BE_LONG, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, scale, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_BcMath_Number___toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_BcMath_Number___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_BcMath_Number___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_FUNCTION(bcadd);
ZEND_FUNCTION(bcsub);
ZEND_FUNCTION(bcmul);
ZEND_FUNCTION(bcdiv);
ZEND_FUNCTION(bcmod);
ZEND_FUNCTION(bcdivmod);
ZEND_FUNCTION(bcpowmod);
ZEND_FUNCTION(bcpow);
ZEND_FUNCTION(bcsqrt);
ZEND_FUNCTION(bccomp);
ZEND_FUNCTION(bcscale);
ZEND_FUNCTION(bcfloor);
ZEND_FUNCTION(bcceil);
ZEND_FUNCTION(bcround);
ZEND_METHOD(BcMath_Number, __construct);
ZEND_METHOD(BcMath_Number, add);
ZEND_METHOD(BcMath_Number, sub);
ZEND_METHOD(BcMath_Number, mul);
ZEND_METHOD(BcMath_Number, div);
ZEND_METHOD(BcMath_Number, mod);
ZEND_METHOD(BcMath_Number, divmod);
ZEND_METHOD(BcMath_Number, powmod);
ZEND_METHOD(BcMath_Number, pow);
ZEND_METHOD(BcMath_Number, sqrt);
ZEND_METHOD(BcMath_Number, floor);
ZEND_METHOD(BcMath_Number, ceil);
ZEND_METHOD(BcMath_Number, round);
ZEND_METHOD(BcMath_Number, compare);
ZEND_METHOD(BcMath_Number, __toString);
ZEND_METHOD(BcMath_Number, __serialize);
ZEND_METHOD(BcMath_Number, __unserialize);

static const zend_function_entry ext_functions[] = {
	ZEND_FE(bcadd, arginfo_bcadd)
	ZEND_FE(bcsub, arginfo_bcsub)
	ZEND_FE(bcmul, arginfo_bcmul)
	ZEND_FE(bcdiv, arginfo_bcdiv)
	ZEND_FE(bcmod, arginfo_bcmod)
	ZEND_FE(bcdivmod, arginfo_bcdivmod)
	ZEND_FE(bcpowmod, arginfo_bcpowmod)
	ZEND_FE(bcpow, arginfo_bcpow)
	ZEND_FE(bcsqrt, arginfo_bcsqrt)
	ZEND_FE(bccomp, arginfo_bccomp)
	ZEND_FE(bcscale, arginfo_bcscale)
	ZEND_FE(bcfloor, arginfo_bcfloor)
	ZEND_FE(bcceil, arginfo_bcceil)
	ZEND_FE(bcround, arginfo_bcround)
	ZEND_FE_END
};

static const zend_function_entry class_BcMath_Number_methods[] = {
	ZEND_ME(BcMath_Number, __construct, arginfo_class_BcMath_Number___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(BcMath_Number, add, arginfo_class_BcMath_Number_add, ZEND_ACC_PUBLIC)
	ZEND_ME(BcMath_Number, sub, arginfo_class_BcMath_Number_sub, ZEND_ACC_PUBLIC)
	ZEND_ME(BcMath_Number, mul, arginfo_class_BcMath_Number_mul, ZEND_ACC_PUBLIC)
	ZEND_ME(BcMath_Number, div, arginfo_class_BcMath_Number_div, ZEND_ACC_PUBLIC)
	ZEND_ME(BcMath_Number, mod, arginfo_class_BcMath_Number_mod, ZEND_ACC_PUBLIC)
	ZEND_ME(BcMath_Number, divmod, arginfo_class_BcMath_Number_divmod, ZEND_ACC_PUBLIC)
	ZEND_ME(BcMath_Number, powmod, arginfo_class_BcMath_Number_powmod, ZEND_ACC_PUBLIC)
	ZEND_ME(BcMath_Number, pow, arginfo_class_BcMath_Number_pow, ZEND_ACC_PUBLIC)
	ZEND_ME(BcMath_Number, sqrt, arginfo_class_BcMath_Number_sqrt, ZEND_ACC_PUBLIC)
	ZEND_ME(BcMath_Number, floor, arginfo_class_BcMath_Number_floor, ZEND_ACC_PUBLIC)
	ZEND_ME(BcMath_Number, ceil, arginfo_class_BcMath_Number_ceil, ZEND_ACC_PUBLIC)
	ZEND_ME(BcMath_Number, round, arginfo_class_BcMath_Number_round, ZEND_ACC_PUBLIC)
	ZEND_ME(BcMath_Number, compare, arginfo_class_BcMath_Number_compare, ZEND_ACC_PUBLIC)
	ZEND_ME(BcMath_Number, __toString, arginfo_class_BcMath_Number___toString, ZEND_ACC_PUBLIC)
	ZEND_ME(BcMath_Number, __serialize, arginfo_class_BcMath_Number___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(BcMath_Number, __unserialize, arginfo_class_BcMath_Number___unserialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_BcMath_Number(zend_class_entry *class_entry_Stringable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "BcMath", "Number", class_BcMath_Number_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_READONLY_CLASS);
	zend_class_implements(class_entry, 1, class_entry_Stringable);

	zval property_value_default_value;
	ZVAL_UNDEF(&property_value_default_value);
	zend_string *property_value_name = zend_string_init("value", sizeof("value") - 1, 1);
	zend_declare_typed_property(class_entry, property_value_name, &property_value_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY|ZEND_ACC_VIRTUAL, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_value_name);

	zval property_scale_default_value;
	ZVAL_UNDEF(&property_scale_default_value);
	zend_string *property_scale_name = zend_string_init("scale", sizeof("scale") - 1, 1);
	zend_declare_typed_property(class_entry, property_scale_name, &property_scale_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY|ZEND_ACC_VIRTUAL, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_scale_name);

	return class_entry;
}
