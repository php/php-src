--TEST--
Test gmstrftime() function : usage variation - Passing day related format strings to format argument.
--FILE--
<?php
/* Prototype  : string gmstrftime(string format [, int timestamp])
 * Description: Format a GMT/UCT time/date according to locale settings 
 * Source code: ext/date/php_date.c
 * Alias to functions: 
 */

echo "*** Testing gmstrftime() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$timestamp = gmmktime(8, 8, 8, 8, 8, 2008);
locale_set_default("en_US");
date_default_timezone_set("Asia/Calcutta");

//array of values to iterate over
$inputs = array(
	  'Day of the month as decimal number' => "%d",
	  'Day of the year as a decimal number' => "%j",
	  'Day of the week as a decimal number' => "%w"
);

// loop through each element of the array for timestamp

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( gmstrftime($value) );
      var_dump( gmstrftime($value, $timestamp) );
};

?>
===DONE===
--EXPECTF--
*** Testing gmstrftime() : usage variation ***

--Day of the month as decimal number--
unicode(%d) "%d"
unicode(2) "08"

--Day of the year as a decimal number--
unicode(%d) "%d"
unicode(3) "221"

--Day of the week as a decimal number--
unicode(%d) "%d"
unicode(1) "5"
===DONE===
