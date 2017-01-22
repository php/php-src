--TEST--
Test timezone_transitions_get() function : basic functionality 
--FILE--
<?php
/* Prototype  : array timezone_transitions_get  ( DateTimeZone $object, [ int $timestamp_begin  [, int $timestamp_end  ]]  )
 * Description: Returns all transitions for the timezone
 * Source code: ext/date/php_date.c
 * Alias to functions: DateTimeZone::getTransitions()
 */

echo "*** Testing timezone_transitions_get() : basic functionality ***\n";

//Set the default time zone 
date_default_timezone_set("Europe/London");

// Create a DateTimeZone object
$tz = timezone_open("Europe/London");

$tran = timezone_transitions_get($tz);

echo "\n-- Get all 60s transitions --\n";
$tran = timezone_transitions_get($tz, -306972000, -37241999);
var_dump( gettype($tran) );

echo "\n-- Total number of transitions: " . count($tran). " --\n"; 

echo "\n-- Format a sample entry pfor Spring 1963 --\n";
var_dump( $tran[6] );

?>
===DONE===
--EXPECT--
*** Testing timezone_transitions_get() : basic functionality ***

-- Get all 60s transitions --
string(5) "array"

-- Total number of transitions: 18 --

-- Format a sample entry pfor Spring 1963 --
array(5) {
  ["ts"]=>
  int(-213228000)
  ["time"]=>
  string(24) "1963-03-31T02:00:00+0000"
  ["offset"]=>
  int(3600)
  ["isdst"]=>
  bool(true)
  ["abbr"]=>
  string(3) "BST"
}
===DONE===