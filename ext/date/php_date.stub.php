<?php

// NB: Adding return types is a BC break!

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
}

class DateTimeImmutable implements DateTimeInterface {
}
