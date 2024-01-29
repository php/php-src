/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: f28dafc2a279f5421cd0d0e668fde0032e996ebc */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_bcadd, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, num1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, num2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, scale, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_bcsub arginfo_bcadd

#define arginfo_bcmul arginfo_bcadd

#define arginfo_bcdiv arginfo_bcadd

#define arginfo_bcmod arginfo_bcadd

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

ZEND_FUNCTION(bcadd);
ZEND_FUNCTION(bcsub);
ZEND_FUNCTION(bcmul);
ZEND_FUNCTION(bcdiv);
ZEND_FUNCTION(bcmod);
ZEND_FUNCTION(bcpowmod);
ZEND_FUNCTION(bcpow);
ZEND_FUNCTION(bcsqrt);
ZEND_FUNCTION(bccomp);
ZEND_FUNCTION(bcscale);

static const zend_function_entry ext_functions[] = {
	ZEND_RAW_FENTRY("bcadd", zif_bcadd, arginfo_bcadd, 0, NULL)
	ZEND_RAW_FENTRY("bcsub", zif_bcsub, arginfo_bcsub, 0, NULL)
	ZEND_RAW_FENTRY("bcmul", zif_bcmul, arginfo_bcmul, 0, NULL)
	ZEND_RAW_FENTRY("bcdiv", zif_bcdiv, arginfo_bcdiv, 0, NULL)
	ZEND_RAW_FENTRY("bcmod", zif_bcmod, arginfo_bcmod, 0, NULL)
	ZEND_RAW_FENTRY("bcpowmod", zif_bcpowmod, arginfo_bcpowmod, 0, NULL)
	ZEND_RAW_FENTRY("bcpow", zif_bcpow, arginfo_bcpow, 0, NULL)
	ZEND_RAW_FENTRY("bcsqrt", zif_bcsqrt, arginfo_bcsqrt, 0, NULL)
	ZEND_RAW_FENTRY("bccomp", zif_bccomp, arginfo_bccomp, 0, NULL)
	ZEND_RAW_FENTRY("bcscale", zif_bcscale, arginfo_bcscale, 0, NULL)
	ZEND_FE_END
};
