<?php

/** @generate-class-entries */

/** @not-serializable */
class NumberFormatter
{
    /* UNumberFormatStyle constants */

    /** @cvalue UNUM_PATTERN_DECIMAL */
    public const int PATTERN_DECIMAL = UNKNOWN;
    /** @cvalue UNUM_DECIMAL */
    public const int DECIMAL = UNKNOWN;
    /** @cvalue UNUM_CURRENCY */
    public const int CURRENCY = UNKNOWN;
    /** @cvalue UNUM_PERCENT */
    public const int PERCENT = UNKNOWN;
    /** @cvalue UNUM_SCIENTIFIC */
    public const int SCIENTIFIC = UNKNOWN;
    /** @cvalue UNUM_SPELLOUT */
    public const int SPELLOUT = UNKNOWN;
    /** @cvalue UNUM_ORDINAL */
    public const int ORDINAL = UNKNOWN;
    /** @cvalue UNUM_DURATION */
    public const int DURATION = UNKNOWN;
    /** @cvalue UNUM_PATTERN_RULEBASED */
    public const int PATTERN_RULEBASED = UNKNOWN;
    /** @cvalue UNUM_IGNORE */
    public const int IGNORE = UNKNOWN;
#if U_ICU_VERSION_MAJOR_NUM >= 53
    /** @cvalue UNUM_CURRENCY_ACCOUNTING */
    public const int CURRENCY_ACCOUNTING = UNKNOWN;
#endif
    /** @cvalue UNUM_DEFAULT */
    public const int DEFAULT_STYLE = UNKNOWN;

    /* UNumberFormatRoundingMode */

    /** @cvalue UNUM_ROUND_CEILING */
    public const int ROUND_CEILING = UNKNOWN;
    /** @cvalue UNUM_ROUND_FLOOR */
    public const int ROUND_FLOOR = UNKNOWN;
    /** @cvalue UNUM_ROUND_DOWN */
    public const int ROUND_DOWN = UNKNOWN;
    /** @cvalue UNUM_ROUND_UP */
    public const int ROUND_UP = UNKNOWN;
    /** @cvalue UNUM_ROUND_HALFEVEN */
    public const int ROUND_HALFEVEN = UNKNOWN;
    /** @cvalue UNUM_ROUND_HALFDOWN */
    public const int ROUND_HALFDOWN = UNKNOWN;
    /** @cvalue UNUM_ROUND_HALFUP */
    public const int ROUND_HALFUP = UNKNOWN;

    /* UNumberFormatPadPosition */

    /** @cvalue UNUM_PAD_BEFORE_PREFIX */
    public const int PAD_BEFORE_PREFIX = UNKNOWN;
    /** @cvalue UNUM_PAD_AFTER_PREFIX */
    public const int PAD_AFTER_PREFIX = UNKNOWN;
    /** @cvalue UNUM_PAD_BEFORE_SUFFIX */
    public const int PAD_BEFORE_SUFFIX = UNKNOWN;
    /** @cvalue UNUM_PAD_AFTER_SUFFIX */
    public const int PAD_AFTER_SUFFIX = UNKNOWN;

    /* UNumberFormatAttribute */

    /** @cvalue UNUM_PARSE_INT_ONLY */
    public const int PARSE_INT_ONLY = UNKNOWN;
    /** @cvalue UNUM_GROUPING_USED */
    public const int GROUPING_USED = UNKNOWN;
    /** @cvalue UNUM_DECIMAL_ALWAYS_SHOWN */
    public const int DECIMAL_ALWAYS_SHOWN = UNKNOWN;
    /** @cvalue UNUM_MAX_INTEGER_DIGITS */
    public const int MAX_INTEGER_DIGITS = UNKNOWN;
    /** @cvalue UNUM_MIN_INTEGER_DIGITS */
    public const int MIN_INTEGER_DIGITS = UNKNOWN;
    /** @cvalue UNUM_INTEGER_DIGITS */
    public const int INTEGER_DIGITS = UNKNOWN;
    /** @cvalue UNUM_MAX_FRACTION_DIGITS */
    public const int MAX_FRACTION_DIGITS = UNKNOWN;
    /** @cvalue UNUM_MIN_FRACTION_DIGITS */
    public const int MIN_FRACTION_DIGITS = UNKNOWN;
    /** @cvalue UNUM_FRACTION_DIGITS */
    public const int FRACTION_DIGITS = UNKNOWN;
    /** @cvalue UNUM_MULTIPLIER */
    public const int MULTIPLIER = UNKNOWN;
    /** @cvalue UNUM_GROUPING_SIZE */
    public const int GROUPING_SIZE = UNKNOWN;
    /** @cvalue UNUM_ROUNDING_MODE */
    public const int ROUNDING_MODE = UNKNOWN;
    /** @cvalue UNUM_ROUNDING_INCREMENT */
    public const int ROUNDING_INCREMENT = UNKNOWN;
    /** @cvalue UNUM_FORMAT_WIDTH */
    public const int FORMAT_WIDTH = UNKNOWN;
    /** @cvalue UNUM_PADDING_POSITION */
    public const int PADDING_POSITION = UNKNOWN;
    /** @cvalue UNUM_SECONDARY_GROUPING_SIZE */
    public const int SECONDARY_GROUPING_SIZE = UNKNOWN;
    /** @cvalue UNUM_SIGNIFICANT_DIGITS_USED */
    public const int SIGNIFICANT_DIGITS_USED = UNKNOWN;
    /** @cvalue UNUM_MIN_SIGNIFICANT_DIGITS */
    public const int MIN_SIGNIFICANT_DIGITS = UNKNOWN;
    /** @cvalue UNUM_MAX_SIGNIFICANT_DIGITS */
    public const int MAX_SIGNIFICANT_DIGITS = UNKNOWN;
    /** @cvalue UNUM_LENIENT_PARSE */
    public const int LENIENT_PARSE = UNKNOWN;

    /* UNumberFormatTextAttribute */

    /** @cvalue UNUM_POSITIVE_PREFIX */
    public const int POSITIVE_PREFIX = UNKNOWN;
    /** @cvalue UNUM_POSITIVE_SUFFIX */
    public const int POSITIVE_SUFFIX = UNKNOWN;
    /** @cvalue UNUM_NEGATIVE_PREFIX */
    public const int NEGATIVE_PREFIX = UNKNOWN;
    /** @cvalue UNUM_NEGATIVE_SUFFIX */
    public const int NEGATIVE_SUFFIX = UNKNOWN;
    /** @cvalue UNUM_PADDING_CHARACTER */
    public const int PADDING_CHARACTER = UNKNOWN;
    /** @cvalue UNUM_CURRENCY_CODE */
    public const int CURRENCY_CODE = UNKNOWN;
    /** @cvalue UNUM_DEFAULT_RULESET */
    public const int DEFAULT_RULESET = UNKNOWN;
    /** @cvalue UNUM_PUBLIC_RULESETS */
    public const int PUBLIC_RULESETS = UNKNOWN;

    /* UNumberFormatSymbol */

    /** @cvalue UNUM_DECIMAL_SEPARATOR_SYMBOL */
    public const int DECIMAL_SEPARATOR_SYMBOL = UNKNOWN;
    /** @cvalue UNUM_GROUPING_SEPARATOR_SYMBOL */
    public const int GROUPING_SEPARATOR_SYMBOL = UNKNOWN;
    /** @cvalue UNUM_PATTERN_SEPARATOR_SYMBOL */
    public const int PATTERN_SEPARATOR_SYMBOL = UNKNOWN;
    /** @cvalue UNUM_PERCENT_SYMBOL */
    public const int PERCENT_SYMBOL = UNKNOWN;
    /** @cvalue UNUM_ZERO_DIGIT_SYMBOL */
    public const int ZERO_DIGIT_SYMBOL = UNKNOWN;
    /** @cvalue UNUM_DIGIT_SYMBOL */
    public const int DIGIT_SYMBOL = UNKNOWN;
    /** @cvalue UNUM_MINUS_SIGN_SYMBOL */
    public const int MINUS_SIGN_SYMBOL = UNKNOWN;
    /** @cvalue UNUM_PLUS_SIGN_SYMBOL */
    public const int PLUS_SIGN_SYMBOL = UNKNOWN;
    /** @cvalue UNUM_CURRENCY_SYMBOL */
    public const int CURRENCY_SYMBOL = UNKNOWN;
    /** @cvalue UNUM_INTL_CURRENCY_SYMBOL */
    public const int INTL_CURRENCY_SYMBOL = UNKNOWN;
    /** @cvalue UNUM_MONETARY_SEPARATOR_SYMBOL */
    public const int MONETARY_SEPARATOR_SYMBOL = UNKNOWN;
    /** @cvalue UNUM_EXPONENTIAL_SYMBOL */
    public const int EXPONENTIAL_SYMBOL = UNKNOWN;
    /** @cvalue UNUM_PERMILL_SYMBOL */
    public const int PERMILL_SYMBOL = UNKNOWN;
    /** @cvalue UNUM_PAD_ESCAPE_SYMBOL */
    public const int PAD_ESCAPE_SYMBOL = UNKNOWN;
    /** @cvalue UNUM_INFINITY_SYMBOL */
    public const int INFINITY_SYMBOL = UNKNOWN;
    /** @cvalue UNUM_NAN_SYMBOL */
    public const int NAN_SYMBOL = UNKNOWN;
    /** @cvalue UNUM_SIGNIFICANT_DIGIT_SYMBOL */
    public const int SIGNIFICANT_DIGIT_SYMBOL = UNKNOWN;
    /** @cvalue UNUM_MONETARY_GROUPING_SEPARATOR_SYMBOL */
    public const int MONETARY_GROUPING_SEPARATOR_SYMBOL = UNKNOWN;

    /** @cvalue FORMAT_TYPE_DEFAULT */
    public const int TYPE_DEFAULT = UNKNOWN;
    /** @cvalue FORMAT_TYPE_INT32 */
    public const int TYPE_INT32 = UNKNOWN;
    /** @cvalue FORMAT_TYPE_INT64 */
    public const int TYPE_INT64 = UNKNOWN;
    /** @cvalue FORMAT_TYPE_DOUBLE */
    public const int TYPE_DOUBLE = UNKNOWN;
    /**
     * @deprecated
     * @cvalue FORMAT_TYPE_CURRENCY
     */
    public const int TYPE_CURRENCY = UNKNOWN;

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
