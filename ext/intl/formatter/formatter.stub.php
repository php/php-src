<?php

/** @generate-class-entries */

class NumberFormatter
{
    public function __construct(string $locale, int $style, ?string $pattern = null) {}

    /**
     * @tentative-return-type
     * @alias numfmt_create
     */
    public static function create(string $locale, int $style, ?string $pattern = null): ?NumberFormatter {}

    /**
     * @tentative-return-type
     * @alias numfmt_format
     */
    public function format(int|float $num, int $type = NumberFormatter::TYPE_DEFAULT): string|false {}

    /**
     * @param int $offset
     * @tentative-return-type
     * @alias numfmt_parse
     */
    public function parse(string $string, int $type = NumberFormatter::TYPE_DOUBLE, &$offset = null): int|float|false {}

    /**
     * @tentative-return-type
     * @alias numfmt_format_currency
     */
    public function formatCurrency(float $amount, string $currency): string|false {}

    /**
     * @param string $currency
     * @param int $offset
     * @tentative-return-type
     * @alias numfmt_parse_currency
     */
    public function parseCurrency(string $string, &$currency, &$offset = null): float|false {}

    /**
     * @tentative-return-type
     * @alias numfmt_set_attribute
     */
    public function setAttribute(int $attribute, int|float $value): bool {}

    /**
     * @tentative-return-type
     * @alias numfmt_get_attribute
     */
    public function getAttribute(int $attribute): int|float|false {}

    /**
     * @tentative-return-type
     * @alias numfmt_set_text_attribute
     */
    public function setTextAttribute(int $attribute, string $value): bool {}

    /**
     * @tentative-return-type
     * @alias numfmt_get_text_attribute
     */
    public function getTextAttribute(int $attribute): string|false {}

    /**
     * @tentative-return-type
     * @alias numfmt_set_symbol
     */
    public function setSymbol(int $symbol, string $value): bool {}

    /**
     * @tentative-return-type
     * @alias numfmt_get_symbol
     */
    public function getSymbol(int $symbol): string|false {}

    /**
     * @tentative-return-type
     * @alias numfmt_set_pattern
     */
    public function setPattern(string $pattern): bool {}

    /**
     * @tentative-return-type
     * @alias numfmt_get_pattern
     */
    public function getPattern(): string|false {}

    /**
     * @tentative-return-type
     * @alias numfmt_get_locale
     */
    public function getLocale(int $type = ULOC_ACTUAL_LOCALE): string|false {}

    /**
     * @tentative-return-type
     * @alias numfmt_get_error_code
     */
    public function getErrorCode(): int {}

    /**
     * @tentative-return-type
     * @alias numfmt_get_error_message
     */
    public function getErrorMessage(): string {}
}
