<?php

/** @generate-function-entries */

class IntlChar
{
    /** @return bool|null */
    public static function hasBinaryProperty(int|string $codepoint, int $property) {}

    /** @return array|null */
    public static function charAge(int|string $codepoint) {}

    /** @return int|null */
    public static function charDigitValue(int|string $codepoint) {}

    /** @return int|null
     */
    public static function charDirection(int|string $codepoint) {}

    /** @return int|null */
    public static function charFromName(string $name, int $type = IntlChar::UNICODE_CHAR_NAME) {}

    /** @return int|string|null */
    public static function charMirror(int|string $codepoint) {}

    /** @return string|null */
    public static function charName(int|string $codepoint, int $type = IntlChar::UNICODE_CHAR_NAME) {}

    /** @return int|null */
    public static function charType(int|string $codepoint) {}

    /** @return string|null */
    public static function chr(int|string $codepoint) {}

    /** @return int|false|null */
    public static function digit(int|string $codepoint, int $base = 10) {}

    /** @return bool|null */
    public static function enumCharNames(int|string $start, int|string $end, callable $callback, int $type = IntlChar::UNICODE_CHAR_NAME) {}

    /** @return void */
    public static function enumCharTypes(callable $callback) {}

    /**
     * @return int|string|null
     */
    public static function foldCase(int|string $codepoint, int $options = IntlChar::FOLD_CASE_DEFAULT) {}

    /** @return int */
    public static function forDigit(int $digit, int $base = 10) {}

#if U_ICU_VERSION_MAJOR_NUM >= 52
    /** @return int|string|null */
    public static function getBidiPairedBracket(int|string $codepoint) {}
#endif

    /** @return int|null */
    public static function getBlockCode(int|string $codepoint) {}

    /** @return int|null */
    public static function getCombiningClass(int|string $codepoint) {}

    /** @return string|false|null */
    public static function getFC_NFKC_Closure(int|string $codepoint) {}

    /** @return int */
    public static function getIntPropertyMaxValue(int $property) {}

    /** @return int */
    public static function getIntPropertyMinValue(int $property) {}

    /** @return int|null */
    public static function getIntPropertyValue(int|string $codepoint, int $property) {}

    /** @return float|null */
    public static function getNumericValue(int|string $codepoint) {}

    /** @return int */
    public static function getPropertyEnum(string $alias) {}

    /** @return string|false */
    public static function getPropertyName(int $property, int $type = IntlChar::LONG_PROPERTY_NAME) {}

    /** @return int */
    public static function getPropertyValueEnum(int $property, string $name) {}

    /** @return string|false */
    public static function getPropertyValueName(int $property, int $value, int $type = IntlChar::LONG_PROPERTY_NAME) {}

    /** @return array */
    public static function getUnicodeVersion() {}

    /** @return bool|null */
    public static function isalnum(int|string $codepoint) {}

    /** @return bool|null */
    public static function isalpha(int|string $codepoint) {}

    /** @return bool|null */
    public static function isbase(int|string $codepoint) {}

    /** @return bool|null */
    public static function isblank(int|string $codepoint) {}

    /** @return bool|null */
    public static function iscntrl(int|string $codepoint) {}

    /** @return bool|null */
    public static function isdefined(int|string $codepoint) {}

    /** @return bool|null */
    public static function isdigit(int|string $codepoint) {}

    /** @return bool|null */
    public static function isgraph(int|string $codepoint) {}

    /** @return bool|null */
    public static function isIDIgnorable(int|string $codepoint) {}

    /** @return bool|null */
    public static function isIDPart(int|string $codepoint) {}

    /** @return bool|null */
    public static function isIDStart(int|string $codepoint) {}

    /** @return bool|null */
    public static function isISOControl(int|string $codepoint) {}

    /** @return bool|null */
    public static function isJavaIDPart(int|string $codepoint) {}

    /** @return bool|null */
    public static function isJavaIDStart(int|string $codepoint) {}

    /** @return bool|null */
    public static function isJavaSpaceChar(int|string $codepoint) {}

    /** @return bool|null */
    public static function islower(int|string $codepoint) {}

    /** @return bool|null */
    public static function isMirrored(int|string $codepoint) {}

    /** @return bool|null */
    public static function isprint(int|string $codepoint) {}

    /** @return bool|null */
    public static function ispunct(int|string $codepoint) {}

    /** @return bool|null */
    public static function isspace(int|string $codepoint) {}

    /** @return bool|null */
    public static function istitle(int|string $codepoint) {}

    /** @return bool|null */
    public static function isUAlphabetic(int|string $codepoint) {}

    /** @return bool|null */
    public static function isULowercase(int|string $codepoint) {}

    /** @return bool|null */
    public static function isupper(int|string $codepoint) {}

    /** @return bool|null */
    public static function isUUppercase(int|string $codepoint) {}

    /** @return bool|null */
    public static function isUWhiteSpace(int|string $codepoint) {}

    /** @return bool|null */
    public static function isWhitespace(int|string $codepoint) {}

    /** @return bool|null */
    public static function isxdigit(int|string $codepoint) {}

    /** @return int|null */
    public static function ord(int|string $character) {}

    /** @return int|string|null */
    public static function tolower(int|string $codepoint) {}

    /** @return int|string|null */
    public static function totitle(int|string $codepoint) {}

    /** @return int|string|null */
    public static function toupper(int|string $codepoint) {}
}
