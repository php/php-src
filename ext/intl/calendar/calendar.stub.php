<?php

class IntlCalendar
{
    private function __construct() {}

    /**
     * @param IntlTimeZone|DateTimeZone|string|null $timeZone
     * @return IntlCalendar|null
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
