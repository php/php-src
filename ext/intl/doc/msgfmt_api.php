<?php

/**
 * Message formatter class.
 *
 * Message Format provides for runtime formatting of messages in a manner
 * somewhat similar to sprintf. The pattern string has its component parts
 * replaced in a locale-sensitive manner using items in the arguments array.
 *
 * @see http://www.icu-project.org/apiref/icu4c/umsg_8h.html
 *
 */
class MessageFormatter {

	/**
	 * Constructs a new Message Formatter
	 * 
	 * @param  string   $locale  the locale to use when formatting arguments
	 * @param  string   $pattern the pattern string to stick arguments into
	 */
	public function __construct($locale, $pattern) {}

	/**
	 * Constructs a new Message Formatter
	 * 
	 * @param  string   $locale  the locale to use when formatting arguments
	 * @param  string   $pattern the pattern string to stick arguments into
	 */
	public static function create($locale, $pattern) {}

	/**
	 * Format the message
	 * @param   array   $args   arguments to insert into the pattern string
	 * @return  string  the formatted string, or false if an error occurred
	 */
	public function format($args) {}

	/**
	 * Parse input string and returns any extracted items as an array
	 *
	 * $error will contain any error code. If an error occurs, $parse_pos contains
	 * the position of the error.
	 *
	 * @param  string  $value      string to parse for items
	 * @return array               array containing items extracted
	 *
	 */
	public function parse($value) {}

	/**
	 * Inserts the items in $args into $pattern, formatting them
	 * according to $locale. This is the static implementation.
	 *
	 * @param string    $locale   the locale to use when formatting numbers and dates and suchlike
	 * @param string    $pattern  the pattern string to insert things into
	 * @param array     $args     the array of values to insert into $pattern
	 * @return string             the formatted pattern string or false if an error occurred
	 */
	public static function formatMessage($locale, $pattern, $args) {}

	/**
	 * parses input string and returns any extracted items as an array
	 *
	 * $error will contain any error code. If an error occurs, $parse_pos contains
	 * the position of the error.
	 *
	 * @param  string  $locale     the locale to use when formatting numbers and dates and suchlike
	 * @param  string  $value      string to parse for items
	 * @return array               array containing items extracted
	 *
	 */
	public static function parseMessage($locale, $value) {}

	/**
	 * Get the pattern used by the formatter
	 *
	 * @return  string  the pattern string for this message formatter
	 */
	public function getPattern() {}

	/**
	 * Set the pattern used by the formatter
	 *
	 * @param  string  $pattern  the pattern string to use in this message formatter
	 * @return boolean 'true' if successful, 'false' if an error
	 */
	public function setPattern($pattern) {}

	/**
	 * Get the error code from last operation
	 *
	 * Returns error code from the last number formatting operation.
	 *
	 * @return integer the error code, one of UErrorCode values. Initial value is U_ZERO_ERROR.
	 */
	public function getErrorCode() {}
	/**
	 * Get the error text from the last operation.
	 *
	 * @return string Description of the last error.
	 */
	public function getErrorMessage() {}
	/**
	 * Get the locale for which the formatter was created.
	 *
	 * @return string locale name
	 */
	public function getLocale() {}
}

	/** Now the same as procedural API */


	/**
	 * Constructs a new Message Formatter
	 * 
	 * @param  string   $locale  the locale to use when formatting arguments
	 * @param  string   $pattern the pattern string to stick arguments into
	 * @return MessageFormatter formatter object
	 */
	function msgfmt_create($locale, $pattern) {}

	/**
	 * Format the message
	 * @param MessageFormatter $fmt The message formatter 
	 * @param   array   $args   arguments to insert into the pattern string
	 * @return  string  the formatted string, or false if an error occurred
	 */
	function msgfmt_format($fmt, $args) {}

	/**
	 * parses input string and returns any extracted items as an array
	 *
	 * $error will contain any error code. If an error occurs, $parse_pos contains
	 * the position of the error.
	 *
	 * @param MessageFormatter $fmt The message formatter 
	 * @param  string  $value      string to parse for items
	 * @return array               array containing items extracted
	 *
	 */
	function msgfmt_parse($fmt, $value) {}

	/**
	 * Inserts the items in $args into $pattern, formatting them
	 * according to $locale. This is the static implementation.
	 *
	 * @param string    $locale   the locale to use when formatting numbers and dates and suchlike
	 * @param string    $pattern  the pattern string to insert things into
	 * @param array     $args     the array of values to insert into $pattern
	 * @return string             the formatted pattern string or false if an error occurred
	 */
	function msgfmt_format_message($locale, $pattern, $args) {}

	/**
	 * parses input string and returns any extracted items as an array
	 *
	 * $error will contain any error code. If an error occurs, $parse_pos contains
	 * the position of the error.
	 *
	 * @param  string  $locale     the locale to use when formatting numbers and dates and suchlike
	 * @param  string  $value      string to parse for items
	 * @return array               array containing items extracted
	 *
	 */
	function msgfmt_parse_message($locale, $value) {}

	/**
	 * Get the pattern used by the formatter
	 *
	 * @param MessageFormatter $fmt The message formatter 
	 * @return  string  the pattern string for this message formatter
	 */
	function msgfmt_get_pattern($fmt) {}

	/**
	 * Set the pattern used by the formatter
	 *
	 * @param MessageFormatter $fmt The message formatter 
	 * @param  string  $pattern  the pattern string to use in this message formatter
	 * @return boolean 'true' if successful, 'false' if an error
	 */
	function msgfmt_set_pattern($fmt, $pattern) {}

	/**
	 * Get the error code from last operation
	 *
	 * Returns error code from the last number formatting operation.
	 *
	 * @param MessageFormatter $fmt The message formatter 
	 * @return integer the error code, one of UErrorCode values. Initial value is U_ZERO_ERROR.
	 */
	function msgfmt_get_error_code($fmt) {}
	/**
	 * Get the error text from the last operation.
	 *
	 * @param MessageFormatter $fmt The message formatter 
	 * @return string Description of the last error.
	 */
	function msgfmt_get_error_message($fmt) {}

	/**
	 * Get the locale for which the formatter was created.
	 *
	 * @param NumberFormatter $formatter The formatter resource
	 * @return string locale name
	 */
	function msgfmt_get_locale($formatter) {}
?>
