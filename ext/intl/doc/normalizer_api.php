<?php

#############################################################################
# Object-oriented API
#############################################################################

/**
 * Normalizer class.
 *
 * Normalizer provides access to Unicode normalization of strings. This class consists
 * only of static methods. The iterator interface to normalizer is rarely used, so is
 * not provided here.
 *
 * Example:
 * <code>
 *
 * </code>
 *
 * @see http://www.icu-project.org/apiref/icu4c/unorm_8h.html
 * @see http://www.icu-project.org/apiref/icu4c/classNormalizer.html
 *
 */
class Normalizer {
#############################################################################
# Common constants.
#############################################################################

	/**
	 * Valid normalization form values.
	 *
	 * @see Normalizer::normalize()
	 * @see Normalizer::isNormalize()
	 * @see normalizer_normalize()
	 * @see normalizer_is_normalized()
	*/
	const NONE    = 1;
	/** Canonical decomposition. */
	const NFD = 2;
	const FORM_D  = NFD;
	/** Compatibility decomposition. */
	const NFKD = 3;
	const FORM_KD = NFKD;
	/** Canonical decomposition followed by canonical composition. */
	const NFC = 4;
	const FORM_C  = NFC;
	/** Compatibility decomposition followed by canonical composition. */
	const NFKC =5;
	const FORM_KC = NFKC;


	/**
	 * Normalizes the input provided and returns the normalized string
	 * @param   string     $input    The input string to normalize
	 * @param   [int]      $form     One of the normalization forms
	 * @return  string               The normalized string or null if an error occurred.
	*/
	public static function normalize($input, $form = Normalizer::FORM_C) {}

	/**
	 * Checks if the provided string is already in the specified normalization form.
	 * @param  string    $input    The input string to normalize
	 * @param  [int]     $form     One of the normalization forms
	 * @return boolean             True if normalized, false otherwise or if there is an error
	*/
	public static function isNormalized($input, $form = Normalizer::FORM_C) {}

}

#############################################################################
# Procedural API
#############################################################################

	/**
	 * Normalizes the input provided and returns the normalized string
	 * @param  string    $input    The input string to normalize
	 * @param  [int]     $form     One of the normalization forms
	 * @return string              The normalized string or null if an error occurred.
	*/
	function normalizer_normalize($input, $form = Normalizer::FORM_C) {}

	/**
	 * Checks if the provided string is already in the specified normalization form.
	 * @param  string    $input    The input string to normalize
	 * @param  [int]     $form     One of the normalization forms
	 * @return boolean             True if normalized, false otherwise or if there an error
	*/
	function normalizer_is_normalized($input, $form = Normalizer::FORM_C) {}


?>


