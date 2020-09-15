<?php

/** @generate-function-entries */

class IntlChar
{
    /** @return bool|null */
    public static function hasBinaryProperty(string|int $codepoint, int $property) {}

    /** @return array|null */
    public static function charAge(string|int $codepoint) {}

    /** @return int|null */
    public static function charDigitValue(string|int $codepoint) {}

    /** @return int|null
     */
    public static function charDirection(string|int $codepoint) {}

    /** @return int|null */
    public static function charFromName(string $characterName, int $nameChoice = IntlChar::UNICODE_CHAR_NAME) {}

    /** @return string|int|null */
    public static function charMirror(string|int $codepoint) {}

    /** @return string|null */
    public static function charName(string|int $codepoint, int $nameChoice = IntlChar::UNICODE_CHAR_NAME) {}

    /** @return int|null */
    public static function charType(string|int $codepoint) {}

    /** @return string|null */
    public static function chr(string|int $codepoint) {}

    /** @return int|false|null */
    public static function digit(string|int $codepoint, int $radix = 10) {}

    /** @return bool|null */
    public static function enumCharNames(string|int $start, string|int $limit, callable $callback, int $nameChoice = IntlChar::UNICODE_CHAR_NAME) {}

    /** @return void */
    public static function enumCharTypes(callable $callback) {}

    /**
     * @return string|int|null
     */
    public static function foldCase(string|int $codepoint, int $options = IntlChar::FOLD_CASE_DEFAULT) {}

    /** @return int */
    public static function forDigit(int $digit, int $radix = 10) {}

#if U_ICU_VERSION_MAJOR_NUM >= 52
    /** @return string|int|null */
    public static function getBidiPairedBracket(string|int $codepoint) {}
#endif

    /** @return int|null */
    public static function getBlockCode(string|int $codepoint) {}

    /** @return int|null */
    public static function getCombiningClass(string|int $codepoint) {}

    /** @return string|false|null */
    public static function getFC_NFKC_Closure(string|int $codepoint) {}

    /** @return int */
    public static function getIntPropertyMaxValue(int $property) {}

    /** @return int */
    public static function getIntPropertyMinValue(int $property) {}

    /** @return int|null */
    public static function getIntPropertyValue(string|int $codepoint, int $property) {}

    /** @return float|null */
    public static function getNumericValue(string|int $codepoint) {}

    /** @return int */
    public static function getPropertyEnum(string $alias) {}

    /** @return string|false */
    public static function getPropertyName(int $property, int $nameChoice = IntlChar::LONG_PROPERTY_NAME) {}

    /** @return int */
    public static function getPropertyValueEnum(int $property, string $name) {}

    /** @return string|false */
    public static function getPropertyValueName(int $property, int $value, int $nameChoice = IntlChar::LONG_PROPERTY_NAME) {}

    /** @return array */
    public static function getUnicodeVersion() {}

    /** @return bool|null */
    public static function isalnum(string|int $codepoint) {}

    /** @return bool|null */
    public static function isalpha(string|int $codepoint) {}

    /** @return bool|null */
    public static function isbase(string|int $codepoint) {}

    /** @return bool|null */
    public static function isblank(string|int $codepoint) {}

    /** @return bool|null */
    public static function iscntrl(string|int $codepoint) {}

    /** @return bool|null */
    public static function isdefined(string|int $codepoint) {}

    /** @return bool|null */
    public static function isdigit(string|int $codepoint) {}

    /** @return bool|null */
    public static function isgraph(string|int $codepoint) {}

    /** @return bool|null */
    public static function isIDIgnorable(string|int $codepoint) {}

    /** @return bool|null */
    public static function isIDPart(string|int $codepoint) {}

    /** @return bool|null */
    public static function isIDStart(string|int $codepoint) {}

    /** @return bool|null */
    public static function isISOControl(string|int $codepoint) {}

    /** @return bool|null */
    public static function isJavaIDPart(string|int $codepoint) {}

    /** @return bool|null */
    public static function isJavaIDStart(string|int $codepoint) {}

    /** @return bool|null */
    public static function isJavaSpaceChar(string|int $codepoint) {}

    /** @return bool|null */
    public static function islower(string|int $codepoint) {}

    /** @return bool|null */
    public static function isMirrored(string|int $codepoint) {}

    /** @return bool|null */
    public static function isprint(string|int $codepoint) {}

    /** @return bool|null */
    public static function ispunct(string|int $codepoint) {}

    /** @return bool|null */
    public static function isspace(string|int $codepoint) {}

    /** @return bool|null */
    public static function istitle(string|int $codepoint) {}

    /** @return bool|null */
    public static function isUAlphabetic(string|int $codepoint) {}

    /** @return bool|null */
    public static function isULowercase(string|int $codepoint) {}

    /** @return bool|null */
    public static function isupper(string|int $codepoint) {}

    /** @return bool|null */
    public static function isUUppercase(string|int $codepoint) {}

    /** @return bool|null */
    public static function isUWhiteSpace(string|int $codepoint) {}

    /** @return bool|null */
    public static function isWhitespace(string|int $codepoint) {}

    /** @return bool|null */
    public static function isxdigit(string|int $codepoint) {}

    /** @return int|null */
    public static function ord(string|int $character) {}

    /** @return string|int|null */
    public static function tolower(string|int $codepoint) {}

    /** @return string|int|null */
    public static function totitle(string|int $codepoint) {}

    /** @return string|int|null */
    public static function toupper(string|int $codepoint) {}
}
