<?php

class MessageFormatter
{
    public function __construct(string $locale, string $pattern) {}

    /** @return MessageFormatter|null */
    public static function create(string $locale, string $pattern) {}

    /** @return string|false */
    public function format(array $args) {}

    /** @return string|false */
    public static function formatMessage(string $locale, string $pattern, array $args) {}

    /** @return array|false */
    public function parse(string $value) {}

    /** @return array|false */
    public static function parseMessage(string $locale, string $pattern, string $source) {}

    /** @return bool */
    public function setPattern(string $pattern) {}

    /** @return string|false */
    public function getPattern() {}

    /** @return string */
    public function getLocale() {}

    /** @return int */
    public function getErrorCode() {}

    /** @return string */
    public function getErrorMessage() {}
}
