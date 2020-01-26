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

function locale_get_default(): string {}

function locale_set_default(string $name): bool {}

function locale_get_primary_language(string $locale): ?string {}

function locale_get_script(string $locale): ?string {}

function locale_get_region(string $locale): ?string {}

function locale_get_keywords(string $locale): array|false|null {}

function locale_get_display_script(string $locale, ?string $in_locale = null): string|false {}

function locale_get_display_region(string $locale, ?string $in_locale = null): string|false {}

function locale_get_display_name(string $locale, ?string $in_locale = null): string|false {}

function locale_get_display_language(string $locale, ?string $in_locale = null): string|false {}

function locale_get_display_variant(string $locale, ?string $in_locale = null): string|false {}

function locale_compose(array $subtags): string|false {}

function locale_parse(string $locale): ?array {}

function locale_get_all_variants(string $locale): ?array {}

function locale_filter_matches(string $langtag, string $locale, bool $canonicalize = false): ?bool {}

function locale_canonicalize(string $locale): ?string {}

function locale_lookup(array $langtag, string $locale, bool $canonicalize = false, ?string $def = null): ?string {}

function locale_accept_from_http(string $header): string|false {}
