/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 2befed14ddac05ad3a44e1614b828bc53fd986a0 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlTimeZone___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_IntlTimeZone_countEquivalentIDs, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, timezoneId, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_IntlTimeZone_createDefault, 0, 0, IntlTimeZone, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_IntlTimeZone_createEnumeration, 0, 0, IntlIterator, MAY_BE_FALSE)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, countryOrRawOffset, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_IntlTimeZone_createTimeZone, 0, 1, IntlTimeZone, 1)
	ZEND_ARG_TYPE_INFO(0, timezoneId, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_IntlTimeZone_createTimeZoneIDEnumeration, 0, 1, IntlIterator, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, region, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, rawOffset, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_IntlTimeZone_fromDateTimeZone, 0, 1, IntlTimeZone, 1)
	ZEND_ARG_OBJ_INFO(0, timezone, DateTimeZone, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_IntlTimeZone_getCanonicalID, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, timezoneId, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, isSystemId, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_IntlTimeZone_getDisplayName, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, dst, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, style, IS_LONG, 0, "IntlTimeZone::DISPLAY_LONG")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, locale, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlTimeZone_getDSTSavings, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_IntlTimeZone_getEquivalentID, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, timezoneId, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_IntlTimeZone_getErrorCode, 0, 0, MAY_BE_LONG|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_IntlTimeZone_getErrorMessage, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlTimeZone_getGMT arginfo_class_IntlTimeZone_createDefault

#define arginfo_class_IntlTimeZone_getID arginfo_class_IntlTimeZone_getErrorMessage

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlTimeZone_getOffset, 0, 4, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, local, _IS_BOOL, 0)
	ZEND_ARG_INFO(1, rawOffset)
	ZEND_ARG_INFO(1, dstOffset)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlTimeZone_getRawOffset arginfo_class_IntlTimeZone_getDSTSavings

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_IntlTimeZone_getRegion, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, timezoneId, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlTimeZone_getTZDataVersion arginfo_class_IntlTimeZone_getErrorMessage

#define arginfo_class_IntlTimeZone_getUnknown arginfo_class_IntlTimeZone_createDefault

#if U_ICU_VERSION_MAJOR_NUM >= 52
ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_IntlTimeZone_getWindowsID, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, timezoneId, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if U_ICU_VERSION_MAJOR_NUM >= 52
ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_IntlTimeZone_getIDForWindowsID, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, timezoneId, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, region, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlTimeZone_hasSameRules, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, other, IntlTimeZone, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_IntlTimeZone_toDateTimeZone, 0, 0, DateTimeZone, MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_IntlTimeZone_useDaylightTime, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(IntlTimeZone, __construct);
ZEND_FUNCTION(intltz_count_equivalent_ids);
ZEND_FUNCTION(intltz_create_default);
ZEND_FUNCTION(intltz_create_enumeration);
ZEND_FUNCTION(intltz_create_time_zone);
ZEND_FUNCTION(intltz_create_time_zone_id_enumeration);
ZEND_FUNCTION(intltz_from_date_time_zone);
ZEND_FUNCTION(intltz_get_canonical_id);
ZEND_FUNCTION(intltz_get_display_name);
ZEND_FUNCTION(intltz_get_dst_savings);
ZEND_FUNCTION(intltz_get_equivalent_id);
ZEND_FUNCTION(intltz_get_error_code);
ZEND_FUNCTION(intltz_get_error_message);
ZEND_FUNCTION(intltz_get_gmt);
ZEND_FUNCTION(intltz_get_id);
ZEND_FUNCTION(intltz_get_offset);
ZEND_FUNCTION(intltz_get_raw_offset);
ZEND_FUNCTION(intltz_get_region);
ZEND_FUNCTION(intltz_get_tz_data_version);
ZEND_FUNCTION(intltz_get_unknown);
#if U_ICU_VERSION_MAJOR_NUM >= 52
ZEND_FUNCTION(intltz_get_windows_id);
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 52
ZEND_FUNCTION(intltz_get_id_for_windows_id);
#endif
ZEND_FUNCTION(intltz_has_same_rules);
ZEND_FUNCTION(intltz_to_date_time_zone);
ZEND_FUNCTION(intltz_use_daylight_time);


static const zend_function_entry class_IntlTimeZone_methods[] = {
	ZEND_ME(IntlTimeZone, __construct, arginfo_class_IntlTimeZone___construct, ZEND_ACC_PRIVATE)
	ZEND_ME_MAPPING(countEquivalentIDs, intltz_count_equivalent_ids, arginfo_class_IntlTimeZone_countEquivalentIDs, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(createDefault, intltz_create_default, arginfo_class_IntlTimeZone_createDefault, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(createEnumeration, intltz_create_enumeration, arginfo_class_IntlTimeZone_createEnumeration, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(createTimeZone, intltz_create_time_zone, arginfo_class_IntlTimeZone_createTimeZone, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(createTimeZoneIDEnumeration, intltz_create_time_zone_id_enumeration, arginfo_class_IntlTimeZone_createTimeZoneIDEnumeration, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(fromDateTimeZone, intltz_from_date_time_zone, arginfo_class_IntlTimeZone_fromDateTimeZone, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(getCanonicalID, intltz_get_canonical_id, arginfo_class_IntlTimeZone_getCanonicalID, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(getDisplayName, intltz_get_display_name, arginfo_class_IntlTimeZone_getDisplayName, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getDSTSavings, intltz_get_dst_savings, arginfo_class_IntlTimeZone_getDSTSavings, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getEquivalentID, intltz_get_equivalent_id, arginfo_class_IntlTimeZone_getEquivalentID, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(getErrorCode, intltz_get_error_code, arginfo_class_IntlTimeZone_getErrorCode, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getErrorMessage, intltz_get_error_message, arginfo_class_IntlTimeZone_getErrorMessage, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getGMT, intltz_get_gmt, arginfo_class_IntlTimeZone_getGMT, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(getID, intltz_get_id, arginfo_class_IntlTimeZone_getID, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getOffset, intltz_get_offset, arginfo_class_IntlTimeZone_getOffset, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getRawOffset, intltz_get_raw_offset, arginfo_class_IntlTimeZone_getRawOffset, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getRegion, intltz_get_region, arginfo_class_IntlTimeZone_getRegion, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(getTZDataVersion, intltz_get_tz_data_version, arginfo_class_IntlTimeZone_getTZDataVersion, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME_MAPPING(getUnknown, intltz_get_unknown, arginfo_class_IntlTimeZone_getUnknown, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
#if U_ICU_VERSION_MAJOR_NUM >= 52
	ZEND_ME_MAPPING(getWindowsID, intltz_get_windows_id, arginfo_class_IntlTimeZone_getWindowsID, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
#endif
#if U_ICU_VERSION_MAJOR_NUM >= 52
	ZEND_ME_MAPPING(getIDForWindowsID, intltz_get_id_for_windows_id, arginfo_class_IntlTimeZone_getIDForWindowsID, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
#endif
	ZEND_ME_MAPPING(hasSameRules, intltz_has_same_rules, arginfo_class_IntlTimeZone_hasSameRules, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(toDateTimeZone, intltz_to_date_time_zone, arginfo_class_IntlTimeZone_toDateTimeZone, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(useDaylightTime, intltz_use_daylight_time, arginfo_class_IntlTimeZone_useDaylightTime, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_IntlTimeZone(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "IntlTimeZone", class_IntlTimeZone_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;

	zval const_DISPLAY_SHORT_value;
	ZVAL_LONG(&const_DISPLAY_SHORT_value, TimeZone::SHORT);
	zend_string *const_DISPLAY_SHORT_name = zend_string_init_interned("DISPLAY_SHORT", sizeof("DISPLAY_SHORT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DISPLAY_SHORT_name, &const_DISPLAY_SHORT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DISPLAY_SHORT_name);

	zval const_DISPLAY_LONG_value;
	ZVAL_LONG(&const_DISPLAY_LONG_value, TimeZone::LONG);
	zend_string *const_DISPLAY_LONG_name = zend_string_init_interned("DISPLAY_LONG", sizeof("DISPLAY_LONG") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DISPLAY_LONG_name, &const_DISPLAY_LONG_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DISPLAY_LONG_name);

	zval const_DISPLAY_SHORT_GENERIC_value;
	ZVAL_LONG(&const_DISPLAY_SHORT_GENERIC_value, TimeZone::SHORT_GENERIC);
	zend_string *const_DISPLAY_SHORT_GENERIC_name = zend_string_init_interned("DISPLAY_SHORT_GENERIC", sizeof("DISPLAY_SHORT_GENERIC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DISPLAY_SHORT_GENERIC_name, &const_DISPLAY_SHORT_GENERIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DISPLAY_SHORT_GENERIC_name);

	zval const_DISPLAY_LONG_GENERIC_value;
	ZVAL_LONG(&const_DISPLAY_LONG_GENERIC_value, TimeZone::LONG_GENERIC);
	zend_string *const_DISPLAY_LONG_GENERIC_name = zend_string_init_interned("DISPLAY_LONG_GENERIC", sizeof("DISPLAY_LONG_GENERIC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DISPLAY_LONG_GENERIC_name, &const_DISPLAY_LONG_GENERIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DISPLAY_LONG_GENERIC_name);

	zval const_DISPLAY_SHORT_GMT_value;
	ZVAL_LONG(&const_DISPLAY_SHORT_GMT_value, TimeZone::SHORT_GMT);
	zend_string *const_DISPLAY_SHORT_GMT_name = zend_string_init_interned("DISPLAY_SHORT_GMT", sizeof("DISPLAY_SHORT_GMT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DISPLAY_SHORT_GMT_name, &const_DISPLAY_SHORT_GMT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DISPLAY_SHORT_GMT_name);

	zval const_DISPLAY_LONG_GMT_value;
	ZVAL_LONG(&const_DISPLAY_LONG_GMT_value, TimeZone::LONG_GMT);
	zend_string *const_DISPLAY_LONG_GMT_name = zend_string_init_interned("DISPLAY_LONG_GMT", sizeof("DISPLAY_LONG_GMT") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DISPLAY_LONG_GMT_name, &const_DISPLAY_LONG_GMT_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DISPLAY_LONG_GMT_name);

	zval const_DISPLAY_SHORT_COMMONLY_USED_value;
	ZVAL_LONG(&const_DISPLAY_SHORT_COMMONLY_USED_value, TimeZone::SHORT_COMMONLY_USED);
	zend_string *const_DISPLAY_SHORT_COMMONLY_USED_name = zend_string_init_interned("DISPLAY_SHORT_COMMONLY_USED", sizeof("DISPLAY_SHORT_COMMONLY_USED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DISPLAY_SHORT_COMMONLY_USED_name, &const_DISPLAY_SHORT_COMMONLY_USED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DISPLAY_SHORT_COMMONLY_USED_name);

	zval const_DISPLAY_GENERIC_LOCATION_value;
	ZVAL_LONG(&const_DISPLAY_GENERIC_LOCATION_value, TimeZone::GENERIC_LOCATION);
	zend_string *const_DISPLAY_GENERIC_LOCATION_name = zend_string_init_interned("DISPLAY_GENERIC_LOCATION", sizeof("DISPLAY_GENERIC_LOCATION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DISPLAY_GENERIC_LOCATION_name, &const_DISPLAY_GENERIC_LOCATION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DISPLAY_GENERIC_LOCATION_name);

	zval const_TYPE_ANY_value;
	ZVAL_LONG(&const_TYPE_ANY_value, UCAL_ZONE_TYPE_ANY);
	zend_string *const_TYPE_ANY_name = zend_string_init_interned("TYPE_ANY", sizeof("TYPE_ANY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TYPE_ANY_name, &const_TYPE_ANY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TYPE_ANY_name);

	zval const_TYPE_CANONICAL_value;
	ZVAL_LONG(&const_TYPE_CANONICAL_value, UCAL_ZONE_TYPE_CANONICAL);
	zend_string *const_TYPE_CANONICAL_name = zend_string_init_interned("TYPE_CANONICAL", sizeof("TYPE_CANONICAL") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TYPE_CANONICAL_name, &const_TYPE_CANONICAL_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TYPE_CANONICAL_name);

	zval const_TYPE_CANONICAL_LOCATION_value;
	ZVAL_LONG(&const_TYPE_CANONICAL_LOCATION_value, UCAL_ZONE_TYPE_CANONICAL_LOCATION);
	zend_string *const_TYPE_CANONICAL_LOCATION_name = zend_string_init_interned("TYPE_CANONICAL_LOCATION", sizeof("TYPE_CANONICAL_LOCATION") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_TYPE_CANONICAL_LOCATION_name, &const_TYPE_CANONICAL_LOCATION_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_TYPE_CANONICAL_LOCATION_name);

	return class_entry;
}
