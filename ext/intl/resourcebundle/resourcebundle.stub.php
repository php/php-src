<?php

class ResourceBundle implements Traversable
{
    public function __construct(?string $locale, ?string $bundlename, bool $fallback = true) {}

    /** @return ResourceBundle|null */
    public static function create(?string $locale, ?string $bundlename, bool $fallback = true) {}

    /**
     * @param string|int $index
     * @return mixed
     */
    public function get($index, bool $fallback = true) {}

    /** @return int|false */
    public function count() {}

    /** @return array|false */
    public static function getLocales(string $bundlename) {}

    /** @return int */
    public function getErrorCode() {}

    /** @return string */
    public function getErrorMessage() {}
}

function resourcebundle_create(?string $locale, ?string $bundlename, bool $fallback = true): ?ResourceBundle {}

/**
 * @param string|int $index
 * @return mixed
 */
function resourcebundle_get(ResourceBundle $bundle, $index) {}

function resourcebundle_count(ResourceBundle $bundle): int|false {}

function resourcebundle_locales(string $bundlename): array|false {}

function resourcebundle_get_error_code(ResourceBundle $bundle): int {}

function resourcebundle_get_error_message(ResourceBundle $bundle): string {}
