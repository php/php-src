<?php

class IntlTimeZone
{
    private function __construct() {}

    /** @return int|false */
    public static function countEquivalentIDs(string $zoneId) {}

    /** @return IntlTimeZone */
    public static function createDefault() {}

    /**
     * @param IntlTimeZone|string|int|double|null $countryOrRawOffset
     * @return IntlIterator|false
     */
    public static function createEnumeration($countryOrRawOffset = null) {}

    /** @return ?IntlTimeZone */
    public static function createTimeZone(string $zoneId) {}

    /** @return IntlIterator|false */
    public static function createTimeZoneIDEnumeration(int $zoneType, ?string $region = null, ?int $rawOffset = null) {}

    /** @return ?IntlTimeZone */
    public static function fromDateTimeZone(DateTimeZone $zone) {}

    /** @return string|false */
    public static function getCanonicalID(string $zoneId, &$isSystemID = null) {}

    /** @return string|false */
    public function getDisplayName(bool $isDaylight = false, int $style = IntlTimeZone::DISPLAY_LONG, ?string $locale = null) {}

    /** @return int|false */
    public function getDSTSavings() {}

    /** @return string|false */
    public static function getEquivalentID(string $zoneId, int $index) {}

    /** @return int|false */
    public function getErrorCode() {}

    /** @return string|false */
    public function getErrorMessage() {}

    /** @return IntlTimeZone */
    public static function getGMT() {}

    /** @return string|false */
    public function getID() {}

    /** @return bool */
    public function getOffset(float $date, bool $local, &$rawOffset, &$dstOffset) {}

    /** @return int|false */
    public function getRawOffset() {}

    /** @return string|false */
    public static function getRegion(string $zoneId) {}

    /** @return string|false */
    public static function getTZDataVersion() {}

    /** @return IntlTimeZone */
    public static function getUnknown() {}

#if U_ICU_VERSION_MAJOR_NUM >= 52
    /** @return string|false */
    public static function getWindowsID(string $timezone) {}

    /** @return string|false */
    public static function getIDForWindowsID(string $timezone, string $region = UNKNOWN) {}
#endif
    /** @return bool */
    public function hasSameRules(IntlTimeZone $otherTimeZone) {}

    /** @return DateTimeZone|false */
    public function toDateTimeZone() {}

    /** @return bool */
    public function useDaylightTime() {}
}

function intltz_count_equivalent_ids(string $zoneId): int|false {}

function intltz_create_default(): IntlTimeZone {}

/**
 * @param IntlTimeZone|string|int|double|null $countryOrRawOffset
 * @return IntlIterator|false
 */
function intltz_create_enumeration($countryOrRawOffset = null) {}

function intltz_create_time_zone(string $zoneId): ?IntlTimeZone {}

function intltz_create_time_zone_id_enumeration(int $zoneType, ?string $region = null, ?int $rawOffset = null): IntlIterator|false {}

function intltz_from_date_time_zone(DateTimeZone $zone): ?IntlTimeZone {}

function intltz_get_canonical_id(string $zoneId, &$isSystemID = null): string|false {}

function intltz_get_display_name(IntlTimeZone $tz, bool $isDaylight = false, int $style = IntlTimeZone::DISPLAY_LONG, ?string $locale = null): string|false {}

function intltz_get_dst_savings(IntlTimeZone $tz): int|false {}

function intltz_get_equivalent_id(string $zoneId, int $index): string|false {}

function intltz_get_error_code(IntlTimeZone $tz): int|false {}

function intltz_get_error_message(IntlTimeZone $tz): string|false {}

function intltz_get_gmt(): IntlTimeZone {}

function intltz_get_id(IntlTimeZone $tz): string|false {}

function intltz_get_offset(IntlTimeZone $tz, float $date, bool $local, &$rawOffset, &$dstOffset): bool {}

function intltz_get_raw_offset(IntlTimeZone $tz): int|false {}

function intltz_get_region(string $zoneId): string|false {}

function intltz_get_tz_data_version(): string|false {}

function intltz_get_unknown(): IntlTimeZone {}

#if U_ICU_VERSION_MAJOR_NUM >= 52
function intltz_get_windows_id(string $timezone): string|false {}

function intltz_get_id_for_windows_id(string $timezone, string $region = UNKNOWN): string|false {}
#endif

function intltz_has_same_rules(IntlTimeZone $tz, IntlTimeZone $otherTimeZone): bool {}

function intltz_to_date_time_zone(IntlTimeZone $tz): DateTimeZone|false {}

function intltz_use_daylight_time(IntlTimeZone $tz): bool {}
