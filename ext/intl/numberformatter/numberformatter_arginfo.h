/* This is a generated file, edit numberformatter.stub.php instead.
 * Stub hash: f99ec6e872e46e2274887cc608eb336417c52c78 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlNumberFormatter___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_IntlNumberFormatter_createFromSkeleton, 0, 2, IntlNumberFormatter, 1)
	ZEND_ARG_TYPE_INFO(0, skeleton, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_IntlNumberFormatter_format, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_MASK(0, value, MAY_BE_DOUBLE|MAY_BE_LONG|MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_IntlNumberFormatter_getErrorCode, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_IntlNumberFormatter_getErrorMessage, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_METHOD(IntlNumberFormatter, __construct);
ZEND_METHOD(IntlNumberFormatter, createFromSkeleton);
ZEND_METHOD(IntlNumberFormatter, format);
ZEND_METHOD(IntlNumberFormatter, getErrorCode);
ZEND_METHOD(IntlNumberFormatter, getErrorMessage);

static const zend_function_entry class_IntlNumberFormatter_methods[] = {
	ZEND_ME(IntlNumberFormatter, __construct, arginfo_class_IntlNumberFormatter___construct, ZEND_ACC_PRIVATE)
	ZEND_ME(IntlNumberFormatter, createFromSkeleton, arginfo_class_IntlNumberFormatter_createFromSkeleton, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(IntlNumberFormatter, format, arginfo_class_IntlNumberFormatter_format, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlNumberFormatter, getErrorCode, arginfo_class_IntlNumberFormatter_getErrorCode, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlNumberFormatter, getErrorMessage, arginfo_class_IntlNumberFormatter_getErrorMessage, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_IntlNumberFormatter(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "IntlNumberFormatter", class_IntlNumberFormatter_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	return class_entry;
}
