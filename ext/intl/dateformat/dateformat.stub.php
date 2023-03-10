<?php

/** @generate-class-entries */

/** @not-serializable */
class IntlDateFormatter
{
    /**
     * @var int
     * @cvalue UDAT_FULL
     */
    public const FULL = UNKNOWN;
    /**
     * @var int
     * @cvalue UDAT_LONG
     */
    public const LONG = UNKNOWN;
    /**
     * @var int
     * @cvalue UDAT_MEDIUM
     */
    public const MEDIUM = UNKNOWN;
    /**
     * @var int
     * @cvalue UDAT_SHORT
     */
    public const SHORT = UNKNOWN;
    /**
     * @var int
     * @cvalue UDAT_NONE
     */
    public const NONE = UNKNOWN;
    /**
     * @var int
     * @cvalue UDAT_FULL_RELATIVE
     */
    public const RELATIVE_FULL = UNKNOWN;
    /**
     * @var int
     * @cvalue UDAT_LONG_RELATIVE
     */
    public const RELATIVE_LONG = UNKNOWN;
    /**
     * @var int
     * @cvalue UDAT_MEDIUM_RELATIVE
     */
    public const RELATIVE_MEDIUM = UNKNOWN;
    /**
     * @var int
     * @cvalue UDAT_SHORT_RELATIVE
     */
    public const RELATIVE_SHORT = UNKNOWN;

    /**
     * @var int
     * @cvalue UCAL_GREGORIAN
     */
    public const GREGORIAN = UNKNOWN;
    /**
     * @var int
     * @cvalue UCAL_TRADITIONAL
     */
    public const TRADITIONAL = UNKNOWN;

    /**
     * @param IntlTimeZone|DateTimeZone|string|null $timezone
     * @param IntlCalendar|int|null $calendar
     */
    public function __construct(
        ?string $locale,
        int $dateType = IntlDateFormatter::FULL,
        int $timeType = IntlDateFormatter::FULL,
        $timezone = null,
        $calendar = null,
        ?string $pattern = null
    ) {}

    /**
     * @param IntlTimeZone|DateTimeZone|string|null $timezone
     * @tentative-return-type
     * @alias datefmt_create
     */
    public static function create(
        ?string $locale,
        int $dateType = IntlDateFormatter::FULL,
        int $timeType = IntlDateFormatter::FULL,
        $timezone = null,
        IntlCalendar|int|null $calendar = null,
        ?string $pattern = null
    ): ?IntlDateFormatter {}

    /**
     * @tentative-return-type
     * @alias datefmt_get_datetype
     */
    public function getDateType(): int|false {}

    /**
     * @tentative-return-type
     * @alias datefmt_get_timetype
     */
    public function getTimeType(): int|false {}

    /**
     * @tentative-return-type
     * @alias datefmt_get_calendar
     */
    public function getCalendar(): int|false {}

    /**
     * @tentative-return-type
     * @alias datefmt_set_calendar
     */
    public function setCalendar(IntlCalendar|int|null $calendar): bool {}

    /**
     * @tentative-return-type
     * @alias datefmt_get_timezone_id
     */
    public function getTimeZoneId(): string|false {}

    /**
     * @tentative-return-type
     * @alias datefmt_get_calendar_object
     */
    public function getCalendarObject(): IntlCalendar|false|null {}

    /**
     * @tentative-return-type
     * @alias datefmt_get_timezone
     */
    public function getTimeZone(): IntlTimeZone|false {}

    /**
     * @param IntlTimeZone|DateTimeZone|string|null $timezone
     * @tentative-return-type
     * @alias datefmt_set_timezone
     */
    public function setTimeZone($timezone): bool {}

    /**
     * @tentative-return-type
     * @alias datefmt_set_pattern
     */
    public function setPattern(string $pattern): bool {}

    /**
     * @tentative-return-type
     * @alias datefmt_get_pattern
     */
    public function getPattern(): string|false {}

    /**
     * @tentative-return-type
     * @alias datefmt_get_locale
     */
    public function getLocale(int $type = ULOC_ACTUAL_LOCALE): string|false {}

    /**
     * @tentative-return-type
     * @alias datefmt_set_lenient
     */
    public function setLenient(bool $lenient): void {}

    /**
     * @tentative-return-type
     * @alias datefmt_is_lenient
     */
    public function isLenient(): bool {}

    /**
     * @param IntlCalendar|DateTimeInterface|array|string|int|float $datetime
     * @tentative-return-type
     * @alias datefmt_format
     */
    public function format($datetime): string|false {}

    /**
     * @param IntlCalendar|DateTimeInterface $datetime
     * @param array|int|string|null $format
     * @tentative-return-type
     * @alias datefmt_format_object
     */
    public static function formatObject($datetime, $format = null, ?string $locale = null): string|false {}

    /**
     * @param int $offset
     * @tentative-return-type
     * @alias datefmt_parse
     */
    public function parse(string $string, &$offset = null): int|float|false {}

    /**
     * @param int $offset
     * @return array<string, int>|false
     * @tentative-return-type
     * @alias datefmt_localtime
     */
    public function localtime(string $string, &$offset = null): array|false {}

    /**
     * @tentative-return-type
     * @alias datefmt_get_error_code
     */
    public function getErrorCode(): int {}

    /**
     * @tentative-return-type
     * @alias datefmt_get_error_message
     */
    public function getErrorMessage(): string {}
}
