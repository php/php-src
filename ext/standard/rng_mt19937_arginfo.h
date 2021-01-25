/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 1729f4d864a1f9a8b07dd607128c1f6493c69f2c */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RNG_MT19937___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, seed, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_RNG_MT19937_next, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_RNG_MT19937_next64 arginfo_class_RNG_MT19937_next

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_RNG_MT19937___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_RNG_MT19937___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(RNG_MT19937, __construct);
ZEND_METHOD(RNG_MT19937, next);
ZEND_METHOD(RNG_MT19937, next64);
ZEND_METHOD(RNG_MT19937, __serialize);
ZEND_METHOD(RNG_MT19937, __unserialize);


static const zend_function_entry class_RNG_MT19937_methods[] = {
	ZEND_ME(RNG_MT19937, __construct, arginfo_class_RNG_MT19937___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(RNG_MT19937, next, arginfo_class_RNG_MT19937_next, ZEND_ACC_PUBLIC)
	ZEND_ME(RNG_MT19937, next64, arginfo_class_RNG_MT19937_next64, ZEND_ACC_PUBLIC)
	ZEND_ME(RNG_MT19937, __serialize, arginfo_class_RNG_MT19937___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(RNG_MT19937, __unserialize, arginfo_class_RNG_MT19937___unserialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
