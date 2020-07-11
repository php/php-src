<?php

/** @generate-function-entries */

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
     * @alias datefmt_create
     */
    public static function create(?string $locale, int $datetype, int $timetype, $timezone = null, $calendar = null, string $pattern = "") {}

    /**
     * @return int|false
     * @alias datefmt_get_datetype
     */
    public function getDateType() {}

    /**
     * @return int|false
     * @alias datefmt_get_timetype
     */
    public function getTimeType() {}

    /**
     * @return int|false
     * @alias datefmt_get_calendar
     */
    public function getCalendar() {}

    /**
     * @param IntlCalendar|int|null $which
     * @return bool
     * @alias datefmt_set_calendar
     */
    public function setCalendar($which) {}

    /**
     * @return string|false
     * @alias datefmt_get_timezone_id
     */
    public function getTimeZoneId() {}

    /**
     * @return IntlCalendar|null|false
     * @alias datefmt_get_calendar_object
     */
    public function getCalendarObject() {}

    /**
     * @return IntlTimeZone|false
     * @alias datefmt_get_timezone
     */
    public function getTimeZone() {}

    /**
     * @param IntlTimeZone|DateTimeZone|string|null $zone
     * @return bool|null
     * @alias datefmt_set_timezone
     */
    public function setTimeZone($zone) {}

    /**
     * @return bool
     * @alias datefmt_set_pattern
     */
    public function setPattern(string $pattern) {}

    /**
     * @return string|false
     * @alias datefmt_get_pattern
     */
    public function getPattern() {}

    /**
     * @return string|false
     * @alias datefmt_get_locale
     */
    public function getLocale(int $which = ULOC_ACTUAL_LOCALE) {}

    /**
     * @return void
     * @alias datefmt_set_lenient
     */
    public function setLenient(bool $lenient) {}

    /**
     * @return bool
     * @alias datefmt_is_lenient
     */
    public function isLenient() {}

    /**
     * @param array|int $value
     * @return string|false
     * @alias datefmt_format
     */
    public function format($value) {}

    /**
     * @param IntlCalendar|DateTime $object
     * @param array|int|string|null $format
     * @return string|false
     * @alias datefmt_format_object
     */
    public static function formatObject($object, $format = null, ?string $locale = null) {}

    /**
     * @return int|float|false
     * @alias datefmt_parse
     */
    public function parse(string $value, &$position = null) {}

    /**
     * @return array|false
     * @alias datefmt_localtime
     */
    public function localtime(string $value, &$position = null) {}

    /**
     * @return int
     * @alias datefmt_get_error_code
     */
    public function getErrorCode() {}

    /**
     * @return string
     * @alias datefmt_get_error_message
     */
    public function getErrorMessage() {}
}
