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

/**
 * @param IntlTimeZone|DateTimeZone|string|null $timezone
 * @param IntlCalendar|int|null $calendar
 */
function datefmt_create(?string $locale, int $datetype, int $timetype, $timezone = null, $calendar = null, string $pattern = ""): ?IntlDateFormatter {}

function datefmt_get_datetype(IntlDateFormatter $df): int|false {}

function datefmt_get_timetype(IntlDateFormatter $df): int|false {}

function datefmt_get_calendar(IntlDateFormatter $df): int|false {}

/** @param IntlCalendar|int|null $which */
function datefmt_set_calendar(IntlDateFormatter $df, $which): bool {}

function datefmt_get_timezone_id(IntlDateFormatter $df): string|false {}

function datefmt_get_calendar_object(IntlDateFormatter $df): IntlCalendar|false|null {}

function datefmt_get_timezone(IntlDateFormatter $df): IntlTimeZone|false {}

/** @param IntlTimeZone|DateTimeZone|string|null $zone */
function datefmt_set_timezone(IntlDateFormatter $df, $zone): ?bool {}

function datefmt_set_pattern(IntlDateFormatter $df, string $pattern): bool {}

function datefmt_get_pattern(IntlDateFormatter $df): string|false {}

function datefmt_get_locale(IntlDateFormatter $df, int $which = UNKNOWN): string|false {}

function datefmt_set_lenient(IntlDateFormatter $df, bool $lenient): void {}

function datefmt_is_lenient(IntlDateFormatter $df): bool {}

/** @param mixed $value */
function datefmt_format(IntlDateFormatter $df, $value): string|false {}

/**
 * @param IntlCalendar|DateTimeInterface $object
 * @param array|int|string|null $format
 */
function datefmt_format_object($object, $format = null, ?string $locale = null): string|false {}

function datefmt_parse(IntlDateFormatter $df, string $value, &$position = null): int|float|false {}

function datefmt_localtime(IntlDateFormatter $df, string $value, &$position = null): array|false {}

function datefmt_get_error_code(IntlDateFormatter $df): int {}

function datefmt_get_error_message(IntlDateFormatter $df): string {}
