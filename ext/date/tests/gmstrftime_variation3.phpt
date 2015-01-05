--TEST--
Test gmstrftime() function : usage variation - Passing week related format strings to format argument.
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

//array of values to iterate over
$inputs = array(
      'Abbreviated weekday name' => "%a",
      'Full weekday name' => "%A",
	  'Week number of the year' => "%U",
	  'Week number of the year in decimal number' => "%W",	
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

--Abbreviated weekday name--
string(%d) "%s"
string(3) "Fri"

--Full weekday name--
string(%d) "%s"
string(6) "Friday"

--Week number of the year--
string(%d) "%d"
string(2) "31"

--Week number of the year in decimal number--
string(%d) "%d"
string(2) "31"
===DONE===
