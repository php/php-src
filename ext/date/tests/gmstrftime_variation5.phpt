--TEST--
Test gmstrftime() function : usage variation - Passing date related format strings to format argument.
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
setlocale(LC_ALL, "en_US");
date_default_timezone_set("Asia/Calcutta");


//array of values to iterate over
$inputs = array(
	  'Year as decimal number without a century' => "%y",
	  'Year as decimal number including the century' => "%Y",
	  'Time zone offset' => "%Z",
	  'Time zone offset' => "%z",
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

--Year as decimal number without a century--
string(%d) "%d"
string(2) "08"

--Year as decimal number including the century--
string(%d) "%d"
string(4) "2008"

--Time zone offset--
string(%s) "%s"
string(%s) "%s"
===DONE===
