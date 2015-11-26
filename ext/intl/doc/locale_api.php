<?php

/**
 * A "Locale" is an identifier used to get language, culture, or regionally-specific 
 * behavior from an API. PHP locales are organized and identified the same
 * way that the CLDR locales used by ICU (and many vendors of Unix-like operating 
 * systems, the Mac, Java, and so forth) use. Locales are identified using
 * RFC 4646 language tags (which use hyphen, not underscore) in addition to the
 * more traditional underscore-using identifiers. Unless otherwise noted
 * the functions in this class are tolerant of both formats. 
 *
 * Examples of identifiers include:
 *
 *  * en-US (English, United States)
 *  * zh-Hant-TW (Chinese, Traditional Script, Taiwan)
 *  * fr-CA, fr-FR (French for Canada and France respectively) 
 *
 * The Locale class (and related procedural functions) are used to interact 
 * with locale identifiers--to verify that an ID is well-formed, valid, 
 * etc. The extensions used by CLDR in UAX #35 (and inherited by ICU) are 
 * valid and used wherever they would be in ICU normally.
 *
 * Locales cannot be instantiated as objects. All of the functions/methods 
 * provided are static.
 *
 *   * The null or empty string obtains the "root" locale. 
 *     The "root" locale is equivalent to "en_US_POSIX" in CLDR.
 *   * Language tags (and thus locale identifiers) are case insensitive. There
 *     exists a canonicalization function to make case match the specification.
 *
 * @see http://www.icu-project.org/apiref/icu4c/uloc_8h.html
 * @see http://www.unicode.org/reports/tr35/
 *
 */
class Locale {

#############################################################################
# Common constants.
#############################################################################

   	/**
     	 * The following static members are used with the getLocale methods of
     	 * the various locale affected classes, such as numfmt.
     	 */
  	const DEFAULT_LOCALE 			= null;

   	/**
	 * identifiers for the actual locale, valid locale
 	 * WARNING:
	 * The values described here are NOT the actual values in PHP code.
	 * They are references to the ICU C definitions, so the line
	 *    const ACTUAL_LOCALE = 'ULOC_ACTUAL_LOCALE';
	 * actually means that Locale::ACTUAL_LOCALE is the same as
	 * ULOC_ACTUAL_LOCALE constant in the ICU library.
	 */
	const ACTUAL_LOCALE 			= 'ULOC_ACTUAL_LOCALE';
   	const VALID_LOCALE 			= 'ULOC_VALID_LOCALE';

	/** 
	 * Valid locale tag and subtag values
	 */
	const LANG_TAG 				= "language";
	const EXTLANG_TAG 			= "extlang";
	const SCRIPT_TAG 			= "script";
	const REGION_TAG 			= "region";
	const VARIANT_TAG 			= "variant";
	const GRANDFATHERED_LANG_TAG 		= "grandfathered";
	const PRIVATE_TAG 			= "private";
	

#############################################################################
# Object-oriented API
#############################################################################

	/**
	 * Gets the default locale value from the INTL global 'default_locale'
	 * At the PHP initilaization (MINIT) this value is set to 
	 * 'intl.default_locale' value from php.ini if that value exists 
	 * or from ICU's function  uloc_getDefault() 
	 * Then onwards picks up changes from setDefault() calls also
	 *
     	 * @return string the current runtime locale 
     	 */
	public static function getDefault() {}

	/**
     	 * sets the default runtime locale to $locale
 	 * This changes the value of INTL global 'default_locale'
     	 *
     	 * @param 	string	 $locale 	is a BCP 47 compliant language tag containing the 
     	 *					locale identifier. UAX #35 extensions are accepted.
     	 * @return	boolean 		'true' if okay, 'false' if an error
     	 */
    	public static function setDefault($locale) {}


    	/**
	 * Gets the primary language for the input locale
	 *
     	 * @param	string	$locale 	the locale to extract the primary language code from
     	 * @return 	string 			the language code associated with the language 
	 *					or null in case of error. 
     	 */
    	public static function getPrimaryLanguage($locale) {}


    	/**
	 * Gets the script for the input locale
	 *
     	 * @param 	string 	$locale 	the locale to extract the script code from
     	 * @return 	string 			the script subtag for the locale or null if not present
     	 */
    	public static function getScript($locale) {}


    	/**
	 * Gets the region for the input locale
	 *
     	 * @param 	string 	$locale 	the locale to extract the region code from
     	 * @return 	string 			the region subtag for the locale or null if not present
     	 */
    	public static function getRegion($locale) {}


    	/**
	 * Gets the variants for the input locale
	 *
    	 * @param 	string 	$locale 	the locale to extract the variants from
     	 * @return 	array 			the array containing the list of all variants 
	 *					subtag for the locale or null if not present
     	 */
	public static function getAllVariants($locale) {}


	/**
	 * Gets the keywords for the input locale
	 *
     	 * @param 	string 	$locale 	the locale to extract the keywords from
     	 * @return 	array 			associative array containing the keyword-value pairs for this locale 
     	 */
    	public static function getKeywords($locale) {}


    	/**
	 * Returns an appropriately localized display name for the input locale
	 *
    	 * @param 	string 	 	$locale 	the locale to return a displayname for
	 * @param 	[string] 	$in_locale  	optional format locale
	 * 						If is 'null' then the default locale is used. 
	 * @return 	string  			display name of the locale in the format
	 * 						appropriate for $in_locale. 
     	 */
    	public static function getDisplayName($locale, $in_locale = null) {}


    	/**
	 * Returns an appropriately localized display name for language of the input locale
	 *
     	 * @param 	string 	 	$locale 	the locale to return a display language for
	 * @param 	[string] 	$in_locale  	optional format locale to use to display the language name
	 * 						If is 'null' then the default locale is used. 
	 * @return 	string  			display name of the language for the $locale in the format
	 * 						appropriate for $in_locale. 
     	 */
	public static function getDisplayLanguage($lang, $in_locale = null) {}

    	/**
	 * Returns an appropriately localized display name for script of the input locale
	 *
    	 * @param 	string 	 	$locale 	the locale to return a display script for
	 * @param 	[string] 	$in_locale  	optional format locale to use to display the script name
	 * 						If is 'null' then the default locale is used. 
	 * @return 	string  			display name of the script for the $locale in the format
	 * 						appropriate for $in_locale. 
     	 */
	public static function getDisplayScript($script, $in_locale = null) {}


    	/**
	 * Returns an appropriately localized display name for region of the input locale
	 *
    	 * @param 	string 	 	$locale 	the locale to return a display region for
	 * @param 	[string] 	$in_locale  	optional format locale to use to display the region name
	 * 						If is 'null' then the default locale is used. 
	 * @return 	string  			display name of the region for the $locale in the format
	 * 						appropriate for $in_locale. 
     	 */
	public static function getDisplayRegion($region, $in_locale = null) {}


    	/**
	 * Returns an appropriately localized display name for variants of the input locale
	 *
    	 * @param 	string 	 	$locale 	the locale to return a display variant for
	 * @param 	[string] 	$in_locale  	optional format locale to use to display the variant name
	 * 						If is 'null' then the default locale is used. 
	 * @return 	string  			display name of the variant for the $locale in the format
	 * 						appropriate for $in_locale. 
     	 */
	public static function getDisplayVariant($variant, $in_locale = null) {}


    	/**
	 * Checks if a $langtag  filter matches with $locale according to 
	 * RFC 4647's basic filtering algorithm
	 *
     	 * @param 	string 		$langtag	the language tag to check
	 * @param 	string 		$locale		the language range to check against
	 * @param 	bool 		$canonicalize	Canonicalize parameters?
     	 * @return 	boolean 			'true' if $locale matches $langtag 'false' otherwise
	 */
    	public static function filterMatches($langtag, $locale, $canonicalize) {}

	/**
     	 * Searchs the items in $langtag for the best match to the language
	 * range specified in $locale according to RFC 4647's lookup algorithm. 
	 *
	 * @param 	array 		$langtag  	an array containing a list of language tags to compare
	 *            					to $locale
	 * @param 	string 		$locale   	the locale to use as the language range when matching
	 * @param 	string 		$default  	the locale to use if no match is found
	 * @return 	string 				closest matching language tag, $default, 
	 *						or empty string
	 */
    	public static function lookup(array $langtag, $locale, $default = null) {}


	/**
     	 * Returns a correctly ordered and delimited locale ID 
	 *
	 * @param 	array 		$subtags 	an array containing a list of key-value pairs, where 
	 * 						the keys identify the particular locale ID subtags, 	
	 *						and the values are the associated subtag values. 
	 *
	 * @return 	string 				the corresponding locale identifier.
	 */
    	public static function composeLocale(array $subtags) {}


	/**
	 * Returns a key-value array of locale ID subtag elements.
	 *
	 * @param 	string		$locale		the locale to extract the subtag array from 					
	 *
	 * @return 	array 		$subtags 	an array containing a list of key-value pairs, where 
	 * 						the keys identify the particular locale ID subtags, 	
	 *						and the values are the associated subtag values. 
	 */
    	public static function parseLocale($locale) {}

}

#############################################################################
# Procedural API
#############################################################################


	/**
	 * Gets the default locale value from the INTL global 'default_locale'
	 * At the PHP initilaization (MINIT) this value is set to 
	 * 'intl.default_locale' value from php.ini if that value exists 
	 * or from ICU's function  uloc_getDefault() 
	 * Then onwards picks up changes from setDefault() calls also
	 *
     	 * @return string the current runtime locale 
     	 */
	 function locale_get_default() {}

	/**
     	 * sets the default runtime locale to $locale
 	 * This changes the value of INTL global 'default_locale'
     	 *
     	 * @param 	string	 $locale 	is a BCP 47 compliant language tag containing the 
     	 *					locale identifier. UAX #35 extensions are accepted.
     	 * @return	boolean 		'true' if okay, 'false' if an error
     	 */
    	 function locale_set_default($locale) {}


    	/**
	 * Gets the primary language for the input locale
	 *
     	 * @param	string	$locale 	the locale to extract the primary language code from
     	 * @return 	string 			the language code associated with the language 
	 *					or null in case of error. 
     	 */
    	 function locale_get_primary_language($locale) {}


    	/**
	 * Gets the script for the input locale
	 *
     	 * @param 	string 	$locale 	the locale to extract the script code from
     	 * @return 	string 			the script subtag for the locale or null if not present
     	 */
    	 function locale_get_script($locale) {}


    	/**
	 * Gets the region for the input locale
	 *
     	 * @param 	string 	$locale 	the locale to extract the region code from
     	 * @return 	string 			the region subtag for the locale or null if not present
     	 */
    	 function locale_get_region($locale) {}


    	/**
	 * Gets the variants for the input locale
	 *
    	 * @param 	string 	$locale 	the locale to extract the variants from
     	 * @return 	array 			the array containing the list of all variants 
	 *					subtag for the locale or null if not present
     	 */
	 function locale_get_all_variants($locale) {}


	/**
	 * Gets the keywords for the input locale
	 *
     	 * @param 	string 	$locale 	the locale to extract the keywords from
     	 * @return 	array 			associative array containing the keyword-value pairs for this locale 
     	 */
    	 function locale_get_keywords($locale) {}


    	/**
	 * Returns an appropriately localized display name for the input locale
	 *
    	 * @param 	string 	 	$locale 	the locale to return a displayname for
	 * @param 	[string] 	$in_locale  	optional format locale
	 * 						If is 'null' then the default locale is used. 
	 * @return 	string  			display name of the locale in the format
	 * 						appropriate for $in_locale. 
     	 */
    	 function locale_get_display_name($locale, $in_locale = null) {}


    	/**
	 * Returns an appropriately localized display name for language of the input locale
	 *
     	 * @param 	string 	 	$locale 	the locale to return a display language for
	 * @param 	[string] 	$in_locale  	optional format locale to use to display the language name
	 * 						If is 'null' then the default locale is used. 
	 * @return 	string  			display name of the language for the $locale in the format
	 * 						appropriate for $in_locale. 
     	 */
	 function locale_get_display_language($lang, $in_locale = null) {}

    	/**
	 * Returns an appropriately localized display name for script of the input locale
	 *
    	 * @param 	string 	 	$locale 	the locale to return a display script for
	 * @param 	[string] 	$in_locale  	optional format locale to use to display the script name
	 * 						If is 'null' then the default locale is used. 
	 * @return 	string  			display name of the script for the $locale in the format
	 * 						appropriate for $in_locale. 
     	 */
	 function locale_get_display_script($script, $in_locale = null) {}


    	/**
	 * Returns an appropriately localized display name for region of the input locale
	 *
    	 * @param 	string 	 	$locale 	the locale to return a display region for
	 * @param 	[string] 	$in_locale  	optional format locale to use to display the region name
	 * 						If is 'null' then the default locale is used. 
	 * @return 	string  			display name of the region for the $locale in the format
	 * 						appropriate for $in_locale. 
     	 */
	 function locale_get_display_region($region, $in_locale = null) {}


    	/**
	 * Returns an appropriately localized display name for variants of the input locale
	 *
    	 * @param 	string 	 	$locale 	the locale to return a display variant for
	 * @param 	[string] 	$in_locale  	optional format locale to use to display the variant name
	 * 						If is 'null' then the default locale is used. 
	 * @return 	string  			display name of the variant for the $locale in the format
	 * 						appropriate for $in_locale. 
     	 */
	 function locale_get_display_variant($variant, $in_locale = null) {}


    	/**
	 * Checks if a $langtag  filter matches with $locale according to 
	 * RFC 4647's basic filtering algorithm
	 *
     	 * @param 	string 		$langtag	the language tag to check
	 * @param 	string 		$locale		the language range to check against
	 * @param 	bool 		$canonicalize	Canonicalize parameters?
     	 * @return 	boolean 			'true' if $locale matches $langtag 'false' otherwise
	 */
    	 function locale_filter_matches($langtag, $locale, $canonicalize) {}

	/**
     	 * Searchs the items in $langtag for the best match to the language
	 * range specified in $locale according to RFC 4647's lookup algorithm. 
	 *
	 * @param 	array 		$langtag  	an array containing a list of language tags to compare
	 *            					to $locale
	 * @param 	string 		$locale   	the locale to use as the language range when matching
	 * @param 	string 		$default  	the locale to use if no match is found
	 * @return 	string 				closest matching language tag, $default, 
	 *						or empty string
	 */
    	 function locale_lookup(array $langtag, $locale, $default = null) {}


	/**
     	 * Returns a correctly ordered and delimited locale ID 
	 *
	 * @param 	array 		$subtags 	an array containing a list of key-value pairs, where 
	 * 						the keys identify the particular locale ID subtags, 	
	 *						and the values are the associated subtag values. 
	 *
	 * @return 	string 				the corresponding locale identifier.
	 */
    	 function locale_compose_locale(array $subtags) {}


	/**
	 * Returns a key-value array of locale ID subtag elements.
	 *
	 * @param 	string		$locale		the locale to extract the subtag array from 					
	 *
	 * @return 	array 		$subtags 	an array containing a list of key-value pairs, where 
	 * 						the keys identify the particular locale ID subtags, 	
	 *						and the values are the associated subtag values. 
	 */
    	 function locale_parse_locale($locale) {}

?>
