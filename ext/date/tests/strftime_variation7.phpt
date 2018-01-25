--TEST--
Test strftime() function : usage variation - Passing day related format strings to format argument.
--FILE--
<?php
/* Prototype  : string strftime(string format [, int timestamp])
 * Description: Format a local time/date according to locale settings
 * Source code: ext/date/php_date.c
 * Alias to functions:
 */

echo "*** Testing strftime() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
setlocale(LC_ALL, "en_US");
date_default_timezone_set("Asia/Calcutta");
$timestamp = mktime(18, 8, 8, 8, 8, 2008);


//array of values to iterate over
$inputs = array(
	  'Day of the month as a decimal number' => "%d",
	  'Day of the year as a decimal number' => "%j",
	  'Day of the week as a decimal number' => "%w"
);

// loop through each element of the array for timestamp

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( strftime($value) );
      var_dump( strftime($value, $timestamp) );
};

?>
===DONE===
--EXPECTF--
*** Testing strftime() : usage variation ***

--Day of the month as a decimal number--
string(%d) "%d"
string(2) "08"

--Day of the year as a decimal number--
string(%d) "%d"
string(3) "221"

--Day of the week as a decimal number--
string(%d) "%d"
string(1) "5"
===DONE===
