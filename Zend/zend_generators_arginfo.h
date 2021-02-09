/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 50044c6c8d2a162a988906c0b752a5fe28adb933 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Generator_rewind, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Generator_valid, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Generator_current, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Generator_key arginfo_class_Generator_current

#define arginfo_class_Generator_next arginfo_class_Generator_rewind

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Generator_send, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Generator_throw, 0, 1, IS_MIXED, 0)
	ZEND_ARG_OBJ_INFO(0, exception, Throwable, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Generator_getReturn arginfo_class_Generator_current


ZEND_METHOD(Generator, rewind);
ZEND_METHOD(Generator, valid);
ZEND_METHOD(Generator, current);
ZEND_METHOD(Generator, key);
ZEND_METHOD(Generator, next);
ZEND_METHOD(Generator, send);
ZEND_METHOD(Generator, throw);
ZEND_METHOD(Generator, getReturn);


static const zend_function_entry class_Generator_methods[] = {
	ZEND_ME(Generator, rewind, arginfo_class_Generator_rewind, ZEND_ACC_PUBLIC)
	ZEND_ME(Generator, valid, arginfo_class_Generator_valid, ZEND_ACC_PUBLIC)
	ZEND_ME(Generator, current, arginfo_class_Generator_current, ZEND_ACC_PUBLIC)
	ZEND_ME(Generator, key, arginfo_class_Generator_key, ZEND_ACC_PUBLIC)
	ZEND_ME(Generator, next, arginfo_class_Generator_next, ZEND_ACC_PUBLIC)
	ZEND_ME(Generator, send, arginfo_class_Generator_send, ZEND_ACC_PUBLIC)
	ZEND_ME(Generator, throw, arginfo_class_Generator_throw, ZEND_ACC_PUBLIC)
	ZEND_ME(Generator, getReturn, arginfo_class_Generator_getReturn, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_ClosedGeneratorException_methods[] = {
	ZEND_FE_END
};
