<?php

/** @generate-class-entries */

/** @not-serializable */
class IntlCalendar
{
    /**
     * @var int
     * @cvalue UCAL_ERA
     */
    public const FIELD_ERA = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_YEAR
     */
    public const FIELD_YEAR = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_MONTH
     */
    public const FIELD_MONTH = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_WEEK_OF_YEAR
     */
    public const FIELD_WEEK_OF_YEAR = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_WEEK_OF_MONTH
     */
    public const FIELD_WEEK_OF_MONTH = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_DATE
     */
    public const FIELD_DATE = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_DAY_OF_YEAR
     */
    public const FIELD_DAY_OF_YEAR = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_DAY_OF_WEEK
     */
    public const FIELD_DAY_OF_WEEK = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_DAY_OF_WEEK_IN_MONTH
     */
    public const FIELD_DAY_OF_WEEK_IN_MONTH = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_AM_PM
     */
    public const FIELD_AM_PM = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_HOUR
     */
    public const FIELD_HOUR = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_HOUR_OF_DAY
     */
    public const FIELD_HOUR_OF_DAY = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_MINUTE
     */
    public const FIELD_MINUTE = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_SECOND
     */
    public const FIELD_SECOND = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_MILLISECOND
     */
    public const FIELD_MILLISECOND = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_ZONE_OFFSET
     */
    public const FIELD_ZONE_OFFSET = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_DST_OFFSET
     */
    public const FIELD_DST_OFFSET = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_YEAR_WOY
     */
    public const FIELD_YEAR_WOY = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_DOW_LOCAL
     */
    public const FIELD_DOW_LOCAL = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_EXTENDED_YEAR
     */
    public const FIELD_EXTENDED_YEAR = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_JULIAN_DAY
     */
    public const FIELD_JULIAN_DAY = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_MILLISECONDS_IN_DAY
     */
    public const FIELD_MILLISECONDS_IN_DAY = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_IS_LEAP_MONTH
     */
    public const FIELD_IS_LEAP_MONTH = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_FIELD_COUNT
     */
    public const FIELD_FIELD_COUNT = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_DAY_OF_MONTH
     */
    public const FIELD_DAY_OF_MONTH = UNKNOWN;

    /**
     * @var int
     * @cvalue UCAL_SUNDAY
     */
    public const DOW_SUNDAY = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_MONDAY
     */
    public const DOW_MONDAY = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_TUESDAY
     */
    public const DOW_TUESDAY = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_WEDNESDAY
     */
    public const DOW_WEDNESDAY = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_THURSDAY
     */
    public const DOW_THURSDAY = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_FRIDAY
     */
    public const DOW_FRIDAY = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_SATURDAY
     */
    public const DOW_SATURDAY = UNKNOWN;

    /**
     * @var int
     * @cvalue UCAL_WEEKDAY
     */
    public const DOW_TYPE_WEEKDAY = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_WEEKEND
     */
    public const DOW_TYPE_WEEKEND = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_WEEKEND_ONSET
     */
    public const DOW_TYPE_WEEKEND_OFFSET = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_WEEKEND_CEASE
     */
    public const DOW_TYPE_WEEKEND_CEASE = UNKNOWN;

    /**
     * @var int
     * @cvalue UCAL_WALLTIME_FIRST
     */
    public const WALLTIME_FIRST = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_WALLTIME_LAST
     */
    public const WALLTIME_LAST = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_WALLTIME_NEXT_VALID
     */
    public const WALLTIME_NEXT_VALID = UNKNOWN;

    private function __construct() {}

    /**
     * @param IntlTimeZone|DateTimeZone|string|null $timezone
     * @tentative-return-type
     * @alias intlcal_create_instance
     */
    public static function createInstance($timezone = null, ?string $locale = null): ?IntlCalendar {}

    /**
     * @tentative-return-type
     * @alias intlcal_equals
     */
    public function equals(IntlCalendar $other): bool {}

    /**
     * @tentative-return-type
     * @alias intlcal_field_difference
     */
    public function fieldDifference(float $timestamp, int $field): int|false {}

    /**
     * @tentative-return-type
     * @alias intlcal_add
     */
    public function add(int $field, int $value): bool {}

    /**
     * @tentative-return-type
     * @alias intlcal_after
     */
    public function after(IntlCalendar $other): bool {}

    /**
     * @tentative-return-type
     * @alias intlcal_before
     */
    public function before(IntlCalendar $other): bool {}

    /**
     * @return true
     * @alias intlcal_clear
     */
    public function clear(?int $field = null) {} // TODO make return type void

    /**
     * @tentative-return-type
     * @alias intlcal_from_date_time
     */
    public static function fromDateTime(DateTime|string $datetime, ?string $locale = null): ?IntlCalendar {}

    /**
     * @tentative-return-type
     * @alias intlcal_get
     */
    public function get(int $field): int|false {}

    /**
     * @tentative-return-type
     * @alias intlcal_get_actual_maximum
     */
    public function getActualMaximum(int $field): int|false {}

    /**
     * @tentative-return-type
     * @alias intlcal_get_actual_minimum
     */
    public function getActualMinimum(int $field): int|false {}

    /**
     * @return array<int, string>
     * @tentative-return-type
     * @alias intlcal_get_available_locales
     */
    public static function getAvailableLocales(): array {}

    /**
     * @tentative-return-type
     * @alias intlcal_get_day_of_week_type
     */
    public function getDayOfWeekType(int $dayOfWeek): int|false {}

    /**
     * @tentative-return-type
     * @alias intlcal_get_error_code
     */
    public function getErrorCode(): int|false {}

    /**
     * @tentative-return-type
     * @alias intlcal_get_error_message
     */
    public function getErrorMessage(): string|false {}

    /**
     * @tentative-return-type
     * @alias intlcal_get_first_day_of_week
     */
    public function getFirstDayOfWeek(): int|false {}

    /**
     * @tentative-return-type
     * @alias intlcal_get_greatest_minimum
     */
    public function getGreatestMinimum(int $field): int|false {}

    /**
     * @tentative-return-type
     * @alias intlcal_get_keyword_values_for_locale
     */
    public static function getKeywordValuesForLocale(string $keyword, string $locale, bool $onlyCommon): IntlIterator|false {}

    /**
     * @tentative-return-type
     * @alias intlcal_get_least_maximum
     */
    public function getLeastMaximum(int $field): int|false {}

    /**
     * @tentative-return-type
     * @alias intlcal_get_locale
     */
    public function getLocale(int $type): string|false {}

    /**
     * @tentative-return-type
     * @alias intlcal_get_maximum
     */
    public function getMaximum(int $field): int|false {}

    /**
     * @tentative-return-type
     * @alias intlcal_get_minimal_days_in_first_week
     */
    public function getMinimalDaysInFirstWeek(): int|false {}

    /**
     * @return true
     * @alias intlcal_set_minimal_days_in_first_week
     */
    public function setMinimalDaysInFirstWeek(int $days) {} // TODO make return void

    /**
     * @tentative-return-type
     * @alias intlcal_get_minimum
     */
    public function getMinimum(int $field): int|false {}

    /**
     * @tentative-return-type
     * @alias intlcal_get_now
     */
    public static function getNow(): float {}

    /**
     * @tentative-return-type
     * @alias intlcal_get_repeated_wall_time_option
     */
    public function getRepeatedWallTimeOption(): int {}

    /**
     * @tentative-return-type
     * @alias intlcal_get_skipped_wall_time_option
     */
    public function getSkippedWallTimeOption(): int {}

    /**
     * @tentative-return-type
     * @alias intlcal_get_time
     */
    public function getTime(): float|false {}

    /**
     * @tentative-return-type
     * @alias intlcal_get_time_zone
     */
    public function getTimeZone(): IntlTimeZone|false {}

    /**
     * @tentative-return-type
     * @alias intlcal_get_type
     */
    public function getType(): string {}

    /**
     * @tentative-return-type
     * @alias intlcal_get_weekend_transition
     */
    public function getWeekendTransition(int $dayOfWeek): int|false {}

    /**
     * @tentative-return-type
     * @alias intlcal_in_daylight_time
     */
    public function inDaylightTime(): bool {}

    /**
     * @tentative-return-type
     * @alias intlcal_is_equivalent_to
     */
    public function isEquivalentTo(IntlCalendar $other): bool {}

    /**
     * @tentative-return-type
     * @alias intlcal_is_lenient
     */
    public function isLenient(): bool {}

    /**
     * @tentative-return-type
     * @alias intlcal_is_weekend
     */
    public function isWeekend(?float $timestamp = null): bool {}

    /**
     * @param int|bool $value
     * @alias intlcal_roll
     * @tentative-return-type
     */
    public function roll(int $field, $value): bool {}

    /**
     * @tentative-return-type
     * @alias intlcal_is_set
     */
    public function isSet(int $field): bool {}

    /**
     * @return true
     * @alias intlcal_set
     */
    public function set(int $year, int $month, int $dayOfMonth = UNKNOWN, int $hour = UNKNOWN, int $minute = UNKNOWN, int $second = UNKNOWN) {} // TODO make return type void

    public function setDate(int $year, int $month, int $dayOfMonth): void {}

    public function setDateTime(int $year, int $month, int $dayOfMonth, int $hour, int $minute, ?int $second = null): void {}

    /**
     * @return true
     * @alias intlcal_set_first_day_of_week
     */
    public function setFirstDayOfWeek(int $dayOfWeek) {} // TODO make return type void

    /**
     * @return true
     * @alias intlcal_set_lenient
     */
    public function setLenient(bool $lenient) {} // TODO make return type void

    /**
     * @return true
     * @alias intlcal_set_repeated_wall_time_option
     */
    public function setRepeatedWallTimeOption(int $option) {} // TODO make return type void

    /**
     * @return true
     * @alias intlcal_set_skipped_wall_time_option
     */
    public function setSkippedWallTimeOption(int $option) {} // TODO make return type void

    /**
     * @tentative-return-type
     * @alias intlcal_set_time
     */
    public function setTime(float $timestamp): bool {}

    /**
     * @param IntlTimeZone|DateTimeZone|string|null $timezone
     * @tentative-return-type
     * @alias intlcal_set_time_zone
     */
    public function setTimeZone($timezone): bool {}

    /**
     * @tentative-return-type
     * @alias intlcal_to_date_time
     */
    public function toDateTime(): DateTime|false {}
}

/** @not-serializable */
class IntlGregorianCalendar extends IntlCalendar
{
    public static function createFromDate(int $year, int $month, int $dayOfMonth): static {}

    public static function createFromDateTime(int $year, int $month, int $dayOfMonth, int $hour, int $minute, ?int $second = null): static {}

    /**
     * @param DateTimeZone|IntlTimeZone|string|int|null $timezoneOrYear
     * @param string|int|null $localeOrMonth
     * @param int $day
     * @param int $hour
     * @param int $minute
     * @param int $second
     */
    public function __construct($timezoneOrYear = UNKNOWN, $localeOrMonth = UNKNOWN, $day = UNKNOWN, $hour = UNKNOWN, $minute = UNKNOWN, $second = UNKNOWN) {}

    /**
     * @tentative-return-type
     * @alias intlgregcal_set_gregorian_change
     */
    public function setGregorianChange(float $timestamp): bool {}

    /**
     * @tentative-return-type
     * @alias intlgregcal_get_gregorian_change
     */
    public function getGregorianChange(): float {}

    /**
     * @tentative-return-type
     * @alias intlgregcal_is_leap_year
     */
    public function isLeapYear(int $year): bool {}
}
