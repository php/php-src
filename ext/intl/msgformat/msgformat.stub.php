<?php

/** @generate-class-entries */

/** @not-serializable */
class MessageFormatter
{
    public function __construct(string $locale, string $pattern) {}

    /**
     * @tentative-return-type
     * @alias msgfmt_create
     */
    public static function create(string $locale, string $pattern): ?MessageFormatter {}

    /**
     * @tentative-return-type
     * @alias msgfmt_format
     */
    public function format(array $values): string|false {}

    /**
     * @tentative-return-type
     * @alias msgfmt_format_message
     */
    public static function formatMessage(string $locale, string $pattern, array $values): string|false {}

    /**
     * @return array<int, int|float|string>|false
     * @tentative-return-type
     * @alias msgfmt_parse
     */
    public function parse(string $string): array|false {}

    /**
     * @return array<int, int|float|string>|false
     * @tentative-return-type
     * @alias msgfmt_parse_message
     */
    public static function parseMessage(string $locale, string $pattern, string $message): array|false {}

    /**
     * @tentative-return-type
     * @alias msgfmt_set_pattern
     */
    public function setPattern(string $pattern): bool {}

    /**
     * @tentative-return-type
     * @alias msgfmt_get_pattern
     */
    public function getPattern(): string|false {}

    /**
     * @tentative-return-type
     * @alias msgfmt_get_locale
     */
    public function getLocale(): string {}

    /**
     * @tentative-return-type
     * @alias msgfmt_get_error_code
     */
    public function getErrorCode(): int {}

    /**
     * @tentative-return-type
     * @alias msgfmt_get_error_message
     */
    public function getErrorMessage(): string {}
}
