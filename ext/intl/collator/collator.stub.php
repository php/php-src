<?php

class Collator
{
    public function __construct(string $locale) {}

    /** @return Collator|null */
    public static function create(string $locale) {}

    /** @return int|false */
    public function compare(string $str1, string $str2) {}

    /** @return bool */
    public function sort(array &$arr, $sort_flag = Collator::SORT_REGULAR) {}

    /** @return bool */
    public function sortWithSortKeys(array &$arr) {}

    /** @return bool */
    public function asort(array &$arr, int $sort_flag = Collator::SORT_REGULAR) {}

    /** @return int|false */
    public function getAttribute(int $attr) {}

    /** @return bool */
    public function setAttribute(int $attr, int $val) {}

    /** @return int */
    public function getStrength() {}

    /** @return bool */
    public function setStrength(int $strength) {}

    /** @return string|false */
    public function getLocale(int $type) {}

    /** @return int|false */
    public function getErrorCode() {}

    /** @return string|false */
    public function getErrorMessage() {}

    /** @return string|false */
    public function getSortKey(string $str) {}
}
