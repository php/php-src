<?php

/** @generate-class-entries */

/** @not-serializable */
class ResourceBundle implements IteratorAggregate, Countable
{
    public function __construct(?string $locale, ?string $bundle, bool $fallback = true) {}

    /**
     * @tentative-return-type
     * @alias resourcebundle_create
     */
    public static function create(?string $locale, ?string $bundle, bool $fallback = true): ?ResourceBundle {}

    /**
     * @param string|int $index
     * @tentative-return-type
     * @alias resourcebundle_get
     */
    public function get($index, bool $fallback = true): mixed {}

    /**
     * @tentative-return-type
     * @alias resourcebundle_count
     */
    public function count(): int {}

    /**
     * @return array<int, string>|false
     * @tentative-return-type
     * @alias resourcebundle_locales
     */
    public static function getLocales(string $bundle): array|false {}

    /**
     * @tentative-return-type
     * @alias resourcebundle_get_error_code
     */
    public function getErrorCode(): int {}

    /**
     * @tentative-return-type
     * @alias resourcebundle_get_error_message
     */
    public function getErrorMessage(): string {}

    public function getIterator(): Iterator {}
}
