/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 07055c94c2476d7e64cb012dbb71a81f43162a4a */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_lcg_value, 0, 0, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mt_srand, 0, 0, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, seed, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_LONG, 0, "MT_RAND_MT19937")
ZEND_END_ARG_INFO()

#define arginfo_srand arginfo_mt_srand

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_rand, 0, 0, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, min, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, max, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_mt_rand arginfo_rand

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mt_getrandmax, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_getrandmax arginfo_mt_getrandmax

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_random_bytes, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_random_int, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, min, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, max, IS_LONG, 0)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(lcg_value);
ZEND_FUNCTION(mt_srand);
ZEND_FUNCTION(rand);
ZEND_FUNCTION(mt_rand);
ZEND_FUNCTION(mt_getrandmax);
ZEND_FUNCTION(random_bytes);
ZEND_FUNCTION(random_int);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(lcg_value, arginfo_lcg_value)
	ZEND_FE(mt_srand, arginfo_mt_srand)
	ZEND_FALIAS(srand, mt_srand, arginfo_srand)
	ZEND_FE(rand, arginfo_rand)
	ZEND_FE(mt_rand, arginfo_mt_rand)
	ZEND_FE(mt_getrandmax, arginfo_mt_getrandmax)
	ZEND_FALIAS(getrandmax, mt_getrandmax, arginfo_getrandmax)
	ZEND_FE(random_bytes, arginfo_random_bytes)
	ZEND_FE(random_int, arginfo_random_int)
	ZEND_FE_END
};
