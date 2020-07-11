<?php

/** @generate-function-entries */

class NumberFormatter
{
    public function __construct(string $locale, int $style, string $pattern = "") {}

    /**
     * @return NumberFormatter|null
     * @alias numfmt_create
     */
    public static function create(string $locale, int $style, string $pattern = "") {}

    /**
     * @return string|false
     * @alias numfmt_format
     */
    public function format(int|float $value, int $type = NumberFormatter::TYPE_DEFAULT) {}

    /**
     * @return int|float|false
     * @alias numfmt_parse
     */
    public function parse(string $value, int $type = NumberFormatter::TYPE_DOUBLE, &$position = null) {}

    /**
     * @return string|false
     * @alias numfmt_format_currency
     */
    public function formatCurrency(float $value, string $currency) {}

    /**
     * @return float|false
     * @alias numfmt_parse_currency
     */
    public function parseCurrency(string $value, &$currency, &$position = null) {}

    /**
     * @param int|float $value
     * @return bool
     * @alias numfmt_set_attribute
     */
    public function setAttribute(int $attr, $value) {}

    /**
     * @return int|float|false
     * @alias numfmt_get_attribute
     */
    public function getAttribute(int $attr) {}

    /**
     * @return bool
     * @alias numfmt_set_text_attribute
     */
    public function setTextAttribute(int $attr, string $value) {}

    /**
     * @return string|false
     * @alias numfmt_get_text_attribute
     */
    public function getTextAttribute(int $attr) {}

    /**
     * @return bool
     * @alias numfmt_set_symbol
     */
    public function setSymbol(int $attr, string $value) {}

    /**
     * @return string|false
     * @alias numfmt_get_symbol
     */
    public function getSymbol(int $attr) {}

    /**
     * @return bool
     * @alias numfmt_set_pattern
     */
    public function setPattern(string $pattern) {}

    /**
     * @return string|false
     * @alias numfmt_get_pattern
     */
    public function getPattern() {}

    /**
     * @return string|false
     * @alias numfmt_get_locale
     */
    public function getLocale(int $type = ULOC_ACTUAL_LOCALE) {}

    /**
     * @return int
     * @alias numfmt_get_error_code
     */
    public function getErrorCode() {}

    /**
     * @return string
     * @alias numfmt_get_error_message
     */
    public function getErrorMessage() {}
}
