--TEST--
Test gmdate() function : usage variation - Passing predefined constants to format argument.
--FILE--
<?php
/* Prototype  : string gmdate(string format [, long timestamp])
 * Description: Format a GMT date/time 
 * Source code: ext/date/php_date.c
 */

echo "*** Testing gmdate() : usage variation ***\n";

// Initialise all required variables
date_default_timezone_set('UTC');
$timestamp = mktime(8, 8, 8, 8, 8, 2008);

//array of values to iterate over
$inputs = array(
      // Predefined Date constants
      'DATE_ATOM Constant' => DATE_ATOM,
	  'DATE_COOKIE Constant' => DATE_COOKIE,
	  'DATE_RFC822 Constant' => DATE_RFC822,
	  'DATE_RFC850 Constant' => DATE_RFC850,
	  'DATE_RFC1036 Constant' => DATE_RFC1036,
	  'DATE_RFC1123 Constant' => DATE_RFC1123,
	  'DATE_RFC2822 Constant' => DATE_RFC2822,
	  'DATE_RFC3339 Constant' => DATE_RFC3339,
	  'DATE_RSS Constant' => DATE_RSS,
	  'DATE_W3C Constant' => DATE_W3C,
);

// loop through each element of the array for format
foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
	  var_dump( gmdate($value, $timestamp) );
	  var_dump( gmdate($value) );
};

?>
===DONE===
--EXPECTF--
*** Testing gmdate() : usage variation ***

--DATE_ATOM Constant--
string(25) "2008-08-08T08:08:08+00:00"
string(%d) "%s"

--DATE_COOKIE Constant--
string(30) "Friday, 08-Aug-08 08:08:08 GMT"
string(%d) "%s"

--DATE_RFC822 Constant--
string(29) "Fri, 08 Aug 08 08:08:08 +0000"
string(%d) "%s"

--DATE_RFC850 Constant--
string(30) "Friday, 08-Aug-08 08:08:08 GMT"
string(%d) "%s"

--DATE_RFC1036 Constant--
string(29) "Fri, 08 Aug 08 08:08:08 +0000"
string(%d) "%s"

--DATE_RFC1123 Constant--
string(31) "Fri, 08 Aug 2008 08:08:08 +0000"
string(%d) "%s"

--DATE_RFC2822 Constant--
string(31) "Fri, 08 Aug 2008 08:08:08 +0000"
string(%d) "%s"

--DATE_RFC3339 Constant--
string(25) "2008-08-08T08:08:08+00:00"
string(%d) "%s"

--DATE_RSS Constant--
string(31) "Fri, 08 Aug 2008 08:08:08 +0000"
string(%d) "%s"

--DATE_W3C Constant--
string(25) "2008-08-08T08:08:08+00:00"
string(%d) "%s"
===DONE===