/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 449cea95304ac72bc3b1cea39358d33090f0c2c1 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlDateTimePatternGenerator___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, locale, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_IntlDateTimePatternGenerator_create, 0, 0, IntlDateTimePatternGenerator, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, locale, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_IntlDateTimePatternGenerator_getBestPattern, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, skeleton, IS_STRING, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(IntlDateTimePatternGenerator, __construct);
ZEND_FUNCTION(datetimepatterngenerator_create);
ZEND_FUNCTION(datetimepatterngenerator_get_best_pattern);


static const zend_function_entry class_IntlDateTimePatternGenerator_methods[] = {
	ZEND_ME(IntlDateTimePatternGenerator, __construct, arginfo_class_IntlDateTimePatternGenerator___construct, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(create, datetimepatterngenerator_create, arginfo_class_IntlDateTimePatternGenerator_create, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(getBestPattern, datetimepatterngenerator_get_best_pattern, arginfo_class_IntlDateTimePatternGenerator_getBestPattern, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_IntlDateTimePatternGenerator(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "IntlDateTimePatternGenerator", class_IntlDateTimePatternGenerator_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	return class_entry;
}
