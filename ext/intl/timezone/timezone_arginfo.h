/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlTimeZone___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlTimeZone_countEquivalentIDs, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, zoneId, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_IntlTimeZone_createDefault arginfo_class_IntlTimeZone___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlTimeZone_createEnumeration, 0, 0, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, countryOrRawOffset, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_IntlTimeZone_createTimeZone arginfo_class_IntlTimeZone_countEquivalentIDs

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlTimeZone_createTimeZoneIDEnumeration, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, zoneType, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, region, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, rawOffset, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlTimeZone_fromDateTimeZone, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, zone, DateTimeZone, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlTimeZone_getCanonicalID, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, zoneId, IS_STRING, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, isSystemID, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_IntlTimeZone_getDisplayName, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, isDaylight, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, style, IS_LONG, 0, "IntlTimeZone::DISPLAY_LONG")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, locale, IS_STRING, 1, "null")
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
