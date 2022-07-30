<?php

/** @generate-class-entries */

class Locale
{
    /**
     * @var int
     * @cvalue ULOC_ACTUAL_LOCALE
     */
    public const ACTUAL_LOCALE = UNKNOWN;
    /**
     * @var int
     * @cvalue ULOC_VALID_LOCALE
     */
    public const VALID_LOCALE = UNKNOWN;
    /** @var null */
    public const DEFAULT_LOCALE = null;
    /**
     * @var string
     * @cvalue LOC_LANG_TAG
     */
    public const LANG_TAG = UNKNOWN;
    /**
     * @var string
     * @cvalue LOC_EXTLANG_TAG
     */
    public const EXTLANG_TAG = UNKNOWN;
    /**
     * @var string
     * @cvalue LOC_SCRIPT_TAG
     */
    public const SCRIPT_TAG = UNKNOWN;
    /**
     * @var string
     * @cvalue LOC_REGION_TAG
     */
    public const REGION_TAG = UNKNOWN;
    /**
     * @var string
     * @cvalue LOC_VARIANT_TAG
     */
    public const VARIANT_TAG = UNKNOWN;
    /**
     * @var string
     * @cvalue LOC_GRANDFATHERED_LANG_TAG
     */
    public const GRANDFATHERED_LANG_TAG = UNKNOWN;
    /**
     * @var string
     * @cvalue LOC_PRIVATE_TAG
     */
    public const PRIVATE_TAG = UNKNOWN;

    /**
     * @tentative-return-type
     * @alias locale_get_default
     */
    public static function getDefault(): string {}

    /**
     * @return bool
     * @alias locale_set_default
     */
    public static function setDefault(string $locale) {} // TODO make return type void

    /**
     * @tentative-return-type
     * @alias locale_get_primary_language
     */
    public static function getPrimaryLanguage(string $locale): ?string{}

    /**
     * @tentative-return-type
     * @alias locale_get_script
     */
    public static function getScript(string $locale): ?string {}

    /**
     * @tentative-return-type
     * @alias locale_get_region
     */
    public static function getRegion(string $locale): ?string {}

    /**
     * @return array<string, string>|false|null
     * @tentative-return-type
     * @alias locale_get_keywords
     */
    public static function getKeywords(string $locale): array|false|null {}

    /**
     * @tentative-return-type
     * @alias locale_get_display_script
     */
    public static function getDisplayScript(string $locale, ?string $displayLocale = null): string|false {}

    /**
     * @tentative-return-type
     * @alias locale_get_display_region
     */
    public static function getDisplayRegion(string $locale, ?string $displayLocale = null): string|false {}

    /**
     * @tentative-return-type
     * @alias locale_get_display_name
     */
    public static function getDisplayName(string $locale, ?string $displayLocale = null): string|false {}

    /**
     * @tentative-return-type
     * @alias locale_get_display_language
     */
    public static function getDisplayLanguage(string $locale, ?string $displayLocale = null): string|false {}

    /**
     * @tentative-return-type
     * @alias locale_get_display_variant
     */
    public static function getDisplayVariant(string $locale, ?string $displayLocale = null): string|false {}

    /**
     * @tentative-return-type
     * @alias locale_compose
     */
    public static function composeLocale(array $subtags): string|false {}

    /**
     * @tentative-return-type
     * @alias locale_parse
     */
    public static function parseLocale(string $locale): ?array {}

    /**
     * @tentative-return-type
     * @alias locale_get_all_variants
     */
    public static function getAllVariants(string $locale): ?array {}

    /**
     * @tentative-return-type
     * @alias locale_filter_matches
     */
    public static function filterMatches(string $languageTag, string $locale, bool $canonicalize = false): ?bool {}

    /**
     * @tentative-return-type
     * @alias locale_lookup
     */
    public static function lookup(array $languageTag, string $locale, bool $canonicalize = false, ?string $defaultLocale = null): ?string {}

    /**
     * @tentative-return-type
     * @alias locale_canonicalize
     */
    public static function canonicalize(string $locale): ?string {}

    /**
     * @tentative-return-type
     * @alias locale_accept_from_http
     */
    public static function acceptFromHttp(string $header): string|false {}
}
