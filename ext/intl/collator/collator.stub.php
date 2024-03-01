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
    /** @cvalue UCOL_DEFAULT */
    public const int DEFAULT_VALUE = UNKNOWN;

    /** @cvalue UCOL_PRIMARY */
    public const int PRIMARY = UNKNOWN;
    /** @cvalue UCOL_SECONDARY */
    public const int SECONDARY = UNKNOWN;
    /** @cvalue UCOL_TERTIARY */
    public const int TERTIARY = UNKNOWN;
    /** @cvalue UCOL_DEFAULT_STRENGTH */
    public const int DEFAULT_STRENGTH = UNKNOWN;
    /** @cvalue UCOL_QUATERNARY */
    public const int QUATERNARY = UNKNOWN;
    /** @cvalue UCOL_IDENTICAL */
    public const int IDENTICAL = UNKNOWN;

    /** @cvalue UCOL_OFF */
    public const int OFF = UNKNOWN;
    /** @cvalue UCOL_ON */
    public const int ON = UNKNOWN;

    /** @cvalue UCOL_SHIFTED */
    public const int SHIFTED = UNKNOWN;
    /** @cvalue UCOL_NON_IGNORABLE */
    public const int NON_IGNORABLE = UNKNOWN;

    /** @cvalue UCOL_LOWER_FIRST */
    public const int LOWER_FIRST = UNKNOWN;
    /** @cvalue UCOL_UPPER_FIRST */
    public const int UPPER_FIRST = UNKNOWN;

    /* UColAttribute constants */

    /** @cvalue UCOL_FRENCH_COLLATION */
    public const int FRENCH_COLLATION = UNKNOWN;
    /** @cvalue UCOL_ALTERNATE_HANDLING */
    public const int ALTERNATE_HANDLING = UNKNOWN;
    /** @cvalue UCOL_CASE_FIRST */
    public const int CASE_FIRST = UNKNOWN;
    /** @cvalue UCOL_CASE_LEVEL */
    public const int CASE_LEVEL = UNKNOWN;
    /** @cvalue UCOL_NORMALIZATION_MODE */
    public const int NORMALIZATION_MODE = UNKNOWN;
    /** @cvalue UCOL_STRENGTH */
    public const int STRENGTH = UNKNOWN;
    /** @cvalue UCOL_HIRAGANA_QUATERNARY_MODE */
    public const int HIRAGANA_QUATERNARY_MODE = UNKNOWN;
    /** @cvalue UCOL_NUMERIC_COLLATION */
    public const int NUMERIC_COLLATION = UNKNOWN;

    /* sort flags */

    /** @cvalue COLLATOR_SORT_REGULAR */
    public const int SORT_REGULAR = UNKNOWN;
    /** @cvalue COLLATOR_SORT_STRING */
    public const int SORT_STRING = UNKNOWN;
    /** @cvalue COLLATOR_SORT_NUMERIC */
    public const int SORT_NUMERIC = UNKNOWN;

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
