/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlTimeZone___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlTimeZone_countEquivalentIDs, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, zoneId, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlTimeZone_createDefault arginfo_class_IntlTimeZone___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlTimeZone_createEnumeration, 0, 0, 0)
	ZEND_ARG_INFO(0, countryOrRawOffset)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlTimeZone_createTimeZone arginfo_class_IntlTimeZone_countEquivalentIDs

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlTimeZone_createTimeZoneIDEnumeration, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, zoneType, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, region, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, rawOffset, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlTimeZone_fromDateTimeZone, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, zone, DateTimeZone, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlTimeZone_getCanonicalID, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, zoneId, IS_STRING, 0)
	ZEND_ARG_INFO(1, isSystemID)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlTimeZone_getDisplayName, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, isDaylight, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, style, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlTimeZone_getDSTSavings arginfo_class_IntlTimeZone___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlTimeZone_getEquivalentID, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, zoneId, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlTimeZone_getErrorCode arginfo_class_IntlTimeZone___construct

#define arginfo_class_IntlTimeZone_getErrorMessage arginfo_class_IntlTimeZone___construct

#define arginfo_class_IntlTimeZone_getGMT arginfo_class_IntlTimeZone___construct

#define arginfo_class_IntlTimeZone_getID arginfo_class_IntlTimeZone___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlTimeZone_getOffset, 0, 0, 4)
	ZEND_ARG_TYPE_INFO(0, date, IS_DOUBLE, 0)
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
	ZEND_ARG_TYPE_INFO(0, timezone, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if U_ICU_VERSION_MAJOR_NUM >= 52
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlTimeZone_getIDForWindowsID, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, timezone, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, region, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlTimeZone_hasSameRules, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, otherTimeZone, IntlTimeZone, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlTimeZone_toDateTimeZone arginfo_class_IntlTimeZone___construct

#define arginfo_class_IntlTimeZone_useDaylightTime arginfo_class_IntlTimeZone___construct

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intltz_count_equivalent_ids, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, zoneId, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_intltz_create_default, 0, 0, IntlTimeZone, 0)
ZEND_END_ARG_INFO()

#define arginfo_intltz_create_enumeration arginfo_class_IntlTimeZone_createEnumeration

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_intltz_create_time_zone, 0, 1, IntlTimeZone, 1)
	ZEND_ARG_TYPE_INFO(0, zoneId, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_intltz_create_time_zone_id_enumeration, 0, 1, IntlIterator, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, zoneType, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, region, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, rawOffset, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_intltz_from_date_time_zone, 0, 1, IntlTimeZone, 1)
	ZEND_ARG_OBJ_INFO(0, zone, DateTimeZone, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intltz_get_canonical_id, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, zoneId, IS_STRING, 0)
	ZEND_ARG_INFO(1, isSystemID)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intltz_get_display_name, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, tz, IntlTimeZone, 0)
	ZEND_ARG_TYPE_INFO(0, isDaylight, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, style, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intltz_get_dst_savings, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, tz, IntlTimeZone, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intltz_get_equivalent_id, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, zoneId, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_intltz_get_error_code arginfo_intltz_get_dst_savings

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intltz_get_error_message, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, tz, IntlTimeZone, 0)
ZEND_END_ARG_INFO()

#define arginfo_intltz_get_gmt arginfo_intltz_create_default

#define arginfo_intltz_get_id arginfo_intltz_get_error_message

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intltz_get_offset, 0, 5, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, tz, IntlTimeZone, 0)
	ZEND_ARG_TYPE_INFO(0, date, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, local, _IS_BOOL, 0)
	ZEND_ARG_INFO(1, rawOffset)
	ZEND_ARG_INFO(1, dstOffset)
ZEND_END_ARG_INFO()

#define arginfo_intltz_get_raw_offset arginfo_intltz_get_dst_savings

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intltz_get_region, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, zoneId, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intltz_get_tz_data_version, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

#define arginfo_intltz_get_unknown arginfo_intltz_create_default

#if U_ICU_VERSION_MAJOR_NUM >= 52
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intltz_get_windows_id, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, timezone, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if U_ICU_VERSION_MAJOR_NUM >= 52
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_intltz_get_id_for_windows_id, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, timezone, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, region, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intltz_has_same_rules, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, tz, IntlTimeZone, 0)
	ZEND_ARG_OBJ_INFO(0, otherTimeZone, IntlTimeZone, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_intltz_to_date_time_zone, 0, 1, DateTimeZone, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, tz, IntlTimeZone, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_intltz_use_daylight_time, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, tz, IntlTimeZone, 0)
ZEND_END_ARG_INFO()
