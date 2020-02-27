<?php

class IntlChar
{
    /**
     * @param mixed $codepoint
     * @return bool|null
     */
    public static function hasBinaryProperty($codepoint, int $property) {}

    /**
     * @param mixed $codepoint
     * @return array|null
     */
    public static function charAge($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return int|null
     */
    public static function charDigitValue($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return int|null
     */
    public static function charDirection($codepoint) {}

    /** @return int|null */
    public static function charFromName(string $characterName, int $nameChoice = IntlChar::UNICODE_CHAR_NAME) {}

    /**
     * @param mixed $codepoint
     * @return int|string|null
     */
    public static function charMirror($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return string|null
     */
    public static function charName($codepoint, int $nameChoice = IntlChar::UNICODE_CHAR_NAME) {}

    /**
     * @param mixed $codepoint
     * @return int|null
     */
    public static function charType($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return string|null
     */
    public static function chr($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return int|false|null
     */
    public static function digit($codepoint, int $radix = 10) {}

    /**
     * @param mixed $start
     * @param mixed $limit
     * @param callable $callback
     * @return bool|null
     */
    public static function enumCharNames($start, $limit, $callback, int $nameChoice = IntlChar::UNICODE_CHAR_NAME) {}

    /**
     * @param callable $callable
     * @return void
     */
    public static function enumCharTypes($callable) {}

    /**
     * @param mixed $codepoint
     * @return string|int|null
     */
    public static function foldCase($codepoint, int $options = IntlChar::FOLD_CASE_DEFAULT ) {}

    /** @return int */
    public static function forDigit(int $digit, $radix = 10) {}

#if U_ICU_VERSION_MAJOR_NUM >= 52
    /**
     * @param mixed $codepoint
     * @return string|int|null
     */
    public static function getBidiPairedBracket($codepoint) {}
#endif

    /**
     * @param mixed $codepoint
     * @return int|null
     */
    public static function getBlockCode($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return int|null
     */
    public static function getCombiningClass($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return string|false|null
     */
    public static function getFC_NFKC_Closure($codepoint) {}

    /** @return int */
    public static function getIntPropertyMaxValue(int $property) {}

    /** @return int */
    public static function getIntPropertyMinValue(int $property) {}

    /**
     * @param mixed $codepoint
     * @return int|null
     */
    public static function getIntPropertyValue($codepoint, int $property) {}

    /**
     * @param mixed $codepoint
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
     * @param mixed $codepoint
     * @return bool|null
     */
    public static function isalnum($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return bool|null
     */
    public static function isalpha($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return bool|null
     */
    public static function isbase($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return bool|null
     */
    public static function isblank($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return bool|null
     */
    public static function iscntrl($codepoint ) {}

    /**
     * @param mixed $codepoint
     * @return bool|null
     */
    public static function isdefined($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return bool|null
     */
    public static function isdigit($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return bool|null
     */
    public static function isgraph($codepoint ) {}

    /**
     * @param mixed $codepoint
     * @return bool|null
     */
    public static function isIDIgnorable($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return bool|null
     */
    public static function isIDPart($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return bool|null
     */
    public static function isIDStart($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return bool|null
     */
    public static function isISOControl($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return bool|null
     */
    public static function isJavaIDPart($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return bool|null
     */
    public static function isJavaIDStart($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return bool|null
     */
    public static function isJavaSpaceChar($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return bool|null
     */
    public static function islower($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return bool|null
     */
    public static function isMirrored($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return bool|null
     */
    public static function isprint($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return bool|null
     */
    public static function ispunct($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return bool|null
     */
    public static function isspace($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return bool|null
     */
    public static function istitle($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return bool|null
     */
    public static function isUAlphabetic($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return bool|null
     */
    public static function isULowercase($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return bool|null
     */
    public static function isupper($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return bool|null
     */
    public static function isUUppercase($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return bool|null
     */
    public static function isUWhiteSpace($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return bool|null
     */
    public static function isWhitespace($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return bool|null
     */
    public static function isxdigit($codepoint) {}

    /**
     * @param mixed $character
     * @return int|null
     */
    public static function ord($character) {}

    /**
     * @param mixed $codepoint
     * @return string|int|null
     */
    public static function tolower($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return string|int|null
     */
    public static function totitle($codepoint) {}

    /**
     * @param mixed $codepoint
     * @return string|int|null
     */
    public static function toupper($codepoint) {}
}
