<?php

/**
 * Number formatter class - locale-dependent number formatting/parsing.
 *
 * This class represents the ICU number formatting functionality. It allows to display
 * number according to the localized format or given pattern or set of rules, and to
 * parse strings into numbers according to the above patterns.
 *
 * Example:
 * <code>
 * $value = 1234567;
 * $formatter = new NumberFormatter("de_DE", NumberFormatter::DECIMAL);
 * echo $formatter->format($value);
 * </code>
 *
 * @see http://www.icu-project.org/apiref/icu4c/unum_8h.html
 * @see http://www.icu-project.org/apiref/icu4c/classNumberFormat.html
 *
 * The class would also contain all the constants listed in the following enums:
 * UNumberFormatStyle, UNumberFormatRoundingMode, UNumberFormatPadPosition,
 * UNumberFormatAttribute, UNumberFormatTextAttribute, UNumberFormatSymbol.
 */
class NumberFormatter {
#############################################################################
# Common constants.
#############################################################################

	/*
	 * WARNING:
	 * The values described here are NOT the actual values in PHP code.
	 * They are references to the ICU C definitions, so the line
	 *    const PATTERN_DECIMAL = 'UNUM_PATTERN_DECIMAL';
	 * actually means that NumberFormatter::PATTERN_DECIMAL is the same as
	 * UNUM_PATTERN_DECIMAL constant in the ICU library.
	 */

	/*
	 * These constants define formatter/parser argument type - integer, floating point or currency.
	 */
	const TYPE_DEFAULT     = 'FORMAT_TYPE_DEFAULT';
	const TYPE_INT32       = 'FORMAT_TYPE_INT32';
	const TYPE_INT64       = 'FORMAT_TYPE_INT64';
	const TYPE_DOUBLE      = 'FORMAT_TYPE_DOUBLE';
	const TYPE_CURRENCY    = 'FORMAT_TYPE_CURRENCY';

	/*
	 * UNumberFormatStyle constants
	 */
	const PATTERN_DECIMAL   = 'UNUM_PATTERN_DECIMAL';
	const DECIMAL           = 'UNUM_DECIMAL';
	const CURRENCY          = 'UNUM_CURRENCY';
	const PERCENT           = 'UNUM_PERCENT';
	const SCIENTIFIC        = 'UNUM_SCIENTIFIC';
	const SPELLOUT          = 'UNUM_SPELLOUT';
	const ORDINAL           = 'UNUM_ORDINAL';
	const DURATION          = 'UNUM_DURATION';
	const PATTERN_RULEBASED = 'UNUM_PATTERN_RULEBASED';
	const DEFAULT           = 'UNUM_DEFAULT';
	const IGNORE            = 'UNUM_IGNORE';

	/*
	 * UNumberFormatRoundingMode
	 */
	const ROUND_CEILING  = 'UNUM_ROUND_CEILING';
	const ROUND_FLOOR    = 'UNUM_ROUND_FLOOR';
	const ROUND_DOWN     = 'UNUM_ROUND_DOWN';
	const ROUND_UP       = 'UNUM_ROUND_UP';
	const ROUND_HALFEVEN = 'UNUM_ROUND_HALFEVEN';
	const ROUND_HALFDOWN = 'UNUM_ROUND_HALFDOWN';
	const ROUND_HALFUP   = 'UNUM_ROUND_HALFUP';

	/*
	 * UNumberFormatPadPosition
	 */
	const PAD_BEFORE_PREFIX = 'UNUM_PAD_BEFORE_PREFIX';
	const PAD_AFTER_PREFIX  = 'UNUM_PAD_AFTER_PREFIX';
	const PAD_BEFORE_SUFFIX = 'UNUM_PAD_BEFORE_SUFFIX';
	const PAD_AFTER_SUFFIX  = 'UNUM_PAD_AFTER_SUFFIX';

	/*
	 * UNumberFormatAttribute
	 */
	const PARSE_INT_ONLY          = 'UNUM_PARSE_INT_ONLY';
	const GROUPING_USED           = 'UNUM_GROUPING_USED';
	const DECIMAL_ALWAYS_SHOWN    = 'UNUM_DECIMAL_ALWAYS_SHOWN';
	const MAX_INTEGER_DIGITS      = 'UNUM_MAX_INTEGER_DIGITS';
	const MIN_INTEGER_DIGITS      = 'UNUM_MIN_INTEGER_DIGITS';
	const INTEGER_DIGITS          = 'UNUM_INTEGER_DIGITS';
	const MAX_FRACTION_DIGITS     = 'UNUM_MAX_FRACTION_DIGITS';
	const MIN_FRACTION_DIGITS     = 'UNUM_MIN_FRACTION_DIGITS';
	const FRACTION_DIGITS         = 'UNUM_FRACTION_DIGITS';
	const MULTIPLIER              = 'UNUM_MULTIPLIER';
	const GROUPING_SIZE           = 'UNUM_GROUPING_SIZE';
	const ROUNDING_MODE           = 'UNUM_ROUNDING_MODE';
	const ROUNDING_INCREMENT      = 'UNUM_ROUNDING_INCREMENT';
	const FORMAT_WIDTH            = 'UNUM_FORMAT_WIDTH';
	const PADDING_POSITION        = 'UNUM_PADDING_POSITION';
	const SECONDARY_GROUPING_SIZE = 'UNUM_SECONDARY_GROUPING_SIZE';
	const SIGNIFICANT_DIGITS_USED = 'UNUM_SIGNIFICANT_DIGITS_USED';
	const MIN_SIGNIFICANT_DIGITS  = 'UNUM_MIN_SIGNIFICANT_DIGITS';
	const MAX_SIGNIFICANT_DIGITS  = 'UNUM_MAX_SIGNIFICANT_DIGITS';
	const LENIENT_PARSE           = 'UNUM_LENIENT_PARSE';

	/*
	 * UNumberFormatTextAttribute
	 */
	const POSITIVE_PREFIX   = 'UNUM_POSITIVE_PREFIX';
	const POSITIVE_SUFFIX   = 'UNUM_POSITIVE_SUFFIX';
	const NEGATIVE_PREFIX   = 'UNUM_NEGATIVE_PREFIX';
	const NEGATIVE_SUFFIX   = 'UNUM_NEGATIVE_SUFFIX';
	const PADDING_CHARACTER = 'UNUM_PADDING_CHARACTER';
	const CURRENCY_CODE     = 'UNUM_CURRENCY_CODE';
	const DEFAULT_RULESET   = 'UNUM_DEFAULT_RULESET';
	const PUBLIC_RULESETS   = 'UNUM_PUBLIC_RULESETS';

	/*
	 * UNumberFormatSymbol
	 */
	const DECIMAL_SEPARATOR_SYMBOL           = 'UNUM_DECIMAL_SEPARATOR_SYMBOL';
	const GROUPING_SEPARATOR_SYMBOL          = 'UNUM_GROUPING_SEPARATOR_SYMBOL';
	const PATTERN_SEPARATOR_SYMBOL           = 'UNUM_PATTERN_SEPARATOR_SYMBOL';
	const PERCENT_SYMBOL                     = 'UNUM_PERCENT_SYMBOL';
	const ZERO_DIGIT_SYMBOL                  = 'UNUM_ZERO_DIGIT_SYMBOL';
	const DIGIT_SYMBOL                       = 'UNUM_DIGIT_SYMBOL';
	const MINUS_SIGN_SYMBOL                  = 'UNUM_MINUS_SIGN_SYMBOL';
	const PLUS_SIGN_SYMBOL                   = 'UNUM_PLUS_SIGN_SYMBOL';
	const CURRENCY_SYMBOL                    = 'UNUM_CURRENCY_SYMBOL';
	const INTL_CURRENCY_SYMBOL               = 'UNUM_INTL_CURRENCY_SYMBOL';
	const MONETARY_SEPARATOR_SYMBOL          = 'UNUM_MONETARY_SEPARATOR_SYMBOL';
	const EXPONENTIAL_SYMBOL                 = 'UNUM_EXPONENTIAL_SYMBOL';
	const PERMILL_SYMBOL                     = 'UNUM_PERMILL_SYMBOL';
	const PAD_ESCAPE_SYMBOL                  = 'UNUM_PAD_ESCAPE_SYMBOL';
	const INFINITY_SYMBOL                    = 'UNUM_INFINITY_SYMBOL';
	const NAN_SYMBOL                         = 'UNUM_NAN_SYMBOL';
	const SIGNIFICANT_DIGIT_SYMBOL           = 'UNUM_SIGNIFICANT_DIGIT_SYMBOL';
	const MONETARY_GROUPING_SEPARATOR_SYMBOL = 'UNUM_MONETARY_GROUPING_SEPARATOR_SYMBOL';

	/**
	 * Create a number formatter
	 *
	 * Creates a number formatter from locale and pattern. This formatter would be used to
	 * format or parse numbers.
	 *
	 * @param integer $style     Style of the formatting, one of the UNumberFormatStyle constants
	 * @param string $locale     Locale in which the number would be formatted
	 * @param [string] $pattern  Pattern string in case chose style requires pattern
	 * @return NumberFormatter
	 */
	public function __construct($locale, $style, $pattern = null) {}

	/**
	 * Create a number formatter
	 *
	 * Creates a number formatter from locale and pattern. This formatter would be used to
	 * format or parse numbers.
	 *
	 * This method is useful when you prefer just to get null on error,
	 * as if you called numfmt_create().
	 *
	 * @param integer  $style    Style of the formatting, one of the UNumberFormatStyle constants
	 * @param string   $locale   Locale in which the number would be formatted
	 * @param [string] $pattern  Pattern string in case chose style requires pattern
	 * @return NumberFormatter
	 * @see __construct
	 * @see numfmt_create
	 */
	public static function create($locale, $style, $pattern = null) {}

	/**
	 * Format a number according to current formatting rules.
	 *
	 * If the type is not specified, the type is derived from the $number parameter. I.e., if it's
	 * integer then INT32 would be chosen on 32-bit, INT64 on 64-bit, if it's double, DOUBLE would be
	 * chosen. It is possible to format 64-bit number on 32-bit machine by passing it as double and using
	 * TYPE_INT64.
	 * When formatting currency, default formatter's currency code is used.
	 *
	 * @param integer|double $number Number to format
	 * @param [integer]      $type   Type of the formatting - one of TYPE constants. If not specified, default for the type.
	 * @return string formatted number
	 */
	public function format($number, $type = 0) {}

	/**
	 * Parse a number according to current formatting rules.
	 *
	 * @param string     $string   String to parse
	 * @param [integer]  $type     Type of the formatting - one of TYPE constants.
	 *                             TYPE_DOUBLE is used by default.
	 * @param [integer]  $position On input, the position to start parsing, default is 0;
	 *                             on output, moved to after the last successfully parse character;
	 *                             on parse failure, does not change.
	 * @return integer|double|false  Parsed number, false if parsing failed
	 */
	public function parse($string, $type, &$position) {}

	/**
	 * Format number as currency.
	 *
	 * Uses user-defined currency string.
	 *
	 * @param double $number    Number to format
	 * @param string $currency  3-letter currency code (ISO 4217) to use in format
	 */
	public function formatCurrency($number, $currency) {}

	/**
	 * Parse currency string
	 *
	 * This parser would use parseCurrency API string to parse currency string. The format is defined by the
	 * formatter, returns both number and currency code.
	 *
	 * @param string    $string    String to parse
	 * @param string    $currency  Parameter to return parsed currency code
	 * @param [integer] $position  On input, the position within text to match, default is 0;
	 *                             on output, the position after the last matched character;
	 *                             on parse failure, does not change.
	 * @return double currency number
	 */
	public function parseCurrency($string, &$currency, &$position) {}

	/**
	 * Set formatter attribute.
	 *
	 * This function is used to set any of the formatter attributes. Example:
	 *
	 * $formatter->setAttribute(NumberFormat::FORMAT_WIDTH, 10);
 	 *
	 * @param integer        $attr  One of UNumberFormatAttribute constants
	 * @param integer|double $value Value of the attribute
	 * @return false if attribute is unknown or can not be set, true otherwise
	 */
	public function setAttribute($attr, $value) {}
	/**
	 * Set formatter attribute.
	 *
	 * This function is used to set any of the formatter attributes. Example:
	 *
	 * $formatter->setTextAttribute(NumberFormat::POSITIVE_PREFIX, "+");
	 *
	 * @param integer $attr  One of UNumberFormatTextAttribute constants
	 * @param string  $value Value of the attribute
	 * @return false if attribute is unknown or can not be set, true otherwise
	 */
	public function setTextAttribute($attr, $value) {}
	/**
	 * Set formatting symbol.
	 *
	 * Example:
	 *
	 * $formatter->setSymbol(NumberFormat::EXPONENTIAL_SYMBOL, "E");
	 *
	 * @param integer|array $attr  One of UNumberFormatSymbol constants or array of symbols, indexed by
	 * 								these constants
	 * @param string        $value Value of the symbol
	 */
	public function setSymbol($attr, $value) {}
	/**
	 * Set pattern used by the formatter
	 *
	 * Valid only if the formatter is using pattern and is not rule-based.
	 * @see http://www.icu-project.org/apiref/icu4c/classDecimalFormat.html
	 * Localized patterns are not currently supported.
	 *
	 * @param string $pattern  The pattern to be used.
	 * @return boolean         false if formatter pattern could not be set, true otherwise
	 */
	public function setPattern($pattern) {}
	/**
	 * Get value of the formatter attribute
	 *
	 * @param integer $attr One of UNumberFormatAttribute constants
	 * @return integer|double value of the attribute or false if the value can not be obtained
	 */
	public function getAttribute($attr) {}
	/**
	 * Get value of the formatter attribute
	 *
	 * @param integer $attr One of UNumberFormatTextAttribute constants
	 * @return string value of the attribute or false if the value can not be obtained
	 */
	public function getTextAttribute($attr) {}
	/**
	 * Get value of the formatter symbol
	 *
	 * @param integer $attr One of UNumberFormatSymbol constants specifying the symbol
	 * @return string|false The symbol value, or false if the value can not be obtained
	 */
	public function getSymbol($attr) {}
	/**
	 * Get pattern used by the formatter.
	 *
	 * Gets current state of the formatter as a pattern.
	 * Localized patterns are not currently supported.
	 *
	 * Valid only if the formatter is UNUM_PATTERN_DECIMAL
	 * @return string|false The pattern used by the formatter or false if formatter is of a type
	 *                      that does not support patterns.
	 */
	public function getPattern() {}
	/**
	 * Get the locale for which the formatter was created.
	 *
	 * @param [integer] $type One of  ULocDataLocaleType  values
	 * @return string locale name
	 */
	public function getLocale($type = 0) {}
	/**
	 * Get the error code from last operation
	 *
	 * Returns error code from the last number formatting operation.
	 *
	 * @return integer the error code, one of UErrorCode values. Initial value is U_ZERO_ERROR.
	 */
	public function getErrorCode() {}
	/**
	 * Get the error text from the last operation.
	 *
	 * @return string Description of the last occured error.
	 */
	public public function getErrorMessage() {}

}

/** Now the same as procedural API */

/**
 * Create a number formatter
 *
 * Creates a number formatter from locale and pattern. This formatter would be used to
 * format or parse numbers.
 *
 * @param string   $locale   Locale in which the number would be formatted
 * @param integer  $style    Style of the formatting, one of the UNumberFormatStyle constants
 * @param [string] $pattern  Pattern string in case chose style requires pattern
 * @return Numberformatter resource NumberFormatter
 */
function numfmt_create($locale, $style, $pattern = null) {}
/**
 * Format a number according to current formatting rules.
 *
 * If the type is not specified, the type is derived from the $number parameter. I.e., if it's
 * integer then INT32 would be chosen on 32-bit, INT64 on 64-bit, if it's double, DOUBLE would be
 * chosen. It is possible to format 64-bit number on 32-bit machine by passing it as double and using
 * TYPE_INT64.
 *
 * @param NumberFormatter $formatter The formatter resource
 * @param integer|double  $number	 Number to format
 * @param [integer]       $type		 Type of the formatting - one of TYPE constants. If not specified, default for the type.
 * @return string formatted number
 */
function numfmt_format($formatter, $number, $type = null) {}
/**
 * Parse a number according to current formatting rules.
 *
 * This parser uses DOUBLE type by default. When parsing currency,
 * default currency definitions are used.
 *
 * @param NumberFormatter $formatter       The formatter resource
 * @param string                 $string   String to parse
 * @param [integer]              $type     Type of the formatting - one of TYPE constants.
 * @param [integer]              $position String position after the end of parsed data.
 * @return integer|double|false  Parsed number, false if parsing failed
 */
function numfmt_parse($formatter, $string, $type, &$position) {}
/**
 * Format number as currency.
 *
 * Uses user-defined currency string.
 *
 * @param NumberFormatter $formatter The formatter resource
 * @param double          $number    Number to format
 * @param string          $currency  3-letter currency code (ISO 4217) to use in format
 */
function numfmt_format_currency($formatter, $number, $currency) {}
/**
 * Parse currency string
 *
 * This parser would use parseCurrency API string to parse currency string. The format is defined by the
 * formatter, returns both number and currency code.
 *
 * @param NumberFormatter $formatter The formatter resource
 * @param string          $string    String to parse
 * @param string          $currency  Parameter to return parsed currency code
 * @param [integer]       $position  String position after the end of parsed data.
 * @return double currency number
 */
function numfmt_parse_currency($formatter, $string, &$currency, &$position) {}
/**
 * Set formatter attribute.
 *
 * This function is used to set any of the formatter attributes. Example:
 *
 * numfmt_format_set_attribute($formatter, NumberFormat::FORMAT_WIDTH, 10);
 *
 * @param NumberFormatter $formatter The formatter resource
 * @param integer         $attr      One of UNumberFormatAttribute constants
 * @param integer|double  $value     Value of the attribute
 * @return false if attribute is unknown or can not be set, true otherwise
 */
function numfmt_set_attribute($formatter, $attribute, $value) {}
/**
 * Set formatter attribute.
 *
 * This function is used to set any of the formatter attributes. Example:
 *
 * numfmt_format_set_text_attribute($formatter, NumberFormat::POSITIVE_PREFIX, "+");
 *
 * @param NumberFormatter $formatter The formatter resource
 * @param integer         $attr      One of UNumberFormatTextAttribute constants
 * @param string          $value     Value of the attribute
 * @return false if attribute is unknown or can not be set, true otherwise
 */
function numfmt_set_text_attribute($formatter, $attribute, $value) {}
/**
 * Set formatting symbol.
 *
 * Example:
 *
 * $formatter->setSymbol(NumberFormat::EXPONENTIAL_SYMBOL, "E");
 *
 * @param NumberFormatter $formatter The formatter resource
 * @param integer|array   $attr      One of UNumberFormatSymbol constants or array of symbols,
 *                                   indexed by these constants
 * @param string $value Value of the symbol
 */
function numfmt_set_symbol($formatter, $attribute, $value) {}
/**
 * Set pattern used by the formatter
 *
 * Valid only if the formatter is using pattern and is not rule-based.
 * @see http://www.icu-project.org/apiref/icu4c/classDecimalFormat.html
 * Localized patterns are not currently supported.
 *
 * @param NumberFormatter $formatter The formatter resource
 * @param string          $pattern   The pattern to be used.
 * @return boolean false if formatter pattern could not be set, true otherwise
 */
function numfmt_set_pattern($formatter, $pattern) {}
/**
 * Get value of the formatter attribute
 *
 * @param NumberFormatter $formatter The formatter resource
 * @param integer         $attribute One of UNumberFormatAttribute constants
 * @return integer|double value of the attribute or false if the value can not be obtained
 */
function numfmt_get_attribute($formatter, $attribute) {}
/**
 * Get value of the formatter attribute
 *
 * @param NumberFormatter $formatter The formatter resource
 * @param integer         $attribute One of UNumberFormatTextAttribute constants
 * @return string value of the attribute or false if the value can not be obtained
 */
function numfmt_get_text_attribute($formatter, $attribute) {}
/**
 * Get value of the formatter symbol
 *
 * @param NumberFormatter $formatter The formatter resource
 * @param integer         $attribute One of UNumberFormatSymbol constants specifying the symbol
 * @return string|false The symbol value, or false if the value can not be obtained
 */
function numfmt_get_symbol($formatter, $attribute) {}
/**
 * Get pattern used by the formatter.
 *
 * Gets current state of the formatter as a pattern.
 * Localized patterns are not currently supported.
 *
 * Valid only if the formatter is   UNUM_PATTERN_DECIMAL
 * @param NumberFormatter $formatter The formatter resource
 * @return string|false The pattern used by the formatter or false if formatter is of a type
 *                      that does not support patterns.
 */
function numfmt_get_pattern($formatter) {}
/**
 * Get the locale for which the formatter was created.
 *
 * @param NumberFormatter $formatter The formatter resource
 * @param [integer]       $type      One of ULocDataLocaleType values
 * @return string locale name
 */
function numfmt_get_locale($formatter, $type = 0) {}
/**
 * Get the error code from last operation
 *
 * Returns error code from the last number formatting operation.
 *
 * @param NumberFormatter $formatter The formatter resource
 * @return integer the error code, one of UErrorCode values. Initial value is U_ZERO_ERROR.
 */
function numfmt_get_error_code($formatter) {}
/**
 * Get the error text from the last operation.
 *
 * @param NumberFormatter $formatter The formatter resource
 * @return string Description of the last occured error.
 */
function numfmt_get_error_message($formatter) {}
