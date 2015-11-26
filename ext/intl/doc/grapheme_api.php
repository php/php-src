<?php
#############################################################################
# Grapheme constants.
#############################################################################

	/**
	 * grapheme_extract extract_type
	 *
	*/
	/** Extract the given number of whole grapheme clusters from the string: */
	define('GRAPHEME_EXTR_COUNT', 0);
	/** Extract as many whole grapheme clusters as will fit into the given number of bytes: */
	define('GRAPHEME_EXTR_MAXBYTES', 1);
	/** Extract whole grapheme clusters up to a maximum number of UTF-8 characters: */
	define('GRAPHEME_EXTR_MAXCHARS', 2);


#############################################################################
# Grapheme API
#############################################################################

	/**
	 * Get string length in grapheme units
	 * @param  string	$input		The string being measured for length.
	 * @return int		The length of the string on success, and 0 if the string is empty.
	*/
	function grapheme_strlen($input) {}

	/**
	 * Find position (in grapheme units) of first occurrence of a string
	 * @param string	$haystack	The string to look in
	 * @param string	$needle		The string to look for
	 * @param [int]		$offset		The optional offset parameter allows you to specify 
						which character in haystack  to start searching. The position 
						returned is still relative to the beginning of haystack.
	 * @return int		Returns the position as an integer. If needle is not found, strpos() will return boolean FALSE.
	*/
	function grapheme_strpos($haystack, $needle, $offset = 0) {}

	
	/**
         * Find position (in grapheme units) of first occurrence of a case-insensitive string
         * @param string        $haystack       The string to look in
         * @param string        $needle         The string to look for
         * @param [int]         $offset         The optional offset parameter allows you to specify
                                                which character in haystack  to start searching. The position
                                                returned is still relative to the beginning of haystack.
         * @return int          Returns the position as an integer. If needle is not found, grapheme_stripos() will return boolean FALSE.
        */
        function grapheme_stripos($haystack, $needle, $offset = 0) {}
 

	/**
         * Find position (in grapheme units) of last occurrence of a string
         * @param string        $haystack       The string to look in
         * @param string        $needle         The string to look for
         * @param [int]         $offset         The optional offset parameter allows you to specify
                                                which character in haystack  to start searching. The position
                                                returned is still relative to the beginning of haystack.
         * @return int          Returns the position as an integer. If needle is not found, grapheme_strrpos() will return boolean FALSE.
        */
        function grapheme_strrpos($haystack, $needle, $offset = 0) {}


	/**
         * Find position (in grapheme units) of last occurrence of a case-insensitive string
         * @param string        $haystack       The string to look in
         * @param string        $needle         The string to look for
         * @param [int]         $offset         The optional offset parameter allows you to specify
                                                which character in haystack  to start searching. The position
                                                returned is still relative to the beginning of haystack.
         * @return int          Returns the position as an integer. If needle is not found, grapheme_strripos() will return boolean FALSE.
        */
        function grapheme_strripos($haystack, $needle, $offset = 0) {}


	/**
	 * Return part of a string
	 * @param string	$string		The input string.
	 * @param int		$start		If start  is non-negative, the returned string will start at the 
						start'th position in string, counting from zero. If start is negative,
						the returned string will start at the start'th character from the 
						end of string.
	 * @param [int]		$length		If length  is given and is positive, the string returned will contain
						at most length characters beginning from start (depending on the 
						length of string). If string is less than or equal to start characters
						long, FALSE  will be returned. If length is given and is negative, then
						that many characters will be omitted from the end of string (after the
						start position has been calculated when a start is negative). If start
						denotes a position beyond this truncation, an empty string will be returned.
	 * @return int		Returns the extracted part of string.
	*/
	function grapheme_substr($string, $start, $length = -1) {}


	/**
	 * Returns part of haystack string from the first occurrence of needle to the end of haystack.
	 * @param string	$haystack	The input string.
	 * @param string	$needle		The string to look for.
	 * @param [boolean]	$before_needle	If TRUE (the default is FALSE), grapheme_strstr() returns the part of the
						haystack before the first occurrence of the needle.
	 * @return string	Returns the portion of string, or FALSE if needle is not found.
	*/
	function grapheme_strstr($haystack, $needle, $before_needle = FALSE) {}


	/**
	 * Returns part of haystack string from the first occurrence of case-insensitive needle to the end of haystack.
         * @param string        $haystack       The input string.
         * @param string        $needle         The string to look for.
         * @param [boolean]     $before_needle  If TRUE (the default is FALSE), grapheme_strstr() returns the part of the
                                                haystack before the first occurrence of the needle.
         * @return string       Returns the portion of string, or FALSE if needle is not found.
        */
        function grapheme_stristr($haystack, $needle, $before_needle = FALSE) {}


	/**
	 * Function to extract a sequence of default grapheme clusters from a text buffer, which must be encoded in UTF-8.
	 * @param string 	$haystack	string to search
	 * @param int		$size		maximum number of units - based on the $extract_type - to return
	 * @param [int]		$extract_type	one of GRAPHEME_EXTR_COUNT (default), GRAPHEME_EXTR_MAXBYTES, or GRAPHEME_EXTR_MAXCHARS
	 * @param [int]		$start		starting position in $haystack in bytes
	 * @param [&int]	$next		set to next starting position in bytes
	 * @return string	A string starting at offset $start containing no more than $size grapheme clusters 
				and ending on a default grapheme cluster boundary.
	*/
	function grapheme_extract($haystack, $size, $extract_type = GRAPHEME_EXTR_COUNT, $start = 0, &$next) {}

?>


