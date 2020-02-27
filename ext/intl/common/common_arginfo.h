/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlIterator_current, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlIterator_key arginfo_class_IntlIterator_current

#define arginfo_class_IntlIterator_next arginfo_class_IntlIterator_current

#define arginfo_class_IntlIterator_rewind arginfo_class_IntlIterator_current

#define arginfo_class_IntlIterator_valid arginfo_class_IntlIterator_current

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intl_get_error_code, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intl_get_error_message, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intl_is_failure, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, error_code, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intl_error_name, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, error_code, IS_LONG, 0)
ZEND_END_ARG_INFO()
