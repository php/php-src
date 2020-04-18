<?php

/** @generate-function-entries */

class Collator
{
    public function __construct(string $locale) {}

    /**
     * @return Collator|null
     * @alias collator_create
     */
    public static function create(string $locale) {}

    /**
     * @return int|false
     * @alias collator_compare
     */
    public function compare(string $str1, string $str2) {}

    /**
     * @return bool
     * @alias collator_sort
     */
    public function sort(array &$arr, $sort_flag = Collator::SORT_REGULAR) {}

    /**
     * @return bool
     * @alias collator_sort_with_sort_keys
     */
    public function sortWithSortKeys(array &$arr) {}

    /**
     * @return bool
     * @alias collator_asort
     */
    public function asort(array &$arr, int $sort_flag = Collator::SORT_REGULAR) {}

    /**
     * @return int|false
     * @alias collator_get_attribute
     */
    public function getAttribute(int $attr) {}

    /**
     * @return bool
     * @alias collator_set_attribute
     */
    public function setAttribute(int $attr, int $val) {}

    /**
     * @return int
     * @alias collator_get_strength
     */
    public function getStrength() {}

    /**
     * @return bool
     * @alias collator_set_strength
     */
    public function setStrength(int $strength) {}

    /**
     * @return string|false
     * @alias collator_get_locale
     */
    public function getLocale(int $type) {}

    /**
     * @return int|false
     * @alias collator_get_error_code
     */
    public function getErrorCode() {}

    /**
     * @return string|false
     * @alias collator_get_error_message
     */
    public function getErrorMessage() {}

    /**
     * @return string|false
     * @alias collator_get_sort_key
     */
    public function getSortKey(string $str) {}
}
