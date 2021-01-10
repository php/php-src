/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 2a6e59481066545f803f93422c67ef124e4503eb */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_RNG_XorShift128Plus___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, seed, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_RNG_XorShift128Plus_next, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_RNG_XorShift128Plus_next64 arginfo_class_RNG_XorShift128Plus_next

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_RNG_XorShift128Plus___serialize, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_RNG_XorShift128Plus___unserialize, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(RNG_XorShift128Plus, __construct);
ZEND_METHOD(RNG_XorShift128Plus, next);
ZEND_METHOD(RNG_XorShift128Plus, next64);
ZEND_METHOD(RNG_XorShift128Plus, __serialize);
ZEND_METHOD(RNG_XorShift128Plus, __unserialize);


static const zend_function_entry class_RNG_XorShift128Plus_methods[] = {
	ZEND_ME(RNG_XorShift128Plus, __construct, arginfo_class_RNG_XorShift128Plus___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(RNG_XorShift128Plus, next, arginfo_class_RNG_XorShift128Plus_next, ZEND_ACC_PUBLIC)
	ZEND_ME(RNG_XorShift128Plus, next64, arginfo_class_RNG_XorShift128Plus_next64, ZEND_ACC_PUBLIC)
	ZEND_ME(RNG_XorShift128Plus, __serialize, arginfo_class_RNG_XorShift128Plus___serialize, ZEND_ACC_PUBLIC)
	ZEND_ME(RNG_XorShift128Plus, __unserialize, arginfo_class_RNG_XorShift128Plus___unserialize, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
