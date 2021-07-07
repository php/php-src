<?php

/** @generate-class-entries */

class Collator
{
    public function __construct(string $locale) {}

    /**
     * @tentative-return-type
     * @alias collator_create
     */
    public static function create(string $locale): ?Collator {}

    /**
     * @tentative-return-type
     * @alias collator_compare
     */
    public function compare(string $string1, string $string2): int|false {}

    /**
     * @tentative-return-type
     * @alias collator_sort
     */
    public function sort(array &$array, int $flags = Collator::SORT_REGULAR): bool {}

    /**
     * @tentative-return-type
     * @alias collator_sort_with_sort_keys
     */
    public function sortWithSortKeys(array &$array): bool {}

    /**
     * @tentative-return-type
     * @alias collator_asort
     */
    public function asort(array &$array, int $flags = Collator::SORT_REGULAR): bool {}

    /**
     * @tentative-return-type
     * @alias collator_get_attribute
     */
    public function getAttribute(int $attribute): int|false {}

    /**
     * @tentative-return-type
     * @alias collator_set_attribute
     */
    public function setAttribute(int $attribute, int $value): bool {}

    /**
     * @tentative-return-type
     * @alias collator_get_strength
     */
    public function getStrength(): int {}

    /**
     * @return bool
     * @alias collator_set_strength
     */
    public function setStrength(int $strength) {} // TODO make return type void

    /**
     * @tentative-return-type
     * @alias collator_get_locale
     */
    public function getLocale(int $type): string|false {}

    /**
     * @tentative-return-type
     * @alias collator_get_error_code
     */
    public function getErrorCode(): int|false {}

    /**
     * @tentative-return-type
     * @alias collator_get_error_message
     */
    public function getErrorMessage(): string|false {}

    /**
     * @tentative-return-type
     * @alias collator_get_sort_key
     */
    public function getSortKey(string $string): string|false {}
}
