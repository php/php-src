/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: afd0e74b29d54cde9789787b924af9b43539a7f4 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlTimeZone___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlTimeZone_countEquivalentIDs, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, timezoneId, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlTimeZone_createDefault arginfo_class_IntlTimeZone___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlTimeZone_createEnumeration, 0, 0, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, countryOrRawOffset, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_IntlTimeZone_createTimeZone arginfo_class_IntlTimeZone_countEquivalentIDs

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlTimeZone_createTimeZoneIDEnumeration, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, region, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, rawOffset, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlTimeZone_fromDateTimeZone, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, timezone, DateTimeZone, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlTimeZone_getCanonicalID, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, timezoneId, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, isSystemId, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlTimeZone_getDisplayName, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, dst, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, style, IS_LONG, 0, "IntlTimeZone::DISPLAY_LONG")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, locale, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_IntlTimeZone_getDSTSavings arginfo_class_IntlTimeZone___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlTimeZone_getEquivalentID, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, timezoneId, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlTimeZone_getErrorCode arginfo_class_IntlTimeZone___construct

#define arginfo_class_IntlTimeZone_getErrorMessage arginfo_class_IntlTimeZone___construct

#define arginfo_class_IntlTimeZone_getGMT arginfo_class_IntlTimeZone___construct

#define arginfo_class_IntlTimeZone_getID arginfo_class_IntlTimeZone___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlTimeZone_getOffset, 0, 0, 4)
	ZEND_ARG_TYPE_INFO(0, timestamp, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, local, _IS_BOOL, 0)
	ZEND_ARG_INFO(1, rawOffset)
	ZEND_ARG_INFO(1, dstOffset)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlTimeZone_getRawOffset arginfo_class_IntlTimeZone___construct

#define arginfo_class_IntlTimeZone_getRegion arginfo_class_IntlTimeZone_countEquivalentIDs

#define arginfo_class_IntlTimeZone_getTZDataVersion arginfo_class_IntlTimeZone___construct

#define arginfo_class_IntlTimeZone_getUnknown arginfo_class_IntlTimeZone___construct

#if U_ICU_VERSION_MAJOR_NUM >= 52
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlTimeZone_getWindowsID, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, timezoneId, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if U_ICU_VERSION_MAJOR_NUM >= 52
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlTimeZone_getIDForWindowsID, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, timezoneId, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, region, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlTimeZone_hasSameRules, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, other, IntlTimeZone, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlTimeZone_toDateTimeZone arginfo_class_IntlTimeZone___construct

#define arginfo_class_IntlTimeZone_useDaylightTime arginfo_class_IntlTimeZone___construct


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
