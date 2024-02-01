<?php

/** @generate-class-entries */

/** @not-serializable */
class NumberFormatter
{
    /* UNumberFormatStyle constants */

    /**
     * @var int
     * @cvalue UNUM_PATTERN_DECIMAL
     */
    public const PATTERN_DECIMAL = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_DECIMAL
     */
    public const DECIMAL = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_CURRENCY
     */
    public const CURRENCY = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_PERCENT
     */
    public const PERCENT = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_SCIENTIFIC
     */
    public const SCIENTIFIC = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_SPELLOUT
     */
    public const SPELLOUT = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_ORDINAL
     */
    public const ORDINAL = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_DURATION
     */
    public const DURATION = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_PATTERN_RULEBASED
     */
    public const PATTERN_RULEBASED = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_IGNORE
     */
    public const IGNORE = UNKNOWN;
#if U_ICU_VERSION_MAJOR_NUM >= 53
    /**
     * @var int
     * @cvalue UNUM_CURRENCY_ACCOUNTING
     */
    public const CURRENCY_ACCOUNTING = UNKNOWN;
#endif
    /**
     * @var int
     * @cvalue UNUM_DEFAULT
     */
    public const DEFAULT_STYLE = UNKNOWN;

    /* UNumberFormatRoundingMode */

    /**
     * @var int
     * @cvalue UNUM_ROUND_CEILING
     */
    public const ROUND_CEILING = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_ROUND_FLOOR
     */
    public const ROUND_FLOOR = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_ROUND_DOWN
     */
    public const ROUND_DOWN = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_ROUND_UP
     */
    public const ROUND_UP = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_ROUND_HALFEVEN
     */
    public const ROUND_HALFEVEN = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_ROUND_HALFDOWN
     */
    public const ROUND_HALFDOWN = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_ROUND_HALFUP
     */
    public const ROUND_HALFUP = UNKNOWN;

    /* UNumberFormatPadPosition */

    /**
     * @var int
     * @cvalue UNUM_PAD_BEFORE_PREFIX
     */
    public const PAD_BEFORE_PREFIX = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_PAD_AFTER_PREFIX
     */
    public const PAD_AFTER_PREFIX = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_PAD_BEFORE_SUFFIX
     */
    public const PAD_BEFORE_SUFFIX = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_PAD_AFTER_SUFFIX
     */
    public const PAD_AFTER_SUFFIX = UNKNOWN;

    /* UNumberFormatAttribute */

    /**
     * @var int
     * @cvalue UNUM_PARSE_INT_ONLY
     */
    public const PARSE_INT_ONLY = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_GROUPING_USED
     */
    public const GROUPING_USED = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_DECIMAL_ALWAYS_SHOWN
     */
    public const DECIMAL_ALWAYS_SHOWN = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_MAX_INTEGER_DIGITS
     */
    public const MAX_INTEGER_DIGITS = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_MIN_INTEGER_DIGITS
     */
    public const MIN_INTEGER_DIGITS = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_INTEGER_DIGITS
     */
    public const INTEGER_DIGITS = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_MAX_FRACTION_DIGITS
     */
    public const MAX_FRACTION_DIGITS = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_MIN_FRACTION_DIGITS
     */
    public const MIN_FRACTION_DIGITS = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_FRACTION_DIGITS
     */
    public const FRACTION_DIGITS = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_MULTIPLIER
     */
    public const MULTIPLIER = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_GROUPING_SIZE
     */
    public const GROUPING_SIZE = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_ROUNDING_MODE
     */
    public const ROUNDING_MODE = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_ROUNDING_INCREMENT
     */
    public const ROUNDING_INCREMENT = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_FORMAT_WIDTH
     */
    public const FORMAT_WIDTH = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_PADDING_POSITION
     */
    public const PADDING_POSITION = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_SECONDARY_GROUPING_SIZE
     */
    public const SECONDARY_GROUPING_SIZE = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_SIGNIFICANT_DIGITS_USED
     */
    public const SIGNIFICANT_DIGITS_USED = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_MIN_SIGNIFICANT_DIGITS
     */
    public const MIN_SIGNIFICANT_DIGITS = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_MAX_SIGNIFICANT_DIGITS
     */
    public const MAX_SIGNIFICANT_DIGITS = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_LENIENT_PARSE
     */
    public const LENIENT_PARSE = UNKNOWN;

    /* UNumberFormatTextAttribute */

    /**
     * @var int
     * @cvalue UNUM_POSITIVE_PREFIX
     */
    public const POSITIVE_PREFIX = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_POSITIVE_SUFFIX
     */
    public const POSITIVE_SUFFIX = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_NEGATIVE_PREFIX
     */
    public const NEGATIVE_PREFIX = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_NEGATIVE_SUFFIX
     */
    public const NEGATIVE_SUFFIX = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_PADDING_CHARACTER
     */
    public const PADDING_CHARACTER = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_CURRENCY_CODE
     */
    public const CURRENCY_CODE = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_DEFAULT_RULESET
     */
    public const DEFAULT_RULESET = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_PUBLIC_RULESETS
     */
    public const PUBLIC_RULESETS = UNKNOWN;

    /* UNumberFormatSymbol */

    /**
     * @var int
     * @cvalue UNUM_DECIMAL_SEPARATOR_SYMBOL
     */
    public const DECIMAL_SEPARATOR_SYMBOL = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_GROUPING_SEPARATOR_SYMBOL
     */
    public const GROUPING_SEPARATOR_SYMBOL = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_PATTERN_SEPARATOR_SYMBOL
     */
    public const PATTERN_SEPARATOR_SYMBOL = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_PERCENT_SYMBOL
     */
    public const PERCENT_SYMBOL = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_ZERO_DIGIT_SYMBOL
     */
    public const ZERO_DIGIT_SYMBOL = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_DIGIT_SYMBOL
     */
    public const DIGIT_SYMBOL = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_MINUS_SIGN_SYMBOL
     */
    public const MINUS_SIGN_SYMBOL = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_PLUS_SIGN_SYMBOL
     */
    public const PLUS_SIGN_SYMBOL = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_CURRENCY_SYMBOL
     */
    public const CURRENCY_SYMBOL = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_INTL_CURRENCY_SYMBOL
     */
    public const INTL_CURRENCY_SYMBOL = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_MONETARY_SEPARATOR_SYMBOL
     */
    public const MONETARY_SEPARATOR_SYMBOL = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_EXPONENTIAL_SYMBOL
     */
    public const EXPONENTIAL_SYMBOL = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_PERMILL_SYMBOL
     */
    public const PERMILL_SYMBOL = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_PAD_ESCAPE_SYMBOL
     */
    public const PAD_ESCAPE_SYMBOL = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_INFINITY_SYMBOL
     */
    public const INFINITY_SYMBOL = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_NAN_SYMBOL
     */
    public const NAN_SYMBOL = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_SIGNIFICANT_DIGIT_SYMBOL
     */
    public const SIGNIFICANT_DIGIT_SYMBOL = UNKNOWN;
    /**
     * @var int
     * @cvalue UNUM_MONETARY_GROUPING_SEPARATOR_SYMBOL
     */
    public const MONETARY_GROUPING_SEPARATOR_SYMBOL = UNKNOWN;

    /**
     * @var int
     * @cvalue FORMAT_TYPE_DEFAULT
     */
    public const TYPE_DEFAULT = UNKNOWN;
    /**
     * @var int
     * @cvalue FORMAT_TYPE_INT32
     */
    public const TYPE_INT32 = UNKNOWN;
    /**
     * @var int
     * @cvalue FORMAT_TYPE_INT64
     */
    public const TYPE_INT64 = UNKNOWN;
    /**
     * @var int
     * @cvalue FORMAT_TYPE_DOUBLE
     */
    public const TYPE_DOUBLE = UNKNOWN;
    /**
     * @var int
     * @cvalue FORMAT_TYPE_CURRENCY
     */
    public const TYPE_CURRENCY = UNKNOWN;

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
