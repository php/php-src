<?php

/** @generate-class-entries */

class IntlChar
{
    /** @tentative-return-type */
    public static function hasBinaryProperty(int|string $codepoint, int $property): ?bool {}

    /** @tentative-return-type */
    public static function charAge(int|string $codepoint): ?array {}

    /** @tentative-return-type */
    public static function charDigitValue(int|string $codepoint): ?int {}

    /** @tentative-return-type */
    public static function charDirection(int|string $codepoint): ?int {}

    /** @tentative-return-type */
    public static function charFromName(string $name, int $type = IntlChar::UNICODE_CHAR_NAME): ?int {}

    /** @tentative-return-type */
    public static function charMirror(int|string $codepoint): int|string|null {}

    /** @tentative-return-type */
    public static function charName(int|string $codepoint, int $type = IntlChar::UNICODE_CHAR_NAME): ?string {}

    /** @tentative-return-type */
    public static function charType(int|string $codepoint): ?int {}

    /** @tentative-return-type */
    public static function chr(int|string $codepoint): ?string {}

    /** @tentative-return-type */
    public static function digit(int|string $codepoint, int $base = 10): int|false|null {}

    /** @tentative-return-type */
    public static function enumCharNames(int|string $start, int|string $end, callable $callback, int $type = IntlChar::UNICODE_CHAR_NAME): ?bool {} // TODO return values just don't make sense

    /** @tentative-return-type */
    public static function enumCharTypes(callable $callback): void {}

    /** @tentative-return-type */
    public static function foldCase(int|string $codepoint, int $options = IntlChar::FOLD_CASE_DEFAULT): int|string|null {}

    /** @tentative-return-type */
    public static function forDigit(int $digit, int $base = 10): int {}

#if U_ICU_VERSION_MAJOR_NUM >= 52
    /** @tentative-return-type */
    public static function getBidiPairedBracket(int|string $codepoint): int|string|null {}
#endif

    /** @tentative-return-type */
    public static function getBlockCode(int|string $codepoint): ?int {}

    /** @tentative-return-type */
    public static function getCombiningClass(int|string $codepoint): ?int {}

    /** @tentative-return-type */
    public static function getFC_NFKC_Closure(int|string $codepoint): string|false|null {}

    /** @tentative-return-type */
    public static function getIntPropertyMaxValue(int $property): int {}

    /** @tentative-return-type */
    public static function getIntPropertyMinValue(int $property): int {}

    /** @tentative-return-type */
    public static function getIntPropertyValue(int|string $codepoint, int $property): ?int {}

    /** @tentative-return-type */
    public static function getNumericValue(int|string $codepoint): ?float {}

    /** @tentative-return-type */
    public static function getPropertyEnum(string $alias): int {}

    /** @tentative-return-type */
    public static function getPropertyName(int $property, int $type = IntlChar::LONG_PROPERTY_NAME): string|false {}

    /** @tentative-return-type */
    public static function getPropertyValueEnum(int $property, string $name): int {}

    /** @tentative-return-type */
    public static function getPropertyValueName(int $property, int $value, int $type = IntlChar::LONG_PROPERTY_NAME): string|false {}

    /** @tentative-return-type */
    public static function getUnicodeVersion(): array {}

    /** @tentative-return-type */
    public static function isalnum(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isalpha(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isbase(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isblank(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function iscntrl(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isdefined(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isdigit(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isgraph(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isIDIgnorable(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isIDPart(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isIDStart(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isISOControl(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isJavaIDPart(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isJavaIDStart(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isJavaSpaceChar(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function islower(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isMirrored(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isprint(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function ispunct(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isspace(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function istitle(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isUAlphabetic(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isULowercase(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isupper(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isUUppercase(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isUWhiteSpace(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isWhitespace(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isxdigit(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function ord(int|string $character): ?int {}

    /** @tentative-return-type */
    public static function tolower(int|string $codepoint): int|string|null {}

    /** @tentative-return-type */
    public static function totitle(int|string $codepoint): int|string|null {}

    /** @tentative-return-type */
    public static function toupper(int|string $codepoint): int|string|null {}
}
