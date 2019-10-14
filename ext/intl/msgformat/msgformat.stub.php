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

    /** @return string|false */
    public function getLocale() {}

    /** @return int */
    public function getErrorCode() {}

    /** @return string */
    public function getErrorMessage() {}
}

function msgfmt_create(string $locale, string $pattern): ?MessageFormatter {}

/** @return string|false */
function msgfmt_format(MessageFormatter $fmt, array $args): string {}

/** @return string|false */
function msgfmt_format_message(string $locale, string $pattern, array $args) {}

/** @return array|false */
function msgfmt_parse(MessageFormatter $fmt, string $value) {}

/** @return array|false */
function msgfmt_parse_message(string $locale, string $pattern, string $source) {}

function msgfmt_set_pattern(MessageFormatter $fmt, string $pattern): bool {}

/** @return string|false */
function msgfmt_get_pattern(MessageFormatter $fmt) {}

/** @return string|false */
function msgfmt_get_locale(MessageFormatter $fmt) {}

function msgfmt_get_error_code(MessageFormatter $fmt): int {}

function msgfmt_get_error_message(MessageFormatter $fmt): string {}
