<?php

class IntlChar
{
    /**
     * @param int|string $codepoint
     * @return bool|null
     */
    public static function hasBinaryProperty($codepoint, int $property) {}

    /**
     * @param int|string $codepoint
     * @return array|null
     */
    public static function charAge($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return int|null
     */
    public static function charDigitValue($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return int|null
     */
    public static function charDirection($codepoint) {}

    /** @return int|null */
    public static function charFromName(string $characterName, int $nameChoice = IntlChar::UNICODE_CHAR_NAME) {}

    /**
     * @param int|string $codepoint
     * @return int|string|null
     */
    public static function charMirror($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return string|null
     */
    public static function charName($codepoint, int $nameChoice = IntlChar::UNICODE_CHAR_NAME) {}

    /**
     * @param int|string $codepoint
     * @return int|null
     */
    public static function charType($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return string|null
     */
    public static function chr($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return int|false|null
     */
    public static function digit($codepoint, int $radix = 10) {}

    /**
     * @param int|string $start
     * @param int|string $limit
     * @return bool|null
     */
    public static function enumCharNames($start, $limit, callable $callback, int $nameChoice = IntlChar::UNICODE_CHAR_NAME) {}

    /** @return void */
    public static function enumCharTypes(callable $callback) {}

    /**
     * @param int|string $codepoint
     * @return int|string|null
     */
    public static function foldCase($codepoint, int $options = IntlChar::FOLD_CASE_DEFAULT) {}

    /** @return int */
    public static function forDigit(int $digit, $radix = 10) {}

#if U_ICU_VERSION_MAJOR_NUM >= 52
    /**
     * @param int|string $codepoint
     * @return int|string|null
     */
    public static function getBidiPairedBracket($codepoint) {}
#endif

    /**
     * @param int|string $codepoint
     * @return int|null
     */
    public static function getBlockCode($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return int|null
     */
    public static function getCombiningClass($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return string|false|null
     */
    public static function getFC_NFKC_Closure($codepoint) {}

    /** @return int */
    public static function getIntPropertyMaxValue(int $property) {}

    /** @return int */
    public static function getIntPropertyMinValue(int $property) {}

    /**
     * @param int|string $codepoint
     * @return int|null
     */
    public static function getIntPropertyValue($codepoint, int $property) {}

    /**
     * @param int|string $codepoint
     * @return float|null
     */
    public static function getNumericValue($codepoint) {}

    /**
     * @param string $alias
     * @return int
     */
    public static function getPropertyEnum(string $alias) {}

    /** @return string|false */
    public static function getPropertyName(int $property, int $nameChoice = IntlChar::LONG_PROPERTY_NAME) {}

    /** @return int */
    public static function getPropertyValueEnum(int $property, string $name) {}

    /** @return string|false */
    public static function getPropertyValueName(int $property, int $value, $nameChoice = IntlChar::LONG_PROPERTY_NAME) {}

    /** @return array */
    public static function getUnicodeVersion() {}

    /**
     * @param int|string $codepoint
     * @return bool|null
     */
    public static function isalnum($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return bool|null
     */
    public static function isalpha($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return bool|null
     */
    public static function isbase($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return bool|null
     */
    public static function isblank($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return bool|null
     */
    public static function iscntrl($codepoint ) {}

    /**
     * @param int|string $codepoint
     * @return bool|null
     */
    public static function isdefined($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return bool|null
     */
    public static function isdigit($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return bool|null
     */
    public static function isgraph($codepoint ) {}

    /**
     * @param int|string $codepoint
     * @return bool|null
     */
    public static function isIDIgnorable($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return bool|null
     */
    public static function isIDPart($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return bool|null
     */
    public static function isIDStart($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return bool|null
     */
    public static function isISOControl($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return bool|null
     */
    public static function isJavaIDPart($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return bool|null
     */
    public static function isJavaIDStart($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return bool|null
     */
    public static function isJavaSpaceChar($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return bool|null
     */
    public static function islower($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return bool|null
     */
    public static function isMirrored($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return bool|null
     */
    public static function isprint($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return bool|null
     */
    public static function ispunct($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return bool|null
     */
    public static function isspace($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return bool|null
     */
    public static function istitle($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return bool|null
     */
    public static function isUAlphabetic($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return bool|null
     */
    public static function isULowercase($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return bool|null
     */
    public static function isupper($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return bool|null
     */
    public static function isUUppercase($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return bool|null
     */
    public static function isUWhiteSpace($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return bool|null
     */
    public static function isWhitespace($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return bool|null
     */
    public static function isxdigit($codepoint) {}

    /**
     * @param int|string $character
     * @return int|null
     */
    public static function ord($character) {}

    /**
     * @param int|string $codepoint
     * @return int|string|null
     */
    public static function tolower($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return int|string|null
     */
    public static function totitle($codepoint) {}

    /**
     * @param int|string $codepoint
     * @return int|string|null
     */
    public static function toupper($codepoint) {}
}
