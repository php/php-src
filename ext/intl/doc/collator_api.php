<?php
#############################################################################
# Object-oriented API
#############################################################################

/**
 * Collator class.
 *
 * This is a wrapper around ICU Collator C API (declared in ucol.h).
 *
 * Example:
 * <code>
 *
 * </code>
 *
 * @see http://www.icu-project.org/apiref/icu4c/ucol_8h.html
 * @see http://www.icu-project.org/apiref/icu4c/classCollator.html
 *
 */
class Collator {
#############################################################################
# Common constants.
#############################################################################

/**
 * Locale-related constants.
 *
 * These will be moved out of Collator when Locale class is created.
 */
	const ULOC_ACTUAL_LOCALE    = 0;
	const ULOC_VALID_LOCALE     = 1;
	const ULOC_REQUESTED_LOCALE = 2;

	/*
	 * WARNING:
	 * The values described here are NOT the actual values in PHP code.
	 * They are references to the ICU C definitions, so the line
	 *    const DEFAULT_STRENGTH = 'UCOL_DEFAULT_STRENGTH';
	 * actually means that Collator::DEFAULT_STRENGTH is the same as
	 * UCOL_DEFAULT_STRENGTH constant in the ICU library.
	 */
	/**
     * Valid attribute values.
     *
     * @see Collator::setAttribute()
     * @see collator_set_attribute()
     */
    const DEFAULT_VALUE    = 'UCOL_DEFAULT';
    const PRIMARY          = 'UCOL_PRIMARY';
    const SECONDARY        = 'UCOL_SECONDARY';
    const TERTIARY         = 'UCOL_TERTIARY';
    const DEFAULT_STRENGTH = 'UCOL_DEFAULT_STRENGTH';
    const QUATERNARY       = 'UCOL_QUATERNARY';
    const IDENTICAL        = 'UCOL_IDENTICAL';
    const OFF              = 'UCOL_OFF';
    const ON               = 'UCOL_ON';
    const SHIFTED          = 'UCOL_SHIFTED';
    const NON_IGNORABLE    = 'UCOL_NON_IGNORABLE';
    const LOWER_FIRST      = 'UCOL_LOWER_FIRST';
    const UPPER_FIRST      = 'UCOL_UPPER_FIRST';

    /**
     * Valid attribute names.
     *
     * @see Collator::setAttribute()
     * @see collator_set_attribute()
     */
    const FRENCH_COLLATION         = 'UCOL_FRENCH_COLLATION';
    const ALTERNATE_HANDLING       = 'UCOL_ALTERNATE_HANDLING';
    const CASE_FIRST               = 'UCOL_CASE_FIRST';
    const CASE_LEVEL               = 'UCOL_CASE_LEVEL';
    const NORMALIZATION_MODE       = 'UCOL_NORMALIZATION_MODE';
    const STRENGTH                 = 'UCOL_STRENGTH';
    const HIRAGANA_QUATERNARY_MODE = 'UCOL_HIRAGANA_QUATERNARY_MODE';
    const NUMERIC_COLLATION        = 'UCOL_NUMERIC_COLLATION';

    /**
     * Create a collator
     *
     * @param string $locale The locale whose collation rules
     *                       should be used. Special values for
     *                       locales can be passed in - if null is
     *                       passed for the locale, the default
     *                       locale collation rules will be used. If
     *                       empty string ("") or "root" are passed,
     *                       UCA rules will be used.
     *
     * @return Collator     New instance of Collator object.
     */
    public function __construct( $locale ) {}

    /**
     * Create a collator
     *
     * Creates a new instance of Collator.
     *
     * This method is useful when you prefer just to get null on error,
     * as if you called collator_create().
     *
     * @return Collator      Newly created Collator instance,
     *                       or null on error.
     *
     * @see __construct()
     * @see collator_create()
     */
    public static function create( $locale ) {}

    /**
     * Get collator's last error code.
     *
     * @return  int  Error code returned by the last
     *               Collator method call.
     */
    public function getErrorCode() {}

    /**
     * Return error text for the last ICU operation.
     *
     * @return string Description of an error occurred in the last
     *                Collator method call.
     */
    public function getErrorMessage() {}

    /**
     * Compare two strings using PHP strcmp() semantics.
     *
     * Wrapper around ICU ucol_strcoll().
     *
     * @param string $str1  First string to compare.
     * @param string $str2  Second string to compare.
     *
     * @return int   1   if $str1 is  greater than  $str2;
     *               0   if $str1 is  equal to      $str2;
     *               -1  if $str1 is  less than     $str2.
     *               On error false is returned.
     */
    public function compare( $str1, $str2 ) {}

    /**
     * Equivalent to standard PHP sort() using Collator.
     *
     * @param array $arr         Array of strings to sort
     * @param int   $sort_flags  Optional sorting type, one of the following:
     *                           - SORT_REGULAR - compare items normally (don't change types)
     *                           - SORT_NUMERIC - compare items numerically
     *                           - SORT_STRING - compare items as strings
     *                           Default sorting type is SORT_REGULAR.
     *
     * @return bool true on success or false on failure.
     */
    public function sort( $arr, $sort_flags ) {}

    /**
     * Sort array maintaining index association.
     *
     * Equivalent to standard PHP asort() using Collator.
     *
     * @param array $arr         Array of strings to sort
     * @param int   $sort_flags  Optional sorting type
     *
     * @return bool true on success or false on failure.
     *
     * @see Collator::sort()
     */
    public function asort( $arr, $sort_flags ) {}

    /**
     * Equivalent to standard PHP sort() using Collator.
     *
     * Similar to Collator::sort().
     * Uses ICU ucol_getSortKey() to gain more speed on large arrays.
     *
     * @param array $arr  Array of strings to sort
     *
     * @return bool       true on success or false on failure.
     */
    public function sortWithSortKeys( $arr ) {}

    /**
     * @todo  Do we want to support other standard PHP sort functions:  ksort, rsort, asort?
     */

    /**
     * Get collation attribute value.
     *
     * Wrapper around ICU ucol_getAttribute().
     *
     * @param  int      $attr Attribute to get value for.
     *
     * @return int      Attribute value, or false on error.
     */
    public function getAttribute( $attr ) {}

    /**
     * Set collation attribute.
     *
     * Wrapper around ICU ucol_setAttribute().
     *
     * @param int       $attr Attribute.
     * @param int       $val  Attribute value.
     *
     * @return bool     true on success, false otherwise.
     */
    public function setAttribute( $attr, $val ) {}

    /**
     * Get current collation strength.
     *
     * Wrapper around ICU ucol_getStrength().
     *
     * @return int     Current collation strength, or false on error.
     */
    public function getStrength() {}

    /**
     * Set collation strength.
     *
     * Wrapper around ICU ucol_setStrength().
     *
     * @param int      $strength Strength to set.
     *
     * @return bool    true on success, false otherwise.
     */
    public function setStrength( $strength ) {}

    /**
     * Get the locale name of the collator.
     *
     * Wrapper around ICU ucol_getLocaleByType().
     *
     * @param int      $type You can choose between requested, valid
     *                       and actual locale
     *                       (ULOC_REQUESTED_LOCALE,
     *                       ULOC_VALID_LOCALE, ULOC_ACTUAL_LOCALE,
     *                       respectively).
     *
     * @return string        Real locale name from which the
     *                       collation data comes. If the collator
     *                       was instantiated from rules or an error occurred,
     *                       returns false.
     */
    public function getLocale( $type ) {}
}

#############################################################################
# Procedural API
#############################################################################

/**
 * Create collator.
 *
 * @param string     $locale  The locale containing the required
 *                            collation rules. Special values for
 *                            locales can be passed in - if null is
 *                            passed for the locale, the default
 *                            locale collation rules will be used. If
 *                            empty string ("") or "root" are passed,
 *                            UCA rules will be used.
 *
 * @return Collator  New instance of Collator object, or null on error.
 */
function collator_create( $locale ) {}

/**
 * Compare two strings.
 *
 * The strings will be compared using the options already
 * specified.
 *
 * @param Collator $coll Collator object.
 * @param string   $str1 The first string to compare.
 * @param string   $str2 The second string to compare.
 *
 * @return int     1   if $str1 is  greater than  $str2;
 *                 0   if $str1 is  equal to      $str2;
 *                 -1  if $str1 is  less than     $str2.
 *                 On error false is returned.
 *
 */
function collator_compare( $coll, $str1, $str2 ) {}

/**
 * Sort array using specified collator.
 *
 * @param  Collator $coll        Collator object.
 * @param  array    $arr         Array of strings to sort.
 * @param  int      $sort_flags  Optional sorting type, one of the following:
 *                               - SORT_REGULAR - compare items normally (don't change types)
 *                               - SORT_NUMERIC - compare items numerically
 *                               - SORT_STRING - compare items as strings
 *                               Default sorting type is SORT_REGULAR.
 *
 * @return bool     true on success or false on failure.
 */
function collator_sort( $coll, $arr, $sort_flags ) {}

/**
 * Sort array maintaining index association.
 *
 * @param Collator $coll        Collator object.
 * @param array    $arr         Array of strings to sort.
 * @param int      $sort_flags  Optional sorting type.
 *
 * @return bool    true on success or false on failure.
 *
 * @see collator_sort()
 */
function collator_asort( $coll, $arr, $sort_flags ) {}

/**
 * Sort array using specified collator.
 *
 * Similar to collator_sort().
 * Uses ICU ucol_getSortKey() to gain more speed on large arrays.
 *
 * @param  Collator $coll  Collator object.
 * @param  array    $arr   Array of strings to sort
 *
 * @return bool     true on success or false on failure.
 */
function collator_sort_with_sort_keys( $coll, $arr ) {}

/**
 * Get the locale name of the collator.
 *
 * @param Collator $coll Collator object.
 * @param int      $type You can choose between valid and 
 *                       actual locale
 *                       (ULOC_VALID_LOCALE, ULOC_ACTUAL_LOCALE
 *                       respectively).
 *
 * @return string  Real locale name from which the
 *                 collation data comes. If the collator
 *                 was instantiated from rules or an error occurred,
 *                 returns false.
 */
function collator_get_locale( $coll, $type ) {}

/**
 * Get collation attribute value.
 *
 * @param  Collator $coll Collator object.
 * @param  int      $attr Attribute to get value for.
 *
 * @return int      Attribute value, or false on error.
 */
function collator_get_attribute( $coll, $attr ) {}

/**
 * Get current collation strength.
 *
 * @param Collator $coll Collator object.
 *
 * @return int     Current collation strength, or false on error.
 */
function collator_get_strength( $coll ) {}

/**
 * Set collation strength.
 *
 * @param Collator $coll      Collator object.
 * @param int      $strength  Strength to set.
 *
 * @return bool    true on success, false otherwise.
 */
function collator_set_strength( $coll, $strength ) {}

/**
 * Set collation attribute.
 *
 * @param Collator  $coll  Collator object.
 * @param int       $attr  Attribute.
 * @param int       $val   Attribute value.
 *
 * @return bool     true on success, false otherwise.
 */
function collator_set_attribute( $coll, $attr, $val ) {}

/**
 * Get collator's last error code.
 *
 * @param Collator $coll    Collator object.
 *
 * @return int     Error code returned by the last
 *                 Collator API function call.
 */
function collator_get_error_code( $coll ) {}

/**
 * Get text for collator's last error code.
 *
 * @param Collator $coll    Collator object.
 *
 * @return string  Description of an error occurred in the last
 *                 Collator API function call.
 */
function collator_get_error_message( $coll ) {}
