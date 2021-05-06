/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: b48d5f26a4f0663a06a274aeba1cfca2e0e80f91 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_php_user_filter_filter, 0, 0, 4)
	ZEND_ARG_INFO(0, in)
	ZEND_ARG_INFO(0, out)
	ZEND_ARG_INFO(1, consumed)
	ZEND_ARG_TYPE_INFO(0, closing, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_php_user_filter_onCreate, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_php_user_filter_onClose arginfo_class_php_user_filter_onCreate


ZEND_METHOD(php_user_filter, filter);
ZEND_METHOD(php_user_filter, onCreate);
ZEND_METHOD(php_user_filter, onClose);


static const zend_function_entry class_php_user_filter_methods[] = {
	ZEND_ME(php_user_filter, filter, arginfo_class_php_user_filter_filter, ZEND_ACC_PUBLIC)
	ZEND_ME(php_user_filter, onCreate, arginfo_class_php_user_filter_onCreate, ZEND_ACC_PUBLIC)
	ZEND_ME(php_user_filter, onClose, arginfo_class_php_user_filter_onClose, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
