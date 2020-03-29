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

    /** @return int */
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

    /** @return int */
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
