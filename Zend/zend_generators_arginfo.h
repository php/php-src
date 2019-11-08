/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Generator_rewind, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Generator_valid, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Generator_current, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Generator_key arginfo_class_Generator_current

#define arginfo_class_Generator_next arginfo_class_Generator_rewind

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Generator_send, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Generator_throw, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, exception, Throwable, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Generator_getReturn arginfo_class_Generator_current
