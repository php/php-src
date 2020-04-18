<?php

/** @generate-function-entries */

class MessageFormatter
{
    public function __construct(string $locale, string $pattern) {}

    /**
     * @return MessageFormatter|null
     * @alias msgfmt_create
     */
    public static function create(string $locale, string $pattern) {}

    /**
     * @return string|false
     * @alias msgfmt_format
     */
    public function format(array $args) {}

    /**
     * @return string|false
     * @alias msgfmt_format_message
     */
    public static function formatMessage(string $locale, string $pattern, array $args) {}

    /**
     * @return array|false
     * @alias msgfmt_parse
     */
    public function parse(string $value) {}

    /**
     * @return array|false
     * @alias msgfmt_parse_message
     */
    public static function parseMessage(string $locale, string $pattern, string $source) {}

    /**
     * @return bool
     * @alias msgfmt_set_pattern
     */
    public function setPattern(string $pattern) {}

    /**
     * @return string|false
     * @alias msgfmt_get_pattern
     */
    public function getPattern() {}

    /**
     * @return string
     * @alias msgfmt_get_locale
     */
    public function getLocale() {}

    /**
     * @return int
     * @alias msgfmt_get_error_code
     */
    public function getErrorCode() {}

    /**
     * @return string
     * @alias msgfmt_get_error_message
     */
    public function getErrorMessage() {}
}
