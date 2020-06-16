/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_php_user_filter_filter, 0, 0, 4)
	ZEND_ARG_INFO(0, in)
	ZEND_ARG_INFO(0, out)
	ZEND_ARG_INFO(1, consumed)
	ZEND_ARG_INFO(0, closing)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_php_user_filter_onCreate, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_php_user_filter_onClose arginfo_class_php_user_filter_onCreate


ZEND_FUNCTION(user_filter_nop);


static const zend_function_entry class_php_user_filter_methods[] = {
	ZEND_ME_MAPPING(filter, user_filter_nop, arginfo_class_php_user_filter_filter, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(onCreate, user_filter_nop, arginfo_class_php_user_filter_onCreate, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(onClose, user_filter_nop, arginfo_class_php_user_filter_onClose, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
