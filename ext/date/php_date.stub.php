<?php

/** @generate-class-entries */

function strtotime(string $datetime, ?int $baseTimestamp = null): int|false {}

function date(string $format, ?int $timestamp = null): string {}

function idate(string $format, ?int $timestamp = null): int|false {}

function gmdate(string $format, ?int $timestamp = null): string {}

function mktime(
    int $hour, ?int $minute = null, ?int $second = null,
    ?int $month = null, ?int $day = null, ?int $year = null): int|false {}

function gmmktime(
    int $hour, ?int $minute = null, ?int $second = null,
    ?int $month = null, ?int $day = null, ?int $year = null): int|false {}

function checkdate(int $month, int $day, int $year): bool {}

function strftime(string $format, ?int $timestamp = null): string|false {}

function gmstrftime(string $format, ?int $timestamp = null): string|false {}

function time(): int {}

function localtime(?int $timestamp = null, bool $associative = false): array {}

function getdate(?int $timestamp = null): array {}

function date_create(string $datetime = "now", ?DateTimeZone $timezone = null): DateTime|false {}

function date_create_immutable(
    string $datetime = "now", ?DateTimeZone $timezone = null): DateTimeImmutable|false {}

function date_create_from_format(
    string $format, string $datetime, ?DateTimeZone $timezone = null): DateTime|false {}

function date_create_immutable_from_format(
    string $format, string $datetime, ?DateTimeZone $timezone = null): DateTimeImmutable|false {}

function date_parse(string $datetime): array {}

function date_parse_from_format(string $format, string $datetime): array {}

function date_get_last_errors(): array|false {}

function date_format(DateTimeInterface $object, string $format): string {}

function date_modify(DateTime $object, string $modifier): DateTime|false {}

function date_add(DateTime $object, DateInterval $interval): DateTime {}

function date_sub(DateTime $object, DateInterval $interval): DateTime {}

function date_timezone_get(DateTimeInterface $object): DateTimeZone|false {}

function date_timezone_set(DateTime $object, DateTimeZone $timezone): DateTime {}

function date_offset_get(DateTimeInterface $object): int {}

function date_diff(
    DateTimeInterface $baseObject, DateTimeInterface $targetObject, bool $absolute = false): DateInterval {}

function date_time_set(
    DateTime $object, int $hour, int $minute, int $second = 0, int $microsecond = 0): DateTime {}

function date_date_set(DateTime $object, int $year, int $month, int $day): DateTime {}

function date_isodate_set(DateTime $object, int $year, int $week, int $dayOfWeek = 1): DateTime {}

function date_timestamp_set(DateTime $object, int $timestamp): DateTime {}

function date_timestamp_get(DateTimeInterface $object): int {}

function timezone_open(string $timezone): DateTimeZone|false {}

function timezone_name_get(DateTimeZone $object): string {}

function timezone_name_from_abbr(string $abbr, int $utcOffset = -1, int $isDST = -1): string|false {}

function timezone_offset_get(DateTimeZone $object, DateTimeInterface $datetime): int {}

function timezone_transitions_get(
    DateTimeZone $object, int $timestampBegin = PHP_INT_MIN, int $timestampEnd = PHP_INT_MAX): array|false {}

function timezone_location_get(DateTimeZone $object): array|false {}

function timezone_identifiers_list(int $timezoneGroup = DateTimeZone::ALL, ?string $countryCode = null): array {}

function timezone_abbreviations_list(): array {}

function timezone_version_get(): string {}

function date_interval_create_from_date_string(string $datetime): DateInterval|false {}

function date_interval_format(DateInterval $object, string $format): string {}

function date_default_timezone_set(string $timezoneId): bool {}

function date_default_timezone_get(): string {}

function date_sunrise(
    int $timestamp, int $returnFormat = SUNFUNCS_RET_STRING,
    ?float $latitude = null, ?float $longitude = null, ?float $zenith = null,
    ?float $utcOffset = null): string|int|float|false {}

function date_sunset(
    int $timestamp, int $returnFormat = SUNFUNCS_RET_STRING,
    ?float $latitude = null, ?float $longitude = null, ?float $zenith = null,
    ?float $utcOffset = null): string|int|float|false {}

function date_sun_info(int $timestamp, float $latitude, float $longitude): array {}

interface DateTimeInterface
{
    /** @tentative-return-type */
    public function format(string $format): string;

    /** @tentative-return-type */
    public function getTimezone(): DateTimeZone|false;

    /** @tentative-return-type */
    public function getOffset(): int;

    /** @tentative-return-type */
    public function getTimestamp(): int|false;

    /** @tentative-return-type */
    public function diff(DateTimeInterface $targetObject, bool $absolute = false): DateInterval;

    /** @tentative-return-type */
    public function __wakeup(): void;
}

class DateTime implements DateTimeInterface
{
    public function __construct(string $datetime = "now", ?DateTimeZone $timezone = null) {}

    /** @tentative-return-type */
    public function __wakeup(): void {}

    /** @tentative-return-type */
    public static function __set_state(array $array): DateTime {}

    /** @tentative-return-type */
    public static function createFromImmutable(DateTimeImmutable $object): DateTime {}

    public static function createFromInterface(DateTimeInterface $object): DateTime {}

    /**
     * @tentative-return-type
     * @alias date_create_from_format
     */
    public static function createFromFormat(string $format, string $datetime, ?DateTimeZone $timezone = null): DateTime|false {}

    /**
     * @tentative-return-type
     * @alias date_get_last_errors
     */
    public static function getLastErrors(): array|false {}

    /**
     * @tentative-return-type
     * @alias date_format
     */
    public function format(string $format): string {}

    /**
     * @tentative-return-type
     * @alias date_modify
     */
    public function modify(string $modifier): DateTime|false {}

    /**
     * @tentative-return-type
     * @alias date_add
     */
    public function add(DateInterval $interval): DateTime {}

    /**
     * @tentative-return-type
     * @alias date_sub
     */
    public function sub(DateInterval $interval): DateTime {}

    /**
     * @tentative-return-type
     * @alias date_timezone_get
     */
    public function getTimezone(): DateTimeZone|false {}

    /**
     * @tentative-return-type
     * @alias date_timezone_set
     */
    public function setTimezone(DateTimeZone $timezone): DateTime {}

    /**
     * @tentative-return-type
     * @alias date_offset_get
     */
    public function getOffset(): int {}

    /**
     * @tentative-return-type
     * @alias date_time_set
     */
    public function setTime(int $hour, int $minute, int $second = 0, int $microsecond = 0): DateTime {}

    /**
     * @tentative-return-type
     * @alias date_date_set
     */
    public function setDate(int $year, int $month, int $day): DateTime {}

    /**
     * @tentative-return-type
     * @alias date_isodate_set
     */
    public function setISODate(int $year, int $week, int $dayOfWeek = 1): DateTime {}

    /**
     * @tentative-return-type
     * @alias date_timestamp_set
     */
    public function setTimestamp(int $timestamp): DateTime {}

    /**
     * @tentative-return-type
     * @alias date_timestamp_get
     */
    public function getTimestamp(): int {}

    /**
     * @tentative-return-type
     * @alias date_diff
     */
    public function diff(DateTimeInterface $targetObject, bool $absolute = false): DateInterval {}
}

class DateTimeImmutable implements DateTimeInterface
{
    public function __construct(string $datetime = "now", ?DateTimeZone $timezone = null) {}

    /** @tentative-return-type */
    public function __wakeup(): void {}

    /** @tentative-return-type */
    public static function __set_state(array $array): DateTimeImmutable {}

    /**
     * @tentative-return-type
     * @alias date_create_immutable_from_format
     */
    public static function createFromFormat(string $format, string $datetime, ?DateTimeZone $timezone = null): DateTimeImmutable|false {}

    /**
     * @tentative-return-type
     * @alias date_get_last_errors
     */
    public static function getLastErrors(): array|false {}

    /**
     * @tentative-return-type
     * @alias date_format
     */
    public function format(string $format): string {}

    /**
     * @tentative-return-type
     * @alias date_timezone_get
     */
    public function getTimezone(): DateTimeZone|false {}

    /**
     * @tentative-return-type
     * @alias date_offset_get
     */
    public function getOffset(): int {}

    /**
     * @tentative-return-type
     * @alias date_timestamp_get
     */
    public function getTimestamp(): int {}

    /**
     * @tentative-return-type
     * @alias date_diff
     */
    public function diff(DateTimeInterface $targetObject, bool $absolute = false): DateInterval {}

    /** @tentative-return-type */
    public function modify(string $modifier): DateTimeImmutable|false {}

    /** @tentative-return-type */
    public function add(DateInterval $interval): DateTimeImmutable {}

    /** @tentative-return-type */
    public function sub(DateInterval $interval): DateTimeImmutable {}

    /** @tentative-return-type */
    public function setTimezone(DateTimeZone $timezone): DateTimeImmutable {}

    /** @tentative-return-type */
    public function setTime(int $hour, int $minute, int $second = 0, int $microsecond = 0): DateTimeImmutable {}

    /** @tentative-return-type */
    public function setDate(int $year, int $month, int $day): DateTimeImmutable {}

    /** @tentative-return-type */
    public function setISODate(int $year, int $week, int $dayOfWeek = 1): DateTimeImmutable {}

    /** @tentative-return-type */
    public function setTimestamp(int $timestamp): DateTimeImmutable {}

    /** @tentative-return-type */
    public static function createFromMutable(DateTime $object): DateTimeImmutable {}

    public static function createFromInterface(DateTimeInterface $object): DateTimeImmutable {}
}

class DateTimeZone
{
    public function __construct(string $timezone) {}

    /**
     * @tentative-return-type
     * @alias timezone_name_get
     */
    public function getName(): string {}

    /**
     * @tentative-return-type
     * @alias timezone_offset_get
     */
    public function getOffset(DateTimeInterface $datetime): int {}

    /**
     * @tentative-return-type
     * @alias timezone_transitions_get
     */
    public function getTransitions(int $timestampBegin = PHP_INT_MIN, int $timestampEnd = PHP_INT_MAX): array|false {}

    /**
     * @tentative-return-type
     * @alias timezone_location_get
     */
    public function getLocation(): array|false {}

    /**
     * @tentative-return-type
     * @alias timezone_abbreviations_list
     */
    public static function listAbbreviations(): array {}

    /**
     * @tentative-return-type
     * @alias timezone_identifiers_list
     */
    public static function listIdentifiers(int $timezoneGroup = DateTimeZone::ALL, ?string $countryCode = null): array {}

    /** @tentative-return-type */
    public function __wakeup(): void {}

    /** @tentative-return-type */
    public static function __set_state(array $array): DateTimeZone {}
}

class DateInterval
{
    public function __construct(string $duration) {}

    /**
     * @tentative-return-type
     * @alias date_interval_create_from_date_string
     */
    public static function createFromDateString(string $datetime): DateInterval|false {}

    /**
     * @tentative-return-type
     * @alias date_interval_format
     */
    public function format(string $format): string {}

    /** @tentative-return-type */
    public function __wakeup(): void {}

    /** @tentative-return-type */
    public static function __set_state(array $array): DateInterval {}
}

class DatePeriod implements IteratorAggregate
{
    /**
     * @param DateTimeInterface|string $start
     * @param DateInterval|int $interval
     * @param DateTimeInterface|int $end
     * @param int $options
     */
    public function __construct($start, $interval = UNKNOWN, $end = UNKNOWN, $options = UNKNOWN) {}

    /** @tentative-return-type */
    public function getStartDate(): DateTimeInterface {}

    /** @tentative-return-type */
    public function getEndDate(): ?DateTimeInterface {}

    /** @tentative-return-type */
    public function getDateInterval(): DateInterval {}

    /** @tentative-return-type */
    public function getRecurrences(): ?int {}

    /** @tentative-return-type */
    public function __wakeup(): void {}

    /** @tentative-return-type */
    public static function __set_state(array $array): DatePeriod {}

    public function getIterator(): Iterator {}
}
