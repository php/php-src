<?php

/** @generate-function-entries */

class Locale
{
    /**
     * @return string
     * @alias locale_get_default
     */
    public static function getDefault() {}

    /**
     * @return bool
     * @alias locale_set_default
     */
    public static function setDefault(string $locale) {}

    /**
     * @return string|null
     * @alias locale_get_primary_language
     */
    public static function getPrimaryLanguage(string $locale) {}

    /**
     * @return string|null
     * @alias locale_get_script
     */
    public static function getScript(string $locale) {}

    /**
     * @return string|null
     * @alias locale_get_region
     */
    public static function getRegion(string $locale) {}

    /**
     * @return array|false|null
     * @alias locale_get_keywords
     */
    public static function getKeywords(string $locale) {}

    /**
     * @return string|false
     * @alias locale_get_display_script
     */
    public static function getDisplayScript(string $locale, ?string $displayLocale = null) {}

    /**
     * @return string|false
     * @alias locale_get_display_region
     */
    public static function getDisplayRegion(string $locale, ?string $displayLocale = null) {}

    /**
     * @return string|false
     * @alias locale_get_display_name
     */
    public static function getDisplayName(string $locale, ?string $displayLocale = null) {}

    /**
     * @return string|false
     * @alias locale_get_display_language
     */
    public static function getDisplayLanguage(string $locale, ?string $displayLocale = null) {}

    /**
     * @return string|false
     * @alias locale_get_display_variant
     */
    public static function getDisplayVariant(string $locale, ?string $displayLocale = null) {}

    /**
     * @return string|false
     * @alias locale_compose
     */
    public static function composeLocale(array $subtags) {}

    /**
     * @return array|null
     * @alias locale_parse
     */
    public static function parseLocale(string $locale) {}

    /**
     * @return array|null
     * @alias locale_get_all_variants
     */
    public static function getAllVariants(string $locale) {}

    /**
     * @return bool|null
     * @alias locale_filter_matches
     */
    public static function filterMatches(string $languageTag, string $locale, bool $canonicalize = false) {}

    /**
     * @return string|null
     * @alias locale_lookup
     */
    public static function lookup(array $languageTag, string $locale, bool $canonicalize = false, ?string $defaultLocale = null) {}

    /**
     * @return string|null
     * @alias locale_canonicalize
     */
    public static function canonicalize(string $locale) {}

    /**
     * @return string|false
     * @alias locale_accept_from_http
     */
    public static function acceptFromHttp(string $header) {}
}
