<?php

class NumberFormatter
{
    public function __construct(string $locale, int $style, string $pattern = "") {}

    /** @return NumberFormatter|null */
    public static function create(string $locale, int $style, string $pattern = "") {}

    /** @return string|false */
    public function format($value, int $type = NumberFormatter::TYPE_DEFAULT) {}

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

function numfmt_create(string $locale, int $style, string $pattern = ""): ?NumberFormatter {}

function numfmt_format(NumberFormatter $fmt, $value, int $type = NumberFormatter::TYPE_DEFAULT): string|false {}

function numfmt_parse(NumberFormatter $fmt, string $value, int $type = NumberFormatter::TYPE_DOUBLE, &$position = null): int|float|false {}

function numfmt_format_currency(NumberFormatter $fmt, float $value, string $currency): string|false {}

function numfmt_parse_currency(NumberFormatter $fmt, string $value, &$currency, &$position = null): float|false {}

/** @param int|float $value */
function numfmt_set_attribute(NumberFormatter $fmt, int $attr, $value): bool {}

function numfmt_get_attribute(NumberFormatter $fmt, int $attr): int|double|false {}

function numfmt_set_text_attribute(NumberFormatter $fmt, int $attr, string $value): bool {}

function numfmt_get_text_attribute(NumberFormatter $fmt, int $attr): string|false {}

function numfmt_set_symbol(NumberFormatter $fmt, int $attr, string $value): bool {}

function numfmt_get_symbol(NumberFormatter $fmt, int $attr): string|false {}

function numfmt_set_pattern(NumberFormatter $fmt, string $pattern): bool {}

function numfmt_get_pattern(NumberFormatter $fmt): string|false {}

function numfmt_get_locale(NumberFormatter $fmt, int $type = ULOC_ACTUAL_LOCALE): string|false {}

function numfmt_get_error_code(NumberFormatter $fmt): int {}

function numfmt_get_error_message(NumberFormatter $fmt): string {}
