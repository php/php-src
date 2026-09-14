/* This is a generated file, edit reldateformatter.stub.php instead.
 * Stub hash: 5b6e0801001e79ff1d1e9c0546109016d6479351 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlRelativeDateTimeFormatter___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, locale, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, style, IS_LONG, 0, "IntlRelativeDateTimeFormatter::STYLE_LONG")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, capitalizationContext, IS_LONG, 0, "IntlRelativeDateTimeFormatter::CAPITALIZATION_NONE")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, numberFormatter, NumberFormatter, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_IntlRelativeDateTimeFormatter_format, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_MASK(0, offset, MAY_BE_LONG|MAY_BE_DOUBLE, NULL)
	ZEND_ARG_TYPE_INFO(0, unit, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlRelativeDateTimeFormatter_formatNumeric arginfo_class_IntlRelativeDateTimeFormatter_format

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_IntlRelativeDateTimeFormatter_combineDateAndTime, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, relativeDate, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, time, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_IntlRelativeDateTimeFormatter_getErrorCode, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_IntlRelativeDateTimeFormatter_getErrorMessage, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_METHOD(IntlRelativeDateTimeFormatter, __construct);
ZEND_METHOD(IntlRelativeDateTimeFormatter, format);
ZEND_METHOD(IntlRelativeDateTimeFormatter, formatNumeric);
ZEND_METHOD(IntlRelativeDateTimeFormatter, combineDateAndTime);
ZEND_METHOD(IntlRelativeDateTimeFormatter, getErrorCode);
ZEND_METHOD(IntlRelativeDateTimeFormatter, getErrorMessage);

static const zend_function_entry class_IntlRelativeDateTimeFormatter_methods[] = {
	ZEND_ME(IntlRelativeDateTimeFormatter, __construct, arginfo_class_IntlRelativeDateTimeFormatter___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlRelativeDateTimeFormatter, format, arginfo_class_IntlRelativeDateTimeFormatter_format, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlRelativeDateTimeFormatter, formatNumeric, arginfo_class_IntlRelativeDateTimeFormatter_formatNumeric, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlRelativeDateTimeFormatter, combineDateAndTime, arginfo_class_IntlRelativeDateTimeFormatter_combineDateAndTime, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlRelativeDateTimeFormatter, getErrorCode, arginfo_class_IntlRelativeDateTimeFormatter_getErrorCode, ZEND_ACC_PUBLIC)
	ZEND_ME(IntlRelativeDateTimeFormatter, getErrorMessage, arginfo_class_IntlRelativeDateTimeFormatter_getErrorMessage, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_IntlRelativeDateTimeFormatter(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "IntlRelativeDateTimeFormatter", class_IntlRelativeDateTimeFormatter_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	zval const_STYLE_LONG_value;
	ZVAL_LONG(&const_STYLE_LONG_value, UDAT_STYLE_LONG);
	zend_string *const_STYLE_LONG_name = zend_string_init_interned("STYLE_LONG", sizeof("STYLE_LONG") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_STYLE_LONG_name, &const_STYLE_LONG_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_STYLE_LONG_name, true);

	zval const_STYLE_SHORT_value;
	ZVAL_LONG(&const_STYLE_SHORT_value, UDAT_STYLE_SHORT);
	zend_string *const_STYLE_SHORT_name = zend_string_init_interned("STYLE_SHORT", sizeof("STYLE_SHORT") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_STYLE_SHORT_name, &const_STYLE_SHORT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_STYLE_SHORT_name, true);

	zval const_STYLE_NARROW_value;
	ZVAL_LONG(&const_STYLE_NARROW_value, UDAT_STYLE_NARROW);
	zend_string *const_STYLE_NARROW_name = zend_string_init_interned("STYLE_NARROW", sizeof("STYLE_NARROW") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_STYLE_NARROW_name, &const_STYLE_NARROW_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_STYLE_NARROW_name, true);

	zval const_CAPITALIZATION_NONE_value;
	ZVAL_LONG(&const_CAPITALIZATION_NONE_value, UDISPCTX_CAPITALIZATION_NONE);
	zend_string *const_CAPITALIZATION_NONE_name = zend_string_init_interned("CAPITALIZATION_NONE", sizeof("CAPITALIZATION_NONE") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CAPITALIZATION_NONE_name, &const_CAPITALIZATION_NONE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CAPITALIZATION_NONE_name, true);

	zval const_CAPITALIZATION_FOR_MIDDLE_OF_SENTENCE_value;
	ZVAL_LONG(&const_CAPITALIZATION_FOR_MIDDLE_OF_SENTENCE_value, UDISPCTX_CAPITALIZATION_FOR_MIDDLE_OF_SENTENCE);
	zend_string *const_CAPITALIZATION_FOR_MIDDLE_OF_SENTENCE_name = zend_string_init_interned("CAPITALIZATION_FOR_MIDDLE_OF_SENTENCE", sizeof("CAPITALIZATION_FOR_MIDDLE_OF_SENTENCE") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CAPITALIZATION_FOR_MIDDLE_OF_SENTENCE_name, &const_CAPITALIZATION_FOR_MIDDLE_OF_SENTENCE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CAPITALIZATION_FOR_MIDDLE_OF_SENTENCE_name, true);

	zval const_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE_value;
	ZVAL_LONG(&const_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE_value, UDISPCTX_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE);
	zend_string *const_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE_name = zend_string_init_interned("CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE", sizeof("CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE_name, &const_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE_name, true);

	zval const_CAPITALIZATION_FOR_UI_LIST_OR_MENU_value;
	ZVAL_LONG(&const_CAPITALIZATION_FOR_UI_LIST_OR_MENU_value, UDISPCTX_CAPITALIZATION_FOR_UI_LIST_OR_MENU);
	zend_string *const_CAPITALIZATION_FOR_UI_LIST_OR_MENU_name = zend_string_init_interned("CAPITALIZATION_FOR_UI_LIST_OR_MENU", sizeof("CAPITALIZATION_FOR_UI_LIST_OR_MENU") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CAPITALIZATION_FOR_UI_LIST_OR_MENU_name, &const_CAPITALIZATION_FOR_UI_LIST_OR_MENU_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CAPITALIZATION_FOR_UI_LIST_OR_MENU_name, true);

	zval const_CAPITALIZATION_FOR_STANDALONE_value;
	ZVAL_LONG(&const_CAPITALIZATION_FOR_STANDALONE_value, UDISPCTX_CAPITALIZATION_FOR_STANDALONE);
	zend_string *const_CAPITALIZATION_FOR_STANDALONE_name = zend_string_init_interned("CAPITALIZATION_FOR_STANDALONE", sizeof("CAPITALIZATION_FOR_STANDALONE") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_CAPITALIZATION_FOR_STANDALONE_name, &const_CAPITALIZATION_FOR_STANDALONE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_CAPITALIZATION_FOR_STANDALONE_name, true);

	zval const_UNIT_YEAR_value;
	ZVAL_LONG(&const_UNIT_YEAR_value, UDAT_REL_UNIT_YEAR);
	zend_string *const_UNIT_YEAR_name = zend_string_init_interned("UNIT_YEAR", sizeof("UNIT_YEAR") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_UNIT_YEAR_name, &const_UNIT_YEAR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_UNIT_YEAR_name, true);

	zval const_UNIT_QUARTER_value;
	ZVAL_LONG(&const_UNIT_QUARTER_value, UDAT_REL_UNIT_QUARTER);
	zend_string *const_UNIT_QUARTER_name = zend_string_init_interned("UNIT_QUARTER", sizeof("UNIT_QUARTER") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_UNIT_QUARTER_name, &const_UNIT_QUARTER_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_UNIT_QUARTER_name, true);

	zval const_UNIT_MONTH_value;
	ZVAL_LONG(&const_UNIT_MONTH_value, UDAT_REL_UNIT_MONTH);
	zend_string *const_UNIT_MONTH_name = zend_string_init_interned("UNIT_MONTH", sizeof("UNIT_MONTH") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_UNIT_MONTH_name, &const_UNIT_MONTH_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_UNIT_MONTH_name, true);

	zval const_UNIT_WEEK_value;
	ZVAL_LONG(&const_UNIT_WEEK_value, UDAT_REL_UNIT_WEEK);
	zend_string *const_UNIT_WEEK_name = zend_string_init_interned("UNIT_WEEK", sizeof("UNIT_WEEK") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_UNIT_WEEK_name, &const_UNIT_WEEK_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_UNIT_WEEK_name, true);

	zval const_UNIT_DAY_value;
	ZVAL_LONG(&const_UNIT_DAY_value, UDAT_REL_UNIT_DAY);
	zend_string *const_UNIT_DAY_name = zend_string_init_interned("UNIT_DAY", sizeof("UNIT_DAY") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_UNIT_DAY_name, &const_UNIT_DAY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_UNIT_DAY_name, true);

	zval const_UNIT_HOUR_value;
	ZVAL_LONG(&const_UNIT_HOUR_value, UDAT_REL_UNIT_HOUR);
	zend_string *const_UNIT_HOUR_name = zend_string_init_interned("UNIT_HOUR", sizeof("UNIT_HOUR") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_UNIT_HOUR_name, &const_UNIT_HOUR_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_UNIT_HOUR_name, true);

	zval const_UNIT_MINUTE_value;
	ZVAL_LONG(&const_UNIT_MINUTE_value, UDAT_REL_UNIT_MINUTE);
	zend_string *const_UNIT_MINUTE_name = zend_string_init_interned("UNIT_MINUTE", sizeof("UNIT_MINUTE") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_UNIT_MINUTE_name, &const_UNIT_MINUTE_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_UNIT_MINUTE_name, true);

	zval const_UNIT_SECOND_value;
	ZVAL_LONG(&const_UNIT_SECOND_value, UDAT_REL_UNIT_SECOND);
	zend_string *const_UNIT_SECOND_name = zend_string_init_interned("UNIT_SECOND", sizeof("UNIT_SECOND") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_UNIT_SECOND_name, &const_UNIT_SECOND_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_UNIT_SECOND_name, true);

	zval const_UNIT_SUNDAY_value;
	ZVAL_LONG(&const_UNIT_SUNDAY_value, UDAT_REL_UNIT_SUNDAY);
	zend_string *const_UNIT_SUNDAY_name = zend_string_init_interned("UNIT_SUNDAY", sizeof("UNIT_SUNDAY") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_UNIT_SUNDAY_name, &const_UNIT_SUNDAY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_UNIT_SUNDAY_name, true);

	zval const_UNIT_MONDAY_value;
	ZVAL_LONG(&const_UNIT_MONDAY_value, UDAT_REL_UNIT_MONDAY);
	zend_string *const_UNIT_MONDAY_name = zend_string_init_interned("UNIT_MONDAY", sizeof("UNIT_MONDAY") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_UNIT_MONDAY_name, &const_UNIT_MONDAY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_UNIT_MONDAY_name, true);

	zval const_UNIT_TUESDAY_value;
	ZVAL_LONG(&const_UNIT_TUESDAY_value, UDAT_REL_UNIT_TUESDAY);
	zend_string *const_UNIT_TUESDAY_name = zend_string_init_interned("UNIT_TUESDAY", sizeof("UNIT_TUESDAY") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_UNIT_TUESDAY_name, &const_UNIT_TUESDAY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_UNIT_TUESDAY_name, true);

	zval const_UNIT_WEDNESDAY_value;
	ZVAL_LONG(&const_UNIT_WEDNESDAY_value, UDAT_REL_UNIT_WEDNESDAY);
	zend_string *const_UNIT_WEDNESDAY_name = zend_string_init_interned("UNIT_WEDNESDAY", sizeof("UNIT_WEDNESDAY") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_UNIT_WEDNESDAY_name, &const_UNIT_WEDNESDAY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_UNIT_WEDNESDAY_name, true);

	zval const_UNIT_THURSDAY_value;
	ZVAL_LONG(&const_UNIT_THURSDAY_value, UDAT_REL_UNIT_THURSDAY);
	zend_string *const_UNIT_THURSDAY_name = zend_string_init_interned("UNIT_THURSDAY", sizeof("UNIT_THURSDAY") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_UNIT_THURSDAY_name, &const_UNIT_THURSDAY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_UNIT_THURSDAY_name, true);

	zval const_UNIT_FRIDAY_value;
	ZVAL_LONG(&const_UNIT_FRIDAY_value, UDAT_REL_UNIT_FRIDAY);
	zend_string *const_UNIT_FRIDAY_name = zend_string_init_interned("UNIT_FRIDAY", sizeof("UNIT_FRIDAY") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_UNIT_FRIDAY_name, &const_UNIT_FRIDAY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_UNIT_FRIDAY_name, true);

	zval const_UNIT_SATURDAY_value;
	ZVAL_LONG(&const_UNIT_SATURDAY_value, UDAT_REL_UNIT_SATURDAY);
	zend_string *const_UNIT_SATURDAY_name = zend_string_init_interned("UNIT_SATURDAY", sizeof("UNIT_SATURDAY") - 1, true);
	zend_declare_typed_class_constant(class_entry, const_UNIT_SATURDAY_name, &const_UNIT_SATURDAY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release_ex(const_UNIT_SATURDAY_name, true);

	return class_entry;
}
