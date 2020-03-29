<?php

class NumberFormatter
{
    public function __construct(string $locale, int $style, string $pattern = "") {}

    /** @return NumberFormatter|null */
    public static function create(string $locale, int $style, string $pattern = "") {}

    /** @return string|false */
    public function format(int|float $value, int $type = NumberFormatter::TYPE_DEFAULT) {}

    /** @return int|float|false */
    public function parse(string $value, int $type = NumberFormatter::TYPE_DOUBLE, &$position = null) {}

    /** @return string|false */
    public function formatCurrency(float $value, string $currency) {}

    /** @return float|false */
    public function parseCurrency(string $value, &$currency, &$position = null) {}

    /**
     * @param int|float $value
     * @return bool
     */
    public function setAttribute(int $attr, $value) {}

    /** @return int|float|false */
    public function getAttribute(int $attr) {}

    /** @return bool */
    public function setTextAttribute(int $attr, string $value) {}

    /** @return string|false */
    public function getTextAttribute(int $attr) {}

    /** @return bool */
    public function setSymbol(int $attr, string $value) {}

    /** @return string|false */
    public function getSymbol(int $attr) {}

    /** @return bool */
    public function setPattern(string $pattern) {}

    /** @return string|false */
    public function getPattern() {}

    /** @return string|false */
    public function getLocale(int $type = ULOC_ACTUAL_LOCALE) {}

    /** @return int */
    public function getErrorCode() {}

    /** @return string */
    public function getErrorMessage() {}
}
