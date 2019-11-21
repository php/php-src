<?php

class IntlCalendar
{
    private function __construct() {}

    /**
     * @param IntlTimeZone|DateTimeZone|string|null $timeZone
     * @return IntlGregorianCalendar|null
     */
    public static function createInstance($timeZone = null, ?string $locale = null) {}

    /** @return bool */
    public function equals(IntlCalendar $calendar) {}

    /** @return int|false */
    public function fieldDifference(float $when, int $field) {}

    /** @return bool */
    public function add(int $field, int $amount) {}

    /** @return bool */
    public function after(IntlCalendar $calendar) {}

    /** @return bool */
    public function before(IntlCalendar $calendar) {}

    /** @return bool */
    public function clear(?int $field = null) {}

    /**
     * @param DateTime|string $dateTime
     * @return IntlCalendar|null
     */
    public static function fromDateTime($dateTime, ?string $locale = null) {}

    /** @return int|false */
    public function get(int $field) {}

    /** @return int|false */
    public function getActualMaximum(int $field) {}

    /** @return int|false */
    public function getActualMinimum(int $field) {}

    /** @return array */
    public static function getAvailableLocales() {}

    /** @return int|false */
    public function getDayOfWeekType(int $dayOfWeek) {}

    /** @return int|false */
    public function getErrorCode() {}

    /** @return string|false */
    public function getErrorMessage() {}

    /** @return int|false */
    public function getFirstDayOfWeek() {}

    /** @return int|false */
    public function getGreatestMinimum(int $field) {}

    /** @return IntlIterator|false */
    public static function getKeywordValuesForLocale(string $key, string $locale, bool $commonlyUsed) {}

    /** @return int|false */
    public function getLeastMaximum(int $field) {}

    /** @return string|false */
    public function getLocale(int $localeType) {}

    /** @return int|false */
    public function getMaximum(int $field) {}

    /** @return int|false */
    public function getMinimalDaysInFirstWeek() {}

    /** @return bool */
    public function setMinimalDaysInFirstWeek(int $numberOfDays) {}

    /** @return int|false */
    public function getMinimum(int $field) {}

    /** @return float */
    public static function getNow() {}

    /** @return int */
    public function getRepeatedWallTimeOption() {}

    /** @return int */
    public function getSkippedWallTimeOption() {}

    /** @return float|false */
    public function getTime() {}

    /** @return IntlTimeZone|false */
    public function getTimeZone() {}

    /** @return string */
    public function getType() {}

    /** @return int|false */
    public function getWeekendTransition(int $dayOfWeek) {}

    /** @return bool */
    public function inDaylightTime() {}

    /** @return bool */
    public function isEquivalentTo(IntlCalendar $calendar) {}

    /** @return bool */
    public function isLenient() {}

    /** @return bool */
    public function isWeekend(?float $date = null) {}

    /**
     * @param int|bool $amountOrUpOrDown
     * @return bool
     */
    public function roll(int $field, $amountOrUpOrDown) {}

    /** @return bool */
    public function isSet(int $field) {}

    /** @return bool */
    public function set(int $year, int $month, int $dayOfMonth = UNKNOWN, $hour = UNKNOWN, int $minute = UNKNOWN, int $second = UNKNOWN) {}

    /** @return bool */
    public function setFirstDayOfWeek(int $dayOfWeek) {}

    /** @return bool */
    public function setLenient(bool $isLenient) {}

    /** @return bool */
    public function setRepeatedWallTimeOption(int $wallTimeOption) {}

    /** @return bool */
    public function setSkippedWallTimeOption(int $wallTimeOption) {}

    /** @return bool */
    public function setTime(float $date) {}

    /**
     * @param IntlTimeZone|DateTimeZone|string|null $timeZone
     * @return bool
     */
    public function setTimeZone($timeZone) {}

    /** @return DateTime|false */
    public function toDateTime() {}
}

class IntlGregorianCalendar extends IntlCalendar
{
    public function __construct() {}

    /**
     * @param IntlTimeZone|DateTimeZone|string|null $timeZone
     * @return IntlGregorianCalendar|null
     */
    public static function createInstance($timeZone = null, ?string $locale = null) {}

    /** @return bool */
    public function setGregorianChange(float $change) {}

    /** @return float|false */
    public function getGregorianChange() {}

    /** @return bool */
    public function isLeapYear(int $year) {}
}

/** @param IntlTimeZone|DateTimeZone|string|null $timeZone */
function intlcal_create_instance($timeZone = null, ?string $locale = null): IntlCalendar|null {}

function intlcal_get_keyword_values_for_locale(string $key, string $locale, bool $commonlyUsed): Iterator|false {}

function intlcal_get_now(): float {}

function intlcal_get_available_locales(): array {}

function intlcal_get(IntlCalendar $calendar, int $field): int|false {}

function intlcal_get_time(IntlCalendar $calendar): float|false {}

function intlcal_set_time(IntlCalendar $calendar, float $date): bool {}

function intlcal_add(IntlCalendar $calendar, int $field, int $amount): bool {}

/** @param IntlTimeZone|DateTimeZone|string|null $timeZone */
function intlcal_set_time_zone(IntlCalendar $calendar, $timeZone): bool {}

function intlcal_after(IntlCalendar $calendarObject, IntlCalendar $calendar): bool {}

function intlcal_before(IntlCalendar $calendarObject, IntlCalendar $calendar): bool {}

function intlcal_set(IntlCalendar $calendar, int $year, int $month, int $dayOfMonth = UNKNOWN, $hour = UNKNOWN, int $minute = UNKNOWN, int $second = UNKNOWN): bool {}

/** @param int|bool $amountOrUpOrDown */
function intlcal_roll(IntlCalendar $calendar, int $field, $amountOrUpOrDown): bool {}

function intlcal_clear(IntlCalendar $calendar, ?int $field = null): bool {}

function intlcal_field_difference(IntlCalendar $calendar, float $when, int $field): int|false {}

function intlcal_get_actual_maximum(IntlCalendar $calendar, int $field): int|false {}

function intlcal_get_actual_minimum(IntlCalendar $calendar, int $field): int|false {}

function intlcal_get_day_of_week_type(IntlCalendar $calendar, int $dayOfWeek): int|false {}

function intlcal_get_first_day_of_week(IntlCalendar $calendar): int|false {}

function intlcal_greatest_minimum(IntlCalendar $calendar, int $field): int|false {}

function intlcal_get_least_maximum(IntlCalendar $calendar, int $field): int|false {}

function intlcal_get_greatest_minimum(IntlCalendar $calendar, int $field): int|false {}

function intlcal_get_locale(IntlCalendar $calendar, int $localeType): string|false {}

function intlcal_get_maximum(IntlCalendar $calendar, int $field): int|false {}

function intlcal_get_minimal_days_in_first_week(IntlCalendar $calendar): int|false {}

function intlcal_set_minimal_days_in_first_week(IntlCalendar $calendar, int $numberOfDays): bool {}

function intlcal_get_minimum(IntlCalendar $calendar, int $field): int|false {}

function intlcal_get_time_zone(IntlCalendar $calendar): IntlTimeZone|false {}

function intlcal_get_type(IntlCalendar $calendar): string {}

function intlcal_get_weekend_transition(IntlCalendar $calendar, int $dayOfWeek): int|false {}

function intlcal_in_daylight_time(IntlCalendar $calendar): bool {}

function intlcal_is_lenient(IntlCalendar $calendar): bool {}

function intlcal_is_set(IntlCalendar $calendar, int $field): bool {}

function intlcal_is_equivalent_to(IntlCalendar $calendarObject, IntlCalendar $calendar): bool {}

function intlcal_is_weekend(IntlCalendar $calendar, ?float $date = null): bool {}

function intlcal_set_first_day_of_week(IntlCalendar $calendar, int $dayOfWeek): bool {}

function intlcal_set_lenient(IntlCalendar $calendar, bool $isLenient): bool {}

function intlcal_get_repeated_wall_time_option(IntlCalendar $calendar): int {}

function intlcal_equals(IntlCalendar $calendarObject, IntlCalendar $calendar): bool {}

function intlcal_get_skipped_wall_time_option(IntlCalendar $calendar): int {}

function intlcal_set_repeated_wall_time_option(IntlCalendar $calendar, int $wallTimeOption): bool {}

function intlcal_set_skipped_wall_time_option(IntlCalendar $calendar, int $wallTimeOption): bool {}

/** @param DateTime|string $dateTime */
function intlcal_from_date_time($dateTime, ?string $locale = null): ?IntlCalendar {}

function intlcal_to_date_time(IntlCalendar $calendar): DateTime|false {}

function intlcal_get_error_code(IntlCalendar $calendar): int|false {}

function intlcal_get_error_message(IntlCalendar $calendar): string|false {}

/** @param IntlTimeZone|DateTimeZone|string|null $timeZone */
function intlgregcal_create_instance($timeZone = null, ?string $locale = null): ?IntlGregorianCalendar {}

function intlgregcal_set_gregorian_change(IntlGregorianCalendar $calendar, float $change): bool {}

function intlgregcal_get_gregorian_change(IntlGregorianCalendar $calendar): float|false {}

function intlgregcal_is_leap_year(IntlGregorianCalendar $calendar, int $year): bool {}
