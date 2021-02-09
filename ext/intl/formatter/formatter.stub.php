<?php

/** @generate-function-entries */

class NumberFormatter
{
    public function __construct(string $locale, int $style, ?string $pattern = null) {}

    /**
     * @return NumberFormatter|null
     * @alias numfmt_create
     */
    public static function create(string $locale, int $style, ?string $pattern = null) {}

    /**
     * @return string|false
     * @alias numfmt_format
     */
    public function format(int|float $num, int $type = NumberFormatter::TYPE_DEFAULT) {}

    /**
     * @param int $offset
     * @return int|float|false
     * @alias numfmt_parse
     */
    public function parse(string $string, int $type = NumberFormatter::TYPE_DOUBLE, &$offset = null) {}

    /**
     * @return string|false
     * @alias numfmt_format_currency
     */
    public function formatCurrency(float $amount, string $currency) {}

    /**
     * @param string $currency
     * @param int $offset
     * @return float|false
     * @alias numfmt_parse_currency
     */
    public function parseCurrency(string $string, &$currency, &$offset = null) {}

    /**
     * @return bool
     * @alias numfmt_set_attribute
     */
    public function setAttribute(int $attribute, int|float $value) {}

    /**
     * @return int|float|false
     * @alias numfmt_get_attribute
     */
    public function getAttribute(int $attribute) {}

    /**
     * @return bool
     * @alias numfmt_set_text_attribute
     */
    public function setTextAttribute(int $attribute, string $value) {}

    /**
     * @return string|false
     * @alias numfmt_get_text_attribute
     */
    public function getTextAttribute(int $attribute) {}

    /**
     * @return bool
     * @alias numfmt_set_symbol
     */
    public function setSymbol(int $symbol, string $value) {}

    /**
     * @return string|false
     * @alias numfmt_get_symbol
     */
    public function getSymbol(int $symbol) {}

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
