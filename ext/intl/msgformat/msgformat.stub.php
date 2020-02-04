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

function msgfmt_create(string $locale, string $pattern): ?MessageFormatter {}

function msgfmt_format(MessageFormatter $fmt, array $args): string|false {}

function msgfmt_format_message(string $locale, string $pattern, array $args): string|false {}

function msgfmt_parse(MessageFormatter $fmt, string $value): array|false {}

function msgfmt_parse_message(string $locale, string $pattern, string $source): array|false {}

function msgfmt_set_pattern(MessageFormatter $fmt, string $pattern): bool {}

function msgfmt_get_pattern(MessageFormatter $fmt): string|false {}

function msgfmt_get_locale(MessageFormatter $fmt): string {}

function msgfmt_get_error_code(MessageFormatter $fmt): int {}

function msgfmt_get_error_message(MessageFormatter $fmt): string {}
