<?php

/* TODO: Convert "uninitialized" into an exception. This will remove many
 * possibilities for false return values in here. */

function strtotime(string $time, int $now = UNKNOWN): int|false {}

function date(string $format, int $timestamp = UNKNOWN): string {}

function idate(string $format, int $timestamp = UNKNOWN): int|false {}

function gmdate(string $format, int $timestamp = UNKNOWN): string {}

function mktime(
    int $hour, int $min = UNKNOWN, int $sec = UNKNOWN,
    int $mon = UNKNOWN, int $day = UNKNOWN, int $year = UNKNOWN): int|false {}

function gmmktime(
    int $hour, int $min = UNKNOWN, int $sec = UNKNOWN,
    int $mon = UNKNOWN, int $day = UNKNOWN, int $year = UNKNOWN): int|false {}

function checkdate(int $m, int $d, int $y): bool {}

function strftime(string $format, int $timestamp = UNKNOWN): string|false {}

function gmstrftime(string $format, int $timestamp = UNKNOWN): string|false {}

function time(): int {}

function localtime(int $timestamp = UNKNOWN, bool $associative = false): array {}

function getdate(int $timestamp = UNKNOWN): array {}

function date_create(string $time = "now", ?DateTimeZone $timezone = null): DateTime|false {}

function date_create_immutable(
    string $time = "now", ?DateTimeZone $timezone = null): DateTimeImmutable|false {}

function date_create_from_format(
    string $format, string $time, ?DateTimeZone $timezone = null): DateTime|false {}

function date_create_immutable_from_format(
    string $format, string $time, ?DateTimeZone $timezone = null): DateTimeImmutable|false {}

function date_parse(string $date): array {}

function date_parse_from_format(string $format, string $date): array {}

function date_get_last_errors(): array|false {}

function date_format(DateTimeInterface $object, string $format): string {}

function date_modify(DateTime $object, string $modify): DateTime|false {}

function date_add(DateTime $object, DateInterval $interval): DateTime {}

function date_sub(DateTime $object, DateInterval $interval): DateTime {}

function date_timezone_get(DateTimeInterface $object): DateTimeZone|false {}

function date_timezone_set(DateTimeInterface $object, DateTimeZone $timezone): DateTime {}

function date_offset_get(DateTimeInterface $object): int {}

function date_diff(
    DateTimeInterface $object, DateTimeInterface $object2, bool $absolute = false): DateInterval {}

function date_time_set(
    DateTime $object, int $hour, int $minute, int $second = 0, int $microseconds = 0): DateTime {}

function date_date_set(DateTime $object, int $year, int $month, int $day): DateTime {}

function date_isodate_set(DateTime $object, int $year, int $week, int $day = 1): DateTime {}

function date_timestamp_set(DateTime $object, int $timestamp): DateTime {}

function date_timestamp_get(DateTimeInterface $object): int|false {}

function timezone_open(string $timezone): DateTimeZone|false {}

function timezone_name_get(DateTimeZone $object): string {}

function timezone_name_from_abbr(string $abbr, int $gmtoffset = -1, int $isdst = -1): string|false {}

function timezone_offset_get(DateTimeZone $object, DateTimeInterface $datetime): int {}

function timezone_transitions_get(
    DateTimeZone $object, int $timestamp_begin = PHP_INT_MIN, int $timestamp_end = PHP_INT_MAX): array|false {}

function timezone_location_get(DateTimeZone $object): array|false {}

function timezone_identifiers_list(int $what = DateTimeZone::ALL, ?string $country = null): array|false {}

function timezone_abbreviations_list(): array {}

function timezone_version_get(): string {}

function date_interval_create_from_date_string(string $time): DateInterval|false {}

function date_interval_format(DateInterval $object, string $format): string {}

function date_default_timezone_set(string $timezone_identifier): bool {}

function date_default_timezone_get(): string {}

function date_sunrise(
    int $time, int $retformat = SUNFUNCS_RET_STRING,
    float $latitude = UNKNOWN, float $longitude = UNKNOWN, float $zenith = UNKNOWN,
    float $gmt_offset = 0): string|int|float|false {}

function date_sunset(
    int $time, int $retformat = SUNFUNCS_RET_STRING,
    float $latitude = UNKNOWN, float $longitude = UNKNOWN, float $zenith = UNKNOWN,
    float $gmt_offset = 0): string|int|float|false {}

function date_sun_info(int $time, float $latitude, float $longitude): array {}

// NB: Adding return types to methods is a BC break!
// For now only using @return annotations here.

interface DateTimeInterface {
    /** @return string */
    public function format(string $format);

    /** @return DateTimeZone|false */
    public function getTimezone();

    /** @return int|false */
    public function getOffset();

    /** @return int|false */
    public function getTimestamp();

    /** @return DateInterval|false */
    public function diff(DateTimeInterface $object, bool $absolute = false);

    public function __wakeup();
}

class DateTime implements DateTimeInterface {
    public function __construct(string $time = "now", ?DateTimeZone $timezone = null);

    /** @return DateTime */
    public static function __set_state(array $array);

    /** @return DateTime */
    public static function createFromImmutable(DateTimeImmutable $object);

    /** @return DateTime|false */
    public static function createFromFormat(
        string $format, string $time, ?DateTimeZone $timezone = null);

    /** @return array|false */
    public static function getLastErrors();

    /** @return DateTime|false */
    public function modify(string $modify);

    /** @return DateTime */
    public function add(DateInterval $interval);

    /** @return DateTime */
    public function sub(DateInterval $interval);

    /** @return DateTime */
    public function setTimezone(DateTimeZone $timezone);

    /** @return DateTime */
    public function setTime(int $hour, int $minute, int $second = 0, int $microseconds = 0);

    /** @return DateTime */
    public function setDate(int $year, int $month, int $day);

    /** @return DateTime */
    public function setISODate(int $year, int $week, int $day = 1);

    /** @return DateTime */
    public function setTimestamp(int $timestamp);
}

class DateTimeImmutable implements DateTimeInterface {
    public function __construct(string $time = "now", ?DateTimeZone $timezone = null);

    /** @return DateTimeZone */
    public static function __set_state(array $array);

    /** @return DateTimeImmutable */
    public static function createFromMutable(DateTime $object);

    /** @return DateTimeImmutable|false */
    public static function createFromFormat(
        string $format, string $time, ?DateTimeZone $timezone = null);

    /** @return array|false */
    public static function getLastErrors();

    /** @return DateTimeImmutable|false */
    public function modify(string $modify);

    /** @return DateTimeImmutable */
    public function add(DateInterval $interval);

    /** @return DateTimeImmutable */
    public function sub(DateInterval $interval);

    /** @return DateTimeImmutable */
    public function setTimezone(DateTimeZone $timezone);

    /** @return DateTimeImmutable */
    public function setTime(int $hour, int $minute, int $second = 0, int $microseconds = 0);

    /** @return DateTimeImmutable */
    public function setDate(int $year, int $month, int $day);

    /** @return DateTimeImmutable */
    public function setISODate(int $year, int $week, int $day = 1);

    /** @return DateTimeImmutable */
    public function setTimestamp(int $timestamp);
}

class DateTimeZone {
    public function __construct(string $timezone);

    /** @return string */
    public function getName();

    /** @return int */
    public function getOffset(DateTimeInterface $datetime);

    /** @return array|false */
    public function getTransitions(
        int $timestamp_begin = PHP_INT_MIN, int $timestamp_end = PHP_INT_MAX);

    /** @return array|false */
    public function getLocation();

    /** @return array */
    public static function listAbbreviations();

    /** @return array|false */
    public static function listIdentifiers(int $what = DateTimeZone::ALL, ?string $country = null);

    public function __wakeup();

    /** @return DateTimeZone */
    public static function __set_state(array $array);
}

class DateInterval {
    public function __construct(string $interval_spec);

    /** @return DateInterval|false */
    public static function createFromDateString(string $time);

    /** @return string */
    public function format(string $format);

    public function __wakeup();

    /** @return DateInterval */
    public static function __set_state(array $array);
}

class DatePeriod implements Traversable {
    /* Has an overloaded signature */
    public function __construct($start, $interval = UNKNOWN, $end = UNKNOWN);

    /** @return DateTimeInterface */
    public function getStartDate();

    /** @return DateTimeInterface|null */
    public function getEndDate();

    /** @return DateInterval */
    public function getDateInterval();

    /** @return int|null */
    public function getRecurrences();

    public function __wakeup();

    /** @return DatePeriod */
    public static function __set_state(array $array);
}
