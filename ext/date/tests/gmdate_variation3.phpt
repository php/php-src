--TEST--
Test gmdate() function : usage variation - Passing numeric representation of day formats.
--FILE--
<?php
/* Prototype  : string gmdate(string format [, long timestamp])
 * Description: Format a GMT date/time 
 * Source code: ext/date/php_date.c
 * Alias to functions: 
 */

echo "*** Testing gmdate() : usage variation ***\n";

// Initialise all required variables
date_default_timezone_set('UTC');
$timestamp = mktime(8, 8, 8, 8, 8, 2008);

//array of values to iterate over
$inputs = array(

	 'Day with leading zeros' => 'd',
	 'Day without leading zeros' => 'j',
	 'ISO representation' => 'N',
	 'Numeric representation of day' => 'w',
	 'Day of the year' => 'z'
);

// loop through each element of the array for timestamp

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( gmdate($value) );
      var_dump( gmdate($value, $timestamp) );
};

?>
===DONE===
--EXPECTF--
*** Testing gmdate() : usage variation ***

--Day with leading zeros--
unicode(%d) "%d"
unicode(2) "08"

--Day without leading zeros--
unicode(%d) "%d"
unicode(1) "8"

--ISO representation--
unicode(%d) "%d"
unicode(1) "5"

--Numeric representation of day--
unicode(%d) "%d"
unicode(1) "5"

--Day of the year--
unicode(%d) "%d"
unicode(3) "220"
===DONE===
