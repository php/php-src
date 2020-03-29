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

    /** @return int */
    public function count() {}

    /** @return array|false */
    public static function getLocales(string $bundlename) {}

    /** @return int */
    public function getErrorCode() {}

    /** @return string */
    public function getErrorMessage() {}
}
