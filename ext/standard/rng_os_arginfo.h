/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 002ac5ccf036dde144c80a2a8d7950d56a50e7bc */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_RNG_OS_next, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_RNG_OS_next64 arginfo_class_RNG_OS_next


ZEND_METHOD(RNG_OS, next);
ZEND_METHOD(RNG_OS, next64);


static const zend_function_entry class_RNG_OS_methods[] = {
	ZEND_ME(RNG_OS, next, arginfo_class_RNG_OS_next, ZEND_ACC_PUBLIC)
	ZEND_ME(RNG_OS, next64, arginfo_class_RNG_OS_next64, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
