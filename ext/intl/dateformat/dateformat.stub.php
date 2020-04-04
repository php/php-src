<?php

class IntlDateFormatter
{
    /**
     * @param IntlTimeZone|DateTimeZone|string|null $timezone
     * @param IntlCalendar|int|null $calendar
     */
    public function __construct(?string $locale, int $datetype, int $timetype, $timezone = null, $calendar = null, string $pattern = "") {}

    /**
     * @param IntlTimeZone|DateTimeZone|string|null $timezone
     * @param IntlCalendar|int|null $calendar
     * @return IntlDateFormatter|null
     */
    public static function create(?string $locale, int $datetype, int $timetype, $timezone = null, $calendar = null, string $pattern = "") {}

    /** @return int|false */
    public function getDateType() {}

    /** @return int|false */
    public function getTimeType() {}

    /** @return int|false */
    public function getCalendar() {}

    /**
     * @param IntlCalendar|int|null $which
     * @return bool
     */
    public function setCalendar($which) {}

    /** @return string|false */
    public function getTimeZoneId() {}

    /** @return IntlCalendar|null|false */
    public function getCalendarObject() {}

    /** @return IntlTimeZone|false */
    public function getTimeZone() {}

    /**
     * @param IntlTimeZone|DateTimeZone|string|null $zone
     * @return ?bool
     */
    public function setTimeZone($zone) {}

    /** @return bool */
    public function setPattern(string $pattern) {}

    /** @return string|false */
    public function getPattern() {}

    /** @return string|false */
    public function getLocale(int $which = UNKNOWN) {}

    /** @return void */
    public function setLenient(bool $lenient) {}

    /** @return bool */
    public function isLenient() {}

    /**
     * @param array|int $value
     * @return string|false
     */
    public function format($value) {}

    /**
     * @param IntlCalendar|DateTime $object
     * @param array|int|string|null $format
     * @return string|false
     */
    public static function formatObject($object, $format = null, ?string $locale = null) {}

    /** @return int|float|false */
    public function parse(string $value, &$position = null) {}

    /** @return array|false */
    public function localtime(string $value, &$position = null) {}

    /** @return int */
    public function getErrorCode() {}

    /** @return string */
    public function getErrorMessage() {}
}
