<?php

/** @generate-class-entries */

/** @not-serializable */
class ResourceBundle implements IteratorAggregate, Countable, DimensionReadable
{
    public function __construct(?string $locale, ?string $bundle, bool $fallback = true) {}

    /**
     * @tentative-return-type
     * @alias resourcebundle_create
     */
    public static function create(?string $locale, ?string $bundle, bool $fallback = true): ?ResourceBundle {}

    /** @tentative-return-type */
    public function get(string|int $index, bool $fallback = true): ResourceBundle|array|string|int|null {}

    /** @param string|int $offset */
    public function offsetGet(mixed $offset): ResourceBundle|array|string|int|null {}

    /** @param string|int $offset */
    public function offsetExists(mixed $offset): bool {}

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
