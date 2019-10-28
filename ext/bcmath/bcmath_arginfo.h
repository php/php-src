/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_bcadd, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, left_operand, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, right_operand, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, scale, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_bcsub arginfo_bcadd

#define arginfo_bcmul arginfo_bcadd

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_bcdiv, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, dividend, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, divisor, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, scale, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_bcmod arginfo_bcdiv

ZEND_BEGIN_ARG_INFO_EX(arginfo_bcpowmod, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, base, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, exponent, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, modulus, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, scale, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_bcpow, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, base, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, exponent, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, scale, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_bcsqrt, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, operand, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, scale, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_bccomp, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, left_operand, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, right_operand, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, scale, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_bcscale, 0, 0, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, scale, IS_LONG, 0)
ZEND_END_ARG_INFO()
