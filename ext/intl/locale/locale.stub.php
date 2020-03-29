<?php

class Locale
{
    /** @return string */
    public static function getDefault() {}

    /** @return bool */
    public static function setDefault(string $locale) {}

    /** @return ?string */
    public static function getPrimaryLanguage(string $locale) {}

    /** @return ?string */
    public static function getScript(string $locale) {}

    /** @return ?string */
    public static function getRegion(string $locale) {}

    /** @return array|false|null */
    public static function getKeywords(string $locale) {}

    /** @return string|false */
    public static function getDisplayScript(string $locale, ?string $in_locale = null) {}

    /** @return string|false */
    public static function getDisplayRegion(string $locale, ?string $in_locale = null) {}

    /** @return string|false */
    public static function getDisplayName(string $locale, ?string $in_locale = null) {}

    /** @return string|false */
    public static function getDisplayLanguage(string $locale, ?string $in_locale = null) {}

    /** @return string|false */
    public static function getDisplayVariant(string $locale, ?string $in_locale = null) {}

    /** @return string|false */
    public static function composeLocale(array $subtags) {}

    /** @return ?array */
    public static function parseLocale(string $locale) {}

    /** @return ?array */
    public static function getAllVariants(string $locale) {}

    /** @return ?bool */
    public static function filterMatches(string $langtag, string $locale, bool $canonicalize = false) {}

    /** @return ?string */
    public static function lookup(array $langtag, string $locale, bool $canonicalize = false, ?string $def = null) {}

    /** @return ?string */
    public static function canonicalize(string $locale) {}

    /** @return string|false */
    public static function acceptFromHttp(string $header) {}
}
