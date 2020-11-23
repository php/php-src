<?php

/** @generate-function-entries */

class IntlTimeZone
{
    private function __construct() {}

    /**
     * @return int|false
     * @alias intltz_count_equivalent_ids
     */
    public static function countEquivalentIDs(string $timezoneId) {}

    /**
     * @return IntlTimeZone
     * @alias intltz_create_default
     */
    public static function createDefault() {}

    /**
     * @param IntlTimeZone|string|int|float|null $countryOrRawOffset
     * @return IntlIterator|false
     * @alias intltz_create_enumeration
     */
    public static function createEnumeration($countryOrRawOffset = null) {}

    /**
     * @return IntlTimeZone|null
     * @alias intltz_create_time_zone
     */
    public static function createTimeZone(string $timezoneId) {}

    /**
     * @return IntlIterator|false
     * @alias intltz_create_time_zone_id_enumeration
     */
    public static function createTimeZoneIDEnumeration(int $type, ?string $region = null, ?int $rawOffset = null) {}

    /**
     * @return IntlTimeZone|null
     * @alias intltz_from_date_time_zone
     */
    public static function fromDateTimeZone(DateTimeZone $timezone) {}

    /**
     * @param bool $isSystemId
     * @return string|false
     * @alias intltz_get_canonical_id
     */
    public static function getCanonicalID(string $timezoneId, &$isSystemId = null) {}

    /**
     * @return string|false
     * @alias intltz_get_display_name
     */
    public function getDisplayName(bool $dst = false, int $style = IntlTimeZone::DISPLAY_LONG, ?string $locale = null) {}

    /**
     * @return int
     * @alias intltz_get_dst_savings
     */
    public function getDSTSavings() {}

    /**
     * @return string|false
     * @alias intltz_get_equivalent_id
     */
    public static function getEquivalentID(string $timezoneId, int $offset) {}

    /**
     * @return int|false
     * @alias intltz_get_error_code
     */
    public function getErrorCode() {}

    /**
     * @return string|false
     * @alias intltz_get_error_message
     */
    public function getErrorMessage() {}

    /**
     * @return IntlTimeZone
     * @alias intltz_get_gmt
     */
    public static function getGMT() {}

    /**
     * @return string|false
     * @alias intltz_get_id
     */
    public function getID() {}

    /**
     * @param int $rawOffset
     * @param int $dstOffset
     * @return bool
     * @alias intltz_get_offset
     */
    public function getOffset(float $timestamp, bool $local, &$rawOffset, &$dstOffset) {}

    /**
     * @return int
     * @alias intltz_get_raw_offset
     */
    public function getRawOffset() {}

    /**
     * @return string|false
     * @alias intltz_get_region
     */
    public static function getRegion(string $timezoneId) {}

    /**
     * @return string|false
     * @alias intltz_get_tz_data_version
     */
    public static function getTZDataVersion() {}

    /**
     * @return IntlTimeZone
     * @alias intltz_get_unknown
     */
    public static function getUnknown() {}

#if U_ICU_VERSION_MAJOR_NUM >= 52
    /**
     * @return string|false
     * @alias intltz_get_windows_id
     */
    public static function getWindowsID(string $timezoneId) {}

    /**
     * @return string|false
     * @alias intltz_get_id_for_windows_id
     */
    public static function getIDForWindowsID(string $timezoneId, ?string $region = null) {}
#endif
    /**
     * @return bool
     * @alias intltz_has_same_rules
     */
    public function hasSameRules(IntlTimeZone $other) {}

    /**
     * @return DateTimeZone|false
     * @alias intltz_to_date_time_zone
     */
    public function toDateTimeZone() {}

    /**
     * @return bool
     * @alias intltz_use_daylight_time
     */
    public function useDaylightTime() {}
}
