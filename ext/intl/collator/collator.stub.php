<?php

/** @generate-class-entries */

/**
 * @var int
 * @cvalue ULOC_ACTUAL_LOCALE
 */
const ULOC_ACTUAL_LOCALE = UNKNOWN;
/**
 * @var int
 * @cvalue ULOC_VALID_LOCALE
 */
const ULOC_VALID_LOCALE = UNKNOWN;

/** @not-serializable */
class Collator
{
    /**
     * @var int
     * @cvalue UCOL_DEFAULT
     */
    const DEFAULT_VALUE = UNKNOWN;

    /**
     * @var int
     * @cvalue UCOL_PRIMARY
     */
    const PRIMARY = UNKNOWN;
    /**
     * @var int
     * @cvalue UCOL_SECONDARY
     */
    const SECONDARY = UNKNOWN;
    /**
     * @var int
     * @cvalue UCOL_TERTIARY
     */
    const TERTIARY = UNKNOWN;
    /**
     * @var int
     * @cvalue UCOL_DEFAULT_STRENGTH
     */
    const DEFAULT_STRENGTH = UNKNOWN;
    /**
     * @var int
     * @cvalue UCOL_QUATERNARY
     */
    const QUATERNARY = UNKNOWN;
    /**
     * @var int
     * @cvalue UCOL_IDENTICAL
     */
    const IDENTICAL = UNKNOWN;

    /**
     * @var int
     * @cvalue UCOL_OFF
     */
    const OFF = UNKNOWN;
    /**
     * @var int
     * @cvalue UCOL_ON
     */
    const ON = UNKNOWN;

    /**
     * @var int
     * @cvalue UCOL_SHIFTED
     */
    const SHIFTED = UNKNOWN;
    /**
     * @var int
     * @cvalue UCOL_NON_IGNORABLE
     */
    const NON_IGNORABLE = UNKNOWN;

    /**
     * @var int
     * @cvalue UCOL_LOWER_FIRST
     */
    const LOWER_FIRST = UNKNOWN;
    /**
     * @var int
     * @cvalue UCOL_UPPER_FIRST
     */
    const UPPER_FIRST = UNKNOWN;

    /* UColAttribute constants */

    /**
     * @var int
     * @cvalue UCOL_FRENCH_COLLATION
     */
    const FRENCH_COLLATION = UNKNOWN;
    /**
     * @var int
     * @cvalue UCOL_ALTERNATE_HANDLING
     */
    const ALTERNATE_HANDLING = UNKNOWN;
    /**
     * @var int
     * @cvalue UCOL_CASE_FIRST
     */
    const CASE_FIRST = UNKNOWN;
    /**
     * @var int
     * @cvalue UCOL_CASE_LEVEL
     */
    const CASE_LEVEL = UNKNOWN;
    /**
     * @var int
     * @cvalue UCOL_NORMALIZATION_MODE
     */
    const NORMALIZATION_MODE = UNKNOWN;
    /**
     * @var int
     * @cvalue UCOL_STRENGTH
     */
    const STRENGTH = UNKNOWN;
    /**
     * @var int
     * @cvalue UCOL_HIRAGANA_QUATERNARY_MODE
     */
    const HIRAGANA_QUATERNARY_MODE = UNKNOWN;
    /**
     * @var int
     * @cvalue UCOL_NUMERIC_COLLATION
     */
    const NUMERIC_COLLATION = UNKNOWN;

    /* sort flags */

    /**
     * @var int
     * @cvalue COLLATOR_SORT_REGULAR
     */
    const SORT_REGULAR = UNKNOWN;
    /**
     * @var int
     * @cvalue COLLATOR_SORT_STRING
     */
    const SORT_STRING = UNKNOWN;
    /**
     * @var int
     * @cvalue COLLATOR_SORT_NUMERIC
     */
    const SORT_NUMERIC = UNKNOWN;

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
