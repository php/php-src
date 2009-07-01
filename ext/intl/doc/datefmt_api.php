<?php

/**
 * Date Formatter class - locale-dependent formatting/parsing of dates using pattern strings and/or canned patterns.
 *
 * This class represents the ICU date formatting functionality. It allows users to
 * display dates in a localized format or to parse strings 
 * into PHP date values using pattern strings and/or canned patterns.
 * 
 * Example:
 * <code>
 * $datefmt = new DateFormatter("de-DE", LONG, SHORT, date_default_timezone_get());
 * echo $formatter->format(time());
 * </code>
 *
 * <code>
 *  $datefmt = new DateFormatter("de-DE", LONG, SHORT, date_default_timezone_get() , GREGORIAN , "yyyy-MM-dd HH:mm:ss z");
 *  echo $formatter->format(time());
 * </code>
 *
 * @see http://www.icu-project.org/apiref/icu4c/udat_8h.html
 *
 */
class DateFormatter {

#############################################################################
# Common constants.
#############################################################################

   /**
    * The following constants are used to specify different formats
    * in the constructor.
    */
    const NONE = -1;
    const FULL = 0;
    const LONG = 1;
    const MEDIUM = 2;
    const SHORT = 3;

    /**
     * The following int constants are used to specify the calendar. 
     * These calendars are all based directly on the Gregorian calendar 
     * Non-Gregorian calendars need to be specified in locale. 
     * Examples might include locale="hi@calendar=BUDDHIST"
     */
    const TRADITIONAL = 0; // non-Gregorian calendar that is locale-defined, required by ICU
    const GREGORIAN = 1 ;// Gregorian calendar
	
#############################################################################
# Object-oriented API
#############################################################################
	/**
	 * Create a date formatter 
	 *
	 * @param string  $locale     Locale to use when formatting or parsing
	 * @param integer $datetype   Date type to use (none, short, medium, long, full)
	 * @param integer $timetype   Time type to use (none, short, medium, long, full)
	 * @param [String] $timezone  Time zone ID ; default is system default
	 * @param [integer] $calendar Calendar to use for formatting or parsing; default is
	 *                            GREGORIAN
	 * @param [string]  $pattern  Optional pattern to use when formatting or parsing
	 * @return DateFormatter
	 * @see __construct
	 * @see datefmt_create
	 */
	public function __construct($locale, $datetype, $timetype, $timezone = null, $calendar= null , $pattern= null) {}

	/**
	 * Create a date formatter 
	 *
	 * @param string  $locale     Locale to use when formatting or parsing
	 * @param integer $datetype   Date type to use (none, short, medium, long, full)
	 * @param integer $timetype   Time type to use (none, short, medium, long, full)
	 * @param [string] $timezone  Time zone ID ; default is system default
	 * @param [integer] $calendar Calendar to use for formatting or parsing; default is
	 *                            GREGORIAN
	 * @param [string]  $pattern  Optional pattern to use when formatting or parsing
	 * @return DateFormatter
	 * @see __construct
	 * @see datefmt_create
	 */
	public static function create($locale, $datetype, $timetype, $timezone = null, $calendar= null , $pattern= null) {}

	/**
         * formats the time value as a string.
	 * @param mixed $value - value to format
         *     integer: a unix timestamp value (seconds since epoch, UTC)
         *     array: a localtime array  - uses 24 hour clock in tm_hour field
         * @return string  a formatted string or, if an error occurred, 'null'. 
	 */
        public function format($value) {}


        /**
         * converts string $value to an incremental time value, starting at
	 * $parse_pos and consuming as much of the input value as possible
	 * If no error occurs before $value is consumed, $parse_pos will contain -1
	 * otherwise it will contain the position at which parsing ended (and the error
	 * occurred). 
	 * @param string  $value      string to convert to a time
	 * @param integer $parse_pos  position at which to start the parsing in $value (zero-based)
	 *                            This variable will contain the end position if the parse fails
	 *                            If $parse_pos > strlen($value), the parse fails immediately.
	 * @return integer timestamp  parsed value
	 */
	 public function parse($value, $parse_pos=0) {}

	 
        /**
         * converts string $value to a field-based time value, starting at
	 * $parse_pos and consuming as much of the input value as possible
	 * If no error occurs before $value is consumed, $parse_pos will contain -1
	 * otherwise it will contain the position at which parsing ended (and the error
	 * occurred). 
	 * @param string  $value      string to convert to a time
	 * @param integer $parse_pos  position at which to start the parsing in $value (zero-based)
	 *                            This variable will contain the end position if the parse fails
	 *                            If $parse_pos > strlen($value), the parse fails immediately.
	 * @return array localtime compatible array of integers  - uses 24 hour clock in tm_hour field
	 */
	 public function localtime($value, $parse_pos=0) {}


	 /**
	  * Gets the datetype in use 
	  * @return integer the current 'datetype' value of the formatter
	  */
         public function getDateType() {}


	 /**
	  * Gets the timetype in use 
	  * @return integer the current 'timetype' value of the formatter
	  */
         public function getTimeType() {}


	 /**
	  * Gets the leniency in use 
	  * @return boolean   'true' if parser is lenient, 'false' if parser is strict
	  *                   default value for parser is 'false'.
	  */
         public function isLenient() {}


	 /**
	  * Sets the leniency to use 
	  * @param boolean $lenient  sets whether the parser is lenient or not, default is 'false'
          *                          'true' sets the parser to accept otherwise flawed date or 
	  *                          time patterns, parsing as much as possible to obtain a value.
          *                          'false' sets the parser to strictly parse strings into dates. 
	  *                          Extra space, unrecognized tokens, or invalid values 
	  *                          ("Feburary 30th") are not accepted.
	  *
          * @return boolean          'true' if successful; 'false' if an error occurred. 
	  */
	 public function setLenient($lenient) {}


	 /**
	  * Gets the locale in use 
	  * @param  [integer]  which locale should be returned? 
	  *                    values may include ULOC_ACTUAL_LOCALE,
	  *                    ULOC_VALID_LOCALE. By default the actual
	  *                    locale is returned.
	  * @return string     the locale of this formatter  or 'false' if error
	 */

	 public function getLocale($type = ULOC_ACTUAL_LOCALE) {}


         /**
	  * @return string ID string for the time zone used by this formatter
	  */
	 public function getTimeZoneId() {}


	/**
	 * sets the time zone to use
	 * @param string $zone zone ID string of the time zone to use.
	 *                     if null or the empty string, the default time zone for
	 *                     the runtime is used.
	 * @return boolean 'true' on successful setting of the time zone, 'false'
	 *                 if an error occurred (such as the time zone wasn't recognized).
	 */
        public function setTimeZoneId($zone) {}


        /**
         * Sets the calendar used to the appropriate calendar, which must be 
	 * one of the constants defined above. Some examples include:
         *   - Gregorian calendar
         *   - Traditional
         * Default value is GREGORIAN
	 * @param integer $which the calendar (an enumerated constant) to use.
         * @return boolean 'true' if successful, 'false' if an error occurred or if the calendar was not recognized 
	 */
         public function setCalendar($which) {}


        /**
	 * Gets the Calendar in use 
	 * @return integer the calendar being used by the formatter
	 */
         public function getCalendar() {}


        /**
	 * Gets the pattern in use 
    	 * @return string the pattern string being used to format/parse 
	 */
	public function getPattern() {}


        /**
	 * Sets the pattern to  use 
         * @param  string $pattern new pattern string to use
         * @return boolean 'true' if successful, 'false' if an error occured. Bad format
         *                 strings are usually the cause of the latter.
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


}

#############################################################################
# Procedural API
#############################################################################


	/**
	 * Create a date formatter 
	 *
	 * @param string  $locale     Locale to use when formatting or parsing
	 * @param integer $datetype   Date type to use (none, short, medium, long, full)
	 * @param integer $timetype   Time type to use (none, short, medium, long, full)
	 * @param [string] $timezone  Time zone ID ; default is system default
	 * @param [integer] $calendar Calendar to use for formatting or parsing; default is
	 *                            GREGORIAN
	 * @param [string]  $pattern  Optional pattern to use when formatting or parsing
	 * @return DateFormatter
	 * @see datefmt_create
	 */
	function datefmt_create($locale, $datetype, $timetype, $timezone = null, $calendar= null ,$pattern=null ) {}


	/**
         * formats the time value as a string.
         * @param DateFormatter $fmt The date formatter resource
	 * @param mixed $value - value to format
         *     integer: a unix timestamp value (seconds since epoch, UTC)
         *     array: a localtime array   - uses 24 hour clock in tm_hour field
         * @return string  a formatted string or, if an error occurred, 'null'. 
	 */
        function datefmt_format($fmt , $value) {}


        /**
         * converts string $value to an incremental time value, starting at
	 * $parse_pos and consuming as much of the input value as possible
	 * If no error occurs before $value is consumed, $parse_pos will contain -1
	 * otherwise it will contain the position at which parsing ended (and the error
	 * occurred). 
         * @param DateFormatter $fmt The date formatter resource
	 * @param string  $value      string to convert to a time
	 * @param integer $parse_pos  position at which to start the parsing in $value (zero-based)
	 *                            This variable will contain the end position if the parse fails
	 *                            If $parse_pos > strlen($value), the parse fails immediately.
	 * @return integer timestamp  parsed value
	 */
	 function datefmt_parse($fmt , $value, $parse_pos=0) {}

	 
        /**
         * converts string $value to a field-based time value, starting at
	 * $parse_pos and consuming as much of the input value as possible
	 * If no error occurs before $value is consumed, $parse_pos will contain -1
	 * otherwise it will contain the position at which parsing ended (and the error
	 * occurred). 
         * @param DateFormatter $fmt The date formatter resource
	 * @param string  $value      string to convert to a time
	 * @param integer $parse_pos  position at which to start the parsing in $value (zero-based)
	 *                            This variable will contain the end position if the parse fails
	 *                            If $parse_pos > strlen($value), the parse fails immediately.
	 * @return array localtime compatible array of integers  - uses 24 hour clock in tm_hour field
	 */
	 function datefmt_localtime($fmt , $value, $parse_pos=0) {}


	 /**
	  * Gets the  Datetype in use
          * @param DateFormatter $fmt The date formatter resource
	  * @return integer the current 'datetype' value of the formatter
	  */
         function datefmt_get_datetype($fmt ) {}


	 /**
	  * Gets the  timetype in use
          * @param DateFormatter $fmt The date formatter resource
	  * @return integer the current 'timetype' value of the formatter
	  */
         function datefmt_get_timetype($fmt) {}


	 /**
	  * Gets the leniency of the formatter 
          * @param DateFormatter $fmt The date formatter resource
	  * @return boolean   'true' if parser is lenient, 'false' if parser is strict
	  *                   default value for parser is 'false'.
	  */
         function datefmt_is_lenient($fmt) {}


	 /**
	  * Sets the leniency of the formatter 
          * @param DateFormatter $fmt The date formatter resource
	  * @param boolean $lenient  sets whether the parser is lenient or not, default is 'false'
          *                          'true' sets the parser to accept otherwise flawed date or 
	  *                          time patterns, parsing as much as possible to obtain a value.
          *                          'false' sets the parser to strictly parse strings into dates. 
	  *                          Extra space, unrecognized tokens, or invalid values 
	  *                          ("Feburary 30th") are not accepted.
	  *
          * @return boolean          'true' if successful; 'false' if an error occurred. 
	  */
	 function datefmt_set_lenient($fmt , $lenient) {}


	 /**
	  * Gets the locale in  use
          * @param DateFormatter $fmt The date formatter resource
	  * @param  [integer]  which locale should be returned? 
	  *                    values may include ULOC_ACTUAL_LOCALE,
	  *                    ULOC_VALID_LOCALE. By default the actual
	  *                    locale is returned.
	  * @return string     the locale of this formatter  or 'false' if error
	 */
	 function datefmt_get_locale($fmt , $type = ULOC_ACTUAL_LOCALE) {}


         /**
	  * Gets the time zone id in  use
          * @param DateFormatter $fmt The date formatter resource
	  * @return string ID string for the time zone used by this formatter
	  */
	 function datefmt_get_timezone_id($fmt) {}


	/**
	 * Sets the time zone to use
         * @param DateFormatter $fmt The date formatter resource
	 * @param string $zone zone ID string of the time zone to use.
	 *                     if null or the empty string, the default time zone for
	 *                     the runtime is used.
	 * @return boolean 'true' on successful setting of the time zone, 'false'
	 *                 if an error occurred (such as the time zone wasn't recognized).
	 */
        function datefmt_set_timezone_id($fmt , $zone) {}


        /**
         * Sets the calendar used to the appropriate calendar, which must be 
	 * one of the constants defined above. Some examples include:
         *   - Gregorian calendar
         *   - Traditional
         * Default value is GREGORIAN
         * @param DateFormatter $fmt The date formatter resource
	 * @param integer $which the calendar (an enumerated constant) to use.
         * @return boolean 'true' if successful, 'false' if an error occurred or if the calendar was not recognized 
	 */
         function datefmt_set_calendar($fmt , $which) {}


        /**
	 * Gets the calendar in use
         * @param DateFormatter $fmt The date formatter resource
	 * @return integer the calendar being used by the formatter
	 */
         function datefmt_get_calendar($fmt) {}


        /**
	 * Gets the pattern in use
         * @param DateFormatter $fmt The date formatter resource
         * @return string the pattern string being used to format/parse
         */
        function  datefmt_get_pattern($fmt) {}


        /**
	 * Sets the pattern to use
         * @param DateFormatter $fmt The date formatter resource
         * @param  string $pattern new pattern string to use
         * @return boolean 'true' if successful, 'false' if an error occured. Bad format
         *                 strings are usually the cause of the latter.
         */
        function datefmt_set_pattern($fmt , $pattern) {}


	/**
	 * Get the error code from last operation
	 *
         * @param DateFormatter $fmt The date formatter resource
	 * Returns error code from the last number formatting operation.
	 *
	 * @return integer the error code, one of UErrorCode values. Initial value is U_ZERO_ERROR.
	 */
	function datefmt_get_error_code($fmt) {}


	/**
	 * Get the error text from the last operation.
	 *
         * @param DateFormatter $fmt The date formatter resource
	 * @return string Description of the last error.
	 */
	function datefmt_get_error_message($fmt) {}
