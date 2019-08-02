<?php

function strtotime(string $time, int $now = UNKNOWN) {}

// TODO: Finish me (free functions)

// NB: Adding return types to methods is a BC break!
// For now only using @return annotations here.

interface DateTimeInterface {
    /** @return string */
    public function format(string $format);

    /** @return DateTimeZone|false */
    public function getTimezone();

    /** @return int */
    public function getOffset();

    /** @return int */
    public function getTimestamp();

    /** @return DateInterval */
    public function diff(DateTimeInterface $object, bool $absolute = false);

    public function __wakeup();
}

class DateTime implements DateTimeInterface {
    public function __construct(string $time, ?DateTimeZone $timezone = null);

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

    // TODO: Finish me
}

class DateTimeImmutable implements DateTimeInterface {
    // TODO: Finish me
}

// TODO: Finish me (classes)
