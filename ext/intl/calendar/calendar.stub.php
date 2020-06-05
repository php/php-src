<?php

/** @generate-function-entries */

class IntlCalendar
{
    private function __construct() {}

    /**
     * @param IntlTimeZone|DateTimeZone|string|null $timeZone
     * @return IntlCalendar|IntlGregorianCalendar|null
     * @alias intlcal_create_instance
     */
    public static function createInstance($timeZone = null, ?string $locale = null) {}

    /**
     * @return bool
     * @alias intlcal_equals
     */
    public function equals(IntlCalendar $calendar) {}

    /**
     * @return int
     * @alias intlcal_field_difference
     */
    public function fieldDifference(float $when, int $field) {}

    /**
     * @return bool
     * @alias intlcal_add
     */
    public function add(int $field, int $amount) {}

    /**
     * @return bool
     * @alias intlcal_after
     */
    public function after(IntlCalendar $calendar) {}

    /**
     * @return bool
     * @alias intlcal_before
     */
    public function before(IntlCalendar $calendar) {}

    /**
     * @return bool
     * @alias intlcal_clear
     */
    public function clear(?int $field = null) {}

    /**
     * @param DateTime|string $dateTime
     * @return IntlCalendar|null
     * @alias intlcal_from_date_time
     */
    public static function fromDateTime($dateTime, ?string $locale = null) {}

    /**
     * @return int
     * @alias intlcal_get
     */
    public function get(int $field) {}

    /**
     * @return int
     * @alias intlcal_get_actual_maximum
     */
    public function getActualMaximum(int $field) {}

    /**
     * @return int
     * @alias intlcal_get_actual_minimum
     */
    public function getActualMinimum(int $field) {}

    /**
     * @return array
     * @alias intlcal_get_available_locales
     */
    public static function getAvailableLocales() {}

    /**
     * @return int
     * @alias intlcal_get_day_of_week_type
     */
    public function getDayOfWeekType(int $dayOfWeek) {}

    /**
     * @return int|false
     * @alias intlcal_get_error_code
     */
    public function getErrorCode() {}

    /**
     * @return string|false
     * @alias intlcal_get_error_message
     */
    public function getErrorMessage() {}

    /**
     * @return int
     * @alias intlcal_get_first_day_of_week
     */
    public function getFirstDayOfWeek() {}

    /**
     * @return int
     * @alias intlcal_get_greatest_minimum
     */
    public function getGreatestMinimum(int $field) {}

    /**
     * @return IntlIterator|false
     * @alias intlcal_get_keyword_values_for_locale
     */
    public static function getKeywordValuesForLocale(string $key, string $locale, bool $commonlyUsed) {}

    /**
     * @return int
     * @alias intlcal_get_least_maximum
     */
    public function getLeastMaximum(int $field) {}

    /**
     * @return string
     * @alias intlcal_get_locale
     */
    public function getLocale(int $localeType) {}

    /**
     * @return int
     * @alias intlcal_get_maximum
     */
    public function getMaximum(int $field) {}

    /**
     * @return int
     * @alias intlcal_get_minimal_days_in_first_week
     */
    public function getMinimalDaysInFirstWeek() {}

    /**
     * @return bool
     * @alias intlcal_set_minimal_days_in_first_week
     */
    public function setMinimalDaysInFirstWeek(int $numberOfDays) {}

    /**
     * @return int
     * @alias intlcal_get_minimum
     */
    public function getMinimum(int $field) {}

    /**
     * @return float
     * @alias intlcal_get_now
     */
    public static function getNow() {}

    /**
     * @return int
     * @alias intlcal_get_repeated_wall_time_option
     */
    public function getRepeatedWallTimeOption() {}

    /**
     * @return int
     * @alias intlcal_get_skipped_wall_time_option
     */
    public function getSkippedWallTimeOption() {}

    /**
     * @return float
     * @alias intlcal_get_time
     */
    public function getTime() {}

    /**
     * @return IntlTimeZone|false
     * @alias intlcal_get_time_zone
     */
    public function getTimeZone() {}

    /**
     * @return string
     * @alias intlcal_get_type
     */
    public function getType() {}

    /**
     * @return int
     * @alias intlcal_get_weekend_transition
     */
    public function getWeekendTransition(int $dayOfWeek) {}

    /**
     * @return bool
     * @alias intlcal_in_daylight_time
     */
    public function inDaylightTime() {}

    /**
     * @return bool
     * @alias intlcal_is_equivalent_to
     */
    public function isEquivalentTo(IntlCalendar $calendar) {}

    /**
     * @return bool
     * @alias intlcal_is_lenient
     */
    public function isLenient() {}

    /**
     * @return bool
     * @alias intlcal_is_weekend
     */
    public function isWeekend(?float $date = null) {}

    /**
     * @param int|bool $amountOrUpOrDown
     * @return bool
     * @alias intlcal_roll
     */
    public function roll(int $field, $amountOrUpOrDown) {}

    /**
     * @return bool
     * @alias intlcal_is_set
     */
    public function isSet(int $field) {}

    /**
     * @return bool
     * @alias intlcal_set
     */
    public function set(int $year, int $month, int $dayOfMonth = UNKNOWN, $hour = UNKNOWN, int $minute = UNKNOWN, int $second = UNKNOWN) {}

    /**
     * @return bool
     * @alias intlcal_set_first_day_of_week
     */
    public function setFirstDayOfWeek(int $dayOfWeek) {}

    /**
     * @return bool
     * @alias intlcal_set_lenient
     */
    public function setLenient(bool $isLenient) {}

    /**
     * @return bool
     * @alias intlcal_set_repeated_wall_time_option
     */
    public function setRepeatedWallTimeOption(int $wallTimeOption) {}

    /**
     * @return bool
     * @alias intlcal_set_skipped_wall_time_option
     */
    public function setSkippedWallTimeOption(int $wallTimeOption) {}

    /**
     * @return bool
     * @alias intlcal_set_time
     */
    public function setTime(float $date) {}

    /**
     * @param IntlTimeZone|DateTimeZone|string|null $timeZone
     * @return bool
     * @alias intlcal_set_time_zone
     */
    public function setTimeZone($timeZone) {}

    /**
     * @return DateTime|false
     * @alias intlcal_to_date_time
     */
    public function toDateTime() {}
}

class IntlGregorianCalendar extends IntlCalendar
{
    public function __construct($timeZoneOrYear = UNKNOWN, $localeOrMonth = UNKNOWN, $dayOfMonth = UNKNOWN, $hour = UNKNOWN, $minute = UNKNOWN, $second = UNKNOWN) {}

    /**
     * @return bool
     * @alias intlgregcal_set_gregorian_change
     */
    public function setGregorianChange(float $change) {}

    /**
     * @return float
     * @alias intlgregcal_get_gregorian_change
     */
    public function getGregorianChange() {}

    /**
     * @return bool
     * @alias intlgregcal_is_leap_year
     */
    public function isLeapYear(int $year) {}
}
