<?php

/** @generate-class-entries */

/** @not-serializable */
class IntlTimeZone
{
    /**
     * @var int
     * @cvalue TimeZone::SHORT
     */
    public const DISPLAY_SHORT = UNKNOWN;
    /**
     * @var int
     * @cvalue TimeZone::LONG
     */
    public const DISPLAY_LONG = UNKNOWN;

    /**
     * @var int
     * @cvalue TimeZone::SHORT_GENERIC
     */
    public const DISPLAY_SHORT_GENERIC = UNKNOWN;
    /**
     * @var int
     * @cvalue TimeZone::LONG_GENERIC
     */
    public const DISPLAY_LONG_GENERIC = UNKNOWN;
    /**
     * @var int
     * @cvalue TimeZone::SHORT_GMT
     */
    public const DISPLAY_SHORT_GMT = UNKNOWN;
    /**
     * @var int
     * @cvalue TimeZone::LONG_GMT
     */
    public const DISPLAY_LONG_GMT = UNKNOWN;
    /**
     * @var int
     * @cvalue TimeZone::SHORT_COMMONLY_USED
     */
    public const DISPLAY_SHORT_COMMONLY_USED = UNKNOWN;
    /**
     * @var int
     * @cvalue TimeZone::GENERIC_LOCATION
     */
    public const DISPLAY_GENERIC_LOCATION = UNKNOWN;

    /**
     * @var int
     * @cvalue UCAL_ZONE_TYPE_ANY
     */
    public const TYPE_ANY = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_ZONE_TYPE_CANONICAL
     */
    public const TYPE_CANONICAL = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_ZONE_TYPE_CANONICAL_LOCATION
     */
    public const TYPE_CANONICAL_LOCATION = UNKNOWN;

    private function __construct() {}

    /**
     * @tentative-return-type
     * @alias intltz_count_equivalent_ids
     */
    public static function countEquivalentIDs(string $timezoneId): int|false {}

    /**
     * @tentative-return-type
     * @alias intltz_create_default
     */
    public static function createDefault(): IntlTimeZone {}

    /**
     * @param IntlTimeZone|string|int|float|null $countryOrRawOffset
     * @tentative-return-type
     * @alias intltz_create_enumeration
     */
    public static function createEnumeration($countryOrRawOffset = null): IntlIterator|false {}

    /**
     * @tentative-return-type
     * @alias intltz_create_time_zone
     */
    public static function createTimeZone(string $timezoneId): ?IntlTimeZone {}

    /**
     * @tentative-return-type
     * @alias intltz_create_time_zone_id_enumeration
     */
    public static function createTimeZoneIDEnumeration(int $type, ?string $region = null, ?int $rawOffset = null): IntlIterator|false {}

    /**
     * @tentative-return-type
     * @alias intltz_from_date_time_zone
     */
    public static function fromDateTimeZone(DateTimeZone $timezone): ?IntlTimeZone {}

    /**
     * @param bool $isSystemId
     * @tentative-return-type
     * @alias intltz_get_canonical_id
     */
    public static function getCanonicalID(string $timezoneId, &$isSystemId = null): string|false {}

    /**
     * @tentative-return-type
     * @alias intltz_get_display_name
     */
    public function getDisplayName(bool $dst = false, int $style = IntlTimeZone::DISPLAY_LONG, ?string $locale = null): string|false {}

    /**
     * @tentative-return-type
     * @alias intltz_get_dst_savings
     */
    public function getDSTSavings(): int {}

    /**
     * @tentative-return-type
     * @alias intltz_get_equivalent_id
     */
    public static function getEquivalentID(string $timezoneId, int $offset): string|false {}

    /**
     * @tentative-return-type
     * @alias intltz_get_error_code
     */
    public function getErrorCode(): int|false {}

    /**
     * @tentative-return-type
     * @alias intltz_get_error_message
     */
    public function getErrorMessage(): string|false {}

    /**
     * @tentative-return-type
     * @alias intltz_get_gmt
     */
    public static function getGMT(): IntlTimeZone {}

    /**
     * @tentative-return-type
     * @alias intltz_get_id
     */
    public function getID(): string|false {}

    /**
     * @param int $rawOffset
     * @param int $dstOffset
     * @tentative-return-type
     * @alias intltz_get_offset
     */
    public function getOffset(float $timestamp, bool $local, &$rawOffset, &$dstOffset): bool {}

    /**
     * @tentative-return-type
     * @alias intltz_get_raw_offset
     */
    public function getRawOffset(): int {}

    /**
     * @tentative-return-type
     * @alias intltz_get_region
     */
    public static function getRegion(string $timezoneId): string|false {}

    /**
     * @tentative-return-type
     * @alias intltz_get_tz_data_version
     */
    public static function getTZDataVersion(): string|false {}

    /**
     * @tentative-return-type
     * @alias intltz_get_unknown
     */
    public static function getUnknown(): IntlTimeZone {}

#if U_ICU_VERSION_MAJOR_NUM >= 52
    /**
     * @tentative-return-type
     * @alias intltz_get_windows_id
     */
    public static function getWindowsID(string $timezoneId): string|false {}

    /**
     * @tentative-return-type
     * @alias intltz_get_id_for_windows_id
     */
    public static function getIDForWindowsID(string $timezoneId, ?string $region = null): string|false {}
#endif
    /**
     * @tentative-return-type
     * @alias intltz_has_same_rules
     */
    public function hasSameRules(IntlTimeZone $other): bool {}

    /**
     * @tentative-return-type
     * @alias intltz_to_date_time_zone
     */
    public function toDateTimeZone(): DateTimeZone|false {}

    /**
     * @tentative-return-type
     * @alias intltz_use_daylight_time
     */
    public function useDaylightTime(): bool {}
}
