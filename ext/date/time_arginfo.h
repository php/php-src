/* This is a generated file, edit time.stub.php instead.
 * Stub hash: b145db05ac54d90df4a1d5eb139c2d0c5e70401c */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Time_Duration___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Time_Duration_fromSeconds, 0, 1, Time\\Duration, 0)
	ZEND_ARG_TYPE_INFO(0, seconds, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, nanoseconds, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Time_Duration_fromNanoseconds, 0, 1, Time\\Duration, 0)
	ZEND_ARG_TYPE_INFO(0, nanoseconds, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Time_Duration_fromMicroseconds, 0, 1, Time\\Duration, 0)
	ZEND_ARG_TYPE_INFO(0, microseconds, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Time_Duration_fromMilliseconds, 0, 1, Time\\Duration, 0)
	ZEND_ARG_TYPE_INFO(0, milliseconds, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Time_Duration_fromMinutes, 0, 1, Time\\Duration, 0)
	ZEND_ARG_TYPE_INFO(0, minutes, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Time_Duration_fromHours, 0, 1, Time\\Duration, 0)
	ZEND_ARG_TYPE_INFO(0, hours, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Time_Duration_fromIso8601DurationString, 0, 1, Time\\Duration, 0)
	ZEND_ARG_TYPE_INFO(0, specification, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Time_Duration_negate, 0, 0, Time\\Duration, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Time_Duration_absolute arginfo_class_Time_Duration_negate

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Time_Duration_add, 0, 1, Time\\Duration, 0)
	ZEND_ARG_OBJ_INFO(0, duration, Time\\Duration, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Time_Duration_sub arginfo_class_Time_Duration_add

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Time_Duration_multiplyBy, 0, 1, Time\\Duration, 0)
	ZEND_ARG_TYPE_INFO(0, factor, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Time_Duration_divideBy, 0, 1, Time\\Duration, 0)
	ZEND_ARG_TYPE_INFO(0, divisor, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Time_Duration_compare, 0, 2, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, a, Time\\Duration, 0)
	ZEND_ARG_OBJ_INFO(0, b, Time\\Duration, 0)
ZEND_END_ARG_INFO()

ZEND_METHOD(Time_Duration, __construct);
ZEND_METHOD(Time_Duration, fromSeconds);
ZEND_METHOD(Time_Duration, fromNanoseconds);
ZEND_METHOD(Time_Duration, fromMicroseconds);
ZEND_METHOD(Time_Duration, fromMilliseconds);
ZEND_METHOD(Time_Duration, fromMinutes);
ZEND_METHOD(Time_Duration, fromHours);
ZEND_METHOD(Time_Duration, fromIso8601DurationString);
ZEND_METHOD(Time_Duration, negate);
ZEND_METHOD(Time_Duration, absolute);
ZEND_METHOD(Time_Duration, add);
ZEND_METHOD(Time_Duration, sub);
ZEND_METHOD(Time_Duration, multiplyBy);
ZEND_METHOD(Time_Duration, divideBy);
ZEND_METHOD(Time_Duration, compare);

static const zend_function_entry class_Time_Duration_methods[] = {
	ZEND_ME(Time_Duration, __construct, arginfo_class_Time_Duration___construct, ZEND_ACC_PRIVATE)
	ZEND_ME(Time_Duration, fromSeconds, arginfo_class_Time_Duration_fromSeconds, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Time_Duration, fromNanoseconds, arginfo_class_Time_Duration_fromNanoseconds, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Time_Duration, fromMicroseconds, arginfo_class_Time_Duration_fromMicroseconds, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Time_Duration, fromMilliseconds, arginfo_class_Time_Duration_fromMilliseconds, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Time_Duration, fromMinutes, arginfo_class_Time_Duration_fromMinutes, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Time_Duration, fromHours, arginfo_class_Time_Duration_fromHours, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Time_Duration, fromIso8601DurationString, arginfo_class_Time_Duration_fromIso8601DurationString, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Time_Duration, negate, arginfo_class_Time_Duration_negate, ZEND_ACC_PUBLIC)
	ZEND_ME(Time_Duration, absolute, arginfo_class_Time_Duration_absolute, ZEND_ACC_PUBLIC)
	ZEND_ME(Time_Duration, add, arginfo_class_Time_Duration_add, ZEND_ACC_PUBLIC)
	ZEND_ME(Time_Duration, sub, arginfo_class_Time_Duration_sub, ZEND_ACC_PUBLIC)
	ZEND_ME(Time_Duration, multiplyBy, arginfo_class_Time_Duration_multiplyBy, ZEND_ACC_PUBLIC)
	ZEND_ME(Time_Duration, divideBy, arginfo_class_Time_Duration_divideBy, ZEND_ACC_PUBLIC)
	ZEND_ME(Time_Duration, compare, arginfo_class_Time_Duration_compare, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Time_TimeException(zend_class_entry *class_entry_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Time", "TimeException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Exception, ZEND_ACC_NO_DYNAMIC_PROPERTIES);

	return class_entry;
}

static zend_class_entry *register_class_Time_Duration(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Time", "Duration", class_Time_Duration_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_READONLY_CLASS);

	zval property_seconds_default_value;
	ZVAL_UNDEF(&property_seconds_default_value);
	zend_string *property_seconds_name = zend_string_init("seconds", sizeof("seconds") - 1, true);
	zend_declare_typed_property(class_entry, property_seconds_name, &property_seconds_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(property_seconds_name, true);

	zval property_nanoseconds_default_value;
	ZVAL_UNDEF(&property_nanoseconds_default_value);
	zend_string *property_nanoseconds_name = zend_string_init("nanoseconds", sizeof("nanoseconds") - 1, true);
	zend_declare_typed_property(class_entry, property_nanoseconds_name, &property_nanoseconds_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(property_nanoseconds_name, true);

	zval property_negative_default_value;
	ZVAL_UNDEF(&property_negative_default_value);
	zend_string *property_negative_name = zend_string_init("negative", sizeof("negative") - 1, true);
	zend_declare_typed_property(class_entry, property_negative_name, &property_negative_default_value, ZEND_ACC_PUBLIC|ZEND_ACC_READONLY, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release_ex(property_negative_name, true);

	return class_entry;
}
