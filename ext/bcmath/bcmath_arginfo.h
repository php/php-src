/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: cd3d182e13cb0ca22b27c13a8d0a86c20fde5b76 */

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

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_bcfloor, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, num, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_bcceil arginfo_bcfloor

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_bcround, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, num, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, precision, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "PHP_ROUND_HALF_UP")
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
ZEND_FUNCTION(bcfloor);
ZEND_FUNCTION(bcceil);
ZEND_FUNCTION(bcround);

static const zend_function_entry ext_functions[] = {
	ZEND_FE(bcadd, arginfo_bcadd)
	ZEND_FE(bcsub, arginfo_bcsub)
	ZEND_FE(bcmul, arginfo_bcmul)
	ZEND_FE(bcdiv, arginfo_bcdiv)
	ZEND_FE(bcmod, arginfo_bcmod)
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
