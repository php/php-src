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

function collator_create(string $locale): ?Collator {}

function collator_compare(Collator $object, string $str1, string $str2): int|false {}

function collator_get_attribute(Collator $object, int $attr): int|false {}

function collator_set_attribute(Collator $object, int $attr, int $val): bool {}

function collator_get_strength(Collator $object): int {}

function collator_set_strength(Collator $object, int $strength): bool {}

function collator_sort(Collator $object, array &$arr, int $sort_flag = Collator::SORT_REGULAR): bool {}

function collator_sort_with_sort_keys(Collator $object, array &$arr): bool {}

function collator_asort(Collator $object, array &$arr, int $sort_flag = Collator::SORT_REGULAR): bool {}

function collator_get_locale(Collator $object, int $type): string|false {}

function collator_get_error_code(Collator $object): int|false {}

function collator_get_error_message(Collator $object): string|false {}

function collator_get_sort_key(Collator $object, string $str): string|false {}
